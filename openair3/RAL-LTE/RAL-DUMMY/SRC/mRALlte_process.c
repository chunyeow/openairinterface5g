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
#define MRAL_MODULE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

#include "mRALlte_variables.h"
#include "mRALlte_proto.h"
#include "MIH_C.h"
#include "mRALlte_mih_msg.h"
#include "mRALlte_thresholds.h"
// for real-time
//#include "nas_ue_ioctl.h"
//#include "nasmt_constant.h"
// for dummy
#include "nas_ue_netlink.h"

//global variables
extern int meas_counter;
extern int s_nas;

//---------------------------------------------------------------------------
// Temp - Enter hard-coded measures in IAL
void IAL_NAS_measures_init(void)
//---------------------------------------------------------------------------
{
#ifdef RAL_DUMMY
   ralpriv->num_measures = 3;
#endif
#ifdef RAL_REALTIME
   ralpriv->num_measures = 1;
#endif
   ralpriv->meas_cell_id[0] = 0;
#ifdef MIH_USER_CONTROL
   ralpriv->last_meas_level[0] = 0;
#else
   ralpriv->last_meas_level[0] = 30;
#endif
   ralpriv->integrated_meas_level[0] = ralpriv->last_meas_level[0];
   ralpriv->provider_id[0] = 25;
   ralpriv->meas_cell_id[1] = 1;
   ralpriv->last_meas_level[1] = 50;
   ralpriv->integrated_meas_level[1] = ralpriv->last_meas_level[1];
   ralpriv->provider_id[1] = 1;
   ralpriv->meas_cell_id[2] = 20;
   ralpriv->last_meas_level[2] = 20;
   ralpriv->integrated_meas_level[2] = ralpriv->last_meas_level[2];
   ralpriv->provider_id[2] = 25;
}

//---------------------------------------------------------------------------
// Temp - Enter hard-coded measures in IAL
void IAL_NAS_measures_update(int i)
//---------------------------------------------------------------------------
{
   int j;

   //ralpriv->meas_cell_id[0] = ralpriv->cell_id;
   j = i%2;
   if (j==0){
   ralpriv->integrated_meas_level[2] = (ralpriv->integrated_meas_level[2] + (ralpriv->last_meas_level[2]/5));
   }else{
   ralpriv->integrated_meas_level[2] = (ralpriv->integrated_meas_level[2] - (ralpriv->last_meas_level[2]/5));
   }
   ralpriv->curr_signal_level = ralpriv->integrated_meas_level[0];

//   DEBUG ("Measure update - cell 1 %d, %d \n", ralpriv->last_meas_level[1], ralpriv->integrated_meas_level[1]);
}

//---------------------------------------------------------------------------
void IAL_integrate_measure(int measure, int i){
//---------------------------------------------------------------------------
  int new_integrated = 0;

  new_integrated = (((10-LAMBDA)*ralpriv->last_meas_level[i])+(LAMBDA*measure))/10;
  //apply correction to get a value between 0-100 - now is linear
  new_integrated = (new_integrated*100)/MEAS_MAX_RSSI;
  // print result
  #ifdef DEBUG_MRALU_MEASURES
  DEBUG ("Integrate measure : old %d, new %d, integrated %d\n", ralpriv->last_meas_level[i], measure,new_integrated  );
  #endif
  // store the result
  ralpriv->last_meas_level[i] =  measure;
  ralpriv->prev_integrated_meas_level[i] = ralpriv->integrated_meas_level[i];
  ralpriv->integrated_meas_level[i] =  new_integrated;
}


