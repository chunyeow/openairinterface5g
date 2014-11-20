/* Shared library add-on to iptables for the GTPUAH target
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

#include "xt_GTPUAH.h"

#if (IPTVERSION <= 135)
#include <iptables.h>
#elif (IPTVERSION > 135)
#include <xtables.h>
#endif

#if (IPTVERSION <= 135)
#define GTPUAH_PARAM_BAD_VALUE 0
#define GTPUAH_PARAM_ONLY_ONCE 1
#define XT_GTPUAH_VERSION IPTABLES_VERSION_STRING
#define gtpuah_strtoui(s,v,m,M) !string_to_number(s,m,M,v)
#define gtpuah_exit_error exit_error
#elif (IPTVERSION > 135) && (IPTVERSION <= 141) 
#define GTPUAH_PARAM_BAD_VALUE P_BAD_VALUE
#define GTPUAH_PARAM_ONLY_ONCE P_ONLY_ONCE
#define XT_GTPUAH_VERSION XTABLES_VERSION
#define gtpuah_param_act param_act
#define gtpuah_strtoui(s,v,m,M) !string_to_number(s,m,M,v)
#define gtpuah_exit_error exit_error
#elif (IPTVERSION > 141)
#define GTPUAH_PARAM_BAD_VALUE XTF_BAD_VALUE
#define GTPUAH_PARAM_ONLY_ONCE XTF_ONLY_ONCE
#define XT_GTPUAH_VERSION XTABLES_VERSION
#define gtpuah_param_act xtables_param_act
#define gtpuah_strtoui(s,v,m,M) xtables_strtoui(s,NULL,v,m,M)
#define gtpuah_exit_error xtables_error
#endif

enum {
    PARAM_LADDR = 1 << 0,
    PARAM_LTUN = 1 << 1,
    PARAM_RADDR = 1 << 2,
    PARAM_RTUN = 1 << 3,
    PARAM_ACTION = 1 << 4,
};

static void GTPUAH_help(void)
{
    printf(
"GTPUAH target options\n"
"  --action         value        Set action <value: add>\n"
"  --own-ip         value        Set own IP address\n"
"  --own-tun        value        Set own tunnel id <value: 1-2^31>\n"
"  --peer-ip        value        Set peer IP address\n"
"  --peer-tun       value        Set peer tunnel id <value: 1-2^31>\n");
}

#if (IPTVERSION <= 135)
/* Stolen from iptables v1.4.7 code */
void gtpuah_param_act(unsigned int status, const char *p1, ...)
{
        const char *p2, *p3;
        va_list args;
        int b;

        va_start(args, p1);

        switch (status) {
        case GTPUAH_PARAM_ONLY_ONCE:
                p2 = va_arg(args, const char *);
                b  = va_arg(args, unsigned int);
                if (!b)
                        return;
                exit_error(PARAMETER_PROBLEM,
                           "%s: \"%s\" option may only be specified once",
                           p1, p2);
                break;
        case GTPUAH_PARAM_BAD_VALUE:
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

static void parse_gtpuah_addr(const char *s, struct xt_gtpuah_target_info *info, int flag)
{
    in_addr_t addr;

    if ((addr = inet_addr(s)) == -1)
    {
        switch (flag)
        {
            case PARAM_LADDR:
                gtpuah_param_act(GTPUAH_PARAM_BAD_VALUE, "GTPUAH", "--own-ip", s);
                break;
            case PARAM_RADDR:
                gtpuah_param_act(GTPUAH_PARAM_BAD_VALUE, "GTPUAH", "--peer-ip", s);
                break;
        }
    }
     
    switch (flag)
    {
        case PARAM_LADDR:
            info->laddr = addr;
            break;
        case PARAM_RADDR:
            info->raddr = addr;
            break;
    }
}

static void parse_gtpuah_tunid(char *s, struct xt_gtpuah_target_info *info, int flag)
{
    unsigned int value;

    if (!gtpuah_strtoui(s, &value, 0, UINT32_MAX))
    {
        switch (flag)
        {
            case PARAM_LTUN:
                gtpuah_param_act(GTPUAH_PARAM_BAD_VALUE, "GTPUAH", "--own-tun", s);
                break;
            case PARAM_RTUN:
                gtpuah_param_act(GTPUAH_PARAM_BAD_VALUE, "GTPUAH", "--peer-tun", s);
                break;
        }
    }

    switch (flag)
    {
        case PARAM_LTUN:
            info->ltun = value;
            break;
        case PARAM_RTUN:
            info->rtun = value;
            break;
    }
}

static void parse_gtpuah_action(char *s, struct xt_gtpuah_target_info *info, unsigned int *flags)
{
    if (!strcmp(s, "add"))
    {
        info->action = PARAM_GTPUAH_ACTION_ADD;
        *flags |= PARAM_GTPUAH_ACTION_ADD;
    }
    else
    {
        gtpuah_param_act(GTPUAH_PARAM_BAD_VALUE, "GTPUAH", "--action", s);
    }
}

#if (IPTVERSION <= 135)
static int
GTPUAH_parse(int c, char **argv, int invert, unsigned int *flags,
           const struct ipt_entry *entry,
           struct ipt_entry_target **target)
#else
static int
GTPUAH_parse(int c, char **argv, int invert, unsigned int *flags,
           const void *entry, struct xt_entry_target **target)
#endif
{
    struct xt_gtpuah_target_info *info = (struct xt_gtpuah_target_info *) (*target)->data;

    switch (c) 
    {
        case '1':
                gtpuah_param_act(GTPUAH_PARAM_ONLY_ONCE, "GTPUAH", "--own-ip", *flags & PARAM_LADDR);
                parse_gtpuah_addr(optarg, info, PARAM_LADDR);
                *flags |= PARAM_LADDR;
                return 1;
        case '2':
                gtpuah_param_act(GTPUAH_PARAM_ONLY_ONCE, "GTPUAH", "--own-tun", *flags & PARAM_LTUN);
                parse_gtpuah_tunid(optarg, info, PARAM_LTUN);
                *flags |= PARAM_LTUN;
                return 1;
        case '3':
                gtpuah_param_act(GTPUAH_PARAM_ONLY_ONCE, "GTPUAH", "--peer-ip", *flags & PARAM_RADDR);
                parse_gtpuah_addr(optarg, info, PARAM_RADDR);
                *flags |= PARAM_RADDR;
                return 1;
        case '4':
                gtpuah_param_act(GTPUAH_PARAM_ONLY_ONCE, "GTPUAH", "--peer-tun", *flags & PARAM_RTUN);
                parse_gtpuah_tunid(optarg, info, PARAM_RTUN);
                *flags |= PARAM_RTUN;
                return 1;
        case '5':
                gtpuah_param_act(GTPUAH_PARAM_ONLY_ONCE, "GTPUAH", "--action", *flags & PARAM_ACTION);
                parse_gtpuah_action(optarg, info, flags);
                *flags |= PARAM_ACTION;
                return 1;
    }

    return 1;
}

static void GTPUAH_check(unsigned int flags)
{
    if (!(flags & PARAM_ACTION))
    {
        gtpuah_exit_error(PARAMETER_PROBLEM, "GTPUAH: You must specify action");
    }
    
    if (!(flags & PARAM_LADDR))
    {
        gtpuah_exit_error(PARAMETER_PROBLEM, "GTPUAH: You must specify local addr");
    }
    if (!(flags & PARAM_LTUN))
    {
        gtpuah_exit_error(PARAMETER_PROBLEM, "GTPUAH: You must specify local tunnel id");
    }
    if (!(flags & PARAM_RADDR))
    {
        gtpuah_exit_error(PARAMETER_PROBLEM, "GTPUAH: You must specify remote addr");
    }
    if (!(flags & PARAM_RTUN))
    {
        gtpuah_exit_error(PARAMETER_PROBLEM, "GTPUAH: You must specify remote tunnel id");
    }
}

static void convert_action_to_string(int action, char *actionstr)
{
    switch(action)
    {
        case PARAM_GTPUAH_ACTION_ADD:
            sprintf (actionstr, "add");
            break;
        default :
            sprintf (actionstr, "unspecified!!!");
            break;
    }
}

#if (IPTVERSION <= 135)
static void
GTPUAH_print(const struct ipt_ip *ip,
         const struct ipt_entry_target *target,
         int numeric)

#else
static void
GTPUAH_print(const void *ip,
           const struct xt_entry_target *target,
           int numeric)
#endif
{
    const struct xt_gtpuah_target_info *info =
        (struct xt_gtpuah_target_info *) target->data;

    char laddr[64], raddr[64], actionstr[32];

    convert_action_to_string(info->action, actionstr);

    sprintf (laddr, "%s", inet_ntoa(*(struct in_addr*)&info->laddr));
    sprintf (raddr, "%s", inet_ntoa(*(struct in_addr*)&info->raddr));
    printf("GTPUAH self: %s tunnel: 0x%x / peer: %s tunnel: 0x%x / action: %s",
           laddr, info->ltun, raddr, info->rtun, actionstr);
}

static struct option GTPUAH_opts[] = {
    { "own-ip", 1, NULL, '1' },
    { "own-tun", 1, NULL, '2' },
    { "peer-ip", 1, NULL, '3' },
    { "peer-tun", 1, NULL, '4' },
    { "action", 1, NULL, '5' },
    { .name = NULL }
};

#if (IPTVERSION <= 135)
static struct iptables_target gtpuah_tg_reg = {
#else
static struct xtables_target gtpuah_tg_reg = {
#endif
    .name             = "GTPUAH",
    .version          = XT_GTPUAH_VERSION,
#if (IPTVERSION > 135)
    .family           = NFPROTO_IPV4,
#endif
    .size             = XT_ALIGN(sizeof(struct xt_gtpuah_target_info)),
    .userspacesize    = XT_ALIGN(sizeof(struct xt_gtpuah_target_info)),
    .help             = GTPUAH_help,
    .parse            = GTPUAH_parse,
    .final_check      = GTPUAH_check,
    .print            = GTPUAH_print,
    .extra_opts       = GTPUAH_opts,
};

void _init(void)
{
#if (IPTVERSION <= 135)
    register_target(&gtpuah_tg_reg);
#else
    xtables_register_target(&gtpuah_tg_reg);
#endif
}

