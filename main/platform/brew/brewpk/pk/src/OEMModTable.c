/*======================================================
FILE:  OEMModTable.c

SERVICES:  BREW Supplied Static Module/Class Tables.

GENERAL DESCRIPTION:
   This file specifies static module table and static class
   table, that BREW uses to instantiate objects.

   Typically, OEMs need not modify this file as all the
   entries are featurized using FEATURE_XXX feature flag defines.
   OEMs can control the inclusion of entries by defining the feature
   flags in OEMFeatures.h

   OEMs should not modify this file to add new classes/modules. 
   This should be done through the creation or modification of OEMModTableExt.c. 
   For more information on the format of this file see OEMModTableExt.h. 
   OEMs who are using OEMModTableExt.c should verify that 
   FEATURE_BREW_EXTENDED_MODTABLE is defined in OEMFeatures.h

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright 1999-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
=====================================================*/
#include "OEMFeatures.h"
#if defined(OEMMODTABLE)
#include "OEMIMM.h"
#include "OEMIMUI.h"

#include "OEMModTable.h"
#include "OEMModTableExt.h"

#include "OEMClassIDs.h"
#include "OEMMediaFormats.h"

#if defined(FEATURE_DEV_NOTIFIER)
#include "OEMDeviceNotifier.h"
#endif

#if defined(FEATURE_BREW_APPFRAME)
#include "OEMAppFrame.h"
#endif

#if defined(FEATURE_BREW_ANNUNCIATOR_CONTROL)
#include "AEEAnnunciatorControl.bid"
#endif

#if defined(FEATURE_ADDRBOOK_RUIM)
#include "ADDRBOOK_RUIM.bid"
#endif

//#if defined(FEATURE_SEC)
//#include "SecCrypt.bid"
//#endif

#ifdef FEATURE_FM
#include "AEEFMRadio.bid"
#include "AEEFMRDS.bid"
extern int OEMFMRadio_New(IShell *ps, AEECLSID cls, void **ppObj);
extern int OEMFMRDS_New(IShell *ps, AEECLSID cls, void **ppObj);
#endif // FEATURE_FM

#if defined(FEATURE_IFOTA)
#include "FOTA.bid"
#endif

#include "ALARMMGR.BID"

#if (defined(FEATURE_IFLIP)||(defined(AEE_SIMULATOR) && !defined(_AEEWIN)))
#include "AEECLSID_FLIP.BID"
#endif

#if defined(FEATURE_GSM1x)
#include "AEEGSM1xControl.h"
#include "AEEGSMSMS.h"
#include "AEEGSM1xSig.h"
#endif  /* FEATURE_GSM1x */

#include "AEE_OEMDispatch.h"

#include "AEEStockFonts.h"

#include "AEEFile.h"

#include "AEEHtmlViewer_static.h"

#include "AEEBTIL_static.h"

#include "AEEWebUtil_static.h"
#include "AEEWebOpts_static.h"
#include "AEESecurity_static.h"

#include "AEEResArbiter.h"

#if defined(FEATURE_BREW_ROOTCERTS)
   #include "AEERootCerts.h"
#endif

#if defined(FEATURE_BREW_USBDRIVER)
   #include "AEECLSID_USBDRIVER.bid"
#endif

#include "AEELogger.h"

#if defined(FEATURE_BREW_NETMTPDNOTIFIER)
#include "AEECLSID_NETMTPDNOTIFIER.BID"
#endif // defined(FEATURE_BREW_NETMTPDNOTIFIER)

#if defined(FEATURE_BREW_MCAST)
#include "AEECLSID_MCASTSESSION.BID"
#include "AEECLSID_BCMCSDATABASE.bid"
#endif

#if defined(FEATURE_BREW_MCAST)
   #include "dssocket.h"
   #if ((defined DSS_VERSION) && (DSS_VERSION >= 1703 && DSS_VERSION != 1800))
      #define FEATURE_BREW_MCAST2_SUPPORTED
   #endif // ((defined DSS_VERSION) && (DSS_VERSION >= 1703 && DSS_VERSION != 1800))
#endif // defined(EATURE_BREW_MCAST)

#if defined(FEATURE_BREW_MCAST2_SUPPORTED)
#include "AEECLSID_MCASTSESSION2.bid"
#include "AEECLSID_MCASTBUNDLE.bid"
#endif

#if defined(FEATURE_BREW_QOS)
#include "AEECLSID_QOSSPEC.BID"
#include "AEECLSID_QOSLIST.BID"
#include "AEECLSID_QOSSESSION.BID"
#include "AEECLSID_QOSFLOW.BID"
#include "AEECLSID_QOSFILTER.BID"
#include "AEECLSID_QOSBUNDLE.bid"
#include "AEECLSID_PRIMARYQOSSESSION.bid"
#endif // defined(FEATURE_BREW_QOS)

#include "AEECLSID_NETWORK.BID"
#include "AEECLSID_NETUTILS.bid"
#include "AEECLSID_NETUTILS2.bid"

#if defined(FEATURE_BREW_DEBUGGER)
   #include "AEEDebugger.h"
#endif /* FEATURE_BREW_DEBUGGER */

#if defined(FEATURE_BREW_BATTERY)
#include "AEEBATTERY.bid"
#include "AEEBATTERYNOTIFIER.bid"
#endif // FEATURE_BREW_BATTERY

#if defined(FEATURE_BACKLIGHT)
#include "AEEBacklight.h"
#endif // FEATURE_BACKLIGHT

#if defined(FEATURE_BREW_RSA)
#include "RSA.bid"
#endif // FEATURE_BREW_RSA

#if (defined(FEATURE_BREW_JOYSTICK))
#include "AEEJoystick.h"
#endif

#if defined(FEATURE_BREW_USBDRIVER)
#include "AEECLSID_USBDRIVER.bid"
#endif

#if defined(FEATURE_BREW_NATIVENOTIFIER)
#include "NativeNotifier.bid"
extern int OEMNativeNotifierClass_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif // FEATURE_BREW_NATIVENOTIFIER

#ifdef FEATURE_BREW_IME // {
//extern int OEMIMUI_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif // } FEATURE_BREW_IME

#if defined(FEATURE_BREW_IME) || defined(FEATURE_ZICORP_EZITEXT) // {
extern int OEMIMMgr_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif // } FEATURE_BREW_IME || FEATURE_ZICORP_EZITEXT

#if defined(FEATURE_DEV_NOTIFIER) // {
extern int AEEDeviceNotifier_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif // } FEATURE_DEV_NOTIFIER

