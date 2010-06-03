#ifndef AEEIServingNetworks_H
#define AEEIServingNetworks_H
/*============================================================================
File:         AEEIServingNetworks.h

Services:     ServingNetworks interface to BREW

Description:  Base level definitions, typedefs, etc. for IServingNetworks
   
==============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "AEEISignal.h"
#include "AEETelephoneTypes.h"

#define AEEIID_IServingNetworks            0x0104d090

/*============================================================================
// IServingNetworks Interface
============================================================================*/

// Maximum size of the SSID.
#define AEET_WLAN_SSID_MAX_SIZE         32

/* Specifies a network id/descriptor. */
typedef uint16  AEETNetworkID;

/* Channel data type. */
typedef word  AEETChanType;

/* Type for WLAN BSS id. */
typedef uint64  AEETWlanBssid;

/* Type for WLAN BSS types.  */
typedef int16  AEETWlanBssType;

/* Type for WLAN technology type/version.  */
typedef int16  AEETWlanTech;


/*
** Enumeration of WLAN BSS types. 
*/
#define  AEET_WLAN_BSS_TYPE_NONE            -1
#define  AEET_WLAN_BSS_TYPE_ADHOC            0  /* Ad-Hoc Type BSS.  */
#define  AEET_WLAN_BSS_TYPE_INFRA            1  /* Infrastructure Mode BSS. */
#define  AEET_WLAN_BSS_TYPE_ANY              2  /* Any BSS Mode Type.  */
#define  AEET_WLAN_BSS_TYPE_ADHOC_START      3  /* Ad-Hoc Type BSS created by MS. */

  
/*
** Enumeration of Technology type/version.
*/  
#define  AEET_TECH_NONE                    -1,
#define  AEET_TECH_WLAN_80211A              0  /* WLAN 802.11a technology. */
#define  AEET_TECH_WLAN_80211B              1  /* WLAN 802.11b technology. */
#define  AEET_TECH_WLAN_80211G              2  /* WLAN 802.11g technology. */


/* Define a structure that contains basic information for any kind of network */
typedef struct 
{
   AEETSysMode            sysMode;
   /* The system's mode. */
   
   AEETSrvStatus          srvStatus;
   /* The service status (SERVICE/NO SERVICE) of the system. */
   
   AEETNetworkID          nwid; 
  /* Unique ID given to each available network */
  
}AEETNwInfo;


/* Define a structure for SSID of WLAN system. */
typedef struct {
   uint8                  ucLen;
   /* Length of the SSID, if == 0, then SSID = broadcast SSID.*/

   uint8                  aucSsid[AEET_WLAN_SSID_MAX_SIZE];
   /* SSID of the wlan system.*/
 
} AEETWLANSysID;


/* Define a structure that contains detailed information for WLAN network  */
typedef struct
{
   AEETSrvStatus      srvStatus;
   /* The service status (SERVICE/NO SERVICE) of the system. */    
   
   AEETSysMode        sysMode;
   /* The system's mode. */  
   
   uint16             usRssi;
   /* WLAN signal strength */

   AEETWLANSysID      ssid; 
   /* WLAN network system id*/
   
   AEETChanType       chan;
   /* Channel on which this BSS provides service. */

   AEETSysBandClass   bandClass;
   /* Band in which this BSS provides service. */

   AEETWlanBssType        bssType;
   /* Type of BSS - Infrastructure or independent. */

   AEETWlanBssid      bssid;
   /* BSS Id. */

   AEETWlanTech       tech;
   /* WLAN technology in use. */
 
} AEETWLANDesc;



