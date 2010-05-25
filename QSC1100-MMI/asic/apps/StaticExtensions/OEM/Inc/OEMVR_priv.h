#ifndef  OEMVR_PRIV_H
#define  OEMVR_PRIV_H
/*==============================================================================
FILE:          OEMVR_priv.h

SERVICES:      Voice recognition and related services.

DESCRIPTION:   Definitions for extended VR interface not exposed in IVR.


        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "AEEVR.h"
#include "AEESound.h"

/*------------------------------------------------------------------------------
   Data Type Definitions
------------------------------------------------------------------------------*/

// AEEVRUtterance values (extended types not defined in IVR)
#define AEEVRUTTERANCE_TYPE_PROMPT     0x20000
#define AEEVRUTTERANCE_TYPE_CWORD      0x30000

// AEEVRUtterance values (prompt IDs)
#define AEEVRUTTERANCE_FIRST_PROMPT                         (0x20000 + 1)
#define AEEVRUTTERANCE_ID_RECORD_AT_TONE                    (0x20000 + 1)
#define AEEVRUTTERANCE_ID_LIMIT_WARNING                     (0x20000 + 2)
#define AEEVRUTTERANCE_ID_LIMIT_REACHED                     (0x20000 + 3)
#define AEEVRUTTERANCE_ID_DO_YOU_WANT_ACTION                (0x20000 + 4)
#define AEEVRUTTERANCE_ID_WAS_THAT_A_YES                    (0x20000 + 5)
#define AEEVRUTTERANCE_ID_GOT_IT                            (0x20000 + 6)
#define AEEVRUTTERANCE_ID_TRAIN_CONTROL_WORDS               (0x20000 + 7)
#define AEEVRUTTERANCE_ID_SAY_THE_WORD                      (0x20000 + 8)
#define AEEVRUTTERANCE_ID_TRAINING_WAS_SUCCESSFUL           (0x20000 + 9)
#define AEEVRUTTERANCE_ID_NAME_PLEASE                       (0x20000 + 10)
#define AEEVRUTTERANCE_ID_AGAIN                             (0x20000 + 11)
#define AEEVRUTTERANCE_ID_SIMILAR_NAME_1                    (0x20000 + 12)
#define AEEVRUTTERANCE_ID_SIMILAR_NAME_2                    (0x20000 + 13)
#define AEEVRUTTERANCE_ID_NBEST_CONFIRMATION                (0x20000 + 14)
#define AEEVRUTTERANCE_ID_ADDED                             (0x20000 + 15)
#define AEEVRUTTERANCE_ID_CALLING                           (0x20000 + 16)
#define AEEVRUTTERANCE_ID_CALLING_FLAT                      (0x20000 + 17)
#define AEEVRUTTERANCE_ID_NO_NUM_TO_CALL                    (0x20000 + 18)
#define AEEVRUTTERANCE_ID_BEEP                              (0x20000 + 19)
#define AEEVRUTTERANCE_ID_DID_YOU_SAY_CANCEL                (0x20000 + 20)
#define AEEVRUTTERANCE_ID_CANCELLED                         (0x20000 + 21)
#define AEEVRUTTERANCE_ID_INCOMING_CALL_ANSWER              (0x20000 + 22)
#define AEEVRUTTERANCE_ID_INCOMING_ROAM_CALL_ANSWER         (0x20000 + 23)
#define AEEVRUTTERANCE_ID_SOH                               (0x20000 + 24)
#define AEEVRUTTERANCE_ID_SZERO                             (0x20000 + 25)
#define AEEVRUTTERANCE_ID_SONE                              (0x20000 + 26)
#define AEEVRUTTERANCE_ID_STWO                              (0x20000 + 27)
#define AEEVRUTTERANCE_ID_STHREE                            (0x20000 + 28)
#define AEEVRUTTERANCE_ID_SFOUR                             (0x20000 + 29)
#define AEEVRUTTERANCE_ID_SFIVE                             (0x20000 + 30)
#define AEEVRUTTERANCE_ID_SSIX                              (0x20000 + 31)
#define AEEVRUTTERANCE_ID_SSEVEN                            (0x20000 + 32)
#define AEEVRUTTERANCE_ID_SEIGHT                            (0x20000 + 33)
#define AEEVRUTTERANCE_ID_SNINE                             (0x20000 + 34)
#define AEEVRUTTERANCE_ID_SPOUND                            (0x20000 + 35)
#define AEEVRUTTERANCE_ID_SSTAR                             (0x20000 + 36)
#define AEEVRUTTERANCE_ID_SAY_NAME                          (0x20000 + 37)
#define AEEVRUTTERANCE_ID_SPK_NAME_CALL_DIAL                (0x20000 + 38)
#define AEEVRUTTERANCE_ID_REDIAL_VMEMO_PBOOK_CNCL           (0x20000 + 39)
#define AEEVRUTTERANCE_ID_COMMAND                           (0x20000 + 40)
#define AEEVRUTTERANCE_ID_OR                                (0x20000 + 41)
#define AEEVRUTTERANCE_ID_SAY_DIGITS_OR                     (0x20000 + 42)
#define AEEVRUTTERANCE_ID_VER_CAN_CL_CALL                   (0x20000 + 43)
#define AEEVRUTTERANCE_ID_CLEARED                           (0x20000 + 44)
#define AEEVRUTTERANCE_ID_CLEAR_ENTIRE_PHN_NUM              (0x20000 + 45)
#define AEEVRUTTERANCE_ID_ALL_DGTS_CLRD                     (0x20000 + 46)
#define AEEVRUTTERANCE_ID_SINGLE_DGT_CLRD                   (0x20000 + 47)
#define AEEVRUTTERANCE_ID_NO_PREV_NUM_TO_REDIAL             (0x20000 + 48)
#define AEEVRUTTERANCE_ID_PBOOK_IS_EMPTY                    (0x20000 + 49)
#define AEEVRUTTERANCE_ID_LISTING_PBOOK                     (0x20000 + 50)
#define AEEVRUTTERANCE_ID_VOICE_MEMO_MENU                   (0x20000 + 51)
#define AEEVRUTTERANCE_ID_DIGIT_MENU                        (0x20000 + 52)
#define AEEVRUTTERANCE_ID_MAIN_MENU                         (0x20000 + 53)
#define AEEVRUTTERANCE_LAST_PROMPT                          (0x20000 + 53)

