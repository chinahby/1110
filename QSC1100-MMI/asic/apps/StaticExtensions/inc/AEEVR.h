#ifndef  AEEVR_H
#define  AEEVR_H
/*==============================================================================
FILE:          AEEVR.h

SERVICES:      Voice recognition and related services.

DESCRIPTION:   Definitions for the IVR interface.


        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#include "AEE.h"

#define AEECLSID_VR   0x01011813
/*------------------------------------------------------------------------------
   Data Type Definitions
------------------------------------------------------------------------------*/

// AEEVRUtterance data type
typedef uint32 AEEVRUtterance;

// AEEVRUtterance values
#define AEEVRUTTERANCE_NONE            0
#define AEEVRUTTERANCE_TYPE_MASK       0xFFFF0000ul
#define AEEVRUTTERANCE_TYPE_VOICETAG   0x10000
#define AEEVRUTTERANCE_ID_MASK         0xFFFF

// AEEVRStatus data type
typedef uint32 AEEVRStatus;

// AEEVRStatus values
#define AEEVRSTATUS_BASE               0
#define AEEVRSTATUS_NONE               (AEEVRSTATUS_BASE)
#define AEEVRSTATUS_RUNNING            (AEEVRSTATUS_BASE + 1)
#define AEEVRSTATUS_DONE               (AEEVRSTATUS_BASE + 2)
#define AEEVRSTATUS_FAILED             (AEEVRSTATUS_BASE + 3)
#define AEEVRSTATUS_ABORTED            (AEEVRSTATUS_BASE + 4)
#define AEEVRSTATUS_RESULT_YES         (AEEVRSTATUS_BASE + 5)
#define AEEVRSTATUS_RESULT_NO          (AEEVRSTATUS_BASE + 6)

// AEEVRFailure data type
typedef uint32 AEEVRFailure;

// AEEVRFailure values
#define AEEVRFAILURE_BASE              0
#define AEEVRFAILURE_NONE              (AEEVRFAILURE_BASE)
#define AEEVRFAILURE_GENERAL           (AEEVRFAILURE_BASE + 1)
#define AEEVRFAILURE_ILLEGAL_ARGS      (AEEVRFAILURE_BASE + 2)
#define AEEVRFAILURE_UNMATCHED         (AEEVRFAILURE_BASE + 3)
#define AEEVRFAILURE_DUPLICATE         (AEEVRFAILURE_BASE + 4)
#define AEEVRFAILURE_SILENCE           (AEEVRFAILURE_BASE + 5)
#define AEEVRFAILURE_TOO_SOON          (AEEVRFAILURE_BASE + 6)
#define AEEVRFAILURE_TOO_SHORT         (AEEVRFAILURE_BASE + 7)
#define AEEVRFAILURE_TOO_LONG          (AEEVRFAILURE_BASE + 8)
#define AEEVRFAILURE_FILE_ERROR        (AEEVRFAILURE_BASE + 9)

// PFNVRSTATUS data type (pointer to callback function)
typedef void (*PFNVRSTATUS)(void*, AEEVRStatus, AEEVRFailure, AEEVRUtterance);

// AEEVRProperties data type
typedef struct
{
   uint16  wStructSize;
   uint16  wLanguageChars;
   uint16  wLabelBytes;
} AEEVRProperties;


/*------------------------------------------------------------------------------
   Interface Definitions
------------------------------------------------------------------------------*/

typedef struct IVR IVR;

