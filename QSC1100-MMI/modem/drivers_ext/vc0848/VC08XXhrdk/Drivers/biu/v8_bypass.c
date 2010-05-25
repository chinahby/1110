#include "v8_bypass.h"
#include "v8_userbaserw.h"


void   Vm_SetRSLow(MMD_U16 val) 
{
    HOSTSETVAL16(MMD_BYPASS_ADDR_RSLOW, val);
}

void   Vm_SetRSHigh(MMD_U16 val)
{
    HOSTSETVAL16(MMD_BYPASS_ADDR_RSHIGH, val);
}

MMD_U16   Vm_GetRSLow(void)
{
    return HOSTGETVAL16(MMD_BYPASS_ADDR_RSLOW);
}

MMD_U16  Vm_GetRSHigh(void)
{              
    return HOSTGETVAL16(MMD_BYPASS_ADDR_RSHIGH);
}
