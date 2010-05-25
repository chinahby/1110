/*
  ============================================================================

FILE:  OEMMedia.h

SERVICES:
       Reference IMedia base class interface.

GENERAL DESCRIPTION:

REVISION HISTORY:

  ============================================================================
  ============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef OEMMEDIA_H
#define OEMMEDIA_H

#include "AEEMedia.h"

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#endif // FEATURE_ACM

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

#ifdef MM_RELEASEIF
#undef MM_RELEASEIF
#endif
#define MM_RELEASEIF(p)          if (p) { IBASE_Release((IBase*)(p)); (p) = NULL; }

//********************************************************************************************************************************
//
//          AEE_OEM Interface
//
//********************************************************************************************************************************
/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      AEE-OEM Functions
-------------------------------------------------------------------*/
// Detects IMedia-based class
extern int OEMMedia_DetectType(const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME);

#if defined(FEATURE_BREW_DETECT_MP3)
extern int OEMMedia_DetectTypeMP3Ex(const void * cpBuf, uint32 * pdwSize);
#endif
//********************************************************************************************************************************
//
//          OEMMedia
//
//********************************************************************************************************************************

/*-------------------------------------------------------------------
      Macros Declarations
-------------------------------------------------------------------*/
#define AEEMedia_IsIdleState(p)        ((p)->m_nState == MM_STATE_IDLE)
#define AEEMedia_IsReadyState(p)       ((p)->m_nState == MM_STATE_READY)
#define AEEMedia_IsPlayState(p)        ((p)->m_nState == MM_STATE_PLAY)
#define AEEMedia_IsPlayPauseState(p)   ((p)->m_nState == MM_STATE_PLAY_PAUSE)
#define AEEMedia_IsRecState(p)         ((p)->m_nState == MM_STATE_RECORD)
#define AEEMedia_IsRecPauseState(p)    ((p)->m_nState == MM_STATE_RECORD_PAUSE)
#define AEEMedia_IsStateChanging(p)    ((p)->m_bStateChanging)

#define AEEMedia_IsPlayOn(p)           (AEEMedia_IsPlayState(p) || AEEMedia_IsPlayPauseState(p))
#define AEEMedia_IsRecOn(p)            (AEEMedia_IsRecState(p) || AEEMedia_IsRecPauseState(p))
#define AEEMedia_IsPlayRecOn(p)        (AEEMedia_IsPlayOn(p) || AEEMedia_IsRecOn(p))
#define AEEMedia_IsSeekFrame(s)        (((s) & 0xF0) == MM_SEEK_MODE_FRAME)

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct _AEEMediaCallback
{
   int                  bInUse;     // Callback/Command availability
   AEECallback          cb;         // Pre-loaded and does not change
   AEEMediaCmdNotify    cmdNotify;  // Callback cmd info
} AEEMediaCallback;

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI) 
#define INHERIT_AEEMedia(iname) \
   DECLARE_VTBL(iname) \
   AEECallback          m_cbSysObj; \
   void *               m_pac; \
   \
   IShell *             m_pIShell; \
   int32                m_nRefs; \
   AEECLSID             m_clsSelf; \
   \
   int                  m_nState; \
   boolean              m_bStateChanging; \
   AEEMediaData         m_md; \
   uint32               m_dwCapsMD; \
   flg                  m_bRaw:1; \
   void *               m_pSpec; \
   uint32               m_dwSpecSize; \
   uint32               m_dwBufferSize; \
   \
   PFNMEDIANOTIFY       m_pfnNotify; \
   void *               m_pUser; \
   \
   char *               m_pszFile; \
   char *               m_pszFileAppPath; \
   AEEMediaData         m_mdInternal; \
   flg                  m_bChannelShare; \
   \
   IAppResCtl *         m_pResCtl; \
   ACM_Callback_data_type m_mAcmCB; \
   flg                  m_bResOwner:1; \
   flg                  m_bForceRelease:1
