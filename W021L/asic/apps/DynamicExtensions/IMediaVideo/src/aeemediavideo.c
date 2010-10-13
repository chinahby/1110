/*===========================================================================

FILE: aeemediavideo.c

SERVICES: video media playback functionality for IMediaVideo

DESCRIPTION
This file implements video media playback functionality that can be
used by IMedia-based classes.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*=========================================================================
                             Edit History

$Header: //depot/asic/msmshared/apps/DynamicExtensions/IMediaVideo/src/aeemediavideo.c#16 $
$DateTime: 2008/05/07 16:02:32 $
$Change: 655470 $

=========================================================================*/

//#if( !defined(AEE_SIMULATOR) || (defined(AEE_SIMULATOR) && defined(_DEBUG)) )
//#define  MV_DEBUG
//#endif /* AEE_SIMULATOR && _DEBUG */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEEStdLib.h"
#include "AEEShell.h"
#include "AEEMediaMPEG4.h"
#include "AEEIMediaVideo.h"
#include "aeemediavideo_priv.h"
#include "AEEMediaVideo.bid"
#include "AEEMediaVideoHandlers.bid"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/
#define MV_SCALING_STEP                   25

#define MV_DEFAULT_TICK_TIME              1000
//The default/minimum startup time and Rebuffering time defined in QTV
#define MV_HTTP_DEFAULT_STARTUP_TIME      4000
#define MV_HTTP_DEFAULT_REBUFFERING_TIME  4000

// 3 byte GPS latitude
#define MV_MAX_GPS_LATITUDE               3
// 3 byte GPS longitude
#define MV_MAX_GPS_LONGITUDE              3
// 3 byte GPS time
#define MV_MAX_GPS_TIME                   3

#define MV_MPEG4_PARM_COUNT               81


#define  MV_NO_ROTATION                   0    // not rotated
#define  MV_90_CW_ROTATION                90   // rotate 90 degrees clockwise
#define  MV_180_ROTATION                  180  // rotate 180 degrees counter clockwise
#define  MV_90_CCW_ROTATION               270  // rotate 270 (90 degrees counter clockwise)
#define  MV_360_CW_ROTATION               360  // rotate 360 degrees counter clockwise

// MediaVideoScalingType Enumeration
// note, not all scaling factors are supported by MDP and DSP,
// IPL can scale from 25 and up in step of 25.
#define  MV_4X_ZOOM                       400   // 4X scaling
#define  MV_2P5X_ZOOM                     250   // 2.5X scaling
#define  MV_2X_ZOOM                       200   // 2X scaling
#define  MV_1P25X_ZOOM                    125   // 1.25X scaling
#define  MV_NO_SCALING                    100   // 1X scaling
#define  MV_0P75X_SHRINK                  75    // 0.75X scaling
#define  MV_2X_SHRINK                     50    // 0.5X scaling
#define  MV_0P25X_SHRINK                  25    // 0.25X scaling
#define  MV_ASCALE                        -4    // ascale flag

typedef struct MVPlaybackPos
{
   uint32      dwVideoPos;    // video position in ms
   uint32      dwAudioPos;    // audio position in ms
   uint32      dwTextPos;     // text position in ms
   uint32      dwPlaybackPos; // max of the three positions in ms
} MVPlaybackPos;

// following telop text types should match the defines in OEMMediaMPEG4.h header
typedef struct MVTelopTextElement
{
  int32       dwTelopSize;
  int32       dwBeginTime;
  int32       dwDuration;
  int32       dwNumberOfSubStrings;
  boolean     bWrapFlag;
  int32       dwBackgroundColor;
  int32       dwDefaultFontColor;
} MVTelopTextElement;
// following telop text substring
typedef struct MVTelopTextSubString
{
  boolean     bLineFeed;
  boolean     bUnderline;
  boolean     bReversal;
  boolean     bLinking;
  int32       dwFontColor;
  int32       dwTextSampleSize;  // in bytes
  int32       dwLinkSize;
  char        *pTextSubString;
  char        *pLinkValue;
} MVTelopTextSubString;

/*===============================================================================

      External DEFINITIONS

===============================================================================*/
#ifndef AEE_SIMULATOR
// for floating initialization when use as dynamic module
 extern void _fp_init(void);
#endif /* AEE_SIMULATOR */
/*===============================================================================

      FUNCTION DECLARATIONS

===============================================================================*/
static void MediaVideo_Delete(MediaVideo * pme);
static int  MediaVideo_RegisterMediaCallback(MediaVideo* pme);
static void MediaVideo_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify);
static int  MediaVideo_ConvertMediaNotify(void *pUser, AEEMediaCmdNotify* pNotify, AEEMediaCmdNotify *pNotifyMPEG4);
static int  MediaVideo_HandleStatusDoneEvent(MediaVideo *pme, AEEMediaCmdNotify* pNotify);

static int MediaVideo_SetRotation(MediaVideo * pme, int nParmID, int32 p1, int32 p2);
static int MediaVideo_SetScaling(MediaVideo * pme, int nParmID, int32 p1, int32 p2);
static int MediaVideo_SetPrerollData(MediaVideo * pme, AEEMediaVideoData* pData);
static int MediaVideo_SetTransport(MediaVideo * pme, AEEMediaVideoData* pData);

static int MediaVideo_GetMediaAtom(MediaVideo * pme, AEEMediaVideoData* pData);
static int MediaVideo_GetMediaSpec(MediaVideo * pme, int type, int* ptr, uint32* pSize);
static int MediaVideo_GetFrameRate(MediaVideo* pme, double* rate);
static int MediaVideo_RequestClipInfo(MediaVideo *pme);

static MediaCodecType MediaVideo_ConvertCodecType(AEEMediaMP4CodecType type);
static boolean MediaVideo_CopyTimedText(MediaVideo *pme, AEE3GPPTimedText* pSrc);
static boolean MediaVideo_CopyTelopText(MediaVideo *pme, MVTelopTextElement* pSrc);
static int MediaVideo_GetClipDim(MediaVideo *pme, MVClipDim* pDim);
static int MediaVideo_GetSupportedScalingValue(MediaVideo *pme, int value);
static int MediaVideo_GetMDPScalingValue(MediaVideo *pme, int value);
static boolean MediaVideo_ConvertToMPEG4Scaling(int value, int* out);
static int MediaVideo_GetSupportedRotationValue(int value);
static int MediaVideo_ConvertRotationValue(int rotate, int* out);

static MVMessageDataType* IMediaVideo_MakePARMTypeTable(MediaVideo * pme);
static int32 MediaVideo_GetMessageType(MediaVideo *pme, int32 nMPEG4, int32 nMV);
/*===========================================================================

                      IMediaVideo DECLARATIONS

===========================================================================*/
static int     IMediaVideo_Create(IShell * ps, AEECLSID cls, IModule *po, void ** ppif);
static IMediaVideo * MediaVideo_MakeClassInstance(int16 nSize);

