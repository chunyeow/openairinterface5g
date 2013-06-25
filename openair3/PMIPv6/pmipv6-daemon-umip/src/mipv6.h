/* $Id: mipv6.h 1.8 06/02/28 18:36:44+02:00 anttit@tcs.hut.fi $ */

/* Put all constants defined in MIPv6 spec in this file */

#ifndef __MIPV6_H__
#define __MIPV6_H__ 1

#include <time.h>

/* 12. Protocol Constants */
#define DHAAD_RETRIES             4   /* retransmissions */
#define INITIAL_BINDACK_TIMEOUT   1   /* second */
#define INITIAL_DHAAD_TIMEOUT     3   /* seconds */
#define INITIAL_SOLICIT_TIMER     3   /* seconds */
#define MAX_BINDACK_TIMEOUT       32  /* seconds */
#define MAX_NONCE_LIFETIME        240 /* seconds */
#define MAX_TOKEN_LIFETIME        210 /* seconds */
#define MAX_RR_BINDING_LIFETIME   420 /* seconds */
#define MAX_CONSECUTIVE_RESENDS   5   /* retransmissions */
#define PREFIX_ADV_RETRIES        3   /* retransmissions */
#define PREFIX_ADV_TIMEOUT        3   /* seconds */

/* Constants below have no explicit names in the spec. */

#define MAX_BINDING_LIFETIME     (0xffff << 2) /* seconds */  

/* Maximum time for a binding to be unused for CN to still send a BRR
 * before the binding expires */
#define CN_BRR_TIME_THRESH 60

/* 5.2.2: 64 bits (recommended) */
#define NONCE_LENGTH 8

/* 5.2.5 */
#define HMAC_SHA1_HASH_LEN  20

/* 5.2.1 */
#define HMAC_SHA1_KEY_SIZE  20

/* 6.2.7: 96 bits */
#define MIPV6_DIGEST_LEN    12

extern const struct timespec initial_bindack_timeout_ts;
extern const struct timespec initial_dhaad_timeout_ts;
extern const struct timespec initial_solicit_timer_ts;
extern const struct timespec max_bindack_timeout_ts;
extern const struct timespec max_nonce_lifetime_ts;
extern const struct timespec max_token_lifetime_ts;
extern const struct timespec max_rr_binding_lifetime_ts;
extern const struct timespec prefix_adv_timeout_ts;

#define INITIAL_BINDACK_TIMEOUT_TS initial_bindack_timeout_ts
#define INITIAL_DHAAD_TIMEOUT_TS initial_dhaad_timeout_ts
#define INITIAL_SOLICIT_TIMER_TS initial_solicit_timer_ts
#define MAX_BINDACK_TIMEOUT_TS max_bindack_timeout_ts
#define MAX_NONCE_LIFETIME_TS max_nonce_lifetime_ts
#define MAX_TOKEN_LIFETIME_TS max_token_lifetime_ts
#define MAX_RR_BINDING_LIFETIME_TS max_rr_binding_lifetime_ts
#define PREFIX_ADV_TIMEOUT_TS prefix_adv_timeout_ts

#endif
