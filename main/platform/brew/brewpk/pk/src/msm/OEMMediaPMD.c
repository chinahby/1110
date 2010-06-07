/*===========================================================================

FILE: OEMMediaPMD.c

SERVICES: IMEDIA interfaces

DESCRIPTION
   This file implements IMedia-based classes on CMX multimedia platform.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc60x0/apps/brew_3.1.5/pk/src/msm/OEMMediaPMD.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

3/27/06    antti   added FEATURE_SVGDOM

===========================================================================*/

#include <stdlib.h>
#include "OEMFeatures.h"

#if defined(FEATURE_WEBAUDIO)
#include "msm/gm2312.h"    // Simplified Chinese Unicode Map
#include "msm/uhc.h"			// Unified Hangul Unicode Map
#include "msm/jisx0208.h"	// Shift JIS Unicode Map
#include "msm/big5.h"      // Traditional Chinese Unicode Map
#include "msm/iscii_dvng.h"// ISCII (Devanagari) Unicode Map
#endif //defined(FEATURE_WEBAUDIO)

#include "cmx.h"
#include "AEEMedia.h"
#include "OEMClassIDs.h"
#include "OEMMediaFormats.h"
#include "OEMObjectMgr.h"
#include "AEECriticalSection.h"

#include "OEMMedia.h"
#include "AEE_OEMDispatch.h"
#include "AEEStdLib.h"
#include "AEEMediaFormats.h"
#include "AEEMimeTypes.h"
#include "AEEFile.h"
#include "AEE_OEM.h"
#include "AEE_OEMFile.h"
#include "OEMFS.h"
#include "AEEBitmap.h"

#if defined(FEATURE_SVG)
#include "AEEMediaSVG.h"

#if defined(FEATURE_SVGDOM)

#include "ui.h"
#include "task.h"

#include "OEMSVGDOM.h"
#include "AEESVGDOM.h"
#include "AEECLSID_SVGDOM.bid"
#endif // defined(FEATURE_SVGDOM)
#endif // defined(FEATURE_SVG)

#include "AEESource.h"

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#include "AEECLSID_SYNTHQUALITY.BID"
#include "OEMSynthQuality.h"
#endif

#if !defined(SVGT_VERSION)
#define SVGT_1_1 0
#define SVGT_VERSION SVGT_1_1
#endif // !defined(SVGT_VERSION)

#if (SVGT_VERSION > SVGT_1_1)
#if !defined(MM_STATUS_SVG_URL_IMAGE)
#include "AEEMediaSVGExt.h"
#endif // !defined(MM_STATUS_SVG_URL_IMAGE)
#endif // (SVGT_VERSION > SVGT_1_1)

/*==========================================================================================
Chinese Unicode (UCS-2/UTF16) conversion interface declarations...
============================================================================================*/
#if defined(FEATURE_WEBAUDIO)
typedef unsigned char       BYTE;
int GetUniStringLength(unsigned char* pmbBuffer, cmx_af_char_set_enum_type eCharSetType, int nLen);
/* Unicode conversion functions for PMD text */
boolean ConvertToUnicodebuffer(uint8 *buffer,uint16 *nLen,AECHAR **pUnicodeString,cmx_af_char_set_enum_type eCharSetType);
boolean GB2312ToUnicode(unsigned char* pGBStr,AECHAR* pUStr,int nLen);
boolean UHCToUnicode(unsigned char* pUHCStr,AECHAR* pUStr, int nUchLen);
boolean ShiftJISToUnicode(unsigned char *pJISStr,AECHAR *pUStr,int nUchLen);
boolean Big5ToUnicode(unsigned char *pBIG5Str,AECHAR *pUStr,int nUchLen);
boolean DevanagariToUnicode(unsigned char *pDVNGStr,AECHAR *pUStr,int nUchLen);


static const AECHAR* const gb2312[]= {
	gb2312_a1,
	gb2312_a2,
	gb2312_a3,
	gb2312_a4,
	gb2312_a5,
	gb2312_a6,
	gb2312_a7,
	gb2312_a8,
	gb2312_a9,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	gb2312_b0,
	gb2312_b1,
	gb2312_b2,
	gb2312_b3,
	gb2312_b4,
	gb2312_b5,
	gb2312_b6,
	gb2312_b7,
	gb2312_b8,
	gb2312_b9,
	gb2312_ba,
	gb2312_bb,
	gb2312_bc,
	gb2312_bd,
	gb2312_be,
	gb2312_bf,
	gb2312_c0,
	gb2312_c1,
	gb2312_c2,
	gb2312_c3,
	gb2312_c4,
	gb2312_c5,
	gb2312_c6,
	gb2312_c7,
	gb2312_c8,
	gb2312_c9,
	gb2312_ca,
	gb2312_cb,
	gb2312_cc,
	gb2312_cd,
	gb2312_ce,
	gb2312_cf,
	gb2312_d0,
	gb2312_d1,
	gb2312_d2,
	gb2312_d3,
	gb2312_d4,
	gb2312_d5,
	gb2312_d6,
	gb2312_d7,
	gb2312_d8,
	gb2312_d9,
	gb2312_da,
	gb2312_db,
	gb2312_dc,
	gb2312_dd,
	gb2312_de,
	gb2312_df,
	gb2312_e0,
	gb2312_e1,
	gb2312_e2,
	gb2312_e3,
	gb2312_e4,
	gb2312_e5,
	gb2312_e6,
	gb2312_e7,
	gb2312_e8,
	gb2312_e9,
	gb2312_ea,
	gb2312_eb,
	gb2312_ec,
	gb2312_ed,
	gb2312_ee,
	gb2312_ef,
	gb2312_f0,
	gb2312_f1,
	gb2312_f2,
	gb2312_f3,
	gb2312_f4,
	gb2312_f5,
	gb2312_f6,
	gb2312_f7,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL};
#endif //defined(FEATURE_WEBAUDIO)
/*conversion Declarations end here..*/

/*===============================================================================

      OEMMediaPMD DECLARATIONS
      These are used by all IMedia classes implemented in this file.

===============================================================================*/

int IMediaPMD_New(IShell * ps, AEECLSID cls, void **ppif);
void IMediaPMD_Init(IShell * ps);

//const AEEStaticClass gAEEMediaPMDClasses[] = {
//   {AEECLSID_MEDIAPMD,        ASCF_UPGRADE, 0, IMediaPMD_Init,  IMediaPMD_New},
//   {AEECLSID_MEDIASVG,        ASCF_UPGRADE, 0, IMediaPMD_Init,  IMediaPMD_New},
//   {0,0,0,NULL,NULL}
//};
// MMLayer global static objects...
typedef struct CMediaInitFlags
{
   flg   gbCMXInit:1;
   flg   gbMediaBgInit:1;
   flg   gbMediaMIDIOutQCPInit:1;
} CMediaInitFlags;

static CMediaInitFlags  gInitFlags;
static AEECallback   gcbAEEExit;


typedef struct WinBMPInfoHdr_s {
   dword dwBIHdrSize;
   dword dwBIWidth;
   int32 dwBIHeight;
   word  wNumPlanes;
   word  wBitsPerPix;
   dword dwBICompress;
   dword dwBISize;
   dword dwBIXPixPerMtr;
   dword dwBIYPixPerMtr;
   dword dwBIClrUsed;
   dword dwBIClrImp;
} WinBMPInfoHdr;

/*===============================================================================

      DownShift Stuff

===============================================================================*/
typedef struct AEEDShift_s {
   uint8   nSrcDepth;
   uint8   nDstDepth;
   uint16  nWidth;
   uint16  nSrcPitch;
   uint16  nDstPitch;
   uint16  nLayout;  //No. of pixels for R, G, B (for eg. 332, for 3R, 3G, 2B)
   boolean bDither;
   byte    Reserved;
} AEEDShift;

#define DITHERING_ON
typedef struct _DitherCoef {
   int16 coRed;
   int16 coGreen;
   int16 coBlue;
   int16 reserved;
} DitherCoef;

typedef struct IDShift  IDShift;
typedef int    (*PFNDSHIFT)(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap);

struct IDShift
{
   IShell         *piShell;

   unsigned short  usRef;

   AEEDShift       dshiftInfo;
   DitherCoef *    pDitherBuf;
   PFNDSHIFT       pfnDShift;
};

#if defined(FEATURE_WEBAUDIO)
static int    IDShift_New(IShell *piShell, void **pp);
static void   IDShift_Delete(IDShift *pMe);
static int    IDShift_Configure(IDShift *pMe, AEEDShift *pShiftInfo);
static int    IDShift_Convert(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap); 
static int    DShiftTo1(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap);
static int    DShiftTo4(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap);
static int    DShiftTo8(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap);
static int    DShiftTo16(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap);
static byte * DitherSrc(IDShift * pMe, byte *pSrc);
#endif // defined(FEATURE_WEBAUDIO)

#define DITHER_RGQUANT(x)  ((x)/32 * 32) 
#define DITHER_BQUANT(x)   ((x)/64 * 64) 
 
#define RICOMP(x, y)       (((x) - (y)) * 7 /16)
#define BOCOMP(x, y)       (((x) - (y)) * 5 /16)
#define BLCOMP(x, y)       (((x) - (y)) * 3 /16)
#define BRCOMP(x, y)       (((x) - (y)) * 1 /16)

typedef struct CMXInfo
{
   int16             nType;   // OEMCMX_TYPE_XXX
   cmx_handle_type * ph;      // Set for Start()/TotalTime command
   uint32            dwCB;
   uint32            dwUserData;
} CMXInfo;

typedef struct CMXDataRequest
{
   uint32   dwPos;
   uint32   dwMaxLen;
   cmx_client_req_cb_func_ptr_type pfnDataReq;
   void *   pServerUserData;
} CMXDataRequest;

#if defined(FEATURE_SVG)
typedef struct SVGURLImageDataInfo
{
   void* handle;
   uint8* buffer;
   uint32 num_bytes;
   char* pURL;
} SVGURLImageDataInfo;
#endif /* defined(FEATURE_SVG) */

#if !defined(FEATURE_BREW_CMX_V3)
/* Structures for QCP recording configuration parameters */
typedef struct {
  cmx_qcp_format_type   format;       /* QCP recording mode            */
  uint32                report_ms;    /* Time callback interval in ms  */
  uint32                auto_stop_ms; /* Silence time to end recording */  
} cmx_qcp_rec_para_type;
#endif // !defined(FEATURE_BREW_CMX_V3)

typedef struct OEMMediaPMD
{
   AEECLSID       m_clsSelf;

   CMXInfo        m_ciStart;
   CMXInfo        m_ciStatus;

   // Play/Ringer
   int16          m_eType; // CMX_AUDFMT_START_XXX

   // QCPRecord
   uint32         m_dwPauseMS;     // Pause between ringers
#ifdef FEATURE_QCP
   cmx_qcp_rec_para_type * m_pQCPRecParm;
#endif
   flg            m_bReverse:1;  // Record locally (ReverseLink)
#if defined(FEATURE_CMX_RAW_PLAYBACK)
   cmx_codec_spec_type *   m_pCodecSpec;
   uint32                  m_dwAudioSync;
#endif

   // MIDIOutQCP
   uint16         m_wVolume;
   uint16         m_wPan;
   uint32         m_dwChannel;
   
#ifdef FEATURE_MULTISEQUENCER
   // Multisequencer
   cmx_audfmt_multi_param_type * m_pMultiParam;
   void *                        m_pSequence;
   cmx_audfmt_seq_param_type *   m_pSeqParam;
   
#endif  //FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
   // PCM Record
   cmx_pcm_rec_para_type * m_pPCMRecParm;
   cmx_pcm_rec_link_type   m_Link;
#endif //FEATURE_PCM_REC

#if defined(FEATURE_SVG)
   // SVG Decode
   cmx_svg_decode_data_type * m_pSVGDecodeData;
   void *                     m_SVGHandle;
#endif //defined(FEATURE_SVG)

} OEMMediaPMD;

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

#define FARF_QUEUE   1
#define FARF_SOURCE  1
#define FARF_MEDIA   1

//$r$
// NOTE: We need to use AEECriticalSection as OEMMediaPMD.c resides in 
// user space and only callback functions get executed in CMX (system)
// context.
static AEECriticalSection gMediaCMXCriticalSection;

#if defined(FEATURE_SVG)
#define OEMMM_DEFAULT_DPI 72

#if (SVGT_VERSION > SVGT_1_1)
#define HTTP_STR "http://"
#define HTTP_STRLEN 7
#endif // (SVGT_VERSION > SVGT_1_1)
#endif // defined(FEATURE_SVG)

#define OEMMM_MAX_CB_CONTEXT_SWITCH    120     // may need to raise this number if too many CMX callbacks come between UI callbacks 
#define OEMMM_BUFFER_SIZE_FACTOR       2048    // Buffer copies allocated will be OEMMM_MAX_CB_CONTEXT_SWITCH / (buffer size / OEMMM_BUFFER_SIZE_FACTOR)

#define OEMMM_ENTER_CRITICAL_SECTION()     AEECriticalSection_Enter(&gMediaCMXCriticalSection); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} AEECriticalSection_Leave(&gMediaCMXCriticalSection);

#define OEMCMX_TYPE_AUDFMT_PLAY     1  // p2 = CMX_AUDFMT_START_XXX
#define OEMCMX_TYPE_AUDFMT_RINGER   2  // p2 = CMX_AUDFMT_START_XXX, p3 = Pause time
#define OEMCMX_TYPE_QCP_RECORD      3  // p2 = cmx_qcp_rec_para_type *, p3 = TRUE/FALSE => ReverseLink(Local)/ForwardLink
#define OEMCMX_TYPE_MIDI_OUT_MSG    4  //
#define OEMCMX_TYPE_MIDI_OUT_QCP    5  // 
#define OEMCMX_TYPE_AUDFMT_PLAY_CODEC  6  // p2 = cmx_codec_spec_type *
#define OEMCMX_TYPE_AUDFMT_PLAY_MULTI  7  // p2 = cmx_audfmt_multi_param_type *
#define OEMCMX_TYPE_PCM_RECORD         8  // p2 = cmx_pcm_rec_para_type *
#if defined(FEATURE_SVG)
#define OEMCMX_TYPE_SVG_DECODE         9  // p2 = cmx_svg_decode_data_type *
#endif // defined(FEATURE_SVG)

#define OEMCMX_ID_TYPE              1  // p1 = CMXInfo *, p2/p3 = see comments in OEMCMX_TYPE_XXX
#define OEMCMX_ID_STATUS_CB         2  // p1 = CMXInfo *
#define OEMCMX_ID_TOTALTIME         3  // p1 = CMXInfo *
#define OEMCMX_ID_VOLUME            4  // p1 = Volume
#define OEMCMX_ID_PAN               5  // p1 = Pan
#define OEMCMX_ID_TEMPO             6  // p1 = Tempo
#define OEMCMX_ID_TUNE              7  // p1 = Tune
#define OEMCMX_ID_MIDI_DEVICE       8  // p1 = TRUE/FALSE => Open/Close, p2 = ADECType, p3 = CMXInfo *
#define OEMCMX_ID_CHANNEL           9  // p1 = Channel #
#define OEMCMX_ID_DATA_RESPONSE     10 // p1 = CMXDataRequest *, p2 = uint8 * (pBuf), p3 = sizeof buffer in p2
#define OEMCMX_ID_SEQUENCE         11 // p1 = Sequence, p2 = cmx_audfmt_seq_param_type *
#define OEMCMX_ID_LINK             12 // p1 = link => forward or reverse link for PCM recording operations
#if defined(FEATURE_SVG)
#define OEMCMX_ID_SVG_KEY          13 // p1 = svg handle, p2 = cmx_svg_key_event_type *,     p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_FOCUS        14 // p1 = svg handle, p2 = focus event,                  p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_POINTER_CLICK  15 // p1 = svg handle, p2 = cmx_svg_pointer_event_type *, p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_POINTER_DOWN   16  // p1 = svg handle, p2 = cmx_svg_pointer_event_type *, p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_POINTER_MOVE   17  // p1 = svg handle, p2 = cmx_svg_pointer_event_type *, p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_POINTER_OVER   18  // p1 = svg handle, p2 = cmx_svg_pointer_event_type *, p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_POINTER_UP     19  // p1 = svg handle, p2 = cmx_svg_pointer_event_type *, p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_POINTER_LEAVE  20  // p1 = svg handle, p2 = cmx_svg_pointer_event_type *, p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_ROTATE         21  // p1 = svg handle, p2 = cmx_svg_rotate_event_type *,  p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_PAN            22  // p1 = svg handle, p2 = cmx_svg_pan_event_type *,     p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_ZOOM           23  // p1 = svg handle, p2 = cmx_svg_zoom_event_type *,    p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_RESET_FRAME    24  // p1 = svg handle
#define OEMCMX_ID_SVG_GENERATE_FRAME 25  // p1 = svg handle, p2 = frame time (ms)
#define OEMCMX_ID_SVG_HANDLE       26 // p1 = svg handle
#define OEMCMX_ID_SVG_KEY_DOWN       27 // p1 = svg handle, p2 = cmx_svg_key_event_type *,     p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_KEY_UP         28 // p1 = svg handle, p2 = cmx_svg_key_event_type *,     p3 = time (ms) since last frame
#define OEMCMX_ID_SVG_URLIMAGE_DATA  29 // p1 = svg handle, p2 = URL data,    
#define OEMCMX_ID_SVG_DOM            30 // result from the DOM to set/get operation

#endif // defined(FEATURE_SVG)

#define OEMMM_CMX_REQ_SIZE_MAX         (4 * sizeof(CMXDataRequest))  // CMX request buffers
#define OEMMM_BUFFER_UNIT_SIZE         (8820)                        // IMedia data fetch unit size
#define OEMMM_BUFFER_SIZE_MAX          (4 * OEMMM_BUFFER_UNIT_SIZE)  // IMedia data buffer size (default)
#define OEMMM_ISSTREAMING(pme)         ((pme)->m_bStreaming)

#define OEMMM_CLIENT_BUF_INIT_COUNT    3     // 1 each for image, animation, text
#define OEMMM_DEF_WIPE_COLOR           MAKE_RGB(0xFF, 0, 0)
#define OEMMM_DEF_TEXT_COLOR           MAKE_RGB(0, 0, 0)


//Stuff needed to draw the images
//Image draw/clear flags
#define  OEMMM_DISP_CLEAR    0x01
#define  OEMMM_DISP_DRAW     0x02

typedef enum OEMMMDrawOp
{
   OEMMM_DRAW_OP_EQ      = 1,
   OEMMM_DRAW_OP_OR_EQ
} OEMMMDrawOp;

typedef struct DrawInfo
{
   uint8 *pDrawFlag;
   uint8 nStatus;   //OEMMM_DISP_XXX
   OEMMMDrawOp eOp;
} DrawInfo;

#define CLEAR_IMAGE  1
#define CLEAR_ANIM   2
#define CLEAR_TEXT   4
#define CLEAR_REDRAW 8  //Redraw must be LAST; Add any items above CLAR_REDRAW.

//SAF constants. These should have been in cmx.h, but are
//in cmx.c instead! Watch for these and fix them if any 
//newer version CMX changes the SAF image bounds.
#define OEMMM_SAF_FRAME_MAXWIDTH   160
#define OEMMM_SAF_FRAME_MAXHEIGHT  160

#define OEMMM_PAL_ALL_WHITE  0x00ffffff

#define  OEMMM_JUST_LEFT     0x01
#define  OEMMM_JUST_RIGHT    0x02
#define  OEMMM_JUST_XCENTR   0x03
#define  OEMMM_JUST_XPCENT   0x04
#define  OEMMM_JUST_XPIXEL   0x05
#define  OEMMM_JUST_TOP      0x10
#define  OEMMM_JUST_BOT      0x20
#define  OEMMM_JUST_YCENTR   0x30
#define  OEMMM_JUST_YPCENT   0x40
#define  OEMMM_JUST_YPIXEL   0x50


#define OEMMM_RINGER_SILENCE_TIME      1000  // 1 sec (default)
#define OEMMM_TICK_PERIOD              1000  // 1 sec (default)

static int  OEMMediaPMD_New(AEECLSID cls, OEMMediaPMD ** ppme);
static int  OEMMediaPMD_PauseResume(OEMMediaPMD * pme, boolean bPause);
#define     OEMMediaPMD_Pause(pme)   OEMMediaPMD_PauseResume(pme, TRUE)
#define     OEMMediaPMD_Resume(pme) OEMMediaPMD_PauseResume(pme, FALSE)
static int OEMMediaPMD_SetParm(OEMMediaPMD * pme, int16 nParmID, int32 p1, int32 p2, int32 p3, boolean bLocal);
static int OEMMediaPMD_Seek(OEMMediaPMD * pme, AEEMediaSeek eSeek, int32 lTimeMS);
static int  OEMMediaPMD_Start(OEMMediaPMD * pme);
static int OEMMediaPMD_Stop(OEMMediaPMD * pme);
static void OEMMediaPMD_Delete(OEMMediaPMD * pme);

#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle   m_hObject;  \
   PFNNOTIFY         m_pfnDelete; \
   OEMMediaPMD*         m_pMediaCMX; \
   \
   uint32            m_dwCaps; \
   uint32            m_dwEnabledCaps; \
   int               m_nAPath; \
   boolean           m_bMute; \
   uint16            m_wVolume; \
   uint16            m_wPan; \
   uint16            m_wTempo; \
   uint16            m_wTune; \
   uint32            m_dwTickTimeMS; \
   uint16            m_wSilenceTimeMS; \
   flg               m_bStopping:1; \
   flg               m_bMIDIOut:1; \
   flg               m_bVolumeScaled:1

OBJECT(CMediaCMX)
{
   INHERIT_CMediaCMX(IMediaMIDI);
};

#ifdef FEATURE_WEBAUDIO
typedef struct TextBlock
{
   AECHAR *    m_text;
   uint16      m_length;
   uint16      m_x;
   uint16      m_y;
   uint32      m_style;
   RGBVAL      m_color;
   RGBVAL      m_wipeColor;
   uint16      m_extent;
   uint16      m_row;
   struct TextBlock * m_next;
} TextBlock;

typedef struct ClientBufInfo
{
   uint8 *        m_buf;  //The buffer passed back to cmx
   uint32         m_size; //Size of buffer
   int            m_copycount;  // Number of copies allocated; may be less than OEMMM_MAX_CB_CONTEXT_SWITCH.
   uint8 *        m_copies[OEMMM_MAX_CB_CONTEXT_SWITCH];  // Copies of the buffer
} ClientBufInfo;

typedef struct ClientBufRequestInfo
{
   uint32                              nClientBufSize;    // size of buffer being requested
   cmx_client_buf_req_cb_func_ptr_type cbClientBufAllocNotify;
   void*                               pBufClientData;
} ClientBufRequestInfo;

typedef struct ContextSwitchCallback
{
   int                  bInUse;     // Callback/Command availability
   AEECallback          cb;         // Pre-loaded and does not change
   IMedia*              pIMedia;    // Pointer to main object
   void*                pPayload;   // The UI context copy of whatever was sent in CMX context
} ContextSwitchCallback;

#if defined(FEATURE_SVG)
#if !defined(CMX_SVG_MAX_URL_LENGTH)
#define CMX_SVG_MAX_URL_LENGTH 248
#endif // !defined(CMX_SVG_MAX_URL_LENGTH)
#endif // defined(FEATURE_SVG)

typedef struct CMediaPMD
{
	AEERect     m_rectImage;
	AEERect     m_rectText;

	cmx_af_char_set_enum_type  m_char_set;  /* Character code set to use for text */

	uint16      m_numToWipe;
	TextBlock * m_textBlock;
	cmx_af_text_ctl_type m_textCtl[OEMMM_MAX_CB_CONTEXT_SWITCH];   
	ContextSwitchCallback m_cbPMDText[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule text processing

	cmx_af_pic_data_type m_picData[OEMMM_MAX_CB_CONTEXT_SWITCH];
	cmx_af_picture_status_type m_picStatus[OEMMM_MAX_CB_CONTEXT_SWITCH];
	ContextSwitchCallback m_cbPicInitialize[OEMMM_MAX_CB_CONTEXT_SWITCH];   // To schedule Initialization of picture bitmap in user context
	ContextSwitchCallback m_cbPicRelease[OEMMM_MAX_CB_CONTEXT_SWITCH];      // To schedule release of picture bitmap in user context
	ContextSwitchCallback m_cbTextRelease[OEMMM_MAX_CB_CONTEXT_SWITCH];     // To schedule release of text in user context

	DrawInfo m_drawInfo[OEMMM_MAX_CB_CONTEXT_SWITCH];
	ContextSwitchCallback m_cbChangeDrawStatus[OEMMM_MAX_CB_CONTEXT_SWITCH];     // To schedule changing the drawing status

	// BMP support
	ContextSwitchCallback m_cbCopyBMPBuf[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule bmp processing

	// PNG support
	byte *      m_pPNGBuf;
	uint32      m_pPNGBufSize;
	ContextSwitchCallback m_cbCopyPNGBuf[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule png processing

	// JPEG Buffer
	byte *      m_pJPEGBuf;
	uint32      m_pJPEGBufSize;
	ContextSwitchCallback m_cbCopyJPEGBuf[OEMMM_MAX_CB_CONTEXT_SWITCH]; // To schedule jpeg processing

	cmx_client_buf_req_cb_func_ptr_type m_cbJPEGBufAllocNotify;
	void*       m_pJPEGClientData;
	AEECallback m_cbJPEGBufAlloc;  // To schedule buffer creation in user context
	AEECallback m_cbJPEGBufFree;   // To schedule buffer release in user context

 #if defined(FEATURE_SAF)
	cmx_saf_info_type m_safInfo[OEMMM_MAX_CB_CONTEXT_SWITCH];
	ContextSwitchCallback m_cbSAFCreateDIB[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule SAF processing
	cmx_saf_data_type m_safData[OEMMM_MAX_CB_CONTEXT_SWITCH];
	ContextSwitchCallback m_cbSAFLoadFrame[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule SAF processing
 #endif  //defined(FEATURE_SAF)

	ContextSwitchCallback m_cbAnimReset[OEMMM_MAX_CB_CONTEXT_SWITCH];      // To schedule resetting animation variables

	IDisplay *  m_pIDisp;
	IDIB *      m_pIAnimBitmap;
	int         m_AnimBitDepth;

	// This is the display information for the animation
	AEERect     m_animDispRect;
	AEERect     m_animOffsetRect;

	IDIB *      m_pIPicBitmap;

	// This is the display information for the picture
	AEERect     m_picDispRect;
	AEERect     m_picOffsetRect;

	IDIB *      m_pIDispBitmap;

	ContextSwitchCallback m_cbPMDImage[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule image drawing
	AEERect     m_dispRect;
	AEERect     m_JPEGRect;
	AEERect     m_offsetRect;
	uint16      m_cxScreen;
	uint16      m_cyScreen;
	byte        m_clrDepth;
	byte        m_xJustPcent_Pixel; // Holds percentage or pixel offset
	byte        m_yJustPcent_Pixel;
	byte        m_fJust;

	ClientBufInfo * m_pClientBufInfos;  // cmx buffers, and copies, allocated in user mode
	int         m_numClientBufs;
	int         m_lastBufAllocated; // indicates most recent buffer allocated, or -1 if allocation failed.
	ClientBufRequestInfo m_ClientBufRequest[OEMMM_MAX_CB_CONTEXT_SWITCH];
	uint8 *     m_ClientBufSent[OEMMM_MAX_CB_CONTEXT_SWITCH];    // address of a buffer sent by cmx to use or to free
	ContextSwitchCallback m_cbClientBufAlloc[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule buffer creation in user context
	ContextSwitchCallback m_cbClientBufFree[OEMMM_MAX_CB_CONTEXT_SWITCH];   // To schedule buffer release in user context

#if defined(FEATURE_SVG)
	cmx_svg_decode_data_type     m_SVGDecodeData;  // For stand-alone SVG playback
	AEEMediaSVGSpec              m_SVGspec;
   uint8                        m_SVGURL[CMX_SVG_MAX_URL_LENGTH];  // For URL requests in SVG files
	uint8                        m_SVGUserPath[FS_FILENAME_MAX_LENGTH_P];  // Path for external file references, as specified by user
	cmx_svg_info_type            m_cmx_svg_info[OEMMM_MAX_CB_CONTEXT_SWITCH];
	cmx_svg_info_type            m_curSVGInfo;
	cmx_svg_frame_done_type      m_SVGFrameData[OEMMM_MAX_CB_CONTEXT_SWITCH];
	NativeColor                  m_SVGTransColor;
	ContextSwitchCallback        m_cbSVGCreateDIB[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule SVG processing
	ContextSwitchCallback        m_cbSVGLoadFrame[OEMMM_MAX_CB_CONTEXT_SWITCH];
	uint32                       m_nUpTimeMS;       // To keep track of timing between frames
	cmx_svg_key_event_type       m_key_event;
	cmx_svg_pointer_event_type   m_pointer_event;
	cmx_svg_rotate_event_type    m_rotate_event;
	cmx_svg_pan_event_type       m_pan_event;
	cmx_svg_zoom_event_type      m_zoom_event;

#if (SVGT_VERSION > SVGT_1_1)
   SVGURLImageDataInfo          m_SVGURLImageData;    // For passing info to CMX layer
   SVGURLImageRequestData       m_SVGURLImageRequest; // To notify app of URL rquest
   SVGURLImageDataAck           m_SVGURLImageDataAck; // To acknowledge to app that URL data loaded
#endif // (SVGT_VERSION > SVGT_1_1)

#if defined(MM_STATUS_SVG_URL_IMAGE)
   AEEMediaData             m_mdDummy;  // Used to fool base class when playing empty DOM doc
   char                     m_cDummyBuf[1];  // Used to fool base class when playing empty DOM doc
#endif // defined(MM_STATUS_SVG_URL_IMAGE)

#if defined(FEATURE_SVGDOM)
   cmx_svgdom_rect_type         m_DOMRect;
   cmx_svgdom_color_type        m_DOMColor;
   cmx_svgdom_svgpath_data_type m_DOMPathData;
   
   // DOM data returned to caller
   SVGDOMCallbackData           m_DOMCallbackData;
   union
   {
      AECHAR                       m_str[CMX_SVG_MAX_URL_LENGTH];
      boolean                      m_bool;
      int32                        m_long;
      uint16                       m_short;
      uint32                       m_fixed;  // 16.16. fixed
      SVGRect                      m_rect;
      RGBVAL                       m_RGB;
      SVGPoint                     m_point;
   } DOMRet;
   
   void                    *pDocument;            // Media spec data specific to this implementation (DOM + extensions)
   void                    *m_pISVGDOM;           // associated SVGDOM object handle
   PFNMEDIANOTIFY           m_pfn;                // associated SVGDOM callback method

#endif //defined(FEATURE_SVGDOM)
#endif //defined(FEATURE_SVG)

	IBitmap*    m_pCallerFrameBmp;

	// These were changed from flg type so they can be used by reference
	uint8       m_fTextDraw;
	uint8       m_fPicDraw;
	uint8       m_fAnimDraw;

	flg         m_bHeaderRead:1;
	flg         m_displayedSaf:1;  // This flag is true if an animation has ever been displayed.
	flg         m_transparency:1;  // This flag is true if an animation contains transparency.
	flg         m_displayedPic:1;  // This flag is true if an image has ever been displayed.

#if defined(FEATURE_SVG)
	flg         m_svgDecoderCalled:1;  // This flag is true if cmx_svg_decode() has been called.
	flg         m_bNoFramesShown:1;  // To be able to tell if a frame is the initial frame
#endif // defined(FEATURE_SVG)

	flg         m_FrameCallbackEnabled:1;  // True if caller has requested frame callbacks
	flg         m_DispAreaChanged:1; // True if the media display area is altered

#if defined(FEATURE_SVG)
#if (SVGT_VERSION > SVGT_1_1)
   flg         m_SVGURLImageHandled:1;  // This flag is true if the app handles fetching URL images
#endif // (SVGT_VERSION > SVGT_1_1)

#if defined(FEATURE_SVGDOM)
   flg         m_bQISVGDOM:1;          // SVGDOM creation in progress
#endif // defined(FEATURE_SVGDOM)
#endif // defined(FEATURE_SVG)

	uint16      m_nCallbacksSkipped; // Use diagnostically to adjust OEMMM_MAX_CB_CONTEXT_SWITCH
} CMediaPMD;
#endif // FEATURE_WEBAUDIO

/*===============================================================================

      CMediaPMDBg: Background Audio (MIDI, MP3, QCP. Includes PMD) related.

===============================================================================*/
typedef struct CMediaPMDBg          CMediaPMDBg;

typedef struct CQueue
{
   uint8 *  pBuffer;    // Size of the queue
   int32    nMaxSize;   // Max size (bytes) of the queue
   int32    nWrite;     // Write pointer
   int32    nRead;      // Read pointer
   int32    nFill;      // Size (bytes) of data filled in the queue
   boolean  bMutex;     // Specifies if mutex lock is required for operations
} CQueue;

static int     CQueue_New(int32 nSize, boolean bMutex, CQueue ** ppQ);
static void    CQueue_Delete(CQueue * pme);
static int32   CQueue_Put(CQueue * pme, uint8 * pData, int32 nSize);
static int32   CQueue_Get(CQueue * pme, uint8 * pData, int32 nSize);
#define        CQueue_MaxSize(pme)           ((pme)->nMaxSize)
#define        CQueue_Size(pme)              ((pme)->nFill)
#define        CQueue_IsEmpty(pme)           (0 == (pme)->nFill)
#define        CQueue_IsFull(pme)            ((pme)->nFill >= (pme)->nMaxSize)
#define        CQueue_IsPutOk(pme, nSize)    ((pme)->nFill + (nSize) <= (pme)->nMaxSize)
#define        CQueue_Flush(pme)             ((pme)->nWrite = (pme)->nRead = (pme)->nFill = 0)
#define        CQueue_GetBuffer(pme)         ((pme)->pBuffer)



#define OEMMM_MAX_CB_PLAY              3     // simultaneous play/record cbs 

struct CMediaPMDBg
{
   INHERIT_CMediaCMX(IMediaMIDI);

#ifdef FEATURE_WEBAUDIO
   union u
   {
      CMediaPMD *    m_pPMD;
   } u;
#endif // FEATURE_WEBAUDIO

   AEEMediaCallback  m_cbPlay[OEMMM_MAX_CB_PLAY];
   AEEMediaCallback  m_cbCommand;
   AEEMediaCallback  m_cbGetTotalTime;

   ISound *          m_pISound;

   CQueue *          m_pqCMXDataReq;
   AEECallback *     m_pcbCMXDataReq;
   CQueue *          m_pqMediaData;
   AEECallback *     m_pcbMediaData;
   uint8 *           m_pSrcBuffer;
   uint32            m_dwSrcBufferSize;

#if defined(FEATURE_CMX_AV_SYNC)
   uint32            m_dwAudioSyncType;   // Time based or sample based
   uint32            m_dwAudioSyncData;   // Time in ms or samples
   AEEMediaAudioSync *m_pAudioSyncInfo;   // CB info
   uint64            m_qwAudioSyncBytes;
#endif //defined(FEATURE_CMX_AV_SYNC)

   flg               m_bStreaming:1;
   flg               m_bEOS:1;
   void   *          m_pSequence;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   uint32            m_dwNotes;
   boolean           m_bISynthQualityRef;
#endif
};

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#define OEMMM_MAX_NOTES    72 // Accommodate as many notes as possible (HP_ALL mode)
static uint32              gdwNotes = OEMMM_MAX_NOTES;
static boolean             gbNotesSet = FALSE;
static ISynthQuality      *gpISynthQuality = NULL;
#endif

#ifdef FEATURE_WEBAUDIO
static ClientBufInfo* GetClientBufInfo(CMediaPMDBg * pme, uint8 * pClientBuf);
static void InitCSCallback(IMedia * po, ContextSwitchCallback * pcb, int nMax, PFNNOTIFY pfn);
static int LockCSCallback(ContextSwitchCallback * pcb, int nMax, CMediaPMDBg *pme);
static void UnlockCSCallback(ContextSwitchCallback * pcb);
static void CancelCSCallback(ContextSwitchCallback * pcb, int nMax);
#endif

static void CMediaPMDBg_Delete(CMediaPMDBg * pme);
static int CMediaPMDBg_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo);
static int CMediaPMDBg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int CMediaPMDBg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
static int CMediaPMDBg_Play(IMedia * po);
static int CMediaPMDBg_Record(IMedia * po);
static int CMediaPMDBg_Stop(IMedia * po);
static int CMediaPMDBg_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS);
static int CMediaPMDBg_Pause(IMedia * po);
static int CMediaPMDBg_Resume(IMedia * po);
static int CMediaPMDBg_GetTotalTime(IMedia * po);
static void CMediaPMDBg_CallbackNotify(void * pUser);

static uint32 CMediaCMX_Release(IMedia * po);
static int    CMediaCMX_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2);
static int     CMediaCMX_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2);
void CMediaCMX_CallbackNotify(void * pUser);


static void CMediaCMX_Delete(CMediaCMX * po);

typedef enum OEMMMStatus
{
   OEMMM_STATUS_PLAY = 0,
   OEMMM_STATUS_PLAYCTL,
   OEMMM_STATUS_SETPARM
} OEMMMStatus;

typedef enum OEMMMCtlCmd
{
   OEMMM_CTLCMD_NONE = 0,
   OEMMM_CTLCMD_SEEK,
   OEMMM_CTLCMD_PAUSE,
   OEMMM_CTLCMD_RESUME,
   OEMMM_CTLCMD_STOP
} OEMMMCtlCmd;

/*===========================================================================
      MMLayer functions
===========================================================================*/

#define CMediaPMDMMLayer_ISPLAYCTL(pcb)     (CMediaPMDMMLayer_ISCOMMAND(pcb) && CMediaPMDMMLayer_ISCMDPLAYREC(pcb) \
                                          && (pcb)->cmdNotify.nSubCmd > OEMMM_CTLCMD_NONE)
#define CMediaPMDMMLayer_ISCMDPLAYREC(pcb)  ((pcb)->cmdNotify.nCmd == MM_CMD_PLAY \
                                          || (pcb)->cmdNotify.nCmd == MM_CMD_RECORD)
#define CMediaPMDMMLayer_ISCOMMAND(pcb)     ((pcb)->bInUse)

static void CMediaPMDMMLayer_Init(IShell * ps);
static void CMediaPMDMMLayer_AEEExitNotify(void * pUser);
static void CMediaPMDMMLayer_InitCallback(IMedia * po, int nCmd, AEEMediaCallback * pcb, int nMax, PFNNOTIFY pfn);
static void CMediaPMDMMLayer_FreeCallback(AEEMediaCallback * pcb);
static void CMediaPMDMMLayer_CancelCallback(AEEMediaCallback * pcb, int nMax);
static int CMediaPMDMMLayer_SetSource(CMediaCMX * pme, cmx_handle_type * ph);
static void CMediaPMDMMLayer_ReadSource(CMediaPMDBg * pme);
static void CMediaPMDMMLayer_RequestSourceData(CMediaPMDBg * pme);
static void CMediaPMDMMLayer_CMXDataReqNotify(CMediaPMDBg * pme);
void CMediaPMDMMLayer_CMXDataRequestCB(uint32 position, uint32 max_len, cmx_client_req_cb_func_ptr_type data_req_cb, 
                                           const void *server_client_data, const void *client_data);
static void CMediaPMDMMLayer_FlushStreamData(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_GetMMStatus(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, OEMMMStatus * peStatus);
static int CMediaPMDMMLayer_GetMMStatus2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, OEMMMStatus * peStatus);
#if defined(FEATURE_SVG)
static void CMediaPMDMMLayer_StatusCB2(cmx_status_type status, const void * pClientData);
static void CMediaPMDMMLayer_PlayCB2(cmx_status_type status, const void * pClientData, void* data, uint32 num_bytes);
#endif // defined(FEATURE_SVG)
static void CMediaPMDMMLayer_StatusCB(cmx_status_type status, const void * pClientData);
static void CMediaPMDMMLayer_PlayCB(cmx_status_type status, const void * pClientData, cmx_af_server_data_type * psd);
static void CMediaPMDMMLayer_ProcessPlayServerData(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, AEEMediaCallback * pcb);
static void CMediaPMDMMLayer_SetMediaSpec(CMediaPMDBg * pme, cmx_af_audio_spec_type * psd, AEEMediaCallback * pcb);

/*---------------------------------------------------------------------------
      Media play/record
-----------------------------------------------------------------------------*/
#if defined(FEATURE_SVG)
static int CMediaPMDMMLayer_SetSVGInput(CMediaPMDBg * pme, uint32 * pFrameTime);
static int CMediaPMDMMLayer_SetSVG_EVENT_KEY(CMediaPMDBg * pme, SVGKeyData * pKeyData, int32 nEvent);
static int CMediaPMDMMLayer_SetSVG_EVENT_FOCUS(CMediaPMDBg * pme, int16 nFocusEvent);
static int CMediaPMDMMLayer_SetSVG_EVENT_POINTER(CMediaPMDBg * pme, SVGPointerData * pPointerData, int16 commandID);
static int CMediaPMDMMLayer_SetSVG_EVENT_ROTATE(CMediaPMDBg * pme, SVGRotateData * pRotateData);
static int CMediaPMDMMLayer_SetSVG_EVENT_PAN(CMediaPMDBg * pme, SVGPanData * pPanData);
static int CMediaPMDMMLayer_SetSVG_EVENT_ZOOM(CMediaPMDBg * pme, SVGZoomData * pZoomData);
static int CMediaPMDMMLayer_SetSVG_EVENT_RESET_FRAME(CMediaPMDBg * pme);
#if (SVGT_VERSION > SVGT_1_1)
static int CMediaMMLayer_SetSVG_URLIMAGE_DATA(CMediaPMDBg * pme, SVGURLImageData* pData);
#endif // (SVGT_VERSION > SVGT_1_1)
#endif //defined(FEATURE_SVG)

#if defined(FEATURE_WEBAUDIO)
static int CMXImage_Process(CMediaPMDBg * pme, cmx_status_type status, cmx_af_server_data_type * psd);
#endif
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SVG)
static int CMXImage_Process2(CMediaPMDBg * pme, cmx_status_type status, void* data, uint32 num_bytes);
#endif 

#ifdef FEATURE_MULTISEQUENCER
/* List of all class ids that are supported in the MultiSequence API */

static AEECLSID multiSequencerClasses[] = {AEECLSID_MEDIAMIDI,
                                           AEECLSID_MEDIAMMF, 
                                           AEECLSID_MEDIAPMD,
                                           AEECLSID_MEDIAPHR,
                                           AEECLSID_MEDIAXMF};

static boolean CMediaPMDMMLayer_IsMultiSequence(CMediaPMDBg *pme);
#endif /* FEATURE_MULTISEQUENCER */

#if defined(FEATURE_SVG)
static void CMediaPMDMMLayer_ProcessPlayServerData2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, AEEMediaCallback * pcb);
static void CopySVGInfo(AEEMediaSVGSpec* dest, cmx_svg_info_type* src);
#endif //defined(FEATURE_SVG)

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
static int CMediaPMDMMLayer_SetSynthQuality(CMediaPMDBg *pme);
#endif

