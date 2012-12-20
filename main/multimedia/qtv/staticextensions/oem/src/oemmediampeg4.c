/*===========================================================================*/
/*!
  @file   OEMMediaMPEG4.c
  @author qtv.oemlayer.team

  @brief  IMediaMPEG4 implementation

  @detail This file implements MPEG4 functionality that can be used by IMedia-based
  classes.

  @defgroup IMediaAPI External IMedia API

*/
/*=========================================================================
        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*=========================================================================
                             Edit History
                             
$Header: //source/qcom/qct/multimedia/qtv/staticextensions/oem/main/latest/src/oemmediampeg4.c#57 $
$DateTime: 2010/12/15 02:39:32 $
$Change: 1554380 $

=========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "OEMMediaMPEG4.h"
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef FEATURE_CMI_MM
#error code not present
#endif
#include "msg.h"
#include "AEEMimeTypes.h"
#include "AEEFile.h"
#include "BREWVersion.h"
#include "OEMDisp.h"    /* For IBitmapCtl */
#include "OEMFeatures.h"
#include "OEMOS.h"
#include "qtv_perf.h"
#ifdef FEATURE_BMP
#include "AEEBase.h"
#include "AEEISettings.h"
#endif
#include "OEMMedia.h"
extern int AEEMedia_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo);
#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
#include "mddihost.h"
#endif
#if MIN_BREW_VERSION(4,0)
# include "OEMefs.h"
/* Brew 4.0 doesn't declare these functions anymore. */
extern AEE_EXPORTS int AEE_CheckPathAccess(const char *cpszFile, 
                                           uint32 dwDesiredRights,
                                           uint32 *pdwActualRights);
extern AEE_EXPORTS int AEE_ResolvePath(const char *cpszIn, char *pszOut, int *pnOutLen);
#else
# include "OEMFS.h"
#endif /* MIN_BREW_VERSION(4,0) */
#if MIN_BREW_VERSION(3,0)
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* MIN_BREW_VERSION(3,0) */

#if defined (FEATURE_REAL_PLAYER_USE_QTV_API) && defined (FEATURE_REAL_NV_CONFIG)
#include "OEMMediaRealTranslation.h"
#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
#include "AEESource.h"
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_APP_MEDIA_MULTIPLE_FILE
#include "OEMMediaMultipleFile.h"
#endif

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#endif // FEATURE_ACM

#if defined(FEATURE_QTV_PSEUDO_STREAM) || \
    defined (FEATURE_QTV_QOS_SELECTION)|| \
    defined (FEATURE_QTV_PDP_SELECTION)|| \
    defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

   #include "AEEMediaUtil.h"

#endif

#ifdef   FEATURE_APP_MPEG4

// Define pass thru routine as AEE_SetAppContext is defined only in BREW 2.x
static OEMCriticalSection gcmCriticalSection;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&gcmCriticalSection)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&gcmCriticalSection)
# define AEE_SetAppContext AEE_EnterAppContext

#include "BREWVersion.h"
#if MIN_BREW_VERSION(4,0)
#include "AEEIBitmapCtl.h"
#define AEEIID_BITMAPCTL AEEIID_IBitmapCtl
#endif

int nMdpFramePendingCounter = 0;

boolean b_is_GetFrame = FALSE;// Gemsea Add

#ifndef T_QSC1110 // Gemsea Add
extern void disp_lock_screen(word start_row,word num_row,word start_column,word num_column);
extern void disp_update_yuv420(void * buf_ptr, int16 num_of_rows,int16 num_of_columns);
#endif
/*==============================================================================

                               MACRO DEFINITIONS

==============================================================================*/
/* How long to poll the UI thread to release the callbacks */
#define UI_TRANSITION_TO_RUN_TIMEOUT        500 /* in ms */

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#define MDP_IS_ROTATED_SIDEWAYS(a) \
   (((a) == OEM_90_CW_ROTATION) || \
    ((a) == OEM_90_CCW_ROTATION))
#define MDPOP_ROT_MASK   0x00000007
#define MDPOP_SCALE_MASK 0x00000078
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

OEMCriticalSection TimedText3gppCS;

/* Macros for changing app context.  The underlying function changes
 * among BREW versions, so this macro abstracts the BREW version.
 */
#if MIN_BREW_VERSION(3, 0)
#define ENTER_APP_CONTEXT AEE_EnterAppContext
#define LEAVE_APP_CONTEXT AEE_LeaveAppContext
#else
#define ENTER_APP_CONTEXT AEE_SetAppContext
#define LEAVE_APP_CONTEXT AEE_SetAppContext
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
#define DEFAULT_ISOURCE_STREAM_SIZE (2*1024*1024)
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifndef PLATFORM_QTV_DLL
/* For non-QtvDLL builds, use the standard _Notify callback.  For QtvDLL builds,
 * we need to make sure that the _Notify callback is called from a RexNT task
 * and not a plain Windows thread.  Therefore we need to leave this macro
 * undefined so a wrapper can be placed around the _Notify callback.  This macro
 * is defined in BrewQTV.cpp in the QtvDLL build environment.
 */
#define OEMMEDIAMPEG4_NOTIFY_CB OEMMediaMPEG4_Notify
#endif /* !PLATFORM_QTV_DLL */

/*==============================================================================

                             EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
/*! @dontinclude
 */
OBJECT(OEMMediaMPEG4)
{
   INHERIT_AEEMedia(IMediaMPEG4);
   boolean  m_bRelMark;
   #ifdef FEATURE_BMP
   struct QTV_ISeetings {
     AEEBASE_INHERIT(ISettings, OEMMediaMPEG4);
   }m_qtvISettings;   
   #endif
   OEMMediaMPEG4Layer *m_pMPEG4;
   int                 m_nPlayType;
};

/*==============================================================================

                      DATA DECLARATIONS

==============================================================================*/
uint32                    g_uMPEG4RefCnt = 0;
//Add one more NULL if there is support for four instances
OEMMediaMPEG4Layer        *g_pOEMLayer[MAX_NUM_OEM_PLAYBACK_INST] = { NULL, NULL, NULL }; 

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
static uint8 bPseudoStream = 0;                    
static uint32 gHttpInterfaceId = MMD_HTTP_PUSH_BUFFER_PLAYBACK;
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE
extern boolean qtv_cfg_enable_dsp_release;

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /*  FEATURE_QTV_QOS_SELECTION */
#endif /*  FEATURE_QTV_QDSP_RELEASE_RESTORE */

#ifdef QTV_ARMREGISTER_TIMING
#include "qw.h"
/* Array size maintained as 10. 4 is the max */
qword         start_bitblt_time_ms[10];
qword         end_bitblt_time_ms[10];
uint32 		  bitblt_time_ms, bitblt_start_index, bitblt_end_index, total_frames_displayed, maxdisplay=0;
#endif
uint32        total_frames_sent_to_display=0;
uint32        no_of_getframes_called = 0;
/*! Sound function pointer type so that a function pointer array can be defined as follows, 
    with each pointer points to a Sound function */
typedef int (*OEM_SoundFunction)(OEMHandle pOEM, void * param);

/*! This function pointer array is used for pending sound command when clip 
    info is not available.  Modifed the protocol of the following function 
    so that they all have the same inputs and output.
*/
static OEM_SoundFunction OEMMediaMPEG4Qtv_SoundFunction[SOUNDCMD_MAX] = 
{
  OEMMediaMPEG42PV_SetVolume,   /*!<  set volume */
  OEMMediaMPEG42PV_GetVolume,   /*!<  get volume */
  OEMMediaMPEG42PV_SetMuteCtl,  /*!<  set mute */
  NULL,                         /*!<  get mute, always return current mute without calling ISOUND, so not necessary to have 
                                      this request pending. */
  OEMMediaMPEG42PV_SetDevice,   /*!<  set device */
  NULL                          /*!<  get device, not supported */
};

/*===============================================================================

      COMMON FUNCTION DECLARATIONS
      These functions are used by IMedia classes implemented in this file.

===============================================================================*/
static void   OEMMediaMPEG4_CalcClipping(OEMHandle pOEM, AEEBitmapInfo  bitmapInfo);
static int OEMMediaMPEG4_SetPlayType(OEMMediaMPEG4 * pme, int nPlayType);

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
static int  OEMMediaMPEG4_CopyMediaDataEx(OEMMediaMPEG4 * pme, AEEMediaDataEx *pmdExList, int nCount);
static void OEMMediaMPEG4_FreeMediaDataEx(OEMMediaMPEG4 * pme);
extern void AEEMedia_CleanUpResource(AEEMedia * po);
extern  int AEEMedia_RequestResource(AEEMedia * pme, flg isPlay);
#endif // FEATURE_ACM || FEATURE_BMP_ACM
#if defined (FEATURE_ACM) 
void OEMMediaMPEG4_GetResourceCntlData(AEEMedia * po, int * nPlayType, uint32 *acmCB);
int OEMMediaMPEG4_AcmSetMediaDataCB(ACM_Callback_data_type * po);
int OEMMediaMPEG4_AcmPlayCB(ACM_Callback_data_type * po);
#endif // FEATURE_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
static void   OEMMediaMPEG4_MdpCb(void *,MDP_STATUS);
static void   OEMMediaMPEG4_MDPDeregister(OEMHandle pOEM);
static uint32 OEMMediaMPEG4_MDPCalcTransforms(OEMHandle pOEM);
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
// this value needs to be the same as defined in qtvplayer.h
#define MAXIMUM_MIME_BUFFER_LENGTH        16
static int OEMMediaMPEG4Qtv_PlayISOURCE(OEMMediaMPEG4* pme);
static void OEMMediaMPEG4_ReadSource(OEMMediaMPEG4* po);
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
static OEMMediaMPEG4Layer * OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn);
static void  OEMMediaMPEG4Layer_Delete(OEMMediaMPEG4Layer * pOEM);
static int   OEMMediaMPEG4_OpenURN(AEEMediaDataEx *pmdExList, int nCount, OEMHandle pOEM);
static void OEMMediaMPE4Qtv_handleSoundPending(OEMHandle pOEM);
/*===========================================================================

                      IMediaMPEG4 DECLARATIONS

===========================================================================*/
static uint32 OEMMediaMPEG4_Release(IMedia * po);
static int    OEMMediaMPEG4_QueryInterface(IMedia * po, AEECLSID clsid, void **ppo);
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

/*! @ingroup IMediaAPI
 */
static const VTBL(IMediaMPEG4) gMediaMPEG4Funcs =
{
   AEEMedia_AddRef,
   OEMMediaMPEG4_Release,
   OEMMediaMPEG4_QueryInterface,
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
// QueryInterface
static int OEMMediaMPEG4_QueryInterface(IMedia * po, AEECLSID clsid, void **ppo)
{
   int nResult = ECLASSNOTSUPPORT;
   #ifdef FEATURE_BMP
   OEMMediaMPEG4 *pMe = (OEMMediaMPEG4 *)po;

   *ppo = NULL;

   if(AEEIID_ISettings == clsid){
      *ppo = (void *)&pMe->m_qtvISettings;
      nResult = AEE_SUCCESS;
   }

   if(nResult == AEE_SUCCESS){
      (void)AEEMedia_AddRef(po);
   }
   else
   {   

     nResult = AEEMedia_QueryInterface(po, clsid, ppo);   
   }
   #endif
   return nResult;
}

// Get the value of the setting using its URI
#ifdef FEATURE_BMP
int OEMMediaMPEG4Settings_Get(ISettings *po, const char *cpszKey, char *pszValue, int nValueLen, int *pnValueLenReq)
{
  return AEE_EUNSUPPORTED;
}

// Set the value of a setting using its URI
int OEMMediaMPEG4Settings_Set(ISettings *po, const char *cpszKey, const char *cpszValue)
{
  if (!po)
  {
     MSG_ERROR("OEMMediaMPEG4Settings_Set, po is NULL!", 0, 0, 0);
     return EFAILED;
  }
  else
  {
    struct QTV_ISeetings * pISetting = (struct QTV_ISeetings *)po;
    OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)pISetting->pMe;
    if (NULL != pme)
    {
        OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);
        return (OEMMediaMPEG42Qtv_ISettings_Set((char *)cpszKey,(char *)cpszValue,pOEM));
    }
    else
    {
        MSG_ERROR("OEMMediaMPEG4Settings_Set, pme is NULL!", 0, 0, 0);
        return EFAILED;
    }
  }
}

// Delete a setting from the settings store
int OEMMediaMPEG4Settings_Delete(ISettings *po, const char *cpszKey)
{
  return AEE_EUNSUPPORTED;
}

// Reset settings to their default values
int OEMMediaMPEG4Settings_Reset(ISettings *po, const char *cpszKey)
{
  return AEE_EUNSUPPORTED;
}

// Register a signal to fire when a particular setting (or settings subtree) changes
int OEMMediaMPEG4Settings_OnChange(ISettings *po, const char *cpszKey, ISignal *piSignal, IQueryInterface **ppiReg)
{
  return AEE_EUNSUPPORTED;
}

// Return a count of the children under the specified parent settings node
int OEMMediaMPEG4Settings_GetNumChildren(ISettings *po, const char *cpszKey, int *pnChildren)
{
  return AEE_EUNSUPPORTED;
}

// Return the Nth child under the specified parent settings node
int OEMMediaMPEG4Settings_GetChildName(ISettings *po, const char *cpszKey, int nChild, char *pszResults, int nResultSize, int *pnResultLen)
{
  return AEE_EUNSUPPORTED;
}

// Get an ISettings object rooted at the specified key
int OEMMediaMPEG4Settings_GetSubTree(ISettings *po, const char *cpszKey, ISettings **ppo)
{
  return AEE_EUNSUPPORTED;
}
#endif
/*==================================================================
   Destructor
==================================================================*/
void OEMMediaMPEG4_Delete(IMedia * po, boolean bFree)
{
   OEMMediaMPEG4      * pme = (OEMMediaMPEG4 *)po;	
   MSG_HIGH("OEMMediaMPEG4Layer_Delete Entry", 0, 0, 0);
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
#ifndef T_QSC1110 // Gemsea Add
   disp_lock_screen(0,0,0,0);
#endif
   MSG_HIGH("OEMMediaMPEG4Layer_Delete Exit", 0, 0, 0);
}

/*==================================================================
   Destructor
==================================================================*/
void OEMMediaMPEG4Layer_Delete(OEMMediaMPEG4Layer * pOEM)
{
  int i;

  // Check to see if this media has already been deleted.
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  if (pOEM == NULL)
  {
    MSG_ERROR("OEMMediaMPEG4Layer_Delete pOEM has already been deleted!", 0, 0, 0);
    return;
  }

  pOEM->m_bTerminating = TRUE;
  OEMMediaMPEG42PV_Terminate(pOEM);
  pOEM->m_bTerminating = FALSE;

  for ( i=0; i<MAX_NUM_OEM_PLAYBACK_INST; i++ ) {
    if ( g_pOEMLayer[i] == pOEM ) {
       g_pOEMLayer[i] = NULL;
	--g_uMPEG4RefCnt;
    }
  }

  AEEObjectMgr_Unregister(pOEM->m_hObject);

  /* Cancel any pending callbacks */
  for (i = 0; i < OEMMPEG4_MAX_CB; i++)
  {
    pOEM->m_cb[i].bInUse = FALSE;
    CALLBACK_Cancel(&pOEM->m_cb[i].cb);
  }

  MM_RELEASEIF(pOEM->m_pISound);
  MM_RELEASEIF(pOEM->m_pIDisplay);
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
  if( pOEM->m_Video.id != 0 )
  {
    OEMMediaMPEG4_MDPDeregister(pOEM);
  }
#endif /* #ifdef FEATURE_QTV_MDP_TRANSFORMATIONS */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
  if(pOEM->m_gpISourceStream)
  {
    FREE(pOEM->m_gpISourceStream);
    pOEM->m_gpISourceStream = NULL;
    if(pOEM->m_gpcbISourceMediaData)
    {
      // Cancel pending callbacks..
      CALLBACK_Cancel(pOEM->m_gpcbISourceMediaData);
      FREE(pOEM->m_gpcbISourceMediaData);
      pOEM->m_gpcbISourceMediaData = NULL;
    }
  }
#endif //FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  while (q_cnt(&(pOEM->m_qOutputBuffer)) > 0)
  {
    OEMOutputBufferType* pBufType = (OEMOutputBufferType*)q_get(&(pOEM->m_qOutputBuffer));
    if ( pBufType ) 
    {
      FreeOutputBuffer(NULL, NULL, pBufType->pOutputBuffer);
      FREE(pBufType);    
    }
  }
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

#ifdef QTV_ARMREGISTER_TIMING
  if(total_frames_displayed != 0)
  {
    MSG_MED( "avg_display_time %d", bitblt_time_ms / total_frames_displayed, 0, 0);
    MSG_MED("max display time %d",maxdisplay,0,0);
    MSG_MED( "total_frames_displayed %d", total_frames_displayed, 0, 0);
  }
#endif
  FREE(pOEM);
  MSG_HIGH("OEMMediaMPEG4Layer_Delete...return",0,0,0);
}

