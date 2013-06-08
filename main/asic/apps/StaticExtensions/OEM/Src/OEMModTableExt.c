/*======================================================
FILE:  OEMModTableExt.c

SERVICES:  Reference OEM specific Static Module/Class Tables.

GENERAL DESCRIPTION:
   This file contains the list of static modules and static classes
   that are specific to OEM platform. These get included as part of
   the static tables defined in OEMModTable.c.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright ?1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
//lint -e786 Suppress "String concatenation within initializer" info msg
#include "OEMFeatures.h"
#include "OEMModTableExt.h"
#include "AEEFile.h"
#ifdef CUST_EDITION
#include "OEMClassIDs.h"
#endif
//MTP MCS static Extension
#ifdef FEATURE_SME_DRMMS
#include "AEECLSID_MTPDRM.BID"
#endif


#ifdef FEATURE_ANNUNCIATOR
	#include "AEEAnnunciator.h"
#endif /* FEATURE_ANNUNCIATOR */


// SoundCtl Static Extension
#if (defined(FEATURE_AUDIO_EQUALIZER)||defined(FEATURE_AUDIO_QCONCERT)||defined(FEATURE_AUDIO_SPECTRUM_ANALYZER))
	#include "SoundCtl.bid"
#endif

#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
    #include "PICTBRIDGE.BID"
#endif //(defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))

// QAudioFX Static Extension
#ifdef FEATURE_QAUDIOFX
	#include "AEEMediaAudio.h"
	#include "MEDIAAUDIOENV.BID"
#endif

#ifdef FEATURE_GPSONE_XTRA_BREW_EXT
#error code not present
#endif

#if defined(FEATURE_MFLO)
#error code not present
#endif

//#ifdef FEATURE_I3D
//#include "OEM3D_Demos.h"
//#endif
#ifdef CUST_EDITION
    #undef FEATURE_WMS_APP
    #undef FEATURE_IWMSDIAG
#endif
#ifdef FEATURE_NO_WMS_APP
	#undef FEATURE_WMS_APP
#endif

//#ifdef FEATURE_WMS_APP
	#include "AEEWMS.h"
//#endif

#ifdef FEATURE_BREWAPPCOORD
	#include "BACProxyApp.bid"
	#include "AEEAltEnvShell.bid"
#endif

#ifdef FEATURE_IWMSDIAG
	#include "AEEWMSDIAG.h"
#endif

#ifdef FEATURE_APP_WAP
	#include "AEESEC.BID"
#endif

#if defined (FEATURE_APP_CAT)
	#include "AEECAT.h"
	#include "AEESTK.h"
#endif

#if defined (FEATURE_IVR)
	#include "AEEVR.h"
#endif

#if defined(FEATURE_BREW_IMOVIEFILE)
	#include "AEEMovieFile.h"
#endif

#if defined(FEATURE_BREW_IMOVIEEDIT)
	#include "AEEMovieEdit.h"
#endif

#if defined (FEATURE_IMIC)
	#include "AEEMIC.h"
#endif

#if defined(FEATURE_ICARD)
	#include "AEECard.h"
#endif

#if defined(FEATURE_ICARDSESSION)
	#include "AEECardSession.h"
	#include "AEECardSessionNotifier.h"
	#include "AEECARDSESSION.bid"
	#include "AEECARDSESSION_SLOT2.BID"
	#include "AEECARDSESSION_NOTIFIER.BID"
	#include "AEECARDSESSION_NOTIFIER_SLOT2.BID"
#endif

#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_ICONTENTTEST
#include "IContentTest.bid"
#endif

#ifdef FEATURE_CONTENTEVTMGRTEST
#include "CONTENTEVTMGRTEST.BID"
#endif

#ifdef FEATURE_ICONTENTHANDLERTEST
#include "IContentHandlerTest.bid"
#endif

#ifdef FEATURE_IIPL
	#include "IPL.BID"
	#include "AEEBitmapFX.bid"
	#include "AEEIBitmapFX.h"
#endif

#if defined(FEATURE_ICM)
#include "AEECM.h"
#ifdef FEATURE_ANNUNCIATOR
	#include "AEEAnnunciator.h"
#endif /* FEATURE_ANNUNCIATOR */

#endif /* FEATURE_ICM */
#include "AEEAlert.h"

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

#if defined (FEATURE_IPOSDETEXT)
	#include "AEEPosDetExt.h"
#endif

#if defined(FEATURE_GRAPHICS_APP_OPENGLES_SAMPLES)
#error code not present
#endif

#ifdef FEATURE_IBT
	#include "OEMModTableExt_BT.h"
#endif

#if defined(FEATURE_IYCBCR)
	#include "IYCbCrFactory.h"
#endif

#ifdef FEATURE_JAVA
#error code not present
#endif

#if defined(FEATURE_MMGPS)
#error code not present
#endif

#ifdef FEATURE_SEC_DRM
#error code not present
#endif

#ifdef FEATURE_SEC_IPSEC
	#include "IIPSecSP.h"
	#include "IIPSecSPDB.h"
	#include "IIPSecSA.h"
	#include "IIPSecSADB.h"
#endif

#ifdef FEATURE_ICONTENT
#include "AEECLSID_CONTENTBROWSER.BID"
#include "AEECLSID_CONTENTMGR.BID"
#include "AEECLSID_CATEGORYMGR.BID"
#include "AEECLSID_CONTENTNOTIFIER.BID"

#include "isqlmgr.bid"
#endif

#ifdef FEATURE_ICONTENTHANDLER
#include "AEECLSID_CONTENTHANDLER.BID"
#include "AEECLSID_CONTENTMETAINFOFACTORY.BID"
#include "AEECLSID_CONTENTOMADRMMSG.BID"
#include "AEECLSID_CONTENTEVENTMGR.BID"
#endif

#if defined (FEATURE_STA_RAT)
#error code not present
#endif

#if defined (FEATURE_QVPHONE)
	#include "AEEVideoPhone.h"
#endif /* FEATURE_QVPHONE */

#if defined FEATURE_BREW_NATIVENOTIFIER
	#include "NativeNotifier.bid"
#endif

#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER)
#include "AEEQConfig.h"
#if defined(FEATURE_SECMSM_APPS)
#include "AEEContent.h"
#include "AEEContentDBModel.h"
#include "AEEICategory.h"
#endif /* FEATURE_SECMSM_APPS */
#include "AEESQL.h"
#include "IThemeMgr.h"
#endif //(FEATURE_SECMSM_APPS) || (FEATURE_ODM_BROWSER)

#if defined(FEATURE_ODM_BROWSER)
#include "BROWSER_MODEL.bid"
#endif

// QoS Static Extension
#if defined (FEATURE_IQOSSESSIONEX)
	#include "AEEQoSSessionEx.h"
	#include "AEEQoSBundleEx.h"
#endif

#ifdef FEATURE_SECSERVICES_TESTAPP
	#include "securityservices.bid"
#endif
#ifdef FEATURE_STATIC_MTP
#include "mtph.bid"
#include "mcs.bid"
#endif /* FEATURE_STATIC_MTP */
#ifdef FEATURE_SUPPORT_WAP_APP
#include	"nf3static.h"
#endif

// HDR Static Extension
#if defined (FEATURE_IHDR)
	#include "AEEHDR.h"
#endif
#ifdef FEATURE_ANALOG_TV
#include "IMMITv.h"
#endif
// RDM Static Extension
#if defined (FEATURE_IRDM)
#error code not present
#endif
#ifndef CUST_EDITION
// Address Book Extension
#if defined(FEATURE_ADDRBOOK)
	#include "AddrBookExt.bid"
	extern int OEMAddrBookExt_New(IShell *ps,AEECLSID cls,void ** ppo);
#endif
#endif
// CDMA/GSM/WCDMA tone generation
#ifdef FEATURE_UIONE_HDK
	#include "AEESignalTone_priv.h"
#endif // FEATURE_UIONE_HDK

#if defined(FEATURE_AUDFMT_HYBRID_MODE)
	#include "AEECLSID_SYNTHQUALITY.BID"
#endif

#if defined(FEATURE_I3D)
	extern const AEEStaticClass gAEE3DClasses[];
#endif

#if defined(FEATURE_GRAPHICS_OPENGLES_CL)
#error code not present
#endif

#if defined(FEATURE_GRAPHICS_OPENVG)
#error code not present
#endif
#ifndef WIN32
#if defined(FEATURE_QCT_JPEG_DECODER)
extern const AEEStaticClass gAEEJpegClasses[];
#endif
#if defined FEATURE_SUPPORT_BT_APP
extern int tBtuiApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#endif