/*---------------------------------------------------------------------------
      Sound functions
-----------------------------------------------------------------------------*/
static void CMediaPMDMMLayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eSPStatus, uint32 dwParam);
static int CMediaPMDMMLayer_SetVolume(CMediaPMDBg * pme, uint16 wVolume);
static int CMediaPMDMMLayer_SetDevice(CMediaPMDBg * pme, AEESoundDevice eDevice);
static int CMediaPMDMMLayer_SetPath(CMediaPMDBg * pme, int nAPath);
static int CMediaPMDMMLayer_SetMute(CMediaPMDBg * pme, boolean bMute);

/*---------------------------------------------------------------------------
      Media play/record
-----------------------------------------------------------------------------*/
static int CMediaPMDMMLayer_Play(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_PlayCodec(CMediaPMDBg * pme, cmx_handle_type * ph, AEEMediaCallback * pcb);
static int CMediaPMDMMLayer_Stop(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_Pause(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_Resume(CMediaPMDBg * pme);
static AEEMediaCallback * CMediaPMDMMLayer_AllocCallback(AEEMediaCallback * pcb, int nMax, int nCmd, int nSubCmd);
static int CMediaPMDMMLayer_GetTotalTime(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_Seek(CMediaPMDBg * pme, AEEMediaSeek eSeek, int32 lTimeMS);

static int CMediaPMDMMLayer_SetTempo(CMediaPMDBg * pme, uint32 dwTempo);
static int CMediaPMDMMLayer_SetTune(CMediaPMDBg * pme, uint32 dwTune);
static int CMediaPMDMMLayer_SetPan(CMediaPMDBg * pme, uint32 dwTune);

static void CMediaPMDMMLayer_TimeCB(cmx_status_type status, const void * pClientData, uint32 dwTimeMS);

static size_t strlcpy(char * dst, const char * src, size_t nSize);

#if defined(FEATURE_SAF)
static void     SetSAFOffsets(CMediaPMDBg * pme, cmx_saf_data_type *pSAFData);
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG)
static void     SetAnimOffsets(CMediaPMDBg * pme);
#endif //defined(FEATURE_SVG)
#ifdef FEATURE_AUDIO_POSTCARD
static void     JPEGCopyBuffer(void * pUser);
#endif /* FEATURE_AUDIO_POSTCARD */
#if defined(FEATURE_SAF)
static void     SAFCreateDIB(void * pUser);
static void     SAFLoadFrameToDIB(void * pUser);
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG)
static void     SVGCreateDIB(void * pUser);
static void     SVGLoadFrameToDIB(void * pUser);
static boolean  GetSVGDirNativePath(CMediaPMDBg * pme, void * pData);
#endif //defined (FEATURE_SVG)
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)
static void     FreeJPEGBuf(void * pUser);
static void     SetImageOffsets(CMediaPMDBg * pme);
#endif //defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) \
	|| defined(FEATURE_SVG) || defined(FEATURE_AUDIO_POSTCARD)
static void     CMXImage_Notify(void * pUser);
#endif
#if defined(FEATURE_WEBAUDIO) 
static void     AllocJPEGBuf(void * pUser);
static void     AllocClientBuf(void * pUser);
static void     FreeClientBuf(void * pUser);
static void     TextRelease(void * pUser);
static void     ProcessPMDText(void * pUser);
static void     PICInitialize(void * pUser);
static void     PICRelease(void * pUser);
static void     BMPCopyBuffer(void * pUser);
static void     PNGCopyBuffer(void * pUser);
static void     CMXClear(CMediaPMDBg * pme, uint8 nClearOptions);
static void     CMXChangeDrawStatus(CMediaPMDBg * pme, uint8* pDrawFlag, uint8 nStatus, OEMMMDrawOp eOp, boolean bRedraw);
static void     CMXChangeDrawStatusFlag(void * pUser);
static void     AnimReset(void * pUser);
#endif //defined(FEATURE_WEBAUDIO)

static const VTBL(IMediaMIDI) gMediaBgFuncs =
{
   AEEMedia_AddRef,
   CMediaCMX_Release,
   CMediaPMDBg_QueryInterface,
   AEEMedia_RegisterNotify,
   CMediaPMDBg_SetMediaParm,
   CMediaPMDBg_GetMediaParm,
   CMediaPMDBg_Play,
   CMediaPMDBg_Record,
   CMediaPMDBg_Stop,
   CMediaPMDBg_Seek,
   CMediaPMDBg_Pause,
   CMediaPMDBg_Resume,
   CMediaPMDBg_GetTotalTime,
   AEEMedia_GetState
};

/*==================================================================
   Constructor
==================================================================*/
static IBase * CMediaCMX_New(IShell * ps, AEECLSID cls, const void * pVtbl, uint32 dwSize, PFNNOTIFY pfnDelete)
{
   IMedia *       po;
   CMediaCMX *    pme;

   // Alloc memory for the object
   po = (IMedia *)AEE_NewClass((IBaseVtbl *)pVtbl, dwSize);
   if (!po)
      return NULL;

   pme = (CMediaCMX *)po;

   // Call base class constructor
   if (SUCCESS != AEEMedia_New(po, ps, cls))
      goto Done;

   if (SUCCESS != OEMMediaPMD_New(cls, &pme->m_pMediaCMX))
      goto Done;

   // Initialize MM Layer
   CMediaPMDMMLayer_Init(ps);

   pme->m_pfnDelete = pfnDelete;

   // Register the object with ObjectMgr...
   if (AEEObjectMgr_Register(pme, &pme->m_hObject))
      goto Done;

   pme->m_dwCaps |= MM_CAPS_AUDIO;
   if (pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
      || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
       )
      pme->m_dwCaps |= (MM_CAPS_VIDEO | MM_CAPS_TEXT);
   pme->m_dwEnabledCaps = pme->m_dwCaps;

   pme->m_wVolume = (3 * AEE_MAX_VOLUME) / 4;
   pme->m_nAPath = MM_APATH_LOCAL;
   pme->m_wPan = 64;
   pme->m_bVolumeScaled = FALSE;

   return (IBase *)po;

Done:
   CMediaCMX_Delete(pme);
//   FREE(pme);
   return NULL;
}


/*==================================================================

==================================================================*/
static uint32 CMediaCMX_Release(IMedia * po)
{
   int32    nRef = (int32)AEEMedia_Release(po);

   if (!nRef)
   {
#if defined(FEATURE_SVGDOM)
      if (((CMediaBg *)po)->m_clsSelf == AEECLSID_MEDIASVG &&
          ((CMediaBg *)po)->u.m_pPMD && ((CMediaBg *)po)->u.m_pPMD->m_pISVGDOM)
      {
         ISVGDOM_Release(((CMediaBg *)po)->u.m_pPMD->m_pISVGDOM);
      }
#endif // defined(FEATURE_SVGDOM)
      CMediaCMX_Delete((CMediaCMX *)po);
   }

   return nRef;
}

/*==================================================================
   
==================================================================*/
static int CMediaPMDBg_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaPMDBg *  pme = (CMediaPMDBg *)po;
   int         nRet = EUNSUPPORTED;
   boolean     bProc = TRUE;

#if defined(FEATURE_SVG)
   bProc = TRUE;
   if (pme->m_clsSelf == AEECLSID_MEDIASVG && pme->u.m_pPMD)
   {
      switch (nParmID)
      {
         case MM_PARM_SVG_DPI:
            pme->u.m_pPMD->m_SVGDecodeData.dpi = (uint16)p1;
            nRet = SUCCESS;
            break;

         case MM_PARM_SVG_DIR:
            STRLCPY((char*)pme->u.m_pPMD->m_SVGUserPath, (char*)p1, sizeof(pme->u.m_pPMD->m_SVGUserPath));
            nRet = SUCCESS;
            break;

         case MM_PARM_SVG_CLIENT_FRAME_GENERATION:
            if (p1)
               pme->u.m_pPMD->m_SVGDecodeData.cmx_svg_decode_mode = CMX_SVG_MODE_CLIENT;
            else
               pme->u.m_pPMD->m_SVGDecodeData.cmx_svg_decode_mode = CMX_SVG_MODE_CMX;
            nRet = SUCCESS;
            break;

         case MM_PARM_SVG_FRAMEDELAY:
            pme->u.m_pPMD->m_SVGDecodeData.delay = (uint32)p1;
            nRet = SUCCESS;
            break;

         case MM_PARM_SVG_BACKGROUND_COLOR:
            pme->u.m_pPMD->m_SVGDecodeData.bk_color = (uint32)p1;
            nRet = SUCCESS;
            break;

#if (SVGT_VERSION > SVGT_1_1)
         case MM_PARM_SVG_PROGRESSIVE_RENDER:
            pme->u.m_pPMD->m_SVGDecodeData.progressive = (boolean)p1;
            nRet = SUCCESS;
            break;

         case MM_PARM_SVG_AUTO_STOP:
            pme->u.m_pPMD->m_SVGDecodeData.auto_stop = (boolean)p1;
            nRet = SUCCESS;
            break;

         case MM_PARM_SVG_URLIMAGE_HANDLED:
         {
            boolean  bEnable = (boolean)p1;
            boolean bChanging;
            int nState = IMEDIA_GetState(po, &bChanging);
            if (nState == MM_STATE_READY || nState == MM_STATE_IDLE)
            {
               if (pme->u.m_pPMD->m_SVGURLImageHandled != bEnable)
               {
                  pme->u.m_pPMD->m_SVGURLImageHandled = bEnable;
               }
               nRet = SUCCESS;
            }
            else
               nRet = EBADSTATE;
         }
         break;

         case MM_PARM_SVG_TRANSPARENCY:
            pme->u.m_pPMD->m_SVGDecodeData.transparency = (flg)(p1 & 0x01);
            nRet = SUCCESS;
            break;

#if defined(MM_STATUS_SVG_URL_IMAGE)
         case MM_PARM_SVG_DOM_EMPTY_DOC:
            pme->u.m_pPMD->m_SVGDecodeData.empty = (boolean)p1;
            nRet = SUCCESS;
            if (p1)
            {
               pme->u.m_pPMD->m_mdDummy.clsData = MMD_BUFFER;
               pme->u.m_pPMD->m_mdDummy.pData = pme->u.m_pPMD->m_cDummyBuf;
               pme->u.m_pPMD->m_mdDummy.dwSize = sizeof(pme->u.m_pPMD->m_cDummyBuf);
               
               nRet = IMEDIA_SetMediaData(po, &pme->u.m_pPMD->m_mdDummy);
            }
            break;
#endif // defined(MM_STATUS_SVG_URL_IMAGE)
#endif // (SVGT_VERSION > SVGT_1_1)

         default:
            bProc = FALSE;
            break;
      }

      if (bProc)
         return nRet;
   }
#endif //defined(FEATURE_SVG)

   bProc = TRUE;
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
      {
         nRet = CMediaPMDMMLayer_SetVolume(pme, (uint16)p1);
         if (!nRet || nRet == MM_PENDING)
            pme->m_wVolume = (uint16)p1;
         break;
      }

      case MM_PARM_AUDIO_DEVICE:
         nRet = CMediaPMDMMLayer_SetDevice(pme, (AEESoundDevice)p1);
         break;

      case MM_PARM_AUDIO_PATH:
         nRet = CMediaPMDMMLayer_SetPath(pme, (int)p1);
         break;

      case MM_PARM_MUTE:
         nRet = CMediaPMDMMLayer_SetMute(pme, (boolean)p1);
         break;

      case MM_PARM_TEMPO:
         if (AEEMedia_IsPlayOn(pme))
            nRet = CMediaPMDMMLayer_SetTempo(pme, (uint32)p1);
         else
            nRet = EBADSTATE;
         break;

      case MM_PARM_TUNE:
         if (AEEMedia_IsPlayOn(pme))
            nRet = CMediaPMDMMLayer_SetTune(pme, (uint32)p1);
         else
            nRet = EBADSTATE;
         break;

      case MM_PARM_PAN:
         if (AEEMedia_IsPlayOn(pme))
            nRet = CMediaPMDMMLayer_SetPan(pme, (uint32)p1);
         else
            nRet = EBADSTATE;
         break;

      case MM_PARM_POS:
      case MM_PARM_PLAY_REPEAT:
         break;
         
      case MM_PARM_TICK_TIME:
         if (pme->m_clsSelf == AEECLSID_MEDIAPCM)         
         {
            pme->m_dwTickTimeMS = (int)p1;
            nRet = SUCCESS;
         }
         break;

#if defined(FEATURE_WEBAUDIO)
      case MM_PARM_RECT:
         if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
             || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
             ) && pme->u.m_pPMD)
         {
            if (p1)
            {
               MEMCPY(&pme->u.m_pPMD->m_rectImage, (void *)p1, sizeof(AEERect));
               pme->u.m_pPMD->m_DispAreaChanged = TRUE;
            }
            if (p2)
               MEMCPY(&pme->u.m_pPMD->m_rectText, (void *)p2, sizeof(AEERect));
            nRet = SUCCESS;
         }
         break;

      case MM_PARM_FRAME:
         if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
             || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
             ) && pme->u.m_pPMD)
         {
            pme->u.m_pPMD->m_FrameCallbackEnabled = p1;
            nRet = SUCCESS;
         }
         break;
#endif // defined(FEATURE_WEBAUDIO)

      case MM_PARM_CHANNEL_SHARE:
      {
           boolean  bEnable = (boolean)p1;
           boolean bChanging;
           int nState = IMEDIA_GetState(po, &bChanging);
           if (nState == MM_STATE_READY || nState == MM_STATE_IDLE)
           {
              if (pme->m_bChannelShare != bEnable)
              {
                pme->m_bChannelShare = bEnable;
              }
              nRet = SUCCESS;
           }
           else
              nRet = EBADSTATE;
      }
      break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
      case MM_PARM_NOTES:
         if (CMediaPMDMMLayer_IsMultiSequence(pme))
         {
            if (AEEMedia_IsReadyState(pme))
            {
               // Create ISynthQuality instance if not already done.
               if (!pme->m_bISynthQualityRef)
               {
                  pme->m_bISynthQualityRef = TRUE;
                  if (!gpISynthQuality)
                     ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SYNTHQUALITY, (void **)&gpISynthQuality);
                  else
                     ISYNTHQUALITY_AddRef(gpISynthQuality);
               }

               // Update the number of notes.
               if (!gbNotesSet) // Uninitialized and default
               {
                  gdwNotes = pme->m_dwNotes = (uint32)p1;
                  gbNotesSet = TRUE;
               }
               else
               {
                  // Notes already set. So, set the new number of notes.
                  gdwNotes -= pme->m_dwNotes;
                  pme->m_dwNotes = (uint32)p1;
                  gdwNotes += pme->m_dwNotes;
               }
               nRet = SUCCESS;
            } // if (AEEMedia_IsReadyState(pme))
            else
               nRet = EBADSTATE;
         } // if (CMediaPMDMMLayer_IsMultiSequence(pme))
         break;
#endif

#if defined(FEATURE_CMX_AV_SYNC)
      case MM_PARM_AUDIOSYNC:
      {
         if (p1 && (MM_AUDIOSYNC_MODE_TIME == p1 || MM_AUDIOSYNC_MODE_SAMPLE == p1))
         {
            pme->m_dwAudioSyncType = (uint32)p1;
            pme->m_dwAudioSyncData = (uint32)p2;
         }
         else
            nRet = EBADPARM;
      }
      break;
#endif //defined(FEATURE_CMX_AV_SYNC)

#if defined(FEATURE_SVG)
      case MM_PARM_SVG_INPUT:
         switch (p1)
         {
            case MM_SVG_EVENT_KEY:
#if (SVGT_VERSION > SVGT_1_1)
            case MM_SVG_EVENT_KEY_DOWN:
            case MM_SVG_EVENT_KEY_UP:
#endif // (SVGT_VERSION > SVGT_1_1)
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_KEY(pme, (SVGKeyData*)p2, p1);
               break;
            case MM_SVG_EVENT_FOCUS:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_FOCUS(pme, (int16)p2);
               break;
            case MM_SVG_EVENT_POINTER_CLICK:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_POINTER(pme, (SVGPointerData*)p2, OEMCMX_ID_SVG_POINTER_CLICK);
               break;
            case MM_SVG_EVENT_POINTER_DOWN:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_POINTER(pme, (SVGPointerData*)p2, OEMCMX_ID_SVG_POINTER_DOWN);
               break;
            case MM_SVG_EVENT_POINTER_MOVE:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_POINTER(pme, (SVGPointerData*)p2, OEMCMX_ID_SVG_POINTER_MOVE);
               break;
            case MM_SVG_EVENT_POINTER_OVER:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_POINTER(pme, (SVGPointerData*)p2, OEMCMX_ID_SVG_POINTER_OVER);
               break;
            case MM_SVG_EVENT_POINTER_UP:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_POINTER(pme, (SVGPointerData*)p2, OEMCMX_ID_SVG_POINTER_UP);
               break;
            case MM_SVG_EVENT_POINTER_LEAVE:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_POINTER(pme, (SVGPointerData*)p2, OEMCMX_ID_SVG_POINTER_LEAVE);
               break;
            case MM_SVG_EVENT_ROTATE:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_ROTATE(pme, (SVGRotateData*)p2);
               break;
            case MM_SVG_EVENT_PAN:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_PAN(pme, (SVGPanData*)p2);
               break;
            case MM_SVG_EVENT_ZOOM:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_ZOOM(pme, (SVGZoomData*)p2);
               break;
            case MM_SVG_EVENT_RESET_FRAME:
               nRet = CMediaPMDMMLayer_SetSVG_EVENT_RESET_FRAME(pme);
               break;

#if defined(FEATURE_SVGDOM)
            case MM_SVG_EVENT_FOCUS_IN:
               nRet = EUNSUPPORTED;
               break;

            case MM_SVG_EVENT_FOCUS_OUT:
               nRet = EUNSUPPORTED;
               break;

            case MM_SVG_EVENT_DOM_ACTIVATE:
               nRet = EUNSUPPORTED;
               break;
#endif //defined(FEATURE_SVGDOM)

            default:
               bProc = FALSE;
               break;
         }
         break;
      
      case MM_PARM_SVG_GENERATE_FRAME:
         // No callback is allocated for this call
         nRet = OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_GENERATE_FRAME, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)p1, 0, FALSE);
         if (SUCCESS == nRet && pme->u.m_pPMD->m_FrameCallbackEnabled)
            nRet = MM_PENDING;
         break;

#if (SVGT_VERSION > SVGT_1_1)
      case MM_PARM_SVG_URLIMAGE_DATA:
         nRet = CMediaMMLayer_SetSVG_URLIMAGE_DATA(pme, (SVGURLImageData*)p1);
         break;
#endif // (SVGT_VERSION > SVGT_1_1)
#endif //defined(FEATURE_SVG)

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_SetMediaParm(po, nParmID, p1, p2);
}

/*==================================================================
   
==================================================================*/
static int CMediaPMDBg_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaPMDBg *  pme = (CMediaPMDBg *)po;
   int         nRet = SUCCESS;
   boolean     bProc = TRUE;

#if defined(FEATURE_SVG)
   bProc = TRUE;
   if (pme->m_clsSelf == AEECLSID_MEDIASVG && pme->u.m_pPMD)
   {
      switch (nParmID)
      {
         case MM_PARM_SVG_DPI:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGDecodeData.dpi;
            break;

         case MM_PARM_SVG_DIR:
            if (pP1 && pP2)
               STRLCPY((char*)pP1, (char*)pme->u.m_pPMD->m_SVGUserPath, *pP2);
            break;

         case MM_PARM_SVG_CLIENT_FRAME_GENERATION:
            if (pP1)
               *pP1 = (pme->u.m_pPMD->m_SVGDecodeData.cmx_svg_decode_mode == CMX_SVG_MODE_CLIENT);
            break;

         case MM_PARM_SVG_FRAMEDELAY:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGDecodeData.delay;
            break;

         case MM_PARM_SVG_BACKGROUND_COLOR:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGDecodeData.bk_color;
            break;
 
#if (SVGT_VERSION > SVGT_1_1)
         case MM_PARM_SVG_PROGRESSIVE_RENDER:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGDecodeData.progressive;
            break;

         case MM_PARM_SVG_AUTO_STOP:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGDecodeData.auto_stop;
            break;

         case MM_PARM_SVG_URLIMAGE_HANDLED:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGURLImageHandled;
            break;

         case MM_PARM_SVG_TRANSPARENCY:
            if (pP1)
               *pP1 = (int32)pme->u.m_pPMD->m_SVGDecodeData.transparency;
            break;
            
#if defined(MM_STATUS_SVG_URL_IMAGE)
         case MM_PARM_SVG_DOM_EMPTY_DOC:
            if (pP1)
               *pP1 = pme->u.m_pPMD->m_SVGDecodeData.empty;
            break;
#endif // defined(MM_STATUS_SVG_URL_IMAGE)

#endif // (SVGT_VERSION > SVGT_1_1)

#if defined(MM_PARM_SVG_VERSION)
         case MM_PARM_SVG_VERSION:
            if (pP1)
            {
#if (SVGT_VERSION > SVGT_1_1)
               *pP1 = (int32)SVGT_VERSION_12;
#else
               *pP1 = (int32)SVGT_VERSION_11;
#endif // (SVGT_VERSION > SVGT_1_1)
            }
            else
               nRet = EBADPARM;
            break;
#endif //defined(MM_PARM_SVG_VERSION)

         default:
            bProc = FALSE;
            break;
      }

      if (bProc)
         return nRet;
   }
#endif //defined(FEATURE_SVG)

   bProc = TRUE;
   switch (nParmID)
   {
      case MM_PARM_VOLUME:
         // No need to alloc a command callback since the callback from
         // ISound uniquely identifies this operation and also
         // gets called in BREW context.
         ISOUND_Get(pme->m_pISound, NULL);	// Update the current device settings
         ISOUND_GetVolume(pme->m_pISound);
         nRet = MM_PENDING;
         break;

      case MM_PARM_AUDIO_DEVICE:
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_AUDIO_PATH:
         if (pP1)
            *pP1 = (int32)pme->m_nAPath;
         break;

      case MM_PARM_MUTE:
         if (pP1)
            *pP1 = (int32)pme->m_bMute;
         break;

      case MM_PARM_TEMPO:
         if (pP1)
            *pP1 = (int32)pme->m_wTempo;
         break;

      case MM_PARM_TUNE:
         if (pP1)
            *pP1 = (int32)pme->m_wTune;
         break;

      case MM_PARM_PAN:
         if (pP1)
            *pP1 = (int32)pme->m_wPan;
         break;

      case MM_PARM_POS:
      case MM_PARM_PLAY_REPEAT:
         nRet = EUNSUPPORTED;
         break;
         
      case MM_PARM_TICK_TIME:
         if (pme->m_clsSelf == AEECLSID_MEDIAPCM)         
         {
            if (pP1)
               *pP1 = pme->m_dwTickTimeMS;
         }
         else
            nRet = EUNSUPPORTED;
         break;

      case MM_PARM_RECT:
#if defined(FEATURE_WEBAUDIO)
         if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
             || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
            ) && pme->u.m_pPMD)
         {
            if (pP1)
               MEMCPY(pP1, &pme->u.m_pPMD->m_rectImage, sizeof(AEERect));
            if (pP2)
               MEMCPY(pP2, &pme->u.m_pPMD->m_rectText, sizeof(AEERect));
         }
         else
#endif // defined(FEATURE_WEBAUDIO)
            nRet = EUNSUPPORTED;
         break;

      case MM_PARM_FRAME:
#if defined(FEATURE_WEBAUDIO)
         if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
              || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
             ) && pme->u.m_pPMD)
         {
            //GetFrame
            if (pP1)
            {
               if (pme->u.m_pPMD->m_pCallerFrameBmp)
               {
                  IBITMAP_AddRef(pme->u.m_pPMD->m_pCallerFrameBmp);
                  *pP1 = (int32)pme->u.m_pPMD->m_pCallerFrameBmp;
               }
               else
               {
                  nRet = EFAILED;
                  break;
               }
            }

            //IsFrameCallback
            if (pP2)
              *pP2 = pme->u.m_pPMD->m_FrameCallbackEnabled;

            nRet = SUCCESS;
         }
         else
#endif // defined(FEATURE_WEBAUDIO)
            nRet = EUNSUPPORTED;
         break;

   case MM_PARM_CHANNEL_SHARE:
	  if (pP1)
		 *(boolean *)pP1 = pme->m_bChannelShare;
	  break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   case MM_PARM_NOTES:
	  if (pP1 && CMediaPMDMMLayer_IsMultiSequence(pme))
		 *(uint32 *)pP1 = pme->m_dwNotes;
	  else
		 nRet = EUNSUPPORTED;
	  break;
#endif

#if defined(FEATURE_CMX_AV_SYNC)
      case MM_PARM_AUDIOSYNC:
      {
         if (pP1)
            *(uint32 *)pP1 = pme->m_dwAudioSyncType;
         if (pP2)
            *(uint32 *)pP2 = pme->m_dwAudioSyncData;
      }
      break;
#endif //defined(FEATURE_CMX_AV_SYNC)

      default:
         bProc = FALSE;
         break;
   }

   if (bProc)
      return nRet;

   return CMediaCMX_GetMediaParm(po, nParmID, pP1, pP2);
}

/*==================================================================

==================================================================*/
static int CMediaPMDBg_QueryInterface(IMedia * po, AEECLSID idReq, void ** ppo)
{
   CMediaPMDBg *pme = (CMediaPMDBg*)po;

   if (!ppo)
      return EBADPARM;

   if (!pme->m_pMediaCMX)
      return EBADSTATE;

#if defined (FEATURE_SVG)
#if defined (FEATURE_SVGDOM)
   if (pme->m_clsSelf == AEECLSID_MEDIASVG && (idReq == AEECLSID_SVGDOM || idReq == AEEIID_SVGDOM))
   {
      int result;

      if (!pme->u.m_pPMD || pme->u.m_pPMD->m_pISVGDOM || pme->u.m_pPMD->m_SVGDecodeData.cmx_svg_decode_mode != CMX_SVG_MODE_CLIENT)
         return EFAILED;

      if (NULL == pme->u.m_pPMD->m_curSVGInfo.document)
      {
         pme->u.m_pPMD->m_bQISVGDOM = TRUE;

         IMEDIA_Play(po);

         // wait for MM_STATUS_MEDIA_SPEC
         (void) rex_clr_sigs ( &ui_tcb, AEE_MEDIA_SIG );
         (void) rex_wait( AEE_MEDIA_SIG );

         if (!pme->u.m_pPMD->m_bQISVGDOM)
         {
            return EBADPARM;
         }
      }
      result = OEMSVGDOM_New(pme->m_pIShell, AEECLSID_SVGDOM, po, &pme->u.m_pPMD->m_curSVGInfo, ppo, &pme->u.m_pPMD->m_pfn);
       
      pme->u.m_pPMD->m_pISVGDOM = (void *)(*(uint32 *)ppo);
      return result; 
   }
   else
#endif //defined (FEATURE_SVGDOM)
#endif //defined (FEATURE_SVG)  
   {
      return AEEMedia_QueryInterface(po, idReq, ppo);
   } 
}

/*==================================================================
   Destructor
==================================================================*/
static void CMediaPMDBg_Delete(CMediaPMDBg * pme)
{
   CMediaPMDMMLayer_CancelCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY);
   CMediaPMDMMLayer_CancelCallback(&pme->m_cbCommand, 1);
   CMediaPMDMMLayer_CancelCallback(&pme->m_cbGetTotalTime, 1);

   MM_RELEASEIF(pme->m_pISound);

