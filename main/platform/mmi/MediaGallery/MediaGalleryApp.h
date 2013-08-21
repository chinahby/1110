#ifndef  MEDIAGALLERYAPP_H
#define  MEDIAGALLERYAPP_H
/**--------------------------------------------------------------------------
 * MediaGalleryApp.h -- define structure,enumerate,macro, const and
 *                      typedef for media gallery applet.
 *
 * Copyright(c) 2008 Anylook
 *
 * History:
 *
 * when         who            what and why
 * -----------  -----------    -----------------------------
 * 2008.10.11       Create.
 *
 **--------------------------------------------------------------------------
 */
#ifndef AEE_SIMULATOR
#include "OEMFeatures.h" //Handset Feature Definitions
#else
#include "BREWSimFeatures.h" //Simulator Feature Definitions
#endif//AEE_SIMULATOR

#include "AEEStdLib.h"
#include "AEEShell.h"
#include "AEE_OEMEvent.h"
#include "AEEModTable.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEText.h"
#include "AEEImageCtl.h"
#if defined(BREW_STATIC_APP)
#include "OEMClassIDs.h"
#endif//defined(BREW_STATIC_APP)
#include "AEEOS.h"
#include "AEEMediaFormats.h"
#include "AEEMediaMPEG4.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEUSBDriver.h"
#include "AEEAnnunciator.h"

#include "Appscommon.h"
#include "appscommon.brh"
// pictures resource
#include "appscommonimages.brh"

#ifdef FEATURE_ANICTL
#include "AEEDrawAni.h"
#endif


//Add By zzg 2010_11_05
#ifdef FEATURE_BT_SEND_FILE_ONLY
//#include "BTApp_priv.h"		//"BTApp.h"
#endif
//Add End

#include "BATT_NOTIFIER.BID"
#include "AEEBatt.h"
#include "MediaGallery.h"
#include "MediaGalleryExplorer.h"
#include "Mediagallery.brh"
#include "mediagallery_image.brh"
#include "OEMCFGI.h"
#include "OEMNV.h"


#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
#define MEDIAGALLERYAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif//FEATURE_LCD_TOUCH_ENABLE
/*===========================================================================
 *                      TYPE DECLARATIONs
 * ==========================================================================
 */
typedef  struct  _CMediaGalleryApp CMediaGalleryApp;

typedef boolean (*MG_PFN_ITEMTYPECHECK)(CMediaGalleryApp *pMe,
                                        MGFileInfo *pItemInfo);

/*A return value from the state handler to the main state processing function.*/
typedef enum _NextFSMAction
{
    NFSMACTION_WAIT,
    NFSMACTION_CONTINUE
} NextFSMAction;

// Media Gallery applet states...
// NOTE:for each state definition added to this enum, there must be a
// corresponding state handler defined under MediaGalleryApp_ProcessState in
// MediaGalleryAppStateHandler.c. In otherwords, this enum table must be
// synchronized with the MediaGalleryApp_ProcessState function.
typedef enum _MGAppState
{
   STATE_NONE = 0, //No state.
   STATE_INIT,     //Inital state.
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   STATE_PWD,	   //Application lock
#endif	
   STATE_EXIT,     //State where we close the applet.
   STATE_MAINMENU, //entry menu state
   STATE_PHONEMEM, //state that on phone memory menu
   STATE_CARDMEM,  //state that on card memory menu
   STATE_MEMSTAT,  //state that on memory status
   STATE_UDISK,    //state to operater u disk.
   STATE_MEDIAMENU,  // list pictures & do various operations

   STATE_COPY,     //state to copy file(s)
   STATE_DELETE,   //state to do delete
   STATE_DETAIL,   //state to show file information

   STATE_PLAY,     //state toplay audio/video
   STATE_VIEW,     //state to view a picture
   STATE_RENAME,   //state to rename file name
   STATE_SETAS,    //state to set the current file as wallpaper if a picture, or ringtone if a sound
   STATE_SORT,     //state to set sort type for media menu list

   STATE_SELECTPATH,

   STATE_MUSIC_ADD,
   STATE_VIDEO_ADD,
   STATE_IMAGE_SETTING,
   STATE_MUSIC_SETTING,
   STATE_SAVETOPLAYLIST,
   STATE_MAX            //maximum state.
} MGAppState;

