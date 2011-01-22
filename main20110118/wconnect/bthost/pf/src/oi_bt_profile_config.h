#ifndef _OI_BT_PROFILE_CONFIG_H
#define _OI_BT_PROFILE_CONFIG_H
/**
 * @file  
 * This file provides type definitions for configuration of data structures for each 
 * BLUEmagic 3.0 profile, used for configuring memory usage.
 *
 * The comments in this file contain details on recommended and required usage of the
 * parameters defined herein. The values of these parameters may be changed in the file 
 * oi_bt_profile_config_default.c.
 * 
 * Adhering to naming conventions is critical to proper operation.
 *  The typedef for the configuration structure is 
 *      OI_CONFIG_<module>
 *  where module is one of the enumerated module IDs, minus the OI_MODULE_ prefix 
 * (e.g., OI_CONFIG_AVDTP).
 * 
 */

/** \addtogroup InitConfig */
/**@{*/

/******************************************************************************
  $Revision: #1 $
  Copyright 2003 - 2004 Open Interface North America, Inc. All rights reserved.
*******************************************************************************/

#include "oi_stddefs.h"
#include "oi_time.h"
#include "oi_bt_spec.h"
#include "oi_rfcomm_prefs.h"


/********************************************************/

/** AT command processing:
        There are currently no configuration parameters for 
        this module. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;
} OI_CONFIG_AT;

/********************************************************/

/** Audio/Video Distribution Transport Protocol configuration structure
*/
typedef struct {

    /** the maximum number of endpoints supported */
    OI_UINT8 maxEndpoints;

} OI_CONFIG_AVDTP;


/** Audio/Video Distribution Transport Protocol configuration
*/
extern const OI_CONFIG_AVDTP oi_default_config_AVDTP;

/********************************************************/

/** Audio-Video Remote Control Profile configuration structure
*/
typedef struct  {

    /** maximum number of outstanding transactions */
    OI_UINT8 maxTransactions;
    /** maximum number of connections */
    OI_UINT8 maxConnections;
    /** indicator for Metadata support */
    OI_BOOL metadata;
    /** time period within which Target shall respond to any command except VENDOR DEPENDENT */
    OI_INTERVAL timeoutRCP;
    /** time period before which Target is expected to generate a response for VENDOR DEPENDENT CONTROL commands. */
    OI_INTERVAL timeoutMTC;
    /** time period before which Target is expected to generate an interim response for VENDOR DEPENDENT NOTIFY or
     * final response for VENDOR DEPENDENT STATUS commands. */
    OI_INTERVAL timeoutMTP;
    /** Maximum length of TG response frame. Should not exceed 512 bytes and be less that 16 bytes. 
     * This configuration parameter applies only to AVRCP target, controller must be able to accept 512-bytes frames. */
    OI_UINT16 tgResponseFrameSize;
} OI_CONFIG_AVRCP;

/** Audio-Video Remote Control Profile configuration
 */
extern const OI_CONFIG_AVRCP oi_default_config_AVRCP;

/********************************************************/

/** Basic Imaging Profile

*/
typedef struct {
    OI_UINT32 dummy;
} OI_CONFIG_BIP_CLI;

typedef struct  {
    OI_UINT maxServers;      /**< number of servers that can be registered simultaneously     */
} OI_CONFIG_BIP_SRV;            

extern const OI_CONFIG_BIP_SRV oi_default_config_BIP_SRV;

#ifndef OI_CODE
/* Phone Book Access Profile */
typedef struct  {
    OI_UINT maxServers;      /**< number of servers that can be registered simultaneously     */
} OI_CONFIG_PBAP_SRV;            

extern const OI_CONFIG_PBAP_SRV oi_default_config_PBAP_SRV;
#endif /* OI_CODE */

/********************************************************/

/** Bluetooth Network Encapsulation Protocol:
         A structure of this type is used to initialize BNEP.
*/

typedef struct {

    /** This parameter specifies the maximum number of services that can be registered with the BNEP module. */
    OI_UINT16 maxServices;

    /** This parameter specifies the maximum number of BNEP connections. */
    OI_UINT16 maxConnections;

    /** the timeout before resending a setup control message */
    OI_INTERVAL setupTimeout;

    /** the timeout before resending a filter control message */
    OI_INTERVAL filterTimeout;

} OI_CONFIG_BNEP;

extern const OI_CONFIG_BNEP oi_default_config_BNEP;

/********************************************************/

/** Basic Printing Profile configuration structure:
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_BPP_SENDER;            

typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_BPP_PRINTER;            

/********************************************************/

/** Cordless Telephony Profile configuration structure:
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_CTP;            

/********************************************************/

/** Dial-Up Networking Profile configuration structure:

*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
} OI_CONFIG_DUN;            

extern const OI_CONFIG_DUN oi_default_config_DUN;

/********************************************************/

/** Fax Profile configuration structure:

*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
} OI_CONFIG_FAX;            

extern const OI_CONFIG_FAX oi_default_config_FAX;

/********************************************************/

/** File Transfer Profile configuration structure:

*/

typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
} OI_CONFIG_FTP_SRV;            

typedef struct  {
    OI_UINT8 max_folder_tree_depth;         /**< maximum folder recursion for folder put/get */
} OI_CONFIG_FTP_CLI;            

