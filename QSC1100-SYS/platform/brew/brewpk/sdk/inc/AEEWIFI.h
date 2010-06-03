#ifndef AEEWIFI_H
#define AEEWIFI_H

/*============================================================================
FILE:  AEEWIFI.h

SERVICES:  AEE WIFI Manager Interface

GENERAL DESCRIPTION:
        Interface definitions, data structures, etc. for BREW WIFI Manager

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

 ===========================================================================

                     INCLUDE FILES FOR MODULE

 ===========================================================================*/
#include "AEE.h"
#include "AEEWebOpts.h"

#include "WIFI.BID"            /* AEECLSID_WIFI, AEECLSID_WIFI_313 */
#include "WIFI_OPTS.BID"       /* AEECLSID_WIFI_OPTS */

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/* Defines used for AEEWIFI_OPT_TECH option below */
typedef enum 
{
  AEEWIFI_TECH_80211_A   = 1,
  AEEWIFI_TECH_80211_B   = 2,
  AEEWIFI_TECH_80211_G   = 4,
  AEEWIFI_TECH_80211_G_B = (AEEWIFI_TECH_80211_B | AEEWIFI_TECH_80211_G),
  AEEWIFI_TECH_80211_ANY = (AEEWIFI_TECH_80211_A |    \
			    AEEWIFI_TECH_80211_B |    \
			    AEEWIFI_TECH_80211_G),
  AEEWIFI_TECH_80211_MAX = 0x7FFFFFFF
} AEEWIFITechOption;

/* Defines used for AEEWIFI_OPT_BANDPREF option below */
typedef enum 
{
  AEEWIFI_BAND_2400MHZ_US     = 0,
  AEEWIFI_BAND_2400MHZ_EUROPE = 1,
  AEEWIFI_BAND_2400MHZ_FRANCE = 2,
  AEEWIFI_BAND_2400MHZ_SPAIN  = 3,
  AEEWIFI_BAND_2400MHZ_JAPAN  = 4,
  AEEWIFI_BAND_5000MHZ_US     = 5,
  AEEWIFI_BAND_5000MHZ_EUROPE = 6,
  AEEWIFI_BAND_5000MHZ_FRANCE = 7,
  AEEWIFI_BAND_5000MHZ_SPAIN  = 8,
  AEEWIFI_BAND_5000MHZ_JAPAN  = 9,
  AEEWIFI_BAND_MAX            = AEEWIFI_BAND_5000MHZ_JAPAN,
  AEEWIFI_BAND_ANY            = 31,
  AEEWIFI_BAND_INVALID        = 0x7FFFFFFF
} AEEWIFIBandOption;

/* Defines used for AEEWIFI_OPT_WLAN_MODE option below */
typedef enum 
{
  AEEWIFI_MODE_ADHOC = 1,
  AEEWIFI_MODE_INFRA = 2,
  AEEWIFI_MODE_ANY   = (AEEWIFI_MODE_ADHOC | \
			AEEWIFI_MODE_INFRA ),
  AEEWIFI_MODE_MAX   = 0x7FFFFFFF
} AEEWIFIModeOption;

/* Defines used for AEEWIFI_OPT_ENCRYPTION_TYPE option below */
typedef enum
{
  AEEWIFI_ENCRYPTION_NONE           = 0,
  AEEWIFI_ENCRYPTION_WEP_64bit      = 1,
  AEEWIFI_ENCRYPTION_WEP_128bit     = 2,
  AEEWIFI_ENCRYPTION_TKIP           = 3,
  AEEWIFI_ENCRYPTION_AES            = 4,
  AEEWIFI_ENCRYPTION_MAX            = 0x7FFFFFFF
} AEEWIFIEncryptionOption;

/* Defines used for AEEWIFI_OPT_AUTHENTICATION_TYPE option below */
typedef enum
{
  AEEWIFI_AUTHENTICATION_NONE           = 0,
  AEEWIFI_AUTHENTICATION_WEP            = 1,
  AEEWIFI_AUTHENTICATION_8021X_PASSWORD = 2,
  AEEWIFI_AUTHENTICATION_8021X_TLS      = 3,
  AEEWIFI_AUTHENTICATION_8021X_MD5      = 4,
  AEEWIFI_AUTHENTICATION_MAX            = 0x7FFFFFFF
} AEEWIFIAuthenticationOption;

