/*=============================================================================
FILE:  AEEBTAVRCP_Metadata.c

SERVICES:  Implementation of AEEAVRCPTG_Priv_Ext

GENERAL DESCRIPTION:
   Implements the AEEAVRCPTG_Priv_Ext interface, tied to Bluetooth AVRCP,
   the IBTAVRCPTarget and IRemoteControl_Targetimplementation.

===============================================================================
        Copyright (c) 2008-2009 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary

=============================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-09-16    hs   Fixed the compiler warnings.

 =========================================================================== */

#include "OEMFeatures.h"
#include "AEEStdLib.h"

#if defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV)

#ifdef FEATURE_BT_AVRCP_13

#include "AEEStdErr.h"
#include "AEE_OEM.h"
#include "AEE_OEMHeap.h"
#include "AEE_OEMDispatch.h"

#include "AEEIRemoteControl_Target.h"
#include "AEEAVRCPTG_Priv_Ext.h" 
#include "AEEAVRCPTG_priv_ext_def.h"
#include "AEEAVRCPTG_Priv_Ext.bid"

#include "OEMBTAVRCPTGExt_Def.h" 
#include "AEECriticalSection.h"
#include "AEEISignal.h"
#include "AEEISignalCtl.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalCBFactory.h"
#ifndef BT_SIMULATION
#include "vbatt.h" //battery notifucations required in case of target 
#endif 

#include "AEE_OEMComDef.h"
#include "AEEBTUtils.h"

/*======================================================================= 
   Defines
=======================================================================*/


/* Maximum number of outstanding AVRCP metadata commands at a time */
#define AVRCP_METADATA_MAX_PENDING_CMD 16
#define AVRCPTG_BTSIG_REGD_COMPID      0x001958


//=============================================================================
//   AVRCP Metadata Object
//=============================================================================
struct AEEAVRCPTG_Priv_Ext
{
   const AEEAVRCPTG_Priv_ExtVtbl*      pvt;
   uint32                              uRef;
   IShell                              *pIShell;
   uint8                               notifTxn;
   AEECallback                         cbAppClose;
   AEEBTAVRCPTgMediaEventMask          mediaEventCapMask;
   AEEBTAVRCPTgMediaEventMask          pendingNotificationMask;
   uint32                              companyId[AEEBTAVRCPTG_NUM_OF_COMP_SUPPORTED];
   boolean                             startUpdate;
   uint32                              songPosUpdate;
   AEEBTAVRCPTgLastCmdInfo             lastCmdInfo;
   AEEBTAVRCPTgGrpNavStatus            navigationCommand;
   AEEBTAVRCPTgCharsetSuppByCt         charsetSuppByCt[AEEBTAVRCPTG_MAX_NUM_OF_VALS];
   AEEBTAVRCPTgSystemStatus            systemStatus;
   AEEBTAVRCPTgBatStatus               avrcpTgBatteryStatus;
   AEEBTAVRCPTgBatStatus               avrcpCtBatteryStatus;
   AEEBTAVRCPTgMediaTrackInfo          curMediaInfo;
   AEEBTAVRCPTgPlayerCurAttrValues     curPlayerAttrValues[AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED];
   AEEBTAVRCPTgPlayerCurAttrValSetByCt curPlayerAttrValByCt[AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED];
   AEEBTAVRCPTgAttributeInfo           playerInfo[AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED];
   AEEBTAVRCPTgPlaybackInfo            playbackInfo;
   OEMBTAVRCPTGExt                     *pOemObj;
   ISignal                             *piSigAeeEvt;
   AEECallback                         BREWEvCb;    
   AEEBTEvtQueue                       cmdAeeOemAvrcpExtTgQueue; 
   AEEBTEvtQueue                       cmdAppAvrcpExtTgQueue; 
};

//=============================================================================
//    Forward Function Declarations
//=============================================================================

static uint32 
AEEAVRCPTGPrivExt_AddRef(AEEAVRCPTG_Priv_Ext *me);

static uint32 
AEEAVRCPTGPrivExt_Release(AEEAVRCPTG_Priv_Ext *me);

static AEEResult 
AEEAVRCPTGPrivExt_QueryInterface(AEEAVRCPTG_Priv_Ext *me,
                                 AEEIID iid,
                                 void** ppo);

static AEEResult 
AEEAVRCPTGPrivExt_OnEventNotify(AEEAVRCPTG_Priv_Ext* _me, 
                                ISignal* piSig );

static AEEResult 
AEEAVRCPTGPrivExt_GetEvent(AEEAVRCPTG_Priv_Ext* _me, 
                           AEEBTAVRCPTgEventType* pEvent);

static AEEResult 
AEEAVRCPTGPrivExt_GetAttrConfigUpdates(AEEAVRCPTG_Priv_Ext* _me, 
                                       boolean* startUpdate, 
                                       uint32* songPosUpdateMs);

