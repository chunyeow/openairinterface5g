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

/** bladerf_lib.c
 *
 * Author: navid nikaein
 */


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "bladerf_lib.h"
#include "common_lib.h"
#include "log.h"

#define SAMPLE_LEN samples_per_frame*sizeof(int32_t)

int num_devices=0;
/*These items configure the underlying asynch stream used by the the sync interface. 
 */
unsigned int num_buffers;
unsigned int buffer_size;
unsigned int num_transfers;
unsigned int timeout_ms;


int trx_brf_init(openair0_device *openair0) {
  
}

struct bladerf * open_bladerf_from_serial(const char *serial) {

  int status;
  struct bladerf *dev;
  struct bladerf_devinfo info;
  /* Initialize all fields to "don't care" wildcard values.
   *
   * Immediately passing this to bladerf_open_with_devinfo() would cause
   * libbladeRF to open any device on any available backend. */
  bladerf_init_devinfo(&info);
  /* Specify the desired device's serial number, while leaving all other
   * fields in the info structure wildcard values */
  strncpy(info.serial, serial, BLADERF_SERIAL_LENGTH - 1);
  info.serial[BLADERF_SERIAL_LENGTH - 1] = '\0';
  status = bladerf_open_with_devinfo(&dev, &info);
  
  if (status == BLADERF_ERR_NODEV) {
    printf("No devices available with serial=%s\n", serial);
    return NULL;
  } else if (status != 0) {
    fprintf(stderr, "Failed to open device with serial=%s (%s)\n", serial, bladerf_strerror(status));
    return NULL;
  } else {
    return dev;
  }
}
 
 
 


int trx_brf_start(openair0_device *openair0) {

}

int trx_brf_write(openair0_device *device,openair0_timestamp *ptimestamp, void **buff, int nsamps, int cc) {
  int status;
  struct bladerf_metadata meta;
  int16_t zeros[] = { 0, 0, 0, 0 };
  struct bladerf *dev =  (struct bladerf*) device->priv;
  
  /* BRF has only 1 rx/tx chaine : is it correct? */
  void *sample = (void*)buff[0];
  
  /* Retrieve the current timestamp */
  if ((status=bladerf_get_timestamp(dev, BLADERF_MODULE_TX, &meta.timestamp)) != 0) {
    fprintf(stderr,"Failed to get current RX timestamp: %s\n",bladerf_strerror(status));
  } else {
    *ptimestamp = meta.timestamp;
    printf("Current TX timestamp: 0x%016"PRIx64"\n", meta.timestamp);
  }
  
  meta.flags |= BLADERF_META_FLAG_RX_NOW;
  
  status = bladerf_sync_tx(dev, sample, nsamps, &meta, timeout_ms);
  if (status != 0) {
    fprintf(stderr, "Failed to TX sample: %s\n", bladerf_strerror(status));
    brf_error(status);
  }

  
}

int trx_brf_read(openair0_device *device, openair0_timestamp *ptimestamp, void **buff, int nsamps, int cc) {
  int status, ret;
  struct bladerf_metadata meta;
  unsigned int i;
  struct bladerf *dev =  (struct bladerf*) device->priv;
  /* BRF has only onerx/tx chain: is it correct? */
  void *sample = (void*)buff[0];
  
  /* Retrieve the current timestamp */
  if ((status=bladerf_get_timestamp(dev, BLADERF_MODULE_RX, &meta.timestamp)) != 0) {
    fprintf(stderr,"Failed to get current RX timestamp: %s\n",bladerf_strerror(status));
  } else {
    *ptimestamp = meta.timestamp;
    printf("Current RX timestamp: 0x%016"PRIx64"\n", meta.timestamp);
  }
  
  meta.flags |= BLADERF_META_FLAG_RX_NOW;
  //fflush(stdout);

  status = bladerf_sync_rx(dev, sample, nsamps, &meta, timeout_ms);

  /*if (meta.actual_count < nsamps ) {
    printf("[BRF][RX] received %d samples out of %d\n", meta.actual_count, nsamps);
    }*/

  if (status != 0) {
    fprintf(stderr, "RX failed: %s\n", bladerf_strerror(status));
  } else if (meta.status & BLADERF_META_STATUS_OVERRUN) {
    fprintf(stderr, "Overrun detected in RX. %u valid samples were read \n", meta.actual_count);
  } else if (meta.status & BLADERF_META_STATUS_UNDERRUN) {
    fprintf(stderr, "Underrun detected in RX. %u valid samples were read \n", meta.actual_count);
  }else {
    printf("Got %u samples at t=0x%016"PRIx64"\n",  meta.actual_count, meta.timestamp);
  }

  return meta.actual_count;

}