AEEINTERFACE(IVR)
{
   INHERIT_IQueryInterface(IVR);

   int         (*TrainVoiceTag)           (IVR*, const AEEVRUtterance*);
   int         (*RecognizePrimitive)      (IVR*, const AEEVRUtterance*);
   int         (*RecognizeWithPrompts)    (IVR*, const AEEVRUtterance*, boolean, uint8);
   int         (*YesNoConfirm)            (IVR*);
   int         (*VoiceMenuSelect)         (IVR*, const AEEVRUtterance*);
   int         (*PlayUtterance)           (IVR*, AEEVRUtterance);
   int         (*AbortOperation)          (IVR*);
   int         (*RegisterNotify)          (IVR*, PFNVRSTATUS, void*);
   AEEVRStatus (*GetOperationStatus)      (IVR*, AEEVRFailure*, AEEVRUtterance*);
   int         (*LanguageEnumInit)        (IVR*);
   boolean     (*LanguageEnumNext)        (IVR*, AECHAR*, uint32);
   int         (*GetLanguage)             (IVR*, AECHAR*, uint32);
   int         (*SetLanguage)             (IVR*, const AECHAR*);
   int         (*VoiceTagEnumInit)        (IVR*);
   boolean     (*VoiceTagEnumNext)        (IVR*, AEEVRUtterance*);
   int         (*RemoveVoiceTag)          (IVR*, AEEVRUtterance);
   int         (*GetUtteranceLabel)       (IVR*, AEEVRUtterance, uint8*, uint32*);
   int         (*SetUtteranceLabel)       (IVR*, AEEVRUtterance, const uint8*, uint32);
   uint32      (*GetUtteranceDuration)    (IVR*, AEEVRUtterance);
   int         (*GetProperties)           (IVR*, AEEVRProperties*);
   int         (*SetVoiceTagDB)           (IVR*, const char*);
};

/*------------------------------------------------------------------------------
   Interface Access Macros
------------------------------------------------------------------------------*/

#ifdef FEATURE_PHONE_VR
#define IVR_AddRef(p)                        AEEGETPVTBL(p,IVR)->AddRef(p)
#define IVR_Release(p)                       AEEGETPVTBL(p,IVR)->Release(p)
#define IVR_QueryInterface(p,q,r)            AEEGETPVTBL(p,IVR)->QueryInterface(p,q,r)
#define IVR_TrainVoiceTag(p,q)               AEEGETPVTBL(p,IVR)->TrainVoiceTag(p,q)
#define IVR_RecognizePrimitive(p,q)          AEEGETPVTBL(p,IVR)->RecognizePrimitive(p,q)
#define IVR_RecognizeWithPrompts(p,q,r,s)    AEEGETPVTBL(p,IVR)->RecognizeWithPrompts(p,q,r,s)
#define IVR_YesNoConfirm(p)                  AEEGETPVTBL(p,IVR)->YesNoConfirm(p)
#define IVR_VoiceMenuSelect(p,q)             AEEGETPVTBL(p,IVR)->VoiceMenuSelect(p,q)
#define IVR_PlayUtterance(p,q)               AEEGETPVTBL(p,IVR)->PlayUtterance(p,q)
#define IVR_AbortOperation(p)                AEEGETPVTBL(p,IVR)->AbortOperation(p)
#define IVR_RegisterNotify(p,q,r)            AEEGETPVTBL(p,IVR)->RegisterNotify(p,q,r)
#define IVR_GetOperationStatus(p,q,r)        AEEGETPVTBL(p,IVR)->GetOperationStatus(p,q,r)
#define IVR_LanguageEnumInit(p)              AEEGETPVTBL(p,IVR)->LanguageEnumInit(p)
#define IVR_LanguageEnumNext(p,q,r)          AEEGETPVTBL(p,IVR)->LanguageEnumNext(p,q,r)
#define IVR_GetLanguage(p,q,r)               AEEGETPVTBL(p,IVR)->GetLanguage(p,q,r)
#define IVR_SetLanguage(p,q)                 AEEGETPVTBL(p,IVR)->SetLanguage(p,q)
#define IVR_VoiceTagEnumInit(p)              AEEGETPVTBL(p,IVR)->VoiceTagEnumInit(p)
#define IVR_VoiceTagEnumNext(p,q)            AEEGETPVTBL(p,IVR)->VoiceTagEnumNext(p,q)
#define IVR_RemoveVoiceTag(p,q)              AEEGETPVTBL(p,IVR)->RemoveVoiceTag(p,q)
#define IVR_GetUtteranceLabel(p,q,r,s)       AEEGETPVTBL(p,IVR)->GetUtteranceLabel(p,q,r,s)
#define IVR_SetUtteranceLabel(p,q,r,s)       AEEGETPVTBL(p,IVR)->SetUtteranceLabel(p,q,r,s)
#define IVR_GetUtteranceDuration(p,q)        AEEGETPVTBL(p,IVR)->GetUtteranceDuration(p,q)
#define IVR_GetProperties(p,q)               AEEGETPVTBL(p,IVR)->GetProperties(p,q)
#define IVR_SetVoiceTagDB(p,q)               AEEGETPVTBL(p,IVR)->SetVoiceTagDB(p,q)
#endif

