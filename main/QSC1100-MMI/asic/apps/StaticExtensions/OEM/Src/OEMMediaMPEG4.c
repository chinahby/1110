/*===========================================================================

FILE: OEMMPEG4MPEG4.c

SERVICES: MPEG4 functionality for IMedia

DESCRIPTION
   This file implements MPEG4 functionality that can be used by IMedia-based
   classes.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=========================================================================
                             Edit History

$Header: //depot/asic/msmshared/services/Qtv/QTV.04.04/StaticExtensions/OEM/Src/OEMMediaMPEG4.c#10 $
$DateTime: 2008/01/31 15:28:14 $
$Change: 602202 $

=========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "BREWVersion.h"
#include "msg.h"
#include "OEMMedia.h"
#include "OEMObjectMgr.h"
#include "OEMMediaMPEG4.h"
#include "AEEMimeTypes.h"
#include "AEEMediaFormats.h"
#include "AEEMediaMPEG4.h"
#include "AEEFile.h"
#include "fs.h"
#include "OEMCommonDisp.h"    /* For BREW_OFFSET_X/Y */
#include "OEMFeatures.h"
#include "rex.h"
#include "AEEMediaUtil.h"
#include "AEE_OEMFile.h"
#include "OEMFS.h"
#if MIN_BREW_VERSION(3,0)
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* MIN_BREW_VERSION(3,0) */

#ifdef FEATURE_APP_MEDIA_MULTIPLE_FILE
#include "OEMMediaMultipleFile.h"
#endif

#if defined(FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_QOS_SELECTION)|| \
    defined (FEATURE_QTV_PDP_SELECTION)
#error code not present
#endif

#ifdef FEATURE_QTV_MDP
#include "mdp_drv_api.h"
#endif /* FEATURE_QTV_MDP */

#ifdef   FEATURE_APP_MPEG4

// Define pass thru routine as AEE_SetAppContext is defined only in BREW 2.x
#if MIN_BREW_VERSION(3,0)
static OEMCriticalSection gcmCriticalSection;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&gcmCriticalSection)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&gcmCriticalSection)
# define AEE_SetAppContext AEE_EnterAppContext
#else /* MIN_BREW_VERSION(3,0) */
# define MUTEX_LOCK() OEMOS_MutexStart(); {
# define MUTEX_UNLOCK() } OEMOS_MutexStop();
#endif /* MIN_BREW_VERSION(3,0) */

/*==============================================================================

                               MACRO DEFINITIONS

==============================================================================*/
#define OEMMPEG4_MAX_CB     8
/* How long to poll the UI thread to release the callbacks */
#define UI_TRANSITION_TO_RUN_TIMEOUT        500 /* in ms */

/*==============================================================================

                             EXTERNAL DECLARATIONS

==============================================================================*/
extern int OEMMediaMPEG42PV_GetClipInfo(void);
extern int OEMMediaMPEG42PV_Init(void);
extern int OEMMediaMPEG42PV_OpenURN(char *videoFileName, char *audioFileName);
extern int OEMMediaMPEG42PV_OpenBufferURN(  unsigned char *pVideoBuf, uint32 dwVideoBufSize,
                                                unsigned char *pAudioBuf, uint32 dwAudioBufSize );
extern int OEMMediaMPEG42PV_Pause(void);
extern int OEMMediaMPEG42PV_Play(int);
extern int OEMMediaMPEG42PV_RegisterForCallback(void *);
extern int OEMMediaMPEG42PV_Seek(AEEMediaSeek eSeek, int32 TimeOrFrame);
extern int OEMMediaMPEG42PV_Stop(void);
extern int OEMMediaMPEG42PV_Terminate(void);
extern int OEMMediaMPEG42PV_SetVolume(int32 volume);
extern int OEMMediaMPEG42PV_GetVolume(void);
extern int OEMMediaMPEG42PV_GetMediaSpec(AEEMediaMPEG4Spec **pSpec, int32 *pSize);
#ifndef FEATURE_QTV_VIDEO_DISABLE
extern int OEMMediaMPEG42PV_GetFrame(IBitmap** pb);
extern int OEMMediaMPEG42PV_RotateVideo(AEEMediaMPEG4RotationType RotationType);
extern int OEMMediaMPEG42PV_ScaleVideo(AEEMediaMPEG4ScalingType ScaleFactor);
#endif /* FEATURE_QTV_VIDEO_DISABLE */
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
extern int OEMMediaMPEG42PV_GetTelopSubString(AEETelopTextSubString **ppTextSubStr, int32 nIndex);
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */
extern int OEMMediaMPEG42PV_GetTrackTimeScale(AEEMediaMPEG4TrackType trackType, uint32 *pTimeScale);
extern int OEMMediaMPEG42PV_SetAudOverride(int32 audioFormat);
extern int OEMMediaMPEG42PV_SetTickUpdateInterval(uint32 updateInterval);
extern int OEMMediaMPEG42PV_SetStartPos(int32 lTimeMS);

extern int OEMMediaMPEG42PV_GetPlaybackPos(AEEMediaPlaybackPos * pPlayPos);
extern int OEMMediaMPEG42PV_SetLoopTrack(boolean bLoop);
static int OEMMediaMPEG4_OpenURN(AEEMediaDataEx *pmdExList, int nCount);

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

#ifdef FEATURE_QTV_MFDRM
extern int OEMMediaMPEG42PV_SetDRMKey(void *pDrmKey, int32 sizeOfDrmKey);
#endif

#ifdef FEATURE_QTV_STREAM_RECORD
#error code not present
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
#endif

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif 

#ifdef FEATURE_QTV_SKT_MOD_MIDI
extern int OEMMediaMPEG42PV_GetMidiDataSize(uint32 *pSize);
extern int OEMMediaMPEG42PV_GetMidiData(uint8 *pBuf, uint32 *pSize);
extern int OEMMediaMPEG42PV_GetLinkDataSize(uint32 *pSize);
extern int OEMMediaMPEG42PV_GetLinkData(uint8 *pBuf, uint32 *pSize);
#endif /* FEATURE_QTV_SKT_MOD_MIDI */

