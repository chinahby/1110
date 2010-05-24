/*==============================================================================
FILE:          OEMVR.c

SERVICES:      Voice recognition and related services

DESCRIPTION:   Definitions for the OEMVR extension to the IVR interface


        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMVR.c#13 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/06   jas     Handling file rename for BREW 4.0
05/18/06   jks     Merged ACM changes into MSMSHARED version
05/16/05   sun     Fixed case for include files
07/20/04   sun     Fixed length in Strlcpy
==============================================================================*/

#include "OEMFeatures.h"

#ifdef FEATURE_PHONE_VR

#include "comdef.h"
#include "AEE_OEM.h"
#ifdef FEATURE_BREW_2_0
#include "AEEStaticStdLib.h"
#endif
#include "AEEFile.h"
#include "AEEVR.h"
#include "OEMVR_priv.h"
#include "OEMHeap.h"
#include "vs.h"
#include "vdbrw.h"
#include "vdbvtag.h"
#include "vdbdata.h"
#include "task.h"
#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#include "BREWVersion.h"
#if MIN_BREW_BUILD(4,0,1,5)
#include "AEEConstFile.h"
#else
#include "AEE_OEMFile.h"
#endif /* MIN_BREW_BUILD(4,0,1,5) */
#endif /* FEATURE_BREW_3_0 */

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#endif // FEATURE_ACM

/*------------------------------------------------------------------------------
   Type Definitions
------------------------------------------------------------------------------*/

// Asynchronous operations
typedef enum
{
   OEMVRASYNCOP_NONE,
   OEMVRASYNCOP_TRAIN_VTAG,
   OEMVRASYNCOP_RECOGNIZE_PRIMITIVE,
   OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS,
   OEMVRASYNCOP_YES_NO_CONFIRM,
   OEMVRASYNCOP_MENU_SELECT,
   OEMVRASYNCOP_PLAY_UTTERANCE,
   OEMVRASYNCOP_PLAY_SOUNDS,
   OEMVRASYNCOP_TRAIN_CWORD,
   OEMVRASYNCOP_RECOGNIZE_MULTI,
   OEMVRASYNCOP_VOICE_ACTIVATE,
} OEMVRAsyncOp;

// Number of RW lists that can be set in IVR interface structure, for passing
// to VS training and recognizing functions
#define NUM_RW_LISTS 2

// Implementation of IVR interface
struct IVR
{
union {
    AEEVTBL(IVR) *        pvt;
#ifndef FEATURE_BREW_3_0
    AEESysObject          m_so;
#endif
  } u;
   void *                   m_pac;  // App context in which this IVR object is created

   int32                    nRef;
   IShell *                 pIShell;

   // Current state information
   AEEVRStatus              wStatus;
   AEEVRFailure             wFailure;
   AEEVRUtterance           wUtterance;

   // Current asynchronous operation
   OEMVRAsyncOp             wAsyncOp;

   // User-registered notify callback
   PFNVRSTATUS              pUserCallback;
   void *                   pUserCallbackData;

   // RecognizeWithPrompts configuration
   boolean                  bRecognizeWithPromptsWait;
   boolean                  bRecognizeWithPromptsNBest;
   int                      wRecognizeWithPromptsMatchCount;

   // VS configuration data
   char                     szVSClientDBPath[VDB_CLIENT_NAME_MAX_LENGTH + 1];
   char                     szVSLanguage[VDB_LANGUAGE_NAME_MAX_LENGTH + 1];
   vs_patt_settings_type    wVSPatternSet;

   // Callback to invoke from VS callback, to switch to Brew context
   AEECallback              ProcessVSCB;

   // VS callback data
   vs_status_type           wVSStatus;
   vdb_rwid_type            wVSUtterance;
   short                    wVSValidMatch;

   // VS prompt data (used by training and recognizing functions)
   vs_sound_type *          pPrompt[OEMVRPROMPT_LIMIT];

   // VS RW list data (used by training and recognizing functions)
   vdb_rw_list_type         RWList[NUM_RW_LISTS];
   vdb_rw_list_type *       pRWList[NUM_RW_LISTS + 1];

   // VS sound data (used by vs_play_multi)
   vs_sound_type *          pSound;

   // VS utterance argument (used by vs_play_utterance, vs_train_control_word,
   // and vs_voice_activate)
   vdb_rwid_type            wUtteranceArg;

   // VS multiple recognition type argument (used by vs_recognize_multi)
   vs_rec_multi_type        wMultRecArg;

   // VS language iterator data
   vdb_langi_type           LanguageIter;
   boolean                  bLanguageIterValid;

   // VS vtag iterator data
   vdb_vtagi_type           VtagIter;
   boolean                  bVtagIterValid;

#if defined (FEATURE_ACM)
#if defined (FEATURE_SIRIUS)
#error code not present
#endif // FEATURE_SIRIUS
   ACM_Callback_data_type   mAcmCB;
   boolean                  bResourceAcquired;
#endif // FEATURE_ACM
};


/*------------------------------------------------------------------------------
   Prototypes
------------------------------------------------------------------------------*/

// Static functions called internally in this file only
static void SetCurrentInstance(IVR*);
static void SetAsyncOp(IVR*, OEMVRAsyncOp);
static void AbortAsyncOp(IVR*);
static void SetAsyncResult(IVR*, AEEVRStatus, AEEVRFailure, AEEVRUtterance);
static int CreateVSSoundArray(vs_sound_type**, const OEMVRSound*);
static int CreateVSRWList(vdb_rw_list_type*, const AEEVRUtterance*, AEEVRUtterance);
static vdb_rwid_type ConvertToVSRW(AEEVRUtterance);
static AEEVRUtterance ConvertToAEEVRUtterance(vdb_rwid_type);
static void ProcessVSCallback(void*);
static void OEMVR_vs_play_cb(vs_status_type);
static void OEMVR_vs_train_cb(vs_status_type, vdb_rwid_type, vs_handshake_cb_type);
static void OEMVR_vs_rec_cb(vs_status_type, vdb_rwid_type, short, vs_handshake_cb_type);
static void StartVSOperation(IVR*);

#if defined (FEATURE_ACM)
static int ACM_Acquire(IVR*, OEMVRAsyncOp);
static void ACM_StateChangedCB(ACM_Callback_data_type*);
#endif // FEATURE_ACM

// Static functions called externally through IVR virtual function table
static uint32 OEMVR_AddRef(IVR*);
static uint32 OEMVR_Release(IVR*);
static int OEMVR_QueryInterface(IVR*, AEECLSID, void**);
static int OEMVR_TrainVoiceTag(IVR*, const AEEVRUtterance*);
static int OEMVR_RecognizePrimitive(IVR*, const AEEVRUtterance*);
static int OEMVR_RecognizeWithPrompts(IVR*, const AEEVRUtterance*, boolean, uint8);
static int OEMVR_YesNoConfirm(IVR*);
static int OEMVR_VoiceMenuSelect(IVR*, const AEEVRUtterance*);
static int OEMVR_PlayUtterance(IVR*, AEEVRUtterance);
static int OEMVR_AbortOperation(IVR*);
static int OEMVR_RegisterNotify(IVR*, PFNVRSTATUS, void*);
static AEEVRStatus OEMVR_GetOperationStatus(IVR*, AEEVRFailure*, AEEVRUtterance*);
static int OEMVR_LanguageEnumInit(IVR*);
static boolean OEMVR_LanguageEnumNext(IVR*, AECHAR*, uint32);
static int OEMVR_GetLanguage(IVR*, AECHAR*, uint32);
static int OEMVR_SetLanguage(IVR*, const AECHAR*);
static int OEMVR_VoiceTagEnumInit(IVR*);
static boolean OEMVR_VoiceTagEnumNext(IVR*, AEEVRUtterance*);
static int OEMVR_RemoveVoiceTag(IVR*, AEEVRUtterance);
static int OEMVR_GetUtteranceLabel(IVR*, AEEVRUtterance, uint8*, uint32*);
static int OEMVR_SetUtteranceLabel(IVR*, AEEVRUtterance, const uint8*, uint32);
static uint32 OEMVR_GetUtteranceDuration(IVR*, AEEVRUtterance);
static int OEMVR_GetProperties(IVR*, AEEVRProperties*);
static int OEMVR_SetVoiceTagDB(IVR*, const char*);


/*------------------------------------------------------------------------------
   Data Objects
------------------------------------------------------------------------------*/

// Virtual function table for IVR interface
static const AEEVTBL(IVR) gOEMVRFuncs =
{
   OEMVR_AddRef,
   OEMVR_Release,
   OEMVR_QueryInterface,
   OEMVR_TrainVoiceTag,
   OEMVR_RecognizePrimitive,
   OEMVR_RecognizeWithPrompts,
   OEMVR_YesNoConfirm,
   OEMVR_VoiceMenuSelect,
   OEMVR_PlayUtterance,
   OEMVR_AbortOperation,
   OEMVR_RegisterNotify,
   OEMVR_GetOperationStatus,
   OEMVR_LanguageEnumInit,
   OEMVR_LanguageEnumNext,
   OEMVR_GetLanguage,
   OEMVR_SetLanguage,
   OEMVR_VoiceTagEnumInit,
   OEMVR_VoiceTagEnumNext,
   OEMVR_RemoveVoiceTag,
   OEMVR_GetUtteranceLabel,
   OEMVR_SetUtteranceLabel,
   OEMVR_GetUtteranceDuration,
   OEMVR_GetProperties,
   OEMVR_SetVoiceTagDB,
};

// Implementation-dependent properties
static const AEEVRProperties gProperties =
{
   sizeof(AEEVRProperties),            // wStructSize
   (VDB_LANGUAGE_NAME_MAX_LENGTH + 1), // wLanguageChars
   VDB_MAX_LABEL_BYTES                 // wLabelBytes
};

// Currently "active" instance of IVR
static IVR *pCurrentInstance = NULL;

// Current VS configuration data
static char szVSClientDBPath[VDB_CLIENT_NAME_MAX_LENGTH + 1] = {'\0'};
static char szVSLanguage[VDB_LANGUAGE_NAME_MAX_LENGTH + 1] = {'\0'};
static vs_patt_settings_type wVSPatternSet = VS_PATT_SD_SI;

// Filename prefix to use for default voice tag databases
// HACK:  With the current file system, vdb requires the total pathname length
// to be odd, so this filename prefix gets truncated by one character if
// necessary to make this happen.
static const char *szVtagDBPrefix = "vdb_";

// Language to use by default
static char szDefaultLanguage[VDB_LANGUAGE_NAME_MAX_LENGTH + 1] = {'\0'};

// Data for default prompts
static const vs_sound_type Again[] =
{
   { SND_VOICE_PROMPT,    0,  RWID_AGAIN,               0 },
   { SND_HFK_TONE2,     100,  0,                        0 },
   { SND_STOP,            0,  0,                        0 },
};

static const vs_sound_type ErrorTryAgain[] =
{
   { SND_VOICE_PROMPT,    0,  RWID_BEEP,                0 },
   { SND_VOICE_PROMPT,    0,  RWID_AGAIN,               0 },
   { SND_HFK_TONE2,     100,  0,                        0 },
   { SND_STOP,            0,  0,                        0 },
};

static const vs_sound_type WasThatYes[] =
{
   { SND_VOICE_PROMPT,    0,  RWID_WAS_THAT_A_YES,      0 },
   { SND_HFK_TONE2,     100,  0,                        0 },
   { SND_STOP,            0,  0,                        0 },
};

static const vs_sound_type DidYouSayRWID[] =
{
   { SND_VOICE_PROMPT,    0,  RWID_NBEST_CONFIRMATION,  0 },
   { SND_VOICE_PROMPT,    0,  VDB_RWID_NONE,            0 },
   { SND_HFK_TONE2,     100,  0,                        0 },
   { SND_STOP,            0,  0,                        0 },
};

static const vs_sound_type DoYouPickRWID[] =
{
   { SND_VOICE_PROMPT,    0,  RWID_BEEP,                0 },
   { SND_VOICE_PROMPT,    0,  VDB_RWID_NONE,            0 },
   { SND_VOICE_PROMPT,    0,  RWID_WAS_THAT_A_YES,      0 },
   { SND_HFK_TONE2,     100,  0,                        0 },
   { SND_STOP,            0,  0,                        0 },
};

static const vs_sound_type ErrorBeep[] =
{
   { SND_VOICE_PROMPT,    0,  RWID_BEEP,                0 },
   { SND_STOP,            0,  0,                        0 },
};

static const vs_sound_type HFKTone[] =
{
   { SND_HFK_TONE1,      80,  0,                        0 },
   { SND_STOP,            0,  0,                        0 },
};

// Default prompts
static const vs_sound_type *const pDefaultPrompt[OEMVRPROMPT_LIMIT] =
{
   NULL,                // OEMVRPROMPT_NONE - this array index not used

   // Starting offset for vs_train_utterance and vs_train_control_word
   NULL,                // OEMVRPROMPT_TRAIN_INITIAL
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_INITIAL_TIMEOUT
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_INITIAL_TOO_SOON
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_INITIAL_TOO_LONG
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_INITIAL_TOO_SHORT
   Again,               // OEMVRPROMPT_TRAIN_2ND_TRY
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_2ND_TRY_TIMEOUT
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_2ND_TRY_TOO_SOON
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_2ND_TRY_TOO_LONG
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_2ND_TRY_TOO_SHORT
   ErrorTryAgain,       // OEMVRPROMPT_TRAIN_UNMATCHED

   // Starting offset for vs_recognize_utterance
   NULL,                // OEMVRPROMPT_RECOGNIZE_INITIAL
   ErrorTryAgain,       // OEMVRPROMPT_RECOGNIZE_INITIAL_TIMEOUT
   ErrorTryAgain,       // OEMVRPROMPT_RECOGNIZE_INITIAL_TOO_SOON
   ErrorTryAgain,       // OEMVRPROMPT_RECOGNIZE_INITIAL_TOO_LONG
   ErrorTryAgain,       // OEMVRPROMPT_RECOGNIZE_INITIAL_TOO_SHORT
   ErrorTryAgain,       // OEMVRPROMPT_RECOGNIZE_INITIAL_UNMATCHED
   DidYouSayRWID,       // OEMVRPROMPT_RECOGNIZE_YESNO
   DidYouSayRWID,       // OEMVRPROMPT_RECOGNIZE_YESNO_TIMEOUT
   WasThatYes,          // OEMVRPROMPT_RECOGNIZE_YESNO_TOO_SOON
   WasThatYes,          // OEMVRPROMPT_RECOGNIZE_YESNO_TOO_LONG
   WasThatYes,          // OEMVRPROMPT_RECOGNIZE_YESNO_TOO_SHORT
   WasThatYes,          // OEMVRPROMPT_RECOGNIZE_YESNO_UNMATCHED
   NULL,                // OEMVRPROMPT_RECOGNIZE_YESNO_CONFIRM

   // Starting offset for vs_implicit_yes
   NULL,                // OEMVRPROMPT_YESNO_INITIAL
   WasThatYes,          // OEMVRPROMPT_YESNO_INITIAL_TIMEOUT
   WasThatYes,          // OEMVRPROMPT_YESNO_INITIAL_TOO_SOON
   WasThatYes,          // OEMVRPROMPT_YESNO_INITIAL_TOO_LONG
   WasThatYes,          // OEMVRPROMPT_YESNO_INITIAL_TOO_SHORT
   WasThatYes,          // OEMVRPROMPT_YESNO_INITIAL_UNMATCHED
   WasThatYes,          // OEMVRPROMPT_YESNO_YESNO_TIMEOUT

   // Starting offset for vs_menu_selection
   NULL,                // OEMVRPROMPT_MENU_INITIAL
   DoYouPickRWID,       // OEMVRPROMPT_MENU_UNMATCHED

   // Starting offset for vs_recognize_multi
   NULL,                // OEMVRPROMPT_MULTI_INITIAL
   ErrorBeep,           // OEMVRPROMPT_MULTI_UNMATCHED
   ErrorBeep,           // OEMVRPROMPT_MULTI_TIMEOUT
   NULL,                // OEMVRPROMPT_MULTI_CORRESPONDENT

   // Starting offset for vs_voice_activate
   HFKTone,             // OEMVRPROMPT_ACTIVATE_1ST_CONFIRM
   NULL,                // OEMVRPROMPT_ACTIVATE_2ND_CONFIRM
};