/* Defines used for AEEWIFI_OPT_PREAMBLE_TYPE option below */
typedef enum
{
  AEEWIFI_PREAMBLE_AUTO  = 0,
  AEEWIFI_PREAMBLE_SHORT = 1,
  AEEWIFI_PREAMBLE_LONG  = 2,
  AEEWIFI_PREAMBLE_MAX   = 0x7FFFFFFF
} AEEWIFIPreambleOption;

/* Defines used for AEEWIFI_OPT_DHCP_ENABLED option below */
typedef enum
{
  AEEWIFI_DHCP_DISABLED  = 0,
  AEEWIFI_DHCP_ENABLED   = 1,
  AEEWIFI_DHCP_MAX       = 0x7FFFFFFF
} AEEWIFIDHCPOption;

/* Defines used for AEEWIFI_OPT_IP_TYPE option below */
typedef enum
{
  AEEWIFI_IPTYPE_IPV4  = 0,
  AEEWIFI_IPTYPE_IPV6  = 1,
  AEEWIFI_IPTYPE_MAX   = 0x7FFFFFFF
} AEEWIFIIPTypeOption;

/*===========================================================================
      
                    IWIFIOpts Definitions

===========================================================================*/

/* IWebOpts ID space used by IWIFIOpts */ 
#define WIFI_OPT_OFFSET         0x1800
#define WIFI_OPT_SIZE           0x00FF

/* The range WIFI_OPT_SZ .. WIFI_OPT_SZ_LAST represent an AECHAR string
 * interface to IWIFI clients. 
 *
 * The fact that they map into the char string range for IWebOpt interface
 * provides backward compatibility with BREW 2.1 versions in which 
 * IWebOpt does not have support for AECHAR strings.
 *
 * As a consequence, IWIFI options based on these values should
 * not be passed directly to IWebOpt by IWIFI clients.  They
 * should be manipulated only through the IWIFI interface.
 *
 * [0x00011100, 0x000111FF] */
#define WIFI_OPT_SZ             (WEBOPT_SZ + WIFI_OPT_OFFSET) 
#define WIFI_OPT_SZ_LAST        (WIFI_OPT_SZ + WIFI_OPT_SIZE) 

/* 32-bit quantities */
/* [0x00021100, 0x000211FF] */
#define WIFI_OPT_32BIT          (WEBOPT_32BIT + WIFI_OPT_OFFSET)
#define WIFI_OPT_32BIT_LAST     (WIFI_OPT_32BIT + WIFI_OPT_SIZE)

/* Interface pointers */
/* [0x00031100, 0x000311FF] */
#define WIFI_OPT_INTERFACE      (WEBOPT_INTERFACE + WIFI_OPT_OFFSET)
#define WIFI_OPT_INTERFACE_LAST (WIFI_OPT_INTERFACE + WIFI_OPT_SIZE)

/* Variable length buffer. First two bytes give the length of the following 
   data. The length is little-endian, low byte first. */
/* [0x00041100, 0x000411FF] */
#define WIFI_OPT_VARBUFFER      (WEBOPT_VARBUFFER + WIFI_OPT_OFFSET)
#define WIFI_OPT_VARBUFFER_LAST (WIFI_OPT_VARBUFFER + WIFI_OPT_SIZE)

/* ASN.1 data */
/* [0x00051100, 0x000511FF] */
#define WIFI_OPT_ASN1           (WEBOPT_ASN1 + WIFI_OPT_OFFSET)
#define WIFI_OPT_ASN1_LAST      (WIFI_OPT_ASN1 + WIFI_OPT_SIZE)

/* low 12 bits of the id specify length of the option, in bytes */
/* [0x11100000, 0x111FF000] */
#define WIFI_OPT_VARIABLE       (WEBOPT_VARIABLE + (WIFI_OPT_OFFSET << 3))
#define WIFI_OPT_VARIABLE_LAST  (WIFI_OPT_VARIABLE + (WIFI_OPT_SIZE << 3))


