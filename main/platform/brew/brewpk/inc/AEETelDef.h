#ifndef AEETELDEF_H
#define AEETELDEF_H
/*======================================================
FILE:  AEETelDef.h

SERVICES:

GENERAL DESCRIPTION:
   Definitions for constants and types used by AEETelephone.h

        Copyright (c) 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            Qualcomm Confidential and Proprietary
=====================================================*/
#include "AEEDateTime.h"
#include "AEEIModel.h"
#include "AEEStdDef.h"
#include "AEETSuppsDef.h"
/*===========================================================================
                         C O N S T A N T S
===========================================================================*/
/* An undefined or uninitialized call descriptor */
#define AEET_CALL_DESC_UNDEF    (-1)

/* Maximum length of digits in a number */
#define AEET_MAX_DIGITS_LENGTH    65
/* Maximum number of forwarded info stored */
#define AEET_MAX_NUM_FWDING 5
/* Maximum length of subaddress digits */
#define AEET_MAX_SUBADDRESS_DIGITS_LENGTH   22
/* Maximum length of alphanumeric tag */
#define AEET_MAX_ALPHA_TAG_LENGTH  65
/*
** Define constants used by for mobility management information.
** According to the standard, 3GPP TS 24.0008, the short name and
** full name can be from 3 to N characters in length.
*/
#define AEET_MAX_NW_NAME_CHARS       255
/* Maximum length of user data - as defined in the standard 3GPP TS22.087 */
#define AEET_MAX_USER_DATA_LENGTH    128

/*
** Constant for the maximum number of PLMNS in the PLMN list.  The maximum
** length for the sys_plmn_list_s_type is twice the sum of the maximum lengths
** for the user and operator preferred PLMN lists since sys_plmn_list_s_type
** must contain a separate entry for each radio access technology.
*/
#define AEET_USER_PPLMN_LIST_MAX_LENGTH      10
  /* Maximum number of user preferred/controlled PLMNs */

#define AEET_OPERATOR_PPLMN_LIST_MAX_LENGTH  10
  /* Maximum number of operator preferred/controlled PLMNs */

#define AEET_PLMN_LIST_MAX_LENGTH  ( 2 * ( AEET_USER_PPLMN_LIST_MAX_LENGTH + \
                                          AEET_OPERATOR_PPLMN_LIST_MAX_LENGTH ) )
/* Maximum length of DTMF digits */
#define AEET_INBAND_MAX_DIGITS_LENGTH  257 

#define AEET_BM(val) (1 << (val))

/*===========================================================================
                         T Y P E S
===========================================================================*/

/* Call descriptor uniquely identifies phone call in the system. */
typedef int AEECallDesc;

/* Structure representing multiple calls */
typedef struct {
   uint32      dwCount;
   AEECallDesc calls[1]; 
} AEETCalls;

/*
** The time zone is an offset from Universal time (i.e. difference between 
** local time and Universal time) in increments of 15 minutes.  
*/
typedef int AEETTimeZone;

/*
** Define a structure for the Time Zone and Universal Time mobility
** management information. 
*/
typedef struct 
{
  AEEDateTime                              time;
  AEETTimeZone                             time_zone;
} AEETTimeAndZone;

/* Enumeration of service types for call origination request.
** 
** To specify a service type while making a call ICallOpts should 
** be used. Default service type is AEET_SRV_TYPE_AUTOMATIC.
** 
*/
typedef enum
{

  AEET_SRV_TYPE_NONE = 0, 

  AEET_SRV_TYPE_AMPS=1,
    /* Call is to be connected over analog service.
    **
    ** Note that the client does NOT need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to ANALOG ONLY before origination and returned to
    ** its original setting at the end of the call */

  AEET_SRV_TYPE_HDR=2,
    /* Call is to be connected over HDR(1xEv) service. */

  AEET_SRV_TYPE_CDMA_SPECIFIC=3,
    /* Call is to be connected over CDMA service with specific client
    ** designated CDMA service option.
    **
    ** Note that the application need to specify service option when
    ** originating with this call type.
    **
    ** NOTE! When originating with this service type, the mode preference
    ** might be forced to DIGITAL ONLY before origination and returned to
    ** its original setting at the end of the call */

  AEET_SRV_TYPE_CDMA_AUTOMATIC=4,
    /* Call is to be connected over CDMA service where Call manager automatically
    ** originate with the best available service option for the specified
    ** call type.
    **
    ** Note that the application does NOT need to specify service option when
    ** originating with this call type */

  AEET_SRV_TYPE_AUTOMATIC=5,
    /* Call is to be connected over available service - AMPS or CDMA,
    ** or GSM or WCDMA or HDR.
    **
    ** Note that the application does NOT need to specify service option when
    ** originating with this srv type except for the packet data call*/

  AEET_SRV_TYPE_GSM_WCDMA=6,
    /* Call is to be connected over available service - GSM or WCDMA */

  AEET_SRV_TYPE_WCDMA=7,
     /* Call is to be connected over WCDMA service */

  AEET_SRV_TYPE_GSM=8,
     /* Call is to be connected over GSM service */

  AEET_SRV_TYPE_BIG = 0x10000000
    /* Forcing all enums to be 32 bit */

} AEETServiceType;

