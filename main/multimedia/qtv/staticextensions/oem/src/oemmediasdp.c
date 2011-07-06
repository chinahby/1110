/*===========================================================================

FILE: OEMSDP.c

SERVICES: SDP file pre-parser for IMedia

DESCRIPTION
This file parsers an SDP file looking for the MIME type, then creates the 
appropriate IMedia object.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2002 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/

#include "msg.h"
#include "OEMFeatures.h"
#include "OEMMedia.h"
#include "OEMObjectMgr.h"
#include "AEEMimeTypes.h"
#include "AEEMediaFormats.h"
#include "fs.h"
#include "AEEMediaUtil.h"
#include "AEEFile.h"

#ifdef   FEATURE_APP_SDP

#include "OEMMediaReal.h"
#include "AEEMediaReal.h"
#include "OEMMediaSDP.h"
#include "AEEMediaSDP.h"

#ifdef FEATURE_BREW_3_0
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* FEATURE_BREW_3_0 */


/*==============================================================================

MACRO DEFINITIONS

==============================================================================*/

#define MAX_CB 4
/*==============================================================================

EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

TYPE DEFINITIONS

==============================================================================*/
//
// SDP IMedia objects must contain a reference to this object.
// Note: This class is a friend of IMedia and/or IMedia-based classes.
//
typedef struct OEMMediaSDPLayer
{
   AEEMedia *        m_pMedia;           // Owner IMedia object
   void *            m_pAppContext;      // App context
   AEEObjectHandle   m_hObject;          // Context returned by ObjectMgr
   PFNNOTIFY         m_pfnNotify;        // IMedia notification function
   AEEMediaCallback  m_cb[MAX_CB];
   IMedia           *m_player;
} OEMMediaSDPLayer;

OBJECT(OEMMediaSDP)
{
   INHERIT_AEEMedia(IMediaSDP);
   AEERect  m_rectImage;
   boolean  m_bRelMark;
   OEMMediaSDPLayer *m_pSDP;
};

/*==============================================================================

DATA DECLARATIONS

==============================================================================*/
static uint32        guSDPRefCnt = 0;

/*===============================================================================

COMMON FUNCTION DECLARATIONS
These functions are used by IMedia classes implemented in this file.

===============================================================================*/
void OEMMediaSDP_CallbackNotify(AEEMediaCallback * pcb);
static OEMMediaSDPLayer *OEMMediaSDPLayer_New(IMedia *po, PFNNOTIFY pfn);

/*===========================================================================

IMediaSDP DECLARATIONS

===========================================================================*/
static uint32 OEMMediaSDP_Release(IMedia *po);
static int    OEMMediaSDP_SetMediaParm(IMedia *po, int nParmID, int32 p1, int32 p2);
static int    OEMMediaSDP_GetMediaParm(IMedia *po, int nParmID, int32 *pP1, int32 * pP2);
static int    OEMMediaSDP_Play(IMedia *po);
static int    OEMMediaSDP_RecordUnsupported(IMedia *po);
static int    OEMMediaSDP_Stop(IMedia *po);
static int    OEMMediaSDP_Seek(IMedia *po, AEEMediaSeek eSeek, int32 lTimeMS);
static int    OEMMediaSDP_Pause(IMedia *po);
static int    OEMMediaSDP_Resume(IMedia *po);
static int    OEMMediaSDP_GetTotalTime(IMedia *po);
static IBase *OEMMediaSDP_New(IShell *ps, AEECLSID cls);

static const VTBL(IMediaSDP) gMediaSDPFuncs =
{
   AEEMedia_AddRef,
   OEMMediaSDP_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   OEMMediaSDP_SetMediaParm,
   OEMMediaSDP_GetMediaParm,
   OEMMediaSDP_Play,
   OEMMediaSDP_RecordUnsupported,
   OEMMediaSDP_Stop,
   OEMMediaSDP_Seek,
   OEMMediaSDP_Pause,
   OEMMediaSDP_Resume,
   OEMMediaSDP_GetTotalTime,
   AEEMedia_GetState
};

/*===============================================================================

IMediaSDP PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
Called only once during AEE initialization
==================================================================*/
void IMediaSDP_Init(IShell *ps)
{
   AEEMedia_Init(ps, MEDIASDP_MIME, AEECLSID_MEDIASDP);
}

