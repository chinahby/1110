/*===========================================================================

FILE: OEMSoundCtl.c

SERVICES: ISoundCtl interface implementation

DESCRIPTION
   This file implements BREW ISoundCtl. ISoundCtl controls the sound devices

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/multimedia/audio/oem/staticextensions/isndctl/main/latest/src/OEMSoundCtl.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*=========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
========================================================================= */
#include <stdlib.h>
#include "AEE.h"
#include "OEMFeatures.h"
#include "audqconcert.h"


// FEATURE_AUDIO_EQUALIZER || FEATURE_AUDIO_QCONCERT || FEATURE_AUDIO_SPECTRUM_ANALYZER flag to enable/disable ISoundCtl
#if (defined(FEATURE_AUDIO_EQUALIZER)||defined(FEATURE_AUDIO_QCONCERT)||defined(FEATURE_AUDIO_SPECTRUM_ANALYZER))

#include "BREWVersion.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEESoundCtl.h"
#include "OEMObjectMgr.h"
#include "snd.h"
#include "SoundCtl.bid"

#if MIN_BREW_VERSION(3,0)
# include "OEMHeap.h"
# include "OEMCriticalSection.h"
# include "AEE_OEMDispatch.h"
#endif /* MIN_BREW_VERSION(3,0) */

/*=========================================================================

                      DEFINES

========================================================================= */
#if MIN_BREW_VERSION(3,0)
static OEMCriticalSection gcmCriticalSection;
# define MUTEX_LOCK() OEMCriticalSection_Enter(&gcmCriticalSection)
# define MUTEX_UNLOCK() OEMCriticalSection_Leave(&gcmCriticalSection)
#else /* MIN_BREW_VERSION(3,0) */
# define MUTEX_LOCK() OEMOS_MutexStart(); {
# define MUTEX_UNLOCK() } OEMOS_MutexStop();
#endif /* MIN_BREW_VERSION(3,0) */

#define OEMSOUNDCTL_RSP_MAX                 10  // can have 10 pending callbacks

#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
#define OEMEQUALIZER_NUM_BAND_MAX           12  // defined in cmx core file
#endif

#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
#define OEMANALYZER_DATA_SIZE               SND_SPECTRUM_ANALYZER_NUM_COEFFS  // spertrum analyzer data size
#define OEMANALYZER_DATA_MAX                32 // defined in cmx core file
#endif //FEATURE_AUDIO_SPECTRUM_ANALYZER
/*=========================================================================

                      OEMSoundCtl DECLARATIONS

=========================================================================*/

typedef struct SoundCtlRsp
{
  AEECallback       cb;
  AEESoundCtlNotify Notify;
  boolean           bInUse;
}SoundCtlRsp;


typedef struct AEESoundCtl
{
  const AEEVTBL(ISoundCtl) *pvt;
  int32                    m_nRefs;

  void *                   m_pac;      // App context
  AEECallback              m_pcbExit;
  AEEObjectHandle          m_hObject;

#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  uint16                   m_DataBuf[OEMANALYZER_DATA_SIZE];
  boolean                  m_bDataBufInUse;
  boolean                  m_bSpectrumAnalyzer;
  uint16                   m_nUpdatePeriod;
#endif
  SoundCtlRsp              m_Rsp[OEMSOUNDCTL_RSP_MAX];
  PFNSOUNDCTLNOTIFY        m_pfnNotify;
  void*                    m_pUser;
}AEESoundCtl;


extern int     OEMSoundCtl_New(IShell * ps, AEECLSID cls, void **ppif);
static void    OEMSoundCtl_Delete(ISoundCtl * po);
static uint32  OEMSoundCtl_AddRef(ISoundCtl * po);
static uint32  OEMSoundCtl_Release(ISoundCtl * po);
static int     OEMSoundCtl_QueryInterface(ISoundCtl * po, AEECLSID idReq, void ** ppo);

