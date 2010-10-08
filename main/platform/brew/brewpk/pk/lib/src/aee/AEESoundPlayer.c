/*======================================================

                   ** IMPORTANT ** 

This source is provide strictly for reference and debug
purposes to licensed parties.  Modification of this
code will likely cause instability and/or incompatibility with
future software versions and failure of applications
that leverage these interfaces.  

FILE:  AEESoundPlayer.c

SERVICES:  AEE Sound Player Interfaces

GENERAL DESCRIPTION:  Provides support for ISoundPlayer interface in BREW. These include:
 - Playing MIDI, MP3 files
 - Stop, Pause, Rewind, etc operations while playing these files

PUBLIC CLASSES AND STATIC FUNCTIONS: ISoundPlayer

Copyright © 1999-2005 QUALCOMM Incorporated.
         All Rights Reserved.
       QUALCOMM Proprietary/GTDR

=====================================================*/

#include "OEMHeap.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEMedia.h"
#include "AEEMediaUtil.h"
#include "AEEGraphics_priv.h"

//
// Multi-Media Module Definitions...
//



/*===============================================================================
                     FUNCTION DEFINITIONS
===============================================================================*/

/*===========================================================================

ISoundPlayer implementation begins here...

===========================================================================*/
//
// Sound Player Class Definitions...
//

#define AEESOUNDPLAYER_MIN_READ  500
#define AEESP_SET_BUFFER_SIZE    16 // For SDT_BUFFER in Set() API.

/*===========================================================================

Class Definitions

===========================================================================*/

typedef struct _AEESoundPlayer   AEESoundPlayer;
typedef struct _AEESPCallback    AEESPCallback;

// Indicates which API is used for setting data.
typedef enum
{
   SPS_NONE,
   SPS_SET,
   SPS_SETSTREAM,
   SPS_SETINFO
} SPSetType;

struct _AEESoundPlayer
{
   // Declare the VTBL

   DECLARE_VTBL(ISoundPlayer)

   IShell *             m_pShell;
   int32                m_nRefs;
   int32                m_nIntRefs;

   ISound *             m_pISound;
   IMedia *             m_pMedia;
   IMedia *             m_pMediaPending;

   // SoundPlayer audio source
   AEESoundPlayerInfo   m_spInfo;
   SPSetType            m_eSet;

   // These two vaiables are used for AEESoundPlayer_Set() API.
   // Provide for backward compatibility only.
   char *               m_pszFile;
   void *               m_pRawData;    // May include MIME type

   void *               m_pUser;
   PFNSOUNDPLAYERSTATUS m_pfnUser;
   void *               m_pAppContext;

   boolean              m_bPlaying;

   IAStream *           m_pStream;
   uint32               m_dwBytes;
   uint32               m_dwRead;
   flg                  m_bStreaming:1;

   // NOTE: The following two variables are used to provide backward compatibility
   // to BREW 1.0.1, where users can call following ISoundPlayer API in sequence:
   // ISOUNDPLAYER_Play(), ISOUNDPLAYER_Set(), ISOUNDPLAYER_Play().
   // This causes, the first play to stop and second play to start with the new data set. 
   // Note that ISOUNDPLAYER_Set() has been deprecated -- use ISOUNDPLAYER_SetInfo()
   // instead.
   flg                  m_bDataSet:1;
   flg                  m_bPlayIssued:1;
   flg                  m_bRewind:1;
   flg                  m_bGetTotalTime:1;
};

static uint32  AEESoundPlayer_AddRef(ISoundPlayer * po);
static uint32  AEESoundPlayer_Release(ISoundPlayer * po);
static void    AEESoundPlayer_RegisterNotify(ISoundPlayer * po,PFNSOUNDPLAYERSTATUS pfn, void * pUser);
static void    AEESoundPlayer_Set(ISoundPlayer * po, AEESoundPlayerInput t, void * pData);
static void    AEESoundPlayer_Play(ISoundPlayer * po);
static void    AEESoundPlayer_Stop(ISoundPlayer * po);
static void    AEESoundPlayer_Rewind(ISoundPlayer * po,uint32 dwTime);
static void    AEESoundPlayer_FastForward(ISoundPlayer * po,uint32 dwTime);
static void    AEESoundPlayer_Pause(ISoundPlayer * po);
static void    AEESoundPlayer_Resume(ISoundPlayer * po);
static void    AEESoundPlayer_SetTempo(ISoundPlayer * po,uint32 dwTempoFactor);
static void    AEESoundPlayer_SetTune(ISoundPlayer * po,int8 nStep);
static void    AEESoundPlayer_SetVolume(ISoundPlayer * po,uint16 wVolume);
static void    AEESoundPlayer_GetVolume(ISoundPlayer * po);
static void    AEESoundPlayer_GetTotalTime(ISoundPlayer * po);
static void    AEESoundPlayer_SetSoundDevice(ISoundPlayer * po,AEESoundDevice eDevice, AEESoundMuteCtl eEarMute, AEESoundMuteCtl eMicMute);
static void    AEESoundPlayer_SetStream(ISoundPlayer * po, IAStream * ps);
static int     AEESoundPlayer_SetInfo(ISoundPlayer * po, AEESoundPlayerInfo * pInfo);
static int     AEESoundPlayer_GetInfo(ISoundPlayer * po, AEESoundPlayerInfo * pInfo);

// Helper functions:
static void    AEESoundPlayer_FreeData(AEESoundPlayer * pme);