/* Configuration options supported by WIFI. Multiple configurations are   */
/* supported with each having a unique Profile ID. The range of supported */
/* profile IDs is implementation dependent.                               */
#define AEEWIFI_OPT_PROFILE_ID         (WIFI_OPT_32BIT + 0)
  /* Integer Value: Specifies which WIFI Profile this option pertains to.  */
  /*                A Value of 0 indicates the currently used profile.     */
  /*                To make a profile the current active one, commit it as */
  /*                profile id 0                                           */

#define AEEWIFI_OPT_TECH               (WIFI_OPT_32BIT + 1)
  /* See Type: AEEWIFITechOption: Specificies which WIFI technology to use. */

#define AEEWIFI_OPT_BANDPREF           (WIFI_OPT_32BIT + 2)
  /* See Type AEECMBandPref: Specificies which WIFI Band to use. */

#define AEEWIFI_OPT_CHANNEL            (WIFI_OPT_32BIT + 3)
#define AEEWIFI_CHANNEL_MIN             1
#define AEEWIFI_CHANNEL_MAX            14
#define AEEWIFI_CHANNEL_ANY            0x00FF
  /* Integer Value: Specificies which WIFI Channel to use. 1-14 for 802.11 */

#define AEEWIFI_OPT_GROUPS             (WIFI_OPT_32BIT + 4)
  /* Integer Value: Usage TBD. */

#define AEEWIFI_OPT_WLAN_MODE          (WIFI_OPT_32BIT + 5)
  /* See Type: AEEWIFIModeOption: Specifies the WLAN Mode. */

#define AEEWIFI_OPT_SSID               (WIFI_OPT_SZ + 0)
  /* AECHAR String: Specifies the SSID associated with this profile. */

#define AEEWIFI_OPT_ENCRYPTION_TYPE    (WIFI_OPT_32BIT + 6)
  /* AEEWIFIEncryptionOption: Specificies the encryption type to use */

#define AEEWIFI_OPT_AUTHENTICATION_TYPE (WIFI_OPT_32BIT+ 7)
  /* AEEWIFIAuthenticationOption: Specificies the authentication to use */

#define AEEWIFI_OPT_MOB_IP_PROFILE_ID  (WIFI_OPT_32BIT + 8)
  /* Integer Value: Usage TBD. */

#define AEEWIFI_OPT_WEP_KEY_ID      (WIFI_OPT_32BIT + 9)
 /* DEPRECATED: Integer Value, if multiple WEP Keys are present, which to use (0 based). */

#define AEEWIFI_OPT_WEP_KEY        (WIFI_OPT_SZ + 1)
 /* DEPRECATED: AECHAR Null Terminated String: Used for WEP encryption with current key id.
  * DEPRECATED: Note: To program all 4 WEP keys, each key needs to be added using
  * DEPRECATED: AEEWIFI_OPT_WEP_KEY, AEEWIFI_OPT_WEP_KEY_ID pair  */

/* Simplified WEP Key programming constants to set multiple keys simultaneously */
#define AEEWIFI_OPT_WEP_64BIT_KEY_ID      (WIFI_OPT_32BIT + 17)
 /* Integer Value: Used to identify one of multiple 64-bit WEP Keys */

#define AEEWIFI_OPT_WEP_64BIT_KEY_0        (WIFI_OPT_SZ + 8)
#define AEEWIFI_OPT_WEP_64BIT_KEY_1        (WIFI_OPT_SZ + 9)
#define AEEWIFI_OPT_WEP_64BIT_KEY_2        (WIFI_OPT_SZ + 10)
#define AEEWIFI_OPT_WEP_64BIT_KEY_3        (WIFI_OPT_SZ + 11)
 /* AECHAR Null terminated 10 character hexadecimal string */
 /* 4 distinct keys used for WEP 64-bit encryption.        */

#define AEEWIFI_OPT_WEP_128BIT_KEY_ID      (WIFI_OPT_32BIT + 18)
 /* Integer Value: Used to identify one of multiple 128-bit WEP Keys  */

#define AEEWIFI_OPT_WEP_128BIT_KEY_0        (WIFI_OPT_SZ + 12)
#define AEEWIFI_OPT_WEP_128BIT_KEY_1        (WIFI_OPT_SZ + 13)
#define AEEWIFI_OPT_WEP_128BIT_KEY_2        (WIFI_OPT_SZ + 14)
#define AEEWIFI_OPT_WEP_128BIT_KEY_3        (WIFI_OPT_SZ + 15)
 /* AECHAR Null terminated 26 character hexadecimal string. */
 /* 4 distinct keys used for WEP 128-bit encryption.       */

