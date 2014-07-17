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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "assertions.h"
#include "conversions.h"

#include "mme_config.h"

#include "s1ap_mme.h"
#include "s1ap_common.h"
#include "s1ap_mme_ta.h"

static
int s1ap_mme_compare_plmn(S1ap_PLMNidentity_t *plmn)
{
    int i;
    uint16_t mcc;
    uint16_t mnc;
    uint16_t mnc_len;

    DevAssert(plmn != NULL);

    TBCD_TO_MCC_MNC(plmn, mcc, mnc, mnc_len);

    config_read_lock(&mme_config);

    for (i = 0; i < mme_config.gummei.nb_mme_gid; i++) {
        S1AP_DEBUG("Comparing plmn_mcc %d/%d, plmn_mnc %d/%d plmn_mnc_len %d/%d\n",
            mme_config.gummei.plmn_mcc[i], mcc,
            mme_config.gummei.plmn_mnc[i],mnc,
            mme_config.gummei.plmn_mnc_len[i],mnc_len);

        if ((mme_config.gummei.plmn_mcc[i] == mcc) &&
            (mme_config.gummei.plmn_mnc[i] == mnc) &&
            (mme_config.gummei.plmn_mnc_len[i] == mnc_len))
            /* There is a matching plmn */
            return TA_LIST_AT_LEAST_ONE_MATCH;
    }

    config_unlock(&mme_config);

    return TA_LIST_NO_MATCH;
}

/* @brief compare a list of broadcasted plmns against the MME configured.
 */
static
int s1ap_mme_compare_plmns(S1ap_BPLMNs_t *b_plmns)
{
    int i;
    int matching_occurence = 0;

    DevAssert(b_plmns != NULL);

    for (i = 0; i < b_plmns->list.count; i++) {
        if (s1ap_mme_compare_plmn(b_plmns->list.array[i])
            == TA_LIST_AT_LEAST_ONE_MATCH)
            matching_occurence++;
    }

    if (matching_occurence == 0)
        return TA_LIST_NO_MATCH;
    else if (matching_occurence == b_plmns->list.count - 1)
        return TA_LIST_COMPLETE_MATCH;
    else
        return TA_LIST_AT_LEAST_ONE_MATCH;
}

/* @brief compare a TAC
 */
static
int s1ap_mme_compare_tac(S1ap_TAC_t *tac)
{
    int i;
    uint16_t tac_value;

    DevAssert(tac != NULL);

    OCTET_STRING_TO_TAC(tac, tac_value);

    config_read_lock(&mme_config);

    for (i = 0; i < mme_config.gummei.nb_plmns; i++) {
        S1AP_DEBUG("Comparing config tac %d, received tac = %d\n", mme_config.gummei.plmn_tac[i], tac_value);
        if (mme_config.gummei.plmn_tac[i] == tac_value)
            return TA_LIST_AT_LEAST_ONE_MATCH;
    }

    config_unlock(&mme_config);

    return TA_LIST_NO_MATCH;
}

/* @brief compare a given ta list against the one provided by mme configuration.
 * @param ta_list
 * @return - TA_LIST_UNKNOWN_PLMN if at least one TAC match and no PLMN match
 *         - TA_LIST_UNKNOWN_TAC if at least one PLMN match and no TAC match
 *         - TA_LIST_RET_OK if both tac and plmn match at least one element
 */
int s1ap_mme_compare_ta_lists(S1ap_SupportedTAs_t *ta_list)
{
    int i;
    int tac_ret, bplmn_ret;

    DevAssert(ta_list != NULL);

    /* Parse every item in the list and try to find matching parameters */
    for (i = 0; i < ta_list->list.count; i++) {
        S1ap_SupportedTAs_Item_t *ta;

        ta = ta_list->list.array[i];
        DevAssert(ta != NULL);

        tac_ret = s1ap_mme_compare_tac(&ta->tAC);
        bplmn_ret = s1ap_mme_compare_plmns(&ta->broadcastPLMNs);
        if (tac_ret == TA_LIST_NO_MATCH && bplmn_ret == TA_LIST_NO_MATCH) {
            return TA_LIST_UNKNOWN_PLMN + TA_LIST_UNKNOWN_TAC;
        } else {
            if (tac_ret > TA_LIST_NO_MATCH && bplmn_ret == TA_LIST_NO_MATCH) {
                return TA_LIST_UNKNOWN_PLMN;
            } else if (tac_ret == TA_LIST_NO_MATCH && bplmn_ret > TA_LIST_NO_MATCH) {
                return TA_LIST_UNKNOWN_TAC;
            }
        }
    }
    return TA_LIST_RET_OK;
}