/*Media Gallery DiaLoG RETurn VALUE*/
typedef enum _MGDLGRetValue
{
   MGDLGRET_NONE,
   MGDLGRET_CREATE,   // Initial value indicates that the dialog needs to be created.
   MGDLGRET_OK,       // General dialog response
   MGDLGRET_CANCELED, // General dialog response

   MGDLGRET_MSGBOX_OK,// Message box response
   MGDLGRET_YES,      // Returned by YESNO dialog
   MGDLGRET_NO,       // Returned by YESNO dialog
   MGDLGRET_ERR,
   MGDLGRET_UDISKSTART,
   MGDLGRET_UDISKSTOP,
   MGDLGRET_NOEXTCARD,    //prompt that there are no SD / MMC insert into handset
   MGDLGRET_NOUSBCABLE,   //prompt that there are no USB cable connect
   MGDLGRET_USBConnect,
   MGDLGRET_USBNotConnect,
   MGDLGRET_NOFILES,      //there is no record or no file
   MGDLGRET_DIRNOTEMPTY,  //the directory is empty
   MGDLGRET_BGPLAYPROMPT, //When mp3 player OR fm play on the background.

   MGDLGRET_DONE,         //Operation successfully
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   MGDLGRET_PASS,
   MGDLGRET_FAILD,
#endif
   /*ALL MENU DIALOG*/
   MGDLGRET_MAINMENU,     //main menu dialog
   MGDLGRET_PHONEMEM,     //phone memory dialog, which explore multi-media file in the handset
   MGDLGRET_CARDMEM,
   MGDLGRET_MEMSTAT,
   MGDLGRET_UDISK,

   MGDLGRET_MEDIAMENU,

   /*dialog return value for various operation*/
   MGDLGRET_VIEW,
   MGDLGRET_PLAY,
/*    MGDLGRET_WALLPAPER,
 */
   MGDLGRET_SETAS,
   MGDLGRET_RENAME,
/*    MGDLGRET_DELETE,
 *    MGDLGRET_DELALL,
 *    MGDLGRET_BLUETOOTH,
 *    MGDLGRET_COPY,
 *    MGDLGRET_MOVE,
 */
   MGDLGRET_SORT,
   MGDLGRET_DETAIL,
   MGDLGRET_SELECTPATH,

   MGDLGRET_FILEADD_FINISH,
   MGDLGRET_SETTING_SELECT,
   MGDLGRET_SAVETOPLAYLIST,
   MGDLGRET_MAX
} MGDLGRetValue;

typedef enum MGStartMode {
   MGSM_NORMAL_EXPLORER,
   MGSM_FILE_SELECT,
   MGSM_FILE_BROWSE,
   MGSM_VIDEO_BROWSE,   /*browse video files which take by camera*/
   MGSM_PHOTO_BROWSE,   /*browse photo which take by camera*/
   MGSM_MUSIC_ADD,      /*add music files to play list by music player*/
   MGSM_VIDEO_ADD,      /*add video files to database which used as play list by video player*/
   MGSM_RECORD_VIEW,
   MGSM_IMAGE_SETTING,  /*list files that can use as wallpaper or screensaver*/
   MGSM_MUSIC_SETTING,  /*list files that can use as various ringtone*/

   MGSM_SETTING_WALLPAPER, /*set a picture as wallpaper*/
   MGSM_SETTING_RINGTONE,  /*set a sound/music file as various ringtone ,such as call, or alarm*/
}MGStartMode;

typedef enum MGDLGLevel{
   MGDLG_LEVEL_MAIN = 0,
   MGDLG_LEVEL_MEDIUM = 1,
   //MGDLG_LEVEL_MEDIA = 2,
   MGDLG_LEVEL_MAX,
}MGDLGLevel;

/**********************macro & typedef for memory status*********************/
/*storage medium*/
#define MG_STMED_HANDSET          0X01
#define MG_STMED_MASSCARD         0X02
#define MG_STMED_ALL              0X04

#define MG_CONVERT_CRITICAL       1024
#define MG_CONVERT_SHIFTBITS      20
#define MG_CONVERT_DATALEN        20
#define MG_MEMUNIT_STRLENMAX      32
#define MG_MEMSTAT_ITEMMAX        3

/*unit for memory space*/
typedef enum MG_MEMUNIT{
   MG_MEMUNIT_BYTE = 0,//B
   MG_MEMUNIT_KILO = 1,//KB
   MG_MEMUNIT_MEGA = 2,//MB
   MG_MEMUNIT_GIGA = 3,//GB
   MG_MENUNIT_MAX,
}MG_MEMUNIT;