#define AEEWIFI_OPT_8021X_ID       (WIFI_OPT_SZ + 2)
  /* AECHAR Null Terminated String: Used for RADIUS based 802.1X procedures */

#define AEEWIFI_OPT_8021X_PASSWORD (WIFI_OPT_SZ + 3)
  /* AECHAR Null Terminated String: Used for RADIUS based 802.1X procedures */

#define AEEWIFI_OPT_LISTEN_INTERVAL   (WIFI_OPT_32BIT + 10)
  /* Integer Value: Listen interval, specified as Beacon intervals-Default 10 */

#define AEEWIFI_OPT_PREAMBLE_TYPE     (WIFI_OPT_32BIT + 11)
  /* AEEWIFIPreambleOption:  Specify Preamble length */ 

#define AEEWIFI_OPT_RTSTHRESHOLD      (WIFI_OPT_32BIT + 12)
  /* Integer Value: Value of 0 means RTS/CTS always occurs,    */
  /*                a value greater than 2347 inhibits RTS/CTS */

#define AEEWIFI_OPT_SCANTIMING     (WIFI_OPT_32BIT + 13)
  /* Integer Value: Scan Timing defined in units of Timing Unit - Default 120 */

#define AEEWIFI_OPT_POWERLEVEL     (WIFI_OPT_32BIT + 19)
  /* Integer Value: Specify the maximum transmit power level for the WIFI hardware */
  /*                A larger value gives a larger WLAN coverage */
  /*                The valid range of values is implementation dependent */

#define AEEWIFI_OPT_FRAGMENTATIONTHRESHOLD (WIFI_OPT_32BIT + 14)
  /* Integer Value: Fragmentation threshold - Default 2346 */

#define AEEWIFI_OPT_RSSI_THRESHOLD (WIFI_OPT_32BIT + 20)
  /* Integer Value: Threshold used to determine acceptable link quality */

#define AEEWIFI_OPT_MTU_SIZE (WIFI_OPT_32BIT + 21)
  /* Integer Value: Maximum Transmission Unit; determines the largest packet size */
  /*                that the network can handle - Default 1500                    */

#define AEEWIFI_OPT_DHCP_ENABLED   (WIFI_OPT_32BIT + 15)
  /* AEEWIFIDHCPOption:  Whether the IP address is acquired using DHCP.  */

#define AEEWIFI_OPT_IP_TYPE        (WIFI_OPT_32BIT + 16)
  /* AEEWIFIIPTypeOption: Whether to use IPv4/IPv6 */

#define AEEWIFI_OPT_STATIC_IP      (WIFI_OPT_SZ + 4)
  /* AECHAR Null Terminated String: Specifies the static IP address to use */

#define AEEWIFI_OPT_DNS_ADDRESS    (WIFI_OPT_SZ + 5)
  /* AECHAR Null Terminated String: Specifies the primary DNS address to use */

#define AEEWIFI_OPT_SUBNET_MASK    (WIFI_OPT_SZ + 6)
  /* AECHAR Null Terminated String: Specifies the Subnet Mask to use */

#define AEEWIFI_OPT_DEFAULT_GATEWAY (WIFI_OPT_SZ + 7)
  /* AECHAR Null Terminated String: Specifies the Default Gateway to use */

#define AEEWIFI_OPT_SECONDARY_DNS_ADDRESS    (WIFI_OPT_SZ + 16)
  /* AECHAR Null Terminated String: Specifies the secondary DNS address to use */

#define AEEWIFI_OPT_DOMAIN_NAME     (WIFI_OPT_SZ + 17)
  /* AECHAR Null Terminated String: Name to identify a network or a bunch of IP addresses */
  /*                                max 59 characters                         */

/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/

/* See data structure documentation */
typedef WebOpt WIFIOpt;

typedef struct AEEWIFIStatistics  {
  uint32 dwSize;
  uint32 tx_bytes;
  uint32 rx_bytes;
} AEEWIFIStatistics;

/*===========================================================================

                       IWIFI Interface

===========================================================================*/

typedef struct IWIFI IWIFI;
typedef struct IWIFIOpts IWIFIOpts;

