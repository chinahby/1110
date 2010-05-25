#ifndef __OEMPOSDET_H__
#define __OEMPOSDET_H__
/*============================================================================

FILE:  OEMPosDet.h


==============================================================================

               Copyright © 1999-2003 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#include "AEEComdef.h"
#include "AEEPosDet.h"

typedef struct _OEMPosDet OEMPosDet;

#define  POSDET_E_NONE                        0x0000
#define  POSDET_E_OEM_SESSION_DONE            0x0001
#define  POSDET_E_OEM_SESSION_DOWNLOAD_DONE   0x0002
#define  POSDET_E_OEM_SESSION_BUSY            0x0004
#define  POSDET_E_OEM_SESSION_ERROR           0x0008
#define  POSDET_E_OEM_SESSION_ABORT           0x0010
#define  POSDET_E_OEM_RECEIVER_BUSY           0x0020

typedef unsigned short PosDetEventType;

/* Sector Based position determination info, version 1.1 */
typedef struct AEESectorInfo_11
{
   uint16 wSysID;             /* System Identification */
   uint16 wNetID;             /* Network Identification */
   uint16 wBaseID;            /* Base Station Identification */
   uint16 wBaseClass;         /* Base Station Class */
   uint16 wBestPN;            /* Best Pilot */
} AEESectorInfo_11;
/* Sector Based position determination info, version 2.0 */
typedef struct AEESectorInfo_20
{
   uint16 wSysID;             /* System Identification */
   uint16 wNetID;             /* Network Identification */
   uint16 wBaseID;            /* Base Station Identification */
   uint16 wBaseClass;         /* Base Station Class */
   uint16 wBestPN;            /* Best Pilot */
   uint16 wPacketZoneID;      /* Packet Data Service Zone Identifier */
   uint16 wMobileCountryCode; /* Mobile country code */
} AEESectorInfo_20;
/* Sector Based Position determination info, version 2.1 */
typedef struct AEESectorInfo_21
{
   uint16 wSysID;             /* System Identification */
   uint16 wNetID;             /* Network Identification */
   uint16 wBaseID;            /* Base Station Identification */
   uint16 wBaseClass;         /* Base Station Class */
   uint16 wBestPN;            /* Best Pilot */
   uint16 wPacketZoneID;      /* Packet Data Service Zone Identifier */
   uint16 wMobileCountryCode; /* Mobile country code */
   int32 dwBaseLatitude;      /* Base Station Latitude */
   int32 dwBaseLongitude;     /* Base Station Longitude */
} AEESectorInfo_21;

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************/
/* Services available for OEM layer in PosDet object */
/*****************************************************/
typedef void (*PFN_PD_NOFITY)( void *pUser, PosDetEventType theEvent );

/*******************************************/
/* Services to be implemented by OEM layer */
/*******************************************/
/* Creates and opens the instance with OEM layer */
int OEM_PD_Open( void *pUser, PFN_PD_NOFITY pfn, OEMPosDet **ppo );
/* Closes and releases the instance with OEM layer */
int OEM_PD_Close( OEMPosDet *pMe );
/* Starts a new session for the requested information */
int OEM_PD_InitiateNewSession( OEMPosDet *pMe, const AEEGPSConfig *pConfig, AEEGPSReq req, AEEGPSAccuracy accuracy, AEECLSID cls );
/* Returns the information available */
int OEM_PD_GetPositionInfo( OEMPosDet *pMe, AEEGPSInfo *pInfo );
/* End the session. */
int OEM_PD_EndSession( OEMPosDet *pMe, boolean bStayWarm );
/* When applet releases IPosDet */
int OEM_PD_PosDetRelease( AEECLSID clsApplet );

/* Validate the parameters */
int OEM_PD_Validate_GetGPSInfo( AEEGPSReq req, AEEGPSAccuracy accuracy );
int OEM_PD_Validate_SetGPSConfig( const AEEGPSConfig *pConfig );

/* Get the default configuration settings */
int OEM_PD_GetDefaultConfig( AEEGPSConfig *pConfig, uint32 *pdwTimeout, uint32 *pdwLinger );