/*struct describe memroy status*/
typedef struct MG_MemStat{
   uint32 m_dwTotal; /*total capacity*/
   uint32 m_dwFree;  /*free capacity*/
   uint32 m_dwUsed;  /*used space */
}MG_MemStat;

/**************************macro & typdef for media list menu****************/
/*enumeration for media dialog status*/
typedef enum _MediaDialogStat
{
   MG_DLGSTAT_INIT = 0,
   MG_DLGSTAT_NORMAL,/*main interface of dialog, such as list variou media
                          files*/
   MG_DLGSTAT_POPUP,    /*pop up options menu*/
   MG_DLGSTAT_MESSAGEBOX,/*display message box, auto close in 2 seconds*/
   MG_DLGSTAT_YESNOBOX, /*display message to let user select*/
   MG_DLGSTAT_PROGRESS, /*display progressbar, close when the task complete*/
   MG_DLGSTAT_IMGZOOM,
   MG_DLGSTAT_IMGMOVE,
   MG_DLGSTAT_UNKNOWN,
#ifdef FEATURE_USES_MMS
   MG_DLGSTAT_FORMMS,
#endif   
   MG_DLGSTAT_MAX,
}MediaDlgStat;

/*media gallery call back update*/
#define MG_CBT_NULL				0x00
#define MG_CBT_TESTCARD0		0x01 /*test external SD card whether exist*/
#define MG_CBT_STARTUDISK		0x02 /*to start up U-Disk function*/
#define MG_CBT_BUILDMENU		0x03
#define MG_CBT_BUILDMENUEND	0x04
#define MG_CBT_BUILDDATABASE	0x05
#define MG_CBT_RELEASEMENU		0x06
#define MG_CBT_GETFOLDERINFO  0x07 /*get folder information*/

/*call back status*/
#define MG_CBS_NULL           0x01 /*call back do nothing*/
#define MG_CBS_DOING          0x02 /*call back still under doing*/
#define MG_CBS_DONE           0x03 /*call back have done */
#define MG_CBS_STOP           0x04 /*cancel when applet suspend*/
#define MG_CBS_NOTDOFOR       0x05 /*the call back does not do for inquery thing*/
#define MG_CBS_CANDOFOR       0x06

/*enumerate file result*/
#define MG_ENUM_UNKNOWN			0x00
#define MG_ENUM_SUCCESS			0x01
#define MG_ENUM_FAILED			0x02
#define MG_ENUM_OVERMAX			0x03

/*Handle type when encount empty media menu*/
#define MG_ONMENUEMPTY_MESSAGEBOX   0X01
#define MG_ONMENUEMPTY_STAYEMPTY    0X02
#define MG_ONMENUEMPTY_RETURNUP     0X03

/*Operation*/
#define MG_OP_NULL         0x0000
#define MG_OP_DEL          0x0001
#define MG_OP_DELALL       0x0002
#define MG_OP_VIEWIMG      0x0003
#define MG_OP_PHOTOSLIDE   0x0004
#define MG_OP_ZOOM         0x0005
#define MG_OP_WALLPAPER    0x0006
#define MG_OP_PLAYMUSIC    0x0007
#define MG_OP_PLAYVIDEO    0x0008
#define MG_OP_RENAME       0x0009
#define MG_OP_DETAIL       0x000A
#define MG_OP_COPY         0x000B
#define MG_OP_MOVE         0x000C
#define MG_OP_SORT         0x000D
#define MG_OP_SELECT       0x000E

#ifdef FEATURE_USES_MMS
#define MG_OP_FORMMS_IMAGE 0x000F
#endif
/*Handle type for finish operation*/
#define MG_FNSHOP_NULL   0X00000000 /*Do default handle*/
#define MG_FNSHOP_DONE   0X00000001 /*Show Done message*/
#define MG_FNSHOP_ERROR  0X00000010
/*message box ID*/
typedef enum _MediaDialogMsgID
{
   MG_MSGID_NULL = 0x0,
   MG_MSGID_MP3PLAYBACKGROUND,
   MG_MSGID_FMPLAYBACKGROUND,
   MG_MSGID_OVERWRITE,
   MG_MSGID_DEL,/*prompt user to confirm whether delete file used for
                  wallpaper or ringtone*/
   MG_MSGID_DELALL,
   MG_MSGID_NOFILE,
   MG_MSGID_COPY,
   MG_MSGID_MOVE,

   MG_MSGID_ENUMMAX,

   MG_MSGID_RESORT, /*after change sort type, resorting media menu*/
   MG_MSGID_BUILDVIDEOLIST,

   MG_MSGID_DONE,
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   MG_MSGID_VALIDPWD,
#endif   
   MG_MSGID_INVALIDNAME,
   MG_MSGID_NULLNAME,
   MG_MSGID_REPETITIONNAME,
}MediaDiaMsgID;