#ifdef FEATURE_QTV_MDP
/* It would be better if we didn't extern this function here, but it is
** done to to provide the MDP with a way to release the frame buffer once
** it is done using it. Once the MDP is given a frame buffer, it takes
** ownership of it, and the frame must not be freed by another task. A
** cleaner buffer management mechanism will need to be implemented to support
** future enhancements.
*/
extern mp4_free_frame_buffer(void *pFrameBuf);
#endif /* FEATURE_QTV_MDP */

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
#error code not present
#endif

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
//
// MPEG4 IMedia objects must contain a reference to this object.
// Note: This class is a friend of IMedia and/or IMedia-based classes.
//
typedef struct OEMMediaMPEG4Layer
{
   AEEMedia *        m_pMedia;           // Owner IMedia object
   void *            m_pAppContext;      // App context
#ifdef FEATURE_MP4_MSM6050
   OBJECTCONTEXT     m_cxtMedia;         // Context returned by ObjectMgr
#else /* FEATURE_MP4_MSM6050 */
   AEEObjectHandle   m_hObject;          // Context returned by ObjectMgr
#endif /* FEATURE_MP4_MSM6050 */
   PFNNOTIFY         m_pfnNotify;        // IMedia notification function

   AEEMediaCallback  m_cb[OEMMPEG4_MAX_CB];
   uint32            m_dwCaps;           // Capabilities of IMedia
   ISound *          m_pISound;
   AEEMediaCallback  m_cbCommand;
   boolean           m_bFrameCBEnabled;  // Frame callbacks enabled?
   boolean           m_bClearScreen;     // Clear screen on the next frame callback?
   IDisplay *        m_pIDisplay;        // Pointer to display interface
   uint32            m_iPrevHeight;      // Dimensions of previous frame.  Used
   uint32            m_iPrevWidth;       // to know when to clear screen
   boolean bTerminating;				 //Set to true before calling OEMMediaMPEG42PV_Terminate
} OEMMediaMPEG4Layer;

OBJECT(OEMMediaMPEG4)
{
   INHERIT_AEEMedia(IMediaMPEG4);
   AEERect  m_rectImage;
   boolean  m_bRelMark;
   OEMMediaMPEG4Layer *m_pMPEG4;
#ifdef FEATURE_SMALL_DISPLAY_SCREEN
   boolean  m_bscalevideo;                //if this flag is true scale video to fit in available screen dimensions
#endif /* FEATURE_SMALL_DISPLAY_SCREEN */
};


/*==============================================================================

                      DATA DECLARATIONS

==============================================================================*/
#ifdef FEATURE_MP4_MSM6050
static CObjectMgr   *gpObjectMgr = NULL;
#endif /* FEATURE_MP4_MSM6050 */
static uint32        guMPEG4RefCnt = 0;

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

/*===============================================================================

      COMMON FUNCTION DECLARATIONS
      These functions are used by IMedia classes implemented in this file.

===============================================================================*/

/*===========================================================================

                      IMediaMPEG4 DECLARATIONS

===========================================================================*/
static uint32 OEMMediaMPEG4_Release(IMedia * po);
static int    OEMMediaMPEG4_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int    OEMMediaMPEG4_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 *  pP2);
static int    OEMMediaMPEG4_Play(IMedia * po);
static int    OEMMediaMPEG4_RecordUnsupported(IMedia * po);
static int    OEMMediaMPEG4_Stop(IMedia * po);
static int    OEMMediaMPEG4_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int    OEMMediaMPEG4_Pause(IMedia * po);
static int    OEMMediaMPEG4_Resume(IMedia * po);
static int    OEMMediaMPEG4_GetTotalTime(IMedia * po);
static IBase *OEMMediaMPEG4_New(IShell * ps, AEECLSID cls);
static void   OEMMediaMPEG4_Delete(IMedia * po, boolean bFree);
static OEMMediaMPEG4Layer * OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn);
static void   OEMMediaMPEG4Layer_Delete(OEMMediaMPEG4Layer * pMPEG4);

#if MIN_BREW_VERSION(3,0)
static int OEMMediaMpeg4_FilePathEx(AEEMediaDataEx * pmdList, int nListCount);
#endif

static const VTBL(IMediaMPEG4) gMediaMPEG4Funcs =
{
   AEEMedia_AddRef,
   OEMMediaMPEG4_Release,
   AEEMedia_QueryInterface,
   AEEMedia_RegisterNotify,
   OEMMediaMPEG4_SetMediaParm,
   OEMMediaMPEG4_GetMediaParm,
   OEMMediaMPEG4_Play,
   OEMMediaMPEG4_RecordUnsupported,
   OEMMediaMPEG4_Stop,
   OEMMediaMPEG4_Seek,
   OEMMediaMPEG4_Pause,
   OEMMediaMPEG4_Resume,
   OEMMediaMPEG4_GetTotalTime,
   AEEMedia_GetState
};

/*===============================================================================

      MPEG4 FUNCTION DEFINITIONS

===============================================================================*/

/*==================================================================
   Destructor
==================================================================*/
void OEMMediaMPEG4_Delete(IMedia * po, boolean bFree)
{
   OEMMediaMPEG4      * pme = (OEMMediaMPEG4 *)po;
   if (!pme->m_bRelMark)
   {
      pme->m_bRelMark = TRUE;

      OEMMediaMPEG4Layer_Delete(pme->m_pMPEG4);

      // Call base class destructor
      AEEMedia_Delete(po);
   }

   // Free object memory
   if (bFree)
      FREE(po);
}

