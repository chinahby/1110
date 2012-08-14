#ifndef __BTAPP_UTILS_H__
#define __BTAPP_UTILS_H__
/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppUtils.h                               */
/*                                                                           */   
/*                   SERVICES: Utility functions used by BT App.             */
/*                                                                           */
/*        GENERAL DESCRIPTION: This applet is the BT App of 3GUI.            */  
/*                                                                           */  
/*              Copyright (c) 2007-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/


/*=============================================================================

  $Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/inc/BTAppUtils.h#2 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who   what, where, why
--------    ---   ---------------------------------------------------------
2009-03-24  co    Redefined BTApp_DoSpeedDialLookup().
2008-04-27  rb    Fix compiler warnings

05/03/08    hs    Added for call checking in the macro IS_A2DP_CONNECTABLE()
02/06/08    gb    New function  declaration to format OOB Data status string
01/25/08    mh    Get full path name function prototype is changed.
12/04/07    mh    Added support to select low power configuration.
11/01/07    ag    Merged Lisbon(BT2.1) code to the mainline
03/27/07    pn    Added IS_AG_CONNECTABLE() and IS_A2DP_CONNECTABLE().
03/14/07    ns    Replaced ICM with ITelephone
10/19/06    pg    Added LMP version and subversion to MyInfo screen
05/08/06    ns    Added support to perform role switch operation
03/08/06    dc    Added a function to convert string to BD Address.
02/20/06    jh    Added prototype for BTA2DPDeviceSelected()
09/28/05    ns    Added debug message to track user events posted
09/15/05    ns    Extra parameter added for function UseBTDevice
08/19/05    ns    Added addition parameter for BTApp_ReleaseBTDevice which is 
                  used while audio tranfer
05/12/05    pn    Added BTApp_BTAudioSelected().
04/29/05    pn    Removed unnecessary include files.
03/25/05    pn    Added BTApp_SetBondable() and BTApp_ClearBondable().
03/17/05    jtl   Added A2DP Use/ReleaseDevice
01/11/05    pn    Changed btapp.bid to BTApp.bid.
12/17/04    pn    Added BTAppCallType, BTApp_CallPresent(), 
                  BTApp_CallIncoming() and BTApp_CallConnected().
11/08/04    pn    Removed unused include files.
10/10/04    pn    Added BTApp_ShowDevMsg().
07/23/04    pn    - Added BTAPP_POST_USER_EVENT and BTApp_GetFullPathName().
05/25/04    pn    - Added BTApp_GetDevTypeImageID().
05/04/04    pn    - Added support for setting local unit's discoverable mode
04/30/04    pn    - Added support for OPP and FTP tests
02/20/04    pn    - Added support for displaying SPP test progress
                  - Added BTApp_FormatBondable()
02/17/04    pn    Added BTApp_UseBTDevice() and BTApp_ReleaseBTDevice()
01/14/04    pn    - Added functions BTApp_FormatBTName(), 
                    BTApp_FormatBDAddress(), BTApp_FormatSecurity(), 
                    BTApp_FormatSvcCls(), and BTApp_FormatLMPFeatures.
12/03/03    pn    Externalized BTApp_MessageTimerExpired().
11/26/03    pn    Changed signature of BTApp_ShowMessage().
11/05/03    pn    Initial Revision.
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"
#include "AppComFunc.h"

#include "AEEModGen.h"          /* Module interface definitions */
#include "AEEAppGen.h"          /* Applet interface definitions */
#include "AEEShell.h"           /* Shell interface definitions */
#include "AEEFile.h"            /* File interface definitions */
#include "AEEStdLib.h"          /* AEE Stb Lib Services */
#include "AEEMenu.h"            /* AEE Menu Services */
#include "AEEText.h"

#include "btapp_res.h"
#include "BTApp.bid"

#include <stdlib.h>


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
// menu background color
#define SOFT_MENU_COLOR MAKE_RGB(255, 255, 204)

// selected menu background color
#define SOFT_MENU_SEL_COLOR MAKE_RGB(153, 204, 204);

#define CLEAR_SCREEN() (IDISPLAY_ClearScreen( pMe->m_pIDisplay ))

#define BTAPP_POST_USER_EVENT( profile, ev ) \
  MSG_FATAL( "ISHELL_PostEventEx profile=%d, ev=%d", EVT_##profile, ev, 0 ); \
  ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, \
                      AEECLSID_BLUETOOTH_APP,         \
                      EVT_USER, EVT_##profile, ev )
                               
#define IS_AG_CONNECTABLE() ( (pMe->mAG.bEnabled != FALSE) &&    \
                              ((pMe->mAG.bConnected != FALSE) || \
                               BDADDR_VALID( &pMe->mAG.bdAddr )) )