static AEEResult 
AEEAVRCPTGPrivExt_SetMediaTitle(AEEAVRCPTG_Priv_Ext* _me,
                                uint32 charset,
                                const uint8* title, 
                                int titleLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetArtistName(AEEAVRCPTG_Priv_Ext* _me, 
                                uint32 charset, 
                                const uint8* artistName, 
                                int artistNameLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetAlbumName(AEEAVRCPTG_Priv_Ext* _me, 
                               uint32 charset, 
                               const uint8* albumName, 
                               int albumNameLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetMediaTrackNumber(AEEAVRCPTG_Priv_Ext* _me, 
                                      const uint8* trackNumber, 
                                      int trackNumberLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetTotalTracks(AEEAVRCPTG_Priv_Ext* _me,
                                 const uint8* totalTracks, 
                                 int totalTracksLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetGenre(AEEAVRCPTG_Priv_Ext* _me, 
                           uint32 charset,
                           const uint8* genre, 
                           int genreLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetPlayingTimeMs(AEEAVRCPTG_Priv_Ext* _me,
                                   const uint8* playingTime, 
                                   int playingTimeLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetSongPositionMs(AEEAVRCPTG_Priv_Ext* _me, 
                                    uint32 curPosMs);

static AEEResult 
AEEAVRCPTGPrivExt_SetPlayStatus(AEEAVRCPTG_Priv_Ext* _me,
                                AEEBTAVRCPTgPlayStatus playStatus, 
                                uint32 curPosMs, 
                                const uint8* trackIndex,  
                                int trackIndexLen);

static AEEResult 
AEEAVRCPTGPrivExt_SetCapability(AEEAVRCPTG_Priv_Ext* _me,
                                AEEBTAVRCPTgMediaEventMask eventMask,
                                uint32 companyId);

static AEEResult 
AEEAVRCPTGPrivExt_GetControllerBatteryStatus(AEEAVRCPTG_Priv_Ext* _me,
                                             AEEBTAVRCPTgContBattStatus *batteryStatus);

static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrList(AEEAVRCPTG_Priv_Ext* _me, 
                                    const int* attributeids, 
                                    int attributeidsCount);

static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrValueList(AEEAVRCPTG_Priv_Ext* _me,
                                         uint32 attributeid,
                                         const int* attributeValues,
                                         int attributeValuesCount);

static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerCurrentAttrValue(AEEAVRCPTG_Priv_Ext* _me,
                                            uint32 attributeid,
                                            uint32 attributeValue);

static AEEResult 
AEEAVRCPTGPrivExt_GetPlayerCurrentAttrValue(AEEAVRCPTG_Priv_Ext* _me,
                                            uint32* attributeid,    
                                            uint32* attributeValue);

static AEEResult 
AEEAVRCPTGPrivExt_GetDisplayableCharacterSet(AEEAVRCPTG_Priv_Ext* _me, 
                                             uint32* charset);

static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrText(AEEAVRCPTG_Priv_Ext* _me,
                                    uint32 attributeid,
                                    uint32 charset,
                                    const uint8* attributeText,
                                    int attributeTextLen);
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrValueText(AEEAVRCPTG_Priv_Ext* _me,
                                         uint32 attributeid,
                                         uint32 attributeValue,
                                         uint32 charset,
                                         const uint8* attributeValueText,
                                         int attributeValueTextLen);

static AEEResult 
AEEAVRCPTGPrivExt_GetNavigateGroupCmd(AEEAVRCPTG_Priv_Ext* _me,
                                      AEEBTAVRCPTgGroupNavCommandEnum* navigationCommand);

static AEEResult 
AEEAVRCPTGPrivExt_SetLastCommandStatus(AEEAVRCPTG_Priv_Ext* _me,
                                       AEEBTAVRCPTgCmdResponseStatus status);


//=============================================================================
//    Local functions
//=============================================================================
static void 
AEEBTAVRCPTg_OEMEvtCallback(void* cbData,AEEBTAVRCPTgExtCmdInfo* );

static void AEEBTAVRCPTg_SysEvtCallback(void *cbData);
static void AEEBTAVRCPTg_CheckBattStatus( void *cb );

static void 
AEEBTAVRCPTg_CmdCapInd(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void 
AEEBTAVRCPTg_CmdListAttr(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdListAttrVal(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdGetAttrVal(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdSetAttrVal(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdAttrGetTxt(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdAttrGetValTxt(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdInformCharset(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdInformBattStatus(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdGetElementAttr(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdGetPlayStatus(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void
AEEBTAVRCPTg_CmdEvtNotif(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static void 
AEEBTAVRCPTg_CmdGrpNavigation(AEEAVRCPTG_Priv_Ext*, AEEBTAVRCPTgExtCmdInfo*);

static int AEEBTAVRCPTg_SendMetaReply(AEEAVRCPTG_Priv_Ext*, 
                                      AEEBTAVRCPTgExtCmdInfo*,
                                      AEEBTAVRCPTgPduId pduId , 
                                      AEEBTAVRCPTgCmdResponseStatus);

//=============================================================================
//    V-Table
//=============================================================================
static const AEEVTBL(AEEAVRCPTG_Priv_Ext) gpvtAVRCPMetadata_Target = 
{
   AEEAVRCPTGPrivExt_AddRef,
   AEEAVRCPTGPrivExt_Release,
   AEEAVRCPTGPrivExt_QueryInterface,
   AEEAVRCPTGPrivExt_OnEventNotify,
   AEEAVRCPTGPrivExt_GetEvent,
   AEEAVRCPTGPrivExt_GetAttrConfigUpdates,
   AEEAVRCPTGPrivExt_SetMediaTitle,
   AEEAVRCPTGPrivExt_SetArtistName,
   AEEAVRCPTGPrivExt_SetAlbumName,
   AEEAVRCPTGPrivExt_SetMediaTrackNumber,
   AEEAVRCPTGPrivExt_SetTotalTracks,
   AEEAVRCPTGPrivExt_SetGenre,
   AEEAVRCPTGPrivExt_SetPlayingTimeMs,  
   AEEAVRCPTGPrivExt_SetSongPositionMs,
   AEEAVRCPTGPrivExt_SetPlayStatus,
   AEEAVRCPTGPrivExt_SetCapability,
   AEEAVRCPTGPrivExt_GetControllerBatteryStatus,
   AEEAVRCPTGPrivExt_SetPlayerAttrList,
   AEEAVRCPTGPrivExt_SetPlayerAttrValueList,
   AEEAVRCPTGPrivExt_SetPlayerCurrentAttrValue,
   AEEAVRCPTGPrivExt_GetPlayerCurrentAttrValue,
   AEEAVRCPTGPrivExt_GetDisplayableCharacterSet,
   AEEAVRCPTGPrivExt_SetPlayerAttrText,
   AEEAVRCPTGPrivExt_SetPlayerAttrValueText,
   AEEAVRCPTGPrivExt_GetNavigateGroupCmd,
   AEEAVRCPTGPrivExt_SetLastCommandStatus
};

//=============================================================================
//    Global variables
//=============================================================================


//=============================================================================
//    Defines
//=============================================================================



//=============================================================================
//    Forward Function Declarations
//=============================================================================

//=============================================================================
//               AEEBTAVRCPTg_MemoryCleanUp
//
//=============================================================================
static void AEEBTAVRCPTg_MemoryCleanUp(AEEAVRCPTG_Priv_Ext *pAvrcpTg)
{ 
   uint32    i = 0;
   uint32    j = 0;

   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( NULL != pAvrcpTg->playerInfo[i].values )
      {
         for (j = 0; j < pAvrcpTg->playerInfo[i].numValues; j++ )
         {
            if ( NULL != pAvrcpTg->playerInfo[i].values[j].valueText )
            {
               FREE( pAvrcpTg->playerInfo[i].values[j].valueText );
               pAvrcpTg->playerInfo[i].values[j].valueText = NULL;
            }
         }
         FREE( pAvrcpTg->playerInfo[i].values );
         pAvrcpTg->playerInfo[i].values = NULL;
      }

      if ( NULL != pAvrcpTg->playerInfo[i].attrText )
      {
         FREE( pAvrcpTg->playerInfo[i].attrText );
         pAvrcpTg->playerInfo[i].attrText = NULL;
      }
   }

   if ( NULL != pAvrcpTg->curMediaInfo.albumName )
   {
      FREE(pAvrcpTg->curMediaInfo.albumName);
      pAvrcpTg->curMediaInfo.albumName = NULL;
   }

   if ( NULL != pAvrcpTg->curMediaInfo.playTime )
   {
      FREE(pAvrcpTg->curMediaInfo.playTime);
      pAvrcpTg->curMediaInfo.playTime = NULL;
   }

   if ( NULL != pAvrcpTg->curMediaInfo.trackNum )
   {
      FREE(pAvrcpTg->curMediaInfo.trackNum);
      pAvrcpTg->curMediaInfo.trackNum = NULL;
   }

   if ( NULL != pAvrcpTg->curMediaInfo.trackTotal )
   {
      FREE(pAvrcpTg->curMediaInfo.trackTotal );
      pAvrcpTg->curMediaInfo.trackTotal = NULL;
   }

   if ( NULL != pAvrcpTg->curMediaInfo.trackTitle )
   {
      FREE(pAvrcpTg->curMediaInfo.trackTitle);
      pAvrcpTg->curMediaInfo.trackTitle = NULL;
   }

   if ( NULL != pAvrcpTg->curMediaInfo.trackGenre )
   {
      FREE(pAvrcpTg->curMediaInfo.trackGenre);
      pAvrcpTg->curMediaInfo.trackGenre = NULL;
   }

   if ( NULL != pAvrcpTg->curMediaInfo.trackArtistName )
   {
      FREE(pAvrcpTg->curMediaInfo.trackArtistName);
      pAvrcpTg->curMediaInfo.albumName = NULL;
   }
}

//=============================================================================
//    AEEBTAVRCPMetadataTarget_Cleanup
//=============================================================================
int AEEBTAVRCPMetadataTarget_Cleanup(AEEAVRCPTG_Priv_Ext *me)
{
   int ret = AEE_SUCCESS;

   (void) AEE_CancelTimer (AEEBTAVRCPTg_CheckBattStatus, me);

   if ( me->pendingNotificationMask & AEEBTAVRCPTG_SYSTEM_STATUS_CHANGED )
   {
      me->systemStatus = AEEBTAVRCPTG_SYSTEM_UNPLUGGED;

      ret = OEMBTAVRCPTGExt_SendRegNotifReply(me->pOemObj,
                                              me->notifTxn,
                                              AEEBTAVRCPTG_SYSTEM_STATUS_CHANGED,  
                                              NULL,
                                              0,
                                              me->systemStatus,
                                              TRUE,
                                              NULL,
                                              0);
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdEvtNotif",0,0,0);
      }
      me->pendingNotificationMask = me->pendingNotificationMask & 
                                    (~AEEBTAVRCPTG_SYSTEM_STATUS_CHANGED);
   }

   AEEBTAVRCPTg_MemoryCleanUp( me );

   if (NULL != me->pOemObj)
   {
      MSG_HIGH("AEEBTAVRCP TG Ext: calling oem release... ",0,0,0);
      OEMBTAVRCPTGExt_Release(me->pOemObj); 
      me->pOemObj = NULL;
   }

   if (me->piSigAeeEvt)
   {
      ISignal_Release(me->piSigAeeEvt);
      me->piSigAeeEvt = NULL;
   }

   // free the event queues
   AEEBTEvtQ_Free(&me->cmdAppAvrcpExtTgQueue);
   AEEBTEvtQ_Free(&me->cmdAeeOemAvrcpExtTgQueue);
   CALLBACK_Cancel(&me->cbAppClose);
   RELEASEIF(me->pIShell);
   FREEIF(me);
   MSG_DEBUG("AEEBTAVRCP TG Ext: delete <- ",0,0,0);
   return ret;
}

//=============================================================================
//    AEEBTAVRCPMetadataTarget_New
//=============================================================================
int AEEBTAVRCPMetadataTarget_New(IShell* piShell, AEECLSID cls, void** ppif)
{
   AEEAVRCPTG_Priv_Ext          *me                = NULL;
   int                          ret                = AEE_SUCCESS;

   MSG_DEBUG("AEE AVRCP Metadata Target New ->",0,0,0);

   if (NULL == ppif)
   {
      MSG_ERROR("AEE AVRCP Metadata Target new ppif = NULL ",0,0,0);
      return AEE_EBADPARM;
   }
   // allocate AEE object
   me = MALLOCREC(AEEAVRCPTG_Priv_Ext);

   if (NULL == me)
   {
      MSG_ERROR("AEE AVRCP Metadata Target cannot allocate me",0,0,0);
      return AEE_ENOMEMORY;
   }

   MEMSET((void*)me, 0, sizeof(AEEAVRCPTG_Priv_Ext));

   // initialize the object
   me->pvt                     = &gpvtAVRCPMetadata_Target;
   me->uRef                    = 1;
   me->pIShell                 = piShell;
   me->mediaEventCapMask       = 0;
   me->pendingNotificationMask = 0;
   me->songPosUpdate           = AEEBTAVRCPTG_MEDIA_UPDATE_INTVL;
   me->startUpdate             = TRUE; /* Start updates by default */
   me->systemStatus            = AEEBTAVRCPTG_SYSTEM_POWER_ON;
   me->playbackInfo.playStatus = AEEAVRCPTG_PLAYSTAT_STOPPED;


   ret = AEEBTEvtQ_Init(&me->cmdAppAvrcpExtTgQueue,
                        AVRCP_METADATA_MAX_PENDING_CMD,
                        sizeof(AEEBTAVRCPTgEventType)); 

   if (AEE_SUCCESS != ret)
   {
      FREEIF(me);
      MSG_ERROR("AEE AVRCP Metadata Target Aee event queue can't be initialised",
                0,0,0);
      return AEE_EFAILED;
   }

   ret = AEEBTEvtQ_Init(&me->cmdAeeOemAvrcpExtTgQueue,
                        AVRCP_METADATA_MAX_PENDING_CMD,
                        sizeof(AEEBTAVRCPTgExtCmdInfo)); 

   if (AEE_SUCCESS != ret)
   {
      AEEBTEvtQ_Free(&me->cmdAppAvrcpExtTgQueue);
      FREEIF(me);
      MSG_ERROR("AEE AVRCP Metadata Target App event queue can't be initialised",
                0,0,0);
      return AEE_EFAILED;
   }

   ret = OEMBTAVRCPTGExt_Init(&me->pOemObj, 
                              (void *)me,
                              AEEBTAVRCPTg_OEMEvtCallback);

   if ( AEE_SUCCESS != ret )
   {
      MSG_DEBUG("AEEBTAVRCP TG:oem init error",0,0,0);
      AEEBTEvtQ_Free(&me->cmdAppAvrcpExtTgQueue);
      AEEBTEvtQ_Free(&me->cmdAeeOemAvrcpExtTgQueue);
      FREEIF(me);
      MSG_ERROR("AEE AVRCP Metadata Target App event queue can't be initialised",
                0,0,0);
      return AEE_EFAILED;
   }

   (void) AEE_SetSysTimer (AEEBTAVRCPTG_TG_BATT_STAT_UPDATE_INTVL, 
                           AEEBTAVRCPTg_CheckBattStatus, me);
   // Initialize the cb that will be called in case the app closes
   CALLBACK_Init(&me->cbAppClose, AEEBTAVRCPMetadataTarget_Cleanup, me); 
   AEE_LinkSysObject(&me->cbAppClose);
   ISHELL_AddRef(piShell);

   *ppif = (void *)me;

   MSG_DEBUG("AEEBTAVRCP TG Ext: new ret %x <-",ret,0,0);
   return ret;
}

//=============================================================================
//                           VTABLE FUNCTIONS
//
//=============================================================================

static uint32 AEEAVRCPTGPrivExt_AddRef(AEEAVRCPTG_Priv_Ext *me)
{
   // Increment reference count
   if (NULL != me)
   {
      return ++me->uRef;
   }
   return 0;
}

//=============================================================================
// AEEAVRCPTGPrivExt_Release
//
//=============================================================================
static uint32 AEEAVRCPTGPrivExt_Release(AEEAVRCPTG_Priv_Ext *me)
{
   uint32 uRefs;

   if (0 == me->uRef)
   {
      return me->uRef;
   }

   MSG_MED("AEEBTAVRCP TG Ext: rel - ref cnt = %x", me->uRef,0,0);

   uRefs = --me->uRef;

   if (0 == uRefs)
   {
      AEEBTAVRCPMetadataTarget_Cleanup( me );
   }
   return uRefs;
}

//=============================================================================
// AEEAVRCPTGPrivExt_QueryInterface
//
//=============================================================================
static int AEEAVRCPTGPrivExt_QueryInterface( AEEAVRCPTG_Priv_Ext *me,
                                             AEEIID iid,
                                             void** ppo )
{
   if (!AEE_CHECK_PTR("AEEBTAVRCP TG Ext: Query Interface", ppo, sizeof(void*), TRUE))
   {
      return AEE_EMEMPTR;
   }

   if ((AEEIID_IQI == iid) ||
       (AEEIID_AEEAVRCPTG_Priv_Ext == iid))
   {  /* AEEIID_AEEAVRCPTG_Priv_Ext */
      *ppo = (IQueryInterface*)me;
      IQI_AddRef (*ppo);
      return AEE_SUCCESS;
   }
   return AEE_ECLASSNOTSUPPORT;
}

//=============================================================================
// AEEAVRCPTGPrivExt_OnEventNotify
//
//=============================================================================

static AEEResult
AEEAVRCPTGPrivExt_OnEventNotify(AEEAVRCPTG_Priv_Ext *me,
                                ISignal *piSig)
{
   int ret = AEE_SUCCESS;

   MSG_DEBUG("AEEBTAVRCP TG Ext: On event notif -> ",0,0,0);

   if (NULL == piSig)
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: On event notify, bad sig %x ",piSig,0,0);
      ret = AEE_EBADPARM;
   } 
   else
   {
      if (me->piSigAeeEvt)
      {
         ISignal_Release(me->piSigAeeEvt);
         me->piSigAeeEvt = NULL;
      }

      me->piSigAeeEvt = piSig;
      ISignal_AddRef(piSig);
   }

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_GetEvent
//
//=============================================================================

static int
AEEAVRCPTGPrivExt_GetEvent(AEEAVRCPTG_Priv_Ext *me,
                           AEEBTAVRCPTgEventType *pEvent)
{
   int ret = AEE_ENOMORE;

   MSG_MED("AEEBTAVRCP TG Ext: GetEvent", 0,0,0);

   if ((NULL == pEvent) ||
       (!AEE_CHECK_PTR("AEEAVRCPTGPrivExt_GetEvent",
                       pEvent, sizeof(AEEBTAVRCPTgEventType), TRUE)))
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: get cmd bad param pCmd %x",pEvent,0,0);
      return AEE_EBADPARM;
   }

   MEMSET((void*)pEvent, 0, sizeof(AEEBTAVRCPTgEventType));

   // pop the first event on the queue.
   ret = AEEBTEvtQ_Get(&me->cmdAppAvrcpExtTgQueue, (uint8*)pEvent);

   MSG_MED("AEEBTAVRCP TG Ext: get event ret %x <-",ret,0,0);

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_GetAttrConfigUpdates
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_GetAttrConfigUpdates(AEEAVRCPTG_Priv_Ext* me, 
                                       boolean* startUpdate, 
                                       uint32* songPosUpdateMs)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: GetAttrConfigUpdates", 0,0,0);

   if ( ( NULL == songPosUpdateMs ) || ( NULL == startUpdate ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: GetAttrConfigUpdates bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }


   *startUpdate = me->startUpdate;
   *songPosUpdateMs = me->songPosUpdate; 


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetMediaTitle
//
//=============================================================================
static AEEResult
AEEAVRCPTGPrivExt_SetMediaTitle(AEEAVRCPTG_Priv_Ext* me,
                                uint32 charset,
                                const uint8* title, 
                                int titleLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetMediaTitle", 0,0,0);

   if ( ( NULL == title ) || ( 0 >= titleLen ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetMediaTitle bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }



   if (NULL != me->curMediaInfo.trackTitle)
   {
      FREEIF( me->curMediaInfo.trackTitle );
      me->curMediaInfo.trackTitle = NULL;
   }

   me->curMediaInfo.trackTitle = MALLOC(titleLen);
   if ( NULL == me->curMediaInfo.trackTitle )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetMediaTitle can't allocate memory ",0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.trackTitleCharset = charset;
   me->curMediaInfo.trackTitleLen = (uint32)titleLen;
   MEMCPY(me->curMediaInfo.trackTitle, title, titleLen);



   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetArtistName
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetArtistName(AEEAVRCPTG_Priv_Ext* me, 
                                uint32 charset, 
                                const uint8* artistName, 
                                int artistNameLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetArtistName", 0,0,0);

   if ( ( NULL == artistName ) || ( 0 >= artistNameLen ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetArtistName bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }



   if ( NULL != me->curMediaInfo.trackArtistName )
   {
      FREEIF( me->curMediaInfo.trackArtistName );
      me->curMediaInfo.trackArtistName = NULL;
   }

   me->curMediaInfo.trackArtistName = MALLOC( artistNameLen );
   if ( NULL == me->curMediaInfo.trackArtistName )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetArtistName can't allocate memory ",0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.trackArtistNameCharset = charset;
   me->curMediaInfo.trackArtistNameLen = (uint32)artistNameLen;
   MEMCPY(me->curMediaInfo.trackArtistName, artistName, artistNameLen);


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetAlbumName
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetAlbumName(AEEAVRCPTG_Priv_Ext* me, 
                               uint32 charset, 
                               const uint8* albumName, 
                               int albumNameLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetAlbumName", 0,0,0);

   if ( ( NULL == albumName ) || ( 0 >= albumNameLen ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetAlbumName bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }



   if ( NULL != me->curMediaInfo.albumName )
   {
      FREEIF( me->curMediaInfo.albumName );
      me->curMediaInfo.albumName = NULL;
   }

   me->curMediaInfo.albumName = MALLOC( albumNameLen );
   if ( NULL == me->curMediaInfo.albumName )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetAlbumName can't allocate memory ",0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.albumNameCharset = charset;
   me->curMediaInfo.albumNameLen = (uint32)albumNameLen;
   MEMCPY(me->curMediaInfo.albumName, albumName, albumNameLen);



   return ret;

}

//=============================================================================
// AEEAVRCPTGPrivExt_SetMediaTrackNumber
//
//=============================================================================
AEEResult AEEAVRCPTGPrivExt_SetMediaTrackNumber(AEEAVRCPTG_Priv_Ext* me, 
                                                const uint8* trackNumber, 
                                                int trackNumberLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetMediaTrackNumber", 0,0,0);

   if ( ( 0 >= trackNumberLen ) || ( NULL == trackNumber ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetMediaTrackNumber bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }


   if ( NULL != me->curMediaInfo.trackNum )
   {
      FREEIF( me->curMediaInfo.trackNum );
      me->curMediaInfo.trackNum = NULL;
   }

   me->curMediaInfo.trackNum = MALLOC( trackNumberLen );
   if ( NULL == me->curMediaInfo.trackNum )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetMediaTrackNumber can't allocate memory "
                ,0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.trackNumLen = (uint32)trackNumberLen;
   MEMCPY(me->curMediaInfo.trackNum, trackNumber, trackNumberLen);


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetTotalTracks
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetTotalTracks(AEEAVRCPTG_Priv_Ext* me,
                                 const uint8* totalTracks, 
                                 int totalTracksLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetTotalTracks", 0,0,0);

   if ( ( 0 >= totalTracksLen ) || ( NULL == totalTracks ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetTotalTracks bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }


   if ( NULL != me->curMediaInfo.trackTotal )
   {
      FREEIF( me->curMediaInfo.trackTotal );
      me->curMediaInfo.trackTotal = NULL;
   }

   me->curMediaInfo.trackTotal = MALLOC( totalTracksLen );
   if ( NULL == me->curMediaInfo.trackTotal )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetTotalTracks can't allocate memory ",0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.trackTotalLen = (uint32)totalTracksLen;
   MEMCPY(me->curMediaInfo.trackTotal, totalTracks, totalTracksLen);


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetTotalTracks
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetGenre(AEEAVRCPTG_Priv_Ext* me, 
                           uint32 charset,
                           const uint8* genre, 
                           int genreLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetGenre", 0,0,0);

   if ( ( 0 >= genreLen ) || ( NULL == genre ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetGenre bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }


   if ( NULL != me->curMediaInfo.trackGenre )
   {
      FREEIF( me->curMediaInfo.trackGenre );
      me->curMediaInfo.trackGenre = NULL;
   }

   me->curMediaInfo.trackGenre = MALLOC( genreLen );
   if ( NULL == me->curMediaInfo.trackGenre )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetGenre can't allocate memory ",0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.trackGenreCharset = charset;
   me->curMediaInfo.trackGenreLen     = (uint32)genreLen;
   MEMCPY(me->curMediaInfo.trackGenre, genre, genreLen);


   return ret;

}
//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayingTimeMs
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayingTimeMs(AEEAVRCPTG_Priv_Ext* me,
                                   const uint8* playingTime, 
                                   int playingTimeLen)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayingTimeMs", 0,0,0);

   if ( ( 0 >= playingTimeLen ) || ( NULL == playingTime ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayingTimeMs bad parameter ",0,0,0);
      return AEE_EBADPARM;
   }



   if ( NULL != me->curMediaInfo.playTime )
   {
      FREEIF( me->curMediaInfo.playTime );
      me->curMediaInfo.playTime = NULL;
   }

   me->curMediaInfo.playTime = MALLOC( playingTimeLen );
   if ( NULL == me->curMediaInfo.playTime )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayingTimeMs can't allocate memory ",0,0,0);

      return AEE_ENOMEMORY;
   }
   me->curMediaInfo.playTimeLen = (uint32)playingTimeLen;
   MEMCPY(me->curMediaInfo.playTime, playingTime, playingTimeLen);


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetSongPositionMs
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetSongPositionMs(AEEAVRCPTG_Priv_Ext* me, 
                                    uint32 curPosMs)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: SetSongPositionMs", 0,0,0);


   me->playbackInfo.curSongPos = curPosMs;
   if ( me->pendingNotificationMask & AEEBTAVRCPTG_PLAYBACK_POS_CHANGED )
   {
      /* Call the Event notification reply with the event id and curr pos */

      ret = OEMBTAVRCPTGExt_SendRegNotifReply(me->pOemObj,
                                              me->notifTxn,
                                              AEEBTAVRCPTG_PLAYBACK_POS_CHANGED,  
                                              &me->playbackInfo,
                                              0,
                                              AEEBTAVRCPTG_SYSTEM_POWER_ON,
                                              TRUE,
                                              NULL,
                                              0);
      if ( AEE_SUCCESS == ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetSongPositionMs ",0,0,0);
      }
      /* Reset the mask */
      me->pendingNotificationMask = me->pendingNotificationMask & 
                                    (~AEEBTAVRCPTG_PLAYBACK_POS_CHANGED);
   }

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayStatus
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayStatus(AEEAVRCPTG_Priv_Ext* me,
                                AEEBTAVRCPTgPlayStatus playStatus,
                                uint32 curPosMs,
                                const uint8* trackIndex,
                                int trackIndexLen) 
{
   uint32 ret = AEE_SUCCESS;
   uint32 eventId = 0;

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayStatus", 0,0,0);


   switch ( playStatus )
   {
   case AEEAVRCPTG_PLAY_STAT_TRACK_CHANGED:
      if ( ( NULL == trackIndex ) || (0 >= trackIndexLen) )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayStatus bad parameter ",0,0,0);

         return AEE_EBADPARM;
      }
      MEMCPY(me->playbackInfo.trackIndex, trackIndex, 
             trackIndexLen); 

      eventId = AEEBTAVRCPTG_TRACK_CHANGED;
      break;

   case AEEAVRCPTG_PLAY_STAT_TRACK_REACHED_END:
      eventId = AEEBTAVRCPTG_TRACK_REACHED_END;
      break;

   case AEEAVRCPTG_PLAY_STAT_TRACK_REACHED_START:
      eventId = AEEBTAVRCPTG_TRACK_REACHED_START;
      break;

   case AEEAVRCPTG_PLAYSTAT_STOPPED:
   case AEEAVRCPTG_PLAYSTAT_PLAYING:
   case AEEAVRCPTG_PLAYSTAT_PAUSED: 
   case AEEAVRCPTG_PLAYSTAT_SEEK_FORWARD:
   case AEEAVRCPTG_PLAYSTAT_SEEK_BACKWARD:
      if ( ( NULL == trackIndex ) || (0 >= trackIndexLen) )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayStatus bad parameter ",0,0,0);

         return AEE_EBADPARM;
      }
      eventId = AEEBTAVRCPTG_PLAYBACK_STATUS_CHANGED;
      me->playbackInfo.playStatus = playStatus;
      MEMCPY( me->playbackInfo.trackIndex, trackIndex, trackIndexLen ); 
      break;
   case AEEAVRCPTG_PLAYSTAT_ERROR:
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayStatus error ",0,0,0);
      break;

   default:
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayStatus bad parameter ",0,0,0);
      return AEE_EBADPARM; 
   }

   me->playbackInfo.curSongPos = curPosMs;

   if ( me->pendingNotificationMask & eventId )
   {
      /* Send the notification reply with mask as event id
         and playback status as current status */
      ret = OEMBTAVRCPTGExt_SendRegNotifReply(me->pOemObj,
                                              me->notifTxn,
                                              eventId,  
                                              &me->playbackInfo,
                                              0,
                                              AEEBTAVRCPTG_SYSTEM_POWER_ON,
                                              TRUE,
                                              NULL,
                                              0);
      if ( AEE_SUCCESS == ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayStatus ",0,0,0);
      }
      /* Resetting the mask */
      me->pendingNotificationMask = me->pendingNotificationMask & (~eventId);
   }
   if (0 != eventId)
   {
      eventId = AEEBTAVRCPTG_PLAYBACK_POS_CHANGED;
      if ( me->pendingNotificationMask & eventId )
      {
         /* Send the notification reply with mask as event id 
         and curr position */
         ret = OEMBTAVRCPTGExt_SendRegNotifReply(me->pOemObj,
                                                 me->notifTxn,
                                                 eventId,  
                                                 &me->playbackInfo,
                                                 0,
                                                 AEEBTAVRCPTG_SYSTEM_POWER_ON,
                                                 TRUE,
                                                 NULL,
                                                 0);
         if ( AEE_SUCCESS == ret )
         {
            MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayStatus ",0,0,0);
         }
         me->pendingNotificationMask = me->pendingNotificationMask & (~eventId);
      }
   }

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetCapability
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetCapability(AEEAVRCPTG_Priv_Ext* me,
                                AEEBTAVRCPTgMediaEventMask eventMask, 
                                uint32 companyId)
{
   uint32 ret = AEE_SUCCESS;
   uint32 i = 0;

   MSG_MED("AEEBTAVRCP TG Ext: SetCapability", 0,0,0);

   if ( ( ( AEEBTAVRCPTG_PLAYBACK_STATUS_CHANGED > eventMask )
          && ( AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED < eventMask) ) ||
        ( 0 == companyId ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetCapability ",0,0,0);
      return AEE_EBADPARM;
   }

   me->companyId[i++] = AVRCPTG_BTSIG_REGD_COMPID;
   me->companyId[i] = companyId;
   me->mediaEventCapMask = eventMask;

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_GetControllerBatteryStatus
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_GetControllerBatteryStatus(AEEAVRCPTG_Priv_Ext* me,
                                             AEEBTAVRCPTgContBattStatus* batteryStatus)
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: GetControllerBatteryStatus", 0,0,0);

   if ( NULL == batteryStatus )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: GetControllerBatteryStatus ",0,0,0);
      return AEE_EBADPARM;
   }


   if ( TRUE == me->avrcpCtBatteryStatus.statUpdated )
   {
      *batteryStatus = me->avrcpCtBatteryStatus.battStatus;
      me->avrcpCtBatteryStatus.statUpdated = FALSE;
   } 
   else
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: GetCtBatteryStatus Status not changed "
                ,0,0,0);
      ret = AEE_EFAILED;
   }

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayerAttrList
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrList(AEEAVRCPTG_Priv_Ext* me, 
                                    const int* attributeids, 
                                    int attributeidsCount)
{
   uint32 ret = AEE_SUCCESS;
   uint32 i = 0;

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayerAttrList", 0,0,0);

   if ( ( NULL == attributeids ) || (0 >= attributeidsCount) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrList ",0,0,0);
      return AEE_EBADPARM;
   }

   for (i = 0; i < (uint32)attributeidsCount; i++ )
   {
      if ( ( AEEBTAVRCPTG_PLAYER_ILLEGAL_STATUS == attributeids[i] ) ||
           ( AEEBTAVRCPTG_PLAYER_SCAN_STATUS < attributeids[i]) )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrList ",0,0,0);
         return AEE_EBADPARM;
      }
   }


   for ( i= 0; i < (uint32)attributeidsCount; i++ )
   {
      if ( 0 == me->playerInfo[i].attrId )
      {
         me->playerInfo[i].attrId = (uint32)attributeids[i];
      } 
      else
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrList Attr list already set"
                   ,0,0,0);
         break;
      }
   }

   // me->state = 

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayerAttrValueList
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrValueList(AEEAVRCPTG_Priv_Ext* me,
                                         uint32 attributeid,
                                         const int* attributeValues,
                                         int attributeValuesCount)
{
   uint32 ret = AEE_SUCCESS;
   boolean attr_present = FALSE;
   uint32 i = 0;
   uint32 j = 0;

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayerAttrValueList", 0,0,0);

   if ( ( NULL == attributeValues ) ||  ( 0 >= attributeValuesCount) ||
        ( AEEBTAVRCPTG_PLAYER_ILLEGAL_STATUS == attributeid ) ||
        ( AEEBTAVRCPTG_PLAYER_SCAN_STATUS < attributeid) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrValueList ",0,0,0);
      return AEE_EBADPARM;
   }



   for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( me->playerInfo[i].attrId == attributeid )
      {
         attr_present = TRUE;
         if ( me->playerInfo[i].values != NULL )
         {
            for ( j = 0; j < me->playerInfo[i].numValues; j++ )
            {
               if ( me->playerInfo[i].values[j].valueText != NULL )
               {
                  FREEIF( me->playerInfo[i].values[j].valueText );
                  me->playerInfo[i].values[j].valueText = NULL;
               }
            }
            FREEIF( me->playerInfo[i].values);
            me->playerInfo[i].values = NULL;
         }
         me->playerInfo[i].numValues = (uint8)attributeValuesCount;

         me->playerInfo[i].values = MALLOC( attributeValuesCount * 
                                            sizeof(AEEBTAVRCPTgAttributeValueInfo) );
         if ( me->playerInfo[i].values == NULL )
         {
            MSG_ERROR("AEEBTAVRCP TG Ext:SetPlayerAttrValueList can't alloc mem ",
                      0,0,0);

            return AEE_ENOMEMORY;
         }
         for ( j = 0; j < (uint32)attributeValuesCount; j++ )
         {
            me->playerInfo[i].values[j].valueId = (uint32)attributeValues[j];
         }
         break;
      } // External If loop
   } // External for loop



   if ( FALSE == attr_present )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: Attribute not supported ",0,0,0);
      return AEE_EBADPARM;
   }
   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayerCurrentAttrValue
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerCurrentAttrValue(AEEAVRCPTG_Priv_Ext* me,
                                            uint32 attributeid,
                                            uint32 attributeValue)
{
   uint32 ret = AEE_SUCCESS;
   boolean attr_val_present = FALSE;
   uint32                 i = 0;
   uint32                 j = 0;
   uint32                 k = 0;
   uint32                 attrCount = 0;
   uint32 attrInfo[AEEBTAVRCPTG_MAX_NUM_OF_VALS];

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayerCurrentAttrValue", 0,0,0);

   switch ( attributeid )
   {
   case AEEBTAVRCPTG_PLAYER_EQUALISER_STATUS:  
      if ( (AEEBTAVRCPTG_PLAYER_EQUALISER_OFF != attributeValue) &&
           (AEEBTAVRCPTG_PLAYER_EQUALISER_ON != attributeValue) )
      {
         ret = AEE_EBADPARM;
      }
      break;   

   case AEEBTAVRCPTG_PLAYER_REPEAT_STATUS: 
      if ( (AEEBTAVRCPTG_PLAYER_REPEAT_OFF > attributeValue) || 
           (AEEBTAVRCPTG_PLAYER_REPEAT_GROUP_TRACK < attributeValue) )
      {
         ret = AEE_EBADPARM;
      }
      break;

   case AEEBTAVRCPTG_PLAYER_SHUFFLE_STATUS: 
      if ( (AEEBTAVRCPTG_PLAYER_SHUFFLE_OFF > attributeValue) || 
           (AEEBTAVRCPTG_PLAYER_SHUFFLE_GROUP_TRACKS < attributeValue) )
      {
         ret = AEE_EBADPARM;
      }
      break;

   case AEEBTAVRCPTG_PLAYER_SCAN_STATUS: 
      if ( ( AEEBTAVRCPTG_PLAYER_SCAN_GROUP_TRACKS < attributeValue )
		  || ( AEEBTAVRCPTG_PLAYER_SCAN_OFF > attributeValue ) )
      {
         ret = AEE_EBADPARM;
      }
      break;  

   default:
      ret = AEE_EBADPARM;
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerCurrentAttrValue Not supported param",
                0,0,0);
   }

   if ( AEE_SUCCESS != ret )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerCurrentAttrValue Not supported param",
                0,0,0);
      return AEE_EBADPARM;
   }


   for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( me->playerInfo[i].attrId == attributeid )
      {
         if ( me->playerInfo[i].values != NULL )
         {
            for ( j = 0; j < me->playerInfo[i].numValues; j++ )
            {
               if ( me->playerInfo[i].values[j].valueId == attributeValue )
               {
                  attr_val_present = TRUE;
                  break;
               }
            }
         }
      }
      if ( TRUE == attr_val_present )
      {
         break;
      }
   }

   if ( FALSE == attr_val_present )
   {
      ret = AEE_EBADPARM;
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerCurrentAttrValue Not supported param",
                0,0,0);

      return AEE_EBADPARM;
   } 
   else
   {
      for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
      {
         if ( 0 == me->curPlayerAttrValues[i].attrId )
         {
            me->curPlayerAttrValues[i].attrId = attributeid;
            if( me->curPlayerAttrValues[i].valueId != attributeValue )
            {
              me->curPlayerAttrValues[i].valueId = attributeValue;
              me->curPlayerAttrValues[i].valUpdated = TRUE;
            }
            break;
         } 
         else if ( me->curPlayerAttrValues[i].attrId == attributeid )
         {
            if( me->curPlayerAttrValues[i].valueId != attributeValue )
            {
              me->curPlayerAttrValues[i].valueId = attributeValue;
              me->curPlayerAttrValues[i].valUpdated = TRUE;
            }
            break;
         }
      } 
   }

   if ( me->pendingNotificationMask & AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED )
   {
      for (k = 0; k < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; k++ )
      {
         if ( TRUE == me->curPlayerAttrValues[k].valUpdated )
         {
            attrInfo[2*attrCount] = me->curPlayerAttrValues[k].attrId;
            attrInfo[2*attrCount + 1 ] = me->curPlayerAttrValues[k].valueId;
            me->curPlayerAttrValues[k].valUpdated = FALSE;
            attrCount++;
         }
      }
      ret = OEMBTAVRCPTGExt_SendRegNotifReply(me->pOemObj,
                                              me->notifTxn,
                                              AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED,  
                                              NULL,
                                              0,
                                              AEEBTAVRCPTG_SYSTEM_POWER_ON,
                                              TRUE,
                                              attrInfo,
                                              attrCount);

      if ( AEE_SUCCESS == ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerCurrentAttrValue ",0,0,0);
      }
      me->pendingNotificationMask = me->pendingNotificationMask & 
                                    (~AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED);
   } 
   else
   {
      for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
      {
         me->curPlayerAttrValues[i].valUpdated = FALSE;
      }
   }

   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_GetPlayerCurrentAttrValue
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_GetPlayerCurrentAttrValue(AEEAVRCPTG_Priv_Ext* me,
                                            uint32* attributeid,    
                                            uint32* attributeValue)
{
   uint32 ret = AEE_SUCCESS;
   boolean valueRtrvd = FALSE;
   uint32 i = 0;

   MSG_MED("AEEBTAVRCP TG Ext: GetPlayerCurrentAttrValue", 0,0,0);

   if ( ( NULL == attributeid ) || ( NULL == attributeValue ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: GetPlayerCurrentAttrValue bad param",
                0,0,0);
      return AEE_EBADPARM;
   }



   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( TRUE == me->curPlayerAttrValByCt[i].curValStatusUpdatebyCt )
      {
         *attributeid    = me->curPlayerAttrValByCt[i].curValueUpdate.attrId;
         *attributeValue = me->curPlayerAttrValByCt[i].curValueUpdate.valueId;
         me->curPlayerAttrValByCt[i].curValStatusUpdatebyCt = FALSE;
         valueRtrvd = TRUE;
         break;
      }
   }


   if ( FALSE == valueRtrvd )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: GetPlayerCurrentAttrValue didn't change",
                0,0,0);
      ret = AEE_EFAILED;
   }
   return ret;

}

//=============================================================================
// AEEAVRCPTGPrivExt_GetDisplayableCharacterSet
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_GetDisplayableCharacterSet(AEEAVRCPTG_Priv_Ext* me, 
                                             uint32* charset)
{
   uint32 ret = AEE_SUCCESS;
   uint32 i   = 0;
   boolean    valueRtrvd = FALSE;

   MSG_MED("AEEBTAVRCP TG Ext: GetDisplayableCharacterSet", 0,0,0);

   if ( NULL == charset )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrText Not supported param",
                0,0,0);
      return AEE_EBADPARM;
   }


   for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_VALS; i++ )
   {
      if ( TRUE == me->charsetSuppByCt[i].valUpdated )
      {
         *charset = me->charsetSuppByCt[i].charset;
         me->charsetSuppByCt[i].valUpdated = FALSE;
         valueRtrvd = TRUE;
         break;
      }
   }

   if ( TRUE != valueRtrvd )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: GetDisplayableCharacterSet no charset update",
                0,0,0);

      return AEE_EFAILED;
   }


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayerAttrText
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrText(AEEAVRCPTG_Priv_Ext* me,
                                    uint32 attributeid,
                                    uint32 charset,
                                    const uint8* attributeText,
                                    int attributeTextLen)
{
   uint32 ret           = AEE_SUCCESS;
   boolean attr_present = FALSE;
   uint32  i            = 0;

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayerAttrText", 0,0,0);

   if ( (AEEBTAVRCPTG_PLAYER_ILLEGAL_STATUS == attributeid ) ||
        ( AEEBTAVRCPTG_PLAYER_SCAN_STATUS < attributeid ) || 
        ( NULL == attributeText ) ||
        ( 0 >= attributeTextLen ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrText Not supported param",
                0,0,0);
      return AEE_EBADPARM;
   }


   for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( me->playerInfo[i].attrId == attributeid )
      {
         if ( me->playerInfo[i].attrText != NULL )
         {
            FREEIF( me->playerInfo[i].attrText );
            me->playerInfo[i].attrText = NULL;
         }
         me->playerInfo[i].attrText = MALLOC(attributeTextLen);
         if ( NULL == me->playerInfo[i].attrText )
         {
            MSG_ERROR("AEEBTAVRCP TG Ext:SetPlayerAttrText can't alloc mem ",
                      0,0,0);

            return AEE_ENOMEMORY;
         }
         me->playerInfo[i].attrInfoCharset = charset;
         me->playerInfo[i].attrTextLen = (uint32)attributeTextLen;
         MEMCPY( me->playerInfo[i].attrText, attributeText, attributeTextLen );
         attr_present = TRUE;
         break;
      }
   }


   if ( FALSE == attr_present )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrText Not supported param",
                0,0,0);
      return AEE_EBADPARM;
   }
   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetPlayerAttrValueText
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetPlayerAttrValueText(AEEAVRCPTG_Priv_Ext* me,
                                         uint32 attributeid,
                                         uint32 attributeValue,
                                         uint32 charset,
                                         const uint8* attributeValueText,
                                         int attributeValueTextLen)
{ 
   uint32 ret = AEE_SUCCESS;
   boolean attr_val_present = FALSE;
   uint32         i = 0;
   uint32         j = 0;

   MSG_MED("AEEBTAVRCP TG Ext: SetPlayerAttrValueText", 0,0,0);

   if ( ( NULL == attributeValueText ) ||
        ( 0 >= attributeValueTextLen ) )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrValueText Not supported param",
                0,0,0);
      return AEE_EBADPARM;  
   }

   switch ( attributeid )
   {
   case AEEBTAVRCPTG_PLAYER_EQUALISER_STATUS:  
      if ( (AEEBTAVRCPTG_PLAYER_EQUALISER_OFF != attributeValue) &&
           (AEEBTAVRCPTG_PLAYER_EQUALISER_ON != attributeValue) )
      {
         ret = AEE_EBADPARM;
      }
      break;   

   case AEEBTAVRCPTG_PLAYER_REPEAT_STATUS: 
      if ( (AEEBTAVRCPTG_PLAYER_REPEAT_OFF > attributeValue) || 
           (AEEBTAVRCPTG_PLAYER_REPEAT_GROUP_TRACK < attributeValue) )
      {
         ret = AEE_EBADPARM;
      }
      break;

   case AEEBTAVRCPTG_PLAYER_SHUFFLE_STATUS: 
      if ( (AEEBTAVRCPTG_PLAYER_SHUFFLE_OFF > attributeValue) || 
           (AEEBTAVRCPTG_PLAYER_SHUFFLE_GROUP_TRACKS < attributeValue) )
      {
         ret = AEE_EBADPARM;
      }
      break;

   case AEEBTAVRCPTG_PLAYER_SCAN_STATUS: 
      if ( ( AEEBTAVRCPTG_PLAYER_SCAN_GROUP_TRACKS < attributeValue ) ||
		   ( AEEBTAVRCPTG_PLAYER_SCAN_OFF > attributeValue ) )
      {
         ret = AEE_EBADPARM;
      }
      break;  

   default:
      ret = AEE_EBADPARM;
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrValueText Not supported param",
                0,0,0);
   }

   if ( AEE_SUCCESS != ret )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrValueText Not supported param",
                0,0,0);
      return ret;
   }


   for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( me->playerInfo[i].attrId == attributeid )
      {
         if ( me->playerInfo[i].values != NULL )
         {
            for ( j = 0; j < me->playerInfo[i].numValues; j++ )
            {
               if ( me->playerInfo[i].values[j].valueId == attributeValue )
               {
                  if ( me->playerInfo[i].values[j].valueText != NULL )
                  {
                     FREEIF( me->playerInfo[i].values[j].valueText );
                     me->playerInfo[i].values[j].valueText = NULL;
                  }
                  me->playerInfo[i].values[j].valueText = 
                  MALLOC(attributeValueTextLen);
                  if ( NULL == me->playerInfo[i].values[j].valueText )
                  {
                     MSG_ERROR("AEEBTAVRCP TG Ext:SetPlayerAttrValueText can't alloc mem ",
                               0,0,0);

                     return AEE_ENOMEMORY;
                  }
                  me->playerInfo[i].values[j].valTextLen = (uint32)attributeValueTextLen;
                  me->playerInfo[i].values[j].valueTextCharset = 
                  charset;
                  MEMCPY( me->playerInfo[i].values[j].valueText,
                          attributeValueText, attributeValueTextLen );
                  attr_val_present = TRUE;
                  break;
               }
            }
         }
      }
      if ( TRUE == attr_val_present )
      {
         break;
      }
   }


   if ( TRUE != attr_val_present )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerAttrValueText Not supported param",
                0,0,0);
      return AEE_EBADPARM;  
   }
   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_NavigateGroup
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_GetNavigateGroupCmd( AEEAVRCPTG_Priv_Ext* me,
                                       AEEBTAVRCPTgGroupNavCommandEnum* 
                                       navigationCommand )
{
   uint32 ret = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: GetNavigateGroupCmd", 0,0,0);

   if ( NULL == navigationCommand )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: NavigateGroup Bad param",
                0,0,0);
      return AEE_EBADPARM; 
   }


   if ( TRUE == me->navigationCommand.statUpdated )
   {
      *navigationCommand = me->navigationCommand.grpNav;
      me->navigationCommand.statUpdated = FALSE;
   }


   return ret;
}

//=============================================================================
// AEEAVRCPTGPrivExt_SetLastCommandStatus
//
//=============================================================================
static AEEResult 
AEEAVRCPTGPrivExt_SetLastCommandStatus(AEEAVRCPTG_Priv_Ext* me,
                                       AEEBTAVRCPTgCmdResponseStatus status) 
{
   uint32               ret   = AEE_SUCCESS;
   uint32               result   = AEE_SUCCESS;
   AEEBTAVRCPTgPduId    pduId = AEEBTAVRCPTG_ILLEGAL_PDU_ID;

   MSG_MED("AEEBTAVRCP TG Ext: SetLastCommandStatus status = %x", 
           status,0,0);

   if ( status > AEEBTAVRCPTG_CMD_RESPONSE_CHANGED )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: SetLastCommandStatus Bad param",
                0,0,0);
      return AEE_EBADPARM;  
   }



   me->lastCmdInfo.cmdstatus = status;

   switch ( me->lastCmdInfo.cmdId )
   {
   case AEEBTAVRCPTG_SET_ATTR_CONFIG_UPDATES:
      return ret;

   case AEEBTAVRCPTG_PLAYER_APP_SETTING_MODIFIED:
      pduId = AEEBTAVRCPTG_SET_ATTRIBUTE_VALUES_COMMAND_IND;
      //me->lastCmdInfo.cmdstatus = AEEBTAVRCPTG_CMD_RESPONSE_ACCEPTED;
      break;

   case AEEBTAVRCPTG_CT_BATT_STATUS_CHANGED:
      pduId = AEEBTAVRCPTG_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND;
      break;

   case AEEBTAVRCPTG_INFORM_CHARACTER_SET:
      pduId = AEEBTAVRCPTG_INFORM_CHARACTER_SET_COMMAND_IND;
      break;

   case AEEBTAVRCPTG_GROUP_NAV_COMMAND:
      //me->lastCmdInfo.cmdstatus = AEEBTAVRCPTG_CMD_RESPONSE_ACCEPTED;
      break;
   }

   if ( pduId != AEEBTAVRCPTG_ILLEGAL_PDU_ID )
   {
      ret = OEMBTAVRCPTGExt_SendSimpleMetaReply(me->pOemObj,
                                                me->lastCmdInfo.txn,
                                                pduId,
                                                me->lastCmdInfo.cmdstatus );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetLastCommandStatus",0,0,0);
      }

   } 
   else
   {
      result = OEMBTAVRCPTGExt_SendGroupNavReply(me->pOemObj,
                                                 me->lastCmdInfo.txn,
                                                 me->navigationCommand.grpNav,
                                                 me->navigationCommand.buttPress,
                                                 me->lastCmdInfo.cmdstatus );
      if ( AEE_SUCCESS != result )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SetLastCommandStatus",0,0,0);
      }
   }

   return ret;
}

