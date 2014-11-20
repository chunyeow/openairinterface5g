/* Shared library add-on to iptables for the GTPURH target
 *
 * Copyright (c) 2010-2011 Polaris Networks
 * Author: Pradip Biswas <pradip_biswas@polarisnetworks.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/netfilter.h>

#include "xt_GTPURH.h"

#if (IPTVERSION <= 135)
#include <iptables.h>
#elif (IPTVERSION > 135)
#include <xtables.h>
#endif

#if (IPTVERSION <= 135)
#define GTPURH_PARAM_BAD_VALUE 0
#define GTPURH_PARAM_ONLY_ONCE 1
#define XT_GTPURH_VERSION IPTABLES_VERSION_STRING
#define gtpurh_strtoui(s,v,m,M) !string_to_number(s,m,M,v)
#define gtpurh_exit_error exit_error
#elif (IPTVERSION > 135) && (IPTVERSION <= 141) 
#define GTPURH_PARAM_BAD_VALUE P_BAD_VALUE
#define GTPURH_PARAM_ONLY_ONCE P_ONLY_ONCE
#define XT_GTPURH_VERSION XTABLES_VERSION
#define gtpurh_param_act param_act
#define gtpurh_strtoui(s,v,m,M) !string_to_number(s,m,M,v)
#define gtpurh_exit_error exit_error
#elif (IPTVERSION > 141)
#define GTPURH_PARAM_BAD_VALUE XTF_BAD_VALUE
#define GTPURH_PARAM_ONLY_ONCE XTF_ONLY_ONCE
#define XT_GTPURH_VERSION XTABLES_VERSION
#define gtpurh_param_act xtables_param_act
#define gtpurh_strtoui(s,v,m,M) xtables_strtoui(s,NULL,v,m,M)
#define gtpurh_exit_error xtables_error
#endif

enum {
    PARAM_ACTION = 1 << 0,
};

static void GTPURH_help(void)
{
    printf(
"GTPURH target options\n"
"  --action         value        Set action <value: remove>\n");
}

#if (IPTVERSION <= 135)
/* Stolen from iptables v1.4.7 code */
void gtpurh_param_act(unsigned int status, const char *p1, ...)
{
        const char *p2, *p3;
        va_list args;
        int b;

        va_start(args, p1);

        switch (status) {
        case GTPURH_PARAM_ONLY_ONCE:
                p2 = va_arg(args, const char *);
                b  = va_arg(args, unsigned int);
                if (!b)
                        return;
                exit_error(PARAMETER_PROBLEM,
                           "%s: \"%s\" option may only be specified once",
                           p1, p2);
                break;
        case GTPURH_PARAM_BAD_VALUE:
                p2 = va_arg(args, const char *);
                p3 = va_arg(args, const char *);
                exit_error(PARAMETER_PROBLEM,
                           "%s: Bad value for \"%s\" option: \"%s\"",
                           p1, p2, p3);
                break;
        default:
                exit_error(status, "%s", "Unknown error");
                break;
        }

        va_end(args);
}

#endif



static void parse_gtpurh_action(char *s, struct xt_gtpurh_target_info *info, unsigned int *flags)
{
    if (!strcmp(s, "remove"))
    {
        info->action = PARAM_GTPURH_ACTION_REM;
        *flags |= PARAM_GTPURH_ACTION_REM;
    }
    else
    {
        gtpurh_param_act(GTPURH_PARAM_BAD_VALUE, "GTPURH", "--action", s);
    }
}

#if (IPTVERSION <= 135)
static int
GTPURH_parse(int c, char **argv, int invert, unsigned int *flags,
           const struct ipt_entry *entry,
           struct ipt_entry_target **target)
#else
static int
GTPURH_parse(int c, char **argv, int invert, unsigned int *flags,
           const void *entry, struct xt_entry_target **target)
#endif
{
    struct xt_gtpurh_target_info *info = (struct xt_gtpurh_target_info *) (*target)->data;

    switch (c) 
    {
        case '1':
                gtpurh_param_act(GTPURH_PARAM_ONLY_ONCE, "GTPURH", "--action", *flags & PARAM_ACTION);
                parse_gtpurh_action(optarg, info, flags);
                *flags |= PARAM_ACTION;
                return 1;
    }

    return 1;
}

static void GTPURH_check(unsigned int flags)
{
    if (!(flags & PARAM_ACTION))
    {
        gtpurh_exit_error(PARAMETER_PROBLEM, "GTPURH: You must specify action");
    }

    if (flags & PARAM_GTPURH_ACTION_REM)
    {
        return;
    }
}

static void convert_action_to_string(int action, char *actionstr)
{
    switch(action)
    {
        case PARAM_GTPURH_ACTION_REM:
            sprintf (actionstr, "remove");
            break;
        default :
            sprintf (actionstr, "unspecified!!!");
            break;
    }
}

#if (IPTVERSION <= 135)
static void
GTPURH_print(const struct ipt_ip *ip,
         const struct ipt_entry_target *target,
         int numeric)

#else
static void
GTPURH_print(const void *ip,
           const struct xt_entry_target *target,
           int numeric)
#endif
{
    const struct xt_gtpurh_target_info *info =
        (struct xt_gtpurh_target_info *) target->data;

    char laddr[64], raddr[64], actionstr[32];

    convert_action_to_string(info->action, actionstr);

    if (info->action == PARAM_GTPURH_ACTION_REM)
    {
        printf("GTPURH action: %s", actionstr);
        return;
    }
}

static struct option GTPURH_opts[] = {
    { "action", 1, NULL, '1' },
    { .name = NULL }
};

#if (IPTVERSION <= 135)
static struct iptables_target gtpurh_tg_reg = {
#else
static struct xtables_target gtpurh_tg_reg = {
#endif
    .name             = "GTPURH",
    .version          = XT_GTPURH_VERSION,
#if (IPTVERSION > 135)
    .family           = NFPROTO_IPV4,
#endif
    .size             = XT_ALIGN(sizeof(struct xt_gtpurh_target_info)),
    .userspacesize    = XT_ALIGN(sizeof(struct xt_gtpurh_target_info)),
    .help             = GTPURH_help,
    .parse            = GTPURH_parse,
    .final_check      = GTPURH_check,
    .print            = GTPURH_print,
    .extra_opts       = GTPURH_opts,
};

void _init(void)
{
#if (IPTVERSION <= 135)
    register_target(&gtpurh_tg_reg);
#else
    xtables_register_target(&gtpurh_tg_reg);
#endif
}

