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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/*!
*******************************************************************************

\file       graph_int.c

\brief      Emulation des interfaces du RRM (Radio Ressource Manager )

            Cette application d'envoyer des stimuli sur les interfaces RRM:
                - RRC -> RRM
                - CMM -> RRM

\author     IACOBELLI Lorenzo

\date       20/04/10


\par     Historique:
            L.IACOBELLI 2009-10-19
                + new messages
            L.IACOBELLI 2010-04-15
                + add sensing unit emulation

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>
#include <time.h>

#include "graph_sock.h" 
//#include "rrm_sock.h"
#include "graph_int.h"
#include "graph_enum.h"




#define msg_fct printf
#define msg printf


sock_rrm_t S_graph;
static int flag_not_exit = 1 ; 
static int SN_active = 0;
//mod_lor_10_04_21++
typedef struct {
    unsigned int        NB_chan              ; //!< Number of channels 
    unsigned int        NB_val               ; //!< Number of values
    unsigned int        channels[3*NB_SENS_MAX]; //!< Vector of channels
    unsigned int        values[3*NB_SENS_MAX]    ; //!< Vector of values 

} gen_sens_info_t ;
//mod_lor_10_04_21-- 

int rrm_xface_init(int rrm_inst){

  int sock ; 
  printf("[CRRM_XFACE] init de l'interface ");  
  sleep(3);
  if(open_socket(&S_graph, RRM_SOCK_PATH, RRM_SOCK_PATH,rrm_inst)==-1)
    return (-1);
  
  if (S_graph.s  == -1) 
    {
        printf("ciao");
      return (-1);
    }
  

  printf("Graphical Interface Connected... CRRM of node %d on socket %d\n",rrm_inst, S_graph.s);  
  return 0 ;	
  
}


main(int argc,char **argv) {
    int c = 0;
    int rrm_inst=-1;
    while ((c = getopt(argc,argv,"i:")) != -1)
        switch (c)
        {
            case 'i':
                rrm_inst=atoi(optarg);
            break;
           
            default:
                exit(0);
        }
    //mod_lor_10_04_21++
    //int colorbg; 
    int colorfg; 
    int colorBTS_msg = 30;
    int colorfree=2;
    int colorbusy=9;
    int comments=88;//mod_lor_10_04_22
    int i,j;
    if (rrm_inst == FC_ID){
        //colorbg = 223;
        colorfg = 21;
        //dbg_color++
        /*for (i=0; i<256; i++){
            printf("\e[38;5;%dm",i);
            printf("%d ",i);
        
        }//dbg_color--*/
    }
    else if (rrm_inst == BTS_ID ||((rrm_inst == CH_COLL_ID || rrm_inst >= FIRST_SECOND_CLUSTER_USER_ID )&& SCEN_2_CENTR)){ //mod_lor_10_05_12 AAA
        //colorbg = 103;
        colorfg = 11;
    }
   
    else{
        //colorbg = 223;
        colorfg = 21;
    }
    printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_21
    printf("MAIN Graphical Interface Connected\n");

    rrm_xface_init(rrm_inst);
    msg_head_t *Header ;
    char *Data;
    unsigned short Data_to_read;

    while (1){
        Header = (msg_head_t *) recv_msg(&S_graph) ;
        if (Header == NULL){
            break;
            
        }
        //mod_lor_10_04_21++
        Data_to_read=Header->size;
        if (Data_to_read > 0 ){
            Data = (char *) (Header +1) ;
        }
        //mod_lor_10_04_21--
        INTERF_T msg_interf;
        int msg_type = Header->msg_type;

        if (Header->msg_type<NB_MSG_SNS_RRM)    
            msg_interf=SNS;
        else if ((msg_type-=NB_MSG_SNS_RRM)< NB_MSG_RRC_RRM)
            msg_interf=RRC;
        else if ((msg_type-=NB_MSG_RRC_RRM)< NB_MSG_CMM_RRM)
            msg_interf=CMM;
         else if ((msg_type-=NB_MSG_CMM_RRM)< NB_MSG_IP)
            msg_interf=IP;
        else {
            printf("Error! Unknown message %d!!!\n",Header->msg_type);
            break;
        }

       // msg("Got MSG of Type %d on Inst %d\n",Header->msg_type,Header->inst);
        switch ( msg_interf )
        { 
            case SNS:{
                switch ( msg_type )
                {
                    case SNS_UPDATE_SENS : //mod_lor_10_06_02
                        {
                            msg_fct( "[SENSING]>[CRRM]:UPDATE_SENSING_RESULTS number %d \n",(Header->Trans_id - 4096)) ;
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Sensing information received from sensing unit about frequencies from\n");
                            //msg_fct( "Results about frequencies from ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",(p->channels[0]/1000));
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "MHz and ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",(p->channels[1]/1000));
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "MHz\nBandwidth of analyzed subbands: ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",p->values[0]);
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "KHz\n");
                            msg_fct( "Updating of local sensing database with recived data ...\n");

                            //mod_lor_10_04_21++
                            
                            /*for (i=0;i<p->NB_chan;i++){ //com_lor_10_11_09
                                if(p->values[i]==1){
                                    printf("\e[38;5;%dm",colorfree);   //mod_lor_10_04_21
                                    msg_fct( "      Channel %d: no primary user detected\n",p->channels[i]);
                                    //printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_21
                                }
                                else{
                                    printf("\e[38;5;%dm",colorbusy);   //mod_lor_10_04_21
                                    msg_fct( "      Channel %d: primary user detected\n",p->channels[i]);
                                    //printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_21
                                }
                            }*/
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22*/
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Trasmission of the new information to the Fusion Center\n");
                            else
                                msg_fct( "Trasmission of the new information to the Cluster Head\n");//mod_lor_10_05_12--
                            msg_fct( "Waiting for next sensing update ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_21
                            //mod_lor_10_04_21--
                            
                        }
                        break ;
                    case SNS_END_SCAN_CONF :
                        {
                            
                            msg_fct( "[SENSING]>[CRRM]:END_SENSING_CONFIRMATION\n");
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Sending confirmation of stopped sensing to Fusion Center ...\n");
                            else
                                msg_fct( "Sending confirmation of stopped sensing to Cluster Head ...\n");//mod_lor_10_05_12--
                            printf("\e[38;5;%dm",colorfg);
                        }
                        break ;
           
                    default :
                    msg("[SNS]WARNING: msg unknown %d switched as %d\n",Header->msg_type,msg_type) ;
                }
                break;
            }
            case RRC:{
                switch ( msg_type )
                {
                    case RRC_RB_ESTABLISH_RESP:
                        {
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_RB_ESTABLISH_RESP \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_RB_ESTABLISH_CFM:
                        {
                            
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_RB_ESTABLISH_CFM \n",Header->inst);
                            /*if (Header->inst == BTS_ID && SCEN_1){//mod_lor_10_05_12
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22 
                                msg_fct( "******************************************************************\n");
                                msg_fct( "Link between Fusion Center and BTS opened\n");
                                msg_fct( "******************************************************************\n");
                                printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                            }*/
                            
                        }
                        break ;

                    case RRC_RB_MODIFY_RESP:
                        {
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_RB_MODIFY_RESP \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_RB_MODIFY_CFM:
                        {

                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_RB_MODIFY_CFM\n",Header->inst);
                            
                        }
                        break ;

                    case RRC_RB_RELEASE_RESP:
                        {
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_RB_RELEASE_RESP \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_MR_ATTACH_IND :
                        {
                            
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_MR_ATTACH_IND \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_SENSING_MEAS_RESP:
                        {
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_SENSING_MEAS_RESP \n",Header->inst);
                        }
                        break ;
                    case RRC_CX_ESTABLISH_IND:
                        {
                    
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_CX_ESTABLISH_IND \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_PHY_SYNCH_TO_MR_IND :
                        {
                           
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_PHY_SYNCH_TO_MR_IND.... \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_PHY_SYNCH_TO_CH_IND :
                        {
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_PHY_SYNCH_TO_CH_IND.... \n",Header->inst);
                            

                        }
                        break ;
                    case RRC_SENSING_MEAS_IND :
                        {

                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_SENSING_MEAS_IND \n",Header->inst);
                            
                        }
                        break ;
                    case RRC_RB_MEAS_IND :
                        {
                
                            msg_fct( "[CRRC]>[CRRM]:%d:RRC_RB_MEAS_IND \n",Header->inst);
                        }
                        break ;


                    case RRC_INIT_SCAN_REQ :
                        {
                            msg_fct( "[CRRC]>[CRRM]:START_SENSING_REQUEST\n");
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Order to start sensing activity received from Fusion Center\n");
                            else
                                msg_fct( "Order to start sensing activity received from Cluster Head\n");//mod_lor_10_05_12--
                                
                            msg_fct( "Activation of sensing unit ...\n");
                            msg_fct( "Waiting for sensing results ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                            
                            
                        }
                        break ;
                    case RRC_END_SCAN_CONF : 
                        {
                           
                            msg_fct( "[CRRC]>[CRRM]:END_SENSINS_ACTIVITY_CONFIRMATION\n");
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Sensor %d confirms the end of sensing activity\n",(Header->inst -FIRST_SENSOR_ID+ 1));
                            else
                                msg_fct( "Secondary User %d confirms the end of sensing activity\n",(Header->inst -FIRST_SENSOR_ID+ 1));//mod_lor_10_05_12--
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                        }
                        break ;
                    case RRC_END_SCAN_REQ :  
                        {
                           
                            msg_fct( "[CRRC]>[CRRM]:END_SENSINS_ACTIVITY_REQUEST\n");
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Order to stop sensing activity received from Fusion Center\n");
                            else
                                msg_fct( "Order to stop sensing activity received from Cluster Head\n");//mod_lor_10_05_12--
                            msg_fct( "Command to stop sensing activity transmitted to sensing unit\n");
                            msg_fct( "Waiting for confirmation from sensing unit ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                            
                        }
                        break ;
                    case RRC_INIT_MON_REQ :
                        {
                            msg_fct( "[CRRC]>[CRRM]:RRC_INIT_MON_REQ \n",Header->inst);
                        }
                        break ;
                     case RRC_UP_FREQ_ASS :
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            msg_fct( "[CRRC]>[CRRM]:UPDATE_FREQUENCIES_ASSIGNED\n",Header->inst);
                            if (p->NB_chan!=0){
								printf("\e[38;5;%dm",comments);
								msg_fct( "Channel to use: from ");
								printf("\e[38;5;%dm",colorfree);
								msg_fct( "%d ",(p->channels[0]));
								printf("\e[38;5;%dm",comments);
								msg_fct( "KHz to ");
								printf("\e[38;5;%dm",colorfree);
								msg_fct( "%d ",(p->channels[1]));
								printf("\e[38;5;%dm",comments);
								msg_fct( "KHz\n");
								printf("\e[38;5;%dm",colorfg); 
                            }
                            else {
                            	printf("\e[38;5;%dm",colorbusy);
								msg_fct( "No channels available\n");
								printf("\e[38;5;%dm",colorfg);
                            }
                        }
                        break;
                    case RRC_UP_FREQ_ASS_SEC : //add_lor_10_11_09
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            msg_fct( "[CRRC]>[CRRM]:UPDATE_FREQUENCIES_ASSIGNED_SEC \n",Header->inst);
                            for (i=0;i<p->NB_val;i++){
                                if (p->values[i]){
                                    printf("\e[38;5;%dm",comments);
                                    msg_fct( "Transmitting on channel from ");
                                    printf("\e[38;5;%dm",colorfree);
                                    msg_fct( "%d ",(p->channels[i*2]));
                                    printf("\e[38;5;%dm",comments);
                                    msg_fct( "KHz to ");
                                    printf("\e[38;5;%dm",colorfree);
                                    msg_fct( "%d ",(p->channels[(i*2)+1]));
                                    printf("\e[38;5;%dm",comments);
                                    msg_fct( "KHz\n");
                                    printf("\e[38;5;%dm",colorfg); 
                                }
                                else{
                                    printf("\e[38;5;%dm",comments);
                                    msg_fct( "Receiving on channel from ");
                                    printf("\e[38;5;%dm",colorfree);
                                    msg_fct( "%d ",(p->channels[i*2]));
                                    printf("\e[38;5;%dm",comments);
                                    msg_fct( "KHz to ");
                                    printf("\e[38;5;%dm",colorfree);
                                    msg_fct( "%d ",(p->channels[(i*2)+1]));
                                    printf("\e[38;5;%dm",comments);
                                    msg_fct( "KHz\n");
                                    printf("\e[38;5;%dm",colorfg); 
                                }
                            }
                        }
                        break ;
                    default :
                    msg("[CRRC]WARNING: msg unknown %d switched as %d\n",Header->msg_type,msg_type) ;
                }
                break;
            }
            case IP:{
                switch ( msg_type )
                {
                
                    case UPDATE_SENS_RESULTS_3 : //mod_lor_10_06_02
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            if (SCEN_1){//mod_lor_10_05_12++
                                msg_fct( "[SENSOR %d msg]:UPDATE_SENSING_MEASUREMENT\n",(Header->inst-FIRST_SENSOR_ID+1));
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                                msg_fct( "Sensing information received from sensor %d (local update number: %d)\n",(Header->inst-FIRST_SENSOR_ID+1),(Header->Trans_id - 4096));
                            }else {
                                msg_fct( "[S.U. %d msg]:UPDATE_SENSING_MEASUREMENT\n",(Header->inst-FIRST_SENSOR_ID+1));
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                                msg_fct( "Sensing information received from Secondary User %d (local update number: %d)\n",(Header->inst-FIRST_SENSOR_ID+1),(Header->Trans_id - 4096));
                            }//mod_lor_10_05_12--
                            
                            msg_fct( "Sensing results about frequencies from ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",(p->channels[0]/1000));
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "MHz and ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",(p->channels[1]/1000));
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "MHz\nUpdating of sensing database using recived data ...\n");
                            
                            /*if(SN_active &&p->values[1]!=3){ //comm_lor_10_11_09
                                msg_fct( "Checking if frequencies in use by Secondary Network are still free ...\n");
                                if(p->values[1]==1){
                                    printf("\e[38;5;%dm",colorbusy);
                                    printf("Primary detected on frequencies in use by Secondary Network\n");
                                    printf("\e[38;5;%dm",comments);   
                                    msg_fct( "Sending update of open frequencies to Secondary Network\n");
                                }
                                else if (p->values[1]==0){
                                    printf("\e[38;5;%dm",colorfree);
                                    printf("No primary communication detected on frequencies in use by Secondary Network\n");
                                    
                                }
                            }
                            else if (p->values[1]==1){
                                printf("\e[38;5;%dm",comments);   
                                msg_fct( "Sending update of open frequencies to Secondary Network\n");
                            }*/
                            //mod_lor_10_04_21++
                            /*
                            unsigned int up_to_send = 0;
                            for (i=(p->NB_chan-1);i>=0;i--){
                                if(p->values[i]==1){
                                    printf("\e[38;5;%dm",colorfree);   //mod_lor_10_04_21
                                    msg_fct( "      Channel %d: no primary user detected\n",p->channels[i]);
                                }else{
                                    printf("\e[38;5;%dm",colorbusy);   //mod_lor_10_04_21
                                    msg_fct( "      Channel %d: primary user detected\n",p->channels[i]);
                                    if(p->values[i]==3){
                                        up_to_send = 1;
                                        msg_fct( "        -> Channel used by SN\n");
                                        msg_fct( "        -> Need to send update of open frequencies\n");
                                    }
                                }
                            }
                            if (up_to_send){
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                                msg_fct( "Sending update of open frequencies to Secondary Network\n");
                            }*/
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_21
                            //mod_lor_10_04_21--
                            
                            
                        }
                        break ;
                    case OPEN_FREQ_QUERY_4 :
                        {
                            printf("\e[38;5;%dm",colorBTS_msg);  //mod_lor_10_04_23
                            msg_fct( "[BTS msg]:OPEN_FREQUENCIES_QUERY \n");
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_23
                            msg_fct( "Received a request from the secondary network BTS to know the available frequencies to use\n");
                            msg_fct( "Sending to BTS information about available frequencies\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                        }
                        break ;
                    case ASK_FREQ_TO_CH_3 : //add_lor_10_11_03
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            if (p->NB_val==0){
                                msg_fct( "[SU %d msg]:ASK_FREQUENCIES \n", Header->inst);
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_23
                                msg_fct( "Received a request from secondary user #%d to have a channel to communicate with Secondary User #%d\n",Header->inst,p->channels[0]);
                                msg_fct( "Looking for available channels...\n");
                                printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            }else {
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_23
                                msg_fct( "None channel is available...\n");
                                msg_fct( "Saving channel request...\n");
                                printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            }
                            
                        }
                        break ;
                    case UPDATE_OPEN_FREQ_7 : //mod_lor_10_06_02
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            msg_fct( "[FC msg]:UPDATE_OPEN_FREQUENCIES\n",Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_23
                            msg_fct( "Received information about available frequencies from Fusion Center\n");
                            if (p->NB_chan==0) {
                                printf("\e[38;5;%dm",colorbusy);
                                msg_fct( "No available channels for Secondary Network \n");
                            }else{
                                msg_fct( "Updating of channel database ...\n");
                                for (i=(p->NB_chan-1);i>=0;i--){ 
                                    printf("\e[38;5;%dm",colorfree);   //mod_lor_10_04_21
                                    msg_fct( "      Channel from %d KHz to %d KHz is available\n",p->channels[i-1], p->channels[i]);
                                    i=i-2;
                                }
                                printf("\e[38;5;%dm",comments);   //mod_lor_10_04_23
                                msg_fct( "Selection of frequencies for Secondary network ...\n");
                                //mod_lor_10_05_18++
                                for (i=(p->NB_val-1);i>=0;i--){
                                    for (j=(p->NB_chan-3);j>=0;j=j-3){
                                        if (p->channels[j]==p->values[i]){
                                            printf("\e[38;5;%dm",colorfree);
                                            msg_fct( "      Channel from %d KHz to %d KHz selected for SN\n",p->channels[j+1],p->channels[j+2]);
                                            break;
                                        }
                                    }                                
                                }
                            }
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_23
                            //mod_lor_10_05_18--
                            msg_fct( "Sending update of frequencies used by Secondary Network to Fusion Center ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                            
                        }
                        break ;
                    case UPDATE_SN_OCC_FREQ_5 : //mod_lor_10_06_02
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            if (p->NB_chan>0)
                                SN_active = 1; //mod_lor_10_06_02
                            printf("\e[38;5;%dm",colorBTS_msg);  //mod_lor_10_04_23
                            msg_fct( "[BTS msg]:UPDATE_SN_OCCUPIED_FREQUENCIES\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Update received from BTS about frequencies used by Secondary Network\n");
                            msg_fct( "Updating CHANNEL DATABASE :\n");
                            if (p->NB_chan == 0){
                                 msg_fct( "SN did not select any channel\n");
                                 SN_active = 0;
                             }
                            //printf("\e[38;5;%dm",colorBTS_msg);
                            for (i=0;i<p->NB_chan;i++){
                                printf("\e[38;5;%dm",comments);
                                msg_fct( "      Channel from " );
                                printf("\e[38;5;%dm",colorfree);
                                msg_fct( "%d ",p->channels[i] );
                                printf("\e[38;5;%dm",comments);
                                msg_fct( "MHz to");
                                printf("\e[38;5;%dm",colorfree);
                                msg_fct( " %d ",p->channels[++i] );
                                printf("\e[38;5;%dm",comments);
                                msg_fct( " used by Secondary Network\n" );
                            }
                            if (p->values[0]){
                                if (p->NB_chan != 0){
                                    printf("\e[38;5;%dm",colorbusy);
                                    msg_fct( "  -> Channels in use by SN are not available anymore!\n");
                                    printf("\e[38;5;%dm",comments);
                                }
                                msg_fct( "Sending update of open frequencies to BTS ...\n");
                            }
                            
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                        }
                        break ;
                    //mod_lor_10_05_10++
                    case INIT_COLL_SENS_REQ :
                        {
                            msg_fct( "******************************************************************\n");
                            msg_fct( "[CH 1 msg]:INIT_COLL_SENS_REQ from %d\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Request from another cluster to collaborate in sensing operations\n");
                            msg_fct( "Sending sensing parameters to sensors connected ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                        }
                        break ;
                    case STOP_COLL_SENS :
                        {
                            msg_fct( "[CH 1 msg]:STOP_COLL_SENS from %d\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Request to stop the collaboration in sensing operations\n");
                            msg_fct( "Sending stop sensing order to sensors connected ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                        }
                        break ;
                    case STOP_COLL_SENS_CONF :
                        {
                            printf("\e[38;5;%dm",colorBTS_msg);  //mod_lor_10_04_23
                            msg_fct( "[CH coll msg]:STOP_COLL_SENS_CONF from %d\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Collaborative Cluster Head confirms the end of sensing activity\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                        }
                        break ;
                    case UP_CLUST_SENS_RESULTS :
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            printf("\e[38;5;%dm",colorBTS_msg);  //mod_lor_10_04_23
                            msg_fct( "[CH coll msg]:UP_CLUST_SENS_RESULTS from %d\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Update received from collaborative Cluster about frequencies from\n");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",(p->channels[0]/1000));
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "MHz and ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",(p->channels[1]/1000));
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "MHz\nBandwidth of analyzed subbands: ");
                            printf("\e[38;5;%dm",colorfree);
                            msg_fct( "%d ",p->values[0]);
                            printf("\e[38;5;%dm",comments);
                            msg_fct( "KHz\n");
                            msg_fct( "Updating of sensing database using recived data...\n");
                            printf("\e[38;5;%dm",colorfg);
                        }
                        break ;
                        //mod_lor_10_05_10--
                    case USER_DISCONNECT_9 : //add_lor_10_11_09
                        {
                            msg_fct( "[SU %d msg]:USER_DISCONNECTION\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Secondary User %d wants to interrupt all its current communications.\n", Header->inst);
                            msg_fct( "Sending update to all Secondary Users\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                            
                        }
                        break ;
                    case CLOSE_LINK : //add_lor_10_11_09
                        {
                            gen_sens_info_t  *p = (gen_sens_info_t  *)Data ;
                            int i;
                            msg_fct( "[SU %d msg]:CLOSE_LINK\n", Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Secondary User %d wants to close communication channel from user %d to user %d\n", Header->inst, p->channels[0],p->values[0]);
                            msg_fct( "Sending update to all Secondary Users\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_23
                        }
                        break ;
           
                    default :
                    msg("[IP]WARNING: msg unknown %d switched as %d\n",Header->msg_type,msg_type) ;
                }
                break;
            }
            case CMM:{
                switch ( msg_type )
                {
                    case CMM_CX_SETUP_REQ:
                        {
                            
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_CX_SETUP_REQ\n",Header->inst);
                            
                            }
                        break ;
                    case CMM_CX_MODIFY_REQ:
                        {
                            
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_CX_MODIFY_REQ\n",Header->inst);
                            
                        }
                        break ;
                    case CMM_CX_RELEASE_REQ :
                        {
                           
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_CX_RELEASE_REQ\n",Header->inst);
                            
                        }
                        break ;
                    case CMM_CX_RELEASE_ALL_REQ :
                        {
    
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_CX_RELEASE_ALL_REQ\n",Header->inst);
                            
                        }
                        break ;
                    case CMM_ATTACH_CNF : 
                        {
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_ATTACH_CNF\n",Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "******************************************************************\n");
                            //mod_lor_10_05_12++
                            if (SCEN_1){//mod_lor_11_02_15++ : reorganization sendora scen 1
                                if (BTS_ID>=0 && FC_ID>=0)
                                    msg_fct( "The sensor is now connected to the fusion center\n"); //AAA to fix
                                else if (BTS_ID>=0)
                                    msg_fct( "Secondary User is now connected to the BTS\n");
                                else if (FC_ID>=0)
                                    msg_fct( "The sensor is now connected to the fusion center\n");
                            }//mod_lor_11_02_15--
                            else if(SCEN_2_DISTR)
                                msg_fct( "Secondary User is now connected to Cluster Head\n");
                            else if(Header->inst < FIRST_SECOND_CLUSTER_USER_ID)
                                msg_fct( "Secondary User is now connected to Cluster Head #1\n");
                            else
                                msg_fct( "Secondary User is now connected to Cluster Head #2\n");//mod_lor_10_05_12--
                            msg_fct( "******************************************************************\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                        }
                        break ;
                    case CMM_INIT_MR_REQ :
                        {
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_INIT_MR_REQ \n",Header->inst);
          
                        }
                        break ;
                    case CMM_INIT_CH_REQ :
                        {
                            msg_fct( "[CMM]>[CRRM]:%d:CMM_INIT_CH_REQ \n",Header->inst);
                            
                            
                        }
                        break ;

                    case CMM_INIT_SENSING :
                        {
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "******************************************************************\n"); 
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22           
                            msg_fct( "[CMM]>[CRRM]:START_SENSING\n");
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22
                            msg_fct( "Order to activate sensing received \n");
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Sending sensing parameters to sensors connected ...\n");
                            else
                                msg_fct( "Sending sensing parameters to Secondary Users connected ...\n");//mod_lor_10_05_12--
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                        }
                        break ;
                    case CMM_STOP_SENSING :
                        {
                            msg_fct( "[CMM]>[CRRM]:STOP_SENSING\n",Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22 
                            msg_fct( "Order to stop sensing operations received \n");
                            if (SCEN_1)//mod_lor_10_05_12++
                                msg_fct( "Sending order to stop sensing activity to sensors connected ...\n");
                            else{
                                if (SCEN_2_CENTR)
                                    msg_fct( "Sending order to stop collaborative sensing to collaborative CH ...\n");
                                msg_fct( "Sending order to stop sensing actions to secondary users connected ...\n");
                            }//mod_lor_10_05_12--
                            sleep(1);
                            msg_fct( "Waiting for stop sensing confirmations ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                          
                        }
                        break ;
                    case CMM_ASK_FREQ :
                        {
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22 
                            msg_fct( "******************************************************************\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                            msg_fct( "[CMM]>[CRRM]:ASK_FOR_AVAILABLE_FREQUENCIES\n",Header->inst);
                            printf("\e[38;5;%dm",comments);   //mod_lor_10_04_22 
                            msg_fct( "Need of frequencies for secondary users \n");
                            msg_fct( "Sending request of available channels to Fusion Center ...\n");
                            sleep(1);
                            msg_fct( "Waiting for available channels information ...\n");
                            printf("\e[38;5;%dm",colorfg);   //mod_lor_10_04_22
                           
                        }
                        break ;

           
                    default :
                    msg("[CMM]WARNING: msg unknown %d switched as %d\n",Header->msg_type,msg_type) ;
                }
                break;
            }
   
            default :
            msg("[Graph_xface]WARNING: msg unknown %d switched as %d\n",Header->msg_type,msg_type) ;

        }
    }
    close_socket(&S_graph) ;
}
