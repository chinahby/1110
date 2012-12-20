/*===========================================================================

FILE: OEMMultipleFile.c

SERVICES: Multiple File playback functionality for IMedia

DESCRIPTION
This file implements Multiple File playback functionality that can be 
used by IMedia-based classes.

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
#include "fs_public.h"
#include "AEEMediaUtil.h"
#include "AEEFile.h"

#ifdef   FEATURE_APP_MEDIA_MULTIPLE_FILE

#include "OEMMediaMPEG4.h"
#include "AEEMediaMPEG4.h"
#include "OEMMediaMultipleFile.h"
#include "AEEMediaMultipleFile.h"

#ifdef FEATURE_BREW_3_0
# include "AEE_OEMHeap.h"
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* FEATURE_BREW_3_0 */


#ifdef FEATURE_BREW_3_0
static OEMCriticalSection gcmCriticalSection;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&gcmCriticalSection)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&gcmCriticalSection)
#else /* FEATURE_BREW_3_0 */
# define MUTEX_LOCK() OEMOS_MutexStart(); {
# define MUTEX_UNLOCK() } OEMOS_MutexStop();
#endif /* FEATURE_BREW_3_0 */

/*==============================================================================

MACRO DEFINITIONS

==============================================================================*/

#define MAX_CB 4
#define MIN_TOTAL_TIME 1
/*==============================================================================

EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

TYPE DEFINITIONS

==============================================================================*/
//
// MultipleFile IMedia objects must contain a reference to this object.
// Note: This class is a friend of IMedia and/or IMedia-based classes.
//
typedef enum 
{
   NOT_SUPPORTED,
   COMBINED,
   IMAGE_FIRST,
   SIMULTANEOUS
}OEMMediaMFType;

typedef struct MediaInfoStruct
{
   IMedia      *m_pImedia;
   char         m_file[AEE_MAX_FILE_NAME];
   AEECLSID     m_classID;
   uint32       m_time;
}MediaInfoType;

typedef struct OEMMediaMultipleFileLayer
{
   AEEMedia *        m_pMedia;           // Owner IMedia object
   void *            m_pAppContext;      // App context
   AEEObjectHandle   m_hObject;          // Context returned by ObjectMgr
   PFNNOTIFY         m_pfnNotify;        // IMedia notification function
   AEEMediaCallback  m_cb[MAX_CB];
   boolean           m_playing;
   OEMMediaMFType    m_type;
   MediaInfoType     m_sound;
   MediaInfoType     m_image;
   IImage *          m_pIimage;
   boolean           m_iImageReady;
} OEMMediaMultipleFileLayer;

OBJECT(OEMMediaMultipleFile)
{
   INHERIT_AEEMedia(IMediaMultipleFile);
   AEERect  m_rectImage;
   boolean  m_bRelMark;
   OEMMediaMultipleFileLayer *m_pMultipleFile;
};

/*==============================================================================

DATA DECLARATIONS

==============================================================================*/
static uint32        guMultipleFileRefCnt = 0;

/*===============================================================================

COMMON FUNCTION DECLARATIONS
These functions are used by IMedia classes implemented in this file.

===============================================================================*/
void OEMMediaMultipleFile_CallbackNotify(AEEMediaCallback * pcb);
static OEMMediaMultipleFileLayer *OEMMediaMultipleFileLayer_New(IMedia *po, PFNNOTIFY pfn);

/*===========================================================================

IMediaMultipleFile DECLARATIONS

===========================================================================*/
static uint32 OEMMediaMultipleFile_Release(IMedia *po);
static int    OEMMediaMultipleFile_SetMediaParm(IMedia *po, int nParmID, int32 p1, int32 p2);
static int    OEMMediaMultipleFile_GetMediaParm(IMedia *po, int nParmID, int32 *pP1, int32 * pP2);
static int    OEMMediaMultipleFile_Play(IMedia *po);
static int    OEMMediaMultipleFile_RecordUnsupported(IMedia *po);
static int    OEMMediaMultipleFile_Stop(IMedia *po);
static int    OEMMediaMultipleFile_Seek(IMedia *po, AEEMediaSeek eSeek, int32 lTimeMS);
static int    OEMMediaMultipleFile_Pause(IMedia *po);
static int    OEMMediaMultipleFile_Resume(IMedia *po);
static int    OEMMediaMultipleFile_GetTotalTime(IMedia *po);
static IBase *OEMMediaMultipleFile_New(IShell *ps, AEECLSID cls);

