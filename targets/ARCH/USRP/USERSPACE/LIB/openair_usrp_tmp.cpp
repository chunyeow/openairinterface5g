#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>
#include <fstream>
#include <cmath>
#include "def.h"

// --------------------------------
// headers for polyphaseResample
// --------------------------------
#include "polyphaseResample.h"
#include "coeff_3072_to_25.h"
#include "coeff_25_to_3072.h"

using namespace std;
using std::ofstream;
using std::ifstream;

// --------------------------------
// contant variables
// --------------------------------
#define PI 3.1415926535898
#define SAM_RATE 6.25e6 // 6.25e6 = 100e6/16
#define DL_FREQ 880e6
#define UL_FREQ 835e6

// --------------------------------
// variables for polyphaseResample
// --------------------------------
const int OUTRATE = 625;
const int INRATE = 768;
int *g_branch_table;
int *g_offset_table;
short Rx_history[625*2];
short Tx_history[768*2];

// --------------------------------
// variables for USRP configuration
// --------------------------------
uhd::usrp::multi_usrp::sptr tx_usrp;
uhd::usrp::multi_usrp::sptr rx_usrp;

//create a send streamer and a receive streamer
uhd::stream_args_t stream_args_short("sc16");//complex short
uhd::stream_args_t stream_args_float("fc32");//complex float
uhd::tx_streamer::sptr tx_stream;
uhd::rx_streamer::sptr rx_stream;

uhd::tx_metadata_t tx_md;
uhd::rx_metadata_t rx_md;

//setup variables and allocate buffer
uhd::async_metadata_t async_md;

double tx_timeout;
double rx_timeout;

double rx_freq;

uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);

// --------------------------------
// variables for OAI buffers
// --------------------------------
int oai_tx_buff[SAMPLES_PER_FRM];
int oai_rx_buff[SAMPLES_PER_FRM];
int oai_rx_buff_temp[SAMPLES_PER_FRM_USRP];
int oai_tx_buff_temp[SAMPLES_PER_FRM_USRP];
volatile unsigned long long tx_timestamp,rx_timestamp,clock_usrp,tmp_clock;
volatile int w_pos_usrp_rcv = 0; // value range [0:76800-1]
volatile int r_pos_usrp_send = 0; // value range [0:76800-1]
volatile int w_slot_usrp_rcv, r_slot_usrp_send; // slot index for USRP thread. value range [0:19]
volatile int r_slot_idx, t_slot_idx; // slot index for eNB thread. value range [0;19]
volatile int send_slot_missed = 0;

// --------------------------------
// Debug and output control
// --------------------------------
int g_usrp_debug=0;
int num_underflows=0;
int num_overflows=0;
int num_seq_errors=0;


extern "C"
{
	void UHD_init(void);
	void UHD_init_display(void);
	void Init_send(void);
	void Init_recv(void);
	void send_data(int hw_slot_offset);
	void recv_data(int hw_slot_offset);
	void send_end(void);
	void set_freq(int freq_offset);
	char UE_flag;
	void tx_errorcode_handler(void);
	void sig_int_handler(void);
}
void Rx_resample(void);
void Tx_resample(void);

// --------------------------------
// USRP initial 
// --------------------------------
void UHD_init(void)
{
	uhd::set_thread_priority_safe();
	std::string args = "send_frame_size=3840,recv_frame_size=3840";
	uhd::device_addrs_t device_addr = uhd::device::find(args);
	if (device_addr.size() == 0 )
	{
		std::cerr<<"No USRP Device Found"<<std::endl;
		exit(1);
	}	
	else
		std::cout<<"USRP Device Found"<<std::endl<<device_addr[0].to_pp_string()<<std::endl;
	tx_usrp = uhd::usrp::multi_usrp::make(args);
	rx_usrp = uhd::usrp::multi_usrp::make(args);

	tx_stream = tx_usrp->get_tx_stream(stream_args_short);
	rx_stream = rx_usrp->get_rx_stream(stream_args_short);

	//Lock mboard clocks
	tx_usrp->set_clock_source("internal");
	rx_usrp->set_clock_source("internal");

	tx_usrp->set_tx_rate(SAM_RATE);
	if(UE_flag)
		tx_usrp->set_tx_freq(UL_FREQ);
	else
		tx_usrp->set_tx_freq(DL_FREQ);
	tx_usrp->set_tx_gain(0);
	tx_usrp->set_tx_bandwidth(25e6);
	tx_usrp->set_tx_antenna("TX/RX");

	rx_usrp->set_rx_rate(SAM_RATE);
	if(UE_flag)
	{
		rx_usrp->set_rx_freq(DL_FREQ);
		rx_freq = DL_FREQ;
	}
	else
	{
		rx_usrp->set_rx_freq(UL_FREQ);
		rx_freq = UL_FREQ;
	}
	rx_usrp->set_rx_gain(0);
	rx_usrp->set_rx_bandwidth(25e6);
	rx_usrp->set_rx_antenna("RX2");

	std::cout <<std::endl<< boost::format("Setting device timestamp to 0...") << std::endl;
	tx_usrp->set_time_now(uhd::time_spec_t(0.0));
	rx_usrp->set_time_now(uhd::time_spec_t(0.0));
}