#if defined(FEATURE_WEBAUDIO)
   if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
       || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
       ) && pme->u.m_pPMD)
   {
      int i;
      CancelCSCallback(pme->u.m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CALLBACK_Cancel(&pme->u.m_pPMD->m_cbJPEGBufAlloc);
      CALLBACK_Cancel(&pme->u.m_pPMD->m_cbJPEGBufFree);
      CancelCSCallback(pme->u.m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbPicRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbTextRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbPMDText, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbCopyBMPBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbCopyPNGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbCopyJPEGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
#if defined(FEATURE_SAF)
      CancelCSCallback(pme->u.m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbSAFLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG)
      CancelCSCallback(pme->u.m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbSVGLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif //defined(FEATURE_SVG)
      CancelCSCallback(pme->u.m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH);      
      CancelCSCallback(pme->u.m_pPMD->m_cbAnimReset, OEMMM_MAX_CB_CONTEXT_SWITCH);

      if(pme->u.m_pPMD->m_pIAnimBitmap) {
         if(pme->u.m_pPMD->m_pIAnimBitmap->cntRGB)
            FREEIF(pme->u.m_pPMD->m_pIAnimBitmap->pRGB);
         IDIB_Release(pme->u.m_pPMD->m_pIAnimBitmap);
         pme->u.m_pPMD->m_pIAnimBitmap = NULL;
      }

      if(pme->u.m_pPMD->m_pIPicBitmap) {
         if(pme->u.m_pPMD->m_pIPicBitmap->cntRGB)
            FREEIF(pme->u.m_pPMD->m_pIPicBitmap->pRGB);
         IDIB_Release(pme->u.m_pPMD->m_pIPicBitmap);
         pme->u.m_pPMD->m_pIPicBitmap = NULL;
      }

      pme->u.m_pPMD->m_pIDispBitmap = NULL;

      MM_RELEASEIF(pme->u.m_pPMD->m_pIDisp);
      
      {
         ContextSwitchCallback cscb;
         cscb.pIMedia = (IMedia*)pme;
         TextRelease((void*)&cscb);
      }

      FreeJPEGBuf(pme);

      pme->u.m_pPMD->m_displayedPic = FALSE;
      pme->u.m_pPMD->m_displayedSaf = FALSE;
      pme->u.m_pPMD->m_transparency = FALSE;
      
      for (i=0; i<pme->u.m_pPMD->m_numClientBufs; ++i)
      {
         int nCopy;
         FREEIF(pme->u.m_pPMD->m_pClientBufInfos[i].m_buf);
         for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
            FREEIF(pme->u.m_pPMD->m_pClientBufInfos[i].m_copies[nCopy]);
      }
      FREEIF(pme->u.m_pPMD->m_pClientBufInfos);

      MM_RELEASEIF(pme->u.m_pPMD->m_pCallerFrameBmp);
   }

   FREEIF(pme->u.m_pPMD); // Note: This is a union member!
#endif // defined(FEATURE_WEBAUDIO)

   pme->m_bStreaming = FALSE;

   if (pme->m_pqCMXDataReq)
   {
      CQueue_Delete(pme->m_pqCMXDataReq);
      pme->m_pqCMXDataReq = NULL;
   }

   if (pme->m_pcbCMXDataReq)
   {
      CALLBACK_Cancel(pme->m_pcbCMXDataReq);
      FREE(pme->m_pcbCMXDataReq);
      pme->m_pcbCMXDataReq = NULL;
   }

   if (pme->m_pqMediaData)
   {
      CQueue_Delete(pme->m_pqMediaData);
      pme->m_pqMediaData = NULL;
   }

   if (pme->m_pcbMediaData)
   {
      CALLBACK_Cancel(pme->m_pcbMediaData);
      FREE(pme->m_pcbMediaData);
      pme->m_pcbMediaData = NULL;
   }

   FREEIF(pme->m_pSrcBuffer);

#if defined(FEATURE_CMX_AV_SYNC)
   FREEIF(pme->m_pAudioSyncInfo);
#endif

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   if (pme->m_bISynthQualityRef)
   {
      gdwNotes -= pme->m_dwNotes;
      if (gpISynthQuality)
      {
         int32 nRef = ISYNTHQUALITY_Release(gpISynthQuality);
         if (!nRef)
         {
            gpISynthQuality = NULL;
            gdwNotes = OEMMM_MAX_NOTES;
            gbNotesSet = FALSE;
         }
      } // if (gpISynthQuality) 
      pme->m_bISynthQualityRef = FALSE;
   }
#endif

}


/*==================================================================

==================================================================*/
static int CMediaPMDBg_Play(IMedia * po)
{
   CMediaPMDBg *  pme = (CMediaPMDBg *)po;
   int         nRet;

   nRet = AEEMedia_Play(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaPMDMMLayer_Play(pme);;
}

/*==================================================================
   
==================================================================*/
static int CMediaPMDBg_Record(IMedia * po)
{
   return EUNSUPPORTED;
}

#if defined(FEATURE_SVG)
#if defined(FEATURE_SVGDOM)
/*==================================================================

==================================================================*/
void OEMSVGDOM_Free ( IMedia * po )
{
   CMediaBg * pme = (CMediaBg *)po;

   if (pme->u.m_pPMD && pme->u.m_pPMD->m_pISVGDOM)
   {
      pme->u.m_pPMD->m_pfn = NULL;
      pme->u.m_pPMD->m_pISVGDOM = NULL;
      (void)IMEDIA_Stop( po );
   }
}
#endif // defined(FEATURE_SVGDOM)
#endif // defined(FEATURE_SVG)

/*==================================================================

==================================================================*/
static int CMediaPMDBg_Stop(IMedia * po)
{
   CMediaPMDBg * pme = (CMediaPMDBg *)po;
   int            nRet;

#if defined(FEATURE_SVG)
#if defined(FEATURE_SVGDOM)
   // don't allow stop on active DOM object
   if (pme->m_clsSelf == AEECLSID_MEDIASVG && pme->u.m_pPMD && pme->u.m_pPMD->m_pISVGDOM)
   {
      return EFAILED;
   }
#endif // defined(FEATURE_SVGDOM)
#endif // defined(FEATURE_SVG)

   nRet = AEEMedia_Stop(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaPMDMMLayer_Stop(pme);
}


/*==================================================================

==================================================================*/
static int CMediaPMDBg_Seek(IMedia * po, AEEMediaSeek eSeek, int32 lTimeMS)
{
   CMediaPMDBg * pme = (CMediaPMDBg *)po;
   int            nRet;

   nRet = AEEMedia_Seek(po, eSeek, lTimeMS);
   if (nRet != SUCCESS)
      return nRet;

   if (AEEMedia_IsSeekFrame(eSeek))
      return EUNSUPPORTED;

   return CMediaPMDMMLayer_Seek(pme, eSeek, lTimeMS);
}


/*==================================================================

==================================================================*/
static int CMediaPMDBg_Pause(IMedia * po)
{
   CMediaPMDBg * pme = (CMediaPMDBg *)po;
   int            nRet;

   nRet = AEEMedia_Pause(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaPMDMMLayer_Pause(pme);
}


/*==================================================================

==================================================================*/
static int CMediaPMDBg_Resume(IMedia * po)
{
   CMediaPMDBg * pme = (CMediaPMDBg *)po;
   int            nRet;

   nRet = AEEMedia_Resume(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaPMDMMLayer_Resume(pme);
}

/*==================================================================

==================================================================*/
static int CMediaPMDBg_GetTotalTime(IMedia * po)
{
   CMediaPMDBg * pme = (CMediaPMDBg *)po;
   int            nRet;

   nRet = AEEMedia_GetTotalTime(po);
   if (nRet != SUCCESS)
      return nRet;

   return CMediaPMDMMLayer_GetTotalTime(pme);
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   ISHELL_Resume()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context
==================================================================*/
static void CMediaPMDBg_CallbackNotify(void * pUser)
{
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pUser;
   CMediaPMDBg *           pme;

   if (!pcb)
      return;

   pme = (CMediaPMDBg *)pcb->cmdNotify.pIMedia;
   if (!pme)
      return;

   if (pcb->cmdNotify.nStatus == MM_STATUS_MEDIA_SPEC)
   {
#if defined(FEATURE_SVG)
      if (pme->m_clsSelf == AEECLSID_MEDIASVG && pme->u.m_pPMD)
      {
#if defined(FEATURE_SVGDOM)
         if (pme->u.m_pPMD->m_bQISVGDOM)
         {
            CMediaMMLayer_FreeCallback(pcb);
            rex_set_sigs(&ui_tcb, AEE_MEDIA_SIG);
            return;
         }
         else
#endif // defined(FEATURE_SVGDOM)
         pcb->cmdNotify.pCmdData = (void *)&pme->u.m_pPMD->m_SVGspec;
         pcb->cmdNotify.dwSize = sizeof(AEEMediaSVGSpec);
      }
#endif //defined(FEATURE_SVG)      
   }
#if defined(FEATURE_SVG)
#if defined(FEATURE_SVGDOM)
   else if (pcb->cmdNotify.nStatus == MM_STATUS_ABORT && pme->u.m_pPMD->m_bQISVGDOM)
   {
      pme->u.m_pPMD->m_bQISVGDOM = FALSE;
      CMediaMMLayer_FreeCallback(pcb);
      rex_set_sigs(&ui_tcb, AEE_MEDIA_SIG);
      return;
   }
#endif // defined(FEATURE_SVGDOM)
#endif // defined(FEATURE_SVG)
   else if (pcb->cmdNotify.nStatus == MM_STATUS_DONE || pcb->cmdNotify.nStatus == MM_STATUS_ABORT)
   {
      if (OEMMM_ISSTREAMING(pme))
         CMediaPMDMMLayer_FlushStreamData(pme);
   }

   CMediaCMX_CallbackNotify(pcb);
}

/*==================================================================
   Destructor
==================================================================*/
static void CMediaCMX_Delete(CMediaCMX * pme)
{
   if (pme->m_pfnDelete) {
      pme->m_pfnDelete(pme);
   }

   AEEObjectMgr_Unregister(pme->m_hObject);

   // Call base class destructor
   AEEMedia_Delete((IMedia *)pme);

   OEMMediaPMD_Delete(pme->m_pMediaCMX);

   FREE(pme);
}

/*==================================================================

   Callback Notification Functions: called in BREW context after
   AEE_ResumeCallback()

==================================================================*/
/*==================================================================
   Media callback notify -- called in BREW context.
==================================================================*/
void CMediaCMX_CallbackNotify(void * pUser)
{
   AEEMediaCallback *   pcb = (AEEMediaCallback *)pUser;
   CMediaCMX *          pme;
   AEEMediaCallback     mcb;

   if (!pcb)
      return;

   MEMCPY(&mcb, pcb, sizeof(AEEMediaCallback));
   CMediaPMDMMLayer_FreeCallback(pcb);

   pme = (CMediaCMX *)mcb.cmdNotify.pIMedia;
   if (!pme)
      return;

#if defined(FEATURE_SVGDOM)
   if (MM_CMD_SETMEDIAPARM == mcb.cmdNotify.nCmd && OEMCMX_ID_SVG_DOM == mcb.cmdNotify.nSubCmd )
   {
      ((CMediaBg *)pme)->u.m_pPMD->m_pfn (((CMediaBg *)pme)->u.m_pPMD->m_pISVGDOM, &mcb.cmdNotify);
   }
   else
#endif // defined(FEATURE_SVGDOM)
   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
}


/*==================================================================

==================================================================*/
static int CMediaCMX_GetMediaParm(IMedia * po, int nParmID, int32 * pP1, int32 * pP2)
{
   CMediaCMX * pme = (CMediaCMX *)po;
   int         nRet = SUCCESS;

   if (nParmID == MM_PARM_MEDIA_DATA || nParmID == MM_PARM_CLSID)
      return AEEMedia_GetMediaParm(po, nParmID, pP1, pP2);

   switch (nParmID)
   {
      case MM_PARM_CAPS:
         if (pP1)
            *pP1 = (int32)pme->m_dwCaps;
         if (pP2)
            *pP2 = 0;
         break;

      case MM_PARM_ENABLE:
         if (pP1)
            *pP1 = (int32)pme->m_dwEnabledCaps;
         if (pP2)
            *pP2 = 0;
         break;

      case MM_PARM_CHANNEL_SHARE:
         if (pP1)
            *(boolean *)pP1 = FALSE;
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}
   
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) \
	|| defined(FEATURE_SVG) || defined(FEATURE_AUDIO_POSTCARD)
/*==================================================================

==================================================================*/
static void CMXImage_Notify(void * pUser)
{
   IBitmap *pDispBmp = NULL;
   AEERect rcDest;
   int nRectToDestOffsetX, nRectToDestOffsetY;

   ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
   CMediaPMDBg *   pme;

   if (!pcscb)
      return;

   pme = (CMediaPMDBg *)(pcscb->pIMedia);

   UnlockCSCallback(pcscb);

   if (!pme || !pme->u.m_pPMD || !pme->u.m_pPMD->m_pIDisp)
      return;

   if (pme->u.m_pPMD->m_FrameCallbackEnabled)
   {
      if (IDISPLAY_GetDeviceBitmap(pme->u.m_pPMD->m_pIDisp, &pDispBmp) != SUCCESS)
         return;

      if (pme->u.m_pPMD->m_DispAreaChanged)
         MM_RELEASEIF(pme->u.m_pPMD->m_pCallerFrameBmp);

      if (!pme->u.m_pPMD->m_pCallerFrameBmp)
      {
         IBITMAP_CreateCompatibleBitmap(pDispBmp, &pme->u.m_pPMD->m_pCallerFrameBmp, pme->u.m_pPMD->m_rectImage.dx, pme->u.m_pPMD->m_rectImage.dy);
      }

      IDISPLAY_SetDestination(pme->u.m_pPMD->m_pIDisp, pme->u.m_pPMD->m_pCallerFrameBmp);
 
      // The frame bitmap is the size of the screen's destination rect
      // and we have to re-offset all operations accordingly
      nRectToDestOffsetX = pme->u.m_pPMD->m_rectImage.x;
      nRectToDestOffsetY = pme->u.m_pPMD->m_rectImage.y;
      // Rectangle corresponding to the frame bitmap to return to caller
      SETAEERECT(&rcDest, 0, 0, pme->u.m_pPMD->m_rectImage.dx, pme->u.m_pPMD->m_rectImage.dy);
   }
   else
   {
      // Set further offsets to nothing since destination rect is correct for drawing to screen
      nRectToDestOffsetX = 0;
      nRectToDestOffsetY = 0;
      // Rectangle corresponding to the screen's destination rect
      MEMCPY(&rcDest, &pme->u.m_pPMD->m_rectImage, sizeof(AEERect));
   }

   if (pme->u.m_pPMD->m_pIDisp &&
       (pme->u.m_pPMD->m_fAnimDraw & OEMMM_DISP_CLEAR ||
       pme->u.m_pPMD->m_fPicDraw & OEMMM_DISP_CLEAR ||
       pme->u.m_pPMD->m_fTextDraw & OEMMM_DISP_CLEAR))
   {
      IDISPLAY_EraseRect(pme->u.m_pPMD->m_pIDisp, &rcDest);
      if (pme->u.m_pPMD->m_pIDispBitmap == pme->u.m_pPMD->m_pIPicBitmap)
         pme->u.m_pPMD->m_fPicDraw &= ~OEMMM_DISP_CLEAR;
      if (pme->u.m_pPMD->m_pIDispBitmap == pme->u.m_pPMD->m_pIAnimBitmap)
         pme->u.m_pPMD->m_fAnimDraw &= ~OEMMM_DISP_CLEAR;
      pme->u.m_pPMD->m_fTextDraw &= ~OEMMM_DISP_CLEAR;
   }

   // Picture
   if (pme->u.m_pPMD->m_pIDispBitmap)
   {
      AEERect *pRect = &(pme->u.m_pPMD->m_rectImage);
         uint16 width, height;

      if(pme->u.m_pPMD->m_pIDispBitmap == pme->u.m_pPMD->m_pIPicBitmap ||
         pme->u.m_pPMD->m_pIDispBitmap == pme->u.m_pPMD->m_pIAnimBitmap)
          
      {
         if (pme->u.m_pPMD->m_fPicDraw & OEMMM_DISP_DRAW)
         {

            // Copy the picture to the display if one has ever been displayed.
            if (pme->u.m_pPMD->m_displayedPic == TRUE) {
                if (pme->u.m_pPMD->m_picDispRect.x + pme->u.m_pPMD->m_picDispRect.dx
                    > pRect->x + pRect->dx)
                   width = pRect->x + pRect->dx - pme->u.m_pPMD->m_picDispRect.x;
                else
                   width = pme->u.m_pPMD->m_picDispRect.dx;

                if (pme->u.m_pPMD->m_picDispRect.y + pme->u.m_pPMD->m_picDispRect.dy 
                    > pRect->y + pRect->dy)
                   height = pRect->y + pRect->dy - pme->u.m_pPMD->m_picDispRect.y;
                else
                   height = pme->u.m_pPMD->m_picDispRect.dy;

                if (pme->u.m_pPMD->m_pIPicBitmap)
                   IDISPLAY_BitBlt(pme->u.m_pPMD->m_pIDisp, 
                      pme->u.m_pPMD->m_picDispRect.x - nRectToDestOffsetX, 
                      pme->u.m_pPMD->m_picDispRect.y - nRectToDestOffsetY, 
                      width, 
                      height, 
                      (const void *) pme->u.m_pPMD->m_pIPicBitmap, 
                      pme->u.m_pPMD->m_picOffsetRect.x, 
                      pme->u.m_pPMD->m_picOffsetRect.y, 
                      AEE_RO_COPY);
                      //AEE_RO_TRANSPARENT);
            }
         }
      
         // Animation frame
         if (pme->u.m_pPMD->m_fAnimDraw & OEMMM_DISP_DRAW)
         {
 
            if (pme->u.m_pPMD->m_displayedSaf == TRUE) {
                if (pme->u.m_pPMD->m_animDispRect.x + pme->u.m_pPMD->m_animDispRect.dx
                    > pRect->x + pRect->dx)
                   width = pRect->x + pRect->dx - pme->u.m_pPMD->m_animDispRect.x;
                else
                   width = pme->u.m_pPMD->m_animDispRect.dx;

                if (pme->u.m_pPMD->m_animDispRect.y + pme->u.m_pPMD->m_animDispRect.dy 
                    > pRect->y + pRect->dy)
                   height = pRect->y + pRect->dy - pme->u.m_pPMD->m_animDispRect.y;
                else
                   height = pme->u.m_pPMD->m_animDispRect.dy;

                if (pme->u.m_pPMD->m_pIAnimBitmap) {

                   IDISPLAY_BitBlt(pme->u.m_pPMD->m_pIDisp, 
                           pme->u.m_pPMD->m_animDispRect.x - nRectToDestOffsetX, 
                           pme->u.m_pPMD->m_animDispRect.y - nRectToDestOffsetY, 
                           width, 
                           height, 
                           (const void *) pme->u.m_pPMD->m_pIAnimBitmap, 
                           pme->u.m_pPMD->m_animOffsetRect.x, 
                           pme->u.m_pPMD->m_animOffsetRect.y, 
                           pme->u.m_pPMD->m_transparency ? AEE_RO_TRANSPARENT : AEE_RO_COPY);
                           
#if defined(FEATURE_SVG)
                           // Update time for frames
                           pme->u.m_pPMD->m_nUpTimeMS = GETUPTIMEMS();

                           if (pme->u.m_pPMD->m_bNoFramesShown)
                              pme->u.m_pPMD->m_bNoFramesShown = FALSE;
#endif //defined(FEATURE_SVG)

                }
            }
         }
      }
   }

   // Text
   if (pme->u.m_pPMD->m_textBlock)
   {
      // Process and display PMD text
      CMediaPMD * pPmd = (CMediaPMD *)pme->u.m_pPMD;

      if(pme->u.m_pPMD->m_fTextDraw & OEMMM_DISP_DRAW)
      {
         uint16 numToWipe = pPmd->m_numToWipe;
         TextBlock * pBlock = pPmd->m_textBlock;

         while (pBlock)
         {
            RGBVAL oldColor;
            oldColor = IDISPLAY_SetColor(pPmd->m_pIDisp, CLR_USER_TEXT, pBlock->m_color);
            IDISPLAY_DrawText(pPmd->m_pIDisp, AEE_FONT_LARGE,
               pBlock->m_text, pBlock->m_length,
               pBlock->m_x - nRectToDestOffsetX, pBlock->m_y - nRectToDestOffsetY,
               &rcDest, IDF_TEXT_TRANSPARENT | pBlock->m_style);
            IDISPLAY_SetColor(pPmd->m_pIDisp, CLR_USER_TEXT, oldColor);

            if (numToWipe > 0)
            {
               RGBVAL lastColor;
               uint16 wipeNum;
               lastColor = IDISPLAY_SetColor(pPmd->m_pIDisp, CLR_USER_TEXT, pBlock->m_wipeColor);
               if (pBlock->m_length < numToWipe)
                  wipeNum = pBlock->m_length;
               else
                  wipeNum = numToWipe;
               IDISPLAY_DrawText(pPmd->m_pIDisp, AEE_FONT_LARGE,
                  pBlock->m_text, wipeNum,
                  pBlock->m_x - nRectToDestOffsetX, pBlock->m_y - nRectToDestOffsetY,
                  &rcDest, IDF_TEXT_TRANSPARENT | pBlock->m_style);

               // If this is Ruby also wipe the bottom part
               if (!pBlock->m_extent && pBlock->m_next)
               {
                  uint16 rubyWipeNum;
                  pBlock = pBlock->m_next;
                  IDISPLAY_SetColor(pPmd->m_pIDisp, CLR_USER_TEXT, pBlock->m_color);
                  IDISPLAY_DrawText(pPmd->m_pIDisp, AEE_FONT_LARGE,
                     pBlock->m_text, pBlock->m_length,
                     pBlock->m_x - nRectToDestOffsetX, pBlock->m_y - nRectToDestOffsetY,
                     &rcDest, IDF_TEXT_TRANSPARENT | pBlock->m_style);
                  IDISPLAY_SetColor(pPmd->m_pIDisp, CLR_USER_TEXT, pBlock->m_wipeColor);
                  if (pBlock->m_length < numToWipe)
                     rubyWipeNum = pBlock->m_length;
                  else
                     rubyWipeNum = numToWipe;
                  IDISPLAY_DrawText(pPmd->m_pIDisp, AEE_FONT_LARGE,
                     pBlock->m_text, rubyWipeNum,
                     pBlock->m_x - nRectToDestOffsetX, pBlock->m_y - nRectToDestOffsetY,
                     &rcDest, IDF_TEXT_TRANSPARENT | pBlock->m_style);
                  if (rubyWipeNum > wipeNum)
                     wipeNum = rubyWipeNum;
               }
               
               lastColor = IDISPLAY_SetColor(pPmd->m_pIDisp, CLR_USER_TEXT, lastColor);
               numToWipe -= wipeNum;
            }
            pBlock = pBlock->m_next;
         }
      }
   }

   if (pme->u.m_pPMD->m_FrameCallbackEnabled)
   {
      //((AEEMediaCallback *)(pme->m_pUser))->cmdNotify.nStatus = MM_STATUS_FRAME;
      //CMediaCMX_CallbackNotify(pme->m_pUser);
      //// Set the display bitmap back to the device bitmap
      IDISPLAY_SetDestination(pme->u.m_pPMD->m_pIDisp, pDispBmp);
   }
   else
   {
      IDISPLAY_Update(pme->u.m_pPMD->m_pIDisp);
   }

   //PMDPlayerDisplay(gsBuffer, gsdwSize, pRect->x, pRect->y, pRect->dx, pRect->dy);
}
#endif // defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) 
	   // || defined(FEATURE_SVG) || defined(FEATURE_AUDIO_POSTCARD)


/*==================================================================
   Called only once during AEE initialization.
==================================================================*/
static void CMediaPMDMMLayer_Init(IShell * ps)
{
   if (gInitFlags.gbCMXInit)
      return;

   gInitFlags.gbCMXInit = TRUE;

   CALLBACK_Init(&gcbAEEExit, CMediaPMDMMLayer_AEEExitNotify, NULL);
   ISHELL_OnExit(ps, &gcbAEEExit);
}

/*==================================================================
   Called only once during AEE Exit
==================================================================*/
static void CMediaPMDMMLayer_AEEExitNotify(void * pUser)
{
   gInitFlags.gbCMXInit = FALSE;
   gInitFlags.gbMediaBgInit = FALSE;
   gInitFlags.gbMediaMIDIOutQCPInit = FALSE;
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_StatusCB(cmx_status_type status, const void * pClientData)
{
   CMediaPMDBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int   nStatus = MM_STATUS_UNKNOWN;

   pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   pcb = &pme->m_cbCommand;
   if (!pcb->bInUse)
      goto OEMMM_CBExit;

   nStatus = CMediaPMDMMLayer_GetMMStatus((CMediaCMX *)pme, status, NULL, NULL);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   // Check if it is playctl cmd event. If it is and if status is MM_STATUS_ABORT,
   // the status must be mapped to one of SEEK_FAIL/PAUSE_FAIL/RESUME_FAIL
   // bases on nSubCmd.
   if (nStatus == MM_STATUS_ABORT && CMediaPMDMMLayer_ISPLAYCTL(pcb))
   {
      int   nSubCmd = pcb->cmdNotify.nSubCmd;
      pcb->cmdNotify.nSubCmd = 0;   // Do not send this subcmd to user.

      if (nSubCmd == OEMMM_CTLCMD_SEEK)
         nStatus = MM_STATUS_SEEK_FAIL;
      else if (nSubCmd == OEMMM_CTLCMD_PAUSE)
         nStatus = MM_STATUS_PAUSE_FAIL;
      else if (nSubCmd == OEMMM_CTLCMD_RESUME)
         nStatus = MM_STATUS_RESUME_FAIL;
   }

   pcb->cmdNotify.nStatus = nStatus;

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}


#if defined(FEATURE_SVG)
static void CMediaPMDMMLayer_StatusCB2(cmx_status_type status, const void * pClientData)
{
   CMediaPMDBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int   nStatus = MM_STATUS_UNKNOWN;

   pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   pcb = &pme->m_cbCommand;
   if (!pcb->bInUse)
      goto OEMMM_CBExit;

   nStatus = CMediaPMDMMLayer_GetMMStatus2((CMediaCMX *)pme, status, NULL, 0, NULL);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   // Check if it is playctl cmd event. If it is and if status is MM_STATUS_ABORT,
   // the status must be mapped to one of SEEK_FAIL/PAUSE_FAIL/RESUME_FAIL
   // bases on nSubCmd.
   if (nStatus == MM_STATUS_ABORT && CMediaPMDMMLayer_ISPLAYCTL(pcb))
   {
      int   nSubCmd = pcb->cmdNotify.nSubCmd;
      pcb->cmdNotify.nSubCmd = 0;   // Do not send this subcmd to user.

      if (nSubCmd == OEMMM_CTLCMD_SEEK)
         nStatus = MM_STATUS_SEEK_FAIL;
      else if (nSubCmd == OEMMM_CTLCMD_PAUSE)
         nStatus = MM_STATUS_PAUSE_FAIL;
      else if (nSubCmd == OEMMM_CTLCMD_RESUME)
         nStatus = MM_STATUS_RESUME_FAIL;
   }

   pcb->cmdNotify.nStatus = nStatus;

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}
#endif //defined(FEATURE_SVG)


/*==================================================================

==================================================================*/
static int CMediaCMX_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaCMX * pme = (CMediaCMX *)po;
   int         nRet = SUCCESS;


   if (nParmID == MM_PARM_MEDIA_DATA)
      return AEEMedia_SetMediaParm(po, nParmID, p1, p2);

   // No need to alloc a command callback for ISound operations since the callback
   // from ISound uniquely identifies this operation and also
   // gets called in BREW context.
   // These are MM_PARM_VOLUME, MM_PARM_PATH, MM_PARM_MUTE.

   switch (nParmID)
   {
      case MM_PARM_ENABLE:
         if (p1)
         {
            uint32   dw = pme->m_dwCaps & p1;
            if (dw)
               pme->m_dwEnabledCaps = dw;
            else
               nRet = EBADPARM;
         }
         break;

      default:
         nRet = EUNSUPPORTED;
         break;
   }

   return nRet;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_Play(CMediaPMDBg * pme)
{
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   CMXInfo   ci;

#if defined(FEATURE_SVGDOM)
   boolean              bEmptySVGDOC = FALSE;

   // This is only true for AEECLSID_MEDIASVG.
   if (pme->m_clsSelf == AEECLSID_MEDIASVG && 
       pme->u.m_pPMD && 
       pme->u.m_pPMD->m_SVGDecodeData.empty)
   {
      bEmptySVGDOC = TRUE;
   }

   if (!bEmptySVGDOC)
   {
#endif //defined(FEATURE_SVGDOM)

      nRet = CMediaPMDMMLayer_SetSource((CMediaCMX *)pme, &h);
      if (nRet != SUCCESS)
         return nRet;

#if defined(FEATURE_SVGDOM)
   } 
   else
   {
     h.source_type = CMX_SOURCE_INVALID;
   }
#endif //defined(FEATURE_SVGDOM)

   pcb = CMediaPMDMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);
   if (!pcb)
      return EITEMBUSY;

   // If it is raw data, then select codec and start the playback...
   if (pme->m_bRaw)
      return CMediaPMDMMLayer_PlayCodec(pme, &h, pcb);

   if (pme->m_nAPath != MM_APATH_LOCAL_RINGER)
   {

	   pcb->cmdNotify.nStatus = MM_STATUS_START;

	   AEE_ResumeCallback(&pcb->cb, pme->m_pac);

#ifdef FEATURE_MULTISEQUENCER
      /* See if we need to play a multisequence */
      if (CMediaPMDMMLayer_IsMultiSequence(pme))
      {
         CMXInfo              ci;
         cmx_audfmt_multi_param_type sParam;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
         CMediaPMDMMLayer_SetSynthQuality(pme);
#endif

         sParam.repeat_state = CMX_AUDFMT_REPEAT_DISABLED;
         sParam.repeat_count = 0;
         sParam.repeat_ms = 0;
         sParam.cb_type = CMX_AF_CALLBACK_VALUE;

         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY_MULTI;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaPMDMMLayer_PlayCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&sParam, 0, TRUE);
         OEMMediaPMD_Start(pme->m_pMediaCMX);
      }
      else
#endif  // FEATURE_MULTISEQUENCER
	  {
#if defined(FEATURE_SVG)
         if (pme->m_clsSelf == AEECLSID_MEDIASVG)
         {
            // Set the correct status callback
            ci.nType = OEMCMX_TYPE_SVG_DECODE;
            ci.ph = NULL;
            ci.dwCB = (uint32)CMediaPMDMMLayer_StatusCB2;
            ci.dwUserData = (uint32)pme->m_hObject;
            OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_STATUS_CB, (int32)&ci, 0, 0, TRUE);
            
            if (!pme->u.m_pPMD)
               return ENOMEMORY;
            
            // Fill in SVG file directory info
            if (pme->u.m_pPMD->m_SVGUserPath[0] == 0)
            {
               // Use same path as media file since not specified otherwise
               if (pme->m_md.clsData == MMD_FILE_NAME)
               {
                  char * pEnd;
                  STRLCPY((char*)pme->u.m_pPMD->m_SVGDecodeData.directory, h.efs.filename, FS_FILENAME_MAX_LENGTH_P);
                  pEnd = STRRCHR((char*)pme->u.m_pPMD->m_SVGDecodeData.directory, DIRECTORY_CHAR);
                  if (pEnd)
                     *(pEnd + 1) = 0;  // Include trailing slash
               }
            }
            else
            {
               int nStrLen;
               boolean bSuccessful;
               
               bSuccessful = GetSVGDirNativePath(pme, pme->u.m_pPMD->m_SVGUserPath);
               
               //Append trailing slash
               if ((bSuccessful) && (pme->u.m_pPMD->m_SVGDecodeData.directory[0] != 0))
               {
                  nStrLen = STRLEN(pme->u.m_pPMD->m_SVGDecodeData.directory);
                  if (nStrLen < sizeof(pme->u.m_pPMD->m_SVGDecodeData.directory) - 1)
                  {
                     if (pme->u.m_pPMD->m_SVGDecodeData.directory[nStrLen - 1] != DIRECTORY_CHAR)
                     {
                        pme->u.m_pPMD->m_SVGDecodeData.directory[nStrLen] = DIRECTORY_CHAR;
                        pme->u.m_pPMD->m_SVGDecodeData.directory[nStrLen+1] = 0;
                     }
                  }
               }
            }
            
            // Use display rectangle size for frame size
            pme->u.m_pPMD->m_SVGDecodeData.width = pme->u.m_pPMD->m_rectImage.dx;
            pme->u.m_pPMD->m_SVGDecodeData.height = pme->u.m_pPMD->m_rectImage.dy;
            
            
            ci.nType = OEMCMX_TYPE_SVG_DECODE;
            ci.ph = &h;
            ci.dwCB = (uint32)CMediaPMDMMLayer_PlayCB2;
            ci.dwUserData = (uint32)pme->m_hObject;
            OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)&pme->u.m_pPMD->m_SVGDecodeData, 0, TRUE);
            OEMMediaPMD_Start(pme->m_pMediaCMX);    
            pme->u.m_pPMD->m_svgDecoderCalled = TRUE;           
         }
         else
#endif //defined(FEATURE_SVG)
         {
			   cmx_audfmt_start_enum_type eType;
			   if (pme->m_clsSelf != AEECLSID_MEDIAQCP)
			      eType = CMX_AUDFMT_START_NORMAL;
		 	   else
            {
			      if (pme->m_nAPath == MM_APATH_LOCAL)
				     eType = CMX_AUDFMT_START_PATH_FORWARD;
			      else if (pme->m_nAPath == MM_APATH_REMOTE)
				     eType = CMX_AUDFMT_START_PATH_REVERSE;
			      else if (pme->m_nAPath == MM_APATH_BOTH)
				     eType = CMX_AUDFMT_START_PATH_BOTH;
			      else
				     eType = CMX_AUDFMT_START_NORMAL;
            }

		  	   pcb->cmdNotify.nStatus = MM_STATUS_START;
			   AEE_ResumeCallback(&pcb->cb, pme->m_pac);
			   ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
			   ci.ph = &h;
			   ci.dwCB = (uint32)CMediaPMDMMLayer_PlayCB;
			   ci.dwUserData = (uint32)pme->m_hObject;
			   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)eType, 0, TRUE);
			   OEMMediaPMD_Start(pme->m_pMediaCMX);
         }
      }
   }
#ifdef FEATURE_WEBAUDIO
   if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
        || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
        ) && pme->u.m_pPMD)
   {
      int i;
      CancelCSCallback(pme->u.m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CALLBACK_Cancel(&pme->u.m_pPMD->m_cbJPEGBufAlloc);
      CALLBACK_Cancel(&pme->u.m_pPMD->m_cbJPEGBufFree);
      CancelCSCallback(pme->u.m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbPicRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbTextRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbPMDText, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbCopyBMPBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbCopyPNGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbCopyJPEGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);

#if defined(FEATURE_SAF)
      CancelCSCallback(pme->u.m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbSAFLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif  //defined(FEATURE_SAF)

#if defined(FEATURE_SVG)
      CancelCSCallback(pme->u.m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbSVGLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif //defined(FEATURE_SVG)

      CancelCSCallback(pme->u.m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->u.m_pPMD->m_cbAnimReset, OEMMM_MAX_CB_CONTEXT_SWITCH);

      if(pme->u.m_pPMD->m_pIAnimBitmap) {
         if(pme->u.m_pPMD->m_pIAnimBitmap->cntRGB)
            FREEIF(pme->u.m_pPMD->m_pIAnimBitmap->pRGB);
         IDIB_Release(pme->u.m_pPMD->m_pIAnimBitmap);
         pme->u.m_pPMD->m_pIAnimBitmap = NULL;
      }

      pme->u.m_pPMD->m_pIDispBitmap = NULL;

      {
         ContextSwitchCallback cscb;
         cscb.pIMedia = (IMedia*)pme;
         TextRelease((void*)&cscb);
      }

      FreeJPEGBuf(pme);

      for (i=0; i<pme->u.m_pPMD->m_numClientBufs; ++i)
      {
         int nCopy;
         FREEIF(pme->u.m_pPMD->m_pClientBufInfos[i].m_buf);
         pme->u.m_pPMD->m_pClientBufInfos[i].m_size = 0;
         pme->u.m_pPMD->m_pClientBufInfos[i].m_copycount = 0;
         for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
         {
            FREEIF(pme->u.m_pPMD->m_pClientBufInfos[i].m_copies[nCopy]);
         }
      }

      pme->u.m_pPMD->m_displayedPic = FALSE;
      pme->u.m_pPMD->m_displayedSaf = FALSE;
      pme->u.m_pPMD->m_transparency = FALSE;

#if defined(FEATURE_SVG)
      if (pme->m_clsSelf != AEECLSID_MEDIASVG)
      {
         pme->u.m_pPMD->m_svgDecoderCalled = FALSE;
   }
#endif // defined(FEATURE_SVG)

   }
#endif // FEATURE_WEBAUDIO

   return nRet;
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_PlayCB(cmx_status_type status, const void * pClientData, cmx_af_server_data_type * psd)
{
   CMediaPMDBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()
   
   OEMMMStatus          eStatus;
   int                  nStatus = MM_STATUS_UNKNOWN;

   pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   nStatus = CMediaPMDMMLayer_GetMMStatus((CMediaCMX *)pme, status, psd, &eStatus);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   // For playctl command, cbCommand was reserved...free it since
   // SEEK/PAUSE/RESUME/STOP status is sent thru play cb.
   if (eStatus == OEMMM_STATUS_PLAYCTL && CMediaPMDMMLayer_ISPLAYCTL(&pme->m_cbCommand))
   {
      CMediaPMDMMLayer_FreeCallback(&pme->m_cbCommand);
   }

   // For set parm event (tempo, tune, pan), cbCommand is sent in AEE_ResumeCallback().
   // Else for other playback events, one of cbPlay is sent in AEE_ResumeCallback().
   if (eStatus == OEMMM_STATUS_SETPARM) // || eStatus == OEMMM_STATUS_PLAYCTL)
      pcb = &pme->m_cbCommand;
   else
      pcb = CMediaPMDMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);

   if (pcb)
   {
      pcb->cmdNotify.nStatus = nStatus;
      CMediaPMDMMLayer_ProcessPlayServerData((CMediaCMX *)pme, status, psd, pcb);

      bResume = TRUE;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_ProcessPlayServerData(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, AEEMediaCallback * pcb)
{
   if (!pcb || !psd)
      return;

   switch (status)
   {
      case CMX_PAUSE:
      case CMX_REC_PAUSED:
         pcb->cmdNotify.nSubCmd = OEMMM_CTLCMD_PAUSE;
         pcb->cmdNotify.pCmdData = (void *)psd->elapsed_time_ms;
         pcb->cmdNotify.dwSize = sizeof(psd->elapsed_time_ms);
         break;

      case CMX_RESUME:
      case CMX_REC_RESUMED:              
         pcb->cmdNotify.nSubCmd = OEMMM_CTLCMD_RESUME;
         pcb->cmdNotify.pCmdData = (void *)psd->elapsed_time_ms;
         pcb->cmdNotify.dwSize = sizeof(psd->elapsed_time_ms);
         break;

      case CMX_REWIND:
      case CMX_FFORWARD:
         pcb->cmdNotify.nSubCmd = OEMMM_CTLCMD_SEEK;
         pcb->cmdNotify.pCmdData = (void *)psd->elapsed_time_ms;
         pcb->cmdNotify.dwSize = sizeof(psd->elapsed_time_ms);
         break;

      case CMX_TEMPO:
         pme->m_wTempo = (uint16)psd->tempo;
         pcb->cmdNotify.dwSize = sizeof(psd->tempo);
         break;

      case CMX_TUNE:
         pme->m_wTune = (uint16)psd->tune;
         pcb->cmdNotify.dwSize = sizeof(psd->tune);
         break;

      case CMX_PAN:
         pme->m_wPan = (uint16)psd->pan;
         pcb->cmdNotify.dwSize = sizeof(psd->pan);
         break;

      case CMX_AUDIO_SPEC:
         CMediaPMDMMLayer_SetMediaSpec((CMediaPMDBg *)pme, (cmx_af_audio_spec_type *)psd, pcb);
         break;

      default:
         break;
   } // switch
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_SetMediaSpec(CMediaPMDBg * pme, cmx_af_audio_spec_type * psd, AEEMediaCallback * pcb)
{

#if defined(FEATURE_WEBAUDIO)
   if (psd->file_type == CMX_AF_FILE_CMID)
   {
      // Store the character set for translating later
      if(pme->u.m_pPMD){
		pme->u.m_pPMD->m_char_set = psd->wa_spec.char_set;
	  }
   }
#endif

#if defined(FEATURE_BREW_CMX_V3) 

   if (pme->m_bChannelShare)
   {
#ifdef FEATURE_WEBAUDIO
    if (AEECLSID_MEDIAPMD ==  pme->m_clsSelf)
    {
      pme->m_pSequence = psd->wa_spec.sequence;
    }
#endif
   }
#endif // defined(FEATURE_BREW_CMX_V3) 
}


/*==================================================================

==================================================================*/
#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
static int CMediaPMDMMLayer_SetSynthQuality(CMediaPMDBg *pme)
{
   int   nErr;
   int   nCurrentMode;
   int   nNewMode;

   if (!gpISynthQuality)
      return EFAILED;

   // Get the current mode.
   nErr = ISYNTHQUALITY_GetSynthMode(gpISynthQuality, &nCurrentMode);
   if (nErr)
      goto Done;

   // Compute the new mode.
   if (!gbNotesSet || gdwNotes >= OEMMM_MAX_NOTES)
      nNewMode = AEE_SYNTH_MODE_HP_ALL;
   else if (0 == gdwNotes)
      nNewMode = AEE_SYNTH_MODE_HQ_ALL;
   else // gdwNotes is between (both including) 1 and 71
   {
      if (gdwNotes > 56)
         nNewMode = AEE_SYNTH_MODE_HP_ALL;
      else if (gdwNotes > 46)
         nNewMode = AEE_SYNTH_MODE_HP_HI;
      else if (gdwNotes > 38)
         nNewMode = AEE_SYNTH_MODE_HP_HI_MED;
      else if (gdwNotes > 30)
         nNewMode = AEE_SYNTH_MODE_HP_MED;
      else if (gdwNotes > 20)
         nNewMode = AEE_SYNTH_MODE_HQ_MED;
      else if (gdwNotes > 12)
         nNewMode = AEE_SYNTH_MODE_HQ_HI_MED;
      else
         nNewMode = AEE_SYNTH_MODE_HQ_HI;
   }

   // Set new mode
   if (nCurrentMode != nNewMode)
      nErr = ISYNTHQUALITY_SetSynthMode(gpISynthQuality, nNewMode);

Done:
   return nErr;
}
#endif

/*==================================================================

==================================================================*/
#ifdef FEATURE_MULTISEQUENCER
static boolean CMediaPMDMMLayer_IsMultiSequence(CMediaPMDBg *pme)
{
   int i;
   int numClassesSupported = sizeof(multiSequencerClasses)/sizeof(AEECLSID);

   if (pme->m_bChannelShare)
   {
      for (i = 0; i < numClassesSupported; i++)
      {
         if (multiSequencerClasses[i] == pme->m_clsSelf)
         {
            return TRUE;
         }
      }
   }
   return FALSE;
}
#endif  // FEATURE_MULTISEQUENCER 


/*==================================================================

==================================================================*/
void CMediaPMDMMLayer_TimeCB(cmx_status_type status, const void * pClientData, uint32 dwTimeMS)
{
   CMediaPMDBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   int                  nStatus = MM_STATUS_UNKNOWN;
   pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   pcb = &pme->m_cbGetTotalTime;

   nStatus = CMediaPMDMMLayer_GetMMStatus((CMediaCMX *)pme, status, NULL, NULL);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   pcb->cmdNotify.nStatus = nStatus;

   if (nStatus == MM_STATUS_DONE)
   {
      pcb->cmdNotify.pCmdData = (void *)dwTimeMS;
      pcb->cmdNotify.dwSize = sizeof(uint32);
   }

   bResume = TRUE;

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_GetTotalTime(CMediaPMDBg * pme)
{
   cmx_handle_type      h;
   AEEMediaCallback *   pcb;
   int                  nRet = SUCCESS;
   CMXInfo              ci;

   nRet = CMediaPMDMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbGetTotalTime, 1, MM_CMD_GETTOTALTIME, 0);
   if (!pcb)
      return EITEMBUSY;

   ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
   ci.ph = &h;
   ci.dwCB = (uint32)(int32)CMediaPMDMMLayer_TimeCB;
   ci.dwUserData = (uint32)pme->m_hObject;
   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TOTALTIME, (int32)&ci, 0, 0, FALSE);

   return nRet;
}

#if defined(FEATURE_WEBAUDIO)
/*==================================================================
Called in user context allocate a JPEG buffer
==================================================================*/
static void AllocJPEGBuf(void * pUser)
{
   CMediaPMDBg *   pme = (CMediaPMDBg *)pUser;

   if (!pme || !pme->u.m_pPMD)
      return;

   if (pme->u.m_pPMD->m_pJPEGBuf)
      FREE(pme->u.m_pPMD->m_pJPEGBuf);
      
   pme->u.m_pPMD->m_pJPEGBuf = (byte *)MALLOC(pme->u.m_pPMD->m_pJPEGBufSize);

   if(pme->u.m_pPMD->m_cbJPEGBufAllocNotify != NULL)
   {
      pme->u.m_pPMD->m_cbJPEGBufAllocNotify(pme->u.m_pPMD->m_pJPEGBuf,
                                            pme->u.m_pPMD->m_pJPEGClientData);
   }
}
#endif //defined(FEATURE_WEBAUDIO)

/*==================================================================
Called in user context to free a JPEG buffer allocated in user context
==================================================================*/
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)
static void FreeJPEGBuf(void * pUser)
{
   CMediaPMDBg *   pme = (CMediaPMDBg *)pUser;

   if (!pme || !pme->u.m_pPMD)
      return;

   FREEIF(pme->u.m_pPMD->m_pJPEGBuf);
   pme->u.m_pPMD->m_pJPEGBuf = NULL;
   pme->u.m_pPMD->m_pJPEGBufSize = 0;
   pme->u.m_pPMD->m_cbJPEGBufAllocNotify = NULL;
   pme->u.m_pPMD->m_pJPEGClientData = NULL;
}
#endif 

/*==================================================================
Called in user context allocate a buffer.
Once allocated, these buffers will probably exist as long as the
object exists, so there's no need for complicated management.
The size of the list is expected to stay very small (~3).
==================================================================*/
#if defined(FEATURE_WEBAUDIO)
static void AllocClientBuf(void * pUser)
{
	int i, n, nCopy;
	ClientBufInfo * pNew;
	uint16 nMaxCopy;
	ClientBufRequestInfo* pClientBufRequest;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pClientBufRequest = (ClientBufRequestInfo*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	// Check if a buffer in the list is unallocated
	for (i=0; i<pme->u.m_pPMD->m_numClientBufs; ++i)
	{
	   if (!pme->u.m_pPMD->m_pClientBufInfos[i].m_buf)
	   break;
	}

	// If no buffers in the list are free, expand the list
	if (i == pme->u.m_pPMD->m_numClientBufs)
	{
	   pNew = REALLOC(pme->u.m_pPMD->m_pClientBufInfos, sizeof(ClientBufInfo) * (pme->u.m_pPMD->m_numClientBufs + OEMMM_CLIENT_BUF_INIT_COUNT));
	   if (pNew)
	   {
		  for (n = pme->u.m_pPMD->m_numClientBufs; n < pme->u.m_pPMD->m_numClientBufs + OEMMM_CLIENT_BUF_INIT_COUNT; ++n)
		  {
			 // Initialize the fields of the new list items
			 int nCopy;
			 pNew[n].m_buf = NULL;
			 pNew[n].m_size = 0;
			 pNew[n].m_copycount = 0;
			 for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
			 {
				pNew[n].m_copies[nCopy] = NULL;
			 }
		  }

		  pme->u.m_pPMD->m_pClientBufInfos = pNew;
		  pme->u.m_pPMD->m_numClientBufs += OEMMM_CLIENT_BUF_INIT_COUNT;
	   }
	   else
	   {
		  pme->u.m_pPMD->m_lastBufAllocated = -1;
		  return;
	   }
	}

	// Allocate the buffer in the first free slot
	pme->u.m_pPMD->m_pClientBufInfos[i].m_buf = MALLOC(pClientBufRequest->nClientBufSize);
	if (pme->u.m_pPMD->m_pClientBufInfos[i].m_buf)
	{
	   // Restrict the number of copies based on size of buffer request
	   if (pClientBufRequest->nClientBufSize > OEMMM_BUFFER_SIZE_FACTOR)
	   {
		  nMaxCopy = OEMMM_MAX_CB_CONTEXT_SWITCH / (pClientBufRequest->nClientBufSize / OEMMM_BUFFER_SIZE_FACTOR);

		  // We always need at least 1 copy
		  if (nMaxCopy < 1)
		  {
			 nMaxCopy = 1;
		  }
	   }
	   else
	   {
		  nMaxCopy = OEMMM_MAX_CB_CONTEXT_SWITCH;
	   }

	   for (nCopy = 0; nCopy < nMaxCopy; ++nCopy)
	   {
		  pme->u.m_pPMD->m_pClientBufInfos[i].m_copies[nCopy] = MALLOC(pClientBufRequest->nClientBufSize);
		  if (!pme->u.m_pPMD->m_pClientBufInfos[i].m_copies[nCopy])
		  {
			 // Free the main buffer
			 FREEIF(pme->u.m_pPMD->m_pClientBufInfos[i].m_buf);
			 break;
		  }
	   }

	   // If the main buffer has been freed, then fee any copies that may have been allocated
	   if (!pme->u.m_pPMD->m_pClientBufInfos[i].m_buf)
	   {
		  for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
		  {
			 FREEIF(pme->u.m_pPMD->m_pClientBufInfos[i].m_copies[nCopy]);
		  }

		  pme->u.m_pPMD->m_lastBufAllocated = -1;
		  return;
	   }
	   else
	   {
		  pme->u.m_pPMD->m_pClientBufInfos[i].m_size = pClientBufRequest->nClientBufSize;
		  pme->u.m_pPMD->m_pClientBufInfos[i].m_copycount = nMaxCopy;
		  pme->u.m_pPMD->m_lastBufAllocated = i;
	   }
	}

	// Pass the buffer back to cmx
	if(pClientBufRequest->cbClientBufAllocNotify != NULL)
	{
	   pClientBufRequest->cbClientBufAllocNotify(pme->u.m_pPMD->m_pClientBufInfos[i].m_buf,
												 pClientBufRequest->pBufClientData);
	}
}


/*==================================================================

==================================================================*/
static void PICInitialize(void * pUser) {
	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_af_picture_status_type* pPicStatus;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pPicStatus = (cmx_af_picture_status_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD || !pPicStatus)
	   return;

	pme->u.m_pPMD->m_fPicDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag

	//If the OEM has constants to use for display height and width,
	//they can use it here instead of doing the GetDeviceInfo stuff
	{
	   AEEDeviceInfo  info;

	   ISHELL_GetDeviceInfo(pme->m_pIShell, &info);
	   pme->u.m_pPMD->m_cxScreen = info.cxScreen;
	   pme->u.m_pPMD->m_cyScreen = info.cyScreen;

	   if(info.nColorDepth <= 8)
		  pme->u.m_pPMD->m_clrDepth = 8;
	   else
		  pme->u.m_pPMD->m_clrDepth = 16;
	}

	pme->u.m_pPMD->m_fJust = 0;
	FREEIF(pme->u.m_pPMD->m_pPNGBuf);
	pme->u.m_pPMD->m_pPNGBufSize = 0;

	switch(pPicStatus->xpos_type) {
	   case CMX_AF_POS_LT:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_LEFT;
		  break;
	   case CMX_AF_POS_RB:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_RIGHT;
		  break;
	   case CMX_AF_POS_CENTER:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_XCENTR;
		  break;
	   case CMX_AF_POS_PERCENT:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_XPCENT;
		  pme->u.m_pPMD->m_xJustPcent_Pixel = pPicStatus->xpos_percent;
		  break;
	   case CMX_AF_POS_PIXEL:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_XPIXEL;
		  pme->u.m_pPMD->m_xJustPcent_Pixel = pPicStatus->xpos_percent;
		  break;
	}
	switch(pPicStatus->ypos_type) {
	   case CMX_AF_POS_LT:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_TOP;
		  break;
	   case CMX_AF_POS_RB:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_BOT;
		  break;
	   case CMX_AF_POS_CENTER:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_YCENTR;
		  break;
	   case CMX_AF_POS_PERCENT:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_YPCENT;
		  pme->u.m_pPMD->m_yJustPcent_Pixel = pPicStatus->ypos_percent;
		  break;
	   case CMX_AF_POS_PIXEL:
		  pme->u.m_pPMD->m_fJust |= OEMMM_JUST_YPIXEL;
		  pme->u.m_pPMD->m_yJustPcent_Pixel = pPicStatus->ypos_percent;
		  break;
	}

	pme->u.m_pPMD->m_bHeaderRead = FALSE;

}

static void PICRelease(void * pUser) {
	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	pme->u.m_pPMD->m_fPicDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag

	if(pme->u.m_pPMD->m_pIPicBitmap) {
	   if(pme->u.m_pPMD->m_pIPicBitmap->cntRGB)
		  FREEIF(pme->u.m_pPMD->m_pIPicBitmap->pRGB);
	   IDIB_Release(pme->u.m_pPMD->m_pIPicBitmap);
	   pme->u.m_pPMD->m_pIPicBitmap = NULL;
	}
}
   
/*==================================================================

==================================================================*/
static void ProcessPMDText(void * pUser)
{
   CMediaPMD * pmdInfo;
   cmx_af_text_ctl_type * pTextCtl;
   uint16 iTxtlen=0;

   ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
   CMediaPMDBg *   pme;

   if (!pcscb)
      return;

   pme = (CMediaPMDBg *)(pcscb->pIMedia);
   pTextCtl = (cmx_af_text_ctl_type*)(pcscb->pPayload);
   
   UnlockCSCallback(pcscb);

   if (!pme || !pme->u.m_pPMD)
      return;

   pmdInfo = (CMediaPMD *)pme->u.m_pPMD;

   // Copy the text info and schedule a resume. 
   // Pre-allocate static storage area fpr 2-3 text info and callbacks.
   // In the resume callback,
   // (1) Convert PMD text to HTML text
   // (2) Instantiate IHTMLViewer and display the text
   // TBD: Cost of using IHTMLViewer vs parsing and displaying here??
   if (!(pme->m_dwEnabledCaps & MM_CAPS_TEXT))
      return;

    // Check for supported language characterset encoding
   if ( (pme->u.m_pPMD->m_char_set != CMX_AF_CHAR_SET_ANSI)&&
	    (pme->u.m_pPMD->m_char_set !=CMX_AF_CHAR_SET_CHINESE_SIMP) &&
       (pme->u.m_pPMD->m_char_set !=CMX_AF_CHAR_SET_HANGUL) &&
       (pme->u.m_pPMD->m_char_set !=CMX_AF_CHAR_SET_SHIFTJIS) &&
       (pme->u.m_pPMD->m_char_set !=CMX_AF_CHAR_SET_CHINESE_TRAD) &&
       (pme->u.m_pPMD->m_char_set !=CMX_AF_CHAR_SET_HINDI) )
      return;

   // Switch on the different commands
   switch (pTextCtl->text_ctl)
   {
   // New text
   case CMX_AF_TEXT_CTL_NEW:
	   {
		  ContextSwitchCallback cscb;
		  cscb.pIMedia = (IMedia*)pme;
		  TextRelease((void*)&cscb);
	   }
      break;
   // Append text
   case CMX_AF_TEXT_CTL_APPEND:
      pmdInfo->m_fTextDraw |= OEMMM_DISP_CLEAR;
      break;
   // Clear the text
   case CMX_AF_TEXT_CTL_CLEAR:
      {
      TextBlock * pBlock, * pOldBlock;
      pBlock=pmdInfo->m_textBlock;
      while (pBlock)
      {
         FREEIF(pBlock->m_text);
         pOldBlock = pBlock;
         pBlock = pOldBlock->m_next;
         FREEIF(pOldBlock);
      }
      pmdInfo->m_textBlock = NULL;
      pmdInfo->m_fTextDraw |= OEMMM_DISP_CLEAR;
      break;
      }
   // Advance the number of chars to wipe
   case CMX_AF_TEXT_CTL_ADVANCE:
      pmdInfo->m_numToWipe += (uint16)pTextCtl->num;
      break;
   // Rewind the number of chars to wipe
   case CMX_AF_TEXT_CTL_REWIND:
      pmdInfo->m_numToWipe -= (uint16)pTextCtl->num;
      break;
   case CMX_AF_TEXT_CTL_ENABLE:
      pmdInfo->m_fTextDraw = OEMMM_DISP_DRAW;
      break;
   case CMX_AF_TEXT_CTL_DISABLE:
      pmdInfo->m_fTextDraw &= ~OEMMM_DISP_DRAW;
      break;
   }

   if (pTextCtl->text_ctl == CMX_AF_TEXT_CTL_NEW ||
	   pTextCtl->text_ctl == CMX_AF_TEXT_CTL_APPEND)
   {
      uint16 last, curr, end, row, ruby;
      TextBlock * pBlock, * pOldBlock;
      RGBVAL color, wipeColor;
      uint32 style;
      uint16 textXSize, textYSize;
      uint16 fontYSize;
      AEERect * pRect;

      last=curr=0;
      end = (uint16)pTextCtl->num;

      pBlock = pmdInfo->m_textBlock;
      while (pBlock && pBlock->m_next)
         pBlock = pBlock->m_next;

      if (!pBlock)
      {
         color = OEMMM_DEF_TEXT_COLOR;
         wipeColor = OEMMM_DEF_WIPE_COLOR;
         style = 0;
         row = 1;
         ruby=0;
      }
      else
      {
         color = pBlock->m_color;
         wipeColor = pBlock->m_wipeColor;
         style = pBlock->m_style;
         row   = pBlock->m_row;
         if (!pBlock->m_extent)
            ruby = 1;
         else
            ruby = 0;
      }

      while (curr < end)
      {

         while (pTextCtl->buffer[curr] != '<' &&
                pTextCtl->buffer[curr] != '&' &&
                curr < end) ++curr;

         if (curr-last)
         {
            if (pBlock)
            {
               pBlock->m_next = (TextBlock *)MALLOC(sizeof(TextBlock));
               if (!pBlock->m_next)
                  return;
               pBlock = pBlock->m_next;
            }
            else
            {
               pmdInfo->m_textBlock = (TextBlock *)MALLOC(sizeof(TextBlock));
               if (!pmdInfo->m_textBlock)
                  return;
               pBlock = pmdInfo->m_textBlock;
            }
            iTxtlen=curr-last;			
            //cmx-text block to 16 bit UCS-2 encoded characters, compatible to UTF-16 encoding
            if(!ConvertToUnicodebuffer(&pTextCtl->buffer[last],&iTxtlen,&pBlock->m_text,pme->u.m_pPMD->m_char_set))
            	return;
            pBlock->m_length = curr-last;
            pBlock->m_color = color;
            pBlock->m_wipeColor = wipeColor;
            pBlock->m_style = style;
            pBlock->m_row = row;
            pBlock->m_next = NULL;
            pBlock->m_extent = IDISPLAY_MeasureText(pmdInfo->m_pIDisp,
               AEE_FONT_LARGE, pBlock->m_text);
         }

         if (curr >= end)
            break;

         if (pTextCtl->buffer[curr] == '<')
         {
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "<BR>", 4))
               ++row;
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "<P>", 3))
               row+=2;
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "<FONT COLOR=\"#", 14))
            {
               char * end;
               unsigned long newColor;
               curr += 14;
               newColor = STRTOUL((const char *)&pTextCtl->buffer[curr], &end, 16);
               newColor = NTOHL(newColor);
               if (*end == '\"')
               {
                  color = newColor;
               }
            }
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "<WIPE COLOR=\"#", 14))
            {
               char * end;
               unsigned long newColor;
               curr += 14;
               newColor = STRTOUL((const char *)&pTextCtl->buffer[curr], &end, 16);
               newColor = NTOHL(newColor);
               if (*end == '\"')
               {
                  wipeColor = newColor;
               }
            }
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "<R \"", 3))
            {
                  ruby=1;

               while (pTextCtl->buffer[curr] != '\"' &&
                  curr < end) ++curr;
               ++curr;
               last = curr;
               while (pTextCtl->buffer[curr] != '\"' &&
                  curr < end) ++curr;

               if (curr-last)
               {
                  if (pBlock)
                  {
                     pBlock->m_next = (TextBlock *)MALLOC(sizeof(TextBlock));
                     if (!pBlock->m_next)
                        return;
                     pBlock = pBlock->m_next;
                  }
                  else
                  {
                     pmdInfo->m_textBlock = (TextBlock *)MALLOC(sizeof(TextBlock));
                     if (!pmdInfo->m_textBlock)
                        return;
                     pBlock = pmdInfo->m_textBlock;
                  }
				  iTxtlen=curr-last;	
				  //cmx-text block to 16 bit UCS-2 encoded characters, compatible to UTF-16 encoding
				  if(!ConvertToUnicodebuffer(&pTextCtl->buffer[last],&iTxtlen,&pBlock->m_text,pme->u.m_pPMD->m_char_set))
					   return;

                  pBlock->m_length = curr-last;
                  pBlock->m_color = color;
                  pBlock->m_wipeColor = wipeColor;
                  pBlock->m_style = style;
                  pBlock->m_row = row;
                  pBlock->m_next = NULL;
                  pBlock->m_extent = 0;
               }
            }
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "<U>", 3))
               style |= IDF_TEXT_UNDERLINE;
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "</FONT>", 7))
               color = 0;
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "</WIPE>", 7))
               wipeColor = OEMMM_DEF_WIPE_COLOR;
            if (!STRNICMP((const char *)&pTextCtl->buffer[curr], "</U>", 4))
               style &= ~IDF_TEXT_UNDERLINE;

            while (pTextCtl->buffer[curr++] != '>' &&
               curr < end);

            last = curr;
         }
         else if (pTextCtl->buffer[curr] == '&')
         {
            ++curr;
            if (!STRNCMP((const char *)&pTextCtl->buffer[curr], "#60;", 4))
            {
               pTextCtl->buffer[curr+3] = '<';
               curr+=4;
            }
            else if (!STRNCMP((const char *)&pTextCtl->buffer[curr], "#62;", 4))
            {
               pTextCtl->buffer[curr+3] = '>';
               curr+=4;
            }
            else if (!STRNCMP((const char *)&pTextCtl->buffer[curr], "#34;", 4))
            {
               pTextCtl->buffer[curr+3] = '\"';
               curr+=4;
            }
            else if (!STRNCMP((const char *)&pTextCtl->buffer[curr], "#38;", 4))
            {
               pTextCtl->buffer[curr+3] = '&';
               curr+=4;
            }
            last=curr - 1;
         }
      }

      pRect = &pmdInfo->m_rectImage;
      fontYSize = IDISPLAY_GetFontMetrics(pmdInfo->m_pIDisp, AEE_FONT_LARGE, NULL, NULL);
      if (ruby)
         ++row;
      textYSize = row * (fontYSize + 1) - 1;

	  switch (pTextCtl->ypos_type)
      {
      case CMX_AF_POS_LT: textYSize = pRect->y; break;
      case CMX_AF_POS_RB: textYSize = pRect->y + abs(pRect->dy - textYSize); break;
      case CMX_AF_POS_CENTER: textYSize = pRect->y + abs(pRect->dy - textYSize)/2; break;
      default: textYSize = pRect->y + abs(pRect->dy - textYSize)/2; break;
      }

      for (row = 1, pBlock = pmdInfo->m_textBlock; pBlock; ++row)
      {
         pOldBlock = pBlock;
         textXSize = 0;
         while ((pBlock) &&
                (pBlock->m_row == row))
         {
            textXSize += pBlock->m_extent;
            pBlock = pBlock->m_next;
         }

         // Figure out the placement
		 switch (pTextCtl->xpos_type)
         {
         case CMX_AF_POS_LT: textXSize = pRect->x; break;
         case CMX_AF_POS_RB: textXSize = pRect->x + abs(pRect->dx - textXSize); break;
         case CMX_AF_POS_CENTER: textXSize = pRect->x + abs(pRect->dx - textXSize)/2; break;
         default: textXSize = pRect->x + abs(pRect->dx - textXSize)/2; break;
         }

         while (pOldBlock != pBlock)
         {
            pOldBlock->m_x = textXSize;
            if (pOldBlock->m_extent)
               pOldBlock->m_y = textYSize + (pOldBlock->m_row - 1 + ruby)*(fontYSize + 1);
            else
               pOldBlock->m_y = textYSize + (pOldBlock->m_row - 2 + ruby)*(fontYSize + 1);
            textXSize += pOldBlock->m_extent;
            pOldBlock = pOldBlock->m_next;
         }
      }
   }

   CMXImage_Notify(pUser);

   return;
}