/*------------------------------------------------------------------------------
   Functions
------------------------------------------------------------------------------*/

/*==============================================================================
Description:
   This function sets the given IVR instance as the current instance, aborting
   any asynchronous operation on the previous current instance if necessary.
==============================================================================*/
static void SetCurrentInstance(IVR *pIVR)
{
   if (pIVR != pCurrentInstance)
   {
      if (pCurrentInstance != NULL)
      {
         // Abort asynchronous operations in current instance if needed
         if (pCurrentInstance->wAsyncOp != OEMVRASYNCOP_NONE)
         {
            AbortAsyncOp(pCurrentInstance);
            SetAsyncResult(pCurrentInstance, AEEVRSTATUS_ABORTED,
                           AEEVRFAILURE_NONE, AEEVRUTTERANCE_NONE);
         }

         // Flag VS iterators on current instance no longer valid
         pCurrentInstance->bLanguageIterValid = FALSE;
         pCurrentInstance->bVtagIterValid = FALSE;
      }

      if (pIVR != NULL)
      {
         // Configure VS for appropriate voice tag DB path if needed
         if (STRCMP(pIVR->szVSClientDBPath, szVSClientDBPath))
         {
            // Set path
            if (vdb_set_current_client_db(pIVR->szVSClientDBPath) != VDB_NOERR)
            {
               // Should never happen, since path is prevalidated
               DBGPRINTF("VR: Error setting db (%s)", pIVR->szVSClientDBPath);
            }
            STRLCPY(szVSClientDBPath, pIVR->szVSClientDBPath, STRLEN(pIVR->szVSClientDBPath) +1 );
         }

         // Configure VS for appropriate language if needed
         if (STRCMP(pIVR->szVSLanguage, szVSLanguage))
         {
            // Set language
            if (vdb_set_current_language(pIVR->szVSLanguage) != VDB_NOERR)
            {
               // Should never happen, since language is prevalidated
               DBGPRINTF("VR: Error setting language (%s)", pIVR->szVSLanguage);
            }
            STRLCPY(szVSLanguage, pIVR->szVSLanguage, STRLEN(pIVR->szVSLanguage) + 1);
         }

         // Configure VS for appropriate pattern settings if needed
         if (pIVR->wVSPatternSet != wVSPatternSet)
         {
            // Set pattern setting
            vs_set_vr_engine(pIVR->wVSPatternSet);
            wVSPatternSet = pIVR->wVSPatternSet;
         }
      }

      // Set new instance as current
      pCurrentInstance = pIVR;
   }
}

/*==============================================================================
Description:
   This function sets up an asynchronous operation with the given callback.
==============================================================================*/
static void SetAsyncOp(IVR *pIVR, OEMVRAsyncOp wAsyncOp)
{
   // Set this IVR instance as currently active
   SetCurrentInstance(pIVR);

   // Abort current asynchronous operation if needed
   if (pIVR->wAsyncOp != OEMVRASYNCOP_NONE)
   {
      AbortAsyncOp(pIVR);
      // Note:  The user callback function is not invoked after this abort,
      // since it could be ambiguous whether it was supposed to apply to the
      // old async op or the new one.
   }

   // Set current status and async operation
   pIVR->wStatus = AEEVRSTATUS_RUNNING;
   pIVR->wFailure = AEEVRFAILURE_NONE;
   pIVR->wUtterance = AEEVRUTTERANCE_NONE;
   pIVR->wAsyncOp = wAsyncOp;
}

/*==============================================================================
Description:
   This function aborts the current asynchronous operation (without invoking
   the callback).
==============================================================================*/
static void AbortAsyncOp(IVR *pIVR)
{
   // Call appropriate function to abort the current async op if needed
#if defined (FEATURE_ACM)
   if ((pIVR == pCurrentInstance) && (pIVR->bResourceAcquired))
#else
   if (pIVR == pCurrentInstance)
#endif
   {
      switch (pIVR->wAsyncOp)
      {
      case OEMVRASYNCOP_NONE:
         break;
      case OEMVRASYNCOP_PLAY_UTTERANCE:
      case OEMVRASYNCOP_PLAY_SOUNDS:
         vs_play_abort(NULL);
         break;
      case OEMVRASYNCOP_TRAIN_VTAG:
      case OEMVRASYNCOP_TRAIN_CWORD:
         vs_train_abort(NULL);
         break;
      case OEMVRASYNCOP_RECOGNIZE_PRIMITIVE:
      case OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS:
      case OEMVRASYNCOP_YES_NO_CONFIRM:
         vs_recognize_abort(NULL);
         break;
      case OEMVRASYNCOP_MENU_SELECT:
         vs_menu_selection_abort(NULL);
         break;
      case OEMVRASYNCOP_RECOGNIZE_MULTI:
         vs_recognize_multi_abort(NULL);
         break;
      case OEMVRASYNCOP_VOICE_ACTIVATE:
         vs_voice_activate_abort(NULL);
         break;
      }
   }

   // Set current status and async operation
   pIVR->wStatus = AEEVRSTATUS_ABORTED;
   pIVR->wFailure = AEEVRFAILURE_NONE;
   pIVR->wUtterance = AEEVRUTTERANCE_NONE;
   pIVR->wAsyncOp = OEMVRASYNCOP_NONE;

   // Cancel any callback that may have gotten scheduled before op was aborted
   CALLBACK_Cancel(&pIVR->ProcessVSCB);
}

/*==============================================================================
Description:
   This function establishes a final or intermediate result for the current
   asynchronous operation, invoking the callback to report it.
==============================================================================*/
static void SetAsyncResult(IVR *pIVR, AEEVRStatus wStatus,
                           AEEVRFailure wFailure, AEEVRUtterance wUtterance)
{
   // Set result in IVR structure
   pIVR->wStatus = wStatus;
   pIVR->wFailure = wFailure;
   pIVR->wUtterance = wUtterance;

   // Complete async operation if appropriate
   if (wStatus != AEEVRSTATUS_RUNNING)
   {
      pIVR->wAsyncOp = OEMVRASYNCOP_NONE;

#if defined (FEATURE_ACM)
#if defined (FEATURE_SIRIUS)
#error code not present
#else // !FEATURE_SIRIUS
      OEMACM_PriorityMgr_Free (pIVR);
#endif // FEATURE_SIRIUS
      pIVR->bResourceAcquired = FALSE;
#endif // FEATURE_ACM
   }

   // Invoke user callback function if registered
   //We have to ensure that that right app context is set
   //before dispatching this CB function. Also, it must be dispatched in the
   //BREW Task. The one and only safe way to ensure both of the above is to use
   //an AEECallback to track and dispatch this CB function. Though the app passes
   //a PFN, internally (inside this file) use an AEECallback to store it, track it and
   //dispatch it. We need to ensure that when the app that registered the PFN goes away,
   //the CB is also cancelled.  Ensure that we are always in BREW Task here. Then,
   //set the right app-context and dispatch into app

#ifndef FEATURE_BREW_3_0
   if (OEMOS_ActiveTaskID() != (uint32)&ui_tcb)
      return;
#endif

   if (pIVR->pUserCallback)
   {
#ifdef FEATURE_BREW_3_0
     void * paLast = AEE_EnterAppContext(pIVR->m_pac);
#else
      void *   paLast = AEE_SetAppContext(pIVR->m_pac);
#endif
      (*pIVR->pUserCallback)(pIVR->pUserCallbackData,
                             wStatus, wFailure, wUtterance);
#ifdef FEATURE_BREW_3_0
      AEE_LeaveAppContext(paLast);
#else
      AEE_SetAppContext(paLast);
#endif

   }
}

/*==============================================================================
Description:
   This utility function creates a vs_sound_type array at the given pointer and
   fills it with data from an OEMVRSound array.  If the given pointer already
   points to a non-NULL array, that array is first freed.
==============================================================================*/
static int CreateVSSoundArray(vs_sound_type **ppVSSoundArray,
                              const OEMVRSound *pSoundArray)
{
   const OEMVRSound *p;
   vs_sound_type *s;
   int count;

   if(ppVSSoundArray != NULL)
   FREEIF(*ppVSSoundArray);

   // Error if input array is empty
   if (pSoundArray == NULL)
   {
      return EFAILED;
   }

   // Loop through items in pSoundArray, checking for errors in the process
   for (p = pSoundArray;
        (p->wUtt != AEEVRUTTERANCE_NONE) || (p->pToneData != NULL);
        p++)
   {
      if ((p->wUtt != AEEVRUTTERANCE_NONE) &&
          ((p->wUtt & AEEVRUTTERANCE_TYPE_MASK) == AEEVRUTTERANCE_NONE))
      {
         // Illegal utterance
         return EFAILED;
      }
      if ((p->pToneData != NULL) &&
          ((p->pToneData->eTone <= AEE_TONE_FIRST) ||
           (p->pToneData->eTone >= AEE_TONE_LAST)))
      {
         // Illegal tone
         return EFAILED;
      }
      if ((p->wUtt != AEEVRUTTERANCE_NONE) && (p->pToneData != NULL))
      {
         // Illegal to have both an utterance and a sound
         return EFAILED;
      }
   }

   // Count is number of items looped through in pSoundArray, plus terminator
   count = (p - pSoundArray) + 1;

   // Allocate pointer to VS sound array
   *ppVSSoundArray = (vs_sound_type *)MALLOC(count * sizeof(vs_sound_type));

   if (*ppVSSoundArray == NULL)
   {
      return ENOMEMORY;
   }

   // Initialize VS sound array with data from pSoundArray
   for (p = pSoundArray, s = *ppVSSoundArray; count-- > 0; p++, s++)
   {
      if (p->wUtt != AEEVRUTTERANCE_NONE)
      {
         s->sound = SND_VOICE_PROMPT;
         s->utt_id = ConvertToVSRW(p->wUtt);
         s->utt_index = 0;
      }
      else if (p->pToneData != NULL)
      {
         s->sound = (p->pToneData->eTone == AEE_TONE_SILENCE) ?
                    SND_SILENCE :
                    SND_FIRST_TDB_TONE + (uint16)p->pToneData->eTone;
         s->duration = p->pToneData->wDuration;
      }
      else
      {
         s->sound = SND_STOP;
      }
   }

   return SUCCESS;
}

/*==============================================================================
Description:
   This utility function creates a vdb_rwid_type array at the given
   vdb_rw_list_type and fills it with data from an AEEVRUtterance array.  If
   the given list already points to a non-NULL array, that array is first freed.

   If wFilterType is AEEVRUTTERANCE_TYPE_VOICETAG, the destination RW list is
   filled with the voice tags from the source array or with all the voice tags
   if the source array is NULL, and an error is returned if the source array
   contains anything other than recognizable utterances.

   If wFilterType is AEEVRUTTERANCE_TYPE_CWORD, the destination RW list is
   filled with the control words from the source array, and an error is
   returned if the source array contains anything other than recognizable
   utterances.

   If wFilterType is AEEVRUTTERANCE_TYPE_PROMPT, the destination RW list is
   filled with all the data from the source array, and an error is returned if
   the source array contains anything other than recognizable utterances or
   prompts.
==============================================================================*/
static int CreateVSRWList(vdb_rw_list_type *pRWList,
                          const AEEVRUtterance *pUttList,
                          AEEVRUtterance wFilterType)
{
   vdb_rwid_type *array;
   int count = 0;

   // Free old RWID array if necessary, and reset count
   if(pRWList != NULL)
   {
     FREEIF(pRWList->list);
     pRWList->num_rws = 0;
   }

   // Count items to put in list, checking for errors in the process
   if (pUttList)
   {
      const AEEVRUtterance *p;

      // Loop through items in pUttList up to terminator element
      for (p = pUttList; *p != AEEVRUTTERANCE_NONE; p++)
      {
         AEEVRUtterance wUttType = (*p & AEEVRUTTERANCE_TYPE_MASK);

         if ((wUttType != AEEVRUTTERANCE_TYPE_CWORD) &&
             (wUttType != AEEVRUTTERANCE_TYPE_VOICETAG) &&
             ((wFilterType != AEEVRUTTERANCE_TYPE_PROMPT) ||
              (wUttType != AEEVRUTTERANCE_TYPE_PROMPT)))
         {
            // Illegal utterance
            return EFAILED;
         }

         // count if appropriate type
         if ((wFilterType == AEEVRUTTERANCE_TYPE_PROMPT) ||
             (wUttType == wFilterType))
         {
            count++;
         }
      }
   }
   else if (wFilterType == AEEVRUTTERANCE_TYPE_VOICETAG)
   {
      vdb_vtagi_type iter;
      vdb_rwid_type utt;
      vdb_err_type err;

      // Initialize iterator to loop through all voice tags
      if (vdb_vtagi_init(&iter) != VDB_NOERR)
      {
         return ENOMEMORY;
      }

      // Count all voice tags
      while ((err = vdb_vtagi_next(&iter, &utt)) != VDB_ERR_ITERATION_COMPLETE)
      {
         if (err != VDB_NOERR)
         {
            return ENOMEMORY;
         }

         count++;
      }
   }

   // Allocate new RWID array (or just use NULL if count is zero)
   if (count)
   {
      array = (vdb_rwid_type *)MALLOC(count * sizeof(vdb_rwid_type));

      if (array == NULL)
      {
         return ENOMEMORY;
      }
   }
   else
   {
      array = NULL;
   }

   // Fill new RWID array, using all voice tags if pUttList is NULL
   if (pUttList)
   {
      const AEEVRUtterance *p;
      int i = 0;

      // Loop through pUttList
      for (p = pUttList; *p != AEEVRUTTERANCE_NONE; p++)
      {
         AEEVRUtterance wUttType = (*p & AEEVRUTTERANCE_TYPE_MASK);

         // Fill array with elements of appropriate type
         if ((wFilterType == AEEVRUTTERANCE_TYPE_PROMPT) ||
             (wUttType == wFilterType))
         {
            array[i++] = ConvertToVSRW(*p);
         }
      }
   }
   else if (wFilterType == AEEVRUTTERANCE_TYPE_VOICETAG)
   {
      vdb_vtagi_type iter;
      int i;

      // Initialize iterator
      if (vdb_vtagi_init(&iter) != VDB_NOERR)
      {
         // Shouldn't happen, but return error just in case
         FREEIF(array);
         return ENOMEMORY;
      }

      // Iterate to fill array
      for (i = 0; i < count; i++)
      {
         if (vdb_vtagi_next(&iter, &array[i]) != VDB_NOERR)
         {
            // Shouldn't happen, but return error just in case
            FREEIF(array);
            return ENOMEMORY;
         }
      }
   }

   if(pRWList != NULL)
   {
     // Set up new list
     pRWList->list = array;
     pRWList->num_rws = count;
   }

   return SUCCESS;
}