/*
** Enumeration of service status.  Indicates whether service is
** available or not available.
*/
typedef enum
{
  AEET_SRV_STATUS_NONE = -1,

  AEET_SRV_STATUS_NO_SRV,
    /* No service                             */

  AEET_SRV_STATUS_LIMITED,
    /* Limited service                        */

  AEET_SRV_STATUS_SRV,
    /* Service available                      */

  AEET_SRV_STATUS_LIMITED_REGIONAL,
    /* Limited regional service               */

  AEET_SRV_STATUS_PWR_SAVE,
    /* MS is in power save or deep sleep      */  

  AEET_SRV_STATUS_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSrvStatus;

/*
** The following enumerated type defines type of service.
*/
typedef enum
{
  AEET_SRV_DOMAIN_NONE = -1,

  AEET_SRV_DOMAIN_NO_SRV,
    /* No service                             */

  AEET_SRV_DOMAIN_CS_ONLY,
    /* Circuit Switched Only Capable          */

  AEET_SRV_DOMAIN_PS_ONLY,
    /* Packet Switched Only Capable           */

  AEET_SRV_DOMAIN_CS_PS,
  /* Circuit and Packet Switched Capable    */

  AEET_SRV_DOMAIN_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSrvDomain;

/*
** The following enumerated type defines the service capabilities
** of a network.
*/
typedef enum
{
  AEET_SRV_CAPABILITY_NONE = -1,

  AEET_SRV_CAPABILITY_NOT_AVAILABLE,
    /* Network can not provide service                                   */

  AEET_SRV_CAPABILITY_CS_SERVICE_ONLY,
    /* Network is capable of providing Circuit Switched service          */

  AEET_SRV_CAPABILITY_PS_SERVICE_ONLY,
    /* Network is capable of providing Packet Switched service           */

  AEET_SRV_CAPABILITY_CS_PS_SERVICE,
  /* Network is capable of providing Circuit & Packet Switched service */

  AEET_SRV_CAPABILITY_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSrvCapability;


/*
** Enumeration of system modes.
*/
typedef enum
{
  AEET_SYS_MODE_NONE = -1,

  AEET_SYS_MODE_NO_SRV=0,
    /* No service                                        */

  AEET_SYS_MODE_AMPS=1,
    /* Analog Mobile Phone System (AMPS) mode             */

  AEET_SYS_MODE_CDMA=2,
    /* Code Division Multiple Access (CDMA) mode          */

  AEET_SYS_MODE_GSM=3,
    /* Global System for Mobile communications (GSM) mode */

  AEET_SYS_MODE_HDR=4,
    /* High Data Rate (HDR) mode                          */

  AEET_SYS_MODE_WCDMA=5,
    /* Wideband CDMA (WCDMA) mode                         */

  AEET_SYS_MODE_GPS=6,
    /* Global Positioning System (GPS) mode               */

  AEET_SYS_MODE_WLAN=7,
    /* Wireless LAN mode                                  */

  AEET_SYS_MODE_GW = 0x01072617,
    /* GSM and WCDMA mode                                 */
 
  AEET_SYS_MODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSysMode;

/*
** Enumeration of roaming statuses.  The values for these enumerates
** were derived from the roaming display indicators described in
** 3GPP2 C.R1001-A.  Please note that the actual value of the enumerate
** is not consistent with the value listed in the standard (e.g. the
** value of AEET_ROAM_STATUS_OFF and AEET_ROAM_STATUS_ON are reversed from
** the standard.
*/
typedef enum
{

  AEET_ROAM_STATUS_NONE = -1,

  AEET_ROAM_STATUS_OFF,
    /* Roaming Indicator off                       */

  AEET_ROAM_STATUS_ON,
    /* Roaming Indicator on                        */

  AEET_ROAM_STATUS_BLINK,
    /* Roaming Indicator flashing                  */

  AEET_ROAM_STATUS_OUT_OF_NEIGHBORHOOD,
    /* Out of neighborhood                         */

  AEET_ROAM_STATUS_OUT_OF_BLDG,
    /* Out of building                             */

  AEET_ROAM_STATUS_PREF_SYS,
    /* Roaming - preferred system                  */

  AEET_ROAM_STATUS_AVAIL_SYS,
    /* Roaming - available system                  */

  AEET_ROAM_STATUS_ALLIANCE_PARTNER,
    /* Roaming - alliance partner                  */

  AEET_ROAM_STATUS_PREMIUM_PARTNER,
    /* Roaming - premium partner                   */

  AEET_ROAM_STATUS_FULL_SVC,
    /* Roaming - full service functionality        */

  AEET_ROAM_STATUS_PARTIAL_SVC,
    /* Roaming - partial service functionality     */

  AEET_ROAM_STATUS_BANNER_ON,
    /* Roaming banner on                           */

  AEET_ROAM_STATUS_BANNER_OFF,
    /* Roaming banner off                          */

  AEET_ROAM_STATUS_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETRoamStatus;

/*
** Define a enumerated type that indicates the type of system ID.
*/
typedef enum
{

  AEET_SYS_ID_TYPE_UNDEFINED,
    /* System ID is undefined     */

  AEET_SYS_ID_TYPE_UMTS,
    /* UMTS (WCDMA/GSM) system ID */

  AEET_SYS_ID_TYPE_IS95,
    /* IS-95 system ID            */

  AEET_SYS_ID_TYPE_IS856,
    /* IS-856 (HDR) system ID     */

  AEET_SYS_ID_TYPE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSysIDType;

/*
** Define a structure that identifies an IS-95 system.
**/
typedef struct 
{
  uint16         sid;
       /* The system ID */

  uint16         nid;
       /* The network ID */

} AEETIS95SysID;

/*
** Define a structure that identifies a Public Land Mobile Network (PLMN).
** A PLMN ID is defined within a GSM network to be the combination of a Mobile
** Country Code (MCC) and Mobile Network Code (MNC).  A PLMN ID is stored in 3
** octets as specified in 3G TS 24.008 Section 10.5.1.3 and shown below:
**                                        Bit
**                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
**                 |===============================================|
**         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
**                 |-----------------------------------------------|
**         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
**                 |-----------------------------------------------|
**         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
**                 |===============================================|
**
** The criteria for comparing one PLMN against another is specified in
** 3G TS 23.122 Annex A.
**/
typedef struct 
{
  byte     identity[3];
} AEETPLMNID;

/*
** Type for system ID.
*/
typedef union 
{
  AEETIS95SysID  is95;
    /* IS-95 system ID             */

  byte                    is856[16];
    /* IS-856 system ID            */

  AEETPLMNID      plmn;
    /* UMTS (WCDMA/GSM) system ID  */

} AEETSysIDData;

typedef struct 
{
  AEETSysIDType  id_type;
    /* System identifier type */

  AEETSysIDData  id;
    /* The system identifier */

} AEETSysID;

/*
** The following enumerated type defines the state of the
** Subscriber Identfication Module (SIM) card.
*/
typedef enum
{
  AEET_SIM_STATE_NONE = -1,

  AEET_SIM_STATE_NOT_AVAILABLE,
    /* The SIM is not available               */

  AEET_SIM_STATE_AVAILABLE,
    /* The SIM is available                   */

  AEET_SIM_STATE_CS_INVALID,
    /* The SIM has been marked by the network */
    /* as invalid for CS services             */

  AEET_SIM_STATE_PS_INVALID,
    /* The SIM has been marked by the network */
    /* as invalid for PS services             */

  AEET_SIM_STATE_CS_PS_INVALID,
    /* The SIM has been marked by the network */
    /* as invalid for CS and PS services      */

  AEET_SIM_STATE_BIG = 0x10000000
    /* To force enum to 32 bit */

} AEETSIMState;

/*
** Define a type for the Network Name Coding Scheme.  The format is
** specified by 3GPP TS 24.008 and 03.38.
*/
typedef enum
{
  AEET_NW_NAME_CODING_SCHEME_NONE                             = -1,

  AEET_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET  =  0,
    /* Cell Broadcast data coding, default alphabet, language unspecified */

  AEET_NW_NAME_CODING_SCHEME_UCS2                             =  1,
    /* UCS2 coding scheme */

  AEET_NW_NAME_CODING_SCHEME_RESERVED                         =  2,
    /* Reserved */

  AEET_NW_NAME_CODING_SCHEME_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETNWCodingType;

/*------------------------------------------------------------------------------
   Mobility Management Info Data Types
------------------------------------------------------------------------------*/
/*
** Define types for the network's short name and full name.
**/
typedef struct 
{
  uint8                        length;
    /* Number of characters in the full name */

  AEETNWCodingType            coding_scheme;
  /* The coding scheme used to encode the full name of the network */

  boolean                      add_country_initials;
    /* Flag that indicates whether MS should add the country initials to
       the full name of the network */

  uint8                        name[AEET_MAX_NW_NAME_CHARS];
    /* The full name of the network          */
} AEETNWName;


/*
** Define a type for the Localized Service Area identity.  The format
** is specified by 3GPP TS 24.008 and 23.003.
*/
typedef enum
{

  AEET_LSA_ID_TYPE_NONE         = -1,  
  AEET_LSA_ID_TYPE_PLMN_SIG_NUM =  0,             /* The LSA is a PLMN      */
                                                 /* significant number     */
  AEET_LSA_ID_TYPE_UNIVERSAL    =  1,             /* The LSA is a universal */
  AEET_LSA_ID_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETLSAIdentityType;

/*
** Define a type for the Localized Service Area identity.  The format
** is specified by 3GPP TS 24.008 and 23.003.
*/
typedef struct 
{
  AEETLSAIdentityType              type;
  uint8                            identity[3];
} AEETLSAIdentity;


/*
** Define a structure that contains the currently registered network's
** mobility management information (i.e. network's name, date, time and
** time zone).  All fields in this structure are optional.  If the
** "available" flag is true, then the corresponding data field contains
** valid information.  If the "available" flag is false, then the
** corresponding data field should be ignored.
*/
typedef struct 
{
  /*
  ** Flags that indicate whether the particular mm information is
  ** available.
  **/
  unsigned                           plmn_avail:1;
  unsigned                           full_name_avail:1;
  unsigned                           short_name_avail:1;
  unsigned                           time_and_time_zone_avail:1;
  unsigned                           time_zone_avail:1;
  unsigned                           lsa_identity_avail:1;
  unsigned                           daylight_saving_adj:2;

  AEETPLMNID                       plmn;
  AEETNWName                       full_name;
  AEETNWName                       short_name;

  AEETTimeAndZone                  time_and_time_zone;
  AEETTimeZone                     time_zone;
  AEETLSAIdentity                  lsa_identity;
} AEETMMInfo;

/* OTASP activation code. For CDMA only.
*/
typedef enum
{
  AEET_OTASP_ACT_ABAND,
  /* A band */

  AEET_OTASP_ACT_BBAND,
  /* B band */

  AEET_OTASP_ACT_ABLOCK,
  /* A block */

  AEET_OTASP_ACT_BBLOCK,
  /* B block */

  AEET_OTASP_ACT_CBLOCK,
  /* C block */

  AEET_OTASP_ACT_DBLOCK,
  /* D block */

  AEET_OTASP_ACT_EBLOCK,
  /* E block */

  AEET_OTASP_ACT_FBLOCK,
  /* F block */

  AEET_OTASP_ACT_PREFERRED,
  /* Preferred OTASP */

  AEET_ACTIVATECODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETOTASPActivationCode;

/*
** Enumeration of system band classes.
*/
typedef enum
{
  AEET_SYS_BAND_CLASS_NONE        = -1,

  AEET_SYS_BAND_BC0               = 0,
    /* Band Class 0 - US Cellular band (800 MHz)       */

  AEET_SYS_BAND_BC1               = 1,
    /* Band Class 1 - US */

  AEET_SYS_BAND_BC3               = 3,
    /* Band Class 3 - Japanese Cellular band (800 MHz) */

  AEET_SYS_BAND_BC4               = 4,
    /* Band Class 4 - Korean PCS band (1800 MHz)       */

  AEET_SYS_BAND_BC5               = 5,
    /* Band Class 5 (450 MHz)                          */

  AEET_SYS_BAND_BC6               = 6,
    /* Band Class 6 (2 GHz)                            */

  AEET_SYS_BAND_BC7               = 7,
    /* Band Class 7 (700 MHz)                          */

  AEET_SYS_BAND_BC8               = 8,
    /* Band Class 8 (1800 MHz)                         */

  AEET_SYS_BAND_BC9               = 9,
    /* Band Class 9 (900 MHz)                          */

  AEET_SYS_BAND_BC10              = 10,
    /* Band Class 10 (Second 800 MHz)                  */

  AEET_SYS_BAND_BC11              = 11,
    /* Band Class 11 - European PAMR Band (400 MHz)    */

    /* Reserved 12-39 for CDMA band classes. */

  AEET_SYS_BAND_GSM_450           = 40,
    /* GSM 450 band (450 MHz)                          */

  AEET_SYS_BAND_GSM_480           = 41,
    /* GSM 480 band (480 MHz)                          */

  AEET_SYS_BAND_GSM_750           = 42,
    /* GSM 750 band (750 MHz)                          */

  AEET_SYS_BAND_GSM_850           = 43,
    /* GSM 850 band (850 MHz)                          */

  AEET_SYS_BAND_GSM_EGSM_900      = 44,
    /* GSM Extended GSM (E-GSM) 900 band (900 MHz)         */

  AEET_SYS_BAND_GSM_PGSM_900      = 45,
    /* GSM Primary GSM ( P-GSM) 900 band (900 MHz)         */

  AEET_SYS_BAND_GSM_RGSM_900      = 46,
    /* GSM Railways GSM ( R-GSM) 900 band (900 MHz)        */

  AEET_SYS_BAND_GSM_DCS_1800      = 47,
    /* GSM DCS band (1800 MHz)                         */

  AEET_SYS_BAND_GSM_PCS_1900      = 48,
    /* GSM PCS band (1900 Mhz)                         */

    /* Reserved 49-79 for GSM band classes. */

  AEET_SYS_BAND_WCDMA_I_IMT_2000  = 80,
    /* WCDMA IMT 2000 (2000 MHz)                       */

  AEET_SYS_BAND_WCDMA_II_PCS_1900 = 81,
    /* WCDMA PCS band (1900 MHz)                       */

  AEET_SYS_BAND_WCDMA_III_1700    = 82,
    /* WCDMA 1700 (1700 MHz)                           */

  AEET_SYS_BAND_WCDMA_IV_1700     = 83,
    /* WCDMA 1700 (1700 MHz)                           */

  AEET_SYS_BAND_WCDMA_V_850       = 84,
    /* WCDMA 850 (850 MHz)                             */

  AEET_SYS_BAND_WCDMA_VI_800      = 85,
    /* WCDMA 800 (800 MHz)                             */

  AEET_SYS_BAND_WCDMA_VII_2600    = 86,
    /* WCDMA 2600 (2600 MHz)                           */

  AEET_SYS_BAND_WCDMA_VIII_900    = 87,
    /* WCDMA 900 (900 MHz)                             */

  AEET_SYS_BAND_WCDMA_IX_1700     = 88,
    /* WCDMA 1700 (1700 MHz)                           */


  AEET_SYS_BAND_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSysBandClass;

/*
** Enumeration of PCS frequency blocks/Cellular systems.
*/
typedef enum
{
  AEET_SYS_BLKSYS_NONE = -1,

  AEET_SYS_BLKSYS_PCS_A,
    /* PCS block A                            */

  AEET_SYS_BLKSYS_PCS_B,
    /* PCS block B                            */

  AEET_SYS_BLKSYS_PCS_C,
    /* PCS block C                            */

  AEET_SYS_BLKSYS_PCS_D,
    /* PCS block D                            */

  AEET_SYS_BLKSYS_PCS_E,
    /* PCS block E                            */

  AEET_SYS_BLKSYS_PCS_F,
    /* PCS block F                            */

  AEET_SYS_BLKSYS_PCS_CELL_A,
    /* Cellular system A                      */

  AEET_SYS_BLKSYS_PCS_CELL_B,
    /* Cellular system B                      */

  AEET_SYS_BLKSYS_PCS_G,
    /* PCS E-Block                            */

  AEET_SYS_BLKSYS_PCS_H,
    /* PCS F-Block                            */

  AEET_SYS_BLKSYS_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSysBlocks;


/* Maximum user zone name length.
*/
#define  AEET_PUZL_MAX_UZ_NAME_LEN          32

/*
** structure to hold user zone name information.
*/

typedef struct
{
  byte     uz_name_encoding;

  byte     uz_name_n_octets;

  byte     uz_name[AEET_PUZL_MAX_UZ_NAME_LEN];
} AEETUZName;

/*---------------------------------------------------------------------------
 * AEE Telephone Serving System Information
 *--------------------------------------------------------------------------*/
typedef struct {

  AEETSrvStatus            srv_status;
  /* The service status (SERVICE/NO SERVICE) of the system. */  

  AEETSrvDomain            srv_domain;
  /* The service domain registered on the system. */

  AEETSrvCapability        srv_capability;
  /* The system's service capability.*/

  AEETSysMode              sys_mode;
  /* The system's mode. */

  AEETRoamStatus           roam_status;
  /* Indicate current roaming status */

  AEETSysID                sys_id;
  /* The ID of the selected system. */

  uint16                    rssi;
    /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
    ** AMPS:      -110 to -89 dBm
    ** 800 CDMA:  -105 to -90 dBm
    ** 1900 CDMA: -108 to -93 dBm
    ** GSM/WCDMA: -105 to -60 dBm
    */

  boolean                   hdr_hybrid;
    /* system is hybrid or not. FEATURE_HDR_HYBRID */
 
  uint8                    tssinfo_version;


  AEETSrvStatus            hdr_srv_status;
     /* Indicate current service state - FEATURE_HDR_HYBRID */

  AEETRoamStatus           hdr_roam_status;
     /* Indicate current hybrid hdr roaming status - FEATURE_HDR_HYBRID */

  uint16                    hdr_rssi;
    /* HDR RSSI in positive dBm i.e. 75 means -75dBm - FEATURE_HDR_HYBRID
    ** The range is -105 to -90 dBm */  


  uint16                   wlan_rssi;

  AEETSIMState             sim_state;
    /* Current state of the SIM */

  boolean                   plmn_forbidden;
  /* Indicates whether the PLMN is forbidden */

  AEETMMInfo               mm_information;
  /* Mobility management information, i.e. network name, date,
  ** time and time zone of registered network */

  AEETSysBandClass         band_class;
  /* cellular or PCS */

  AEETSysBlocks            block_or_system;
   /* PCS frequency block: A to F Cellular System A or B*/

  boolean                   is_registered;
  /* is mobile registered on the current system */

  int32                     uz_id;
  /* User zone Identification of the selected user zone */

  AEETUZName               uz_name;
  /* User zone name of the selected user zone. */

  boolean                  ccs_supported;
  /* is concurrent services supported. */

  uint8                    p_rev_in_use;
  /* 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev. */
} AEETSSInfo;

/* Enumeration of telephone call states.
*/
typedef enum
{

   AEET_CALL_STATE_NONE        = 0x00000000,

   AEET_CALL_STATE_IDLE        = 0x00000001,
   /* Call is in idle state - i.e. no call */

   AEET_CALL_STATE_ORIG        = 0x00000002,
   /* Call is in origination state */

   AEET_CALL_STATE_INCOM       = 0x00000004,
   /* Call is in alerting state */

   AEET_CALL_STATE_CONV        = 0x00000008,
   /* Call is in conversation state */

   AEET_CALL_STATE_ENDED       = 0x00000010,
   /* Call has already ended */

   AEET_CALL_STATE_ONHOLD      = 0x00000020,
   /* Call is on hold */

   AEET_CALL_STATE_DORMANT     = 0x00000040,
   /* Call is in dormant mode */

   AEET_CALL_STATE_MULTIPARTY  = 0x00000080,
   /* Call is in a Multiparty mode */
  
   AEET_CALL_STATE_BIG         = 0x10000000
    /* Force 32-bit enums */

} AEETCallState;

/* Enumeration of Telephone call types.
*/
typedef enum
{
   AEET_CALL_TYPE_NONE           = 0x00000000,

   AEET_CALL_TYPE_VOICE          = 0x00000001,
   /* Voice call type.
   **
   ** NOTE! Answering with this type is only successful if the
   ** incoming call is of type VOICE */

   AEET_CALL_TYPE_CS_DATA        = 0x00000002,
   /* CS Data call type - (for modem, fax, etc. calls).
   **
   ** NOTE! Answering with this call is only successful if the
   ** incoming call is of type DATA or VOICE (with AMPS service) */

   AEET_CALL_TYPE_PS_DATA        = 0x00000004,
   /* Packet Switched Data call type */

   AEET_CALL_TYPE_SMS            = 0x00000008,
   /* SMS call type (SMS SO-6 and SO-14). CDMA only
   **
   ** NOTE! Answering with this call type is only successful
   ** if incoming call is of type SMS */

   AEET_CALL_TYPE_PD             = 0x00000010,
   /* Position Determination call type (SO-35 and 36). CDMA only
   ** This call type used to exchange IS-801 */

   AEET_CALL_TYPE_TEST           = 0x00000020,
   /* Test call type (for Markov, Loopback, etc.). CDMA only
   **
   ** NOTE! Answering with this call type is only successful
   ** if incoming call is of type TEST */

   AEET_CALL_TYPE_OTAPA          = 0x00000040,
   /* OTAPA call type (OTAPA SO-18 and SO-19). CDMA only
   **
   ** NOTE! Answering with this call type is only successful
   ** if incoming call is of type OTAPA */
   
   /* This Call type is automatically assigned by system when
   ** detecting an origination dial-string satisfies a pre-programmed
   ** criteria defined by the operator.
   **
   ** NOTE! These call types should not be used to originate calls.
   */

  AEET_CALL_TYPE_STD_OTASP       = 0x00000080,
   /* Standard OTASP call type. CDMA only */
   
   /* This Call type is automatically assigned by system when
   ** detecting an origination dial-string satisfies a pre-programmed
   ** criteria defined by the operator.
   */

  AEET_CALL_TYPE_NON_STD_OTASP   = 0x00000100,
   /* None standard OTASP call type. CDMA only */
   
   /* This Call type is automatically assigned by system when
   ** detecting an origination dial-string satisfies a pre-programmed
   ** criteria defined by the operator.
   **
   ** NOTE! These call types should not be used to originate
   ** or answer calls.
   */

  AEET_CALL_TYPE_EMERGENCY       = 0x00000200,
   /* Emergency call type. */
   /* This Call type is automatically assigned by system when
   ** detecting an origination dial-string satisfies a pre-programmed
   ** criteria defined by the operator.
   */
   /* This call type can be used by Clients also to originate calls.
   **
   ** NOTE! If call manager assigned this call type, the mode
   ** preference is forced to EMERGENCY before origination. If at the
   ** end of the call the phone is not in EMERGENCY callback mode,
   ** the mode is automatically restored to its original selection.
   ** Else the EMERGENCY mode stays in effect until changed by a client
   ** or power-cycle */

  AEET_CALL_TYPE_VIDEO_TELEPHONE      = 0x00000800,
   /* Multimedia call */


  AEET_CALL_TYPE_BIG             = 0x10000000  /* Force 32-bit enums */

} AEETCallType;

/* Direction of Call */
typedef enum
{
  AEET_CALL_DIRECTION_NONE,      /* Undefined Direction */

  AEET_CALL_DIRECTION_MO,        /* Mobile Originated Call */

  AEET_CALL_DIRECTION_MT,        /* Mobile Terminated Call */

  AEET_CALL_DIRECTION_BIG = 0x10000000  /* A 32 bit place holder */

} AEETCallDirection;

/* Service Options */
typedef enum {
  AEET_SO_REJ=0x0000,
  AEET_SO_VOICE_IS96A=0x0001,
  AEET_SO_LOOPBACK=0x0002,
  AEET_SO_VOICE_EVRC=0x0003,
  AEET_SO_ASYNC_DATA_PRE707=0x0004,
  AEET_SO_G3_FAX_PRE707=0x0005,
  AEET_SO_SMS=0x0006,
  AEET_SO_PPP_PKT_DATA_PRE707=0x0007,
  AEET_SO_LOOPBACK_13K=0x0009,
  AEET_SO_ASYNC_DATA_13K_PRE707=0x000C,
  AEET_SO_G3_FAX_13K_PRE707=0x000D,
  AEET_SO_RS2_SMS=0x000E,
  AEET_SO_PPP_PKT_DATA_13K_PRE707=0x000F,
  AEET_SO_VOICE_13K_IS733=0X0011,
  AEET_SO_RS1_OTAPA=0x0012,
  AEET_SO_RS2_OTAPA=0x0013,
  AEET_SO_ANALOG_FAX_RS1=0x0014,
  AEET_SO_ANALOG_FAX_RS2=0x0015,
  AEET_SO_MDR_PKT_DATA_FRS1_RRS1=0x0016,
  AEET_SO_MDR_PKT_DATA_FRS1_RRS2=0x0017,
  AEET_SO_MDR_PKT_DATA_FRS2_RRS1=0x0018,
  AEET_SO_MDR_PKT_DATA_FRS2_RRS2=0x0019,
  AEET_SO_FULL_TDSO=0x0020,
  AEET_SO_PPP_PKT_DATA_3G=0x0021,
  AEET_SO_G3_FAX_13K_PROPTRY=0x8022,
  AEET_SO_RS1_PD=0x0023,
  AEET_SO_RS2_PD=0x0024,
  AEET_SO_MARKOV_SO54=0x0036,
  AEET_SO_LOOPBACK_SO55=0x0037,
  AEET_SO_VOICE_SMV=0x0038,
  AEET_SO_ASYNC_DATA_IS707=0x1004,
  AEET_SO_G3_FAX_IS707=0x1005,
  AEET_SO_PPP_PKT_DATA_IS707=0x1007,
  AEET_SO_VOICE_13K=0x8000,
  AEET_SO_VOICE_IS96=0x8001,
  AEET_SO_MARKOV=0x8002,
  AEET_SO_DATA=0x8003,
  AEET_SO_TDSO=0x8008,
  AEET_SO_MARKOV_13K=0x801C,
  AEET_SO_WLL_OFF_HOOK=0x801D,
  AEET_SO_RS1_MARKOV=0x801E,
  AEET_SO_RS2_MARKOV=0x801F,
  AEET_SO_PPP_PKT_DATA_13K_PROPTRY=0x8020,
  AEET_SO_ASYNC_DATA_13K_PROPTRY=0x8021,
  AEET_SO_MDR_DPT_FRS1_RRS1=0x8024,
  AEET_SO_MDR_DPT_FRS2_RRS2=0x8025,
  AEET_SO_NULL=0xFFFF,

  AEET_SO_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETServiceOption;

typedef enum
{
  AEET_CLI_UNAVAILABLE,
  AEET_CLI_REJECT_BY_USER,
  AEET_CLI_INTERACTION_WITH_OTHER_SRVC,
  AEET_CLI_COIN_PAY_PHONE,
  AEET_CLI_BIG = 0x10000000

} AEETNoCLICause;

/* CLIR Type */
typedef enum {
  AEET_CLIR_DEFAULT,
  AEET_CLIR_INVOCATION = 11,
  AEET_CLIR_SUPPRESSION = 12,
  AEET_CLIR_SUPPRESSION_REJECTED = 46,

  AEET_CLIR_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETCLIR;

/* Closed user group index */
typedef struct
{
   boolean                          present;
   byte                             msb;
   byte                             lsb;
} AEETCugIndex;

/* Forward CUG info */
typedef struct 
{
   boolean                          present;
   boolean                          suppress_pref_cug;
   boolean                          suppress_outgoing_access;
   AEETCugIndex                     cug_index;
} AEETFwdCugInfo;

/* Number type */
typedef enum {
  AEET_TON_UNKNOWN = 0,
  AEET_TON_INTERNATIONAL,
  AEET_TON_NATIONAL,
  AEET_TON_NETWORK_SPECIFIC,
  AEET_TON_DEDICATED_ACCESS,
  AEET_TON_RFU = 0xFF,
  AEET_TON_BIG = 0x10000000      /* Force 32-bit enums */
} AEETNumberType;

/* Number Plan */
typedef enum {
  AEET_NPI_UNKNOWN    = 0x00,
  AEET_NPI_ISDN       = 0x01,
  AEET_NPI_DATA       = 0x03,
  AEET_NPI_TELEX      = 0x04,
  AEET_NPI_NATIONAL   = 0x08,
  AEET_NPI_PRIVATE    = 0x09,
  AEET_NPI_RES_CTS    = 0x0B,
  AEET_NPI_RES_EXT    = 0x0F,
  AEET_NPI_RFU        = 0xFF,
  AEET_NPI_BIG        = 0x10000000  /* Force 32-bit enums */
} AEETNumberPlan;

/* Presentation indicator */
typedef enum {
  AEET_PI_ALLOW = 0x00,          /* Presentation allowed */
  AEET_PI_RES,                   /* Presentation restricted */
  AEET_PI_NOT_AVAIL,             /* Number not available */
  AEET_PI_BIG = 0x10000000       /* Force 32-bit enums */
} AEETPI;

/* Screening indicator */
typedef enum {
  AEET_SI_USER_NOT_SCREENED,    /* User-provided, not screened */
  AEET_SI_USER_PASS,            /* User-provided, verified and passed */
  AEET_SI_USER_FAIL,            /* User-provided, verified and failed */
  AEET_SI_NETWORK,              /* Network-provided */
  AEET_SI_BIG = 0x10000000      /* Force 32-bit enums */

} AEETSI;

/* Sub Address */
typedef struct 
{
  boolean   extension_bit;    /* The extension bit */
  uint8     odd_even_ind;     /* The indicator of even/odd bits - EVEN/ODD */
  char      addr[AEET_MAX_SUBADDRESS_DIGITS_LENGTH]; /* NULL terminated 
                              ** Array of subaddress digits */
} AEETSubAddress;

/* Type for Telephone number.
** Able to store a dialed number or a caller ID number.
*/
typedef struct
{

  char                     buf[AEET_MAX_DIGITS_LENGTH]; 
    /* NULL Terminated Number buffer */

  AEETNumberType           number_type;
    /* Number type */

  AEETNumberPlan           number_plan;
    /* Numbering Plan */

  AEETPI                   pi;
    /* Presentation Indicator */

  AEETSI                   si;
    /* Screening Indicator */

  AEETSubAddress           subaddr;
    /* Subaddress */

} AEETNumber;

/*
** Enumeration that defines the type of network list to be requested.  The
** requested list will either contain the preferred networks or all available
** networks.
*/
typedef enum
{

  AEET_NETWORK_LIST_NONE=-1,

  AEET_NETWORK_LIST_AVAILABLE,
    /* The available network list. */

  AEET_NETWORK_LIST_USER_PREFERRED,
    /* The preferred network list from SIM/USIM */

  AEET_NETWORK_LIST_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETNetworkType;

/* Privacy mode during a call. CDMA only.
*/
typedef enum
{
#ifdef CUST_EDITION
  AEET_PRIVACY_PREF_NONE = -1,
#else
  AEET_PRIVACY_PREF_NONE = 0,
#endif
  AEET_PRIVACY_PREF_STANDARD,
    /* Standard privacy */

  AEET_PRIVACY_PREF_ENHANCED,
    /* Enhanced privacy */

  AEET_PRIVACY_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETPrivacyPref;

/*---------------------------------------------------------------------------
 * Call Progress Information 
 ---------------------------------------------------------------------------*/
typedef enum {
  AEET_PROGIND_USER_SUSPENDED = 0,
  AEET_PROGIND_USER_RESUMED,
  AEET_PROGIND_BEARER_CHANGED,
  AEET_PROGIND_REMOTE_ALERT,
  AEET_PROGIND_UNKNOWN,
  AEET_PROGIND_BIG = 0x10000000
} AEETCallProgress;

/*---------------------------------------------------------------------------
 * Phone line information
 ---------------------------------------------------------------------------*/
typedef struct {
   uint32   dwMaxSupportedLines;
   uint32   dwSelectedLineId;
   boolean  bLineSwitchingEnabled;
} AEETLineInfo;

/*---------------------------------------------------------------------------
 * Call Information
 * Information about a call (includes technology dependent fields)
 ---------------------------------------------------------------------------*/
typedef struct {
   AEETCallState          call_state;  
   /* Call state - idle, orig, incom, conv */
 
   AEETCallType           call_type;
   /* Call type - VOICE, DATA, OTASP, etc */
  
   AEETSysMode            sys_mode;
   /* The system's mode. */
 
   AEETCallDirection      direction;
   /* Direction of the call */
 
   uint32                 start_time;
   /* Start time of the call (milliseconds since device was powered on) */
 
   uint32                 duration;
   /* Duration of call (ms), meaningful only after call has ended */
 
   AECHAR                 alpha[AEET_MAX_ALPHA_TAG_LENGTH];
   /* Last alpha numeric information received */
 
   char                   other_party_no[AEET_MAX_DIGITS_LENGTH];
   /* Other Party number */
 
   char                   other_party_subaddr[AEET_MAX_SUBADDRESS_DIGITS_LENGTH];
   /* Other Party subaddress */
 
   AEETNumber             redirect_party_number;
   /* Redirecting party number */
 
   AEETFwdCugInfo         forward_cug_info;
   /* Closed user Group information */
 
   AEETCLIR               cm_clir;
   /* CLIR information - CLIR suppression / invocation etc. */
 
   AEETNoCLICause         cause_of_no_cli;
   /* Cause of no Caller line identification */
 
   AEETServiceOption      srv_opt;
   /* Service option */
 
   AEETPrivacyPref        privacy_pref;
   /* User privacy preference */
 
   boolean                is_privacy;
   /* Indicate whether privacy mode is on */
 
   boolean                alert_ans_bypass;
   /* Indicate whether the incoming call need to be answered by the clients */
 
   char                   forwarded_from[AEET_MAX_DIGITS_LENGTH];
   /* The number forwarded from, if applicable */
 
   char                   forwarded_to[AEET_MAX_NUM_FWDING][AEET_MAX_DIGITS_LENGTH];
   /* List of numbers forwarded to for a call originating from this MS */
 
   AEETCallProgress       call_progress;
   /* call origination progress information */

   uint32                 line_id;
   /* line id the call is affiliated. */
   
} AEETCallInfo;

/*
** Enumeration of phone operating modes.
*/
typedef enum
{
  AEET_OPRT_MODE_NONE = -1,

  AEET_OPRT_MODE_PWROFF = 0,
    /* Phone is powering off                            */

  AEET_OPRT_MODE_FTM = 1,
    /* Phone is in factory test mode                    */

  AEET_OPRT_MODE_OFFLINE = 2,
    /* Phone is offline                                 */

  AEET_OPRT_MODE_OFFLINE_AMPS = 3,
    /* Phone is offline analog                          */

  AEET_OPRT_MODE_OFFLINE_CDMA = 4,
    /* Phone is offline cdma                            */

  AEET_OPRT_MODE_ONLINE = 5,
    /* Phone is online                                  */

  AEET_OPRT_MODE_LPM = 6,
    /* Phone is in LPM - Low Power Mode                 */

  AEET_OPRT_MODE_RESET = 7,
    /* Phone is resetting - i.e. power-cycling          */

  AEET_OPRT_MODE_NET_TEST_GW = 8,
    /* Phone is conducting network test for GSM/WCDMA.  */
    /* This mode can NOT be set by the clients. It can  */
    /* only be set by the lower layers of the stack.    */

  AEET_OPRT_MODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETOprtMode;

typedef enum
{
  AEET_TEST_CONTROL_TYPE_NONE=-1,

  AEET_TEST_CONTROL_TYPE_NO_TEST,
    /* Init value when no testing is going on */

  AEET_TEST_CONTROL_TYPE_LB_MODE1,
    /* LOOPBACK MODE1 */

  AEET_TEST_CONTROL_TYPE_LB_MODE2,
    /* LOOPBACK MODE2 */

  AEET_TEST_CONTROL_TYPE_LB_MODE1_WITH_DUMMY_SIGNALLING,
    /* LOOPBACK MODE1 with dummy signalling */

  AEET_TEST_CONTROL_TYPE_LB_MODE2_WITH_DUMMY_SIGNALLING,
    /* LOOPBACK MODE2 with dummy signalling */

  AEET_TEST_CONTROL_TYPE_BIG = 0x10000000
    /* Force 32 bit enums */

} AEETTestControlType;

/* Enumeration of phone CDMA lock modes.
** Meaningful in CDMA only
*/
typedef enum
{

  AEET_CDMA_LOCK_MODE_NONE=-1,

  AEET_CDMA_LOCK_MODE_OFF,
    /* phone is NOT CDMA locked */

  AEET_CDMA_LOCK_MODE_ON,
    /* phone is CDMA locked until power-cycle */

  AEET_CDMA_LOCK_MODE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETCDMALockMode;

/* Enumeration of answer voice as data settings. CDMA Only.
*/
typedef enum
{

  AEET_ANSWER_VOICE_NONE=-1,

  AEET_ANSWER_VOICE_AS_VOICE,
    /* Answer voice as voice */

  AEET_ANSWER_VOICE_AS_FAX_ONCE,
    /* Answer voice as fax once (for the next 10 minutes) */

  AEET_ANSWER_VOICE_AS_FAX_ALWAYS,
     /* Answer voice as fax always */

  AEET_ANSWER_VOICE_AS_MODEM_ONCE,
    /* Answer voice as modem once (for the next 10 minutes) */

  AEET_ANSWER_VOICE_AS_MODEM_ALWAYS,
   /* Answer voice as modem always */

  AEET_ANSWER_VOICE_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETAnsVoicePref;

/* Enumeration of mode preferences.
*/
typedef enum
{

  AEET_MODE_PREF_NONE=-1,

  AEET_MODE_PREF_AMPS_ONLY=0,
    /* Service is limited to analog only */

  AEET_MODE_PREF_DIGITAL_ONLY=1,
    /* Service is limited to digital (CDMA, HDR, GW) only */

  AEET_MODE_PREF_AUTOMATIC=2,
    /* Determine the mode automatically */

  AEET_MODE_PREF_EMERGENCY=3,
    /* Emergency mode */

  AEET_MODE_PREF_CDMA_ONLY = 9,
    /* Service is limited to CDMA only */

  AEET_MODE_PREF_HDR_ONLY=10,
    /* Service is limited to HDR only */

  AEET_MODE_PREF_CDMA_AMPS_ONLY=11,
    /* Service is limited to CDMA and AMPS only */

  AEET_MODE_PREF_GPS_ONLY=12,
    /* Service is limited to GPS only */

  /*
  ** The following enums are the radio access technologies for WCDMA and GSM.
  */
  AEET_MODE_PREF_GSM_ONLY=13,
    /* Service is limited to GSM only */

  AEET_MODE_PREF_WCDMA_ONLY=14,
    /* Service is limited to WCMDA only */

  /*
  ** Others
  */
  AEET_MODE_PREF_PERSISTENT=15,
    /* Return to persistent mode preference */

  AEET_MODE_PREF_NO_CHANGE=16,
    /* Do not change the mode preference */

  AEET_MODE_PREF_CDMA_HDR_ONLY=22,
    /* CDMA and HDR modes are supported only */

  AEET_MODE_PREF_WLAN_ONLY=23,
    /* Service is limited to WLAN only */
  
  AEET_MODE_PREF_GSM_WCDMA_ONLY = 0x01072618,
    /* Service is limited to GSM and WCDMA only */
 
  AEET_MODE_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETModePref;

/* Enumeration of mode terms - i.e designate the mode span.
*/
typedef enum
{

  AEET_PREF_TERM_NONE=-1,

  AEET_PREF_TERM_PERMANENT,
    /* Permanent mode change */

  AEET_PREF_TERM_PWR_CYCLE,
    /* Until power cycle */

  AEET_PREF_TERM_1_CALL,
    /* Until end of next call or power-cycle */

  AEET_PREF_TERM_1_CALL_OR_TIME,
    /* Until end of next call, specified timeout or power-cycle */

  AEET_PREF_TERM_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETPrefTerm;

/* Enumeration of GSM/WCDMA acquisition order
*/
typedef enum
{

  AEET_GW_ACQ_ORDER_PREF_NONE = -1,

  AEET_GW_ACQ_ORDER_PREF_AUTOMATIC=0,
    /* Determine mode automatically from the PRL order */

  AEET_GW_ACQ_ORDER_PREF_GSM_WCDMA=1,
    /* Acqisition order is first GSM followed by WCDMA */

  AEET_GW_ACQ_ORDER_PREF_WCDMA_GSM=2,
    /* Acqisition order is first WCDMA followed by GSM */

  AEET_GW_ACQ_ORDER_PREF_NO_CHANGE=3,
    /* Do not change GSM/WCDMA acquisition order */

  AEET_GW_ACQ_ORDER_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETAcqOrderPref;

/*
** Enumeration that defines the service domain being requested (the preferred service domain).
*/
typedef enum
{

  AEET_SRV_DOMAIN_PREF_NONE=-1,

  AEET_SRV_DOMAIN_PREF_CS_ONLY=0,
    /* Prefer Circuit Switched Only */

  AEET_SRV_DOMAIN_PREF_PS_ONLY=1,
    /* Prefer Packet Switched Only */

  AEET_SRV_DOMAIN_PREF_CS_PS=2,
    /* Prefer Circuit and Packet Switched */

  AEET_SRV_DOMAIN_PREF_ANY=3,
    /* Any domain will do. No preference */

  AEET_SRV_DOMAIN_PREF_NO_CHANGE=4,
    /* To be used by clients who do not want to change the service domain */

  AEET_SRV_DOMAIN_PREF_PS_ATTACH=5,
    /*  PS attach on demand.
        Note: This value of service domain preference is not saved in NV  */

  AEET_SRV_DOMAIN_PREF_PS_DETACH=6,
  /* PS detach on demand
       Note: This value of service domain preference is not saved in NV   */

  AEET_SRV_DOMAIN_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETServDomPref;

/*
** Enumeration of system band classes and band classes' sub-bands.
** If there is a new band class, append to the end of list.
*/
typedef enum {
  AEET_SBAND_BC0_A = 0,
    /* Band Class 0, A-System */

  AEET_SBAND_BC0_B = 1,
    /* Band Class 0, B-System */

    /* Band Class 0 AB , GSM 850 Band*/

  AEET_SBAND_BC1 = 2,
    /* Band Class 1, all blocks */

  AEET_SBAND_BC2 = 3,
    /* Band Class 2 place holder */

  AEET_SBAND_BC3 = 4,
    /* Band Class 3, A-System */

  AEET_SBAND_BC4 = 5,
    /* Band Class 4, all blocks */

  AEET_SBAND_BC5 = 6,
    /* Band Class 5, all blocks */

  AEET_SBAND_GSM_DCS_1800 = 7,
    /* GSM DCS band */

  AEET_SBAND_GSM_EGSM_900 = 8,
    /* GSM Extended GSM (E-GSM) band */

  AEET_SBAND_GSM_PGSM_900 = 9,
    /* GSM Primary GSM (P-GSM) band */

  AEET_SBAND_BC6 = 10,
    /* Band Class 6 */

  AEET_SBAND_BC7 = 11,
    /* Band Class 7 */

  AEET_SBAND_BC8 = 12,
    /* Band Class 8 */

  AEET_SBAND_BC9 = 13,
    /* Band Class 9*/

  AEET_SBAND_BC10 = 14,
    /* Band Class 10 */

  AEET_SBAND_BC11 = 15,
    /* Band Class 11 */

  AEET_SBAND_GSM_450 = 16,
    /* GSM 450 band */

  AEET_SBAND_GSM_480 = 17,
    /* GSM 480 band */

  AEET_SBAND_GSM_750 = 18,
    /* GSM 750 band */

  AEET_SBAND_GSM_850 = 19,
    /* GSM 850 band */

  AEET_SBAND_GSM_RGSM_900 = 20,
    /* GSM Railways GSM Band */

  AEET_SBAND_GSM_PCS_1900 = 21,
    /* GSM PCS band */

  AEET_SBAND_WCDMA_I_IMT_2000 = 22,
    /* WCDMA IMT 2000 band */

  AEET_SBAND_WCDMA_II_PCS_1900 = 23,
    /* WCDMA PCS band */

  AEET_SBAND_WCDMA_III_1700 = 24,
    /* WCDMA 1700 band */

  AEET_SBAND_WCDMA_IV_1700 = 25,
    /* WCDMA 1700 band */

  AEET_SBAND_WCDMA_V_850 = 26,
     /* WCDMA 850 band */

  AEET_SBAND_WCDMA_VI_800 = 27,
     /* WCDMA 800 band */

  AEET_SBAND_WLAN_US_2400 = 28,
     /* WLAN US 2.4GHz */

  AEET_SBAND_WLAN_JP_2400 = 29,
     /* WLAN Japan 2.4GHz */

  AEET_SBAND_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSysSubBandClass;

/* Enumeration of band preferences.
*/
typedef enum
{

  AEET_BAND_PREF_NONE= 0x0000,

  AEET_BAND_PREF_BC0_A=AEET_BM(AEET_SBAND_BC0_A),
    /* Acquire band class 0, A-Side systems only */

  AEET_BAND_PREF_BC0_B=AEET_BM(AEET_SBAND_BC0_B),
    /* Acquire band class 0, B-Side systems only */

  AEET_BAND_PREF_BC0=AEET_BM(AEET_SBAND_BC0_A)|AEET_BM(AEET_SBAND_BC0_B),
    /* Acquire band class 0 systems only */

  AEET_BAND_PREF_BC1=AEET_BM(AEET_SBAND_BC1),
    /* Acquire band class 1 systems only */

  AEET_BAND_PREF_BC3=AEET_BM(AEET_SBAND_BC3),
    /* Acquire band class 3 systems only */

  AEET_BAND_PREF_BC4=AEET_BM(AEET_SBAND_BC4),
    /* Acquire band class 4 systems only */

  AEET_BAND_PREF_BC5=AEET_BM(AEET_SBAND_BC5),
    /* Acquire band class 5 systems only */

  AEET_BAND_PREF_GSM_DCS_1800=AEET_BM(AEET_SBAND_GSM_DCS_1800),
    /* Acquire GSM DCS systems only */

  AEET_BAND_PREF_GSM_EGSM_900=AEET_BM(AEET_SBAND_GSM_EGSM_900),
    /* Acquire GSM Extended GSM 900 systems only */

  AEET_BAND_PREF_GSM_PGSM_900=AEET_BM(AEET_SBAND_GSM_PGSM_900),
    /* Acquire GSM Primary GSM 900 systems only */

  AEET_BAND_PREF_BC6=AEET_BM(AEET_SBAND_BC6),
    /* Acquire band class 6 systems only */

  AEET_BAND_PREF_BC7=AEET_BM(AEET_SBAND_BC7),
    /* Acquire band class 7 systems only */

  AEET_BAND_PREF_BC8=AEET_BM(AEET_SBAND_BC8),
    /* Acquire band class 8 systems only */

  AEET_BAND_PREF_BC9=AEET_BM(AEET_SBAND_BC9),
    /* Acquire band class 9 systems only */

  AEET_BAND_PREF_BC10=AEET_BM(AEET_SBAND_BC10),
    /* Acquire band class 10 systems only */

  AEET_BAND_PREF_BC11=AEET_BM(AEET_SBAND_BC11),
    /* Acquire band class 11 systems only */

  AEET_BAND_PREF_GSM_450=AEET_BM( AEET_SBAND_GSM_450 ),
    /* Acquire GSM 450 systems only */

  AEET_BAND_PREF_GSM_480=AEET_BM( AEET_SBAND_GSM_480 ),
    /* Acquire GSM 480 systems only */

  AEET_BAND_PREF_GSM_750=AEET_BM( AEET_SBAND_GSM_750 ),
    /* Acquire GSM 750 systems only */

  AEET_BAND_PREF_GSM_850=AEET_BM( AEET_SBAND_GSM_850 ),
    /* Acquire GSM 850 systems only */

  AEET_BAND_PREF_GSM_RGSM_900=AEET_BM(AEET_SBAND_GSM_RGSM_900),
    /* Acquire GSM Railway GSM 900 systems only */

  AEET_BAND_PREF_GSM_PCS_1900=AEET_BM( AEET_SBAND_GSM_PCS_1900 ),
    /* Acquire GSM PCS systems only */

  AEET_BAND_PREF_WCDMA_I_IMT_2000=AEET_BM( AEET_SBAND_WCDMA_I_IMT_2000 ),
    /* Acquire WCDMA IMT 2000 systems only */

  AEET_BAND_PREF_WCDMA_II_PCS_1900=AEET_BM( AEET_SBAND_WCDMA_II_PCS_1900 ),
    /* Acquire WCDMA PCS systems only */

  AEET_BAND_PREF_WCDMA_III_1700=AEET_BM( AEET_SBAND_WCDMA_III_1700 ),
    /* Acquire WCDMA 1700 systems only */

  AEET_BAND_PREF_WCDMA_IV_1700=AEET_BM( AEET_SBAND_WCDMA_IV_1700 ),
    /* Acquire WCDMA 1700 systems only */

  AEET_BAND_PREF_WCDMA_V_850=AEET_BM( AEET_SBAND_WCDMA_V_850 ),
     /* Acquire WCDMA 850 systems only */

  AEET_BAND_PREF_WCDMA_VI_800=AEET_BM( AEET_SBAND_WCDMA_VI_800 ),
    /* Acquire WCDMA 800 systems only */

  AEET_BAND_PREF_WLAN_US_2400=AEET_BM( AEET_SBAND_WLAN_US_2400 ),

  AEET_BAND_PREF_WLAN_JP_2400=AEET_BM( AEET_SBAND_WLAN_JP_2400 ),

  AEET_BAND_PREF_ANY=0x3FFFFFFF,
    /* Acquire any band class systems */

  AEET_BAND_PREF_NO_CHANGE
    /* Do not change the band preference */

} AEETBandPref;

/* Cellular and PCS band preference.
**
** Note that IS-683-A PRL only provide support for one cellular band and
** one PCS band. Therefore we define a Cellular and PCS band preferences
** that are mapped into different band-classes preferences depending on
** the market, as follows:
**
** Cellular band preference is mapped to band-class 3 preference for the
** Japanese market or band-class 0 preference for any other market.
**
** PCS band preference is mapped to band-class 4 preference for the
** Korean market or band-class 1 preference for any other market.
*/

#define AEET_BAND_PREF_CELL_A   AEET_BAND_PREF_BC0_A
    /* Acquire cellular A-Side systems only */

#define AEET_BAND_PREF_CELL_B   AEET_BAND_PREF_BC0_B
    /* Acquire cellular B-Side systems only */

#define AEET_BAND_PREF_CELL     AEET_BAND_PREF_BC0
    /* Acquire cellular (A or B) systems only */

#define AEET_BAND_PREF_PCS      AEET_BAND_PREF_BC1
    /* Acquire PCS systems only */

/* Enumeration of system selection roam preferences.
*/
typedef enum
{

  AEET_ROAM_PREF_NONE=0,

  AEET_ROAM_PREF_HOME=AEET_BM(AEET_ROAM_STATUS_OFF),
    /* Acquire only systems for which the roaming indication is off
    ** SD_SS_ROAM_PREF_HOME */

  AEET_ROAM_PREF_AFFIL=AEET_BM(AEET_ROAM_STATUS_OFF)|AEET_BM(AEET_ROAM_STATUS_ON),
    /* Acquire only systems for which the roaming indication is off or
    ** solid on - i.e. not flashing. CDMA Only
    ** SD_SS_ROAM_PREF_AFFIL */

  AEET_ROAM_PREF_ANY=0xFF,
    /* Acquire systems regardless of their roaming indication
    ** SD_SS_ROAM_PREF_ANY */

  AEET_ROAM_PREF_NO_CHANGE,
    /* Do not change the roam preference */

  AEET_ROAM_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETRoamPref;

/* Enumeration of NAM selections
*/
typedef enum
{

  AEET_NAM_NONE=-1,

  AEET_NAM_1,
    /* NAM 1*/

  AEET_NAM_2,
    /* NAM 2 - for devices with more than 1 NAM */

  AEET_NAM_3,
    /* NAM 3 - for devices with more than 2 NAM's */

  AEET_NAM_4,
    /* NAM 4 - for devices with more than 3 NAM's */

  AEET_NAM_AUTO,
    /* Auto NAM - for devices with more than 1 NAM */

  AEET_NAM_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETNAM;

/* Enumeration of system selection hybrid preferences.*/
typedef enum 
{

  AEET_HYBR_PREF_NONE = 0,

  AEET_HYBR_PREF_OFF=1,
    /* hybrid operation is not allowed - SD_SS_HYBR_PREF_NONE */

  AEET_HYBR_PREF_ON=2,
    /* hybrid operation is allowed - SD_SS_HYBR_PREF_CDMA_HDR */

  AEET_HYBR_PREF_NO_CHANGE=3,
    /* Do not change the hybr preference */

  AEET_HYBR_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETHybrPref;

/*
** Enumeration that defines whether the network should be selected
** automatically, manually or for limited service. GSM/WCDMA Only.
*/
typedef enum
{

  AEET_NETWORK_SEL_MODE_PREF_NONE=-1,

  AEET_NETWORK_SEL_MODE_PREF_AUTOMATIC=0,
    /* Automatic network selection */

  AEET_NETWORK_SEL_MODE_PREF_MANUAL=1,
    /* Manual network selection */

  AEET_NETWORK_SEL_MODE_PREF_LIMITED_SRV=2,
    /* Select a network for limited service */

  AEET_NETWORK_SEL_MODE_PREF_NO_CHANGE=3,
  /* No change */

  AEET_NETWORK_SEL_MODE_PREF_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETNwSelModePref;

/*
** The following enumerated type defines the access technologies that are
** defined for the elementary file, EFPLMNwAcT (User controlled PLMN selector
** with Access Technology).  The #defines are to be used in a bit mask.
** Refer to TS 11.11.
*/
typedef uint16 AEETAccessTech;

#define  AEET_SYS_ACT_GSM_MASK         ( (1<<0) )
    /* GSM access technology                  */

#define  AEET_SYS_ACT_GSM_COMPACT_MASK ( (1<<1) )
    /* GSM compact access technology          */

#define  AEET_SYS_ACT_UMTS_MASK        ( (1<<2) )
    /* UMTS radio access technology           */

/*
** Define a structure that contains information regarding the user
** preferred/controlled network. 
*/
typedef struct 
{
  AEETPLMNID                        plmn;
    /* PLMN ID */

  AEETAccessTech                    access_tech;
    /* Access technologies that are defined for file EFPLMNwAcT */

} AEETUserPLMNInfo;

/*
** Define a structure for the list of user preferred/controlled networks.
*/
typedef struct 
{
  byte                            length;
    /* The number of PLMNs in the list */

  AEETUserPLMNInfo               info[AEET_USER_PPLMN_LIST_MAX_LENGTH];
    /* PLMN Info */

} AEETUserPLMNList;

/*
** The following enumerated type defines the radio access technologies.
*/
typedef enum
{
  AEET_RAT_NONE = -1,

  AEET_RAT_GSM_RADIO_ACCESS,
    /* GSM radio access technology            */

  AEET_RAT_UMTS_RADIO_ACCESS,
  /* UMTS radio access technology           */

  AEET_RAT_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETRadioAccessTech;

typedef enum
{
  AEET_DETAILED_PLMN_LIST_CATEGORY_NONE = -1,  
  AEET_DETAILED_PLMN_LIST_CATEGORY_HPLMN,
  AEET_DETAILED_PLMN_LIST_CATEGORY_PREFERRED,
  AEET_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED,
  AEET_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED,
  AEET_DETAILED_PLMN_LIST_CATEGORY_OTHER,
  AEET_DETAILED_PLMN_LIST_CATEGORY_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETDetailedPLMNCat;

/*
** The following enumerated type defines the PLMN's signal quality.
*/
typedef enum
{
  AEET_SIGNAL_QUALITY_NONE = -1,                  
  AEET_SIGNAL_QUALITY_HIGH,
  AEET_SIGNAL_QUALITY_LOW,
  AEET_SIGNAL_QUALITY_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSignalQuality;

/*
** The following type defines the PLMN's the signal strength in
** units that allow direct comparison of the signal strength between
** GSM and WCDMA cells.
**
*/
typedef int32 AEETSignalStrength;

/* Detailed PLMN Info */
typedef struct 
{
  AEETPLMNID                       plmn;
    /* The ID of the PLMN                                        */

  AEETRadioAccessTech              rat;
    /* The radio access technology of the PLMN                   */

  AEETSrvCapability                plmn_service_capability;
    /* Indicates the type of service domain the PLMN can provide */

  boolean                           plmn_forbidden;
    /* Indicates if the PLMN is forbidden                        */

  AEETDetailedPLMNCat              list_category;
    /* Indicates the type of PLMN                                */

  AEETSignalQuality                signal_quality;
    /* The signal quality of the PLMN                            */

  AEETSignalStrength               signal_strength;
    /* The signal strength of the PLMN                           */

} AEETDetailedPLMNInfo;

/*
** Define a structure for the detailed PLMN list.
*/
typedef struct 
{
  uint32                           length;
    /* The number of PLMNs in the list */

  AEETDetailedPLMNInfo            info[AEET_PLMN_LIST_MAX_LENGTH];
    /* Detailed PLMN Info */

} AEETDetailedPLMNList;

/* Current RTRE confiuration. Meaningful in CDMA only. */
typedef enum
{

  AEET_RTRE_CONFIG_NONE,

  AEET_RTRE_CONFIG_RUIM_ONLY,
    /* RTRE configuration: use R-UIM only */

  AEET_RTRE_CONFIG_NV_ONLY,
    /* RTRE configuration: use NV only    */

  AEET_RTRE_CONFIG_RUIM_OR_DROP_BACK,
    /* RTRE use R-UIM if available or NV  */

  AEET_RTRE_CONFIG_GSM_SIM,
    /* RTRE use UIM formatted with GSM provisioning data */

  AEET_RTRE_CONFIG_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETRTREConfig;

/* Current RTRE control. Meaningful in CDMA only */
typedef enum
{

  AEET_RTRE_CONTROL_NONE,

  AEET_RTRE_CONTROL_RUIM,
    /* RTRE control: use R-UIM */

  AEET_RTRE_CONTROL_NV,
    /* RTRE control: use NV */

  AEET_RTRE_CONTROL_GSM_SIM,
    /* RTRE control: use GSM SIM card. */

  AEET_RTRE_CONTROL_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETRTREControl;

typedef struct 
{
  uint32    ccm;
    /* Current Call Meter */

  uint32    acm;
    /* Accumulated Call Meter */

  uint32    acm_max;
    /* Accumulated Call Meter Max */

} AEETAOCInfo;

/* Enumeration of Dedicated Data Transmission mode preferences.*/
typedef enum 
{
  AEET_DDTM_PREF_NONE        = -1,

  AEET_DDTM_PREF_OFF,
        /* disable data dedicated transmission mode */

  AEET_DDTM_PREF_ON,
        /* enable data dedicated transmission mode */

  AEET_DDTM_PREF_NO_CHANGE,
        /* Do not change the DDTM preference */

  AEET_DDTM_PREF_BIG=0x10000000
      /* To force enum to 32 bit */

} AEETDDTMPref;

/*---------------------------------------------------------------------------
 * AEE Telephone Information
 *--------------------------------------------------------------------------*/
typedef struct {
   boolean                          is_in_use;
   /* Indicate whether phone is currently in use */
 
   AEETOprtMode                     oprt_mode;
   /* Indicate current operating mode */
 
   AEETTestControlType              test_control_type;
   /* Indicate current test control type */
 
   AEETCDMALockMode                 cdma_lock_mode;
   /* Indicate whether phone is CDMA locked until power cycle */
 
   AEETAnsVoicePref                 answer_voice;
   /* Indicate answer voice preference when active */
 
   AEETModePref                     mode_pref;
   /* Indicate current mode preference */
 
   AEETPrefTerm                     pref_term;
   /* Indicate the current preference term - one-call, power-cycle, etc.*/
 
   AEETAcqOrderPref                 acq_order_pref;
   /* The acquisition order preferred by the client. GSM/WCDMA only. */
 
   AEETServDomPref                  srv_domain_pref;
   /* The service domain preferred by the client. */
   
   AEETBandPref                     band_pref;
    /* Indicate current band preference */
 
   AEETRoamPref                     roam_pref;
   /* Indicate current roam preference */
 
   AEETNAM                          nam_sel;
   /* Indicate the NAM selection. CDMA only */
 
   AEETNAM                          curr_nam;
   /* Indicate the current NAM -
   ** needed when NAM selection is set to AUTO-NAM. CDMA only */
 
   AEETHybrPref                     hybr_pref;
   /* Indicate hybrid preference */
 
   AEETNwSelModePref                network_sel_mode_pref;
   /* The network selection mode selected by the client. GSM/WCDMA only */
 
   AEETPLMNID                       plmn;
   /* The identifier of the PLMN to be selected manually. GSM/WCDMA only */
 
   AEETUserPLMNList                 user_pref_networks;
   /* The user preferred network list. GSM/WCDMA only */
 
   AEETDetailedPLMNList             available_networks;
   /* Available networks. GSM/WCDMA only */
 
   boolean                          is_cdma_subscription_available;
   /* Indicate whether the cdma subscription/provisioning information
   ** is available for consumption.                              */
 
   boolean                          is_gw_subscription_available;
   /* Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              */
 
   AEETRTREConfig                   rtre_config;
   /* Current RTRE configuration - RUIM, NV, RUIM fallback NV. CDMA only */
 
   AEETRTREControl                  rtre_control;
   /* Current RTRE control - RUIM or NV. CDMA only */
 
   AEETAOCInfo                      aoc_info;
   /* Advice of charge info */
 
   AEETDDTMPref                     ddtm_pref;
   /* Dedicated Data Transmission Mode */

   AEETLineInfo                     line_info;
   /* phone line information */

} AEETPhInfo;

typedef struct {
   AEETModePref                     mode_pref;
   AEETPrefTerm                     pref_term;
   uint32                           pref_duration;
   AEETAcqOrderPref                 acq_order_pref;
   AEETBandPref                     band_pref;
   AEETRoamPref                     roam_pref;
   AEETHybrPref                     hybr_pref;
   AEETServDomPref                  srv_domain_pref;
   AEETNwSelModePref                nw_sel_mode_pref;
   AEETPLMNID                       plmn;
} AEETSystemPreference;

typedef struct {
   AEETUserPLMNList  pref_networks;    
} AEETPrefNetworks;

/* Enumeration to indicate the type of system to be avoided
*/
typedef enum {

  AEET_SS_AVOID_SYS_NONE        = -1,

  AEET_SS_AVOID_SYS_SRV_IF_UZ,
    /* Avoid serving system only if MS has currently selected user zone */

  AEET_SS_AVOID_SYS_SRV,
    /* Avoid serving system */

  AEET_SS_AVOID_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETAvoidSSPref;

typedef enum
{
  AEET_DTMF_ON_95,
  /* 95 ms recommended pulse width */

  AEET_DTMF_ON_150,
  /* 150 ms recommended pulse width */

  AEET_DTMF_ON_200,
  /* 200 ms recommended pulse width */

  AEET_DTMF_ON_250,
  /* 250 ms recommended pulse width */

  AEET_DTMF_ON_300,
  /* 300 ms recommended pulse width */

  AEET_DTMF_ON_350,
  /* 350 ms recommended pulse width */
  
  AEET_DTMF_ON_SMS,
  /* SMS Tx special pulse width */

  AEET_DTMF_ON_BIG = 0x10000000
  /* Force 32-bit enums */

} AEETDTMFOnLength;

typedef enum
{
  AEET_DTMF_OFF_60,
  /* 60 ms recommended minimum interdigit interval */

  AEET_DTMF_OFF_100,
  /* 100 ms recommended minimum interdigit interval */

  AEET_DTMF_OFF_150,
  /* 150 ms recommended minimum interdigit interval */

  AEET_DTMF_OFF_200,
  /* 200 ms recommended minimum interdigit interval */

  AEET_DTMF_OFF_BIG = 0x10000000
  /* Force 32-bit enums */

} AEETDTMFOffLength;

/* User Data Activation type */
#define AEET_USER_DATA_TYPE1_IMPLICIT               0X01
/* Service to activated implicitly by presence of user data */

#define AEET_USER_DATA_TYPE1_EXPLICIT_REQUIRED      0x02
/* Service to activated explicitly, user data service is required for the call */

#define AEET_USER_DATA_TYPE1_EXPLICIT_NOTREQUIRED   0x03
/* Service to activated explicitly, user data service is not required for the call */

typedef struct {
   uint8    type;
   /* User data service is to be activated implicitly or explicitly */

   uint16   length;
   /* Length of the user data filled in the buffer */

   uint8    user_data[AEET_MAX_USER_DATA_LENGTH];
   /* User data information */
     
} AEETUserDataInfo;

/*---------------------------------------------------------------------------
  AEET Event Class
 ---------------------------------------------------------------------------*/
#define AEET_EVENT_CLASS( aeet_event ) (uint16)(((uint16)aeet_event) >> 8)
#define AEET_EVENT_CLASS_CALL   0x01
#define AEET_EVENT_CLASS_PH     0x02
#define AEET_EVENT_CLASS_SS     0x03
#define AEET_EVENT_CLASS_INBAND 0x04
#define AEET_EVENT_CLASS_SUPPS  0x07
#define AEET_EVENT_CLASS_MISC   0x08
#define AEET_EVENT_CLASS_USER   0x09

/*---------------------------------------------------------------------------
  AEET Event Types
 ---------------------------------------------------------------------------*/

typedef enum AEETEventEnum
{
  AEET_EVENT_NONE = 0x0000,

  /* Call Events */
  AEET_EVENT_CALL_ORIG = 0x0101, /* Phone originated a call */
  AEET_EVENT_CALL_ANSWER,        /* Incoming call was answered */
  AEET_EVENT_CALL_END,           /* In/Orig call was ended */
  AEET_EVENT_CALL_INCOM,         /* Received an incoming call */
  AEET_EVENT_CALL_CONNECT,       /* In/Orig call connected */
  AEET_EVENT_CALL_ERROR,         /* Call Error */
  AEET_EVENT_CALL_OPS_ERROR,     /* Call related sups error */
  AEET_EVENT_CALL_OPS,           /* phone sent Flash/Flash with Info to BS */
  AEET_EVENT_CALL_CALLER_ID,     /* Caller ID info was received from BS */
  AEET_EVENT_CALL_DISPLAY,        /* display info was received from BS. */
  AEET_EVENT_CALL_CALLED_PARTY,   /* called party info was received from BS. */
  AEET_EVENT_CALL_CONNECTED_NUM,  /* connected number info was received */
  AEET_EVENT_CALL_BARRED,         /* call barred notification */
  AEET_EVENT_CALL_IS_WAITING,     /* call is in waiting notification */
  AEET_EVENT_CALL_ON_HOLD,        /* call on hold notification */ 
  AEET_EVENT_CALL_RETRIEVED,      /* call retrieved notification */
  AEET_EVENT_CALL_ORIG_FWD_STATUS,/* call may be forwarded (to forwarding MS)*/
  AEET_EVENT_CALL_FORWARDED,      /* call forwarded (to forwarding MS) */
  AEET_EVENT_CALL_BEING_FORWARDED,/* call being forwarded (to calling MS) */
  AEET_EVENT_CALL_INCOM_FWD_CALL, /* Incoming forwarded call notification */
  AEET_EVENT_CALL_RESTRICTED,     /* call restricted notification */
  AEET_EVENT_CALL_CUG_INFO_RECEIVED,   /* call forward CUG notification */
  AEET_EVENT_CALL_CNAP_INFO_RECEIVED,  /* caller name info notification */
  AEET_EVENT_CALL_EMERGENCY_FLASHED,   /* voice call converted to emergency */
  AEET_EVENT_CALL_DEFLECTION,        /* Call deflection notification */
  AEET_EVENT_CALL_TRANSFERRED,       /* Call transfer notification */
  AEET_EVENT_CALL_SRV_OPT,           /* Service option changed */
  AEET_EVENT_CALL_PRIVACY,           /* Privacy mode changed */  
  AEET_EVENT_CALL_ABRV_ALERT,        /* CMDA/AMPS abbreviated alert. */
  AEET_EVENT_CALL_ABRV_REORDER,      /* AMPS abbreviated reorder. */
  AEET_EVENT_CALL_ABRV_INTERCEPT,    /* AMPS abbreviated intercept. */
  AEET_EVENT_CALL_SIGNAL,            /* signal info was received from BS */
  AEET_EVENT_CALL_PROGRESS_INFO_IND, /* call origination progress indication */
  AEET_EVENT_CALL_ENTER_DORMANT,     /* Call entered dormant state */
  AEET_EVENT_CALL_EXIT_DORMANT,      /* call exit dormant state. */
  AEET_EVENT_CALL_REDIRECTING_NUMBER, /* Redirecting Number Info Record Event. */
  AEET_EVENT_CALL_EXT_BRST_INTL,     /* This event is sent when mobile station receives a
                                        EXTENDED BURST TYPE INTERNATIONAL data burst message, 
                                        which conveys the charge rate info for the call. */

  /* Phone events */
  AEET_EVENT_PH_OPRT_MODE = 0x0201, /* Operating mode was changed */
  AEET_EVENT_PH_SYSTEM_PREF,        /* System preference changed */
  AEET_EVENT_PH_ANSWER_VOICE,       /* Answer voice as data was changed. */
  AEET_EVENT_PH_NAM_SEL,            /* NAM selection was changed */
  AEET_EVENT_PH_CURR_NAM,           /* Current NAM was changed. */ 
  AEET_EVENT_PH_IN_USE_STATE,       /* In use state was changed */
  AEET_EVENT_PH_CDMA_LOCK_MODE,     /* CDMA lock mode was changed. */
  AEET_EVENT_PH_MAINTREQ,           /* CDMA maintenance required command. */
  AEET_EVENT_PH_STANDBY_SLEEP,      /* Entering powerdown sleep mode */
  AEET_EVENT_PH_STANDBY_WAKE,       /* Exiting powerdown sleep mode */
  AEET_EVENT_PH_INFO_AVAIL,         /* Phone information is now available */
  AEET_EVENT_PH_SUBSCRIPTION,       /* Subscription info changed */
  AEET_EVENT_PH_FUNDS_LOW,          /* Funds running low. */
  AEET_EVENT_PH_NVRUIM_CONFIG,      /* RTRE(Runtime R-UIM Enable) configuration changed. */
  AEET_EVENT_PH_ACM_RESET,          /* Accumulated Call Meter was reset */
  AEET_EVENT_PH_ACMMAX_SET,         /* Accumulated Call Meter was set */
  AEET_EVENT_PH_LINE_INFO,          /* line information changed */
  AEET_EVENT_PH_AVAILABLE_NETWORKS,  /* Available networks refreshed */
  

  /* Serving system Events */
  AEET_EVENT_SS_SRV_CHANGED = 0x0301, /* Serving system information changed */
  AEET_EVENT_SS_RSSI,                 /* RSSI changed */
  AEET_EVENT_SS_HDR_RSSI,             /* HDR RSSI changed */
  AEET_EVENT_SS_WLAN_RSSI,            /* WLAN RSSI changed */

  /* Inband Events */
  AEET_EVENT_INBAND_FWD_BURST_DTMF = 0x0401, /* received a CDMA-burst DTMF */
  AEET_EVENT_INBAND_FWD_START_DTMF,          /* received a start-cont DTMF order */
  AEET_EVENT_INBAND_FWD_STOP_DTMF,           /* received a stop-cont DTMF order */

  /* Network initiated supplementary service transaction. */
  AEET_EVENT_SUPPS_NETWORK_IND = 0x701,
  
  /* outgoing supplementary service transaction. */
  AEET_EVENT_SUPPS_COMMN_STARTED = 0x702,

  /* An ongoing transaction ended */
  AEET_EVENT_SUPPS_COMMN_ENDED = 0x703,
  
  /* User events */
  AEET_EVENT_USER_USER_DATA = 0x901,  /* User to User data event */

  AEET_EVENT_BIG = 0x10000000

} AEETEvent;

/* (Extended) Display Type */
typedef enum {
  AEET_DISP_NORMAL = 0x00,
  AEET_DISP_BLANK = 0x80,
  AEET_DISP_SKIP = 0x81,
  AEET_DISP_CONTINUATION = 0x82,
  AEET_DISP_CALLED_ADDR = 0x83,
  AEET_DISP_CAUSE = 0x84,
  AEET_DISP_PROGRESS_IND = 0x85,
  AEET_DISP_NOTIF_IND = 0x86,
  AEET_DISP_PROMPT = 0x87,
  AEET_DISP_ACCUM_DIGITS = 0x88,
  AEET_DISP_STATUS = 0x89,
  AEET_DISP_INBAND = 0x8A,
  AEET_DISP_CALLING_ADDR = 0x8B,
  AEET_DISP_REASON = 0x8C,
  AEET_DISP_CALLING_NAME = 0x8D,
  AEET_DISP_CALLED_NAME = 0x8E,
  AEET_DISP_ORIGINAL_NAME = 0x8F,
  AEET_DISP_REDIRECTING_NAME = 0x90,
  AEET_DISP_CONNECTED_NAME = 0x91,
  AEET_DISP_ORIG_RESTRICT = 0x92,
  AEET_DISP_DATE_AND_TIME = 0x93,
  AEET_DISP_CALL_APPEARNC = 0x94,
  AEET_DISP_FEATURE_ADDR = 0x95,
  AEET_DISP_REDIRECTION_NAM = 0x96,
  AEET_DISP_REDIRECTION_NUM = 0x97,
  AEET_DISP_REDIRECTING_NUM = 0x98,
  AEET_DISP_ORIG_CALLED_NUM = 0x99,
  AEET_DISP_CONNECTED_NUM = 0x9A,
  AEET_DISP_TEXT = 0x9E,
  AEET_DISP_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETDispType;

/* Signal types */
typedef enum 
{
  AEET_SIGNAL_NONE,
  
  AEET_SIGNAL_GW_TONE,
    /* GSM/WCDMA Tone Signal */
  
  AEET_SIGNAL_CDMA_TONE,
    /* CDMA Tone signal */
    
  AEET_SIGNAL_CDMA_ISDN,
    /* CDMA ISDN Alerting */
    
  AEET_SIGNAL_CDMA_IS54B,
    /* CDMA IS-54B Alerting */

  AEET_SIGNAL_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETSignalType;

/* GSM/WCDMA Signal Tones */
typedef enum 
{

  AEET_GW_DIAL_TONE_ON,
    /* dial tone */

  AEET_GW_RING_BACK_TONE_ON,
    /* ring back tone */

  AEET_GW_INTERCEPT_TONE_ON,
    /* intercept tone */

  AEET_GW_NW_CONGESTION_TONE_ON,
    /* network congestion */

  AEET_GW_BUSY_TONE_ON,
    /* busy tone */

  AEET_GW_CONFIRM_TONE_ON,
    /* call confirmed tone */

  AEET_GW_ANSWER_TONE_ON,
    /* answer tone */

  AEET_GW_CALL_WAITING_TONE_ON,
    /* call waiting tone */

  AEET_GW_OFF_HOOK_WARNING_TONE_ON,
    /* off hook warning tone */

  AEET_GW_TONES_OFF = 0x3F,
    /* no tones */

  AEET_GW_ALERTING_TONE_OFF = 0x4F,
    /* alerting tone off */

  AEET_GW_TONE_BIG = 0x10000000
    /* Force 32-bit enums */
    
} AEETGWSignalTone;

/* CDMA Signal Tones */
typedef enum 
{

  AEET_CDMA_TONE_DIAL,
    /* Dial tone on */

  AEET_CDMA_TONE_RING,
    /* Ring back tone on */

  AEET_CDMA_TONE_INTERCEPT,
    /* Intercept tone on */
  
  AEET_CDMA_TONE_ABBR_INTERCEPT,
    /* Abbreviated intercept: alternating tones repeated for four seconds */

  AEET_CDMA_TONE_REORDER,
    /* Network congestion (reorder) tone on */

  AEET_CDMA_TONE_ABBR_REORDER,
    /* Abbreviated network congestion (reorder) */

  AEET_CDMA_TONE_BUSY,
    /* Busy tone on */

  AEET_CDMA_TONE_CONFIRM,
    /* Confirm tone on */

  AEET_CDMA_TONE_ANSWER,
    /* Answer tone on */

  AEET_CDMA_TONE_CALLWAIT,
    /* Call waiting tone on */

  AEET_CDMA_TONE_PIP=0x0A,
    /* Pip tone on */

  AEET_CDMA_TONE_OFF=0x3F,
    /* Tones off */

  AEET_CDMA_TONE_BIG=0x10000000
    /* Force 32-bit enums */
 
} AEETCDMASignalTone;

/* CDMA ISDN Alerting signals */
typedef enum 
{
  AEET_CDMA_ISDN_NORMAL,
  /* Normal Alerting */

  AEET_CDMA_ISDN_INTERGROUP,
  /* Intergroup Alerting */

  AEET_CDMA_ISDN_SPECIAL,
  /* Special/Priority Alerting */

  AEET_CDMA_ISDN_RESERVED3,
  /* Reserved (ISDN Alerting pattern 3) */

  AEET_CDMA_ISDN_PING,
  /* "Ping ring": single burst of 500 ms */

  AEET_CDMA_ISDN_RESERVED5,
  /* Reserved (ISDN Alerting pattern 5) */

  AEET_CDMA_ISDN_RESERVED6,
  /* Reserved (ISDN Alerting pattern 6) */

  AEET_CDMA_ISDN_RESERVED7,
  /* Reserved (ISDN Alerting pattern 7) */

  AEET_CDMA_ISDN_OFF=0x0F,
  /* Alerting off */  

  AEET_CDMA_ISDN_BIG=0x10000000
  /* Force 32-bit enums */
} AEETCDMASignalISDN;

/* CDMA IS-54B Alerting Signals */
typedef enum 
{
  AEET_CDMA_IS54B_OFF,
  /* No Tone: off */

  AEET_CDMA_IS54B_L,
  /* Long: (standard alert) */

  AEET_CDMA_IS54B_SS,
  /* Short-short */

  AEET_CDMA_IS54B_SSL,
  /* Short-short-long */

  AEET_CDMA_IS54B_SS2,
  /* Short-short-2 */

  AEET_CDMA_IS54B_SLS,
  /* Short-long-short */

  AEET_CDMA_IS54B_SSSS,
  /* Short-short-short-short */

  AEET_CDMA_IS54B_PBX_L,
  /* PBX Long */

  AEET_CDMA_IS54B_PBX_SS,
  /* PBX Short-short */

  AEET_CDMA_IS54B_PBX_SSL,
  /* PBX Short-short-long */

  AEET_CDMA_IS54B_PBX_SLS,
  /* PBX Short-long-short */

  AEET_CDMA_IS54B_PBX_SSSS,
  /* PBX Short-short-short-short */

  AEET_CDMA_IS54B_PPPP,
  /* Pip-Pip-Pip-Pip */

  AEET_CDMA_IS54B_BIG=0x10000000
  /* Force 32-bit enums */

} AEETCDMASignalIS54B;

/* Signal Alert Pitch */
typedef enum
{
  AEET_PITCH_NONE = -1,
    /* No Pitch information */

  AEET_PITCH_MED,
    /* Medium pitch (standard alert) */

  AEET_PITCH_HIGH,
    /* High pitch */

  AEET_PITCH_LOW,
    /* Low Pitch */

  AEET_PITCH_BIG=0x10000000
    /* Force 32-bit enums */

} AEETSignalPitch;

/* 
  Signal value depends on the signal type and the mapping is as follows.
  Signal Type:                   Signal Value 
  ---------------------------------------------------
  AEET_SIGNAL_GW_TONE:          AEETGWSignalTone
  AEET_SIGNAL_CDMA_TONE:        AEETCDMASignalTone
  AEET_SIGNAL_CDMA_ISDN:        AEETCDMASignalISDN
  AEET_SIGNAL_CDMA_IS54B:       AEETCDMASignalIS54B
*/
typedef union 
{
  AEETGWSignalTone      gw_tone;
  AEETCDMASignalTone    cdma_tone;
  AEETCDMASignalISDN    cdma_isdn;
  AEETCDMASignalIS54B   cdma_is54b;
} AEETSignalValue;

/* Signal structure specify tones to be played
*/
typedef struct
{

  boolean               is_signal_info_avail;
    /* Indicate whether signal information is available. */

  AEETSignalType       signal_type;
    /* Signal type - Tone Signal, ISDN Alerting or IS-54B Alerting */

  AEETSignalValue      signal;
    /* Indicate which Tone signals, ISDN Alerting or IS_54B Alerting */

  AEETSignalPitch      signal_pitch;
    /* Alert pitch - Medium, High or Low 
     * (valid only for AEET_SIGNAL_CDMA_IS54B) */

} AEETSignal;

/* Enumeration of call end statuses
** - i.e. the reason for ending a call.
*/
typedef enum
{

  AEET_CALL_END_NONE=-1,

  /* Common
  */
  AEET_CALL_END_OFFLINE=0,
    /* phone is offline */

  /* CDMA
  */
  AEET_CALL_END_CDMA_LOCK=20,
    /* phone is CDMA locked until power cycle. CDMA only */

  AEET_CALL_END_NO_SRV=21,
    /* phone has no service. CDMA only */

  AEET_CALL_END_FADE=22,
    /* Call has ended abnormally. CDMA only */

  AEET_CALL_END_INTERCEPT=23,
    /* received intercept from BS - orig only. CDMA only */

  AEET_CALL_END_REORDER=24,
    /* received reorder from BS - orig only. CDMA only */

  AEET_CALL_END_REL_NORMAL=25,
    /* received release from BS - no reason given. CDMA only */

  AEET_CALL_END_REL_SO_REJ=26,
    /* received release from BS - SO reject. CDMA only */

  AEET_CALL_END_INCOM_CALL=27,
    /* received incoming call from BS */

  AEET_CALL_END_ALERT_STOP=28,
    /* received alert stop from BS - incom only. CDMA only */

  AEET_CALL_END_CLIENT_END=29,
    /* client ended the call */

  AEET_CALL_END_ACTIVATION=30,
    /* received end activation - OTASP call only. CDMA only */

  AEET_CALL_END_MC_ABORT=31,
    /* MC aborts the origination/conversation. CDMA only */

  AEET_CALL_END_MAX_ACCESS_PROBE=32,
    /* Max access probes transmitted. CDMA only */

  AEET_CALL_END_PSIST_NG=33,
    /* Persistence test failure. FEATURE_JCDMA only. CDMA only */

  AEET_CALL_END_UIM_NOT_PRESENT=34,
    /* RUIM is not present */

  AEET_CALL_END_ACC_IN_PROG=35,
    /* Access attempt already in progress. */

  AEET_CALL_END_ACC_FAIL=36,
    /* Access failure for reason other than the above */

  AEET_CALL_END_RETRY_ORDER=37,
    /* Received retry order - orig only. IS-2000. CDMA only */

  AEET_CALL_END_CCS_NOT_SUPPORTED_BY_BS=38,
    /* Concurrent servive is not supported by base station */

  AEET_CALL_END_NO_RESPONSE_FROM_BS=39,
    /* No response recived from base station */

  AEET_CALL_END_REJECTED_BY_BS=40,
    /* Call rejected by the base station. CDMA only */

  AEET_CALL_END_INCOMPATIBLE=41,
    /* Concurrent services requested were not compatible. CDMA only */

  AEET_CALL_END_ACCESS_BLOCK=42,
    /* Access is blocked by base station. CDMA only */

  AEET_CALL_END_ALREADY_IN_TC=43,
    /* Corresponds to AEET_CALL_ORIG_ERR_ALREADY_IN_TC */

  AEET_CALL_END_EMERGENCY_FLASHED=44,
    /* Call is ended due to Emergency call is flashed over this call. CDMA only */

  AEET_CALL_END_USER_CALL_ORIG_DURING_GPS=45,
    /* Used if CM is ending a GPS call in favor of a user call */

  AEET_CALL_END_USER_CALL_ORIG_DURING_SMS=46,
    /* Used if CM is ending a SMS call in favor of a user call */

  /* GSM/WCDMA
  */
  AEET_CALL_END_LL_CAUSE=100,
    /* rxd a reason from lower layer,look in cc_cause. WCDMA/GSM only*/

  AEET_CALL_END_CONF_FAILED,
    /* call orig request failed. WCDMA/GSM only */

  AEET_CALL_END_INCOM_REJ,
    /* client rejected the incoming call. WCDMA/GSM only */

  AEET_CALL_END_SETUP_REJ,
    /* client rejected the setup_ind. WCDMA/GSM only */

  AEET_CALL_END_NETWORK_END,
    /* network ended the call,look in cc_cause. WCDMA/GSM only*/

  AEET_CALL_END_NO_FUNDS,
    /* GSM/WCDMA only */

  AEET_CALL_END_NO_GW_SRV,
    /* GWM/WCDMA only.  Phone has no service */

  AEET_CALL_END_NO_CDMA_SRV,
    /* 1x only.  Phone has no service */


  /* HDR
  */
  AEET_CALL_END_CD_GEN_OR_BUSY=150,
    /* abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = general or network busy */

  AEET_CALL_END_CD_BILL_OR_AUTH=151,
    /* abort connection setup due to the reception of a ConnectionDeny msg
    ** with deny code = billing or authentication failure */

  AEET_CALL_END_CHG_HDR=152,
    /* change HDR system due to redirection or PRL not preferred */

  AEET_CALL_END_EXIT_HDR=153,
    /* exit HDR  due to redirection or PRL not preferred */

  AEET_CALL_END_HDR_NO_SESSION=154,
    /* no HDR session  */

  AEET_CALL_END_CM_COLLOC_ACQ_FAIL=155,
    /* only for internal CM use -Fail to acquire collocated HDR for
    ** origination*/

  AEET_CALL_END_HDR_ORIG_DURING_GPS_FIX=156,
    /* Used if CM is ending a HDR call orig in favor of GPS fix */

  AEET_CALL_END_HDR_CS_TIMEOUT=157,
  
  AEET_CALL_END_OTASP_SPC_ERR=0x010623e0,
  /* call end due to OTASP service programming code value error  */

  AEET_CALL_END_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETEndStatus;

/* Additional reason for ending/rejecting GSM/WCDMA/CDMA call */
typedef enum {

  AEET_END_UNAVAILABLE = 0x0000,

  /* GSM End Reason */
  AEET_GSMEND_UNASSIGNED_CAUSE = 0x0101,
  AEET_GSMEND_NO_ROUTE_TO_DEST = 0x0103,
  AEET_GSMEND_CHANNEL_UNACCEPTABLE = 0x0106,
  AEET_GSMEND_OPERATOR_DETERMINED_BARRING = 0x0108,
  AEET_GSMEND_NORMAL_CALL_CLEARING = 0x0110,
  AEET_GSMEND_USER_BUSY,
  AEET_GSMEND_NO_USER_RESPONDING,
  AEET_GSMEND_USER_ALERTING_NO_ANSWER,
  AEET_GSMEND_CALL_REJECTED = 0x0115,
  AEET_GSMEND_NUMBER_CHANGED,
  AEET_GSMEND_NON_SELECTED_USER_CLEARING = 0x011A,
  AEET_GSMEND_DESTINATION_OUT_OF_ORDER,
  AEET_GSMEND_INVALID_NUMBER_FORMAT,
  AEET_GSMEND_FACILITY_REJECTED,
  AEET_GSMEND_RESPONSE_TO_STATUS_ENQUIRY,
  AEET_GSMEND_NORMAL_UNSPECIFIED,
  AEET_GSMEND_NO_CIRCUIT_CHANNEL_AVAILABLE = 0x0122,
  AEET_GSMEND_NETWORK_OUT_OF_ORDER = 0x0126,
  AEET_GSMEND_TEMPORARY_FAILURE = 0x0129,
  AEET_GSMEND_SWITCHING_EQUIPMENT_CONGESTION,
  AEET_GSMEND_ACCESS_INFORMATION_DISCARDED,
  AEET_GSMEND_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE,
  AEET_GSMEND_RESOURCES_UNAVAILABLE_UNSPECIFIED = 0x012F,
  AEET_GSMEND_QUALITY_OF_SERVICE_UNAVAILABLE = 0x0131,
  AEET_GSMEND_REQUESTED_FACILITY_NOT_SUBSCRIBED,
  AEET_GSMEND_INCOMING_CALL_BARRED_WITHIN_CUG = 0x0137,
  AEET_GSMEND_BEARER_CAPABILITY_NOT_AUTHORISED = 0x0139,
  AEET_GSMEND_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE,
  AEET_GSMEND_SERVICE_OR_OPTION_NOT_AVAILABLE = 0x013F,
  AEET_GSMEND_BEARER_SERVICE_NOT_IMPLEMENTED = 0x0141,
  AEET_GSMEND_ACM_GEQ_ACMMAX = 0x0144,
  AEET_GSMEND_REQUESTED_FACILITY_NOT_IMPLEMENTED,
  AEET_GSMEND_ONLY_RESTRICTED_DIGITAL_INFO_BC_AVAILABLE,
  AEET_GSMEND_SERVICE_OR_OPTION_NOT_IMPLEMENTED = 0x0146,
  AEET_GSMEND_INVALID_TRANSACTION_ID_VALUE = 0x0151,
  AEET_GSMEND_USER_NOT_MEMBER_OF_CUG = 0x0157,
  AEET_GSMEND_INCOMPATIBLE_DESTINATION,
  AEET_GSMEND_INVALID_TRANSIT_NETWORK_SELECTION = 0x015B,
  AEET_GSMEND_SEMANTICALLY_INCORRECT_MESSAGE = 0x015F,
  AEET_GSMEND_INVALID_MANDATORY_INFORMATION,
  AEET_GSMEND_MESSAGE_TYPE_NON_EXISTENT,
  AEET_GSMEND_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
  AEET_GSMEND_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED,
  AEET_GSMEND_CONDITIONAL_IE_ERROR,
  AEET_GSMEND_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE,
  AEET_GSMEND_RECOVERY_ON_TIMER_EXPIRY,
  AEET_GSMEND_PROTOCOL_ERROR_UNSPECIFIED = 0x016F,
  AEET_GSMEND_INTERWORKING_UNSPECIFIED = 0x017F,

  /* GSM Reject Reason */
  AEET_GSMREJ_IMSI_UNKNOWN_IN_HLR = 0x0202,
  AEET_GSMREJ_ILLEGAL_MS = 0x0203,
  AEET_GSMREJ_IMSI_UNKNOWN_IN_VLR = 0x0204,
  AEET_GSMREJ_IMEI_NOT_ACCEPTED = 0x0205,
  AEET_GSMREJ_ILLEGAL_ME = 0x0206,
  AEET_GSMREJ_GPRS_SERVICES_NOT_ALLOWED = 0x0207,
  AEET_GSMREJ_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 0x0208,
  AEET_GSMREJ_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 0x0209,
  AEET_GSMREJ_IMPLICITLY_DETACHED = 0x020A,
  AEET_GSMREJ_PLMN_NOT_ALLOWED = 0x020B,
  AEET_GSMREJ_LA_NOT_ALLOWED = 0x020C,
  AEET_GSMREJ_NATIONAL_ROAMING_NOT_ALLOWED = 0x020D,
  AEET_GSMREJ_NETWORK_FAILURE = 0x0211,
  AEET_GSMREJ_CONGESTTION = 0x0216,
  AEET_GSMREJ_SERVICE_OPTION_NOT_SUPPORTED = 0x0220,
  AEET_GSMREJ_REQ_SERV_OPT_NOT_SUBSCRIBED = 0x0221,
  AEET_GSMREJ_SERVICE_OPT__OUT_OF_ORDER = 0x0222,
  AEET_GSMREJ_CALL_CANNOT_BE_IDENTIFIED = 0x0226,
  AEET_GSMREJ_NO_PDP_CONTEXT_ACTIVATED = 0x0228,
  AEET_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE = 0x0230,
  AEET_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE = 0x023F,
  AEET_GSMREJ_SEMANTICALLY_INCORRECT_MSG = 0x025F,
  AEET_GSMREJ_INVALID_MANDATORY_INFO = 0x0260,
  AEET_GSMREJ_MESSAGE_TYPE_NON_EXISTANT = 0x0261,
  AEET_GSMREJ_MESSAGE_TYPE_NOT_COMP_PRT_ST = 0x0262,
  AEET_GSMREJ_IE_NON_EXISTANT = 0x0263,

  /* CDMA End Reason */
  AEET_CDMA_REL_NORMAL = 0x0300,
  AEET_CDMA_REL_PWDN,
  AEET_CDMA_REL_SO,
  AEET_CDMA_REL_SRV_INACT,

  AEET_CDMA_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETEndCause;

/* ECT State */
typedef enum 
{
  AEET_ECT_ALERTING,
  AEET_ECT_ACTIVE,
  AEET_ECT_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETECTState;

/* Explicit Call Tranfer info */
typedef struct 
{
  boolean                   present;
    /* TRUE if ECT info present */
  
  AEETECTState             ect_state;
    /* Explicit call transfer state */

} AEETECTInfo;

/* Call error provided along with the AEET_EVENT_CALL_ERROR event.
*/
typedef enum 
{
  AEET_CALL_ERROR_NONE = 0x0000,

  AEET_CALL_ERROR_OFFLINE_S,
    /* Phone is offline */

  AEET_CALL_ERROR_CDMA_LOCK_S,
    /* Phone is CDMA locked */

  AEET_CALL_ERROR_FLASH_S,
    /* Client tried to send a flash while a call that not supporting
    ** flashes is in progress. CDMA only */

  AEET_CALL_ERROR_ORIG_RES_S,
    /* Dialed number is not allowed under the current origination
    ** restriction */

  AEET_CALL_ERROR_NO_FUNDS_S,
    /* Dialed number is not allowed because phone is out of funds. GW only */

  AEET_CALL_ERROR_EMERGENCY_MODE_S,
    /* Client tried to originate a NONE EMERGENCY call while EMERGENCY mode
    ** preference is in effect. CDMA only */

  AEET_CALL_ERROR_IN_USE_S,
    /* Can't perform the command in question while phone is in-use */

  AEET_CALL_ERROR_SRV_TYPE_S,
    /* Client originated with service type that is currently not
    ** supported by the phone/PRL */

  AEET_CALL_ERROR_CALL_TYPE_S,
    /* The command in question is invalid for the call(s) in progress.
    ** e.g. Client answered with inappropriate call type relative to
    ** to the type of the incoming call, etc. */

  AEET_CALL_ERROR_CALL_STATE_S,
    /* The command in question is invalid in the current call state -
    ** e.g. answering a call that was already answered,
    ** ending a call that was already ended, etc. */

  AEET_CALL_ERROR_SRV_STATE_S,
    /* The command in question is invalid in the current Phone serving state-
    ** e.g answering a call but serving state is no service */

  AEET_CALL_ERROR_NO_SO_S,
    /* no valid service found for origination with AUTOMATIC
    ** or CDMA_AUTOMATIC srv type. CDMA only  */

  AEET_CALL_ERROR_ALERT_ANS_BYPASS_S,
    /* Client is not allowed to answer the incoming call. CDMA only
    ** Valid for FEATURE_IS95B_ALERT_ANS_BYPASS */

  AEET_CALL_ERROR_UIM_NOT_PRESENT_S,
    /* UIM not present. Valid for FEATURE_UIM_RUIM */

  AEET_CALL_ERROR_BAD_PRIVACY_PREF_S,
    /* Invalid privacy preference. CDMA only */

  AEET_CALL_ERROR_NO_CALL_ID_S,
    /* No call IDs can be allocated */

  AEET_CALL_ERROR_CALL_ORIG_FAIL_S,
    /* Call origination failed in the lower layer */

  AEET_CALL_ERROR_INVALID_MODE_S,
    /* Recieved the command in a mode different from the phone's
    ** operational mode
    */

  AEET_CALL_ERROR_NO_COLLOC_HDR,
    /* There is no clocated HDR */

  AEET_CALL_ERROR_OTHER,
    /* A problem other than the above was found */

  AEET_CALL_ERROR_CALL_CONTROL_REJECTED,
    /* Call control has rejected the command. */

  AEET_CALL_ERROR_LOWER_LAYER,
    /* Call error from lower layer */

  AEET_CALL_ERROR_INVALID_SIM_STATE,
    /* Call origination failed because the SIM was marked
    ** by the network as invalid for the circuit and/or
    ** packet service domain.
    */  

  AEET_CALL_ERROR_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETCallError;

/* Enumeration of Ph status/errors returned through callback */
typedef enum
{
  AEET_PH_ERROR_NONE = 0x0000,

  AEET_PH_ERROR_IN_USE_S,
    /* Phone is in-use (i.e. in a call) */

  AEET_PH_ERROR_ONLINE_S,
    /* Phone is offline */

  AEET_PH_ERROR_OPRT_MODE_S,
    /* Invalid command or operating mode switch relatively to the current
    ** operating mode - e.g. OFFLINE -> ONLINE */

  AEET_PH_ERROR_MODE_PREF_S,
    /* Invalid mode preference selection relatively to what we
    ** currently have in the PRL - e.g. ANALOG_ONLY when there are no
    ** analog entries in the PRL */

  AEET_PH_ERROR_SYS_PREF_S,
    /* Invalid system preference selection for this device.*/

  AEET_PH_ERROR_SYS_SEL_PREF_S,
    /* Invalid system selection preference for this target/PRL
    ** - e.g. band-class 1 for a PCS only target */

  AEET_PH_ERROR_HYBR_PREF_S,
    /* FEATURE_HDR - Invalid command or hybrid preference selection */

  AEET_PH_ERROR_SUBSCRIPTION_AVAILABLE_S,
    /* Invalid command relative to the current value of subscription
    ** available - e.g. transitioning from subscription available to
    ** subscription available is invalid or issuing a command when
    ** the subscription information is not available. GSM/WCDMA Only. */

  AEET_PH_ERROR_SYS_NOT_IN_UZ,
      /* Indicates that the MS is not in user zone, when
      ** "Avoid system" on user zone was processed. CDMA Only
      */

  AEET_PH_ERROR_OTHER,
    /* A problem other than the above was found */

  AEET_PH_ERROR_CURRENT_NAM_IS_NOT_RUIM_NAM_S,
    /* Current NAM is not RUIM NAM. CDMA Only */

  AEET_PH_ERROR_LINE_SWITCHING,
    /* line switching failed */

  AEET_PH_ERROR_ALS_SIM_WRITE_FAILED,
    /* Failed to write the line selection to SIM */

  AEET_PH_ERROR_BIG = 0x10000000
    /* Force 32-bit enums */

} AEETPhError;

/*---------------------------------------------------------------------------
 * Info passed with each Call Event
 ---------------------------------------------------------------------------*/
typedef struct {

  AEECallDesc             cd; 
  /* Call descriptor of the call */

  AEETNumber             number;
  /* Event specific number info */

  AECHAR                  alpha[AEET_MAX_ALPHA_TAG_LENGTH];
  /* Event specific Alpha numeric tag */

  AEETDispType           disp_type;
  /* display type for display info record */

  AEETSignal             signal;
  /* Alert signal information to be played for the incoming call */

  AEETEndStatus          end_status;
  /* Call end status */

  boolean                 missed;
  /* TRUE, if call was not connected. False, otherwise */

  AEETEndCause           end_additional_info;
  /* Additional Info about end/rejection of call 
     (both WCDMA/GSM and CDMA) */

  AEETECTInfo            ect_info;
  /* State of Explicit Call Transfer */

  AEETCallError          call_error;
  /* Call error associated with CALL_ERROR event */

  uint32                 callops_error;
  /* Supplementary services error */

  AEETCallInfo           call_info;
  /* Call Information */

} AEETCallEventData;

/*---------------------------------------------------------------------------
 * Info passed with each Phone Event
 ---------------------------------------------------------------------------*/
typedef AEETPhInfo AEETPhEventData; /* Same as Phone Information */

/* 
 * The changed field mask is sent along with Event Data on an SS event. 
 * It denotes the fields in AEETSSInfo structure that has changed for 
 * this event.
 */

#define AEET_SS_SRV_STATUS_MASK          AEET_BM(0)
  /* Serving Status - No, limited, full */

#define AEET_SS_SRV_DOMAIN_MASK          AEET_BM(1)
  /* Service Domain */

#define AEET_SS_SRV_CAPABILITY_MASK      AEET_BM(2)
  /* System/s service capability */

#define AEET_SS_SYS_MODE_MASK            AEET_BM(3)
  /* System mode */

#define AEET_SS_ROAM_STATUS_MASK         AEET_BM(4)
  /* Roaming status */

#define AEET_SS_SYS_ID_MASK              AEET_BM(5)
  /* System ID */

#define AEET_SS_SIM_STATE_MASK           AEET_BM(6)

#define AEET_SS_PLMN_FORBIDDEN_MASK      AEET_BM(7)
  /* Forbidden PLMN/System */

#define AEET_SS_MOBILITY_MGMT_MASK       AEET_BM(8)
  /* Mobility Management */

#define AEET_SS_SRV_IND_MASK             AEET_BM(9)

#define AEET_SS_UZ_MASK                  AEET_BM(10)

#define AEET_SS_HDR_SRV_STATUS_MASK      AEET_BM(11)

#define AEET_SS_HDR_ROAM_STATUS_MASK     AEET_BM(12)

#define AEET_SS_P_REV_IN_USE_MASK        AEET_BM(13)

#define AEET_SS_CCS_SUPPORTED_MASK       AEET_BM(14)

  /* Service indicators: This mask indicates a change of service occurred.
  ** It is set even if the service is the same as the previous service. */

/*---------------------------------------------------------------------------
 * Info passed with each Serving System Event
 ---------------------------------------------------------------------------*/
typedef struct {

  uint32                  changed_fields;
  /* Bit mask indicating which of the serving system fields
  ** were changed since last notification. */

  AEETSSInfo             ss_info;
  /* Serving System information */
} AEETSSEventData;

/*---------------------------------------------------------------------------
 * Info passed with each Inband Event
 ---------------------------------------------------------------------------*/
typedef struct {

  AEECallDesc   cd;
  /* call ID */

  uint8         on_length;
  /* DTMF pulse width */

  uint8         off_length;
  /* DTMF inter-digit interval */

  AECHAR        digits[AEET_INBAND_MAX_DIGITS_LENGTH];
  /* DTMF digit buffer */

} AEETInbandEventData;

/*---------------------------------------------------------------------------
 * Info passed with each User Data Event
 ---------------------------------------------------------------------------*/
typedef struct {

  AEECallDesc        cd; 
  /* Call descriptor of the call */

  AEETCallInfo       call_info;
  /* Call Information */

  AEETUserDataInfo   user_data;
  /* User data information */

} AEETUserEventData;

/*---------------------------------------------------------------------------
 * Info passed with each Event
 ---------------------------------------------------------------------------*/
typedef union {

  AEETCallEventData    call;     /* call specific data */

  AEETPhEventData      ph;       /* Phone specific Data */

  AEETSSEventData      ss;       /* Serving System specific data */

  AEETInbandEventData  inb;      /* Inband specific data */

  AEETSuppsEventData   supps;    /* Network Initiated supplementary data AEET_EVENT_SUPPS_NETWORK_IND */

  AEETUserEventData    user;     /* User event data, AEET_EVENT_USER_USER_DATA */

} AEETEventData;

/* Notification data provided along with every event. */
typedef struct {
  AEETEvent        event;
  AEETEventData    event_data;
} AEETNotifyInfo;

typedef struct {
   AEETEvent               evCode;

   IModel                  *pModel;

   AEETCallEventData       call;       /* call specific data */
} AEETCallEvent;

typedef struct {
   AEETEvent               evCode;

   IModel                  *pModel;

   AEETPhEventData         ph;         /* Phone specific Data */
} AEETPhoneEvent;

typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETSSEventData         ss;         /* Serving System specific data */
} AEETSSEvent;

typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETInbandEventData     inb;        /* Inband specific data */
} AEETInbandEvent;

typedef struct {
   AEETEvent               evCode;
   IModel                  *pModel;
   AEETSuppsEventData      supps;      /* Supps specific data */
} AEETSuppsEvent;

typedef struct {
   AEETEvent                  evCode; /* AEET_EVENT_SUPPS_COMMN_STARTED */
   IModel                     *pModel;
   AEETSuppsMessageEventData  supps_started; 
} AEETSuppsMessageEvent;

typedef struct {
   AEETEvent                  evCode;  /* AEET_EVENT_SUPPS_COMMN_ENDED */
   IModel                     *pModel;
   AEETSuppsResultEventData   supps_ended;    
} AEETSuppsResultEvent;

typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETUserEventData       user;       /* User event data */
} AEETUserEvent;

typedef ModelListener CallListener;
typedef ModelListener PhoneListener;

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEETCallState

Description:
   Enumeration of telephone call states.

Definition:
typedef enum
{
   AEET_CALL_STATE_NONE        = 0x00000000,
   AEET_CALL_STATE_IDLE        = 0x00000001,
   AEET_CALL_STATE_ORIG        = 0x00000002,
   AEET_CALL_STATE_INCOM       = 0x00000004,
   AEET_CALL_STATE_CONV        = 0x00000008,
   AEET_CALL_STATE_ENDED       = 0x00000010,
   AEET_CALL_STATE_ONHOLD      = 0x00000020,
   AEET_CALL_STATE_DORMANT     = 0x00000040,
   AEET_CALL_STATE_MULTIPARTY  = 0x00000080,
} AEETCallState;

Members:
   AEET_CALL_STATE_NONE : 
   AEET_CALL_STATE_IDLE : Call is in idle state - i.e. no call.
   AEET_CALL_STATE_ORIG : Call is in origination state
   AEET_CALL_STATE_INCOM : Call is in alerting state
   AEET_CALL_STATE_CONV : Call is in conversation state
   AEET_CALL_STATE_ENDED : Call has already ended
   AEET_CALL_STATE_ONHOLD : Call is on hold
   AEET_CALL_STATE_DORMANT : Call is in dormant mode
   AEET_CALL_STATE_MULTIPARTY : Call is in a Multiparty mode

Comments: 
   None

See Also:
   None

=======================================================================

AEETCallType

Description:
   Enumeration of Telephone call types.

Definition:
typedef enum
{
   AEET_CALL_TYPE_NONE           = 0x00000000,
   AEET_CALL_TYPE_VOICE          = 0x00000001,
   AEET_CALL_TYPE_CS_DATA        = 0x00000002,
   AEET_CALL_TYPE_PS_DATA        = 0x00000004,
   AEET_CALL_TYPE_SMS            = 0x00000008,
   AEET_CALL_TYPE_PD             = 0x00000010,
   AEET_CALL_TYPE_TEST           = 0x00000020,
   AEET_CALL_TYPE_OTAPA          = 0x00000040,
   AEET_CALL_TYPE_STD_OTASP      = 0x00000080,
   AEET_CALL_TYPE_NON_STD_OTASP  = 0x00000100,
   AEET_CALL_TYPE_EMERGENCY      = 0x00000200,
   AEET_CALL_TYPE_VIDEO_TELEPHONE      = 0x00000800
} AEETCallType;

Members:
   AEET_CALL_TYPE_VOICE : Voice call type.
   
   AEET_CALL_TYPE_CS_DATA : CS Data call type - (for modem, fax, etc. calls).
   
   AEET_CALL_TYPE_PS_DATA  : Packet Switched Data call type
   
   AEET_CALL_TYPE_SMS : SMS call type (AEET_SO_SMS and AEET_SO_RS2_SMS)
   
   AEET_CALL_TYPE_PD : Position Determination call type (AEET_SO_RS1_PD and AEET_SO_RS2_PD).
      This call type used to exchange IS-801.
      
   AEET_CALL_TYPE_TEST : Test call type (for AEET_SO_MARKOV_SO54, AEET_SO_LOOPBACK_SO55, etc.).
      NOTE! Answering with this call type is only successful if incoming call is of type TEST
      
   AEET_CALL_TYPE_OTAPA : OTAPA call type (AEET_SO_RS1_OTAPA and AEET_SO_RS2_OTAPA).
      NOTE! Answering with this call type is only successful if incoming call is of type OTAPA
      NOTE! These call types should not be used to originate calls.
      
  AEET_CALL_TYPE_STD_OTASP : Standard OTASP call type.
      This Call type is automatically assigned by system when detecting an origination dial-string 
      satisfies a pre-programmed criteria defined by the operator.

  AEET_CALL_TYPE_NON_STD_OTASP : Non standard OTASP call type.
      This Call type is automatically assigned by system when detecting an origination dial-string 
      satisfies a pre-programmed criteria defined by the operator.

  AEET_CALL_TYPE_EMERGENCY : Emergency call type.
      This Call type is automatically assigned by system when detecting an origination dial-string 
      satisfies a pre-programmed criteria defined by the operator.
      This call type can be used by Clients/applications also to originate calls.
      NOTE! If system assigned this call type, the mode preference is forced to EMERGENCY 
      before origination. If at the end of the call the phone is not in EMERGENCY callback mode,
      the mode is automatically restored to its original selection. Else the EMERGENCY mode stays 
      in effect until changed by a client or device power-cycle
  
  AEET_CALL_TYPE_VIDEO_TELEPHONE : Multimedia call type - Video Telephone call.    
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETSubAddress

Description:
   Telephone subaddress.

Definition:
typedef struct 
{
  boolean   extension_bit;
  uint8     odd_even_ind;
  char      addr[AEET_MAX_SUBADDRESS_DIGITS_LENGTH];
} AEETSubAddress;

Members:
   extension_bit : Extension bit
   odd_even_ind : indicator of even/odd bits.
   addr : NULL terminated array of subaddress digits.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETNumber

Description:
   Defines a telephone number. Used to represent a dialed number or a caller ID number.

Definition:
typedef struct
{
  char                     buf[AEET_MAX_DIGITS_LENGTH]; 
  AEETNumberType           number_type;
  AEETNumberPlan           number_plan;
  AEETPI                   pi;
  AEETSI                   si;
  AEETSubAddress           subaddr;
} AEETNumber;

Members:
   buf : NULL terminated number buffer
   number_type : number type.
   number_plan : Numbering plan.
   pi : Presentation indicator
   si : screening indicator.
   subaddr : subaddress
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETPrivacyPref

Description:
   Voice privacy preference

Definition:
typedef enum
{
  AEET_PRIVACY_PREF_NONE = 0,
  AEET_PRIVACY_PREF_STANDARD,
  AEET_PRIVACY_PREF_ENHANCED
} AEETPrivacyPref;

Members:
  AEET_PRIVACY_PREF_NONE : 
  AEET_PRIVACY_PREF_STANDARD : Standard privacy
  AEET_PRIVACY_PREF_ENHANCED : Enhanced privacy
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETCallProgress

Description:
   Call progress information for an origination request.

Definition:
typedef enum {
  AEET_PROGIND_USER_SUSPENDED = 0,
  AEET_PROGIND_USER_RESUMED,
  AEET_PROGIND_BEARER_CHANGED,
  AEET_PROGIND_REMOTE_ALERT,
  AEET_PROGIND_UNKNOWN,
} AEETCallProgress;

Members:
  AEET_PROGIND_USER_SUSPENDED : user suspended
  AEET_PROGIND_USER_RESUMED : user resumed
  AEET_PROGIND_BEARER_CHANGED : bearer changed
  AEET_PROGIND_REMOTE_ALERT : remote is being alerted.
  AEET_PROGIND_UNKNOWN : unknown progress, default value
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETLineInfo

Description:
   Phone line information.

Definition:
typedef struct {
   uint32   dwMaxSupportedLines;  
   uint32   dwSelectedLineId;     
   boolean  bLineSwitchingEnabled;
} AEETLineInfo;

Members:
   dwMaxSupportedLines : Maximum number of phone lines provisioned.
   dwSelectedLineId : identifies the selected line. The values will be 1 - dwMaxSupportedLines.
   bLineSwitchingEnabled : Whether line switching is enabled.

Comments: 
   None

See Also:
   None

=======================================================================

AEETCallInfo

Description:
   Information about a call.

Definition:
typedef struct {
   AEETCallState          call_state;  
   AEETCallType           call_type;
   AEETSysMode            sys_mode;
   AEETCallDirection      direction;
   uint32                 start_time;
   uint32                 duration;
   AECHAR                 alpha[AEET_MAX_ALPHA_TAG_LENGTH];
   char                   other_party_no[AEET_MAX_DIGITS_LENGTH];
   char                   other_party_subaddr[AEET_MAX_SUBADDRESS_DIGITS_LENGTH];
   AEETNumber             redirect_party_number;
   AEETFwdCugInfo         forward_cug_info;
   AEETCLIR               cm_clir;
   AEETNoCLICause         cause_of_no_cli;
   AEETServiceOption      srv_opt;
   AEETPrivacyPref        privacy_pref;
   boolean                is_privacy;
   boolean                alert_ans_bypass;
   char                   forwarded_from[AEET_MAX_DIGITS_LENGTH];
   char                   forwarded_to[AEET_MAX_NUM_FWDING][AEET_MAX_DIGITS_LENGTH];
   AEETCallProgress       call_progress;
   uint32                 line_id;
} AEETCallInfo;

Members:
   call_state : call state - idle, orig, incom, conv, etc.
   call_type : call type voice, data, otasp, etc.
   sys_mode : active system mode for this call.
   direction : direction of the call mobile originated or terminated.
   start_time : start time of the call (milliseconds since device was powered on)
   duration : duration of call in milliseconds. updated only after the call ends.
   alpha : alpha numeric information received.
   other_party_no : Other party number.
   other_party_subaddr : other party subaddress.
   redirect_party_number : redirecting party number
   forward_cug_info : closed user group information
   cm_clir : CLIR information - CLIR suppression / invocation etc.
   cause_of_no_cli : cause of no caller line identification
   srv_opt : service option
   privacy_pref : user privacy preference
   is_privacy : indicate whether privacy mode is ON.
   alert_ans_bypass : indicate whether the incoming call need to be answered by application.
   forwarded_from : the number forwarded from if applicable.
   forwarded_to : list of numbers forwarded to for a call originating from this MS.
   call_progress : call origination progress information.
   line_id : line id the call is affiliated.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETRadioAccessTech

Description:
   Enumerates the radio access technologies.

Definition:
typedef enum
{
  AEET_RAT_NONE = -1,
  AEET_RAT_GSM_RADIO_ACCESS,
  AEET_RAT_UMTS_RADIO_ACCESS,
} AEETRadioAccessTech;

Members:
   AEET_RAT_NONE : 
   AEET_RAT_GSM_RADIO_ACCESS : GSM radio access technology.
   AEET_RAT_UMTS_RADIO_ACCESS : UMTS radio access technology.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETSignalQuality

Description:
   Enumerates the PLMN's signal quality.

Definition:
typedef enum
{
  AEET_SIGNAL_QUALITY_NONE = -1,                  
  AEET_SIGNAL_QUALITY_HIGH,
  AEET_SIGNAL_QUALITY_LOW,
} AEETSignalQuality;

Members:
   AEET_SIGNAL_QUALITY_NONE : 
   AEET_SIGNAL_QUALITY_HIGH : 
   AEET_SIGNAL_QUALITY_LOW : 
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETSignalStrength

Description:
   Defines the PLMN's signal strength in units that allow direct comparison of 
the signal strength between GSM and WCDMA cells.

Definition:
typedef int32 AEETSignalStrength;

Members:
   None
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETDetailedPLMNInfo

Description:
   Detailed PLMN information.

Definition:
typedef struct 
{
  AEETPLMNID                       plmn;
  AEETRadioAccessTech              rat;
  AEETSrvCapability                plmn_service_capability;
  boolean                           plmn_forbidden;
  AEETDetailedPLMNCat              list_category;
  AEETSignalQuality                signal_quality;
  AEETSignalStrength               signal_strength;
} AEETDetailedPLMNInfo;

Members:
   plmn : network identity
   rat : radio access technology of the PLMN
   plmn_service_capability : indicated the type of service domain the PLMN can provide.
   plmn_forbidden : indicates if this PLMN is forbidden.
   list_category : indicates the type of PLMN
   signal_quality : signal quality of the PLMN
   signal_strength : signal strength of the PLMN
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETDetailedPLMNList

Description:
   Defines the structure for detailed PLMN list.

Definition:
typedef struct 
{
  uint32                           length;
  AEETDetailedPLMNInfo             info[AEET_PLMN_LIST_MAX_LENGTH];
} AEETDetailedPLMNList;

Members:
   length : number of PLMN information elements in the info.
   info : Detailed PLMN info.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETRTREConfig

Description:
   Runtime RUIM Enable (RTRE) configuration.

Definition:
typedef enum
{
  AEET_RTRE_CONFIG_NONE,
  AEET_RTRE_CONFIG_RUIM_ONLY,
  AEET_RTRE_CONFIG_NV_ONLY,
  AEET_RTRE_CONFIG_RUIM_OR_DROP_BACK,
  AEET_RTRE_CONFIG_GSM_SIM,
} AEETRTREConfig;

Members:
  AEET_RTRE_CONFIG_NONE : 
  AEET_RTRE_CONFIG_RUIM_ONLY : RTRE configuration: use R-UIM only
  AEET_RTRE_CONFIG_NV_ONLY : RTRE configuration: use NV only
  AEET_RTRE_CONFIG_RUIM_OR_DROP_BACK : RTRE use R-UIM if available or NV
  AEET_RTRE_CONFIG_GSM_SIM : RTRE use UIM formatted with GSM provisioning data
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETRTREControl

Description:
   Current Runtime RUIM Enable control

Definition:
typedef enum
{

  AEET_RTRE_CONTROL_NONE,

  AEET_RTRE_CONTROL_RUIM,
  AEET_RTRE_CONTROL_NV,
  AEET_RTRE_CONTROL_GSM_SIM,
} AEETRTREControl;

Members:
  AEET_RTRE_CONTROL_NONE : 
  AEET_RTRE_CONTROL_RUIM : RTRE control: use R-UIM
  AEET_RTRE_CONTROL_NV : RTRE control: use NV
  AEET_RTRE_CONTROL_GSM_SIM : RTRE control: use GSM SIM card
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETAOCInfo

Description:
   Advice of Charge information

Definition:
typedef struct 
{
  uint32    ccm;
  uint32    acm;
  uint32    acm_max;
} AEETAOCInfo;

Members:
   ccm : Current call meter
   acm : accumulated call meter
   acm_max : maximum accumulated call meter
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETDDTMPref

Description:
   Enumeration of dedicated data transmission mode preference.

Definition:
typedef enum 
{
  AEET_DDTM_PREF_NONE        = -1,
  AEET_DDTM_PREF_OFF,
  AEET_DDTM_PREF_ON,
  AEET_DDTM_PREF_NO_CHANGE,
} AEETDDTMPref;

Members:
  AEET_DDTM_PREF_OFF : disable data dedicated transmission mode
  AEET_DDTM_PREF_ON : enable data dedicated transmission mode
  AEET_DDTM_PREF_NO_CHANGE : Do not change the DDTM preference
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETPhInfo

Description:
   Describes Phone information.

Definition:
typedef struct {
   boolean                          is_in_use;
   AEETOprtMode                     oprt_mode;
   AEETTestControlType              test_control_type;
   AEETCDMALockMode                 cdma_lock_mode;
   AEETAnsVoicePref                 answer_voice;
   AEETModePref                     mode_pref;
   AEETPrefTerm                     pref_term;
   AEETAcqOrderPref                 acq_order_pref;
   AEETServDomPref                  srv_domain_pref;
   AEETBandPref                     band_pref;
   AEETRoamPref                     roam_pref;
   AEETNAM                          nam_sel;
   AEETNAM                          curr_nam;
   AEETHybrPref                     hybr_pref;
   AEETNwSelModePref                network_sel_mode_pref;
   AEETPLMNID                       plmn;
   AEETUserPLMNList                 user_pref_networks;
   AEETDetailedPLMNList             available_networks;
   boolean                          is_cdma_subscription_available;
   boolean                          is_gw_subscription_available;
   AEETRTREConfig                   rtre_config;
   AEETRTREControl                  rtre_control;
   AEETAOCInfo                      aoc_info;
   AEETDDTMPref                     ddtm_pref;
   AEETLineInfo                     line_info;
} AEETPhInfo;

Members:
   is_in_use : indicate whether phone is currently in use
   oprt_mode : indicate current operating mode
   test_control_type : indicate current test control type.
   cdma_lock_mode : indicate whether phone is CDMA locked until power cycle.
   answer_voice : indicate answer voice preference when active
   mode_pref : indicate current mode preference
   pref_term : indicate the current mode preference span.
   acq_order_pref : Acquisition order preferred by the user/application.
   srv_domain_pref : service domain preferred by the user/application.
   band_pref : indicate current band preference
   roam_pref : indicate current roam preference
   nam_sel : indicate the NAM selection
   curr_nam : the current active NAM - when NAM selection is set to AEET_NAM_AUTO
   hybr_pref : indicate hybrid preference
   network_sel_mode_pref : network selection mode selected by user/application
   plmn : identifier of the PLMN to be selcted manually.
   user_pref_network : user preferred network list
   available_networks : available networks.
   is_cdma_subscription_available : indicate whether the cdma subscription/provisioning
          information is available for the device.
   is_gw_subscription_available : indicate whether the gsm/wcdma subscription/provisioning
          information is available for the device.
   rtre_config : Current Runtime RUIM Enable configuration.
   rtre_control : Current Runtime RUIM Enable control.
   aoc_info : Advice of charge information.
   ddtm_pref : Dedidated data transmission mode preference.
   line_info : phone line information.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETPrefNetworks

Description:
   Defines a list of networks used to program preferred networks.

Definition:
typedef struct {
   AEETUserPLMNList  pref_networks;    
} AEETPrefNetworks;

Members:
   pref_networks : List of PLMNs.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETSystemPreference

Description:
   Data structure used to program the system selection preference.

Definition:
typedef struct {
   AEETModePref                     mode_pref;
   AEETPrefTerm                     pref_term;
   uint32                           pref_duration;
   AEETAcqOrderPref                 acq_order_pref;
   AEETBandPref                     band_pref;
   AEETRoamPref                     roam_pref;
   AEETHybrPref                     hybr_pref;
   AEETServDomPref                  srv_domain_pref;
   AEETNwSelModePref                nw_sel_mode_pref;
   AEETPLMNID                       plmn;
} AEETSystemPreference;

Members:
   mode_pref : mode preference. Set it to AEET_MODE_PREF_NO_CHANGE when not changing.
   pref_term : designate the mode span.
   pref_duration : preference duration in seconds when pref_term is AEET_PREF_TERM_1_CALL_OR_TIME.
   acq_order_pref : acquisition order. Set it to AEET_GW_ACQ_ORDER_PREF_NO_CHANGE when not changing
   band_pref : band preference. Set it to AEET_BAND_PREF_NO_CHANGE when not changing.
   roam_pref : system selection roam preference. Set it to AEET_ROAM_PREF_NO_CHANGE when not changing.
   hybr_pref : system selection hybrid preference. Set it to AEET_HYBR_PREF_NO_CHANGE when not changing.
   srv_domain_pref : service domain being requested. Set it to AEET_SRV_DOMAIN_PREF_NO_CHANGE when not changing.
   nw_sel_mode_pref : whether the network should be selected automatically. Set it to AEET_NETWORK_SEL_MODE_PREF_NO_CHANGE when not changing.
   plmn : identifies the Public Land Mobile Network (PLMN). When not changing MEMSET it to 0xFF. 
         
Comments: 
   None

See Also:
   None

=======================================================================

AEETAvoidSSPref

Description:
   Enumeration to indicate the type of system to be avoided.

Definition:
typedef enum {
  AEET_SS_AVOID_SYS_NONE        = -1,
  AEET_SS_AVOID_SYS_SRV_IF_UZ,
  AEET_SS_AVOID_SYS_SRV,
} AEETAvoidSSPref;

Members:
  AEET_SS_AVOID_SYS_SRV_IF_UZ : Avoid serving system only if MS has currently selected user zone
  AEET_SS_AVOID_SYS_SRV : Avoid serving system
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETDTMFOnLength

Description:
   Enumerates the DTMF off duration.

Definition:
typedef enum
{
  AEET_DTMF_ON_95,
  AEET_DTMF_ON_150,
  AEET_DTMF_ON_200,
  AEET_DTMF_ON_250,
  AEET_DTMF_ON_300,
  AEET_DTMF_ON_350,
  AEET_DTMF_ON_SMS,
} AEETDTMFOnLength;

Members:
  AEET_DTMF_ON_95 : 95 ms recommended pulse width
  AEET_DTMF_ON_150 : 150 ms recommended pulse width
  AEET_DTMF_ON_200 : 200 ms recommended pulse width
  AEET_DTMF_ON_250 : 250 ms recommended pulse width
  AEET_DTMF_ON_300 : 300 ms recommended pulse width
  AEET_DTMF_ON_350 : 350 ms recommended pulse width
  AEET_DTMF_ON_SMS : SMS Tx special pulse width
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETDTMFOffLength

Description:
   Enumerates the DTMF off duration.

Definition:
typedef enum
{
  AEET_DTMF_OFF_60,  
  AEET_DTMF_OFF_100,
  AEET_DTMF_OFF_150,
  AEET_DTMF_OFF_200,
} AEETDTMFOffLength;

Members:
  AEET_DTMF_OFF_60 : 60 ms recommended minimum interdigit interval
  AEET_DTMF_OFF_100 : 100 ms recommended minimum interdigit interval
  AEET_DTMF_OFF_150 : 150 ms recommended minimum interdigit interval
  AEET_DTMF_OFF_200 : 200 ms recommended minimum interdigit interval
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETUserDataInfo

Description:
   Defines a User to User Signaling (UUS) data information

Definition:
typedef struct {
   uint8    type;
   uint16   length;
   uint8    user_data[AEET_MAX_USER_DATA_LENGTH];
} AEETUserDataInfo;

Members:
   type      : User data service is to be activated implicitly or explicitly
   length    : Length of the user data filled in the buffer
   user_data : User data information 

Comments: 
   None

See Also:
   None

=======================================================================

AEETEvent

Description:
   Telephone Event.

Definition:
typedef enum AEETEventEnum
{
  AEET_EVENT_NONE = 0x0000,
  
  AEET_EVENT_CALL_ORIG = 0x0101,
  AEET_EVENT_CALL_ANSWER,
  AEET_EVENT_CALL_END,
  AEET_EVENT_CALL_INCOM,
  AEET_EVENT_CALL_CONNECT,
  AEET_EVENT_CALL_ERROR,
  AEET_EVENT_CALL_OPS_ERROR,
  AEET_EVENT_CALL_OPS,
  AEET_EVENT_CALL_CALLER_ID,
  AEET_EVENT_CALL_DISPLAY,
  AEET_EVENT_CALL_CALLED_PARTY,
  AEET_EVENT_CALL_CONNECTED_NUM,
  AEET_EVENT_CALL_BARRED,
  AEET_EVENT_CALL_IS_WAITING,
  AEET_EVENT_CALL_ON_HOLD,
  AEET_EVENT_CALL_RETRIEVED,
  AEET_EVENT_CALL_ORIG_FWD_STATUS,
  AEET_EVENT_CALL_FORWARDED,
  AEET_EVENT_CALL_BEING_FORWARDED,
  AEET_EVENT_CALL_INCOM_FWD_CALL,
  AEET_EVENT_CALL_RESTRICTED,
  AEET_EVENT_CALL_CUG_INFO_RECEIVED,
  AEET_EVENT_CALL_CNAP_INFO_RECEIVED,
  AEET_EVENT_CALL_EMERGENCY_FLASHED,
  AEET_EVENT_CALL_DEFLECTION,
  AEET_EVENT_CALL_TRANSFERRED,
  AEET_EVENT_CALL_SRV_OPT,
  AEET_EVENT_CALL_PRIVACY,
  AEET_EVENT_CALL_ABRV_ALERT,
  AEET_EVENT_CALL_ABRV_REORDER,
  AEET_EVENT_CALL_ABRV_INTERCEPT,
  AEET_EVENT_CALL_SIGNAL,
  AEET_EVENT_CALL_PROGRESS_INFO_IND,
  AEET_EVENT_CALL_ENTER_DORMANT,
  AEET_EVENT_CALL_EXIT_DORMANT,
  AEET_EVENT_CALL_REDIRECTING_NUMBER,
  AEET_EVENT_CALL_EXT_BRST_INTL, 

  AEET_EVENT_PH_OPRT_MODE = 0x0201,
  AEET_EVENT_PH_SYSTEM_PREF,
  AEET_EVENT_PH_ANSWER_VOICE,
  AEET_EVENT_PH_NAM_SEL,
  AEET_EVENT_PH_CURR_NAM,
  AEET_EVENT_PH_IN_USE_STATE,
  AEET_EVENT_PH_CDMA_LOCK_MODE,
  AEET_EVENT_PH_MAINTREQ,
  AEET_EVENT_PH_STANDBY_SLEEP,
  AEET_EVENT_PH_STANDBY_WAKE,
  AEET_EVENT_PH_INFO_AVAIL,
  AEET_EVENT_PH_SUBSCRIPTION,
  AEET_EVENT_PH_FUNDS_LOW,
  AEET_EVENT_PH_NVRUIM_CONFIG,
  AEET_EVENT_PH_ACM_RESET,
  AEET_EVENT_PH_ACMMAX_SET,
  AEET_EVENT_PH_LINE_INFO,
  
  AEET_EVENT_SS_SRV_CHANGED = 0x0301,
  AEET_EVENT_SS_RSSI,
  AEET_EVENT_SS_HDR_RSSI,
  AEET_EVENT_SS_WLAN_RSSI,

  AEET_EVENT_INBAND_FWD_BURST_DTMF = 0x0401,
  AEET_EVENT_INBAND_FWD_START_DTMF,
  AEET_EVENT_INBAND_FWD_STOP_DTMF,

  AEET_EVENT_SUPPS_NETWORK_IND = 0x701,
  AEET_EVENT_SUPPS_COMMN_STARTED = 0x702,
  AEET_EVENT_SUPPS_COMMN_ENDED = 0x703,


  AEET_EVENT_USER_USER_DATA = 0x901,
  
} AEETEvent;

Members:
  AEET_EVENT_CALL_ORIG : Mobile device originated a call.
  AEET_EVENT_CALL_ANSWER : Incoming call was answered
  AEET_EVENT_CALL_END : In/Orig call was ended
  AEET_EVENT_CALL_INCOM : Received an incoming call
  AEET_EVENT_CALL_CONNECT : In/Orig call connected
  AEET_EVENT_CALL_ERROR : Call Error
  AEET_EVENT_CALL_OPS_ERROR : Call related sups error
  AEET_EVENT_CALL_OPS : phone sent Flash/Flash with Info to BS
  AEET_EVENT_CALL_CALLER_ID : Caller ID info was received from BS
  AEET_EVENT_CALL_DISPLAY : display info was received from BS.
  AEET_EVENT_CALL_CALLED_PARTY : called party info was received from BS.
  AEET_EVENT_CALL_CONNECTED_NUM : connected number info was received
  AEET_EVENT_CALL_BARRED : call barred notification
  AEET_EVENT_CALL_IS_WAITING : call is in waiting notification
  AEET_EVENT_CALL_ON_HOLD : call on hold notification
  AEET_EVENT_CALL_RETRIEVED : call retrieved notification
  AEET_EVENT_CALL_ORIG_FWD_STATUS : call may be forwarded (to forwarding MS)
  AEET_EVENT_CALL_FORWARDED : call forwarded (to forwarding MS)
  AEET_EVENT_CALL_BEING_FORWARDED : call being forwarded (to calling MS)
  AEET_EVENT_CALL_INCOM_FWD_CALL : Incoming forwarded call notification
  AEET_EVENT_CALL_RESTRICTED : call restricted notification
  AEET_EVENT_CALL_CUG_INFO_RECEIVED : call forward CUG notification
  AEET_EVENT_CALL_CNAP_INFO_RECEIVED : caller name info notification
  AEET_EVENT_CALL_EMERGENCY_FLASHED : voice call converted to emergency
  AEET_EVENT_CALL_DEFLECTION : Call deflection notification
  AEET_EVENT_CALL_TRANSFERRED : Call transfer notification
  AEET_EVENT_CALL_SRV_OPT : Service option changed
  AEET_EVENT_CALL_PRIVACY : Privacy mode changed
  AEET_EVENT_CALL_ABRV_ALERT : CMDA/AMPS abbreviated alert.
  AEET_EVENT_CALL_ABRV_REORDER : AMPS abbreviated reorder.
  AEET_EVENT_CALL_ABRV_INTERCEPT : AMPS abbreviated intercept.
  AEET_EVENT_CALL_SIGNAL : signal info was received from BS
  AEET_EVENT_CALL_PROGRESS_INFO_IND : call origination progress indication
  AEET_EVENT_CALL_ENTER_DORMANT : call entered dormant state
  AEET_EVENT_CALL_EXIT_DORMANT : call exit dormant state
  AEET_EVENT_CALL_REDIRECTING_NUMBER : received Redirecting Number Info Record.
  AEET_EVENT_CALL_EXT_BRST_INTL: This event is sent when mobile station receives a
                                 EXTENDED BURST TYPE INTERNATIONAL data burst message, 
                                 which conveys the charge rate info for the call.

  AEET_EVENT_PH_OPRT_MODE : Operating mode was changed
  AEET_EVENT_PH_SYSTEM_PREF : System preference changed
  AEET_EVENT_PH_ANSWER_VOICE : Answer voice as data was changed.
  AEET_EVENT_PH_NAM_SEL : NAM selection was changed
  AEET_EVENT_PH_CURR_NAM : Current NAM was changed.
  AEET_EVENT_PH_IN_USE_STATE : In use state was changed
  AEET_EVENT_PH_CDMA_LOCK_MODE : CDMA lock mode was changed.
  AEET_EVENT_PH_MAINTREQ : CDMA maintenance required command.
  AEET_EVENT_PH_STANDBY_SLEEP : Entering powerdown sleep mode
  AEET_EVENT_PH_STANDBY_WAKE : Exiting powerdown sleep mode
  AEET_EVENT_PH_INFO_AVAIL : Phone information is now available
  AEET_EVENT_PH_SUBSCRIPTION : Subscription info changed
  AEET_EVENT_PH_FUNDS_LOW : Funds running low.
  AEET_EVENT_PH_NVRUIM_CONFIG : RTRE(Runtime R-UIM Enable) configuration changed.
  AEET_EVENT_PH_ACM_RESET : Accumulated Call Meter was reset
  AEET_EVENT_PH_ACMMAX_SET : Accumulated Call Meter was set
  AEET_EVENT_PH_LINE_INFO : line information changed.

  AEET_EVENT_SS_SRV_CHANGED : Serving system information changed
  AEET_EVENT_SS_RSSI : RSSI changed
  AEET_EVENT_SS_HDR_RSSI : HDR RSSI changed
  AEET_EVENT_SS_WLAN_RSSI : WLAN RSSI changed
  
  AEET_EVENT_INBAND_FWD_BURST_DTMF : received a CDMA-burst DTMF
  AEET_EVENT_INBAND_FWD_START_DTMF : received a start continous DTMF order
  AEET_EVENT_INBAND_FWD_STOP_DTMF : received a stop continous DTMF order

  AEET_EVENT_SUPPS_NETWORK_IND : Network initiated supplementary service transaction.
  AEET_EVENT_SUPPS_COMMN_STARTED : Outgoing supplementary service transaction, network communication started.
  AEET_EVENT_SUPPS_COMMN_ENDED : An ongoing transaction ended
      
  AEET_EVENT_USER_USER_DATA : 0x901,  received user data 

Comments: 
   None

See Also:
   None

=======================================================================

AEETSignal

Description:
   Signal structure specify tones to be played.

Definition:
typedef struct
{

  boolean              is_signal_info_avail;
  AEETSignalType       signal_type;
  AEETSignalValue      signal;
  AEETSignalPitch      signal_pitch;
} AEETSignal;

Members:
   is_signal_info_avail : Indicate whether signal information is available.
   signal_type : signal type - tone signal, ISDN alerting or IS-54B alerting.
   signal : signal information.
   signal_pitch : alert pitch - medium, high or low
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETEndStatus

Description:
   Enumeration of call end status. The reason for ending a call.

Definition:
typedef enum
{
  AEET_CALL_END_NONE=-1,
  AEET_CALL_END_OFFLINE=0,
  AEET_CALL_END_CDMA_LOCK=20,
  AEET_CALL_END_NO_SRV=21,
  AEET_CALL_END_FADE=22,
  AEET_CALL_END_INTERCEPT=23,
  AEET_CALL_END_REORDER=24,
  AEET_CALL_END_REL_NORMAL=25,
  AEET_CALL_END_REL_SO_REJ=26,
  AEET_CALL_END_INCOM_CALL=27,
  AEET_CALL_END_ALERT_STOP=28,
  AEET_CALL_END_CLIENT_END=29,
  AEET_CALL_END_ACTIVATION=30,
  AEET_CALL_END_MC_ABORT=31,
  AEET_CALL_END_MAX_ACCESS_PROBE=32,
  AEET_CALL_END_PSIST_NG=33,
  AEET_CALL_END_UIM_NOT_PRESENT=34,
  AEET_CALL_END_ACC_IN_PROG=35,
  AEET_CALL_END_ACC_FAIL=36,
  AEET_CALL_END_RETRY_ORDER=37,
  AEET_CALL_END_CCS_NOT_SUPPORTED_BY_BS=38,
  AEET_CALL_END_NO_RESPONSE_FROM_BS=39,
  AEET_CALL_END_REJECTED_BY_BS=40,
  AEET_CALL_END_INCOMPATIBLE=41,
  AEET_CALL_END_ACCESS_BLOCK=42,
  AEET_CALL_END_ALREADY_IN_TC=43,
  AEET_CALL_END_EMERGENCY_FLASHED=44,
  AEET_CALL_END_USER_CALL_ORIG_DURING_GPS=45,
  AEET_CALL_END_USER_CALL_ORIG_DURING_SMS=46,
  AEET_CALL_END_LL_CAUSE=100,
  AEET_CALL_END_CONF_FAILED,
  AEET_CALL_END_INCOM_REJ,
  AEET_CALL_END_SETUP_REJ,
  AEET_CALL_END_NETWORK_END,
  AEET_CALL_END_NO_FUNDS,
  AEET_CALL_END_NO_GW_SRV,
  AEET_CALL_END_NO_CDMA_SRV,
  AEET_CALL_END_CD_GEN_OR_BUSY=150,
  AEET_CALL_END_CD_BILL_OR_AUTH=151,
  AEET_CALL_END_CHG_HDR=152,
  AEET_CALL_END_EXIT_HDR=153,
  AEET_CALL_END_HDR_NO_SESSION=154,
  AEET_CALL_END_CM_COLLOC_ACQ_FAIL=155,
  AEET_CALL_END_HDR_ORIG_DURING_GPS_FIX=156,
  AEET_CALL_END_HDR_CS_TIMEOUT=157,
  AEET_CALL_END_OTASP_SPC_ERR=0x010623e0,
} AEETEndStatus;

Members:
  AEET_CALL_END_NONE : 
  AEET_CALL_END_OFFLINE : phone is offline
  AEET_CALL_END_CDMA_LOCK : phone is CDMA locked until power cycle
  AEET_CALL_END_NO_SRV : phone has no service
  AEET_CALL_END_FADE : Call has ended abnormally
  AEET_CALL_END_INTERCEPT : received intercept from Base Station - orig only
  AEET_CALL_END_REORDER : received reorder from Base Station - orig only

  AEET_CALL_END_REL_NORMAL : received release from BS - no reason given
  AEET_CALL_END_REL_SO_REJ : received release from BS - SO reject

  AEET_CALL_END_INCOM_CALL : received incoming call from BS

  AEET_CALL_END_ALERT_STOP : received alert stop from BS - incom only.

  AEET_CALL_END_CLIENT_END : client ended the call

  AEET_CALL_END_ACTIVATION : received end activation - OTASP call only.

  AEET_CALL_END_MC_ABORT : system aborts the origination/conversation.

  AEET_CALL_END_MAX_ACCESS_PROBE : Maximum access probes transmitted.

  AEET_CALL_END_PSIST_NG : Persistence test failure

  AEET_CALL_END_UIM_NOT_PRESENT : RUIM is not present

  AEET_CALL_END_ACC_IN_PROG : Access attempt already in progress.

  AEET_CALL_END_ACC_FAIL : Access failure for reason other than the above

  AEET_CALL_END_RETRY_ORDER : Received retry order - orig only.

  AEET_CALL_END_CCS_NOT_SUPPORTED_BY_BS : Concurrent servive is not supported by base station

  AEET_CALL_END_NO_RESPONSE_FROM_BS : No response recived from base station

  AEET_CALL_END_REJECTED_BY_BS : Call rejected by the base station

  AEET_CALL_END_INCOMPATIBLE : Concurrent services requested were not compatible.

  AEET_CALL_END_ACCESS_BLOCK : Access is blocked by base station.

  AEET_CALL_END_ALREADY_IN_TC : Corresponds to AEET_CALL_ORIG_ERR_ALREADY_IN_TC

  AEET_CALL_END_EMERGENCY_FLASHED : Call is ended due to Emergency call is flashed over this call.

  AEET_CALL_END_USER_CALL_ORIG_DURING_GPS : Ending a GPS session in favor of a user call

  AEET_CALL_END_USER_CALL_ORIG_DURING_SMS : Ending a SMS call in favor of a user call

  AEET_CALL_END_LL_CAUSE : received a reason from lower layer,look in cc_cause.

  AEET_CALL_END_CONF_FAILED : call orig request failed.

  AEET_CALL_END_INCOM_REJ : client rejected the incoming call.

  AEET_CALL_END_SETUP_REJ : client rejected the setup_ind.

  AEET_CALL_END_NETWORK_END : network ended the call,look in cc_cause.

  AEET_CALL_END_NO_FUNDS : No funds.

  AEET_CALL_END_NO_GW_SRV : Phone has no service GSM/WCDMA

  AEET_CALL_END_NO_CDMA_SRV : Phone has no service 1x.


  AEET_CALL_END_CD_GEN_OR_BUSY : abort connection setup due to the reception of a ConnectionDeny message
    with deny code = general or network busy

  AEET_CALL_END_CD_BILL_OR_AUTH : abort connection setup due to the reception of a ConnectionDeny message
    with deny code = billing or authentication failure

  AEET_CALL_END_CHG_HDR : change HDR system due to redirection or PRL not preferred

  AEET_CALL_END_EXIT_HDR : exit HDR  due to redirection or PRL not preferred

  AEET_CALL_END_HDR_NO_SESSION : no HDR session

  AEET_CALL_END_CM_COLLOC_ACQ_FAIL : Fail to acquire collocated HDR for origination

  AEET_CALL_END_HDR_ORIG_DURING_GPS_FIX : Ending a HDR call orig in favor of GPS fix

  AEET_CALL_END_HDR_CS_TIMEOUT : HDR CS Timeout
  
  AEET_CALL_END_OTASP_SPC_ERR : call end due to OTASP service programming code value error
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETEndCause

Description:
   Additional reason for ending/rejecting call

Definition:
typedef enum {

  AEET_END_UNAVAILABLE = 0x0000,

  AEET_GSMEND_UNASSIGNED_CAUSE = 0x0101,
  AEET_GSMEND_NO_ROUTE_TO_DEST = 0x0103,
  AEET_GSMEND_CHANNEL_UNACCEPTABLE = 0x0106,
  AEET_GSMEND_OPERATOR_DETERMINED_BARRING = 0x0108,
  AEET_GSMEND_NORMAL_CALL_CLEARING = 0x0110,
  AEET_GSMEND_USER_BUSY,
  AEET_GSMEND_NO_USER_RESPONDING,
  AEET_GSMEND_USER_ALERTING_NO_ANSWER,
  AEET_GSMEND_CALL_REJECTED = 0x0115,
  AEET_GSMEND_NUMBER_CHANGED,
  AEET_GSMEND_NON_SELECTED_USER_CLEARING = 0x011A,
  AEET_GSMEND_DESTINATION_OUT_OF_ORDER,
  AEET_GSMEND_INVALID_NUMBER_FORMAT,
  AEET_GSMEND_FACILITY_REJECTED,
  AEET_GSMEND_RESPONSE_TO_STATUS_ENQUIRY,
  AEET_GSMEND_NORMAL_UNSPECIFIED,
  AEET_GSMEND_NO_CIRCUIT_CHANNEL_AVAILABLE = 0x0122,
  AEET_GSMEND_NETWORK_OUT_OF_ORDER = 0x0126,
  AEET_GSMEND_TEMPORARY_FAILURE = 0x0129,
  AEET_GSMEND_SWITCHING_EQUIPMENT_CONGESTION,
  AEET_GSMEND_ACCESS_INFORMATION_DISCARDED,
  AEET_GSMEND_REQUESTED_CIRCUIT_CHANNEL_NOT_AVAILABLE,
  AEET_GSMEND_RESOURCES_UNAVAILABLE_UNSPECIFIED = 0x012F,
  AEET_GSMEND_QUALITY_OF_SERVICE_UNAVAILABLE = 0x0131,
  AEET_GSMEND_REQUESTED_FACILITY_NOT_SUBSCRIBED,
  AEET_GSMEND_INCOMING_CALL_BARRED_WITHIN_CUG = 0x0137,
  AEET_GSMEND_BEARER_CAPABILITY_NOT_AUTHORISED = 0x0139,
  AEET_GSMEND_BEARER_CAPABILITY_NOT_PRESENTLY_AVAILABLE,
  AEET_GSMEND_SERVICE_OR_OPTION_NOT_AVAILABLE = 0x013F,
  AEET_GSMEND_BEARER_SERVICE_NOT_IMPLEMENTED = 0x0141,
  AEET_GSMEND_ACM_GEQ_ACMMAX = 0x0144,
  AEET_GSMEND_REQUESTED_FACILITY_NOT_IMPLEMENTED,
  AEET_GSMEND_ONLY_RESTRICTED_DIGITAL_INFO_BC_AVAILABLE,
  AEET_GSMEND_SERVICE_OR_OPTION_NOT_IMPLEMENTED = 0x0146,
  AEET_GSMEND_INVALID_TRANSACTION_ID_VALUE = 0x0151,
  AEET_GSMEND_USER_NOT_MEMBER_OF_CUG = 0x0157,
  AEET_GSMEND_INCOMPATIBLE_DESTINATION,
  AEET_GSMEND_INVALID_TRANSIT_NETWORK_SELECTION = 0x015B,
  AEET_GSMEND_SEMANTICALLY_INCORRECT_MESSAGE = 0x015F,
  AEET_GSMEND_INVALID_MANDATORY_INFORMATION,
  AEET_GSMEND_MESSAGE_TYPE_NON_EXISTENT,
  AEET_GSMEND_MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
  AEET_GSMEND_IE_NON_EXISTENT_OR_NOT_IMPLEMENTED,
  AEET_GSMEND_CONDITIONAL_IE_ERROR,
  AEET_GSMEND_MESSAGE_NOT_COMPATIBLE_WITH_PROTOCOL_STATE,
  AEET_GSMEND_RECOVERY_ON_TIMER_EXPIRY,
  AEET_GSMEND_PROTOCOL_ERROR_UNSPECIFIED = 0x016F,
  AEET_GSMEND_INTERWORKING_UNSPECIFIED = 0x017F,

  AEET_GSMREJ_IMSI_UNKNOWN_IN_HLR = 0x0202,
  AEET_GSMREJ_ILLEGAL_MS = 0x0203,
  AEET_GSMREJ_IMSI_UNKNOWN_IN_VLR = 0x0204,
  AEET_GSMREJ_IMEI_NOT_ACCEPTED = 0x0205,
  AEET_GSMREJ_ILLEGAL_ME = 0x0206,
  AEET_GSMREJ_GPRS_SERVICES_NOT_ALLOWED = 0x0207,
  AEET_GSMREJ_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED = 0x0208,
  AEET_GSMREJ_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK = 0x0209,
  AEET_GSMREJ_IMPLICITLY_DETACHED = 0x020A,
  AEET_GSMREJ_PLMN_NOT_ALLOWED = 0x020B,
  AEET_GSMREJ_LA_NOT_ALLOWED = 0x020C,
  AEET_GSMREJ_NATIONAL_ROAMING_NOT_ALLOWED = 0x020D,
  AEET_GSMREJ_NETWORK_FAILURE = 0x0211,
  AEET_GSMREJ_CONGESTTION = 0x0216,
  AEET_GSMREJ_SERVICE_OPTION_NOT_SUPPORTED = 0x0220,
  AEET_GSMREJ_REQ_SERV_OPT_NOT_SUBSCRIBED = 0x0221,
  AEET_GSMREJ_SERVICE_OPT__OUT_OF_ORDER = 0x0222,
  AEET_GSMREJ_CALL_CANNOT_BE_IDENTIFIED = 0x0226,
  AEET_GSMREJ_NO_PDP_CONTEXT_ACTIVATED = 0x0228,
  AEET_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MIN_VALUE = 0x0230,
  AEET_GSMREJ_RETRY_UPON_ENTRY_INTO_A_NEW_CELL_MAX_VALUE = 0x023F,
  AEET_GSMREJ_SEMANTICALLY_INCORRECT_MSG = 0x025F,
  AEET_GSMREJ_INVALID_MANDATORY_INFO = 0x0260,
  AEET_GSMREJ_MESSAGE_TYPE_NON_EXISTANT = 0x0261,
  AEET_GSMREJ_MESSAGE_TYPE_NOT_COMP_PRT_ST = 0x0262,
  AEET_GSMREJ_IE_NON_EXISTANT = 0x0263,

  AEET_CDMA_REL_NORMAL = 0x0300,
  AEET_CDMA_REL_PWDN,
  AEET_CDMA_REL_SO,
  AEET_CDMA_REL_SRV_INACT,

} AEETEndCause;

Members:
   None
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETECTState

Description:
   ECT state

Definition:
typedef enum {
  AEET_ECT_ALERTING,
  AEET_ECT_ACTIVE
} AEETECTState;

Members:
   None
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETECTInfo

Description:
   Explicit call transfer information.

Definition:
typedef struct 
{
  boolean                   present;
  AEETECTState             ect_state;
} AEETECTInfo;

Members:
   present : TRUE if ECT information is present
   ect_state : Explicit call transfer state
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETCallError

Description:
   Call error provided along with the AEET_EVENT_CALL_ERROR event.

Definition:
typedef enum 
{
  AEET_CALL_ERROR_NONE = AEE_SUCCESS,
  AEET_CALL_ERROR_OFFLINE_S,
  AEET_CALL_ERROR_CDMA_LOCK_S,
  AEET_CALL_ERROR_FLASH_S,
  AEET_CALL_ERROR_ORIG_RES_S,
  AEET_CALL_ERROR_NO_FUNDS_S,
  AEET_CALL_ERROR_EMERGENCY_MODE_S,
  AEET_CALL_ERROR_IN_USE_S,
  AEET_CALL_ERROR_SRV_TYPE_S,
  AEET_CALL_ERROR_CALL_TYPE_S,
  AEET_CALL_ERROR_CALL_STATE_S,
  AEET_CALL_ERROR_SRV_STATE_S,
  AEET_CALL_ERROR_NO_SO_S,
  AEET_CALL_ERROR_ALERT_ANS_BYPASS_S,
  AEET_CALL_ERROR_UIM_NOT_PRESENT_S,
  AEET_CALL_ERROR_BAD_PRIVACY_PREF_S,
  AEET_CALL_ERROR_NO_CALL_ID_S,
  AEET_CALL_ERROR_CALL_ORIG_FAIL_S,
  AEET_CALL_ERROR_INVALID_MODE_S,
  AEET_CALL_ERROR_NO_COLLOC_HDR,
  AEET_CALL_ERROR_OTHER,
  AEET_CALL_ERROR_CALL_CONTROL_REJECTED,
  AEET_CALL_ERROR_LOWER_LAYER
} AEETCallError;

Members:
  AEET_CALL_ERROR_NONE : No errors.

  AEET_CALL_ERROR_OFFLINE_S : Phone is offline

  AEET_CALL_ERROR_CDMA_LOCK_S : Phone is CDMA locked

  AEET_CALL_ERROR_FLASH_S : Client tried to send a flash while a call that not supporting
    flashes is in progress. CDMA only

  AEET_CALL_ERROR_ORIG_RES_S : Dialed number is not allowed under the current origination restriction

  AEET_CALL_ERROR_NO_FUNDS_S : Dialed number is not allowed because phone is out of funds.

  AEET_CALL_ERROR_EMERGENCY_MODE_S : Client tried to originate a NONE EMERGENCY call while EMERGENCY mode
    preference is in effect.

  AEET_CALL_ERROR_IN_USE_S : Can't perform the command in question while phone is in-use

  AEET_CALL_ERROR_SRV_TYPE_S : Client originated with service type that is currently not
    supported by the phone/PRL

  AEET_CALL_ERROR_CALL_TYPE_S : The command in question is invalid for the call(s) in progress.
    e.g. Client answered with inappropriate call type relative to the type of the incoming call, etc.

  AEET_CALL_ERROR_CALL_STATE_S : The command in question is invalid in the current call state -
    e.g. answering a call that was already answered, ending a call that was already ended, etc.

  AEET_CALL_ERROR_SRV_STATE_S : The command in question is invalid in the current Phone serving state -
    ** e.g answering a call but serving state is no service.

  AEET_CALL_ERROR_NO_SO_S : no valid service found for origination with AUTOMATIC or CDMA_AUTOMATIC srv type.

  AEET_CALL_ERROR_ALERT_ANS_BYPASS_S : Client is not allowed to answer the incoming call.

  AEET_CALL_ERROR_UIM_NOT_PRESENT_S : UIM not present.

  AEET_CALL_ERROR_BAD_PRIVACY_PREF_S : Invalid privacy preference.

  AEET_CALL_ERROR_NO_CALL_ID_S : No call descriptors can be allocated

  AEET_CALL_ERROR_CALL_ORIG_FAIL_S : Call origination failed in the lower layer

  AEET_CALL_ERROR_INVALID_MODE_S : Recieved the command in a mode different from the phone's operational mode

  AEET_CALL_ERROR_NO_COLLOC_HDR : There is no collocated HDR.

  AEET_CALL_ERROR_OTHER : A problem other than the above occured.

  AEET_CALL_ERROR_CALL_CONTROL_REJECTED : Call control has rejected the command.

  AEET_CALL_ERROR_LOWER_LAYER : Call error from lower layer
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETCallEventData

Description:
   Information passed with each call event.

Definition:
typedef struct {
  AEECallDesc             cd; 
  AEETNumber             number;
  AECHAR                  alpha[AEET_MAX_ALPHA_TAG_LENGTH];
  AEETDispType           disp_type;
  AEETSignal             signal;
  AEETEndStatus          end_status;
  boolean                 missed;
  AEETEndCause           end_additional_info;
  AEETECTInfo            ect_info;
  AEETCallError          call_error;
  uint32                 callops_error;
  AEETCallInfo           call_info;
} AEETCallEventData;

Members:
   cd : call descriptor of the call
   number : Remote phone number associated with the call
   alpha : Alpha numeric tag associated with the call
   disp_type : display type for display information record
   signal : Alert signal information to be played for the incoming call
   end_status : call end status
   missed : TRUE if call was not connected. FALSE otherwise.
   end_additional_info : Additional information about end/rejection of call (both WCDMA/GSM and CDMA)
   ect_info : state of explicit call transfer
   call_error : Call error associated with AEET_EVENT_CALL_ERROR
   callops_error : call related supplementary services error
   call_info : Call information
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETPhEventData

Description:
   Information passed with each phone event.

Definition:
typedef AEETPhInfo AEETPhEventData;

Members:
      
Comments: 
   None

See Also:
   AEETPhInfo

=======================================================================

AEETSSEventData

Description:
   Serving system specific event data.

Definition:
typedef struct {
  uint32                 changed_fields;
  AEETSSInfo             ss_info;
} AEETSSEventData;

Members:
   changed_fields : bitmask of ss_info fields changed in this event.
          (eg : AEET_SS_SRV_STATUS_MASK, AEET_SS_SRV_DOMAIN_MASK).
   ss_info : serving system information.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETInbandEventData

Description:
   Inband specific event data.

Definition:
typedef struct {
  AEECallDesc   cd;
  uint8         on_length;
  uint8         off_length;
  AECHAR        digits[AEET_INBAND_MAX_DIGITS_LENGTH];
} AEETInbandEventData;

Members:
   cd : call descriptor on which the inband data is received.
   on_length : on duration.
   off_length : off duration.
   digits : burst of digits received.
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETUserEventData

Description:
   Information passed with each user data event

Definition:
typedef struct {
  AEECallDesc        cd; 
  AEETCallInfo       call_info;
  AEETUserDataInfo   user_data;
} AEETUserEventData;

Members:
   cd :  Call descriptor of the call on which user data is received
   call_info : Call information
   user_data : User data information

Comments: 
   None

See Also:
   AEETUserDataInfo

=======================================================================

AEETEventData

Description:
   Details of event_data given as part of AEETNotifyInfo

Definition:
typedef union {

  AEETCallEventData    call;

  AEETPhEventData      ph;

  AEETSSEventData      ss;

  AEETInbandEventData  inb;

  AEETSuppsEventData   supps;

  AEETUserEventData    user;

} AEETEventData;

Members:
   call : call specific data. With events AEET_EVENT_CALL_*
   ph   : phone specific data. With events AEET_EVENT_PH_*
   ss   : serving system specific data. With events AEET_EVENT_SS_*
   inb  : inband specific data. With events AEET_EVENT_INBAND_*
   supps: Network initiated supplementary service data. With event AEET_EVENT_SUPPS_NETWORK_IND
   user : User data: with event AEET_EVENT_USER_USER_DATA

Comments: 
   None

See Also:
   None

=======================================================================

AEETNotifyInfo

Description:
   Data associated with EVT_NOTIFY. Applications registered for AEET_NMASK_*
on IPhoneNotifier will receive this data structure as pData member of AEENotify.

Definition:
typedef struct {
  AEETEvent        event;
  AEETEventData    event_data;
} AEETNotifyInfo;

Members:
   event : Events from IPhoneNotifier.
   event_data : data associated with event.
      
Comments: 
   None

See Also:
   AEENotify

=======================================================================

AEETCallEvent

Description:
   Specifies the data structure to Listener on ITelephone with AEET_EVENT_CALL_* events.

Definition:
typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETCallEventData       call;
} AEETCallEvent;

Members:
   evCode   : The inband event.
   pModel   : Instance of IModel on the event generator.
   call     : Data associated with the call events.

Comments: 
   None

See Also:
   None

=======================================================================

AEETPhoneEvent

Description:
   Specifies the data structure to Listener on ITelephone with AEET_EVENT_PH_* events.

Definition:
typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETPhEventData         ph;
} AEETPhoneEvent;

Members:
   evCode   : The inband event.
   pModel   : Instance of IModel on the event generator.
   ph       : Data associated with the phone events.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSSEvent

Description:
   Specifies the data structure to Listener on ITelephone with AEET_EVENT_SS_* events.

Definition:
typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETSSEventData         ss;
} AEETSSEvent;

Members:
   evCode   : The serving system event.
   pModel   : Instance of IModel on the event generator.
   ss       : Data associated with the serving system events.

Comments: 
   None

See Also:
   None

=======================================================================

AEETInbandEvent

Description:
   Specifies the data structure to Listener on ITelephone with AEET_EVENT_INBAND_* events.

Definition:
typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETInbandEventData     inb;
} AEETInbandEvent;

Members:
   evCode   : The inband event.
   pModel   : Instance of IModel on the event generator.
   inb      : Data associated with the inband events.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsEvent

Description:
   Specifies the data structure to Listener on ITelephone with AEET_EVENT_SUPPS_* events.

Definition:
typedef struct {
   AEETEvent               evCode;
   IModel                  *pModel;
   AEETSuppsEventData      supps;
} AEETSuppsEvent;

Members:
   evCode   : The inband event.
   pModel   : Instance of IModel on the event generator.
   supps    : Data associated with the Supps  events.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsMessageEvent

Description:
   Specifies the data structure to Listener on ITelephone with 
AEET_EVENT_SUPPS_COMMN_STARTED event.

Definition:
typedef struct {
   AEETEvent                  evCode;

   IModel                     *pModel;

   AEETSuppsMessageEventData  supps_started; 

} AEETSuppsMessageEvent;

Members:
   evCode   : The AEET_EVENT_SUPPS_COMMN_STARTED event.
   pModel   : Instance of IModel on the event generator.
   supps_started : Associated data.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSuppsResultEvent

Description:
   Specifies the data structure to Listener on ITelephone with 
AEET_EVENT_SUPPS_COMMN_ENDED event.

Definition:
typedef struct {
   AEETEvent                  evCode;

   IModel                     *pModel;

   AEETSuppsResultEventData   supps_ended;    

} AEETSuppsResultEvent;

Members:
   evCode   : The AEET_EVENT_SUPPS_COMMN_ENDED event.
   pModel   : Instance of IModel on the event generator.
   supps_ended : Associated data.

Comments: 
   None

See Also:
   None

=======================================================================

AEETUserEvent

Description:
   Specifies the data structure to Listener on ITelephone with 
   AEET_EVENT_USER_USER_DATA event.

Definition:
typedef struct {
   AEETEvent               evCode;
   
   IModel                  *pModel;

   AEETUserEventData       user;
} AEETUserEvent;

Members:
   evCode   : The inband event.
   pModel   : Instance of IModel on the event generator.
   user     : Data associated with the user events.

Comments: 
   None

See Also:
   None

=======================================================================

AEETCalls

Description:
   Buffer used to specify multiple calls. 

Definition:
typedef struct {
   uint32      dwCount;
   AEECallDesc calls[1]; 
} AEETCalls;

Members:
   dwCount  : number of call descriptors in the array.
   calls    : call descriptor array.

Comments: 
   None

See Also:
   ITELEPHONE_GetCalls()

=======================================================================

AEETTimeAndZone

Description:
   Define a structure for the Time Zone and Universal Time mobility
   management information. 

Definition:
   typedef struct 
   {
     AEEDateTime                              time;
     AEETTimeZone                             time_zone;
   } AEETTimeAndZone2;

Members:
   time
   time_zone

Comments: 
   None

See Also:
   None
   
=======================================================================

AEECallDesc

Description:
   Specifies a call descriptor.

Definition:
typedef int AEECallDesc;

Members:
   None

Comments: 
   None

See Also:

=======================================================================

AEETNetworkType

Description:
   Enumeration that defines the type of network list to be requestd. The
   requested list will either contain the preferred networks or all available
   networks.

Definition:
typedef enum
{
   AEET_NETWORK_LIST_AVAILABLE,
   AEET_NETWORK_LIST_USER_PREFERRED
} AEETNetworkType;

Members:
   AEET_NETWORK_LIST_AVAILABLE      : Available network list.
   AEET_NETWORK_LIST_USER_PREFERRED : Preferred network list from SIM/USIM

Comments: 
   None

See Also:
   ITELEPHONE_GetNetworks()

=======================================================================

AEETNWCodingType

Description:
   Enumeration that defines the type of coding scheme for network name
   specification.

Definition:
typedef enum
{
   AEET_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET,
   AEET_NW_NAME_CODING_SCHEME_UCS2,
} AEETNWCodingType;

Members:
   AEET_NW_NAME_CODING_SCHEME_CELL_BROADCAST_DEFAULT_ALPHABET      
                         : Character set GSM 03.38. Stored as
                         single byte (8 bits) per character.
   AEET_NW_NAME_CODING_SCHEME_UCS2 
                         : Unicode character set ISO/IEC10646. 
                         Stored as two consecutive bytes (16bits) 
                         per character.

Comments: 
   None

See Also:
   None

=======================================================================

AEETNWName

Description:
   Defines a structure describing Network name.

Definition:

typedef struct {
   uint8                      length;
   AEETNWCodingType           coding_scheme;
   boolean                    add_country_initials;
   uint8                      name[AEET_MAX_NW_NAME_CHARS];
} AEETNWName;

Members:
   length         : number of characters in the name.
   coding_scheme  : identifies character set. See AEETNWCodingType.
   add_country_initials : when TRUE, application need to add the 
                    letters for Country's initials and a separator (eg: a space)
                    to the name.
   name           : name of the network encoded according to coding_scheme.

Comments: 
   None

See Also:
   None

=======================================================================

AEETMMInfo

Description:
   Definition of a structure that contains the currently registered network's
mobility management information (i.e. network's name, date, time and
time zone).  All fields in this structure are optional.  If the
"available" flag is true, then the corresponding data field contains
valid information.  If the "available" flag is false, then the
corresponding data field should be ignored.

Definition:

typedef struct {
   unsigned                           plmn_avail:1;
   unsigned                           full_name_avail:1;
   unsigned                           short_name_avail:1;
   unsigned                           time_and_time_zone_avail:1;
   unsigned                           time_zone_avail:1;
   unsigned                           lsa_identity_avail:1;
   unsigned                           daylight_saving_adj:2;

   AEETPLMNID                       plmn;
   AEETNWName                       full_name;
   AEETNWName                       short_name;

   AEETTimeAndZone                  time_and_time_zone;
   AEETTimeZone                     time_zone;
   AEETLSAIdentity                  lsa_identity;
} AEETMMInfo;

Members:
   Flags that indicate whether the particular mm information is
   available.
      plmn_avail, full_name_avail, short_name_avail,
      time_and_time_zone_avail, time_zone_avail, lsa_identity_avail.

   plmn                   : plmn identity of the network
   full_name              : full name of the network
   short_name             : short name of the network
   time_and_time_zone     : universal time and time zone information from network.
   time_zone              : time zone is an offset from Universal time 
                            (i.e. difference between local time and Universal time) 
                            in increments of 15 minutes. time zone is compensated
                            for day light savings time. (see daylight_saving_adj)
   lsa_identity           : Localized Service Area identity
   daylight_saving_adj    : possible values : 0 - 4. daylight savings adjustment in 
                            the 60 minute increments.
                            daylight savings is already built into the time_zone. 
                            To obtain the actual time zone, subtract the daylight savings 
                            adjustment. 

Comments: 
   None

See Also:
   ITELEPHONE_GetServingSystemInfo()

=======================================================================

AEETSSInfo

Description:
   Structure provides the telephone Serving System information.

Definition:
typedef struct {
   AEETSrvStatus              srv_status;
   AEETSrvDomain              srv_domain;
   AEETSrvCapability          srv_capability;
   AEETSysMode                sys_mode;
   AEETRoamStatus             roam_status;
   AEETSysID                  sys_id;
   uint16                     rssi;
   boolean                    hdr_hybrid;
   uint8                      tssinfo_version;
   AEETSrvStatus              hdr_srv_status;
   AEETRoamStatus             hdr_roam_status;
   uint16                     hdr_rssi;
   uint16                     wlan_rssi;
   AEETSIMState               sim_state;
   boolean                    plmn_forbidden;
   AEETMMInfo                 mm_information;
   AEETSysBandClass           band_class;
   AEETSysBlocks              block_or_system;
   boolean                    is_registered;
   int32                      uz_id;
   AEETUZName                 uz_name;
   boolean                    ccs_supported;
   uint8                      p_rev_in_use;
} AEETSSInfo;

Members:
   srv_status        : Service status (SERVICE/NO SERVICE) of the system
   srv_domain        : service domain registered on the system
   srv_capability    : system's service capability
   sys_mode          : system's mode
   roam_status       : Indicate current roaming status
   sys_id            : ID of the selected system
   rssi              : RSSI in positive dBm i.e 75 means -75dBm.
                     :  AMSP :      -110 to -89 dBm
                     :  800 CDMA :  -105 to -90 dBm
                     :  1900 CDMA : -108 to -93 dBm
                     :  GSM/WCDMA : -105 to -60 dBm
   hdr_hybrid        : system is operating in hybrid mode with HDR (1xEvDo)
   tssinfo_version   : Version of AEETSSInfo structure. Starting version is 0. Version is 
                       revised to be 1 with the introduction of wlan_rssi, ccs_supported
                       and p_rev_in_use members.
   hdr_srv_status    : indicates current HDR service status 
   hdr_roam_status   : indicates current hybrid HDR roaming status
   hdr_rssi          : HDR RSSI in positive dBm i.e. 75 means -75dBm
                     : range is -105 to -90 dBm
   wlan_rssi         : WLAN RSSI in positive dBm i.e. 75 means -75dBm. Valid only if 
                       tssinfo_version is 1.
   sim_state         : Current state of SIM card
   plmn_forbidden    : indicates whether the PLMN is forbidden
   mm_information    : Mobility management information i.e. network name,
                     : date, time and time zone of registered network
   band_class        : cellular or PCS band class
   block_or_system   : PCS frequency block : A to F
                     : Cellular system A or B
   is_registered     : is mobile registered on the current system
   uz_id             : User zone identificaion of the selected user zone
   uz_name           : User zone name of the selected user zone
   ccs_supported     : is concurrent services supported.  Valid only if 
                       tssinfo_version is 1.
   p_rev_in_use      : 8  bit p_rev in use the lessor of BS p_rev and mobile p_rev. 
                       Valid only if tssinfo_version is 1.

Comments: 
   None

See Also:
   ITELEPHONE_GetServingSystemInfo()

=======================================================================

AEETAnsVoicePref

Description:
   Preference to specify how the next incoming call will be answered.

Definition:
typedef enum
{
  AEET_ANSWER_VOICE_AS_VOICE,
  AEET_ANSWER_VOICE_AS_FAX_ONCE,
  AEET_ANSWER_VOICE_AS_FAX_ALWAYS,
  AEET_ANSWER_VOICE_AS_MODEM_ONCE,
  AEET_ANSWER_VOICE_AS_MODEM_ALWAYS
} AEETAnsVoicePref;

Members:
  AEET_ANSWER_VOICE_AS_VOICE : Answer voice call as voice (default setting).
  AEET_ANSWER_VOICE_AS_FAX_ONCE : Answer next incoming voice call as fax once. This will automatically reset to default setting after the call is disconnected or there is no call in next 10 minutes.
  AEET_ANSWER_VOICE_AS_FAX_ALWAYS : Answer incoming voice call as fax always. 
  AEET_ANSWER_VOICE_AS_MODEM_ONCE : Answer incoming voice call as modem call once. This will automatically reset to default setting after the call is disconnected or there is no call in next 10 minutes.
  AEET_ANSWER_VOICE_AS_MODEM_ALWAYS : Answer incoming voice call as modem always. 
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETNAM

Description:
   Identifies Number assignment module.

Definition:
typedef enum
{
   AEET_NAM_1,
   AEET_NAM_2,
   AEET_NAM_3,
   AEET_NAM_4
} AEETNAM;

Members:
  AEET_NAM_1 : NAM 1
  AEET_NAM_2 : NAM 2
  AEET_NAM_3 : NAM 3
  AEET_NAM_4 : NAM 4
      
Comments: 
   None

See Also:
   None

=======================================================================

AEETOprtMode

Description:
   Enumerates phone operating mode.

Definition:
typedef enum
{
   AEET_OPRT_MODE_PWROFF,
   AEET_OPRT_MODE_FTM,
   AEET_OPRT_MODE_OFFLINE,
   AEET_OPRT_MODE_OFFLINE_AMPS,
   AEET_OPRT_MODE_OFFLINE_CDMA,
   AEET_OPRT_MODE_ONLINE,
   AEET_OPRT_MODE_LPM,
   AEET_OPRT_MODE_RESET,
   AEET_OPRT_MODE_NET_TEST_GW
} AEETOprtMode;

Members:
  AEET_OPRT_MODE_PWROFF : Phone is powering off. When modem and microprocessor are shutting down.
  AEET_OPRT_MODE_FTM : Phone is in factory test mode. Where modem and radio are active for factory programming.
  AEET_OPRT_MODE_OFFLINE : Phone is in offline mode. Where modem is active but not radio.
  AEET_OPRT_MODE_OFFLINE_AMPS : Phone is offline analog. Where modem is active but not radio.
  AEET_OPRT_MODE_OFFLINE_CDMA : phone is offline cdma. Where modem is active but not radio.
  AEET_OPRT_MODE_ONLINE : phone is online. Where modem and radio is actively performing transmit and receive.
  AEET_OPRT_MODE_LPM : Phone is in Low Power mode. Where the modem, transmitter and receiver are off but microprocessor is On.

Comments: 
   None

See Also:
   None

=======================================================================

AEETUserPLMNList

Description:
   Defines list of user preferred/controlled networks.

Definition:
typedef struct
{
   byte              length;
   AEETUserPLMNInfo  info[AEET_USER_PPLMN_LIST_MAX_LENGTH];
} AEETUserPLMNList;

Members:
   length : number of elements in the info array.
   info : information identifying networks.
   
Comments: 
   None

See Also:
   None

=======================================================================

AEETUserPLMNInfo

Description:
   Information regarding user preferred/controlled network.

Definition:
typedef struct
{
   AEETPLMNID           plmn;
   AEETAccessTech       access_tech;
} AEETUserPLMNInfo;

Members:
   plmn : PLMN ID.
   access_tech : Radio access technology. ( one or more of AEET_SYS_ACT_GSM_MASK, AEET_SYS_ACT_GSM_COMPACT_MASK, AEET_SYS_ACT_UMTS_MASK ).

Comments: 
   None

See Also:
   None

=======================================================================

AEETAccessTech

Description:
   Defines a mask for radio access technology.

Definition:
typedef uint16 AEETAccessTech;

Members:
   AEET_SYS_ACT_GSM_MASK : GSM access technology
   AEET_SYS_ACT_GSM_COMPACT_MASK : GSM compact access technology
   AEET_SYS_ACT_UMTS_MASK : UMTS radio access technology

Comments: 
   None

See Also:
   None

=======================================================================

AEETPLMNID

Description:
   Identifies Public Land Mobile Network (PLMN). A PLMN ID is defined within
a GSM network to be the combiniation of mobile country code(MCC) and mobile network code(MNC).
PLMN ID is stored in 3 octets in the format shown below :

                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|


Definition:
typedef struct 
{
  byte     identity[3];
} AEETPLMNID;

Members:
   identity : octets identifying PLMN.

Comments: 
   None

See Also:
   None

=======================================================================

AEETSrvStatus

Description:
   Enumeration of service status.  Indicates whether service is 
   available or not available.

Definition:
typedef enum
{ 
   AEET_SRV_STATUS_NO_SRV,
   AEET_SRV_STATUS_LIMITED,
   AEET_SRV_STATUS_SRV,
   AEET_SRV_STATUS_LIMITED_REGIONAL,
   AEET_SRV_STATUS_PWR_SAVE
} AEETSrvStatus;

Members:
   AEET_SRV_STATUS_NO_SRV : No service                             
   AEET_SRV_STATUS_LIMITED : Limited service                        
   AEET_SRV_STATUS_SRV : Service available                      
   AEET_SRV_STATUS_LIMITED_REGIONAL : Limited regional service               
   AEET_SRV_STATUS_PWR_SAVE : MS is in power save or deep sleep      

Comments: 
   None

See Also:
   None   
   
=======================================================================

AEETSrvDomain

Description: 
   The following  enumeration defines type of service.

Definition:
typedef enum
{
   AEET_SRV_DOMAIN_NO_SRV,
   AEET_SRV_DOMAIN_CS_ONLY,
   AEET_SRV_DOMAIN_PS_ONLY,
   AEET_SRV_DOMAIN_CS_PS
} AEETSrvDomain;

Members:
   AEET_SRV_DOMAIN_NO_SRV : No service                             
   AEET_SRV_DOMAIN_CS_ONLY : Circuit Switched Only Capable          
   AEET_SRV_DOMAIN_PS_ONLY : Packet Switched Only Capable            
   AEET_SRV_DOMAIN_CS_PS : Circuit and Packet Switched Capable    

Comments: 
   None

See Also:
   None
  
======================================================================= 

AEETSrvCapability

Description:
   The following enumeration defines the service capabilities of a network.

Definition:
typedef enum
{
   AEET_SRV_CAPABILITY_NOT_AVAILABLE,
   AEET_SRV_CAPABILITY_CS_SERVICE_ONLY,
   AEET_SRV_CAPABILITY_PS_SERVICE_ONLY,
   AEET_SRV_CAPABILITY_CS_PS_SERVICE

} AEETSrvCapability;

Members:
   AEET_SRV_CAPABILITY_NOT_AVAILABLE : Network can not provide service                                   
   AEET_SRV_CAPABILITY_CS_SERVICE_ONLY : Network is capable of providing Circuit Switched service          
   AEET_SRV_CAPABILITY_PS_SERVICE_ONLY : Network is capable of providing Packet Switched service           
   AEET_SRV_CAPABILITY_CS_PS_SERVICE : Network is capable of providing Circuit & Packet Switched service 

Comments: 
   None

See Also:
   None
   
=======================================================================  

AEETSysMode

Description:
   Enumerates Modem system mode.

Definition:
typedef enum
{
   AEET_SYS_MODE_NO_SRV,
   AEET_SYS_MODE_AMPS,
   AEET_SYS_MODE_CDMA,
   AEET_SYS_MODE_GSM,
   AEET_SYS_MODE_HDR,
   AEET_SYS_MODE_WCDMA,
   AEET_SYS_MODE_GPS,
   AEET_SYS_MODE_WLAN,
   AEET_SYS_MODE_GW
} AEETSysMode;

Members:
   AEET_SYS_MODE_NO_SRV : Phone modem is off.
   AEET_SYS_MODE_AMPS : Analog Phone System(AMPS) mode.
   AEET_SYS_MODE_CDMA : Code Division Multiple Access(CDMA) mode.
   AEET_SYS_MODE_GSM : Global System for Mobile Communications(GSM) mode.
   AEET_SYS_MODE_HDR : CDMA High Data Rage(HDR) mode.
   AEET_SYS_MODE_WCDMA : Wideband CDMA mode.
   AEET_SYS_MODE_GPS : Global Positioning System(GPS) mode.
   AEET_SYS_MODE_WLAN : Wireless LAN mode.
   AEET_SYS_MODE_GW : GSM and WCDMA mode.

Comments: 
   None

See Also:
   None

=======================================================================

AEETRoamStatus

Description: 
   Type for roaming statuses.  The values for these enumerates were derived
   from the roaming display indicators describe   in 3GPP2 C.R1001-A.  
   Please note that the actual value of the enumerate is not consistent with 
   the value listed in th   e standard (e.g. the value of AEET_ROAM_STATUS_OFF 
   and AEET_ROAM_STATUS_ON are reversed from the standard.

Definition:

typedef enum
{
   AEET_ROAM_STATUS_OFF,
   AEET_ROAM_STATUS_ON,
   AEET_ROAM_STATUS_BLINK,
   AEET_ROAM_STATUS_OUT_OF_NEIGHBORHOOD,
   AEET_ROAM_STATUS_OUT_OF_BLDG,
   AEET_ROAM_STATUS_PREF_SYS,
   AEET_ROAM_STATUS_AVAIL_SYS,
   AEET_ROAM_STATUS_ALLIANCE_PARTNER,
   AEET_ROAM_STATUS_PREMIUM_PARTNER,
   AEET_ROAM_STATUS_FULL_SVC,
   AEET_ROAM_STATUS_PARTIAL_SVC,
   AEET_ROAM_STATUS_BANNER_ON,
   AEET_ROAM_STATUS_BANNER_OFF
} AEETRoamStatus;

Members:
   AEET_ROAM_STATUS_OFF : Roaming Indicator off    
   AEET_ROAM_STATUS_ON : Roaming Indicator on                        
   AEET_ROAM_STATUS_BLINK : Roaming Indicator flashing                  
   AEET_ROAM_STATUS_OUT_OF_NEIGHBORHOOD : Out of neighborhood                         
   AEET_ROAM_STATUS_OUT_OF_BLDG : Out of building                             
   AEET_ROAM_STATUS_PREF_SYS : Roaming - preferred system                  
   AEET_ROAM_STATUS_AVAIL_SYS : Roaming - available system                  
   AEET_ROAM_STATUS_ALLIANCE_PARTNER : Roaming - alliance partner                  
   AEET_ROAM_STATUS_PREMIUM_PARTNER : Roaming - premium partner                   
   AEET_ROAM_STATUS_FULL_SVC : Roaming - full service functionality        
   AEET_ROAM_STATUS_PARTIAL_SVC : Roaming - partial service functionality     
   AEET_ROAM_STATUS_BANNER_ON : Roaming banner on                           
   AEET_ROAM_STATUS_BANNER_OFF : Roaming banner off                          

Comments: 
   None

See Also:
   None

======================================================================= 

AEETCDMALockMode

Description:
   Enumeration of phone CDMA lock modes.

Definition:
typedef enum
{
   AEET_CDMA_LOCK_MODE_OFF,
   AEET_CDMA_LOCK_MODE_ON
} AEETCDMALockMode;

Members:
   AEET_CDMA_LOCK_MODE_OFF : phone is not CDMA locked.
   AEET_CDMA_LOCK_MODE_ON : phone is CDMA locked until power-cycle.

Comments: 
   None

See Also:
   None

=======================================================================

AEETModePref

Description:
   Enumerates the modem prefrence for an access technology.

Definition:
typedef enum
{
   AEET_MODE_PREF_AMPS_ONLY,
   AEET_MODE_PREF_DIGITAL_ONLY,
   AEET_MODE_PREF_AUTOMATIC,
   AEET_MODE_PREF_EMERGENCY,
   AEET_MODE_PREF_CDMA_ONLY,
   AEET_MODE_PREF_HDR_ONLY,
   AEET_MODE_PREF_CDMA_AMPS_ONLY,
   AEET_MODE_PREF_GPS_ONLY,
   AEET_MODE_PREF_GSM_ONLY,
   AEET_MODE_PREF_WCDMA_ONLY,
   AEET_MODE_PREF_PERSISTENT,
   AEET_MODE_PREF_NO_CHANGE,
   AEET_MODE_PREF_CDMA_HDR_ONLY,
   AEET_MODE_PREF_WLAN_ONLY,
   AEET_MODE_PREF_GSM_WCDMA_ONLY
} AEETModePref;

Members:
   AEET_MODE_PREF_AMPS_ONLY : Limit service to analog only.
   AEET_MODE_PREF_DIGITAL_ONLY : Service is limited to digital (CDMA, HDR, GSM, WCDMA) only.
   AEET_MODE_PREF_AUTOMATIC : determine the mode automatically.
   AEET_MODE_PREF_EMERGENCY : Emergency mode. Where only emergency connections are allowed
   AEET_MODE_PREF_CDMA_ONLY : Limit service to CDMA only.
   AEET_MODE_PREF_HDR_ONLY : Limit service to HDR only.
   AEET_MODE_PREF_CDMA_AMPS_ONLY : Limit service to CDMA and AMS only.
   AEET_MODE_PREF_GPS_ONLY : Service is limited to GPS only.
   AEET_MODE_PREF_GSM_ONLY : Service is limited to GSM only.
   AEET_MODE_PREF_WCDMA_ONLY : Service is limted to WCDMA only.
   AEET_MODE_PREF_PERSISTENT : Return to persistent(provisioned) mode preference.
   AEET_MODE_PREF_NO_CHANGE : Do not change the mode preference.
   AEET_MODE_PREF_CDMA_HDR_ONLY : Limit service to CDMA and HDR modes only
   AEET_MODE_PREF_WLAN_ONLY : Limit service to WLAN only.
   AEET_MODE_PREF_GSM_WCDMA_ONLY : Service is limited to GSM and WCDMA only.
Comments: 
   None

See Also:
   AEETPrefTerm

=======================================================================

AEETPrefTerm

Description:
   Specifies the span for AEETModePref.

Definition:
typedef enum
{
   AEET_PREF_TERM_PERMANENT,
   AEET_PREF_TERM_PWR_CYCLE,
   AEET_PREF_TERM_1_CALL,
   AEET_PREF_TERM_1_CALL_OR_TIME
} AEETPrefTerm;

Members:
   AEET_PREF_TERM_PERMANENT : Mode preference is persistent.
   AEET_PREF_TERM_PWR_CYCLE : Mode preference is valid until next power cycle.
   AEET_PREF_TERM_1_CALL : Mode prefernce is valid for a single call or power-cycle.
   AEET_PREF_TERM_1_CALL_OR_TIME : Until the end of next call, specified timeout or power-cycle.

Comments: 
   None

See Also:
   None

=======================================================================

AEETAcqOrderPref

Description:
   Enumeration of GSM/WCDMA acquisition order.

Definition:
typedef enum
{
   AEET_GW_ACQ_ORDER_PREF_AUTOMATIC,
   AEET_GW_ACQ_ORDER_PREF_GSM_WCDMA,
   AEET_GW_ACQ_ORDER_PREF_WCDMA_GSM,
   AEET_GW_ACQ_ORDER_PREF_NO_CHANGE,
} AEETAcqOrderPref

Members:
   AEET_GW_ACQ_ORDER_PREF_AUTOMATIC : Determine the mode from the provisioned Preferred Roaming List (PRL).
   AEET_GW_ACQ_ORDER_PREF_GSM_WCDMA : Acquisition order is first GSM followed by WCDMA.
   AEET_GW_ACQ_ORDER_PREF_WCDMA_GSM : Acquisition order is first WCDMA followed by GSM.
   AEET_GW_ACQ_ORDER_PREF_NO_CHANGE : GSM/WCDMA acquisition order unchanged. Used with IPHONECTL_SetSystemPreference().

Comments: 
   None

See Also:
   None

=======================================================================

AEETServDomPref

Description:
   Enumeration defining the preferred service domain.

Definition:
typedef enum
{
  AEET_SRV_DOMAIN_PREF_CS_ONLY,
  AEET_SRV_DOMAIN_PREF_PS_ONLY,
  AEET_SRV_DOMAIN_PREF_CS_PS,
  AEET_SRV_DOMAIN_PREF_ANY,
  AEET_SRV_DOMAIN_PREF_NO_CHANGE,
  AEET_SRV_DOMAIN_PREF_PS_ATTACH,
  AEET_SRV_DOMAIN_PREF_PS_DETACH,
} AEETServDomPref;

Members:
   AEET_SRV_DOMAIN_PREF_CS_ONLY : Prefer circuit switched only
   AEET_SRV_DOMAIN_PREF_PS_ONLY : Prefer packet switched only
   AEET_SRV_DOMAIN_PREF_CS_PS : Prefer circuit and packet switched.
   AEET_SRV_DOMAIN_PREF_ANY : Any domain will do. No preference
   AEET_SRV_DOMAIN_PREF_NO_CHANGE : Used with IPHONECTL_SetSystemPrefernce() to leave service domain preference unchanged.
   AEET_SRV_DOMAIN_PREF_PS_ATTACH : Attach on demand for PS
   AEET_SRV_DOMAIN_PREF_PS_DETACH : Detach on demand for PS.

Comments: 
   None

See Also:
   None

=======================================================================

AEETBandPref

Description:
   Enumeration of band preferences.

Definition:
typedef enum
{
  AEET_BAND_PREF_BC0_A,
  AEET_BAND_PREF_BC0_B,
  AEET_BAND_PREF_BC0,
  AEET_BAND_PREF_BC1,
  AEET_BAND_PREF_BC3,
  AEET_BAND_PREF_BC4,
  AEET_BAND_PREF_BC5,
  AEET_BAND_PREF_GSM_DCS_1800,
  AEET_BAND_PREF_GSM_EGSM_900,
  AEET_BAND_PREF_GSM_PGSM_900,
  AEET_BAND_PREF_BC6,
  AEET_BAND_PREF_BC7,
  AEET_BAND_PREF_BC8,
  AEET_BAND_PREF_BC9,
  AEET_BAND_PREF_BC10,
  AEET_BAND_PREF_BC11,
  AEET_BAND_PREF_GSM_450,
  AEET_BAND_PREF_GSM_480,
  AEET_BAND_PREF_GSM_750,
  AEET_BAND_PREF_GSM_850,
  AEET_BAND_PREF_GSM_RGSM_900,
  AEET_BAND_PREF_GSM_PCS_1900,
  AEET_BAND_PREF_WCDMA_I_IMT_2000,
  AEET_BAND_PREF_WCDMA_II_PCS_1900,
  AEET_BAND_PREF_WCDMA_III_1700,
  AEET_BAND_PREF_WCDMA_V_850,
  AEET_BAND_PREF_WCDMA_VI_800,
  AEET_BAND_PREF_WLAN_US_2400,
  AEET_BAND_PREF_WLAN_JP_2400,
  AEET_BAND_PREF_ANY=0x3FFFFFFF,
  AEET_BAND_PREF_NO_CHANGE
} AEETBandPref;

Members:
  AEET_BAND_PREF_BC0_A : Acquire band class 0, A-side systems only
  AEET_BAND_PREF_BC0_B : Acquire band class 0, B-side systems only
  AEET_BAND_PREF_BC0 : Acquire band class 0 systems only
  AEET_BAND_PREF_BC1 : Acquire band class 1 systems only
  AEET_BAND_PREF_BC3 : Acquire band class 3 systems only
  AEET_BAND_PREF_BC4 : Acquire band class 4 systems only
  AEET_BAND_PREF_BC5 : Acquire band class 5 systems only
  AEET_BAND_PREF_GSM_DCS_1800 : Acquire GSM DCS systems only
  AEET_BAND_PREF_GSM_EGSM_900 : Acquire GSM Extended GSM 900 systems only
  AEET_BAND_PREF_GSM_PGSM_900 : Acquire GSM Primary GSM 900 systems only
  AEET_BAND_PREF_BC6 : Acquire band class 6 systems only
  AEET_BAND_PREF_BC7 : Acquire band class 7 systems only
  AEET_BAND_PREF_BC8 : Acquire band class 8 systems only
  AEET_BAND_PREF_BC9 : Acquire band class 9 systems only
  AEET_BAND_PREF_BC10 : Acquire band class 10 systems only
  AEET_BAND_PREF_BC11 : Acquire band class 11 systems only
  AEET_BAND_PREF_GSM_450 : Acquire GSM 450 systems only
  AEET_BAND_PREF_GSM_480 : Acquire GSM 480 systems only
  AEET_BAND_PREF_GSM_750 : Acquire GSM 750 systems only
  AEET_BAND_PREF_GSM_850 : Acquire GSM 850 systems only
  AEET_BAND_PREF_GSM_RGSM_900 : Acquire GSM Railway GSM 900 systems only
  AEET_BAND_PREF_GSM_PCS_1900 : Acquire GSM PCS systems only
  AEET_BAND_PREF_WCDMA_I_IMT_2000 : Acquire WCDMA IMT 2000 systems only
  AEET_BAND_PREF_WCDMA_II_PCS_1900 : Acquire WCDMA PCS systems only
  AEET_BAND_PREF_WCDMA_III_1700 : Acquire WCDMA 1700 systems only
  AEET_BAND_PREF_WCDMA_V_850 : Acquire WCDMA 850 systems only
  AEET_BAND_PREF_WCDMA_VI_800 : Acquire WCDMA 800 systems only
  AEET_BAND_PREF_WLAN_US_2400 : Acquire WLAN 2.4GHz systems only
  AEET_BAND_PREF_WLAN_JP_2400 : Acquire WLAN 2.4GHz with spectrum inversion systems only
  AEET_BAND_PREF_ANY : Acquire any band class systems
  AEET_BAND_PREF_NO_CHANGE : Do not change band preference (Used with IPHONECTL_SetSystemPreference()).

Comments: 
   None

See Also:
   None

=======================================================================

AEETRoamPref

Description:
   Enumeration of system selection roam preference

Definition:
typedef enum
{
   AEET_ROAM_PREF_HOME,
   AEET_ROAM_PREF_AFFIL,
   AEET_ROAM_PREF_ANY,
   AEET_ROAM_PREF_NO_CHANGE
} AEETRoamPref;

Members:
   AEET_ROAM_PREF_HOME : Acquire only systems for which roaming indication is off
   AEET_ROAM_PREF_AFFIL : Acqire only systems for which roaming indication is off or solid on. i.e not blinking.
   AEET_ROAM_PREF_ANY : Acquire systems regardless of their roaming indication.
   AEET_ROAM_PREF_NO_CHANGE : Donot change roam preference.

Comments: 
   None

See Also:
   IPHONECTL_SetSystemPreference()

=======================================================================

AEETHybrPref

Description:
   Enumeration of system selection hybrid preferences.

Definition:
typedef enum
{
   AEET_HYBR_PREF_OFF,
   AEET_HYBR_PREF_ON,
   AEET_HYBR_PREF_NO_CHANGE
} AEETHybrPref;

Members:
   AEET_HYBR_PREF_OFF : hybrid operation is not allowed.
   AEET_HYBR_PREF_ON : hybrid operation is allowed. i.e if modem supports simultaneous HDR and CDMA.
   AEET_HYBR_PREF_NO_CHANGE : Donot change hybrid preference.

Comments: 
   None

See Also:
   None

=======================================================================

AEETNwSelModePref

Description:
   Enumeration that defines whether network should be selected automatically, manually or for limited service.

Definition:
typedef enum
{
   AEET_NETWORK_SEL_MODE_PREF_AUTOMATIC
   AEET_NETWORK_SEL_MODE_PREF_MANUAL,
   AEET_NETWORK_SEL_MODE_PREF_LIMITED_SRV,
   AEET_NETWORK_SEL_MODE_PREF_NO_CHANGE
} AEETNwSelModePref

Members:
   AEET_NETWORK_SEL_MODE_PREF_AUTOAMTIC : Automatic network selection.
   AEET_NETWORK_SEL_MODE_PREF_MANUAL : Manual network selection.
   AEET_NETWORK_SEL_MODE_PREF_LIMITED_SRV : Select a network for limited service.
   AEET_NETWORK_SEL_MODE_PREF_NO_CHANGE : Do not change preference.

Comments: 
   None

See Also:
   IPHONECTL_SetSystemPreference()

=======================================================================

AEETServiceType

Description: Enumeration that service types for call origination request

Definition:
typedef enum
{
   AEET_SRV_TYPE_AMPS
   AEET_SRV_TYPE_HDR
   AEET_SRV_TYPE_CDMA_SPECIFIC
   AEET_SRV_TYPE_CDMA_AUTOMATIC
   AEET_SRV_TYPE_AUTOMATIC
   AEET_SRV_TYPE_GSM_WCDMA
   AEET_SRV_TYPE_WCDMA
   AEET_SRV_TYPE_GSM
} AEETNwSelModePref

Members:
   AEET_SRV_TYPE_AMPS : 
      Call is to be connected over analog service.
    
      Note that the client does NOT need to specify service option when
      originating with this call type.
    
      NOTE! When originating with this service type, the mode preference
      might be forced to ANALOG ONLY before origination and returned to
      its original setting at the end of the call 

   AEET_SRV_TYPE_HDR :
      Call is to be connected over HDR(1xEv) service.

   AEET_SRV_TYPE_CDMA_SPECIFIC : 
      Call is to be connected over CDMA service with specific client
      designated CDMA service option.
    
      Note that the application need to specify service option when
      originating with this call type.
    
      NOTE! When originating with this service type, the mode preference
      might be forced to DIGITAL ONLY before origination and returned to
      its original setting at the end of the call

   AEET_SRV_TYPE_CDMA_AUTOMATIC : 
      Call is to be connected over CDMA service where Call manager automatically
      originate with the best available service option for the specified
      call type.
    
     Note that the application does NOT need to specify service option when
     originating with this call type

   AEET_SRV_TYPE_AUTOMATIC :
      Call is to be connected over available service - AMPS or CDMA,
      or GSM or WCDMA or HDR.
    
      Note that the application does NOT need to specify service option when
      originating with this srv type except for the packet data call

   AEET_SRV_TYPE_GSM_WCDMA :
      Call is to be connected over available service - GSM or WCDMA

   AEET_SRV_TYPE_WCDMA :
      Call is to be connected over WCDMA service

   AEET_SRV_TYPE_GSM :
      Call is to be connected over GSM service

Comments: 
   To specify a service type while making a call ICallOpts should 
   be used. Default service type is AEET_SRV_TYPE_AUTOMATIC.


See Also:
   ICallOpts
   
=======================================================================

AEETSysIDType

Description: Define a enumerated type that indicates the type of system ID.

Definition:
typedef enum
{
   AEET_SYS_ID_TYPE_UNDEFINED,
   AEET_SYS_ID_TYPE_UMTS,
   AEET_SYS_ID_TYPE_IS95,
   AEET_SYS_ID_TYPE_IS856
} AEETNwSelModePref

Members:
   AEET_SYS_ID_TYPE_UNDEFINED : System ID is undefined.
   AEET_SYS_ID_TYPE_UMTS : UMTS (WCDMA/GSM) system ID
   AEET_SYS_ID_TYPE_IS95 : IS-95 system ID
   AEET_SYS_ID_TYPE_IS856 : IS-856 (HDR) system ID
Comments: 
   None

See Also:
   None

=======================================================================

AEETIS95SysID

Description: Define a structure that identifies an IS-95 system.


Definition:
typedef struct 
{
   uint16         sid;
   uint16         nid;
} AEETIS95SysID

Members:
   sid : The system ID
   nid : The network ID

Comments: 
   None

See Also:
   None

=======================================================================

AEETSysIDData

Description: Type for system ID.

Definition:
typedef union 
{
   AEETIS95SysID  is95;
   byte           is856[16];
   AEETPLMNID     plmn;
} AEETSysIDData;

Members:
   is95 : IS-95 system ID
   is856[16] : IS-856 system ID
   plmn : UMTS (WCDMA/GSM) system ID
    
Comments: 
   None

See Also:
   None
   
=======================================================================

AEETSysID

Description:

Definition:
   typedef struct 
   {
      AEETSysIDType  id_type;
      AEETSysIDData  id;
   } AEETSysID;
Members:
   id_type : System identifier type.
   id : The system identifier

Comments: 
   None

See Also:
   None

=======================================================================

AEETSIMState

Description: defines the state of the Subscriber Identfication Module (SIM) card.

Definition:
typedef enum
{
   AEET_SIM_STATE_NOT_AVAILABLE,
   AEET_SIM_STATE_AVAILABLE,
   AEET_SIM_STATE_CS_INVALID,
   AEET_SIM_STATE_PS_INVALID,
   AEET_SIM_STATE_CS_PS_INVALID
} AEETSIMState

Members:
   AEET_SIM_STATE_NOT_AVAILABLE :  The SIM is not available               
   AEET_SIM_STATE_AVAILABLE :  The SIM is available                   
   AEET_SIM_STATE_CS_INVALID :  The SIM has been marked by the network as invalid for CS services             
   AEET_SIM_STATE_PS_INVALID :  The SIM has been marked by the network as invalid for PS services             
   AEET_SIM_STATE_CS_PS_INVALID :  The SIM has been marked by the network as invalid for CS and PS services      

Comments: 
   None

See Also:
   None
   
=======================================================================   

AEETLSAIdentityType

Description: Define a type for the Localized Service Area identity.  The format
             is specified by 3GPP TS 24.008 and 23.003.

Definition:
typedef enum
{
   AEET_LSA_ID_TYPE_PLMN_SIG_NUM,
   AEET_LSA_ID_TYPE_UNIVERSAL
}AEETLSAIdentityType;

Members:
   AEET_LSA_ID_TYPE_PLMN_SIG_NUM : The LSA is a PLMN significant number     
   AEET_LSA_ID_TYPE_UNIVERSAL : The LSA is a universal 

Comments: 
   None

See Also:
   None

=======================================================================   

AEETLSAIdentity

Description : Define a type for the Localized Service Area identity.  The format
             is specified by 3GPP TS 24.008 and 23.003.

Definition:
   typedef struct 
   {
     AEETLSAIdentityType               type;
     uint8                             identity[3];
   } AEETLSAIdentity;

Members:
   type
   identity

Comments: 
   None

See Also:
   None  

=======================================================================

AEETOTASPActivationCode

Description: Enumeration of OTASP activation code. For CDMA only.


Definition:
typedef enum
{
   AEET_OTASP_ACT_ABAND,
   AEET_OTASP_ACT_BBAND,
   AEET_OTASP_ACT_ABLOCK,
   AEET_OTASP_ACT_BBLOCK,
   AEET_OTASP_ACT_CBLOCK,
   AEET_OTASP_ACT_DBLOCK,
   AEET_OTASP_ACT_EBLOCK,
   AEET_OTASP_ACT_FBLOCK,
   AEET_OTASP_ACT_PREFERRED
} AEETOTASPActivationCode

Members:
   AEET_OTASP_ACT_ABAND : A band
   AEET_OTASP_ACT_BBAND: B band
   AEET_OTASP_ACT_ABLOCK : A block
   AEET_OTASP_ACT_BBLOCK : B block
   AEET_OTASP_ACT_CBLOCK : C block
   AEET_OTASP_ACT_DBLOCK : D block
   AEET_OTASP_ACT_EBLOCK : E block
   AEET_OTASP_ACT_FBLOCK : F block
   AEET_OTASP_ACT_PREFERRED : Preferred OTASP
   
Comments: 
   None

See Also:
   None

=======================================================================

AEETSysBandClass

Description: Enumeration of system band classes.


Definition:
typedef enum
{
   AEET_SYS_BAND_BC0,
   AEET_SYS_BAND_BC1,
   AEET_SYS_BAND_BC3,
   AEET_SYS_BAND_BC4,
   AEET_SYS_BAND_BC5,
   AEET_SYS_BAND_BC6,
   AEET_SYS_BAND_BC7,
   AEET_SYS_BAND_BC8,
   AEET_SYS_BAND_BC9,
   AEET_SYS_BAND_BC10,
   AEET_SYS_BAND_BC11,
   
   AEET_SYS_BAND_GSM_450,
   AEET_SYS_BAND_GSM_480,
   AEET_SYS_BAND_GSM_750,
   AEET_SYS_BAND_GSM_850,
   AEET_SYS_BAND_GSM_EGSM_900,
   AEET_SYS_BAND_GSM_PGSM_900,
   AEET_SYS_BAND_GSM_RGSM_900,
   AEET_SYS_BAND_GSM_DCS_1800,
   AEET_SYS_BAND_GSM_PCS_1900,
   
   AEET_SYS_BAND_WCDMA_I_IMT_2000,
   AEET_SYS_BAND_WCDMA_II_PCS_1900,
   AEET_SYS_BAND_WCDMA_III_1700,
   AEET_SYS_BAND_WCDMA_IV_1700,
   AEET_SYS_BAND_WCDMA_V_850,
   AEET_SYS_BAND_WCDMA_VI_800,
   AEET_SYS_BAND_WCDMA_VII_2600,
   AEET_SYS_BAND_WCDMA_VIII_900,
   AEET_SYS_BAND_WCDMA_IX_1700
} AEETSysBandClass

Members:
   AEET_SYS_BAND_BC0                : Band Class 0 - US Cellular band (800 MHz)       
   AEET_SYS_BAND_BC1                : Band Class 1 - US 
   AEET_SYS_BAND_BC3                : Band Class 3 - Japanese Cellular band (800 MHz) 
   AEET_SYS_BAND_BC4                : Band Class 4 - Korean PCS band (1800 MHz)       
   AEET_SYS_BAND_BC5                : Band Class 5 (450 MHz)                          
   AEET_SYS_BAND_BC6                : Band Class 6 (2 GHz)                            
   AEET_SYS_BAND_BC7                : Band Class 7 (700 MHz)                          
   AEET_SYS_BAND_BC8                : Band Class 8 (1800 MHz)                         
   AEET_SYS_BAND_BC9                : Band Class 9 (900 MHz)                          
   AEET_SYS_BAND_BC10               : Band Class 10 (Second 800 MHz)                  
   AEET_SYS_BAND_BC11               : Band Class 11 - European PAMR Band (400 MHz)    

   Reserved 12-39 for CDMA band classes. 
   AEET_SYS_BAND_GSM_450            : GSM 450 band (450 MHz)                          
   AEET_SYS_BAND_GSM_480            : GSM 480 band (480 MHz)                          
   AEET_SYS_BAND_GSM_750            : GSM 750 band (750 MHz)                          
   AEET_SYS_BAND_GSM_850            : GSM 850 band (850 MHz)                          
   AEET_SYS_BAND_GSM_EGSM_900       : GSM Extended GSM (E-GSM) 900 band (900 MHz)         
   AEET_SYS_BAND_GSM_PGSM_900       : GSM Primary GSM ( P-GSM) 900 band (900 MHz)         
   AEET_SYS_BAND_GSM_RGSM_900       : GSM Railways GSM ( R-GSM) 900 band (900 MHz)        
   AEET_SYS_BAND_GSM_DCS_1800       : GSM DCS band (1800 MHz)                         
   AEET_SYS_BAND_GSM_PCS_1900       : GSM PCS band (1900 Mhz)                         

   Reserved 49-79 for GSM band classes. 
   AEET_SYS_BAND_WCDMA_I_IMT_2000   : WCDMA IMT 2000 (2000 MHz)                       
   AEET_SYS_BAND_WCDMA_II_PCS_1900  : WCDMA PCS band (1900 MHz)                       
   AEET_SYS_BAND_WCDMA_III_1700     : WCDMA 1700 (1700 MHz)                           
   AEET_SYS_BAND_WCDMA_IV_1700      : WCDMA 1700 (1700 MHz)                           
   AEET_SYS_BAND_WCDMA_V_850        : WCDMA 850 (850 MHz)                             
   AEET_SYS_BAND_WCDMA_VI_800       : WCDMA 800 (800 MHz)                             
   AEET_SYS_BAND_WCDMA_VII_2600     : WCDMA 2600 (2600 MHz)                           
   AEET_SYS_BAND_WCDMA_VIII_900     : WCDMA 900 (900 MHz)                             
   AEET_SYS_BAND_WCDMA_IX_1700      : WCDMA 1700 (1700 MHz)  
   
Comments: 
   None

See Also:
   None
=======================================================================

AEETSysBlocks

Description:
   Enumeration of PCS frequency blocks/Cellular systems.

Definition:
typedef enum
{
   AEET_SYS_BLKSYS_PCS_A,
   AEET_SYS_BLKSYS_PCS_B,
   AEET_SYS_BLKSYS_PCS_C,
   AEET_SYS_BLKSYS_PCS_D,
   AEET_SYS_BLKSYS_PCS_E,
   AEET_SYS_BLKSYS_PCS_F,
   AEET_SYS_BLKSYS_PCS_CELL_A,
   AEET_SYS_BLKSYS_PCS_CELL_B,
   AEET_SYS_BLKSYS_PCS_G,
   AEET_SYS_BLKSYS_PCS_H
}AEETSysBlocks

Members:
   AEET_SYS_BLKSYS_PCS_A            :PCS block A                            
   AEET_SYS_BLKSYS_PCS_B            :PCS block B                            
   AEET_SYS_BLKSYS_PCS_C            : PCS block C                            
   AEET_SYS_BLKSYS_PCS_D            : PCS block D                            
   AEET_SYS_BLKSYS_PCS_E            : PCS block E                            
   AEET_SYS_BLKSYS_PCS_F            : PCS block F                            
   AEET_SYS_BLKSYS_PCS_CELL_A       : Cellular system A                      
   AEET_SYS_BLKSYS_PCS_CELL_B       : Cellular system B                      
   AEET_SYS_BLKSYS_PCS_G            : PCS E-Block                            
   AEET_SYS_BLKSYS_PCS_H            : PCS F-Block                            
   
Comments: 
   None

See Also:
   None
   
=======================================================================   

AEETUZName

Description:
   structure to hold user zone name information.

Definition:
   typedef struct
   {
     byte     uz_name_encoding;
     byte     uz_name_n_octets;
     byte     uz_name[AEET_PUZL_MAX_UZ_NAME_LEN];
   } AEETUZName;


Members:
   uz_name_encoding
   uz_name_n_octets
   uz_name

Comments: 
   None

See Also:
   None
=======================================================================

AEETCallDirection

Description: Enumeration of Direction of Call.

Definition:
typedef enum
{
   AEET_CALL_DIRECTION_NONE,
   AEET_CALL_DIRECTION_MO,
   AEET_CALL_DIRECTION_MT
} AEETCallDirection;

Members:
   AEET_CALL_DIRECTION_NONE : Undefined Direction
   AEET_CALL_DIRECTION_MO : Mobile Originated Call
   AEET_CALL_DIRECTION_MT : Mobile Terminated Call

Comments: 
   None

See Also:
   None
=======================================================================

AEETPI

Description: Enumeration of Presentation indicator

Definition:
typedef enum
{
   AEET_PI_ALLOW,
   AEET_PI_RES,
   AEET_PI_NOT_AVAIL
} AEETPI

Members:
   AEET_PI_ALLOW : Presentation allowed
   AEET_PI_RES : Presentation restricted
   AEET_PI_NOT_AVAIL : Number not available

Comments: 
   None

See Also:
   None

=======================================================================

AEETDetailedPLMNCat

Description: : Enumeration of PLMN category.

Definition:
typedef enum
{
   AEET_DETAILED_PLMN_LIST_CATEGORY_HPLMN,
   AEET_DETAILED_PLMN_LIST_CATEGORY_PREFERRED,
   AEET_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED,
   AEET_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED,
   AEET_DETAILED_PLMN_LIST_CATEGORY_OTHER
} AEETDetailedPLMNCat

Members:
  AEET_DETAILED_PLMN_LIST_CATEGORY_HPLMN :
  AEET_DETAILED_PLMN_LIST_CATEGORY_PREFERRED :
  AEET_DETAILED_PLMN_LIST_CATEGORY_USER_PREFERRED :
  AEET_DETAILED_PLMN_LIST_CATEGORY_OPERATOR_PREFERRED :
  AEET_DETAILED_PLMN_LIST_CATEGORY_OTHER :

Comments: 
   None

See Also:


=======================================================================

AEETSignalType

Description: Enumeration of Signal types.

Definition:
typedef enum
{
   AEET_SIGNAL_NONE,
   AEET_SIGNAL_GW_TONE,
   AEET_SIGNAL_CDMA_TONE,
   AEET_SIGNAL_CDMA_ISDN,
   AEET_SIGNAL_CDMA_IS54B
} AEETSignalType

Members:
   AEET_SIGNAL_NONE:
   AEET_SIGNAL_GW_TONE: GSM/WCDMA Tone Signal
   AEET_SIGNAL_CDMA_TONE: CDMA Tone signal
   AEET_SIGNAL_CDMA_ISDN: CDMA ISDN Alerting
   AEET_SIGNAL_CDMA_IS54B: CDMA IS-54B Alerting

Comments: 
   None

See Also:
   None

=======================================================================
AEETGWSignalTone

Description: Enumeration of GSM/WCDMA Signal Tones.

Definition:
typedef enum
{
  AEET_GW_DIAL_TONE_ON,
  AEET_GW_RING_BACK_TONE_ON,
  AEET_GW_INTERCEPT_TONE_ON,
  AEET_GW_NW_CONGESTION_TONE_ON,
  AEET_GW_BUSY_TONE_ON,
  AEET_GW_CONFIRM_TONE_ON,
  AEET_GW_ANSWER_TONE_ON,
  AEET_GW_CALL_WAITING_TONE_ON,
  AEET_GW_OFF_HOOK_WARNING_TONE_ON,
  AEET_GW_TONES_OFF,
  AEET_GW_ALERTING_TONE_OFF
} AEETGWSignalTone

Members:
  AEET_GW_DIAL_TONE_ON : dial tone.
  AEET_GW_RING_BACK_TONE_ON : ring back tone.
  AEET_GW_INTERCEPT_TONE_ON : intercept tone.
  AEET_GW_NW_CONGESTION_TONE_ON : network congestion.
  AEET_GW_BUSY_TONE_ON : busy tone.
  AEET_GW_CONFIRM_TONE_ON : call confirmed tone.
  AEET_GW_ANSWER_TONE_ON : answer tone.
  AEET_GW_CALL_WAITING_TONE_ON : call waiting tone.
  AEET_GW_OFF_HOOK_WARNING_TONE_ON : off hook warning tone.
  AEET_GW_TONES_OFF : no tones.
  AEET_GW_ALERTING_TONE_OFF : alerting tone off. 
Comments: 
   None

See Also:
   None
=======================================================================
AEETCDMASignalTone

Description: Enumeration of CDMA Signal Tones.

Definition:
typedef enum
{
  AEET_CDMA_TONE_DIAL,
  AEET_CDMA_TONE_RING,
  AEET_CDMA_TONE_INTERCEPT,
  AEET_CDMA_TONE_ABBR_INTERCEPT,
  AEET_CDMA_TONE_REORDER,
  AEET_CDMA_TONE_ABBR_REORDER,
  AEET_CDMA_TONE_BUSY,
  AEET_CDMA_TONE_CONFIRM,
  AEET_CDMA_TONE_ANSWER,
  AEET_CDMA_TONE_CALLWAIT,
  AEET_CDMA_TONE_PIP,
  AEET_CDMA_TONE_OFF
} AEETCDMASignalTone

Members:
  AEET_CDMA_TONE_DIAL : Dial tone on.
  AEET_CDMA_TONE_RING : Ring back tone on.
  AEET_CDMA_TONE_INTERCEPT : Intercept tone on.
  AEET_CDMA_TONE_ABBR_INTERCEPT : Abbreviated intercept: alternating tones
                                  repeated for four seconds.
  AEET_CDMA_TONE_REORDER : Network congestion (reorder) tone on.
  AEET_CDMA_TONE_ABBR_REORDER : Abbreviated network congestion (reorder).
  AEET_CDMA_TONE_BUSY : Busy tone on.
  AEET_CDMA_TONE_CONFIRM : Confirm tone on.
  AEET_CDMA_TONE_ANSWER : Answer tone on.
  AEET_CDMA_TONE_CALLWAIT : Call waiting tone on.
  AEET_CDMA_TONE_PIP : Pip tone on.
  AEET_CDMA_TONE_OFF : Tones off.
Comments: 
   None

See Also:
   None
=======================================================================
AEETCDMASignalISDN

Description: Enumaration of CDMA ISDN Alerting signals.

Definition:
typedef enum
{
   AEET_CDMA_ISDN_NORMAL,
   AEET_CDMA_ISDN_INTERGROUP,
   AEET_CDMA_ISDN_SPECIAL,
   AEET_CDMA_ISDN_RESERVED3,
   AEET_CDMA_ISDN_PING,
   AEET_CDMA_ISDN_RESERVED5,
   AEET_CDMA_ISDN_RESERVED6,
   AEET_CDMA_ISDN_RESERVED7,
   AEET_CDMA_ISDN_OFF
} AEETCDMASignalISDN

Members:
   AEET_CDMA_ISDN_NORMAL : Normal Alerting.
   AEET_CDMA_ISDN_INTERGROUP : Intergroup Alerting.
   AEET_CDMA_ISDN_SPECIAL : Special/Priority Alerting.
   AEET_CDMA_ISDN_RESERVED3 : Reserved (ISDN Alerting pattern 3).
   AEET_CDMA_ISDN_PING : "Ping ring": single burst of 500 ms.
   AEET_CDMA_ISDN_RESERVED5 : Reserved (ISDN Alerting pattern 5).
   AEET_CDMA_ISDN_RESERVED6 : Reserved (ISDN Alerting pattern 6).
   AEET_CDMA_ISDN_RESERVED7 : Reserved (ISDN Alerting pattern 7).
   AEET_CDMA_ISDN_OFF : 0x0F,Alerting off.

Comments: 
   None

See Also:
   None

=======================================================================
AEETCDMASignalIS54B

Description: Enumaration of CDMA IS-54B Alerting Signals.

Definition:
typedef enum
{
   AEET_CDMA_IS54B_OFF,
   AEET_CDMA_IS54B_L,
   AEET_CDMA_IS54B_SS,
   AEET_CDMA_IS54B_SSL,
   AEET_CDMA_IS54B_SS2,
   AEET_CDMA_IS54B_SLS,
   AEET_CDMA_IS54B_SSSS,
   AEET_CDMA_IS54B_PBX_L,
   AEET_CDMA_IS54B_PBX_SS,
   AEET_CDMA_IS54B_PBX_SSL
   AEET_CDMA_IS54B_PBX_SLS,
   AEET_CDMA_IS54B_PBX_SSSS,
   AEET_CDMA_IS54B_PPPP
} AEETCDMASignalIS54B

Members:
   AEET_CDMA_IS54B_OFF : No Tone: off.
   AEET_CDMA_IS54B_L : Long: (standard alert).
   AEET_CDMA_IS54B_SS : Short-short.
   AEET_CDMA_IS54B_SSL : Short-short-long.
   AEET_CDMA_IS54B_SS2 : Short-short-2.
   AEET_CDMA_IS54B_SLS : Short-long-short.
   AEET_CDMA_IS54B_SSSS : Short-short-short-short.
   AEET_CDMA_IS54B_PBX_L : PBX Long.
   AEET_CDMA_IS54B_PBX_SS : PBX Short-short.
   AEET_CDMA_IS54B_PBX_SSL : PBX Short-short-long.
   AEET_CDMA_IS54B_PBX_SLS : PBX Short-long-short.
   AEET_CDMA_IS54B_PBX_SSSS : PBX Short-short-short-short.
   AEET_CDMA_IS54B_PPPP : Pip-Pip-Pip-Pip.
Comments: 
   None

See Also:
   None
=======================================================================
AEETSignalPitch

Description: Enumeration of Signal Alert Pitch.

Definition:
typedef enum
{
   AEET_PITCH_NONE,
   AEET_PITCH_MED,
   AEET_PITCH_HIGH,
   AEET_PITCH_LOW
} AEETSignalPitch

Members:
   AEET_PITCH_NONE : -1, No Pitch information.
   AEET_PITCH_MED : Medium pitch (standard alert).
   AEET_PITCH_HIGH : High pitch.
   AEET_PITCH_LOW.Low Pitch.
   
Comments: 
   None

See Also:
   None
=======================================================================
AEETSignalValue

Description: Enumeration of Signal value.

Definition:
typedef union 
{
   AEETGWSignalTone      gw_tone;
   AEETCDMASignalTone    cdma_tone;
   AEETCDMASignalISDN    cdma_isdn;
   AEETCDMASignalIS54B   cdma_is54b;
} AEETSignalValue;

Members:
typedef union 
{
   gw_tone : GSM/WCDMA Signal Tones.
   cdma_tone : CDMA Signal Tones.
   cdma_isdn :  CDMA ISDN Alerting signals.
   cdma_is54b : CDMA IS-54B Alerting Signals.
} AEETSignalValue;
Comments: 
   Signal value depends on the signal type and the mapping is as follows.
   Signal Type:                   Signal Value 
   ---------------------------------------------------
   AEET_SIGNAL_GW_TONE:          AEETGWSignalTone
   AEET_SIGNAL_CDMA_TONE:        AEETCDMASignalTone
   AEET_SIGNAL_CDMA_ISDN:        AEETCDMASignalISDN
   AEET_SIGNAL_CDMA_IS54B:       AEETCDMASignalIS54B

See Also:
   None
=======================================================================
AEETPhError

Description: Enumeration of Ph status/errors returned through callback.


Definition:
typedef enum
{
   AEET_PH_ERROR_NONE,
   AEET_PH_ERROR_IN_USE_S,
   AEET_PH_ERROR_ONLINE_S,
   AEET_PH_ERROR_OPRT_MODE_S,
   AEET_PH_ERROR_MODE_PREF_S,
   AEET_PH_ERROR_SYS_PREF_S,
   AEET_PH_ERROR_SYS_SEL_PREF_S,
   AEET_PH_ERROR_HYBR_PREF_S,
   AEET_PH_ERROR_SUBSCRIPTION_AVAILABLE_S,
   AEET_PH_ERROR_SYS_NOT_IN_UZ,
   AEET_PH_ERROR_OTHER,
   AEET_PH_ERROR_CURRENT_NAM_IS_NOT_RUIM_NAM_S,
   AEET_PH_ERROR_LINE_SWITCHING,
   AEET_PH_ERROR_ALS_SIM_WRITE_FAILED
} AEETPhError

Members:
   AEET_PH_ERROR_NONE : No Error
   AEET_PH_ERROR_IN_USE_S : Phone is in-use (i.e. in a call).
   AEET_PH_ERROR_ONLINE_S : Phone is offline
   AEET_PH_ERROR_OPRT_MODE_S : Invalid command or operating mode switch 
                               relatively to the current operating mode - e.g.
                               OFFLINE -> ONLINE.
   AEET_PH_ERROR_MODE_PREF_S : Invalid mode preference selection relatively to 
                               what we currently have in the PRL - e.g. ANALOG_ONLY
                               when there are no analog entries in the PRL.
   AEET_PH_ERROR_SYS_PREF_S : Invalid system preference selection for this device.
   AEET_PH_ERROR_SYS_SEL_PREF_S : Invalid system selection preference for this 
                                  target/PRL - e.g. band-class 1 for a PCS only target.
   AEET_PH_ERROR_HYBR_PREF_S : FEATURE_HDR - Invalid command or hybrid preference selection.
   AEET_PH_ERROR_SUBSCRIPTION_AVAILABLE_S : Invalid command relative to the current value of subscription
                                            available - e.g. transitioning from subscription available to
                                            subscription available is invalid or issuing a command when
                                            the subscription information is not available. GSM/WCDMA Only.
   AEET_PH_ERROR_SYS_NOT_IN_UZ : Indicates that the MS is not in user zone, when
                                 "Avoid system" on user zone was processed. CDMA Only
   AEET_PH_ERROR_OTHER : A problem other than the above was found.
   AEET_PH_ERROR_CURRENT_NAM_IS_NOT_RUIM_NAM_S : Current NAM is not RUIM NAM. CDMA Only.
   AEET_PH_ERROR_LINE_SWITCHING : line switching failed.
   AEET_PH_ERROR_ALS_SIM_WRITE_FAILED : Failed to write the line selection to SIM.

Comments: 
   None

See Also:
   None

=======================================================================
*/       
#endif /* AEETELDEF_H */