/*==================================================================

==================================================================*/
static void BMPCopyBuffer(void * pUser) {

	WinBMPInfoHdr *pBMPHdr;
	IDShift * pIDShift;
	int       i;
	int       height;
	cmx_af_pic_data_type *pPicData;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pPicData = (cmx_af_pic_data_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	//The BMP starts with the BMP Info header.
	pBMPHdr = (WinBMPInfoHdr *) pPicData->buffer;

	if(pBMPHdr->wBitsPerPix != 24)
	   return;

	height = pBMPHdr->dwBIHeight;

	if(height < 0)
	   height = -height;


	if(pme->u.m_pPMD->m_pIPicBitmap) {
	   if(pme->u.m_pPMD->m_pIPicBitmap->cntRGB && pme->u.m_pPMD->m_pIPicBitmap->pRGB)
		  FREE(pme->u.m_pPMD->m_pIPicBitmap->pRGB);
	   IDIB_Release(pme->u.m_pPMD->m_pIPicBitmap);
	}


	   IDISPLAY_CreateDIBitmap(pme->u.m_pPMD->m_pIDisp,
							   (IDIB**)&pme->u.m_pPMD->m_pIPicBitmap,
							   (uint8) pme->u.m_pPMD->m_clrDepth,
							   (uint16) pBMPHdr->dwBIWidth,
							   (uint16) height
							   );

	if(pme->u.m_pPMD->m_pIPicBitmap) {

	   if(pme->u.m_pPMD->m_clrDepth == 16)
		  pme->u.m_pPMD->m_pIPicBitmap->nColorScheme = IDIB_COLORSCHEME_565;
	   else
		  pme->u.m_pPMD->m_pIPicBitmap->nColorScheme = IDIB_COLORSCHEME_332;

	   IDShift_New(pme->m_pIShell, (void**) &pIDShift);

	   if(pIDShift) {
		  AEEDShift dshift;
		  int       rowWidth = (pBMPHdr->dwBIWidth * 3 + 3)&~3;

		  byte     *pSrc = (byte *) pPicData->buffer + sizeof(WinBMPInfoHdr);

		  dshift.bDither = FALSE;
		  dshift.nDstDepth = (uint8) pme->u.m_pPMD->m_clrDepth;
		  dshift.nSrcDepth = 24;
		  dshift.nWidth  = (uint16) pBMPHdr->dwBIWidth;

		  IDShift_Configure(pIDShift, &dshift);

		  if(pBMPHdr->dwBIHeight > 0) {

			 for(i = 0; i < height; i++)

				IDShift_Convert(pIDShift,
								((byte *)pme->u.m_pPMD->m_pIPicBitmap->pBmp) + i* pme->u.m_pPMD->m_pIPicBitmap->nPitch,
								pSrc + (height - i - 1)*rowWidth, TRUE
							   );
		  }
		  else {
			 for(i = 0; i < height; i++)

				IDShift_Convert(pIDShift,
								((byte *)pme->u.m_pPMD->m_pIPicBitmap->pBmp) + i * pme->u.m_pPMD->m_pIPicBitmap->nPitch,
								pSrc + i*rowWidth, TRUE
							   );
		  }

		  pme->u.m_pPMD->m_dispRect.dx = (uint16) pBMPHdr->dwBIWidth;
		  pme->u.m_pPMD->m_dispRect.dy = (uint16) height;


		  IDShift_Delete(pIDShift);

		  pme->u.m_pPMD->m_pIDispBitmap = pme->u.m_pPMD->m_pIPicBitmap;
		  SetImageOffsets(pme);
		  CMXImage_Notify(pUser);
	   }
	}

	return;
}
#endif // defined(FEATURE_WEBAUDIO)

#if defined(FEATURE_SAF)
/*==================================================================

==================================================================*/
static void SAFCreateDIB(void * pUser) {
	int       numColors;
	uint16     width;
	uint16     height;
	uint32    *pPal;
	uint8     *pSAFPal;
	byte      *pTransp;
	uint32    i;
 //   IDIB **   pBmp = pme->u.m_pPMD->m_pIAnimBitmap;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_saf_info_type *pSAFInfo;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pSAFInfo = (cmx_saf_info_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	width  = pSAFInfo->width;
	height = pSAFInfo->height;

	if(pme->u.m_pPMD->m_pIAnimBitmap) {
	   if(pme->u.m_pPMD->m_pIAnimBitmap->cntRGB && pme->u.m_pPMD->m_pIAnimBitmap->pRGB)
		  FREE(pme->u.m_pPMD->m_pIAnimBitmap->pRGB);
	   IDIB_Release(pme->u.m_pPMD->m_pIAnimBitmap);
	}

 /**
	if(pSAFInfo->width < OEMMM_SAF_FRAME_MAXWIDTH)
	   width = (uint8) pSAFInfo->width;
	if(pSAFInfo->height < OEMMM_SAF_FRAME_MAXHEIGHT)
	   height = (uint8) pSAFInfo->height;
 **/
	IDISPLAY_CreateDIBitmap(pme->u.m_pPMD->m_pIDisp,
							(IDIB **)&pme->u.m_pPMD->m_pIAnimBitmap,
							(uint8) pSAFInfo->bitdepth, 
							width,
							height
							);
	i = sizeof(int);
	if(i > 4)
	   return;

	if(pme->u.m_pPMD->m_pIAnimBitmap) {
	   IBitmap *pBitmap = IDIB_TO_IBITMAP (pme->u.m_pPMD->m_pIAnimBitmap);

	   numColors = pSAFInfo->num_palette_entry;
	   pme->u.m_pPMD->m_pIAnimBitmap->nPitch = (int16) (width * pSAFInfo->bitdepth + 7)/8;
	   if(numColors){

		  //Get bit depth
		  int tempNumColors = numColors;
		  for (pme->u.m_pPMD->m_AnimBitDepth=0; tempNumColors>1; ++pme->u.m_pPMD->m_AnimBitDepth)
			 tempNumColors >>= 1;

		  pme->u.m_pPMD->m_pIAnimBitmap->pRGB = MALLOC(numColors * 4);
		  if(pme->u.m_pPMD->m_pIAnimBitmap->pRGB) {
			 pPal = pme->u.m_pPMD->m_pIAnimBitmap->pRGB;
			 pSAFPal = pSAFInfo->palette;
			 for(i = 0; i < pSAFInfo->num_palette_entry; i++) {
				*pPal = (uint32) (*pSAFPal << 16) ;
				pSAFPal++;
				*pPal |= (uint32) (*pSAFPal << 8);
				pSAFPal++;
				*pPal |= (uint32) *pSAFPal;
				pSAFPal++;
				pPal++;
			 }
			 pme->u.m_pPMD->m_pIAnimBitmap->cntRGB = numColors;
			 pTransp = pSAFInfo->global_trans;

			 //for(i = 0; i < pSAFInfo->glob_trans_count; i++) {
			 if (pSAFInfo->glob_trans_count > 0) {

				uint8  transpIdx = *pTransp;

				//We just pick the first transparent color
				if(transpIdx < numColors) {

					// Since the NativeColor of an RGB value is just it's palette index
					// use the palette index.
					IBITMAP_SetTransparencyColor (pBitmap, transpIdx);

					// Set the transparency flag
					pme->u.m_pPMD->m_transparency = TRUE;
				}
			 }
		  }
		  else
			 pme->u.m_pPMD->m_pIAnimBitmap->cntRGB = 0;
	   }
	   else {
		  pme->u.m_pPMD->m_AnimBitDepth = pSAFInfo->bitdepth;
		  switch(pSAFInfo->bitdepth) {
		  case 8:
		  {
			 uint32 i, red, green, blue;
			 pme->u.m_pPMD->m_pIAnimBitmap->pRGB = MALLOC(256 * 4);
			 if(pme->u.m_pPMD->m_pIAnimBitmap->pRGB)
			 {
				pme->u.m_pPMD->m_pIAnimBitmap->cntRGB = 256;
				for (i = 0; i < 256; ++i) {
				   red = (i >> 5) & 0x07;
				   green = (i >> 2) & 0x07;
				   blue = i & 0x03;
				   pme->u.m_pPMD->m_pIAnimBitmap->pRGB[i] = (((blue * 255 / 3))
					  | ((green * 255 / 7) << 8)
					  | (red * 255 / 7) << 16);
				}
			 }
			 break;
		  }
		  case 12:
			 pme->u.m_pPMD->m_pIAnimBitmap->nColorScheme = IDIB_COLORSCHEME_444;
			 break;
		  case 16:
			 pme->u.m_pPMD->m_pIAnimBitmap->nColorScheme = IDIB_COLORSCHEME_565;
			 break;
		  case 24:
			 pme->u.m_pPMD->m_pIAnimBitmap->nColorScheme = IDIB_COLORSCHEME_888;
			 break;
		  }
	   }
	}


	return;
}

/*==================================================================

==================================================================*/
static void SAFLoadFrameToDIB(void * pUser) {
	int nImgBytes;
	IDIB * pIDIB;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_saf_data_type *pSAFData;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pSAFData = (cmx_saf_data_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	pIDIB = pme->u.m_pPMD->m_pIAnimBitmap;
	if(!pIDIB || !pSAFData->buffer)
	   return;

	nImgBytes = pIDIB->cx * pIDIB->cy * (pme->u.m_pPMD->m_AnimBitDepth/8);
	MEMCPY(pIDIB->pBmp, pSAFData->buffer, nImgBytes);

	//if(psd->pic_data.pic_data == CMX_AF_PIC_DATA_DONE)
	pme->u.m_pPMD->m_fAnimDraw |= OEMMM_DISP_CLEAR;
	pme->u.m_pPMD->m_pIDispBitmap = pme->u.m_pPMD->m_pIAnimBitmap;

	MEMCPY(&pme->u.m_pPMD->m_dispRect, &pme->u.m_pPMD->m_rectImage,
		   sizeof(AEERect));
	pme->u.m_pPMD->m_dispRect.dx = pme->u.m_pPMD->m_pIAnimBitmap->cx;
	pme->u.m_pPMD->m_dispRect.dy = pme->u.m_pPMD->m_pIAnimBitmap->cy;

	SetSAFOffsets(pme, pSAFData);

	//Update screen
	CMXImage_Notify(pUser);
}
#endif  //defined(FEATURE_SAF)

#if defined(FEATURE_SVG)
/*==================================================================

==================================================================*/
static void SVGCreateDIB(void * pUser) {
	uint16     width;
	uint16     height;
	uint32    i;
	uint8     nColorScheme;
	int       nResult;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_svg_info_type *pSVGInfo;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pSVGInfo = (cmx_svg_info_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	pme->u.m_pPMD->m_curSVGInfo = *pSVGInfo;

	width  = pSVGInfo->width;
	height = pSVGInfo->height;

	if(pme->u.m_pPMD->m_pIAnimBitmap) {
	   if(pme->u.m_pPMD->m_pIAnimBitmap->cntRGB && pme->u.m_pPMD->m_pIAnimBitmap->pRGB)
		  FREE(pme->u.m_pPMD->m_pIAnimBitmap->pRGB);
	   IDIB_Release(pme->u.m_pPMD->m_pIAnimBitmap);
	}

	switch(pSVGInfo->color_scheme) {
	   case CMX_COLOR_SCHEME_RGB565:
		  pme->u.m_pPMD->m_AnimBitDepth = 16;
		  nColorScheme = IDIB_COLORSCHEME_565;
		  break;
	   case CMX_COLOR_SCHEME_0RGB888:
      case CMX_COLOR_SCHEME_ARGB888:
		  pme->u.m_pPMD->m_AnimBitDepth = 32;
		  //BREW handles 32-bit color schemes using IDIB_COLORSCHEME_888
		  nColorScheme = IDIB_COLORSCHEME_888;
		  break;
	   default:
		  // Unsupported bit depth/color scheme
		  return;
	}

	nResult = IDISPLAY_CreateDIBitmap(pme->u.m_pPMD->m_pIDisp,
									  (IDIB **)&pme->u.m_pPMD->m_pIAnimBitmap,
									  (uint8) pme->u.m_pPMD->m_AnimBitDepth, 
									  width,
									  height
									 );
	i = sizeof(int);
	if(i > 4)
	   return;

	if((nResult == SUCCESS) && (pme->u.m_pPMD->m_pIAnimBitmap)) {
	   pme->u.m_pPMD->m_pIAnimBitmap->nPitch = width * (pme->u.m_pPMD->m_AnimBitDepth / 8);
	   pme->u.m_pPMD->m_pIAnimBitmap->nColorScheme = nColorScheme;

	   if (pSVGInfo->transparency)
	   {
		  // If standalone SVG, use magenta (0xFF00FF in 0BGR) as transparent color,
		  // otherwise if in pmd file use color passed.
		  IBitmap *pBitmap = IDIB_TO_IBITMAP (pme->u.m_pPMD->m_pIAnimBitmap);;
		  pme->u.m_pPMD->m_SVGTransColor = IBITMAP_RGBToNative(pBitmap, pme->u.m_pPMD->m_svgDecoderCalled ? 0xFF00FF : pSVGInfo->transparency);
		  IBITMAP_SetTransparencyColor (pBitmap, pme->u.m_pPMD->m_SVGTransColor);

		  // Set the transparency flag
		  pme->u.m_pPMD->m_transparency = TRUE;
	   }
	}
	// Initialize frames status
	pme->u.m_pPMD->m_bNoFramesShown = TRUE;
	return;
}

/*==================================================================

==================================================================*/
static boolean GetSVGDirNativePath(CMediaPMDBg * pme, void * pData)
{
   const char *cpszFile = (const char *)pData;
   char *      pszFS = 0;
   int         nLen;
   int         bRet = FALSE;

   if (!cpszFile) 
      goto Done;

   // Get resolved fs:/ path length
   if (SUCCESS != AEE_ResolvePath(cpszFile, NULL, &nLen))
      goto Done;

   pszFS = MALLOC(nLen);
   if (!pszFS)
      goto Done;

   // Resolve path to fs:/
   if (SUCCESS != AEE_ResolvePath(cpszFile, pszFS, &nLen))
      goto Done;

   nLen = sizeof(pme->u.m_pPMD->m_SVGDecodeData.directory);

   /* get native path */   
   if (SUCCESS != OEMFS_GetNativePath(pszFS, pme->u.m_pPMD->m_SVGDecodeData.directory, &nLen))
      goto Done;

   bRet = TRUE;

Done:
   FREEIF(pszFS);
   return bRet;
}

/*==================================================================

==================================================================*/
static void SVGLoadFrameToDIB(void * pUser) {
	int nImgBytes;
	IDIB * pIDIB;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_svg_frame_done_type *pFrameData;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pFrameData = (cmx_svg_frame_done_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	pIDIB = pme->u.m_pPMD->m_pIAnimBitmap;
	if(!pIDIB || !pFrameData->frame_buf)
	   return;

	nImgBytes = pIDIB->cx * pIDIB->cy * (pme->u.m_pPMD->m_AnimBitDepth/8);
	MEMCPY(pIDIB->pBmp, pFrameData->frame_buf, nImgBytes);

	if (pme->u.m_pPMD->m_curSVGInfo.transparency)
	{
	   // Copy the transparent color into the bitmap based on the bitmask
	   // 0 means opaque, 1 means transparent
	   uint32 pixel;
	   IBitmap *pBmp = IDIB_TO_IBITMAP(pIDIB);
	   AEEPoint* pPoints = NULL;

	   pPoints = MALLOC(pFrameData->bitmask_bufsize * sizeof(AEEPoint));
	   if (pPoints)
	   {
		  int iPoint = 0;

		  for (pixel = 0; pixel < pFrameData->bitmask_bufsize; pixel++)
		  {
			 if (pFrameData->bitmask_buf[pixel])
			 {
				pPoints[iPoint].x = pixel%pIDIB->cx;
				pPoints[iPoint].y = pixel/pIDIB->cx;
				++iPoint;
			 }
		  }

		  IBITMAP_SetPixels(pBmp, iPoint, pPoints, pme->u.m_pPMD->m_SVGTransColor, AEE_RO_COPY);
		  FREEIF(pPoints);
	   }
	}

	pme->u.m_pPMD->m_fAnimDraw |= OEMMM_DISP_CLEAR;
	pme->u.m_pPMD->m_pIDispBitmap = pme->u.m_pPMD->m_pIAnimBitmap;

	MEMCPY(&pme->u.m_pPMD->m_dispRect, &pme->u.m_pPMD->m_rectImage,
		   sizeof(AEERect));
	pme->u.m_pPMD->m_dispRect.dx = pme->u.m_pPMD->m_pIAnimBitmap->cx;
	pme->u.m_pPMD->m_dispRect.dy = pme->u.m_pPMD->m_pIAnimBitmap->cy;

	SetAnimOffsets(pme);

	//Update screen
	CMXImage_Notify(pUser);

#if (SVGT_VERSION > SVGT_1_1)
   // Let CMX know it can reuse the buffer
   if (pFrameData->frame_done_cb)
   {
      pFrameData->frame_done_cb(pFrameData->frame_done_data);
   }
#endif // (SVGT_VERSION > SVGT_1_1)
}

/*==================================================================

==================================================================*/
static void  SetAnimOffsets(CMediaPMDBg * pme) {

   AEERect *pRect;

   if (!pme->u.m_pPMD)
      return;

   pRect = &(pme->u.m_pPMD->m_rectImage);

   pme->u.m_pPMD->m_dispRect.x = pRect->x;
   pme->u.m_pPMD->m_dispRect.y = pRect->y;

   if (pme->u.m_pPMD->m_dispRect.x < pRect->x)
   {
      pme->u.m_pPMD->m_offsetRect.x = (pRect->x - pme->u.m_pPMD->m_dispRect.x);
      pme->u.m_pPMD->m_dispRect.x = pRect->x;
   }
   else
      pme->u.m_pPMD->m_offsetRect.x = 0;

   if (pme->u.m_pPMD->m_dispRect.y < pRect->y)
   {
      pme->u.m_pPMD->m_offsetRect.y = (pRect->y - pme->u.m_pPMD->m_dispRect.y);
      pme->u.m_pPMD->m_dispRect.y = pRect->y;
   }
   else
      pme->u.m_pPMD->m_offsetRect.y = 0;

   MEMCPY (&pme->u.m_pPMD->m_animDispRect, &pme->u.m_pPMD->m_dispRect, 
           sizeof (AEERect));

   MEMCPY (&pme->u.m_pPMD->m_animOffsetRect, &pme->u.m_pPMD->m_offsetRect, 
           sizeof (AEERect));
   pme->u.m_pPMD->m_displayedSaf = TRUE;
}
#endif //defined(FEATURE_SVG)


/*==================================================================

==================================================================*/
#if defined(FEATURE_SAF)
static void  SetSAFOffsets(CMediaPMDBg * pme, cmx_saf_data_type *pSAFData) {

   AEERect *pRect;

   if (!pme->u.m_pPMD)
      return;

   pRect = &(pme->u.m_pPMD->m_rectImage);

   //Set display horizontal position
   switch(pSAFData->frame_pos.xpos_type) {
      case CMX_AF_POS_LT:
         pme->u.m_pPMD->m_dispRect.x = pRect->x;
         break;
      case CMX_AF_POS_CENTER:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + (pRect->dx - pme->u.m_pPMD->m_dispRect.dx)/2; 
         break;
      case CMX_AF_POS_RB:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + pRect->dx - pme->u.m_pPMD->m_dispRect.dx;
         break;
      case CMX_AF_POS_PIXEL:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + pSAFData->frame_pos.xpos_num;
         break;
      case CMX_AF_POS_PERCENT:
		 pme->u.m_pPMD->m_dispRect.x = pRect->x + ((pRect->dx *  pSAFData->frame_pos.xpos_num)/100);
         break;
      default:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + (pRect->dx - pme->u.m_pPMD->m_dispRect.dx)/2;
   }
   
   //Set display vertical position
   switch(pSAFData->frame_pos.ypos_type) {
      case CMX_AF_POS_LT:
         pme->u.m_pPMD->m_dispRect.y = pRect->y;
         break;
      case CMX_AF_POS_CENTER:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->u.m_pPMD->m_dispRect.dy)/2;
         break;
      case CMX_AF_POS_RB:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + pRect->dy - pme->u.m_pPMD->m_dispRect.dy;
         break;
      case CMX_AF_POS_PIXEL:
		 pme->u.m_pPMD->m_dispRect.y = pRect->y + pSAFData->frame_pos.ypos_num;
         break;
      case CMX_AF_POS_PERCENT:
		 pme->u.m_pPMD->m_dispRect.y = pRect->y + ((pRect->dy *  pSAFData->frame_pos.ypos_num)/100);
         break;
      default:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->u.m_pPMD->m_dispRect.dy)/2;
         break;
   }

   if (pme->u.m_pPMD->m_dispRect.x < pRect->x)
   {
      pme->u.m_pPMD->m_offsetRect.x = (pRect->x - pme->u.m_pPMD->m_dispRect.x);
      pme->u.m_pPMD->m_dispRect.x = pRect->x;
   }
   else
      pme->u.m_pPMD->m_offsetRect.x = 0;

   if (pme->u.m_pPMD->m_dispRect.y < pRect->y)
   {
      pme->u.m_pPMD->m_offsetRect.y = (pRect->y - pme->u.m_pPMD->m_dispRect.y);
      pme->u.m_pPMD->m_dispRect.y = pRect->y;
   }
   else
      pme->u.m_pPMD->m_offsetRect.y = 0;

   MEMCPY (&pme->u.m_pPMD->m_animDispRect, &pme->u.m_pPMD->m_dispRect, 
           sizeof (AEERect));

   MEMCPY (&pme->u.m_pPMD->m_animOffsetRect, &pme->u.m_pPMD->m_offsetRect, 
           sizeof (AEERect));
   pme->u.m_pPMD->m_displayedSaf = TRUE;

}
#endif  //defined(FEATURE_SAF)



/*==================================================================

==================================================================*/
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)
static void  SetImageOffsets(CMediaPMDBg * pme) {

   AEERect *pRect;

   if (!pme->u.m_pPMD)
      return;

   pRect = &(pme->u.m_pPMD->m_rectImage);

   switch(pme->u.m_pPMD->m_fJust & 0x0f) {
      case OEMMM_JUST_LEFT:
         pme->u.m_pPMD->m_dispRect.x = pRect->x;
         break;
      case OEMMM_JUST_RIGHT:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + pRect->dx - pme->u.m_pPMD->m_dispRect.dx;
         break;
      case OEMMM_JUST_XCENTR:
         pme->u.m_pPMD->m_dispRect.x = (pRect->x + pRect->dx - pme->u.m_pPMD->m_dispRect.dx)/2; 
         break;
      case OEMMM_JUST_XPCENT:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + ((pRect->dx - pme->u.m_pPMD->m_dispRect.dx) * pme->u.m_pPMD->m_xJustPcent_Pixel)/100;
         break;
      case OEMMM_JUST_XPIXEL:
         pme->u.m_pPMD->m_dispRect.x = pRect->x + pme->u.m_pPMD->m_xJustPcent_Pixel;
         break;
      default:
         pme->u.m_pPMD->m_dispRect.x = pRect->x;
   }

   switch(pme->u.m_pPMD->m_fJust & 0xf0) {
      case OEMMM_JUST_TOP:
         pme->u.m_pPMD->m_dispRect.y = pRect->y;
         break;
      case OEMMM_JUST_BOT:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + pRect->dy - pme->u.m_pPMD->m_dispRect.dy;
         break;
      case OEMMM_JUST_YCENTR:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->u.m_pPMD->m_dispRect.dy)/2; 
         break;
      case OEMMM_JUST_YPCENT:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + ((pRect->dy - pme->u.m_pPMD->m_dispRect.dy) * pme->u.m_pPMD->m_yJustPcent_Pixel)/100;
         break;
      case OEMMM_JUST_YPIXEL:
         pme->u.m_pPMD->m_dispRect.y = pRect->y + pme->u.m_pPMD->m_yJustPcent_Pixel;
         break;
      default:
         pme->u.m_pPMD->m_dispRect.y = pRect->y;
   }

   if (pme->u.m_pPMD->m_dispRect.x < pRect->x)
   {
      pme->u.m_pPMD->m_offsetRect.x = (pRect->x - pme->u.m_pPMD->m_dispRect.x);
      pme->u.m_pPMD->m_dispRect.x = pRect->x;
   }
   else
      pme->u.m_pPMD->m_offsetRect.x = 0;

   if (pme->u.m_pPMD->m_dispRect.y < pRect->y)
   {
      pme->u.m_pPMD->m_offsetRect.y = (pRect->y - pme->u.m_pPMD->m_dispRect.y);
      pme->u.m_pPMD->m_dispRect.y = pRect->y;
   }
   else
      pme->u.m_pPMD->m_offsetRect.y = 0;

   MEMCPY (&pme->u.m_pPMD->m_picDispRect, &pme->u.m_pPMD->m_dispRect, 
           sizeof (AEERect));

   MEMCPY (&pme->u.m_pPMD->m_picOffsetRect, &pme->u.m_pPMD->m_offsetRect, 
           sizeof (AEERect));
   pme->u.m_pPMD->m_displayedPic = TRUE;

}
#endif //defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)