/*==============================================================================
Description:
   This utility function returns a VS RWID given an AEEVRUtterance.
==============================================================================*/
static vdb_rwid_type ConvertToVSRW(AEEVRUtterance wUtt)
{
   // Make sure wUtt is a valid voice tag, control word, or prompt
   if (((wUtt & AEEVRUTTERANCE_TYPE_MASK) == AEEVRUTTERANCE_TYPE_VOICETAG) ||
       ((wUtt & AEEVRUTTERANCE_TYPE_MASK) == AEEVRUTTERANCE_TYPE_PROMPT) ||
       ((wUtt & AEEVRUTTERANCE_TYPE_MASK) == AEEVRUTTERANCE_TYPE_CWORD))
   {
      // Lower 16 bits of AEEVRUtterance contain vdb_rwid_type
      return (vdb_rwid_type)(wUtt & 0xffff);
   }
   else
   {
      return VDB_RWID_NONE;
   }
}

/*==============================================================================
Description:
   This utility function returns an AEEVRUtterance given a VS RWID.
   (Supports voice tags and control words only, as there should never be any
   need to convert a VS prompt RWID to an AEEVRUtterance.)
==============================================================================*/
static AEEVRUtterance ConvertToAEEVRUtterance(vdb_rwid_type utterance)
{
   uint16 utt = (uint16)utterance;

   if (utt == VDB_RWID_NONE)
   {
      return AEEVRUTTERANCE_NONE;
   }
   else if ((utt >= (AEEVRUTTERANCE_FIRST_PROMPT & 0xffff)) &&
            (utt <= (AEEVRUTTERANCE_LAST_PROMPT & 0xffff)))
   {
      // Utterance is a prompt (not supported)
      return AEEVRUTTERANCE_NONE;
   }
   else if ((utt >= (AEEVRUTTERANCE_FIRST_CWORD & 0xffff)) &&
            (utt <= (AEEVRUTTERANCE_LAST_CWORD & 0xffff)))
   {
      // Utterance is a control word
      return (AEEVRUtterance)(AEEVRUTTERANCE_TYPE_CWORD | utt);
   }
   else
   {
      // Utterance must be a voice tag
      return (AEEVRUtterance)(AEEVRUTTERANCE_TYPE_VOICETAG | utt);
   }
}

/*==============================================================================
Description:
   This is the internal callback function that processes a status/result from
   a non-Brew-context VS callback.

   Note:  This does not handle the theoretical possibility of multiple
   VS-context callbacks happening before the first one can be handled here in
   Brew-context.  This is assumed not to happen in practice, since the VS
   callbacks should not happen in quick succession.  (If it did ever happen,
   we'd have to put in a mechanism to queue them up in a thread-safe manner.)
==============================================================================*/
static void ProcessVSCallback(void *pNotifyData)
{
   IVR *pIVR = (IVR *)pNotifyData;
   AEEVRStatus wStatus = AEEVRSTATUS_NONE;
   AEEVRFailure wFailure = AEEVRFAILURE_NONE;
   AEEVRUtterance wUtterance = AEEVRUTTERANCE_NONE;
   boolean bDefaultFailure = FALSE;
   boolean bDefaultUtterance = FALSE;

   // If this callback was already queued up and then the IVR instance became
   // not current or the asynchronous operation ended, then do not process it.
   // (Shouldn't happen, since AbortAsyncOp should take care of cancelling any
   // callback that was already scheduled, but handle it just in case.)
   if (pIVR != pCurrentInstance)
   {
      DBGPRINTF("VR:  ProcessVSCallback with non-current pIVR");
      return;
   }
   else if (pIVR->wStatus != AEEVRSTATUS_RUNNING)
   {
      DBGPRINTF("VR:  ProcessVSCallback with wStatus = %d", pIVR->wStatus);
      return;
   }

   // Handle callback according to which asynchronous operation is in progress.
   // Note:  The VS_ABORTED status should never occur for any operation, since
   // wAsyncOp should have already been reset by IVR_AbortOperation.
   switch (pIVR->wAsyncOp)
   {
   case OEMVRASYNCOP_PLAY_UTTERANCE:
   case OEMVRASYNCOP_PLAY_SOUNDS:
      // Handle all possible status codes from vs_play_utterance and
      // vs_play_multi
      switch (pIVR->wVSStatus)
      {
      case VS_SUCCESS:
         // Done
         wStatus = AEEVRSTATUS_DONE;
         break;
      case VS_FAILURE:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      }
      break;

   case OEMVRASYNCOP_TRAIN_VTAG:
   case OEMVRASYNCOP_TRAIN_CWORD:
      // Handle all possible status codes from vs_train_utterance and
      // vs_train_control_word
      switch (pIVR->wVSStatus)
      {
      case VS_SUCCESS:
         // Done, report utterance if voice tag
         wStatus = AEEVRSTATUS_DONE;
         if (pIVR->wAsyncOp == OEMVRASYNCOP_TRAIN_VTAG)
         {
            bDefaultUtterance = TRUE;
         }
         break;
      case VS_FAILURE:
      case VS_DUPLICATE:
      case VS_NV_MEM:
      case VS_FFS_ERR:
      case VS_VDB_ERR:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      case VS_UNMATCHED:
      case VS_TIMEOUT:
      case VS_TOO_SOON:
      case VS_TOO_LONG:
      case VS_TOO_SHORT:
         // Still running, but report an intermediate failure result
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultFailure = TRUE;
         break;
      case VS_CONTINUING:
         // No result to report
         return;
      }
      break;

   case OEMVRASYNCOP_RECOGNIZE_PRIMITIVE:
   case OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS:
      // Handle all possible status codes from vs_recognize_utterance
      switch (pIVR->wVSStatus)
      {
      case VS_SUCCESS:
         // Done, report utterance
         wStatus = AEEVRSTATUS_DONE;
         bDefaultUtterance = TRUE;
         break;
      case VS_FAILURE:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      case VS_UNMATCHED:
      case VS_TIMEOUT:
      case VS_TOO_SOON:
      case VS_TOO_LONG:
      case VS_TOO_SHORT:
      case VS_YESNO_UNMATCHED:
      case VS_YESNO_TIMEOUT:
      case VS_YESNO_TOO_SOON:
      case VS_YESNO_TOO_LONG:
      case VS_YESNO_TOO_SHORT:
         if (pIVR->wAsyncOp == OEMVRASYNCOP_RECOGNIZE_PRIMITIVE)
         {
            // Special case for RecognizePrimitive:
            // Abort operation and count as failure
            AbortAsyncOp(pIVR);
            wStatus = AEEVRSTATUS_FAILED;
            bDefaultFailure = TRUE;
         }
         else if (pIVR->bRecognizeWithPromptsWait &&
                  (pIVR->wVSStatus == VS_TIMEOUT))
         {
            // Special case for timeout in RecognizeWithPrompts with wait:
            // Abort and restart recognition without reporting any result
            vs_recognize_abort(NULL);
            vs_recognize_utterance(
               &pIVR->pPrompt[OEMVRPROMPT_RECOGNIZE_INITIAL],
               pIVR->pRWList, pIVR->bRecognizeWithPromptsNBest,
               pIVR->wRecognizeWithPromptsMatchCount, OEMVR_vs_rec_cb);
            return;
         }
         else
         {
            // Still running, but report an intermediate failure result
            wStatus = AEEVRSTATUS_RUNNING;
            bDefaultFailure = TRUE;
         }
         break;
      case VS_NBEST:
         if ((pIVR->wAsyncOp == OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS) &&
             (pIVR->bRecognizeWithPromptsNBest))
         {
            // Still running, report intermediate utterance
            wStatus = AEEVRSTATUS_RUNNING;
            bDefaultUtterance = TRUE;
         }
         else
         {
            // N-best not allowed:
            // Abort operation and count as failure
            AbortAsyncOp(pIVR);
            wStatus = AEEVRSTATUS_FAILED;
            wFailure = AEEVRFAILURE_DUPLICATE;
         }
         break;
      }
      break;

   case OEMVRASYNCOP_YES_NO_CONFIRM:
      // Handle all possible status codes from vs_implicit_yes
      switch (pIVR->wVSStatus)
      {
      case VS_YES_SUCCESS:
         // Done, report result
         wStatus = AEEVRSTATUS_RESULT_YES;
         break;
      case VS_NO_SUCCESS:
         // Done, report result
         wStatus = AEEVRSTATUS_RESULT_NO;
         break;
      case VS_TIMEOUT:
         // Done, no result
         wStatus = AEEVRSTATUS_DONE;
         break;
      case VS_FAILURE:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      case VS_UNMATCHED:
      case VS_TOO_SOON:
      case VS_TOO_LONG:
      case VS_TOO_SHORT:
         // Still running, but report an intermediate failure result
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultFailure = TRUE;
         break;
      }
      break;

   case OEMVRASYNCOP_MENU_SELECT:
      // Handle all possible status codes from vs_menu_selection
      switch (pIVR->wVSStatus)
      {
      case VS_SUCCESS:
         // Done, report appropriate utterance if there was a match
         wStatus = AEEVRSTATUS_DONE;
         if (pIVR->wVSValidMatch)
         {
            if (pIVR->wVSUtterance == AEEVRUTTERANCE_ID_YES)
            {
               // Report last utterance from VS_CONTINUING result
               wUtterance = pIVR->wUtterance;
            }
            else if (pIVR->wVSUtterance != AEEVRUTTERANCE_ID_CANCEL)
            {
               // Report given utterance
               bDefaultUtterance = TRUE;
            }
         }
         break;
      case VS_FAILURE:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      case VS_CONTINUING:
         // Still running, report intermediate utterance
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultUtterance = TRUE;
         break;
      }
      break;

   case OEMVRASYNCOP_RECOGNIZE_MULTI:
      // Handle all possible status codes from vs_recognize_multi
      switch (pIVR->wVSStatus)
      {
      case VS_SUCCESS:
         // Still running, report intermediate utterance
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultUtterance = TRUE;
         break;
      case VS_FAILURE:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      case VS_CONTINUING:
         // No result to report
         return;
      case VS_UNMATCHED:
      case VS_TIMEOUT:
      case VS_TOO_SOON:
      case VS_TOO_LONG:
      case VS_TOO_SHORT:
         // Still running, but report an intermediate failure result
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultFailure = TRUE;
         break;
      }
      break;

   case OEMVRASYNCOP_VOICE_ACTIVATE:
      // Handle all possible status codes from vs_voice_activate
      switch (pIVR->wVSStatus)
      {
      case VS_SUCCESS:
         // Done, report utterance
         wStatus = AEEVRSTATUS_DONE;
         bDefaultUtterance = TRUE;
         break;
      case VS_FAILURE:
      case VS_VOC_ABORTED:
         // Failed
         wStatus = AEEVRSTATUS_FAILED;
         bDefaultFailure = TRUE;
         break;
      case VS_CONTINUING:
         // Still running, report intermediate utterance
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultUtterance = TRUE;
         break;
      case VS_UNMATCHED:
         // Still running, but report an intermediate failure result
         wStatus = AEEVRSTATUS_RUNNING;
         bDefaultFailure = TRUE;
         break;
      case VS_TIMEOUT:
         // No result to report
         return;
      }
      break;
   }

   // If no status set, report an error.
   // (Should never happen - indicates a missing case above.)
   if (wStatus == AEEVRSTATUS_NONE)
   {
      DBGPRINTF("VR: Case not handled in VS callback (%d, %d)",
                pIVR->wAsyncOp, pIVR->wVSStatus);

      // Treat as a failure with no failure code
      wStatus = AEEVRSTATUS_FAILED;
      wFailure = AEEVRFAILURE_NONE;
      wUtterance = AEEVRUTTERANCE_NONE;
      bDefaultFailure = FALSE;
      bDefaultUtterance = FALSE;
   }

   // Get the default failure code if requested
   if (bDefaultFailure)
   {
      switch (pIVR->wVSStatus)
      {
      case VS_FAILURE:
      case VS_VOC_ABORTED:
         wFailure = AEEVRFAILURE_GENERAL;
         break;
      case VS_UNMATCHED:
      case VS_YESNO_UNMATCHED:
         wFailure = AEEVRFAILURE_UNMATCHED;
         break;
      case VS_DUPLICATE:
         wFailure = AEEVRFAILURE_DUPLICATE;
         break;
      case VS_TIMEOUT:
      case VS_YESNO_TIMEOUT:
         wFailure = AEEVRFAILURE_SILENCE;
         break;
      case VS_TOO_SOON:
      case VS_YESNO_TOO_SOON:
         wFailure = AEEVRFAILURE_TOO_SOON;
         break;
      case VS_TOO_SHORT:
      case VS_YESNO_TOO_SHORT:
         wFailure = AEEVRFAILURE_TOO_SHORT;
         break;
      case VS_TOO_LONG:
      case VS_YESNO_TOO_LONG:
         wFailure = AEEVRFAILURE_TOO_LONG;
         break;
      case VS_NV_MEM:
      case VS_FFS_ERR:
      case VS_VDB_ERR:
         wFailure = AEEVRFAILURE_FILE_ERROR;
         break;
      default:
         wFailure = AEEVRFAILURE_NONE;
         break;
      }
   }

   // Get the default utterance if requested
   if (bDefaultUtterance)
   {
      wUtterance = ConvertToAEEVRUtterance(pIVR->wVSUtterance);
   }

   // Report the result
   SetAsyncResult(pIVR, wStatus, wFailure, wUtterance);
}

