#define _STACK_CONFIG_C_INCLUDED_
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bt_stack_config_default.c#1 $ 

$Log: $
*
*    #2        04 Nov 2004            JTL
* No longer allocating 32k for heap, since it was unused.
*
===========================================================================*/
/** 
 *  @file 
 *  This file defines configuration parameters for various modules, including the CThru Dispatcher,
 *  Memory Manager, Security Manager, security database, Device Manager, Policy Manager, and core protocol
 *  stack modules.
 * 
 *  Values in this file may be changed; these values will be used to populate instances of the data structures
 *  defined in oi_bt_stack_config.h that will be sent as arguments to initialization and configuration functions.
 *  See the the @ref init_docpage section for details of recommended and required usage of these values. 
 *  Do not change the file oi_bt_stack_config.h.
 *
 *  Many of the parameters in this file affect the amount of RAM memory that the stack requires.
 *  To accomodate embedded platforms where RAM may be limited, some configurations in this file 
 *  will depend on whether SMALL_RAM_MEMORY is defined or not. If SMALL_RAM_MEMORY is defined,
 *  configuration will defined for less memory usage. If APP_MEM_CONFIG is defined, the application
 *  will configure memory usage outside of this default configuration file.
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/
#include "oi_stddefs.h"
#include "oi_time.h"
#include "oi_bt_stack_config.h"
#include "oi_hcispec.h"
#include "oi_bt_assigned_nos.h"
#include "oi_dataelem.h"
#include "oi_sdpconfig.h"

/* In order to facilitate backwards compatibility with existing releases,
    include the new file with profile configurations.  The profile configuration
    file should include guards as if it were a .h file
*/
#include "oi_bt_profile_config_default.c"    


/**************************************************
  common configuration
 **************************************************/

/** This constant structure configures parameters used by several modules of the 
    BLUEmagic 3.0 Bluetooth protocol stack and supporting layers. */
const OI_CONFIG_COMMON_CONFIG oi_default_config_COMMON_CONFIG =  {
    FALSE,                /* OI_BOOL     scoSupported */

#ifdef SMALL_RAM_MEMORY
    2,                    /* OI_UINT8    maxAclConnections */

    2,                    /* OI_UINT8    maxScoConnections */

    26,                   /* OI_UINT8    deviceNameMaxLen */
#else
#error code not present
#endif

    1700                  /*  OI_UINT16 maxMTU */
} ;

/**************************************************
  Dispatcher configuration
 **************************************************/

/** This constant structure configures the size of the table used by the CThru Dispatcher
 *  to store callback function registration entries. The parameter indicates the size of
 *  the table in 16-bit units, not number of entries. An entry consumes between 16 and 20 bytes.
 *  A reasonable parameter setting for many applications would be 64. A feasible minimum might 
 *  be between 16 and 32.
 */
const OI_CONFIG_DISPATCH oi_default_config_DISPATCH = {
    64                   /* OI_UINT16 DispatchTableSize */
};

/**************************************************
  Memory Manager pool table
  
    This file defines two default settings for the Memory Manager heap and memory pool configuration:
        - one for large platforms (e.g., Windows and Linux PCs and desktop Linux), 
          which have essentially unlimited memory
        - one for small platforms, typically embedded platforms with limited RAM
    
    These default settings may not be appropriate for a specific application. For those cases,
    the Memory Manager configuration can be defined somewhere else, in which case this file simply creates
    an external reference.
  
 **************************************************/

/** This definition sets the size in bytes of the heap, the section of memory set aside
 *  for allocation by the Memory Manager.     
 */
 
#ifdef  APP_MEM_CONFIG
#error code not present
#else

    /* One of the following default memory configurations will be used. */
#ifdef  SMALL_RAM_MEMORY
#define MEMMGR_TOTAL_HEAP_SIZE  (32764)     
#else
#error code not present
#endif

/** The heap is declared to be an array of elements of type OI_UINT32, with 
 *  a size of @ref MEMMGR_TOTAL_HEAP_SIZE. OI_UINT32 elements ensure proper
 *  byte alignment on all platforms. 
 *
 *  Note: The array is auto-initialized because some linkers require
 *  auto-initialization to perform explicite Locating. If this causes
 *  an increase in flash usage or you do not need to explicitly locate
 *  this memory block, then the auto-initializer can be safely deleted.
 */