/*==================================================================
   Destructor
==================================================================*/
void OEMMediaMPEG4Layer_Delete(OEMMediaMPEG4Layer * pMPEG4)
{
  int i;

  pMPEG4->bTerminating = TRUE;
  OEMMediaMPEG42PV_Terminate();
  pMPEG4->bTerminating = FALSE;

#ifdef FEATURE_MP4_MSM6050
  CObjectMgr_Unregister(gpObjectMgr, pMPEG4->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
  AEEObjectMgr_Unregister(pMPEG4->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */

  /* Cancel any pending callbacks */
  for (i = 0; i < OEMMPEG4_MAX_CB; i++)
  {
	pMPEG4->m_cb[i].bInUse = FALSE;
    CALLBACK_Cancel(&pMPEG4->m_cb[i].cb);
  }

  MM_RELEASEIF(pMPEG4->m_pISound);
  MM_RELEASEIF(pMPEG4->m_pIDisplay);

  FREE(pMPEG4);
}

/*==============================================================================

==============================================================================*/
static uint32 OEMMediaMPEG4_Release(IMedia * po)
{
   int32 nRef = AEEMedia_Release(po);

   if (!nRef)
   {
      OEMMediaMPEG4_Delete(po, TRUE);
      guMPEG4RefCnt = 0;
      /* Set guMPEG4RefCnt to 0 since we're allowing only one instance of
      ** MPEG4 media object.
      */
   }

   return nRef;
}
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
#endif

/*==============================================================================

==============================================================================*/
static int OEMMediaMPEG4_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)po;
   AEEMedia * pMedia = pme->m_pMPEG4->m_pMedia;
   int nRet = SUCCESS;
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
#endif


   switch (nParmID)
   {
     #if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
#error code not present
     #endif

     break;

      case MM_PARM_VOLUME:
      {
        if (!pme->m_pMPEG4->m_pISound)
        {
          nRet = EUNSUPPORTED;
        }
        else if ((uint16)p1 > AEE_MAX_VOLUME)
        {
          nRet =  EBADPARM;
        }
        else
        {
          nRet = OEMMediaMPEG42PV_SetVolume(p1);
        }
        break;
      }
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
      case MM_PARM_MEDIA_DATA:
        if(p2 > 0)
        {
          /* this is extended data */
          nRet = OEMMediaMPEG4_OpenURN((AEEMediaDataEx *)p1, (int)p2);
          if (nRet == SUCCESS)
          {
            pme->m_nState = MM_STATE_READY;
          }
        }
        else
        {
          nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
          if (nRet == SUCCESS)
          {
            switch( pMedia->m_md.clsData )
            {
              case MMD_BUFFER:  /* playback from buffer */

#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /* FEATURE_QTV_PSEUDO_STREAM */
  				        /* here video and audio buffers are same */
                  nRet = OEMMediaMPEG42PV_OpenBufferURN(
                                    (unsigned char *)pMedia->m_md.pData,
                                    pMedia->m_md.dwSize,
                                    (unsigned char *)pMedia->m_md.pData,
                                    pMedia->m_md.dwSize);

                break;


#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif /* FEARTURE_QTV_PSEUDO_STREAM */
            case MMD_FILE_NAME:

                nRet = OEMMediaMPEG42PV_OpenURN(pMedia->m_pszFile, pMedia->m_pszFile);
                break;


              default:
                nRet = EUNSUPPORTED;
            } /* switch( pMedia->m_md.clsData ) */
          } /* end of if(AEEMedia_SetMediaParm == SUCCESS) */
        }
        break;

      case MM_MP4_AUDIO_VIDEO_MEDIA_DATA:
         nRet = OEMMediaMPEG42PV_OpenURN((char *)p1, (char *)p2);
         if (nRet == SUCCESS)
         {
            pme->m_nState = MM_STATE_READY;
         }
         break;

      case MM_PARM_RECT:
         MEMCPY(&pme->m_rectImage, (void *)p1, sizeof(AEERect));
         break;

      case MM_PARM_FRAME:
         pme->m_pMPEG4->m_bFrameCBEnabled = p1;
         break;

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
      case MM_MP4_PARM_ROTATION:
         OEMMediaMPEG42PV_RotateVideo((AEEMediaMPEG4RotationType)p1);
         pme->m_pMPEG4->m_bClearScreen = !pme->m_pMPEG4->m_bFrameCBEnabled;
         break;

      case MM_MP4_PARM_SCALING:
         OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)p1);
         pme->m_pMPEG4->m_bClearScreen = !pme->m_pMPEG4->m_bFrameCBEnabled;
         break;
#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

      case MM_MP4_PARAM_AUDIO_OVERRIDE:
         OEMMediaMPEG42PV_SetAudOverride((int32)p1);
         break;

#ifdef FEATURE_QTV_STREAMING
#error code not present
#endif /* FEATURE_QTV_STREAMING */

      case MM_PARM_TICK_TIME:
         OEMMediaMPEG42PV_SetTickUpdateInterval((uint32)p1);
         break;

      case MM_PARM_POS:
         nRet = OEMMediaMPEG42PV_SetStartPos((uint32)p1);
         break;

      case MM_MP4_PARM_LOOP_TRACK:
         nRet = OEMMediaMPEG42PV_SetLoopTrack((boolean)p1);
         break;

#ifdef FEATURE_QTV_MFDRM
      case MM_MP4_PARM_DECRYPT_KEY:
         nRet = OEMMediaMPEG42PV_SetDRMKey((void*)p1, p2);
         break;
#endif


#ifdef FEATURE_QTV_STREAM_RECORD
#error code not present
#endif

      default:
         nRet = EUNSUPPORTED;
   }

   return nRet;
}


/*==============================================================================

==============================================================================*/
static int OEMMediaMPEG4_GetMediaParm(IMedia * po, int nParmID, int32 * p1, int32 * p2)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)po;
   OEMMediaMPEG4Layer * pMPEG4 = pme->m_pMPEG4;
   int nRet = SUCCESS;

   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_GetMediaParm(po, nParmID, p1, p2);

   switch (nParmID)
   {
      case MM_PARM_VOLUME:
        nRet = OEMMediaMPEG42PV_GetVolume();
        break;

      case MM_PARM_CLSID:
         if (p1 == NULL)
            return EFAILED;

         *p1 = AEECLSID_MEDIAMPEG4;
         break;

      case MM_PARM_CAPS:
         if (p1 == NULL)
            return EFAILED;

         *p1 = (int32)pMPEG4->m_dwCaps;
         if (p2)
            *p2 = 0;
         break;

      case MM_PARM_RECT:
         if ( !p1 | !pme )
         {
           return EFAILED;
         }

         MEMCPY( (void *)p1, &pme->m_rectImage, sizeof(AEERect));
         break;

#ifndef FEATURE_QTV_VIDEO_DISABLE
      case MM_PARM_FRAME:
         if ((p1 == NULL && p2 == NULL) ||
             (p1 != NULL && p2 != NULL))
         { /* Invalid parameters */
            return EFAILED;
         }

         if (p1 == NULL)
         { /* App is calling IMEDIA_IsFrameCallback */
            *p2 = pme->m_pMPEG4->m_bFrameCBEnabled;
         }
         else
         { /* App is calling IMEDIA_GetFrame */
            nRet = OEMMediaMPEG42PV_GetFrame((IBitmap**)p1);
         }
         break;
#endif /* FEATURE_QTV_VIDEO_DISABLE */

      case MM_MP4_PARM_TELOP_SUB_STR:
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
         OEMMediaMPEG42PV_GetTelopSubString((AEETelopTextSubString **)p1, *p2);
         break;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

      case MM_MP4_PARM_MEDIA_SPEC:
         nRet = OEMMediaMPEG42PV_GetMediaSpec((AEEMediaMPEG4Spec **)p1, p2);
         break;

      case MM_MP4_PARM_TRACK_TIME_SCALE:
         nRet = OEMMediaMPEG42PV_GetTrackTimeScale((AEEMediaMPEG4TrackType)(int32)p1, (uint32 *)p2);
         break;

      case MM_MP4_PARAM_PLAYBACK_POS:
         nRet = OEMMediaMPEG42PV_GetPlaybackPos((AEEMediaPlaybackPos *)p1);
         break;

#ifdef FEATURE_QTV_SKT_MOD_MIDI
      case MM_MP4_PARM_MIDI_SIZE:
        nRet = OEMMediaMPEG42PV_GetMidiDataSize((uint32 *)p1);
        break;

      case MM_MP4_PARM_MIDI_DATA:
        nRet = OEMMediaMPEG42PV_GetMidiData((uint8 *)p1, (uint32 *)p2);
        break;

      case MM_MP4_PARM_LINK_SIZE:
        nRet = OEMMediaMPEG42PV_GetLinkDataSize((uint32 *)p1);
        break;

      case MM_MP4_PARM_LINK_DATA:
        nRet = OEMMediaMPEG42PV_GetLinkData((uint8 *)p1, (uint32 *)p2);
        break;

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

      default:
         nRet = EUNSUPPORTED;
   }
   /* TODO
   return OEMMediaMPEG4Layer_GetMediaParm(pme->m_pMPEG4, nParmID, p1, p2);
   */
   return nRet;
}