//=============================================================================
//                      AVRCP METADATA TARGET static functions
// 
//=============================================================================


//=============================================================================
//                    AEEBTAVRCPTg_CheckBattStatus
//
//=============================================================================
static void AEEBTAVRCPTg_CheckBattStatus(void *cb)
{
   uint32                  ret   = AEE_SUCCESS;
   AEEAVRCPTG_Priv_Ext     *pMe = ( AEEAVRCPTG_Priv_Ext* )cb;

   uint8  uNewBattLevel = vbatt_read (); 
   uint32 battStatus = 0;



   battStatus = pMe->avrcpTgBatteryStatus.battStatus;

   if ( AEEBTAVRCPTG_LEVEL_NORMAL(uNewBattLevel) )
   {
      pMe->avrcpTgBatteryStatus.battStatus = AEEBTAVRCPTG_CT_BATT_NORMAL;
   } 
   else if ( AEEBTAVRCPTG_LEVEL_WARNING(uNewBattLevel) )
   {
      pMe->avrcpTgBatteryStatus.battStatus = AEEBTAVRCPTG_CT_BATT_WARNING;
   } 
   else if ( AEEBTAVRCPTG_LEVEL_CRITICAL(uNewBattLevel) )
   {
      pMe->avrcpTgBatteryStatus.battStatus = AEEBTAVRCPTG_CT_BATT_CRITICAL;
   } 
   else if ( AEEBTAVRCPTG_LEVEL_FULL_CHARGE(uNewBattLevel) )
   {
      pMe->avrcpTgBatteryStatus.battStatus = AEEBTAVRCPTG_CT_BATT_FULL;
   }

   if ( pMe->avrcpTgBatteryStatus.battStatus != battStatus )
   {
      pMe->avrcpTgBatteryStatus.statUpdated = TRUE;
   }

   if ( pMe->pendingNotificationMask & AEEBTAVRCPTG_BATT_STATUS_CHANGED )
   {
      if ( FALSE != pMe->avrcpTgBatteryStatus.statUpdated )
      {
         ret = OEMBTAVRCPTGExt_SendRegNotifReply(pMe->pOemObj,
                                                 pMe->notifTxn,
                                                 AEEBTAVRCPTG_BATT_STATUS_CHANGED,  
                                                 NULL,
                                                 pMe->avrcpTgBatteryStatus.battStatus,
                                                 AEEBTAVRCPTG_SYSTEM_POWER_ON,
                                                 TRUE,
                                                 NULL,
                                                 0);
         if ( AEE_SUCCESS != ret )
         {
            MSG_ERROR("AEEBTAVRCP TG Ext: CheckBattStatus",0,0,0);
         }
         pMe->avrcpTgBatteryStatus.statUpdated = FALSE;
         pMe->pendingNotificationMask = pMe->pendingNotificationMask & 
                                        (~AEEBTAVRCPTG_BATT_STATUS_CHANGED);
      }

   }

   (void) AEE_SetSysTimer (AEEBTAVRCPTG_TG_BATT_STAT_UPDATE_INTVL, 
                           AEEBTAVRCPTg_CheckBattStatus, pMe);

}

