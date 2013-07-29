/*===========================================================================

FILE: OEMMedia.c

SERVICES: IMEDIA interface implementation

DESCRIPTION
   This file implements IMedia for BREW MultiMedia format. All 
   media formats like MIDI, MP3, QCP must be derived from this class.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright ?1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"
#include "OEMMedia.h"
#include "OEMFS.h"

#if defined(FEATURE_BREW_MULTIMEDIA)

#include "OEMHeap.h"
#include "AEE_OEM.h"
#include "AEE_OEMFile.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "AEEFile.h"
#include "AEEMimeTypes.h"
#include "AEESource.h"
#ifdef FEATURE_VERSION_K212
#include "snddev.h"
#include "snd.h"
#include "gpio_1100.h"
#include "clk.h"
#endif
#if defined (FEATURE_ACM)
#include "msg.h"
#include "OEMACM.h"
#if defined (FEATURE_REAL_AUDIO)
#include "AEEMediaReal.h"
#endif // FEATURE_REAL_AUDIO
#endif // FEATURE_ACM

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/

#define OEMMEDIA_AAC_IS_RAW(x) (((byte *)(x))[0] == 0x56 && (((byte *)(x))[1]) == 0xE0) // CR 144507
#define AEEMedia_IsPlayOK(p)           (AEEMedia_IsReadyState(p) && !AEEMedia_IsStateChanging(p))
#define AEEMedia_IsRecOK(p)            AEEMedia_IsPlayOK(p)
#define AEEMedia_IsSeekOK(p)           (AEEMedia_IsPlayRecOn(p) && !AEEMedia_IsStateChanging(p))
#define AEEMedia_IsPauseOK(p)          ((AEEMedia_IsPlayState(p) || AEEMedia_IsRecState(p)) \
                                        && !AEEMedia_IsStateChanging(p))
#define AEEMedia_IsResumeOK(p)         ((AEEMedia_IsPlayPauseState(p) || AEEMedia_IsRecPauseState(p)) \
                                        && !AEEMedia_IsStateChanging(p))
#define AEEMedia_IsStopOK(p)           ((p) != NULL) // Stop always ok...(AEEMedia_IsPlayRecOn(p) && !AEEMedia_IsStateChanging(p))
#define AEEMedia_IsGetTotalTimeOK(p)   AEEMedia_IsPlayOK(p)

#define AEEMedia_IsPlayCtlFail(s)      ((s)==MM_STATUS_SEEK_FAIL || (s)==MM_STATUS_PAUSE_FAIL \
                                        || (s)==MM_STATUS_RESUME_FAIL)

#define AEEMedia_SetState(pme, nState) \
   {pme->m_nState = nState; \
    pme->m_bStateChanging = FALSE;}

#if defined (FEATURE_ACM)
#define MM_NOACTION (-1)                // Indicates when no ACM related actions are needed

/*===========================================================================

                     TYPE DEFINITIONS 

===========================================================================*/
typedef struct ResourceCntlData
{ 
  int          nPlayType;             // Play type, ie: MM_PLAY_TYPE_XXX
  uint32       nAcmCB;                // Callback registered with ACM
  flg          bChannelShare;         // Multi-channel audio flag
} ResourceCntlData;
#endif // FEATURE_ACM

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
static int     AEEMedia_SetMediaData(AEEMedia * pme, AEEMediaDataEx * pMediaData, boolean bEx);
static boolean AEEMedia_FilePath(AEEMedia * pme, void * pData);
static boolean AEEMedia_UrlPath(AEEMedia * pme, void * pData);
static void    AEEMedia_AppCallback(AEEMedia * pme, AEEMediaCallback * pmcb);

// State handlers
static void AEEMedia_ReadySH(AEEMedia * pme, AEEMediaCallback * pmcb);
static void AEEMedia_PlaySH(AEEMedia * pme, AEEMediaCallback * pmcb);
static void AEEMedia_PlayPauseSH(AEEMedia * pme, AEEMediaCallback * pmcb);
static void AEEMedia_RecordSH(AEEMedia * pme, AEEMediaCallback * pmcb);
static void AEEMedia_RecordPauseSH(AEEMedia * pme, AEEMediaCallback * pmcb);

#if defined (FEATURE_ACM)
         int    AEEMedia_RequestResource(AEEMedia * pme, flg isPlay);
  static uint32 AEEMedia_GetReasonCode(AEEMedia* pme, ResourceCntlData * rdata, flg isPlay);
  static int AEEMedia_AssignResourceCntlData(AEEMedia* pme, ResourceCntlData * rdata, flg isPlay);

  void AEEMedia_CleanUpResource(AEEMedia *po); 

  extern void CMediaBg_SetResourceCntlCBData(uint32 * acmCB, flg isPlay);
  extern void CMediaBg_GetResourceCntlData(AEEMedia * pme, int * nPlayType, flg * channelShare);

  // these functions are needed so we can get the data needed to acquire
  // the resource.
#if defined (FEATURE_REAL_AUDIO)
  extern void CMediaReal_GetResourceCntlData(AEEMedia * pme, int * nPlayType, uint32 *acmCB);
#endif // FEATURE_REAL_AUDIO
#if defined (FEATURE_APP_MPEG4)
  extern void OEMMediaMPEG4_GetResourceCntlData(AEEMedia * pme, int * nPlayType, uint32 *acmCB);
#endif // FEATURE_APP_MPEG4
  extern void CMediaMIDIOutQCP_GetResourceCntlData(AEEMedia * pme, int * nPlayType, flg * channelShare, uint32 *acmCB);
  extern void CMediaMIDIOutMsg_GetResourceCntlData(AEEMedia * pme, int * nPlayType, uint32 *acmCB);
  extern void CMediaMIDIOutQCP_CleanupResource (AEEMedia *pme);
  extern void CMediaPMD_GetResourceCntlData(AEEMedia *po, uint32 *acmCB);
#endif // FEATURE_ACM

/*===============================================================================

                     PUBLIC FUNCTION DEFINITIONS

===============================================================================*/

/*==================================================================
   Registers the handler and MIME type

==================================================================*/
void AEEMedia_Init(IShell * ps, char * szMIME, AEECLSID clsHandler)
{
   //Deregister any existing handle for this MIME type
   ISHELL_RegisterHandler(ps, AEECLSID_MEDIA, szMIME, 0);
   //Register this class as the handler
   ISHELL_RegisterHandler(ps, AEECLSID_MEDIA, szMIME, clsHandler);
}