// --------------------------------
// USRP information display
// --------------------------------
void UHD_init_display(void)
{
	std::cout << std::endl<<boost::format("Actual TX sample rate: %fMSps...") % (tx_usrp->get_tx_rate()/1e6) << std::endl;
	std::cout << boost::format("Actual RX sample rate: %fMSps...") % (rx_usrp->get_rx_rate()/1e6) << std::endl;

	std::cout << boost::format("Actual TX frequency: %fGHz...") % (tx_usrp->get_tx_freq()/1e9) << std::endl;
	std::cout << boost::format("Actual RX frequency: %fGHz...") % (rx_usrp->get_rx_freq()/1e9) << std::endl;

	std::cout << boost::format("Actual TX gain: %f...") % (tx_usrp->get_tx_gain()) << std::endl;
	std::cout << boost::format("Actual RX gain: %f...") % (rx_usrp->get_rx_gain()) << std::endl;

	std::cout << boost::format("Actual TX bandwidth: %fM...") % (tx_usrp->get_tx_bandwidth()/1e6) << std::endl;
	std::cout << boost::format("Actual RX bandwidth: %fM...") % (rx_usrp->get_rx_bandwidth()/1e6) << std::endl;

	std::cout << boost::format("Actual TX antenna: %s...") % (tx_usrp->get_tx_antenna()) << std::endl;
	std::cout << boost::format("Actual RX antenna: %s...") % (rx_usrp->get_rx_antenna()) << std::endl;

	std::cout << boost::format("Device TX timestamp: %f...") % (tx_usrp->get_time_now().get_real_secs()) << std::endl;
	std::cout << boost::format("Device RX timestamp: %f...") % (rx_usrp->get_time_now().get_real_secs()) << std::endl << std::endl;
}

// -----------------------
//send init
// -----------------------
void Init_send(void)
{
	//reset send buffer
	memset(oai_tx_buff , 0 , sizeof(oai_tx_buff));

	//reset Tx_history
	memset(Tx_history, 0, sizeof(Tx_history));

	// the first slot sent shoud be the 0+N_slot_offset. 
	r_slot_usrp_send = N_slot_offset; 
	//send constantly
	tx_md.start_of_burst = false;
	tx_md.end_of_burst = false;
	tx_md.has_time_spec = true;
	tx_timestamp = T_start + r_slot_usrp_send * SAMPLES_PER_SLOT;
	tx_md.time_spec = uhd::time_spec_t::from_ticks(tx_timestamp,SAM_RATE);
	tx_timeout = tx_timestamp/SAM_RATE + 0.1;
}

// -----------------------
//receive init
// -----------------------
void Init_recv(void)
{
	//reset receive buffer
	memset(oai_rx_buff , 0 , sizeof(oai_rx_buff));

	//reset Rx_history
	memset(Rx_history, 0, sizeof(Rx_history));

	// initialize the rx stream
	stream_cmd.stream_now = false;//When true, the device will begin streaming ASAP. When false, the device will begin streaming at a time specified by time_spec. 
	stream_cmd.num_samps = 0;
	rx_timestamp = T_start + HW_offset;//Device timestamp and time of send sample 
	stream_cmd.time_spec = uhd::time_spec_t::from_ticks(rx_timestamp,SAM_RATE);
	rx_usrp->issue_stream_cmd(stream_cmd);

	//the first call to recv() will block this many seconds before receiving
	rx_timeout = rx_timestamp/SAM_RATE + 0.1; //timeout (delay before receive + padding)

	// the first clock number = rx_timestamp
	clock_usrp = rx_timestamp;
	w_pos_usrp_rcv = 0;
	w_slot_usrp_rcv = 19;
}