static uint32  MediaVideo_AddRef(IMediaVideo * po);
static uint32  MediaVideo_Release(IMediaVideo * po);
static int     MediaVideo_QueryInterface(IMediaVideo * po, AEEIID idReq, void ** ppo);
static int     MediaVideo_RegisterNotify(IMediaVideo * po, PFNMEDIANOTIFY pfnNotify, void * pUser);
static int     MediaVideo_SetMediaParm(IMediaVideo * po, int nParmID, int32 p1, int32 p2);
static int     MediaVideo_GetMediaParm(IMediaVideo * po, int nParmID, int32 * pP1, int32 *  pP2);
static int     MediaVideo_Play(IMediaVideo * po);
static int     MediaVideo_RecordUnsupported(IMediaVideo * po);
static int     MediaVideo_Stop(IMediaVideo * po);
static int     MediaVideo_Seek(IMediaVideo * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int     MediaVideo_Pause(IMediaVideo * po);
static int     MediaVideo_Resume(IMediaVideo * po);
static int     MediaVideo_GetTotalTime(IMediaVideo * po);
static int     MediaVideo_GetState(IMediaVideo * po, boolean * pbStateChanging);
/*==================================================================
  This function is called by AEEModGen when class is being created.
==================================================================*/
int AEEClsCreateInstance(AEECLSID ClsId, IShell * pIShell, IModule * po, void ** ppObj)
{
 *ppObj = NULL;

  switch(ClsId)
  {
    case AEECLSID_MediaVideo:
    case AEECLSID_MediaVideo3G2:
    case AEECLSID_MediaVideoWMA:
    case AEECLSID_MediaVideoURL:
    case AEECLSID_MediaVideoSKM:
    case AEECLSID_MediaVideoSDP:
    case AEECLSID_MediaVideoRM:
    case AEECLSID_MediaVideoRAM:
    case AEECLSID_MediaVideoRA:
    case AEECLSID_MediaVideoPVX:
    case AEECLSID_MediaVideoMWV:
    case AEECLSID_MediaVideoMP4:
    case AEECLSID_MediaVideoM4A:
    case AEECLSID_MediaVideoK3G:
    case AEECLSID_MediaVideoASF:
    case AEECLSID_MediaVideoAMC:
    case AEECLSID_MediaVideo3GPP:
    case AEECLSID_MediaVideo3GP:
    case AEECLSID_MediaVideoDCF:
      return IMediaVideo_Create(pIShell, ClsId, po, ppObj);
  }
  return ECLASSNOTSUPPORT;
}

/*==================================================================
  Creates an instance of the class and initializes it.
==================================================================*/
int IMediaVideo_Create(IShell * ps, AEECLSID cls, IModule *po, void ** ppif)
{
  MediaVideo *  pme;
  int           nRet = SUCCESS;

  if (!ppif)
  {
    return EBADPARM;
  }

#ifdef MV_DEBUG
  DBGPRINTF_LOW("MV: creating MediaVideo with calssID = 0x%X.", cls);
#endif//#ifdef MV_DEBUG

  pme = *ppif = MediaVideo_MakeClassInstance(sizeof(MediaVideo));
  if (!pme)
  {
    DBGPRINTF_ERROR("MV: call to MediaVideo_MakeClassInstance failed.");
    nRet = ENOMEMORY;
  }
  else
  {
    pme->m_pIMediaMPEG4 = NULL;
    // create MPEG4 instatance
    if(SUCCESS != (nRet = ISHELL_CreateInstance(ps, AEECLSID_MEDIAMPEG4, (void **)&pme->m_pIMediaMPEG4)))
    {
      DBGPRINTF_ERROR("MV: failed to create AEECLSID_MEDIAMPEG4");
      FREEIF(pme);
      return nRet;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: AEECLSID_MEDIAMPEG4 instance created, pMPEG4 = 0x%X", pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    pme->m_pIShell = ps;
    ISHELL_AddRef(ps);

    if(po)
    {
      // IModule is used for building as dynamic module
      pme->m_pIModule = po;
      IMODULE_AddRef(po);
    }
    else
      pme->m_pIModule = NULL;

    pme->m_nRefs = 1;
    pme->m_clsSelf = cls;

    pme->m_defaultTimeSetting.m_dwStartupTime   = MV_HTTP_DEFAULT_STARTUP_TIME;
    pme->m_defaultTimeSetting.m_dwBufferingTime = MV_HTTP_DEFAULT_REBUFFERING_TIME;
    pme->m_pSpec = NULL;
    pme->m_bSpec = FALSE;

    pme->m_pTimedText = NULL;
    pme->m_pTelopText = NULL;
    pme->m_bResume = FALSE;

    pme->m_VideoRotation  = MV_NO_ROTATION;
    pme->m_VideoScaling_H = MV_NO_SCALING;
    pme->m_VideoScaling_V = MV_NO_SCALING;

    pme->m_bRotationPending = FALSE;
    pme->m_nPendingRotation = MV_NO_ROTATION;
    pme->m_bScalingPending = FALSE;
    pme->m_PendingVideoScaling_H = MV_NO_SCALING;
    pme->m_PendingVideoScaling_V = MV_NO_SCALING;

    pme->m_pTypeTable_PARM = IMediaVideo_MakePARMTypeTable(pme);

    CALLBACK_Init(&pme->m_pcbExit,MediaVideo_Delete, (IMediaVideo*)pme);
    ISHELL_OnExit(ps, &pme->m_pcbExit);

    // request spec data
    (void)MediaVideo_RequestClipInfo(pme);

    if(pme->m_pIMediaMPEG4)
    {
      // Internally, QTV use 500ms as default tick time,
      // we need to set it to 1000ms, which is specified in AEEMedia.h and be used for cmx
      (void)IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_PARM_TICK_TIME, MV_DEFAULT_TICK_TIME, 0);
      // enable RTSP status callback
      (void)IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_RTSP_STATUS_NOTIFY, TRUE, 0);
    }
  }

  return nRet;
}
/*==================================================================
  This function allocate and initialize a message type comparison table
  The table can be used to convert PARM messages between MediaVideo
  and AEEMediaMPEG4
==================================================================*/
static MVMessageDataType* IMediaVideo_MakePARMTypeTable(MediaVideo * pme)
{
  MVMessageDataType* ptb;
  ptb = MALLOC(MV_MPEG4_PARM_COUNT*sizeof(MVMessageDataType));
  if(ptb)
  {
     MEMSET(ptb, 0, (MV_MPEG4_PARM_COUNT*sizeof(MVMessageDataType)));

     ptb[0].m_nMPEG4 = MM_MP4_PARM_ROTATION;                      ptb[0].m_nMV = MMV_PARM_ROTATION;
     ptb[1].m_nMPEG4 = MM_MP4_PARM_SCALING;                       ptb[1].m_nMV = MMV_PARM_SCALING;
     ptb[2].m_nMPEG4 = MM_MP4_PARM_TELOP_SUB_STR;                 ptb[2].m_nMV = 0;
     ptb[3].m_nMPEG4 = MM_MP4_PARM_BUFFER_UPDATE;                 ptb[3].m_nMV = 0;
     ptb[4].m_nMPEG4 = MM_MP4_PARM_CONNECT_UPDATE;                ptb[4].m_nMV = 0;
     ptb[5].m_nMPEG4 = MM_MP4_PARM_PB_READY;                      ptb[5].m_nMV = 0;
     ptb[6].m_nMPEG4 = MM_MP4_PARM_MEDIA_SPEC;                    ptb[6].m_nMV = MMV_PARM_SPEC;
     ptb[7].m_nMPEG4 = MM_MP4_STREAM_AUTHENTICATE;                ptb[7].m_nMV = 0;
     ptb[8].m_nMPEG4 = MM_MP4_AUDIO_VIDEO_MEDIA_DATA;             ptb[8].m_nMV = 0;
     ptb[9].m_nMPEG4 = MM_MP4_PARM_TRACK_TIME_SCALE;              ptb[9].m_nMV = MMV_PARM_TRACK_TIME_SCALE;
     ptb[10].m_nMPEG4 = MM_MP4_PARAM_AUDIO_OVERRIDE;              ptb[10].m_nMV = 0;
     ptb[11].m_nMPEG4 = MM_MP4_PARM_RTSP_STATUS_NOTIFY;           ptb[11].m_nMV = 0;
     ptb[12].m_nMPEG4 = MM_MP4_PARM_LOOP_TRACK;                   ptb[12].m_nMV = MMV_PARM_LOOP_TRACK;
     ptb[13].m_nMPEG4 = MM_MP4_PARAM_PLAYBACK_POS;                ptb[13].m_nMV = MMV_PARM_SPEC;
     ptb[14].m_nMPEG4 = MM_MP4_PARAM_PSBUF_WRITE_OFFSET;          ptb[14].m_nMV = 0;
     ptb[15].m_nMPEG4 = MM_MP4_PARM_PS_BUFFER_UPDATE;             ptb[15].m_nMV = 0;
     ptb[16].m_nMPEG4 = MM_MP4_SCALING_UPDATE;                    ptb[16].m_nMV = 0;
     ptb[17].m_nMPEG4 = MM_MP4_PARM_RTSP_STREAM_CONVERT;          ptb[17].m_nMV = MMV_PARM_FILE_SAVE_START;
     ptb[18].m_nMPEG4 = MM_MP4_PARM_MIDI_SIZE;                    ptb[18].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[19].m_nMPEG4 = MM_MP4_PARM_MIDI_DATA;                    ptb[19].m_nMV = MMV_PARM_ATOM;
     ptb[20].m_nMPEG4 = MM_MP4_PARM_PB_ABORT;                     ptb[20].m_nMV = 0;
     ptb[21].m_nMPEG4 = MM_MP4_PARM_RTSP_STOP_STR_CONVERT;        ptb[21].m_nMV = MMV_PARM_FILE_SAVE_STOP;
     ptb[22].m_nMPEG4 = MM_MP4_PARM_NET_QOS_POLICY;               ptb[22].m_nMV = MMV_TRANSPORT_NET_POLICY;
     ptb[23].m_nMPEG4 = MM_MP4_PARM_LINK_SIZE;                    ptb[23].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[24].m_nMPEG4 = MM_MP4_PARM_LINK_DATA;                    ptb[24].m_nMV = MMV_PARM_ATOM;
     ptb[25].m_nMPEG4 = MM_MP4_PARM_ATOM_FTYP_SIZE;               ptb[25].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[26].m_nMPEG4 = MM_MP4_PARM_ATOM_FTYP_DATA;               ptb[26].m_nMV = MMV_PARM_ATOM;
     ptb[27].m_nMPEG4 = MM_MP4_PARM_ATOM_DCMD_SIZE;               ptb[27].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[28].m_nMPEG4 = MM_MP4_PARM_ATOM_DCMD_DATA;               ptb[28].m_nMV = MMV_PARM_ATOM;
     ptb[29].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_CPRT_SIZE;          ptb[29].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[30].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_CPRT_DATA;          ptb[30].m_nMV = MMV_PARM_ATOM;
     ptb[31].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_AUTH_SIZE;          ptb[31].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[32].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_AUTH_DATA;          ptb[32].m_nMV = MMV_PARM_ATOM;
     ptb[33].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_TITL_SIZE;          ptb[33].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[34].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_TITL_DATA;          ptb[34].m_nMV = MMV_PARM_ATOM;
     ptb[35].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_DSCP_SIZE;          ptb[35].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[36].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_DSCP_DATA;          ptb[36].m_nMV = MMV_PARM_ATOM;
     ptb[37].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_SIZE;      ptb[37].m_nMV = MMV_PARM_SPEC_SIZE;
     ptb[38].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_DATA;      ptb[38].m_nMV = MMV_PARM_SPEC;
     ptb[39].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_SIZE;      ptb[39].m_nMV = MMV_PARM_SPEC_SIZE;
     ptb[40].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_DATA;      ptb[40].m_nMV = MMV_PARM_SPEC;
     ptb[41].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_WIDTH_SIZE;         ptb[41].m_nMV = MMV_PARM_SPEC_SIZE;
     ptb[42].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_WIDTH_DATA;         ptb[42].m_nMV = MMV_PARM_SPEC;
     ptb[43].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_HEIGHT_SIZE;        ptb[43].m_nMV = MMV_PARM_SPEC_SIZE;
     ptb[44].m_nMPEG4 = MM_MP4_PARM_TEXT_TKHD_HEIGHT_DATA;        ptb[44].m_nMV = MMV_PARM_SPEC;
     ptb[45].m_nMPEG4 = MM_MP4_PARM_HTTP_SETUP_TIME;              ptb[45].m_nMV = MMV_PREROLL_TIME;
     ptb[46].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_RTNG_SIZE;          ptb[46].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[47].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_RTNG_DATA;          ptb[47].m_nMV = MMV_PARM_ATOM;
     ptb[48].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_PERF_SIZE;          ptb[48].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[49].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_PERF_DATA;          ptb[49].m_nMV = MMV_PARM_ATOM;
     ptb[50].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_CLSF_SIZE;          ptb[50].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[51].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_CLSF_DATA;          ptb[51].m_nMV = MMV_PARM_ATOM;
     ptb[52].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_GNRE_SIZE;          ptb[52].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[53].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_GNRE_DATA;          ptb[53].m_nMV = MMV_PARM_ATOM;
     ptb[54].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_KYWD_SIZE;          ptb[54].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[55].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_KYWD_DATA;          ptb[55].m_nMV = MMV_PARM_ATOM;
     ptb[56].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_LOCI_SIZE;          ptb[56].m_nMV = MMV_PARM_ATOM_SIZE;
     ptb[57].m_nMPEG4 = MM_MP4_PARM_ATOM_UDTA_LOCI_DATA;          ptb[57].m_nMV = MMV_PARM_ATOM;
     ptb[58].m_nMPEG4 = MM_MP4_PARM_DECRYPT_KEY;                  ptb[58].m_nMV = 0;
     ptb[59].m_nMPEG4 = MM_MP4_PARM_MEDIA_INFO;                   ptb[59].m_nMV = 0;
     ptb[60].m_nMPEG4 = MM_MP4_PARAM_MODIFY_BUFFER_LOWMARK;       ptb[60].m_nMV = MMV_PREROLL_REBUFFER_PREROLL_TIME;
     ptb[61].m_nMPEG4 = MM_MP4_PARAM_SET_DATA_PORT_RANGE;         ptb[61].m_nMV = MMV_TRANSPORT_DATA_PORT_RANGE;
     ptb[62].m_nMPEG4 = MM_MP4_PARAM_COPY_BUFFER_TO_FILE;         ptb[62].m_nMV = MMV_PARM_FILE_SAVE_START;
     ptb[63].m_nMPEG4 = MM_MP4_PARM_VIDEO_CODEC;                  ptb[63].m_nMV = MMV_PARM_SPEC;
     ptb[64].m_nMPEG4 = MM_MP4_PARM_AUDIO_CODEC;                  ptb[64].m_nMV = MMV_PARM_SPEC;
     ptb[65].m_nMPEG4 = MM_MP4_PARM_VIDEO_FRAME_RATE;             ptb[65].m_nMV = MMV_PARM_SPEC;
     ptb[66].m_nMPEG4 = MM_MP4_PARM_VIDEO_AVERAGE_BIT_RATE;       ptb[66].m_nMV = MMV_PARM_SPEC;
     ptb[67].m_nMPEG4 = MM_MP4_PARM_AUDIO_AVERAGE_BIT_RATE;       ptb[67].m_nMV = MMV_PARM_SPEC;
     ptb[68].m_nMPEG4 = 0;                                        ptb[68].m_nMV = 0;
     ptb[69].m_nMPEG4 = MM_MP4_PARM_FRAME_EXT;                    ptb[69].m_nMV = 0;
     ptb[70].m_nMPEG4 = MM_MP4_PARM_USER_AGENT;                   ptb[70].m_nMV = 0;
     ptb[71].m_nMPEG4 = MM_MP4_PARAM_SET_PVX_SAVE_OPTION;         ptb[71].m_nMV = MMV_PARM_FILE_SAVE_START;
     ptb[72].m_nMPEG4 = MM_MP4_PARAM_SET_PVX_PREROLL;             ptb[72].m_nMV = MMV_PARM_PREROLL;
     ptb[73].m_nMPEG4 = MM_MP4_PARM_TOTAL_RX_DATA_RATE;           ptb[73].m_nMV = 0;
     ptb[74].m_nMPEG4 = MM_MP4_PARM_ASCALING;                     ptb[74].m_nMV = MMV_PARM_SCALING;
     ptb[75].m_nMPEG4 = MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA_SIZE;      ptb[75].m_nMV = 0;
     ptb[76].m_nMPEG4 = MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_DATA;           ptb[76].m_nMV = 0;
     ptb[77].m_nMPEG4 = MM_MP4_PARAM_HTTP_REGISTER_CALLBACK_BUFFER_SUPPORTED_TYPE; ptb[77].m_nMV = 0;
     ptb[78].m_nMPEG4 = MM_MP4_PARAM_HTTP_FILE_SAVE_OPTION;                        ptb[78].m_nMV = 0;
     ptb[79].m_nMPEG4 = MM_MP4_PARAM_HTTP_GET_ENCRYPTED_DATA;                      ptb[79].m_nMV = 0;
     ptb[80].m_nMPEG4 = MM_MP4_PARAM_HTTP_FREE_DOWNLOAD_BUFFER;                    ptb[80].m_nMV = 0;

     // make sure the largest idx should be smaller than MV_MPEG4_PARM_COUNT
  }
  else
    DBGPRINTF_ERROR("MV: Failed to allocate memory.");

  return ptb;
}

/*==================================================================
  This function allocates memory for vtable and initializes it
==================================================================*/
static IMediaVideo * MediaVideo_MakeClassInstance(int16 nSize)
{
  IMediaVideo  *pme;
  VTBL(IMediaVideo) *pVtbl;

  pme = MALLOC(nSize + sizeof(VTBL(IMediaVideo)));
  if (!pme)
  {
    DBGPRINTF_ERROR("MV: Failed to allocate memory.");
    return NULL;
  }

  (void)MEMSET(pme, 0, nSize + sizeof(VTBL(IMediaVideo)));

  pVtbl = (VTBL(IMediaVideo) *) ((byte *)pme + nSize);
  pVtbl->AddRef         = MediaVideo_AddRef;
  pVtbl->Release        = MediaVideo_Release;
  pVtbl->QueryInterface = MediaVideo_QueryInterface;
  pVtbl->RegisterNotify = MediaVideo_RegisterNotify;
  pVtbl->SetMediaParm   = MediaVideo_SetMediaParm;
  pVtbl->GetMediaParm   = MediaVideo_GetMediaParm;
  pVtbl->Play           = MediaVideo_Play;
  pVtbl->Record         = MediaVideo_RecordUnsupported;
  pVtbl->Stop           = MediaVideo_Stop;
  pVtbl->Seek           = MediaVideo_Seek;
  pVtbl->Pause          = MediaVideo_Pause;
  pVtbl->Resume         = MediaVideo_Resume;
  pVtbl->GetTotalTime   = MediaVideo_GetTotalTime;
  pVtbl->GetState       = MediaVideo_GetState;

  INIT_VTBL(pme, IMedia, *pVtbl);

  return (IMediaVideo *)pme;
}

/*==================================================================
  Increase reference count
==================================================================*/
static uint32 MediaVideo_AddRef(IMediaVideo * po)
{
   MediaVideo * pme = (MediaVideo *) po;

   return (++(pme->m_nRefs));
}

/*==================================================================
  Release reference count, delete when 0
==================================================================*/
static uint32 MediaVideo_Release(IMediaVideo * po)
{
  MediaVideo * pme = (MediaVideo *) po;
  if (!pme)
    return EFAILED;

  // return ref count, if lareger than 0
  if (--pme->m_nRefs > 0)
    return pme->m_nRefs;

  MediaVideo_Delete(pme);
  return SUCCESS;
}

/*==================================================================
This function cleans up all allocated memory and deletes MediaVideo object
==================================================================*/
static void MediaVideo_Delete(MediaVideo * pme)
{
  int i;
  CALLBACK_Cancel(&pme->m_pcbExit);
  if(pme)
  {
    MV_RELEASEIF(pme->m_pIMediaMPEG4);

    // free timedtime
    FREEIF(pme->m_pTimedText);
    // free telop text
    if(pme->m_pTelopText && pme->m_pTelopText->pSubString)
    {
      for(i=0; i < pme->m_pTelopText->nNumberOfSubStrings; i++)
      {
        if(&pme->m_pTelopText->pSubString[i])
        {
          FREEIF(pme->m_pTelopText->pSubString[i].pwszTextSubString);
          FREEIF(pme->m_pTelopText->pSubString[i].pwszLinkValue);
        }
      }
      FREEIF(pme->m_pTelopText->pSubString);
    }
    FREEIF(pme->m_pTelopText);

    if(pme->m_pIModule)
      IMODULE_Release(pme->m_pIModule);
    ISHELL_Release(pme->m_pIShell);

    FREEIF(pme->m_pSpec);
    FREEIF(pme->m_pTypeTable_PARM);
  }

  FREEIF(pme);
}