/*==================================================================
   Registers the handler and MIME type

==================================================================*/
int AEEMedia_New(IMedia * po, IShell * ps, AEECLSID cls)
{
   AEEMedia *  pme = (AEEMedia *) po;

   pme->m_pIShell = ps;
   pme->m_clsSelf = cls;

   pme->m_pac = AEE_GetAppContext();
   pme->m_nState = MM_STATE_IDLE;

   CALLBACK_Init(&pme->m_cbSysObj, AEEMedia_Delete, pme);
   AEE_LinkSysObject(&pme->m_cbSysObj);

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)  
  {
      int nRet = SUCCESS;
      nRet = ISHELL_CreateInstance(ps, AEECLSID_APPRESCTL,(void**)(&pme->m_pResCtl));

      if (nRet != SUCCESS)
      {
         return nRet;
      }

      if (NULL == pme->m_pResCtl)
      {
         return ENOMEMORY;
      }
   }
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM

   return SUCCESS;
}

/*==================================================================
   Registers the handler and MIME type

==================================================================*/
void AEEMedia_Delete(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *) po;

   // Cancel user registered callback
   pme->m_pfnNotify = NULL;
   pme->m_pUser = NULL;

   // If the media is in play/rec state, then stop it.
   if (AEEMedia_IsStopOK(pme))
      IMEDIA_Stop(po);

   CALLBACK_Cancel(&pme->m_cbSysObj);

   FREEIF(pme->m_pszFile);
   FREEIF(pme->m_pszFileAppPath);

   if (pme->m_md.clsData >= MMD_ISOURCE && pme->m_md.pData)
      MM_RELEASEIF(pme->m_md.pData);

   FREEIF(pme->m_pSpec);

#if defined (FEATURE_ACM) 
    AEEMedia_CleanUpResource(pme);
#if defined (FEATURE_ODM_UI)
    if (pme->m_pResCtl)
    {
        IAPPRESCTL_Release(pme->m_pResCtl);
        pme->m_pResCtl = NULL;
    }
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM
}

/*==================================================================

==================================================================*/
uint32 AEEMedia_AddRef(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *) po;
   
   return (++(pme->m_nRefs));
}

/*==================================================================

==================================================================*/
uint32 AEEMedia_Release(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *) po;

   if(pme->m_nRefs <= 0)
      return 0;

   if (--pme->m_nRefs) 
      return pme->m_nRefs;      

   return(0);


}

/*==================================================================

==================================================================*/
int AEEMedia_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!ppo)
      return EBADPARM;

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
   if(idReq == AEECLSID_APPRESCTL){
     *ppo = pme->m_pResCtl;
      IAPPRESCTL_AddRef(*ppo);
      return SUCCESS;
   } 
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM

   if (NULL != IQI_SELF(po, idReq, ppo, pme->m_clsSelf)) 
   {
      IQI_AddRef(*ppo);
      return SUCCESS;
   }
   else if (AEECLSID_MEDIA == idReq)
   {
      *ppo = pme;
      IQI_AddRef(*ppo);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

/*==================================================================

==================================================================*/
int AEEMedia_RegisterNotify(IMedia * po, PFNMEDIANOTIFY pfnNotify, void * pUser)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!AEEMedia_IsReadyState(pme))
      return EBADSTATE;

   pme->m_pfnNotify = pfnNotify;
   pme->m_pUser = pUser;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEMedia_SetMediaParm(IMedia * po, int nParamID, int32 p1, int32 p2)
{
   AEEMedia *  pme = (AEEMedia *)po;
   int         nRet = EBADSTATE;

   if (AEEMedia_IsIdleState(pme))
   {
         // Processed in IDLE state only ...

      switch (nParamID)
      {
         case MM_PARM_MEDIA_DATA:
            nRet = AEEMedia_SetMediaData(pme, (AEEMediaDataEx *)p1, (boolean)(0 != p2));
            if (nRet == SUCCESS)
               pme->m_nState = MM_STATE_READY;
            break;

         default:
            break;
      }
   }

   return nRet;
}

/*==================================================================

==================================================================*/
int AEEMedia_GetMediaParm(IMedia * po, int nParamID, int32 * pP1, int32 * pP2)
{
   AEEMedia * pme = (AEEMedia *)po;
   int         nRet = SUCCESS;

   if (!AEEMedia_IsIdleState(pme))
   {
      switch (nParamID)
      {
         case MM_PARM_MEDIA_DATA:
            if (pP1)
               MEMCPY(pP1, &pme->m_mdInternal, sizeof(AEEMediaData));
            break;
       
        case MM_PARM_CLSID:
            if (pP1)
               *pP1 = (AEECLSID)pme->m_clsSelf;
            break;

         default:
            nRet = EBADSTATE;
            break;
      }
   }
   else
   {
      if (pP1 && MM_PARM_CLSID == nParamID)
         *pP1 = (AEECLSID)pme->m_clsSelf;
      else
         nRet = EBADSTATE;
   }

   return nRet;
}

/*==================================================================

==================================================================*/
int AEEMedia_Play(IMedia * po)
{
   AEEMedia *           pme = (AEEMedia *)po;
   int                  nRet = SUCCESS;

   if (!AEEMedia_IsPlayOK(pme))
      return EBADSTATE;

#if defined (FEATURE_ACM)
   nRet =  AEEMedia_RequestResource(pme, TRUE);
   if(nRet != SUCCESS && nRet != MM_PENDING)
   {
     return nRet;
   }
#endif // FEATURE_ACM

   pme->m_bStateChanging = TRUE;

   return nRet;
}

/*==================================================================

==================================================================*/
int AEEMedia_Record(IMedia * po)
{
   AEEMedia *           pme = (AEEMedia *)po;
   int                  nRet = SUCCESS;

   if (!AEEMedia_IsRecOK(pme))
      return EBADSTATE;

   // Read-only streams are not allowed in recording.
   // Only MMD_FILE_NAME and MMD_BUFFER are valid.
   if (pme->m_md.clsData >= MMD_ISOURCE)
      return MM_EBADMEDIADATA;

   //Fixed files are not allowed to be written to, fail here.
   if(pme->m_mdInternal.clsData == MMD_FILE_NAME)
   {
      IFileMgr * pFileMgr = NULL;
      IFile * pFile = NULL;

      if (SUCCESS == ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_FILEMGR,
         (void **)&pFileMgr))
      {
         if(NULL != pFileMgr)
         {
            pFile = IFILEMGR_OpenFile(pFileMgr, (char *) pme->m_mdInternal.pData,
               _OFM_READ);
            if(NULL != pFile)
            {
               FileInfo fi;
               if(SUCCESS == IFILE_GetInfo(pFile, &fi))
               {
                  if(fi.attrib & AEE_FA_FIXED)
                  {
                     IFILE_Release(pFile);
                     IFILEMGR_Release(pFileMgr);
                     return MM_EBADMEDIADATA;
                  }

               }
               IFILE_Release(pFile);
            }
            IFILEMGR_Release(pFileMgr);
         }
      }
   }