/*==============================================================================

==============================================================================*/
/*!
 * @ingroup IMediaAPI
 */
static uint32 OEMMediaMPEG4_Release(IMedia * po)
{
   int32 nRef = AEEMedia_Release(po);
   MSG_HIGH("OEMMediaMPEG4_Release Entry",0,0,0);

   if (!nRef)
   {
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
      OEMMediaMPEG4_FreeMediaDataEx((OEMMediaMPEG4 *) po);
#endif
      OEMMediaMPEG4_Delete(po, TRUE);
   }
   MSG_HIGH("OEMMediaMPEG4_Release Exit nRef = %d",nRef,0,0);
   return nRef;
}

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
int AEEMediaUtil_CreatePseudoMediaEx(IShell * ps, AEEMediaDataEx * pmd, IMedia ** ppm)
{
   int         nRet;

   if (!ps || !pmd || !pmd->pData || !ppm)
      return EBADPARM;

   if ((pmd->clsData == MMD_BUFFER_FRAGMENT) ||
       (pmd->clsData == MMD_HTTP_PUSH_BUFFER_PLAYBACK) ||
       (pmd->clsData == MMD_HTTP_PULL_BUFFER_PLAYBACK))
   {
      gHttpInterfaceId = pmd->clsData;
      pmd->clsData = MMD_BUFFER;
      bPseudoStream = 1;
      nRet = AEEMediaUtil_CreateMedia(ps, (AEEMediaData *)pmd, ppm);
      pmd->clsData = gHttpInterfaceId;
      bPseudoStream = 0;
   }
   else
   {
     nRet = AEEMediaUtil_CreateMedia(ps, (AEEMediaData *)pmd, ppm);
   }
   return nRet;
}
#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
static int OEMMediaMPEG4Qtv_PlayISOURCE( OEMMediaMPEG4 * pme)
{
  AEEMedia * pMedia = pme->m_pMPEG4->m_pMedia;
  OEMMediaMPEG4Layer * pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);

  if (pOEM == NULL) 
  {
    MSG_ERROR("OEMMediaMPEG4Qtv_PlayISOURCE, pOEM is NULL", 0, 0, 0);
    return EBADPARM;
  }

  if(pMedia->m_md.dwSize == 0xFFFFFFFF || pMedia->m_md.dwSize == 0)
  {
    //Set to default.
    pMedia->m_md.dwSize = DEFAULT_ISOURCE_STREAM_SIZE;
  }

  if(pOEM->m_gpISourceStream)
    FREE(pOEM->m_gpISourceStream);

  pOEM->m_gpISourceStream = (uint8 *)MALLOC(pMedia->m_md.dwSize);
  if(!pOEM->m_gpISourceStream)
  {
    return ENOMEMORY;
  }

  pOEM->m_gpcbISourceMediaData = (AEECallback *)MALLOC(sizeof(AEECallback));
  if (!pOEM->m_gpcbISourceMediaData)
  {
    FREE(pOEM->m_gpISourceStream);
    return ENOMEMORY;
  }
  CALLBACK_Init(pOEM->m_gpcbISourceMediaData, (PFNNOTIFY)OEMMediaMPEG4_ReadSource, (void *)pme);
  
  pOEM->m_sourceBufferWritePtrOffset = 0;
  pOEM->m_bStreamOpened = FALSE;
  pOEM->m_nStreamOpenReturn = SUCCESS;
  OEMMediaMPEG4_ReadSource(pme);

  return pOEM->m_nStreamOpenReturn;
}
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

/*!
  @brief
  Execute pending sound request
  
  @detail
  When clip info is available, set sound method based on audio codec type.  And then
  execute pending sound command.  If the pending sound command is not successful, queue 
  a callback to indicate the status.
*/
static void OEMMediaMPE4Qtv_handleSoundPending(OEMHandle pOEM)
{
  int retValue = 0;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);
  // Need to provide valid OEMhandle, return if pOEM is NULL
  if(!pOEM)
  {
    MSG_ERROR("OEMMediaMPEG42PV_SetSoundMethod, pOEM passed in is NULL.", 0, 0, 0);
    return;
  }

  if (pOEM->m_bClipInfoAvail) 
  {
    OEMMediaMPEG42PV_SetSoundMethod(pOEM);
  }

  if(pOEM->m_Sound.cmd_pending)
  {
    pOEM->m_Sound.cmd_pending = FALSE;
    if( !pOEM->m_bClipInfoAvail )
    {
      OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                  MM_STATUS_ABORT,
                                  pOEM->m_Sound.sound_cmd.aee_cmd,
                                  pOEM->m_Sound.sound_cmd.aee_param,
                                  NULL,
                                  0);
    }
    else
    // Based on pending sound cmd, call the corresponding sound function
    // if result is not pending, send MM_STATUS_ABORT callback
    if(((pOEM->m_Sound.sound_cmd.cmd) < SOUNDCMD_MAX) && (OEMMediaMPEG4Qtv_SoundFunction[pOEM->m_Sound.sound_cmd.cmd]))
    {
      retValue = OEMMediaMPEG4Qtv_SoundFunction[pOEM->m_Sound.sound_cmd.cmd](pOEM, 
                                                                     &(pOEM->m_Sound.sound_cmd.param));
      if(retValue != MM_PENDING && retValue != EALREADY)
      {
        OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                    MM_STATUS_ABORT,
                                    pOEM->m_Sound.sound_cmd.aee_cmd,
                                    pOEM->m_Sound.sound_cmd.aee_param,
                                    NULL,
                                    0);
      }
    }
  }
}

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))

/*!
 * @brief
 * Reference function for implementing QoS media on UMTS/WCDMA platform
 * This function is not part of BREW and is strictly for reference only
 */
int AEEMediaUtil_CreateQoSMedia(IShell * ps, AEEMediaDataWithQoS * pmd,
                                IMedia ** ppm)
{
  AEECLSID        Cls;
  int             nRet;
  IMedia *        pMedia;
  AEEMediaData    mediaData;

  *ppm = NULL;
  if (!ps || !pmd || !pmd->pData || !ppm)
      return EBADPARM;
  mediaData.clsData = pmd->clsData;
  mediaData.pData   = pmd->pData;
  mediaData.dwSize  = pmd->dwSize;
  nRet = AEEMediaUtil_FindClass(ps, &mediaData, &Cls);
  if (nRet != SUCCESS)
    return nRet;

  // Create IMedia-based object
  if (ISHELL_CreateInstance(ps,Cls, (void **)&pMedia) || !pMedia)
      return MM_ENOMEDIAMEMORY;

  /* Set the QoS Netpolicy before attempting to move IMedia to ready state */
  IMEDIA_SetMediaParm(pMedia,
                      MM_MP4_PARM_NET_QOS_POLICY,
                      (int32)(pmd->qosInfo),
                      0);

  // Set the media data and put IMedia in Ready state.
  // It results in a call to OpenURN...() at OEM layer
  nRet = IMEDIA_SetQoSMediaData(pMedia,pmd);
  if (nRet != SUCCESS)
  {
     IMEDIA_Release(pMedia);
     return nRet;
  }

  *ppm = pMedia;

  return SUCCESS;
}

#endif

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
static void OEMMediaMPEG4_ReadSource(OEMMediaMPEG4* po)
{
    AEEMedia *  pMedia = po->m_pMPEG4->m_pMedia;
    OEMMediaMPEG4Layer * pOEM = OEMMediaMPEG42PV_GetOEMLayer(po->m_pMPEG4);
    ISource *   pSource = (ISource *)pMedia->m_md.pData;
    int32       nRead;    

    if (pOEM == NULL) 
    {
      return;
    }

    while(1)
    {
        nRead = ISOURCE_Read(pSource, 
            (char *)pOEM->m_gpISourceStream + pOEM->m_sourceBufferWritePtrOffset, 
            (int32)pMedia->m_md.dwSize - pOEM->m_sourceBufferWritePtrOffset);

        if(nRead > 0)
        {
            pOEM->m_sourceBufferWritePtrOffset += nRead;            
            
            // read at least MAXIMUM_MIME_BUFFER_LENGTH bytes before calling OEMMediaMPEG4Qtv_OpenPseudoStream().
            if((pOEM->m_sourceBufferWritePtrOffset >= MAXIMUM_MIME_BUFFER_LENGTH) && 
              (!pOEM->m_bStreamOpened ))
              {
                pOEM->m_nStreamOpenReturn = OEMMediaMPEG4Qtv_OpenPseudoStream(
                                                        (unsigned char *)pOEM->m_gpISourceStream,
                                                         pMedia->m_md.dwSize,
                                                         pOEM->m_sourceBufferWritePtrOffset,
                                                         MMD_HTTP_PUSH_BUFFER_PLAYBACK,
                                                         pOEM);

                pOEM->m_bStreamOpened = TRUE;
              }

            OEMMediaMPEG4Qtv_UpdatePSBufferOffset(pOEM->m_sourceBufferWritePtrOffset);
        }
        else if (ISOURCE_WAIT == nRead)
        {
            ISOURCE_Readable(pSource, pOEM->m_gpcbISourceMediaData);
            break;
        }
        else if (ISOURCE_END == nRead || ISOURCE_Exhausted(nRead))
        {
            // Inform the status that stream has never been opened
            if (!pOEM->m_bStreamOpened) 
            {
              OEMMediaMPEG4_QueueCallback(pOEM->m_pCallbackData,
                                    MM_STATUS_ABORT,
                                    MM_CMD_PLAY,
                                    0,
                                    (void *)NULL,
                                    0);
            }

            // Cancel pending callbacks..
            CALLBACK_Cancel(pOEM->m_gpcbISourceMediaData);
            FREE(pOEM->m_gpcbISourceMediaData);
            pOEM->m_gpcbISourceMediaData = NULL;
            break;
        }
    }
}
#endif // FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

/*==============================================================================
FUNCTION:   OEMMediaMPEG4_SetMediaData(AEEMedia * pMedia, int32 p1)
DESCRIPTION:
   Function to set the media data for the real player.
==============================================================================*/
static int OEMMediaMPEG4_SetMediaData (AEEMedia * pMedia, int32 p1)
{
  int nRet = SUCCESS;
  OEMMediaMPEG4 *pme = (OEMMediaMPEG4*)pMedia;
  OEMMediaMPEG4Layer *pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
  AEEMediaDataEx *mediaDataEx = NULL;
#endif /* defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

  if (pOEM == NULL) 
  {
    MSG_ERROR("OEMMediaMPEG4_SetMediaData, pOEM is invalid!", 0, 0, 0);
    return EBADPARM;
  }

  if (pOEM->m_dwEnabledCaps != pOEM->m_dwCaps)
  {
    return OEMMediaMPEG4_OpenURN((AEEMediaDataEx *)p1, 1, pOEM);
  }

  switch( pMedia->m_md.clsData )
  {
    case MMD_BUFFER:  /* playback from buffer */
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      /* Option to use MMD_BUFFER for pseudo streaming */
      if (bPseudoStream)
      {
        if (p1 <= 0)
        {
           MSG_ERROR("OEMMediaMPEG4_SetMediaData, p1 is invalid!", 0, 0, 0);
           return EBADPARM;
        }
        mediaDataEx = (AEEMediaDataEx *)p1;
        pOEM->m_nPseudoBufferWritePtrOffset = mediaDataEx->dwBufferSize;

        nRet = OEMMediaMPEG4Qtv_OpenPseudoStream(
                                  (unsigned char *)pMedia->m_md.pData,
                                   pMedia->m_md.dwSize, 
                                   mediaDataEx->dwBufferSize,
                                   gHttpInterfaceId, 
                                   pOEM);
      }
      else
#endif /* FEATURE_QTV_PSEUDO_STREAM  || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
        /* here video and audio buffers are same */
      {
        nRet = OEMMediaMPEG42PV_OpenBufferURN(
                                  (unsigned char *)pMedia->m_md.pData,
                                  pMedia->m_md.dwSize,
                                  (unsigned char *)pMedia->m_md.pData,
                                  pMedia->m_md.dwSize, pOEM);
      }  
      break;

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      case MMD_HTTP_PUSH_BUFFER_PLAYBACK:
      case MMD_HTTP_PULL_BUFFER_PLAYBACK:
        if (p1 <= 0)
        {
           MSG_ERROR("OEMMediaMPEG4_SetMediaData, p1 is invalid!", 0, 0, 0);
           return EBADPARM;
        }
        mediaDataEx = (AEEMediaDataEx *)p1;
        pOEM->m_nPseudoBufferWritePtrOffset = mediaDataEx->dwBufferSize;

        nRet = OEMMediaMPEG4Qtv_OpenPseudoStream(
                                 (unsigned char *)pMedia->m_md.pData,
                                  pMedia->m_md.dwSize,
                                  mediaDataEx->dwBufferSize,
                                  pMedia->m_md.clsData, 
                                  pOEM);
    	  break;

#endif /* FEATURE_QTV_PSEUDO_STREAM || FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
      case MMD_FILE_NAME:
        nRet = OEMMediaMPEG42PV_OpenURN(pMedia->m_pszFile, pMedia->m_pszFile, pMedia->m_pszFile, pOEM);
        break;


#if(defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || defined FEATURE_QTV_DRM_DCF)
      case MMD_ISOURCE:
      {
        // ISource *   pSource = (ISource *)pMedia->m_md.pData;
        // int32       nRead;
#ifdef FEATURE_QTV_DRM_DCF
        ISource* pSource = (ISource*)pMedia->m_md.pData;
        nRet = OEMMediaMPEG42PV_OpenIxStreamFromISource(pSource, pOEM);

        if(( nRet == SUCCESS)||(nRet == EBADSTATE))
        {
          //These are only status codes returned from OEMMediaMPEG42PV_OpenIxStreamFromISource
          //once we extract valid IxStream from ISource.Just break here if we have valid IxStream
          //but some other error occured.
          break;
        }                
#endif//#ifdef FEATURE_QTV_DRM_DCF

#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
        nRet = OEMMediaMPEG4Qtv_PlayISOURCE(pme);
#else 
        nRet = EUNSUPPORTED;
#endif//#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
                
        break;
      }//case MMD_ISOURCE:

#endif//#if(defined FEATURE_QTV_3GPP_PROGRESSIVE_DNLD || defined FEATURE_QTV_DRM_DCF)

      default:
        nRet = EUNSUPPORTED;
  } /* switch( pMedia->m_md.clsData ) */

  return nRet;
}

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
/*==================================================================
   OEMMediaMPEG4_CheckRectChanged:
   compares the dimensions of two rectangles.
   returns TRUE if they are different, otherwise returns FALSE
==================================================================*/
static boolean OEMMediaMPEG4_CheckRectChanged(AEERect* rect1, AEERect* rect2)
{
  if ( (rect2->dx != rect1->dx) || (rect2->dy != rect1->dy) ||
       (rect2->x != rect1->x)   || (rect2->y != rect1->y) )
    return TRUE;

  return FALSE;
}
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

/*==============================================================================

==============================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)po;
   AEEMedia * pMedia = pme->m_pMPEG4->m_pMedia;
   int nRet = SUCCESS;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);
#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
   AEEMediaDataBufferChunk *m_md;
#endif /* FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_HTTP_PROGRESSIVE_DNLD */
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
   AEEMediaDataSetupTime *m_ds;
#endif

#if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))

   AEENetPolicyInfo *m_netPolicyInfo = NULL;
#endif

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
   int nACMStatus;
