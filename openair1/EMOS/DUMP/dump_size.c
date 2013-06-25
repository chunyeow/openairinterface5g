#include "gps.h"
#include "SCHED/phy_procedures_emos.h"

int main(void)
{
  printf("MAX_CQI_BITS = %d;\n",MAX_CQI_BITS);
  printf("PHY_measurements_size = %d;\n",sizeof(PHY_MEASUREMENTS));
  printf("PHY_measurements_eNb_size = %d;\n",sizeof(PHY_MEASUREMENTS_eNB));
  //printf("UCI_data_t_size = %d;\n",sizeof(UCI_DATA_t));
  //printf("DCI_alloc_t_size = %d;\n",sizeof(DCI_ALLOC_t));
  printf("eNb_UE_stats_size = %d;\n",sizeof(LTE_eNB_UE_stats));
  printf("fifo_dump_emos_UE_size = %d;\n",sizeof(fifo_dump_emos_UE));
  printf("fifo_dump_emos_eNb_size = %d;\n",sizeof(fifo_dump_emos_eNB));
  printf("gps_fix_t_size = %d\n",sizeof(struct gps_fix_t));

  return(0);
}
