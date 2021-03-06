/*===========================================================================

FILE: OEMMediaPMD.c

SERVICES: IMEDIA interfaces

DESCRIPTION
   This file implements IMedia-based classes on CMX multimedia platform.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright � 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR


                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/oem/BREW3.1.5/main/latest/src/OEMMediaPMD.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/09   bk     Featurized for removing MM warnings.
01/19/09   bk     Properly featurized for fixing compilation issues arised
                  after trying to remove FEATURE_WEBAUDIO
11/21/08   pcn    Fixed CR 156552. 
10/15/08	kd 	   Fixed klockwork warnings  	
09/17/08    ss     Moved CS Callbacks before OEMMediaPMD_Start as the 
                   callback data is lost due to CMX context switch.
                   Issue orginiated from BREW 4.0(CR_139040) and 
                   sometime back in BREW 3.1.5 in customer builds.
09/17/08  ss/gs    Added pcb allocation fixes that was originated from 
                   CMX_STARTED. Fixed CR_146672.
04/16/08    ss     Fixed Compiler Warning that was introduced in Ver#6.
04/03/08  ryd/kd   Fixed cr 141969
03/31/08  ant/ss   Added ixstream_length in CMediaMMLayer_SetSource() for
                   get_total_time fix. Fixed CR_141269.
03/21/08    ss     Added a NULL check in SAFLoadFrameToDIB.Fixed CR_139068.
03/07/08    ss     Added ixstream_delete_stream in CMediaCMX_Delete and
                   CMediaCMX_SetMediaParm for a fix related to OMA DRM.
01/11/08    ss     Fixed a minor bug in the code.(CR_135279)
11/05/07   sk/lg   Added SVG Scaling support.
09/17/07   sk/lg   Added support to allow specifying SVG display
                   when with PMD.CR 118438.
08/01/06    li     Removed FEATURE_SVGDOM

07/13/06    qz	   Added single frame SVG PAN support and fixed animated flag

3/27/06    antti   added FEATURE_SVGDOM

===========================================================================*/

#include <stdlib.h>
#include "OEMFeatures.h"

#if defined(FEATURE_WEBAUDIO)
#include "gm2312.h"    // Simplified Chinese Unicode Map
#include "uhc.h"			// Unified Hangul Unicode Map
#include "jisx0208.h"	// Shift JIS Unicode Map
#include "big5.h"      // Traditional Chinese Unicode Map
#include "iscii_dvng.h"// ISCII (Devanagari) Unicode Map
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

#include "AEESource.h"

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#include "AEECLSID_SYNTHQUALITY.BID"
#include "OEMSynthQuality.h"
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)

#if defined(FEATURE_SVG)
#if !defined(SVGT_VERSION)
#define SVGT_1_1 0
#define SVGT_VERSION SVGT_1_1
#endif // !defined(SVGT_VERSION)
#endif // defined(FEATURE_SVG)

#if defined(FEATURE_QTV_DRM_DCF)
#define FEATURE_PMD_STREAMING

#undef __cplusplus
#include "IxStreamUtils.h"
#endif // defined(FEATURE_QTV_DRM_DCF)

#if defined(FEATURE_ACM)
#include "OEMACM.h"
#endif

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
//   {0,0,0,NULL,NULL}
//};

// MMLayer global static objects...

typedef struct CMediaInitFlags
{
   flg   gbCMXInit:1;
   flg   gbMediaBgInit:1;
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
   flg      bReqPending;
} CMXDataRequest;

typedef struct OEMMediaPMD
{
   AEECLSID       m_clsSelf;

   CMXInfo        m_ciStart;
   CMXInfo        m_ciStatus;

   int16          m_eType; // CMX_AUDFMT_START_XXX
   uint32         m_dwPauseMS;     // Pause between ringers
   uint16         m_wVolume;
   uint16         m_wPan;

#ifdef FEATURE_MULTISEQUENCER
   // Multisequencer
   cmx_audfmt_multi_param_type * m_pMultiParam;
   void *                        m_pSequence;
   cmx_audfmt_seq_param_type *   m_pSeqParam;
   
#endif  //FEATURE_MULTISEQUENCER

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

#define OEMMM_MAX_CB_CONTEXT_SWITCH    120     // may need to raise this number if too many CMX callbacks come between UI callbacks 
#define OEMMM_BUFFER_SIZE_FACTOR       2048    // Buffer copies allocated will be OEMMM_MAX_CB_CONTEXT_SWITCH / (buffer size / OEMMM_BUFFER_SIZE_FACTOR)

#define OEMMM_ENTER_CRITICAL_SECTION()     AEECriticalSection_Enter(&gMediaCMXCriticalSection); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} AEECriticalSection_Leave(&gMediaCMXCriticalSection);

#define OEMCMX_TYPE_AUDFMT_PLAY        1  // p2 = CMX_AUDFMT_START_XXX
#define OEMCMX_TYPE_AUDFMT_RINGER      2  // p2 = CMX_AUDFMT_START_XXX, p3 = Pause time
#define OEMCMX_TYPE_AUDFMT_PLAY_MULTI  3  // p2 = cmx_audfmt_multi_param_type *

#define OEMCMX_ID_TYPE              1  // p1 = CMXInfo *, p2/p3 = see comments in OEMCMX_TYPE_XXX
#define OEMCMX_ID_STATUS_CB         2  // p1 = CMXInfo *
#define OEMCMX_ID_TOTALTIME         3  // p1 = CMXInfo *
#define OEMCMX_ID_VOLUME            4  // p1 = Volume
#define OEMCMX_ID_PAN               5  // p1 = Pan
#define OEMCMX_ID_TEMPO             6  // p1 = Tempo
#define OEMCMX_ID_TUNE              7  // p1 = Tune
#define OEMCMX_ID_SEQUENCE          8  // p1 = Sequence, p2 = cmx_audfmt_seq_param_type *

#if defined(FEATURE_PMD_STREAMING)
#define OEMCMX_ID_DATA_RESPONSE     9  // p1 = CMXDataRequest *, p2 = uint8 * (pBuf), p3 = sizeof buffer in p2

#define OEMMM_CMX_REQ_SIZE_MAX         (4 * sizeof(CMXDataRequest))  // CMX request buffers
#define OEMMM_BUFFER_UNIT_SIZE         (8820)                        // IMedia data fetch unit size
#define OEMMM_BUFFER_SIZE_MAX          (4 * OEMMM_BUFFER_UNIT_SIZE)  // IMedia data buffer size (default)

#define OEMMM_ISSTREAMING(pme)         ((pme)->m_bStreaming)
#endif // defined(FEATURE_PMD_STREAMING)

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

#if defined(FEATURE_PMD_STREAMING)
typedef struct CQueue
{
   uint8 *  pBuffer;    // Size of the queue
   int32    nMaxSize;   // Max size (bytes) of the queue
   int32    nWrite;     // Write pointer
   int32    nRead;      // Read pointer
   int32    nFill;      // Size (bytes) of data filled in the queue
   boolean  bMutex;     // Specifies if mutex lock is required for operations
} CQueue;
#endif // defined(FEATURE_PMD_STREAMING)

#if defined(FEATURE_QTV_DRM_DCF)
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   OEMMediaPMD *             m_pMediaCMX; \
   \
   CQueue *                  m_pqCMXDataReq; \
   AEECallback *             m_pcbCMXDataReq; \
   CQueue *                  m_pqMediaData; \
   AEECallback *             m_pcbMediaData; \
   uint8 *                   m_pSrcBuffer; \
   uint32                    m_dwSrcBufferSize; \
   CMXDataRequest            m_sReq; \
   IxStreamReadRspT          m_pReadRsp; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   boolean                   m_bMute; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint32                    m_dwTickTimeMS; \
   uint16                    m_wSilenceTimeMS; \
   flg                       m_bStreaming:1; \
   flg                       m_bCancelRead : 1; \
   flg                       m_bEOS:1; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
#elif defined(FEATURE_PMD_STREAMING)
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   OEMMediaPMD *             m_pMediaCMX; \
   \
   CQueue *                  m_pqCMXDataReq; \
   AEECallback *             m_pcbCMXDataReq; \
   CQueue *                  m_pqMediaData; \
   AEECallback *             m_pcbMediaData; \
   uint8 *                   m_pSrcBuffer; \
   uint32                    m_dwSrcBufferSize; \
   CMXDataRequest            m_sReq; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   boolean                   m_bMute; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint32                    m_dwTickTimeMS; \
   uint16                    m_wSilenceTimeMS; \
   flg                       m_bStreaming:1; \
   flg                       m_bCancelRead : 1; \
   flg                       m_bEOS:1; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
#else
#define INHERIT_CMediaCMX(iname) \
   INHERIT_AEEMedia(iname); \
   AEEObjectHandle           m_hObject;  \
   PFNNOTIFY                 m_pfnDelete; \
   OEMMediaPMD *             m_pMediaCMX; \
   \
   uint32                    m_dwCaps; \
   uint32                    m_dwEnabledCaps; \
   int                       m_nAPath; \
   boolean                   m_bMute; \
   uint16                    m_wVolume; \
   uint16                    m_wPan; \
   uint16                    m_wTempo; \
   uint16                    m_wTune; \
   uint32                    m_dwTickTimeMS; \
   uint16                    m_wSilenceTimeMS; \
   flg                       m_bStopping:1; \
   flg                       m_bMIDIOut:1; \
   flg                       m_bVolumeScaled:1
#endif

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
	cmx_svg_info_type            m_cmx_svg_info[OEMMM_MAX_CB_CONTEXT_SWITCH];
	cmx_svg_info_type            m_curSVGInfo;
	cmx_svg_frame_done_type      m_SVGFrameData[OEMMM_MAX_CB_CONTEXT_SWITCH];
	NativeColor                  m_SVGTransColor;
	ContextSwitchCallback        m_cbSVGCreateDIB[OEMMM_MAX_CB_CONTEXT_SWITCH];  // To schedule SVG processing
	ContextSwitchCallback        m_cbSVGLoadFrame[OEMMM_MAX_CB_CONTEXT_SWITCH];
#endif //defined(FEATURE_SVG)

	IBitmap*    m_pCallerFrameBmp;

   uint16      m_nCallbacksSkipped; // Use diagnostically to adjust OEMMM_MAX_CB_CONTEXT_SWITCH

	// These were changed from flg type so they can be used by reference
	uint8       m_fTextDraw;
	uint8       m_fPicDraw;
	uint8       m_fAnimDraw;

	flg         m_bHeaderRead:1;
	flg         m_displayedSaf:1;  // This flag is true if an animation has ever been displayed.
	flg         m_transparency:1;  // This flag is true if an animation contains transparency.
	flg         m_displayedPic:1;  // This flag is true if an image has ever been displayed.
	flg         m_FrameCallbackEnabled:1;  // True if caller has requested frame callbacks
	flg         m_DispAreaChanged:1; // True if the media display area is altered
} CMediaPMD;
#endif // FEATURE_WEBAUDIO

/*===============================================================================

      CMediaPMDBg: Background Audio (MIDI, MP3, QCP. Includes PMD) related.

===============================================================================*/
typedef struct CMediaPMDBg          CMediaPMDBg;

#if defined(FEATURE_PMD_STREAMING)
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
#endif // defined(FEATURE_PMD_STREAMING)


#define OEMMM_MAX_CB_PLAY              3     // simultaneous play/record cbs 

struct CMediaPMDBg
{
   INHERIT_CMediaCMX(IMediaMIDI);

#ifdef FEATURE_WEBAUDIO
   CMediaPMD *       m_pPMD;
#endif // FEATURE_WEBAUDIO

   AEEMediaCallback  m_cbPlay[OEMMM_MAX_CB_PLAY];
   AEEMediaCallback  m_cbCommand;
   AEEMediaCallback  m_cbGetTotalTime;

   ISound *          m_pISound;

#if defined(FEATURE_MULTISEQUENCER)
   void   *          m_pSequence;
#endif // defined(FEATURE_MULTISEQUENCER)

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   uint32            m_dwNotes;
   boolean           m_bISynthQualityRef;
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
};

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
#define OEMMM_MAX_NOTES    72 // Accommodate as many notes as possible (HP_ALL mode)
static uint32              gdwNotes = OEMMM_MAX_NOTES;
static boolean             gbNotesSet = FALSE;
static ISynthQuality      *gpISynthQuality = NULL;
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)

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

#ifdef FEATURE_ACM
static int CMediaPMD_AcmPlayCB(ACM_Callback_data_type *po);
extern void AEEMedia_CleanUpResource(AEEMedia * po);
#endif

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

#if defined(FEATURE_PMD_STREAMING)
static void CMediaPMDMMLayer_ReadSource(CMediaPMDBg * pme);
static void CMediaPMDMMLayer_RequestSourceData(CMediaPMDBg * pme);
static void CMediaPMDMMLayer_CMXDataReqNotify(CMediaPMDBg * pme);
void CMediaPMDMMLayer_CMXDataRequestCB(uint32 position, uint32 max_len, cmx_client_req_cb_func_ptr_type data_req_cb, 
                                           const void *server_client_data, const void *client_data);
static void CMediaPMDMMLayer_FlushStreamData(CMediaPMDBg * pme);
#endif // #if defined(FEATURE_PMD_STREAMING)

static int CMediaPMDMMLayer_GetMMStatus(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, OEMMMStatus * peStatus);
static int CMediaPMDMMLayer_GetMMStatus2(CMediaCMX * pme, cmx_status_type status, void* data, uint32 num_bytes, OEMMMStatus * peStatus);
static void CMediaPMDMMLayer_StatusCB(cmx_status_type status, const void * pClientData);
static void CMediaPMDMMLayer_PlayCB(cmx_status_type status, const void * pClientData, cmx_af_server_data_type * psd);
static void CMediaPMDMMLayer_ProcessPlayServerData(CMediaCMX * pme, cmx_status_type status, cmx_af_server_data_type * psd, AEEMediaCallback * pcb);
static void CMediaPMDMMLayer_SetMediaSpec(CMediaPMDBg * pme, cmx_af_audio_spec_type * psd, AEEMediaCallback * pcb);

/*---------------------------------------------------------------------------
      Media play/record
-----------------------------------------------------------------------------*/