#endif
   MSG_HIGH("OEMMediaMPEG4_SetMediaParm Entry",0,0,0);
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif 

   MSG_HIGH("OEMMediaMPEG4_SetMediaParm... nParmID:%d; p1:%d; p2:%d", nParmID, p1, p2);

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_SetMediaParm: returning, pOEM is NULL", 0, 0, 0);
      return EBADPARM;
   }

   /* return EBADSTATE when the OEM is in suspended state except for the case 
     where an incoming call or SMS message is recieved and  requires QTV 
     to clear the screen of the previous frame.
   */
   if ((pOEM->m_bSuspend) && (nParmID != MM_PARM_RECT))
   {
      MSG_ERROR("OEMMediaMPEG4_SetMediaParm: returning, EBADSTATE", 0, 0, 0);   
      return EBADSTATE;
   }

   switch (nParmID)
   {
     #if (defined (FEATURE_QTV_QOS_SELECTION)|| defined (FEATURE_QTV_PDP_SELECTION))
      case MM_MP4_PARM_NET_QOS_POLICY:
         m_netPolicyInfo = (AEENetPolicyInfo*)p1;
         OEMMediaMPEG42Qtv_SetNetPolicyInfo(m_netPolicyInfo, pOEM);
      break;
     #endif

     #ifdef FEATURE_QTV_ENCRYPTED_STREAMS
      case MM_MP4_PARM_DESCRAMBLER_HANDLER:
         OEMMediaMPEG42Qtv_SetDescramblerHandler((void *)p1, pOEM);
      break;
     #endif

      case MM_MP4_PARM_USER_AGENT:
        OEMMediaMPEG42Qtv_SetUserAgent((char *)p1);
     break;

      case MM_PARM_AUDIO_DEVICE:
        nRet = OEMMediaMPEG42PV_SetDevice(pOEM, &p1);
        break;
      
      case MM_PARM_VOLUME:
      {
        if (!pOEM->m_pISound)
        {
          nRet = EUNSUPPORTED;
        }
        else if ((uint16)p1 > AEE_MAX_VOLUME)
        {
          nRet =  EBADPARM;
        }
        else
        {
          nRet = OEMMediaMPEG42PV_SetVolume(pOEM, &p1);
        }
        break;
      }
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD       
      case MM_MP4_PARM_HTTP_SETUP_TIME:
        m_ds = (AEEMediaDataSetupTime *)p1;
        OEMMediaMPEG4Qtv_SetStartAndBufferingTime(m_ds->dwStartupTime, m_ds->dwBufferingTime, pOEM);
        break;
#endif

#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
	  /* case MM_PARM_NEW_CHUNK: */
	  /* Turn on when BREW supports MM_PARM_NEW_CHUNK*/
      case MM_MP4_PARAM_PSBUF_WRITE_OFFSET:
        m_md = (AEEMediaDataBufferChunk *)p1;
        pOEM->m_nPseudoBufferWritePtrOffset += m_md->dwSize;
        OEMMediaMPEG4Qtv_UpdatePSBufferOffset(pOEM->m_nPseudoBufferWritePtrOffset);
        break;
#endif /* FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */

      case MM_PARM_MEDIA_DATA:
        if(p2 > 0)
        {
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
          // SetMediaDataEx can only be called in idle state
          if (!AEEMedia_IsIdleState(pMedia))
          {
            MSG_ERROR("OEMMediaMPEG4_SetMediaParm: returning, EBADSTATE", 0, 0, 0);
            return EBADSTATE;
          }
          
          // Need to call requestResource before setting the state to READY
          // since we are using the media state to choose the callback
          // function to be registered with ACM. The callback function is 
          // different for IMEDIA_SetMediaParm and IMEDIA_Play.
          nRet = AEEMedia_RequestResource((AEEMedia *)po, TRUE);

          // acquired
          if (nRet == SUCCESS)
          {
#endif /* FEATURE_ACM || FEATURE_BMP_ACM */
            
            if( ((AEEMediaDataEx *)p1)->clsData == MMD_ISOURCE)
            {
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
              if( p2 > 1)
              {
                // For MMD_ISOURCE, Qtv does not support media data count more than one
                nRet = EUNSUPPORTED;
              }
              else
              {
                nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
                if (nRet == SUCCESS)
                {
                  nRet = OEMMediaMPEG4Qtv_PlayISOURCE((OEMMediaMPEG4 *)po);
                }              
              }
#else
              nRet = EUNSUPPORTED;
#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */             
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 
              break;
            }


            nRet = OEMMediaMPEG4_OpenURN((AEEMediaDataEx *)p1, (int)p2, pOEM);
            if (nRet == SUCCESS)
            { 
              pme->m_nState = MM_STATE_READY;
            }
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
            else
            {
              AEEMedia_CleanUpResource((AEEMedia *)po);
            }
          }
          // delayed acquiring
          else if (nRet == MM_PENDING)
          {
            nRet = OEMMediaMPEG4_CopyMediaDataEx (pme, 
                                                  (AEEMediaDataEx *)p1, 
                                                  (int)p2);
            if (nRet == SUCCESS) 
            {
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif 
              pme->m_nState = MM_STATE_READY;
            }
            else
            {
              AEEMedia_CleanUpResource((AEEMedia *)po);
              nRet = EFAILED;
            }
            MSG_HIGH("OEMMediaMPEG4_SetMediaParm... return nRet:%d", nRet, 0, 0);
            return nRet;
          }
          // resource not granted
          else
          {
            MSG_ERROR("OEMMediaMPEG4_SetMediaParm: returning, EFAILED", 0, 0, 0);
            return EFAILED;
          }
#endif // FEATURE_ACM || FEATURE_BMP_ACM
        }
        else
        {
#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
          int replaymode = 0;
          if(p1 != NULL)
          {
            replaymode = ((AEEMediaData*)p1)->clsData;
          }
          // if this is PULL replay mode, just copy the data and set state to ready
          if(replaymode == MMD_HTTP_PULL_BUFFER_PLAYBACK)
          {
            // pull mode replay
            MEMCPY(&pMedia->m_md, (AEEMediaData*)p1, sizeof(AEEMediaData));
            pme->m_nState = MM_STATE_READY;
            nRet = SUCCESS;
          }
          else
          {
            nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
          }

          if (nRet == SUCCESS)
          {
            nRet = OEMMediaMPEG4_SetMediaData (pMedia, p1) ;
          }
#else // !FEATURE_ACM && !FEATURE_BMP_ACM
          int replaymode = 0;
          // SetMediaDataEx can only be called in idle state
          if (!AEEMedia_IsIdleState(pMedia))
          {
            MSG_HIGH("OEMMediaMPEG4_SetMediaParm... returning EBADSTATE", 0, 0, 0);
            return EBADSTATE;
          }

          // Need to call requestResource before setting the state to READY
          // since we are using the media state to choose the callback
          // function to be registered with ACM. The callback function is 
          // different for IMEDIA_SetMediaParm and IMEDIA_Play.
          nACMStatus = AEEMedia_RequestResource((AEEMedia *)po, TRUE);
          if(nACMStatus != SUCCESS && nACMStatus != MM_PENDING)
          {
            MSG_ERROR("OEMMediaMPEG4_SetMediaParm: returning, EBADSTATE", 0, 0, 0);
            return EBADSTATE;
          }

          if(p1 != NULL)
          {
            replaymode = ((AEEMediaData*)p1)->clsData;
          }
          // if this is PULL replay mode, just copy the data and set state to ready
          if(replaymode == MMD_HTTP_PULL_BUFFER_PLAYBACK)
          {
            // pull mode replay
            MEMCPY(&pMedia->m_md, (AEEMediaData*)p1, sizeof(AEEMediaData));
            pme->m_nState = MM_STATE_READY;
            nRet = SUCCESS;
          }
          else
          {
            // Call AEEMedia_SetMediaParm, this will automatically
            // save the AEEMediaData to be used for delayed acquiring
            nRet = AEEMedia_SetMediaParm(po, nParmID, p1, p2);
          }
          if (nRet == SUCCESS)
          {
            if (nACMStatus == MM_PENDING)
            {
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif
              return SUCCESS;
            }
            else
            {
              nRet = OEMMediaMPEG4_SetMediaData (pMedia, p1);
            }
          }

          if (nRet != SUCCESS)
          {  
            AEEMedia_CleanUpResource((AEEMedia *)po);
          }
#endif // FEATURE_ACM || FEATURE_BMP_ACM
        }
        break;

      case MM_MP4_AUDIO_VIDEO_MEDIA_DATA:
         nRet = OEMMediaMPEG42PV_OpenURN((char *)p1, (char *)p2, NULL, pOEM );
         if (nRet == SUCCESS)
         {
            pme->m_nState = MM_STATE_READY;
         }
         break;

      case MM_PARM_RECT:
         /* Clear the screen when the clipping rectangle has changed.
          * It is important to do this synchronously instead of waiting
          * until the next frame update since the App may have drawn to the
          * screen by then.
          */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
         /* First check whether rectangle dimensions have actually changed */
         if (OEMMediaMPEG4_CheckRectChanged(&pOEM->m_rectClip, (AEERect *)p1) != TRUE)
           break;
#ifdef FEATURE_MDP
         mdp_clear_ovimg(pOEM->m_Video.id);
		//MDP requires deregistration to clear the screen
		 mdp_deregister_ovimg(pOEM->m_Video.id);
#endif /* FEATURE_MDP */
#else /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
         IDISPLAY_EraseRgn( pOEM->m_pIDisplay,
                            pOEM->m_rectClip.x,
                            pOEM->m_rectClip.y,
                            pOEM->m_rectClip.dx,
                            pOEM->m_rectClip.dy );
#endif /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
         MEMCPY(&pOEM->m_rectClip, (void *)p1, sizeof(AEERect));
         pOEM->m_bRectClipChanged = TRUE;
#ifndef T_QSC1110 // Gemsea Add
         if(pme->m_nState == MM_STATE_PLAY)
         {
            disp_lock_screen(pOEM->m_rectClip.y,pOEM->m_rectClip.dy,pOEM->m_rectClip.x,pOEM->m_rectClip.dx);
         }
#endif
         break;

      case MM_PARM_FRAME:
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
         if( !pOEM->m_bFrameCBEnabled && p1 )
         {  /* If we're switching from Direct Mode to Frame CB Mode */
            /* Erase the frame from the MDP */
            pOEM->m_bMDPScale = FALSE;
            OEMMediaMPEG4_MDPDeregister(pOEM);
         }
#endif /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
         pOEM->m_bFrameCBEnabled = p1;
         break;

      case MM_MP4_PARM_ROTATION:
         nRet = OEMMediaMPEG42PV_RotateVideo((AEEMediaMPEG4RotationType)p1, pOEM);
        break;

      case MM_MP4_PARM_SCALING:
         nRet = OEMMediaMPEG42PV_ScaleVideo((AEEMediaMPEG4ScalingType)p1, 0, 0, pOEM);
        break;

      case MM_MP4_PARM_ASCALING:
         nRet = OEMMediaMPEG42PV_ScaleVideo(MM_MPEG4_ASCALE, (uint16)p1, (uint16)p2, pOEM);
         break;

      case MM_MP4_PARAM_AUDIO_OVERRIDE:
         OEMMediaMPEG42PV_SetAudOverride((int32)p1);
         break;

      case MM_MP4_PARM_RTSP_STATUS_NOTIFY:
         OEMMediaMPEG42PV_SetRTSPStatusNotify(p1, pOEM);
         break;

      case MM_PARM_TICK_TIME:
         // set the tick time what ever application sends as p1
         // m_nTickInterval reflects the Periodicity of MM_TICK_UPDATE event 
         // default implies 1000ms and zero implies no event 
         pOEM->m_nTickInterval = ((uint32)p1);
         break;

      case MM_PARM_POS:
      {
         int32 startTime = 0;
         int32 nIsStop = (p1 & ((int32)(0x0000FFFF)));           // 16-bit LSB:0/1 for start/stop
         int32 nSeekMode = ((p1 & ((int32)(0xFFFF0000))) >> 16); // 16-bit MSB: Seek mode (MM_SEEK_MODE_XXX), 

         if (nSeekMode == MM_SEEK_MODE_TIME)
         { 
            // Ensure that the specified time is valid.
            if (p2 >= 0 || p2 == -1)
            { 
               // set startTime to p2 if 16-bit LSB of p1 is 0
               if (!nIsStop) 
               {
                  startTime = p2;
               }
               
               // p2 = -1 indicates last value. skip SetStartPos in this case
               if (startTime >= 0)
               {
                  nRet = OEMMediaMPEG42PV_SetStartPos((uint32)startTime, pOEM); 
               }
            }
            else
            {
               nRet = EBADPARM; // Invalid time (p2)
            }
         }
         else 
         {
            nRet = EUNSUPPORTED; //Unsupported seek mode
         }

         break;
      }

      case MM_MP4_PARM_LOOP_TRACK:
         nRet = OEMMediaMPEG42PV_SetLoopTrack((boolean)p1, pOEM);
         break;

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

    case MM_MP4_PARM_MEDIA_INFO:
         nRet = OEMMediaMPEG42PV_SetMediaInfo((void *)p1, p2, pOEM);
         break;

#ifdef FEATURE_QTV_STREAM_RECORD
      case MM_MP4_PARM_RTSP_STREAM_CONVERT:
         nRet =  OEMMediaMPEG42PV_RecordStream((AEEStreamRecordInfo *)p1, pOEM);
         break;

      case MM_MP4_PARM_RTSP_STOP_STR_CONVERT:
         nRet = OEMMediaMPEG42PV_StopStreamRecording();
         break;
#endif

      case MM_PARM_PLAY_TYPE:
        nRet = OEMMediaMPEG4_SetPlayType(pme, (int)p1);
        break;

      case MM_MP4_PARAM_MODIFY_BUFFER_LOWMARK:
         nRet = OEMMediaMPEG42PV_ModifyBufferLowMark(p1);
         break;
 
      case MM_MP4_PARAM_SET_DATA_PORT_RANGE:
        nRet = OEMMediaMPEG42PV_SetDataPortRange((int)p1, (int)p2);
        break;

      case MM_PARM_MUTE:
        nRet = OEMMediaMPEG42PV_SetMuteCtl(pOEM, &p1);
         break;

#if defined (FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
      case MM_MP4_PARAM_COPY_BUFFER_TO_FILE:
         nRet = OEMMediaMPEG42PV_CopyBufferToFile((char *)p1,p2);
         break;

      case MM_MP4_PARAM_SET_PVX_SAVE_OPTION:
         nRet = OEMMediaMPEG42PV_SetPvxSaveOption((char *)p1);
         break;

      case MM_MP4_PARAM_SET_PVX_PREROLL:
         nRet = OEMMediaMPEG42PV_SetPvxPreroll(p1);
         break;    
#endif /* FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2 */

      case MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_DATA_SIZE:
         nRet = OEMMediaMPEG42PV_RegisterFetchDataSizeCallback((uint32 *)p1);
         break;

      case MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_DATA:
         nRet = OEMMediaMPEG42PV_RegisterFetchDataCallback((uint32 *)p1);
         break;

      case MM_MP4_PARAM_REGISTER_DRM_CALLBACK_BUFFER_SUPPORTED_TYPE:
         nRet = OEMMediaMPEG42PV_RegisterIsMimeSupportedCallback((uint32 *)p1);
         break;

      case MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA_SIZE:
         if(p1)
         {
           pOEM->m_bPULLDataMode = TRUE;
           pOEM->m_FetchBufferSizeCB = (AEEFetchBufferedDataSizeT)p1;
           if(p2)
           {
             pOEM->m_pClientData = (void*)p2;
           }
         }		 
         break;

      case MM_MP4_PARAM_REGISTER_CALLBACK_BUFFER_DATA:
         if(p1)
         {
           pOEM->m_bPULLDataMode = TRUE;
           pOEM->m_FetchBufferCB = (AEEFetchBufferedDataT)p1;
           if(p2)
           {
             pOEM->m_pClientData = (void*)p2;
           }
         }		 
         break;

      case MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION:
         nRet = OEMMediaMPEG42PV_SetFileSaveOption((char *)p1);
         break;

      case MM_MP4_PARAM_HTTP_FREE_DOWNLOAD_BUFFER:
         nRet = OEMMediaMPEG42PV_FreeDownloadBuffer((char *)p1); 
         break;
      case MM_MP4_PARM_PRIORITY:
         nRet = EBADPARM;
         if (p1 > 0) 
         {
           pOEM->m_nPriority = (uint32)p1;
           nRet = SUCCESS;
           // Register the Default priority for QTV
           //nRet = OEMMediaMPEG42PV_RegisterInst(pOEM,pOEM->m_nPriority);
         }
         break;
#ifdef FEATURE_QTV_GENERIC_BCAST
      case MM_MP4_PARM_MUTE_TRACK:
         nRet = OEMMediaMPEG42PV_MutePlaybackTracks((uint32)p1, (uint32)p2, pOEM);
         break;
      case MM_MP4_PARM_SELECT_PB:
         nRet = OEMMediaMPEG42PV_SelectPlaybackTracks((uint32)p1, (AEETrackID*)p2, pOEM);
         break;
      case MM_MP4_PARM_REPOSITION:
         nRet = OEMMediaMPEG42PV_PausedReposition((uint32)p1, (uint32*)p2, pOEM);
         break;
      case MM_MP4_PARM_PLAY_IFRAME_MODE:
         if ((!pOEM->m_bSeekPending || !pOEM->m_bSkipInPause) &&
             (pOEM->m_bPaused || !pOEM->m_bPlayStarted))
         {
           pOEM->m_nPlayFrameMode = p1;
           nRet = SUCCESS;
         }
         else 
         {
           nRet = EBADSTATE;
         }
         break;
#endif /* FEATURE_QTV_GENERIC_BCAST */

      case MM_MP4_PARM_PB_SPEED:
      {
        nRet = OEMMediaMPEG42PV_SetPlaybackSpeed((AEEMediaPlaybackSpeedType)p1, pOEM);
        break;
      }

#ifdef FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION
      case MM_MP4_PARM_DUAL_MONO:
      {
        nRet = OEMMediaMPEG42PV_SetDualMonoOutput(pOEM);
        break;
      }
#endif /* FEATURE_QTV_DUAL_MONO_OUTPUT_SELECTION */
     case MM_MP4_PARM_START_FROM_POS:
      { 
        pOEM->m_positionAtStop = p1; 
        break;
      }

	 case MM_MP4_PARM_DRM_DECRYPTION:
         /* Register the DRM decryption method */

         nRet = EBADPARM;
         if (p1)
         {
           nRet = OEMMediaMPEG42PV_RegisterDRMDecryptMethod((AEEMediaDRMDecryptMethodT)p1, (void *)p2, pOEM);
         }
         break;
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
      case MM_PARM_ENABLE: 
         if (p1 != 0 && (pOEM->m_dwCaps & p1) != 0)
         {
            pOEM->m_dwEnabledCaps = (pOEM->m_dwCaps & p1); 
         }
         else
         {
            nRet = EBADPARM;
         }
         break; 

      default:
         nRet = EUNSUPPORTED;
   }	
   MSG_HIGH("OEMMediaMPEG4_SetMediaParm Exit nRet = %d",nRet,0,0);
   return nRet;
}