static int   OEMSoundCtl_RegisterNotify(ISoundCtl * po, PFNSOUNDCTLNOTIFY pfn, const void * pUser);
static void  OEMSoundCtl_InitRsp(ISoundCtl * po);
static void  OEMSoundCtl_FreeSoundCtlRsp(ISoundCtl * po);
SoundCtlRsp* OEMSoundCtl_GetCallback(ISoundCtl * po);
static void  OEMSoundCtl_CallbackNotify(SoundCtlRsp * pRsp);

// Get Parameters
static int   OEMSoundCtl_GetParm(ISoundCtl * po, int nParamID, int32 * pP1, int32 * pP2);

// Equalizer functions
static int   OEMSoundCtl_EnableEqualizer(ISoundCtl* po, boolean bEnable);
static int   OEMSoundCtl_SetEqualizer(ISoundCtl* po, AEESoundCtlEqualizerFilter* eqtype, uint32 bands);
static int   OEMSoundCtl_SetEqualizerGain(ISoundCtl* po, uint16 band, int16 nGain);

// Audio expander functions
static int   OEMSoundCtl_EnableAudioExpander(ISoundCtl* po, boolean bEnable);
static int   OEMSoundCtl_SetAudioExpander(ISoundCtl* po, AEESoundCtlAudioExpander* aepType);
// Audio qconcert reverb functions
static int   OEMSoundCtl_SetQconcertPlusReverbParams(ISoundCtl* po, AEESoundCtlAudioQconcertPlusReverb* aepType);

// Angled Mixing functions
static int   OEMSoundCtl_EnableAngledMixing(ISoundCtl* po, boolean bEnable);
static int   OEMSoundCtl_SetAngledMixing(ISoundCtl* po, AEESoundCtlAngledMixing* aepType, uint16 nParams);

//Spectrum analyzer functions
static int   OEMSoundCtl_EnableSpectrumAnalyzer(ISoundCtl* po, boolean bEnable);
static int   OEMSoundCtl_SetSpectrumAnalyzer(ISoundCtl* po, uint16 update_period);

// callback functions to cmx
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
static void OEMSoundCtl_EnableEqualizerCB(const void *client_data, snd_status_type status);
static void OEMSoundCtl_SetEqualizerCB(const void *client_data, snd_status_type status);
static void OEMSoundCtl_SetEqualizerGainCB(const void *client_data, snd_status_type status);
#endif


#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
static void OEMSoundCtl_EnableAudioExpanderCB(const void *client_data, snd_status_type status);
static void OEMSoundCtl_SetAudioExpanderCB(const void *client_data, snd_status_type status);
static void OEMSoundCtl_SetAudioQconcertPlusReverbCB(const void *client_data, snd_status_type status);
#endif

#if defined( FEATURE_AUDIO_QENSEMBLE)
#error code not present
#endif

#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
static void OEMSoundCtl_EnableSpectrumAnalyzerCB(const void *client_data, snd_status_type status);
static void OEMSoundCtl_SetSpectrumAnalyzerCB(const void *client_data, snd_status_type status);
static void OEMSoundCtl_SpectrumAnalyzerDataCB(const uint16 *data, void *client_data);
#endif

#if ((defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER)) || \
     (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))   || \
     (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER)))
static void OEMSoundCtl_ExecuteCallback(const void *client_data, snd_status_type status, int16 cmd);
static int   OEMSoundCtl_GetAEESoundStatus(snd_status_type e);
// put inside to remove compiler warnings, when none of FEATUREs are defined
static AEESoundCtl* toObject(ISoundCtl* po);
#endif
// helper functions to convert back and forth between ISoundCtl and AEESoundCtl
static ISoundCtl* toInterface(AEESoundCtl* pme);

