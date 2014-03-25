/*****************************************************************************/
/*                                                                           */
/*                          FILE: BTAppUtils.c                               */
/*                                                                           */   
/*                   SERVICES: QCT UI BT BREW Application.                   */
/*                                                                           */
/*        GENERAL DESCRIPTION: Utility functions used by BT App.             */  
/*                                                                           */  
/*              Copyright (c) 2003-2009 QUALCOMM Incorporated.               */
/*                      All Rights Reserved.                                 */ 
/*              Qualcomm Confidential and Proprietary                        */
/*****************************************************************************/
/*===========================================================================
                        EDIT HISTORY FOR MODULE
                        
$Header: //source/qcom/qct/wconnect/bthost/brew/statapp/rel/00.00.26/src/BTAppUtils.c#3 $
$DateTime: 2009/06/03 12:43:40 $
$Author: phuongn $

when        who   what, where, why
--------    ---   ----------------------------------------------------------
06/01/09    pn    Fixed critical Lint errors.
03/24/09    co    Use IContacts for speeddial lookup on BMP builds.
12/23/08    pn    EIR flag is checked before UUID list is pulled from driver.
10/08/08    rs    Removing the usage of deprecated functions.
05/21/08    co    Not to send redundant QXDM msg when use/release BT device.
                  Mutually excluded A2DP and SCO devices.
04/01/08    co    Ignored mute settings if SCO REWOEK is enabled.
02/06/08    gb    New function  definition to format OOB Data status string
01/25/08    mh    Replacing strcpy and other deprecated functions.
01/02/08    dsn   Remove coreapp dependency from BTApp.
12/04/07    mh    Added support to select low power configuration. 
11/01/07    ag    Merged Lisbon(BT2.1) code to the mainline
05/30/07    jj    Fixed the problem of setting of sound devices with ui_snd_xxx
                  calls,used URLs to set the sound devices.
05/17/07    pn    Replaced uisnd_set_device_specific() with 
                  uisnd_set_device_auto().
03/30/07    pn    Fixed problem of snd device not getting unmuted in some
                  scenarios
03/14/07    ns    Replaced ICM with ITelephone
01/16/07    pn    Replaced uisnd_set_mute() with the non-reference counted 
                  uisnd_force_device_mute().
10/26/06    vk    Added check for PBAP and AVRCP in BTApp_ClearBondable function.
10/19/06    pg    Added LMP version and subversion to MyInfo screen
09/12/06    pn    Made BTApp work with and without SCO_REWORK enabled.
08/14/06    dsn   Remove code stubbed out for SoC audio bring-up.
07/31/06    ns    Added changes related to audio rework feature
07/07/06    pn    Fixed problem of Audio connection being brought up
                  unintentionally.
04/11/06    pn    Updated UseA2DPDevice() and ReleaseA2DPDevice() to have the
                  device selection happen immediately.
05/08/06    ns    Added support to perform role switch operation
04/11/06    pn    Changed UseBTDevice() and ReleaseBTDevice() to properly set/
                  unset vocoder muting.
03/08/06    dc    Added a function to convert string to BD Address.
02/15/06    pn    UseBTDevice() now uses BTA2DPDeviceSelected() to find out
                  whether to use BT_A2DP_SCO sound device type.
01/09/06    pn    Fixed featurization so BTApp can build without ICM.
12/14/05   dsn    Added featurization for MSM7500 BT bringup.
10/21/05    ns    Moved ICM handle out of AG as it is used across profiles
10/14/05    pn    Modified UseBTDevice and ReleaseBTDevice for the case A2DP
                  and AG are both used.
09/15/05    ns    Modified UseBTDevice for scenarios where force mute is not 
                  required
08/19/05    ns    Modified BTApp_UseBTDevice and BTApp_ReleaseBTDevice for
                  using new sound interfaces for setting device and muting
                  Added addition parameter for BTApp_ReleaseBTDevice() which 
                  is used while audio tranfer
06/16/05    ns    Fixed problem in displaying the numbers when menu items 
                  are more than nine in function BTApp_AddMenuItem().
06/16/05    sn    Fixed issue with key presses such as CLR, UP and etc when 
                  number of items displayed on the menu is more than 9.
05/12/05    pn    Added BTApp_BTAudioSelected().
03/25/05    pn    Added BTApp_SetBondable() and BTApp_ClearBondable().
03/17/05    jtl   Added A2DP Use/ReleaseDevice
01/31/05    pn    Used UI_SND_DEV_MGMT to determine which UI sound API to use.
12/17/04    pn    Added BTApp_CallPresent(), BTApp_CallIncoming(), and
                  BTApp_CallConnected().
11/08/04    pn    Changed BTApp_res.h to btapp_res.h.
10/10/04    pn    Updated BTApp_GetDevTypeImageID(). Added BTApp_ShowDevMsg().
                  Set flag when busy icon is displayed.
09/23/04    pn    Sets bBusyIconUp when displaying busy icon.  No longer clears
                  screen before displaying busy icon.
08/10/04    pn    Modified BTApp_ShowMessage to accept string not from resource
                  file.
07/27/04    pn    Added BTApp_GetFullPathName().
07/26/04    pn    Added include file BTApp_res.h
05/25/04    pn    Added BTApp_GetDevTypeImageID().
05/20/04    pn    Made 3G features target independent
05/04/04    pn    - Added support for setting local unit's discoverable mode
04/30/04    pn    - Added support for OPP and FTP tests
02/27/04    pn    - Fixed featurization problem
02/20/04    pn    - Added support for displaying SPP test progress
                  - Added BTApp_FormatBondable()
02/17/04    pn    - Added BTApp_UseBTDevice() and BTApp_ReleaseBTDevice()
01/14/04    pn    - Added functions BTApp_FormatBTName(), 
                    BTApp_FormatBDAddress(), BTApp_FormatSecurity(), 
                    BTApp_FormatSvcCls(), and BTApp_FormatLMPFeatures.
                  - Modified BTApp_UpdateMenuItemImage()
12/03/03    pn    - Used appropriate lengths for preallocated text buffers.
                  - Externalized BTApp_MessageTimerExpired().
11/26/03    pn    General clean up.
09/26/03    pn    Initial Revision for BT App.

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_APP_BLUETOOTH

#include "BTApp_priv.h"		//"BTApp.h"

#include "BTAppUtils.h"
#include "btapp_res.h"

#include "AEEStd.h"

#ifdef FEATURE_BMP
#include "pim_IContacts.h"
#include "pim_IContactsSearch.h"
#include "pim_IContactsResults.h"
#include "pim_IContactsRecord.h"
#include "pim_EFSDBDevice.bid"
#include "pim_ContactsSearch.bid"
#endif /* FEATURE_BMP */

#ifdef FEATURE_UI_CORE
#include "uixsnd.h"
#endif // FEATURE_UI_CORE

#define BTAPP_VOICECALL_INCOMING        0
#define BTAPP_VOICECALL_PRESENT         1
#define BTAPP_VOICECALL_CONNECTED       2

#define BTAPP_IS_VOICECALL_INCOMING(po)                   \
  (BTApp_CheckVoiceCallState( po, BTAPP_VOICECALL_INCOMING))

#define BTAPP_IS_VOICECALL_PRESENT(po)                   \
  (BTApp_CheckVoiceCallState( po, BTAPP_VOICECALL_PRESENT))

#define BTAPP_IS_VOICECALL_CONNECTED(po)                   \
  (BTApp_CheckVoiceCallState( po, BTAPP_VOICECALL_CONNECTED))

#ifdef FEATURE_BT_VT
#define AEEVP_IS_VIDEOCALL_INCOMING(po)                   \
  (IVIDEOPHONE_GetActiveCallID( po,                       \
                                (AEEVP_CALL_STATE_INCOM), \
                                NULL ) > 0 ? TRUE:FALSE)
#endif /* FEATURE_BT_VT */

/* Application Manager Data*/
BTappMgr appMgr[7];

/* ==========================================================================
FUNCTION BTAPP_INITADDITEM
DESCRIPTION
============================================================================= */
void BTApp_InitAddItem( CtlAddItem* pItem )
{
  pItem->pImage       = NULL;
  pItem->pszResImage  = AEE_APPSBTAPP_RES_FILE; 
  pItem->pszResText   = AEE_APPSBTAPP_RES_FILE;
  pItem->wFont        = AEE_FONT_NORMAL;
  pItem->dwData       = 0;
}

/* ==========================================================================
FUNCTION BTAPP_ADDMENUITEM
DESCRIPTION
============================================================================= */
void BTApp_AddMenuItem( 
  CBTApp*     pMe,
  IMenuCtl*   pMenu, 
  CtlAddItem* pItem, 
  uint16      stringID,
  uint16      imageID
)
{
  uint8  len = 0;
  int    idx = IMENUCTL_GetItemCount( pMenu ) + 1;

  pItem->wItemID  = stringID;
  pItem->wImage   = imageID;
  pItem->dwData   = AVK_0 + idx;

  if ( pItem->wImage == 0 )
  {
    pItem->pszResImage = NULL;
  }

  if ( (pMenu == pMe->m_pIMenu) && (pItem->wImage == 0) )
  {
    if ( idx < 10 )
    {
      pMe->pText1[ len++ ] = (AECHAR)(unsigned char) ('0' + idx);
    }
    else
    {
      pMe->pText1[ len++ ] = (AECHAR)(unsigned char) ('0' + (idx/10));
      pMe->pText1[ len++ ] = (AECHAR)(unsigned char) ('0' + (idx%10));
    }
    pMe->pText1[ len++ ] = (AECHAR)(unsigned char) ('.');
    pMe->pText1[ len++ ] = (AECHAR)(unsigned char) (' ');
    pMe->pText1[ len ] = (AECHAR)(unsigned char) ('\0');

    len = WSTRLEN( pMe->pText1 );

    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, stringID, 
                          &pMe->pText1[ len ], 
                          (LONG_TEXT_BUF_LEN-len) * sizeof(AECHAR) );
    pItem->pText = pMe->pText1;
  }
  else
  {
    pItem->wText = stringID;
    pItem->pText = NULL;
  }

  if ( IMENUCTL_AddItemEx( pMenu, pItem ) == FALSE )
  {
    MSG_ERROR( "AddMenuItem - Failed to add %d", stringID, 0, 0 );
  }
}


/* ==========================================================================
FUNCTION BTAPP_BDADDR2WSTR
DESCRIPTION
============================================================================= */
void BTApp_BDAddr2Wstr
(
  AECHAR*       pwStr,
  const AEEBTBDAddr*  pAddr
)
{

  uint8   i, p = sizeof( AEEBTBDAddr ) * 2;
  uint16  j, k;

  pwStr[p] = (AECHAR)(unsigned char) ('\0');

  for ( i = 0; i < sizeof( AEEBTBDAddr ); i++ )
  {
    j = (uint16) pAddr->uAddr[i];
    k = j & 0x000F;
    pwStr[--p] = (AECHAR)(unsigned char) (( k <= 9 ) ? k + '0' : k - 10 + 'A');
    k = ( j >> 4 ) & 0x000F;
    pwStr[--p] = (AECHAR)(unsigned char) (( k <= 9 ) ? k + '0' : k - 10 + 'A');
  }
}

