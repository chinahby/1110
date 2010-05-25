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

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
//lint -e786 Suppress "String concatenation within initializer" info msg
#include "OEMFeatures.h"
#include "OEMModTableExt.h"
#include "AEEAnnunciator.h"
#include "AEEFile.h"
#include "OEMClassIDs.h"


#ifdef FEATURE_I3D
//#include "OEM3D_Demos.h"
#endif

#ifdef FEATURE_NO_WMS_APP
#undef FEATURE_WMS_APP
#endif
#ifndef WIN32
#ifdef FEATURE_WMS_APP
#include "AEEWMS.h"
#endif
#else
#include "AEEWMS.h"
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
#include "Diag.bid"
#endif

#ifdef FEATURE_IIPL
#include "IPL.BID"
#endif

#if defined(FEATURE_ICM)

#include "AEECM.h"
#include "AEEAlert.h"

#if defined(FEATURE_WLAN)
#include "AEEWIFI.h"
#endif

#include "AEEBatt.h"
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_GPRS)
#include "PDP.BID"
#endif
#endif

#if defined(FEATURE_UIALARM) && !defined( AEE_SIMULATOR)
#include "AEEAlarm.h"
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#include "AEESuppSvc.h"
#endif

#if defined (FEATURE_IPOSDETEXT)
 #include "AEEPosDetExt.h"
#endif


#if defined(FEATURE_GRAPHICS_APP_OPENGLES_SAMPLES)
#include "OpenGLES_Samples_modtable.h"
#endif

#ifdef FEATURE_IBT
#include "OEMModTableExt_BT.h"
#endif

#if defined(FEATURE_IYCBCR)
#include "IYCbCrFactory.h"
#endif

#ifdef FEATURE_JAVA
#include "java_config.h"
#endif

#if defined(FEATURE_MMGPS)
#include "AEELCA.h"
#endif

#ifdef FEATURE_SEC_DRM
#include "AEEDRM.h"
#include "AEEDCF.h"
#endif

#ifdef FEATURE_SEC_IPSEC
#include "IIPSecSP.h"
#include "IIPSecSPDB.h"
#endif

#if defined (FEATURE_STA_RAT)
#include "OEMRats.h"
#endif

#if defined (FEATURE_QVPHONE)
#include "AEEVideoPhone.h"
#endif /* FEATURE_QVPHONE */

#if defined FEATURE_BREW_NATIVENOTIFIER
#include "NativeNotifier.bid"
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
extern const AEEStaticClass gAEEOpenGLESClasses[];
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


#if defined FEATURE_SUPPORT_WAP_APP
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

#if defined(FEATURE_STATIC_APP_J9)
extern int J9Mod_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);
#endif
#if defined(FEATURE_STATIC_APP_JEODE)
extern int JeodeK_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);
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
#ifdef FEATURE_WMS_APP
 extern int OEMWMS_New (IShell *piShell, AEECLSID cls, void **pp);
#endif

#if defined(FEATURE_IWMSDIAG) && !defined(FEATURE_MANGO_UI) && !defined (USES_MMI)
 extern int AEEWMSDIAG_New (IShell *piShell, AEECLSID cls, void **pp);
 extern int AEEWMSDIAGNotifier_New (IShell *piShell, AEECLSID cls, void **pp);
#endif

// ICM
#if defined(FEATURE_ICM)
extern int OEMALERT_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMALERT_NOTIFIER_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMCM_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMCMNotifier_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMCallOpts_New (IShell *pIShell, AEECLSID cls, void **ppif);

#if defined(FEATURE_WLAN)
extern int OEMWIFI_New (IShell *pIShell, AEECLSID cls, void **ppif);
extern int OEMWIFIOpts_New (IShell *pIShell, AEECLSID cls, void **ppif);
#endif

//PDP
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_GPRS)
 extern int OEMPDP_New (IShell *pIShell, AEECLSID cls, void **ppif);
 #endif
//IBatt
extern int AEEBatt_New(IShell *pIShell, AEECLSID cls, void **ppif);
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
 extern int OEMSUPPSVC_NOTIFIER_New(IShell *pIShell, AEECLSID cls, void **ppif);
 extern int OEMSUPPSVC_New(IShell *pIShell, AEECLSID cls, void **ppif);
 extern int OEMSUPPSVCOpts_New (IShell *pIShell, AEECLSID cls, void **ppif);
