/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_config_table.c#1 $ 

$Log: $
*
*    #4        29 Aug 2006            GS
* Fixing klocwork error.
*
*    #3        26 Jan 2005            JH
* Removed all arguments besides the format string from calls to OI_LOG_ERROR() 
*  as part of the modification to enable these error messages.
*
===========================================================================*/
/**
 * @file
 
 This file contains the configuration table and access routines.
 
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#define __OI_MODULE__ OI_MODULE_SUPPORT

#include "oi_core_common.h"
#include "oi_modules.h"
#include "oi_config_table.h"
#include "oi_init_flags.h"
#include "oi_bt_stack_config.h"     // core stack config structures typedef'd
#include "oi_bt_profile_config.h"   // profile config structures typedef'd


/* The configuration table contains pointers to configuration structures.
    In release mode, the table is indexed directly by module id.
    In debug mode, the table contains the module id as well as the config pointer in order to
        verify consistency of the module id enum with the configuration table.
*/

typedef struct {
    void    *pConfig ;
    
    #ifdef OI_DEBUG
#error code not present
    #endif
    
} OI_CONFIG_TABLE_ENTRY ;

#ifdef OI_DEBUG
#error code not present
#else
#define DEFAULT_CONFIG_REC(module)  { (void*)(&oi_default_config_##module) }
#define NULL_CONFIG_REC(module)     { NULL }
#endif

/* Constant table of default configurations.

    The entries must be in exactly the same order as enumerated in modules.h */

static const OI_CONFIG_TABLE_ENTRY defaultConfigurations[OI_NUM_STACK_MODULES] =  {
    /* profiles and protocols */
    
    NULL_CONFIG_REC(AT),                    /**< AT command processing */
    NULL_CONFIG_REC(A2DP),                  /**< A2DP */
    NULL_CONFIG_REC(AVCTP),                 /**< Audio-Visual Control Transport Protocol */
    DEFAULT_CONFIG_REC(AVDTP),              /**< Audio-Visual Distribution Protocol */
    DEFAULT_CONFIG_REC(AVRCP),              /**< Audio-Visual Remote Control Profile */
    NULL_CONFIG_REC(BIP_CLI),               /**< Basic Imaging Profile protocol client */
    DEFAULT_CONFIG_REC(BIP_SRV),            /**< Basic Imaging Profile protocol server*/
    DEFAULT_CONFIG_REC(BNEP),               /**< Bluetooth Network Encapsulation Protocol */
    NULL_CONFIG_REC(BPP_SENDER),            /**< Basic Printing Profile */
    NULL_CONFIG_REC(BPP_PRINTER),           /**< Basic Printing Profile */
    NULL_CONFIG_REC(CTP),                   /**< Cordless Telephony Profile */
    DEFAULT_CONFIG_REC(DUN),                /**< Dial-Up Networking Profile */
    DEFAULT_CONFIG_REC(FAX),                /**< Fax Profile */
    DEFAULT_CONFIG_REC(FTP_CLI),            /**< File Transfer Profile protocol client */
    DEFAULT_CONFIG_REC(FTP_SRV),            /**< File Transfer Profile protocol server */
    DEFAULT_CONFIG_REC(HANDSFREE),          /**< Hands-Free Profile */
    DEFAULT_CONFIG_REC(HANDSFREE_AG),       /**< Hands-Free Profile */
    NULL_CONFIG_REC(HCRP_CLI),              /**< Hardcopy Cable Replacement Profile */
    DEFAULT_CONFIG_REC(HCRP_SRV),           /**< Hardcopy Cable Replacement Profile */
    DEFAULT_CONFIG_REC(HEADSET),            /**< Headset Profile */
    DEFAULT_CONFIG_REC(HEADSET_AG),         /**< Hands-Free Profile */
    NULL_CONFIG_REC(HID),                   /**< Human Interface Device profile */
    NULL_CONFIG_REC(INTERCOM),              /**< Intercom Profile */
    NULL_CONFIG_REC(LAN),                   /**< LAN Access Profile */
    DEFAULT_CONFIG_REC(OBEX_CLI),           /**< OBEX protocol, Generic Object Exchange Profile */
    DEFAULT_CONFIG_REC(OBEX_SRV),           /**< OBEX protocol, Generic Object Exchange Profile */
    NULL_CONFIG_REC(OPP_CLI),               /**< Object Push Profile protocol client */
    DEFAULT_CONFIG_REC(OPP_SRV),            /**< Object Push Profile protocol server */
    NULL_CONFIG_REC(PAN),                   /**< PAN profile */
    NULL_CONFIG_REC(SAP_CLI),               /**< SIM Access Profile client */
    DEFAULT_CONFIG_REC(SAP_SRV),            /**< SIM Access Profile server */
    DEFAULT_CONFIG_REC(SPP),                /**< Serial Port Profile */
    NULL_CONFIG_REC(SYNC_CLI),              /**< Synchronization Profile */
    NULL_CONFIG_REC(SYNC_SRV),              /**< Synchronization Profile */
    NULL_CONFIG_REC(SYNC_CMD_CLI),          /**< Synchronization Profile */
    NULL_CONFIG_REC(SYNC_CMD_SRV),          /**< Synchronization Profile */
    NULL_CONFIG_REC(SYNCML),                /**< Synchronization Profile */
    NULL_CONFIG_REC(TCS),                   /**< TCS Binary */
    
    /* corestack components */
    
    DEFAULT_CONFIG_REC(COMMON_CONFIG),      /**< Config common to all modules */
    NULL_CONFIG_REC(CMDCHAIN),              /**< Command chaining utility */
    DEFAULT_CONFIG_REC(DISPATCH),           /**< Dispatcher */
    NULL_CONFIG_REC(DATAELEM),              /**< Data Elements, marshaller */
    DEFAULT_CONFIG_REC(DEVMGR),             /**< Device Manager */
    DEFAULT_CONFIG_REC(HCI),                /**< Host Controller Interface command layer */
    DEFAULT_CONFIG_REC(L2CAP),              /**< L2CAP */
    DEFAULT_CONFIG_REC(MEMMGR),             /**< modules that do memory management */
    DEFAULT_CONFIG_REC(POLICYMGR),          /**< Policy Manager */
    DEFAULT_CONFIG_REC(RFCOMM),             /**< RFCOMM */
    NULL_CONFIG_REC(RFCOMM_SD),             /**< RFCOMM SD */
    NULL_CONFIG_REC(SDP_CLI),               /**< Service Discovery Protocol client */
    DEFAULT_CONFIG_REC(SDP_SRV),            /**< Service Discovery Protocol server */
    NULL_CONFIG_REC(SDPDB),                 /**< Service Discovery Protocol database */
    DEFAULT_CONFIG_REC(SECMGR),             /**< Security Manager */
    DEFAULT_CONFIG_REC(SECURITYDB),         /**< Security Database */
    NULL_CONFIG_REC(SNIFFLOG),              /**< sniff log */
    NULL_CONFIG_REC(SUPPORT),               /**< support functions, including CThru Dispatcher, Memory Manager, time functions, and stack initialization */
    NULL_CONFIG_REC(TRANSPORT),             /**< transport layer between HCI command layer and driver */
    NULL_CONFIG_REC(TEST)                   /**< internal test - no configuration defined */
} ;