#if defined(FEATURE_BACKLIGHT) // {
extern int AEEBacklight_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif // } FEATURE_BACKLIGHT

#if defined(FEATURE_A1)
#include "A1ModEnv.bid"
extern int A1ModEnv_CreateInstance(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif // FEATURE_A1

#if defined(FEATURE_BREW_ISYNTH)
#include "AEECSynth.bid"
#endif

#if defined(FEATURE_EXTENDED_KEYS)
#include "AEECLSID_KEYSMAPPING.bid"
#endif

#include "AEEServingNetworks.bid"
#include "AEEMannerMode.bid"
//
//
// Static Module Table
//
// The static module table contains the entries that correspond to the static
// modules installed on the handset.  This mechanism allows static applications
// and extensions to execute in a manner that mirrors the MIF/MOD mechanism
// used for dynamic code.  The mechanism works as follows:
//
// - MIF files are converted to source (bin2src)
// - A reference to a global variable containing the MIF data is added to
//   OEMFSPersistent.c.  This allows the MIF to be visible to the
//   OEMFS (IFileMgr / IFile) layers
// - The static code for the module/applet is cross-referenced to
//   the static MIF by name in the StaticModList below.
// - At startup, BREW enumerates MIF files and discovers the
//   static MIFs found in OEMFSPersistent.c
// - After the MIFs are enumerated and loaded, the StaticModList is
//   enumerated and entries are matched to the corresponding MIFs
//   found in the file system.
// - At load time, the module is accessed by calling the PFNMODENTRY
//   listed in the table entry for the MIF.
//

//
// Default Image Viewer App
//
#if defined(FEATURE_DEFAULT_VIEW_APP)
extern int ViewApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

//
// App Manager
//
#if defined(FEATURE_APP_MANAGER)
extern int AppMgr_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(AEE_SIMULATOR) && !defined(_AEEWIN)
extern int EmuIdleApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_IFOTA)
extern int OEMFOTA_New(IShell *piShell, AEECLSID cls, void **ppif);
#endif

extern int AEEAlarmMgr_New(IShell * ps, AEECLSID cls, void **ppif);
#if (defined(FEATURE_IFLIP)|| (defined(AEE_SIMULATOR) && !defined(_AEEWIN)))
extern int OEMFLIP_New(IShell* piShell, AEECLSID cls, void **ppif);
#endif

#if (defined(FEATURE_BREW_JOYSTICK))
extern int AEEJoystick_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#include "AEEMemCache.bid"
#ifndef FEATURE_MANGO_BREW
extern int OEMMemCache_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_AEE_DEBUGSET)
extern int AEEDebugApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEESMSDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEETelephonyDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEENetworkDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEEDnsDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEEWebDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEEQOSDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEESocketDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEEDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int AEEFileDebugSet_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if (defined(FEATURE_BREW_ISYNTH))
extern int ISynth_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

/*==================================================================
   Returns pointer to lists of static module entries.
==================================================================*/
const AEEStaticMod ***OEMMod_GetStaticModLists(void)
{
   //
   //
   // MODULE TABLE DECLARATION
   //
   //
   static AEEStaticMod asmBREW[] = {
#if defined(FEATURE_APP_MANAGER)
      {AEEFS_MIF_DIR"brewappmgr.mif", AppMgr_Load},
#endif
#if defined(FEATURE_DEFAULT_VIEW_APP) && !defined(_AEEWIN)
      {AEEFS_MIF_DIR"viewapp.mif", ViewApp_Load},
#endif
#if defined(AEE_SIMULATOR) && !defined(_AEEWIN)
      {AEEFS_MIF_DIR"emuidleapp.mif", EmuIdleApp_Load},
#endif
#if defined(FEATURE_AEE_DEBUGSET)
      {AEEFS_MIF_DIR"aeedebugapp.mif", AEEDebugApp_Load},
      {AEEFS_MIF_DIR"aeefiledebugset.mif", AEEFileDebugSet_Load},
      {AEEFS_MIF_DIR"aeesmsdebugset.mif", AEESMSDebugSet_Load},
      {AEEFS_MIF_DIR"aeetelephonydebugset.mif", AEETelephonyDebugSet_Load},
      {AEEFS_MIF_DIR"aeenetworkdebugset.mif", AEENetworkDebugSet_Load},
      {AEEFS_MIF_DIR"aeednsdebugset.mif", AEEDnsDebugSet_Load},
      {AEEFS_MIF_DIR"aeewebdebugset.mif", AEEWebDebugSet_Load},
      {AEEFS_MIF_DIR"aeeqosdebugset.mif", AEEQOSDebugSet_Load},
      {AEEFS_MIF_DIR"aeesocketdebugset.mif", AEESocketDebugSet_Load},
      {AEEFS_MIF_DIR"aeedebugset.mif", AEEDebugSet_Load},
#endif
      {NULL,NULL}
   };

   static const AEEStaticMod *aasmBREW[] = {
      asmBREW,
      0,
   };

   static const AEEStaticMod **aaasm[] = {
      aasmBREW,            /* supplied above... */
#ifdef FEATURE_BREW_EXTENDED_MODTABLE
      gOEMStaticModLists,  /* supplied by OEMModTableExt.c */
#endif
      0,
   };

   return aaasm; /* array of arrays of arrays of AEEStaticMods */
}



/*==================================================================
   AEEStaticClass lists...
==================================================================*/

//
// OEM Text Input Control
//
extern int TextCtl_New(IShell * piShell, AEECLSID cls, void **ppif);

//
// DMSS TAPI Entry-points
//
#if defined(FEATURE_BREW_TAPI)
extern int AEETAPI_New(IShell * ps, AEECLSID cls, void **ppif);

#if !defined(AEE_SIMULATOR)
extern int AEEPhone_New(IShell * piShell, AEECLSID cls, void **ppif);
#endif /* #if !defined(AEE_SIMULATOR) */
#endif /* #if defined(FEATURE_TAPI) */

//
// DB Module (AEEDB.lib)
//
#if defined(AEEDB_LIB)
extern int AEEDBMgr_New(IShell *ps, AEECLSID cls, void ** ppo);
#endif

#if defined(FEATURE_SOUNDPLAYER)
int AEESoundPlayer_New(IShell * pShell, AEECLSID clsid, void **ppobj);
#endif

extern const AEEStaticClass gascAEEGraphics[];

extern const AEEStaticClass gAEESysClockClasses[];
extern const AEEStaticClass gAEESecSysClockClasses[];

// Most common encodings & transformation pipes support
#if defined (FEATURE_ENCODE)
extern int AEEEncoding_New(IShell*, AEECLSID, void**);
#endif