/*==============================================================================
Description:
   This is the VS callback function for vs_play_utterance and vs_play_multi.
==============================================================================*/
static void OEMVR_vs_play_cb(vs_status_type status)
{
   // Current instance should never be NULL, since any asynchronous operation
   // is aborted when that happens, but check just in case.
   if (pCurrentInstance != NULL)
   {
      // Since this function is called outside of Brew context, just store away
      // the data and invoke a callback function to do the real processing.
      pCurrentInstance->wVSStatus = status;
      pCurrentInstance->wVSUtterance = VDB_RWID_NONE;
      pCurrentInstance->wVSValidMatch = 0;
      CALLBACK_Init(&pCurrentInstance->ProcessVSCB, ProcessVSCallback,
                    pCurrentInstance);
#ifdef FEATURE_BREW_3_0
  AEE_ResumeCallback(&pCurrentInstance->ProcessVSCB,  pCurrentInstance->m_pac);
#else
      AEE_ResumeEx(&pCurrentInstance->ProcessVSCB, 0, pCurrentInstance->m_pac);
#endif
   }
}

/*==============================================================================
Description:
   This is the VS callback function for vs_train_utterance and
   vs_train_control_word.
==============================================================================*/
static void OEMVR_vs_train_cb(vs_status_type status, vdb_rwid_type utterance,
                              vs_handshake_cb_type handshakePtr)
{
   // Current instance should never be NULL, since any asynchronous operation
   // is aborted when that happens, but check just in case.
   if (pCurrentInstance != NULL)
   {
      // Since this function is called outside of Brew context, just store away
      // the data and invoke a callback function to do the real processing.
      pCurrentInstance->wVSStatus = status;
      pCurrentInstance->wVSUtterance = utterance;
      pCurrentInstance->wVSValidMatch = 0;
      CALLBACK_Init(&pCurrentInstance->ProcessVSCB, ProcessVSCallback,
                    pCurrentInstance);
#ifdef FEATURE_BREW_3_0
  AEE_ResumeCallback(&pCurrentInstance->ProcessVSCB,  pCurrentInstance->m_pac);
#else
      AEE_ResumeEx(&pCurrentInstance->ProcessVSCB, 0, pCurrentInstance->m_pac);
#endif
   }
}

/*==============================================================================
Description:
   This is the VS callback function for vs_recognize_utterance, vs_implicit_yes,
   vs_menu_selection, vs_recognize_multi, and vs_voice_activate.
==============================================================================*/
static void OEMVR_vs_rec_cb(vs_status_type status, vdb_rwid_type utterance,
                            short valid_match,
                            vs_handshake_cb_type handshakePtr)
{
   // Current instance should never be NULL, since any asynchronous operation
   // is aborted when that happens, but check just in case.
   if (pCurrentInstance != NULL)
   {
      // Since this function is called outside of Brew context, just store away
      // the data and invoke a callback function to do the real processing.
      pCurrentInstance->wVSStatus = status;
      pCurrentInstance->wVSUtterance = utterance;
      pCurrentInstance->wVSValidMatch = valid_match;
      CALLBACK_Init(&pCurrentInstance->ProcessVSCB, ProcessVSCallback,
                    pCurrentInstance);

#ifdef FEATURE_BREW_3_0
  AEE_ResumeCallback(&pCurrentInstance->ProcessVSCB,  pCurrentInstance->m_pac);
#else
      AEE_ResumeEx(&pCurrentInstance->ProcessVSCB, 0, pCurrentInstance->m_pac);
#endif
   }
}

/*==============================================================================
Description:
   This function invokes VS to start the current asynchronous operation.
   This assumes all the appropriate data structures have already been set up
   for the relevant operation.
==============================================================================*/
static void StartVSOperation(IVR *pIVR)
{
#if defined (FEATURE_ACM)
   // Can't start until resource acquired
   if (!pIVR->bResourceAcquired)
   {
      return;
   }
#endif

   switch (pIVR->wAsyncOp)
   {
   case OEMVRASYNCOP_TRAIN_VTAG:
      vs_train_utterance(&pIVR->pPrompt[OEMVRPROMPT_TRAIN_INITIAL],
                         VS_UTT_ECHO_FIRST, pIVR->pRWList,
                         OEMVR_vs_train_cb);
      break;
   case OEMVRASYNCOP_RECOGNIZE_PRIMITIVE:
      {
         // Prompts for RecognizePrimitive are all NULL, and cannot be overridden
         static vs_sound_type *pRecPrimPrompts[13];
         
         vs_recognize_utterance(pRecPrimPrompts, pIVR->pRWList, FALSE, 1,
                                OEMVR_vs_rec_cb);
      }
      break;
   case OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS:
      vs_recognize_utterance(&pIVR->pPrompt[OEMVRPROMPT_RECOGNIZE_INITIAL],
                             pIVR->pRWList,
                             pIVR->bRecognizeWithPromptsNBest,
                             pIVR->wRecognizeWithPromptsMatchCount,
                             OEMVR_vs_rec_cb);
      break;
   case OEMVRASYNCOP_YES_NO_CONFIRM:
      vs_implicit_yes(&pIVR->pPrompt[OEMVRPROMPT_YESNO_INITIAL], OEMVR_vs_rec_cb);
      break;
   case OEMVRASYNCOP_MENU_SELECT:
      {
         vs_menu_play_prompt_type wPromptOrder;

         // If "unmatched" prompt begins with slot for the RWID, then the order
         // is "RWID followed by prompt", else assume "prompt followed by RWID".
         if ((pIVR->pPrompt[OEMVRPROMPT_MENU_UNMATCHED]) &&
             (pIVR->pPrompt[OEMVRPROMPT_MENU_UNMATCHED][0].sound ==
              SND_VOICE_PROMPT) &&
             (pIVR->pPrompt[OEMVRPROMPT_MENU_UNMATCHED][0].utt_id ==
              VDB_RWID_NONE))
         {
            wPromptOrder = VS_MENU_RWID_PROMPT;
         }
         else
         {
            wPromptOrder = VS_MENU_PROMPT_RWID;
         }

         vs_menu_selection(&pIVR->pPrompt[OEMVRPROMPT_MENU_INITIAL],
                           &pIVR->RWList[0], VS_MENU_LIST_INCLUDED,
                           wPromptOrder, OEMVR_vs_rec_cb);
      }
      break;
   case OEMVRASYNCOP_PLAY_UTTERANCE:
      vs_play_utterance(pIVR->wUtteranceArg, 0, VS_SND_FORWARD,
                        OEMVR_vs_play_cb);
      break;
   case OEMVRASYNCOP_PLAY_SOUNDS:
      vs_play_multi(pIVR->pSound, VS_SND_FORWARD, OEMVR_vs_play_cb);
      break;
   case OEMVRASYNCOP_TRAIN_CWORD:
      vs_train_control_word(&pIVR->pPrompt[OEMVRPROMPT_TRAIN_INITIAL],
                            VS_UTT_ECHO_FIRST, pIVR->wUtteranceArg,
                            pIVR->pRWList, OEMVR_vs_train_cb);
      break;
   case OEMVRASYNCOP_RECOGNIZE_MULTI:
      vs_recognize_multi(&pIVR->pPrompt[OEMVRPROMPT_MULTI_INITIAL],
                         &pIVR->RWList[0], &pIVR->RWList[1],
                         pIVR->wMultRecArg, OEMVR_vs_rec_cb);
      break;
   case OEMVRASYNCOP_VOICE_ACTIVATE:
      vs_voice_activate(&pIVR->pPrompt[OEMVRPROMPT_ACTIVATE_1ST_CONFIRM],
                        &pIVR->RWList[0], pIVR->wUtteranceArg,
                        OEMVR_vs_rec_cb);
      break;
   }
}

/*==============================================================================

Function: OEMVR_New

Description:
   This function is from the IBase interface.  It increments the reference
   count of the IVR object.  This allows the object to be shared by multiple
   callers.  The object is freed when the reference count reaches zero.

Prototype:
   int OEMVR_New(IShell *pIShell, AEECLSID cls, void **ppo)

Parameters:
   pIShell     [in]: Pointer to IVR interface object.
   cls         [in]: Requested class ID exposed by IVR interface object.
   ppo        [out]: Where to return the object pointer.

Return Value:
   SUCCESS
   EBADPARM
   ENOMEMORY

Comments:
   None

Side Effects:
   None

See Also:
   None

==============================================================================*/
int OEMVR_New(IShell *pIShell, AEECLSID cls, void **ppo)
{
   IVR *pIVR;
   AEEDeviceInfo di;
   char *szPath= NULL;
   int i;
 #ifdef FEATURE_BREW_3_0
   int nLen = AEE_MAX_FILE_NAME;
 #endif

   // Allocate new IVR class
   pIVR = (IVR *)AEE_NewClass((AEEVTBL(IBase) *)&gOEMVRFuncs, sizeof(IVR));

   if (pIVR == NULL)
   {
      return ENOMEMORY;
   }

   if(ppo == NULL)
   {
     return EFAILED;
   }

   //MAKE This a Link Sys object by calling
   //AEE_LinkSysObject() so that we can track this object
   //when the app that created it goes away. When the app goes away, appropriate
   //clean up can be done
#ifndef FEATURE_BREW_3_0
   AEE_LinkSysObject(&pIVR->u.m_so);
#endif
   pIVR->m_pac = AEE_GetAppContext();


   // Initialize IShell pointer
   pIVR->pIShell = pIShell;
   ISHELL_AddRef(pIVR->pIShell);

   di.wStructSize = sizeof(AEEDeviceInfo);

   ISHELL_GetDeviceInfo(pIShell, &di);

   szPath = MALLOC(di.wMaxPath + 1);

   if (szPath == NULL)
   {
      return ENOMEMORY;
   }

#if defined (FEATURE_ACM) && defined (FEATURE_SIRIUS)
#error code not present
#endif // FEATURE_ACM && FEATURE_SIRIUS

   // Get a Brew-app-apecific default pathname for the voice tag database
#ifdef FEATURE_BREW_3_0
   if (AEE_ResolvePath(szVtagDBPrefix, szPath, &nLen) == 0)
#else
   if (AEE_BuildPath(pIShell, szVtagDBPrefix, szPath, NULL) == NULL)
#endif
   {
      // Couldn't get Brew-app-specific path
      DBGPRINTF("VR: Couldn't build Brew db path");

      // Just make path go from root instead
      STRLCPY(szPath, szVtagDBPrefix, STRLEN(szVtagDBPrefix) + 1);
   }
   else if ((STRLEN(szPath) + 8) > VDB_CLIENT_NAME_MAX_LENGTH)
   {
      // Brew-app-specific path (plus 8-char class ID to be appended) too long
      DBGPRINTF("VR: Brew db path too long (%s)", szPath);

      // Just make path go from root instead
      STRLCPY(szPath, szVtagDBPrefix, STRLEN(szVtagDBPrefix) + 1);
   }

   // HACK:  With current FS, vdb requires pathname length to be odd
   if (((i = STRLEN(szPath)) & 1) == 0)
   {
      // Truncate pathname by 1 char (before appending class ID)
      szPath[i - 1] = '\0';
   }

   // Save pathname in pIVR, appending unique class ID (8 chars)
   SPRINTF(pIVR->szVSClientDBPath, "%s%.8x", szPath,
           (uint32)AEE_GetAppContextCls(AEE_GetAppContext()));

   // Initialize language to default, which is assumed to be the current VS
   // language when the first IVR instance is created
   if (!szDefaultLanguage[0])
   {
      vdb_get_current_language(szVSLanguage);
      STRLCPY(szDefaultLanguage, szVSLanguage, STRLEN(szVSLanguage) + 1);
   }
   STRLCPY(pIVR->szVSLanguage, szDefaultLanguage, STRLEN(szDefaultLanguage) + 1);

   // Initialize pattern setting to default
   pIVR->wVSPatternSet = VS_PATT_SD_SI;

   // Initialize status
   pIVR->wStatus = AEEVRSTATUS_NONE;
   pIVR->wFailure = AEEVRFAILURE_NONE;
   pIVR->wUtterance = AEEVRUTTERANCE_NONE;

   // Initialize asynchronous operation
   pIVR->wAsyncOp = OEMVRASYNCOP_NONE;

   // Initialize callback pointer
   pIVR->pUserCallback = NULL;

   // Initialize VS prompt data
   for (i = 0; i < OEMVRPROMPT_LIMIT; i++)
   {
      pIVR->pPrompt[i] = (vs_sound_type *)pDefaultPrompt[i];
   }

   // Initialize VS RW list data
   for (i = 0; i < NUM_RW_LISTS; i++)
   {
     pIVR->RWList[i].list = NULL;
     pIVR->RWList[i].num_rws = 0;
     pIVR->pRWList[i] = &pIVR->RWList[i];
   }
   pIVR->pRWList[i] = NULL;

   // Initialize VS sound data
   pIVR->pSound = NULL;

   // Initialize VS language iter data
   pIVR->bLanguageIterValid = FALSE;

   // Initialize VS vtag iter data
   pIVR->bVtagIterValid = FALSE;

   // Set object pointer to new IVR object
   *ppo = pIVR;

   FREE(szPath);

   return SUCCESS;
}

/*==============================================================================

Function: IVR_AddRef

Description:
   This function is from the IBase interface.  It increments the reference
   count of the IVR object.  This allows the object to be shared by multiple
   callers.  The object is freed when the reference count reaches zero.

Prototype:
   uint32 IVR_AddRef(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Incremented reference count for the IVR object.

Comments:
   None

Side Effects:
   None

See Also:
   IVR_Release

==============================================================================*/
static uint32 OEMVR_AddRef(IVR *pIVR)
{
   return ++(pIVR->nRef);
}

