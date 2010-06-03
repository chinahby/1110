#ifndef _OI_BT_PROFILE_CONFIG_C
#define _OI_BT_PROFILE_CONFIG_C
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bt_profile_config_default.c#1 $ 

$Log: $
*    #9       10 Jan 2007            DSN
* Fixing typos in featurization.
*
*    #8       22 Dec 2006            DSN
* Change OBEX packet size for BT2.0+ EDR based SOC solutions.
*
*    #7       30 Aug 2006        JH
* Set maxPktLen field in oi_default_config_OBEX_* structure to the proper value
*  since OBEX packet overhead should not be addressed here.
*
*    #6       15 Mar 2005        JH
* Increased OBEX client and server max receive packet length from 2K to 4266.
*
*    #5       11 Nov 2004        JH
* Increased OBEX client and server max receive packet length from 1K to 2K.
*
===========================================================================*/
/** 
 *  @file 
 *  This file defines configuration parameters for various profiles and protocols.
 * 
 *  Values in this file may be changed; these values will be used to populate instances 
 *  of the data structures defined in oi_bt_profile_config.h that will be used during 
 *  profile initialization and configuration functions.
 *
 *  Do not change the file oi_bt_profile_config.h.
 *
 */

/**********************************************************************************
  $Revision: #1 $
  Copyright 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_rfcomm_prefs.h"
#include "oi_bt_profile_config.h"


/****************************************************/

/** AT command processing */

// const OI_CONFIG_AT      oi_default_config_AT = {  };

/****************************************************/

/* Audio/Video Distribution Transport Protocol configuration
*/
const OI_CONFIG_AVDTP   oi_default_config_AVDTP =
{ 
    3   //OI_UINT8 maxEndpoints - number of endpoints that can be registered
};        

/****************************************************/

/* Audio-Video Remote Control Profile configuration */

const OI_CONFIG_AVRCP   oi_default_config_AVRCP =
{
    16,   //OI_UINT8 maxTransactions - number of outstanding commands supported
    16,   //OI_UINT8 maxConnections - number of simultaneous AVRCP sessions supported
    TRUE, //OI_BOOL  metadata - indicator for Metadata support
    OI_MSECONDS(400), // OI_INTERVAL tRCP - time period within which Target shall respond to any command except VENDOR DEPENDENT
    OI_MSECONDS(800), // OI_INTERVAL tMTC - time period before which Target is expected to generate a response for VENDOR DEPENDENT CONTROL commands
    OI_MSECONDS(2000),  // OI_INTERVAL tMTP - time period before which Target is expected to generate  an interim response for VENDOR DEPENDENT NOTIFY or final response for VENDOR DEPENDENT STATUS commands
    512               // Maximum frame length for target-generated response. Should not exceed 512 bytes and be less that 16 bytes. 
};

/****************************************************/

/** Basic Imaging Profile */

const OI_CONFIG_BIP_SRV oi_default_config_BIP_SRV = 
{
    3     // OI_UINT     maxServers - numer of servers that can be registered simaltaneously    
};

/****************************************************/

/** Bluetooth Network Encapsulation Protocol */

const OI_CONFIG_BNEP oi_default_config_BNEP = 
{  
    2,             //OI_UINT16 maxServices;
    2,             //OI_UINT16 maxConnections;
    OI_SECONDS(5), //OI_INTERVAL setupTimeout;
    OI_SECONDS(5)  //OI_INTERVAL filterTimeout;
};

/****************************************************/

/** Basic Printing Profile */

// const OI_CONFIG_BPP_SENDER oi_default_config_BPP_SENDER = {  };

// const OI_CONFIG_BPP_PRINTER oi_default_config_BPP_PRINTER = {  };

/****************************************************/

/** Cordless Telephony Profile */

// const OI_CONFIG_CTP     oi_default_config_CTP = {  };

/****************************************************/

/** Dial-Up Networking Profile */

