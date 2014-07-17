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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gmp.h>
#include <sys/time.h>

#include "auc.h"

typedef struct random_state_s {
    pthread_mutex_t lock;
    gmp_randstate_t state;
} random_state_t;

random_state_t random_state;

void random_init(void)
{
//    mpz_t number;
//    pthread_mutex_init(&random_state.lock, NULL);
//    mpz_init(number);
//    gmp_randinit_default(random_state.state);
//    srand(time(NULL));
    struct timeval t1;
    gettimeofday(&t1, NULL);
    srand(t1.tv_usec * t1.tv_sec);
}

/* Generate a random number between 0 and 2^length - 1 where length is expressed
 * in bits.
 */
void generate_random(uint8_t *random_p, ssize_t length)
{
//    random_t random_nb;

//    mpz_init_set_ui(random_nb, 0);

//    pthread_mutex_lock(&random_state.lock);
//    mpz_urandomb(random_nb, random_state.state, 8 * length);
//    pthread_mutex_unlock(&random_state.lock);

//    mpz_export(random_p, NULL, 1, length, 0, 0, random_nb);
    int r = 0, i, mask = 0, shift;

    for (i = 0; i < length; i ++) {
//        if ((i % sizeof(i)) == 0)
//            r = rand();
//        shift = 8 * (i % sizeof(i));
//        mask = 0xFF << shift;
//        random_p[i] = (r & mask) >> shift;
        random_p[i] = rand();
    }
}