#else // !FEATURE_ODM_UI
#define INHERIT_AEEMedia(iname) \
   DECLARE_VTBL(iname) \
   AEECallback          m_cbSysObj; \
   void *               m_pac; \
   \
   IShell *             m_pIShell; \
   int32                m_nRefs; \
   AEECLSID             m_clsSelf; \
   \
   int                  m_nState; \
   boolean              m_bStateChanging; \
   AEEMediaData         m_md; \
   uint32               m_dwCapsMD; \
   flg                  m_bRaw:1; \
   void *               m_pSpec; \
   uint32               m_dwSpecSize; \
   uint32               m_dwBufferSize; \
   \
   PFNMEDIANOTIFY       m_pfnNotify; \
   void *               m_pUser; \
   \
   char *               m_pszFile; \
   char *               m_pszFileAppPath; \
   AEEMediaData         m_mdInternal; \
   flg                  m_bChannelShare; \
   \
   ACM_Callback_data_type m_mAcmCB; \
   flg                  m_bResOwner:1; \
   flg                  m_bForceRelease:1
#endif // FEATURE_ODM_UI
#else // !FEATURE_ACM
#define INHERIT_AEEMedia(iname) \
   DECLARE_VTBL(iname) \
   AEECallback          m_cbSysObj; \
   void *               m_pac; \
   \
   IShell *             m_pIShell; \
   int32                m_nRefs; \
   AEECLSID             m_clsSelf; \
   \
   int                  m_nState; \
   boolean              m_bStateChanging; \
   AEEMediaData         m_md; \
   uint32               m_dwCapsMD; \
   flg                  m_bRaw:1; \
   void *               m_pSpec; \
   uint32               m_dwSpecSize; \
   uint32               m_dwBufferSize; \
   \
   PFNMEDIANOTIFY       m_pfnNotify; \
   void *               m_pUser; \
   \
   char *               m_pszFile; \
   char *               m_pszFileAppPath; \
   AEEMediaData         m_mdInternal; \
   flg                  m_bChannelShare
#endif // FEATURE_ACM

typedef struct _AEEMedia
{
   INHERIT_AEEMedia(IMedia);
} AEEMedia;

/*---------------------------------------------------------------------------
      IMedia Utility Functions
-----------------------------------------------------------------------------*/
// Registers the handler
extern void AEEMedia_Init(IShell * ps, char * szMIME, AEECLSID clsHandler);

// Constructor and destructor
extern int  AEEMedia_New(IMedia * po, IShell * ps, AEECLSID cls);
extern void AEEMedia_Delete(IMedia * po);

/*---------------------------------------------------------------------------
   Callback Notification Function: called in BREW context in response to
   AEE_ResumeCallback(). This can be called from the derived class
-----------------------------------------------------------------------------*/
extern void AEEMedia_CallbackNotify(AEEMedia * pme, AEEMediaCallback * pmcb);

/*---------------------------------------------------------------------------
      Common IMedia API. Derived class can use/extend/replace these functions
-----------------------------------------------------------------------------*/
// These functions can be directly used by dervied class
extern uint32     AEEMedia_AddRef(IMedia * po);
extern uint32     AEEMedia_Release(IMedia * po);
extern int        AEEMedia_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo);
extern int        AEEMedia_RegisterNotify(IMedia * po, PFNMEDIANOTIFY pfnNotify, void * pUser);
extern int        AEEMedia_GetState(IMedia * po, boolean * pbStateChanging);

// These functions perform state management and/or command execution checks.
// The overridden method in derived class can call the corresponding function first.
extern int        AEEMedia_SetMediaParm(IMedia * po, int nParamID, int32 p1, int32 p2);
extern int        AEEMedia_GetMediaParm(IMedia * po, int nParamID, int32 * pP1, int32 * pP2);
extern int        AEEMedia_Play(IMedia * po);
extern int        AEEMedia_Record(IMedia * po);
extern int        AEEMedia_Stop(IMedia * po);
extern int        AEEMedia_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
extern int        AEEMedia_Pause(IMedia * po);
extern int        AEEMedia_Resume(IMedia * po);
extern int        AEEMedia_GetTotalTime(IMedia * po);


