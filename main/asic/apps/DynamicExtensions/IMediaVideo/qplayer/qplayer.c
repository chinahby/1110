/*===========================================================================

FILE: QPlayer.c

SERVICES: Sample media player applet showing usage of BREW IMediaVideo interfaces.

DESCRIPTION
  This file contains the implementation of a QPlayer applet using the
  AEE IMediaVideo interface services.

  QPlayer app
  (1) Plays video and MIDI, MP3, QCP and PMD media formats
  (3) Displays BMP, BCI, PNG and JPEG image formats
  (4) Provides playback controls: stop, rewind, ff, pause, resume, zoom, rotation
  (5) Allows image panning for images
  (6) Provides volume controls
  (7) Can display images/video in Full Screen mode
  (8) Can be started either in stand-alone mode or as a plugin

  Demonstrates usage of
  (1) IMedia API
  (2) ImediaVideo API
  (3) AEEMediaUtil_CreateMedia()
  (4) Customize AEEMediaUtil_FindClass to search MediaVideo based MIME types
  (5) IImageCtl

        Copyright ©2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEAppGen.h"        // AEEApplet structure and Applet services
#include "qplayer.bid"
#include "qplayer.brh"

// AEE Services used by app
#include "AEEStdLib.h"        // AEE Stb Lib Services
#include "AEEMenu.h"          // AEE Menu Services
#include "AEEIMediaVideo.h"   // AEE video services
#include "AEEMediaVideo.bid"
#include "AEEImageCtl.h"      // AEE ImageCtl Services
#include "AEEMimeTypes.h"     // AEE MIME definitions
#include "AEEMediaUtil.h"     // AEE Media Utility functions
#include "AEESource.h"        // ISource and IPeek definitions
#include "AEEFile.h"          // IFile, IFileMgr




/*===========================================================================
                      INTERFACE DEFINITION
===========================================================================*/
//
// This is a generic IWindow interface.
//
typedef struct _IWindow  IWindow;
QINTERFACE(IWindow)
{
   // Enables/Disables the window. Window controls will not process
   // events if the window is disabled.
   void     (*Enable)(IWindow * po, boolean bEnable);

   // Redraws the window if enabled
   void     (*Redraw)(IWindow * po);

   // Handles the events routed to the window
   boolean  (*HandleEvent)(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);

   // Releases the window resources
   void     (*Delete)(IWindow * po);
};

#define IWINDOW_Enable(p)                 GET_PVTBL(p, IWindow)->Enable(p, TRUE)
#define IWINDOW_Disable(p)                GET_PVTBL(p, IWindow)->Enable(p, FALSE)
#define IWINDOW_Redraw(p)                 GET_PVTBL(p, IWindow)->Redraw(p)
#define IWINDOW_HandleEvent(p, e, w, dw)  GET_PVTBL(p, IWindow)->HandleEvent(p, e, w, dw)
#define IWINDOW_Delete(p)                 GET_PVTBL(p, IWindow)->Delete(p)

/*===============================================================================
                     MACROS
=============================================================================== */
#define QP_IWINDOW_SETVTBL(pVtbl, pfnEn, pfnRd, pfnHE, pfnDel) \
   (pVtbl)->Enable      = (pfnEn); \
   (pVtbl)->Redraw      = (pfnRd); \
   (pVtbl)->HandleEvent = (pfnHE); \
   (pVtbl)->Delete      = (pfnDel)

#define QP_RELEASEIF(p)                QP_FreeIF((IBase **)&(p))
#define QP_RELEASEWIN(p)               QP_FreeWin((IWindow **)&(p))
#define QP_PEEK_RETURN(p, lCode )      { QP_RELEASEIF(p); return (lCode); }
#define QP_WINERR_RETURN(p)            QP_PEEK_RETURN(p, NULL)


#define QP_ISPLUGIN(p)                 ((p)->m_pOwner->m_bPlugin)

#define QP_DRAWHEADER(pme)             QP_DrawImage((pme)->m_pOwner->m_pHdrImage, &(pme)->m_pOwner->m_rectHdr, TRUE)

#define QP_ISEVTKEY(e)                 ((e) == EVT_KEY)
#define QP_ISCLR(e)                    (QP_ISEVTKEY(e) && wParam == AVK_CLR)
#define QP_ISEVTCMD(e)                 ((e) == EVT_COMMAND)
#define QP_ISCMD(e, c)                 (QP_ISEVTCMD(e) && (c) == wParam)
#define QP_ISFULLSCRN(p)               ((p)->m_bImage && (p)->m_bFullScreen)

#define CQPlayer_CancelRedraw(p)       { CALLBACK_Cancel(&(p)->m_cbRedraw); (p)->m_bRedraw = FALSE; }
#define CQPlayer_DisableWin(p)         { IWINDOW_Disable((p)->m_pWin); CQPlayer_CancelRedraw(p); }

#define CPlayerWin_IsImage(p)          ((p)->m_eWin == MPPW_IMAGE)
#define CPlayerWin_IsMedia(p)          ((p)->m_eWin == MPPW_PLAY)

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/
#define QP_SPLASH_TIMER       750

#define QP_MAX_STRLEN         64
#define QP_MAX_FILES          64
#define QP_MEDIA_DIR          "media"
#define QP_EXT_SEPARATOR      ","

#define QP_VOLUME_STEP        25
#define QP_SEEK_TIME          10000    // 10 seconds
#define QP_PROG_TIMER         2000

#define QP_HEADER_CY          16
#define QP_ICONVIEWCTL_CY     20

#define QP_PROGBAR_DX_DIV     3     // dx is dx/3 of main rectangle
#define QP_PROGBAR_DY_DIV     2     // dy is dy/2 of main rectangle
#define QP_PROGBAR_DX_OFFSET  4     // dx is reduced by offset
#define QP_CLR_PROG_FILL      MAKE_RGB(0, 128, 192)
#define QP_ZOOM_STEP          10
#define QP_ROTATE_STEP        10


#define QP_MAX_FILE_NAME      256

#define EVT_CREATEMEDIA       EVT_USER

// Based on Menu style sheet:
#define MENU8_FT                 AEE_FT_NONE
#define MENU8_SELECT_FT          AEE_FT_RAISED
#define MENU8_RO                 AEE_RO_TRANSPARENT
#define MENU8_SELECT_RO          AEE_RO_TRANSPARENT
#define MENU8_COLOR_MASK         (MC_BACK | MC_SEL_BACK | MC_SEL_TEXT)
#define MENU8_BACKGROUND        MAKE_RGB(255,255,204)
#define MENU8_SELECT_BACKGROUND  MAKE_RGB(153, 204, 204)
#define MENU8_SELECT_TEXT        RGB_BLACK

#define TB8_BACKGROUND           MAKE_RGB(192,192,192)
#define TB8_SELECT_BACKGROUND     MAKE_RGB(192, 192, 192)

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/
typedef enum MPWindow
{
   MPW_NONE,
   MPW_MAIN,
   MPW_FILELIST,
   MPW_PLAYER,
   MPW_LAST
} MPWindow;

typedef enum MPPlayerWin
{
   MPPW_PLAY,
   MPPW_IMAGE
} MPPlayerWin;

typedef struct CQPlayer       CQPlayer;
typedef struct CWindow        CWindow;
typedef struct CMainWin       CMainWin;
typedef struct CFileListWin   CFileListWin;
typedef struct CPlayerWin     CPlayerWin;
typedef struct CProgCtl       CProgCtl;

#define INHERIT_CWindow(iname) \
   DECLARE_VTBL(iname) \
   CQPlayer *     m_pOwner; \
   IShell *       m_pIShell; \
   IDisplay *     m_pIDisplay; \
   boolean        m_bActive

// Base class of all IWindow objects.
struct CWindow
{
   INHERIT_CWindow(IWindow);
};

// Main window: Displays main menu.
struct CMainWin
{
   INHERIT_CWindow(IWindow);

   IImage *       m_pLogo;
   AEERect        m_rectLogo;
   IMenuCtl *     m_pMainMenu;
   boolean        m_bAbout;
};

// File list window: Lists files in media dir for selection.
struct CFileListWin
{
   INHERIT_CWindow(IWindow);

   IMenuCtl *     m_pFileMenu;
   char *         m_szFileArray[QP_MAX_FILES];
   int            m_NumFiles; // Number of files read into m_szFileArray
};

// Progress control: includes progress bar and the title
struct CProgCtl
{
   IShell *          m_pIShell;
   IDisplay *        m_pIDisplay;
   IStatic *         m_pTitle;
   AEERect           m_rectMain;
   AEERect           m_rectBar;
};

// Player window: Plays the media.
struct CPlayerWin
{
   INHERIT_CWindow(IWindow);

   MPPlayerWin       m_eWin;           // Window type

   IStatic *         m_pTitle;         // Media title text
   AECHAR            m_szText[QP_MAX_STRLEN];

   AEERect           m_rectImage;      // Image/Video rectangle
   AEERect           m_rectImageCopy;  // Saved Image/Video rectangle when full screen is displayed

   char *            m_pszFile;        // Saved file name alloc'd by CQPlayer

   CProgCtl          m_ProgCtl;        // ProgCtl
   uint16            m_wPct;

   IMenuCtl *        m_pPlayerMenu;

   AEEMediaData      m_md;             // Media data
   IMedia*           m_pMedia;         // IMedia-based object

   IImageCtl *       m_pImageCtl;      // ImageCtl displaying the selected image
   IImage *          m_pImage;         // Image contained in ImageCtl

   uint32            m_dwTotalTime;    // Total playback time
   uint32            m_dwPlayPos;      // Current playback pos
   uint16            m_wVolume;        // Volume

   int16             m_ZoomDirection;

   boolean           m_bMediaVideo;    // video media
   boolean           m_bPlayRec;       // = TRUE, if Playback progress flag
   boolean           m_bImage;         // = TRUE, if media contains Video/Image component.
   boolean           m_bFullScreen;    // = TRUE, if Video/Image is displayed full screen
   boolean           m_bProgTimer;     // = TRUE, if prog timer is running. Regular display is not updated.
   boolean           m_bPlaypending;   // play request pending
};

//
// QPlayer app global structure.
// Note: m_bPlugin == TRUE indicates that QPlayer tries to
// play the file directly in CPlayerWin. Pressing CLR will close
// the app.
//
struct CQPlayer
{
   AEEApplet         a;

  int               m_cxWidth;
  int               m_cyHeight;
   uint16            m_nColorDepth;
   int               m_nNChSize;    // Large char size
   int               m_nLChSize;    // Normal char size

   IImage *          m_pHdrImage;
   AEERect           m_rectHdr;

   MPWindow          m_eActiveWin;  // Active window
   MPWindow          m_eSuspendWin; // Suspended window
   IWindow *         m_pWin;

   uint16            m_wMainWin;    // CurSel of CMainWin
   uint16            m_wFileListWin;// CurSel of CFileListWin

   char *            m_pszAudioExt; // Registered audio extension string: "mid, mp3, ..."
   char *            m_pszVideoExt; // Registered video extension string: "pmd, ..."
   char *            m_pszImageExt; // Registered image extension string: "bmp, png, ..."

   AEECallback       m_cbRedraw;
   boolean           m_bRedraw;   // Processing redraw

   boolean           m_bPlugin;   // = TRUE, if QPlayer is in Plugin mode.
};

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/
static boolean    CQPlayer_InitAppData(IApplet* po);
static boolean    CQPlayer_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void       CQPlayer_DrawSplash(CQPlayer * pme);
static boolean    CQPlayer_SetWindow(CQPlayer * pme, MPWindow eWin, uint32 dwParam);
static void       CQPlayer_Redraw(CQPlayer * pme, boolean bDefer);
static void       CQPlayer_RedrawNotify(CQPlayer * pme);
static void       CQPlayer_PlayFile(CQPlayer * pme, const char * pszFile);
static boolean    CQPlayer_PlayImage(CQPlayer * pme, const char * pszFile);
static uint16     CQPlayer_FindHandlerType(CQPlayer * pme, const char * pszFile);
static uint16     CQPlayer_IsExtension(CQPlayer * pme, const char * pszFile, char * pszExt, int nExtLen);

// CWindow abstract window
static IWindow *  CWindow_New(int16 nSize, CQPlayer * pOwner, VTBL(IWindow) * pvt);
static boolean    CWindow_ProcessEnable(IWindow * po, boolean bEnable);