/*==================================================================
Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaSDP_New(IShell *ps, AEECLSID cls, void **ppif)
{
   IBase *pobj;

   if (guSDPRefCnt == 0)
   {
      pobj = OEMMediaSDP_New(ps, cls);
      guSDPRefCnt = (pobj) ? 1 : 0;
   }
   else
   {
      pobj = NULL;
   }

   *ppif = pobj;

   return pobj ? SUCCESS : ENOMEMORY;
}

/*===============================================================================

SDP FUNCTION DEFINITIONS

===============================================================================*/
/* ===========================================================================
FUNCTION OEMMEDIASDP_NEW
DESCRIPTION
============================================================================== */
static IBase *OEMMediaSDP_New(IShell *ps, AEECLSID cls)
{
   IMedia *po;
   OEMMediaSDP *pme;

   // Alloc memory for the object
   po = (IMedia *)AEE_NewClass((IBaseVtbl *)&gMediaSDPFuncs, sizeof(OEMMediaSDP));
   if (!po)
      return NULL;

   // Call base class constructor
   if (SUCCESS != AEEMedia_New(po, ps, cls))
   {
      FREE(po);
      return NULL;
   }

   // Call SDP Layer constructor
   pme = (OEMMediaSDP *)po;
   pme->m_pSDP = OEMMediaSDPLayer_New(po, (PFNNOTIFY)OEMMediaSDP_CallbackNotify);

   if ( !pme->m_pSDP )
   {
      AEEMedia_Delete(po);
      FREE(po);
      return NULL;
   }

   return (IBase *)po;
}

/*==================================================================
Destructor
==================================================================*/
static void OEMMediaSDP_Delete(IMedia *po, boolean bFree)
{
   OEMMediaSDP      *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP;
   int i;

   if (!pme->m_bRelMark)
   {
      pme->m_bRelMark = TRUE;

      /* Cancel any pending callbacks */
      for (i = 0; i < MAX_CB; i++)
      {
         CALLBACK_Cancel(&pLayer->m_cb[i].cb);
      }

      if (pLayer->m_player)
      {
         IMEDIA_Release(pLayer->m_player);
         pLayer->m_player = NULL;
      }

      AEEObjectMgr_Unregister(pLayer->m_hObject);

      // Call base class destructor
      AEEMedia_Delete(po);
   }

   // Free object memory
   if (bFree)
      FREE(po);
}

/*==============================================================================

==============================================================================*/
static uint32 OEMMediaSDP_Release(IMedia *po)
{
   int32 nRef = AEEMedia_Release(po);

   if (!nRef)
   {
      OEMMediaSDP_Delete(po, TRUE);
      guSDPRefCnt = 0;
      /* Set guSDPRefCnt to 0 since we're allowing only one instance of
      ** SDP media object.
      */
   }

   return nRef;
}

/*==================================================================
   Called by the PV MPEG4 Engine callback function to queue a
   callback to execute in the BREW context
==================================================================*/
int OEMMediaSDP_QueueCallback(IMedia *po, int nStatus, int nCmd, int nSubCmd, void *pData, uint32 dwSize)
{
   OEMMediaSDP      *pme    = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int i;
   AEEMediaCallback   * pcb = NULL;

   /* Find a free callback structure */
   for (i = 0; i < MAX_CB; i++)
   {
      if (pLayer->m_cb[i].bInUse == FALSE)
      {
         pLayer->m_cb[i].bInUse = TRUE;
         pcb = &pLayer->m_cb[i];
         break;
      }
   }

   if (i == MAX_CB)
   {
      MSG_FATAL("Out of callback structures", 0, 0, 0);
      return -1;
   }

   pcb->cmdNotify.nStatus = nStatus;
   pcb->cmdNotify.nCmd = nCmd;
   pcb->cmdNotify.nSubCmd = nSubCmd;
   pcb->cmdNotify.pCmdData = pData; 
   pcb->cmdNotify.dwSize = dwSize;
#ifdef FEATURE_BREW_3_0
   AEE_ResumeCallback(&pcb->cb, pLayer->m_pAppContext);
#else
   AEE_ResumeEx(&pcb->cb, 0, pLayer->m_pAppContext);
#endif /* FEATURE_BREW_3_0 */
   return 0;
}