/*item that insert into a menuctl, in order to build an insert items array*/
typedef struct _MenuInsItem
{
   uint16   nItemID; /*the item identity(index)*/
   boolean  bInsert; /*some time, item in array do not really inset into*/
}MenuInsItem;

/*struct use for copy file*/
typedef struct _CMGFileCopy
{
   char   *m_pszSrcName;   /*file name (include path and base name) of source*/
   char   *m_pszDestName;  /*file name (include path and base name) of destination*/
   IFile  *m_pSrcFile;
   IFile  *m_pDestFile;
   void   *m_pBuffer;     /*buffer for copy*/
   uint32 m_dwBufLen;     /*buffer count in byte*/
   uint32 m_dwCopyCount;  /*need copy count in byte, in actually it is the source
                           *file size, add this field for overwrite */
}CMGFileCopy;
typedef struct _CPlaylist
{
  char  pPlaylistName[128];//用来存储不包括路径名和扩展名的播放列表文件名
}CPlaylist;
typedef struct _FileName{
   char m_szFileName[128];
}FileName;
typedef struct _CMusiclist
{
  char pMusicName[257];//用来存储包括路径名和扩展名的音乐文件名
}CMusiclist;

/*MediaGallery Applet object structure*/
struct  _CMediaGalleryApp{
   DECLARE_VTBL(IMediaGallery)
   AEECLSID          clsID;
   uint32            m_nRefs;//reference count
   IModule           *m_pModule;
   IDisplay          *m_pDisplay;
   IShell            *m_pShell;

   AEERect           m_rc;
   AEERect           m_ClipRect;
   boolean           m_bSuspending;/*True if EVT_APP_SUSPEND as been received*/
   boolean           m_USBConnect;
   boolean           m_STOPUSB;
     
   MGStartMode       m_StartMode;

   /* Overwrite the dialog result? general is FALSE, need overwrite*/
   boolean           m_bNotOverwriteDlgRet;
   MGDLGRetValue     m_eDlgRet;/* Result code returned when the active dialog closes*/
   MGAppState        m_ePreState;/*previous applet state*/
   MGAppState        m_eCurState;/*current applet state*/
   IDialog           *m_pActiveDlg;/* Pointer to the currently active dialog*/
   uint16            m_nActiveDlgID;/*Active dialog ID*/

   // To save the menu selection for the previously active menu
   // Used to restore the menu selection while switching back to the previous
   // screen.
   uint16            m_nPrevMenuItemSel[MGDLG_LEVEL_MAX];
   uint16            m_nMediaMenuId;/*identity of the current menu control of media file*/
   uint16            m_nMediaMenuDlgId;/*identity of dialog the menu control set*/
   uint16            m_nMsBoxId;/*identity of the message box, especial yes-no & progress type*/
   uint16	         m_nMsgBoxTextId;/*String resource to use for the Msgbox text */
   PromptMsg_e_Type  m_eMsgBoxType; /*message box type */
   BottomBar_e_Type  m_eMsgBoxBottomBar;/*softkey bar type */
   const char        *m_pszMsgBoxResFile;/*String resource file name*/

   MediaDlgStat      m_MediaDlgStat;/* indicate status of dialogs that lis
                                       media files, list files, popup options
                                       menu or message box*/

   boolean           m_bCardExist;/*Is external card exist*/
   boolean           m_bKeepMediaMenu;/*do not free media menu item data , release menu?*/
   boolean           m_bMediaMenuEmpty;/*if media menu empty*/

   boolean           m_bUpdateSelItem;
   uint8			      m_nEnumeResult;
   uint8             m_StoreMedium;/*storage medium*/
   uint32            m_nListIdx;
   IVector           *m_pFolderList;
   IMenuCtl          *m_pPathMenu;

