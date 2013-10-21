#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "NwGtpv2c.h"

#include "s11_common.h"

NwRcT s11_ie_indication_generic(NwU8T  ieType,
                                NwU8T  ieLength,
                                NwU8T  ieInstance,
                                NwU8T *ieValue,
                                void  *arg)
{
    S11_DEBUG("Received IE Parse Indication for of type %u, length %u, "
    "instance %u!\n", ieType, ieLength, ieInstance);
    return NW_OK;
}