static int     AEESoundPlayer_IntSetInfo(AEESoundPlayer * po, AEESoundPlayerInfo * pInfo);

static void    AEESoundPlayer_NotifyCmdSuccess(AEESoundPlayer * pme, AEESoundPlayerCmd cbType);
static void    AEESoundPlayer_NotifyCmdFailure(AEESoundPlayer * pme, AEESoundPlayerCmd cbType);
static void    AEESoundPlayer_NotifyCmdAborted(AEESoundPlayer * pme, AEESoundPlayerCmd cbType);
static void    AEESoundPlayer_NotifyCmdStatus(AEESoundPlayer * pme, int nRet, AEESoundPlayerCmd cmd, void * pData, int nSize);

//static boolean AEESoundPlayer_SetupSound(AEESoundPlayer * pme);
static IMedia * AEESoundPlayer_NewMedia(AEESoundPlayer * pme, AEESoundPlayerInfo * psi);
static void    AEESoundPlayer_MediaNotify(void * pUser, AEEMediaCmdNotify * pNotify);

// Sound callback function to be registered with ISound interface
static void AEESoundPlayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eSPStatus, uint32 dwParam);

static const VTBL(ISoundPlayer) gSoundPlayerFuncs =
{
   AEESoundPlayer_AddRef,
   AEESoundPlayer_Release,
   AEESoundPlayer_RegisterNotify,
   AEESoundPlayer_Set,
   AEESoundPlayer_Play,
   AEESoundPlayer_Stop,
   AEESoundPlayer_Rewind,
   AEESoundPlayer_FastForward,
   AEESoundPlayer_Pause,
   AEESoundPlayer_Resume,
   AEESoundPlayer_SetTempo,
   AEESoundPlayer_SetTune,
   AEESoundPlayer_SetVolume,
   AEESoundPlayer_GetVolume,
   AEESoundPlayer_GetTotalTime,
   AEESoundPlayer_SetSoundDevice,
   AEESoundPlayer_SetStream,
   AEESoundPlayer_SetInfo,
   AEESoundPlayer_GetInfo
};

static AEESoundPlayerCmdData   gsCmdData; // one static copy

/*===============================================================================
                     MACRO DEFINITIONS
===============================================================================*/
#define AEESoundPlayer_IsBusy(pme)        ((pme)->m_bPlaying || (pme)->m_bStreaming)
#define AEESoundPlayer_CanPlay(pme)       ((pme)->m_pMedia && !AEESoundPlayer_IsBusy(pme))
#define AEESoundPlayer_CanPlayCtlCmd(pme) ((pme)->m_pMedia && (pme)->m_bPlaying && !(pme)->m_bStreaming)

#ifdef MM_RELEASEIF
#undef MM_RELEASEIF
#endif
#define MM_RELEASEIF(p)          if (p) { IBASE_Release((IBase*)(p)); (p) = NULL; }

/*===============================================================================
                     FUNCTION DEFINITIONS
===============================================================================*/

/*==================================================================
Function: AEESoundPlayer_New

Description:
   Called to create SoundPlayer class.

Prototype:
   int AEESoundPlayer_New(IShell * pShell, AEECLSID clsid, void **ppobj);

Parameter(s):
   IShell * pShell: Pointer to the public IShell object
   AEECLSID clsId: the classid
   void **ppobj: dest pointer

Return Value: 
   SUCCESS on success
   some AEEError otherwise

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==================================================================*/
int AEESoundPlayer_New(IShell * pShell, AEECLSID clsid, void **ppobj)
{
   AEESoundPlayer *pme = 0;
   ISound         *piSound;
   int             nErr;

   // We need an ISound interface. If we cannot get one, return NULL.
   nErr = ISHELL_CreateInstance(pShell, AEECLSID_SOUND, (void **) &piSound);
   
   if (SUCCESS == nErr) {

      pme = (AEESoundPlayer *)AEE_OEM_NEWCLASS( (IBaseVtbl *)&gSoundPlayerFuncs, sizeof(AEESoundPlayer));
      
      if ((AEESoundPlayer *)0 != pme) {

         pme->m_pShell = pShell;
         pme->m_pAppContext = AEE_GetAppContext();

         pme->m_pISound = piSound;
         ISOUND_RegisterNotify(pme->m_pISound, &AEESoundPlayer_SoundCB, pme);
      } else {
         nErr = ENOMEMORY;
      }
   }
   
   *ppobj = pme;
   
   return nErr;
}

/*=====================================================================

Interface Method - Increment Reference Count

======================================================================*/
static uint32 AEESoundPlayer_AddRef(ISoundPlayer * po)
{
   AEESoundPlayer * pme = (AEESoundPlayer *) po;
   
   return (++(pme->m_nRefs));
}

/*=====================================================================

Interface Method - Decrement Reference Count. If Count reaches zero, does internal
cleanup

======================================================================*/
static uint32 AEESoundPlayer_Release(ISoundPlayer * po)
{
   AEESoundPlayer * pme = (AEESoundPlayer *) po;
   
   if(pme->m_nRefs <= 0)
      return 0;

   if (--pme->m_nRefs)
      return pme->m_nRefs;

   // Cancel ISound callback and release ISound
   if (pme->m_pISound)
   {
      ISOUND_RegisterNotify(pme->m_pISound, NULL, NULL);
      ISOUND_Release(pme->m_pISound);
   }

   AEESoundPlayer_FreeData(pme);

   FREE(pme);

   return(0);
}

