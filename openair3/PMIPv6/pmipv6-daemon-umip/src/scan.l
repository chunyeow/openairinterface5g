/*
 * $Id: scan.l 1.44 06/05/12 11:48:36+03:00 vnuorval@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors: Antti Tuominen <anttit@tcs.hut.fi>
 *          Ville Nuorvala <vnuorval@tcs.hut.fi>
 *
 * Copyright 2003-2005 Go-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */

%top{
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
}

%{
#include <arpa/inet.h>
#include <glob.h>
#include <string.h>
#include <stdio.h>
#include "gram.h"

#define YY_NO_INPUT 1

int lineno = 1;

static int yywrap(void) { return 1; }
void yyerror(char *s);


int incl_level = 0;       /* Current inclusion level */
#define MAX_INCLUDE_DEPTH  5  /* Max inclusion level */

/* Stack of parsing states for include directive */
struct incl_states_t {
	YY_BUFFER_STATE parent_state; /* Remaining buffer from parent file */
	char *parent_incl_file;       /* Name of parent file */
	int parent_lineno;            /* lineno in parent file before include*/
	glob_t files;                 /* List of files to be included */
	int cur_file;                 /* Current position in previous list */
} incl_states[MAX_INCLUDE_DEPTH];

/* incl_file points to current included file to allow meaningful debug
 * statements to be generated from gram.y. It is kept NULL when in
 * main config file */
char *incl_file = NULL;

int glob_err_cb(const char *epath, int eerrno)
{
	fprintf(stderr, "Failed to scan %s: %s\n", epath, strerror(eerrno));
	return 1;
}


%}

%option nounput
%option noyyget_lineno noyyset_lineno
%option noyyget_in noyyset_in
%option noyyget_out noyyset_out
%option noyyget_debug noyyset_debug
%option noyyget_leng
%option noyyget_text

%x in_include

ws		[ \t]+
comment		#.*
nl		\n
qstring		\"[^\"\n]*[\"\n]
digit		[0-9]
number		({digit})+
decimal		({number}"."{number})
hexdec		([a-f]|[A-F]|[0-9])
addr1		{hexdec}{1,4}":"({hexdec}{1,4}":")*(":"{hexdec}{1,4})+
addr2		{hexdec}{1,4}(":"{hexdec}{1,4}){0,6}"::"
addr3		({hexdec}{1,4}":"){7}{hexdec}{1,4}
addr		({addr1}|{addr2}|{addr3}|"::")
macaddr1	({hexdec}{1,2}":"){5}{hexdec}{1,2}
macaddr2	({hexdec}{1,2}":"{hexdec}{1,2}":"{hexdec}{1,2}":"{hexdec}{1,2}":"{hexdec}{1,2}":"{hexdec}{1,2})
macaddr		({macaddr1}|{macaddr2})
true		(enabled|allow|true)
false		(disabled|deny|block|false)
%%

