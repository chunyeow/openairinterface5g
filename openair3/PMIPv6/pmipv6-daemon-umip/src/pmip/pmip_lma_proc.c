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
/*! \file pmip_lma_proc.c
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/
#define PMIP
#define PMIP_LMA_PROC_C
#ifdef HAVE_CONFIG_H
#    include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_handler.h"
#include "pmip_hnp_cache.h"
#include "pmip_lma_proc.h"
#include "pmip_tunnel.h"
//---------------------------------------------------------------------------------------------------------------------
#include "rtnl.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//---------------------------------------------------------------------------------------------------------------------
int lma_setup_route(struct in6_addr *pmip6_addr, int tunnel)
{
    int res = 0;
    if (conf.TunnelingEnabled) {
        dbg("Forward: Add new route for %x:%x:%x:%x:%x:%x:%x:%x in table %d\n", NIP6ADDR(pmip6_addr), RT6_TABLE_MIP6);
        res = route_add(tunnel, RT6_TABLE_MIP6, RTPROT_MIP, 0, IP6_RT_PRIO_MIP6_FWD, &in6addr_any, 0, pmip6_addr, 128, NULL);
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------
int lma_remove_route(struct in6_addr *pmip6_addr, int tunnel)
{
    int res = 0;
    if (conf.TunnelingEnabled) {
        //Delete existing rule for the deleted MN
        dbg("Delete old route for: %x:%x:%x:%x:%x:%x:%x:%x from table %d\n", NIP6ADDR(pmip6_addr), RT6_TABLE_MIP6);
        res = route_del(tunnel, RT6_TABLE_MIP6, IP6_RT_PRIO_MIP6_FWD, &in6addr_any, 0, pmip6_addr, 128, NULL);
    }
    return res;
}
//---------------------------------------------------------------------------------------------------------------------
int lma_reg(pmip_entry_t * bce)
{
    if (bce != NULL) {
        //create a tunnel between MAG and LMA && add a route for peer address.
        bce->tunnel = pmip_tunnel_add(&conf.OurAddress, &bce->mn_serv_mag_addr, bce->link);
        lma_setup_route(get_mn_addr(bce), bce->tunnel);
        bce->status = 0;        //PBU was Accepted!
        //Add task for entry expiry.
        pmip_cache_start(bce);
        //Send a PBA to ack new serving MAG
        dbg("Create PBA to new Serving MAG...\n");
        struct in6_addr_bundle addrs;
        addrs.src = &conf.OurAddress;
        addrs.dst = &bce->mn_serv_mag_addr;
        mh_send_pba(&addrs, bce, &bce->lifetime, 0);
        return 0;
    } else {
        dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
        return -1;
    }
}
//---------------------------------------------------------------------------------------------------------------------
int lma_reg_no_new_tunnel(pmip_entry_t * bce)
{
    if (bce != NULL) {
        bce->status = 0;        //PBU was Accepted!
        //Add task for entry expiry.
        pmip_cache_start(bce);
        //Send a PBA to ack new serving MAG
        dbg("Create PBA to new Serving MAG...\n");
        struct in6_addr_bundle addrs;
        addrs.src = &conf.OurAddress;
        addrs.dst = &bce->mn_serv_mag_addr;
        mh_send_pba(&addrs, bce, &bce->lifetime, 0);
        return 0;
    } else {
        dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
        return -1;
    }
}
//---------------------------------------------------------------------------------------------------------------------
int lma_dereg(pmip_entry_t * bce, msg_info_t * info, int propagate)
{
    if (bce != NULL) {
        //Delete the Task
        del_task(&bce->tqe);
        //delete old route to old tunnel.
        lma_remove_route(get_mn_addr(bce), bce->tunnel);
        //decrement users of old tunnel.
        pmip_tunnel_del(bce->tunnel);
        if (propagate) {
            dbg("Create PBA for deregistration for MAG (%x:%x:%x:%x:%x:%x:%x:%x)\n", NIP6ADDR(&bce->mn_serv_mag_addr));
            struct in6_addr_bundle addrs;
            struct timespec lifetime = { 0, 0 };
            addrs.src = &conf.LmaAddress;
            addrs.dst = &bce->mn_serv_mag_addr;
            bce->seqno_in = info->seqno;
            mh_send_pba(&addrs, bce, &lifetime, 0);
        } else {
            dbg("Doing nothing....\n");
        }
        bce->type = BCE_NO_ENTRY;
    } else {
        dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
        return -1;
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int lma_update_binding_entry(pmip_entry_t * bce, msg_info_t * info)
{
	int result = 0;
    if (bce != NULL) {
        if (info != NULL) {
            int result;
            struct in6_addr r_tmp, r_tmp1;
            memset(&r_tmp1, 0, sizeof(struct in6_addr));
            dbg("Store Binding Entry\n");
            bce->our_addr = conf.OurAddress;
            bce->mn_suffix = info->mn_iid;
            bce->mn_hw_address = EUI64_to_EUI48(info->mn_iid);
            dbg("searching for the prefix for a new BCE entry...\n");
            r_tmp = lma_mnid_hnp_map(bce->mn_hw_address, &result);
            if (result >= 0) {
                if (IN6_ARE_ADDR_EQUAL(&r_tmp, &info->mn_prefix)) {
                    bce->mn_prefix = r_tmp;
                    dbg("found the prefix  %x:%x:%x:%x:%x:%x:%x:%x in lma_update_binding entry \n", NIP6ADDR(&bce->mn_prefix));

                    if (IN6_ARE_ADDR_EQUAL(&bce->mn_serv_mag_addr, &r_tmp1)) {
                        dbg("First new serving MAG:  %x:%x:%x:%x:%x:%x:%x:%x  \n", NIP6ADDR(&info->src));
                        result = 1;
                    } else if (!(IN6_ARE_ADDR_EQUAL(&bce->mn_serv_mag_addr, &info->src))) {
                        dbg("New serving MAG:  %x:%x:%x:%x:%x:%x:%x:%x  (Old MAG: %x:%x:%x:%x:%x:%x:%x:%x)\n", NIP6ADDR(&info->src), NIP6ADDR(&bce->mn_serv_mag_addr));
                        //delete old route to old tunnel.
                        lma_remove_route(get_mn_addr(bce), bce->tunnel);
                        //decrement users of old tunnel.
                        dbg("Deleting the old tunnel \n");
                        pmip_tunnel_del(bce->tunnel);
                        result = 1;
                    } else {
                        dbg("Same serving MAG: %x:%x:%x:%x:%x:%x:%x:%x, No need to delete tunnel\n", NIP6ADDR(&info->src));
                        // to do here in future: cancel possible delete timer on this tunnel
                        result = 0;
                    }
                } else {
                    dbg("Mobine node prefix changed, delete route, tunnel\n");
                    //delete old route to old tunnel.
                    lma_remove_route(get_mn_addr(bce), bce->tunnel);
                    //decrement users of old tunnel.
                    pmip_tunnel_del(bce->tunnel);
                    dbg("Deleting the old tunnel \n");
                    result = 1;
                }


            }
            bce->timestamp.first    = info->timestamp.first;
            bce->timestamp.second   = info->timestamp.second;
            bce->mn_prefix          = info->mn_prefix;
            //bce->mn_addr            = info->mn_addr;
            bce->mn_link_local_addr = info->mn_link_local_addr;
            bce->mn_serv_mag_addr   = info->src;
            bce->lifetime.tv_sec    = info->lifetime.tv_sec;
            bce->lifetime.tv_nsec   = 0;
            bce->n_rets_counter     = conf.MaxMessageRetransmissions;
            bce->seqno_in           = info->seqno;

			// update bce->mn_addr with bce->mn_prefix and bce->mn_suffix
			get_mn_addr(bce);
			
            dbg("bce->link %d => %d\n", bce->link, info->iif);
            bce->link               = info->iif;
            dbg("Finished updating the binding cache\n");
            return result;
        } else {
            dbg("WARNING parameter msg_info_t* info is NULL\n");
            result = -1;
            return result;
        }
    } else {
        dbg("WARNING parameter pmip_entry_t * bce is NULL\n");
        result = -1;
        return result;
    }
}
