/*===========================================================================

FILE: OEMVocoder.c

SERVICES: DMSS implementation of IVocoder interface

DESCRIPTION
  This file contains the implementation of the functions in IVocoder
  based on the QCT DMSS vocoder packet interface

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Include Files
-------------------------------------------------------------------*/
#include <stdlib.h>
#include "comdef.h"
#include "OEMFeatures.h"

/* QCT DMSS Vocoder Frame Interface */
#include "rex.h"
#ifdef FEATURE_MVS
#include "mvs.h"
#include "mvssup.h"
#endif
#include "snd.h" /* for playing dtmf tones */

#include "OEMVocoder.h"
#include "OEMHeap.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEEVocoder.h"
#include "AEEError.h"
#include "AEEStdLib.h"
#include "BREWVersion.h"
#include "AEECriticalSection.h"

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#if defined (FEATURE_ODM_UI)
#include "OEMAppResCtl.h"
#endif // FEATURE_ODM_UI

typedef struct IVOCODER_ACMInfo
{
#if defined (FEATURE_ODM_UI)
   IAppResCtl *            m_pAppResCtl;
#else // !FEATURE_ODM_UI
   OEMINSTANCE             handle;
#endif // FEATURE_ODM_UI
   uint32                  m_uAppResCtl_State;
   ACM_Callback_data_type  m_acm_callback_data_type;
} IVOCODER_ACMInfo;

#if defined (FEATURE_ODM_UI)
static int  OEMVocoder_ACMCreateInstance(IVOCODER_ACMInfo * pAcmInfo);
#endif // FEATURE_ODM_UI
static int  OEMVocoder_ACMAcquire(IVOCODER_ACMInfo * pAcmInfo, uint32 reasonCode);
static int  OEMVocoder_ACMFree(IVOCODER_ACMInfo * pAcmInfo);
static void OEMVocoder_ACMChangeNotify(void *p);
static void OEMVocoder_ACM_InStart_ChangeNotify(void *p);
static void OEMVocoder_ACM_OutStart_ChangeNotify(void *p);
static void OEMVocoder_ACM_InStart(void);
static void OEMVocoder_ACM_OutStart(void);

#endif // FEATURE_ACM

#define FARF_VOC_STATUS        0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

#if !defined(FEATURE_IVOCODER)

// The feature is not available in this DMSS build
/*
uint32 OEMVocoder_New(AEECLSID classId, int *nErr)
{
   if(nErr)
      *nErr = ECLASSNOTSUPPORT;

   FARF(VOC_STATUS,("OEMVocoder_New. Feature Not Supported"));

   return(0);
}
*/
#elif defined(FEATURE_MVS)

#include "queue.h"
#include "qdspcmd.h"
#include "rex.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

#define FRAME_DURATION  20
#define PCM_FRAME_LENGTH   320

//This MACRO is used to check the input parameter, IVocoder *, for NULL,
//to insure that this instance has access to use the vocoder, and to check
//to see if the vocoder has been configured.
#define CHECKUSAGE(pV) \
   if (!pV) \
   { \
      return(EBADCLASS); \
   } \
   if (!gpVocInUse) \
   { \
      return(AEE_VOC_ENOT_CONFIGED); \
   } \
   if (gpVocInUse != pV) \
   { \
      return(AEE_VOC_EIN_USE); \
   } \

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

typedef struct _OEMVocoder       OEMVocoder;

typedef enum
{
   BUFFERING,
   PLAYING,
   STOPPED
} DecodeStatusType;

/* frame sizes for each vocoder type and rate */
static uint16 const RateToLength[VOC_MAX][MAX_RATE] = {
   {0,  0,  0,  0,  0,  2}, //VOC_NONE
   {0,  0,  0,  0,  0,  2}, //VOC_IS96
   {0,  0,  0,  0,  0,  2}, //VOC_IS96A
   {0,  3,  7, 16, 34,  2}, //VOC_IS733
   {0,  2,  5, 10, 22,  2}, //VOC_IS127
   {0,  2,  5, 10, 22,  2}, //VOC_SMV
};

static const mvs_mode_type gVocoders[VOC_MAX+1] = {
   MVS_MODE_NONE,  //NONE
   MVS_MODE_NONE,  //IS-96
   MVS_MODE_NONE,  //IS-96A

#ifdef FEATURE_VOC_PACKET_INTERFACE
   MVS_MODE_IS733,
   MVS_MODE_IS127,
#else
   MVS_MODE_NONE,
   MVS_MODE_NONE,
#endif

   MVS_MODE_NONE,  //SMV

#ifdef FEATURE_VOC_PCM_INTERFACE
   MVS_MODE_LINEAR_PCM,
#else
   MVS_MODE_NONE,
#endif

#ifdef FEATURE_AMR_VOCODER
   MVS_MODE_AMR,
#else
   MVS_MODE_NONE,
#endif

#ifdef FEATURE_VOC_G711
   MVS_MODE_G711,
#else
   MVS_MODE_NONE,
#endif

#ifdef FEATURE_VOC_4GV
   MVS_MODE_4GV_NB,
#else
   MVS_MODE_NONE,
#endif

   MVS_MODE_NONE
};


struct _OEMVocoder
{
   uint32             m_nWatermark;
   uint16             m_nTxReduction;
   VocoderType        m_eVocoder;
   DataRateType       m_eMin;
   DataRateType       m_eMax;
   boolean            m_bOverwrite;
#if defined (FEATURE_ACM)
   IVOCODER_ACMInfo   m_pAcmInfo;
#endif // FEATURE_ACM
};

typedef struct
{
   q_link_type link;    //DMSS queue type
   uint8 * data;        //pointer to data block storing frame data
   uint16 length;       //length of frame in bytes
   VocoderType vocoder; //vocoder used to encode/decode the frame

   mvs_frame_info_type frameInfo; // rate/mode info for vocoder
} QFrame;


typedef struct
{
   q_type freeQ;        //Queue containing frame items not currently in use
   q_type inUseQ;       //Queue containing frame items currently in use
   QFrame * frames;     //Pointer to array of frame items. Used for storage only
   uint8 * data;        //Pointer to data block storing all frame data.
   boolean callApp;     //flag indicating if a user callback is needed.
   boolean operating;   //flag indicating if current process is operating
} QOperation;

typedef enum
{
   IVOC_MVS_FAILED,
   IVOC_MVS_SHUT,
   IVOC_MVS_OFF,
   IVOC_MVS_SHUT_AND_ACQUIRE,
   IVOC_MVS_ACQUIRE,
   IVOC_MVS_MODE_ENABLE,
   IVOC_MVS_ON
} ivoc_mvs_status_type;

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

// Internal functions
static void FreeIfUsed(void **ppObj);

static boolean OEMVocoder_InitPlayData(uint32 qSize,uint16 frameLen);
static void    OEMVocoder_ReleasePlayData(void);

static void   VocStateCB(mvs_event_type *event);

static void    ShutDownVocoder(void);


static void VocoderFrameAcquiredCB(uint8 *data, mvs_frame_info_type  *frame_info,
                                   uint16 len, mvs_pkt_status_type  *status);


static void VocoderPlayoutFrameCB(uint8 *data, mvs_frame_info_type *frame_info,
                                  mvs_pkt_status_type *status);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


static QOperation gAcquire = {0};         /* Struct used for encoding frames */
static QOperation gPlay = {0};            /* Struct used for decoding frames */
static OEMVocoder * gpVocInUse = NULL;    /* Pointer to the IVocoder with access
                                             to vocoder */
static QFrame gErasure;                   /* Erasure Frame used when no other
                                             frame available */
static uint16 gErasureCount = 0;          /* Count of frames erased since last
                                             callback */
static uint16 gPlayedCount = 0;           /* Count of frames played since last
                                             callback */
#ifdef FEATURE_VOC_PACKET_INTERFACE
static uint8 erasure_bits[2] = {0,0};     /* data bits to stuff into vocoder when
                                             playing erasure.  Needs to be
                                             allocated, contents don't matter */
#endif

/* NOTE:  It is necessary to control the 'operating' boolean for decoding by the
          DecodeStatusType variable because the vocoder task utilizes the
          'operating' boolean.  The 'operating' boolean for decoding cannot be
          modified directly by any application call or a race condition may be
          encountered. */
static DecodeStatusType gDecStat = STOPPED;        //State of the decoding operation.


//Callbacks registered by AEE Layer
AEECallback *gpcbReady = NULL,*gpcbHaveData = NULL,*gpcbNeedData = NULL,*gpcbPlayedData = NULL;

//mvs state change track
static ivoc_mvs_status_type gMvsStatus = IVOC_MVS_OFF; /* status of mvs */

static AEECriticalSection gIVocCriticalSection;

#define IVOC_LOCK()    AEECriticalSection_Enter(&gIVocCriticalSection)
#define IVOC_UNLOCK()  AEECriticalSection_Leave(&gIVocCriticalSection)

/*===============================================================================
                     FUNCTION DEFINITIONS
===============================================================================*/


/*===========================================================================

  Function: OEMVocoder_New

==============================================================================*/
OEMINSTANCE OEMVocoder_New(AEECLSID classId, int *nErr)
{
   OEMVocoder * pV;

   FARF(VOC_STATUS,("OEMVocoder_New"));

   if (classId != AEECLSID_VOCODER)
   {
      if(nErr)
         *nErr = ECLASSNOTSUPPORT;

      return(0);
   }

   pV = (OEMVocoder *)sys_malloc(sizeof(OEMVocoder));

   if (!pV)
   {
      FARF(VOC_STATUS,("OEMVocoder_New: Out of memory"));

      if(nErr)
         *nErr = ENOMEMORY;

      return(0);
   }


#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
   *nErr = OEMVocoder_ACMCreateInstance(&pV->m_pAcmInfo);
   if (*nErr)
       *nErr = EFAILED;
#else // !FEATURE_ODM_UI
   (pV->m_pAcmInfo).handle = (OEMINSTANCE) pV;
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM

   pV->m_eVocoder = VOC_NONE;

   if(nErr)
      *nErr = SUCCESS;

   FARF(VOC_STATUS,("OEMVocoder_New Success"));

   return((OEMINSTANCE)pV);
}


/*===========================================================================

   Function: OEMVocoder_Release

==============================================================================*/

