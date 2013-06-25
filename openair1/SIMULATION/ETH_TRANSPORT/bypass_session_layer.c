/*! \file bypass_session_layer.h
* \brief implementation of emultor tx and rx 
* \author Navid Nikaein and Raymond Knopp
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr
*/ 

#include "PHY/defs.h"
#include "defs.h"
#include "extern.h"
//#include "mac_extern.h"
#include "UTIL/OCG/OCG.h"
#include "UTIL/OCG/OCG_extern.h"
#include "UTIL/LOG/log.h"

#ifdef USER_MODE
#include "multicast_link.h"
#endif

/***************************************************************************/
char rx_bufferP[BYPASS_RX_BUFFER_SIZE];
static unsigned int num_bytesP=0;
int N_P=0,N_R=0;
char     bypass_tx_buffer[BYPASS_TX_BUFFER_SIZE];
unsigned int Master_list_rx, Seq_nb;
/***************************************************************************/
mapping transport_names[] = {
    {"WAIT PM TRANSPORT INFO", WAIT_PM_TRANSPORT_INFO},
    {"WAIT SM TRANSPORT INFO", WAIT_SM_TRANSPORT_INFO},
    {"SYNC TRANSPORT INFO", SYNC_TRANSPORT_INFO},
    {"ENB_TRANSPORT INFO", ENB_TRANSPORT_INFO},
    {"UE TRANSPORT INFO", UE_TRANSPORT_INFO},
    {"RELEASE TRANSPORT INFO", RELEASE_TRANSPORT_INFO},
    {NULL, -1}
};

void init_bypass (void){

  msg ("[PHYSIM] INIT BYPASS\n");
  pthread_mutex_init (&Tx_mutex, NULL);
  pthread_cond_init (&Tx_cond, NULL);
  Tx_mutex_var = 1;
  pthread_mutex_init (&emul_low_mutex, NULL);
  pthread_cond_init (&emul_low_cond, NULL);
  emul_low_mutex_var = 1;
  bypass_init (emul_tx_handler, emul_rx_handler);
}

