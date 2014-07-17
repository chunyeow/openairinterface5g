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
void do_f2f3f5(uint8_t *key, uint8_t *rand, uint8_t *op, uint8_t *f2_exp,
                  uint8_t *f5_exp, uint8_t *f3_exp)
{
    uint8_t res_f2[8];
    uint8_t res_f5[6];
    uint8_t res_f3[16];
    uint8_t res_f4[16];

    SetOPc(op);

    f2345(key, rand, res_f2, res_f3, res_f4, res_f5);
    if (compare_buffer(res_f2, 8, f2_exp, 8) != 0) {
        fail("Fail: f2");
    }
    if (compare_buffer(res_f5, 6, f5_exp, 6) != 0) {
        fail("Fail: f5");
    }
    if (compare_buffer(res_f3, 16, f3_exp, 16) != 0) {
        fail("Fail: f3");
    }
}

void
doit (void)
{
    /* Test set 1 #5.3 */
    do_f2f3f5(H("465b5ce8 b199b49f aa5f0a2e e238a6bc"),
                 H("23553cbe 9637a89d 218ae64d ae47bf35"),
                 H("cdc202d5 123e20f6 2b6d676a c72cb318"),
                 H("a54211d5 e3ba50bf"), H("aa689c64 8370"),
                 H("b40ba9a3 c58b2a05 bbf0d987 b21bf8cb"));

    /* Test set 2 #5.4 */
    do_f2f3f5(H("0396eb31 7b6d1c36 f19c1c84 cd6ffd16"),
                 H("c00d6031 03dcee52 c4478119 494202e8"),
                 H("ff53bade 17df5d4e 793073ce 9d7579fa"),
                 H("d3a628ed 988620f0"), H("c4778399 5f72"),
                 H("58c433ff 7a7082ac d424220f 2b67c556"));

    /* Test set 3 #5.5 */
    do_f2f3f5(H("fec86ba6 eb707ed0 8905757b 1bb44b8f"),
                 H("9f7c8d02 1accf4db 213ccff0 c7f71a6a"),
                 H("dbc59adc b6f9a0ef 735477b7 fadf8374"),
                 H("8011c48c 0c214ed2"), H("33484dc2 136b"),
                 H("5dbdbb29 54e8f3cd e665b046 179a5098"));

    /* Test set 4 #5.6 */
    do_f2f3f5(H("9e5944ae a94b8116 5c82fbf9 f32db751"),
                 H("ce83dbc5 4ac0274a 157c17f8 0d017bd6"),
                 H("223014c5 806694c0 07ca1eee f57f004f"),
                 H("f365cd68 3cd92e96"), H("f0b9c08a d02e"),
                 H("e203edb3 971574f5 a94b0d61 b816345d"));

    /* Test set 5 #5.7 */
    do_f2f3f5(H("4ab1deb0 5ca6ceb0 51fc98e7 7d026a84"),
                 H("74b0cd60 31a1c833 9b2b6ce2 b8c4a186"),
                 H("2d16c5cd 1fdf6b22 383584e3 bef2a8d8"),
                 H("5860fc1b ce351e7e"), H("31e11a60 9118"),
                 H("7657766b 373d1c21 38f307e3 de9242f9"));

    /* Test set 6 #5.8 */
    do_f2f3f5(H("6c38a116 ac280c45 4f59332e e35c8c4f"),
                 H("ee6466bc 96202c5a 557abbef f8babf63"),
                 H("1ba00a1a 7c6700ac 8c3ff3e9 6ad08725"),
                 H("16c8233f 05a0ac28"), H("45b0f69a b06c"),
                 H("3f8c7587 fe8e4b23 3af676ae de30ba3b"));
}