#if defined(FEATURE_WEBAUDIO)
static int CMXImage_Process(CMediaPMDBg * pme, cmx_status_type status, cmx_af_server_data_type * psd);
#endif
#if defined(FEATURE_WEBAUDIO)
static int CMXImage_Process2(CMediaPMDBg * pme, cmx_status_type status, void* data, uint32 num_bytes);
#endif 

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
static int CMediaPMDMMLayer_SetSynthQuality(CMediaPMDBg *pme);
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)

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
static int CMediaPMDMMLayer_PlayCodec(CMediaPMDBg * pme, cmx_handle_type * ph);
static int CMediaPMDMMLayer_Stop(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_Pause(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_Resume(CMediaPMDBg * pme);
static AEEMediaCallback * CMediaPMDMMLayer_AllocCallback(AEEMediaCallback * pcb, int nMax, int nCmd, int nSubCmd);
static int CMediaPMDMMLayer_GetTotalTime(CMediaPMDBg * pme);
static int CMediaPMDMMLayer_Seek(CMediaPMDBg * pme, AEEMediaSeek eSeek, int32 lTimeMS);

static int CMediaPMDMMLayer_SetTempo(CMediaPMDBg * pme, uint32 dwTempo);
static int CMediaPMDMMLayer_SetTune(CMediaPMDBg * pme, uint32 dwTune);
static int CMediaPMDMMLayer_SetPan(CMediaPMDBg * pme, uint32 dwPan);

static void CMediaPMDMMLayer_TimeCB(cmx_status_type status, const void * pClientData, uint32 dwTimeMS);

#if defined(FEATURE_SAF)
static void     SetSAFOffsets(CMediaPMDBg * pme, cmx_saf_data_type *pSAFData);
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG) && defined(FEATURE_WEBAUDIO)
static void  SetAnimOffsets(CMediaPMDBg * pme, cmx_af_pos_struct_type *frame_pos);
#endif //defined(FEATURE_SVG)
#ifdef FEATURE_AUDIO_POSTCARD
static void     JPEGCopyBuffer(void * pUser);
#endif /* FEATURE_AUDIO_POSTCARD */
#if defined(FEATURE_SAF)
static void     SAFCreateDIB(void * pUser);
static void     SAFLoadFrameToDIB(void * pUser);
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG) && defined (FEATURE_WEBAUDIO)
static void     SVGCreateDIB(void * pUser);
static void     SVGLoadFrameToDIB(void * pUser);
#endif //defined (FEATURE_SVG)
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)
static void     FreeJPEGBuf(void * pUser);
static void     SetImageOffsets(CMediaPMDBg * pme);
#endif //defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)

/*#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_SAF) \
	|| defined(FEATURE_SVG) || defined(FEATURE_AUDIO_POSTCARD)*/

#if defined(FEATURE_WEBAUDIO) 
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
      case MM_PARM_TICK_TIME:
         break;

#if defined(FEATURE_WEBAUDIO)
      case MM_PARM_RECT:
         if (pme->m_pPMD)
         {
            if (p1)
            {
               MEMCPY(&pme->m_pPMD->m_rectImage, (void *)p1, sizeof(AEERect));
               pme->m_pPMD->m_DispAreaChanged = TRUE;
            }

            if (p2)
               MEMCPY(&pme->m_pPMD->m_rectText, (void *)p2, sizeof(AEERect));

            nRet = SUCCESS;
         }
         break;

      case MM_PARM_FRAME:
         if (pme->m_pPMD)
         {
            pme->m_pPMD->m_FrameCallbackEnabled = p1;
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
         if (pme->m_bChannelShare)
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
         } 
         break;
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)

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
      case MM_PARM_TICK_TIME:
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_RECT:
#if defined(FEATURE_WEBAUDIO)
         if (pme->m_pPMD)
         {
            if (pP1)
               MEMCPY(pP1, &pme->m_pPMD->m_rectImage, sizeof(AEERect));

            if (pP2)
               MEMCPY(pP2, &pme->m_pPMD->m_rectText, sizeof(AEERect));
         }
         else
#endif // defined(FEATURE_WEBAUDIO)
         nRet = EUNSUPPORTED;
         break;

      case MM_PARM_FRAME:
#if defined(FEATURE_WEBAUDIO)
         if (pme->m_pPMD)
         {
            //GetFrame
            if (pP1)
            {
               if (pme->m_pPMD->m_pCallerFrameBmp)
               {
                  IBITMAP_AddRef(pme->m_pPMD->m_pCallerFrameBmp);
                  *pP1 = (int32)pme->m_pPMD->m_pCallerFrameBmp;
               }
               else
               {
                  nRet = EFAILED;
                  break;
               }
            }

            //IsFrameCallback
            if (pP2)
            {
              *pP2 = pme->m_pPMD->m_FrameCallbackEnabled;
            }
         }
         else
#endif // defined(FEATURE_WEBAUDIO)
         {
            nRet = EUNSUPPORTED;
         }
         break;

   case MM_PARM_CHANNEL_SHARE:
	  if (pP1)
		 *(boolean *)pP1 = pme->m_bChannelShare;
	  break;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
   case MM_PARM_NOTES:
	  if (pP1 && pme->m_bChannelShare)
             *(uint32 *)pP1 = pme->m_dwNotes;
	  else
	     nRet = EUNSUPPORTED;
	  break;
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)

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

   return AEEMedia_QueryInterface(po, idReq, ppo);
}