// -----------------------
// send function
// -----------------------
void send_data(int hw_slot_offset)
{

	int diff;
	// Handle the frame wrap-arround
	if ((r_slot_usrp_send<4)&&(t_slot_idx>=16)){
		diff = t_slot_idx - (r_slot_usrp_send + 20) ;
	}
	else if((r_slot_usrp_send>=16)&&(t_slot_idx<4)){
		diff = (t_slot_idx + 20) - r_slot_usrp_send;
	}
	else{
		diff = t_slot_idx - r_slot_usrp_send;
	}
	format_printf(g_usrp_debug, "[send] diff: %d\n",diff);
	if (diff<0){}
	else if (diff>1)
	{
		format_printf(g_usrp_debug, "Sending is late\n");
		r_slot_usrp_send = t_slot_idx;
		// count the missed sending slots
		send_slot_missed += (diff - 1);
	}
	else // diff = 0,1
	{
		for (int ii=0;ii<diff+1;ii++)
		{

			format_printf(g_usrp_debug,"[send] r_usrp: %d t_slot: %d w_usrp: %d w_pos_usrp_rcv: %d\n", r_slot_usrp_send,t_slot_idx,w_slot_usrp_rcv, w_pos_usrp_rcv);
			format_printf(g_usrp_debug,"[send] clock_usrp: %llu tx_timestamp: %llu\n", clock_usrp, tx_timestamp);

			Tx_resample();

			// sending
			size_t num_tx_samps;
			r_pos_usrp_send = (t_slot_idx * SAMPLES_PER_SLOT + hw_slot_offset) % SAMPLES_PER_FRM;
			if((r_pos_usrp_send + SAMPLES_PER_SLOT) <= SAMPLES_PER_FRM)
			{
				memcpy((& oai_tx_buff_temp[0]) , (& oai_tx_buff[r_pos_usrp_send]) , SAMPLES_PER_SLOT*4 );
				//num_tx_samps = tx_stream->send((& oai_tx_buff[r_pos_usrp_send]), SAMPLES_PER_SLOT_USRP, tx_md, tx_timeout);
			}
			else
			{
				int tmp_len = SAMPLES_PER_FRM - r_pos_usrp_send;
				//num_tx_samps = tx_stream->send((& oai_tx_buff[r_pos_usrp_send]), tmp_len, tx_md, tx_timeout);
				memcpy((& oai_tx_buff_temp[0]) , (& oai_tx_buff[r_pos_usrp_send]) , tmp_len*4 );
				//num_tx_samps = tx_stream->send((& oai_tx_buff[0]),SAMPLES_PER_SLOT_USRP - tmp_len, tx_md, tx_timeout);
				memcpy((& oai_tx_buff_temp[tmp_len]) , (& oai_tx_buff[0]) , (SAMPLES_PER_SLOT - tmp_len)*4 );
			}
	
			num_tx_samps = tx_stream->send((& oai_tx_buff_temp[0]), SAMPLES_PER_SLOT_USRP, tx_md, tx_timeout);

			//without timestamp
			tx_md.has_time_spec = false;

			tx_timeout = 0.1;

			r_slot_usrp_send++;
			if(r_slot_usrp_send==20) r_slot_usrp_send=0;

		}
	}
}

// --------------------------------
// handle TX error codes
// --------------------------------
void tx_errorcode_handler(void){
	if (not tx_stream->recv_async_msg(async_md)) {}
	else{
		switch(async_md.event_code){
			case uhd::async_metadata_t::EVENT_CODE_BURST_ACK:
				return;
			case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW:
				format_printf(g_usrp_debug,"[send] USRP TX UNDERFLOW!\n");
				num_underflows++;
				break;
			case uhd::async_metadata_t::EVENT_CODE_UNDERFLOW_IN_PACKET:
				format_printf(g_usrp_debug,"[send] USRP TX UNDERFLOW IN_PACKET!\n");
				break;
			case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR:
			case uhd::async_metadata_t::EVENT_CODE_SEQ_ERROR_IN_BURST:
				num_seq_errors++;
				break;
			default:
				//std::cerr << "Event code: " << async_md.event_code << std::endl;
				//std::cerr << "Unexpected event on async recv, continuing..." << std::endl;
				break;
		}
	}
}
// -----------------------
// send end of burst
// -----------------------
void send_end()
{
	//send a mini EOB packet
	tx_md.end_of_burst = true;
	tx_stream->send("", 0, tx_md);
	tx_md.end_of_burst = false;
	format_printf(1,"num_underflows: %d   num_overflows: %d   num_seq_errors: %d\n",num_underflows,num_overflows,num_seq_errors);
}