/***************************************************************************/
void bypass_init ( unsigned int (*tx_handlerP) (unsigned char,char*, unsigned int*, unsigned int*),unsigned int (*rx_handlerP) (unsigned char,char*,unsigned int)){
/***************************************************************************/
#ifdef USER_MODE
  multicast_link_start (bypass_rx_handler, oai_emulation.info.multicast_group, oai_emulation.info.multicast_ifname);
#endif //USER_MODE
  tx_handler = tx_handlerP;
  rx_handler = rx_handlerP;
  Master_list_rx=0;
  emu_tx_status = WAIT_SYNC_TRANSPORT;
  emu_rx_status = WAIT_SYNC_TRANSPORT;
}
/***************************************************************************/
int bypass_rx_data (unsigned int frame, unsigned int last_slot, unsigned int next_slot){
/***************************************************************************/
  bypass_msg_header_t *messg;
  bypass_proto2multicast_header_t *bypass_read_header;
  eNB_transport_info_t *eNB_info;
  UE_transport_info_t  *UE_info;
  int ue_info_ix, enb_info_ix;
  //  int             tmp_byte_count;
  int             bytes_read = 0;
  int             bytes_data_to_read;
  //  int             num_flows;
  //  int             current_flow; 
  int             m_id, n_enb, n_ue, n_dci, total_tbs=0, total_header=0;
 
  // printf("in bypass_rx_data ...\n");
 
 
  pthread_mutex_lock(&emul_low_mutex);
  if(emul_low_mutex_var){
    //LOG_T(EMU, " WAIT BYPASS_PHY...\n");
    pthread_cond_wait(&emul_low_cond, &emul_low_mutex); 
  }

  if(num_bytesP==0){
    //msg("[BYPASS] IDLE_WAIT\n");
    //exit(0);
    pthread_mutex_unlock(&emul_low_mutex);
  }

  else{
    //LOG_T(EMU,"BYPASS_RX_DATA: IN, Num_bytesp=%d...\n",num_bytesP);
    bypass_read_header = (bypass_proto2multicast_header_t *) (&rx_bufferP[bytes_read]);
    bytes_read += sizeof (bypass_proto2multicast_header_t);
    bytes_data_to_read = bypass_read_header->size;
    if(num_bytesP!=bytes_read+bytes_data_to_read) {
      LOG_W(EMU, "WARNINIG BYTES2READ # DELIVERED BYTES!!!\n");
    }
    else{
      messg = (bypass_msg_header_t *) (&rx_bufferP[bytes_read]);
      bytes_read += sizeof (bypass_msg_header_t);
      if ( (messg->frame != frame) || (messg->subframe != next_slot>>1) )
	LOG_W(EMU, "Received %s from master %d for (frame %d,subframe %d) currently (frame %d,subframe %d)\n", 
	      map_int_to_str(transport_names,messg->Message_type), messg->master_id,
	      messg->frame, messg->subframe,
	      frame, next_slot>>1);
      //chek if MASTER in my List
      // switch(Emulation_status){
    switch(messg->Message_type){	
	//case WAIT_SYNC_TRANSPORT:
      
      case WAIT_PM_TRANSPORT_INFO:
	if (messg->master_id==0 )
	  Master_list_rx=((Master_list_rx) |(1<< messg->master_id));
	break;
      
      case WAIT_SM_TRANSPORT_INFO:
	Master_list_rx=((Master_list_rx) |(1<< messg->master_id));
	break;
      case SYNC_TRANSPORT_INFO:
	
	// determite the total number of remote enb & ue 
	oai_emulation.info.nb_enb_remote += messg->nb_enb;
	oai_emulation.info.nb_ue_remote += messg->nb_ue;
	// determine the index of local enb and ue wrt the remote ones  
	if (  messg->master_id < oai_emulation.info.master_id ){
	  oai_emulation.info.first_enb_local +=messg->nb_enb;
	  oai_emulation.info.first_ue_local +=messg->nb_ue;
	}
	
	// store param for enb per master
	if ((oai_emulation.info.master[messg->master_id].nb_enb = messg->nb_enb) > 0 ){
	  for (m_id=0;m_id < messg->master_id; m_id++ ){
	    oai_emulation.info.master[messg->master_id].first_enb+=oai_emulation.info.master[m_id].nb_enb;
	  }
	  LOG_I(EMU, "[ENB] WAIT_SYNC_TRANSPORT state:  for master %d (first enb %d, totan enb %d)\n",
	  	messg->master_id, 
		oai_emulation.info.master[messg->master_id].first_enb,
		oai_emulation.info.master[messg->master_id].nb_enb);	  
	}
	// store param for ue per master
	if ((oai_emulation.info.master[messg->master_id].nb_ue  = messg->nb_ue) > 0){
	  for (m_id=0;m_id < messg->master_id; m_id++ ){
	    oai_emulation.info.master[messg->master_id].first_ue+=oai_emulation.info.master[m_id].nb_ue;
	  }
	  LOG_I(EMU, "[UE]WAIT_SYNC_TRANSPORT state: for master %d (first ue %d, total ue%d)\n",
		messg->master_id, 
		oai_emulation.info.master[messg->master_id].first_ue,
		oai_emulation.info.master[messg->master_id].nb_ue );	
	}      
	
	Master_list_rx=((Master_list_rx) |(1<< messg->master_id));
	if (Master_list_rx == oai_emulation.info.master_list) {
	  emu_rx_status = SYNCED_TRANSPORT;
	}
	LOG_I(EMU,"WAIT_SYNC_TRANSPORT state: m_id %d total enb remote %d total ue remote %d \n", 
	     messg->master_id,oai_emulation.info.nb_enb_remote, oai_emulation.info.nb_ue_remote );

	break;

	//case WAIT_ENB_TRANSPORT:
      case ENB_TRANSPORT_INFO:
#ifdef DEBUG_EMU	
	LOG_D(EMU," RX ENB_TRANSPORT INFO from master %d \n",messg->master_id);
#endif
	clear_eNB_transport_info(oai_emulation.info.nb_enb_local+oai_emulation.info.nb_enb_remote);
	
	if (oai_emulation.info.master[messg->master_id].nb_enb > 0 ){
	  enb_info_ix =0;
	  total_header=0;
	  total_header += sizeof(eNB_transport_info_t)-MAX_TRANSPORT_BLOCKS_BUFFER_SIZE;
	  
	  eNB_info = (eNB_transport_info_t *) (&rx_bufferP[bytes_read]);
	  for (n_enb = oai_emulation.info.master[messg->master_id].first_enb; 
	       n_enb < oai_emulation.info.master[messg->master_id].first_enb+oai_emulation.info.master[messg->master_id].nb_enb ;
	       n_enb ++) {
	    total_tbs=0;
	    for (n_dci = 0 ; 
		 n_dci < (eNB_info[enb_info_ix].num_ue_spec_dci+eNB_info[enb_info_ix].num_common_dci);
		 n_dci ++) { 
	      total_tbs+=eNB_info[enb_info_ix].tbs[n_dci];
	    }
	    enb_info_ix++;
	    if ( (total_tbs + total_header) > MAX_TRANSPORT_BLOCKS_BUFFER_SIZE ){ 
	      LOG_W(EMU,"RX eNB Transport buffer total size %d (header%d,tbs %d) \n",
		    total_header+total_tbs, total_header,total_tbs);
	    }
	    memcpy (&eNB_transport_info[n_enb],eNB_info, total_header+total_tbs);
	    eNB_info = (eNB_transport_info_t *)((unsigned int)eNB_info + total_header+total_tbs);
	    bytes_read+=total_header+total_tbs;
	  }
	    
	  for (n_enb = oai_emulation.info.master[messg->master_id].first_enb; 
	       n_enb < oai_emulation.info.master[messg->master_id].first_enb+oai_emulation.info.master[messg->master_id].nb_enb ;
	       n_enb ++) 
	    fill_phy_enb_vars(n_enb,next_slot);
	}
	else{
	  LOG_T(EMU,"WAIT_ENB_TRANSPORT state: no enb transport info from master %d \n", messg->master_id);
	}

	Master_list_rx=((Master_list_rx) |(1<< messg->master_id));
	if (Master_list_rx == oai_emulation.info.master_list) {
	  emu_rx_status = SYNCED_TRANSPORT;
	}	
	break;
	
      case UE_TRANSPORT_INFO:
#ifdef DEBUG_EMU
	LOG_D(EMU," RX UE TRANSPORT INFO from master %d\n",messg->master_id);
#endif	
clear_UE_transport_info(oai_emulation.info.nb_ue_local+oai_emulation.info.nb_ue_remote);	

	
	if (oai_emulation.info.master[messg->master_id].nb_ue > 0 ){ //&& oai_emulation.info.nb_enb_local >0 ){
	  // get the header first 
	  ue_info_ix =0;
	  total_header=0;
	  total_header += sizeof(UE_transport_info_t)-MAX_TRANSPORT_BLOCKS_BUFFER_SIZE;
	  UE_info = (UE_transport_info_t *) (&rx_bufferP[bytes_read]);
	  // get the total size of the transport blocks
	  for (n_ue = oai_emulation.info.master[messg->master_id].first_ue; 
	       n_ue < oai_emulation.info.master[messg->master_id].first_ue+oai_emulation.info.master[messg->master_id].nb_ue ;
	       n_ue ++) {
	    total_tbs=0;
	    for (n_enb = 0;n_enb < UE_info[ue_info_ix].num_eNB; n_enb ++) { 
	      total_tbs+=UE_info[ue_info_ix].tbs[n_enb];
	    }
	    ue_info_ix++;
	    if (total_tbs + total_header > MAX_TRANSPORT_BLOCKS_BUFFER_SIZE ){
	      LOG_W(EMU,"RX [UE %d] Total size of buffer is %d (header%d,tbs %d) \n",
		    n_ue, total_header+total_tbs,total_header,total_tbs);
	    }
	    memcpy (&UE_transport_info[n_ue], UE_info, total_header+total_tbs);
	    UE_info = (UE_transport_info_t *)((unsigned int)UE_info + total_header+total_tbs);
	    bytes_read+=total_header+total_tbs;
	  }
#ifdef DEBUG_EMU
	  for (n_enb=0; n_enb < UE_info[0].num_eNB; n_enb ++ )
	    LOG_T(EMU,"dump ue transport info rnti %x enb_id %d, harq_id %d tbs %d\n", 
		  UE_transport_info[0].rnti[n_enb],
		  UE_transport_info[0].eNB_id[n_enb],
		  UE_transport_info[0].harq_pid[n_enb],
		  UE_transport_info[0].tbs[n_enb]);
#endif	  
	  for (n_ue = oai_emulation.info.master[messg->master_id].first_ue; 
	       n_ue < oai_emulation.info.master[messg->master_id].first_ue + oai_emulation.info.master[messg->master_id].nb_ue ;
	       n_ue ++) {
	    fill_phy_ue_vars(n_ue,last_slot);
	  }
	}
	else{
	  LOG_T(EMU,"WAIT_UE_TRANSPORT state: no UE transport info from master %d\n", messg->master_id );
	}
	
	Master_list_rx=((Master_list_rx) |(1<< messg->master_id));
	if (Master_list_rx == oai_emulation.info.master_list) {
	  emu_rx_status = SYNCED_TRANSPORT;
	}
	break;
      case RELEASE_TRANSPORT_INFO :
	Master_list_rx = oai_emulation.info.master_list;
	LOG_E(EMU, "RX RELEASE_TRANSPORT_INFO\n");
	  break;
      default:
	msg("[MAC][BYPASS] ERROR RX UNKNOWN MESSAGE\n");    
	//mac_xface->macphy_exit("");
	break;
      }
    }
  
    num_bytesP=0;
    emul_low_mutex_var=1; 
    //msg("[BYPASS] CALLING_SIGNAL_HIGH_MAC\n");
    pthread_cond_signal(&emul_low_cond);
    pthread_mutex_unlock(&emul_low_mutex);
    bypass_signal_mac_phy(frame,last_slot, next_slot);


  }

  //printf("leaving ...\n");
  return bytes_read;
}

