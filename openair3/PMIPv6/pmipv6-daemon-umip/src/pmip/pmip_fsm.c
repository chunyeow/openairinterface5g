/*
 * This file is part of the PMIP, Proxy Mobile IPv6 for Linux.
 *
 * Authors: OPENAIR3 <openair_tech@eurecom.fr>
 *
 * Copyright 2010-2011 EURECOM (Sophia-Antipolis, FRANCE)
 * 
 * Proxy Mobile IPv6 (or PMIPv6, or PMIP) is a network-based mobility 
 * management protocol standardized by IETF. It is a protocol for building 
 * a common and access technology independent of mobile core networks, 
 * accommodating various access technologies such as WiMAX, 3GPP, 3GPP2 
 * and WLAN based access architectures. Proxy Mobile IPv6 is the only 
 * network-based mobility management protocol standardized by IETF.
 * 
 * PMIP Proxy Mobile IPv6 for Linux has been built above MIPL free software;
 * which it involves that it is under the same terms of GNU General Public
 * License version 2. See MIPL terms condition if you need more details. 
 */
/*! \file pmip_fsm.c
 * \brief
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair_tech@eurecom.fr
 */
#define PMIP
#define PMIP_FSM_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_fsm.h"
#include "pmip_hnp_cache.h"
#include "pmip_lma_proc.h"
#include "pmip_mag_proc.h"
#include "pmip_handler.h"
//---------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
int mag_init_fsm(void)
{
    if (pthread_rwlock_init(&fsm_lock, NULL))
        return -1;
    else
        return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int mag_fsm(msg_info_t * info)
{
    int result = 0;
    int aaa_result = 0;
    pmip_entry_t *bce;
    struct in6_addr prefix;
    struct in6_addr hw_address = EUI64_to_EUI48(info->mn_iid);
    int type = pmip_cache_exists(&conf.OurAddress, &hw_address);
    int mutex_return_code;


    mutex_return_code = pthread_rwlock_wrlock(&fsm_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_wrlock(&fsm_lock) %s\n", strerror(mutex_return_code));
    }
    switch (type) {
    //--------------------------------------
    case BCE_NO_ENTRY:
        dbg("BCE_NO_ENTRY\n");
        if (info->iif == g_ingress_iif) {
          if (info->msg_event == hasRS) {
              dbg("New MN is found sending RS, start new registration ...\n\n");
              bce = pmip_cache_alloc(BCE_TEMP);
              prefix = mnid_hnp_map(hw_address, &aaa_result);
              if (aaa_result >= 0) {
                  bce->mn_prefix = prefix;
                  bce->mn_suffix = info->mn_iid;
                  bce->mn_hw_address = EUI64_to_EUI48(info->mn_iid);
                  info->mn_prefix = prefix;
                  result = mag_pmip_md(info, bce);
                  dbg("Movement detection is finished, now going to add an entry into the cache\n\n");
                  pmip_cache_add(bce);
                  dbg("pmip_cache_add is done \n\n");
              } else {
                  dbg("Authentication failed\n");
              }
              //yet to process
          } else if (info->msg_event == hasWLCCP) {
              dbg("Incoming MN is detected by Wireless Access Point, start new registration ...\n\n");
              bce = pmip_cache_alloc(BCE_TEMP);
              prefix = mnid_hnp_map(hw_address, &aaa_result);
              if (aaa_result >= 0) {
                  bce->mn_prefix = prefix;
                  bce->mn_suffix = info->mn_iid;
                  bce->mn_hw_address = hw_address;
                  info->mn_prefix = prefix;
                  result = mag_pmip_md(info, bce);
                  dbg("Movement detection is finished, now going to add an entry into the cache\n\n");
                  pmip_cache_add(bce);
                  dbg("pmip_cache_add is done \n\n");
              } else {
                  dbg("Authentication failed\n");
              }
              //yet to process
          } else if (info->msg_event == hasDEREG) {
              dbg("Received DEREG message\n");
              dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
          }
        } else {
            dbg("Received Message on wrong interface index (%d instead of %d), ignoring this message\n\n", info->iif,  g_ingress_iif);
        }
        break;
        //--------------------------------------
    case BCE_TEMP:
        dbg("BCE_TEMP\n");
        if (info->msg_event == hasPBA) {
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            if (is_pba_is_response_to_last_pbu_sent(info, bce)) {
                dbg("Handling PBA. Moving from BCE_TEMP to BCE_PMIP\n");
                dbg("Finish Location Registration\n");
                //Modify the entry with additional info.
                del_task(&bce->tqe);    //Delete timer retransmission PBU (if any)
                bce->PBA_flags = info->PBA_flags;
                // trick not to let LMA break the tunnel a few milli-seconds before MAG send a new PBU -> loss of traffic
                if (info->lifetime.tv_sec > 10) {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec - 8;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 8 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                } else if (info->lifetime.tv_sec > 5) {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec - 3;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 3 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                } else if (info->lifetime.tv_sec > 1) {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec - 1;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 1 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                } else {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) =  %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                }
                dbg("Prefix before ending registration : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
                // LG COMMENT GOT PREFIX BY RADIUS - bce->mn_prefix = info->mn_prefix;   //adding the hn prefix value receive in PBA to MAG cache
                mag_end_registration(bce, info->iif);
            }
            pmipcache_release_entry(bce);
        }
        break;
        //--------------------------------------
    case BCE_PMIP:
        dbg("BCE_PMIP\n");
        if (info->msg_event == hasRS) {
            dbg("Router solicitation received for existing MN\n");
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            dbg("prefix before entering kickoff_ra : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
            mag_kickoff_ra(bce);
            pmipcache_release_entry(bce);
            dbg("RA sent after router solicitation ...\n");
        } else if (info->msg_event == hasPBA) {
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            if (is_pba_is_response_to_last_pbu_sent(info, bce)) {
                dbg("Finish Location Registration\n");
                //Modify the entry with additional info.
                del_task(&bce->tqe);    //Delete timer retransmission PBU (if any)
                bce->PBA_flags = info->PBA_flags;
                // trick not to let LMA break the tunnel a few milli-seconds before MAG send a new PBU -> loss of traffic
                bce->lifetime.tv_nsec = 0;
                if (info->lifetime.tv_sec > 10) {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec - 8;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 8 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                } else if (info->lifetime.tv_sec > 5) {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec - 3;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 3 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                } else if (info->lifetime.tv_sec > 1) {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec - 1;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) - 1 second = %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                } else {
                    bce->lifetime.tv_sec = info->lifetime.tv_sec;
                    dbg("Set BCE lifetime to PBA lifetime (%d seconds) =  %d seconds\n", info->lifetime.tv_sec, bce->lifetime.tv_sec);
                }
                dbg("Prefix before ending registration : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
                // LG COMMENT GOT PREFIX BY RADIUS - bce->mn_prefix = info->mn_prefix;   //adding the hn prefix value receive in PBA to MAG cache
                mag_end_registration_no_new_tunnel(bce, info->iif);
            }
            pmipcache_release_entry(bce);
        } else if (info->msg_event == hasWLCCP) {
            dbg("Incomming MN is detected by Wireless Access Point, existing MN\n");
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            dbg("Prefix before entering kickoff_ra : %x:%x:%x:%x:%x:%x:%x:%x \n", NIP6ADDR(&bce->mn_prefix));
            mag_kickoff_ra(bce);
            // Some case where the access point did not detect the departure of the mobile node
            // so we have to register again to the LMA
            mag_force_update_registration(bce, info->iif);
            pmipcache_release_entry(bce);
            dbg("RA sent after MN AP detection ...\n");
        } else if (info->msg_event == hasDEREG) {
            dbg("Deregistration procedure detected by Wireless Access Point for a registered MN\n");
            dbg("Start Location Deregistration\n");
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            mag_dereg(bce, 1);
            pmipcache_release_entry(bce);
            pmip_bce_delete(bce);
        } else if (info->msg_event == hasNA) {
            //Reset counter, Delete task for entry deletion  & Add a new task for NS expiry.
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            bce->n_rets_counter = conf.MaxMessageRetransmissions;    //Reset the Retransmissions Counter.
            dbg("Reset the Reachability Counter = %d for %x:%x:%x:%x:%x:%x:%x:%x\n", bce->n_rets_counter, NIP6ADDR(&info->mn_iid));
            del_task(&bce->tqe);
            mag_force_update_registration(bce, info->iif);
            //pmip_cache_start(bce);
            pmipcache_release_entry(bce);
        }
        break;
    default:
        dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
    }
    mutex_return_code = pthread_rwlock_unlock(&fsm_lock);
    if (mutex_return_code != 0) {
        dbg("pthread_rwlock_unlock(&fsm_lock) %s\n", strerror(mutex_return_code));
    }
    return result;
}
//---------------------------------------------------------------------------------------------------------------------
int lma_fsm(msg_info_t * info)
{
    struct in6_addr r_tmp1;
    int result = 0;
    pmip_entry_t *bce = NULL;
    struct in6_addr hw_address = EUI64_to_EUI48(info->mn_iid);
    int type = pmip_cache_exists(&conf.OurAddress, &hw_address);
    switch (type) {
    //--------------------------------------
    case BCE_NO_ENTRY:
        dbg("No PMIP entry found for %x:%x:%x:%x:%x:%x:%x:%x ... \n", NIP6ADDR(&info->mn_iid));
        if (info->msg_event == hasPBU && (info->lifetime.tv_sec > 0)) {
            //Create New Proxy Binding Entry storing information
            dbg("PBU for a new MN ... Location Registration starting now...\n");
            bce = pmip_cache_alloc(BCE_PMIP);
            if (bce != NULL) {
                pmip_insert_into_hnp_cache(hw_address, info->mn_prefix);
                lma_update_binding_entry(bce, info);   //Save information into bce
                lma_reg(bce);
                pmip_cache_add(bce);
            }
        } else if ((info->msg_event == hasPBU) && (info->lifetime.tv_sec == 0) && (info->lifetime.tv_nsec == 0)) {
            dbg("PBU with Lifetime = 0 for a not-registered MN... \n");
            // LG Comment lma_dereg(bce, info, 0);
            // LG Comment pmipcache_release_entry(bce);
        }

        break;
        //--------------------------------------
    case BCE_PMIP:
        if ((info->msg_event == hasPBU) && (info->lifetime.tv_sec > 0)) {
            dbg("PBU for an existing MN ... update serving MAG\n");
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            memset(&r_tmp1, 0, sizeof(struct in6_addr));
            if (IN6_ARE_ADDR_EQUAL(&r_tmp1, &bce->mn_serv_mag_addr)) //Received PBU from serving MAG
            {
                lma_update_binding_entry(bce, info);
                lma_reg(bce);
            }
            if (IN6_ARE_ADDR_EQUAL(&info->src, &bce->mn_serv_mag_addr) ) //Received PBU from serving MAG
            {
                lma_update_binding_entry(bce, info);
                lma_reg_no_new_tunnel (bce);
            } else { //Received PBU from an already unregistered MAG
                dbg("Deregistration for previously registered MAG\n");
                lma_dereg(bce, info, 0);
                bce->type = BCE_PMIP;
                lma_update_binding_entry(bce, info);
                lma_reg(bce);
            }
            pmipcache_release_entry(bce);
        } else if (info->msg_event == hasPBU && info->lifetime.tv_sec == 0 && info->lifetime.tv_nsec == 0) {
            dbg("PBU with Lifetime = 0... start Location Deregistration\n");
            bce = pmip_cache_get(&conf.OurAddress, &hw_address);
            if (IN6_ARE_ADDR_EQUAL(&info->src, &bce->mn_serv_mag_addr)) //Received PBU from serving MAG
            {
                dbg("Deregistration case...\n");
                lma_dereg(bce, info, 1);
                pmipcache_release_entry(bce);
                pmip_bce_delete(bce);
            } else { //Received PBU from an already unregistered MAG
                dbg("Deregistration for not registered MAG, doing nothing\n");
                //lma_dereg(bce, info, 0);
                pmipcache_release_entry(bce);
            }
        }
        // update bce->mn_addr attribute for vt display
        get_mn_addr(bce);
        break;
        //--------------------------------------
    default:
        dbg("No action for this event (%d) at current state (%d) !\n", info->msg_event, type);
    }
    return result;
}