AEEINTERFACE(IWIFI)
{
  INHERIT_IQueryInterface(IWIFI);

  /* WIFI Functions */
  int (*LoadOptions)   (IWIFI *po, uint32 id, IWIFIOpts *options);
  int (*CommitOptions) (IWIFI *po, uint32 id, IWIFIOpts *options);
  int (*GetStatistics) (IWIFI *po, AEEWIFIStatistics *statistics);
  int (*EnumProfileInit) (IWIFI *po);
  int (*EnumNextProfile) (IWIFI *po, uint32 *id);
};

AEEINTERFACE(IWIFIOpts)
{
  INHERIT_IQueryInterface(IWIFIOpts);

  int (*AddOpt)(IWIFIOpts *po, WIFIOpt *pOpt);
  int (*RemoveOpt)(IWIFIOpts *po, int32 nOptId);
  int (*GetOpt)(IWIFIOpts *po, int32 nOptId, WIFIOpt *pOpt);
};

/* Definition for the IWIFI object. */
#define IWIFI_AddRef(p)                                                     \
                AEEGETPVTBL((p),IWIFI)->AddRef((p))
#define IWIFI_Release(p)                                                    \
                AEEGETPVTBL((p),IWIFI)->Release((p))
#define IWIFI_QueryInterface(p, clsid, pp)                                  \
                AEEGETPVTBL((p),IWIFI)->QueryInterface((p), (clsid), (pp))
#define IWIFI_LoadOptions(p, id, options)                                   \
                AEEGETPVTBL((p),IWIFI)->LoadOptions((p), (id), (options))
#define IWIFI_CommitOptions(p, id, options)                                 \
                AEEGETPVTBL((p),IWIFI)->CommitOptions((p), (id), (options))
#define IWIFI_GetStatistics(p, stats)                                       \
                AEEGETPVTBL((p),IWIFI)->GetStatistics((p), (stats))
#define IWIFI_EnumProfileInit(p)                                       \
                AEEGETPVTBL((p),IWIFI)->EnumProfileInit((p))
#define IWIFI_EnumNextProfile(p, i)                                       \
                AEEGETPVTBL((p),IWIFI)->EnumNextProfile((p), (i))

/* Definition for the IWIFIOptions object. */
#define IWIFIOpts_AddRef(p)                                                   \
                AEEGETPVTBL((p),IWIFIOpts)->AddRef((p))
#define IWIFIOpts_Release(p)                                                  \
                AEEGETPVTBL((p),IWIFIOpts)->Release((p))
#define IWIFIOpts_QueryInterface(p, clsid, ppo)                               \
                AEEGETPVTBL((p),IWIFIOpts)->QueryInterface((p), (clsid), (ppo))
#define IWIFIOpts_AddOpt(p, o)                                                \
                AEEGETPVTBL((p),IWIFIOpts)->AddOpt((p), (o))
#define IWIFIOpts_RemoveOpt(p, i)                                             \
                AEEGETPVTBL((p),IWIFIOpts)->RemoveOpt((p), (i))
#define IWIFIOpts_GetOpt(p, i, o)                                             \
                AEEGETPVTBL((p),IWIFIOpts)->GetOpt((p), (i), (o))

/*===========================================================================

                       INTERFACES DOCUMENTATION

===========================================================================*/


/*---------------------------------------------------------------------------
                          Wireless LAN FUNCTIONS
---------------------------------------------------------------------------*/