#endif

// Native Notifier interface
#if defined FEATURE_BREW_NATIVENOTIFIER
extern int OEMNativeNotifierClass_New(IShell * pIShell, AEECLSID cls, void **ppobj);
#endif

#ifdef FEATURE_MMGPS
extern int OEMLCA_New(IShell *pIShell, AEECLSID cls, void **pp);
extern int OEMLCANotifier_New(IShell *pIShell, AEECLSID cls, void **pp);
#endif

#ifdef FEATURE_SEC_DRM
extern int OEMDRM_New( IShell *pShell, AEECLSID cls, void **ppif );
extern int OEMDCF_New( IShell *pShell, AEECLSID cls, void **ppif );
#endif

#ifdef FEATURE_SEC_IPSEC
extern int SECIPSSP_New( IShell *pShell, AEECLSID cls, void **ppif );
extern int SECIPSSPDB_New( IShell *pShell, AEECLSID cls, void **ppif );
#endif

#if defined (FEATURE_IPOSDETEXT)
extern int OEMPosDetExt_New(IShell *piShell, AEECLSID cls,void **pp);
#endif


// OEMDiag
//
#ifdef FEATURE_APP_TEST_AUTOMATION
extern int OEMDiag_New(IShell *ps, AEECLSID cls, void **ppif);
#endif

#ifdef FEATURE_IIPL
extern int OEMIPL_New(IShell *ps, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_QVPHONE)
  //VideoPhone
  extern int OEMVideoPhone_New(IShell *pIShell, AEECLSID cls, void **pp);
  extern int OEMVideoPhone_NOTIFIER_New(IShell *pIShell, AEECLSID cls, void **pp);
#endif /* FEATURE_QVPHONE */
   // MOVIEFILE
#if defined(FEATURE_BREW_IMOVIEFILE)
  extern int     IMovieFile_New(IShell * ps, AEECLSID cls, void **ppif);
#endif
#ifdef FEATURE_TSG_EXT
   //Required 01013fd6 Interface
   extern int AEE01013fd6_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
#ifdef FEATURE_STATIC_RAPTOR
   extern int BnmRm_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
#endif
#endif

#if defined(FEATURE_ANICTL)
extern int AniCtl_New (IShell * pIShell, AEECLSID clsid, void ** ppif);
#endif

// UI App
#if defined(FEATURE_APP_UI)
extern int CoreApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#ifndef USES_MMI
extern int CoreStartApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
#endif

extern int QuickTest_Load(IShell *pIShell,void *ph,IModule **ppMod);

extern int ExtraMenuMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
extern int StopWatchMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
extern int ClockAppsMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
extern int CalcMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
extern int WorldTimeMod_Load(IShell *pIShell,void *ph,IModule **ppMod);

extern int ScheduleApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
//#endif

#ifdef FEATURE_SHORT_CODE_NAM_COUNT
extern int OtkMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif //FEATURE_SHORT_CODE_NAM_COUNT

extern int MainMenuMod_Load( IShell *pIShell, void *ph, IModule **ppMod);
#if defined( FEATURE_GAME_TETRIS)
extern int TetrisMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#if defined( FEATURE_GAME_BRICKATTACK)
extern int Brick_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif
#if defined( FEATURE_GAME_BLACKJACK)
extern int BlackJack_Load(IShell *ps, void *pHelpers, IModule **pMod);
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
#if defined(FEATURE_APP_RECENTCALLS)
extern int RecentCalls_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// UTK App
#if defined(FEATURE_APP_UTK)
extern int UTK_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
// NumberManager App
#if defined(FEATURE_APP_NUMBERMANAGER)
extern int  NumberManagerMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
#endif
// Vector Interface
#if defined(FEATURE_IVECTOR)
extern int IVectorMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// WMS App
#if defined(FEATURE_WMS_APP)
extern int  CWMSMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// QCAMERA App
#if defined(FEATURE_APP_QCAMERA)
extern int QCam_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#if defined(FEATURE_APP_QALBUM)
extern int Qalbum_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif // FEATURE_APP_QALBUM
#endif