/*==================================================================
   This function retrieves a pointer to an interface conforming to
   the specified class ID.  This can be used to query for extended
   functionality, like future versions or proprietary extensions.
==================================================================*/
static int MediaVideo_QueryInterface(IMediaVideo * po, AEEIID idReq, void ** ppo)
{
  if (!po || !ppo)
  {
    DBGPRINTF_ERROR("MV: Bad input parm, NULL ptr.");
    return EBADPARM;
  }

  switch(idReq)
  {
    case AEECLSID_QUERYINTERFACE:
    case AEEIID_IMediaVideo:
    case AEEIID_IMedia:
       *ppo = (void *)po;
      MediaVideo_AddRef(po);
      return SUCCESS;
    default:
      break;
  }

  return ECLASSNOTSUPPORT;
}

/*==================================================================
  This function registers the callback function, called by client.
==================================================================*/
static int MediaVideo_RegisterNotify(IMediaVideo * po, PFNMEDIANOTIFY pfnNotify, void * pUser)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    pme->m_pfnNotify = pfnNotify;
    pme->m_pUser = (void *) pUser;

    // register callback to IMediaMPEG4 object
    return MediaVideo_RegisterMediaCallback(pme);
  }


  DBGPRINTF_ERROR("MV: Bad input parm, NULL ptr.");
  return EBADPARM;
}

/*==============================================================================
  called by client to set parameters
==============================================================================*/
int MediaVideo_SetMediaParm(IMediaVideo * po, int nParmID, int32 p1, int32 p2)
{
  if(po)
  {
    int nRet = EUNSUPPORTED;
    MediaVideo *pme = (MediaVideo *)po;

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: MediaVideo_SetMediaParm. po=0x%X, id = %d (0x%X)", po, nParmID, nParmID);
#endif //#ifdef MV_DEBUG

    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: NULL MPEG4 ptr.");
      return EFAILED;
    }

    switch(nParmID)
    {
      case MMV_PARM_ROTATION:
        nRet = MediaVideo_SetRotation(pme, nParmID, p1, p2);
        break;

      case MMV_PARM_SCALING:
        nRet = MediaVideo_SetScaling(pme, nParmID, p1, p2);
        break;

      case MMV_PARM_FILE_SAVE_START:
      {
        AEEMediaVideoFileSave* pfs;
        pfs = (AEEMediaVideoFileSave*)p1;
        if (pfs->bCopyBuffer)
        {
          nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4,
                                     MM_MP4_PARAM_COPY_BUFFER_TO_FILE,
                                     (int32)pfs->pszFileName, pfs->bOverWrite);
        }
        else
        {
          AEEStreamRecordInfo pInfo;
          pInfo.filename  = pfs->pszFileName;
          pInfo.overwrite = pfs->bOverWrite;
          pInfo.mode      = pfs->wMode;
          pInfo.duration  = pfs->nDuration;

          nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4,
                                     MM_MP4_PARM_RTSP_STREAM_CONVERT,
                                     (int32)&pInfo, 0);
        }
        break;
      }
      case MMV_PARM_FILE_SAVE_STOP:
        nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4,
                                   MM_MP4_PARM_RTSP_STOP_STR_CONVERT,
                                   0, 0);
        break;
      case MMV_PARM_PREROLL:
        nRet = MediaVideo_SetPrerollData(pme, (AEEMediaVideoData*) p1);
        break;

      case MMV_PARM_TRANSPORT:
        nRet = MediaVideo_SetTransport(pme, (AEEMediaVideoData*) p1);
        break;

      case MMV_PARM_FRAME_DATA:
        // this reserves for the future use
        break;

      case MMV_PARM_LOOP_TRACK:
        nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_LOOP_TRACK, p1, p2);
        break;

      default:
        nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, nParmID, p1, p2);
        break;
    }
    return nRet;

  }
  else
  {
    DBGPRINTF_ERROR("MV: NULL ptr on calling MediaVideo_SetMediaParm().");
  }
  return EBADPARM;
}

/*==================================================================
  This function set rotation to MPEG4, If the requested value is not supported,
  the next available one will be used

  p1 = int32,  requested rotation value in degrees
==================================================================*/
static int MediaVideo_SetRotation(MediaVideo * pme, int nParmID, int32 p1, int32 p2)
{
  int rotate;
  int32 mpeg4value = MV_NO_ROTATION;
  int nRet = EUNSUPPORTED;

  // give it a try on MPEG4, if this is not supported
  // find the next available value and try again
  nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ROTATION, p1, 0);
  if(nRet != EUNSUPPORTED && nRet != EBADPARM)
  {
    if(nRet == SUCCESS)
    {
      // successfully set rotation to MPEG4 layer, retrieve setting and save it
      if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ROTATION, &mpeg4value, 0))
      {
        pme->m_VideoRotation = mpeg4value;
      }
      else
      {
        // failed to retrieve, save the requested value by user
        pme->m_VideoRotation = p1;
      }
    }

    // process pending
    if(nRet == MM_PENDING)
    {
      pme->m_bRotationPending = TRUE;
      pme->m_nPendingRotation = p1;
    }
    return nRet;
  }

  // find the next available value and try again
  rotate = MediaVideo_GetSupportedRotationValue(p1);
  if(SUCCESS == (nRet = MediaVideo_ConvertRotationValue(rotate, (int*)&mpeg4value)))
  {
    if(SUCCESS == (nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ROTATION, mpeg4value, 0)))
    {
      // successfully set rotation to MPEG4 layer, retrieve setting
      if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ROTATION, &mpeg4value, 0))
      {
        // save setting
        pme->m_VideoRotation = mpeg4value;
      }
      else
      {
        // failed to retrieve, save the requested value by user
        pme->m_VideoRotation = rotate;
      }
    }
    else if(nRet == MM_PENDING)
    {
      pme->m_bRotationPending = TRUE;
      pme->m_nPendingRotation = rotate;
    }
    else if(nRet == EALREADY)
    {
      DBGPRINTF_ERROR("MV: Rotation in processing, request dropped %d.", nRet);
    }
    else if(nRet == EUNSUPPORTED)
    {
      DBGPRINTF_ERROR("MV: Set rotation return EUNSUPPORTED");
    }
    else
    {
      DBGPRINTF_ERROR("MV: Rotation failed, %d.", nRet);
    }
  }
  else
  {
    DBGPRINTF_ERROR("MV: MediaVideo_ConvertRotationValue() failed %d.", nRet);
  }
  return nRet;
}
/*==================================================================
  This function set scaling to MPEG4. If the requested value is not supported,
  the next available one will be used. When zooming in, the next value will be
  the one larger, otherwise, it will be the one smaller.

  p1 = int32,  requested scaling value for width in %, original size = 100
  p2 = int32,  requested scaling value for height in %, original size = 100
==================================================================*/
static int MediaVideo_SetScaling(MediaVideo * pme, int nParmID, int32 p1, int32 p2)
{
  MVClipDim cliDim;
  int nRet = EUNSUPPORTED;

  if(!pme->m_bScalingPending && SUCCESS == MediaVideo_GetClipDim(pme, &cliDim))
  {
    // Try AScale first
    if(p1 && p2)
    {
      // Ascaling uses actual width and height values in stead of scaling factor,
      // convert user's zoom ratial to width and height
      int32 width  = (cliDim.m_nWidth*p1)/100;
      int32 height = (cliDim.m_nHeight*p2)/100;

      // set ascale
      nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ASCALING, width, height);
      if(nRet != EUNSUPPORTED && nRet != EBADPARM)
      {
        int32 factor_x;
        int32 factor_y;
        // successfully set scaling to MPEG4 layer, retrieve and save current scaling factors
        if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ASCALING, &factor_x, &factor_y))
        {
          // save settings, we need to convert back to the scaling ratio from
          // width and height
          pme->m_VideoScaling_H = factor_x*100/cliDim.m_nWidth;
          pme->m_VideoScaling_V = factor_y*100/cliDim.m_nHeight;
        }
        else
        {
          //failed to retrieve, save the requested values by user
          pme->m_VideoScaling_H = width*100/cliDim.m_nWidth;
          pme->m_VideoScaling_V = height*100/cliDim.m_nHeight;
        }
        return nRet;
      }
      else if(nRet == MM_PENDING)
      {
        pme->m_bScalingPending = TRUE;
        pme->m_PendingVideoScaling_H = width*100/cliDim.m_nWidth;
        pme->m_PendingVideoScaling_V = height*100/cliDim.m_nHeight;

        DBGPRINTF_LOW("MV: Ascale Zoom pending...");
        return nRet;
      }

      // failed on trying for scale, now use regular scaling
      if(nRet == EUNSUPPORTED || nRet == EBADPARM)
      {
        int32 scaling;
        int scaling_factor;

        // take the smaller one of p1 and p2 as scaling factor
        scaling_factor = MIN(p1, p2);
        // try set it to MPEG4 now
        nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_SCALING, scaling_factor, 0);
        if(nRet != EUNSUPPORTED && nRet != EBADPARM)
        {
          if(nRet == SUCCESS)
          {
            // successfully set scaling to MPEG4 layer, retrieve setting now
            if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_SCALING, &scaling, 0))
            {
              // retrieve and save current scaling factors
              pme->m_VideoScaling_H = scaling;
              pme->m_VideoScaling_V = scaling;
            }
            else
            {
              // save the requested value by user
              pme->m_VideoScaling_H = scaling_factor;
              pme->m_VideoScaling_V = scaling_factor;
            }
          }
          else if(nRet == MM_PENDING)
          {
            pme->m_bScalingPending = TRUE;
            pme->m_PendingVideoScaling_H = scaling_factor;
            pme->m_PendingVideoScaling_V = scaling_factor;
          }
          return nRet;
        }

        // failed on setting user's value into MPEG4, now get the next one and try it again
        scaling_factor = MediaVideo_GetSupportedScalingValue(pme, scaling_factor);
        scaling_factor = MediaVideo_GetMDPScalingValue(pme, scaling_factor);

        if(MediaVideo_ConvertToMPEG4Scaling(scaling_factor, (int*)&scaling))
        {
          if(SUCCESS == (nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_SCALING, scaling, 0)))
          {
            // successfully set rotation to MPEG4 layer, retrieve setting now
            if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_SCALING, &scaling, 0))
            {
              // retrieve and save current scaling factors
              pme->m_VideoScaling_H = scaling;
              pme->m_VideoScaling_V = scaling;
            }
            else
            {
              // save the requested value by user
              pme->m_VideoScaling_H = scaling_factor;
              pme->m_VideoScaling_V = scaling_factor;
            }
          }
          else if(nRet == MM_PENDING)
          {
            pme->m_bScalingPending = TRUE;
            pme->m_PendingVideoScaling_H = scaling_factor;
            pme->m_PendingVideoScaling_V = scaling_factor;
          }
          else if(nRet == EUNSUPPORTED)
          {
            DBGPRINTF_ERROR("MV: Set scaling return EUNSUPPORTED");
          }
          else
          {
            DBGPRINTF_ERROR("MV: Set scaling return error. %d", nRet);
          }
        }
        else
          DBGPRINTF_ERROR("MV: unable to convert scaling factor %d.", scaling_factor);
      }
    }
    else
    {
      nRet = EBADPARM;
      DBGPRINTF_ERROR("MV: set scaling with bad input %d, %d.", p1, p2);
    }
  }
  else
  {
    if(pme->m_bScalingPending)
      DBGPRINTF_LOW("MV: scaling pending...");
    else
      DBGPRINTF_LOW("MV: No clip info, cannot perform zoom.");
  }

  return nRet;
}

/*==================================================================
  This function set preroll data into MPEG4
==================================================================*/

static int MediaVideo_SetPrerollData(MediaVideo * pme, AEEMediaVideoData* pData)
{
  int nRet = EUNSUPPORTED;

  switch(pData->dwType)
  {
    case MMV_PREROLL_TIME:
    {
      pme->m_defaultTimeSetting.m_dwStartupTime = (uint32)pData->pData;
      nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4,
                                 MM_MP4_PARM_HTTP_SETUP_TIME,
                                 (int32)&pme->m_defaultTimeSetting, 0);
      break;
    }

    case MMV_PREROLL_REBUFFER_PREROLL_TIME:
      pme->m_defaultTimeSetting.m_dwBufferingTime = (uint32)pData->pData;

      nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4,
                                 MM_MP4_PARM_HTTP_SETUP_TIME,
                                 (int32)&pme->m_defaultTimeSetting, 0);
      break;
    case MMV_PREROLL_SIZE:
      break;

    case MMV_PREROLL_REBUFFER_PREROLL_SIZE:
      break;

    default:
      break;
  }
  return nRet;
}