// Main window
static IWindow *  CMainWin_New(CQPlayer * pOwner);
static void       CMainWin_Delete(IWindow * po);
static void       CMainWin_Enable(IWindow * po, boolean bEnable);
static void       CMainWin_Redraw(IWindow * po);
static boolean    CMainWin_HandleEvent(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static void       CMainWin_About(CMainWin * pme);

// File List window
static IWindow *  CFileListWin_New(CQPlayer * pOwner);
static void       CFileListWin_Delete(IWindow * po);
static void       CFileListWin_Enable(IWindow * po, boolean bEnable);
static void       CFileListWin_Redraw(IWindow * po);
static boolean    CFileListWin_HandleEvent(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// Player window
static IWindow *  CPlayerWin_New(CQPlayer * pOwner, MPPlayerWin eWin);
static void       CPlayerWin_Delete(IWindow * po);
static void       CPlayerWin_Enable(IWindow * po, boolean bEnable);
static void       CPlayerWin_Redraw(IWindow * po);
static boolean    CPlayerWin_HandleEvent(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static int        CPlayerWin_HandleCreateMedia(CPlayerWin * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);

static void       CPlayerWin_MediaNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify);
static void       CPlayerWin_ImageNotify(void * pUser, IImage * pImage, AEEImageInfo * pi, int nErr);
static void       CPlayerWin_UpdateProgCtl(CPlayerWin * pme, int nCmd, int nSubCmd, uint16 wResID);
static boolean    CPlayerWin_IsPause(CPlayerWin * pme);
static void       CPlayerWin_ProgTimerNotify(CPlayerWin * pme);
static boolean    CPlayerWin_SetMediaData(CPlayerWin * pme, AEEMediaData *pmd);
static boolean    CPlayerWin_FullScreen(CPlayerWin * pme, boolean bFull, boolean bDeferRedraw);

// ProgCtl
static boolean    CProgCtl_Init(CProgCtl * pme, AEERect * pRectMain);
static void       CProgCtl_SetPos(CProgCtl * pme, AECHAR * psz, uint16 wPct);
static void       CProgCtl_Release(CProgCtl * pme);
static void       CProgCtl_DrawHist(CProgCtl * pme, uint16 wPct);

// Helper Functions
static IMediaVideo* QP_GetVideoInterface(IMedia* pMedia);
static int        QP_FindClass(IShell * ps, AEEMediaData * pmd, AEECLSID * pCls);
static int        QP_CreateMedia(IShell * ps, AEEMediaData * pmd, IMedia ** ppm);
static void       QP_DrawImage(IImage * pImage, AEERect * pRect, boolean bCenter);
static boolean    QP_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData);
static void       QP_ErrorDlg(CQPlayer * pme, uint16 wResErrID);
static char *     QP_GetFileName(const char * psz);
static void       QP_FrameRect(IDisplay * pd, AEERect * pRect);
static void       QP_FitStaticText(IDisplay * pd, IStatic * ps, AEEFont font, AECHAR * pszText);
static void       QP_SetMenuAttr(IMenuCtl * pMenu, AEECLSID clsMenu, uint16 nColorDepth, AEERect * pRect, uint32 dwProps);
static void       QP_FreeIF(IBase ** ppif);
static void       QP_FreeWin(IWindow ** ppif);
static boolean    QP_AddExtension(char ** ppszExtList, char * psz);
static void       QP_SendMediaNotify(CPlayerWin *pme, int nCmd, int nSubCmd, int nStatus, void *pData, uint32 dwSize);
/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/

//
// QPlayer app can either be statically built into BREW or dynamically linked during run-time.
// If AEE_STATIC is defined, then QPlayer app will be a static app.
// NOTE: Static apps can be built only by OEMs or Carriers. App developers can build dynamic apps only.
//
#if defined(AEE_STATIC)

int         CQPlayer_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
static int  CQPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod);

//
// Constant Data...
//
static const AEEAppInfo    gaiQPlayerApp = {AEECLSID_QPLAYER,QPLAYER_RES_FILE,IDS_TITLE,IDB_TNAIL,IDB_TNAIL,0,0,0};

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

//===========================================================================
//
//
//
//===========================================================================
PFNMODENTRY CQPlayer_GetModInfo(IShell * ps, AEECLSID ** ppClasses, AEEAppInfo ** pApps, uint16 * pnApps,uint16 * pwMinPriv)
{
   *pApps = (AEEAppInfo *)&gaiQPlayerApp;
   *pnApps = 1;
   return((PFNMODENTRY)CQPlayer_Load);
}

//===========================================================================
//
//===========================================================================
static int CQPlayer_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,CQPlayer_CreateInstance,NULL));
}

#endif  //AEE_STATIC

/*===========================================================================

Function:  CQPlayer_InitAppData()

Description:
    This function initializes the app specific data.

Prototype:
  void CQPlayer_InitAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
  to store applet data. (CQPlayer*, in this case).

Return Value:
  TRUE: If successful
  FALSE: IF failed

Comments:  None

Side Effects: None

==============================================================================*/
boolean CQPlayer_InitAppData(IApplet* po)
{
   CQPlayer *        pme = (CQPlayer*)po;
   int               nAscent, nDescent;
   AEEDeviceInfo *   pdi;

   // Get screen pixel count
   pdi = MALLOC(sizeof(AEEDeviceInfo));
   if (!pdi)
      return FALSE;
   ISHELL_GetDeviceInfo(pme->a.m_pIShell, pdi);
   pme->m_cxWidth = pdi->cxScreen;
   pme->m_cyHeight = pdi->cyScreen;
   pme->m_nColorDepth = pdi->nColorDepth;
   FREEIF(pdi);

   IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_LARGE, &nAscent, &nDescent);
   pme->m_nLChSize = nAscent + nDescent;

   IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay, AEE_FONT_NORMAL, &nAscent, &nDescent);
   pme->m_nNChSize = nAscent + nDescent;

   pme->m_pHdrImage = ISHELL_LoadResImage(pme->a.m_pIShell, QPLAYER_RES_FILE, IDB_HEADER);
   if (!pme->m_pHdrImage)
      return FALSE;
   SETAEERECT(&pme->m_rectHdr, 0, 0, pme->m_cxWidth, QP_HEADER_CY);

   CALLBACK_Init(&pme->m_cbRedraw, (PFNNOTIFY)CQPlayer_RedrawNotify, pme);

   pme->m_pWin = CMainWin_New(pme);
   if (!pme->m_pWin)
      return FALSE;

   return TRUE;
}

/*===========================================================================
Function:  CQPlayer_FreeAppData()

Description:
  This function frees the app data. This function is registered with the
  applet framework when the applet is created (inside AEEClsCreateInstance() function).
  This function is called by the app framework when the reference count of the
  applet reaches zero. This function must free all the app data that has been
  allocated by this app. For ex: if their is data that was remembered when
  the app is suspended and resumed, those data must be freed here.

Prototype:
  void CQPlayer_FreeAppData(IApplet* po)

Parameters:
   IApplet* po: Pointer to the applet structure. This is of the type defined
  to store applet data. (CQPlayer*, in this case).

Return Value:
  None

Comments:  None

Side Effects: None
==============================================================================*/
void CQPlayer_FreeAppData(IApplet* po)
{
   CQPlayer * pme = (CQPlayer *)po;

   QP_RELEASEIF(pme->m_pHdrImage);

   CQPlayer_CancelRedraw(pme);
   QP_RELEASEWIN(pme->m_pWin);

   FREEIF(pme->m_pszAudioExt);
   FREEIF(pme->m_pszVideoExt);
   FREEIF(pme->m_pszImageExt);
}

/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
  This function is invoked while the app is being loaded. All Modules must provide this
  function. Ensure to retain the same name and parameters for this function.
  In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
  that has been provided in AEEAppGen.c.

  After invoking AEEApplet_New(), this function can do app specific initialization. In this
  example, a generic structure is provided so that app developers need not change app specific
  initialization section every time.

PROTOTYPE:
  int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
  clsID: [in]: Specifies the ClassID of the applet which is being loaded

  pIShell: [in]: Contains pointer to the IShell interface.

  pIModule: pin]: Contains pointer to the IModule interface to the current module to which
  this app belongs

  ppObj: [out]: On return, *ppApplet must point to a valid IBase object.
  If the classID  represnts an applet, then ppObj must point to a valid AEEApplet structure.Allocation
  of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the class/app creation was successful.
  EFAILED: Error occurred while creating the class/app. In this case, the app/class will
  not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int CQPlayer_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
  *ppObj = NULL;

   //
   // Here a check is done to see if the ClsID is that of QPlayer app.
   // The reason is if this module has more than one applets or classes, then this function is invoked
   // once for each applet or class. Checking here ensures that the correct IApplet or class object is
   // constructed.
   //
   if(ClsId == AEECLSID_QPLAYER)
  {
    //Create the applet
    if(AEEApplet_New(sizeof(CQPlayer), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)CQPlayer_HandleEvent,(PFNFREEAPPDATA)CQPlayer_FreeAppData))
    {
      //Initialize applet data
      if(CQPlayer_InitAppData((IApplet*)*ppObj))
      {
        //Data initialized successfully
        return(AEE_SUCCESS);
      }
      else
      {
        //Release the applet. This will free the memory allocated for the applet when
        *ppObj = NULL;
        IAPPLET_Release((IApplet*)*ppObj);
        return EFAILED;
      }

      }//AEEApplet_New

   }// ClsId == AEECLSID_QPLAYER

   return(EFAILED);
}

/*===========================================================================

FUNCTION CQPlayer_HandleEvent

DESCRIPTION
  This is the EventHandler for this app. All events to this app are handled in this
  function. All APPs must supply an Event Handler.

PROTOTYPE:
  boolean CQPlayer_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
  pi:    Pointer to the AEEApplet structure. This structure contains information specific
         to this applet. It was initialized during the AppClsCreateInstance() function.
  ecode: Specifies the Event sent to this applet
  wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/
static boolean CQPlayer_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    CQPlayer * pme = (CQPlayer *)pi;

    switch ( eCode )
    {
         case EVT_APP_START:   // Process Start event
         {
            //
            // An app can be started from app manager or from another app with arguments.
            // dwParam contains the string argument passed to the applet when it is started.
            // In case of QPlayer app, if the argument is present, then it means that
            // the applet is started as a plugin with argument specifying file name of the
            // media. In this case, the player window is directly invoked and the media
            // starts playing immediately.
            //
            AEEAppStart * pas = (AEEAppStart *)dwParam;
            if (pas->pszArgs)
            {
               pme->m_bPlugin = TRUE;
               CQPlayer_PlayFile(pme, pas->pszArgs);
            }
            else
            {
               CQPlayer_DrawSplash(pme);
            }
            return TRUE;
         }

         case EVT_APP_BROWSE_FILE:
         {
            //
            // This event is sent when ISHELL_BrowseFile() API finds
            // QPlayer app as the registered handler. Shell first
            // sends EVT_APP_START followed by this event. The dwParam
            // contains the file name.
            // Start the QPlayer app in plugin mode.
            //
            if (dwParam)
            {
               char *   psz = (char *)dwParam;
               CQPlayer_SetWindow(pme, MPW_NONE, NULL);
               pme->m_bPlugin = TRUE;
               CQPlayer_PlayFile(pme, psz);
            }
            return TRUE;
         }

         case EVT_APP_STOP:        // process STOP event
            return (TRUE);

         case EVT_APP_SUSPEND:
            //
            // If playback ia going on, then stop the playback and close the window. This ensures that
            // all the multimedia resources allocated for playback are released.
            // On resume, open file list window.
            //
            pme->m_eSuspendWin = pme->m_eActiveWin == MPW_PLAYER ? MPW_FILELIST : pme->m_eActiveWin;
            CQPlayer_SetWindow(pme, MPW_NONE, 0);
            return (TRUE);

         case EVT_APP_RESUME:
            CQPlayer_SetWindow(pme, pme->m_eSuspendWin, 0);
            return (TRUE);

         case EVT_KEY:              // Process key event
         case EVT_COMMAND:          // Process menu command event
         case EVT_CREATEMEDIA:      // Create media
         case EVT_COPYRIGHT_END:    // Copyright dialog ended
            if (pme->m_pWin)
            {
               return IWINDOW_HandleEvent(pme->m_pWin, eCode, wParam, dwParam);
            }

    }

    return FALSE;
}

/*===========================================================================
   This function draws the splash screen and brings up the main window
   after the splash timer runs out.
===========================================================================*/
static void CQPlayer_DrawSplash(CQPlayer * pme)
{
   // The following 'If statement' is entered only after the splash timer runs out...
   if (pme->m_eActiveWin == MPW_MAIN)
   {
      CQPlayer_SetWindow(pme, MPW_MAIN, 0);
      return;
   }

   // Draw the splash screen, set the timer.
   // The timer callback calls this function and redraws the main window.
   {
      IImage * pi = ISHELL_LoadResImage(pme->a.m_pIShell, QPLAYER_RES_FILE, IDB_LOGO);

      if (pi)
      {
         AEERect  rect;

         IDISPLAY_ClearScreen(pme->a.m_pIDisplay);
         SETAEERECT(&rect, 0, 0, pme->m_cxWidth, pme->m_cyHeight);
         QP_DrawImage(pi, &rect, TRUE);
         IDISPLAY_Update(pme->a.m_pIDisplay);
         QP_RELEASEIF(pi);
      }

      // Set main window as active and start the timer.
      pme->m_eActiveWin = MPW_MAIN;
      ISHELL_SetTimer(pme->a.m_pIShell, QP_SPLASH_TIMER, (PFNNOTIFY)CQPlayer_DrawSplash, pme);
   }
}

