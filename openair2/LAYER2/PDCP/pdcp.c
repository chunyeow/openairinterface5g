/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file pdcp.c
* \brief pdcp interface with RLC
* \author  Lionel GAUTHIER and Navid Nikaein
* \date 2009
* \version 0.5

*/

#define PDCP_C
//#include "rtos_header.h"
#ifndef USER_MODE
#include <rtai_fifos.h>
#endif
#include "pdcp.h"
#include "LAYER2/RLC/rlc.h"
#include "LAYER2/MAC/extern.h"

//#include "mpls.h"
//#include "w3g4free_extern.h"

#define PDCP_DATA_REQ_DEBUG 1
#define PDCP_DATA_IND_DEBUG 1

//#define IDROMEL_NEMO 1

extern rlc_op_status_t rlc_data_req     (module_id_t, rb_id_t, mui_t, confirm_t, sdu_size_t, mem_block_t*);

//-----------------------------------------------------------------------------
void
pdcp_data_req (module_id_t module_idP, rb_id_t rab_idP, sdu_size_t data_sizeP, char* sduP)
{
//-----------------------------------------------------------------------------

    mem_block_t      *new_sdu = NULL;
    //    int i;

    if ((data_sizeP > 0)) {
      if(data_sizeP > MAX_IP_PACKET_SIZE){
	msg("[PDCP] REQ FOR  SIZE %d !!!Abort\n",data_sizeP);
	mac_xface->macphy_exit("");
      }
        new_sdu = get_free_mem_block (data_sizeP);
        if (new_sdu) {


#ifdef PDCP_DATA_REQ_DEBUG
	  msg("[PDCP] TTI %d, INST %d: PDCP_DATA_REQ size %d RAB %d:\n",Mac_rlc_xface->frame,module_idP,data_sizeP,rab_idP);

	  //	  for (i=0;i<20;i++)
	  //	    msg("%02X.",((unsigned char*)sduP)[i]);
	  //	  msg("\n");
#endif //PDCP_DATA_REQ_DEBUG
            // PROCESS OF DECOMPRESSION HERE:
            memcpy (&new_sdu->data[0], sduP, data_sizeP);

	    rlc_data_req(module_idP, rab_idP, RLC_MUI_UNDEFINED, RLC_SDU_CONFIRM_NO, data_sizeP, new_sdu);
	    if(Mac_rlc_xface->Is_cluster_head[module_idP]==1){
	      Pdcp_stats_tx[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]++;
	      Pdcp_stats_tx_bytes[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]+=data_sizeP;
	    }
	    else{
	      Pdcp_stats_tx[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]++;
	      Pdcp_stats_tx_bytes[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]+=data_sizeP;

	    }

        } else {
	  msg("[PDCP][RAB %d][ERROR] PDCP_DATA_REQ OUT OF MEMORY\n", rab_idP);
        }
	//	free_mem_block (sduP);

    } else {
            msg("[PDCP][RAB %d][ERROR] PDCP_DATA_REQ SDU SIZE %d\n", rab_idP, data_sizeP);
    }
}
//-----------------------------------------------------------------------------
void
pdcp_data_ind (module_id_t module_idP, rb_id_t rab_idP, sdu_size_t data_sizeP, mem_block_t * sduP)
{
  //-----------------------------------------------------------------------------
  mem_block_t      *new_sdu = NULL;
  int i;

  if ((data_sizeP > 0)) {

    //   if(Mac_rlc_xface->Is_cluster_head[0]==1 && Mac_rlc_xface->frame%10==0)
    //msg("[PDCP][RAB %d][INST %d] PDCP_DATA_IND size %d\n", rab_idP,module_idP,data_sizeP);

#ifdef PDCP_DATA_IND_DEBUG
    msg("[PDCP][RAB %d][INST %d] TTI %d PDCP_DATA_IND size %d\n",
	rab_idP,module_idP,Mac_rlc_xface->frame,data_sizeP);

    for (i=0;i<20;i++)
      msg("%02X.",(unsigned char)sduP->data[i]);
    msg("\n");

#endif //PDCP_DATA_IND_DEBUG

    new_sdu = get_free_mem_block (data_sizeP + sizeof (pdcp_data_ind_header_t));

    if (new_sdu) {
      memset (new_sdu->data, 0, sizeof (pdcp_data_ind_header_t));
      ((pdcp_data_ind_header_t *) new_sdu->data)->rb_id     = rab_idP;
      ((pdcp_data_ind_header_t *) new_sdu->data)->data_size = data_sizeP;

      // Here there is no virtualization possible
#ifdef IDROMEL_NEMO
      if (Mac_rlc_xface->Is_cluster_head[module_idP] == 0)
	((pdcp_data_ind_header_t *) new_sdu->data)->inst = rab_idP/8;
      else
	((pdcp_data_ind_header_t *) new_sdu->data)->inst = 0;

#else
      ((pdcp_data_ind_header_t *) new_sdu->data)->inst = module_idP;
#endif

      // PROCESS OF DECOMPRESSION HERE:
      memcpy (&new_sdu->data[sizeof (pdcp_data_ind_header_t)], &sduP->data[0], data_sizeP);
      list_add_tail_eurecom (new_sdu, &pdcp_sdu_list);
      if(Mac_rlc_xface->Is_cluster_head[module_idP]==1){
	Pdcp_stats_rx[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]++;
	Pdcp_stats_rx_bytes[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]+=data_sizeP;
      }
      else{
	Pdcp_stats_rx[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]++;
	Pdcp_stats_rx_bytes[module_idP][(rab_idP & RAB_OFFSET2 )>> RAB_SHIFT2][(rab_idP & RAB_OFFSET)-DTCH]+=data_sizeP;

      }
    }


    free_mem_block (sduP);
  }
}
//-----------------------------------------------------------------------------
void
pdcp_run (void)
{
//-----------------------------------------------------------------------------
  // NAS -> PDCP traffic

#ifndef NAS_NETLINK
#ifdef USER_MODE
  //#define PDCP_DUMMY_BUFFER_SIZE 38
  //  unsigned char pdcp_dummy_buffer[PDCP_DUMMY_BUFFER_SIZE];


  //msg("[PDCP] PDCP Run Id %d\n",modId);


  /*
  if(Mac_rlc_xface->frame %1 == 0 && (Mac_rlc_xface->frame > 30))  {
    if (Mac_rlc_xface->Is_cluster_head[0] ==0){
      pdcp_data_req(0,5,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
      pdcp_data_req(0,4,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);

    //     pdcp_data_req(0,4,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
    // pdcp_data_req(3,4,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
    //pdcp_data_req(4,4,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
    }
    // if (Mac_rlc_xface->Is_cluster_head[modId] == 1 && Mac_rlc_xface->frame %20 == 0)  {
    else{
      pdcp_data_req(0,20,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
      pdcp_data_req(0,21,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
      pdcp_data_req(0,12,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
      pdcp_data_req(0,13,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);

	  //pdcp_data_req(0,20,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
    }
    // pdcp_data_req(0,28,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);
    //       pdcp_data_req(0,36,PDCP_DUMMY_BUFFER_SIZE,pdcp_dummy_buffer);

    }*/

#endif
#endif
  unsigned int diff,i,k,j;
  if((Mac_rlc_xface->frame%128)==0) {
    //    for(i=0;i<NB_INST;i++)
    for(i=0;i<NB_UE_INST;i++)
      for (j=0;j<NB_CNX_CH;j++)
	for(k=0;k<NB_RAB_MAX;k++){
	  diff = Pdcp_stats_tx_bytes[i][j][k];
	  Pdcp_stats_tx_bytes[i][j][k]=0;
	  Pdcp_stats_tx_rate[i][j][k] = (diff*8)>>7;// (Pdcp_stats_tx_rate[i][k]*1+(7*diff*8)>>7)/8;


	  diff = Pdcp_stats_rx_bytes[i][j][k];
	  Pdcp_stats_rx_bytes[i][j][k]=0;
	  Pdcp_stats_rx_rate[i][j][k] =(diff*8)>>7;//(Pdcp_stats_rx_rate[i][k]*1 + (7*diff*8)>>7)/8;
	}
  }


  // printf("[PDCP]Read sdus from NAS\n");
  pdcp_fifo_read_input_sdus();
    // PDCP -> NAS traffic
  pdcp_fifo_flush_sdus();

  /*printf("PDCP TTI %d\n", Mac_rlc_xface->frame);
  for (i = 0; i < 3; i++) {
      printf("[RLC_RRC][MOD ID %d] AM:",  i);
      for (j = 0; j < RLC_MAX_NUM_INSTANCES_RLC_AM; j++) {
          printf("%d", rlc[i].m_rlc_am_array[j].allocation);
      }
      printf("\n[RLC_RRC][MOD ID %d] UM:", i);
      for (j = 0; j < RLC_MAX_NUM_INSTANCES_RLC_UM; j++) {
          printf("%d", rlc[i].m_rlc_um_array[j].allocation);
      }
      printf("\n");
  }*/
}
//-----------------------------------------------------------------------------
void
pdcp_config_req (module_id_t module_idP, rb_id_t rab_idP)
{
//-----------------------------------------------------------------------------
    //msg ("[PDCP] pdcp_confiq_req()\n");
}
//-----------------------------------------------------------------------------
void
pdcp_config_release (module_id_t module_idP, rb_id_t rab_idP)
{
//-----------------------------------------------------------------------------
    //msg ("[PDCP] pdcp_config_release()\n");
}
//-----------------------------------------------------------------------------
int
pdcp_module_init ()
{

//-----------------------------------------------------------------------------
#ifndef USER_MODE
  int ret;

  ret=rtf_create(PDCP2NAS_FIFO,32768);

  if (ret < 0) {
    printk("[openair][MAC][INIT] Cannot create PDCP2NAS fifo %d (ERROR %d)\n",PDCP2NAS_FIFO,ret);

    return(-1);
  }
  else{
    printk("[openair][MAC][INIT] Created PDCP2NAS fifo %d\n",PDCP2NAS_FIFO);
    rtf_reset(PDCP2NAS_FIFO);
  }

  ret=rtf_create(NAS2PDCP_FIFO,32768);

  if (ret < 0) {
    printk("[openair][MAC][INIT] Cannot create NAS2PDCP fifo %d (ERROR %d)\n",NAS2PDCP_FIFO,ret);

    return(-1);
  }
  else{
    printk("[openair][MAC][INIT] Created NAS2PDCP fifo %d\n",NAS2PDCP_FIFO);
    rtf_reset(NAS2PDCP_FIFO);
  }

  pdcp_2_nas_irq = 0;
  pdcp_input_sdu_remaining_size_to_read=0;
  pdcp_input_sdu_size_read=0;
#endif

  return(0);

}
//-----------------------------------------------------------------------------
void
pdcp_module_cleanup ()
//-----------------------------------------------------------------------------
{

#ifndef USER_MODE
  rtf_destroy(NAS2PDCP_FIFO);
  rtf_destroy(PDCP2NAS_FIFO);
#endif
}
//-----------------------------------------------------------------------------
void
pdcp_layer_init ()
{
//-----------------------------------------------------------------------------
  unsigned int i,j,k;
    list_init (&pdcp_sdu_list, NULL);

    msg("[PDCP] pdcp_layer_init \n ");
    pdcp_output_sdu_bytes_to_write=0;
    pdcp_output_header_bytes_to_write=0;
    pdcp_input_sdu_remaining_size_to_read=0;
    //    for (i=0;i<NB_INST;i++)
    for (i=0;i<NB_UE_INST;i++)
      for (k=0;k<NB_CNX_CH;k++)
	for(j=0;j<NB_RAB_MAX;j++){
	  Pdcp_stats_tx[i][k][j]=0;
	  Pdcp_stats_tx_bytes[i][k][j]=0;
	  Pdcp_stats_tx_bytes_last[i][k][j]=0;
	  Pdcp_stats_tx_rate[i][k][j]=0;

	  Pdcp_stats_rx[i][k][j]=0;
	  Pdcp_stats_rx_bytes[i][k][j]=0;
	  Pdcp_stats_rx_bytes_last[i][k][j]=0;
	  Pdcp_stats_rx_rate[i][k][j]=0;
	}

}
//-----------------------------------------------------------------------------
void
pdcp_layer_cleanup ()
//-----------------------------------------------------------------------------
{
    list_free (&pdcp_sdu_list);
}

#ifndef USER_MODE
EXPORT_SYMBOL(pdcp_2_nas_irq);
#endif //USER_MODE