#if defined (FEATURE_ACM)
   nRet =  AEEMedia_RequestResource(pme, FALSE);
   if(nRet != SUCCESS && nRet != MM_PENDING)
   {
      return nRet;
   }
#endif // FEATURE_ACM

   MEMCPY(&pme->m_md, &pme->m_mdInternal, sizeof(AEEMediaData));

   pme->m_bStateChanging = TRUE;

   return nRet;
}

/*==================================================================

==================================================================*/
int AEEMedia_Stop(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!AEEMedia_IsStopOK(pme))
      return EBADSTATE;

#ifdef FEATURE_ACM   
   if(pme->m_bResOwner == FALSE)
   {
      return EBADSTATE;
   }
#endif

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEMedia_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!AEEMedia_IsSeekOK(pme))
      return EBADSTATE;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEMedia_Pause(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!AEEMedia_IsPauseOK(pme))
      return EBADSTATE;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEMedia_Resume(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!AEEMedia_IsResumeOK(pme))
      return EBADSTATE;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEMedia_GetTotalTime(IMedia * po)
{
   AEEMedia * pme = (AEEMedia *)po;

   if (!AEEMedia_IsGetTotalTimeOK(pme))
      return EBADSTATE;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
int AEEMedia_GetState(IMedia * po, boolean * pbStateChanging)
{
   AEEMedia *  pme = (AEEMedia *)po;

   if (pbStateChanging)
      *pbStateChanging = pme->m_bStateChanging;

   return pme->m_nState;
}

/*==================================================================
   Callback Notification Function: called in BREW context after
   AEE_SYS_RESUME()
==================================================================*/
void AEEMedia_CallbackNotify(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   int      nCmd;

   if (!pme || !pmcb)
      return;

   pmcb->bInUse = FALSE;

//r1   DBGPRINTF("(state, cmd, status): (%d %d %d)", pme->m_nState, pmcb->cmdNotify.nCmd, pmcb->cmdNotify.nStatus);

   // If the command is GetTotalTime()/SetMediaParm()/GetMediaParm(), make the callback to user
   // else do the state management.
   nCmd = pmcb->cmdNotify.nCmd;
   if (nCmd == MM_CMD_PLAY || nCmd == MM_CMD_RECORD)
   {
      if (AEEMedia_IsPlayCtlFail(pmcb->cmdNotify.nStatus))
      {
         AEEMedia_AppCallback(pme, pmcb);
      }
      else
      {
         // Perform state change...
         switch (pme->m_nState)
         {
            case MM_STATE_READY: AEEMedia_ReadySH(pme, pmcb); break;
            case MM_STATE_PLAY: AEEMedia_PlaySH(pme, pmcb); break;
            case MM_STATE_PLAY_PAUSE: AEEMedia_PlayPauseSH(pme, pmcb); break;
            case MM_STATE_RECORD: AEEMedia_RecordSH(pme, pmcb); break;
            case MM_STATE_RECORD_PAUSE: AEEMedia_RecordPauseSH(pme, pmcb); break;
            default: break;
         }
      } // else
   } // if (nCmd...)
   else
   {
      AEEMedia_AppCallback(pme, pmcb);
   }
}

static void AEEMedia_AppCallback(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   ACONTEXT *pacRestore = AEE_EnterAppContext(pme->m_pac);
   IMEDIA_AddRef((IMedia *)pme);
   if (pme->m_pfnNotify) {
      pme->m_pfnNotify(pme->m_pUser, &pmcb->cmdNotify);
   }
   IMEDIA_Release((IMedia *)pme);
   AEE_LeaveAppContext(pacRestore);
}

/*==================================================================

   LOCAL FUNCTIONS

==================================================================*/
//
// State handlers
//
static void AEEMedia_ReadySH(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   int   nCmd = pmcb->cmdNotify.nCmd;
   int   nStatus = pmcb->cmdNotify.nStatus;
   int   nState;

   if (nCmd != MM_CMD_PLAY && nCmd != MM_CMD_RECORD)
      return;

   switch (nStatus)
   {
      case MM_STATUS_START:
      {
         if (nCmd == MM_CMD_PLAY)
            nState = MM_STATE_PLAY;
         else
            nState = MM_STATE_RECORD;
         AEEMedia_SetState(pme, nState);
         break;
      }
      
      case MM_STATUS_ABORT:
      case MM_STATUS_DONE:
         AEEMedia_SetState(pme, MM_STATE_READY);
         break;

      default:
         break;
   }

   AEEMedia_AppCallback(pme, pmcb);
}

static void AEEMedia_PlaySH(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   int   nCmd = pmcb->cmdNotify.nCmd;
   int   nStatus = pmcb->cmdNotify.nStatus;

   if (nCmd != MM_CMD_PLAY)
      return;

   switch (nStatus)
   {
      case MM_STATUS_PAUSE:
         AEEMedia_SetState(pme, MM_STATE_PLAY_PAUSE);
         break;

      case MM_STATUS_MEDIA_SPEC:
      case MM_STATUS_TICK_UPDATE:
      case MM_STATUS_SEEK:
      case MM_STATUS_RESUME:
      case MM_STATUS_DATA_IO_DELAY:         
         break;
      
      case MM_STATUS_DONE:
      case MM_STATUS_ABORT:

#if defined (FEATURE_ACM)
         // if force release initiated stop command, send abort status back to app
         if (pme->m_bForceRelease) 
         {
            pmcb->cmdNotify.nStatus = MM_STATUS_ABORT;
            pme->m_bForceRelease = FALSE;
         }

         // The list of clsID that need to have midi out resource
         // See: CMediaMIDIOutQCP_SetMediaEnv
         if ((pme->m_clsSelf == AEECLSID_MEDIAPCM ||
              pme->m_clsSelf == AEECLSID_MEDIAADPCM ||
              pme->m_clsSelf == AEECLSID_MEDIAQCP || 
              pme->m_clsSelf == AEECLSID_MEDIAMIDIOUTQCP) &&
             (pme->m_bChannelShare == 1))
         {
            CMediaMIDIOutQCP_CleanupResource (pme);
         }
         else
         {
            AEEMedia_CleanUpResource(pme);
         }

#endif // FEATURE_ACM
         AEEMedia_SetState(pme, MM_STATE_READY);
         break;

      default:
         break;
   }

   AEEMedia_AppCallback(pme, pmcb);
}

static void AEEMedia_PlayPauseSH(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   int   nCmd = pmcb->cmdNotify.nCmd;
   int   nStatus = pmcb->cmdNotify.nStatus;

   if (nCmd != MM_CMD_PLAY)
      return;

   switch (nStatus)
   {
      case MM_STATUS_RESUME:
         AEEMedia_SetState(pme, MM_STATE_PLAY);
         break;
      
      case MM_STATUS_SEEK:
      case MM_STATUS_DATA_IO_DELAY:         
         break;

      case MM_STATUS_DONE:
      case MM_STATUS_ABORT:
#if defined (FEATURE_ACM)
         // if force release initiated stop command, send abort status back to app
         if (pme->m_bForceRelease) 
         {
           pmcb->cmdNotify.nStatus = MM_STATUS_ABORT;
           pme->m_bForceRelease = FALSE;
         }
         AEEMedia_CleanUpResource(pme);
#endif // FEATURE_ACM
         AEEMedia_SetState(pme, MM_STATE_READY);
         break;

      default:
         break;
   }

   AEEMedia_AppCallback(pme, pmcb);
}

static void AEEMedia_RecordSH(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   int   nCmd = pmcb->cmdNotify.nCmd;
   int   nStatus = pmcb->cmdNotify.nStatus;

   if (nCmd != MM_CMD_RECORD)
      return;

   switch (nStatus)
   {
      case MM_STATUS_PAUSE:
         AEEMedia_SetState(pme, MM_STATE_RECORD_PAUSE);
         break;
      
      case MM_STATUS_MEDIA_SPEC:
      case MM_STATUS_TICK_UPDATE:
      case MM_STATUS_SEEK:
      case MM_STATUS_RESUME:
      case MM_STATUS_DATA_IO_DELAY:         
      case MM_STATUS_SPACE_WARNING:
         break;

      case MM_STATUS_DONE:
      case MM_STATUS_ABORT:
      case MM_STATUS_SPACE_ERROR:
#if defined (FEATURE_ACM)
         if ((nStatus == MM_STATUS_DONE) && pme->m_bForceRelease)
         {
            // if force release initiated stop, send abort status back to app
            pmcb->cmdNotify.nStatus = MM_STATUS_ABORT;
            pme->m_bForceRelease = FALSE;
         }
         AEEMedia_CleanUpResource(pme);
#endif // FEATURE_ACM
         AEEMedia_SetState(pme, MM_STATE_READY);
         break;

      default:
         break;
   }

   AEEMedia_AppCallback(pme, pmcb);
}

static void AEEMedia_RecordPauseSH(AEEMedia * pme, AEEMediaCallback * pmcb)
{
   int   nCmd = pmcb->cmdNotify.nCmd;
   int   nStatus = pmcb->cmdNotify.nStatus;

   if (nCmd != MM_CMD_RECORD)
      return;

   switch (nStatus)
   {
      case MM_STATUS_RESUME:
         AEEMedia_SetState(pme, MM_STATE_RECORD);
         break;
      
      case MM_STATUS_TICK_UPDATE:
      case MM_STATUS_SEEK:
      case MM_STATUS_DATA_IO_DELAY:         
      case MM_STATUS_SPACE_WARNING:
         break;

      case MM_STATUS_DONE:
      case MM_STATUS_ABORT:
      case MM_STATUS_SPACE_ERROR:
#if defined (FEATURE_ACM)
         // if force release initiated stop command, send abort status back to app
         if (pme->m_bForceRelease) 
         {
            pmcb->cmdNotify.nStatus = MM_STATUS_ABORT;
            pme->m_bForceRelease = FALSE;
         }
         AEEMedia_CleanUpResource(pme);
#endif // FEATURE_ACM
         AEEMedia_SetState(pme, MM_STATE_READY);
         break;

      default:
         break;
   }

   AEEMedia_AppCallback(pme, pmcb);
}

static int AEEMedia_SetMediaData(AEEMedia * pme, AEEMediaDataEx * pMediaData, boolean bEx)
{
   int               nRet = SUCCESS;
   AEEMediaDataEx    md;
   AEEMediaDataEx *  pmd = &md;
   int               nSize = bEx ? sizeof(AEEMediaDataEx) : sizeof(AEEMediaData);

   if (!pMediaData || !pMediaData->pData)
      return EBADPARM;

   if (bEx && pMediaData->dwStructSize != (uint32)nSize)
      return EBADPARM;

   MEMCPY(pmd, pMediaData, nSize);

   if (pmd->clsData == MMD_FILE_NAME)
   {
       if (!*(char *)(pmd->pData) )
          nRet = EBADPARM;
       else if (!AEEMedia_UrlPath(pme, pmd->pData))
       {
          if (AEEMedia_FilePath(pme, pmd->pData))
            pmd->pData = pme->m_pszFileAppPath;
          else
          nRet = EBADPARM;
   }
   }
   else if (pmd->clsData == MMD_BUFFER)
   {
      if (pmd->dwSize == 0)
         nRet = EBADPARM;
   }
   else
   {
      // Assume pmd->clsData is ISource type
      ISource *   ps;
      nRet = ISOURCE_QueryInterface((ISource *)pmd->pData, AEECLSID_SOURCE, (void **)&ps);
      if (nRet == SUCCESS)
      {
         if (pmd->dwSize == 0)
            pmd->dwSize = 0xFFFFFFFF;
      }
   }

   if (nRet == SUCCESS)
   {
      MEMCPY(&pme->m_md, pmd, sizeof(AEEMediaData));

      //Check for const files
      MEMCPY(&pme->m_mdInternal, pmd, sizeof(AEEMediaData));
      if(pme->m_md.clsData == MMD_FILE_NAME)
      {
         IFileMgr * pFileMgr = NULL;
         IFile * pFile = NULL;

         if (SUCCESS == ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_FILEMGR,
                                              (void **)&pFileMgr))
         {
            if(NULL != pFileMgr)
            {
               pFile = IFILEMGR_OpenFile(pFileMgr, (char *) pme->m_md.pData,
                  _OFM_READ);
               if(NULL != pFile)
               {
                  void * pMem = NULL;
                  pMem = IFILE_Map(pFile, NULL, 0, AEE_FMAP_PROT_READ,
                     AEE_FMAP_SHARED, 0);
                  if(NULL != pMem)
                  {
                     FileInfo fi;
                     if(SUCCESS == IFILE_GetInfo(pFile, &fi))
                     {
                        //We have a mapping use a buffer instead
                        pme->m_md.clsData = MMD_BUFFER;
                        pme->m_md.pData = pMem;
                        pme->m_md.dwSize = fi.dwSize;
                     }
                  }
                  IFILE_Release(pFile);
               }
               IFILEMGR_Release(pFileMgr);
            }
         }
      }
      if (bEx)
      {
         pme->m_bRaw = pmd->bRaw;
         pme->m_dwCapsMD = pmd->dwCaps;
         pme->m_dwBufferSize = pmd->dwBufferSize;
         if (pmd->pSpec && pmd->dwSpecSize)
         {
            pme->m_pSpec = MALLOC(pmd->dwSpecSize);
            if (pme->m_pSpec)
            {
               MEMCPY(pme->m_pSpec, pmd->pSpec, pmd->dwSpecSize);
               pme->m_dwSpecSize = pmd->dwSpecSize;
            }
            else
               return ENOMEMORY;
         }
      }
   }

   return nRet;
}

