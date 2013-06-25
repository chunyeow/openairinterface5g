/***************************************************************************
                          mRALlte_proto.h  -  description
                             -------------------
    copyright            : (C) 2005, 2007 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
 ***************************************************************************
  Dummy A21_MT_RAL_UMTS prototypes
 ***************************************************************************/
#ifndef __RAL_LTE_PROTO_H__
#define __RAL_LTE_PROTO_H__




//mRALuD_NAS.c
int IAL_decode_NAS_message(void);
int IAL_process_DNAS_message(int ioctl_obj, int ioctl_cmd, int ioctl_cellid);

//mRALuD_process.c
void IAL_NAS_measures_init(void);
void IAL_NAS_measures_update(int i);
void IAL_integrate_measure(int measure, int i);
void rallte_NAS_measures_polling(void);
int  rallte_NAS_corresponding_cell(int req_index);
void rallte_verifyPendingConnection(void);

#endif