/******************************************************************************************************/ 
#ifndef USER_MODE 
int bypass_rx_handler(unsigned int fifo, int rw){
 /******************************************************************************************************/ 
  //  if(rw=='w'){
    int             bytes_read;
    int             bytes_processed=0;
    int             header_bytes; //, elapsed_time;
    //printk("[BYPASS] BYPASS_RX_HANDLER IN...\n");
    header_bytes= rtf_get(fifo_bypass_phy_user2kern, rx_bufferP,sizeof(bypass_proto2multicast_header_t) );
    if (header_bytes> 0) {
      bytes_read = rtf_get(fifo_bypass_phy_user2kern, &rx_bufferP[header_bytes],((bypass_proto2multicast_header_t *) (&rx_bufferP[0]))->size);
      // printk("BYTES_READ=%d\n",bytes_read);
      if (bytes_read > 0) {
	num_bytesP=header_bytes+bytes_read;
	emul_low_mutex_var=0;
	//printk("BYPASS_PHY SIGNAL MAC_LOW...\n");
	pthread_cond_signal(&emul_low_cond);
      }
    }
    // }
  return 0;
}
#else //USER_MODE
/******************************************************************************************************/ 
void bypass_rx_handler(unsigned int Num_bytes,char *Rx_buffer){
/******************************************************************************************************/ 
//  msg("[BYPASS] BYPASS RX_HANDLER IN ...\n");
  if(Num_bytes >0){
    pthread_mutex_lock(&emul_low_mutex);
    while(!emul_low_mutex_var){
      //    msg("[BYPASS] BYPASS: WAIT MAC_LOW...\n");
      pthread_cond_wait(&emul_low_cond, &emul_low_mutex); 
    }
    num_bytesP=Num_bytes;
    memcpy(rx_bufferP,Rx_buffer,Num_bytes);
    emul_low_mutex_var=0;
    //msg("[BYPASS] RX_HANDLER SIGNAL MAC_LOW\n");
    pthread_cond_signal(&emul_low_cond); //on ne peut que signaler depuis un context linux (rtf_handler); pas de wait, jamais!!!!!!
    pthread_mutex_unlock(&emul_low_mutex);
  }
}
#endif //USER_MODE