/*===========================================================================
   This function switches from one window to another:
   (1) Releases all the resources associated with the current window
   (2) Contructs the new window, if any
   (3) Enables and redraws the new window, if any
===========================================================================*/
static boolean CQPlayer_SetWindow(CQPlayer * pme, MPWindow eWin, uint32 dwParam)
{
   // If same window, then redraw and return.
   if (pme->m_pWin && pme->m_eActiveWin == eWin && eWin != MPW_NONE)
   {
      CQPlayer_Redraw(pme, TRUE);
      return TRUE;
   }

   QP_RELEASEWIN(pme->m_pWin);

   switch (eWin)
   {
      case MPW_MAIN:
         pme->m_pWin = CMainWin_New(pme);
         break;

      case MPW_FILELIST:
         pme->m_pWin = CFileListWin_New(pme);
         break;

      case MPW_PLAYER:
         pme->m_pWin = CPlayerWin_New(pme, (MPPlayerWin)dwParam);
         break;

      case MPW_NONE:
         return TRUE;
         break;

      default:
         return FALSE;
         break;
   }

   if (!pme->m_pWin)
   {
      eWin = MPW_NONE;
      return FALSE;
   }

   pme->m_eActiveWin = eWin;

   CQPlayer_Redraw(pme, TRUE);

   return TRUE;
}

/*===========================================================================
   This function schedules an aynchronous redraw if bDefer is TRUE else
   it redras immediately.
===========================================================================*/
static void CQPlayer_Redraw(CQPlayer * pme, boolean bDefer)
{
   if (pme->m_bRedraw)
      return;

   pme->m_bRedraw = TRUE;

   if (bDefer)
      ISHELL_Resume(pme->a.m_pIShell, &pme->m_cbRedraw);
   else
      CQPlayer_RedrawNotify(pme);
}

/*===========================================================================
   This function redraws the current window.
   This function is called either by IShell in response to the resume scheduled
   in CQPlayer_Redraw() or by CQPlayer_Redraw() directly.
===========================================================================*/
static void CQPlayer_RedrawNotify(CQPlayer * pme)
{
   if (!pme->m_bRedraw)
      return;

   pme->m_bRedraw = FALSE;

   if (pme->m_pWin)
   {
      IWINDOW_Enable(pme->m_pWin);
      IWINDOW_Redraw(pme->m_pWin);
   }
}

/*===========================================================================
   This function opens the player window with the specified file.
   It also tries to play the file if QPlayer is in plugin mode.

   Note: This function allocates memory for the file name and the memory
   is released by player window's destructor.
===========================================================================*/
static void CQPlayer_PlayFile(CQPlayer * pme, const char * pszFile)
{
   AEEMediaData   md;
   char *         pszBuf;

   // File name is a special case...
   // pszFile will be released by CFileListWin when CPlayerWin opens.
   // Allocate memory for file name here. It will be used and released
   // by CPlayerWin.
   pszBuf = STRDUP(pszFile);
   if (!pszBuf)
      return;

   // First check if this is image...
   if (CQPlayer_PlayImage(pme, pszBuf))
      return;

   // This is not image...it must be media.

   md.clsData = MMD_FILE_NAME;
   md.pData = (void *)pszBuf;
   md.dwSize = 0;

   if (!CQPlayer_SetWindow(pme, MPW_PLAYER, MPPW_PLAY))
   {
      FREE(pszBuf);
      return;
   }

   if (CPlayerWin_SetMediaData((CPlayerWin *)pme->m_pWin, &md))
   {
      ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_QPLAYER, EVT_CREATEMEDIA, 0, 0);
      if (pme->m_bPlugin)
        ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_QPLAYER, EVT_COMMAND, IDM_PM_PLAY, 0);
   }
}


/*===========================================================================
   This function displays the image in the player window.
===========================================================================*/
static boolean CQPlayer_PlayImage(CQPlayer * pme, const char * pszFile)
{
   IImage *       pImage = ISHELL_LoadImage(pme->a.m_pIShell, pszFile);
   CPlayerWin *   ppw;

   if (!pImage)
      return FALSE;

   if (!CQPlayer_SetWindow(pme, MPW_PLAYER, MPPW_IMAGE))
   {
      IIMAGE_Release(pImage);
      return FALSE;
   }

   ppw = (CPlayerWin *)pme->m_pWin;
   ppw->m_bImage = TRUE;      // This is an image
   ppw->m_bPlayRec = TRUE;    // It always plays
   ppw->m_pszFile = (char *)pszFile;  // Save the pointer

   if (SUCCESS != ISHELL_CreateInstance(ppw->m_pIShell, AEECLSID_IMAGECTL, (void **)&ppw->m_pImageCtl))
   {
      QP_ErrorDlg(ppw->m_pOwner, IDS_ERR_CREATEMEDIA);
   }
   else
   {
      IIMAGECTL_SetRect(ppw->m_pImageCtl, &ppw->m_rectImage);
      IIMAGECTL_SetImage(ppw->m_pImageCtl, pImage);
      ppw->m_pImage = pImage;
      IIMAGE_Notify(pImage, CPlayerWin_ImageNotify, ppw);
      CQPlayer_Redraw(pme, TRUE);
   }

   return TRUE;
}

/*===========================================================================
   This function returns the image/audio/video/default icon resource id if
   extension is present in the internal list.
===========================================================================*/
static uint16 CQPlayer_IsExtension(CQPlayer * pme, const char * pszFile, char * pszInExt, int nExtLen)
{
   char *   psz;
   uint16   wRet = 0;
   char     szExt[10];

   psz = STRRCHR(pszFile, '.');
   if (!psz)
      return 0;

   psz++;
   if (!*psz)
      return 0;

   // Form a string like "mid, "
   STRLCPY(szExt, psz, sizeof(szExt));
   STRLCAT(szExt, QP_EXT_SEPARATOR, (sizeof(szExt) - STRLEN(szExt)));

   if (pszInExt && nExtLen > (int)STRLEN(psz))
      STRLCPY(pszInExt, psz, nExtLen);

   if (pme->m_pszAudioExt && STRSTR(pme->m_pszAudioExt, szExt))
      wRet = IDB_AUDIO_ICON;
   else if (pme->m_pszVideoExt && STRSTR(pme->m_pszVideoExt, szExt))
      wRet = IDB_VIDEO_ICON;
   else if (pme->m_pszImageExt && STRSTR(pme->m_pszImageExt, szExt))
      wRet = IDB_IMAGE_ICON;

   return wRet;
}

/*===========================================================================
   This function tries to find the handler for an extension as follows:
   (1) Search the internal lists for audio, video and image extensions
   (2) If not found, then query Shell Registry for audio/video/image based handler
   (3) If found, add the extension to internal list

   Since IMediaVideo class handlers are registered using AEECLSID_MediaVideo
   as a base class ID, we also need to search MediaVideo based MIME types
===========================================================================*/
static uint16 CQPlayer_FindHandlerType(CQPlayer * pme, const char * pszFile)
{
   AEECLSID cls;
   char     szMIME[32];
   char     szExt[10] = {0,0,0,0};
   uint16   wExtType = CQPlayer_IsExtension(pme, pszFile, szExt, sizeof(szExt));

   if (wExtType)
      return wExtType;

   if (!STRCMP(szExt, ""))
      return 0;

   // Now query the Shell Registry...

   // Look for audio based handler
   STRLCPY(szMIME, AUDIO_MIME_BASE, sizeof(szMIME));
   STRLCAT(szMIME, szExt, sizeof(szMIME) - STRLEN(szMIME));
   cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
   if (cls && QP_AddExtension(&pme->m_pszAudioExt, szExt))
      return IDB_AUDIO_ICON;

   // Look for mediavideo based handler
   STRLCPY(szMIME, VIDEO_MIME_BASE, sizeof(szMIME));
   STRLCAT(szMIME, szExt, sizeof(szMIME) - STRLEN(szMIME));
   cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MediaVideo, szMIME);
   if (cls && QP_AddExtension(&pme->m_pszVideoExt, szExt))
      return IDB_VIDEO_ICON;

   // Look for default video based handler
   STRLCPY(szMIME, VIDEO_MIME_BASE, sizeof(szMIME));
   STRLCAT(szMIME, szExt, sizeof(szMIME) - STRLEN(szMIME));
   cls = ISHELL_GetHandler(pme->a.m_pIShell, AEECLSID_MEDIA, szMIME);
   if (cls && QP_AddExtension(&pme->m_pszVideoExt, szExt))
      return IDB_VIDEO_ICON;

   // Look for image based handler
   STRLCPY(szMIME, IMAGE_MIME_BASE, sizeof(szMIME));
   STRLCAT(szMIME, szExt, sizeof(szMIME) - STRLEN(szMIME));
   cls = ISHELL_GetHandler(pme->a.m_pIShell, HTYPE_VIEWER, szMIME);
   if (cls && QP_AddExtension(&pme->m_pszImageExt, szExt))
      return IDB_IMAGE_ICON;

   return 0;
}

/*===============================================================================

                     CWindow Functions

=============================================================================== */
/*===========================================================================
   This function is the base class constructor of an IWindow object.
   It allocates memory for the interface and sets the vtbl.
===========================================================================*/
static IWindow * CWindow_New(int16 nSize, CQPlayer * pOwner, VTBL(IWindow) * pvt)
{
   CWindow *         pme;
   VTBL(IWindow)  *  pVtbl;

   pme = MALLOC(nSize + sizeof(VTBL(IWindow)));
   if (!pme)
      return NULL;

   pme->m_pOwner = pOwner;
   pme->m_pIShell = pme->m_pOwner->a.m_pIShell;
   pme->m_pIDisplay = pme->m_pOwner->a.m_pIDisplay;

   pVtbl = (VTBL(IWindow) *)((byte *)pme + nSize);
   MEMCPY(pVtbl, pvt, sizeof(VTBL(IWindow)));
   INIT_VTBL(pme, IWindow, *pVtbl);

   return (IWindow *)pme;
}

/*===========================================================================
   This function indicates if IWINDOW_Enable()/IWINDOW_Disable() needs to be
   processed and also sets the bActive flag appropriately.
   It is called by derived class IWINDOW_Enable() function before processing
   the command.
===========================================================================*/
static boolean CWindow_ProcessEnable(IWindow * po, boolean bEnable)
{
   CWindow *   pme = (CWindow *)po;
   boolean     bProc = TRUE;

   if (!bEnable)
   {
      if (pme->m_bActive)
      {
         pme->m_bActive = FALSE;
         CQPlayer_CancelRedraw(pme->m_pOwner);
      }
      else
         bProc = FALSE;
   }
   else
   {
      if (pme->m_bActive)
         bProc = FALSE;
      else
         pme->m_bActive = TRUE;
   }

   return bProc;
}

/*===============================================================================

                     CMainWin Functions

=============================================================================== */
/*===========================================================================
   This function constucts the main window.
===========================================================================*/
static IWindow * CMainWin_New(CQPlayer * pOwner)
{
   CMainWin *        pme;
   VTBL(IWindow)     vtbl;

   QP_IWINDOW_SETVTBL(&vtbl, CMainWin_Enable, CMainWin_Redraw, CMainWin_HandleEvent, CMainWin_Delete);
   pme = (CMainWin *)CWindow_New(sizeof(CMainWin), pOwner, &vtbl);
   if (!pme)
      return NULL;

   {
      int      cx = pme->m_pOwner->m_cxWidth;
      int      cy = pme->m_pOwner->m_cyHeight;
      int      y, dy;
      AEERect  rect;

      // Initialize logo below the header
      pme->m_pLogo = ISHELL_LoadResImage(pme->m_pIShell, QPLAYER_RES_FILE, IDB_LOGO);
      if (!pme->m_pLogo)
         QP_WINERR_RETURN(pme);

      y = pme->m_pOwner->m_rectHdr.dy + 1;
      dy = cy/2 - y;
      SETAEERECT(&pme->m_rectLogo, 0, y, cx, dy);
      IIMAGE_SetFrameCount(pme->m_pLogo, 2);
      IIMAGE_SetAnimationRate(pme->m_pLogo, 500);

      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pMainMenu))
         QP_WINERR_RETURN(pme);

      SETAEERECT(&rect, 0, cy/2 + 1, cx, cy/2 - 1);
      QP_SetMenuAttr(pme->m_pMainMenu, AEECLSID_MENUCTL, pme->m_pOwner->m_nColorDepth, &rect, 0);
      QP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_PLAYFILE, NULL, IDB_PLAY,     IDM_MAIN_PLAYFILE, 0);
      QP_AddMenuItem(pme->m_pMainMenu, IDM_MAIN_ABOUT,    NULL, IDB_ABOUT,    IDM_MAIN_ABOUT,    0);

   }

   return (IWindow *)pme;
}

