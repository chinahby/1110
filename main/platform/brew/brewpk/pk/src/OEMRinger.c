/*======================================================
FILE:  OEMRinger.c

SERVICES:  OEM MIDI/MP3 Ringer Interface

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "OEMFeatures.h"
#if defined(OEMRINGERS)

#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEERinger.h"
#include "AEEStdLib.h"
#include "AEEMimeTypes.h"
#ifdef CUST_EDITION
#include "OEMCFGI.h"
#include "OEMRinger.h"
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define OEM_RING_BUFF   0x400

#ifdef DOT_SEPARATOR // {
#undef DOT_SEPARATOR
#endif // } DOT_SEPARATOR
#define DOT_SEPARATOR   "."

/*===========================================================================

                     OEM supported ringer formats table definition
NOTE TO OEMs:
(1) OEMs can customize this table and put only those entries that are 
    supported as ringers by the device.

(2) To add new media format as ringer, say, ".foo"
    (1) Start the format IDs from (AEE_SOUNDPLAYER_FILE + 1)
    (2) Do not return this value to user in response to IRINGERMGR_GetFormats() API. Instead 0 (AEE_SOUNDPLAYER_FILE_UNKNOWN)
        should be returned as format ID.
    (3) Return the gnFormats as the number of formats supported in response to IRINGERMGR_GetNumFormats()

===========================================================================*/
typedef struct RingerFormat
{
   uint8       nFormat;
   AEECLSID    cls;
   char *      pszFile;
} RingerFormat;

RingerFormat   gFormats[] =
{
   // All AEESoundPlayerFile enums
   {AEE_SOUNDPLAYER_FILE_MIDI,      AEECLSID_MEDIAMIDI,     AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "mid"},
   {AEE_SOUNDPLAYER_FILE_MP3,       AEECLSID_MEDIAMP3,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "mp3"},

// {AEE_SOUNDPLAYER_FILE_QCP,       AEECLSID_MEDIAQCP,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "qcp"},
#ifdef FEATURE_AUDFMT_SMAF
   {AEE_SOUNDPLAYER_FILE_MMF,       AEECLSID_MEDIAMMF,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "mmf"},
   {AEE_SOUNDPLAYER_FILE_PHR,       AEECLSID_MEDIAPHR,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "spf"},
   {AEE_SOUNDPLAYER_FILE_IMY,       AEECLSID_MEDIAIMELODY,  AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "imy"},
#endif

// {AEE_SOUNDPLAYER_FILE_ADPCM,     AEECLSID_MEDIAADPCM,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "wav"},
// {AEE_SOUNDPLAYER_FILE_PCM,       AEECLSID_MEDIAPCM,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "wav"},
// {AEE_SOUNDPLAYER_FILE_AAC,       AEECLSID_MEDIAAAC,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "aac"},
// {AEE_SOUNDPLAYER_FILE_AMR,       AEECLSID_MEDIAAMR,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "amr"},

#ifdef FEATURE_WEBAUDIO
   {AEE_SOUNDPLAYER_FILE_PMD,       AEECLSID_MEDIAPMD,      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "pmd"},
#endif
#ifdef FEATURE_BREW_QTV_RINGERS
   {AEE_SOUNDPLAYER_FILE_MPEG4,     AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "mp4"},
   {AEE_SOUNDPLAYER_FILE_3GP,       AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "3gp"},
   {AEE_SOUNDPLAYER_FILE_3G2,       AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "3g2"},
   {AEE_SOUNDPLAYER_FILE_MPEG4,     AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "amc"},
   {AEE_SOUNDPLAYER_FILE_MPEG4,     AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "k3g"},
   {AEE_SOUNDPLAYER_FILE_MPEG4,     AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "skm"},
   {AEE_SOUNDPLAYER_FILE_MPEG4,     AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "m4a"},

   {AEE_SOUNDPLAYER_FILE_WMA,       AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "wma"},
   {AEE_SOUNDPLAYER_FILE_WMV,       AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "wmv"},
   {AEE_SOUNDPLAYER_FILE_ASF,       AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "asf"},
   {AEE_SOUNDPLAYER_FILE_RA,        AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "ra"},
   {AEE_SOUNDPLAYER_FILE_RM,        AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "rm"},
   {AEE_SOUNDPLAYER_FILE_RM,        AEECLSID_MEDIAMPEG4,    AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "ram"},
#endif //FEATURE_BREW_QTV_RINGERS
// {AEE_SOUNDPLAYER_FILE_MOV,       0,                      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "mov"},
// {AEE_SOUNDPLAYER_FILE_QT,        0,                      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "qt"},
// {AEE_SOUNDPLAYER_FILE_AVI,       0,                      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "avi"},
// {AEE_SOUNDPLAYER_FILE_AIFF,      0,                      AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "aiff"},

   //
   // NOTE TO OEMs:
   // Add new supported formats here...refer to the following sample ".foo" entry.
   //
// {AEE_SOUNDPLAYER_FILE_LAST + 1,  AEECLSID_MEDIAFOO,     AEE_RINGER_DIR DIRECTORY_STR "*" DOT_SEPARATOR "foo"}
};

int   gnFormats = sizeof(gFormats) / sizeof(RingerFormat);

#define ISPREDEFINED(eFmt)   ((eFmt) > AEE_SOUNDPLAYER_FILE_UNKNOWN && (eFmt) < AEE_SOUNDPLAYER_FILE_LAST)
#define FORMATID(nIdx)        (ISPREDEFINED(gFormats[(nIdx)].nFormat) ? (AEESoundPlayerFile)gFormats[(nIdx)].nFormat : AEE_SOUNDPLAYER_FILE_UNKNOWN)

