/*! \file extern.h
* \brief specifies the extern variables for phy emulation
* \author Navid Nikaein and Raymomd Knopp and Hicham Anouar
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr
*/ 

#ifndef __BYPASS_SESSION_LAYER_EXTERN_H__
#    define __BYPASS_SESSION_LAYER_EXTERN_H__

#ifndef USER_MODE
#include <rtai_posix.h>
#else
#include <pthread.h>
#endif //RTAI_ENABLED

extern unsigned char Emulation_status;
extern unsigned char emu_tx_status;
extern unsigned char emu_rx_status;
//extern unsigned int Master_list;
//extern unsigned short Master_id;
//extern unsigned int Is_primary_master;

extern pthread_mutex_t emul_low_mutex;
extern pthread_cond_t emul_low_cond;
extern char emul_low_mutex_var;
extern pthread_mutex_t Tx_mutex;
extern pthread_cond_t Tx_cond;
extern char Tx_mutex_var;


extern int (*rx_handler) (unsigned char,char*,int);
extern int (*tx_handler) (unsigned char,char*, unsigned int*, unsigned int*);

extern eNB_transport_info_t eNB_transport_info[NUMBER_OF_eNB_MAX];
extern u16 eNB_transport_info_TB_index[NUMBER_OF_eNB_MAX];

extern UE_transport_info_t UE_transport_info[NUMBER_OF_UE_MAX];
extern u16 UE_transport_info_TB_index[NUMBER_OF_UE_MAX];

extern UE_cntl ue_cntl_delay[NUMBER_OF_UE_MAX][2];

#endif