/*=====================================================================

Interface Method - Registers the status event CB Function

======================================================================*/
static void AEESoundPlayer_RegisterNotify(ISoundPlayer * po,PFNSOUNDPLAYERSTATUS pfn, void * pUser)
{
   AEESoundPlayer * pme = (AEESoundPlayer *) po;
   
   pme->m_pfnUser = pfn;
   pme->m_pUser = pUser;
}

/*=====================================================================

Interface Method - Sets SoundPlayer Info Struct

======================================================================*/
int AEESoundPlayer_SetInfo(ISoundPlayer * po, AEESoundPlayerInfo * pInfo)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *)po;

   // If ISoundPlayer is waiting for a command completion, SetInfo is not allowed
   if (AEESoundPlayer_IsBusy(pme))
      return EITEMBUSY;

   AEESoundPlayer_FreeData(pme);
   if (!pInfo)
      return AEE_SUCCESS;

   pme->m_eSet = SPS_SETINFO;
   return AEESoundPlayer_IntSetInfo(pme, pInfo);
}

/*=====================================================================

Interface Method - Used by ISOUNDPLAYER_SetInfo

======================================================================*/

int AEESoundPlayer_IntSetInfo(AEESoundPlayer * pme, AEESoundPlayerInfo * pInfo)
{

   // If ISoundPlayer is waiting for a command completion, SetInfo is not allowed
//   if (AEESoundPlayer_IsBusy(pme))
//      return EITEMBUSY;

   // If no source data, return error
   if (!pInfo->pData)
      return EBADPARM;

   switch (pInfo->eInput)
   {
      case SDT_FILE:
         // Check for blank file name
         if ( STRCMP((char *)pInfo->pData, "") == 0 )
            return EBADPARM;

         pme->m_spInfo.pData = STRDUP((char *)pInfo->pData);
         pme->m_spInfo.dwSize = 0;
         pme->m_spInfo.eInput =  pInfo->eInput;       
         break;

      case SDT_BUFFER:
         // Make sure that data size is non-zero
         if (pInfo->dwSize == 0)
            return EBADPARM;

         MEMCPY(&pme->m_spInfo, pInfo, sizeof(pme->m_spInfo));

         break;

      case SDT_VOICEPROMPT:
         return EUNSUPPORTED;

      default:
         return EBADPARM;
   }

   if (!pme->m_bDataSet)
      pme->m_pMedia = AEESoundPlayer_NewMedia(pme, &pme->m_spInfo);

   return AEE_SUCCESS;
}

/*=====================================================================

Interface Method - Get SoundPlayer Info

======================================================================*/

int AEESoundPlayer_GetInfo(ISoundPlayer * po, AEESoundPlayerInfo * pInfo)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *)po;

   // If pInfo is NULL, then return the state of ISoundPlayer
   if (!pInfo)
      return AEESoundPlayer_IsBusy(pme) ? EITEMBUSY : EIDLE;

   MEMCPY(pInfo, &pme->m_spInfo, sizeof(pme->m_spInfo));

   return AEE_SUCCESS;
}

/*=====================================================================

Interface Method - Set the data/file for the SoundPlayer

======================================================================*/
static void AEESoundPlayer_Set(ISoundPlayer * po, AEESoundPlayerInput t, void * pData)
{
   AEESoundPlayer *     pme = (AEESoundPlayer *)po;
   AEESoundPlayerInfo   spInfo;

   // If playback is on and the second Play() is not already pending, 
   // then allow the Set() to proceed and mark that data has been set
   // while a playback is on.
   if (pme->m_bPlaying)
   {
      if (pme->m_bPlayIssued)
      {
         AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
         if (pData && t == SDT_BUFFER)
         {
            FREE(pData);
         }

         return;
      }

      // Create new media and keep it pending...
      spInfo.eInput = t;
      spInfo.pData = pData;
      spInfo.dwSize = AEESP_SET_BUFFER_SIZE; // AEEMedia_CreateUtil() needs at least 16 bytes.
      pme->m_pMediaPending = AEESoundPlayer_NewMedia(pme, &spInfo);
      if (!pme->m_pMediaPending)
      {
         AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
         return;
      }

      pme->m_bDataSet = TRUE;
   }
   else
      pme->m_bDataSet = FALSE;

   AEESoundPlayer_FreeData(pme);

   spInfo.eInput = t;
   spInfo.pData = pData;

   switch (t)
   {
      case SDT_VOICEPROMPT:
      case SDT_FILE:
      break;

      case SDT_BUFFER:
         if ( pData )
         {
            pme->m_pRawData = pData;
            spInfo.pData = pData;
         }
         // Note: ISOUNDPLAYER_Set() does not work for SDT_BUFFER input type since
         // size of the buffer is not specified.
         // As a result, this API is deprecated. Instead app developers must use
         // ISOUNDPLAYER_SetInfo().
         // NOTE: On MSM-based devices MIDI and QCP play even if size is not set but MP3 will not play.
         // For backward compatibility, dwSize is set to 1 so that 
         // AEESoundPlayer_SetInfo() succeeds.
         spInfo.dwSize = AEESP_SET_BUFFER_SIZE; // AEEMedia_CreateUtil() needs at least 16 bytes.
         break;
   } // switch

   // Call AEESoundPlayer_IntSetInfo() to set the source audio data
   pme->m_eSet = SPS_SET;
   if ( AEE_SUCCESS != AEESoundPlayer_IntSetInfo(pme, &spInfo) )
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
   else
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);
}