/*===========================================================================

                     CRingerMgr class

===========================================================================*/
typedef struct RingerMgrAppInfo RingerMgrAppInfo;

OBJECT(CRingerMgr)
{
   DECLARE_VTBL(IRingerMgr)

   // private object data;

   IShell *             m_pShell;
   long                 m_nRefs;             // reference count

   // List of apps that have an instance of IRingerMgr
   RingerMgrAppInfo    *appList; 

//   int                  m_nFormats;
//   AEESoundPlayerFile   m_formats[16];  // Max formats - MID, MP3

   
   AEERingerID          m_enumRinger;
   int                  m_nEnumFormatIdx;
   AEERingerCatID       m_enumCat;

   // Ringer Creation...

   AECHAR *             m_pszName;
   AEESoundPlayerFile   m_eFormat;
   int32                m_nBytes;

   IFile *              m_pFile;
   IAStream *           m_pStream;
   byte  *              m_pBuff;
   char *               m_pszFile;

   // Playback stuff...

   ISoundPlayer *       m_pPlayer;
   uint32               m_dwDelay;
   boolean              m_bPlaying;
   boolean				m_bInRelease;
   IFile               *m_pROMFile;
 
};

struct RingerMgrAppInfo {
   ACONTEXT         *appCtx;       // App context
   long              nRefs;        // reference count for this app context
   AEECallback       cbSysObject;  // Applet cleanup callback
   IFileMgr         *m_pfm;        // IFileMgr for this applet.

   PFNRINGEREVENT    m_pfnEvent; // Each app can do RegisterNotify
   void  *           m_pEvent;

   RingerMgrAppInfo *pNext;
};

typedef boolean (*PFNRINGERFIND)(CRingerMgr * pme, AEERingerInfo * pri, uint32 dw);

static IBase *       CRingerMgr_New(IShell * ps);

static uint32        CRingerMgr_AddRef(IRingerMgr * po);
static uint32        CRingerMgr_Release(IRingerMgr * po);
static void          CRingerMgr_RegisterNotify(IRingerMgr * po, PFNRINGEREVENT pfn,void * pUser);
static int           CRingerMgr_GetNumberFormats(IRingerMgr * po);
static int           CRingerMgr_GetFormats(IRingerMgr * po, AEESoundPlayerFile * pwFormat, int nCount);
static int           CRingerMgr_SetRinger(IRingerMgr * po, AEERingerCatID idCat, AEERingerID id);
static int           CRingerMgr_EnumCategoryInit(IRingerMgr * po);
static boolean       CRingerMgr_EnumNextCategory(IRingerMgr * po, AEERingerCat * pc);
static int           CRingerMgr_EnumRingerInit(IRingerMgr * po);
static boolean       CRingerMgr_EnumNextRinger(IRingerMgr * po,AEERingerInfo * pi);
static AEERingerID   CRingerMgr_GetRingerID(IRingerMgr * po, const char * pszFile);
static int           CRingerMgr_GetRingerInfo(IRingerMgr * po, AEERingerID id, AEERingerInfo * pi);
static int           CRingerMgr_Create(IRingerMgr * po,const AECHAR * pszName, AEESoundPlayerFile format, IAStream * pStream);
static int           CRingerMgr_Remove(IRingerMgr * po, AEERingerID id);
static int           CRingerMgr_Play(IRingerMgr * po,AEERingerID id,const char * pszFile, IAStream * pStream,uint32 dwPause);
static int           CRingerMgr_Stop(IRingerMgr * po);

static int           CRingerMgr_CreateRinger(CRingerMgr * pme);
static int           CRingerMgr_DetectType(CRingerMgr * pme);
static boolean       CRingerMgr_FinishCreate(CRingerMgr * pme,AEERingerID id, int nErr);
static boolean       CRingerMgr_Find(IRingerMgr * po, AEERingerInfo * pri, PFNRINGERFIND pfn, uint32 dwData);
static boolean       CRingerMgr_FileMatch(CRingerMgr * pme, AEERingerInfo * pri,char * pszMatch);
static boolean       CRingerMgr_IDMatch(CRingerMgr * pme, AEERingerInfo * pri, AEERingerID id);
static int           CRingerMgr_WriteFile(CRingerMgr * pme);
static void          CRingerMgr_SoundStatus(void * pUser, AEESoundPlayerCmd  eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam);

static boolean       CRingerMgr_Notify(CRingerMgr * pme, AEERingerEvent evt, uint32 dw, int nErr);

static void          FormatRingerName(char * pszIn, AECHAR * pszDest, int nSize);
static boolean       FormatFileName(AECHAR * pszIn, char * pszDest, int nSize,AEESoundPlayerFile t);
static void          RingerMgr_FreeAppInfo(RingerMgrAppInfo *thisInfo);
static IFileMgr     *RingerMgr_GetFileMgr(void);