// AEEVRUtterance values (control word IDs)
#define AEEVRUTTERANCE_FIRST_CWORD                          (0x30000 + 54)
#define AEEVRUTTERANCE_ID_YES                               (0x30000 + 54)
#define AEEVRUTTERANCE_ID_NO                                (0x30000 + 55)
#define AEEVRUTTERANCE_ID_WAKEUP                            (0x30000 + 56)
#define AEEVRUTTERANCE_ID_CANCEL                            (0x30000 + 57)
#define AEEVRUTTERANCE_ID_PHONEBOOK                         (0x30000 + 58)
#define AEEVRUTTERANCE_ID_REDIAL                            (0x30000 + 59)
#define AEEVRUTTERANCE_ID_DIAL                              (0x30000 + 60)
#define AEEVRUTTERANCE_ID_ONE                               (0x30000 + 61)
#define AEEVRUTTERANCE_ID_TWO                               (0x30000 + 62)
#define AEEVRUTTERANCE_ID_THREE                             (0x30000 + 63)
#define AEEVRUTTERANCE_ID_FOUR                              (0x30000 + 64)
#define AEEVRUTTERANCE_ID_FIVE                              (0x30000 + 65)
#define AEEVRUTTERANCE_ID_SIX                               (0x30000 + 66)
#define AEEVRUTTERANCE_ID_SEVEN                             (0x30000 + 67)
#define AEEVRUTTERANCE_ID_EIGHT                             (0x30000 + 68)
#define AEEVRUTTERANCE_ID_NINE                              (0x30000 + 69)
#define AEEVRUTTERANCE_ID_ZERO                              (0x30000 + 70)
#define AEEVRUTTERANCE_ID_OH                                (0x30000 + 71)
#define AEEVRUTTERANCE_ID_POUND                             (0x30000 + 72)
#define AEEVRUTTERANCE_ID_STAR                              (0x30000 + 73)
#define AEEVRUTTERANCE_ID_CLEAR                             (0x30000 + 74)
#define AEEVRUTTERANCE_ID_VERIFY                            (0x30000 + 75)
#define AEEVRUTTERANCE_ID_CALL                              (0x30000 + 76)
#define AEEVRUTTERANCE_ID_VOICE_MEMO                        (0x30000 + 77)
#define AEEVRUTTERANCE_LAST_CWORD                           (0x30000 + 77)