/*===========================================================================
   This function deletes the main window.
===========================================================================*/
static void CMainWin_Delete(IWindow * po)
{
   CMainWin *  pme = (CMainWin *)po;

   QP_RELEASEIF(pme->m_pLogo);

   if (pme->m_pMainMenu)
      pme->m_pOwner->m_wMainWin = IMENUCTL_GetSel(pme->m_pMainMenu);
   QP_RELEASEIF(pme->m_pMainMenu);

   FREE(pme);
}

/*===========================================================================
   This function enables/disables the main window.
===========================================================================*/
static void CMainWin_Enable(IWindow * po, boolean bEnable)
{
   CMainWin *  pme = (CMainWin *)po;

   if (!CWindow_ProcessEnable(po, bEnable))
      return;

   if (!pme->m_bActive)
   {
      IMENUCTL_SetActive(pme->m_pMainMenu, FALSE);
      IIMAGE_Stop(pme->m_pLogo);
      return;
   }

   IMENUCTL_SetActive(pme->m_pMainMenu, TRUE);
   IMENUCTL_SetSel(pme->m_pMainMenu, pme->m_pOwner->m_wMainWin);
   QP_DrawImage(pme->m_pLogo, &pme->m_rectLogo, TRUE);
}

/*===========================================================================
   This function redraws the main window.
===========================================================================*/
static void CMainWin_Redraw(IWindow * po)
{
   CMainWin *  pme = (CMainWin *)po;

   if (!pme->m_bActive)
      return;

   (void)IDISPLAY_ClearScreen(pme->m_pIDisplay);

   QP_DRAWHEADER(pme);
   QP_DrawImage(pme->m_pLogo, &pme->m_rectLogo, TRUE);
   (void)IMENUCTL_Redraw(pme->m_pMainMenu);

   IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This function processes events routed to main window.
===========================================================================*/
static boolean CMainWin_HandleEvent(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CMainWin *  pme = (CMainWin *)po;
   boolean     bRet = TRUE;

   if (eCode == EVT_COPYRIGHT_END && pme->m_bAbout)
   {
      pme->m_bAbout = FALSE;
      CQPlayer_Redraw(pme->m_pOwner, TRUE);
      return TRUE;
   }

   if (QP_ISEVTKEY(eCode))
      return IMENUCTL_HandleEvent(pme->m_pMainMenu, eCode, wParam, dwParam);

   if (!QP_ISEVTCMD(eCode))
      return FALSE;

   switch (wParam)
   {
      case IDM_MAIN_PLAYFILE:
         CQPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
         break;

      case IDM_MAIN_ABOUT:
         CMainWin_About(pme);
         break;

      default:
         bRet = FALSE;
         break;
   }

   return bRet;
}

/*===========================================================================
   This function displays the About dialog of the app.
===========================================================================*/
static void CMainWin_About(CMainWin * pme)
{
   CQPlayer_DisableWin(pme->m_pOwner);

   pme->m_bAbout = TRUE;

   IDISPLAY_ClearScreen(pme->m_pIDisplay);
   QP_DRAWHEADER(pme);
   ISHELL_ShowCopyright(pme->m_pIShell);
}

/*===============================================================================

                     CFileListWin Functions

=============================================================================== */

/*===========================================================================
   This function constucts the file list window.
===========================================================================*/
static IWindow * CFileListWin_New(CQPlayer * pOwner)
{
   CFileListWin *    pme;
   VTBL(IWindow)     vtbl;

   QP_IWINDOW_SETVTBL(&vtbl, CFileListWin_Enable, CFileListWin_Redraw, CFileListWin_HandleEvent, CFileListWin_Delete);
   pme = (CFileListWin *)CWindow_New(sizeof(CFileListWin), pOwner, &vtbl);
   if (!pme)
      return NULL;

   {
      int            cx = pme->m_pOwner->m_cxWidth;
      int            cy = pme->m_pOwner->m_cyHeight;
      AEERect        rect;
      AEEFileInfoEx  fi;
      uint16         wItemID = 0;
      AECHAR *       szBuf;
      int            y;
      IFileMgr *     pIFileMgr;
      char*          pFileName;

      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MENUCTL, (void **)&pme->m_pFileMenu))
         QP_WINERR_RETURN(pme);

      y = pme->m_pOwner->m_rectHdr.dy + 1;
      SETAEERECT(&rect, 0, y, cx, cy - y);

      QP_SetMenuAttr(pme->m_pFileMenu, AEECLSID_MENUCTL, pme->m_pOwner->m_nColorDepth, &rect, 0);
      (void)IMENUCTL_SetTitle(pme->m_pFileMenu, QPLAYER_RES_FILE, IDS_FILELISTMENU_TITLE, NULL);

      pFileName = MALLOC(QP_MAX_FILE_NAME);
      if(!pFileName)
      {
        DBGPRINTF_ERROR("MP: cannot allocate memory.");
        QP_WINERR_RETURN(pme);
      }

      MEMSET (pFileName, 0, QP_MAX_FILE_NAME);
      MEMSET(&fi, 0, sizeof(fi));
      fi.nStructSize = sizeof(fi);
      fi.pszFile = pFileName;
      fi.nMaxFile = QP_MAX_FILE_NAME;

      // Enumerate all files and add them to menu
      szBuf = MALLOC(QP_MAX_FILE_NAME * sizeof(AECHAR));
      if(!szBuf)
         QP_WINERR_RETURN(pme);

      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_FILEMGR, (void **)&pIFileMgr))
      {
         FREE(szBuf);
         QP_WINERR_RETURN(pme);
      }

      IFILEMGR_EnumInit(pIFileMgr, QP_MEDIA_DIR, FALSE);
      while (wItemID < QP_MAX_FILES  && IFILEMGR_EnumNextEx(pIFileMgr, &fi))
      {
         char *   szName;

         pme->m_szFileArray[wItemID] = STRDUP(fi.pszFile);
         if (!pme->m_szFileArray[wItemID])
         {
            pme->m_NumFiles = wItemID;
            FREE(szBuf);
            QP_RELEASEIF(pIFileMgr);
            QP_WINERR_RETURN(pme);
         }

         // Add only the file name
         szName = QP_GetFileName(fi.pszFile);
         if (szName)
         {
            uint16   wIconID = CQPlayer_FindHandlerType(pme->m_pOwner, szName);
            if (wIconID)
            {
               STRTOWSTR(szName, szBuf, QP_MAX_FILE_NAME);
               QP_AddMenuItem(pme->m_pFileMenu, 0, szBuf, wIconID, (uint16)(IDM_FILELIST_BASE + wItemID), 0);
            }
         }

         wItemID++;
      }

      pme->m_NumFiles = wItemID;

      QP_RELEASEIF(pIFileMgr);
      FREE(szBuf);
      FREE(pFileName);
   }

   return (IWindow *)pme;
}

/*===========================================================================
   This function deletes the file list window.
===========================================================================*/
static void CFileListWin_Delete(IWindow * po)
{
   CFileListWin * pme = (CFileListWin *)po;
   int            i;

   for ( i = 0; i < pme->m_NumFiles; i++ )
   {
      if (pme->m_szFileArray[i])
         FREE((void *)pme->m_szFileArray[i]);
   }

   if (pme->m_pFileMenu)
      pme->m_pOwner->m_wFileListWin = IMENUCTL_GetSel(pme->m_pFileMenu);
   QP_RELEASEIF(pme->m_pFileMenu);

   FREE(pme);
}

/*===========================================================================
   This function enables/disables the file list window.
===========================================================================*/
static void CFileListWin_Enable(IWindow * po, boolean bEnable)
{
   CFileListWin * pme = (CFileListWin *)po;

   if (!CWindow_ProcessEnable(po, bEnable))
      return;

   if (!pme->m_bActive)
   {
      IMENUCTL_SetActive(pme->m_pFileMenu, FALSE);
      return;
   }

   if (pme->m_NumFiles)
   {
      IMENUCTL_SetActive(pme->m_pFileMenu, TRUE);
      IMENUCTL_SetSel(pme->m_pFileMenu, pme->m_pOwner->m_wFileListWin);
   }
   else
   {
      QP_ErrorDlg(pme->m_pOwner, IDS_ERR_NOFILES);
   }
}

/*===========================================================================
   This function redraws the file list window.
===========================================================================*/
static void CFileListWin_Redraw(IWindow * po)
{
   CFileListWin * pme = (CFileListWin *)po;

   if (!pme->m_bActive)
      return;

   (void)IDISPLAY_ClearScreen(pme->m_pIDisplay);

   QP_DRAWHEADER(pme);
   (void)IMENUCTL_Redraw(pme->m_pFileMenu);

   (void)IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This function processes events routed to the file list window.
===========================================================================*/
static boolean CFileListWin_HandleEvent(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CFileListWin * pme = (CFileListWin *)po;

   if (QP_ISCLR(eCode) || QP_ISCMD(eCode, IDS_OK))
   {
      CQPlayer_SetWindow(pme->m_pOwner, MPW_MAIN, 0);
      return TRUE;
   }

   if (QP_ISEVTKEY(eCode))
   {
      return IMENUCTL_HandleEvent(pme->m_pFileMenu, eCode, wParam, dwParam);
   }

   // Check if it is EVT_COMMAND and corresponds to files
   if (!QP_ISEVTCMD(eCode))
      return FALSE;
   if (wParam < IDM_FILELIST_BASE)
      return FALSE;

   // Play the file...
   CQPlayer_PlayFile(pme->m_pOwner, pme->m_szFileArray[(int)wParam - IDM_FILELIST_BASE]);

   return TRUE;
}

/*===============================================================================

                     CPlayerWin Functions

=============================================================================== */
/*===========================================================================
   This function constructs player window.
   Player window type can be
   (1) MPPW_PLAY: Plays media files
   (2) MPPW_IMAGE: Displays image (of IImage type)
===========================================================================*/
static IWindow * CPlayerWin_New(CQPlayer * pOwner, MPPlayerWin eWin)
{
   CPlayerWin *      pme;
   VTBL(IWindow)     vtbl;
   int               y;
   int               dy;

   QP_IWINDOW_SETVTBL(&vtbl, CPlayerWin_Enable, CPlayerWin_Redraw, CPlayerWin_HandleEvent, CPlayerWin_Delete);
   pme = (CPlayerWin *)CWindow_New(sizeof(CPlayerWin), pOwner, &vtbl);
   if (!pme)
      return NULL;

   pme->m_eWin = eWin;

   // Set title and image rects. Create the static control.
   {
      int      cx = pme->m_pOwner->m_cxWidth;
      int      cy = pme->m_pOwner->m_cyHeight;
      int      nLChSize = pme->m_pOwner->m_nLChSize;
      int      cyProgMain = pme->m_pOwner->m_nNChSize + 2;
      AEERect  rectTitle;
      AEERect  rectMenu;

      // File name text rect
      y = pme->m_pOwner->m_rectHdr.dy + 1;
      dy = nLChSize;
      SETAEERECT(&rectTitle, 0, y, cx, dy);

      // Create IStatic for file name
      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pTitle))
         QP_WINERR_RETURN(pme);
      ISTATIC_SetRect(pme->m_pTitle, &rectTitle);
      ISTATIC_SetProperties(pme->m_pTitle, ST_CENTERTEXT | ST_NOSCROLL);

      // Image rect
      y += rectTitle.dy;
      dy = cy - y - cyProgMain - QP_ICONVIEWCTL_CY - 2;
      SETAEERECT(&pme->m_rectImage, 0, y, cx, dy);
      MEMCPY(&pme->m_rectImageCopy, &pme->m_rectImage, sizeof(AEERect));

      // IconViewCtl rect
      y = cy - QP_ICONVIEWCTL_CY;
      dy = QP_ICONVIEWCTL_CY;
      SETAEERECT(&rectMenu, 0, y, cx, dy);
      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pme->m_pPlayerMenu))
         QP_WINERR_RETURN(pme);

      QP_SetMenuAttr(pme->m_pPlayerMenu, AEECLSID_ICONVIEWCTL, pme->m_pOwner->m_nColorDepth, &rectMenu, MP_MAXSOFTKEYITEMS);
   }

   if (CPlayerWin_IsImage(pme))
   {
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_FULLSCRN, IDM_PM_FULLSCRN, 0);
   }
   else if (CPlayerWin_IsMedia(pme))
   {
      int      cx = pme->m_pOwner->m_cxWidth;
      int      cy = pme->m_pOwner->m_cyHeight;
      int      cyProgMain = pme->m_pOwner->m_nNChSize + 2;
      AEERect  rectProg;

      // ProgMain rect
      y = cy - cyProgMain - QP_ICONVIEWCTL_CY - 2;
      dy = cyProgMain;
      SETAEERECT(&rectProg, 0, y, cx, dy);

      // Initialize ProgCtl
      pme->m_ProgCtl.m_pIShell = pme->m_pIShell;
      pme->m_ProgCtl.m_pIDisplay = pme->m_pIDisplay;
      if (!CProgCtl_Init(&pme->m_ProgCtl, &rectProg))
         QP_WINERR_RETURN(pme);

      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_PLAY,     IDM_PM_PLAY,     0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_STOP,     IDM_PM_STOP,     0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_FULLSCRN, IDM_PM_FULLSCRN, 0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_REWIND,   IDM_PM_REWIND,   0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_PAUSE,    IDM_PM_PAUSE,    0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_FF,       IDM_PM_FF,       0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_UP,       IDM_PM_UP,       0);
      QP_AddMenuItem(pme->m_pPlayerMenu, 0, NULL, IDB_DOWN,     IDM_PM_DOWN,     0);

   }

   pme->m_bMediaVideo = FALSE;
   pme->m_ZoomDirection = 0;
   pme->m_bPlaypending = FALSE;
   return (IWindow *)pme;
}