/*=====================================================================

Internal Method - Reallocation routine for the data buffer

======================================================================*/

static boolean AEESoundPlayer_Realloc(AEESoundPlayer * pme)
{
   void * pNew;

   pme->m_dwBytes += AEESOUNDPLAYER_MIN_READ;

   pNew = sys_realloc(pme->m_pRawData, pme->m_dwBytes);   

   if(pNew){
      pme->m_pRawData = pNew;
      return(TRUE);
   }

   sys_free(pme->m_pRawData);
   pme->m_dwBytes = 0;
   return(FALSE);
}
   
/*=====================================================================

Internal Method - CB Fucntion invoked when data read from buffer

======================================================================*/

static void AEESoundPlayer_ReadableCB(AEESoundPlayer * pme)
{
   int32    nRead,nAvail;
   byte *   pRead;

   while (1)
   {
      nAvail = pme->m_dwBytes - pme->m_dwRead;
      if (!nAvail)
      {
         if(!AEESoundPlayer_Realloc(pme))
            break;
         nAvail = AEESOUNDPLAYER_MIN_READ;
      }   
      pRead = (byte *)pme->m_pRawData + pme->m_dwRead;
      nRead = IASTREAM_Read(pme->m_pStream,pRead,nAvail);

   // In Progress?

      if (nRead == AEE_STREAM_WOULDBLOCK)
         break;

      if (nRead ==  AEE_NET_ERROR)
      {
         AEESoundPlayer_FreeData(pme);
         AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
         return;
      }

   // Finished?

      if (!nRead)
      {
         AEESoundPlayerInfo   spInfo;

         pme->m_bStreaming = FALSE;
         IASTREAM_Release(pme->m_pStream);
         pme->m_pStream = NULL;

         // Call AEESoundPlayer_IntSetInfo() to set the source audio data
         spInfo.eInput = SDT_BUFFER;
         spInfo.pData = pme->m_pRawData;
         spInfo.dwSize = pme->m_dwRead;
         if ( AEE_SUCCESS != AEESoundPlayer_IntSetInfo(pme, &spInfo) )
            AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
         else
            AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);

         return;
      } // if

      pme->m_dwRead += nRead;
   } // while

   IASTREAM_Readable(pme->m_pStream, (PFNNOTIFY)AEESoundPlayer_ReadableCB, pme);
}
            

/*=====================================================================

Interface Method - SetStream in SoundPlayer

======================================================================*/
static void AEESoundPlayer_SetStream(ISoundPlayer * po, IAStream * ps)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *)po;
   int32             nSize;

   if (AEESoundPlayer_IsBusy(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   AEESoundPlayer_FreeData(pme);

   pme->m_dwBytes = pme->m_dwRead = 0;

   nSize = IASTREAM_Read(ps, NULL, 0);
   if(nSize > 0)
   {
      nSize++;
      pme->m_pRawData = sys_malloc(nSize); 
      if (!pme->m_pRawData)
      {
         AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
         return;
      }
      pme->m_dwBytes = nSize;
   }

   pme->m_pStream = ps;
   pme->m_bStreaming = TRUE;
   pme->m_eSet = SPS_SETSTREAM;
   // Inc ref count of IAStream and internal ref count of ISoundPlayer
   IASTREAM_AddRef(ps);
   AEESoundPlayer_ReadableCB(pme);
}


/*=====================================================================

Interface Method - Plays the given file

======================================================================*/

static void AEESoundPlayer_Play(ISoundPlayer * po)
{
   AEESoundPlayer *     pme = (AEESoundPlayer *) po;

   // If Set() has been done during a playback and if second play is not pending,
   // then issue a Stop() and mark the second play as pending.
   if (pme->m_bDataSet && !pme->m_bPlayIssued)
   {
      pme->m_bDataSet = FALSE;
      pme->m_bPlayIssued = TRUE;
      AEESoundPlayer_Stop(po);
      return;
   }

   if (!AEESoundPlayer_CanPlay(pme))
   {
      if (!pme->m_bPlayIssued)
         AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_PLAY_CB);
      return;
   }

   // If a get time is already in progress, send the abort.
   if (pme->m_bGetTotalTime)
   {
      pme->m_bGetTotalTime = FALSE;
      AEESoundPlayer_NotifyCmdAborted(pme, AEE_SOUNDPLAYER_TIME_CB);
   }

   // Create IMedia and start playback...
   if (!pme->m_pMedia || IMEDIA_Play(pme->m_pMedia) != SUCCESS)
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_PLAY_CB);
   else
   {
      pme->m_bPlaying = TRUE;
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_PLAY_CB);
   }
}

/*=====================================================================

Interface Method - Stops the previous SoundPlayer command 

======================================================================*/
static void AEESoundPlayer_Stop(ISoundPlayer * po)
{
   AEESoundPlayer * pme = (AEESoundPlayer *)po;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   if (SUCCESS == IMEDIA_Stop(pme->m_pMedia))
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);
   else
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
}

/*=====================================================================

Interface Method - Performs Rewind Operation

======================================================================*/
static void AEESoundPlayer_Rewind(ISoundPlayer * po,uint32 dwTime)
{
   AEESoundPlayer * pme = (AEESoundPlayer *)po;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   if (SUCCESS == IMEDIA_Rewind(pme->m_pMedia, (int32)dwTime))
   {
      pme->m_bRewind = TRUE;
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);
   }
   else
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
}

