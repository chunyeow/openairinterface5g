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

typedef struct {
    uint8_t key[16];
    uint8_t rand[16];
    uint8_t sqn[6];
    uint8_t amf[2];
    uint8_t op[16];
    uint8_t f1_exp[8];
    uint8_t f1_star_exp[8];
} test_set_t;

test_set_t test_set[] = {
    /* 35.207 #4.3 */
    {
        .key  = {
            0x46, 0x5b, 0x5c, 0xe8, 0xb1, 0x99, 0xb4, 0x9f, 0xaa, 0x5f, 0x0a,
            0x2e, 0xe2, 0x38, 0xa6, 0xbc
        },
        .rand = {
            0x23, 0x55, 0x3c, 0xbe, 0x96, 0x37, 0xa8, 0x9d, 0x21, 0x8a, 0xe6,
            0x4d, 0xae, 0x47, 0xbf, 0x35
        },
        .sqn  = { 0xff, 0x9b, 0xb4, 0xd0,  0xb6, 0x07 },
        .amf  = { 0xb9, 0xb9 },
        .op   = {
            0xcd, 0xc2, 0x02, 0xd5, 0x12, 0x3e, 0x20, 0xf6, 0x2b, 0x6d, 0x67,
            0x6a, 0xc7, 0x2c, 0xb3, 0x18
        },
        .f1_exp = { 0x4a, 0x9f, 0xfa, 0xc3, 0x54, 0xdf, 0xaf, 0xb3 },
        .f1_star_exp = { 0x01, 0xcf, 0xaf, 0x9e, 0xc4, 0xe8, 0x71, 0xe9 },
    },
    {
        .key  = {
            0x03, 0x96, 0xeb, 0x31, 0x7b, 0x6d, 0x1c, 0x36, 0xf1, 0x9c, 0x1c,
            0x84, 0xcd, 0x6f, 0xfd, 0x16
        },
        .rand = {
            0xc0, 0x0d, 0x60, 0x31, 0x03, 0xdc, 0xee, 0x52, 0xc4, 0x47, 0x81,
            0x19, 0x49, 0x42, 0x02, 0xe8,
        },
        .sqn = { 0xfd, 0x8e, 0xef, 0x40, 0xdf, 0x7d },
        .amf = { 0xaf, 0x17 },
        .op = {
            0xff, 0x53, 0xba, 0xde, 0x17, 0xdf, 0x5d, 0x4e, 0x79, 0x30, 0x73,
            0xce, 0x9d, 0x75, 0x79, 0xfa
        },
        .f1_exp = { 0x5d, 0xf5, 0xb3, 0x18, 0x07, 0xe2, 0x58, 0xb0 },
        .f1_star_exp = { 0xa8, 0xc0, 0x16, 0xe5, 0x1e, 0xf4, 0xa3, 0x43 }
    },
};

void
doit (void)
{
    int i;

    for (i = 0; i < sizeof(test_set) / sizeof(test_set_t); i++) {
        uint8_t res[8];

        SetOPc(test_set[i].op);
        f1(test_set[i].key, test_set[i].rand, test_set[i].sqn, test_set[i].amf, res);
//         printf("%02x%02x%02x%02x%02x%02x%02x%02x\n", res[0], res[1], res[2],
//                res[3], res[4], res[5], res[6], res[7]);
        if (memcmp(res, test_set[i].f1_exp, 8) != 0) {
            fail("Test set %d (f1) : failed\n", i);
        } else {
            success("Test set %d (f1) : success\n", i);
        }

        f1star(test_set[i].key, test_set[i].rand, test_set[i].sqn, test_set[i].amf, res);
//         printf("%02x%02x%02x%02x%02x%02x%02x%02x\n", res[0], res[1], res[2],
//                res[3], res[4], res[5], res[6], res[7]);
        if (memcmp(res, test_set[i].f1_star_exp, 8) != 0) {
            fail("Test set %d (f1*): failed\n", i);
        } else {
            success("Test set %d (f1*): success\n", i);
        }
    }
}