/*==================================================================
FUNCTION:  OEMMediaSDP_Notify()
DESCRIPTION:
==================================================================*/
static void OEMMediaSDP_Notify(AEEMediaCallback * pcb)
{
   OEMMediaSDP *      pMedia;
   OEMMediaSDPLayer * pme;
   AEEMediaCallback   mcb;

   if (!pcb)
      return;

   pMedia = (OEMMediaSDP *)pcb->cmdNotify.pIMedia;
   if (!pMedia)
      return;

   pme = pMedia->m_pSDP;
   if (!pme)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));

   /* Free the callback structure */
   pcb->bInUse = FALSE;

   if (pme->m_pfnNotify)
      pme->m_pfnNotify(&mcb);
}

/* ===========================================================================
FUNCTION OEMMEDIASDP_CALLBACKNOTIFY
DESCRIPTION
============================================================================== */
void OEMMediaSDP_CallbackNotify(AEEMediaCallback * pcb)
{
   OEMMediaSDP *    pme;

   if (!pcb)
      return;

   pme = (OEMMediaSDP *)pcb->cmdNotify.pIMedia;

   AEEMedia_CallbackNotify((AEEMedia *)pme, pcb);
}


#define END_CHAR_COUNT 2
const char *OEMMediaSDP_GetEndLocation(const char *startLoc)
{
   char endChar[END_CHAR_COUNT] = 
   {
      '\n', // Carriage return
      '\r', // Line feed
   };
   const char *endLoc[END_CHAR_COUNT];
   int i;
   for (i = 0; i < END_CHAR_COUNT; i++)
   {
      endLoc[i] = STRCHR(startLoc, endChar[i]);
   }

   // figure out which one is first
   for (i = 1; i < END_CHAR_COUNT; i++)
   {
      if (endLoc[0] == NULL || (endLoc[i] != NULL && endLoc[i] < endLoc[0]))
      {
         endLoc[0] = endLoc[i];
      }
   }
   return endLoc[0];
}

/* ==========================================================================
FUNCTION PARSEVALUE
DESCRIPTION
============================================================================= */
static boolean OEMMediaSDP_ParseValue(
   const char *input, 
   const char *keyWord, 
   char *value, 
   int valueLength
)
{
   const char *inputStr = input;
   const char *startLoc;
   const char *endLoc;
   boolean result = FALSE;
   value[0] = '\0';

   startLoc = STRSTR(inputStr, keyWord);
   if (startLoc != NULL)
   {
      startLoc = STRCHR(startLoc, ':');
      if (startLoc != NULL)
      {
         result = TRUE;
         // skip the ':'
         startLoc++;

         // Account for quoted strings
         if (*startLoc == '\"')
         {
            // skip the '"'
            startLoc++;
            endLoc = STRCHR(startLoc, '\"');
         }
         else
         {
            endLoc = OEMMediaSDP_GetEndLocation(startLoc);
         }


         if (endLoc != NULL)
         {
            STRNCPY(value, startLoc, MIN(endLoc - startLoc, valueLength - 1));
            value[MIN(endLoc - startLoc, valueLength - 1)] = '\0';
         }
         else
         {
            STRNCPY(value, startLoc, valueLength - 1);
            value[valueLength - 1] = '\0';
         }
      }
   }
   return result;
}

char *OEMMediaSDP_skip_whitespace( char * ptr )
{
  while ( ptr && *ptr )
  {
    if ( *ptr != ' ' && *ptr != '\t' )
    {
      break;
    }

    ++ptr;
  }

  return ptr;
}

