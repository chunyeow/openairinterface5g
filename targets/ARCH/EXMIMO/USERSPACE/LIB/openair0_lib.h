/** openair0_lib : API to interface with ExpressMIMO kernel driver
 * 
 *  Authors: Matthias Ihmig <matthias.ihmig@mytum.de>, 2013
 *           Raymond Knopp <raymond.knopp@eurecom.fr>
 * 
 *  Changelog:
 *  28.01.2013: Initial version
 */

#ifndef __OPENAIR0_LIB_H__
#define __OPENAIR0_LIB_H__

#include "pcie_interface.h"
#include "openair_device.h"

// Use this to access shared memory (configuration structures, adc/dac data buffers, ...)
// contains userspace pointers
extern exmimo_pci_interface_bot_virtual_t openair0_exmimo_pci[MAX_CARDS];

extern int openair0_fd;

extern int openair0_num_antennas[MAX_CARDS];

extern int openair0_num_detected_cards;

// opens device and mmaps kernel memory and calculates interface and system_id userspace pointers
// return 0 on success
int openair0_open(void);

// close device and unmaps kernel memory
// return 0 on success
int openair0_close(void);

// trigger config update on card
// return 0 on success
int openair0_dump_config(int card);

// triggers recording of exactly 1 frame
// in case of synchronized multiple cards, send this only to the master card
// return 0 on success
int openair0_get_frame(int card);

// starts continuous acquisition/transmission
// in case of synchronized multiple cards, send this only to the master card
// return 0 on success
int openair0_start_rt_acquisition(int card);

// stops continuous acquitision/transmission
// return 0 on success
int openair0_stop(int card);

#endif