/*=====================================================================

Interface Method - Performs Fast Forward Operation

======================================================================*/
static void AEESoundPlayer_FastForward(ISoundPlayer * po,uint32 dwTime)
{
   AEESoundPlayer * pme = (AEESoundPlayer *)po;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   if (SUCCESS == IMEDIA_FastForward(pme->m_pMedia, dwTime))
   {
      pme->m_bRewind = FALSE;
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);
   }
   else
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
}

/*=====================================================================

Interface Method - Increment Reference Count

======================================================================*/

static void AEESoundPlayer_Pause(ISoundPlayer * po)
{
   AEESoundPlayer * pme = (AEESoundPlayer *)po;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   if (SUCCESS == IMEDIA_Pause(pme->m_pMedia))
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);
   else
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
}

/*=====================================================================

Interface Method - Performs Resume Operation

======================================================================*/

static void AEESoundPlayer_Resume(ISoundPlayer * po)
{
   AEESoundPlayer * pme = (AEESoundPlayer *)po;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   if (SUCCESS == IMEDIA_Resume(pme->m_pMedia))
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_STATUS_CB);
   else
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
}

/*=====================================================================

Interface Method - Performs SetTempo Operation

======================================================================*/
static void AEESoundPlayer_SetTempo(ISoundPlayer * po,uint32 dwTempoFactor)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *)po;
   int               nRet;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme))
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   nRet = IMEDIA_SetMediaParm(pme->m_pMedia, MM_PARM_TEMPO, (int32)dwTempoFactor, 0);
   AEESoundPlayer_NotifyCmdStatus(pme, nRet, AEE_SOUNDPLAYER_STATUS_CB, NULL, 0);
}

/*=====================================================================

Interface Method - Performs SetTune Operation

======================================================================*/

static void AEESoundPlayer_SetTune(ISoundPlayer * po, int8 nStep)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *) po;
   int               nRet;

   // Check if playback control command can be issued
   if (!AEESoundPlayer_CanPlayCtlCmd(pme) || nStep > 12 || nStep < -12)
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_STATUS_CB);
      return;
   }

   nRet = IMEDIA_SetMediaParm(pme->m_pMedia, MM_PARM_TUNE, (int32)(0x40 + nStep), 0);
   AEESoundPlayer_NotifyCmdStatus(pme, nRet, AEE_SOUNDPLAYER_STATUS_CB, NULL, 0);
}

/*=====================================================================

Interface Method - Performs SetVolume Operation

======================================================================*/
static void AEESoundPlayer_SetVolume(ISoundPlayer * po,uint16 wVolume)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *) po;
   int               nRet;
   
//   if (AEESoundPlayer_SetupSound(pme))
//      ISOUND_SetVolume(pme->m_pISound, wVolume);

   if (pme->m_pMedia)
      nRet = IMEDIA_SetVolume(pme->m_pMedia, wVolume);
   else
      nRet = EFAILED;

   AEESoundPlayer_NotifyCmdStatus(pme, nRet, AEE_SOUNDPLAYER_STATUS_CB, NULL, 0);
}

/*=====================================================================

Interface Method - Performs GetVolume Operation

======================================================================*/
static void AEESoundPlayer_GetVolume(ISoundPlayer * po)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *) po;
   int               nRet;
   uint16            wVolume;

//   if (AEESoundPlayer_SetupSound(pme))
//      ISOUND_GetVolume(pme->m_pISound);

   if (pme->m_pMedia)
      nRet = IMEDIA_GetVolume(pme->m_pMedia, &wVolume);
   else
      nRet = EFAILED;

   if (nRet != MM_PENDING)
   {
      int      nStatus;
      uint32   dwParam = 0;

      if (!pme->m_pfnUser)
         return;

      nStatus = (nRet == SUCCESS) ? AEE_SOUNDPLAYER_SUCCESS : AEE_SOUNDPLAYER_FAILURE;
      if (nRet == SUCCESS)
      {
         gsCmdData.wVolume = wVolume;
         dwParam = (uint32)&gsCmdData;
      }
      pme->m_pfnUser(pme->m_pUser, AEE_SOUNDPLAYER_VOLUME_CB, (AEESoundPlayerStatus) nStatus, dwParam);
   }
}

/*=====================================================================

Interface Method - Performs GetTotalTime operation

======================================================================*/
static void AEESoundPlayer_GetTotalTime(ISoundPlayer * po)
{
   AEESoundPlayer *  pme = (AEESoundPlayer *)po;

   // If playback, streaming or another GetTotalTime is going on, 
   // then return failure
   if (pme->m_bPlaying || pme->m_bStreaming)
   {
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_TIME_CB);
      return;
   }

   // If a get time is already in progress, send the abort.
   if (pme->m_bGetTotalTime)
   {
      AEESoundPlayer_NotifyCmdAborted(pme, AEE_SOUNDPLAYER_TIME_CB);
      return;
   }

   if (!pme->m_pMedia || IMEDIA_GetTotalTime(pme->m_pMedia) != SUCCESS)
      AEESoundPlayer_NotifyCmdFailure(pme, AEE_SOUNDPLAYER_TIME_CB);
   else
   {
      pme->m_bGetTotalTime = TRUE;
      AEESoundPlayer_NotifyCmdSuccess(pme, AEE_SOUNDPLAYER_TIME_CB);
   }
}