//=============================================================================
//               AEEBTAVRCPTg_SysEvtCallback
//
//=============================================================================
static void AEEBTAVRCPTg_SysEvtCallback( void *cbData )
{
   AEEAVRCPTG_Priv_Ext       *pAvTgExt = ( AEEAVRCPTG_Priv_Ext* )cbData;
   AEEBTAVRCPTgExtCmdInfo    pCmd;

   int ret = AEE_EFAILED;

   MSG_MED("AEEBTAVRCP TG Ext: SysEvtCallback", 0,0,0);

   // pop the first event on the queue.
   ret = AEEBTEvtQ_Get(&pAvTgExt->cmdAeeOemAvrcpExtTgQueue, (uint8*)&pCmd);

   while (AEE_SUCCESS == ret)
   {
      MSG_MED("AEEBTAVRCP TG Ext:SysEvtCallback event %x",pCmd.cmdId.cmdPduId,0,0);

      switch (pCmd.cmdId.cmdPduId)
      {
      case AEEBTAVRCPTG_CAPABILITY_COMMAND_IND:
         AEEBTAVRCPTg_CmdCapInd(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_LIST_ATTRIBUTES_COMMAND_IND:
         AEEBTAVRCPTg_CmdListAttr(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_LIST_ATTRIBUTE_VALUES_COMMAND_IND:
         AEEBTAVRCPTg_CmdListAttrVal(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_GET_ATTRIBUTE_VALUES_COMMAND_IND:
         AEEBTAVRCPTg_CmdGetAttrVal(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_SET_ATTRIBUTE_VALUES_COMMAND_IND:
         AEEBTAVRCPTg_CmdSetAttrVal(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND:
         AEEBTAVRCPTg_CmdAttrGetTxt(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND:
         AEEBTAVRCPTg_CmdAttrGetValTxt(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_INFORM_CHARACTER_SET_COMMAND_IND:
         AEEBTAVRCPTg_CmdInformCharset(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND:
         AEEBTAVRCPTg_CmdInformBattStatus(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_GET_ELEMENT_ATTRIBUTE_COMMAND_IND:
         AEEBTAVRCPTg_CmdGetElementAttr(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_GET_PLAY_STATUS_COMMAND_IND:
         AEEBTAVRCPTg_CmdGetPlayStatus(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_EVENT_NOTIFICATION_COMMAND_IND:
         AEEBTAVRCPTg_CmdEvtNotif(pAvTgExt, &pCmd);
         break;
      case AEEBTAVRCPTG_GROUP_NAVIGATION_COMMAND_IND:
         AEEBTAVRCPTg_CmdGrpNavigation(pAvTgExt, &pCmd);
         break;
      default:   
         MSG_ERROR("AEEBTAVRCP TG Ext: SysEvtCallback Invalid cmd", 0,0,0);
      }
      ret = AEEBTEvtQ_Get(&pAvTgExt->cmdAeeOemAvrcpExtTgQueue, (uint8*)&pCmd);
   }
   return;

}

//=============================================================================
//               AEEBTAVRCPTg_OEMEvtCallback
//
//=============================================================================
static void AEEBTAVRCPTg_OEMEvtCallback(void   *cbData,
                                        AEEBTAVRCPTgExtCmdInfo    *pCmd)
{
   AEEAVRCPTG_Priv_Ext        *pAvTgExt = ( AEEAVRCPTG_Priv_Ext* )cbData;

   MSG_MED("AEEBTAVRCP TG Ext: OEMEvtCallback", 0,0,0);

   AEEBTEvtQ_Put(&pAvTgExt->cmdAeeOemAvrcpExtTgQueue, (uint8*)pCmd);
   CALLBACK_Init(&pAvTgExt->BREWEvCb, AEEBTAVRCPTg_SysEvtCallback, pAvTgExt);
   AEE_SYS_RESUME(&pAvTgExt->BREWEvCb);
}


//=============================================================================
//               AEEBTAVRCPTg_CmdCapInd
//
//=============================================================================
static void 
AEEBTAVRCPTg_CmdCapInd(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int ret            = AEE_SUCCESS;
   uint8 compCount    = 0;
   uint8 eventCount   = 0;
   uint32 i           = 0;           
   uint32 events[] = {
      AEEBTAVRCPTG_PLAYBACK_STATUS_CHANGED,
      AEEBTAVRCPTG_TRACK_CHANGED,
      AEEBTAVRCPTG_TRACK_REACHED_END,
      AEEBTAVRCPTG_TRACK_REACHED_START,
      AEEBTAVRCPTG_PLAYBACK_POS_CHANGED,
      AEEBTAVRCPTG_BATT_STATUS_CHANGED,
      AEEBTAVRCPTG_SYSTEM_STATUS_CHANGED,
      AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED};


   MSG_MED("AEEBTAVRCP TG Ext: CmdCapInd", 0,0,0);


   if ( AEEBTAVRCPTG_CAPABILITY_COMPANY_ID == cmdInfo->pduInfo.capId )
   {
      for (i = 0; i < AEEBTAVRCPTG_NUM_OF_COMP_SUPPORTED; i++ )
      {
         if ( 0 != pAvrcpTg->companyId[i] )
         {
            compCount++;
         }
      }

      if ( 0 == compCount)
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CMD_RESPONSE_REJECTED for CAPABILITY_COMPANY_ID ",
                   0,0,0); 
         AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo, cmdInfo->cmdId.cmdPduId, 
                                    AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );

         return;
      }
      ret = OEMBTAVRCPTGExt_SendCompIdReply( pAvrcpTg->pOemObj,
                                             cmdInfo->cmdId.txn,
                                             compCount,
                                             pAvrcpTg->companyId );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SendCompIdReply",
                   0,0,0);
      }
   } 
   else if ( AEEBTAVRCPTG_CAPABILITY_EVENTS_SUPPORTED == cmdInfo->pduInfo.capId )
   {
      for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_EVENTS_SUPPORTED; i++ )
      {
         if ( pAvrcpTg->mediaEventCapMask &  events[i])
         {
            events[eventCount] = events[i];
            eventCount++;
         }
      }

      if ( 0 == eventCount )
      {
         AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo, cmdInfo->cmdId.cmdPduId, 
                                    AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );

         return;
      }

      OEMBTAVRCPTGExt_SendEventIdReply(pAvrcpTg->pOemObj,
                                       cmdInfo->cmdId.txn,
                                       eventCount,
                                       events );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SendEventIdReply",
                   0,0,0);
      }
   } 
   else
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: Invalid cap param ",
                0,0,0);
   }


}