/*==================================================================

==================================================================*/
#ifdef FEATURE_QTV_MDP
uint32 mpeg_mdp_id = 0;
#endif /* FEATURE_QTV_MDP */
static int OEMMediaMPEG4_Play(IMedia * po)
{
   int nRet;
#ifdef FEATURE_QTV_MDP
   MDPIMG mdpImg;
   IDIB *pDib;
   IBitmap *bmp;
#endif /* FEATURE_QTV_MDP */

#if defined(FEATURE_QTV_MDP) || defined(FEATURE_SMALL_DISPLAY_SCREEN)
   OEMMediaMPEG4 *    pme = (OEMMediaMPEG4 *)po;
#endif

   nRet = AEEMedia_Play(po);
   if (nRet != SUCCESS)
      return nRet;

#ifdef FEATURE_QTV_MDP
   mdp_enable_all_ovimg(PRIMARY_LCD_TYPE, FALSE);

   mdpImg.dest = PRIMARY_LCD_TYPE;
   mdpImg.imgType = MDP_RGB565;
   mdpImg.width = 240;
   mdpImg.cwin.x1 = 0;
   mdpImg.cwin.x2 = 239;
   mdpImg.cwin.y1 = 244;
   mdpImg.cwin.y2 = 319;
   mdpImg.lcd_x = 0;
   mdpImg.lcd_y = 244;
   mdpImg.mdpOp = MDPOP_NOP;

   IDISPLAY_GetDeviceBitmap(pme->m_pMPEG4->m_pIDisplay, &bmp);
   IBITMAP_QueryInterface(bmp, AEECLSID_DIB, (void**)&pDib);
   mdpImg.bmy_addr = (uint32 *) pDib->pBmp;
   mpeg_mdp_id = mdp_register_ovimg(&mdpImg);
#endif /* FEATURE_QTV_MDP */

#ifdef FEATURE_SMALL_DISPLAY_SCREEN
  //Turn on the default scaling option. 
  pme->m_bscalevideo = TRUE;
#endif 

   return OEMMediaMPEG42PV_Play(0);
}