/*===========================================================================
   This function deletes the player window.
===========================================================================*/
static void CPlayerWin_Delete(IWindow * po)
{
   CPlayerWin *   pme = (CPlayerWin *)po;

   QP_RELEASEIF(pme->m_pTitle);
   QP_RELEASEIF(pme->m_pPlayerMenu);

   // Release memory for file name allocated in CQPlayer
   FREEIF(pme->m_pszFile);

   if (CPlayerWin_IsMedia(pme))
   {
      QP_RELEASEIF(pme->m_pMedia);

      CProgCtl_Release(&pme->m_ProgCtl);

      if (pme->m_bProgTimer)
         ISHELL_CancelTimer(pme->m_pIShell, (PFNNOTIFY)CPlayerWin_ProgTimerNotify, pme);
   }
   else if (CPlayerWin_IsImage(pme))
   {
      QP_RELEASEIF(pme->m_pImage);
      QP_RELEASEIF(pme->m_pImageCtl);
   }

   FREE(pme);
}

/*===========================================================================
   This function enables/disables the player window.
===========================================================================*/
static void CPlayerWin_Enable(IWindow * po, boolean bEnable)
{
   CPlayerWin *   pme = (CPlayerWin *)po;
   boolean        bActive;

   if (!CWindow_ProcessEnable(po, bEnable))
      return;

   bActive = pme->m_bActive;

   ISTATIC_SetActive(pme->m_pTitle, bActive);
   IMENUCTL_SetActive(pme->m_pPlayerMenu, bActive);
   if (CPlayerWin_IsImage(pme))
      IIMAGECTL_SetActive(pme->m_pImageCtl, bActive);

   if (!pme->m_bActive)
   {
      if (CPlayerWin_IsMedia(pme) && pme->m_pMedia)
            (void)IMEDIA_Stop((IMedia*)pme->m_pMedia);
   }
   else
   {
      if(pme->m_eWin == MPPW_PLAY )
      {
        IMENUCTL_SetSel(pme->m_pPlayerMenu, (uint16) IDM_PM_PLAY);
      }
   }
}

/*===========================================================================
   This function redraws player window.
   If the full screen mode is on, then
   (1) if playback is not ongoing, then it paints the screen with black.
   (2) if image is being displayed, then redraws the image.
===========================================================================*/
static void CPlayerWin_Redraw(IWindow * po)
{
   CPlayerWin *   pme = (CPlayerWin *)po;
   char *         pszFile = pme->m_pszFile;
   AEERect        rect;

   if (!pme->m_bActive)
      return;

   if (QP_ISFULLSCRN(pme))
   {
      if (!pme->m_bPlayRec)
      {
         IDISPLAY_FillRect(pme->m_pIDisplay, NULL, RGB_BLACK);
         IDISPLAY_Update(pme->m_pIDisplay);
      }
      else if (CPlayerWin_IsImage(pme))
      {
         IDISPLAY_ClearScreen(pme->m_pIDisplay);
         IIMAGECTL_Redraw(pme->m_pImageCtl);
         IDISPLAY_Update(pme->m_pIDisplay);
      }

      return;
   }

   IDISPLAY_ClearScreen(pme->m_pIDisplay);

   // Header
   QP_DRAWHEADER(pme);

   // File name (title) text
   if (pszFile)
   {
      STRTOWSTR(QP_GetFileName(pszFile), pme->m_szText, sizeof(pme->m_szText));
      QP_FitStaticText(pme->m_pIDisplay, pme->m_pTitle, AEE_FONT_NORMAL, pme->m_szText);
      ISTATIC_Redraw(pme->m_pTitle);
   }

   // Draw the line below text.
   ISTATIC_GetRect(pme->m_pTitle, &rect);
   rect.y += rect.dy - 1; rect.dy = 1;
   QP_FrameRect(pme->m_pIDisplay, &rect);

   // SoftKey
   IMENUCTL_Redraw(pme->m_pPlayerMenu);

   if (CPlayerWin_IsMedia(pme))
   {
      // Progress bar...
      CPlayerWin_UpdateProgCtl(pme, 0, 0, 0);
   }
   else if (CPlayerWin_IsImage(pme))
   {
      IIMAGECTL_Redraw(pme->m_pImageCtl);
      IDISPLAY_Update(pme->m_pIDisplay);
   }
}

/*===========================================================================
   This function handles the events routed to the player window.
   If the event is
   (1) CLR or OK from error dialog:
       (a) If app is in plugin mode, it is closed.
       (b) If media playback is on, then file list window is opened

   (2) AVK_0 or EVT_COMMAND(IDM_PM_FULLSCRN): Full screen is toggled
   (3) EVT_CREATEMEDIA: Media is created using AEEMediaUtil_CreateMedia()
   (4) EVT_COMMAND: processes the menu commands.
   (5) EVT_KEY: routed appropriately to the controls.
===========================================================================*/
static boolean CPlayerWin_HandleEvent(IWindow * po, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   CPlayerWin    *pme = (CPlayerWin *)po;
   IMediaVideo   *pMediaVideo = NULL;
   int            nRet;

   nRet = CPlayerWin_HandleCreateMedia(pme, eCode, wParam, dwParam);
   if(nRet >= 0)
   {
      return (boolean)nRet;
   }

   // Process CLR or OK from error dlg
   if (QP_ISCLR(eCode) || QP_ISCMD(eCode, IDS_OK))
   {
      // Stop the app if it is in plugin mode.
      if (QP_ISPLUGIN(pme))
         ISHELL_CloseApplet(pme->m_pIShell, FALSE);
      else
      {
         if (pme->m_eWin == MPPW_PLAY || pme->m_eWin == MPPW_IMAGE)
         {
            CQPlayer_SetWindow(pme->m_pOwner, MPW_FILELIST, 0);
         }
      }
      return TRUE;
   }

   // Process EVT_KEY...
   if (QP_ISEVTKEY(eCode))
   {
      if (wParam == AVK_0)
      {
         CPlayerWin_FullScreen(pme, (boolean)!pme->m_bFullScreen, FALSE);
      }
      else if (CPlayerWin_IsMedia(pme))
      {
         if(wParam == AVK_UP)
         {
            uint16   wVol = pme->m_wVolume + QP_VOLUME_STEP;

            wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;
            nRet = IMEDIA_SetVolume((IMedia*)pme->m_pMedia, wVol);
            DBGPRINTF_MED("QP: set volume to %d = %d", wVol);
            if (SUCCESS == nRet)
            {
              QP_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME, MM_STATUS_DONE, NULL, 0);
            }
            return TRUE;
         }
         else if(wParam == AVK_DOWN)
         {
            int16    nVol = (int16)pme->m_wVolume - QP_VOLUME_STEP;

            nVol = (nVol < 0) ? 0 : nVol;
            nRet = IMEDIA_SetVolume((IMedia*)pme->m_pMedia, (uint16)nVol);
            DBGPRINTF_MED("QP: set volume to %d = %d", nVol);
            if (SUCCESS == nRet)
            {
              QP_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME, MM_STATUS_DONE, NULL, 0);
            }
            return TRUE;
         }
         else
         {
           return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, eCode, wParam, dwParam);
         }
      }
      else if (CPlayerWin_IsImage(pme))
      {
         if (wParam !=  AVK_SELECT)
            return IIMAGECTL_HandleEvent(pme->m_pImageCtl, eCode, wParam, dwParam);
         else
            return IMENUCTL_HandleEvent(pme->m_pPlayerMenu, eCode, wParam, dwParam);
      }

      return TRUE;
   }

   // If this event is not EVT_COMMAND, then return...
   if (!QP_ISEVTCMD(eCode))
      return FALSE;

   if ( (CPlayerWin_IsMedia(pme) && !pme->m_pMedia) ||
        (CPlayerWin_IsImage(pme) && !pme->m_pImageCtl) )
      return TRUE;

   if(pme->m_bMediaVideo)
   {
     pMediaVideo = QP_GetVideoInterface(pme->m_pMedia);
     if(NULL == pMediaVideo)
     {
       DBGPRINTF_ERROR("QP:failed to get IMediaVideo.");
       return TRUE;
     }
   }
   // Process EVT_COMMAND...
   switch (wParam)
   {
      case IDM_PM_PLAY:
        DBGPRINTF_MED("QP: IDM_PM_PLAY received.");
        if (CPlayerWin_IsPause(pme))
        {
           DBGPRINTF("QP: it is in pause, resume now.");
           (void)IMEDIA_Resume((IMedia*)pme->m_pMedia);
        }
        else if (SUCCESS == IMEDIA_GetTotalTime((IMedia*)pme->m_pMedia))
        {
          DBGPRINTF("QP: has total time, replay now.");
          if (AEE_SUCCESS == IMEDIA_Play((IMedia*)pme->m_pMedia))
            pme->m_bPlayRec = TRUE;
          pme->m_bPlaypending = FALSE;
        }
        else
        {
          pme->m_bPlaypending = TRUE;
        }
        break;

      case IDM_PM_STOP:
        DBGPRINTF_MED("QP: IDM_PM_STOP event received.");
        (void)IMEDIA_Stop((IMedia*)pme->m_pMedia);
        break;

      case IDM_PM_REWIND:
         DBGPRINTF_MED("QP: IDM_PM_REWIND event received.");
         (void)IMEDIA_Rewind((IMedia*)pme->m_pMedia, QP_SEEK_TIME);
         break;

      case IDM_PM_FF:
         DBGPRINTF_MED("QP: IDM_PM_FF event received.");

         (void)IMEDIA_FastForward((IMedia*)pme->m_pMedia, QP_SEEK_TIME);
         break;

      case IDM_PM_PAUSE:
         DBGPRINTF_MED("QP: IDM_PM_PAUSE event received.");

         if (CPlayerWin_IsPause(pme))
            (void)IMEDIA_Resume((IMedia*)pme->m_pMedia);
         else
            (void)IMEDIA_Pause((IMedia*)pme->m_pMedia);
         break;

      case IDM_PM_UP:
      {
         uint16   wVol = pme->m_wVolume + QP_VOLUME_STEP;

         DBGPRINTF_MED("QP: IDM_PM_UP event received.");

         wVol = (wVol > AEE_MAX_VOLUME) ? AEE_MAX_VOLUME : wVol;
         nRet = IMEDIA_SetVolume((IMedia*)pme->m_pMedia, wVol);
         DBGPRINTF_MED("QP: set volume to %d = %d", wVol);
         if (SUCCESS == nRet)
         {
           QP_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME, MM_STATUS_DONE, NULL, 0);
         }

         break;
      }

      case IDM_PM_DOWN:
      {
         int16    nVol = (int16)pme->m_wVolume - QP_VOLUME_STEP;

         DBGPRINTF_MED("QP: IDM_PM_DOWN event received.");

         nVol = (nVol < 0) ? 0 : nVol;
         nRet = IMEDIA_SetVolume((IMedia*)pme->m_pMedia, (uint16)nVol);
         DBGPRINTF_MED("QP: set volume to %d = %d", nVol);
         if (SUCCESS == nRet)
         {
           QP_SendMediaNotify(pme, MM_CMD_SETMEDIAPARM, MM_PARM_VOLUME, MM_STATUS_DONE, NULL, 0);
         }

         break;
      }

      case IDM_PM_ZOOM_OUT:
      case IDM_PM_ZOOM_IN:
      {
        int32 ReqZoomX, ReqZoomY;
        int32 zoomX, zoomY;

        DBGPRINTF_MED("QP: IDM_PM_ZOOM event received.");
        pme->m_ZoomDirection = ((wParam == IDM_PM_ZOOM_IN) ? 1 : -1);
        if(SUCCESS == IMediaVideo_GetScaling(pMediaVideo, &ReqZoomX, &ReqZoomY))
        {
          ReqZoomX = ReqZoomX + (pme->m_ZoomDirection* QP_ZOOM_STEP);
          ReqZoomY = ReqZoomY + (pme->m_ZoomDirection* QP_ZOOM_STEP);
          if(SUCCESS == IMediaVideo_SetScaling(pMediaVideo, ReqZoomX, ReqZoomY))
          {
            if(SUCCESS == IMediaVideo_GetScaling(pMediaVideo, &zoomX, &zoomY))
              DBGPRINTF_MED("QP: scale req(x=%d, y=%d), scaling(x=%d, y=%d).", ReqZoomX, ReqZoomY, zoomX, zoomY);
            else
              DBGPRINTF_ERROR("QP:failed to get scale factor.");
          }
          else
            DBGPRINTF_ERROR("QP:failed to perform scaling.");

          /*
           * Clear pending direction in case of error, so we can
           * try again later.
           */
          pme->m_ZoomDirection = 0;
        }
        else
          DBGPRINTF_ERROR("QP:failed to get scale factor.");
        break;
      }
      case IDM_PM_ROTATE:
      {
        int32 RotationValueReq;
        int32 RotationValue;

        DBGPRINTF_MED("QP: IDM_PM_ROTATE event received.");
        if(SUCCESS == IMediaVideo_GetRotation(pMediaVideo, &RotationValueReq))
        {
          RotationValueReq += QP_ROTATE_STEP;
          if(SUCCESS == IMediaVideo_SetRotation(pMediaVideo, RotationValueReq))
          {
            if(SUCCESS == IMediaVideo_GetRotation(pMediaVideo, &RotationValue))
              DBGPRINTF_MED("QP: rotate req = %d, rotate = %d", RotationValueReq, RotationValue);
            else
              DBGPRINTF_ERROR("QP: failed to retrieve rotate value.");
          }
          else
            DBGPRINTF_ERROR("QP: failed to rotate.");
        }
        else
          DBGPRINTF_ERROR("QP: failed to retrieve rotate value.");
        break;
      }
      case IDM_PM_FULLSCRN:
         CPlayerWin_FullScreen(pme, (boolean)!pme->m_bFullScreen, FALSE);
         break;

      default:
         return FALSE;
   }

   return TRUE;
}