// QCAMCORDER App
#if defined(FEATURE_APP_QCAMCORDER)
extern int Qcamcorder_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// QMDP App
#if defined(FEATURE_APP_QMDP)
extern int Qmdp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
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
OPENGLES_SAMPLES_DECLARATIONS
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
extern int UI3D_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Media Player App
#if defined(FEATURE_APP_MEDIAPLAYER)
extern int CMediaPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// GPS App
#if defined(FEATURE_GPSAPP)
extern int gpsOne_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
// Dialer app
#if defined (FEATURE_APP_DIALER)
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
#if defined (FEATURE_APP_FLDDBG) && !defined (USES_MMI)
extern int CFieldDebugMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

// Datastats
#ifdef FEATURE_APP_DATASTATS
extern int CDataStatsApp_Load(IShell   *ps, void     *pHelpers, IModule **pMod);
#endif

#ifdef FEATURE_QVPHONE
extern int  CVideoPhone_Load(IShell *ps, void * pHelpers, IModule ** pMod);
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
   // QVM
extern int JavaApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#ifdef FEATURE_JAVA_AMS_APP
   // QVM external AMS
extern int AMSApp_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
#endif

#if defined(FEATURE_AUDFMT_HYBRID_MODE)
extern int ISynthQuality_New(IShell * ps, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_CALLLIST)
extern int  OEMCallList_New(IShell *pIShell, AEECLSID cls, void **ppif);
#endif

#if defined(FEATURE_APP_MEDIAGALLERY)
extern int MediaGalleryApp_Load(IShell* ps, void* pHelpers, IModule** ppMod); 
#endif
#ifndef WIN32
#if defined(FEATURE_APP_CAMERA)
extern int  CCameraAppMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif
#endif
#if defined(FEATURE_APP_MUSICPLAYER)
extern int MusicPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_VIDEOPLAYER)
extern int  VideoPlayerMod_Load(IShell *ps, void * pHelpers, IModule ** pMod);
#endif

#if defined(FEATURE_APPTIMER)
extern int AppTimerMod_Load(IShell *ps, void *pHelpers, IModule **pMod);
#endif

#if defined(FEATURE_CONVERTER)
extern int  ConverterMod_Load(IShell* ps, void* pHelpers, IModule** ppMod);
#endif
//end added

#ifdef FEATURE_SPORTS_APP
extern int SportsMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif

#ifdef FEATURE_MORSE
extern int MorseMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif

#ifdef FEATURE_CARRIER_CHINA_TELCOM
extern int CustServiceMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
extern int EsurfingMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
extern int NotebookMod_Load(IShell *pIShell,void *ph,IModule **ppMod);
#endif /*FEATURE_CARRIER_CHINA_TELCOM*/
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 20090407 add
extern int AdjustPenMod_Load(IShell *p,void *ph,IModule **ppMod);
extern int TSIM_New   (IShell * piShell,AEECLSID cls,void **ppif);
extern int VkeyCtl_New(IShell * pIShell, AEECLSID cls, void ** ppobj);
#endif
/* =====================================================
   Statically linked application list.
=====================================================*/
static const AEEStaticMod gOEMStaticModList[] =
{
#ifndef FEATURE_UIONE_HDK
#if defined(FEATURE_APP_UI)
    {AEEFS_MIF_DIR"coreapp.mif", CoreApp_Load},
#ifndef USES_MMI
    {AEEFS_MIF_DIR"corestartapp.mif", CoreStartApp_Load},
#endif
#endif
#endif // FEATURE_UIONE_HDK


    {AEEFS_MIF_DIR"extramenu.mif", ExtraMenuMod_Load},
    {AEEFS_MIF_DIR"quicktest.mif", QuickTest_Load},

    {AEEFS_MIF_DIR"clockapps.mif", ClockAppsMod_Load},
    {AEEFS_MIF_DIR"calcapp.mif", CalcMod_Load},
    {AEEFS_MIF_DIR"worldtime.mif", WorldTimeMod_Load},
    {AEEFS_MIF_DIR"stopwatch.mif", StopWatchMod_Load},

    {AEEFS_MIF_DIR"scheduleapp.mif", ScheduleApp_Load},
#ifdef FEATURE_SHORT_CODE_NAM_COUNT
    {AEEFS_MIF_DIR"otk.mif", OtkMod_Load},
#endif //FEATURE_SHORT_CODE_NAM_COUNT


    {AEEFS_MIF_DIR"mainmenu.mif", MainMenuMod_Load},

#if defined( FEATURE_GAME_TETRIS)
    {AEEFS_MIF_DIR"tetris.mif", TetrisMod_Load},
#endif


#if defined( FEATURE_GAME_BRICKATTACK)
    {AEEFS_MIF_DIR"brickattack.mif", Brick_Load},
#endif
#if defined( FEATURE_GAME_BLACKJACK)
    {AEEFS_MIF_DIR"blackjack.mif", BlackJack_Load},
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

#if defined(FEATURE_CONTACT_APP)
    {AEEFS_MIF_DIR"contactapp.mif", ContApp_Load},
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
#if defined (FEATURE_APP_RECENTCALLS)
    {AEEFS_MIF_DIR"recentcalls.mif", RecentCalls_Load},
#endif

#if defined (FEATURE_APP_UTK)
    {AEEFS_MIF_DIR"utk.mif", UTK_Load},
#endif
#if defined (FEATURE_APP_NUMBERMANAGER)
    {AEEFS_MIF_DIR"numbermanager.mif", NumberManagerMod_Load},
#endif
#if defined(FEATURE_IVECTOR)
    {AEEFS_MIF_DIR"vector.mif", IVectorMod_Load},
#endif

#if defined(FEATURE_WMS_APP)
    {AEEFS_MIF_DIR"wms.mif", CWMSMod_Load},
#endif

#if defined(FEATURE_APP_QCAMERA)
    {AEEFS_MIF_DIR"qcamera.mif", QCam_Load},
#if defined(FEATURE_APP_QALBUM)
    {AEEFS_MIF_DIR"qalbum.mif", Qalbum_Load},
#endif // FEATURE_APP_QALBUM
#endif

#if defined(FEATURE_APP_QCAMCORDER)
    {AEEFS_MIF_DIR"qcamcorder.mif", Qcamcorder_Load},
#endif

#if defined(FEATURE_APP_QMDP)
    {AEEFS_MIF_DIR"qmdp.mif", Qmdp_Load},
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
  OPENGLES_GOEMSTATICMODLIST_ENTRIES,
#endif

#if defined(FEATURE_GRAPHICS_APP_GIGAMON)
  {AEEFS_MIF_DIR"gigamon.mif", gigamon_Load},
#endif

#if defined(FEATURE_GRAPHICS_APP_CALMON)
  {AEEFS_MIF_DIR"calmon.mif", calmon_Load},
#endif

#if defined(FEATURE_UI_3D_APP)
  {AEEFS_MIF_DIR"ui3d.mif", UI3D_Load},
#endif

#if defined(FEATURE_GPSAPP)
    {AEEFS_MIF_DIR"gpsone.mif", gpsOne_Load},
#endif

#if defined(FEATURE_APP_MEDIAPLAYER)
      {AEEFS_MIF_DIR"mediaplayer.mif", CMediaPlayer_Load},
#endif

#if defined(FEATURE_APP_SVCPRG) && !defined( AEE_SIMULATOR)
      {AEEFS_MIF_DIR"svcprg.mif", CSvcPrgMod_Load},
#endif

#if defined (FEATURE_APP_FLDDBG) && !defined (USES_MMI)
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
// Datastats
#ifdef FEATURE_APP_DATASTATS
  {AEEFS_MIF_DIR"datastats.mif", CDataStatsApp_Load},
#endif
#ifdef FEATURE_QVPHONE
  {AEEFS_MIF_DIR"videophone.mif", CVideoPhone_Load},
#endif
#ifdef FEATURE_IBT
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
#endif
#ifdef FEATURE_JAVA
      {AEEFS_MIF_DIR"java.mif", JavaApp_Load},
#ifdef FEATURE_JAVA_AMS_APP
      {AEEFS_MIF_DIR"ams.mif", AMSApp_Load},
#endif
#endif

#if defined(FEATURE_APP_MEDIAGALLERY)
      {AEEFS_MIF_DIR"mediagallery.mif", MediaGalleryApp_Load},
#endif
#ifndef WIN32
#ifdef FEATURE_APP_CAMERA
    {AEEFS_MIF_DIR"cameraapp.mif", CCameraAppMod_Load},
#endif
#endif
#if defined(FEATURE_APP_MUSICPLAYER)
      {AEEFS_MIF_DIR"musicplayer.mif", MusicPlayer_Load},
#endif

#if defined(FEATURE_VIDEOPLAYER)
    {AEEFS_MIF_DIR"videoplayer.mif", VideoPlayerMod_Load},
#endif

#if defined(FEATURE_APPTIMER)
      {AEEFS_MIF_DIR"apptimer.mif", AppTimerMod_Load},//wu raojin add
#endif
#ifndef WIN32
#if defined FEATURE_SUPPORT_BT_APP
    {AEEFS_MIF_DIR"bluetooth.mif", tBtuiApp_Load},
#endif
#endif
#if defined FEATURE_SUPPORT_BUIW
    {AEEFS_MIF_DIR"forms.mif", FormsMod_Load},
    {AEEFS_MIF_DIR"widgets.mif", WidgetMod_Load},
#endif


#if defined FEATURE_SUPPORT_WAP_APP
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
#if defined(FEATURE_CONVERTER)
         {AEEFS_MIF_DIR"converter.mif", ConverterMod_Load},
#endif

#ifdef FEATURE_SPORTS_APP
{ AEEFS_MIF_DIR"sportsapp.mif",SportsMod_Load},
#endif

#ifdef FEATURE_MORSE
   { AEEFS_MIF_DIR"morse.mif",MorseMod_Load},
#endif
#ifdef FEATURE_CARRIER_CHINA_TELCOM
            {AEEFS_MIF_DIR"custservice.mif",CustServiceMod_Load},
            {AEEFS_MIF_DIR"esurfing.mif",EsurfingMod_Load},
            {AEEFS_MIF_DIR"notebook.mif",NotebookMod_Load},
#endif /*FEATURE_CARRIER_CHINA_TELCOM*/


#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh 20090407 add
	  { AEEFS_MIF_DIR"adjustpenapp.mif",AdjustPenMod_Load},
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

//extern int VkeyCtl_New(IShell * pIShell, AEECLSID cls, void ** ppobj);



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

#if defined(FEATURE_BREW_BN)
   {AEECLSID_BN,              ASCF_PRIV,0,NULL,AEEBN_New},
#endif

#if defined(FEATURE_ANNUNCIATOR)
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

#if defined(FEATURE_IIPL)
   {AEECLSID_IPL,                   ASCF_PRIV,0,NULL,OEMIPL_New},
#endif

#if defined(FEATURE_ICM)
   {AEECLSID_CM,                   ASCF_PRIV,0,NULL,OEMCM_New},
   {AEECLSID_CM_NOTIFIER,          ASCF_PRIV,0,NULL,OEMCMNotifier_New},
   {AEECLSID_CALLOPTS,             ASCF_PRIV,0,NULL,OEMCallOpts_New},
   {AEECLSID_ALERT,                   ASCF_PRIV,0,NULL,OEMALERT_New},
   {AEECLSID_ALERT_NOTIFIER,          ASCF_PRIV,0,NULL,OEMALERT_NOTIFIER_New},
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM_GPRS)
      {AEECLSID_PDP,                 ASCF_PRIV,0,NULL,OEMPDP_New},