/*==============================================================================

==============================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_GetMediaParm(IMedia * po, int nParmID, int32 * p1, int32 * p2)
{
   OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)po;
   OEMMediaMPEG4Layer * pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);
   int nRet = SUCCESS;

   MSG_HIGH("OEMMediaMPEG4_GetMediaParm... nParmID:%d; p1:%d; p2:%d",nParmID,p1,p2);
   
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_GetMediaParm: pOEM is NULL, return badparam", 0, 0, 0);
      return EBADPARM;
   }

   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_GetMediaParm(po, nParmID, p1, p2);

   switch (nParmID)
   {
      case MM_PARM_AUDIO_DEVICE:
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_VOLUME:
        nRet = OEMMediaMPEG42PV_GetVolume(pOEM, NULL);
        break;

      case MM_PARM_CLSID:
         if (p1 == NULL)
         {
            MSG_ERROR("OEMMediaMPEG4_GetMediaParm: returning EFAILED", 0, 0, 0);
            return EFAILED;
         } 

         *p1 = AEECLSID_MEDIAMPEG4;
         break;

      case MM_PARM_CAPS:
         if (p1 == NULL)
         {
            MSG_ERROR("OEMMediaMPEG4_GetMediaParm: returning EFAILED", 0, 0, 0);
            return EFAILED;
         }

         *p1 = (int32)pOEM->m_dwCaps;
         if (p2)
            *p2 = 0;
         break;

      case MM_PARM_RECT:
         if ( !p1 | !pme )
         {
           MSG_ERROR("OEMMediaMPEG4_GetMediaParm: returning EFAILED", 0, 0, 0);
           return EFAILED;
         }

         MEMCPY( (void *)p1, &pOEM->m_rectClip, sizeof(AEERect));
         break;

      case MM_PARM_FRAME:
         if ((p1 == NULL && p2 == NULL) ||
             (p1 != NULL && p2 != NULL))
         { /* Invalid parameters */
            MSG_ERROR("OEMMediaMPEG4_GetMediaParm: returning EFAILED", 0, 0, 0);
            return EFAILED;
         }

         if (p1 == NULL)
         { /* App is calling IMEDIA_IsFrameCallback */
            *p2 = pOEM->m_bFrameCBEnabled;
           MSG_HIGH("App calling IMEDIA_IsFrameCallback GetFrameInfo Wont be called",0,0,0);
         }
         else
         { /* App is calling IMEDIA_GetFrame */
            IBitmap* pFrame;
            /* The standard GetFrame does not care about extended info
               so just pass a NULL for extended info ptr-ptr */
            b_is_GetFrame = TRUE; // Gemsea Add
            nRet = OEMMediaMPEG42PV_GetFrame(&pFrame, NULL, pOEM);
            MSG_FATAL("b_is_GetFrame========%d",b_is_GetFrame,0,0);// Gemsea Add
            if (nRet == SUCCESS)
            {
               *(IBitmap**)p1 = pFrame;
               no_of_getframes_called++;
            }
            else if (p1 != NULL)
            {
               *p1 = 0;
            }
         }
         break;

      case MM_PARM_POS:
         {
            AEEMediaPlaybackPos pos;
            nRet = OEMMediaMPEG42PV_GetPlaybackPos(&pos, pOEM);
            if (SUCCESS == nRet) 
            {
               *(uint32 *)p1 = pos.dwPlaybackPos;
            }
         }
         break;

      case MM_MP4_PARM_FRAME_EXT:
         /*-------------------------------------------------------------------
         Retrieve the extended frame info pointer in addition to the frame 
         pointer that is returned for the standard MM_PARM_FRAME
         -------------------------------------------------------------------*/
         if (p1 == NULL || p2 == NULL) 
         { /* Invalid params. Extended frame info requires both frame and
              extended frame info ptrs. */
           MSG_ERROR("OEMMediaMPEG4_GetMediaParm: returning EFAILED", 0, 0, 0);
           return EFAILED;
         }
         else
         {
           IBitmap* pFrame;
           AEEMediaExtFrameInfoHeader *pExtFrmInfo;
           nRet = OEMMediaMPEG42PV_GetFrame(&pFrame, &pExtFrmInfo, pOEM);
           if (nRet == SUCCESS)
           {
             *(IBitmap**)p1 = pFrame;
             *(AEEMediaExtFrameInfoHeader**)p2 = pExtFrmInfo;
           }
         }
         break;

      case MM_MP4_PARM_TELOP_SUB_STR:
#ifdef FEATURE_MP4_KDDI_TELOP_TEXT
         OEMMediaMPEG42PV_GetTelopSubString((AEETelopTextSubString **)p1, *p2, pOEM);
         break;
#endif /* FEATURE_MP4_KDDI_TELOP_TEXT */

      case MM_MP4_PARM_MEDIA_SPEC:
         nRet = OEMMediaMPEG42PV_GetMediaSpec((AEEMediaMPEG4Spec **)p1, p2, pOEM);
         break;

      case MM_MP4_PARM_TRACK_TIME_SCALE:
         nRet = OEMMediaMPEG42PV_GetTrackTimeScale((AEEMediaMPEG4TrackType)(int32)p1, (uint32 *)p2);
         break;

      case MM_MP4_PARAM_PLAYBACK_POS:
         nRet = OEMMediaMPEG42PV_GetPlaybackPos((AEEMediaPlaybackPos *)p1, pOEM);
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
      case MM_MP4_PARM_ATOM_FTYP_SIZE: //’ftyp’ atom
      case MM_MP4_PARM_ATOM_DCMD_SIZE: //’dcmd’ atom : DRM
      case MM_MP4_PARM_ATOM_UDTA_CPRT_SIZE: //’cprt’ atom
      case MM_MP4_PARM_ATOM_UDTA_AUTH_SIZE: //’auth’ atom
      case MM_MP4_PARM_ATOM_UDTA_TITL_SIZE: //'titl' atom
      case MM_MP4_PARM_ATOM_UDTA_DSCP_SIZE: //'dscp' atom
      case MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_SIZE: //Text origin_x
      case MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_SIZE: //Text origin_y
      case MM_MP4_PARM_TEXT_TKHD_WIDTH_SIZE: //Text width
      case MM_MP4_PARM_TEXT_TKHD_HEIGHT_SIZE: //Text height
        OEMMediaMPEG42PV_GetDataSize((int32)nParmID, (uint32 *)p1);
        break;

      case MM_MP4_PARM_ATOM_FTYP_DATA: //’ftyp’ atom
      case MM_MP4_PARM_ATOM_DCMD_DATA: //’dcmd’ atom : DRM
      case MM_MP4_PARM_ATOM_UDTA_CPRT_DATA: //’cprt’ atom
      case MM_MP4_PARM_ATOM_UDTA_AUTH_DATA: //’auth’ atom
      case MM_MP4_PARM_ATOM_UDTA_TITL_DATA: //'titl' atom
      case MM_MP4_PARM_ATOM_UDTA_DSCP_DATA: //'dscp' atom
      case MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_DATA: //Text origin_x
      case MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_DATA: //Text origin_y
      case MM_MP4_PARM_TEXT_TKHD_WIDTH_DATA: //Text width
      case MM_MP4_PARM_TEXT_TKHD_HEIGHT_DATA: //Text height
      case MM_MP4_PARM_MEDIA_INFO: // Media information
        OEMMediaMPEG42PV_GetData((int32)nParmID, (uint8 *)p1, (uint32 *)p2);
        break;

      case MM_PARM_MUTE:
        nRet = OEMMediaMPEG42PV_GetMuteCtl(pOEM, p1);
        break;

      case MM_MP4_PARM_VIDEO_CODEC:
        nRet = OEMMediaMPEG42PV_GetVideoCodec((AEEMediaMP4CodecType *)p1, pOEM);
        break;

      case MM_MP4_PARM_AUDIO_CODEC:
        nRet = OEMMediaMPEG42PV_GetAudioCodec((AEEMediaMP4CodecType *)p1, pOEM);
        break;	 

      case MM_MP4_PARM_VIDEO_FRAME_RATE:
        nRet = OEMMediaMPEG42PV_GetVideoFrameRate((float *)p1, pOEM);
        break;		

      case MM_MP4_PARM_VIDEO_FRAME_RATE_EX:
        nRet = OEMMediaMPEG4Qtv_GetFrameRate((double *)p1, pOEM);
        break;

      case MM_MP4_PARM_VIDEO_AVERAGE_BIT_RATE:
        nRet = OEMMediaMPEG42PV_GetVideoAverageBitRate((unsigned long *)p1, pOEM);
        break;	 

      case MM_MP4_PARM_AUDIO_AVERAGE_BIT_RATE:
        nRet = OEMMediaMPEG42PV_GetAudioAverageBitRate((unsigned long *)p1, pOEM);
        break;

      case MM_MP4_PARM_TOTAL_RX_DATA_RATE:
        nRet = OEMMediaMPEG42PV_GetTotalRxDataRate((uint32*)p1);
        break;

      case MM_PARM_PLAY_TYPE:
         if (p1)
            *p1 = pme->m_nPlayType;
         break;

      case MM_MP4_PARAM_HTTP_GET_ENCRYPTED_DATA:
         nRet = OEMMediaMPEG42PV_GetEncryptedData((uint32 *)p1, (uint32 *)p2);
         break;

#ifdef FEATURE_QTV_DIVX_DRM
      case MM_MP4_PARM_DRM_REG_CODE:
        nRet = OEMMediaMPEG42PV_GetRegistrationCode((char *)p1, (int*)p2, pOEM);
        break;
#endif//#ifdef FEATURE_QTV_DIVX_DRM

#ifdef FEATURE_QTV_GENERIC_BCAST
      case MM_MP4_PARM_READ_TRACK:
         nRet = OEMMediaMPEG42PV_ReadPlaybackTracks((uint32 *)p1, (AEETrackList *)p2, pOEM);
         break;
#endif /* FEATURE_QTV_GENERIC_BCAST */

      case MM_PARM_ENABLE: 
         if (p1 != NULL)
         {
            *p1= (int32)pOEM->m_dwEnabledCaps;
         }
         if (p2!= NULL)
         {
            *p2 = 0;
         }
         break; 

      default:
        nRet = EUNSUPPORTED;
   }
   /* TODO
   return OEMMediaMPEG4Layer_GetMediaParm(pOEM, nParmID, p1, p2);
   */
   MSG_HIGH("OEMMediaMPEG4_GetMediaParm...return nRet:%d",nRet,0,0);
   return nRet;
}

/*==================================================================

==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_Play(IMedia * po)
{
   int nRet;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(((OEMMediaMPEG4*)po)->m_pMPEG4);
	
    MSG_HIGH("OEMMediaMPEG4_Play Entry ",0,0,0);
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_Play: pOEM is NULL, return badparam", 0, 0, 0);
      return EBADPARM;
   }
   if (pOEM->m_bSuspend) 
   {
      MSG_ERROR("OEMMediaMPEG4_Play: pOEM is NULL, return badstate", 0, 0, 0);
      return EBADSTATE;
   }

   pOEM->m_bPausePending = FALSE;

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
   if (pOEM->b_SetMediaDataRequest)
   {
      pOEM->b_PlayRequest = TRUE; 
      return SUCCESS; 
   }
#endif

   nRet = AEEMedia_Play(po);

   MSG_HIGH("OEMMediaMPEG4_Play calling AEEMEDIA_PLAY , nRet = %d",nRet,0,0);
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
   if (nRet == MM_PENDING)
   {
     // ACM Resource Acquire Pending
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif
     return SUCCESS;
   }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   if (nRet != SUCCESS)
   {
      MSG_HIGH("OEMMediaMPEG4_Play... return nRet:%d",nRet,0,0);
      return nRet;
   }

#if defined (FEATURE_QTV_QOS_SELECTION) && defined (FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif

#ifndef T_QSC1110 // Gemsea Add
   disp_lock_screen(pOEM->m_rectClip.y,pOEM->m_rectClip.dy,pOEM->m_rectClip.x,pOEM->m_rectClip.dx);
#endif

   nRet = OEMMediaMPEG42PV_Play(0, pOEM);
   MSG_HIGH("OEMMediaMPEG4_Play calling OEMMediaMPEG42PV_Play , nRet = %d",nRet,0,0);
   nMdpFramePendingCounter =0;
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
   if(nRet != SUCCESS)
     AEEMedia_CleanUpResource((AEEMedia*)po);
#endif // FEATURE_ACM || FEATURE_BMP_ACM
   MSG_HIGH("OEMMediaMPEG4_Play... return nRet:%d",nRet,0,0);
   return nRet;
}

/*==================================================================
   Unsupported functions -- used in vtbl of IMedia classes
==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_RecordUnsupported(IMedia * po)
{
   MSG_HIGH("OEMMediaMPEG4_RecordUnsupported Entry",0,0,0);
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 
   MSG_HIGH("OEMMediaMPEG4_RecordUnsupported Exit ",0,0,0);
   return EUNSUPPORTED;
}

/*==================================================================

==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_Stop(IMedia * po)
{
   int            nRet;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(((OEMMediaMPEG4*)po)->m_pMPEG4);	
   MSG_HIGH("OEMMediaMPEG4_Stop Entry",0,0,0);
#if defined(FEATURE_BMP_ACM)   
#error code not present
#endif 

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_Stop: pOEM is NULL, return badstate", 0, 0, 0);
      return EBADSTATE;
   }
   nRet = AEEMedia_Stop(po);

#if defined(FEATURE_BMP_ACM)   
#error code not present
#endif 

   pOEM->m_bPausePending  = FALSE;
   pOEM->m_nPlayFrameMode = 0;
   if (nRet != SUCCESS)
      return nRet;
#if defined (FEATURE_QTV_QOS_SELECTION) && defined (FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif
  MSG_HIGH("OEMMediaMPEG4_Stop, nMdpFramePendingCounter %d",nMdpFramePendingCounter,0,0);
  nMdpFramePendingCounter=0;
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
   OEMMediaMPEG4_MDPDeregister(pOEM);
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
   MSG_HIGH("OEMMediaMPEG4_Stop Exit returns from OEMMediaMPEG42PV_Stop",0,0,0);

#ifndef T_QSC1110 // Gemsea Add
   disp_lock_screen(0,0,0,0);
#endif
   return OEMMediaMPEG42PV_Stop(pOEM);
}
/*=====================================================================
This API is called when QTV_PLAYER_STATUS_PAUSED_SUSPENDED is notified 
Releases the resource
======================================================================*/
void OEMMediaMPEG4_Conc_ACM_Stop(AEEMedia *pOEM)
{
   
   if(NULL == pOEM)
   {
     return;
   }
#if defined(FEATURE_BMP_ACM)   
#error code not present
#endif

   /* This releases the CMX resource from Qtv */
   AEEMedia_Stop((IMedia *)pOEM);

   return;
}
/*==================================================================

==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   int            nRet;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(((OEMMediaMPEG4*)po)->m_pMPEG4);

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_Seek - pOEM is NULL", 0, 0, 0);
      return EBADPARM;
   }
   if (pOEM->m_bSuspend) 
   {
      MSG_ERROR("OEMMediaMPEG4_Seek - In Suspend state", 0, 0, 0);
      return EBADSTATE;
   }
   if( pOEM->m_bPauseInProcess == TRUE )
   {
      MSG_ERROR("OEMMediaMPEG4_Seek - Pause in process", 0, 0, 0);
	   return EBADSTATE;
   }
   pOEM->m_bPausePending = FALSE;
   pOEM->m_nPlayFrameMode = 0;
   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
   {
      MSG_ERROR("OEMMediaMPEG4_Seek - AEEMedia_Seek return %d", nRet, 0, 0);
      return nRet;
   }
#if defined (FEATURE_QTV_QOS_SELECTION) && defined (FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif
   MSG_HIGH("OEMMediaMPEG4_Seek - Return",0,0,0);

   return OEMMediaMPEG42PV_Seek(eSeek, lTimeMS, pOEM);
}

/*==================================================================

==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_Pause(IMedia * po)
{
   int            nRet;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(((OEMMediaMPEG4*)po)->m_pMPEG4);

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_Pause - pOEM is NULL", 0, 0, 0);
      return EBADSTATE;
   }
   pOEM->m_nPlayFrameMode = 0;
#ifdef FEATURE_QTV_QDSP_RELEASE_RESTORE

#ifdef FEATURE_QTV_QOS_SELECTION
#error code not present
#endif /* FEATURE_QTV_QOS_SELECTION */
   /* Allow pause procesing to go through if it is due to an
   ** incoming call.
   */
   if ( !qtv_cfg_enable_dsp_release 
        || (AEEMedia_IsPlayState((AEEMedia *)po) || AEEMedia_IsRecState((AEEMedia *)po)) )