#if defined FEATURE_SUPPORT_BUIW
extern int FormsMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int WidgetMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

#ifdef	 FEATURE_APP_BLUETOOTH
//extern int BTApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int BTAppMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

//#if defined FEATURE_SUPPORT_WAP_APP
#if 0//def 0
extern int EditApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int EditExt_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int OEMExtFor3Part_Load(IShell *ps, void * pHelpers, IModule ** pMod);

extern int AEEBrwAppMod_Load(IShell *pIShell, void *ph, IModule **ppMod);					
extern int AEEBrwMod_Load(IShell *pIShell, void *ph, IModule **ppMod);                                           
extern int AEEDrmMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
extern int AEESecMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
extern int AEEWapMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
extern int AEEWecscMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
extern int AEEPushMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
#endif

#if defined(FEATURE_IYCBCR)
	extern int IYCbCrFactory_New(IShell *pIShell, AEECLSID cls,void **ppo);
#endif

#if defined (FEATURE_PHONE_VR)
	extern int OEMVR_New(IShell *pIShell, AEECLSID cls,void **ppo);
#endif

#ifdef FEATURE_FLEXI_STATIC_BREW_APP

#ifdef FEATURE_NASRANI
extern int NASRANI_Load(IShell *ps, void * pHelpers, IModule **pMod);
#endif

#ifdef FEATURE_FMN2010 
    extern int Fmn2010_Load(IShell *ps, void * pHelpers, IModule **pMod);
#endif

#ifdef FEATURE_FPT005
extern int Fpt005_Load(IShell *ps, void * pHelpers, IModule **pMod);
#endif

#ifdef FEATURE_CAH006
extern int Cah006_Load(IShell *ps, void * pHelpers, IModule **pMod);
#endif

#ifdef FEATURE_GURU
extern int GURU_Load(IShell *ps, void * pHelpers,  IModule **pMod);
#endif
#endif  /*FEATURE_FLEXI_STATIC_BREW_APP*/

#ifdef FEATURE_VERSION_FLEXI203P
extern int ODPMORA_Load(IShell *ps, void * pHelpers,  IModule **pMod);
extern int FlexiMusik_Load(IShell *ps, void * pHelpers,  IModule **pMod);
#endif

#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
extern int BSHOP_Load(IShell *ps, void * pHelpers, IModule **pMod);
extern int TWITTER_Load(IShell *ps, void * pHelpers, IModule **pMod);
extern int FACEBOOK_Load(IShell *ps, void * pHelpers, IModule **pMod);
extern int msf_Load(IShell *ps, void * pHelpers, IModule **pMod);
extern int ODPMORA_Load(IShell *ps, void * pHelpers,  IModule **pMod);
extern int SmartMusik_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int Tombapp_Load(IShell *pIShell, void *pHelpers, IModule **ppMod);
//extern int MOKA_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_STATIC_APP_J9)
	extern int J9Mod_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);
#endif
#if defined(FEATURE_STATIC_APP_JEODE)
	extern int JeodeK_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);
#endif

#if defined (FEATURE_QCT_JPEG_DECODER)
	extern void JPEGViewer_Init(IShell * ps);
	extern int  JPEGViewer_New (IShell * pIShell, AEECLSID ClsId, void ** ppObj);
#endif

#if defined(FEATURE_MFLO)
#error code not present
#endif

#if defined(FEATURE_OVERLAY)
extern const AEEStaticClass gOEMOverlayClasses[];
#endif

// Annunciator
#if defined (FEATURE_ANNUNCIATOR)
	extern int OEMAnnunciator_New (IShell *piShell, AEECLSID cls, void **pp);
#endif /* FEATURE_ANNUNCIATOR */

// IMIC
#if defined (FEATURE_IMIC)
	extern int AEEMIC_New(IShell *ps, AEECLSID cls, void **ppif);
#endif

#if defined (FEATURE_UIONE_HDK)
	extern int TestAppsMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// CAT
#if defined (FEATURE_APP_CAT)
	extern int OEMCAT_New(IShell *pIShell, AEECLSID cls, void **ppif);
	extern int AEESTK_New(IShell *pIShell, AEECLSID cls, void **ppif);
	extern int AEESTKNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

////WMSAPP
//#ifdef FEATURE_WMS_APP
	extern int OEMWMS_New (IShell *piShell, AEECLSID cls, void **pp);
//#endif

#ifdef FEATURE_ANALOG_TV
	extern int CTVAppMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif
#ifdef FEATURE_WAPBROWSER_APP
#endif
#ifdef FEATURE_IWMSDIAG
	extern int AEEWMSDIAG_New (IShell *piShell, AEECLSID cls, void **pp);
	extern int AEEWMSDIAGNotifier_New (IShell *piShell, AEECLSID cls, void **pp);
#endif

// ICM
#if defined(FEATURE_ICM)
extern int OEMCM_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMCMNotifier_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMCallOpts_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMCMModel_New (IShell *pIShell, AEECLSID cls, void **ppif);
//IBatt
#if (!defined(FEATURE_UIONE_HDK) )
	extern int AEEBatt_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif /* FEATURE_UIONE_HDK */
#else /* !FEATURE_ICM */
#if defined(FEATURE_MANGO_UI)
   extern int AEEBatt_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif
#endif /* FEATURE_ICM */

#if !defined(FEATURE_MANGO_UI)
	extern int OEMALERT_New (IShell *pIShell, AEECLSID cls, void **ppif);
	extern int OEMALERT_NOTIFIER_New (IShell *pIShell, AEECLSID cls, void **ppif);
#endif


#if defined(FEATURE_UIALARM) // {
extern int AEEAlarm_New(IShell *ps, AEECLSID cls, void **ppif);
#endif // } FEATURE_UIALARM
// CARD
#if defined(FEATURE_ICARD)
	extern int OEMCard_New (IShell *pIShell, AEECLSID cls, void **ppif);
	extern int OEMCardNotifier_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

// ICARD's IMODELsupport
#if defined(FEATURE_ICARD_IMODEL)
	extern int OEMCardModel_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_ICARDSESSION)
	extern int OEMCardSession_New (IShell *pIShell, AEECLSID cls, void **ppif);
	extern int OEMCardSessionNotifier_New (IShell *pIShell, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

// Native Notifier interface
#if defined FEATURE_BREW_NATIVENOTIFIER
	extern int OEMNativeNotifierClass_New(IShell * pIShell, AEECLSID cls, void **ppobj);
#endif

#ifdef FEATURE_MMGPS
#error code not present
#endif

#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER)
extern const AEEStaticMod gODMAppStaticMods[];
// header to declare vars included below by including ODMAppsMods.c
#include "ODMAppsMods.h"
#endif //FEATURE_SECMSM_APPS) || (FEATURE_ODM_BROWSER)

#ifdef FEATURE_SEC_DRM
#error code not present
#endif

#ifdef FEATURE_SEC_IPSEC
	int AEEIPSecSP_New( IShell* pIShell, AEECLSID  cls, void** ppif );
	int AEEIPSecSPDB_New( IShell*   pIShell, AEECLSID  cls, void**    ppif);
	int AEEIPSecSA_New( IShell* pIShell, AEECLSID  cls, void** ppif );
	int AEEIPSecSADB_New( IShell* pIShell, AEECLSID  cls, void** ppif );
        extern int SECIPSSP_New( IShell *pShell, AEECLSID cls, void **ppif );
        extern int SECIPSSPDB_New( IShell *pShell, AEECLSID cls, void **ppif );
#endif

#if defined(FEATURE_ENABLE_SVG_SVGDOM)
extern void IMediaSVG_Init(IShell * ps);
extern int  IMediaSVG_New(IShell * ps, AEECLSID cls, void **ppif);
#endif //defined(FEATURE_ENABLE_SVG_SVGDOM)

#if defined (FEATURE_IPOSDETEXT)
	extern int OEMPosDetExt_New(IShell *piShell, AEECLSID cls,void **pp);
#endif

#ifdef FEATURE_ICONTENT
extern int     AEEContentBrowser_New (IShell * pIShell, AEECLSID cls, void **ppif) ;
extern int     AEEContentMgr_New (IShell * pIShell, AEECLSID cls, void **ppif) ;
extern int     AEECategoryMgr_New (IShell * pIShell, AEECLSID cls, void **ppo) ;
extern int     AEEContentNotifier_New (IShell * pIShell, AEECLSID cls, void **ppif) ;
extern int     AEESQLMgr_New (IShell * pIShell, AEECLSID cls, void **ppif) ;
#endif

#ifdef FEATURE_ICONTENTHANDLER
extern int AEEContentHandler_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int AEEContentMetaInfoFactory_New (IShell *piShell, AEECLSID cls, void **ppif);
extern int AEEContentOmaDrmMsg_New (IShell *piShell, AEECLSID cls, void **ppif);
extern int  AEEContentEventMgr_New (IShell *piShell, AEECLSID cls, void **ppif);
#endif


// OEMDiag
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif

#ifdef FEATURE_IIPL
	extern int OEMIPL_New(IShell *ps, AEECLSID cls, void **ppif);
	extern int OEMBitmapFX_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

  //VideoPhone
#if defined(FEATURE_QVPHONE)
	extern int OEMVideoPhone_New(IShell *pIShell, AEECLSID cls, void **pp);
	extern int OEMVideoPhone_NOTIFIER_New(IShell *pIShell, AEECLSID cls, void **pp);
#endif /* FEATURE_QVPHONE */

// MOVIEFILE
#if defined(FEATURE_BREW_IMOVIEFILE)
	extern int     IMovieFile_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_BREW_IMOVIEEDIT)
	extern int     IMovieEdit_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#ifdef FEATURE_TSG_EXT
	//Required 01023c45 Interface - AEEAPPLOG
	extern int AEE01023c45_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