/*===========================================================================
  This function handles the create media event.
  return -1 if event is not handled
  return 1, if SUCCESS, and event is handled
  return 0, if failed to create media object, but the event is handled (not used now)
===========================================================================*/
static int CPlayerWin_HandleCreateMedia(CPlayerWin * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   int nReturn = -1;

   // Process create media events...
   if (eCode == EVT_CREATEMEDIA)
   {
      int   nRet = EFAILED;
      if (eCode == EVT_CREATEMEDIA)
      {
         IMediaVideo* pMediaVideo;

         // create media object
         nRet = QP_CreateMedia(pme->m_pIShell, &pme->m_md, &pme->m_pMedia);
         if(SUCCESS == nRet)
         {
           // find out if this is the AEEIID_MediaVideo based media
           if(SUCCESS == IMEDIA_QueryInterface(pme->m_pMedia, AEEIID_IMediaVideo, (void **)&pMediaVideo))
           {
             (void)IMEDIA_Release((IMedia*)pme->m_pMedia);
             // this is IMediaVideo based object, save the ptr
             pme->m_pMedia = (IMedia*)pMediaVideo;
             pme->m_bMediaVideo = TRUE;
           }
           else
           {
             // this is not IMediaVideo based object
             pme->m_bMediaVideo = FALSE;
           }
         }
         else 
         {
            DBGPRINTF_ERROR("QP: failed to create media.");
            QP_ErrorDlg(pme->m_pOwner, IDS_ERR_CREATEMEDIA);
            nReturn = 0;
            return nReturn;
         }

         // update menu
         if (SUCCESS == nRet && pme->m_bMediaVideo)
         {
           AEEMediaVideoNetPolicyInfo policy;
           policy.dwSize = sizeof(AEEMediaVideoNetPolicyInfo);
           policy.bQoSEnable = TRUE;
           policy.wPrimaryPDPProfileNo = 1; // set PDP profile number

           // it is ok to faile, if the target is CDMA
           if(SUCCESS != IMediaVideo_SetTransport(pMediaVideo, MMV_TRANSPORT_NET_POLICY, (int32*)&policy))
           {
             DBGPRINTF_HIGH("QP: failed to set net policy.");
           }
           // enable zoom and rotation menu
           QP_AddMenuItem(pme->m_pPlayerMenu,  0, NULL, IDB_ZOOM_OUT, IDM_PM_ZOOM_OUT, 0);
           QP_AddMenuItem(pme->m_pPlayerMenu,  0, NULL, IDB_ZOOM_IN,  IDM_PM_ZOOM_IN,  0);
           QP_AddMenuItem(pme->m_pPlayerMenu,  0, NULL, IDB_ROTATE,   IDM_PM_ROTATE,   0);
         }

         if(SUCCESS == nRet)
         {
           // Set the media data and put IMedia in Ready state.
           nRet = IMEDIA_SetMediaData((IMedia*)pme->m_pMedia, &pme->m_md);
           if (nRet != SUCCESS)
           {
              DBGPRINTF_HIGH("QP: failed to set media data.");
           }
         }


         if (SUCCESS != IMEDIA_RegisterNotify((IMedia*)pme->m_pMedia, CPlayerWin_MediaNotify, pme))
         {
            DBGPRINTF_ERROR("QP: failed to register Notify callback.");
            QP_ErrorDlg(pme->m_pOwner, IDS_ERR_MEDIA);
         }
         else
         {
            uint32   dwCaps;
            nRet = IMEDIA_GetMediaParm((IMedia*)pme->m_pMedia, MM_PARM_CAPS, (int32 *)&dwCaps, 0);
            if(nRet != SUCCESS)
               DBGPRINTF_ERROR("QP: get MM_PARM_CAPS error.");
    
            pme->m_bImage = (dwCaps & MM_CAPS_VIDEO) ? TRUE : FALSE;
            if (!pme->m_bImage)
               (void)IMENUCTL_DeleteItem(pme->m_pPlayerMenu, IDM_PM_FULLSCRN);
            else
               (void)IMEDIA_SetRect((IMedia*)pme->m_pMedia, &pme->m_rectImage, NULL);
    
            CQPlayer_Redraw(pme->m_pOwner, TRUE);
         }
    
         DBGPRINTF_LOW("QP: media created.");
         nReturn = 1;
      }
   }

   return nReturn;
}
/*===========================================================================
   This function indicates IMedia is in paused state.
===========================================================================*/
static boolean CPlayerWin_IsPause(CPlayerWin * pme)
{
   boolean  bChg;
   int      nState = IMEDIA_GetState((IMedia*)pme->m_pMedia, &bChg);

   if (bChg)
      return FALSE;

   return (nState == MM_STATE_PLAY_PAUSE);
}

/*===========================================================================
   This function saves the file name and sets the media data locally.
   Assumption: pmd is not NULL.
===========================================================================*/
static boolean CPlayerWin_SetMediaData(CPlayerWin * pme, AEEMediaData * pmd)
{
   if (!pmd->pData)
      return FALSE;

   pme->m_pszFile = (char *)pmd->pData;
   MEMCPY(&pme->m_md, pmd, sizeof(AEEMediaData));

   return TRUE;
}

/*===========================================================================
   This function toggles full screen mode and does a redraw.
===========================================================================*/
static boolean CPlayerWin_FullScreen(CPlayerWin * pme, boolean bFull, boolean bDeferRedraw)
{
   if (!pme->m_bImage)
      return FALSE;

   if (!pme->m_bFullScreen)
   {
      SETAEERECT(&pme->m_rectImage, 0, 0, pme->m_pOwner->m_cxWidth, pme->m_pOwner->m_cyHeight);
   }
   else
   {
      MEMCPY(&pme->m_rectImage, &pme->m_rectImageCopy, sizeof(AEERect));
   }

   if (CPlayerWin_IsMedia(pme))
      (void)IMEDIA_SetRect((IMedia*)pme->m_pMedia, &pme->m_rectImage, NULL);
   else if (CPlayerWin_IsImage(pme))
      (void)IIMAGECTL_SetRect(pme->m_pImageCtl, &pme->m_rectImage);

   pme->m_bFullScreen = bFull;

   CQPlayer_Redraw(pme->m_pOwner, bDeferRedraw);

   return TRUE;
}

/*===============================================================================

                     ProgCtl Functions

=============================================================================== */
/*===========================================================================
   This funtion initializes the ProgCtl.
   ProgCtl contains two rectangles: main rectangle and within it the progress
   bar rectangle. The title is displayed on left of the progress bar within
   the main rectangle.
===========================================================================*/
static boolean CProgCtl_Init(CProgCtl * pme, AEERect * pRectMain)
{
   AEERect  rect;
   int      x, y;
   int      dxProg, dyProg;

   MEMCPY(&pme->m_rectMain, pRectMain, sizeof(AEERect));

   if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_STATIC, (void **)&pme->m_pTitle))
      return FALSE;

   // ProgBar rect
   dxProg = pRectMain->dx / QP_PROGBAR_DX_DIV;
   dyProg = pRectMain->dy / QP_PROGBAR_DY_DIV;
   x = pRectMain->x + pRectMain->dx - dxProg + 1;
   y = pRectMain->y + pRectMain->dy / 2 - dyProg/2;

   if (dxProg > QP_PROGBAR_DX_OFFSET)
      dxProg -= QP_PROGBAR_DX_OFFSET;
   SETAEERECT(&pme->m_rectBar, x, y, dxProg, dyProg);

   SETAEERECT(&rect, pRectMain->x + 1, pRectMain->y + 1, pRectMain->dx - dxProg, pRectMain->dy);
   ISTATIC_SetRect(pme->m_pTitle, &rect);
   ISTATIC_SetProperties(pme->m_pTitle, ST_CENTERTEXT | ST_NOSCROLL);

   return TRUE;
}

/*===========================================================================
   This function updates the progress bar and the title.
===========================================================================*/
static void CProgCtl_SetPos(CProgCtl * pme, AECHAR * psz, uint16 wPct)
{
   QP_FitStaticText(pme->m_pIDisplay, pme->m_pTitle, AEE_FONT_NORMAL, psz);
   ISTATIC_Redraw(pme->m_pTitle);
   CProgCtl_DrawHist(pme, wPct);
   QP_FrameRect(pme->m_pIDisplay, &pme->m_rectMain);
}

/*===========================================================================
   This function deletes the ProgCtl
===========================================================================*/
static void CProgCtl_Release(CProgCtl * pme)
{
   QP_RELEASEIF(pme->m_pTitle);
}

/*===========================================================================
   This function fills wPct of  progress bar rectangle.
===========================================================================*/
static void CProgCtl_DrawHist(CProgCtl * pme, uint16 wPct)
{
   AEERect  rc;

   MEMCPY(&rc, &pme->m_rectBar, sizeof(AEERect));

   if(wPct > 100)
      wPct = 100;

   IDISPLAY_DrawFrame(pme->m_pIDisplay, &rc, AEE_FT_RAISED, CLR_SYS_SCROLLBAR);
   rc.dx = (rc.dx * wPct) / 100;
   IDISPLAY_FillRect(pme->m_pIDisplay, &rc, QP_CLR_PROG_FILL);
}

/*===============================================================================

                     Local Functions

=============================================================================== */
/*===========================================================================
   This function draws the image and centers it within the specified
   rectangle if bCenter is TRUE.
===========================================================================*/
static void QP_DrawImage(IImage * pImage, AEERect * pRect, boolean bCenter)
{
   AEEImageInfo   ii;
   int            x;
   int            y;

   IIMAGE_GetInfo(pImage, &ii);

   // Do not display if image does not fit in the allocated rectangle.
   if (ii.cx > pRect->dx || ii.cy > pRect->dy)
      return;

   if (bCenter)
   {
      x = pRect->x + (pRect->dx / 2) - (ii.cxFrame / 2);
      y = pRect->y + (pRect->dy / 2) - (ii.cy / 2);
   }
   else
   {
      x = pRect->x;
      y = pRect->y;
   }

   IIMAGE_Start(pImage, x, y);
}