static boolean AEEMedia_FilePath(AEEMedia * pme, void * pData)
{
   const char *cpszFile = (const char *)pData;
   char *      pszFS = 0;
   int         nLen;
   int         bRet = FALSE;
   char *      pszTempFileAppPath = pme->m_pszFileAppPath; // Save the ptr

   if (!cpszFile) 
      goto Done;

   FREEIF(pme->m_pszFile);

   // Get resolved fs:/ path length
   if (SUCCESS != AEE_ResolvePath(cpszFile, NULL, &nLen))
      goto Done;

   pszFS = MALLOC(nLen);
   if (!pszFS)
      goto Done;

   // Resolve path to fs:/
   if (SUCCESS != AEE_ResolvePath(cpszFile, pszFS, &nLen))
      goto Done;

   // Check for access
   if (SUCCESS != AEE_CheckPathAccess(pszFS, AEEFP_READ,0)) 
      goto Done;

   /* measure native path */
   if (SUCCESS != OEMFS_GetNativePath(pszFS,0, &nLen))
      goto Done;

   pme->m_pszFile = MALLOC(nLen);
   if (!pme->m_pszFile) 
      goto Done;

   /* get native path */   
   if (SUCCESS != OEMFS_GetNativePath(pszFS,pme->m_pszFile,&nLen))
      goto Done;

   pme->m_pszFileAppPath = STRDUP(cpszFile); // Make a new copy
   FREEIF(pszTempFileAppPath); // Free the old copy
   

   bRet = TRUE;

Done:
   FREEIF(pszFS);
   return bRet;
}