/*=====================================================================

Interface Method - Performs SetSoundDevice Operation

======================================================================*/

static void AEESoundPlayer_SetSoundDevice(ISoundPlayer * po,AEESoundDevice eDevice,AEESoundMuteCtl eEarMute,AEESoundMuteCtl eMicMute)
{
   AEESoundPlayer * pme = (AEESoundPlayer *) po;
   AEESoundInfo   si;
   
   if (!pme->m_pISound) // || pme->m_cbCommand.bInUse)
   {
      if ( pme->m_pfnUser ) {
         pme->m_pfnUser(pme->m_pUser, AEE_SOUNDPLAYER_SOUND_CB, AEE_SOUNDPLAYER_FAILURE, 0);
      }
      return;
   }

   ISOUND_RegisterNotify(pme->m_pISound, &AEESoundPlayer_SoundCB, pme);

  // Get the current sound info
   ISOUND_Get(pme->m_pISound, &si);

   // Fill-in new values and set sound info
   si.eDevice = eDevice;
   si.eEarMuteCtl = eEarMute;
   si.eMicMuteCtl = eMicMute;
   if ( pme->m_pfnUser && (AEE_SUCCESS != ISOUND_Set(pme->m_pISound, &si)) )
   {
      pme->m_pfnUser(pme->m_pUser, AEE_SOUNDPLAYER_SOUND_CB, AEE_SOUNDPLAYER_FAILURE, 0);
      return;
   }
   
   ISOUND_SetDevice(pme->m_pISound);
}

/*==================================================================

  Local Function; Frees Sound Player Data

==================================================================*/
static void AEESoundPlayer_FreeData(AEESoundPlayer * pme)
{

   if (pme->m_pStream)
   {
      IASTREAM_Release(pme->m_pStream);
      pme->m_pStream = NULL;
      pme->m_dwBytes = pme->m_dwRead = 0;
      pme->m_bStreaming = FALSE;
   }

   // In case SetStream() is done, then m_pRawData must be released
   // App buffer free not required: Since IShell will free and AsyncSysError() 
   // is not displayed on commercial handset.
   if (pme->m_eSet == SPS_SETSTREAM)
   {
      if (pme->m_pRawData)
      {
         sys_free(pme->m_pRawData);
         pme->m_pRawData = NULL;
      }
   }
   else if (pme->m_eSet == SPS_SET)
   {
      //
      // The following is provided for backward compatibility with BREW 1.0.
      // If an ISOUNDPLAYER_Set() has already been done, then free the
      // buffer allocated by user.
      //

      if (pme->m_pRawData)
      {
         FREE(pme->m_pRawData);
         pme->m_pRawData = NULL;
      }
   }

   if (pme->m_spInfo.eInput == SDT_FILE)
      FREE(pme->m_spInfo.pData);

   if (!pme->m_bDataSet)
      MM_RELEASEIF(pme->m_pMedia);

   pme->m_spInfo.eInput = SDT_NONE;
   pme->m_spInfo.pData = NULL;
   pme->m_spInfo.dwSize = 0;
//   pme->m_eSet = SPS_NONE;
}

/*==================================================================

  Local Function

==================================================================*/
static void AEESoundPlayer_NotifyCmdSuccess(AEESoundPlayer * pme, AEESoundPlayerCmd cbType)
{
   if (pme->m_pfnUser) {
      pme->m_pfnUser(pme->m_pUser, cbType, AEE_SOUNDPLAYER_SUCCESS, 0);
   }
}

/*==================================================================

  Local Function

==================================================================*/
static void AEESoundPlayer_NotifyCmdFailure(AEESoundPlayer * pme, AEESoundPlayerCmd cbType)
{
   if (pme->m_pfnUser) {
      pme->m_pfnUser(pme->m_pUser, cbType, AEE_SOUNDPLAYER_FAILURE, 0);
   }
}

/*==================================================================

  Local Function

==================================================================*/
static void AEESoundPlayer_NotifyCmdAborted(AEESoundPlayer * pme, AEESoundPlayerCmd cbType)
{
   if (pme->m_pfnUser) {
      pme->m_pfnUser(pme->m_pUser, cbType, AEE_SOUNDPLAYER_ABORTED, 0);
   }
}

/*==================================================================

  Local Function

==================================================================*/
static void AEESoundPlayer_NotifyCmdStatus(AEESoundPlayer * pme, int nRet, AEESoundPlayerCmd cmd, void * pData, int nSize)
{
   AEESoundPlayerCmdData   gsCmdData;

   if (!pme->m_pfnUser)
      return;

   MEMSET(&gsCmdData, (int)0, sizeof(gsCmdData));

   switch (nRet)
   {
      case SUCCESS:
      {
         uint32   dwParam = 0;
         if (pData)
         {
            MEMCPY(&gsCmdData, pData, nSize);
            dwParam = (uint32)&gsCmdData;
         }
         pme->m_pfnUser(pme->m_pUser, cmd, AEE_SOUNDPLAYER_SUCCESS, dwParam);
         break;
      }

      case MM_PENDING:
         break;

      default:
         pme->m_pfnUser(pme->m_pUser, cmd, AEE_SOUNDPLAYER_FAILURE, 0);
         break;
   }
}