static const AEEVTBL(ISoundCtl) gvtAEESoundCtl =
{
  OEMSoundCtl_AddRef,
  OEMSoundCtl_Release,
  OEMSoundCtl_QueryInterface,
  OEMSoundCtl_RegisterNotify,
  OEMSoundCtl_GetParm,
  OEMSoundCtl_EnableEqualizer,
  OEMSoundCtl_SetEqualizer,
  OEMSoundCtl_SetEqualizerGain,
  OEMSoundCtl_EnableAudioExpander,
  OEMSoundCtl_SetAudioExpander,
  OEMSoundCtl_SetQconcertPlusReverbParams,
  OEMSoundCtl_EnableAngledMixing,
  OEMSoundCtl_SetAngledMixing,
  OEMSoundCtl_EnableSpectrumAnalyzer,
  OEMSoundCtl_SetSpectrumAnalyzer
};
/*=========================================================================

                     ISoundCtl FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================
Function: OEMSoundCtl_New

Description:
  Function called by ISHELL_CreateInstance() to allocate and construct
  ISoundCtl object

Prototype:
   int OEMSoundCtl_New(IShell * ps, AEECLSID cls, void **ppif)

Parameter(s):
   cls: Pointer to the public IShell object
   cls: class id
   ppif: point to object created

Return Value:
   return SUCCESS or error codes

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
int OEMSoundCtl_New(IShell * ps, AEECLSID cls, void **ppif)
{
  AEESoundCtl *  pme;
  int            nRet = SUCCESS;

  if (!ppif)
    return EBADPARM;

  // Alloc memory for the object
  pme = *ppif = (AEESoundCtl *)AEE_NewClass((IBaseVtbl *)&gvtAEESoundCtl, sizeof(AEESoundCtl));
  if (!pme)
  {
    nRet = ENOMEMORY;
  }
  else
  {
    // register AEESoundCtl into object manager
    nRet = AEEObjectMgr_Register(pme, &pme->m_hObject);
    if(nRet != SUCCESS )
    {
      FREE(pme);
      return nRet;
    }

    pme->m_pac = (void*)AEE_GetAppContext();
    CALLBACK_Init(&pme->m_pcbExit,OEMSoundCtl_Delete, toInterface(pme));
    ISHELL_OnExit(ps, &pme->m_pcbExit);

    OEMSoundCtl_InitRsp((ISoundCtl*)pme);
    pme->m_nRefs = 1;
  }

#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  MEMSET(pme->m_DataBuf, 0, sizeof(pme->m_DataBuf));
  pme->m_bDataBufInUse = FALSE;
  pme->m_nUpdatePeriod = (uint16)SND_SPECTRUM_ANALYZER_MAX_PERIOD;
  pme->m_bSpectrumAnalyzer = FALSE;
#endif

  pme->m_pfnNotify = NULL;
  return nRet;
}

/*==============================================================================
FUNCTION:  OEMSoundCtl_Delete()
DESCRIPTION:
   Function to clean-up, unregister, and delete the OEMSoundCtl.
==============================================================================*/
static void OEMSoundCtl_Delete(ISoundCtl * po)
{
  AEESoundCtl * pme = (AEESoundCtl *) po;

  DBGPRINTF_HIGH("OEMSoundCtl: OEMSoundCtl_Delete() start, pme = 0x%x.", (pme) ? pme: 0);
  if(NULL != pme)
  {
    CALLBACK_Cancel(&pme->m_pcbExit);
    if(pme->m_hObject)
    {
      AEEObjectMgr_Unregister(pme->m_hObject);
    }

  // make sure if data callback been stopped
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
    if(pme->m_bSpectrumAnalyzer)
    {
      snd_spectrum_analyzer_ctl(pme->m_nUpdatePeriod, NULL, NULL, NULL);
    }
    pme->m_bSpectrumAnalyzer = FALSE;
    pme->m_nUpdatePeriod = (uint16)SND_SPECTRUM_ANALYZER_MAX_PERIOD;
#endif

    // Delete all the Rsps...
    OEMSoundCtl_FreeSoundCtlRsp(po);

    pme->m_hObject = NULL;
    pme->m_pfnNotify = NULL;
  }

  FREEIF(pme);
}

/*==================================================================
  Increase reference count
==================================================================*/
static uint32 OEMSoundCtl_AddRef(ISoundCtl * po)
{
   AEESoundCtl * pme = (AEESoundCtl *) po;

   return (++(pme->m_nRefs));
}

/*==================================================================
  Release reference count, delete when 0
==================================================================*/
static uint32 OEMSoundCtl_Release(ISoundCtl * po)
{
  AEESoundCtl * pme = (AEESoundCtl *) po;
  if (!pme)
    return EFAILED;

  // return ref count, if lareger than 0
  if (--pme->m_nRefs > 0)
    return pme->m_nRefs;

  OEMSoundCtl_Delete(po);
  return SUCCESS;
}