   IMenuCtl          *m_pMenuPopup;/*menu cnotrol for pop up menu*/
   IMenuCtl          *m_pMediaMenu;/*menu control for list variou media file*/
   IFileMgr          *m_pFileMgr;/*file manager interface*/
   AECHAR            *m_pBuffer;
   //用于保存播放列表
   CPlaylist         m_Playlist[10];
   CMusiclist  		 m_Musiclist[50];
   uint8             m_nPlaylistNum;
   uint8             m_nPlaylistMusicNum;
   CFSExplorer       m_Explorer; /*struct for enumerate files & folders*/
   ExplorerPara      *m_pExpPara;

   CFSExplorer       *m_pPathExp;

   CFileIcons        *m_pFileIcons;
   MGFolderInfo      *m_pFolderInfo;

   FileNamesBuf      m_pszNameList;
   uint16            m_nSelNum;/*file conunt in name list send to the other
                                 applet  through callback function when
                                 fininsh explorer */
   int16             m_PopupOps;/*the select operation on popup menu*/
   int16             m_ImgViewOps;

   CMGFileCopy       m_Copy;   /*struct for copy/move file*/

   AEECallback       m_CallBack;
   PFNNOTIFY         m_pfnNotify;
   boolean           m_bCallbackResumed;/*whether the call back have been resumed */
   boolean           m_bCallbackDoing;/*in progress that add file information to media menu*/
   uint8			      m_nCallbackDoFor;

   AECHAR            *m_pInputFileName;/*To store input when applet suspend in edit status*/
   boolean           m_bHaveExt;  /*the file have extension name, for rename operation*/

   IDatabase         *m_pFilesDB; /*Database pointer*/
   IStatic		      *m_pDetailText;

   IMedia            *m_pMedia;    // IMedia-based object
   void              *m_pMediaSpec;
   uint32            m_dwDuration;
   boolean           m_bGotMediaInfo;
   uint16            m_nRingVolume;/*restore ring volume when listen music*/
   byte              m_nKeyBeepVol;/*when listen mp3 which may set as ringtone
                                     under music setting, prohit key beep,
                                     then recover when approprite*/
   IImage            *m_pImage;
   AEEImageInfo      m_ImgInfo;
   boolean           m_Gif;
   
#ifdef FEATURE_ANICTL
   IAniCtl           *m_pAniCtl;
#endif
   boolean           m_bImgLoadDone;
   boolean           m_videoLoadDone;
   IAnnunciator        *m_pIAnn;
#ifdef FEATURE_USES_MMS
   boolean           m_isForMMS;
#endif
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   char              *m_strPhonePWD;
#endif
} ;

/*===========================================================================
 *
 *                Macro and constant
 *
 *===========================================================================
 */
#define MGRES_LANGFILE (AEE_RES_LANGDIR MEDIAGALLERY_RES_FILE)
//
//Conditional Debug Output
//
#define FARF_STATE   TRUE
#define FARF_EVENT   TRUE
#define FARF_ADDR    TRUE
#ifdef MG_FARF
#undef MG_FARF
#endif

#define MG_FARF(x, p)


/*-----------------------------------------------------------------------------
 *  user define error code
 *-----------------------------------------------------------------------------*/
#define  ERROR_DEEPDIR      (ERROR_USER + 1) //the directory too deep.


/*-----------------------------------------------------------------------------
 *  helper macro
 *-----------------------------------------------------------------------------*/
#define MG_SENDEVENT AEE_Event

#define MG_CLEAR_COPYDATA(sCopy) \
   do{ \
      RELEASEIF(sCopy.m_pSrcFile);\
      RELEASEIF(sCopy.m_pDestFile);\
      FREEIF(sCopy.m_pBuffer);\
      FREEIF(sCopy.m_pszSrcName);\
      FREEIF(sCopy.m_pszDestName);\
   }while(0)


/*-----------------------------------------------------------------------------
 *  constant define
 *-----------------------------------------------------------------------------*/
#define MG_MENUITEM_NULL      0xFFFF
#define MG_TEXTID_INVALID     0xFFFF
#define IDS_MG_CUSTOM         0xFFFF
#define MG_VECTOR_ALL        (uint32)(~1)
#define MG_FS_PADD            0x00001000UL
#define MG_FS_BUFFER          0x00010000UL
#define MG_FILE_MIDDLETHRESHOLD 0x000F0000UL
#define MG_FILE_LARGETHRESHOLD   0x00F00000UL
#define MG_VECTOR_INCSTEP     (MG_MAX_FILESPERDIR) / 16