/*==================================================================
   Destructor
==================================================================*/
static void CMediaPMDBg_Delete(CMediaPMDBg * pme)
{

#ifdef FEATURE_WEBAUDIO
   int i;
#endif /*FEATURE_WEBAUDIO*/

   CMediaPMDMMLayer_CancelCallback(pme->m_cbPlay, OEMMM_MAX_CB_PLAY);
   CMediaPMDMMLayer_CancelCallback(&pme->m_cbCommand, 1);
   CMediaPMDMMLayer_CancelCallback(&pme->m_cbGetTotalTime, 1);

   MM_RELEASEIF(pme->m_pISound);

#if defined(FEATURE_WEBAUDIO) 
   if (pme->m_pPMD)
   {
      CancelCSCallback(pme->m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CALLBACK_Cancel(&pme->m_pPMD->m_cbJPEGBufAlloc);
      CALLBACK_Cancel(&pme->m_pPMD->m_cbJPEGBufFree);
      CancelCSCallback(pme->m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbPicRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbTextRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbPMDText, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbCopyBMPBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbCopyPNGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbCopyJPEGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
#if defined(FEATURE_SAF)
      CancelCSCallback(pme->m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbSAFLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG)
      CancelCSCallback(pme->m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbSVGLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif //defined(FEATURE_SVG)
      CancelCSCallback(pme->m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH);      
      CancelCSCallback(pme->m_pPMD->m_cbAnimReset, OEMMM_MAX_CB_CONTEXT_SWITCH);

      if(pme->m_pPMD->m_pIAnimBitmap) {
         if(pme->m_pPMD->m_pIAnimBitmap->cntRGB)
            FREEIF(pme->m_pPMD->m_pIAnimBitmap->pRGB);
         IDIB_Release(pme->m_pPMD->m_pIAnimBitmap);
         pme->m_pPMD->m_pIAnimBitmap = NULL;
      }

      if(pme->m_pPMD->m_pIPicBitmap) {
         if(pme->m_pPMD->m_pIPicBitmap->cntRGB)
            FREEIF(pme->m_pPMD->m_pIPicBitmap->pRGB);
         IDIB_Release(pme->m_pPMD->m_pIPicBitmap);
         pme->m_pPMD->m_pIPicBitmap = NULL;
      }

      pme->m_pPMD->m_pIDispBitmap = NULL;

      MM_RELEASEIF(pme->m_pPMD->m_pIDisp);
      
      {
         ContextSwitchCallback cscb;

         cscb.pIMedia = (IMedia*)pme;
         TextRelease((void*)&cscb);
      }

      FreeJPEGBuf(pme);

      pme->m_pPMD->m_displayedPic = FALSE;
      pme->m_pPMD->m_displayedSaf = FALSE;
      pme->m_pPMD->m_transparency = FALSE;
      
      for (i=0; i<pme->m_pPMD->m_numClientBufs; ++i)
      {
         int nCopy;

         FREEIF(pme->m_pPMD->m_pClientBufInfos[i].m_buf);
         for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
            FREEIF(pme->m_pPMD->m_pClientBufInfos[i].m_copies[nCopy]);
      }
      FREEIF(pme->m_pPMD->m_pClientBufInfos);
      MM_RELEASEIF(pme->m_pPMD->m_pCallerFrameBmp);
   }
   FREEIF(pme->m_pPMD);
#endif // defined(FEATURE_WEBAUDIO)

#if defined(FEATURE_PMD_STREAMING)
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
#endif // defined(FEATURE_PMD_STREAMING)

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
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
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

/*==================================================================

==================================================================*/
static int CMediaPMDBg_Stop(IMedia * po)
{
   CMediaPMDBg * pme = (CMediaPMDBg *)po;
   int            nRet;

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

#if defined(FEATURE_PMD_STREAMING)
   if (pcb->cmdNotify.nStatus == MM_STATUS_DONE || pcb->cmdNotify.nStatus == MM_STATUS_ABORT)
   {
      if (OEMMM_ISSTREAMING(pme))
         CMediaPMDMMLayer_FlushStreamData(pme);
   }
#endif // defined(FEATURE_PMD_STREAMING)

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

#if defined(FEATURE_PMD_STREAMING)
   pme->m_bStreaming = FALSE;

#if defined(FEATURE_QTV_DRM_DCF)
   if ( pme->m_pReadRsp.pStream != NULL )
   {
      (void)ixstream_close_sync ( pme->m_pReadRsp.pStream );
      ixstream_delete_stream( pme->m_pReadRsp.pStream );
      pme->m_pReadRsp.pStream = NULL;
   }
#endif // defined(FEATURE_QTV_DRM_DCF)
#endif // defined(FEATURE_PMD_STREAMING)

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
   
#if defined(FEATURE_WEBAUDIO) 
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

   if (!pme || !pme->m_pPMD || !pme->m_pPMD->m_pIDisp)
      return;

   if (pme->m_pPMD->m_FrameCallbackEnabled)
   {
      if (IDISPLAY_GetDeviceBitmap(pme->m_pPMD->m_pIDisp, &pDispBmp) != SUCCESS)
         return;

      if (pme->m_pPMD->m_DispAreaChanged)
         MM_RELEASEIF(pme->m_pPMD->m_pCallerFrameBmp);

      if (!pme->m_pPMD->m_pCallerFrameBmp)
      {
         IBITMAP_CreateCompatibleBitmap(pDispBmp, &pme->m_pPMD->m_pCallerFrameBmp, pme->m_pPMD->m_rectImage.dx, pme->m_pPMD->m_rectImage.dy);
      }

      IDISPLAY_SetDestination(pme->m_pPMD->m_pIDisp, pme->m_pPMD->m_pCallerFrameBmp);
 
      // The frame bitmap is the size of the screen's destination rect
      // and we have to re-offset all operations accordingly
      nRectToDestOffsetX = pme->m_pPMD->m_rectImage.x;
      nRectToDestOffsetY = pme->m_pPMD->m_rectImage.y;
      // Rectangle corresponding to the frame bitmap to return to caller
      SETAEERECT(&rcDest, 0, 0, pme->m_pPMD->m_rectImage.dx, pme->m_pPMD->m_rectImage.dy);
   }
   else
   {
      // Set further offsets to nothing since destination rect is correct for drawing to screen
      nRectToDestOffsetX = 0;
      nRectToDestOffsetY = 0;
      // Rectangle corresponding to the screen's destination rect
      MEMCPY(&rcDest, &pme->m_pPMD->m_rectImage, sizeof(AEERect));
   }

   if (pme->m_pPMD->m_pIDisp &&
       (pme->m_pPMD->m_fAnimDraw & OEMMM_DISP_CLEAR ||
       pme->m_pPMD->m_fPicDraw & OEMMM_DISP_CLEAR ||
       pme->m_pPMD->m_fTextDraw & OEMMM_DISP_CLEAR))
   {
      IDISPLAY_EraseRect(pme->m_pPMD->m_pIDisp, &rcDest);
      if (pme->m_pPMD->m_pIDispBitmap == pme->m_pPMD->m_pIPicBitmap)
         pme->m_pPMD->m_fPicDraw &= ~OEMMM_DISP_CLEAR;
      if (pme->m_pPMD->m_pIDispBitmap == pme->m_pPMD->m_pIAnimBitmap)
         pme->m_pPMD->m_fAnimDraw &= ~OEMMM_DISP_CLEAR;
      pme->m_pPMD->m_fTextDraw &= ~OEMMM_DISP_CLEAR;
   }

   // Picture
   if (pme->m_pPMD->m_pIDispBitmap)
   {
      AEERect *pRect = &(pme->m_pPMD->m_rectImage);
         uint16 width, height;

      if(pme->m_pPMD->m_pIDispBitmap == pme->m_pPMD->m_pIPicBitmap ||
         pme->m_pPMD->m_pIDispBitmap == pme->m_pPMD->m_pIAnimBitmap)
          
      {
         if (pme->m_pPMD->m_fPicDraw & OEMMM_DISP_DRAW)
         {

            // Copy the picture to the display if one has ever been displayed.
            if (pme->m_pPMD->m_displayedPic == TRUE) {
                if (pme->m_pPMD->m_picDispRect.x + pme->m_pPMD->m_picDispRect.dx
                    > pRect->x + pRect->dx)
                   width = pRect->x + pRect->dx - pme->m_pPMD->m_picDispRect.x;
                else
                   width = pme->m_pPMD->m_picDispRect.dx;

                if (pme->m_pPMD->m_picDispRect.y + pme->m_pPMD->m_picDispRect.dy 
                    > pRect->y + pRect->dy)
                   height = pRect->y + pRect->dy - pme->m_pPMD->m_picDispRect.y;
                else
                   height = pme->m_pPMD->m_picDispRect.dy;

                if (pme->m_pPMD->m_pIPicBitmap)
                   IDISPLAY_BitBlt(pme->m_pPMD->m_pIDisp, 
                      pme->m_pPMD->m_picDispRect.x - nRectToDestOffsetX, 
                      pme->m_pPMD->m_picDispRect.y - nRectToDestOffsetY, 
                      width, 
                      height, 
                      (const void *) pme->m_pPMD->m_pIPicBitmap, 
                      pme->m_pPMD->m_picOffsetRect.x, 
                      pme->m_pPMD->m_picOffsetRect.y, 
                      AEE_RO_COPY);
                      //AEE_RO_TRANSPARENT);
            }
         }
      
         // Animation frame
         if (pme->m_pPMD->m_fAnimDraw & OEMMM_DISP_DRAW)
         {
 
            if (pme->m_pPMD->m_displayedSaf == TRUE) {
                if (pme->m_pPMD->m_animDispRect.x + pme->m_pPMD->m_animDispRect.dx
                    > pRect->x + pRect->dx)
                   width = pRect->x + pRect->dx - pme->m_pPMD->m_animDispRect.x;
                else
                   width = pme->m_pPMD->m_animDispRect.dx;

                if (pme->m_pPMD->m_animDispRect.y + pme->m_pPMD->m_animDispRect.dy 
                    > pRect->y + pRect->dy)
                   height = pRect->y + pRect->dy - pme->m_pPMD->m_animDispRect.y;
                else
                   height = pme->m_pPMD->m_animDispRect.dy;

                if (pme->m_pPMD->m_pIAnimBitmap) {

                   IDISPLAY_BitBlt(pme->m_pPMD->m_pIDisp, 
                           pme->m_pPMD->m_animDispRect.x - nRectToDestOffsetX, 
                           pme->m_pPMD->m_animDispRect.y - nRectToDestOffsetY, 
                           width, 
                           height, 
                           (const void *) pme->m_pPMD->m_pIAnimBitmap, 
                           pme->m_pPMD->m_animOffsetRect.x, 
                           pme->m_pPMD->m_animOffsetRect.y, 
                           pme->m_pPMD->m_transparency ? AEE_RO_TRANSPARENT : AEE_RO_COPY);
                }
            }
         }
      }
   }

   // Text
   if (pme->m_pPMD->m_textBlock)
   {
      // Process and display PMD text
      CMediaPMD * pPmd = (CMediaPMD *)pme->m_pPMD;

      if(pme->m_pPMD->m_fTextDraw & OEMMM_DISP_DRAW)
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

   if (pme->m_pPMD->m_FrameCallbackEnabled)
   {
      //// Set the display bitmap back to the device bitmap
      IDISPLAY_SetDestination(pme->m_pPMD->m_pIDisp, pDispBmp);
   }
   else
   {
      IDISPLAY_Update(pme->m_pPMD->m_pIDisp);
   }
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


/*==================================================================

==================================================================*/
static int CMediaCMX_SetMediaParm(IMedia * po, int nParmID, int32 p1, int32 p2)
{
   CMediaCMX * pme = (CMediaCMX *)po;
   int         nRet = SUCCESS;


   if (nParmID == MM_PARM_MEDIA_DATA)
   {
#if defined(FEATURE_QTV_DRM_DCF)
      if ( pme->m_pReadRsp.pStream != NULL )
      {
         (void)ixstream_close_sync ( pme->m_pReadRsp.pStream );
         ixstream_delete_stream( pme->m_pReadRsp.pStream );
         pme->m_pReadRsp.pStream = NULL;
      }
#endif // defined(FEATURE_QTV_DRM_DCF)
      return AEEMedia_SetMediaParm(po, nParmID, p1, p2);
   }

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
   cmx_handle_type    h;
   int                nRet = SUCCESS;
   CMXInfo            ci;

#ifdef FEATURE_WEBAUDIO
   int                i;
#endif /* FEATURE_WEBAUDIO*/
   nRet = CMediaPMDMMLayer_SetSource((CMediaCMX *)pme, &h);
   if (nRet != SUCCESS)
      return nRet;

#ifdef FEATURE_WEBAUDIO
   if (pme->m_pPMD)
   {
      CancelCSCallback(pme->m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CALLBACK_Cancel(&pme->m_pPMD->m_cbJPEGBufAlloc);
      CALLBACK_Cancel(&pme->m_pPMD->m_cbJPEGBufFree);
      CancelCSCallback(pme->m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbPicRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbTextRelease, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbPMDText, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbCopyBMPBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbCopyPNGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbCopyJPEGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH);

#if defined(FEATURE_SAF)
      CancelCSCallback(pme->m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbSAFLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif  //defined(FEATURE_SAF)

#if defined(FEATURE_SVG)
      CancelCSCallback(pme->m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbSVGLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH);
#endif //defined(FEATURE_SVG)

      CancelCSCallback(pme->m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH);
      CancelCSCallback(pme->m_pPMD->m_cbAnimReset, OEMMM_MAX_CB_CONTEXT_SWITCH);

      if(pme->m_pPMD->m_pIAnimBitmap) {
         if(pme->m_pPMD->m_pIAnimBitmap->cntRGB)
            FREEIF(pme->m_pPMD->m_pIAnimBitmap->pRGB);
         IDIB_Release(pme->m_pPMD->m_pIAnimBitmap);
         pme->m_pPMD->m_pIAnimBitmap = NULL;
      }

      pme->m_pPMD->m_pIDispBitmap = NULL;

      {
         ContextSwitchCallback cscb;

         cscb.pIMedia = (IMedia*)pme;
         TextRelease((void*)&cscb);
      }

      FreeJPEGBuf(pme);

      for (i=0; i<pme->m_pPMD->m_numClientBufs; ++i)
      {
         int nCopy;

         FREEIF(pme->m_pPMD->m_pClientBufInfos[i].m_buf);
         pme->m_pPMD->m_pClientBufInfos[i].m_size = 0;
         pme->m_pPMD->m_pClientBufInfos[i].m_copycount = 0;
         for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
         {
            FREEIF(pme->m_pPMD->m_pClientBufInfos[i].m_copies[nCopy]);
         }
      }

      pme->m_pPMD->m_displayedPic = FALSE;
      pme->m_pPMD->m_displayedSaf = FALSE;
      pme->m_pPMD->m_transparency = FALSE;
   }
#endif // FEATURE_WEBAUDIO

   // If it is raw data, then select codec and start the playback...
   if (pme->m_bRaw)
      return CMediaPMDMMLayer_PlayCodec(pme, &h);

   if (pme->m_nAPath != MM_APATH_LOCAL_RINGER)
   {

#ifdef FEATURE_MULTISEQUENCER
      /* See if we need to play a multisequence */
      if (pme->m_bChannelShare)
      {
         CMXInfo              ci;
         cmx_audfmt_multi_param_type sParam;

#if defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
         CMediaPMDMMLayer_SetSynthQuality(pme);
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
  
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
         cmx_audfmt_start_enum_type eType;

         eType = CMX_AUDFMT_START_NORMAL;

         ci.nType = OEMCMX_TYPE_AUDFMT_PLAY;
         ci.ph = &h;
         ci.dwCB = (uint32)CMediaPMDMMLayer_PlayCB;
         ci.dwUserData = (uint32)pme->m_hObject;
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_TYPE, (int32)&ci, (int32)eType, 0, TRUE);
         OEMMediaPMD_Start(pme->m_pMediaCMX);
      }
   }

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

   

   if (bResume)
      AEE_ResumeCallback(&pcb->cb, pme->m_pac);

   OEMMM_LEAVE_CRITICAL_SECTION()
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
      if(pme->m_pPMD){
		   pme->m_pPMD->m_char_set = psd->wa_spec.char_set;
      }
   }

#if defined(FEATURE_MULTISEQUENCER)
   if (pme->m_bChannelShare)
   {
      pme->m_pSequence = psd->wa_spec.sequence;
   }
#endif // defined(FEATURE_MULTISEQUENCER)
#endif // defined(FEATURE_WEBAUDIO)
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
#endif // defined(FEATURE_AUDFMT_HYBRID_MODE) && defined(FEATURE_MULTISEQUENCER)
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

   if (!pme || !pme->m_pPMD)
      return;

   if (pme->m_pPMD->m_pJPEGBuf)
      FREE(pme->m_pPMD->m_pJPEGBuf);
      
   pme->m_pPMD->m_pJPEGBuf = (byte *)MALLOC(pme->m_pPMD->m_pJPEGBufSize);

   if(pme->m_pPMD->m_cbJPEGBufAllocNotify != NULL)
   {
      pme->m_pPMD->m_cbJPEGBufAllocNotify(pme->m_pPMD->m_pJPEGBuf,
                                          pme->m_pPMD->m_pJPEGClientData);
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

   if (!pme || !pme->m_pPMD)
      return;

   FREEIF(pme->m_pPMD->m_pJPEGBuf);
   pme->m_pPMD->m_pJPEGBuf = NULL;
   pme->m_pPMD->m_pJPEGBufSize = 0;
   pme->m_pPMD->m_cbJPEGBufAllocNotify = NULL;
   pme->m_pPMD->m_pJPEGClientData = NULL;
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

	if (!pme || !pme->m_pPMD)
	   return;

	// Check if a buffer in the list is unallocated
	for (i=0; i<pme->m_pPMD->m_numClientBufs; ++i)
	{
	   if (!pme->m_pPMD->m_pClientBufInfos[i].m_buf)
	   break;
	}

	// If no buffers in the list are free, expand the list
	if (i == pme->m_pPMD->m_numClientBufs)
	{
	   pNew = REALLOC(pme->m_pPMD->m_pClientBufInfos, sizeof(ClientBufInfo) * (pme->m_pPMD->m_numClientBufs + OEMMM_CLIENT_BUF_INIT_COUNT));
	   if (pNew)
	   {
		  for (n = pme->m_pPMD->m_numClientBufs; n < pme->m_pPMD->m_numClientBufs + OEMMM_CLIENT_BUF_INIT_COUNT; ++n)
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

		  pme->m_pPMD->m_pClientBufInfos = pNew;
		  pme->m_pPMD->m_numClientBufs += OEMMM_CLIENT_BUF_INIT_COUNT;
	   }
	   else
	   {
		  pme->m_pPMD->m_lastBufAllocated = -1;
		  return;
	   }
	}

	// Allocate the buffer in the first free slot
	pme->m_pPMD->m_pClientBufInfos[i].m_buf = MALLOC(pClientBufRequest->nClientBufSize);
	if (pme->m_pPMD->m_pClientBufInfos[i].m_buf)
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
		  pme->m_pPMD->m_pClientBufInfos[i].m_copies[nCopy] = MALLOC(pClientBufRequest->nClientBufSize);
		  if (!pme->m_pPMD->m_pClientBufInfos[i].m_copies[nCopy])
		  {
			 // Free the main buffer
			 FREEIF(pme->m_pPMD->m_pClientBufInfos[i].m_buf);
			 break;
		  }
	   }

	   // If the main buffer has been freed, then fee any copies that may have been allocated
	   if (!pme->m_pPMD->m_pClientBufInfos[i].m_buf)
	   {
		  for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
		  {
			 FREEIF(pme->m_pPMD->m_pClientBufInfos[i].m_copies[nCopy]);
		  }

		  pme->m_pPMD->m_lastBufAllocated = -1;
		  return;
	   }
	   else
	   {
		  pme->m_pPMD->m_pClientBufInfos[i].m_size = pClientBufRequest->nClientBufSize;
		  pme->m_pPMD->m_pClientBufInfos[i].m_copycount = nMaxCopy;
		  pme->m_pPMD->m_lastBufAllocated = i;
	   }
	}

	// Pass the buffer back to cmx
	if(pClientBufRequest->cbClientBufAllocNotify != NULL)
	{
	   pClientBufRequest->cbClientBufAllocNotify(pme->m_pPMD->m_pClientBufInfos[i].m_buf,
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

	if (!pme || !pme->m_pPMD || !pPicStatus)
	   return;

	pme->m_pPMD->m_fPicDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag

	//If the OEM has constants to use for display height and width,
	//they can use it here instead of doing the GetDeviceInfo stuff
	{
	   AEEDeviceInfo  info;

	   ISHELL_GetDeviceInfo(pme->m_pIShell, &info);
	   pme->m_pPMD->m_cxScreen = info.cxScreen;
	   pme->m_pPMD->m_cyScreen = info.cyScreen;

	   if(info.nColorDepth <= 8)
		  pme->m_pPMD->m_clrDepth = 8;
	   else
		  pme->m_pPMD->m_clrDepth = 16;
	}

	pme->m_pPMD->m_fJust = 0;
	FREEIF(pme->m_pPMD->m_pPNGBuf);
	pme->m_pPMD->m_pPNGBufSize = 0;

	switch(pPicStatus->xpos_type) {
	   case CMX_AF_POS_LT:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_LEFT;
		  break;
	   case CMX_AF_POS_RB:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_RIGHT;
		  break;
	   case CMX_AF_POS_CENTER:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_XCENTR;
		  break;
	   case CMX_AF_POS_PERCENT:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_XPCENT;
		  pme->m_pPMD->m_xJustPcent_Pixel = pPicStatus->xpos_percent;
		  break;
	   case CMX_AF_POS_PIXEL:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_XPIXEL;
		  pme->m_pPMD->m_xJustPcent_Pixel = pPicStatus->xpos_percent;
		  break;
	}
	switch(pPicStatus->ypos_type) {
	   case CMX_AF_POS_LT:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_TOP;
		  break;
	   case CMX_AF_POS_RB:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_BOT;
		  break;
	   case CMX_AF_POS_CENTER:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_YCENTR;
		  break;
	   case CMX_AF_POS_PERCENT:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_YPCENT;
		  pme->m_pPMD->m_yJustPcent_Pixel = pPicStatus->ypos_percent;
		  break;
	   case CMX_AF_POS_PIXEL:
		  pme->m_pPMD->m_fJust |= OEMMM_JUST_YPIXEL;
		  pme->m_pPMD->m_yJustPcent_Pixel = pPicStatus->ypos_percent;
		  break;
	}

	pme->m_pPMD->m_bHeaderRead = FALSE;

}

static void PICRelease(void * pUser) {
	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->m_pPMD)
	   return;

	pme->m_pPMD->m_fPicDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag

	if(pme->m_pPMD->m_pIPicBitmap) {
	   if(pme->m_pPMD->m_pIPicBitmap->cntRGB)
		  FREEIF(pme->m_pPMD->m_pIPicBitmap->pRGB);
	   IDIB_Release(pme->m_pPMD->m_pIPicBitmap);
	   pme->m_pPMD->m_pIPicBitmap = NULL;
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

   if (!pme || !pme->m_pPMD)
      return;

   pmdInfo = (CMediaPMD *)pme->m_pPMD;

   // Copy the text info and schedule a resume. 
   // Pre-allocate static storage area fpr 2-3 text info and callbacks.
   // In the resume callback,
   // (1) Convert PMD text to HTML text
   // (2) Instantiate IHTMLViewer and display the text
   // TBD: Cost of using IHTMLViewer vs parsing and displaying here??
   if (!(pme->m_dwEnabledCaps & MM_CAPS_TEXT))
      return;

    // Check for supported language characterset encoding
   if ((pme->m_pPMD->m_char_set != CMX_AF_CHAR_SET_ANSI)&&
	    (pme->m_pPMD->m_char_set !=CMX_AF_CHAR_SET_CHINESE_SIMP) &&
       (pme->m_pPMD->m_char_set !=CMX_AF_CHAR_SET_HANGUL) &&
       (pme->m_pPMD->m_char_set !=CMX_AF_CHAR_SET_SHIFTJIS) &&
       (pme->m_pPMD->m_char_set !=CMX_AF_CHAR_SET_CHINESE_TRAD) &&
       (pme->m_pPMD->m_char_set !=CMX_AF_CHAR_SET_HINDI) )
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
            if(!ConvertToUnicodebuffer(&pTextCtl->buffer[last],&iTxtlen,&pBlock->m_text,pme->m_pPMD->m_char_set))
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
				  if(!ConvertToUnicodebuffer(&pTextCtl->buffer[last],&iTxtlen,&pBlock->m_text,pme->m_pPMD->m_char_set))
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

	if (!pme || !pme->m_pPMD)
	   return;

	//The BMP starts with the BMP Info header.
	pBMPHdr = (WinBMPInfoHdr *) pPicData->buffer;

	if(pBMPHdr->wBitsPerPix != 24)
	   return;

	height = pBMPHdr->dwBIHeight;

	if(height < 0)
	   height = -height;


	if(pme->m_pPMD->m_pIPicBitmap) {
	   if(pme->m_pPMD->m_pIPicBitmap->cntRGB && pme->m_pPMD->m_pIPicBitmap->pRGB)
		  FREE(pme->m_pPMD->m_pIPicBitmap->pRGB);
	   IDIB_Release(pme->m_pPMD->m_pIPicBitmap);
	}


	   IDISPLAY_CreateDIBitmap(pme->m_pPMD->m_pIDisp,
							   (IDIB**)&pme->m_pPMD->m_pIPicBitmap,
							   (uint8) pme->m_pPMD->m_clrDepth,
							   (uint16) pBMPHdr->dwBIWidth,
							   (uint16) height
							   );

	if(pme->m_pPMD->m_pIPicBitmap) {

	   if(pme->m_pPMD->m_clrDepth == 16)
		  pme->m_pPMD->m_pIPicBitmap->nColorScheme = IDIB_COLORSCHEME_565;
	   else
		  pme->m_pPMD->m_pIPicBitmap->nColorScheme = IDIB_COLORSCHEME_332;

	   IDShift_New(pme->m_pIShell, (void**) &pIDShift);

	   if(pIDShift) {
		  AEEDShift dshift;
		  int       rowWidth = (pBMPHdr->dwBIWidth * 3 + 3)&~3;

		  byte     *pSrc = (byte *) pPicData->buffer + sizeof(WinBMPInfoHdr);

		  dshift.bDither = FALSE;
		  dshift.nDstDepth = (uint8) pme->m_pPMD->m_clrDepth;
		  dshift.nSrcDepth = 24;
		  dshift.nWidth  = (uint16) pBMPHdr->dwBIWidth;

		  IDShift_Configure(pIDShift, &dshift);

		  if(pBMPHdr->dwBIHeight > 0) {

			 for(i = 0; i < height; i++)

				IDShift_Convert(pIDShift,
								((byte *)pme->m_pPMD->m_pIPicBitmap->pBmp) + i* pme->m_pPMD->m_pIPicBitmap->nPitch,
								pSrc + (height - i - 1)*rowWidth, TRUE
							   );
		  }
		  else {
			 for(i = 0; i < height; i++)

				IDShift_Convert(pIDShift,
								((byte *)pme->m_pPMD->m_pIPicBitmap->pBmp) + i * pme->m_pPMD->m_pIPicBitmap->nPitch,
								pSrc + i*rowWidth, TRUE
							   );
		  }

		  pme->m_pPMD->m_dispRect.dx = (uint16) pBMPHdr->dwBIWidth;
		  pme->m_pPMD->m_dispRect.dy = (uint16) height;


		  IDShift_Delete(pIDShift);

		  pme->m_pPMD->m_pIDispBitmap = pme->m_pPMD->m_pIPicBitmap;
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
	int                     numColors;
	uint16                  width;
	uint16                  height;
	uint32                * pPal;
	uint8                 * pSAFPal;
	byte                  * pTransp;
	uint32                  i;
	ContextSwitchCallback * pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg           * pme;
	cmx_saf_info_type     * pSAFInfo;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pSAFInfo = (cmx_saf_info_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if (!pme || !pme->m_pPMD)
	   return;

	width  = pSAFInfo->width;
	height = pSAFInfo->height;

	if(pme->m_pPMD->m_pIAnimBitmap) {
	   if(pme->m_pPMD->m_pIAnimBitmap->cntRGB && pme->m_pPMD->m_pIAnimBitmap->pRGB)
		  FREE(pme->m_pPMD->m_pIAnimBitmap->pRGB);
	   IDIB_Release(pme->m_pPMD->m_pIAnimBitmap);
	}

	IDISPLAY_CreateDIBitmap(pme->m_pPMD->m_pIDisp,
							(IDIB **)&pme->m_pPMD->m_pIAnimBitmap,
							(uint8) pSAFInfo->bitdepth, 
							width,
							height
							);
	i = sizeof(int);
	if(i > 4)
	   return;

	if(pme->m_pPMD->m_pIAnimBitmap) {
	   IBitmap *pBitmap = IDIB_TO_IBITMAP (pme->m_pPMD->m_pIAnimBitmap);

	   numColors = pSAFInfo->num_palette_entry;
	   pme->m_pPMD->m_pIAnimBitmap->nPitch = (int16) (width * pSAFInfo->bitdepth + 7)/8;
	   if(numColors){

		  //Get bit depth
		  int tempNumColors = numColors;
		  for (pme->m_pPMD->m_AnimBitDepth=0; tempNumColors>1; ++pme->m_pPMD->m_AnimBitDepth)
			 tempNumColors >>= 1;

		  pme->m_pPMD->m_pIAnimBitmap->pRGB = MALLOC(numColors * 4);
		  if(pme->m_pPMD->m_pIAnimBitmap->pRGB) {
			 pPal = pme->m_pPMD->m_pIAnimBitmap->pRGB;
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
			 pme->m_pPMD->m_pIAnimBitmap->cntRGB = numColors;
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
					pme->m_pPMD->m_transparency = TRUE;
				}
			 }
		  }
		  else
			 pme->m_pPMD->m_pIAnimBitmap->cntRGB = 0;
	   }
	   else {
		  pme->m_pPMD->m_AnimBitDepth = pSAFInfo->bitdepth;
		  switch(pSAFInfo->bitdepth) {
		  case 8:
		  {
			 uint32 i, red, green, blue;
			 pme->m_pPMD->m_pIAnimBitmap->pRGB = MALLOC(256 * 4);
			 if(pme->m_pPMD->m_pIAnimBitmap->pRGB)
			 {
				pme->m_pPMD->m_pIAnimBitmap->cntRGB = 256;
				for (i = 0; i < 256; ++i) {
				   red = (i >> 5) & 0x07;
				   green = (i >> 2) & 0x07;
				   blue = i & 0x03;
				   pme->m_pPMD->m_pIAnimBitmap->pRGB[i] = (((blue * 255 / 3))       |
                                                       ((green * 255 / 7) << 8) |
                                                        (red * 255 / 7) << 16);
				}
			 }
			 break;
		  }
		  case 12:
			 pme->m_pPMD->m_pIAnimBitmap->nColorScheme = IDIB_COLORSCHEME_444;
			 break;

		  case 16:
			 pme->m_pPMD->m_pIAnimBitmap->nColorScheme = IDIB_COLORSCHEME_565;
			 break;

		  case 24:
			 pme->m_pPMD->m_pIAnimBitmap->nColorScheme = IDIB_COLORSCHEME_888;
			 break;
		  }
	   }
	}


	return;
}

/*==================================================================

==================================================================*/
static void SAFLoadFrameToDIB(void * pUser) {
	int     nImgBytes;
	IDIB   *pIDIB;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_saf_data_type *pSAFData;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pSAFData = (cmx_saf_data_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);

	if( NULL != pSAFData ){

	   if (!pme || !pme->m_pPMD)
	      return;

	   pIDIB = pme->m_pPMD->m_pIAnimBitmap;
	   if(!pIDIB || !pSAFData->buffer)
	      return;

	   nImgBytes = pIDIB->cx * pIDIB->cy * (pme->m_pPMD->m_AnimBitDepth/8);
	   MEMCPY(pIDIB->pBmp, pSAFData->buffer, nImgBytes);

	   pme->m_pPMD->m_fAnimDraw |= OEMMM_DISP_CLEAR;
	   pme->m_pPMD->m_pIDispBitmap = pme->m_pPMD->m_pIAnimBitmap;

	   MEMCPY(&pme->m_pPMD->m_dispRect, &pme->m_pPMD->m_rectImage, sizeof(AEERect));
	   pme->m_pPMD->m_dispRect.dx = pme->m_pPMD->m_pIAnimBitmap->cx;
	   pme->m_pPMD->m_dispRect.dy = pme->m_pPMD->m_pIAnimBitmap->cy;

	   SetSAFOffsets(pme, pSAFData);

	   //Update screen
	   CMXImage_Notify(pUser);
	} else { 
           return;
        }
}
#endif  //defined(FEATURE_SAF)

#if defined(FEATURE_SVG) && defined(FEATURE_WEBAUDIO)
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

	pme->m_pPMD->m_curSVGInfo = *pSVGInfo;

	width  = pSVGInfo->width;
	height = pSVGInfo->height;

	if(pme->m_pPMD->m_pIAnimBitmap) {
	   if(pme->m_pPMD->m_pIAnimBitmap->cntRGB && pme->m_pPMD->m_pIAnimBitmap->pRGB)
		  FREE(pme->m_pPMD->m_pIAnimBitmap->pRGB);
	   IDIB_Release(pme->m_pPMD->m_pIAnimBitmap);
	}

	switch(pSVGInfo->color_scheme) {
	   case CMX_COLOR_SCHEME_RGB565:
		  pme->m_pPMD->m_AnimBitDepth = 16;
		  nColorScheme = IDIB_COLORSCHEME_565;
		  break;

	   case CMX_COLOR_SCHEME_0RGB888:
      case CMX_COLOR_SCHEME_ARGB888:
		  pme->m_pPMD->m_AnimBitDepth = 32;
		  //BREW handles 32-bit color schemes using IDIB_COLORSCHEME_888
		  nColorScheme = IDIB_COLORSCHEME_888;
		  break;

	   default:
		  // Unsupported bit depth/color scheme
		  return;
	}

	nResult = IDISPLAY_CreateDIBitmap(pme->m_pPMD->m_pIDisp,
									  (IDIB **)&pme->m_pPMD->m_pIAnimBitmap,
									  (uint8) pme->m_pPMD->m_AnimBitDepth, 
									  width,
									  height
									 );
	i = sizeof(int);
	if(i > 4)
	   return;

	if((nResult == SUCCESS) && (pme->m_pPMD->m_pIAnimBitmap)) {
	   pme->m_pPMD->m_pIAnimBitmap->nPitch = width * (pme->m_pPMD->m_AnimBitDepth / 8);
	   pme->m_pPMD->m_pIAnimBitmap->nColorScheme = nColorScheme;

	   if (pSVGInfo->transparency)
	   {
		  // If standalone SVG, use magenta (0xFF00FF in 0BGR) as transparent color,
		  // otherwise if in pmd file use color passed.
		  IBitmap *pBitmap = IDIB_TO_IBITMAP (pme->m_pPMD->m_pIAnimBitmap);;
		  pme->m_pPMD->m_SVGTransColor = IBITMAP_RGBToNative(pBitmap, pSVGInfo->transparency);
		  IBITMAP_SetTransparencyColor (pBitmap, pme->m_pPMD->m_SVGTransColor);

		  // Set the transparency flag
		  pme->m_pPMD->m_transparency = TRUE;
	   }
	}

	return;
}

/*==================================================================

==================================================================*/
static void SVGLoadFrameToDIB(void * pUser) {
	int     nImgBytes;
	IDIB   *pIDIB;

	ContextSwitchCallback* pcscb = (ContextSwitchCallback *)pUser;
	CMediaPMDBg *   pme;
	cmx_svg_frame_done_type *pFrameData;

	if (!pcscb)
	   return;

	pme = (CMediaPMDBg *)(pcscb->pIMedia);
	pFrameData = (cmx_svg_frame_done_type*)(pcscb->pPayload);

	UnlockCSCallback(pcscb);
	if( pFrameData != NULL )
	{
		if (!pme || !pme->m_pPMD)
		   return;

		pIDIB = pme->m_pPMD->m_pIAnimBitmap;
		if(!pIDIB || !pFrameData->frame_buf)
		   return;

		nImgBytes = pIDIB->cx * pIDIB->cy * (pme->m_pPMD->m_AnimBitDepth/8);
		MEMCPY(pIDIB->pBmp, pFrameData->frame_buf, nImgBytes);

		if (pme->m_pPMD->m_curSVGInfo.transparency)
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

			  IBITMAP_SetPixels(pBmp, iPoint, pPoints, pme->m_pPMD->m_SVGTransColor, AEE_RO_COPY);
			  FREEIF(pPoints);
		   }
		}

		pme->m_pPMD->m_fAnimDraw |= OEMMM_DISP_CLEAR;
		pme->m_pPMD->m_pIDispBitmap = pme->m_pPMD->m_pIAnimBitmap;

		MEMCPY(&pme->m_pPMD->m_dispRect, &pme->m_pPMD->m_rectImage, sizeof(AEERect));
		pme->m_pPMD->m_dispRect.dx = pme->m_pPMD->m_pIAnimBitmap->cx;
		pme->m_pPMD->m_dispRect.dy = pme->m_pPMD->m_pIAnimBitmap->cy;

    	       SetAnimOffsets(pme, &pFrameData->frame_pos);

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
	
}

/*==================================================================

==================================================================*/
static void  SetAnimOffsets(CMediaPMDBg * pme, cmx_af_pos_struct_type *frame_pos) {

   AEERect *pRect;

   if (!pme->m_pPMD)
      return;

   pRect = &(pme->m_pPMD->m_rectImage);

   if( frame_pos != NULL){
     switch(frame_pos->xpos_type) {
        case CMX_AF_POS_LT:
           pme->m_pPMD->m_dispRect.x = pRect->x;
           break;
  
        case CMX_AF_POS_CENTER:
           pme->m_pPMD->m_dispRect.x = pRect->x + (pRect->dx - pme->m_pPMD->m_dispRect.dx)/2; 
           break;
  
        case CMX_AF_POS_RB:
           pme->m_pPMD->m_dispRect.x = pRect->x + pRect->dx - pme->m_pPMD->m_dispRect.dx;
           break;
  
        case CMX_AF_POS_PIXEL:
           pme->m_pPMD->m_dispRect.x = pRect->x + frame_pos->xpos_num;
           break;
  
        case CMX_AF_POS_PERCENT:
           pme->m_pPMD->m_dispRect.x = pRect->x + ((pRect->dx *  frame_pos->xpos_num)/100);
           break;
  
        default:
           pme->m_pPMD->m_dispRect.x = pRect->x + (pRect->dx - pme->m_pPMD->m_dispRect.dx)/2;
     }
  
     //Set display vertical position
     switch(frame_pos->ypos_type) {
        case CMX_AF_POS_LT:
           pme->m_pPMD->m_dispRect.y = pRect->y;
           break;
  
        case CMX_AF_POS_CENTER:
           pme->m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->m_pPMD->m_dispRect.dy)/2;
           break;
  
        case CMX_AF_POS_RB:
           pme->m_pPMD->m_dispRect.y = pRect->y + pRect->dy - pme->m_pPMD->m_dispRect.dy;
           break;
  
        case CMX_AF_POS_PIXEL:
           pme->m_pPMD->m_dispRect.y = pRect->y + frame_pos->ypos_num;
           break;
  
        case CMX_AF_POS_PERCENT:
           pme->m_pPMD->m_dispRect.y = pRect->y + ((pRect->dy *  frame_pos->ypos_num)/100);
           break;
  
        default:
           pme->m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->m_pPMD->m_dispRect.dy)/2;
           break;
     }
   } else {
     pme->m_pPMD->m_dispRect.x = pRect->x;
     pme->m_pPMD->m_dispRect.y = pRect->y;
   }

   if (pme->m_pPMD->m_dispRect.x < pRect->x)
   {
      pme->m_pPMD->m_offsetRect.x = (pRect->x - pme->m_pPMD->m_dispRect.x);
      pme->m_pPMD->m_dispRect.x = pRect->x;
   }
   else
      pme->m_pPMD->m_offsetRect.x = 0;

   if (pme->m_pPMD->m_dispRect.y < pRect->y)
   {
      pme->m_pPMD->m_offsetRect.y = (pRect->y - pme->m_pPMD->m_dispRect.y);
      pme->m_pPMD->m_dispRect.y = pRect->y;
   }
   else
      pme->m_pPMD->m_offsetRect.y = 0;

   MEMCPY (&pme->m_pPMD->m_animDispRect, &pme->m_pPMD->m_dispRect, sizeof (AEERect));
   MEMCPY (&pme->m_pPMD->m_animOffsetRect, &pme->m_pPMD->m_offsetRect, sizeof (AEERect));
   pme->m_pPMD->m_displayedSaf = TRUE;
}
#endif //defined(FEATURE_SVG)


/*==================================================================

==================================================================*/
#if defined(FEATURE_SAF)
static void  SetSAFOffsets(CMediaPMDBg * pme, cmx_saf_data_type *pSAFData) {

   AEERect *pRect;

   if (!pme->m_pPMD)
      return;

   pRect = &(pme->m_pPMD->m_rectImage);

   //Set display horizontal position
   switch(pSAFData->frame_pos.xpos_type) {
      case CMX_AF_POS_LT:
         pme->m_pPMD->m_dispRect.x = pRect->x;
         break;

      case CMX_AF_POS_CENTER:
         pme->m_pPMD->m_dispRect.x = pRect->x + (pRect->dx - pme->m_pPMD->m_dispRect.dx)/2; 
         break;

      case CMX_AF_POS_RB:
         pme->m_pPMD->m_dispRect.x = pRect->x + pRect->dx - pme->m_pPMD->m_dispRect.dx;
         break;

      case CMX_AF_POS_PIXEL:
         pme->m_pPMD->m_dispRect.x = pRect->x + pSAFData->frame_pos.xpos_num;
         break;

      case CMX_AF_POS_PERCENT:
		 pme->m_pPMD->m_dispRect.x = pRect->x + ((pRect->dx *  pSAFData->frame_pos.xpos_num)/100);
         break;

      default:
         pme->m_pPMD->m_dispRect.x = pRect->x + (pRect->dx - pme->m_pPMD->m_dispRect.dx)/2;
   }
   
   //Set display vertical position
   switch(pSAFData->frame_pos.ypos_type) {
      case CMX_AF_POS_LT:
         pme->m_pPMD->m_dispRect.y = pRect->y;
         break;

      case CMX_AF_POS_CENTER:
         pme->m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->m_pPMD->m_dispRect.dy)/2;
         break;

      case CMX_AF_POS_RB:
         pme->m_pPMD->m_dispRect.y = pRect->y + pRect->dy - pme->m_pPMD->m_dispRect.dy;
         break;

      case CMX_AF_POS_PIXEL:
		 pme->m_pPMD->m_dispRect.y = pRect->y + pSAFData->frame_pos.ypos_num;
         break;

      case CMX_AF_POS_PERCENT:
		 pme->m_pPMD->m_dispRect.y = pRect->y + ((pRect->dy *  pSAFData->frame_pos.ypos_num)/100);
         break;

      default:
         pme->m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->m_pPMD->m_dispRect.dy)/2;
         break;
   }

   if (pme->m_pPMD->m_dispRect.x < pRect->x)
   {
      pme->m_pPMD->m_offsetRect.x = (pRect->x - pme->m_pPMD->m_dispRect.x);
      pme->m_pPMD->m_dispRect.x = pRect->x;
   }
   else
      pme->m_pPMD->m_offsetRect.x = 0;

   if (pme->m_pPMD->m_dispRect.y < pRect->y)
   {
      pme->m_pPMD->m_offsetRect.y = (pRect->y - pme->m_pPMD->m_dispRect.y);
      pme->m_pPMD->m_dispRect.y = pRect->y;
   }
   else
      pme->m_pPMD->m_offsetRect.y = 0;

   MEMCPY (&pme->m_pPMD->m_animDispRect, &pme->m_pPMD->m_dispRect, sizeof (AEERect));
   MEMCPY (&pme->m_pPMD->m_animOffsetRect, &pme->m_pPMD->m_offsetRect, sizeof (AEERect));
   pme->m_pPMD->m_displayedSaf = TRUE;

}
#endif  //defined(FEATURE_SAF)


/*==================================================================

==================================================================*/
#if defined(FEATURE_WEBAUDIO) || defined(FEATURE_AUDIO_POSTCARD)
static void  SetImageOffsets(CMediaPMDBg * pme) {

   AEERect *pRect;

   if (!pme->m_pPMD)
      return;

   pRect = &(pme->m_pPMD->m_rectImage);

   switch(pme->m_pPMD->m_fJust & 0x0f) {
      case OEMMM_JUST_LEFT:
         pme->m_pPMD->m_dispRect.x = pRect->x;
         break;

      case OEMMM_JUST_RIGHT:
         pme->m_pPMD->m_dispRect.x = pRect->x + pRect->dx - pme->m_pPMD->m_dispRect.dx;
         break;

      case OEMMM_JUST_XCENTR:
         pme->m_pPMD->m_dispRect.x = (pRect->x + pRect->dx - pme->m_pPMD->m_dispRect.dx)/2; 
         break;

      case OEMMM_JUST_XPCENT:
         pme->m_pPMD->m_dispRect.x = pRect->x + ((pRect->dx - pme->m_pPMD->m_dispRect.dx) * pme->m_pPMD->m_xJustPcent_Pixel)/100;
         break;

      case OEMMM_JUST_XPIXEL:
         pme->m_pPMD->m_dispRect.x = pRect->x + pme->m_pPMD->m_xJustPcent_Pixel;
         break;

      default:
         pme->m_pPMD->m_dispRect.x = pRect->x;
   }

   switch(pme->m_pPMD->m_fJust & 0xf0) {
      case OEMMM_JUST_TOP:
         pme->m_pPMD->m_dispRect.y = pRect->y;
         break;

      case OEMMM_JUST_BOT:
         pme->m_pPMD->m_dispRect.y = pRect->y + pRect->dy - pme->m_pPMD->m_dispRect.dy;
         break;

      case OEMMM_JUST_YCENTR:
         pme->m_pPMD->m_dispRect.y = pRect->y + (pRect->dy - pme->m_pPMD->m_dispRect.dy)/2; 
         break;

      case OEMMM_JUST_YPCENT:
         pme->m_pPMD->m_dispRect.y = pRect->y + ((pRect->dy - pme->m_pPMD->m_dispRect.dy) * pme->m_pPMD->m_yJustPcent_Pixel)/100;
         break;

      case OEMMM_JUST_YPIXEL:
         pme->m_pPMD->m_dispRect.y = pRect->y + pme->m_pPMD->m_yJustPcent_Pixel;
         break;

      default:
         pme->m_pPMD->m_dispRect.y = pRect->y;
   }

   if (pme->m_pPMD->m_dispRect.x < pRect->x)
   {
      pme->m_pPMD->m_offsetRect.x = (pRect->x - pme->m_pPMD->m_dispRect.x);
      pme->m_pPMD->m_dispRect.x = pRect->x;
   }
   else
      pme->m_pPMD->m_offsetRect.x = 0;

   if (pme->m_pPMD->m_dispRect.y < pRect->y)
   {
      pme->m_pPMD->m_offsetRect.y = (pRect->y - pme->m_pPMD->m_dispRect.y);
      pme->m_pPMD->m_dispRect.y = pRect->y;
   }
   else
      pme->m_pPMD->m_offsetRect.y = 0;

   MEMCPY (&pme->m_pPMD->m_picDispRect, &pme->m_pPMD->m_dispRect, sizeof (AEERect));
   MEMCPY (&pme->m_pPMD->m_picOffsetRect, &pme->m_pPMD->m_offsetRect, sizeof (AEERect));
   pme->m_pPMD->m_displayedPic = TRUE;

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

	if (!pme || !pme->m_pPMD)
	   return;

	// Figure out what to do based on combination of
	// pPicData->pic_data and pme->m_pPMD->m_bHeaderRead

	if(!pme->m_pPMD->m_bHeaderRead && pPicData->pic_data == CMX_AF_PIC_DATA_DONE)
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

	   pme->m_pPMD->m_bHeaderRead = TRUE;

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

	   pme->m_pPMD->m_JPEGRect.dx = (int16)width;
	   pme->m_pPMD->m_JPEGRect.dy = (int16)height;

	   if(pme->m_pPMD->m_pIPicBitmap) {
		  if(pme->m_pPMD->m_pIPicBitmap->cntRGB && pme->m_pPMD->m_pIPicBitmap->pRGB)
			 FREE(pme->m_pPMD->m_pIPicBitmap->pRGB);
		  IDIB_Release(pme->m_pPMD->m_pIPicBitmap);
	   }


	   IDISPLAY_CreateDIBitmap(pme->m_pPMD->m_pIDisp,
							   (IDIB**)&pme->m_pPMD->m_pIPicBitmap,
							   (uint8) 24,
							   (int16) width,
							   (int16) height
							   );

	   if(pme->m_pPMD->m_pIPicBitmap) {

		  //We can set the pitch correctly here
		  pme->m_pPMD->m_pIPicBitmap->nPitch = (uint16) width * 3;
		  pme->m_pPMD->m_pIPicBitmap->nColorScheme = IDIB_COLORSCHEME_888;
		  goto Done;
	   }
	   else
		  return;
	}

	if (!pme->m_pPMD->m_pIPicBitmap || !pme->m_pPMD->m_pIPicBitmap->pBmp)
	   return;

	pDest = (byte *)pme->m_pPMD->m_pIPicBitmap->pBmp;
	end = pme->m_pPMD->m_JPEGRect.dx * pme->m_pPMD->m_JPEGRect.dy;

	MEMCPY(pDest, pme->m_pPMD->m_pJPEGBuf, end * 3);

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
	   pme->m_pPMD->m_bHeaderRead = FALSE;
	   pme->m_pPMD->m_pIDispBitmap = pme->m_pPMD->m_pIPicBitmap;
	   MEMCPY(&pme->m_pPMD->m_dispRect, &pme->m_pPMD->m_JPEGRect, sizeof(AEERect));
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

	if (!pme)
	   return;

	// Ignore the text section
	if (pPicData->pic_data == CMX_AF_PIC_DATA_TEXT)
	   return;

	// Copy out the data and append
	oldSize = pme->m_pPMD->m_pPNGBufSize;
	pme->m_pPMD->m_pPNGBufSize += pPicData->num;

	pNewBuf = (byte *)REALLOC(pme->m_pPMD->m_pPNGBuf, pme->m_pPMD->m_pPNGBufSize);
	if (pNewBuf)
	{
	   pme->m_pPMD->m_pPNGBuf = pNewBuf;
	}
	else
	{
	   return;
	}

	MEMCPY(pme->m_pPMD->m_pPNGBuf + oldSize, pPicData->buffer, pPicData->num);

	// If this is an append return, otherwise we're done so process it
	if (pPicData->pic_data == CMX_AF_PIC_DATA_APPEND)
	   return;

	// Read the header
	if (sscanf((const char *)pme->m_pPMD->m_pPNGBuf, "%ld\n%ld %ld\n%ld\n", &colorType, &width, &height, &depth) != 4)
	   return;

	bpp = (depth + 4)/8;

	while (lines--)
	{
	   while (pme->m_pPMD->m_pPNGBuf[index++] != 0x0a)
		  if (index > pme->m_pPMD->m_pPNGBufSize)
			 return;
	}

	do
	{
      if (index > pme->m_pPMD->m_pPNGBufSize)
         return;

	   // Check for subchunks
	   if (!STRNICMP((const char *)&pme->m_pPMD->m_pPNGBuf[index], "tran", 4))
	   {
		  // transparent chunk
		  uint32 size;
		  index += 5;

        if (index > pme->m_pPMD->m_pPNGBufSize)
            return;

		  if(sscanf((const char *)&pme->m_pPMD->m_pPNGBuf[index], "%d", &size) != 1)
			 return;

		  while (pme->m_pPMD->m_pPNGBuf[index++] != 0x0a)
        {
			 if (index > pme->m_pPMD->m_pPNGBufSize)
				return;
        }
		  index += size;
		  continue;
	   }
	   if (!STRNICMP((const char *)&pme->m_pPMD->m_pPNGBuf[index], "bkgd", 4))
	   {
		  // background chunk
		  while (pme->m_pPMD->m_pPNGBuf[index++] != 0x0a)
        {
			 if (index > pme->m_pPMD->m_pPNGBufSize)
				return;
        }
		  continue;
	   }
	   if (!STRNICMP((const char *)&pme->m_pPMD->m_pPNGBuf[index], "plte", 4))
	   {
		  // pallete chunk
		  index += 5;

        if (index > pme->m_pPMD->m_pPNGBufSize)
            return;

		  if (sscanf((const char *)&pme->m_pPMD->m_pPNGBuf[index], "%d", &colorMapSize) != 1)
			 return;

		  while (pme->m_pPMD->m_pPNGBuf[index++] != 0x0a)
        {
			 if (index > pme->m_pPMD->m_pPNGBufSize)
				return;
        }

        if ((index + colorMapSize ) > pme->m_pPMD->m_pPNGBufSize)
            return;

		  colorMap = (byte *)MALLOC(colorMapSize);
		  if (colorMap)
			 MEMCPY(colorMap, &pme->m_pPMD->m_pPNGBuf[index], colorMapSize);

		  index += colorMapSize;
		  continue;
	   }
	   // Done
	   break;
	} while (1);

	// Create the DIB
	if(pme->m_pPMD->m_pIPicBitmap) {
	   if(pme->m_pPMD->m_pIPicBitmap->cntRGB && pme->m_pPMD->m_pIPicBitmap->pRGB)
		  FREE(pme->m_pPMD->m_pIPicBitmap->pRGB);
	   IDIB_Release(pme->m_pPMD->m_pIPicBitmap);
	}

	   if (IDISPLAY_CreateDIBitmap(pme->m_pPMD->m_pIDisp,
								   (IDIB**)&pme->m_pPMD->m_pIPicBitmap,
								   (uint8) depth,
								   (uint16) width,
								   (uint16) height) != SUCCESS)
	   return;

	if(!pme->m_pPMD->m_pIPicBitmap)
	   return;

	pDib = (IDIB *)pme->m_pPMD->m_pIPicBitmap;
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

   if ((index + width*height*bpp) > pme->m_pPMD->m_pPNGBufSize)
      return;

	MEMCPY(pDib->pBmp, &pme->m_pPMD->m_pPNGBuf[index], width*height*bpp);
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
		  /*Maxium value of colorMapSize is 768 so end max value has to be 256
		  */
		  if ( end <= 256) {
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
	}

	pme->m_pPMD->m_dispRect.dx = (uint16)width;
	pme->m_pPMD->m_dispRect.dy = (uint16)height;

	FREEIF(colorMap);
	FREEIF(pme->m_pPMD->m_pPNGBuf);
	pme->m_pPMD->m_pPNGBuf = NULL;
	pme->m_pPMD->m_pPNGBufSize = 0;

	if(pPicData->pic_data == CMX_AF_PIC_DATA_DONE) {
	   pme->m_pPMD->m_pIDispBitmap = pme->m_pPMD->m_pIPicBitmap;
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

	if (!pme || !pme->m_pPMD || !pme->m_pPMD->m_pClientBufInfos || !pClientBufSent)
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

   if (!pme || !pme->m_pPMD)
      return;

   pmdInfo = (CMediaPMD *)pme->m_pPMD;
   
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
      if (pme->m_bChannelShare)
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
      if (pme->m_bChannelShare)
   {
         cmx_audfmt_seq_param_type seqParam;
         seqParam.cmd = CMX_AUDFMT_SEQ_CMD_PAUSE;
         seqParam.num = 0; 
         OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
      }
#endif // FEATURE_MULTISEQUENCER

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
   ISound *       pISound = pme->m_pISound;

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
   if (pme->m_bChannelShare)
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
   if (pme->m_bChannelShare)
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
   if (pme->m_bChannelShare)
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
static int CMediaPMDMMLayer_PlayCodec(CMediaPMDBg * pme, cmx_handle_type * ph)
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
          if (pme->m_bChannelShare)
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
   if (pme->m_pPMD)
   {
	   CancelCSCallback(pme->m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH);
   }
#endif // defined(FEATURE_WEBAUDIO)

#ifdef FEATURE_MULTISEQUENCER
   /* Check if it is a multisequence */
   if (pme->m_bChannelShare)
   {
       cmx_audfmt_seq_param_type   seqParam;
       seqParam.cmd = CMX_AUDFMT_SEQ_CMD_STOP;
       seqParam.num = 0;
       OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_SEQUENCE, (int32)pme->m_pSequence, (int32)&seqParam, 0, TRUE);
   }
#endif  // FEATURE_MULTISEQUENCER

   OEMMediaPMD_Stop(pme->m_pMediaCMX);

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

      case CMX_STARTED:                
         nStatus = MM_STATUS_START; 
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
			 CMXClear((CMediaPMDBg*)pme, CLEAR_REDRAW);
#endif // defined(FEATURE_WEBAUDIO)
         break;

#if defined(FEATURE_SVG) && defined(FEATURE_WEBAUDIO)
      case CMX_SVG_INFO:
         if (data)
         {
            nStatus = MM_STATUS_MEDIA_SPEC;

            if (!pme->m_bStopping)
               CMXImage_Process2((CMediaPMDBg *)pme, status, data, num_bytes);
         }
         break;
         
      case CMX_SVG_FRAME:
         if (!pme->m_bStopping)
         {
			   CMediaPMDBg * pmeBg = (CMediaPMDBg *)pme;

            if (pmeBg->m_pPMD->m_FrameCallbackEnabled)
            {
               nStatus = MM_STATUS_FRAME;
            }
			   nStatus = CMXImage_Process2(pmeBg, status, data, num_bytes);
         }
         break;
#endif // FEATURE_SVG
   }

   return nStatus;
}