include		BEGIN(in_include);
<in_include>{
{ws}		; /* skip whitespaces */
{qstring}	{ /* Name of the file to include; directly passed to glob. */
			int globerror=0;
	                struct incl_states_t *state;
			char *buf = strdup(yytext+1);

			if (buf == NULL) {
				fprintf(stderr, "Not enough memory to parse"
					" include directive.\n");
				return INV_TOKEN;
			}
			if (buf[yyleng-2] != '"') {
				fprintf(stderr, "Missing \" after file name to"
					" include: %s\n", yytext);
				return INV_TOKEN;
			}
			buf[yyleng-2] = '\0';

			if (incl_level >= MAX_INCLUDE_DEPTH) {
				fprintf(stderr, "Reached %d inclusion levels"
					" in configuration files. Giving up\n",
					MAX_INCLUDE_DEPTH);
				return INV_TOKEN;
			}
	                state = &incl_states[incl_level];

			/* glob provided include pattern */
			globerror = glob(buf, GLOB_ERR, glob_err_cb,
					 &state->files);
			free(buf);

			if (globerror == GLOB_NOSPACE) {
				fprintf(stderr, "Not enough memory to parse"
					" include directive.\n");
				return INV_TOKEN;
			}
			if (globerror == GLOB_ABORTED) {
				fprintf(stderr, "An error was encountered in"
					" include directive.\n");
				return INV_TOKEN;
			}
			if (globerror == GLOB_NOMATCH) {
				fprintf(stderr, "WARNING: No match found for"
					" provided include pattern %s\n",
					yytext);
				globfree(&state->files);
				goto nomatch;
			}
			if (globerror) {
				fprintf(stderr, "Unexpected error in glob"
					" (%d).\n", globerror);
				return INV_TOKEN;
			}

			/* We now have a list of files to include. */

			/* Parsing of current buffer will continue after
			 * parsing of new buffer. Stack old one for now. */
			state->parent_state = YY_CURRENT_BUFFER;
			state->parent_lineno = lineno;
			state->parent_incl_file = incl_file;
			lineno = 1;

			/* Start with first match */
			state->cur_file = 0;
			yyin = fopen(state->files.gl_pathv[0], "r");
			if (!yyin) {
				fprintf(stderr, "Unable to open included file"
					" %s.", state->files.gl_pathv[0]);
				return INV_TOKEN;
			}

			yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
			incl_file = state->files.gl_pathv[0];
			incl_level++;

nomatch:
			BEGIN(INITIAL);
		}
}
<<EOF>>		{
	                struct incl_states_t *state;

			if (incl_level == 0) { /* End of parsing */
				incl_file = NULL;
				yyterminate();
			}

			yy_delete_buffer(YY_CURRENT_BUFFER);

			/* Go to next file, if any */
			state = &incl_states[incl_level-1];
			state->cur_file++;
			if (state->files.gl_pathv[state->cur_file] == NULL) {
				/* We are over with this list of includes,
				 * let's continue parsing of parent file */
				globfree(&state->files);
				lineno = state->parent_lineno;
				incl_file = state->parent_incl_file;
				yy_switch_to_buffer(state->parent_state);
				--incl_level;
			} else {
				/* Proceed to next included file */
				incl_file = state->files.gl_pathv[state->cur_file];
				yyin = fopen(incl_file, "r");
				if (!yyin) {
					fprintf(stderr, "Unable to open included"
						" file %s.", incl_file);
					return INV_TOKEN;
				}
				yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
				lineno = 1;
			}
		}

{ws}		;
{comment}	;
{nl}		{ lineno++; }
{qstring}	{
			yylval.string = strdup(yytext+1);
			if (yylval.string[yyleng-2] != '"')
				fprintf(stderr, "Unterminated string\n");
			else
				yylval.string[yyleng-2] = '\0';
			return QSTRING;
		}