/*==================================================================
   Unsupported functions -- used in vtbl of IMedia classes
==================================================================*/
static int OEMMediaMPEG4_RecordUnsupported(IMedia * po)
{
   return EUNSUPPORTED;
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Stop(IMedia * po)
{
   int            nRet;

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

#ifdef FEATURE_QTV_MDP
   mdp_deregister_ovimg(mpeg_mdp_id);
   mdp_enable_all_ovimg(PRIMARY_LCD_TYPE, TRUE);
#endif /* FEATURE_QTV_MDP */

   return OEMMediaMPEG42PV_Stop();
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   int            nRet;

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

   return OEMMediaMPEG42PV_Seek(eSeek, lTimeMS);
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Pause(IMedia * po)
{
   int            nRet;

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
#error code not present
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
   {
      nRet = AEEMedia_Pause(po);
      if (nRet != SUCCESS)
         return nRet;
   }
   return OEMMediaMPEG42PV_Pause();
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_Resume(IMedia * po)
{
   int            nRet;

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   return OEMMediaMPEG42PV_Play(-1);
}

/*==================================================================

==================================================================*/
static int OEMMediaMPEG4_GetTotalTime(IMedia * po)
{
   int            nRet;

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   return OEMMediaMPEG42PV_GetClipInfo();
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context.
==================================================================*/
void OEMMediaMPEG4_CallbackNotify(AEEMediaCallback * pcb)
{
  OEMMediaMPEG4 *    pme;

#ifdef FEATURE_SMALL_DISPLAY_SCREEN
  uint32 scr_width=0, scr_height=0;
#endif

  if (!pcb)
    return;

  pme = (OEMMediaMPEG4 *)pcb->cmdNotify.pIMedia;

  if (  pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
        pcb->cmdNotify.nStatus == MM_STATUS_FRAME &&
        !pme->m_pMPEG4->m_bFrameCBEnabled )
  {
    /* We're about to send an MM_STATUS_FRAME to the Brew app.  However, they
    * haven't enabled them, so handle it ourselves and return.
    */
    IBitmap      * pFrame;
    AEEBitmapInfo  bi;
    uint16 x,  y;
    uint16 dx, dy;

    /*
    * IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
    */
    IMEDIA_GetFrame(pme, &pFrame);
    if (pFrame != NULL)
    {
      // Get the bitmap size
      IBITMAP_GetInfo(pFrame, &bi, sizeof(bi));

      /* Should clear the screen if we've indicated and the dimensions of the
      * current frame have changed from the previous one.
      *
      * 'and' is not sufficient here.  There's a race condition where a button masher
      * pushing 'rotate' at full speed can cause two rotations between processings of
      * this loop, resulting in an improper screen erase between rotations.  We need
      * to clear the screen whenever the flag is high OR the image dimensions have changed.
      */
      if( pme->m_pMPEG4->m_bClearScreen ||
          (pme->m_pMPEG4->m_iPrevWidth  != bi.cx ||
          pme->m_pMPEG4->m_iPrevHeight != bi.cy) )
      {
        /* Frame callbacks are not enabled, so clear the screen */
#ifndef FEATURE_QTV_MDP
        IDISPLAY_EraseRgn(  pme->m_pMPEG4->m_pIDisplay,
                            pme->m_rectImage.x,
                            pme->m_rectImage.y,
                            pme->m_rectImage.dx,
                            pme->m_rectImage.dy );
#endif /* FEATURE_QTV_MDP */
        pme->m_pMPEG4->m_bClearScreen = FALSE;
      }

#ifdef FEATURE_SMALL_DISPLAY_SCREEN
      //For 6030 we will turn on m_bscalevideo at the start when we play a clip.
      //We will scale the video only if the available screen size is less
      //than the video size. For scaling down we have 3 available options
      //MM_MPEG4_0P25X_SHRINK,MM_MPEG4_0P75X_SHRINK & MM_MPEG4_2X_SHRINK.
      if(pme->m_bscalevideo)
      {
         //Calculate the screen width available for display.
	 scr_width = pme->m_rectImage.dx - pme->m_rectImage.x;
	 scr_height = pme->m_rectImage.dy - pme->m_rectImage.y;

         if((bi.cx > scr_width) || (bi.cy > scr_height))
	 {
            //If the video size is no bigger than 30% of screen size
	    if((bi.cx <= 1.3*scr_width) && (bi.cy <= 1.3*scr_height))
            {
	       OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)MM_MPEG4_0P25X_SHRINK);
	    }
	    else //If the video size is between 30% - 60% more than screen size
	    if((bi.cx <= 1.6*scr_width) && (bi.cy <= 1.6*scr_height))
	    {
               OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)MM_MPEG4_0P75X_SHRINK); 
	    }
	    else //If the video size is bigger than 60% of the screen size
	    {
	       OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)MM_MPEG4_2X_SHRINK); 
	    }
	    pme->m_pMPEG4->m_bClearScreen = !pme->m_pMPEG4->m_bFrameCBEnabled;	
         }
         //After we have scaled the video we should turn this flag off since
         //we want the video to be scaled only once at the start of play.
         pme->m_bscalevideo = FALSE;
      }
#endif /* FEATURE_SMALL_DISPLAY_SCREEN */

      /* Save the dimensions of the current frame */
      pme->m_pMPEG4->m_iPrevWidth  = bi.cx;
      pme->m_pMPEG4->m_iPrevHeight = bi.cy;

      dx = MIN( pme->m_rectImage.dx, bi.cx );
      dy = MIN( pme->m_rectImage.dy, bi.cy );
      x = (uint16)((pme->m_rectImage.dx - dx) / 2 + pme->m_rectImage.x);
      y = (uint16)((pme->m_rectImage.dy - dy) / 2 + pme->m_rectImage.y);

#ifdef FEATURE_QTV_MDP
      {
        /* Use MDP for post processing and display */
        MDPIMG mdpImg;
        IDIB *pYCbCr;

        /* Obtain IDIB pointer from an IBitmap */
        if (IBITMAP_QueryInterface(pFrame, AEECLSID_DIB, (void**)&pYCbCr) != SUCCESS)
        {
          MSG_ERROR("Invalid interface ID",0,0,0);
          mp4_free_frame_buffer(NULL);
          IBITMAP_Release(pFrame);
          return;
        }
        if (!pYCbCr)
        {
          MSG_ERROR("Could not get IDIB",0,0,0);
          mp4_free_frame_buffer(NULL);
          IBITMAP_Release(pFrame);
          return;
        }

        mdpImg.dest = PRIMARY_LCD_TYPE;
        mdpImg.imgType = MDP_YCBCR420;
        mdpImg.bmy_addr = (uint32 *)pYCbCr->pBmp;
        mdpImg.cbcr_addr = (uint32 *)&pYCbCr->pBmp[bi.cx * bi.cy];

        mdpImg.width = bi.cx;
        mdpImg.cwin.x1 = 0;
        mdpImg.cwin.x2 = dx-1;
        mdpImg.cwin.y1 = 0;
        mdpImg.cwin.y2 = dy-1;
        mdpImg.lcd_x = x;
        mdpImg.lcd_y = y;
        mdpImg.mdpOp = MDPOP_NOP;

        /* Display the specified image on the LCD screen */
        if (1 != mdp_disp_update(&mdpImg))
        {
          MSG_ERROR("MDP display update failed",0,0,0);
        }

        /* The IDIB pointer must be released once we are finished using it */
        IDIB_Release(pYCbCr);

        /* Signal the codec to tell it that the MDP has finished accessing the
        ** frame buffer. The call to mp4_free_frame_buffer() is tentative;
        ** a cleaner buffer management mechanism will need to be implemented to
        ** support future enhancements.
        */
        mp4_free_frame_buffer(NULL);
      }
#else
      /* Display the frame at centered on the screen */
      IDISPLAY_BitBlt(pme->m_pMPEG4->m_pIDisplay, x, y, dx, dy, pFrame, 0, 0, AEE_RO_COPY);
      IDISPLAY_Update(pme->m_pMPEG4->m_pIDisplay);
#endif /* FEATURE_QTV_MDP */
      IBITMAP_Release(pFrame);
    }
#ifdef FEATURE_QTV_MDP
    else
    {
      MSG_ERROR("pFrame is NULL",0,0,0);
      mp4_free_frame_buffer(NULL);
    }
#endif /* FEATURE_QTV_MDP */
    /* Do not send this callback to the app as they are not registered for it */
    return;
  }
  AEEMedia_CallbackNotify((AEEMedia *)pme, pcb);
}

#ifdef FEATURE_MP4_MSM6050
static void OEMMediaMPEG4_ObjMgrNotify(void * pUser, IBase * pObject)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)pUser;

   if (!pUser || !pObject)
      return;

   // Stop the media, if playback/recording is in progress. This also releases
   // most of device multimedia resources.
   if (AEEMedia_IsPlayRecOn(pme))
      IMEDIA_Stop((IMedia *)pme);

   // Release the allocated resources but do not delete the object itself.
   OEMMediaMPEG4_Delete((IMedia *)pUser, FALSE);
}
#endif /* FEATURE_MP4_MSM6050 */

/*==================================================================

==================================================================*/
static void OEMMediaMPEG4_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam)
{
   OEMMediaMPEG4 *      pme = (OEMMediaMPEG4 *)pUser;
   AEESoundCmdData *    pData = (AEESoundCmdData *)dwParam;
   AEEMediaCallback     mcb;
   void *               paLast;

   MEMCPY(&mcb, &pme->m_pMPEG4->m_cbCommand, sizeof(AEEMediaCallback));

   mcb.bInUse = TRUE;
   mcb.cmdNotify.nSubCmd = MM_PARM_VOLUME;
   mcb.cmdNotify.nCmd = (eCBType == AEE_SOUND_VOLUME_CB) ? MM_CMD_GETMEDIAPARM : MM_CMD_SETMEDIAPARM;
   mcb.cmdNotify.nStatus = (eStatus == AEE_SOUND_SUCCESS) ? MM_STATUS_DONE : MM_STATUS_ABORT;
   mcb.cmdNotify.pCmdData = NULL;
   mcb.cmdNotify.dwSize = 0;

   if (pData && eCBType == AEE_SOUND_VOLUME_CB && eStatus == AEE_SOUND_SUCCESS)
   {
      mcb.cmdNotify.pCmdData = (void *)(uint32)pData->wVolume;
      mcb.cmdNotify.dwSize = sizeof(pData->wVolume);
   }

   paLast = AEE_SetAppContext(pme->m_pMPEG4->m_pAppContext);
   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
   AEE_SetAppContext(paLast);
}

