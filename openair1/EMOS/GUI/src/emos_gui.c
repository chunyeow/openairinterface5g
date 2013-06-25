#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <gps.h>

// Realtime includes
//#include <rtai.h>
#include <rtai_fifos.h>

// Xform includes
#include <forms.h>

// Openair includes
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#ifdef CBMIMO1
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#endif
#ifdef PLATON
#include "daq.h"
#endif

#include "PHY/CONFIG/vars.h"
#include "MAC_INTERFACE/vars.h"
//#include "PHY/TOOLS/defs.h"

// Program includes
#include "emos_gui.h"
#include "emos_gui_forms.h"
#include "matrix_util.h"


// Definition of the main form
FD_main_frm *main_frm;
Window main_wnd;
FD_config_dialog *config_frm;
Window config_wnd;

// Diverse variables and default values
int power = TERM_OFF;
int record = REC_OFF;
int record_multi = REC_OFF;
int terminal_mode = TERM_MODE_SINGL;
int file_index = 0;
int emos_ready = EMOS_READY;
int openair_dev_fd;
double screen_refresh_period = 1; // in seconds
//int screen_refresh_in_frames = 0; // in frames
int fifo_fd;
int frame_counter = 0;
int rec_frame_counter = 0;
FILE *dumpfile_id = NULL; 
char data_buffer[CHANNEL_BUFFER_SIZE];
int values_in_memory=0;
long long start_time;
int num_tx_ant = 2;
int num_ch = 2;
int terminal_idx = 0;
int pdu_errors[2] = {0,0};
int pdu_errors_last[2] = {0,0};
int domain_selector = TIME_DOMAIN;
int noise_selector = SNR;
int chsch_index;
int is_cluster_head = 0;
int node_id = 8;
int tx_gain = 0;
CHBCH_RX_t rxmode = ML;

unsigned char tx_gain_table_c[36] = {
  113, 111, 0, 0, //-20dBm
  119, 118, 0, 0, //-15dBm
  126, 125, 0, 0, //-10dBm
  133, 131, 0, 0, //-5dBm
  139, 138, 0, 0, //0dBm
  147, 145, 0, 0, //5dBm
  152, 151, 0, 0, //10dBm
  160, 158, 0, 0, //15dBm
  177, 173, 30, 17}; //20dBm
unsigned int *tx_gain_table = (unsigned int*) tx_gain_table_c;


fifo_read_emos *fifo_output = NULL;
char *fifo_buffer = NULL;
char *fifo_ptr = NULL;
char  date_string[1024] = "date: ";
char  dumpfile_dir[1024] = "$HOME/EMOS/data/";

// GUI variables
float power1_memory[2][SCREEN_MEMORY_SIZE];
float power2_memory[2][SCREEN_MEMORY_SIZE];
float noise1_memory[2][SCREEN_MEMORY_SIZE];
float noise2_memory[2][SCREEN_MEMORY_SIZE];
float snr1_memory[2][SCREEN_MEMORY_SIZE];
float snr2_memory[2][SCREEN_MEMORY_SIZE];
// float bw1_memory[];
// float bw2_memory[];
float capacity_memory[SCREEN_MEMORY_SIZE];
float time_memory[SCREEN_MEMORY_SIZE];
float ***channel = NULL;						// this variable is of size [NB_ANTENNAS](num_tx_ant][NUMBER_OF_OFDM_CARRIERS_EMOS]
struct complexf ***channelT = NULL; // this variable is of size [NUMBER_OF_OFDM_CARRIERS_EMOS][NB_ANTENNAS](num_tx_ant]
float *subcarrier_ind = NULL;
float *delay_ind = NULL;

//Openair variables
PHY_CONFIG PHY_config_mem;
PHY_CONFIG *PHY_config;

// GPS
//char gpsd_buffer[GPSD_BUFFER_SIZE];
//int gpsd_fd;
struct gps_data_t *gps_data = NULL;
struct gps_fix_t dummy_gps_data;

// Function prototypes
void exit_callback(FL_OBJECT *ob, long user_data);
void power_callback(FL_OBJECT *ob, long user_data);
void terminal_mode_callback(FL_OBJECT *ob, long user_data);
//void use_gps_callback(FL_OBJECT *ob, long user_data);
//void set_origin_callback(FL_OBJECT *ob, long user_data);
void refresh_callback(FL_OBJECT *ob, long user_data);
void record_callback(FL_OBJECT *ob, long user_data);
void new_data_callback(FL_OBJECT *ob, long user_data);
void time_freq_callback(FL_OBJECT *ob, long user_data);
void noise_snr_callback(FL_OBJECT *ob, long user_data);
void gps_data_callback(FL_OBJECT *ob, long user_data);

void initialize_interface();
void stop_interface();
void refresh_interface();
int open_dumpfile();

void message(const char *msg);
void error(const char *msg);

int mac_phy_init();

// void get_gps_time_position(int socknum, char *buffer, int buffersize);
// int open_gpsd_socket();
// void  update_gps_data(struct gps_data_t *gps_data, char *buf);

int help() 
{
  printf("Valid options + default values: -c %d (# CHs), -n %d (# TX ant), -g %d (TX gain (dBm)), -h (help)\n", num_ch, num_tx_ant, tx_gain);
}