//
// OEM Ringers Interface (OEMRinger.c)
//
#if defined(OEMRINGERS)
extern int  Ringer_New(IShell * ps, AEECLSID cls, void ** ppobj);
#endif

//
// GIF Viewer
//
#if defined(FEATURE_GIF_DECODE)
extern void GIFViewer_Init(IShell *ps);
extern int  GIFViewer_New(IShell * ps, AEECLSID cls, void ** ppobj);
#endif

//
// Unzip Stream
//
#if defined(FEATURE_UNZIP)
extern int  AEEUnzip_New(IShell * ps, AEECLSID cls, void ** ppobj);
#endif

//
//  Security
//
#if defined(FEATURE_BREW_SECURITY)
extern int AEERandomSource_New(IShell *ps, AEECLSID cls, void ** ppobj);

#endif

//
//  IRSA
//
#if defined(FEATURE_BREW_RSA)
extern int AEERSA_New(IShell *ps, AEECLSID cls, void ** ppobj);
#endif

//
// DES and 3DES
//
#if defined(FEATURE_BREW_DES3DES)
extern int AEEDES3DES_New(IShell *ps, AEECLSID ClsId, void ** ppObj);
#endif

#if defined(FEATURE_BREW_SEED)
extern int AEESEED_New(IShell *ps, AEECLSID ClsId, void ** ppObj);
#endif


#if defined(FEATURE_GSM1x)
// GSM1x Interfaces
extern void OEMGSM1xSig_Init(IShell *ps);
extern int OEMGSM1xSig_New(IShell * piShell, AEECLSID cls, void **ppif);

extern void OEMGSM1xSigNotifier_Init(IShell *ps);
extern int OEMGSM1xSigNotifier_New(IShell * piShell, AEECLSID cls, void **ppif);

extern void OEMGSM1xControl_Init(IShell *ps);
extern int OEMGSM1xControl_New(IShell * piShell, AEECLSID cls, void **ppif);

extern void OEMGSM1xControlNotifier_Init(IShell *ps);
extern int OEMGSM1xControlNotifier_New(IShell * piShell, AEECLSID cls, void **ppif);

extern void OEMGSMSMS_Init(IShell *ps);
extern int OEMGSMSMS_New(IShell * piShell, AEECLSID cls, void **ppif);

extern void OEMGSMSMSNotifier_Init(IShell *ps);
extern int OEMGSMSMSNotifier_New(IShell * piShell, AEECLSID cls, void **ppif);

#endif /* FEATURE_GSM1x */

//Address Book
#if defined(FEATURE_ADDRBOOK)
extern int AEEAddrBook_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int OEMAddrBook_New(IShell *ps,AEECLSID cls,void ** ppo);
#endif

// RUIM Address Book
#if defined(FEATURE_ADDRBOOK_RUIM)
extern int AEEAddrBook_New(IShell *ps,AEECLSID cls,void ** ppo);
extern int OEMRUIMAddrBook_New(IShell *ps,AEECLSID cls,void ** ppo);
#endif

// RUIM Interface
#if defined (FEATURE_IRUIM)
extern int OEMRUIM_New(IShell *ps, AEECLSID cls, void **pp);
#endif

// Call History Interface
#if defined (FEATURE_CALLHISTORY)
extern void OEMCallHistory_Init(IShell *pShell);
int OEMCallHistory_New(IShell *pIShell, AEECLSID ClsId, void **ppObj);
#endif

#if defined(FEATURE_BREW_SOUND)
extern void Sound_Init(IShell * ps);
extern int Sound_New(IShell * pShell,AEECLSID cls, void **ppif);
#endif

//Position Determination
#if defined(FEATURE_POSITION_DETERMINATION)
extern int AEEPosDet_New(IShell *piShell, AEECLSID cls,void **pp);
#endif

//Vocoder Frame Interface
#if (defined(FEATURE_IVOCODER) && !defined(AEE_SIMULATOR))
extern int AEEVocoder_New(IShell *ps, AEECLSID cls, void ** ppo);

#endif

// ICamera interface
#if defined(FEATURE_BREW_CAMERA)
#if defined(FEATURE_CAMERA_MULTI_SENSOR)
#include "AEECamera.bid"
#endif
extern void    AEECamera_Init(IShell * ps);
extern int     AEECamera_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

// ISerialPort interface
#if defined(FEATURE_BREW_SIO)
extern void    AEESIO_Init(IShell * ps);
extern int     ISerialPort_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

// USB Driver Interface
#if defined(FEATURE_BREW_USBDRIVER)
extern void OEMUSBDriver_Init(IShell *piShell);
extern int OEMUSBDriver_New(IShell *piShell, AEECLSID cls, void **ppif);
#endif /* #if defined(FEATURE_BREW_USBDRIVER) */

#if defined(FEATURE_BREW_RAMCACHE)
extern int IRamCache_New(IShell *piShell, AEECLSID uCls, void **ppif);
#endif /* #if defined(FEATURE_BREW_RAMCACHE) */

#if defined(FEATURE_BREW_SPRITE)
extern int Sprite_New(IShell *piShell, AEECLSID uCls, void **ppif);
#endif /* if defined(FEATURE_BREW_SPRITE) */

//
// Logging Interfaces
//
#if defined(FEATURE_LOGGER_FILE)
extern int  OEMLoggerFile_New(   IShell * ps, AEECLSID cls, void ** ppobj );
#endif
#if defined(FEATURE_LOGGER_SERIAL)
extern int  OEMLoggerSerial_New( IShell * ps, AEECLSID cls, void ** ppobj );
#endif
#if defined(FEATURE_LOGGER_DMSS)
extern int  OEMLoggerDMSS_New(   IShell * ps, AEECLSID cls, void ** ppobj );
#endif
#if defined(FEATURE_LOGGER_WIN)
extern int  OEMLoggerWin_New(    IShell * ps, AEECLSID cls, void ** ppobj );
#endif
#if defined(FEATURE_LOGGER_BTIL)
extern int  OEMLoggerBTIL_New(   IShell * ps, AEECLSID cls, void ** ppobj );
#endif


#if defined(FEATURE_BREW_ROOTCERTS)
   // Brew Root & TS certs
   extern int BrewRoots_New(IShell *piShell, AEECLSID uCls, void **ppif);
#endif

//
// App Frame
//
#if defined(FEATURE_BREW_APPFRAME)
extern int OEMAppFrame_New(IShell * piShell, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_BREW_ANNUNCIATOR_CONTROL)
extern int OEMAnnunciatorControl_New(IShell * pIShell, AEECLSID clsID, void ** ppobj);
#endif

