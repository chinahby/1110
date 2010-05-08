/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_init_flags.c#1 $ 

$Log: $
*
*    #3-4      29 Aug 2006            GS
* Fixing klocwork error.

===========================================================================*/
/**
 * @file
    
    This file contains the global initialization table and access routines.
    
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_SUPPORT

#include "oi_core_common.h"
#include "oi_modules.h"
#include "oi_init_flags.h"

/* The initialization table is an array of initialization flags.  It is global in 
    release mode, so that applications can efficiently access the flags (via macros). */

#ifdef OI_DEBUG    
#error code not present
#else

OI_INIT_FLAG OI_GV_InitializationFlags[OI_NUM_MODULES] ;

#endif
      
/*************************************************************

    Reset all initialization flags
    
    Know what you're doing if you call this function.  
    
    It should only be called as part of a global, reset/restart process.
    
*************************************************************/


void OI_InitFlags_ResetAllFlags(void)
{
    OI_DBGPRINT(("OI_InitFlags_ResetAllFlags - resetting all initialization flags")) ;
    OI_MemSet(OI_GV_InitializationFlags, 0, sizeof(OI_GV_InitializationFlags)) ;
}
/*************************************************************

    Initialization flag access
    
*************************************************************/

OI_INIT_FLAG OI_InitFlags_GetFlag(OI_MODULE module)
{
    OI_ASSERT(module < OI_ARRAYSIZE(OI_GV_InitializationFlags)) ;
    return (OI_GV_InitializationFlags[module]) ;
}

void OI_InitFlags_PutFlag(OI_INIT_FLAG flagValue, OI_MODULE module, OI_MODULE callingModule)
{
    OI_ASSERT(module == callingModule);
    OI_ASSERT(module < OI_ARRAYSIZE(OI_GV_InitializationFlags)) ;
    if (OI_GV_InitializationFlags[module] == OI_INIT_FLAG_UNITIALIZED_VALUE) {
        if (flagValue != OI_INIT_FLAG_UNITIALIZED_VALUE) {
            // was uninitialized, changing to initialized
            if (module != OI_MODULE_SUPPORT) { /* avoid an infinite recursion when initializing the debugprint code */
                OI_DBGPRINT(("%s init flag changed from uninitialized to %d\n",
                             OI_ModuleToString(module), flagValue)) ; 
            }
        }
    }
    else if (flagValue == OI_INIT_FLAG_UNITIALIZED_VALUE) {
        // was initialized, changing to uninitialized
        if (module != OI_MODULE_SUPPORT) { /* avoid an infinite recursion when initializing the debugprint code */
            OI_DBGPRINT(("%s init flag changed from %d to uninitialized\n",
                         OI_ModuleToString(module), OI_GV_InitializationFlags[module])) ; 
        }
    }
    OI_GV_InitializationFlags[module] = flagValue ;
}    

void OI_InitFlags_Increment(OI_MODULE module)
{
    OI_ASSERT(module < OI_ARRAYSIZE(OI_GV_InitializationFlags)) ;
    ++OI_GV_InitializationFlags[module];
}

void OI_InitFlags_Decrement(OI_MODULE module)
{
    OI_ASSERT(module < OI_ARRAYSIZE(OI_GV_InitializationFlags)) ;
    OI_ASSERT(OI_GV_InitializationFlags[module]);
    --OI_GV_InitializationFlags[module];
}

OI_BOOL OI_InitFlags_AllUninitialized(void)
{
    OI_INT  i ;
    
    for (i = 0; i < OI_ARRAYSIZE(OI_GV_InitializationFlags); ++i) {
        if (OI_INIT_FLAG_UNITIALIZED_VALUE != OI_GV_InitializationFlags[i]) {
            return(FALSE) ;
        }
    }
    return(TRUE) ;
}
/*************************************************************/

#if 0
// Test code

OI_INIT_FLAG flag1 ;
OI_INIT_FLAG flag2 ;
OI_BYTE*    flag3 ;

static void test(void)
{
    flag1 = OI_INIT_FLAG_VALUE(BHSRV) ;
    
    OI_INIT_FLAG_PUT_FLAG(flag2, BHSRV) ;
    
    flag3 = (OI_BYTE*)OI_INIT_FLAG_VALUE(BHSRV) ;
    
}

#endif
/*****************************************************************************/