/* TRUE if device shares the GPS receiver with voice communication */
boolean OEM_PD_SharedVoiceReceiver( void );

/* Get the serving basestation info. */
int OEM_PD_GetBaseStationInfo( AEESectorInfo *psi, uint16 wSize );

/* Get the Orientation of the device. */
int OEM_OR_GetOrientation( AEEOrientationReq req, void *pInfo, AEECallback *pcb );

#ifdef __cplusplus
}
#endif

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

PosDetEventType

Description:
This data type is used to inform PosDet of the events.

Definition:

typedef unsigned short PosDetEventType;

Supported Values:
   POSDET_E_NONE                        : No events.
   POSDET_E_OEM_SESSION_DONE            : A position request session is complete.
   POSDET_E_OEM_SESSION_DOWNLOAD_DONE   : A data download session is complete.
   POSDET_E_OEM_SESSION_BUSY            : gpsOne engine is busy to start a new session.
   POSDET_E_OEM_SESSION_ERROR           : An error occured trying to start a new session.
   POSDET_E_OEM_SESSION_ABORT           : A session in progress has been aborted.

Comments:
None

See Also:
PosDet_Wakeup();

=======================================================================
INTERFACES   DOCUMENTATION
=======================================================================
PosDet Interface
=======================================================================
Function: PosDet_Wakeup()

Description:
  Invoke this routine to inform PosDet of the events to process.

Prototype:

   void PosDet_Wakeup( PosDet *pPosDet, PosDetEventType theEvent );

Parameters:
   pPosDet: [in]. Pointer to the PosDet object.
   theEvent: [in]. The event to report.

Return Value:

   None.

Comments:

Side Effects:
   None

See Also:
   PosDetEventType

=======================================================================
Function: OEM_PD_Open()

Description:
  Opens the instance of Position Determination (PD) engine in the System Software.
  Only one instance is expected to be open at a time.

Prototype:

   int OEM_PD_Open( PosDet *pi, OEMPosDet **po );

Parameters:
   pPosDet: [in]. Pointer to the PosDet object.
   po: [out]. Handle to instance of Position Determination engine.

Return Value:

   SUCCESS.
   EBADPARM.
   ENOMEMORY.
   EFAILED.

Comments:
   OEM_PD_Close is definitely invoked on a successfully opened Instance.

Side Effects:
   None

See Also:
   OEM_PD_Close()
=======================================================================
Function: OEM_PD_Close()

Description:
  Close the instance of Position Determination engine created using OEM_PD_Open.

Prototype:

   int OEM_PD_Close( OEMPosDet *pMe );

Parameters:
   pMe: [in]. Handle of the instance obtained using OEM_PD_Open.

Return Value:

   SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   OEM_PD_Open()
=======================================================================
Function: OEM_PD_InitiateNewSession()

Description:
  Initiates a new position determination session.

Prototype:

   int OEM_PD_InitiateNewSession( OEMPosDet *pMe, const AEEGPSConfig *pConfig, AEEGPSReq req, AEEGPSAccuracy accuracy, AEECLSID cls );

Parameters:
   pMe: [in]. Handle of the instance obtained using OEM_PD_Open.
   pConfig:
   req:
   accuracy:
   cls: [in]  AEECLSID of the application initiating the Position Determination Request.
        Used to support user plane LBS authorization.

Return Value:

   SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
=======================================================================
Function: OEM_PD_GetPositionInfo()

Description:
  Obtain the latest position if available. Doesnot start a session if unavailable.

Prototype:

   int OEM_PD_GetPositionInfo( OEMPosDet *pMe, AEEGPSInfo *pInfo );

Parameters:
   pMe: [in]. PD Handle.
   pInfo: [out]. Populate position related information.

Return Value:

   SUCCESS   : If the information is available.

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================
Function: OEM_PD_EndSession()

Description:
  Terminate the Position determination session if any in progress.

Prototype:

   int OEM_PD_EndSession( OEMPosDet *pMe, boolean bStayWarm );

