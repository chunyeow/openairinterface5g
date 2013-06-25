#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
#include <pcmcia/cs_types.h>
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
#include <pcmcia/cs.h>
#endif

#include_next <pcmcia/cistpl.h>