/* Define a structure that contains detailed information for CDMA network */
typedef struct
{
   AEETSrvStatus      srvStatus;
   /* The service status (SERVICE/NO SERVICE) of the system. */    
   
   AEETSysMode        sysMode;
   /* The system's mode. */  
   
   AEETSrvDomain      srvDomain;
   /* The service domain registered on the system. */  
   
   AEETSrvCapability  srvCapability;
   /* The system's service capability.*/  
   
   AEETRoamStatus     roamStatus;
   /* Indicate current roaming status */
   
   uint16             usRssi;
   /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
   ** 800 CDMA:  -105 to -90 dBm
   ** 1900 CDMA: -108 to -93 dBm */
   
   AEETIS95SysID      is95;
   /* CDMA/IS-95 specific system ID */
   
   boolean            bCcsSupported;
   /* Is concurrent services supported. */
   
   boolean            bRegistered;
   /* Is mobile registered on the current system */
     
   AEETSysBandClass   bandClass;
   /* Cellular or PCS */
     
   int32              nUzId;
   /* User zone Identification of the selected user zone */
     
   AEETUZName         uzName;
   /* User zone name of the selected user zone. */
     
   AEETSysBlocks      blockSystem;
   /* PCS frequency block: A to F Cellular System A or B*/

   uint16             usPacketZone;
   /* 8  bit packet zone, 0xffff means PACKET ZONE NONE. */

   uint8              ucBsPRev;
   /* 8  bit base station p_rev. */

   uint8              ucPRevInUse;
   /* 8  bit ucPRevInUse in use the lesser of BS p_rev and mobile p_rev. */    

} AEETCDMADesc;


/* Define a structure that contains detailed information for GSM/CDMA network */
typedef struct
{
   AEETSrvStatus      srvStatus;
   /* The service status (SERVICE/NO SERVICE) of the system. */    
   
   AEETSysMode        sysMode;
   /* The system's mode. */  
   
   AEETSrvDomain      srvDomain;
   /* The service domain registered on the system. */  
   
   AEETSrvCapability  srvCapability;
   /* The system's service capability.*/  
   
   AEETRoamStatus     roamStatus;
   /* Indicate current roaming status */
   
   uint16             usRssi;
   /* RSSI in positive dBm i.e. 75 means -75dBm. The range for
   ** GSM/WCDMA: -105 to -60 dBm */
   
   AEETPLMNID         plmn;
   /* GSM/WCDMA specific system ID */
     
   AEETSIMState       simState;
   /* Current state of the SIM */  
   
   boolean            bPlmnForbidden;
   /* Indicates whether the PLMN is forbidden */
   
   AEETMMInfo         mmInformation;
   /* Mobility management information, i.e. network name, date,
   ** time and time zone of registered network */
   
   boolean            bCcsSupported;
   /* Is concurrent services supported. */  
   
} AEETGWDesc;



/* Define a structure that contains detailed information for HDR(1xEVDO) network */
typedef struct
{
   AEETSrvStatus      srvStatus;
   /* The service status (SERVICE/NO SERVICE) of the system. */    
   
   AEETSysMode        sysMode;
   /* The system's mode. */  
   
   AEETSrvDomain      srvDomain;
   /* The service domain registered on the system. */  
   
   AEETSrvCapability  srvCapability;
   /* The system's service capability.*/  
   
   AEETRoamStatus     roamStatus;
   /* Indicate current roaming status */
   
   uint16             usRssi;
   /* HDR RSSI in positive dBm i.e. 75 means -75dBm 
   ** The range is -105 to -90 dBm */
   
   //  network specific
   byte               ucaIs856[16];
   /* HDR(1xEVDO) specific system ID */
     
   AEETSysBandClass   bandClass;
   /* Cellular or PCS */  
   
   uint8              ucPRevInUse;
   /* Rev version used for EVDO network */
   
} AEETHDRDesc;


#define INHERIT_IServingNetworks(iname) \
   INHERIT_IQI(iname); \
   int (*GetSrvNetworks)( iname* po, AEETNwInfo *pNwInfo, int nSize, int *pnAvailableNetworks ); \
   int (*GetNwInfo)( iname* po, AEETNetworkID nwDescriptor, char *pBuffer, int nBuffLen ); \
   int (*RegisterSignal)( iname* po, ISignal *piSignal )

/* Declare interface */
AEEINTERFACE_DEFINE(IServingNetworks);


static __inline uint32 IServingNetworks_AddRef(IServingNetworks * me)
{
   return AEEGETPVTBL(me, IServingNetworks)->AddRef(me);
}