/*==================================================================
   This function retrieves a pointer to an interface conforming to the specified
   class ID.  This can be used to query for extended functionality, like future
   versions or proprietary extensions.
==================================================================*/
static int OEMSoundCtl_QueryInterface(ISoundCtl * po, AEECLSID idReq, void ** ppo)
{
  if (!po || !ppo)
    return EBADPARM;

  if (NULL != IQI_SELF(po, idReq, ppo, AEECLSID_SOUNDCTL))
  {
    IQI_AddRef(*ppo);
    return SUCCESS;
  }

  return ECLASSNOTSUPPORT;
}

/*==================================================================
  This function registers the callback function.
==================================================================*/
static int OEMSoundCtl_RegisterNotify(ISoundCtl * po, PFNSOUNDCTLNOTIFY pfn, const void * pUser)
{
  AEESoundCtl * pme = (AEESoundCtl *) po;
  if(po)
  {
    pme->m_pfnNotify = pfn;
    pme->m_pUser = (void *) pUser;
    return SUCCESS;
  }
  return EFAILED;
}

/*==================================================================
  Initialize all SoundCtlRsp within OEMSoundCtl
==================================================================*/
static void OEMSoundCtl_InitRsp(ISoundCtl * po)
{
   AEESoundCtl * pme = (AEESoundCtl *) po;
   SoundCtlRsp * pRsp;
   int         nLen;
   int         i;

   pRsp = pme->m_Rsp;
   nLen = OEMSOUNDCTL_RSP_MAX;

   for (i = 0; i < nLen; i++)
   {
      pRsp[i].bInUse = FALSE;
      pRsp[i].Notify.nCmd = -1;
      pRsp[i].Notify.nStatus = -1;
      pRsp[i].Notify.pSoundCtl = po;
      CALLBACK_Init(&pRsp[i].cb, (PFNNOTIFY)OEMSoundCtl_CallbackNotify, &pRsp[i]);
   }
}

/*==================================================================
  Cancel registered call backs
==================================================================*/
static void OEMSoundCtl_FreeSoundCtlRsp(ISoundCtl * po)
{
  AEESoundCtl * pme = (AEESoundCtl *) po;
  int           nLen = OEMSOUNDCTL_RSP_MAX;
  int           i;

  MUTEX_LOCK();
  for (i = 0; i < nLen; i++)
  {
    CALLBACK_Cancel(&pme->m_Rsp[i].cb);
  }
  MUTEX_UNLOCK();
}

/*==================================================================
// find first avaliable SoundCtlRsp object
==================================================================*/
SoundCtlRsp *OEMSoundCtl_GetCallback(ISoundCtl * po)
{
  AEESoundCtl * pme   = (AEESoundCtl *) po;
  SoundCtlRsp * pList = pme->m_Rsp;
  int           i;

  for (i = 0; i < OEMSOUNDCTL_RSP_MAX; i++)
  {
    if (!pList[i].bInUse)
    {
      pList[i].bInUse = TRUE;
      return &pList[i];
    }
  }
  return NULL;
}

/*==================================================================
   Called in BREW context,
==================================================================*/
static void OEMSoundCtl_CallbackNotify(SoundCtlRsp * pRsp)
{
  AEESoundCtl *   pme;
  ACONTEXT *pacRestore;
  
  if (!pRsp)
    return;

  pme = (AEESoundCtl*)pRsp->Notify.pSoundCtl;
  if (NULL == pme)
  {
    return;
  }

#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  if(AEE_SOUNDCTL_CMD_SPECTRUM_ANALYZER_DATA == pRsp->Notify.nCmd)
  {
    // set buffer flag to block any new updates
    pme->m_bDataBufInUse = TRUE;
  }
#endif //#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  // increase ref count before calling back to app
  (void)OEMSoundCtl_AddRef((ISoundCtl *)pme);

  pacRestore = (ACONTEXT*)AEE_EnterAppContext(pme->m_pac);
  if (NULL == pacRestore)
  {
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
    pme->m_bDataBufInUse = FALSE;
#endif //#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))

    (void)OEMSoundCtl_Release((ISoundCtl *)pme);
    DBGPRINTF_ERROR("OEMSoundCtl: AEE_EnterAppContext failed.");
    return;
  }

  // calls back to client
  if (pme->m_pfnNotify)
  {
    pme->m_pfnNotify(pme->m_pUser, (void *)&pRsp->Notify);
  }
  AEE_LeaveAppContext(pacRestore);

  // Free callback structure
  pRsp->bInUse = FALSE;
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  pme->m_bDataBufInUse = FALSE;
#endif //#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))

  // decrease ref count back
  (void)OEMSoundCtl_Release((ISoundCtl *)pme);
}