// -----------------------
// receive function
// -----------------------
void recv_data(int hw_slot_offset)
{
	size_t num_rx_samps;

	//volatile unsigned long long tmp_clock;
	num_rx_samps = rx_stream->recv((&oai_rx_buff_temp[0]), SAMPLES_PER_SLOT_USRP,rx_md,rx_timeout);
	Rx_resample();

	//after the first receiving, reset the timeout value
	rx_timeout = 0.1; 
	tmp_clock = rx_md.time_spec.to_ticks(SAM_RATE);

	// Update writing position
	w_pos_usrp_rcv = (w_pos_usrp_rcv + (int)(tmp_clock - clock_usrp))%SAMPLES_PER_FRM;

	// Copy data into the PHY layer buffer
	if((w_pos_usrp_rcv+num_rx_samps) <= SAMPLES_PER_FRM){ 
		memcpy(& oai_rx_buff[w_pos_usrp_rcv],& oai_rx_buff_temp[0],num_rx_samps*sizeof(int));
	}
	else{
		int tmp_len = SAMPLES_PER_FRM - w_pos_usrp_rcv;
		memcpy(& oai_rx_buff[w_pos_usrp_rcv],& oai_rx_buff_temp[0],tmp_len*sizeof(int));
		memcpy(& oai_rx_buff[0],& oai_rx_buff_temp[tmp_len],(num_rx_samps-tmp_len)*sizeof(int));
	}

	w_slot_usrp_rcv = (w_pos_usrp_rcv - hw_slot_offset + num_rx_samps - SAMPLES_PER_SLOT)%SAMPLES_PER_FRM / SAMPLES_PER_SLOT;

	// update clock
	clock_usrp = tmp_clock;

	// show log
	format_printf(g_usrp_debug,"[recv] w_pos: %d num_rx_samps=%d clock_usrp=%llu w_usrp: %d\n",w_pos_usrp_rcv,num_rx_samps,clock_usrp,w_slot_usrp_rcv);

	//handle the error code
	switch(rx_md.error_code){
		case uhd::rx_metadata_t::ERROR_CODE_NONE:
			break;
		case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW:
			format_printf(g_usrp_debug,"[recv] USRP RX OVERFLOW!\n");
			num_overflows++;
			break;
		case uhd::rx_metadata_t::ERROR_CODE_TIMEOUT:
			format_printf(g_usrp_debug, "[recv] USRP RX TIMEOUT!\n");
			break;
		default:
			format_printf(g_usrp_debug, "[recv] Unexpected error on RX, Error code: 0x%x\n",rx_md.error_code);    
			break;
	}
}

// -----------------------
// set RX frequency in UE
// -----------------------
void set_freq(int freq_offset)
{
	rx_usrp->set_rx_freq(rx_freq + freq_offset);
}

// -----------------------
// Rx_resample
// -----------------------
void Rx_resample(void)
{
	short Rx_data_in[6250+625*2];
	short Rx_data_out[7680+768*2];
	memset(Rx_data_in, 0, sizeof(Rx_data_in));
	memset(Rx_data_out, 0, sizeof(Rx_data_out));

	memcpy(Rx_data_in, Rx_history, 625*4);
	memcpy(Rx_data_in+625*2, & oai_rx_buff_temp[0], 3125*4);

	polyphaseResample(Rx_data_in, sizeof(Rx_data_in) / sizeof(short) / 2 ,Rx_data_out , sizeof(Rx_data_out) / sizeof(short) / 2 , coeff_25_to_3072, sizeof(coeff_25_to_3072) / sizeof(short), INRATE, OUTRATE, 13, g_branch_table, g_offset_table);
	memcpy(& oai_rx_buff_temp[0], Rx_data_out+768*2, 3840*4);
	memcpy(Rx_history, Rx_data_in+6250-625*2, 625*4);
}

// -----------------------
// Tx_resample
// -----------------------
void Tx_resample(void)
{
	short Tx_data_out[6250+625*2];
	short Tx_data_in[7680+768*2];
	memset(Tx_data_in, 0, sizeof(Tx_data_in));
	memset(Tx_data_out, 0, sizeof(Tx_data_out));

	memcpy(Tx_data_in, Tx_history, 768*4);
	memcpy(Tx_data_in+768*2, & oai_tx_buff_temp[0], 3840*4);

	polyphaseResample(Tx_data_in, sizeof(Tx_data_in) / sizeof(short) / 2 ,Tx_data_out , sizeof(Tx_data_out) / sizeof(short) / 2 , coeff_3072_to_25, sizeof(coeff_3072_to_25) / sizeof(short) , OUTRATE, INRATE, 11, g_branch_table, g_offset_table);
	memcpy((& oai_tx_buff_temp[0]) , Tx_data_out+625*2, 3125*4);
	memcpy(Tx_history, Tx_data_in+7680-768*2, 768*4);
}
