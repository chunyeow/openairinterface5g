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
void do_f1_f1star(uint8_t *key, uint8_t *rand, uint8_t *sqn,
                  uint8_t *amf, uint8_t *op, uint8_t *f1_exp,
                  uint8_t *f1star_exp)
{
    uint8_t res[8];

    SetOPc(op);

    f1(key, rand, sqn, amf, res);
    if (compare_buffer(res, 8, f1_exp, 8) != 0) {
        fail("Fail: f1");
    }
    f1star(key, rand, sqn, amf, res);
    if (compare_buffer(res, 8, f1star_exp, 8) != 0) {
        fail("Fail: f1*");
    }
}

void
doit (void)
{
    /* Test suite taken from 3GPP TS 35.207 */

    /* Test set 1 #4.3 */
    do_f1_f1star(
        H("465b5ce8 b199b49f aa5f0a2e e238a6bc"),
        H("23553cbe 9637a89d 218ae64d ae47bf35"),
        H("ff9bb4d0 b607"),
        H("b9b9"),
        H("cdc202d5 123e20f6 2b6d676a c72cb318"),
        H("4a9ffac3 54dfafb3"),
        H("01cfaf9e c4e871e9"));
    /* Test set 2 #4.4 */
    do_f1_f1star(
        H("0396eb31 7b6d1c36 f19c1c84 cd6ffd16"),
        H("c00d6031 03dcee52 c4478119 494202e8"),
        H("fd8eef40 df7d"),
        H("af17"),
        H("ff53bade 17df5d4e 793073ce 9d7579fa"),
        H("5df5b318 07e258b0"),
        H("a8c016e5 1ef4a343"));
    /* Test set 3 #4.5 */
    do_f1_f1star(
        H("fec86ba6 eb707ed0 8905757b 1bb44b8f"),
        H("9f7c8d02 1accf4db 213ccff0 c7f71a6a"),
        H("9d027759 5ffc"),
        H("725c"),
        H("dbc59adc b6f9a0ef 735477b7 fadf8374"),
        H("9cabc3e9 9baf7281"),
        H("95814ba2 b3044324"));
    /* Test set 4 #4.5 */
    do_f1_f1star(
        H("9e5944ae a94b8116 5c82fbf9 f32db751"),
        H("ce83dbc5 4ac0274a 157c17f8 0d017bd6"),
        H("0b604a81 eca8"),
        H("9e09"),
        H("223014c5 806694c0 07ca1eee f57f004f"),
        H("74a58220 cba84c49"),
        H("ac2cc74a 96871837"));
    /* Test set 5 #4.6 */
    do_f1_f1star(
        H("4ab1deb0 5ca6ceb0 51fc98e7 7d026a84"),
        H("74b0cd60 31a1c833 9b2b6ce2 b8c4a186"),
        H("e880a1b5 80b6"),
        H("9f07"),
        H("2d16c5cd 1fdf6b22 383584e3 bef2a8d8"),
        H("49e785dd 12626ef2"),
        H("9e857903 36bb3fa2"));
    /* Test set 6 #4.7 */
    do_f1_f1star(
        H("6c38a116 ac280c45 4f59332e e35c8c4f"),
        H("ee6466bc 96202c5a 557abbef f8babf63"),
        H("414b9822 2181"),
        H("4464"),
        H("1ba00a1a 7c6700ac 8c3ff3e9 6ad08725"),
        H("078adfb4 88241a57"),
        H("80246b8d 0186bcf1"));
}