/*======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 
IWIFI Interface

IWIFI interface is used by applications to retrieve and store WiFi configurations.
Each WiFi configuration consist of a set of options (AEEWIFI_OPT_*) and is controlled by
its profile ID (AEEWIFI_OPT_PROFILE_ID).

Use of these APIs will expect that the caller has AEECLSID_WIFI privilege.

The following header file is required:~
AEEWIFI.h
======================================================================= 

FUNCTION: IWIFI_LoadOptions

DESCRIPTION:  
  Loads the options for the given profile id into an IWIFIOpts object. 
  The profile currently in use has profile id 0. Other ids may be used as a data store.
  To make a profile id, other than profile id 0, the current profile, copy the IWIFIOpts 
  contents for that profile id to a new IWIFIOpts object and call CommitOptions() with
  profile id 0 with the copy of IWIFIOpts.

  int IWIFI_LoadOptions (IWIFI *po, uint32 id, IWIFIOpts *options);

PROTOTYPE:
  int IWIFI_LoadOptions (IWIFI *po,
                         uint32 id,
			 IWIFIOpts *options);

PARAMETERS:
  po                [in]: IWIFI interface pointer.
  id                [in]: Profile ID to get.
  options       [in/out]: Options associated with the given ID.

RETURN VALUE: 
  AEE_SUCCESS:  If a set of options was found for the given ID.
  ENOSUCH:      If a set of options with the given ID does not exist.
  EBADPARM:     If ID or options are out of range.
  ENOMEMORY     If there is not enough heap memory to complete the operation.
  EUNSUPPORTED: Used if we do not support multiple profiles.
  EFAILED:      Otherwise.

COMMENTS:
  When the user requests the given options, this method will first
  empty the current options variable and then load it with the current
  set of options.  ID 0 indicates the profile which is currently being
  used.

  Depending on the privilege level of the application, some options
  related to WIFI security will not be returned.

SIDE EFFECTS:
   None

SEE ALSO:
   None

=============================================================================
FUNCTION: IWIFI_CommitOptions

DESCRIPTION:  
  Stores the options for a given profile. Options not present will be
  unchanged.  Storing with id 0 causes the currently active WIFI parameters 
  to change.

PROTOTYPE:
  int IWIFI_CommitOptions(IWIFI *po,
                          uint32 id,
			  IWIFIOpts *options);

PARAMETERS:
  po                [in]: IWIFI interface pointer.
  id                [in]: Profile ID to get.
  options           [in]: Options associated with the given ID.

RETURN VALUE: 
  AEE_SUCCESS:  If the options were saved succesfully. 
  EBADPARM:     If ID or options are out of range.
  EUNSUPPORTED: Used if we do not support multiple profiles.
  ENOTALLOWED:  Application does not have permission to change one of 
                options present.
  EFAILED:      Otherwise.

COMMENTS:
   None

SIDE EFFECTS:
   None

SEE ALSO:
   None

=============================================================================
FUNCTION: IWIFI_GetStatistics

DESCRIPTION:  
  Gets the current Statistics for 802.11 activity.

PROTOTYPE:

  int IWIFI_GetStatistics (IWIFI *po, AEEWIFIStatistics *statistics)

PARAMETERS:
  po                [in]: IWIFI interface pointer.
  statistics    [in/out]: Buffer to be filled in with the statistics.

RETURN VALUE: 
  AEE_SUCCESS:  If the statictics buffer was filled.
  EBADPARM:     If the statistics buffer is not provided.
  EFAILED:      Unspecified failure.

COMMENTS:
   None  

SIDE EFFECTS:
   None

SEE ALSO:
   None

=============================================================================
FUNCTION: IWIFI_EnumProfileInit()

DESCRIPTION:
    Resets the IWIFI’s internal profile enumeration.

PROTOTYPE:
    int WIFI_EnumProfiletInit(IWIFI *po)

PARAMETERS:
    po            [in]: IWIFI interface pointer.

RETURN VALUE:
    AEE_SUCCESS:  If enumeration of profiles is allowed.
    EFAILED:      Unspecified internal failure.

COMMENTS:
    This call is used in conjunction with IWIFI_EnumNextProfile(), as
    shown in the following example:
===pre>

    if ( AEE_SUCCESS == IWIFI_EnumProfileInit( pWifi ))
    {
       while ( AEE_SUCCESS == IWIFI_EnumNextProfile( pWifi, &ni ))
       { process... }
    }
===/pre>

SIDE EFFECTS:
    None

SEE ALSO:
    IWIFI_EnumNextProfile()

=======================================================================

FUNCTION: IWIFI_EnumNextProfile()

DESCRIPTION:
    Returns next stored profile ID if any.

PROTOTYPE:
    int IWIFI_EnumNextProfile(IWIFI *po, uint32 *id)

PARAMETERS:
    po           [in]: IWIFI interface pointer.
    id           [in/out]: Pointer to profile id

RETURN VALUE:
    AEE_SUCCESS:  Valid profile id was returned.
    EBADPARM:     If any of the parameters are NULL.
    EFAILED:      All profile ids have been returned in previous calls
                  or an error occured.

COMMENTS:
    This call is used in conjunction with IWIFI_EnumProfileInit()

SIDE EFFECTS:
    None

SEE ALSO:
    IWIFI_EnumProfileInit()

======================================================================= 

======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 
IWIFIOpts Interface

Description:


IWIFIOpts interface is used by application to examine and change
individual WiFi options in a configuration identified by its profile ID.

The typical usage of these interfaces is as follows:

 1) Create an IWIFI and IWIFIOpts objects.
 2) Call IWIF_LoadOptions() to populate IWIFIOpts object from permanent
storage.
 3) Call various IWIFIOpts functions to manipulate options values
locally.
 4) Call IWIFI_CommitOptions() to save the option values in IWIFIOpts
object.

The following header file is required:~
AEEWIFI.h
===================================================================================================================================================
FUNCTION: IWIFIOpts_AddOpt

DESCRIPTION:  
  Adds an ID-Value pair to IWIFIOpts. Only one value is allowed for each
  ID except for WEPKEY's which may contain up to 4.

PROTOTYPE:
  int IWIFIOpts_AddOpt(IWIFIOpts *po, WIFIOpt *pOpt)

PARAMETERS:
  po                [in]: IWIFIOpts interface pointer.
  pOpt              [in]: WIFI option

RETURN VALUE:
  SUCCESS:   If addition was successful.
  ENOMEMORY: If enough memory couldn't be allocated
  EBADPARM:  If any of the input parameters is NULL.

COMMENTS:
   None
  
SIDE EFFECTS:
   None

SEE ALSO:
  IWEBOPTS_AddOpt()

=============================================================================
FUNCTION: IWIFIOpts_RemoveOpt

DESCRIPTION:  
  Remove the ID-value pair matching the specified ID. 

PROTOTYPE:
  boolean IWIFIOpts_RemoveOpt(IWIFIOpts *po, int nOptId)

PARAMETERS:
  po                [in]: IWIFIOpts interface pointer.
  nOptId            [in]: Option ID.

RETURN VALUE:
  SUCCESS: If remove was successful
  EFAILED: If ID was not found

COMMENTS:
  Corresponds to IWEBOPTS_RemoveOpt() with index 0.

SIDE EFFECTS:
   None

SEE ALSO:
  IWEBOPTS_RemoveOpt()
  
=============================================================================
FUNCTION: IWIFIOpts_GetOpt

DESCRIPTION:  
  Find the ID-value pair matching the specified ID. 

PROTOTYPE:
  int IWIFIOpts_GetOpt(IWIFIOpts *po, int32 nOptId, WIFIOpt *pOpt)

PARAMETERS:
  po                [in]: IWIFIOpts interface pointer.
  nOptId            [in]: Option ID to search for.
  pOpt          [in/out]: Reference to WIFIOpt where matching call option
                          is copied.

RETURN VALUE:
  SUCCESS: If a matching call option is found.
  EFAILED: Otherwise

COMMENTS:
   Caller must deallocate memory returned in pOpt.pVal for the following options:
     AEEWIFI_OPT_SSID
     AEEWIFI_OPT_WEP_KEY
     AEEWIFI_OPT_WEP_64BIT_KEY_0
     AEEWIFI_OPT_WEP_64BIT_KEY_1
     AEEWIFI_OPT_WEP_64BIT_KEY_2
     AEEWIFI_OPT_WEP_64BIT_KEY_3
     AEEWIFI_OPT_WEP_128BIT_KEY_0
     AEEWIFI_OPT_WEP_128BIT_KEY_1
     AEEWIFI_OPT_WEP_128BIT_KEY_2
     AEEWIFI_OPT_WEP_128BIT_KEY_3
     AEEWIFI_OPT_8021X_PASSWORD
     AEEWIFI_OPT_8021X_ID
     AEEWIFI_OPT_STATIC_IP
     AEEWIFI_OPT_DNS_ADDRESS
     AEEWIFI_OPT_SUBNET_MASK
     AEEWIFI_OPT_DEFAULT_GATEWAY
     AEEWIFI_OPT_SECONDARY_DNS_ADDRESS
     AEEWIFI_OPT_DOMAIN_NAME

SIDE EFFECTS:
   None

SEE ALSO:
  IWEBOPTS_GetOpt()

=============================================================================*/
#endif /* AEEWIFI_H */