#ifdef FEATURE_AUDIO_POSTCARD
static void JPEGCopyBuffer(void * pUser) {
	byte * pDest;
	byte clr;
	uint32  i, end;
	boolean bReadHeader;
	cmx_af_pic_data_type *pPicData;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pPicData = (cmx_af_pic_data_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	// Figure out what to do based on combination of
	// pPicData->pic_data and pme->u.m_pPMD->m_bHeaderRead

	if(!pme->u.m_pPMD->m_bHeaderRead && pPicData->pic_data == CMX_AF_PIC_DATA_DONE)
	   // We are getting data but haven't had a header
	   return;

	if (pPicData->pic_data == CMX_AF_PIC_DATA_APPEND)
	   bReadHeader = TRUE;
	else
	   bReadHeader = FALSE;

	if(bReadHeader) {

	   int32     width, height, max_pixel_value;
	   char *   pszHeader = (char *)pPicData->buffer;
	   char *   psz;

	   if (!pszHeader)
		  return;

	   psz = STRSTR(pszHeader, "P6");

	   pme->u.m_pPMD->m_bHeaderRead = TRUE;

	   if (!psz)
		  return;

	   //Get to the end of "P6"
	   psz += 2;

	   if (!*psz)
		  return;

	   /* Header is in the first chunk of data sent */
	   sscanf( psz,
			   "%ld%ld\n%ld\n",
			   &width,
			   &height,
			   &max_pixel_value );

	   pme->u.m_pPMD->m_JPEGRect.dx = (int16)width;
	   pme->u.m_pPMD->m_JPEGRect.dy = (int16)height;

	   if(pme->u.m_pPMD->m_pIPicBitmap) {
		  if(pme->u.m_pPMD->m_pIPicBitmap->cntRGB && pme->u.m_pPMD->m_pIPicBitmap->pRGB)
			 FREE(pme->u.m_pPMD->m_pIPicBitmap->pRGB);
		  IDIB_Release(pme->u.m_pPMD->m_pIPicBitmap);
	   }


	   IDISPLAY_CreateDIBitmap(pme->u.m_pPMD->m_pIDisp,
							   (IDIB**)&pme->u.m_pPMD->m_pIPicBitmap,
							   (uint8) 24,
							   (int16) width,
							   (int16) height
							   );

	   if(pme->u.m_pPMD->m_pIPicBitmap) {

		  //We can set the pitch correctly here
		  pme->u.m_pPMD->m_pIPicBitmap->nPitch = (uint16) width * 3;
		  pme->u.m_pPMD->m_pIPicBitmap->nColorScheme = IDIB_COLORSCHEME_888;
		  goto Done;
	   }
	   else
		  return;
	}

	if (!pme->u.m_pPMD->m_pIPicBitmap || !pme->u.m_pPMD->m_pIPicBitmap->pBmp)
	   return;

	pDest = (byte *)pme->u.m_pPMD->m_pIPicBitmap->pBmp;
	end = pme->u.m_pPMD->m_JPEGRect.dx * pme->u.m_pPMD->m_JPEGRect.dy;

	MEMCPY(pDest,
		   pme->u.m_pPMD->m_pJPEGBuf,
		   end*3
		  );

	for(i = 0; i < end; i++) {
	  clr        = *pDest;
	  *pDest     = *(pDest + 2);
	  *(pDest+2) = clr;
	  pDest += 3;
	}

	// Free JPEG buffer
	FreeJPEGBuf(pme);

	goto Done;

 Done:
	if(pPicData->pic_data == CMX_AF_PIC_DATA_DONE)
	{
	   pme->u.m_pPMD->m_bHeaderRead = FALSE;
	   pme->u.m_pPMD->m_pIDispBitmap = pme->u.m_pPMD->m_pIPicBitmap;
	   MEMCPY(&pme->u.m_pPMD->m_dispRect, &pme->u.m_pPMD->m_JPEGRect, sizeof(AEERect));
	   SetImageOffsets(pme);
	   // Update the screen
	   CMXImage_Notify(pUser);
	}

	return;
}
#endif /* FEATURE_AUDIO_POSTCARD */

#if defined(FEATURE_WEBAUDIO)

/*==================================================================

==================================================================*/
static void PNGCopyBuffer(void * pUser)
{

	uint32 oldSize;
	uint32 colorType, width, height, depth, colorMapSize;
	uint32 index = 0;
	uint8 lines = 3;
	int bpp; //bytes per pixel
	byte * colorMap = NULL;
	IDIB * pDib = NULL;
	byte * pNewBuf;
	cmx_af_pic_data_type *pPicData;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pPicData = (cmx_af_pic_data_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD)
	   return;

	// Ignore the text section
	if (pPicData->pic_data == CMX_AF_PIC_DATA_TEXT)
	   return;

	// Copy out the data and append
	oldSize = pme->u.m_pPMD->m_pPNGBufSize;
	pme->u.m_pPMD->m_pPNGBufSize += pPicData->num;

	pNewBuf = (byte *)REALLOC(pme->u.m_pPMD->m_pPNGBuf, pme->u.m_pPMD->m_pPNGBufSize);
	if (pNewBuf)
	{
	   pme->u.m_pPMD->m_pPNGBuf = pNewBuf;
	}
	else
	{
	   return;
	}

	MEMCPY(pme->u.m_pPMD->m_pPNGBuf + oldSize, pPicData->buffer, pPicData->num);

	// If this is an append return, otherwise we're done so process it
	if (pPicData->pic_data == CMX_AF_PIC_DATA_APPEND)
	   return;

	// Read the header
	if (sscanf((const char *)pme->u.m_pPMD->m_pPNGBuf, "%ld\n%ld %ld\n%ld\n", &colorType, &width, &height, &depth) != 4)
	   return;

	bpp = (depth + 4)/8;

	while (lines--)
	{
	   while (pme->u.m_pPMD->m_pPNGBuf[index++] != 0x0a)
		  if (index > pme->u.m_pPMD->m_pPNGBufSize)
			 return;
	}

	do
	{
	   // Check for subchunks
	   if (!STRNICMP((const char *)&pme->u.m_pPMD->m_pPNGBuf[index], "tran", 4))
	   {
		  // transparent chunk
		  uint8 size;
		  index += 5;
		  if(sscanf((const char *)&pme->u.m_pPMD->m_pPNGBuf[index], "%d", &size) != 1)
			 return;
		  while (pme->u.m_pPMD->m_pPNGBuf[index++] != 0x0a)
			 if (index > pme->u.m_pPMD->m_pPNGBufSize)
				return;
		  index += size;
		  continue;
	   }
	   if (!STRNICMP((const char *)&pme->u.m_pPMD->m_pPNGBuf[index], "bkgd", 4))
	   {
		  // background chunk
		  while (pme->u.m_pPMD->m_pPNGBuf[index++] != 0x0a)
			 if (index > pme->u.m_pPMD->m_pPNGBufSize)
				return;
		  continue;
	   }
	   if (!STRNICMP((const char *)&pme->u.m_pPMD->m_pPNGBuf[index], "plte", 4))
	   {
		  // pallete chunk
		  index += 5;
		  if (sscanf((const char *)&pme->u.m_pPMD->m_pPNGBuf[index], "%d", &colorMapSize) != 1)
			 return;
		  while (pme->u.m_pPMD->m_pPNGBuf[index++] != 0x0a)
			 if (index > pme->u.m_pPMD->m_pPNGBufSize)
				return;
		  colorMap = (byte *)MALLOC(colorMapSize);
		  if (colorMap)
			 MEMCPY(colorMap, &pme->u.m_pPMD->m_pPNGBuf[index], colorMapSize);
		  index += colorMapSize;
		  continue;
	   }
	   // Done
	   break;
	} while (1);

	// Create the DIB
	if(pme->u.m_pPMD->m_pIPicBitmap) {
	   if(pme->u.m_pPMD->m_pIPicBitmap->cntRGB && pme->u.m_pPMD->m_pIPicBitmap->pRGB)
		  FREE(pme->u.m_pPMD->m_pIPicBitmap->pRGB);
	   IDIB_Release(pme->u.m_pPMD->m_pIPicBitmap);
	}

	   if (IDISPLAY_CreateDIBitmap(pme->u.m_pPMD->m_pIDisp,
								   (IDIB**)&pme->u.m_pPMD->m_pIPicBitmap,
								   (uint8) depth,
								   (uint16) width,
								   (uint16) height) != SUCCESS)
	   return;

	if(!pme->u.m_pPMD->m_pIPicBitmap)
	   return;

	pDib = (IDIB *)pme->u.m_pPMD->m_pIPicBitmap;
	pDib->nPitch = (uint16)width*bpp;

	if (colorType == 3)
	{
	   pDib->nColorScheme = 0;
	}
	else if(colorType == 2 || colorType == 6)
	{
	   switch (depth)
	   {
		  case 8:  pDib->nColorScheme = IDIB_COLORSCHEME_332; break;
		  case 12: pDib->nColorScheme = IDIB_COLORSCHEME_444; break;
		  case 15: pDib->nColorScheme = IDIB_COLORSCHEME_555; break;
		  case 16: pDib->nColorScheme = IDIB_COLORSCHEME_565; break;
		  case 24: pDib->nColorScheme = IDIB_COLORSCHEME_888; break;
		  case 32: pDib->nColorScheme = IDIB_COLORSCHEME_888; break;
		  default: return;
	   }
	}
	else
	{
	   return;
	}

	MEMCPY(pDib->pBmp, &pme->u.m_pPMD->m_pPNGBuf[index], width*height*bpp);
	if (colorMap)
	{
	   int i;
	   int end = colorMapSize/3;
	   uint32 * pPal;
	   byte * pSAFPal;
	   pDib->cntRGB = (uint16)colorMapSize/3;
	   pDib->pRGB = (uint32 *)MALLOC(colorMapSize/3*4);

	   if (pDib->pRGB)
	   {
		  pPal = pDib->pRGB;
		  pSAFPal = colorMap;
		  for(i = 0; i < end; i++) {
			 *pPal = (uint32) (*pSAFPal << 16) ;
			 pSAFPal++;
			 *pPal |= (uint32) (*pSAFPal << 8);
			 pSAFPal++;
			 *pPal |= (uint32) *pSAFPal;
			 pSAFPal++;
			 pPal++;
		  }
	   }
	}

	pme->u.m_pPMD->m_dispRect.dx = (uint16)width;
	pme->u.m_pPMD->m_dispRect.dy = (uint16)height;

	FREEIF(colorMap);
	FREEIF(pme->u.m_pPMD->m_pPNGBuf);
	pme->u.m_pPMD->m_pPNGBuf = NULL;
	pme->u.m_pPMD->m_pPNGBufSize = 0;

	if(pPicData->pic_data == CMX_AF_PIC_DATA_DONE) {
	   pme->u.m_pPMD->m_pIDispBitmap = pme->u.m_pPMD->m_pIPicBitmap;
	   SetImageOffsets(pme);
	   CMXImage_Notify(pUser);
	}

	return;
}


/*==================================================================
Called in user context to free a buffer allocated in user context
==================================================================*/
static void FreeClientBuf(void * pUser)
{
	int nCopy;
	ClientBufInfo *pClientBufInfo;
	uint8* pClientBufSent;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pClientBufSent = (uint8*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->u.m_pPMD || !pme->u.m_pPMD->m_pClientBufInfos || !pClientBufSent)
	   return;

	//Find the buffer in the list and free it
	pClientBufInfo = GetClientBufInfo(pme, pClientBufSent);
	if (!pClientBufInfo)
	   return;

	FREEIF(pClientBufInfo->m_buf);
	pClientBufInfo->m_size = 0;
	pClientBufInfo->m_copycount = 0;
	for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
	{
	   FREEIF(pClientBufInfo->m_copies[nCopy]);
	}
}
#endif //defined(FEATURE_WEBAUDIO)


static size_t strlcpy(char * dst, const char * src, size_t nSize)
{
   size_t  ret;
   char   *pcDst;

   if( !nSize )
      return STRLEN(src);

   for( ret = 0, pcDst = dst; (ret < nSize) && (*pcDst++ = *src); ret++, src++ ) {}

   if( ret == nSize ) dst[nSize-1] = '\0';

   while( *src++ ) ret++;

   return ret;
}


#if defined(FEATURE_WEBAUDIO)
/*==================================================================
Releases text: Must be called in user context
==================================================================*/
static void TextRelease(void * pUser) 
{
   TextBlock * pBlock, * pOldBlock;
   CMediaPMD * pmdInfo;

   ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
   CMediaPMDBg *   pme = (CMediaPMDBg *)pUser;

   if (!pcscb)
      return;

   pme = (CMediaPMDBg *)(pcscb->pIMedia);

   UnlockCSCallback(pcscb);

   if (!pme || !pme->u.m_pPMD)
      return;

   pmdInfo = (CMediaPMD *)pme->u.m_pPMD;
   
   pBlock=pmdInfo->m_textBlock;
   while (pBlock)
   {
      FREEIF(pBlock->m_text);
      pOldBlock = pBlock;
      pBlock = pOldBlock->m_next;
      FREEIF(pOldBlock);
   }
   pmdInfo->m_textBlock = NULL;
   pmdInfo->m_numToWipe = 0;
   pmdInfo->m_fTextDraw |= OEMMM_DISP_CLEAR;
}
#endif //defined(FEATURE_WEBAUDIO)


/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetVolume(CMediaPMDBg * pme, uint16 wVolume)
{
   if (!pme->m_pISound)
      return EUNSUPPORTED;

   if (wVolume > 100)
      return EBADPARM;

   ISOUND_Get(pme->m_pISound, NULL);	// Update the current device settings
   ISOUND_SetVolume(pme->m_pISound, wVolume);

   return MM_PENDING;
}


/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_Resume(CMediaPMDBg * pme)
{
   AEEMediaCallback *   pcb;
   int                  nCmd;

   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_RESUME);
   if (!pcb)
      return EITEMBUSY;
#ifdef FEATURE_MULTISEQUENCER
      if (CMediaPMDMMLayer_IsMultiSequence(pme))
      {
         cmx_audfmt_seq_param_type seqParam;
         seqParam.cmd = CMX_AUDFMT_SEQ_CMD_RESUME;
         seqParam.num = 0;
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
      }
#endif  // FEATURE_MULTISEQUENCER
   OEMMediaPMD_Resume(pme->m_pMediaCMX);

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_Pause(CMediaPMDBg * pme)
{
   AEEMediaCallback *   pcb;
   int                  nCmd;

   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_PAUSE);
   if (!pcb)
      return EITEMBUSY;
#ifdef FEATURE_MULTISEQUENCER
      /* See if it is a multisequence */
      if (CMediaPMDMMLayer_IsMultiSequence(pme))
   {
         cmx_audfmt_seq_param_type seqParam;
         seqParam.cmd = CMX_AUDFMT_SEQ_CMD_PAUSE;
         seqParam.num = 0; 
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
      }
#endif
   OEMMediaPMD_Pause(pme->m_pMediaCMX);
   return SUCCESS;
}
/*==================================================================

==================================================================*/
static AEEMediaCallback * CMediaPMDMMLayer_AllocCallback(AEEMediaCallback * pcb, int nMax, int nCmd, int nSubCmd)
{
   AEEMediaCallback *   pcbRet = NULL;

   OEMMM_ENTER_CRITICAL_SECTION()

   int   i;

   for (i = 0; i < nMax; i++)
   {
      if (!pcb[i].bInUse)
      {
         pcb[i].bInUse = TRUE;
         pcb[i].cmdNotify.nCmd = nCmd;
         pcb[i].cmdNotify.nSubCmd = nSubCmd;
         pcb[i].cmdNotify.pCmdData = NULL;
         pcb[i].cmdNotify.dwSize = 0;
         pcbRet = &pcb[i];
         goto Done;
      }
   }

Done:
   OEMMM_LEAVE_CRITICAL_SECTION()

   return pcbRet;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetDevice(CMediaPMDBg * pme, AEESoundDevice eDevice)
{
   AEESoundInfo   si;
   ISound *       pISound = pme->m_pISound   ;

   if (!pISound)
      return EUNSUPPORTED;

   ISOUND_Get(pISound, &si);
   si.eDevice = eDevice;
   ISOUND_Set(pISound, &si);

   ISOUND_SetDevice(pISound);

   return MM_PENDING;
}

/*==================================================================
   For playback
==================================================================*/
static int CMediaPMDMMLayer_SetPath(CMediaPMDBg * pme, int nAPath)
{
   return EUNSUPPORTED;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetMute(CMediaPMDBg * pme, boolean bMute)
{
   pme->m_bMute = bMute;

   return CMediaPMDMMLayer_SetVolume(pme, (uint16)(bMute ? 0 : pme->m_wVolume));
}


/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetPan(CMediaPMDBg * pme, uint32 dwPan)
{
   AEEMediaCallback *   pcb;

#ifdef FEATURE_MULTISEQUENCER    // Pan is currently unsupported
   if (CMediaPMDMMLayer_IsMultiSequence(pme))
   {
      return EUNSUPPORTED;
   }
#endif // FEATURE_MULTISEQUENCER

   if (dwPan > MM_MAX_PAN)
      return EBADPARM;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_PAN);
   if (!pcb)
      return EITEMBUSY;

#if !defined(AEE_SIMULATOR)
   pme->m_wPan = (uint16)dwPan;

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_PAN, (int32)dwPan, 0, 0, FALSE);

   return MM_PENDING;
#else
   CMediaPMDMMLayer_FreeCallback(pcb);
   return EFAILED;
#endif //!defined(AEE_SIMULATOR)
}


/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetTune(CMediaPMDBg * pme, uint32 dwTune)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_TUNE);
   if (!pcb)
      return EITEMBUSY;

   pme->m_wTune = (uint16)dwTune;

#ifdef FEATURE_MULTISEQUENCER
   if (CMediaPMDMMLayer_IsMultiSequence(pme))
   {
      cmx_audfmt_seq_param_type seqParam;
      seqParam.cmd = CMX_AUDFMT_SEQ_CMD_TUNE;
      seqParam.num = dwTune;
      OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, FALSE);      
   }
   else
#endif  // FEATURE_MULTISEQUENCER
   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TUNE, (int32)dwTune, 0, 0, FALSE);

   return MM_PENDING;
}


/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetTempo(CMediaPMDBg * pme, uint32 dwTempo)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_TEMPO);
   if (!pcb)
      return EITEMBUSY;

   pme->m_wTempo = (uint16)dwTempo;

#ifdef FEATURE_MULTISEQUENCER
   if (CMediaPMDMMLayer_IsMultiSequence(pme))
   {
      cmx_audfmt_seq_param_type seqParam;
      seqParam.cmd = CMX_AUDFMT_SEQ_CMD_TEMPO;
      seqParam.num = dwTempo;
      OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, FALSE);    
   }
   else
#endif  // FEATURE_MULTISEQUENCER

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TEMPO, (int32)dwTempo, 0, 0, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_PlayCodec(CMediaPMDBg * pme, cmx_handle_type * ph, AEEMediaCallback * pcb)
{
   return EUNSUPPORTED;
}
/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_Seek(CMediaPMDBg * pme, AEEMediaSeek eSeek, int32 lTimeMS)
{
   AEEMediaCallback *   pcb;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_PLAY, OEMMM_CTLCMD_SEEK);
   if (!pcb)
      return EITEMBUSY;

   switch (eSeek)
   {
      case MM_SEEK_CURRENT:
#ifdef FEATURE_MULTISEQUENCER
          /* See if it is a multisequence */
          if (CMediaPMDMMLayer_IsMultiSequence(pme))
          {
             cmx_audfmt_seq_param_type seqParam;
             seqParam.cmd = lTimeMS < 0 ? CMX_AUDFMT_SEQ_CMD_REWIND : CMX_AUDFMT_SEQ_CMD_FFORWARD;
             seqParam.num = lTimeMS < 0 ? -lTimeMS : lTimeMS;
             OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
          }
#endif  // FEATURE_MULTISEQUENCER          
         OEMMediaPMD_Seek(pme->m_pMediaCMX, MM_SEEK_CURRENT, lTimeMS);
         break;

      case MM_SEEK_START:
      case MM_SEEK_END:
      {
         CMediaPMDMMLayer_FreeCallback(pcb);
         return EUNSUPPORTED;
      }
   }

   return SUCCESS;
}


/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_InitCallback(IMedia * po, int nCmd, AEEMediaCallback * pcb, int nMax, PFNNOTIFY pfn)
{
   AEEMedia *  pMedia = (AEEMedia *)po;
   int         i;

   for (i = 0; i < nMax; i++)
   {
      pcb[i].bInUse = FALSE;
      CALLBACK_Init(&pcb[i].cb, pfn, &pcb[i]);
      pcb[i].cmdNotify.clsMedia = pMedia->m_clsSelf;
      pcb[i].cmdNotify.pIMedia = po;
      pcb[i].cmdNotify.nCmd = nCmd;
   }
}


/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_FreeCallback(AEEMediaCallback * pcb)
{
   OEMMM_ENTER_CRITICAL_SECTION()

   pcb->bInUse = FALSE;
   pcb->cmdNotify.nSubCmd = 0;
   pcb->cmdNotify.pCmdData = NULL;
   pcb->cmdNotify.dwSize = 0;

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_CancelCallback(AEEMediaCallback * pcb, int nMax)
{
   OEMMM_ENTER_CRITICAL_SECTION()

   int   i;

   for (i = 0; i < nMax; i++)
   {
      CALLBACK_Cancel(&pcb[i].cb);
   }

   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_Stop(CMediaPMDBg * pme)
{
   AEEMediaCallback *   pcb = &pme->m_cbCommand;
//   int                  nCmd;

   // Callback must always be available for stop command.
   CMediaPMDMMLayer_FreeCallback(pcb);
//   nCmd = AEEMedia_IsPlayOn(pme) ? MM_CMD_PLAY : MM_CMD_RECORD;
//   CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, nCmd, OEMMM_CTLCMD_STOP);

   // Media is stopping...do not process callback data.
   pme->m_bStopping = TRUE;

   //Cancel the callback to draw
   #if defined(FEATURE_WEBAUDIO)
     if ((pme->m_clsSelf == AEECLSID_MEDIAPMD
#if defined(FEATURE_SVG)
        || pme->m_clsSelf == AEECLSID_MEDIASVG
#endif
        ) && pme->u.m_pPMD)
	   CancelCSCallback(pme->u.m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH);
   #endif // defined(FEATURE_WEBAUDIO)

   if (pme->m_clsSelf == AEECLSID_MEDIAQCP && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_QCP)
      OEMMediaPMD_Stop(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_QCP)
   }
   else if (((pme->m_clsSelf == AEECLSID_MEDIAPCM) || (pme->m_clsSelf == AEECLSID_MEDIAADPCM)) && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_PCM_REC)
      cmx_pcm_rec_link_type link;

      if (pme->m_nAPath == MM_APATH_LOCAL)
         link = CMX_PCM_REC_LINK_REVERSE;
      else if (pme->m_nAPath == MM_APATH_REMOTE)
         link = CMX_PCM_REC_LINK_FORWARD;
      else
         link = CMX_PCM_REC_LINK_INVALID;

      if (link != CMX_PCM_REC_LINK_INVALID)
      {
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_LINK, (int32)link, 0, 0, TRUE);
      }
   else
   {
         return EBADPARM;
      }

      OEMMediaPMD_Stop(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_PCM_REC)
   }
   else if (pme->m_clsSelf == AEECLSID_MEDIAAAC && AEEMedia_IsRecOn(pme))
   {
#if defined(FEATURE_AAC_REC)
	OEMMediaPMD_Stop(pme->m_pMediaCMX);
#else
      return EFAILED;
#endif // defined(FEATURE_AAC_REC)
   }
   else
   {
      OEMMediaPMD_Stop(pme->m_pMediaCMX);
   }

   return SUCCESS;
}

/*==================================================================
  This version has a signature used by some stand-alone decoders
==================================================================*/
static int CMediaPMDMMLayer_GetMMStatus2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, OEMMMStatus * peStatus)
{
   int   nStatus = MM_STATUS_UNKNOWN;

   if (peStatus)
      *peStatus = OEMMM_STATUS_PLAY;

   switch (status)
   {
      case CMX_SUCCESS:             
         break;

      case CMX_DONE:                
         nStatus = MM_STATUS_DONE; 
         pme->m_bStopping = FALSE;
         break;

      case CMX_ABORTED:             
         // Differentiate between error and user stop
         if (pme->m_bStopping)
            nStatus = MM_STATUS_DONE;
         else
            nStatus = MM_STATUS_ABORT; 
         pme->m_bStopping = FALSE;
         break;

      case CMX_PAUSE:
      case CMX_REC_PAUSED:
         nStatus = MM_STATUS_PAUSE;
         if (peStatus)
            *peStatus = OEMMM_STATUS_PLAYCTL;
         break;

      case CMX_RESUME:              
      case CMX_REC_RESUMED:              
         nStatus = MM_STATUS_RESUME;
         if (peStatus)
            *peStatus = OEMMM_STATUS_PLAYCTL;
         break;

      case CMX_ERROR: // fall thru...              
      case CMX_FAILURE:             
         if (pme->m_bStopping)
            pme->m_bStopping = FALSE;
         nStatus = MM_STATUS_ABORT;
         break;
         
      case CMX_CLIENT_BUF_FREE:
      case CMX_CLIENT_BUF_REQ:
#if defined(FEATURE_WEBAUDIO)
         if (!pme->m_bStopping)
            CMXImage_Process2((CMediaPMDBg *)pme, status, data, num_bytes);
#endif // defined(FEATURE_WEBAUDIO)
         break;

      case CMX_ENABLED: // fall thru...
      case CMX_DISABLED:            
         break;

      case CMX_TICK_UPDATE:
         if (pme->m_dwTickTimeMS)
            nStatus = MM_STATUS_TICK_UPDATE;
         break;

      case CMX_REPEAT:              
         nStatus = MM_STATUS_REPEAT; 
#if defined(FEATURE_WEBAUDIO)
         if (pme->m_clsSelf == AEECLSID_MEDIAPMD)
			 CMXClear((CMediaPMDBg*)pme, CLEAR_IMAGE | CLEAR_ANIM | CLEAR_TEXT | CLEAR_REDRAW);
#endif // defined(FEATURE_WEBAUDIO)
         break;

      case CMX_DATA_ACCESS_DELAY:   
         nStatus = MM_STATUS_DATA_IO_DELAY; 
         break;

      case CMX_FLUSHED:  // fall thru...
      case CMX_INVALID_DATA_SOURCE: 
         break;

      case CMX_REWIND: // fall thru...         
      case CMX_FFORWARD:
         nStatus = MM_STATUS_SEEK;
         if (peStatus)
            *peStatus = OEMMM_STATUS_PLAYCTL;
#if defined(FEATURE_WEBAUDIO)
         if (pme->m_clsSelf == AEECLSID_MEDIAPMD)
			 CMXClear((CMediaPMDBg*)pme, CLEAR_IMAGE | CLEAR_ANIM | CLEAR_TEXT | CLEAR_REDRAW);
#endif // defined(FEATURE_WEBAUDIO)
         break;

#if defined(FEATURE_SVG)
      case CMX_SVG_INFO:
         if (data)
         {
            nStatus = MM_STATUS_MEDIA_SPEC;
            CopySVGInfo(&(((CMediaPMDBg *) pme)->u.m_pPMD->m_SVGspec), (cmx_svg_info_type*)data);

            OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_HANDLE, (int32)((cmx_svg_info_type*)data)->handle, 0, 0, TRUE);

            if (!pme->m_bStopping)
               CMXImage_Process2((CMediaPMDBg *)pme, status, data, num_bytes);
         }
         break;
         
      case CMX_SVG_FRAME:
         if (!pme->m_bStopping)
         {
			 CMediaPMDBg * pmeBg = (CMediaPMDBg *)pme;
          if (pmeBg->u.m_pPMD->m_FrameCallbackEnabled)
          {
             nStatus = MM_STATUS_FRAME;
          }
			 nStatus = CMXImage_Process2(pmeBg, status, data, num_bytes);
         }
         break;
      
      case CMX_SVG_PREPARE_DONE:
         nStatus = MM_STATUS_SVG_PREPARE_DONE;
         break;
         
      case CMX_SVG_URL_REQ:
         nStatus = MM_STATUS_SVG_URL;
         break;

         case CMX_SVG_EVENT:
         if (data)
         {
            cmx_svg_event_done_type* pEventResult = (cmx_svg_event_done_type*)data;
            nStatus = pEventResult->handled ? MM_STATUS_DONE : MM_STATUS_ABORT;
         }
         if (peStatus)
            *peStatus = OEMMM_STATUS_SETPARM;
         break;

#if (SVGT_VERSION > SVGT_1_1)
#if defined(MM_STATUS_SVG_URL_IMAGE)
      case CMX_SVG_EXT_REQ:  // Request for external URL data
         {
            CMediaPMDBg * pmeBg = (CMediaPMDBg *)pme;
         
            if (pmeBg->u.m_pPMD->m_SVGURLImageHandled)
            {
               cmx_svg_request_type* pReq = (cmx_svg_request_type*)data;

               if (pReq && (0 == STRNCMP(pReq->filename, HTTP_STR, HTTP_STRLEN)))
               {
                  nStatus = MM_STATUS_SVG_URL_IMAGE;
               }
            }
         }   
         break;

      case CMX_SVG_EXT_FREE:  // Ack that URL data was loaded
         nStatus = MM_STATUS_SVG_URL_DATA;
         {
            CMediaPMDBg * pmeBg = (CMediaPMDBg *)pme;
         
            if (pmeBg->u.m_pPMD->m_SVGURLImageHandled)
            {
               cmx_svg_request_type* pReq = (cmx_svg_request_type*)data;

               if (pReq && (0 == STRNCMP(pReq->filename, HTTP_STR, HTTP_STRLEN)))
               {
                  nStatus = MM_STATUS_SVG_URL_DATA;
               }
            }
         }
         break;
#endif // defined(MM_STATUS_SVG_URL_IMAGE)
#endif // (SVGT_VERSION > SVGT_1_1)

#if defined(FEATURE_SVGDOM)
      case CMX_SVG_DOM_DATA:
         nStatus = CMX_SUCCESS;
         if (peStatus)
            *peStatus = OEMMM_STATUS_SETPARM;
         break;
#endif //defined(FEATURE_SVGDOM)
#endif // FEATURE_SVG
   }

   return nStatus;
}