/*==============================================================================
   DATA TYPE DOCUMENTATION
================================================================================

Data type:  AEEVRUtterance

Description:
   The value of this data type identifies a voice utterance that can optionally
   be played or recognized.  The upper 16 bits of the value specify the type
   of the utterance, and the lower 16 bits specify the ID.  Currently, the only
   utterance type supported is AEEVRUTTERANCE_TYPE_VOICETAG.  Voice tags are
   utterances which have no predefined meaning, and must be individually
   trained prior to use.  Other types of utterances are reserved for possible
   future expansion.

Definition:
   typedef uint32 AEEVRUtterance;

Values:
   Special identifier value indicating no utterance:
      AEEVRUTTERANCE_NONE

   Mask value to access the utterance type:
      AEEVRUTTERANCE_TYPE_MASK

   Utterance type values:
      AEEVRUTTERANCE_TYPE_VOICETAG

   Mask value to access the utterance ID:
      AEEVRUTTERANCE_ID_MASK

   Utterance ID values:
      These are assigned dynamically when new voice tags are trained.

Comments:
   None

See Also:
   IVR_TrainVoiceTag, IVR_RecognizePrimitive, IVR_RecognizeWithPrompts,
   IVR_PlayUtterance, IVR_VoiceTagEnumNext, IVR_RemoveVoiceTag

================================================================================

Data type:  AEEVRStatus

Description:
   The value of this data type identifies the status of the current or last
   asynchronous VR operation.  (Training, recognizing, and playing are
   considered asynchronous operations, because the associated function calls
   just start the operation and return without waiting for the operation to
   complete.)

Definition:
   typedef uint32 AEEVRStatus;

Values:
   AEEVRSTATUS_NONE           Invalid status or no VR operation started.
   AEEVRSTATUS_RUNNING        VR operation in progress.
   AEEVRSTATUS_DONE           VR operation completed successfully.
   AEEVRSTATUS_FAILED         VR operation failed.
   AEEVRSTATUS_ABORTED        VR operation aborted by IVR_AbortOperation.
   AEEVRSTATUS_RESULT_YES     VR operation done, "Yes" result detected.
   AEEVRSTATUS_RESULT_NO      VR operation done, "No" result detected.

Comments:
   None

See Also:
   IVR_GetOperationStatus, IVR_RegisterNotify

================================================================================

Data type:  AEEVRFailure

Description:
   The value of this data type identifies a failure that occurred during the
   current or last asynchronous VR operation.  A failure is reported when the
   status of the operation is AEEVRSTATUS_FAILED, in which case the failure is
   a final error that could not be handled internally by the operation and must
   instead be handled by the application.  Failures may also be intermediate in
   nature (i.e. reported when the current status is AEEVRSTATUS_RUNNING), in
   which case the failure is being handled internally by the operation, and is
   only being reported to the application for informational purposes.
   

Definition:
   typedef uint32 AEEVRFailure;

Values:
   AEEVRFAILURE_NONE                 No failure reported by operation.
   AEEVRFAILURE_GENERAL              Unspecified general failure.
   AEEVRFAILURE_ILLEGAL_ARGS         Illegal arguments passed to function.
   AEEVRFAILURE_UNMATCHED            Speech did not match any given utterance.
   AEEVRFAILURE_DUPLICATE            Speech was duplicate of another utterance.
   AEEVRFAILURE_SILENCE              No speech detected.
   AEEVRFAILURE_TOO_SOON             Spoke too soon, could not capture speech.
   AEEVRFAILURE_TOO_SHORT            Speech was too short to recognize.
   AEEVRFAILURE_TOO_LONG             Speech was too long to recognize.
   AEEVRFAILURE_FILE_ERROR           File error (e.g. out of space).

Comments:
   Platforms are not required to support all of the failure codes, in any or
   all circumstances.  For example, a platform may or may not report
   intermediate failures like AEEVRFAILURE_TOO_SHORT or AEEVRFAILURE_UNMATCHED
   while some VR operation is in progress.  Also, a platform may use the
   AEEVRFAILURE_GENERAL failure code for any or all failures instead of
   reporting the individual failure cases.

See Also:
   AEEVRStatus, IVR_GetOperationStatus, IVR_RegisterNotify

================================================================================

Data type:  PFNVRSTATUS

Description:
   This data type points to a callback function which is invoked when there is
   a change in the status of the current asynchronous operation to report to the
   application.  (Training, recognizing, and playing are considered asynchronous
   operations, because the associated function calls just start the operation
   and return without waiting for the operation to complete.)

Definition:
   typedef void (*PFNVRSTATUS)(void* pUser, AEEVRStatus wStatus,
                               AEEVRFailure wFailure, AEEVRUtterance wUtt);

Function Parameters:
   pUser       [in]: User data specified when callback is registered.
   wStatus     [in]: New status of asynchronous operation.
   wFailure    [in]: Failure code, if applicable.
   wUtt        [in]: Utterance, if applicable.

Comments:
   The user callback is invoked only after being registered via
   IVR_RegisterNotify.  The status can also be obtained by calling
   IVR_GetOperationStatus.

See Also:
   AEEVRStatus, AEEVRFailure, AEEVRUtterance, IVR_RegisterNotify

================================================================================

Data type:  AEEVRProperties

Description:
   This is a structure containing various properties of the VR system.

Definition:
   typedef struct
   {
      uint16  wStructSize;
      uint16  wLanguageChars;
      uint16  wLabelBytes;
   } AEEVRProperties;

Members:
   wStructSize    : Size of this structure (in bytes).
   wLanguageChars : Maximum number of characters (including NULL) that can
                    be used to identify the language setting of the VR system.
   wLabelBytes    : Maximum number of bytes that can be used as an utterance
                    label.

Comments:
   The properties values are implementation-dependent.  An application that
   needs to know these values can obtain them with IVR_GetProperties.

See Also:
   IVR_GetProperties, IVR_GetLanguage, IVR_GetUtteranceLabel

==============================================================================*/