//Required 01013fd6 Interface
extern int AEE01013fd6_New (IShell * pIShell, AEECLSID clsid, void ** ppif);

#ifdef FEATURE_STATIC_RAPTOR
extern int BnmRm_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
extern PFNMODENTRY BNMApp_GetModInfo(IShell *ps, AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv);
#endif /* FEATURE_STATIC_RAPTOR */
#endif

// Openwave Browser App
#if defined(FEATURE_ODM_BROWSER)
extern int BrowserModel_New(IShell *pIShell, AEECLSID cls, void **ppObj);
#endif //FEATURE_ODM_BROWSER

#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER)
#if defined(FEATURE_ICONTENT)
extern int     ContentDBModel_New(IShell *pIShell, AEECLSID cls, void **ppObj);
#endif // FEATURE_ICONTENT
extern int     OEMConfig_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);
extern int     IThemeMgr_New(IShell *piShell, AEECLSID cls, void **ppif);
extern int     OEMGenericViewer_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif //FEATURE_SECMSM_APPS) || FEATURE_ODM_BROWSER

//RDM 
#if defined(FEATURE_IRDM)
#error code not present
#endif //FEATURE_IRDM

#if defined(FEATURE_ANICTL)
extern int AniCtl_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
#endif

// UI App
#if defined(FEATURE_APP_UI) && !defined(FEATURE_MANGO_UI)
extern int CoreApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#ifndef USES_MMI
extern int CoreStartApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
#endif
#ifdef FEATURE_ANALOG_TV
extern int CMMITv_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
extern int OEMTLGAtv_New(IShell *pIShell,AEECLSID ClsID,OEMINSTANCE* ppInterface);
#endif
#ifdef FEATURE_APP_QUICKTEST
extern int QuickTest_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif

#ifdef FEATURE_APP_MIZONE
extern int MiZone_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif

extern int CWMSMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
extern int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);
extern int ExtraMenuMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#ifdef FEATURE_APP_STOPWATCH
extern int StopWatchMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
extern int ClockAppsMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#ifdef FEATURE_APP_CALC
extern int CalcMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#ifdef FEATURE_APP_APPLICATION
extern int ApplicationMod_Load( IShell *pIShell, void *ph, IModule **ppMod);
#endif

#ifdef FEATURE_APP_WORLDTIME
extern int WorldTimeMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif /*FEATURE_APP_WORLDTIME*/

#ifdef FEATURE_APP_SCHEDULE
extern int ScheduleApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#ifdef FEATURE_SHORT_CODE_NAM_COUNT
extern int OtkMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif //FEATURE_SHORT_CODE_NAM_COUNT

//#if defined( FEATURE_GAME_TETRIS)
//#endif
#if defined( FEATURE_GAME_BRICKATTACK)
extern int Brick_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#if defined( FEATURE_GAME_BLACKJACK)
extern int BlackJack_Load(IShell *ps, void *pHelpers, IModule **pMod);
#if defined(FEATURE_VERSION_X3)
//extern int FiveBall_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int PinTu_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

#if defined (FEATURE_VERSION_ESIA)
extern int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

#if defined(FEATURE_VERSION_C316)
//extern int GmFive_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

//extern int GmFive_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#if defined( FEATURE_JEWISH_CALENDAR)
extern int JewishCalendar_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
extern int LKV_Load(IShell *ps, void *pHelpers, IModule **pMod);
#if defined(FEATURE_RECORDER)
extern int Recorder_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Contact App
#if defined(FEATURE_CONTACT_APP)
extern int ContApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
// Brew App Coordination. Proxy App to represent App from Alternate Env.
// and AltEnvShell Extension for BREW App developers who want to deal with
// Alternate Environment applications (start/stop/postevent)
#if defined (FEATURE_BREWAPPCOORD)
extern int BACProxyApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int AEEAltEnvShell_New(IShell *ps, AEECLSID cls, void** ppif);
#endif

// Contact App
#if defined(FEATURE_APP_CONTACT)
extern int ContactMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Alarm Clock App
#if defined(FEATURE_ALARMCLOCK_APP)
extern int AlarmClock_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined (FEATURE_ICONTENTTEST)
extern int IContenttestApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined (FEATURE_CONTENTEVTMGRTEST)
extern int IContentEvtMgrTestApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined (FEATURE_ICONTENTHANDLERTEST)
extern int IContentHandlerTestApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// NET App
#if defined (FEATURE_NETSETTINGS_APP)
extern int NetSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// AUXSettings App
#if defined (FEATURE_AUXSETTINGS_APP)
extern int AUXSettings_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// CAT
#if defined (FEATURE_APP_CAT)
extern int CCatApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Recent Calls App
//#if defined(FEATURE_APP_RECENTCALLS)
extern int RecentCalls_Load(IShell *ps, void * pHelpers, IModule ** pMod);
//#endif

// UTK App
#if defined(FEATURE_APP_UTK)
extern int UTK_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
// NumberManager App
#if defined(FEATURE_APP_NUMBERMANAGER)
extern int  NumberManagerMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
#endif
// Vector Interface
#if defined(FEATURE_IVECTOR) && !defined(FEATURE_MANGO_UI)
extern int IVectorMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// WMS App
#if defined(FEATURE_WMS_APP)
extern int  WMSAPP_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// QCAMERA App
#if defined(FEATURE_APP_QCAMERA) && !defined(FEATURE_MANGO_UI)
extern int QCam_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#if defined(FEATURE_APP_QALBUM)
extern int Qalbum_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif // FEATURE_APP_QALBUM
#endif

// QTCAM App
#if defined(FEATURE_APP_QTCAM)
extern int QTCam_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// QCAMCORDER App
#if defined(FEATURE_APP_QCAMCORDER) && !defined(FEATURE_MANGO_UI)
extern int Qcamcorder_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// QMDP App
#if defined(FEATURE_APP_QMDP)
#error code not present
#endif

// WAP Security
#if defined(FEATURE_APP_WAP)
extern int AEESEC_New(IShell * ps, AEECLSID cls, void **ppobj);
#endif


// MMS App/WAP App (TMT)
#if defined(FEATURE_APP_MMS) || defined(FEATURE_APP_WAP)
extern  int  TMT_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_SMS_LISTENER)
extern int  SMSListener_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// SHOW3D App
#if defined(FEATURE_APP_SHOW3D)
extern int Show3D_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// MOBIMON App
#if defined(FEATURE_APP_MOBIMON)
extern int MobiMon_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

 // PureVoice App
#if defined(FEATURE_APP_PUREVOICE)
extern int purevoice_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// GuniMON App
#if defined(FEATURE_APP_GUNIMON)
extern int GUniMon_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Sprite game
#if defined(FEATURE_APP_SPRITEGAME)
extern int spritegame_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// OpenGLES Applications
#if defined(FEATURE_GRAPHICS_APP_OPENGLES_SAMPLES)
#error code not present
#endif

// Gigamon
#if defined(FEATURE_GRAPHICS_APP_GIGAMON)
extern int gigamon_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Calmon
#if defined(FEATURE_GRAPHICS_APP_CALMON)
extern int calmon_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// UI3D App
#if defined(FEATURE_UI_3D_APP)
#error code not present
#endif

// Media Player App
#if defined(FEATURE_APP_MEDIAPLAYER)
extern int CMediaPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// GPS App
#if defined(FEATURE_GPSAPP)
#error code not present
#endif

// Dialer app
#if defined (FEATURE_APP_DIALER) && !defined(FEATURE_MANGO_UI)
    extern int CallAppMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
#endif

