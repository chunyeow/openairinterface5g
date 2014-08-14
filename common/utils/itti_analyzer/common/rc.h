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

#include <errno.h>
#include <string.h>

#ifndef RC_H_
#define RC_H_

#define RC_OK 0
#define RC_FAIL         -1
#define RC_BAD_PARAM    -2
#define RC_NULL_POINTER -3

static const char * const rc_strings[] =
    {"Ok", "fail", "bad parameter", "null pointer"};

#define CHECK_FCT(fCT)              \
do {                                \
    int rET;                        \
    if ((rET = fCT) != RC_OK) {     \
        fprintf(stderr, #fCT" has failed (%s:%d)\n", __FILE__, __LINE__);   \
        return rET;                 \
    }                               \
} while(0)

#define CHECK_FCT_POSIX(fCT)        \
do {                                \
    if (fCT == -1) {                \
        fprintf(stderr, #fCT" has failed (%d:%s) (%s:%d)\n", errno, \
                strerror(errno), __FILE__, __LINE__);               \
        return RC_FAIL;             \
    }                               \
} while(0)

#define CHECK_FCT_DO(fCT, dO)       \
do {                                \
    int rET;                        \
    if ((rET = fCT) != RC_OK) {     \
        fprintf(stderr, #fCT" has returned %d (%s:%d)\n", rET, __FILE__, __LINE__);   \
        dO;                         \
    }                               \
} while(0)

#define CHECK_BUFFER(bUFFER)        \
do {                                \
    if ((bUFFER) == NULL) {         \
        fprintf(stderr, #bUFFER" is NULL (%s:%d)\n", __FILE__, __LINE__);   \
        return RC_NULL_POINTER;     \
    }                               \
} while(0)

#endif /* RC_H_ */