/*==============================================================================
   INTERFACE DOCUMENTATION
================================================================================

Interface Name: IVR

Description:
   This interface provides functions for performing non-continuous voice
   recognition and related operations.  A separate database of recognizable
   words or phrases, known as voice tags, will be associated with each
   application using this interface.  These voice tags must be individually
   trained and added to the database before they can be recognized.  The
   recognition of voice tags is thus speaker-dependent.

   Voice tags are currently the only type of utterance that can be explicitly
   recognized.  However, the VR system can also recognize certain common words
   such as "yes" and "no" internally.  No interface is provided to recognize
   such built-in words explicitly, but the platform implementation may choose
   to recognize them as part of the high-level interface operations.  This
   recognition may be performed in either a speaker-independent or speaker-
   dependent manner, depending on the platform.

   In this interface, training, recognizing, and playing are considered
   asynchronous operations, because the associated function calls just start
   the operation and return without waiting for the operation to complete.
   There can only be one of these asynchronous VR operations started at any
   given time.  The application can either poll the status of the asynchronous
   operation to find out when it is complete, or it can register to be notified
   of status changes via a callback function.

Functions:
   IVR_AddRef()
   IVR_Release()
   IVR_QueryInterface()
   IVR_TrainVoiceTag()
   IVR_RecognizePrimitive()
   IVR_RecognizeWithPrompts()
   IVR_YesNoConfirm()
   IVR_VoiceMenuSelect()
   IVR_PlayUtterance()
   IVR_AbortOperation()
   IVR_RegisterNotify()
   IVR_GetOperationStatus()
   IVR_LanguageEnumInit()
   IVR_LanguageEnumNext()
   IVR_GetLanguage()
   IVR_SetLanguage()
   IVR_VoiceTagEnumInit()
   IVR_VoiceTagEnumNext()
   IVR_RemoveVoiceTag()
   IVR_GetUtteranceLabel()
   IVR_SetUtteranceLabel()
   IVR_GetUtteranceDuration()
   IVR_GetProperties()
   IVR_SetVoiceTagDB()

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

================================================================================

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

#endif /* AEEVR_H */
