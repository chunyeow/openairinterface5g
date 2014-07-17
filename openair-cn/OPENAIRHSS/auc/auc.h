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
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>

#ifndef AUC_H_
#define AUC_H_

#define SQN_LENGTH_BITS   (48)
#define SQN_LENGTH_OCTEST (SQN_LENGTH_BITS/8)
#define IK_LENGTH_BITS   (128)
#define IK_LENGTH_OCTETS (IK_LENGTH_BITS/8)
#define RAND_LENGTH_OCTETS  (16)
#define RAND_LENGTH_BITS    (RAND_LENGTH_OCTETS*8)
#define XRES_LENGTH_OCTETS  (8)
#define AUTN_LENGTH_OCTETS  (16)
#define KASME_LENGTH_OCTETS (32)
#define MAC_S_LENGTH        (8)

extern uint8_t opc[16];

typedef mpz_t random_t;
typedef mpz_t sqn_t;

typedef uint8_t u8;

typedef struct {
    uint8_t rand[16];
    uint8_t rand_new;
    uint8_t xres[8];
    uint8_t autn[16];
    uint8_t kasme[32];
} auc_vector_t;

void RijndaelKeySchedule(u8 key[16]);
void RijndaelEncrypt(u8 in[16], u8 out[16]);

/* Sequence number functions */
struct sqn_ue_s;
struct sqn_ue_s *sqn_exists(uint64_t imsi);
void sqn_insert(struct sqn_ue_s *item);
void sqn_init(struct sqn_ue_s *item);
struct sqn_ue_s *sqn_new(uint64_t imsi);
void sqn_list_init(void);
void sqn_get(uint64_t imsi, uint8_t sqn[6]);

/* Random number functions */
struct random_state_s;
void random_init(void);
void generate_random(uint8_t *random, ssize_t length);

void SetOPc(u8 op_c[16]);

void f1 ( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
          u8 mac_a[8] );
void f1star( u8 k[16], u8 rand[16], u8 sqn[6], u8 amf[2],
             u8 mac_s[8] );
void f2345 ( u8 k[16], u8 rand[16],
             u8 res[8], u8 ck[16], u8 ik[16], u8 ak[6] );
void f5star( u8 k[16], u8 rand[16],
             u8 ak[6] );

void generate_autn(u8 sqn[6], u8 ak[6], u8 amf[2], u8 mac_a[8], u8 autn[16]);
int generate_vector(uint64_t imsi, uint8_t key[16], uint8_t plmn[3],
                    uint8_t sqn[6], auc_vector_t *vector);

inline
void kdf(uint8_t *key, uint16_t key_len, uint8_t *s, uint16_t s_len, uint8_t *out,
         uint16_t out_len);

inline
void derive_kasme(uint8_t ck[16], uint8_t ik[16], uint8_t plmn[3], uint8_t sqn[6],
                  uint8_t ak[6], uint8_t kasme[32]);

uint8_t *sqn_ms_derive(uint8_t *key, uint8_t *auts, uint8_t *rand);

static inline void print_buffer(const char *prefix, uint8_t *buffer, int length)
{
  int i;

  fprintf(stdout, "%s", prefix);
  for (i = 0; i < length; i++) {
      fprintf(stdout, "%02x.", buffer[i]);
  }
  fprintf(stdout, "\n");
}

#endif /* AUC_H_ */
