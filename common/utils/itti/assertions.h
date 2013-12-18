/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#if defined(ENB_MODE)
# define display_backtrace()
#else
# include "backtrace.h"
#endif

#ifndef ASSERTIONS_H_
#define ASSERTIONS_H_

#define _Assert_(cOND, eXIT, fORMAT, aRGS...)                             \
do {                                                                      \
    if (!(cOND)) {                                                        \
        fprintf(stderr, "%s:%d:%s Assertion `"#cOND"` failed!\n" fORMAT,  \
                __FILE__, __LINE__, __FUNCTION__, ##aRGS);                \
        if (eXIT != 0) {                                                  \
            display_backtrace();                                          \
            fflush(stdout);                                               \
            fflush(stderr);                                               \
            exit(EXIT_FAILURE);                                           \
        }                                                                 \
    }                                                                     \
} while(0)

#define AssertFatal(cOND, fORMAT, aRGS...)  _Assert_(cOND, 1, fORMAT, ##aRGS)

#define AssertError(cOND, fORMAT, aRGS...)  _Assert_(cOND, 0, fORMAT, ##aRGS)



#define DevCheck(cOND, vALUE1, vALUE2, vALUE3)                            \
_Assert_(cOND, 1, #vALUE1": %d\n"#vALUE2": %d\n"#vALUE3": %d\n\n",        \
         (int)vALUE1, (int)vALUE2, (int)vALUE3)

#define DevParam(vALUE1, vALUE2, vALUE3)    DevCheck(0, vALUE1, vALUE2, vALUE3)

#define DevAssert(cOND)                     _Assert_(cOND, 1, "")

#define DevMessage(mESSAGE)                 _Assert_(0, 1, #mESSAGE)

#define CHECK_INIT_RETURN(fCT)                                  \
do {                                                            \
    int fct_ret;                                                \
    if ((fct_ret = (fCT)) != 0) {                               \
        fprintf(stderr, "Function "#fCT" has failed\n"          \
        "returning %d\n", fct_ret);                             \
        exit(EXIT_FAILURE);                                     \
    }                                                           \
} while(0)

#endif /* ASSERTIONS_H_ */