/*==================================================================

  Local Function

==================================================================*/
static IMedia * AEESoundPlayer_NewMedia(AEESoundPlayer * pme, AEESoundPlayerInfo * psi)
{   
   AEEMediaData         md;
   AEESoundPlayerInput  eInput = psi->eInput;
   IMedia *             pMedia;

   // Create IMedia interface...

   if (eInput == SDT_FILE)
      md.clsData = MMD_FILE_NAME;
   else if (eInput == SDT_BUFFER)
      md.clsData = MMD_BUFFER;
   else
      return NULL;

   md.pData = psi->pData;
   md.dwSize = psi->dwSize;

   if (AEEMediaUtil_CreateMedia(pme->m_pShell, &md, &pMedia))
      return NULL;
   else if (IMEDIA_RegisterNotify(pMedia, AEESoundPlayer_MediaNotify, pme))
      MM_RELEASEIF(pMedia);

   return pMedia;
}

/*==================================================================

  Local Function

==================================================================*/
/*
static boolean AEESoundPlayer_SetupSound(AEESoundPlayer * pme)
{
   AEESoundInfo      si;
   AEECLSID          cls = 0;

   if (!pme->m_pISound)
   {
      if ( pme->m_pfnUser ) {
         pme->m_pfnUser(pme->m_pUser, AEE_SOUNDPLAYER_SOUND_CB, AEE_SOUNDPLAYER_FAILURE, 0);
      }
      return FALSE;
   }
   
   ISOUND_RegisterNotify(pme->m_pISound, &AEESoundPlayer_SoundCB, pme);

   // Get the current sound info
   ISOUND_Get(pme->m_pISound, &si);

   if (pme->m_pMedia)
      IMEDIA_GetClassID(pme->m_pMedia, &cls);

   if (cls == AEECLSID_MEDIAMIDI || cls == AEECLSID_MEDIAMP3)
      si.eMethod = (AEESoundMethod) AEE_SOUND_METHOD_MIDI;
   else if (cls == AEECLSID_MEDIAQCP)
      si.eMethod = (AEESoundMethod) AEE_SOUND_METHOD_VOICE;
   else
      si.eMethod = (AEESoundMethod) AEE_SOUND_METHOD_BEEP;
   
   // Fill-in new values and set sound info
   if ( AEE_SUCCESS != ISOUND_Set(pme->m_pISound, &si) )
   {
      if ( pme->m_pfnUser ) {
         pme->m_pfnUser(pme->m_pUser, AEE_SOUNDPLAYER_SOUND_CB, AEE_SOUNDPLAYER_FAILURE, 0);
      }
      return FALSE;
   }

   return TRUE;
}
*/

/*==================================================================

  ISOUND callback.  Issued in foreground by ISound

==================================================================*/
static void AEESoundPlayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eSPStatus, uint32 dwParam)
{
   // This function is always called in AEE context. User callback can be directly called
   AEESoundPlayer *     pme = (AEESoundPlayer *) pUser;
   AEESoundPlayerStatus eSP = AEE_SOUNDPLAYER_UNKNOWN;
   AEESoundPlayerCmd    eCmd;
   
   if ( eCBType == AEE_SOUND_STATUS_CB )
      eCmd = AEE_SOUNDPLAYER_SOUND_CB;
   else
      eCmd = AEE_SOUNDPLAYER_VOLUME_CB;
   
   MEMSET(&gsCmdData, (int)0, sizeof(gsCmdData));

   // If user notification is canceled, then do not process callback
   if ( pme && pme->m_pfnUser )
   {
      switch (eSPStatus)
      {
         case AEE_SOUND_SUCCESS:
            eSP = AEE_SOUNDPLAYER_SUCCESS;
            if (eCmd == AEE_SOUNDPLAYER_VOLUME_CB && dwParam)
               gsCmdData.wVolume = ((AEESoundCmdData *)dwParam)->wVolume;
            break;
         case AEE_SOUND_FAILURE:
            eSP = AEE_SOUNDPLAYER_FAILURE;
            break;
      }

      pme->m_pfnUser(pme->m_pUser, eCmd, eSP, dwParam ? (uint32)&gsCmdData : 0);
   }
}