/*==================================================================

==================================================================*/
static int   OEMSoundCtl_GetParm(ISoundCtl * po, int nParamID, int32 * pP1, int32 * pP2)
{
  int ret = AEE_SOUNDCTL_FAILURE;
  if(!po) return ret;

  switch(nParamID)
  {
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
    case AEE_SOUNDCTL_PARM_EQUALIZER_NUM_BAND_MAX:
      *pP1 = (int32)OEMEQUALIZER_NUM_BAND_MAX;
      ret = AEE_SOUNDCTL_SUCCESS;
      break;
#endif

#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
    case AEE_SOUNDCTL_PARM_SPECTRUM_DATA_PERIOD:
      *pP1 = (int32)SND_SPECTRUM_ANALYZER_MIN_PERIOD;
      *pP2 = (int32)SND_SPECTRUM_ANALYZER_MAX_PERIOD;
      ret = AEE_SOUNDCTL_SUCCESS;
      break;
    case AEE_SOUNDCTL_PARM_SPECTRUM_DATA_MAX:
      *pP1 = (int32)OEMANALYZER_DATA_MAX;
      ret = AEE_SOUNDCTL_SUCCESS;
      break;
#endif
    default:
      ret = AEE_SOUNDCTL_UNSUPPORTED;
      break;
  }

  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_EnableEqualizer(ISoundCtl* po, boolean bEnable)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
  if(!po) return AEE_SOUNDCTL_FAILURE;

  snd_equalizer_ctl(bEnable, &OEMSoundCtl_EnableEqualizerCB, (const void*)toObject(po)->m_hObject);
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_SetEqualizer(ISoundCtl* po, AEESoundCtlEqualizerFilter* eqtype, uint32 bands)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
  int i;
  snd_equalizer_filter_type* eq;
  if(!po) return AEE_SOUNDCTL_FAILURE;

  eq = (snd_equalizer_filter_type *)MALLOC(sizeof(snd_equalizer_filter_type) * bands);
  if(eq)
  {
    for(i=0; i < bands; i++)
    {
      eq[i].freq = eqtype[i].nFreq;
      eq[i].gain = eqtype[i].nGain;
      eq[i].qf   = eqtype[i].nQF;
      eq[i].type = (snd_equalizer_filter_boost_enum_type)eqtype[i].dwType;
    }
    snd_set_equalizer(eq, bands, &OEMSoundCtl_SetEqualizerCB, (void*)toObject(po)->m_hObject);
    FREE(eq);
  }
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_SetEqualizerGain(ISoundCtl* po, uint16 band, int16 nGain)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
  if(!po) return AEE_SOUNDCTL_FAILURE;

  snd_equalizer_gain_ctl(band, nGain, &OEMSoundCtl_SetEqualizerGainCB, (const void*)toObject(po)->m_hObject);
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_EnableAudioExpander(ISoundCtl* po, boolean bEnable)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
  if(!po) return AEE_SOUNDCTL_FAILURE;

  snd_qconcert_ctl(bEnable, &OEMSoundCtl_EnableAudioExpanderCB, (const void*)toObject(po)->m_hObject);	
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_SetAudioExpander(ISoundCtl* po, AEESoundCtlAudioExpander* aepType)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
	snd_qconcert_params_type   qc;
  if(!po || !aepType) return AEE_SOUNDCTL_FAILURE;

  qc.mode     = (snd_qconcert_mode_enum_type)aepType->dwMode;
  qc.gain     = aepType->nGain;
  qc.spread   = aepType->nSpread;
  qc.geometry = aepType->nGeometry;

  snd_set_qconcert_params(&qc, &OEMSoundCtl_SetAudioExpanderCB, (void*)toObject(po)->m_hObject);
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int OEMSoundCtl_SetQconcertPlusReverbParams(ISoundCtl* po, AEESoundCtlAudioQconcertPlusReverb* aepType)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
  auddef_qconcert_plus_reverb_config_cmd_params_type qc;    
  if(!po || !aepType) return AEE_SOUNDCTL_FAILURE;

  qc.cmd_id = (auddef_qconcert_plus_reverb_config_cmd_type)aepType->nCmd;
  qc.data = aepType->nData;
  
  snd_set_qconcert_plus_reverb_params(&qc, &OEMSoundCtl_SetAudioQconcertPlusReverbCB, (void*)toObject(po)->m_hObject);
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}
/*==================================================================

==================================================================*/
static int  OEMSoundCtl_EnableAngledMixing(ISoundCtl* po, boolean bEnable)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;   
#if defined( FEATURE_AUDIO_QENSEMBLE)
#error code not present
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_SetAngledMixing(ISoundCtl* po, AEESoundCtlAngledMixing* aepType, uint16 nParams)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;   
#if defined( FEATURE_AUDIO_QENSEMBLE)
#error code not present
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_EnableSpectrumAnalyzer(ISoundCtl* po, boolean bEnable)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  AEESoundCtl *   pme = toObject(po);
  if(!po) return AEE_SOUNDCTL_FAILURE;

  if(bEnable)
  {
    // enable data callback
    snd_spectrum_analyzer_ctl(pme->m_nUpdatePeriod, &OEMSoundCtl_SpectrumAnalyzerDataCB,
                            &OEMSoundCtl_EnableSpectrumAnalyzerCB, (void*)pme->m_hObject);
  }
  else
  {
    // disable data callback by passing NULL
    snd_spectrum_analyzer_ctl(pme->m_nUpdatePeriod, NULL,
                            &OEMSoundCtl_EnableSpectrumAnalyzerCB, (void*)pme->m_hObject);
  }
  pme->m_bSpectrumAnalyzer = bEnable;
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
static int  OEMSoundCtl_SetSpectrumAnalyzer(ISoundCtl* po, uint16 update_period)
{
  int ret = AEE_SOUNDCTL_UNSUPPORTED;	
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
  AEESoundCtl * pme = toObject(po);
  if(!po) return AEE_SOUNDCTL_FAILURE;

  // save period for next time, enable/disable
  pme->m_nUpdatePeriod = update_period;
  // set will enable spectrum analyzer
  snd_spectrum_analyzer_ctl(pme->m_nUpdatePeriod, &OEMSoundCtl_SpectrumAnalyzerDataCB,
                            &OEMSoundCtl_SetSpectrumAnalyzerCB, (const void*)pme->m_hObject);
  ret = AEE_SOUNDCTL_SUCCESS;
#endif
  return ret;
}