// OEMVRSound data type
typedef struct
{
   AEEVRUtterance    wUtt;
   AEESoundToneData *pToneData;
} OEMVRSound;

// OEMVRPrompt data type
typedef uint32 OEMVRPrompt;

// OEMVRPrompt values
#define OEMVRPROMPT_NONE                              0
#define OEMVRPROMPT_BASE                              1

// OEMVRPrompt values (for vs_train_utterance and vs_train_cword)
#define OEMVRPROMPT_TRAIN_INITIAL                     (OEMVRPROMPT_BASE + 0)
#define OEMVRPROMPT_TRAIN_INITIAL_TIMEOUT             (OEMVRPROMPT_BASE + 1)
#define OEMVRPROMPT_TRAIN_INITIAL_TOO_SOON            (OEMVRPROMPT_BASE + 2)
#define OEMVRPROMPT_TRAIN_INITIAL_TOO_LONG            (OEMVRPROMPT_BASE + 3)
#define OEMVRPROMPT_TRAIN_INITIAL_TOO_SHORT           (OEMVRPROMPT_BASE + 4)
#define OEMVRPROMPT_TRAIN_2ND_TRY                     (OEMVRPROMPT_BASE + 5)
#define OEMVRPROMPT_TRAIN_2ND_TRY_TIMEOUT             (OEMVRPROMPT_BASE + 6)
#define OEMVRPROMPT_TRAIN_2ND_TRY_TOO_SOON            (OEMVRPROMPT_BASE + 7)
#define OEMVRPROMPT_TRAIN_2ND_TRY_TOO_LONG            (OEMVRPROMPT_BASE + 8)
#define OEMVRPROMPT_TRAIN_2ND_TRY_TOO_SHORT           (OEMVRPROMPT_BASE + 9)
#define OEMVRPROMPT_TRAIN_UNMATCHED                   (OEMVRPROMPT_BASE + 10)

// OEMVRPrompt values (for vs_recognize_utterance)
#define OEMVRPROMPT_RECOGNIZE_INITIAL                 (OEMVRPROMPT_BASE + 11)
#define OEMVRPROMPT_RECOGNIZE_INITIAL_TIMEOUT         (OEMVRPROMPT_BASE + 12)
#define OEMVRPROMPT_RECOGNIZE_INITIAL_TOO_SOON        (OEMVRPROMPT_BASE + 13)
#define OEMVRPROMPT_RECOGNIZE_INITIAL_TOO_LONG        (OEMVRPROMPT_BASE + 14)
#define OEMVRPROMPT_RECOGNIZE_INITIAL_TOO_SHORT       (OEMVRPROMPT_BASE + 15)
#define OEMVRPROMPT_RECOGNIZE_INITIAL_UNMATCHED       (OEMVRPROMPT_BASE + 16)
#define OEMVRPROMPT_RECOGNIZE_YESNO                   (OEMVRPROMPT_BASE + 17)
#define OEMVRPROMPT_RECOGNIZE_YESNO_TIMEOUT           (OEMVRPROMPT_BASE + 18)
#define OEMVRPROMPT_RECOGNIZE_YESNO_TOO_SOON          (OEMVRPROMPT_BASE + 19)
#define OEMVRPROMPT_RECOGNIZE_YESNO_TOO_LONG          (OEMVRPROMPT_BASE + 20)
#define OEMVRPROMPT_RECOGNIZE_YESNO_TOO_SHORT         (OEMVRPROMPT_BASE + 21)
#define OEMVRPROMPT_RECOGNIZE_YESNO_UNMATCHED         (OEMVRPROMPT_BASE + 22)
#define OEMVRPROMPT_RECOGNIZE_YESNO_CONFIRM           (OEMVRPROMPT_BASE + 23)

