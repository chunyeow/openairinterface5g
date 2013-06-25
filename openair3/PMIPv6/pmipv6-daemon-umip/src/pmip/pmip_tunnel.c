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
/*! \file pmip_tunnel.c
* \brief
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/
#define PMIP
#define PMIP_TUNNEL_C
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_tunnel.h"
#include "pmip_handler.h"
//---------------------------------------------------------------------------------------------------------------------
#include "tunnelctl.h"
#include "util.h"
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"
//-----------------------------------------------------------------------------
int pmip_tunnel_get_timer_index(struct in6_addr  *remoteP)
{
    int    mag;
    int    first_no_mag = -1;
    for (mag = 0 ; mag < PMIP_MAX_MAGS; mag++) {
        if (IN6_ARE_ADDR_EQUAL(&g_tunnel_timer_table[mag].remote, remoteP)) {
            return mag;
        } else if ((IN6_IS_ADDR_UNSPECIFIED(&g_tunnel_timer_table[mag].remote)) && (first_no_mag == -1)) {
            first_no_mag = mag;
        }
    }
    return first_no_mag;
}
//-----------------------------------------------------------------------------
int pmip_tunnels_init(void)
{
    unsigned int mag;
    unsigned int link;

    memset((void*)g_tunnel_timer_table, 0, sizeof(tunnel_timer_t) * PMIP_MAX_MAGS);
    for (mag = 0 ; mag < PMIP_MAX_MAGS; mag++) {
        INIT_LIST_HEAD(&g_tunnel_timer_table[mag].tqe.list);
    }
    // create static tunnels if requested
    if (conf.TunnelingEnabled) {
        if (conf.DynamicTunnelingEnabled == 0) {
            dbg("DynamicTunnelingEnabled is False\n");
            if (is_ha()) {
                for (mag = 0 ; mag < conf.NumMags; mag++) {
                    link = if_nametoindex(conf.LmaPmipNetworkDevice);
                    pmip_tunnel_add(&conf.LmaAddress, &conf.MagAddressEgress[mag], link);
                }
            } else if (is_mag()) {
                link = if_nametoindex(conf.MagDeviceEgress);
                pmip_tunnel_add(&conf.MagAddressEgress[0], &conf.LmaAddress, link);
            }
        } else {
            dbg("DynamicTunnelingEnabled is True\n");
		}
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_tunnel_set_timer(struct in6_addr  *remoteP, int tunnelP, struct timespec time_outP)
{
    struct timespec    now_time;
    struct timespec    delay_before_delete_time;
    struct timespec    expires;
    int                mag_index;

    tscpy(delay_before_delete_time, time_outP);

    clock_gettime(CLOCK_REALTIME, &now_time);

    tsadd(now_time, delay_before_delete_time, expires);

    mag_index = pmip_tunnel_get_timer_index(remoteP);
    dbg("Found mag index %d for mag@ %x:%x:%x:%x:%x:%x:%x:%x\n", mag_index, NIP6ADDR(remoteP));

    // No timer lauched for this link
    if ((g_tunnel_timer_table[mag_index].lifetime.tv_nsec == 0) && (g_tunnel_timer_table[mag_index].lifetime.tv_sec == 0)) {

        dbg("PMIP add task pmip_timer_tunnel_expired_handler in %d seconds for tunel %d\n", time_outP.tv_sec, tunnelP);
        tscpy(g_tunnel_timer_table[mag_index].lifetime, expires);
        add_task_abs(&expires, &g_tunnel_timer_table[mag_index].tqe, pmip_timer_tunnel_expired_handler);

    } else  if (tsafter(g_tunnel_timer_table[mag_index].lifetime, expires)) {

        dbg("PMIP del old task pmip_timer_tunnel_expired_handler\n");
        del_task(&g_tunnel_timer_table[mag_index].tqe);
        dbg("PMIP add new task pmip_timer_tunnel_expired_handler in %d seconds for tunnel %d\n", time_outP.tv_sec, tunnelP);
        tscpy(g_tunnel_timer_table[mag_index].lifetime, expires);
        add_task_abs(&expires, &g_tunnel_timer_table[mag_index].tqe, pmip_timer_tunnel_expired_handler);

    } else {
        dbg("No new timer set for tunnel since a timer is currently running with higher time-out %d\n");
    }
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_tunnel_add(struct in6_addr *localP, struct in6_addr *remoteP, int linkP)
{
    int                mag_index;

    if (conf.TunnelingEnabled) {
        // Time-out timer on this tunnel does not exist
        mag_index = pmip_tunnel_get_timer_index(remoteP);
        dbg("Found mag index %d for mag@ %x:%x:%x:%x:%x:%x:%x:%x\n", mag_index, NIP6ADDR(remoteP));
        if (conf.DynamicTunnelingEnabled) {
            if ((g_tunnel_timer_table[mag_index].lifetime.tv_nsec == 0) && (g_tunnel_timer_table[mag_index].lifetime.tv_sec == 0)) {
                int tunnel = tunnel_add(localP, remoteP, linkP, 0, 0); // -1 if error
                g_tunnel_timer_table[mag_index].tunnel = tunnel;
                g_tunnel_timer_table[mag_index].remote = *remoteP;
                dbg("Creating dynamic IP-in-IP tunnel %d link %d from %x:%x:%x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x:%x:%x...\n", tunnel, linkP, NIP6ADDR(localP), NIP6ADDR(remoteP));
                return tunnel;
            } else {
                dbg("PMIP del old task pmip_timer_tunnel_expired_handler\n");
                del_task(&g_tunnel_timer_table[mag_index].tqe);
                int usercount = tunnel_getusers(g_tunnel_timer_table[mag_index].tunnel);
                dbg("Keeping IP-in-IP tunnel %d link %d usercount %d\n", g_tunnel_timer_table[mag_index].tunnel, linkP, usercount);
                return g_tunnel_timer_table[mag_index].tunnel;
            }
        } else {
            if (IN6_ARE_ADDR_EQUAL(&g_tunnel_timer_table[mag_index].remote, remoteP)) {
                dbg("Static IP-in-IP tunnel %d link %d from %x:%x:%x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x:%x:%x...\n", g_tunnel_timer_table[mag_index].tunnel, linkP, NIP6ADDR(localP), NIP6ADDR(remoteP));
                return g_tunnel_timer_table[mag_index].tunnel;
            } else if (IN6_IS_ADDR_UNSPECIFIED(&g_tunnel_timer_table[mag_index].remote)) {
                int tunnel = tunnel_add(localP, remoteP, linkP, 0, 0); // -1 if error
                g_tunnel_timer_table[mag_index].tunnel = tunnel;
                g_tunnel_timer_table[mag_index].remote = *remoteP;
                dbg("Creating static IP-in-IP tunnel %d link %d from %x:%x:%x:%x:%x:%x:%x:%x to %x:%x:%x:%x:%x:%x:%x:%x...\n", tunnel, linkP, NIP6ADDR(localP), NIP6ADDR(remoteP));
                return tunnel;
            } else {
                return -1;
            }
        }
    } else {
        dbg("IP-in-IP tunneling is disabled, no tunnel is created\n");
        return 0;
    }
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_tunnel_del(int tunnelP)
{
    int res = 0;
    int mag_index;

    dbg(" tunnel %d\n", tunnelP);
    if (conf.TunnelingEnabled) {
        if (tunnelP > 0) {
            if (conf.DynamicTunnelingEnabled) {
                int usercount = tunnel_getusers(tunnelP);
                if (usercount == 1) {
                    // search if_index
                    for (mag_index = 0 ; mag_index < PMIP_MAX_MAGS; mag_index++) {
                        if (g_tunnel_timer_table[mag_index].tunnel == tunnelP) break;
                    }
                    assert(mag_index < PMIP_MAX_MAGS);
                    // Do not delete before MaxDelayBeforeDynamicTunnelingDelete
                    pmip_tunnel_set_timer(&g_tunnel_timer_table[mag_index].remote, tunnelP, conf.MaxDelayBeforeDynamicTunnelingDelete);
                } else {
                    // this will only decrease the number of users for this tunnel
                    res = tunnel_del(tunnelP, 0, 0);
                }
            } else {
                dbg("Static Tunneling, tunnel %d is not deleted\n", tunnelP);
            }
        } else {
            res = -1;
        }
    } else {
        dbg("IP-in-IP tunneling is disabled, no tunnel is deleted\n");
    }
    return res;
}