#endif /* FEATURE_QTV_QDSP_RELEASE_RESTORE */
   {
     nRet = AEEMedia_Pause(po);
     if (nRet != SUCCESS)
     {
        MSG_ERROR("OEMMediaMPEG4_Pause - AEEMedia_Pause return %d", nRet, 0, 0);
        return nRet;
     }
   }
   pOEM->m_bPausePending = TRUE;
   MSG_HIGH("OEMMediaMPEG4_Pause - Return",0,0,0);
#ifndef T_QSC1110 // Gemsea Add
   disp_lock_screen(0,0,0,0);
#endif
   return OEMMediaMPEG42PV_Pause(pOEM);
}

/*==================================================================

==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_Resume(IMedia * po)
{
   int            nRet;
   /* If streamType is not MEDIA_BUFFERED_LIVE_STREAMING, resume should start from
      position at stop.
   */
   int startPlayPos = -1; 
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(((OEMMediaMPEG4*)po)->m_pMPEG4);   
   MSG_HIGH("OEMMediaMPEG4_Resume - Entry",0,0,0);
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 
   MSG_HIGH("OEMMediaMPEG4_Resume - AEEMedia_RequestResource",0,0,0);
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
   nRet =  AEEMedia_RequestResource((AEEMedia *)po, TRUE);
   if(nRet != SUCCESS && nRet != MM_PENDING)
   {
      MSG_HIGH("OEMMediaMPEG4_Resume - AEEMedia_RequestResource FAILED ",0,0,0);
      return nRet;
   }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_Resume - pOEM is NULL", 0, 0, 0);
      return EBADPARM;
   }
   if (pOEM->m_bSuspend) 
   {
      MSG_ERROR("OEMMediaMPEG4_Resume - In Suspend state", 0, 0, 0);
      return EBADSTATE;
   }

   pOEM->m_bPausePending = FALSE;
   pOEM->m_bPauseInProcess = FALSE;
   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
   {
      MSG_ERROR("OEMMediaMPEG4_Resume - AEEMedia_Resume return %d", nRet, 0, 0);
      return nRet;
   }
#if defined (FEATURE_QTV_QOS_SELECTION) && defined (FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif

   // if QTV is not in Pause state or is resume pending, reject the resume call
   if((!((OEMMediaMPEG4*)po)->m_pMPEG4->m_bPaused) || (((OEMMediaMPEG4*)po)->m_pMPEG4->m_bResumePending))
   {
     MSG_ERROR("OEMMediaMPEG4_Resume - Not in Pause or Resume Pending state", 0, 0, 0);
     return EBADSTATE;
   }
  
   ((OEMMediaMPEG4*)po)->m_pMPEG4->m_bResumePending = TRUE;

   /* In case repositioning is allowed, it is a recorded playback, allow to resume with -1 -1 */
   if (pOEM->m_MPEG4Spec.streamtype == MM_MEDIA_BUFFERED_LIVE_STREAMING && pOEM->m_MPEG4Spec.bRepositioningAllowed ==0)
   {
      /* if streamtype is MM_MEDIA_BUFFERED_LIVE_STREAMING, resume from the tip position */
      startPlayPos = 0;
   }
   MSG_HIGH("OEMMediaMPEG4_Resume - Return",0,0,0);
#ifndef T_QSC1110 // Gemsea Add
   disp_lock_screen(pOEM->m_rectClip.y,pOEM->m_rectClip.dy,pOEM->m_rectClip.x,pOEM->m_rectClip.dx);
#endif
   return OEMMediaMPEG42PV_Play(startPlayPos, pOEM);
}
/*==================================================================

==================================================================*/
/*! @ingroup IMediaAPI
 */
static int OEMMediaMPEG4_GetTotalTime(IMedia * po)
{
   int            nRet;
   OEMHandle pOEM = OEMMediaMPEG42PV_GetOEMLayer(((OEMMediaMPEG4*)po)->m_pMPEG4);
   MSG_HIGH("OEMMediaMPEG4_GetTotalTime - Entry",0,0,0);

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

   if (pOEM == NULL) 
   {
      MSG_ERROR("OEMMediaMPEG4_GetTotalTime: pOEM is NULL, return badparam", 0, 0, 0);
      return EBADPARM;
   }
   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
   { 
      MSG_ERROR("OEMMediaMPEG4_GetTotalTime: AEEMedia_GetTotalTime failed nRet=%d", nRet, 0, 0);
      return nRet;
   }
   MSG_HIGH("OEMMediaMPEG4_GetTotalTime - returns from OEMMediaMPEG42PV_GetClipInfo",0,0,0);
   return OEMMediaMPEG42PV_GetClipInfo(pOEM);
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*!
  @brief  BREW Notification Callback

  @detail
  This routine receives all of the callbacks queue'd via
  OEMMediaMPEG4_QueueCallback.  This function allows the OEM layer to filter or
  pre-process callbackss before 

  @note   This function is called in BREW/UI task context.
*/
void OEMMediaMPEG4_CallbackNotify(AEEMediaCallback * pcb)
{
  OEMMediaMPEG4 *    pme;
  OEMMediaMPEG4Layer *pOEM;
  void *             paLast;

  if (!pcb)
  {
    MSG_ERROR("PCB is NULL. Ignoring callback!", 0, 0, 0);
    return;
  }

  pme = (OEMMediaMPEG4 *)pcb->cmdNotify.pIMedia;
  if (!pme)
  {
    MSG_ERROR("PME is NULL. Ignoring callback!", 0, 0, 0);	
    return;
  }

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);
  if (!pOEM)
  {
	MSG_ERROR("POEM is NULL. Ignoring callback!", 0, 0, 0);
    return;
  }

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
  if ( pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
       (pcb->cmdNotify.nStatus == MM_MP4_STATUS_VIDEO_ABORT ||
        pcb->cmdNotify.nStatus == MM_STATUS_DONE            ||
        pcb->cmdNotify.nStatus == MM_STATUS_ABORT 
#ifdef FEATURE_QTV_FCS
#error code not present
#endif
	))
  {
    /* Video has ended */
    OEMMediaMPEG4_MDPDeregister(pOEM);
  }
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

#ifdef FEATURE_QTV_GENERIC_BCAST
  /* If in FramePlayMode, and received successful frame play, ask for the next one. */
  if (pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
      pcb->cmdNotify.nStatus == MM_MP4_STATUS_PLAYING &&
      pOEM->m_nPlayFrameMode ) 
  {
    int retVal;
    uint32 TransID;
    retVal = OEMMediaMPEG42PV_FrameSeek(pOEM->m_nPlayFrameMode, &TransID, TRUE, pOEM);
    if (retVal != SUCCESS) 
    {
      pcb->cmdNotify.nStatus = MM_STATUS_ABORT;
    }
    else
    {
      return;
    }
  }
#endif /* FEATURE_QTV_GENERIC_BCAST */

  /* If playback ready callback is received, execute the pending sound
     commend if there is any. */
  if (  pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
        pcb->cmdNotify.nStatus == MM_MP4_STATUS_PB_READY)
  {
    OEMMediaMPE4Qtv_handleSoundPending(pOEM);    
  }

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

  if (  pcb->cmdNotify.nCmd    == MM_CMD_PLAY &&
        pcb->cmdNotify.nStatus == MM_STATUS_FRAME &&
        !pOEM->m_bFrameCBEnabled )
  {
    /* We're about to send an MM_STATUS_FRAME to the Brew app.  However, they
    * haven't enabled them, so handle it ourselves and return.
    */
    IBitmap      * pFrame = NULL;
    /* frameInfo contains the information about the original frame to be
     * displayed
     */
    AEEBitmapInfo  frameInfo;
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
    boolean releaseFrame = FALSE;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
    /*
    * IMPORTANT NOTE: You need to do IBITMAP_Release(pFrame) after you're done with pFrame.
    */

    if(pOEM->m_bPausePending)
    {
      /* Player is paused just release the frame sent.
       *  Sending NULL to the below call gets the frames sent releases 
      */
      OEMMediaMPEG42PV_ReleaseCurrentVideoFrameBuffer(NULL);
      return;
    }
    if( IMEDIA_GetFrame((IMedia *)pme, &pFrame) == SUCCESS)
    {
      MSG_HIGH("IMEDIA_GetFrame called count = %d ",no_of_getframes_called,0,0);
      //GetFrame passed bitmap ref count is 2 now
      // Get the bitmap size
      IBITMAP_GetInfo(pFrame, &frameInfo, sizeof(frameInfo));

      /* Should recalculate dimensions and possibly clear the screen if the
       * dimensions of the current frame have changed from the previous one or
       * if the clipping rectangle has changed.
       *
       * Note: There's a race condition where a button masher pushing 'rotate' at
       * full speed can cause two rotations between processings of this loop,
       * resulting in an improper screen erase between rotations.  We need to
       * clear the screen whenever the image dimensions have changed.
       *
       * Also, perform frame clipping calculations only when the screen changes
       */
      if( (pOEM->m_dimPrev.x != frameInfo.cx) ||
          (pOEM->m_dimPrev.y != frameInfo.cy) ||
          (pOEM->m_bRectClipChanged) )
      {
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
        IBitmap *pDevBitmap;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
        
        /* Do not clear the screen on the first pass through or if the previous
         * dimensions were zero since we haven't drawn anything on the screen.
         */
        if( pOEM->m_dimPrev.x != 0 && pOEM->m_dimPrev.y != 0 )
        {
          /* Frame callbacks are not enabled, so clear the screen */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
#ifdef FEATURE_MDP
          mdp_clear_ovimg(pOEM->m_Video.id);
#endif /*FEATURE_MDP*/
#else /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
          IDISPLAY_EraseRgn( pOEM->m_pIDisplay,
                             pOEM->m_rectClip.x,
                             pOEM->m_rectClip.y,
                             pOEM->m_rectClip.dx,
                             pOEM->m_rectClip.dy );
#endif /* !FEATURE_QTV_MDP_TRANSFORMATIONS */
        }

        /* Reset the clip rectangle changed flag */
        pOEM->m_bRectClipChanged = FALSE;

        /* Keep track of the transformed (in case of MDP) video dimensions to
         * determine whether the video needs to be cleared the next time around.
         */
        pOEM->m_dimPrev.x = frameInfo.cx;
        pOEM->m_dimPrev.y = frameInfo.cy;
        
        /* Initialize the clipped image rectangle to the original frame's
         * dimensions.
         */
        pOEM->m_rectImage.dx = frameInfo.cx;
        pOEM->m_rectImage.dy = frameInfo.cy;

        /* Call the clipping function to calculate visible image */
        OEMMediaMPEG4_CalcClipping( pOEM, frameInfo );

#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
        if ( (pOEM->m_rectImage.dx >= VGA_SIZE) || 
             (pOEM->m_rectImage.dy >= VGA_SIZE) ) 
        {
          mddi_host_set_fast_clock(TRUE);
        }
        else if ( (pOEM->m_rectImage.dx < VGA_SIZE) &&
                  (pOEM->m_rectImage.dy < VGA_SIZE) ) 
        {
          mddi_host_set_fast_clock(FALSE);
        }
#endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2 */

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
        /* Support full-screen (annunciator bar off) mode by using AppToScreen
         * to convert between app coordinates and screen coordinates.  If we
         * can't get these interfaces, then the following code will assume that
         * we're in full-screen
         */
        if(IDISPLAY_GetDeviceBitmap(pOEM->m_pIDisplay, &pDevBitmap) == SUCCESS)
        {
          IBitmapCtl *pBitmapCtl;
          if(IBITMAP_QueryInterface(pDevBitmap, AEEIID_BITMAPCTL, (void **) &pBitmapCtl) == SUCCESS)
          {
            AEEPoint scrPoint, appPoint;
            appPoint.x = pOEM->m_rectImage.x;
            appPoint.y = pOEM->m_rectImage.y;
#if MIN_BREW_VERSION(4,0)
            scrPoint = IBitmapCtl_AppToScreen(pBitmapCtl, appPoint);
            IBitmapCtl_Release(pBitmapCtl);
#else
            scrPoint = IBITMAPCTL_AppToScreen(pBitmapCtl, appPoint);
            IBITMAPCTL_Release(pBitmapCtl);
#endif
            pOEM->m_rectImage.x = scrPoint.x;
            pOEM->m_rectImage.y = scrPoint.y;
          }
          IBITMAP_Release(pDevBitmap);
        }
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
      }               

      /* If the entire image is clipped, don't bother doing anything else.  This
       * is especially important for MDP where specifying a window w/ 0 h/w
       * causes strange behavior.
       */
      if( pOEM->m_rectImage.dx != 0 && pOEM->m_rectImage.dy != 0 )
      {
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
        IDIB   *pDIB = NULL;
        IYCbCr *pYCbCr = NULL;

        /* Let's see what the IBitmap really is.  First check IDIB */
        if( SUCCESS == IBITMAP_QueryInterface(pFrame, AEECLSID_DIB, (void**)&pDIB ) &&
            pDIB != NULL )
        {
          pOEM->m_Video.image.imgType   = MDP_RGB_565;
          pOEM->m_Video.image.bmy_addr  = (uint32*)pDIB->pBmp;
          pOEM->m_Video.image.width     = pDIB->cx;
          /* Scale the source height & width by the clipped result.  In other
           * words, multiply source width (pDIB->cx) by the ratio of clipped
           * width (m_rectImage.dx) to desired width (frameInfo.cx).
           */
          pOEM->m_Video.image.cwin.x2   = (pDIB->cx * pOEM->m_rectImage.dx / frameInfo.cx) - 1;
          pOEM->m_Video.image.cwin.y2   = (pDIB->cy * pOEM->m_rectImage.dy / frameInfo.cy) - 1;
        }
        else if( SUCCESS == IBITMAP_QueryInterface(pFrame, AEEIID_YCBCR, (void**)&pYCbCr) &&
                 pYCbCr != NULL )
        {
#ifdef FEATURE_QTV_MDP_COLOR_FORMAT_SWAP
#error code not present
#else
          pOEM->m_Video.image.imgType   = MDP_Y_CBCR_H2V2;
#endif
          pOEM->m_Video.image.bmy_addr  = (uint32*)pYCbCr->pLuma;
          pOEM->m_Video.image.cbcr_addr = (uint32*)pYCbCr->pChroma;
          pOEM->m_Video.image.width     = pYCbCr->nYPitch;
          /* Scale the source height & width by the clipped result.  In other
           * words, multiply source width (pYCbCr->cx) by the ratio of clipped
           * width (m_rectImage.dx) to desired width (frameInfo.cx).
           */
          pOEM->m_Video.image.cwin.x2   = (pYCbCr->cx * pOEM->m_rectImage.dx / frameInfo.cx) - 1;
          pOEM->m_Video.image.cwin.y2   = (pYCbCr->cy * pOEM->m_rectImage.dy / frameInfo.cy) - 1;
        }
        else
        {
          MSG_ERROR("Invalid interface ID and/or couldn't get IDIB/IYCbCr(%d,%d)", pDIB, pYCbCr, 0);
           /*BTW, if this happens then get frame itself should have failed :) */
            IBITMAP_Release(pFrame);
          return;
        }

        /* Fill in the MDP info structure for video */
        pOEM->m_Video.image.dest      = PRIMARY_LCD_TYPE;
        pOEM->m_Video.image.layer     = MDP_FGL;
        pOEM->m_Video.image.cwin.x1   = 0;
        pOEM->m_Video.image.cwin.y1   = 0;
        pOEM->m_Video.image.lcd_x     = pOEM->m_rectImage.x - BREW_OFFSET_X;
        pOEM->m_Video.image.lcd_y     = pOEM->m_rectImage.y - BREW_OFFSET_Y;
        pOEM->m_Video.image.mdpOp     = 
        OEMMediaMPEG4_MDPCalcTransforms(pOEM);

        QTV_PERF(QTV_PERF_MDP_START);
        /* Check if the current MDP handle is valid */
#ifdef QTV_ARMREGISTER_TIMING
        time_start_calculate( start_bitblt_time_ms[bitblt_start_index]);
		bitblt_start_index = bitblt_start_index + 1;
		bitblt_start_index = bitblt_start_index % 10;
#endif
#ifdef FEATURE_MDP
        if( mdp_is_ovimg_valid(pOEM->m_Video.id) )
        {
          /* The MDP handle was valid, so update the params via
           * mdp_replace_ovimg
           */
          if(!mdp_replace_ovimg(&pOEM->m_Video.image, pOEM->m_Video.id))
          {
              MSG_ERROR("mdp_replace_ovimg - OEMMediaMpeg4_CallbackNotify",0,0,0);
          }
        }
        else
        {
          /* The MDP handle wasn't valid, so register a new one! */
          pOEM->m_Video.id = mdp_register_ovimg(&pOEM->m_Video.image);
          MSG_MED( "Registered new MDP ID %d", pOEM->m_Video.id, 0, 0 );
        }
#endif /*FEATURE_MDP*/

        /* By now we should really have a valid MDP handle, but check just to
         * make sure
         */
        if( pOEM->m_Video.id != 0 )
        {
          QTVBITMAP_SUBTYPE  *temp_pBitmap;

          /* Update the display */
          MDP_CB cb;
          cb.cb_func = OEMMediaMPEG4_MdpCb;
          cb.arg = pFrame;//this should be unique for every frame

          /* release the previous frame as we just deregister it with MDP, it will get really released after 
           * both this release being called AND the one called in MDP callback */
          if(pOEM->m_Video.prev_pFrame)
          {
            MSG_HIGH("Ref count should be 2 here , releasing %x",pOEM->m_Video.prev_pFrame,0,0);
            IBITMAP_Release(pOEM->m_Video.prev_pFrame);
            pOEM->m_Video.prev_pFrame = NULL;
          }

          /* We have registered current frame, store it so that it can be released when we receive next frame */
          if((SUCCESS != IBITMAP_QueryInterface( pFrame, QTVBITMAP_IID, (void**)&temp_pBitmap)) && temp_pBitmap == NULL)
          {
            //update failed.
            MSG_ERROR("FAILED to store the Frame Buffer!", 0, 0, 0);
          }

          /* By calling the query interface, we increase the reference count of bitmap pFrame, we purposely keep
           * the increased reference count because we don't want to free it immediately after 
           * MDP callback.  We will keep the bitmap until MDP displays it ( after we receive MDP callback) 
           * AND it is deregistered with MDP so that we don't have flickering/blinking kind of video 
           */
          pOEM->m_Video.prev_pFrame = QTVBITMAP_SUBTYPE_TO_IBITMAP(temp_pBitmap);
#ifdef FEATURE_MDP
          if(!mdp_update_ovimg_async(pOEM->m_Video.id,&cb))
          {
            //update failed.
            //Free buffer immediately.
            MSG_ERROR("FAILED  mdp_update_ovimg_async %d", pOEM->m_Video.id, 0, 0);
            releaseFrame = TRUE;
          }
	  else 
          {
	      total_frames_sent_to_display++;
              MSG_HIGH(" OEMMediaMPEG4_CallbackNotify Success:  total_frames_sent_to_display = %d",total_frames_sent_to_display, 0, 0);
              nMdpFramePendingCounter++;
              MSG_HIGH("mdp_update_ovimg_async successful. Current pending frames to MDP %d", nMdpFramePendingCounter, 0, 0);
	  }
#endif /*FEATURE_MDP */
        }
        else
        {
          releaseFrame = TRUE;
          MSG_ERROR("MDP registration/replacement failed!!!", 0, 0, 0);
        }

        /* The IDIB/IYCbCr pointer must be released once we are finished using it */
        if( pDIB != NULL )
        {
          IDIB_Release(pDIB);
        }
        if( pYCbCr != NULL )
        {
          MSG_HIGH("  IYCBCR_Release(pYCbCr) ",0, 0, 0);
          IYCBCR_Release(pYCbCr);
        }
        if(releaseFrame)
        {
           /* If releaseFrame is true, it means queueing to MDP has failed.
            So release the frame */
          MSG_HIGH("  queueing to MDP has failed",0, 0, 0);
          IBITMAP_Release(pFrame);
        }

#else
#ifndef T_QSC1110 // Gemsea Add
    {
        IDIB *pDIB;
        
        if( SUCCESS == IBITMAP_QueryInterface(pFrame, AEECLSID_DIB, (void**)&pDIB) )
        {
            disp_update_yuv420(pDIB->pBmp, pDIB->cy, pDIB->cx);
            IDIB_Release(pDIB);
        }
    }
#else
        /* Display the frame at centered on the screen */
        IDISPLAY_BitBlt(pOEM->m_pIDisplay,
                        pOEM->m_rectImage.x,  pOEM->m_rectImage.y,
                        pOEM->m_rectImage.dx, pOEM->m_rectImage.dy,
                        pFrame, 0, 0, AEE_RO_COPY);
        IDISPLAY_Update(pOEM->m_pIDisplay);
#endif
        IBITMAP_Release(pFrame);
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
      } /* if( dx != 0 && dy != 0 ) */
      else
      {
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
        IBITMAP_Release(pFrame);
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */
      }
    }
    else
    {
      if(pFrame != NULL)
        IBITMAP_Release(pFrame);
    }
    /* Do not send this callback to the app as they are not registered for it */
    return;
  }
  

  if(MM_MP4_STATUS_3GPP_TTEXT == pcb->cmdNotify.nStatus)
  {
       MUTEX_LOCK_CS(TimedText3gppCS);
  }
  
  paLast = ENTER_APP_CONTEXT(pOEM->m_pAppContext);
  AEEMedia_CallbackNotify((AEEMedia *)pme, pcb);
  LEAVE_APP_CONTEXT(paLast);

  if(MM_MP4_STATUS_3GPP_TTEXT == pcb->cmdNotify.nStatus)
  {
       MUTEX_UNLOCK_CS(TimedText3gppCS);
  }


  if(pcb->cmdNotify.nStatus == MM_MP4_STATUS_INFO && pcb->cmdNotify.pCmdData != NULL)
  {
    // Media sent data to the app, release the buffer allocated in Mpeg4Player
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
    ReleaseOutputBuffer(NULL, NULL, pcb->cmdNotify.pCmdData);
#else
    FreeOutputBuffer(NULL,NULL,pcb->cmdNotify.pCmdData);
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
  }
}

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
/*!
 * @detail
 * Callback function called by MDP when it has finished processing the frame
 */