/*==================================================================
  This function set transport data into MPEG4
==================================================================*/
static int MediaVideo_SetTransport(MediaVideo * pme, AEEMediaVideoData* pData)
{
  int nRet = EUNSUPPORTED;

  switch(pData->dwType)
  {
    case MMV_TRANSPORT_DATA_PORT_RANGE:
    {
      AEEMediaVideoDataPort* pPort = (AEEMediaVideoDataPort*)pData->pData;
      nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4,
                                 MM_MP4_PARAM_SET_DATA_PORT_RANGE,
                                 pPort->dwBeginPort, pPort->dwEndPort);
      break;
    }

    case MMV_TRANSPORT_NET_POLICY:
    {
      AEEMediaVideoNetPolicyInfo *pinfo = (AEEMediaVideoNetPolicyInfo*)pData->pData;
      AEENetPolicyInfo netPolicyInfo;

      netPolicyInfo.size                =  sizeof(AEEMediaVideoNetPolicyInfo);
      netPolicyInfo.isSet               =  TRUE;
      netPolicyInfo.isQoSEnabled        =  pinfo->bQoSEnable;
      netPolicyInfo.primaryPDPProfileNo =  pinfo->wPrimaryPDPProfileNo;

#ifdef MV_DEBUG
      DBGPRINTF_HIGH("MV: QosEnabled = %d, PDP = %d.", netPolicyInfo.isQoSEnabled, netPolicyInfo.primaryPDPProfileNo);
#endif //#ifdef MV_DEBUG
      nRet = IMEDIA_SetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_NET_QOS_POLICY, (int32)&netPolicyInfo, 0);
      break;
    }

    default:
      break;
  }
  return nRet;
}

/*==============================================================================
  this function is called by client to get parameters
==============================================================================*/
int MediaVideo_GetMediaParm(IMediaVideo * po, int nParmID, int32 * p1, int32 * p2)
{
  if(po)
  {
    int nRet = EUNSUPPORTED;
    MediaVideo * pme = (MediaVideo*)po;
    IMedia* pMedia = pme->m_pIMediaMPEG4;

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: MediaVideo_GetMediaParm. po=0x%X, id = %d (0x%X)", po, nParmID, nParmID);
#endif //#ifdef MV_DEBUG

    if(!pme->m_pIMediaMPEG4)
      return EFAILED;

    switch(nParmID)
    {
      case MMV_PARM_ROTATION:
        *p1 = pme->m_VideoRotation;
        nRet = SUCCESS;
        break;
      case MMV_PARM_SCALING:
        *p1 = pme->m_VideoScaling_H;
        *p2 = pme->m_VideoScaling_V;
        nRet = SUCCESS;
        break;
      case MMV_PARM_TRACK_TIME_SCALE:
      {
        int32 nTrackType = (int32)*p1;
        // convert to MPEG4 track type
        switch(nTrackType)
        {
          case MM_CAPS_VIDEO:
            nTrackType = VIDEO_TRACK;
            nRet = SUCCESS;
            break;
          case MM_CAPS_AUDIO:
            nTrackType = AUDIO_TRACK;
            nRet = SUCCESS;
            break;
          case MM_CAPS_TEXT:
            nTrackType = TEXT_TRACK;
            nRet = SUCCESS;
            break;
          case MMV_CAPS_INVALID:
          case MMV_CAPS_STILL_IMAGE:
          case MMV_CAPS_VIDEO_BCAST_FLO:
          default:
            DBGPRINTF_ERROR("MV: MediaVideo_GetMediaParm() with unsupported inputs, nParmID = %d, p1 = %d.",
                            nParmID, nTrackType);
            break;
        }

        if(nRet == SUCCESS)
          nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TRACK_TIME_SCALE, (int32 *)nTrackType, p2);
        break;
      }
      case MMV_PARM_PLAYBACK_VIDEO_POS:
      {
        MVPlaybackPos pos;
        if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARAM_PLAYBACK_POS, (int32 *)(&pos), 0)))
          *p1 = pos.dwVideoPos;
        break;
      }
      case MMV_PARM_PLAYBACK_AUDIO_POS:
      {
        MVPlaybackPos pos;
        if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARAM_PLAYBACK_POS, (int32 *)(&pos), 0)))
          *p1 = pos.dwAudioPos;
        break;
      }
      case MMV_PARM_PLAYBACK_TEXT_POS:
      {
        MVPlaybackPos pos;
        if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARAM_PLAYBACK_POS, (int32 *)(&pos), 0)))
          *p1 = pos.dwTextPos;
        break;
      }
      case MMV_PARM_ATOM_SIZE:
      case MMV_PARM_ATOM:
        nRet = MediaVideo_GetMediaAtom(pme, (AEEMediaVideoData*) p1);
        break;

      case MMV_PARM_SPEC_SIZE:
      case MMV_PARM_SPEC:
      {
        AEEMediaVideoData* pData = (AEEMediaVideoData*)p1;
        nRet = MediaVideo_GetMediaSpec(pme, pData->dwType, pData->pData, &pData->dwSize);
        break;
      }
      case MMV_PARM_FRAME_DATA:
        DBGPRINTF_ERROR("MV: MMV_PARM_FRAME_DATA unsupported.");
        break;

      case MM_PARM_CLSID:
         if (p1 == NULL)
           return EBADPARM;
         *p1 = pme->m_clsSelf;
         nRet = SUCCESS;
         break;

      default:
        nRet = IMEDIA_GetMediaParm(pMedia, nParmID, p1, p2);
        break;
    }
    return nRet;
  }
  else
  {
    DBGPRINTF_ERROR("MV: NULL ptr on calling MediaVideo_GetMediaParm().");
  }
  return EBADPARM;
}

/*==============================================================================
  this function retrieve clip atom information
==============================================================================*/
static int MediaVideo_GetMediaAtom(MediaVideo * pme, AEEMediaVideoData* pData)
{
  int nRet = EUNSUPPORTED;
  IMedia* pMedia = pme->m_pIMediaMPEG4;

  switch(pData->dwType)
  {
    case MMV_ATOM_MIDI:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_MIDI_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_MIDI_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }

    case MMV_ATOM_LINK:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_LINK_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_LINK_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }

    case MMV_ATOM_FTYP:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_FTYP_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_FTYP_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }
    case MMV_ATOM_DCMD:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_DCMD_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_DCMD_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }
    case MMV_ATOM_UDTA_CPRT:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_CPRT_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_CPRT_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }
    case MMV_ATOM_UDTA_AUTH:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_AUTH_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_AUTH_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }
    case MMV_ATOM_UDTA_TITL:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_TITL_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_TITL_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }
    case MMV_ATOM_UDTA_DSCP:
    {
      if (pData->pData == NULL)
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_DSCP_SIZE, (int32 *)&pData->dwSize, (int32 *)0);
      else
        nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_ATOM_UDTA_DSCP_DATA, (int32 *)pData->pData, (int32 *)&pData->dwSize);
      break;
    }
    default:
      DBGPRINTF_ERROR("MV: Unknown ATOM type = %d.", pData->dwType);
      break;
  }
  return nRet;
}

