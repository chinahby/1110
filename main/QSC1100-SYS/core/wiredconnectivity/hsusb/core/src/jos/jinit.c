/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include "jos_internal.h"
#include "jinit.h"

jbool_t bootverbose;
jbool_t cold;

#ifdef JDEBUG
void check_byte_order(void)
{
    juint32_t res_val = 0, test_val = 0x01020304;

#if CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN
    htole32_t(&res_val, test_val);
#elif CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN 
    htobe32_t(&res_val, test_val);
#endif
    KASSERT(res_val == test_val, ("Invalid CONFIG_BYTE_ORDER specified\n"));
}
#endif

jresult_t jos_init(uw_args_t *args)
{
    jresult_t rc;

#ifdef JDEBUG
    check_byte_order();
#endif

    rc = jos_mem_init(args);
    if (rc)
        return rc;

    rc = os_port_init();
    if (rc)
    {
        jos_mem_uninit();
        return rc;
    }

    rc = jos_sync_init();
    if (rc)
    {
        os_port_uninit();
        jos_mem_uninit();
        return rc;
    }

    return 0;
}

void jos_uninit()
{
    jos_sync_uninit();
    
    jdma_free_all();

    os_port_uninit();

    jos_mem_uninit();
}