/*===========================================================================
   This function adds one item to the specified IMenuCtl.
===========================================================================*/
static boolean QP_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData)
{
   CtlAddItem  ai;

   // Fill in the CtlAddItem structure values
   ai.pText = pText;
   ai.pImage = NULL;
   ai.pszResImage = QPLAYER_RES_FILE;
   ai.pszResText = QPLAYER_RES_FILE;
   ai.wText = wTextID;
   ai.wFont = AEE_FONT_NORMAL;
   ai.wImage = wImageID;
   ai.wItemID = wItemID;
   ai.dwData = dwData;

   // Add the item to the menu control
   return IMENUCTL_AddItemEx( pMenu, &ai );
}

/*===========================================================================
   This function adds the extension string to the given string.
===========================================================================*/
static boolean QP_AddExtension(char ** ppszExtList, char * psz)
{
   char *   pExt;
   int      nLen;

   if (!ppszExtList || !psz)
      return FALSE;

   if (*ppszExtList)
   {
      nLen = STRLEN(*ppszExtList) + STRLEN(psz) + STRLEN(QP_EXT_SEPARATOR) + 1;
      pExt = REALLOC(*ppszExtList, nLen);
   }
   else
   {
      nLen = STRLEN(psz) + STRLEN(QP_EXT_SEPARATOR) + 1;
      pExt = MALLOC(STRLEN(psz) + STRLEN(QP_EXT_SEPARATOR) + 1);
   }

   if (!pExt)
      return FALSE;

   STRLCAT(pExt, psz, nLen);
   STRLCAT(pExt, QP_EXT_SEPARATOR, nLen);

   *ppszExtList = pExt;
   return TRUE;
}

/*===========================================================================
   This function displays the specified error message and disables the
   current window.
===========================================================================*/
static void QP_ErrorDlg(CQPlayer * pme, uint16 wResErrID)
{
   AEEPromptInfo  pi;
   uint16         wButtonIDs[] = {IDS_OK, 0};

   if (pme->m_pWin)
      CQPlayer_DisableWin(pme);

   pi.pszRes = QPLAYER_RES_FILE;
   pi.pTitle = NULL;
   pi.pText = NULL;
   pi.wTitleID = 0; //IDS_ERR_BASE;
   pi.wTextID = wResErrID;
   pi.wDefBtn = IDS_OK;
   pi.pBtnIDs = wButtonIDs;
   pi.dwProps = ST_MIDDLETEXT | ST_CENTERTITLE;
   pi.fntTitle = AEE_FONT_BOLD;
   pi.fntText = AEE_FONT_NORMAL;
   pi.dwTimeout = 10000;
   if (ISHELL_Prompt(pme->a.m_pIShell, &pi) == FALSE )
     return;

   QP_DrawImage(pme->m_pHdrImage, &pme->m_rectHdr, TRUE);
   IDISPLAY_Update(pme->a.m_pIDisplay);
}

/*===========================================================================
   This function frames the rectangle with CLR_USER_FRAME and
   CLR_SYS_DK_SHADOW
===========================================================================*/
static void QP_FrameRect(IDisplay * pd, AEERect * pRect)
{
   RGBVAL   clr = IDISPLAY_SetColor(pd, CLR_USER_FRAME, CLR_SYS_DK_SHADOW);
   IDISPLAY_FrameRect(pd, pRect);
   IDISPLAY_SetColor(pd, CLR_USER_FRAME, clr);
}

/*===========================================================================
   This function fits the text within the IStatic rectangle so that the
   scroll bar is not drawn.
===========================================================================*/
static void QP_FitStaticText(IDisplay * pd, IStatic * ps, AEEFont font, AECHAR * pszText)
{
   int      nFits;
   AEERect  rect;
   int      nLen = WSTRLEN(pszText);
   AECHAR   chSave = (AECHAR)0;

   ISTATIC_GetRect(ps, &rect);
   IDISPLAY_MeasureTextEx(pd, font, pszText, -1, rect.dx,  &nFits);
   if (nFits < nLen)
   {
      chSave = pszText[nFits];
      pszText[nFits] = (AECHAR)0;
   }
   ISTATIC_SetText(ps, NULL, pszText, AEE_FONT_NORMAL, font);
   if (nFits < nLen)
      pszText[nFits] = chSave;
}

/*===========================================================================
   This function returns the file name portion of the path.
===========================================================================*/
static char * QP_GetFileName(const char * psz)
{
   char *   pszName = STRRCHR(psz, (int)DIRECTORY_CHAR);

   if (pszName)
      pszName++;
   else
      pszName = (char *)psz;

   return pszName;
}

/*===========================================================================
   This function sets the menu attributes based on BREW Style Sheet for
   menu control.
===========================================================================*/
static void QP_SetMenuAttr(IMenuCtl * pMenu, AEECLSID clsMenu, uint16 nColorDepth, AEERect * pRect, uint32 dwProps)
{
   AEEItemStyle    sel, normal;
   AEEMenuColors    col;

   // Menu Style
   normal.ft = MENU8_FT;
   normal.xOffset = 0;
   normal.yOffset = 0;
   normal.roImage = MENU8_RO;

   sel.ft = MENU8_SELECT_FT;
   sel.xOffset = 0;
   sel.yOffset = 0;
   sel.roImage = MENU8_SELECT_RO;

   // Menu Colors
   col.cSelText = MENU8_SELECT_TEXT;
   col.wMask = MENU8_COLOR_MASK;

   if (clsMenu == AEECLSID_MENUCTL)
   {
     col.cBack = MENU8_BACKGROUND;
     col.cSelBack = MENU8_SELECT_BACKGROUND;

      dwProps |= IMENUCTL_GetProperties(pMenu);
   }
   else if (clsMenu == AEECLSID_SOFTKEYCTL || clsMenu == AEECLSID_ICONVIEWCTL)
   {
     col.cBack = TB8_BACKGROUND;
     col.cSelBack = TB8_SELECT_BACKGROUND;

      dwProps |= MP_ICON_TEXT_TOP | MP_NO_ARROWS;
   }

   if (clsMenu == AEECLSID_MENUCTL || clsMenu == AEECLSID_SOFTKEYCTL || clsMenu == AEECLSID_ICONVIEWCTL)
   {
     IMENUCTL_SetStyle(pMenu, &normal, &sel);
     IMENUCTL_SetColors(pMenu, &col);
     IMENUCTL_SetProperties(pMenu, dwProps);
      if (pRect)
         IMENUCTL_SetRect(pMenu, pRect);
   }
}

/*===============================================================================

                     IMedia Callback Function

=============================================================================== */
/*===========================================================================
   This is the main callback notification function for the IMedia object
   within the player window.
   (1) Handles Play, GetTotalTime, GetMediaParm and
       SetMediaParm commands and corresponding status for those commands.
   (2) Updates the ProgCtl, if required.
===========================================================================*/
static void CPlayerWin_MediaNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify)
{
   CPlayerWin *   pme = (CPlayerWin *)pUser;
   uint16         nTextID = 0;
   IMediaVideo*   pMediaVideo = NULL;


   if(!pme || !pCmdNotify)
   {
     DBGPRINTF_ERROR("QP: Null ptr pUsr = 0x%x, pCmdNotify = 0x%x", pme, pCmdNotify);
     return;
   }

   if(pme->m_bMediaVideo)
   {
     pMediaVideo = QP_GetVideoInterface(pme->m_pMedia);
     if(NULL == pMediaVideo)
     {
       DBGPRINTF_ERROR("QP:failed to get IMediaVideo.");
       return;
     }
   }

   switch (pCmdNotify->nStatus)
   {
      case MM_STATUS_SEEK_FAIL:
      case MM_STATUS_PAUSE_FAIL:
      case MM_STATUS_RESUME_FAIL:
         nTextID = IDS_ERR_PLAYCTL_CMD;
         break;
   }

   if (pCmdNotify->nCmd == MM_CMD_PLAY)
   {
      if(pCmdNotify->nCmd == MM_CMD_PLAY)
      {
        nTextID = IDS_PLAY;
      }
      else
      {
         return;
      }

      switch (pCmdNotify->nStatus)
      {
         case MMV_STATUS_PLAYBACK_READY:
            break;

         case MM_STATUS_START:
            IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_STOP);
            break;

         case MM_STATUS_MEDIA_SPEC:
            // print out some IMediaVideo debug information
            if(pme->m_bMediaVideo)
            {
               int32 size;
               int32 value;
               int ret;
               double framerate = 0.0;
               char buf[64];
               AECHAR buf1[64];

               DBGPRINTF_HIGH("QP: SPEC is available.");

               size = 32;

               // duration
               ret = IMediaVideo_GetSPEC(pMediaVideo, MMV_SPEC_DURATION, &value, &size);
               DBGPRINTF_HIGH("QP: SPEC MMV_SPEC_DURATION ret = %d, val = %d.", ret, value);

               // file sie
               ret = IMediaVideo_GetSPEC(pMediaVideo, MMV_SPEC_FILE_SIZE, &value, &size);
               DBGPRINTF_HIGH("QP: SPEC MMV_SPEC_FILE_SIZE ret = %d, val = %d.", ret, value);

               // codec type
               ret = IMediaVideo_GetSPEC(pMediaVideo, MMV_SPEC_VIDEO_CODEC_TYPE, &value, &size);
               DBGPRINTF_HIGH("QP: SPEC MMV_SPEC_VIDEO_CODEC_TYPE ret = %d, val = %d.", ret, value);

               // framt rate
               if(SUCCESS == (ret = IMediaVideo_GetSPEC(pMediaVideo,MMV_SPEC_VIDEO_FRAMERATE, (int32 *)&framerate, (int32 *)&size)))
               {
                 FLOATTOWSTR(framerate, buf1, sizeof(buf1));
                 WSTRTOSTR(buf1, buf, sizeof(buf1));
                 DBGPRINTF_HIGH("QP: MMV_SPEC_VIDEO_FRAMERATE, video frame rate = %s", buf);
               }
               else
               {
                 DBGPRINTF_ERROR("QP: get frame rate error, ret = %d.", ret);
               }
               break;
            }
         case MM_STATUS_TICK_UPDATE:  // Typcally, one-second update
            pme->m_dwPlayPos++;
            if (!pme->m_dwTotalTime) // If no total time increase by 20!
               pme->m_wPct = (pme->m_wPct + 20) % 100;
            break;

         case MM_STATUS_SEEK:
            nTextID = IDS_SEEK;
            if (pCmdNotify->dwSize)
               pme->m_dwPlayPos = (uint32)pCmdNotify->pCmdData / 1000;
            break;

         case MM_STATUS_PAUSE:
            nTextID = IDS_PAUSE;
            if (pCmdNotify->dwSize)
               pme->m_dwPlayPos = (uint32)pCmdNotify->pCmdData / 1000;
            break;

         case MM_STATUS_RESUME:
            if (pCmdNotify->dwSize)
               pme->m_dwPlayPos = (uint32)pCmdNotify->pCmdData / 1000;
            break;

         case MM_STATUS_DONE:    // playback done
         case MM_STATUS_ABORT:   // playback aborted
         {
            nTextID = (pCmdNotify->nStatus == MM_STATUS_DONE) ? IDS_STOP : IDS_ABORT;
            pme->m_bPlayRec = FALSE;
            pme->m_dwPlayPos = 0;
            pme->m_wPct = 0;

            IMENUCTL_SetSel(pme->m_pPlayerMenu, IDM_PM_PLAY);
            CQPlayer_Redraw(pme->m_pOwner, TRUE);
            break;
         }
      }

      CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
   } // MM_CMD_PLAY
   else if (pCmdNotify->nCmd == MM_CMD_GETTOTALTIME)
   {
      DBGPRINTF_LOW("QP: received total time notify.");
      if (pCmdNotify->nStatus == MM_STATUS_DONE || pCmdNotify->nStatus == MM_STATUS_ABORT)
      {
         pme->m_dwTotalTime = ((uint32)pCmdNotify->pCmdData) / 1000;
         if (pme->m_bPlaypending && AEE_SUCCESS == IMEDIA_Play((IMedia*)pme->m_pMedia))
            pme->m_bPlayRec = TRUE;
         pme->m_bPlaypending = FALSE;

         CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, nTextID);
      }
   } // MM_CMD_GETTOTALTIME
   else if (pCmdNotify->nCmd == MM_CMD_SETMEDIAPARM)
   {
      if (pCmdNotify->nSubCmd == MM_PARM_VOLUME && pCmdNotify->nStatus == MM_STATUS_DONE)
      {
         int nRet;
         nRet = IMEDIA_GetMediaParm((IMedia*)pme->m_pMedia, MM_PARM_VOLUME, NULL, NULL);
         DBGPRINTF_MED("MP: Retrieve VOLUME, nRet = %d", nRet);
         return;
      }
   } // MM_CMD_SETMEDIAPARM
   else if (pCmdNotify->nCmd == MM_CMD_GETMEDIAPARM) // IMEDIA_GetMediaParm() events
   {
      if (pCmdNotify->nSubCmd == MM_PARM_VOLUME && pCmdNotify->nStatus == MM_STATUS_DONE)
      {
         pme->m_wVolume = (uint16)(uint32)pCmdNotify->pCmdData;

         DBGPRINTF_MED("MP: Volume set, vol = %d", pme->m_wVolume);

         pme->m_bProgTimer = FALSE;
         CPlayerWin_UpdateProgCtl(pme, pCmdNotify->nCmd, pCmdNotify->nSubCmd, IDS_VOLUME);
         pme->m_bProgTimer = TRUE;
         // Set the prog timer so that the display stays for few seconds.
         ISHELL_SetTimer(pme->m_pIShell, QP_PROG_TIMER, (PFNNOTIFY)CPlayerWin_ProgTimerNotify, pme);
      }
   }  // MM_CMD_GETMEDIAPARM
}