// OEMVRPrompt values (for vs_implicit_yes)
#define OEMVRPROMPT_YESNO_INITIAL                     (OEMVRPROMPT_BASE + 24)
#define OEMVRPROMPT_YESNO_INITIAL_TIMEOUT             (OEMVRPROMPT_BASE + 25)
#define OEMVRPROMPT_YESNO_INITIAL_TOO_SOON            (OEMVRPROMPT_BASE + 26)
#define OEMVRPROMPT_YESNO_INITIAL_TOO_LONG            (OEMVRPROMPT_BASE + 27)
#define OEMVRPROMPT_YESNO_INITIAL_TOO_SHORT           (OEMVRPROMPT_BASE + 28)
#define OEMVRPROMPT_YESNO_INITIAL_UNMATCHED           (OEMVRPROMPT_BASE + 29)
#define OEMVRPROMPT_YESNO_YESNO_TIMEOUT               (OEMVRPROMPT_BASE + 30)

// OEMVRPrompt values (for vs_menu_selection)
#define OEMVRPROMPT_MENU_INITIAL                      (OEMVRPROMPT_BASE + 31)
#define OEMVRPROMPT_MENU_UNMATCHED                    (OEMVRPROMPT_BASE + 32)

// OEMVRPrompt values (for vs_recognize_multi)
#define OEMVRPROMPT_MULTI_INITIAL                     (OEMVRPROMPT_BASE + 33)
#define OEMVRPROMPT_MULTI_UNMATCHED                   (OEMVRPROMPT_BASE + 34)
#define OEMVRPROMPT_MULTI_TIMEOUT                     (OEMVRPROMPT_BASE + 35)
#define OEMVRPROMPT_MULTI_CORRESPONDENT               (OEMVRPROMPT_BASE + 36)

// OEMVRPrompt values (for vs_voice_activate)
#define OEMVRPROMPT_ACTIVATE_1ST_CONFIRM              (OEMVRPROMPT_BASE + 37)
#define OEMVRPROMPT_ACTIVATE_2ND_CONFIRM              (OEMVRPROMPT_BASE + 38)

// OEMVRPrompt values
#define OEMVRPROMPT_LIMIT                             (OEMVRPROMPT_BASE + 39)


/*------------------------------------------------------------------------------
   Interface Definitions
------------------------------------------------------------------------------*/

int OEMVR_SetPrompt(IVR*, OEMVRPrompt, const OEMVRSound*);
int OEMVR_PlaySounds(IVR*, const OEMVRSound*);
int OEMVR_TrainControlWord(IVR*, const AEEVRUtterance*, AEEVRUtterance);
int OEMVR_RecognizeUtteranceMulti(IVR*, const AEEVRUtterance*, const AEEVRUtterance*, boolean);
int OEMVR_VoiceActivate(IVR*, const AEEVRUtterance*, AEEVRUtterance);
uint32 OEMVR_GetMaxRecommendedVocab(IVR*);
uint32 OEMVR_GetUtteranceNumBytes(IVR*, AEEVRUtterance);
int OEMVR_UntrainControlWord(IVR*, AEEVRUtterance);
int OEMVR_SetSpeakerIndependent(IVR*, boolean);