// This is intended to be used to recognize any valid url, 
static boolean AEEMedia_UrlPath(AEEMedia * pme, void * pData)
{
   if( pData && (STRNICMP((char *)pData, "rtsp://", 7 ) == 0 ))
   { 
      FREEIF(pme->m_pszFile);
      pme->m_pszFile = STRDUP((char *)pData);
      if (!pme->m_pszFile)
         return FALSE;
      else
         return TRUE;
   }

   return FALSE;
}

/*===========================================================================

                      OEMMedia Format Detection Functions

===========================================================================*/
#define OEMMM_FMT_BYTES       12
#define OEMMM_SMAF_BYTES      18
#define OEMMM_FMT_BYTES_MAX   18

#define FMT_MIDI_MTHD      "MThd"
#define FMT_SMAF_MTHD      "MMMD"
#define FMT_PMD_CMID       "cmid"
#define FMT_PMD_MELO       "melo"
#define FMT_PMD_WASF       "wasf"
#define FMT_QCP_RIFF       "RIFF"
#define FMT_QCP_QLCM       "QLCM"
#define FMT_IMELODY_BEGIN  "BEGIN:IMELODY"
#define FMT_AMR_BEGIN      "#!AMR"
#define FMT_AMRWB_BEGIN    "#!AMR-WB"
#define FMT_MP4_MARK       "ftyp"
#define FMT_ADPCM_MARK     "WAVE"
#define FMT_XMF_MARK       "XMF_"
#define FMT_DLS_MARK       "DLS "
#define FMT_RM_MARK        ".RMF"
#define FMT_EVRCNB_BEGIN   "#!EVRC-B"

// 16 bytes, grouped by uint32s
#define FMT_WMA_4BYTES1     0x75B22630
#define FMT_WMA_4BYTES2     0x11CF668E
#define FMT_WMA_4BYTES3     0xAA00D9A6
#define FMT_WMA_4BYTES4     0x6CCE6200


#define OEMMEDIA_IS_SYNC(x) \
  (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF0) == 0xF0)

#define OEMMEDIA_IS_ID3V2_TAG(x) \
  (((byte *)(x))[0] == 0x49 && ((byte *)(x))[1] == 0x44 && \
                                                      ((byte *)(x))[2] == 0x33)
#define OEMMEDIA_AAC_IS_SYNC(x) \
     (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF6) == 0xF0)

#define OEMMEDIA_AAC_IS_ADIF(x) \
   (\
      (x)[0] == 'A' && \
      (x)[1] == 'D' && \
      (x)[2] == 'I' && \
      (x)[3] == 'F'    \
   )
     
#define OEMMEDIA_DT_RETURN(pcpsz, mime) \
   { \
      if (pcpsz) \
         *pcpsz = mime; \
      return SUCCESS; \
   }