/* ==========================================================================
FUNCTION BTAPP_SVCCLS2WSTR
DESCRIPTION
============================================================================= */
void BTApp_SvcCls2Wstr( 
  AECHAR* pwBuf, 
  uint16 uNumBytes, 
  AEEBTServiceClass svcCls
)
{
  uint16 uLen = 0;

  if ( svcCls == AEEBT_COD_SC_ALL )
  {
    STRTOWSTR( "All", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    return;
  }
  if ( svcCls == AEEBT_COD_SC_UNKNOWN )
  {
    STRTOWSTR( "Unknown", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    return;
  }

  if ( svcCls & AEEBT_COD_SC_LIMITED_DISCOVERABLE_MODE )
  {
    STRTOWSTR( "L,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_NETWORKING )
  {
    STRTOWSTR( "N,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_RENDERING )
  {
    STRTOWSTR( "R,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_CAPTURING )
  {
    STRTOWSTR( "C,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_OBJECT_TRANSFER )
  {
    STRTOWSTR( "O,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_AUDIO )
  {
    STRTOWSTR( "A,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_TELEPHONY )
  {
    STRTOWSTR( "T,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_INFORMATION )
  {
    STRTOWSTR( "I,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( svcCls & AEEBT_COD_SC_POSITIONING )
  {
    STRTOWSTR( "P,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }

  pwBuf[ uLen - 1 ] = (AECHAR)(unsigned char) ('\0'); // remove last comma
}

/* ==========================================================================
FUNCTION BTAPP_LMPFEAT2WSTR
DESCRIPTION
============================================================================= */
void BTApp_LMPFeat2Wstr( 
  AECHAR* pwBuf, 
  uint16  uNumBytes, 
  uint8*  pLMPFeatures
)
{
  uint16 uLen = 0;
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_3_SLOT_PKTS, pLMPFeatures ) )
  {
    STRTOWSTR( "3SP,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_5_SLOT_PKTS, pLMPFeatures ) )
  {
    STRTOWSTR( "5SP,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_ENCRYPT, pLMPFeatures ) )
  {
    STRTOWSTR( "Enc,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_SLOT_OFFSET, pLMPFeatures ) )
  {
    STRTOWSTR( "SOf,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_TIMING_ACCURACY, pLMPFeatures ) )
  {
    STRTOWSTR( "TAc,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_SWITCH, pLMPFeatures ) )
  {
    STRTOWSTR( "Swi,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_HOLD_MODE, pLMPFeatures ) )
  {
    STRTOWSTR( "HMo,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_SNIFF_MODE, pLMPFeatures ) )
  {
    STRTOWSTR( "SMo,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_PARK_MODE, pLMPFeatures ) )
  {
    STRTOWSTR( "PMo,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_RSSI, pLMPFeatures ) )
  {
    STRTOWSTR( "Rss,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_AUTO_DATA_RATE, pLMPFeatures ) )
  {
    STRTOWSTR( "ADR,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_SCO_LINK, pLMPFeatures ) )
  {
    STRTOWSTR( "SCO,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_HV2_PKTS, pLMPFeatures ) )
  {
    STRTOWSTR( "HV2,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_HV3_PKTS, pLMPFeatures ) )
  {
    STRTOWSTR( "HV3,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_U_LAW_LOG, pLMPFeatures ) )
  {
    STRTOWSTR( "ULL,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_A_LAW_LOG, pLMPFeatures ) )
  {
    STRTOWSTR( "ALL,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_CVSD, pLMPFeatures ) )
  {
    STRTOWSTR( "Cvs,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_PAGING_SCHEME, pLMPFeatures ) )
  {
    STRTOWSTR( "PSc,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_POWER_CTRL, pLMPFeatures ) )
  {
    STRTOWSTR( "PCo,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  if ( AEEBT_LMP_FEATURE_SUPPORTED( AEEBT_LMP_FEAT_TRANS_SCO_DATA, pLMPFeatures ) )
  {
    STRTOWSTR( "TSD,", &pwBuf[ uLen ], uNumBytes - uLen*sizeof( AECHAR ) );
    uLen = WSTRLEN( pwBuf );
  }
  pwBuf[ uLen - 1 ] = (AECHAR)(unsigned char) ('\0'); // remove last comma
}

/* ==========================================================================
FUNCTION BTApp_FormatBTName
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatBTName( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       pwName )
{  
	AECHAR wBuf1[ 24 ];	//16 
	AECHAR wStr[3];	

#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_IC241A_MMX)|| defined (FEATURE_VERSION_KK5)
	MEMSET(pwStr, 0, WSTRLEN(pwStr)*sizeof(AECHAR));	
	
	ISHELL_LoadResString(pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_BT_NAME, wBuf1, sizeof( wBuf1 )); 
	WSTRLCAT( pwStr, wBuf1, (WSTRLEN(wBuf1) * sizeof(AECHAR)));  
	
	MEMSET(wBuf1, 0, WSTRLEN(wBuf1)*sizeof(AECHAR));	
	wStr[0] = (AECHAR)(unsigned char) (':');
	wStr[1] = (AECHAR)(unsigned char) ('\n');
	wStr[2] = (AECHAR)(unsigned char) ('\0');
	
	WSTRLCAT( pwStr, wStr, (uLen* sizeof(AECHAR)));

	WSTRLCAT( pwStr, pwName, (uLen * sizeof(AECHAR)));  

	MEMSET(wBuf1, 0, WSTRLEN(wBuf1)*sizeof(AECHAR));
	wStr[0] = (AECHAR)(unsigned char) ('\n');
	wStr[1] = (AECHAR)(unsigned char) ('\0');
	
	WSTRLCAT( pwStr, wStr, (uLen* sizeof(AECHAR)));	
#elif defined FEATURE_VERSION_W317A
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_DEVICE_NAME, wBuf1, sizeof( wBuf1 ) );
	WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pwName );
	wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
	wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
	WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
    
#else
	ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_BT_NAME, wBuf1, sizeof( wBuf1 ) );
	WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pwName );
	wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
	wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
	WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
#endif

	return WSTRLEN( pwStr );
}

#ifdef FEATURE_BT_2_1
/* ==========================================================================
FUNCTION BTApp_FormatBTShortName
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatBTShortName( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       pwName,
  boolean       bNameSame )
{
  AECHAR wBuf1[ 32 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_BT_SHORT_NAME, 
                        wBuf1, sizeof( wBuf1 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pwName );
  if ( bNameSame != TRUE )
  {
    wBuf1[ 0 ] = (AECHAR)(unsigned char) ('.');
    wBuf1[ 1 ] = (AECHAR)(unsigned char) ('.');
    wBuf1[ 2 ] = (AECHAR)(unsigned char) ('\n');
    wBuf1[ 3 ] = (AECHAR)(unsigned char) ('\0');
    WSTRLCAT( pwStr, wBuf1, uLen );
  }
  else 
  {
    wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
    wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
    WSTRLCAT( pwStr, wBuf1, uLen);
  }
  return WSTRLEN( pwStr );
}

/*==========================================================================
FUNCTION BTApp_FormatSSPCapable
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatSSPCapable( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  boolean       bSSPCapable )
{
  AECHAR wBuf1[ 16 ];
  AECHAR wBuf2[ 8 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_SSP_CAPABLE, wBuf1, 
                        sizeof( wBuf1 ) );
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        bSSPCapable ? IDS_BT_YES : IDS_BT_NO, 
                        wBuf2, sizeof( wBuf2 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);
  return WSTRLEN( pwStr );
}

/*==========================================================================
FUNCTION BTApp_FormatLinkKey
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatLinkKey( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  uint8         linkKeyType )
{
  AECHAR wBuf1[ 32 ];
  AECHAR* wBuf2;
  char*   pcStr;

  static struct {
    uint8 LinkKeyType;
    char* LKStatus;
  } LinkKeyLookupTable[] = { 
    { AEEBT_LINK_KEY_STATUS_NONE, "NONE" },    
    { AEEBT_LINK_KEY_STATUS_GENERAL, "UNAUTH,GEN" },   
    { AEEBT_LINK_KEY_STATUS_AUTH_GENERAL, "AUTH,GEN" },   
    { AEEBT_LINK_KEY_STATUS_DEDICATED, "UNAUTH,DED" },
    { AEEBT_LINK_KEY_STATUS_AUTH_DEDICATED, "AUTH,DED" },
    { AEEBT_LINK_KEY_STATUS_DEBUG, "DEBUG"  },
    { AEEBT_LINK_KEY_STATUS_COMBINATION, "COMB" },  
    { AEEBT_LINK_KEY_STATUS_UNBONDED, "UNAUTH,NON" },  
    { AEEBT_LINK_KEY_STATUS_AUTH_UNBONDED, "AUTH,NON" },  
    { AEEBT_LINK_KEY_STATUS_UNIT, "UNIT" }
  };
  
  pcStr = MALLOC(SHORT_TEXT_BUF_LEN* sizeof(char));
  wBuf2 = MALLOC(SHORT_TEXT_BUF_LEN* sizeof(AECHAR));

  SNPRINTF ( pcStr, SHORT_TEXT_BUF_LEN* sizeof(char), 
            "{%s}", LinkKeyLookupTable[linkKeyType].LKStatus );
  
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_LINK_KEY, 
                        wBuf1, sizeof( wBuf1 ) );
  STRTOWSTR( pcStr, wBuf2, (SHORT_TEXT_BUF_LEN* sizeof(AECHAR)));
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);

  FREEIF (pcStr);
  FREEIF (wBuf2);

  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatLowPowerMode
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatLowPowerMode( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AEEBTLowPowerControl pwrMode )
{
  AECHAR wBuf1[ 128 ];
  AECHAR wBuf2[ 32 ];
  uint16 msgID;

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_LOW_POWER_MODE, wBuf1, 
                        sizeof( wBuf1 ) );
  switch ( pwrMode )
  {
    case AEEBT_LPC_ACTIVE:
      msgID = IDS_LPM_ACTIVE;
      break;
    case AEEBT_LPC_SNIFF:
      msgID = IDS_LMP_SNIFF;
      break;
    case AEEBT_LPC_SNIFF_LOCK:
      msgID = IDS_LPM_SNIFF_LCK;
      break;
    default:
      msgID = IDS_NOT_AVAIL;
      break;
  }

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, msgID, 
                        wBuf2, sizeof(wBuf2) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatMaxInterval
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatMaxInterval( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       wMaxInterval)
{
  AECHAR wBuf1[ 128 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_MAX_INTERVAL, wBuf1, 
                        sizeof( wBuf1 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wMaxInterval );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);

  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatIdleTieout
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatIdleTieout( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       wIdleTimeout)
{
  AECHAR wBuf1[ 128 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_IDLE_TIMEOUT, 
                        wBuf1, sizeof( wBuf1 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wIdleTimeout );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);

  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatMaxLatency
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatMaxLatency( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       wMaxLatency)
{
  AECHAR wBuf1[ 128 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_MAX_LATENCY, 
                        wBuf1, sizeof( wBuf1 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wMaxLatency );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);

  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatSniffParams
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatSniffParams( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  uint16        msgID,
  boolean       bflag
 )
{
  AECHAR wBuf1[ 128 ];
  AECHAR wBuf2[ 64 ];

  switch( msgID )
  {
    case IDS_STK_APP_PARAM:
    {
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                            IDS_LABEL_STICK_TO_APP, wBuf1, 
                            sizeof( wBuf1 ) );
      break;
    }
    case IDS_SNIF_INIT_ORIENTED:
    {
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                            IDS_LABEL_SNIFF_INIT_ORIENTED, wBuf1, 
                            sizeof( wBuf1 ) );
      break;
    }
  }
  if ( bflag == TRUE )
  {
    STRTOWSTR("True", wBuf2,sizeof (wBuf1) );
    WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  }
  else
  {
    STRTOWSTR("False", wBuf2,sizeof (wBuf1) );
    WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  }
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT(pwStr, wBuf1, uLen);
  return WSTRLEN( pwStr );
}


uint16 BTApp_FormatOOBDataStr( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  boolean       bOOBDataPrsent
  )
{
  AECHAR wBuf1[ 128 ];
  AECHAR wBuf2[ 16 ];

  if ( bOOBDataPrsent )
  {
   ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                         IDS_MSG_OOB_DATA_PRESENT, 
                         wBuf1, sizeof( wBuf1 ) );
  }
  else
  {
   ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE,
                         IDS_MSG_OOB_DATA_NOT_PRESENT, 
                         wBuf1, sizeof( wBuf1 ) );
  }
  
  wBuf2[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf2[ 1 ] = (AECHAR)(unsigned char) ('\0');

  WSTRLCAT( wBuf1, wBuf2 , sizeof (wBuf1) );
  WSTRLCAT (pwStr , wBuf1 ,( uLen * sizeof(AECHAR) ) );

  return WSTRLEN( pwStr );
}

#endif

/* ==========================================================================
FUNCTION BTApp_FormatBDAddress
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatBDAddress( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AEEBTBDAddr*  pAddr )
{
  AECHAR wBuf1[ 16 ];
  AECHAR wBuf2[ 16 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_BD_ADDR, 
                        wBuf1, sizeof( wBuf1 ) );
  BTApp_BDAddr2Wstr( wBuf2, pAddr );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf2[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf2[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf2,  uLen * sizeof(AECHAR) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatSecurity
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatSecurity( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AEEBTSecurity sec )
{
  AECHAR wBuf1[ 16 ];
  AECHAR wBuf2[ 32 ];
  uint16 msgID;

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_SECURITY, 
                        wBuf1, sizeof( wBuf1 ) );
  switch ( sec )
  {
    case AEEBT_SEC_NONE:
      msgID = IDS_SECURITY_NONE;
      break;
    case AEEBT_SEC_AUTHENTICATE:
      msgID = IDS_SECURITY_AUTH;
      break;
    case AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT:
      msgID = IDS_SECURITY_AUTH_ENC;
      break;
    default:
      msgID = IDS_NOT_AVAIL;
      break;
  }

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, msgID, 
                        wBuf2, sizeof(wBuf2) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatBondable
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatBondable( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen,
  boolean bBondable )
{
  AECHAR wBuf1[ 16 ];
  AECHAR wBuf2[ 8 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_BONDABLE, 
                        wBuf1, sizeof( wBuf1 ) );
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        bBondable ? IDS_BT_YES : IDS_BT_NO, 
                        wBuf2, sizeof( wBuf2 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatDiscoverable
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatDiscoverable( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen,
  boolean bDiscoverable )
{
  AECHAR wBuf1[ 32 ];
  AECHAR wBuf2[ 8 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_DISCOVERABLE, wBuf1, sizeof( wBuf1 ) );
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        bDiscoverable ? IDS_BT_YES : IDS_BT_NO, 
                        wBuf2, sizeof( wBuf2 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatSvcCls
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatSvcCls( 
  CBTApp*           pMe, 
  AECHAR*           pwStr, 
  uint16            uLen,
  AEEBTServiceClass svcCls )
{
  AECHAR wBuf1[ 32 ];
  AECHAR wBuf2[ 16 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_SVC_CLS, 
                        wBuf1, sizeof( wBuf1 ) );
  BTApp_SvcCls2Wstr( wBuf2, sizeof( wBuf2 ), svcCls );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatLMPFeatures
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatLMPFeatures( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen,
  uint8*  pLMPFeatures )
{
  AECHAR wBuf1[ 32 ];
  AECHAR wBuf2[ 256 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_LMP_FEAT, 
                        wBuf1, sizeof( wBuf1 ) );
  if ( (pLMPFeatures[0] == 0) && 
       (pLMPFeatures[1] == 0) && 
       (pLMPFeatures[2] == 0) )
  {
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_NOT_AVAIL, 
                          wBuf2, sizeof( wBuf2 ) );
  }
  else
  {
    BTApp_LMPFeat2Wstr( wBuf2, sizeof(wBuf2), pLMPFeatures );
  }
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatLMPVersion
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatLMPVersion( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen )
{
  AECHAR wBuf1[ 32 ];
  AECHAR wBuf2[ 32 ];

  switch ( pMe->mRM.myInfo.uLMPVersion )
  {
    /* Following strings are taken from BT SIG spec.
       https://www.bluetooth.org/assigned-numbers/link_manager_protocol.php
    */
    case 0:
      STRTOWSTR( "Bluetooth LMP 1.0", wBuf2, sizeof( wBuf2 ) );
      break;
    case 1:
      STRTOWSTR( "Bluetooth LMP 1.1", wBuf2, sizeof( wBuf2 ) );
      break;
    case 2:
      STRTOWSTR( "Bluetooth LMP 1.2", wBuf2, sizeof( wBuf2 ) );
      break;
    case 3:
      STRTOWSTR( "Bluetooth LMP 2.0", wBuf2, sizeof( wBuf2 ) );
      break;
    default: 
      /* At this time, this version number is unassigned.
         Just ouput the integer id.
      */
      STRTOWSTR( "%u", wBuf1, sizeof( wBuf1 ) );
      WSPRINTF( wBuf2, sizeof( wBuf2 ), wBuf1, pMe->mRM.myInfo.uLMPVersion );
      break;
  }

  STRTOWSTR( "LMP Version: %s", wBuf1, sizeof( wBuf1 ) );

  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatLMPSubVersion
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatLMPSubVersion( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen )
{
  AECHAR wBuf1[ 32 ];

  STRTOWSTR( "LMP Subversion: %u", wBuf1, sizeof( wBuf1 ) );

  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pMe->mRM.myInfo.uLMPSubVersion );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatPortStatus
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatPortStatus(
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen )
{
  AECHAR wBuf1[ 64 ];
  AECHAR wBuf2[ 64 ];
  uint16 strID;

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_PORT_STATUS, 
                        wBuf1, sizeof( wBuf1 ) );

  switch ( pMe->mSPP.status.state )
  {
    case AEEBT_SPP_ST_OPENED:
      strID = pMe->mSPP.bClient ? IDS_CONNECTING : IDS_LISTENING;
      break;
    case AEEBT_SPP_ST_CONNECTED:
      strID = IDS_CONNECTED;
      break;
    case AEEBT_SPP_ST_DISCONNECTED:
      strID = IDS_DISCONNECTED;
      break;
    default:
      strID = IDS_UNKNOWN;
      break;
  }
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, strID, 
                        wBuf2, sizeof(wBuf2) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatBytesSent
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatBytesSent(
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen )
{
  AECHAR wBuf1[ 64 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_BYTES_SENT, 
                        wBuf1, sizeof( wBuf1 ) );

  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pMe->mSPP.uTotalBytesSent );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatBytesRcvd
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatBytesRcvd(
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen )
{
  AECHAR wBuf1[ 64 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_BYTES_RCVD, 
                        wBuf1, sizeof( wBuf1 ) );

  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pMe->mSPP.uTotalBytesRcvd );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatDevRole
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatDevRole( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen,
  boolean bMaster )
{
  AECHAR wBuf1[ 25 ];
  AECHAR wBuf2[ 8 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_LABEL_DEV_ROLE, 
                        wBuf1, sizeof( wBuf1 ) );
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        bMaster ? IDS_MASTER : IDS_SLAVE, 
                        wBuf2, sizeof( wBuf2 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, wBuf2 );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, ( uLen * sizeof(AECHAR) ) );
  return WSTRLEN( pwStr );
}

#ifdef FEATURE_BT_2_1
/*==========================================================================
FUNCTION BTApp_FormatManuData
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatManuData( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       pwName )
{
  AECHAR wBuf1[ 30 ];

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_BT_MANU_DATA, wBuf1, 
                        sizeof( wBuf1 ) );
  WSPRINTF( pwStr, uLen*sizeof( AECHAR ), wBuf1, pwName );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen );
  return WSTRLEN( pwStr );
}

/* ==========================================================================
FUNCTION BTApp_FormatUUID
DESCRIPTION
  UUID16:  {111F,HFAG},1234, <- 5 chars/UUID w/out name, up to 13 with name
  UUID32:  11223344,12122323, <- 9 chars/UUID
  UUID128: {1122,3344,5566,7788,9900,AABB,CCDD,EEFF}, <- 42 chars/UUID
============================================================================= */
uint16 BTApp_FormatUUID( 
  CBTApp* pMe, 
  AECHAR* pwStr, 
  uint16  uLen,
  AEEBTDeviceInfo*  pDev )
{
  AECHAR  wBuf[ 30 ];
  AECHAR* pwTemp = pwStr; 
  char*   pszTemp;
  uint16* pUUIDBuf;
  uint8   i, j, k, uBufSize, uMaxBufSize = 0, uTotalChars = 0;
  int     result;
  int     nlenTemp = 0;

  static uint8 uWordsPerUUID[ AEEBT_NUM_UUID_TYPES ] = {1,2,8};
  static uint8 uCharsPerUUID[ AEEBT_NUM_UUID_TYPES ] = {13,9,42};
  static uint16 uMsgID[ AEEBT_NUM_UUID_TYPES ] = 
    {IDS_BT_LABEL_UUID16,IDS_BT_LABEL_UUID32,IDS_BT_LABEL_UUID128};
  static struct {
    uint16 UUID;
    char*  pszName;
  } NameLookupTable[] = { 
    { 0x0008, "OBEXs" },  
    { 0x1101, "SPP" },    // serial port profile
    { 0x1102, "LAN" },    // LAN access PPP
    { 0x1103, "DUN" },    // Dial-up networking
    { 0x1105, "OPP" },
    { 0x1106, "FTP" },
    { 0x1108, "HS"  },
    { 0x110A, "AVSrc" },  // audio source
    { 0x110B, "AVSnk" },  // audio sink
    { 0x110C, "AvRCT" },  // AV remote control target
    { 0x110D, "A2DP" },
    { 0x110E, "AVRCP" },
    { 0x110F, "VidCf" },  // video conferencing
    { 0x1110, "IntCm" },  // intercom
    { 0x1111, "Fax" },
    { 0x1112, "HSAG"  },
    { 0x1113, "WAP"  },
    { 0x111B, "BIPs"  },
    { 0x111E, "HF"  },
    { 0x111F, "HFAG"  },
    { 0x1120, "BPPs"  },
    { 0x1122, "BIP"  },
    { 0x1124, "HID"  },
    { 0x112D, "SAP"  },
    { 0x112E, "PBAPc"  },
    { 0x112F, "PBAPs"  },
    { 0,      NULL }
  };
  if ( (uLen < 15) ||
       ((pDev->EIRData.uFlags & AEEBT_EIR_DATA_RCVD_B) == 0) )
  {
    // either space in destination string is too small, or
    // no EIR data is received
    return uLen; 
  }

  MSG_MED("FormatUUID: numUUIDs=%d,%d,%d", 
          pDev->EIRData.uNumUUIDs[0],
          pDev->EIRData.uNumUUIDs[1],
          pDev->EIRData.uNumUUIDs[2]);

  for (i = 0; i < AEEBT_NUM_UUID_TYPES; i++)
  {
    /* Calculate the size to be used */
    uTotalChars += (uCharsPerUUID[ i ] * pDev->EIRData.uNumUUIDs[ i ]);
    uBufSize = (uWordsPerUUID[ i ] * pDev->EIRData.uNumUUIDs[ i ]);
    uMaxBufSize = MAX( uMaxBufSize, uBufSize );
  }

  if ( uTotalChars == 0 )
  {
    return uLen; // nothing to do
  }

  uMaxBufSize <<= 1; // convert to number of bytes
  MSG_MED("FormatUUID: uTotalChars=%d max=%d", uTotalChars, uMaxBufSize, 0);

  pszTemp  = MALLOC( uTotalChars ) ;        // temp string in char
  pUUIDBuf = MALLOC( uMaxBufSize );         // for retreiving UUIDs

  if ( (pszTemp == NULL) || (pUUIDBuf == NULL) )
  {
    MSG_ERROR( "FormatUUID: Failed allocating mem", 0, 0, 0 );
    FREEIF (pUUIDBuf);
    FREEIF (pszTemp);
    return uLen;
  }

  /* Get UUIDs and format the output */
  for ( i = 0; i < AEEBT_NUM_UUID_TYPES; i++ )
  {
    if (pDev->EIRData.uNumUUIDs[ i ] == 0)
    {
      continue; // no UUID of this type, skip it
    }

    pszTemp[ 0 ] = NULL;
    uBufSize = uMaxBufSize;
    result = IBTEXTRM_GetEIRUUIDList( pMe->mRM.po, &(pDev->bdAddr), 
                                      i, 0, &uBufSize, pUUIDBuf );
    MSG_MED( "FormatUUID: GetEIRUUIDList [%d] r=%d size=%d", i, result, 
             uBufSize);
    if( result == SUCCESS )
    {
      switch ( i )
      {
        case AEEBT_RM_UUID16:
        {
          uBufSize >>= 1; // convert to number of words
          for ( j=0; j<uBufSize; j++ )
          {
            uint16 curUUID = *(((uint16*)pUUIDBuf)+j);
            for ( k=0; (NameLookupTable[ k ].pszName != NULL) &&
                       (NameLookupTable[ k ].UUID != curUUID); k++ );

            nlenTemp = STRLEN(pszTemp);
            if ( NameLookupTable[ k ].pszName == NULL )
            {
              SNPRINTF ( &pszTemp[ nlenTemp ], 
                         (uTotalChars - nlenTemp),
                         "%04X,", curUUID );
            }
            else
            {
              SNPRINTF ( &pszTemp[ nlenTemp ], 
                         (uTotalChars - nlenTemp),
                         "{%04X,%s},",
                         curUUID, 
                         NameLookupTable[ k ].pszName );
            }
          }
          // replace the last comma with '\n'
          pszTemp[ STRLEN(pszTemp) - 1] = '\n';
          break;
        }
        case AEEBT_RM_UUID32:
        {
          for ( j=0; j<uBufSize; j+=4 )
          {
            SNPRINTF(&pszTemp[ STRLEN(pszTemp) ], 
                     (uTotalChars - STRLEN(pszTemp)),
                     "%08X,", 
                     *((uint32*)(((uint8*)pUUIDBuf)+j)) );
          }
          // replace the last comma with '\n'
          pszTemp[ STRLEN(pszTemp) - 1] = '\n';
          break;
        }
        case AEEBT_RM_UUID128:
        {
          uBufSize >>= 1; // convert to number of words
          STRLCAT(  pszTemp, "{", uTotalChars );
          for ( j=0; j<uBufSize; j++ )
          {
            uint16 curWord = *(((uint16*)pUUIDBuf)+j);
            if ( ((j % uWordsPerUUID[ i ]) == 0) && (j!=0) )
            {
              STRLCPY( &pszTemp[ STRLEN(pszTemp) - 1],
                       "},{", (uTotalChars - STRLEN(pszTemp)-1) );
            }
            SNPRINTF( &pszTemp[ STRLEN(pszTemp) ], 
                      (uTotalChars - STRLEN(pszTemp)),
                      "%04X,", 
                      AEE_htons(curWord) );
          }
          // replace the last comma with '}\n'
          STRLCPY( &pszTemp[ STRLEN(pszTemp) - 1 ],
                   "}\n",
                   (uTotalChars - STRLEN(pszTemp)-1));
          break;
        }
      } // switch ( i )


      // get the label
      ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, uMsgID[ i ], 
                            wBuf, sizeof( wBuf )  );
      j = WSTRLEN( wBuf );
      if ( (uLen < j) || (uLen-j) <= 5 )
      {
        break; // not even enough room for the label
      }
      else
      {
        if ( STRLEN( pszTemp ) >= (uLen-j) )
        {
          STRLCPY(&pszTemp[ uLen-j-5 ], "...\n", (uTotalChars - uLen-j-5) ); 
        }
        WSTRLCPY ( pwTemp, wBuf, uLen );
        STRTOWSTR( pszTemp, &pwTemp[ j ], ((uLen-j) * sizeof(AECHAR)) );

        j = WSTRLEN( pwTemp );
        pwTemp = pwTemp + j;
        uLen -= j;
      }
    } // if( result == SUCCESS)
  }

  FREEIF (pUUIDBuf);
  FREEIF (pszTemp);
  return WSTRLEN(pwStr);
}

/* ==========================================================================
FUNCTION BTApp_FormatPasskey
DESCRIPTION
============================================================================= */
uint16 BTApp_FormatPasskey( 
  CBTApp*       pMe, 
  AECHAR*       pwStr, 
  uint16        uLen,
  AECHAR*       pwPasskey,
  AECHAR*       pwRemotePasskey )
{
  AECHAR wBuf1[ 64 ];
  
  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_LOCAL_PSSKEY, wBuf1, 
                        sizeof( wBuf1 ) );
  WSPRINTF( pwStr, (LONG_TEXT_BUF_LEN - uLen)*sizeof( AECHAR ), wBuf1, 
            pwPasskey );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen);

  uLen += WSTRLEN(pwStr);

  ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, 
                        IDS_LABEL_REMOTE_PSSKEY, wBuf1, 
                        sizeof( wBuf1 ) );
  WSPRINTF( (pwStr+WSTRLEN(pwStr) ), 
            (LONG_TEXT_BUF_LEN - uLen)*sizeof( AECHAR ), 
            wBuf1, pwRemotePasskey );
  wBuf1[ 0 ] = (AECHAR)(unsigned char) ('\n');
  wBuf1[ 1 ] = (AECHAR)(unsigned char) ('\0');
  WSTRLCAT( pwStr, wBuf1, uLen );
  return WSTRLEN( pwStr );
}
#endif

/* ==========================================================================
FUNCTION BTApp_UpdateMenuItemImage
DESCRIPTION
============================================================================= */
void BTApp_UpdateMenuItemImage( 
  IDisplay* pDisplay, 
  IMenuCtl* pMenu, 
  uint16    selection,
  uint16    imageID
)
{
  CtlAddItem ai;

  ai.pszResImage = ai.pszResText = AEE_APPSBTAPP_RES_FILE;
  ai.pImage = NULL;
  ai.wImage = imageID;
  IMENUCTL_SetItem( pMenu, selection, MSIF_IMAGE, &ai );

  IMENUCTL_Redraw( pMenu );
  IDISPLAY_UpdateEx( pDisplay, FALSE );
}

/* ==========================================================================
FUNCTION BTApp_GetDevTypeImageID
DESCRIPTION
============================================================================= */
uint16 BTApp_GetDevTypeImageID( 
  CBTApp*           pMe, 
  AEEBTServiceClass svcCls,
  AEEBTBDAddr*      pBDAddr )
{
  uint16 imageID;

  switch ( svcCls )
  {
    case AEEBT_COD_SC_UNKNOWN:
      imageID = IDB_U;
      break;
    case AEEBT_COD_SC_LIMITED_DISCOVERABLE_MODE:
      imageID = IDB_L;
      break;
    case AEEBT_COD_SC_NETWORKING:
      if ( (pMe->mNA.bConnected != FALSE) && 
           (AEEBT_BD_ADDRS_EQUAL( &pMe->mNA.connectedBDAddr,
                                  pBDAddr ) != FALSE) )
      {
        imageID = IDB_N_CONN;
      }
      else
      {
        imageID = IDB_N;
      }
      break;
    case AEEBT_COD_SC_RENDERING:
      imageID = IDB_R;
      break;
    case AEEBT_COD_SC_CAPTURING:
      imageID = IDB_C;
      break;
    case AEEBT_COD_SC_OBJECT_TRANSFER:
      imageID = IDB_O;
      break;
    case AEEBT_COD_SC_AUDIO:
      if ( (pMe->mAG.bConnected != FALSE) &&
            (AEEBT_BD_ADDRS_EQUAL( &pMe->mAG.connectedBDAddr, 
                                   pBDAddr ) != FALSE) )
      {
        imageID = IDB_A_CONN;
      }
      else if (AEEBT_BD_ADDRS_EQUAL( &pMe->mAG.bdAddr, pBDAddr ) != FALSE)
      {
        imageID = IDB_A_SEL;
      }
      else
      {
        imageID = IDB_A;
      }
      break;
    case AEEBT_COD_SC_TELEPHONY:
      imageID = IDB_T;
      break;
    case AEEBT_COD_SC_INFORMATION:
      imageID = IDB_I;
      break;
    case AEEBT_COD_SC_POSITIONING:
      imageID = IDB_P;
      break;
    default:
      if ( ((pMe->mAG.bConnected != FALSE) &&
             (AEEBT_BD_ADDRS_EQUAL( &pMe->mAG.connectedBDAddr, 
                                    pBDAddr ) != FALSE)) || 
           ((pMe->mNA.bConnected != FALSE) &&
             (AEEBT_BD_ADDRS_EQUAL( &pMe->mNA.connectedBDAddr, 
                                    pBDAddr ) != FALSE)) )
      {
        imageID = IDB_M_CONN;
      }
      else if (AEEBT_BD_ADDRS_EQUAL( &pMe->mAG.bdAddr, pBDAddr ) != FALSE)
      {
        imageID = IDB_M_SEL;
      }
      else
      {
        imageID = IDB_M;
      }
      break;
  }

  return imageID;
}

/* ==========================================================================
FUNCTION BTApp_GetFullPathName
DESCRIPTION
============================================================================= */
void BTApp_GetFullPathName( char* pszName, uint16 nameBufSize,  
                            char* pszFolder, uint16 folderBufSize )
{
  char szBuf[ AEEBT_MAX_FILE_NAME * 2 ];

  STRLCPY( szBuf, pszName, sizeof( szBuf ) );
  STRLCPY( pszName, pszFolder, nameBufSize );
  STRLCAT( pszName, DIRECTORY_STR, nameBufSize);
  STRLCAT( pszName, szBuf, nameBufSize );
}

/* ==========================================================================
FUNCTION BTApp_NumKey2Selection
DESCRIPTION
============================================================================= */
uint16 BTApp_NumKey2Selection( IMenuCtl* pMenu, uint16 key )
{
  uint16 selection = 0;
  uint8  keyLimit = 0;

  int numItems = IMENUCTL_GetItemCount( pMenu );

  keyLimit = ( numItems < 9 ) ? numItems : 9;
  if ( (key >= AVK_1) && (key <= (AVK_0 + keyLimit)) )
  {
    MSG_MED( "NumKey2Selectio - %d key pressed", 
             key - AVK_0, 0, 0 );
    selection = IMENUCTL_GetItemID( pMenu, key - AVK_0 - 1 );
    IMENUCTL_SetSel( pMenu, selection );
  }
  return selection;
}

/* ==========================================================================
FUNCTION BTApp_ShowBusyIcon
DESCRIPTION
============================================================================= */
void BTApp_ShowBusyIcon( CBTApp* pMe )
{
  return;
	
  ShowBusyIcon( pMe->m_pShell, 
                pMe->m_pIDisplay, 
                &pMe->m_rect, 
                FALSE );
  pMe->bBusyIconUp = TRUE;
}

/* ==========================================================================
FUNCTION BTApp_MessageTimerExpired
DESCRIPTION
============================================================================= */
void BTApp_MessageTimerExpired( CBTApp* pMe )
{
  ISHELL_PostEventEx( pMe->m_pShell, EVTFLG_ASYNC, 
                      AEECLSID_BLUETOOTH_APP,
                      EVT_USER, EVT_CLR_MSG_SCR, 0L);
}

/* ==========================================================================
FUNCTION BTApp_ShowMessage
DESCRIPTION
============================================================================= */

void BTApp_ShowMessage( 
  CBTApp* pMe, 
  uint16  msgID, 
  AECHAR* wArg, 
  uint8   secondsUp // amount of time the message stays on screen, 0 = 4ever
)
{	
	boolean needstr = FALSE;

	/*
	//Add By zzg 2011_03_04
	if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
	{		
		MSG_FATAL("***zzg BTApp_ShowMessage ISHELL_ActiveApplet=%x***", ISHELL_ActiveApplet(pMe->m_pShell), 0, 0);
		ISHELL_StartApplet(pMe->m_pShell, AEECLSID_BLUETOOTH_APP);

		pMe->m_msg_id = msgID;	
		pMe->m_bNeedStr = needstr;

		pMe->m_app_flag = TRUE;
		pMe->m_user_wParam = msgID;
		return;			
	}
	//Add End
	*/

	//Add By zzg 2011_12_28
	if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
	{		
		return;			
	}
	//Add End	

	IDISPLAY_Backlight( pMe->m_pIDisplay, TRUE );

	MEMSET(pMe->wMsgBuf, 0, WSTRLEN(pMe->wMsgBuf)*sizeof(AECHAR));
	
	if (wArg == NULL)
	{
		needstr = FALSE;
	}
	else
	{
		WSTRCPY(pMe->wMsgBuf, wArg);			
		needstr = TRUE;
	}

	DBGPRINTF("***zzg BTApp_ShowMessage wMsgBuf=%s***", pMe->wMsgBuf);

	MSG_FATAL("***zzg BTApp_ShowMsg msgID=%d, WSTRLEN(wArg)=%d***", msgID, WSTRLEN(wArg), 0);
	
	MSG_FATAL("***zzg BTApp_ShowMsg msgID=%d, WSTRLEN(wMsgBuf)=%d***", msgID, WSTRLEN(pMe->wMsgBuf), 0);

	pMe->m_msg_id = msgID;	
	pMe->m_bNeedStr = needstr;

	
	//CLOSE_DIALOG(DLGRET_BT_MSGBOX)   
	pMe->m_eDlgRet = DLGRET_BT_MSGBOX; 
	(void) ISHELL_EndDialog(pMe->m_pShell); 
   

  /*
  //Add By zzg 2010_12_29
  if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_BLUETOOTH_APP)
  {		
		MSG_FATAL("***zzg BTApp_ShowMessage ActiveApplet=%x***",ISHELL_ActiveApplet(pMe->m_pShell), 0, 0);
		return;
  }
  //Add End

  CLEAR_SCREEN();

  // get the title
  //ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, IDS_BT_TITLE, wTempBuf, sizeof( wTempBuf ) );

  //Add By zzg 2010_11_01
  if(pMe->m_pIAnn != NULL)
  {
      IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
  }  
  {   
    	ISHELL_LoadResString(pMe->m_pShell,
                             AEE_APPSBTAPP_RES_FILE,                                
                             IDS_BT_TITLE,
                             wTempBuf,
                             sizeof( wTempBuf ));
  
      if(pMe->m_pIAnn != NULL)
    	{
    	    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn, wTempBuf);
    	}
  }
  //Add End

  if ( msgID == 0 )
  {
    // use the wArg as msg
    pText = wArg;
  }
  else
  {
    // get the text
    ISHELL_LoadResString( pMe->m_pShell, AEE_APPSBTAPP_RES_FILE, msgID, 
                          pMe->pText2, SHORT_TEXT_BUF_LEN*sizeof( AECHAR ) );
    if ( wArg != NULL )
    {
      // build the message
      WSPRINTF( pMe->pText1, LONG_TEXT_BUF_LEN*sizeof( AECHAR ), 
                pMe->pText2, wArg );
      pText = pMe->pText1;
    }
  }

  // set rect for info display area
  SETAEERECT ( &rc, pMe->m_rect.x, 
               pMe->m_rect.y, 
               pMe->m_rect.dx, 
               pMe->m_rect.dy);
  ISTATIC_SetRect( pMe->m_pStatic, &rc );

  ISTATIC_SetProperties(pMe->m_pStatic, ISTATIC_GetProperties( pMe->m_pStatic ) | ST_MIDDLETEXT );
  
  //Add By zzg 2010_11_01     
  ISTATIC_SetProperties(pMe->m_pStatic, ST_NOSCROLL|ST_GRAPHIC_BG);  
  ISTATIC_SetBackGround(pMe->m_pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); 	
  //Add End

  //ISTATIC_SetText( pMe->m_pStatic, wTempBuf, pText, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  //Modify By zzg 2010_11_01
  ISTATIC_SetText( pMe->m_pStatic, NULL, pText, AEE_FONT_BOLD, AEE_FONT_NORMAL );
  
  //ISTATIC_SizeToFit( pMe->m_pStatic, &rc );	//Del By zzg 2010_11_01
  
  ISTATIC_Redraw( pMe->m_pStatic );
  IDISPLAY_UpdateEx( pMe->m_pIDisplay, FALSE );

  pMe->uCurrMsgId = msgID;
  pMe->bBusyIconUp = FALSE;  

  if ( secondsUp > 0)
  {
    ISHELL_SetTimer( pMe->m_pShell, ONE_SECOND*2, 
                     (PFNNOTIFY) BTApp_MessageTimerExpired, pMe );
  }
  */
}



/* ==========================================================================
FUNCTION BTApp_ShowDevMsg
DESCRIPTION
============================================================================= */
void BTApp_ShowDevMsg(
  CBTApp*             pMe, 
  uint16              msgID, 
  const AEEBTBDAddr*  pAddr,
  uint8               secondsUp
)
{
	AEEBTDeviceInfo dev;
	dev.bdAddr = *pAddr;
    
    MSG_FATAL("***zzg BTApp_ShowDevMsg 111 msgID=%x***", msgID, 0, 0);
    
	//IDISPLAY_Backlight(pMe->m_pIDisplay, TRUE);

    MSG_FATAL("***zzg BTApp_ShowDevMsg 222 msgID=%x***", msgID, 0, 0);

	if ((IBTEXTRM_DeviceRead(pMe->mRM.po, &dev) != SUCCESS) || (WSTRLEN(dev.wName) == 0))
	{
		BTApp_BDAddr2Wstr(dev.wName, pAddr);
	}

    MSG_FATAL("***zzg BTApp_ShowDevMsg 333 msgID=%x***", msgID, 0, 0);

	//BTApp_ShowMessage( pMe, msgID, dev.wName, secondsUp );

	//Add By zzg 2011_01_13
	pMe->m_msg_id = msgID;		
	pMe->m_bNeedStr = TRUE;	

	MEMSET(pMe->wMsgBuf, 0, WSTRLEN(pMe->wMsgBuf)*sizeof(AECHAR));
	WSTRCPY(pMe->wMsgBuf, dev.wName);	

    MSG_FATAL("***zzg SearchResult bStartFromOtherApp=%x***", pMe->bStartFromOtherApp, 0, 0);
    
	//Add By zzg 2012_11_22
	if ((pMe->bStartFromOtherApp == TRUE) && (msgID == IDS_MSG_BONDED))
	{
		MSG_FATAL("***zzg SearchResult mOPP.bConnected=%x***", pMe->mOPP.bConnected, 0, 0);

		if (pMe->mOPP.bConnected == TRUE)
		{
			BTApp_OPPPushEx(pMe, pMe->m_pfilepath, AEEBT_OPP_UNKNOWN_TYPE);
		}		
		else
		{					
#ifdef FEATURE_BT_2_1
			//Client side service security settings
			IBTEXTRM_SetSecBySvcCls(pMe->mRM.po, AEEBT_SD_SERVICE_CLASS_OBEX_OBJECT_PUSH, pMe->mOPP.srvSecType,FALSE,FALSE);
#endif 
			BTApp_OPPConnect( pMe, &pMe->mRM.device[ pMe->mRM.uCurDevIdx ].bdAddr);  			
		}		
		return;
	}
	//Add End

	//CLOSE_DIALOG(DLGRET_BT_MSGBOX)   
	pMe->m_eDlgRet = DLGRET_BT_MSGBOX; 
	(void) ISHELL_EndDialog(pMe->m_pShell); 	
	//Add End  
 
}

#ifdef FEATURE_BT_EXTPF_AV
/* ==========================================================================
FUNCTION BTApp_UseA2DPDevice
DESCRIPTION
============================================================================= */
void BTApp_UseA2DPDevice( CBTApp* pMe )
{
	MSG_FATAL("***zzg BTApp_UseA2DPDevice bAudioSelected=%x***", pMe->mA2DP.bAudioSelected, 0, 0);
  if ( pMe->mA2DP.bAudioSelected == FALSE )
  {
    MSG_MED( "UseA2DPDevice", 0, 0, 0 );
    pMe->mA2DP.bAudioSelected = TRUE;

#ifdef FEATURE_AVS_BT_SCO_REWORK
    BTApp_ReleaseBTDevice( pMe, FALSE );
#endif /* FEATURE_AVS_BT_SCO_REWORK */

#ifdef UI_UISND_NO_DIRECT_STATUS_CALLS
    uisnd_set_device_status( SND_DEVICE_BT_A2DP_HEADSET, UISND_DEV_ENABLED );
#else
    uisnd_force_device_autoselect();
#endif /* UI_UISND_NO_DIRECT_STATUS_CALLS */

    ISHELL_SendURL(pMe->m_pShell, "soundman:unmute");
    if ( ISHELL_SendURL( pMe->m_pShell, 
                         "soundman:select?dev=BTStereoHeadset" ) == FALSE )
    {
      uisnd_set_device_auto( NULL, NULL );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_ReleaseA2DPDevice
DESCRIPTION
============================================================================= */
void BTApp_ReleaseA2DPDevice( CBTApp* pMe )
{
	MSG_FATAL("***zzg BTApp_ReleaseA2DPDevice bAudioSelected=%x***", pMe->mA2DP.bAudioSelected, 0, 0);
  if ( pMe->mA2DP.bAudioSelected != FALSE )
  {
    MSG_MED( "ReleaseA2DPDevice", 0, 0, 0 );
    pMe->mA2DP.bAudioSelected = FALSE;

#ifdef UI_UISND_NO_DIRECT_STATUS_CALLS
    uisnd_set_device_status( SND_DEVICE_BT_A2DP_HEADSET, UISND_DEV_UNATTACHED );
#else
    uisnd_force_device_autoselect();
#endif /* UI_UISND_NO_DIRECT_STATUS_CALLS */

    if ( ISHELL_SendURL( pMe->m_pShell, 
                         "soundman:deselect?dev=BTStereoHeadset") == FALSE )
    {
      uisnd_set_device_auto( NULL, NULL );
    }
  }
}
#endif /* FEATURE_BT_EXTPF_AV */

/* ==========================================================================
FUNCTION BTApp_BTAudioSelected
DESCRIPTION
============================================================================= */
boolean BTApp_BTAudioSelected( CBTApp* pMe )
{
  boolean bSelected = FALSE;
#ifdef FEATURE_UI_CORE
  if ( pMe->mAG.bAudioSelected != FALSE )
#else
  if ( ui.bt_is_voice_path != FALSE )
#endif // FEATURE_UI_CORE
  {
    bSelected = TRUE;
  }
  return bSelected;
}

/* ==========================================================================
FUNCTION BTApp_UseBTDevice
DESCRIPTION
============================================================================= */
void BTApp_UseBTDevice( CBTApp* pMe, boolean bForceUnmute )
{
#if defined (FEATURE_BT)
  boolean a2s = 0;
#ifdef FEATURE_BT_EXTPF_AV
  a2s = pMe->mA2DP.bAudioSelected;
#endif

  MSG_FATAL("***zzg BTApp_UseBTDevice bAudioSelected=%x***", pMe->mAG.bAudioSelected, 0, 0);

  if ( pMe->mAG.bAudioSelected == FALSE )
  {
    MSG_MED( "UseBTDevice - a2s=%d", a2s, 0, 0 );
    pMe->mAG.bAudioSelected = TRUE;

#ifdef FEATURE_AVS_BT_SCO_REWORK
    BTApp_ReleaseA2DPDevice( pMe );
#endif /* FEATURE_AVS_BT_SCO_REWORK */

#ifdef UI_UISND_NO_DIRECT_STATUS_CALLS
#ifdef FEATURE_BT_AG
    uisnd_set_device_status( SND_DEVICE_BT_HEADSET, UISND_DEV_ENABLED );
#endif
#else	
    uisnd_force_device_autoselect();
#endif /* UI_UISND_NO_DIRECT_STATUS_CALLS */
	
    if ( ISHELL_SendURL( pMe->m_pShell, 
                         "soundman:select?dev=BTHeadset" ) == FALSE )
    {      
      uisnd_set_device_auto( NULL, NULL );
    }
  }

#ifndef FEATURE_AVS_BT_SCO_REWORK
  if ( bForceUnmute != FALSE )
  {
    MSG_MED( "UseBTDevice - forceUnmute", 0, 0, 0 );
    if ( ISHELL_SendURL( pMe->m_pShell, "soundman:unmute" ) == FALSE )
    {
      uisnd_notify_data_s_type sndInfo;
      uisnd_get_device( &sndInfo );
      MSG_LOW( "UseBTDevice - dev=%d sMute=%d mMute=%d", sndInfo.out_device, 
               sndInfo.speaker_mute, sndInfo.microphone_mute );
      if ( (sndInfo.microphone_mute == SND_MUTE_MUTED) || 
           (sndInfo.speaker_mute == SND_MUTE_MUTED) )
      {
        uisnd_force_device_mute( SND_DEVICE_CURRENT, UISND_MUTE_UNMUTED, 
                                 UISND_MUTE_UNMUTED, NULL, NULL );
      }
    }
  }
#endif /* !FEATURE_AVS_BT_SCO_REWORK */
#endif /* defined (FEATURE_BT) */  
}

/* ==========================================================================
FUNCTION BTApp_ReleaseBTDevice
DESCRIPTION
============================================================================= */
void BTApp_ReleaseBTDevice( CBTApp* pMe, boolean bForceUnmute )
{
  MSG_FATAL("***zzg BTApp_ReleaseBTDevice bForceUnmute=%x***", bForceUnmute, 0, 0);
  
  if ( pMe->mAG.bAudioSelected != FALSE )
  {
    MSG_MED( "ReleaseBTDevice - fum=%d", bForceUnmute, 0, 0 );
    pMe->mAG.bAudioSelected = FALSE;

#ifdef UI_UISND_NO_DIRECT_STATUS_CALLS
#ifdef FEATURE_BT_AG
    uisnd_set_device_status( SND_DEVICE_BT_HEADSET, UISND_DEV_UNATTACHED );
#endif
#else
    uisnd_force_device_autoselect();
#endif /* UI_UISND_NO_DIRECT_STATUS_CALLS */

    if ( ISHELL_SendURL( pMe->m_pShell, 
                         "soundman:deselect?dev=BTHeadset" ) == FALSE )
    {
      uisnd_set_device_auto( NULL, NULL );
    }

#ifndef FEATURE_AVS_BT_SCO_REWORK
    if ( bForceUnmute != FALSE )
    {
      if ( ISHELL_SendURL( pMe->m_pShell, "soundman:unmute" ) == FALSE )
      {
        uisnd_notify_data_s_type sndInfo;
        uisnd_get_device( &sndInfo );
        MSG_LOW( "ReleaseBTDevice - dev=%d sMute=%d mMute=%d",
                 sndInfo.out_device,
                 sndInfo.speaker_mute, sndInfo.microphone_mute );
        if ( sndInfo.microphone_mute == SND_MUTE_MUTED )
        {
          uisnd_force_device_mute(
            SND_DEVICE_CURRENT, (uisnd_mute_control_type)sndInfo.speaker_mute, 
                                 UISND_MUTE_UNMUTED, NULL, NULL );
        }
      }
    }
#endif /* !FEATURE_AVS_BT_SCO_REWORK */
  }
}

/* ==========================================================================
FUNCTION BTApp_CheckVoiceCallState
DESCRIPTION
============================================================================= */
boolean BTApp_CheckVoiceCallState( CBTApp* pMe, uint8 uState )
{
  AEETCalls*           pCallsDesc = NULL;
  uint8                uSizeOfCallsDesc = 0;
  AEETCallInfo         callInfo;
  uint8                index = 0;
  boolean              bCallState = FALSE;

  //MSG_FATAL("***zzg BTApp_CheckVoiceCallState uState=%x**", uState, 0, 0);

  uSizeOfCallsDesc = sizeof(AEETCalls) + (sizeof(AEECallDesc)*(BTAPP_MAX_NUM_CALLS - 1));
  pCallsDesc = (AEETCalls*)MALLOC( uSizeOfCallsDesc );

  //MSG_FATAL("***zzg BTApp_CheckVoiceCallState pCallsDesc=%x**", pCallsDesc, 0, 0);
  
  if ( pCallsDesc == NULL )
  {
    MSG_ERROR( "IS_VOICECALL_INCOMING - Malloc failed", 0, 0, 0 );
    return FALSE;  
  }  
  
  if ( ITELEPHONE_GetCalls( pMe->pIPhone, pCallsDesc, 
                            uSizeOfCallsDesc ) == SUCCESS )
  {
  	//MSG_FATAL("***zzg BTApp_CheckVoiceCallState dwCount=%x**", pCallsDesc->dwCount, 0, 0);
	
    /* process information for each call */
    for ( index = 0; index < pCallsDesc->dwCount; index++ )
    {
      /* get call info */
      if ( ITELEPHONE_GetCallInfo( pMe->pIPhone, pCallsDesc->calls[index],
                                       &callInfo, sizeof(callInfo) ) != SUCCESS )
      {
        MSG_ERROR( "IS_VOICECALL_INCOMING - Can't obtain callinfo callID:%d",
                   pCallsDesc->calls[index], 0, 0 );
        bCallState = FALSE;
        break;
      }

	  /*MSG_FATAL("***zzg BTApp_CheckVoiceCallState index=%x, call_type=%x, call_state=%x**", 
	  			pCallsDesc->dwCount, callInfo.call_type, callInfo.call_state);*/
	  
      if ( uState == BTAPP_VOICECALL_INCOMING )
      {
        if ( (callInfo.call_type == AEET_CALL_TYPE_VOICE) && 
             (callInfo.call_state == AEET_CALL_STATE_INCOM) )
        {
          bCallState = TRUE;
          break;
        }
      }
      else if ( uState == BTAPP_VOICECALL_PRESENT )
      {
        if ( callInfo.call_type == AEET_CALL_TYPE_VOICE )
        {
          bCallState = TRUE;
          break;
        }
      }
      else if ( uState == BTAPP_VOICECALL_CONNECTED )
      {
        if ( (callInfo.call_type == AEET_CALL_TYPE_VOICE) && 
             ((callInfo.call_state == AEET_CALL_STATE_CONV) ||
              (callInfo.call_state == AEET_CALL_STATE_ONHOLD) ||
              (callInfo.call_state == AEET_CALL_STATE_DORMANT)) )
        {
          bCallState = TRUE;
          break;
        }
      }
    }    
  }
  FREEIF( pCallsDesc );
  
  //MSG_FATAL("***zzg BTApp_CheckVoiceCallState bCallState=%x**", bCallState, 0, 0);
  return bCallState;
}

/* ==========================================================================
FUNCTION BTApp_CallPresent
DESCRIPTION
============================================================================= */
BTAppCallType BTApp_CallPresent( CBTApp* pMe )
{
  BTAppCallType callPresent = BT_APP_CALL_NONE;

  if ( BTAPP_IS_VOICECALL_PRESENT(pMe ) )
  {
    callPresent = BT_APP_CALL_VOICE;
  }
#ifdef FEATURE_BT_VT
  else if ( AEEVP_IS_VIDEOCALL_PRESENT( pMe->mAG.pIVP ) )
  {
    callPresent = BT_APP_CALL_VT;
  }
#endif /* FEATURE_BT_VT */

  return callPresent;
}

/* ==========================================================================
FUNCTION BTApp_CallIncoming
DESCRIPTION
============================================================================= */
BTAppCallType BTApp_CallIncoming( CBTApp* pMe )
{
  BTAppCallType callIncoming = BT_APP_CALL_NONE;
  if ( BTAPP_IS_VOICECALL_INCOMING( pMe ) )
  {
    callIncoming = BT_APP_CALL_VOICE;
  }
#ifdef FEATURE_BT_VT
  else if ( AEEVP_IS_VIDEOCALL_INCOMING( pMe->mAG.pIVP ) )
  {
    callIncoming = BT_APP_CALL_VT;
  }
#endif /* FEATURE_BT_VT */

  //MSG_FATAL("***zzg BTApp_CallIncoming callIncoming=%x***", callIncoming, 0, 0);

  return callIncoming;
}

/* ==========================================================================
FUNCTION BTApp_CallConnected
DESCRIPTION
============================================================================= */
BTAppCallType BTApp_CallConnected( CBTApp* pMe )
{
  BTAppCallType callConnected = BT_APP_CALL_NONE;

  if ( BTAPP_IS_VOICECALL_CONNECTED( pMe ) )
  {
    callConnected = BT_APP_CALL_VOICE;
  }
#ifdef FEATURE_BT_VT
  else if ( AEEVP_IS_VIDEOCALL_CONNECTED( pMe->mAG.pIVP ) )
  {
    callConnected = BT_APP_CALL_VT;
  }
#endif /* FEATURE_BT_VT */

  MSG_FATAL("***zzg BTApp_CallConnected callConnected=%x***", callConnected, 0, 0);

  return callConnected;
}

/* ==========================================================================
FUNCTION BTApp_SetBondable
DESCRIPTION
============================================================================= */
void BTApp_SetBondable( CBTApp* pMe )
{

  MSG_FATAL("***zzg BTApp_SetBondable bBondable=%d***", pMe->mRM.bBondable, 0, 0);
  
  if ( pMe->mRM.bBondable == FALSE )
  {
    if ( IBTEXTRM_SetBondable( pMe->mRM.po, TRUE ) != SUCCESS )
    {
      BTApp_ShowMessage( pMe, IDS_MSG_BONDABLE_NOT_SET, NULL, 2 );
    }
  }
}

/* ==========================================================================
FUNCTION BTApp_ClearBondable
DESCRIPTION
============================================================================= */
void BTApp_ClearBondable( CBTApp* pMe )
{
  if ( (pMe->mRM.bBondable != FALSE) &&
       (pMe->mAG.bEnabled == FALSE) &&
       (pMe->mNA.bEnabled == FALSE)
#ifdef FEATURE_BT_EXTPF_OPP
       && (pMe->mOPP.bRegistered == FALSE)
#endif

#ifdef FEATURE_BT_EXTPF_FTP
       && (pMe->mFTP.bRegistered == FALSE)
#endif

#ifdef FEATURE_BT_EXTPF_BIP
       && (pMe->mBIP.bRegistered == FALSE)
#endif

#ifdef FEATURE_BT_EXTPF_BPP
       && (pMe->mBPP.bRegistered == FALSE)
#endif

#ifdef FEATURE_BT_EXTPF_CTP
       && (pMe->mCTP.bRegistered == FALSE)
       && (pMe->mICP.bRegistered == FALSE)
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_AV
       && (pMe->mA2DP.bEnabled  == FALSE)
       && (pMe->mAVRCP.bEnabled == FALSE)
#endif

#ifdef FEATURE_BT_EXTPF_PBAP
       && (pMe->mPBAP.bSrvRegistered == FALSE)
#endif
       )
  {
    if ( IBTEXTRM_SetBondable( pMe->mRM.po, FALSE ) != SUCCESS )
    {
      BTApp_ShowMessage( pMe, IDS_MSG_BONDABLE_NOT_CLEARED, NULL, 2 );
    }
  }
}


/*=============================================================================
FUNCTION: BTApp_DoSpeedDialLookup

DESCRIPTION:  Searches the Contacts database in the speed dial list category
              for a contact number match for the the speed dial index.

PARAMETERS:
   *pIShell        [in]:  IShell object pointer
   *speeddialindex [in]:  index number in the speed dial list range 1 - 99
   *number         [out]:  Phone number to lookup
   size:           [in]:  Size of name in bytes

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void BTApp_DoSpeedDialLookup(IShell *pIShell,
                             AECHAR *speeddialindex,
                             AECHAR *number,
                             uint32 size)
{
#ifdef FEATURE_BMP
  int nLenReq;
  int nPhoneNums = 0;

  pim_IContacts *pEfsContacts = NULL; // we only support the Contact Book on the phone
  pim_IContactsSearch *piContactsSearch = NULL;
  pim_IContactsResults* piResults = NULL;
  pim_IContactsRecord* piRec = NULL;

  char KeyStr[ sizeof("Phone/xyz/SpeedDial") ] = {"Phone/"};
  AECHAR wszKeyStr[ (sizeof("SPEEDDIAL = ") + AEEBT_MAX_MEM_DIAL_DIGITS) *
                      sizeof(AECHAR) ] = {L"SPEEDDIAL = "};

  if ( (NULL == pIShell) ||
       (NULL == number) ||
       (NULL == speeddialindex) ||
       (0 == WSTRLEN(speeddialindex)) )
  {
    MSG_ERROR("BTApp_DoSpeedDialLookup - invalid param", 0, 0, 0);
    return;
  }

  // Construct the key string for record search
  WSTRLCAT( wszKeyStr, speeddialindex, sizeof(wszKeyStr) / sizeof(AECHAR) );

  if ( AEE_SUCCESS != ISHELL_CreateInstance( pIShell,
                                             pim_AEECLSID_EFSDBDevice,
                                             (void **)&pEfsContacts ) )
  {
    MSG_ERROR("BTApp_DoSpeedDialLookup - faied to create EfsContacts",
              0, 0, 0);
  }
  // we don't actually have to create an IContactsSearch object if we are
  // only earching for EFS contacts. we can use pim_IContacts_Search with the
  // pEfsContacts object. IContactsSearch is intended for searching across
  // multiple icontacts devices.
  // 
  // however, we want the code to be extendable for future support to devices
  // other than EFS contacts.
  else if ( AEE_SUCCESS != ISHELL_CreateInstance( pIShell,
                                                  pim_AEECLSID_ContactsSearch,
                                                  (void **)&piContactsSearch ) )
  {
    MSG_ERROR("BTApp_DoSpeedDialLookup - faied to create iContactsSearch",
              0, 0, 0);
  }
  else if ( AEE_SUCCESS != pim_IContactsSearch_Search( piContactsSearch,
                                                       &pEfsContacts,
                                                       1,
                                                       wszKeyStr,
                                                       &piResults ) )
  {
    MSG_ERROR("BTApp_DoSpeedDialLookup - contact search failed", 0, 0, 0);
  }
  // there should only be one result.
  // get the number of phone entries on the record here as well
  else if ( (AEE_SUCCESS != pim_IContactsResults_GetNext(
                              piResults, &piRec )) ||
            (AEE_SUCCESS != pim_IContactsRecord_GetNumChildren(
                              piRec,
                              "Phone/",
                              &nPhoneNums )) ||
            (999 < nPhoneNums) ) // normally, nPhoneNums should have less than 3 digits
  {
    MSG_ERROR("BTApp_DoSpeedDialLookup - record retrieval failed, phNums=%d",
              nPhoneNums, 0, 0);
  }
  else // so far so good, let's find the number to dial
  {
    while ( nPhoneNums )
    {
      // Construct the key string for SpeedDial index
      std_strlprintf( KeyStr, sizeof(KeyStr),
                      "Phone/%d/SpeedDial",
                      nPhoneNums );

      if ( (AEE_SUCCESS != pim_IContactsRecord_GetFieldValue(
                             piRec,
                             KeyStr,
                             wszKeyStr,
                             sizeof(wszKeyStr) / sizeof(AECHAR),
                             &nLenReq )) ||
          ((sizeof(wszKeyStr) / sizeof(AECHAR)) < nLenReq) )
      {
        MSG_ERROR("BTApp_DoSpeedDialLookup - failed to get wszKeyStr",
                  0, 0, 0);
        break;
      }
      else if ( 0 == WSTRNCMP( wszKeyStr, speeddialindex, sizeof(wszKeyStr) ) )
      {
        //hit! now get the number associated with the entry
        std_strlprintf( KeyStr, sizeof(KeyStr),
                        "Phone/%d/Value",
                        nPhoneNums );

        if ( (AEE_SUCCESS != pim_IContactsRecord_GetFieldValue(
                               piRec,
                               KeyStr,
                               number,
                               size,
                               &nLenReq) ) ||
             (size < nLenReq) )
        {
          MSG_ERROR("BTApp_DoSpeedDialLookup - failed to get number", 0, 0, 0);
          number[0] = (AECHAR) '\0';
        }

        break;
      }
      // else, continue
    } // end of the loop
  }

  IQI_RELEASEIF(pEfsContacts);
  IQI_RELEASEIF(piContactsSearch);
  IQI_RELEASEIF(piResults);
  IQI_RELEASEIF(piRec);

#else /* else, !FEATURE_BMP */

  IAddrBook *pIAddrBook = NULL;
  IAddrRec *addrRec = NULL;
  int       wTotalFields;
  int       j;
  uint16    wIndex; //will hold integer value of speeddialindex
  boolean   bFound;

  if ( (NULL == pIShell) ||
       (NULL == number) ||
       (NULL == speeddialindex) ||
       (0 == WSTRLEN(speeddialindex)) )
  {
    MSG_ERROR("BTApp_DoSpeedDialLookup - invalid param", 0, 0, 0);
    return;
  }

  if ( ISHELL_CreateInstance( pIShell, AEECLSID_ADDRBOOK,
                              (void**) &pIAddrBook ) != SUCCESS )
  {
    MSG_ERROR( "BTApp_DoSpeedDialLookup - failed to create IAddrBook obj",
               0, 0, 0 );
    return;
  }

  wIndex = (uint16)WSTRTOFLOAT(speeddialindex);

  //Start searching the address book with the search string
  if ( SUCCESS == IADDRBOOK_EnumRecInit( pIAddrBook, AEE_ADDR_CAT_SPEEDDIAL,
                                         AEE_ADDRFIELD_NONE, NULL, 0) ) 
  {
    for (;;) 
    {
      bFound = FALSE;
      addrRec = IADDRBOOK_EnumNextRec( pIAddrBook );
      if ( NULL == addrRec ) 
      {
        // No more matching records found, exit loop
        break;
      }
      //Get the name of the Contact from the record
      wTotalFields = IADDRREC_GetFieldCount(addrRec);
      for ( j = 0; j < wTotalFields; j++ )
      {
        AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);
        if ( addrField && (addrField->fID == AEE_ADDRFIELD_INDEX) ) 
        {
          ASSERT(addrField->pBuffer != NULL);
          if ( wIndex == *((uint16 *)addrField->pBuffer) )
          {
            bFound = TRUE;
          }
        }
      }
      /* Should not assume that order of fields within the record
         will remain the same. If that is a BREW requirement, we need
         to change handling of AEE_ADDRFIELD_INDEX in OEM layer */
      if ( bFound == TRUE )
      {
        for ( j = 0; j < wTotalFields; j++ ) 
        {
          AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);
          if ( addrField && ((addrField->fID == AEE_ADDRFIELD_PHONE_OTHER) ||
                             (addrField->fID == AEE_ADDRFIELD_PHONE_GENERIC)) )
          {
            //if the slot id has already matched
            (void) WSTRNCOPYN(number,
                              (int)(size / sizeof(AECHAR)),
                              addrField->pBuffer,
                              -1);

            // Found one, bail out immediately
            //
            // NOTE:
            //    There could be more than one contact database match,
            //    but for simplicity just take the first one that was
            //    found.
            //
            IADDRREC_Release(addrRec);
            IADDRBOOK_Release( pIAddrBook );
            return;
          }
        }
      }
      IADDRREC_Release(addrRec);
    }
  }
  IADDRBOOK_Release( pIAddrBook );
#endif /* FEATURE_BMP */
}


/* ==========================================================================
FUNCTION BTApp_GetBatteryLevel
DESCRIPTION
============================================================================= */
boolean  BTApp_GetBatteryLevel( CBTApp *pMe , uint8* battLevel )

{
  uint32  pdwData;
  uint16  uBattLevel ;
  uint16  uBattScale;
  uint8   ATBattLevel;
  uint8   uLevel;
  if ( IBATTERY_GetBatteryLevel( pMe->mAG.pIBattery, 
                                 &(pdwData) ) != SUCCESS )
  {
    MSG_ERROR("BTApp_GetBatteryLevel - Cannot obtain battery level", 0, 0, 0);
    return FALSE;
  }
  uBattLevel = GETBATTERYLEVEL(pdwData);
  uBattScale = GETBATTERYSCALE(pdwData);
  if ( uBattScale == 0 )
  {
    MSG_ERROR("BTApp_GetBatteryLevel - Invalid no of Batt Scale", 0, 0, 0);
    return FALSE;
  }
  if ( uBattLevel == 0 )
  {
    ATBattLevel = 0;
  }
  else
  {
    ATBattLevel = (uBattLevel * 100) / uBattScale;
  }
  /* convert to 0-5 levels */
  if ( ATBattLevel == 0 )
  {
    uLevel = AEEBT_AG_BATTCHG_LEVEL_0;
  }
  else if ( ATBattLevel <= 20 )
  {
    uLevel = AEEBT_AG_BATTCHG_LEVEL_1;
  }
  else if ( ATBattLevel <= 40 )
  {
    uLevel = AEEBT_AG_BATTCHG_LEVEL_2;
  }
  else if ( ATBattLevel <= 60 )
  {
    uLevel = AEEBT_AG_BATTCHG_LEVEL_3;
  }
  else if ( ATBattLevel <= 80 )
  {
    uLevel = AEEBT_AG_BATTCHG_LEVEL_4;
  }
  else
  {
    uLevel = AEEBT_AG_BATTCHG_LEVEL_5;
  }

  MSG_LOW( "ProcessBattEvents - Batt indicator value=%d", uLevel, 0, 0 );
  *battLevel = uLevel;
  return TRUE;
}

/* ==========================================================================

FUNCTION BTAPP_Str2BDAddr

DESCRIPTION

============================================================================= */

boolean BTApp_Str2BDAddr
(
  const char*         pStr,  // str len should be twice size of BD Address
  AEEBTBDAddr*        pAddr
)
{
  uint8  len = sizeof( AEEBTBDAddr ) * 2;
  uint8  j = sizeof( AEEBTBDAddr );
  char   ch;
  uint8  i, k, l;
  i = 0;
  while ( i < len )
  {
    k = 0;
    l = 0;
    ch = pStr[i++];
    k = (uint8) ( (ch >= 'A') ? ( 10 + (uint8)( ch - 'A' )) : (ch - '0') );

    if ( k > 15 )
    {
      MSG_LOW( "Str2BDAddr - illegal BD address", 0, 0, 0 );
      return FALSE;
    }
    k = (k << 4) & 0xF0;
    ch = pStr[i++];
    l = (uint8) ( (ch >= 'A') ? ( 10 + (uint8)( ch - 'A' )) : (ch - '0') );
    if ( l > 15 )
    {
      MSG_LOW( "Str2BDAddr - illegal BD address", 0, 0, 0 );
      return FALSE;
    }
    l = l & 0x0F;
    pAddr->uAddr[--j] = k | l;
  }
  return TRUE;
}

/* ==========================================================================

FUNCTION BTApp_CheckToClearDiscoverable

DESCRIPTION

============================================================================= */
void BTApp_CheckToClearDiscoverable( CBTApp* pMe )
{
  if ( !BTApp_IsDiscoverabilityReq(pMe) && ( pMe->mSD.bDiscoverable !=FALSE ) )
    {
      IBTEXTSD_SetDiscoverable( pMe->mSD.po, FALSE );
    }
}

/* ==========================================================================

FUNCTION BTApp_IsDiscoverabilityReq

DESCRIPTION

============================================================================= */
boolean BTApp_IsDiscoverabilityReq(CBTApp* pMe )
{
    if( TRUE 
#ifdef FEATURE_BT_EXTPF_OPP
       && ( pMe->mOPP.bRegistered == FALSE )
#endif
#ifdef FEATURE_BT_EXTPF_FTP
       && ( pMe->mFTP.bRegistered == FALSE )
#endif
#ifdef FEATURE_BT_EXTPF_BIP
       && ( pMe->mBIP.bRegistered == FALSE )
#endif
#ifdef FEATURE_BT_EXTPF_BPP
       && ( pMe->mBPP.bRegistered == FALSE )
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
       && ( pMe->mPBAP.bSrvRegistered == FALSE )
#endif 
#ifdef FEATURE_IOBEX
       && ( pMe->mOBEX.bRegistered  == FALSE )
#endif 
#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif
#ifdef FEATURE_BT_EXTPF_AV
       && ( pMe->mA2DP.bEnabled  == FALSE )
       && ( pMe->mAVRCP.bEnabled == FALSE )
#endif
       )
    {
         return FALSE;
    }
    return TRUE;

}
        
/* ==========================================================================

FUNCTION BTApp_IsBTAppOn

DESCRIPTION

============================================================================= */    
boolean BTApp_IsBTAppOn(CBTApp* pMe )
{
  if( BTApp_IsDiscoverabilityReq(pMe ) ||
      ( pMe->mAG.bEnabled ) ||
      ( pMe->mNA.bEnabled ) ||
      ( pMe->mL2.bEnabled ) 
     )
  {
      return TRUE;
  }
  return FALSE;
}

/* ==========================================================================

FUNCTION BTApp_AllocConn

DESCRIPTION
Allocates new entry in APP manager connection table.
============================================================================= */    
boolean BTApp_AllocConn( AEEBTBDAddr* pBDAddr, uint8 *pIndex )
{
  uint8 i;

  for(i = 0; i < 7; i++)
  {
    if( appMgr[i].bValid != TRUE ) 
    {
      appMgr[i].bdAddr = *pBDAddr;
      appMgr[i].bValid = TRUE;
      *pIndex = i ;
      return TRUE ;
    }
  }

  return FALSE;
}

/* ==========================================================================

FUNCTION BTApp_ConnFromBDAddr

DESCRIPTION
Gets connection index from BD address.
============================================================================= */    
boolean BTApp_ConnFromBDAddr( AEEBTBDAddr* pBDAddr, uint8 *pIndex )
{
  uint8 i;

  for(i = 0; i < 7; i++)
  {
    if( (AEEBT_BD_ADDRS_EQUAL( &appMgr[i].bdAddr, pBDAddr ) == TRUE) &&
          (appMgr[i].bValid == TRUE)) 
    {
      *pIndex = i ;
      return TRUE ;
    }
  }
  return FALSE;
}

/* ==========================================================================

FUNCTION    BTApp_SelectLowPowerCfg

DESCRIPTION 
Selects low power config type depending on profile type and state 

============================================================================= */    
AEEBTLowPowerControl BTApp_SelectLowPowerCfg( uint8 connIndex )
{
  uint32 profileType ;
  uint32 stateType ;
  
  profileType = appMgr[connIndex].profileType ;
  stateType = appMgr[connIndex].stateType ;

  MSG_HIGH( "BTAPP-Mgr: LPC : Profile:%x States:%x", 
                      profileType, stateType, 0 );

  if( (stateType == BTAPP_MGR_USE_A2DP) && 
            (profileType & BTAPP_MGR_A2DP) ) 
  {
    return AEEBT_LPC_ACTIVE ;
  }

  if( (profileType & BTAPP_MGR_FTP) || 
       (profileType & BTAPP_MGR_OPP) || 
        (profileType & BTAPP_MGR_BIP) || 
          (profileType & BTAPP_MGR_BPP) ||
            (profileType & BTAPP_MGR_PBAP) ) 
  {
    return AEEBT_LPC_DONT_CARE ;
  }

  if( (profileType & BTAPP_MGR_AG) ||
        (profileType & BTAPP_MGR_SAP) ) 
  {
    return AEEBT_LPC_SNIFF_LOCK ;
  }

  return AEEBT_LPC_DONT_CARE ;
}
 
/* ==========================================================================

FUNCTION      BTAppMgr_UpdateProfileType

DESCRIPTION
Updates profile connection and configures low power mode.

============================================================================= */    
void BTAppMgr_UpdateProfileType(CBTApp* pMe, AEEBTBDAddr* pBDAddr, uint32 profileType, 
                                  uint8 state )
{
  uint8 connIndex = 0;
  AEEBTLowPowerControl lowPwrMode;

  MSG_HIGH( "BTAPPMgr Update Profile: State=%x ProfileType=%x", 
                                      state, profileType, 0 );

  if ( AEEBT_BD_ADDRS_EQUAL( pBDAddr, &NULL_BD_ADDR ) == TRUE )
  {
    return;
  }

  if( BTApp_ConnFromBDAddr( pBDAddr, &connIndex ) != TRUE ) 
  {
    if (state == BTAPP_MGR_CONNECTED) 
    {
      if( BTApp_AllocConn( pBDAddr, &connIndex ) != TRUE )
      {
        MSG_MED( "BTAPP-Mgr:Update Profile Type : Out of free entry", 0, 0, 0 );
        return;
      }
      else
      {
        MSG_MED( "BTAPP-Mgr:Update Profile Type : New conn", 0, 0, 0 );
      }
    }
    else
    {
      MSG_MED( "BTAPP-Mgr:Update Profile Type: Invalid connection", 0, 0, 0 );
      return;
    }
  }
  else
  {
    MSG_MED( "BTAPP-Mgr:Update Profile Type: Existing conn", 0, 0, 0 );
  }

  if(state == BTAPP_MGR_CONNECTED)
  {
    /*Connected - Update profile type*/
    appMgr[connIndex].profileType |= profileType ;
    appMgr[connIndex].numProfiles++ ;
  }

  if(state == BTAPP_MGR_DISCONNECTED)
  {
    /*Disconnected - Clear profile type*/
    profileType = ~ profileType ;
    appMgr[connIndex].profileType &= profileType ;
    appMgr[connIndex].numProfiles-- ;
  }

  if( !appMgr[connIndex].profileType )
  {
    MSG_MED( "BTAPPMgr:Update Profile: No profiles on this BD addr", 0, 0, 0) ;
    /*No more profiles on this BD address. Free entry*/
     appMgr[connIndex].bValid = FALSE; 
     appMgr[connIndex].currentLPMode = 0x00;
     appMgr[connIndex].numProfiles = 0x00;
     if ( IBTEXTRM_LowPowerConfig( pMe->mRM.po, pBDAddr, 
                                     AEEBT_LPC_DONT_CARE, 0,
                                     0, 0, 
                                     0, 0 ) != SUCCESS )
      {
        MSG_MED( "LowPowerConfig : Failed", 0, 0, 0 );
      }
  }
  else
  {
    MSG_MED( "BTAPP-Mgr:Current LPC : LPC:%x", appMgr[connIndex].currentLPMode, 0, 0 );
    lowPwrMode = BTApp_SelectLowPowerCfg (connIndex);
    MSG_MED( "BTAPP-Mgr:New LPC : LPC:%x", lowPwrMode, 0, 0 );

    if( lowPwrMode != appMgr[connIndex].currentLPMode )
    {
      appMgr[connIndex].currentLPMode = lowPwrMode ;
      if ( IBTEXTRM_LowPowerConfig( pMe->mRM.po, pBDAddr, 
                                     lowPwrMode, 0,
                                     0, 0, 
                                     0, 0 ) != SUCCESS )
      {
        MSG_MED( "LowPowerConfig : Failed", 0, 0, 0 );
      }
    }
  }
  return;
}

/* ==========================================================================

FUNCTION      BTAppMgr_UpdateStateType

DESCRIPTION   
Updates state for each connection and configures low power mode.

============================================================================= */    
void BTAppMgr_UpdateStateType( CBTApp* pMe, AEEBTBDAddr* pBDAddr, uint32 stateType )
{
  AEEBTLowPowerControl lowPwrMode;
  uint8 connIndex = 0;

  MSG_HIGH( "BTAPPMgr: Update State : StateType=%x", stateType, 0, 0 );

  if ( AEEBT_BD_ADDRS_EQUAL( pBDAddr, &NULL_BD_ADDR ) == TRUE )
  {
    return;
  }

  if ( BTApp_ConnFromBDAddr( pBDAddr, &connIndex ) != TRUE )
  {
    MSG_MED( "BTAPP-Mgr:Update State Type: Invalid connection", 0, 0, 0 );
    return;
  }
    
  appMgr[connIndex].stateType = stateType ;

  MSG_MED( "BTAPP-Mgr:Current LPC : LPC:%x", appMgr[connIndex].currentLPMode, 0, 0 );
  lowPwrMode = BTApp_SelectLowPowerCfg (connIndex);
  MSG_MED( "BTAPP-Mgr:New LPC : LPC:%x", lowPwrMode, 0, 0 );

  if( lowPwrMode != appMgr[connIndex].currentLPMode )
  {
    appMgr[connIndex].currentLPMode = lowPwrMode ;
    MSG_MED( "BTAPP-Mgr:Update State Type: LPC:%x", lowPwrMode, 0, 0 );
    if ( IBTEXTRM_LowPowerConfig( pMe->mRM.po, pBDAddr, 
                                    lowPwrMode, 0,
                                    0, 0, 
                                    0, 0 ) != SUCCESS )
    {
      MSG_MED( "LowPowerConfig : Failed", 0, 0, 0 );
    }
  }
  return;
}

#endif
