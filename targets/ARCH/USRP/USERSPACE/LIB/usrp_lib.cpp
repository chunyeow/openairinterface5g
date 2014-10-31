/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
    included in this distribution in the file called "COPYING". If not, 
    see <http://www.gnu.org/licenses/>.

   Contact Information
   OpenAirInterface Admin: openair_admin@eurecom.fr
   OpenAirInterface Tech : openair_tech@eurecom.fr
   OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
   Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/** usrp_lib.cpp
 *
 * Author: HongliangXU : hong-liang-xu@agilent.com
 */

#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <uhd/utils/thread_priority.hpp>
#include <uhd/usrp/multi_usrp.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <complex>
#include <fstream>
#include <cmath>
#include "common_lib.h"

typedef struct
{

  // --------------------------------
  // variables for USRP configuration
  // --------------------------------
  uhd::usrp::multi_usrp::sptr usrp;
  //uhd::usrp::multi_usrp::sptr rx_usrp;

  //create a send streamer and a receive streamer
  uhd::tx_streamer::sptr tx_stream;
  uhd::rx_streamer::sptr rx_stream;

  uhd::tx_metadata_t tx_md;
  uhd::rx_metadata_t rx_md;

  uhd::time_spec_t tm_spec;
  //setup variables and allocate buffer
  uhd::async_metadata_t async_md;

  double sample_rate;
  // time offset between transmiter timestamp and receiver timestamp;
  double tdiff;
  // use usrp_time_offset to get this value
  int tx_forward_nsamps; //166 for 20Mhz


  // --------------------------------
  // Debug and output control
  // --------------------------------
  int num_underflows;
  int num_overflows;
  int num_seq_errors;

  int64_t tx_count;
  int64_t rx_count;
  openair0_timestamp rx_timestamp;

} usrp_state_t;


static int trx_usrp_start(openair0_device *device)
{
  usrp_state_t *s = (usrp_state_t*)device->priv;

  // init recv and send streaming
  uhd::stream_cmd_t cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
  cmd.time_spec = s->usrp->get_time_now() + uhd::time_spec_t(0.01);
  cmd.stream_now = false; // start at constant delay
  s->rx_stream->issue_stream_cmd(cmd);

  s->tx_md.time_spec = s->usrp->get_time_now() + uhd::time_spec_t(1-(double)s->tx_forward_nsamps/s->sample_rate);
  s->tx_md.has_time_spec = true;
  s->tx_md.start_of_burst = true;
  s->tx_md.end_of_burst = false;


  s->rx_count = 0;
  s->tx_count = 0;
  s->rx_timestamp = 0;
}

static void trx_usrp_end(openair0_device *device)
{
  usrp_state_t *s = (usrp_state_t*)device->priv;

  s->rx_stream->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);

	//send a mini EOB packet
	s->tx_md.end_of_burst = true;
	s->tx_stream->send("", 0, s->tx_md);
	s->tx_md.end_of_burst = false;
}
static void trx_usrp_write(openair0_device *device, openair0_timestamp timestamp, const void **buff, int nsamps, int flags)
{
  usrp_state_t *s = (usrp_state_t*)device->priv;

  s->tx_md.time_spec = uhd::time_spec_t::from_ticks(timestamp, s->sample_rate);
  if(flags)
    s->tx_md.has_time_spec = true;
  else
    s->tx_md.has_time_spec = false;
  s->tx_stream->send(buff, nsamps, s->tx_md);
  s->tx_md.start_of_burst = false;
}

static int trx_usrp_read(openair0_device *device, openair0_timestamp *ptimestamp, void **buff, int nsamps, int cc)
{

  usrp_state_t *s = (usrp_state_t*)device->priv;

  int samples_received[cc],i;
  
  samples_received = s->rx_stream->recv(buff, nsamps, s->rx_md);

  //handle the error code
  switch(s->rx_md.error_code){
  case uhd::rx_metadata_t::ERROR_CODE_NONE:
    break;
  case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW:
    printf("[recv] USRP RX OVERFLOW!\n");
    s->num_overflows++;
    break;
  case uhd::rx_metadata_t::ERROR_CODE_TIMEOUT:
    printf("[recv] USRP RX TIMEOUT!\n");
    break;
  default:
    printf("[recv] Unexpected error on RX, Error code: 0x%x\n",s->rx_md.error_code);
    break;
  }
  s->rx_count += nsamps;
  s->rx_timestamp = s->rx_md.time_spec.to_ticks(s->sample_rate);
  *ptimestamp = s->rx_timestamp;
  return samples_received;
}

openair0_timestamp get_usrp_time(openair0_device *device) 
{
 
  usrp_state_t *s = (usrp_state_t*)device->priv;
  
  return s->usrp->get_time_now().to_ticks(s->sample_rate);
} 

static bool is_equal(double a, double b)
{
  return std::fabs(a-b) < std::numeric_limits<double>::epsilon();
}

int openair0_set_frequencies(openair0_device* device, openair0_config_t *openair0_cfg) {

  usrp_state_t *s = (usrp_state_t*)device->priv;

  s->usrp->set_tx_freq(openair0_cfg[0].tx_freq[0]);
  s->usrp->set_rx_freq(openair0_cfg[0].rx_freq[0]);

  return(0);
  
}