#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SVG)
/*==================================================================
  This version is called for standalone decoders.
==================================================================*/
static int CMXImage_Process2(CMediaPMDBg * pme, cmx_status_type status, void* data, uint32 num_bytes)
{
	int nCallback = -1;
 #if defined(FEATURE_SVG)
	ClientBufInfo* pClientBufInfo = NULL;
	int nMaxCallback;
 #endif //defined(FEATURE_SVG)
	int   nStatus = MM_STATUS_UNKNOWN;

	if (!data)
	   return nStatus;

	if (!pme->u.m_pPMD)
	   return nStatus;

	if (!(pme->m_dwEnabledCaps & MM_CAPS_VIDEO))
	   return nStatus;

	switch (status)
	{
	   case CMX_CLIENT_BUF_FREE:
		  nCallback = LockCSCallback(pme->u.m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;

		  pme->u.m_pPMD->m_ClientBufSent[nCallback] = (uint8*)data;
		  pme->u.m_pPMD->m_cbClientBufFree[nCallback].pPayload = &pme->u.m_pPMD->m_ClientBufSent[nCallback];

		  AEE_ResumeCallback(&pme->u.m_pPMD->m_cbClientBufFree[nCallback].cb, pme->m_pac);
		  break;

	   case CMX_CLIENT_BUF_REQ:
	   {
		  cmx_af_server_data_type* psd = (cmx_af_server_data_type*)data;
		  nCallback = LockCSCallback(pme->u.m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;

		  pme->u.m_pPMD->m_ClientBufRequest[nCallback].nClientBufSize = psd->client_buf_req.buffer_size;
		  pme->u.m_pPMD->m_ClientBufRequest[nCallback].cbClientBufAllocNotify = psd->client_buf_req.cb_func;
		  pme->u.m_pPMD->m_ClientBufRequest[nCallback].pBufClientData = psd->client_buf_req.client_data;

		  pme->u.m_pPMD->m_cbClientBufAlloc[nCallback].pPayload = &pme->u.m_pPMD->m_ClientBufRequest[nCallback];

		  psd->client_buf_req.buffer_supplied = TRUE;

		  AEE_ResumeCallback(&pme->u.m_pPMD->m_cbClientBufAlloc[nCallback].cb, pme->m_pac);
		  break;
	   }

 #if defined(FEATURE_SVG)
	   case CMX_SVG_INFO:
		  //Copy server data and queue callback in user mode
		  nCallback = LockCSCallback(pme->u.m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;
		  pme->u.m_pPMD->m_cmx_svg_info[nCallback] = *(cmx_svg_info_type *)data;
		  pme->u.m_pPMD->m_cbSVGCreateDIB[nCallback].pPayload = &pme->u.m_pPMD->m_cmx_svg_info[nCallback];
		  AEE_ResumeCallback(&pme->u.m_pPMD->m_cbSVGCreateDIB[nCallback].cb, pme->m_pac);
		  break;

	   case CMX_SVG_FRAME:
		  {
			 cmx_svg_frame_done_type *pFrameData = (cmx_svg_frame_done_type *)data;

			 if (pFrameData->frame_buf)
			 {
				// Find out which ClientBufInfo structure contains this buffer
				pClientBufInfo = GetClientBufInfo(pme, pFrameData->frame_buf);
				if (!pClientBufInfo)
				   return nStatus;

				nMaxCallback = pClientBufInfo->m_copycount;
			 }
			 else
			 {
				nMaxCallback = OEMMM_MAX_CB_CONTEXT_SWITCH;
			 }

			 nCallback = LockCSCallback(pme->u.m_pPMD->m_cbSVGLoadFrame, nMaxCallback, pme);
			 if (nCallback == -1)
				return nStatus;

			 // Shallow copy the structure received
			 pme->u.m_pPMD->m_SVGFrameData[nCallback] = *pFrameData;

			 // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
			 if (pFrameData->frame_buf != NULL)
			 {
				MEMCPY(pClientBufInfo->m_copies[nCallback], pFrameData->frame_buf, pClientBufInfo->m_size);
				pme->u.m_pPMD->m_SVGFrameData[nCallback].frame_buf = pClientBufInfo->m_copies[nCallback];
			 }

			 pme->u.m_pPMD->m_cbSVGLoadFrame[nCallback].pPayload = &pme->u.m_pPMD->m_SVGFrameData[nCallback];
			 AEE_ResumeCallback(&pme->u.m_pPMD->m_cbSVGLoadFrame[nCallback].cb, pme->m_pac);

			 if (pme->u.m_pPMD->m_FrameCallbackEnabled)
			 {
				nStatus = MM_STATUS_FRAME;
			 }

			 break;
		  }
 #endif //defined(FEATURE_SVG)

	   default:
		  break;

	}

	return nStatus;
}
#endif

/*==================================================================

==================================================================*/
#if defined(FEATURE_WEBAUDIO)
static int CMXImage_Process(CMediaPMDBg * pme, cmx_status_type status, cmx_af_server_data_type * psd)
{
	int nCallback = -1;
	ClientBufInfo* pClientBufInfo = NULL;
	int nMaxCallback;
	int   nStatus = MM_STATUS_UNKNOWN;


	if (!psd)
	   return nStatus;

	if (!pme->u.m_pPMD)
	   return nStatus;

	if (!(pme->m_dwEnabledCaps & MM_CAPS_VIDEO))
	   return nStatus;

   switch (status)
   {
      case CMX_PICTURE_STATUS:
         switch(psd->pic_status.pic_status) {
            case CMX_AF_PIC_STATUS_INIT:
               //Copy server data and queue callback in user mode
               nCallback = LockCSCallback(pme->u.m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
               if (nCallback == -1)
                  return nStatus;
               pme->u.m_pPMD->m_picStatus[nCallback] = psd->pic_status;
               pme->u.m_pPMD->m_cbPicInitialize[nCallback].pPayload = &pme->u.m_pPMD->m_picStatus[nCallback];
               AEE_ResumeCallback(&pme->u.m_pPMD->m_cbPicInitialize[nCallback].cb, pme->m_pac);
               break;

		    case CMX_AF_PIC_STATUS_CLEAR:
				// pme->u.m_pPMD->m_fPicDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag
				CMXClear(pme, CLEAR_IMAGE | CLEAR_REDRAW);  
				if (pme->u.m_pPMD->m_FrameCallbackEnabled)
				   nStatus = MM_STATUS_FRAME;
				break;
			 case CMX_AF_PIC_STATUS_ENABLE:
				// pme->u.m_pPMD->m_fPicDraw |= OEMMM_DISP_DRAW;   // Set DRAW flag, leave prev. CLEAR flag
				CMXChangeDrawStatus(pme, &pme->u.m_pPMD->m_fPicDraw, OEMMM_DISP_DRAW, OEMMM_DRAW_OP_OR_EQ, TRUE);
				if (pme->u.m_pPMD->m_FrameCallbackEnabled)
				   nStatus = MM_STATUS_FRAME;
				break;
			 case CMX_AF_PIC_STATUS_DISABLE:
				//pme->u.m_pPMD->m_fPicDraw = OEMMM_DISP_CLEAR;   // Set CLEAR flag, clear DRAW flag
				CMXChangeDrawStatus(pme, &pme->u.m_pPMD->m_fPicDraw, OEMMM_DISP_CLEAR, OEMMM_DRAW_OP_EQ, TRUE);
				if (pme->u.m_pPMD->m_FrameCallbackEnabled)
				   nStatus = MM_STATUS_FRAME;
				break;
            default:
               break;
         }
         break;

      case CMX_PICTURE_DATA:
		  {
			 ContextSwitchCallback *pcscb = NULL;

			 if (psd->pic_data.buffer)
			 {
				// Find out which ClientBufInfo structure contains this buffer
				pClientBufInfo = GetClientBufInfo(pme, (uint8*)psd->pic_data.buffer);
				if (!pClientBufInfo)
				   return nStatus;

				nMaxCallback = pClientBufInfo->m_copycount;
			 }
			 else
			 {
				nMaxCallback = OEMMM_MAX_CB_CONTEXT_SWITCH;
			 }

			 // Queue callback in user mode
			 switch(psd->pic_data.pic_type) {
				case CMX_AF_PIC_BMP:
				   //process BMP only if we have all the data
				   if(psd->pic_data.pic_data == CMX_AF_PIC_DATA_DONE) {
					  nCallback = LockCSCallback(pme->u.m_pPMD->m_cbCopyBMPBuf, nMaxCallback, pme);
					  if (nCallback == -1)
						 return nStatus;
					  pcscb = &pme->u.m_pPMD->m_cbCopyBMPBuf[nCallback];
				   }
				   break;

				case CMX_AF_PIC_PNG:
				   nCallback = LockCSCallback(pme->u.m_pPMD->m_cbCopyPNGBuf, nMaxCallback, pme);
				   if (nCallback == -1)
					  return nStatus;
				   pcscb = &pme->u.m_pPMD->m_cbCopyPNGBuf[nCallback];
				   break;


 #ifdef FEATURE_AUDIO_POSTCARD
				case CMX_AF_PIC_JPEG:
				   nCallback = LockCSCallback(pme->u.m_pPMD->m_cbCopyJPEGBuf, nMaxCallback, pme);
				   if (nCallback == -1)
					  return nStatus;
				   pcscb = &pme->u.m_pPMD->m_cbCopyJPEGBuf[nCallback];
				   break;
 #endif /* FEATURE_AUDIO_POSTCARD */

				default:
				   return nStatus;
			 }

			 // Shallow copy the structure received
			 pme->u.m_pPMD->m_picData[nCallback] = psd->pic_data;

			 // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
			 if (psd->pic_data.buffer != NULL)
			 {
				MEMCPY(pClientBufInfo->m_copies[nCallback], psd->pic_data.buffer, pClientBufInfo->m_size);
				pme->u.m_pPMD->m_picData[nCallback].buffer = pClientBufInfo->m_copies[nCallback];
			 }

			 pcscb->pPayload = &pme->u.m_pPMD->m_picData[nCallback];
			 AEE_ResumeCallback(&pcscb->cb, pme->m_pac);

			 if(psd->pic_data.pic_data == CMX_AF_PIC_DATA_DONE && pme->u.m_pPMD->m_FrameCallbackEnabled)
				nStatus = MM_STATUS_FRAME;
		  }
		  break;

      case CMX_CLIENT_BUF_FREE:
         CMXImage_Process2(pme, status, psd->client_buf_free, sizeof(uint8));
         break;
         
      case CMX_CLIENT_BUF_REQ:
         CMXImage_Process2(pme, status, psd, sizeof(cmx_af_server_data_type));
         break;

#if defined(FEATURE_BREW_CMX_V3)
      case CMX_GET_BUFFER:
         //Copy the callback data and queue the allocation to happen in user mode
         pme->u.m_pPMD->m_pJPEGBufSize = psd->client_buf_req.buffer_size;
         pme->u.m_pPMD->m_cbJPEGBufAllocNotify = psd->client_buf_req.cb_func;
         pme->u.m_pPMD->m_pJPEGClientData = psd->client_buf_req.client_data;

         AEE_ResumeCallback(&pme->u.m_pPMD->m_cbJPEGBufAlloc, pme->m_pac);
         break;
#endif // defined(FEATURE_BREW_CMX_V3)

#if defined(FEATURE_SAF)
      case CMX_SAF_INFO:
         //this is where all the SAF frame stuff comes

         //Copy server data and queue callback in user mode
		  nCallback = LockCSCallback(pme->u.m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;
		  pme->u.m_pPMD->m_safInfo[nCallback] = psd->saf_info;
		  pme->u.m_pPMD->m_cbSAFCreateDIB[nCallback].pPayload = &pme->u.m_pPMD->m_safInfo[nCallback];
		  AEE_ResumeCallback(&pme->u.m_pPMD->m_cbSAFCreateDIB[nCallback].cb, pme->m_pac);
         break;

      case CMX_SAF_FRAME:
		  if (psd->saf_data.buffer)
		  {
			 // Find out which ClientBufInfo structure contains this buffer
			 pClientBufInfo = GetClientBufInfo(pme, (uint8*)psd->saf_data.buffer);
			 if (!pClientBufInfo)
				return nStatus;

				nMaxCallback = pClientBufInfo->m_copycount;
		  }
		  else
		  {
			 nMaxCallback = OEMMM_MAX_CB_CONTEXT_SWITCH;
		  }

		  nCallback = LockCSCallback(pme->u.m_pPMD->m_cbSAFLoadFrame, nMaxCallback, pme);
		  if (nCallback == -1)
			 return nStatus;

		  // Shallow copy the structure received
		  pme->u.m_pPMD->m_safData[nCallback] = psd->saf_data;

		  // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
		  if (psd->saf_data.buffer != NULL)
		  {
			 MEMCPY(pClientBufInfo->m_copies[nCallback], psd->saf_data.buffer, pClientBufInfo->m_size);
			 pme->u.m_pPMD->m_safData[nCallback].buffer = pClientBufInfo->m_copies[nCallback];
		  }

		  pme->u.m_pPMD->m_cbSAFLoadFrame[nCallback].pPayload = &pme->u.m_pPMD->m_safData[nCallback];
		  AEE_ResumeCallback(&pme->u.m_pPMD->m_cbSAFLoadFrame[nCallback].cb, pme->m_pac);

		  if (pme->u.m_pPMD->m_FrameCallbackEnabled)
			 nStatus = MM_STATUS_FRAME;

		  break;
#endif  //defined(FEATURE_SAF)

      case CMX_ANIM_CLEAR:
		  // pme->u.m_pPMD->m_fSafDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag
		  CMXClear(pme, CLEAR_ANIM | CLEAR_REDRAW);
          if (pme->u.m_pPMD->m_FrameCallbackEnabled)
		    nStatus = MM_STATUS_FRAME;
          break;

      case CMX_ANIM_ENABLE:
		  // pme->u.m_pPMD->m_fSafDraw |= OEMMM_DISP_DRAW;   // Set DRAW flag, leave prev. CLEAR flag
		  CMXChangeDrawStatus(pme, &pme->u.m_pPMD->m_fAnimDraw, OEMMM_DISP_DRAW, OEMMM_DRAW_OP_OR_EQ, TRUE);         
		  if (pme->u.m_pPMD->m_FrameCallbackEnabled)
			 nStatus = MM_STATUS_FRAME;
		  break;

      case CMX_ANIM_DISABLE:
		  //pme->u.m_pPMD->m_fSafDraw = OEMMM_DISP_CLEAR;   // Set CLEAR flag, clear DRAW flag
		  CMXChangeDrawStatus(pme, &pme->u.m_pPMD->m_fAnimDraw, OEMMM_DISP_CLEAR, OEMMM_DRAW_OP_EQ, TRUE);         
		  if (pme->u.m_pPMD->m_FrameCallbackEnabled)
			 nStatus = MM_STATUS_FRAME;
		  break;

      default:
         break;
 
   }

   return nStatus;

}
#endif //defined(FEATURE_WEBAUDIO)


/*==================================================================
   Assumption: pme and ph are not NULL.
==================================================================*/
int CMediaPMDMMLayer_SetSource(CMediaCMX * pme, cmx_handle_type * ph)
{
   AEEMedia *     pMedia = (AEEMedia *)pme;
   AEEMediaData * pmd = &pMedia->m_md;
   int            nRet = SUCCESS;

   if (pmd->clsData == MMD_FILE_NAME)
   {
      ph->efs.source_type = CMX_SOURCE_EFS;
      if (pmd->pData)
         strlcpy(ph->efs.filename, pMedia->m_pszFile, sizeof(ph->efs.filename));
      else
         nRet = EBADPARM;
   }
   else if (pmd->clsData == MMD_BUFFER)
   {
      ph->mem.source_type = CMX_SOURCE_MEM;
      if ( pmd->pData )
      {
         ph->mem.buf = (byte *)pmd->pData;
         /* 
            OEM NOTE: Backward Compatibility
            ISOUNDPLAYER_Set with SDT_BUFFER workaround
            if the size was set to 16, then ISOUNDPLAYER
            Placed 16 as a key to calculate the size of the media
            Please determine the type of media, then the size of the
            media and fill hType.dwLength with the new value 
         */
         /*
           if( pmd->dwSize == 16 ){
             uint32 nType;
             nType = GetMediaFormat(pmd->pData); // Write this function
             switch( nType ){
             case 0x444D4D4D:
               //calculate SMAF MMF data size, inline or in function
               pmd->dwSize = GetMMFSMAFSize(pmd->pData);// Write this function
             case 0x6468544D:
               // calculate MIDI data size, inline or in function
               pmd->dwSize = GetMidiSize(pmd->pData);// Write this function
             case ...: // Other OEM known formats
               pmd->dwSize = GetOtherSize(pmd->pData);// Write these functions
             ...
             }
          } 
         */
         ph->mem.len = pmd->dwSize;
      }
      else
         nRet = EBADPARM;
   }

   if (pmd->clsData >= MMD_ISOURCE)
   {
      // Perform streaming....
      
      CMediaPMDBg *  pBg = (CMediaPMDBg *)pme;

      if (AEECLSID_MEDIAMIDIOUTQCP == pme->m_clsSelf || 
          AEECLSID_MEDIAMIDIOUTMSG == pme->m_clsSelf)
      {
          return EUNSUPPORTED;
      }

      if (OEMMM_ISSTREAMING(pBg))
         CMediaPMDMMLayer_FlushStreamData(pBg);
      else
      {
         uint32   dwBufSize = pme->m_dwBufferSize ? pme->m_dwBufferSize : OEMMM_BUFFER_SIZE_MAX;
         FARF(SOURCE, ("===Src setup: Start"));
         if (CQueue_New(OEMMM_CMX_REQ_SIZE_MAX, TRUE, &pBg->m_pqCMXDataReq))
            return ENOMEMORY;

         if (CQueue_New(dwBufSize, FALSE, &pBg->m_pqMediaData))
            return ENOMEMORY;
      
         pBg->m_pcbCMXDataReq = MALLOC(sizeof(AEECallback));
         if (!pBg->m_pcbCMXDataReq)
            return ENOMEMORY;
         CALLBACK_Init(pBg->m_pcbCMXDataReq, (PFNNOTIFY)CMediaPMDMMLayer_CMXDataReqNotify, (void *)pBg);

         pBg->m_pcbMediaData = MALLOC(sizeof(AEECallback));
         if (!pBg->m_pcbMediaData)
            return ENOMEMORY;
         CALLBACK_Init(pBg->m_pcbMediaData, (PFNNOTIFY)CMediaPMDMMLayer_ReadSource, (void *)pBg);

         pBg->m_dwSrcBufferSize = MIN(OEMMM_BUFFER_UNIT_SIZE, dwBufSize);
         pBg->m_pSrcBuffer = MALLOC(pBg->m_dwSrcBufferSize);
         if (!pBg->m_pSrcBuffer)
            return ENOMEMORY;

         FARF(SOURCE, ("===Src setup: Done"));
         pBg->m_bStreaming = TRUE;
      }

      ph->client.source_type = ph->source_type = CMX_SOURCE_CLIENT;
      ph->client.data_config = CMX_CLIENT_DATA_REAL_TIME;
      ph->client.data_len = 0xFFFFffff;   // Set it to max.
      ph->client.data_req_ptr = CMediaPMDMMLayer_CMXDataRequestCB;

      CMediaPMDMMLayer_ReadSource(pBg);

   }

   return nRet;
}


/*==================================================================
   Called in CMX context.
==================================================================*/
void CMediaPMDMMLayer_CMXDataRequestCB(
  uint32 position,
  uint32 max_len,
  cmx_client_req_cb_func_ptr_type data_req_cb,
  const void *server_client_data,
  const void *client_data
)
{
   CMXDataRequest req;
   CMediaPMDBg *  pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);

   FARF(SOURCE, ("===CMXCB: pos=%ul len=%ul", position, max_len));

   if (!pme)
   {
      data_req_cb(CMX_SUCCESS, NULL, 0, server_client_data);   
      return;
   }

   if (!max_len || !data_req_cb)
      return;

   // Save the request in queue...
   req.dwPos = position;
   req.dwMaxLen = max_len;
   req.pfnDataReq = data_req_cb;
   req.pServerUserData = (void *)server_client_data;
//   req.pClientUserData = (void *)client_data;

   if (CQueue_Put(pme->m_pqCMXDataReq, (uint8 *)&req, sizeof(req)))
      AEE_ResumeCallback(pme->m_pcbCMXDataReq, pme->m_pac);
}


//$r$
/*==================================================================
   CQueue
==================================================================*/
static int  CQueue_New(int32 nMaxSize, boolean bMutex, CQueue ** ppQ)
{
   CQueue * pme;
   uint8 *  pBuf = MALLOC(sizeof(CQueue) + nMaxSize);
   if (!pBuf)
      return ENOMEMORY;

   pme = (CQueue *)pBuf;
   pme->pBuffer = pBuf + sizeof(CQueue);
   pme->nMaxSize = nMaxSize;
   pme->bMutex = bMutex;

   *ppQ = pme;

   return SUCCESS;
}

static void CQueue_Delete(CQueue * pme)
{
   FREE(pme);
}

static int32 CQueue_Put(CQueue * pme, uint8 * pData, int32 nSize)
{
   int32 nRet;
   
   if (pme->bMutex)
      AEECriticalSection_Enter(&gMediaCMXCriticalSection);

   if (CQueue_IsPutOk(pme, nSize))
   {
      int32   nTillEnd, nWrap;

      if (pme->nWrite + nSize > pme->nMaxSize)
      {
         nTillEnd = pme->nMaxSize - pme->nWrite;
         nWrap = nSize - nTillEnd;
      }
      else
      {
         nTillEnd = nSize;
         nWrap = 0;
      }

      MEMCPY(pme->pBuffer + pme->nWrite, pData, nTillEnd);
      if (nWrap)
      {
         MEMCPY(pme->pBuffer, pData + nTillEnd, nWrap);
         pme->nWrite = nWrap;
      }
      else
      {
         pme->nWrite += nTillEnd;
         if (pme->nWrite >= pme->nMaxSize)
            pme->nWrite = 0;
      }

      pme->nFill += nSize;

      nRet = nSize;
   }
   else
      nRet = 0;

   if (pme->bMutex)
      AEECriticalSection_Leave(&gMediaCMXCriticalSection);

   FARF(QUEUE, ("===Put:F:%ul W:%ul R:%ul S:%ul===", pme->nFill, pme->nWrite, pme->nRead, nRet));
   return nRet;
}


static int32 CQueue_Get(CQueue * pme, uint8 * pData, int32 nSize)
{
   int32   nRet;

   if (pme->bMutex)
      AEECriticalSection_Enter(&gMediaCMXCriticalSection);

   if (pme->nFill)// && pme->nWrite != pme->m_nRead)
   {
      int32   nTillEnd, nWrap;
      int32   nActualSize = MIN(nSize, pme->nFill);

      if (pme->nRead + nActualSize > pme->nMaxSize)
      {
         nTillEnd = pme->nMaxSize - pme->nRead;
         nWrap = nActualSize - nTillEnd;
      }
      else
      {
         nTillEnd = nActualSize;
         nWrap = 0;
      }

      MEMCPY(pData, pme->pBuffer + pme->nRead, nTillEnd);
      if (nWrap)
      {
         MEMCPY(pData + nTillEnd, pme->pBuffer, nWrap);
         pme->nRead = nWrap;
      }
      else
      {
         pme->nRead += nTillEnd;
         if (pme->nRead >= pme->nMaxSize)
            pme->nRead = 0;
      }

      pme->nFill -= nActualSize;

      nRet = nActualSize;
   }
   else
      nRet = 0;

   if (pme->bMutex)
      AEECriticalSection_Leave(&gMediaCMXCriticalSection);

   FARF(QUEUE, ("===Get:F:%ul W:%ul R:%ul S:%ul===", pme->nFill, pme->nWrite, pme->nRead, nRet));
   return nRet;
}



/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_CMXDataReqNotify(CMediaPMDBg * pme)
{
   int            nRead;
   int            nLen;
   CMXDataRequest req;

   if (!pme)
      return;

   while (1)
   {
      if (CQueue_IsEmpty(pme->m_pqCMXDataReq))
         break;

      if (CQueue_IsEmpty(pme->m_pqMediaData))
      {
         FARF(SOURCE, ("===CMXNotify: No media data"));
         CMediaPMDMMLayer_RequestSourceData(pme);
         break;
      }

      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&req, sizeof(req)))
      {
         FARF(SOURCE, ("===CMXNotify: Fatal mdq error"));
         break;
      }

      CMediaPMDMMLayer_RequestSourceData(pme);

      nLen = MIN(pme->m_dwSrcBufferSize, req.dwMaxLen);
      nRead = CQueue_Get(pme->m_pqMediaData, pme->m_pSrcBuffer, nLen);

      // This check is not required as it is done when callback arrives.
      //   if (!req.pfnDataReq || !req.dwMaxLen)
      //      return;
      FARF(SOURCE, ("===CMXNotify: nReq=%ul nLen=%d", req.dwMaxLen, nLen));

      OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&req, (int32)pme->m_pSrcBuffer, (int32)nRead, FALSE);
//      req.pfnDataReq(CMX_SUCCESS, pme->m_pSrcBuffer, (uint32)nRead, req.pServerUserData);
   }
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_ReadSource(CMediaPMDBg * pme)
{
   ISource *   pSource = (ISource *)pme->m_md.pData;
   int32       nRead;
   boolean     bNotify = FALSE;

   while (1)
   {
      if (CQueue_IsFull(pme->m_pqMediaData) || !CQueue_IsPutOk(pme->m_pqMediaData, (int32)pme->m_dwSrcBufferSize))
         break;

      nRead = ISOURCE_Read(pSource, (char *)pme->m_pSrcBuffer, (int32)pme->m_dwSrcBufferSize);

      if (ISOURCE_WAIT == nRead)
      {
         ISOURCE_Readable(pSource, pme->m_pcbMediaData);
         break;
      }

      if (ISOURCE_END == nRead || ISOURCE_Exhausted(nRead))
      {
         FARF(SOURCE, ("===ReadSrc: EOS %d", nRead));
         pme->m_bEOS = TRUE;
         break;
      }

      if (!CQueue_Put(pme->m_pqMediaData, pme->m_pSrcBuffer, nRead))
      {
         FARF(SOURCE, ("===ReadSrc: Put error nRead=%l", nRead));
         break;
      }

      bNotify = TRUE;
   }

   if (bNotify)
   {
      FARF(SOURCE, ("===ReadSrc: Proc CMXRq"));
      CMediaPMDMMLayer_CMXDataReqNotify(pme);
   }
}


/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_RequestSourceData(CMediaPMDBg * pme)
{
   if (pme->m_bEOS || CALLBACK_IsQueued(pme->m_pcbMediaData))
      return;

   if (CQueue_Size(pme->m_pqMediaData) <= (CQueue_MaxSize(pme->m_pqMediaData) / 2))
   {
      FARF(SOURCE, ("===ReqSrc: MORE data"));
      AEE_ResumeCallback(pme->m_pcbMediaData, pme->m_pac);
   }
}


/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_FlushStreamData(CMediaPMDBg * pme)
{
   FARF(SOURCE, ("===Flushing stream data..."));

//   cmx_audio_playback_control(CMX_AUDIO_PB_CMD_FLUSH, NULL, NULL);

   // Cancel pending callbacks...
   CALLBACK_Cancel(pme->m_pcbMediaData);
   CALLBACK_Cancel(pme->m_pcbCMXDataReq);

   // Flush source data queue.
   CQueue_Flush(pme->m_pqMediaData);

   // Flush CMX buffers...
   while (!CQueue_IsEmpty(pme->m_pqCMXDataReq))
   {
      CMXDataRequest req;

      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&req, sizeof(req)))
         break;

      OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&req, (int32)NULL, (int32)0, FALSE);
//      req.pfnDataReq(CMX_SUCCESS, NULL, 0, req.pServerUserData);
   }

   if (pme->m_bEOS)
      pme->m_bEOS = FALSE;
}


/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_GetMMStatus(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, OEMMMStatus * peStatus)
{
   int   nStatus = MM_STATUS_UNKNOWN;

   if (!pme)
      return nStatus;

   if (peStatus)
      *peStatus = OEMMM_STATUS_PLAY;

   switch (status)
   {
      case CMX_SUCCESS:             
      case CMX_DONE:                
      case CMX_ABORTED:             
      case CMX_PAUSE:
      case CMX_REC_PAUSED:
      case CMX_RESUME:              
      case CMX_REC_RESUMED:              
      case CMX_ERROR:
      case CMX_FAILURE:             
      case CMX_ENABLED:
      case CMX_DISABLED:            
      case CMX_TICK_UPDATE:
      case CMX_REPEAT:              
      case CMX_DATA_ACCESS_DELAY:   
      case CMX_FLUSHED:
      case CMX_INVALID_DATA_SOURCE: 
      case CMX_REWIND:
      case CMX_FFORWARD:
         nStatus = CMediaPMDMMLayer_GetMMStatus2(pme, status, NULL, 0, peStatus);
         break;

#if defined(FEATURE_SVG)  // SVG inside PMD
      case CMX_SVG_INFO:
         if (psd)
         {
            nStatus = CMediaPMDMMLayer_GetMMStatus2(pme, status,
                                                 &psd->svg_info,
                                                 sizeof(cmx_svg_info_type),
                                                 peStatus);
         }
         break;

      case CMX_SVG_FRAME:
         if (psd)
         {
            nStatus = CMediaPMDMMLayer_GetMMStatus2(pme, status,
                                                 &psd->svg_frame,
                                                 sizeof(cmx_svg_frame_done_type),
                                                 peStatus);
         }
         break;
#endif // defined(FEATURE_SVG)

      case CMX_TEMPO: // fall thru...             
      case CMX_TUNE: // fall thru...                
      case CMX_PAN:                 
         nStatus = MM_STATUS_DONE;
         if (peStatus)
            *peStatus = OEMMM_STATUS_SETPARM;
         break;

      case CMX_AUDIO_SPEC:          
         nStatus = MM_STATUS_MEDIA_SPEC; 
         break;

#ifdef FEATURE_QCP
      case CMX_QCP_SPACE_WARNING:   
         nStatus = MM_STATUS_SPACE_WARNING;
         break;

      case CMX_QCP_SPACE_DONE:      
         nStatus = MM_STATUS_SPACE_ERROR;
         break;
#endif // FEATURE_QCP

#ifdef FEATURE_WEBAUDIO
   case CMX_TEXT_CTL:
	  if (!pme->m_bStopping && psd && ((CMediaPMDBg*)pme)->u.m_pPMD)
	  {
		 CMediaPMDBg* pmeBg = (CMediaPMDBg*)pme;
		 int nCallback;
		 ClientBufInfo* pClientBufInfo = NULL;
		 int nMaxCallback;


		 if (psd->text_ctl.buffer)
		 {
			// Find out which ClientBufInfo structure contains this buffer
			pClientBufInfo = GetClientBufInfo(pmeBg, (uint8*)psd->text_ctl.buffer);
			if (!pClientBufInfo)
			   break;

			nMaxCallback = pClientBufInfo->m_copycount;
		 }
		 else
		 {
			nMaxCallback = OEMMM_MAX_CB_CONTEXT_SWITCH;
		 }

		 nCallback = LockCSCallback(pmeBg->u.m_pPMD->m_cbPMDText, nMaxCallback, pmeBg);
		 if (nCallback == -1)
			break;

		 // Shallow copy the structure received
		 pmeBg->u.m_pPMD->m_textCtl[nCallback] = psd->text_ctl;

		 // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
		 if (psd->text_ctl.buffer != NULL)
		 {
			MEMCPY(pClientBufInfo->m_copies[nCallback], psd->text_ctl.buffer, pClientBufInfo->m_size);
			pmeBg->u.m_pPMD->m_textCtl[nCallback].buffer = pClientBufInfo->m_copies[nCallback];
		 }

		 pmeBg->u.m_pPMD->m_cbPMDText[nCallback].pPayload = &pmeBg->u.m_pPMD->m_textCtl[nCallback];
		 AEE_ResumeCallback(&pmeBg->u.m_pPMD->m_cbPMDText[nCallback].cb, pmeBg->m_pac);

		 if (pmeBg->u.m_pPMD->m_FrameCallbackEnabled)
		 {
			nStatus = MM_STATUS_FRAME;
		 }
	  }
	  break;

      case CMX_CLIENT_BUF_FREE:
      case CMX_CLIENT_BUF_REQ:
#if defined(FEATURE_BREW_CMX_V3)
      case CMX_GET_BUFFER:
#endif // defined(FEATURE_BREW_CMX_V3)
      case CMX_PICTURE_STATUS:       
      case CMX_PICTURE_DATA:
         if (!pme->m_bStopping)
            CMXImage_Process((CMediaPMDBg *)pme, status, psd);
         break;

#ifdef FEATURE_SAF
      case CMX_SAF_INFO:             
         if (!pme->m_bStopping)
            CMXImage_Process((CMediaPMDBg *)pme, status, psd);
         break;

      case CMX_SAF_FRAME:
         if (!pme->m_bStopping)
         {
			 CMediaPMDBg * pmeBg = (CMediaPMDBg *)pme;
			 nStatus = CMXImage_Process(pmeBg, status, psd);
         }
         break;
      
#endif // FEATURE_SAF

      case CMX_ANIM_CLEAR:
      case CMX_ANIM_ENABLE:
      case CMX_ANIM_DISABLE:
         if (!pme->m_bStopping)
            CMXImage_Process((CMediaPMDBg *)pme, status, psd);
         break;

#endif // FEATURE_WEBAUDIO
   }

   
   return nStatus;
}


/*==================================================================

   Callback Notification Functions: called in BREW context after
   AEE_ResumeCallback()

==================================================================*/
/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_SoundCB(void * pUser, AEESoundCmd eCBType, AEESoundStatus eStatus, uint32 dwParam)
{
   CMediaPMDBg *      pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pUser);
   AEESoundCmdData *    pData = (AEESoundCmdData *)dwParam;
   AEEMediaCallback     mcb;

   if (!pme)
      return;
   
   MEMCPY(&mcb, &pme->m_cbCommand, sizeof(AEEMediaCallback));

   mcb.bInUse = TRUE;
   mcb.cmdNotify.nSubCmd = MM_PARM_VOLUME;
   mcb.cmdNotify.nCmd = (eCBType == AEE_SOUND_VOLUME_CB) ? MM_CMD_GETMEDIAPARM : MM_CMD_SETMEDIAPARM;
   mcb.cmdNotify.nStatus = (eStatus == AEE_SOUND_SUCCESS) ? MM_STATUS_DONE : MM_STATUS_ABORT;
   mcb.cmdNotify.pCmdData = NULL;
   mcb.cmdNotify.dwSize = 0;

   if (pData && eCBType == AEE_SOUND_VOLUME_CB && eStatus == AEE_SOUND_SUCCESS)
   {
      mcb.cmdNotify.pCmdData = (void *)(uint32)pData->wVolume;
      mcb.cmdNotify.dwSize = sizeof(pData->wVolume);
   }

   AEEMedia_CallbackNotify((AEEMedia *)pme, &mcb);
}


/*==================================================================

==================================================================*/
static int OEMMediaPMD_Seek(OEMMediaPMD * pme, AEEMediaSeek eSeek, int32 lTimeMS)
{
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;

   if (eSeek != MM_SEEK_CURRENT)
      return EUNSUPPORTED;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_RINGER:
         if (lTimeMS < 0)
            cmx_audfmt_rewind(-1*lTimeMS, pfn, pUser);
         else
            cmx_audfmt_fforward(lTimeMS, pfn, pUser);
         break;

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#if defined(FEATURE_SVG)
      case OEMCMX_TYPE_SVG_DECODE:
         if (lTimeMS < 0)
            cmx_svg_rewind(pme->m_SVGHandle, -1*lTimeMS, pfn, pUser);
         else
            cmx_svg_fforward(pme->m_SVGHandle, lTimeMS, pfn, pUser);
         break;
#endif // defined(FEATURE SVG)

      default:
         return EFAILED;
   }

   return SUCCESS;
}



