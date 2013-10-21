#include <stdlib.h>
#include <stdio.h>

#include "intertask_interface.h"

#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_statistics.h"

int mme_app_statistics_display(void)
{
    fprintf(stdout, "================== Statistics ==================\n");
    fprintf(stdout, "        |   Global   | Since last display |\n");
    fprintf(stdout, "UE      | %10u |     %10u     |\n",
            mme_app_desc.mme_ue_contexts.nb_ue_managed,
            mme_app_desc.mme_ue_contexts.nb_ue_since_last_stat);
    fprintf(stdout, "Bearers | %10u |     %10u     |\n",
            mme_app_desc.mme_ue_contexts.nb_bearers_managed,
            mme_app_desc.mme_ue_contexts.nb_bearers_since_last_stat);

    mme_app_desc.mme_ue_contexts.nb_ue_since_last_stat = 0;
    mme_app_desc.mme_ue_contexts.nb_bearers_since_last_stat = 0;

    return 0;
}