Parameters:
   pMe: [in]. PD Handle
   bStayWarm: [in]. TRUE to terminate the stay warm session, FALSE to
         terminate position fix session.

Return Value:

   SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================
Function: OEM_PD_PosDetRelease()

Description:
  When BREW applet releases its last reference to an instance of IPosDet.

Prototype:

   int OEM_PD_PosDetRelease( AEECLSID clsApplet );

Parameters:
   clsApplet: [in]. Class ID of applet. Used to support user plane 
              LBS authorization.

Return Value:

   SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================
Function: OEM_PD_Validate_GetGPSInfo()

Description:
  Validate the paramters passed. Invoked by IPosDet to validate the supported
  feature set on the device.

Prototype:

   int OEM_PD_Validate_GetGPSInfo( AEEGPSReq req, AEEGPSAccuracy accuracy );

Parameters:
   req: [in]. requested position information.
   accuracy: [in]. accuracy requested.

Return Value:

   SUCCESS
   EUNSUPPORTED

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEM_PD_Validate_SetGPSConfig()

Description:
  Invoked by IPosDet to validate the parameters and their support on the
  device.

Prototype:

   int OEM_PD_Validate_SetGPSConfig( const AEEGPSConfig *pConfig );

Parameters:
   pConfig: [in]. the configuration.

Return Value:

   SUCCESS
   EUNSUPPORTED
   EBADPARM

Comments:

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEM_PD_GetDefaultConfig()

Description:
  Provide the default configuration data.

Prototype:
   int OEM_PD_GetDefaultConfig( AEEGPSConfig *pConfig, uint32 *pdwTimeout, uint32 *pdwLinger );

Parameters:
   pConfig: [out]. Pointer to the AEEGPSConfig
   pdwTimeout: [out]. Timeout in seconds to be used on position requests. On every new
               session started with OEM_PD_InitiateNewSession, this timer is started. When
               this timer expires AEEGPS_ERR_TIMEOUT is posted to the requesting application.
   pdwLinger: [out]. Timeout in seconds used to keep the OEM session instance. When
               there are no pending service requests from Application this timer is
               started. When this timer expires OEM_PD_Close() is invoked.

Return Value:
   SUCCESS

Comments:
   None.

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEM_PD_SharedVoiceReceiver()

Description:
  Indicate whether the GPS reciever is shared for voice communication. If 
this function returns TRUE, IPosDet sessions monitor for any voice calls and
raise a status code AEEGPS_ERR_RECEIVER_BUSY to applications.

Prototype:
   boolean OEM_PD_SharedVoiceReceiver( void );

Parameters:

Return Value:
   TRUE if device shares the GPS reciever with voice communication.

Comments:
   When using a shared receiver, a position determination process may cause 
the receiver tune to GPS frequencies when needed. Such tune away during voice
communication may affect the voice quality. Enabling this function (with a 
return value of TRUE) will prevent IPosDet sessions interfere with voice 
communication.

Side Effects:
   None

See Also:
   AEEGPS_ERR_RECEIVER_BUSY

=======================================================================
Function: OEM_PD_GetBaseStationInfo()

Description:
  Provide the base station information.

Prototype:

   int OEM_PD_GetBaseStationInfo( AEESectorInfo *psi, uint16 wSize );

Parameters:
   psi: [out]. Pointer to the AEESectorInfo
   wSize: [in]. The size of structure AEESectorInfo

Return Value:

   SUCCESS
   EBADPARM

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: OEM_OR_GetOrientation()

Description:
  Provide the orientation information.

Prototype:

   int OEM_OR_GetOrientation( AEEOrientationReq req, void *pInfo, AEECallback *pcb );

Parameters:
   req: [in]. the request code.
   pInfo: [out]. the response data.
   pcb: callback to AEE_ResumeCallback() when response is available. A cancel on this callback
        stops the request.

Return Value:

   SUCCESS
   EBADPARM

Comments:
   None

Side Effects:
   None

See Also:
  IPOSDET_GetOrientation()

==============================================================
AEEPosDet

See the IPosDet Interface in the BREW API Reference.

=======================================================================*/

#endif /* !__OEMPOSDET_H__ */