static __inline uint32 IServingNetworks_Release(IServingNetworks * me)
{
   return AEEGETPVTBL(me, IServingNetworks)->Release(me);
}

static __inline int IServingNetworks_QueryInterface(IServingNetworks *me, AEEIID i, void **pp)
{
   return AEEGETPVTBL(me, IServingNetworks)->QueryInterface(me, i, pp);
}

static __inline int IServingNetworks_GetSrvNetworks(IServingNetworks* me, 
                                                    AEETNwInfo *pNwInfo, 
                                                    int nSize, 
                                                    int *pAvailableNetworks)
{
   return AEEGETPVTBL(me, IServingNetworks)->GetSrvNetworks(me, pNwInfo, nSize, pAvailableNetworks);
}

static __inline int IServingNetworks_GetNwInfo(IServingNetworks* me, 
                                               AEETNetworkID nwDescriptor, 
                                               char *pBuffer, 
                                               int buffLen)
{
   return AEEGETPVTBL(me, IServingNetworks)->GetNwInfo(me, nwDescriptor, pBuffer, buffLen);
}

static __inline int IServingNetworks_RegisterSignal(IServingNetworks* me, ISignal *piSignal)
{
   return AEEGETPVTBL(me, IServingNetworks)->RegisterSignal(me, piSignal);
}