extern const OI_CONFIG_FTP_SRV oi_default_config_FTP_SRV;
extern const OI_CONFIG_FTP_CLI oi_default_config_FTP_CLI;

/********************************************************/

/** Hands-Free Profile configuration structure:

*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
    OI_INTERVAL command_timeout;            /**< The timeout to receive a response from an AT command.  This is applicable
                                                 to all commands except those listed for command_timeout_long. */
    OI_INTERVAL command_timeout_long;       /**< The timeout for responses requiring user or network interaction.  This
                                                 includes the D, D>, BLDN, BINP, A, and vendor commands. */
    /**
     * Indicate whether the profile should initiate a disconnect when a timeout occurs.  This defaults to TRUE,
     * and under almost all circumstances, this is the correct behavior.  However, to interoperate with a GW
     * that does not respond correctly, this may be set to FALSE.  Note that sending any commands after a timeout
     * is inherently problematic since the sides may be out of sync with each other.
     */
    OI_BOOL disconnectOnTimeout;
} OI_CONFIG_HANDSFREE;    

typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
    OI_INTERVAL send_timeout;               /**< timeout on send complete for AT response */
} OI_CONFIG_HANDSFREE_AG;

extern const OI_CONFIG_HANDSFREE oi_default_config_HANDSFREE;
extern const OI_CONFIG_HANDSFREE_AG oi_default_config_HANDSFREE_AG;

/********************************************************/

/** Hardcopy Cable Replacement Profile configuration structure:

*/
typedef struct { 
    OI_UINT16 maxClients;           /**< HCRP server: maximum number of clients supported */
} OI_CONFIG_HCRP_SRV;

extern const OI_CONFIG_HCRP_SRV oi_default_config_HCRP_SRV;

/********************************************************/

/** Headset Profile configuration structure:

*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
    OI_INTERVAL command_timeout;            /**< timeout to receive a response from an AT command */
    /**
     * Indicate whether the profile should initiate a disconnect when a timeout occurs.  This defaults to TRUE,
     * and under almost all circumstances, this is the correct behavior.  However, to interoperate with a GW
     * that does not respond correctly, this may be set to FALSE.  Note that sending any commands after a timeout
     * is inherently problematic since the sides may be out of sync with each other.
     */
    OI_BOOL disconnectOnTimeout;
} OI_CONFIG_HEADSET;        

typedef struct  {
    OI_UINT8 rfcomm_channel_pref;           /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
    OI_INTERVAL send_timeout;               /**< timeout on send complete for AT response */
} OI_CONFIG_HEADSET_AG; 
       
extern const OI_CONFIG_HEADSET oi_default_config_HEADSET;
extern const OI_CONFIG_HEADSET_AG oi_default_config_HEADSET_AG;

/********************************************************/

/** Human Interface Device profile configuration structure:
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_HID;            

/********************************************************/

/** Intercom Profile configuration structure: 
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_INTERCOM;      

/********************************************************/

/** LAN Access Profile configuration structure: 
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_LAN;            

/********************************************************/

/**  Generic Object Exchange Profile client configuration structure:
*/

typedef struct {
    OI_INTERVAL responseTimeout;   /**< how long to wait for a response to an OBEX command */
    OI_UINT     unused;
    OI_UINT16   maxPktLen;         /**< maximum length of a request packet */
    OI_CHAR     *privateKey;       /**< key used for OBEX authentication */
} OI_CONFIG_OBEX_CLI;

/**  Generic Object Exchange Profile server configuration structure:
*/
typedef struct {
    OI_UINT     unused;
    OI_UINT16   maxPktLen;         /**< maximum length packet (must be >= 255) */
    OI_CHAR     *privateKey;       /**< pointer to key used for OBEX authentication */
} OI_CONFIG_OBEX_SRV;

extern const OI_CONFIG_OBEX_CLI     oi_default_config_OBEX_CLI;
extern const OI_CONFIG_OBEX_SRV     oi_default_config_OBEX_SRV;

/********************************************************/

/** Object Push Profile configuration structure: 
*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;     /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
} OI_CONFIG_OPP_SRV;            

extern const OI_CONFIG_OPP_SRV oi_default_config_OPP_SRV;

/********************************************************/

/** PAN profile configuration structure: 
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_PAN;            

/********************************************************/

/** Phonebook Access Profile configuration structure:

*/

#ifdef OI_CODE
#error code not present
#endif

/********************************************************/

/** SIM Access Profile configuration structure:
*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;     /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
} OI_CONFIG_SAP_SRV;    

extern const OI_CONFIG_SAP_SRV oi_default_config_SAP_SRV;

/********************************************************/

/** Serial Port Profile configuration structure:
*/
typedef struct  {
    OI_UINT8 rfcomm_channel_pref;     /**< preferred RFCOMM channel ID (from oi_rfcomm_prefs.h) */
} OI_CONFIG_SPP;    

extern const OI_CONFIG_SPP oi_default_config_SPP;

/********************************************************/

/** Synchronization Profile configuration structure: 
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_SYNC;          

/********************************************************/

/** TCS Binary configuration structure: 
        There are currently no configuration parameters for 
        this profile. The struct below is just a placeholder.    
*/
typedef struct  {
    OI_UINT32 dummy;    
} OI_CONFIG_TCS;            

/********************************************************/

#endif  /* _OI_BT_PROFILE_CONFIG_H */

/**@}*/