/*==================================================================

==================================================================*/
static int OEMMediaPMD_SetParm(OEMMediaPMD * pme, int16 nParmID, int32 p1, int32 p2, int32 p3, boolean bLocal)
{ 
   cmx_cb_func_ptr_type pfn;
   void *               pUser;
   int                  nErr = SUCCESS;

   // Process MIDI_DEVICE open/close which may not require object instance.
#if defined(FEATURE_MIDI_OUT)
   if (OEMCMX_ID_MIDI_DEVICE == nParmID)
   {
      boolean     bOpen = (boolean)p1;
      int         eADEC = (int)p2;
      CMXInfo *   pci = (CMXInfo *)p3;

      if (bLocal)
         return SUCCESS;

      pfn = pci ? (cmx_cb_func_ptr_type)pci->dwCB : NULL;
      pUser = pci ? (void *)pci->dwUserData : NULL;

      if (bOpen)
#if defined(FEATURE_BREW_CMX_V3)
         cmx_midi_out_open_adec_format((cmx_adec_mode_enum_type)eADEC, pfn, pUser);
#else
         cmx_midi_out_open(pfn, pUser);
#endif // defined(FEATURE_BREW_CMX_V3)
      else
         cmx_midi_out_close(pfn, pUser);

      return SUCCESS;
   }
#endif // defined(FEATURE_MIDI_OUT_QCP)

   if (!pme)
      return SUCCESS;

   pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   pUser = (void *)pme->m_ciStatus.dwUserData;

   switch (nParmID)
   {
      case OEMCMX_ID_TYPE:
         if (!bLocal)
            break;

         if (p1)
            MEMCPY(&pme->m_ciStart, (CMXInfo *)p1, sizeof(CMXInfo));

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType)
               pme->m_eType = (int16)p2;
         else if (OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
         {
            pme->m_eType = (int16)p2;
            pme->m_dwPauseMS = (uint32)p3;
         }
#ifdef FEATURE_QCP
         else if (OEMCMX_TYPE_QCP_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pQCPRecParm = (cmx_qcp_rec_para_type *)p2;
            pme->m_bReverse = (boolean)p3;
         }
#endif

#ifdef FEATURE_MULTISEQUENCER
         else if (OEMCMX_TYPE_AUDFMT_PLAY_MULTI == pme->m_ciStart.nType)
         {
            pme->m_pMultiParam = (cmx_audfmt_multi_param_type *)p2;
         }
#endif // FEATURE_MULTISEQUENCER         

#ifdef FEATURE_PCM_REC
         else if (OEMCMX_TYPE_PCM_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pPCMRecParm = (cmx_pcm_rec_para_type *)p2;
         }
#endif // FEATURE_PCM_REC
#if defined(FEATURE_CMX_RAW_PLAYBACK)
         else if (OEMCMX_TYPE_AUDFMT_PLAY_CODEC == pme->m_ciStart.nType)
         {
            pme->m_pCodecSpec = (cmx_codec_spec_type *)p2;
            pme->m_dwAudioSync = (uint32)p3;
         }
#endif // FEATURE_CMX_RAW_PLAYBACK

#ifdef FEATURE_MULTISEQUENCER
         else if (OEMCMX_TYPE_AUDFMT_PLAY_MULTI == pme->m_ciStart.nType)
         {
            pme->m_pMultiParam = (cmx_audfmt_multi_param_type *)p2;
         }
#endif // FEATURE_MULTISEQUENCER         

#ifdef FEATURE_PCM_REC
         else if (OEMCMX_TYPE_PCM_RECORD == pme->m_ciStart.nType)
         {
            pme->m_pPCMRecParm = (cmx_pcm_rec_para_type *)p2;
         }
#endif // FEATURE_PCM_REC

#if defined(FEATURE_SVG)
         else if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
         {
            pme->m_pSVGDecodeData = (cmx_svg_decode_data_type *)p2;
         }
#endif // defined(FEATURE_SVG)       

         else
            nErr = EBADPARM;
         break;

      case OEMCMX_ID_STATUS_CB:
         if (!bLocal)
            break;
         if (p1)
            MEMCPY(&pme->m_ciStatus, (CMXInfo *)p1, sizeof(CMXInfo));
         break;


      case OEMCMX_ID_TOTALTIME:
      {
         CMXInfo *   pci = (CMXInfo *)p1;

         if (bLocal)
            break;

         if (pci && OEMCMX_TYPE_AUDFMT_PLAY == pci->nType)
         {
            cmx_audfmt_get_time(pci->ph, 
                                (cmx_audfmt_time_cb_func_ptr_type)pci->dwCB, 
                                (void *)pci->dwUserData);
         }
         else
            nErr = EBADPARM;
         break;
      }

      case OEMCMX_ID_VOLUME:
         pme->m_wVolume = (uint16)p1;
         if (bLocal)
            break;

#if defined(FEATURE_MIDI_OUT_QCP)
         if (OEMCMX_TYPE_MIDI_OUT_QCP == pme->m_ciStart.nType)
            cmx_midi_out_qcp_volume((void *)pme->m_dwChannel, pme->m_wVolume, pfn, pUser);
         else
#endif // defined(FEATURE_MIDI_OUT_QCP)
            nErr = EUNSUPPORTED;
         break;
         
      case OEMCMX_ID_PAN:
         pme->m_wPan = (uint16)p1;
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
            cmx_audfmt_pan((uint32)pme->m_wPan, pfn, pUser);
#if defined(FEATURE_MIDI_OUT_QCP)
         else if (OEMCMX_TYPE_MIDI_OUT_QCP == pme->m_ciStart.nType)
            cmx_midi_out_qcp_pan((void *)pme->m_dwChannel, (sint15)pme->m_wPan, pfn, pUser);
#endif // defined(FEATURE_MIDI_OUT_QCP)
         else
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_TEMPO:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
            cmx_audfmt_tempo((uint32)p1, pfn, pUser);
         else
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_TUNE:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
            cmx_audfmt_tune((uint32)p1, pfn, pUser);
         else
            nErr = EUNSUPPORTED;
         break;

      case OEMCMX_ID_CHANNEL:
         if (!bLocal)
            break;

         pme->m_dwChannel = (uint32)p1;
         break;

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_ID_SEQUENCE:
         pme->m_pSequence = (void*)p1;
         pme->m_pSeqParam = (cmx_audfmt_seq_param_type *)p2;
         if (bLocal)
            break;
           
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
      case OEMCMX_ID_LINK:
         pme->m_Link = (cmx_pcm_rec_link_type)p1;
	 break;
#endif // FEATURE_PCM_REC
      case OEMCMX_ID_DATA_RESPONSE:
         if (bLocal)
            break;
         else
         {
            CMXDataRequest *  pReq = (CMXDataRequest *)p1;
            uint8 *           pBuffer = (uint8 *)p2;
            uint32            dwBufSize = (uint32)p3;

            pReq->pfnDataReq(CMX_SUCCESS, pBuffer, dwBufSize, pReq->pServerUserData);
         }
         
         break;

#if defined(FEATURE_SVG)
      case OEMCMX_ID_SVG_HANDLE:
         if (!bLocal)
            break;

         pme->m_SVGHandle = (void*)p1;
         break;

      case OEMCMX_ID_SVG_KEY:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_KEY_EVENT, (void*)p2, sizeof(cmx_svg_key_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_KEY_DOWN:
         if (bLocal)
            break;

#if (SVGT_VERSION > SVGT_1_1)
         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_KEY_DOWN_EVENT, (void*)p2, sizeof(cmx_svg_key_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
#else
            nErr = EUNSUPPORTED;
#endif // (SVGT_VERSION > SVGT_1_1)
         break;

      case OEMCMX_ID_SVG_KEY_UP:
         if (bLocal)
            break;

#if (SVGT_VERSION > SVGT_1_1)
         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_KEY_UP_EVENT, (void*)p2, sizeof(cmx_svg_key_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
#else
            nErr = EUNSUPPORTED;
#endif // (SVGT_VERSION > SVGT_1_1)
         break;

      case OEMCMX_ID_SVG_FOCUS:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
         {
#if (SVGT_VERSION > SVGT_1_1)
            if (CMX_SVG_FOCUS_EVENT_BEGIN == (cmx_svg_focus_event_enum_type)p2 ||
                CMX_SVG_FOCUS_EVENT_END == (cmx_svg_focus_event_enum_type)p2)
            {
               nErr = EUNSUPPORTED;
            }
            else
#endif // (SVGT_VERSION > SVGT_1_1)
            {
            cmx_svg_process_event((void*)p1, CMX_SVG_FOCUS_EVENT, (void*)&p2, sizeof(cmx_svg_focus_event_enum_type), (uint32)p3);
            }
         }
         else
         {
            nErr = EBADSTATE;
         }
         break;

      case OEMCMX_ID_SVG_POINTER_CLICK:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_POINTER_CLICK_EVENT, (void*)p2, sizeof(cmx_svg_pointer_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_POINTER_DOWN:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_POINTER_DOWN_EVENT, (void*)p2, sizeof(cmx_svg_pointer_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_POINTER_MOVE:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_POINTER_MOVE_EVENT, (void*)p2, sizeof(cmx_svg_pointer_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_POINTER_OVER:
         if (bLocal)
            break;

#if (SVGT_VERSION == SVGT_1_1)
         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_POINTER_OVER_EVENT, (void*)p2, sizeof(cmx_svg_pointer_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
#else
            nErr = EUNSUPPORTED;
#endif // (SVGT_VERSION == SVGT_1_1)
         break;

      case OEMCMX_ID_SVG_POINTER_UP:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_POINTER_UP_EVENT, (void*)p2, sizeof(cmx_svg_pointer_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_POINTER_LEAVE:
         if (bLocal)
            break;

#if (SVGT_VERSION == SVGT_1_1)
         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_POINTER_LEAVE_EVENT, (void*)p2, sizeof(cmx_svg_pointer_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
#else
            nErr = EUNSUPPORTED;
#endif // (SVGT_VERSION == SVGT_1_1)
         break;

      case OEMCMX_ID_SVG_ROTATE:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_ROTATE_EVENT, (void*)p2, sizeof(cmx_svg_rotate_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_PAN:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_PAN_EVENT, (void*)p2, sizeof(cmx_svg_pan_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_ZOOM:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_process_event((void*)p1, CMX_SVG_ZOOM_EVENT, (void*)p2, sizeof(cmx_svg_zoom_event_type), (uint32)p3);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_RESET_FRAME:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_reset_frame((void*)p1, pfn, pUser);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_GENERATE_FRAME:
         if (bLocal)
            break;

         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
            cmx_svg_frame_cmd((void*)p1, (uint32)p2, pfn, pUser);
         else
            nErr = EBADSTATE;
         break;

      case OEMCMX_ID_SVG_URLIMAGE_DATA:
         if (bLocal)
            break;

#if (SVGT_VERSION > SVGT_1_1)
         if (OEMCMX_TYPE_SVG_DECODE == pme->m_ciStart.nType)
         {
            SVGURLImageDataInfo* data = (SVGURLImageDataInfo*)p1;
            cmx_svg_req_cb(data->handle, &data->buffer, data->num_bytes);
         }
         else
            nErr = EBADSTATE;
#else
            nErr = EUNSUPPORTED;
#endif // (SVGT_VERSION > SVGT_1_1)
         break;

#endif // defined(FEATURE_SVG)

      default:
         nErr = EFAILED;
   }
   
   return nErr;
}

/*==================================================================

==================================================================*/
static int OEMMediaPMD_New(AEECLSID cls, OEMMediaPMD ** ppme) 
{ 

   if (!ppme)
      return EBADPARM;

   *ppme = (OEMMediaPMD *)MALLOC(sizeof(OEMMediaPMD));
   if (!*ppme)
      return ENOMEMORY;

   (*ppme)->m_clsSelf = cls;
   

   return SUCCESS; 
}

/*==================================================================

==================================================================*/
static int OEMMediaPMD_PauseResume(OEMMediaPMD * pme, boolean bPause)
{
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_RINGER:
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
         if (bPause)
            cmx_audfmt_pause(pfn, pUser);
         else
            cmx_audfmt_resume(pfn, pUser);
         break;

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#if defined(FEATURE_SVG)
      case OEMCMX_TYPE_SVG_DECODE:
         if (bPause)
            cmx_svg_pause(pme->m_SVGHandle, pfn, pUser);
         else
            cmx_svg_resume(pme->m_SVGHandle, pfn, pUser);
         break;
#endif // deined (FEATURE SVG)

      default:
         return EFAILED;
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int OEMMediaPMD_Stop(OEMMediaPMD * pme)
{
   cmx_cb_func_ptr_type pfn = (cmx_cb_func_ptr_type)pme->m_ciStatus.dwCB;
   void *               pUser = (void *)pme->m_ciStatus.dwUserData;


   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_RINGER: // Fall thru...
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
         cmx_audfmt_stop(pfn, pUser);
         break;

#ifdef FEATURE_QCP
      case OEMCMX_TYPE_QCP_RECORD:
         cmx_qcp_record_stop(pfn, pUser);
         break;
#endif

      case OEMCMX_TYPE_MIDI_OUT_MSG:
         // No effect.
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
         cmx_midi_out_qcp_stop((void *)pme->m_dwChannel, pfn, pUser);
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

#ifdef FEATURE_PCM_REC
      case OEMCMX_TYPE_PCM_RECORD:
         cmx_pcm_record_stop(pme->m_Link, pfn, pUser);
         break;
#endif // FEATURE_PCM_REC

#if defined(FEATURE_SVG)
      case OEMCMX_TYPE_SVG_DECODE:
         cmx_svg_abort(pme->m_SVGHandle, pfn, pUser);
         break;
#endif // defined(FEATURE SVG)

      default:
         return EFAILED;
   }

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int  OEMMediaPMD_Start(OEMMediaPMD * pme)
{
   void *      pUser = (void *)pme->m_ciStart.dwUserData;

   if (!pme->m_ciStart.ph)
      return EBADPARM;

   switch (pme->m_ciStart.nType)
   {
      case OEMCMX_TYPE_AUDFMT_PLAY:
         cmx_audfmt_play(pme->m_ciStart.ph, CMX_AF_CALLBACK_VALUE, (cmx_audfmt_start_enum_type)pme->m_eType, 
                         (cmx_audfmt_play_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                         pUser);
         break;

      case OEMCMX_TYPE_AUDFMT_RINGER:
         cmx_audfmt_ringer(pme->m_ciStart.ph, (uint16)pme->m_dwPauseMS, (cmx_audfmt_start_enum_type)pme->m_eType,
                           (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                           pUser);
         break;

#if defined(FEATURE_QCP)
      case OEMCMX_TYPE_QCP_RECORD:
         if (pme->m_bReverse)
         {
#if defined(FEATURE_BREW_CMX_V3)
            cmx_qcp_record_reverse(pme->m_ciStart.ph, pme->m_pQCPRecParm,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                   pUser);
#else
            cmx_qcp_record_reverse(pme->m_ciStart.ph, pme->m_pQCPRecParm->format, pme->m_pQCPRecParm->report_ms,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                   pUser);
#endif // defined(FEATURE_BREW_CMX_V3)
         }
         else
         {
#if defined(FEATURE_BREW_CMX_V3)
            cmx_qcp_record_forward(pme->m_ciStart.ph, pme->m_pQCPRecParm,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                   pUser);
#else
            cmx_qcp_record_forward(pme->m_ciStart.ph, pme->m_pQCPRecParm->format, pme->m_pQCPRecParm->report_ms,
                                   (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                   pUser);
#endif // defined(FEATURE_BREW_CMX_V3)
         }
         break;
#endif // defined(FEATURE_QCP)

#if defined(FEATURE_MIDI_OUT)
      case OEMCMX_TYPE_MIDI_OUT_MSG:
         cmx_midi_out_msg(pme->m_ciStart.ph,
                          (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                          pUser);
         break;

#if defined(FEATURE_MIDI_OUT_QCP)
      case OEMCMX_TYPE_MIDI_OUT_QCP:
         cmx_midi_out_qcp(pme->m_ciStart.ph, pme->m_wVolume, (sint15)(pme->m_wPan), 
                          (cmx_midi_out_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                          pUser);
         break;
#endif // defined(FEATURE_MIDI_OUT_QCP)
#endif // defined(FEATURE_MIDI_OUT)

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_play_multi(pme->m_ciStart.ph, pme->m_pMultiParam, 
                               (cmx_audfmt_play_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                               pUser);
         break;
#endif //FEATURE MULTISEQUENCER

#if defined(FEATURE_PCM_REC)
      case OEMCMX_TYPE_PCM_RECORD:
            cmx_pcm_record(pme->m_ciStart.ph, pme->m_pPCMRecParm,
                           (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                            pUser);
         break;
#endif // defined(FEATURE_PCM_REC)
#if defined(FEATURE_CMX_RAW_PLAYBACK)
      case OEMCMX_TYPE_AUDFMT_PLAY_CODEC:
      {
         if (!pme->m_dwAudioSync)
         {
            cmx_audfmt_play_codec(pme->m_ciStart.ph, pme->m_pCodecSpec, 
                                 (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                 pUser);
         }
         else
         {
#if defined(FEATURE_CMX_AV_SYNC)
            cmx_audfmt_play_codec_av_sync(pme->m_ciStart.ph, pme->m_pCodecSpec, 
                                          (cmx_av_sync_type *)pme->m_dwAudioSync,
                                          (cmx_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                                          pUser);
#else // defined(FEATURE_CMX_AV_SYNC)
            return EFAILED;
#endif // defined(FEATURE_CMX_AV_SYNC)

         }

      }
                        
      break;
#endif //defined(FEATURE_CMX_RAW_PLAYBACK)

#if defined(FEATURE_SVG)
      case OEMCMX_TYPE_SVG_DECODE:
         cmx_svg_decode(pme->m_ciStart.ph, pme->m_pSVGDecodeData, 
                        (cmx_image_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                        pUser);
         break;
#endif // defined(FEATURE SVG)

      default:
         return EFAILED;
   }

   return SUCCESS; 
}
/*==================================================================

==================================================================*/
static void OEMMediaPMD_Delete(OEMMediaPMD * pme)
{ 
   FREE(pme);
}


/*===============================================================================

                     CMediaPMDBg PUBLIC FUNCTION DEFINITIONS

===============================================================================*/
/*==================================================================
   Called only once during AEE initialization
==================================================================*/
void IMediaPMD_Init(IShell * ps)
{
   if (gInitFlags.gbMediaBgInit)
      return;

   gInitFlags.gbMediaBgInit = TRUE;

   // If found in the mod table, then register the formats...
   //if (CMediaFormat_InModTable(AEECLSID_MEDIAPMD, (void *)IMediaPMD_Init, NULL))
   {
      AEEMedia_Init(ps, MT_VIDEO_PMD, AEECLSID_MEDIAPMD);
      AEEMedia_Init(ps, "video/mld", AEECLSID_MEDIAPMD);
   }

#if defined(FEATURE_SVG)
   //if (CMediaFormat_InModTable(AEECLSID_MEDIASVG, (void *)IMediaPMD_Init, NULL))
   {
      AEEMedia_Init(ps, MT_VIDEO_SVG, AEECLSID_MEDIASVG);
      AEEMedia_Init(ps, MT_VIDEO_SVGZ, AEECLSID_MEDIASVG);
#if defined(FEATURE_SVGDOM)
      AEEMedia_Init(ps, MT_SVGDOM, AEECLSID_MEDIASVG);
#endif // defined(FEATURE_SVGDOM)
   }
#endif // defined(FEATURE_SVG)

//   if (CMediaFormat_InModTable(AEECLSID_MEDIADLS, (void *)IMediaPMD_Init, (void *)NULL))
//         AEEMedia_Init(ps, MT_AUDIO_DLS, AEECLSID_MEDIADLS);
   // Initialize ASIC MM Layer here...
   CMediaPMDMMLayer_Init(ps);
}

/*==================================================================
   Called by ISHELL_CreateInstance()
==================================================================*/
int IMediaPMD_New(IShell * ps, AEECLSID cls, void **ppif)
{
   int         nRet;
   CMediaPMDBg *  pme;

   // Create the object
   if (ppif)
   {
      pme = *ppif = CMediaCMX_New(ps, cls, &gMediaBgFuncs, sizeof(CMediaPMDBg), (PFNNOTIFY)CMediaPMDBg_Delete);
      nRet = pme ? SUCCESS : ENOMEMORY;
   }
   else {
      nRet = EBADPARM;
      pme = 0;
   }

   if (nRet == SUCCESS)
   {
      AEESoundMethod eMethod;
      AEESoundInfo   si;
      // Create ISound, register callback notify, and set method based on media.
      pme->m_pISound = NULL;
      if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SOUND, (void **)&pme->m_pISound))
         goto Done;
      ISOUND_RegisterNotify(pme->m_pISound, CMediaPMDMMLayer_SoundCB, (void *)pme->m_hObject);
      if (AEECLSID_MEDIAQCP == cls || AEECLSID_MEDIAAMR == cls)
         eMethod = AEE_SOUND_METHOD_VOICE;
      else
         eMethod = AEE_SOUND_METHOD_MIDI;
      ISOUND_Get(pme->m_pISound, &si);
      si.eMethod = eMethod;
      ISOUND_Set(pme->m_pISound, &si);

      switch (cls)
      {
         case AEECLSID_MEDIAPMD:
#if defined(FEATURE_SVG)
         case AEECLSID_MEDIASVG:
#endif // defined(FEATURE_SVG)
         {
#if defined(FEATURE_WEBAUDIO)

            AEEDeviceInfo  di;

            pme->u.m_pPMD = MALLOC(sizeof(CMediaPMD));
            if (!pme->u.m_pPMD)
               goto Done;
          
            ISHELL_GetDeviceInfo(pme->m_pIShell, &di);
            pme->u.m_pPMD->m_rectImage.dx = di.cxScreen;
            pme->u.m_pPMD->m_rectImage.dy = di.cyScreen;
            pme->u.m_pPMD->m_rectText.dx = di.cxScreen;
            pme->u.m_pPMD->m_rectText.dy = di.cyScreen;


            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH, CMXImage_Notify);
            CALLBACK_Init(&pme->u.m_pPMD->m_cbJPEGBufAlloc, AllocJPEGBuf, pme);
            CALLBACK_Init(&pme->u.m_pPMD->m_cbJPEGBufFree, FreeJPEGBuf, pme);
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH, AllocClientBuf);
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH, FreeClientBuf);
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH, PICInitialize);
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbPicRelease, OEMMM_MAX_CB_CONTEXT_SWITCH, PICRelease);
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbTextRelease, OEMMM_MAX_CB_CONTEXT_SWITCH, TextRelease);               
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbPMDText, OEMMM_MAX_CB_CONTEXT_SWITCH, ProcessPMDText);               
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbCopyBMPBuf, OEMMM_MAX_CB_CONTEXT_SWITCH, BMPCopyBuffer);               
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbCopyPNGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH, PNGCopyBuffer);               
#ifdef FEATURE_AUDIO_POSTCARD
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbCopyJPEGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH, JPEGCopyBuffer);               
#endif
#if defined(FEATURE_SAF)
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, SAFCreateDIB);               
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbSAFLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH, SAFLoadFrameToDIB);               
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG)
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, SVGCreateDIB);               
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbSVGLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH, SVGLoadFrameToDIB);               
#endif //defined(FEATURE_SVG)
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH, CMXChangeDrawStatusFlag);
            InitCSCallback((IMedia *)pme, pme->u.m_pPMD->m_cbAnimReset, OEMMM_MAX_CB_CONTEXT_SWITCH, AnimReset);

            pme->u.m_pPMD->m_numToWipe = 0;
            pme->u.m_pPMD->m_textBlock = NULL;
            MEMSET(pme->u.m_pPMD->m_textCtl, 0, sizeof(cmx_af_text_ctl_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
            MEMSET(pme->u.m_pPMD->m_picData, 0, sizeof(cmx_af_pic_data_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
            MEMSET(pme->u.m_pPMD->m_picStatus, 0, sizeof(cmx_af_picture_status_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
#if defined(FEATURE_SAF)
            MEMSET(pme->u.m_pPMD->m_safInfo, 0, sizeof(cmx_saf_info_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
            MEMSET(pme->u.m_pPMD->m_safData, 0, sizeof(cmx_saf_data_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);            
#endif  //defined(FEATURE_SAF)
            MEMSET(pme->u.m_pPMD->m_drawInfo, 0, sizeof(DrawInfo) * OEMMM_MAX_CB_CONTEXT_SWITCH);

            pme->u.m_pPMD->m_pPNGBuf = NULL;
            pme->u.m_pPMD->m_pPNGBufSize = 0;
            pme->u.m_pPMD->m_pJPEGBuf = NULL;
            pme->u.m_pPMD->m_pJPEGBufSize = 0;
            pme->u.m_pPMD->m_cbJPEGBufAllocNotify = NULL;
            pme->u.m_pPMD->m_pJPEGClientData = NULL;
            pme->u.m_pPMD->m_fTextDraw = OEMMM_DISP_DRAW;
            pme->u.m_pPMD->m_pIAnimBitmap = NULL;
            pme->u.m_pPMD->m_pIPicBitmap = NULL;
            pme->u.m_pPMD->m_pIDispBitmap = NULL;
            pme->u.m_pPMD->m_displayedPic = FALSE;
            pme->u.m_pPMD->m_displayedSaf = FALSE;
            pme->u.m_pPMD->m_transparency = FALSE;
#if defined(FEATURE_SVG)
            pme->u.m_pPMD->m_svgDecoderCalled = FALSE;
#endif // defined(FEATURE_SVG)

            pme->u.m_pPMD->m_fPicDraw = OEMMM_DISP_CLEAR | OEMMM_DISP_DRAW;
            pme->u.m_pPMD->m_fAnimDraw = OEMMM_DISP_CLEAR | OEMMM_DISP_DRAW;

            if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_DISPLAY, (void **) &pme->u.m_pPMD->m_pIDisp))
            {
               FREEIF(pme->u.m_pPMD);
               goto Done;
            }
            
            pme->u.m_pPMD->m_pClientBufInfos = MALLOC(sizeof(ClientBufInfo) * OEMMM_CLIENT_BUF_INIT_COUNT);
            if (pme->u.m_pPMD->m_pClientBufInfos)
            {
               int nBuf;
               for (nBuf = 0; nBuf < OEMMM_CLIENT_BUF_INIT_COUNT; ++nBuf)
               {
                  int nCopy;
                  pme->u.m_pPMD->m_pClientBufInfos[nBuf].m_buf = NULL;
                  pme->u.m_pPMD->m_pClientBufInfos[nBuf].m_size = 0;
                  pme->u.m_pPMD->m_pClientBufInfos[nBuf].m_copycount = 0;
                  for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
                  {
                     pme->u.m_pPMD->m_pClientBufInfos[nBuf].m_copies[nCopy] = NULL;
                  }
               }
            }
            else            
            {
               MM_RELEASEIF(pme->u.m_pPMD->m_pIDisp);
               FREEIF(pme->u.m_pPMD);
               goto Done;
            }
            pme->u.m_pPMD->m_numClientBufs = OEMMM_CLIENT_BUF_INIT_COUNT;
            pme->u.m_pPMD->m_lastBufAllocated = -1;
            MEMSET (pme->u.m_pPMD->m_ClientBufSent, 0, sizeof(uint8*) * OEMMM_MAX_CB_CONTEXT_SWITCH);
            MEMSET (pme->u.m_pPMD->m_ClientBufRequest, 0, sizeof(ClientBufRequestInfo) * OEMMM_MAX_CB_CONTEXT_SWITCH);

#if defined(FEATURE_SVG)
            pme->u.m_pPMD->m_SVGDecodeData.dpi = OEMMM_DEFAULT_DPI;
            pme->u.m_pPMD->m_SVGDecodeData.directory[0] = 0;
            pme->u.m_pPMD->m_SVGDecodeData.cmx_svg_decode_mode = CMX_SVG_MODE_CMX;
            pme->u.m_pPMD->m_SVGDecodeData.delay = 0;
            pme->u.m_pPMD->m_SVGDecodeData.transparency = FALSE;
            pme->u.m_pPMD->m_SVGDecodeData.bk_color = 0xFFFFFFFF;  //White - BGR0
            pme->u.m_pPMD->m_SVGDecodeData.empty = FALSE;
            pme->u.m_pPMD->m_SVGDecodeData.color_scheme = CMX_COLOR_SCHEME_NONE;
            pme->u.m_pPMD->m_SVGUserPath[0] = 0;

#if (SVGT_VERSION > SVGT_1_1)
            pme->u.m_pPMD->m_SVGDecodeData.progressive = FALSE;
            pme->u.m_pPMD->m_SVGDecodeData.auto_stop = TRUE;
            pme->u.m_pPMD->m_SVGURLImageHandled = FALSE;
#endif // (SVGT_VERSION > SVGT_1_1)

#if defined(FEATURE_SVGDOM)
            pme->u.m_pPMD->pDocument = NULL;
#endif //defined(FEATURE_SVGDOM)
#endif //defined(FEATURE_SVG)

            pme->u.m_pPMD->m_FrameCallbackEnabled = FALSE;
            pme->u.m_pPMD->m_pCallerFrameBmp = NULL;
            pme->u.m_pPMD->m_DispAreaChanged = FALSE;

            break;
#else
			nRet = EUNSUPPORTED;
			goto Done;
#endif // defined(FEATURE_WEBAUDIO)
         } // case
      } // switch

      pme->m_wTempo = 100;
      pme->m_wTune = 0x40;
      pme->m_wSilenceTimeMS = OEMMM_RINGER_SILENCE_TIME; // one second wait between rings.
      pme->m_dwTickTimeMS = OEMMM_TICK_PERIOD;

      // Initialize callbacks.
      CMediaPMDMMLayer_InitCallback((IMedia *)pme, MM_CMD_PLAY, pme->m_cbPlay, OEMMM_MAX_CB_PLAY, CMediaPMDBg_CallbackNotify);
      CMediaPMDMMLayer_InitCallback((IMedia *)pme, 0, &pme->m_cbCommand, 1, CMediaPMDBg_CallbackNotify);
      CMediaPMDMMLayer_InitCallback((IMedia *)pme, MM_CMD_GETTOTALTIME, &pme->m_cbGetTotalTime, 1, CMediaPMDBg_CallbackNotify);

      {
         CMXInfo   ci;

         
         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
         ci.ph = NULL;
         ci.dwCB = (uint32)CMediaPMDMMLayer_StatusCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_STATUS_CB, (int32)&ci, 0, 0, TRUE);
      }

   } // if

   return nRet;

Done:
   // Free anything that could have been successfully allocated before "goto Done"
   if (pme->m_pISound)
   {
      ISOUND_RegisterNotify(pme->m_pISound, NULL, (void *)pme->m_hObject);
      ISOUND_Release(pme->m_pISound);
   }

   FREE(pme);
   return ENOMEMORY;
}

#if defined(FEATURE_WEBAUDIO) 
static int IDShift_New(IShell *piShell, void **pp)
{
   IDShift *pMe;
   *pp = 0;

   
   pMe = MALLOCREC(IDShift);

   if ((IDShift *)0 == pMe) {
      return ENOMEMORY;
   }

   *pp = (void *)pMe;

   pMe->piShell = piShell;
   ISHELL_AddRef(piShell);

   return SUCCESS;
}

static void IDShift_Delete(IDShift *pMe)
{

   ISHELL_Release(pMe->piShell);

   if(pMe->pDitherBuf)
      FREE(pMe->pDitherBuf);

   FREE(pMe);
}

static int    IDShift_Configure(IDShift *pMe, AEEDShift *pShiftInfo) {

   if(!pShiftInfo || pShiftInfo->nSrcDepth != 24)
      return EBADPARM;


   switch(pShiftInfo->nDstDepth) {
      case 1:
         pMe->pfnDShift = (PFNDSHIFT) DShiftTo1;
         break;
      case 4:
         pMe->pfnDShift = (PFNDSHIFT) DShiftTo4;
         break;
      case 8:
         pMe->pfnDShift = (PFNDSHIFT) DShiftTo8;
         break;
      case 16:
         pMe->pfnDShift = (PFNDSHIFT) DShiftTo16;
         break;

      default:
         return EBADPARM;
   }
   if(pShiftInfo->bDither) {
      pMe->pDitherBuf = (DitherCoef *) MALLOC(pShiftInfo->nSrcPitch);
      if(!pMe->pDitherBuf) {
         pMe->pfnDShift = NULL;
         return EFAILED;
      }
   }

   MEMCPY((void *) &pMe->dshiftInfo, (void *) pShiftInfo, sizeof(AEEDShift));

   return SUCCESS;
}

static int    IDShift_Convert(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap) {

   if(pMe->pfnDShift) {
      if(pMe->dshiftInfo.bDither)
         pSrc = DitherSrc(pMe, pSrc);
      return(pMe->pfnDShift(pMe, pDst, pSrc, bSwap));
   }
   else
      return EFAILED;
}

static int    DShiftTo1(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap) {


   return SUCCESS;
}

static int    DShiftTo4(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap) {


   return SUCCESS;
}

static int    DShiftTo8(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap) {

   int i;

   if(bSwap) {
      for(i = 0; i < pMe->dshiftInfo.nWidth; i++) {
         *pDst  = ((*pSrc++) & 0xC0) >> 6;
         *pDst |= ((*pSrc++) & 0xE0) >> 3;
         *pDst |= (*pSrc++) & 0xE0;
         pDst++;
      }

   }
   else {
      for(i = 0; i < pMe->dshiftInfo.nWidth; i++) {
         *pDst  = (*pSrc++) & 0xE0;
         *pDst |= ((*pSrc++) & 0xE0) >> 3;
         *pDst |= ((*pSrc++) & 0xC0) >> 6;
         pDst++;
      }
   }

   return SUCCESS;
}

static int    DShiftTo16(IDShift *pMe, byte *pDst, byte *pSrc, boolean bSwap) {

   int i;
   uint16 *pDst16 = (uint16 *) pDst;

   if(!bSwap) {
      for(i = 0; i < pMe->dshiftInfo.nWidth; i++) {
         *pDst16  = ((uint16)((*pSrc++) & 0xF8)) << 8;
         *pDst16 |= ((uint16)((*pSrc++) & 0xFC)) << 3;
         *pDst16 |= ((*pSrc++) & 0xF8) >> 3;
         pDst16++;
      }
   }
   else {
      for(i = 0; i < pMe->dshiftInfo.nWidth; i++) {
         *pDst16  = ((*pSrc++) & 0xF8) >> 3;
         *pDst16 |= ((uint16)((*pSrc++) & 0xFC)) << 3;
         *pDst16 |= ((uint16)((*pSrc++) & 0xF8)) << 8;
         pDst16++;
      }
   }

   return SUCCESS;
}

static byte * DitherSrc(IDShift * pMe, byte *pSrc) {

   uint32 i;
   
   byte * pDitherSrc = pSrc;
   int   compVal, errSrc;
   int brRed = 0;
   int brGreen = 0;
   int brBlue = 0;
   uint32 nPix = pMe->dshiftInfo.nSrcPitch;

   
   if(!pMe->pDitherBuf)
      pMe->pDitherBuf = (DitherCoef *) MALLOC(pMe->dshiftInfo.nWidth * sizeof(DitherCoef));

   if(pMe->pDitherBuf) {
      for(i = 0; i < nPix; i++) {
         //Red First
         errSrc = *pDitherSrc + pMe->pDitherBuf[i].coRed;

         compVal = DITHER_RGQUANT(errSrc);
         *pDitherSrc = MIN(compVal, 255);

         if(brRed != 0)
            pMe->pDitherBuf[i].coRed = brRed;
         
         pMe->pDitherBuf[i].coRed += BOCOMP(errSrc, compVal);


         if( i < (nPix - 1)) {
            pMe->pDitherBuf[i+1].coRed += RICOMP(errSrc, compVal);
            brRed = BRCOMP(errSrc, compVal);
         }
         if (i > 0)
            pMe->pDitherBuf[i-1].coRed += BLCOMP(errSrc, compVal);

         //Then Green
         pDitherSrc++;
         errSrc = *pDitherSrc + pMe->pDitherBuf[i].coGreen;
         
         compVal = DITHER_RGQUANT(errSrc);

         *pDitherSrc = MIN(compVal, 255);

         if(brGreen != 0)
            pMe->pDitherBuf[i].coGreen = brGreen;
         
         pMe->pDitherBuf[i].coGreen += BOCOMP(errSrc, compVal);


         if( i < (nPix - 1)) {
            pMe->pDitherBuf[i+1].coGreen += RICOMP(errSrc, compVal);
            brGreen = BRCOMP(errSrc, compVal);
         }
         if (i > 0)
            pMe->pDitherBuf[i-1].coGreen += BLCOMP(errSrc, compVal);

         //Then blue
         pDitherSrc++;
         errSrc = *pDitherSrc + pMe->pDitherBuf[i].coBlue;

         compVal = DITHER_BQUANT(errSrc);
         *pDitherSrc = MIN(compVal, 255);

         if(brBlue != 0)
            pMe->pDitherBuf[i].coBlue = brBlue;
         
         pMe->pDitherBuf[i].coBlue += BOCOMP(errSrc, compVal);

         if( i < (nPix - 1)) {
            pMe->pDitherBuf[i+1].coBlue += RICOMP(errSrc, compVal);
            brBlue = BRCOMP(errSrc, compVal);
         }
         if (i > 0)
            pMe->pDitherBuf[i-1].coBlue += BLCOMP(errSrc, compVal);
         
         pDitherSrc++;
      }
 
   }

   return pSrc;
}

/*==================================================================
Initializes a Context Switch callback structure
==================================================================*/
static void InitCSCallback(IMedia * po, ContextSwitchCallback * pcb, int nMax, PFNNOTIFY pfn)
{
   int         i;

   for (i = 0; i < nMax; i++)
   {
      pcb[i].bInUse = FALSE;
      CALLBACK_Init(&pcb[i].cb, pfn, &pcb[i]);
      pcb[i].pIMedia = po;
      pcb[i].pPayload = NULL;
   }
}

/*==================================================================
Returns the index of the first unused Context Switch callback or -1
if none available
==================================================================*/
static int LockCSCallback(ContextSwitchCallback * pcb, int nMax, CMediaPMDBg *pme)
{
   int   i;

   for (i = 0; i < nMax; i++)
   {
      if (!pcb[i].bInUse)
      {
         pcb[i].bInUse = TRUE;
         pme->u.m_pPMD->m_nCallbacksSkipped = 0;
         return i;
      }
   }

   ++pme->u.m_pPMD->m_nCallbacksSkipped;
   return -1;
}

/*==================================================================

==================================================================*/
static void UnlockCSCallback(ContextSwitchCallback * pcb)
{
   pcb->bInUse = FALSE;
   pcb->pPayload = NULL;
}

static void CancelCSCallback(ContextSwitchCallback * pcb, int nMax)
{
   int   i;

   for (i = 0; i < nMax; i++)
   {
      CALLBACK_Cancel(&pcb[i].cb);
   }
}
#endif //defined(FEATURE_WEBAUDIO)

#if defined(FEATURE_SVG)
/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVGInput(CMediaPMDBg * pme, uint32 * pFrameTime)
{
   AEEMediaCallback *   pcb;

   if (!pme)
      return ENOMEMORY;

   if (pme->m_clsSelf != AEECLSID_MEDIASVG)
      return EFAILED;

   if (!pme->u.m_pPMD)
      return ENOMEMORY;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_SVG_INPUT);
   if (!pcb)
      return EITEMBUSY;

   // Determine the time since the last frame
   if (pme->u.m_pPMD->m_bNoFramesShown)
      *pFrameTime = 0;
   else
      *pFrameTime = GETUPTIMEMS() - pme->u.m_pPMD->m_nUpTimeMS;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_KEY(CMediaPMDBg * pme, SVGKeyData * pKeyData, int32 nEvent)
{
   uint32 nFrameTime;
   int nRet;
   int16 nParmID = 0;

#if (SVGT_VERSION > SVGT_1_1)
   if (MM_SVG_KEY_MOD_NONE != pKeyData->nModifier || MM_SVG_VK_UNDEFINED != pKeyData->nVirtualKey)
   {
      return EUNSUPPORTED;
   }
#endif // (SVGT_VERSION > SVGT_1_1)

   nRet = CMediaPMDMMLayer_SetSVGInput(pme, &nFrameTime);
   if (nRet != SUCCESS)
      return nRet;

   pme->u.m_pPMD->m_key_event.key         = pKeyData->nKey;
#if (SVGT_VERSION == SVGT_1_1)
   pme->u.m_pPMD->m_key_event.virtual_key = pKeyData->nVirtualKey;
   pme->u.m_pPMD->m_key_event.modifier    = pKeyData->nModifier;
#endif // (SVGT_VERSION == SVGT_1_1)

   switch (nEvent)
   {
      case MM_SVG_EVENT_KEY:
         nParmID = OEMCMX_ID_SVG_KEY;
         break;

#if (SVGT_VERSION > SVGT_1_1)
      case MM_SVG_EVENT_KEY_DOWN:
         nParmID = OEMCMX_ID_SVG_KEY_DOWN;
         break;
      case MM_SVG_EVENT_KEY_UP:
         nParmID = OEMCMX_ID_SVG_KEY_UP;
         break;
#endif // (SVGT_VERSION > SVGT_1_1)

      default:
         return EBADPARM;
   }

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, nParmID, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)&pme->u.m_pPMD->m_key_event, (int32)nFrameTime, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_FOCUS(CMediaPMDBg * pme, int16 nFocusEvent)
{
   uint32 nFrameTime;
   int nRet;
   int32 nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_NONE;

   // Backwards-incompatible changes to focus event list in CMX.h
   // make the following conversion necessary.
   switch (nFocusEvent)
   {
#if (SVGT_VERSION == SVGT_1_1)
      case MM_SVG_FOCUS_EVENT_BEGIN:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_BEGIN;
         break;
      case MM_SVG_FOCUS_EVENT_END:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_END;
         break;
#endif // (SVGT_VERSION == SVGT_1_1)

      case MM_SVG_FOCUS_EVENT_ACTIVATE:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_ACTIVATE;
         break;
      case MM_SVG_FOCUS_EVENT_NEXT:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_NEXT;
         break;
      case MM_SVG_FOCUS_EVENT_PREVIOUS:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_PREVIOUS;
         break;

#if (SVGT_VERSION > SVGT_1_1)
      case MM_SVG_FOCUS_EVENT_NORTH:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_NORTH;
         break;
      case MM_SVG_FOCUS_EVENT_NORTH_EAST:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_NORTH_EAST;
         break;
      case MM_SVG_FOCUS_EVENT_EAST:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_EAST;
         break;
      case MM_SVG_FOCUS_EVENT_SOUTH_EAST:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_SOUTH_EAST;
         break;
      case MM_SVG_FOCUS_EVENT_SOUTH:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_SOUTH;
         break;
      case MM_SVG_FOCUS_EVENT_SOUTH_WEST:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_SOUTH_WEST;
         break;
      case MM_SVG_FOCUS_EVENT_WEST:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_WEST;
         break;
      case MM_SVG_FOCUS_EVENT_NORTH_WEST:
         nCMXFocusEvent = CMX_SVG_FOCUS_EVENT_NORTH_WEST;
         break;
#endif // (SVGT_VERSION > SVGT_1_1)
      default:
         return EUNSUPPORTED;
         break;
   }

   nRet = CMediaPMDMMLayer_SetSVGInput(pme, &nFrameTime);
   if (nRet != SUCCESS)
      return nRet;

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_FOCUS, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)nFocusEvent, (int32)nFrameTime, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_POINTER(CMediaPMDBg * pme, SVGPointerData * pPointerData, int16 commandID)
{
   uint32 nFrameTime;
   int nRet;

#if (SVGT_VERSION > SVGT_1_1)
   if (MM_SVG_KEY_MOD_NONE != pPointerData->nKeyModifier)
   {
      return EUNSUPPORTED;
   }
#endif // (SVGT_VERSION > SVGT_1_1)

   nRet = CMediaPMDMMLayer_SetSVGInput(pme, &nFrameTime);
   if (nRet != SUCCESS)
      return nRet;

   pme->u.m_pPMD->m_pointer_event.x            = pPointerData->x;
   pme->u.m_pPMD->m_pointer_event.y            = pPointerData->y;
#if (SVGT_VERSION == SVGT_1_1)
   pme->u.m_pPMD->m_pointer_event.key_modifier = pPointerData->nKeyModifier;
#endif // (SVGT_VERSION == SVGT_1_1)

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, commandID, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)&pme->u.m_pPMD->m_pointer_event, (int32)nFrameTime, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_ROTATE(CMediaPMDBg * pme, SVGRotateData * pRotateData)
{
   uint32 nFrameTime;
   int nRet;

   nRet = CMediaPMDMMLayer_SetSVGInput(pme, &nFrameTime);
   if (nRet != SUCCESS)
      return nRet;

   MEMCPY(&pme->u.m_pPMD->m_rotate_event, pRotateData, sizeof(cmx_svg_rotate_event_type));  // All members are the same

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_ROTATE, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)&pme->u.m_pPMD->m_rotate_event, (int32)nFrameTime, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_PAN(CMediaPMDBg * pme, SVGPanData * pPanData)
{
   uint32 nFrameTime;
   int nRet;

   nRet = CMediaPMDMMLayer_SetSVGInput(pme, &nFrameTime);
   if (nRet != SUCCESS)
      return nRet;

   pme->u.m_pPMD->m_pan_event.x           = pPanData->x;
   pme->u.m_pPMD->m_pan_event.y           = pPanData->y;
   pme->u.m_pPMD->m_pan_event.fast_scroll = (pPanData->flgScrollMode == MM_SVG_SCROLL_MODE_FAST);

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_PAN, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)&pme->u.m_pPMD->m_pan_event, (int32)nFrameTime, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_ZOOM(CMediaPMDBg * pme, SVGZoomData * pZoomData)
{
   uint32 nFrameTime;
   int nRet;

   nRet = CMediaPMDMMLayer_SetSVGInput(pme, &nFrameTime);
   if (nRet != SUCCESS)
      return nRet;

   pme->u.m_pPMD->m_zoom_event.x           = pZoomData->x;
   pme->u.m_pPMD->m_zoom_event.y           = pZoomData->y;
   pme->u.m_pPMD->m_zoom_event.zoom_factor = pZoomData->nZoomFactor;
   pme->u.m_pPMD->m_zoom_event.centered    = (pZoomData->flgCentered == MM_SVG_CENTER_MODE_CENTERED);

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_ZOOM, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, (int32)&pme->u.m_pPMD->m_zoom_event, (int32)nFrameTime, FALSE);

   return MM_PENDING;
}

/*==================================================================

==================================================================*/
static int CMediaPMDMMLayer_SetSVG_EVENT_RESET_FRAME(CMediaPMDBg * pme)
{
   // No callback is allocated for this call

   OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_RESET_FRAME, (int32)pme->u.m_pPMD->m_curSVGInfo.handle, 0, 0, FALSE);

   return SUCCESS;
}

#if (SVGT_VERSION > SVGT_1_1)
/*==================================================================

==================================================================*/
static int CMediaMMLayer_SetSVG_URLIMAGE_DATA(CMediaPMDBg * pme, SVGURLImageData* pData)
{
   AEEMediaCallback *pcb;

   if (!pme)
      return ENOMEMORY;

   if (pme->m_clsSelf != AEECLSID_MEDIASVG)
      return EFAILED;

   if (!pme->u.m_pPMD)
      return ENOMEMORY;

   pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, MM_PARM_SVG_URLIMAGE_DATA);
   if (!pcb)
      return EITEMBUSY;

   pme->u.m_pPMD->m_SVGURLImageData.handle = pme->u.m_pPMD->m_curSVGInfo.handle;
   pme->u.m_pPMD->m_SVGURLImageData.buffer = pData->pBuffer;
   pme->u.m_pPMD->m_SVGURLImageData.num_bytes = pData->dwBufferSize;
   pme->u.m_pPMD->m_SVGURLImageData.pURL = pData->pURL;

   return OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SVG_URLIMAGE_DATA, (int32)&pme->u.m_pPMD->m_SVGURLImageData, 0, 0, FALSE);
}
#endif // (SVGT_VERSION > SVGT_1_1)

/*==================================================================
  This version of the callback is used for standalone decoders
==================================================================*/
static void CMediaPMDMMLayer_PlayCB2(cmx_status_type status, const void * pClientData, void* data, uint32 num_bytes)
{
   CMediaPMDBg *           pme;
   AEEMediaCallback *   pcb = NULL;
   boolean              bResume = FALSE;

   OEMMM_ENTER_CRITICAL_SECTION()

   OEMMMStatus          eStatus;
   int                  nStatus = MM_STATUS_UNKNOWN;

   pme = (CMediaPMDBg *)AEEObjectMgr_GetObject((AEEObjectHandle)pClientData);
   if (!pme)
      goto OEMMM_CBExit;

   nStatus = CMediaPMDMMLayer_GetMMStatus2((CMediaCMX *)pme, status, data, num_bytes, &eStatus);
   if (nStatus == MM_STATUS_UNKNOWN)
      goto OEMMM_CBExit;

   // For playctl command, cbCommand was reserved...free it since
   // SEEK/PAUSE/RESUME/STOP status is sent thru play cb.
   if (eStatus == OEMMM_STATUS_PLAYCTL && CMediaPMDMMLayer_ISPLAYCTL(&pme->m_cbCommand))
   {
      CMediaPMDMMLayer_FreeCallback(&pme->m_cbCommand);
   }

   // For set parm event (tempo, tune, pan), cbCommand is sent in AEE_ResumeCallback().
   // Else for other playback events, one of cbPlay is sent in AEE_ResumeCallback().
   if (eStatus == OEMMM_STATUS_SETPARM) // || eStatus == OEMMM_STATUS_PLAYCTL)
   {
 #if defined (FEATURE_SVGDOM)
      if (CMX_SVG_DOM_DATA == status)
      {
         pcb = CMediaPMDMMLayer_AllocCallback(&pme->m_cbCommand, 1, MM_CMD_SETMEDIAPARM, OEMCMX_ID_SVG_DOM);
      }
      else
#endif // defined (FEATURE_SVGDOM)
         pcb = &pme->m_cbCommand;
   }
   else
      pcb = CMediaPMDMMLayer_AllocCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY, MM_CMD_PLAY, 0);

   if (pcb)
   {
      pcb->cmdNotify.nStatus = nStatus;
      CMediaPMDMMLayer_ProcessPlayServerData2((CMediaCMX *)pme, status, data, num_bytes, pcb);

      bResume = TRUE;
   }

OEMMM_CBExit:

   OEMMM_LEAVE_CRITICAL_SECTION()

   if (bResume)
   {
#if defined (FEATURE_SVGDOM)
      if (MM_STATUS_MEDIA_SPEC != pcb->cmdNotify.nStatus && pme->u.m_pPMD && pme->u.m_pPMD->m_bQISVGDOM)
      {
         // drop all media play events while waiting for the SVGDOM document handle

         if (MM_STATUS_SVG_PREPARE_DONE == pcb->cmdNotify.nStatus)
         {
            CMediaMMLayer_FreeCallback(pcb);
            pme->u.m_pPMD->m_bQISVGDOM = FALSE;
         }
         else if (MM_STATUS_ABORT == pcb->cmdNotify.nStatus)
         {
            pcb->cb.pfnNotify( pcb );
         }
         else
         {
            CMediaMMLayer_FreeCallback(pcb);
         }
      }
      else if ((CMX_SVG_DOM_DATA == status && MM_CMD_SETMEDIAPARM == pcb->cmdNotify.nCmd) ||
          (MM_STATUS_MEDIA_SPEC == pcb->cmdNotify.nStatus && pme->u.m_pPMD && pme->u.m_pPMD->m_bQISVGDOM))
      {
         if (pme->u.m_pPMD->m_bQISVGDOM)
         {
            // pull out the document handle from the callback data
            pme->u.m_pPMD->m_curSVGInfo = *(cmx_svg_info_type *)data;
         }

         pcb->cb.pfnNotify( pcb );
      }
      else
#endif // defined (FEATURE_SVGDOM)

      AEE_ResumeCallback(&pcb->cb, pme->m_pac);
   }
}

/*==================================================================
  This version is called for standalone decoders.
  Sets the data to be sent to the user callback with the status.
==================================================================*/
static void CMediaPMDMMLayer_ProcessPlayServerData2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, AEEMediaCallback * pcb)
{
   CMediaPMD * pPMD;

   if (!pme || !pcb || !data)
      return;

   pPMD = ((CMediaPMDBg*)pme)->u.m_pPMD;

   if (!pPMD)
      return;

   switch (status)
   {
      case CMX_SVG_INFO:
         //Media spec to send back is set in CMediaPMDBg_CallbackNotify
         break;

      case CMX_SVG_URL_REQ:
         if (data)
         {
            int nLen = num_bytes;
            if (CMX_SVG_MAX_URL_LENGTH < num_bytes)
              nLen = CMX_SVG_MAX_URL_LENGTH;

            MEMCPY(&pPMD->m_SVGURL, data, nLen);
            pcb->cmdNotify.pCmdData = (void*)pPMD->m_SVGURL;
            pcb->cmdNotify.dwSize = nLen;
         }
         break;

      case CMX_SVG_EVENT:
         if (data)
         {
            uint16 nData = 0;
            cmx_svg_event_done_type* pEventResult = (cmx_svg_event_done_type*)data;
            switch (pEventResult->event)
            {
               case CMX_SVG_KEY_EVENT:
                  nData = MM_SVG_EVENT_KEY;
                  break;

#if (SVGT_VERSION > SVGT_1_1)
               case CMX_SVG_KEY_DOWN_EVENT:
                  nData = MM_SVG_EVENT_KEY_DOWN;
                  break;

               case CMX_SVG_KEY_UP_EVENT:
                  nData = MM_SVG_EVENT_KEY_UP;
                  break;
#endif // (SVGT_VERSION > SVGT_1_1)

               case CMX_SVG_FOCUS_EVENT:
                  nData = MM_SVG_EVENT_FOCUS;
                  break;

               case CMX_SVG_POINTER_CLICK_EVENT:
                  nData = MM_SVG_EVENT_POINTER_CLICK;
                  break;

               case CMX_SVG_POINTER_DOWN_EVENT:
                  nData = MM_SVG_EVENT_POINTER_DOWN;
                  break;

               case CMX_SVG_POINTER_MOVE_EVENT:
                  nData = MM_SVG_EVENT_POINTER_MOVE;
                  break;

#if (SVGT_VERSION == SVGT_1_1)
               case CMX_SVG_POINTER_OVER_EVENT:
                  nData = MM_SVG_EVENT_POINTER_OVER;
                  break;

               case CMX_SVG_POINTER_LEAVE_EVENT:
                  nData = MM_SVG_EVENT_POINTER_LEAVE;
                  break;
#endif // (SVGT_VERSION == SVGT_1_1)

               case CMX_SVG_POINTER_UP_EVENT:
                  nData = MM_SVG_EVENT_POINTER_UP;
                  break;

               case CMX_SVG_ROTATE_EVENT:
                  nData = MM_SVG_EVENT_ROTATE;
                  break;

               case CMX_SVG_PAN_EVENT:
                  nData = MM_SVG_EVENT_PAN;
                  break;

               case CMX_SVG_ZOOM_EVENT:
                  nData = MM_SVG_EVENT_ZOOM;
                  break;
            }
            pcb->cmdNotify.nSubCmd = MM_PARM_SVG_INPUT;
            pcb->cmdNotify.pCmdData = (void*)nData;
            pcb->cmdNotify.dwSize = sizeof(nData);
         }
         break;

#if (SVGT_VERSION > SVGT_1_1)
      case CMX_SVG_EXT_REQ:
         if (data)
         {
            cmx_svg_request_type* pReq = (cmx_svg_request_type*)data;

            STRLCPY((char*)pPMD->m_SVGURL, pReq->filename, sizeof(pPMD->m_SVGURL));
            pPMD->m_SVGURLImageRequest.pURL = (char*)pPMD->m_SVGURL;
            pPMD->m_SVGURLImageRequest.bMoreRequests = pReq->more_requests;
            pcb->cmdNotify.pCmdData = (void*)&pPMD->m_SVGURLImageRequest;
            pcb->cmdNotify.dwSize = sizeof(SVGURLImageRequestData);
         }
         break;

      case CMX_SVG_EXT_FREE:
         if (data)
         {
            cmx_svg_request_free_type* pReq = (cmx_svg_request_free_type*)data;
           
            STRLCPY((char*)pPMD->m_SVGURL, pReq->filename, sizeof(pPMD->m_SVGURL));

            pPMD->m_SVGURLImageDataAck.pURL = (char *)pPMD->m_SVGURL;
            pPMD->m_SVGURLImageDataAck.bLoaded = pReq->loaded;
            pcb->cmdNotify.pCmdData = (void*)&pPMD->m_SVGURLImageDataAck;
            pcb->cmdNotify.dwSize = sizeof(SVGURLImageDataAck);
         }
         break;
#endif // (SVGT_VERSION > SVGT_1_1)
         
#if defined(FEATURE_SVGDOM)
      case CMX_SVG_DOM_DATA:
         if (data)
         {
            cmx_svgdom_data_type* pDOMResult = (cmx_svgdom_data_type*)data;
            pPMD->m_DOMCallbackData.nOperation = pDOMResult->api;
            pPMD->m_DOMCallbackData.nError = pDOMResult->error;
            pPMD->m_DOMCallbackData.nDataType = pDOMResult->type;
            switch (pDOMResult->type)
            {
               case CMX_SVGDOM_DATA_NONE:
                  pPMD->m_DOMCallbackData.pData = NULL;
                  break;
                  
               case CMX_SVGDOM_DATA_DOCUMENT:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.doc;
                  break;
                  
               case CMX_SVGDOM_DATA_ELEMENT:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.element;
                  break;
                  
               case CMX_SVGDOM_DATA_NODE:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.node;
                  break;
                  
               case CMX_SVGDOM_DATA_STRING:
                  if ( NULL !=  pDOMResult->data.string )
                  {
                     WSTRNCOPYN(pPMD->DOMRet.m_str, CMX_SVG_MAX_URL_LENGTH,
                                pDOMResult->data.string, -1);
                     pPMD->m_DOMCallbackData.pData = pPMD->DOMRet.m_str;
                  }
                  else
                  {
                     pPMD->m_DOMCallbackData.pData = NULL;
                  }
                  break;
                  
               case CMX_SVGDOM_DATA_FLOAT:
                  pPMD->DOMRet.m_fixed = pDOMResult->data.f; 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_fixed;
                  break;
                  
               case CMX_SVGDOM_DATA_BOOL:
                  pPMD->DOMRet.m_bool = pDOMResult->data.b; 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_bool;
                  break;
                  
               case CMX_SVGDOM_DATA_LONG:
                  pPMD->DOMRet.m_long = pDOMResult->data.l; 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_long;
                  break;
                  
               case CMX_SVGDOM_DATA_SHORT:
                  pPMD->DOMRet.m_short = pDOMResult->data.s; 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_short;
                  break;
                  
               case CMX_SVGDOM_DATA_EVTTARGET:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.evttarget;
                  break;
                  
               case CMX_SVGDOM_DATA_EVTLISTENER:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.evtlistener;
                  break;
                  
               case CMX_SVGDOM_DATA_MATRIX:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.matrix;
                  break;

               case CMX_SVGDOM_DATA_POINT:
                  pPMD->DOMRet.m_point.cx = pDOMResult->data.point.x; 
                  pPMD->DOMRet.m_point.cy = pDOMResult->data.point.y; 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_point;
                  break;

               case CMX_SVGDOM_DATA_PATH:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.path;
                  break;

               case CMX_SVGDOM_DATA_RECT:
                  pPMD->DOMRet.m_rect.x = pDOMResult->data.rect->x; 
                  pPMD->DOMRet.m_rect.y = pDOMResult->data.rect->y; 
                  pPMD->DOMRet.m_rect.dx = pDOMResult->data.rect->width; 
                  pPMD->DOMRet.m_rect.dy = pDOMResult->data.rect->height; 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_rect;
                  break;

#if (SVGT_VERSION == SVGT_1_1)
               case CMX_SVGDOM_DATA_RECT_PTR:
                  pPMD->m_DOMCallbackData.pData = (void *)pDOMResult->data.rect; 
                  break;
#endif // (SVGT_VERSION == SVGT_1_1)

               case CMX_SVGDOM_DATA_COLOR:
                  pPMD->DOMRet.m_RGB = MAKE_RGB(pDOMResult->data.color->red, pDOMResult->data.color->green, pDOMResult->data.color->blue); 
                  pPMD->m_DOMCallbackData.pData = (void*)&pPMD->DOMRet.m_RGB;
                  break;

               case CMX_SVGDOM_DATA_COLOR_PTR:
                  pPMD->m_DOMCallbackData.pData = (void*)pDOMResult->data.color;
                  break;

               case CMX_SVGDOM_DATA_SVGSVGELEMENT:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.element;
                  break;

               case CMX_SVGDOM_DATA_EVENT:
                  pPMD->m_DOMCallbackData.pData = pDOMResult->data.event;
                  break;

               default:
                  break;
            }
            pcb->cmdNotify.nSubCmd = OEMCMX_ID_SVG_DOM;
            pcb->cmdNotify.pCmdData = (void*)&pPMD->m_DOMCallbackData;
            pcb->cmdNotify.dwSize = sizeof(pPMD->m_DOMCallbackData);
            
         }      
         break;
#endif //defined(FEATURE_SVGDOM)

      default:
         break;
   } // switch
}

static void CopySVGInfo(AEEMediaSVGSpec* dest, cmx_svg_info_type* src)
{
   if (!dest || !src)
	   return;

   //The data types don't line up exactly, so have to copy elements individually.
   dest->flgFeatures = MM_SVG_SPEC_FEATURE_NONE;
#if (SVGT_VERSION == SVGT_1_1)
   dest->flgFeatures |= (src->animated ? MM_SVG_SPEC_FEATURE_ANIMATED : 0);
#endif // (SVGT_VERSION == SVGT_1_1)
   dest->flgFeatures |= (src->zoom_pan_events ? MM_SVG_SPEC_FEATURE_ZOOMPANEVENTS : 0);
   dest->flgFeatures |= (src->text_only ? MM_SVG_SPEC_FEATURE_TEXTONLY : 0);
   MEMCPY(dest->szTitle, src->title, sizeof(dest->szTitle));
   dest->nColorScheme = src->color_scheme;
//   dest->rgbTransparentColor = src->tr_color;
//   dest->bTransparent = src->transparency;
   dest->nFrameWidth = src->width;
   dest->nFrameHeight = src->height;
   dest->nStructSize = sizeof(AEEMediaSVGSpec);
}
#endif //defined(FEATURE_SVG)

#if defined(FEATURE_WEBAUDIO)
/*==================================================================

==================================================================*/
static void CMXClear(CMediaPMDBg * pme, uint8 nClearOptions)
{
   int nCallback;
   ContextSwitchCallback *pcscb;
   int nOption;
   
   for (nOption = 1; nOption <= CLEAR_REDRAW; nOption <<= 1)
   {
      pcscb = NULL;
      switch (nOption)
      {
         case CLEAR_IMAGE:
            pcscb = pme->u.m_pPMD->m_cbPicRelease;
            break;
         case CLEAR_ANIM:
            pcscb = pme->u.m_pPMD->m_cbAnimReset;
            break;
         case CLEAR_TEXT:
            pcscb = pme->u.m_pPMD->m_cbTextRelease;
            break;
         case CLEAR_REDRAW:
            pcscb = pme->u.m_pPMD->m_cbPMDImage;
            break;
         default:
            break;
      }
            
      if ((nClearOptions & nOption) && (pcscb != NULL))
      {
         nCallback = LockCSCallback(pcscb, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
         if (nCallback == -1)
            return;
         AEE_ResumeCallback(&pcscb[nCallback].cb, pme->m_pac);
      }
   }
   
}
   
/*==================================================================
This function queues a UI callback to change the draw status flags.
It's important to queue these changes rather than changing in CMX 
context so that everything is done in the correct order when other 
commands that use these flags are queued as UI callbacks.
==================================================================*/
static void CMXChangeDrawStatus(CMediaPMDBg * pme, uint8* pDrawFlag, uint8 nStatus, OEMMMDrawOp eOp, boolean bRedraw)
{
   int nCallback;

   nCallback = LockCSCallback(pme->u.m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
   if (nCallback == -1)
      return;
   pme->u.m_pPMD->m_drawInfo[nCallback].pDrawFlag = pDrawFlag;
   pme->u.m_pPMD->m_drawInfo[nCallback].nStatus = nStatus;
   pme->u.m_pPMD->m_drawInfo[nCallback].eOp = eOp;
   pme->u.m_pPMD->m_cbChangeDrawStatus[nCallback].pPayload = &pme->u.m_pPMD->m_drawInfo[nCallback];
   AEE_ResumeCallback(&pme->u.m_pPMD->m_cbChangeDrawStatus[nCallback].cb, pme->m_pac);

   if (bRedraw)
   {
      nCallback = LockCSCallback(pme->u.m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
      if (nCallback == -1)
         return;
      AEE_ResumeCallback(&pme->u.m_pPMD->m_cbPMDImage[nCallback].cb, pme->m_pac);
   }
}
   
/*==================================================================

==================================================================*/
static void CMXChangeDrawStatusFlag(void * pUser)
{
   ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
   CMediaPMDBg *   pme;
   DrawInfo* pDrawInfo;

   if (!pcscb)
      return;

   pme = (CMediaPMDBg *)(pcscb->pIMedia);
   pDrawInfo = (DrawInfo*)(pcscb->pPayload);
   
   UnlockCSCallback(pcscb);

   if (!pme || !pme->u.m_pPMD || !pDrawInfo)
      return;
      
   switch (pDrawInfo->eOp)
   {
      case OEMMM_DRAW_OP_EQ:
         *pDrawInfo->pDrawFlag = pDrawInfo->nStatus;
         break;
      case OEMMM_DRAW_OP_OR_EQ:
         *pDrawInfo->pDrawFlag |= pDrawInfo->nStatus;
         break;
      default:
         break;
   }
}

/*==================================================================

==================================================================*/
static void AnimReset(void * pUser) {
   ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
   CMediaPMDBg *   pme;

   if (!pcscb)
      return;

   pme = (CMediaPMDBg *)(pcscb->pIMedia);

   UnlockCSCallback(pcscb);

   if (!pme || !pme->u.m_pPMD)
      return;

   pme->u.m_pPMD->m_fAnimDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag
   pme->u.m_pPMD->m_displayedSaf = FALSE;
}

/*==================================================================
Checks if the address passed in matches one of the client-allocated
buffers.  Returns a pointer to the matching ClientBufInfo data, or
NULL if no match.
==================================================================*/
static ClientBufInfo* GetClientBufInfo(CMediaPMDBg * pme, uint8 * pClientBuf)
{
   int i;

   if (!pme || !pme->u.m_pPMD || !pme->u.m_pPMD->m_pClientBufInfos)
      return NULL;

   // Check if a buffer in the list matches the one passed
   for (i=0; i<pme->u.m_pPMD->m_numClientBufs; ++i)
   {
      if (pme->u.m_pPMD->m_pClientBufInfos[i].m_buf == pClientBuf)
      break;
   }
   
   // If no buffers in the list matched, return NULL
   if (i == pme->u.m_pPMD->m_numClientBufs)
      return NULL;

   return &pme->u.m_pPMD->m_pClientBufInfos[i];
}
#endif //defined(FEATURE_WEBAUDIO)

/*==============================================================================================================================
chinese GB2312-Unicode conversion definitions...
*/
/*===========================================================================
   This function Gets the mapping Unicode (UCS-2/UTF16) value.
===========================================================================*/
#if defined(FEATURE_WEBAUDIO)
AECHAR GetUniChar(unsigned short sValue)
{
	BYTE ch1= (BYTE)((sValue&0xff00)>>8);
	BYTE ch2=(BYTE)sValue;

	AECHAR ch;

	if( (ch1 >= 0xA1)&& (ch1 <= 0xFE) )
	{
		if( (ch2 >= 0xA1)&& (ch2 <= 0xFE))
		{

			ch = gb2312[ch1-0xA1][ch2-0xA1];
			return ch;

		}
		else
		{
			return NULL;
		}
	}
	else
	{
      return NULL;
	}	
}

/*===========================================================================
   This function converts the GB2312 16 bit UCS-2 encoded characters compatible 
   to UTF-16 encoding 
===========================================================================*/
boolean GB2312ToUnicode(unsigned char* pGBStr,AECHAR* pUStr,int nLen)
{
	
	int nCnt=0;
	int nByte1,nByte2=0;
	AECHAR wCh;	

    AECHAR* pUnStr;

	if( (!pUStr) || (!pGBStr) || ( nLen<= 0) )
	  	 return NULL;	
	
	pUnStr=pUStr;
    
	while(*pGBStr!='\0')
	{
		nByte1=(int)(*pGBStr);
		if (nByte1 >= 0xA1 && nByte1 <= 0xFE && *(pGBStr+1))
		{			
			nByte2=(int)(*(pGBStr+1));
			if (0xA1 <= nByte2 && nByte2 <= 0xFE)
			{			    
			   wCh=gb2312[nByte1-0xA1][nByte2-0xA1];
			   pUnStr[nCnt]= wCh;				
			}
			else
			  pUnStr[nCnt]=0xFFFD;

		   pGBStr+=2;
		}
		else if (nByte1<0x80)
		{
			pUnStr[nCnt]=nByte1;
			pGBStr += 1;
		}
		else
		{
			pUnStr[nCnt]=0xFFFD;
			pGBStr+=1;
		}
		nCnt++;
		nLen--;
		if(nLen <= 0)
		  break;
		//pGBStr++;
	}
	pUnStr[nCnt] = 0;
	return TRUE;
}

/*=============================================================================
   This function converts the Unified Hangul (UHC) Korean code to 
   16 bit UCS-2 encoded characters compatible to UTF-16 encoding
===============================================================================*/
boolean UHCToUnicode(unsigned char* pUHCStr,AECHAR* pUStr, int nUchLen)
{                                                                       
	int nLen		= 0;
	int nCnt		= 0;
	int bIsUHC	= 0;

	AECHAR *pUnStr;
	if ((!pUStr) || (!pUHCStr) || (nUchLen<=0))
	   return FALSE;

	pUnStr = pUStr;
	while (pUHCStr[nLen])
	{
      int nByte1 = (int) pUHCStr[nLen];
      bIsUHC = 0;
		
		/* UHC part 1 */
	   if (nByte1 >= 0x81 && nByte1 <= 0xA0)
      {
         int nByte2 = (int) pUHCStr[nLen+1];
         if ((nByte2 >= 0x41 && nByte2 < 0x5B) || 
             (nByte2 >= 0x61 && nByte2 < 0x7B) || 
             (nByte2 >= 0x81 && nByte2 < 0xFF)) 
        {
            unsigned int nRow = nByte1 - 0x81;
            unsigned int nCol = nByte2 - (nByte2 >= 0x81 ? 0x4D : 
                                nByte2 >= 0x61 ? 0x47 : 0x41);
            unsigned int nIndex = 178 * nRow + nCol;
            if (nIndex < 5696) 
            {
               int nIdx = 2*nRow+(nCol>=89?1:0);
               pUnStr[nCnt] = (AECHAR) (uhc_1_2uni_main_page81[nIdx] + 
                                        uhc_1_2uni_page81[nIndex]);
               bIsUHC = 1;
            }
         }  
      }
      else if (nByte1 >= 0xA1 && nByte1 < 0xFF) 
      {
         int nByte2 = (int) pUHCStr[nLen+1];
         if (nByte2 < 0xA1)
         {
            /* UHC part 2 */
            if ((nByte1 >= 0xA1 && nByte1 <= 0xC6)) 
            {
               if ((nByte2 >= 0x41 && nByte2 < 0x5B) || 
                   (nByte2 >= 0x61 && nByte2 < 0x7B) || 
                   (nByte2 >= 0x81 && nByte2 < 0xA1)) 
               {
                  unsigned int nRow = nByte1 - 0xA1;
                  unsigned int nCol = nByte2 - (nByte2 >= 0x81 ? 0x4D : 
                                      nByte2 >= 0x61 ? 0x47 : 0x41);
                  unsigned int nIndex = 84 * nRow + nCol;
                  if (nIndex < 3126) 
                  {
                     int nIdx = 2*nRow+(nCol>=42?1:0);
                     pUnStr[nCnt] = (AECHAR)(uhc_2_2uni_main_pagea1[nIdx] + 
                                             uhc_2_2uni_pagea1[nIndex]);
                     bIsUHC = 1;
                  }
               }
            }
         }
         else if (nByte2 < 0xFF) 
         {
            /* Code set 1 (KS C 5601-1992)*/
            nByte1 -= 0x80; nByte2 -= 0x80;
            if ((nByte1 >= 0x21 && nByte1 <= 0x2C) || 
                (nByte1 >= 0x30 && nByte1 <= 0x48) || 
                (nByte1 >= 0x4A && nByte1 <= 0x7D)) 
            {
               if (nByte2 >= 0x21 && nByte2 < 0x7F) 
               {
                  unsigned int nIndex = 94 * (nByte1 - 0x21) + (nByte2 - 0x21);
                  AECHAR wCh = 0xFFFD;
                  if (nIndex < 1410) 
                  {
                     if (nIndex < 1115)
                     wCh = ksc5601_2uni_page21[nIndex];
                  } 
                  else if (nIndex < 3854) 
                  {
                     if (nIndex < 3760) 
                     wCh = ksc5601_2uni_page30[nIndex-1410];
                  } 
                  else 
                  {
                     if (nIndex < 8742) 
                        wCh = ksc5601_2uni_page4a[nIndex-3854];
                  }

                  if (wCh != 0xFFFD) 
                  {
                     pUnStr[nCnt] = wCh;
                     bIsUHC = 1;
                  }
               }
            }
         }
      }

      if (bIsUHC) 
         nLen++;
      else
         pUnStr[nCnt] = (AECHAR) nByte1 < 0x80 ? nByte1 : 0xFFFD; // Handle ASCII code

      nLen++;
      nCnt++;
      if(nCnt >= nUchLen)
         break;
   }
   pUnStr[nCnt] = 0;
   return TRUE;
}

/*=============================================================================
   This function converts the Shift JIS (Japanese) code to 16 bit UCS-2 encoded 
   characters compatible to UTF-16 encoding
===============================================================================*/
boolean ShiftJISToUnicode(unsigned char *pJISStr,AECHAR *pUStr,int nUchLen)
{
   int nLen		= 0;
   int nCnt		= 0;
   int nBytes	= 0;

   AECHAR	*pUnStr;

   if ((!pUStr) || (!pJISStr) || (nUchLen<=0))
      return FALSE;

   pUnStr = pUStr;
   while (pJISStr[nLen])
	{
      int nByte1 = (int) pJISStr[nLen];
      nBytes = 0;
      if (nByte1 < 0x80 || (nByte1 >= 0xA1 && nByte1 <= 0xDF)) 
      {
         if (nByte1 < 0x80) 
            pUnStr[nCnt] = nByte1 == 0x5C ? 0x00A5 : 
                           nByte1 == 0x7E ? 0x203E : nByte1;
         else if (nByte1 >= 0xA1 && nByte1 < 0xE0) 
            pUnStr[nCnt] = nByte1 + 0xFEC0;
         nBytes = 1;
      } 
      else if ((nByte1 >= 0x81 && nByte1 <= 0x9F) || 
               (nByte1 >= 0xE0 && nByte1 <= 0xEA)) 
      {
         int nByte2 = (int) pJISStr[nLen+1];
         if ((nByte2 >= 0x40 && nByte2 <= 0x7E) || 
             (nByte2 >= 0x80 && nByte2 <= 0xFC)) 
         {
            int t1 = (nByte1 < 0xE0 ? nByte1-0x81 : nByte1-0xC1);
            int t2 = (nByte2 < 0x80 ? nByte2-0x40 : nByte2-0x41);
            int nNewByte1 = 2*t1 + (t2 < 0x5E ? 0 : 1) + 0x21;
            int nNewByte2 = (t2 < 0x5E ? t2 : t2-0x5E) + 0x21;
            if ((nNewByte1 >= 0x21 && nNewByte1 <= 0x28) || 
                (nNewByte1 >= 0x30 && nNewByte1 <= 0x74)) 
            {
               if (nNewByte2 >= 0x21 && nNewByte2 < 0x7F)
               {
                  int nIndex = 94 * (nNewByte1 - 0x21) + (nNewByte2 - 0x21);
                  unsigned short wCh = 0xFFFD;
                  if (nIndex < 1410) 
                  {
                     if (nIndex < 690)
                        wCh = jisx0208_2uni_page21[nIndex];
                  } 
                  else if (nIndex < 7808)
                  {
                     wCh = jisx0208_2uni_page30[nIndex-1410];
                  }
                  if (wCh != 0xFFFD) 
                  {
                     pUnStr[nCnt] = (AECHAR) wCh;
                     nBytes = 2;
                  }
               }
            }
         }
      } 
      else if (nByte1 >= 0xF0 && nByte1 <= 0xF9)
      {
         int nByte2 = (int) pJISStr[nLen+1];
         if ((nByte2 >= 0x40 && nByte2 <= 0x7E) || 
             (nByte2 >= 0x80 && nByte2 <= 0xFC))
         {
            pUnStr[nCnt] = (AECHAR) 0xe000 + 188*(nByte1 - 0xF0) + 
                           (nByte2 < 0x80 ? nByte2-0x40 : nByte2-0x41);
            nBytes = 2;
         }
      }

      if (nBytes == 0) 
         pUnStr[nCnt] = 0xFFFD;
      else if (nBytes > 1)
         nLen++;
      nLen++;
      nCnt++;
      if(nCnt >= nUchLen)
         break;
   }
   pUnStr[nCnt] = 0;
   return TRUE;
}

/*=============================================================================
   This function converts the Traditional Chinese (Big5) code to 16 bit UCS-2 encoded 
   characters compatible to UTF-16 encoding
===============================================================================*/
boolean Big5ToUnicode(unsigned char *pBIG5Str,AECHAR *pUStr,int nUchLen)
{
   int nLen		= 0;
   int nCnt		= 0;
   int bIsBig5	= 0;

   AECHAR *pUnStr;

   if ((!pUStr) || (!pBIG5Str) || (nUchLen<=0))
      return FALSE;

   pUnStr = pUStr;

   while (pBIG5Str[nLen])
   {
      int nByte1 = (int) pBIG5Str[nLen];
      bIsBig5 = 0;
      if ((nByte1 >= 0xA1 && nByte1 <= 0xC7) || 
          (nByte1 >= 0xC9 && nByte1 <= 0xF9)) 
      {
		   int nByte2 = (int) pBIG5Str[nLen+1];
         if ((nByte2 >= 0x40 && nByte2 < 0x7F) || 
             (nByte2 >= 0xA1 && nByte2 < 0xFF)) 
         {
            unsigned int nIndex = 157 * (nByte1 - 0xA1) + 
                                  (nByte2 - (nByte2 >= 0xA1 ? 0x62 : 0x40));
            AECHAR wCh = 0xFFFD;
            if (nIndex < 6280)
            {
               if (nIndex < 6121)
                  wCh = (AECHAR) big5_2uni_pagea1[nIndex];
            }
            else 
            {
               if (nIndex < 13932)
                  wCh = (AECHAR) big5_2uni_pagec9[nIndex-6280];
            }
            if (wCh != 0xFFFD) 
            {
               pUnStr[nCnt] = wCh;
               bIsBig5 = 1;
            }
         }
      }
      if (bIsBig5) 
         nLen ++;
      else
         pUnStr[nCnt] = (AECHAR) nByte1 < 0x80 ? nByte1 : 0xFFFD;

      nLen++;
      nCnt++;
      if(nCnt >= nUchLen)
         break;
   }
   pUnStr[nCnt] = 0;
   return TRUE;
}

/*=============================================================================
   This function converts the Hindi(Devanagari) coded characters to UCS-2 encoded character
   compatible to UTF-16 encoding
===============================================================================*/
boolean DevanagariToUnicode(unsigned char *pDVNGStr,AECHAR *pUStr,int nUchLen)
{
   int nCnt = 0;
   AECHAR *pUnStr;

   if ((!pUStr) || (!pDVNGStr) || (nUchLen<=0))
      return FALSE;

   pUnStr = pUStr;
   while(*pDVNGStr != '\0')
   {
      unsigned char nByte = *pDVNGStr;
      if (nByte >= 0xA1 && nByte <= 0xFE)
         pUnStr[nCnt] = (AECHAR) iscii_dvng_2uni_page[nByte - 0xA1];
      else if (nByte < 0x80)
         pUnStr[nCnt] = (AECHAR) nByte;
      else
         pUnStr[nCnt] = (AECHAR) 0xFFFD;

      pDVNGStr++;
      nCnt++;
      if(nCnt >= nUchLen)
         break;
   }
   pUnStr[nCnt] = 0;
   return TRUE;
}

/*===========================================================================
This function Gets the exact length of 16 bit UCS-2 encoded string
===========================================================================*/
int GetUniStringLength(unsigned char* pmbBuffer, cmx_af_char_set_enum_type eCharSetType, int nLen)
{
   int nUchLen = 0;
	int nCnt		= 0;
   int nMin = 0x81, nMax = 0xFE;
   switch(eCharSetType)
   {
   case CMX_AF_CHAR_SET_CHINESE_TRAD:
   case CMX_AF_CHAR_SET_CHINESE_SIMP:
      nMin = 0xA1;
      break;
   case CMX_AF_CHAR_SET_SHIFTJIS:
      nMax = 0x9F;
      break;
   case CMX_AF_CHAR_SET_HANGUL:
      break;
   case CMX_AF_CHAR_SET_ANSI:
   case CMX_AF_CHAR_SET_HINDI:
   default:
      return (nLen - 1);
   }

   while(nCnt < nLen)
   {
      int nByte = pmbBuffer[nCnt++];
      nUchLen++;
      if ((nByte >= nMin && nByte <= nMax) ||
          (eCharSetType == CMX_AF_CHAR_SET_SHIFTJIS && 
          nByte >= 0xE0 && nByte <= 0xF9))
         nCnt++;
   }
   return nUchLen;
}

/*===========================================================================================================
   This function variates the cmx-text block based on the characterset encoding (ANSI, GB2312, Hangul, ShiftJIS etc.,) 
   to 16 bit UCS-2 encoded characters compatible to UTF-16 encoding
=============================================================================================================*/
boolean ConvertToUnicodebuffer(uint8 *buffer,uint16 *iTxtlen,AECHAR **pUnicodeString,cmx_af_char_set_enum_type eCharSetType)
{
   AECHAR *pUnicodebuffer;
   int nUchLen			 = 0;
   int nLen				 = *iTxtlen;
   boolean bConverted = FALSE;
   nUchLen = GetUniStringLength(buffer, eCharSetType, nLen);
   pUnicodebuffer = MALLOC((nUchLen+1) * sizeof(AECHAR));
   if (!pUnicodebuffer)
      return FALSE;
   MEMSET(pUnicodebuffer, 0, (nUchLen+1) * sizeof(AECHAR));

   switch(eCharSetType)
   {
      case CMX_AF_CHAR_SET_ANSI:
         nUchLen++;
         UTF8TOWSTR(buffer,nUchLen,pUnicodebuffer,nUchLen*sizeof(AECHAR));
         bConverted = TRUE;
         break;
      case CMX_AF_CHAR_SET_CHINESE_SIMP:
         bConverted = GB2312ToUnicode(buffer,pUnicodebuffer,nUchLen);
         break;
      case CMX_AF_CHAR_SET_HANGUL:
         bConverted = UHCToUnicode(buffer,pUnicodebuffer,nUchLen);
         break;
      case CMX_AF_CHAR_SET_SHIFTJIS:
         bConverted = ShiftJISToUnicode(buffer,pUnicodebuffer,nUchLen);
         break;
      case CMX_AF_CHAR_SET_CHINESE_TRAD:
         bConverted = Big5ToUnicode(buffer,pUnicodebuffer,nUchLen);
         break;
      case CMX_AF_CHAR_SET_HINDI:
         bConverted = DevanagariToUnicode(buffer,pUnicodebuffer,nUchLen);
         break;
      default:
         bConverted = FALSE;
         break;
   }
   *iTxtlen=nUchLen;
   //exact string length is referenced to chop the unwanted characters 
   //seen while rendering the characters on to the device.	

   *pUnicodeString=pUnicodebuffer;
	
   if(*pUnicodeString && bConverted)
      return TRUE;
   else
   {
      FREEIF(pUnicodebuffer);
      return FALSE;
   }
}

#endif // defined(FEATURE_WEBAUDIO)

