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

#ifndef ASSERTIONS_H_
#define ASSERTIONS_H_

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#define _Assert_Exit_                           \
{                                               \
    fprintf(stderr, "\nExiting execution\n");   \
    fflush(stdout);                             \
    fflush(stderr);                             \
    abort();                                    \
}

#define _Assert_(cOND, aCTION, fORMAT, aRGS...)             \
do {                                                        \
    if (!(cOND)) {                                          \
        fprintf(stderr, "\nAssertion ("#cOND") failed!\n"   \
                "In %s() %s:%d\n" fORMAT,                   \
                __FUNCTION__, __FILE__, __LINE__, ##aRGS);  \
        aCTION;                                             \
    }                                                       \
} while(0)

#define AssertFatal(cOND, fORMAT, aRGS...)          _Assert_(cOND, _Assert_Exit_, fORMAT, ##aRGS)

#define AssertError(cOND, aCTION, fORMAT, aRGS...)  _Assert_(cOND, aCTION, fORMAT, ##aRGS)



#define DevCheck(cOND, vALUE1, vALUE2, vALUE3)                                                          \
_Assert_(cOND, _Assert_Exit_, #vALUE1": %"PRIdMAX"\n"#vALUE2": %"PRIdMAX"\n"#vALUE3": %"PRIdMAX"\n\n",  \
         (intmax_t)vALUE1, (intmax_t)vALUE2, (intmax_t)vALUE3)

#define DevCheck4(cOND, vALUE1, vALUE2, vALUE3, vALUE4)                                                                         \
_Assert_(cOND, _Assert_Exit_, #vALUE1": %"PRIdMAX"\n"#vALUE2": %"PRIdMAX"\n"#vALUE3": %"PRIdMAX"\n"#vALUE4": %"PRIdMAX"\n\n",   \
         (intmax_t)vALUE1, (intmax_t)vALUE2, (intmax_t)vALUE3, (intmax_t)vALUE4)

#define DevParam(vALUE1, vALUE2, vALUE3)    DevCheck(0, vALUE1, vALUE2, vALUE3)

#define DevAssert(cOND)                     _Assert_(cOND, _Assert_Exit_, "")

#define DevMessage(mESSAGE)                 _Assert_(0, _Assert_Exit_, #mESSAGE)

#endif /* ASSERTIONS_H_ */