static const VTBL(IMediaMultipleFile) gMediaMultipleFileFuncs =
{
   AEEMedia_AddRef,
   OEMMediaMultipleFile_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   OEMMediaMultipleFile_SetMediaParm,
   OEMMediaMultipleFile_GetMediaParm,
   OEMMediaMultipleFile_Play,
   OEMMediaMultipleFile_RecordUnsupported,
   OEMMediaMultipleFile_Stop,
   OEMMediaMultipleFile_Seek,
   OEMMediaMultipleFile_Pause,
   OEMMediaMultipleFile_Resume,
   OEMMediaMultipleFile_GetTotalTime,
   AEEMedia_GetState
};

/*===============================================================================

IMediaMultipleFile PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
Called only once during AEE initialization
==================================================================*/
void IMediaMultipleFile_Init(IShell *ps)
{
   AEEMedia_Init(ps, MEDIAMULTIPLEFILE_MIME, AEECLSID_MEDIAMULTIPLEFILE);
}

/*==================================================================
Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaMultipleFile_New(IShell *ps, AEECLSID cls, void **ppif)
{
   IBase *pobj;

   if (guMultipleFileRefCnt == 0)
   {
      pobj = OEMMediaMultipleFile_New(ps, cls);
      guMultipleFileRefCnt = (pobj) ? 1 : 0;
   }
   else
   {
      pobj = NULL;
   }

   *ppif = pobj;

   return pobj ? SUCCESS : ENOMEMORY;
}

/*===============================================================================

MultipleFile FUNCTION DEFINITIONS

===============================================================================*/
/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_NEW
DESCRIPTION
============================================================================== */
static IBase *OEMMediaMultipleFile_New(IShell *ps, AEECLSID cls)
{
   IMedia *po;
   OEMMediaMultipleFile *pme;

   // Alloc memory for the object
   po = (IMedia *)AEE_NewClass((IBaseVtbl *)&gMediaMultipleFileFuncs, sizeof(OEMMediaMultipleFile));
   if (!po)
      return NULL;

   // Call base class constructor
   if (SUCCESS != AEEMedia_New(po, ps, cls))
   {
      FREE(po);
      return NULL;
   }

   // Call MultipleFile Layer constructor
   pme = (OEMMediaMultipleFile *)po;
   pme->m_pMultipleFile = OEMMediaMultipleFileLayer_New(po, (PFNNOTIFY)OEMMediaMultipleFile_CallbackNotify);

   if ( !pme->m_pMultipleFile )
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
static void OEMMediaMultipleFile_Delete(IMedia *po, boolean bFree)
{
   OEMMediaMultipleFile      *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile;
   int i;

   if (!pme->m_bRelMark)
   {
      pme->m_bRelMark = TRUE;

      /* Cancel any pending callbacks */
      for (i = 0; i < MAX_CB; i++)
      {
         CALLBACK_Cancel(&pLayer->m_cb[i].cb);
      }

      if (pLayer->m_pIimage)
      {
         IIMAGE_Release(pLayer->m_pIimage);
         pLayer->m_pIimage = NULL;
      }
      if (pLayer->m_sound.m_pImedia)
      {
         IMEDIA_Release(pLayer->m_sound.m_pImedia);
         pLayer->m_sound.m_pImedia = NULL;
      }
      if (pLayer->m_image.m_pImedia)
      {
         IMEDIA_Release(pLayer->m_image.m_pImedia);
         pLayer->m_image.m_pImedia = NULL;
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
static uint32 OEMMediaMultipleFile_Release(IMedia *po)
{
   int32 nRef = AEEMedia_Release(po);

   if (!nRef)
   {
      OEMMediaMultipleFile_Delete(po, TRUE);
      guMultipleFileRefCnt = 0;
      /* Set guMultipleFileRefCnt to 0 since we're allowing only one instance of
      ** MultipleFile media object.
      */
   }

   return nRef;
}

/*==================================================================
   Called by the PV MPEG4 Engine callback function to queue a
   callback to execute in the BREW context
==================================================================*/
int OEMMediaMultipleFile_QueueCallback(IMedia *po, int nStatus, int nCmd, int nSubCmd, void *pData, uint32 dwSize)
{
   OEMMediaMultipleFile      *pme    = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int i;
   AEEMediaCallback   * pcb = NULL;

   /* Find a free callback structure */
   MUTEX_LOCK();
   for (i = 0; i < MAX_CB; i++)
   {
      if (pLayer->m_cb[i].bInUse == FALSE)
      {
         pLayer->m_cb[i].bInUse = TRUE;
         pcb = &pLayer->m_cb[i];
         break;
      }
   }
   MUTEX_UNLOCK();

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
FUNCTION:  OEMMediaMultipleFile_Notify()
DESCRIPTION:
==================================================================*/
static void OEMMediaMultipleFile_Notify(AEEMediaCallback * pcb)
{
   OEMMediaMultipleFile *      pMedia;
   OEMMediaMultipleFileLayer * pme;
   AEEMediaCallback   mcb;

   if (!pcb)
      return;

   pMedia = (OEMMediaMultipleFile *)pcb->cmdNotify.pIMedia;
   if (!pMedia)
      return;

   pme = pMedia->m_pMultipleFile;
   if (!pme)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));

   /* Free the callback structure */
   pcb->bInUse = FALSE;

   if (pme->m_pfnNotify)
      pme->m_pfnNotify(&mcb);
}

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_CALLBACKNOTIFY
DESCRIPTION
============================================================================== */
void OEMMediaMultipleFile_CallbackNotify(AEEMediaCallback * pcb)
{
   OEMMediaMultipleFile *    pme;

   if (!pcb)
      return;

   pme = (OEMMediaMultipleFile *)pcb->cmdNotify.pIMedia;

   AEEMedia_CallbackNotify((AEEMedia *)pme, pcb);
}


#define END_CHAR_COUNT 5
const char *OEMMediaMultipleFile_GetEndLocation(const char *startLoc)
{
   char endChar[END_CHAR_COUNT] = 
   {
      ' ',  // Space
      '\n', // Carriage return
      '\r', // Line feed
      '\t', // Tab
      '>'   // HTML end comment character
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
static boolean OEMMediaMultipleFile_ParseValue(
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
      startLoc = STRCHR(startLoc, '=');
      if (startLoc != NULL)
      {
         result = TRUE;
         // skip the '='
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
            endLoc = OEMMediaMultipleFile_GetEndLocation(startLoc);
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


/*==================================================================

==================================================================*/
/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_GETAUDIOVIDEOFILENAMES
DESCRIPTION
   pFile is the absolute path to the file, pRelativePath is the BREW relative 
   path to the file.  pFile would be "brew/media/..." while pRelativePath would 
   be "media/...".  pFile is used to open the file directly using the file 
   system utilities, while pRelativePath is used to open the file using the BREW 
   utilities such as AEEMediaUtil_CreateMedia.
============================================================================== */
boolean OEMMediaMultipleFile_GetAudioVideoFileNames(const char *pFile, const char *pRelativePath, 
                                                    char *pImageBuf, char *pSoundBuf, int bufSize)
{
   const char *imageTag = "IMAGE";
   const char *soundTag = "SOUND";
   const char *endPath = NULL;
   char       buf[AEE_MAX_FILE_NAME*4];
   char       image[AEE_MAX_FILE_NAME];
   char       sound[AEE_MAX_FILE_NAME];
   int filedes = -1;
   fs_ssize_t result;   

   filedes = efs_open(pFile,O_RDONLY);
   if(filedes < 0)
   {
      return FALSE;
   }

   MEMSET(buf, 0, sizeof(buf));
   result = efs_read(filedes, buf, sizeof(buf));
   buf[sizeof(buf) - 1] = '\0';
   efs_close(filedes);
   if(result < 0)
   {
      return FALSE;
   }

   if (!OEMMediaMultipleFile_ParseValue(buf, imageTag, image, AEE_MAX_FILE_NAME))
   {
      return FALSE;
   }

   if (!OEMMediaMultipleFile_ParseValue(buf, soundTag, sound, AEE_MAX_FILE_NAME))
   {
      return FALSE;
   }

   // Append the input file path if required
   endPath = STRRCHR(pRelativePath, DIRECTORY_CHAR);

   if (image[0] != DIRECTORY_CHAR && endPath != NULL)
   {
      STRNCPY(pImageBuf, pRelativePath, endPath - pRelativePath + 1);
      STRLCPY(pImageBuf + (endPath - pRelativePath + 1), image, bufSize);
   }
   else
   {
      STRLCPY(pImageBuf, image, bufSize);
   }

   if (sound[0] != DIRECTORY_CHAR && endPath != NULL)
   {
      STRNCPY(pSoundBuf, pRelativePath, endPath - pRelativePath + 1);
      STRLCPY(pSoundBuf + (endPath - pRelativePath + 1), sound, bufSize);
   }
   else
   {
      STRLCPY(pSoundBuf, sound, bufSize);
   }

   return TRUE;
}

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_GETTYPE
DESCRIPTION
============================================================================== */
static OEMMediaMFType OEMMediaMultipleFile_GetType(OEMMediaMultipleFileLayer *pLayer)
{
   OEMMediaMFType type = NOT_SUPPORTED;
 
   if ((pLayer->m_image.m_classID == AEECLSID_MEDIAMPEG4))
   {
      if (pLayer->m_sound.m_classID == AEECLSID_MEDIAMPEG4)
      {
         type = COMBINED;
      }
      else if ( (pLayer->m_sound.m_classID == AEECLSID_MEDIAQCP) || 
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAPMD) ||
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAMP3) ||
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAAAC) ||
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAAMR) )
      {
         type = SIMULTANEOUS;
      }
   }
   else if (pLayer->m_pIimage != NULL)
   {
      type = IMAGE_FIRST;
   }
   return type;
} /* END OEMMediaMultipleFile_GetType */

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_CREATEQTVMEDIA
DESCRIPTION
============================================================================== */
static int OEMMediaMultipleFile_CreateQTVMedia(IMedia *po, char *iFname, char *sFname, IMedia **media)
{
   int ret;
   char soundFile[AEE_MAX_FILE_NAME];
   char imageFile[AEE_MAX_FILE_NAME];
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   if (iFname != NULL)
   {
      STRLCPY(imageFile, "brew/", AEE_MAX_FILE_NAME);
      STRLCAT(imageFile, iFname, AEE_MAX_FILE_NAME);
      iFname = imageFile;
   }
   if (sFname != NULL)
   {
      STRLCPY(soundFile, "brew/", AEE_MAX_FILE_NAME);
      STRLCAT(soundFile, sFname, AEE_MAX_FILE_NAME);
      sFname = soundFile;
   }

   ret = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_MEDIAMPEG4, (void **)media);

   if (ret == SUCCESS && *media != NULL)
   {
      IMEDIA_SetMediaParm(*media, MM_MP4_AUDIO_VIDEO_MEDIA_DATA, (uint32)iFname, (uint32)sFname);
   }

   return ret;
} /* END OEMMediaMultipleFile_CreateQTVMedia */

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_CREATEMEDIA
DESCRIPTION
============================================================================== */
static int OEMMediaMultipleFile_CreateMedia(IMedia *po, char *fname, IMedia **media)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   AEEMediaData md;

   md.clsData = MMD_FILE_NAME;
   md.pData = fname;
   md.dwSize = STRLEN(md.pData);

   return AEEMediaUtil_CreateMedia(pme->m_pIShell, &md, media);
} /* END OEMMediaMultipleFile_CreateMedia */

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_GETMEDIACLASSID
DESCRIPTION
============================================================================== */
static int OEMMediaMultipleFile_GetMediaClassID(IMedia *po, char *fname, AEECLSID *cls)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   AEEMediaData md;

   md.clsData = MMD_FILE_NAME;
   md.pData = fname;
   md.dwSize = 0;

   *cls = 0;
   return AEEMediaUtil_FindClass(pme->m_pIShell, &md, cls);
} /* END OEMMediaMultipleFile_CreateMedia */

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_DISPLAYIMAGE
DESCRIPTION
============================================================================== */
static void OEMMediaMultipleFile_DisplayImage(IMedia *po)
{
   IBitmap                   *pBitmap;
   IDisplay                  *pIDisplay;
   IBitmap                   *pDispBitmap;
   AEEImageInfo               info;
   OEMMediaMultipleFile      *pme    = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 

   IIMAGE_GetInfo(pLayer->m_pIimage, &info); 

   // Get the display
   ISHELL_CreateInstance(pLayer->m_pMedia->m_pIShell, AEECLSID_DISPLAY, (void **)&pIDisplay);

   // Clear the display rect
   IDISPLAY_EraseRgn(pIDisplay,
                     pme->m_rectImage.x,
                     pme->m_rectImage.y,
                     pme->m_rectImage.dx,
                     pme->m_rectImage.dy);

   // Get the display bitmap
   IDISPLAY_GetDeviceBitmap(pIDisplay, &pDispBitmap);

   // Create a compatible bitmap
   IBITMAP_CreateCompatibleBitmap(pDispBitmap, &pBitmap, info.cx, info.cy);

   // Set the destination to the compatible bitmap
   IDISPLAY_SetDestination(pIDisplay, pBitmap);

   // Draw the image to the compatible bitmap
   IIMAGE_Draw(pLayer->m_pIimage, 0, 0);

   // Restore the display bitmap
   IDISPLAY_SetDestination(pIDisplay, pDispBitmap);

   // Release the display bitmap
   IBITMAP_Release(pDispBitmap);

   // Display the compatible bitmap
   IDISPLAY_BitBlt(pIDisplay, pme->m_rectImage.x, pme->m_rectImage.y, 
                   pme->m_rectImage.dx, pme->m_rectImage.dy, pBitmap, 0, 0, AEE_RO_COPY);

   // Release the compatible bitmap
   IBITMAP_Release(pBitmap);

   // Release the display
   IDISPLAY_Release(pIDisplay);

   return;
}
   