/*===========================================================================
   This function updates the ProgCtl if full screen mode is not active
   or if progress timer is not running.
===========================================================================*/
static void CPlayerWin_UpdateProgCtl(CPlayerWin * pme, int nCmd, int nSubCmd, uint16 wResID)
{
   char     sz[32];
   AECHAR   szBuf[32];

   if (QP_ISFULLSCRN(pme))
      return;

   if (pme->m_bProgTimer)
      return;

   if ( !ISHELL_LoadResString(pme->m_pIShell, QPLAYER_RES_FILE, wResID, pme->m_szText, sizeof(pme->m_szText)) )
      pme->m_szText[0] = (AECHAR)0;

   if (nCmd == MM_CMD_PLAY)
   {
      SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d", pme->m_dwPlayPos/60, pme->m_dwPlayPos%60, pme->m_dwTotalTime/60, pme->m_dwTotalTime%60);
      STRTOWSTR(sz, szBuf, sizeof(szBuf));
      WSTRLCAT(pme->m_szText, szBuf, sizeof(pme->m_szText) - WSTRLEN(pme->m_szText));

      if (pme->m_dwTotalTime)
         pme->m_wPct = (uint16)((100 * pme->m_dwPlayPos) / pme->m_dwTotalTime);
   }
   else if (nCmd == MM_CMD_GETTOTALTIME)
   {
      SNPRINTF(sz, sizeof(sz), " %02d:%02d/%02d:%02d", pme->m_dwPlayPos/60, pme->m_dwPlayPos%60, pme->m_dwTotalTime/60, pme->m_dwTotalTime%60);
      STRTOWSTR(sz, szBuf, sizeof(szBuf));
      WSTRLCAT(pme->m_szText, szBuf, sizeof(pme->m_szText) - WSTRLEN(pme->m_szText));
   }
   else if (nCmd == MM_CMD_GETMEDIAPARM && nSubCmd == MM_PARM_VOLUME)
   {
      SNPRINTF(sz, sizeof(sz), "[%u]", pme->m_wVolume);
      STRTOWSTR(sz, szBuf, sizeof(szBuf));
      WSTRLCAT(pme->m_szText, szBuf, sizeof(pme->m_szText) - WSTRLEN(pme->m_szText));
      pme->m_wPct = (100 * pme->m_wVolume) / AEE_MAX_VOLUME;
   }
   else
   {
      pme->m_wPct = 0;
   }

   CProgCtl_SetPos(&pme->m_ProgCtl, pme->m_szText, pme->m_wPct);

   IDISPLAY_Update(pme->m_pIDisplay);
}

/*===========================================================================
   This clears the progress timer.
===========================================================================*/
static void CPlayerWin_ProgTimerNotify(CPlayerWin * pme)
{
   pme->m_bProgTimer = FALSE;
   if (!pme->m_bPlayRec)
      CQPlayer_Redraw(pme->m_pOwner, TRUE);
}

/*===========================================================================
   This function redraws when the image is fully decoded.
===========================================================================*/
static void CPlayerWin_ImageNotify(void * pUser, IImage * pImage, AEEImageInfo * pi, int nErr)
{
   CPlayerWin *   pme = (CPlayerWin *)pUser;

   if (pme->m_pImage == pImage && !nErr)
      CQPlayer_Redraw(pme->m_pOwner, FALSE);
}

/*===========================================================================
   This function releases IBase.
===========================================================================*/
static void QP_FreeIF(IBase ** ppif)
{
   if (ppif && *ppif)
   {
      IBASE_Release(*ppif);
      *ppif = NULL;
   }
}

/*===========================================================================
   This function releases IWindow.
===========================================================================*/
static void QP_FreeWin(IWindow ** ppif)
{
   if (ppif && *ppif)
   {
      IWINDOW_Delete(*ppif);
      *ppif = NULL;
   }
}

/*===========================================================================
   This function create a IMedia based media object
===========================================================================*/
static int QP_CreateMedia(IShell * ps, AEEMediaData * pmd, IMedia ** ppm)
{
   AEECLSID    cls;
   int         nRet;
   IMedia *    pMedia;

   if (!ps || !pmd || !pmd->pData || !ppm)
      return EBADPARM;

   *ppm = NULL;

   // Find the class ID. This function uses ISHELL_DetectType() API to get the IMedia
   // MIME and subsequently class ID...
   nRet = QP_FindClass(ps, pmd, &cls);
   if (nRet != SUCCESS)
      return nRet;


   // Create IMedia-based object
   if (SUCCESS != ISHELL_CreateInstance(ps, cls, (void **)&pMedia) || !pMedia)
      return MM_ENOMEDIAMEMORY;

   *ppm = pMedia;
   return SUCCESS;
}

/*===========================================================================
   Since IMediaVideo class handlers are registered using AEECLSID_MediaVideo
   as a base class ID, we need to customize AEEMediaUtil_FindClass to search
   MediaVideo based MIME types
===========================================================================*/
static int QP_FindClass(IShell * ps, AEEMediaData * pmd, AEECLSID * pCls)
{
   void *      pBuf = NULL;
   uint32      dwReqSize = 0;
   AEECLSID    cls;
   char        szMIME[32 + 1];
   const char *cpszMIME;
   int         nErr = SUCCESS;

   *pCls = 0;

   if (ENEEDMORE != ISHELL_DetectType(ps, NULL, &dwReqSize, NULL, NULL) || !dwReqSize)
         return EFAILED;

   // If it is file name, then first check the extension.
   // If there is no extension, then read the file data and
   // analyze it to determine if it is MIDI, QCP, PMD or MP3 format.
   if (pmd->clsData == MMD_FILE_NAME)
   {
      IFileMgr *  pfm;
      IFile *     pIFile;
      int         nRead;
      char *      psz = (char *)pmd->pData;
      char *      pszExt;

      // Check the extension...

      pszExt = STRRCHR(psz, '.');
      if (pszExt)
         pszExt++;

      if (pszExt && *pszExt)
      {
         // Check audio MIME base
         STRLCPY(szMIME, AUDIO_MIME_BASE, sizeof(szMIME));
         STRLCAT(szMIME, pszExt, (sizeof(szMIME) - STRLEN(szMIME)));
         cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, szMIME);
         if (cls)
         {
            *pCls = cls;
            return SUCCESS;
         }

         // Check MediaVideo MIME base
         STRLCPY(szMIME, VIDEO_MIME_BASE, sizeof(szMIME));
         STRLCAT(szMIME, pszExt, (sizeof(szMIME) - STRLEN(szMIME)));
         cls = ISHELL_GetHandler(ps, AEECLSID_MediaVideo, szMIME);
         if (cls)
         {
            *pCls = cls;
            return SUCCESS;
         }

         // Check video MIME base also
         STRLCPY(szMIME, VIDEO_MIME_BASE, sizeof(szMIME));
         STRLCAT(szMIME, pszExt, (sizeof(szMIME) - STRLEN(szMIME)));
         cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, szMIME);
         if (cls)
         {
            *pCls = cls;
            return SUCCESS;
         }
      }

      // Now, read the file contents and try to determine the format...

      if (ISHELL_CreateInstance(ps, AEECLSID_FILEMGR, (void **)&pfm))
         return ENOMEMORY;

      pIFile = IFILEMGR_OpenFile(pfm, (char *)pmd->pData, _OFM_READ);
      if (!pIFile)
      {
         QP_RELEASEIF(pfm);
         return ENOMEMORY;
      }

      pBuf = MALLOC(dwReqSize);
      if (!pBuf)
      {
         QP_RELEASEIF(pfm);
         return ENOMEMORY;
      }

      nRead = IFILE_Read(pIFile, pBuf, dwReqSize);

      QP_RELEASEIF(pIFile);
      QP_RELEASEIF(pfm);

      if (nRead < (int)dwReqSize)
      {
         FREE(pBuf);
         return MM_ENOTENOUGHTDATA;
      }

   } // if (pmd->clsData == MMD_FILE_NAME)
   else if (pmd->clsData == MMD_BUFFER)
   {
      if (pmd->dwSize >= (uint32)dwReqSize)
         pBuf = pmd->pData;
      else
         return MM_ENOTENOUGHTDATA;
   }
   else if (pmd->clsData >= MMD_ISOURCE) // Streaming is asynchronous -- return.
   {
      ISource *   pSource = (ISource *)pmd->pData;
      IPeek *     pPeek;
      char *      pPeekedBuf;
      int32       lAvailable;
      int32       lCode;

      // Check if ISource is IPeek
      if (SUCCESS != ISOURCE_QueryInterface(pSource, AEECLSID_PEEK, (void **)&pPeek) || !pPeek)
         return MM_EBADMEDIADATA;

      // Peek to see if some data is available
      pPeekedBuf = IPEEK_Peek(pPeek, 0, &lAvailable, &lCode);
      if (lCode == IPEEK_END || lCode == IPEEK_ERROR || lCode == IPEEK_FULL)
         QP_PEEK_RETURN(pPeek, MM_EPEEK);

      // Peek till required data is fetched
      while (lAvailable < (long)dwReqSize)
      {
         pPeekedBuf = IPEEK_Peek(pPeek, dwReqSize - lAvailable, &lAvailable, &lCode);
         if (lCode == IPEEK_END || lCode == IPEEK_ERROR || lCode == IPEEK_FULL)
            QP_PEEK_RETURN(pPeek, MM_EPEEK);

         // Indicate the caller to do IPEEK_Peekable() and call this function again...
         if (lCode == IPEEK_WAIT)
            QP_PEEK_RETURN(pPeek, ENEEDMORE);
      }

      if (!pPeekedBuf)
         QP_PEEK_RETURN(pPeek, MM_EPEEK);


      pBuf = MALLOC(dwReqSize);
      if (!pBuf)
         QP_PEEK_RETURN(pPeek, ENOMEMORY);

      MEMCPY(pBuf, pPeekedBuf, dwReqSize);

      QP_RELEASEIF(pPeek);
   }

   nErr = ISHELL_DetectType(ps, pBuf, &dwReqSize, NULL, &cpszMIME);

   if (!nErr)
   {
      cls = ISHELL_GetHandler(ps, AEECLSID_MediaVideo, cpszMIME);
      if (cls)
      {
         *pCls = cls;
      }
      else
      {
        cls = ISHELL_GetHandler(ps, AEECLSID_MEDIA, cpszMIME);
        if (cls)
        {
          *pCls = cls;
        }
      }
   }

   if (pBuf && pBuf != pmd->pData)
      FREE(pBuf);

   return nErr;
}

// this function will return IMediaVideo ptr if pMedia is a IMediaVideo based
static IMediaVideo* QP_GetVideoInterface(IMedia* pMedia)
{
  IMediaVideo* pMediaVideo;

  if(NULL == pMedia)
    return NULL;

  if(SUCCESS == IMEDIA_QueryInterface(pMedia, AEEIID_IMediaVideo, (void **)&pMediaVideo))
  {
    (void)IMEDIA_Release(pMedia);
    // this is IMediaVideo based object
    return pMediaVideo;
  }
  else
  {
    return NULL;
  }
}

/*===========================================================================
   This function sends a media command notification.
===========================================================================*/
static void QP_SendMediaNotify(CPlayerWin *pme, int nCmd, int nSubCmd, int nStatus,
                               void *pData, uint32 dwSize)
{
  AEEMediaCmdNotify mcn;
  AEECLSID  cls;

  if (pme->m_pMedia)
  {
    (void)IMEDIA_GetClassID(pme->m_pMedia, &cls);
    mcn.clsMedia = cls;
  }
  else
  {
    mcn.clsMedia = 0;
  }
  mcn.pIMedia = pme->m_pMedia;
  mcn.nCmd = nCmd;
  mcn.nSubCmd = nSubCmd;
  mcn.nStatus = nStatus;
  mcn.pCmdData = pData;
  mcn.dwSize = dwSize;

  CPlayerWin_MediaNotify(pme, &mcn);
}

/*========================End of File ================================================*/