extern int SettingMenuMod_Load(IShell   *pIShell,  void  *ph, IModule **ppMod);
extern int SecurityMenuMod_Load(IShell   *pIShell, void  *ph,  IModule  **ppMod);
extern int SoundMenuMod_Load(IShell   *pIShell, void *ph, IModule **ppMod);
extern int DisplayMenuMod_Load(IShell   *pIShell, void *ph, IModule **ppMod);

#if defined( FEATURE_FM_RADIO)
extern int FmRadioMod_Load(IShell   *pIShell, void  *ph,  IModule  **ppMod);
#endif
// Svc Programming
#if defined (FEATURE_APP_SVCPRG)
extern int CSvcPrgMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Field Debug
#if defined (FEATURE_APP_FLDDBG)
extern int CFieldDebugMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined (FEATURE_SECSERVICES_TESTAPP)
extern int SecurityServices_Load (IShell *pIShell, void *pHelpers, IModule **ppIMod);
#endif

#if defined (FEATURE_STATIC_MTP)
extern int MCS_Load(IShell *pIShell, void *pHelpers, IModule **ppMod);
extern int MTPH_Load(IShell *pIShell, void *pHelpers, IModule **ppMod);
#endif

// Datastats
#ifdef FEATURE_APP_DATASTATS
extern int CDataStatsApp_Load(IShell   *ps, void     *pHelpers, IModule **pMod);
#endif

#ifdef FEATURE_QVPHONE_APP
#error code not present
#endif

#ifdef FEATURE_STATIC_RAPTOR

extern int BNMApp_Mod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#ifdef RAPTOR_UMTS_DDA
extern int RUDDA_Mod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif // RAPTOR_UMTS_DDA

#endif // FEATURE_STATIC_RAPTOR

// IJoystick interface
#if defined(FEATURE_BREW_JOYSTICK)
#include "AEEJoystick.h"
extern int     AEEJoystick_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#ifdef FEATURE_JAVA
#error code not present
#endif /* FEATURE_JAVA */

//MTP MCS static Extension
#if defined (FEATURE_SME_DRMMS)
extern int AEEMtpDrm_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

#ifdef FEATURE_IMS
#error code not present
#endif // FEATURE_IMS
#ifdef FEATURE_IMS_ISIP
  // IMS isip extension
  extern int AEESipMod_Load(IShell * pIShell, void * ph, IModule ** ppMod);
#endif

// Static SoundCtl Extension
#if (defined(FEATURE_AUDIO_EQUALIZER)||defined(FEATURE_AUDIO_QCONCERT)||defined(FEATURE_AUDIO_SPECTRUM_ANALYZER))
extern int OEMSoundCtl_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

// QAudioFX Static Extension
#ifdef FEATURE_QAUDIOFX
extern int IMediaAudioEnv_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

// QoS Static Extension
#if defined(FEATURE_IQOSSESSIONEX)
extern int IQoSSessionEx_New(IShell *ps, AEECLSID cls, void **pMod);
extern int IQoSBundleEx_New(IShell *ps, AEECLSID cls, void **pMod);
#endif

#if defined (FEATURE_IHDR)
extern int IHDR_New(IShell *ps, AEECLSID cls, void **pMod);
#endif

#if defined(FEATURE_AUDFMT_HYBRID_MODE)
extern int ISynthQuality_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
extern int OEMPB_New(IShell * ps, AEECLSID cls, void **ppObj);
#endif //(defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))

#ifdef FEATURE_APP_PBPRINT
#error code not present
#endif //FEATURE_APP_PBPRINT

// BREW Signal
#ifdef FEATURE_BREW_SIGNAL
extern const AEEStaticClass gascSignalClassList[];
#endif
#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER)
// build-generated list of modules
//  -- this is necessary only because adding another list in the gOEMStaticModLists
//     structure below does not work due to a bug in BREW where it only loads the
//     first list
#include "ODMAppsMods.c"
#endif //(FEATURE_SECMSM_APPS) || (FEATURE_ODM_BROWSER)

#ifdef FEATURE_SENSORS
#include "OEMModTableExt_Sensors.h"
#endif

#ifdef FEATURE_GPSONE_XTRA_BREW_EXT
#error code not present
#endif

// FM Radio Application
#if defined(FEATURE_FMRADIO_APP) && !defined(FEATURE_MANGO_UI)
extern int  FMRadioApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if  defined(FEATURE_MANGO_UI)
extern int  brewPort_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_CALLLIST)
extern int  OEMCallList_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_APP_MEDIAGALLERY)
extern int MediaGalleryApp_Load(IShell* ps, void* pHelpers, IModule** ppMod); 
#endif

#if defined(FEATURE_APP_CAMERA)
extern int  CCameraAppMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_APP_MUSICPLAYER)
extern int MusicPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_VIDEOPLAYER)
extern int  VideoPlayerMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_APP_TIMERAPP)
extern int AppTimerMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#if defined(FEATURE_SUPPORT_WAP_APP)
extern int CNetFront3_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

#if defined(FEATURE_NEMOBREW)
extern int nemobrew_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

#if defined(FEATURE_CONVERTER)
extern int  ConverterMod_Load(IShell* ps, void* pHelpers, IModule** ppMod);
#endif

#ifdef FEATURE_SPORTS_APP
//extern int SportsMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif

#ifdef FEATURE_MORSE
extern int MorseMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif

#ifdef FEATURE_CARRIER_CHINA_TELCOM
//extern int CustServiceMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
//extern int EsurfingMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
//extern int NotebookMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif /*FEATURE_CARRIER_CHINA_TELCOM*/
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 20090407 add
extern int AdjustPenMod_Load(IShell *p,void *ph,IModule **ppMod);
extern int TSIM_New   (IShell * piShell,AEECLSID cls,void **ppif);
extern int VkeyCtl_New(IShell * pIShell, AEECLSID cls, void ** ppobj);
#endif
extern int GameMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);

#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
extern int FrenDuoAppMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_VERSION_C316)
extern int IndexZoneAppMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int ServiceAppMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int MobileTrackerMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);//add by yangdecai
#endif

#ifdef FEATURE_APP_MULTIMEDIA
extern int MultimedMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
#if defined(FEATURE_FLEXI_STATIC_BREW_APP) || defined(FEATURE_SMARTFREN_STATIC_BREW_APP)||defined (FEATURE_VERSION_FLEXI203P)
extern int StaticappMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