/*==================================================================

  Foreground callback to notify the caller about status

==================================================================*/
static void AEESoundPlayer_MediaNotify(void * pUser, AEEMediaCmdNotify * pNotify)
{
   AEESoundPlayer *        pme = (AEESoundPlayer *)pUser;
   int                     nCmd = -1;
   int                     nStatus = -1;
   uint32                  dwParam = (uint32)pNotify->pCmdData;
   boolean                 bCmdData = FALSE;
   void *                  paLast;

   if (!pme || !pNotify || !pme->m_pMedia || pNotify->pIMedia != pme->m_pMedia)
      return;

   MEMSET(&gsCmdData, (int)0, sizeof(gsCmdData));

   switch (pNotify->nStatus)
   {
      case MM_STATUS_DONE:          nStatus = AEE_SOUNDPLAYER_DONE;        break;
      case MM_STATUS_ABORT:         nStatus = AEE_SOUNDPLAYER_ABORTED;     break;
      case MM_STATUS_TICK_UPDATE:   nStatus = AEE_SOUNDPLAYER_TICK_UPDATE; break;
      case MM_STATUS_REPEAT:        nStatus = AEE_SOUNDPLAYER_REPEAT;      break;
      case MM_STATUS_UNKNOWN:       nStatus = AEE_SOUNDPLAYER_UNKNOWN;     break;

      case MM_STATUS_MEDIA_SPEC:
      {
         AEECLSID cls;

         nStatus = AEE_SOUNDPLAYER_AUDIO_SPEC;

         bCmdData = TRUE;
         IMEDIA_GetClassID(pme->m_pMedia, &cls);
         if (cls == AEECLSID_MEDIAMIDI)
            gsCmdData.spSpec.fType = AEE_SOUNDPLAYER_FILE_MIDI;
         else if (cls == AEECLSID_MEDIAMP3)
            gsCmdData.spSpec.fType = AEE_SOUNDPLAYER_FILE_MP3;
         else
            gsCmdData.spSpec.fType = AEE_SOUNDPLAYER_FILE_UNKNOWN;

         break;
      }

      case MM_STATUS_DATA_IO_DELAY:
         nStatus = AEE_SOUNDPLAYER_DATA_ACCESS_DELAY;
         gsCmdData.dwElapsedTime = dwParam;
         break;

      case MM_STATUS_SEEK:
         nStatus = pme->m_bRewind ? AEE_SOUNDPLAYER_REWIND : AEE_SOUNDPLAYER_FFORWARD; 
         gsCmdData.dwElapsedTime = dwParam;
         bCmdData = TRUE;
         break;

      case MM_STATUS_PAUSE:
         nStatus = AEE_SOUNDPLAYER_PAUSE;
         gsCmdData.dwElapsedTime = dwParam;
         bCmdData = TRUE;
         break;

      case MM_STATUS_RESUME:
         nStatus = AEE_SOUNDPLAYER_RESUME;
         gsCmdData.dwElapsedTime = dwParam;
         bCmdData = TRUE;
         break;
   } // switch

   switch (pNotify->nCmd)
   {
      case MM_CMD_PLAY:
         nCmd = AEE_SOUNDPLAYER_PLAY_CB;
         break;

      case MM_CMD_GETTOTALTIME:
         nCmd = AEE_SOUNDPLAYER_TIME_CB; 
         if (nStatus == AEE_SOUNDPLAYER_DONE)
         {
            gsCmdData.dwTotalTime = dwParam;
            bCmdData = TRUE;
         }
         break;

      case MM_CMD_SETMEDIAPARM:
         if (pNotify->nSubCmd == MM_PARM_VOLUME)
            nCmd = AEE_SOUNDPLAYER_SOUND_CB;
         else if (pNotify->nSubCmd == MM_PARM_TUNE || pNotify->nSubCmd == MM_PARM_TEMPO)
         {
            nCmd = AEE_SOUNDPLAYER_PLAY_CB;
            if (nStatus == AEE_SOUNDPLAYER_DONE)
            {
               gsCmdData.dwTune = dwParam;
               bCmdData = TRUE;
            }
         }

         if (nStatus == AEE_SOUNDPLAYER_DONE)
            nStatus = AEE_SOUNDPLAYER_SUCCESS;
         else
            nStatus = AEE_SOUNDPLAYER_FAILURE;

         break;

      case MM_CMD_GETMEDIAPARM:
         if (pNotify->nSubCmd == MM_PARM_VOLUME)
         {
            nCmd = AEE_SOUNDPLAYER_VOLUME_CB;
            if (nStatus == AEE_SOUNDPLAYER_DONE)
            {
               gsCmdData.wVolume = (uint16)dwParam;
               bCmdData = TRUE;
            }
         }

         if (nStatus == AEE_SOUNDPLAYER_DONE)
            nStatus = AEE_SOUNDPLAYER_SUCCESS;
         else
            nStatus = AEE_SOUNDPLAYER_FAILURE;

         break;
   } // switch

   // For play and time commands, if DONE or ABORTED is received, then
   // the IMedia object must be released.
   if (nCmd == AEE_SOUNDPLAYER_PLAY_CB || nCmd == AEE_SOUNDPLAYER_TIME_CB)
   {
      if (nStatus == AEE_SOUNDPLAYER_DONE || nStatus == AEE_SOUNDPLAYER_ABORTED)
      {
         if (nCmd == AEE_SOUNDPLAYER_PLAY_CB)
         {
            pme->m_bPlaying = FALSE;   // No longer playing
            // This callback may be the result of second Play() that issued a stop
            // due to a Set() during the playback. 
            // If that is the case, then issue a new Play() command to playback the
            // new media data. Clear the flags.
            pme->m_bDataSet = FALSE;
            if (pme->m_bPlayIssued)
            {
               MM_RELEASEIF(pme->m_pMedia);
               pme->m_pMedia = pme->m_pMediaPending;
               pme->m_pMediaPending = NULL;
               pme->m_bPlayIssued = FALSE;
               AEESoundPlayer_Play((ISoundPlayer *)pme);
            }
         } // play cmd AEE_SOUNDPLAYER_TIME_CB
         else
         {
            if (pme->m_bGetTotalTime)
               pme->m_bGetTotalTime = FALSE;
            else
            {
               nCmd = -1;
               nStatus = -1;
            }
         }
      } // done or abort
   } // play or time cmd

   if (pme->m_pfnUser && nCmd != -1 && nStatus != -1)
   {
      paLast = AEE_EnterAppContext(pme->m_pAppContext);
      AEESoundPlayer_AddRef((ISoundPlayer *)pme);
      pme->m_pfnUser(pme->m_pUser, (AEESoundPlayerCmd)nCmd, (AEESoundPlayerStatus)nStatus, bCmdData ? (uint32)&gsCmdData : 0);
      AEESoundPlayer_Release((ISoundPlayer *)pme);
      AEE_LeaveAppContext(paLast);
   }
}