int openair0_set_gains(openair0_device* device, openair0_config_t *openair0_cfg) {

  usrp_state_t *s = (usrp_state_t*)device->priv;

  s->usrp->set_tx_gain(openair0_cfg[0].tx_gain[0]);
  s->usrp->set_rx_gain(openair0_cfg[0].rx_gain[0]);
  return(0);
}
 
int openair0_device_init(openair0_device* device, openair0_config_t *openair0_cfg)
{
  uhd::set_thread_priority_safe(1.0);
  usrp_state_t *s = (usrp_state_t*)malloc(sizeof(usrp_state_t));
  memset(s, 0, sizeof(usrp_state_t));

  // Initialize USRP device
  std::string args = "type=b200";
  uhd::device_addrs_t device_adds = uhd::device::find(args);
  size_t i;

  if(device_adds.size() == 0)
  {
    std::cerr<<"No USRP Device Found. " << std::endl;
    free(s);
    return -1;
  }
  s->usrp = uhd::usrp::multi_usrp::make(args);

  // lock mboard clocks
  s->usrp->set_clock_source("internal");
  // set master clock rate and sample rate for tx & rx for streaming
  s->usrp->set_master_clock_rate(30.72e6);
  s->usrp->set_rx_rate(openair0_cfg[0].sample_rate);
  s->usrp->set_tx_rate(openair0_cfg[0].sample_rate);

  for(i=0;i<usrp->get_rx_num_channels();i++) {
    if (i<openair0_cfg[0].rx_num_channels) {
      s->usrp->set_rx_freq(openair0_cfg[0].rx_freq[i]);
      s->usrp->set_rx_gain(openair0_cfg[0].rx_gain[i]);
    }
  }
  for(i=0;i<usrp->get_tx_num_channels();i++) {
    if (i<openair0_cfg[0].tx_num_channels) {
      s->usrp->set_tx_freq(openair0_cfg[0].tx_freq[i]);
      s->usrp->set_tx_gain(openair0_cfg[0].tx_gain[i]);
    }
  }
  s->usrp->set_tx_bandwidth(openair0_cfg[0].tx_bw);
  s->usrp->set_rx_bandwidth(openair0_cfg[0].rx_bw);

  // create tx & rx streamer
  uhd::stream_args_t stream_args_rx("sc16", "sc16");
  uhd::stream_args_t stream_args_tx("sc16", "sc16");
  for (i = 0; i<  openair0_cfg[0].rx_num_channels(); i++)
      stream_args_rx.channels.push_back(i);
  for (i = 0; i<  openair0_cfg[0].tx_num_channels(); i++)
      stream_args_tx.channels.push_back(i);

  s->tx_stream = s->usrp->get_tx_stream(stream_args_tx);
  s->rx_stream = s->usrp->get_rx_stream(stream_args_rx);

  s->usrp->set_time_now(uhd::time_spec_t(0.0));

  // display USRP settings
  std::cout << std::endl<<boost::format("Actual TX sample rate: %fMSps...") % (s->usrp->get_tx_rate()/1e6) << std::endl;
  std::cout << boost::format("Actual RX sample rate: %fMSps...") % (s->usrp->get_rx_rate()/1e6) << std::endl;
  
  std::cout << boost::format("Actual TX frequency: %fGHz...") % (s->usrp->get_tx_freq()/1e9) << std::endl;
  std::cout << boost::format("Actual RX frequency: %fGHz...") % (s->usrp->get_rx_freq()/1e9) << std::endl;
  
  std::cout << boost::format("Actual TX gain: %f...") % (s->usrp->get_tx_gain()) << std::endl;
  std::cout << boost::format("Actual RX gain: %f...") % (s->usrp->get_rx_gain()) << std::endl;
  
  std::cout << boost::format("Actual TX bandwidth: %fM...") % (s->usrp->get_tx_bandwidth()/1e6) << std::endl;
  std::cout << boost::format("Actual RX bandwidth: %fM...") % (s->usrp->get_rx_bandwidth()/1e6) << std::endl;
  
  std::cout << boost::format("Actual TX antenna: %s...") % (s->usrp->get_tx_antenna()) << std::endl;
  std::cout << boost::format("Actual RX antenna: %s...") % (s->usrp->get_rx_antenna()) << std::endl;
  
  std::cout << boost::format("Device timestamp: %f...") % (s->usrp->get_time_now().get_real_secs()) << std::endl;

  device->priv = s;
  device->trx_start_func = trx_usrp_start;
  device->trx_end_func   = trx_usrp_end;
  device->trx_read_func  = trx_usrp_read;
  device->trx_write_func = trx_usrp_write;

  s->sample_rate = openair0_cfg[0].sample_rate;
  // TODO:
  // init tx_forward_nsamps based usrp_time_offset ex
  if(is_equal(s->sample_rate, (double)30.72e6))
    s->tx_forward_nsamps  = 176;
  if(is_equal(s->sample_rate, (double)15.36e6))
    s->tx_forward_nsamps = 90;
  if(is_equal(s->sample_rate, (double)7.68e6))
    s->tx_forward_nsamps = 50;

  return 0;
}
