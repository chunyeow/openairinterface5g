#ifndef LINUX_26_COMPAT_H
#define LINUX_26_COMPAT_H

#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,1,0))
#include <linux/kconfig.h>
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33))
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#endif
#include <linux/compat_autoconf.h>
#include <linux/init.h>

/*
 * The define overwriting module_init is based on the original module_init
 * which looks like this:
 * #define module_init(initfn)					\
 *	static inline initcall_t __inittest(void)		\
 *	{ return initfn; }					\
 *	int init_module(void) __attribute__((alias(#initfn)));
 *
 * To the call to the initfn we added the symbol dependency on compat
 * to make sure that compat.ko gets loaded for any compat modules.
 */
void compat_dependency_symbol(void);

#undef module_init
#define module_init(initfn)						\
	static int __init __init_compat(void)				\
	{								\
		compat_dependency_symbol();				\
		return initfn();					\
	}								\
	int init_module(void) __attribute__((alias("__init_compat")));

/*
 * Each compat file represents compatibility code for new kernel
 * code introduced for *that* kernel revision.
 */

#include <linux/compat-2.6.22.h>
#include <linux/compat-2.6.23.h>
#include <linux/compat-2.6.24.h>
#include <linux/compat-2.6.25.h>
#include <linux/compat-2.6.26.h>
#include <linux/compat-2.6.27.h>
#include <linux/compat-2.6.28.h>
#include <linux/compat-2.6.29.h>
#include <linux/compat-2.6.30.h>
#include <linux/compat-2.6.31.h>
#include <linux/compat-2.6.32.h>
#include <linux/compat-2.6.33.h>
#include <linux/compat-2.6.34.h>
#include <linux/compat-2.6.35.h>
#include <linux/compat-2.6.36.h>
#include <linux/compat-2.6.37.h>
#include <linux/compat-2.6.38.h>
#include <linux/compat-2.6.39.h>
#include <linux/compat-3.0.h>
#include <linux/compat-3.1.h>
#include <linux/compat-3.2.h>
#include <linux/compat-3.3.h>
#include <linux/compat-3.4.h>
#include <linux/compat-3.5.h>

#endif /* LINUX_26_COMPAT_H */