//---------------------------------------------------------------------------
// poll for measures in NAS
void rallte_NAS_measures_polling(void){
//---------------------------------------------------------------------------
    MIH_C_LINK_GD_REASON_T      going_down_reason_code;
    MIH_C_LINK_DN_REASON_T      down_reason_code;
    MIH_C_TRANSACTION_ID_T      transaction_id;
    MIH_C_LINK_TUPLE_ID_T       link_identifier;
    MIH_C_LINK_DET_INFO_T       link_detected_info;
    MIH_C_UNSIGNED_INT2_T       time_interval;
    //MIH_C_LINK_PARAM_RPT_LIST_T link_parameters_report_list;

    IAL_process_DNAS_message(IO_OBJ_MEAS, IO_CMD_LIST, 0);

// hard-coded trigger for test
#ifdef MRALU_SIMU_LINKDOWN
    meas_counter ++;
    if (meas_counter == 4){
       ralpriv->curr_signal_level = ralpriv->curr_signal_level/10;
       IAL_integrate_measure(ralpriv->curr_signal_level, 0);
       DEBUG ("\n signal level %d , integrated new value : %d\n",ralpriv->curr_signal_level, ralpriv->integrated_meas_level[0]);
    }

    if (meas_counter == 5){
       ralpriv->curr_signal_level = 40;
       IAL_integrate_measure(ralpriv->curr_signal_level, 0);
       DEBUG ("\n signal level %d , integrated new value : %d\n",ralpriv->curr_signal_level, ralpriv->integrated_meas_level[0]);
    }
    if (meas_counter == 6){
       ralpriv->curr_signal_level = 0;
       IAL_integrate_measure(ralpriv->curr_signal_level, 0);
       DEBUG ("\n signal level %d , integrated new value : %d\n",ralpriv->curr_signal_level, ralpriv->integrated_meas_level[0]);
    }
#endif
    DEBUG ("signal level %d , integrated new value : %d , integrated old value :  (%d)\n",
              ralpriv->curr_signal_level,
              ralpriv->integrated_meas_level[0],
              ralpriv->prev_integrated_meas_level[0]);
//  condition still TBD - message dropped or level = 0
    if ((!ralpriv->curr_signal_level)  &&
        (ralpriv->link_to_be_detected == MIH_C_BOOLEAN_FALSE) &&
        (ralpriv->state != DISCONNECTED)
        ) {
        transaction_id                           = MIH_C_get_new_transaction_id();
        down_reason_code                         = MIH_C_LINK_DOWN_REASON_NO_RESOURCE;
        link_identifier.link_id.link_type        = MIH_C_WIRELESS_UMTS;
        link_identifier.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
        MIH_C_3GPP_ADDR_load_3gpp_str_address(&link_identifier.link_id.link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP);
        link_identifier.choice                   = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

        mRALlte_send_link_down_indication(&transaction_id,
                                          &link_identifier,
                                          NULL,
                                          &down_reason_code);

        ralpriv->link_to_be_detected = MIH_C_BOOLEAN_TRUE;
        // warning may be repeated several times
    } else if ((ralpriv->link_to_be_detected == MIH_C_BOOLEAN_FALSE) &&
               (ralpriv->curr_signal_level <= ralpriv->integrated_meas_level[0]) &&
               (ralpriv->integrated_meas_level[0] < ralpriv->prev_integrated_meas_level[0]) &&
               (ralpriv->integrated_meas_level[0] < PREDEFINED_LINK_GOING_DOWN_INDICATION_SIG_STRENGTH) &&
               (ralpriv->state != DISCONNECTED)
               ) {
        transaction_id                           = MIH_C_get_new_transaction_id();

        link_identifier.link_id.link_type        = MIH_C_WIRELESS_UMTS;
        link_identifier.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
        MIH_C_3GPP_ADDR_load_3gpp_str_address(&link_identifier.link_id.link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP);
        link_identifier.choice                   = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

        time_interval                            = (MIH_C_UNSIGNED_INT2_T)0; // unknown

        going_down_reason_code                   = MIH_C_LINK_GOING_DOWN_REASON_LINK_PARAMETER_DEGRADING;

        mRALlte_send_link_going_down_indication(&transaction_id,
                                                &link_identifier,
                                                &time_interval,
                                                &going_down_reason_code);

    } else if ((ralpriv->link_to_be_detected == MIH_C_BOOLEAN_TRUE) && (ralpriv->curr_signal_level > PREDEFINED_LINK_DETECTED_INDICATION_SIG_STRENGTH)) {
        transaction_id                           = MIH_C_get_new_transaction_id();
        // MIH_C_LINK_TUPLE_ID_T
        link_detected_info.link_tuple_id.link_id.link_type        = MIH_C_WIRELESS_UMTS;
        link_detected_info.link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
        MIH_C_3GPP_ADDR_set(&link_detected_info.link_tuple_id.link_id.link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP, strlen(DEFAULT_ADDRESS_3GPP));
        link_detected_info.link_tuple_id.choice                   = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;
        // MIH_C_NETWORK_ID_T
        MIH_C_NETWORK_ID_set(&link_detected_info.network_id, (u_int8_t *)PREDEFINED_MIH_NETWORK_ID, strlen(PREDEFINED_MIH_NETWORK_ID));
        // MIH_C_NET_AUX_ID_T
        MIH_C_NET_AUX_ID_set(&link_detected_info.net_aux_id, (u_int8_t *)PREDEFINED_MIH_NETAUX_ID, strlen(PREDEFINED_MIH_NETAUX_ID));
        // MIH_C_SIG_STRENGTH_T
        link_detected_info.sig_strength.choice     = MIH_C_SIG_STRENGTH_CHOICE_PERCENTAGE;
        link_detected_info.sig_strength._union.percentage = ralpriv->curr_signal_level;
        // sinr
        link_detected_info.sinr = PREDEFINED_LINK_DETECTED_INDICATION_SINR;
        // MIH_C_LINK_DATA_RATE_T
        link_detected_info.link_data_rate = PREDEFINED_LINK_DETECTED_INDICATION_LINK_DATA_RATE;
        // MIH_C_LINK_MIHCAP_FLAG
        link_detected_info.link_mihcap_flag = MIH_C_BIT_EVENT_SERVICE_SUPPORTED    |
                                              MIH_C_BIT_COMMAND_SERVICE_SUPPORTED  |
                                              MIH_C_BIT_INFORMATION_SERVICE_SUPPORTED;
        // MIH_C_NET_CAPS_T
        link_detected_info.net_caps = MIH_C_BIT_NET_CAPS_QOS_CLASS0 |
                                      MIH_C_BIT_NET_CAPS_QOS_CLASS1 |
                                      MIH_C_BIT_NET_CAPS_INTERNET_ACCESS;


        mRALlte_send_link_detected_indication(&transaction_id, &link_detected_info);
        ralpriv->link_to_be_detected = MIH_C_BOOLEAN_FALSE;
    }
    // LG: TO DO CHECK IF INDEX IS 0
    mRALlte_check_thresholds_signal_strength(ralpriv->integrated_meas_level[0], ralpriv->prev_integrated_meas_level[0]);
}