/*==============================================================================
  this function retrieve clip SPEC information
==============================================================================*/
int MediaVideo_GetMediaSpec(MediaVideo * pme, int type, int* ptr, uint32* pSize)
{
  int nRet = EUNSUPPORTED;
  IMedia* pMedia = pme->m_pIMediaMPEG4;

  if(!pme->m_bSpec)
  {
    nRet = MediaVideo_RequestClipInfo(pme);
    if(nRet == MM_PENDING)
    {
      // user needs to ask latter.
      DBGPRINTF_HIGH("MV: get SPEC pending.");
      return nRet;
    }

    if(!pme->m_bSpec)
    {
      DBGPRINTF_HIGH("MV: SPEC is not available.");
      return nRet;
    }
  }

  switch(type)
  {
    case MMV_SPEC_DURATION:
      if(ptr)
      {
        *ptr = pme->m_pSpec->dwDuration;
      }
      *pSize = sizeof(pme->m_pSpec->dwDuration);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_FILE_SIZE:
      if(ptr)
      {
        *ptr = pme->m_pSpec->dwSize;
      }
      *pSize = sizeof(pme->m_pSpec->dwSize);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_STREAM_TYPE:
    {
      MediaStreamType streamTytpe;
      // convert from AEEMediaMPEG4 to AEEMediaVideo streaming type
      switch(pme->m_pSpec->streamtype)
      {
        case MM_MEDIA_NON_STREAMING:
          streamTytpe = MMV_STREAMING_LOCAL;
          break;
        case MM_MEDIA_STREAMING:
          // we can't identify the type, return RTSP as default
          streamTytpe = MMV_STREAMING_RTSP;
          break;
        case MM_MEDIA_LIVE_STREAMING:
          streamTytpe = MMV_STREAMING_LIVE;
          break;
        default:
          streamTytpe = MMV_STREAMING_UNKNOWN;
          DBGPRINTF_ERROR("MV: Unknown stream type = %d.", pme->m_pSpec->streamtype);
          break;
      }

      if(ptr)
      {
        *ptr = streamTytpe;
      }
      *pSize  = sizeof(MediaStreamType);
      nRet = SUCCESS;
      break;
    }

    case MMV_SPEC_AUDIO_CODEC_TYPE:
    {
      if(ptr)
      {
        if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_AUDIO_CODEC, (int32 *)ptr, (int32 *)0)))
        {
          // convert from AEEMediaMPEG4 to AEEMediaVideo codec type
          *ptr = (MediaCodecType)MediaVideo_ConvertCodecType((AEEMediaMP4CodecType)*ptr);
          *pSize = sizeof(MediaCodecType);
        }
        else
        {
          DBGPRINTF_ERROR("MV: failed to get CODEC, nRet = %d.", nRet);
          break;
        }
      }

      *pSize = sizeof(MediaCodecType);
      nRet = SUCCESS;
      break;
    }

    case MMV_SPEC_VIDEO_CODEC_TYPE:
    {
      if(ptr)
      {
        if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_VIDEO_CODEC, (int32 *)ptr, (int32 *)0)))
        {
          // convert from AEEMediaMPEG4 to AEEMediaVideo codec type
          *ptr = (MediaCodecType)MediaVideo_ConvertCodecType((AEEMediaMP4CodecType)*ptr);
          *pSize = sizeof(MediaCodecType);
        }
        else
        {
          DBGPRINTF_ERROR("MV: failed to get CODEC, nRet = %d.", nRet);
          break;
        }
      }

      *pSize = sizeof(MediaCodecType);
      nRet = SUCCESS;
      break;
    }
    case MMV_SPEC_AUDIO_BITRATE:
    {
      if(ptr)
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_AUDIO_AVERAGE_BIT_RATE, (int32 *)ptr, (int32 *)0)))
        {
          DBGPRINTF_ERROR("MV: failed to get AUDIO_AVERAGE_BIT_RATE, nRet = %d.", nRet);
          break;
        }
      }
      *pSize = sizeof(int32);
      nRet = SUCCESS;
      break;
    }

    case MMV_SPEC_VIDEO_BITRATE:
    {
      if(ptr)
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_VIDEO_AVERAGE_BIT_RATE, (int32 *)ptr, (int32 *)0)))
        {
          DBGPRINTF_ERROR("MV: failed to get VIDEO_AVERAGE_BIT_RATE, nRet = %d.", nRet);
          break;
        }
      }
      *pSize = sizeof(int32);
      nRet = SUCCESS;
      break;
    }

    case MMV_SPEC_VIDEO_FRAMERATE:
    {
      if(ptr)
      {
        if(SUCCESS != (nRet = MediaVideo_GetFrameRate(pme, (double*) ptr)))
        {
          break;
        }
      }
      *pSize = sizeof(double);
      nRet = SUCCESS;
      break;
    }

    case MMV_SPEC_AUDIO_ONLY_FLAG:
      if(ptr)
      {
        *ptr = pme->m_pSpec->bAudioOnlyPlaybackAllowed;
      }
      *pSize = sizeof(pme->m_pSpec->bAudioOnlyPlaybackAllowed);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_VIDEO_ONLY_FLAG:
      if(ptr)
      {
        *ptr = pme->m_pSpec->bVideoOnlyPlaybackAllowed;
      }
      *pSize = sizeof(pme->m_pSpec->bVideoOnlyPlaybackAllowed);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_REPOSITION_FLAG:
      if(ptr)
      {
        *ptr = pme->m_pSpec->bRepositioningAllowed;
      }
      *pSize = sizeof(pme->m_pSpec->bRepositioningAllowed);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_VIDEO_FRAME_SIZE_WIDTH:
      if(ptr)
      {
        *ptr = pme->m_pSpec->dwWidth;
      }
      *pSize = sizeof(pme->m_pSpec->dwWidth);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_VIDEO_FRAME_SIZE_HEIGHT:
      if(ptr)
      {
        *ptr = pme->m_pSpec->dwHeight;
      }
      *pSize = sizeof(pme->m_pSpec->dwHeight);
      nRet = SUCCESS;
      break;

    case MMV_SPEC_TEXT_TKHD_ORIGIN_X:
      if (ptr)
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_DATA, (int32 *)ptr, (int32 *)pSize)))
        {
          DBGPRINTF_ERROR("MV: failed to get ORIGIN_X_DATA, nRet = %d.", nRet);
        }
      }
      else
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_ORIGIN_X_SIZE, (int32 *)pSize, (int32 *)0)))
        {
          DBGPRINTF_ERROR("MV: failed to get ORIGIN_X_SIZE, nRet = %d.", nRet);
        }
      }
      break;

    case MMV_SPEC_TEXT_TKHD_ORIGIN_Y:
      if (ptr)
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_DATA, (int32 *)ptr, (int32 *)pSize)))
        {
          DBGPRINTF_ERROR("MV: failed to get ORIGIN_Y_DATA, nRet = %d.", nRet);
        }
      }
      else
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_ORIGIN_Y_SIZE, (int32 *)pSize, (int32 *)0)))
        {
          DBGPRINTF_ERROR("MV: failed to get ORIGIN_Y_SIZE, nRet = %d.", nRet);
        }
      }
      break;

    case MMV_SPEC_TEXT_TKHD_WIDTH:
      if (ptr)
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_WIDTH_DATA, (int32 *)ptr, (int32 *)pSize)))
        {
          DBGPRINTF_ERROR("MV: failed to get WIDTH_DATA, nRet = %d.", nRet);
        }
      }
      else
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_WIDTH_SIZE, (int32 *)pSize, (int32 *)0)))
        {
          DBGPRINTF_ERROR("MV: failed to get WIDTH_SIZE, nRet = %d.", nRet);
        }
      }
      break;

    case MMV_SPEC_TEXT_TKHD_HEIGHT:
      if (ptr)
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_HEIGHT_DATA, (int32 *)ptr, (int32 *)pSize)))
        {
          DBGPRINTF_ERROR("MV: failed to get HEIGHT_DATA, nRet = %d.", nRet);
        }
      }
      else
      {
        if(SUCCESS != (nRet = IMEDIA_GetMediaParm(pMedia, MM_MP4_PARM_TEXT_TKHD_HEIGHT_SIZE, (int32 *)pSize, (int32 *)0)))
        {
          DBGPRINTF_ERROR("MV: failed to get HEIGHT_SIZE, nRet = %d.", nRet);
        }
      }
      break;

    case MMV_SPEC_PURGE_DOWNLOAD_FILE:
      if(pme->m_pSpec->eFormatType == MM_PVX_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.PvxSpec.bPurgeDowloadedFile;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.PvxSpec.bPurgeDowloadedFile);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_ALLOW_USER_PLAYBACK:
      if(pme->m_pSpec->eFormatType == MM_PVX_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.PvxSpec.bAllowUserPlayback;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.PvxSpec.bAllowUserPlayback);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_DOWNLOAD_PLAYBACK_CONTROL:
      {
        boolean bSupport = TRUE;
        if(pme->m_pSpec->eFormatType == MM_PVX_FORMAT_TYPE)
        {
          if(ptr)
          {
            switch(pme->m_pSpec->ExtendedInfo.PvxSpec.downloadPlaybackControl)
            {
              case MM_NO_PLAYBACK:
                *ptr = MMV_DPC_NO_PLAY;
                break;
              case MM_PLAYBACK_AFTER_DOWNLOAD:
                *ptr = MM_PLAYBACK_AFTER_DOWNLOAD;
                break;
              case MM_PLAYBACK_ASAP:
                *ptr = MM_PLAYBACK_ASAP;
                break;
              default:
                nRet = EUNSUPPORTED;
                bSupport = FALSE;
                break;
            }
          }
          if(bSupport)
          {
            *pSize = sizeof(pme->m_pSpec->ExtendedInfo.PvxSpec.downloadPlaybackControl);
            nRet = SUCCESS;
          }
        }
      }
      break;

    case MMV_SPEC_TITLE_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szTitle);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szTitle, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szTitle);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szTitle, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szTitle);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szTitle, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            break;
        }
      }
      break;

    case MMV_SPEC_AUTHOR_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szAuthor);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szAuthor, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szAuthor);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szAuthor, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szAuthor);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szAuthor, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            break;
        }
      }
      break;

    case MMV_SPEC_COPYRIGHT_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szCopyright);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szCopyright, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szCopyright);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szCopyright, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szCopyright);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szCopyright, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            break;
        }
      }
      break;

    case MMV_SPEC_RATING_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szRating);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szRating, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szRating);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szRating, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            break;
        }
      }
      break;

    case MMV_SPEC_DESCRIPTION_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szDescription);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szDescription, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szMemo);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szMemo, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szDescription);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szDescription, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;

    case MMV_SPEC_OWNER_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szOwner);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szOwner, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szOwner);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szOwner, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;

    case MMV_SPEC_VERSION_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szVersion);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szVersion, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szVersion);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szVersion, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;

    case MMV_SPEC_CREATIONDATE_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szCreationDate);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szCreationDate, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szCreationDate);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szCreationDate, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;

    case MMV_SPEC_ARTIST_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szArtist);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szArtist, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szArtist);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szArtist, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;

    case MMV_SPEC_ALBUM_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szAlbum);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szAlbum, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szAlbum);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szAlbum, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;

    case MMV_SPEC_INFO_STRING:
      {
        uint32 nSize = *pSize;
        switch(pme->m_pSpec->eFormatType)
        {
          case MM_MPEG4_STD_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.StdSpec.szInfo);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.StdSpec.szInfo, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_PVX_FORMAT_TYPE:
            nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szInfo);
            if (nSize >= 1)
               nSize = nSize + 1;
            if(ptr != NULL)
            {
              *pSize = MIN(nSize, *pSize);
              (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.PvxSpec.StdSpec.szInfo, *pSize);
            }
            else
              *pSize = nSize;
            nRet = SUCCESS;
            break;

          case MM_MPEG4_UUID_FORMAT_TYPE:
          case MM_MPEG4_UNKNOWN_FORMAT_TYPE:
          default:
            DBGPRINTF_ERROR("MV: Unknown MPEG4 format type = %d.", pme->m_pSpec->eFormatType);
            break;
        }
      }
      break;
    case MMV_SPEC_KEYWORD_STRING: //unsupported?
      break;

    case MMV_SPEC_COPYPROHIBITION_FLAG:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwCopyProhibitionFlag;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwCopyProhibitionFlag);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_VALIDITY_EFFECTIVE_DATE:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwValidityEffectiveDate;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwValidityEffectiveDate);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_VALIDITY_PERIOD:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwValidityPeriod;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwValidityPeriod);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_ALLOWED_PLAYBACKS:
     if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
     {
       if(ptr)
       {
        *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwNumberOfAllowedPlayBacks;
       }
       *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwNumberOfAllowedPlayBacks);
        nRet = SUCCESS;

      }
      break;

    case MMV_SPEC_GENERATION_DLL_VERSION:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGenerationDLLVersion;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGenerationDLLVersion);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_EDIT_FLAG:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwEditFlags;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwEditFlags);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_RECORDING_MODE:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwRecordingMode;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwRecordingMode);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_RECORDING_DATE:
      if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        if(ptr)
        {
          *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwRecordingDate;
        }
        *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwRecordingDate);
        nRet = SUCCESS;
      }
      break;

    case MMV_SPEC_DEVICE_NAME_STRING:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szDeviceName);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szDeviceName, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_MODEL_NAME_STRING:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szModelName);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szModelName, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_ENCODER_INFORMATION_STRING:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szEncoderInformation);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szEncoderInformation, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_MUX_INFORMATION_STRING:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szMuxInformation);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szMuxInformation, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_BYTE_ORDER:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.wGPSByteOrder;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.wGPSByteOrder);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_VERSION_ID:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSVersionID;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSVersionID);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_LATITUDE_REF:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSLatitudeRef;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSLatitudeRef);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_LONGITUDE_REF:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSLongitudeRef;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSLongitudeRef);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_ALTITUDE_REF:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            *ptr = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSAltitudeRef;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSAltitudeRef);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_LATITUDE_ARRAY:
      if  (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        nRet = SUCCESS;
        if(ptr == NULL)
          *pSize = MV_MAX_GPS_LATITUDE;
        else if(*pSize >= MV_MAX_GPS_LATITUDE)
        {
          int i;
          uint64 *pb = (uint64*)ptr;
          for(i=0; i < MV_MAX_GPS_LATITUDE; i++)
            pb[i] = pme->m_pSpec->ExtendedInfo.UUIDSpec.GPSLatitudeArray[i];
        }
        else
        {
          nRet = EBADPARM;
          DBGPRINTF_ERROR("MV: Invalid buffer size, size = %d", *pSize);
        }
      }
      break;

    case MMV_SPEC_GPS_LONGITUDE_ARRAY:
      if  (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        nRet = SUCCESS;
        if(ptr == NULL)
          *pSize = MV_MAX_GPS_LONGITUDE;
        else if(*pSize >= MV_MAX_GPS_LONGITUDE)
        {
          int i;
          uint64 *pb = (uint64*)ptr;
          for(i=0; i < MV_MAX_GPS_LONGITUDE; i++)
            pb[i] = pme->m_pSpec->ExtendedInfo.UUIDSpec.GPSLongitudeArray[i];
        }
        else
        {
          nRet = EBADPARM;
          DBGPRINTF_ERROR("MV: Invalid buffer size, size = %d", *pSize);
        }
      }
      break;

    case MMV_SPEC_GPS_ALTITUDE:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            uint64 *p = (uint64*)ptr;
            *p = pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSAltitude;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.dwGPSAltitude);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_TIME_ARRAY:
      if  (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
      {
        nRet = SUCCESS;
        if(ptr == NULL)
          *pSize = MV_MAX_GPS_TIME;
        else if(*pSize >= MV_MAX_GPS_TIME)
        {
          int i;
          uint64 *pb = (uint64*)ptr;
          for(i=0; i < MV_MAX_GPS_TIME; i++)
            pb[i] = pme->m_pSpec->ExtendedInfo.UUIDSpec.GPSTimeArray[i];
        }
        else
        {
          nRet = EBADPARM;
          DBGPRINTF_ERROR("Invalid buffer size, size = %d", *pSize);
        }
      }
      break;

    case MMV_SPEC_GPS_MAP_SCALE_INFO:
      {
        if (pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          if(ptr)
          {
            uint64 *p = (uint64*)ptr;
            *p = pme->m_pSpec->ExtendedInfo.UUIDSpec.ddwGPSExtensionMapScaleInfo;
          }
          *pSize = sizeof(pme->m_pSpec->ExtendedInfo.UUIDSpec.ddwGPSExtensionMapScaleInfo);
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_SURVEY_DATA:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSSurveyData);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSSurveyData, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_POSITIONING_METHOD:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSPositioningMethod);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSPositioningMethod, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_POSITIONING_NAME:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSPositioningName);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSPositioningName, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    case MMV_SPEC_GPS_DATE:
      {
        uint32 nSize = *pSize;
        if(pme->m_pSpec->eFormatType == MM_MPEG4_UUID_FORMAT_TYPE)
        {
          nSize = WSTRLEN(pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSDate);
          if (nSize >= 1)
               nSize = nSize + 1;
          if(ptr != NULL)
          {
            *pSize = MIN(nSize, *pSize);
            (void)WSTRLCPY((AECHAR*)ptr, pme->m_pSpec->ExtendedInfo.UUIDSpec.szGPSDate, *pSize);
          }
          else
            *pSize = nSize;
          nRet = SUCCESS;
        }
      }
      break;

    default:
      DBGPRINTF_ERROR("MV: Unknown type on getSPEC = %d.", type);
      break;
  }


  return nRet;
}

/*==============================================================================
 this function retrieves framerate from MP4 in double type
==============================================================================*/
static int MediaVideo_GetFrameRate(MediaVideo* pme, double* rate)
{
  int nRet = EUNSUPPORTED;

#ifdef MM_MP4_PARM_VIDEO_FRAME_RATE_EX
  double FrameRate;

  if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_VIDEO_FRAME_RATE_EX, (int32 *)&FrameRate, (int32 *)0)))
  {
    *rate = FrameRate;
  }
  else
  {
    DBGPRINTF_ERROR("MV: Failed to retrieve frame rate.");
  }
#endif //MM_MP4_PARM_VIDEO_FRAME_RATE_EX

  return nRet;
}

/*==============================================================================
  this function converts codec types from IMediaMPEG4 to IMediaVideo
==============================================================================*/
static MediaCodecType MediaVideo_ConvertCodecType(AEEMediaMP4CodecType type)
{
  MediaCodecType CodecType;
  switch(type)
  {
  case MM_MPEG4_EVRC_CODEC:
    CodecType = MMV_CODEC_AUDIO_EVRC;
    break;
  case MM_MPEG4_QCELP_CODEC:
    CodecType = MMV_CODEC_AUDIO_QCELP;
    break;
  case MM_MPEG4_AAC_CODEC:
    CodecType = MMV_CODEC_AUDIO_AAC;
    break;
  case MM_MPEG4_GSM_AMR_CODEC:
    CodecType = MMV_CODEC_AUDIO_GSM_AMR;
    break;
  case MM_MPEG4_MPEG4_CODEC:
    CodecType = MMV_CODEC_VIDEO_MPEG4;
    break;
  case MM_MPEG4_H263_CODEC:
    CodecType = MMV_CODEC_VIDEO_H263;
    break;
  case MM_MPEG4_STILL_IMAGE_CODEC:
    CodecType = MMV_CODEC_VIDEO_STILL_IMAGE;
    break;
  case MM_MPEG4_TIMED_TEXT_CODEC:
    CodecType = MMV_CODEC_VIDEO_TIMED_TEXT;
    break;
  case MM_MPEG4_MP3_CODEC:
    CodecType = MMV_CODEC_AUDIO_MP3;
    break;
  case MM_MPEG4_H264_CODEC:
    CodecType = MMV_CODEC_VIDEO_H264;
    break;
  case MM_MPEG4_WMV1_CODEC:
    CodecType = MMV_CODEC_VIDEO_WMV1;
    break;
  case MM_MPEG4_WMV2_CODEC:
    CodecType = MMV_CODEC_VIDEO_WMV2;
    break;
  case MM_MPEG4_WMV3_CODEC:
    CodecType = MMV_CODEC_VIDEO_WMV3;
    break;
  case MM_MPEG4_WMA_CODEC:
    CodecType = MMV_CODEC_AUDIO_WMA;
    break;
  case MM_MPEG4_UNKNOWN_CODEC:
  default:
    CodecType = MMV_CODEC_UNKNOWN;
    DBGPRINTF_ERROR("MV: Unknown codec type.");
    break;
  }
  return CodecType;
}
/*==================================================================
 call to MPEG4 to play video clip
==================================================================*/
int MediaVideo_Play(IMediaVideo * po)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: No MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to MediaVideo_Play. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    pme->m_bResume = FALSE;
    return IMEDIA_Play(pme->m_pIMediaMPEG4);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}

/*==================================================================
   Unsupported functions -- used in vtbl of IMediaVideo classes
==================================================================*/
int MediaVideo_RecordUnsupported(IMediaVideo * po)
{
   return EUNSUPPORTED;
}