#ifdef OI_CODE
#error code not present
#endif /* OI_CODE */

/** This array is a table of pairs that indicate how the Memory Manager will partition
    the portion of the heap from which it dynamically allocates memory. 
    A memory pool is a linked list of memory blocks of the same size. Each entry
    in this table configures one memory pool: the first field indicates the number of blocks 
    that will be allotted in a pool; the second field indicates the size of the blocks in this pool.    
    For example, the entry {100, 64} indicates that a pool of 100 64-byte blocks will be set aside.

    Note:
    - The last entry must always be a pair of zeroes. 
    - The block sizes must be multiples of 4.
    - The pool table must be ordered such that block sizes are in increasing order.
*/
#ifdef  SMALL_RAM_MEMORY
static const OI_MEMMGR_POOL_CONFIG memmgrPoolTable[] =
{     
    {10,     8},
    {10,    16},
    {10,    32},
    {10,    80},
    {30,   128},
    { 5,   256},
    { 4,   400},
    { 6,   640},
    { 4,  1024},
    { 2,  2048},
    { 0,     0}

};
#else
#error code not present
#endif


/** This single structure is used for passing all memory configuration 
 *  information to OI_MEMMGR_Init(). 
 */
const OI_CONFIG_MEMMGR  oi_default_config_MEMMGR =
{
    &memmgrPoolTable[0],
#ifdef OI_CODE
#error code not present
#else
    NULL
#endif
} ;

#endif  /* default memory configuration */
/**************************************************
  HCI configurations
 **************************************************/

/** This constant structure is used by HCI initialization 
*/
const OI_CONFIG_HCI    oi_default_config_HCI =  {

    1024,       /* OI_UINT16 transportMaxRxBufSize (large default value chosen) */

    8,          /* OI_UINT8  maxPendingCommands */
                     
    5,          /* OI_UINT8  cmdCompleteTimeoutSecs */
                     
    5,          /* OI_UINT8  cmdStatusTimeoutSecs */
                     
    90,         /* OI_UINT8  cmdCompleteEventTimeoutSecs */
    
    FALSE,      /* OI_BOOL   hostAclFlowControlEnabled */
    FALSE,      /* OI_BOOL   hostScoFlowControlEnabled */
#ifdef NO_RESET_CMD
    TRUE,       /* OI_BOOL   initialResetCmdDisabled */
#else
    FALSE,      /* OI_BOOL   initialResetCmdDisabled */
#endif                     
};

/**************************************************
 * Security Manager configuration parameters
 **************************************************/

/**   This constant structure is used to pass Security Manager configuration
      parameters to OI_SECMGR_Init(). This does not set a security policy. Setting
      a security policy is accomplished through a call to OI_SECMGR_Configure(). 
      See the @ref secmgr_api section for more information on setting a security
      policy. 
*/
const OI_CONFIG_SECMGR    oi_default_config_SECMGR =  {

    OI_SECONDS(30),     /* OI_INTERVAL policyEnforcementTimeout  */

    4,                  /* OI_UINT8    minPinCodeLength */
                     
    FALSE,              /* OI_BOOL     unitKeyPairingSupported */
                     
} ;

/**************************************************
  Device Manager configuration parameters
 **************************************************/

/** This constant structure is the default Device Manager configuration */

const OI_CONFIG_DEVMGR    oi_default_config_DEVMGR =  {
    12,                 /* OI_UINT16      inquiryCacheSize */
                             
    OI_SECONDS(1),      /* OI_INTERVAL    disconnectTimeout */
                             
    OI_SECONDS(20),     /* OI_INTERVAL    disconnectTimeoutZombie */

    4,                  /* OI_UINT8       maxConcurrentConnectReqs */
                             
    2,                  /* OI_UINT8       maxCallbacksPerEvent */
                             
    OI_DHCI_ALL_PKT_TYPES,  /* OI_UINT16      aclPacketTypes */
    
                        /* OI_UINT16   powerSaveLinkPolicy */
    (OI_DHCI_LINKPOLICY_ENABLE_HOLD | OI_DHCI_LINKPOLICY_ENABLE_SNIFF | OI_DHCI_LINKPOLICY_ENABLE_PARK),
                        
    1                   /* OI_UINT8    maxPiconetsSupported */ 
} ;


/**************************************************
  Policy Manager configuration parameters
 **************************************************/