//=============================================================================
//               AEEBTAVRCPTg_CmdListAttr
//
//=============================================================================
static void 
AEEBTAVRCPTg_CmdListAttr(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                            ret            = AEE_SUCCESS;
   uint8                          attrCount      = 0;
   uint32                         i              = 0; 
   AEEBTAVRCPTgPlayerAppAttrs     attrIdList[AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED];          

   MSG_MED("AEEBTAVRCP TG Ext: CmdListAttr", 0,0,0);



   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( 0 != pAvrcpTg->playerInfo[i].attrId )
      {
         if ( AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED > attrCount)
         {
            attrIdList[attrCount] = pAvrcpTg->playerInfo[i].attrId;
            attrCount++;
         }
      }
   }

   if ( 0 == attrCount )
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );

      return;
   }

   ret= OEMBTAVRCPTGExt_ListAttrIdReply(pAvrcpTg->pOemObj,
                                        cmdInfo->cmdId.txn,
                                        attrCount,
                                        attrIdList);
   if ( AEE_SUCCESS != ret )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: ListAttrIdReply",0,0,0);
   }

}

//=============================================================================
//               AEEBTAVRCPTg_CmdListAttrVal
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdListAttrVal(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                     ret            = AEE_SUCCESS;
   uint8                   attrValCount   = 0;
   uint32                  i              = 0; 
   uint32                  j              = 0;
   uint32                  attrVals[AEEBTAVRCPTG_MAX_NUM_OF_PLAYER_ATTR_VALS];

   MSG_MED("AEEBTAVRCP TG Ext: CmdListAttrVal", 0,0,0);



   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( cmdInfo->attrId == pAvrcpTg->playerInfo[i].attrId )
      {
         if ( ( 0 != pAvrcpTg->playerInfo[i].numValues ) && 
              ( NULL != pAvrcpTg->playerInfo[i].values ) )
         {
            for ( j = 0; j < pAvrcpTg->playerInfo[i].numValues; j++ )
            {
               if ( AEEBTAVRCPTG_MAX_NUM_OF_PLAYER_ATTR_VALS > attrValCount )
               {
                  attrVals[attrValCount++] = pAvrcpTg->playerInfo[i].values[j].valueId;
               }
            }
         }
      }
   }

   if ( 0 == attrValCount )
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );

      return;
   }

   ret = OEMBTAVRCPTGExt_ListAttrValReply(pAvrcpTg->pOemObj,
                                          cmdInfo->cmdId.txn,
                                          cmdInfo->attrId,
                                          attrValCount,
                                          attrVals);
   if ( AEE_SUCCESS != ret )
   {
      MSG_ERROR("AEEBTAVRCP TG Ext: CmdListAttrVal",0,0,0);
   }

}