/******************************************************************************************************/ 
void  bypass_signal_mac_phy(unsigned int frame, unsigned int last_slot, unsigned int next_slot){
/******************************************************************************************************/ 
//  char tt=1;   

  if(Master_list_rx != oai_emulation.info.master_list){
#ifndef USER_MODE
    rtf_put(fifo_mac_bypass,&tt,1);  // the Rx window is still opened  (Re)signal bypass_phy (emulate MAC signal)  
#endif //USER_MODE      
    bypass_rx_data(frame,last_slot, next_slot);
  }
  else Master_list_rx=0;
}

#ifndef USER_MODE
/***************************************************************************/
int multicast_link_write_sock (int groupP, char *dataP, unsigned int sizeP){
/***************************************************************************/
  int             tx_bytes=0;
   
  pthread_mutex_lock(&Tx_mutex);  
  while(!Tx_mutex_var){
    //msg("[BYPASS]RG WAIT USER_SPACE FIFO SIGNAL..\n");
    pthread_cond_wait(&Tx_cond,&Tx_mutex);
  }
  Tx_mutex_var=0;
  N_P=(int)((sizeP-sizeof (bypass_proto2multicast_header_t))/1000)+2;
  tx_bytes += rtf_put (fifo_bypass_phy_kern2user, &dataP[tx_bytes],sizeof (bypass_proto2multicast_header_t));
  while(tx_bytes<sizeP){
    if(sizeP-tx_bytes<=1000)
      tx_bytes += rtf_put (fifo_bypass_phy_kern2user, &dataP[tx_bytes],sizeP-tx_bytes);
    else
      tx_bytes += rtf_put (fifo_bypass_phy_kern2user, &dataP[tx_bytes],1000);
  }
  //RG_tx_mutex_var=0;
  pthread_mutex_unlock(&Tx_mutex);
  
  return tx_bytes;
}
#endif