#if defined(FEATURE_WEBAUDIO)
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

	if (!data || !pme->m_pPMD || !(pme->m_dwEnabledCaps & MM_CAPS_VIDEO))
	   return nStatus;

	switch (status)
	{
	   case CMX_CLIENT_BUF_FREE:
		  nCallback = LockCSCallback(pme->m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;

		  pme->m_pPMD->m_ClientBufSent[nCallback] = (uint8*)data;
		  pme->m_pPMD->m_cbClientBufFree[nCallback].pPayload = &pme->m_pPMD->m_ClientBufSent[nCallback];

		  AEE_ResumeCallback(&pme->m_pPMD->m_cbClientBufFree[nCallback].cb, pme->m_pac);
		  break;

	   case CMX_CLIENT_BUF_REQ:
	   {
		  cmx_af_server_data_type* psd = (cmx_af_server_data_type*)data;
		  nCallback = LockCSCallback(pme->m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;

		  pme->m_pPMD->m_ClientBufRequest[nCallback].nClientBufSize = psd->client_buf_req.buffer_size;
		  pme->m_pPMD->m_ClientBufRequest[nCallback].cbClientBufAllocNotify = psd->client_buf_req.cb_func;
		  pme->m_pPMD->m_ClientBufRequest[nCallback].pBufClientData = psd->client_buf_req.client_data;

		  pme->m_pPMD->m_cbClientBufAlloc[nCallback].pPayload = &pme->m_pPMD->m_ClientBufRequest[nCallback];

		  psd->client_buf_req.buffer_supplied = TRUE;

		  AEE_ResumeCallback(&pme->m_pPMD->m_cbClientBufAlloc[nCallback].cb, pme->m_pac);
		  break;
	   }

 #if defined(FEATURE_SVG)
	   case CMX_SVG_INFO:
		  //Copy server data and queue callback in user mode
		  nCallback = LockCSCallback(pme->m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;
		  pme->m_pPMD->m_cmx_svg_info[nCallback] = *(cmx_svg_info_type *)data;
		  pme->m_pPMD->m_cbSVGCreateDIB[nCallback].pPayload = &pme->m_pPMD->m_cmx_svg_info[nCallback];
		  AEE_ResumeCallback(&pme->m_pPMD->m_cbSVGCreateDIB[nCallback].cb, pme->m_pac);
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

			 nCallback = LockCSCallback(pme->m_pPMD->m_cbSVGLoadFrame, nMaxCallback, pme);
			 if (nCallback == -1)
				return nStatus;

			 // Shallow copy the structure received
			 pme->m_pPMD->m_SVGFrameData[nCallback] = *pFrameData;

#if (SVGT_VERSION > SVGT_1_1)
          if (!pFrameData->frame_done_cb)
#endif // (SVGT_VERSION > SVGT_1_1)
          {
			    // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
			    if (pFrameData->frame_buf != NULL)
             {
				   MEMCPY(pClientBufInfo->m_copies[nCallback], pFrameData->frame_buf, pClientBufInfo->m_size);
				   pme->m_pPMD->m_SVGFrameData[nCallback].frame_buf = pClientBufInfo->m_copies[nCallback];
             }
          }

			 pme->m_pPMD->m_cbSVGLoadFrame[nCallback].pPayload = &pme->m_pPMD->m_SVGFrameData[nCallback];
			 AEE_ResumeCallback(&pme->m_pPMD->m_cbSVGLoadFrame[nCallback].cb, pme->m_pac);

			 if (pme->m_pPMD->m_FrameCallbackEnabled)
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


	if (!psd || !pme->m_pPMD || !(pme->m_dwEnabledCaps & MM_CAPS_VIDEO))
	   return nStatus;

   switch (status)
   {
      case CMX_PICTURE_STATUS:
         switch(psd->pic_status.pic_status) {
            case CMX_AF_PIC_STATUS_INIT:
               //Copy server data and queue callback in user mode
               nCallback = LockCSCallback(pme->m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
               if (nCallback == -1)
                  return nStatus;
               pme->m_pPMD->m_picStatus[nCallback] = psd->pic_status;
               pme->m_pPMD->m_cbPicInitialize[nCallback].pPayload = &pme->m_pPMD->m_picStatus[nCallback];
               AEE_ResumeCallback(&pme->m_pPMD->m_cbPicInitialize[nCallback].cb, pme->m_pac);
               break;

		    case CMX_AF_PIC_STATUS_CLEAR:
            // Set CLEAR flag, leave prev. DRAW flag
				CMXClear(pme, CLEAR_IMAGE | CLEAR_REDRAW);  
				if (pme->m_pPMD->m_FrameCallbackEnabled)
				   nStatus = MM_STATUS_FRAME;
				break;

			 case CMX_AF_PIC_STATUS_ENABLE:
            // Set DRAW flag, leave prev. CLEAR flag
				CMXChangeDrawStatus(pme, &pme->m_pPMD->m_fPicDraw, OEMMM_DISP_DRAW, OEMMM_DRAW_OP_OR_EQ, TRUE);
				if (pme->m_pPMD->m_FrameCallbackEnabled)
				   nStatus = MM_STATUS_FRAME;
				break;

			 case CMX_AF_PIC_STATUS_DISABLE:
				// Set CLEAR flag, clear DRAW flag
				CMXChangeDrawStatus(pme, &pme->m_pPMD->m_fPicDraw, OEMMM_DISP_CLEAR, OEMMM_DRAW_OP_EQ, TRUE);
				if (pme->m_pPMD->m_FrameCallbackEnabled)
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
					  nCallback = LockCSCallback(pme->m_pPMD->m_cbCopyBMPBuf, nMaxCallback, pme);
					  if (nCallback == -1)
						 return nStatus;
					  pcscb = &pme->m_pPMD->m_cbCopyBMPBuf[nCallback];
				   }
				   break;

				case CMX_AF_PIC_PNG:
				   nCallback = LockCSCallback(pme->m_pPMD->m_cbCopyPNGBuf, nMaxCallback, pme);
				   if (nCallback == -1)
					  return nStatus;
				   pcscb = &pme->m_pPMD->m_cbCopyPNGBuf[nCallback];
				   break;


 #ifdef FEATURE_AUDIO_POSTCARD
				case CMX_AF_PIC_JPEG:
				   nCallback = LockCSCallback(pme->m_pPMD->m_cbCopyJPEGBuf, nMaxCallback, pme);
				   if (nCallback == -1)
					  return nStatus;
				   pcscb = &pme->m_pPMD->m_cbCopyJPEGBuf[nCallback];
				   break;
 #endif /* FEATURE_AUDIO_POSTCARD */

				default:
				   return nStatus;
			 }

			 if (nCallback == -1)
				 return nStatus;
			 // Shallow copy the structure received
			 pme->m_pPMD->m_picData[nCallback] = psd->pic_data;

			 // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
			 if (psd->pic_data.buffer != NULL)
			 {
				MEMCPY(pClientBufInfo->m_copies[nCallback], psd->pic_data.buffer, pClientBufInfo->m_size);
				pme->m_pPMD->m_picData[nCallback].buffer = pClientBufInfo->m_copies[nCallback];
			 }

			 pcscb->pPayload = &pme->m_pPMD->m_picData[nCallback];
			 AEE_ResumeCallback(&pcscb->cb, pme->m_pac);

			 if(psd->pic_data.pic_data == CMX_AF_PIC_DATA_DONE && pme->m_pPMD->m_FrameCallbackEnabled)
				nStatus = MM_STATUS_FRAME;
		  }
		  break;

      case CMX_CLIENT_BUF_FREE:
         CMXImage_Process2(pme, status, psd->client_buf_free, sizeof(uint8));
         break;
         
      case CMX_CLIENT_BUF_REQ:
         CMXImage_Process2(pme, status, psd, sizeof(cmx_af_server_data_type));
         break;

      case CMX_GET_BUFFER:
         //Copy the callback data and queue the allocation to happen in user mode
         pme->m_pPMD->m_pJPEGBufSize = psd->client_buf_req.buffer_size;
         pme->m_pPMD->m_cbJPEGBufAllocNotify = psd->client_buf_req.cb_func;
         pme->m_pPMD->m_pJPEGClientData = psd->client_buf_req.client_data;

         AEE_ResumeCallback(&pme->m_pPMD->m_cbJPEGBufAlloc, pme->m_pac);
         break;

#if defined(FEATURE_SAF)
      case CMX_SAF_INFO:
         //this is where all the SAF frame stuff comes

         //Copy server data and queue callback in user mode
		  nCallback = LockCSCallback(pme->m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
		  if (nCallback == -1)
			 return nStatus;
		  pme->m_pPMD->m_safInfo[nCallback] = psd->saf_info;
		  pme->m_pPMD->m_cbSAFCreateDIB[nCallback].pPayload = &pme->m_pPMD->m_safInfo[nCallback];
		  AEE_ResumeCallback(&pme->m_pPMD->m_cbSAFCreateDIB[nCallback].cb, pme->m_pac);
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

		  nCallback = LockCSCallback(pme->m_pPMD->m_cbSAFLoadFrame, nMaxCallback, pme);
		  if (nCallback == -1)
			 return nStatus;

		  // Shallow copy the structure received
		  pme->m_pPMD->m_safData[nCallback] = psd->saf_data;

		  // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
		  if (psd->saf_data.buffer != NULL)
		  {
			 MEMCPY(pClientBufInfo->m_copies[nCallback], psd->saf_data.buffer, pClientBufInfo->m_size);
			 pme->m_pPMD->m_safData[nCallback].buffer = pClientBufInfo->m_copies[nCallback];
		  }

		  pme->m_pPMD->m_cbSAFLoadFrame[nCallback].pPayload = &pme->m_pPMD->m_safData[nCallback];
		  AEE_ResumeCallback(&pme->m_pPMD->m_cbSAFLoadFrame[nCallback].cb, pme->m_pac);

		  if (pme->m_pPMD->m_FrameCallbackEnabled)
			 nStatus = MM_STATUS_FRAME;

		  break;
#endif  //defined(FEATURE_SAF)

      case CMX_ANIM_CLEAR:
		  // Set CLEAR flag, leave prev. DRAW flag
		  CMXClear(pme, CLEAR_ANIM | CLEAR_REDRAW);
          if (pme->m_pPMD->m_FrameCallbackEnabled)
		    nStatus = MM_STATUS_FRAME;
          break;

      case CMX_ANIM_ENABLE:
		  // Set DRAW flag, leave prev. CLEAR flag
		  CMXChangeDrawStatus(pme, &pme->m_pPMD->m_fAnimDraw, OEMMM_DISP_DRAW, OEMMM_DRAW_OP_OR_EQ, TRUE);         
		  if (pme->m_pPMD->m_FrameCallbackEnabled)
			 nStatus = MM_STATUS_FRAME;
		  break;

      case CMX_ANIM_DISABLE:
		  // Set CLEAR flag, clear DRAW flag
		  CMXChangeDrawStatus(pme, &pme->m_pPMD->m_fAnimDraw, OEMMM_DISP_CLEAR, OEMMM_DRAW_OP_EQ, TRUE);         
		  if (pme->m_pPMD->m_FrameCallbackEnabled)
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
#if defined(FEATURE_PMD_STREAMING)   
   uint32         nLength = 0xFFFFffff;   // Set it to max.
#endif

   if (pmd->clsData == MMD_FILE_NAME)
   {
      ph->efs.source_type = CMX_SOURCE_EFS;
      if (pmd->pData)
         STRLCPY(ph->efs.filename, pMedia->m_pszFile, sizeof(ph->efs.filename));
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
#if defined(FEATURE_PMD_STREAMING)
      CMediaPMDBg *  pBg = (CMediaPMDBg *)pme;

#if defined(FEATURE_QTV_DRM_DCF)
      if ( pme->m_pReadRsp.pStream != NULL )
      {
         // the IxStream is already active; seek to the beginning

         if ( E_SUCCESS != ixstream_seek ( pme->m_pReadRsp.pStream, 0, IX_STRM_SEEK_START ))
         {
            return EBADPARM;
         }
         if ( E_SUCCESS != ixstream_length ( pme->m_pReadRsp.pStream, &nLength, NULL ))
         {
            nLength = 0xFFFFffff;
         }
      }
      else if (( E_SUCCESS == ix_stream_recreate_stream(( ISource * )pmd->pData, & pme->m_pReadRsp.pStream )) &&
          ( pme->m_pReadRsp.pStream != NULL ))
      {
         // this is IxStream with DRM content

         pme->m_pReadRsp.pUserData = ( void * ) pme->m_hObject;
         if ( E_SUCCESS != ixstream_length ( pme->m_pReadRsp.pStream, &nLength, NULL ))
         {
            nLength = 0xFFFFffff;
         }
      }
      else
#endif // defined(FEATURE_QTV_DRM_DCF)
      {
         return EBADPARM;
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
      ph->client.data_len = nLength;
      ph->client.data_req_ptr = CMediaPMDMMLayer_CMXDataRequestCB;

      CMediaPMDMMLayer_ReadSource(( CMediaPMDBg * ) pme );
#else
      nRet = EBADPARM;
#endif // defined(FEATURE_PMD_STREAMING)
   }

   return nRet;
}

#if defined(FEATURE_PMD_STREAMING)
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
static void CMediaPMDMMLayer_DeliverCMXData ( CMediaPMDBg * pme )
{
   int nRead = -1;

   while (1)
   {
      // called only internally, so pme != NULL always
      int nLen = MIN(pme->m_dwSrcBufferSize, pme->m_sReq.dwMaxLen);

      nRead = CQueue_Get(pme->m_pqMediaData, pme->m_pSrcBuffer, nLen);

      FARF(SOURCE, ("===CMXNotify: nReq=%ul nLen=%d", pme->m_sReq.dwMaxLen, nLen));

      OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&pme->m_sReq, (int32)pme->m_pSrcBuffer, (int32)nRead, FALSE);
      pme->m_sReq.bReqPending = FALSE;

      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&pme->m_sReq, sizeof(pme->m_sReq)))
      {
         break;
      }

      pme->m_sReq.bReqPending = TRUE;

      if ( !pme->m_bEOS && CQueue_IsEmpty( pme->m_pqMediaData ))
      {
         FARF(SOURCE, ("===CMXNotify: No media data"));
         CMediaPMDMMLayer_RequestSourceData(pme);
         break;
      }
   }

   if ( pme->m_bEOS && ( 0 == nRead ) && !pme->m_bStopping)
   {
      IMEDIA_Stop(( IMedia * )pme );
   }
}

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_CMXDataReqNotify(CMediaPMDBg * pme)
      {
   if ( NULL == pme )
   {
      return;
      }

   if (!pme->m_sReq.bReqPending)
   {
      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&pme->m_sReq, sizeof(pme->m_sReq)))
      {
         FARF(SOURCE, ("===CMXNotify: Fatal mdq error"));
         return;
      }

      pme->m_sReq.bReqPending = TRUE;

      if ( !pme->m_bEOS && CQueue_IsEmpty( pme->m_pqMediaData ))
      {
         FARF(SOURCE, ("===CMXNotify: No media data"));
         CMediaPMDMMLayer_RequestSourceData(pme);
      }
      else
      {
         CMediaPMDMMLayer_DeliverCMXData ( pme );
      }
   }
}

