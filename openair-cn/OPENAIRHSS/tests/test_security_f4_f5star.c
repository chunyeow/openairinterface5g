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
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "test_utils.h"
#include "test_fd.h"

#include "auc.h"

static
void do_f4f5star(uint8_t *key, uint8_t *rand, uint8_t *op, uint8_t *f4_exp,
                  uint8_t *f5star_exp)
{
    uint8_t res_f2[8];
    uint8_t res_f5[6];
    uint8_t res_f3[16];
    uint8_t res_f4[16];
    uint8_t res_f5star[6];

    SetOPc(op);

    f2345(key, rand, res_f2, res_f3, res_f4, res_f5);
    if (compare_buffer(res_f4, 16, f4_exp, 16) != 0) {
        fail("Fail: f4");
    }
    f5star(key, rand, res_f5star);
    if (compare_buffer(res_f5star, 6, f5star_exp, 6) != 0) {
        fail("Fail: f5star");
    }
}

void
doit (void)
{
    /* Test set 1 #6.3 */
    do_f4f5star(H("465b5ce8 b199b49f aa5f0a2e e238a6bc"),
                 H("23553cbe 9637a89d 218ae64d ae47bf35"),
                 H("cdc202d5 123e20f6 2b6d676a c72cb318"),
                 H("f769bcd7 51044604 12767271 1c6d3441"),
                 H("451e8bec a43b"));
    /* Test set 2 #6.4 */
    do_f4f5star(H("0396eb31 7b6d1c36 f19c1c84 cd6ffd16"),
                 H("c00d6031 03dcee52 c4478119 494202e8"),
                 H("ff53bade 17df5d4e 793073ce 9d7579fa"),
                 H("21a8c1f9 29702adb 3e738488 b9f5c5da"),
                 H("30f11970 61c1"));
    /* Test set 3 #6.5 */
    do_f4f5star(H("fec86ba6 eb707ed0 8905757b 1bb44b8f"),
                 H("9f7c8d02 1accf4db 213ccff0 c7f71a6a"),
                 H("dbc59adc b6f9a0ef 735477b7 fadf8374"),
                 H("59a92d3b 476a0443 487055cf 88b2307b"),
                 H("deacdd84 8cc6"));
    /* Test set 4 #6.5 */
    do_f4f5star(H("9e5944ae a94b8116 5c82fbf9 f32db751"),
                 H("ce83dbc5 4ac0274a 157c17f8 0d017bd6"),
                 H("223014c5 806694c0 07ca1eee f57f004f"),
                 H("0c4524ad eac041c4 dd830d20 854fc46b"),
                 H("6085a86c 6f63"));
    /* Test set 5 #6.6 */
    do_f4f5star(H("4ab1deb0 5ca6ceb0 51fc98e7 7d026a84"),
                 H("74b0cd60 31a1c833 9b2b6ce2 b8c4a186"),
                 H("2d16c5cd 1fdf6b22 383584e3 bef2a8d8"),
                 H("1c42e960 d89b8fa9 9f2744e0 708ccb53"),
                 H("fe2555e5 4aa9"));
    /* Test set 6 #6.7 */
    do_f4f5star(H("6c38a116 ac280c45 4f59332e e35c8c4f"),
                 H("ee6466bc 96202c5a 557abbef f8babf63"),
                 H("1ba00a1a 7c6700ac 8c3ff3e9 6ad08725"),
                 H("a7466cc1 e6b2a133 7d49d3b6 6e95d7b4"),
                 H("1f53cd2b 1113"));
}
