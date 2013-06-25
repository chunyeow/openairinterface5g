/*******************************************************************************

Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2010 Eurecom

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
#define RLC_AM_MODULE
#define RLC_AM_WINDOWS_C
//-----------------------------------------------------------------------------
#ifdef USER_MODE
#include <assert.h>
#endif
//-----------------------------------------------------------------------------
//#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am.h"
#include "UTIL/LOG/log.h"
//-----------------------------------------------------------------------------
signed int rlc_am_in_tx_window(rlc_am_entity_t* rlcP, u16_t snP)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if (snP >= RLC_AM_SN_MODULO) {
        return 0;
    }
    shifted_sn  = ((u16_t)(snP - rlcP->vt_a)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(rlcP->vt_ms - rlcP->vt_a)) % RLC_AM_SN_MODULO;
    if ((shifted_sn >= 0) && (shifted_sn < upper_bound)) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_in_rx_window(rlc_am_entity_t* rlcP, u16_t snP)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if (snP >= RLC_AM_SN_MODULO) {
        return 0;
    }
    shifted_sn  = ((u16_t)(snP - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(rlcP->vr_mr - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    if ((shifted_sn >= 0) && (shifted_sn < upper_bound)) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_sn_gte_vr_h(rlc_am_entity_t* rlcP, u16_t snP)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if (snP >= RLC_AM_SN_MODULO) {
        return 0;
    }
    shifted_sn  = ((u16_t)(snP - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(rlcP->vr_h - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    if (shifted_sn >= upper_bound) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_sn_gte_vr_x(rlc_am_entity_t* rlcP, u16_t snP)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if (snP >= RLC_AM_SN_MODULO) {
        return 0;
    }
    shifted_sn  = ((u16_t)(snP - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(rlcP->vr_x - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    if (shifted_sn >= upper_bound) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_sn_gt_vr_ms(rlc_am_entity_t* rlcP, u16_t snP)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if (snP >= RLC_AM_SN_MODULO) {
        return 0;
    }
    shifted_sn  = ((u16_t)(snP - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(rlcP->vr_ms - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    if (shifted_sn > upper_bound) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_tx_sn1_gt_sn2(rlc_am_entity_t* rlcP, u16_t sn1P, u16_t sn2P)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if ((sn1P >= RLC_AM_SN_MODULO) || (sn2P >= RLC_AM_SN_MODULO)) {
        return 0;
    }
    shifted_sn  = ((u16_t)(sn1P - rlcP->vt_a)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(sn2P - rlcP->vt_a)) % RLC_AM_SN_MODULO;
    if (shifted_sn > upper_bound) {
        return 1;
    } else {
        return 0;
    }
}
//-----------------------------------------------------------------------------
signed int rlc_am_rx_sn1_gt_sn2(rlc_am_entity_t* rlcP, u16_t sn1P, u16_t sn2P)
//-----------------------------------------------------------------------------
{
    u16_t shifted_sn;
    u16_t upper_bound;

    if ((sn1P >= RLC_AM_SN_MODULO) || (sn2P >= RLC_AM_SN_MODULO)) {
        return 0;
    }
    shifted_sn  = ((u16_t)(sn1P - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    upper_bound = ((u16_t)(sn2P - rlcP->vr_r)) % RLC_AM_SN_MODULO;
    if (shifted_sn > upper_bound) {
        return 1;
    } else {
        return 0;
    }
}