/*==============================================================================

Function: IVR_Release

Description:
   This function is from the IBase interface.  It decrements the reference
   count of IVR interface object.  The object is freed from memory and is no
   longer valid once the reference count reaches zero.

Prototype:
   uint32 IVR_Release(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Decremented reference count for the IVR interface object, or zero if the
   IVR interface object has been freed and is no longer valid.

Comments:
   None

Side Effects:
   None

See Also:
   IVR_AddRef

==============================================================================*/
static uint32 OEMVR_Release(IVR *pIVR)
{
   int i;

   if (--pIVR->nRef)
   {
      return pIVR->nRef;
   }
   else
   {
      // Make this not be the current instance, cleaning up if it was
      if (pIVR == pCurrentInstance)
      {
         SetCurrentInstance(NULL);
      }

      //Need to do AEE_UnlinkSysObject() here.
#ifndef FEATURE_BREW_3_0
      AEE_UnlinkSysObject(&pIVR->u.m_so);
#endif

      // Release IShell reference
      ISHELL_Release(pIVR->pIShell);

      // Free VS prompt data
      for (i = 0; i < OEMVRPROMPT_LIMIT; i++)
      {
         if (pIVR->pPrompt[i] != pDefaultPrompt[i])
         {
            FREE(pIVR->pPrompt[i]);
         }
      }

      // Free VS RW list data
      for (i = 0; i < NUM_RW_LISTS; i++)
      {
         FREEIF(pIVR->RWList[i].list);
      }

      // Free VS sound data
      FREEIF(pIVR->pSound);

#if defined (FEATURE_ACM) && defined (FEATURE_SIRIUS)
#error code not present
#endif // FEATURE_ACM && FEATURE_SIRIUS

      // Finally, free the IVR instance itself
      FREE(pIVR);

      return 0;
   }
}

/*==============================================================================

Function: IVR_QueryInterface

Description:
   This function is from the IQueryInterface interface.  It asks the IVR
   interface object for another API contract that it exposes.

Prototype:
   int IVR_QueryInterface(IVR *pIVR, AEECLSID idReq, void **ppo);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   idReq       [in]: Requested class ID exposed by IVR interface object.
   ppo        [out]: Where to return the object pointer.

Return Value:
   SUCCESS           Interface found.
   ENOMEMORY         Insufficient memory.
   ECLASSNOTSUPPORT  Requested interface is unsupported.

Comments:
   The pointer in *ppo is set to the new interface (with refcount accordingly
   positive), or NULL if the class ID is not supported by the object.

Side Effects:
   None

See Also:
   AEECLSID

==============================================================================*/
static int OEMVR_QueryInterface(IVR *pIVR, AEECLSID idReq, void **ppo)
{
   if(ppo == NULL)
     return EFAILED;

   switch (idReq)
   {
   case AEECLSID_VR:
   case AEECLSID_QUERYINTERFACE:
      *ppo = pIVR;
      OEMVR_AddRef(pIVR);
      return SUCCESS;
#if defined (FEATURE_ACM) && defined (FEATURE_SIRIUS)
#error code not present
#endif // FEATURE_ACM && FEATURE_SIRIUS
   default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
   }
}

/*==============================================================================

Function: IVR_TrainVoiceTag

Description:
   Starts an asynchronous operation to train a new voice tag.  When finished,
   the new voice tag can be obtained with IVR_GetOperationStatus or via the
   callback registered with IVR_RegisterNotify.

Prototype:
   int IVR_TrainVoiceTag(IVR *pIVR, const AEEVRUtterance *pDupList);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pDupList    [in]: Array of utterances to check against for duplicates,
                     terminated by AEEVRUTTERANCE_NONE.  If pDupList is NULL,
                     then the set of all voice tags associated with the
                     application is checked against.  If pDupList points to
                     a single element with the value AEEVRUTTERANCE_NONE,
                     then no duplicate-checking is performed.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Training completed (new voice tag assigned).
      AEEVRSTATUS_FAILED      Training failed.
      AEEVRSTATUS_ABORTED     Training was aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING     Training still in progress (used to report
                              intermediate failures).

Comments:
   The exact behavior of this function is platform-dependent.  Typically, the
   platform will capture some speech, then prompt the user to repeat it for
   verification.  The platform may optionally attempt to do this several times
   as needed, reporting any intermediate failure each time by invoking the
   registered callback with the status set to AEEVRSTATUS_RUNNING.  On all
   platforms, if there is still a failure after some unspecified reasonable
   number of attempts, the registered callback will finally be invoked with the
   status set to AEEVRSTATUS_FAILED.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation

==============================================================================*/
static int OEMVR_TrainVoiceTag(IVR *pIVR, const AEEVRUtterance *pDupList)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_TRAIN_VTAG);

   // Set up RWLists, checking for errors
   if ((CreateVSRWList(&pIVR->RWList[0], pDupList,
                       AEEVRUTTERANCE_TYPE_VOICETAG) == SUCCESS) &&
       (CreateVSRWList(&pIVR->RWList[1], pDupList,
                       AEEVRUTTERANCE_TYPE_CWORD) == SUCCESS))
   {
      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: IVR_RecognizePrimitive

Description:
   Starts an asynchronous operation to recognize a voice utterance.  This is
   the most primitive function to recognize an utterance.  It only makes a
   single attempt at recognition, without prompting the user in any way, even
   if no speech is detected or the recognition attempt is unsuccesful for any
   other reason.  When finished, the recognized utterance can be obtained with
   IVR_GetOperationStatus or via the callback registered with
   IVR_RegisterNotify.

Prototype:
   int IVR_RecognizePrimitive(IVR *pIVR, const AEEVRUtterance *pRecogList);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pRecogList  [in]: Array of possible utterances to recognize, terminated by
                     AEEVRUTTERANCE_NONE.  If NULL, recognizes any utterance
                     from the set of all voice tags associated with the
                     application.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Recognizing completed (utterance recognized).
      AEEVRSTATUS_FAILED      Recognizing failed.
      AEEVRSTATUS_ABORTED     Recognizing was aborted with IVR_AbortOperation.

Comments:
   The exact behavior of this function is platform-dependent.  The platform will
   make a single attempt to capture some speech, typically for a few seconds.
   If no speech is captured or if the recognition attempt fails, the registered
   callback will be invoked with the status set to AEEVRSTATUS_FAILED.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation

==============================================================================*/
static int OEMVR_RecognizePrimitive(IVR *pIVR, const AEEVRUtterance *pRecogList)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_RECOGNIZE_PRIMITIVE);

   // Set up RWLists, checking for errors
   if ((CreateVSRWList(&pIVR->RWList[0], pRecogList,
                       AEEVRUTTERANCE_TYPE_VOICETAG) == SUCCESS) &&
       (CreateVSRWList(&pIVR->RWList[1], pRecogList,
                       AEEVRUTTERANCE_TYPE_CWORD) == SUCCESS))
   {
      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: IVR_RecognizeWithPrompts

Description:
   Starts an asynchronous operation to recognize an utterance.  This is similar
   to IVR_RecognizePrimitive, but more advanced in that it attempts to make the
   recognition process more automatic by prompting the user to try again if
   necessary, or to pick which utterance was intended if the detected speech
   was a good match for multiple utterances.  When finished, the recognized
   utterance can be obtained with IVR_GetOperationStatus or via the callback
   registered with IVR_RegisterNotify.

Prototype:
   int IVR_RecognizeWithPrompts(IVR *pIVR, const AEEVRUtterance *pRecogList,
                                boolean bWait, uint8 wMatchCount);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pRecogList  [in]: Array of possible utterances to recognize, terminated by
                     AEEVRUTTERANCE_NONE.  If NULL, recognizes any utterance
                     from the set of all voice tags associated with the
                     application.
   bWait       [in]: If TRUE, keeps waiting (with no prompt) until speech is
                     detected (useful when user may need some indeterminate
                     amount of time before speaking).
                     If FALSE, prompts user to say something when no speech is
                     detected, eventually failing if still no speech detected.
   wMatchCount [in]: Maximum number of matches to attempt to resolve.  If
                     greater than 1, user may be prompted to choose which
                     utterance was intended, if the detected speech was a good
                     match for multiple utterances.  The value 0 is acceptable,
                     and means the same thing as the value 1.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Recognizing completed (utterance recognized).
      AEEVRSTATUS_FAILED      Recognizing failed.
      AEEVRSTATUS_ABORTED     Recognizing was aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING     Recognizing still in progress (used to report
                              current utterance or intermediate failures).

Comments:
   The exact behavior of this function is platform-dependent.  Typically, the
   platform will attempt to capture some speech and recognize it, prompting
   the user to try again if the capture fails and bWait was FALSE, or in any
   case if the recognition fails.  It may attempt to do this several times as
   needed, reporting the intermediate failures each time by invoking the
   registered callback with the status set to AEEVRSTATUS_RUNNING.  On all
   platforms, if there is still a failure after some unspecified reasonable
   number of attempts, the registered callback will finally be invoked with the
   status set to AEEVRSTATUS_FAILED.

   If the captured speech matches multiple utterances, and wMatchCount is
   greater than one, the platform will typically indicate this with some kind
   of audible feedback, then ask the user to say "Yes" or "No" for each of the
   matches.  When asking the user to confirm a match, the registered callback
   may be invoked with the status set to AEEVRSTATUS_RUNNING and the utterance
   set to the current match, to inform the application which utterance is being
   confirmed.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation

==============================================================================*/
static int OEMVR_RecognizeWithPrompts(IVR *pIVR,
                                      const AEEVRUtterance *pRecogList,
                                      boolean bWait, uint8 wMatchCount)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS);

   // Set up bRecognizeWithPrompts configuration
   pIVR->bRecognizeWithPromptsWait = bWait;
   pIVR->bRecognizeWithPromptsNBest = (wMatchCount > 1) ? TRUE : FALSE;
   pIVR->wRecognizeWithPromptsMatchCount = (wMatchCount > 1) ? wMatchCount : 1;

   // Set up RWLists, checking for errors
   if ((CreateVSRWList(&pIVR->RWList[0], pRecogList,
                       AEEVRUTTERANCE_TYPE_VOICETAG) == SUCCESS) &&
       (CreateVSRWList(&pIVR->RWList[1], pRecogList,
                       AEEVRUTTERANCE_TYPE_CWORD) == SUCCESS))
   {
      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: IVR_YesNoConfirm

Description:
   Starts an asynchronous operation to recognize a "Yes", "No", or silence
   response.  When finished, the result can be obtained with
   IVR_GetOperationStatus or via the callback registered with
   IVR_RegisterNotify.

Prototype:
   int IVR_YesNoConfirm(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_RESULT_YES   "Yes" response detected.
      AEEVRSTATUS_RESULT_NO    "No" response detected.
      AEEVRSTATUS_DONE         No speech was detected.
      AEEVRSTATUS_FAILED       Recognizing failed.
      AEEVRSTATUS_ABORTED      Recognizing was aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING      Recognizing still in progress (used to report
                               intermediate failures).

Comments:
   The exact behavior of this function is platform-dependent. If no speech is
   detected, the exact timeout before an AEEVRSTATUS_DONE result is not
   specified, but will typically be a few seconds.  This gives the user enough
   time to speak if he so chooses, but is short enough that he will not grow
   impatient if he instead chooses to remain silent when an explicit response
   is not required (e.g. in cases where silence is treated as an implicit "Yes"
   or implicit "No" by the application).

   If an utterance is detected that is neither "Yes" nor "No", the platform will
   typically take reasonable measures to attempt to resolve the situation.  For
   example, the platform may ask, "Was that a yes?" and attempt to get another
   response.  On all platforms, if these reasonable measures fail to obtain an
   appropriate response, the registered callback will finally be invoked with
   the status set to AEEVRSTATUS_FAILED.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   IVR_RegisterNotify, IVR_GetOperationStatus, IVR_AbortOperation

==============================================================================*/
static int OEMVR_YesNoConfirm(IVR *pIVR)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_YES_NO_CONFIRM);

   // Call VS function to do the work
   StartVSOperation(pIVR);

   return SUCCESS;
}

/*==============================================================================

Function: IVR_VoiceMenuSelect

Description:
   Starts an asynchronous operation to recognize a response to a voice menu.
   The user is presented with a list of utterances and allowed to choose one.
   When finished, the selected utterance can be obtained with
   IVR_GetOperationStatus or via the callback registered with
   IVR_RegisterNotify.

Prototype:
   int IVR_VoiceMenuSelect(IVR *pIVR, const AEEVRUtterance *pMenuList);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pMenuList   [in]: Array of utterances to select from, terminated by
                     AEEVRUTTERANCE_NONE.  If NULL, the set of all voice tags
                     associated with the application is used.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE         Menu selection completed successfully.
      AEEVRSTATUS_FAILED       Menu selection failed.
      AEEVRSTATUS_ABORTED      Menu selection aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING      Menu selection still in progress (used to report
                               current utterance or intermediate failures).

Comments:
   The exact behavior of this function is platform-dependent.  Typically, the
   platform will cycle through the list of utterances one at a time, allowing
   the user to say "Yes" to select that utterance, or "No" to go on to the next
   utterance.  Some platforms may also give user other options such as the
   ability to make a selection by saying the utterance itself, or the ability
   to start over at the top of the list by syaing "List", or to go back to the
   previous utterance by saying "Back".  When asking the user about a particular
   menu item, the registered callback may be invoked with the status set to
   AEEVRSTATUS_RUNNING and the utterance set to the current menu item, to inform
   the application which voice tag is being confirmed.  On all platforms, if the
   user says "Cancel" or otherwise fails to make a valid selection after some
   number of attempts, the callback will finally be invoked with the status set
   to AEEVRSTATUS_FAILED.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation

==============================================================================*/
static int OEMVR_VoiceMenuSelect(IVR *pIVR, const AEEVRUtterance *pMenuList)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_MENU_SELECT);

   // Set up RWList, checking for errors
   // Note:  Only voice tags supported for now
   if (CreateVSRWList(&pIVR->RWList[0], pMenuList,
                      AEEVRUTTERANCE_TYPE_VOICETAG) == SUCCESS)
   {
      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: IVR_PlayUtterance

Description:
   Starts an asynchronous operation to audibly play the given utterance.  When
   finished, the status can be obtained with IVR_GetOperationStatus or via the
   callback registered with IVR_RegisterNotify.

Prototype:
   int IVR_PlayUtterance(IVR *pIVR, AEEVRUtterance wUtt);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wUtt        [in]: The utterance to play.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Playback completed successfully.
      AEEVRSTATUS_FAILED      Playback failed.
      AEEVRSTATUS_ABORTED     Playback was aborted with IVR_AbortOperation.

Comments:
   The playback device and volume are controlled through the ISound interface.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation, ISound

==============================================================================*/
static int OEMVR_PlayUtterance(IVR *pIVR, AEEVRUtterance wUtt)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_PLAY_UTTERANCE);

   // Save utterance argument
   pIVR->wUtteranceArg = ConvertToVSRW(wUtt);

   // Call VS function to do the work
   StartVSOperation(pIVR);

   return SUCCESS;
}