// This function is not implemented in AEEMedia -- derived class needs to implement it
//extern boolean    AEEMedia_IsSupported(IMedia * po, byte * pData, uint32 dwSize, boolean bMIME);

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
=============================================================================

AEEMediaCallback

Description:
   This structure defines IMedia-specific callback info structure wrapped
   around AEECallback and IMedia callback info structures.

Definition:
   typedef struct _AEEMediaCallback
   {
      int                  bInUse;
      AEECallback          cb;
      AEEMediaCmdNotify    cmdNotify;
   } AEEMediaCallback;

Members:
   bInUse:     Callback/Command availability; = TRUE, means this structure is in use.
   cb:         Pre-loaded and does not change
   cmdNotify:  IMedia callback-specific info

Comments:
   This structure is typically used by all the derived classes.

See Also:
   AEEMedia_CallbackNotify()

=============================================================================

AEEMedia

Description:
   This structure defines the data members common across all IMedia-based
   classes.

Definition:
   typedef struct _AEEMedia
   {
      INHERIT_AEEMedia(IMedia);
   } AEEMedia;

Members:
   INHERIT_AEEMedia(IMedia):  A macro that declares the following members.
   DECLARE_VTBL(iname):       Declares the virtual table of the class
   m_pIShell:                 Pointer to IShell
   m_nRefs:                   Reference count
   m_clsSelf:                 Class ID of the IMedia
   m_nState:                  State of the media
   m_bStateChanging:          = TRUE, means IMedia is in state transition
   m_md:                      Media data source/sink
   m_pfnNotify:               User registered callback function
   m_pUser:                   User data passed when m_pfnNotify() is called
   m_pszFile:                 Full path of the file name
   m_mdInternal:              Copy of media data to support const file playback

FEATURE_ACM members:
   m_pResCtl:                 Pointer to AppResCtl (Used only in Sirius target)
   m_mAcmCB                   ACM Callback data for force release/delayed acquire
   m_bResOwner:               = TRUE if Resource Acquired from ACM state manager
   m_bForceRelease:           = TRUE if Media has been force released

Comments:
   This structure is inherited by all the derived classes. Derived classes
   can directly use the AEEMedia functions defined in this file if they
   inherit from structure and also can use AEEMedia functions in their
   vtbls.

See Also:

=============================================================================

=============================================================================
  INTERFACE DOCUMENTATION
=============================================================================
Interface Name: AEEMedia

Description:
   AEEMedia is the base class of all IMedia-based classes.

   AEEMedia
   (1) Registers the MIME type of the class
   (2) Makes sure if a command can be executed in a particular state
   (2) Manages the IMedia state machine
   (3) Handles IMedia callbacks and makes callbacks into app
   (4) Stops IMedia when IMedia is released
=============================================================================

Function: AEEMedia_Init()

Description:
   This function registers the MIME type in the Shell registry.

Prototype:
   void AEEMedia_Init(IShell * ps, char * szMIME, AEECLSID clsHandler);

Parameters:
   ps: Pointer to IShell
   szMIME: MIME string
   clsHandler: Class ID of the IMedia-based class

Return Value:
   None

Comments:
   This function is called only once during BREW initialization.

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_New()

Description:
   This function is the base class constructor.

Prototype:
   int AEEMedia_New(IMedia * po, IShell * ps, AEECLSID cls);

Parameters:
   po: Pointer to IMedia
   ps: Pointer to IShell
   cls: Class ID of the IMedia-based class

Return Value:
   SUCCESS: Successful

Comments:
   None

Side Effects:
   None

See Also:
   AEEMedia_Delete(),
   AEEMedia_Init()

=============================================================================

Function: AEEMedia_Delete()

Description:
   This function is the base class destructor.

