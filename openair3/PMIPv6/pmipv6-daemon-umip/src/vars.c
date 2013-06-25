/* $Id: vars.c 1.2 06/02/28 18:36:44+02:00 anttit@tcs.hut.fi $ */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <netinet/in.h>
#include <time.h>
#include "mipv6.h"

const struct timespec initial_bindack_timeout_ts = 
{ INITIAL_BINDACK_TIMEOUT, 0 };
const struct timespec initial_dhaad_timeout_ts =
{ INITIAL_DHAAD_TIMEOUT, 0 };
const struct timespec initial_solicit_timer_ts =
{ INITIAL_SOLICIT_TIMER, 0 };
const struct timespec max_bindack_timeout_ts =
{ MAX_BINDACK_TIMEOUT, 0 };
const struct timespec max_nonce_lifetime_ts =
{ MAX_NONCE_LIFETIME, 0 };
const struct timespec max_token_lifetime_ts =
{ MAX_TOKEN_LIFETIME, 0 };
const struct timespec max_rr_binding_lifetime_ts =
{ MAX_RR_BINDING_LIFETIME, 0 };
const struct timespec prefix_adv_timeout_ts =
{ PREFIX_ADV_TIMEOUT, 0 };

const char loopback_dev_name[] = "lo";

#define IN6ADDR_ALL_NODES_MC_INIT \
	{ { { 0xff,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0x1 } } }
#define IN6ADDR_ALL_ROUTERS_MC_INIT \
	{ { { 0xff,0x02,0,0,0,0,0,0,0,0,0,0,0,0,0,0x2 } } }

const struct in6_addr in6addr_all_nodes_mc = IN6ADDR_ALL_NODES_MC_INIT;
const struct in6_addr in6addr_all_routers_mc = IN6ADDR_ALL_ROUTERS_MC_INIT;