static const VTBL(IRingerMgr) gCRingerMgrFuncs = {CRingerMgr_AddRef,
                                                   CRingerMgr_Release,
                                                   CRingerMgr_RegisterNotify,
                                                   CRingerMgr_GetNumberFormats,
                                                   CRingerMgr_GetFormats,
                                                   CRingerMgr_SetRinger,
                                                   CRingerMgr_EnumCategoryInit,
                                                   CRingerMgr_EnumNextCategory,
                                                   CRingerMgr_EnumRingerInit,
                                                   CRingerMgr_EnumNextRinger,
                                                   CRingerMgr_GetRingerID,
                                                   CRingerMgr_GetRingerInfo,
                                                   CRingerMgr_Create,
                                                   CRingerMgr_Remove,
                                                   CRingerMgr_Play,
                                                   CRingerMgr_Stop};

#define CAT_PERSONAL    1
#define CAT_BUSINESS    2
#define CAT_MAX         (CAT_BUSINESS)

#define AEE_RINGER_CATEGORY_NONE ((AEERingerCatID)0xffffffff)

static const char *     gpszCategories[] = {"All","Personal","Business",NULL};
static AEERingerID      gCategoryRings[] = {0,    0,         0};
static CRingerMgr       gMgr = {0};

/*===========================================================================


===========================================================================*/
int Ringer_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   IBase * po;

   if(ClsId != AEECLSID_RINGERMGR)
      return(EBADPARM);
   po = CRingerMgr_New(pIShell);

   *ppObj = po;

   if(po)
      return(0);

   return(ENOMEMORY);
}

/*===========================================================================

                      RINGER CLASS

===========================================================================*/

/*===========================================================================


===========================================================================*/
static IBase * CRingerMgr_New(IShell * ps)
{
   if(!gMgr.m_nRefs){
      INIT_VTBL(&gMgr, IRingerMgr, gCRingerMgrFuncs);

      gMgr.m_nRefs = 1;
      gMgr.m_pShell = ps;
		gMgr.m_enumCat = AEE_RINGER_CATEGORY_NONE;
		gMgr.m_enumRinger = AEE_RINGER_ID_NONE;
   }
   else
      gMgr.m_nRefs++;

   //
   // Each applet needs it's own IFileMgr interface and each applet
   // can register for notifications
   //
   {
      RingerMgrAppInfo *info;
      ACONTEXT        *appCtx = AEE_GetAppContext();

      // Run through the existing app info list, looking for one that 
      // matches the current app context.
      info = gMgr.appList;
      while ( (info != NULL) && (info->appCtx != appCtx) ) {
         info = info->pNext;
      }

      if (NULL == info) {
         //
         // Didn't find it.  Lets create a new one:
         //

         // Allocate some memory...
         info = AEEHeap_Realloc(NULL, sizeof(*info));

         if (info != NULL) {
            info->appCtx = appCtx;
            info->nRefs = 1;
            info->m_pfnEvent = NULL;
            info->m_pEvent = NULL;

            // Allow BREW to manage the app info for us, so that it will
            // be released if the applet exists without releasing all 
            // its references to us.
            CALLBACK_Init(&info->cbSysObject, 
                          (PFNNOTIFY) RingerMgr_FreeAppInfo, 
                          (void *) info);

            AEE_LinkSysObject(&info->cbSysObject);
         
            ISHELL_CreateInstance(ps, AEECLSID_FILEMGR, (void **) &info->m_pfm);

            // Only try to create the ringer dir if we are the first instance
            // of the IRingerMgr
            if (info->m_pfm && gMgr.m_nRefs == 1) {
               // don't need to test, will fail later
               IFILEMGR_MkDir(info->m_pfm,AEEFS_RINGERS_DIR);
            }

            // Insert it at the front of the singly-linked list.
            info->pNext = gMgr.appList;
            gMgr.appList = info;
         }
      } else {
         info->nRefs++;
      }
   }


   return((IBase *)&gMgr);
}

/*===========================================================================


===========================================================================*/
static uint32  CRingerMgr_AddRef(IRingerMgr * po)
{
   RingerMgrAppInfo *info;
   ACONTEXT         *appCtx = AEE_GetAppContext();

   // Run through the existing app info list, looking for one that 
   // matches the current app context.
   info = gMgr.appList;
   while ( (info != NULL) && (info->appCtx != appCtx) ) {
      info = info->pNext;
   }

   if (NULL == info) {
      // Didn't find the app info, this is bad.  Either
      //
      // 1. OEMRinger.c has a bug.
      // 2. Somebody is sharing an IRingerMgr interface between
      //    app contexts.
      //
      DBGPRINTF_ERROR("Do not share IRingerMgr between app contexts!");
   } else {
      info->nRefs++;
   }

   return(++((CRingerMgr *)po)->m_nRefs);
}

/*===========================================================================


===========================================================================*/
static uint32 CRingerMgr_Release(IRingerMgr * po)
{
   CRingerMgr * pme = (CRingerMgr *)po;

   if(pme->m_nRefs){

      if(--pme->m_nRefs) {
         { 
            RingerMgrAppInfo *info;
            ACONTEXT         *appCtx = AEE_GetAppContext();

            // Run through the existing app info list, looking for one that 
            // matches the current app context.
            info = gMgr.appList;
            while ( (info != NULL) && (info->appCtx != appCtx) ) {
               info = info->pNext;
            }

            if (info != NULL) { // found it.

               // Need to decrement the ref count of this app context.  If it
               // hits zero then the RingerMgrAppInfo structure will be
               // freed for this app (meaning that it does not have any more
               // IRingerMgr references).
               if (info->nRefs > 1) {
                  info->nRefs--;
               } else {
                  RingerMgr_FreeAppInfo(info);
               }
            }
         }

         return(pme->m_nRefs);
      }


	  if(pme->m_bInRelease)
		  return(0);
	  
	  pme->m_bInRelease = TRUE;
         
      CRingerMgr_Stop(po);

      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EINCOMPLETEITEM);
      
      // Free all the IFileMgr interfaces for all the applets.
      {
         RingerMgrAppInfo *info, *oldInfo;

         // Release the app info
         info = pme->appList;
         while (info != NULL) {

            // Cancel the callback.
            CALLBACK_Cancel(&info->cbSysObject);

            if (info->m_pfm) {
               IFILEMGR_Release(info->m_pfm);
               info->m_pfm = NULL;
            }

            oldInfo = info;
            info = info->pNext;

            // Free the memory
            (void) AEEHeap_Realloc(oldInfo, 0);
         }
      }

      MEMSET(&gMgr,0,sizeof(gMgr));

	  pme->m_bInRelease = FALSE;
   }
   return(0);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_GetNumberFormats(IRingerMgr * po)
{
   return(gnFormats);
}