/*==================================================================

==================================================================*/
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
static void OEMSoundCtl_EnableEqualizerCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_ENABLE_DISABLE_EQUALIZER);
}
#endif
/*==================================================================
Function: OEMSoundCtl_SetEqualizerCB

Description:
   callback from cmx for the status of SetEqualizer.

Prototype:
   static void OEMSoundCtl_SetEqualizerCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
static void OEMSoundCtl_SetEqualizerCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_SET_EQUALIZER);
}
#endif
/*==================================================================
Function: OEMSoundCtl_SetEqualizerGainCB

Description:
   callback from cmx for the status of SetEqualizerGain.

Prototype:
   static void OEMSoundCtl_SetEqualizerGainCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER))
static void OEMSoundCtl_SetEqualizerGainCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_SET_EQUALIZER_GAIN);
}
#endif
/*==================================================================
Function: OEMSoundCtl_EnableAudioExpanderCB

Description:
   callback from cmx for the status of EnableAudioExpander.

Prototype:
   static void OEMSoundCtl_EnableAudioExpanderCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
static void OEMSoundCtl_EnableAudioExpanderCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_ENABLE_DISABLE_AUDIO_EXPANDER);
}
#endif
/*==================================================================
Function: OEMSoundCtl_SetAudioExpanderCB

Description:
   callback from cmx for the status of SetAudioExpander.

Prototype:
   static void OEMSoundCtl_SetAudioExpanderCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
static void OEMSoundCtl_SetAudioExpanderCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_SET_AUDIO_EXPANDER);
}
#endif

/*==================================================================
Function: OEMSoundCtl_SetAudioQconcertPlusReverbCB

Description:
   callback from cmx for the status of SetAudioQconcertPlusReverb.

Prototype:
   static void OEMSoundCtl_SetAudioQconcertPlusReverbCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT))
static void OEMSoundCtl_SetAudioQconcertPlusReverbCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_SET_QCONCERT_PLUS_REVERB);
}
#endif

/*==================================================================
Function: OEMSoundCtl_EnableAngledMixingCB

Description:
   callback from cmx for the status of EnableAngledMixing.

Prototype:
   static void OEMSoundCtl_EnableAngledMixingCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if defined( FEATURE_AUDIO_QENSEMBLE)
#error code not present
#endif
/*==================================================================
Function: OEMSoundCtl_SetAngledMixingCB

Description:
   callback from cmx for the status of SetAngledMixing.

Prototype:
   static void OEMSoundCtl_SetAngledMixingCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if defined( FEATURE_AUDIO_QENSEMBLE)
#error code not present
#endif

/*==================================================================
Function: OEMSoundCtl_EnableSpectrumAnalyzerCB

Description:
   callback from cmx for the status of EnableSpectrumAnalyzer.

Prototype:
   static void OEMSoundCtl_EnableSpectrumAnalyzerCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
static void OEMSoundCtl_EnableSpectrumAnalyzerCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_ENABLE_DISABLE_SPECTRUM_ANALYZER);
}
#endif
/*==================================================================
Function: OEMSoundCtl_SetSpectrumAnalyzerCB

Description:
   callback from cmx for the status of SetSpectrumAnalyzer.

Prototype:
   static void OEMSoundCtl_SetSpectrumAnalyzerCB(const void *client_data, snd_status_type status);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
static void OEMSoundCtl_SetSpectrumAnalyzerCB(const void *client_data, snd_status_type status)
{
  OEMSoundCtl_ExecuteCallback(client_data, status, AEE_SOUNDCTL_CMD_SET_SPECTRUM_ANALYZER);
}
#endif

#if ((defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER)) || \
     (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT)) || \
     (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER)))
/*==================================================================
Function: OEMSoundCtl_ExecuteCallback

Description:
   Resume registered function in Brew to call back to client

Prototype:
   static void OEMSoundCtl_ExecuteCallback(const void *client_data, snd_status_type status, int16 cmd);

Parameter(s):
   client_data: ISoundCtl pointer carries back by cmx
   status: SND_SUCCESS or SND_FAILURE
   cmd : command types

Return Value:
   None.

Comments:
  ISoundCtl Object Handle will be verified first from Object Manager.
  If ISoundCtl Object is released, the callback does not do anything.

Side Effects:
   calls back to client.

See Also:
   None.

==================================================================*/
static void OEMSoundCtl_ExecuteCallback(const void *client_data, snd_status_type status, int16 cmd)
{
  AEESoundCtl * pme;
  SoundCtlRsp * pRsp;

  MUTEX_LOCK();
  pme = toObject(AEEObjectMgr_GetObject((AEEObjectHandle)client_data));
  if (pme)
  {
    pRsp = OEMSoundCtl_GetCallback(toInterface(pme));
    if(pRsp)
    {
      pRsp->Notify.nCmd = cmd;
      pRsp->Notify.nStatus = OEMSoundCtl_GetAEESoundStatus(status);
      pRsp->Notify.pData = NULL;
      pRsp->Notify.dwDataSize = 0;
      pRsp->Notify.pSoundCtl = toInterface(pme);

#if MIN_BREW_VERSION(3,0)
      AEE_ResumeCallback((AEECallback *)pRsp, pme->m_pac);
#else
      AEE_ResumeEx((AEECallback *)pRsp, 0, pme->m_pac);
#endif /* MIN_BREW_VERSION(3,0) */

    }
  }
  MUTEX_UNLOCK();
}
#endif
/*==================================================================
Function: OEMSoundCtl_SpectrumAnalyzerDataCB

Description:
   callback from cmx Spectrum Analyzer data.

Prototype:
   static void OEMSoundCtl_SpectrumAnalyzerDataCB(const uint16 *data);

Parameter(s):
   data: Spectrum Analyzer data

Return Value:
   None.

Comments:
  ISoundCtl Object Handle will be verified first from Object Manager.
  If ISoundCtl Object is released, the callback does not do anything.

Side Effects:
   calls back to client.

See Also:
   None.
==================================================================*/
#if (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER))
static void OEMSoundCtl_SpectrumAnalyzerDataCB(const uint16 *data, void *client_data)
{
  AEESoundCtl * pme;
  SoundCtlRsp * pRsp;

  MUTEX_LOCK();
  pme = toObject(AEEObjectMgr_GetObject((AEEObjectHandle)client_data));
  if(pme)
  {
    // callback only when data baffer is not in use
    if(FALSE == pme->m_bDataBufInUse)
    {
      // copy to local buffer
      MEMCPY(pme->m_DataBuf, data, OEMANALYZER_DATA_SIZE*sizeof(uint16));
      pRsp = OEMSoundCtl_GetCallback(toInterface(pme));
      if(pRsp)
      {
        pRsp->Notify.nCmd = AEE_SOUNDCTL_CMD_SPECTRUM_ANALYZER_DATA;
        pRsp->Notify.pData = (void*)pme->m_DataBuf;
        pRsp->Notify.nStatus = AEE_SOUNDCTL_SUCCESS;
        pRsp->Notify.dwDataSize = OEMANALYZER_DATA_SIZE;
        pRsp->Notify.pSoundCtl = toInterface(pme);

#if MIN_BREW_VERSION(3,0)
        AEE_ResumeCallback((AEECallback *)pRsp, pme->m_pac);
#else
        AEE_ResumeEx((AEECallback *)pRsp, 0, pme->m_pac);
#endif /* MIN_BREW_VERSION(3,0) */
      }
    }
    else
    {
      ;//DBGPRINTF_HIGH("OEMSoundCtl: SPA data dropped.");
    }
  }

  MUTEX_UNLOCK();
}
#endif

