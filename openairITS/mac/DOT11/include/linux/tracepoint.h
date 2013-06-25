#ifndef _COMPAT_LINUX_TRACEPOINT_H
#define _COMPAT_LINUX_TRACEPOINT_H 1

#include <linux/version.h>

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32))
/*
 * Disable all tracing for older kernels
 * < 2.6.27		had no tracing
 * 2.6.27		had broken tracing
 * 2.6.28-2.6.32	didn't have anything like DECLARE_EVENT_CLASS
 *			and faking it would be extremely difficult
 */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
/*
 * For 2.6.28+ include the original tracepoint.h but override
 * the defines new code uses to disable tracing completely.
 */
#include_next <linux/tracepoint.h>
#endif

#undef TRACE_EVENT
#define TRACE_EVENT(name, proto, ...) \
static inline void trace_ ## name(proto) {}
#undef DECLARE_EVENT_CLASS
#define DECLARE_EVENT_CLASS(...)
#undef DEFINE_EVENT
#define DEFINE_EVENT(evt_class, name, proto, ...) \
static inline void trace_ ## name(proto) {}

#define TP_PROTO(args...)	args
#define TP_ARGS(args...)	args
#define TP_CONDITION(args...)	args

#else
/* since 2.6.33, tracing hasn't changed, so just include the kernel's file */
#include_next <linux/tracepoint.h>

#endif /* (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32)) */

#endif	/* _COMPAT_LINUX_TRACEPOINT_H */