#if defined(FEATURE_QTV_DRM_DCF)
/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_IxStreamReadAsyncCB ( void * pUser )
{
   IxStreamReadRspT * pReadResp = ( IxStreamReadRspT * ) pUser;
   CMediaCMX        * pme = ( CMediaCMX * ) AEEObjectMgr_GetObject (( AEEObjectHandle ) pReadResp->pUserData );

   if ( NULL == pme )
   {
      return;
   }

   if ( pme->m_bCancelRead )
   {
      pme->m_sReq.bReqPending = FALSE;
      pme->m_bCancelRead = FALSE;

      // kick off a new read if needed
      if ( !CQueue_IsEmpty ( pme->m_pqCMXDataReq ))
      {
         AEE_ResumeCallback(pme->m_pcbCMXDataReq, pme->m_pac);
      }
   }
   else if ( pme->m_sReq.bReqPending )
   {
      if ( E_SUCCESS != pReadResp->error )
      {
         pme->m_bEOS = TRUE;
      }
      else if ( !CQueue_Put ( pme->m_pqMediaData, pme->m_pSrcBuffer, pReadResp->nBytesRead ))
      {
         FARF( SOURCE, ( "===ReadSrc: Put error nRead=%l", pReadResp->nBytesRead ));
         return;
      }

      FARF( SOURCE, ( "===ReadSrc: Proc CMXRq" ));

      CMediaPMDMMLayer_DeliverCMXData (( CMediaPMDBg * ) pme );
   }
}
#endif // defined(FEATURE_QTV_DRM_DCF)