/***************************************************************************/
void bypass_tx_data(char Type, unsigned int frame, unsigned int next_slot){
  /***************************************************************************/
  unsigned int         num_flows;
  bypass_msg_header_t *messg;
  unsigned int         byte_tx_count;
  //  eNB_transport_info_t *eNB_info;
  int n_enb,n_ue, n_dci,total_tbs=0,total_size=0;
  messg = (bypass_msg_header_t *) (&bypass_tx_buffer[sizeof (bypass_proto2multicast_header_t)]);
  num_flows = 0;
  messg->master_id       = oai_emulation.info.master_id; //Master_id;
  //  messg->nb_master       = oai_emulation.info.nb_master;
  messg->nb_enb          = oai_emulation.info.nb_enb_local; //Master_id;
  messg->nb_ue           = oai_emulation.info.nb_ue_local; //Master_id;
  messg->nb_flow         = num_flows;
  messg->frame           = frame;
  messg->subframe        = next_slot>>1;

  byte_tx_count = sizeof (bypass_msg_header_t) + sizeof (bypass_proto2multicast_header_t);
  
  if(Type==WAIT_PM_TRANSPORT){
    messg->Message_type = WAIT_PM_TRANSPORT_INFO;
    LOG_T(EMU,"[TX_DATA] WAIT SYNC PM TRANSPORT\n");
  }
  else if(Type==WAIT_SM_TRANSPORT){
    messg->Message_type = WAIT_SM_TRANSPORT_INFO;
    LOG_T(EMU,"[TX_DATA] WAIT SYNC SM TRANSPORT\n");
  }
  else if(Type==SYNC_TRANSPORT){
    messg->Message_type = SYNC_TRANSPORT_INFO;
    // make sure that sync messages from the masters are received in increasing order of master id
    sleep(oai_emulation.info.master_id+1);
    LOG_T(EMU,"[TX_DATA] SYNC TRANSPORT\n");
  }
  else if(Type==ENB_TRANSPORT){
    LOG_D(EMU,"[TX_DATA] ENB TRANSPORT\n");
     messg->Message_type = ENB_TRANSPORT_INFO;
     total_size=0;
     total_tbs=0;
     for (n_enb=oai_emulation.info.first_enb_local;n_enb<(oai_emulation.info.first_enb_local+oai_emulation.info.nb_enb_local);n_enb++) {
       total_tbs=0;
       for (n_dci =0 ; 
	    n_dci < (eNB_transport_info[n_enb].num_ue_spec_dci+ eNB_transport_info[n_enb].num_common_dci);
	    n_dci++) {
	 total_tbs +=eNB_transport_info[n_enb].tbs[n_dci];
       }
       if (total_tbs <= MAX_TRANSPORT_BLOCKS_BUFFER_SIZE)
	 total_size = sizeof(eNB_transport_info_t)+total_tbs-MAX_TRANSPORT_BLOCKS_BUFFER_SIZE;
       else 
	 LOG_E(EMU,"[eNB]running out of memory for the eNB emulation transport buffer of size %d\n", MAX_TRANSPORT_BLOCKS_BUFFER_SIZE);
       memcpy(&bypass_tx_buffer[byte_tx_count], (char*)&eNB_transport_info[n_enb], total_size);
       byte_tx_count +=total_size;
     }
  }
  else if (Type == UE_TRANSPORT){ 
    LOG_D(EMU,"[TX_DATA] UE TRANSPORT\n");
    messg->Message_type = UE_TRANSPORT_INFO;
    total_size=0;
      total_tbs=0; // compute the actual size of transport_blocks
      for (n_ue = oai_emulation.info.first_ue_local; n_ue < (oai_emulation.info.first_ue_local+oai_emulation.info.nb_ue_local);n_ue++){
	for (n_enb=0;n_enb<UE_transport_info[n_ue].num_eNB;n_enb++) {
	total_tbs+=UE_transport_info[n_ue].tbs[n_enb];
      }
      if (total_tbs <= MAX_TRANSPORT_BLOCKS_BUFFER_SIZE)
	total_size = sizeof(UE_transport_info_t)+total_tbs-MAX_TRANSPORT_BLOCKS_BUFFER_SIZE;
       else
	 LOG_E(EMU,"[UE]running out of memory for the UE emulation transport buffer of size %d\n", MAX_TRANSPORT_BLOCKS_BUFFER_SIZE);
      memcpy(&bypass_tx_buffer[byte_tx_count], (char*)&UE_transport_info[n_ue], total_size);
      byte_tx_count +=total_size;
    }
  } 
  else if (Type == RELEASE_TRANSPORT){
    messg->Message_type = RELEASE_TRANSPORT_INFO;
  }else {
    LOG_T(EMU,"[TX_DATA] UNKNOWN MSG  \n");
  }

  ((bypass_proto2multicast_header_t *) bypass_tx_buffer)->size = byte_tx_count - sizeof (bypass_proto2multicast_header_t); 
  //if(mac_xface->frame%1000==0)   
  multicast_link_write_sock (oai_emulation.info.multicast_group, bypass_tx_buffer, byte_tx_count);
}

#ifndef USER_MODE 
/*********************************************************************************************************************/
int bypass_tx_handler(unsigned int fifo, int rw){
  /***************************************************************************/
  // if(rw=='r'){
    if(++N_R==N_P){
      //msg("[OPENAIR][RG_BYPASS] TX_handler..\n");
  //    pthread_mutex_lock(&RG_tx_mutex);
      rtf_reset(fifo_bypass_phy_kern2user);
  //     pthread_mutex_lock(&RG_tx_mutex);
      Tx_mutex_var=1;
      N_R=0;
//      pthread_mutex_unlock(&RG_tx_mutex);
      pthread_cond_signal(&Tx_cond);    
    }
    // }
}
#endif