/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_IMAGENOTIFY
DESCRIPTION
============================================================================== */
void OEMMediaMultipleFile_ImageNotify(void * pUser, IImage * pImage, AEEImageInfo * pi, int nErr)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)pUser;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile;
   pLayer->m_iImageReady = TRUE;
   if (pLayer->m_playing == TRUE)
   {
      OEMMediaMultipleFile_DisplayImage(pUser);
   }
   return;
} /* END OEMMediaMultipleFile_ImageNotify */

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_MEDIANOTIFY
DESCRIPTION
============================================================================== */
static void OEMMediaMultipleFile_MediaNotify(void * pUser, AEEMediaCmdNotify * pCmdNotify)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)pUser;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile;
   boolean sendNotify = TRUE;

   switch (pCmdNotify->nCmd)
   {
   case MM_CMD_PLAY:
      if (pLayer->m_type == SIMULTANEOUS)
      {
        if(pCmdNotify->nStatus ==MM_MP4_STATUS_OPEN_COMPLETE)
        {
          if ( (pLayer->m_sound.m_classID == AEECLSID_MEDIAMP3)  ||
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAAAC) ||
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAAMR) ||
                (pLayer->m_sound.m_classID == AEECLSID_MEDIAQCP) )
          {
            IMEDIA_SetMediaParm( pLayer->m_image.m_pImedia, MM_MP4_PARAM_AUDIO_OVERRIDE,
                                   (int32)(pLayer->m_sound.m_classID), 0 );
          }
        }
        else if( (pCmdNotify->nStatus == MM_STATUS_DONE) || (pCmdNotify->nStatus == MM_STATUS_ABORT))
        {
          if (pCmdNotify->pIMedia == pLayer->m_image.m_pImedia)
          {
              sendNotify = (pLayer->m_image.m_time > pLayer->m_sound.m_time);
          }
          else if (pCmdNotify->pIMedia == pLayer->m_sound.m_pImedia)
          {
              sendNotify = (pLayer->m_sound.m_time >= pLayer->m_image.m_time);
          }
          else
          {
              sendNotify = FALSE;
          }
        }
      }
      break;
   case MM_CMD_GETTOTALTIME:
      if (pCmdNotify->nStatus == MM_STATUS_DONE && pLayer->m_type == SIMULTANEOUS)
      {
         if (pCmdNotify->pIMedia == pLayer->m_image.m_pImedia)
         {
            if ((uint32)pCmdNotify->pCmdData < MIN_TOTAL_TIME)
            {
               pLayer->m_image.m_time = MIN_TOTAL_TIME;
            }
            else
            {
               pLayer->m_image.m_time = (uint32)pCmdNotify->pCmdData;
            }
         }
         else if (pCmdNotify->pIMedia == pLayer->m_sound.m_pImedia)
         {
            if ((uint32)pCmdNotify->pCmdData < MIN_TOTAL_TIME)
            {
               pLayer->m_sound.m_time = MIN_TOTAL_TIME;
            }
            else
            {
               pLayer->m_sound.m_time = (uint32)pCmdNotify->pCmdData;
            }
         }
         if (pLayer->m_image.m_time > 0 && pLayer->m_sound.m_time > 0)
         {
            OEMMediaMultipleFile_QueueCallback(pUser, pCmdNotify->nStatus, 
               pCmdNotify->nCmd, pCmdNotify->nSubCmd, 
               (void *)MAX(pLayer->m_image.m_time, pLayer->m_sound.m_time), 
               pCmdNotify->dwSize);
         }
         sendNotify = FALSE;
      }
      // Fall through to default case
   default:
      break;
   }
   if (sendNotify)
   {
      OEMMediaMultipleFile_QueueCallback(pUser, pCmdNotify->nStatus, 
         pCmdNotify->nCmd, pCmdNotify->nSubCmd, pCmdNotify->pCmdData, pCmdNotify->dwSize);
   }
}