//---------------------------------------------------------------------------
// find correponding cell in NAS measures
int rallte_NAS_corresponding_cell(int req_index)
//---------------------------------------------------------------------------
{
  int index, i=0;

  while ((i<=ralpriv->num_measures)&&(ralpriv->req_cell_id[req_index]!=ralpriv->meas_cell_id[i]))
    i++;

  index = i;
  return index;
}

//---------------------------------------------------------------------------
void rallte_verifyPendingConnection(void){
//---------------------------------------------------------------------------
    int                                 if_ready = 0;

    if ((ralpriv->pending_req_flag)%5==0){
        DEBUG("Pending Req Flag %d\n", ralpriv->pending_req_flag);
        DEBUG(" >>>> rallte_verifyPendingConnection ");
        //poll status from driver
#ifdef RAL_DUMMY
        IAL_process_DNAS_message(IO_OBJ_CNX, IO_CMD_LIST, ralpriv->cell_id);

        if ((ralpriv->nas_state == NAS_CONNECTED)&&(ralpriv->num_rb>=1)){
           if_ready = 1;
           ralpriv->state = CONNECTED;
        }
#endif
#ifdef RAL_REALTIME
        IAL_process_NAS_message(IO_OBJ_CNX, IO_CMD_LIST, ralpriv->cell_id);
        if ((ralpriv->nas_state == GRAAL_CX_DCH)&&(ralpriv->num_rb>=1)){
           if_ready = 1;
           ralpriv->state = CONNECTED;
        }
#endif
        if ((if_ready==1)||(ralpriv->pending_req_flag > 100)){
            // here or just after ioctl? or between?
            //mRALu_send_link_switch_cnf();
        	//mRALte_send_link_action_confirm();

            mRALlte_send_link_action_confirm(&ralpriv->pending_req_transaction_id, &ralpriv->pending_req_status, NULL, &ralpriv->pending_req_ac_result);

            DEBUG("After response, Pending Req Flag = %d\n", ralpriv->pending_req_flag);
            ralpriv->pending_req_flag = 0;
            // Link_up Ind not needed anymore
            //sleep(2);
            //mRALte_send_link_up_indication();
        }
    }
}
