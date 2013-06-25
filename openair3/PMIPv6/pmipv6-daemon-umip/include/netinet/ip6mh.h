#ifdef HAVE_NETINET_IP6MH_H
#include_next <netinet/ip6mh.h>
#endif

#ifndef _NETINET_IP6MH_H
#define _NETINET_IP6MH_H 1

#include <inttypes.h>
#include <netinet/in.h>

struct ip6_mh {
	uint8_t		ip6mh_proto;	/* NO_NXTHDR by default */
	uint8_t		ip6mh_hdrlen;	/* Header Len in unit of 8 Octets
				   excluding the first 8 Octets */
	uint8_t		ip6mh_type;	/* Type of Mobility Header */
	uint8_t		ip6mh_reserved;	/* Reserved */
	uint16_t	ip6mh_cksum;	/* Mobility Header Checksum */
	/* Followed by type specific messages */
} __attribute__ ((packed));

struct ip6_mh_binding_request {
	struct ip6_mh	ip6mhbr_hdr;
	uint16_t	ip6mhbr_reserved;
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

struct ip6_mh_home_test_init {
	struct ip6_mh	ip6mhhti_hdr;
	uint16_t	ip6mhhti_reserved;
	uint32_t	ip6mhhti_cookie[2];	/* 64 bit Cookie by MN */
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

struct ip6_mh_careof_test_init {
	struct ip6_mh	ip6mhcti_hdr;
	uint16_t	ip6mhcti_reserved;
	uint32_t	ip6mhcti_cookie[2];	/* 64 bit Cookie by MN */
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

struct ip6_mh_home_test {
	struct ip6_mh	ip6mhht_hdr;
	uint16_t	ip6mhht_nonce_index;
	uint32_t	ip6mhht_cookie[2];	/* Cookie from HOTI msg */
	uint32_t	ip6mhht_keygen[2];	/* 64 Bit Key by CN */
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

struct ip6_mh_careof_test {
	struct ip6_mh	ip6mhct_hdr;
	uint16_t	ip6mhct_nonce_index;
	uint32_t	ip6mhct_cookie[2];	/* Cookie from COTI message */
	uint32_t	ip6mhct_keygen[2];	/* 64bit key by CN */
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

struct ip6_mh_binding_update {
	struct ip6_mh	ip6mhbu_hdr;
	uint16_t	ip6mhbu_seqno;		/* Sequence Number */
	uint16_t	ip6mhbu_flags;
	uint16_t	ip6mhbu_lifetime;	/* Time in unit of 4 sec */
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

/* ip6mhbu_flags */
#if BYTE_ORDER == BIG_ENDIAN
#define IP6_MH_BU_ACK		0x8000	/* Request a binding ack */
#define IP6_MH_BU_HOME		0x4000	/* Home Registration */
#define IP6_MH_BU_LLOCAL	0x2000	/* Link-local compatibility */
#define IP6_MH_BU_KEYM		0x1000	/* Key management mobility */
#define IP6_MH_BU_MAP		0x0800	/* HMIPv6 MAP Registration */
#define IP6_MH_BU_MR		0x0400	/* NEMO MR Registration */
#define IP6_MH_BU_PR		0x0200  /* Proxy Registration */
#else				/* BYTE_ORDER == LITTLE_ENDIAN */
#define IP6_MH_BU_ACK		0x0080	/* Request a binding ack */
#define IP6_MH_BU_HOME		0x0040	/* Home Registration */
#define IP6_MH_BU_LLOCAL	0x0020	/* Link-local compatibility */
#define IP6_MH_BU_KEYM		0x0010	/* Key management mobility */
#define IP6_MH_BU_MAP		0x0008	/* HMIPv6 MAP Registration */
#define IP6_MH_BU_MR		0x0004	/* NEMO MR Registration */
#define IP6_MH_BU_PR		0x0002  /* Proxy Registration */
#endif

struct ip6_mh_binding_ack {
	struct ip6_mh	ip6mhba_hdr;
	uint8_t 	ip6mhba_status;	/* Status code */
	uint8_t		ip6mhba_flags;
	uint16_t	ip6mhba_seqno;
	uint16_t	ip6mhba_lifetime;
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

/* ip6mhba_flags */
#define IP6_MH_BA_KEYM		0x80	/* Key management mobility */
#define IP6_MH_BA_MR		0x40	/* NEMO MR registration */

struct ip6_mh_binding_error {
	struct ip6_mh	ip6mhbe_hdr;
	uint8_t 	ip6mhbe_status;	/* Error Status */
	uint8_t		ip6mhbe_reserved;
	struct in6_addr	ip6mhbe_homeaddr;
	/* Followed by optional Mobility Options */
} __attribute__ ((packed));

/*
 * Mobility Option TLV data structure
 */
struct ip6_mh_opt {
	uint8_t		ip6mhopt_type;	/* Option Type */
	uint8_t		ip6mhopt_len;	/* Option Length */
	/* Followed by variable length Option Data in bytes */
} __attribute__ ((packed));

/*
 * Mobility Option Data Structures 
 */
struct ip6_mh_opt_refresh_advice {
	uint8_t		ip6mora_type;
	uint8_t		ip6mora_len;
	uint16_t	ip6mora_interval;	/* Refresh interval in 4 sec */
} __attribute__ ((packed));

struct ip6_mh_opt_altcoa {
	uint8_t		ip6moa_type;
	uint8_t		ip6moa_len;
	struct in6_addr	ip6moa_addr;		/* Alternate Care-of Address */
} __attribute__ ((packed));

struct ip6_mh_opt_nonce_index {
	uint8_t		ip6moni_type;
	uint8_t		ip6moni_len;
	uint16_t	ip6moni_home_nonce;
	uint16_t	ip6moni_coa_nonce;
} __attribute__ ((packed));

struct ip6_mh_opt_auth_data {
	uint8_t		ip6moad_type;
	uint8_t 	ip6moad_len;
	uint8_t 	ip6moad_data[12];	/* 96 bit Authenticator */
} __attribute__ ((packed));

struct ip6_mh_opt_mob_net_prefix {
	uint8_t 	ip6mnp_type;
	uint8_t 	ip6mnp_len;
	uint8_t 	ip6mnp_reserved;
	uint8_t 	ip6mnp_prefix_len;
	struct in6_addr ip6mnp_prefix;
} __attribute__ ((packed));

/*
 *     Mobility Header Message Types
 */
#define IP6_MH_TYPE_BRR		0	/* Binding Refresh Request */
#define IP6_MH_TYPE_HOTI	1	/* HOTI Message */
#define IP6_MH_TYPE_COTI	2	/* COTI Message */
#define IP6_MH_TYPE_HOT		3	/* HOT Message */
#define IP6_MH_TYPE_COT		4	/* COT Message */
#define IP6_MH_TYPE_BU		5	/* Binding Update */
#define IP6_MH_TYPE_BACK	6	/* Binding ACK */
#define IP6_MH_TYPE_BERROR	7	/* Binding Error */

/*
 *     Mobility Header Message Option Types
 */
#define IP6_MHOPT_PAD1		0x00	/* PAD1 */
#define IP6_MHOPT_PADN		0x01	/* PADN */
#define IP6_MHOPT_BREFRESH	0x02	/* Binding Refresh */
#define IP6_MHOPT_ALTCOA	0x03	/* Alternate COA */
#define IP6_MHOPT_NONCEID	0x04	/* Nonce Index */
#define IP6_MHOPT_BAUTH		0x05	/* Binding Auth Data */
#define IP6_MHOPT_MOB_NET_PRFX	0x06	/* Mobile Network Prefix */

/*
 *    Status values accompanied with Mobility Binding Acknowledgement
 */
#define IP6_MH_BAS_ACCEPTED		0	/* BU accepted */
#define IP6_MH_BAS_PRFX_DISCOV		1	/* Accepted, but prefix discovery
						   required */
#define IP6_MH_BAS_UNSPECIFIED		128	/* Reason unspecified */
#define IP6_MH_BAS_PROHIBIT		129	/* Administratively prohibited */
#define IP6_MH_BAS_INSUFFICIENT		130	/* Insufficient resources */
#define IP6_MH_BAS_HA_NOT_SUPPORTED	131	/* HA registration not supported */
#define IP6_MH_BAS_NOT_HOME_SUBNET	132	/* Not Home subnet */
#define IP6_MH_BAS_NOT_HA		133	/* Not HA for this mobile node */
#define IP6_MH_BAS_DAD_FAILED		134	/* DAD failed */
#define IP6_MH_BAS_SEQNO_BAD		135	/* Sequence number out of range */
#define IP6_MH_BAS_HOME_NI_EXPIRED	136	/* Expired Home nonce index */
#define IP6_MH_BAS_COA_NI_EXPIRED	137	/* Expired Care-of nonce index */
#define IP6_MH_BAS_NI_EXPIRED		138	/* Expired Nonce Indices */
#define IP6_MH_BAS_REG_NOT_ALLOWED	139	/* Registration type change 
						   disallowed */
#define IP6_MH_BAS_MR_OP_NOT_PERMITTED	140	/* MR Operation not permitted */
#define IP6_MH_BAS_INVAL_PRFX		141	/* Invalid Prefix */
#define IP6_MH_BAS_NOT_AUTH_FOR_PRFX	142	/* Not Authorized for Prefix */
#define IP6_MH_BAS_FWDING_FAILED	143	/* Forwarding Setup failed */
/*
 *    Status values for the Binding Error mobility messages
 */
#define IP6_MH_BES_UNKNOWN_HAO	1	/* Unknown binding for HOA */
#define IP6_MH_BES_UNKNOWN_MH	2	/* Unknown MH Type */

#endif	/* netinet/ip6mh.h */