/*===========================================================================


===========================================================================*/
static void CRingerMgr_RegisterNotify(IRingerMgr * po, PFNRINGEREVENT pfn,void * pUser)
{
   CRingerMgr * pme = (CRingerMgr *)po;

   ACONTEXT *appCtx = AEE_GetAppContext();

   // Run through the existing app info list, looking for one that 
   // matches the current app context.
   RingerMgrAppInfo * info = pme->appList;
   while ( (info != NULL) && (info->appCtx != appCtx) ) {
      info = info->pNext;
   }

   if ( info != NULL ) {

      info->m_pfnEvent = pfn;
      info->m_pEvent = pUser;

   }

}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_GetFormats(IRingerMgr * po, AEESoundPlayerFile * pwFormat, int nCount)
{
//   CRingerMgr * pme = (CRingerMgr *)po;

   if(nCount && pwFormat)
   {
      if(nCount >= gnFormats)
      {
         int   i;

         for (i = 0; i < gnFormats; i++)
            pwFormat[i] = FORMATID(i);
         return(0);
      }
   }
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_SetRinger(IRingerMgr * po, AEERingerCatID idCat, AEERingerID id)
{
   AEERingerInfo  ri;

   if(!CRingerMgr_GetRingerInfo(po, id, &ri)){
#if defined(OEM_RINGER_SUPPORT)
      return(OEM_SetActiveRinger(idCat,id,ri.szFile));
#else
      if( idCat <= CAT_MAX ){
         gCategoryRings[idCat] = id;
         return(SUCCESS);
      }
#endif
   }
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_EnumCategoryInit(IRingerMgr * po)
{
   CRingerMgr * pme = (CRingerMgr *)po;

   pme->m_enumCat = AEE_RINGER_CATEGORY_ALL;
   return(0);
}

/*===========================================================================


===========================================================================*/
static boolean CRingerMgr_EnumNextCategory(IRingerMgr * po, AEERingerCat * pc)
{
   CRingerMgr *   pme = (CRingerMgr *)po;
   char *         pszName;

   if(!pc)
      return(FALSE);

   MEMSET(pc,0,sizeof(AEERingerCat));

   if(pme->m_enumCat > CAT_MAX)
      return(FALSE);

   pszName = (char *)gpszCategories[pme->m_enumCat];
   pc->id = pme->m_enumCat;
   STR_TO_WSTR(pszName,pc->szName,sizeof(pc->szName));
#if defined(OEM_RINGER_SUPPORT)
   pc->idRinger = OEM_GetActiveRinger(pc->id, pc->szName);
#else
   pc->idRinger = gCategoryRings[pme->m_enumCat];
#endif
   pme->m_enumCat++;
   return(TRUE);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_EnumRingerInit(IRingerMgr * po)
{
   CRingerMgr  *  pme = (CRingerMgr *)po;
   IFileMgr    *pfm = RingerMgr_GetFileMgr();

   if(gnFormats){
      pme->m_enumRinger = 0;
      pme->m_nEnumFormatIdx = 0;
      for(pme->m_nEnumFormatIdx = 0; pme->m_nEnumFormatIdx < gnFormats; pme->m_nEnumFormatIdx++){
         if(!IFILEMGR_EnumInit(pfm, gFormats[pme->m_nEnumFormatIdx].pszFile, FALSE))
            return(0);
      }         
   }
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
static boolean  CRingerMgr_EnumNextRinger(IRingerMgr * po,AEERingerInfo * pi)
{
   CRingerMgr *   pme = (CRingerMgr *)po;
   boolean        bRet;
   FileInfo       fi;
   IFileMgr    *pfm = RingerMgr_GetFileMgr();

   if (!pi)
      return FALSE;
      
   MEMSET(pi,0,sizeof(AEERingerInfo));

   if(pme->m_enumRinger == AEE_RINGER_ID_NONE)
      return(FALSE);

   while(pme->m_nEnumFormatIdx < gnFormats){

      bRet = IFILEMGR_EnumNext(pfm,&fi);

      if(bRet){
         pi->format = FORMATID(pme->m_nEnumFormatIdx);
         pi->id = ++pme->m_enumRinger;
         STRCPY(pi->szFile,fi.szName);
         FormatRingerName(fi.szName, pi->szName, sizeof(pi->szName));
         return(TRUE);
      }

      for(++pme->m_nEnumFormatIdx; pme->m_nEnumFormatIdx < gnFormats; pme->m_nEnumFormatIdx++){
         if(!IFILEMGR_EnumInit(pfm, gFormats[pme->m_nEnumFormatIdx].pszFile, FALSE))
            break;
      }         
   }
   return(FALSE);
}

/*===========================================================================


===========================================================================*/
static AEERingerID CRingerMgr_GetRingerID(IRingerMgr * po, const char * pszFile)
{
   AEERingerInfo  ri;
   char *         pszIn;

   pszIn = STRRCHR(pszFile,DIRECTORY_CHAR);
   if(pszIn)
      pszIn++;
   else
      pszIn = (char *)pszFile;
   
   if(CRingerMgr_Find(po,&ri,(PFNRINGERFIND)(CRingerMgr_FileMatch),(uint32)pszIn))
      return(ri.id);

   return(AEE_RINGER_ID_NONE);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_GetRingerInfo(IRingerMgr * po, AEERingerID id, AEERingerInfo * pri)
{
   if(CRingerMgr_Find(po,pri,(PFNRINGERFIND)(CRingerMgr_IDMatch),id))
      return(0);
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_Create(IRingerMgr * po,const AECHAR * pszName, AEESoundPlayerFile format, IAStream * pStream)
{
   CRingerMgr *   pme = (CRingerMgr *)po;
   int            nLen = WSTRLEN(pszName)+1;

   CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EINCOMPLETEITEM);

   if(!pStream)
   {
      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EBADPARM);
      return(EBADPARM);
   }

   pme->m_pStream = pStream;
   IASTREAM_AddRef(pStream);
   pme->m_eFormat = format;

   pme->m_pszName = MALLOC(sizeof(AECHAR)*nLen);
   if (!pme->m_pszName)
   {
      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,ENOMEMORY);
      return ENOMEMORY;
   }
   WSTRCPY(pme->m_pszName, pszName);

   pme->m_pBuff = (byte *)MALLOC(OEM_RING_BUFF);
   if(!pme->m_pBuff)
   {
      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,ENOMEMORY);
      return(ENOMEMORY);
   }

   if (ISPREDEFINED(pme->m_eFormat))
      return CRingerMgr_CreateRinger(pme);

   return CRingerMgr_DetectType(pme);
}

static int CRingerMgr_DetectType(CRingerMgr * pme)
{
   const char *   cpszMIME = NULL;
   int32          nBytes;
   int            nErr;

   while (1)
   {
      if (pme->m_nBytes >= OEM_RING_BUFF)
         break;

      nBytes = IASTREAM_Read(pme->m_pStream, pme->m_pBuff + pme->m_nBytes, OEM_RING_BUFF - pme->m_nBytes);
      if(!nBytes)
      {
         CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,ENEEDMORE);
         return(ENEEDMORE);
      }
      if(nBytes == AEE_NET_WOULDBLOCK){
         IASTREAM_Readable(pme->m_pStream,(PFNNOTIFY)(CRingerMgr_DetectType),pme);
         return(AEE_STREAM_WOULDBLOCK);
      }
      
      // nBytes read...
      {
         //
         // NOTE TO OEMs:
         // OEMs can add additional custom type detection code here.
         //
         pme->m_nBytes += nBytes;
         nErr = ISHELL_DetectType(pme->m_pShell, pme->m_pBuff, (uint32 *)&pme->m_nBytes, NULL, &cpszMIME);
         if (!nErr)
            break;
         else if (ENEEDMORE == nErr)
            CRingerMgr_Notify(pme, ARE_WRITE, nBytes, 0);
         else
         {
            CRingerMgr_FinishCreate(pme, AEE_RINGER_ID_NONE, nErr);
            return(nErr);
         }
      }
   } // while

   // MIME found...
   // Now, get the handler and make sure that the handler is a supported ringer type.
   if (cpszMIME)
   {
      int      i;
      uint8    nFormat = 0;
      AEECLSID cls = ISHELL_GetHandler(pme->m_pShell, AEECLSID_MEDIA, cpszMIME);
      if (!cls)
      {
         CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EUNSUPPORTED);
         return(EUNSUPPORTED);
      }
      
      // See if this cls is supported as ringer...
      for (i = 0; i < gnFormats; i++)
      {
         if (cls == gFormats[i].cls)
         {
            nFormat = gFormats[i].nFormat;
            break;
         }
      }

      pme->m_eFormat = (AEESoundPlayerFile)nFormat;
      return CRingerMgr_CreateRinger(pme);
   }

   CRingerMgr_FinishCreate(pme, AEE_RINGER_ID_NONE, EFAILED);
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
//static int CRingerMgr_Create(IRingerMgr * po,const AECHAR * pszName, AEESoundPlayerFile format, IAStream * pStream)
static int CRingerMgr_CreateRinger(CRingerMgr * pme)
{
   char     *     pszFile;
   int            nErr;
   AEEDeviceInfo  di;
   IFileMgr    *pfm = RingerMgr_GetFileMgr();
   IShell* pShell = AEE_GetShell();

   di.wStructSize = sizeof(AEEDeviceInfo);
   ISHELL_GetDeviceInfo(pShell, &di);
   
   pszFile = MALLOC(di.wMaxPath);

   if(!pszFile)
      return EFAILED;

//   CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EINCOMPLETEITEM);

   if(!FormatFileName((AECHAR *)pme->m_pszName, pszFile, di.wMaxPath, pme->m_eFormat)){
      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EFAILED);
      FREE(pszFile);
      return(EFAILED);
   }

   if(!IFILEMGR_Test(pfm,pszFile)){
      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EFILEEXISTS);
      FREE(pszFile);
      return(EFILEEXISTS);
   }

