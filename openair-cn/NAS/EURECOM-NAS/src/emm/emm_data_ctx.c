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

#include <stdlib.h>
#include <string.h>

#if defined(EPC_BUILD) && defined(NAS_MME)
#include "assertions.h"
#include "tree.h"
#include "emmData.h"
#include "nas_log.h"

static inline
int emm_data_ctxt_compare_ueid(struct emm_data_context_s *p1,
                               struct emm_data_context_s *p2);

RB_PROTOTYPE(emm_data_context_map, emm_data_context_s, entries,
             emm_data_ctxt_compare_ueid);

/* Generate functions used for the MAP */
RB_GENERATE(emm_data_context_map, emm_data_context_s, entries,
            emm_data_ctxt_compare_ueid);

static inline
int emm_data_ctxt_compare_ueid(struct emm_data_context_s *p1,
                               struct emm_data_context_s *p2)
{
    if (p1->ueid > p2->ueid) {
        return 1;
    }
    if (p1->ueid < p2->ueid) {
        return -1;
    }
    /* Matching reference -> return 0 */
    return 0;
}

struct emm_data_context_s *emm_data_context_get(
    emm_data_t *emm_data, unsigned int _ueid)
{
    struct emm_data_context_s reference;

    DevAssert(emm_data != NULL);
    DevCheck(_ueid > 0, _ueid, 0, 0);

    memset(&reference, 0, sizeof(struct emm_data_context_s));
    reference.ueid = _ueid;
    return RB_FIND(emm_data_context_map, &emm_data->ctx_map, &reference);
}

struct emm_data_context_s *emm_data_context_remove(
    emm_data_t *emm_data, struct emm_data_context_s *elm)
{
    LOG_TRACE(INFO, "EMM-CTX - Remove in context %p UE id %u", elm, elm->ueid);
    return RB_REMOVE(emm_data_context_map, &emm_data->ctx_map, elm);
}

void emm_data_context_add(emm_data_t *emm_data, struct emm_data_context_s *elm)
{
    LOG_TRACE(INFO, "EMM-CTX - Add in context %p UE id %u", elm, elm->ueid);
    RB_INSERT(emm_data_context_map, &emm_data->ctx_map, elm);
}

#endif