#define MG_PREFS_VER          0x0001
#define MG_MAX_UINT16         0xFFFF
#define MG_MAX_BYTE           0xFF
#ifdef FEATURE_VERSION_M8P
#define MG_TEST_JPEG          "fs:/image/Gambar/bca164478.jpg"
#define MG_TEST_PNG           "fs:/image/Gambar/Background1.png"
#define MG_TEST_BMP           "fs:/image/Gambar/MShopLogo.bmp"
#else
#define MG_TEST_JPEG          "fs:/image/pictures/bca164478.jpg"
#define MG_TEST_PNG           "fs:/image/pictures/Background1.png"
#define MG_TEST_BMP           "fs:/image/pictures/MShopLogo.bmp"
#endif
#define MG_TEST_MUSIC         "fs:/sounds/Sample"
#define MG_TEST_COPYMOVEDEST  "fs:/test/dest"
/*===========================================================================
 *
 *               Function Declarations
 *
 *===========================================================================
 */
/*===========================================================================
 * FUNCTION:MediaGalleryApp_ShowDialog
 * DESCRIPTION:This function is used by various state handlers to bring up
 *    one of the many media gallery dialogs. The calling function passes the
 *    dialog resource ID as a parameter to this function and this function
 *    creates the dialog.
 * RETURN VALUE:SUCCESS- create dialog
 *         EFAILED- fail to create dialog
 *===========================================================================
 */
int MediaGalleryApp_ShowDialog(CMediaGalleryApp* pMe, uint16 nDlgResId);


/*===========================================================================
 * FUNCTION:MediaGalleryApp_RouteDialogEvent
 * DESCRIPTION: Route a BREW event to the active dialog handler
 * RETURN VALUE:
 *   TRUE:If the dialog handler has processed the event.
 *   FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
boolean MediaGalleryApp_RouteDialogEvent(CMediaGalleryApp* pMe,
                                          AEEEvent eCode,
                                          uint16   wParam,
                                          uint32   dwParam);

/*===========================================================================
 * FUNCTION:MediaGalleryApp_ProcessState
 * DESCRIPTION: Looking at the intenal static state handler (by using the
 *      pMe->m_eCurState ), this function routes the call to the state handler.
 * RETURN VALUE:
 *    NFSMACTION_CONTINUE:To indicate that there are subsequent states to be
 *                      followed and hence the state machine should not be
 *                      stopped.
 *    NFSMACTION_WAIT:To indicate that the state machine can be suspended as we
 *                  are going to show a dialog to the user.
 *===========================================================================
 */
NextFSMAction MediaGalleryApp_ProcessState(CMediaGalleryApp* pMe);

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_GetAppPref
 * DESCRIPTION  :  获取App相关的设置，
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_GetAppPref(CMediaGalleryApp* pMe);

/*
 * ==========================================================================
 * FUNCTION     :  CMediaGallery_SetAppPref
 * DESCRIPTION  :  保存App相关的设置到文件中，保证App退出或者手机关机后，再进
 * 入App时能获取到这写设置值。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int32 CMediaGallery_SetAppPref(CMediaGalleryApp* pMe);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_DoExplorerAddCB
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MediaGallery_DoExplorerAddCB(FileNamesBuf pszName, uint16 nNum);

int MediaGalleryApp_RegisterBatteryNotify(CMediaGalleryApp *pMe);

int MediaGalleryApp_DeRegisterBatteryNotify(CMediaGalleryApp *pMe);

/*============================================================================
 * FUNCTION: MediaGalleryApp_RegisterKeyEvent
 * DESCRIPTION: register shell to receive key event notify
 * RETURN VALUE:
 *============================================================================
 */
int MediaGalleryApp_RegisterKeyEvent(CMediaGalleryApp *pMe);

/*============================================================================
 * FUNCTION: MediaGalleryApp_DeRegisterKeyEvent
 * DESCRIPTION: don't need to receive key event notify
 * RETURN VALUE:
 *============================================================================
 */
int MediaGalleryApp_DeRegisterKeyEvent(CMediaGalleryApp *pMe);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_RegisterPlayNext
 * DESCRIPTION  :
 * PARAMETERS   : bRegister -- TRUE , Register
 *                          -- FALSE, Deregister
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGalleryApp_RegisterPlayNext(CMediaGalleryApp *pMe,
                                          boolean bRegister);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_PlayNext
 * DESCRIPTION  :  tell player to play previous/current/next music or video
 * PARAMETERS   :  nPlayOn -- MG_PLAY_NEXT , forward
 *                         -- MG_PLAY_CUR ,  current
 *                         -- MG_PLAY_PREV,  backward
 * RETURN VALUE :
 * ==========================================================================
 */
 #define MG_PLAY_PREV 1
 #define MG_PLAY_CUR  2
 #define MG_PLAY_NEXT 3