//BREW Debugger
#if defined(FEATURE_BREW_DEBUGGER)
extern int  OEMDEBUGGER_New(   IShell * ps, AEECLSID cls, void ** ppobj );
#endif

#if defined(FEATURE_TOPVISIBLE)
extern int  AEETopVisibleCtl_New(IShell * pShell, AEECLSID cls, void ** ppObj);
#endif

#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_USES_LOWMEM)&&!defined(FEATURE_LOWER_MEM)
extern int CServingNetworks_New(IShell *pIShell, AEECLSID cls, void ** ppo);

#if defined(FEATURE_EXTENDED_KEYS)
extern int AEEKeysMapping_New(IShell *ps, AEECLSID cls, void **ppif);
#endif
#endif /*FEATURE_W021_LOW_MEMORY*/
#endif

extern int AEEMannerModeStore_New(IShell* ps, AEECLSID cls, void** ppif);

//
// Default Class ID Mappings (OEMDefaults.c)
//
extern const AEEStaticClass gascOEMDefaults[];

//
// Menu Module (AEEMenu.lib)
//
#if defined(AEEMENU_LIB)
extern const AEEStaticClass gMenuClassList[];
#endif

//
// Custom Control Module (AEEControls.lib)
//
#if defined(AEECONTROLS_LIB)
extern const AEEStaticClass gControlClassList[];
#endif

extern const AEEStaticClass gTelephoneClassList[];

//
// Device Bitmap
//
extern const AEEStaticClass gOEMDisplayDevClasses[];


//
// I3D stuff
//
#if defined(FEATURE_3D)
extern const AEEStaticClass gAEE3DClasses[];
extern const AEEStaticClass gAEE3DUtilClasses[];
extern const AEEStaticClass gAEE3DModelClasses[];
#endif

// IKeyMaker interface for access to Mutual Auth keys and other
#if defined(FEATURE_BREW_KEYMAKER)
#include "AEEKeyMaker.bid"
extern int AEEKeyMaker_New(IShell *pIShell, AEECLSID ClsId, void **ppObj);

#endif

#include "AEEGPSOneLock.bid"

//
// SMS Entry-points
//
#if defined(FEATURE_BREW_SMS)
extern int ISMSNotifier_New(IShell * ps, AEECLSID cls, void **ppif);
extern int ISMS_New(IShell * ps, AEECLSID cls, void **ppif);
extern int ISMSMsg_New(IShell * ps, AEECLSID cls, void **ppif);
extern int ISMSStorage_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int ISMSBCSrvOpts_New(IShell * ps, AEECLSID cls, void **ppif);
extern int ISMSBCConfig_New(IShell * ps, AEECLSID cls, void **ppif);
#endif // FEATURE_BREW_SMS

//
// Battery Entry-points
//
#if defined(FEATURE_BREW_BATTERY)
extern int IBatteryNotifier_New(IShell * ps, AEECLSID cls, void **ppif);
extern int IBattery_New(IShell * ps, AEECLSID cls, void **ppif);
#endif // FEATURE_BREW_BATTERY



// Resource Arbiter
extern int OEMResArbiter_New(IShell * ps, AEECLSID cls, void **ppif);

// Network MT PD notifier
#if defined(FEATURE_BREW_NETMTPDNOTIFIER)
extern int INetMTPDNotifier_New(IShell *pIShell, AEECLSID uCls, void **ppif);
#endif // defined(FEATURE_BREW_NETMTPDNOTIFIER)

#if defined(FEATURE_BREW_MCAST)
extern int IMcastSession_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int BcmcsDatabase_New(IShell *pIShell, AEECLSID uCls, void **ppif);
#endif

#if defined(FEATURE_BREW_MCAST2_SUPPORTED)
extern int IMcastBundle_New(IShell *pIShell, AEECLSID uCls, void **ppif);
#endif

#if defined(FEATURE_BREW_QOS)
extern int IQoSSpec_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int IQoSList_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int IQoSSession_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int IQoSFlow_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int IQoSFilter_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int IQoSBundle_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int IPrimaryQoSSession_New(IShell *pIShell, AEECLSID uCls, void **ppif);
#endif

extern int INetwork_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int INetUtils_New(IShell *pIShell, AEECLSID uCls, void **ppif);
extern int INetUtils2_New(IShell *pIShell, AEECLSID uCls, void **ppif);

extern int AEEGPSOneConfig_New(IShell * pShell, AEECLSID cls, void **ppif);


//
// Image Scaling
//
#if defined(FEATURE_BREW_SCALE)
extern const AEEStaticClass gAEEScaleClassList[];
#endif

//
// Generic Image Viewer
//
#ifndef FEATURE_MANGO_BREW
extern const AEEStaticClass gAEEGenericViewerClasses[];
#endif
//
// BMP Decoder/Viewer
//
#ifndef FEATURE_MANGO_BREW
extern const AEEStaticClass gAEEBmpClasses[];
#endif
//
// JPEG Decoder/Viewer
//
#ifndef FEATURE_MANGO_BREW
#if defined(FEATURE_JPEG_DECODER)
extern const AEEStaticClass gAEEIJGClasses[];
#endif
#endif
//
// PNG Decoder/Viewer
//
#if defined(FEATURE_BREW_PNG_DECODE)
extern const AEEStaticClass gAEEPNGClasses[];
#endif
//
// BCI Decoder/Viewer
//
#if defined(FEATURE_BCI_DECODE)
extern const AEEStaticClass gAEEBCIClasses[];
#endif




#if defined(FEATURE_BREW_SECURITY)
extern const AEEStaticClass gascICipherNew[];
#if defined(FEATURE_BREW_DES3DES)
extern const AEEStaticClass gascICipherNewDES[];
#endif
#if defined(FEATURE_BREW_SEED)
extern const AEEStaticClass gascICipherNewSEED[];
#endif
extern const AEEStaticClass gascICipherWrapper[];
extern const AEEStaticClass gascICipherFactory[];
#endif

extern const AEEStaticClass gascIWebUtil[];
extern const AEEStaticClass gascIWebOpts[];

#if defined(FEATURE_BREW_SECURITY)
extern const AEEStaticClass gascIHashCTX[];
extern const AEEStaticClass gascIHash[];
extern const AEEStaticClass gascICipher[];
#endif /* if defined(FEATURE_BREW_SECURITY) */

#if defined(FEATURE_BREW_AEENET)
extern const AEEStaticClass gascNetClassList[];
#endif