static void OEMMediaMPEG4_MdpCb(void *pArgs,MDP_STATUS Status) 
{
#ifdef QTV_ARMREGISTER_TIMING
  uint32 bitblt_time_temp = 0;
#endif
  if(pArgs != NULL)
  {
    /*PArgs should be unique since we create bitmap for each frame */
    IBITMAP_Release((IBitmap *)pArgs);
    QTV_PERF(QTV_PERF_MDP_STOP);
    nMdpFramePendingCounter--;
    MSG_HIGH("MDP release CB successful. Current pending frames to MDP %d, CB Status %d", nMdpFramePendingCounter, Status, 0);
    MSG_HIGH("OEMMediaMPEG4_MdpCb - total_frames_displayed = %d frame %x",total_frames_displayed,pArgs,0);
#ifdef QTV_ARMREGISTER_TIMING
    time_end_calculate(start_bitblt_time_ms[bitblt_end_index], end_bitblt_time_ms[bitblt_end_index], bitblt_time_temp); 
    bitblt_time_ms += bitblt_time_temp;
    total_frames_displayed++;
if(bitblt_time_temp > maxdisplay)
    {    
      maxdisplay = bitblt_time_temp;
    }
    bitblt_end_index = bitblt_end_index + 1;
    bitblt_end_index = bitblt_end_index % 10;
#endif
  }
  else 
  { 
      MSG_HIGH("OEMMediaMPEG4_MdpCb - pArgs = NULL",0,0,0);
  }
}
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

/*!
  @brief
  Callback from ISOUND

  @detail
  Callback from ISOUND.  Mapping the status and command based on the current sound command which 
  has been sent to ISOUND.  Update current volume or mute if callback indicates that set/get volume,
  set mute request succeeds.
*/
static void OEMMediaMPEG4_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam)
{
  OEMMediaMPEG4 *      pme = (OEMMediaMPEG4 *)AEEObjectMgr_GetObject((AEEObjectHandle)pUser);
  OEMHandle            pOEM;
  AEESoundCmdData *    pData = (AEESoundCmdData *)dwParam;
  AEEMediaCallback     mcb;
  void *               paLast;

  if (!pme) 
  {
    return;
  }

  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);
  if (pOEM == NULL) 
  {
    return;
  }

  MEMCPY(&mcb, &pOEM->m_cbCommand, sizeof(AEEMediaCallback));

  mcb.bInUse = TRUE;
  mcb.cmdNotify.nSubCmd = pOEM->m_Sound.sound_cmd.aee_param; // get MM_PARM_, such as MM_PARM_VOLUME
  mcb.cmdNotify.nCmd = pOEM->m_Sound.sound_cmd.aee_cmd;  // get MM_CMD_, such as MM_CMD_SETMEDIAPARM
  mcb.cmdNotify.nStatus = (eStatus == AEE_SOUND_SUCCESS) ? MM_STATUS_DONE : MM_STATUS_ABORT;
  mcb.cmdNotify.pCmdData = NULL;
  mcb.cmdNotify.dwSize = 0;

  if(eStatus == AEE_SOUND_SUCCESS)
  {
    // set volume is successful, save it
    if(pOEM->m_Sound.sound_cmd.cmd == SOUNDCMD_SET_VOLUME)
  {
      // update current volume
      pOEM->m_Sound.current_volume = pOEM->m_Sound.sound_cmd.param.volume;  
    }
    else if (pOEM->m_Sound.sound_cmd.cmd == SOUNDCMD_SET_MUTE) // set mute is sucessful, save it
    {
      // update current volume
      pOEM->m_Sound.current_mute = pOEM->m_Sound.sound_cmd.param.bMute;
    }
    if(pData && (pOEM->m_Sound.sound_cmd.cmd == SOUNDCMD_GET_VOLUME))
    {
      // update current volume
      pOEM->m_Sound.current_volume = pData->wVolume;
    mcb.cmdNotify.pCmdData = (void *)(uint32)pData->wVolume;
    mcb.cmdNotify.dwSize = sizeof(pData->wVolume);
  }
  }

  pOEM->m_Sound.cb_pending = FALSE;
  paLast = ENTER_APP_CONTEXT(pOEM->m_pAppContext);
  AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
  LEAVE_APP_CONTEXT(paLast);
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
  AEEMediaCallback *  pmcb;

  if (!pcb) 
  {
    MSG_ERROR("OEMMediaMPEG4_Notify, pcb is NULL. Ignoring callback!", 0, 0, 0);
    return;
  }

  pMedia = (OEMMediaMPEG4 *)pcb->cmdNotify.pIMedia;
  if (!pMedia) 
  {
    MSG_ERROR("OEMMediaMPEG4_Notify, pMedia is NULL. Ignoring callback!", 0, 0, 0);
    return;
  }

  pme = pMedia->m_pMPEG4;
  if (!pme) 
  {
    MSG_ERROR("OEMMediaMPEG4_Notify, pme is NULL. Ignoring callback!", 0, 0, 0);
    return;
  }

  pmcb = MALLOC(sizeof(AEEMediaCallback));
  if (!pmcb)
  {
    MSG_ERROR("OEMMediaMPEG4_Notify, MALLOC can't allocate %d bytes for pmcb", sizeof(AEEMediaCallback), 0, 0);
    return;
  }

  MEMCPY(pmcb, pcb, sizeof(AEEMediaCallback));

  /* Free the callback structure */
  MUTEX_LOCK();
  pcb->bInUse = FALSE;
  MUTEX_UNLOCK();

  if (pme->m_pfnNotify) 
  {
    pme->m_pfnNotify(pmcb);
  }

switch (pcb->cmdNotify.nStatus)
  {
  	case MM_STATUS_TICK_UPDATE:
        AEE_SetTimer( pme->m_nTickInterval, UpdateProgressBar, (void*)pme );
        break;

	#if defined (FEATURE_QTV_PSEUDO_STREAM) || defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        case MM_MP4_STATUS_PS_BUFFER_UPDATE:
        AEE_SetTimer( pme->m_nTickInterval, UpdateProgressBar, (void*)pme );
	break;
	#endif

	#ifdef FEATURE_QTV_PROGRESSIVE_DL_STREAMING_2
	case MM_STATUS_DOWNLOAD_TICK_UPDATE:
	AEE_SetTimer( pme->m_nTickInterval, UpdateDownloadProgressBar, (void*)pme );
	break;
	#endif
  }


  FREE(pmcb);
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
  int freeIndex = 0;
  MSG_HIGH("OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn) Entry",0,0,0);
  /* Only allow one instantiation of the MPEG4 IMedia object and one Ringer inst*/
  if( !po || (g_uMPEG4RefCnt >= MAX_NUM_OEM_PLAYBACK_INST) )      
    return NULL;

  pme = (OEMMediaMPEG4Layer *)MALLOC(sizeof(OEMMediaMPEG4Layer));

  if (!pme)
    return NULL;

  MEMSET(pme,0,sizeof(OEMMediaMPEG4Layer));
  pme->m_pMedia = (AEEMedia *)po;
  pme->m_pAppContext = AEE_GetAppContext();

  if (AEEObjectMgr_Register(pme->m_pMedia, &pme->m_hObject) != SUCCESS)
  {
    FREE(pme);
    return NULL;
  }

  pme->m_pfnNotify = pfn;  // pfn is OEMMediaMPEG4_CallbackNotify
  pme->m_dwCaps = MM_CAPS_AUDIO | MM_CAPS_VIDEO;
  pme->m_dwEnabledCaps = pme->m_dwCaps; // Enable all capabilities by default

  // Initialize callbacks.
  for (i = 0; i < OEMMPEG4_MAX_CB; i++)
  {
    pme->m_cb[i].bInUse = FALSE;
    pme->m_cb[i].cmdNotify.pIMedia = po;
    pme->m_cb[i].cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;
    pme->m_cb[i].cmdNotify.nStatus = 0;
    CALLBACK_Init(&pme->m_cb[i].cb, OEMMEDIAMPEG4_NOTIFY_CB, &pme->m_cb[i]);
  }

  pme->m_cbCommand.cmdNotify.pIMedia = po;
  pme->m_cbCommand.cmdNotify.clsMedia = ((AEEMedia *)po)->m_clsSelf;

  ISHELL_GetDeviceInfo(pme->m_pMedia->m_pIShell, &di);

  /* Get a pointer to the IDisplay */
  nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_DISPLAY, (void **)&pme->m_pIDisplay);
  if( nRetVal != SUCCESS )
  {
    MSG_FATAL( "Get IDisplay Failed(%d)", nRetVal, 0, 0 );
    AEEObjectMgr_Unregister(pme->m_hObject);
    FREE(pme);
    return NULL;
  }

  nRetVal = ISHELL_CreateInstance(pme->m_pMedia->m_pIShell, AEECLSID_SOUND, (void **)&pme->m_pISound);
  if(SUCCESS != nRetVal)
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MSG_FATAL( "Get Isound Failed(%d)", nRetVal, 0, 0 );
    AEEObjectMgr_Unregister(pme->m_hObject);
    FREE(pme);
    return NULL;
  }

    ISOUND_RegisterNotify(pme->m_pISound, OEMMediaMPEG4_SoundCB, (void *)pme->m_hObject);

  /* Initialize other variables */
  pme->m_bFrameCBEnabled = FALSE;
  pme->m_dimPrev.y = pme->m_dimPrev.x = 0;
  pme->m_nPriority = MM_QTV_PRIORITY_DEFAULT; //QTV_PRIORITY_DEFAULT 