boolean MediaGalleryApp_PlayNext(CMediaGalleryApp *pMe, uint8 nPlayOn);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_RemovDir
 * DESCRIPTION  :  删除目录下的子目录和文件，也可能删除目录本身。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_RemovDir(IFileMgr *pIFlMr,
                        IVector *pIVec,
                        const char* pszDir,
                        boolean bDelRootDir);
/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_FreeVectorElements
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
__inline int32 MediaGalleryApp_FreeVectorElements(IVector* pVector);


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_InitBuildMediaMenu
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_InitBuildMediaMenu(CMediaGalleryApp* pMe, PFNNOTIFY pfnNotify);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_InitBuildFileList
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_InitBuildFileList(CMediaGalleryApp* pMe, PFNNOTIFY pfnNotify);


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_InitGetFolderInfo
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_InitGetFolderInfo(CMediaGalleryApp* pMe, PFNNOTIFY pfnNotify);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_InitBuildVideoDatabase
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_InitBuildVideoDatabase(CMediaGalleryApp* pMe,
                                     const char * pszDBFile,
                                     PFNNOTIFY pfnNotify);


boolean MGAppUtil_SetMediaDlgStat(CMediaGalleryApp *pMe, MediaDlgStat eSetStat);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_UpdateMediaMenuTitle
 * DESCRIPTION  :  更新Media menu菜单的标题，例如，移动焦点时。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
__inline int MGExplorer_UpdateMediaMenuTitle(CMediaGalleryApp *pMe,
                                             IMenuCtl *pMenuCtl);

/* ---------------------------------------------------------------------------
 *       有关文件浏览结构体的函数简单封装，以方便调用
 * ---------------------------------------------------------------------------
 */

static __inline MGFileInfo *MediaGalleryApp_GetCurrentNode(CMediaGalleryApp *pMe)
{
   if(NULL == pMe)
      return NULL;

   return MGExplorer_GetCurrentNode(&pMe->m_Explorer);
}//MediaGalleryApp_GetCurrentNode

static __inline boolean MediaGalleryApp_SetCurrentNode(CMediaGalleryApp *pMe,
                                                         MGFileInfo  *pNode)
{
   if(NULL == pMe)
      return FALSE;

   return MGExplorer_SetCurrentNode(&pMe->m_Explorer, pNode);
}//MediaGalleryApp_SetCurrentNode

static __inline char *MediaGalleryApp_GetCurrentNodeName(CMediaGalleryApp *pMe)
{
   if(NULL == pMe)
      return NULL;

   return MGExplorer_GetCurrentNodeName(&pMe->m_Explorer);
}//MediaGalleryApp_GetCurrentNodeName

static __inline boolean MediaGalleryApp_SetExplorerMime(CMediaGalleryApp *pMe,
      uint16 nMime)
{
   if(NULL == pMe)
      return FALSE;

   return MGExplorer_SetMime(&pMe->m_Explorer, nMime);
}//MediaGalleryApp_SetExplorerMime

static __inline uint16 MediaGalleryApp_GetExplorerMime(CMediaGalleryApp *pMe)
{
   if(NULL == pMe)
      return MG_MIME_UNKNOWN;

   return MGExplorer_GetMime(&pMe->m_Explorer);
}//MediaGalleryApp_GetExplorerMime

static __inline uint8 MediaGalleryApp_GetCurrentDepth(CMediaGalleryApp *pMe)
{
   if(NULL == pMe)
      return MG_CURDEPTH_INIT;

   return MGExplorer_GetCurrentDepth(&pMe->m_Explorer);
}//MediaGalleryApp_GetCurrentDepth

static __inline boolean MediaGalleryApp_SetCurrentDepth(CMediaGalleryApp *pMe,
                                                        uint8 nDepth)
{
   if(NULL == pMe)
      return FALSE;

   return MGExplorer_SetCurrentDepth(&pMe->m_Explorer, nDepth);
}//MediaGalleryApp_SetCurrentDepth


/* ---------------------------------------------------------------------------
 *          有关回调结构体的状态查询
 * ---------------------------------------------------------------------------
 */
void MediaGalleryApp_SetCallbackStateStart(CMediaGalleryApp *pMe,
                                           uint8 nDoType);