/*==================================================================
 call to MPEG4 to stop playing video clip
==================================================================*/
int MediaVideo_Stop(IMediaVideo * po)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: No MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to IMEDIA_Stop. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    pme->m_bResume = FALSE;
    return IMEDIA_Stop(pme->m_pIMediaMPEG4);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}

/*==================================================================
call to MPEG4 to perform seek
==================================================================*/
int MediaVideo_Seek(IMediaVideo * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: No MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to IMEDIA_Seek. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    return IMEDIA_Seek(pme->m_pIMediaMPEG4, eSeek, lTimeMS);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}


/*==================================================================
call to MPEG4 to pause playing
==================================================================*/
int MediaVideo_Pause(IMediaVideo * po)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: No MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to IMEDIA_Pause. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    return IMEDIA_Pause(pme->m_pIMediaMPEG4);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}

/*==================================================================
call to MPEG4 to resume from paused play
==================================================================*/
int MediaVideo_Resume(IMediaVideo * po)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: No MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to IMEDIA_Resume. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    return IMEDIA_Resume(pme->m_pIMediaMPEG4);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}

/*==================================================================
call to MPEG4 to retrieve total time of a clip
==================================================================*/
int MediaVideo_GetTotalTime(IMediaVideo * po)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: no MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to IMEDIA_GetTotalTime. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    return IMEDIA_GetTotalTime(pme->m_pIMediaMPEG4);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}

/*==================================================================
call to MPEG4 to get current play state
==================================================================*/
int MediaVideo_GetState(IMediaVideo * po, boolean * pbStateChanging)
{
  MediaVideo * pme = (MediaVideo *) po;
  if(pme)
  {
    if(!pme->m_pIMediaMPEG4)
    {
      DBGPRINTF_ERROR("MV: No MPEG4.");
      return EFAILED;
    }

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: call to IMEDIA_GetState. pme = 0x%X, pMPEG4 = 0x%X", pme, pme->m_pIMediaMPEG4);
#endif //#ifdef MV_DEBUG

    return IMEDIA_GetState(pme->m_pIMediaMPEG4, pbStateChanging);
  }
  else
  {
    DBGPRINTF_ERROR("MV: bad input parm.");
    return EBADPARM;
  }
}

/*==================================================================
 this functin will free IMediaVideo interface
==================================================================*/
void MediaVideo_FreeIF(IBase ** ppif)
{
  if (ppif && *ppif)
  {
#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: releasing IMediaVideo");
#endif //#ifdef MV_DEBUG

    IBASE_Release(*ppif);
    *ppif = NULL;
  }
}

/*==================================================================
This function registers callback to IMediaMPEG4 object
==================================================================*/
static int MediaVideo_RegisterMediaCallback(MediaVideo* pme)
{
  int nRet = EFAILED;
  if(pme && pme->m_pIMediaMPEG4)
  {
    if(SUCCESS != (nRet = IMEDIA_RegisterNotify(pme->m_pIMediaMPEG4, MediaVideo_MediaNotify, pme)))
    {
      DBGPRINTF_ERROR("MV: failed to register notify to MPEG4.");
    }
  }
  else
    DBGPRINTF_ERROR("MV: MediaVideo_RegisterMediaCallback(), NULL ptr.");

  return nRet;
}

/*==================================================================
callback from IMediaMPEG4 object
==================================================================*/
static void MediaVideo_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
  AEEMediaCmdNotify Notify;
  MediaVideo *pme = (MediaVideo *)pUser;

  // convert to IMediaVideo status type if it is necessary
  if(SUCCESS == MediaVideo_ConvertMediaNotify(pUser, &Notify, pCmdNotify))
  {
    // callback user client
    pme->m_pfnNotify(pme->m_pUser, (void *)&Notify);
  }
  else
  {
    DBGPRINTF_ERROR("MV: media notify conversion failed, drop callback.");
    DBGPRINTF_ERROR("      nCmd = %d, nStatus = %d, nSubCmd = %d, dwSize = %d, pCmdData = 0x%X",
                   pCmdNotify->nCmd, pCmdNotify->nStatus, pCmdNotify->nSubCmd,
                   pCmdNotify->dwSize, pCmdNotify->pCmdData );
  }
}

/*==================================================================
   this function converts STATUS type from IMediaMPEG4 to IMediaVideo
==================================================================*/
static int MediaVideo_ConvertMediaNotify(void *pUser, AEEMediaCmdNotify* pNotify, AEEMediaCmdNotify *pNotifyMPEG4)
{
  MediaVideo *pme = (MediaVideo *)pUser;
  int nRet = SUCCESS;

  if(pme == NULL)
  {
    DBGPRINTF_ERROR("MV: MediaVideo_ConvertMediaNotify(), pme = NULL.");
    nRet = EFAILED;
    return nRet;
  }

  if (!pNotifyMPEG4 || !pNotify)
  {
    DBGPRINTF_ERROR("MV: MediaVideo_ConvertMediaNotify(), pNotifyMPEG4 = 0x%X, pNotify = 0x%X.",pNotifyMPEG4, pNotify);
    nRet = EFAILED;
    return nRet;
  }

  // make a copy of src notify
  pNotify->clsMedia = pme->m_clsSelf;
  pNotify->pIMedia  = (IMedia*)pme;
  pNotify->nCmd     = pNotifyMPEG4->nCmd;
  pNotify->nStatus  = pNotifyMPEG4->nStatus;
  pNotify->nSubCmd  = pNotifyMPEG4->nSubCmd;
  pNotify->dwSize   = pNotifyMPEG4->dwSize;
  pNotify->pCmdData = pNotifyMPEG4->pCmdData;

#ifdef MV_DEBUG
    DBGPRINTF_LOW("MV: Notify, m_clsSelf=0x%X, nCmd = %d, nStatus = %d, nSubCmd = %d, dwSize = %d, pCmdData = 0x%X",
                  pNotify->clsMedia, pNotify->nCmd, pNotify->nStatus, pNotify->nSubCmd, pNotify->dwSize, pNotify->pCmdData );
#endif //#ifdef MV_DEBUG

  switch (pNotifyMPEG4->nStatus)
  {
    case MM_STATUS_TICK_UPDATE:
      // put this in the first for perfomance
      break;
    case MM_MP4_STATUS_3GPP_TTEXT:
    {
      AEE3GPPTimedText *pTTxt = (AEE3GPPTimedText *) (pNotifyMPEG4->pCmdData);
      if(pTTxt)
      {
        (void)MediaVideo_CopyTimedText(pme, pTTxt);

        pNotify->nStatus = MMV_STATUS_TIMED_TEXT;
        pNotify->pCmdData = (void*)pme->m_pTimedText;
      }
      else
      {
        pNotify->nStatus = MMV_ERROR_TEXT;
        pNotify->dwSize = 0;
      }
      break;
    }
    case MM_MP4_STATUS_TELOP_TEXT:
    {
      MVTelopTextElement *pTxtElement = (MVTelopTextElement *) (pNotifyMPEG4->pCmdData);
      (void)MediaVideo_CopyTelopText(pme, pTxtElement);

      pNotify->nStatus = MMV_STATUS_TELOP_TEXT;
      pNotify->pCmdData = (void*)pme->m_pTelopText;

      break;
    }
    case MM_MP4_STATUS_PS_BUFFER_UPDATE:
    case MM_MP4_STATUS_BUFFER_UPDATE:
    {
      uint32 prerollPercentage;
      if(pNotifyMPEG4->pCmdData)
      {
        prerollPercentage = *((uint32 *)pNotifyMPEG4->pCmdData);
        pNotify->pCmdData = (uint32 *)prerollPercentage;
      }
      pNotify->nStatus = MMV_STATUS_BUFFER_UPDATE;
     break;
    }

    case MM_STATUS_DOWNLOAD_TICK_UPDATE:
    {
      uint32 downloadPos;
      if (pNotifyMPEG4->pCmdData)
      {
        downloadPos =  *((uint32 *)pNotifyMPEG4->pCmdData);
        pNotify->pCmdData = (uint32 *)downloadPos;
      }
      pNotify->nStatus = MMV_STATUS_DOWNLOAD_TICK_UPDATE;
      break;
    }

    case MM_STATUS_RESUME:
    {
      // seek during the pause
      MVPlaybackPos pos;
      if(!pNotify->pCmdData && SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARAM_PLAYBACK_POS, (int32 *)(&pos), 0))
      {
        pNotify->pCmdData = (void *)pos.dwVideoPos;
        pNotify->dwSize = sizeof(pos.dwVideoPos);
      }
      break;
    }
    case MM_MP4_STATUS_CONNECTION_UPDATE:
      pNotify->nStatus = MMV_STATUS_CONNECTION_UPDATE;
      break;
    case MM_MP4_STATUS_PB_READY:
      pNotify->nStatus = MMV_STATUS_PLAYBACK_READY;
      break;
    case MM_MP4_STATUS_OPEN_COMPLETE:
      pNotify->nStatus = MMV_STATUS_OPEN_COMPLETE;
      break;
    case MM_MP4_STATUS_NO_SYNC_FRAME:
      pNotify->nStatus = MMV_STATUS_NO_SYNC_FRAME;
      break;
    case MM_MP4_STATUS_RTSP:
      pNotify->nStatus = MMV_STATUS_RTSP;
      if(pNotifyMPEG4->pCmdData)
      {
        AEERTSPStatus* pStatus;
        pStatus = (AEERTSPStatus*)pNotifyMPEG4->pCmdData;

        pme->m_StatusData.dwType = pStatus->statusCode;
        pme->m_StatusData.pData = pStatus->statusString;
        pme->m_StatusData.dwSize = STRLEN(pStatus->statusString);

        pNotify->pCmdData = &pme->m_StatusData;
        pNotify->dwSize = sizeof(AEEMediaVideoData) + pme->m_StatusData.dwSize;
      }
      break;
    case MM_MP4_STATUS_RESTARTING_VIDEO:
      pNotify->nStatus = MMV_STATUS_RESTARTING_VIDEO;
      break;
    case MM_MP4_STATUS_RESTARTING_AUDIO:
      pNotify->nStatus = MMV_STATUS_RESTARTING_AUDIO;
      break;
    case MM_MP4_STATUS_RESTARTING_TEXT:
      pNotify->nStatus = MMV_STATUS_RESTARTING_TEXT;
      break;
    case MM_MP4_STATUS_UNHANDLED_SDP:
      pNotify->nStatus = MMV_STATUS_UNHANDLED_SDP;
      if(pNotifyMPEG4->pCmdData)
      {
        AEEUnhandledSDP_RTSP_Status* pStatus;
        pStatus = (AEEUnhandledSDP_RTSP_Status*)pNotifyMPEG4->pCmdData;

        pme->m_StatusData.dwType = 0;
        pme->m_StatusData.pData = pStatus->statusString;
        pme->m_StatusData.dwSize = STRLEN(pStatus->statusString);

        pNotify->pCmdData = &pme->m_StatusData;
        pNotify->dwSize = sizeof(AEEMediaVideoData) + pme->m_StatusData.dwSize;
      }
      break;
    case MM_MP4_STATUS_UNHANDLED_RTSP:
      pNotify->nStatus = MMV_STATUS_UNHANDLED_RTSP;
      if(pNotifyMPEG4->pCmdData)
      {
        AEEUnhandledSDP_RTSP_Status* pStatus;
        pStatus = (AEEUnhandledSDP_RTSP_Status*)pNotifyMPEG4->pCmdData;

        pme->m_StatusData.dwType = 0;
        pme->m_StatusData.pData = pStatus->statusString;
        pme->m_StatusData.dwSize = STRLEN(pStatus->statusString);

        pNotify->pCmdData = &pme->m_StatusData;
        pNotify->dwSize = sizeof(AEEMediaVideoData) + pme->m_StatusData.dwSize;
      }
      break;
    case MM_MP4_STATUS_AUDIO_ABORT:
      pNotify->nStatus = MMV_STATUS_ERROR;
      pNotify->pCmdData = (void*)MMV_ERROR_AUDIO;
      pNotify->dwSize = sizeof(uint32);
      break;
    case MM_MP4_STATUS_VIDEO_ABORT:
      pNotify->nStatus = MMV_STATUS_ERROR;
      pNotify->pCmdData = (void*)MMV_ERROR_VIDEO;
      pNotify->dwSize = sizeof(uint32);
      break;

    case MM_MP4_STATUS_PLAYING:
      if(pme->m_bResume)
      {
        // seek during the play
        MVPlaybackPos pos;
        pNotify->nStatus = MM_STATUS_RESUME;
        if(!pNotify->pCmdData && SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARAM_PLAYBACK_POS, (int32 *)(&pos), 0))
        {
          pNotify->pCmdData = (void *)pos.dwVideoPos;
          pNotify->dwSize = sizeof(pos.dwVideoPos);
        }
      }
      else
        pme->m_bResume = TRUE;
      break;
    case MM_MP4_STATUS_FILE_SYS_LIMIT:
      pNotify->nStatus = MM_STATUS_SPACE_WARNING;
      break;
    case MM_MP4_STATUS_FILE_SYS_FULL:
      pNotify->nStatus = MM_STATUS_SPACE_ERROR;
      break;
    case MM_MP4_STATUS_FILE_ALREADY_EXISTS:
      pme->m_StatusData.dwType = MMV_FILE_SAVING_ALREADY_EXISTS;
      pme->m_StatusData.pData = NULL;
      pme->m_StatusData.dwSize = 0;

      pNotify->nStatus = MMV_STATUS_FILE_SAVING;
      pNotify->pCmdData = &pme->m_StatusData;
      pNotify->dwSize = sizeof(AEEMediaVideoData);
      break;
    case MM_MP4_STATUS_FILE_OW_FAILURE:
    {
      pme->m_StatusData.dwType = MMV_FILE_SAVING_OW_FAILURE;
      pme->m_StatusData.pData = NULL;
      pme->m_StatusData.dwSize = 0;

      pNotify->nStatus = MMV_STATUS_FILE_SAVING;
      pNotify->pCmdData = &pme->m_StatusData;
      pNotify->dwSize = sizeof(AEEMediaVideoData);
      break;
    }
    case MM_MP4_STATUS_RECORDER_FAILED:
    {
      pme->m_StatusData.dwType = MMV_FILE_SAVING_RECORD_FAILED;
      pme->m_StatusData.pData = NULL;
      pme->m_StatusData.dwSize = 0;

      pNotify->nStatus = MMV_STATUS_FILE_SAVING;
      pNotify->pCmdData = &pme->m_StatusData;
      pNotify->dwSize = sizeof(AEEMediaVideoData);
      break;
    }
    case MM_MP4_STATUS_RECORD_COMPLETED:
    {
      pme->m_StatusData.dwType = MMV_FILE_SAVING_COMPLETED;
      pme->m_StatusData.pData = NULL;
      pme->m_StatusData.dwSize = 0;

      pNotify->nStatus = MMV_STATUS_FILE_SAVING;
      pNotify->pCmdData = &pme->m_StatusData;
      pNotify->dwSize = sizeof(AEEMediaVideoData);
      break;
    }
    case MM_MP4_STATUS_VIDEO_MEDIA_END:
      pNotify->nStatus = MMV_STATUS_VIDEO_END;
      break;
    case MM_MP4_STATUS_AUDIO_MEDIA_END:
      pNotify->nStatus = MMV_STATUS_AUDIO_END;
      break;
    case MM_MP4_STATUS_TEXT_MEDIA_END:
      pNotify->nStatus = MMV_STATUS_TEXT_END;
      break;
    case MM_MP4_STATUS_PSEUDO_PAUSE:
      pNotify->nStatus = MM_STATUS_DATA_IO_DELAY;
      break;
    case MM_MP4_STATUS_PSEUDO_RESUME:
      pNotify->nStatus = MM_STATUS_RESUME;
      break;
    case MM_MP4_STATUS_HTTP_PAUSE:
      pNotify->nStatus = MM_STATUS_DATA_IO_DELAY;
      break;
    case MM_MP4_STATUS_HTTP_RESUME:
      pNotify->nStatus = MM_STATUS_RESUME;
      break;
    case MM_MP4_STATUS_RECORDED_CLIP_DURATION:
    {
      uint32    record_duration;
      record_duration = *((uint32 *)pNotifyMPEG4->pCmdData);

      pme->m_StatusData.dwType = MMV_FILE_SAVING_DURATION;
      pme->m_StatusData.pData = (void*)record_duration;
      pme->m_StatusData.dwSize = sizeof(record_duration);

      pNotify->nStatus = MMV_STATUS_FILE_SAVING;
      pNotify->pCmdData = &pme->m_StatusData;
      pNotify->dwSize = sizeof(AEEMediaVideoData) + sizeof(record_duration);
      break;
    }
    case MM_MP4_STATUS_TS_LAYER_PRESENT:
      pNotify->nStatus = MMV_STATUS_TS_LAYER_PRESENT;
      break;
    case MM_MP4_STATUS_VIDEO_RESOLUTION_NOT_SUPPORTED:
      pNotify->nStatus = MMV_STATUS_ERROR;
      pNotify->pCmdData = (void*)MMV_ERROR_VIDEO_RESOLUTION_NOT_SUPPORTED;
      pNotify->dwSize = sizeof(uint32);
      break;

    case MM_STATUS_MEDIA_SPEC:
      pNotify->pCmdData = NULL;
      pNotify->dwSize = 0;
      break;

    case MM_STATUS_ABORT:
    case MM_STATUS_DONE:
      nRet = MediaVideo_HandleStatusDoneEvent(pme, pNotify);
      break;

    default:
      if(pNotify->nStatus >= MM_STATUS_USER_BASE && pNotify->nStatus < MMV_STATUS_USER_BASE)
      {
        DBGPRINTF_ERROR("MV: Unknown status type, nStatus = %d.", pNotify->nStatus);
        nRet = EFAILED;
      }
      break;
  }

  return nRet;
}