#ifdef FEATURE_BT_EXTPF_AV
#define IS_A2DP_CONNECTABLE() ( (pMe->mA2DP.bEnabled != FALSE) &&    \
                                ((pMe->mA2DP.bConnected != FALSE) || \
                                 BDADDR_VALID( &pMe->mA2DP.bdAddr )) && \
                                (!(BTApp_CallPresent( pMe )) && \
                                (!BTApp_CallIncoming( pMe ))) ) 
#endif // FEATURE_BT_EXTPF_AV

/* BT APP Manager */
typedef struct _BTappMgr
{
  AEEBTBDAddr           bdAddr;
  uint32                profileType;  /* Profile Type */
  uint32                stateType;    /* State Type */
  AEEBTLowPowerControl  currentLPMode;/* Current low power mode*/ 
  uint8                 numProfiles ; /* Number of profiles */
  boolean               bValid;       /* Valid Connection Entry */
} BTappMgr;

#define  BTAPP_MGR_DISCONNECTED    0
#define  BTAPP_MGR_CONNECTED       1

#define BTAPP_MGR_AG                 0x00000001
#define BTAPP_MGR_A2DP               0x00000002
#define BTAPP_MGR_SAP                0x00000004
#define BTAPP_MGR_FTP                0x00000008
#define BTAPP_MGR_OPP                0x00000010
#define BTAPP_MGR_BIP                0x00000020
#define BTAPP_MGR_BPP                0x00000040
#define BTAPP_MGR_PBAP               0x00000080

#define BTAPP_MGR_USE_A2DP           1
#define BTAPP_MGR_RELEASE_A2DP       2

typedef uint8     AEEBTLowPowerControl;

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
typedef enum
{
  BT_APP_CALL_NONE,   // no call
  BT_APP_CALL_VOICE,  // voice call
  BT_APP_CALL_VT,     // video telephony call
  BT_APP_CALL_MAX
} BTAppCallType;

/*===========================================================================

                    STATIC DATA

===========================================================================*/


/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/
//extern void BTApp_SetMenuStyle( IMenuCtl* pIMenu, int so_x, int so_y );
extern void BTApp_InitAddItem( CtlAddItem* pItem );
extern void BTApp_AddMenuItem( CBTApp* pMe, IMenuCtl* pMenu, CtlAddItem* pItem, 
                               uint16 stringID, uint16 imageID );
extern void BTApp_UpdateMenuItemImage( IDisplay*, IMenuCtl*, uint16, uint16 );
extern void BTApp_BDAddr2Wstr( AECHAR* pwStr, const AEEBTBDAddr* pAddr );
extern void BTApp_SvcCls2Wstr( AECHAR* pwBuf, uint16 uNumBytes, 
                               AEEBTServiceClass svcCls );
extern void BTApp_LMPFeat2Wstr( AECHAR* pwBuf, uint16  uNumBytes, 
                                uint8*  pLMPFeatures );
extern uint16 BTApp_FormatBTName( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                  AECHAR* pwName );
#ifdef FEATURE_BT_2_1
extern uint16 BTApp_FormatSSPCapable( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                      boolean bSSPCapable );
extern uint16 BTApp_FormatLinkKey( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                   uint8 linkKeyType );
extern uint16 BTApp_FormatPasskey( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                   AECHAR* pwPasskey, AECHAR* pwRemotePasskey );
extern uint16 BTApp_FormatBTShortName( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                       AECHAR* pwName, boolean bNameSame );
extern uint16 BTApp_FormatManuData( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                    AECHAR* pwName );
extern uint16 BTApp_FormatUUID( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                AEEBTDeviceInfo*  pDev );

extern uint16 BTApp_FormatLowPowerMode( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                        AEEBTLowPowerControl pwrMode );
extern uint16 BTApp_FormatMaxInterval( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                       AECHAR* wMaxInterval );
extern uint16 BTApp_FormatIdleTieout( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                      AECHAR*  wIdleTimeout );
extern uint16 BTApp_FormatMaxLatency( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                      AECHAR*  wMaxLatency );
extern uint16 BTApp_FormatSniffParams( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                       uint16  msgID, boolean       bflag );
extern uint16 BTApp_FormatOOBDataStr( CBTApp* pMe, AECHAR* pwStr,uint16 uLen ,
                                      boolean bOOBDataPrsent );