/*============================================================================
  DATA STRUCTURE DOCUMENTATION
==============================================================================


AEETNetworkID

Description:
   
   Specifies a network id/descriptor.

Definition:
typedef uint16 AEETNetworkID;

Members:
   None

Comments: 
   None

See Also:
   None

==============================================================================

AEETChanType

Description:
   
   Specifies the channel type.

Definition:
typedef word  AEETChanType;

Members:
   None

Comments: 
   None

See Also:
   None

==============================================================================

AEETWlanBssid

Description:
   
   Specifies the WLAN BSS id.

Definition:
typedef uint64  AEETWlanBssid;

Members:
   None

Comments: 
   None

See Also:
   None

==============================================================================

AEETWlanBssType

Description:
 
   Specifies the WLAN BSS types.

Definition:
typedef int16  AEETWlanBssType;

Members:
   Following is the pre-defined list of WLAN BSS types.
   AEET_WLAN_BSS_TYPE_NONE            :No type
   AEET_WLAN_BSS_TYPE_ADHOC           :Ad-Hoc type BSS.
   AEET_WLAN_BSS_TYPE_INFRA           :Infrastructure mode BSS.
   AEET_WLAN_BSS_TYPE_ANY             :Any BSS mode type.
   AEET_WLAN_BSS_TYPE_ADHOC_START     :Ad-Hoc type BSS created by MS.

Comments: 
   None

See Also:
   None

==============================================================================

AEETWlanTech

Description:
   
   Specifies the WLAN technology type/version.

Definition:
typedef int16  AEETWlanTech;

Members:
   Following is the pre-defined list of WLAN technology
   AEET_TECH_NONE                    :None
   AEET_TECH_WLAN_80211A             :WLAN 802.11a technology.
   AEET_TECH_WLAN_80211B             :WLAN 802.11b technology.
   AEET_TECH_WLAN_80211G             :WLAN 802.11g technology.

Comments: 
   None

See Also:
   None
          
==============================================================================

AEETNwInfo

Description:

   Define a structure that contains basic information for any kind of network

Definition:

typedef struct 
{
   AEETSysMode        sysMode;
   AEETSrvStatus      srvStatus;
   AEETNetworkID      nwid; 
}AEETNwInfo;

Members:

   sysMode          : system mode
   srvStatus        : service status (SERVICE/NO SERVICE) of the system
   nwid             : unique ID given to each available network   

Comments: 
   None

See Also:
   None
   
==============================================================================

AEETWLANDesc

Description:
   
   Define a structure that contains detailed information for WLAN network 

Definition:

typedef struct
{
   AEETSrvStatus      srvStatus;
   AEETSysMode        sysMode;
   uint16             usRssi;
   AEETWLANSysID      ssid; 
   AEETChanType       chan;
   AEETSysBandClass   bandClass;
   AEETWlanBssType    bssType;
   AEETWlanBssid      bssid;
   AEETWlanTech       tech;
} AEETWLANDesc;

Members:

   srvStatus        : service status (SERVICE/NO SERVICE) of the system
   sysMode          : system mode
   usRssi           : WLAN signal strength 
   ssid             : WLAN network system id
   bandClass        : Band in which this BSS provides service
   bssType          : Type of BSS - Infrastructure or independent
   bssid            : BSS Id
   tech             : WLAN technology in use
 Comments: 
   None

See Also:
   None
   
==============================================================================

AEETCDMADesc

Description:
   
   Define a structure that describe detailed information for CDMA network 

Definition:

typedef struct
{
   AEETSrvStatus      srvStatus;
   AEETSysMode        sysMode;
   AEETSrvDomain      srvDomain;
   AEETSrvCapability  srvCapability;
   AEETRoamStatus     roamStatus;
   uint16             usRssi;
   AEETIS95SysID      is95;
   boolean            bCcsSupported;
   boolean            bRegistered;
   AEETSysBandClass   bandClass;
   int32              nUzId;
   AEETUZName         uzName;
   AEETSysBlocks      blockSystem;
   uint16             usPacketZone;
   uint8              ucBsPRev;
   uint8              ucPRevInUse;
} AEETCDMADesc;


Members:
   srvStatus        : service status (SERVICE/NO SERVICE) of the system
   sysMode          : system mode
   srvDomain        : service domain registered on the system
   srvCapability    : system's service capability
   roamStatus       : indicate current roaming status
   usRssi           : RSSI in positive dBm i.e. 75 means -75dBm. The range for
                      800 CDMA:  -105 to -90 dBm,  1900 CDMA: -108 to -93 dBm 
   is95             : CDMA/IS-95 specific system ID
   bCcsSupported    : is concurrent services supported
   bRegistered      : is mobile registered on the current system
   bandClass        : cellular or PCS 
   nUzId            : user zone Identification of the selected user zone 
   uzName           : user zone name of the selected user zone. 
   blockSystem      : PCS frequency block: A to F Cellular System A or B
   usPacketZone     : 8  bit packet zone, 0xffff means PACKET ZONE NONE
   ucBsPRev         : 8  bit base station p_rev
   ucPRevInUse      : 8  bit ucPRevInUse in use the lesser of BS p_rev and mobile p_rev.

Comments: 
   None

See Also:
   None

==============================================================================

AEETGWDesc

Description:

   Define a structure that describe detailed information for GSM/WCDMA network 

Definition:

typedef struct
{
   AEETSrvStatus      srvStatus;
   AEETSysMode        sysMode;
   AEETSrvDomain      srvDomain;
   AEETSrvCapability  srvCapability;
   AEETRoamStatus     roamStatus;
   uint16             usRssi;
   AEETPLMNID         plmn;
   AEETSIMState       simState;
   boolean            plmnForbidden;
   AEETMMInfo         mmInformation;
   boolean            bCcsSupported;
} AEETGWDesc;


Members:

   srvStatus        : service status (SERVICE/NO SERVICE) of the system
   sysMode          : system mode
   srvDomain        : service domain registered on the system
   srvCapability    : system's service capability
   roamStatus       : indicate current roaming status
   usRssi           : RSSI in positive dBm i.e. 75 means -75dBm. The range for
                      GSM/WCDMA: -105 to -60 dBm
   plmn             : GSM/WCDMA specific system ID
   simState         : Current state of the SIM
   plmnForbidden    : Indicates whether the PLMN is forbidden
   mmInformation    : Mobility management information, i.e. network name, date,
                       time and time zone of registered network 
   bCcsSupported    : is concurrent services supported.
} AEETGWDesc;

Comments: 
   None

See Also:
   None

==============================================================================

AEETHDRDesc

Description: 
   
   Define a structure that contains detailed information for HDR(1xEVDO) network 

Definition:

typedef struct
{
   AEETSrvStatus      srvStatus;
   AEETSysMode        sysMode;
   AEETSrvDomain      srvDomain;
   AEETSrvCapability  srvCapability;
   AEETRoamStatus     roamStatus;
   uint16             usRssi;
   byte               ucaIs856[16];   
   AEETSysBandClass   bandClass;
   uint8              ucPRevInUse;   
} AEETHDRDesc


Members:
 
   srvStatus        : service status (SERVICE/NO SERVICE) of the system
   sysMode          : system mode
   srvDomain        : service domain registered on the system
   srvCapability    : The system's service capability  
   roamStatus       : Indicate current roaming status
   usRssi           : HDR RSSI in positive dBm 
                      i.e. 75 means -75dBm 
                      The range is -105 to -90 dBm 
   ucaIs856         : HDR(1xEVDO) specific system ID
   bandClass        : Cellular or PCS
   ucPRevInUse      : Rev version used for EVDO network
} AEETHDRDesc;


Comments: 
   None

See Also:
   None
   
==============================================================================
  INTERFACES   DOCUMENTATION
==============================================================================

IServingNetworks Interface

IServingNetworks is a simple interface to telephony layer in the device.
It provides following services

- Getting available network and its information in details

- Registering for any changes in serving network

This interface only read serving networks and doesn't change phone state
or serving networks.


==============================================================================

IServingNetworks_GetSrvNetworks


Description:

This function provide all available networks basic information

The typical means to use this function in two steps

- First get number of currently available networks, 
  by passing NULL pNwInfo and 0 nSize.

- Second, get the each/all network basic information 


Prototype:

int IServingNetworks_GetSrvNetworks
   (
   IServingNetworks * pServingNetworks,
   AEETNwInfo       * pNwInfo,
   int                nSize,
   int              * nAvailableNetworks
   }
   
Parameters: 
   pServingNetworks    : [in]  Pointer to IServingNetworks interface object.
   pNwInfo             : [out] Pointer to buffer to which NwInfo is copied .
   nSize               : [in]  Size of pNwInfo buffer
   nAvailableNetworks  : [out] return the number of available networks 
    
   
Return Value:
   AEE values
   
Comments:
   If pNwInfo is NULL - it will return only number of available networks.
   If nSize is 0 - it will return only number of available networks.
   if nSize > 0 then will return network info.
   
   nSize should be in multiple size of AEETNwInfo.


==============================================================================

IServingNetworks_GetNwInfo

Description:

This function provides network information of a given network ID.


Prototype:

int IServingNetworks_GetNwInfo
   (
   IServingNetworks * pServingNetworks,
   AEETNetworkID      networkID,
   char             * pBuffer,
   int                nBuffLen,
   }
    
Parameters: 
   pServingNetworks   : [in]  Pointer to IServingNetworks interface object.
   networkID          : [in]  ID of network which information required.
   pBuffer            : [out] pointer to buffer to which requested network 
                               type structure (eg. AEETHDRDesc, AEETWLANDesc).
   nBuffLen           : [in]  Size of pBuffer.
     

Return Value:
   AEE values
    
Comments:
   bufLen should be size of requested network description (ex. AEETCDMADesc, 
   AEETGWDesc, AEETWLANDesc etc.)
  
See Also:
   IServingNetworks_GetSrvNetworks
   
==============================================================================

IServingNetworks_RegisterSignal

Description:

This function registers a signal which is set whenever any of the serving network
information changes (srvStatus, channel, usRssi etc)

Prototype:

int IServingNetworks_RegisterSignal
   (
   IServingNetworks   * pServingNetworks,
   ISignal            * piSignal
   }
    
Parameters: 
   pServingNetworks    : [in] Pointer to IServingNetworks interface object.
   piSignal            : [in] Pointer to signal which will be set when 
                              any of serving system information changed.
     
Return Value:
   AEE values
    
Comments:
   Default delta threshold RSSI will be considered for RSSI changes.
   RSSI-changes delta threshold can be modified by 
   IPHONECTL_SetRSSIDeltaThreshold

See Also:
   IPHONECTL_SetRSSIDeltaThreshold


============================================================================*/


#endif   /* ifndef AEEIServingNetworks_H */