Prototype:
   void AEEMedia_Delete(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=============================================================================

Function: AEEMedia_CallbackNotify()

Description:
   This function is registered with CMediaMMLayer for callbacks from the
   multimedia layer. It performs state management and calls the user-registered
   callback function.

Prototype:
   void AEEMedia_CallbackNotify(AEEMedia * pme, AEEMediaCallback * pmcb);

Parameters:
   pme: Pointer to AEEMedia
   pmcb: Pointer to media callback structure

Return Value:
   None

Comments:
   This function is called in BREW context.

Side Effects:
   None

See Also:
   AEEMedia_Play(),
   AEEMedia_Record(),
   AEEMedia_Stop()
=============================================================================

Function: AEEMedia_AddRef()

Description:
   Increments the refrence count of IMedia object.

Prototype:
   uint32 AEEMedia_AddRef(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   Incremented ref count

Comments:
   None

Side Effects:
   None

See Also:
   AEEMedia_Release()
=============================================================================

Function: AEEMedia_Release()

Description:
   Decrements the reference count of IMedia object. If reference count goes
   down to zero, then it deregisters the user registered callback and
   stops the IMedia playback/recording, if any.

Prototype:
   uint32 AEEMedia_Release(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   Updated reference count. Zero if object is released.

Comments:
   None

Side Effects:
   None

See Also:
   AEEMedia_AddRef()

=============================================================================

Function: AEEMedia_QueryInterface()

Description:
   This function can be used to
   (1) get a pointer to an interface or data based on the input class ID
   (2) query an extended version of the IMedia-derived class
   (3) support version compatibility

Prototype:
   int AEEMEDIA_QueryInterface(IMedia * po, AEECLSID clsReq, void ** ppo);

Parameters:
   po [in]: Pointer to IMedia interface.

   clsReq [in]: A globally unique id to identify the entity (interface or data) that we
                are trying to query.

   ppo[out]: Pointer to the interface or data that we want to retrieve.  If the value
             passed back is NULL, the interface or data that we query are not available.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments:
   If ppo is back a NULL pointer, the interface or data that we query is not
   available.

Side Effects:
   If an interface is retrieved, then this function increments its reference count.
   If a data structure is retrieved, then a pointer to the internal structure is
   given and user should not free it.

See Also:
   None
=============================================================================

Function: AEEMedia_RegisterNotify()

Description:
   This function registers a callback notification function with IMedia object.
   IMedia reports asynchronous events thru this callback.

Prototype:
   int AEEMEDIA_RegisterNotify(IMedia * po, PFNMEDIANOTIFY pfnNotify, void * pUser);

Parameters:
   po: Pointer to the IMedia Interface object
   pfnNotify: User callback function pointer
   pUser: User data to be used when calling pfnNotify()

Return Value:
   SUCCESS:    Successful.
   EBADSTATE:  Error - IMedia is not in Ready state.

Comments:

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_GetState()

Description:
   This function returns the current state of IMedia and also indicates if
   the IMedia object is currently in state transition.

Prototype:
   int AEEMEDIA_GetState(IMedia * po, boolean * pbStateChanging);

Parameters:
   po : Pointer to the IMedia Interface object
   pbStateChanging: TRUE means IMedia is currently busy transitioning the state.

Return Value:
   MM_STATE_XXX

Comments:

Side Effects:
   If IMedia is currently is in state transition, then most of IMedia APIs fail
   and return EBADSTATE.

See Also:
   None
=============================================================================

Function: AEEMedia_SetMediaParm()

Description:
   This function handles MM_PARM_MEDIA_DATA in the Idle state.

Prototype:
   int AEEMedia_SetMediaParm(IMedia * po, int nParamID, int32 p1, int32 p2);

Parameters:
   po :     Pointer to the IMedia Interface object
   nParmID: MM_PARM_XXX
   p1:      Depends on parm
   p2:      Depends on parm

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot set parm in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_GetMediaParm()

Description:
   This function handles MM_PARM_MEDIA_DATA and MM_PARM_CLSID.

Prototype:
   int AEEMedia_GetMediaParm(IMedia * po, int nParamID, int32 * pP1, int32 * pP2);

Parameters:
   po :     Pointer to the IMedia Interface object
   nParmID: MM_PARM_XXX
   pP1:     Depends on parm
   pP2:     Depends on parm

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot get parm in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_Play()

Description:
   This function checks if playback command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_Play(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_Record()

Description:
   This function checks if record command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_Record(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state
   MM_EBADMEDIADATA: Bad media data. Possibly ISource is used for recording.

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_Stop()

Description:
   This function checks if stop command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_Stop(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_Seek()

Description:
    This function checks if seek command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);

Parameters:
   po: Pointer to IMedia
   eSeek:
   lTimeMS:

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_Pause()

Description:
   This function checks if pause command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_Pause(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=============================================================================

Function: AEEMedia_Resume()

Description:
   This function checks if resume command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_Resume(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None

=============================================================================

Function: AEEMedia_GetTotalTime()

Description:
   This function checks if get total time command is valid in the current IMedia
   state.

Prototype:
   int AEEMedia_GetTotalTime(IMedia * po);

Parameters:
   po: Pointer to IMedia

Return Value:
   SUCCESS:    Successful
   EBADSTATE:  Cannot issue command in the current state

Comments:
   None

Side Effects:
   None

See Also:
   None
=================================================================================
  INTERFACE DOCUMENTATION
=============================================================================
Interface Name: OEMMedia
=============================================================================
Function: OEMMedia_DetectType()

Description:
   Given data in a buffer or the name of an object, this function detects
   the MIME type of the media.

   This function is typically used to get the handler associated with the data type.
   For example, if the data represents standard MIDI format, then this
   function returns the MIME "audio/mid". Using the MIME type, you can query
   Shell registry to obtain the handler (Class ID) of type AEECLSID_MEDIA.

Prototype:

   int OEMMedia_DetectType(const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME);

Parameters:
   cpBuf [in]:        Buffer containing the data whose type needs to be determined
   pdwSize [in/out]: On input: Size of data in pBuf, unless pBuf is NULL, then ignored
                     On output: number of additional data bytes needed to perform type detection
   cpszName [in]:    Name of the object whose type needs to be determined (may be null, if unknown).
   pcpszMIME [out]:  MIME string returned to caller, on return, filled with a pointer to a constant string (do not free)

Return Value:
   SUCCESS:    Data type is detected and MIME is returned
   ENOTYPE:    There is no type associated with this data
   EBADPARM:   Wrong input data (parameter(s))
   ENEEDMORE:  Need more data to perform type detection. *pdwSize contains the the required
               number of additional bytes.
   EUNSUPPORTED: Type detection for the specified input is not supported

Comments:
   pBuf takes precedence over pszName. If both of them are specified, then first pBuf is used for type detection
   followed by pszName.

   If the function returns ENEEDMORE, then *pdwSize is filled with the required additional bytes to carry out the
   operation. Call this function again with (original dwSize + *pdwSize) bytes.

   To determine the maximum number of bytes required to enable type detection, you can call
===pre>
      if (ENEEDMORE == ISHELL_DetectType(ps, NULL, &dwReqSize, NULL, NULL))
      {
         // dwReqSize contains the max bytes needed for type detection
      }
===/pre>
Side Effects:
   None

See Also:
   OEMRegistry_DetectType()
   ISHELL_DetectType()
   ISHELL_GetHandler()
   ISHELL_CreateInstance()

=============================================================================
Function: OEMMedia_DetectTypeMP3Ex()

Description:
   Given data in a buffer, This function searches through the file buffer to 
   detect if the file type is MP3. Used in case where header is further down the buffer.
   This is special case for MP3 files.
   
 
Prototype:
   int OEMMedia_DetectTypeMP3Ex(const void * cpBuf, uint32 * pdwSize);

Parameters:
   cpBuf [in]:        Buffer containing the data whose type needs to be determined
   pdwSize [in/out]: On input: Size of data in pBuf, unless pBuf is NULL, then ignored
                     On output: number of additional data bytes needed to perform type detection
   
Return Value:
   SUCCESS:    Data type is detected.
   EFAILED:    Failed to detect data type.
   
Side Effects:
   None
   
=============================================================================
AEEIMedia

See the IMedia Interface in the BREW API Reference.


=============================================================================*/

#endif /* #ifndef OEMMEDIA_H */
