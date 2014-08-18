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

\file       emul_interface.c

\brief      Emulation des interfaces du RRM (Radio Ressource Manager )

            Cette application d'envoyer des stimuli sur les interfaces RRM:
                - RRC -> RRM
                - CMM -> RRM

\author     BURLOT Pascal

\date       10/07/08


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

#include "debug.h"

#include "L3_rrc_defs.h"
#include "cmm_rrm_interface.h"

#include "rrm_sock.h"
#include "cmm_msg.h"
#include "rrc_rrm_msg.h"
#include "ip_msg.h" //mod_lor_10_04_27
#include "pusu_msg.h"
#include "sensing_rrm_msg.h"

#include "transact.h"
#include "actdiff.h"
#include "rrm_util.h"
#include "rrm_constant.h"

#define NUM_SCENARIO  14
#define SENSORS_NB 3 //mod_lor_10_03_03
#define PUSU_EMUL
/*#define BTS_ID 1
#define FC_ID 0
#define CH_COLL_ID 0*/


#ifdef RRC_EMUL

extern msg_t *msg_rrc_rb_meas_ind(Instance_t inst, RB_ID Rb_id, L2_ID L2_id, MEAS_MODE Meas_mode, MAC_RLC_MEAS_T *Mac_rlc_meas_t, Transaction_t Trans_id );
extern msg_t *msg_rrc_sensing_meas_ind( Instance_t inst, L2_ID L2_id, unsigned int NB_meas, SENSING_MEAS_T *Sensing_meas, Transaction_t Trans_id );
extern msg_t *msg_rrc_sensing_meas_resp( Instance_t inst, Transaction_t Trans_id )  ;
extern msg_t *msg_rrc_cx_establish_ind( Instance_t inst, L2_ID L2_id, Transaction_t Trans_id,unsigned char *L3_info, L3_INFO_T L3_info_t,
                                    RB_ID DTCH_B_id, RB_ID DTCH_id );
extern msg_t *msg_rrc_phy_synch_to_MR_ind( Instance_t inst, L2_ID L2_id);
extern msg_t *msg_rrc_phy_synch_to_CH_ind( Instance_t inst, unsigned int Ch_index,L2_ID L2_id );
extern msg_t *msg_rrc_rb_establish_resp( Instance_t inst, Transaction_t Trans_id  );
extern msg_t *msg_rrc_rb_establish_cfm( Instance_t inst, RB_ID Rb_id, RB_TYPE RB_type, Transaction_t Trans_id );
extern msg_t *msg_rrc_rb_modify_resp( Instance_t inst, Transaction_t Trans_id );
extern msg_t *msg_rrc_rb_modify_cfm(Instance_t inst, RB_ID Rb_id, Transaction_t Trans_id  );
extern msg_t *msg_rrc_rb_release_resp( Instance_t inst, Transaction_t Trans_id );
extern msg_t *msg_rrc_MR_attach_ind( Instance_t inst, L2_ID L2_id );
extern msg_t *msg_rrc_update_sens( Instance_t inst,  /*double info_time,*/ L2_ID L2_id, unsigned int NB_info, Sens_ch_t *Sens_meas, Transaction_t Trans_id);
#endif

#ifdef SNS_EMUL
extern msg_t *msg_sensing_end_scan_conf ( Instance_t inst);
#endif

typedef struct {
    L2_ID               L2_id               ; ///< identification de niveau L2
    L3_INFO_T           L3_info_t           ; ///< type de l'identification de niveau L3
    unsigned char       L3_info[MAX_L3_INFO]; ///< identification de niveau L3
} node_info_t ;

static char c; //mod_lor_10_04_27