/* =====================================================
   Statically linked application list.
=====================================================*/
static const AEEStaticMod gOEMStaticModList[] =
{
#ifndef FEATURE_UIONE_HDK
#if defined(FEATURE_APP_UI) && !defined(FEATURE_MANGO_UI)
      {AEEFS_MIF_DIR"coreapp.mif", CoreApp_Load},
#ifndef USES_MMI
    {AEEFS_MIF_DIR"corestartapp.mif", CoreStartApp_Load},
#endif
#endif
#endif // FEATURE_UIONE_HDK

    {AEEFS_MIF_DIR"wms.mif", CWMSMod_Load},
#ifdef FEATURE_APP_QUICKTEST    
    //{AEEFS_MIF_DIR"extramenu.mif", ExtraMenuMod_Load},
    {AEEFS_MIF_DIR"quicktest.mif", QuickTest_Load},
#endif  
#ifdef FEATURE_APP_MIZONE
        {AEEFS_MIF_DIR"mizone.mif", MiZone_Load},
#endif 
    {AEEFS_MIF_DIR"clockapps.mif", ClockAppsMod_Load},
#ifdef FEATURE_APP_CALC
    {AEEFS_MIF_DIR"calcapp.mif", CalcMod_Load},
#endif
#ifdef FEATURE_APP_WORLDTIME	
    {AEEFS_MIF_DIR"worldtime.mif", WorldTimeMod_Load},
#endif
#ifdef FEATURE_APP_STOPWATCH
    {AEEFS_MIF_DIR"stopwatch.mif", StopWatchMod_Load},
#endif    
#ifdef FEATURE_APP_SCHEDULE
    {AEEFS_MIF_DIR"scheduleapp.mif", ScheduleApp_Load},
#endif

#ifdef FEATURE_SHORT_CODE_NAM_COUNT
    {AEEFS_MIF_DIR"otk.mif", OtkMod_Load},
#endif //FEATURE_SHORT_CODE_NAM_COUNT


    {AEEFS_MIF_DIR"mainmenu.mif", MainMenuMod_Load},
#ifdef FEATURE_APP_APPLICATION	
    {AEEFS_MIF_DIR"application.mif", ApplicationMod_Load},
#endif    
//#if defined( FEATURE_GAME_TETRIS)
//    {AEEFS_MIF_DIR"tetris.mif", TetrisMod_Load},
#ifdef FEATURE_APP_GAME
	{AEEFS_MIF_DIR"game.mif",GameMod_Load},
#endif
	
#if defined(FEATURE_VERSION_M8) || defined(FEATURE_VERSION_M8P) || defined (FEATURE_VERSION_M8021)
	{AEEFS_MIF_DIR"frenduo.mif",FrenDuoAppMod_Load},
#endif

#if defined(FEATURE_VERSION_C316)
	{AEEFS_MIF_DIR"indexzone.mif",IndexZoneAppMod_Load},
	{AEEFS_MIF_DIR"service.mif",ServiceAppMod_Load},
	{AEEFS_MIF_DIR"mobiletracker.mif",MobileTrackerMod_Load}, //add by yangdecai
#endif
	
#ifdef FEATURE_APP_MULTIMEDIA
		{AEEFS_MIF_DIR"multimedia.mif",MultimedMod_Load},
#endif
//#endif


#if defined( FEATURE_GAME_BRICKATTACK)
    {AEEFS_MIF_DIR"brickattack.mif", Brick_Load},
#endif
#if defined( FEATURE_GAME_BLACKJACK)
    {AEEFS_MIF_DIR"blackjack.mif", BlackJack_Load},
//    {AEEFS_MIF_DIR"fiveball.mif", FiveBall_Load},
#if defined(FEATURE_VERSION_X3)
      {AEEFS_MIF_DIR"pintu.mif", PinTu_Load},
      {AEEFS_MIF_DIR"tetris.mif", TetrisMod_Load},
#endif

#if defined (FEATURE_VERSION_ESIA)
    {AEEFS_MIF_DIR"tetris.mif", TetrisMod_Load},
#endif

//    {AEEFS_MIF_DIR"gmfive.mif", GmFive_Load},
#if defined(FEATURE_VERSION_C316)
      //{AEEFS_MIF_DIR"gmfive.mif", GmFive_Load},
      {AEEFS_MIF_DIR"tetris.mif", TetrisMod_Load},
#endif
#endif

#if defined( FEATURE_GAME_LKV)
    {AEEFS_MIF_DIR"lkv.mif", LKV_Load},
#endif
#if defined( FEATURE_JEWISH_CALENDAR)
	{ AEEFS_MIF_DIR"jewishcalendar.mif",JewishCalendar_Load},
#endif

#if defined(FEATURE_RECORDER)
	{ AEEFS_MIF_DIR"recorder.mif", Recorder_Load},
#endif

#ifdef FEATURE_ANALOG_TV
    {AEEFS_MIF_DIR"tvapp.mif", CTVAppMod_Load}, 
#endif
#ifdef FEATURE_WAPBROWSER_APP
	//{AEEFS_MIF_DIR"wapbrowser.mif",CWapBrowserAppMod_Load},
#endif
#if defined(FEATURE_CONTACT_APP)
    {AEEFS_MIF_DIR"contactapp.mif", ContApp_Load},
#endif
#ifdef FEATURE_APP_PBPRINT
#error code not present
#endif //FEATURE_APP_PBPRINT

#if defined (FEATURE_BREWAPPCOORD)
   {AEEFS_MIF_DIR"bacproxyapp.mif",BACProxyApp_Load},
#endif

#if defined(FEATURE_APP_CONTACT)
    {AEEFS_MIF_DIR"contactapp.mif", ContactMod_Load},
#endif

#if defined(FEATURE_ALARMCLOCK_APP)
    {AEEFS_MIF_DIR"alarmclock.mif", AlarmClock_Load},
#endif

#if defined(FEATURE_NETSETTINGS_APP)
      {AEEFS_MIF_DIR"netsettings.mif", NetSettings_Load},
#endif

#if defined(FEATURE_AUXSETTINGS_APP)
      {AEEFS_MIF_DIR"auxsettings.mif", AUXSettings_Load},
#endif

#if defined(FEATURE_UIONE_HDK)
      {AEEFS_MIF_DIR"testapps.mif", TestAppsMod_Load},
#endif

#if defined(FEATURE_APP_CAT)

   {AEEFS_MIF_DIR"catapp.mif", CCatApp_Load},
#endif

//#if defined (FEATURE_APP_RECENTCALLS)
    {AEEFS_MIF_DIR"recentcalls.mif", RecentCalls_Load},
//#endif

#if defined (FEATURE_APP_UTK)
    {AEEFS_MIF_DIR"utk.mif", UTK_Load},
#endif
#if defined (FEATURE_APP_NUMBERMANAGER)
    {AEEFS_MIF_DIR"numbermanager.mif", NumberManagerMod_Load},
#endif
#if defined(FEATURE_IVECTOR) && !defined(FEATURE_MANGO_UI)
    {AEEFS_MIF_DIR"vector.mif", IVectorMod_Load},
#endif

#if defined(FEATURE_WMS_APP)
    {AEEFS_MIF_DIR"wms.mif", WMSAPP_Load},
#endif

#if defined	(FEATURE_APP_BLUETOOTH)
//	{AEEFS_MIF_DIR"btapp.mif",  BTApp_Load},
	{AEEFS_MIF_DIR"btapp.mif",  BTAppMod_Load},
#endif

#if defined(FEATURE_MFLO)
#error code not present
#endif

#if defined(FEATURE_APP_QCAMERA) && !defined(FEATURE_MANGO_UI)
    {AEEFS_MIF_DIR"qcamera.mif", QCam_Load},
#if defined(FEATURE_APP_QALBUM) && !defined(FEATURE_MANGO_UI)
    {AEEFS_MIF_DIR"qalbum.mif", Qalbum_Load},
#endif // FEATURE_APP_QALBUM
#endif

#if defined(FEATURE_APP_QTCAM)
    {AEEFS_MIF_DIR"qtcam.mif", QTCam_Load},
#endif

#if defined(FEATURE_APP_QCAMCORDER) && !defined(FEATURE_MANGO_UI)
    {AEEFS_MIF_DIR"qcamcorder.mif", Qcamcorder_Load},
#endif

#if defined(FEATURE_APP_QMDP)
#error code not present
#endif

#if defined (FEATURE_ICONTENTTEST)
        {AEEFS_MIF_DIR"icontenttest.mif", IContenttestApp_Load},
#endif

#if defined (FEATURE_CONTENTEVTMGRTEST)
        {AEEFS_MIF_DIR"contentevtmgrtest.mif", IContentEvtMgrTestApp_Load},
#endif

#if defined (FEATURE_ICONTENTHANDLERTEST)
        {AEEFS_MIF_DIR"icontenthandlertest.mif", IContentHandlerTestApp_Load},
#endif


#if defined(FEATURE_APP_MMS) || defined(FEATURE_APP_WAP)
    {AEEFS_MIF_DIR"tmt.mif", TMT_Load},
#endif

// SMS Listener App
#if defined(FEATURE_SMS_LISTENER)
    {AEEFS_MIF_DIR"smslistener.mif", SMSListener_Load},
#endif

#if defined(FEATURE_APP_SHOW3D)
    {AEEFS_MIF_DIR"show3d.mif", Show3D_Load},
#endif

#if defined(FEATURE_APP_MOBIMON)
    {AEEFS_MIF_DIR"mobimon.mif", MobiMon_Load},
#endif

#if defined(FEATURE_APP_PUREVOICE)
    {AEEFS_MIF_DIR"purevoice.mif", purevoice_Load},
#endif

#if defined(FEATURE_APP_GUNIMON)
    {AEEFS_MIF_DIR"gunimon.mif", GUniMon_Load},
#endif

#if defined(FEATURE_APP_SPRITEGAME)
    {AEEFS_MIF_DIR"spritegame.mif", spritegame_Load},
#endif

#if defined(FEATURE_GRAPHICS_APP_OPENGLES_SAMPLES)
#error code not present
#endif

#if defined(FEATURE_GRAPHICS_APP_GIGAMON)
  {AEEFS_MIF_DIR"gigamon.mif", gigamon_Load},
#endif

#if defined(FEATURE_GRAPHICS_APP_CALMON)
  {AEEFS_MIF_DIR"calmon.mif", calmon_Load},
#endif

#if defined(FEATURE_UI_3D_APP)
#error code not present
#endif

#if defined(FEATURE_GPSAPP)
#error code not present
#endif

#if defined(FEATURE_APP_MEDIAPLAYER)
      {AEEFS_MIF_DIR"mediaplayer.mif", CMediaPlayer_Load},
#endif

#if defined(FEATURE_APP_SVCPRG) && !defined( AEE_SIMULATOR)
      {AEEFS_MIF_DIR"svcprg.mif", CSvcPrgMod_Load},
#endif

#if defined (FEATURE_APP_FLDDBG)
      {AEEFS_MIF_DIR"flddbg.mif", CFieldDebugMod_Load},
#endif

#if defined (FEATURE_APP_DIALER)
    {AEEFS_MIF_DIR"callapp.mif", CallAppMod_Load},
#endif
    {AEEFS_MIF_DIR"settingmenu.mif", SettingMenuMod_Load},
    {AEEFS_MIF_DIR"securitymenu.mif", SecurityMenuMod_Load},
    {AEEFS_MIF_DIR"soundmenu.mif", SoundMenuMod_Load},
    {AEEFS_MIF_DIR"displaymenu.mif", DisplayMenuMod_Load},


#if defined( FEATURE_FM_RADIO)
    {AEEFS_MIF_DIR"fmradio.mif", FmRadioMod_Load},
#endif
#if defined (FEATURE_SECSERVICES_TESTAPP)
      {AEEFS_MIF_DIR"securityservices.mif", SecurityServices_Load},
#endif

#if defined (FEATURE_STATIC_MTP)
          {AEEFS_MIF_DIR"mtph.mif", MTPH_Load},
          {AEEFS_MIF_DIR"mcs.mif", MCS_Load},
#endif
#ifndef CUST_EDITION
#ifndef FEATURE_UIONE_HDK
#if defined (FEATURE_APP_DIALER) && !defined(FEATURE_MANGO_UI)
    {AEEFS_MIF_DIR"dialer.mif", CDialerApp_Load},
#endif
#endif // FEATURE_UIONE_HDK
#endif
// Datastats
#ifdef FEATURE_APP_DATASTATS
  {AEEFS_MIF_DIR"datastats.mif", CDataStatsApp_Load},
#endif

#ifdef FEATURE_QVPHONE_APP
#error code not present
#endif

#if defined(FEATURE_IBT) && !defined(FEATURE_MANGO_UI)
  BT_STATIC_MOD_LIST_ENTRY
#endif
#ifndef WIN32
#ifdef FEATURE_STATIC_RAPTOR
#ifdef RAPTOR_UMTS_VPA
      {AEEFS_MIF_DIR"ruvpa.mif", BNMApp_Mod_Load},
#else
      {AEEFS_MIF_DIR"bnmapp.mif", BNMApp_Mod_Load},
#endif
#ifdef RAPTOR_UMTS_DDA
      {AEEFS_MIF_DIR"rudda.mif", RUDDA_Mod_Load},
#endif
#endif  // FEATURE_STATIC_RAPTOR
#endif // WIN32
#ifdef FEATURE_JAVA
#error code not present
#endif

#ifdef FEATURE_IMS
#error code not present
#endif // FEATURE_IMS
#ifdef FEATURE_IMS_ISIP
      {AEEFS_MIF_DIR"isipmanager.mif", AEESipMod_Load},
#endif // FEATURE_ISIP

#if defined(FEATURE_FMRADIO_APP) && !defined(FEATURE_MANGO_UI)
  {AEEFS_MIF_DIR"fmradioapp.mif", FMRadioApp_Load},
#endif
#if defined(FEATURE_MANGO_UI)
  {AEEFS_MIF_DIR"brewport.mif", brewPort_Load},
#endif

#if defined(FEATURE_APP_MEDIAGALLERY)
      {AEEFS_MIF_DIR"mediagallery.mif", MediaGalleryApp_Load},
#endif

#ifdef FEATURE_APP_CAMERA
    {AEEFS_MIF_DIR"cameraapp.mif", CCameraAppMod_Load}, 
#endif

#if defined(FEATURE_APP_MUSICPLAYER)
      {AEEFS_MIF_DIR"musicplayer.mif", MusicPlayer_Load},
#endif

#if defined(FEATURE_VIDEOPLAYER)
    {AEEFS_MIF_DIR"videoplayer.mif", VideoPlayerMod_Load},
#endif

#ifdef FEATURE_NEMOBREW
    {AEEFS_MIF_DIR"nemobrew.mif", nemobrew_Load},
#endif

#if defined(FEATURE_SUPPORT_WAP_APP)
    {AEEFS_MIF_DIR"netfront3.mif", CNetFront3_Load},
#endif

#if defined(FEATURE_APP_TIMERAPP)
      {AEEFS_MIF_DIR"apptimer.mif", AppTimerMod_Load},//wu raojin add
#endif
#ifndef WIN32
#ifndef CUST_EDITION
#if defined FEATURE_SUPPORT_BT_APP
    {AEEFS_MIF_DIR"bluetooth.mif", tBtuiApp_Load},
#endif
#endif
#endif
#if defined FEATURE_SUPPORT_BUIW
    {AEEFS_MIF_DIR"forms.mif", FormsMod_Load},
    {AEEFS_MIF_DIR"widgets.mif", WidgetMod_Load},
#endif


#if 0//defined FEATURE_SUPPORT_WAP_APP
    {AEEFS_MIF_DIR"editapplet.mif", EditApp_Load},
    {AEEFS_MIF_DIR"editextension.mif", EditExt_Load},
    {AEEFS_MIF_DIR"oemextfor3part.mif", OEMExtFor3Part_Load},
    
    {AEEFS_MIF_DIR"browserwidget.mif", AEEBrwMod_Load},
    {AEEFS_MIF_DIR"brw_app.mif", AEEBrwAppMod_Load},
    {AEEFS_MIF_DIR"drm.mif", AEEDrmMod_Load},
    {AEEFS_MIF_DIR"sec.mif", AEESecMod_Load},
    {AEEFS_MIF_DIR"wap.mif", AEEWapMod_Load},
    {AEEFS_MIF_DIR"wecsc.mif", AEEWecscMod_Load},
    {AEEFS_MIF_DIR"pushe.mif", AEEPushMod_Load},
#endif
//#endif

#if defined(FEATURE_CONVERTER)
         {AEEFS_MIF_DIR"converter.mif", ConverterMod_Load},
#endif

#ifdef FEATURE_SPORTS_APP
//{ AEEFS_MIF_DIR"sportsapp.mif",SportsMod_Load},
#endif

#ifdef FEATURE_MORSE
   { AEEFS_MIF_DIR"morse.mif",MorseMod_Load},
#endif
#ifdef FEATURE_CARRIER_CHINA_TELCOM
            //{AEEFS_MIF_DIR"custservice.mif",CustServiceMod_Load},
            //{AEEFS_MIF_DIR"esurfing.mif",EsurfingMod_Load},
            //{AEEFS_MIF_DIR"notebook.mif",NotebookMod_Load},
#endif /*FEATURE_CARRIER_CHINA_TELCOM*/


#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 20090407 add
	  { AEEFS_MIF_DIR"adjustpenapp.mif",AdjustPenMod_Load},
#endif
#if defined (FEATURE_FLEXI_STATIC_BREW_APP) || defined(FEATURE_VERSION_FLEXI203P)
	   {AEEFS_MIF_DIR"staticapp.mif",StaticappMod_Load},
#ifdef FEATURE_NASRANI
	   { AEEFS_MIF_DIR"nasrani.mif",NASRANI_Load},
#endif
#ifdef FEATURE_FMN2010
  	   { AEEFS_MIF_DIR"fmn2010.mif",Fmn2010_Load},
#endif  	   

#ifdef FEATURE_FPT005
	   { AEEFS_MIF_DIR"fpt005.mif",Fpt005_Load},
#endif	   
#ifdef FEATURE_CAH006	   
	   { AEEFS_MIF_DIR"cah006.mif",Cah006_Load},
#endif

#ifdef FEATURE_GURU
	  { AEEFS_MIF_DIR"guru.mif",GURU_Load},
#endif



#endif	   /*FEATURE_FLEXI_STATIC_BREW_APP*/

#ifdef FEATURE_VERSION_FLEXI203P
	 { AEEFS_MIF_DIR"odpmora.mif",ODPMORA_Load},
	 
	{ AEEFS_MIF_DIR"fgen.mif",FlexiMusik_Load},
		 

#endif

#ifdef FEATURE_SMARTFREN_STATIC_BREW_APP
	   { AEEFS_MIF_DIR"staticapp.mif",StaticappMod_Load},
#ifdef FEATURE_SMARTFREN_BSHOP
	   { AEEFS_MIF_DIR"bshop.mif",BSHOP_Load},
#endif

#ifdef FEATURE_SMARTFREN_TWITTER
	   { AEEFS_MIF_DIR"twitter.mif",TWITTER_Load},
#endif

#ifdef FEATURE_SMARTFREN_FACEBOOK
	   { AEEFS_MIF_DIR"facebook.mif",FACEBOOK_Load},
#endif

#ifdef FEATURE_SMARTFREN_MSF
	   { AEEFS_MIF_DIR"msf.mif",msf_Load},
#endif

#ifdef FEATURE_SMARTFREN_MDIRECTORY
	   { AEEFS_MIF_DIR"odpmora.mif",ODPMORA_Load},
#endif

#ifdef FEATURE_SMARTFREN_MUSICBOX
	   { AEEFS_MIF_DIR"fgen.mif",SmartMusik_Load},
#endif

#ifdef FEATURE_SMARTFREN_TOMB
		{AEEFS_MIF_DIR"tomb.mif",Tombapp_Load},
#endif
		//{AEEFS_MIF_DIR"moka.mif",MOKA_Load},
#endif
   {NULL, NULL}
};

