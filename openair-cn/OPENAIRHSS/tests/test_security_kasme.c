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
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "test_utils.h"
#include "test_fd.h"

#include "auc.h"

static
void do_derive_kasme(uint8_t *sn_id, uint8_t *sqn, uint8_t *ak,
                     uint8_t *ck, uint8_t *ik, uint8_t *kasme_exp)
{
    uint8_t kasme[32];
    derive_kasme(ck, ik, sn_id, sqn, ak, kasme);

    if (compare_buffer(kasme, 32, kasme_exp, 32) != 0) {
        fail("Fail: derive_kasme\n");
    }
}

void
doit (void)
{
    /* 20834 -> 024830 SNid */
    do_derive_kasme(H("024830"), H("FD8EEF40DF7D"), H("AA689C648370"),
                    H("B40BA9A3C58B2A05BBF0D987B21BF8CB"),
                    H("F769BCD751044604127672711C6D3441"),
                    H("238E457E0F758BADBCA8D34BB2612C10"
                      "428D426757CB5553B2B184FA64BFC549"));
    do_derive_kasme(H("02F843"), H("FD8EEF40DF7D"), H("AA689C648370"),
                    H("B40BA9A3C58B2A05BBF0D987B21BF8CB"),
                    H("F769BCD751044604127672711C6D3441"),
                    H("BD7A0903A7D0F68767EE2F5C90CB7D7D"
                      "835998D940AFDBF73173E63567C5B894"));
    do_derive_kasme(H("21F354"), H("FD8EEF40DF7D"), H("AA689C648370"),
                    H("B40BA9A3C58B2A05BBF0D987B21BF8CB"),
                    H("F769BCD751044604127672711C6D3441"),
                    H("546A79BC6D1613A72A4D631EE0351D66"
                      "036B2A0C44A3831BE6D365E24F023013"));
    do_derive_kasme(H("024830"), H("FF9BB4D0B607"), H("AA689C648370"),
                    H("B40BA9A3C58B2A05BBF0D987B21BF8CB"),
                    H("F769BCD751044604127672711C6D3441"),
                    H("564CB4D2007E4F293B67D9B29392A64A"
                      "DD4C776B133D895AF6499AA6882AAB62"));
    do_derive_kasme(H("02F843"), H("FF9BB4D0B607"), H("AA689C648370"),
                    H("B40BA9A3C58B2A05BBF0D987B21BF8CB"),
                    H("F769BCD751044604127672711C6D3441"),
                    H("34865EB0DC9A6D788A905C0514529BF5"
                      "88485DA817FFBE92E9A9B4D033B8CC6F"));
    do_derive_kasme(H("21F354"), H("FF9BB4D0B607"), H("AA689C648370"),
                    H("B40BA9A3C58B2A05BBF0D987B21BF8CB"),
                    H("F769BCD751044604127672711C6D3441"),
                    H("9EA141DA4B24CDEBC8F5FB3F61A05112"
                      "16681F121199B23EBCFACC75B358BE43"));
}