/* ===========================================================================
FUNCTION OEMMEDIAMULTIPLEFILE_CREATEMEDIA
DESCRIPTION
============================================================================== */
static int OEMMediaMultipleFile_CreateAllMedia(IMedia *po)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile;
   AEEMedia *pMedia = pLayer->m_pMedia;
   int nRet = EUNSUPPORTED;

   if( OEMMediaMultipleFile_GetAudioVideoFileNames(pMedia->m_pszFile, pMedia->m_md.pData,
                                                   pLayer->m_image.m_file, pLayer->m_sound.m_file, 
                                                   AEE_MAX_FILE_NAME) )
   {
      pLayer->m_pIimage = ISHELL_LoadImage(pLayer->m_pMedia->m_pIShell, pLayer->m_image.m_file);
      if (pLayer->m_pIimage)
      {
         IIMAGE_Notify(pLayer->m_pIimage, OEMMediaMultipleFile_ImageNotify, po);
      }
      else
      {
         // Determine the image file type
         nRet = OEMMediaMultipleFile_GetMediaClassID(po, pLayer->m_image.m_file, &pLayer->m_image.m_classID);
      }

      // Determine the sound file type
      nRet = OEMMediaMultipleFile_GetMediaClassID(po, pLayer->m_sound.m_file, &pLayer->m_sound.m_classID);
      
      pLayer->m_type = OEMMediaMultipleFile_GetType(pLayer);
      switch (pLayer->m_type)
      {
      case NOT_SUPPORTED:
         nRet = EFAILED;
         break;
      case COMBINED:
         // Put combined into the sound object
         nRet = OEMMediaMultipleFile_CreateQTVMedia(po, pLayer->m_image.m_file, 
            pLayer->m_sound.m_file, &pLayer->m_sound.m_pImedia);
         break;
      case IMAGE_FIRST:
         if (pLayer->m_sound.m_classID == AEECLSID_MEDIAMPEG4)
         {
            nRet = OEMMediaMultipleFile_CreateQTVMedia(po, NULL, pLayer->m_sound.m_file, 
                                                       &pLayer->m_sound.m_pImedia);
         }
         else
         {
            nRet = OEMMediaMultipleFile_CreateMedia(po, pLayer->m_sound.m_file, &pLayer->m_sound.m_pImedia);
         }
         break;
      case SIMULTANEOUS:
         if (pLayer->m_sound.m_classID == AEECLSID_MEDIAMPEG4)
         {
            nRet = OEMMediaMultipleFile_CreateQTVMedia(po, NULL, pLayer->m_sound.m_file, 
                                                       &pLayer->m_sound.m_pImedia);
         }
         else
         {
            nRet = OEMMediaMultipleFile_CreateMedia(po, pLayer->m_sound.m_file, &pLayer->m_sound.m_pImedia);
         }
         if (pLayer->m_image.m_classID == AEECLSID_MEDIAMPEG4)
         {
            nRet = OEMMediaMultipleFile_CreateQTVMedia(po, pLayer->m_image.m_file, NULL, &pLayer->m_image.m_pImedia);
         }
         else
         {
            nRet = OEMMediaMultipleFile_CreateMedia(po, pLayer->m_image.m_file, &pLayer->m_image.m_pImedia);
         }
         break;
      }
      if (pLayer->m_sound.m_pImedia)
      {
         IMEDIA_RegisterNotify(pLayer->m_sound.m_pImedia, OEMMediaMultipleFile_MediaNotify, po);
      }
      if (pLayer->m_image.m_pImedia)
      {
         IMEDIA_RegisterNotify(pLayer->m_image.m_pImedia, OEMMediaMultipleFile_MediaNotify, po);
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
static int OEMMediaMultipleFile_SetMediaParm(IMedia *po, int nParmID, int32 p1, int32 p2)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   AEEMedia *pMedia = pme->m_pMultipleFile->m_pMedia;
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
            if( STRSTR(pMedia->m_pszFile, ".mft") )
            {
               nRet = OEMMediaMultipleFile_CreateAllMedia(po);
            }
            else
            {
               nRet = EFAILED;
            }
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
         int nRetSound = EUNSUPPORTED;
         int nRetImage = EUNSUPPORTED;
         if (pme->m_pMultipleFile->m_image.m_pImedia)
         {
            nRetImage = IMEDIA_SetMediaParm(pme->m_pMultipleFile->m_image.m_pImedia, nParmID, p1, p2);
         }
         if (pme->m_pMultipleFile->m_sound.m_pImedia)
         {
            nRetImage = IMEDIA_SetMediaParm(pme->m_pMultipleFile->m_sound.m_pImedia, nParmID, p1, p2);
         }
         if (nRetImage != EUNSUPPORTED)
         {
            nRet = nRetImage;
         }
         else if (nRetSound != EUNSUPPORTED)
         {
            nRet = nRetSound;
         }
      }
   }

   return nRet;
}