NodeConfig			{ return MIP6ENTITY; }
HA				{ return MIP6HA; }
CN				{ return MIP6CN; }
MN				{ return MIP6MN; }
LMA						{ return MIP6LMA; }
MAG						{ return MIP6MAG; }
DebugLevel			{ return DEBUGLEVEL; }
DebugLogFile			{ return DEBUGLOGFILE; }
DoRouteOptimizationCN		{ return DOROUTEOPTIMIZATIONCN; }
DoRouteOptimizationMN		{ return DOROUTEOPTIMIZATIONMN; }
HomeAddress			{ return HOMEADDRESS; }
HomeAgentAddress		{ return HOMEAGENTADDRESS; }
InitialBindackTimeoutFirstReg	{ return INITIALBINDACKTIMEOUTFIRSTREG; }
InitialBindackTimeoutReReg	{ return INITIALBINDACKTIMEOUTREREG; }
LinkName			{ return LINKNAME; }
HaMaxBindingLife		{ return HAMAXBINDINGLIFE; }
MnMaxHaBindingLife		{ return MNMAXHABINDINGLIFE; }
MnMaxCnBindingLife		{ return MNMAXCNBINDINGLIFE; }
MaxMobPfxAdvInterval		{ return MAXMOBPFXADVINTERVAL; }
MinMobPfxAdvInterval		{ return MINMOBPFXADVINTERVAL; }
MnHomeLink			{ return MNHOMELINK; }
NonVolatileBindingCache		{ return NONVOLATILEBINDINGCACHE; }
SendMobPfxSols			{ return SENDMOBPFXSOLS; }
SendUnsolMobPfxAdvs	       	{ return SENDUNSOLMOBPFXADVS; }
SendMobPfxAdvs			{ return SENDMOBPFXADVS; }
UseMovementModule		{ return USEMOVEMENTMODULE; }
UsePolicyModule			{ return USEPOLICYMODULE; }
IPsecPolicySet			{ return IPSECPOLICYSET; }
IPsecPolicy			{ return IPSECPOLICY; }
IPsecType			{ return IPSECTYPE; }
UseAltCoa			{ return USEALTCOA; }
UseESP				{ return USEESP; }
UseAH				{ return USEAH; }
UseIPComp			{ return USEIPCOMP; }
UseMnHaIPsec			{ return USEMNHAIPSEC; }
KeyMngMobCapability		{ return KEYMNGMOBCAPABILITY; }
HomeRegBinding			{ return HOMEREGBINDING; }
Mh				{ return MH; }
MobPfxDisc			{ return MOBPFXDISC; }
TunnelHomeTesting		{ return TUNNELHOMETESTING; }
TunnelMh			{ return TUNNELMH; }
TunnelPayload			{ return TUNNELPAYLOAD; }
MnRoPolicy       		{ return MNROPOLICY; }
ICMP				{ return ICMP; }
any				{ return ANY; }
DoRouteOptimization		{ return DOROUTEOPT; }
DefaultBindingAclPolicy		{ return DEFAULTBINDINGACLPOLICY; }
BindingAclPolicy		{ return BINDINGACLPOLICY; }
UseCnBuAck			{ return USECNBUACK; }
Interface			{ return INTERFACE; }
IfName				{ return IFNAME; }
IfType				{ return IFTYPE; }
MnIfPreference			{ return MNIFPREFERENCE; }
Tunnel                          { return ISTUNNEL; }
MnUseAllInterfaces     		{ return MNUSEALLINTERFACES; }
MnRouterProbes			{ return MNROUTERPROBES; }
MnRouterProbeTimeout		{ return MNROUTERPROBETIMEOUT; }
MnDiscardHaParamProb		{ return MNDISCARDHAPARAMPROB; }
OptimisticHandoff		{ return OPTIMISTICHANDOFF; }
HaAcceptMobRtr   		{ return HAACCEPTMOBRTR; }
IsMobRtr       			{ return ISMOBRTR; }
HaServedPrefix       	       	{ return HASERVEDPREFIX; }
HomePrefix     			{ return HOMEPREFIX; }
MobRtrUseExplicitMode    	{ return MOBRTRUSEEXPLICITMODE; }
RFC5213TimestampBasedApproachInUse		{ return RFC5213TIMESTAMPBASEDAPPROACHINUSE; }
RFC5213MobileNodeGeneratedTimestampInUse	{ return RFC5213MOBILENODEGENERATEDTIMESTAMPINUSE; }
RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks	{ return RFC5213FIXEDMAGLINKLOCALADDRESSONALLACCESSLINKS; }
RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks	{ return RFC5213FIXEDMAGLINKLAYERADDRESSONALLACCESSLINKS; }
RFC5213MinDelayBeforeBCEDelete			{ return RFC5213MINDELAYBEFOREBCEDELETE; }
RFC5213MaxDelayBeforeNewBCEAssign		{ return RFC5213MAXDELAYBEFORENEWBCEASSIGN; }
RFC5213TimestampValidityWindow			{ return RFC5213TIMESTAMPVALIDITYWINDOW; }
RFC5213EnableMAGLocalRouting			{ return RFC5213ENABLEMAGLOCALROUTING; }
LmaPmipNetworkAddress					{ return LMAPMIPNETWORKADDRESS; }
LmaPmipNetworkDevice                            { return LMAPMIPNETWORKDEVICE; }
LmaCoreNetworkAddress                           { return LMACORENETWORKADDRESS; }
LmaCoreNetworkDevice                            { return LMACORENETWORKDEVICE; }
MagAddressIngress				{ return MAGADDRESSINGRESS; }
MagAddressEgress				{ return MAGADDRESSEGRESS; }
MagDeviceIngress				{ return MAGDEVICEINGRESS; }
MagDeviceEgress					{ return MAGDEVICEEGRESS; }
OurAddress					{ return OURADDRESS; }
HomeNetworkPrefix				{ return HOMENETWORKPREFIX; }
PBULifeTime					{ return PBULIFETIME; }
PBALifeTime					{ return PBALIFETIME; }
RetransmissionTimeOut				{ return RETRANSMISSIONTIMEOUT; }
MaxMessageRetransmissions			{ return MAXMESSAGERETRANSMISSIONS; }
TunnelingEnabled				{ return TUNNELINGENABLED; }
DynamicTunnelingEnabled				{ return DYNAMICTUNNELINGENABLED; }
MaxDelayBeforeDynamicTunnelingDelete	{ return MAXDELAYBEFOREDYNAMICTUNNELINGDELETE; }
ProxyMipLma					{ return PROXYMIPLMA; }
ProxyMipMag					{ return PROXYMIPMAG; }
RadiusPassword					{ return RADIUSPASSWORD; }
RadiusClientConfigFile				{ return RADIUSCLIENTCONFIGFILE; }
PcapSyslogAssociationGrepString			{ return PCAPSYSLOGASSOCIATIONGREPSTRING; }
PcapSyslogDeAssociationGrepString		{ return PCAPSYSLOGDEASSOCIATIONGREPSTRING; }
internal			{ return INTERNAL; }