/*==================================================================

==================================================================*/
static void CMediaPMDMMLayer_ReadSource(CMediaPMDBg * pme)
{
   int32       nRead;
   boolean     bNotify = FALSE;

   while (1)
   {
      if (CQueue_IsFull(pme->m_pqMediaData) ||
          !CQueue_IsPutOk(pme->m_pqMediaData, (int32)pme->m_dwSrcBufferSize) || 
          !pme->m_sReq.bReqPending)
      {
         break;
      }

      nRead = 0;
#if defined(FEATURE_QTV_DRM_DCF) 
      if ( E_SUCCESS == ixstream_seek ( pme->m_pReadRsp.pStream, (const int32) pme->m_sReq.dwPos, IX_STRM_SEEK_START ))
      {
         if ( pme->m_sReq.dwMaxLen > pme->m_dwSrcBufferSize )
         {
            pme->m_sReq.dwMaxLen = pme->m_dwSrcBufferSize;
         }

         if ( E_AGAIN == ixstream_read_async ( pme->m_pReadRsp.pStream, (byte *) pme->m_pSrcBuffer, pme->m_sReq.dwMaxLen, (uint32 *) &nRead, (IxStreamCbT) CMediaPMDMMLayer_IxStreamReadAsyncCB, & pme->m_pReadRsp ))
         {
            break;
         }
      }
      if (0 == nRead)
      {
         FARF(SOURCE, ("===ReadSrc: EOS %d", nRead));
         pme->m_bEOS = TRUE;
         break;
      }
#else
      FARF(SOURCE, ("===ReadSrc: Random access ISource not supported"));
      break; // not supported
#endif // defined(FEATURE_QTV_DRM_DCF)

      if ( !CQueue_Put ( pme->m_pqMediaData, pme->m_pSrcBuffer, nRead ))
      {
         FARF(SOURCE, ("===ReadSrc: Put error nRead=%l", nRead));
         break;
      }

      bNotify = TRUE;

#if defined(FEATURE_QTV_DRM_DCF) 
      if ( pme->m_pReadRsp.pStream != NULL )
      {
         break; // something is wrong or we read to the EOF
      }
#endif // defined(FEATURE_QTV_DRM_DCF) 
   }

   if (bNotify  && pme->m_sReq.bReqPending)
   {
      FARF(SOURCE, ("===ReadSrc: Proc CMXRq"));
      CMediaPMDMMLayer_DeliverCMXData(pme);
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
      if (!CQueue_Get(pme->m_pqCMXDataReq, (uint8 *)&pme->m_sReq, sizeof(pme->m_sReq)))
         break;

      OEMMediaPMD_SetParm(pme->m_pMediaCMX, OEMCMX_ID_DATA_RESPONSE, (int32)&pme->m_sReq, (int32)NULL, (int32)0, FALSE);
//      req.pfnDataReq(CMX_SUCCESS, NULL, 0, pme->m_sReq.pServerUserData);
   }

   pme->m_bEOS = FALSE;
#if defined(FEATURE_QTV_DRM_DCF)
   if ( pme->m_sReq.bReqPending )
   {
      pme->m_bCancelRead = TRUE;
   }
#else
   pme->m_sReq.bReqPending = FALSE;
#endif // defined(FEATURE_QTV_DRM_DCF)
}
#endif // defined(FEATURE_PMD_STREAMING)

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
      case CMX_STARTED:
         nStatus = CMediaPMDMMLayer_GetMMStatus2(pme, status, NULL, 0, peStatus);
         break;