/*==================================================================
FUNCTION:  OEMMediaMPEG4_Notify()
DESCRIPTION:
   Callback function queued by the PV MPEG4 engine to run in BREW's
   context.
==================================================================*/
static void OEMMediaMPEG4_Notify(AEEMediaCallback * pcb)
{
   OEMMediaMPEG4 *      pMedia;
   OEMMediaMPEG4Layer * pme;
   AEEMediaCallback   mcb;

   if (!pcb)
      return;

   pMedia = (OEMMediaMPEG4 *)pcb->cmdNotify.pIMedia;
   if (!pMedia)
      return;

   pme = pMedia->m_pMPEG4;
   if (!pme)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));

   /* Free the callback structure */
   pcb->bInUse = FALSE;

   if (pme->m_pfnNotify)
      pme->m_pfnNotify(&mcb);
}

/*==================================================================
   IMediaMPEG4 constructor functions
==================================================================*/
OEMMediaMPEG4Layer * OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn)
{
  OEMMediaMPEG4Layer *   pme;
  int           i;
  int           nRetVal;
  AEEDeviceInfo di;

  /* Only allow one instantiation of the MPEG4 IMedia object */
  if( !po || (guMPEG4RefCnt != 0) )
    return NULL;

  pme = MALLOC(sizeof(OEMMediaMPEG4Layer));

  if (!pme)
    return NULL;

  MEMSET(pme,0,sizeof(OEMMediaMPEG4Layer));
  pme->m_pMedia = (AEEMedia *)po;
  pme->m_pAppContext = AEE_GetAppContext();

#ifdef FEATURE_MP4_MSM6050
  gpObjectMgr = CObjectMgr_New(pme->m_pMedia->m_pIShell);
  if (!gpObjectMgr)
  {
    FREE(pme);
    return NULL;
  }

  pme->m_cxtMedia = CObjectMgr_Register(gpObjectMgr, (IBase*)pme->m_pMedia,
  OEMMediaMPEG4_ObjMgrNotify, pme->m_pMedia);
  if (!pme->m_cxtMedia)
  {
    FREE(pme);
    return NULL;
  }
#else /* FEATURE_MP4_MSM6050 */
  if (AEEObjectMgr_Register(pme->m_pMedia, &pme->m_hObject) != SUCCESS)
  {
    FREE(pme);
    return NULL;
  }
#endif /* FEATURE_MP4_MSM6050 */

  pme->m_pfnNotify = pfn;  // pfn is OEMMediaMPEG4_CallbackNotify
  pme->m_dwCaps = MM_CAPS_AUDIO | MM_CAPS_VIDEO;

  // Initialize callbacks.
  for (i = 0; i < OEMMPEG4_MAX_CB; i++)
  {
    pme->m_cb[i].bInUse = FALSE;
    pme->m_cb[i].cmdNotify.pIMedia = po;
    pme->m_cb[i].cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;
    pme->m_cb[i].cmdNotify.nStatus = 0;
    CALLBACK_Init(&pme->m_cb[i].cb, OEMMediaMPEG4_Notify, &pme->m_cb[i]);
  }

  ISHELL_GetDeviceInfo(pme->m_pMedia->m_pIShell, &di);

  /* Get a pointer to the IDisplay */
  nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
  if( nRetVal != SUCCESS )
  {
    MSG_FATAL( "Get IDisplay Failed(%d)", nRetVal, 0, 0 );
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    FREE(pme);
    return NULL;
  }

  nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_SOUND, (void **)&pme->m_pISound);
  if(SUCCESS != nRetVal)
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MSG_FATAL( "Get Isound Failed(%d)", nRetVal, 0, 0 );
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    FREE(pme);
    return NULL;
  }

  ISOUND_RegisterNotify(pme->m_pISound, OEMMediaMPEG4_SoundCB, pme);

  /* Initialize other variables */
  pme->m_bFrameCBEnabled = FALSE;
  pme->m_iPrevHeight = pme->m_iPrevWidth = 0;

  // At this point, the media player has not made a call informing the OEM
  // layer the size of the display area.  For now, we'll call
  // PVPlayer::Init() with a 0 x 0 display area.  When the media player sets
  // the available display area for the video, we'll forward that information
  // to the PV MPEG4 engine via PVPlayer::SetDisplayArea().
  nRetVal = OEMMediaMPEG42PV_Init();
  if( nRetVal != SUCCESS )
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MM_RELEASEIF(pme->m_pISound);
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    FREE(pme);
    return NULL;
  }

#ifdef FEATURE_MP4_MSM6050
  nRetVal = OEMMediaMPEG42PV_RegisterForCallback(pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
  nRetVal = OEMMediaMPEG42PV_RegisterForCallback((void*)pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */

  if (nRetVal != SUCCESS)
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MM_RELEASEIF(pme->m_pISound);
#ifdef FEATURE_MP4_MSM6050
    CObjectMgr_Unregister(gpObjectMgr, pme->m_cxtMedia);
#else /* FEATURE_MP4_MSM6050 */
    AEEObjectMgr_Unregister(pme->m_hObject);
#endif /* FEATURE_MP4_MSM6050 */
    pme->bTerminating=TRUE;
    OEMMediaMPEG42PV_Terminate();
	pme->bTerminating=FALSE;
    FREE(pme);
    pme = NULL;
  }

  return pme;
}

static IBase * OEMMediaMPEG4_New(IShell * ps, AEECLSID cls)
{
  IMedia *      po;
  OEMMediaMPEG4 * pme;

  // Alloc memory for the object
  po = (IMedia *)AEE_NewClass((IBaseVtbl *)&gMediaMPEG4Funcs, sizeof(OEMMediaMPEG4));
  if (!po)
    return NULL;

  // Call base class constructor
  if (SUCCESS != AEEMedia_New(po, ps, cls))
  {
    FREE(po);
    return NULL;
  }

  // Call MPEG4 Layer constructor
  pme = (OEMMediaMPEG4 *)po;
  pme->m_pMPEG4 = OEMMediaMPEG4Layer_New(po, (PFNNOTIFY)OEMMediaMPEG4_CallbackNotify);

  if ( !pme->m_pMPEG4 )
  {
    AEEMedia_Delete(po);
    FREE(po);
    return NULL;
  }
  return (IBase *)po;
}