int OEMMedia_DetectType(const void * cpBuf, uint32 * pdwSize, const char * cpszName, const char ** pcpszMIME)
{
   byte *   cbBuf = (byte *)cpBuf;
   uint16   wLen;

   // If all are NULL except pdwSize, return the required number of bytes
   // to do type detection
   if (!cpBuf && !cpszName && !pcpszMIME && pdwSize)
   {
      *pdwSize = OEMMM_FMT_BYTES_MAX;
      return ENEEDMORE;
   }

   if (cpszName && !cpBuf)
      return EUNSUPPORTED;

   if (!cpBuf || !pdwSize)
      return EBADPARM;

   if (*pdwSize < OEMMM_FMT_BYTES)
   {
      *pdwSize = OEMMM_FMT_BYTES - *pdwSize;
      return ENEEDMORE;
   }

   // MIDI: 
   // Bytes 0-3 = 4d 54 68 64 ("MThd")
   // Bytes 4-5 = 00 00 / 00 01 (Format 0 or Format 1)
   {
      wLen = 4;
      if (!MEMCMP(cbBuf, FMT_MIDI_MTHD, wLen))
      {
         if (cbBuf[wLen] == 0x00 && (cbBuf[wLen+1] == 0x00 || cbBuf[wLen+1] == 0x01))
            OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_MIDI);
      }
   }
   // SMAF: 
   // Bytes 0-3 = 4d 4d 4d 44 ("MMMD")
   // Bytes 17  = if byte 17 == 0xf0 (SPF) otherwise (MMF)
   {
      wLen = 4;

      if (!MEMCMP(cbBuf, FMT_SMAF_MTHD, wLen))
      {
         if (*pdwSize < OEMMM_SMAF_BYTES)
         {
            *pdwSize = OEMMM_SMAF_BYTES - *pdwSize;
            return ENEEDMORE;
         }
         if (cbBuf[17] == 0xf0 ){
            OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_PHR);
         } else {
            OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_MMF);
         }
      }
   }

   // PMD (WebAudio): 
   // Bytes 0-3 = "cmid" / "melo" / "wasf"
   {
      wLen = 4;
      if (!MEMCMP(cbBuf, FMT_PMD_CMID, wLen) || 
          !MEMCMP(cbBuf, FMT_PMD_WASF, wLen))
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, MT_VIDEO_PMD);
      }
      else if(!MEMCMP(cbBuf, FMT_PMD_MELO, wLen)) 
      { 
         OEMMEDIA_DT_RETURN(pcpszMIME, "video/mld"); 
      } 
   }

   // QCP (PureVoice):
   // Bytes 0-3 = "RIFF"
   // Bytes 4-7 = Length of chunck - 8 (not checked)
   // Bytes 8-11 = "QLCM"
   {
      wLen = 4;
      if (!MEMCMP(cbBuf, FMT_QCP_RIFF, wLen) && !MEMCMP(cbBuf+8, FMT_QCP_QLCM, wLen))
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_QCP);
      }
   }

   // AAC
   {
      if (OEMMEDIA_AAC_IS_SYNC(cbBuf) || OEMMEDIA_AAC_IS_ADIF(cbBuf)|| OEMMEDIA_AAC_IS_RAW(cbBuf)
		  #if defined(FEATURE_BREW_DETECT_AAC)
         || (OEMMedia_DetectTypeAACEx(cbBuf, pdwSize)==SUCCESS)
		 #endif
		)
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_AAC);
      }
   }

   // MP3
   {
      if (OEMMEDIA_IS_SYNC(cbBuf) || OEMMEDIA_IS_ID3V2_TAG(cbBuf)
#if defined(FEATURE_BREW_DETECT_MP3)
          || (OEMMedia_DetectTypeMP3Ex(cbBuf, pdwSize)==SUCCESS)
#endif
          )
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_MP3);
      }
   }

   // IMELODY:
   // Bytes 0-11 = ("BEGIN:IMELODY")
   {
     wLen = 12;
     if (!MEMCMP(cbBuf, FMT_IMELODY_BEGIN, wLen))
     {
       OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_IMELODY);
     }
   }

   // AMR-WB
   // Bytes 0 - 7
   {
     wLen = 8;
     if (!MEMCMP(cbBuf, FMT_AMRWB_BEGIN, wLen))
     {
       OEMMEDIA_DT_RETURN(pcpszMIME, "audio/awb");
     }
   }

   // AMR
   // Bytes 0 - 4
   {
     wLen = 5;
     if (!MEMCMP(cbBuf, FMT_AMR_BEGIN, wLen))
     {
       OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_AMR);
     }
   }

   // EVRC-NB
   // Bytes 0 - 7: ("#!EVRC-B")
   {
     wLen = 8;
     if (!MEMCMP(cbBuf, FMT_EVRCNB_BEGIN, wLen))
     {
       OEMMEDIA_DT_RETURN(pcpszMIME, "audio/evb");
     }
   }

   // Windows Media: WMA/WMV/ASF
   // Bytes 0 - 15: {0x30, 0x26, 0xb2, 0x75, 0x8e, 0x66, 0xcf, 0x11, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c}
   {
      uint32   nCompare = FMT_WMA_4BYTES1;
      wLen = 4;
      if (!MEMCMP((void *)cbBuf, (void *)&nCompare, wLen)){
         nCompare = FMT_WMA_4BYTES2;
         if(!MEMCMP((void *)(cbBuf+4), (void *)&nCompare, wLen)){
            nCompare = FMT_WMA_4BYTES3;
            if(!MEMCMP((void *)(cbBuf+8), (void *)&nCompare, wLen)){
               nCompare = FMT_WMA_4BYTES4;
               if(!MEMCMP((void *)(cbBuf+12), (void *)&nCompare, wLen)){
                  OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_WMA);
               }
            }
         }
      }
   }
   
   // MP4
   // Bytes 4-8 = "ftyp"
   {
     wLen = 4;
     if (!MEMCMP(cbBuf+4, FMT_MP4_MARK, wLen))
     {
       OEMMEDIA_DT_RETURN(pcpszMIME, MT_VIDEO_MPEG4);
     }
   }

   // ADPCM WAV files
   // Bytes 0-3 = "RIFF"
   // Bytes 8-11 = "WAVE"
   {
     wLen = 4;
     if ( !MEMCMP(cbBuf, FMT_QCP_RIFF, wLen) && 
          !MEMCMP(cbBuf+8, FMT_ADPCM_MARK, wLen))
     {
       OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_ADPCM);
     }
   }

   // XMF: 
   // Bytes 0-3 = "XMF_"
   {
      wLen = 4;
      if (!MEMCMP(cbBuf, FMT_XMF_MARK, wLen))
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_XMF);
      }
   }

   // DLS: 
   // Bytes 0-3 = "DLS "
   {
      wLen = 4;
      if (!MEMCMP(cbBuf, FMT_DLS_MARK, wLen))
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, MT_AUDIO_DLS);
      }
   }

   // Real Media files
   // Bytes 0-3 = ".RMF"
   {
      wLen = 4;
      if ( !MEMCMP(cbBuf, FMT_RM_MARK, wLen))
      {
         OEMMEDIA_DT_RETURN(pcpszMIME, "video/rm");
      }
   }

   if (pcpszMIME)
      *pcpszMIME = "";
   return ENOTYPE;
}