/*==================================================================

==================================================================*/
#define MAX_MIME_STR_LEN 256
/* ===========================================================================
FUNCTION OEMMediaSDP_GetMIMEType
DESCRIPTION
============================================================================== */
char *OEMMediaSDP_GetMIMEType(const char *pFile)
{
   const char *mimeTag = "a=rtpmap";
   char       *buf = NULL;
   char       *pos;
   static char mime[MAX_MIME_STR_LEN] = "";
   char       *mimepos = NULL;
   unsigned long        file_length;
   int result;
   struct fs_stat sbuf;
   int filedes = -1;
   int read_count;

   result = efs_stat (pFile, &sbuf);
   if(result != 0)
   {
      return FALSE;
   }

   file_length = sbuf.st_size;
   buf = (char *)MALLOC(file_length + 1);

   if (buf == NULL)
   {
      return FALSE;
   }


   filedes = efs_open(pFile,O_RDONLY);
   if(filedes < 0)
   {
      FREE(buf);
      return FALSE;
   }


   MEMSET(buf, 0, sizeof(buf));
   read_count = efs_read(filedes, buf, file_length);
   buf[file_length] = '\0';
   efs_close(filedes);
   if(read_count < 0)
   {
      FREE(buf);
      return FALSE;
   }

   if (!OEMMediaSDP_ParseValue(buf, mimeTag, mime, 256))
   {
      return FALSE;
   }
   FREE(buf);

   // skip any leading whitespace
   mimepos = OEMMediaSDP_skip_whitespace(mime);
   // Skip the payload number
   pos = strchr(mimepos, ' ');
   if (pos)
   {
      mimepos = pos+1;
      // Remove the sample rate
      pos = strchr(mime, '/');
      if (pos)
      {
         *pos = '\0';
      }
   }


   return mimepos;
}

/* ===========================================================================
FUNCTION OEMMEDIASDP_CREATEMEDIAOBJECT
DESCRIPTION
============================================================================== */
static int OEMMediaSDP_CreateMediaObject(IMedia *po, char *fname, uint32 clsid, IMedia **media)
{
   int ret;
   OEMMediaSDP *pme = (OEMMediaSDP *)po;

   ret = ISHELL_CreateInstance(pme->m_pIShell, clsid, (void **)media);

   if (ret == SUCCESS && *media != NULL)
   {
      AEEMediaData md;

      md.clsData = MMD_FILE_NAME;

      md.pData = (void *)fname;
      md.dwSize = 0;
      IMEDIA_SetMediaParm(*media, MM_PARM_MEDIA_DATA, (uint32)&md, 0);
   }

   return ret;
} /* END OEMMediaSDP_CreateMediaObject */

/* ===========================================================================
FUNCTION OEMMEDIASDP_MEDIANOTIFY
DESCRIPTION
============================================================================== */
static void OEMMediaSDP_MediaNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify)
{
   OEMMediaSDP_QueueCallback(pUser, pCmdNotify->nStatus, 
      pCmdNotify->nCmd, pCmdNotify->nSubCmd, pCmdNotify->pCmdData, pCmdNotify->dwSize);
}

/* ===========================================================================
FUNCTION OEMMEDIASDP_CREATEMEDIA
DESCRIPTION
============================================================================== */
static int OEMMediaSDP_CreateMedia(IMedia *po)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP;
   AEEMedia *pMedia = pLayer->m_pMedia;
   int nRet = EUNSUPPORTED;

   char *mime = OEMMediaSDP_GetMIMEType(pMedia->m_pszFile);

   if (mime)
   {
      if (strcmp(mime, REAL_PLAYER_MIME_TYPE_AUDIO) == 0 ||
          strcmp(mime, REAL_PLAYER_MIME_TYPE_VIDEO) == 0)
      {
         nRet = EFAILED;
#ifdef FEATURE_APP_REAL
         nRet = OEMMediaSDP_CreateMediaObject(po, pMedia->m_pszFileAppPath, 
            AEECLSID_MEDIAREAL, &pLayer->m_player);
#endif
      }
      else
      {
         nRet = OEMMediaSDP_CreateMediaObject(po, pMedia->m_pszFileAppPath, 
            AEECLSID_MEDIAMPEG4, &pLayer->m_player);
      }
      if (pLayer->m_player)
      {
         IMEDIA_RegisterNotify(pLayer->m_player, OEMMediaSDP_MediaNotify, po);
      }
   }
   else
   {
      nRet = EFAILED;
   }
   return nRet;
}

