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
 
 /** common_lib.h
 *
 * Author: HongliangXU : hong-liang-xu@agilent.com
 */
#ifndef COMMON_LIB_H
#define COMMON_LIB_H
#include <stdint.h>

typedef int64_t openair0_timestamp;
typedef struct openair0_device_t openair0_device;
/* structrue holds the parameters to configure USRP devices
 */

#ifndef EXMIMO 
#define MAX_CARDS 1
#endif

typedef enum {
  max_gain=0,med_gain,byp_gain
} rx_gain_t;


typedef struct {
  /* Module ID for this configuration */
  int Mod_id;
  /* the sample rate for both transmit and receive. */
  double sample_rate;
  /* number of RX channels (=RX antennas) */
  int rx_num_channels;
  /* number of TX channels (=TX antennas) */
  int tx_num_channels;
  /* center frequency in Hz for RX */
  double rx_freq[4];
  /* center frequency in Hz for TX */
  double tx_freq[4];
  /* mode for rxgain (ExpressMIMO2)*/
  rx_gain_t rxg_mode[4];
  /* gain for RX in dB */
  double rx_gain[4];
  /* gain for TX in dB */
  double tx_gain[4];
  /* RX bandwidth in Hz */
  double rx_bw;
  /* TX bandwidth in Hz */
  double tx_bw;
  /* RRH IP addr for Ethernet interface */
  char *rrh_ip;
  /* RRH port number for Ethernet interface */
  int rrh_port;
} openair0_config_t;

typedef struct {
  /* card id */
  int card;
  /* rf chain id */
  int chain;
} openair0_rf_map;



struct openair0_device_t {
  /* Module ID of this device */
  int Mod_id;

  /* RF frontend parameters set by application */
  openair0_config_t openair0_cfg;
  
  /* Can be used by driver to hold internal structure*/
  void *priv;

  /* Functions API, which are called by the application*/

  /* Called to start the transceiver. Return 0 if OK, < 0 if error */
  int (*trx_start_func)(openair0_device *device);

  /* Write 'nsamps' samples on each channel from buffers. buff[0] is the array for
   * the first channel. timestamp if the time (in samples) at which the first sample
   * MUST be sent
   * use flags = 1 to send as timestamp specfied*/
  void (*trx_write_func)(openair0_device *device, openair0_timestamp timestamp, const void *buff, int nsamps, int flags);

  /* Read 'nsamps' samples from each channel to buffers. buff[0] is the array for
   * the first channel. *ptimestamp is the time at which the first sample
   * was received.
   * Return the number of sample read */
  int (*trx_read_func)(openair0_device *device, openair0_timestamp *ptimestamp, void *buff, int nsamps);

  /* Terminate operation of the transceiver -- free all associated resources */
  void (*trx_end_func)(openair0_device *device);
};


#ifdef __cplusplus
extern "C"
{
/* return 0 if OK, < 0 if error */
int openair0_device_init(openair0_device* device, openair0_config_t *openair0_cfg);
openair0_timestamp get_usrp_time(openair0_device *device);
  int openair0_set_frequencies(openair0_device* device, openair0_config_t *openair0_cfg);

int openair0_set_gains(openair0_device* device, openair0_config_t *openair0_cfg);
}
#else
int openair0_device_init(openair0_device* device, openair0_config_t *openair0_cfg);
openair0_timestamp get_usrp_time(openair0_device *device);
int openair0_set_frequencies(openair0_device* device, openair0_config_t *openair0_cfg);
int openair0_set_gains(openair0_device* device, openair0_config_t *openair0_cfg);
#endif

#endif // COMMON_LIB_H
 