const OI_CONFIG_DUN oi_default_config_DUN =  
{
    RFCOMM_PREF_DUN         // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

/****************************************************/

/** Fax Profile */

const OI_CONFIG_FAX oi_default_config_FAX =  
{
    RFCOMM_PREF_FAX         // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

/****************************************************/

/** File Transfer Profile */

const OI_CONFIG_FTP_SRV oi_default_config_FTP_SRV = {
    RFCOMM_PREF_FTP_SRV    // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

const OI_CONFIG_FTP_CLI oi_default_config_FTP_CLI = {
    5        // OI_UINT8 max_folder_tree_depth - maximum folder recursion for folder put/get */
};

/****************************************************/

/** Hands-Free Profile */

const OI_CONFIG_HANDSFREE oi_default_config_HANDSFREE = 
{
    RFCOMM_PREF_HANDSFREE   // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

const OI_CONFIG_HANDSFREE_AG oi_default_config_HANDSFREE_AG = 
{
    RFCOMM_PREF_HANDSFREE_AG // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

/****************************************************/

/** Hardcopy Cable Replacement Profile */

const OI_CONFIG_HCRP_SRV oi_default_config_HCRP_SRV =
{                             
    2       /* OI_UINT16 maxClients; */
};

/****************************************************/

/** Headset Profile */

const OI_CONFIG_HEADSET oi_default_config_HEADSET = 
{
    RFCOMM_PREF_HEADSET     // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

const OI_CONFIG_HEADSET_AG oi_default_config_HEADSET_AG = 
{
    RFCOMM_PREF_HEADSET_AG  // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

/****************************************************/

/** Human Interface Device profile */

// const OI_CONFIG_HID  oi_default_config_HID = {  };

/****************************************************/

/** Intercom Profile */

// const OI_CONFIG_INTERCOM oi_default_config_INTERCOM = {  };

/****************************************************/

/** LAN Access Profile */

// const OI_CONFIG_LAN  oi_default_config_LAN = {  };

/****************************************************/

/** OBEX protocol, Generic Object Exchange Profile */

const OI_CONFIG_OBEX_CLI oi_default_config_OBEX_CLI = {
      100,     // OI_INTERVAL responseTimeout;
      0,       // Unused
    #ifdef  SMALL_RAM_MEMORY
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
     (32768 / 2), 
#else
     32768,
#endif
#else
      /* 
       * 4277 = 329 * 13 where 329 is the RFCOMM ideal packet length
       */ 
      4277,    // OI_UINT16 maxPktLen;
#endif /* FEATURE_BT_SOC */
    #else
#error code not present
    #endif    
      "YYYY Open Interface BLUEmagic 3.0 YYYY"    // OI_CHAR *privateKey;
};


const OI_CONFIG_OBEX_SRV oi_default_config_OBEX_SRV = {
      0,      // Unused
    #ifdef  SMALL_RAM_MEMORY
#ifdef FEATURE_BT_SOC
#ifdef FEATURE_BT_REDUCE_MEMORY_USAGE
     (32768 / 2), 
#else
     32768,
#endif
#else
       /* 
       * 4277 = 329 * 13 where 329 is the RFCOMM ideal packet length
       */  
      4277,   // OI_UINT16   maxPktLen; 
#endif /* FEATURE_BT_SOC */
    #else
#error code not present
    #endif
      "XXXX Open Interface BLUEmagic 3.0 XXXX",   // OI_CHAR     *privateKey;
};

/****************************************************/

/** Object Push Profile */

const OI_CONFIG_OPP_SRV oi_default_config_OPP_SRV = {
    RFCOMM_PREF_OPP_SRV
};

 
/****************************************************/

/** PAN Profile */

// const OI_CONFIG_PAN oi_default_config_PAN = {  };

/****************************************************/

/** SIM Access Profile */

const OI_CONFIG_SAP_SRV oi_default_config_SAP_SRV = {  
    RFCOMM_PREF_SAP_SRV
};

/****************************************************/

/** Serial Port Profile */

const OI_CONFIG_SPP oi_default_config_SPP = {  
    RFCOMM_PREF_SPP  // OI_UINT8    rfcomm_channel_pref - preferred rfcomm channel id (from oi_rfcomm_prefs.h)
};

/****************************************************/

/* Synchronization Profile */

// const OI_CONFIG_SYNC oi_default_config_SYNC = {  };

/****************************************************/

/* TCS Binary */

// const OI_CONFIG_TCS oi_default_config_TCS = {  };

/****************************************************/

#endif  // _OI_BT_PROFILE_CONFIG_C