/*===============================================================================

                     IMediaMPEG4 PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaMPEG4_Init(IShell * ps)
{
   AEEMedia_Init(ps, MT_VIDEO_MPEG4, AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/url", AEECLSID_MEDIAMPEG4);
#ifndef FEATURE_APP_SDP
   AEEMedia_Init(ps, "video/sdp", AEECLSID_MEDIAMPEG4);
#endif
   AEEMedia_Init(ps, "video/pvx", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/3gp", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/amc", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/3g2", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/k3g", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/skm", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/mfpt", AEECLSID_MEDIAMPEG4);
}

/*==================================================================
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaMPEG4_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj;

   if (guMPEG4RefCnt == 0)
   {
      pobj = OEMMediaMPEG4_New(ps, cls);
      guMPEG4RefCnt = (pobj) ? 1 : 0;
   }
   else
   {
      pobj = NULL;
   }

   *ppif = pobj;

   return pobj ? SUCCESS : ENOMEMORY;
}

/*==================================================================
   Called by the PV MPEG4 Engine callback function to queue a
   callback to execute in the BREW context
==================================================================*/
int OEMMediaMPEG4_QueueCallback(void *pClientData, int nStatus, int nCmd, int nSubCmd, void *pData, uint32 dwSize)
{
   int i,j;
   OEMMediaMPEG4      * pMedia;
   OEMMediaMPEG4Layer * pme;
   AEEMediaCallback   * pcb = NULL;

#ifdef FEATURE_MP4_MSM6050
   pMedia = (OEMMediaMPEG4 *)CObjectMgr_GetObject(gpObjectMgr, (OBJECTCONTEXT)pClientData);
#else /* FEATURE_MP4_MSM6050 */
   pMedia = (OEMMediaMPEG4 *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
#endif /* FEATURE_MP4_MSM6050 */
   pme = pMedia->m_pMPEG4;

   /* Find a free callback structure */
   MUTEX_LOCK();
   for (i = 0; i < OEMMPEG4_MAX_CB; i++)
   {
      if (pme->m_cb[i].bInUse == FALSE)
      {
         pme->m_cb[i].bInUse = TRUE;
         pcb = &pme->m_cb[i];
         break;
      }
   }
   MUTEX_UNLOCK();

   if (i == OEMMPEG4_MAX_CB)
   {
	  if(pme->bTerminating)
	  {
	      return -1;
	  }
      else	{
		  unsigned int totalSleepDuration = 0;
		  do{

              /* sleep in chuncks of 16ms, until a callback is free*/
			  rex_sleep(16);
			  totalSleepDuration += 16;
			  MUTEX_LOCK();
			  for (j = 0; j < OEMMPEG4_MAX_CB; j++)
			   {
				  if (pme->m_cb[j].bInUse == FALSE)
   				  {
					 pme->m_cb[j].bInUse = TRUE;
					 pcb = &pme->m_cb[j];
					 break;
				  }
			   }
			  MUTEX_UNLOCK();
		  }while(j== OEMMPEG4_MAX_CB && totalSleepDuration < UI_TRANSITION_TO_RUN_TIMEOUT);


		  if(j== OEMMPEG4_MAX_CB){
      		MSG_FATAL("Out of callback structures", 0, 0, 0);
      		return -1;
   		  }
	  }
   }

   pcb->cmdNotify.nStatus = nStatus;
   pcb->cmdNotify.nCmd = nCmd;
   pcb->cmdNotify.nSubCmd = nSubCmd;
   pcb->cmdNotify.pCmdData = pData;
   pcb->cmdNotify.dwSize = dwSize;
#if MIN_BREW_VERSION(3,0)
   AEE_ResumeCallback(&pcb->cb, pme->m_pAppContext);
#else
   AEE_ResumeEx(&pcb->cb, 0, pme->m_pAppContext);
#endif /* MIN_BREW_VERSION(3,0) */
   return 0;
}

/*==================================================================
  This function extracts audio/video file names from extended media
  data structure and then calls QTV openURN function.
==================================================================*/
int OEMMediaMPEG4_OpenURN(AEEMediaDataEx *pmdExList, int nCount)
{
  AEECLSID  clsData;
  int       i;

  if( !pmdExList || !nCount )
    return EBADPARM;

  /* currently QTV only support one type of input (file(s) or buffer(s)) */
  clsData = pmdExList[0].clsData;
  for(i=1; i<nCount; i++)
  {
    if(clsData != pmdExList[i].clsData)
      return EUNSUPPORTED;
  }

  if(clsData ==  MMD_FILE_NAME)   /* all members are file name */
  {
    char     *pVideoFile=NULL;
    char     *pAudioFile=NULL;

    /* this will accept first audio or video file in the list */
    for(i=0; i<nCount; i++)
    {
      if( !pVideoFile && (pmdExList[i].dwCaps & MM_CAPS_VIDEO) )
      {
        pVideoFile = (char *)pmdExList[i].pData;
      }
      if( !pAudioFile && (pmdExList[i].dwCaps & MM_CAPS_AUDIO) )
      {
        pAudioFile = (char *)pmdExList[i].pData;
      }
    }
    if(pVideoFile || pAudioFile)
    {
      return OEMMediaMPEG42PV_OpenURN(pVideoFile, pAudioFile);
    }
  }
  else if(clsData ==  MMD_BUFFER) /* all members are buffer pointers */
  {
    unsigned char *pVideoBuf=NULL;
    unsigned char *pAudioBuf=NULL;
    uint32         nVideoBufSize=0;
    uint32         nAudioBufSize=0;

    /* this will accept first audio or video file in the list */
    for(i=0; i<nCount; i++)
    {
      if( !nVideoBufSize && (pmdExList[i].dwCaps & MM_CAPS_VIDEO) )
      {
        pVideoBuf = (unsigned char *)pmdExList[i].pData;
        nVideoBufSize = pmdExList[i].dwSize;
      }
      if( !nAudioBufSize && (pmdExList[i].dwCaps & MM_CAPS_AUDIO) )
      {
        pAudioBuf = (unsigned char *)pmdExList[i].pData;
        nAudioBufSize = pmdExList[i].dwSize;
      }
    }
    if(nVideoBufSize || nAudioBufSize)
    {
      return OEMMediaMPEG42PV_OpenBufferURN(pVideoBuf, nVideoBufSize, pAudioBuf, nAudioBufSize);
    }
  }
  return EBADPARM;
}

/*=========================================================================
Function: CREATEMEDIAEX

Description:
   Given AEEMediaData, this function analyzes media data and creates
   IMedia-based object (IMedia object).

Prototype:

   int AEEMediaUtil_CreateMediaEx( IShell * ps,
                                   AEEMediaData * pmdList,
                                   int * pnCapsList,
                                   int nListCount,
                                   IMedia ** ppm );

Parameters:
   ps  [in]:        Shell pointer
   pmdList [in]:    Media data info list
   pnCapsList [in]: Media data capability list
   nListCount [in]: number of media data elements in the list
   ppm [out]:       IMedia object pointer returned to caller

Return Value:
   SUCCESS:          IMedia object successfully created
   EBADPARM:         Input parameter(s) is wrong
   EUNSUPPORTED:     Class not found
   MM_EBADMEDIADATA: Media data is not correct
   MM_ENOMEDIAMEMORY:No memory to create IMedia object
   ENEEDMORE:        Need more data to create IMedia object.
                     For MMD_ISOURCE, call IPEEK_Peekable(). (See Comments).
   MM_EPEEK:         Error while peeking for the data. (See Comments)
   MM_ENOTENOUGHDATA Aborted due to insufficient data
   EFAILED:          General failure

Comments:
  repeate following tests for all elements in the list
    If pmdList[i]->clsData is MMD_FILE_NAME, then
    (1) file extension is checked to see if any IMedia class is registered
        for the extension.
    (2) If not, the file is opened and file contents are read. Using
        ISHELL_DetectType(), the MIME of the media data is determined and IMedia class
        is found from Shell registry.

    If pmd->clsData is MMD_BUFFER, then
    (1) The buffer contents are analyzed using ISHELL_DetectType() to determine the MIME
        of the media data is determined and IMedia class is found from Shell registry.

    If pmd->clsData is MMD_ISOURCE, then
    (1) The caller needs to set pmd->pData to IPeek *.
    (2) This function peeks for the data to see if enough data is available. The buffer
        contents are analyzed using ISHELL_DetectType(), the MIME of the media data is determined
        and IMedia class is found from Shell registry.
    (3) If enough data is not there, then it calls IPEEK_Peek() requesting required
        number of bytes. If IPEEK_Peek() returns IPEEK_WAIT, then it returns
        ENEEDMORE. In response to this, caller needs to call IPEEK_Peekable().
        After the data is becomes available, caller should call this function again to
        create IMedia object.

   If IMedia class is found and all the elements point to same class, then IMedia object is
   created and media data is set. This puts IMedia object in Ready state.

Side Effects:
   None.

See Also:
   ISHELL_DetectType()

========================================================================= */
int AEEMediaUtil_CreateMediaEx( IShell * ps,
                                AEEMediaDataEx * pmdList,
                                int nListCount,
                                IMedia ** ppm )
{
  AEECLSID        Cls;
  AEECLSID        tempCls;
  IMedia *        pMedia;
  AEEMediaData    mediaData;
  int             nRet;
  int             i;

  if (!ps || !pmdList || !nListCount || !ppm)
    return EBADPARM;

  for(i=0; i<nListCount; i++)
  {
    if( !(pmdList[i].pData) )
      return EBADPARM;
  }

  *ppm = NULL;

  // Find the class ID. This function uses ISHELL_DetectType() API to get the IMedia
  // MIME and subsequently class ID. All the media data elements should point to the
  // same class ID.
  // currently find class function only works for AEEMediaData structure
  mediaData.clsData = pmdList[0].clsData;
  mediaData.pData   = pmdList[0].pData;
  mediaData.dwSize  = pmdList[0].dwSize;
  nRet = AEEMediaUtil_FindClass(ps, &mediaData, &Cls);
  if (nRet != SUCCESS)
    return nRet;

  for(i=1; i<nListCount; i++)
  {
    mediaData.clsData = pmdList[i].clsData;
    mediaData.pData   = pmdList[i].pData;
    mediaData.dwSize  = pmdList[i].dwSize;
    nRet = AEEMediaUtil_FindClass(ps, &mediaData, &tempCls);

    if (nRet != SUCCESS)
      return nRet;

    if(Cls != tempCls)
      return MM_EBADMEDIADATA;
  }

  // Create IMedia-based object
  if (ISHELL_CreateInstance(ps, Cls, (void **)&pMedia) || !pMedia)
    return MM_ENOMEDIAMEMORY;

#if MIN_BREW_VERSION(3,0)
   /* this will resolve the relative file path to full file path */
  if(OEMMediaMpeg4_FilePathEx(pmdList, nListCount) != SUCCESS)
    return MM_EBADMEDIADATA;
#endif

  // Set the media data and put IMedia in Ready state.
  nRet = IMEDIA_SetMediaDataEx(pMedia, pmdList, nListCount);
  if (nRet != SUCCESS)
  {
    IMEDIA_Release(pMedia);
    return nRet;
  }

  *ppm = pMedia;

  return SUCCESS;
}

#if MIN_BREW_VERSION(3,0)
/*==================================================================
This function resolves the file path for AEEMediaDataEx, which is
not done by BREW.
==================================================================*/
static int OEMMediaMpeg4_FilePathEx(AEEMediaDataEx * pmdList, int nListCount)
{
  int i, nLen;
  char *pszFS = NULL;
  int nRet = EFAILED;

  //EFS and MMC start tags to check if the path specified 
  //in .mfpt file is a full path or relative path.
  
  char* EfsFullPathStartTag = "brew/";
  char* MmcFullPathStartTag = "mmc1/";

  if (!pmdList || !nListCount) 
    goto Done;

  for(i=0, nLen=0; i<nListCount; i++)
  {

   //If the path starts with "brew/" or "mmc1/" then its a full path and 
   //AEE_ResolvePath should be skipped. For playing from EFS, relative or 
   //full path will work, however, when playing from memocr card,
   //user needs to specify full path as 
   //AEE_ResolvePath doesn't work properly for memory card.

    if(!strncmp(EfsFullPathStartTag,(char *)(pmdList[i].pData),strlen(EfsFullPathStartTag)) ||
       !strncmp(MmcFullPathStartTag,(char *)(pmdList[i].pData),strlen(MmcFullPathStartTag)))
    {
      continue;
    }


   // Get resolved fs:/ path length
    if (SUCCESS != AEE_ResolvePath((char *)(pmdList[i].pData), NULL, &nLen))
      goto Done;

    pszFS = MALLOC(nLen);
    if (!pszFS)
      goto Done;

    // Resolve path to fs:/
    if (SUCCESS != AEE_ResolvePath((char *)(pmdList[i].pData), pszFS, &nLen))
      goto Done;

    // Check for access
    if (SUCCESS != AEE_CheckPathAccess((char *)(pmdList[i].pData), AEEFP_READ,0)) 
      goto Done;

    /* measure native path */
    if (SUCCESS != OEMFS_GetNativePath(pszFS,0, &nLen))
      goto Done;

    FREEIF((char *)(pmdList[i].pData));
    pmdList[i].pData = MALLOC(nLen);
    if (!pmdList[i].pData) 
      goto Done;

    pmdList[i].dwSize = nLen;

    /* get native path */   
    if (SUCCESS != OEMFS_GetNativePath(pszFS,pmdList[i].pData,&nLen))
      goto Done;
  }

  nRet = SUCCESS;

Done:
  FREEIF(pszFS);
  return nRet;
}
#endif  // MIN_BREW_VERSION(3,0)


#endif  // FEATURE_APP_MPEG4