#if defined(FEATURE_SVG) && defined (FEATURE_WEBAUDIO)  // SVG inside PMD
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
	  if (!pme->m_bStopping && psd && ((CMediaPMDBg*)pme)->m_pPMD)
	  {
		 CMediaPMDBg* pmeBg = (CMediaPMDBg*)pme;
		 int nCallback;
		 ClientBufInfo* pClientBufInfo = NULL;
		 int nMaxCallback;


       if (((CMX_AF_TEXT_CTL_TITLE  == psd->text_ctl.text_ctl)  ||
            (CMX_AF_TEXT_CTL_NEW    == psd->text_ctl.text_ctl)  ||
            (CMX_AF_TEXT_CTL_APPEND == psd->text_ctl.text_ctl)) && 
           (psd->text_ctl.buffer && pmeBg->m_md.clsData != MMD_BUFFER))
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

		 nCallback = LockCSCallback(pmeBg->m_pPMD->m_cbPMDText, nMaxCallback, pmeBg);
		 if (nCallback == -1)
			break;

		 // Shallow copy the structure received
		 pmeBg->m_pPMD->m_textCtl[nCallback] = psd->text_ctl;

		 // Make a copy of the data buffer member and set copied struct's pointer to the copied buffer
		 if ((CMX_AF_TEXT_CTL_TITLE  == psd->text_ctl.text_ctl)  ||
           (CMX_AF_TEXT_CTL_NEW    == psd->text_ctl.text_ctl)  ||
           (CMX_AF_TEXT_CTL_APPEND == psd->text_ctl.text_ctl))
		 {
           if (pmeBg->m_md.clsData != MMD_BUFFER)
           {
			     MEMCPY(pClientBufInfo->m_copies[nCallback], psd->text_ctl.buffer, pClientBufInfo->m_size);
			     pmeBg->m_pPMD->m_textCtl[nCallback].buffer = pClientBufInfo->m_copies[nCallback];
           }
           else
           {
              pmeBg->m_pPMD->m_textCtl[nCallback].buffer = psd->text_ctl.buffer;
           }
		 }

		 pmeBg->m_pPMD->m_cbPMDText[nCallback].pPayload = &pmeBg->m_pPMD->m_textCtl[nCallback];
		 AEE_ResumeCallback(&pmeBg->m_pPMD->m_cbPMDText[nCallback].cb, pmeBg->m_pac);

		 if (pmeBg->m_pPMD->m_FrameCallbackEnabled)
		 {
			nStatus = MM_STATUS_FRAME;
		 }
	  }
	  break;

      case CMX_CLIENT_BUF_FREE:
      case CMX_CLIENT_BUF_REQ:
      case CMX_GET_BUFFER:
      case CMX_PICTURE_STATUS:       
      case CMX_PICTURE_DATA:
         if (!pme->m_bStopping)
            nStatus = CMXImage_Process((CMediaPMDBg *)pme, status, psd);
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
         {
            pme->m_eType = (int16)p2;
         }
         else if (OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
         {
            pme->m_eType = (int16)p2;
            pme->m_dwPauseMS = (uint32)p3;
         }   

#ifdef FEATURE_MULTISEQUENCER
         else if (OEMCMX_TYPE_AUDFMT_PLAY_MULTI == pme->m_ciStart.nType)
         {
            pme->m_pMultiParam = (cmx_audfmt_multi_param_type *)p2;
         }
#endif // FEATURE_MULTISEQUENCER         

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

         nErr = EUNSUPPORTED;
         break;
         
      case OEMCMX_ID_PAN:
         pme->m_wPan = (uint16)p1;
         if (bLocal)
            break;

         if (OEMCMX_TYPE_AUDFMT_PLAY == pme->m_ciStart.nType || OEMCMX_TYPE_AUDFMT_RINGER == pme->m_ciStart.nType)
            cmx_audfmt_pan((uint32)pme->m_wPan, pfn, pUser);
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

#if defined(FEATURE_PMD_STREAMING)
      case OEMCMX_ID_DATA_RESPONSE:
         if (!bLocal)
         {
            CMXDataRequest *  pReq = (CMXDataRequest *)p1;
            uint8 *           pBuffer = (uint8 *)p2;
            uint32            dwBufSize = (uint32)p3;

            pReq->pfnDataReq(CMX_SUCCESS, pBuffer, dwBufSize, pReq->pServerUserData);
         }
         break;
#endif // defined(FEATURE_PMD_STREAMING)

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
         cmx_audfmt_stop(pfn, pUser);
         break;

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_sequence_cmd(pme->m_pSequence, pme->m_pSeqParam, 
                                 pfn, pUser);
         break;
#endif // FEATURE_MULTISEQUENCER

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

#ifdef FEATURE_MULTISEQUENCER
      case OEMCMX_TYPE_AUDFMT_PLAY_MULTI:
         cmx_audfmt_play_multi(pme->m_ciStart.ph, pme->m_pMultiParam, 
                               (cmx_audfmt_play_cb_func_ptr_type)pme->m_ciStart.dwCB, 
                               pUser);
         break;
#endif //FEATURE MULTISEQUENCER

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

   AEEMedia_Init(ps, MT_VIDEO_PMD, AEECLSID_MEDIAPMD);
   AEEMedia_Init(ps, "video/mld", AEECLSID_MEDIAPMD);

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
      eMethod = AEE_SOUND_METHOD_MIDI;
      ISOUND_Get(pme->m_pISound, &si);
      si.eMethod = eMethod;
      ISOUND_Set(pme->m_pISound, &si);

      {
#if defined(FEATURE_WEBAUDIO)
         AEEDeviceInfo  di;

         pme->m_pPMD = MALLOC(sizeof(CMediaPMD));
         if (!pme->m_pPMD)
            goto Done;
          
         ISHELL_GetDeviceInfo(pme->m_pIShell, &di);
         pme->m_pPMD->m_rectImage.dx = di.cxScreen;
         pme->m_pPMD->m_rectImage.dy = di.cyScreen;
         pme->m_pPMD->m_rectText.dx = di.cxScreen;
         pme->m_pPMD->m_rectText.dy = di.cyScreen;
#ifdef FEATURE_CMX_SCREEN_DIMENSIONS
         cmx_audfmt_set_screen_size( di.cxScreen, di.cyScreen); 
#endif /* FEATURE_CMX_SCREEN_DIMENSIONS */

         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH, CMXImage_Notify);
         CALLBACK_Init(&pme->m_pPMD->m_cbJPEGBufAlloc, AllocJPEGBuf, pme);
         CALLBACK_Init(&pme->m_pPMD->m_cbJPEGBufFree, FreeJPEGBuf, pme);
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbClientBufAlloc, OEMMM_MAX_CB_CONTEXT_SWITCH, AllocClientBuf);
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbClientBufFree, OEMMM_MAX_CB_CONTEXT_SWITCH, FreeClientBuf);
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbPicInitialize, OEMMM_MAX_CB_CONTEXT_SWITCH, PICInitialize);
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbPicRelease, OEMMM_MAX_CB_CONTEXT_SWITCH, PICRelease);
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbTextRelease, OEMMM_MAX_CB_CONTEXT_SWITCH, TextRelease);               
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbPMDText, OEMMM_MAX_CB_CONTEXT_SWITCH, ProcessPMDText);               
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbCopyBMPBuf, OEMMM_MAX_CB_CONTEXT_SWITCH, BMPCopyBuffer);               
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbCopyPNGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH, PNGCopyBuffer);               
#ifdef FEATURE_AUDIO_POSTCARD
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbCopyJPEGBuf, OEMMM_MAX_CB_CONTEXT_SWITCH, JPEGCopyBuffer);               
#endif
#if defined(FEATURE_SAF)
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbSAFCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, SAFCreateDIB);               
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbSAFLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH, SAFLoadFrameToDIB);               
#endif  //defined(FEATURE_SAF)
#if defined(FEATURE_SVG)
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbSVGCreateDIB, OEMMM_MAX_CB_CONTEXT_SWITCH, SVGCreateDIB);               
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbSVGLoadFrame, OEMMM_MAX_CB_CONTEXT_SWITCH, SVGLoadFrameToDIB);               
#endif //defined(FEATURE_SVG)
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH, CMXChangeDrawStatusFlag);
         InitCSCallback((IMedia *)pme, pme->m_pPMD->m_cbAnimReset, OEMMM_MAX_CB_CONTEXT_SWITCH, AnimReset);

         pme->m_pPMD->m_numToWipe = 0;
         pme->m_pPMD->m_textBlock = NULL;
         MEMSET(pme->m_pPMD->m_textCtl, 0, sizeof(cmx_af_text_ctl_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
         MEMSET(pme->m_pPMD->m_picData, 0, sizeof(cmx_af_pic_data_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
         MEMSET(pme->m_pPMD->m_picStatus, 0, sizeof(cmx_af_picture_status_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
#if defined(FEATURE_SAF)
         MEMSET(pme->m_pPMD->m_safInfo, 0, sizeof(cmx_saf_info_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);
         MEMSET(pme->m_pPMD->m_safData, 0, sizeof(cmx_saf_data_type) * OEMMM_MAX_CB_CONTEXT_SWITCH);            
#endif  //defined(FEATURE_SAF)
         MEMSET(pme->m_pPMD->m_drawInfo, 0, sizeof(DrawInfo) * OEMMM_MAX_CB_CONTEXT_SWITCH);

         pme->m_pPMD->m_pPNGBuf = NULL;
         pme->m_pPMD->m_pPNGBufSize = 0;
         pme->m_pPMD->m_pJPEGBuf = NULL;
         pme->m_pPMD->m_pJPEGBufSize = 0;
         pme->m_pPMD->m_cbJPEGBufAllocNotify = NULL;
         pme->m_pPMD->m_pJPEGClientData = NULL;
         pme->m_pPMD->m_fTextDraw = OEMMM_DISP_DRAW;
         pme->m_pPMD->m_pIAnimBitmap = NULL;
         pme->m_pPMD->m_pIPicBitmap = NULL;
         pme->m_pPMD->m_pIDispBitmap = NULL;
         pme->m_pPMD->m_displayedPic = FALSE;
         pme->m_pPMD->m_displayedSaf = FALSE;
         pme->m_pPMD->m_transparency = FALSE;

         pme->m_pPMD->m_fPicDraw = OEMMM_DISP_CLEAR | OEMMM_DISP_DRAW;
         pme->m_pPMD->m_fAnimDraw = OEMMM_DISP_CLEAR | OEMMM_DISP_DRAW;

         if (ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_DISPLAY, (void **) &pme->m_pPMD->m_pIDisp))
         {
            FREEIF(pme->m_pPMD);
            goto Done;
         }
            
         pme->m_pPMD->m_pClientBufInfos = MALLOC(sizeof(ClientBufInfo) * OEMMM_CLIENT_BUF_INIT_COUNT);
         if (pme->m_pPMD->m_pClientBufInfos)
         {
            int nBuf;

            for (nBuf = 0; nBuf < OEMMM_CLIENT_BUF_INIT_COUNT; ++nBuf)
            {
               int nCopy;

               pme->m_pPMD->m_pClientBufInfos[nBuf].m_buf = NULL;
               pme->m_pPMD->m_pClientBufInfos[nBuf].m_size = 0;
               pme->m_pPMD->m_pClientBufInfos[nBuf].m_copycount = 0;
               for (nCopy = 0; nCopy < OEMMM_MAX_CB_CONTEXT_SWITCH; ++nCopy)
               {
                  pme->m_pPMD->m_pClientBufInfos[nBuf].m_copies[nCopy] = NULL;
               }
            }
         }
         else            
         {
            MM_RELEASEIF(pme->m_pPMD->m_pIDisp);
            FREEIF(pme->m_pPMD);
            goto Done;
         }
         pme->m_pPMD->m_numClientBufs = OEMMM_CLIENT_BUF_INIT_COUNT;
         pme->m_pPMD->m_lastBufAllocated = -1;
         MEMSET (pme->m_pPMD->m_ClientBufSent, 0, sizeof(uint8*) * OEMMM_MAX_CB_CONTEXT_SWITCH);
         MEMSET (pme->m_pPMD->m_ClientBufRequest, 0, sizeof(ClientBufRequestInfo) * OEMMM_MAX_CB_CONTEXT_SWITCH);

         pme->m_pPMD->m_FrameCallbackEnabled = FALSE;
         pme->m_pPMD->m_pCallerFrameBmp = NULL;
         pme->m_pPMD->m_DispAreaChanged = FALSE;
#else
         nRet = EUNSUPPORTED;
         goto Done;
#endif // defined(FEATURE_WEBAUDIO)
      }

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
   *ppif = NULL;

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
         pme->m_pPMD->m_nCallbacksSkipped = 0;
         return i;
      }
   }

   ++pme->m_pPMD->m_nCallbacksSkipped;
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
            pcscb = pme->m_pPMD->m_cbPicRelease;
            break;

         case CLEAR_ANIM:
            pcscb = pme->m_pPMD->m_cbAnimReset;
            break;

         case CLEAR_TEXT:
            pcscb = pme->m_pPMD->m_cbTextRelease;
            break;

         case CLEAR_REDRAW:
            pcscb = pme->m_pPMD->m_cbPMDImage;
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

   nCallback = LockCSCallback(pme->m_pPMD->m_cbChangeDrawStatus, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
   if (nCallback == -1)
      return;
   pme->m_pPMD->m_drawInfo[nCallback].pDrawFlag = pDrawFlag;
   pme->m_pPMD->m_drawInfo[nCallback].nStatus = nStatus;
   pme->m_pPMD->m_drawInfo[nCallback].eOp = eOp;
   pme->m_pPMD->m_cbChangeDrawStatus[nCallback].pPayload = &pme->m_pPMD->m_drawInfo[nCallback];
   AEE_ResumeCallback(&pme->m_pPMD->m_cbChangeDrawStatus[nCallback].cb, pme->m_pac);

   if (bRedraw)
   {
      nCallback = LockCSCallback(pme->m_pPMD->m_cbPMDImage, OEMMM_MAX_CB_CONTEXT_SWITCH, pme);
      if (nCallback == -1)
         return;
      AEE_ResumeCallback(&pme->m_pPMD->m_cbPMDImage[nCallback].cb, pme->m_pac);
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

   if (!pme || !pme->m_pPMD || !pDrawInfo)
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

   if (!pme || !pme->m_pPMD)
      return;

   pme->m_pPMD->m_fAnimDraw |= OEMMM_DISP_CLEAR;  // Set CLEAR flag, leave prev. DRAW flag
   pme->m_pPMD->m_displayedSaf = FALSE;
}

/*==================================================================
Checks if the address passed in matches one of the client-allocated
buffers.  Returns a pointer to the matching ClientBufInfo data, or
NULL if no match.
==================================================================*/
static ClientBufInfo* GetClientBufInfo(CMediaPMDBg * pme, uint8 * pClientBuf)
{
   int i;

   if (!pme || !pme->m_pPMD || !pme->m_pPMD->m_pClientBufInfos)
      return NULL;

   // Check if a buffer in the list matches the one passed
   for (i=0; i<pme->m_pPMD->m_numClientBufs; ++i)
   {
      if (pme->m_pPMD->m_pClientBufInfos[i].m_buf == pClientBuf)
      break;
   }
   
   // If no buffers in the list matched, return NULL
   if (i == pme->m_pPMD->m_numClientBufs)
      return NULL;

   return &pme->m_pPMD->m_pClientBufInfos[i];
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

#if defined(FEATURE_ACM)
void CMediaPMD_GetResourceCntlData(AEEMedia *po, uint32 *acmCB)
{
   *acmCB = (uint32)CMediaPMD_AcmPlayCB;
}

static int CMediaPMD_AcmPlayCB(ACM_Callback_data_type *po)
{
   AEEMedia* pAEEMedia = (AEEMedia *)(po->pdata);
   int32 nRet = SUCCESS;

   if (po->state == ACM_RESOURCE_ACQUIRED)
   {
      // ACM DELAYED ACQUIRE
      // start playing
      pAEEMedia->m_bResOwner = TRUE;
      nRet = CMediaPMDMMLayer_Play((CMediaPMDBg *)pAEEMedia);

      if (nRet != SUCCESS)
      {
         AEEMedia_CleanUpResource(pAEEMedia);
      }
   }
   else
   {
      // ACM FORCE RELEASE
      // update AEEMedia state for ACM resource owner and stop
      if(pAEEMedia->m_bResOwner == TRUE)
      {
         // Protect against getting another force release while stopping 
         pAEEMedia->m_bResOwner = FALSE;
         pAEEMedia->m_bForceRelease = TRUE;
         nRet = CMediaPMDBg_Stop((IMedia *)pAEEMedia);
      }
   }
   return nRet;
}
#endif // FEATURE_ACM

#endif // defined(FEATURE_WEBAUDIO)