//   if(!pStream){
//      CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EBADPARM);
//      return(EBADPARM);
//   }

   pme->m_pFile = IFILEMGR_OpenFile(pfm,pszFile,_OFM_CREATE);
   if(pme->m_pFile){
//      pme->m_pBuff = (byte *)MALLOC(OEM_RING_BUFF + STRLEN(szFile) + 1);
//      if(!pme->m_pBuff){
//         CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,ENOMEMORY);
//         return(ENOMEMORY);
//      }     
      pme->m_pszFile = (char *)MALLOC(STRLEN(pszFile) + 1);
      if(!pme->m_pszFile){
         CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,ENOMEMORY);
         FREE(pszFile);
         return(ENOMEMORY);
      }     
      STRCPY(pme->m_pszFile, pszFile);
//      pme->m_pStream = pStream;
//      IASTREAM_AddRef(pStream);
      FREE(pszFile);
      return(CRingerMgr_WriteFile(pme));
   }

   nErr = IFILEMGR_GetLastError(pfm);
   CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,nErr);
   FREE(pszFile);
   return(nErr);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_Remove(IRingerMgr * po, AEERingerID id)
{
   AEERingerInfo  ri;
   IFileMgr    *pfm = RingerMgr_GetFileMgr();

   if(!CRingerMgr_GetRingerInfo(po,id,&ri))
      return(IFILEMGR_Remove(pfm,ri.szFile));
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_Play(IRingerMgr * po,AEERingerID id,const char * pszFile,IAStream * pStream,uint32 dwPause)
{
   CRingerMgr *   pme = (CRingerMgr *)po;
   AEERingerInfo  ri;
   IFileMgr    *pfm = RingerMgr_GetFileMgr();

   CRingerMgr_Stop(po);

   // If the ringer was found, then use the file associated with it...

   if(id != AEE_RINGER_ID_NONE && !CRingerMgr_GetRingerInfo(po,id,&ri))
      pszFile = ri.szFile;
   
   if(pszFile || pStream){
      if(!ISHELL_CreateInstance(pme->m_pShell, AEECLSID_SOUNDPLAYER, (void **)(&pme->m_pPlayer))){
         pme->m_dwDelay = dwPause;
         pme->m_bPlaying = TRUE;
         ISOUNDPLAYER_RegisterNotify(pme->m_pPlayer,CRingerMgr_SoundStatus,po);
         if(pszFile) {
            AEESoundPlayerInfo info;
            ZEROAT(&info);

            // This file may actually be a ROM-based file (OEMConstFiles.c). If
            // this is the case then we need to locate it in ROM and use a
            // SDT_BUFFER source instead of SDT_FILE, as BREW ROM-based files
            // are only accessible from within the BREW environment and not from
            // the CMX task
            
            
            // Try to open the file, read only.
            if (pme->m_pROMFile) {
               IFILE_Release(pme->m_pROMFile);
            }
            pme->m_pROMFile = IFILEMGR_OpenFile(pfm, pszFile, _OFM_READ);

            if (pme->m_pROMFile) {
               FileInfo fileInfo;

               // Get the file size
               ZEROAT(&fileInfo);
               (void) IFILE_GetInfo(pme->m_pROMFile, &fileInfo);
               info.dwSize = fileInfo.dwSize;

               // Map the file into memory
               info.pData = IFILE_Map(pme->m_pROMFile,
                                      NULL, 
                                      info.dwSize,
                                      AEE_FMAP_PROT_READ,
                                      AEE_FMAP_SHARED,
                                      0);
            }

            if (info.pData != NULL) {
               int result;

               info.eInput = SDT_BUFFER;
               result = ISOUNDPLAYER_SetInfo(pme->m_pPlayer, &info);

               if (result != SUCCESS) {
                  if (pme->m_pROMFile) {
                     IFILE_Release(pme->m_pROMFile);
                     pme->m_pROMFile = NULL;
                  }
                  return result;
               }

            } else {
               // Normal EFS file...
            ISOUNDPLAYER_Set(pme->m_pPlayer, SDT_FILE, (void *)pszFile);
            }
         } else {
            ISOUNDPLAYER_SetStream(pme->m_pPlayer, pStream);
         }

         ISOUNDPLAYER_Play(pme->m_pPlayer);
         return(0);
      }
   }
   return(EFAILED);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_Stop(IRingerMgr * po)
{
   CRingerMgr *      pme = (CRingerMgr *)po;
   ISoundPlayer *    psp;

   pme->m_dwDelay = 0;

   ISHELL_CancelTimer(pme->m_pShell, NULL, pme);
   
   if (pme->m_pROMFile) {
      IFILE_Release(pme->m_pROMFile);
      pme->m_pROMFile = NULL;
   }

   psp = pme->m_pPlayer;
   if(psp){
      pme->m_pPlayer = NULL;
      ISOUNDPLAYER_Release(psp);
      CRingerMgr_Notify(pme, ARE_PLAY,0,(int)(pme->m_bPlaying ? EINCOMPLETEITEM : 0));
      return(0);
   }
   return(EFAILED);
}

/*===========================================================================

   PRIVATE STUFF

===========================================================================*/
static boolean CRingerMgr_FinishCreate(CRingerMgr * pme,AEERingerID id, int nErr)
{
   IFileMgr    *pfm = RingerMgr_GetFileMgr();
   boolean  bNotify = FALSE;

   if(pme->m_pStream){
      bNotify = TRUE;
      IASTREAM_Release(pme->m_pStream);
      pme->m_pStream = NULL;
   }
      
   if(pme->m_pFile){
      bNotify = TRUE;
      IFILE_Release(pme->m_pFile);
      pme->m_pFile = NULL;
   }

   if(pme->m_pBuff){
      bNotify = TRUE;
      FREE(pme->m_pBuff);
      pme->m_pBuff = NULL;
      pme->m_nBytes = 0;
   }

   if (pme->m_pszFile)
   {
      bNotify = TRUE;
      if(nErr)
         IFILEMGR_Remove(pfm, pme->m_pszFile);
      FREE(pme->m_pszFile);
      pme->m_pszFile = NULL;
   }

   if (pme->m_pszName)
   {
      FREE(pme->m_pszName);
      pme->m_pszName = NULL;
   }

   if(bNotify)
      return(CRingerMgr_Notify(pme, ARE_CREATE,id,nErr));
   return(TRUE);
}

/*===========================================================================


===========================================================================*/
static boolean CRingerMgr_Find(IRingerMgr * po, AEERingerInfo * pri, PFNRINGERFIND pfn, uint32 dwData)
{
   CRingerMgr *   pme = (CRingerMgr *)po;

   // Search each ringer...

   if(!CRingerMgr_EnumRingerInit(po)){
      while(CRingerMgr_EnumNextRinger(po,pri)){
         if(pfn(pme,pri,dwData))
            return(TRUE);
      }
   }
   return(FALSE);
}

/*===========================================================================


===========================================================================*/
static int CRingerMgr_WriteFile(CRingerMgr * pme)
{
   int32    nBytes;
   int      nErr;

   if(pme->m_pStream && pme->m_pFile)
   {
      // Write the bytes already read during type detection.
      if (pme->m_nBytes && IFILE_Write(pme->m_pFile, pme->m_pBuff, pme->m_nBytes) != (uint32)pme->m_nBytes)
      {
         CRingerMgr_Notify(pme, ARE_CREATE,0,EFSFULL);
         return(EFSFULL);
      }

      while(1){
         nBytes = IASTREAM_Read(pme->m_pStream, pme->m_pBuff, OEM_RING_BUFF);
         if(!nBytes){
            AEERingerID    id;
         
   // See if the ringer is valid by searching the list.  If not, the FinishCreate function will
   // remove it for us!!!!!

            IFILE_Release(pme->m_pFile);
            pme->m_pFile = NULL;

            id = CRingerMgr_GetRingerID((IRingerMgr *)pme, pme->m_pszFile);
            if(id != AEE_RINGER_ID_NONE)
               nErr = 0;
            else
               nErr = EFAILED;

            CRingerMgr_FinishCreate(pme,id,nErr);
            return(nErr);
         }
         if(nBytes == AEE_NET_WOULDBLOCK){
            IASTREAM_Readable(pme->m_pStream,(PFNNOTIFY)(CRingerMgr_WriteFile),pme);
            return(AEE_STREAM_WOULDBLOCK);
         }
         if(IFILE_Write(pme->m_pFile,pme->m_pBuff,nBytes) != (uint32)nBytes){
            CRingerMgr_FinishCreate(pme,AEE_RINGER_ID_NONE,EFSFULL);
            return(EFSFULL);
         }
         CRingerMgr_Notify(pme, ARE_WRITE,nBytes,0);
      }
   }
   return(EFAILED);
}
   
/*===========================================================================


===========================================================================*/
static boolean CRingerMgr_IDMatch(CRingerMgr * pme, AEERingerInfo * pri, AEERingerID id)
{
   return((boolean)(pri->id == id));
}

/*===========================================================================


===========================================================================*/
static boolean CRingerMgr_FileMatch(CRingerMgr * pme, AEERingerInfo * pri,char * pszMatch)
{
   char  * pszFound;

   pszFound = STRRCHR(pri->szFile,DIRECTORY_CHAR);
   if(pszFound)
      pszFound++;
   else
      pszFound = pri->szFile;
   return(STRICMP(pszFound, pszMatch) == 0);
}

/*===========================================================================


===========================================================================*/
static void CRingerMgr_Replay(CRingerMgr * pme)
{
   if(pme->m_pPlayer)
      ISOUNDPLAYER_Play(pme->m_pPlayer);
}

/*===========================================================================


===========================================================================*/
static boolean CRingerMgr_Notify(CRingerMgr * pme, AEERingerEvent evt, uint32 dw,int nErr)
{
   ACONTEXT *pold;

   // Run through the existing app info list, send notification
   // to every app that is registered for it.
   RingerMgrAppInfo * info = pme->appList;
   
   while(info != NULL){

      if(info->m_pfnEvent != NULL){
         CRingerMgr_AddRef((IRingerMgr *)pme);
         
         //notifications need to be sent in the context of the app
         //that is getting notified, else any memory they allocate is bugga
         pold = AEE_EnterAppContext(info->appCtx);
         info->m_pfnEvent(info->m_pEvent, evt, dw,nErr);
         AEE_LeaveAppContext(pold);

         if(CRingerMgr_Release((IRingerMgr *)pme) == 0)
            return(FALSE);
      }
      info = info->pNext;
   }
   return(TRUE);
}

/*===========================================================================


===========================================================================*/
static void CRingerMgr_SoundStatus(void * pUser, AEESoundPlayerCmd  eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam)
{
   CRingerMgr * pme = (CRingerMgr *)pUser;
   
   if(eCBType == AEE_SOUNDPLAYER_PLAY_CB){
      if ((eSPStatus == AEE_SOUNDPLAYER_DONE) ||
          (eSPStatus == AEE_SOUNDPLAYER_ABORTED)) {
         pme->m_bPlaying = FALSE;
         if(!CRingerMgr_Notify(pme, ARE_PLAY,0,0))
            return;

         if(pme->m_dwDelay)
            ISHELL_SetTimer(pme->m_pShell, pme->m_dwDelay, (PFNNOTIFY)(CRingerMgr_Replay),pme);
      }
   }
}

/*===========================================================================


===========================================================================*/
static void FormatRingerName(char * pszIn, AECHAR * pszDest, int nSize)
{
   char *   psz, * pszw, ch;
   boolean  bNextCap;

   // Clip the extension...

   psz = STRRCHR(pszIn,'.');
   if(psz)
      *psz = 0;

   // Fast-Forward to the file name...
      
   psz = STRRCHR(pszIn,DIRECTORY_CHAR);
   if(psz)
      *(psz++) = 0;
   else
      psz = pszIn;

   // Capitalize the name...

   for(bNextCap = TRUE, pszw = psz; (ch = *pszw) != 0; pszw++){
      if(ch == '_')
         *pszw = ' ';
      ch = *pszw;
      if(ch == ' ')
         bNextCap = TRUE;
      else{
         if(bNextCap && ch >= 'a' && ch <= 'z')
            *pszw = (ch - ('a' - 'A'));
         bNextCap = FALSE;
      }
   }

   STR_TO_WSTR(psz,pszDest,nSize);
}


/*===========================================================================


===========================================================================*/
static boolean FormatFileName(AECHAR * pszIn, char * pszDest, int nSize,AEESoundPlayerFile t)
{
   int      i;
   char     ch;

   STRCPY(pszDest,(AEE_RINGER_DIR DIRECTORY_STR));
   i        = STRLEN(pszDest);
   pszDest += i;

   WSTR_TO_STR(pszIn,pszDest,nSize - i);

   while((ch = *pszDest) != 0){
      if(ch == ' ')
         *pszDest = '_';
      else{
         if(ch >= 'A' && ch <= 'Z')
            *pszDest = ch + ('a' - 'A');
      }
      pszDest++;
   }

   // Append the extension...
   {
      char *   pszExt;
      int      i;
      boolean  bFound = FALSE;
      
      // Search for t...
      for (i = 0; i < gnFormats; i++)
      {
         if ((uint8)t == gFormats[i].nFormat)
         {
            bFound = TRUE;
            break;
         }
      }

      if (!bFound)
         return FALSE;

      pszExt = STRRCHR(gFormats[i].pszFile, '.');
      if (!pszExt)
         return FALSE;
      STRCPY(pszDest, pszExt);
   }

   return(TRUE);
}

/*===========================================================================


===========================================================================*/
static void RingerMgr_FreeAppInfo(RingerMgrAppInfo *thisInfo)
{
   RingerMgrAppInfo *currInfo, *prevInfo;

   // Ensure the sys callback is canceled
   CALLBACK_Cancel(&thisInfo->cbSysObject);
   
   currInfo = gMgr.appList;
   prevInfo = NULL;

   // Remove "thisInfo" from the RingerMgrAppInfo singly-linked list stored
   // in the RingerMgr object
   while (currInfo != NULL) {
      if (currInfo == thisInfo) {
         if (prevInfo) {
            prevInfo->pNext = currInfo->pNext;
         } else {
            gMgr.appList = currInfo->pNext;
         }
         break;
      }

      prevInfo = currInfo;
      currInfo = currInfo->pNext;
   }

   if (thisInfo->m_pfm) {
      // Release the payload, an IFileMgr interface
      IFILEMGR_Release(thisInfo->m_pfm);
   }

   // Free the memory
   (void) AEEHeap_Realloc(thisInfo, NULL);
}

/*===========================================================================


===========================================================================*/
static IFileMgr *RingerMgr_GetFileMgr(void)
{
   RingerMgrAppInfo *info;
   ACONTEXT        *appCtx = AEE_GetAppContext();

   // Run through the existing app info list, looking for one that 
   // matches the current app context.
   info = gMgr.appList;
   while ( (info != NULL) && (info->appCtx != appCtx) ) {
      info = info->pNext;
   }

   if (NULL == info) {
      return NULL;
   }

   // Do not add a reference to the interface!
   return info->m_pfm;
}

#ifdef OEM_RINGER_SUPPORT
/*===========================================================================


===========================================================================*/
 int OEM_SetActiveRinger(AEERingerCatID idCategory, AEERingerID idRinger, char * szFilename)
{
    byte data;
    // 1st ringer has idRinger value 1 and menu item value should be 0 for this

    data = (byte)idRinger;
    return OEM_SetConfig(CFGI_RINGER_TYPE, &data, sizeof(byte));
}

/*===========================================================================


===========================================================================*/
AEERingerID OEM_GetActiveRinger(AEERingerCatID idRingerCat, AECHAR * szwName)
{
    byte data;

    if (OEM_GetConfig(CFGI_RINGER_TYPE, &data, sizeof(byte)) != SUCCESS)
    {
        return 1;
    }
    else
    {
        return data;
    }
}
#endif

#endif   // OEMRINGERS
