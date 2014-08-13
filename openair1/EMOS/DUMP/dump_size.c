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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
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