//=============================================================================
//               AEEBTAVRCPTg_CmdGetAttrVal
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdGetAttrVal(AEEAVRCPTG_Priv_Ext  *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                     ret            = AEE_SUCCESS;
   uint8                   attrInfoCount   = 0;
   uint32                  i               = 0; 
   uint32                  j               = 0;
   uint32                  attrInfo[AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED * 2];

   MSG_MED("AEEBTAVRCP TG Ext: CmdGetAttrVal ", 0,0,0 );



   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      for ( j = 0; j < cmdInfo->pduInfo.pduData.count; j++ )
      {
         if ( ( pAvrcpTg->curPlayerAttrValues[i].attrId == 
              cmdInfo->pduInfo.pduData.val[j] ) && 
			  ( 0 != cmdInfo->pduInfo.pduData.val[j] ) )
         {
            if ( AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED > attrInfoCount )
            {
               attrInfo[2*attrInfoCount] = pAvrcpTg->curPlayerAttrValues[i].attrId;
               attrInfo[2*attrInfoCount + 1] = pAvrcpTg->curPlayerAttrValues[i].valueId;
               attrInfoCount++;
            }
         }
      }
   }

   if ( 0 == attrInfoCount )
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
   } 
   else
   {
      ret = OEMBTAVRCPTGExt_GetAttrValReply(pAvrcpTg->pOemObj,
                                            cmdInfo->cmdId.txn,
                                            attrInfoCount,
                                            attrInfo);
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdGetAttrVal",0,0,0);
      }
   }

}

//=============================================================================
//               AEEBTAVRCPTg_CmdSetAttrVal
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdSetAttrVal(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                        ret             = AEE_SUCCESS;
   uint32                     i               = 0; 
   uint32                     j               = 0;
   boolean                    valUpdated      = FALSE; 
   AEEBTAVRCPTgEventType       evt;
   boolean                    attrValUpdatePending =  FALSE;
   boolean                    isValInvalid = FALSE;

   MSG_MED("AEEBTAVRCP TG Ext: CmdSetAttrVal ", 0,0,0 );

   MEMSET((void*)&evt,0,sizeof(AEEBTAVRCPTgEventType));



   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      attrValUpdatePending = attrValUpdatePending || 
                             pAvrcpTg->curPlayerAttrValByCt[i].curValStatusUpdatebyCt; 
      if ( attrValUpdatePending )
      {
         break;
      }
   }

   if ( FALSE == attrValUpdatePending )
   {
      for ( i = 0; i < cmdInfo->pduInfo.pduData.count; i++ )
      {
         if ( ( cmdInfo->pduInfo.pduData.val[2*i] > AEEBTAVRCPTG_PLAYER_ILLEGAL_STATUS )
              && (cmdInfo->pduInfo.pduData.val[2*i] <= AEEBTAVRCPTG_PLAYER_SCAN_STATUS ) )
         {
            switch ( cmdInfo->pduInfo.pduData.val[2*i] )
            {
            case AEEBTAVRCPTG_PLAYER_EQUALISER_STATUS:  
               if ( (AEEBTAVRCPTG_PLAYER_EQUALISER_OFF != cmdInfo->pduInfo.pduData.val[2*i + 1]) &&
                    (AEEBTAVRCPTG_PLAYER_EQUALISER_ON != cmdInfo->pduInfo.pduData.val[2*i + 1]) )
               {
                  isValInvalid = TRUE;
               }
               break;   

            case AEEBTAVRCPTG_PLAYER_REPEAT_STATUS: 
               if ( (AEEBTAVRCPTG_PLAYER_REPEAT_OFF > cmdInfo->pduInfo.pduData.val[2*i + 1] ) || 
                    (AEEBTAVRCPTG_PLAYER_REPEAT_GROUP_TRACK < cmdInfo->pduInfo.pduData.val[2*i + 1]) )
               {
                  isValInvalid = TRUE;
               }
               break;

            case AEEBTAVRCPTG_PLAYER_SHUFFLE_STATUS: 
               if ( (AEEBTAVRCPTG_PLAYER_SHUFFLE_OFF > cmdInfo->pduInfo.pduData.val[2*i + 1]) || 
                    (AEEBTAVRCPTG_PLAYER_SHUFFLE_GROUP_TRACKS < cmdInfo->pduInfo.pduData.val[2*i + 1]) )
               {
                  isValInvalid = TRUE;
               }
               break;

            case AEEBTAVRCPTG_PLAYER_SCAN_STATUS: 
               if ( ( AEEBTAVRCPTG_PLAYER_SCAN_OFF > cmdInfo->pduInfo.pduData.val[2*i + 1] ) ||
                    ( AEEBTAVRCPTG_PLAYER_SCAN_GROUP_TRACKS < cmdInfo->pduInfo.pduData.val[2*i + 1] ) )
               {
                  isValInvalid = TRUE;
               }
               break;  

            default:
               isValInvalid = TRUE;
               MSG_ERROR("AEEBTAVRCP TG Ext: SetPlayerCurrentAttrValue Not supported param",
                         0,0,0);
            }

            if ( FALSE == isValInvalid )
            {
               for ( j = 0; j < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; j++ )
               {
                  MSG_MED("AEEBTAVRCP TG Ext: CmdSetAttrVal Attr = %x, Val = %x", 
                          cmdInfo->pduInfo.pduData.val[2*i],
                          cmdInfo->pduInfo.pduData.val[2*i + 1],0 );
                  if ( 0 == pAvrcpTg->curPlayerAttrValByCt[j].curValueUpdate.attrId )
                  {
                     pAvrcpTg->curPlayerAttrValByCt[j].curValueUpdate.attrId = 
                     cmdInfo->pduInfo.pduData.val[2*i];
                     pAvrcpTg->curPlayerAttrValByCt[j].curValueUpdate.valueId = 
                     cmdInfo->pduInfo.pduData.val[2*i + 1];
                  } 
                  else if ( cmdInfo->pduInfo.pduData.val[2*i] == 
                              pAvrcpTg->curPlayerAttrValByCt[j].curValueUpdate.attrId )
                  {
                     pAvrcpTg->curPlayerAttrValByCt[j].curValueUpdate.valueId = 
                     cmdInfo->pduInfo.pduData.val[2*i + 1];
                  } 
                  else
                  {
                     continue;
                  }
                  pAvrcpTg->curPlayerAttrValByCt[j].curValStatusUpdatebyCt = TRUE;
                  valUpdated = TRUE;
                  pAvrcpTg->lastCmdInfo.cmdId = AEEBTAVRCPTG_PLAYER_APP_SETTING_MODIFIED;
                  pAvrcpTg->lastCmdInfo.txn = cmdInfo->cmdId.txn;
                  evt.event = AEEBTAVRCPTG_PLAYER_APP_SETTING_MODIFIED;
                  evt.evparam = 0;
                  AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
                  if ( pAvrcpTg->piSigAeeEvt )
                  {
                     MSG_MED("AEEBTAVRCP TG Ext: CmdSetAttrVal Setting the signal for attr = %x", 
                              cmdInfo->pduInfo.pduData.val[2*i],0,0);
                     ISignal_Set(pAvrcpTg->piSigAeeEvt);
                  }
                  break;
               } 
            }
         }
      }
   }

   if ( TRUE != valUpdated )
   {
      ret = AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                       AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdSetAttrVal",0,0,0);
      }
   } 
   else
   {
      evt.event = AEEBTAVRCPTG_UPDATE_LAST_CMD_STATUS;
      evt.evparam = 0;
      AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
      if ( pAvrcpTg->piSigAeeEvt )
      {
         ISignal_Set(pAvrcpTg->piSigAeeEvt);
      }
   }


}