int main(int argc, char *argv[])
{
	char temp_text[1024];
	char c;
	int ar,at,as;
	long temp = 0; 
	int ioctl_result;

	printf("sizeof(fifo_read_emos) = %x\n",sizeof(fifo_read_emos));
	printf("sizeof(gps_fix_t) = %x\n",sizeof(struct gps_fix_t));
	
	while ((c = getopt (argc, argv, "hn:c:g:")) != -1)
	  {
	    switch (c)
	      {
	      case 'h':
		help ();
		exit (1);
		/*
	      case 'b':
		is_cluster_head = atoi (optarg);
		break;
	      case 'i':
		node_id = atoi (optarg);
		break;
		*/
	      case 'g':
		tx_gain = atoi (optarg);
		break;
	      case 'n':
		num_tx_ant = atoi (optarg);
		break;
	      case 'c':
		num_ch = atoi (optarg);
		break;
	      default:
		help ();
		exit (-1);
		break;
	      }
	  }
	
	// Initialize the buffers for the channels according to the number of TX antennas
	channel = (float***) malloc(NB_ANTENNAS_RX * sizeof(float**));
	if (!channel) 
		error("Cannot allocate channel (1)");
	for (ar = 0; ar < NB_ANTENNAS_RX; ar++)
	{  	// Loops over the receive antennas
		channel[ar] = (float**) malloc(num_ch * num_tx_ant * sizeof(float*)); 
		if (!channel[ar]) 
			error("Cannot allocate channel (2)");
		for (at = 0; at < num_tx_ant*num_ch; at++)
		{  	// Loops over the transmit antennas
			channel[ar][at] = (float*) malloc(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant * sizeof(float));
			if (!channel[ar][at]) 
				error("Cannot allocate channel (3)");
		}
	}
	
	channelT = (struct complexf***) malloc(NUMBER_OF_USEFUL_CARRIERS_EMOS * sizeof(struct complexf**));
	if (!channelT) 
		error("Cannot allocate channelT (1)");
	else
	{
		for (as = 0; as < NUMBER_OF_USEFUL_CARRIERS_EMOS/num_tx_ant; as++)
		{  	
			channelT[as] = (struct complexf**) malloc(NB_ANTENNAS_RX * sizeof(struct complexf*)); 
			if (!channelT[as]) 
				error("Cannot allocate channelT (2)");
			else
			{
				for (ar = 0; ar < NB_ANTENNAS_RX; ar++)
				{  	// Loops over the transmit antennas
					channelT[as][ar] = (struct complexf*) malloc(num_tx_ant * sizeof(struct complexf));
					if (!channelT[as][ar]) 
						error("Cannot allocate channelT (3)");
				}
			}
		}
	}
	
	subcarrier_ind = (float*) malloc(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant * sizeof(float));
	if (!subcarrier_ind)
		error("Cannot allocate subcarrier_ind");
	for (as = 0; as < NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant; as++)
		subcarrier_ind[as] = //(NUMBER_OF_OFDM_CARRIERS_EMOS/2 + as * num_tx_ant)%NUMBER_OF_OFDM_CARRIERS_EMOS;
				as * num_tx_ant - NUMBER_OF_OFDM_CARRIERS_EMOS/2;
	
	delay_ind = (float*) malloc(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant * sizeof(float));
	if (!delay_ind)
		error("Cannot allocate delay_ind");
	for (as = 0; as < NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant; as++)
		delay_ind[as] = as / 7.68e6;

	fl_initialize(&argc, argv, "EMOS terminal GUI", 0, 0);
	main_frm = create_form_main_frm();
	config_frm = create_form_config_dialog();
	
	// Get time stamp of startup time (for time difference calculations)
	start_time = rt_get_time_ns();
	
	fl_set_timer(main_frm->refresh_timer, screen_refresh_period);
	fl_suspend_timer(main_frm->refresh_timer);
	
	//fl_set_timer(main_frm->gps_timer, 0.05);
	//fl_suspend_timer(main_frm->gps_timer);

	stop_interface();
	
	main_wnd = fl_show_form(main_frm->main_frm, FL_PLACE_HOTSPOT, FL_FULLBORDER, "EMOS terminal GUI");
	
	// Check for openair device
	if ((openair_dev_fd = open("/dev/openair0", O_RDWR,0)) < 0)
	{
		//Could not find the module
		fl_set_object_lcolor(main_frm->cbmimo_lbl, SCREEN_COLOR_OFF);
		fl_set_object_color(main_frm->cbmimo_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
		
		emos_ready = EMOS_NOT_READY;
		
		error("Openair module not found. Load modules first!");
	}
	else
	{
		mac_phy_init();
	}
	
	//screen_refresh_in_frames = ceil(screen_refresh_period/2.6667e-3);
	
	// set the default dumfile directory
	sprintf(dumpfile_dir,"%s/EMOS/data/",getenv("HOME"));
	printf("Default directory = %s\n",dumpfile_dir);
	
	fl_do_forms();
	
  return EXIT_SUCCESS;
}


void initialize_interface()
{
	char temp_label[1024];
	
	// Pannel widgets
	/////////////////////////////
	
	// Screen widgets
	/////////////////////////////
	
	// Labels
	fl_set_object_lcolor(main_frm->rx1_lbl, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->rx2_lbl, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->pwr1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->pwr1_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->pwr2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->pwr2_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->noise1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->noise1_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->noise2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->noise2_xyp, SCREEN_COLOR_ON);
	
/*	fl_set_object_color(main_frm->bw1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->bw1_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->bw2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->bw2_xyp, SCREEN_COLOR_ON);*/
	
	fl_set_object_color(main_frm->ch11_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch11_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch12_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch12_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch13_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch13_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch14_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch14_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch21_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch21_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch22_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch22_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch23_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch23_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->ch24_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->ch24_xyp, SCREEN_COLOR_ON);
	
	fl_set_object_color(main_frm->cap_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->cap_xyp, SCREEN_COLOR_ON);
	
	// Labels
	sprintf(temp_label, "idx:");
	fl_set_object_label(main_frm->idx_lbl, temp_label);
	fl_set_object_lcolor(main_frm->idx_lbl, SCREEN_COLOR_ON);
	
	// Terminal mode indicator
	fl_set_object_lcolor(main_frm->mode_lbl, SCREEN_COLOR_ON);
	
	if(domain_selector == TIME_DOMAIN)
	{
		fl_set_button(main_frm->time_domain_btn,1);
		fl_set_button(main_frm->freq_domain_btn,0);
	}
	else
	{
		fl_set_button(main_frm->time_domain_btn,0);
		fl_set_button(main_frm->freq_domain_btn,1);
	}
	
	if(noise_selector == N0)
	{
		fl_set_button(main_frm->n0_btn,1);
		fl_set_button(main_frm->snr_btn,0);
	}
	else
	{
		fl_set_button(main_frm->n0_btn,0);
		fl_set_button(main_frm->snr_btn,1);
	}
	
	if(terminal_mode == TERM_MODE_SINGL)
	{
		sprintf(temp_label, "Mode: SGL");
		fl_set_object_label(main_frm->mode_lbl, temp_label);
		fl_set_object_color(main_frm->single_led, LED_COLOR_ON, LED_COLOR_OFF);
		fl_set_object_color(main_frm->multi_led, LED_COLOR_OFF, LED_COLOR_OFF);
	}
	else
	{
		sprintf(temp_label, "Mode: MLT");
		fl_set_object_label(main_frm->mode_lbl, temp_label);
		fl_set_object_color(main_frm->single_led, LED_COLOR_OFF, LED_COLOR_OFF);
		fl_set_object_color(main_frm->multi_led, LED_COLOR_ON, LED_COLOR_OFF);
	}
	
	// Recording indicator OFF
	fl_set_object_lcolor(main_frm->rec_lbl, SCREEN_COLOR_OFF);
	fl_set_object_color(main_frm->rec_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Sync indicator
	fl_set_object_lcolor(main_frm->sync_lbl, SCREEN_COLOR_OFF);
	fl_set_object_color(main_frm->sync_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Error indicator
	fl_set_object_lcolor(main_frm->error_lbl, SCREEN_COLOR_OFF);
	fl_set_object_color(main_frm->error_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Buffer indicator
	fl_set_object_lcolor(main_frm->buffer_lbl, SCREEN_COLOR_OFF);
	
	// CBMIMO indicator
	if(emos_ready)
	{
		fl_set_object_lcolor(main_frm->cbmimo_lbl, SCREEN_COLOR_OFF);
		fl_set_object_color(main_frm->cbmimo_lbl, SCREEN_COLOR_ON, SCREEN_COLOR_OFF);
	}
	else
	{
		fl_set_object_lcolor(main_frm->cbmimo_lbl, SCREEN_COLOR_OFF);
		fl_set_object_color(main_frm->cbmimo_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
	}
	
	// GPS indicator
	fl_set_object_lcolor(main_frm->gps_lbl, SCREEN_COLOR_OFF);
	
	// Fix indicator
	fl_set_object_lcolor(main_frm->fix_lbl, SCREEN_COLOR_OFF);
	
	// X indicator
	fl_set_object_lcolor(main_frm->gps_lat_lbl, SCREEN_COLOR_OFF);
	
	// Y indicator
	fl_set_object_lcolor(main_frm->gps_lon_lbl, SCREEN_COLOR_OFF);
	
	// Date indicator
	fl_set_object_label(main_frm->date_lbl, date_string);
	fl_set_object_lcolor(main_frm->date_lbl, SCREEN_COLOR_ON);
	
	// Refresh interval indicator
	fl_set_object_lcolor(main_frm->refresh_lbl, SCREEN_COLOR_ON);
	sprintf(temp_label, "Refresh interval: %.2f s", screen_refresh_period);
	fl_set_object_label(main_frm->refresh_lbl, temp_label);
	fl_set_dial_value(main_frm->refresh_dial, screen_refresh_period);
	
	// BLER indicator
	fl_set_object_lcolor(main_frm->bler_lbl, SCREEN_COLOR_OFF);
	
	// REC'd frames indicator
	fl_set_object_lcolor(main_frm->n_recd_frames_lbl, SCREEN_COLOR_OFF);

	// RX mode indicator
	fl_set_object_lcolor(main_frm->rx_mode_lbl, SCREEN_COLOR_OFF);
	
	// Messages
	fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
	
	switch(terminal_idx)
	{
		case 1:
			fl_set_button(main_frm->terminal_btn1,1);
			break;
		case 2:
			fl_set_button(main_frm->terminal_btn2,1);
			break;
		case 3:
			fl_set_button(main_frm->terminal_btn3,1);
			break;
		case 4:
			fl_set_button(main_frm->terminal_btn4,1);
			break;
	}

	switch(rxmode)
	{
	        case SINGLE:
			fl_set_button(main_frm->single_btn,1);
			break;
		case MMSE:
			fl_set_button(main_frm->mmse_btn,1);
			break;
		case ML:
			fl_set_button(main_frm->ml_btn,1);
			break;
	}	
	
	
// 	fl_set_object_color(main_frm->const1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
// 	fl_set_object_color(main_frm->const2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_ON);
// 	fl_set_object_lcolor(main_frm->const1_xyp, SCREEN_COLOR_ON);
// 	fl_set_object_lcolor(main_frm->const2_xyp, SCREEN_COLOR_ON);

	fl_deactivate_object(main_frm->num_sym_dial);
	fl_deactivate_object(main_frm->num_sym_plus);
	fl_deactivate_object(main_frm->num_sym_minus);
	//fl_deactivate_object(main_frm->use_gps_btn);
	//fl_deactivate_object(main_frm->set_origin_btn);
}

void stop_interface()
{
	// Pannel widgets
	/////////////////////////////
	fl_set_object_color(main_frm->single_led, LED_COLOR_OFF, LED_COLOR_OFF);
	fl_set_object_color(main_frm->multi_led, LED_COLOR_OFF, LED_COLOR_OFF);
	
	// Screen widgets
	/////////////////////////////
	
	// Labels
	fl_set_object_lcolor(main_frm->rx1_lbl, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->rx2_lbl, SCREEN_COLOR_BG);
	
	// Graphs
	fl_set_object_color(main_frm->pwr1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->pwr1_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->pwr2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->pwr2_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->noise1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->noise1_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->noise2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->noise2_xyp, SCREEN_COLOR_BG);
	
/*	fl_set_object_color(main_frm->bw1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->bw1_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->bw2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->bw2_xyp, SCREEN_COLOR_BG);*/
	
	fl_set_object_color(main_frm->ch11_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch11_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch12_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch12_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch13_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch13_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch14_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch14_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch21_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch21_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch22_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch22_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch23_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch23_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->ch24_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->ch24_xyp, SCREEN_COLOR_BG);
	
	fl_set_object_color(main_frm->cap_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->cap_xyp, SCREEN_COLOR_BG);
	
	// Labels
	
	// Index indicator
	fl_set_object_lcolor(main_frm->idx_lbl, SCREEN_COLOR_BG);
	
	// Terminal mode indicator
	fl_set_object_lcolor(main_frm->mode_lbl, SCREEN_COLOR_BG);
	
	// Recording indicator OFF
	fl_set_object_lcolor(main_frm->rec_lbl, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->rec_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Sync indicator
	fl_set_object_lcolor(main_frm->sync_lbl, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->sync_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Error indicator
	fl_set_object_lcolor(main_frm->error_lbl, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->error_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Buffer indicator
	fl_set_object_lcolor(main_frm->buffer_lbl, SCREEN_COLOR_BG);
	
	// CBMIMO indicator
	fl_set_object_lcolor(main_frm->cbmimo_lbl, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->cbmimo_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// GPS indicator
	fl_set_object_lcolor(main_frm->gps_lbl, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->gps_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
	
	// Fix indicator
	fl_set_object_lcolor(main_frm->fix_lbl, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->fix_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	
	// X indicator
	fl_set_object_lcolor(main_frm->gps_lat_lbl, SCREEN_COLOR_BG);
	
	// Y indicator
	fl_set_object_lcolor(main_frm->gps_lon_lbl, SCREEN_COLOR_BG);
	
	// Date indicator
	fl_set_object_lcolor(main_frm->date_lbl, SCREEN_COLOR_BG);
	
	// Refresh interval indicator
	fl_set_object_lcolor(main_frm->refresh_lbl, SCREEN_COLOR_BG);
	
	// BLER indicator
	fl_set_object_lcolor(main_frm->bler_lbl, SCREEN_COLOR_BG);

	// REC'd frames indicator
	fl_set_object_lcolor(main_frm->n_recd_frames_lbl, SCREEN_COLOR_BG);
	
	// RX mode indicator
	fl_set_object_lcolor(main_frm->rx_mode_lbl, SCREEN_COLOR_BG);
	
	// Messages
	fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_BG);
	
/*	fl_set_object_color(main_frm->const1_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_color(main_frm->const2_xyp, SCREEN_COLOR_BG, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->const1_xyp, SCREEN_COLOR_BG);
	fl_set_object_lcolor(main_frm->const2_xyp, SCREEN_COLOR_BG);*/
	
}

void refresh_interface()
{
	char temp_label[1024];
	int i,ar,at,as,ac, gps_error;
	float disp_max_power = 0;
	struct complexf channel_temp[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant];	//frequency response of one link of the MIMO channel
	struct complexf channel_temp2[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant/2];	//frequency response of one link of the MIMO channel
	struct complexf **IpHH=NULL;													//matrix used for the capacity of one subcarrier C[i] = log(det(IpHH))
	float norm = 0;
	double snr_lin;
	float capacity4x2 = 0; 																									//overall capacity of the 4x2 frequency selcective MIMO channel
	static RTIME last_timestamp = (RTIME) 0; 
	int disp_min_power = 30;
	
	IpHH = (struct complexf **) malloc(NB_ANTENNAS_RX * sizeof(struct complexf *));
	for (ar=0; ar<NB_ANTENNAS_RX; ar++)
		IpHH[ar] = (struct complexf *) malloc(NB_ANTENNAS_RX * sizeof(struct complexf));
	
	if (fifo_output)
	{
		
	  // Channel response
	  // Reshape the channel array for visialization
	  for (ar = 0; ar < NB_ANTENNAS_RX; ar++)
	    {  	// Loops over the receive antennas
	      for (ac = 0; ac < num_ch; ac++)
		{  	// Loops over the cluster heads
		  for (at = 0; at < num_tx_ant; at++)
		    {  	// Loops over the transmit antennas
		      for (as = 0; as < NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant; as++)
			{  	// Loops over the symbols
			  channel_temp[as].r = (float) fifo_output->channel_f_unpacked[ac][ar][as*num_tx_ant + at].r;					
			  channel_temp[as].i = (float) fifo_output->channel_f_unpacked[ac][ar][as*num_tx_ant + at].i;					
			}
				
		      if(domain_selector == TIME_DOMAIN)
			{
			  // IFFT of the freq domain channel
			  FFT((float*) channel_temp, NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, -1);
					
			  // normalize by 1/sqrt(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant)
			  for (i=0; i<NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant; i++)
			    {
			      channel_temp[i].r = channel_temp[i].r / sqrt(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant);
			      channel_temp[i].i = channel_temp[i].i / sqrt(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant);
			    }
			}
		      else
			{
			  // do an fft shift
			  memcpy(channel_temp2, channel_temp, NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant/2*sizeof(struct complexf));
			  memcpy(channel_temp, channel_temp+NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant/2, NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant/2*sizeof(struct complexf));
			  memcpy(channel_temp+NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant/2, channel_temp2, NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant/2*sizeof(struct complexf));
			}
					
				
		      for (as = 0; as < NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant; as++)
			{  	
			  // Loops over the transmit antennas					
			  channel[ar][at+ac*num_ch][as] = 10*log10(channel_temp[as].r * channel_temp[as].r 
								   + channel_temp[as].i * channel_temp[as].i + 1);
							
			  if (channel[ar][at][as] > disp_max_power)
			    disp_max_power = channel[ar][at][as];
			}
		    }
		}
	    }
		disp_max_power = 90;
		
		// Lost frames
		pdu_errors_last[0] = pdu_errors[0];
		pdu_errors[0] = fifo_output->pdu_errors[0];
		pdu_errors_last[1] = pdu_errors[1];
		pdu_errors[1] = fifo_output->pdu_errors[1];
		
		// Capacity
		// We calculate the capacity of the frequency selective channel according to [Eq. (4.58), Paulraj et al. 2003]:
		// C = 1/(NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant) sum_{i=0}^{NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant}
		// log2(det(I + 1/num_tx_ant H H^H))
		
		// Reshape the channel stored in fifo_output->channel_f_unpacked and calculate norm
		norm = 0;
		for (as=0; as<NUMBER_OF_USEFUL_CARRIERS_EMOS/num_tx_ant; as++)
		{
		  for (ar=0; ar<NB_ANTENNAS_RX; ar++)
		    {
		      for (at=0; at<num_tx_ant; at++)
			{
			  channelT[as][ar][at].r = (float) fifo_output->channel_f_unpacked[0][ar][(176+as*num_tx_ant)%NUMBER_OF_OFDM_CARRIERS_EMOS + at].r;
			  channelT[as][ar][at].i = (float) fifo_output->channel_f_unpacked[0][ar][(176+as*num_tx_ant)%NUMBER_OF_OFDM_CARRIERS_EMOS + at].i;
			  /*					channelT[as][ar][at].r = (float) 1;
								channelT[as][ar][at].i = (float) 0;			*/
			}
		    }
		  norm += norm_fro_sq(channelT[as],NB_ANTENNAS_RX,num_tx_ant);
		}
		//printf("H = [%g %g; %g %g]\n",channelT[0][0][0].r,channelT[0][0][1].r,channelT[0][1][0].r,channelT[0][1][1].r);
		
		norm /= (NUMBER_OF_USEFUL_CARRIERS_EMOS/num_tx_ant);
		//printf("norm=%g\n",norm);
		
		snr_lin = (pow(10,(double) (fifo_output->PHY_measurements.rx_power_dB[0] - fifo_output->PHY_measurements.n0_power_dB[0])/10) +
			   pow(10,(double) (fifo_output->PHY_measurements.rx_power_dB[1] - fifo_output->PHY_measurements.n0_power_dB[1])/10))/2;					
			
		// calculate capacity for each subcarrier and take the mean
		capacity4x2 = 0;
		for (as=0; as<NUMBER_OF_USEFUL_CARRIERS_EMOS/num_tx_ant; as++)
		{
		  square_mat(channelT[as],NB_ANTENNAS_RX,num_tx_ant,IpHH);
		  addI_scale_mat(IpHH,NB_ANTENNAS_RX,NB_ANTENNAS_RX*snr_lin/norm);
		  capacity4x2 += log(det(IpHH))/log(2);
		}
		capacity4x2 /= NUMBER_OF_USEFUL_CARRIERS_EMOS;
		//printf("IpHH = [%g %g; %g %g]\n",IpHH[0][0].r,IpHH[0][1].r,IpHH[1][0].r,IpHH[1][1].r);
		//printf("capacity=%g\n",capacity4x2);
		
		// Prepare data for Power meters
		if (last_timestamp != fifo_output->timestamp)
		{
		  //printf("rx_power=%d, noise_power=%d, snr=%d\n",fifo_output->rx_power_db[0],fifo_output->n0_power_db[0],fifo_output->rx_power_db[0]-fifo_output->n0_power_db[0]);

		  /*
		    if(values_in_memory < SCREEN_MEMORY_SIZE)
		    {		
		    // Initial vector filling 
		    time_memory[values_in_memory] = (float)(fifo_output->timestamp - start_time) / (float)1e9;
		    power1_memory[chsch_index-1][values_in_memory] = (float)fifo_output->PHY_measurements.rx_rssi_dBm[chsch_index][0];
		    power2_memory[chsch_index-1][values_in_memory] = (float)fifo_output->PHY_measurements.rx_rssi_dBm[chsch_index][1];
		    noise1_memory[chsch_index-1][values_in_memory] = (float)fifo_output->PHY_measurements.n0_power_dB[chsch_index][0];
		    noise2_memory[chsch_index-1][values_in_memory] = (float)fifo_output->PHY_measurements.n0_power_dB[chsch_index][1];
		    //noise2_memory[values_in_memory] = ((float) (pdu_errors - pdu_errors_last)/(screen_refresh_period/2.6667e-3));
		    snr1_memory[chsch_index-1][values_in_memory] = (float)(fifo_output->PHY_measurements.rx_power_dB[chsch_index][0] - fifo_output->PHY_measurements.n0_power_dB[chsch_index][0]);
		    snr2_memory[chsch_index-1][values_in_memory] = (float)(fifo_output->PHY_measurements.rx_power_dB[chsch_index][1] - fifo_output->PHY_measurements.n0_power_dB[chsch_index][1]);					
		    capacity_memory[values_in_memory] = capacity4x2;
		    values_in_memory++;
		    }
		    else
		  */

		  // FIFO vector filling
		  for (i = 0; i < SCREEN_MEMORY_SIZE - 1; i++) {
			time_memory[i] = time_memory[i+1];
			for (chsch_index=0;chsch_index<2;chsch_index++){
			  power1_memory[chsch_index][i] = power1_memory[chsch_index][i+1];
			  power2_memory[chsch_index][i] = power2_memory[chsch_index][i+1];
			  noise1_memory[chsch_index][i] = noise1_memory[chsch_index][i+1];
			  noise2_memory[chsch_index][i] = noise2_memory[chsch_index][i+1];
			  snr1_memory[chsch_index][i] = snr1_memory[chsch_index][i+1];
			  snr2_memory[chsch_index][i] = snr2_memory[chsch_index][i+1];
			}
			capacity_memory[i] = capacity_memory[i+1];
		  }
		  
		    
		  time_memory[SCREEN_MEMORY_SIZE - 1] = (float)(fifo_output->timestamp - start_time) / (float)1e9;
		  for (chsch_index=0;chsch_index<2;chsch_index++){
		    if (!is_cluster_head) {
		      power1_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.rx_rssi_dBm[chsch_index+1][0];
		      power2_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.rx_rssi_dBm[chsch_index+1][1];
		      noise1_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.n0_power_dB[chsch_index+1][0];
		      noise2_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.n0_power_dB[chsch_index+1][1];
		      snr1_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)(fifo_output->PHY_measurements.rx_power_dB[chsch_index+1][0] -
									       fifo_output->PHY_measurements.n0_power_dB[chsch_index+1][0]);
		      snr2_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)(fifo_output->PHY_measurements.rx_power_dB[chsch_index+1][1] -
									       fifo_output->PHY_measurements.n0_power_dB[chsch_index+1][1]);
		    }
		    else {
		      power1_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.rx_rssi_dBm[chsch_index][0];
		      power2_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.rx_rssi_dBm[chsch_index][1];
		      noise1_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.n0_power_dB[chsch_index][0];
		      noise2_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)fifo_output->PHY_measurements.n0_power_dB[chsch_index][1];
		      snr1_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)(fifo_output->PHY_measurements.rx_power_dB[chsch_index][0] -
									       fifo_output->PHY_measurements.n0_power_dB[chsch_index][0]);
		      snr2_memory[chsch_index][SCREEN_MEMORY_SIZE-1] = (float)(fifo_output->PHY_measurements.rx_power_dB[chsch_index][1] -
									       fifo_output->PHY_measurements.n0_power_dB[chsch_index][1]);
		    }
		  }
		  capacity_memory[SCREEN_MEMORY_SIZE - 1] = capacity4x2;

		  values_in_memory = SCREEN_MEMORY_SIZE;
		}

		last_timestamp = fifo_output->timestamp;
		
		// Pannel widgets
		/////////////////////////////
		
		// Screen widgets
		/////////////////////////////
		
		// Power meters
		fl_set_xyplot_data(main_frm->pwr1_xyp, time_memory, power1_memory[0], values_in_memory, "", "time (s)", "dBm");
		fl_add_xyplot_overlay(main_frm->pwr1_xyp, 1, time_memory, power1_memory[1], values_in_memory, FL_BLUE);
		fl_set_xyplot_ybounds(main_frm->pwr1_xyp,	-100, -40);
		fl_set_xyplot_data(main_frm->pwr2_xyp, time_memory, power2_memory[0], values_in_memory, "", "time (s)", "dBm");
		fl_add_xyplot_overlay(main_frm->pwr2_xyp, 1, time_memory, power2_memory[1], values_in_memory, FL_BLUE);
		fl_set_xyplot_ybounds(main_frm->pwr2_xyp,	-100, -40);
		
		if (noise_selector == N0)
		{
			fl_set_xyplot_data(main_frm->noise1_xyp, time_memory, noise1_memory[0], values_in_memory, "", "time (s)", "dB");
			fl_add_xyplot_overlay(main_frm->noise1_xyp, 1, time_memory, noise1_memory[1], values_in_memory, FL_BLUE);
			fl_set_xyplot_ybounds(main_frm->noise1_xyp,	0, 40);
			fl_set_xyplot_data(main_frm->noise2_xyp, time_memory, noise2_memory[0], values_in_memory, "", "time (s)", "dB");
			fl_add_xyplot_overlay(main_frm->noise2_xyp, 1, time_memory, noise2_memory[1], values_in_memory, FL_BLUE);
			fl_set_xyplot_ybounds(main_frm->noise2_xyp,	0, 40);
			//fl_set_xyplot_ybounds(main_frm->noise2_xyp,	0, 1);
		}
		else
		{
			fl_set_xyplot_data(main_frm->noise1_xyp, time_memory, snr1_memory[0], values_in_memory, "", "time (s)", "dB");
			fl_add_xyplot_overlay(main_frm->noise1_xyp, 1, time_memory, snr1_memory[1], values_in_memory, FL_BLUE);
			fl_set_xyplot_ybounds(main_frm->noise1_xyp,	0, 40);
			fl_set_xyplot_data(main_frm->noise2_xyp, time_memory, snr2_memory[0], values_in_memory, "", "time (s)", "dB");
			fl_add_xyplot_overlay(main_frm->noise2_xyp, 1, time_memory, snr2_memory[1], values_in_memory, FL_BLUE);
			fl_set_xyplot_ybounds(main_frm->noise2_xyp,	0, 40);
		}
		// Channel response
		if(domain_selector == FREQ_DOMAIN)
		{
			// Frequency domain plots
			fl_set_xyplot_data(main_frm->ch11_xyp, subcarrier_ind, channel[0][0], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
			fl_set_xyplot_xtics(main_frm->ch11_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch11_xyp, -1, -1);
			fl_set_xyplot_xbounds(main_frm->ch11_xyp, -100, 100);
			fl_set_xyplot_ybounds(main_frm->ch11_xyp,	disp_min_power, disp_max_power);
			fl_set_xyplot_data(main_frm->ch12_xyp, subcarrier_ind, channel[0][1], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
			fl_set_xyplot_xtics(main_frm->ch12_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch12_xyp, -1, -1);
			fl_set_xyplot_xbounds(main_frm->ch12_xyp, -100, 100);
			fl_set_xyplot_ybounds(main_frm->ch12_xyp,	disp_min_power, disp_max_power);
			fl_set_xyplot_data(main_frm->ch21_xyp, subcarrier_ind, channel[1][0], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
			fl_set_xyplot_xtics(main_frm->ch21_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch21_xyp, -1, -1);
			fl_set_xyplot_xbounds(main_frm->ch21_xyp, -100, 100);
			fl_set_xyplot_ybounds(main_frm->ch21_xyp,	disp_min_power, disp_max_power);
			fl_set_xyplot_data(main_frm->ch22_xyp, subcarrier_ind, channel[1][1], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
			fl_set_xyplot_xtics(main_frm->ch22_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch22_xyp, -1, -1);
			fl_set_xyplot_xbounds(main_frm->ch22_xyp, -100, 100);
			fl_set_xyplot_ybounds(main_frm->ch22_xyp,	disp_min_power, disp_max_power);
			if ((num_tx_ant*num_ch)==4)
			{	
				fl_set_xyplot_data(main_frm->ch13_xyp, subcarrier_ind, channel[0][2], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
				fl_set_xyplot_xtics(main_frm->ch13_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch13_xyp, -1, -1);
				fl_set_xyplot_xbounds(main_frm->ch13_xyp, -100, 100);
				fl_set_xyplot_ybounds(main_frm->ch13_xyp,	disp_min_power, disp_max_power);
				fl_set_xyplot_data(main_frm->ch14_xyp, subcarrier_ind, channel[0][3], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
				fl_set_xyplot_xtics(main_frm->ch14_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch14_xyp, -1, -1);
				fl_set_xyplot_xbounds(main_frm->ch14_xyp, -100, 100);
				fl_set_xyplot_ybounds(main_frm->ch14_xyp,	disp_min_power, disp_max_power);
				fl_set_xyplot_data(main_frm->ch23_xyp, subcarrier_ind, channel[1][2], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
				fl_set_xyplot_xtics(main_frm->ch23_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch23_xyp, -1, -1);
				fl_set_xyplot_xbounds(main_frm->ch23_xyp, -100, 100);
				fl_set_xyplot_ybounds(main_frm->ch23_xyp,	disp_min_power, disp_max_power);
				fl_set_xyplot_data(main_frm->ch24_xyp, subcarrier_ind, channel[1][3], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "subcarrier index", "dB");
				fl_set_xyplot_xtics(main_frm->ch24_xyp, 0, 0);
			//fl_set_xyplot_ytics(main_frm->ch24_xyp, -1, -1);
				fl_set_xyplot_xbounds(main_frm->ch24_xyp, -100, 100);
				fl_set_xyplot_ybounds(main_frm->ch24_xyp,	disp_min_power, disp_max_power);
			}
		}
		else
		{
			// Time domain plots
			fl_set_xyplot_data(main_frm->ch11_xyp, delay_ind, channel[0][0], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
			fl_set_xyplot_xtics(main_frm->ch11_xyp, 2, 1);
			fl_set_xyplot_xbounds(main_frm->ch11_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
			fl_set_xyplot_ybounds(main_frm->ch11_xyp,	disp_min_power, disp_max_power);
			fl_set_xyplot_data(main_frm->ch12_xyp, delay_ind, channel[0][1], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
			fl_set_xyplot_xtics(main_frm->ch12_xyp, 2, 1);
			fl_set_xyplot_xbounds(main_frm->ch12_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
			fl_set_xyplot_ybounds(main_frm->ch12_xyp,	disp_min_power, disp_max_power);
			fl_set_xyplot_data(main_frm->ch21_xyp, delay_ind, channel[1][0], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
			fl_set_xyplot_xtics(main_frm->ch21_xyp, 2, 1);
			fl_set_xyplot_xbounds(main_frm->ch21_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
			fl_set_xyplot_ybounds(main_frm->ch21_xyp,	disp_min_power, disp_max_power);
			fl_set_xyplot_data(main_frm->ch22_xyp, delay_ind, channel[1][1], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
			fl_set_xyplot_xtics(main_frm->ch22_xyp, 2, 1);
			fl_set_xyplot_xbounds(main_frm->ch22_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
			fl_set_xyplot_ybounds(main_frm->ch22_xyp,	disp_min_power, disp_max_power);
			if ((num_tx_ant*num_ch)==4)
			{	
				fl_set_xyplot_data(main_frm->ch13_xyp, delay_ind, channel[0][2], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
				fl_set_xyplot_xtics(main_frm->ch13_xyp, 2, 1);
				fl_set_xyplot_xbounds(main_frm->ch13_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
				fl_set_xyplot_ybounds(main_frm->ch13_xyp,	disp_min_power, disp_max_power);
				fl_set_xyplot_data(main_frm->ch14_xyp, delay_ind, channel[0][3], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
				fl_set_xyplot_xtics(main_frm->ch14_xyp, 2, 1);
				fl_set_xyplot_xbounds(main_frm->ch14_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
				fl_set_xyplot_ybounds(main_frm->ch14_xyp,	disp_min_power, disp_max_power);
				fl_set_xyplot_data(main_frm->ch23_xyp, delay_ind, channel[1][2], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
				fl_set_xyplot_xtics(main_frm->ch23_xyp, 2, 1);
				fl_set_xyplot_xbounds(main_frm->ch23_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
				fl_set_xyplot_ybounds(main_frm->ch23_xyp,	disp_min_power, disp_max_power);
				fl_set_xyplot_data(main_frm->ch24_xyp, delay_ind, channel[1][3], NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant, "", "delay", "dB");
				fl_set_xyplot_xtics(main_frm->ch24_xyp, 2, 1);
				fl_set_xyplot_xbounds(main_frm->ch24_xyp,	0, delay_ind[NUMBER_OF_OFDM_CARRIERS_EMOS/num_tx_ant]);
				fl_set_xyplot_ybounds(main_frm->ch24_xyp,	disp_min_power, disp_max_power);
			}
		}
			
		// BLER
		if ((is_cluster_head) || (fifo_output->rx_mode==0)) {
		  sprintf(temp_label, "BLER: %f", ((double) (pdu_errors[0] - pdu_errors_last[0])/(screen_refresh_period/0.0031508)));
		  fl_set_object_label(main_frm->bler_lbl, temp_label);
		  fl_set_object_lcolor(main_frm->bler_lbl, SCREEN_COLOR_ON);
		}
		else {
		  sprintf(temp_label, "BLER: %f, %f", ((double) (pdu_errors[0] - pdu_errors_last[0])/(screen_refresh_period/0.0031508)), ((double) (pdu_errors[1] - pdu_errors_last[1])/(screen_refresh_period/0.0031508)));
		  fl_set_object_label(main_frm->bler_lbl, temp_label);
		  fl_set_object_lcolor(main_frm->bler_lbl, SCREEN_COLOR_ON);
		}

		// RX mode
		if (!is_cluster_head) {
		  switch (fifo_output->rx_mode) {
		  case 0:
		    sprintf(temp_label, "RX mode: SINGLE");
		    break;
		  case 1:
		    sprintf(temp_label, "RX mode: MMSE");
		    break;
		  case 2:
		    sprintf(temp_label, "RX mode: ML");
		    break;
		  default:
		    sprintf(temp_label, "RX mode: %d",fifo_output->rx_mode);
		  }
		  fl_set_object_label(main_frm->rx_mode_lbl, temp_label);
		  fl_set_object_lcolor(main_frm->rx_mode_lbl, SCREEN_COLOR_ON);
		}
		
		// Capacity
		fl_set_xyplot_data(main_frm->cap_xyp, time_memory, capacity_memory, values_in_memory, "", "time (s)", "Bits/sec/Hz");
		fl_set_xyplot_ybounds(main_frm->cap_xyp,	0, 10);


	}

	// update GPS
	if(gps_data)
	{
		if (gps_data->online)
		{
			switch(gps_data->fix.mode)
			{					
			case MODE_2D:
				// Fix indicator		
				fl_set_object_color(main_frm->fix_lbl, SCREEN_COLOR_ON, SCREEN_COLOR_OFF);
				fl_set_object_label(main_frm->fix_lbl, "2D");
				
				// X indicator
				sprintf(temp_label, "lat: %f deg", gps_data->fix.latitude);
				fl_set_object_lcolor(main_frm->gps_lat_lbl, SCREEN_COLOR_ON);
				fl_set_object_label(main_frm->gps_lat_lbl, temp_label);
				
				// Y indicator
				sprintf(temp_label, "lon: %f deg", gps_data->fix.longitude);
				fl_set_object_lcolor(main_frm->gps_lon_lbl, SCREEN_COLOR_ON);
				fl_set_object_label(main_frm->gps_lon_lbl, temp_label);
				
				break;
				
			case MODE_3D:
				// Fix indicator		
				fl_set_object_color(main_frm->fix_lbl, SCREEN_COLOR_ON, SCREEN_COLOR_OFF);
				fl_set_object_label(main_frm->fix_lbl, "3D");
				
				// X indicator
				sprintf(temp_label, "lat: %f deg", gps_data->fix.latitude);
				fl_set_object_lcolor(main_frm->gps_lat_lbl, SCREEN_COLOR_ON);
				fl_set_object_label(main_frm->gps_lat_lbl, temp_label);
				
				// Y indicator
				sprintf(temp_label, "lon: %f deg", gps_data->fix.longitude);
				fl_set_object_lcolor(main_frm->gps_lon_lbl, SCREEN_COLOR_ON);
				fl_set_object_label(main_frm->gps_lon_lbl, temp_label);
				break;
				
			default:
				// STATUS_NO_FIX:
				fl_set_object_color(main_frm->fix_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
				fl_set_object_lcolor(main_frm->fix_lbl, SCREEN_COLOR_OFF);
				fl_set_object_label(main_frm->fix_lbl, "NO");
				
				// X indicator
				sprintf(temp_label, "lat: %f deg", gps_data->fix.latitude);
				fl_set_object_label(main_frm->gps_lat_lbl, temp_label);
				fl_set_object_lcolor(main_frm->gps_lat_lbl, SCREEN_COLOR_OFF);
				
				// Y indicator
				sprintf(temp_label, "lon: %f deg", gps_data->fix.longitude);
				fl_set_object_label(main_frm->gps_lon_lbl, temp_label);
				fl_set_object_lcolor(main_frm->gps_lon_lbl, SCREEN_COLOR_OFF);
				break;
			}
		}
		else
		{
			// STATUS_NO_FIX:
			fl_set_object_color(main_frm->fix_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
			fl_set_object_lcolor(main_frm->fix_lbl, SCREEN_COLOR_OFF);
			fl_set_object_label(main_frm->fix_lbl, "NO");

			//printf("GPS not online\n");
		}
	}
	
	for (ar=0; ar<NB_ANTENNAS_RX; ar++)
		free(IpHH[ar]);
	free(IpHH);
	IpHH = NULL;

} 

void gps_data_callback(FL_OBJECT *ob, long user_data)
{
	//char tmptxt[1024];
	//printf("GPS timer called\n");
	if (gps_data)
	{
		if (gps_poll(gps_data) != 0)
		{
			//sprintf(tmptxt,"Error polling data from GPS, gps_data = %x", gps_data);
			error("Error polling data from GPS");
		}
		//else
			//printf("GPS poll called\n");
	}
		//fl_set_timer(ob, 0.05);

}
  
void message(const char *msg)
{	
	fl_set_object_label(main_frm->msg_text, msg);
	fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
}

void error(const char *msg)
{	
	fprintf(stderr,msg);
	fprintf(stderr,"\n");
	fl_set_object_label(main_frm->msg_text, msg);
	fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
	fl_set_object_lcolor(main_frm->error_lbl, SCREEN_COLOR_OFF);
	fl_set_object_color(main_frm->error_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);	
}
//
// EXIT function
//
void exit_callback(FL_OBJECT *ob, long user_data)
{
	char temp_text[1024];
	int ar,at,as;
	
	if (power)
	{
		//message("Turn the terminal off with <PWR> prior to exiting.");
		sprintf(temp_text, "Turn the terminal off with <PWR> prior to exiting.");
		fl_set_object_label(main_frm->msg_text, temp_text);
	}
	else
	{
		printf("exiting normally.\n");
		
		// Openair device
		close(openair_dev_fd);
		
		// Free the channel buffer memory
		/*
		if (num_tx_ant == 2)
		{
			free(ch11);
			free(ch12);
			free(ch21);
			free(ch22);
		}
		else if (num_tx_ant == 4)
		{
			free(ch11);
			free(ch12);
			free(ch13);
			free(ch14);
			free(ch21);
			free(ch22);
			free(ch23);
			free(ch24);
		}
		*/
		
		for (ar = 0; ar < NB_ANTENNAS_RX; ar++)
		{  	// Loops over the receive antennas
			for (at = 0; at < num_tx_ant*num_ch; at++)
			{  	// Loops over the transmit antennas
				free(channel[ar][at]);
			}
			free(channel[ar]); 
		}
		free(channel);
		channel = NULL;
		for (as = 0; as < NUMBER_OF_USEFUL_CARRIERS_EMOS/num_tx_ant; as++)
		{  	// Loops over the receive antennas
			for (ar = 0; ar < NB_ANTENNAS_RX; ar++)
			{  	// Loops over the transmit antennas
				free(channelT[as][ar]);
			}
			free(channelT[as]); 
		}
		free(channelT);
		channelT = NULL;
		free(subcarrier_ind);
		subcarrier_ind = NULL;
		free(delay_ind);
		delay_ind = NULL;
		
		exit(0);
	}
}

void power_callback(FL_OBJECT *ob, long user_data)
{
	int ioctl_result;
	char temp_text[1024];
	
	if (emos_ready)
	{
	  // Turns on and off the acquisition
	  if (!power)
	    {
	      if (terminal_idx!=0) 
		{
			// Turn the receiver on
			unsigned int fc = 0; 
			unsigned int frequency = 1;
			//unsigned char gains[4];

		        //gains[0] = 177;
			//gains[1] = 173;
			//gains[2] = 30;
			//gains[3] = 17;
			
			ioctl_result=ioctl(openair_dev_fd,openair_SET_TX_GAIN,&tx_gain_table[tx_gain/5+4]);
			ioctl_result=ioctl(openair_dev_fd,openair_SET_RX_MODE,&rxmode);
 
			if (terminal_idx==1) {
			  is_cluster_head = 1;
			  node_id = 0; 
			  fc = (1) | ((frequency&3)<<1) | ((frequency&3)<<3) |  ((node_id&0xFF) << 5);
			  ioctl_result+=ioctl(openair_dev_fd, openair_START_1ARY_CLUSTERHEAD, &fc);
			}
			else if (terminal_idx==2) {
			  is_cluster_head = 2;
			  node_id = 1; 
			  fc = (1) | ((frequency&3)<<1) | ((frequency&3)<<3) |  ((node_id&0xFF) << 5);
			  ioctl_result+=ioctl(openair_dev_fd, openair_START_2ARY_CLUSTERHEAD, &fc);
			}
			else if (terminal_idx==3) {
			  is_cluster_head = 0;
			  node_id = 8; 
			  fc = (1) | ((frequency&3)<<1) | ((frequency&3)<<3) |  ((node_id&0xFF) << 5);
			  ioctl_result+=ioctl(openair_dev_fd, openair_START_NODE, &fc);
			}
			else if (terminal_idx==4) {
			  is_cluster_head = 0;
			  node_id = 9; 
			  fc = (1) | ((frequency&3)<<1) | ((frequency&3)<<3) |  ((node_id&0xFF) << 5);
			  ioctl_result+=ioctl(openair_dev_fd, openair_START_NODE, &fc);
			}
			else 
			  error("Unknown terminal index");

			if (ioctl_result != 0) 
			{
				printf(temp_text, "Error starting terminal mode!");
				fl_set_object_label(main_frm->msg_text, temp_text);
				fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
				fl_set_object_lcolor(main_frm->error_lbl, SCREEN_COLOR_OFF);
				fl_set_object_color(main_frm->error_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
			}
			
			// Open the EMOS FIFO
			fifo_fd = open(CHANSOUNDER_FIFO_DEV, O_RDONLY);
			if (fifo_fd < 0)
			{
				printf(temp_text, "Error opening the EMOS FIFO!");
				fl_set_object_label(main_frm->msg_text, temp_text);
				fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
				fl_set_object_lcolor(main_frm->error_lbl, SCREEN_COLOR_OFF);
				fl_set_object_color(main_frm->error_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
			}		
			
			// flush the FIFO
			if (rtf_reset(fifo_fd))
				error("Error reseting FIFO");
			
			// Ties the FIFO handling routine
			fl_add_io_callback(fifo_fd, FL_READ, &new_data_callback, NULL);
						
			// open GPS
			gps_data = gps_open("127.0.0.1","2947");
			if (gps_data == NULL) 
			{
				fl_set_object_color(main_frm->gps_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
				error("Could not open GPS");
			}
			else
			{
				if (gps_query(gps_data, "w+x\n") != 0)
				{
					//sprintf(tmptxt,"Error sending command to GPS, gps_data = %x", gps_data);
					error("Error sending command to GPS");
				}
				else
					fl_set_object_color(main_frm->gps_lbl, SCREEN_COLOR_ON, SCREEN_COLOR_OFF);
			}

			if (gps_data)
				fl_add_io_callback(gps_data->gps_fd, FL_READ, &gps_data_callback, NULL);

			
			// resume timer
			fl_resume_timer(main_frm->refresh_timer);
			//fl_resume_timer(main_frm->gps_timer);

			initialize_interface();
			power = TERM_ON;
		}
	      else
		{
		  error("Set terminal index first!");		  
		}
		}
		else
		{
			if (record)
			{
				sprintf(temp_text, "Still recording! Stop recording prior to powering down.");
				fl_set_object_label(main_frm->msg_text, temp_text);
			}
			else
			{
				fl_suspend_timer(main_frm->refresh_timer);
				
				// Turn the receiver off	
				ioctl_result = ioctl(openair_dev_fd, openair_STOP, NULL);
				if (ioctl_result != 0) 
				{
					printf(temp_text, "Error stopping terminal mode!");
					fl_set_object_label(main_frm->msg_text, temp_text);
					fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
					fl_set_object_lcolor(main_frm->error_lbl, SCREEN_COLOR_OFF);
					fl_set_object_color(main_frm->error_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
				}
				
				//fl_suspend_timer(main_frm->gps_timer);
				
				if (gps_data)
				{
					gps_close(gps_data);
					gps_data = NULL;
				}
				
				stop_interface();
				power = TERM_OFF;
			}
		}
	}
}


//
// Terminal mode function
//
void terminal_mode_callback(FL_OBJECT *ob, long user_data)
{
	char temp_label[1024];
	
	terminal_mode ^= 0x1;
	
	if(terminal_mode == TERM_MODE_SINGL)
	{
		sprintf(temp_label, "Mode: SGL");
		fl_set_object_label(main_frm->mode_lbl, temp_label);
		fl_set_object_color(main_frm->single_led, LED_COLOR_ON, LED_COLOR_OFF);
		fl_set_object_color(main_frm->multi_led, LED_COLOR_OFF, LED_COLOR_OFF);
	}
	else
	{
		sprintf(temp_label, "Mode: MLT");
		fl_set_object_label(main_frm->mode_lbl, temp_label);
		fl_set_object_color(main_frm->single_led, LED_COLOR_OFF, LED_COLOR_OFF);
		fl_set_object_color(main_frm->multi_led, LED_COLOR_ON, LED_COLOR_OFF);
	}
}

// //
// // GPS function
// //
// void use_gps_callback(FL_OBJECT *ob, long user_data)
// {
// 	//gpsd_fd = open_gpsd_socket();
// 	if(!gps_data)
// 	{
// 		gps_data = gps_open("127.0.0.1","2947");
// 		if (gps_data == NULL) 
// 		{
// 			fl_set_object_color(main_frm->gps_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
// 			error("Could not open GPS");
// 		}
// 		else
// 		{
// 			fl_set_object_color(main_frm->gps_lbl, SCREEN_COLOR_ON, SCREEN_COLOR_OFF);
// 		}
// 		//gps_set_raw_hook(gps_data, &update_gps_data);
// 	}
// 	else
// 	{
// 		gps_close(gps_data);
// 		gps_data = NULL;
// 		fl_set_object_color(main_frm->gps_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
// 		fl_set_object_color(main_frm->fix_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
// 		fl_set_object_lcolor(main_frm->gps_lat_lbl, SCREEN_COLOR_OFF);
// 		fl_set_object_lcolor(main_frm->gps_lon_lbl, SCREEN_COLOR_OFF);		
// 	}
// }

// //
// // Origin function
// //
// void set_origin_callback(FL_OBJECT *ob, long user_data)
// {
// 	message("Not yet implemented");
// 	//printf("exiting normally.\n");
// 	//exit(0);
// }

//
// Refresh function
//
void refresh_callback(FL_OBJECT *ob, long user_data)
{
	char temp_label[1024];
	
	// update the dial
	screen_refresh_period = fl_get_dial_value(main_frm->refresh_dial);
	screen_refresh_period += (double) user_data / 10.0;
	if (screen_refresh_period > 5)
		screen_refresh_period = 5;
	else if (screen_refresh_period < 0.05)
		screen_refresh_period = 0.05;

	fl_set_dial_value(main_frm->refresh_dial, screen_refresh_period);
	
	fl_set_timer(main_frm->refresh_timer,screen_refresh_period);
	
	// Update the screen refresh in frames
	// screen_refresh_in_frames = ceil(screen_refresh_period/2.6667e-3)+1;
	
	sprintf(temp_label, "Refresh interval: %.2f s", screen_refresh_period);
	fl_set_object_label(main_frm->refresh_lbl, temp_label);
}

//
// Refresh function
//
void record_callback(FL_OBJECT *ob, long user_data)
{	
	char temp_text[1024];
	
	if(power)
	{
		// Terminal mode leds
		if(record)
		{
			// @TODO write rest of the buffer to disk 
			
			// Turn off recording
			record = REC_OFF;
			rec_frame_counter = 0;
			
			// release the buffer
			free(fifo_buffer);
			fifo_buffer = NULL;
			fifo_output = NULL;
			fifo_ptr = NULL;
			
			// close the dumpfile
			fclose(dumpfile_id);
			dumpfile_id = NULL;

		}
		else
		{
			// Turn on recording
			if (open_dumpfile() == 0)
				record = REC_ON;
//			else
//				error("Error opening dumpfile");

		}
	}
}

int open_dumpfile()
{
	char  dumpfile_name[1024];
	char temp_label[1024];
			
	// create the dumpfile buffer
	// allocate memory for NO_FRAMES_DISK channes estimations 
	fifo_buffer = malloc(NO_ESTIMATES_DISK*CHANNEL_BUFFER_SIZE);
	fifo_ptr = fifo_buffer;
	
	if(fifo_buffer == NULL)
	{
		error( "Could not allocate memory for fifo2file_buffer");
		return -1;
	}
			
			// open the dumpfile
	time_t starttime_tmp;
	struct tm starttime;
	
	time(&starttime_tmp);
	localtime_r(&starttime_tmp,&starttime);
	snprintf(dumpfile_name,1024,"%sdata_term%d_idx%d_%d%0.2d%0.2d_%0.2d%0.2d%0.2d.EMOS",dumpfile_dir,terminal_idx,file_index,1900+starttime.tm_year, starttime.tm_mon+1, starttime.tm_mday, starttime.tm_hour, starttime.tm_min, starttime.tm_sec); 
	
	dumpfile_id = fopen(dumpfile_name,"w");
	if (dumpfile_id == NULL)
	{
		sprintf(temp_label,"Error opening dumpfile %s", dumpfile_name);
		error(temp_label);
		return -1;
	}
	
	snprintf(date_string,1024,"date: %d%0.2d%0.2d_%0.2d%0.2d%0.2d",1900+starttime.tm_year, starttime.tm_mon+1, starttime.tm_mday, starttime.tm_hour, starttime.tm_min, starttime.tm_sec); 
	fl_set_object_label(main_frm->date_lbl, date_string);
	sprintf(temp_label, "idx: %d", file_index);
	fl_set_object_label(main_frm->idx_lbl, temp_label);

	return 0;
}

//
// Prcesses all the new data in the FIFO
//
void new_data_callback(FL_OBJECT *ob, long user_data)
{
	int n_bytes = 0;
	char temp_text[1024];
	
	// Read data from FIFO
	n_bytes = rtf_read_all_at_once(fifo_fd, &data_buffer, CHANNEL_BUFFER_SIZE);
	if (n_bytes != CHANNEL_BUFFER_SIZE)
	{
		sprintf(temp_text, "Error reading FIFO.");
		fl_set_object_label(main_frm->msg_text, temp_text);
		fl_set_object_lcolor(main_frm->msg_text, SCREEN_COLOR_ON);
	}
	
	fifo_output = (fifo_read_emos*) data_buffer;
	
	if (terminal_mode == TERM_MODE_MULTI)
	{
/*		if (fifo_output->crc_status == 0)
		{
			memcpy(&file_index,fifo_output->chbch_pdu+sizeof(RTIME)+4,sizeof(int)); 
			record_multi = (int) fifo_output->chbch_pdu[16];
		}*/
		
		if (fifo_output->PHY_measurements.frame_tx[0] % REC_FRAMES_MAX < REC_FRAMES_PER_FILE)  
			record_multi = REC_ON;
		else 
			record_multi = REC_OFF;
		
		file_index = fifo_output->PHY_measurements.frame_tx[0]/REC_FRAMES_MAX % REC_FILE_IDX_MAX;
	}
		
	if((frame_counter % 1000) == 0)
	{
		printf("record = %d. record_multi = %d, terminal_mode = %d, frame_tx=%d, file_index = %d\n", record, record_multi, terminal_mode, fifo_output->PHY_measurements.frame_tx[0], file_index);
/*		printf("sizeof(struct gps_fix_t)=%d\n",sizeof(struct gps_fix_t));
		refresh_interface();*/
	}
		
	frame_counter++;

	if ((record==REC_ON && terminal_mode == TERM_MODE_SINGL) || (record==REC_ON && record_multi==REC_ON && terminal_mode == TERM_MODE_MULTI))
	{
		// Recording indicator ON
		fl_set_object_lcolor(main_frm->rec_lbl, SCREEN_COLOR_OFF);
		fl_set_object_color(main_frm->rec_lbl, SCREEN_COLOR_ON, SCREEN_COLOR_OFF);
		fl_set_object_lcolor(main_frm->n_recd_frames_lbl, SCREEN_COLOR_ON);
		fl_set_object_lcolor(main_frm->buffer_lbl, SCREEN_COLOR_ON);
		
		if (dumpfile_id == NULL)
		{
			error("Dumpfile not open");
		}
		else
		{
			// copy the data to the fifo buffer
			memcpy(fifo_ptr, data_buffer, CHANNEL_BUFFER_SIZE);
			fifo_ptr += CHANNEL_BUFFER_SIZE;
			rec_frame_counter++;
			
			sprintf(temp_text,"Buffer: %3d%%", ((rec_frame_counter%NO_ESTIMATES_DISK)*100/NO_ESTIMATES_DISK));
			fl_set_object_label(main_frm->buffer_lbl,temp_text);
			sprintf(temp_text,"Rec'd Frames: %d", rec_frame_counter);
			fl_set_object_label(main_frm->n_recd_frames_lbl,temp_text);
			
			// store data to disk
			if (rec_frame_counter%NO_ESTIMATES_DISK==0)
			{
				//reset stuff
				fifo_ptr = fifo_buffer;
				
				//flush buffer to disk
				printf("flushing buffer to disk\n");
				
				if (fwrite(fifo_buffer, sizeof(char), NO_ESTIMATES_DISK*CHANNEL_BUFFER_SIZE, dumpfile_id) != NO_ESTIMATES_DISK*CHANNEL_BUFFER_SIZE)
				{
					message("Error writing to dumpfile, stopping recording");
					record = REC_OFF;
				}
				// write GPS data to disk
				if (gps_data)
				{
					if (fwrite(&(gps_data->fix), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
					{
						message("Error writing to dumpfile, stopping recording");
						record = REC_OFF;
					}
				}
				else
				{
					message("WARNING: No GPS data available, storing dummy packet");
					if (fwrite(&(dummy_gps_data), sizeof(char), sizeof(struct gps_fix_t), dumpfile_id) != sizeof(struct gps_fix_t))
					{
						message("Error writing to dumpfile, stopping recording");
						record = REC_OFF;
					}
				}
			}
		}
	}
	else if (record==REC_ON && record_multi==REC_OFF && terminal_mode == TERM_MODE_MULTI)
	{
		// Recording indicator Ready
		fl_set_object_lcolor(main_frm->rec_lbl, SCREEN_COLOR_OFF);
		fl_set_object_color(main_frm->rec_lbl, SCREEN_COLOR_HL, SCREEN_COLOR_OFF);
		
		if (rec_frame_counter > 0)
		{
			// Turn off recording
			record = REC_OFF;
			rec_frame_counter = 0;
				
			// release the buffer
			free(fifo_buffer);
			fifo_buffer = NULL;
			fifo_output = NULL;
			fifo_ptr = NULL;
				
			// close the dumpfile
			fclose(dumpfile_id);
			dumpfile_id = NULL;
	
			// open another dumpfile
			if (open_dumpfile() == 0)
				record = REC_ON;
			else
				error("Error opening dumpfile");
		}
	}
	else //record==REC_OFF
	{
		// Recording indicator OFF
		fl_set_object_lcolor(main_frm->rec_lbl, SCREEN_COLOR_OFF);
		fl_set_object_color(main_frm->rec_lbl, SCREEN_COLOR_BG, SCREEN_COLOR_OFF);
		fl_set_object_lcolor(main_frm->n_recd_frames_lbl, SCREEN_COLOR_OFF);
		fl_set_object_lcolor(main_frm->buffer_lbl, SCREEN_COLOR_OFF);
	}
}

//
// Initializes the MAC and PHY vars
//
int mac_phy_init()
{
	int ioctl_result;
		
	// FILE *config;		--> already declared in config_vars.h
	// FILE *scenario;  --> already declared in config_vars.h
	
	char temp_text[1024];
	
	// Load configuration files
	if ((config = fopen("./config.cfg","r")) == NULL)
	{
		error("Openair configuration file <config.cfg> could not be found!");		
		emos_ready = EMOS_NOT_READY;
		return -1;
	}

	if ((scenario = fopen("./scenario.scn","r")) == NULL)
	{
		error("Openair scenario file <scenario.scn> could not be found!");		
		emos_ready = EMOS_NOT_READY;
		return -1;
	}
	
	// Load openair configuration
	PHY_config = (PHY_CONFIG *)&PHY_config_mem;
// 	PHY_vars = malloc(sizeof(PHY_VARS));
// 	mac_xface = malloc(sizeof(MAC_xface));
  
	reconfigure_MACPHY(scenario);
	
	//phy_init();
	//mac_init();
	
	// Load the configuration to the device driver
	ioctl_result = ioctl(openair_dev_fd, openair_DUMP_CONFIG,(char *)PHY_config);
	if (ioctl_result != 0)
	{
		error("Loading openair configuration in kernel space failed!");		
		emos_ready = EMOS_NOT_READY;
		return -1;
	}
	
	// Openair configuration
	fclose(config);
		
	// Openair scenario
	// fclose(scenario_fd);		--> already closed in reconfigure_MACPHY
	
	dump_config();
	return 0;
}
		
//
// Refresh function
//
void terminal_button_callback(FL_OBJECT *ob, long user_data)
{
	fl_set_button(main_frm->terminal_btn1,0);
	fl_set_button(main_frm->terminal_btn2,0);
	fl_set_button(main_frm->terminal_btn3,0);
	fl_set_button(main_frm->terminal_btn4,0);
	fl_set_button(ob,1);
	terminal_idx = user_data;
}

void rx_mode_button_callback(FL_OBJECT *ob, long user_data)
{
	fl_set_button(main_frm->single_btn,0);
	fl_set_button(main_frm->mmse_btn,0);
	fl_set_button(main_frm->ml_btn,0);
	fl_set_button(ob,1);
	switch (user_data) {
	case 0:
	  rxmode = SINGLE;
	  break;
	case 1:
	  rxmode = MMSE;
	  break;
	case 2:
	  rxmode = ML;
	  break;
	default:
	  rxmode = ML;
	}
}


//
// Time/Freq domain selector
//
void time_freq_callback(FL_OBJECT *ob, long user_data)
{
	fl_set_button(main_frm->time_domain_btn,0);
	fl_set_button(main_frm->freq_domain_btn,0);
	fl_set_button(ob,1);
	domain_selector = user_data;
}

//
// SNR/N0 selector
//
void noise_snr_callback(FL_OBJECT *ob, long user_data)
{
	fl_set_button(main_frm->snr_btn,0);
	fl_set_button(main_frm->n0_btn,0);
	fl_set_button(ob,1);
	noise_selector = user_data;
	printf("noise_selector = %d\n",noise_selector);
}

//
// Refresh function
//
void file_index_callback(FL_OBJECT *ob, long user_data)
{
	char temp_label[1024];
	
	// update the dial
	file_index = fl_get_dial_value(main_frm->file_index_dial);
	file_index += user_data;
	if (file_index > 100)
		file_index = 100;
	else if (file_index < 0)
		file_index = 0;

	fl_set_dial_value(main_frm->file_index_dial, file_index);
	
	sprintf(temp_label, "idx: %d", file_index);
	fl_set_object_label(main_frm->idx_lbl, temp_label);
}

void refresh_timer_callback(FL_OBJECT *ob, long user_data)
{
	refresh_interface();
	fl_set_timer(ob, screen_refresh_period);
}

void config_btn_callback(FL_OBJECT *ob, long user_data)
{
			
	config_wnd = fl_show_form(config_frm->config_dialog, FL_PLACE_HOTSPOT, FL_FULLBORDER, "Select Directory");
	fl_set_input(config_frm->dir_input,dumpfile_dir); 
	
}

void get_dir_callback(FL_OBJECT *ob, long user_data)
{
	const char *filename;
	filename = fl_get_input(config_frm->dir_input);	
	if (user_data==0)
		strcpy(dumpfile_dir, filename);
	printf("Directory = %s\n",dumpfile_dir);
	fl_hide_form(config_frm->config_dialog);
}	
/*
void get_gps_time_position(int socknum, char *buffer, int buffersize)
{
	dprintf(socknum, "dp\n");
	memset(buffer, 0, buffersize); 
	read(socknum, buffer, buffersize);
}

int open_gpsd_socket()
{
	struct sockaddr_in skin;
	struct servent *sp;
	int socknum = -1;
	
	if ((sp=getservbyname("gpsd","tcp"))==NULL) //get port number for gpsd daemon service
		error("get gpsd port number failed");
	if ((socknum=socket(AF_INET,SOCK_STREAM,0))<0)
		error("failed to create gpsd socket");
	
	memset(&skin,0,sizeof(struct sockaddr_in));
	
	skin.sin_family = AF_INET;
	skin.sin_port = sp->s_port;
	inet_aton("127.0.0.1",&(skin.sin_addr.s_addr));
	
	if (connect(socknum,&skin,sizeof(struct sockaddr_in))< 0) {
		error("Failed to connect to GPSD socket");
		return -1;
	}	else
		return socknum;

}

void update_gps_data(struct gps_data_t *gps_data, char *buf)
{
	printf("%s\n",buf);	
	printf("GPS online = %f, mode = %d, lat = %f, lon = %f\n",gps_data->online,gps_data->fix.mode,gps_data->fix.latitude,gps_data->fix.longitude);
}	

*/
