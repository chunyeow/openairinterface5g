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

#ifndef ASSERTIONS_H_
#define ASSERTIONS_H_

#define DevCheck(cOND, vALUE1, vALUE2, vALUE3)                          \
do {                                                                    \
    if (!(cOND)) {                                                      \
        fprintf(stderr, "%s:%d:%s Assertion `"#cOND"` failed.\n",       \
                __FILE__, __LINE__, __FUNCTION__);                      \
        fprintf(stderr, #vALUE1": %d\n"#vALUE2": %d\n"#vALUE3": %d\n",  \
        (int)vALUE1, (int)vALUE2, (int)vALUE3);                         \
        abort();                                                        \
    }                                                                   \
} while(0)

#define DevCheck4(cOND, vALUE1, vALUE2, vALUE3, vALUE4)                 \
do {                                                                    \
    if (!(cOND)) {                                                      \
        fprintf(stderr, "%s:%d:%s\nAssertion `"#cOND"` failed.\n",      \
                __FILE__, __LINE__, __FUNCTION__);                      \
        fprintf(stderr, #vALUE1": %d\n"#vALUE2": %d\n"#vALUE3": %d\n"   \
        #vALUE4": %d\n",                                                \
        (int)vALUE1, (int)vALUE2, (int)vALUE3, (int)vALUE4);            \
        exit(EXIT_FAILURE);                                             \
    }                                                                   \
} while(0)

#define DevParam(vALUE1, vALUE2, vALUE3)    \
    DevCheck(0 == 1, vALUE1, vALUE2, vALUE3)

#define DevAssert(cOND)                                                 \
do {                                                                    \
    if (!(cOND))    {                                                   \
        fprintf(stderr, "%s:%d:%s Assertion `"#cOND"` failed.\n",       \
        __FILE__, __LINE__, __FUNCTION__);                              \
        abort();                                                        \
    }                                                                   \
} while(0)

#define DevMessage(mESSAGE)                                             \
do {                                                                    \
    fprintf(stderr, "%s:%d:%s Execution interrupted: `"#mESSAGE"`.\n",  \
    __FILE__, __LINE__, __FUNCTION__);                                  \
    abort();                                                            \
} while(0)

#endif /* ASSERTIONS_H_ */