//=============================================================================
//               AEEBTAVRCPTg_CmdAttrGetTxt
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdAttrGetTxt(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                        ret         = AEE_SUCCESS;
   uint8                      attrCount   = 0;
   uint32                     i           = 0; 
   uint32                     j           = 0;
   AEEBTAVRCPTgItemTxt        attrItem[AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED]; 

   MSG_MED("AEEBTAVRCP TG Ext: CmdAttrGetTxt ", 0,0,0 );


   for ( i = 0; i < cmdInfo->pduInfo.pduData.count; i++ )
   {
      for ( j = 0; j < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; j++ )
      {
         if ( cmdInfo->pduInfo.pduData.val[i] == pAvrcpTg->playerInfo[j].attrId )
         {
            if ( ( NULL != pAvrcpTg->playerInfo[j].attrText ) && 
                 (0 != pAvrcpTg->playerInfo[j].attrTextLen) )
            {
               if (AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED > attrCount )
               {
                  attrItem[attrCount].itemId = pAvrcpTg->playerInfo[j].attrId;
                  attrItem[attrCount].charset = pAvrcpTg->playerInfo[j].attrInfoCharset;
                  attrItem[attrCount].itemTxt = pAvrcpTg->playerInfo[j].attrText;
                  attrItem[attrCount].itemTxtLen = pAvrcpTg->playerInfo[j].attrTextLen;
                  attrCount++;
               }
            }
         }
      }
   }

   if ( 0 != attrCount )
   {
      ret = OEMBTAVRCPTGExt_GetAttrSettingTxtReply(pAvrcpTg->pOemObj,
                                                   cmdInfo->cmdId.txn,
                                                   attrCount,
                                                   attrItem );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdAttrGetTxt",0,0,0);
      }
   } 
   else
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
   }

}

//=============================================================================
//               AEEBTAVRCPTg_CmdAttrGetValTxt
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdAttrGetValTxt(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                        ret         = AEE_SUCCESS;
   uint8                      attrValCount = 0;
   uint32                     i           = 0; 
   uint32                     j           = 0;
   uint32                     k           = 0;
   AEEBTAVRCPTgItemTxt        attrTextItem[AEEBTAVRCPTG_MAX_NUM_OF_PLAYER_ATTR_VALS];

   MSG_MED("AEEBTAVRCP TG Ext: CmdAttrGetValTxt ", 0,0,0 );



   for ( i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
   {
      if ( cmdInfo->attrId == pAvrcpTg->playerInfo[i].attrId )
      {
         if ( NULL != pAvrcpTg->playerInfo[i].values )
         {
            for ( j = 0; j < pAvrcpTg->playerInfo[i].numValues ; j++ )
            {
               for ( k = 0; k < cmdInfo->pduInfo.pduData.count ; k++ )
               {
                  if ( pAvrcpTg->playerInfo[i].values[j].valueId == 
                       cmdInfo->pduInfo.pduData.val[k])
                  {
                     if ( NULL != pAvrcpTg->playerInfo[i].values[j].valueText )
                     {
                        if (AEEBTAVRCPTG_MAX_NUM_OF_PLAYER_ATTR_VALS > attrValCount )
                        {
                           attrTextItem[attrValCount].itemId = 
                           pAvrcpTg->playerInfo[i].values[j].valueId;
                           attrTextItem[attrValCount].charset = 
                           pAvrcpTg->playerInfo[i].values[j].valueTextCharset;
                           attrTextItem[attrValCount].itemTxt = 
                           pAvrcpTg->playerInfo[i].values[j].valueText;
                           attrTextItem[attrValCount].itemTxtLen = 
                           pAvrcpTg->playerInfo[i].values[j].valTextLen;
                           attrValCount++;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   if ( 0 != attrValCount )
   {
      ret = OEMBTAVRCPTGExt_GetAttrSettingValTxtReply(pAvrcpTg->pOemObj,
                                                      cmdInfo->cmdId.txn,
                                                      attrValCount,
                                                      attrTextItem );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdAttrGetValTxt",0,0,0);
      }
   } 
   else
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
   }


}


//=============================================================================
//               AEEBTAVRCPTg_CmdInformCharset
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdInformCharset(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                             ret = AEE_SUCCESS;  
   AEEBTAVRCPTgEventType           evt;
   AEEBTAVRCPTgCmdResponseStatus   status = AEEBTAVRCPTG_CMD_RESPONSE_REJECTED;
   uint32                          i;
   uint32                          charsetCount = 0;
   boolean                         charsetUpdatePending =  FALSE; 

   MSG_MED("AEEBTAVRCP TG Ext: CmdInformCharset ", 0,0,0 );

   MEMSET((void*)&evt,0,sizeof(AEEBTAVRCPTgEventType));


   for ( i = 0; i < cmdInfo->pduInfo.pduData.count; i++ )
   {
      charsetUpdatePending = charsetUpdatePending || 
                             pAvrcpTg->charsetSuppByCt[i].valUpdated;
      if ( charsetUpdatePending )
      {
         break;
      }
   }

   if ( FALSE == charsetUpdatePending )
   {
      for ( i = 0; i < cmdInfo->pduInfo.pduData.count; i++ )
      {
         pAvrcpTg->charsetSuppByCt[i].charset = cmdInfo->pduInfo.pduData.val[i];
         pAvrcpTg->charsetSuppByCt[i].valUpdated = TRUE;
         pAvrcpTg->lastCmdInfo.cmdId = AEEBTAVRCPTG_INFORM_CHARACTER_SET;
         pAvrcpTg->lastCmdInfo.txn = cmdInfo->cmdId.txn;
         evt.event = AEEBTAVRCPTG_INFORM_CHARACTER_SET;
         evt.evparam = 0;
         AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
         if ( pAvrcpTg->piSigAeeEvt )
         {
            ISignal_Set(pAvrcpTg->piSigAeeEvt);
         }
         charsetCount++;
      }   
   }

   if ( 0 == charsetCount )
   {
      ret = AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                       status );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdInformCharset",0,0,0);
      }

      return;
   } 
   else
   {
      evt.event = AEEBTAVRCPTG_UPDATE_LAST_CMD_STATUS;
      evt.evparam = 0;
      AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
      if ( pAvrcpTg->piSigAeeEvt )
      {
         ISignal_Set(pAvrcpTg->piSigAeeEvt);
      }
   }

}


//=============================================================================
//               AEEBTAVRCPTg_CmdInformBattStatus
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdInformBattStatus(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{  
   int                        ret = AEE_SUCCESS;  
   AEEBTAVRCPTgEventType      evt;
   AEEBTAVRCPTgCmdResponseStatus  status = AEEBTAVRCPTG_CMD_RESPONSE_REJECTED;

   MSG_MED("AEEBTAVRCP TG Ext: CmdInformBattStatus BattStatus = %x", 
           cmdInfo->pduInfo.ctBattStatus,0,0);

   MEMSET((void*)&evt,0,sizeof(AEEBTAVRCPTgEventType));


   if ( FALSE == pAvrcpTg->avrcpCtBatteryStatus.statUpdated )
   {
      pAvrcpTg->avrcpCtBatteryStatus.battStatus  = cmdInfo->pduInfo.ctBattStatus; 
      pAvrcpTg->avrcpCtBatteryStatus.statUpdated = TRUE;
      pAvrcpTg->lastCmdInfo.cmdId = AEEBTAVRCPTG_CT_BATT_STATUS_CHANGED;
      pAvrcpTg->lastCmdInfo.txn = cmdInfo->cmdId.txn;
      evt.event = AEEBTAVRCPTG_CT_BATT_STATUS_CHANGED;
      evt.evparam = 0;
      AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
      if ( pAvrcpTg->piSigAeeEvt )
      {
         ISignal_Set(pAvrcpTg->piSigAeeEvt);
      }
   } 
   else
   {
      ret = AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                       status );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: SendMetaReply",0,0,0);
      }

      return;
   }

   evt.event = AEEBTAVRCPTG_UPDATE_LAST_CMD_STATUS;
   evt.evparam = 0;
   AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
   if ( pAvrcpTg->piSigAeeEvt )
   {
      ISignal_Set(pAvrcpTg->piSigAeeEvt);
   }

}


//=============================================================================
//               AEEBTAVRCPTg_CmdGetElementAttr
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdGetElementAttr(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                       ret             = AEE_SUCCESS;
   uint8                     mediaAttrCount  = 0;
   uint32                    i               = 0; 
   uint32                    j               = 0;
   AEEBTAVRCPTgItemTxt       mediaInfo[AEEBTAVRCPTG_MAX_NUM_OF_VALS];
   boolean                   invalidTrackIndex = FALSE;

   MSG_MED("AEEBTAVRCP TG Ext: CmdGetElementAttr. ", 0,0,0);

   MEMSET((void*)mediaInfo, 0, AEEBTAVRCPTG_MAX_NUM_OF_VALS );
   MSG_MED("AEEBTAVRCP TG Ext: CmdGetElementAttr. mediaAttrcount = %x", 
           cmdInfo->pduInfo.pduData.count,0,0);

   for ( i = 0; i < AEEBTAVRCPTG_SIZE_OF_TRACK_INDEX; i++ )
   {
      if ( pAvrcpTg->playbackInfo.trackIndex[i] != cmdInfo->trackIndex[i] )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdGetElementAttr. Invalid track index",
                   0,0,0);
		 invalidTrackIndex = TRUE;
         break;
      }
   }

   if( ( AEEAVRCPTG_PLAYSTAT_STOPPED == pAvrcpTg->playbackInfo.playStatus ) ||
	   ( TRUE == invalidTrackIndex ) )
   {
         AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                    AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
         return;
   }
 
   for ( j = 0; j < cmdInfo->pduInfo.pduData.count ; j++ )
   {
      mediaInfo[mediaAttrCount].itemId  = cmdInfo->pduInfo.pduData.val[j];

      switch ( cmdInfo->pduInfo.pduData.val[j] )
      {
      case AEEBTAVRCPTG_MEDIA_ATTR_ILLEGAL:
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_TITLE:
         if ( NULL != pAvrcpTg->curMediaInfo.trackTitle )
         {
            mediaInfo[mediaAttrCount].charset = 
            pAvrcpTg->curMediaInfo.trackTitleCharset;
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.trackTitleLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.trackTitle;
         }
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_ARTIST:
         if ( NULL != pAvrcpTg->curMediaInfo.trackArtistName )
         {
            mediaInfo[mediaAttrCount].charset = 
            pAvrcpTg->curMediaInfo.trackArtistNameCharset;
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.trackArtistNameLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.trackArtistName;
         }
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_ALBUM:
         if ( NULL != pAvrcpTg->curMediaInfo.albumName )
         {
            mediaInfo[mediaAttrCount].charset = 
            pAvrcpTg->curMediaInfo.albumNameCharset;
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.albumNameLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.albumName;
         }
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_TRACK_NUMBER:
         if ( NULL != pAvrcpTg->curMediaInfo.trackNum )
         {
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.trackNumLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.trackNum;
         }
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_TOTAL_TRACK_NUMBER:
         if ( NULL != pAvrcpTg->curMediaInfo.trackTotal)
         {
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.trackTotalLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.trackTotal;
         }
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_GENRE:
         if ( NULL != pAvrcpTg->curMediaInfo.trackGenre)
         {
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.trackGenreLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.trackGenre;
            mediaInfo[mediaAttrCount].charset =
            pAvrcpTg->curMediaInfo.trackGenreCharset;
         }
         break;

      case AEEBTAVRCPTG_MEDIA_ATTR_PLAYING_TIME:
         if ( NULL != pAvrcpTg->curMediaInfo.playTime )
         {
            mediaInfo[mediaAttrCount].itemTxtLen = 
            pAvrcpTg->curMediaInfo.playTimeLen;
            mediaInfo[mediaAttrCount].itemTxt = 
            pAvrcpTg->curMediaInfo.playTime;
         }
         break;
      }

      if( AEEBTAVRCPTG_MEDIA_ATTR_ILLEGAL != cmdInfo->pduInfo.pduData.val[j] )
      {
         mediaAttrCount++;
      }
   }

   MSG_MED("AEEBTAVRCP TG Ext: CmdGetElementAttr. mediaAttrcount = %x", 
           mediaAttrCount,0,0); 

   if ( 0 != mediaAttrCount )
   {
      ret = OEMBTAVRCPTGExt_SendGetElementAttrReply(pAvrcpTg->pOemObj,
                                                    cmdInfo->cmdId.txn,
                                                    mediaAttrCount,
                                                    mediaInfo );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: GetElementAttrReply",0,0,0);
      }
   } 
   else
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
   }

}