#ifdef FEATURE_QTV_OEM_BUFFER_MGR
  q_init( &pme->m_qOutputBuffer );
#endif /* FEATURE_QTV_OEM_BUFFER_MGR */
  pme->m_positionAtStop = 0;

  // At this point, the media player has not made a call informing the OEM
  // layer the size of the display area.  For now, we'll call
  // PVPlayer::Init() with a 0 x 0 display area.  When the media player sets
  // the available display area for the video, we'll forward that information
  // to the PV MPEG4 engine via PVPlayer::SetDisplayArea().
  nRetVal = OEMMediaMPEG42PV_Init(pme);
  if( nRetVal != SUCCESS )
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MM_RELEASEIF(pme->m_pISound);
    AEEObjectMgr_Unregister(pme->m_hObject);
    FREE(pme);
    return NULL;
  }

  nRetVal = OEMMediaMPEG42PV_RegisterForCallback((void*)pme->m_hObject, pme);

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

  if (nRetVal != SUCCESS)
  {
    MM_RELEASEIF(pme->m_pIDisplay);
    MM_RELEASEIF(pme->m_pISound);
    AEEObjectMgr_Unregister(pme->m_hObject);
    pme->m_bTerminating=TRUE;
    OEMMediaMPEG42PV_Terminate(pme);
    pme->m_bTerminating=FALSE;
    FREE(pme);
    pme = NULL;
  }
  nMdpFramePendingCounter = 0;
  
  pme->m_bPULLDataMode     = FALSE;
  pme->m_pClientData       = NULL;
  pme->m_FetchBufferSizeCB = NULL;
  pme->m_FetchBufferCB     = NULL;
  
  for(freeIndex = 0; freeIndex < MAX_NUM_OEM_PLAYBACK_INST; freeIndex++)
  {
       if(g_pOEMLayer[freeIndex] == NULL)
       break;
  }
  if(freeIndex < MAX_NUM_OEM_PLAYBACK_INST )
  {
      MSG_HIGH("OEMMediaMPEG4Layer_New freeIndex = %d",freeIndex,0,0);      
      g_pOEMLayer[freeIndex] = pme;
  }
  else 
  {
      MSG_HIGH("OEMMediaMPEG4Layer_New freeIndex not found returning NULL",0,0,0);
      return NULL;
  }
  ++g_uMPEG4RefCnt;
  MSG_HIGH("OEMMediaMPEG4Layer_New(IMedia * po, PFNNOTIFY pfn) Exit",0,0,0);
  return pme;
}

static IBase * OEMMediaMPEG4_New(IShell * ps, AEECLSID cls)
{
  IMedia *      po;
  OEMMediaMPEG4 * pme;
  #ifdef FEATURE_BMP
   static const AEEVTBL(ISettings) vt = {
      AEEBASE_AddRef(ISettings),
      AEEBASE_Release(ISettings),
      AEEBASE_QueryInterface(ISettings),
      OEMMediaMPEG4Settings_Get,
      OEMMediaMPEG4Settings_Set,
      OEMMediaMPEG4Settings_Delete,
      OEMMediaMPEG4Settings_Reset,
      OEMMediaMPEG4Settings_OnChange,
      OEMMediaMPEG4Settings_GetNumChildren,
      OEMMediaMPEG4Settings_GetChildName,
      OEMMediaMPEG4Settings_GetSubTree
   };
  #endif
  MSG_HIGH("OEMMediaMPEG4_New Entry",0,0,0);   
  // Alloc memory for the object
  po = (IMedia *)AEE_NewClass((IBaseVtbl *)&gMediaMPEG4Funcs, sizeof(OEMMediaMPEG4));
  if (!po)
  {
    MSG_ERROR("OEMMediaMPEG4_New, po is NULL...", 0, 0, 0);
    return NULL;
  }

#if defined (FEATURE_QTV_QOS_SELECTION) && defined (FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif

  // Call base class constructor
  if (SUCCESS != AEEMedia_New(po, ps, cls))
  {
    MSG_ERROR("OEMMediaMPEG4_New, AEEMedia_New failed....", 0, 0, 0);
    FREE(po);
    return NULL;
  }

  // Call MPEG4 Layer constructor
  pme = (OEMMediaMPEG4 *)po;
  pme->m_pMPEG4 = OEMMediaMPEG4Layer_New(po, (PFNNOTIFY)OEMMediaMPEG4_CallbackNotify);

  #ifdef FEATURE_BMP
  pme->m_qtvISettings.pMe = pme;   
  AEEINITVTBL(&(pme->m_qtvISettings), IModule, vt);
  #endif

  if ( !pme->m_pMPEG4 )
  {
    AEEMedia_Delete(po);
    FREE(po);
    MSG_ERROR("OEMMediaMPEG4_New, pme->m_pMPEG4 is NULL....", 0, 0, 0);
    return NULL;
  }

#if defined(FEATURE_BMP_ACM)  
#error code not present
#endif 

  pme->m_nPlayType = MM_PLAY_TYPE_NORMAL;
  MSG_HIGH("OEMMediaMPEG4_New Exit",0,0,0);
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
   AEEMedia_Init(ps, "video/3gpp", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/amc", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/3g2", AEECLSID_MEDIAMPEG4); 
   AEEMedia_Init(ps, "video/3gpp2", AEECLSID_MEDIAMPEG4);  
   AEEMedia_Init(ps, "video/k3g", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/skm", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/mfpt", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/m4a", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/mp4a-latm", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/mp4", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/m4a", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/x-m4a", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/3gpp2", AEECLSID_MEDIAMPEG4);

   
#ifdef FEATURE_QTV_WINDOWS_MEDIA
   AEEMedia_Init(ps, "video/asf", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/wmv", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/wma", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/asx", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/wma", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/wax", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/x-wav", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "audio/x-ms-wma", AEECLSID_MEDIAMPEG4);
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#ifdef FEATURE_QTV_AVI
  AEEMedia_Init(ps, "video/avi", AEECLSID_MEDIAMPEG4);
#ifdef FEATURE_QTV_AVI_DIVX_PARSER
  AEEMedia_Init(ps, "video/divx", AEECLSID_MEDIAMPEG4);
#endif /* FEATURE_QTV_AVI_DIVX_PARSER */
#endif

   AEEMedia_Init(ps, "video/flo", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/tam", AEECLSID_MEDIAMPEG4);
   AEEMedia_Init(ps, "video/qml", AEECLSID_MEDIAMPEG4);
#ifdef FEATURE_REAL_PLAYER_USE_QTV_API
#ifdef FEATURE_REAL_NV_CONFIG
    if (OEMMediaReal_IsRealEnabled())
#endif
    {
       AEEMedia_Init(ps, "video/rm", AEECLSID_MEDIAMPEG4);
       AEEMedia_Init(ps, "video/ra", AEECLSID_MEDIAMPEG4);
       AEEMedia_Init(ps, "video/rv", AEECLSID_MEDIAMPEG4);
       AEEMedia_Init(ps, "video/ram", AEECLSID_MEDIAMPEG4);
    }
#endif

#ifdef FEATURE_QTV_DRM_DCF
  AEEMedia_Init(ps, "video/dcf", AEECLSID_MEDIAMPEG4);
#endif

#ifdef FEATURE_QTV_MAPI_MEDIA_MASK
#error code not present
#endif
#ifdef FEATURE_QTV_MAPI_REPLAY
#error code not present
#endif
}

/*==================================================================
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaMPEG4_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IBase * pobj;

   if (g_uMPEG4RefCnt < MAX_NUM_OEM_PLAYBACK_INST)
   {
      pobj = OEMMediaMPEG4_New(ps, cls);
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

   pMedia = (OEMMediaMPEG4 *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   //safty guard to prevent crash
   if (pMedia == NULL)
   {
      return 0;
   }
   pme = pMedia->m_pMPEG4;

   if(pme != NULL)
   {
     for( i = 0; i < MAX_NUM_OEM_PLAYBACK_INST; i++ ) 
     {
       if(g_pOEMLayer[i] == pme) 
         break;
     }

     if( i >= MAX_NUM_OEM_PLAYBACK_INST) 
     {
       MSG_HIGH( "OEMMediaMPEG4_QueueCallback() : pme is not equal to any element of global array g_pOEMLayer , just return 0 ", 0, 0, 0 );
       return 0;
     }
   }
   else
   {
     MSG_HIGH( "OEMMediaMPEG4_QueueCallback() : pme is NULL , just return 0 ", 0, 0, 0 );
     return 0;
   }

   /* Find a free callback structure */
   MUTEX_LOCK();
   for (i = 0; i < OEMMPEG4_MAX_CB; i++)
   {
      if (pme->m_cb[i].bInUse == FALSE)
      {
         pme->m_cb[i].bInUse = TRUE;
         pcb = &pme->m_cb[i];
	 pcb->cmdNotify.nStatus = nStatus;
   	 pcb->cmdNotify.nCmd = nCmd;
   	 pcb->cmdNotify.nSubCmd = nSubCmd;
   	 pcb->cmdNotify.pCmdData = pData;
   	 pcb->cmdNotify.dwSize = dwSize;
   	 AEE_ResumeCallback(&pcb->cb, pme->m_pAppContext);
         break;
      }
   }
   MUTEX_UNLOCK();

   if (i == OEMMPEG4_MAX_CB)
   {
     if(pme->m_bTerminating)
     {
       return -1;
     }
     else
     {
       unsigned int totalSleepDuration = 0;
       do
       {
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
	     pcb->cmdNotify.nStatus = nStatus;
             pcb->cmdNotify.nCmd = nCmd;
             pcb->cmdNotify.nSubCmd = nSubCmd;
             pcb->cmdNotify.pCmdData = pData;
             pcb->cmdNotify.dwSize = dwSize;
             AEE_ResumeCallback(&pcb->cb, pme->m_pAppContext);
             break;
           }
         }
         MUTEX_UNLOCK();
       } while(j== OEMMPEG4_MAX_CB && totalSleepDuration < UI_TRANSITION_TO_RUN_TIMEOUT);

       
       if( j == OEMMPEG4_MAX_CB )
       {
         MSG_FATAL("Out of callback structures", 0, 0, 0);
         return -1;
       }
     }
   }
   
   return 0;
}

/*==================================================================
  This function extracts audio/video file names from extended media
  data structure and then calls QTV openURN function.
==================================================================*/
static int OEMMediaMPEG4_OpenURN(AEEMediaDataEx *pmdExList, int nCount, OEMHandle pOEM)
{
  AEECLSID  clsData;
  int       i;
  pOEM = OEMMediaMPEG42PV_GetOEMLayer(pOEM);

  if( !pmdExList || !nCount || !pOEM )
    return EBADPARM;

  /* currently QTV only support one type of input (file(s) or buffer(s)) */
  clsData = pmdExList[0].clsData;
  for(i=1; i<nCount; i++)
  {
    if(clsData != pmdExList[i].clsData)
      return EUNSUPPORTED;
  }

#if defined (FEATURE_QTV_QOS_SELECTION) && defined (FEATURE_QTV_QDSP_RELEASE_RESTORE)
#error code not present
#endif

  if(clsData ==  MMD_FILE_NAME)   /* all members are file name */
  {
    char     *pVideoFile=NULL;
    char     *pAudioFile=NULL;
    char     *pTextFile=NULL;

    int nLen = 0;
    int nRet = 0;
    char* pTempFilePath = NULL; 
    const char* RelativePathStartTag = "fs:";
    boolean allocateVideoFile = FALSE;
    boolean allocateAudioFile = FALSE;
    boolean allocateTextFile = FALSE;

    /* this will accept first audio or video file in the list */
    for(i=0; i<nCount; i++)
    {
      /*
      * While running OAT Media, we get relative path. 
      * So, need to convert relative file path to absolute file path before passing down to QTV. 
      * AEE_ResolvePath and OEMFS_GetNativePath together helps to convert to abs. file path.
      * 
      * These functions have no effct if we pass in absolute file path.
      */
      if( !pVideoFile && (pmdExList[i].dwCaps & MM_CAPS_VIDEO) )
      {
        // if file path starts with "fs:/~", it is a relative path and needs to resolve.
        if(!strncmp(RelativePathStartTag,(char *)(pmdExList[i].pData),strlen(RelativePathStartTag)))
        {
           allocateVideoFile = TRUE;
           if(SUCCESS == AEE_ResolvePath((char *)pmdExList[i].pData, 0, &nLen))
           {
             pTempFilePath = (char*)MALLOC(nLen);
             if(pTempFilePath)
             {
               if(SUCCESS == AEE_ResolvePath((char *)pmdExList[i].pData, pTempFilePath, &nLen) )
               {
                 #if MIN_BREW_VERSION(4,0)
                  if (SUCCESS == OEMefs_GetNativePath(pTempFilePath,0, 0, &nLen))              
                 #else
                  if (SUCCESS == OEMFS_GetNativePath(pTempFilePath,0, &nLen))  
                 #endif
                 {
                   pVideoFile = (char*)MALLOC(nLen);
                   if(pVideoFile)
                   {
                     #if MIN_BREW_VERSION(4,0)
                       OEMefs_GetNativePath(pTempFilePath,pVideoFile, nLen, &nLen);                 
                     #else
                       OEMFS_GetNativePath(pTempFilePath,pVideoFile, &nLen);  
                     #endif
                   }
                 }
                 if(pTempFilePath)
                 {
                   FREE(pTempFilePath);
                   pTempFilePath = NULL;
                 }            
               }
             }
           }
        }
        else
        {   
            pVideoFile = (char *)pmdExList[i].pData;
        }
      }
      if( !pAudioFile && (pmdExList[i].dwCaps & MM_CAPS_AUDIO) )
      {
        // if file path starts with "fs:/~", it is a relative path and needs to resolve.
        if(!strncmp(RelativePathStartTag,(char *)(pmdExList[i].pData),strlen(RelativePathStartTag)))
        {
           allocateAudioFile = TRUE;
           if(SUCCESS == AEE_ResolvePath((char *)pmdExList[i].pData, 0, &nLen))
           {
             pTempFilePath = (char*)MALLOC(nLen);
             if(pTempFilePath)
             {
               if(SUCCESS == AEE_ResolvePath((char *)pmdExList[i].pData, pTempFilePath, &nLen) )
               {
                 #if MIN_BREW_VERSION(4,0)
                   if (SUCCESS == OEMefs_GetNativePath(pTempFilePath,0, 0, &nLen))            
                 #else
                   if (SUCCESS == OEMFS_GetNativePath(pTempFilePath,0, &nLen))   
                 #endif
                 {
                   pAudioFile = (char*)MALLOC(nLen);
                   if(pAudioFile)
                   {
                     #if MIN_BREW_VERSION(4,0)
                       OEMefs_GetNativePath(pTempFilePath,pAudioFile, nLen, &nLen);                  
                     #else
                       OEMFS_GetNativePath(pTempFilePath,pAudioFile, &nLen); 
                     #endif
                   }
                 }
                 if(pTempFilePath)
                 {
                   FREE(pTempFilePath);
                   pTempFilePath = NULL;
                 }            
               }
             }
           }
        }
        else
        {
          pAudioFile = (char *)pmdExList[i].pData;
        }
      }
      if( !pTextFile && (pmdExList[i].dwCaps & MM_CAPS_TEXT) )
      {
        // if file path starts with "fs:/~", it is a relative path and needs to resolve.
        if(!strncmp(RelativePathStartTag,(char *)(pmdExList[i].pData),strlen(RelativePathStartTag)))
        {
           allocateTextFile = TRUE;
           if(SUCCESS == AEE_ResolvePath((char *)pmdExList[i].pData, 0, &nLen))
           {
             pTempFilePath = (char*)MALLOC(nLen);
             if(pTempFilePath)
             {
               if(SUCCESS == AEE_ResolvePath((char *)pmdExList[i].pData, pTempFilePath, &nLen) )
               {
                 #if MIN_BREW_VERSION(4,0)
                   if (SUCCESS == OEMefs_GetNativePath(pTempFilePath,0, 0, &nLen)) 
                 #else
                   if (SUCCESS == OEMFS_GetNativePath(pTempFilePath,0, &nLen)) 
                 #endif
                 {
                   pTextFile = (char*)MALLOC(nLen);
                   if(pTextFile)
                   {
                     #if MIN_BREW_VERSION(4,0)
                       OEMefs_GetNativePath(pTempFilePath,pTextFile, nLen, &nLen);
                     #else
                       OEMFS_GetNativePath(pTempFilePath,pTextFile, &nLen);
                     #endif
                   }
                 }
                 if(pTempFilePath)
                 {
                   FREE(pTempFilePath);
                   pTempFilePath = NULL;
                 }            
               }
             }
           }
        }
        else
        {
            pTextFile = (char *)pmdExList[i].pData;
        }
      }
    }
    if(pVideoFile || pAudioFile || pTextFile)
    {
      nRet = OEMMediaMPEG42PV_OpenURN(pVideoFile, pAudioFile, pTextFile, pOEM);
      /*
      * Free memory if allocated.
      */
      if((allocateVideoFile == TRUE)&&pVideoFile)
      {
        FREE(pVideoFile);
      }
      if((allocateAudioFile == TRUE)&&pAudioFile)
      {
        FREE(pAudioFile);
      }
      if((allocateTextFile == TRUE)&&pTextFile)
      {
        FREE(pTextFile);
      }
      return nRet;   
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
      return OEMMediaMPEG42PV_OpenBufferURN(pVideoBuf, nVideoBufSize, pAudioBuf, 
                                            nAudioBufSize, pOEM);
    }
  }
  return EBADPARM;
}

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
/*==================================================================
  This function clears the screen and disables the MDP
==================================================================*/
static void OEMMediaMPEG4_MDPDeregister(OEMHandle pOEM)
{
  if (!pOEM) 
{
    return;
  }

  /* Deregister the video overlay */
  if( pOEM->m_Video.id != 0 )
  {
    MSG_MED( "Deregistering MDP %d", pOEM->m_Video.id, 0, 0 );
#ifdef FEATURE_MDP
    mdp_clear_ovimg(pOEM->m_Video.id);
    mdp_deregister_ovimg(pOEM->m_Video.id);
#endif /* FEATURE_MDP */
  }
  pOEM->m_Video.id = 0;
  pOEM->m_dimPrev.y = 0;
  pOEM->m_dimPrev.x = 0;

  /* Free frame callback buffer */
  if (pOEM->m_Video.pDIB) 
  {
    if (pOEM->m_Video.pDIB->pBmp) 
    {
      FREE(pOEM->m_Video.pDIB->pBmp);
      pOEM->m_Video.pDIB->pBmp = 0;
    }
    IDIB_Release(pOEM->m_Video.pDIB);
    pOEM->m_Video.pDIB = 0;
  }
  if (pOEM->m_Video.pBitmap) 
  {
    IBITMAP_Release(pOEM->m_Video.pBitmap);
    pOEM->m_Video.pBitmap = 0;
  }
  
  /* free the stored frame buffer */
  if(pOEM->m_Video.prev_pFrame)
  {
    IBITMAP_Release(pOEM->m_Video.prev_pFrame);
    pOEM->m_Video.prev_pFrame = NULL;
  }
}

