#ifndef _OI_MODULES_H
#define _OI_MODULES_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_modules.h#1 $ 

$Log: $
*
*    #3        30 May 2006            GS
* Added support for PBAP feature.
*

===========================================================================*/
/**
 * @file  
 *
 * Enumeration type defining the inidivual stack components.
 *
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

/**
 * This enumeration lists constants for referencing the components of 
 * the BLUEmagic 3.0 protocol stack, profiles, and other functionalities.
 *
 * In order to distinguish types of modules, items are grouped with markers to 
 * delineate start and end of the groups
 *
 * The module type is used for various purposes:
 *      identification in debug print statements
 *      access to initialization flags
 *      access to the configuration table
 */
 
typedef enum {
    /* profiles and protocols  --> Updates to oi_debug.c and oi_config_table.c */

    OI_MODULE_AT,           /**< AT command processing */
    OI_MODULE_A2DP,         /**< Advanced Audio Distribution Profile */
    OI_MODULE_AVCTP,        /**< Audio-Visual Control Transport Profile */
    OI_MODULE_AVDTP,        /**< Audio-Visual Distribution Protocol */
    OI_MODULE_AVRCP,        /**< Audio-Visual Remote Control Profile */
    OI_MODULE_BIP_CLI,      /**< Basic Imaging Profile protocol client */
    OI_MODULE_BIP_SRV,      /**< Basic Imaging Profile protocol server */
    OI_MODULE_BNEP,         /**< Bluetooth Network Encapsulation Protocol */
    OI_MODULE_BPP_SENDER,   /**< Basic Printing Profile */
    OI_MODULE_BPP_PRINTER,  /**< Basic Printing Profile */
    OI_MODULE_CTP,          /**< Cordless Telephony Profile */
    OI_MODULE_DUN,          /**< Dial-Up Networking Profile */
    OI_MODULE_FAX,          /**< Fax Profile */
    OI_MODULE_FTP_CLI,      /**< File Transfer Profile protocol client */
    OI_MODULE_FTP_SRV,      /**< File Transfer Profile protocol server */
    OI_MODULE_HANDSFREE,    /**< Hands-Free Profile */
    OI_MODULE_HANDSFREE_AG, /**< Hands-Free Profile */
    OI_MODULE_HCRP_CLI,     /**< Hardcopy Cable Replacement Profile */
    OI_MODULE_HCRP_SRV,     /**< Hardcopy Cable Replacement Profile */
    OI_MODULE_HEADSET,      /**< Headset Profile */
    OI_MODULE_HEADSET_AG,   /**< Headset Profile */
    OI_MODULE_HID,          /**< Human Interface Device profile */
    OI_MODULE_INTERCOM,     /**< Intercom Profile */
    OI_MODULE_LAN,          /**< LAN Access Profile */
    OI_MODULE_OBEX_CLI,     /**< OBEX protocol client, Generic Object Exchange Profile */
    OI_MODULE_OBEX_SRV,     /**< OBEX protocol server, Generic Object Exchange Profile */
    OI_MODULE_OPP_CLI,      /**< Object Push Profile protocol client */
    OI_MODULE_OPP_SRV,      /**< Object Push Profile protocol server */
    OI_MODULE_PAN,          /**< PAN profile */
    OI_MODULE_SAP_CLI,      /**< SIM Access Profile */
    OI_MODULE_SAP_SRV,      /**< SIM Access Profile */
    OI_MODULE_SPP,          /**< Serial Port Profile */
    OI_MODULE_SYNC_CLI,     /**< Synchronization Profile */
    OI_MODULE_SYNC_SRV,     /**< Synchronization Profile */
    OI_MODULE_SYNC_CMD_CLI, /**< Synchronization Profile */
    OI_MODULE_SYNC_CMD_SRV, /**< Synchronization Profile */
    OI_MODULE_SYNCML,       /**< SyncML Profile */
    OI_MODULE_TCS,          /**< TCS Binary */

    /* QC extensions */
    OI_MODULE_PBAP_CLI,     /**< Phone Book Access Profile protocol client */
    OI_MODULE_PBAP_SRV,     /**< Phone Book Access Profile protocol server */
    
    /* corestack components   --> Updates to oi_debug.c and oi_config_table.c */
    
    OI_MODULE_COMMON_CONFIG,/**< Common configuration, module has no meaning other than for config struct */
    OI_MODULE_CMDCHAIN,     /**< Command chaining utility */
    OI_MODULE_DISPATCH,     /**< Dispatcher */
    OI_MODULE_DATAELEM,     /**< Data Elements, marshaller */
    OI_MODULE_DEVMGR,       /**< Device Manager */
    OI_MODULE_HCI,          /**< Host Controller Interface command layer */
    OI_MODULE_L2CAP,        /**< L2CAP */
    OI_MODULE_MEMMGR,       /**< modules that do memory management */
    OI_MODULE_POLICYMGR,    /**< Policy Manager */
    OI_MODULE_RFCOMM,       /**< RFCOMM */
    OI_MODULE_RFCOMM_SD,    /**< RFCOMM Service discovery */
    OI_MODULE_SDP_CLI,      /**< Service Discovery Protocol client */
    OI_MODULE_SDP_SRV,      /**< Service Discovery Protocol server */
    OI_MODULE_SDPDB,        /**< Service Discovery Protocol database */
    OI_MODULE_SECMGR,       /**< Security Manager */
    OI_MODULE_SECURITYDB,   /**< Security service database */
    OI_MODULE_SNIFFLOG,     /**< sniff log */
    OI_MODULE_SUPPORT,      /**< support functions, including CThru Dispatcher, time functions, and stack initialization */
    OI_MODULE_TRANSPORT,    /**< transport layer between HCI command layer and driver */
    OI_MODULE_TEST,         /**< used to debug output from internal test programs */
    
    // bhapi components  --> Updates to oi_debug.c
    
    OI_MODULE_BHAPI,        /**< BLUEmagic Host API generic */
    OI_MODULE_BHCLI,        /**< BLUEmagic Host API client side */
    OI_MODULE_BHSRV,        /**< BLUEmagic Host API server side */
    OI_MODULE_MSGQ,         /**< module that handles message queuing */
    
            /* various pieces of code depend on these last 2 elements occuring in a specific order:
                    OI_MODULE_ALL must be the 2nd to last element
                    OI_MODULE_UNKNOWN must be the last element 
            */
    OI_MODULE_ALL,         /**< special value identifying all modules - used for control of debug print statements */
    OI_MODULE_UNKNOWN      /**< special value - used for debug print statements */
} OI_MODULE;

/**
 * This constant is the number of actual modules in the list.  ALL and UNKNOWN are 
 * special values that are not actually modules.
 * Used for debug print and memmgr profiling
 */
#ifdef OI_CODE
#error code not present
#else
#define OI_NUM_MODULES  OI_MODULE_UNKNOWN + 1
#endif


/**
 * This constant is the number of profile and core components.  It is used to size
 * the initialization and configuration tables.
 */
#define OI_NUM_STACK_MODULES    OI_MODULE_BHAPI     


#endif /* _OI_MODULES_H */