//=============================================================================
//               AEEBTAVRCPTg_CmdGetPlayStatus
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdGetPlayStatus(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   uint32                    songLen         = 0;
   int                       ret             = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: CmdGetPlayStatus. ", 0,0,0);



   if ( NULL != pAvrcpTg->curMediaInfo.playTime )
   {
      songLen = (uint32) ATOI( (const char*)pAvrcpTg->curMediaInfo.playTime );

      ret = OEMBTAVRCPTGExt_SendPlayStatusReply(pAvrcpTg->pOemObj,
                                                cmdInfo->cmdId.txn,
                                                songLen,
                                                pAvrcpTg->playbackInfo.curSongPos,
                                                pAvrcpTg->playbackInfo.playStatus );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: GetPlayStatus",0,0,0);
      }
   } 
   else
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED ); 
   }

   return;
}


//=============================================================================
//               AEEBTAVRCPTg_CmdEvtNotif
//
//=============================================================================
static void
AEEBTAVRCPTg_CmdEvtNotif(AEEAVRCPTG_Priv_Ext *pAvrcpTg, AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   AEEBTAVRCPTgEventType          evt;
   uint32                         ret = AEE_SUCCESS;
   uint32                         i;
   uint32                         attrCount = 0;
   uint32                         attrInfo[AEEBTAVRCPTG_MAX_NUM_OF_VALS];
   uint32                         *attrs = NULL;

   MEMSET((void*)&evt,0,sizeof(AEEBTAVRCPTgEventType));



   if ( 0 == cmdInfo->pduInfo.notifyInfo.eventId )
   {
      AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                 AEEBTAVRCPTG_CMD_RESPONSE_REJECTED );
   } 
   else
   {
      if ( pAvrcpTg->mediaEventCapMask & cmdInfo->pduInfo.notifyInfo.eventId )
      {
         pAvrcpTg->pendingNotificationMask = pAvrcpTg->pendingNotificationMask |
                                             cmdInfo->pduInfo.notifyInfo.eventId;

         if ( AEEBTAVRCPTG_PLAYBACK_POS_CHANGED == cmdInfo->pduInfo.notifyInfo.eventId)
         {
            pAvrcpTg->songPosUpdate = cmdInfo->pduInfo.notifyInfo.playbackInterval;
            pAvrcpTg->startUpdate   = TRUE;
            evt.event = AEEBTAVRCPTG_SET_ATTR_CONFIG_UPDATES;
            evt.evparam = 0;
            AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
            if ( pAvrcpTg->piSigAeeEvt )
            {
               ISignal_Set(pAvrcpTg->piSigAeeEvt);
            }
         }

         pAvrcpTg->notifTxn = cmdInfo->cmdId.txn;

         if( AEEBTAVRCPTG_PLAYER_APP_SETTING_CHANGED == cmdInfo->pduInfo.notifyInfo.eventId )
         {
           for (i = 0; i < AEEBTAVRCPTG_MAX_NUM_OF_ATTR_SUPPORTED; i++ )
           {
              if ( ( 0 != pAvrcpTg->curPlayerAttrValues[i].attrId ) &&
                 ( 0 != pAvrcpTg->curPlayerAttrValues[i].valueId ) )
              {
                 attrInfo[2*attrCount] = pAvrcpTg->curPlayerAttrValues[i].attrId;
                 attrInfo[2*attrCount + 1 ] = pAvrcpTg->curPlayerAttrValues[i].valueId;
                 attrCount++;
              }
           }

           if( 0 != attrCount )
           {
              attrs = attrInfo;
           }
         }
         
         ret = OEMBTAVRCPTGExt_SendRegNotifReply(pAvrcpTg->pOemObj,
                                                 cmdInfo->cmdId.txn,
                                                 cmdInfo->pduInfo.notifyInfo.eventId,  
                                                 &pAvrcpTg->playbackInfo,
                                                 pAvrcpTg->avrcpTgBatteryStatus.battStatus,
                                                 pAvrcpTg->systemStatus,
                                                 FALSE,
                                                 attrs,
                                                 attrCount);
         if ( AEE_SUCCESS != ret )
         {
            MSG_ERROR("AEEBTAVRCP TG Ext: CmdEvtNotif",0,0,0);
         }
      } 
      else
      {
         AEEBTAVRCPTg_SendMetaReply(pAvrcpTg, cmdInfo,cmdInfo->cmdId.cmdPduId, 
                                    AEEBTAVRCPTG_CMD_RESPONSE_NOT_IMPLEMENTED );
      } 
   } 

}

//=============================================================================
//               AEEBTAVRCPTg_CmdGrpNavigation
//
//=============================================================================
static void 
AEEBTAVRCPTg_CmdGrpNavigation(AEEAVRCPTG_Priv_Ext *pAvrcpTg, 
                              AEEBTAVRCPTgExtCmdInfo *cmdInfo)
{
   int                            ret    = AEE_SUCCESS;
   AEEBTAVRCPTgEventType          evt;
   AEEBTAVRCPTgCmdResponseStatus  status = AEEBTAVRCPTG_CMD_RESPONSE_REJECTED;

   MSG_MED("AEEBTAVRCP TG Ext: CmdGrpNavigation cmd= %x, butt press status = %x ",
           cmdInfo->pduInfo.grpNavInfo.grpNavCmd,
           cmdInfo->pduInfo.grpNavInfo.buttPress,0);


   if ( FALSE == cmdInfo->pduInfo.grpNavInfo.buttPress )
   {
      MEMSET((void*)&evt,0,sizeof(AEEBTAVRCPTgEventType));

      if ( TRUE != pAvrcpTg->navigationCommand.statUpdated )
      {
         pAvrcpTg->navigationCommand.grpNav = cmdInfo->pduInfo.grpNavInfo.grpNavCmd ;
         pAvrcpTg->navigationCommand.statUpdated = TRUE;
         pAvrcpTg->navigationCommand.buttPress= cmdInfo->pduInfo.grpNavInfo.buttPress;
         pAvrcpTg->lastCmdInfo.cmdId = AEEBTAVRCPTG_GROUP_NAV_COMMAND;
         pAvrcpTg->lastCmdInfo.txn = cmdInfo->cmdId.txn;
         evt.event = AEEBTAVRCPTG_GROUP_NAV_COMMAND;
         evt.evparam = 0;
         AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
         if ( pAvrcpTg->piSigAeeEvt )
         {
            ISignal_Set(pAvrcpTg->piSigAeeEvt);
         }
      }
      else
      {
         ret = OEMBTAVRCPTGExt_SendGroupNavReply(pAvrcpTg->pOemObj,
                                                 cmdInfo->cmdId.txn,
                                                 cmdInfo->pduInfo.grpNavInfo.grpNavCmd,
                                                 cmdInfo->pduInfo.grpNavInfo.buttPress,
                                                 status );
         if ( AEE_SUCCESS != ret )
         {
            MSG_ERROR("AEEBTAVRCP TG Ext: CmdGrpNavigation",0,0,0);
         }

         return;
      } 

      evt.event = AEEBTAVRCPTG_UPDATE_LAST_CMD_STATUS;
      evt.evparam = 0;
      AEEBTEvtQ_Put(&pAvrcpTg->cmdAppAvrcpExtTgQueue, (uint8*)&evt);
      if ( pAvrcpTg->piSigAeeEvt )
      {
         ISignal_Set(pAvrcpTg->piSigAeeEvt);
      }
   }
   else
   {
      MSG_MED("AEEBTAVRCP TG Ext: CmdGrpNavigation cmd= %x, butt press status = %x ",
           cmdInfo->pduInfo.grpNavInfo.grpNavCmd,
           cmdInfo->pduInfo.grpNavInfo.buttPress,0);
      ret = OEMBTAVRCPTGExt_SendGroupNavReply(pAvrcpTg->pOemObj,
                                              cmdInfo->cmdId.txn,
                                              cmdInfo->pduInfo.grpNavInfo.grpNavCmd,
                                              cmdInfo->pduInfo.grpNavInfo.buttPress,
                                              AEEBTAVRCPTG_CMD_RESPONSE_ACCEPTED );
      if ( AEE_SUCCESS != ret )
      {
         MSG_ERROR("AEEBTAVRCP TG Ext: CmdGrpNavigation",0,0,0);
      }
     
   }

}
//=============================================================================
//               AEEBTAVRCPTg_SendMetaReply
//
//=============================================================================
static int AEEBTAVRCPTg_SendMetaReply(AEEAVRCPTG_Priv_Ext *pAvrcpTg, 
                                      AEEBTAVRCPTgExtCmdInfo *cmdInfo,
                                      AEEBTAVRCPTgPduId pduId, 
                                      AEEBTAVRCPTgCmdResponseStatus status)

{
   int   ret             = AEE_SUCCESS;

   MSG_MED("AEEBTAVRCP TG Ext: CmdGrpNavigation Pdu= %x, status = %x ",
           pduId, status, 0 );



   ret = OEMBTAVRCPTGExt_SendSimpleMetaReply(pAvrcpTg->pOemObj,
                                             cmdInfo->cmdId.txn,
                                             pduId,
                                             status );

   return ret;
}

//=============================================================================
// 
//
//=============================================================================
#endif /* FEATURE_BT_AVRCP_13 */
#endif /* defined(FEATURE_IBT_DYNAMIC) && defined(FEATURE_BT_EXTPF_AV) */