/*node_info_t node_info[10] = {
 { .L2_id={{0x00,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x01,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x02,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x03,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x04,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x05,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x06,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x07,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x08,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} },
 { .L2_id={{0x09,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv6_ADDR, .L3_info={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11} }
} ;*/
//mod_lor_10_01_25++
node_info_t node_info[10] = {
 { .L2_id={{0x00,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0xC0,0xA8,0x0C,0x43} },
 { .L2_id={{0x01,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0xC0,0xA8,0x0C,0x14} },
 { .L2_id={{0x02,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x03,0x03} },
 { .L2_id={{0x03,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x04,0x04} },
 { .L2_id={{0x04,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x05,0x05} },
 { .L2_id={{0x05,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x06,0x06} },
 { .L2_id={{0x06,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x07,0x07} },
 { .L2_id={{0x07,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x08,0x08} },
 { .L2_id={{0x08,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x09,0x09} },
 { .L2_id={{0x09,0x00,0xAA,0xCC,0x33,0x55,0x00,0x11}}, .L3_info_t=IPv4_ADDR, .L3_info={0x0A,0x00,0x10,0x10} }
} ;
//mod_lor_10_01_25--*/
/*****************************************************************************
 * \brief  Sensing Parameters
 */
unsigned int     Start_fr   = 2420000; //!< Low frequency limit KHz
unsigned int     Stop_fr    = 2438000; //!< High frequency limit KHz
unsigned int     Meas_band  =   9000;  //!< Channel bandwidth KHz
unsigned int     Meas_tpf   = 2;    //!< Misuration time per frequency
unsigned int     Overlap    = 5;    //!< Overlap
unsigned int     Sampl_freq = 10;   //!< Sample frequency
            

//void print_pusu_msg( neighbor_entry_RRM_to_CMM_t *pEntry );

static int flag_not_exit = 1 ;
int attached_sensors = 0;//mod_lor_10_01_25

static pthread_t        pthread_rrc_hnd,
                        pthread_sns_hnd,
                        pthread_cmm_hnd ,
                        pthread_pusu_hnd , // Publish Subscribe : -> routing CH
                        pthread_action_differe_hnd;

pthread_mutex_t         cmm_transact_exclu,
                        rrc_transact_exclu,
                        sns_transact_exclu;

unsigned int            cmm_transaction=512,
                        rrc_transaction=256,
                        sns_transaction=128 ;

transact_t              *cmm_transact_list=NULL,
                        *rrc_transact_list=NULL,
                        *sns_transact_list=NULL ;

static RB_ID rb_id      =4 ;

pthread_mutex_t actdiff_exclu;
actdiff_t       *list_actdiff   = NULL ;
unsigned int    cnt_actdiff     = 512;


extern void scenario(  int num , sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm, sock_rrm_t *s_sns) ;

/*****************************************************************************
 * \brief  thread d'emulation de l'interface du Publish/subcribe (routingCH).
 * \return NULL
 */
#ifdef PUSU_EMUL
static void *fn_pusu (
    void * p_data /**< parametre du pthread */
    )
{
    sock_rrm_t *s = (sock_rrm_t *) p_data ;
    msg_head_t  *header ;

#ifdef TRACE
    FILE *fd = fopen( "VCD/rrm2pusu.txt", "w") ;
    PNULL(fd) ;
#endif

    fprintf(stderr,"PUSU interfaces :starting ...\n");

    while (flag_not_exit)
    {
        header = (msg_head_t *) recv_msg(s) ;
        if (header == NULL )
        {
            fprintf(stderr,"Server closed connection\n");
            //flag_not_exit = 0; //mod_lor_10_04_27
        }
        else
        {
            char *msg = NULL ;

            if ( header->size > 0 )
            {
                msg = (char *) (header +1) ;
            }
#ifdef TRACE
            if ( header->msg_type < NB_MSG_RRM_PUSU )
            fprintf(fd,"%lf RRM->PUSU %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_pusu_rrm[header->msg_type],header->msg_type,header->Trans_id);
            else
            fprintf(fd,"%lf RRM->PUSU %-30s %d %d\n",get_currentclock(), "inconnu", header->msg_type,header->Trans_id);
            fflush(fd);
#endif
            switch ( header->msg_type )
            {
                case RRM_PUBLISH_IND:
                    {
         //               msg_fct( "[RRM]>[PUSU]:%d:RRM_PUBLISH_IND\n",header->inst);
                        send_msg( s, msg_pusu_resp( header->inst, PUSU_PUBLISH_RESP, header->Trans_id )) ;
                    }
                    break ;
                case RRM_UNPUBLISH_IND:
                    {
         //                 msg_fct( "[RRM]>[PUSU]:%d:RRM_UNPUBLISH_IND\n",header->inst);
                        send_msg( s, msg_pusu_resp( header->inst, PUSU_UNPUBLISH_RESP, header->Trans_id )) ;
                    }
                    break ;
                case RRM_LINK_INFO_IND:
                    {
                        msg_fct( "[RRM]>[PUSU]:%d:RRM_LINK_INFO_IND\n",header->inst);
                        send_msg( s, msg_pusu_resp( header->inst, PUSU_LINK_INFO_RESP, header->Trans_id )) ;
                    }
                    break ;
                case RRM_SENSING_INFO_IND:
                    {
              //            msg_fct( "[RRM]>[PUSU]:%d:RRM_SENSING_INFO_IND\n",header->inst);
                        send_msg( s, msg_pusu_resp( header->inst, PUSU_SENSING_INFO_RESP, header->Trans_id )) ;
                    }
                    break ;
                case RRM_CH_LOAD_IND:
                    {
               //           msg_fct( "[RRM]>[PUSU]:%d:RRM_CH_LOAD_IND\n",header->inst);
                        send_msg( s, msg_pusu_resp( header->inst, PUSU_CH_LOAD_RESP, header->Trans_id )) ;
                    }
                    break ;
                default:
                    fprintf(stderr, "[RRM]>[PUSU]: msg unknown %d\n", header->msg_type) ;
                    //printHex(msg,n,1);
            }
            RRM_FREE(header);
        }
    }

    fprintf(stderr,"... stopped PUSU interfaces\n");
#ifdef TRACE
    fclose(fd) ;
#endif

    return NULL;
}
#endif /* PUSU_EMUL */

#ifdef RRC_EMUL

/*****************************************************************************
 * \brief  thread d'emulation de l'interface du rrc.
 * \return NULL
 */
static void * fn_rrc (
    void * p_data /**< parametre du pthread */
    )
{
    sock_rrm_t *s = (sock_rrm_t *) p_data ;
    msg_head_t *header ;

#ifdef TRACE
    FILE *fd = fopen( "VCD/rrm2rrc.txt", "w") ;
    PNULL(fd) ;
#endif

    fprintf(stderr,"RRC interfaces :starting ...\n");
    fprintf(stderr,"prova\n"); //dbg

    while (flag_not_exit)
    {
        header = (msg_head_t *) recv_msg(s) ;
        if (header == NULL )
        {
            fprintf(stderr,"Server closed connection\n");
            //flag_not_exit = 0;//mod_lor_10_04_27
        }
        else
        {
            char *msg = NULL ;

            if ( header->size > 0 )
            {
                msg = (char *) (header +1) ;
            }
#ifdef TRACE
            if ( header->msg_type < NB_MSG_RRC_RRM )
            fprintf(fd,"%lf RRM->RRC %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_rrc_rrm[header->msg_type],header->msg_type,header->Trans_id);
            else
            fprintf(fd,"%lf RRM->RRC %-30s %d %d\n",get_currentclock(), "inconnu", header->msg_type,header->Trans_id);
            fflush(fd);
#endif
            switch ( header->msg_type )
            {
                case RRM_RB_ESTABLISH_REQ:
                    {
                        //rrm_rb_establish_req_t *p = (rrm_rb_establish_req_t *) msg ;
                        msg_fct( "[RRM]>[RRC]:%d:RRM_RB_ESTABLISH_REQ\n",header->inst);

                        send_msg( s, msg_rrc_rb_establish_resp( header->inst, header->Trans_id )) ;

                        pthread_mutex_lock( &actdiff_exclu  ) ;
                        add_actdiff(&list_actdiff,0.05, cnt_actdiff++,  s,
                                    msg_rrc_rb_establish_cfm( header->inst, rb_id++, UNICAST,header->Trans_id) ) ;

                        pthread_mutex_unlock( &actdiff_exclu ) ;
                    }
                    break ;
                case RRM_RB_MODIFY_REQ:
                    {
                        rrm_rb_modify_req_t *p = (rrm_rb_modify_req_t *) msg ;
                        msg_fct( "[RRM]>[RRC]:%d:RRM_RB_MODIFY_REQ\n",header->inst);

                        send_msg( s, msg_rrc_rb_modify_resp( header->inst,header->Trans_id )) ;

                        pthread_mutex_lock( &actdiff_exclu  ) ;

                        add_actdiff(&list_actdiff,0.05, cnt_actdiff++,  s,
                                    msg_rrc_rb_modify_cfm( header->inst, p->Rb_id, header->Trans_id) ) ;

                        pthread_mutex_unlock( &actdiff_exclu ) ;

                    }
                    break ;

                case RRM_RB_RELEASE_REQ:
                    {
                        //rrm_rb_release_req_t *p = (rrm_rb_release_req_t *) msg ;
                        msg_fct( "[RRM]>[RRC]:%d:RRM_RB_RELEASE_REQ\n",header->inst);

                        send_msg( s, msg_rrc_rb_release_resp( header->inst,header->Trans_id )) ;
                    }
                    break ;

                case RRM_SENSING_MEAS_REQ:
                    {
                        //rrm_sensing_meas_req_t *p = (rrm_sensing_meas_req_t *) msg ;
                        msg_fct( "[RRM]>[RRC]:%d:RRM_SENSING_MEAS_REQ\n",header->inst);

                        send_msg( s, msg_rrc_sensing_meas_resp( header->inst, header->Trans_id ) );
                    }
                    break ;

                case RRCI_CX_ESTABLISH_RESP:
                    {
                        rrci_cx_establish_resp_t *p = (rrci_cx_establish_resp_t *) msg ;
                        msg_fct( "[RRCI]>[RRC]:%d:RRCI_CX_ESTABLISH_RESP\n",header->inst);

                        fprintf(stderr,"L3_id: ");
                        print_L3_id(p->L3_info_t, p->L3_info );
                        fprintf(stderr,"\n");

                    }
                    break ;

                case RRM_SENSING_MEAS_RESP:
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_SENSING_MEAS_RESP\n",header->inst);
                    }
                    break ;

                case RRM_RB_MEAS_RESP:
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_RB_MEAS_RESP\n",header->inst);
                    }
                    break ;

                case RRM_INIT_CH_REQ:
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_INIT_CH_REQ\n",header->inst);
                    }
                    break ;

                case RRCI_INIT_MR_REQ:
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_INIT_MR_REQ\n",header->inst);
                    }
                    break ;
                case RRM_INIT_MON_REQ:
                    {
                        rrm_init_mon_req_t *p = (rrm_init_mon_req_t *) msg ;
                        msg_fct( "[RRM]>[RRC]:%d:RRM_INIT_MON_REQ on channels: ",header->inst);
                        fprintf(stdout,"chan nb: %d\n", p->NB_chan); //dbg
                        for ( int i=0;i<p->NB_chan;i++)
                            msg_fct("%d ", p->ch_to_scan[i]);
                        msg_fct( "\n");
                    
                    }
                    break ;
                case RRM_INIT_SCAN_REQ:
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_INIT_SCAN_REQ\n",header->inst);
                        
                    }
                    break ;
               
                case RRM_END_SCAN_REQ:
                    {
                        rrm_end_scan_req_t *p = (rrm_end_scan_req_t *) msg ;
                        msg_fct( "[RRM]>[RRC]:%d:RRM_END_SCAN_REQ on sensor",header->inst);
                        for ( int i=0;i<8;i++)
                            msg_fct("%02X", p->L2_id.L2_id[i]);
                        msg_fct( "\n");
                        
                        
                    }
                    break ;
                case RRM_END_SCAN_ORD:
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_END_SCAN_ORD\n",header->inst);
                        
                    }
                    break ;
                case RRM_UP_FREQ_ASS :
                    {
                        msg_fct( "[RRM]>[RRC]:%d:RRM_UP_FREQ_ASS\n",header->inst);   
                    }
                    break ;

                default :
                    fprintf(stderr, "RRC: msg unknown %d\n", header->msg_type) ;
                    //printHex(msg,n,1);
            }

            RRM_FREE(header);
        }

    }

    fprintf(stderr,"... stopped RRC interfaces\n");

#ifdef TRACE
    fclose(fd) ;
#endif

    return NULL;
}

#endif /* RRC_EMUL */

/*****************************************************************************
 * \brief  thread d'emulation de l'interface du cmm.
 * \return NULL
 */
static void * fn_cmm (
    void * p_data /**< parametre du pthread */
    )
{
    sock_rrm_t *s = (sock_rrm_t *) p_data ;
    msg_head_t *header ;

#ifdef TRACE
    FILE *fd = fopen( "VCD/rrm2cmm.txt", "w") ;
    PNULL(fd) ;
#endif

    fprintf(stderr,"CMM interfaces :starting ...\n");

    while (flag_not_exit)
    {
        header = (msg_head_t *) recv_msg(s) ;
        if (header == NULL )
        {
            fprintf(stderr,"Server closed connection\n");
            //flag_not_exit = 0;//mod_lor_10_04_27
        }
        else
        {
            char *msg = NULL ;

            if ( header->size > 0 )
            {
                msg = (char *) (header +1) ;
            }
#ifdef TRACE
            if ( header->msg_type < NB_MSG_CMM_RRM )
            fprintf(fd,"%lf RRM->CMM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_cmm_rrm[header->msg_type],header->msg_type,header->Trans_id);
            else
            fprintf(fd,"%lf RRM->CMM %-30s %d %d\n",get_currentclock(),"inconnu",header->msg_type,header->Trans_id);
            fflush(fd);
#endif
            switch ( header->msg_type )
            {
                case RRM_CX_SETUP_CNF :
                    {
                        // rrm_cx_setup_cnf_t *p = (rrm_cx_setup_cnf_t *) msg ;
             //             msg_fct( "[RRM]>[CMM]:%d:RRM_CX_SETUP_CNF\n",header->inst);

                        pthread_mutex_lock( &cmm_transact_exclu ) ;
                        del_item_transact( &cmm_transact_list, header->Trans_id );
                        pthread_mutex_unlock( &cmm_transact_exclu ) ;
                    }
                    break ;
                case RRM_CX_MODIFY_CNF :
                    {
                        //rrm_cx_modify_cnf_t *p = (rrm_cx_modify_cnf_t *) msg ;
                //          msg_fct( "[RRM]>[CMM]:%d:RRM_CX_MODIFY_CNF\n",header->inst);
                    }
                    break ;
                case RRM_CX_RELEASE_CNF :
                    {
                        //rrm_cx_release_cnf_t *p = (rrm_cx_release_cnf_t *) msg ;
                //          msg_fct( "[RRM]>[CMM]:%d:RRM_CX_RELEASE_CNF\n",header->inst);
                    }
                    break ;
                case RRM_CX_RELEASE_ALL_CNF :
                    {
                        //rrm_cx_release_all_cnf_t *p = (rrm_cx_release_all_cnf_t *) msg ;
                 //         msg_fct( "[RRM]>[CMM]:%d:RRM_CX_RELEASE_ALL_CNF\n",header->inst);
                    }
                    break ;
                case RRCI_ATTACH_REQ :
                    {
#ifndef PHY_EMUL
                        float delai = 0.05 ;
#else
                        float delai = 0.00 ;
#endif
                        rrci_attach_req_t *p = (rrci_attach_req_t *) msg ;
                 //         msg_fct( "[RRM]>[CMM]:%d:RRCI_ATTACH_REQ \n",header->inst);
                        //MSG_L2ID(p->L2_id);
                        //add_lor_11_01_07++
                        if (SCEN_2_CENTR)
                            sleep(2);
                        //add_lor_11_01_07--
                        pthread_mutex_lock( &actdiff_exclu  ) ;
                        

                        add_actdiff(&list_actdiff,delai, cnt_actdiff++, s,
                                msg_cmm_attach_cnf(header->inst,p->L2_id,node_info[header->inst].L3_info_t,node_info[header->inst].L3_info,header->Trans_id ) ) ;
                    
                        pthread_mutex_unlock( &actdiff_exclu ) ;
                    }
                    break ;
                case RRM_ATTACH_IND :
                    { //mod_lor: 10_02_09++
                        if (WSN && header->inst == 0) //inst_to_change: remove header->inst == 0 in case WSN and SN not on the same machine
                            attached_sensors ++;//mod_lor: 10_01_25
                        //msg_fct( "attached_sensors %d \n\n",attached_sensors); //dbg
#ifndef PHY_EMUL
                        float delai = 0.05 ;
#else
                        float delai = 0.00 ;
#endif
                 //         msg_fct( "[RRM]>[CMM]:%d:RRM_ATTACH_IND\n",header->inst);
                        
                         /*   //mod_lor_10_04_27++
                        //if (WSN && attached_sensors==SENSORS_NB && header->inst == 0){ //AAA inst_to_change: remove header->inst == 0 in case WSN and SN not on the same machine
                            //mod_lor_10_03_12++
                            unsigned int     Start_fr   = 1000;
                            unsigned int     Stop_fr    = 2000;
                            unsigned int     Meas_band  = 200;
                            unsigned int     Meas_tpf   = 2;
                            unsigned int     Nb_channels= (Stop_fr-Start_fr)/Meas_band; 
                            unsigned int     Overlap    = 5;
                            unsigned int     Sampl_freq = 10;
                            
                          
                            scanf("%c",&c); //mod_lor_10_04_27
                            if (c == 's'){//mod_lor_10_04_27

                                pthread_mutex_lock( &actdiff_exclu  ) ; 
                                add_actdiff(&list_actdiff,5, cnt_actdiff++, s,
                                        msg_cmm_init_sensing(header->inst,Start_fr,Stop_fr,Meas_band,Meas_tpf,
                                        Nb_channels,Overlap, Sampl_freq) );

                                pthread_mutex_unlock( &actdiff_exclu ) ;  //mod_lor: 10_02_09--
                                scanf("%c",&c); //mod_lor_10_04_27
                            }
                            
                            if (c == 'e'){//mod_lor_10_04_27
                                pthread_mutex_lock( &actdiff_exclu  ) ; 
                                add_actdiff(&list_actdiff,5, cnt_actdiff++, s,
                                        msg_cmm_stop_sensing(0) );

                                pthread_mutex_unlock( &actdiff_exclu ) ;  //mod_lor: 10_02_09--
                            }
                            
                        } //mod_lor_10_03_12++
                        //mod_lor_10_04_27--*/
                    }
                    break ;
                case RRM_MR_ATTACH_IND :
                    {
                        L2_ID L2_id_mr;
#ifndef PHY_EMUL
                        float delai = 0.05 ;
#else
                        float delai = 0.00 ;
#endif
                        rrm_MR_attach_ind_t *p = (rrm_MR_attach_ind_t *) msg ;

                  //        msg_fct( "[RRM]>[CMM]:%d:RRM_MR_ATTACH_IND\n",header->inst);
                        memcpy( L2_id_mr.L2_id, p->L2_id.L2_id, sizeof(L2_ID));

                        //add_lor_11_01_07++
                        if (SCEN_2_CENTR)
                            sleep(2);
                        //add_lor_11_01_07--
                        pthread_mutex_lock( &actdiff_exclu  ) ;

                        cmm_transaction++;
                        add_actdiff(&list_actdiff,delai, cnt_actdiff++, s,
                                    msg_cmm_cx_setup_req(header->inst,node_info[header->inst].L2_id,L2_id_mr, QOS_DTCH_D, cmm_transaction ) ) ;

                        pthread_mutex_unlock( &actdiff_exclu ) ;

                        pthread_mutex_lock( &cmm_transact_exclu ) ;
                        add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM,CMM_CX_SETUP_REQ,0,NO_PARENT);
                        pthread_mutex_unlock( &cmm_transact_exclu ) ;
                    }
                    break ;
                case ROUTER_IS_CH_IND :
                    {
#ifndef PHY_EMUL
                        float delai  = 0.05 ; //mod_lor_10_04_22: 5 instead of 0.05
                        float delai2 = 0.08 ;
#else
                        float delai  = 0.00 ;
                        float delai2 = 0.00 ;
#endif
                        router_is_CH_ind_t *p =(router_is_CH_ind_t *)msg ;
                 //         msg_fct( "[RRM]>[CMM]:%d:ROUTER_IS_CH_IND\n",header->inst);

                        memcpy( node_info[header->inst].L2_id.L2_id, p->L2_id.L2_id, sizeof(L2_ID));
                        //print_L2_id(&L2_id_ch ); printf("=>L2_id_ch\n");

                        pthread_mutex_lock( &actdiff_exclu  ) ;

                        add_actdiff(&list_actdiff,delai, cnt_actdiff++,  s,
                                    msg_cmm_init_ch_req( header->inst,node_info[header->inst].L3_info_t,node_info[header->inst].L3_info )) ;

                        cmm_transaction++;
                        add_actdiff(&list_actdiff,delai2, cnt_actdiff++, s,
                                    msg_cmm_cx_setup_req(header->inst,node_info[header->inst].L2_id,node_info[header->inst].L2_id, QOS_DTCH_B, cmm_transaction ) ) ;

                        pthread_mutex_unlock( &actdiff_exclu ) ;

                        pthread_mutex_lock( &cmm_transact_exclu ) ;
                        add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM, CMM_CX_SETUP_REQ,0,NO_PARENT);
                        pthread_mutex_unlock( &cmm_transact_exclu ) ;
                        
                        /*//mod_lor_10_04_27++
                        if (header->inst==1){
                            //scanf("%c",&c); //mod_lor_10_04_27
                            //if (c == 'a'){//mod_lor_10_04_27
                                pthread_mutex_lock( &actdiff_exclu  ) ; 
                                add_actdiff(&list_actdiff,20, cnt_actdiff++, s, msg_cmm_ask_freq(header->inst) );
                                pthread_mutex_unlock( &actdiff_exclu ) ;
                            //}
                        }//mod_lor_10_04_27--*/
                    }
                    break ;
                case RRCI_CH_SYNCH_IND :
                    {
              //            msg_fct( "[RRM]>[CMM]:%d:RRCI_CH_SYNCH_IND\n",header->inst);
                    }
                    break ;
                case RRM_MR_SYNCH_IND :
                    {
          //                msg_fct( "[RRM]>[CMM]:%d:RRM_MR_SYNCH_IND\n",header->inst);
                    }
                    break ;
                case RRM_NO_SYNCH_IND:
                    {
          //                msg_fct( "[RRM]>[CMM]:%d:RRM_NO_SYNCH_IND\n",header->inst);
                    }
                    break ;
                default :
                    fprintf(stderr, "CMM:unknown msg %d\n", header->msg_type) ;
                    //printHex(msg,n,1);
            }
            RRM_FREE(header);
        }
    }

    fprintf(stderr,"... stopped CMM interfaces\n");

#ifdef TRACE
    fclose(fd) ;
#endif

    return NULL;
}
//mod_lor_10_04_15++ -> emulation sensing
/*****************************************************************************
 * \brief  thread d'emulation de l'interface du Publish/subcribe (routingCH).
 * \return NULL
 */
#ifdef SNS_EMUL
static void *fn_sns (
    void * p_data /**< parametre du pthread */
    )
{
    sock_rrm_t *s = (sock_rrm_t *) p_data ;
    msg_head_t  *header ;

#ifdef TRACE
    FILE *fd = fopen( "VCD/rrm2sns.txt", "w") ;
    PNULL(fd) ;
#endif

    fprintf(stderr,"SNS interfaces :starting ...\n");

    while (flag_not_exit)
    {
        header = (msg_head_t *) recv_msg(s) ;
        if (header == NULL )
        {
            fprintf(stderr,"Server closed connection\n");
            //flag_not_exit = 0; //mod_lor_10_04_27
        }
        else
        {
            char *msg = NULL ;

            if ( header->size > 0 )
            {
                msg = (char *) (header +1) ;
            }
#ifdef TRACE
            if ( header->msg_type < NB_MSG_SNS_RRM  )
            fprintf(fd,"%lf RRM->SENSING %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_sns_rrm[header->msg_type],header->msg_type,header->Trans_id);
            else
            fprintf(fd,"%lf RRM->SENSING %-30s %d %d\n",get_currentclock(), "inconnu", header->msg_type,header->Trans_id);
            fflush(fd);
#endif
            switch ( header->msg_type )
            {
                 case RRM_SCAN_ORD:
                    {
                        rrm_scan_ord_t *p  = (rrm_scan_ord_t *) msg ;
                        msg_fct( "[RRM]>[SENSING]:%d:RRM_SCAN_ORD\n",header->inst);
                        
                        /*fprintf(stderr,"NB_chan = %d;\nMeas_tpf: %d;\nOverlap: %d;\nSampl_freq: %d;\n",p->NB_chan, p->Meas_tpf, p->Overlap,p->Sampl_freq);//dbg
                        fprintf(stderr,"Channels ids:   ");//dbg
                        for ( int i=0; i<p->NB_chan; i++)//dbg
                            fprintf(stderr," %d     ",p->ch_to_scan[i].Ch_id);//dbg
                        fprintf(stderr," \n\n");//dbg*/
                        
                    }
                    break ;
                case RRM_END_SCAN_ORD:
                    {
                        msg_fct( "[RRM]>[SENSING]:%d:RRM_END_SCAN_ORD\n",header->inst);
                        send_msg( s, msg_sensing_end_scan_conf( header->inst)) ;
                        
                    }
                    break ;
              
                
                default:
                    fprintf(stderr, "[RRM]>[SENSING]: msg unknown %d\n", header->msg_type) ;
                    //printHex(msg,n,1);
            }
            RRM_FREE(header);
        }
    }

    fprintf(stderr,"... stopped SENSING interfaces\n");
#ifdef TRACE
    fclose(fd) ;
#endif

    return NULL;
}
#endif /* SNS_EMUL */

//mod_lor_10_04_15--

/*****************************************************************************
 * \brief  thread d'emulation de l'interface du cmm.
 * \return NULL
 */
static void * fn_action_differe (
    void * p_data /**< parametre du pthread */
    )
{
    fprintf(stderr,"thread action differe :starting ...\n");

    while (flag_not_exit)
    {
        usleep( 10*1000 ) ;

        //  traitement de liste d'actions differees
        pthread_mutex_lock( &actdiff_exclu  ) ;
        processing_actdiff(&list_actdiff ) ;
        pthread_mutex_unlock( &actdiff_exclu ) ;

    }
    fprintf(stderr,"... stopped thread action differe\n");
    return NULL;
}

int main( int argc , char **argv )
{
    int ret = 0;
    int tr, rec;
#ifdef RRC_EMUL
    sock_rrm_t s_rrc ;
#endif /* RRC_EMUL */
#ifdef SNS_EMUL
    sock_rrm_t s_sns ;
#endif /* SNS_EMUL */
    sock_rrm_t s_cmm ;
    sock_rrm_t s_pusu ;

    /* ***** MUTEX ***** */
    pthread_attr_t attr ;

    // initialise les attributs des threads
    pthread_attr_init( &attr ) ;
    pthread_attr_setschedpolicy( &attr, SCHED_RR ) ;

    pthread_mutex_init( &actdiff_exclu      , NULL ) ;
    pthread_mutex_init( &cmm_transact_exclu , NULL ) ;
    pthread_mutex_init( &rrc_transact_exclu , NULL ) ;
    pthread_mutex_init( &sns_transact_exclu , NULL ) ;

    fprintf(stderr,"Emulation des interfaces\n");

#ifdef RRC_EMUL
    fprintf(stderr,"Trying to connect... RRM-RRC\n");
    open_socket(&s_rrc, RRC_RRM_SOCK_PATH, RRM_RRC_SOCK_PATH,0) ;
    if (s_rrc.s  == -1)
        exit(1);
    fprintf(stderr,"Connected... RRM-RRC (s=%d)\n",s_rrc.s);
#endif /* RRC_EMUL */

#ifdef SNS_EMUL
    fprintf(stderr,"Trying to connect... RRM-SNS\n");
    open_socket(&s_sns, SENSING_RRM_SOCK_PATH, RRM_SENSING_SOCK_PATH,0) ;
    if (s_sns.s  == -1)
        exit(1);
    fprintf(stderr,"Connected... RRM-SNS (s=%d)\n",s_sns.s);
#endif /* SNS_EMUL */


    fprintf(stderr,"Trying to connect... CRRM-CMM\n");
    open_socket(&s_cmm,CMM_RRM_SOCK_PATH,RRM_CMM_SOCK_PATH,0) ;
    if (s_cmm.s  == -1)
        exit(1);
    fprintf(stderr,"Connected... CRRM-CMM (s=%d)\n",s_cmm.s);

#ifdef PUSU_EMUL
    fprintf(stderr,"Trying to connect... CRRM-PUSU\n");
    open_socket(&s_pusu,PUSU_RRM_SOCK_PATH,RRM_PUSU_SOCK_PATH,0) ;
    if (s_pusu.s  == -1)
        exit(1);
    fprintf(stderr,"Connected... CRRM-PUSU (s=%d)\n",s_pusu.s);
#endif

#ifdef RRC_EMUL
   /* Creation du thread RRC */
    fprintf(stderr,"Creation du thread RRC \n");
    ret = pthread_create ( &pthread_rrc_hnd, NULL, fn_rrc, &s_rrc );
    if (ret)
    {
        fprintf(stderr, "%s", strerror (ret));
    }
#endif /* RRC_EMUL */

#ifdef SNS_EMUL
   /* Creation du thread SENSING */
    fprintf(stderr,"Creation du thread SNS \n");
    ret = pthread_create ( &pthread_sns_hnd, NULL, fn_sns, &s_sns );
    if (ret)
    {
        fprintf(stderr, "%s", strerror (ret));
    }
#endif /* SNS_EMUL */

    /* Creation du thread CMM */
    ret = pthread_create(&pthread_cmm_hnd , NULL, fn_cmm, &s_cmm );
    if (ret)
    {
        fprintf(stderr, "%s", strerror (ret));
    }

#ifdef PUSU_EMUL
    /* Creation du thread Publish Subscribe (Routing CH) */
    ret = pthread_create (&pthread_pusu_hnd , NULL, fn_pusu, &s_pusu );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
    }
#endif /*PUSU_EMUL */

    /* Creation du thread action_differe */
    ret = pthread_create (&pthread_action_differe_hnd , NULL, fn_action_differe, NULL );
    if (ret)
    {
        fprintf(stderr, "%s", strerror (ret));
    }

#ifdef RRC_EMUL
    usleep(100000);
    scenario( NUM_SCENARIO, &s_rrc, &s_cmm, &s_sns );
#endif /* RRC_EMUL */
    sleep(5);
    //mod_lor_11_02_15 ++: reorganization
    printf("\n\n\nCommands: \n    'q' to exit\n" );
    if(SCEN_2_CENTR || (SCEN_1 && FC_ID>=0)){
        printf("    's' to start sensing\n    'e' to end sensing\n" );
        if (SCEN_1)
             printf("\n\n" );
    }
    if (SCEN_1 && BTS_ID>=0)
        printf("    'a' to active BTS request\n\n\n" );
    if (SCEN_2_CENTR)
        printf("    'c' to active collaborative sensing\n    'a' to active a secondary user\n    'l' to remove a link between two secondary users\n    'd' to disconnect a secondary user\n\n\n" );
    //mod_lor_11_02_15 --
    
    //getchar() ;//mod_lor_10_04_27
    while (flag_not_exit){
        scanf("%c",&c);
        if (c == 'q')
            flag_not_exit = 0;//mod_lor_10_04_27
        else if (c == 's'){//mod_lor_10_04_27
            printf("Starting sensing ... \n\n");
            unsigned int     Nb_channels= (Stop_fr-Start_fr)/Meas_band; 
            if (Nb_channels>NB_SENS_MAX){
                printf("ERROR! too many channels! Maximum number of channels is %d",NB_SENS_MAX);
                break;
            }
            pthread_mutex_lock( &actdiff_exclu  ) ; 
            add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm,
                    msg_cmm_init_sensing(FC_ID,Start_fr,Stop_fr,Meas_band,Meas_tpf,
                    Nb_channels,Overlap, Sampl_freq) );
            pthread_mutex_unlock( &actdiff_exclu ) ;  //mod_lor: 10_02_09--
        }
        else if (c == 'd'){ //add_lor_10_11_09
            printf("Select user to disconnect (from 1 to 3)  ... \n");
            scanf("%d",&tr);
            while (tr<1||tr>3){
                printf(" User id not valid!\nSelect user (from 1 to 3)  ... \n");
                scanf("%d",&tr);
            }
            //mod_lor_10_12_07++
            tr = tr+FIRST_SENSOR_ID-1;  
            //mod_lor_10_12_07--
            pthread_mutex_lock( &actdiff_exclu  ) ; 
            add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm, msg_cmm_user_disc(tr) );
            pthread_mutex_unlock( &actdiff_exclu ) ;

        }
        else if (c == 'l'){ //add_lor_10_11_09
            printf("Digit id of users involved in the link to remove (from 1 to 3)  ... \n");
            scanf("%d",&tr);
            while (tr<1||tr>3){
                printf(" User id not valid!\nSelect user (from 1 to 3)  ... \n");
                scanf("%d",&tr);
            }
            scanf("%d",&rec);
            while (rec<1||rec>3||rec==tr){
                if (rec==tr)
                    printf(" A user cannot have a link with itself!\n");
                else
                    printf(" User id not valid!\n");
                printf("Select user (from 1 to 3)  ... \n");
                scanf("%d",&rec);
            }
            //mod_lor_10_12_07++
            tr = tr+FIRST_SENSOR_ID-1;  
            rec = rec+FIRST_SENSOR_ID-1;
            //mod_lor_10_12_07--
            
            pthread_mutex_lock( &actdiff_exclu  ) ; 
            add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm, msg_cmm_link_disc(tr,rec) );
            pthread_mutex_unlock( &actdiff_exclu ) ;

        }
        else if (c == 'c'){//add_lor_10_11_08
            printf("Starting collaborative sensing ... \n\n");
            unsigned int     Nb_channels= (Stop_fr-Start_fr)/Meas_band; 
            if (Nb_channels>NB_SENS_MAX){
                printf("ERROR! too many channels! Maximum number of channels is %d",NB_SENS_MAX);
                break;
            }
            pthread_mutex_lock( &actdiff_exclu  ) ; 
            add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm,
                    msg_cmm_init_coll_sensing(FC_ID,Start_fr,Stop_fr,Meas_band,Meas_tpf,
                    Nb_channels,Overlap, Sampl_freq) );
            pthread_mutex_unlock( &actdiff_exclu ) ;  
        }
        else if (c == 'e'){//mod_lor_10_04_27
            printf("Ending sensing ... \n\n");
            pthread_mutex_lock( &actdiff_exclu  ) ; 
            add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm,
                    msg_cmm_stop_sensing(FC_ID) );

            pthread_mutex_unlock( &actdiff_exclu ) ;  //mod_lor: 10_02_09--*/
        }
        else if (c == 'a'){
            if (BTS_ID>=0){
                printf("Activating BTS to ask for frequencies  ... \n\n");
                pthread_mutex_lock( &actdiff_exclu  ) ; 
                add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm, msg_cmm_ask_freq(BTS_ID) );
                pthread_mutex_unlock( &actdiff_exclu ) ;
            }
            //mod_lor_10_10_28++
            //mod_lor_10_10_28--
            else if (SCEN_2_CENTR){
                printf("Select user that wants to transmit (from 1 to 3)  ... \n");
                scanf("%d",&tr);
                while (tr<1||tr>3){
                    printf(" User id not valid!\nSelect user that wants to transmit (from 1 to 3)  ... \n");
                    scanf("%d",&tr);
                }
                printf("Select user destination of the transmission(from 1 to 3)  ... \n");
                scanf("%d",&rec);
                while (rec<1||rec>3||rec==tr){
                    if (rec==tr)
                        printf(" Receiver could not be equal to transmitter!\n");
                    else
                        printf(" User id not valid!\n");
                    printf("Select user destination (from 1 to 3)  ... \n");
                    scanf("%d",&rec);
                }
                //mod_lor_10_12_07++
                tr = tr+FIRST_SENSOR_ID-1;  
                rec = rec+FIRST_SENSOR_ID-1;
                //mod_lor_10_12_07--
                    
                pthread_mutex_lock( &actdiff_exclu  ) ; 
                add_actdiff(&list_actdiff,0, cnt_actdiff++, &s_cmm, msg_cmm_need_to_tx(tr,rec,1) );
                pthread_mutex_unlock( &actdiff_exclu ) ;
            }
            else
                printf("BTS not connected  ... \n\n");
        }
        
    }
    //mod_lor_10_04_27--*/    
    
    
#ifdef RRC_EMUL
    close_socket(&s_rrc);
#endif /* RRC_EMUL */

#ifdef SNS_EMUL
    close_socket(&s_sns);
#endif /* SNS_EMUL */

    close_socket(&s_cmm);

#ifdef PUSU_EMUL
    close_socket(&s_pusu);
#endif /*PUSU_EMUL */

    /* Attente de la fin des threads. */
    pthread_join (pthread_cmm_hnd, NULL);
#ifdef PUSU_EMUL
    pthread_join (pthread_pusu_hnd, NULL);
#endif /*PUSU_EMUL */
#ifdef RRC_EMUL
    pthread_join (pthread_rrc_hnd, NULL);
#endif /* RRC_EMUL */
#ifdef SNS_EMUL
    pthread_join (pthread_sns_hnd, NULL);
#endif /* SNS_EMUL */
    pthread_join (pthread_action_differe_hnd, NULL);

    return 0 ;
}