/*==============================================================================
   DATA STRUCTURE DOCUMENTATION
================================================================================

Data type:  AEEVRUtterance

Description:
   The value of this data type identifies a voice utterance that can optionally
   be played or recognized.  The upper 16 bits of the value specify the type
   of the utterance, and the lower 16 bits specify the ID.  Currently, the only
   utterance type supported directly by the IVR interface in AEEVR.h is
   AEEVRUTTERANCE_TYPE_VOICETAG.  This file (OEMVR.h) adds two other utterance
   types for internal use by the OEM only.  Prompts are predefined utterances
   that cannot be trained or recognized, but can be played.  Control words are
   predefined utterances that can be trained and recognized.  They may also
   optionally have speaker-independent patterns associated with them, so that
   they can be recognized without first being trained.

Definition:
   typedef uint32 AEEVRUtterance;

Values:
   Special identifier value indicating no utterance:
      AEEVRUTTERANCE_NONE

   Mask value to access the utterance type:
      AEEVRUTTERANCE_TYPE_MASK

   Utterance type values:
      AEEVRUTTERANCE_TYPE_VOICETAG
      AEEVRUTTERANCE_TYPE_PROMPT
      AEEVRUTTERANCE_TYPE_CWORD

   Mask value to access the utterance ID:
      AEEVRUTTERANCE_ID_MASK

   Utterance ID values:
      - For voice tags, these are assigned dynamically when new voice tags are
        trained.
      - For prompts and control words, see the #defines.

Comments:
   This data type is defined in AEEVR.h.

See Also:
   IVR_TrainVoiceTag, IVR_RecognizePrimitive, IVR_RecognizeWithPrompts,
   IVR_PlayUtterance, IVR_VoiceTagEnumNext, IVR_RemoveVoiceTag

================================================================================

Data type:  OEMVRSound

Description:
   This is a structure identifying a sound which can be used as a component of
   a VR prompt.  This consists of an optional utterance followed by an optional
   tone from the ISound interface.

Definition:
   typedef struct
   {
      AEEVRUtterance    wUtt;
      AEESoundToneData *pToneData;
   } OEMVRSound;

Members:
   wUtt          : Utterance, or AEEVRUTTERANCE_NONE if this sound consists
                   of a tone only, not preceded by an utterance.
   pToneData     : Pointer to tone data from ISound interface, or NULL if this
                   sound consists of an utterance only, not followed by a tone.

Comments:
   This data structure is normally used in an array to make up a prompt to be
   set up using OEMVR_SetupPrompt or played directly using OEMVR_PlaySounds.
   Each element of the array identifies one or two components of the prompt,
   and the array is terminated by an element with wUtt set to
   AEEVRUTTERANCE_NONE and pToneData set to NULL.

See Also:
   AEEVRUtterance, AEESoundToneData, OEMVR_SetupPrompt, OEMVR_PlaySounds

================================================================================

Data type:  OEMVRPrompt

Description:
   The value of this data type identifies an individual prompt that is used
   internally in a recognizing or training operation.

Definition:
   typedef uint32 OEMVRPrompt;

Values:
   See the #defines and the DMSS VS ISOD for their meanings.

Comments:
   None

See Also:
   OEMVRSound, OEMVR_SetupPrompt

==============================================================================*/

/*==============================================================================
   INTERFACE DOCUMENTATION
================================================================================

Interface Name: OEMVR

Description:
   This interface is a DMSS OEM-specific extension to IVR.  It provides
   additional functions for voice-recognition services which are not made
   available in the actual Brew IVR interface.

Functions:
   OEMVR_SetPrompt();
   OEMVR_PlaySounds();
   OEMVR_TrainControlWord();
   OEMVR_RecognizeUtteranceMulti();
   OEMVR_VoiceActivate();
   OEMVR_GetMaxRecommendedVocab()
   OEMVR_GetUtteranceNumBytes()

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

#endif /* OEMVR_PRIV_H */