int OEMVocoder_Delete(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   FARF(VOC_STATUS,("OEMVocoder_Delete"));

   if(!pV)
      return(EFAILED);

   //See if the instance being released had access to the vocoder,
   if (pV == gpVocInUse)
   {
      ShutDownVocoder();
   }

   FARF(VOC_STATUS,("Freeing OEMVocoder"));

   sys_free(pV);

   FARF(VOC_STATUS,("Ret OEMVoc_Del"));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_Configure

==============================================================================*/
int16 OEMVocoder_Configure(OEMINSTANCE handle,
                           const IVocoderConfigType *pconfig,
                           IVocoderInfoType * pInfo)
{
   uint16 newLen = 0;

   OEMVocoder *pV = (OEMVocoder *)handle;
   IVocoderConfigType config;

   if (!pV)
   {
      return EBADPARM;
   }
   else if (gpVocInUse && (gpVocInUse != pV))
   {
      // If vocoder is configured and it is not owned by this instance, then
      // return error.
      FARF(VOC_STATUS,("Do not have access to vocoder"));
      return AEE_VOC_EIN_USE;
   }

   if(!pconfig)
   {
      return(EBADPARM);
   }

   FARF(VOC_STATUS,("OEMVoc_Config"));

   config = *pconfig;

   //Zero out the return parameter, if provided.
   if (pInfo)
   {
      pInfo->frameDuration = 0;
      pInfo->maxFrameSize = 0;
   }

   // No vocoder indicates that the user is deregistering usage of the vocoder.
   if (config.vocoder == VOC_NONE)
   {
      IVOC_LOCK();
      gMvsStatus = IVOC_MVS_SHUT;
      IVOC_UNLOCK();

      ShutDownVocoder();
      return(SUCCESS);
   }

   if(!mvs_mode_supported(gVocoders[config.vocoder]))
   {
      FARF(VOC_STATUS,("Cap not support."));
      return( AEE_VOC_ENOT_AVAIL );
   }

   // these vocoders are not supported. Make sure.
   if(VOC_IS96 == config.vocoder ||
      VOC_IS96A == config.vocoder ||
      VOC_SMV == config.vocoder)
   {
      FARF(VOC_STATUS,("Cap not support."));
      return( AEE_VOC_ENOT_AVAIL );
   }

   //Validate the Rate
   if (((config.vocoder == VOC_IS127)  ||
        (config.vocoder == VOC_4GV_NB) ||
        (config.vocoder == VOC_IS733)) &&
       ((config.min < EIGHTH_RATE)     ||
        (config.min >  config.max)     ||
        (config.max < EIGHTH_RATE)     ||
        (config.max > FULL_RATE)))
   {
      FARF(VOC_STATUS,("RateOutOfRange"));
      return( AEE_VOC_ERATE_OUT_OF_RANGE );
   }
   else if(config.vocoder == VOC_AMR)
   {
      if(config.max >= MVS_AMR_MODE_MAX)
      {
         FARF(VOC_STATUS,("RateOutOfRange"));
         return( AEE_VOC_ERATE_OUT_OF_RANGE );
      }
   }
   else if(config.vocoder == VOC_G711)
   {
      if((config.max < IVOC_G711_MODE_MULAW) ||
         (config.max >= IVOC_G711_MODE_MAX))
      {
         FARF(VOC_STATUS,("RateOutOfRange"));
         return( AEE_VOC_ERATE_OUT_OF_RANGE );
      }
   }

   //Validate txReduction
   if((config.txReduction > 0) &&
      ((config.vocoder == VOC_IS127) ||
      (config.vocoder == VOC_IS733)))
   {
      //{2,4,6,8} are the only acceptable non-zero values, and min/max must
      //be 1/8 and 1 respectively.
      if ((config.txReduction & 1) ||
          (config.txReduction > 8) ||
          (config.min != EIGHTH_RATE) ||
          (config.max != FULL_RATE))
      {
          FARF(VOC_STATUS,("BadTxRed"));
          return( AEE_VOC_EBAD_TX_REDUCTION );
      }
   }

   //Validate Watermark. Must have a positive value for IVOCODER to operate
   if (config.watermark == 0)
   {
      FARF(VOC_STATUS,("Zero Watermark"));
      return( EBADPARM );
   }

   if((VOC_IS127 == config.vocoder) ||
      (VOC_IS733 == config.vocoder))
   {
#ifdef FEATURE_VOC_PACKET_INTERFACE
      newLen = RateToLength[config.vocoder][config.max];
#endif
   }
   else if(VOC_AMR == config.vocoder)
   {
#ifdef FEATURE_AMR_VOCODER
      newLen = mvs_amr_frame_len(MVS_AMR_SPEECH_GOOD,
                                (mvs_amr_mode_type)config.max);
#endif
   }
   else if(VOC_G711 == config.vocoder)
   {
#ifdef FEATURE_VOC_G711
      newLen = MVS_VOC_G711_PKT_SIZE;
#endif
   }
   else if (VOC_4GV_NB == config.vocoder)
   {
#ifdef FEATURE_VOC_4GV
      // 4GV vocoder frame lengths are same as EVRC. Reusing the
      // RateToLength table.
      newLen = RateToLength[VOC_IS127][config.max];
#endif
   }
   else
   {
#ifdef FEATURE_VOC_PCM_INTERFACE
      newLen = PCM_FRAME_LENGTH;
#endif
   }

   //Shutdown the Vocoder and re-config so that the new configuration takes effect.
   if(gpVocInUse)
   {
      IVOC_LOCK();
      gMvsStatus = IVOC_MVS_SHUT;
      IVOC_UNLOCK();
      ShutDownVocoder();
   }
   else
   {
      IVOC_LOCK();
      gMvsStatus = IVOC_MVS_OFF;
      IVOC_UNLOCK();
   }

   //If the q items have not been created yet, or if the number and size do not
   //match, delete the existing ones, if any, and re-create them.
   //If frame max lengths differ, only re-allocate if newLen is larger.
   if (!OEMVocoder_InitPlayData(config.watermark, newLen))
   {
      FARF(VOC_STATUS,("NoMem"));
      return(ENOMEMORY);
   }

   IVOC_LOCK();

   pV->m_nWatermark = config.watermark;
   pV->m_eVocoder = config.vocoder;
   pV->m_eMin = config.min;
   pV->m_eMax = config.max;
   pV->m_nTxReduction = config.txReduction;
   pV->m_bOverwrite  = config.overwrite;

   if(VOC_IS127 == config.vocoder ||
      VOC_IS733 == config.vocoder)
   {
#ifdef FEATURE_VOC_PACKET_INTERFACE
      gErasure.length = RateToLength[config.vocoder][ERASURE];
      gErasure.frameInfo.voc_rate.rx_info.rate = MVS_VOC_ERASURE;
      gErasure.data = erasure_bits;
#endif
   }
   else
   {
      gErasure.length = 0;
   }

   gpVocInUse = pV;
   gDecStat = STOPPED;
   gPlay.operating = FALSE;

   if(gMvsStatus != IVOC_MVS_OFF)
   {
      // The VocStateCB has not yet fired to change state of
      // vocoder to mode ready after shutdown.
      FARF(VOC_STATUS,("changing gMvsStatus to S&A"));
      gMvsStatus = IVOC_MVS_SHUT_AND_ACQUIRE;
      IVOC_UNLOCK();
   }
   else
   {
      FARF(VOC_STATUS,("changing gMvsStatus to Acq"));
      gMvsStatus = IVOC_MVS_ACQUIRE;
      IVOC_UNLOCK();

      mvs_acquire(MVS_CLIENT_QCHAT, VocStateCB);
   }


   if (pInfo)
   {
      pInfo->frameDuration = FRAME_DURATION;
      pInfo->maxFrameSize = newLen;
   }

   FARF(VOC_STATUS,("OEMVoc_Config.Success"));
   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InStart

==============================================================================*/
int16 OEMVocoder_InStart(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   FARF(VOC_STATUS,("OEMVoco_InStart"));

   CHECKUSAGE(pV);

#if defined (FEATURE_ACM)
   // Acquire resources with ACM, and handle remaining work there
   return OEMVocoder_ACMAcquire (&pV->m_pAcmInfo, ACM_MOBILE_ORIGINATED_POC_CALL);

#else   // !FEATURE_ACM

   IVOC_LOCK();

   if(!gAcquire.operating)
   {
      gAcquire.callApp = FALSE;
      gAcquire.operating = TRUE;
   }

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("End OEMVoco_InStart"));
   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
#endif // FEATURE_ACM
}

/*===========================================================================

   Function: OEMVocoder_InStop

==============================================================================*/
int16 OEMVocoder_InStop(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   FARF(VOC_STATUS,("OEMVoco_InStop"));

   CHECKUSAGE(pV);

   IVOC_LOCK();

   gAcquire.operating = FALSE;
   gAcquire.callApp = TRUE;

   IVOC_UNLOCK();


#if defined (FEATURE_ACM)
   OEMVocoder_ACMFree (&pV->m_pAcmInfo);
#endif // FEATURE_ACM

   FARF(VOC_STATUS,("End OEMVoco_InStop"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InReset

==============================================================================*/
int16 OEMVocoder_InReset(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   QFrame * pQF;
   boolean prev = gAcquire.operating;

   CHECKUSAGE(pV);

   FARF(VOC_STATUS,("OEMVoco_InReset"));

   IVOC_LOCK();

   //Have to stop the encoding process to flush the contents.
   gAcquire.operating = FALSE;
   gAcquire.callApp = FALSE;

   while ((pQF = (QFrame *)q_get(&gAcquire.inUseQ)) != NULL)
   {
      q_put(&gAcquire.freeQ, &pQF->link);
   }

   gAcquire.operating = prev;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("End OEMVoco_InReset"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutStart

==============================================================================*/
int16 OEMVocoder_OutStart(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   CHECKUSAGE(pV);

#if defined (FEATURE_ACM)
   // Acquire resources with ACM, and handle remaining work there
   return OEMVocoder_ACMAcquire (&pV->m_pAcmInfo, ACM_NETWORK_ORIGINATED_POC_CALL);

#else   // !FEATURE_ACM

   FARF(VOC_STATUS,("OEMVoco_OutStart"));

   IVOC_LOCK();

   if (gDecStat == STOPPED)
   {
      if (q_cnt(&gPlay.inUseQ) >= gpVocInUse->m_nWatermark)
      {
         gDecStat = PLAYING;
      }
      else
      {
      gDecStat = BUFFERING;
      }
   }

   gPlay.callApp = FALSE;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("EndOEMVoco_OutStart"));


   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
#endif // FEATURE_ACM
}

/*===========================================================================

   Function: OEMVocoder_OutStop

==============================================================================*/
int16 OEMVocoder_OutStop(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   FARF(VOC_STATUS,("OEMVoco_OutStop"));

   CHECKUSAGE(pV);

   IVOC_LOCK();

   gDecStat = STOPPED;
   gPlay.callApp = FALSE;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("EndOEMVoco_OutStop"));


#if defined (FEATURE_ACM)
   OEMVocoder_ACMFree (&pV->m_pAcmInfo);
#endif // FEATURE_ACM

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutReset

==============================================================================*/
int16 OEMVocoder_OutReset(OEMINSTANCE handle)
{
   QFrame * pQF;
   boolean prev = gPlay.operating;
   OEMVocoder *pV = (OEMVocoder *)handle;

   FARF(VOC_STATUS,("OEMVoco_OutReset"));

   CHECKUSAGE(pV);

   IVOC_LOCK();

   //Have to stop the decoding process to flush the contents.
   gPlay.operating = FALSE;
   gPlay.callApp = FALSE;

   while ((pQF = (QFrame *)q_get(&gPlay.inUseQ)) != NULL)
   {
      q_put(&gPlay.freeQ, &pQF->link);
   }

   //If the vocoder was playing, it now has nothing to play, so go back to
   //buffering.
   if (gDecStat == PLAYING)
   {
      gDecStat = BUFFERING;
   }

   gPlay.operating = prev;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("EndOEMVoco_OutReset"));


   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InRead

==============================================================================*/
int16 OEMVocoder_InRead(OEMINSTANCE handle,
                           DataRateType * pRate,
                           uint16 * pLength,
                           uint8 * pData)
{
   QFrame * pQF;

   OEMVocoder *pV = (OEMVocoder *)handle;
   uint16 nLen;

   FARF(VOC_STATUS,("OEMVoco_InRead"));

   CHECKUSAGE(pV);

   pQF = (QFrame *)q_get(&gAcquire.inUseQ);

   if (!pQF)
   {
      FARF(VOC_STATUS,("Buffer Empty"));
      return(AEE_VOC_EBUFFER_EMPTY);
   }

   if (pRate)
   {
      if(VOC_AMR == pQF->vocoder)
      {
#ifdef FEATURE_AMR_VOCODER
         *pRate = (DataRateType)pQF->frameInfo.amr_rate.ul_info.mode;
#endif
      }
      else if(VOC_PCM == pQF->vocoder)
      {
#ifdef FEATURE_VOC_PCM_INTERFACE
         *pRate = 0;
#endif
      }
      else if(VOC_G711 == pQF->vocoder)
      {
#ifdef FEATURE_VOC_G711
         *pRate = pQF->frameInfo.g711_rate.ul_info.g711_mode;
#endif
      }
      else if((VOC_IS733 == pQF->vocoder)  ||
              (VOC_IS127 == pQF->vocoder))
      {
#ifdef FEATURE_VOC_PACKET_INTERFACE
         *pRate = pQF->frameInfo.voc_rate.tx_info.rate;
#endif
      }
      else if (VOC_4GV_NB == pQF->vocoder)
      {
#ifdef FEATURE_VOC_4GV
         *pRate = pQF->frameInfo.voc_rate.tx_info.rate;
#endif
      }
      else
      {
         *pRate = ERASURE;
      }
   }

   nLen = *pLength;
   *pLength = pQF->length;

   if(nLen > pQF->length)
      nLen = pQF->length;

   if (pData)
   {
      MEMMOVE(pData,pQF->data,nLen);
   }

   q_put(&gAcquire.freeQ, &pQF->link);

   FARF(VOC_STATUS,("EndOEMVoco_InRead"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutWrite

==============================================================================*/
int16 OEMVocoder_OutWrite(OEMINSTANCE handle,
                                    DataRateType rate,
                                    uint16 length,
                                    const uint8 * pData)
{
   QFrame * pQF;
   int16 status = SUCCESS;
   OEMVocoder *pV = (OEMVocoder *)handle;
   uint16   calculatedLength = 0;

   CHECKUSAGE(pV);

   FARF(VOC_STATUS,("OEMVoco_OutWrite"));
   if(VOC_AMR == pV->m_eVocoder)
   {
#ifdef FEATURE_AMR_VOCODER
      calculatedLength = mvs_amr_frame_len((mvs_amr_frame_type)MVS_AMR_SPEECH_GOOD,
                                           (mvs_amr_mode_type)rate);
#endif
   }
   else if(pV->m_eVocoder == VOC_PCM)
   {
#if defined(FEATURE_VOC_PCM_INTERFACE)
      calculatedLength = PCM_FRAME_LENGTH;
#endif
   }
   else if(VOC_G711 == pV->m_eVocoder)
   {
#ifdef FEATURE_VOC_G711
      calculatedLength = MVS_VOC_G711_PKT_SIZE;
#endif
   }
   else if(VOC_IS127 == pV->m_eVocoder ||
           VOC_IS733 == pV->m_eVocoder)
   {
#ifdef FEATURE_VOC_PACKET_INTERFACE
      calculatedLength = RateToLength[pV->m_eVocoder][rate];
#endif
   }
   else if (VOC_4GV_NB == pV->m_eVocoder)
   {
#ifdef FEATURE_VOC_4GV
      // 4GV frame lengths are same as EVRC.
      calculatedLength = RateToLength[VOC_IS127][rate];
#endif
   }

   if ( ((rate == ERASURE) &&
         ((pV->m_eVocoder == VOC_IS127) ||
         (pV->m_eVocoder == VOC_IS733))) ||
        ((length == calculatedLength) && pData))
   {
      pQF = (QFrame *)q_get(&gPlay.freeQ);

      if (!pQF)
      {
         status = AEE_VOC_EBUFFER_FULL;
      }
      else
      {
         pQF->vocoder = pV->m_eVocoder;
         pQF->length = length;

         if(VOC_AMR == pV->m_eVocoder)
         {
#ifdef FEATURE_AMR_VOCODER
            pQF->frameInfo.amr_rate.dl_info.mode  = (mvs_amr_mode_type)rate;
            pQF->frameInfo.amr_rate.dl_info.frame = MVS_AMR_SPEECH_GOOD;
#endif
         }
         else if(VOC_PCM == pV->m_eVocoder)
         {
#if defined(FEATURE_VOC_PCM_INTERFACE)
            pQF->frameInfo.pcm_rate.dl_info.info = 0; // for now.
#endif
         }
         else if(VOC_G711 == pV->m_eVocoder)
         {
#ifdef FEATURE_VOC_G711
            pQF->frameInfo.g711_rate.dl_info.g711_mode = (mvs_g711_mode_type)rate;
#endif
         }
         else if ((VOC_IS733 == pV->m_eVocoder) ||
                  (VOC_IS127 == pV->m_eVocoder))
         {
#ifdef FEATURE_VOC_PACKET_INTERFACE
            pQF->frameInfo.voc_rate.rx_info.rate = (mvs_voc_rate_type)rate;
#endif
         }
         else if (VOC_4GV_NB == pV->m_eVocoder)
         {
#ifdef FEATURE_VOC_4GV
            pQF->frameInfo.voc_rate.rx_info.rate = (mvs_voc_rate_type)rate;
#endif
         }
         else
         {
            rate = ERASURE;
         }

         if ((rate == ERASURE) && (pV->m_eVocoder != VOC_AMR) &&
             (pV->m_eVocoder != VOC_PCM) && (pV->m_eVocoder != VOC_G711))
         {
            MEMCPY(pQF->data,gErasure.data,gErasure.length);
         }
         else
         {
            MEMMOVE(pQF->data,pData,length);
         }

         q_put(&gPlay.inUseQ,&pQF->link);

         IVOC_LOCK();

         if (!gPlay.operating)
         {
            if ((gDecStat == BUFFERING) &&
                (q_cnt(&gPlay.inUseQ) >= gpVocInUse->m_nWatermark))
            {
               gDecStat = PLAYING;
               FARF(VOC_STATUS,("TURN PLAY ON"));
               gPlay.operating = TRUE;
            }
            else if (gDecStat == STOPPED)
            {
               status = AEE_VOC_ENOT_STARTED;
            }
         }
         IVOC_UNLOCK();
      }
   }
   else
   {
      FARF(VOC_STATUS,("Bad Frame: l=%d, r=%d",length, rate, 0));
      status = EBADPARM;
   }

   FARF(VOC_STATUS,("EndOEMVoco_OutWrite %d",status));
   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(status);
}

/*===========================================================================

   Function: OEMVocoder_GetInDepth

==============================================================================*/
int16 OEMVocoder_GetInDepth(OEMINSTANCE handle,
                            uint16 * pDepth)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   CHECKUSAGE(pV);

   if (!pDepth)
   {
      return(EBADPARM);
   }

   *pDepth = (uint16)q_cnt(&gAcquire.inUseQ);

   FARF(VOC_STATUS,("EndOEMVoco_InDepth"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_GetOutDepth

==============================================================================*/
int16 OEMVocoder_GetOutDepth(OEMINSTANCE handle,
                             uint16 * pDepth)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   CHECKUSAGE(pV);

   if (!pDepth)
   {
      return(EBADPARM);
   }

   *pDepth = (uint16)q_cnt(&gPlay.inUseQ);

   FARF(VOC_STATUS,("EndOEMVoco_OutDepth"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_GetFrameLength

==============================================================================*/
int16 OEMVocoder_GetFrameLength(OEMINSTANCE handle,
                                VocoderType vocoder,
                                DataRateType rate,
                                uint16 * pLength)
{
   int16 retVal = EBADPARM;
   OEMVocoder *pV = (OEMVocoder *)handle;

   if(!pV)
   {
      return(EBADPARM);
   }

   if(pLength && (VOC_AMR == vocoder))
   {
      if((rate >= AMR_MODE_0475) && (rate < AMR_MODE_MAX))
      {
#ifdef FEATURE_AMR_VOCODER
         *pLength = mvs_amr_frame_len(MVS_AMR_SPEECH_GOOD, rate);
         retVal = SUCCESS;
#endif
      }
   }
   else if(pLength && (VOC_PCM == vocoder))
   {
#if defined(FEATURE_VOC_PCM_INTERFACE)
      *pLength = PCM_FRAME_LENGTH;
      retVal = SUCCESS;
#endif
   }
   else if(pLength && (VOC_G711 == vocoder))
   {
#ifdef FEATURE_VOC_G711
      *pLength = MVS_VOC_G711_PKT_SIZE;
      retVal = SUCCESS;
#endif
   }
   else if (pLength && (VOC_4GV_NB == vocoder))
   {
#ifdef FEATURE_VOC_4GV
      // 4GV frame lengths are same as EVRC.
      *pLength = RateToLength[VOC_IS127][rate];
      retVal = SUCCESS;
#endif
   }
   else if(pLength && (vocoder > VOC_NONE) &&
       (vocoder < VOC_MAX)  &&
       (rate >= BLANK_RATE) &&
       (rate < MAX_RATE))
   {
#ifdef FEATURE_VOC_PACKET_INTERFACE
      *pLength = RateToLength[vocoder][rate];
      retVal = SUCCESS;
#endif
   }

   FARF(VOC_STATUS,("EndOEMVoco_GetFramLen"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(retVal);
}

/*===========================================================================

   Function: OEMVocoder_PlayTone

==============================================================================*/
int16 OEMVocoder_PlayTone(OEMINSTANCE handle,
                          uint16 wTone1,
                          uint16 wTone2,
                          int16  wVolume,
                          uint16 wDuration)
{

   int16 status = SUCCESS;
   OEMVocoder *pV = (OEMVocoder *)handle;

   CHECKUSAGE(pV);

   IVOC_LOCK();

   status = ((gPlay.operating || gAcquire.operating) ? AEE_VOC_EACTIVE : SUCCESS);

   IVOC_UNLOCK();

   if(status == SUCCESS)
   {
      snd_set_volume((uint16) SND_DEVICE_HANDSET,
         (uint16) SND_METHOD_VOICE, (uint8)wVolume,
         NULL, NULL);

      if(wTone1 > wTone2) {
         snd_freq_tone_start( (uint16) SND_DEVICE_HANDSET,
            (uint16) SND_METHOD_VOICE,
            wTone1, wTone2, wDuration, AEE_SOUND_APATH_LOCAL,
            NULL, NULL);
      }
      else {
         snd_freq_tone_start( (uint16) SND_DEVICE_HANDSET,
            (uint16) SND_METHOD_VOICE,
            wTone2, wTone1, wDuration, AEE_SOUND_APATH_LOCAL,
            NULL, NULL);
      }
   }

   FARF(VOC_STATUS,("EndOEMVoco_PlayTone"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(status);
}

/*===========================================================================
   Registration Function for CBs
===========================================================================*/
int   OEMVocoder_RegisterCB(AEECallback *pcbReady,
                            AEECallback *pcbHaveData,
                            AEECallback *pcbNeedData,
                            AEECallback *pcbPlayedData)
{
   IVOC_LOCK();

   gpcbReady = pcbReady;
   gpcbHaveData = pcbHaveData;
   gpcbNeedData = pcbNeedData;
   gpcbPlayedData = pcbPlayedData;

   IVOC_UNLOCK();

   return(SUCCESS);
}

/*===========================================================================
   Get number of frames available
===========================================================================*/
uint16 OEMVocoder_GetDataAvailCount(void)
{
   uint16 nCount = (uint16)q_cnt(&gAcquire.inUseQ);

   FARF(VOC_STATUS,("AvailCount %d",nCount));

   return(nCount);
}

/*===========================================================================
   Get number of frames played
===========================================================================*/
uint16 OEMVocoder_GetDataPlayedCount(void)
{
   uint16 nCount;

   IVOC_LOCK();

   nCount= gPlayedCount;
   gPlayedCount = 0;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("PlayedCount %d",nCount));

   return(nCount);
}

/*===========================================================================
   Get number of Erasures Played
===========================================================================*/
uint16 OEMVocoder_GetErasureCount(void)
{
   uint16 nCount;

   IVOC_LOCK();

   nCount = gErasureCount;
   gErasureCount = 0;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("ErasureCount %d",nCount));

   return(nCount);
}


/*===========================================================================
   Free Memory and set it to NULL
===========================================================================*/
static void FreeIfUsed(void **ppObj)
{
   if (ppObj)
   {
      if (*ppObj)
      {
         sys_free(*ppObj);
         *ppObj = NULL;
      }
   }
}

/*===========================================================================
  Initialize the Play Data
===========================================================================*/
static boolean OEMVocoder_InitPlayData(uint32 qSize,uint16 frameLen)
{
   boolean retVal = FALSE;

   FARF(VOC_STATUS,("Begin InitPlayData"));

   IVOC_LOCK();

   OEMVocoder_ReleasePlayData();

   //Create the q items and put them all on the free q.
   (void)q_init(&gPlay.freeQ);
   (void)q_init(&gPlay.inUseQ);
   (void)q_init(&gAcquire.freeQ);
   (void)q_init(&gAcquire.inUseQ);

   gPlay.frames = (QFrame*)sys_malloc(qSize * sizeof(QFrame));
   gPlay.data = (uint8 *)sys_malloc(qSize  * frameLen);
   gAcquire.frames = (QFrame*)sys_malloc(qSize * sizeof(QFrame));
   gAcquire.data = (uint8 *)sys_malloc(qSize * frameLen);

   //Check to see if all memory has been allocated succesfully.
   if (gPlay.frames && gPlay.data && gAcquire.frames && gAcquire.data)
   {
      uint32 i;
      uint8 * acquireIndex = gAcquire.data;
      uint8 * playIndex = gPlay.data;

      //Point the data fields in the QFrames to a block in memory
      for (i = 0; i < qSize; i++)
      {
         gPlay.frames[i].data = playIndex;
         gAcquire.frames[i].data = acquireIndex;

         playIndex += frameLen;
         acquireIndex += frameLen;

         //Put the newly created frames onto the respective free queues.
         q_put(&gPlay.freeQ, q_link(&gPlay.frames[i], &gPlay.frames[i].link));
         q_put(&gAcquire.freeQ, q_link(&gAcquire.frames[i],
                                       &gAcquire.frames[i].link));
      }
      retVal = TRUE;
   }
   else
   {
      OEMVocoder_ReleasePlayData();
   }

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("InitPlayData %d",retVal));
   return(retVal);
}

/*===========================================================================
 Cleanup Play Data. It is assumed that the caller has done INTLOCK
===========================================================================*/
static void OEMVocoder_ReleasePlayData(void)
{
   gAcquire.callApp = TRUE;
   gAcquire.operating = FALSE;
   gPlay.callApp = TRUE;
   gPlay.operating = FALSE;
   FreeIfUsed((void **)(&gPlay.data));
   FreeIfUsed((void **)(&gPlay.frames));
   FreeIfUsed((void **)(&gAcquire.data));
   FreeIfUsed((void **)(&gAcquire.frames));
}

/*===========================================================================

Function: VocStateCB

Description:
This function is called by the vocoder task whenever the vocoder changes
state.  When the vocoder is in a state where it will accept rate cmds,
set the rates. When the mode is ready, inform the user

==============================================================================*/
static void   VocStateCB(mvs_event_type *event)
{
   uint32 mvsMode = MVS_MODE_NONE;

   FARF(VOC_STATUS,("InVocStaeCB"));

   if (NULL == event)
   {
      FARF(VOC_STATUS,("NULL mvs_event_type pointer"));
      return;
   }

   switch(event->hdr.event) {
    case MVS_EV_COMMAND:
       if(event->cmd.cmd_status == MVS_CMD_SUCCESS)
       {
          FARF(VOC_STATUS,("Command Success %d", gMvsStatus));

          IVOC_LOCK();
          if (!gpVocInUse || (IVOC_MVS_ACQUIRE != gMvsStatus))
          {
             IVOC_UNLOCK();
             return;
          }

          gMvsStatus = IVOC_MVS_MODE_ENABLE;

          if(VOC_IS733 == gpVocInUse->m_eVocoder ||
             VOC_IS127 == gpVocInUse->m_eVocoder)
          {
#ifdef FEATURE_VOC_PACKET_INTERFACE
             FARF(VOC_STATUS,("Acquired MVS - voc"));
 #ifdef FEATURE_MVS_RAPI
             mvs_voc_set_frame_rate((mvs_voc_rate_type)gpVocInUse->m_eMax,
                                       (mvs_voc_rate_type)gpVocInUse->m_eMin);

             mvs_voc_tx_rate_limit(gpVocInUse->m_nTxReduction);
#else
             mvssup_voc_set_frame_rate((mvs_voc_rate_type)gpVocInUse->m_eMax,
                                       (mvs_voc_rate_type)gpVocInUse->m_eMin);

             mvssup_voc_tx_rate_limit(gpVocInUse->m_nTxReduction);
#endif
#endif
          }
          else if(VOC_4GV_NB == gpVocInUse->m_eVocoder)
          {
#ifdef FEATURE_VOC_4GV
             FARF(VOC_STATUS,("Acquired MVS - 4gv"));
 #ifdef FEATURE_MVS_RAPI
             mvs_voc_set_frame_rate((mvs_voc_rate_type)gpVocInUse->m_eMax,
                (mvs_voc_rate_type)gpVocInUse->m_eMin);
#else
             mvssup_voc_set_frame_rate((mvs_voc_rate_type)gpVocInUse->m_eMax,
                (mvs_voc_rate_type)gpVocInUse->m_eMin);
#endif
#endif
          }
          else if(VOC_AMR == gpVocInUse->m_eVocoder)
          {
#ifdef FEATURE_AMR_VOCODER
             FARF(VOC_STATUS,("Acquired MVS - amr"));
#if defined(T_MSM6280)
#ifdef FEATURE_MVS_RAPI
             mvs_amr_set_amr_mode((mvs_amr_mode_type)gpVocInUse->m_eMax);
#else
             mvssup_amr_set_amr_mode((mvs_amr_mode_type)gpVocInUse->m_eMax);
#endif
#endif
             if(gpVocInUse->m_bOverwrite & IVOC_AMR_SCRMODE)
             {
#if defined(T_MSM6280)
#ifdef FEATURE_MVS_RAPI
                mvs_amr_set_scr_mode(MVS_SCR_ENABLE);
#else
                mvssup_amr_set_scr_mode(MVS_SCR_ENABLE);
#endif
#endif
             }
             else
             {
#if defined(T_MSM6280)
#ifdef FEATURE_MVS_RAPI
                mvs_amr_set_scr_mode(MVS_SCR_DISABLE);
#else
                mvssup_amr_set_scr_mode(MVS_SCR_DISABLE);
#endif
#endif
             }

             if(gpVocInUse->m_bOverwrite & IVOC_AMR_DTXMODE)
             {
#if defined(T_MSM6280)
#ifdef FEATURE_MVS_RAPI
                mvs_gsm_set_dtx_mode(MVS_DTX_ON);
#else
                mvssup_gsm_set_dtx_mode(MVS_DTX_ON);
#endif
#endif
             }
             else
             {
#if !defined(T_MSM6280)
#ifdef FEATURE_MVS_RAPI
                mvs_gsm_set_dtx_mode(MVS_DTX_OFF);
#else
                mvssup_gsm_set_dtx_mode(MVS_DTX_OFF);
#endif
#endif
             }
#endif
          }
          else if(VOC_PCM == gpVocInUse->m_eVocoder)
          {
             // nothing to do
          }
          else if(VOC_G711 == gpVocInUse->m_eVocoder)
          {
#ifdef FEATURE_VOC_G711
#if !defined(T_MSM6280)
#ifdef FEATURE_MVS_RAPI
           mvs_set_g711_mode((mvs_g711_mode_type)gpVocInUse->m_eMax);
#else
            mvssup_set_g711_mode((mvs_g711_mode_type)gpVocInUse->m_eMax);
#endif
#endif
#endif
          }
          else
          {
             FARF(VOC_STATUS,("Unsupported Vocoder"));
             IVOC_UNLOCK();
             return;
          }

          mvsMode = gVocoders[gpVocInUse->m_eVocoder];
          IVOC_UNLOCK();

          mvs_enable(MVS_CLIENT_QCHAT, mvsMode, VocoderFrameAcquiredCB,
                     VocoderPlayoutFrameCB, MVS_PKT_CONTEXT_VOC);

       }
       else if(event->cmd.cmd_status == MVS_CMD_FAILURE)
       {
          FARF(VOC_STATUS,("MVS command failed"));

          IVOC_LOCK();
          gMvsStatus = IVOC_MVS_FAILED;
          IVOC_UNLOCK();
       }
       else if(event->cmd.cmd_status == MVS_CMD_BUSY)
       {
          FARF(VOC_STATUS,("MVS busy, try later"));
          IVOC_LOCK();
          gMvsStatus = IVOC_MVS_FAILED;
          IVOC_UNLOCK();
       }

       break;

    case MVS_EV_MODE:

          if(event->mode.mode_status == MVS_MODE_INIT)
          {
             FARF(VOC_STATUS,("Mode initializing %d", gMvsStatus));
          }
          else if(event->mode.mode_status == MVS_MODE_READY)
          {
             FARF(VOC_STATUS,("Mode Ready %d", gMvsStatus));

             IVOC_LOCK();

             if(IVOC_MVS_MODE_ENABLE == gMvsStatus)
             {
                gMvsStatus = IVOC_MVS_ON;
             }
             else if(IVOC_MVS_SHUT == gMvsStatus)
             {
                gMvsStatus = IVOC_MVS_OFF;
             }
             else if(IVOC_MVS_SHUT_AND_ACQUIRE == gMvsStatus)
             {
                gMvsStatus = IVOC_MVS_ACQUIRE;
             }

             if(IVOC_MVS_ON == gMvsStatus)
             {
                //Call the client

                if(gpVocInUse && gpcbReady)
#if MIN_BREW_VERSION(3,1)
                AEE_SYS_RESUME(gpcbReady);
#else
                AEE_SYS_RESUME(gpcbReady,0);
#endif
             }
             else if(IVOC_MVS_ACQUIRE == gMvsStatus)
             {
                IVOC_UNLOCK();
                mvs_acquire(MVS_CLIENT_QCHAT, VocStateCB);
                return;
             }

             IVOC_UNLOCK();
          }
          else if(event->mode.mode_status == MVS_MODE_NOT_AVAIL)
          {
             FARF(VOC_STATUS,("Mode not available %d", gMvsStatus));
             IVOC_LOCK();
             if(IVOC_MVS_MODE_ENABLE == gMvsStatus)
             {
                gMvsStatus = IVOC_MVS_FAILED;
             }
             IVOC_UNLOCK();
          }

       break;

    default:
       FARF(VOC_STATUS,("Unknown event"));
       break;
   }
}

/*===========================================================================

Function: OEMVocoder_TaskCallbackAcquire

==============================================================================*/
void OEMVocoder_TaskCallbackAcquire(void)
{
   if (gpVocInUse && gpcbHaveData)
   {
      FARF(VOC_STATUS,("setting have-data 1"));
#if MIN_BREW_VERSION(3,1)
      AEE_SYS_RESUME(gpcbHaveData);
#else
      AEE_SYS_RESUME(gpcbHaveData,0);
#endif
   }
}

/*===========================================================================

Function: ShutDownVocoder

Description:
     This function will perform all necessary steps to completely shut down
     the vocoder, and release any data associated with using the vocoder.

==============================================================================*/
void ShutDownVocoder(void)
{
   FARF(VOC_STATUS,("OEMVoc_ShutDownVoc"));

   IVOC_LOCK();

   gAcquire.callApp = FALSE;
   gAcquire.operating = FALSE;
   gPlay.callApp = FALSE;
   gPlay.operating = FALSE;

   //Indicate that no instance of IVocoder is currently using the vocoder.
   gpVocInUse = NULL;

   //Cancel any pending callbacks
   CALLBACK_Cancel(gpcbReady);
   CALLBACK_Cancel(gpcbHaveData);
   CALLBACK_Cancel(gpcbNeedData);
   CALLBACK_Cancel(gpcbPlayedData);

   OEMVocoder_ReleasePlayData();

   gMvsStatus = IVOC_MVS_SHUT;

   IVOC_UNLOCK();

   mvs_release(MVS_CLIENT_QCHAT);

   FARF(VOC_STATUS,("End OEMShutDownVoc"));
}


/*===========================================================================

Function: VocoderFrameAcquiredCB

Description:
This function is called by the vocoder task when there are uplink
data packets to be transferred from vocoder to user.

==============================================================================*/

static void VocoderFrameAcquiredCB(uint8 *data,
                                   mvs_frame_info_type *frame_info,
                                   uint16 len,
                                   mvs_pkt_status_type *status)
{
   QFrame *pQF = NULL;

   FARF(VOC_STATUS,("ACQ CB"));

   if(NULL == frame_info || NULL == data || 0 == len)
   {
      FARF(VOC_STATUS,("AcquireCB - error - null ptr"));
      if(NULL != status)
      {
         *status = MVS_PKT_STATUS_FAST;
      }

      return;
   }

   *status = MVS_PKT_STATUS_FAST;

   IVOC_LOCK();

   if (gpVocInUse && gAcquire.operating)
   {
      // Grab an empty frame buffer
      pQF = (QFrame *)q_get(&gAcquire.freeQ);
      *status = MVS_PKT_STATUS_NORMAL;

      if (!pQF && (gpVocInUse->m_bOverwrite & TRUE))
      {
         //If the application has opted to overwrite frames when the encoding
         //buffer fills, pull an already encoded frame and re-queue it with
         //the new data.  This effectively erases the front end of the queue.
         pQF = (QFrame *)q_get(&gAcquire.inUseQ);

         *status = MVS_PKT_STATUS_FAST; /* packet loss */
         if (!pQF)
         {
            FARF(VOC_STATUS,("Voc:Problems with Queues"));
            IVOC_UNLOCK();
            return;
         }
      }

      if (pQF)
      {
         // Copy the frame information and bits
         pQF->vocoder = gpVocInUse->m_eVocoder;

         if(VOC_AMR == pQF->vocoder)
         {
#ifdef FEATURE_AMR_VOCODER
            pQF->frameInfo.amr_rate.ul_info.mode =
                                     frame_info->amr_rate.ul_info.mode;
            pQF->frameInfo.amr_rate.ul_info.frame =
                                     frame_info->amr_rate.ul_info.frame;

            pQF->length = mvs_amr_frame_len(
                                    frame_info->amr_rate.ul_info.frame,
                                    frame_info->amr_rate.ul_info.mode);

            FARF(VOC_STATUS,("ACQ CB l=%d,dl=%d,r=%d", len,pQF->length,pQF->frameInfo.amr_rate.ul_info.mode));
#endif
         }
         else if((VOC_IS733 == pQF->vocoder) ||
                 (VOC_IS127 == pQF->vocoder))
         {
#ifdef FEATURE_VOC_PACKET_INTERFACE
            pQF->frameInfo.voc_rate.tx_info.rate = frame_info->voc_rate.tx_info.rate;

            //The len parameter given to us by dmss can be incorrect, so don't
            //use it.  If a frame type has an odd length, dmss rounds it up to
            //an even length.

            pQF->length = RateToLength[pQF->vocoder][pQF->frameInfo.voc_rate.tx_info.rate];

            FARF(VOC_STATUS,("ACQ CB l=%d,dl=%d,r=%d", len,pQF->length,pQF->frameInfo.voc_rate.tx_info.rate));
#endif
         }
         else if(VOC_4GV_NB == pQF->vocoder)
         {
#ifdef FEATURE_VOC_4GV
            pQF->frameInfo.voc_rate.tx_info.rate = frame_info->voc_rate.tx_info.rate;
            pQF->length = len;

            FARF(VOC_STATUS,("ACQ CB l=%d,dl=%d,r=%d", len,pQF->length,pQF->frameInfo.voc_rate.tx_info.rate));
#endif
         }
         else if(VOC_PCM == pQF->vocoder)
         {
#if defined(FEATURE_VOC_PCM_INTERFACE)
            // till ul_info is supplied, using a dummy value.

            pQF->frameInfo.pcm_rate.ul_info.info = 0;
            pQF->length = len;

            FARF(VOC_STATUS,("ACQ CB l=%d,dl=%d,r=%d", len,pQF->length,0));
#endif
         }
         else if(VOC_G711 == pQF->vocoder)
         {
#ifdef FEATURE_VOC_G711
            pQF->frameInfo.g711_rate.ul_info.g711_mode = frame_info->g711_rate.ul_info.g711_mode;
            pQF->length = len;
#endif
         }
         else
         {
            //unknown vocoder.
            FARF(VOC_STATUS,("unknown vocoder in Acquire CB"));
         }


         MEMCPY(pQF->data, data, pQF->length);

         // Give the frame to the application
         q_put(&gAcquire.inUseQ, &pQF->link);

         if (!gAcquire.callApp)
         {
            FARF(VOC_STATUS,("Calling OEMVocoder_TaskCallbackAcquire"));
            OEMVocoder_TaskCallbackAcquire();
         }
      }
      else
      {
         FARF(VOC_STATUS,("Voc:Acquire buffer full"));
         *status = MVS_PKT_STATUS_FAST;
         if (!gAcquire.callApp)
         {
            OEMVocoder_TaskCallbackAcquire();
         }
      }
   }

   IVOC_UNLOCK();
}

/*===========================================================================

Function: VocoderPlayoutFrameCB

Description:
This function is called by the vocoder task when it needs downlink
packets - from user into vocoder.

==============================================================================*/
static void VocoderPlayoutFrameCB(uint8 *data,
                                  mvs_frame_info_type *frame_info,
                                  mvs_pkt_status_type *status)
{
   QFrame * pQF = NULL;

   FARF(VOC_STATUS,("PLY CB"));

   IVOC_LOCK();

   if(!gPlay.operating)
   {
      *status = MVS_PKT_STATUS_SLOW;
      IVOC_UNLOCK();
      return;
   }

   IVOC_UNLOCK();

   if(NULL == data || NULL == frame_info)
   {
      FARF(VOC_STATUS,("Playout CB - null error"));

      if(NULL != status)
      {
         *status = MVS_PKT_STATUS_FAST; /* indicates program error */
      }
      return;
   }

   FARF(VOC_STATUS,("PLAY IS ON . . . ."));
   IVOC_LOCK();

   // Get a frame from the playout queue supplied by the application
   if ((pQF = (QFrame *)q_get(&gPlay.inUseQ)) != NULL)
   {
      gPlayedCount++;

      if(VOC_AMR == pQF->vocoder)
      {
#ifdef FEATURE_AMR_VOCODER
          MEMCPY(data, pQF->data, pQF->length);
          frame_info->amr_rate.dl_info.frame =
                       pQF->frameInfo.amr_rate.dl_info.frame;
          frame_info->amr_rate.dl_info.mode =
                       pQF->frameInfo.amr_rate.dl_info.mode;
          *status = MVS_PKT_STATUS_NORMAL;
#endif
      }
      else if((VOC_IS733 == pQF->vocoder) ||
              (VOC_IS127 == pQF->vocoder))
      {
#ifdef FEATURE_VOC_PACKET_INTERFACE
         MEMCPY(data, pQF->data, pQF->length);
         frame_info->voc_rate.rx_info.rate = pQF->frameInfo.voc_rate.rx_info.rate;
         *status = MVS_PKT_STATUS_NORMAL;
#endif
      }
      else if(VOC_4GV_NB == pQF->vocoder)
      {
#ifdef FEATURE_VOC_4GV
         MEMCPY(data, pQF->data, pQF->length);
         frame_info->voc_rate.rx_info.rate = pQF->frameInfo.voc_rate.rx_info.rate;
         *status = MVS_PKT_STATUS_NORMAL;
#endif
      }
      else if(VOC_PCM == pQF->vocoder)
      {
#if defined(FEATURE_VOC_PCM_INTERFACE)
         MEMCPY(data, pQF->data, pQF->length);
         frame_info->pcm_rate.dl_info.info = pQF->frameInfo.pcm_rate.dl_info.info;
         *status = MVS_PKT_STATUS_NORMAL;
#endif
      }
      else if(VOC_G711 == pQF->vocoder)
      {
#ifdef FEATURE_VOC_G711
         MEMCPY(data, pQF->data, pQF->length);
         frame_info->g711_rate.dl_info.g711_mode = pQF->frameInfo.g711_rate.dl_info.g711_mode;
         *status = MVS_PKT_STATUS_NORMAL;
#endif
      }
      else
      {
          //unknown vocoder type.
          *status = MVS_PKT_STATUS_FAST; /* indicates program error */
          FARF(VOC_STATUS,("unknown vocoder type in Playout CB"));
      }

      q_put(&gPlay.freeQ, &pQF->link);

      if (!gPlay.callApp && gpcbPlayedData)
      {
         //Call the client
#if MIN_BREW_VERSION(3,1)
         AEE_SYS_RESUME(gpcbPlayedData);
#else
         AEE_SYS_RESUME(gpcbPlayedData,0);
#endif
      }
   }
   else
   {
      //We have run out of frames to play, so suspend playout until more are
      //added. The decode state should have been 'playing', so go back to
      //buffering.

      gErasureCount++;
      gPlay.operating = FALSE;
      gDecStat = BUFFERING;

      *status = MVS_PKT_STATUS_SLOW;

      FARF(VOC_STATUS,("PLAY ERASE . . . ."));

      if (!gPlay.callApp && gpcbNeedData)
      {
         //Call the client
#if MIN_BREW_VERSION(3,1)
         AEE_SYS_RESUME(gpcbNeedData);
#else
         AEE_SYS_RESUME(gpcbNeedData,0);
#endif
      }
   }

   IVOC_UNLOCK();
}

/////////////////////////////////////////////////////////////////////////////////////
// old support for vocoder not using MVS service.
//
//
// Following code is meant to support OEM's with AMSS release
// which does not have MVS service.
//
/////////////////////////////////////////////////////////////////////////////////////

#elif !defined(FEATURE_MVS)

#include <stdlib.h>
#include "comdef.h"
#include "OEMFeatures.h"
#include "OEMVocoder.h"
#include "OEMHeap.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEEVocoder.h"
#include "AEEError.h"
#include "AEEStdLib.h"

#define FARF_VOC_STATUS        0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }


#if !defined(FEATURE_IVOCODER) || !defined(FEATURE_VOC_PACKET_INTERFACE)

// The feature is not available in this DMSS build

OEMINSTANCE OEMVocoder_New(AEECLSID classId, int *nErr)
{
   if(nErr)
      *nErr = ECLASSNOTSUPPORT;

   FARF(VOC_STATUS,("OEMVocoder_New. Feature Not Supported"));

   return(0);
}

#else

/* QCT DMSS Vocoder Frame Interface */

#include "voci.h"
#include "queue.h"
#include "voc.h"
#include "qdspcmd.h"
#include "rex.h"

/* The signal AEE_VOC_ISR_SIG must be defined and used. This signal is used to communicate
between the ISR and tha vocoder task. When the ISR is invoked, it sets this signal to inform
the vocoder task to invoke a function in the vocoder task context.

Please do the following steps to define and use the vocoder signal.
1. Define the signal in voci.h to a un-used signal value (ex: #define AEE_VOC_ISR_SIG 0x0200)

2. Add the following line to the sig mask that rex_wait(in the function voc_task) waits on:
AEE_VOC_ISR_SIG           |

3. Modify the function voc_task() in voctask.c to handle this signal. When this signal is set, the
function OEMVocoder_TaskCallback() must be invoked. Here is the sequence:
if ( (sigs & AEE_VOC_ISR_SIG) != 0 ) {
         (void) rex_clr_sigs( &voc_tcb, AEE_VOC_ISR_SIG );
         OEMVocoder_TaskCallback();
   }

4. Extern the function OEMVocoder_TaskCallback() in voctask.c
   extern void OEMVocoder_TaskCallback(void);

*/

#ifndef AEE_VOC_ISR_SIG

#error  Vocoder Signal AEE_VOC_ISR_SIG not defined. Please do the following steps to define and use the \
        vocoder signal. \
1. Define the signal in voci.h to a un-used signal value (ex: #define AEE_VOC_ISR_SIG 0x0200) \
2. Add the following line to the sig mask that rex_wait(in the function voc_task) waits on: \
AEE_VOC_ISR_SIG           | \
3. Modify the function voc_task() in voctask.c to handle this signal. When this signal is set, the \
function OEMVocoder_TaskCallback() must be invoked. Here is the sequence: \
if ( (sigs & AEE_VOC_ISR_SIG) != 0 ) { \
         (void) rex_clr_sigs( &voc_tcb, AEE_VOC_ISR_SIG ); \
         OEMVocoder_TaskCallback(); \
   }\
4. Extern the function OEMVocoder_TaskCallback() in voctask.c \
   extern void OEMVocoder_TaskCallback(void); \

#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

#define FRAME_DURATION  20

//This MACRO is used to check the input parameter, IVocoder *, for NULL,
//to insure that this instance has access to use the vocoder, and to check
//to see if the vocoder has been configured.
#define CHECKUSAGE(pV) \
   if (!pV) \
   { \
      return(EBADCLASS); \
   } \
   if (!gpVocInUse) \
   { \
      return(AEE_VOC_ENOT_CONFIGED); \
   } \
   if (gpVocInUse != pV) \
   { \
      return(AEE_VOC_EIN_USE); \
   } \

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

typedef struct _OEMVocoder       OEMVocoder;

typedef enum
{
   BUFFERING,
   PLAYING,
   STOPPED
} DecodeStatusType;

/* frame sizes for each vocoder type and rate */
static uint16 const RateToLength[VOC_MAX][MAX_RATE] = {
   {0,  0,  0,  0,  0,  2}, //VOC_NONE
   {0,  0,  0,  0,  0,  2}, //VOC_IS96
   {0,  0,  0,  0,  0,  2}, //VOC_IS96A
   {0,  3,  7, 16, 34,  2}, //VOC_IS733
   {0,  2,  5, 10, 22,  2}, //VOC_IS127
   {0,  2,  5, 10, 22,  2}, //VOC_SMV
};

static const voc_capability_type gVocoders[VOC_MAX] = {
   VOC_CAP_NONE,
   VOC_CAP_IS96,
   VOC_CAP_IS96A,
   VOC_CAP_IS733,
   VOC_CAP_IS127,
#ifdef FEATURE_SMV
   VOC_CAP_SMV
#endif /* FEATURE_SMV */
};

struct _OEMVocoder
{
   uint32             m_nWatermark;
   uint16             m_nTxReduction;
   VocoderType        m_eVocoder;
   DataRateType       m_eMin;
   DataRateType       m_eMax;
   boolean            m_bOverwrite;
};

typedef struct
{
   q_link_type link;    //DMSS queue type
   uint8 * data;        //pointer to data block storing frame data
   uint16 length;       //length of frame in bytes
   VocoderType vocoder; //vocoder used to encode/decode the frame
   voc_rate_type rate;  //rate of frame
} QFrame;

typedef struct
{
   q_type freeQ;        //Queue containing frame items not currently in use
   q_type inUseQ;       //Queue containing frame items currently in use
   QFrame * frames;     //Pointer to array of frame items. Used for storage only
   uint8 * data;        //Pointer to data block storing all frame data.
   boolean callApp;     //flag indicating if a user callback is needed.
   boolean operating;   //flag indicating if current process is operating
} QOperation;

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

// Internal functions
static void FreeIfUsed(void **ppObj);

static boolean OEMVocoder_InitPlayData(uint32 qSize,uint16 frameLen);
static void    OEMVocoder_ReleasePlayData(void);
static void    VocStateCB(voc_inform_type new_status );
static void    ShutDownVocoder(void);


void OEMVocoder_TaskCallback(void);


/* Reverse Packet Interface */
static void VocoderFrameAcquiredCB(uint8          *vocoder_packet,
                                   voc_rate_type  current_rate,
                                   uint16         packet_length);

/* Forward Packet Interface */
static void VocoderPlayoutFrameCB(uint8          **vocoder_packet,
                                  voc_rate_type  *rate);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/


static QOperation gAcquire = {0};         /* Struct used for encoding frames */
static QOperation gPlay = {0};            /* Struct used for decoding frames */
static OEMVocoder * gpVocInUse = NULL;    /* Pointer to the IVocoder with access
                                             to vocoder */
static QFrame gErasure;                   /* Erasure Frame used when no other
                                             frame available */
static uint16 gErasureCount = 0;          /* Count of frames erased since last
                                             callback */
static uint16 gPlayedCount = 0;           /* Count of frames played since last
                                             callback */
static uint8 erasure_bits[2] = {0,0};     /* data bits to stuff into vocoder when
                                             playing erasure.  Needs to be
                                             allocated, contents don't matter */

/* NOTE:  It is necessary to control the 'operating' boolean for decoding by the
          DecodeStatusType variable because the vocoder task utilizes the
          'operating' boolean.  The 'operating' boolean for decoding cannot be
          modified directly by any application call or a race condition may be
          encountered. */
static DecodeStatusType gDecStat = STOPPED;        //State of the decoding operation.


//Callbacks registered by AEE Layer
AEECallback *gpcbReady = NULL,*gpcbHaveData = NULL,*gpcbNeedData = NULL,*gpcbPlayedData = NULL;


//TaskControl Block for the Vocoder Task
static rex_tcb_type* gpVocTCB;

/*===============================================================================
                     FUNCTION DEFINITIONS
===============================================================================*/


/*===========================================================================

  Function: OEMVocoder_New

==============================================================================*/
OEMINSTANCE OEMVocoder_New(AEECLSID classId, int *nErr)
{
   OEMVocoder * pV;

   FARF(VOC_STATUS,("OEMVocoder_New"));

   if (classId != AEECLSID_VOCODER)
   {
      if(nErr)
         *nErr = ECLASSNOTSUPPORT;

      return(0);
   }

   pV = (OEMVocoder *)sys_malloc(sizeof(OEMVocoder));

   if (!pV)
   {
      FARF(VOC_STATUS,("OEMVocoder_New: Out of memory"));

      if(nErr)
         *nErr = ENOMEMORY;

      return(0);
   }

#if defined(FEATURE_ACM) && defined (FEATURE_ODM_UI)
   *nErr = OEMVocoder_ACMCreateInstance(&pV->m_pAcmInfo);
#endif // FEATURE_ACM && FEATURE_ODM_UI

   pV->m_eVocoder = VOC_NONE;

   if(nErr)
      *nErr = SUCCESS;



   FARF(VOC_STATUS,("OEMVocoder_New Success"));

   return((OEMINSTANCE)pV);
}


/*===========================================================================

   Function: OEMVocoder_Release

==============================================================================*/

int OEMVocoder_Delete(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   FARF(VOC_STATUS,("OEMVocoder_Delete"));

   if(!pV)
      return(EFAILED);

   //See if the instance being released had access to the vocoder,
   if (pV == gpVocInUse)
   {
      ShutDownVocoder();
   }

   FARF(VOC_STATUS,("Freeing OEMVocoder"));


   sys_free(pV);

   FARF(VOC_STATUS,("Ret OEMVoc_Del"));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_Configure

==============================================================================*/
int16 OEMVocoder_Configure(OEMINSTANCE handle,
                           const IVocoderConfigType *pconfig,
                           IVocoderInfoType * pInfo)
{
   uint16 newLen;

   OEMVocoder *pV = (OEMVocoder *)handle;
   IVocoderConfigType config;
   dword dwSave = 0;

   if(!pV || !pconfig)
      return(EBADPARM);

   FARF(VOC_STATUS,("OEMVoc_Config"));

   config = *pconfig;

   //Zero out the return parameter, if provided.
   if (pInfo)
   {
      pInfo->frameDuration = 0;
      pInfo->maxFrameSize = 0;
   }

   // If vocoder is configured and it is not owned by this instance, then
   // return error.
   if (gpVocInUse && gpVocInUse != pV)
   {
      FARF(VOC_STATUS,("Do not have access to vocoder"));
      return(AEE_VOC_EIN_USE);
   }

   // No vocoder indicates that the user is deregistering usage of the vocoder.
   if (config.vocoder == VOC_NONE)
   {
      ShutDownVocoder();
      return(SUCCESS);
   }

   //Check if  Vocoder is Supported
   if ((config.vocoder < VOC_NONE) ||
       (config.vocoder >= VOC_MAX) ||
       !voc_capability_supported(gVocoders[config.vocoder]))
   {
      FARF(VOC_STATUS,("Cap not support."));
      return( AEE_VOC_ENOT_AVAIL );
   }


   //Validate the Rate
   if ((config.min < EIGHTH_RATE) ||
       (config.min >  config.max) ||
       (config.max < EIGHTH_RATE) ||
       (config.max > FULL_RATE))
   {
      FARF(VOC_STATUS,("RateOutOfRange"));

      return( AEE_VOC_ERATE_OUT_OF_RANGE );
   }

   //Validate txReduction
   if (config.txReduction >  0)
   {
      //Tx Reduction value is vocoder specific.

      if(config.vocoder == VOC_SMV)
      {
        //Acceptable SMV modes are 0-5.  See SMV standard for details.
        if (config.txReduction > 5)
        {
           FARF(VOC_STATUS,("SMV BadTxRed"));

           return( AEE_VOC_EBAD_TX_REDUCTION );
        }
      }
      else
      {
        //{2,4,6,8} are the only acceptable non-zero values, and min/max must
        //be 1/8 and 1 respectively.
        if ((config.txReduction & 1) ||
            (config.txReduction > 8) ||
            (config.min != EIGHTH_RATE) ||
            (config.max != FULL_RATE))
        {
           FARF(VOC_STATUS,("BadTxRed"));
           return( AEE_VOC_EBAD_TX_REDUCTION );
        }
      }

   }

   //Validate Watermark. Must have a positive value for IVOCODER to operate
   if (config.watermark == 0)
   {
      FARF(VOC_STATUS,("Zero Watermark"));
      return( EBADPARM );
   }

   newLen = RateToLength[config.vocoder][config.max];

   //Shutdown the Vocoder and re-config so that the new configuration takes effect.
   if(gpVocInUse)
      ShutDownVocoder();

   //If the q items have not been created yet, or if the number and size do not
   //match, delete the existing ones, if any, and re-create them.
   //If frame max lengths differ, only re-allocate if newLen is larger.
   if (!OEMVocoder_InitPlayData(config.watermark, newLen))
   {
      FARF(VOC_STATUS,("NoMem"));
      return(ENOMEMORY);

   }

   pV->m_nWatermark = config.watermark;
   pV->m_eVocoder = config.vocoder;
   pV->m_eMin = config.min;
   pV->m_eMax = config.max;
   pV->m_nTxReduction = config.txReduction;
   pV->m_bOverwrite = config.overwrite;

   INTLOCK_SAV(dwSave);

   gErasure.length = RateToLength[config.vocoder][ERASURE];
   gpVocInUse = pV;
   gDecStat = STOPPED;

   //Set up the erasure frame
   gErasure.rate = VOC_ERASURE;
   gErasure.data = erasure_bits;

   INTFREE_SAV(dwSave);

   //Power up the vocoder(may already be powered up by other client)
   voc_acquire(VOC_ACQ_VS, VocStateCB);

   if (voc_config(VOC_ACQ_VS, gVocoders[pV->m_eVocoder]) != VOC_DONE_S)
   {
      FARF(VOC_STATUS,("Voc Config Error"));

      ShutDownVocoder();

      return(EFAILED);
   }

   if (pInfo)
   {
      pInfo->frameDuration = FRAME_DURATION;
      pInfo->maxFrameSize = RateToLength[config.vocoder][config.max];
   }

   FARF(VOC_STATUS,("OEMVoc_Config.Success"));
   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InStart

==============================================================================*/
int16 OEMVocoder_InStart(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   dword dwSave = 0;

   FARF(VOC_STATUS,("OEMVoco_InStart"));

   CHECKUSAGE(pV);

   INTLOCK_SAV(dwSave);

   if(!gAcquire.operating)
   {
      gAcquire.callApp = FALSE;
      gAcquire.operating = TRUE;
   }

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("End OEMVoco_InStart"));
   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InStop

==============================================================================*/
int16 OEMVocoder_InStop(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   dword dwSave = 0;

   FARF(VOC_STATUS,("OEMVoco_InStop"));

   CHECKUSAGE(pV);

   INTLOCK_SAV(dwSave);

   gAcquire.operating = FALSE;
   gAcquire.callApp = FALSE;

   INTFREE_SAV(dwSave);

#if defined (FEATURE_ACM)
   OEMVocoder_ACMFree (&pV->m_pAcmInfo);
#endif // FEATURE_ACM

   FARF(VOC_STATUS,("End OEMVoco_InStop"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InReset

==============================================================================*/
int16 OEMVocoder_InReset(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   QFrame * pQF;
   boolean prev = gAcquire.operating;
   dword dwSave = 0;

   CHECKUSAGE(pV);

   FARF(VOC_STATUS,("OEMVoco_InReset"));

   INTLOCK_SAV(dwSave);

   //Have to stop the encoding process to flush the contents.
   gAcquire.operating = FALSE;
   gAcquire.callApp = FALSE;

   while ((pQF = (QFrame *)q_get(&gAcquire.inUseQ)) != NULL)
   {
      q_put(&gAcquire.freeQ, &pQF->link);
   }

   gAcquire.operating = prev;

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("End OEMVoco_InReset"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutStart

==============================================================================*/
int16 OEMVocoder_OutStart(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;
   dword       dwSave = 0;

   CHECKUSAGE(pV);

   FARF(VOC_STATUS,("OEMVoco_OutStart"));

   INTLOCK_SAV(dwSave);

   if (gDecStat == STOPPED)
   {
      if (q_cnt(&gPlay.inUseQ) >= gpVocInUse->m_nWatermark)
      {
         gDecStat = PLAYING;
      }
      else
      {
      gDecStat = BUFFERING;
      }
   }

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("EndOEMVoco_OutStart"));


   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutStop

==============================================================================*/
int16 OEMVocoder_OutStop(OEMINSTANCE handle)
{
   OEMVocoder *pV = (OEMVocoder *)handle;
   dword       dwSave = 0;

   FARF(VOC_STATUS,("OEMVoco_OutStop"));

   CHECKUSAGE(pV);

   INTLOCK_SAV(dwSave);

   gDecStat = STOPPED;
   gPlay.callApp = FALSE;

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("EndOEMVoco_OutStop"));

#if defined (FEATURE_ACM)
   OEMVocoder_ACMFree (&pV->m_pAcmInfo);
#endif // FEATURE_ACM

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutReset

==============================================================================*/
int16 OEMVocoder_OutReset(OEMINSTANCE handle)
{
   QFrame * pQF;
   boolean prev = gPlay.operating;
   OEMVocoder *pV = (OEMVocoder *)handle;
   dword    dwSave = 0;

   FARF(VOC_STATUS,("OEMVoco_OutReset"));

   CHECKUSAGE(pV);

   INTLOCK_SAV(dwSave);

   //Have to stop the decoding process to flush the contents.
   gPlay.operating = FALSE;
   gPlay.callApp = FALSE;

   while ((pQF = (QFrame *)q_get(&gPlay.inUseQ)) != NULL)
   {
      q_put(&gPlay.freeQ, &pQF->link);
   }

   //If the vocoder was playing, it now has nothing to play, so go back to
   //buffering.
   if (gDecStat == PLAYING)
   {
      gDecStat = BUFFERING;
   }

   gPlay.operating = prev;

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("EndOEMVoco_OutReset"));


   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_InRead

==============================================================================*/
int16 OEMVocoder_InRead(OEMINSTANCE handle,
                           DataRateType * pRate,
                           uint16 * pLength,
                           uint8 * pData)
{
   QFrame * pQF;

   OEMVocoder *pV = (OEMVocoder *)handle;
   uint16 nLen;

   FARF(VOC_STATUS,("OEMVoco_InRead"));

   CHECKUSAGE(pV);

   pQF = (QFrame *)q_get(&gAcquire.inUseQ);

   if (!pQF)
   {
      FARF(VOC_STATUS,("Buffer Empty"));
      return(AEE_VOC_EBUFFER_EMPTY);
   }

   if (pRate)
   {
      *pRate = (DataRateType)pQF->rate;
   }

   nLen = *pLength;
   *pLength = pQF->length;

   if(nLen > pQF->length)
      nLen = pQF->length;

   if (pData)
   {
      MEMMOVE(pData,pQF->data,nLen);
   }

   q_put(&gAcquire.freeQ, &pQF->link);

   FARF(VOC_STATUS,("EndOEMVoco_InRead"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_OutWrite

==============================================================================*/
int16 OEMVocoder_OutWrite(OEMINSTANCE handle,
                                    DataRateType rate,
                                    uint16 length,
                                    const uint8 * pData)
{
   QFrame * pQF;
   int16 status = SUCCESS;
   OEMVocoder *pV = (OEMVocoder *)handle;
   dword    dwSave = 0;

   CHECKUSAGE(pV);

   FARF(VOC_STATUS,("OEMVoco_OutWrite"));

   if ((rate == ERASURE) ||
       ((length == RateToLength[pV->m_eVocoder][rate]) && pData))
   {
      pQF = (QFrame *)q_get(&gPlay.freeQ);

      if (!pQF)
      {
         status = AEE_VOC_EBUFFER_FULL;
      }
      else
      {
         pQF->rate = (voc_rate_type)rate;

         if (rate == ERASURE)
         {
            MEMCPY(pQF->data,gErasure.data,gErasure.length);
         }
         else
         {
            MEMMOVE(pQF->data,pData,length);
         }

         q_put(&gPlay.inUseQ,&pQF->link);

         INTLOCK_SAV(dwSave);

         if (!gPlay.operating)
         {
            if ((gDecStat == BUFFERING) &&
                (q_cnt(&gPlay.inUseQ) >= gpVocInUse->m_nWatermark))
            {
               gDecStat = PLAYING;
               gPlay.operating = TRUE;
            }
            else if (gDecStat == STOPPED)
            {
               status = AEE_VOC_ENOT_STARTED;
            }
         }
         INTFREE_SAV(dwSave);
      }
   }
   else
   {
      FARF(VOC_STATUS,("Bad Frame: l=%d, r=%d",length, rate, 0));
      status = EBADPARM;
   }

   FARF(VOC_STATUS,("EndOEMVoco_OutWrite %d",status));
   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(status);
}

/*===========================================================================

   Function: OEMVocoder_GetInDepth

==============================================================================*/
int16 OEMVocoder_GetInDepth(OEMINSTANCE handle,
                            uint16 * pDepth)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   CHECKUSAGE(pV);

   if (!pDepth)
   {
      return(EBADPARM);
   }

   *pDepth = (uint16)q_cnt(&gAcquire.inUseQ);

   FARF(VOC_STATUS,("EndOEMVoco_InDepth"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_GetOutDepth

==============================================================================*/
int16 OEMVocoder_GetOutDepth(OEMINSTANCE handle,
                             uint16 * pDepth)
{
   OEMVocoder *pV = (OEMVocoder *)handle;

   CHECKUSAGE(pV);

   if (!pDepth)
   {
      return(EBADPARM);
   }

   *pDepth = (uint16)q_cnt(&gPlay.inUseQ);

   FARF(VOC_STATUS,("EndOEMVoco_OutDepth"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(SUCCESS);
}

/*===========================================================================

   Function: OEMVocoder_GetFrameLength

==============================================================================*/
int16 OEMVocoder_GetFrameLength(OEMINSTANCE handle,
                                VocoderType vocoder,
                                DataRateType rate,
                                uint16 * pLength)
{
   int16 retVal = EBADPARM;
   OEMVocoder *pV = (OEMVocoder *)handle;

   if(!pV)
      return(EBADPARM);

   if (pLength &&
       (vocoder > VOC_NONE) &&
       (vocoder < VOC_MAX) &&
       (rate >= BLANK_RATE) &&
       (rate < MAX_RATE))
   {
      *pLength = RateToLength[vocoder][rate];
      retVal = SUCCESS;
   }

   FARF(VOC_STATUS,("EndOEMVoco_GetFramLen"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(retVal);
}

/*===========================================================================

   Function: OEMVocoder_PlayTone

==============================================================================*/
int16 OEMVocoder_PlayTone(OEMINSTANCE handle,
                          uint16 tone1,
                          uint16 tone2,
                          int16 volume,
                          uint16 duration)
{
   int16 status = SUCCESS;
   voc_status_type cmdStatus;
   OEMVocoder *pV = (OEMVocoder *)handle;
   dword dwSave = 0;

   CHECKUSAGE(pV);

   INTLOCK_SAV(dwSave);

   status = ((gPlay.operating || gAcquire.operating) ? AEE_VOC_EACTIVE : SUCCESS);

   INTFREE_SAV(dwSave);

   if(status == SUCCESS)
   {
      cmdStatus = voc_dtmf_cmd(tone1,tone2,volume,duration);

      if (cmdStatus != VOC_DONE_S)
      {
         FARF(VOC_STATUS,("voc_dtmf_cmd failed %d",cmdStatus));
         status = EFAILED;
      }
   }

   FARF(VOC_STATUS,("EndOEMVoco_PlayTone"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return(status);
}

/*===========================================================================
   Registration Function for CBs
===========================================================================*/
int   OEMVocoder_RegisterCB(AEECallback *pcbReady,
                            AEECallback *pcbHaveData,
                            AEECallback *pcbNeedData,
                            AEECallback *pcbPlayedData)
{
   dword dwSave = 0;

   INTLOCK_SAV(dwSave);

   gpcbReady = pcbReady;
   gpcbHaveData = pcbHaveData;
   gpcbNeedData = pcbNeedData;
   gpcbPlayedData = pcbPlayedData;

   INTFREE_SAV(dwSave);

   return(SUCCESS);
}

/*===========================================================================
   Get number of frames available
===========================================================================*/
uint16 OEMVocoder_GetDataAvailCount(void)
{
   uint16 nCount = (uint16)q_cnt(&gAcquire.inUseQ);

   FARF(VOC_STATUS,("AvailCount %d",nCount));

   return(nCount);
}

/*===========================================================================
   Get number of frames played
===========================================================================*/
uint16 OEMVocoder_GetDataPlayedCount(void)
{
   uint16 nCount;
   dword  dwSave = 0;

   INTLOCK_SAV(dwSave);

   nCount= gPlayedCount;
   gPlayedCount = 0;

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("PlayedCount %d",nCount));

   return(nCount);
}

/*===========================================================================
   Get number of Erasures Played
===========================================================================*/
uint16 OEMVocoder_GetErasureCount(void)
{
   uint16 nCount;
   dword  dwSave = 0;

   INTLOCK_SAV(dwSave);

   nCount = gErasureCount;
   gErasureCount = 0;

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("ErasureCount %d",nCount));

   return(nCount);
}


/*===========================================================================
   Free Memory and set it to NULL
===========================================================================*/
static void FreeIfUsed(void **ppObj)
{
   if (ppObj)
   {
      if (*ppObj)
      {
         sys_free(*ppObj);
         *ppObj = NULL;
      }
   }
}

/*===========================================================================
  Initialize the Play Data
===========================================================================*/
static boolean OEMVocoder_InitPlayData(uint32 qSize,uint16 frameLen)
{
   boolean retVal = FALSE;
   dword   dwSave = 0;

   FARF(VOC_STATUS,("Begin InitPlayData"));

   INTLOCK_SAV(dwSave);

   OEMVocoder_ReleasePlayData();

   //Create the q items and put them all on the free q.
   (void)q_init(&gPlay.freeQ);
   (void)q_init(&gPlay.inUseQ);
   (void)q_init(&gAcquire.freeQ);
   (void)q_init(&gAcquire.inUseQ);

   gPlay.frames = (QFrame*)sys_malloc(qSize * sizeof(QFrame));
   gPlay.data = (uint8 *)sys_malloc(qSize  * frameLen);
   gAcquire.frames = (QFrame*)sys_malloc(qSize * sizeof(QFrame));
   gAcquire.data = (uint8 *)sys_malloc(qSize * frameLen);

   //Check to see if all memory has been allocated succesfully.
   if (gPlay.frames && gPlay.data && gAcquire.frames && gAcquire.data)
   {
      uint32 i;
      uint8 * acquireIndex = gAcquire.data;
      uint8 * playIndex = gPlay.data;

      //Point the data fields in the QFrames to a block in memory
      for (i = 0; i < qSize; i++)
      {
         gPlay.frames[i].data = playIndex;
         gAcquire.frames[i].data = acquireIndex;

         playIndex += frameLen;
         acquireIndex += frameLen;

         //Put the newly created frames onto the respective free queues.
         q_put(&gPlay.freeQ, q_link(&gPlay.frames[i], &gPlay.frames[i].link));
         q_put(&gAcquire.freeQ, q_link(&gAcquire.frames[i],
                                       &gAcquire.frames[i].link));
      }
      retVal = TRUE;
   }
   else
   {
      OEMVocoder_ReleasePlayData();
   }

   INTFREE_SAV(dwSave);

   FARF(VOC_STATUS,("InitPlayData %d",retVal));
   return(retVal);
}

/*===========================================================================
 Cleanup Play Data. It is assumed that the caller has done INTLOCK
===========================================================================*/
static void OEMVocoder_ReleasePlayData(void)
{
   gAcquire.callApp = FALSE;
   gAcquire.operating = FALSE;
   gPlay.callApp = FALSE;
   gPlay.operating = FALSE;
   FreeIfUsed((void **)(&gPlay.data));
   FreeIfUsed((void **)(&gPlay.frames));
   FreeIfUsed((void **)(&gAcquire.data));
   FreeIfUsed((void **)(&gAcquire.frames));
}


/*===========================================================================

Function: VocStateCB

Description:
      This function is called by the vocoder task whenever the vocoder changes
      state.  When the vocoder is in a state where it will accept rate cmds,
      set the rates, and inform the user that the vocoder is ready for use.

==============================================================================*/
static void VocStateCB(voc_inform_type new_status )
{
   //This function is in the Vocoder Task. Get the TCB for it if we do not have it already
   if(!gpVocTCB)
      gpVocTCB = rex_self();

   FARF(VOC_STATUS,("InVocStaeCB"));

   if (new_status == VOC_INFORM_VOCODER)
   {
      // For anything except SMV, perform these steps
      if(gpVocInUse->m_eVocoder != VOC_SMV)
      {
         //This function always returns SUCCESS
         (void)voc_set_frame_rate_cmd((voc_rate_type)gpVocInUse->m_eMax,
                                      (voc_rate_type)gpVocInUse->m_eMin);

         (void)voc_tx_rate_limit(gpVocInUse->m_nTxReduction);

      }
      else // For SMV TxReduction is overloaded for use in voc_set_rate_level()
      {
#ifdef FEATURE_SMV
         voc_set_rate_level(gpVocInUse->m_nTxReduction);
#endif /* FEATURE_SMV */
      }

      // Register reverse (from the vocoder) and forward link (to the vocoder)
      // callbacks
      voc_register_packet_clients(VocoderFrameAcquiredCB, VocoderPlayoutFrameCB);

      //Call the client
      if(gpVocInUse && gpcbReady)
         AEE_SYS_RESUME(gpcbReady);
   }

}

/*===========================================================================

Function: OEMVocoder_TaskCallback

Description:
      This function is directly invoked by the VocoderTask when the signal AEE_VOC_ISR_SIG is set
      by the ISR. This function will call the appropriate AEE-supplied callback using
      BREW Resumes (AEE_SYS_RESUME).

==============================================================================*/
void OEMVocoder_TaskCallback(void)
{
   dword dwSave = 0;

   FARF(VOC_STATUS,("InVocTaskCB"));

   // See if one or more frames have been acquired, and if the application has
   //set a callback for this event.


   if (gpVocInUse && gAcquire.callApp && gpcbHaveData)
   {
      INTLOCK_SAV(dwSave);

      gAcquire.callApp = FALSE;

      INTFREE_SAV(dwSave);

      AEE_SYS_RESUME(gpcbHaveData);

   }

   // See if one or more erasures have been played, and if the application has
   //set a callback for this event.
   if (gpVocInUse && gPlay.callApp)
   {
      dword dwSave   = 0;
      uint16 erasures;

      //The voc isr might interrupt BREW here, so make copies of the values
      //so they can start incrementing again.
      INTLOCK_SAV(dwSave);

      gPlay.callApp = FALSE;
      erasures = gErasureCount;
      INTFREE_SAV(dwSave);

      if (erasures > 0)
      {
         //We have run out of frames to play, so suspend playout until more are
         //added. The decode state should have been 'playing', so go back to
         //buffering.

         INTLOCK_SAV(dwSave);
         gPlay.operating = FALSE;
         gDecStat = BUFFERING;

         INTFREE_SAV(dwSave);

         if (gpcbNeedData)
         {
            AEE_SYS_RESUME(gpcbNeedData);

         }
      }
      else
      {
         if (gpcbPlayedData)
         {
            AEE_SYS_RESUME(gpcbPlayedData);
         }
      }
   }

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));
}

/*===========================================================================

Function: ShutDownVocoder

Description:
     This function will perform all necessary steps to completely shut down
     the vocoder, and release any data associated with using the vocoder.

==============================================================================*/
void ShutDownVocoder(void)
{
   dword dwSave = 0;

   FARF(VOC_STATUS,("OEMVoc_ShutDownVoc"));

   INTLOCK_SAV(dwSave);

   gAcquire.callApp = FALSE;
   gAcquire.operating = FALSE;
   gPlay.callApp = FALSE;
   gPlay.operating = FALSE;

   //Indicate that no instance of IVocoder is currently using the vocoder.
   gpVocInUse = NULL;

   OEMVocoder_ReleasePlayData();

   //Cancel any pending callbacks
   CALLBACK_Cancel(gpcbReady);
   CALLBACK_Cancel(gpcbHaveData);
   CALLBACK_Cancel(gpcbNeedData);
   CALLBACK_Cancel(gpcbPlayedData);

   INTFREE_SAV(dwSave);

   // Deregister reverse and forward link functions
   voc_register_packet_clients(NULL, NULL);

   //Power down vocoder.(May not actually power it down if another client
   //has acquired the vocoder)
   voc_release(VOC_ACQ_VS);

   FARF(VOC_STATUS,("End OEMShutDownVoc"));
}

/*===========================================================================

Function:  VocoderFrameAcquiredCB

Description:
      This is the ISR. This routine will pull(if acquiring!) a frame out of the vocoder and
      put it onto a queue.  It will also notify the Vocoder task that it has done so by setting
      the signal AEE_VOC_ISR_SIG. The VocoderTask will then invoke the function OEMVocoder_TaskCallback
      in the vocoder task context.

==============================================================================*/
static void VocoderFrameAcquiredCB(uint8 *data, voc_rate_type rate, uint16 len)
{
   if (gpVocInUse && gAcquire.operating)
   {
      // Grab an empty frame buffer
      QFrame * pQF = (QFrame *)q_get(&gAcquire.freeQ);

      if (!pQF && gpVocInUse->m_bOverwrite)
      {
         //If the application has opted to overwrite frames when the encoding
         //buffer fills, pull an already encoded frame and re-queue it with
         //the new data.  This effectively erases the front end of the queue.
         pQF = (QFrame *)q_get(&gAcquire.inUseQ);

         if (!pQF)
         {
            //FARF(VOC_STATUS,("Voc:Problems with Queues"));
            return;
         }
      }

      if (pQF)
      {
         // Copy the frame information and bits
         pQF->vocoder = gpVocInUse->m_eVocoder;
         pQF->rate = rate;

         //The len parameter given to us by dmss can be incorrect, so don't
         //use it.  If a frame type has an odd length, dmss rounds it up to
         //an even length.
         pQF->length = RateToLength[gpVocInUse->m_eVocoder][rate];
         MEMCPY(pQF->data, data, pQF->length);

         // Give the frame to the application
         q_put(&gAcquire.inUseQ, &pQF->link);

         if (!gAcquire.callApp)
         {
            gAcquire.callApp = TRUE;

            rex_set_sigs(gpVocTCB, AEE_VOC_ISR_SIG);
         }
      }
      else
      {
         //FARF(VOC_STATUS,("Voc:Acquire buffer full"));
      }
   }
}

/*===========================================================================

Function:  VocoderPlayoutFrameCB

Description:
      This is the ISR. This routoine will dequeue a frame queued by the application and put it
      into the vocoder.  An erasure will be substituted if no such frame is
      available.  The vocoder task will be notified (using the signal AEE_VOC_ISR_SIG) if an erasure
      has been played when the decoding process is in operation. The VocoderTask will then invoke the
      function OEMVocoder_TaskCallback  in the vocoder task context.

      When this routine returns, the parameters pData and pRate must contain the next frame to be
      played by the Vocoder.
==============================================================================*/
static void VocoderPlayoutFrameCB(uint8** pData, voc_rate_type *pRate)
{
   voc_rate_type rate = gErasure.rate;
   uint8 * data = gErasure.data;

   QFrame * pQF = NULL;

   if (gPlay.operating)
   {
      // Get a frame from the playout queue supplied by the application
      if ((pQF = (QFrame *)q_get(&gPlay.inUseQ)) != NULL)
      {
         gPlayedCount++;

         rate = pQF->rate;
         data = pQF->data;
         // q_put needlessly locks interrupts when we are in an
         // interrupt context, so if there is something faster...
         q_put(&gPlay.freeQ, &pQF->link);
      }
      else
      {
         gErasureCount++;
      }

      if (!gPlay.callApp)
      {
         rex_set_sigs(gpVocTCB, AEE_VOC_ISR_SIG);
         gPlay.callApp = TRUE;
      }
   }

   // Give the frame to the vocoder
   // n.b. We assume this function is called at high priority
   // else we have a race for the data pointer which is now
   // in the free queue

   // The parameters pData and pRate must contain the next frame to be played by the Vocoder.

   *pRate = rate;
   *pData = data;
}

#endif // #if !defined(FEATURE_IVOCODER) || !defined(FEATURE_VOC_PACKET_INTERFACE)

#endif // #if !defined(FEATURE_IVOCODER)


#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
/*=============================================================================
FUNCTION: OEMVocoder_ACMCreateInstance

DESCRIPTION: Create a new instance for APPRESCTL

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMVocoder_ACMCreateInstance(IVOCODER_ACMInfo * pAcmInfo)
{
   int                       ret = SUCCESS;
   IShell *                  pIShell = AEE_GetShell();

   /* Create AppResCtl object */
   ret=ISHELL_CreateInstance(pIShell,AEECLSID_APPRESCTL,(void**)(&pAcmInfo->m_pAppResCtl));

   if(ret != SUCCESS)
   {
      MSG_ERROR("ISHELL_CreateInstance failed. RC=%d",ret,0,0);
      pAcmInfo->m_pAppResCtl = NULL;
      return EFAILED;
   }

   MSG_LOW("ISHELL_CreateInstance success. Ptr=%x",pAcmInfo->m_pAppResCtl,0,0);

   return SUCCESS;
}
#endif // FEATURE_ODM_UI

/*=============================================================================
FUNCTION: OEMVocoder_ACMAcquire

DESCRIPTION: Acquire resources with ACM with specified reasonCode

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMVocoder_ACMAcquire(IVOCODER_ACMInfo * pAcmInfo, uint32 reasonCode)
{
   uint32                    nRet;
#if defined (FEATURE_ODM_UI)
   uint32                    resctl_status;
#endif // FEATURE_ODM_UI

   ACM_resource_state_type   pmgr_status;
   PFNNOTIFY                 pcb = &OEMVocoder_ACMChangeNotify;

#if defined (FEATURE_ODM_UI)
   if (pAcmInfo->m_pAppResCtl->state == ACM_RESOURCE_ACQUIRED)
   {
      MSG_LOW("IAPPRESCTL_Acquire resource already acquired",0,0,0);
   }
   else
   {
      // Start acquiring resource
      if((nRet = IAPPRESCTL_Acquire(pAcmInfo->m_pAppResCtl, reasonCode, &resctl_status)) != SUCCESS)
         return nRet;
   }

   if( resctl_status == RESCTL_STATUS_BUSY )
   {
      MSG_ERROR("IAPPRESCTL_Acquire failed with RESCTL_STATUS_BUSY",0,0,0);
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }

   MSG_LOW("IAPPRESCTL_Acquire succeded",0,0,0);

   // Fill in call back information
   pAcmInfo->m_acm_callback_data_type.state = ACM_RESOURCE_UNKNOWN;
   //pAcmInfo->m_acm_callback_data_type.pdata = pAcmInfo->m_pCBInfo;
   if (reasonCode == ACM_NETWORK_ORIGINATED_POC_CALL)
   {
      pcb = &OEMVocoder_ACM_OutStart_ChangeNotify;
   }
   else if (reasonCode == ACM_MOBILE_ORIGINATED_POC_CALL)
   {
      pcb = &OEMVocoder_ACM_InStart_ChangeNotify;
   }

   // Register callback for resouce state change
   nRet = OEMACM_StateMgr_OnStatusChangeNotify(pAcmInfo->m_pAppResCtl, pcb,
                                               &pAcmInfo->m_acm_callback_data_type);

   if(nRet != SUCCESS)
   {
      MSG_ERROR("OEMACM_OnStatusChangeNotify failed with RC=%d",nRet,0,0);
      OEMVocoder_ACMFree (pAcmInfo);
      return nRet;
   }

   // Get resource state from ACM state manager
   if((nRet = OEMACM_StateMgr_GetResourceState(pAcmInfo->m_pAppResCtl,&pmgr_status)) != SUCCESS)
   {
      MSG_ERROR("OEMACM_StateMgr_GetResourceState failed with RC=%d",nRet,0,0);
      OEMVocoder_ACMFree (pAcmInfo);
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }

   if(pmgr_status == ACM_RESOURCE_ACQUIRED)
   {
      // Immediate Acquire
      MSG_LOW("OEMACM_GetResourceState returned with Immediate Acquire",0,0,0);

      switch (reasonCode)
      {
         case ACM_NETWORK_ORIGINATED_POC_CALL:
            OEMVocoder_ACM_OutStart();
            break;
         case ACM_MOBILE_ORIGINATED_POC_CALL:
            OEMVocoder_ACM_InStart();
            break;
         default:
            // Do nothing
            break;
      }

      return SUCCESS;
   }
   else
   {
      // Delayed Acquire
      MSG_LOW("OEMACM_GetResourceState returned with Delayed Acquire",0,0,0);
      return SUCCESS;
   }
   OEMVocoder_ACMFree (pAcmInfo);
   return EFAILED;
#else // !FEATURE_ODM_UI
   nRet = OEMACM_PriorityMgr_Acquire (pAcmInfo->handle, reasonCode, &pmgr_status);
   if (nRet != SUCCESS)
      return nRet;

   if (pmgr_status == ACM_RESOURCE_BUSY )
   {
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }

   MSG_LOW("ACM Acquire succeded",0,0,0);

   // Fill in call back information
   pAcmInfo->m_acm_callback_data_type.state = ACM_RESOURCE_UNKNOWN;
   //pAcmInfo->m_acm_callback_data_type.pdata = pAcmInfo->m_pCBInfo;
   if (reasonCode == ACM_NETWORK_ORIGINATED_POC_CALL)
   {
      pcb = &OEMVocoder_ACM_OutStart_ChangeNotify;
   }
   else if (reasonCode == ACM_MOBILE_ORIGINATED_POC_CALL)
   {
      pcb = &OEMVocoder_ACM_InStart_ChangeNotify;
   }

   // Register callback for resouce state change
   nRet = OEMACM_StateMgr_OnStatusChangeNotify(pAcmInfo->handle, pcb,
                                               &pAcmInfo->m_acm_callback_data_type);

   if(nRet != SUCCESS)
   {
      MSG_ERROR("OEMACM_OnStatusChangeNotify failed with RC=%d",nRet,0,0);
      OEMVocoder_ACMFree (pAcmInfo);
      return nRet;
   }

   if (pmgr_status == ACM_RESOURCE_ACQUIRED)
   {
      // Immediate Acquire
      MSG_LOW("OEMACM_GetResourceState returned with Immediate Acquire",0,0,0);

      switch (reasonCode)
      {
         case ACM_NETWORK_ORIGINATED_POC_CALL:
            OEMVocoder_ACM_OutStart();
            break;
         case ACM_MOBILE_ORIGINATED_POC_CALL:
            OEMVocoder_ACM_InStart();
            break;
         default:
            // Do nothing
            break;
      }

      return SUCCESS;
   }
   else
   {
      // Delayed Acquire
      MSG_LOW("OEMACM_GetResourceState returned with Delayed Acquire",0,0,0);
      return SUCCESS;
   }
#endif // FEATURE_ODM_UI
}

/*=============================================================================
FUNCTION: OEMVocoder_ACMFree

DESCRIPTION: Release resources with ACM

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static int OEMVocoder_ACMFree(IVOCODER_ACMInfo * pAcmInfo)
{
#if defined (FEATURE_ODM_UI)
   int ret = 0;

   /* Release resources with ACM */
   MSG_LOW("OEMVocoder_Release Releasing AppResCtl object %x",pAcmInfo->m_pAppResCtl,0,0);

   if (pAcmInfo->m_pAppResCtl != NULL)
   {
      ret=IAPPRESCTL_Free(pAcmInfo->m_pAppResCtl, NULL);
   }

   if (ret != SUCCESS)
   {
      MSG_ERROR("OEMVocoder_Release failed to release AppResCtl object with RC=%d",ret,0,0);
      return ret;
   }

   //OEMVocoder_FREE (pAcmInfo->m_pCBInfo);
   MSG_LOW("OEMVocoder_Release Releasing AppResCtl object done",0,0,0);
#else // !FEATURE_ODM_UI
   return OEMACM_PriorityMgr_Free (pAcmInfo->handle);
#endif // FEATURE_ODM_UI
}


/*=============================================================================
FUNCTION: OEMVocoder_ACMChangeNotify

DESCRIPTION: Default callback for ACM status change notify


DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMVocoder_ACMChangeNotify(void *p)
{
   ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
   ACM_resource_state_type state = cbdata->state;

   switch(state)
   {
      case ACM_RESOURCE_ACQUIRED:
      MSG_LOW("OEMVocoder_ACMChangeNotify received ACM_RESOURCE_ACQUIRED",0,0,0);
      break;

      default:
         MSG_LOW("OEMVocoder_ACMChangeNotify received resource state=%d",state,0,0);
         break;
   }
}


/*=============================================================================
FUNCTION: OEMVocoder_ACM_InStart_ChangeNotify

DESCRIPTION: ACM status change notify CB for InStart

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMVocoder_ACM_InStart_ChangeNotify(void *p)
{
   ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
   ACM_resource_state_type state = cbdata->state;

   switch(state)
   {
      case ACM_RESOURCE_ACQUIRED:
         OEMVocoder_ACM_InStart();
         MSG_LOW("OEMVocoder_ACMChangeNotify received ACM_RESOURCE_ACQUIRED",0,0,0);
         break;

      default:
         // TODO: Forced release
         MSG_LOW("OEMVocoder_ACMChangeNotify received resource state=%d",state,0,0);
         break;
   }
}

/*=============================================================================
FUNCTION: OEMVocoder_ACM_OutStart_ChangeNotify

DESCRIPTION: ACM status change notify CB for OutStart

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMVocoder_ACM_OutStart_ChangeNotify(void *p)
{
   ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
   ACM_resource_state_type state = cbdata->state;

   switch(state)
   {
       case ACM_RESOURCE_ACQUIRED:
          OEMVocoder_ACM_OutStart();
          MSG_LOW("OEMVocoder_ACMChangeNotify received ACM_RESOURCE_ACQUIRED",0,0,0);
          break;

      default:
         // TODO: Forced release
         MSG_LOW("OEMVocoder_ACMChangeNotify received resource state=%d",state,0,0);
         break;
   }
}


/*=============================================================================
FUNCTION: OEMVocoder_ACM_InStart

DESCRIPTION: Signal services layer for recording

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMVocoder_ACM_InStart()
{
   IVOC_LOCK();

   if(!gAcquire.operating)
   {
      gAcquire.callApp = FALSE;
      gAcquire.operating = TRUE;
   }

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("End OEMVoco_InStart"));
   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return;
}

/*=============================================================================
FUNCTION: OEMVocoder_ACM_OutStart

DESCRIPTION: Signal services layer for playing

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static void OEMVocoder_ACM_OutStart()
{
   FARF(VOC_STATUS,("OEMVoco_OutStart"));

   IVOC_LOCK();

   if (gDecStat == STOPPED)
   {
      if (q_cnt(&gPlay.inUseQ) >= gpVocInUse->m_nWatermark)
      {
         gDecStat = PLAYING;
      }
      else
      {
         gDecStat = BUFFERING;
      }
   }

   gPlay.callApp = FALSE;

   IVOC_UNLOCK();

   FARF(VOC_STATUS,("EndOEMVoco_OutStart"));

   FARF(VOC_STATUS,("gOperating=%d,gDecStat=%d",gPlay.operating,gDecStat));
   FARF(VOC_STATUS,("gAcquire=%d",gAcquire.operating));

   return;
}

#endif  // FEATURE_ACM