/*==============================================================================

==============================================================================*/
static int OEMMediaMultipleFile_GetMediaParm(IMedia *po, int nParmID, int32 *p1, int32 *p2)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int nRet = SUCCESS;

   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_GetMediaParm(po, nParmID, p1, p2);

   switch (nParmID)
   {
   case MM_PARM_VOLUME:
      if (pLayer->m_sound.m_pImedia)
      {
         nRet = IMEDIA_GetMediaParm(pLayer->m_sound.m_pImedia, nParmID, p1, p2);
      }
      else
      {
         nRet = EUNSUPPORTED;
      }
      break;

   case MM_PARM_CLSID:
      if (p1 == NULL)
         return EFAILED;

      *p1 = AEECLSID_MEDIAMULTIPLEFILE;
      break;

   case MM_PARM_CAPS:
      if (p1 == NULL)
      {
         return EFAILED;
      }
      else
      {
         *p1 = 0;
         if (pLayer->m_pIimage)
         {
            *p1 |= MM_CAPS_VIDEO;
         }
         if (pLayer->m_sound.m_pImedia)
         {
            int32 caps = 0;
            IMEDIA_GetMediaParm(pLayer->m_sound.m_pImedia, nParmID, &caps, NULL);
            *p1 |= caps;
         }
         if (pLayer->m_image.m_pImedia)
         {
            int32 caps = 0;
            IMEDIA_GetMediaParm(pLayer->m_image.m_pImedia, nParmID, &caps, NULL);
            *p1 |= caps;
         }
      }
      if (p2)
      {
         *p2 = 0;
      }
      break;

   case MM_PARM_RECT:
      if ( !p1 | !pme )
      {
         return EFAILED;
      }

      MEMCPY( (void *)p1, &pme->m_rectImage, sizeof(AEERect));
      break;

   case MM_PARM_FRAME:
      if (pLayer->m_image.m_pImedia)
      {
         nRet = IMEDIA_GetMediaParm(pLayer->m_image.m_pImedia, nParmID, p1, p2);
      }
      else
      {
         nRet = EUNSUPPORTED;
      }
      break;
   default:
      if (pLayer->m_sound.m_pImedia)
      {
         nRet = IMEDIA_GetMediaParm(pLayer->m_sound.m_pImedia, nParmID, p1, p2);
      }
      if (pLayer->m_image.m_pImedia && nRet == EUNSUPPORTED)
      {
         nRet = IMEDIA_GetMediaParm(pLayer->m_image.m_pImedia, nParmID, p1, p2);
      }
      break;
   }

   return nRet;
}