#if defined(FEATURE_BREW_AEETHREAD)
extern const AEEStaticClass gascThreadClassList[];
#endif

#if defined(FEATURE_BREW_PROVISION)
extern const AEEStaticClass gascProvisionClassList[];
#endif

#if defined(FEATURE_BREW_LICENSE)
extern const AEEStaticClass gascLicenseClassList[];
#endif

#if defined(FEATURE_BREW_AEEAPPHISTORY)
extern const AEEStaticClass gascAppHistoryClassList[];
#endif

#if defined(FEATURE_BREW_AEEAPPLETCTL)
extern const AEEStaticClass gascAppletCtlClassList[];
#endif

#if defined(FEATURE_BREW_SIGNAL)
extern const AEEStaticClass gascSignalClassList[];
#endif

#if defined(AEE_SIMULATOR)
extern const AEEStaticClass **AEESIM_GetStaticClassLists(void);
#endif /* #if defined(AEE_SIMULATOR) */

/*==================================================================
   Returns pointer to lists of static class entries.
==================================================================*/
const AEEStaticClass ***OEMMod_GetStaticClassLists(void)
{
   static const AEEStaticClass ascBREW[] =
      {
#ifndef FEATURE_MANGO_BREW
         {AEECLSID_TEXTCTL,         ASCF_UPGRADE,0, NULL, TextCtl_New},
         {AEECLSID_TEXTCTL_10,      ASCF_UPGRADE,0, NULL, TextCtl_New},
#endif

#if defined(AEEDB_LIB)
         {AEECLSID_DBMGR,           ASCF_UPGRADE,PL_FILE, NULL, AEEDBMgr_New},
#endif

#if defined(FEATURE_ADDRBOOK)
         {AEECLSID_ADDRBOOK,        ASCF_UPGRADE,PL_ADDRBOOK, NULL, AEEAddrBook_New},
         {AEECLSID_ADDRBOOK_21,     ASCF_UPGRADE,PL_ADDRBOOK, NULL, AEEAddrBook_New},
         {AEECLSID_ADDRBOOK_20,     ASCF_UPGRADE,PL_ADDRBOOK, NULL, AEEAddrBook_New},
         {AEECLSID_OEMADDRBOOK,     ASCF_UPGRADE,PL_SYSTEM,   NULL, OEMAddrBook_New},
#endif

#if defined(FEATURE_ADDRBOOK_RUIM)
         {AEECLSID_OEMRUIMADDRBOOK, ASCF_UPGRADE,PL_SYSTEM,   NULL, OEMRUIMAddrBook_New},
         {AEECLSID_ADDRBOOK_RUIM,   ASCF_UPGRADE,PL_ADDRBOOK, NULL, AEEAddrBook_New},
#endif

#if defined(FEATURE_IRUIM)
         {AEECLSID_RUIM,            ASCF_UPGRADE,PL_ADDRBOOK, NULL, OEMRUIM_New},
#endif

#if defined(FEATURE_CALLHISTORY)
         /*OEMCallHistory_Init added to only one entry, adding it to both entries will cause OEMCallHistory_Init to be called twice on AEE_Init*/
         {AEECLSID_CALLHISTORY_30,	 (ASCF_PRIV | ASCF_UPGRADE),PL_FILE,NULL,OEMCallHistory_New},
         {AEECLSID_CALLHISTORY,		 (ASCF_PRIV | ASCF_UPGRADE),PL_FILE,OEMCallHistory_Init,OEMCallHistory_New},
#endif

#if defined(FEATURE_BREW_SOUND)
         {AEECLSID_SOUND,           ASCF_UPGRADE,0,Sound_Init,Sound_New},
         {AEECLSID_SOUND_10,        ASCF_UPGRADE,0,Sound_Init,Sound_New},
#endif

#if defined(FEATURE_SOUNDPLAYER)
         {AEECLSID_SOUNDPLAYER,     ASCF_UPGRADE,0,NULL,AEESoundPlayer_New},
#endif

#if defined(FEATURE_ENCODE)
         {AEECLSID_ENCODING_UTF8,          ASCF_UPGRADE, 0, NULL, AEEEncoding_New},
         {AEECLSID_ENCODING_UTF16,         ASCF_UPGRADE, 0, NULL, AEEEncoding_New},
         {AEECLSID_ENCODING_ISOLATIN1,     ASCF_UPGRADE, 0, NULL, AEEEncoding_New},
         {AEECLSID_ENCODE_UTF16TOUTF8,     ASCF_UPGRADE, 0, NULL, AEEEncoding_New},
         {AEECLSID_ENCODE_ISOLATIN1TOUTF8, ASCF_UPGRADE, 0, NULL, AEEEncoding_New},
#endif

#if defined(OEMRINGERS)
         {AEECLSID_RINGERMGR,       ASCF_UPGRADE,0,NULL,Ringer_New},
#endif

#if defined(FEATURE_GIF_DECODE)
         {AEECLSID_GIF,             ASCF_UPGRADE,0,GIFViewer_Init,GIFViewer_New},
#endif
#if defined(FEATURE_UNZIP)
         {AEECLSID_UNZIPSTREAM,     ASCF_UPGRADE,0,NULL,AEEUnzip_New},
#endif
#if defined(FEATURE_BREW_SECURITY)
         {AEECLSID_RANDOM,          ASCF_UPGRADE,0,NULL,AEERandomSource_New},
#endif
#if defined(FEATURE_BREW_RSA)
         {AEECLSID_RSA,             ASCF_UPGRADE,0,NULL,AEERSA_New},
         {AEECLSID_RSA_BEST,        ASCF_UPGRADE,0,NULL,AEERSA_New},
#endif
#if defined(FEATURE_BREW_DES3DES)
         {AEECLSID_DES_ENCRYPT,     ASCF_UPGRADE,0,NULL,AEEDES3DES_New},
         {AEECLSID_DES_DECRYPT,     ASCF_UPGRADE,0,NULL,AEEDES3DES_New},
         {AEECLSID_3DES_ENCRYPT,    ASCF_UPGRADE,0,NULL,AEEDES3DES_New},
         {AEECLSID_3DES_DECRYPT,    ASCF_UPGRADE,0,NULL,AEEDES3DES_New},
#endif
#if defined(FEATURE_BREW_SEED)
         {AEECLSID_SEED_ENCRYPT,     ASCF_UPGRADE,0,NULL,AEESEED_New},
         {AEECLSID_SEED_DECRYPT,     ASCF_UPGRADE,0,NULL,AEESEED_New},
#endif
#if defined(FEATURE_POSITION_DETERMINATION)
         {AEECLSID_POSDET_11,       ASCF_UPGRADE,0,NULL,AEEPosDet_New},
         {AEECLSID_POSDET_20,       ASCF_UPGRADE,0,NULL,AEEPosDet_New},
         {AEECLSID_POSDET_302,      ASCF_UPGRADE,0,NULL,AEEPosDet_New},
         {AEECLSID_POSDET_312,      ASCF_UPGRADE,0,NULL,AEEPosDet_New},
         {AEECLSID_POSDET,          ASCF_UPGRADE,0,NULL,AEEPosDet_New},
#endif
#if (defined(FEATURE_IVOCODER) && !defined(AEE_SIMULATOR))
         {AEECLSID_VOCODER,         ASCF_PRIV | ASCF_UPGRADE,0,NULL,AEEVocoder_New},
#endif
#if defined(FEATURE_BREW_TAPI)
         {AEECLSID_TAPI,            ASCF_UPGRADE,           PL_TAPI,    NULL, AEETAPI_New},

#if !defined(AEE_SIMULATOR)
         {AEECLSID_PHONE,           ASCF_UPGRADE,PL_TAPI,NULL,AEEPhone_New},
#endif
#endif

#if defined(FEATURE_BREW_RAMCACHE)
         {AEECLSID_RAMCACHE,           ASCF_UPGRADE,0,NULL,IRamCache_New},
#endif /* #if defined(FEATURE_BREW_RAMCACHE) */

#if defined(FEATURE_BREW_SPRITE)
         {AEECLSID_SPRITE,             ASCF_UPGRADE,0,NULL,Sprite_New},
#endif /* #if defined(FEATURE_BREW_SPRITE) */
#if defined(FEATURE_LOGGER_FILE)
         {AEECLSID_LOGGER_FILE,        ASCF_UPGRADE,0,NULL,OEMLoggerFile_New},
#endif
#if defined(FEATURE_LOGGER_SERIAL)
         {AEECLSID_LOGGER_SERIAL,      ASCF_UPGRADE,0,NULL,OEMLoggerSerial_New},
#endif
#if defined(FEATURE_LOGGER_DMSS)
         {AEECLSID_LOGGER_DMSS,        ASCF_UPGRADE,0,NULL,OEMLoggerDMSS_New},
#endif
#if defined(FEATURE_LOGGER_WIN)
         {AEECLSID_LOGGER_WIN,         ASCF_UPGRADE,0,NULL,OEMLoggerWin_New},
#endif
#if defined(FEATURE_LOGGER_BTIL)
   {AEECLSID_LOGGER_BTIL,        ASCF_UPGRADE,0,NULL,OEMLoggerBTIL_New},
#endif
#if defined(FEATURE_BREW_CAMERA)
         {AEECLSID_CAMERA,             (ASCF_PRIV | ASCF_UPGRADE),0,AEECamera_Init,AEECamera_New},
#ifndef FEATURE_DSP
#if defined(FEATURE_CAMERA_MULTI_SENSOR)
         {AEECLSID_CAMERA2,            (ASCF_PRIV | ASCF_UPGRADE),0,AEECamera_Init,AEECamera_New},
#endif
#endif
#endif
#if defined(FEATURE_BREW_SIO)
         {AEECLSID_SERIAL,          (ASCF_PRIV | ASCF_UPGRADE),0,AEESIO_Init,ISerialPort_New},
#endif
#if defined(FEATURE_BREW_USBDRIVER)
         {AEECLSID_USBDRIVER,          (ASCF_PRIV | ASCF_UPGRADE), 0, OEMUSBDriver_Init, OEMUSBDriver_New},
#endif
#if defined(FEATURE_BREW_APPFRAME)
         {AEECLSID_APPFRAME1,          ASCF_UPGRADE,PL_SYSTEM,NULL,OEMAppFrame_New},
         {AEECLSID_APPFRAME2,          ASCF_UPGRADE,PL_SYSTEM,NULL,OEMAppFrame_New},
         {AEECLSID_APPFRAME3,          ASCF_UPGRADE,PL_SYSTEM,NULL,OEMAppFrame_New},
         {AEECLSID_APPFRAME4,          ASCF_UPGRADE,PL_SYSTEM,NULL,OEMAppFrame_New},
#endif

#if defined(FEATURE_BREW_ANNUNCIATOR_CONTROL)
         {AEECLSID_AnnunciatorControl_Display1, ASCF_UPGRADE,0,NULL,OEMAnnunciatorControl_New},
         {AEECLSID_AnnunciatorControl_Display2, ASCF_UPGRADE,0,NULL,OEMAnnunciatorControl_New},
         {AEECLSID_AnnunciatorControl_Display3, ASCF_UPGRADE,0,NULL,OEMAnnunciatorControl_New},
         {AEECLSID_AnnunciatorControl_Display4, ASCF_UPGRADE,0,NULL,OEMAnnunciatorControl_New}, 
#endif

#if defined(FEATURE_GSM1x)
         {AEECLSID_IGSM1XSIG,              ASCF_UPGRADE,0        ,OEMGSM1xSig_Init,            OEMGSM1xSig_New},
         {AEECLSID_IGSM1XSIGNOTIFIER,      ASCF_UPGRADE,PL_SYSTEM,OEMGSM1xSigNotifier_Init,    OEMGSM1xSigNotifier_New},
         {AEECLSID_IGSM1XCONTROL,          ASCF_UPGRADE,PL_SYSTEM,OEMGSM1xControl_Init,        OEMGSM1xControl_New},
         {AEECLSID_IGSM1XCONTROLNOTIFIER,  ASCF_UPGRADE,PL_SYSTEM,OEMGSM1xControlNotifier_Init,OEMGSM1xControlNotifier_New},
         {AEECLSID_IGSMSMS,                ASCF_UPGRADE,PL_SYSTEM,OEMGSMSMS_Init,              OEMGSMSMS_New},
         {AEECLSID_IGSMSMSNOTIFIER,        ASCF_UPGRADE,PL_SYSTEM,OEMGSMSMSNotifier_Init,      OEMGSMSMSNotifier_New},
#endif /* FEATURE_GSM1x */

#if defined(FEATURE_BREW_KEYMAKER)
         {AEECLSID_IKEYMAKER,               ASCF_UPGRADE,0         , NULL, AEEKeyMaker_New},
#endif /* FEATURE_BREW_KEYMAKER */

#if defined(FEATURE_BREW_ROOTCERTS)
         {AEECLSID_BREW_ROOTCERT,          ASCF_UPGRADE,0       , NULL, BrewRoots_New},
         {AEECLSID_TS_ROOTCERT,            ASCF_UPGRADE,0       , NULL, BrewRoots_New},
#endif

         /* BREW DEBUGGER */
#if defined(FEATURE_BREW_DEBUGGER)
         {AEECLSID_DEBUGGER,        ASCF_PRIV,0,NULL,OEMDEBUGGER_New},
#endif

#if defined(FEATURE_BREW_SMS)
         {AEECLSID_SMSNOTIFIER,     ASCF_UPGRADE,PL_TAPI,NULL,ISMSNotifier_New},
         {AEECLSID_SMS,             ASCF_UPGRADE,PL_TAPI,NULL,ISMS_New},
         {AEECLSID_SMSMSG,          ASCF_UPGRADE,PL_TAPI,NULL,ISMSMsg_New},
         {AEECLSID_SMSSTORAGE,      ASCF_UPGRADE,PL_TAPI,NULL,ISMSStorage_New},
         {AEECLSID_SMSBCSRVOPTS,    ASCF_UPGRADE,PL_TAPI,NULL,ISMSBCSrvOpts_New},
         {AEECLSID_SMSBCCONFIG,     ASCF_UPGRADE,PL_TAPI,NULL,ISMSBCConfig_New},
#endif

#if defined(FEATURE_BREW_BATTERY)
         {AEECLSID_BATTERYNOTIFIER,     ASCF_UPGRADE,0,NULL,IBatteryNotifier_New},
         {AEECLSID_BATTERY,             ASCF_UPGRADE,0,NULL,IBattery_New},
#endif // FEATURE_BREW_BATTERY

         {AEECLSID_RESARBITER,      ASCF_UPGRADE, 0,NULL,OEMResArbiter_New},
#if defined(FEATURE_TOPVISIBLE)
         {AEECLSID_TOPVISIBLECTL,   0,            0,NULL,AEETopVisibleCtl_New},
#endif

#ifdef FEATURE_BREW_IME // {
  //       {AEECLSID_OEMIMUI,                 0,0,NULL,OEMIMUI_New},
#endif // } FEATURE_BREW_IME
#if defined(FEATURE_BREW_IME) || defined(FEATURE_ZICORP_EZITEXT) // {
 //        {AEECLSID_OEMIMMGR,                 0,0,NULL,OEMIMMgr_New},
#endif // } FEATURE_BREW_IME || FEATURE_ZICORP_EZITEXT
         // Device Notifier
#if defined(FEATURE_DEV_NOTIFIER) // {
         {AEECLSID_DEVICENOTIFIER,  ASCF_UPGRADE,0,NULL,AEEDeviceNotifier_New},
#endif // } FEATURE_DEV_NOTIFIER
#if defined(FEATURE_BREW_NATIVENOTIFIER) // {
         {AEECLSID_NATIVENOTIFIER,  ASCF_UPGRADE,0,NULL,OEMNativeNotifierClass_New},
#endif // } FEATURE_BREW_NATIVENOTIFIER
#if defined(FEATURE_BREW_NETMTPDNOTIFIER) 
         {AEECLSID_NETMTPDNOTIFIER  ,0 ,PL_NETWORK, NULL, INetMTPDNotifier_New},
#endif // defined(FEATURE_BREW_NETMTPDNOTIFIER)
#if defined(FEATURE_BREW_MCAST)
         {AEECLSID_MCASTSESSION  ,0 ,PL_NETWORK, NULL, IMcastSession_New},
         {AEECLSID_BCMCSDATABASE ,0 ,0 ,NULL ,BcmcsDatabase_New},
#endif // defined(FEATURE_BREW_MCAST)
#if defined(FEATURE_BREW_MCAST2_SUPPORTED)
         {AEECLSID_MCASTSESSION2 ,0 ,PL_NETWORK, NULL, IMcastSession_New},
         {AEECLSID_MCASTBUNDLE   ,0 ,PL_NETWORK, NULL, IMcastBundle_New},
#endif // defined(FEATURE_BREW_MCAST2_SUPPORTED)
#if defined(FEATURE_BREW_QOS)
         {AEECLSID_QOSSPEC  ,0 ,PL_NETWORK, NULL, IQoSSpec_New},
         {AEECLSID_QOSLIST  ,0 ,PL_NETWORK, NULL, IQoSList_New},
         {AEECLSID_QOSSESSION  ,0 ,PL_NETWORK, NULL, IQoSSession_New},
         {AEECLSID_QOSFLOW  ,0 ,PL_NETWORK, NULL, IQoSFlow_New},
         {AEECLSID_QOSFILTER  ,0 ,PL_NETWORK, NULL, IQoSFilter_New},
         {AEECLSID_QOSBUNDLE  ,0 ,PL_NETWORK, NULL, IQoSBundle_New},
         {AEECLSID_PRIMARYQOSSESSION  ,0 ,PL_NETWORK, NULL, IPrimaryQoSSession_New},
#endif // defined(FEATURE_BREW_QOS)
#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_USES_LOWMEM)&&!defined(FEATURE_LOWER_MEM)
         {AEECLSID_NETWORK  ,0 ,PL_NETWORK, NULL, INetwork_New},
         {AEECLSID_NETUTILS ,0 ,PL_NETWORK, NULL, INetUtils_New},
         {AEECLSID_NETUTILS2 ,0 ,0, NULL, INetUtils2_New},
#endif         
#endif
#if defined(FEATURE_BACKLIGHT) // {
         {AEECLSID_BACKLIGHT, ASCF_UPGRADE, 0, NULL, AEEBacklight_New},
#endif // } FEATURE_BACKLIGHT

#if defined(FEATURE_BACKLIGHT_DISPLAY2) // {
         {AEECLSID_BACKLIGHT_DISPLAY2, ASCF_UPGRADE, 0, NULL, AEEBacklight_New},
#endif // } FEATURE_BACKLIGHT_DISPLAY2

#if defined(FEATURE_BACKLIGHT_DISPLAY3) // {
         {AEECLSID_BACKLIGHT_DISPLAY3, ASCF_UPGRADE, 0, NULL, AEEBacklight_New},
#endif // } FEATURE_BACKLIGHT_DISPLAY3

#if defined(FEATURE_BACKLIGHT_DISPLAY4) // {
         {AEECLSID_BACKLIGHT_DISPLAY4, ASCF_UPGRADE, 0, NULL, AEEBacklight_New},
#endif // } FEATURE_BACKLIGHT_DISPLAY4

#if defined(FEATURE_BACKLIGHT_KEYPAD) // {
         {AEECLSID_BACKLIGHT_KEYPAD, ASCF_UPGRADE, 0, NULL, AEEBacklight_New},
#endif // } FEATURE_BACKLIGHT_KEYPAD

#if defined(FEATURE_IFOTA)
         {AEECLSID_FOTA, 0, PL_SYSTEM, NULL, OEMFOTA_New},
#endif
#if defined(FEATURE_A1)
         {AEECLSID_A1MODENV, ASCF_PRIV|ASCF_UPGRADE, 0, NULL, A1ModEnv_CreateInstance},
#endif // FEATURE_A1
#ifndef CUST_EDITION//FEATURE_VERSION_W208S
         {AEECLSID_ALARMMGR, ASCF_UPGRADE, 0, NULL, AEEAlarmMgr_New},
#endif
#if (defined(FEATURE_IFLIP)|| (defined(AEE_SIMULATOR) && !defined(_AEEWIN)))
         {AEECLSID_FLIP,	ASCF_UPGRADE, 0, NULL, OEMFLIP_New}, 
#endif
#if (defined(FEATURE_BREW_JOYSTICK))
         {AEECLSID_IJOYSTICK, ASCF_UPGRADE, 0, NULL, AEEJoystick_New},
#endif
#ifndef FEATURE_MANGO_BREW
         {AEECLSID_MEMCACHE, 0, 0, NULL, OEMMemCache_New},
#endif
#if defined(FEATURE_BREW_ISYNTH)
         {AEECLSID_SYNTH, (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, ISynth_New},
#endif
#if defined(FEATURE_EXTENDED_KEYS)
#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_USES_LOWMEM)&&!defined(FEATURE_LOWER_MEM)
         {AEECLSID_KEYSMAPPING, ASCF_UPGRADE, 0, NULL, AEEKeysMapping_New},
#endif         
#endif
#endif
#if defined(FEATURE_FM)
         {AEECLSID_FMRadio, 0, 0, NULL, OEMFMRadio_New},
         {AEECLSID_FMRDS,0,0,NULL,OEMFMRDS_New},
#endif
#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_USES_LOWMEM)&&!defined(FEATURE_LOWER_MEM)
         {AEECLSID_ServingNetworks, (ASCF_UPGRADE), 0, NULL, CServingNetworks_New},
#if 0
         {AEECLSID_GPSOneLock, ASCF_PRIV, 0, NULL, AEEGPSOneConfig_New},
#endif
         {AEECLSID_MannerMode, ASCF_PRIV, 0, NULL, AEEMannerModeStore_New},
#endif         
#endif
         {0,0,0,NULL,NULL}
      };

   static const AEEStaticClass *aascBREW[] = {
      ascBREW,
      gascOEMDefaults,
#if defined(AEEMENU_LIB)
      gMenuClassList,
#endif
#if defined(AEECONTROLS_LIB)
      gControlClassList,
#endif
      gTelephoneClassList,
#if defined(FEATURE_BREW_FONTS)
      gAEEStockFontsClasses,
      gAEEStandardFontsClasses,
#endif
#if defined(FEATURE_BREW_MULTIMEDIA)
      gAEEMediaClasses,
#endif
#if defined(FEATURE_BREW_BITMAPDEV)
      gOEMDisplayDevClasses,
#endif
#if defined(FEATURE_3D)
      gAEE3DClasses,
      gAEE3DUtilClasses,
      gAEE3DModelClasses,
#endif
#if defined(FEATURE_HTMLVIEWER)
      gAEEHtmlviewerClasses,
#endif
#if defined(FEATURE_BREW_SCALE)
      gAEEScaleClassList,
#endif
#ifndef FEATURE_MANGO_BREW
      gAEEGenericViewerClasses,
      gAEEBmpClasses,
#if defined(FEATURE_JPEG_DECODER) 
      gAEEIJGClasses,
#endif
#endif
#if defined(FEATURE_BREW_PNG_DECODE)
      gAEEPNGClasses,
#endif
#if defined(FEATURE_BCI_DECODE)
      gAEEBCIClasses,
#endif
#if defined(FEATURE_BTIL)
      gAEEBTILClasses,
#endif
#if defined(FEATURE_BREW_SECURITY)
      gascICipherNew,
#if defined(FEATURE_BREW_DES3DES)
      gascICipherNewDES,
#endif
#if defined(FEATURE_BREW_SEED)
      gascICipherNewSEED,
#endif
      gascICipherWrapper,
      gascICipherFactory,
#endif
#ifndef FEATURE_MANGO_BREW
#if !defined(FEATURE_USES_LOWMEM)&&!defined(FEATURE_LOWER_MEM)
      gascIWebUtil,
#endif      
#endif
      gascIWebOpts,
#if defined(FEATURE_BREW_SECURITY)
      gascIHashCTX,
      gascIHash,
      gascICipher,
#endif /* if defined(FEATURE_BREW_SECURITY) */
#if defined(FEATURE_GRAPHICS)
      gascAEEGraphics,
#endif
#if defined(FEATURE_BREW_AEENET)
      gascNetClassList,
#endif
#if defined(FEATURE_BREW_AEETHREAD)
      gascThreadClassList,
#endif
#if defined(FEATURE_BREW_PROVISION)
      gascProvisionClassList,
#endif
#if defined(FEATURE_BREW_LICENSE)
      gascLicenseClassList,
#endif
#if defined(FEATURE_BREW_AEEAPPHISTORY)
      gascAppHistoryClassList,
#endif
#if defined(FEATURE_BREW_AEEAPPLETCTL)
      gascAppletCtlClassList,
#endif
#if defined(FEATURE_BREW_SIGNAL)
      gascSignalClassList,
#endif

      gAEESysClockClasses,
      gAEESecSysClockClasses,
      NULL
   };

   static const AEEStaticClass **aaasc[] =
      {
#if   defined(AEE_SIMULATOR)
         0, /* static extensions, to be filled in below
               calling AEESIM_GetStaticClassLists() */
#endif /* #if defined(AEE_SIMULATOR) */

         aascBREW,              /* supplied by OEMModTable.c, above */
#ifdef FEATURE_BREW_EXTENDED_MODTABLE
         gOEMStaticClassLists,  /* supplied by OEMModTableExt.c */
#endif
         0
      };

#ifdef AEE_SIMULATOR

         aaasc[0] = AEESIM_GetStaticClassLists();

#endif /* #if defined(AEE_SIMULATOR) */

   return aaasc;
}

#endif   // OEMMODTABLE