const OI_CONFIG_POLICYMGR oi_default_config_POLICYMGR = {
    20,                 /* OI_UINT16   maxConcurrentConnections */ 
    {
        /* OI_CONNECT_POLICY   defaultConnectPolicy */
        OI_UUID_NULL,                       /* OI_UUID32           serviceUuid32 ;*/
        OI_SEC_REQUIRE_NOTHING,             /* OI_SECURITY_POLICY  securityPolicy ; */
        FALSE,                              /* OI_BOOL             mustBeMaster ;*/
        NULL,                               /* OI_L2CAP_FLOWSPEC   *flowspec;    */
        0                                   /* OI_UINT8            powerSavingDisables ;    */
    }
};

/**************************************************
    security database configuration
**************************************************/

const OI_CONFIG_SECURITYDB oi_default_config_SECURITYDB[] =  { 

//    { OI_UUID_OBEXFileTransfer, OI_SEC_REQUIRE_AUTHENTICATION },

    { OI_UUID_NULL,     OI_SEC_REQUIRE_NOTHING }      // terminator
} ;

/**************************************************
  L2CAP configuration parameters
 **************************************************/

/** This constant structure is used to pass L2CAP configuration parameters to OI_L2CAP_Init(). */
const OI_CONFIG_L2CAP oi_default_config_L2CAP = {
    4,           /*  OI_UINT16 maxPSMs */
    4,           /*  OI_UINT16 maxChannels */
    670,         /*  OI_UINT16 connectionlessMTU */
    OI_SECONDS(6),  /*  OI_INTERVAL RTXTimeout */
    OI_SECONDS(60) /*  OI_INTERVAL ERTXTimeout */
};


/**************************************************
  RFCOMM configuration parameters
 **************************************************/

/** This constant structure is used to pass RFCOMM configuration parameters to OI_RFCOMM_Init(). */
const OI_CONFIG_RFCOMM oi_default_config_RFCOMM = {
    2,            /* OI_UINT8     maxSessions */
    3,            /* OI_UINT8     maxLinksPerSession */
    6,            /* OI_UINT8     maxServers; */
    5,            /* OI_UINT8     creditGrantThreshold */
    OI_SECONDS(20),  /* OI_INTERVAL  frameTimeout */
    OI_SECONDS(60),  /* OI_INTERVAL  extendedFrameTimeout */
    OI_SECONDS(20),  /* OI_INTERVAL  commandTimeout */
};


/**************************************************
  SDP server configuration parameters
 **************************************************/

/** A structure of this type is used to pass SDP server configuration parameters to OI_SDPServer_Init(). */
const OI_CONFIG_SDP_SRV oi_default_config_SDP_SRV = {
    3,             /* OI_UINT8    MaxClientConnections */
    256,           /* OI_UINT16   MaxResponseLen */
    256,           /* OI_UINT16   MaxRequestLen */
    32             /* OI_UINT16   SDPDBMaxServiceRecords */
};

/** This is the language list to use for all service records. The default language must be first. */
static const OI_DATAELEM LanguageList[] = {

    OI_ELEMENT_UINT16(OI_LANG_ID_ENGLISH), /* Default language MUST be first. */
    OI_ELEMENT_UINT16(OI_LANG_ENCODING_UTF8),
    OI_ELEMENT_UINT16(BASE_ENGLISH(0)),

    OI_ELEMENT_UINT16(OI_LANG_ID_FRENCH),
    OI_ELEMENT_UINT16(OI_LANG_ENCODING_UTF8),
    OI_ELEMENT_UINT16(BASE_FRANCAIS(0)),

    OI_ELEMENT_UINT16(OI_LANG_ID_GERMAN),
    OI_ELEMENT_UINT16(OI_LANG_ENCODING_UTF8),
    OI_ELEMENT_UINT16(BASE_DEUTSCH(0)),
    
    OI_ELEMENT_UINT16(OI_LANG_ID_JAPANESE),
    OI_ELEMENT_UINT16(OI_LANG_ENCODING_UTF8),
    OI_ELEMENT_UINT16(BASE_JAPANESE(0))
};

/** The language attribute list: this is shared by all service records */
OI_SDPDB_ATTRIBUTE OI_SDP_LanguageList =
    { OI_ATTRID_LanguageBaseAttributeIDList, OI_ELEMENT_SEQ(LanguageList) };


/*****************************************************************************/