/*==============================================================================

Function: IVR_AbortOperation

Description:
   Aborts the current asynchronous VR operation, if one is in progress.
   (Training, recognizing, and playing are considered asynchronous operations,
   because the associated function calls just start the operation and return
   without waiting for the operation to complete.  This function allows these
   operations to be aborted before they are finished.)

Prototype:
   int IVR_AbortOperation(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Always returns SUCCESS.

Comments:
   None

Side Effects:
   None

See Also:
   IVR_GetOperationStatus, IVR_TrainVoiceTag, IVR_RecognizePrimitive,
   IVR_RecognizeWithPrompts, IVR_YesNoConfirm, IVR_VoiceMenuSelect,
   IVR_PlayUtterance

==============================================================================*/
static int OEMVR_AbortOperation(IVR *pIVR)
{
   // Abort current asynchronous operation and invoke callback function
   if (pIVR->wAsyncOp != OEMVRASYNCOP_NONE)
   {
      AbortAsyncOp(pIVR);
      SetAsyncResult(pIVR, AEEVRSTATUS_ABORTED, AEEVRFAILURE_NONE,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: IVR_RegisterNotify

Description:
   Registers a callback function to be invoked whenever there is some new
   status or information to report about an asynchronous VR operation.
   (Training, recognizing, and playing are considered asynchronous operations,
   because the associated function calls just start the operation and return
   without waiting for the operation to complete.)  This allows the application
   to be notified asynchronously, instead of having to poll the status by
   calling IVR_GetOperationStatus.

Prototype:
   int IVR_RegisterNotify(IVR *pIVR, PFNVRSTATUS pfn, void *pUser);

Parameters:
   pIVR          [in]: Pointer to IVR interface object.
   pfn           [in]: Pointer to the callback function to invoke to notify the
                       application of status updates (NULL to deregister).
   pUser         [in]: User data to be passed to callback when it is invoked
                       (can be NULL if no identifying data is required).

Return Value:
   Always returns SUCCESS.

Comments:
   None

Side Effects:
   None

See Also:
   PFNVRSTATUS, AEEVRStatus, AEEVRFailure, AEEVRUtterance,
   IVR_GetOperationStatus, IVR_TrainVoiceTag, IVR_RecognizePrimitive,
   IVR_RecognizeWithPrompts, IVR_YesNoConfirm, IVR_VoiceMenuSelect,
   IVR_PlayUtterance

==============================================================================*/
static int OEMVR_RegisterNotify(IVR *pIVR, PFNVRSTATUS pfn, void *pUser)
{
   // Register user callback function and data
   pIVR->pUserCallback = pfn;
   pIVR->pUserCallbackData = pUser;

   return SUCCESS;
}

/*==============================================================================

Function: IVR_GetOperationStatus

Description:
   Gets the status of the current or last asynchronous VR operation.  (Training,
   recognizing, and playing are considered asynchronous operations, because the
   associated function calls just start the operation and return without waiting
   for the operation to complete.)  In addition, this function retrieves any
   failure code associated with the asynchronous operation, as well as any
   utterance assigned by the operation.

Prototype:
   AEEVRStatus IVR_GetOperationStatus(IVR *pIVR, AEEVRFailure *pFailure,
                                      AEEVRUtterance *pUtt);

Parameters:
   pIVR          [in]: Pointer to IVR interface object.
   pFailure  [in/out]: Where to return failure code, or NULL to ignore failure
                       code.
   pUtt      [in/out]: Where to return utterance, or NULL to ignore utterance.

Return Value:
   AEEVRStatus value indicating status of the current or last VR operation.

Comments:
   None

Side Effects:
   None

See Also:
   AEEVRStatus, AEEVRFailure, AEEVRUtterance, IVR_TrainVoiceTag,
   IVR_RecognizePrimitive, IVR_RecognizeWithPrompts, IVR_YesNoConfirm,
   IVR_VoiceMenuSelect, IVR_PlayUtterance, IVR_RegisterNotify

==============================================================================*/
static AEEVRStatus OEMVR_GetOperationStatus(IVR *pIVR, AEEVRFailure *pFailure,
                                            AEEVRUtterance *pUtt)
{
   // Set failure code if appropriate
   if (pFailure)
   {
      *pFailure = pIVR->wFailure;
   }

   // Set utterance if appropriate
   if (pUtt)
   {
      *pUtt = pIVR->wUtterance;
   }

   // Return status
   return pIVR->wStatus;
}

/*==============================================================================

Function: IVR_LanguageEnumInit

Description:
   Initializes the IVR interface object to enumerate through the available
   languages, starting with the first available language, when
   IVR_LanguageEnumNext is called.

Prototype:
   int IVR_LanguageEnumInit(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Always returns SUCCESS.

Comments:
   None

Side Effects:
   None

See Also:
   IVR_LanguageEnumNext

==============================================================================*/
static int OEMVR_LanguageEnumInit(IVR *pIVR)
{
   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Initialize VS iterator
   if (vdb_language_iter_init(&pIVR->LanguageIter) == VDB_NOERR)
   {
      pIVR->bLanguageIterValid = TRUE;
   }
   else
   {
      // Should never happen, but just in case
      pIVR->bLanguageIterValid = FALSE;
   }

   // Return SUCCESS always, since error should never happen
   return SUCCESS;
}

/*==============================================================================

Function: IVR_LanguageEnumNext

Description:
   Gets the next available language supported by the VR system.  The language
   is returned in the form of a null-terminated wide-character string.

Prototype:
   boolean IVR_LanguageEnumNext(IVR *pIVR, AECHAR *pLanguage, uint32 nSize);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pLanguage  [out]: Where to return the language.
   nSize       [in]: Size (in bytes) of language buffer (pLanguage).

Return Value:
   TRUE     Successfully returned next available language.
   FALSE    No more languages to enumerate.

Comments:
   IVR_LanguageEnumInit must be called before this function is called for the
   first time.  The passed-in language buffer may be of any size; however, if
   it does not have enough room for the language string, the result will be
   truncated (but still null-terminated).  To guarantee that this does not
   happen, make the buffer large enough to hold the number of characters
   specified by the wLanguageChars field in the AEEVRProperties data obtained
   via IVR_GetProperties.

Side Effects:
   None

See Also:
   IVR_LanguageEnumInit, IVR_GetProperties, AEEVRProperties

==============================================================================*/
static boolean OEMVR_LanguageEnumNext(IVR *pIVR, AECHAR *pLanguage,
                                      uint32 nSize)
{
   char szLang[VDB_LANGUAGE_NAME_MAX_LENGTH + 1];

   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Make sure VS iterator has been initialized and destination is valid
   if (pIVR->bLanguageIterValid && pLanguage)
   {
      if (vdb_language_iter_next(&pIVR->LanguageIter, szLang) == VDB_NOERR)
      {
         // Return next language in pLanguage
         STRTOWSTR(szLang, pLanguage, nSize);
         return TRUE;
      }
   }

   // No more language, or some kind of error
   pIVR->bLanguageIterValid = FALSE;

   return FALSE;
}

/*==============================================================================

Function: IVR_GetLanguage

Description:
   Gets the current language that the VR system is set to use.  The language
   is returned in the form of a null-terminated wide-character string.

Prototype:
   int IVR_GetLanguage(IVR *pIVR, AECHAR *pLanguage, uint32 nSize);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pLanguage  [out]: Where to return the current language.
   nSize       [in]: Size (in bytes) of language buffer (pLanguage).

Return Value:
   SUCCESS    Language successfully returned.
   EFAILED    Invalid arguments

Comments:
   The passed-in language buffer may be of any size; however, if it does not
   have enough room for the language string, the result will be truncated (but
   still null-terminated).  To guarantee that this does not happen, make the
   buffer large enough to hold the number of characters specified by the
   wLanguageChars field in the AEEVRProperties data obtained via
   IVR_GetProperties.

Side Effects:
   None

See Also:
   IVR_SetLanguage, IVR_GetProperties, AEEVRProperties

==============================================================================*/
static int OEMVR_GetLanguage(IVR *pIVR, AECHAR *pLanguage, uint32 nSize)
{
   // Verify destination argument
   if (pLanguage == NULL)
   {
      return EFAILED;
   }
    // Copy current language to destination
    STRTOWSTR(pIVR->szVSLanguage, pLanguage, nSize);
    return SUCCESS;

}

/*==============================================================================

Function: IVR_SetLanguage

Description:
   Sets the current language for the voice recognition system.

Prototype:
   int IVR_SetLanguage(IVR *pIVR, const AECHAR *pLanguage);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pLanguage   [in]: Language to set (null-terminated wide-character string).

Return Value:
   SUCCESS    Language successfully set.
   EFAILED    Invalid arguments or unsupported language.

Comments:
   The language must be one of the ones supported by the voice recognition
   system.  The supported languages are platform dependent, and typically
   case-sensitive.  They can be determined by using IVR_LanguageEnumInit and
   IVR_LanguageEnumNext.

Side Effects:
   The current language setting may affect the recognition performance and the
   maximum number of voice tags recommended for best results.

See Also:
   IVR_LanguageEnumInit, IVR_LanguageEnumNext, IVR_GetLanguage

==============================================================================*/
static int OEMVR_SetLanguage(IVR *pIVR, const AECHAR *pLanguage)
{
   char szLang[VDB_LANGUAGE_NAME_MAX_LENGTH + 1];

   // Verify language is valid
   if ((pLanguage == NULL) ||
       (WSTRLEN(pLanguage) > VDB_LANGUAGE_NAME_MAX_LENGTH))
   {
      return EFAILED;
   }

   // Convert language to ASCII string
   WSTRTOSTR(pLanguage, szLang, sizeof(szLang));

   // No need to do anything if new language matches current pIVR setting
   if (STRCMP(szLang, pIVR->szVSLanguage))
   {
      // No need to attempt to set new language if already set
      if (STRCMP(szLang, szVSLanguage))
      {
         // Make sure VS isn't in the middle of something
         SetCurrentInstance(NULL);

         // Attempt to set new language using VS
         if (vdb_set_current_language(szLang) != VDB_NOERR)
         {
            szVSLanguage[0] = '\0';
            return EFAILED;
         }

         // Set current language to new setting
         STRLCPY(szVSLanguage, szLang, STRLEN(szLang) + 1);
      }

      // Save language in pIVR
      STRLCPY(pIVR->szVSLanguage, szLang, sizeof(szLang) + 1);
   }

   return SUCCESS;
}

/*==============================================================================

Function: IVR_VoiceTagEnumInit

Description:
   Initializes the IVR interface object to enumerate through the voice tag
   database associated with the current application, starting with the first
   voice tag, when IVR_VoiceTagEnumNext is called.

Prototype:
   int IVR_VoiceTagEnumInit(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Always returns SUCCESS.

Comments:
   The ordering of the voice tag database is platform-dependent.

Side Effects:
   None

See Also:
   IVR_VoiceTagEnumNext

==============================================================================*/
static int OEMVR_VoiceTagEnumInit(IVR *pIVR)
{
   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Initialize VS iterator
   if (vdb_vtagi_init(&pIVR->VtagIter) == VDB_NOERR)
   {
      pIVR->bVtagIterValid = TRUE;
   }
   else
   {
      // Should never happen, but just in case
      pIVR->bVtagIterValid = FALSE;
      return EFAILED;
   }

   // Return SUCCESS always, since error should never happen
   return SUCCESS;
}

/*==============================================================================

Function: IVR_VoiceTagEnumNext

Description:
   Gets the next voice tag from the database associated with the current
   application.

Prototype:
   boolean IVR_VoiceTagEnumNext(IVR *pIVR, AEEVRUtterance *pVoiceTag);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pVoiceTag  [out]: Where to return the voice tag.

Return Value:
   TRUE     Successfully returned next voice tag.
   FALSE    No more voice tags to enumerate.

Comments:
   IVR_VoiceTagEnumInit must be called before this function is called for the
   first time.

Side Effects:
   None

See Also:
   AEEVRUtterance, IVR_VoiceTagEnumInit

==============================================================================*/
static boolean OEMVR_VoiceTagEnumNext(IVR *pIVR, AEEVRUtterance *pVoiceTag)
{
   vdb_rwid_type utt;

   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Make sure VS iterator has been initialized and destination is valid
   if (pIVR->bVtagIterValid && pVoiceTag)
   {
      if (vdb_vtagi_next(&pIVR->VtagIter, &utt) == VDB_NOERR)
      {
         // Return next voice tag in pVoiceTag
         *pVoiceTag = ConvertToAEEVRUtterance(utt);
         return TRUE;
      }
   }

   // No more voice tags, or some kind of error
   pIVR->bVtagIterValid = FALSE;

   return FALSE;
}

/*==============================================================================

Function: IVR_RemoveVoiceTag

Description:
   Removes a voice tag from the database associated with the application.

Prototype:
   int IVR_VoiceTagRemove(IVR *pIVR, AEEVRUtterance wVoiceTag);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wVoiceTag   [in]: Voice tag to remove.

Return Value:
   SUCCESS     Voice tag removed successfully.
   EFAILED     Invalid argument.

Comments:
   After a voice tag is removed from the database, it can no longer be
   enumerated or recognized.

Side Effects:
   None

See Also:
   AEEVRUtterance

==============================================================================*/
static int OEMVR_RemoveVoiceTag(IVR *pIVR, AEEVRUtterance wVoiceTag)
{
   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Make sure this is really a voice tag
   if ((wVoiceTag & AEEVRUTTERANCE_TYPE_MASK) != AEEVRUTTERANCE_TYPE_VOICETAG)
   {
      return EFAILED;
   }

   // Remove voice tag
   if (vdb_vtag_remove(ConvertToVSRW(wVoiceTag)) == VDB_NOERR)
   {
      return SUCCESS;
   }

   // Some kind of error (should never happen, but just in case)
   return EFAILED;
}

/*==============================================================================

Function: IVR_GetUtteranceLabel

Description:
   Gets the label data associated with the given utterance.  A label is an
   arbitrary sequence of bytes which has no meaning to the voice recognition
   system itself, and can be used for lookup or identification or whatever
   other purpose the application desires.  By default, all voice tags have an
   empty (zero-length) label associated with them, until IVR_SetUtteranceLabel
   is used to assign a different label.

Prototype:
   int IVR_GetUtteranceLabel(IVR *pIVR, AEEVRUtterance wUtt,
                             uint8 *pLabel, uint32 *nSize);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wUtt        [in]: Utterance to get label for.
   pLabel     [out]: Where to return the label data.
   nSize   [in/out]: Pointer to size (in bytes) of label buffer (pLabel).
                     On return, this contains the size of the label.

Return Value:
   SUCCESS     Label successfully returned.
   EFAILED     Invalid argument or buffer too small.

Comments:
   Currently, voice tags are the only type of utterances supported.  If the
   label buffer is too small to receive the label, this function will fail.
   To guarantee that this does not happen, make the label buffer large enough
   to hold any label set by IVR_SetUtteranceLabel, or use the size specified
   by the wLabelSize field in the AEEVRProperties data obtained via
   IVR_GetProperties.

Side Effects:
   None

See Also:
   AEEVRUtterance, IVR_SetUtteranceLabel, IVR_GetProperties, AEEVRProperties

==============================================================================*/
static int OEMVR_GetUtteranceLabel(IVR *pIVR, AEEVRUtterance wUtt,
                                   uint8 *pLabel, uint32 *nSize)
{
   uint8 buf[VDB_MAX_LABEL_BYTES];
   int len;
   vdb_rw_type RW;
   vdb_err_type err;

   // Verify arguments
   if ((pLabel == NULL) || (wUtt == AEEVRUTTERANCE_NONE))
   {
      return EFAILED;
   }

   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Initialize vdb_rw_type object
   if (vdb_rw_init(&RW, ConvertToVSRW(wUtt)) != VDB_NOERR)
   {
      return EFAILED;
   }

   // Get the label
   err = vdb_rwdata_get_utt_label(&RW.base_rwdata, 0, &len, buf);

   // Release vdb_rw_type object
   vdb_rw_release(&RW);

   // Verify return code, and make sure label fits
   if ((err != VDB_NOERR) || (len > *nSize))
   {
      //If len > *nSize, return ENEEDMORE with nSize giving the required number of addtional bytes.

      return ENEEDMORE;
   }

   // Copy label to output buffer and set size return
   MEMCPY(pLabel, buf, len);
   *nSize = len;

   return SUCCESS;
}

/*==============================================================================

Function: IVR_SetUtteranceLabel

Description:
   Associates a label with the given utterance.  A label is an arbitrary
   sequence of bytes which has no meaning to the voice recognition system
   itself, and can be used for lookup or identification or whatever other
   purpose the application desires.  By default, all voice tags have an empty
   (zero-length) label associated with them, until this function is used to
   assign a different label.

Prototype:
   int IVR_SetUtteranceLabel(IVR *pIVR, AEEVRUtterance wUtt,
                             const uint8 *pLabel, uint32 nSize);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wUtt        [in]: Utterance to set label for.
   pLabel      [in]: Label to set for utterance.
   nSize       [in]: Size of label in bytes.

Return Value:
   SUCCESS     Label successfully set.
   EFAILED     Invalid argument or label too large.

Comments:
   Currently, voice tags are the only type of utterances supported.  The
   label must not be larger than the size specified by the wLabelSize
   field in the AEEVRProperties data obtained via IVR_GetProperties, or
   this function will fail.

Side Effects:
   None

See Also:
   AEEVRUtterance, IVR_GetUtteranceLabel, IVR_GetProperties, AEEVRProperties

==============================================================================*/
static int OEMVR_SetUtteranceLabel(IVR *pIVR, AEEVRUtterance wUtt,
                                   const uint8 *pLabel, uint32 nSize)
{
   vdb_rw_type RW;
   vdb_err_type err;

   // Verify arguments
   if ((pLabel == NULL) || (nSize > VDB_MAX_LABEL_BYTES))
   {
      return EFAILED;
   }

   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Initialize vdb_rw_type object
   if (vdb_rw_init(&RW, ConvertToVSRW(wUtt)) != VDB_NOERR)
   {
      return EFAILED;
   }

   // Set the label
   err = vdb_rwdata_set_utt_label(&RW.base_rwdata, 0, nSize, pLabel);

   // Release vdb_rw_type object
   vdb_rw_release(&RW);

   return (err == VDB_NOERR) ? SUCCESS : EFAILED;
}

/*==============================================================================

Function: IVR_GetUtteranceDuration

Description:
   Returns the duration of the given utterance.  The duration is the number of
   milliseconds it takes to play the utterance.

Prototype:
   uint32 IVR_GetUtteranceDuration(IVR *pIVR, AEEVRUtterance wUtt);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wUtt        [in]: Utterance to get duration of.

Return Value:
   Duration of the utterance in milliseconds.
   (This will be zero if the utterance is invalid or not stored.)

Comments:
   Currently, voice tags are the only type of utterances supported.

Side Effects:
   None

See Also:
   AEEVRUtterance

==============================================================================*/
static uint32 OEMVR_GetUtteranceDuration(IVR *pIVR, AEEVRUtterance wUtt)
{
   vdb_rw_type RW;
   uint32 duration;

   // Return immediately for invalid argument
   if (wUtt == AEEVRUTTERANCE_NONE)
   {
      return 0;
   }

   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Initialize vdb_rw_type object
   if (vdb_rw_init(&RW, ConvertToVSRW(wUtt)) != VDB_NOERR)
   {
      return 0;
   }

   // Get duration in milliseconds
   duration = (vdb_rwdata_utt_frames(&RW.base_rwdata, 0) * 20);

   // Release vdb_rw_type object
   vdb_rw_release(&RW);

   return duration;
}

/*==============================================================================

Function: IVR_GetProperties

Description:
   Retrieves the VR system properties.

Prototype:
   int IVR_GetProperties(IVR *pIVR, AEEVRProperties *pProps);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pProps  [in/out]: Pointer to data structure to receive the VR system
                     properties.  On input, the wStructSize field must be set
                     to size of structure in bytes.

Return Value:
   SUCCESS     Properties successfully returned.
   EFAILED     Invalid properties structure.

Comments:
   The properties values are implementation-dependent.

Side Effects:
   None

See Also:
   AEEVRProperties

==============================================================================*/
static int OEMVR_GetProperties(IVR *pIVR, AEEVRProperties *pProps)
{
   uint16 len;

   // Verify properties structure
   if ((pProps == NULL) || (pProps->wStructSize == 0))
   {
      return EFAILED;
   }

   // Get length of properties structure to copy.
   // (Caller could have different version of properties structure.)
   len = MIN(pProps->wStructSize, sizeof(AEEVRProperties));

   // Copy properties structure to output buffer and set size field
   MEMCPY(pProps, &gProperties, len);
   pProps->wStructSize = len;

   return SUCCESS;
}

/*==============================================================================

Function: IVR_SetVoiceTagDB

Description:
   Sets up the VR system to use the specified path for its voice tag database.
   Note:  The caller is required to have the System privilege level (PL_SYSTEM)
   to use this function.

Prototype:
   int IVR_SetVoiceTagDB(IVR *pIVR, const char *szPath);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   szPath      [in]: ASCII string giving full path name of voice tag database.

Return Value:
   SUCCESS     Voice tag database successfully set.
   EFAILED     Invalid path.
   EPRIVLEVEL  Caller does not have the required PL_SYSTEM privilege level.

Comments:
   Normally, the IVR interface puts the voice tag database in a default path
   in the application's home directory.  This function allows a system
   application to override this default behavior.  It should be called before
   calling any voice tag enumeration functions or starting any other VR
   operation.

Side Effects:
   None

See Also:
   None

==============================================================================*/
static int OEMVR_SetVoiceTagDB(IVR *pIVR, const char *szPath)
{
   char szHackPath[VDB_CLIENT_NAME_MAX_LENGTH + 1];
   int len = STRLEN(szPath);

   // Verify PL_SYSTEM privilege level
   if (!ISHELL_CheckPrivLevel(pIVR->pIShell, PL_SYSTEM, TRUE))
   {
      return EPRIVLEVEL;
   }

   // Verify legal length of desired path
   if ((len == 0) || (len > VDB_CLIENT_NAME_MAX_LENGTH))
   {
      return EFAILED;
   }

   // HACK:  With current FS, vdb requires pathname length to be odd
   if ((len & 1) == 0)
   {
      // Truncate path by one char
      STRLCPY(szHackPath, szPath, STRLEN(szPath) + 1);
      szHackPath[len - 1] = '\0';
      szPath = szHackPath;
   }

   // No need to do anything if new path matches current pIVR setting
   if (STRCMP(szPath, pIVR->szVSClientDBPath))
   {
      // No need to attempt to set new path if already set
      if (STRCMP(szPath, szVSClientDBPath))
      {
         // Make sure VS isn't in the middle of something
         SetCurrentInstance(NULL);

         // Attempt to set new path using VS
         if (vdb_set_current_client_db(szPath) != VDB_NOERR)
         {
            szVSClientDBPath[0] = '\0';
            return EFAILED;
         }

         // Set current path to new setting
         STRLCPY(szVSClientDBPath, szPath,STRLEN(szPath) + 1);
      }

      // Save path in pIVR
      STRLCPY(pIVR->szVSClientDBPath, szPath,STRLEN(szPath) + 1);
   }

   return SUCCESS;
}

/*==============================================================================

Function: OEMVR_SetPrompt

Description:
   Configures the VR system to use the given list of sounds for the specified
   prompt.  This allows the application to customize the behavior of subsequent
   VR operations that are started with the given instance of the IVR interface
   object.

Prototype:
   int OEMVR_SetPrompt(IVR *pIVR, OEMVRPrompt wPrompt,
                       const OEMVRSound *pSoundData);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wPrompt     [in]: Which prompt to configure.
   pSoundData  [in]: Array of sounds to be used as the specified prompt,
                     terminated by an element with wUtt set to
                     AEEVRUTTERANCE_NONE and pToneData set to NULL.
                     If pSoundData is NULL, then the prompt is set back to its
                     default.  If pSoundData points to a single terminator
                     element, the prompt is set to nothing (silence).

Return Value:
   SUCCESS    Prompt successfully set.
   EFAILED    Invalid arguments.
   ENOMEMORY  Out of memory.

Comments:
   For some prompts, a voice tag RWID associated with the current operation is
   substituted at a place in the prompt where there is no sound or tone.  This
   place is specified in pSoundData with an element where wUtt is set to
   AEEVRUTTERANCE_TYPE_VOICETAG, thus distinguishing it from the terminator
   element. See the DMSS VS ISOD for information on which prompts are used in
   which operations.

Side Effects:
   None

See Also:
   OEMVRPrompt, OEMVRSound, IVR_TrainVoiceTag, IVR_RecognizeWithPrompts,
   IVR_YesNoConfirm, IVR_VoiceMenuSelect, OEMVR_TrainControlWord,
   OEMVR_RecognizeUtteranceMulti, OEMVR_VoiceActivate

==============================================================================*/
int OEMVR_SetPrompt(IVR *pIVR, OEMVRPrompt wPrompt,
                    const OEMVRSound *pSoundArray)
{
   vs_sound_type *pNewPrompt = NULL;
   int err;

   // Check for invalid wPrompt
   if ((wPrompt < OEMVRPROMPT_BASE) || (wPrompt >= OEMVRPROMPT_LIMIT))
   {
      return EFAILED;
   }

   // Create new prompt data if needed
   if (pSoundArray)
   {
      if ((err = CreateVSSoundArray(&pNewPrompt, pSoundArray)) != SUCCESS)
      {
         return err;
      }
   }
   else
   {
      pNewPrompt = (vs_sound_type *)pDefaultPrompt[wPrompt];
   }

   // Free old prompt data if needed
   if (pIVR->pPrompt[wPrompt] != pDefaultPrompt[wPrompt])
   {
      FREEIF(pIVR->pPrompt[wPrompt]);
   }

   // Set the new prompt
   pIVR->pPrompt[wPrompt] = pNewPrompt;

   return SUCCESS;
}

/*==============================================================================

Function: OEMVR_PlaySounds

Description:
   Starts an asynchronous operation to audibly play the given sounds.  When
   finished, the status can be obtained with IVR_GetOperationStatus or via the
   callback registered with IVR_RegisterNotify.

Prototype:
   int OEMVR_PlaySounds(IVR *pIVR, const OEMVRSound *pSoundArray);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pSoundArray [in]: Array of sounds to be played, terminated by an element
                     with wUtt set to AEEVRUTTERANCE_NONE and pToneData set to
                     NULL.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Playback completed successfully.
      AEEVRSTATUS_FAILED      Playback failed.
      AEEVRSTATUS_ABORTED     Playback was aborted with IVR_AbortOperation.

Comments:
   The playback device and volume are controlled through the ISound interface.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   OEMVRSound, IVR_PlayUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation, ISound

==============================================================================*/
int OEMVR_PlaySounds(IVR *pIVR, const OEMVRSound *pSoundArray)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_PLAY_SOUNDS);

   if (CreateVSSoundArray(&pIVR->pSound, pSoundArray) == SUCCESS)
   {
      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: OEMVR_TrainControlWord

Description:
   Starts an asynchronous operation to train or re-train the given control word.
   This is similar to IVR_TrainVoiceTag, but for a given control word instead
   of a new voice tag.  When finished, the status can be obtained with
   IVR_GetOperationStatus or via the callback registered with
   IVR_RegisterNotify.

Prototype:
   int OEMVR_TrainControlWord(IVR *pIVR, const AEEVRUtterance *pDupList,
                              AEEVRUtterance wCWord);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pDupList    [in]: Array of utterances to check against for duplicates,
                     terminated by AEEVRUTTERANCE_NONE (must be non-NULL).
                     If pDupList points to a single element with the value
                     AEEVRUTTERANCE_NONE, then no duplicate-checking is
                     performed.
   wCWord      [in]: The control word to train.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Training completed.
      AEEVRSTATUS_FAILED      Training failed.
      AEEVRSTATUS_ABORTED     Training was aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING     Training still in progress (used to report
                              intermediate failures).

Comments:
   None

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation, IVR_TrainVoiceTag

==============================================================================*/
int OEMVR_TrainControlWord(IVR *pIVR, const AEEVRUtterance *pDupList,
                           AEEVRUtterance wCWord)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_TRAIN_CWORD);

   // Make sure this is really a control word
   if ((wCWord & AEEVRUTTERANCE_TYPE_MASK) != AEEVRUTTERANCE_TYPE_CWORD)
   {
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   // Set up RWLists, checking for errors
   if ((CreateVSRWList(&pIVR->RWList[0], pDupList,
                       AEEVRUTTERANCE_TYPE_VOICETAG) == SUCCESS) &&
       (CreateVSRWList(&pIVR->RWList[1], pDupList,
                       AEEVRUTTERANCE_TYPE_CWORD) == SUCCESS))
   {
      // Save control word as utterance argument
      pIVR->wUtteranceArg = ConvertToVSRW(wCWord);

      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: OEMVR_RecognizeUtteranceMulti

Description:
   Starts an asynchronous operation to recognize multiple voice tags.  This is
   similar to IVR_RecognizeWithPrompts, except that it does not complete after
   the successful recognition of an utterance, but rather continues by
   attempting to recognize another utterance, until encountering a failure or
   being explicitly aborted.  Whenever a voice tag is recognized, it can be
   obtained with IVR_GetOperationStatus or via the callback registered with
   IVR_RegisterNotify.

Prototype:
   int OEMVR_RecognizeUtteranceMulti(IVR *pIVR, const AEEVRUtterance *pRecogList,
                                     const AEEVRUtterance *pPlayList,
                                     boolean bShortGap);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pRecogList  [in]: Array of utterances to recognize, terminated by
                     AEEVRUTTERANCE_NONE (must be non-NULL).
   pPlayList   [in]: Array of utterances to be played after a recognized
                     utterance, with each item corresponding to the item of the
                     same index in pRecogList above.  If NULL, pRecogList is
                     used, meaning the recognized utterance will be echoed.
                     Note:  This list may contain elements without any utterance
                     ID, but these elements must contain a valid utterance type
                     (e.g. AEEVRUTTERANCE_TYPE_PROMPT).
   bShortGap   [in]: If TRUE, assumes shorter intra-word gaps, which allows
                     faster recognition of short words like digits.  If FALSE,
                     assumes default longer intra-word gap.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_FAILED      Recognizing failed.
      AEEVRSTATUS_ABORTED     Recognizing was aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING     Recognizing still in progress (used to report
                              recognized utterance or intermediate failures).

Comments:
   None

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation

==============================================================================*/
int OEMVR_RecognizeUtteranceMulti(IVR *pIVR, const AEEVRUtterance *pRecogList,
                                  const AEEVRUtterance *pPlayList,
                                  boolean bShortGap)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_RECOGNIZE_MULTI);

   // If pPlayList is NULL, use pRecogList
   if (pPlayList == NULL)
   {
      pPlayList = pRecogList;
   }

   // Set up RWLists, checking for errors
   // Note:  Only control words supported for now
   if ((pRecogList) &&
       (CreateVSRWList(&pIVR->RWList[0], pRecogList,
                       AEEVRUTTERANCE_TYPE_CWORD) == SUCCESS) &&
       (CreateVSRWList(&pIVR->RWList[1], pPlayList,
                       AEEVRUTTERANCE_TYPE_PROMPT) == SUCCESS))
   {
      // Save multiple recognition type argument
      pIVR->wMultRecArg = (bShortGap ? VS_RECOG_DIGIT : VS_RECOG_DEFAULT);

      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: OEMVR_VoiceActivate

Description:
   Starts an asynchronous operation to wait until a certain command is spoken,
   to "wakeup" or "activate" the voice application, for example. The command
   must be spoken twice, after which the callback is invoked.  When finished,
   the status can be obtained with IVR_GetOperationStatus or via the callback
   registered with IVR_RegisterNotify.

Prototype:
   int OEMVR_VoiceActivate(IVR *pIVR, const AEEVRUtterance *pRecogList,
                           AEEVRUtterance wUtt);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   pRecogList  [in]: Array of voice tags to recognize, terminated by
                     AEEVOICETAG_NONE (must be non-NULL).
   wUtt        [in]: The utterance to recognize as the wakeup word.

Return Value:
   Always returns SUCCESS.
   Additional results are thereafter reported via the callback registered
   with IVR_RegisterNotify, and will be one of the following:
      AEEVRSTATUS_DONE        Recognizing completed.
      AEEVRSTATUS_FAILED      Recognizing failed.
      AEEVRSTATUS_ABORTED     Recognizing was aborted with IVR_AbortOperation.
      AEEVRSTATUS_RUNNING     Recognizing still in progress (used to report
                              intermediate failures).

Comments:
   This function is primarily intended for use with the hands-free kit.

Side Effects:
   If any other VR operation is in progress when this function is called, it is
   aborted.

See Also:
   AEEVRUtterance, IVR_RegisterNotify, IVR_GetOperationStatus,
   IVR_AbortOperation

==============================================================================*/
int OEMVR_VoiceActivate(IVR *pIVR, const AEEVRUtterance* pRecogList,
                        AEEVRUtterance wUtt)
{
#if defined (FEATURE_ACM)
   if (ACM_Acquire(pIVR, OEMVRASYNCOP_TRAIN_VTAG) != SUCCESS)
   {
      // IVR doesn't have priority to run
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }
#endif // FEATURE_ACM

   // Set up for asynchronous operation
   SetAsyncOp(pIVR, OEMVRASYNCOP_VOICE_ACTIVATE);

   // Set up RWList, checking for errors
   // Note:  Only control words supported for now
   if ((pRecogList) &&
       (CreateVSRWList(&pIVR->RWList[0], pRecogList,
                       AEEVRUTTERANCE_TYPE_CWORD) == SUCCESS))
   {
      // Save utterance argument
      pIVR->wUtteranceArg = ConvertToVSRW(wUtt);

      // Call VS function to do the work
      StartVSOperation(pIVR);
   }
   else
   {
      // Set failure result due to argument error
      SetAsyncResult(pIVR, AEEVRSTATUS_FAILED, AEEVRFAILURE_ILLEGAL_ARGS,
                     AEEVRUTTERANCE_NONE);
   }

   return SUCCESS;
}

/*==============================================================================

Function: OEMVR_GetMaxRecommendedVocab

Description:
   Returns the maximum recommended number of utterances to try to recognize
   against for the the current language on the current platform.

Prototype:
   uint32 OEMVR_GetMaxRecommendedVocab(IVR *pIVR);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.

Return Value:
   Maximum recommended number of words to try to recognize.

Comments:
   None

Side Effects:
   None

See Also:
   IVR_SetLanguage

==============================================================================*/
uint32 OEMVR_GetMaxRecommendedVocab(IVR *pIVR)
{
   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Return recommended max vocabulary size from VS
   return vs_get_max_vocabulary_size();
}

/*==============================================================================

Function: OEMVR_GetUtteranceNumBytes

Description:
   Returns the number of bytes that are used to store the given utterance.

Prototype:
   uint32 OEMVR_GetUtteranceNumBytes(IVR *pIVR, AEEVRUtterance wUtt);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wUtt        [in]: Utterance to get number of bytes for.

Return Value:
   Number of bytes used to store the utterance.
   (This will be zero if the utterance is invalid or not stored.)

Comments:
   None

Side Effects:
   None

See Also:
   AEEVRUtterance

==============================================================================*/
uint32 OEMVR_GetUtteranceNumBytes(IVR *pIVR, AEEVRUtterance wUtt)
{
   vdb_rw_type RW;
   int length;

   // Return immediately for invalid argument
   if (wUtt == AEEVRUTTERANCE_NONE)
   {
      return 0;
   }

   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Initialize vdb_rw_type object
   if (vdb_rw_init(&RW, ConvertToVSRW(wUtt)) != VDB_NOERR)
   {
      return 0;
   }

   // Get length in bytes
   length = vdb_rwdata_utt_length(&RW.base_rwdata, 0);

   // Release vdb_rw_type object
   vdb_rw_release(&RW);

   return length;
}

/*==============================================================================

Function: OEMVR_UntrainControlWord

Description:
   Removes the speaker-dependent voice pattern associated with a control word,
   so that the control word can no longer be recognized in speaker-dependent
   fashion, until it is retrained with OEMVR_TrainControlWord.

Prototype:
   int OEMVR_UntrainControlWord(IVR *pIVR, AEEVRUtterance wCWord);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   wCWord      [in]: Control word to untrain.

Return Value:
   SUCCESS     Control word untrained successfully.
   EFAILED     Invalid argument.

Comments:
   After a control word is untrained, it can still be recognized in
   speaker-independent fashion.

Side Effects:
   None

See Also:
   AEEVRUtterance, OEMVR_TrainControlWord

==============================================================================*/
int OEMVR_UntrainControlWord(IVR *pIVR, AEEVRUtterance wCWord)
{
   // Configure VS for current instance before making any VS calls
   SetCurrentInstance(pIVR);

   // Make sure this is really a control word
   if ((wCWord & AEEVRUTTERANCE_TYPE_MASK) != AEEVRUTTERANCE_TYPE_CWORD)
   {
      return EFAILED;
   }

   // Remove pattern data associated with control word
   if (vdb_rw_remove_pattern(ConvertToVSRW(wCWord)) == VDB_NOERR)
   {
      return SUCCESS;
   }

   // Some kind of error (should never happen, but just in case)
   return EFAILED;
}

/*==============================================================================

Function: OEMVR_SetSpeakerIndependent

Description:
   Sets up the VR system to enable or disable recognizing control words in
   speaker-independent fashion.  Speaker-independent recognition is enabled by
   default, and uses pre-stored speaker-independent patterns in addition to the
   speaker-dependent patterns created by OEMVR_TrainControlWord.

Prototype:
   int OEMVR_SetSpeakerIndependent(IVR *pIVR, boolean bSIEnable);

Parameters:
   pIVR        [in]: Pointer to IVR interface object.
   bSIEnable   [in]: TRUE to enable speaker-independent recognition, FALSE to
                     disable.

Return Value:
   Always returns SUCCESS.

Comments:
   Speaker-dependent recognition is always enabled.

Side Effects:
   None

See Also:
   OEMVR_TrainControlWord

==============================================================================*/
int OEMVR_SetSpeakerIndependent(IVR *pIVR, boolean bSIEnable)
{
   vs_patt_settings_type wPatternSet;

   // Get VS pattern setting based on whether or not SI is enabled
   wPatternSet = bSIEnable ? VS_PATT_SD_SI : VS_PATT_SD;

   // No need to do anything if new setting matches current pIVR setting
   if (wPatternSet != pIVR->wVSPatternSet)
   {
      // Save pattern setting in pIVR
      pIVR->wVSPatternSet = wPatternSet;

      // See if current pattern setting in VS needs to be changed
      if (wPatternSet != wVSPatternSet)
      {
         // Make sure VS isn't in the middle of something, and let
         // SetCurrentInstance take care of new VS setting the next time it
         // gets called.
         SetCurrentInstance(NULL);
      }
   }

   return SUCCESS;
}

#if defined (FEATURE_ACM)
/*==============================================================================
Description:
   This function tells ACM we need to acquire the resource for the operation.
   Sets pIVR->bResourceAcquired if the resource is acquired.
   Returns SUCCESS if the resource is acquired or pending, else returns error.
==============================================================================*/
static int ACM_Acquire(IVR *pIVR, OEMVRAsyncOp vrOp)
{
   ACM_resource_state_type  stateAcm = ACM_RESOURCE_UNKNOWN;
#if defined (FEATURE_SIRIUS)
#error code not present
#else
   ACM_acquire_reason_code  reasonCode;
#endif // FEATURE_SIRIUS

   // Ensure resource acquired flag set to FALSE by default
   pIVR->bResourceAcquired = FALSE;

   // Get reason code appropriate for operation
   switch (vrOp)
   {
      // Training operations
      case OEMVRASYNCOP_TRAIN_VTAG:
      case OEMVRASYNCOP_TRAIN_CWORD:
         reasonCode = ACM_VOICE_TRAIN;
         break;

      // Recognizing operations
      case OEMVRASYNCOP_RECOGNIZE_PRIMITIVE:
      case OEMVRASYNCOP_RECOGNIZE_WITH_PROMPTS:
      case OEMVRASYNCOP_YES_NO_CONFIRM:
      case OEMVRASYNCOP_MENU_SELECT:
      case OEMVRASYNCOP_RECOGNIZE_MULTI:
      case OEMVRASYNCOP_VOICE_ACTIVATE:
         reasonCode = ACM_VOICE_DIAL;
         break;

      // Playing operations
      case OEMVRASYNCOP_PLAY_UTTERANCE:
      case OEMVRASYNCOP_PLAY_SOUNDS:
         reasonCode = ACM_VOICE_PLAY_UTTERANCE;
         break;

      // If a new operation is added, must add support in ACM functions
      default:
         return EBADPARM;
   }

#if defined (FEATURE_SIRIUS)
#error code not present
#else // !SIRIUS_PLATOFRM
   if (OEMACM_PriorityMgr_Acquire(pIVR, reasonCode, &stateAcm) != SUCCESS)
   {
      return EFAILED;
   }

   if (stateAcm == ACM_RESOURCE_BUSY)
   {
      return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
   }

   // ACM_RESOURCE_ACQUIRING and ACM_RESOURCE_ACQUIRED
   pIVR->mAcmCB.pdata = pIVR;
   pIVR->mAcmCB.state = ACM_RESOURCE_UNKNOWN;
   if (OEMACM_StateMgr_OnStatusChangeNotify(pIVR,
                                            (PFNNOTIFY)ACM_StateChangedCB,
                                            &(pIVR->mAcmCB)) != SUCCESS)
   {
      return EFAILED;
   }

   if (stateAcm == ACM_RESOURCE_ACQUIRED)
   {
      // Immediate Acquire
      pIVR->bResourceAcquired = TRUE;
   }
   
   return SUCCESS;
#endif // FEATURE_SIRIUS
}

/*==============================================================================
Description:
   This function is called from ACM when a pending resource is acquired or a
   pending or acquired resource is forced to release.
==============================================================================*/
static void ACM_StateChangedCB(ACM_Callback_data_type *po)
{
   IVR *pIVR = (IVR *)(po->pdata);

   if (po->state == ACM_RESOURCE_ACQUIRED)
   {
      // Resource is now acquired, start the VS operation
      pIVR->bResourceAcquired = TRUE;
      StartVSOperation(pIVR);
   }
   else
   {
      // ACM is forcing us to release resource (needed by higher-priority app)
      // We simulate an abort in this case
      OEMVR_AbortOperation(pIVR);
   }
}

#endif // FEATURE_ACM
#endif // FEATURE_PHONE_VR