/*==============================================================================

==============================================================================*/
static int OEMMediaSDP_SetMediaParm(IMedia *po, int nParmID, int32 p1, int32 p2)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   AEEMedia *pMedia = pme->m_pSDP->m_pMedia;
   int nRet = EUNSUPPORTED;

   switch (nParmID)
   {
   case MM_PARM_MEDIA_DATA:
      nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
      if (nRet == SUCCESS)
      {
         switch( pMedia->m_md.clsData )
         {
         case MMD_FILE_NAME:
            nRet = OEMMediaSDP_CreateMedia(po);
            break;

         default:
            nRet = EUNSUPPORTED;
         }
      }
      break;
   case MM_PARM_RECT:
      MEMCPY(&pme->m_rectImage, (void *)p1, sizeof(AEERect));
      // Fall through to do the default case also
   default:
      {
         if (pme->m_pSDP->m_player)
         {
            nRet = IMEDIA_SetMediaParm(pme->m_pSDP->m_player, nParmID, p1, p2);
         }
      }
   }

   return nRet;
}


/*==============================================================================

==============================================================================*/
static int OEMMediaSDP_GetMediaParm(IMedia *po, int nParmID, int32 *p1, int32 *p2)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int nRet = SUCCESS;

   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_GetMediaParm(po, nParmID, p1, p2);

   if (pLayer->m_player)
   {
      nRet = IMEDIA_GetMediaParm(pLayer->m_player, nParmID, p1, p2);
   }

   return nRet;
}

/*==================================================================

==================================================================*/
static int OEMMediaSDP_Play(IMedia *po)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int nRet = EFAILED;

   if (pLayer->m_player)
   {
      nRet = IMEDIA_Play(pLayer->m_player);
   }

   if (nRet == SUCCESS)
   {
      nRet = AEEMedia_Play(po);
   }

   return nRet;
}

/*==================================================================
Unsupported functions -- used in vtbl of IMedia classes
==================================================================*/
static int OEMMediaSDP_RecordUnsupported(IMedia *po)
{
   return EUNSUPPORTED;
}

/*==================================================================

==================================================================*/
static int OEMMediaSDP_Stop(IMedia *po)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int            nRet;

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_player)
   {
      IMEDIA_Stop(pLayer->m_player);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaSDP_Seek(IMedia *po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int            nRet;

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_player)
   {
      IMEDIA_Seek(pLayer->m_player, eSeek, lTimeMS);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaSDP_Pause(IMedia *po)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int            nRet;

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_player)
   {
      IMEDIA_Pause(pLayer->m_player);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaSDP_Resume(IMedia *po)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int            nRet;

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_player)
   {
      IMEDIA_Resume(pLayer->m_player);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaSDP_GetTotalTime(IMedia *po)
{
   OEMMediaSDP *pme = (OEMMediaSDP *)po;
   OEMMediaSDPLayer *pLayer = pme->m_pSDP; 
   int sound = 0;
   int image = 0;
   int nRet;

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_player)
   {
      sound = IMEDIA_GetTotalTime(pLayer->m_player);
   }
   return MAX(sound,image);
}

/*==================================================================
IMediaSDP constructor functions
==================================================================*/
static OEMMediaSDPLayer *OEMMediaSDPLayer_New(IMedia *po, PFNNOTIFY pfn)
{
   OEMMediaSDPLayer *  pme = MALLOC(sizeof(OEMMediaSDPLayer));
   int           i;

   if (!pme || !po)
      return NULL;

   /* Only allow one instantiation of the SDP IMedia object */
   if (guSDPRefCnt != 0)
      return NULL;

   guSDPRefCnt++;

   pme->m_pMedia = (AEEMedia *)po;
   pme->m_pAppContext = AEE_GetAppContext();

   // Initialize callbacks.
   for (i = 0; i < MAX_CB; i++)
   {
      pme->m_cb[i].bInUse = FALSE;
      pme->m_cb[i].cmdNotify.pIMedia = po;
      pme->m_cb[i].cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;
      pme->m_cb[i].cmdNotify.nStatus = 0;
      CALLBACK_Init(&pme->m_cb[i].cb, OEMMediaSDP_Notify, &pme->m_cb[i]);
   }

   pme->m_pfnNotify = pfn;  // pfn is OEMMediaSDP_CallbackNotify

   if (AEEObjectMgr_Register(pme->m_pMedia, &pme->m_hObject) != SUCCESS)
   {
      FREE(pme);
      return NULL;
   }

   return pme;
}

#endif  // FEATURE_APP_SDP
