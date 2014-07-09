/** usrp_lib.h
 *
 * Author: HongliangXU : hong-liang-xu@agilent.com
 */
#ifndef USRP_LIB_H
#define USRP_LIB_H

typedef int64_t openair0_timestamp;
typedef struct openair0_device_t openair0_device;
/* structrue holds the parameters to configure USRP devices
 */
typedef struct {
  /* the sample rate for both transmit and receive. */
  double sample_rate;
  /* number of RX channels (=RX antennas) */
  int rx_num_channels;
  /* number of TX channels (=TX antennas) */
  int tx_num_channels;
  /* center frequency in Hz for RX */
  double rx_freq;
  /* center frequency in Hz for TX */
  double tx_freq;
  /* gain for RX in dB */
  double rx_gain;
  /* gain for TX in dB */
  double tx_gain;
  /* RX bandwidth in Hz */
  double rx_bw;
  /* TX bandwidth in Hz */
  double tx_bw;
} openair0_config_t;

struct openair0_device_t {
  /* USRP RF frontend parameters set by application */
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
}
#endif

#endif // USRP_LIB_H