int trx_brf_end(openair0_device *device) {

  int status;
  struct bladerf *dev =  (struct bladerf*) device->priv;
  // Disable RX module, shutting down our underlying RX stream
  if ((status=bladerf_enable_module(device->priv, BLADERF_MODULE_RX, false))  != 0) {
    fprintf(stderr, "Failed to disable RX module: %s\n", bladerf_strerror(status));
  }
  if ((status=bladerf_enable_module(device->priv, BLADERF_MODULE_TX, false))  != 0) {
    fprintf(stderr, "Failed to disable TX module: %s\n",  bladerf_strerror(status));
  }
  bladerf_close(dev);
  return 0;
}

//int openair0_device_brf_init(openair0_device *device, openair0_config_t *openair0_cfg) {
int openair0_device_init(openair0_device *device, openair0_config_t *openair0_cfg) {

  int status;
  int card=0;
  //struct bladerf_version *version;
  //printf("Opening the brf device (version %s)...\n", bladerf_version(version));
  
  // opaque data struct
  struct bladerf *dev;// =  (struct bladerf*)malloc(sizeof(struct bladerf));
  //memset(dev, 0, sizeof(struct bladerf));

  if ((status=bladerf_open(&dev, "")) != 0 ) {
    fprintf(stderr,"Failed to open brf device: %s\n",bladerf_strerror(status));
    brf_error(status);
  }
  printf("[BRF] device speed is %d\n",bladerf_device_speed(dev) );

  // RX
  if ((status=bladerf_set_frequency(dev, BLADERF_MODULE_RX, openair0_cfg[card].rx_freq[0])) != 0){
    fprintf(stderr,"Failed to set RX frequency: %s\n",bladerf_strerror(status));
    brf_error(status);
  }
  if ((status=bladerf_set_sample_rate(dev, BLADERF_MODULE_RX, openair0_cfg[card].sample_rate, NULL)) != 0){
    fprintf(stderr,"Failed to set RX sample rate: %s\n", bladerf_strerror(status));
    brf_error(status);
  }
  if ((status=bladerf_set_bandwidth(dev, BLADERF_MODULE_RX, openair0_cfg[card].rx_bw, NULL)) != 0){
    fprintf(stderr,"Failed to set RX bandwidth: %s\n", bladerf_strerror(status));
    brf_error(status);
  }
  if ((status=bladerf_set_gain(dev, BLADERF_MODULE_RX, openair0_cfg[card].rx_gain[0])) != 0) {
    fprintf(stderr,"Failed to set RX gain: %s\n",bladerf_strerror(status));
    brf_error(status);
  }

  /* Configure the device's RX module for use with the sync interface.
   * SC16 Q11 samples *with* metadata are used. */
  if ((status=bladerf_sync_config(dev, BLADERF_MODULE_RX, BLADERF_FORMAT_SC16_Q11_META,num_buffers,buffer_size,num_transfers,timeout_ms)) != 0 ) {
    fprintf(stderr,"Failed to configure RX sync interface: %s\n", bladerf_strerror(status));
     brf_error(status);
  }

   /* We must always enable the RX module after calling bladerf_sync_config(), and 
    * before  attempting to RX samples via  bladerf_sync_rx(). */
  if ((status=bladerf_enable_module(dev, BLADERF_MODULE_RX, true)) != 0) {
    fprintf(stderr,"Failed to enable RX module: %s\n", bladerf_strerror(status));
    brf_error(status);
  }

  // TX
  if ((status=bladerf_set_frequency(dev, BLADERF_MODULE_TX, openair0_cfg[card].tx_freq[0])) != 0){
    fprintf(stderr,"Failed to set TX frequency: %s\n",bladerf_strerror(status));
    brf_error(status);
  }
  if ((status=bladerf_set_sample_rate(dev, BLADERF_MODULE_TX, openair0_cfg[card].sample_rate, NULL)) != 0){
    fprintf(stderr,"Failed to set TX sample rate: %s\n", bladerf_strerror(status));
    brf_error(status);
  }
  if ((status=bladerf_set_bandwidth(dev, BLADERF_MODULE_TX,openair0_cfg[card].tx_bw, NULL)) != 0){
    fprintf(stderr, "Failed to set RX bandwidth: %s\n", bladerf_strerror(status));
    brf_error(status);
  }
  if ((status=bladerf_set_gain(dev, BLADERF_MODULE_TX, openair0_cfg[card].tx_gain[0])) != 0) {
    fprintf(stderr,"Failed to set TX gain: %s\n",bladerf_strerror(status));
    brf_error(status);
  }

  /* Configure the device's TX module for use with the sync interface.
   * SC16 Q11 samples *with* metadata are used. */
  if ((status=bladerf_sync_config(dev, BLADERF_MODULE_TX,BLADERF_FORMAT_SC16_Q11_META,num_buffers,buffer_size,num_transfers,timeout_ms)) != 0 ) {
    fprintf(stderr,"Failed to configure TX sync interface: %s\n", bladerf_strerror(status));
     brf_error(status);
  }

   /* We must always enable the TX module after calling bladerf_sync_config(), and 
    * before  attempting to TX samples via  bladerf_sync_tx(). */
  if ((status=bladerf_enable_module(dev, BLADERF_MODULE_TX, true)) != 0) {
    fprintf(stderr,"Failed to enable TX module: %s\n", bladerf_strerror(status));
    brf_error(status);
  }

  num_buffers = 16;
  buffer_size = openair0_cfg[card].samples_per_packet*sizeof(int32_t);
  num_transfers = 8; // ? device->openair0_cfg.samples_per_packets
  timeout_ms = 1;


  bladerf_log_set_verbosity(get_brf_log_level(openair0_cfg[card].log_level));
  
  printf("BLADERF: Initializing openair0_device\n");
  device->priv           = dev; 
  device->Mod_id         = num_devices++;
  device->trx_start_func = trx_brf_start;
  device->trx_end_func   = trx_brf_end;
  device->trx_read_func  = trx_brf_read;
  device->trx_write_func = trx_brf_write;
  memcpy((void*)&device->openair0_cfg,(void*)openair0_cfg,sizeof(openair0_config_t));
}

void brf_error(int status) {
  
  exit(-1);
}

int get_brf_log_level(int log_level){

  int level=BLADERF_LOG_LEVEL_INFO;
  
  switch(log_level) {
  case LOG_DEBUG:
    level=BLADERF_LOG_LEVEL_DEBUG;
    break;
  case LOG_INFO:
    level= BLADERF_LOG_LEVEL_INFO;
    break;
  case LOG_WARNING:
    level=BLADERF_LOG_LEVEL_WARNING;
    break;
  case LOG_ERR:
    level=BLADERF_LOG_LEVEL_ERROR;
    break;
  case LOG_CRIT:
    level=BLADERF_LOG_LEVEL_CRITICAL;
    break;
  case LOG_EMERG:
    level = BLADERF_LOG_LEVEL_SILENT;
    break;
  default:
    break;
  }
  return level;
}