/* =====================================================
   statically linked list of lists of AEEStaticApp
=====================================================*/
const AEEStaticMod *gOEMStaticModLists[] = {
   gOEMStaticModList,
   NULL
};

/* =====================================================
   Statically linked class list.
=====================================================*/
static const AEEStaticClass gOEMStaticClassList[] = {
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 20090407 add
   {AEECLSID_VKEY_CONTROL,    ASCF_UPGRADE,PL_SYSTEM, NULL, VkeyCtl_New},
   {AEECLSID_OEMITSIM,                   0,0, NULL, TSIM_New},   //add by ydc
#endif
#if defined(FEATURE_STATIC_APP_JEODE)
//{AEECLSID_APP_JEODE,        (ASCF_PRIV | ASCF_UPGRADE),0, NULL, JeodeK_New},
   {0x10000001,               (ASCF_PRIV | ASCF_UPGRADE),0, NULL, JeodeK_New},
#endif
#if defined(FEATURE_STATIC_APP_J9)
//{AEECLSID_APP_J9,           (ASCF_PRIV | ASCF_UPGRADE),0, NULL, J9Mod_New},
   {0x11109999,               (ASCF_PRIV | ASCF_UPGRADE),0, NULL, J9Mod_New},
#endif
#if defined (FEATURE_QCT_JPEG_DECODER)
   {AEECLSID_JPEG,            ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
#endif
#if defined(FEATURE_BREW_BN)
   {AEECLSID_BN,              ASCF_PRIV,0,NULL,AEEBN_New},
#endif

#if (defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))
   {AEECLSID_PICTBRIDGE,   (ASCF_PRIV | ASCF_UPGRADE),0, NULL, OEMPB_New},
#endif //(defined(FEATURE_PICTBRIDGE) || defined(FEATURE_PICTBRIDGE_HS))

#if (defined(FEATURE_ANNUNCIATOR) && !defined(FEATURE_MANGO_UI))
   {AEECLSID_ANNUNCIATOR,        ASCF_PRIV,0,NULL,OEMAnnunciator_New},
#endif /* FEATURE_ANNUNCIATOR */

#if defined(FEATURE_IMIC)
   {AEECLSID_MIC,                (ASCF_PRIV | ASCF_UPGRADE),0,NULL, AEEMIC_New},
#endif
#if defined(FEATURE_APP_CAT)
   {AEECLSID_CAT,                ASCF_PRIV,0,NULL,OEMCAT_New},
   {AEECLSID_STK,                0,0,NULL,AEESTK_New},
   {AEECLSID_STKNOTIFIER,        0,0,NULL,AEESTKNotifier_New},
#endif
#if defined (FEATURE_BREWAPPCOORD)
   {AEECLSID_ALTENVSHELL,        0,0,NULL,AEEAltEnvShell_New},
#endif

#if defined(FEATURE_IIPL)
   {AEECLSID_IPL,                   ASCF_PRIV,0,NULL,OEMIPL_New},
#ifndef CUST_EDITION
   {AEECLSID_BitmapFX,                   ASCF_PRIV,0,NULL,OEMBitmapFX_New},
#endif
#endif

#if defined(FEATURE_ICM)
   {AEECLSID_CM,                   ASCF_PRIV,0,NULL,OEMCM_New},
   {AEECLSID_CM_NOTIFIER,          ASCF_PRIV,0,NULL,OEMCMNotifier_New},
   //{AEECLSID_CALLOPTS,             ASCF_PRIV,0,NULL,OEMCallOpts_New},
   //{AEECLSID_CM_MODEL,                   ASCF_PRIV,0,NULL,OEMCMModel_New},
#endif /* FEATURE_ICM */
   
#if !defined(FEATURE_MANGO_UI)
   {AEECLSID_ALERT,                   ASCF_PRIV,0,NULL,OEMALERT_New},
   {AEECLSID_ALERT_NOTIFIER,          ASCF_PRIV,0,NULL,OEMALERT_NOTIFIER_New},
#endif

#if defined(FEATURE_UIALARM)
    {AEECLSID_UIALARM,      ASCF_PRIV,0,NULL,AEEAlarm_New},
#endif
#if defined(FEATURE_APP_WAP)
   {AEECLSID_SEC_WAP_APP,        (ASCF_PRIV | ASCF_UPGRADE),0,NULL,AEESEC_New},
#endif

//#if defined (FEATURE_WMS_APP)
   {AEECLSID_WMS,                ASCF_PRIV,0,NULL,OEMWMS_New},
//#endif
#ifdef FEATURE_ANALOG_TV
	{MMI_CLSID_IMMITV,		   ASCF_PRIV,0,NULL,CMMITv_New},
	{AEECLSID_TLG_ATV,		  ASCF_UPGRADE, 0, NULL, OEMTLGAtv_New},
#endif

#if defined(FEATURE_IWMSDIAG) && !defined(FEATURE_MANGO_UI)
   {AEECLSID_WMSDIAG,         ASCF_PRIV,0,NULL,AEEWMSDIAG_New},
   {AEECLSID_WMSDIAG_NOTIFIER,  ASCF_PRIV,0,NULL,AEEWMSDIAGNotifier_New},
#endif

#if defined (FEATURE_STA_RAT)
#error code not present
#endif

#if defined (FEATURE_IVR)
   {AEECLSID_VR,                 ASCF_PRIV,0,NULL,OEMVR_New},
#endif


#if defined(FEATURE_ICARD)
   {AEECLSID_CARD,            ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCard_New},
   {AEECLSID_CARD_NOTIFIER,   ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCardNotifier_New},
#endif
#ifndef WIN32
#if defined(FEATURE_ICARDSESSION)
   {AEECLSID_CARDSESSION,                    ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCardSession_New},
   {AEECLSID_CARDSESSION_SLOT2,              ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCardSession_New},
   {AEECLSID_CARDSESSION_NOTIFIER,           ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCardSessionNotifier_New},
   {AEECLSID_CARDSESSION_NOTIFIER_SLOT2,     ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCardSessionNotifier_New},
#endif
#endif
#if defined(FEATURE_ICARD_IMODEL)
   {AEECLSID_CARD_MODEL,     ASCF_PRIV, PL_ADDRBOOK, NULL, OEMCardModel_New},
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

#if defined(FEATURE_BREW_DEV)

#if defined(FEATURE_DEBUG_EVENT)
{AEECLSID_DEBUGEVENT,        ASCF_PRIV,0,NULL,OEMDebugEvent_New},
#endif

#endif

#if defined FEATURE_BREW_NATIVENOTIFIER
  {AEECLSID_NATIVENOTIFIER,  ASCF_PRIV, 0, NULL,OEMNativeNotifierClass_New},
#endif

#ifdef FEATURE_MMGPS
#error code not present
#endif

#ifdef FEATURE_SEC_DRM
#error code not present
#endif

#ifdef FEATURE_SEC_IPSEC
   {AEECLSID_IPSECSP,        ASCF_PRIV,0,NULL,AEEIPSecSP_New},
   {AEECLSID_IPSECSPDB,        ASCF_PRIV,0,NULL,AEEIPSecSPDB_New},
   {AEECLSID_IPSECSA,        ASCF_PRIV,0,NULL,AEEIPSecSA_New},
   {AEECLSID_IPSECSADB,        ASCF_PRIV,0,NULL,AEEIPSecSADB_New},
#endif

#if defined (FEATURE_IPOSDETEXT)
   {AEECLSID_POSDETEXT,       ASCF_PRIV,0,NULL,OEMPosDetExt_New},
#endif
#ifdef FEATURE_IBT
   BT_STATIC_CLASS_LIST_ENTRY
#endif
#if defined(FEATURE_APP_TEST_AUTOMATION) && !defined(FEATURE_MANGO_UI)
   {AEECLSID_DIAG,         ASCF_PRIV,0, NULL, OEMDiag_New},
#endif
#ifdef FEATURE_QVPHONE
      {AEECLSID_VIDEOPHONE,         ASCF_PRIV,0,NULL, OEMVideoPhone_New},
      {AEECLSID_VIDEOPHONE_NOTIFIER,      ASCF_PRIV,0,NULL,OEMVideoPhone_NOTIFIER_New},
#endif
#if defined (FEATURE_BREW_IMOVIEFILE)
      {AEECLSID_MOVIEFILE,          (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, IMovieFile_New},
#endif
#if defined (FEATURE_BREW_IMOVIEEDIT)
      {AEECLSID_IMOVIEEDIT,   (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, IMovieEdit_New},
#endif
#ifndef WIN32
#if defined(FEATURE_TSG_EXT)
      {0x01023c45,  ASCF_PRIV, 0, NULL, AEE01023c45_New}, // AEEAPPLOG 
      {0x01013fd6,  ASCF_PRIV, 0, NULL, AEE01013fd6_New},
#ifdef FEATURE_STATIC_RAPTOR
      {0x01012eff,  ASCF_PRIV|ASCF_UPGRADE, 0, NULL, BnmRm_New},
#endif
#endif
#endif // Win32
//MTP MCS static Extension
#if defined (FEATURE_SME_DRMMS)
   {AEECLSID_MTPDRM,  ASCF_UPGRADE, 0, NULL, AEEMtpDrm_New},
#endif

   // Static SoundCtl Extension
#if (defined(FEATURE_AUDIO_EQUALIZER)||defined(FEATURE_AUDIO_QCONCERT)||defined(FEATURE_AUDIO_SPECTRUM_ANALYZER))
   {AEECLSID_SOUNDCTL,  ASCF_UPGRADE, 0, NULL, OEMSoundCtl_New},
#endif

#if defined(FEATURE_ENABLE_SVG_SVGDOM)
   {AEECLSID_MEDIASVG,        ASCF_UPGRADE, 0, IMediaSVG_Init,  IMediaSVG_New},
#elif defined(FEATURE_SVG)
   {AEECLSID_MEDIASVG,        ASCF_UPGRADE, 0, IMediaPMD_Init,  IMediaPMD_New},
#endif // defined(FEATURE_ENABLE_SVG_SVGDOM)


#if defined(FEATURE_BREW_JOYSTICK)
   {AEECLSID_IJOYSTICK1,          (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, AEEJoystick_New},
   {AEECLSID_IJOYSTICK2,         (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, AEEJoystick_New},
#endif
// QAudioFX Static Extension
#ifdef FEATURE_QAUDIOFX
   {AEECLSID_MEDIAAUDIOENV, ASCF_UPGRADE, 0, NULL, IMediaAudioEnv_New},
#endif
// QoS Static Extension
#if defined(FEATURE_IQOSSESSIONEX)
   {AEECLSID_QOSSESSIONEX,        (ASCF_PRIV | ASCF_UPGRADE), PL_SYSTEM, NULL, IQoSSessionEx_New},
   {AEECLSID_QOSBUNDLEEX,         (ASCF_PRIV | ASCF_UPGRADE), PL_SYSTEM, NULL, IQoSBundleEx_New},
#endif
#if defined(FEATURE_BREW_JOYSTICK)
   {AEECLSID_IJOYSTICK1,          (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, AEEJoystick_New},
   {AEECLSID_IJOYSTICK2,         (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, AEEJoystick_New},
#endif

// HDR static extension
#if defined (FEATURE_IHDR)
   {AEECLSID_HDR,        (ASCF_PRIV | ASCF_UPGRADE), PL_SYSTEM, NULL, IHDR_New},
#endif
// YCbCrFactory Static Extension
#if defined (FEATURE_IYCBCR)
   {AEECLSID_YCBCR, ASCF_PRIV, 0, NULL, IYCbCrFactory_New},
#endif
#ifdef FEATURE_ICONTENT
   {AEECLSID_CONTENTBROWSER     ,       ASCF_UPGRADE, 0, NULL, AEEContentBrowser_New},
   {AEECLSID_CONTENTMGR         ,       ASCF_UPGRADE, 0, NULL, AEEContentMgr_New},
   {AEECLSID_CATEGORYMGR                ,       ASCF_UPGRADE, 0, NULL, AEECategoryMgr_New},
   {AEECLSID_CONTENTNOTIFIER    ,       ASCF_UPGRADE, 0, NULL, AEEContentNotifier_New},
   {AEECLSID_ISQLMGR,  ASCF_UPGRADE, 0, NULL, AEESQLMgr_New},
#endif

#ifdef FEATURE_ICONTENTHANDLER
   {AEECLSID_CONTENTHANDLER             ,       ASCF_UPGRADE, 0, NULL, AEEContentHandler_New},
   {AEECLSID_CONTENTMETAINFOFACTORY     ,       ASCF_UPGRADE, 0, NULL, AEEContentMetaInfoFactory_New},
   {AEECLSID_CONTENTOMADRMMSG           ,       ASCF_UPGRADE, 0, NULL, AEEContentOmaDrmMsg_New},
   {AEECLSID_CONTENTEVENTMGR  ,   ASCF_UPGRADE, 0, NULL, AEEContentEventMgr_New},
#endif

#if defined(FEATURE_SECMSM_APPS) || defined(FEATURE_ODM_BROWSER)
#if defined(FEATURE_ICONTENT)
   {AEECLSID_CONTENT_DB_MODEL,     ASCF_UPGRADE, 0, NULL, ContentDBModel_New},
#endif // FEATURE_ICONTENT
   {AEECLSID_QCONFIG, 0,0,NULL,OEMConfig_New},
   {AEECLSID_QCONFIGMODEL, 0,0,NULL,OEMConfig_New},
   {AEECLSID_ITHEMEMGR,      ASCF_UPGRADE, 0, NULL, IThemeMgr_New},
   {AEECLSID_GENERICVIEWER_EX, ASCF_UPGRADE,  0, 0, OEMGenericViewer_New},
#endif //(FEATURE_SECMSM_APPS) || (FEATURE_ODM_BROWSER)

#if defined(FEATURE_SECMSM_APPS)
   {AEECLSID_MMC, 0, 0, NULL, AEEMMC_New},
   {AEECLSID_MMCNOTIFIER, 0, 0, NULL, AEEMMC_New},
#endif //(FEATURE_SECMSM_APPS)

#if defined(FEATURE_ODM_BROWSER)
    {AEECLSID_BROWSER_MODEL, 0, 0, NULL, BrowserModel_New},
#endif //(FEATURE_ODM_BROWSER)
#ifndef CUST_EDITION
#if defined(FEATURE_ADDRBOOK)
   {AEECLSID_ADDRBOOKEXT, ASCF_UPGRADE, PL_SYSTEM, NULL, OEMAddrBookExt_New},
#endif
#endif
#ifdef FEATURE_UIONE_HDK
   {AEECLSID_AEESIGNALTONE, ASCF_PRIV, 0, NULL, AEESignalTone_New},
#endif // FEATURE_UIONE_HDK
#if defined(FEATURE_AUDFMT_HYBRID_MODE)
   {AEECLSID_SYNTHQUALITY, ASCF_UPGRADE, 0, NULL, ISynthQuality_New},
#endif
#if defined(FEATURE_CALLLIST)
   {AEECLSID_CALLLIST,ASCF_PRIV,0,NULL,OEMCallList_New},
#endif
#if defined(FEATURE_ANICTL)
   {AEECLSID_ANICTL, ASCF_UPGRADE, 0, NULL, AniCtl_New},
#endif
#ifdef FEATURE_SENSORS
   SENSORS_STATIC_CLASS_LIST_ENTRY
#endif

#ifdef FEATURE_GPSONE_XTRA_BREW_EXT
#error code not present
#endif

#if defined(FEATURE_IRDM)
#error code not present
#endif

   {0,0,0,NULL,NULL}
};

/* =====================================================
   statically linked list of lists of AEEStaticClasses
=====================================================*/
const AEEStaticClass *gOEMStaticClassLists[] = {
   gOEMStaticClassList,
#if defined(FEATURE_I3D)
   gAEE3DClasses,
#endif
#if defined(FEATURE_OVERLAY)
      gOEMOverlayClasses,
#endif
#if defined(FEATURE_GRAPHICS_OPENGLES_CL)
#error code not present
#endif
#if defined(FEATURE_GRAPHICS_OPENVG)
#error code not present
#endif
#ifndef WIN32
#if defined (FEATURE_QCT_JPEG_DECODER)
   gAEEJpegClasses,
#endif
#endif
#if defined(FEATURE_BREW_SIGNAL)
   gascSignalClassList,
#endif
   NULL
};

//lint -restore