/*==================================================================
   this function handles status_done/status_abort event by looking
   at if there is and previous pending process
==================================================================*/
static int MediaVideo_HandleStatusDoneEvent(MediaVideo *pme, AEEMediaCmdNotify* pNotify)
{
  int nRet = SUCCESS;

  switch(pNotify->nCmd)
  {
    case MM_CMD_SETMEDIAPARM:
      switch(pNotify->nSubCmd)
      {
        case MM_MP4_PARM_ROTATION:
          if(pNotify->nStatus == MM_STATUS_DONE && pme->m_bRotationPending)
          {
            int32 mpeg4value;
            // successfully set rotation to MPEG4 layer
            if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ROTATION, &mpeg4value, 0))
            {
              // retrieve setting
              pme->m_VideoRotation = mpeg4value;
            }
            else
            {
              // save the requested value by user
              pme->m_VideoRotation = pme->m_nPendingRotation;
            }
          }
          pNotify->nSubCmd = MMV_PARM_ROTATION;
          pme->m_bRotationPending = FALSE;
          break;

        case MM_MP4_PARM_ASCALING:
          if(pNotify->nStatus == MM_STATUS_DONE && pme->m_bScalingPending)
          {
            int32 factor_x;
            int32 factor_y;
            // successfully set scaling to MPEG4 layer
            if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_ASCALING, &factor_x, &factor_y))
            {
              // retrieve and save current scaling factors
              pme->m_VideoScaling_H = factor_x;
              pme->m_VideoScaling_V = factor_y;
            }
            else
            {
              pme->m_VideoScaling_H = pme->m_PendingVideoScaling_H;
              pme->m_VideoScaling_V = pme->m_PendingVideoScaling_V;
            }
          }
          pNotify->nSubCmd = MMV_PARM_SCALING;
          pme->m_bScalingPending = FALSE;
          break;

        case MM_MP4_PARM_SCALING:
          if(pNotify->nStatus == MM_STATUS_DONE && pme->m_bScalingPending)
          {
            int32 scaling;
            // successfully set rotation to MPEG4 layer
            if(SUCCESS == IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_SCALING, &scaling, 0))
            {
              // retrieve and save current scaling factors
              pme->m_VideoScaling_H = scaling;
              pme->m_VideoScaling_V = scaling;
            }
            else
            {
              pme->m_VideoScaling_H = pme->m_PendingVideoScaling_H;
              pme->m_VideoScaling_V = pme->m_PendingVideoScaling_V;
            }
          }
          pNotify->nSubCmd = MMV_PARM_SCALING;
          pme->m_bScalingPending = FALSE;
          break;

        default:
          if(pNotify->nCmd >= MM_PARM_USER_BASE && pNotify->nCmd < MMV_PARM_BASE)
          {
            int32 nMVMessage;
            nMVMessage = MediaVideo_GetMessageType(pme, pNotify->nSubCmd, 0);

            if(nMVMessage)
            {
              DBGPRINTF_HIGH("MV: Unknown nCmd type on MM_CMD_SETMEDIAPARM, nStatus = %d, nCmd = %d, nSubCmd = %d, nMVMessage = %d.",
                             pNotify->nStatus, pNotify->nCmd, pNotify->nSubCmd, nMVMessage);
              pNotify->nSubCmd = nMVMessage;
            }
            else
            {
              DBGPRINTF_ERROR("MV: block Cmd type on MM_CMD_SETMEDIAPARM, nStatus = %d, nCmd = %d, nSubCmd = %d, nMVMessage = %d.",
                             pNotify->nStatus, pNotify->nCmd, pNotify->nSubCmd, nMVMessage);
              nRet = EFAILED;
            }
          }
          break;
      }
      break;
    case MM_CMD_GETMEDIAPARM:
      switch(pNotify->nSubCmd)
      {
        case MM_MP4_PARM_MEDIA_SPEC:
          if(pNotify->nStatus == MM_STATUS_DONE && pNotify->pCmdData != NULL)
          {
            if(!pme->m_pSpec)
            {
              pme->m_pSpec = MALLOC(sizeof(AEEMediaMPEG4Spec));
            }

            if(pme->m_pSpec)
            {
              (void)MEMMOVE(pme->m_pSpec, (void*)pNotify->pCmdData, sizeof(AEEMediaMPEG4Spec));
              pme->m_bSpec = TRUE;
              DBGPRINTF_LOW("MV: SPEC is available.");
            }
            else
            {
              pNotify->nStatus = MM_STATUS_ABORT;
              DBGPRINTF_ERROR("MV: cannot allocate memory.");
              break;
            }
          }
          pNotify->nSubCmd = MMV_PARM_SPEC;
          break;

        default:
          if(pNotify->nCmd >= MM_PARM_USER_BASE && pNotify->nCmd < MMV_PARM_BASE)
          {
            int32 nMVMessage;
            nMVMessage = MediaVideo_GetMessageType(pme, pNotify->nSubCmd, 0);

            if(nMVMessage)
            {
              DBGPRINTF_HIGH("MV: Unknown nCmd type on MM_CMD_GETMEDIAPARM, nStatus = %d, nCmd = %d, nSubCmd = %d, nMVMessage = %d.",
                             pNotify->nStatus, pNotify->nCmd, pNotify->nSubCmd, nMVMessage);
              pNotify->nSubCmd = nMVMessage;
            }
            else
            {
              DBGPRINTF_ERROR("MV: block Cmd type on MM_CMD_GETMEDIAPARM, nStatus = %d, nCmd = %d, nSubCmd = %d, nMVMessage = %d.",
                             pNotify->nStatus, pNotify->nCmd, pNotify->nSubCmd, nMVMessage);
              nRet = EFAILED;
            }
          }
          break;
      }
      break;
    default:
      break;
  }

  return nRet;
}
/*==============================================================================
 convert PARM messages between MediaVideo and AEEMediaMPEG4

 if ((nMPEG4 == NULL) && nMV)----> return AEEMediaMPEG4 PARM message
 if (nMPEG4 && (nMV == NULL))----> return MediaVideo PARM message
 if ((nMPEG4 == NULL) && (nMV == NULL)) ----> return NULL

==============================================================================*/
static int32 MediaVideo_GetMessageType(MediaVideo *pme, int32 nMPEG4, int32 nMV)
{
  int i;
  if(pme == NULL || pme->m_pTypeTable_PARM == NULL)
    return 0;

  if(nMPEG4 && (nMV == 0))
  {
    for(i=0; i < MV_MPEG4_PARM_COUNT; i++)
    {
      if(pme->m_pTypeTable_PARM[i].m_nMPEG4 == nMPEG4)
      {
        return pme->m_pTypeTable_PARM[i].m_nMV;
      }
    }
  }
  else if((nMPEG4 == 0) && nMV)
  {
    for(i=0; i < MV_MPEG4_PARM_COUNT; i++)
    {
      if(pme->m_pTypeTable_PARM[i].m_nMV == nMV)
      {
        return pme->m_pTypeTable_PARM[i].m_nMPEG4;
      }
    }
  }

  return 0;
}

/*==============================================================================
 this function make a local copy of timed text object
==============================================================================*/
static boolean MediaVideo_CopyTimedText(MediaVideo *pme, AEE3GPPTimedText* pSrc)
{
  if(pme && pSrc)
  {
    int i;
    if(pme->m_pTimedText == NULL)
    {
      // first time? allocate memory
      pme->m_pTimedText = (AEEMediaVideo3GPPTimedText*)MALLOC(sizeof(AEEMediaVideo3GPPTimedText));
      if(pme->m_pTimedText == NULL)
      {
        DBGPRINTF_ERROR("MV: failed to allocate memory.");
        return FALSE;
      }
      pme->m_pTimedText->pwszText = NULL;
      pme->m_pTimedText->dwTextSize = 0;
      pme->m_pTimedText->pFontList = NULL;
      pme->m_pTimedText->pSampleModifiersBuffer = NULL;
    }

    pme->m_pTimedText->pwszText   = pSrc->pszText;
    pme->m_pTimedText->dwTextSize = WSTRLEN(pSrc->pszText);
    pme->m_pTimedText->dwDuration = pSrc->dwDuration;
    pme->m_pTimedText->dwDisplayFlags = pSrc->dwDisplayFlags;
    pme->m_pTimedText->nHorzJustification = pSrc->dwHorzJustification;
    pme->m_pTimedText->nVertJustification = pSrc->dwVertJustification;
    pme->m_pTimedText->nBoxTop = pSrc->dwBoxTop;
    pme->m_pTimedText->nBoxLeft = pSrc->dwBoxLeft;
    pme->m_pTimedText->nBoxBottom = pSrc->dwBoxBottom;
    pme->m_pTimedText->nBoxRight = pSrc->dwBoxRight;
    pme->m_pTimedText->dwStartChar = pSrc->dwStartChar;
    pme->m_pTimedText->dwEndChar = pSrc->dwEndChar;
    pme->m_pTimedText->dwFontID = pSrc->dwFontID;
    pme->m_pTimedText->dwFontStyleFlags = pSrc->dwFontStyleFlags;
    pme->m_pTimedText->dwFontSize = pSrc->dwFontSize;
    for(i = 0; i < MMV_MAX_3GPP_TEXT_COLOR; i++)
      pme->m_pTimedText->BackgroundColorRGBA[i] = pSrc->BackgroundColorRGBA[i];
    for(i = 0; i < MMV_MAX_3GPP_TEXT_COLOR; i++)
      pme->m_pTimedText->TextColorRGBA[i] = pSrc->TextColorRGBA[i];
    pme->m_pTimedText->dwFontListSize = pSrc->dwFontListSize;

    pme->m_pTimedText->pFontList = (AEEMediaVideoTTFontInfo*)pSrc->pFontList;
    pme->m_pTimedText->dwSampleModifiersSize = pSrc->dwSampleModifiersSize;
    pme->m_pTimedText->pSampleModifiersBuffer = pSrc->pSampleModifiersBuffer;

    return TRUE;
  }
  else
    DBGPRINTF_ERROR("MV: Null ptr, pme = 0x%X, pSrc = 0x%X.", pme, pSrc);
  return FALSE;
}

