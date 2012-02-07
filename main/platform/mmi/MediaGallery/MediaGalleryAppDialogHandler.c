/**--------------------------------------------------------------------------
 * MediaGalleryAppDialogHandler.c -- This file implements all media gallery 
 * dialog event handlers which are uesed by the applet to bring up various
 * scenario to the end user.
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
#include "MediaGalleryApp.h"
#include "AppComFunc.h"
#include "AEEMediaUtil.h"
#include "MusicPlayer.h"
#if defined(FEATURE_VIDEOPLAYER)
#include "VideoPlayer.h"
#endif
#include "AEEAnnunciator.h"
#ifdef WIN32
#include "oemhelperfunctype.h"
#endif//WIN32
#ifdef FEATURE_MG_WAPMMS_SUPPORT
#include "simutils.h"
#endif
#include "Msg.h"

//Add By zzg 2010_11_04
#ifdef FEATURE_BT_SEND_FILE_ONLY
//#include "BTApp_priv.h"		//"BTApp.h"
#include "Btapp.brh"
#endif
//Add End

/*===========================================================================
 *                      MACRO DECLARATIONs
 * ==========================================================================
 */
/* Macro for end current dialog */
#define MGCLOSE_DIALOG(DlgRet)               \
        {                                    \
           pMe->m_eDlgRet = DlgRet;          \
           ISHELL_EndDialog(pMe->m_pShell);  \
        }

/*Macro for add an item into no icon pop up menuctl*/
#define MGMENU_ADDITEM(pMenu, ID)            \
         IMENUCTL_AddItem((pMenu), (MGRES_LANGFILE), (ID), (ID), 0, 0)

/*===========================================================================
 *                      TYPE DECLARATIONs
 * ==========================================================================
 */
typedef boolean (*MG_PFN_DIALOG_HANDLEEVENT)(CMediaGalleryApp* pMe,
                                          AEEEvent eCode,
                                          uint16   wParam,
                                          uint32   dwParam);

/*===========================================================================
 *
 *                      FUNCTION DECLARATIONS
 *
 * ==========================================================================
*/
static void MediaGalleryApp_MsgBoxTimeout(void* pUser);
static boolean MediaGalleryApp_CancelMsgBoxTimer(CMediaGalleryApp* pMe,
                                                 boolean bTimeOut);
static int MGAppUtil_BuildPopupMenuItems(CMediaGalleryApp* pMe,
                                         IMenuCtl* pMenuCtl);
static int MGAppUtil_CreatePopupMenu(IMenuCtl** ppIMenu,
                                     MenuInsItem* pItemList,
                                     uint16 nItemNum,
                                     uint32 dwProps,
                                     int bottomBar_e_Type);
static void MGAppUtil_GetMediaMenuSelectItem(void* po);

static __inline void MGAppUtil_DrawSoftkey(CMediaGalleryApp* pMe,
                                           BottomBar_e_Type eSoftkeyType);

static void MGAppUtil_DrawSoftkeyOverImage(CMediaGalleryApp* pMe,
                                           const char *pszResFile,
                                           uint16 nRightTxtID,
                                           uint16 nMidTxtID,
                                           uint16 nLeftTxtID);
static boolean MGAppUtil_UpdateMediaMenuSoftkey(CMediaGalleryApp* pMe);
static boolean MGAppUtil_UpdateImgViewerSoftkey(CMediaGalleryApp* pMe);
static boolean MGAppUtil_UpdateTxtEditSoftkey(CMediaGalleryApp *pMe,
                                              ITextCtl *pEditText);

static __inline int MGAppUtil_DrawTitleArrow(CMediaGalleryApp* pMe,
                                             AEERect *prc);

static __inline void MGAppUtil_SetMenuCtlRectProp(CMediaGalleryApp* pMe,
                                                  uint32 dwProps,
                                                  IMenuCtl* pMenuCtl);
static __inline boolean MGAppUtil_CreateMediaMenu(CMediaGalleryApp *pMe,
                                                  IMenuCtl **ppMenuCtl);
static __inline boolean MGAppUtil_StartMediaMenu(CMediaGalleryApp* pMe,
                                                 IMenuCtl* pMenuCtl,
                                                 AEEEvent eCode,
                                                 uint16 wParam,
                                                 uint32 dwParam);
static void MGAppUtil_MediaMenuBuildComplete(void *po);
static void MGAppUtil_FileListBuildComplete(void *po);
static void MGAppUtil_VideoDatabaseBuildComplete(void *po);
static void MGAppUtil_GetFolderInfoNotify(void *po);

static uint16 MGAppUtil_GetMediaDlgStat(CMediaGalleryApp *pMe,
                                        MediaDlgStat *pGetStat);
static boolean MGAppUtil_IsNeedHandleKey(CMediaGalleryApp *pMe, uint16 nKey);
static __inline boolean MGAppUtil_OnMediaMenuClrKeyEvt(CMediaGalleryApp* pMe,
                                                       IMenuCtl* pMenuCtl);
static __inline boolean MGAppUtil_OnMediaMenuInfoKeyEvt(CMediaGalleryApp* pMe,
                                                        IMenuCtl* pMenuCtl,
                                                        AEEEvent eCode,
                                                        uint16 wParam,
                                                        uint32 dwParam);
static __inline boolean MGAppUtil_OnMediaMenuSelectKeyEvt(CMediaGalleryApp *pMe,
                                                          IMenuCtl* pMenuCtl,
                                                          AEEEvent eCode,
                                                          uint16 wParam,
                                                          uint32 dwParam);
static boolean MGAppUtil_OnMediaMenuDefaultKeyEvt(CMediaGalleryApp* pMe,
                                                  IMenuCtl* pMenuCtl,
                                                  AEEEvent eCode,
                                                  uint16 wParam,
                                                  uint32 dwParam);
static boolean MGAppUtil_OnMediaMenuDlgEnd(CMediaGalleryApp *pMe,
                                           IMenuCtl *pMenuCtl,
                                           uint16 wParam,
                                           uint32 dwParam);
static __inline boolean MGAppUtil_OnMediaMenuCommandEvt(CMediaGalleryApp* pMe,
                                                        IMenuCtl* pMenuCtl,
                                                        uint16 wParam,
                                                        uint32 dwParam);
static boolean MGAppUtil_OnMediaMenuSelChange(CMediaGalleryApp* pMe,
                                              uint32 eDlgStat);
static boolean MGAppUtil_UpdateSelItemCheck(CMediaGalleryApp *pMe);

static boolean MGAppUtil_OnMediaMenuMsgBoxTimeOut(CMediaGalleryApp* pMe,
                                                  IMenuCtl* pMenuCtl,
                                                  uint16 wParam,
                                                  uint32 dwParam);
static boolean MGAppUtil_OnMediaMenuEmpty(CMediaGalleryApp *pMe, uint8 nType);
static boolean MediaGalleryApp_ShowMsgBox(CMediaGalleryApp* pMe,
                                          const AECHAR *pwszText,
                                          const char * pszResFile,
                                          uint16 nTextResId,
                                          PromptMsg_e_Type eMsgBoxType,
                                          BottomBar_e_Type eMsgBoxBottomBar);
static __inline void MediaGalleryApp_ShowDoneMsgBox(CMediaGalleryApp *pMe);

static boolean MediaGalleryApp_MsgBoxDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                     AEEEvent eCode,
                                                     uint16 wParam,
                                                     uint32 dwParam);
static boolean MediaGalleryApp_MainMenuDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                       AEEEvent eCode,
                                                       uint16 wParam,
                                                       uint32 dwParam);
static boolean MediaGalleryApp_PhoneMemDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                       AEEEvent eCode,
                                                       uint16 wParam,
                                                       uint32 dwParam);
static boolean MediaGalleryApp_CardMemDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                      AEEEvent eCode,
                                                      uint16 wParam,
                                                      uint32 dwParam);
static boolean MediaGalleryApp_MemStatDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                      AEEEvent eCode,
                                                      uint16 wParam,
                                                      uint32 dwParam);
static boolean MediaGalleryApp_UDiskDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                    AEEEvent eCode,
                                                    uint16 wParam,
                                                    uint32 dwParam);
static boolean MediaGalleryApp_MediaMenuDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                        AEEEvent eCode,
                                                        uint16 wParam,
                                                        uint32 dwParam);

static boolean MediaGalleryApp_MusicAddDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                       AEEEvent eCode,
                                                       uint16 wParam,
                                                       uint32 dwParam);
static boolean MediaGalleryApp_VideoAddDlg_HandleEvent(CMediaGalleryApp *pMe,
                                                       AEEEvent eCode,
                                                       uint16 wParam,
                                                       uint32 dwParam);

static boolean MediaGalleryApp_ImageSettingDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                           AEEEvent eCode,
                                                           uint16 wParam,
                                                           uint32 dwParam);
static boolean MediaGalleryApp_SoundSettingDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                           AEEEvent eCode,
                                                           uint16 wParam,
                                                           uint32 dwParam);

static boolean MediaGalleryApp_OnDefaultOperate(CMediaGalleryApp* pMe,
                                                IMenuCtl* pMenuCtl,
                                                uint16 wParam,
                                                uint32 dwParam);

static boolean MediaGalleryApp_OnPopupMenuCommand(CMediaGalleryApp* pMe,
                                                  IMenuCtl* pMenuCtl,
                                                  uint16 wParam,
                                                  uint32 dwParam);

static boolean MGAppPopupMenu_OnImageViewer(CMediaGalleryApp* pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam);
static int MGAppPopupMenu_OnMusicPlay(CMediaGalleryApp* pMe,
                                      IMenuCtl* pMenuCtl);
#if defined(FEATURE_VIDEOPLAYER)
static int MGAppPopupMenu_OnVideoPlay(CMediaGalleryApp* pMe,
                                      IMenuCtl* pMenuCtl);
#endif
static boolean MGAppPopupMenu_OnOpenSubFolder(CMediaGalleryApp *pMe,
                                              MGFileInfo *pCurNode);
static int MGAppPopupMenu_OnCopy(CMediaGalleryApp* pMe,
                                 IMenuCtl* pMenuCtl);
static int MGAppPopupMenu_OnMove(CMediaGalleryApp *pMe,
                                 IMenuCtl *pMenuCtl);
static int MGAppPopupMenu_OnDelete(CMediaGalleryApp* pMe,
                                   IMenuCtl* pMenuCtl);
static int MGAppPopupMenu_OnDeleteAll(CMediaGalleryApp* pMe,
                                      IMenuCtl* pMenuCtl);
static int MGAppPopupMenu_OnBluetooth(CMediaGalleryApp* pMe,
                                      IMenuCtl* pMenuCtl);
static boolean MGAppPopupMenu_OnRename(CMediaGalleryApp* pMe,
                                       AEEEvent eCode,
                                       uint16 wParam,
                                       uint32 dwParam);
static boolean MGAppPopupMenu_OnSetWallpaper(CMediaGalleryApp *pMe,
                                             MGFileInfo *pItemData);
static boolean MGAppPopupMenu_OnSetAs(CMediaGalleryApp* pMe,
                                      AEEEvent eCode,
                                      uint16 wParam,
                                      uint32 dwParam);

static boolean MGAppPopupMenu_OnSavetoplaylist(CMediaGalleryApp* pMe,
                                      			AEEEvent eCode,
                                      			uint16 wParam,
                                      			uint32 dwParam);
static int MGAppUtil_BuildPlaylist(CMediaGalleryApp *pMe,IMenuCtl *pMenuCtl);
static int MGAppUtil_WriteMusiclist(CMediaGalleryApp *pMe , char *curFileName);
static int MGAppUtil_ReadMusiclist(CMediaGalleryApp *pMe , char *curFileName,boolean isPlaying);
static boolean MGAppPopupMenu_OnDetail(CMediaGalleryApp* pMe,
                                       AEEEvent eCode,
                                       uint16 wParam,
                                       uint32 dwParam);
static boolean MGAppPopupMenu_OnSort(CMediaGalleryApp* pMe,
                                     AEEEvent eCode,
                                     uint16 wParam,
                                     uint32 dwParam);
static boolean MGAppPopupMenu_OnSelectPath(CMediaGalleryApp *pMe,
                                           AEEEvent eCode,
                                           uint16 wParam,
                                           uint32 dwParam);
static __inline int MGAppPopupMenu_OperationDone(CMediaGalleryApp *pMe,
                                                 int nType);
static __inline int MGAppUtil_ScheduleRingTonePreview(CMediaGalleryApp *pMe);
static __inline int MGAppUtil_StopRingTonePreview(CMediaGalleryApp *pMe,
                                                  boolean bRelease);
static void MGAppUtil_PreviewRingtone(void *po);
static int MGAppUtil_ExplorerBuildSelItems(CMediaGalleryApp *pMe,
                                           IMenuCtl *pMenuCtl);
static int MGAppUtil_ExplorerGetSelectCount(CMediaGalleryApp *pMe,
                                            IMenuCtl *pMenuCtl);
static int MGAppUtil_RedrawImage(CMediaGalleryApp *pMe,
                                 AEEImageInfo *pInfo,
                                 boolean bResume);
static boolean MGAppUtil_DrawImageViewerBG(CMediaGalleryApp *pMe);
static void MGAppUtil_LoadMediaNotify(void * pUser,
                                      AEEMediaCmdNotify *pCmdNotify);
static void MGAppUtil_LoadImageNotify(void * pUser,
                                      IImage * po,
                                      AEEImageInfo * pi,
                                      int nErr);
static boolean MGAppUtil_SetWallpaper(CMediaGalleryApp *pMe,
                                      AEEImageInfo *pImgInfo);
static __inline boolean MGAppUtil_NextVideoCheck(CMediaGalleryApp *pMe,
                                                 MGFileInfo *pItemInfo);
static __inline boolean MGAppUtil_NextMusicCheck(CMediaGalleryApp *pMe,
                                                 MGFileInfo *pItemInfo);
static __inline boolean MGAppUtil_ImageSlideCheck(CMediaGalleryApp *pMe,
                                                  MGFileInfo *pItemInfo);
static __inline boolean MGAppUtil_ImageZoomCheck(CMediaGalleryApp *pMe,
                                                 MGFileInfo *pItemInfo);
static boolean MGAppUtil_ChangeMediaMenuItemByType(CMediaGalleryApp *pMe,
                                                   IMenuCtl *pMenuCtl,
                                                   boolean bDirection,
                                                   MG_PFN_ITEMTYPECHECK pfnCheck);
static boolean MGAppUtil_WallpaperSettingCheck(IImage *po,
                                               AEEImageInfo *pImgInfo,
                                               IShell *ps);
static int MGAppUtil_GetDestFilePath(const char *pszSrc,
                                     const char *pszDestDir,
                                     char *pszDestPath,
                                     uint16 nDestPathLen);
boolean MGAppUtil_InitFileCopy(CMediaGalleryApp* pMe);
static int MGAppUtil_DoCopyFile(CMediaGalleryApp *pMe);
static int MGAppUtil_HandleOperateFileError(CMediaGalleryApp* pMe,
                                            IFileMgr *pFileMgr);
static void MGAppUtil_StartUDisk(void *po);
static void CheckUSBCableConnect_HandleDialogTimer(void *pUser);

#ifdef FEATURE_USES_MMS
static boolean MGAppPopupMenu_OnSetMMSImage(CMediaGalleryApp *pMe,
                                             MGFileInfo *pItemData);
#endif
/*===========================================================================
 *                      Public Function Definitions
 *===========================================================================
 */

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_RouteDialogEvent
 * DESCRIPTION  :  事件分发函数，把App收到的事件分发到当前的Dialog处理函数。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGalleryApp_RouteDialogEvent(CMediaGalleryApp* pMe,
                                       AEEEvent eCode,
                                       uint16   wParam,
                                       uint32   dwParam)
{
   MG_PFN_DIALOG_HANDLEEVENT fcnPtr = NULL;

   if( NULL == pMe || NULL == pMe->m_pActiveDlg)
   {
      return FALSE;
   }
   MSG_FATAL("MediaGalleryApp_RouteDialogEvent m_nActiveDlgID=%d", pMe->m_nActiveDlgID,0,0);
   switch(pMe->m_nActiveDlgID)
   {
      case IDD_MG_MSGBOX:
         fcnPtr = MediaGalleryApp_MsgBoxDlg_HandleEvent;
         break;

      case IDD_MG_MAINMENU:
         fcnPtr = MediaGalleryApp_MainMenuDlg_HandleEvent;
         break;

      case IDD_MG_PHONEMEM:
         fcnPtr = MediaGalleryApp_PhoneMemDlg_HandleEvent;
         break;

      case IDD_MG_CARDMEM:
         fcnPtr = MediaGalleryApp_CardMemDlg_HandleEvent;
         break;

      case IDD_MG_UDISK:
         fcnPtr = MediaGalleryApp_UDiskDlg_HandleEvent;
         break;

      case IDD_MG_MEMSTAT:
         fcnPtr = MediaGalleryApp_MemStatDlg_HandleEvent;
         break;

      case IDD_MG_MEDIAMENU:
         fcnPtr = MediaGalleryApp_MediaMenuDlg_HandleEvent;
         break;

      case IDD_MG_IMAGEVIEWER:
         fcnPtr = MGAppPopupMenu_OnImageViewer;
         break;

      case IDD_MG_RENAME:
         fcnPtr = MGAppPopupMenu_OnRename;
         break;

      case IDD_MG_SETAS:
         fcnPtr = MGAppPopupMenu_OnSetAs;
         break;
	  case IDD_MG_SAVETOPLAYLIST:
	  	 fcnPtr = MGAppPopupMenu_OnSavetoplaylist;
	  	 break;
      case IDD_MG_DETAIL:
         fcnPtr = MGAppPopupMenu_OnDetail;
         break;

      case IDD_MG_SORT:
         fcnPtr = MGAppPopupMenu_OnSort;
         break;

      case IDD_MG_SELECTPATH:
         fcnPtr = MGAppPopupMenu_OnSelectPath;
         break;

      case IDD_MG_MUSICADD:
         fcnPtr = MediaGalleryApp_MusicAddDlg_HandleEvent;
         break;

      case IDD_MG_VIDEOADD:
         fcnPtr = MediaGalleryApp_VideoAddDlg_HandleEvent;
         break;

      case IDD_MG_IMAGESETTING:
         fcnPtr = MediaGalleryApp_ImageSettingDlg_HandleEvent;
         break;

      case IDD_MG_MUSICETTING:
         fcnPtr = MediaGalleryApp_SoundSettingDlg_HandleEvent;
         break;

      default:
         return FALSE;
         break;
   }

   return fcnPtr(pMe, eCode, wParam, dwParam);
}

int MediaGalleryApp_ShowDialog(CMediaGalleryApp* pMe, uint16 nDlgResId)
{
   int nRet;

   if(!pMe)
   {
      return EFAILED;
   }

   /*At most one dialog open at once*/
   if(NULL != ISHELL_GetActiveDialog(pMe->m_pShell))
   {
      /*Looks like there is one dialog already opened.
       * Flag an error  return without doing anything.
       */
      MG_FARF(ADDR, ("Try to create a dialog without closing the previous"));
      return EFAILED;
   }

   MG_FARF(ADDR, ("Show MediaGallery %d", nDlgResId));

   nRet = ISHELL_CreateDialog(pMe->m_pShell,
                              MGRES_LANGFILE,
                              nDlgResId,
                              NULL);

   if(nRet != SUCCESS)
   {
      MG_FARF(ADDR, ("Failed to create dialog in Media Gallery"));
   }

   return nRet;
}//MediaGallery_ShowDialog

/*===========================================================================
 *    Function Definitions(used only in MediaGalleryAppDialogHandler.c file)
 *===========================================================================
 */
static boolean  MediaGalleryApp_ShowMsgBox(CMediaGalleryApp* pMe,
                                 const AECHAR *pwszText,
                                 const char * pszResFile,
                                 uint16 nTextResId,
                                 PromptMsg_e_Type eMsgBoxType,
                                 BottomBar_e_Type eMsgBoxBottomBar)
{
   IStatic* pPromTxt = NULL;
   AECHAR szText[MSGBOX_MAXTEXTLEN];
   PromptMsg_Param_type MsgParam={0};
   MediaDlgStat eDlgStat;

   if(!pMe || !(pwszText || pszResFile))
   {
      MG_FARF(ADDR, ("bad PARAMETER!"));
      return FALSE;
   }

   if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,
            (void **)&pPromTxt))
   {
      MG_FARF(ADDR, ("Create instance Static failed!"));
      return FALSE;
   }

   MSG_FATAL("***zzg MediaGalleryApp_ShowMsgBox***", 0, 0, 0);

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   /*Set the static control text based on the user settings of the
    * m_nMsgBoxTextId parameter.*/
   if(NULL == pwszText)
   {
      if(0 == ISHELL_LoadResString(pMe->m_pShell,
               pszResFile,
               nTextResId,
               szText,
               sizeof(szText)))
      {
         MG_FARF(ADDR, ("Load string failed!"));
         return FALSE;
      }

      MsgParam.pwszMsg = szText;
   }
   else
   {
      MsgParam.pwszMsg = (AECHAR *)pwszText;
   }

   MsgParam.ePMsgType = eMsgBoxType;
   MsgParam.eBBarType = eMsgBoxBottomBar;
   DrawPromptMessage(pMe->m_pDisplay, pPromTxt, &MsgParam);

   RELEASEIF(pPromTxt);

   /*If it is progress bar, do not defer, otherwise it may not be update*/
   if(eDlgStat == MG_DLGSTAT_PROGRESS)
   {
      IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
   }
   else
   {
      if(eDlgStat != MG_DLGSTAT_YESNOBOX && eDlgStat != MG_DLGSTAT_MESSAGEBOX)
      {
         eDlgStat = MG_DLGSTAT_MESSAGEBOX;
         MGAppUtil_SetMediaDlgStat(pMe, eDlgStat);
      }
      IDISPLAY_Update(pMe->m_pDisplay);
   }

   /*reserve current status.*/
   pMe->m_nMsgBoxTextId = nTextResId;
   pMe->m_eMsgBoxType = eMsgBoxType;
   pMe->m_eMsgBoxBottomBar = eMsgBoxBottomBar;
   pMe->m_pszMsgBoxResFile = pszResFile;

   MSG_FATAL("***zzg MediaGalleryApp_ShowMsgBox eDlgStat=%x***", eDlgStat, 0, 0);

   /*Always start a timer for the message box.*/
   if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
   {
      ISHELL_SetTimer(pMe->m_pShell,
            PROMPTMSG_TIMER,
            MediaGalleryApp_MsgBoxTimeout,
            pMe);
   }

   return TRUE;
}//MediaGalleryApp_ShowMsgBox

/*show message box that will auto close or can close by CLR key*/
static void MediaGalleryApp_ShowPromptMsgBox(CMediaGalleryApp *pMe,
                                 uint16 nTextResId,
                                 PromptMsg_e_Type eMsgBoxType,
                                 BottomBar_e_Type eMsgBoxBottomBar)
{
   if(NULL == pMe)
      return;
   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_MESSAGEBOX);
   MediaGalleryApp_ShowMsgBox(pMe,
                              NULL,
                              MGRES_LANGFILE,
                              nTextResId,
                              eMsgBoxType,
                              eMsgBoxBottomBar);
   IDISPLAY_Update(pMe->m_pDisplay);
}//MediaGalleryApp_ShowPromptMsgBox

static __inline void MediaGalleryApp_ShowDoneMsgBox(CMediaGalleryApp *pMe)
{
   if(!pMe)
      return;

   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_MESSAGEBOX);
   MediaGalleryApp_SetMsgBoxID(pMe, MG_MSGID_DONE);
   MediaGalleryApp_ShowMsgBox(pMe,
                              NULL,
                              APPSCOMMON_RES_LANG_FILE,
                              IDS_POP_DONE,
                              MESSAGE_INFORMATION,
                              BTBAR_BACK);//Prompt success!
}//MediaGalleryApp_ShowDoneMsgBox

static __inline void MediaGalleryApp_ShowErrorMsgBox(CMediaGalleryApp *pMe)
{
   if(!pMe)
      return;

   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_MESSAGEBOX);
   MediaGalleryApp_SetMsgBoxID(pMe, MG_MSGID_DONE);
   MediaGalleryApp_ShowMsgBox(pMe,
                              NULL,
                              MGRES_LANGFILE,
                              IDS_MG_FILEOPERR,
                              MESSAGE_INFORMATION,
                              BTBAR_BACK);//Prompt success!
}//MediaGalleryApp_ShowDoneMsgBox


static __inline void MediaGalleryApp_ShowProgressBox(CMediaGalleryApp *pMe,
                                                      uint16 nMsgBoxID)
{
   uint16 iTextID = IDS_MG_PROGBARHINT;
   if(!pMe)
      return;

   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_PROGRESS);
   MediaGalleryApp_SetMsgBoxID(pMe, nMsgBoxID);
   if(nMsgBoxID == MG_MSGID_BUILDVIDEOLIST)
   	{
   	  iTextID = IDS_MG_LOADING;
   	}
    else if(nMsgBoxID == MG_MSGID_COPY)
    {
      iTextID = IDS_MG_COPYING;
    }
    else if(nMsgBoxID == MG_MSGID_MOVE)
    {
      iTextID = IDS_MG_MOVEING;
    } 
    else if(nMsgBoxID == MG_MSGID_RESORT)
    {
      iTextID = IDS_MG_EXECUTE;
    } 
   	else
   	{
   	  iTextID = IDS_MG_PROGBARHINT;
   	}
   MediaGalleryApp_ShowMsgBox(pMe,
         NULL,
         MGRES_LANGFILE,
         iTextID,
         MESSAGE_WAITING,
         BTBAR_NONE);
}//MediaGalleryApp_ShowProgressBox

static __inline void MediaGalleryApp_ShowYesNoBox(CMediaGalleryApp *pMe,
								const char * pszResFile,
								uint16 nTextResId,
                                                      uint16 nMsgBoxID)
{
   if(!pMe || !pszResFile)
      return;

   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_YESNOBOX);
   MediaGalleryApp_SetMsgBoxID( pMe, nMsgBoxID);
   MediaGalleryApp_ShowMsgBox(pMe,
                              NULL,
                              pszResFile,
                              nTextResId,
                              MESSAGE_CONFIRM,
                              BTBAR_OK_CANCEL);
}

/*===========================================================================
 * FUNCTION: MediaGalleryApp_MsgBoxTimeout
 * DESCRIPTION:
 * Callback function passed to the BREW layer so that whenever the timer that
 * we have set in the dialog handler expires, we can post a timeout event to the
 * dialog.
 * RETURN VALUE:None.
 * ==========================================================================
 */
static void MediaGalleryApp_MsgBoxTimeout(void* pUser)
{
   CMediaGalleryApp* pMe = (CMediaGalleryApp*)pUser;
   MSG_FATAL("MediaGalleryApp_MsgBoxTimeout Start",0,0,0);
   if(NULL == pMe || NULL == pMe->m_pActiveDlg)
   {
      return;
   }

   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

   if(TRUE == pMe->m_bSuspending)
   {
      ISHELL_SendEvent(pMe->m_pShell,
                       AEECLSID_MEDIAGALLERY,
                       EVT_DISPLAYDIALOGTIMEOUT,
                       0,
                       0);
   }
   else
   {
      ISHELL_PostEvent(pMe->m_pShell,
                       AEECLSID_MEDIAGALLERY,
                       EVT_DISPLAYDIALOGTIMEOUT,
                       0,
                       0);
   }
   MSG_FATAL("MediaGalleryApp_MsgBoxTimeout End",0,0,0);
}//MediaGalleryApp_MsgBoxTimeout


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_CancelMsgBoxTimer
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_CancelMsgBoxTimer(CMediaGalleryApp* pMe,
                                                 boolean bTimeOut)
{
   boolean bRet = FALSE;
   MSG_FATAL("MediaGalleryApp_CancelMsgBoxTimer Start",0,0,0);
   if(NULL == pMe)
      return bRet;

   ISHELL_CancelTimer(pMe->m_pShell, MediaGalleryApp_MsgBoxTimeout, pMe);

   if(MG_DLGSTAT_MESSAGEBOX == MGAppUtil_GetMediaDlgStat(pMe, NULL))
   {
      bRet = TRUE;
      if(TRUE == bTimeOut)
      {
         MediaGalleryApp_MsgBoxTimeout((void *)pMe);
      }
      else
      {
         MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);
      }
   }
   MSG_FATAL("MediaGalleryApp_CancelMsgBoxTimer End",0,0,0);
   return bRet;
}//MediaGalleryApp_CancelMsgBoxTimer

/*============================================================================
 * FUNCTION:MediaGalleryApp_MsgBoxDlg_HandleEvent
 * DESCRIPTION:
 *   This dialog handler is to create a dialog with only a static control (no
 *   softkeys) in it. This is shared by many media gallery states to give status
 *   updates and warning/informative messages to the user.
 *
 *   After the dialog is created, it starts a timer and when the timer expires
 *   it closes the dialog (in case if the dialog is not yet closed by the user)
 * RETURN VALUE:
 *    TRUE:If the dialog handler has processed the event.
 *    FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
static boolean MediaGalleryApp_MsgBoxDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                      AEEEvent eCode,
                                                      uint16 wParam,
                                                      uint32 dwParam)
{
   MediaDlgStat eDlgStat;

   if(!pMe)
   {
      return FALSE;
   }

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   switch(eCode)
   {
   case EVT_DIALOG_INIT:
      {
         IStatic* pIStatic = NULL;
         AECHAR szText[MSGBOX_MAXTEXTLEN];
         PromptMsg_Param_type MsgParam={0};

         if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_STATIC,
                                             (void **)&pIStatic))
         {
            MG_FARF(ADDR, ("Create static cotrol instance failed!"));
            return FALSE;
         }

         /*Set the static control text based on the user settings of the
          * m_nMsgBoxTextId parameter.*/
         ISHELL_LoadResString(pMe->m_pShell,
                              pMe->m_pszMsgBoxResFile,
                              pMe->m_nMsgBoxTextId,
                              szText,
                              sizeof(szText));
         MsgParam.ePMsgType = pMe->m_eMsgBoxType;
         MsgParam.pwszMsg = szText;
         MsgParam.eBBarType = pMe->m_eMsgBoxBottomBar;
         /*we do not special ISTATIC control, because of DIALOG will redraw
          * ISTATIC control, and the text color is the default, it may be the
          * same as background, but DrawPromptMessage function will change the
          * text color.*/
         DrawPromptMessage(pMe->m_pDisplay, pIStatic, &MsgParam);
         RELEASEIF(pIStatic);

         if(eDlgStat != MG_DLGSTAT_MESSAGEBOX &&
            eDlgStat != MG_DLGSTAT_YESNOBOX &&
            eDlgStat != MG_DLGSTAT_PROGRESS)
         {
            eDlgStat = MG_DLGSTAT_MESSAGEBOX;
            MGAppUtil_SetMediaDlgStat(pMe, eDlgStat);
         }

         /*Always start a timer for the message box.*/
         if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
         {
            ISHELL_SetTimer(pMe->m_pShell,
                            PROMPTMSG_TIMER,
                            MediaGalleryApp_MsgBoxTimeout,
                            pMe);
         }
         return TRUE;
      }

   case EVT_DIALOG_START:
      {
         /* KLUDGE! Need to wait for the Dialog to draw itself before
          * the ISTATIC_Redraw() command can be issued , otherwise the Dialog
          * will overwrite the IStatic control
          */
         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
                          EVT_USER_REDRAW, 0, 0);
         return TRUE;
      }

   case EVT_USER_REDRAW:
      {
         return TRUE;
      }

   case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending)
         {
            ;
         }

         return TRUE;
      }

   case EVT_KEY:
      {
         switch(wParam)
         {
         case AVK_CLR:
            if(eDlgStat == MG_DLGSTAT_YESNOBOX )
            {
               //uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

               MGCLOSE_DIALOG(MGDLGRET_NO);
               eDlgStat = MG_DLGSTAT_UNKNOWN;
            }
            else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
            {
               ISHELL_CancelTimer(pMe->m_pShell,
                                  MediaGalleryApp_MsgBoxTimeout, pMe);

               MGCLOSE_DIALOG(MGDLGRET_MSGBOX_OK);
               eDlgStat = MG_DLGSTAT_UNKNOWN;

            }
            break;

         case AVK_SELECT:
            if(eDlgStat == MG_DLGSTAT_YESNOBOX )
            {
               MGCLOSE_DIALOG(MGDLGRET_YES);
               eDlgStat = MG_DLGSTAT_UNKNOWN;
            }
            break;

         default:
            break;
         }

         MGAppUtil_SetMediaDlgStat(pMe, eDlgStat);

         return TRUE;
         break;
      }
#ifdef FEATURE_LCD_TOUCH_ENABLE
       case EVT_PEN_UP:
          {
              AEEDeviceInfo devinfo;
              int nBarH ;
              AEERect rc;
              int16 wXPos = (int16)AEE_GET_X(dwParam);
              int16 wYPos = (int16)AEE_GET_Y(dwParam);
              MSG_FATAL("MGAppPopupMenu_OnImageViewer wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
              nBarH = GetBottomBarHeight(pMe->m_pDisplay);
              MEMSET(&devinfo, 0, sizeof(devinfo));
              ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
              SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
   
              if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
              {
                  if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                  {
                      boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_SELECT,0);
                      return rt;
                  }
                  else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                  {                       
                       boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_CLR,0);
                       return rt;
                  }
              }            
          }
          break;
#endif //FEATURE_LCD_TOUCH_ENABLE

   case EVT_DISPLAYDIALOGTIMEOUT:
      {
         MGDLGRetValue eMsgRet = MGDLGRET_MSGBOX_OK;
         uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
         MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT Start",0,0,0);
         switch(nMsgBoxId)
         {
         default:
            break;
         }
         if(pMe->m_USBConnect)
         {
             MGCLOSE_DIALOG(MGDLGRET_UDISK);
         }
         else
         {
            if(pMe->m_STOPUSB)
            {
               MGCLOSE_DIALOG(MGDLGRET_CANCELED);
            }
            else
            {
                MGCLOSE_DIALOG(eMsgRet);
            }
             
         }
         
         return TRUE;
      }

   default:
      break;
   }

   return FALSE;
}//MediaGalleryApp_MsgBoxDlg_HandleEvent

/*===========================================================================
 * FUNCTION:MediaGalleryApp_MainMenuDlg_HandleEvent
 * DESCRIPTION:Media gallery , main menu event handler.
 * RETURN VALUE:
 *    TRUE:If the dialog handler has processed the event.
 *    FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
static boolean MediaGalleryApp_MainMenuDlg_HandleEvent(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
   uint16 *pPrevSelItemID;

   if(!pMe)
   {
      return FALSE;
   }

   if(NULL == (pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                        IDC_MG_MAINMENU)))
   {
      return FALSE;
   }

   pPrevSelItemID = pMe->m_nPrevMenuItemSel + MGDLG_LEVEL_MAIN;

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         /*add menu item*/
		 //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_MEDIAGALLERY,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_PHONEMEMORY);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_CARDMEMORY);

         /*need the two item only when start applet from menu tree, */
         if(pMe->m_StartMode == MGSM_NORMAL_EXPLORER)
         {
            MGMENU_ADDITEM(pMenuCtl, IDS_MG_MEMORYSTATUS);
            MGMENU_ADDITEM(pMenuCtl, IDS_MG_UDISK);
         }
         return TRUE;
      }

      case EVT_DIALOG_START:
      {
         const char *pszResFile = MGRES_LANGFILE;
         uint16 wResID = IDS_MG_MEDIAGALLERY;
         MGStartMode eStartMode = pMe->m_StartMode;

         if(eStartMode == MGSM_NORMAL_EXPLORER)
         {
            wResID = IDS_MG_MEDIAGALLERY;
         }
         else if(eStartMode == MGSM_MUSIC_ADD)
         {
            wResID = IDS_MG_MUSIC;
         }
         else if(eStartMode == MGSM_MUSIC_SETTING)
         {
            wResID = IDS_MG_MUSIC;
         }
         else if(eStartMode == MGSM_PHOTO_BROWSE)
         {
            wResID = IDS_MG_PHOTOGALLERY;
         }
         else if(eStartMode == MGSM_VIDEO_BROWSE)
         {
            wResID = IDS_MG_VIDEOGALLERY;
         }
         else if(eStartMode == MGSM_IMAGE_SETTING)
         {
            wResID = IDS_MG_PICTURES;
         }
         else if(eStartMode == MGSM_VIDEO_ADD)
         {
            wResID = IDS_MG_VIDEOS;
         }
         else if(eStartMode == MGSM_RECORD_VIEW ||
                 eStartMode == MGSM_FILE_SELECT)
         {
            wResID = IDS_MG_MEDIAGALLERY;
         }

         //IMENUCTL_SetTitle(pMenuCtl, pszResFile, wResID, NULL);

         MGAppUtil_SetMenuCtlRectProp(pMe,
               MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY,
               pMenuCtl);

        IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

         if(MG_MENUITEM_NULL != *pPrevSelItemID)
         {
            IMENUCTL_SetSel(pMenuCtl, *pPrevSelItemID);
         }
         IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
         /*Set all previous menu select item id to MG_MENUITEM_NULL*/
         MGExplorer_InitFileIcon(&pMe->m_pFileIcons, TRUE);
         MGExplorer_ResetPreItemID(&pMe->m_Explorer, TRUE);
         /* KLUDGE! Need to wait for the Dialog to draw itself before the
          * MGAppUtil_DrawSoftkey() command can be issued ,
          * otherwise the Dialog will overwrite the the softkey
          */
         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
               EVT_USER_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_USER_REDRAW:
      {
         int nBackground;
         uint8 nState;
         if((nBackground = app_media_scheduler()) != APP_MEDIA_ALLOW)
         {
            MG_FARF(ADDR, ("nBackground is %d", nBackground));
            MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_YESNOBOX);
            MediaGalleryApp_SetMsgBoxID( pMe,
            (uint16)(nBackground == APP_MEDIA_IMPACT_BY_FM ?
                  MG_MSGID_FMPLAYBACKGROUND : MG_MSGID_MP3PLAYBACKGROUND));
            MGCLOSE_DIALOG(MGDLGRET_BGPLAYPROMPT);
         }
         else if(MG_MENUITEM_NULL == *pPrevSelItemID)
         {
            nState = MediaGalleryApp_GetCallbackState(pMe, MG_CBT_TESTCARD0);
            //Do not need test card0 when return from sublevel, or resume if
            //we have done
            if(MG_CBS_NULL == nState)
            {
               CALLBACK_Init(&pMe->m_CallBack,
                             MediaGallery_TestCard0,
                             (void *)pMe);
               MediaGalleryApp_SetCallbackStateStart(pMe, MG_CBT_TESTCARD0);
               ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
            }
         }

         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         if(pMe->m_bSuspending == TRUE)
         {
            ;
         }

         /*when suspend or press AVK_CLR, ENTER to submenu we ensure we have
          * tested SD card whetehr exist*/


         ISHELL_CancelTimer(pMe->m_pShell,
                            MediaGalleryApp_MsgBoxTimeout,
                            pMe);
         return TRUE;
      }

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               *pPrevSelItemID = MG_MAX_UINT16;
               MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               return TRUE;

            default:
               break;
         }
      break;

      case EVT_COMMAND:
      {
         if(CALLBACK_IsQueued(&pMe->m_CallBack))
         {
            MG_FARF(ADDR, ("SD CARD status unknow!!!"));
            return TRUE;
         }

         if(wParam != *pPrevSelItemID || MG_MAX_UINT16 ==  *pPrevSelItemID)
         {
            *pPrevSelItemID = wParam;
            pMe->m_bKeepMediaMenu = FALSE;
         }

         switch(wParam)
         {
         case IDS_MG_PHONEMEMORY:
            MGCLOSE_DIALOG(MGDLGRET_PHONEMEM);
            return TRUE;

         case IDS_MG_CARDMEMORY:
            MSG_FATAL("IDS_MG_CARDMEMORY pMe->m_bCardExist=%d", pMe->m_bCardExist, 0, 0);
            if(pMe->m_bCardExist)
            {
#ifdef FEATURE_MG_MASSCARD_EXPLORER
               const char* pDirs = MG_MASSCARD_ROOTDIR;

               pMe->m_Explorer.m_eView = MG_VIEW_DEFAULT;
               pMe->m_StoreMedium = MG_STMED_MASSCARD;

               MGExplorer_Init(&pMe->m_Explorer,
                               pMe->m_pFileMgr,
                               pDirs,
                               MG_MIME_MISCELL);

               MGCLOSE_DIALOG(MGDLGRET_CARDMEM);
#else//#ifdef FEATURE_MG_MASSCARD_EXPLORER
               MGCLOSE_DIALOG(MGDLGRET_CARDMEM);
#endif//#ifdef FEATURE_MG_MASSCARD_EXPLORER
            }
            else
            {
               /*SD card do not insert*/
               MGCLOSE_DIALOG(MGDLGRET_NOEXTCARD);
            }
            return TRUE;

         case IDS_MG_MEMORYSTATUS:
            MGCLOSE_DIALOG(MGDLGRET_MEMSTAT);
            return TRUE;

         case IDS_MG_UDISK:
            MGCLOSE_DIALOG(MGDLGRET_UDISK);
            return TRUE;

         default:
            break;
         }
         break;
      }

      case EVT_DISPLAYDIALOGTIMEOUT:
         MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT Start",0,0,0);
         MGCLOSE_DIALOG(MGDLGRET_MSGBOX_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_MainMenuDlg_HandleEvent

/*===========================================================================
 * FUNCTION:MediaGalleryApp_PhoneMemDlg_HandleEvent
 * DESCRIPTION:Phone Memory's menu event handler
 * RETURN VALUE:
 *    TRUE:If the dialog handler has processed the event.
 *    FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
static boolean MediaGalleryApp_PhoneMemDlg_HandleEvent(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
	 uint16 *pPrevSelItemID;

   if(!pMe)
   {
      return FALSE;
   }

   if(NULL == (pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                      IDC_MG_PHONEMEM)))
   {
      return FALSE;
   }

   pPrevSelItemID = pMe->m_nPrevMenuItemSel + MGDLG_LEVEL_MEDIUM;

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
		 //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_PHONEMEMORY,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
         MSG_FATAL("MediaGalleryApp_PhoneMemDlg_HandleEvent init",0,0,0);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_PICTURES);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_MUSIC);
#if !defined(FEATRUE_MG_NOHSVIDEO)
#if 0
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_VIDEOS);
#endif
#endif
         //MGMENU_ADDITEM(pMenuCtl, IDS_MG_CALLMEMO);
#ifdef FEATURE_MG_WAPMMS_SUPPORT
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_DOWNLOAD);
#endif

         return TRUE;
      }

      case EVT_DIALOG_START:
      {
         MGAppUtil_SetMenuCtlRectProp(pMe,
            MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY,
            pMenuCtl);
        IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

         if(MG_MENUITEM_NULL != *pPrevSelItemID)
         {
            IMENUCTL_SetSel(pMenuCtl, *pPrevSelItemID);
         }
         IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
         /* KLUDGE! Need to wait for the Dialog to draw itself before the
          * MGAppUtil_DrawSoftkey() command can be issued ,
          * otherwise the Dialog will overwrite the the softkey
          */
         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
               EVT_USER_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_USER_REDRAW:
      {
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending)
         {
            ;
         }

         return TRUE;
      }

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               *pPrevSelItemID = MG_MAX_UINT16;
               MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               return TRUE;
            default:
               break;
         }
         break;

      case EVT_COMMAND:
      {
         const char *pDirs;
         boolean bInit;
         uint16 eFolderType;

         switch(wParam)
         {
         case IDS_MG_PICTURES:
            /*Now we do not show grid view.*/
            //pMe->m_Explorer.m_eView = MG_VIEW_DEFAULT;
            //MG_PHONE_ROOTDIR MG_PICTURES_FOLDER;
            pDirs = MG_PHONEPICTURE_PATH;
            eFolderType= MG_MIME_IMGBASE;
            break;

         case IDS_MG_MUSIC:
            pDirs = MG_PHONEMUSIC_PATH;
            eFolderType=MG_MIME_SNDBASE;
            break;

         case IDS_MG_VIDEOS:
            pDirs = MG_PHONEVIDEOS_PATH;
            eFolderType=MG_MIME_VIDEOBASE;
            break;
#if 0
         case IDS_MG_CALLMEMO:
            pDirs = MG_PHONECALLMEMO_PATH;
            eFolderType= MG_MIME_CALLREC;
            break;
#endif
#ifdef  FEATURE_MG_WAPMMS_SUPPORT
         case IDS_MG_DOWNLOAD:
           pDirs = MG_PHONEDOWNLOAD_PATH;
           eFolderType = MG_MIME_MISCELL;
           break;
#endif

         default:
            return FALSE;
         }

         if(wParam != *pPrevSelItemID || MG_MAX_UINT16 ==  *pPrevSelItemID)
         {
            *pPrevSelItemID = wParam;
            pMe->m_bKeepMediaMenu = FALSE;
         }

         pMe->m_Explorer.m_eView = MG_VIEW_DEFAULT;
         pMe->m_StoreMedium = MG_STMED_HANDSET;

            bInit = MGExplorer_Init(&pMe->m_Explorer,
                            pMe->m_pFileMgr,
                            pDirs,
                            eFolderType);

         if(bInit == TRUE)
         {
            MGCLOSE_DIALOG(MGDLGRET_MEDIAMENU);
         }
         else
         {
            //no files in the folder, pop up message, before it, redraw the
            //active again, othwise the tranparency will lose for the prvious
            //progress bar message box
            IDIALOG_Redraw(pMe->m_pActiveDlg);
            MGCLOSE_DIALOG(MGDLGRET_NOFILES);
         }

         return TRUE;
      }

      case EVT_DISPLAYDIALOGTIMEOUT:
         MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT Start",0,0,0);
         MGCLOSE_DIALOG(MGDLGRET_MSGBOX_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_PhoneMemDlg_HandleEvent


/*===========================================================================
 * FUNCTION:MediaGalleryApp_CardMemDlg_HandleEvent
 * DESCRIPTION:Card Memory's menu event handler, entry external mass storge
 *  such as MMC/SD card
 * RETURN VALUE:
 *    TRUE:If the dialog handler has processed the event.
 *    FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
static boolean MediaGalleryApp_CardMemDlg_HandleEvent(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
	 uint16 *pPrevSelItemID;
   MSG_FATAL("MediaGalleryApp_CardMemDlg_HandleEvent eCode=0x%x, wParam=0x%x, dwParam=0x%x", eCode, wParam, dwParam); 
   if(!pMe)
   {
      return FALSE;
   }

   if(NULL == (pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
               IDC_MG_CARDMEM)))
   {
      return FALSE;
   }
         pPrevSelItemID = pMe->m_nPrevMenuItemSel + MGDLG_LEVEL_MEDIUM;

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
		 //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_CARDMEMORY,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_PICTURES);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_MUSIC);
		 #if 0
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_VIDEOS);
		 #endif
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_VOICEMEMO);
#ifdef FEATURE_MG_WAPMMS_SUPPORT
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_DOWNLOAD);
#endif
         return TRUE;
      }

      case EVT_DIALOG_START:
      {
         MGAppUtil_SetMenuCtlRectProp(pMe,
               MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY,
               pMenuCtl);
       IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

         if(MG_MENUITEM_NULL != *pPrevSelItemID)
         {
            IMENUCTL_SetSel(pMenuCtl, *pPrevSelItemID);
         }
         IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
         /* KLUDGE! Need to wait for the Dialog to draw itself before the
          * MGAppUtil_DrawSoftkey() command can be issued ,
          * otherwise the Dialog will overwrite the the softkey
          */
         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
               EVT_USER_REDRAW, 0, 0);
         return TRUE;
      }

      case EVT_USER_REDRAW:
      {
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending)
         {
            ;
         }

         return TRUE;
      }

/*       case EVT_KEY_PRESS:
 *       case EVT_KEY_RELEASE:
 */
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	  case EVT_USER:
#endif
      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
      *pPrevSelItemID = MG_MAX_UINT16;
               MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               return TRUE;

            default:
               break;
         }
         break;

      case EVT_COMMAND:
         {
            const char* pDirs;
            boolean bInit;
            uint16 eFolderType;

            switch(wParam)
            {
               case IDS_MG_PICTURES:
                  /*Now we do not show grid view.*/
                  //pMe->m_Explorer.m_eView = MG_VIEW_GRID;
                  //MG_MASSCARD_ROOTDIR MG_PICTURES_FOLDER
                  pDirs = MG_MASSCARDPICTURE_PATH;
                  eFolderType= MG_MIME_IMGBASE;
                  break;

               case IDS_MG_MUSIC:
                  pDirs = MG_MASSCARDMUSIC_PATH;
                  eFolderType=MG_MIME_SNDBASE;
                  break;

               case IDS_MG_VIDEOS:
                  pDirs = MG_MASSCARDVIDEO_PATH;
                  eFolderType=MG_MIME_VIDEOBASE;
                  break;

               case IDS_MG_VOICEMEMO:
                  pDirs = MG_MASSCARDVOICEMEMO_PATH;
                  eFolderType= MG_MIME_VOICEREC;
                  break;

#ifdef FEATURE_MG_WAPMMS_SUPPORT
               case IDS_MG_DOWNLOAD:
                  pDirs = MG_MASSCARDDOWNLOAD_PATH;
                  eFolderType = MG_MIME_MISCELL;
                  break;
#endif

               default:
                  return FALSE;
            }

         if(wParam != *pPrevSelItemID || MG_MAX_UINT16 ==  *pPrevSelItemID)
         {
         *pPrevSelItemID = wParam;
         pMe->m_bKeepMediaMenu = FALSE;
         }
            pMe->m_Explorer.m_eView = MG_VIEW_DEFAULT;
            pMe->m_StoreMedium = MG_STMED_MASSCARD;

               bInit = MGExplorer_Init(&pMe->m_Explorer,
                     pMe->m_pFileMgr,
                     pDirs,
                     eFolderType);

            if(bInit == TRUE)
            {
               MGCLOSE_DIALOG(MGDLGRET_MEDIAMENU);
            }
            else
            {
               //no files in the folder, pop up message, before it, redraw the
               //active again, othwise the tranparency will lose for the
               //prvious progress bar message box
               IDIALOG_Redraw(pMe->m_pActiveDlg);
               MGCLOSE_DIALOG(MGDLGRET_NOFILES);
            }
            return TRUE;
         }

      case EVT_DISPLAYDIALOGTIMEOUT:
         MGCLOSE_DIALOG(MGDLGRET_MSGBOX_OK)
         return TRUE;

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_CardMemDlg_HandleEvent


/*===========================================================================
 * FUNCTION:MediaGalleryApp_MemStatDlg_HandleEvent
 * DESCRIPTION:Memory Status dialog event handler
 * RETURN VALUE:
 *    TRUE:If the dialog handler has processed the event.
 *    FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
static boolean MediaGalleryApp_MemStatDlg_HandleEvent(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IStatic* pStatic;
   IMenuCtl* pMenuCtl;

   if(!pMe)
   {
      return FALSE;
   }

   pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MG_MEMSTAT);
   pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MG_MEMSTATTITLE);

   if((NULL == pStatic)||(NULL == pMenuCtl))
   {
      return FALSE;
   }

   switch(eCode)
   {
   case EVT_DIALOG_INIT://GETFSFREE
      {
         AEERect rc;
         AEEDeviceInfo DevInfo;

         ISHELL_GetDeviceInfo(pMe->m_pShell, &DevInfo);
         rc = pMe->m_rc;
         rc.dy =  GetTitleBarHeight(pMe->m_pDisplay);
		 
		 //add by yangdecai
		{
			AECHAR WTitle[40] = {0};
			(void)ISHELL_LoadResString(pMe->m_pShell,
                    MGRES_LANGFILE,                                
                    IDS_MG_MEMORYSTATUS,
                    WTitle,
                    sizeof(WTitle));
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		}
		 
         IMENUCTL_SetRect(pMenuCtl, &rc);
         IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE | MP_WRAPSCROLL);

         //rc.y += rc.dy;
         rc.dy =  DevInfo.cyScreen - rc.y - GetBottomBarHeight(pMe->m_pDisplay);

         ISTATIC_SetRect(pStatic, &rc);
         ISTATIC_SetProperties(pStatic, ST_MIDDLETEXT|ST_GRAPHIC_BG);
         ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND); //modified by yangdecai
         return TRUE;
      }

   case EVT_DIALOG_START:
      {
         IFileMgr* pIFileMgr = NULL;
         AECHAR *pszBuffer = NULL;
         int nStatus = EFAILED;
         AECHAR szContent[MG_MAX_LINETEXTNUM];
         AECHAR szUsed[MG_MAX_LINETEXTNUM];
         AECHAR szFree[MG_MAX_LINETEXTNUM];
         AECHAR szTotal[MG_MAX_LINETEXTNUM];
         AECHAR szMemUnit[MG_MEMUNIT_STRLENMAX];
         MG_MemStat MemStats[MG_MEMSTAT_ITEMMAX];
         int16 nCount;
         boolean bRet;
         int16 nItemID[MG_MEMSTAT_ITEMMAX]={
            IDS_MG_PHONEMEMSTAT,
            IDS_MG_CARDMEMSTAT,
            IDS_MG_TOTALMEMSTAT};

         //GETFSFREE(&dwTotal);
         MEMSET(MemStats, 0, sizeof(MemStats));

         if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_FILEMGR,
                                             (void**)&pIFileMgr))
         {
            IFILEMGR_GetFreeSpaceEx(pIFileMgr, MG_PHONE_ROOTDIR,
                                    &MemStats[0].m_dwTotal, &MemStats[0].m_dwFree);

            //f = IFILEMGR_GetFreeSpace(pIFileMgr, &t);

            MemStats[0].m_dwUsed = MemStats[0].m_dwTotal - MemStats[0].m_dwFree;

            if(pMe->m_bCardExist)
            {
               nStatus = IFILEMGR_GetFreeSpaceEx(pIFileMgr,
                                                 MG_MASSCARD_ROOTDIR,
                                                 &MemStats[1].m_dwTotal,
                                                 &MemStats[1].m_dwFree);
               if(SUCCESS != nStatus)
               {
                  MG_FARF(ADDR, ("Error! Get SD CARD MEM FAILED"));
                  //return FALSE;
               }
               else
               {
                  MemStats[1].m_dwUsed = MemStats[1].m_dwTotal - MemStats[1].m_dwFree;

                  /*total memory status*/
                  MemStats[2].m_dwTotal = MemStats[0].m_dwTotal + MemStats[1].m_dwTotal;
                  MemStats[2].m_dwFree = MemStats[0].m_dwFree + MemStats[1].m_dwFree;
                  MemStats[2].m_dwUsed = MemStats[0].m_dwUsed + MemStats[1].m_dwUsed;
               }
            }

            pszBuffer = (AECHAR*) MALLOC(sizeof(AECHAR) * MG_MAX_TEXT_SIZE);

            if(!pszBuffer)
               goto MEMSTATEXIT;

            MEMSET(pszBuffer, 0, sizeof(pszBuffer));

            ISHELL_LoadResString(pMe->m_pShell, MGRES_LANGFILE,
                                 IDS_MG_SPACEUSED, szUsed, sizeof(szUsed));
            ISHELL_LoadResString(pMe->m_pShell, MGRES_LANGFILE,
                                 IDS_MG_SPACEFREE, szFree, sizeof(szFree));
            ISHELL_LoadResString(pMe->m_pShell, MGRES_LANGFILE,
                                 IDS_MG_SPACETOTAL, szTotal, sizeof(szTotal));
         }
         else
         {
            MG_FARF(ADDR, ("Error! Create file manager failed"));
            return EFAILED;
         }

         for(nCount = 0; nCount < ARRAYSIZE(nItemID); nCount++)
         {
            if((!pMe->m_bCardExist || SUCCESS != nStatus) &&
               (IDS_MG_PHONEMEMSTAT != nItemID[nCount]))
            {
               break;
            }

            ISHELL_LoadResString(pMe->m_pShell, MGRES_LANGFILE,
                                 nItemID[nCount], szContent, sizeof(szContent));
            MG_FARF(ADDR, ("%S",szContent));

            WSTRCAT(pszBuffer, szContent);
            //WSTRCAT(pszBuffer, MG_DOUBLELINEBREK_WSTR);
            WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);

            bRet = MGAppUtil_SpaceUnitConvert(pMe->m_pShell,
                                              MemStats[nCount].m_dwUsed,
                                              szMemUnit, sizeof(szMemUnit));
            if(TRUE == bRet)
            {
               WSTRCAT(pszBuffer, szUsed);
               WSTRCAT(pszBuffer, szMemUnit);
               WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);
            }

            bRet = MGAppUtil_SpaceUnitConvert(pMe->m_pShell,
                                              MemStats[nCount].m_dwFree,
                                              szMemUnit, sizeof(szMemUnit));
            if(TRUE == bRet)
            {
               WSTRCAT(pszBuffer, szFree);
               WSTRCAT(pszBuffer, szMemUnit);
               WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);
            }

            bRet = MGAppUtil_SpaceUnitConvert(pMe->m_pShell,
                                              MemStats[nCount].m_dwTotal,
                                              szMemUnit, sizeof(szMemUnit));
            if(TRUE == bRet)
            {
               WSTRCAT(pszBuffer, szTotal);
               WSTRCAT(pszBuffer, szMemUnit);
            }

            //WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);
            WSTRCAT(pszBuffer, MG_DOUBLELINEBREK_WSTR);
            
            MG_FARF(ADDR, ("%S",pszBuffer));
         }

         MG_FARF(ADDR, ("%S,",pszBuffer));

         ISTATIC_SetText(pStatic, NULL, pszBuffer,
                         AEE_FONT_NORMAL, AEE_FONT_NORMAL);

         /* KLUDGE! Need to wait for the Dialog to draw itself before the
          * MGAppUtil_DrawSoftkey() command can be issued ,
          * otherwise the Dialog will overwrite the the softkey
          */
         ISHELL_PostEvent(pMe->m_pShell,
                          AEECLSID_MEDIAGALLERY,
                          EVT_USER_REDRAW,
                          0,
                          0);

MEMSTATEXIT:
         RELEASEIF(pIFileMgr);
         FREEIF(pszBuffer);

         return TRUE;
      }

   case EVT_USER_REDRAW:
      {
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
		 AEEDeviceInfo devinfo;
		 int nBarH ;
		 AEERect rc;
#endif

         ISTATIC_Redraw(pStatic);
         MGAppUtil_DrawSoftkey(pMe, BTBAR_BACK);
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH//wlh 响应触摸//存储状态//这里没有处理坐标，加上
		nBarH = GetBottomBarHeight(pMe->m_pDisplay);
		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
		SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
		IMENUCTL_SetRect(pMenuCtl,&rc);
		IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_BACK);
		IMENUCTL_SetActive(pMenuCtl, TRUE); 
#endif
         IANNUNCIATOR_Redraw(pMe->m_pIAnn);
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending)
         {
            ;
         }

         return TRUE;
      }
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	  case EVT_USER:
#endif
   case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
         MGCLOSE_DIALOG(MGDLGRET_CANCELED);
         return TRUE;
      default:
         break;
      }
      break;

   case EVT_DISPLAYDIALOGTIMEOUT:
      MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT Start",0,0,0);
      MGCLOSE_DIALOG(MGDLGRET_MSGBOX_OK)
         return TRUE;

   default:
      break;
   }
   return FALSE;
}//MediaGalleryApp_MemStatDlg_HandleEvent



/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_UDiskDlg_HandleEvent
 * DESCRIPTION  :  U-Disk function
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_UDiskDlg_HandleEvent(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IStatic *pText;

   if(!pMe)
   {
      MG_FARF(ADDR, ("U Disk dialog encounter NULL!"));
      return FALSE;
   }
   pText = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MG_UDISKRUN);

   if(!pText)
      return FALSE;

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         IMenuCtl* pMenuCtl;
         AEERect rc;
         AEEDeviceInfo DevInfo;

         pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MG_UDISK);
		 //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_UDISK,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }

         ISHELL_GetDeviceInfo(pMe->m_pShell, &DevInfo);
         rc = pMe->m_rc;
         rc.dy =  GetTitleBarHeight(pMe->m_pDisplay);
         if(pMenuCtl)
         {

            IMENUCTL_SetRect(pMenuCtl, &rc);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE | MP_WRAPSCROLL);
         }

         rc.y += rc.dy;
         rc.dy =  DevInfo.cyScreen - rc.y - GetBottomBarHeight(pMe->m_pDisplay);

         ISTATIC_SetRect(pText, &rc);
         ISTATIC_SetProperties(pText, ST_MIDDLETEXT|ST_GRAPHIC_BG);
         ISTATIC_SetBackGround(pText, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);//modified by yangdecai
         return TRUE;
      }

      case EVT_DIALOG_START:
      {
         AECHAR szBuffer[MG_MAX_LINETEXTNUM];

         ISHELL_LoadResString(pMe->m_pShell, MGRES_LANGFILE,
               IDS_MG_RUNUDISK, szBuffer, sizeof(szBuffer));

         ISTATIC_SetText(pText, NULL, szBuffer,
               AEE_FONT_NORMAL, AEE_FONT_NORMAL);

         CALLBACK_Cancel(&pMe->m_CallBack);  
         CALLBACK_Init(&pMe->m_CallBack, MGAppUtil_StartUDisk, (void *)pMe);
#if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T) 

         ISHELL_SetTimer(pMe->m_pShell,2000,CheckUSBCableConnect_HandleDialogTimer,pMe);
#endif
         pMe->m_nCallbackDoFor = MG_CBT_STARTUDISK;
         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
               EVT_USER_REDRAW, 0, 0);
         ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
         return TRUE;
      }

      case EVT_USER_REDRAW:
      { 
          MGAppUtil_DrawSoftkey(pMe, BTBAR_STOP);
          IDISPLAY_Update(pMe->m_pDisplay);
        
         return TRUE;
      }
      case EVT_DIALOG_END:
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : Can be suspended???
         if(pMe->m_bSuspending)
         {
            ;
         }
      pMe->m_USBConnect = FALSE;
      ISHELL_CancelTimer(pMe->m_pShell, CheckUSBCableConnect_HandleDialogTimer, pMe);
      CALLBACK_Cancel(&pMe->m_CallBack);
      /*if applet is suspended, do not stop UDisk, if we check
       * pMe->m_bSuspending,  press AVK_END will not stop udisk*/
      if(/*FALSE == pMe->m_bSuspending &&*/
         TRUE == MediaGallery_CheckUdiskStat())
      {
         boolean bRet = FALSE;

         bRet = MediaGallery_StopUDisk(pMe);
      }
      return TRUE;

/*       case EVT_KEY_PRESS:
 *       case EVT_KEY_RELEASE:
 *             break;
 */

      case EVT_KEY:
         switch(wParam)
         {
            case AVK_CLR:
               //MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               if( TRUE == MediaGallery_CheckUdiskStat())
               {
                #if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T) 
                  pMe->m_USBConnect = FALSE;
                  pMe->m_STOPUSB = TRUE;
                  ISHELL_CancelTimer(pMe->m_pShell, CheckUSBCableConnect_HandleDialogTimer, pMe);
                #endif
                  MGCLOSE_DIALOG(MGDLGRET_UDISKSTOP);
               }

               return TRUE;

            default:
               break;
         }

      break;

      case EVT_COMMAND:
      {
         IUSBDriver *pUSBDriver;

         if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                  AEECLSID_USBDRIVER, (void **)&pUSBDriver))
         {
            MG_FARF(ADDR, ("Create USB Driver instance failed!"));
            return FALSE;
         }
         return TRUE;

      }

      case EVT_DISPLAYDIALOGTIMEOUT:
      {
         MG_FARF(ADDR, ("display dialog time out"));
         return TRUE;
      }

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
      case EVT_PEN_UP:
          {
              AEEDeviceInfo devinfo;
              int nBarH ;
              AEERect rc;
              int16 wXPos = (int16)AEE_GET_X(dwParam);
              int16 wYPos = (int16)AEE_GET_Y(dwParam);
              MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
              nBarH = GetBottomBarHeight(pMe->m_pDisplay);
      
              MEMSET(&devinfo, 0, sizeof(devinfo));
              ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
              SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

              if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
              {
                  if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                  {                       
                       boolean rt = MediaGalleryApp_UDiskDlg_HandleEvent((void*)pMe,EVT_KEY,AVK_CLR,0);
                       return rt;
                  }
              }
          }
          break;
#endif //FEATURE_LCD_TOUCH_ENABLE

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_UDiskDlg_HandleEvent

static void CheckUSBCableConnect_HandleDialogTimer(void *pUser)
{
    CMediaGalleryApp *pMe = (CMediaGalleryApp *)pUser;

    if((pMe->m_pActiveDlg == NULL)||(pMe == NULL))
    {
        return;
    }
  #if 1
    if(MediaGallery_CheckUSBCableConnect())  
    {
       if(pMe->m_USBConnect ==FALSE)
       {                                       
          pMe->m_USBConnect=TRUE;
          MGCLOSE_DIALOG(MGDLGRET_USBConnect);
          
       }
       

    }
    else
    {
       if(pMe->m_USBConnect ==TRUE)
       {                                
         pMe->m_USBConnect=FALSE;
         MGCLOSE_DIALOG(MGDLGRET_USBNotConnect);
          
       }
    }
 #endif
 ISHELL_SetTimer(pMe->m_pShell,2000,CheckUSBCableConnect_HandleDialogTimer,pMe);
// ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER_REDRAW,0,0);
}

/*===========================================================================
 * FUNCTION:MediaGalleryApp_MediaMenuDlg_HandleEvent
 * DESCRIPTION: browse various media files and do various operation
 * RETURN VALUE:
 *    TRUE:If the dialog handler has processed the event.
 *    FALSE:If the dialog handler did not process the event.
 *===========================================================================
 */
static boolean MediaGalleryApp_MediaMenuDlg_HandleEvent(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl = NULL;
   MediaDlgStat eDlgStat;

   if(!pMe)
   {
      return FALSE;
   }
   
   pMenuCtl = pMe->m_pMediaMenu;
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   MSG_FATAL("MediaMenuDlg_HandleEvent eCode=0x%x, wParam=0x%x, eDlgStat=%d",eCode,wParam,eDlgStat); 
   if(NULL == pMenuCtl && eCode != EVT_DIALOG_INIT)
   {
      return FALSE;
   }
 /*  if(pMenuCtl != NULL)
   {
       MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent pMenuCtl != NULL",0,0,0);  
       if (IMENUCTL_HandleEvent(pMenuCtl, eCode, wParam, dwParam))
       {
           MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent 0",0,0,0); 
           return TRUE;
       }
   }*/
   MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent 1",0,0,0);
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent EVT_DIALOG_INIT",0,0,0);
         MGAppUtil_CreateMediaMenu(pMe,  NULL);

         return TRUE;
      }

      case EVT_DIALOG_START:
      {
#ifdef FEATURE_THUMBNAIL
         AEEMenuColors MenuCol={0};

         MGAppUtil_SetMenuCtlRectProp(pMe,
                                      MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ICON_TEXT_TOP,
                                      pMenuCtl);
         IMENUCTL_SetOemProperties(pMenuCtl,
                                   OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_ICON_FIX_ITEMMAX);
         MenuCol.wMask = MC_BACK;
         MenuCol.cBack = MAKE_RGB(255,255,204);//MAKE_RGB(192,192,192);
         IMENUCTL_SetColors(pMenuCtl, &MenuCol);
#else
         MGAppUtil_SetMenuCtlRectProp(pMe,
                                      MP_UNDERLINE_TITLE | MP_WRAPSCROLL |MP_ALLOW_DUPE_IDS | MP_NO_REDRAW,
                                      pMenuCtl);
         IMENUCTL_SetOemProperties(pMenuCtl,
                                   OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

#endif
         MSG_FATAL("MGAppUtil_CreateMediaMenu m_bKeepMediaMenu=%d",pMe->m_bKeepMediaMenu,0,0);
         if(pMe->m_bKeepMediaMenu == FALSE)
         {
            MGAppUtil_StartMediaMenu(pMe, pMenuCtl, eCode, wParam, dwParam);
         }
         else
         {
            MSG_FATAL("MGAppUtil_CreateMediaMenu m_PopupOps=0X%x",pMe->m_PopupOps,0,0);
            switch(pMe->m_PopupOps)
            {
            case MG_OP_COPY:
               IMENUCTL_SetActive(pMenuCtl, FALSE);
               if(TRUE == MGAppUtil_InitFileCopy(pMe))
               {
                  //in MGAppUtil_InitFileCopy may show message box, ex. overwrite
                  //confirm , we do not need recover Media menu interface,
                  //just do nothing.
                  MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

                  if(eDlgStat == MG_DLGSTAT_NORMAL)
                  {
                     MediaGalleryApp_ShowProgressBox(pMe, MG_MSGID_COPY);

                     MGAppPopupMenu_OnCopy(pMe, pMenuCtl);
                  }
               }
               break;

            case MG_OP_MOVE:
               IMENUCTL_SetActive(pMenuCtl, FALSE);
               if(TRUE == MGAppUtil_InitFileCopy(pMe))
               {
                  //in MGAppUtil_InitFileCopy may show message box, ex. overwrite
                  //confirm , we do not need recover Media menu interface,
                  //just do nothing.
                  MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
                  if(eDlgStat == MG_DLGSTAT_NORMAL)
                  {
                     MediaGalleryApp_ShowProgressBox(pMe, MG_MSGID_MOVE);

                     MGAppPopupMenu_OnMove(pMe, pMenuCtl);
                  }
               }

               break;

            default:
               break;
            }
         }

         return TRUE;
      }

      case EVT_USER_REDRAW:
      {
         MGAppUtil_UpdateMediaMenuSoftkey(pMe);
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         MGAppUtil_OnMediaMenuDlgEnd(pMe, pMenuCtl, wParam, dwParam);
         return TRUE;
      }

/*       case EVT_KEY_PRESS:
 *       case EVT_KEY_RELEASE:
 */
      case EVT_KEY:
      {
         if(FALSE == MGAppUtil_IsNeedHandleKey(pMe, wParam))
         {
            return TRUE;
         }
#ifdef FEATURE_USES_MMS
         if(pMe->m_isForMMS && (pMe->m_StartMode == MGSM_VIDEO_BROWSE) && (wParam == AVK_SELECT))
         {
            wParam = AVK_INFO;
         }
#endif         
         switch(wParam)
         {
            case AVK_CLR:
               return MGAppUtil_OnMediaMenuClrKeyEvt(pMe, pMenuCtl);
               break;

            case AVK_INFO:
               MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent AVK_INFO",0,0,0); 
               return MGAppUtil_OnMediaMenuInfoKeyEvt(pMe,
                                                      pMenuCtl,
                                                      eCode,
                                                      wParam,
                                                      dwParam);
               break;

            case AVK_SELECT:
               MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent AVK_SELECT",0,0,0);  
               return MGAppUtil_OnMediaMenuSelectKeyEvt(pMe,
                                                        pMenuCtl,
                                                        eCode,
                                                        wParam,
                                                        dwParam);
               break;

            default:
               return MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                         pMenuCtl,
                                                         eCode,
                                                         wParam,
                                                         dwParam);
         }
         break;
      }

      case EVT_COMMAND:
        MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent EVT_COMMAND",0,0,0);
        if(eDlgStat == MG_DLGSTAT_POPUP)
        {
            MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent EVT_COMMAND 1",0,0,0);
            return MGAppUtil_OnMediaMenuCommandEvt(pMe,
                                                pMenuCtl,
                                                wParam,
                                                dwParam);
        } 
        else
        {
            MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent EVT_COMMAND 2",0,0,0);
            return MGAppUtil_OnMediaMenuInfoKeyEvt(pMe,
                                                  pMenuCtl,
                                                  eCode,
                                                  wParam,
                                                  0);   
        }
     /*    return MGAppUtil_OnMediaMenuCommandEvt(pMe,
                                                pMenuCtl,
                                                wParam,
                                                dwParam);*/
         break;

      case EVT_CTL_SEL_CHANGED:
         MGAppUtil_OnMediaMenuSelChange(pMe, eDlgStat);
         return TRUE;

      case EVT_DISPLAYDIALOGTIMEOUT:
         {
            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT Start",0,0,0);
#ifdef FEATURE_USES_MMS   
            {
                uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);  
                MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT nMsgBoxId=%d",nMsgBoxId,0,0); 
                if(nMsgBoxId == MG_MSGID_DONE)
                {
                   MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 0",0,0,0); 
                   if(pMe->m_StartMode == MGSM_VIDEO_BROWSE)
                   {
                      MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 1",0,0,0); 
                      MGCLOSE_DIALOG(MGDLGRET_SETTING_SELECT);
                      return TRUE;
                   }
				   //Add By zzg 2011_12_14
				   else
				   {				
				   	    MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 1111",0,0,0); 
	                    return MGAppUtil_OnMediaMenuMsgBoxTimeOut(pMe,
	                                              pMenuCtl,
	                                              wParam,
	                                              dwParam);
				   }
				   //Add End
				   	
                }  
                else
                {
                    MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 2",nMsgBoxId,0,0); 
                    return MGAppUtil_OnMediaMenuMsgBoxTimeOut(pMe,
                                              pMenuCtl,
                                              wParam,
                                              dwParam);
                }
            }
#else            
            return MGAppUtil_OnMediaMenuMsgBoxTimeOut(pMe,
                                                      pMenuCtl,
                                                      wParam,
                                                      dwParam);
#endif
         }
         break;
      
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
                MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
                else
                {
                    if(eDlgStat == MG_DLGSTAT_POPUP)
                    {
                         MSG_FATAL("eDlgStat == MG_DLGSTAT_POPUP",0,0,0);  
                        return IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
                    }  
                    else if(pMenuCtl != NULL)
                    {
                        MSG_FATAL("EVT_PEN_UP pMenuCtl != NULL",0,0,0);  
                        if (IMENUCTL_HandleEvent(pMenuCtl, eCode, wParam, dwParam))
                        {
                            MSG_FATAL("MediaGalleryApp_MediaMenuDlg_HandleEvent 0",0,0,0); 
                            return TRUE;
                        }
                    }    
                }
			}
			break;
#endif //FEATURE_LCD_TOUCH_ENABLE
      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_MediaMenuDlg_HandleEvent


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_OnDefaultOperate
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_OnDefaultOperate(CMediaGalleryApp* pMe,
                                    IMenuCtl* pMenuCtl,
                                    uint16 wParam,
                                    uint32 dwParam)
{
   MGFileInfo *pCurNode;
   boolean bRet = TRUE;

   if(NULL == pMe || NULL == pMenuCtl)
      return FALSE;

   MGAppUtil_UpdateSelItemCheck(pMe);
   pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

   if (NULL == pCurNode ||
       TRUE == pMe->m_bCallbackDoing ||
       TRUE == pMe->m_bMediaMenuEmpty)
   {
      MSG_FATAL("pCurNode NULL, or in build or menu empty!",0,0,0);
      return FALSE;
   }

   if(pCurNode->attrib == AEE_FA_DIR)
   {
      bRet = MGAppPopupMenu_OnOpenSubFolder(pMe, pCurNode);
   }
   else
   {
      MGMimeType     eMimeBase;

      eMimeBase = MediaGalleryApp_GetExplorerMime(pMe);
	  MSG_FATAL("eMimeBase::::::::::::::::::::::%d:::::::::",eMimeBase,0,0);
      if(eMimeBase == MG_MIME_MISCELL ||
         !MG_BETWEEN(eMimeBase,MG_MIME_MISMAX,MG_MIME_MAX))
      {
         uint16     uMimeBase;
         int        nRet;

         uMimeBase = MGMediaInfo_MimeBaseConvert(eMimeBase);
         DBGPRINTF("pCurNode->szName=%s",pCurNode->szName);   
         /*if eMimeBase is MG_MIME_UNKNOWN etc, we need detect each
          * type*/
         nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                        pMe->m_pFileMgr,
                                        pCurNode->szName,
                                        uMimeBase,
                                        &eMimeBase);

         if(SUCCESS != nRet)
            return FALSE;
      }
      if(MG_BETWEEN(eMimeBase, MG_MIME_IMGBASE, MG_MIME_IMGMAX))
      {
         DBGPRINTF("1pCurNode->szName=%s",pCurNode->szName);  
         pMe->m_PopupOps = MG_OP_VIEWIMG;
         MGCLOSE_DIALOG(MGDLGRET_VIEW);
      }
      else if(MG_BETWEEN(eMimeBase, MG_MIME_SNDBASE, MG_MIME_SNDMAX) ||
              (eMimeBase == MG_MIME_VOICEREC) ||
              (eMimeBase == MG_MIME_CALLREC))
      {
         DBGPRINTF("2pCurNode->szName=%s",pCurNode->szName);  
         if(SUCCESS != MGAppPopupMenu_OnMusicPlay(pMe, pMenuCtl))
            bRet = FALSE;
      }
#if defined(FEATURE_VIDEOPLAYER)
      else if(MG_BETWEEN(eMimeBase, MG_MIME_VIDEOBASE, MG_MIME_VIDEOMAX))
      {
         DBGPRINTF("3pCurNode->szName=%s",pCurNode->szName);
#ifdef FEATURE_USES_MMS
         if(pMe->m_isForMMS) 
         {
            IConfig *pConfig = NULL;
            if(pCurNode->dwSize >= 300*1024)
            {
               MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                IDS_MG_LARGEFILE,
                                                MESSAGE_ERR,
                                                BTBAR_BACK);
 
               return TRUE;
            }               
            if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG, (void **)&pConfig))
            {
                ICONFIG_SetItem(pConfig, CFGI_MMSVIDEO, pCurNode->szName, sizeof(pCurNode->szName));   
                MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);
                RELEASEIF(pConfig);
                return TRUE;                
            }
            return FALSE;
         }
#endif           
         if(SUCCESS != MGAppPopupMenu_OnVideoPlay(pMe, pMenuCtl))
            bRet = FALSE;
      }
#endif
      else
      {
         DBGPRINTF("4pCurNode->szName=%s",pCurNode->szName);  
#ifdef FEATURE_USES_MMS
         if(pMe->m_isForMMS && (MG_BETWEEN(eMimeBase, MG_MIME_VIDEOBASE, MG_MIME_VIDEOMAX))) 
         {
            IConfig *pConfig = NULL;
            if(pCurNode->dwSize >= 300*1024)
            {
               MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                IDS_MG_LARGEFILE,
                                                MESSAGE_ERR,
                                                BTBAR_BACK);
 
               return TRUE;
            }              
            if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG, (void **)&pConfig))
            {
                ICONFIG_SetItem(pConfig, CFGI_MMSVIDEO, pCurNode->szName, sizeof(pCurNode->szName));   
                MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);
                RELEASEIF(pConfig);
                return TRUE;
            }
            return FALSE;
         }
#endif         
      	 //add by yangdecai 2010-08-04
		 MediaGalleryApp_ShowPromptMsgBox(pMe,
                                          IDS_MG_FILEOPERR,
                                          MESSAGE_ERR,
                                          BTBAR_BACK);
		 //add by yangdecai 2010-08-04 end
         bRet = FALSE;
      }
   }

   return bRet;
}//MediaGalleryApp_OnDefaultOperate

#define MG_OPS_DEFAULT     0x00
#define MG_OPS_MEDIAMENU   0x01
#define MG_OPS_IMAGEVIEW   0x02

static void MediaGalleryApp_SetOps(CMediaGalleryApp* pMe,
                                   uint8 nMethod,
                                   int16 nOps)
{
   if(NULL != pMe)
   {
      switch(nMethod)
      {
      case MG_OPS_MEDIAMENU:
         pMe->m_PopupOps = nOps;
         break;

      case MG_OPS_IMAGEVIEW:
         pMe->m_ImgViewOps = nOps;
         break;

      default:
         if(pMe->m_PopupOps == MG_OP_VIEWIMG)
            pMe->m_ImgViewOps = nOps;
         else
            pMe->m_PopupOps = nOps;
         break;
      }
   }
}//MediaGalleryApp
/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_OnPopupMenuCommand
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_OnPopupMenuCommand(CMediaGalleryApp* pMe,
                        IMenuCtl* pMenuCtl, uint16 wParam, uint32 dwParam)
{
   MGFileInfo *pItemData = NULL;
   boolean  bRet = TRUE;

   if(!pMe || !pMenuCtl)
      return FALSE;
   MSG_FATAL("MediaGalleryApp_OnPopupMenuCommand Start",0,0,0);
   pItemData = MediaGalleryApp_GetCurrentNode(pMe);

   if(!pItemData)
   {
      MSG_FATAL("On popup menu command: no item data",0,0,0);
      return FALSE;
   }

   /*It is not good when release interface at its call back or notify*/
   //RELEASEIF(pMe->m_pMenuPopup);
   if(IMENUCTL_IsActive(pMe->m_pMenuPopup))
      IMENUCTL_SetActive(pMe->m_pMenuPopup, FALSE);

   if(pMe->m_PopupOps == MG_OP_VIEWIMG)
   {
      MGAppUtil_RedrawImage(pMe, NULL, TRUE);
   }
   else
   {
      MediaGalleryApp_RedrawInactiveMenu(pMenuCtl);
   }

   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

   //TODO: it may not permit to do here, for YESNO dialog will get operatr
   //tyep.
   switch(wParam)
   {
   case IDS_MG_PLAY:
      if(pItemData->attrib != AEE_FA_DIR)
      {
         bRet = MediaGalleryApp_OnDefaultOperate(pMe,
                                                 pMenuCtl,
                                                 wParam,
                                                 dwParam);
      }
      break;

   case IDS_MG_VIEW:
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_VIEWIMG);
      MediaGalleryApp_SetOps(pMe, MG_OPS_IMAGEVIEW, MG_OP_NULL);
      MGCLOSE_DIALOG(MGDLGRET_VIEW);
      break;

   case IDS_MG_SLIDE:
      MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_PHOTOSLIDE);
      MGCLOSE_DIALOG(MGDLGRET_VIEW);
      break;

   case IDS_MG_OPEN:
      bRet = MGAppPopupMenu_OnOpenSubFolder(pMe, pItemData);
      break;

   case IDS_MG_SETWALLPAPER:
      {
         /*
          * if call IIMAGE_Notify for png, bmp, it will be async, if for it
          * is sync. So we must set status before call IIMAGE_Notify */
         MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_WALLPAPER);
         bRet = MGAppPopupMenu_OnSetWallpaper(pMe, pItemData);
         return bRet;
      }

   case IDS_MG_SETAS:
      MGCLOSE_DIALOG(MGDLGRET_SETAS);
      break;
   case IDS_MSG_SAVEPLAYLIST:
   	  MGCLOSE_DIALOG(MGDLGRET_SAVETOPLAYLIST);
   	  break;
   case IDS_MG_RENAME:
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_RENAME);
      MGCLOSE_DIALOG(MGDLGRET_RENAME);
      break;

   case IDS_MG_DELETE:
      {
	  	 MSG_FATAL("***zzg MediaGalleryApp  IDS_MG_DELETE***", 0, 0, 0);
         MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_DEL);
         /*check the file is use for setting, such as ringtone, wallpaper,
          * we should ask user.*/
         MediaGalleryApp_ShowYesNoBox(pMe,
                                      APPSCOMMON_RES_LANG_FILE,
                                      IDS_MSG_CONFIRM_DELETE,
                                      MG_MSGID_DEL);
      }

      break;

   case IDS_MG_DELALL:
      /*If we not list all file, that is m_bCallbackDoing is TRUE, do not
       * permit this operation, or ask user to confirm!!!*/
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_DELALL);
      MediaGalleryApp_ShowYesNoBox(pMe,
                                   APPSCOMMON_RES_LANG_FILE,
                                   IDS_MSG_CONFIRM_DELETEALL,
                                   MG_MSGID_DELALL);
      break;

   case IDS_MG_BLUETOOTH:   	  
      MGAppPopupMenu_OnBluetooth(pMe, pMenuCtl);
      break;

   case IDS_MG_COPY:
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_COPY);
      MGCLOSE_DIALOG(MGDLGRET_SELECTPATH);
      break;

   case IDS_MG_MOVE:
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_MOVE);
      MGCLOSE_DIALOG(MGDLGRET_SELECTPATH);
      break;

   case IDS_MG_SORT:
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_SORT);
      MGCLOSE_DIALOG(MGDLGRET_SORT);
      break;

   case IDS_MG_DETAIL:
      MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_DETAIL);
      MGCLOSE_DIALOG(MGDLGRET_DETAIL);
      break;

   case IDS_MG_ZOOM:
      MGAppUtil_SetMediaDlgStat(pMe,MG_DLGSTAT_IMGZOOM);
      ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
                       EVT_USER_REDRAW, 0, 0);
      break;

   case IDS_MG_BUILDPL:
      {
         MediaGalleryApp_ShowProgressBox(pMe,
                                         MG_MSGID_BUILDVIDEOLIST);
#ifndef AEE_SIMULATOR
         MGExplorer_InitBuildVideoDatabase(pMe,
                                           MG_VIDEOFILE_DB_NAME,
                                           MGAppUtil_VideoDatabaseBuildComplete);
#else
         MGAppUtil_VideoDatabaseBuildComplete((void*)pMe);
#endif
      }
      break;

   case IDS_MG_SELECT:
#ifdef FEATURE_MG_WAPMMS_SUPPORT
      if(NULL != pMe->m_pExpPara && MGSM_FILE_SELECT == pMe->m_StartMode)
      {
         char *pSelPath;

         pSelPath = STRDUP(pItemData->szName);
         if(NULL != pSelPath)
         {
            MG_FARF(ADDR, ("client should free this memory"));
            MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_SELECT);
            bRet = ISHELL_PostEvent(pMe->m_pShell,
                                    pMe->m_pExpPara->nClsId,
                                    EVT_MYDATA_RETURN,
                                    0,
                                    (uint32)pSelPath);
         }
         else
         {
            bRet = FALSE;
         }

         if(bRet == TRUE)
         {
            MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);
         }
         else
         {
            MG_FARF(ADDR, ("client do not handle EVT_MYDATA_RETRUN"));
            FREEIF(pSelPath);
         }
      }
      else
      {
         MG_FARF(ADDR, ("ERROR ON select"));
         bRet = FALSE;
      }
#endif
      break;

   default:
      bRet = FALSE;
      break;
   }

   if(FALSE == bRet)
   {
      MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_NULL);
      MSG_FATAL("MGAppUtil_SetMediaDlgStat(pMe,MG_DLGSTAT_NORMAL)",0,0,0);
      MGAppUtil_SetMediaDlgStat(pMe,MG_DLGSTAT_NORMAL);

      IMENUCTL_SetActive(pMenuCtl, TRUE);
      IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
      MG_SENDEVENT(EVT_USER_REDRAW, 0, 0);
   }

   return bRet;
}/*MediaGalleryApp_OnPopupMenuCommand*/

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_OnImageViewer
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnImageViewer(CMediaGalleryApp* pMe,
                  AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
   IImageCtl* pImageCtl = NULL;
   MediaDlgStat eDlgStat;
   static int8 nsScale = 0;
   if(!pMe)
   {
      return FALSE;
   }
#ifdef FEATURE_LCD_TOUCH_ENABLE
   if(eCode==EVT_PEN_UP)
   {
   		AEEDeviceInfo devinfo;
		int nBarH ;
		AEERect rcR;
		AEERect rcL;
		int16 wXPos = (int16)AEE_GET_X(dwParam);
		int16 wYPos = (int16)AEE_GET_Y(dwParam);

		nBarH = GetBottomBarHeight(pMe->m_pDisplay);

		MEMSET(&devinfo, 0, sizeof(devinfo));
		ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
		SETAEERECT(&rcR, 0,0,50,30);
		SETAEERECT(&rcL, 190,0,50,30);
		if(TOUCH_PT_IN_RECT(wXPos,wYPos,rcR))
		{
			eCode = EVT_KEY;
			wParam = AVK_RIGHT;
		}
		else if(TOUCH_PT_IN_RECT(wXPos,wYPos,rcL))
		{
			eCode = EVT_KEY;
			wParam = AVK_LEFT;
		}
   }
#endif    
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   MSG_FATAL("MGAppPopupMenu_OnImageViewer eCode=0x%x, wParam=0x%x, eDlgStat=%d",eCode,wParam,eDlgStat);
   if(NULL == (pImageCtl = (IImageCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
               IDC_MG_IMAGEVIEWER)))
   {
      return FALSE;
   }
   MSG_FATAL("MGAppPopupMenu_OnImageViewer 2",0,0,0);
   switch(eCode)
   {
   case EVT_DIALOG_INIT:
      {
         IMenuCtl *pm = pMe->m_pMediaMenu;

         /*If menu active, timer belong to it will work, and we will find item
          * text still scroll after display other screen.
          * before do hide annunciator */
         MSG_FATAL("MGAppPopupMenu_OnImageViewer EVT_DIALOG_INIT",0,0,0);
         if(pm && TRUE == IMENUCTL_IsActive(pm))
         {
            IMENUCTL_SetActive(pm, FALSE);
         }

         MediaGallery_HideAnnunciatorBar(pMe->m_pDisplay, NULL, TRUE);
         MGAppUtil_DrawImageViewerBG(pMe);

         MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

         nsScale = 0;
         return TRUE;
      }

   case EVT_DIALOG_START:
      {
         MGFileInfo *pCurNode;
         MGMimeType  eMimeBase;
         int        nRet;
         MSG_FATAL("MGAppPopupMenu_OnImageViewer EVT_DIALOG_START",0,0,0);
         pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

         if(!pCurNode || AEE_FA_DIR == pCurNode->attrib){
            MSG_FATAL("OnImageViewer encounter NULL pointor",0,0,0);
            return FALSE;
         }

         if((pCurNode->dwSize >= MG_QUALIMGDECODE_MAX && pMe->m_StoreMedium == MG_STMED_MASSCARD) ||
            (pCurNode->dwSize >= MG_QUALIMGDECODE_MAX && pMe->m_StoreMedium == MG_STMED_HANDSET))
         {
            MSG_FATAL("Image's Size is too big",0,0,0);
            MediaGalleryApp_ShowPromptMsgBox(pMe,
                                             IDS_MG_LARGEFILE,
                                             MESSAGE_ERR,
                                             BTBAR_BACK);
            return TRUE;
         }

         nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                        pMe->m_pFileMgr,
                                        pCurNode->szName,
                                        MGMIME_BASE_IMAGE,
                                        &eMimeBase);

         pMe->m_bImgLoadDone = FALSE;
         if(pMe->m_ImgViewOps == MG_OP_NULL)
         {
            RELEASEIF(pMe->m_pImage);
         }
         
         if(pMe->m_pImage == NULL)
         {
            pMe->m_pImage = ISHELL_LoadImage(pMe->m_pShell,
                                             pCurNode->szName);
         }
         
         if(pMe->m_pImage != NULL)
         {
            IIMAGE_GetInfo(pMe->m_pImage, &pMe->m_ImgInfo);
			MSG_FATAL("MGAppUtil_LoadImageNotify....start",0,0,0);
            IIMAGE_Notify(pMe->m_pImage,
                          MGAppUtil_LoadImageNotify,
                          pMe);
         }
         else
         {
            pMe->m_bImgLoadDone = TRUE;
            MediaGalleryApp_ShowPromptMsgBox(pMe,
                                             IDS_MG_LOADFAILED,
                                             MESSAGE_ERR,
                                             BTBAR_BACK);
            MG_FARF(ADDR, ("ISHELL_LoadImage failed, may be file too long!"));
         }
         return TRUE;
      }

   case EVT_USER_REDRAW:
       MGAppUtil_UpdateImgViewerSoftkey(pMe);
       return TRUE;
       
   case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         MSG_FATAL("MGAppPopupMenu_OnImageViewer EVT_DIALOG_END",0,0,0);
         if(pMe->m_bSuspending)
         {
            ;
         }

         MediaGallery_HideAnnunciatorBar(pMe->m_pDisplay,
                                         &pMe->m_ClipRect,
                                         FALSE);	 
         return TRUE;
      }


#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
   case EVT_USER:
#endif
   case EVT_KEY:
      {
         switch(wParam)
         {
         case AVK_CLR:
            if (eDlgStat == MG_DLGSTAT_NORMAL)
            {
               pMe->m_PopupOps = MG_OP_NULL;
               pMe->m_ImgViewOps = MG_OP_NULL;

               RELEASEIF(pMe->m_pImage);
               MGExplorer_RecoverMediaMenu(pMe, pMe->m_pMediaMenu);
               MGCLOSE_DIALOG(MGDLGRET_CANCELED);
            }
            else if (eDlgStat == MG_DLGSTAT_POPUP ||
                     eDlgStat == MG_DLGSTAT_IMGZOOM ||
                     eDlgStat == MG_DLGSTAT_YESNOBOX)
            {
               IImage *pImage = pMe->m_pImage;

               if(eDlgStat == MG_DLGSTAT_IMGZOOM && NULL != pImage)
               {
                  IIMAGE_SetParm(pImage,
                                 IPARM_SCALE,
                                 pMe->m_ImgInfo.cx,
                                 pMe->m_ImgInfo.cy);
                  nsScale = 0;
               }

               /*Immediately release, other wise it will popup if the
                * item text is wide than clip rect, more dangerous is
                * that we use 848 decode image, it will destroy screen
                * status and corrupt file. For when 848 is decoding, 6020
                * can not send display buffer to 848 2009-3-12 15:06:13*/
               RELEASEIF(pMe->m_pMenuPopup);

               MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);
               MGAppUtil_RedrawImage(pMe, NULL, TRUE);
            }

            return TRUE;

         case AVK_SELECT:
            {
                MSG_FATAL("MGAppPopupMenu_OnImageViewer AVK_SELECT 1",0,0,0);
 #ifdef FEATURE_USES_MMS
               if(pMe->m_isForMMS)      
               {
                   return TRUE;
               }
#endif               
               if(eDlgStat == MG_DLGSTAT_NORMAL &&
                  (pMe->m_bImgLoadDone == TRUE))
               {
                  MenuInsItem ImgViewOptions[]={
                     {IDS_MG_ZOOM, TRUE}, {IDS_MG_SETWALLPAPER, TRUE},
                     {IDS_MG_DELETE, TRUE}, {IDS_MG_DETAIL, TRUE}
                  };
                  AEEImageInfo ImgInfo;
                  IImage *pi = pMe->m_pImage;

                  MSG_FATAL("MGAppPopupMenu_OnImageViewer AVK_SELECT 2",0,0,0);
                  if(pi)
                  {
                     IIMAGE_GetInfo(pi, &ImgInfo);
                     if(ImgInfo.bAnimated)
                        IIMAGE_Stop(pi);
                  }

                  ImgViewOptions[0].bInsert = MGAppUtil_ImageZoomCheck(pMe,
                                                                       pMe->m_Explorer.m_pCurNode);
                  //ImgViewOptions[1].bInsert =
                  //   MGAppUtil_WallpaperSettingCheck(NULL, &ImgInfo,
                  //         pMe->m_pShell);
                  ImgViewOptions[2].bInsert = FALSE;//!MG_FILE_PROTECT(FileAttrib);
                  /* :WARNING:2009-1-21 16:15:34:shenlong xu: We may use
                   * popup menu where show yes-no message after select popup*/
                  MGAppUtil_CreatePopupMenu(&pMe->m_pMenuPopup,
                                            ImgViewOptions,
                                            sizeof(ImgViewOptions)/sizeof(MenuInsItem),
                                            MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY,
                                            BTBAR_SELECT_BACK);
                  MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_POPUP);

               }
               else if (eDlgStat == MG_DLGSTAT_POPUP)
               {
                  IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
               }
               else if(eDlgStat == MG_DLGSTAT_YESNOBOX)
               {
                  uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

                  switch(nMsgBoxId)
                  {
                  case MG_MSGID_DEL:
                     MGAppPopupMenu_OnDelete(pMe, pMe->m_pMediaMenu);
                     break;
                  default:
                     break;
                  }

               }
               return TRUE;
            }

         case AVK_INFO:
            MSG_FATAL("MGAppPopupMenu_OnImageViewer AVK_INFO",0,0,0);
            if( eDlgStat == MG_DLGSTAT_IMGZOOM &&
               ( pMe->m_bImgLoadDone == TRUE ))
            {
               //do zoom
               if(pMe->m_pImage)
               {
                  IImage *pImage = pMe->m_pImage;

                  nsScale = (nsScale + 1) % 4;
                  if(nsScale == 0)
                  {
                      IIMAGE_SetParm(pImage,
                                     IPARM_SCALE,
                                     pMe->m_ImgInfo.cx,
                                     pMe->m_ImgInfo.cy);
                  }
                  else
                  {
                      IIMAGE_SetParm(pImage,
                                     IPARM_SCALE,
                                     pMe->m_ImgInfo.cx/(2*nsScale),
                                     pMe->m_ImgInfo.cy/(2*nsScale));
                  }
                  MGAppUtil_RedrawImage(pMe, NULL, FALSE);
               }
            }
            else if(eDlgStat == MG_DLGSTAT_POPUP)
            {
               return IMENUCTL_HandleEvent(pMe->m_pMenuPopup,
                                           eCode,
                                           wParam,
                                           dwParam);
            }
#ifdef FEATURE_USES_MMS
            else if(pMe->m_isForMMS)
            {
                MGFileInfo *pItemData = NULL;
                boolean  bRet = TRUE;
                MSG_FATAL("(pMe->m_isForMMS)",0,0,0);
                pItemData = MediaGalleryApp_GetCurrentNode(pMe);
                if(!pItemData)
                {
                  MSG_FATAL("On popup menu command: no item data",0,0,0);
                  return FALSE;
                }                
                MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_FORMMS_IMAGE);
                bRet = MGAppPopupMenu_OnSetMMSImage(pMe, pItemData);   
                if(bRet)
                {
                    MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);
                }
            }
#endif               
            return TRUE;

         case AVK_RIGHT:
         case AVK_LEFT:
            MSG_FATAL("MGAppPopupMenu_OnImageViewer AVK_LEFT, eDlgStat=%d",eDlgStat,0,0);
            if(eDlgStat == MG_DLGSTAT_NORMAL &&
               (pMe->m_bImgLoadDone == TRUE ))
            {
               AEEImageInfo ImgInfo;
               IImage *pi = pMe->m_pImage;

               if(pi)
               {
                  IIMAGE_GetInfo(pi, &ImgInfo);
                  if(ImgInfo.bAnimated)
                     IIMAGE_Stop(pi);
               }
               
               MGAppUtil_ChangeMediaMenuItemByType(pMe,
                                                   pMe->m_pMediaMenu,
                                                   (boolean)(wParam == AVK_RIGHT),
                                                   MGAppUtil_ImageSlideCheck);
               ISHELL_PostEvent(pMe->m_pShell,
                                AEECLSID_MEDIAGALLERY,
                                EVT_DIALOG_START,
                                0,
                                0);
            }

            return TRUE;

         default:
            if(eDlgStat == MG_DLGSTAT_POPUP && pMe->m_pMenuPopup)
            {
               return   IMENUCTL_HandleEvent(pMe->m_pMenuPopup,
                                             eCode, wParam, dwParam);
            }
            return FALSE;
         }
      }
      return TRUE;

   case EVT_COMMAND:
      {
         //MediaDlgStat
         if(eDlgStat == MG_DLGSTAT_POPUP)
         {
            MediaGalleryApp_OnPopupMenuCommand(pMe,
                                               pMe->m_pMediaMenu,
                                               wParam,
                                               dwParam);
         }
      }
      return TRUE;

   case EVT_DISPLAYDIALOGTIMEOUT:
      {
         uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

         MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT Start",0,0,0);

         MGAppPopupMenu_OperationDone(pMe, 0);

         if(nMsgBoxId == MG_MSGID_DONE)
         {
            if(pMe->m_StartMode == MGSM_IMAGE_SETTING)
            {
               MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 1",0,0,0); 
               MGCLOSE_DIALOG(MGDLGRET_SETTING_SELECT);
               return TRUE;
            }
         }

         if(SUCCESS == MGAppUtil_RedrawImage(pMe, NULL, TRUE))
         {
            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT MGAppUtil_SetMediaDlgStat",0,0,0);
            MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);
         }
         else
         {
            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 2",0,0,0);
            pMe->m_PopupOps = MG_OP_NULL;
            pMe->m_ImgViewOps = MG_OP_NULL;

            RELEASEIF(pMe->m_pImage);
            MGExplorer_RecoverMediaMenu(pMe, pMe->m_pMediaMenu);
            MGCLOSE_DIALOG(MGDLGRET_CANCELED);

         }
      }

      return TRUE;

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
    case EVT_PEN_UP:
       {
           AEEDeviceInfo devinfo;
           int nBarH ;
           AEERect rc;
           int16 wXPos = (int16)AEE_GET_X(dwParam);
           int16 wYPos = (int16)AEE_GET_Y(dwParam);
           MSG_FATAL("MGAppPopupMenu_OnImageViewer wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
           nBarH = GetBottomBarHeight(pMe->m_pDisplay);
           MEMSET(&devinfo, 0, sizeof(devinfo));
           ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
           SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

           if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
           {
               if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
               {
                   boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_SELECT,0);
                   return rt;
               }
               else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
               {
                    boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_INFO,0);
                    return rt;
               }
               else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
               {                       
                    boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_CLR,0);
                    return rt;
               }
           }
           else if((eDlgStat == MG_DLGSTAT_POPUP) && (pMe->m_pMenuPopup != NULL))
            {
                MSG_FATAL("EVT_PEN_UP 111",0,0,0);
                return IMENUCTL_HandleEvent(pMe->m_pMenuPopup,
                                           eCode,
                                           wParam,
                                           dwParam);
            }             
       }
       break;
#endif //FEATURE_LCD_TOUCH_ENABLE

   default:
      break;
   }
   MSG_FATAL("MGAppPopupMenu_OnImageViewer End",0,0,0);
   return FALSE;
}//MGAppPopupMenu_OnImageViewer


/*===========================================================================
 * FUNCTION: MGAppPopupMenu_OnMusicPlay
 *
 * DESCRIPTION: play music file selected in menu control
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppPopupMenu_OnMusicPlay(CMediaGalleryApp* pMe,
                                    IMenuCtl* pMenuCtl)
{
   IMusicPlayer *pMusicPlayer = NULL;
   MGFileInfo *pCurNode;
   if(!pMe || !pMenuCtl)
      return EBADPARM;

   pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

   if(!pCurNode)
      return EBADPARM;

   ISHELL_CreateInstance(pMe->m_pShell,
         AEECLSID_APP_MUSICPLAYER,
         (void**)&pMusicPlayer);

   /* Must deactive media menu.  Although we deactive media menu when popup
    * menu, but we may active it when close popup menu.when media menu active,
    * and if the focus item text is scroll, We can see a line scrolling black
    * text which is just the focus item tex in music player interface, and
    * this result in phone crashing more frequently.
    * */
   if(TRUE == IMENUCTL_IsActive(pMenuCtl))
      IMENUCTL_SetActive(pMenuCtl, FALSE);

   if(pMusicPlayer != NULL)
   {
      pMe->m_PopupOps =  MG_OP_PLAYMUSIC;
      IMUSICPLAYER_SimplePlayer(pMusicPlayer,pCurNode->szName);
      IMUSICPLAYER_Release(pMusicPlayer);
      pMusicPlayer = NULL;
   }
   else
   {
   	  //Add By zzg 2010_07_28
   	  MediaGalleryApp_ShowPromptMsgBox(pMe,
                                          IDS_MG_FILEOPERR,
                                          MESSAGE_ERR,
                                          BTBAR_BACK);
	  //Add End
	  
#ifndef AEE_SIMULATOR
      return EFAILED;
#else
      pMe->m_nRingVolume = AEE_MAX_VOLUME >> 1;
      MGAppUtil_PreviewRingtone(pMe);
#endif
   }

   return SUCCESS;
}//MGAppPopupMenu_OnMusicPlay

#if defined(FEATURE_VIDEOPLAYER)
/*===========================================================================
 * FUNCTION: MGAppPopupMenu_OnVideoPlay
 *
 * DESCRIPTION: play video thant select by menu control
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppPopupMenu_OnVideoPlay(CMediaGalleryApp* pMe, IMenuCtl* pMenuCtl)
{
   IVideoPlayer *pVideoPlayer = NULL;
   MGFileInfo *pCurNode;
   MSG_FATAL("MGAppPopupMenu_OnVideoPlay Start",0,0,0);
   if(!pMe || !pMenuCtl)
      return EBADPARM;

   pCurNode = MediaGalleryApp_GetCurrentNode(pMe);
   MSG_FATAL("MGAppPopupMenu_OnVideoPlay 1",0,0,0);
   if(!pCurNode)
      return EBADPARM;

   /* Must deactive media menu.  Although we deactive media menu when popup
    * menu, but we may active it when close popup menu.when media menu active,
    * and if the focus item text is scroll, We can see a line scrolling black
    * text which is just the focus item tex in music player interface, and
    * this result in phone crashing more frequently.
    * */
   if(TRUE == IMENUCTL_IsActive(pMenuCtl))
      IMENUCTL_SetActive(pMenuCtl, FALSE);

   ISHELL_CreateInstance(pMe->m_pShell,
         AEECLSID_VIDEOPLAYER,
         (void**)&pVideoPlayer);

   if(pVideoPlayer != NULL)
   {
      MSG_FATAL("MGAppPopupMenu_OnVideoPlay 2",0,0,0);
      pMe->m_PopupOps =  MG_OP_PLAYVIDEO;
      IVideoPlayer_Play(pVideoPlayer, pCurNode->szName);
      IVideoPlayer_Release(pVideoPlayer);
      pVideoPlayer = NULL;
   }
   else
   {
      MSG_FATAL("MGAppPopupMenu_OnVideoPlay 3",0,0,0);
      return EFAILED;
   }
   MSG_FATAL("MGAppPopupMenu_OnVideoPlay End",0,0,0);
   return SUCCESS;
}//MGAppPopupMenu_OnVideoPlay
#endif
/*
 * ==========================================================================
 * FUNCTION     :  MGAppPopupMenu_OnOpenSubFolder
 * DESCRIPTION  :  On the folder that select in the menu control
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnOpenSubFolder(CMediaGalleryApp *pMe,
                                          MGFileInfo *pCurNode)
{
   MGFileInfo *pItemData = NULL;
   int nResult;
   if(!pMe)
      return FALSE;

   pItemData = pCurNode;

   if(!pItemData)
      return FALSE;
   
   if(SUCCESS == (nResult = MGExplorer_ChangeCurDir( &pMe->m_Explorer,
                                                    MG_DIR_GODOWN,
                                                    pItemData->szName)))
   {
      // enter subfoler
      MGExplorer_InitBuildMediaMenu(pMe, MGAppUtil_MediaMenuBuildComplete);
   }
   else if(ERROR_DEEPDIR == nResult)
   {
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_DEEPPATH,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);

      MG_FARF(ADDR, ("Change to subfolder failed!"));
      return FALSE;
   }

   return TRUE;
}//MGAppPopupMenu_OnOpenSubFolder

/*===========================================================================
 * FUNCTION: MGAppPopupMenu_OnSetWallpaper
 *
 * DESCRIPTION: set the image file that select by menu control as wallpaper
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnSetWallpaper(CMediaGalleryApp *pMe,
                                             MGFileInfo *pItemData)
{
   MGFileInfo *pSelData = pItemData;
   
   if(!pMe)
      return FALSE;   

   if(pSelData->dwSize >= MG_WALLPAPER_MAX)
   {
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_LARGEFILE,
                                       MESSAGE_ERR,
                                       BTBAR_BACK);

      return TRUE;
   }

   if(pMe->m_PopupOps != MG_OP_VIEWIMG)
   {
      if(NULL == pSelData)
      {
         pSelData = MediaGalleryApp_GetCurrentNode(pMe);
      }

      if(!pSelData || !pSelData->szName)
      {
         MG_FARF(ADDR, ("Data is invalidate"));
         return FALSE;
      }
      
      // Check the image size, too small or to large are not permit. And the
      //animate image also do not permit
	  
      RELEASEIF(pMe->m_pImage);
      pMe->m_pImage = ISHELL_LoadImage(pMe->m_pShell, pSelData->szName);
      if(pMe->m_pImage)
      {
          IIMAGE_GetInfo(pMe->m_pImage, &pMe->m_ImgInfo);		
		  if(STRENDS(".gif", pSelData->szName))
		  {
		  	MSG_FATAL("pMe->m_ImgInfo.bAnimated...=%d",pMe->m_ImgInfo.bAnimated,0,0);
		  	pMe->m_Gif = TRUE;
			MSG_FATAL("pMe->m_Gif...=%d",pMe->m_Gif,0,0);
		  }
		  else
		  {
		  	pMe->m_Gif = FALSE;
		  }
      }
   }
   else if(pMe->m_ImgViewOps == MG_OP_WALLPAPER)
   {
      // Nothing TODO
   }
   else
   {
      return FALSE;
   }

   MG_FARF(ADDR, ("PopupOps is %d, ImgViewOps is %d", pMe->m_PopupOps, pMe->m_ImgViewOps));
   if(pMe->m_pImage)
   {
      /*
       * if call IIMAGE_Notify for png, bmp, it will be async, if for it
       * is sync. So we must set status before call IIMAGE_Notify */	  
      pMe->m_bImgLoadDone = FALSE;
	  if(STRENDS(".gif", pSelData->szName))
		  {
		  	MSG_FATAL("pMe->m_ImgInfo.bAnimated...=%d",pMe->m_ImgInfo.bAnimated,0,0);
		  	pMe->m_Gif = TRUE;
			MSG_FATAL("pMe->m_Gif...=%d",pMe->m_Gif,0,0);
		  }
		  else
		  {
		  	pMe->m_Gif = FALSE;
		  }
      IIMAGE_Notify(pMe->m_pImage, MGAppUtil_LoadImageNotify, pMe);
   }
   else
   {
      /* 由于ISHELL_LoadImage只用文件名的扩展名来判断是否是图片，如果不是，
       * 就不LoadImage。例如：对于长文件名，把扩展名截掉了，我们用
       * ISHELL_LoadImage就不成功。*/
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_LOADFAILED,
                                       MESSAGE_ERR,
                                       BTBAR_BACK);

      MG_FARF(ADDR, ("IImage interface is 0x%x", pMe->m_pImage));
   }
   return TRUE;
}//MGAppPopupMenu_OnSetWallpaper

/*===========================================================================
 * FUNCTION: MGAppPopupMenu_OnSetAs
 *
 * DESCRIPTION: set musci file that select by the menu control as variou
 * ringtone, such as call ringtone, sms ringtone
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnSetAs(CMediaGalleryApp *pMe,
                                 AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
   MediaDlgStat eDlgStat;

   if(!pMe)
   {
      return FALSE;
   }

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MG_SETAS);

   if(NULL == pMenuCtl)
      return FALSE;

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            IConfig *pConfig = NULL;
            MGFileInfo *pSelData = NULL;
            byte CurProfileNum;
            ringID nNewConfigRinger[PROFILENUMBER];

            MGAppUtil_SetMenuCtlRectProp(pMe,
                                         MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_MULTI_SEL,
                                         pMenuCtl);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

            MGMENU_ADDITEM(pMenuCtl, IDS_MG_CALLRINGTONE);
            MGMENU_ADDITEM(pMenuCtl, IDS_MG_SMSRINGTONE);
            MGMENU_ADDITEM(pMenuCtl, IDS_MG_ALARMRINGTONE);
            {
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_SETAS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }

            //IMENUCTL_SetTitle(pMenuCtl, MGRES_LANGFILE, IDS_MG_SETAS, NULL);
            pMe->m_nSelNum = 0;
            IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SAVE_BACK);

            pSelData = MediaGalleryApp_GetCurrentNode(pMe);

            if((SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                                AEECLSID_CONFIG,
                                                (void **)&pConfig)) &&
               (NULL != pSelData))
            {
               ICONFIG_GetItem(pConfig,
                               CFGI_PROFILE_CUR_NUMBER,
                               &CurProfileNum,
                               sizeof(CurProfileNum));

               ICONFIG_GetItem(pConfig,
                               CFGI_PROFILE_CALL_RINGER,
                               (void*)nNewConfigRinger,
                               sizeof(nNewConfigRinger));
               if(STRCMP(nNewConfigRinger[CurProfileNum].szMusicname, pSelData->szName) == 0)
               {
                  IMENUCTL_SetSelEx(pMenuCtl, IDS_MG_CALLRINGTONE, TRUE);
               }

               ICONFIG_GetItem(pConfig,
                               CFGI_PROFILE_SMS_RINGER_ID,
                               (void*)nNewConfigRinger,
                               sizeof(nNewConfigRinger));
               if(STRCMP(nNewConfigRinger[CurProfileNum].szMusicname, pSelData->szName) == 0)
               {
                  IMENUCTL_SetSelEx(pMenuCtl, IDS_MG_SMSRINGTONE, TRUE);
               }

               ICONFIG_GetItem(pConfig,
                               CFGI_PROFILE_ALARM_RINGER,
                               (void*)nNewConfigRinger,
                               sizeof(nNewConfigRinger));
               if(STRCMP(nNewConfigRinger[CurProfileNum].szMusicname, pSelData->szName) == 0)
               {
                  IMENUCTL_SetSelEx(pMenuCtl, IDS_MG_ALARMRINGTONE, TRUE);
               }
            }

            ISHELL_PostEvent(pMe->m_pShell,
                             AEECLSID_MEDIAGALLERY,
                             EVT_USER_REDRAW, 0, 0);

            RELEASEIF(pConfig);

            return TRUE;
         }

      case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(TRUE == pMe->m_bSuspending)
         {

         }

         return TRUE;
      }

      case EVT_USER_REDRAW:
         {
            IDISPLAY_Update(pMe->m_pDisplay);
            return TRUE;
         }
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	  case EVT_USER:
#endif
      case EVT_KEY:
         switch(wParam)
         {
         case AVK_CLR:
            if(eDlgStat == MG_DLGSTAT_NORMAL)
            {
               MGAppPopupMenu_OperationDone(pMe, 0);
               if(pMe->m_StartMode == MGSM_SETTING_RINGTONE)
               {
                  MGCLOSE_DIALOG(MGDLGRET_NONE);
               }
               else
               {
                  MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               }
            }
            else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
            {
               MediaGalleryApp_CancelMsgBoxTimer(pMe, TRUE);
            }
            return TRUE;

         case AVK_SELECT:
         //case AVK_INFO:
            {
               uint16 uSel;
               int nResult;
               AEEConfigItem ConfigItem;
               IConfig *pConfig = NULL;
               MGFileInfo *pSelData;
               byte CurProfileNum;
               ringID nNewConfigRinger[PROFILENUMBER];
               boolean bSel = FALSE;

               pSelData = MediaGalleryApp_GetCurrentNode(pMe);

               if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                   AEECLSID_CONFIG,
                                                   (void **)&pConfig))
               {
                  MG_FARF(ADDR, ("Create config interface failed"));
                  return EFAILED;
               }

               if(FALSE == IMENUCTL_EnumSelInit(pMenuCtl))
                  return FALSE;

               ICONFIG_GetItem(pConfig,
                               CFGI_PROFILE_CUR_NUMBER,
                               &CurProfileNum,
                               sizeof(CurProfileNum));

               while((uSel =IMENUCTL_EnumNextSel(pMenuCtl)) != (uint16)-1)
               {
                  //TODO: SET CONFIGURE
                  if(IDS_MG_CALLRINGTONE == uSel)
                  {
                     ConfigItem =CFGI_PROFILE_CALL_RINGER;//CFGI_CALL_RINGER;
                  }
                  else if(IDS_MG_SMSRINGTONE == uSel)
                  {
                     ConfigItem =CFGI_PROFILE_SMS_RINGER_ID;//CFGI_SMS_RINGER;
                  }
                  else if(IDS_MG_ALARMRINGTONE == uSel)
                  {
                     ConfigItem =CFGI_PROFILE_ALARM_RINGER;// CFGI_ALARM_RINGER;
                  }
                  else
                  {
                     continue;
                     MG_FARF(ADDR, ("ERROR"));
                  }

                  ICONFIG_GetItem(pConfig,
                                  ConfigItem,
                                  (void*)nNewConfigRinger,
                                  sizeof(nNewConfigRinger));
                  STRCPY(nNewConfigRinger[CurProfileNum].szMusicname,
                         pSelData->szName);
                  nNewConfigRinger[CurProfileNum].ringType = OEMNV_MP3_RINGER;
                  nResult = ICONFIG_SetItem(pConfig,
                                            ConfigItem,
                                            (void*)nNewConfigRinger,
                                            sizeof(nNewConfigRinger));
                  MG_FARF(ADDR, ("ICONFIG Result %d", nResult));
                  MG_FARF(ADDR, ("select item %d", uSel));
                  MG_FARF(ADDR, ("select item %s", pSelData->szName));
                  bSel = TRUE;
               }

               RELEASEIF(pConfig);
               if(bSel == TRUE)
               {
                  IMENUCTL_SetActive(pMenuCtl, FALSE);
                  MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);
               }
               else
               {
                  IMENUCTL_SetActive(pMenuCtl, FALSE);
                  MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                   IDS_MG_SELECTNONE,
                                                   MESSAGE_INFORMATION,
                                                   BTBAR_BACK);
               }

            }
            return TRUE;

         default:
            break;
         }
         return FALSE;

      case EVT_DISPLAYDIALOGTIMEOUT:
         {
            uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT",0,0,0);
            if(MG_MSGID_DONE == nMsgBoxId)
            {
               MGAppPopupMenu_OperationDone(pMe, 0);
               /*EXIT*/
               if(pMe->m_StartMode == MGSM_SETTING_RINGTONE)
               {
                  MGCLOSE_DIALOG(MGDLGRET_NONE);
               }
               else
               {
                  MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               }
            }
            else
            {
               IMENUCTL_SetActive(pMenuCtl, TRUE);
            }
            return TRUE;
         }


      default:
         break;
   }

   return FALSE;
}//MGAppPopupMenu_OnSetAs

static boolean MGAppPopupMenu_OnSavetoplaylist(CMediaGalleryApp* pMe,
                                      			AEEEvent eCode,
                                      			uint16 wParam,
                                      			uint32 dwParam)
{
	IMenuCtl* pMenuCtl;
    MediaDlgStat eDlgStat;
	MGFileInfo *pCurNode;
	 if(!pMe)
    {
       return FALSE;
    }
   	pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MENU_PLAYLIST);

    if(NULL == pMenuCtl)
      return FALSE;
    MSG_FATAL("MGAppPopupMenu_OnSavetoplaylisteCode=%d,wParam=%d",eCode,wParam,0);
    switch(eCode)
   {
      case EVT_DIALOG_INIT:
         return TRUE;

      case EVT_DIALOG_START:
         {
            
            MGFileInfo *pSelData = NULL;
            byte CurProfileNum;
            MGAppUtil_SetMenuCtlRectProp(pMe,
                                         MP_UNDERLINE_TITLE | MP_WRAPSCROLL,
                                         pMenuCtl); 
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
            {
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MSG_SAVEPLAYLIST,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            pMe->m_nSelNum = 0;
            
            MGAppUtil_BuildPlaylist(pMe,pMenuCtl);
            IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SAVE_BACK);
            //MGAppUtil_DrawSoftkey(pMe,BTBAR_SAVE_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);
            ISHELL_PostEvent(pMe->m_pShell,
                             AEECLSID_MEDIAGALLERY,
                             EVT_USER_REDRAW, 0, 0);
            return TRUE;
         }

      case EVT_DIALOG_END:
      {
         if(TRUE == pMe->m_bSuspending)
         {
         }
         return TRUE;
      }
      case EVT_USER_REDRAW:
         {
         	//IMENUCTL_Redraw(pMenuCtl);
         	IDISPLAY_Update(pMe->m_pDisplay);
            return TRUE;
         }
      case EVT_KEY_PRESS:
      case EVT_KEY_RELEASE:
      	   return TRUE;
      	   break;
      case EVT_COMMAND:
      	   {
      	   	  int i = wParam;
      	   	  int result = EFAILED;
      	   	  pMe->m_nPlaylistMusicNum = 0;
  	  		  result = MGAppUtil_ReadMusiclist(pMe,pMe->m_Playlist[i].pPlaylistName,FALSE);
  	  		  if(pMe->m_nPlaylistMusicNum<50)
  	  		  {
  	  		  	  int spareNum=0;
  	  		  	  spareNum = 50 - pMe->m_nPlaylistMusicNum;
	  	  		  pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName[0]=(pMe->m_nPlaylistMusicNum+48);
	  	  		  STRLCPY(pMe->m_Musiclist[pMe->m_nPlaylistMusicNum].pMusicName+1,
                  pCurNode->szName,256 * sizeof(char));
	  	  		  MGAppUtil_WriteMusiclist(pMe,pMe->m_Playlist[i].pPlaylistName);
	  	  		  MediaGalleryApp_ShowPromptMsgBox(pMe,
			                                       IDS_ADD_SUCCESS,
			                                       MESSAGE_INFORMATION,
			                                       BTBAR_BACK);
  	  		  }
  	  		  else
  	  		  {
  	  		  	  MediaGalleryApp_ShowPromptMsgBox(pMe,
			                                       IDS_PLAYLIST_FULL,
			                                       MESSAGE_INFORMATION,
			                                       BTBAR_BACK);
  	  		  }
			 
      	   }
      case EVT_KEY:
      	switch(wParam)
      	{
      		case AVK_CLR:
      			MGCLOSE_DIALOG(MGDLGRET_CANCELED);
      			return TRUE;
      			break;
      		default:
      			break;
      	}
      	break;
      default:
      	break;
    }
    return FALSE;
}

/*===========================================================================
 * FUNCTION: MGAppPopupMenu_OnRename
 *
 * DESCRIPTION: rename  file that select by menu control, now not support
 * rename directory
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnRename(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   ITextCtl* pEditText;
   MediaDlgStat eDlgStat;

   if(!pMe)
   {
      return FALSE;
   }

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   pEditText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                             IDC_MG_EDITNAME);

   if(NULL == pEditText)
   {
      return FALSE;
   }

   switch(eCode)
   {
   case EVT_DIALOG_INIT:
      {
         return TRUE;
      }

   case EVT_DIALOG_START:
      {
         AEEDeviceInfo DevInfo;
         AEERect rc;

         ISHELL_GetDeviceInfo(pMe->m_pShell, &DevInfo);

         SETAEERECT(&rc,  0,   0,  pMe->m_rc.dx,
                    DevInfo.cyScreen - GetBottomBarHeight(pMe->m_pDisplay));
         ITEXTCTL_SetRect(pEditText, &rc);

         ITEXTCTL_SetTitle(pEditText, MGRES_LANGFILE,
                           IDS_MG_RENAME ,NULL);

         ITEXTCTL_SetProperties(pEditText,
               TP_FRAME | TP_MULTILINE |TP_STARKEY_SWITCH | TP_DISPLAY_COUNT|TP_FOCUS_NOSEL | TP_GRAPHIC_BG);

         ITEXTCTL_SetMaxSize(pEditText, MG_MAX_FILE_INPUTLEN);

         if(pMe->m_pInputFileName)
         {
            ITEXTCTL_SetText(pEditText, pMe->m_pInputFileName, -1);
            FREEIF(pMe->m_pInputFileName);
         }
         else
         {
            AECHAR *pszOld = NULL;
            int nLen;
            MGFileInfo *pCurNode;
            char *pTempName;
            char *pszEnd;

            pCurNode = MediaGalleryApp_GetCurrentNode(pMe);
            pszOld = (AECHAR*)MALLOC(MG_MAX_FILE_NAME);


            if(!pCurNode || !pszOld) {
               MG_FARF(ADDR, ("Rename error: pCurNode or pszOld NULL!"));
               return FALSE;
            }
            MEMSET(pszOld, 0, MG_MAX_FILE_NAME);
            /*extract file name, and convert from UTF-8 code to AECHAR*/
            pTempName = BASENAME(pCurNode->szName);

            if(NULL == pTempName)
               pTempName = pCurNode->szName;

            pszEnd = STRRCHR(pTempName, MG_DOT_CHAR);

            if(pszEnd != NULL) {
               pMe->m_bHaveExt = TRUE;
               nLen = ABS(pszEnd - pTempName);
            }
            else {
               pMe->m_bHaveExt = FALSE;
               nLen = STRLEN(pTempName);
            }

            UTF8TOWSTR((const byte*)pTempName, nLen, pszOld,MG_MAX_FILE_NAME);
            ITEXTCTL_SetText(pEditText, pszOld, -1);
            FREEIF(pszOld);
         }

         ITEXTCTL_SetCursorPos(pEditText, TC_CURSOREND);
         MGAppUtil_DrawSoftkey(pMe, BTBAR_SAVE_DELETE);

         MGAppUtil_SetMediaDlgStat(pMe,MG_DLGSTAT_NORMAL);

         return TRUE;
      }

   case EVT_UPDATE_ARROW_BUTTON:
      {
         MGAppUtil_UpdateTxtEditSoftkey(pMe, pEditText);
         return TRUE;
      }
      break;

   case EVT_USER_REDRAW:
      {
          int nLen =0;
          AECHAR* pwstrText;
          pwstrText = ITEXTCTL_GetTextPtr(pEditText);
          if (NULL != pwstrText)
          {
             nLen = WSTRLEN(pwstrText);
          }
          
          if(nLen == 0)
          {
             MGAppUtil_DrawSoftkey(pMe, BTBAR_BACK);
          }
          else
          {
             MGAppUtil_DrawSoftkey(pMe, BTBAR_SAVE_BACK);
          }

      }
      return TRUE;

   case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending)
         {
            ;
         }

         //Store input name for suspend or fix focus item when rebuild media
         //menu
         FREEIF(pMe->m_pInputFileName);
         if(pMe->m_bSuspending == TRUE){
            pMe->m_pInputFileName = WSTRDUP(ITEXTCTL_GetTextPtr(pEditText));
         }

         return TRUE;
      }
   case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
         if(MG_DLGSTAT_NORMAL == eDlgStat)
         {
            MGAppPopupMenu_OperationDone(pMe, 0);
            MGCLOSE_DIALOG(MGDLGRET_CANCELED);
         }
         else if(MG_DLGSTAT_MESSAGEBOX == eDlgStat)
         {
            ITEXTCTL_SetActive(pEditText, TRUE);
            IDIALOG_Redraw(pMe->m_pActiveDlg);
            MGAppUtil_UpdateTxtEditSoftkey(pMe, pEditText);
         }
         return TRUE;

      case AVK_INFO:
      case AVK_SELECT:
         if(MG_DLGSTAT_NORMAL == eDlgStat)
         {
            char *pszNewName = NULL;
            char *pszNewPath = NULL;
            char *pszDelimiter = NULL;
            AECHAR *pwstrText = NULL;
            MGFileInfo *pCurNode = NULL;
            int nLen=0;
            int nRet = EFAILED;

            ITEXTCTL_SetActive(pEditText, FALSE);
            pwstrText = ITEXTCTL_GetTextPtr(pEditText);

            if (NULL != pwstrText) {
               nLen = WSTRLEN(pwstrText);
            }

            if (nLen > 0 )
            {
               if(0 == WSTRNCMP(pwstrText, L".", 1) ||
                  NULL != WSTRCHR(pwstrText, L'\\') ||
                  NULL != WSTRCHR(pwstrText, L'"') ||
                  NULL != WSTRCHR(pwstrText, L'/') ||
                  NULL != WSTRCHR(pwstrText, L'|') ||
                  NULL != WSTRCHR(pwstrText, L':') ||
                  NULL != WSTRCHR(pwstrText, L'?') ||
                  NULL != WSTRCHR(pwstrText, L'*') ||
                  NULL != WSTRCHR(pwstrText, L'<') ||
                  NULL != WSTRCHR(pwstrText, L'>'))
               {
                  MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                   IDS_MG_INVALIDNAME,
                                                   MESSAGE_INFORMATION,
                                                   BTBAR_BACK);
                  return FALSE;
               }

               pCurNode = MediaGalleryApp_GetCurrentNode(pMe);
               pszNewName = (char*)MALLOC(MG_MAX_FILE_NAME);
               /* WARNING: memory overflow.
                * do not use STRDUP, for pszNewPath use to store new file
                * path, it may exceed old file path. It result in Complain
                * AEEBRK_CORRUPTNODE, when free memory*/
               //pszNewPath = STRDUP(pCurNode->szName);
               pszNewPath = (char*)MALLOC(MG_MAX_FILE_NAME);
               STRCPY(pszNewPath, pCurNode->szName);

               if(!pszNewName || !pszNewPath)
                  return FALSE;

               WSTRTOUTF8(pwstrText,
                          sizeof(AECHAR) * (MG_MAX_FILE_INPUTLEN + 1),
                          (byte *)pszNewName, MG_MAX_FILE_NAME);

               if(TRUE == pMe->m_bHaveExt)
               {
                  pszDelimiter = STRRCHR(pCurNode->szName, MG_DOT_CHAR);
                  if(pszDelimiter )
                     STRCAT(pszNewName, pszDelimiter);
               }

               MG_FARF(ADDR, ("pwstrText :0X%X", pwstrText));

               pszDelimiter = BASENAME(pszNewPath);
               if(pszDelimiter)   {
                  STRCPY(pszDelimiter, pszNewName);

                  /*check whether new name if exist*/
                  if(SUCCESS == STRCMP(pszNewPath, pCurNode->szName)
                     ||SUCCESS == IFILEMGR_Test(pMe->m_pFileMgr, pszNewPath))
                  {
                     MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                      IDS_MG_REPETITION,
                                                      MESSAGE_INFORMATION,
                                                      BTBAR_BACK);
                     goto RENAMEDONEFREE;
                  }

                  nRet = IFILEMGR_Rename(pMe->m_pFileMgr,
                                         pCurNode->szName, pszNewPath);
               }
            }
            else
            {
               MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                IDS_MG_NULLNAME,
                                                MESSAGE_INFORMATION,
                                                BTBAR_BACK);
               return FALSE;
            }

            if(SUCCESS == nRet)
            {
               MGExplorer_UpdateCurrentMediaMenuItem(pMe, pszNewPath);
               MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);
            }
            else
            {
               MGAppUtil_HandleOperateFileError(pMe, pMe->m_pFileMgr);
            }

RENAMEDONEFREE:
            if(nRet == SUCCESS)
            {
               MGExplorer_ResetPreItemID(&pMe->m_Explorer, FALSE);
            }
            FREEIF(pszNewPath);
            FREEIF(pszNewName);
         }
         return TRUE;

      default:
         break;
      }

   case EVT_KEY_RELEASE:
      return MGAppUtil_UpdateTxtEditSoftkey(pMe, pEditText);

   case EVT_DISPLAYDIALOGTIMEOUT:
      {
         uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

         MG_FARF(ADDR, ("display dialog time out"));
         if(nMsgBoxId == MG_MSGID_DONE)
         {
            MGCLOSE_DIALOG(MGDLGRET_CANCELED);
            MGAppPopupMenu_OperationDone(pMe, 0);
         }
         else
         {
            ITEXTCTL_SetActive(pEditText, TRUE);
            IDIALOG_Redraw(pMe->m_pActiveDlg);
            MGAppUtil_UpdateTxtEditSoftkey(pMe, pEditText);
         }
         return TRUE;
      }
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
   case EVT_PEN_UP:
	   {
		   AEEDeviceInfo devinfo;
		   int nBarH ;
		   AEERect rc;
		   int16 wXPos = (int16)AEE_GET_X(dwParam);
		   int16 wYPos = (int16)AEE_GET_Y(dwParam);

		   nBarH = GetBottomBarHeight(pMe->m_pDisplay);
   
		   MEMSET(&devinfo, 0, sizeof(devinfo));
		   ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
		   SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

		   if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
		   {
			   if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
			   {
				   boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_SELECT,0);
				   return rt;
			   }
			   else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
			   {					   
					boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_CLR,0);
					return rt;
			   }
		   }

	   }
	   break;
#endif 

   default:
      break;
   }

   return FALSE;
}//MGAppPopupMenu_OnRename

/*===========================================================================
 * FUNCTION: MGAppupMenu_OnDelete
 *
 * DESCRIPTION: Delete file that select by the menu control
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppPopupMenu_OnDelete(CMediaGalleryApp* pMe,
                                    IMenuCtl* pMenuCtl)
{
   MGFileInfo *pItemData;
   uint16   SelItemID;
   boolean  bSysDir = FALSE;
   int nRet = EFAILED;

   if(!pMe || !pMenuCtl)
      return EBADPARM;

   pItemData = MediaGalleryApp_GetCurrentNode(pMe);

   SelItemID = IMENUCTL_GetSel(pMenuCtl);
   if(!pItemData)
   {
      MG_FARF(ADDR, ("DATE IS NULL "));
      return EFAILED;
   }

   if(pItemData && pItemData->szName)
   {
      if( AEE_FA_DIR != pItemData->attrib )
      {
         if(MG_FILE_PROTECT(pItemData->attrib))
         {
            /*FILE IS PROTECT*/
            MG_FARF(ADDR, ("Delete warning: file is protect!"));

            MediaGalleryApp_ShowPromptMsgBox(pMe,
                                             IDS_MG_PROTECT,
                                             MESSAGE_INFORMATION,
                                             BTBAR_BACK);
            return FALSE;
         }

         /*
          *  if(SUCCESS != IFILEMGR_CheckPathAccess(pMe->m_pFileMgr,
          *  pItemData->m_szName,AEEFP_DELETE, NULL))
          *  {
          *    return FALSE;
          *  }
          *
          */

         nRet = IFILEMGR_Remove(pMe->m_pFileMgr, pItemData->szName);
      }
      else
      {
         bSysDir = MGExplorer_IsSysDirInMassStorage(pItemData->szName,
               pMe->m_StoreMedium);
         if(FALSE == bSysDir)
         {
            nRet = IFILEMGR_RmDir(pMe->m_pFileMgr, pItemData->szName);
         }
      }

      if(SUCCESS == nRet)
      {
         FREEIF(pItemData);
         IMENUCTL_DeleteItem(pMenuCtl, SelItemID);
         pMe->m_bMediaMenuEmpty =
	         MGExplorer_CheckMediaMenuEmpty(pMenuCtl);

         MGExplorer_UpdateMediaMenuTitle(pMe, pMe->m_pMediaMenu);
         MGAppUtil_GetMediaMenuSelectItem(pMe);

         //TODO: need to check the file whether set to ringtone or wallpaper
         //if yes, reset the settings to default
         MediaGalleryApp_ShowDoneMsgBox(pMe);
      }
      else
      {
         MGAppUtil_HandleOperateFileError(pMe, pMe->m_pFileMgr);
      }
   }
   return SUCCESS;
}//MGAppPopupMenu_OnDelete


/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppPopupMenu_OnDeleteAll(CMediaGalleryApp* pMe,
                                       IMenuCtl* pMenuCtl)
{
   MGFileInfo *pItemData;
   int      nCount;
   int      nIdx;
   int      nRet;
   uint16   uItemID;
   boolean  bDelAll = TRUE;
   boolean  bSysDir = FALSE;
   boolean  bHaveConstFile = FALSE;

   if(!pMe || !pMenuCtl)
      return EBADPARM;

   nCount = IMENUCTL_GetItemCount(pMenuCtl);

   if(nCount < 1)
   {
      MG_FARF(ADDR, ("There are no file to delete"));
      return EBADPARM;
   }

   IMENUCTL_Redraw(pMenuCtl);
   //show progress bar
   MediaGalleryApp_ShowProgressBox(pMe, MG_MSGID_DELALL);

   /*it is simple, we can just do the following step by step in the callback,
    * until all item in the menu control
    * */
   for(nIdx = nCount - 1; nIdx >= 0 ; nIdx--)
   {
      uItemID = IMENUCTL_GetItemID(pMenuCtl, nIdx);
      IMENUCTL_GetItemData(pMenuCtl,uItemID, (uint32*)&pItemData);
      if(pItemData && pItemData->szName)
      {
         if(TRUE == MG_FILE_PROTECT(pItemData->attrib))
         {
            bDelAll = FALSE;
            bHaveConstFile = TRUE;
            FREEIF(pItemData);
            continue;
         }

         if( AEE_FA_DIR == pItemData->attrib )
         {
            bSysDir = MGExplorer_IsSysDirInMassStorage(pItemData->szName,
                                                       pMe->m_StoreMedium);
            nRet = EINVALIDOPERATION;

            if(FALSE == bSysDir)
            {
               nRet = IFILEMGR_RmDir(pMe->m_pFileMgr, pItemData->szName);
            }

            if(nRet == EFAILED || TRUE == bSysDir)
            {
               bDelAll = FALSE;
            }
         }
         else
         {
            nRet = IFILEMGR_Remove(pMe->m_pFileMgr, pItemData->szName);

            /*
             * Notice: If we delete item one by one, not delete all at once,
             * and get item from 0 to max, it will ***crash***. In this
             * condition, we will get item form max to 0.  */
            if(nRet == EFAILED)
            {
               bDelAll = FALSE;
            }
         }

         FREEIF(pItemData);
      }
   }

   /*
    * do not use IMENUCTL_DeleteItem(pMediaMenu, uItemID); it will be do
    * much more thing, and result in very slowly. We also can not delete
    * asynchronous, because IMENUCTL_DeleteItem will redraw. What we can do is
    * free item data, and set item data to null, it seem that can not improve
    * efficient.
    */
   IMENUCTL_DeleteAll(pMenuCtl);
   IMENUCTL_SetActive(pMenuCtl, FALSE);
   //TODO: need to check the file whether set to ringtone or
   //wallpaper. if yes, reset the settings to default

   if(TRUE == bDelAll)
   {
      pMe->m_bMediaMenuEmpty = TRUE;
      MediaGalleryApp_ShowDoneMsgBox(pMe);
   }
   else if (TRUE == bHaveConstFile)
   {
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_FILENOTDEL,
                                       MESSAGE_ERR,
                                       BTBAR_BACK);
   }
   else
   {
      MGAppUtil_HandleOperateFileError(pMe, pMe->m_pFileMgr);
   }

   return SUCCESS;
}//MGAppPopupMenu_OnDeleteAll


/*===========================================================================
 * FUNCTION:    MGAppPopupMenu_OnBluetooth
 *
 * DESCRIPTION: （启动）蓝牙传输选中的文件。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppPopupMenu_OnBluetooth(CMediaGalleryApp* pMe,
                                       IMenuCtl* pMenuCtl)
{
  MGFileInfo *pSelData;
  //char szArg[MG_MAX_FILE_NAME + 2] = {'F','\0'};   
  char szArg[MG_MAX_FILE_NAME + 1] = {'\0'};   

  if (!pMe || !pMenuCtl)
  {
  	return EBADPARM;
  }

  pSelData = MediaGalleryApp_GetCurrentNode(pMe);

  if(!pSelData || !pSelData->szName)
  {
     MG_FARF(ADDR, ("Date is validate"));
     return EBADPARM;
  }

  STRCAT(szArg, pSelData->szName);

//Add By zzg 2010_11_03
#ifdef FEATURE_BT_SEND_FILE_ONLY 
  return ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, szArg);
#else
  return FALSE;
#endif
//Add End

/*  
#ifdef FEATURE_SUPPORT_BT_APP
  return ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BTUIAPP, szArg);
#else
  return FALSE;
#endif
*/

}//MGAppPopupMenu_OnBluetooth

/*===========================================================================
 * FUNCTION:   MGAppPopupMenu_OnCopy
 *
 * DESCRIPTION:copy file (not protect)
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppPopupMenu_OnCopy(CMediaGalleryApp* pMe,
                                    IMenuCtl* pMenuCtl)
{
   int nRet = SUCCESS;

   if(!pMe || !pMenuCtl)
      return EBADPARM;

   nRet = MGAppUtil_DoCopyFile(pMe);

   /*这里要把copy的数据结构释放掉，是因为文件系统读写操作的缓冲，
    * 如果不释放文件句柄，那么拷贝后，去查看文件，数据没有真正写到文件里，特别
    * 是对小文件，查看到文件大小是0，那么导致不能查看图片（如果是图片的话）。
    * */
   MG_CLEAR_COPYDATA(pMe->m_Copy);

   if(nRet == EFILEEOF)
   {
      MediaGalleryApp_ShowDoneMsgBox(pMe);
   }
   else if(nRet == SUCCESS)
   {
      ;
   }

   return nRet;
}//MGAppPopupMenu_OnCopy


/*
 * ==========================================================================
 * FUNCTION     :  MGAppPopupMenu_OnMove
 * DESCRIPTION  :  移动文件。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static int MGAppPopupMenu_OnMove(CMediaGalleryApp *pMe,
                                    IMenuCtl *pMenuCtl)
{
   MGFileInfo *pSelData;
   int nRet = SUCCESS;

   if(!pMe || !pMenuCtl)
      return EFAILED;

   if(!pMe->m_pFileMgr)
   {
      MG_FARF(ADDR, ("Date is validate"));
      return EBADPARM;
   }

   nRet = MGAppUtil_DoCopyFile(pMe);

   if(nRet == EFILEEOF)
   {
      /*copy success!*/
      pSelData = MediaGalleryApp_GetCurrentNode(pMe);

      if(!pSelData || !pSelData->szName)
      {
         MG_FARF(ADDR, ("Date is validate"));
         return EBADPARM;
      }

      /*close the source file and delete it, then release other resource*/
      MG_CLEAR_COPYDATA(pMe->m_Copy);
      if(EFAILED == IFILEMGR_Remove(pMe->m_pFileMgr, pSelData->szName))
      {
         MGAppUtil_HandleOperateFileError(pMe, pMe->m_pFileMgr);
      }
      else
      {
         uint16   SelItemID;
         MGFileInfo *pItemData;

         SelItemID = IMENUCTL_GetSel(pMenuCtl);
         IMENUCTL_GetItemData(pMenuCtl,SelItemID, (uint32*)&pItemData);
         /*in actually pItemData is the same as pMe->m_Explorer.m_pCurNode */
         FREEIF(pItemData);
         IMENUCTL_DeleteItem(pMenuCtl, SelItemID);

         MediaGalleryApp_ShowDoneMsgBox(pMe);
      }
   }
   else if(nRet == SUCCESS)
   {
      //reserve for async call
      ;
   }
   else
   {
      MG_CLEAR_COPYDATA(pMe->m_Copy);
   }

   return nRet;
}//MGAppPopupMenu_OnMove



/*
 * ==========================================================================
 * FUNCTION     :  MGAppPopupMenu_OnSelectPath
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnSelectPath(CMediaGalleryApp *pMe,
                                         AEEEvent eCode,
                                         uint16 wParam,
                                         uint32 dwParam)
{
   IMenuCtl* pMenuCtl = NULL;

   if(NULL == pMe)
      return FALSE;

   pMenuCtl = pMe->m_pPathMenu;

   if(NULL == pMenuCtl && eCode != EVT_DIALOG_INIT)
   {
      return FALSE;
   }

   switch(eCode)
   {
   case EVT_DIALOG_INIT:
      {
         AEECLSID cls = AEECLSID_MENUCTL;

         if(pMe->m_Explorer.m_eView == MG_VIEW_GRID)
            cls = AEECLSID_ICONVIEWCTL;

         if(NULL == pMenuCtl)
         {
            if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                cls,
                                                (void **)&pMenuCtl))
            {
               MG_FARF(ADDR, ("Create media menu failed!"));
               return FALSE;
            }

            pMe->m_pPathMenu = pMenuCtl;
         }

         if (NULL == pMe->m_pPathExp)
         {
            pMe->m_pPathExp = (CFSExplorer *)MALLOC(sizeof(CFSExplorer));
            MEMSET(pMe->m_pPathExp, 0, sizeof(CFSExplorer));
         }
      }
      return TRUE;

   case EVT_DIALOG_START:
      {
         int nItemNum = IMENUCTL_GetItemCount(pMenuCtl);

         if (0 == nItemNum)
         {
            MGAppUtil_SetMenuCtlRectProp(pMe,
                                         MP_UNDERLINE_TITLE |
                                         MP_WRAPSCROLL |
                                         MP_ALLOW_DUPE_IDS |
                                         MP_NO_REDRAW,
                                         pMenuCtl);
            IMENUCTL_SetOemProperties(pMenuCtl,
                                      OEMMP_DISTINGUISH_INFOKEY_SELECTKEY |
                                      OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl,
                                   AEE_APPSCOMMONRES_IMAGESFILE,
                                   IDI_MEDIA_BACKGROUND);
#endif

            /*add menu item*/
            if (0 == MGExplorer_GetCurrentDepth(pMe->m_pPathExp) )
            {
               MGMENU_ADDITEM(pMenuCtl, IDS_MG_PHONEMEMORY);
               MGMENU_ADDITEM(pMenuCtl, IDS_MG_CARDMEMORY);
            }
            {
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_SELECTPATH,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }

            //IMENUCTL_SetTitle(pMenuCtl, MGRES_LANGFILE, IDS_MG_SELECTPATH, NULL);
            IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
         }

         IMENUCTL_SetActive(pMenuCtl, TRUE);
         IMENUCTL_Redraw(pMenuCtl);

         return TRUE;
      }

   case EVT_DIALOG_END:
      //if we are suspending (EVT_APP_SUSPEND is sent before
      //EVT_DIALOG_END). Handle suspend for current status.
      if(pMe->m_bSuspending == FALSE)
      {
         MGExplorer_FreeMediaMenuItem(pMe->m_pPathMenu);
         RELEASEIF(pMe->m_pPathMenu);

         if(NULL != pMe->m_pPathExp)
         {
            MEMSET(pMe->m_pPathExp, 0, sizeof(pMe->m_pPathExp));
            FREE(pMe->m_pPathExp);
            pMe->m_pPathExp = NULL;
         }
      }
      else
      {
         IMENUCTL_SetActive(pMenuCtl, FALSE);
      }

      return TRUE;

   case EVT_KEY:
      {
         CFSExplorer *pFSExp = pMe->m_pPathExp;
         const char *pszPath = NULL;
         uint8    nCurDepth = MGExplorer_GetCurrentDepth(pFSExp);
         uint16   SelItemID;

         switch(wParam)
         {
         case AVK_CLR:
            if (nCurDepth == 0)
            {
               MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_NULL);
               MGCLOSE_DIALOG(MGDLGRET_CANCELED);
            }
            else if(nCurDepth == MG_CURDEPTH_INIT)
            {
               MGExplorer_SetCurrentDepth(pFSExp, 0);
               MGExplorer_FreeMediaMenuItem(pMenuCtl);
               MGMENU_ADDITEM(pMenuCtl, IDS_MG_PHONEMEMORY);
               MGMENU_ADDITEM(pMenuCtl, IDS_MG_CARDMEMORY);
               if (0 == STRCMP(pFSExp->m_RootNode.szName, MG_MASSCARD_ROOTDIR))
               {
                  IMENUCTL_SetFocus(pMenuCtl, IDS_MG_CARDMEMORY);
               }
               IMENUCTL_Redraw(pMenuCtl);
            }
            else
            {
               /*假如当前路径下没有目录，则返回到上一级路径时把Menu控件激活并
                * 修改软件行*/
               if ( FALSE == IMENUCTL_IsActive(pMenuCtl))
               {
                  IMENUCTL_SetActive(pMenuCtl, TRUE);
               }

               if (TRUE == MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
               {
                  IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
               }

               pszPath = MGExplorer_GetCurrentFolderPath(pFSExp);
               if(SUCCESS == MGExplorer_ChangeCurDir(pFSExp,
                                                     MG_DIR_GOUP,
                                                     pszPath))
               {
                  MGExplorer_BuildSubfolderMenu(pFSExp,
                                                pMe->m_pFileMgr,
                                                pMenuCtl,
                                                pMe->m_pFileIcons);
               }
            }
            return TRUE;

         case AVK_INFO:
            if (nCurDepth == 0)
            {
               const char* pDirs = NULL;

               SelItemID = IMENUCTL_GetSel(pMenuCtl);
               if(SelItemID == IDS_MG_PHONEMEMORY)
               {
                  pDirs = MG_PHONE_ROOTDIR;
               }
               else if(SelItemID == IDS_MG_CARDMEMORY)
               {
                  if (pMe->m_bCardExist == TRUE)
                  {
                     pDirs = MG_MASSCARD_ROOTDIR;
                  }
                  else
                  {
                     /*SD card do not exist*/
                     MGCLOSE_DIALOG(MGDLGRET_NOEXTCARD);
                     return TRUE;
                  }
               }
               pFSExp->m_eView = MG_VIEW_DEFAULT;

               MGExplorer_Init(pFSExp,
                               pMe->m_pFileMgr,
                               pDirs,
                               MG_MIME_MISCELL);
            }
            else
            {
            	
               if (TRUE == MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
               {
                  return TRUE;
               }

               pszPath = MGExplorer_GetCurrentNodeName(pFSExp);
               if (SUCCESS != MGExplorer_ChangeCurDir(pFSExp,
                                                      MG_DIR_GODOWN,
                                                      pszPath))
               {
                  IMENUCTL_SetActive(pMenuCtl, FALSE);
                  MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                   IDS_MG_DEEPPATH,
                                                   MESSAGE_INFORMATION,
                                                   BTBAR_BACK);

                  break;
               }
            }

            MGExplorer_BuildSubfolderMenu(pFSExp,
                                          pMe->m_pFileMgr,
                                          pMenuCtl,
                                          pMe->m_pFileIcons);

            if (TRUE == MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
            {
               IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_BACK);
               /* 把菜单设为deactive， 否则向上移动，再退回时会死机。*/
               IMENUCTL_SetActive(pMenuCtl, FALSE);
               IMENUCTL_Redraw(pMenuCtl);

               MGAppUtil_OnMediaMenuEmpty(pMe, MG_ONMENUEMPTY_STAYEMPTY);
            }
            return TRUE;

         case AVK_SELECT:
            {
               uint32 dwFree;
               const char *cpszSrcPath = NULL;
               char *pDestFileName = NULL;
               FileInfo Info;

               if (TRUE == MGExplorer_CheckMediaMenuEmpty(pMenuCtl))
               {
                  return TRUE;
               }

               if (nCurDepth == 0)
               {
                  const char* pDirs = NULL;

                  SelItemID = IMENUCTL_GetSel(pMenuCtl);
                  if(SelItemID == IDS_MG_PHONEMEMORY)
                  {
                     pDirs = MG_PHONE_ROOTDIR;
                  }
                  else if(SelItemID == IDS_MG_CARDMEMORY)
                  {
                     if (pMe->m_bCardExist == TRUE)
                     {
                        pDirs = MG_MASSCARD_ROOTDIR;
                     }
                     else
                     {
                        /*SD card do not exist*/
                        MGCLOSE_DIALOG(MGDLGRET_NOEXTCARD);
                        return TRUE;
                     }
                  }
                  pFSExp->m_eView = MG_VIEW_DEFAULT;

                  MGExplorer_Init(pFSExp,
                                  pMe->m_pFileMgr,
                                  pDirs,
                                  MG_MIME_MISCELL);
                  MGExplorer_BuildSubfolderMenu(pFSExp,
                                                pMe->m_pFileMgr,
                                                pMenuCtl,
                                                pMe->m_pFileIcons);
               }
               else
               {
               	  
                  cpszSrcPath = MediaGalleryApp_GetCurrentNodeName(pMe);

                  if ( NULL == cpszSrcPath)
                  {
                     MG_FARF(ADDR,( "Source path is null"));
                     return FALSE;
                  }

                  IFILEMGR_GetInfo(pMe->m_pFileMgr, cpszSrcPath, &Info);

                  pszPath = MGExplorer_GetCurrentNodeName(pFSExp);
                  IFILEMGR_GetFreeSpaceEx(pMe->m_pFileMgr, pszPath, NULL, &dwFree);

                  if(dwFree < Info.dwSize + MG_FS_PADD)
                  {
                     MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                      IDS_MG_INSUFFICIENT,
                                                      MESSAGE_INFORMATION,
                                                      BTBAR_BACK);

                     MG_FARF(ADDR, ("File system is full!!!!"));
                     return TRUE;
                  }

                  pDestFileName = (char *)MALLOC(sizeof(char) * MG_MAX_FILE_NAME);

                  if(NULL == pDestFileName)
                  {
                     MG_FARF(ADDR, ("MALLOC failed!"));
                     return FALSE;
                  }

                  MGAppUtil_GetDestFilePath(cpszSrcPath,
                                            pszPath,
                                            pDestFileName,
                                            sizeof(char) * MG_MAX_FILE_NAME);

                  FREEIF(pMe->m_Copy.m_pszDestName);
                  FREEIF(pMe->m_Copy.m_pszSrcName);
                  pMe->m_Copy.m_pszDestName = pDestFileName;
                  pMe->m_Copy.m_pszSrcName = STRDUP(cpszSrcPath);

                  MGCLOSE_DIALOG(MGDLGRET_DONE);
               }

               return TRUE;
            }

         default:
            IMENUCTL_HandleEvent(pMenuCtl, eCode, wParam, dwParam);
            break;
         }
      }
      break;

   case EVT_CTL_SEL_CHANGED:
      {
         uint8    nCurDepth = MGExplorer_GetCurrentDepth(pMe->m_pPathExp);

         if (nCurDepth >= MG_CURDEPTH_INIT)
         {
            IMENUCTL_GetItemData(pMenuCtl,
                                 wParam,
                                 (uint32*)&pMe->m_pPathExp->m_pCurNode);
            MGExplorer_GetMediaMenuSelectItem(pMe->m_pPathExp, pMenuCtl);
         }
      }
      return TRUE;

   case EVT_DISPLAYDIALOGTIMEOUT:
      {
         MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT",0,0,0);
         if(FALSE == IMENUCTL_IsActive(pMenuCtl))
         {
            IMENUCTL_SetActive(pMenuCtl, TRUE);
         }
         IMENUCTL_Redraw(pMenuCtl);
      }
      return TRUE;

   default:
      break;
   }

   return FALSE;
}//MGAppPopupMenu_OnSelectPath


/*
 * ==========================================================================
 * FUNCTION     :  MGAppPopupMenu_OnSort
 * DESCRIPTION  :  set sort type which used by build files/folders list
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnSort(CMediaGalleryApp* pMe,
                              AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl;

   if(!pMe)
   {
      return FALSE;
   }

   pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MG_SORTMENU);

   if(NULL == pMenuCtl)
      return FALSE;

   switch(eCode)
   {
   case EVT_DIALOG_INIT:
      return TRUE;

   case EVT_DIALOG_START:
      {
         MGSortOrder eSortOrder = pMe->m_Explorer.m_eSortOrder;

         if(
#ifdef FEATURE_SORT_TYPE
            eSortOrder != MG_SORT_TYPE &&
#endif
            eSortOrder != MG_SORT_NONE &&
            eSortOrder != MG_SORT_NAME &&
            eSortOrder != MG_SORT_DATE &&
            eSortOrder != MG_SORT_SIZE
           )
         {
            eSortOrder = MG_SORT_NONE;
            pMe->m_Explorer.m_eSortOrder = eSortOrder;
         }

         MGAppUtil_SetMenuCtlRectProp(pMe,
                                      MP_UNDERLINE_TITLE | MP_WRAPSCROLL |
                                      MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT,
                                      pMenuCtl);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

		 //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,                                
                        IDS_MG_SORT,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_BYNULL);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_BYNAME);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_BYDATE);
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_BYSIZE);
#ifdef FEATURE_SORT_TYPE
         MGMENU_ADDITEM(pMenuCtl, IDS_MG_BYTYPE);
         SetMenuIcon(pMenuCtl, IDS_MG_BYTYPE,
                     (boolean)(eSortOrder == MG_SORT_TYPE));

#endif
         SetMenuIcon(pMenuCtl, IDS_MG_BYSIZE,
                     (boolean)(eSortOrder == MG_SORT_SIZE));
         SetMenuIcon(pMenuCtl, IDS_MG_BYDATE,
                     (boolean)(eSortOrder == MG_SORT_DATE));
         SetMenuIcon(pMenuCtl, IDS_MG_BYNAME,
                     (boolean)(eSortOrder == MG_SORT_NAME));
         SetMenuIcon(pMenuCtl, IDS_MG_BYNULL,
                     (boolean)(eSortOrder == MG_SORT_NONE));

         //IMENUCTL_SetTitle(pMenuCtl, MGRES_LANGFILE, IDS_MG_SORT, NULL);
         IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SAVE_BACK);

         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
                          EVT_USER_REDRAW, 0, 0);

         return TRUE;
      }

   case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending)
         {
            ;
         }

         MGAppPopupMenu_OperationDone(pMe, 0);
         return TRUE;
      }

   case EVT_USER_REDRAW:
      {
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
	  case EVT_USER:
#endif
   case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
         MGCLOSE_DIALOG(MGDLGRET_CANCELED);
         return TRUE;

      default:
         break;
      }
      break;

   case EVT_COMMAND:
      {
         MGSortOrder eSortOrder = MG_SORT_NONE;

         switch(wParam)
         {
         case IDS_MG_BYNULL:
            eSortOrder =MG_SORT_NONE;
            break;

         case IDS_MG_BYNAME:
            eSortOrder = MG_SORT_NAME;
            break;

         case IDS_MG_BYDATE:
            eSortOrder = MG_SORT_DATE;
            break;

         case IDS_MG_BYSIZE:
            eSortOrder = MG_SORT_SIZE;
            break;

         case IDS_MG_BYTYPE:
            eSortOrder = MG_SORT_TYPE;
            break;

         default:
            break;
         }

         if(pMe->m_Explorer.m_eSortOrder != eSortOrder)
         {
            pMe->m_Explorer.m_eSortOrder = eSortOrder;
            MediaGalleryApp_ShowProgressBox(pMe, MG_MSGID_RESORT);
            MGExplorer_MediaMenuSort(pMe, pMe->m_pMediaMenu, eSortOrder);
            MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

         }

         IMENUCTL_SetActive(pMenuCtl, FALSE);

         MediaGalleryApp_ShowDoneMsgBox(pMe);

      }
      return TRUE;

   case EVT_DISPLAYDIALOGTIMEOUT:
      MGCLOSE_DIALOG(MGDLGRET_CANCELED);
      return TRUE;

   default:
      break;
   }
   return FALSE;
}//MGAppPopupMenu_OnSort

/*===========================================================================
 * FUNCTION:MGAppPopupMenu_OnDetail
 *
 * DESCRIPTION: display detail information of file and folder.
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppPopupMenu_OnDetail(CMediaGalleryApp* pMe,
                                       AEEEvent eCode,
                                       uint16 wParam,
                                       uint32 dwParam)
{
   IStatic *pDetailText;

   if(!pMe)
      return FALSE;

   pDetailText = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                               IDC_MG_DETAILTEXT);

   if(!pDetailText)
      return FALSE;

   switch(eCode)
   {
   case EVT_DIALOG_INIT:
      {
         AEERect rc;
         AECHAR szDetail[MG_MAX_LINETEXTNUM];
         AECHAR *pszTitle;
         const char *pszFileName;
         const char *pszPath;
         IMenuCtl *pMenuCtl;

         pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                  IDC_MG_DETAIL);

         if(!pMenuCtl)
            return FALSE;

         ISHELL_LoadResString(pMe->m_pShell,
                              MGRES_LANGFILE,
                              IDS_MG_DETAIL,
                              szDetail,
                              sizeof(szDetail));

         pszPath = MediaGalleryApp_GetCurrentNodeName(pMe);
         pszFileName = BASENAME(pszPath);

         pszTitle = (AECHAR *)MALLOC(MG_MAX_FILE_NAME);
         if(!pszFileName || !pszTitle)
         {
            MG_FARF(ADDR, ("Memory allocte failed."));
            return FALSE;
         }

         UTF8TOWSTR((const byte*)pszFileName,
                    STRLEN(pszFileName) + 1,
                    pszTitle,
                    MG_MAX_FILE_NAME);

         WSTRCAT(pszTitle, L" ");
         WSTRLOWER(szDetail);
         WSTRCAT(pszTitle, szDetail);

         rc = pMe->m_rc;
         rc.dy =  GetTitleBarHeight(pMe->m_pDisplay);
         IMENUCTL_SetTitle(pMenuCtl, NULL, 0, pszTitle);
         IMENUCTL_SetRect(pMenuCtl, &rc);
         IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE | MP_WRAPSCROLL);
         IMENUCTL_SetActive(pMenuCtl, TRUE);

         rc.y += rc.dy;
         rc.dy = pMe->m_rc.dy - rc.y - GetBottomBarHeight(pMe->m_pDisplay);

         ISTATIC_SetRect(pDetailText, &rc);
         ISTATIC_SetProperties(pDetailText, ST_MIDDLETEXT|ST_GRAPHIC_BG);
         ISTATIC_SetBackGround(pDetailText, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);//modified by yangdecai
         
         RELEASEIF(pMe->m_pDetailText);
         pMe->m_pDetailText = pDetailText;
         ISTATIC_AddRef(pMe->m_pDetailText);

         FREEIF(pszTitle);
         return TRUE;
      }

   case EVT_DIALOG_START:
      {
         //      int nRet;
         //       boolean bRet;
         MGFileInfo *pCurNode;

         pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

         if(pCurNode->attrib == AEE_FA_DIR)
         {
            uint8 nState;

            nState = MediaGalleryApp_GetCallbackState(pMe,
                                                      pMe->m_nCallbackDoFor);

            if(nState == MG_CBS_DOING)
            {
               MG_FARF(ADDR, ("call back still doing"));
               return FALSE;
            }
            nState = MediaGalleryApp_GetCallbackState(pMe,
                                                      MG_CBT_GETFOLDERINFO);

            if(MG_CBS_STOP == nState)
            {
               MGAppUtil_GetFolderInfoNotify((void *)pMe);
               ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
            }
            else
            {
               MGExplorer_InitGetFolderInfo(pMe,
                                            MGAppUtil_GetFolderInfoNotify);
            }
         }
         else
         {
            AECHAR szTemp[MG_MAX_LINETEXTNUM];
            AECHAR szCont[MG_MAX_LINETEXTNUM];
            AECHAR szMemUnit[MG_MEMUNIT_STRLENMAX];
            AECHAR szAttrib[MG_MAX_LINETEXTNUM];
            AECHAR *pszBuffer;
            JulianType jdate;

            pszBuffer = (AECHAR*) MALLOC(sizeof(AECHAR) * MG_MAX_TEXT_SIZE);
            if(!pszBuffer)
            {
               MG_FARF(ADDR, ("Memory failed!"));
               return FALSE;
            }

            MEMSET(pszBuffer, 0, sizeof(AECHAR) * MG_MAX_TEXT_SIZE);

            if(pCurNode->dwCreationDate != 0)
            {
               GETJULIANDATE(pCurNode->dwCreationDate, &jdate);
            }
            else
            {
               jdate.wYear = MG_DEFAULT_JYEAR;
               jdate.wMonth = MG_DEFAULT_JMONTH;
               jdate.wDay = MG_DEFAULT_JDAY;
            }

            ISHELL_LoadResString(pMe->m_pShell,
                                 MGRES_LANGFILE,
                                 IDS_MG_CREATIONDATE,
                                 szTemp,
                                 sizeof(szTemp));
            WSPRINTF(szCont, sizeof(szCont), szTemp,
                     jdate.wYear, jdate.wMonth, jdate.wDay);
            WSTRCAT(pszBuffer, szCont);
            WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);

            MGAppUtil_SpaceUnitConvert(pMe->m_pShell,
                                       pCurNode->dwSize,
                                       szMemUnit,
                                       sizeof(szMemUnit));
            ISHELL_LoadResString(pMe->m_pShell,
                                 MGRES_LANGFILE,
                                 IDS_MG_SIZE,
                                 szCont,
                                 sizeof(szCont));
            WSTRCAT(szCont, szMemUnit);
            WSTRCAT(pszBuffer, szCont);
            WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);

            MGAppUtil_FileAttribConvert(pMe->m_pShell,
                                        pCurNode->attrib,
                                        szAttrib,
                                        sizeof(szAttrib));
            ISHELL_LoadResString(pMe->m_pShell,
                                 MGRES_LANGFILE,
                                 IDS_MG_ATTRIB,
                                 szCont,
                                 sizeof(szCont));
            //WARNING:WSPRINTF do not support %S,   encode error!!!
            //WSPRINTF(szCont, sizeof(szCont),szTemp,szAttrib);
            WSTRCAT(szCont, szAttrib);
            WSTRCAT(pszBuffer, szCont);

            ISTATIC_SetText(pDetailText,
                            NULL/* L"File"*/,
                            pszBuffer,
                            AEE_FONT_NORMAL,
                            AEE_FONT_NORMAL);
            MG_FARF(ADDR, ("file detail:%S", pszBuffer));

            FREEIF(pszBuffer);
         }

#if 0
         if(pCurNode->attrib == AEE_FA_DIR)
         {
            //TODO:display directory attribute
         }
         else
         {
            MGMimeType     eMimeBase;

            eMimeBase = MediaGalleryApp_GetExplorerMime(pMe);

            if(eMimeBase == MG_MIME_MISCELL ||
               !MG_BETWEEN(eMimeBase,MG_MIME_MISMAX,MG_MIME_MAX))
            {
               uint16     uMimeBase;

               uMimeBase = MGMediaInfo_MimeBaseConvert(eMimeBase);
               /*if eMimeBase is MG_MIME_UNKNOWN etc, we need detect each
                * type*/
               nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                              pMe->m_pFileMgr,
                                              pCurNode->szName,
                                              uMimeBase,
                                              &eMimeBase);

               if(SUCCESS != nRet)
                  return FALSE;
            }

            if(MG_BETWEEN(eMimeBase, MG_MIME_IMGBASE, MG_MIME_IMGMAX))
            {

            }
            else if(MG_BETWEEN(eMimeBase, MG_MIME_SNDBASE, MG_MIME_SNDMAX) ||
                    (eMimeBase == MG_MIME_VOICEREC) || (eMimeBase == MG_MIME_CALLREC))
            {
#ifdef AEE_SIMULATOR
               MGInfo_ID3V1 Mp3Tag;
               IFile *pIFile;

               pIFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, pCurNode->szName,_OFM_READ );

               if(NULL == pIFile)
               {
                  MG_FARF(ADDR, ("Open media file failed!"));
                  return FALSE;
               }

               MGMediaInfo_GetID3V2(pIFile);
               MGMediaInfo_GetID3V1(pIFile, &Mp3Tag);
               RELEASEIF(pIFile);
#else
               AEEMediaData md;
               boolean bStateChange;
               //AEECLSID cls = 0;

               md.clsData = MMD_FILE_NAME;
               md.pData = pCurNode->szName;
               md.dwSize = 0;

               RELEASEIF(pMe->m_pMedia);
               nRet = AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);

               if(nRet != SUCCESS)
               {
                  MG_FARF(ADDR, ("Create Media Failed!"));
                  return FALSE;
               }

               IMEDIA_RegisterNotify(pMe->m_pMedia, MGAppUtil_LoadMediaNotify, pMe);

               nRet = IMEDIA_GetTotalTime(pMe->m_pMedia);
               nRet = IMEDIA_GetState(pMe->m_pMedia, &bStateChange);
               //IMedia_GetMediaParm(IMedia *p,  AEEMediaData * pmd);
#endif
            }
            else if(MG_BETWEEN(eMimeBase, MG_MIME_VIDEOBASE, MG_MIME_VIDEOMAX))
            {
               //TODO:
            }
            else
            {
               bRet = FALSE;
            }
         }
#endif

         ISHELL_PostEvent(pMe->m_pShell,
                          AEECLSID_MEDIAGALLERY,
                          EVT_USER_REDRAW,
                          0,
                          0);

         return TRUE;
      }

   case EVT_DIALOG_END:
      {
         //if we are suspending (EVT_APP_SUSPEND is sent before
         //EVT_DIALOG_END). Handle suspend for current status.
         //TODO : SUSPNED
         if(pMe->m_bSuspending == FALSE)
         {
            // MGAppPopupMenu_OperationDone(pMe, 0);
         }

         return TRUE;
      }

   case EVT_USER_REDRAW:
      {
         MGAppUtil_DrawSoftkey(pMe, BTBAR_BACK);
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }

   case EVT_KEY:
      switch(wParam)
      {
      case AVK_CLR:
         MGAppPopupMenu_OperationDone(pMe, 0);
         MGCLOSE_DIALOG(MGDLGRET_CANCELED);
         return TRUE;

      default:
         break;
      }
      break;

   case EVT_KEY_PRESS:
      break;

   case EVT_COMMAND:
      return TRUE;

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
   case EVT_PEN_UP:
       {
           AEEDeviceInfo devinfo;
           int nBarH ;
           AEERect rc;
           int16 wXPos = (int16)AEE_GET_X(dwParam);
           int16 wYPos = (int16)AEE_GET_Y(dwParam);
           MSG_FATAL("MGAppPopupMenu_OnDetail wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
           nBarH = GetBottomBarHeight(pMe->m_pDisplay);
   
           MEMSET(&devinfo, 0, sizeof(devinfo));
           ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
           SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

           if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
           {
               if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
               {                       
                    boolean rt = MGAppPopupMenu_OnDetail(pMe,EVT_KEY,AVK_CLR,0);
                    return rt;
               }
           }
       }
       break;
#endif //FEATURE_LCD_TOUCH_ENABLE

   default:
      break;
   }

   return FALSE;
}//MGAppPopupMenu_OnDetail



/*
 * ==========================================================================
 * FUNCTION     :  MGAppPopupMenu_OperationDone
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline int MGAppPopupMenu_OperationDone(CMediaGalleryApp *pMe,
                                                 int nType)
{
   if(!pMe)
      return EFAILED;

   if(pMe->m_bSuspending == TRUE)
      return EFAILED;

   if(nType == MG_FNSHOP_DONE)
   {
      MediaGalleryApp_ShowDoneMsgBox(pMe);
   }
   else if(nType == MG_FNSHOP_ERROR)
	{
		MediaGalleryApp_ShowErrorMsgBox(pMe);
	}
   else
   {
      MediaGalleryApp_SetOps(pMe, MG_OPS_DEFAULT, MG_OP_NULL);
   }

   return SUCCESS;
}//MGAppPopupMenu_OperationDone

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_MusicAddDlg_HandleEvent
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_MusicAddDlg_HandleEvent(CMediaGalleryApp* pMe,
                           AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
   MediaDlgStat eDlgStat;

   if(!pMe)
      return FALSE;

   pMenuCtl = pMe->m_pMediaMenu;
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(NULL == pMenuCtl  && eCode != EVT_DIALOG_INIT)
   {
      return FALSE;
   }
#ifdef FEATURE_LCD_TOUCH_ENABLE
   if(eCode == EVT_PEN_UP)
       {
           int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
           int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
           AEEDeviceInfo devinfo;
           int nBarH ;
           AEERect rc;
           MSG_FATAL("CContApp_HandleOneDialNumFldSelDlgEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
            
           nBarH = GetBottomBarHeight(pMe->m_pDisplay);
           MEMSET(&devinfo, 0, sizeof(devinfo));
           ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
           SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
           if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
           {
               if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
               {
                    eCode = EVT_KEY;
                    wParam = AVK_SELECT;
               } 
               else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
               { 
                   eCode = EVT_KEY;
                    wParam = AVK_CLR;
               }
           }
           else if(pMenuCtl != NULL)
           {
            IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam);
           }
       }                               
#endif  
   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
        MSG_FATAL("MGAppUtil_CreateMediaMenu",0,0,0);
         MGAppUtil_CreateMediaMenu(pMe,  NULL);

         return TRUE;
      }

      case EVT_DIALOG_START:
         {
            MGAppUtil_SetMenuCtlRectProp(pMe,
                  MP_UNDERLINE_TITLE | MP_WRAPSCROLL | MP_MULTI_SEL |MP_ALLOW_DUPE_IDS | MP_NO_REDRAW,
                  pMenuCtl);
            IMENUCTL_SetOemProperties(pMenuCtl,
                  OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

            MediaGalleryApp_SetExplorerMime(pMe,MG_MIME_SNDBASE);

            if(pMe->m_bKeepMediaMenu == FALSE)
            {
               pMe->m_nSelNum = 0;
               MGAppUtil_StartMediaMenu(pMe, pMenuCtl, eCode, wParam, dwParam);
            }

            return TRUE;
         }

      case EVT_DIALOG_END:
         MGAppUtil_OnMediaMenuDlgEnd(pMe, pMenuCtl, wParam, dwParam);
         return TRUE;

      case EVT_USER_REDRAW:
         {
            MGAppUtil_UpdateMediaMenuSoftkey(pMe);
            return TRUE;
         }

      case EVT_KEY:
      {
         if(FALSE == MGAppUtil_IsNeedHandleKey(pMe, wParam))
         {
            return TRUE;
         }

         switch(wParam)
         {
            case AVK_CLR:
               return MGAppUtil_OnMediaMenuClrKeyEvt(pMe, pMenuCtl);

            //dele by yangdecai 2010-08-06
            case AVK_SELECT:
               if(eDlgStat == MG_DLGSTAT_NORMAL)
               {
                  /*finish select file */
                  MGAppUtil_UpdateSelItemCheck(pMe);
                  MGAppUtil_ExplorerBuildSelItems(pMe, pMenuCtl);
               }
               return TRUE;
			//modi by yangdecai 2010-08-06  
			//case AVK_RIGHT:
			case AVK_INFO: 
               {
                  if(eDlgStat == MG_DLGSTAT_NORMAL)
                  {
                     if(MG_SELECT_MAX >= pMe->m_nSelNum)
                     {
                        MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                           pMenuCtl,
                                                           eCode,
                                                           wParam,
                                                           dwParam);
                        MGAppUtil_ExplorerGetSelectCount(pMe, pMenuCtl);
                        MGAppUtil_UpdateMediaMenuSoftkey(pMe);
                     }
                     else
                     {
                        IMENUCTL_SetActive(pMenuCtl, FALSE);
                        MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                         IDS_MG_LISTFULL,
                                                         MESSAGE_INFORMATION,
                                                         BTBAR_BACK);
                     }
                  }
                  return TRUE;
               }

            default:
               return MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                         pMenuCtl,
                                                         eCode,
                                                         wParam,
                                                         dwParam);
         }
         break;
      }

      case EVT_COMMAND:
        //pMe->m_nCurPlaylistID = IMENUCTL_GetSel(pMenuCtl);
         if(eDlgStat == MG_DLGSTAT_NORMAL)
         {
              if(MG_SELECT_MAX >= pMe->m_nSelNum)
              {
                 MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                    pMenuCtl,
                                                    eCode,
                                                    wParam,
                                                    dwParam);
                 MGAppUtil_ExplorerGetSelectCount(pMe, pMenuCtl);
                 MGAppUtil_UpdateMediaMenuSoftkey(pMe);
              }
              else
              {
                 IMENUCTL_SetActive(pMenuCtl, FALSE);
                 MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                  IDS_MG_LISTFULL,
                                                  MESSAGE_INFORMATION,
                                                  BTBAR_BACK);
              }
              return TRUE;
         }
         else
         {
            MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe, pMenuCtl,eCode,wParam, dwParam);
         }
           break;
      case EVT_CTL_SEL_CHANGED:
         MGAppUtil_OnMediaMenuSelChange(pMe, eDlgStat);
         return TRUE;

      case EVT_DISPLAYDIALOGTIMEOUT:
         {
            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT",0,0,0);
            if(FALSE == IMENUCTL_IsActive(pMenuCtl))
            {
               IMENUCTL_SetActive(pMenuCtl, TRUE);
            }
            MGAppUtil_UpdateMediaMenuSoftkey(pMe);
         }
         return TRUE;

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_MusicAddDlg_HandleEvent

/*===========================================================================
 * FUNCTION:
 *
 * DESCRIPTION: create and build video file list, and store them in the database
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MediaGalleryApp_VideoAddDlg_HandleEvent(CMediaGalleryApp* pMe,
                                                       AEEEvent eCode,
                                                       uint16 wParam,
                                                       uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
   MediaDlgStat eDlgStat;
   static uint16 swRecID = 0;

   if(!pMe)
      return FALSE;

   MSG_FATAL("VideoAddDlg eCode:%d, wParam:%d, dwParam:%d",
            eCode, wParam, dwParam);

   pMenuCtl = pMe->m_pMediaMenu;
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(NULL == pMenuCtl  && eCode != EVT_DIALOG_INIT)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
         swRecID = 0;
         MSG_FATAL("MGAppUtil_CreateMediaMenu",0,0,0);
         MGAppUtil_CreateMediaMenu(pMe,  NULL);

         return TRUE;

      case EVT_DIALOG_START:
      {
         MGAppUtil_SetMenuCtlRectProp(pMe,
               MP_UNDERLINE_TITLE | MP_WRAPSCROLL |MP_ALLOW_DUPE_IDS | MP_NO_REDRAW,
               pMenuCtl);
         IMENUCTL_SetOemProperties(pMenuCtl,
               OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

         MediaGalleryApp_SetExplorerMime(pMe,MG_MIME_VIDEOBASE);

         if(pMe->m_bKeepMediaMenu == FALSE)
         MGAppUtil_StartMediaMenu(pMe, pMenuCtl, eCode, wParam, dwParam);

         return TRUE;
      }

      case EVT_DIALOG_END:
      MGAppUtil_OnMediaMenuDlgEnd(pMe, pMenuCtl, wParam, dwParam);
      return TRUE;

      case EVT_USER_REDRAW:
      {
	  	 pMe->m_videoLoadDone = TRUE;
         MGAppUtil_UpdateMediaMenuSoftkey(pMe);
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }

      case EVT_KEY:
      {
         if(FALSE == MGAppUtil_IsNeedHandleKey(pMe, wParam))
         {
            return TRUE;
         }

         switch(wParam)
         {
            case AVK_CLR:
               return MGAppUtil_OnMediaMenuClrKeyEvt(pMe, pMenuCtl);

            case AVK_INFO:
               if(eDlgStat == MG_DLGSTAT_NORMAL)
               {
                  IMENUCTL_SetActive(pMenuCtl, FALSE);
                  MGAppUtil_UpdateSelItemCheck(pMe);
                  MediaGalleryApp_ShowProgressBox(pMe,
                                                  MG_MSGID_BUILDVIDEOLIST);
#ifndef AEE_SIMULATOR
                  MGExplorer_InitBuildVideoDatabase(pMe,
                                       MG_VIDEOFILE_DB_NAME,
                                       MGAppUtil_VideoDatabaseBuildComplete);
#else
                  MGAppUtil_VideoDatabaseBuildComplete((void*)pMe);
#endif
                  return TRUE;
               }
               else
               {
                  return MGAppUtil_OnMediaMenuInfoKeyEvt(pMe,
                        pMenuCtl, eCode, wParam, dwParam);
               }
               break;

            case AVK_SELECT:
            {
               if(eDlgStat == MG_DLGSTAT_NORMAL)
               {
                  MenuInsItem OptItems[] =
                  {
                     //{IDS_MG_BUILDPL, TRUE},
                     {IDS_MG_DELETE, TRUE}, {IDS_MG_DELALL, TRUE},
                     {IDS_MG_SORT, TRUE},  {IDS_MG_DETAIL, TRUE}
                  };
                  int retVal = SUCCESS;
                  uint16 nItemNum = sizeof(OptItems)/sizeof(MenuInsItem);
                  boolean bProtect;
                  MGFileInfo *pItemData;
                  uint32 dwProps = MP_UNDERLINE_TITLE |
                     MP_WRAPSCROLL |
                     MP_BIND_ITEM_TO_NUMBER_KEY;

                  MGAppUtil_UpdateSelItemCheck(pMe);
                  pItemData = MediaGalleryApp_GetCurrentNode(pMe);
                  if(NULL == pItemData)
                     return TRUE;

                  bProtect = MG_FILE_PROTECT(pItemData->attrib);
                 // if(bProtect == TRUE)
                  {
                     OptItems[1].bInsert = FALSE;
                     OptItems[2].bInsert = FALSE;
                  }

                  retVal = MGAppUtil_CreatePopupMenu(&pMe->m_pMenuPopup,
                                                     OptItems,
                                                     nItemNum,
                                                     dwProps,
                                                     BTBAR_SELECT_BACK);

                  if(retVal == SUCCESS )
                  {
                     MGAppUtil_SetMediaDlgStat(pMe,MG_DLGSTAT_POPUP);
                     IMENUCTL_SetActive(pMenuCtl, FALSE);
                     IMENUCTL_Redraw(pMe->m_pMenuPopup);
                  }

                  return TRUE;
               }
               else
                  return  MGAppUtil_OnMediaMenuSelectKeyEvt(pMe,
                        pMenuCtl, eCode, wParam, dwParam);
            }
            break;

            default:
               return MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                         pMenuCtl,
                                                         eCode,
                                                         wParam,
                                                         dwParam);
         }
         break;
      }

      case EVT_KEY_PRESS:
      {
         if(eDlgStat != MG_DLGSTAT_NORMAL || !pMe->m_pFilesDB)
         {
            break;
         }

         switch(wParam)
         {
            case AVK_UP:
               {
                  MGFileInfo Info;
                  uint32 dwCount;
                  dwCount = IDATABASE_GetRecordCount(pMe->m_pFilesDB);

                  if(0 == dwCount)
                  {
                     MG_FARF(ADDR, ("No record"));
                     return FALSE;
                  }

                  MG_FARF(ADDR, ("VideoAddDlg receive AVK_UP"));

                  CMediaGallery_GetFileInfoRecord(pMe->m_pFilesDB,
                        swRecID,
                        &Info);
                  /*if dwCount == 0, it will crash, for divide to 0*/
                  swRecID = (uint16)((swRecID + 1) % dwCount);

               }
               return TRUE;

            case AVK_DOWN:
               {
                  MGFileInfo Info;
                  uint32 dwCount;
                  dwCount = IDATABASE_GetRecordCount(pMe->m_pFilesDB);

                  if(0 == dwCount)
                  {
                     MG_FARF(ADDR, ("No record"));
                     return FALSE;
                  }

                  MG_FARF(ADDR, ("VideoAddDlg receive AVK_DOWN"));
                  CMediaGallery_GetFileInfoRecord(pMe->m_pFilesDB,
                        swRecID,
                        &Info);
                  /*if dwCount == 0, it will crash, for divide to 0*/
                  swRecID = (uint16)((swRecID + dwCount - 1) % dwCount);

               }
               return TRUE;

            default:
               break;
         }
      }
      break;

      case EVT_COMMAND:
        /* return MGAppUtil_OnMediaMenuCommandEvt(pMe,
                                 pMenuCtl, wParam, dwParam);*/
           MSG_FATAL("MediaGalleryApp_VideoAddDlg_HandleEvent EVT_COMMAND eDlgStat=%d",eDlgStat,0,0);                      
           if(eDlgStat == MG_DLGSTAT_NORMAL)
           {
              IMENUCTL_SetActive(pMenuCtl, FALSE);
              MGAppUtil_UpdateSelItemCheck(pMe);
              MediaGalleryApp_ShowProgressBox(pMe,
                                              MG_MSGID_BUILDVIDEOLIST);
#ifndef AEE_SIMULATOR
              MGExplorer_InitBuildVideoDatabase(pMe,
                                   MG_VIDEOFILE_DB_NAME,
                                   MGAppUtil_VideoDatabaseBuildComplete);
#else
              MGAppUtil_VideoDatabaseBuildComplete((void*)pMe);
#endif
              return TRUE;
           }
           else
           {
                return MGAppUtil_OnMediaMenuCommandEvt(pMe,
                                 pMenuCtl, wParam, dwParam);
           }
           break;                                 

      case EVT_CTL_SEL_CHANGED:
         MGAppUtil_OnMediaMenuSelChange(pMe, eDlgStat);
         return TRUE;

      case EVT_DISPLAYDIALOGTIMEOUT:
         MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT",0,0,0);
         return MGAppUtil_OnMediaMenuMsgBoxTimeOut(pMe,
                                            pMenuCtl, wParam, dwParam);

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
      case EVT_PEN_UP:
          {
              AEEDeviceInfo devinfo;
              int nBarH ;
              AEERect rc;
              int16 wXPos = (int16)AEE_GET_X(dwParam);
              int16 wYPos = (int16)AEE_GET_Y(dwParam);
              MSG_FATAL("MediaGalleryApp_VideoAddDlg_HandleEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
              nBarH = GetBottomBarHeight(pMe->m_pDisplay);
      
              MEMSET(&devinfo, 0, sizeof(devinfo));
              ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
              SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

              if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
              {
                  if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                  {
                      return  MediaGalleryApp_VideoAddDlg_HandleEvent(pMe,EVT_KEY,AVK_SELECT,0);
                  }
                  else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//中
                  {
                       return MediaGalleryApp_VideoAddDlg_HandleEvent(pMe,EVT_KEY,AVK_INFO,0);
                  }
                  else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                  {                       
                       return  MediaGalleryApp_VideoAddDlg_HandleEvent(pMe,EVT_KEY,AVK_CLR,0);
                  }
              }
              else
              {
                  if(eDlgStat == MG_DLGSTAT_POPUP)
                  {
                       MSG_FATAL("eDlgStat == MG_DLGSTAT_POPUP",0,0,0);  
                      return IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
                  }  
                  else if(pMenuCtl != NULL)
                  {
                      MSG_FATAL("EVT_PEN_UP pMenuCtl != NULL",0,0,0);  
                      if (IMENUCTL_HandleEvent(pMenuCtl, eCode, wParam, dwParam))
                      {
                          MSG_FATAL("MediaGalleryApp_VideoAddDlg_HandleEvent 0",0,0,0); 
                          return TRUE;
                      }
                  }    
              }
          }
          break;
#endif //FEATURE_LCD_TOUCH_ENABLE

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_VideoAddDlg_HandleEvent

static boolean MediaGalleryApp_ImageSettingDlg_HandleEvent(
                                             CMediaGalleryApp* pMe,
                                             AEEEvent eCode,
                                             uint16 wParam,
                                             uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
   MediaDlgStat eDlgStat;

   if(!pMe)
      return FALSE;

   pMenuCtl = pMe->m_pMediaMenu;
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(NULL == pMenuCtl  && eCode != EVT_DIALOG_INIT)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         MSG_FATAL("MGAppUtil_CreateMediaMenu",0,0,0);
         MGAppUtil_CreateMediaMenu(pMe,  NULL);

         return TRUE;
      }

      case EVT_DIALOG_START:
      {
         MGAppUtil_SetMenuCtlRectProp(pMe,
               MP_UNDERLINE_TITLE | MP_WRAPSCROLL |MP_ALLOW_DUPE_IDS | MP_NO_REDRAW,
               pMenuCtl);
         IMENUCTL_SetOemProperties(pMenuCtl,
               OEMMP_DISTINGUISH_INFOKEY_SELECTKEY |OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
         IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif

         MediaGalleryApp_SetExplorerMime(pMe,MG_MIME_IMGBASE);

         if(pMe->m_bKeepMediaMenu == FALSE)
            MGAppUtil_StartMediaMenu(pMe, pMenuCtl, eCode, wParam, dwParam);
         return TRUE;
      }

      case EVT_DIALOG_END:
      {
         MGAppUtil_OnMediaMenuDlgEnd(pMe, pMenuCtl, wParam, dwParam);
         return TRUE;
      }

      case EVT_USER_REDRAW:
      {
         MGAppUtil_UpdateMediaMenuSoftkey(pMe);
         IDISPLAY_Update(pMe->m_pDisplay);
         return TRUE;
      }
#ifdef FEATURE_LCD_TOUCH_ENABLE//WLH ADD FOR LCD TOUCH
      case EVT_USER:
#endif
      case EVT_KEY:
      {
         if(FALSE == MGAppUtil_IsNeedHandleKey(pMe, wParam))
         {
            return TRUE;
         }

         switch(wParam)
         {
         case AVK_CLR:
            return MGAppUtil_OnMediaMenuClrKeyEvt(pMe, pMenuCtl);

         case AVK_INFO:
            if(eDlgStat == MG_DLGSTAT_NORMAL)
            {
               MGAppUtil_UpdateSelItemCheck(pMe);

               MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_VIEWIMG);
               MediaGalleryApp_SetOps(pMe, MG_OPS_IMAGEVIEW, MG_OP_NULL);
               MGCLOSE_DIALOG(MGDLGRET_VIEW);
               return TRUE;
            }
            else
            {
               return MGAppUtil_OnMediaMenuInfoKeyEvt(pMe,
                                                      pMenuCtl,
                                                      eCode,
                                                      wParam,
                                                      dwParam);
            }

            break;

         case AVK_SELECT:
            {
               if(eDlgStat == MG_DLGSTAT_NORMAL)
               {
                  MenuInsItem OptItems[] =
                  {
                     {IDS_MG_VIEW, TRUE}, {IDS_MG_SETWALLPAPER, TRUE},
                     {IDS_MG_SORT, TRUE}, {IDS_MG_DETAIL, TRUE}
                  };
                  int retVal = SUCCESS;
                  uint16 nItemNum = sizeof(OptItems)/sizeof(MenuInsItem);
                  uint32 dwProps = MP_UNDERLINE_TITLE | MP_WRAPSCROLL |
                                    MP_BIND_ITEM_TO_NUMBER_KEY;

                  MGAppUtil_UpdateSelItemCheck(pMe);
                  retVal = MGAppUtil_CreatePopupMenu(&pMe->m_pMenuPopup,
                                                     OptItems,
                                                     nItemNum,
                                                     dwProps,
                                                     BTBAR_SELECT_BACK);

                  if(retVal == SUCCESS )
                  {
                     MGAppUtil_SetMediaDlgStat(pMe,MG_DLGSTAT_POPUP);
                     IMENUCTL_SetActive(pMenuCtl, FALSE);
                     IMENUCTL_Redraw(pMe->m_pMenuPopup);
                  }

                  return TRUE;
               }
               else
               {
                  return  MGAppUtil_OnMediaMenuSelectKeyEvt(pMe,
                                                            pMenuCtl,
                                                            eCode,
                                                            wParam,
                                                            dwParam);
               }
            }
            return TRUE;

         default:
            return MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                      pMenuCtl,
                                                      eCode,
                                                      wParam,
                                                      dwParam);
         }
         break;
      }

      case EVT_COMMAND:
        if(eDlgStat == MG_DLGSTAT_NORMAL)
        {
           MGAppUtil_UpdateSelItemCheck(pMe);

           MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_VIEWIMG);
           MediaGalleryApp_SetOps(pMe, MG_OPS_IMAGEVIEW, MG_OP_NULL);
           MGCLOSE_DIALOG(MGDLGRET_VIEW);
           return TRUE;
        }
        else  
        {
            return MGAppUtil_OnMediaMenuCommandEvt(pMe,
                                                 pMenuCtl,
                                                 wParam,
                                                 dwParam);
        }
        
      case EVT_CTL_SEL_CHANGED:
         MGAppUtil_OnMediaMenuSelChange(pMe, eDlgStat);
         return TRUE;

     case EVT_DISPLAYDIALOGTIMEOUT:
         {
            uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT",0,0,0);

            if (nMsgBoxId == MG_MSGID_DONE)
            {
               MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT 1",0,0,0); 
               MGCLOSE_DIALOG(MGDLGRET_SETTING_SELECT);
               return TRUE;
            }
            else
            {
               return MGAppUtil_OnMediaMenuMsgBoxTimeOut(pMe,
                                                         pMenuCtl,
                                                         wParam,
                                                         dwParam);
            }

         }
         break;

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
  case EVT_PEN_UP:
     {
         AEEDeviceInfo devinfo;
         int nBarH ;
         AEERect rc;
         int16 wXPos = (int16)AEE_GET_X(dwParam);
         int16 wYPos = (int16)AEE_GET_Y(dwParam);
         MSG_FATAL("MGAppPopupMenu_OnImageViewer wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
         nBarH = GetBottomBarHeight(pMe->m_pDisplay);
         MEMSET(&devinfo, 0, sizeof(devinfo));
         ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
         SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

         if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
         {
             if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
             {
                 boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_SELECT,0);
                 return rt;
             }
             else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
             {
                  boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_INFO,0);
                  return rt;
             }
             else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
             {                       
                  boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_MEDIAGALLERY,EVT_USER,AVK_CLR,0);
                  return rt;
             }
         }
        else
        {
            if(eDlgStat == MG_DLGSTAT_POPUP)
            {
                MSG_FATAL("eDlgStat == MG_DLGSTAT_POPUP",0,0,0);  
                return IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
            }  
            else if(pMenuCtl != NULL)
            {
                MSG_FATAL("EVT_PEN_UP pMenuCtl != NULL",0,0,0);  
                if (IMENUCTL_HandleEvent(pMenuCtl, eCode, wParam, dwParam))
                {
                    MSG_FATAL("MediaGalleryApp_ImageSettingDlg_HandleEvent 0",0,0,0); 
                    return TRUE;
                }
            }    
        }         
     }
     break;
#endif //FEATURE_LCD_TOUCH_ENABLE

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_ImageSettingDlg_HandleEvent


/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_SoundSettingDlg_HandleEvent
 * DESCRIPTION  :  call by profile applet to set ringtone for call/sms/alarm etc
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MediaGalleryApp_SoundSettingDlg_HandleEvent(CMediaGalleryApp *pMe,
                                             AEEEvent eCode,
                                             uint16 wParam,
                                             uint32 dwParam)
{
   IMenuCtl* pMenuCtl;
   MediaDlgStat eDlgStat;
   MSG_FATAL("SoundSettingDlg_HandleEvent eCode=0x%x,wParam=0x%x, dwParam=0x%x",eCode,wParam,dwParam );
   if(!pMe)
      return FALSE;

   pMenuCtl = pMe->m_pMediaMenu;
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(NULL == pMenuCtl  && eCode != EVT_DIALOG_INIT)
   {
      return FALSE;
   }

   switch(eCode)
   {
      case EVT_DIALOG_INIT:
      {
         MSG_FATAL("MGAppUtil_CreateMediaMenu",0,0,0);
         MGAppUtil_CreateMediaMenu(pMe,  NULL);

         return TRUE;
      }

      case EVT_DIALOG_START:
         {
            if(pMe->m_bKeepMediaMenu == FALSE)
            {
               MGAppUtil_SetMenuCtlRectProp(pMe,
                                            MP_UNDERLINE_TITLE | MP_WRAPSCROLL |MP_ALLOW_DUPE_IDS | MP_NO_REDRAW,
                                            pMenuCtl);
               IMENUCTL_SetOemProperties(pMenuCtl,
                                         /*OEMMP_DISTINGUISH_INFOKEY_SELECTKEY |*/
                                         OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
               IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MEDIA_BACKGROUND);
#endif
               MediaGallery_SetRingVolumn(pMe);
               MediaGalleryApp_SetExplorerMime(pMe,MG_MIME_SNDBASE);
               MGAppUtil_StartMediaMenu(pMe, pMenuCtl, eCode, wParam, dwParam);
            }
            else
            {
               MGAppUtil_ScheduleRingTonePreview(pMe);
            }
            return TRUE;
         }

      case EVT_DIALOG_END:
         {
            //if we are suspending (EVT_APP_SUSPEND is sent before
            //EVT_DIALOG_END). Handle suspend for current status.
            //if(pMe->m_bSuspending)
            {
               if(FALSE == MGAppUtil_UpdateSelItemCheck(pMe))
               {
                  ISHELL_CancelTimer(pMe->m_pShell,
                                     (PFNNOTIFY)MGAppUtil_PreviewRingtone,
                                     (void *)pMe);

                  MGAppUtil_StopRingTonePreview(pMe, TRUE);
               }
            }

            /*Restore key beep volume*/
            if(pMe->m_nKeyBeepVol != MG_MAX_BYTE)
            {
               IConfig *pConfig = NULL;

               if(SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                                   AEECLSID_CONFIG,
                                                   (void **)&pConfig))
               {
                  ICONFIG_SetItem(pConfig,
                                  CFGI_BEEP_VOL,
                                  &pMe->m_nKeyBeepVol,
                                  sizeof(byte));
               }
               else
               {
                  MG_FARF(ADDR, ("Create config interface failed"));
               }
               pMe->m_nKeyBeepVol = MG_MAX_BYTE;
            }
            /*当Release时，Media在stop时不会通知客户，所以，万全之计先设置为
             * FALSE*/
            MediaGallery_SetPlayMusicStatus(FALSE);
            MGAppUtil_OnMediaMenuDlgEnd(pMe, pMenuCtl, wParam, dwParam);
            return TRUE;
         }

      case EVT_USER_REDRAW:
         {
            MGAppUtil_UpdateMediaMenuSoftkey(pMe);
            IDISPLAY_Update(pMe->m_pDisplay);
            return TRUE;
         }

      case EVT_KEY:
         {
            MSG_FATAL("SoundSettingDlg_HandleEvent EVT_KEY",0,0,0 );
            if(FALSE == MGAppUtil_IsNeedHandleKey(pMe, wParam))
            {
               return TRUE;
            }
            MSG_FATAL("SoundSettingDlg_HandleEvent EVT_KEY 1 wParam=0x%x, dwParam=0x%x",wParam,dwParam,0 );
            switch(wParam)
            {
            case AVK_CLR:
               if((dwParam & KB_AUTOREPEAT) != 0)
               {
                  return FALSE;
               }
               if(eDlgStat == MG_DLGSTAT_NORMAL)
               {
                  if(pMe->m_bCallbackDoing && CALLBACK_IsQueued(&pMe->m_CallBack))
                  {
                     pMe->m_bCallbackDoing = FALSE;
                     CALLBACK_Cancel(&pMe->m_CallBack);
                     MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
                  }
                  else
                  {
                     if(FALSE == MGAppUtil_UpdateSelItemCheck(pMe))
                     {
                        MGAppUtil_StopRingTonePreview(pMe, TRUE);
                     }
                  }

                  MGExplorer_ResetPreItemID(&pMe->m_Explorer, FALSE);
                  IMENUCTL_SetActive(pMenuCtl, FALSE);
                  MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               }
               else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
               {
                  MediaGalleryApp_CancelMsgBoxTimer(pMe, TRUE);
               }

               return TRUE;

            default:
               MSG_FATAL("SoundSettingDlg_HandleEvent EVT_KEY 2",0,0,0 ); 
               return MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                         pMenuCtl,
                                                         eCode,
                                                         wParam,
                                                         dwParam);
            }
         }
         break;

      case EVT_CTL_SEL_CHANGED:
         if(eDlgStat == MG_DLGSTAT_NORMAL &&
               FALSE == pMe->m_bCallbackDoing)
         {
            MGAppUtil_ScheduleRingTonePreview(pMe);
         }
         return TRUE;

      case EVT_COMMAND:
         {
            char *szName = NULL;
            MSG_FATAL("SoundSettingDlg_HandleEvent EVT_COMMAND",0,0,0);
            
            MGAppUtil_UpdateSelItemCheck(pMe);
            szName = MediaGalleryApp_GetCurrentNodeName(pMe);
            DBGPRINTF("szName=%s",szName);
            if(SUCCESS ==
               MediaGallery_DoExplorerAddCB((FileNamesBuf)szName, 1))
            {
                MSG_FATAL("SoundSettingDlg_HandleEvent EVT_COMMAND 1",0,0,0);
               MGCLOSE_DIALOG(MGDLGRET_SETTING_SELECT);
            }
            else
            {
                MSG_FATAL("SoundSettingDlg_HandleEvent EVT_COMMAND 2",0,0,0);
               MGCLOSE_DIALOG(MGDLGRET_SETTING_SELECT);
               //MGCLOSE_DIALOG(MGDLGRET_CANCELED);
               MG_FARF(ADDR, ("do call back failed!"));
            }
         }
         return TRUE;

      case EVT_DISPLAYDIALOGTIMEOUT:
         {
            uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
            MSG_FATAL("EVT_DISPLAYDIALOGTIMEOUT",0,0,0);
            if(nMsgBoxId == MG_MSGID_ENUMMAX)
            {
               MGAppUtil_ScheduleRingTonePreview(pMe);
            }
            MG_FARF(ADDR, ("display dialog time out"));
            return MGAppUtil_OnMediaMenuMsgBoxTimeOut(pMe,
                                                      pMenuCtl,
                                                      wParam,
                                                      dwParam);
         }

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch   
        case EVT_PEN_UP:
            {
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                int16 wXPos = (int16)AEE_GET_X(dwParam);
                int16 wYPos = (int16)AEE_GET_Y(dwParam);
                MSG_FATAL("MediaGalleryApp_VideoAddDlg_HandleEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
  
                if(MEDIAGALLERYAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        return MGAppUtil_OnMediaMenuDefaultKeyEvt(pMe,
                                                                  pMenuCtl,
                                                                  EVT_KEY,
                                                                  AVK_SELECT,
                                                                  0);                        
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                    {                       
                         return  MediaGalleryApp_SoundSettingDlg_HandleEvent(pMe,EVT_KEY,AVK_CLR,0);
                    }
                }
                else
                {
                    return IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam);
                }
            }
            break;
#endif //FEATURE_LCD_TOUCH_ENABLE

      default:
         break;
   }

   return FALSE;
}//MediaGalleryApp_ImageSettingDlg_HandleEvent

/*
 * ==========================================================================
 * FUNCTION     :  MediaGalleryApp_RegisterPlayNext
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MediaGalleryApp_RegisterPlayNext(CMediaGalleryApp *pMe,
                                          boolean bRegister)
{
   int16 *pnOp;

   if(!pMe)
      return FALSE;

   pnOp = &pMe->m_PopupOps;

   if(*pnOp == MG_OP_PLAYVIDEO || *pnOp == MG_OP_PLAYMUSIC)
   {
      if(bRegister)
         MediaGalleryApp_RegisterKeyEvent(pMe);
      else
         MediaGalleryApp_DeRegisterKeyEvent(pMe);

		*pnOp = MG_OP_NULL;
   }
   else
      return FALSE;

   return TRUE;
}//MediaGalleryApp_RegisterPlayNext


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
boolean MediaGalleryApp_PlayNext(CMediaGalleryApp *pMe, uint8 nPlayOn)
{
   boolean bRet = TRUE;
   int16 nOp;
   AEECLSID nPlayerID = 0;
   MG_PFN_ITEMTYPECHECK pfnCheck = NULL;
   const char *pszFileName = NULL;

   if(!pMe)
      return FALSE;

   nOp = pMe->m_PopupOps;

   if(nOp == MG_OP_PLAYVIDEO){
      nPlayerID = AEECLSID_VIDEOPLAYER;
      pfnCheck = MGAppUtil_NextVideoCheck;
   }
   else if(nOp == MG_OP_PLAYMUSIC){
      nPlayerID = AEECLSID_APP_MUSICPLAYER;
      pfnCheck = MGAppUtil_NextMusicCheck;
   }
   else
   {
      MG_FARF(ADDR, ("Do not know which applet to play"));
      return FALSE;
   }

   if(nPlayOn != MG_PLAY_CUR)
   {
   bRet =  MGAppUtil_ChangeMediaMenuItemByType(pMe,
         pMe->m_pMediaMenu,
         (boolean)(nPlayOn == MG_PLAY_NEXT ? TRUE : FALSE),
         pfnCheck);
   }

   pszFileName = MediaGalleryApp_GetCurrentNodeName(pMe);

   MG_FARF(ADDR, ("Receive key when suspend, next file %s",
            pszFileName));

   if(bRet == TRUE)
   {
      ISHELL_SendEvent(pMe->m_pShell,
            nPlayerID,
            EVT_PLAYNEXT,
            0,
            (uint32)pszFileName);
   }

   return bRet;
}//MediaGalleryApp_PlayNext


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_ScheduleRingTonePreview
 * DESCRIPTION  :  设置一个定时器，延迟播放音乐，这样处理主要是为了防止用户快
 * 速按键时，频繁对IMEDIA接口的操作会影响性能（响应速度）。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline int MGAppUtil_ScheduleRingTonePreview(CMediaGalleryApp *pMe)
{
   if(!pMe)
      return EFAILED;

   MGAppUtil_StopRingTonePreview(pMe, FALSE);

   MGExplorer_UpdateMediaMenuTitle(pMe, pMe->m_pMediaMenu);

   /*当pMe->m_nRingVolume == 0 时，说明是静音模式，不需要播放铃声。*/
   if (pMe->m_nRingVolume != 0)
   {
      pMe->m_bUpdateSelItem = TRUE;
      ISHELL_SetTimer(pMe->m_pShell,
                      KB_AUTOREPEAT_RATE + 100,
                      (PFNNOTIFY)MGAppUtil_PreviewRingtone,
                      (void *)pMe);
   }
   return SUCCESS;
}//MGAppUtil_ScheduleRingTonePreview


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_StopRingTonePreview
 * DESCRIPTION  :  停止播放音乐/铃声。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline int MGAppUtil_StopRingTonePreview(CMediaGalleryApp *pMe,
                                                  boolean bRelease)
{
   if(!pMe)
      return EFAILED;

   MG_FARF(ADDR, ("StopRingTonePreview, bRelease:%d",bRelease));

   if(NULL != pMe->m_pMedia)
   {
      if(TRUE == MediaGallery_GetPlayMusicStatus())
      {
         MG_FARF(ADDR, ("Stop"));
         IMEDIA_Stop(pMe->m_pMedia);
      }

      if(TRUE == bRelease)
      {
         IMEDIA_RegisterNotify(pMe->m_pMedia, NULL, NULL);
         RELEASEIF(pMe->m_pMedia);
      }
   }

   return SUCCESS;
}


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_PreviewRingtone
 * DESCRIPTION  :  播放音乐/铃声。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static void MGAppUtil_PreviewRingtone(void *po)
{
   AEEMediaData md;
   int nRet;
   MGFileInfo *pCurNode;
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   if(!pMe)
      return;

   MGAppUtil_GetMediaMenuSelectItem((void *)pMe);

   pCurNode = MediaGalleryApp_GetCurrentNode(pMe);

   if(!pCurNode)
      return;

   md.clsData = MMD_FILE_NAME;
   md.pData = pCurNode->szName;
   md.dwSize = 0;

   if(pMe->m_pMedia)
   {
      if(TRUE == MediaGallery_GetPlayMusicStatus())
      {
         IMEDIA_Stop(pMe->m_pMedia);
      }
      IMEDIA_RegisterNotify(pMe->m_pMedia, NULL, NULL);
      RELEASEIF(pMe->m_pMedia);
   }

   nRet = AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);

   if(nRet != SUCCESS)
   {
      MG_FARF(ADDR, ("Create Media Failed!"));
      return;
   }

   nRet = IMEDIA_RegisterNotify(pMe->m_pMedia,
                                MGAppUtil_LoadMediaNotify,
                                pMe);

   if(nRet)
   {
      RELEASEIF(pMe->m_pMedia);
      return;
   }

   IMEDIA_SetVolume(pMe->m_pMedia, pMe->m_nRingVolume);
   IMEDIA_Play(pMe->m_pMedia);

   return;
}//MGAppUtil_PreviewRingtone


/*===========================================================================
 * FUNCTION:MGAppUtil_BuildPopupMenuItems
 *
 * DESCRIPTION: build pop up menu according to file type & file attribute in
 * order to select various operation
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppUtil_BuildPopupMenuItems(CMediaGalleryApp* pMe,
                                          IMenuCtl* pMenuCtl)
{
   int retVal = SUCCESS;
   MGFileInfo *pItemData;
   boolean    bProtect;
   MediaDlgStat eDlgStat;

   if(!pMe || !pMenuCtl)
      return EFAILED;

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(eDlgStat != MG_DLGSTAT_NORMAL)
   {
      return EFAILED;
   }

   MGAppUtil_UpdateSelItemCheck(pMe);
   pItemData = MediaGalleryApp_GetCurrentNode(pMe);

   if(!pItemData ||
         TRUE == pMe->m_bCallbackDoing ||
         TRUE == pMe->m_bMediaMenuEmpty
     )
   {
      MG_FARF(ADDR, ("error: not permit to popup menu!"));
      return EFAILED;
   }

   bProtect = MG_FILE_PROTECT(pItemData->attrib);

   if(pItemData->attrib == AEE_FA_DIR)
   {
      MenuInsItem DirPopupItems[]={
         {IDS_MG_OPEN, TRUE}, /*{IDS_MG_RENAME, TRUE}, */
         {IDS_MG_DELETE, TRUE},{IDS_MG_DELALL, TRUE},
         {IDS_MG_SORT, TRUE}, {IDS_MG_DETAIL, TRUE}};
      boolean  bSysDir = FALSE;

      bSysDir = MGExplorer_IsSysDirInMassStorage(pItemData->szName,
            pMe->m_StoreMedium);

      if(bProtect == TRUE || bSysDir == TRUE)
      {
         //DirPopupItems[1].bInsert = FALSE;/*no RENAME*/
         DirPopupItems[1].bInsert = FALSE;/*no delete*/
         DirPopupItems[2].bInsert = FALSE;/*no delete all*/
      }

      retVal = MGAppUtil_CreatePopupMenu(&pMe->m_pMenuPopup,
            DirPopupItems,
            sizeof(DirPopupItems)/sizeof(MenuInsItem),
            MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY,
            BTBAR_SELECT_BACK);
   }
   else
   {
      MGMimeType     eMimeBase;
      IMenuCtl	  **ppPopupMenu = &pMe->m_pMenuPopup;

      retVal = MGAppUtil_CreatePopupMenu(ppPopupMenu,
            0,
            0,
            MP_UNDERLINE_TITLE|MP_WRAPSCROLL |MP_BIND_ITEM_TO_NUMBER_KEY,
            BTBAR_SELECT_BACK);

#ifdef FEATURE_MG_WAPMMS_SUPPORT
      if(MGSM_FILE_SELECT == pMe->m_StartMode)
      {
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_SELECT);
      }
#endif

      eMimeBase = MediaGalleryApp_GetExplorerMime(pMe);

      if(eMimeBase == MG_MIME_MISCELL ||
            !MG_IN(eMimeBase,MG_MIME_MISMAX,MG_MIME_MAX))
      {
         int        nRet;
         uint16     uMimeBase;

         uMimeBase = MGMediaInfo_MimeBaseConvert(eMimeBase);
         /*if eMimeBase is MG_MIME_UNKNOWN etc, we need detect each
          * type*/
         nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
               pMe->m_pFileMgr,
               pItemData->szName,
               uMimeBase,
               &eMimeBase);

         if(SUCCESS != nRet) {
            MG_FARF(ADDR, ("UNKNOW FILE TYPE"));
            eMimeBase = MG_MIME_UNKNOWN;
         }
      }

      if(MG_BETWEEN(eMimeBase, MG_MIME_IMGBASE, MG_MIME_IMGMAX))
      {
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_VIEW);
         /*Check whether can slide*/
         //if(TRUE  == MGAppUtil_ImageSlideCheck(pMe, pItemData))
         //MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_SLIDE);
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_SETWALLPAPER);
      }
      else if(MG_BETWEEN(eMimeBase, MG_MIME_SNDBASE, MG_MIME_SNDMAX) ||
            (eMimeBase == MG_MIME_VOICEREC) || (eMimeBase == MG_MIME_CALLREC))
      {
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_PLAY);
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_SETAS);
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MSG_SAVEPLAYLIST);
      }
      else if(MG_BETWEEN(eMimeBase, MG_MIME_VIDEOBASE, MG_MIME_VIDEOMAX))
      {
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_PLAY);
      }

      if(bProtect == FALSE)
      {
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_RENAME);
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_DELETE);
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_DELALL);
//#ifdef FEATURE_SUPPORT_BT_APP
#ifdef FEATURE_BT_SEND_FILE_ONLY		//Modify by zzg 2010_11_03
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_BLUETOOTH);
#endif

#ifdef FEATRUE_MG_COPYMOVE
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_COPY);
         MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_MOVE);
#endif
      }
  
      MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_SORT);
      MGMENU_ADDITEM(*ppPopupMenu, IDS_MG_DETAIL);
	  
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      IMENUCTL_SetPopMenuRect(*ppPopupMenu);
      IMENUCTL_SetBottomBarType(*ppPopupMenu, BTBAR_SELECT_BACK);
#endif
      IMENUCTL_SetActive(*ppPopupMenu, TRUE);
   }

   pMe->m_PopupOps = MG_OP_NULL;

   if(retVal == SUCCESS )
   {
#if defined(FEATURE_DISP_160X128)          
        {
            AEERect rc = {0};
            IMENUCTL_SetPopMenuRect(pMe->m_pMenuPopup); 
            IMENUCTL_GetRect(pMe->m_pMenuPopup, &rc);
            if(rc.y < 0)
            {
                int temp = -(rc.y);
                rc.y += temp;
                rc.dy -= temp;
                IMENUCTL_SetRect(pMe->m_pMenuPopup, &rc);
            }
        }
#elif defined(FEATURE_DISP_220X176)          
        {
            AEERect rc = {0};
            IMENUCTL_SetPopMenuRect(pMe->m_pMenuPopup); 
            IMENUCTL_GetRect(pMe->m_pMenuPopup, &rc);
            if(rc.y < 0)
            {
                int temp = -(rc.y);
                rc.y += temp;
                rc.dy -= temp;
                IMENUCTL_SetRect(pMe->m_pMenuPopup, &rc);
            }
        }
#elif defined(FEATURE_DISP_128X160)          
        {
            AEERect rc = {0};
            IMENUCTL_SetPopMenuRect(pMe->m_pMenuPopup); 
            IMENUCTL_GetRect(pMe->m_pMenuPopup, &rc);
            if(rc.y < 0)
            {
                int temp = -(rc.y);
                rc.y += temp;
                rc.dy -= temp;
                IMENUCTL_SetRect(pMe->m_pMenuPopup, &rc);
            }
        }
#elif defined(FEATURE_DISP_176X220)          
        {
            AEERect rc = {0};
            IMENUCTL_SetPopMenuRect(pMe->m_pMenuPopup); 
            IMENUCTL_GetRect(pMe->m_pMenuPopup, &rc);
            if(rc.y < 0)
            {
                int temp = -(rc.y);
                rc.y += temp;
                rc.dy -= temp;
                IMENUCTL_SetRect(pMe->m_pMenuPopup, &rc);
            }
        }
#elif defined(FEATURE_DISP_240X320)          
        {
            AEERect rc = {0};
            IMENUCTL_SetPopMenuRect(pMe->m_pMenuPopup); 
            IMENUCTL_GetRect(pMe->m_pMenuPopup, &rc);
            if(rc.y < 0)
            {
                int temp = -(rc.y);
                rc.y += temp;
                rc.dy -= temp;
                IMENUCTL_SetRect(pMe->m_pMenuPopup, &rc);
            }
        }
#elif defined(FEATURE_DISP_320X240)          
        {
            AEERect rc = {0};
            IMENUCTL_SetPopMenuRect(pMe->m_pMenuPopup); 
            IMENUCTL_GetRect(pMe->m_pMenuPopup, &rc);
            if(rc.y < 0)
            {
                int temp = -(rc.y);
                rc.y += temp;
                rc.dy -= temp;
                IMENUCTL_SetRect(pMe->m_pMenuPopup, &rc);
            }
        }


#endif   
      MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_POPUP);
      IMENUCTL_SetActive(pMenuCtl, FALSE);
      IMENUCTL_Redraw(pMe->m_pMenuPopup);
   }

   return SUCCESS;
}//MGAppUtil_BuildPopupMenuItems


/*===========================================================================
 * FUNCTION:MGAppUtil_CreatePopupMenu
 * DESCRIPTION: create a popup menu
 * PARAMETER: ppIMenu - a pointor point to an IMenuCtl pointor, will store the
 *                      create IMenuCtl poitor.
 *             pItemList - a item list that will insert into the menu
 *             nItemNum - item number in the list
 *             dwProps - menuctl property
 *             bottomBar_e_Type - bottom bar type
 * RETURN VALUE:
 *    SUCCESS: popup menu create
 * ==========================================================================
 */
static int MGAppUtil_CreatePopupMenu(IMenuCtl** ppIMenu,
                                       MenuInsItem* pItemList,
                                       uint16 nItemNum,
                                       uint32 dwProps,
                                       int bottomBar_e_Type)
{
   IMenuCtl*   pPopupMenu = NULL;
   int         retVal;
   uint16      nCount;

   MSG_FATAL("MGAppUtil_CreatePopupMenu Start",0,0,0);
   if(NULL == *ppIMenu)
   {
      retVal=ISHELL_CreateInstance(AEE_GetShell(),
                                   AEECLSID_MENUCTL,
                                   (void**)&pPopupMenu);

      if( SUCCESS != retVal)
      {
         return retVal;
      }
      *ppIMenu = pPopupMenu;
   }
   else
   {
      pPopupMenu = *ppIMenu;
      IMENUCTL_DeleteAll(pPopupMenu);
   }

   /*if  pItemList is not null*/
   if(NULL != pItemList)
   {
      for(nCount = 0; nCount < nItemNum; nCount++)
      {
         if(TRUE == pItemList[nCount].bInsert)
         {
            MGMENU_ADDITEM(pPopupMenu, pItemList[nCount].nItemID);
         }
      }
   }

   IMENUCTL_SetProperties(pPopupMenu, dwProps);

   if(nItemNum)
   {
#if defined( FEATURE_CUSTOMIZED_MENU_STYLE)
      IMENUCTL_SetPopMenuRect(pPopupMenu);
      IMENUCTL_SetBottomBarType(pPopupMenu, bottomBar_e_Type);
#endif

      IMENUCTL_SetActive(pPopupMenu, TRUE);
   }
   MSG_FATAL("MGAppUtil_CreatePopupMenu End",0,0,0);
   return SUCCESS;
}

/*===========================================================================
 * FUNCTION:MGAppUtil_DrawSoftkey
 *
 * DESCRIPTION:draw soft key bar at bottom
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline void MGAppUtil_DrawSoftkey(CMediaGalleryApp* pMe,
                                       BottomBar_e_Type eSoftkeyType)
{
   if(pMe)
   {
      BottomBar_Param_type BarParam={0};
      BarParam.eBBarType = eSoftkeyType;
      DrawBottomBar(pMe->m_pDisplay, &BarParam);
      IDISPLAY_Update(pMe->m_pDisplay);
   }
}//MGAppUtil_DrawSoftkey

/*===========================================================================
 * FUNCTION:MGAppUtil_DrawSoftkeyOverImage
 *
 * DESCRIPTION: draw text at softkey poisition when view photo/image
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static void MGAppUtil_DrawSoftkeyOverImage(CMediaGalleryApp* pMe,
                                          const char *pszResFile,
                                          uint16 nLeftTxtID,
                                          uint16 nMidTxtID,
                                          uint16 nRightTxtID)
{
   AEERect rc;
   AEERect OldRc;
   int  nAscent;
   int  nDescent;
   AECHAR  wszText[20];
   AEEDeviceInfo DevInfo;

   if(!pMe)
   {
      MG_FARF(ADDR, ("DrawSoftkeyOverImage bad parameter!"));
      return;
   }

   ISHELL_GetDeviceInfo(pMe->m_pShell, &DevInfo);

   IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                           AEE_FONT_BOLD,
                           &nAscent,
                           &nDescent);

   rc = pMe->m_rc;
   rc.dy = nAscent + nDescent;
   rc.y =  DevInfo.cyScreen - rc.dy;

   // Display the string of soft key
   if(0 != nLeftTxtID)
   {
      ISHELL_LoadResString(pMe->m_pShell,
                           pszResFile,
                           nLeftTxtID,
                           wszText,
                           sizeof(wszText));

      DrawTextWithProfile(pMe->m_pShell,
                          pMe->m_pDisplay,
                          RGB_WHITE,//_NO_TRANS,
                          AEE_FONT_BOLD,
                          wszText, -1,
                          0, 0, &rc,
                          IDF_ALIGN_MIDDLE | IDF_ALIGN_LEFT | IDF_TEXT_TRANSPARENT);
   }

   // Display the string of soft key
   if(0 != nMidTxtID)
   {
      ISHELL_LoadResString(pMe->m_pShell,
                           pszResFile,
                           nMidTxtID,
                           wszText,
                           sizeof(wszText));

      DrawTextWithProfile(pMe->m_pShell,
                          pMe->m_pDisplay,
                          RGB_WHITE,//_NO_TRANS,
                          AEE_FONT_BOLD,
                          wszText, -1,
                          0, 0, &rc,
                          IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
   }

   // Display the string of soft key
   if(0 != nRightTxtID)
   {
      ISHELL_LoadResString(pMe->m_pShell,
                           pszResFile,
                           nRightTxtID,
                           wszText,
                           sizeof(wszText));

      DrawTextWithProfile(pMe->m_pShell,
                          pMe->m_pDisplay,
                          RGB_WHITE,//_NO_TRANS,
                          AEE_FONT_BOLD,
                          wszText, -1,
                          0, 0, &rc,
                          IDF_ALIGN_MIDDLE | IDF_ALIGN_RIGHT | IDF_TEXT_TRANSPARENT);
   }
}//MGAppUtil_DrawSoftkeyOverImage

/*===========================================================================
 * FUNCTION: MGAppUtil_UpdateMediaMenuSoftkey
 *
 * DESCRIPTION: draw meida menu softkey according media menu condition
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppUtil_UpdateMediaMenuSoftkey(CMediaGalleryApp* pMe)
{
   MGFileInfo *pItemData;
   int nSoftkeyType = BTBAR_NONE;
   MediaDlgStat eDlgStat;
   MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey Start",0,0,0);
   if(!pMe || !pMe->m_pMediaMenu)
      return FALSE;
   
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(eDlgStat != MG_DLGSTAT_NORMAL)
   {
      return EFAILED;
   }

   pItemData = MediaGalleryApp_GetCurrentNode(pMe);

   if(NULL == pItemData ||
      TRUE == pMe->m_bCallbackDoing ||
      TRUE == pMe->m_bMediaMenuEmpty)
   {
      MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey 1",0,0,0);
      nSoftkeyType = BTBAR_BACK;
   }
   else
   {
      MSG_FATAL("pMe->m_Explorer.m_nCurDepth=%d m_StartMode=%d",pMe->m_Explorer.m_nCurDepth, pMe->m_StartMode,0);
      switch(pMe->m_StartMode)
      {
      case MGSM_MUSIC_ADD:
         if(0 == pMe->m_nSelNum)
            nSoftkeyType = BTBAR_BACK;
         else
            nSoftkeyType = BTBAR_SAVE_BACK;
         break;

      case MGSM_MUSIC_SETTING:
         //case MGSM_IMAGE_SETTING:
         nSoftkeyType = BTBAR_SELECT_BACK;
         break;

      default:
         if(pMe->m_Explorer.m_nCurDepth == MG_CURDEPTH_INIT)
         {
         	#if defined(FEATURE_VERSION_S1000T)
			if(pMe->m_videoLoadDone)
			{
                MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey 2",0,0,0);
				nSoftkeyType = BTBAR_0PTION_PLAY_BACK;
				pMe->m_videoLoadDone = FALSE;
			}
			else
			{
                MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey 3",0,0,0);
				nSoftkeyType = BTBAR_OPTION_BACK;
			}
			#else
            MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey 4",0,0,0);
            nSoftkeyType = BTBAR_OPTION_BACK;
			#endif
#ifdef FEATURE_USES_MMS
            if(pMe->m_isForMMS && (pMe->m_StartMode == MGSM_VIDEO_BROWSE))
            {
                nSoftkeyType =  BTBAR_SELECT_BACK;
            }
            else
            {
                nSoftkeyType =  BTBAR_OPTION_UP;
            }
#endif            
         }
         else
         {
            MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey BTBAR_OPTION_UP",0,0,0);
            nSoftkeyType =  BTBAR_OPTION_UP;
         }

         break;
      }
   }
   MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey 3",0,0,0); 
   IMENUCTL_SetBottomBarType(pMe->m_pMediaMenu, nSoftkeyType);
   IMENUCTL_Redraw(pMe->m_pMediaMenu);
   MSG_FATAL("MGAppUtil_UpdateMediaMenuSoftkey End",0,0,0); 
   return TRUE;
}//MGAppUtil_UpdateMediaMenuSoftkey


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_UpdateImgViewerSoftkey
 * DESCRIPTION  :  查看图片时绘制软件行。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_UpdateImgViewerSoftkey(CMediaGalleryApp* pMe)
{
   MediaDlgStat eDlgStat;
   uint16 nLeftResID = 0;

   if(NULL == pMe)
      return FALSE;

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
#ifdef FEATURE_USES_MMS
    if(pMe->m_isForMMS)
    {
      MGAppUtil_DrawSoftkeyOverImage(pMe,
                                     MGRES_LANGFILE,
                                     0,
                                     IDS_MG_SELECT,
                                     0);

      MGAppUtil_DrawSoftkeyOverImage(pMe,
                                     APPSCOMMON_RES_LANG_FILE,
                                     0,
                                     0,
                                     IDS_BACK);        
    }
    else
#endif
    {
       if(eDlgStat == MG_DLGSTAT_IMGZOOM)
       {
          MGAppUtil_DrawSoftkeyOverImage(pMe,
                                         MGRES_LANGFILE,
                                         0,
                                         IDS_MG_ZOOM,
                                         0);

          MGAppUtil_DrawSoftkeyOverImage(pMe,
                                         APPSCOMMON_RES_LANG_FILE,
                                         0,
                                         0,
                                         IDS_BACK);
       }
       else
       {
          if( eDlgStat == MG_DLGSTAT_NORMAL &&
             (pMe->m_bImgLoadDone == TRUE))
          {
             nLeftResID = IDS_OPTION;
          }

          MGAppUtil_DrawSoftkeyOverImage(pMe,
                                         APPSCOMMON_RES_LANG_FILE,
                                         nLeftResID,
                                         0,
                                         IDS_BACK);
       }
    }
   IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

   return TRUE;
}//MGAppUtil_UpdateImgViewerSoftkey

/*===========================================================================
 * FUNCTION: MGAppUtil_UpdateTxtEditSoftkey
 *
 * DESCRIPTION: draw softkey when input file name
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppUtil_UpdateTxtEditSoftkey(CMediaGalleryApp *pMe,
                                                ITextCtl *pEditText)
{
   int nLen = 0;
   AECHAR* pwstrText;
   MediaDlgStat eDlgStat;

   if(!pMe || !pEditText)
   {
      MG_FARF(ADDR, ("Bad parameter!"));
      return FALSE;
   }

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   if(eDlgStat != MG_DLGSTAT_NORMAL)
   {
      return EFAILED;
   }

   pwstrText = ITEXTCTL_GetTextPtr(pEditText);

   if (NULL != pwstrText)
   {
      nLen = WSTRLEN(pwstrText);
   }

   if(nLen == 0)
   {
      MGAppUtil_DrawSoftkey(pMe, BTBAR_BACK);
   }
   #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
   else if (ITEXTCTL_GetCursorPos(pEditText) != TC_CURSORSTART)
   {
      MGAppUtil_DrawSoftkey(pMe, BTBAR_SAVE_DELETE);
   }
   #endif
   else
   {
      MGAppUtil_DrawSoftkey(pMe, BTBAR_SAVE_BACK);
   }

   return TRUE;
}//MGAppUtil_UpdateTxtEditSoftkey

/*===========================================================================
 * FUNCTION:      MGAppUtil_DrawTitleArrow
 *
 * DESCRIPTION:   在标题栏部位绘制箭头。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline int MGAppUtil_DrawTitleArrow(CMediaGalleryApp* pMe,
                                             AEERect *prc)
{
   AEEImageInfo  ImageInfo;
   IImage *pImage = NULL;
   int x;
   int dy;
   int nStart = 0;
   int16 nIndex;
   int16 ImageID[]= {
      IDB_LEFTARROW,
      IDB_RIGHTARROW
   };
   MSG_FATAL("MGAppUtil_DrawTitleArrow Start",0,0,0);
   dy = GetBottomBarHeight(pMe->m_pDisplay);

   for( nIndex = 0; nIndex < ARRAYSIZE(ImageID); nIndex++)
   {
      pImage  = ISHELL_LoadResImage(pMe->m_pShell,
            AEE_APPSCOMMONRES_IMAGESFILE,
            ImageID[nIndex]);

      if(pImage != NULL)
      {
         IIMAGE_GetInfo(pImage, &ImageInfo);

         if(ImageID[nIndex] == IDB_LEFTARROW)
         {
            x = pMe->m_rc.x + 1;
            nStart = x + ImageInfo.cx;
         }
         else
            x = pMe->m_rc.x + pMe->m_rc.dx - ImageInfo.cx - 1;

         IIMAGE_Draw(pImage, x, (dy - ImageInfo.cy) / 2);
         IIMAGE_Release(pImage);
      }
   }
   MSG_FATAL("MGAppUtil_DrawTitleArrow End",0,0,0);
   return nStart;
}//MGAppUtil_DrawTitleArrow


/*set the rectangle and properties of the menu control in dialog
 */
/*===========================================================================
 * FUNCTION:    MGAppUtil_SetMenuCtlRectProp
 *
 * DESCRIPTION: 设置菜单控件的属性和大小。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline void MGAppUtil_SetMenuCtlRectProp(CMediaGalleryApp* pMe,
                                             uint32 dwProps,
                                             IMenuCtl* pMenuCtl)
{
   if(pMe && pMenuCtl)
   {
      AEERect rc;
      AEEDeviceInfo DevInfo;

      ISHELL_GetDeviceInfo(pMe->m_pShell, &DevInfo);
      rc = pMe->m_rc;
      rc.dy = DevInfo.cyScreen ;//- GetBottomBarHeight(pMe->m_pDisplay);

      IMENUCTL_SetRect(pMenuCtl, &rc);
      IMENUCTL_SetProperties(pMenuCtl, dwProps);
   }
}//MGAppUtil_SetMenuCtlRectProp

/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_CreateMediaMenu
 * DESCRIPTION  :  we can use menu control only , don't put it to the dialog.
 * 当Menu控件没创建时，创建控件；当控件有项，但不要保留时，删除里面的全部项；
 * 但保留控件时，我们要刷新控件，使之显示出来。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGAppUtil_CreateMediaMenu(CMediaGalleryApp *pMe,
                                                  IMenuCtl **ppMenuCtl)
{
   IMenuCtl *pMenuCtl;
   AEECLSID cls = AEECLSID_MENUCTL;
   uint8 nState;

   if(!pMe )
      return FALSE;

   if(pMe->m_Explorer.m_eView == MG_VIEW_GRID )
      cls = AEECLSID_ICONVIEWCTL;

   nState = MediaGalleryApp_GetCallbackState(pMe, pMe->m_nCallbackDoFor);

   if(nState == MG_CBS_DOING)
   {
      MSG_FATAL("call back still doing",0,0,0);
      return FALSE;
   }

   pMenuCtl = pMe->m_pMediaMenu;

   /*
    * If media gallery change view type, ex. from list to grid, or from grid to
    * list, it should first free and release when do view type change,
    * MGAppUtil_CreateMediaMenu do not do this.
    * Another note: specify dialog rectangle in resource to (0, 0, 0, 0),
    * other than (-1, -1, -1, -1).
    * */
#if 0
   if(NULL =!= pMenuCtl &&
      cls != IMENUCTL_GetMenuClassID(pMenuCtl))
   {
      pMenuCtl = NULL;
      RELEASEIF(pMe->m_pMediaMenu);
   }
#endif

   if(NULL == pMenuCtl)
   {
      if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                          cls,
                                          (void **)&pMenuCtl))
      {
         MSG_FATAL("Create media menu failed!",0,0,0);
         return FALSE;
      }

      pMe->m_bKeepMediaMenu = FALSE;
      pMe->m_pMediaMenu = pMenuCtl;
      if(ppMenuCtl != NULL)
         *ppMenuCtl = pMenuCtl;
   }
   else if(pMe->m_bKeepMediaMenu == FALSE)
   {
      MSG_FATAL("MGAppUtil_CreateMediaMenu 1",0,0,0);
      MGExplorer_FreeMediaMenuItem(pMenuCtl);
   }
   else
   {
      nState = MediaGalleryApp_GetCallbackState(pMe, MG_CBT_BUILDMENU);
      MSG_FATAL("callback state 0x%x", nState,0,0);

      switch(nState)
      {
      case MG_CBS_STOP:
         IMENUCTL_Redraw(pMenuCtl);
         ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);
         break;

      case MG_CBS_DONE:
      case MG_CBS_CANDOFOR:
         if(FALSE == pMe->m_bMediaMenuEmpty )
         {
            MSG_FATAL("FALSE == pMe->m_bMediaMenuEmpty",0,0,0);
            if( FALSE ==IMENUCTL_IsActive(pMenuCtl))
            {
               IMENUCTL_SetActive(pMenuCtl ,TRUE);
            }
            else
            {
               MSG_FATAL("ERROR: Media menu active!",0,0,0);
            }
            IMENUCTL_Redraw(pMenuCtl);

         }
         else
         {
            MSG_FATAL("FALSE != pMe->m_bMediaMenuEmpty",0,0,0);
            MGAppUtil_UpdateMediaMenuSoftkey(pMe);
            MGAppUtil_OnMediaMenuEmpty(pMe,
                                       /*MG_ONMENUEMPTY_MESSAGEBOX*/
                                       MG_ONMENUEMPTY_STAYEMPTY);
         }
         break;

      case MG_CBS_DOING:
         MSG_FATAL("Error, invalid status",0,0,0);
         break;

      default:
         break;
      }
   }

   pMe->m_bUpdateSelItem = FALSE;
   MSG_FATAL("MGAppUtil_CreateMediaMenu MGAppUtil_SetMediaDlgStat",0,0,0);
   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

   if(pMe->m_bKeepMediaMenu == FALSE)
   {
      MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_NULL);
      MediaGalleryApp_SetOps(pMe, MG_OPS_IMAGEVIEW, MG_OP_NULL);
   }
   return TRUE;

}//MGAppUtil_CreateMediaMenu


/*===========================================================================
 * FUNCTION:      MGAppUtil_StartMediaMenu
 *
 * DESCRIPTION:   开始枚举文件或者文件/文件夹，建立文件列表。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
 static __inline boolean MGAppUtil_StartMediaMenu(CMediaGalleryApp* pMe,
                                                      IMenuCtl* pMenuCtl,
                                                      AEEEvent eCode,
                                                      uint16 wParam,
                                                      uint32 dwParam)
{
   boolean bRet = FALSE;

   if(!pMe || !pMenuCtl)
   {
      MG_FARF(ADDR, ("Bad parameter MGAppUtil_StartMediaMenu"));
      return FALSE;
   }

   MG_FARF(ADDR, ("MGAppUtil_StartMediaMenu start!!!111111111"));
   if(pMe->m_StartMode == MGSM_NORMAL_EXPLORER ||
      pMe->m_StartMode == MGSM_FILE_SELECT)
   {
      bRet = MGExplorer_InitBuildMediaMenu(pMe,
                        MGAppUtil_MediaMenuBuildComplete);
   }
   else
   {
      MGExplorer_ResetPreItemID(&pMe->m_Explorer, TRUE);
      MediaGalleryApp_SetCurrentDepth(pMe, MG_CURDEPTH_INIT);
      bRet = MGExplorer_InitBuildFileList(pMe,
                        MGAppUtil_FileListBuildComplete);
   }


   return bRet;
}//MGAppUtil_StartMediaMenu

/*===========================================================================
 * FUNCTION:      MGAppUtil_MediaMenuBuildComplete
 *
 * DESCRIPTION:   枚举完文件/文件夹后的回调函数。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
void MGAppUtil_MediaMenuBuildComplete(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;

   if(!pMe || !pMe->m_pMediaMenu)
   {
      MG_FARF(ADDR, ("MediaMenuBuildComplete bad parameter!"));
      return;
   }

   pMe->m_bKeepMediaMenu = TRUE;

   if(pMe->m_bMediaMenuEmpty)
   {
      //Media menu is empty
      MGAppUtil_OnMediaMenuEmpty(pMe,
            /*MG_ONMENUEMPTY_MESSAGEBOX*/
            MG_ONMENUEMPTY_STAYEMPTY);
      return;
   }

   MGAppUtil_UpdateMediaMenuSoftkey(pMe);

   if(pMe->m_nEnumeResult == MG_ENUM_OVERMAX)
   {
      IMENUCTL_SetActive(pMe->m_pMediaMenu, FALSE);
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_MANYFILES,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
   }
}//MGAppUtil_MediaMenuBuildComplete

/*===========================================================================
 * FUNCTION:      MGAppUtil_FileListBuildComplete
 *
 * DESCRIPTION:   枚举完文件后的回调函数。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
void MGAppUtil_FileListBuildComplete(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;

   if(!pMe || !pMe->m_pMediaMenu)
   {
      MG_FARF(ADDR, ("MediaMenuBuildComplete bad parameter!"));
      return;
   }

   pMe->m_bKeepMediaMenu = TRUE;

   if(pMe->m_bMediaMenuEmpty)
   {
      MGAppUtil_OnMediaMenuEmpty(pMe,
                                 /*MG_ONMENUEMPTY_MESSAGEBOX*/
                                 MG_ONMENUEMPTY_STAYEMPTY);
   }
   else
   {
      if(pMe->m_nEnumeResult == MG_ENUM_OVERMAX)
      {
         IMENUCTL_SetActive(pMe->m_pMediaMenu, FALSE);
         MediaGalleryApp_SetMsgBoxID(pMe, MG_MSGID_ENUMMAX);
         MediaGalleryApp_ShowPromptMsgBox(pMe,
                                          IDS_MG_MANYFILES,
                                          MESSAGE_INFORMATION,
                                          BTBAR_BACK);
      }
      else
      {
         if(pMe->m_StartMode == MGSM_MUSIC_SETTING)
         {
            MGAppUtil_ScheduleRingTonePreview(pMe);
         }

         ISHELL_PostEvent(pMe->m_pShell,
                          AEECLSID_MEDIAGALLERY,
                          EVT_USER_REDRAW,
                          0,
                          0);
      }

   }
}//MGAppUtil_FileListBuildComplete


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_VideoDatabaseBuildComplete
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static void MGAppUtil_VideoDatabaseBuildComplete(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   uint32 dwCount = 0;

#ifndef AEE_SIMULATOR
   if(!pMe || !pMe->m_pFilesDB)
   {
      MG_FARF(ADDR, ("VideoDatabaseBuildComplete bad parameter!"));
      return;
   }

   dwCount = IDATABASE_GetRecordCount(pMe->m_pFilesDB);
#else
   dwCount = 1;
#endif

   if(0 == dwCount)
   {
      MGCLOSE_DIALOG(MGDLGRET_NOFILES);
   }
   else
   {
      MGCLOSE_DIALOG(MGDLGRET_DONE);
   }
}//MGAppUtil_VideoDatabaseBuildComplete


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_GetFolderInfoNotify
 * DESCRIPTION  :  获取文件信息时，完成后的回调函数。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static void MGAppUtil_GetFolderInfoNotify(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   IStatic *pDetailText = NULL;
   AECHAR *pszBuffer;
   MGFolderInfo *pDirInfo;
   JulianType jdate;
   AECHAR szTemp[MG_MAX_LINETEXTNUM];
   AECHAR szCont[MG_MAX_LINETEXTNUM];
   //AECHAR szNums[MG_MAX_LINETEXTNUM];
   AECHAR szMemUnit[MG_MEMUNIT_STRLENMAX];

   if(NULL == pMe)
   {
      MG_FARF(ADDR, ("GetFolderInfoNotify bad parameter!"));
      return;
   }

   pDetailText = pMe->m_pDetailText;
   pDirInfo = pMe->m_pFolderInfo;
   if(NULL == pDetailText)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return;
   }

   pszBuffer = (AECHAR*) MALLOC(sizeof(AECHAR) * MG_MAX_TEXT_SIZE);
   if(!pszBuffer)
   {
      MG_FARF(ADDR, ("Memory failed!"));
      return;
   }
   MEMSET(pszBuffer, 0, sizeof(AECHAR) * MG_MAX_TEXT_SIZE);

   GETJULIANDATE(pDirInfo->dwCreationDate, &jdate);
   ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,
                        IDS_MG_CREATIONDATE,
                        szTemp,
                        sizeof(szTemp));
   WSPRINTF(szCont, sizeof(szCont), szTemp,
            jdate.wYear, jdate.wMonth, jdate.wDay);
   WSTRCAT(pszBuffer, szCont);
   WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);

   ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,
                        IDS_MG_FOLDERS,
                        szTemp,
                        sizeof(szTemp));
   WSTRCAT(pszBuffer, szTemp);

   WSPRINTF(szCont, sizeof(szCont), L": %u", pDirInfo->nSubDirsNum);
   WSTRCAT(pszBuffer, szCont);
   WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);

   ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,
                        IDS_MG_FILES,
                        szTemp,
                        sizeof(szTemp));
   WSTRCAT(pszBuffer, szTemp);

   WSPRINTF(szCont, sizeof(szCont), L": %u", pDirInfo->nFilesNum);
   WSTRCAT(pszBuffer, szCont);
   WSTRCAT(pszBuffer, MG_LINEBREAK_WSTR);

   MGAppUtil_SpaceUnitConvert(pMe->m_pShell,
                              (uint32)pDirInfo->lDirSize,
                              szMemUnit,
                              sizeof(szMemUnit));
   ISHELL_LoadResString(pMe->m_pShell,
                        MGRES_LANGFILE,
                        IDS_MG_SIZE,
                        szCont,
                        sizeof(szCont));
   WSTRCAT(szCont, szMemUnit);
   WSTRCAT(pszBuffer, szCont);

   ISTATIC_SetText(pDetailText,
                   NULL,
                   pszBuffer,
                   AEE_FONT_NORMAL,
                   AEE_FONT_NORMAL);
   ISTATIC_Redraw(pDetailText);

   FREEIF(pMe->m_pInputFileName);
   pMe->m_pInputFileName = pszBuffer;
}//MGAppUtil_GetFolderInfoNotify


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_GetMediaDlgStat
 * DESCRIPTION  :  获取Dialog的状态，
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static uint16 MGAppUtil_GetMediaDlgStat(CMediaGalleryApp *pMe,
                                        MediaDlgStat *pGetStat)
{
   MediaDlgStat      eDlgStat;

   if(!pMe )
      return MG_DLGSTAT_UNKNOWN;

   eDlgStat = pMe->m_MediaDlgStat;

   if(NULL  != pGetStat)
      *pGetStat = eDlgStat;

   return eDlgStat;
}//MGAppUtil_GetMediaDlgStat


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_SetMediaDlgStat
 * DESCRIPTION  :  设置Dialog的状态
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
boolean MGAppUtil_SetMediaDlgStat(CMediaGalleryApp *pMe,
                                  MediaDlgStat eSetStat)
{
   MSG_FATAL("MGAppUtil_SetMediaDlgStat Start eSetStat=%d",eSetStat,0,0);
   if(!pMe)
      return FALSE;
   
   if(eSetStat >= MG_DLGSTAT_MAX || eSetStat <= MG_DLGSTAT_INIT)
   {
      MSG_FATAL("Invalid dialog status!!!",0,0,0);
      return FALSE;
   }

   pMe->m_MediaDlgStat = eSetStat;
   MSG_FATAL("MGAppUtil_SetMediaDlgStat Start End",0,0,0);
   return TRUE;
}//MGAppUtil_SetMediaDlgStat


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_IsNeedHandleKey
 * DESCRIPTION  :  按键是否要处理，但手机在枚举文件的过程中/获取文件夹信息的过
 * 程中等异步过程时，除AVK_CLR按键，其余的都不响应。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_IsNeedHandleKey(CMediaGalleryApp *pMe, uint16 nKey)
{
   boolean bHandle = FALSE;

   if(NULL == pMe)
      return bHandle;

  bHandle = ((pMe->m_bCallbackDoing == FALSE &&
              pMe->m_bMediaMenuEmpty == FALSE) ||
            nKey == AVK_CLR);

   return bHandle;
}

/*===========================================================================
 * FUNCTION:MGAppUtil_OnMediaMenuClrKeyEvt
 *
 * DESCRIPTION:handle AVK_CLR of EVT_KEY event
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline boolean MGAppUtil_OnMediaMenuClrKeyEvt(CMediaGalleryApp* pMe,
                                                      IMenuCtl* pMenuCtl)
{
   MediaDlgStat eDlgStat;

   if(!pMe)
   {
      return FALSE;
   }

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   MSG_FATAL("MGAppUtil_OnMediaMenuClrKeyEvt MGAppUtil_SetMediaDlgStat",0,0,0);
   MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

   if(eDlgStat == MG_DLGSTAT_NORMAL)
   {
      const char *pszPath;
      CFSExplorer *pFSExp = &pMe->m_Explorer;

      if(pMe->m_bCallbackDoing && CALLBACK_IsQueued(&pMe->m_CallBack))
      {
         pMe->m_bCallbackDoing = FALSE;
         CALLBACK_Cancel(&pMe->m_CallBack);
         MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
      }
      else
      {
         MGAppUtil_UpdateSelItemCheck(pMe);
      }

      if(MGExplorer_GetCurrentDepth(pFSExp) > MG_CURDEPTH_INIT)
      {
         pszPath = MGExplorer_GetCurrentFolderPath(pFSExp);

         if(SUCCESS == MGExplorer_ChangeCurDir(pFSExp, MG_DIR_GOUP, pszPath))
         {
            // enter subfoler
            MGExplorer_InitBuildMediaMenu(pMe,
                                          MGAppUtil_MediaMenuBuildComplete);
         }
         else
         {
            MG_FARF(ADDR, ("return up folder failed!!!"));
            return FALSE;
         }
      }
      else
      {
         /*when exit media menu, deactive it or delete, otherwise it still
          * active when we back to up menu*/
         // MGExplorer_FreeMediaMenuItem(pMe->m_pMediaMenu);
         IMENUCTL_SetActive(pMe->m_pMediaMenu, FALSE);
         MGExplorer_ResetPreItemID(&pMe->m_Explorer, FALSE);
         MediaGalleryApp_SetOps(pMe, MG_OPS_MEDIAMENU, MG_OP_NULL);
         MGCLOSE_DIALOG(MGDLGRET_CANCELED);
      }
   }
   else if(eDlgStat == MG_DLGSTAT_POPUP) // cancel the pop menu...
   {
      RELEASEIF(pMe->m_pMenuPopup);
      IMENUCTL_SetActive(pMenuCtl, TRUE);
      MG_SENDEVENT(EVT_USER_REDRAW, 0, 0);
   }
   else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
   {
      MediaGalleryApp_CancelMsgBoxTimer(pMe, TRUE);
   }
   else if(eDlgStat == MG_DLGSTAT_YESNOBOX)
   {
      uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);

      MediaGalleryApp_SetMsgBoxID(pMe, MG_MSGID_NULL);
      /*According the phone key map,clear key is use as left softkey too.*/
      IMENUCTL_SetActive(pMenuCtl, TRUE);
      IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
      ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
                       EVT_USER_REDRAW, 0, 0);

      switch(nMsgBoxId)
      {
      case MG_MSGID_DEL:

         break;

      case MG_MSGID_DELALL:
         break;

      case MG_MSGID_OVERWRITE:
         /*clear the previous copy data if have any*/
         MG_CLEAR_COPYDATA(pMe->m_Copy);
         break;

      default:
         break;
      }
   }
   else if(eDlgStat == MG_DLGSTAT_PROGRESS)
   {
      return TRUE;
   }
   else
   {
      MG_FARF(ADDR, ("Error, unknown status !"));
      return FALSE;
   }

   return TRUE;
}//MGAppUtil_OnMediaMenuClrKeyEvt


/*===========================================================================
 * FUNCTION:      MGAppUtil_OnMediaMenuInfoKeyEvt
 *
 * DESCRIPTION:   响应AVK_INFO按键。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline boolean MGAppUtil_OnMediaMenuInfoKeyEvt(CMediaGalleryApp* pMe,
                                                      IMenuCtl* pMenuCtl,
                                                      AEEEvent eCode,
                                                      uint16 wParam,
                                                      uint32 dwParam)
{
   boolean bMenuEmpty;
   MediaDlgStat eDlgStat;
   MSG_FATAL("MGAppUtil_OnMediaMenuInfoKeyEvt Start wParam=0x%x, dwParam=0x%x",wParam,dwParam,0); 
   if(!pMe || !pMenuCtl)
   {
      return FALSE;
   }
   MSG_FATAL("MGAppUtil_OnMediaMenuInfoKeyEvt 1",0,0,0); 
   if((dwParam & KB_AUTOREPEAT) != 0)
   {
      /*Do not handle repeat event for info key*/
      return FALSE;
   }
   MSG_FATAL("MGAppUtil_OnMediaMenuInfoKeyEvt 2",0,0,0); 
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   bMenuEmpty = pMe->m_bMediaMenuEmpty;
   MSG_FATAL("MGAppUtil_OnMediaMenuInfoKeyEvt bMenuEmpty=%d,eDlgStat=%d",bMenuEmpty,eDlgStat,0); 
   if(eDlgStat == MG_DLGSTAT_NORMAL)
   {
       //do default operation on current object
      if(bMenuEmpty == FALSE)
         MediaGalleryApp_OnDefaultOperate(pMe, pMenuCtl,wParam,dwParam);
   }
   else if(eDlgStat == MG_DLGSTAT_POPUP)
   {
      return IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
   }
   else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX||
           eDlgStat == MG_DLGSTAT_PROGRESS ||
           eDlgStat == MG_DLGSTAT_YESNOBOX)
   {
      return FALSE;
   }
   else
   {
      MG_FARF(ADDR, ("Error, unknown status !"));
      return FALSE;
   }

   return TRUE;
}//MGAppUtil_OnMediaMenuInfoKeyEvt

/*===========================================================================
 * FUNCTION:    MGAppUtil_OnMediaMenuSelectKeyEvt
 *
 * DESCRIPTION: 响应AVK_SELECT按键。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline boolean MGAppUtil_OnMediaMenuSelectKeyEvt(CMediaGalleryApp *pMe,
                                       IMenuCtl* pMenuCtl,
                                       AEEEvent eCode,
                                       uint16 wParam,
                                       uint32 dwParam)
{
   MediaDlgStat eDlgStat;
   boolean bMenuEmpty;
   MSG_FATAL("MGAppUtil_OnMediaMenuSelectKeyEvt Start",0,0,0);   
   if(!pMe || !pMenuCtl)
   {
      MG_FARF(ADDR, ("Bad parameter, MGAppUtil_OnMediaMenuSelectKeyEvt!"));
      return FALSE;
   }

   bMenuEmpty = pMe->m_bMediaMenuEmpty;

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   MSG_FATAL("MGAppUtil_OnMediaMenuSelectKeyEvt eDlgStat=%d",eDlgStat,0,0);
   if(eDlgStat == MG_DLGSTAT_NORMAL)
   {
   return MGAppUtil_OnMediaMenuCommandEvt(pMe,
                        pMenuCtl, wParam, dwParam);
   }
   else if(eDlgStat == MG_DLGSTAT_POPUP)
   {
      return IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
   }
   else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX)
   {
      if( bMenuEmpty == FALSE &&
            TRUE == IMENUCTL_IsActive(pMenuCtl))
         IMENUCTL_SetActive(pMenuCtl, FALSE);
   }
   else if(eDlgStat == MG_DLGSTAT_YESNOBOX)
   {
      uint16   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
      boolean  bActiveMenu = TRUE;
      MSG_FATAL("MGAppUtil_OnMediaMenuSelectKeyEvt MGAppUtil_SetMediaDlgStat",0,0,0);
      MGAppUtil_SetMediaDlgStat(pMe, MG_DLGSTAT_NORMAL);

      switch(nMsgBoxId)
      {
         case MG_MSGID_DEL:
            MGAppPopupMenu_OnDelete(pMe, pMenuCtl);
            return TRUE;

         case MG_MSGID_DELALL:
            MGAppPopupMenu_OnDeleteAll(pMe, pMenuCtl);
            return TRUE;

         case MG_MSGID_OVERWRITE:
            if(pMe->m_Copy.m_pDestFile)
            {
               int nRet = SUCCESS;
               /*Clear the destination file , and to overwrite, especial the
                * source file size less than destination file*/
               nRet = IFILE_Truncate(pMe->m_Copy.m_pDestFile, 0);

               if(nRet != SUCCESS)
               {
                  MG_FARF(ADDR, ("IFILE_Truncate failed!!!"));
                  return FALSE;
               }

               IMENUCTL_SetActive(pMenuCtl, TRUE);
               IMENUCTL_Redraw(pMenuCtl);
               IMENUCTL_SetActive(pMenuCtl,FALSE);

               bActiveMenu = FALSE;
               MediaGalleryApp_ShowProgressBox(pMe, MG_MSGID_COPY);

               nRet = MGAppPopupMenu_OnCopy(pMe, pMenuCtl);

               if(nRet == EFILEEOF || nRet == SUCCESS)
                  return TRUE;

            }
            break;

         default:
            break;
      }
     /*According the phone key map, select key is use as right softkey too.*/
      if(TRUE == bActiveMenu)
      {
         IMENUCTL_SetActive(pMenuCtl, TRUE);

         ISHELL_PostEvent(pMe->m_pShell, AEECLSID_MEDIAGALLERY,
                          EVT_USER_REDRAW, 0, 0);
      }
    return TRUE;
   }
   else if(eDlgStat == MG_DLGSTAT_PROGRESS )
   {
      return TRUE;
   }
   else
   {
      MG_FARF(ADDR, ("Error, unknown status !"));
      return FALSE;
   }

   return TRUE;
}//MGAppUtil_OnMediaMenuSelectKeyEvt


/*===========================================================================
 * FUNCTION:      MGAppUtil_OnMediaMenuDefaultKeyEvt
 *
 * DESCRIPTION:   Menu控件按键的默认处理。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppUtil_OnMediaMenuDefaultKeyEvt(CMediaGalleryApp* pMe,
                                                  IMenuCtl* pMenuCtl,
                                                  AEEEvent eCode,
                                                  uint16 wParam,
                                                  uint32 dwParam)
{
   MediaDlgStat eDlgStat;
   boolean bMenuEmpty;
   MSG_FATAL("MGAppUtil_OnMediaMenuDefaultKeyEvt Start",0,0,0);
   if(!pMe || !pMenuCtl)
   {
      MSG_FATAL("Bad parameter, MGAppUtil_OnMediaMenuDefaultKeyEvt!",0,0,0);
      return FALSE;
   }

   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);
   bMenuEmpty = pMe->m_bMediaMenuEmpty;
   MSG_FATAL("eDlgStat=%d, bMenuEmpty=%d",eDlgStat,bMenuEmpty,0); 
   if(eDlgStat == MG_DLGSTAT_NORMAL)
   {
      if(FALSE == bMenuEmpty && FALSE ==IMENUCTL_IsActive(pMenuCtl))
      {
         MSG_FATAL("MGAppUtil_OnMediaMenuDefaultKeyEvt 1",0,0,0);
         IMENUCTL_SetActive(pMenuCtl ,TRUE);
      }
      else
      {
         MSG_FATAL("MGAppUtil_OnMediaMenuDefaultKeyEvt 2",0,0,0);
         IMENUCTL_HandleEvent(pMenuCtl, eCode, wParam, dwParam);
      }
   }
   else if(eDlgStat == MG_DLGSTAT_POPUP)
   {
      return IMENUCTL_HandleEvent(pMe->m_pMenuPopup, eCode, wParam, dwParam);
   }
   else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX||
         eDlgStat == MG_DLGSTAT_PROGRESS ||
         eDlgStat == MG_DLGSTAT_YESNOBOX)
   {
      if(FALSE == bMenuEmpty && TRUE == IMENUCTL_IsActive(pMenuCtl))
         IMENUCTL_SetActive(pMenuCtl, FALSE);
   }
   else
   {
      MG_FARF(ADDR, ("Error, unknown status !"));
      return FALSE;
   }
#ifdef FEATURE_USES_MMS   
    MSG_FATAL("MGAppUtil_OnMediaMenuDefaultKeyEvt m_isForMMS=%d",pMe->m_isForMMS,0,0);
    if((pMe->m_nActiveDlgID == IDD_MG_MUSICETTING) &&
        ((wParam == AVK_SELECT) || (wParam == AVK_INFO)) &&
        pMe->m_isForMMS)
    {
       uint16 i = IMENUCTL_GetSel(pMenuCtl);
       MSG_FATAL("i=%d",i,0,0);
       if(i != 0)
       {
            MGFileInfo *pItemData;
            char  pszTemp[MG_MAX_FILE_NAME]={'/0'};
            MSG_FATAL("MGAppUtil_OnMediaMenuDefaultKeyEvt 0",0,0,0);          
            IMENUCTL_GetItemData(pMenuCtl,i, (uint32*)&pItemData);
            if(pItemData->dwSize >= 300*1024)
            {
               MediaGalleryApp_ShowPromptMsgBox(pMe,
                                                IDS_MG_LARGEFILE,
                                                MESSAGE_ERR,
                                                BTBAR_BACK);
 
               return TRUE;
            }            
            if(pItemData && pItemData->szName) 
            {
                IConfig *pConfig = NULL;	
                DBGPRINTF("szName=%s, len=%d", pItemData->szName, STRLEN(pItemData->szName));
                if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_CONFIG, (void **)&pConfig))
                {
                    MSG_FATAL("Create config interface failed",0,0,0);
                    return FALSE;
                }
                ICONFIG_SetItem(pConfig, CFGI_MMSSOUND,pItemData->szName, sizeof(pItemData->szName));    
                RELEASEIF(pConfig);
            }
       }                       
    }
#endif

   
   MSG_FATAL("MGAppUtil_OnMediaMenuDefaultKeyEvt End",0,0,0);
   return TRUE;

}//MGAppUtil_OnMediaMenuDefaultKeyEvt


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_OnMediaMenuDlgEnd
 * DESCRIPTION  :  响应Dialog的EVT_DIALOG_END事件。
 *
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_OnMediaMenuDlgEnd(CMediaGalleryApp *pMe,
                                           IMenuCtl *pMenuCtl,
                                           uint16 wParam,
                                           uint32 dwParam)
{
   if(NULL == pMe || NULL == pMenuCtl)
   {
      MG_FARF(ADDR, ("Bad parameter, MGAppUtil_OnMediaMenuDlgEnd!"));
      return FALSE;
   }
   //if we are suspending (EVT_APP_SUSPEND is sent before
   //EVT_DIALOG_END). Handle suspend for current status.
   //TODO : SUSPNED
   if(FALSE == pMe->m_bSuspending)
   {
      ;
   }
   else if(TRUE == pMe->m_bSuspending)
   {
      ;
   }

   MediaGalleryApp_CancelMsgBoxTimer(pMe, FALSE);

   MediaGalleryApp_SetCallbackStateStop(pMe);
   MGAppUtil_UpdateSelItemCheck(pMe);


   /*Put last, for the up operation may set menu control to active , ex. if
    * MediaGalleryApp_CancelMsgBoxTimer second parameter set to TRUE, It send
    * sync event , and cause menu control active
    */
   if(TRUE == IMENUCTL_IsActive(pMenuCtl))
   {
      IMENUCTL_SetActive(pMenuCtl, FALSE);
   }

   RELEASEIF(pMe->m_pMenuPopup);

   return SUCCESS;
}//MGAppUtil_OnMediaMenuDlgEnd

static boolean MGAppUtil_OnMediaMenuSelChange(CMediaGalleryApp* pMe,
                                                      uint32 eDlgStat)
{
   boolean bBuildMenu;

   if(!pMe || MG_DLGSTAT_NORMAL != eDlgStat )
   {
      return FALSE;
   }

   bBuildMenu = pMe->m_bCallbackDoing;

   if(TRUE == bBuildMenu)
   {
      return FALSE;
   }
   pMe->m_bUpdateSelItem = TRUE;
   MGExplorer_UpdateMediaMenuTitle(pMe, pMe->m_pMediaMenu);

   ISHELL_SetTimer(pMe->m_pShell,
                   MAX(100, KB_AUTOREPEAT_RATE - 200), //
                   (PFNNOTIFY)MGAppUtil_GetMediaMenuSelectItem,
                   (void *)pMe);

   return TRUE;
}


static boolean MGAppUtil_UpdateSelItemCheck(CMediaGalleryApp *pMe)
{
   MSG_FATAL("UpdateSelItemCheck m_StartMode=%d, m_bUpdateSelItem=%d",pMe->m_StartMode,pMe->m_bUpdateSelItem,0);
   if(NULL == pMe)
      return FALSE;

   if(pMe->m_bUpdateSelItem == TRUE)
   {
      if(pMe->m_StartMode == MGSM_MUSIC_SETTING)
      {
         MGAppUtil_StopRingTonePreview(pMe, TRUE);
         MGAppUtil_GetMediaMenuSelectItem((void *)pMe);
         ISHELL_CancelTimer(pMe->m_pShell,
                            (PFNNOTIFY)MGAppUtil_PreviewRingtone,
                            (void *)pMe);
      }
      else
      {
         ISHELL_CancelTimer(pMe->m_pShell,
                            (PFNNOTIFY)MGAppUtil_GetMediaMenuSelectItem,
                            (void *)pMe);
      }

      MGAppUtil_GetMediaMenuSelectItem((void *)pMe);
      return TRUE;
   }
   return FALSE;
}

static void MGAppUtil_GetMediaMenuSelectItem(void* po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   IMenuCtl *pMenuCtl;

   if(NULL == po)
      return;

   pMenuCtl = pMe->m_pMediaMenu;

   if(!pMenuCtl)
      return;

   pMe->m_bUpdateSelItem = FALSE;
   MGExplorer_GetMediaMenuSelectItem(&pMe->m_Explorer, pMenuCtl);

   IMENUCTL_Redraw(pMenuCtl);
}


/*===========================================================================
 * FUNCTION:      MGAppUtil_OnMediaMenuCommandEvt
 *
 * DESCRIPTION:   显示Dialog中MenuCtl的EVT_COMMAND事件。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static __inline boolean MGAppUtil_OnMediaMenuCommandEvt(CMediaGalleryApp* pMe,
                                                      IMenuCtl* pMenuCtl,
                                                      uint16 wParam,
                                                      uint32 dwParam)
{
   MediaDlgStat eDlgStat;
   MSG_FATAL("MGAppUtil_OnMediaMenuCommandEvt Start",0,0,0);
   if(!pMe || !pMenuCtl)
   {
      MG_FARF(ADDR, ("Bad parameter, MGAppUtil_OnMediaMenuCommandEvt!"));
      return FALSE;
   }
   MGAppUtil_GetMediaDlgStat(pMe, &eDlgStat);

   /* 1, EVT_KEY_REALSE, enter into new dialog by AVK_SELECT, it create
    * pop up menu together.
    * 2, EVT_KEY_PRESS, enter dialog, then press AVK_SELECT, pop up menu
    * display some time, and hide automatic.
    * 3, create pop up menu at EVT_KEY, but BREW may send multi-time.
    * But actually, AEEMenu control will hande EVT_KEY event when it
    * receive AVK_SELECT, and will send EVT_COMMAND asynchronic, so we
    * create pop up menu at EVT_COMMAND.
    * 4, pop up should receive key event, and it do at EVT_KEY
    */
   MSG_FATAL("MGAppUtil_OnMediaMenuCommandEvt eDlgStat=%d",eDlgStat,0,0); 
   if(eDlgStat == MG_DLGSTAT_NORMAL)
   {
      MSG_FATAL("MGAppUtil_OnMediaMenuCommandEvt MGAppUtil_BuildPopupMenuItems",0,0,0);
      if(MGAppUtil_BuildPopupMenuItems(pMe, pMenuCtl) == SUCCESS)
         return TRUE;
   }
   else if(eDlgStat == MG_DLGSTAT_POPUP)
   {
      return MediaGalleryApp_OnPopupMenuCommand(pMe,
            pMenuCtl, wParam, dwParam);
   }
   else if(eDlgStat == MG_DLGSTAT_MESSAGEBOX||
           eDlgStat == MG_DLGSTAT_PROGRESS ||
           eDlgStat == MG_DLGSTAT_YESNOBOX)
   {
      MG_FARF(ADDR, ("Receive EVT_COMMAND when display message box!"));
      return FALSE;
   }
   else
   {
      MG_FARF(ADDR, ("Error, unknown status !"));
      return FALSE;
   }

   return TRUE;
}//MGAppUtil_OnMediaMenuCommandEvt


/*===========================================================================
 * FUNCTION:      MGAppUtil_OnMediaMenuMsgBoxTimeOut
 *
 * DESCRIPTION:   消息框显示结束后，状态处理。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static boolean MGAppUtil_OnMediaMenuMsgBoxTimeOut(CMediaGalleryApp* pMe,
                                                      IMenuCtl* pMenuCtl,
                                                      uint16 wParam,
                                                      uint32 dwParam)
{
   uint16   nMsgBoxId;
   int16    nOps;

   if(!pMe || !pMenuCtl)
   {
      MG_FARF(ADDR, ("Bad parameter, MGAppUtil_OnMediaMenuMsgBoxTimeOut!"));
      return FALSE;
   }

   nMsgBoxId =MediaGalleryApp_GetMsgBoxID(pMe);
   nOps = pMe->m_PopupOps;

   /*If folder is empty*/
   if(pMe->m_bMediaMenuEmpty)
   {
      MGExplorer_UpdateMediaMenuTitle(pMe,pMenuCtl);
      MGAppUtil_UpdateMediaMenuSoftkey(pMe);

      MGAppUtil_OnMediaMenuEmpty(pMe,
            /*MG_ONMENUEMPTY_RETURNUP*/
            MG_ONMENUEMPTY_STAYEMPTY);
   }
   else
   {
      if(nMsgBoxId == MG_MSGID_DONE)
      {
         MGAppPopupMenu_OperationDone(pMe, 0);
      }

      if(nOps == MG_OP_DELALL)
      {
         /*rebuild media menu, for there are still have file(s) or folder(s) */
         MGExplorer_InitBuildMediaMenu(pMe, MGAppUtil_MediaMenuBuildComplete);
         return TRUE;
      }
      else if(nOps == MG_OP_SELECT && nMsgBoxId == MG_MSGID_DONE)
      {
         MGCLOSE_DIALOG(MGDLGRET_SETTING_SELECT);
         return TRUE;
      }
      else
      {
         if(FALSE == IMENUCTL_IsActive(pMenuCtl))
         {
            IMENUCTL_SetActive(pMenuCtl, TRUE);
         }
         MGAppUtil_UpdateMediaMenuSoftkey(pMe);
      }
   }

   return TRUE;
}//MGAppUtil_OnMediaMenuMsgBoxTimeOut


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_OnMediaMenuEmpty
 * DESCRIPTION  :  当菜单为空时，如何处理：或者显示一个Message box，然后退回到
 * 上个界面；或者显示No record，停留在当前界面。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_OnMediaMenuEmpty(CMediaGalleryApp *pMe, uint8 nType)
{
   if(NULL == pMe)
   {
      MG_FARF(ADDR, ("Bad parameter On media menu empty!"));
      return FALSE;
   }

   if(nType == MG_ONMENUEMPTY_MESSAGEBOX)
   {
      MediaGalleryApp_SetMsgBoxID(pMe, MG_MSGID_NOFILE);
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_EMPTY,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
   }
   else if(nType == MG_ONMENUEMPTY_STAYEMPTY)
   {
      AECHAR wstrText[16] = {0};
      RGBVAL fontColor;
      IMenuCtl *pMenuCtl = pMe->m_pMediaMenu;
      IDisplay *piDisp = pMe->m_pDisplay;

      if(IMENUCTL_IsActive(pMenuCtl))
         IMENUCTL_SetActive(pMenuCtl, FALSE);

      ISHELL_LoadResString(pMe->m_pShell,
            MGRES_LANGFILE,
            IDS_MG_EMPTY,
            wstrText,
            sizeof(wstrText));
      fontColor = IDISPLAY_SetColor(piDisp, CLR_USER_TEXT, RGB_WHITE);

      IDISPLAY_DrawText(piDisp, AEE_FONT_NORMAL, wstrText, -1, 6,
            6 + TITLEBAR_HEIGHT,
            NULL,
            IDF_TEXT_TRANSPARENT);
      IDISPLAY_SetColor(piDisp, CLR_USER_TEXT, fontColor);
      IDISPLAY_UpdateEx(piDisp, FALSE);
   }
   else if(nType == MG_ONMENUEMPTY_RETURNUP)
   {
      const char *pszPath;
      CFSExplorer *pFSExp = &pMe->m_Explorer;
      pszPath = MGExplorer_GetCurrentFolderPath(pFSExp);

      if(MGExplorer_GetCurrentDepth(pFSExp) > MG_CURDEPTH_INIT)
      {
         /*back to parent foler*/
         if(SUCCESS == MGExplorer_ChangeCurDir(pFSExp, MG_DIR_GOUP, pszPath))
         {

            MGExplorer_InitBuildMediaMenu(pMe,
                                          MGAppUtil_MediaMenuBuildComplete);
         }
         else
         {
            MG_FARF(ADDR, ("return up folder failed!!!"));
            return FALSE;
         }
      }
      else
      {
         /*return up menu of menu tree*/
         MGCLOSE_DIALOG(MGDLGRET_CANCELED);
      }
   }
   else
   {
      return FALSE;
   }

   return TRUE;
}//MGAppUtil_OnMediaMenuEmpty

/*===========================================================================
 * FUNCTION:     MGAppUtil_ExplorerBuildSelItems
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppUtil_ExplorerBuildSelItems(CMediaGalleryApp *pMe,
                                          IMenuCtl *pMenuCtl)
{
   uint32 nSize;
   uint16 i;
   //uint16 nIndex;
   MGFileInfo *pObj;
  // boolean  bSel;
   FileNamesBuf pszTemp;

   if(!pMe || !pMenuCtl)
   {
      MG_FARF(ADDR, ("Error, bad param"));
      return EFAILED;
   }

   if(0 == pMe->m_nSelNum)
   {
      /*no item select*/
      IMENUCTL_SetActive(pMenuCtl, FALSE);
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_SELECTNONE,
                                       MESSAGE_WARNNING,
                                       BTBAR_BACK);

      return SUCCESS;
   }

   FREEIF(pMe->m_pszNameList);
   nSize = pMe->m_nSelNum * MG_MAX_FILE_NAME;
   pMe->m_pszNameList = (FileNamesBuf)MALLOC(nSize);

   if(!pMe->m_pszNameList)
      return FALSE;

   pszTemp = pMe->m_pszNameList;

#if 1
   if(FALSE == IMENUCTL_EnumSelInit(pMenuCtl))
      return FALSE;

   do{
      i = IMENUCTL_EnumNextSel(pMenuCtl);
       if(i != (uint16)-1)
      {
         if(TRUE == IMENUCTL_GetItemData(pMenuCtl, i, (uint32 *)&pObj))
         {
            MEMCPY(pszTemp, pObj->szName,
                  sizeof(pObj->szName));
            pszTemp++;
         }
      }
   }while(i != (uint16)-1);
#else

   nSize = IMENUCTL_GetItemCount(pMenuCtl);
   for(i = 0; i < nSize; i++)
   {
      /*Use IMENUCTL_EnumSelInit & IMENUCTL_EnumNextSel is
       * fine too.*/
      nIndex = IMENUCTL_GetItemID(pMenuCtl, i);
      bSel = GetCheckBoxVal(pMenuCtl, nIndex
            /* (uint16)(MG_EXPLORER_PARENTDIR + i)*/);

      if(bSel)
      {
         if(SUCCESS == IMENUCTL_GetItemData(pMenuCtl, i, (uint32 *)&pObj))
         {
            MG_FARF(ADDR, ("Select file %s!", pObj->m_szNodeName));
            MEMCPY(pszTemp, pObj->m_szNodeName,
                  sizeof(pObj->m_szNodeName));
            pszTemp++;
         }
      }
   }
#endif

   MediaGallery_DoExplorerAddCB(pMe->m_pszNameList,
         pMe->m_nSelNum);

   MGCLOSE_DIALOG(MGDLGRET_FILEADD_FINISH);

   return SUCCESS;
}//MGAppUtil_ExplorerBuildSelItems

/*===========================================================================
 * FUNCTION:    MGAppUtil_ExplorerGetSelectCount
 *
 * DESCRIPTION: 复选列表中，选中的项数。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppUtil_ExplorerGetSelectCount(CMediaGalleryApp *pMe,
                                          IMenuCtl *pMenuCtl)
{
   uint16 wID;
   boolean bSelMax = FALSE;

   if(!pMe || !pMenuCtl)
   {
      MG_FARF(ADDR, ("Error, bad param"));
      return EFAILED;
   }

   pMe->m_nSelNum = 0;

   if(FALSE == IMENUCTL_EnumSelInit(pMenuCtl))
   {
      return EFAILED;
   }

   while(1){
      wID =  IMENUCTL_EnumNextSel(pMenuCtl);

      if((uint16)-1 != wID)
      {
         if(MG_SELECT_MAX >= pMe->m_nSelNum)
         {
            pMe->m_nSelNum++;
         }
         else
         {
            if(!bSelMax)
               bSelMax = TRUE;
            IMENUCTL_SetSelEx(pMenuCtl, wID, FALSE);
         }
      }
      else
      {
         /*finish enumerate, exit loop*/
         break;
      }
   }

   if(TRUE == bSelMax)
   {
      //TO DO: Pop up message box to indicate user
      MG_FARF(ADDR, ("Select files exceed %d",MG_SELECT_MAX));
      IMENUCTL_SetActive(pMenuCtl, FALSE);
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_LISTFULL,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
   }
   return SUCCESS;
}//MGAppUtil_ExplorerGetSelectCount


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_NextVideoCheck
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGAppUtil_NextVideoCheck(CMediaGalleryApp *pMe,
                                                 MGFileInfo *pItemInfo)
{
   int        nRet;
   MGMimeType     eMimeBase;

   if(!pMe || !pMe->m_pShell || !pMe->m_pFileMgr || !pItemInfo)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return FALSE;
   }

   if(pItemInfo->attrib == AEE_FA_DIR)
   {
      return FALSE;
   }

   nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                  pMe->m_pFileMgr,
                                  pItemInfo->szName,
                                  MGMIME_BASE_VIDEO,
                                  &eMimeBase);

   if(SUCCESS == nRet)
   {
      return TRUE;
   }

   return FALSE;
}


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_NextMusicCheck
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static __inline boolean MGAppUtil_NextMusicCheck(CMediaGalleryApp *pMe,
                                    MGFileInfo *pItemInfo)
{
   int        nRet;
   MGMimeType     eMimeBase;

   if(!pMe || !pMe->m_pShell || !pMe->m_pFileMgr || !pItemInfo){
      MG_FARF(ADDR, ("Bad parameter"));
      return FALSE;
   }

   if(pItemInfo->attrib == AEE_FA_DIR){
      return FALSE;
   }

   nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                  pMe->m_pFileMgr,
                                  pItemInfo->szName,
                                  MGMIME_BASE_MUSIC,
                                  &eMimeBase);

   if(SUCCESS == nRet){
      return TRUE;
   }

   return FALSE;
}

/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_ImageSlideCheck
 * DESCRIPTION  :
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_ImageSlideCheck(CMediaGalleryApp *pMe,
                                         MGFileInfo *pItemInfo)
{
   int        nRet;
   MGMimeType     eMimeBase;
   MSG_FATAL("MGAppUtil_ImageSlideCheck Start",0,0,0);
   if(!pMe || !pMe->m_pShell || !pMe->m_pFileMgr || !pItemInfo){
      MSG_FATAL("Bad parameter",0,0,0);
      return FALSE;
   }

   if(pItemInfo->attrib == AEE_FA_DIR){
      return FALSE;
   }

   nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
                                  pMe->m_pFileMgr,
                                  pItemInfo->szName,
                                  MGMIME_BASE_IMAGE ,
                                  &eMimeBase);

   if(SUCCESS == nRet){
      return TRUE;
   }
   MSG_FATAL("MGAppUtil_ImageSlideCheck End",0,0,0);
   return FALSE;
}//MGAppUtil_ImageSlideCheck


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_ImageZoomCheck
 * DESCRIPTION  :  检查图片是否支持缩放
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_ImageZoomCheck(CMediaGalleryApp *pMe,
                                    MGFileInfo *pItemInfo)
{
   int        nRet;
   MGMimeType     eMimeBase;

   if(!pMe || !pMe->m_pShell || !pMe->m_pFileMgr || !pItemInfo){
      MG_FARF(ADDR, ("Bad parameter"));
      return FALSE;
   }

   if(pItemInfo->attrib == AEE_FA_DIR){
      return FALSE;
   }

   nRet = MGMediaInfo_GetMimeType(pMe->m_pShell,
         pMe->m_pFileMgr,
         pItemInfo->szName,
         MGMIME_BASE_IMAGE ,
         &eMimeBase);
   
   if(SUCCESS == nRet &&
      (eMimeBase == MG_MIME_PNG ||
       eMimeBase == MG_MIME_BMP ||
       eMimeBase == MG_MIME_JPEG ||
       eMimeBase == MG_MIME_JPG)){
      return TRUE;
   }

   return FALSE;
}//MGAppUtil_ImageZoomCheck


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_WallpaperSettingCheck
 * DESCRIPTION  :  检查图片是否可以设置为墙纸。现在没有限制图片文件的大小，而
 * 是限制图片显示的长/宽像素大小。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_WallpaperSettingCheck(IImage *po,
                                               AEEImageInfo *pImgInfo,
                                               IShell *ps)
{
   AEEDeviceInfo DevInfo;
   AEEImageInfo ImgInfo;
   AEEImageInfo *pi;

   if(!(pImgInfo || po) || !ps)
   {    
   	 return FALSE;
   	}

   if(pImgInfo)
   {
   	pi = pImgInfo;
   }
   else
   {
      IIMAGE_GetInfo(po, &ImgInfo);
      pi = &ImgInfo;
   }

   if(pi->bAnimated)
   {   	
   	return FALSE;
   }

   ISHELL_GetDeviceInfo(ps, &DevInfo);
   MG_FARF(ADDR, ("DEVINFO CX:%d, CY:%d", DevInfo.cxScreen, DevInfo.cyScreen));
   MG_FARF(ADDR, ("IMGINFO CX:%d, CY:%d", pi->cx,pi->cy));

   MSG_FATAL("***zzg DEVINFO CX:%d, CY:%d***", DevInfo.cxScreen, DevInfo.cyScreen, 0);
   MSG_FATAL("***zzg IMGINFO CX:%d, CY:%d***",  pi->cx,pi->cy, 0);

   if(MG_BETWEEN( pi->cx, MG_WALLPAPER_PIXEL_MIN, MG_WALLPAPER_PIXEL_MAX) && MG_BETWEEN(pi->cy, MG_WALLPAPER_PIXEL_MIN, MG_WALLPAPER_PIXEL_MAX))
      return TRUE;
   else
	  return FALSE;
}//MGAppUtil_WallpaperSettingCheck


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_SetWallpaper
 * DESCRIPTION  :  把文件列表中的当前图片设为待机图片。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
#ifndef BI_BITFIELDS
#define BI_BITFIELDS    3
#endif
#define HEXING_OEM_WALLPAPER1 MG_PHONEMUSIC_PATH"oemwallpaper1.bmp"
#define HEXING_OEM_WALLPAPER2 MG_PHONEMUSIC_PATH"oemwallpaper2.bmp"

// BMP转换用
typedef PACKED struct _DIBFileHeader           // DIB文件头
{
  uint16        bfType ;          // 标记为"BM"或0x4D42
  uint32        bfSize ;          // 整个文件的尺寸
  uint16        bfReserved1 ;     // 保留，为0
  uint16        bfReserved2 ;     // 保留，为0
  uint32        bfOffsetBits ;    // DIB像素在文件中的偏移
} DIBFileHeader;

typedef PACKED struct _DIBInfoHeader 
{
  uint32 biSize ;              // 结构体的大小 = 40
  uint32 biWidth ;             // 图片的宽度（像素）
  int32  biHeight ;            // 图片的高度（像素）
  uint16 biPlanes ;            // = 1
  uint16 biBitCount ;          // 色深(1, 4, 8, 16, 24, or 32)
  uint32 biCompression ;       // 压缩码
  uint32 biSizeImage ;         // 图像的字节数
  uint32 biXPelsPerMeter ;     // horizontal resolution
  uint32 biYPelsPerMeter ;     // vertical resolution
  uint32 biClrUsed ;           // 使用的颜色数量
  uint32 biClrImportant ;      // 重要的颜色数量
}DIBInfoHeader;


// 将指定的IBitmap写成pFileName命名的文件
int ImageExplorer_WriteDIBFile(IShell *pIShell, char *pFileName, IBitmap *pBmp)
{
  IFileMgr *pFileMgr = NULL;
  int       ret= AEE_SUCCESS;
  IFile    *pDIBFile = NULL;
  IDIB     *pDIB = NULL;
  uint32    dwDIBSize = 0;
  boolean   bWirteScheme = FALSE;
  uint32    dwScheme[3] = {0};
  
  DIBFileHeader myDIBFileHeader = {0};
  DIBInfoHeader myDIBInfoHeader = {0};
  
  if(!pIShell || !pFileName || !pBmp)
  {
    MSG_FATAL("invalid param",0,0,0);
    return EBADPARM;
  }
  
  if(AEE_SUCCESS != IBITMAP_QueryInterface(pBmp, AEECLSID_DIB, (void **)&pDIB))
  {
	MSG_FATAL("IBITMAP_QueryInterface failed",0,0,0);
    return EFAILED;
  }
  
  ret = ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void **)&pFileMgr);
//  MSG_FATAL("filename =%",0,0,0);

//  LOG("filename=%s", pFileName);

  //如果存在同名的文件，就删除原有文件
  if (AEE_SUCCESS == IFILEMGR_Test(pFileMgr, HEXING_OEM_WALLPAPER1))
  {    
    MSG_FATAL("IFILEMGR_Test failed",0,0,0);
    ret = IFILEMGR_Remove(pFileMgr, HEXING_OEM_WALLPAPER1);//
  }
  
  if (AEE_SUCCESS == IFILEMGR_Test(pFileMgr, HEXING_OEM_WALLPAPER2))
  {    
    MSG_FATAL("IFILEMGR_Test failed",0,0,0);
    ret = IFILEMGR_Remove(pFileMgr, HEXING_OEM_WALLPAPER2);//
  }
  pDIBFile = IFILEMGR_OpenFile(pFileMgr, pFileName, _OFM_CREATE);
  if(!pDIBFile)
  {  
    MSG_FATAL("IFILEMGR_OpenFile failed",0,0,0);
    ret = EFAILED;
  }
  
  dwDIBSize = pDIB->cx*pDIB->cy*((pDIB->nDepth+7)/8); 
  
  // 设置BMP文件头信息
  myDIBFileHeader.bfType          = 0x4D42;
  myDIBFileHeader.bfSize          = (uint32)(sizeof(myDIBFileHeader)+sizeof(myDIBInfoHeader)+dwDIBSize);
  myDIBFileHeader.bfReserved1     = 0;
  myDIBFileHeader.bfReserved2     = 0;
  myDIBFileHeader.bfOffsetBits    = sizeof(myDIBFileHeader)+sizeof(myDIBInfoHeader);
  
  // 设置BMP信息头
  myDIBInfoHeader.biSize          = sizeof(myDIBInfoHeader);
  myDIBInfoHeader.biWidth         = pDIB->cx;
  myDIBInfoHeader.biHeight        = pDIB->cy;
  myDIBInfoHeader.biPlanes        = 1;
  myDIBInfoHeader.biBitCount      = pDIB->nDepth;
  myDIBInfoHeader.biCompression   = 0;
  myDIBInfoHeader.biSizeImage     = dwDIBSize;
  myDIBInfoHeader.biXPelsPerMeter = 0;
  myDIBInfoHeader.biYPelsPerMeter = 0;
  myDIBInfoHeader.biClrUsed       = 0;
  myDIBInfoHeader.biClrImportant  = 0;
  
  if(pDIB->nPitch>0)
  {
    // TOP-DOWN Image
    myDIBInfoHeader.biHeight = -myDIBInfoHeader.biHeight;
  }
  else
  {
    // DOWN-TOP-DOWN
    // Nothing Todo
  }
  
  if(pDIB->nColorScheme == IDIB_COLORSCHEME_555)
  {
    // Nothing todo
  }
  else if(pDIB->nColorScheme == IDIB_COLORSCHEME_565)
  {
    myDIBInfoHeader.biCompression   = BI_BITFIELDS;
    dwScheme[0] = 0xF800;
    dwScheme[1] = 0x07E0;
    dwScheme[2] = 0x001F;
    bWirteScheme = TRUE;
    myDIBFileHeader.bfSize += sizeof(dwScheme);
    myDIBFileHeader.bfOffsetBits += sizeof(dwScheme);
  }
  // 写入文件
#ifdef WIN32
  {
    byte buf[14];
    myDIBFileHeader.bfSize       -= 2;
    myDIBFileHeader.bfOffsetBits -= 2;
    *((uint16*)(&buf[0]))  = myDIBFileHeader.bfType;
    *((uint32*)(&buf[2]))  = myDIBFileHeader.bfSize;
    *((uint16*)(&buf[6]))  = myDIBFileHeader.bfReserved1;
    *((uint16*)(&buf[8]))  = myDIBFileHeader.bfReserved2;
    *((uint32*)(&buf[10])) = myDIBFileHeader.bfOffsetBits;
    
    if (0 == IFILE_Write(pDIBFile, buf, 14))
    {
      ret = EFAILED;
	  MSG_FATAL("IFILE_Write failed",0,0,0);
    }
  }
#else
  if (0 == IFILE_Write(pDIBFile, &myDIBFileHeader, sizeof(myDIBFileHeader)) )
  {
    ret = EFAILED;
    MSG_FATAL("IFILE_Write failed",0,0,0);
  }
    
#endif
  if (0 == IFILE_Write(pDIBFile, &myDIBInfoHeader, sizeof(myDIBInfoHeader)))
  {
    ret = EFAILED;
    MSG_FATAL("IFILE_Write failed",0,0,0);
  }
    
  if(bWirteScheme)
  {
    if (0 == IFILE_Write(pDIBFile, dwScheme, sizeof(dwScheme)) )
    {
      ret = EFAILED;
	  MSG_FATAL("IFILE_Write failed",0,0,0);
    }
  }
  
  if (0 == IFILE_Write(pDIBFile, pDIB->pBmp, dwDIBSize))
  {
    ret = EFAILED;
    MSG_FATAL("IFILE_Write failed",0,0,0);
  }
    
  //  MSG_FATAL("ImageExplorer_WriteDIBFile Leave",0,0,0);

error:  
  RELEASEIF(pDIB);
  RELEASEIF(pDIBFile);
  RELEASEIF(pFileMgr);
  return ret;
}


// 将指定的IImage转换成指定的IBitmap接口
int ImageExplorer_ImageToBmp(IDisplay *pIDisplay, 
                             IImage  *pImage, 
                             IBitmap **ppBmp, 
                             uint16  wExpectWidth, 
                             uint16  wExpectHeight)
{
  int         nRet = AEE_SUCCESS;
  IBitmap     *pSysBmp = NULL;
  IBitmap     *pNewBmp = NULL;
  AEEImageInfo ImageInfo;
  
  if(!pIDisplay || !pImage || !ppBmp)
  {
    MSG_FATAL("invalid param",0,0,0);
    return EBADPARM;
  }

  if (wExpectWidth == 0  || wExpectHeight == 0)
  {
    MSG_FATAL("invalid param, width or height is zero",0,0,0);
    return EBADPARM;
  }

  MSG_FATAL("Enter ImageExplorer_ImageToBmp",0,0,0);
  
  IIMAGE_GetInfo(pImage, &ImageInfo);
 // MSG_FATAL("cx=%d, cy=%d", ImageInfo.cx, ImageInfo.cy,0);
  wExpectWidth  = wExpectWidth <ImageInfo.cx ? wExpectWidth : ImageInfo.cx;
  wExpectHeight = wExpectHeight<ImageInfo.cy ? wExpectHeight : ImageInfo.cy;
  
  //MSG_FATAL("wExpectWidth=%d, wExpectHeight=%d", wExpectWidth, wExpectHeight,0);

  if (wExpectWidth == 0  || wExpectHeight == 0)
  {
    MSG_FATAL("invalid param, width or height is zero",0,0,0);
    return EBADPARM;
  }

  // 复制一个系统Display
  pSysBmp = IDISPLAY_GetDestination(pIDisplay);
  if(!pSysBmp)
  {
    return EFAILED;
  }
  
  // 创建一个新的Bitmap接口
  nRet = IBITMAP_CreateCompatibleBitmap(pSysBmp, &pNewBmp, wExpectWidth, wExpectHeight);
  if(nRet != AEE_SUCCESS)
  {
    RELEASEIF(pSysBmp);
    return nRet;
  }
  
  nRet = IDISPLAY_SetDestination(pIDisplay, pNewBmp);
  if(nRet != AEE_SUCCESS)
  {
    RELEASEIF(pSysBmp);
    RELEASEIF(pNewBmp);
    return nRet;
  }
  // 缩小图像尺寸并刷新到pNewBmp中去
  IIMAGE_SetParm(pImage, IPARM_SCALE, wExpectWidth, wExpectHeight);
  IIMAGE_Draw(pImage, 0, 0);
  
  RELEASEIF(pSysBmp);
  //IBITMAP_Release(pNewBmp);//此指针需要传出，因此不再释放了
  IDISPLAY_SetDestination(pIDisplay, NULL);
  *ppBmp = pNewBmp;
  return AEE_SUCCESS;
}

static boolean MGAppUtil_SetWallpaper(CMediaGalleryApp *pMe,
                                      AEEImageInfo *pImgInfo)
{
   IConfig *pConfig = NULL;
   MGFileInfo *pSelData;   
   
   //add by miaoxiaoming
   AEEImageInfo myInfo;	   
   IBitmap *pIBitmap = NULL;
   char BmpFileName[MG_MAX_FILE_NAME];
   int ret = EFAILED;
   
   if(NULL == pMe||pMe->m_pImage==NULL)
   {
      return FALSE;
   }

   pSelData = MediaGalleryApp_GetCurrentNode(pMe);

   if(NULL == pSelData)
   {
      MG_FARF(STATE, ("ERR, LoadMediaNotify"));
      return FALSE;
   }

   //Add By zzg 2011_12_09 
   MSG_FATAL("pImgInfo->bAnimated=====%dpMe->m_Gif=%d",pImgInfo->bAnimated,pMe->m_Gif,0);
   if ((TRUE == pImgInfo->bAnimated) || (pMe->m_Gif ==TRUE))
   {
   	  MSG_FATAL("MediaGalleryApp_ShowPromptMsgBox",0,0,0);
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_ANIMATE_CANNOTSET_WALLPAPER,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
      return FALSE;
   }
   //Add End

   if(FALSE == MGAppUtil_WallpaperSettingCheck(NULL, pImgInfo, pMe->m_pShell))
   {
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_WALLPAPERNOSET,
                                       MESSAGE_INFORMATION,
                                       BTBAR_BACK);
      return FALSE;
   }
   
   if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                       AEECLSID_CONFIG,
                                       (void **)&pConfig))
   {
      MG_FARF(ADDR, ("Create config interface failed"));
      return FALSE;
   }
	/*   	
   pImage= ISHELL_LoadImage(pMe->m_pShell,pSelData->szName);
   if (NULL == pImage)
   {
   	  MSG_FATAL("MGAppUtil_SetWallpaper ISHELL_LoadImage failed",0,0,0);
      return FALSE;
   }
   */

	IImage_GetInfo(pMe->m_pImage,&myInfo);
#ifdef FEATURE_BREW_SCALE
	//MSG_FATAL("pMe->m_rc.dx=%d pMe->m_rc.dy=%d",pMe->m_rc.dx,pMe->m_rc.dy,0);
	//MSG_FATAL("myInfo.cx=%d myInfo.cy=%d",myInfo.cx,myInfo.cy,0);
    if(myInfo.cy > 0 && myInfo.cx > 0)
    {
#if 1    
        if((myInfo.cx*1000)/myInfo.cy > (pMe->m_rc.dx*1000)/pMe->m_rc.dy)
        {
            myInfo.cx = (myInfo.cx*pMe->m_rc.dy)/myInfo.cy;
            myInfo.cy = pMe->m_rc.dy;
        }
        else
        {
            myInfo.cy = (myInfo.cy*pMe->m_rc.dx)/myInfo.cx;
            myInfo.cx = pMe->m_rc.dx;
        }

        IImage_SetParm(pMe->m_pImage,
               IPARM_SCALE,
               myInfo.cx,
               myInfo.cy);
#else

        IImage_SetParm(pMe->m_pImage,
                       IPARM_SCALE,
                       pMe->m_rc.dx,
                       pMe->m_rc.dy);
#endif
    }
		
	//MSG_FATAL("myInfo.cx=%d myInfo.cy=%d",myInfo.cx,myInfo.cy,0);
#endif

    ret = ImageExplorer_ImageToBmp(pMe->m_pDisplay,pMe->m_pImage,&pIBitmap,myInfo.cx,myInfo.cy);
    if (ret != SUCCESS)
	{
		
		RELEASEIF(pConfig);
		if (pMe->m_pImage!=NULL)
		{
			IImage_Release(pMe->m_pImage);
		}
		if (NULL != pIBitmap)
		{
			IBase_Release((IBase *)pIBitmap);
		}
		
		MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_ERROR);
		return FALSE;
	}

	ICONFIG_GetItem(pConfig, CFGI_WALLPAPER,
					BmpFileName, sizeof(BmpFileName));
	if(STRNCMP(BmpFileName,HEXING_OEM_WALLPAPER1,STRLEN(HEXING_OEM_WALLPAPER1))!=0)
	{		
		MSG_FATAL("HEXING_OEM_WALLPAPER2",0,0,0);
		STRCPY(BmpFileName,HEXING_OEM_WALLPAPER1);	
	}
	else
	{
		MSG_FATAL("HEXING_OEM_WALLPAPER2",0,0,0);
        STRCPY(BmpFileName,HEXING_OEM_WALLPAPER2);	
	}

		
	ret = ImageExplorer_WriteDIBFile(pMe->m_pShell,BmpFileName,pIBitmap);
    if (ret != SUCCESS)
	{		
		RELEASEIF(pConfig);
		if (pMe->m_pImage!=NULL)
		{
			IImage_Release(pMe->m_pImage);
		}
		if (NULL != pIBitmap)
		{
			IBase_Release((IBase *)pIBitmap);
		}
		
		MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_ERROR);
		return FALSE;
	}
	
	if (NULL != pIBitmap)
	{
		IBase_Release((IBase *)pIBitmap);
	}
	
	//MSG_FATAL("MGAppUtil_SetWallpaper Leave",0,0,0);
   
   ICONFIG_SetItem(pConfig, CFGI_WALLPAPER,
                   BmpFileName, sizeof(BmpFileName));

   MGAppPopupMenu_OperationDone(pMe, MG_FNSHOP_DONE);

   RELEASEIF(pConfig);
   if(pMe->m_PopupOps == MG_OP_WALLPAPER)
   {
      RELEASEIF(pMe->m_pImage);
   }
   return TRUE;
}


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_DrawImageViewerBG
 * DESCRIPTION  :  在显示图片时绘制背景图片。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_DrawImageViewerBG(CMediaGalleryApp *pMe)
{
   IImage *pImage = NULL;
   MSG_FATAL("MGAppUtil_DrawImageViewerBG Start",0,0,0);
   if(!pMe)
   {
      MSG_FATAL("MGAppUtil_DrawImageViewerBG Parameter error!",0,0,0);
      return FALSE;
   }

   pImage = ISHELL_LoadResImage(pMe->m_pShell,
         AEE_APPSCOMMONRES_IMAGESFILE,
         IDB_BACKGROUND);  //modified by yangdecai

   if(NULL != pImage)
   {
      IIMAGE_Draw(pImage, 0, 0);
   }

   RELEASEIF(pImage);
   MSG_FATAL("MGAppUtil_DrawImageViewerBG End",0,0,0);
   return TRUE;
}


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_RedrawImage
 * DESCRIPTION  :  显示图片，其中SD卡上的图片使用848解码，手机上的使用Qualcomm
 * CMX解码。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static int MGAppUtil_RedrawImage(CMediaGalleryApp *pMe,
                                 AEEImageInfo *pInfo,
                                 boolean bResume)
{
   AEEImageInfo ImgInfo;
   AEERect rc;
   int nDrawX;
   int nDrawY;
   int  nAscent;
   int  nDescent;
   AEEImageInfo *pi = pInfo;
   AECHAR *pszTitle = NULL;
   IImage *po = NULL;
   IDisplay *pDisp = NULL;
   const char *pszPath = NULL;
   const char *pszFileName = NULL;
   boolean bRet = FALSE;
   AECHAR wszTitle[MG_MAX_LINETEXTNUM * 2];
   MSG_FATAL("MGAppUtil_RedrawImage Start",0,0,0);
   if(NULL == pMe)
      return EFAILED;

   MEMSET(wszTitle, 0, sizeof(wszTitle));
   pDisp = pMe->m_pDisplay;
   pszPath = MediaGalleryApp_GetCurrentNodeName(pMe);
   pszFileName = BASENAME(pszPath);

   UTF8TOWSTR((const byte*)pszFileName,
              sizeof(char) * (STRLEN(pszFileName) + 1),
              wszTitle,
              sizeof(wszTitle));
   IDISPLAY_GetFontMetrics(pDisp,
                           AEE_FONT_BOLD,
                           &nAscent,
                           &nDescent);
   
   po = pMe->m_pImage;
   if(NULL == po)
   {
      FREEIF(pszTitle);
      MSG_FATAL("Image interface have been release",0,0,0);
      return EFAILED;
   }

   MGAppUtil_DrawImageViewerBG(pMe);

   if(NULL == pInfo)
   {
      pi = &ImgInfo;
      IIMAGE_GetInfo(po, pi);
   }
   MSG_FATAL("pi->cx==%d,pi->cy=%d",pi->cx,pi->cy,0);

   rc = pMe->m_rc;

   /*对于动画,由于目前不能支持每帧刷文字,留空间显示标题和软键 */
   if(pi->bAnimated)
   {
      rc.y = nAscent + nDescent + 2;
      rc.dy -= rc.y * 2;
      rc.y++;
   }

   MSG_FATAL("APPLET rectangle cx:%d, cy:%d ", rc.dx, rc.dy,0);

   nDrawX = pi->cx > rc.dx ? 0 : ((rc.dx - pi->cx) / 2);
   nDrawY = pi->cy > rc.dy ? 0 : ((rc.dy - pi->cy) / 2);

   if(pi->cx > rc.dx || pi->cy > rc.dy)  //MODI BY yandecai 2011-07-28
   	{
   	  int x;
	  int y;
	  int scalex = pi->cx/rc.dx;
	  int scaley = pi->cy/rc.dy;
	  int mx = pi->cx%rc.dx;
	  int my = pi->cy%rc.dy;
	  if(mx>0)
	  {
	  	  scalex++;
	  }
	  if(my>0)
	  {
	  	  scaley++;
	  }
      //IIMAGE_SetDrawSize(po, rc.dx, rc.dy);
      if(scalex>scaley)
      {
      	  x = pi->cx/scalex;
		  y = pi->cy/scalex;
      }
	  else
	  {
	  	  x = pi->cx/scaley;
		  y = pi->cy/scaley;
	  }
      
      IIMAGE_SetParm(po,
                     IPARM_SCALE,
                     x,
                     y);
	  nDrawX = (rc.dx -x)/2;
	  nDrawY = (rc.dy -y)/2;
   	}

   if(pi->bAnimated)
   {
      nDrawY += rc.y;

      MSG_FATAL("nDrawX:%d, nDrawY:%d, rc.y:%d", nDrawX, nDrawY, rc.y);

      if(FALSE == bResume)
      {
         MSG_FATAL("cx:%d, cxFrame:%d",pi->cx, pi->cxFrame,0);
#ifdef FEATURE_ANICTL
         if (pMe->m_pAniCtl == NULL)
         {
            if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                AEECLSID_ANICTL,
                                                (void **)&pMe->m_pAniCtl))
            {
               MSG_FATAL("Error create anictl",0,0,0);
               return EFAILED;
            }
         }
     //    IAniCtl_StartDisplay(pMe->m_pAniCtl, po, NULL);
         return SUCCESS;
#else
         IIMAGE_Start(po, nDrawX, nDrawY);
#endif
      }
      else
      {
         IIMAGE_DrawFrame(po, 0, nDrawX, nDrawY);
      }
   }
   else
   {
      IIMAGE_Draw(po, nDrawX, nDrawY);
   }

   rc = pMe->m_rc;

   rc.x = MGAppUtil_DrawTitleArrow(pMe, NULL) + 1;
   rc.dx = pMe->m_rc.dx - rc.x * 2;
   rc.y = 1;
   rc.dy = nAscent + nDescent;
   bRet = MGAppUtil_ShrinkString(pDisp,
                                 rc.dx,
                                 wszTitle,
                                 (AECHAR **)&pszTitle);
   if(NULL == pszTitle)
      bRet = FALSE;
   DrawTextWithProfile(pMe->m_pShell,
                       pDisp,
                       RGB_WHITE,//_NO_TRANS,
                       AEE_FONT_BOLD,
                       (AECHAR *)(bRet == TRUE ? pszTitle : wszTitle),
                       -1,
                       0,
                       0,
                       &rc,
                       IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_TEXT_TRANSPARENT);
   MGAppUtil_UpdateImgViewerSoftkey(pMe);

   FREEIF(pszTitle);
   MSG_FATAL("MGAppUtil_RedrawImage End",0,0,0);
   return SUCCESS;
}

/*
 * ==========================================================================
 * FUNCTION     : MGAppUtil_ChangeMediaMenuItemByType
 * DESCRIPTION  : 从文件列表中获取下一个同种类型的文件，比如查看图片时，调用此
 * 函数获取上/下一张图片。
 * PARAMETERS   : bDirection:  TRUE-- Next item
 *                            FALE-- Previous item
 *                pfnCheck :function pointer point to function which check item
 *                attribute, if it is not NULL.
 * RETURN VALUE :
 * ==========================================================================
 */
static boolean MGAppUtil_ChangeMediaMenuItemByType(CMediaGalleryApp *pMe,
                                                   IMenuCtl *pMenuCtl,
                                                   boolean bDirection,
                                                   MG_PFN_ITEMTYPECHECK pfnCheck)
{
   uint16 *pSelItemID;
   uint16 nNextSelItemID;
   uint16 nIdx, nCurIdx;
   uint16 nSize;
   CtlAddItem  ai;
   MGFileInfo *pItemInfo;
   MSG_FATAL("MGAppUtil_ChangeMediaMenuItemByType Start",0,0,0);
   nCurIdx = 0;

   if(!pMe || !pMenuCtl){
      MSG_FATAL("Parameter error!",0,0,0);
      return FALSE;
   }

   pSelItemID = pMe->m_Explorer.m_nPreItemID +
               (pMe->m_Explorer.m_nCurDepth - 1);

   nSize = IMENUCTL_GetItemCount(pMenuCtl);
   MSG_FATAL("Size:%d", nSize,0,0);
   if(nSize <= 1){
      MSG_FATAL("There only 1 or even 0 item, Size:%d", nSize,0,0);
      return FALSE;
   }

   for(nIdx = 0; nIdx < nSize; nIdx++){
      if(*pSelItemID == IMENUCTL_GetItemID(pMenuCtl, nIdx)) {
         nCurIdx = nIdx;
         break;
      }
   }

   if(nCurIdx != nIdx || nIdx == nSize) {
      MSG_FATAL("No item match",0,0,0);
      return FALSE;
   }

   if( TRUE == bDirection )
   {
      MSG_FATAL("TRUE == bDirection",0,0,0);
      nIdx = (nIdx + 1) % nSize;
      for(; nIdx - nCurIdx < nSize;) {
         nNextSelItemID = IMENUCTL_GetItemID(pMenuCtl, nIdx);
         nIdx = (nIdx + 1) % nSize;

         /*check the item id whether exist*/
         if(FALSE ==IMENUCTL_GetItem(pMenuCtl, nNextSelItemID, &ai))
         {
            MSG_FATAL("FALSE ==IMENUCTL_GetItem",0,0,0);
            return FALSE;
         }

         IMENUCTL_GetItemData(pMenuCtl, nNextSelItemID, (uint32*)&pItemInfo);

         /*check the next item*/
         if(NULL == pfnCheck || TRUE == pfnCheck(pMe, pItemInfo)) {
            MediaGalleryApp_SetCurrentNode(pMe, pItemInfo);
            *pSelItemID = nNextSelItemID;
            break;
         }
      }
   }
   else
   {
      MSG_FATAL("FALSE == bDirection",0,0,0);
      nIdx = (nIdx + nSize - 1) % nSize;
      for(;nCurIdx != nIdx;) {
         nNextSelItemID = IMENUCTL_GetItemID(pMenuCtl, nIdx);
         nIdx = (nIdx + nSize - 1) % nSize;

         /*check the item id whether exist*/
         if(FALSE ==IMENUCTL_GetItem(pMenuCtl, nNextSelItemID, &ai)) 
         {
            MSG_FATAL("FALSE ==IMENUCTL_GetItem",0,0,0);
            return FALSE;
         }

         IMENUCTL_GetItemData(pMenuCtl, nNextSelItemID, (uint32*)&pItemInfo);

         /*check the next image*/
         if(NULL == pfnCheck || TRUE == pfnCheck(pMe, pItemInfo))
         {
            MediaGalleryApp_SetCurrentNode(pMe, pItemInfo);
            *pSelItemID = nNextSelItemID;
            break;
         }
      }
   }
   //MGAppUtil_GetMediaMenuSelectItem(pMe, pMenuCtl);
   MSG_FATAL("MGAppUtil_ChangeMediaMenuItemByType End",0,0,0);
   return TRUE;
}//MGAppUtil_ChangeMediaMenuItemByType

/*===========================================================================
 * FUNCTION:    MGAppUtil_LoadImageNotify
 *
 * DESCRIPTION: 用于接收加载图片通知的回调函数。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static void MGAppUtil_LoadImageNotify(void *pUser,
                                    IImage *po,
                                    AEEImageInfo *pi,
                                    int nErr)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)pUser;
   uint16 nTextResId = IDS_MG_FILEOPERR;

   if(!pMe || !po || !pi)
   {
      MG_FARF(ADDR, ("LoadImageNotify bad parameter!"));
      return;
   }

   pMe->m_bImgLoadDone = TRUE;

   if(SUCCESS != nErr)
   {
      if(pMe->m_PopupOps == MG_OP_WALLPAPER)
      {
         RELEASEIF(pMe->m_pImage);
         nTextResId = IDS_MG_WALLPAPERNOSET;
      }

      MG_FARF(ADDR, ("Load Image occur error!"));
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       nTextResId,
                                       MESSAGE_ERR,
                                       BTBAR_BACK);
      return;
   }

   MG_FARF(ADDR, ("PopupOps is %d, ImgViewOps is %d",
                  pMe->m_PopupOps, pMe->m_ImgViewOps));

   if(pMe->m_PopupOps == MG_OP_VIEWIMG &&
      pMe->m_ImgViewOps != MG_OP_WALLPAPER
#ifdef FEATURE_USES_MMS      
      && pMe->m_ImgViewOps != MG_OP_FORMMS_IMAGE
#endif      
      )
   {
      MGAppUtil_RedrawImage(pMe, pi, FALSE);
   }
   else if (pMe->m_PopupOps == MG_OP_WALLPAPER ||
            pMe->m_ImgViewOps == MG_OP_WALLPAPER)
   {
   	  MSG_FATAL("pMe->MGAppUtil_SetWallpaper//",0,0,0);
      MGAppUtil_SetWallpaper(pMe, pi);
   }
}//MGAppUtil_LoadImageNotify


/*===========================================================================
 * FUNCTION:    MGAppUtil_LoadMediaNotify
 *
 * DESCRIPTION:  用于接收加载多媒体文件（MP3等音乐文件）通知的回调函数。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
/*call back function for IMedia*/
static void MGAppUtil_LoadMediaNotify(void * pUser,
                                 AEEMediaCmdNotify * pCmdNotify)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)pUser;

   if(!pMe || !pCmdNotify)
   {
      MG_FARF(ADDR,("WARN: MediaNotify pMe or pCmdNotify is NULL!"));
      return;
   }

   switch(pCmdNotify->nStatus)
   {
   case MM_STATUS_SEEK_FAIL:
   case MM_STATUS_PAUSE_FAIL:
   case MM_STATUS_RESUME_FAIL:
      MG_FARF(ADDR, ("seek, pause or resume failed!"));
      return;

   default:
      break;
   }

   switch(pCmdNotify->nCmd)
   {
   case MM_CMD_PLAY://after IMedia_RegisterNotify, call IMedia_Play
      switch (pCmdNotify->nStatus)
      {
      case MM_STATUS_START:
      case MM_STATUS_RESUME:
         MediaGallery_SetPlayMusicStatus(TRUE);
         break;

      case MM_STATUS_ABORT:
      case MM_STATUS_PAUSE:
      case MM_STATUS_DONE:
         MediaGallery_SetPlayMusicStatus(FALSE);
         break;
      }
      break;

   case MM_CMD_RECORD:  //after IMedia_RegisterNotify, call IMedia_Record
   case MM_CMD_SETMEDIAPARM://after IMedia_RegisterNotify, call IMedia_SetMediaParm
      break;

   case MM_CMD_GETMEDIAPARM://after IMedia_RegisterNotify, call IMedia_GetMediaParm
      {
         switch (pCmdNotify->nStatus)
         {
         case MM_STATUS_MEDIA_SPEC:
            {
               AEECLSID ClsID;
               int nRet;

               nRet = IMEDIA_GetClassID(pMe->m_pMedia, &ClsID);
               if(nRet == SUCCESS && ClsID == AEECLSID_MEDIAMP3)
               {
                  AEEMediaMP3Spec* pSpec;
                  
                  FREEIF(pMe->m_pMediaSpec);
                  pMe->m_pMediaSpec = (void *)MALLOC(sizeof(AEEMediaMP3Spec));

                  if(!pMe->m_pMediaSpec)
                  {
                     MG_FARF(ADDR, ("malloc failed!!!"));
                     return;
                  }

                  pSpec = (AEEMediaMP3Spec *)pCmdNotify->pCmdData;
                  MG_FARF(ADDR, ("title:%s", pSpec->szTitle));
                  MG_FARF(ADDR, ("year:%s",  pSpec->szYear));
                  MG_FARF(ADDR, ("sample rate:%d", pSpec->dwSampleRate));

                  MEMMOVE(pMe->m_pMediaSpec, pSpec, sizeof(AEEMediaMP3Spec));
                  pMe->m_bGotMediaInfo = TRUE;
               }
            }
            break;

         case MM_STATUS_FRAME:
            {
               int nStatus;
               IBitmap *pFrame = NULL;
               AEEBitmapInfo bi;
               uint16 x, y;
               uint16 dx, dy;

               /*IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame)after
                * you're done with pFrame.*/
               RELEASEIF(pMe->m_pMedia);
               nStatus = IMEDIA_GetFrame(pMe->m_pMedia, &pFrame);

               if(nStatus != SUCCESS || pFrame == NULL)
               {
                  return;
               }

               /* Get the bitmap info...this can be saved in app global
                * structure.*/
               IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

               dx = MIN(pMe->m_rc.dx, (uint16)bi.cx);
               dy = MIN(pMe->m_rc.dy, (uint16)bi.cy);
               x = (uint16)((pMe->m_rc.dx - dx) / 2) + pMe->m_rc.x;
               y = (uint16)((pMe->m_rc.dy - dy) / 2) + pMe->m_rc.y;

               /* Display the frame at centered on the screen */
               IDISPLAY_BitBlt(pMe->m_pDisplay, x, y, dx, dy, pFrame, 0, 0,
                               AEE_RO_COPY);
               IDISPLAY_Update(pMe->m_pDisplay);
               IBITMAP_Release(pFrame);
            }
            break;

         default:
            break;
         }
      }
      break;

   case MM_CMD_GETTOTALTIME:
      {
         if(pCmdNotify->nStatus == MM_STATUS_DONE)
         {
            pMe->m_dwDuration = ((uint32)pCmdNotify->pCmdData);
         }
         break;
      }

   default:
      break;
   }
}//MGAppUtil_LoadMediaNotify

/*===========================================================================
 * FUNCTION:       MGAppUtil_GetDestFilePath
 *
 * DESCRIPTION:    根据参数，为拷贝/移动文件，产生目的文件的路径。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppUtil_GetDestFilePath(const char *cpszSrc,
                                     const char *cpszDestDir,
                                     char *pszDestPath,
                                     uint16 nDestPathLen/*byte*/)
{
   const char *cpBasename = NULL;
   uint16 nBasenameLen;
   uint16 nDestDirLen;
   uint16 nPad = 0; /* whether add '/' */

   if(!cpszSrc || !cpszDestDir || !pszDestPath)
   {
      MG_FARF(ADDR, ("Bad parameter, MGAppUtil_GetDestFilePath"));
      return EFAILED;
   }

   nDestDirLen = STRLEN(cpszDestDir);

   if (nDestDirLen == 0)
   {
      MG_FARF(ADDR, ("Destination directory length is 0"));
      return EFAILED;
   }

   if (DIRECTORY_CHAR != *(cpszDestDir + nDestDirLen - 1))
   {
      nPad = 1;
   }

   cpBasename = BASENAME(cpszSrc);
   nBasenameLen = STRLEN(cpBasename);

   if(NULL == cpBasename)
   {
      MG_FARF(ADDR, ("Can not fetch file name!"));
      return EFAILED;
   }

   if(nDestDirLen + nBasenameLen + nPad + 1 /* '\0' */ > nDestPathLen)
   {
      MG_FARF(ADDR, ("buffer for destination path in insufficent"));
      return EFAILED;
   }

   MEMSET(pszDestPath, 0, nDestPathLen);
   STRCPY(pszDestPath, cpszDestDir);

   if (nPad != 0)
   {
      STRCAT(pszDestPath, MG_DIRECTORY_STR);
   }

   STRCAT(pszDestPath, cpBasename);

   return SUCCESS;
}//MGAppUtil_GetDestFilePath

/*===========================================================================
 * FUNCTION:     MGAppUtil_InitFileCopy
 *
 * DESCRIPTION:  拷贝文件的初始化。打开源文件和目的文件的文件句柄。假如目的文
 * 件不存在，则新建文件；否则，询问用户是否要覆盖。
 *
 * PARAMETERS:
 *
 *    TRUE  - do success, or pop up message box, so client should check
 *    current media dialog status to decide how to handle next.
 *    FALSE - fail , and shoud return to normal interface
 * ==========================================================================
 */
boolean MGAppUtil_InitFileCopy(CMediaGalleryApp* pMe)
{
   boolean bOverWtirte = FALSE;
   boolean bRet = TRUE;
   const char *pSrcName = NULL;
   const char *pDestName = NULL;
   IFile *pSrcFile = NULL;
   IFile *pDestFile = NULL;
   FileInfo Info;

   if(NULL == pMe)
   {
      return FALSE;
   }

   pSrcName = pMe->m_Copy.m_pszSrcName;
   pDestName = pMe->m_Copy.m_pszDestName;

   if(NULL == pMe->m_pFileMgr)
   {
      MG_FARF(ADDR, ("Date is validate"));
      return FALSE;
   }

   MEMSET(&Info, 0, sizeof(Info));
   IFILEMGR_GetInfo(pMe->m_pFileMgr, pSrcName, &Info);

   if(Info.attrib == AEE_FA_DIR)
   {
      //TODO: directory not permit to move
      MG_FARF(ADDR, ("%s is directory.", pSrcName));
      return FALSE;
   }

   pSrcFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, pSrcName, _OFM_READ);

   if(SUCCESS == IFILEMGR_Test(pMe->m_pFileMgr, pDestName))
   {
      bOverWtirte = TRUE;
   }
   else
   {
      pDestFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, pDestName, _OFM_CREATE);
      RELEASEIF(pDestFile);
   }

   pDestFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, pDestName, _OFM_READWRITE);

   if(NULL == pDestFile || NULL == pSrcFile)
   {
      MG_FARF(ADDR, ("Create or open file failed!"));
      bRet = MGAppUtil_HandleOperateFileError(pMe, pMe->m_pFileMgr);
      RELEASEIF(pDestFile);
      RELEASEIF(pSrcFile);
      goto INITFILECOPYEXIT;
   }

   /*clear the previous copy data if have any*/
   RELEASEIF(pMe->m_Copy.m_pSrcFile);
   RELEASEIF(pMe->m_Copy.m_pDestFile);
   FREEIF(pMe->m_Copy.m_pBuffer);

   pMe->m_Copy.m_pSrcFile = pSrcFile;
   pMe->m_Copy.m_pDestFile = pDestFile;

#if 0
   if(Info.dwSize > MG_FILE_MIDDLETHRESHOLD)
   {
      uint32 dwCache;
      dwCache = IFILE_SetCacheSize(pSrcFile, SCS_BEST);
      MG_FARF(ADDR, ("source file cache size %d!", dwCache));
      dwCache = IFILE_SetCacheSize(pDestFile,dwCache);
      MG_FARF(ADDR, ("destion file cache size %d!", dwCache));
      pMe->m_Copy.m_dwBufLen = dwCache;
   }
   else
#endif
   {
      pMe->m_Copy.m_dwBufLen = MIN(MG_FS_BUFFER, Info.dwSize);
   }

   if(Info.dwSize == 0)
   {
      goto INITFILECOPYEXIT;
   }

   pMe->m_Copy.m_pBuffer = (void *)MALLOC(pMe->m_Copy.m_dwBufLen);

   if(!pMe->m_Copy.m_pBuffer)
   {
      MG_FARF(ADDR, ("MALLOC failed!"));
      bRet = FALSE;
      goto INITFILECOPYEXIT;
   }

   if(bOverWtirte == TRUE)
   {
      MediaGalleryApp_ShowYesNoBox(pMe,
                                   MGRES_LANGFILE,
                                   IDS_MG_OVERWRITE,
                                   MG_MSGID_OVERWRITE);

      MG_FARF(ADDR, ("Dest file is exist , do you want to overwrite???"));
      //we also check whether can overwrite???  if the file is open ,such as
      //music player??? check it after user confirm.
   }

INITFILECOPYEXIT:
   return bRet;
}//MGAppUtil_InitFileCopy

/*===========================================================================
 * FUNCTION:      MGAppUtil_DoCopyFile
 *
 * DESCRIPTION:   拷贝文件，从源文件把一定数据读到buffer，再把buffer中的数据写
 * 到目的文件。
 *
 * PARAMETERS:
 *
 *    EFILEEOF  - finish copy whole file
 *    SUCCESS - copy a block of file success
 *    EFAILED - fail.
 * ==========================================================================
 */
static int MGAppUtil_DoCopyFile(CMediaGalleryApp *pMe)
{
   uint32 dwReadCount;
   uint32 dwWriteCount;
   uint32 dwDstFileSize;
   uint32 dwSrcFileSize;
   int nRet = EFILEEOF;
   IFile *pDestFile = NULL;
   IFile *pSrcFile = NULL;
   void *pBuffer = NULL;
   uint32 dwBufLen;
   FileInfo Info;

   if(NULL == pMe)
   {
      MG_FARF(ADDR, ("Bad parameter"));
      return EFAILED;
   }

   pDestFile = pMe->m_Copy.m_pDestFile;
   pSrcFile = pMe->m_Copy.m_pSrcFile;
   pBuffer = pMe->m_Copy.m_pBuffer;
   dwBufLen = pMe->m_Copy.m_dwBufLen;

   if(NULL == pDestFile || NULL == pSrcFile )
   {
      return EFAILED;
   }

   IFILE_GetInfo(pDestFile, &Info);
   dwDstFileSize =  Info.dwSize;
   IFILE_GetInfo(pSrcFile, &Info);
   dwSrcFileSize = Info.dwSize;

   if(Info.dwSize == 0)
   {
      return nRet;
   }
   else if(NULL == pBuffer)
   {
      return EFAILED;
   }

   do{
      dwReadCount = IFILE_Read(pSrcFile, pBuffer, dwBufLen);

      /*the last block read may less than the buffer*/
      if((0 == dwReadCount) && (dwSrcFileSize != dwDstFileSize))
      {
         MG_FARF(ADDR, ("Read error!"));
         nRet = EFAILED;
         goto DOCOPYEXIT;
      }

      dwWriteCount = IFILE_Write(pDestFile, pBuffer, dwReadCount);

      if(0 == dwWriteCount)
      {
#ifdef FEATURE_MG_LONGPATH
         AEEFileInfoEx Info;
         MGFileInfo sMGFile;

         MEMSET(&Info, 0, sizeof(AEEFileInfoEx));
         MEMSET(&sMGFile, 0, sizeof(MGFileInfo));

         Info.nStructSize = sizeof(Info);
         Info.nMaxFile = sizeof(sMGFile.szName);
         Info.pszFile = sMGFile.szName;
         IFILE_GetInfoEx(pDestFile, &Info);


         if(SUCCESS != IFILEMGR_CheckPathAccess(pMe->m_pFileMgr,
                                                sMGFile.szName,
                                                AEEFP_WRITE,
                                                NULL))
#else
         FileInfo Info;

         IFILE_GetInfo(pDestFile, &Info);

         if(SUCCESS != IFILEMGR_CheckPathAccess(pMe->m_pFileMgr,
                                                Info.szName,
                                                AEEFP_WRITE,
                                                NULL))
#endif
         {
            MG_FARF(ADDR, ("Write access check failed!"));
         }
         MG_FARF(ADDR, ("Write error!"));
         nRet = EFAILED;
         goto DOCOPYEXIT;
      }
      else if(dwReadCount != dwWriteCount)
      {
         MG_FARF(ADDR, ("file write opreation is not complete!"));
         //TODO: rollback , it need indeed?
         nRet = EFAILED;
         break;
      }

      MG_FARF(ADDR, ("DO COPYING"));
      dwDstFileSize += dwWriteCount;

   } while(dwSrcFileSize != dwDstFileSize);

DOCOPYEXIT:
   return nRet;
}//MGAppUtil_DoCopyFile


/*===========================================================================
 * FUNCTION:     MGAppUtil_HandleOperateFileError
 *
 * DESCRIPTION:  处理文件操作时的错误。
 *
 * PARAMETERS:
 *
 * ==========================================================================
 */
static int MGAppUtil_HandleOperateFileError(CMediaGalleryApp* pMe,
                                             IFileMgr *pFileMgr)
{
   uint16  nTextResId;
   int nError = SUCCESS;

   if(!pMe || !pFileMgr)
      return FALSE;

   nError = IFILEMGR_GetLastError(pFileMgr);

   /*ERROR Handle Reference to function: MP_ErrorStateToString*/
   switch(nError)
   {
      case EFILENOEXISTS:
         nTextResId = IDS_MG_FILENOEXISTS;
         break;

      case EFILEOPEN:
         nTextResId = IDS_MG_FILEOPENED;
         break;

      case EBADFILENAME:
         nTextResId = IDS_MG_INVALIDNAME;
         break;

      case EDIRNOEXISTS:
         nTextResId = IDS_MG_DIRNOEXISTS;
         break;

      case EDIRNOTEMPTY:
         nTextResId = IDS_MG_FOLDERNOTEMPTY;
         break;

      case EINVALIDOPERATION:/*We may check whether the file is read-only*/
      case ENOMEDIA:
      case EFILEEOF:
      case EOUTOFNODES:
      default:
         nTextResId = IDS_MG_FILEOPERR;
         break;
   }

   MediaGalleryApp_ShowPromptMsgBox(pMe,
                                    nTextResId,
                                    MESSAGE_ERR,
                                    BTBAR_BACK);

   return TRUE;
}//MGAppUtil_HandleOperateFileError


/*
 * ==========================================================================
 * FUNCTION     :  MGAppUtil_StartUDisk
 * DESCRIPTION  :  启动U盘功能。
 * PARAMETERS   :
 * RETURN VALUE :
 * ==========================================================================
 */
static void MGAppUtil_StartUDisk(void *po)
{
   CMediaGalleryApp *pMe = (CMediaGalleryApp *)po;
   boolean bRet = FALSE;

   if(!pMe)
   {
      MG_FARF(ADDR, ("MGAppUtil_StartUDisk bad parameter!!!"));
      return;
   }
   MSG_FATAL("MGAppUtil_StartUDisk---------1",0,0,0);
   bRet = MediaGallery_StartUDisk(pMe);

   if(FALSE == bRet)
   {
      if(FALSE == pMe->m_bCardExist)
      {
         MGCLOSE_DIALOG(MGDLGRET_NOEXTCARD);
      }
      else if(FALSE == MediaGallery_CheckUSBCableConnect())
      {
         /*no usb cable connect*/
         MGCLOSE_DIALOG(MGDLGRET_NOUSBCABLE);
      }
      else
      {
         MG_FARF(STATE, ("Occur unknow error!"));
         MGCLOSE_DIALOG(MGDLGRET_CANCELED);
      }
   }

   MSG_FATAL("MGAppUtil_StartUDisk---------bRet =%d",bRet ,0,0);

}//MGAppUtil_StartUDisk

static int MGAppUtil_BuildPlaylist(CMediaGalleryApp *pMe,IMenuCtl *pMenuCtl)
{
	AEEFileInfoEx  fi;
	FileName szFileName;
	//AEEFileInfoEx  fiex;
	uint32	fitime[10];//用来保存播放列表文件的时间信息
	int i,j;
	uint32	temp;
	char fileName[128];
	CtlAddItem ai;
	
	ASSERT(pMe != NULL); 
	if(pMenuCtl == NULL||pMe->m_pFileMgr==NULL)
	{
	 return EBADPARM;
	}
	(void)IMENUCTL_DeleteAll(pMenuCtl);
	if (SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, MUSICPLAYLIST_DIR))
	{
	 (void)IFILEMGR_MkDir(pMe->m_pFileMgr, MUSICPLAYLIST_DIR);
	}

	//枚举playlist文件夹	
	(void)IFILEMGR_EnumInit(pMe->m_pFileMgr, MUSICPLAYLIST_DIR, FALSE);
	MEMSET(&ai,0,sizeof(ai));
	if(pMe->m_pBuffer)
	{
	 FREEIF(pMe->m_pBuffer);
	}
	pMe->m_pBuffer = (AECHAR*)MALLOC(128 * sizeof(AECHAR)); 

	if(NULL == pMe->m_pBuffer)
	{
	 return ENOMEMORY;
	}

	pMe->m_nPlaylistNum=0;
	for(i=0;i<10;i++)
	{
		MEMSET(pMe->m_Playlist[i].pPlaylistName, 0,128*sizeof(char));
	}
	// 填充fi结构体
   MEMSET(&fi, 0, sizeof(fi));
   fi.nStructSize = sizeof(fi);
   fi.pszFile = szFileName.m_szFileName;
   fi.nMaxFile = sizeof(szFileName.m_szFileName);	
	//scan playlist文件夹下每一个文件并添加IMENUCTL项  
   while (IFILEMGR_EnumNextEx(pMe->m_pFileMgr, &fi))
   {
		char *			  psz = NULL;
		char *			  pf = NULL;
		//由于IFILEMGR_EnumNext函数在处理时并没有给dwCreationDate赋值所以要重新获取一遍
		//IFILEMGR_GetInfoEx(pMe->m_pFileMgr, fi.pszFile, &fi);

		fitime[pMe->m_nPlaylistNum]= fi.dwCreationDate;
		psz = STRRCHR(fi.pszFile, '.');

		if (NULL == psz)
		{
		  continue; 
		}
			
		pf = STRRCHR(fi.pszFile, '/');
		if (NULL == pf)
		{
		  continue;
		}  
		pf++;

		if(psz > pf)
		{
		 STRCPY(pMe->m_Playlist[pMe->m_nPlaylistNum].pPlaylistName, pf);
		 pMe->m_Playlist[pMe->m_nPlaylistNum].pPlaylistName[psz-pf ] = '\0';
		} 

		pMe->m_nPlaylistNum++; 
		if(pMe->m_nPlaylistNum >= 10)
		{
			break;
		}
   }
   // if(pMe->m_nPlaylistNum==0)
	//{
	//	 IMENUCTL_AddItem(pMenuCtl, MUSICPLAYER_RES_FILE_LANG, IDS_PLAYLIST_NULL, IDS_PLAYLIST_NULL, NULL, 0);
	   //IMENUCTL_SetActive(pMenuCtl, FALSE);
	//}
	//else //由于文件枚举的时候是根据文件名来排序枚举的所以在这里根据文件创建时间来排序
	if(pMe->m_nPlaylistNum > 0)
	{
	 for(i=0;i<pMe->m_nPlaylistNum;i++)
	 { 
		for(j=i+1;j<pMe->m_nPlaylistNum;j++)
		{
		 if(fitime[i]>fitime[j])
		 {
		   temp=fitime[i];
		   fitime[i]=fitime[j];
		   fitime[j]=temp;
		  (void)STRCPY(fileName,pMe->m_Playlist[i].pPlaylistName);
		  (void)STRCPY(pMe->m_Playlist[i].pPlaylistName,pMe->m_Playlist[j].pPlaylistName);
		  (void)STRCPY(pMe->m_Playlist[j].pPlaylistName,fileName);
		 }
		}
	 }
	 for(i=0;i<pMe->m_nPlaylistNum;i++)
	 {	  
		ai.pszResImage =AEE_APPSCOMMONRES_IMAGESFILE;
		ai.wImage = IDI_PLAYLISTMG;
	   
	   if(STRCMP(DEFAULT_PLAYLIST,pMe->m_Playlist[i].pPlaylistName)== 0)
	   {
		  (void)ISHELL_LoadResString(pMe->m_pShell,
								MGRES_LANGFILE,								  
								IDS_DEFAULTPLAYLISTNAME,
								pMe->m_pBuffer,
								128 * sizeof(AECHAR));
	   }
	   else
	   {
		   (void)UTF8TOWSTR((const byte *)pMe->m_Playlist[i].pPlaylistName,
							 STRLEN(pMe->m_Playlist[i].pPlaylistName),
							 pMe->m_pBuffer,
							 128 * sizeof(AECHAR));
	   }
	   ai.pText = pMe->m_pBuffer;
	   ai.wItemID = i;
		if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
		{
		 return EFAILED;
		}
	   MEMSET(pMe->m_pBuffer,0,128 * sizeof(AECHAR));
	}
   }
	FREEIF(pMe->m_pBuffer);
	return SUCCESS;
}
/*==============================================================================
函数： MGAppUtil_WriteMusiclist

说明：
       用来写入指定播放列表里面的所有音乐名称

参数：
      pMe:MGAppUtil *类型指针
      curfileName:不包含路径和后缀的文件名

返回值：
    SUCCESS:读取成功
    EFAILED:读取失败

备注：:
        无
        
==============================================================================*/
int MGAppUtil_WriteMusiclist(CMediaGalleryApp *pMe , char *curFileName)
{
    IFile  *pFile;
    char fileName[256];
    if(pMe==NULL||fileName==NULL)
    {
    	return EFAILED;
    }
    (void)STRCPY(fileName,MUSICPLAYLIST_DIR);
    (void)STRCAT(fileName,"/");
    (void)STRCAT(fileName,curFileName);
    (void)STRCAT(fileName,".txt");

    pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, fileName, _OFM_READWRITE);
    if(pFile == NULL)
    {
        ERR("FILE IS NULL",0,0,0);
        // 尚未创建，使用默认值
        return EFAILED;
    } //else 打开文件继续执行

    // 从文件开头写入数据
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
        (void)IFILE_Release(pFile);
        pFile = NULL;
        return EFAILED;
    }
    (void) IFILE_Write(pFile, pMe->m_Musiclist, sizeof(pMe->m_Musiclist));
    (void)IFILE_Release(pFile);
    pFile = NULL;
    return SUCCESS;
}
/*==============================================================================
函数： MGAppUtil_ReadMusiclist

说明：
      用来读取指定播放列表里面的所有音乐名称

参数：
      pMe:MGAppUtil *类型指针
      curfileName:不包含路径和后缀的文件名
      isPlaying:TRUE:把从文件中读取的数据放到pMe->m_PlayingMusiclist中
                FALSE:把从文件中读取的数据存放到pMe->m_Musiclist中

返回值：
    SUCCESS:读取成功
    EFAILED:读取失败

备注：:
        无
        
==============================================================================*/
static	int MGAppUtil_ReadMusiclist(CMediaGalleryApp *pMe , char *curFileName,boolean isPlaying)
{
    IFile  *pFile;
    int i = 0;
    char fileName[256];
    if(pMe==NULL||curFileName==NULL)
    {
    	MSG_FATAL("curFileName IS NULL",0,0,0);
     	return EFAILED;
    }
    (void)STRCPY(fileName,MUSICPLAYLIST_DIR);
    (void)STRCAT(fileName,"/");
    (void)STRCAT(fileName,curFileName);
    (void)STRCAT(fileName,".txt");
    pFile = IFILEMGR_OpenFile(pMe->m_pFileMgr, fileName, _OFM_READ);
    if(pFile == NULL)
    {
        MSG_FATAL("FILE IS NULL",0,0,0);
        // 尚未创建，使用默认值
        return EFAILED;
    } //else 打开文件继续执行

    // 从文件开始读取数据
    if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
    {
    	MSG_FATAL("IFILE_Read...FAILED...2222222",0,0,0);
        (void)IFILE_Release(pFile);
        pFile = NULL;
        return EFAILED;
    }
    i = IFILE_Read(pFile, pMe->m_Musiclist, sizeof(pMe->m_Musiclist));
    MSG_FATAL("i==================%d",i,0,0);
    if(i==0)
    {
      MSG_FATAL("IFILE_Read...FAILED...333333",0,0,0);
      pMe->m_nPlaylistMusicNum = 0;
      (void)IFILE_Release(pFile);
      pFile = NULL;
      return SUCCESS;
    }
    else
    {
    	for(i=0;i<50;i++)
    	{
        	if(pMe->m_Musiclist[i].pMusicName[0]!='\0')
        	{  
         		//   IFILEMGR_GetInfo(pMe->m_pFileMgr,pMe->m_Musiclist[i].pMusicName +1, &fi);
         		//   fitime[pMe->m_nPlaylistMusicNum]=fi.dwCreationDate;
          		//  fisize[pMe->m_nPlaylistMusicNum]=fi.dwSize;
            	pMe->m_nPlaylistMusicNum++;
        	}
    	}
    	MSG_FATAL("pMe->m_nPlaylistMusicNum====%d",pMe->m_nPlaylistMusicNum,0,0);
    }
   
    (void)IFILE_Release(pFile);
    pFile = NULL;
    return SUCCESS;
}


#ifdef FEATURE_USES_MMS
static boolean MGAppPopupMenu_OnSetMMSImage(CMediaGalleryApp *pMe,
                                             MGFileInfo *pItemData)
{
   MGFileInfo *pSelData = pItemData;
   IConfig *pConfig = NULL;
   MSG_FATAL("MGAppPopupMenu_OnSetMMSImage Start",0,0,0);
   if(!pMe)
      return FALSE;

   if(SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                       AEECLSID_CONFIG,
                                       (void **)&pConfig))
   {
      MSG_FATAL("Create config interface failed",0,0,0);
      return FALSE;
   }

   if(pSelData->dwSize >= 300*1024)
   {
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_LARGEFILE,
                                       MESSAGE_ERR,
                                       BTBAR_BACK);
      RELEASEIF(pConfig);
      return FALSE;
   }

   if(pMe->m_PopupOps != MG_OP_VIEWIMG)
   {
      if(NULL == pSelData)
      {
         pSelData = MediaGalleryApp_GetCurrentNode(pMe);
      }

      if(!pSelData || !pSelData->szName)
      {
         MSG_FATAL("Data is invalidate",0,0,0);
         RELEASEIF(pConfig);
         return FALSE;
      }
      
      // Check the image size, too small or to large are not permit. And the
      //animate image also do not permit
      RELEASEIF(pMe->m_pImage);
      pMe->m_pImage = ISHELL_LoadImage(pMe->m_pShell, pSelData->szName);
      if(pMe->m_pImage)
      {
          IIMAGE_GetInfo(pMe->m_pImage, &pMe->m_ImgInfo);
      }
   }
   else if(pMe->m_ImgViewOps != MG_OP_FORMMS_IMAGE)
   {
      RELEASEIF(pConfig); 
      return FALSE;
   }

   MSG_FATAL("PopupOps is %d, ImgViewOps is %d", pMe->m_PopupOps, pMe->m_ImgViewOps,0);
   if(pMe->m_pImage)
   {
      /*
       * if call IIMAGE_Notify for png, bmp, it will be async, if for it
       * is sync. So we must set status before call IIMAGE_Notify */
       DBGPRINTF("pSelData->szName=%s", pSelData->szName);
       pMe->m_bImgLoadDone = FALSE;
       ICONFIG_SetItem(pConfig, CFGI_MMSIMAGE,
                       pSelData->szName, sizeof(pSelData->szName));      
    //  IIMAGE_Notify(pMe->m_pImage, MGAppUtil_LoadImageNotify, pMe);
   }
   else
   {
      /* 由于ISHELL_LoadImage只用文件名的扩展名来判断是否是图片，如果不是，
       * 就不LoadImage。例如：对于长文件名，把扩展名截掉了，我们用
       * ISHELL_LoadImage就不成功。*/
      MediaGalleryApp_ShowPromptMsgBox(pMe,
                                       IDS_MG_LOADFAILED,
                                       MESSAGE_ERR,
                                       BTBAR_BACK);

      MSG_FATAL("IImage interface is 0x%x", pMe->m_pImage,0,0);
      RELEASEIF(pConfig);
      return FALSE;
   }
   RELEASEIF(pConfig);
   return TRUE;
}//MGAppPopupMenu_OnSetWallpaper
#endif