#endif

#if defined(FEATURE_WLAN)
   {AEECLSID_WIFI,                 0,0,NULL,OEMWIFI_New},
   {AEECLSID_WIFI_OPTS,             0,0,NULL,OEMWIFIOpts_New},
#endif /* FEATURE_WLAN */

   //IBatt
      {AEECLSID_BATT,                ASCF_PRIV,0,NULL,AEEBatt_New},
      {AEECLSID_BATT_NOTIFIER,       ASCF_PRIV, 0,NULL, AEEBatt_New},
#endif /* FEATURE_ICM */
#if defined(FEATURE_UIALARM)
    {AEECLSID_UIALARM,      ASCF_PRIV,0,NULL,AEEAlarm_New},
#endif

#if defined(FEATURE_APP_WAP)
   {AEECLSID_SEC_WAP_APP,        (ASCF_PRIV | ASCF_UPGRADE),0,NULL,AEESEC_New},
#endif
#if defined (FEATURE_WMS_APP)
   {AEECLSID_WMS,                ASCF_PRIV,0,NULL,OEMWMS_New},
#endif

#if defined(FEATURE_IWMSDIAG) && !defined(FEATURE_MANGO_UI) && !defined (USES_MMI)
   {AEECLSID_WMSDIAG,         ASCF_PRIV,0,NULL,AEEWMSDIAG_New},
   {AEECLSID_WMSDIAG_NOTIFIER,  ASCF_PRIV,0,NULL,AEEWMSDIAGNotifier_New},