/*==============================================================================
 this function make a local copy of telop text object
==============================================================================*/
static boolean MediaVideo_CopyTelopText(MediaVideo *pme, MVTelopTextElement* pSrc)
{
  if(pme && pSrc)
  {
    int i, nCnt, nLen;
    MVTelopTextSubString *pTxtSubStr;
    if(pme->m_pTelopText == NULL)
    {
      // first time? allocate memory
      pme->m_pTelopText = (AEEMediaVideoTelopTextElement*)MALLOC(sizeof(AEEMediaVideoTelopTextElement));
      if(pme->m_pTelopText == NULL)
      {
        DBGPRINTF_ERROR("MV: failed to allocate memory.");
        return FALSE;
      }
      pme->m_pTelopText->pSubString = NULL;
    }

    if(pme->m_pTelopText->pSubString)
    {
      for(i=0; i < pme->m_pTelopText->nNumberOfSubStrings; i++)
      {
        if(&pme->m_pTelopText->pSubString[i])
        {
          FREEIF(pme->m_pTelopText->pSubString[i].pwszTextSubString);
          FREEIF(pme->m_pTelopText->pSubString[i].pwszLinkValue);
        }
      }
      FREEIF(pme->m_pTelopText->pSubString);
    }

    pme->m_pTelopText->dwTelopSize = pSrc->dwTelopSize;
    pme->m_pTelopText->nBeginTime = pSrc->dwBeginTime;
    pme->m_pTelopText->nDuration = pSrc->dwDuration;
    pme->m_pTelopText->bWrapFlag = pSrc->bWrapFlag;
    pme->m_pTelopText->nBackgroundColor = pSrc->dwBackgroundColor;
    pme->m_pTelopText->nDefaultFontColor = pSrc->dwDefaultFontColor;
    pme->m_pTelopText->nNumberOfSubStrings = pSrc->dwNumberOfSubStrings;

    nCnt = pme->m_pTelopText->nNumberOfSubStrings;

    if(nCnt)
    {
      pme->m_pTelopText->pSubString = (AEEMediaVideoTelopTextSubString*)MALLOC(nCnt * sizeof(AEEMediaVideoTelopTextSubString));
      if(!pme->m_pTelopText->pSubString)
      {
        DBGPRINTF_ERROR("MV: failed to allocate memory.");
      }
    }

    for (i = 0; i < nCnt; i++)
    {
      (void)IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_TELOP_SUB_STR, (int32 *)&pTxtSubStr, (int32 *)&i);
      if(pTxtSubStr && pme->m_pTelopText->pSubString)
      {
        pme->m_pTelopText->pSubString[i].bLineFeed = pTxtSubStr->bLineFeed;
        pme->m_pTelopText->pSubString[i].bUnderline = pTxtSubStr->bUnderline;
        pme->m_pTelopText->pSubString[i].bReversal = pTxtSubStr->bReversal;
        pme->m_pTelopText->pSubString[i].bLinking = pTxtSubStr->bLinking;
        pme->m_pTelopText->pSubString[i].nFontColor = pTxtSubStr->dwFontColor;
        pme->m_pTelopText->pSubString[i].dwTextSampleSize = pTxtSubStr->dwTextSampleSize;
        pme->m_pTelopText->pSubString[i].dwLinkSize = pTxtSubStr->dwLinkSize;

        nLen = pTxtSubStr->dwTextSampleSize;
        if(nLen)
        {
          pme->m_pTelopText->pSubString[i].pwszTextSubString = (AECHAR*)MALLOC((nLen + 1)* sizeof(AECHAR));
          MEMSET(pme->m_pTelopText->pSubString[i].pwszTextSubString, 0, (nLen + 1)* sizeof(AECHAR));

          (void)UTF8TOWSTR((byte *)pTxtSubStr->pTextSubString, nLen,
                          pme->m_pTelopText->pSubString[i].pwszTextSubString, (nLen) * sizeof(AECHAR));
        }
        else
        {
          pme->m_pTelopText->pSubString[i].pwszTextSubString = NULL;
        }

        nLen = pTxtSubStr->dwLinkSize;
        if(nLen)
        {
          pme->m_pTelopText->pSubString[i].pwszLinkValue = (AECHAR*)MALLOC((nLen + 1)* sizeof(AECHAR));
          MEMSET(pme->m_pTelopText->pSubString[i].pwszLinkValue, 0, (nLen + 1)* sizeof(AECHAR));
          (void)UTF8TOWSTR((byte *)pTxtSubStr->pLinkValue, nLen,
                          pme->m_pTelopText->pSubString[i].pwszLinkValue, (nLen) * sizeof(AECHAR));
        }
        else
        {
          pme->m_pTelopText->pSubString[i].pwszLinkValue = NULL;
        }
      }
    }

    return TRUE;
  }
  else
  {
    DBGPRINTF_ERROR("MV: Null ptr, pme = 0x%X, pSrc = 0x%X.", pme, pSrc);
  }

  return FALSE;
}

/*==============================================================================
 this function retrieves clip dimension
==============================================================================*/
static int MediaVideo_GetClipDim(MediaVideo *pme, MVClipDim* pDim)
{
  if( pme->m_bSpec && pme->m_pSpec)
  {
    pDim->m_nWidth     = pme->m_pSpec->dwWidth;
    pDim->m_nHeight    = pme->m_pSpec->dwHeight;
    return SUCCESS;
  }

  // we haven't received SPEC info, request now
  if(SUCCESS == MediaVideo_RequestClipInfo(pme))
  {
    pDim->m_nWidth     = pme->m_pSpec->dwWidth;
    pDim->m_nHeight    = pme->m_pSpec->dwHeight;
    return SUCCESS;
  }

  DBGPRINTF_HIGH("MV: SPEC is not available.");
  return EFAILED;
}

/*==============================================================================
 this function retrieves clip info
==============================================================================*/
static int MediaVideo_RequestClipInfo(MediaVideo *pme)
{
  AEEMediaMPEG4Spec* pSpec;
  int32              dwSize;
  int32              nRet = EFAILED;

  if(pme->m_pIMediaMPEG4)
  {
    if(SUCCESS == (nRet = IMEDIA_GetMediaParm(pme->m_pIMediaMPEG4, MM_MP4_PARM_MEDIA_SPEC, (int32 *)&pSpec, &dwSize)))
    {
      // may need to compare if dwSize == sizeof(AEEMediaMPEG4Spec)
      if(!pme->m_pSpec)
        pme->m_pSpec = MALLOC(sizeof(AEEMediaMPEG4Spec));

      if(pme->m_pSpec)
      {
        (void)MEMMOVE(pme->m_pSpec, pSpec, sizeof(AEEMediaMPEG4Spec));
        pme->m_bSpec = TRUE;
        DBGPRINTF_LOW("MV: SPEC is available.");
      }
      else
      {
        DBGPRINTF_ERROR("MV: cannot allocate memory.");
      }
    }
    else if(nRet == MM_PENDING)
    {
      DBGPRINTF_LOW("MV: get SPEC pending.");
    }
    else if(nRet != MM_PENDING)
    {
      DBGPRINTF_ERROR("MV: failed to get SPEC, nRet = %d.", nRet);
    }
  }

  return nRet;
}
/*==============================================================================
 This function will get next supported scaling factor
  value >  current, return next larger one
  value <  current, return next smaller one
  value == current, return value
 Note, in none Ascale mode, m_VideoScaling_H and m_VideoScaling_V are equal
==============================================================================*/
static int MediaVideo_GetSupportedScalingValue(MediaVideo *pme, int value)
{
  int x = 0;
  int result = 0;

  if(value <= 0)
  {
    result = MV_SCALING_STEP; // return minimal value
  }
  else if((uint32)value > pme->m_VideoScaling_H)
  {
    // zoom in
    x =FLTTOINT(FCEIL(FDIV((double)value, (double)MV_SCALING_STEP)));
    result = x*MV_SCALING_STEP;
  }
  else if((uint32)value < pme->m_VideoScaling_H)
  {
    // zoom out
    x = FLTTOINT(FFLOOR(FDIV((double)value, (double)MV_SCALING_STEP)));
    result = x*MV_SCALING_STEP;
  }
  else // none
  {
    result = pme->m_VideoScaling_H;
  }

#ifdef MV_DEBUG
  DBGPRINTF_HIGH("MV: conver scaling value, value = %d,  m_VideoScaling_H = %d, result = %d",
                 value, m_VideoScaling_H, result);
#endif //MV_DEBUG

  return result;
}

/*==============================================================================
 This function will get next supported MDP(none AScale mode) scaling factor
==============================================================================*/
static int MediaVideo_GetMDPScalingValue(MediaVideo *pme, int value)
{
  int result = 0;
  switch (value)
  {
    case MV_0P25X_SHRINK:  // 25%
    case MV_2X_SHRINK:     // 50%
    case MV_0P75X_SHRINK:  // 75%
    case MV_NO_SCALING:    // 100%
    case MV_2X_ZOOM:       // 200%
      result = value;
      break;
    case MV_1P25X_ZOOM:    // 125%
    case MV_2P5X_ZOOM:     // 250%
    default:
    {
      if((uint32)value > pme->m_VideoScaling_H)
      {
        result = MV_2X_ZOOM;
      }
      else if((uint32)value < pme->m_VideoScaling_H)
      {
        if(pme->m_VideoScaling_H == MV_2X_ZOOM ||pme->m_VideoScaling_H == MV_2P5X_ZOOM )
        {
          result = MV_NO_SCALING;
        }
        else
        {
          result = pme->m_VideoScaling_H;
        }
      }
      else
      {
        result = pme->m_VideoScaling_H;
      }
    }
    break;
  }


#ifdef MV_DEBUG
  DBGPRINTF_HIGH("MV: MDP scaling value, value = %d,  m_VideoScaling_H = %d, result = %d",
                 value, m_VideoScaling_H, result);
#endif //MV_DEBUG
  return result;
}

/*==============================================================================
 this function converts the scaling factor to the type, which MPEG4
 can accepte
==============================================================================*/
boolean MediaVideo_ConvertToMPEG4Scaling(int value, int* out)
{
  boolean bRet = TRUE;

  switch (value)
  {
  case MV_0P25X_SHRINK:
    *out =  MM_MPEG4_0P25X_SHRINK;
    break;
  case MV_2X_SHRINK:
    *out =  MM_MPEG4_2X_SHRINK;
    break;
  case MV_0P75X_SHRINK:
    *out =  MM_MPEG4_0P75X_SHRINK;
    break;
  case MV_NO_SCALING:
    *out =  MM_MPEG4_NO_SCALING;
    break;
  case MV_1P25X_ZOOM:
    *out =  MM_MPEG4_1P25X_ZOOM;
    break;
  case MV_2X_ZOOM:
    *out =  MM_MPEG4_2X_ZOOM;
    break;
  case MV_2P5X_ZOOM:
    *out =  MM_MPEG4_2P5X_ZOOM;
    break;
  case MV_ASCALE:
    *out =  MM_MPEG4_ASCALE;
    break;
  default:
    DBGPRINTF_ERROR("MV: MediaVideo_ConvertToMPEG4Scaling(), unsupported value= %d", value);
    bRet = FALSE;
    break;
  }

#ifdef MV_DEBUG
  DBGPRINTF_HIGH("MV: convert to MPEG4 scaling, value = %d,  result = %d, bRet = %d",
                  value, *out, bRet);
#endif //MV_DEBUG

  return bRet;
}

/*==============================================================================
  convert to supported rotation value. the output is always in the range of
  0-360
==============================================================================*/
static int MediaVideo_GetSupportedRotationValue(int value)
{
  int nRotate = value;

  // convert to 0-360
  if(nRotate > MV_360_CW_ROTATION || nRotate < -MV_360_CW_ROTATION)
    nRotate = nRotate%MV_360_CW_ROTATION;

  // reverse rotate direction if necessary
  if(nRotate < 0)
    nRotate = MV_360_CW_ROTATION + nRotate;

  if(nRotate == MV_NO_ROTATION)
  {
    nRotate = MV_NO_ROTATION;
  }
  else if(nRotate <= MV_90_CW_ROTATION)
  {
    nRotate = MV_90_CW_ROTATION;
  }
  else if(nRotate <= MV_180_ROTATION)
  {
    nRotate = MV_180_ROTATION;
  }
  else if(nRotate <= MV_90_CCW_ROTATION)
  {
    nRotate = MV_90_CCW_ROTATION;
  }
  else if(nRotate <= MV_360_CW_ROTATION)
  {
    nRotate = MV_NO_ROTATION;
  }
  else
    nRotate = MV_NO_ROTATION;

  return nRotate;
}

/*==============================================================================
 this function converts the rotation to the type, which MPEG4
 can accept
==============================================================================*/
static int MediaVideo_ConvertRotationValue(int rotate, int* out)
{
  switch (rotate)
  {
    case MV_NO_ROTATION:
      *out = MM_MPEG4_NO_ROTATION; /* 0 */
      break;
    case MV_90_CW_ROTATION:
      *out = MM_MPEG4_90_CW_ROTATION; /* 90 */
      break;
    case MV_180_ROTATION:
      *out = MM_MPEG4_180_ROTATION; /* 180 */
      break;
    case MV_90_CCW_ROTATION:
      *out = MM_MPEG4_90_CCW_ROTATION; /* 270 */
      break;
    default:
      DBGPRINTF_ERROR("MV: MediaVideo_ConvertRotationValue(), unsupported rotate value= %d", rotate);
      return EBADPARM;
  }
  return SUCCESS;
}