#if defined (FEATURE_ACM)
/*==================================================================
   This function retrieves a specified number of bytes (nBytes) from 
   the media file, starting from the beginning of the media file and 
   put it in the provided buffer (buf).

   Assumption: AEEMedia_SetMediaData has been called prior to invoking
   this function.
   
   This routine is modelled after AEEMediaUtil_FindClass
   (IShell * ps, AEEMediaData * pmd, AEECLSID * pCls)

   Note that for ISource file, if IPEEK_Peek returns PEEK_WAIT, then
   it does not work properly.
==================================================================*/
int AEEMedia_GetMediaHeader (AEEMedia *pMedia, byte *buf, int nBytes)
{
   AEEMediaData*   pmd = &pMedia->m_md;
   
   // If it is file name, then read the file.
   if (pmd->clsData == MMD_FILE_NAME)
   {
      IFileMgr *  pfm;
      IFile *     pIFile;
      int         nRead;
      
      if (ISHELL_CreateInstance(pMedia->m_pIShell, AEECLSID_FILEMGR, (void **)&pfm))
         return EFAILED;

      pIFile = IFILEMGR_OpenFile(pfm, (char *)pmd->pData, _OFM_READ);
      if (!pIFile)
      {
         MM_RELEASEIF(pfm);
         return EFAILED;
      }

      nRead = IFILE_Read(pIFile, buf, nBytes);

      if (pIFile)
      {
         IFILE_Release (pIFile);
      }
      
      if (pfm)
      {
         IFILEMGR_Release (pfm);
      }

      if (nRead < nBytes)
      {
         return EFAILED;
      }
      else
      {
         return SUCCESS;
      }
   } 
   else if (pmd->clsData == MMD_BUFFER)
   {
      if (pmd->dwSize >= nBytes)
      {
         MEMCPY (buf, pmd->pData, nBytes);
         return SUCCESS;
      }
      else
      {
         return EFAILED;
      }
   }
   else if (pmd->clsData >= MMD_ISOURCE)
   {
      ISource *   pSource = (ISource *)pmd->pData;
      IPeek *     pPeek;
      char *      pPeekedBuf;
      int32       lAvailable;
      int32       lCode;

      // Check if ISource is IPeek
      if (SUCCESS != ISOURCE_QueryInterface(pSource, AEECLSID_PEEK, (void **)&pPeek) || !pPeek)
         return EFAILED;
      
      // Peek to see if some data is available
      pPeekedBuf = IPEEK_Peek(pPeek, 0, &lAvailable, &lCode);
      if (lCode == IPEEK_END || lCode == IPEEK_ERROR || lCode == IPEEK_FULL)
      {
         IPEEK_Release (pPeek);
         return EFAILED;
      }

      // Peek till required data is fetched
      while (lAvailable < (long) nBytes)
      {
         pPeekedBuf = IPEEK_Peek(pPeek, nBytes - lAvailable, &lAvailable, &lCode);
         if (lCode == IPEEK_END || lCode == IPEEK_ERROR || lCode == IPEEK_FULL)
         {
            IPEEK_Release (pPeek);
            return EFAILED;
         }

         // This function is synchronous, have to return ERROR in this case
         if (lCode == IPEEK_WAIT)
         {
            IPEEK_Release (pPeek);
            return EFAILED;
         }
      }

      if (!pPeekedBuf)
      {
         IPEEK_Release (pPeek);
         return EFAILED;
      }

      MEMCPY(buf, pPeekedBuf, nBytes);

      if (pPeek)
      {
         IPEEK_Release (pPeek);
         return SUCCESS;
      }
   }
   return EFAILED;
}

/* should get reason code in here rather than derived media class because
 * we do acquire on this level.  This function gets the reason code
 * that we need to pass to ACM acquire.
 */
static uint32 AEEMedia_GetReasonCode(AEEMedia* po, ResourceCntlData * rdata, flg isPlay)
{
#if defined (FEATURE_ODM_UI)  
   uint32 reason = ACM_PLAY_AUDIO;
   flg isAudio = TRUE;

   if(po->m_clsSelf == AEECLSID_MEDIAMPEG4)
      isAudio = FALSE;

#if defined (FEATURE_REAL_AUDIO)
   else if(po->m_clsSelf == AEECLSID_MEDIAREAL)
      isAudio = FALSE;
#endif // defined (FEATURE_REAL_AUDIO)

   if(!isPlay)
   {
      reason = ACM_AUDIO_RECORD;
   }
   else if(rdata->nPlayType == MM_PLAY_TYPE_RINGER)
   {
      reason = (isAudio) ? ACM_ALERT_FILE_PLAY_BACK_AUDIO : ACM_ALERT_FILE_PLAY_BACK_VIDEO;
   }
   else if(rdata->nPlayType == MM_PLAY_TYPE_REMINDER)
   {
      reason = (isAudio) ? ACM_REMINDER_FILE_PLAY_BACK_AUDIO : ACM_REMINDER_FILE_PLAY_BACK_VIDEO;
   }
   else if(rdata->bChannelShare)  
   {
      reason = ACM_PLAY_MULTICHANNEL_AUDIO;
   }
   else
   {
      reason = (isAudio) ? ACM_PLAY_AUDIO : ACM_PLAY_VIDEO;
   }
   return reason;
#else // !FEATURE_ODM_UI
   uint32 reason = ACM_PLAY_MEDIA;

   if(!isPlay)
   {
      reason = ACM_AUDIO_RECORD;
   }
   else if ((po->m_clsSelf == AEECLSID_MEDIAADPCM) && !(rdata->bChannelShare))
   {
      // Special case for non-shared ADPCM:
      // ADPCM files with 32KHz sample rate are played as shutter sound
      // so we use a special reason code for them.

      // The sample rate for ADPCM file locates at 24-27 bytes in the file. 
      // Big endian format.
      byte headerBuf[28];
      uint16 numOfChannels;
      uint32  sampleRate;

      if (AEEMedia_GetMediaHeader (po, (byte*) headerBuf, 28) == SUCCESS)
      {
         numOfChannels = (headerBuf[23] << 8) + headerBuf[22];
         sampleRate = (headerBuf[27] << 24) + (headerBuf[26] << 16) + 
                      (headerBuf[25] << 8)  + (headerBuf[24]);
         if (numOfChannels == 1 && sampleRate == 32000)
         {
            reason = ACM_PLAY_SHUTTER;
         }
      }
   }

   return reason;
#endif // FEATURE_ODM_UI
}

/* AEEMedia_AssignResourceCntlData gets the data from the
 * derived class.  We need the play type, multichannel
 * flag and acm Callback. 
 * This design was proposed by Aditya.
 */