/*==================================================================

==================================================================*/
static int OEMMediaMultipleFile_Play(IMedia *po)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int nRet = EFAILED;

   pLayer->m_playing = TRUE;
   switch(pLayer->m_type)
   {
      case NOT_SUPPORTED:
         nRet = EFAILED;
         break;
      case COMBINED:
         // Put combined into the sound object
         nRet = IMEDIA_Play(pLayer->m_sound.m_pImedia);
         break;
      case IMAGE_FIRST:
         if (pLayer->m_iImageReady)
         {
            // Image is not ready, return fail
            OEMMediaMultipleFile_DisplayImage(po);
            IMEDIA_Play(pLayer->m_sound.m_pImedia);
            nRet = EFAILED;
         }
         break;
      case SIMULTANEOUS:
         nRet = IMEDIA_Play(pLayer->m_image.m_pImedia);
         nRet = IMEDIA_Play(pLayer->m_sound.m_pImedia);
         break;
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
static int OEMMediaMultipleFile_RecordUnsupported(IMedia *po)
{
   return EUNSUPPORTED;
}

/*==================================================================

==================================================================*/
static int OEMMediaMultipleFile_Stop(IMedia *po)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int            nRet;

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

   pLayer->m_playing = FALSE;

   if (pLayer->m_image.m_pImedia)
   {
      IMEDIA_Stop(pLayer->m_image.m_pImedia);
   }
   if (pLayer->m_sound.m_pImedia)
   {
      IMEDIA_Stop(pLayer->m_sound.m_pImedia);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMultipleFile_Seek(IMedia *po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int            nRet;

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_image.m_pImedia)
   {
      IMEDIA_Seek(pLayer->m_image.m_pImedia, eSeek, lTimeMS);
   }
   if (pLayer->m_sound.m_pImedia)
   {
      IMEDIA_Seek(pLayer->m_sound.m_pImedia, eSeek, lTimeMS);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMultipleFile_Pause(IMedia *po)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int            nRet;

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_image.m_pImedia)
   {
      IMEDIA_Pause(pLayer->m_image.m_pImedia);
   }
   if (pLayer->m_sound.m_pImedia)
   {
      IMEDIA_Pause(pLayer->m_sound.m_pImedia);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMultipleFile_Resume(IMedia *po)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int            nRet;

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_image.m_pImedia)
   {
      IMEDIA_Resume(pLayer->m_image.m_pImedia);
   }
   if (pLayer->m_sound.m_pImedia)
   {
      IMEDIA_Resume(pLayer->m_sound.m_pImedia);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaMultipleFile_GetTotalTime(IMedia *po)
{
   OEMMediaMultipleFile *pme = (OEMMediaMultipleFile *)po;
   OEMMediaMultipleFileLayer *pLayer = pme->m_pMultipleFile; 
   int sound = 0;
   int image = 0;
   int nRet;

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   if (pLayer->m_sound.m_pImedia)
   {
      sound = IMEDIA_GetTotalTime(pLayer->m_sound.m_pImedia);
   }
   if (pLayer->m_image.m_pImedia)
   {
      image = IMEDIA_GetTotalTime(pLayer->m_image.m_pImedia);
   }
   return MAX(sound,image);
}

/*==================================================================
IMediaMultipleFile constructor functions
==================================================================*/
static OEMMediaMultipleFileLayer *OEMMediaMultipleFileLayer_New(IMedia *po, PFNNOTIFY pfn)
{
   OEMMediaMultipleFileLayer *  pme = (OEMMediaMultipleFileLayer *)MALLOC(sizeof(OEMMediaMultipleFileLayer));
   int           i;

   if (!pme || !po)
      return NULL;

   /* Only allow one instantiation of the MultipleFile IMedia object */
   if (guMultipleFileRefCnt != 0)
      return NULL;

   guMultipleFileRefCnt++;

   pme->m_pMedia = (AEEMedia *)po;
   pme->m_pAppContext = AEE_GetAppContext();

   // Initialize callbacks.
   for (i = 0; i < MAX_CB; i++)
   {
      pme->m_cb[i].bInUse = FALSE;
      pme->m_cb[i].cmdNotify.pIMedia = po;
      pme->m_cb[i].cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;
      pme->m_cb[i].cmdNotify.nStatus = 0;
      CALLBACK_Init(&pme->m_cb[i].cb, OEMMediaMultipleFile_Notify, &pme->m_cb[i]);
   }

   pme->m_pfnNotify = pfn;  // pfn is OEMMediaMultipleFile_CallbackNotify

   if (AEEObjectMgr_Register(pme->m_pMedia, &pme->m_hObject) != SUCCESS)
   {
      FREE(pme);
      return NULL;
   }

   return pme;
}

#endif  // FEATURE_APP_MultipleFile
