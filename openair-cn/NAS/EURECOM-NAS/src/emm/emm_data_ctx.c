
#include <stdlib.h>
#include <string.h>

#if defined(EPC_BUILD) && defined(NAS_MME)
#include "assertions.h"
#include "tree.h"
#include "emmData.h"

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

struct emm_data_context_s * emm_data_context_remove(
    emm_data_t *emm_data, struct emm_data_context_s *elm)
{
    return RB_REMOVE(emm_data_context_map, &emm_data->ctx_map, elm);
}

void emm_data_context_add(emm_data_t *emm_data, struct emm_data_context_s *elm)
{
    RB_INSERT(emm_data_context_map, &emm_data->ctx_map, elm);
}

#endif