#endif
extern uint16 BTApp_FormatBDAddress( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                     AEEBTBDAddr* pAddr );
extern uint16 BTApp_FormatSecurity( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                    AEEBTSecurity sec );
extern uint16 BTApp_FormatBondable( CBTApp* pMe, AECHAR* pwStr, uint16  uLen,
                                    boolean bBondable );
extern uint16 BTApp_FormatDiscoverable( CBTApp* pMe, AECHAR* pwStr, 
                                        uint16  uLen, boolean bBondable );
extern uint16 BTApp_FormatSvcCls( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                  AEEBTServiceClass svcCls );
extern uint16 BTApp_FormatLMPFeatures( CBTApp* pMe, AECHAR* pwStr, uint16 uLen,
                                       uint8* pLMPFeatures );
extern uint16 BTApp_FormatLMPVersion( CBTApp* pMe, AECHAR* pwStr, uint16 uLen );
extern uint16 BTApp_FormatLMPSubVersion( CBTApp* pMe, AECHAR* pwStr, uint16 uLen );
extern uint16 BTApp_FormatPortStatus( CBTApp* pMe, AECHAR* pwStr, uint16 uLen );
extern uint16 BTApp_FormatBytesSent( CBTApp* pMe, AECHAR* pwStr, uint16 uLen );
extern uint16 BTApp_FormatBytesRcvd( CBTApp* pMe, AECHAR* pwStr, uint16 uLen );
extern uint16 BTApp_FormatDevRole( CBTApp* pMe, AECHAR* pwStr, uint16  uLen, 
                                   boolean bMaster );
extern uint16 BTApp_NumKey2Selection( IMenuCtl* pMenu, uint16 key );
extern uint16 BTApp_GetDevTypeImageID( CBTApp* pMe, AEEBTServiceClass svcCls,
                                       AEEBTBDAddr* pBDAddr );

extern void BTApp_GetFullPathName( char* pszName, uint16 nameBufSize,  
                                   char* pszFolder, uint16 folderBufSize ) ;
extern void BTApp_ShowBusyIcon( CBTApp* pme );
extern void BTApp_ShowMessage( CBTApp* pMe, uint16 msgID, AECHAR* wArg, 
                               uint8 secondsUp );
extern void BTApp_ShowDevMsg( CBTApp* pMe, uint16 msgID, 
                              const AEEBTBDAddr* pAddr, uint8 secondsUp );
extern void BTApp_MessageTimerExpired( CBTApp* pMe );
extern boolean BTApp_BTAudioSelected( CBTApp* pMe );
extern void BTApp_UseBTDevice( CBTApp* pMe, boolean bForceUnmute );
extern void BTApp_ReleaseBTDevice( CBTApp* pMe, boolean bForceUnmute );
#ifdef FEATURE_BT_EXTPF_AV
extern void BTApp_UseA2DPDevice( CBTApp* pMe );
extern void BTApp_ReleaseA2DPDevice( CBTApp* pMe );
extern boolean BTA2DPDeviceSelected( void );
#endif /* FEATURE_BT_EXTPF_AV */
extern BTAppCallType BTApp_CallPresent( CBTApp* pMe );
extern BTAppCallType BTApp_CallIncoming( CBTApp* pMe );
extern BTAppCallType BTApp_CallConnected( CBTApp* pMe );
extern void BTApp_SetBondable( CBTApp *pMe );
extern void BTApp_ClearBondable( CBTApp *pMe );
extern boolean BTApp_Str2BDAddr( const char* pStr, AEEBTBDAddr* pAddr );
extern void BTApp_DoSpeedDialLookup( IShell *pIShell, AECHAR *speedialindex,
                                     AECHAR *number, uint32 size);
extern boolean BTApp_GetBatteryLevel( CBTApp *pMe , uint8* battLevel );
extern void BTApp_CheckToClearDiscoverable( CBTApp* pMe );
extern boolean BTApp_IsBTAppOn(CBTApp* pMe );
extern boolean BTApp_IsDiscoverabilityReq(CBTApp* pMe );


extern void BTAppMgr_UpdateProfileType( CBTApp* pMe, AEEBTBDAddr* pBDAddr, uint32 profileType, 
                                  uint8 state );
extern void BTAppMgr_UpdateStateType( CBTApp* pMe, AEEBTBDAddr* pBDAddr, uint32 stateType );

#endif // __BTAPP_UTILS_H__