uint8 MediaGalleryApp_GetCallbackState(CMediaGalleryApp *pMe,
                                       uint8 nInqueryType);
int MediaGalleryApp_SetCallbackStateStop(CMediaGalleryApp *pMe);
boolean MediaGalleryApp_IsCallbackResumed(CMediaGalleryApp *pMe);
int MediaGalleryApp_SetCallbackResume(CMediaGalleryApp *pMe,
                                      boolean bResume);
int MediaGalleryApp_SetCallbackStateDone(CMediaGalleryApp *pMe);

/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_UpdateCurrentMediaMenuItem
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_UpdateCurrentMediaMenuItem(CMediaGalleryApp *pMe,
                                          const char *pNewFilePath);



/*
 * ==========================================================================
 * FUNCTION     :  MGMediaInfo_GetForderFromMimeBase
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGMediaInfo_GetForderFromMimeBase(uint16 uMimeBase,
                                      uint8 nMedium,
                                      char const ** cppszPath);


/*
 * ==========================================================================
 * FUNCTION     :  MGExplorer_MediaMenuSort
 * DESCRIPTION  :  对菜单进行重新排序
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_MediaMenuSort(CMediaGalleryApp *pMe,
                              IMenuCtl *pMenuCtl,
                              MGSortOrder eOrder);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_RecoverMediaMenu
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
int MGExplorer_RecoverMediaMenu(CMediaGalleryApp *pMe,  IMenuCtl *pMenuCtl);


/* ---------------------------------------------------------------------------
 *     SD卡和U盘相关的函数。
 * ---------------------------------------------------------------------------
 */
#ifdef FEATURE_SUPPORT_VC0848
boolean MediaGalleryApp_CheckSDCard(void);

void MediaGalleryApp_StopSDCard(void);
#endif//ifdef FEATURE_SUPPORT_VC0848

boolean MediaGallery_StartUDisk(CMediaGalleryApp *pMe);

boolean MediaGallery_StopUDisk(CMediaGalleryApp *pMe);

/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_HideAnnunciatorBar
 * DESCRIPTION  :  是否隐藏状态栏（屏幕顶上的）
 * PARAMETERS   : bHide -- TRUE    隐藏
 *                      -- FALSE   显示
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGallery_HideAnnunciatorBar(IDisplay *pdisp,
                                       AEERect *pClipRect,
                                       boolean bHide);
/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_SetRingVolumn
 * DESCRIPTION  :  设置铃声预览时播放的音量，与情景模式相关。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGallery_SetRingVolumn(CMediaGalleryApp *pMe);


/*
 * ==========================================================================
 * FUNCTION     :  MediaGallery_TestCard0
 * DESCRIPTION  :  callback function, to detect whether SD card insert
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
void MediaGallery_TestCard0(void *po);


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_RedrawInactiveMenu
 * DESCRIPTION  :  对于属性设置了MP_NO_REDRAW的Menu，当其出于非活动状态时重画
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline void MediaGalleryApp_RedrawInactiveMenu(IMenuCtl *pMenuCtl)
{
   if (NULL == pMenuCtl)
   {
      return;
   }

   if ( FALSE == IMENUCTL_IsActive(pMenuCtl) )
   {
      /*把Menu暂时设为active是为了画焦点条*/
      IMENUCTL_SetActive(pMenuCtl, TRUE);
      IMENUCTL_Redraw(pMenuCtl);
      /*deactive Menu 时会模糊菜单区域, 不需要redraw， */
      IMENUCTL_SetActive(pMenuCtl, FALSE);
   }

}//MediaGalleryApp_RedrawInactiveMenu


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SetMsgBoxID
 * DESCRIPTION  :  设置MessageBox的ID（identity) --标识符
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline void MediaGalleryApp_SetMsgBoxID(CMediaGalleryApp *pMe,
                                                   uint16 nMsgBoxID)
{
   if(!pMe)
      return;

   pMe->m_nMsBoxId = nMsgBoxID;
}//MediaGalleryApp_SetMsgBoxID


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_GetMsgBoxID
 * DESCRIPTION  :  获取消息框的标识符（ID）
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline uint16 MediaGalleryApp_GetMsgBoxID(CMediaGalleryApp *pMe)
{
   if(!pMe)
      return MG_MSGID_NULL;

   return pMe->m_nMsBoxId;
}//MediaGalleryApp_GetMsgBoxID

#endif// MEDIAGALLERYAPP_H