/* Ram table of current configurations */

static OI_CONFIG_TABLE_ENTRY curConfig[OI_NUM_MODULES] ;

/*************************************************************

    Initialize the configuration table
    
    Initialization sets the all configuration pointers to their default values.
    This function should be called before initializing any profiles or core stack
    components
    
*************************************************************/

void OI_ConfigTable_Init(void)
{
    const OI_CONFIG_TABLE_ENTRY *src ;
    OI_CONFIG_TABLE_ENTRY       *dest ;
    OI_INT                      i ;

    /* memmgr may not yet be initialized - cannot depend on any OI support at this time. */
    
    src = &defaultConfigurations[0] ;
    dest = &curConfig[0] ;
    
    for (i = 0; i < OI_ARRAYSIZE(defaultConfigurations); ++i) {
        #ifdef OI_DEBUG
#error code not present
        #endif    
        
        *dest = *src ;
        ++src ;
        ++dest ;
    }
}
/*************************************************************

    OI_ConfigTable_GetConfig (module)
    
    Returns the current configuration pointer for the specified module
    
        
*************************************************************/

void* OI_ConfigTable_GetConfig(OI_MODULE module)
{
    OI_ASSERT((module < OI_NUM_STACK_MODULES) && (module >= 0)) ;
    return(curConfig[module].pConfig) ;
}

/*************************************************************

    OI_ConfigTable_SetConfig (configPtr, module)
    
    The specified pointer becomes the current configuration pointer for the indicated module.
        
*************************************************************/

void  OI_ConfigTable_SetConfig(void *configPtr, OI_MODULE module)
{
    OI_ASSERT((module < OI_NUM_STACK_MODULES) && (module >= 0)) ;
    
    // can't set config if module is already initialized
    OI_ASSERT((OI_INIT_FLAG_UNITIALIZED_VALUE == OI_InitFlags_GetFlag(module))) ;
    if (OI_INIT_FLAG_UNITIALIZED_VALUE != OI_InitFlags_GetFlag(module)) {
#ifdef OI_CODE
#error code not present
#else
        OI_LOG_ERROR(("OI_ConfigTable_SetConfig fail: module %d is already initialized"));
#endif
        return ;
    }
    
    // Common configuration can be changed only if ALL modules are uninitialized
    if ((OI_MODULE_COMMON_CONFIG == module) && !OI_InitFlags_AllUninitialized()) {
        OI_LOG_ERROR(("OI_ConfigTable_SetConfig fail: cannot set Common Configuration")) ;
        return ;
    }
    
    curConfig[module].pConfig = configPtr ;
}
/*************************************************************

    Test Code
    
*************************************************************/

// Test code

#if 0
OI_CONFIG_INTERCOM     myConfig ;
OI_CONFIG_INTERCOM     *pConfig ;


void oi_config_table_test(void)
{
    pConfig = OI_CONFIG_TABLE_GET(INTERCOM) ;
        
    OI_CONFIG_TABLE_PUT(&myConfig, INTERCOM) ;
}
#endif 
/*****************************************************************************/