{addr}		{
			int i;
				
			i = inet_pton(AF_INET6, yytext, &yylval.addr);

			if (i < 1) {
				return INV_TOKEN;
			}
			return ADDR;
		}

{macaddr}	{
			unsigned int i;
                        char hex_digits [3];
                        hex_digits[2] = 0;
                        yylval.macaddr.s6_addr[0]  = 0;
                        yylval.macaddr.s6_addr[1]  = 0;
                        yylval.macaddr.s6_addr[2]  = 0;
                        yylval.macaddr.s6_addr[3]  = 0;
                        yylval.macaddr.s6_addr[4]  = 0;
                        yylval.macaddr.s6_addr[5]  = 0;
                        yylval.macaddr.s6_addr[6]  = 0;
                        yylval.macaddr.s6_addr[7]  = 0;
                        yylval.macaddr.s6_addr[8]  = 0;
                        yylval.macaddr.s6_addr[9]  = 0;
                        hex_digits[0] = yytext[0];
                        hex_digits[1] = yytext[1];
                        sscanf(hex_digits, "%02x", &i); 
                        yylval.macaddr.s6_addr[10] = i & 0x000000FF; 
                        hex_digits[0] = yytext[3];
                        hex_digits[1] = yytext[4];
                        sscanf(hex_digits, "%02x", &i); 
                        yylval.macaddr.s6_addr[11] = i & 0x000000FF; 
                        hex_digits[0] = yytext[6];
                        hex_digits[1] = yytext[7];
                        sscanf(hex_digits, "%02x", &i); 
                        yylval.macaddr.s6_addr[12] = i & 0x000000FF; 
                        hex_digits[0] = yytext[9];
                        hex_digits[1] = yytext[10];
                        sscanf(hex_digits, "%02x", &i); 
                        yylval.macaddr.s6_addr[13] = i & 0x000000FF; 
                        hex_digits[0] = yytext[12];
                        hex_digits[1] = yytext[13];
                        sscanf(hex_digits, "%02x", &i); 
                        yylval.macaddr.s6_addr[14] = i & 0x000000FF; 
                        hex_digits[0] = yytext[15];
                        hex_digits[1] = yytext[16];
                        sscanf(hex_digits, "%02x", &i); 
                        yylval.macaddr.s6_addr[15] = i & 0x000000FF; 
                        printf("scan.l: %02x:%02x:%02x:%02x:%02x:%02x\n", yylval.macaddr.s6_addr[10], yylval.macaddr.s6_addr[11],yylval.macaddr.s6_addr[12],yylval.macaddr.s6_addr[13],yylval.macaddr.s6_addr[14],yylval.macaddr.s6_addr[15]);
			return MACADDR;
		}

{number}	{ yylval.num = atoi(yytext); return NUMBER; }

{decimal}	{ yylval.dec = atof(yytext); return DECIMAL; }

{true}  	{ yylval.bool = 1; return BOOL; }

{false} 	{ yylval.bool = 0; return BOOL; }

"{"|"}"|";"|"/"|"="|"("|")"|","	{ return *yytext; }

.		{ return INV_TOKEN; }
%%