#endif

#if defined (FEATURE_STA_RAT)
   {AEECLSID_RATS,               ASCF_PRIV,0,NULL,OEMRats_New},
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
   {AEECLSID_SUPPSVC_NOTIFIER,                 ASCF_PRIV,0,NULL,OEMSUPPSVC_NOTIFIER_New},
   {AEECLSID_SUPPSVC,                          ASCF_PRIV,0,NULL,OEMSUPPSVC_New},
   {AEECLSID_SUPPSVCOPTS,                      ASCF_PRIV,0,NULL,OEMSUPPSVCOpts_New},
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
   {AEECLSID_LCA,                ASCF_PRIV,0,NULL, OEMLCA_New},
   {AEECLSID_LCANOTIFIER,        ASCF_PRIV,0,NULL, OEMLCANotifier_New},
#endif

#ifdef FEATURE_SEC_DRM
   {AEECLSID_IDRM,        ASCF_PRIV,0,NULL,OEMDRM_New},
   {AEECLSID_IDCF,        ASCF_PRIV,0,NULL,OEMDCF_New},
#endif

#ifdef FEATURE_SEC_IPSEC
   {AEECLSID_IPSECSP,        ASCF_PRIV,0,NULL,SECIPSSP_New},
   {AEECLSID_IPSECSPDB,        ASCF_PRIV,0,NULL,SECIPSSPDB_New},
#endif

#if defined (FEATURE_IPOSDETEXT)
   {AEECLSID_POSDETEXT,       ASCF_PRIV,0,NULL,OEMPosDetExt_New},
#endif
#ifdef FEATURE_IBT
   BT_STATIC_CLASS_LIST_ENTRY
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
   {AEECLSID_DIAG,         ASCF_PRIV,0, NULL, OEMDiag_New},
#endif
#ifdef FEATURE_QVPHONE
      {AEECLSID_VIDEOPHONE,         ASCF_PRIV,0,NULL, OEMVideoPhone_New},
      {AEECLSID_VIDEOPHONE_NOTIFIER,      ASCF_PRIV,0,NULL,OEMVideoPhone_NOTIFIER_New},
#endif
#if defined (FEATURE_BREW_IMOVIEFILE)
      {AEECLSID_MOVIEFILE,          (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, IMovieFile_New},
#endif
#ifndef WIN32
#if defined(FEATURE_TSG_EXT)
      {0x01013fd6,  ASCF_PRIV, 0, NULL, AEE01013fd6_New},
#ifdef FEATURE_STATIC_RAPTOR
      {0x01012eff,  ASCF_PRIV|ASCF_UPGRADE, 0, NULL, BnmRm_New},
#endif
#endif
#endif
#if defined(FEATURE_BREW_JOYSTICK)
   {AEECLSID_IJOYSTICK1,          (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, AEEJoystick_New},
   {AEECLSID_IJOYSTICK2,         (ASCF_PRIV | ASCF_UPGRADE), 0, NULL, AEEJoystick_New},
#endif
// YCbCrFactory Static Extension
#if defined (FEATURE_IYCBCR)
   {AEECLSID_YCBCR, ASCF_PRIV, 0, NULL, IYCbCrFactory_New},
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
#if defined(FEATURE_GRAPHICS_OPENGLES_CL)
   gAEEOpenGLESClasses,
#endif
#ifndef WIN32
#if defined (FEATURE_QCT_JPEG_DECODER)
   gAEEJpegClasses,
#endif
#endif
   NULL
};

//lint -restore