/*!
 * @brief
 * Set the MDP ascale variables and return the corresponding MDPOP 
 */
static uint32 OEMMediaMPEG4_MDPCalcTransforms( OEMHandle pOEM )
{
  uint32 MDPOperation = 0;
  OEMRotationType rot = pOEM->m_QtvCurrentRotation;
  OEMScalingType scale = pOEM->m_QtvCurrentScaling;
  
#ifdef FEATURE_QTV_MDP_ASCALE
  if ( pOEM->m_QtvCurrentScaling == OEM_ASCALE ) 
  {
    if (MDP_IS_ROTATED_SIDEWAYS(pOEM->m_QtvCurrentRotation))
    {
      pOEM->m_Video.image.ascale.height = MIN(pOEM->m_QtvCurrentAScale.x, 
                                              pOEM->m_rectClip.dx);
      pOEM->m_Video.image.ascale.width =  MIN(pOEM->m_QtvCurrentAScale.y,
                                              pOEM->m_rectClip.dy); 
    }
    else 
    {
      pOEM->m_Video.image.ascale.height = MIN(pOEM->m_QtvCurrentAScale.y,
                                              pOEM->m_rectClip.dy);
      pOEM->m_Video.image.ascale.width =  MIN(pOEM->m_QtvCurrentAScale.x,
                                              pOEM->m_rectClip.dx);
    }

#ifdef FEATURE_USE_CLKREGIM_DEVMAN_REV2
    if ( !pOEM->m_bClkrgmIsReg && 
         ((pOEM->m_Video.image.ascale.height >= VGA_SIZE) ||
         (pOEM->m_Video.image.ascale.height >= VGA_SIZE)) )
    {
       OEMMediaMPEG42PV_ClkrgmIncrease( pOEM );
    }
    else if ( pOEM->m_bClkrgmIsReg &&
              (pOEM->m_Video.image.ascale.height < VGA_SIZE) &&
              (pOEM->m_Video.image.ascale.width < VGA_SIZE) ) 
    {
       OEMMediaMPEG42PV_ClkrgmRelease( pOEM );
    }
#endif /* FEATURE_USE_CLKREGIM_DEVMAN_REV2 */

    /* If we would be AScaling by 1x, don't bother asking for it. */
    if ( (pOEM->m_Video.image.ascale.width  == (pOEM->m_Video.image.cwin.x2+1)) &&
         (pOEM->m_Video.image.ascale.height == (pOEM->m_Video.image.cwin.y2+1)) )
    {
      scale = OEM_NO_SCALING;
    }
 }
#endif /* FEATURE_QTV_MDP_ASCALE */
  
  switch( rot )
  {
    case OEM_90_CW_ROTATION:
      MDPOperation |= MDPOP_ROT90;
      break;
    case OEM_90_CCW_ROTATION:
      MDPOperation |= MDPOP_ROT270;
      break;
    case OEM_180_ROTATION:
      MDPOperation |= MDPOP_ROT180;
      break;
    case OEM_NO_ROTATION:
      MDPOperation |= MDPOP_NOP;
      break;
    default:
      /* error */
      MDPOperation |= MDPOP_NOP;
      break;
  }

  switch( scale )
  {
    case OEM_0P25X_SHRINK:
      MDPOperation |= MDPOP_VDO_1BY4;
      break;
    case OEM_0P5X_SHRINK:
      MDPOperation |= MDPOP_VDO_1BY2;
      break;
    case OEM_0P75X_SHRINK:
      MDPOperation |= MDPOP_VDO_3BY4;
      break;
    case OEM_2X_ZOOM:
      MDPOperation |= MDPOP_VDO_X2;
      break;
    case OEM_XSCALE:
    case OEM_NO_SCALING:
      MDPOperation |= MDPOP_NOP;
      break;
#ifdef FEATURE_QTV_MDP_ASCALE
    case OEM_ASCALE:
      MDPOperation |= MDPOP_ASCALE;
      break;
#endif /* FEATURE_QTV_MDP_ASCALE */
    default:
      /* error */
      MDPOperation |= MDPOP_NOP;
      break;
  }

  return MDPOperation;
}
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

static void OEMMediaMPEG4_CalcClipping( OEMHandle pOEM, AEEBitmapInfo  bitmapInfo)
{
  AEERect rectClip = pOEM->m_rectClip;
  AEERect *pImageRect = &pOEM->m_rectImage;
  uint16 dxClip = MIN( rectClip.dx, bitmapInfo.cx );
  uint16 dyClip = MIN( rectClip.dy, bitmapInfo.cy );
  
  /* Calculate the height and width of the image that will fit into the clipping
   * rectangle 'rectClip'.
   */
#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
  /* Using MDP, we need to specify the coordinates in terms of the original
   * frame dimensions.  Therefore, compare dimImage and rectClip.  Depending on
   * if we're rotated, scale the dimensions using height or width.  Obviously,
   * this gets ugly real quick.
   */
  if( bitmapInfo.cx > rectClip.dx )
  {
    if( MDP_IS_ROTATED_SIDEWAYS(pOEM->m_QtvCurrentRotation) && 
        (pOEM->m_QtvCurrentScaling != OEM_ASCALE) )
    {
      pImageRect->dy = (int16) ((uint32)pImageRect->dy) * rectClip.dx / bitmapInfo.cx;
    }
    else
    {
      pImageRect->dx = (int16) ((uint32)pImageRect->dx) * rectClip.dx / bitmapInfo.cx;
    }
  }
  if( bitmapInfo.cy > rectClip.dy )
  {
    if( MDP_IS_ROTATED_SIDEWAYS(pOEM->m_QtvCurrentRotation) &&
        (pOEM->m_QtvCurrentScaling != OEM_ASCALE) )
    {
      pImageRect->dx = (int16) ((uint32)pImageRect->dx) * rectClip.dy / bitmapInfo.cy;
    }
    else
    {
      pImageRect->dy = (int16) ((uint32)pImageRect->dy) * rectClip.dy / bitmapInfo.cy;
    }
  }
#else
  /* No MDP, so just calculate the veiwable width and height of the video
   * (dx and dy), with the maximum value defined by the video rectangle,
   * m_rectClip.
   */
  pImageRect->dx = dxClip;
  pImageRect->dy = dyClip;
#endif /* FEATURE_QTV_MDP_TRANSFORMATIONS */

  /* Calculate the top-left coordinates of where the video will be
   * displayed on the LCD.  Centers the image if it fits.  Otherwise fits
   * upper-left corner
   */
  pImageRect->x = (uint16)((rectClip.dx - dxClip) / 2 + rectClip.x);
  pImageRect->y = (uint16)((rectClip.dy - dyClip) / 2 + rectClip.y);

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


static int OEMMediaMPEG4_SetPlayType(OEMMediaMPEG4 * pme, int nPlayType)
{

   if (pme->m_clsSelf != AEECLSID_MEDIAMPEG4)
     return EUNSUPPORTED;

   if (nPlayType > MM_PLAY_TYPE_REMINDER)
     return EBADPARM;

   pme->m_nPlayType = nPlayType;

   return SUCCESS;
}

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
/*==================================================================
  This function frees the memory for the parms in IMedia_SetMediaDataEx 
  for delayed acquiring.
==================================================================*/
static void OEMMediaMPEG4_FreeMediaDataEx(OEMMediaMPEG4 * pme)
{
  int                 i;
  OEMMediaMPEG4Layer *pOEM = pme->m_pMPEG4;

  for(i=0; i < pOEM->m_nCount; i++)
  {
    if(pOEM->m_pmdExList[i].clsData == MMD_FILE_NAME) 
    {
      FREEIF (pOEM->m_pmdExList[i].pData);
    }
  }

  FREEIF (pOEM->m_pmdExList);
  pOEM->m_nCount = 0;
}

/*==================================================================
  This function saves the parms in IMedia_SetMediaDataEx for
  delayed acquiring..
==================================================================*/
static int OEMMediaMPEG4_CopyMediaDataEx(OEMMediaMPEG4 * pme,
                                         AEEMediaDataEx *pmdExList, 
                                         int nCount)
{
  int       i;
  OEMMediaMPEG4Layer * pOEM = OEMMediaMPEG42PV_GetOEMLayer(pme->m_pMPEG4);

  if( !pmdExList || !nCount || !pOEM )
    return EBADPARM;

  pOEM->m_pmdExList = MALLOC (sizeof(AEEMediaDataEx) * nCount);
  if (pOEM->m_pmdExList == NULL)
  {
    return ENOMEMORY;
  }
    
  MEMCPY (pOEM->m_pmdExList, pmdExList, sizeof(AEEMediaDataEx) * nCount);
  // the buffer is assumed to stay, so we only need to copy the file name
  for(i=0; i<nCount; i++)
  {
    if(pmdExList[i].clsData == MMD_FILE_NAME) 
    {
      pOEM->m_pmdExList[i].pData = STRDUP ((char *)pmdExList[i].pData);
      if (pOEM->m_pmdExList[i].pData == NULL)
      {
        break;
      }
    }
  }
    
  // set m_nCount to indicate number of valid entries
  pOEM->m_nCount = i;
  if (i == nCount)
  {
    return SUCCESS;
  }
  else
  {
    OEMMediaMPEG4_FreeMediaDataEx(pme);
    return ENOMEMORY;
  }
}
#endif // FEATURE_ACM || FEATURE_BMP_ACM

#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

#if defined (FEATURE_ACM)

void OEMMediaMPEG4_GetResourceCntlData(AEEMedia * po, int * nPlayType, uint32 * acmCB)
{
  OEMMediaMPEG4* pOEMMediaMPEG4 = (OEMMediaMPEG4*) po;

  *nPlayType = pOEMMediaMPEG4->m_nPlayType;

  if (AEEMedia_IsIdleState(po))
  {
    *acmCB = (uint32) OEMMediaMPEG4_AcmSetMediaDataCB;
  }
  else
  {
    (*acmCB) = (uint32) OEMMediaMPEG4_AcmPlayCB;
  }
}

int OEMMediaMPEG4_AcmSetMediaDataCB(ACM_Callback_data_type * po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  OEMMediaMPEG4 * pme = (OEMMediaMPEG4 *)pAEEMedia;
  int             nRet = SUCCESS;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;  
    
    // For IMedia_SetMediaParm
    if (pme->m_pMPEG4->m_nCount == 0)
    {   
      nRet = OEMMediaMPEG4_SetMediaData(pAEEMedia, 0);
    }
    // For IMedia_SetMediaParmEx
    else
    {
      nRet = OEMMediaMPEG4_OpenURN(pme->m_pMPEG4->m_pmdExList, 
                                   pme->m_pMPEG4->m_nCount, pme->m_pMPEG4);
    }

    if (nRet != SUCCESS)
    {      
      AEEMedia_CleanUpResource(pAEEMedia);
    }
  }
  else
  {
    // FORCE RELEASE from ACM
    // update AEEMedia state for ACM resource owner and stop
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      if (pAEEMedia->m_bResOwner == TRUE)
      {
        // In this case, the stop may not deliver us an event.
        // So, the ACM force release timer will kick in and delete
        // this entry.
        nRet = OEMMediaMPEG4_Stop((IMedia *)pAEEMedia);
        // Need to set this variable since AEEMedia_CleanupResource
        // may not be called.
        pAEEMedia->m_bResOwner = FALSE;
      }
      else
      {
        AEEMedia_CleanUpResource(pAEEMedia);
      }
    }
  }

  OEMMediaMPEG4_FreeMediaDataEx((OEMMediaMPEG4*) pAEEMedia);
  return nRet;
}

int OEMMediaMPEG4_AcmPlayCB(ACM_Callback_data_type * po)
{
  AEEMedia *      pAEEMedia = (AEEMedia *)(po->pdata);
  int             nRet = SUCCESS;

  if (pAEEMedia->m_clsSelf != AEECLSID_MEDIAMPEG4)
     return EUNSUPPORTED;

  if (po->state == ACM_RESOURCE_ACQUIRED)
  {
    pAEEMedia->m_bResOwner = TRUE;
     // DELAYED ACQUIRE from ACM

#ifdef FEATURE_QTV_MDP_TRANSFORMATIONS
    if( ((OEMMediaMPEG4*)po)->m_pMPEG4->m_Video.id != 0 )
    {
      OEMMediaMPEG4_MDPDeregister( ((OEMMediaMPEG4*)po)->m_pMPEG4 );
    }
#endif /* #ifdef FEATURE_QTV_MDP_TRANSFORMATIONS */

     nRet = OEMMediaMPEG42PV_Play(0, NULL);
     if(nRet != SUCCESS)
     {
       AEEMedia_CleanUpResource(pAEEMedia);
       return EFAILED;
     }
  }
  else
  {
    // FORCE RELEASE from ACM
    if(pAEEMedia->m_bForceRelease == FALSE)
    {
      pAEEMedia->m_bForceRelease = TRUE;
      if (pAEEMedia->m_bResOwner == TRUE)
      {
        nRet = OEMMediaMPEG4_Stop((IMedia *)pAEEMedia);
        pAEEMedia->m_bResOwner = FALSE;
      }
      else
      {
        AEEMedia_CleanUpResource(pAEEMedia);
      }
    }
  }
  return nRet;
}
#endif /* #ifdef FEATURE_ACM */
#endif  // FEATURE_APP_MPEG4