// helper functions to convert back and forth between ISoundCtl and AEESoundCtl
// no any checking, use carefully
ISoundCtl* toInterface(AEESoundCtl* pme)
{
  return (ISoundCtl*)pme;
}
#if ((defined( FEATURE_AUDIO_EQUALIZER) && defined( FEATURE_AUDFMT_EQUALIZER)) || \
     (defined( FEATURE_AUDIO_QCONCERT) && defined( FEATURE_AUDFMT_QCONCERT)) || \
     (defined( FEATURE_AUDIO_SPECTRUM_ANALYZER) && defined( FEATURE_AUDFMT_SPECTRUM_ANALYZER)))

/*==================================================================
  Convert status type from cmx to AEESoundCtl
==================================================================*/
static int OEMSoundCtl_GetAEESoundStatus(snd_status_type e)
{
  int eResult;
  switch(e)
  {
    case SND_SUCCESS:
      eResult = AEE_SOUNDCTL_SUCCESS;
      break;
    case SND_FAILURE:
      eResult = AEE_SOUNDCTL_FAILURE;
      break;
    default:
      eResult = AEE_SOUNDCTL_UNKNOWN;
      break;
  }
  return eResult;
}

AEESoundCtl* toObject(ISoundCtl* po)
{
  return (AEESoundCtl*)po;
}
#endif
#endif // FEATURE_AUDIO_EQUALIZER || FEATURE_AUDIO_QCONCERT || FEATURE_AUDIO_SPECTRUM_ANALYZER
/* ============= End of File =============================================== */