static int AEEMedia_AssignResourceCntlData(AEEMedia * po, ResourceCntlData * rdata, flg isPlay)
{
   if(AEECLSID_MEDIAMP3 == po->m_clsSelf ||
      AEECLSID_MEDIAMIDI == po->m_clsSelf ||
      AEECLSID_MEDIAIMELODY == po->m_clsSelf ||
      (AEECLSID_MEDIAADPCM == po->m_clsSelf && po->m_bChannelShare == 0) ||
      AEECLSID_MEDIAQCP == po->m_clsSelf ||
      AEECLSID_MEDIAAMR == po->m_clsSelf ||
      AEECLSID_MEDIAAAC == po->m_clsSelf ||
      AEECLSID_MEDIAXMF == po->m_clsSelf ||
      AEECLSID_MEDIADLS == po->m_clsSelf ||
      AEECLSID_MEDIASVG == po->m_clsSelf ||
      AEECLSID_MEDIAMMF == po->m_clsSelf ||
      AEECLSID_MEDIAPHR == po->m_clsSelf ||
      AEECLSID_MEDIAPCM == po->m_clsSelf)
   {
      // private functions
      CMediaBg_GetResourceCntlData(po, &(rdata->nPlayType), &(rdata->bChannelShare));
      CMediaBg_SetResourceCntlCBData(&(rdata->nAcmCB), isPlay);
   }
   else if(AEECLSID_MEDIAPMD == po->m_clsSelf )
   {
      CMediaPMD_GetResourceCntlData(po, &(rdata->nAcmCB));
   }
#if defined (FEATURE_APP_MPEG4)
   else if(AEECLSID_MEDIAMPEG4 == po->m_clsSelf )
   {
      // private functions
      OEMMediaMPEG4_GetResourceCntlData(po, &(rdata->nPlayType), &(rdata->nAcmCB));
   }
#endif // FEATURE_APP_MPEG4
   else if(AEECLSID_MEDIAMIDIOUTMSG == po->m_clsSelf)    
   {                              
      CMediaMIDIOutMsg_GetResourceCntlData(po, &(rdata->nPlayType), &(rdata->nAcmCB));
   }
   else if((AEECLSID_MEDIAADPCM == po->m_clsSelf && po->m_bChannelShare == 1) ||
           (AEECLSID_MEDIAMIDIOUTQCP == po->m_clsSelf))
   {
      CMediaMIDIOutQCP_GetResourceCntlData(po, 
                                           &(rdata->nPlayType), 
                                           &(rdata->bChannelShare), 
                                           &(rdata->nAcmCB));
   }
#ifdef FEATURE_REAL_AUDIO
   else if(AEECLSID_MEDIAREAL == po->m_clsSelf)
   {
      CMediaReal_GetResourceCntlData(po, &(rdata->nPlayType), &(rdata->nAcmCB));
   }
#endif
   else
   {
      // No ResourceCntl needed for some types like SDP-voice over ip, multiple file.
      return MM_NOACTION;
   }

   return SUCCESS;
}

void AEEMedia_CleanUpResource(AEEMedia *po)
{
#if defined (FEATURE_ODM_UI) 
   if(po->m_pResCtl)
   {   
      IAPPRESCTL_Free(po->m_pResCtl, NULL);
      po->m_bResOwner = FALSE;
      po->m_bForceRelease = FALSE;
   }
#else // non FEATURE_ODM_UI
   OEMACM_PriorityMgr_Free (po);
   po->m_bResOwner = FALSE;
   po->m_bForceRelease = FALSE;
#endif // FEATURE_ODM_UI
}

/************************************************************
 * AEEMedia_RequestResource - the function that does all the ACM
 * interaction and calls functions to get data needed for ACM calls.
 **********************************************************/
int AEEMedia_RequestResource(AEEMedia * pme, flg isPlay)
{
   int                      nRet = SUCCESS; 
   uint32                   reasonCode;
#if defined (FEATURE_ODM_UI) 
   uint32                   retval = RESCTL_STATUS_BUSY;
#endif // FEATURE_ODM_UI
   ACM_resource_state_type  pdwstatus = ACM_RESOURCE_UNKNOWN;
   ResourceCntlData         rdata;

   rdata.bChannelShare = FALSE;
   rdata.nPlayType = MM_PLAY_TYPE_NORMAL;

   rdata.nAcmCB = 0;

   if(AEEMedia_AssignResourceCntlData(pme, &rdata, isPlay) == MM_NOACTION)
   {
      // no Resource Cntl was needed for this class so we are finished.
      return SUCCESS;
   }

   reasonCode = AEEMedia_GetReasonCode(pme, &rdata, isPlay);
#if defined (FEATURE_ODM_UI) 
   nRet = IAPPRESCTL_Acquire(pme->m_pResCtl, reasonCode, &retval);

   if(nRet == SUCCESS)
   {
      if( retval == RESCTL_STATUS_BUSY )
      {
         return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
      }
      else
      {
         // RESCTL_STATUS_ACQUIRED
         pme->m_mAcmCB.pdata = pme;  // need to give pointer to self for callback data
         pme->m_mAcmCB.state = ACM_RESOURCE_UNKNOWN;
         nRet = (int)OEMACM_StateMgr_OnStatusChangeNotify(pme->m_pResCtl, (PFNNOTIFY)rdata.nAcmCB, &(pme->m_mAcmCB));

         if(nRet == SUCCESS)
         {
            if((nRet = (int)OEMACM_StateMgr_GetResourceState(pme->m_pResCtl,&pdwstatus)) == SUCCESS)
            {
               if(pdwstatus == ACM_RESOURCE_ACQUIRED)
               {
                  pme->m_bResOwner = TRUE;         
               }
               else
               {
                  // Delayed Acquire
                  return MM_PENDING;
               }
            }
         }
      }
   }
#else // non FEATURE_ODM_UI
   nRet = OEMACM_PriorityMgr_AcquireEx (pme, 
                                        reasonCode, 
                                        pme->m_clsSelf,
                                        rdata.bChannelShare,
                                        &pdwstatus);

   if(nRet == SUCCESS)
   {
      if( pdwstatus == ACM_RESOURCE_BUSY )
      {
         return AEERESCTL_ERROR_RESOURCE_NOT_GRANTED;
      }
      else
      {
         // ACM_RESOURCE_ACQUIRED or ACM_RESOURCE_ACQUIRING
         pme->m_mAcmCB.pdata = pme;  // need to give pointer to self for callback data
         pme->m_mAcmCB.state = ACM_RESOURCE_UNKNOWN;
         nRet = (int)OEMACM_StateMgr_OnStatusChangeNotify (pme, 
                                                           (PFNNOTIFY)rdata.nAcmCB, 
                                                           &(pme->m_mAcmCB));

         if(nRet == SUCCESS)
         {
            if(pdwstatus == ACM_RESOURCE_ACQUIRED)
            {
               pme->m_bResOwner = TRUE;         
            }
            else
            {
               // Delayed Acquire
               return MM_PENDING;
            }
         }
         else
         {        
            OEMACM_PriorityMgr_Free (pme) ;
         }
      }
   }
#endif // FEATURE_ODM_UI
   return nRet;
}
#endif // FEATURE_ACM
#endif // FEATURE_BREW_MULTIMEDIA


