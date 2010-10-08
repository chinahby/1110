/*======================================================
FILE:  OEMJPEGViewer.c

SERVICES:  OEM JPEG Viewer Interface

GENERAL DESCRIPTION:
    AEE JPEG Viewer Interface. This file is for distribution
   with DMSS Porting kit only.

  This interface is derived from the IImage class and provides
  methods to view JPEG images. The JPEG images
  are read from a stream into a buffer using the COEMJPEGViewer_SetStream
  method. The JPEG is read from a stream and converted to bitmap using QCT's JPEG
  decoder.

PUBLIC CLASSES AND STATIC FUNCTIONS:

                    COEMJPEGViewer_AddRef
                    COEMJPEGViewer_Release
                    COEMJPEGViewer_GetInfo
                    COEMJPEGViewer_SetParm
                    COEMJPEGViewer_Draw
                    COEMJPEGViewer_DrawFrame
                    COEMJPEGViewer_Start
                    COEMJPEGViewer_Stop
                    COEMJPEGViewer_SetStream
                    COEMJPEGViewer_Notify
                    COEMJPEGViewer_HandleEvent


        Copyright © 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/oem/imagepro/rel/3.1.5/latest/src/OEMJPEG.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/09    KC     Added code for decode abort functionality and removed deprecated FS APIs
12/05/08   KC      Replacing all FS API's with EFS API's as the former ones are deprecated
10/14/08  chai   Fixed Rvct warnings on ULC
04/10/08   KC      Fixed KLock errors. 
03/25/08   KC     Adding Geo_tagging feature.
02/14/08   KV      Small Correction in JPEG_CreateIBitmap.
10/25/07   KV      Added support for JPEG_COLOR_FORMAT_YCRCB420LP and 
                   JPEG_COLOR_FORMAT_YCRCB422LP
07/27/07   arv     Fixed the issue when continuous jpeg decoding and abort can corrrupt the
                   jpeg state machine                     
05/07/07   arv     Default to ARM decoder if Video/Audio DSP resource is utilized
02/07/07   arv     Fixed the issue related to decoding thumbnails using Java app
12/05/06   arv     Added support for FEATURE_JPEGD_RESIZE.
10/22/06   arv     Cleaned up code referencing CMX image buffer
10/06/06   arv     Fixed memory leak during cancellation of decoding session
10/04/06   arv     Fixed the issue related to exif returning success even incase
                   of invalid JFIF/EXIF
10/03/06   arv     Fixed the issue related to memory leak in EXIF
09/25/06   arv     Merged changes so as to share it across all MSMs
09/13/06   arv     Enabled standalone ARM based decoding feature
07/11/06   arv     Added jpeg_cancel_decode function to make sure that decoder
                   goes to IDLE state when the app cancels the decoding session
                   Fixes the graph overflow buffer issue
07/07/06   NC      Fixed compiler warning if FEATURE_ACM is not defined.
07/07/06   NC      Fixed double free on failing to malloc a buffer.
05/18/06   HV      Added RGB888 support and fixed lint errors
10/31/05   pdo     Fixed the memory stream interface for memory stream jpeg decoder
10/01/05   arv     Fixed the memory leak in extractThumbnail()
09/30/05   jlg     Gracefully handle multiple decode sessions.
09/30/05   kln     Added a call to free exif structure dynamic buffers
09/14/05   dle     Make sure no memory leak in _extractThumbnail().
08/04/05   pdo     Changed memory allocation method for extract thumbnail.
06/30/05   jn      Support IPARM_GETBITMAP in IIMAGE_SetParm.
05/27/05   jn      Allow Jpeg Viewer to be instantiated with 6 new class ids.
05/10/05   jn      Added OEMJPEG_SetExifAsciiValue().
05/02/05   jn      Added OEMJPEG_GetExifValue().
04/25/05   jn      Release bit stream as soon as reading is done to allow IImage
                   instance alive so that exif info can be retrieved.
                   Exif info is read for main images as well as thumbnail.
03/09/05   jn      Featurized YCbCr support under FEATURE_JPEG_DECODER_REV2.
02/08/05   jn      Added support for decoder to output YCbCr format.
01/13/05   pdo     Removed T_MSM6550 directive since DSP decoder is now available
11/04/04   ark     Changed jpeg_ARM_based_decode() and graph_jpeg_decode() for
                   proper playback of PMD files
08/13/04   pdo     Reflects 2.1 changes with JFIF support and removed EXIF
06/08/04   pdo     Featurized the JPEG decoder engine for MSM6550
06/04/04   pdo     Added conditional check for videophone
05/13/04   pdo     Added in-call JPEG decoding capability
05/03/04   pdo     Removed JPEG_setBuffer and
                      removed COEMJPEGViewer_FreeData() in ...Viewer_SetStream
04/16/04   pdo     Removed DSHIFT since BREW3.0 no longer support that.
                   Only output of RGB565 is supported from this version on.
02/12/04    st     Modified code to use new graph_jpeg_set_buf() and
                   JPEG_CMX_GET_BUF command APIs.
12/04/03   pdo     Added supports for RGB565 output from DSP.

===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
//lint -e740 suppress errors on pointer cast
//lint -e611

#include "OEMFeatures.h"


#ifdef FEATURE_JPEGD_CONCURRENCY 
#include "clkrgm_msm.h"
#endif

#ifdef FEATURE_JPEG_DECODER
#include "graph.h"
#include "cmph.h"
#include "jpeg.h"
#include "OEMObjectMgr.h"
#include "AEE.h"
#include "AEE_OEM.h"
#include "AEEMimeTypes.h"
#include "OEMClassIDs.h"
#include "AEE_OEMDispatch.h"
#include "AEEModTable.h"
#include "JPEG.BID"
#include "AEEImageExtFunc.h"
#include "OEMImageParmExt.h"
#include "exif.h"
#include "IIPL.h"
#include "IPL.BID"

#ifdef FEATURE_BREW_3_0
#include "OEMOS.h"
#include "AEEStdLib.h"
#include "OEMCriticalSection.h"
#endif

#ifdef FEATURE_QVPHONE
#include "AEEVideoPhone.h"
#endif

#ifdef FEATURE_GEOTAGGING
#include "OEMFS.h"
#include "AEEFile.h"
#endif /* FEATURE_GEOTAGGING */
#ifdef FEATURE_JPEG_DECODER_REV2
#include "IYCbCr.h"
#include "YCbCr_priv.h"
#endif // FEATURE_JPEG_DECODER_REV2

#if defined (FEATURE_ACM)
#include "OEMACM.h"
#if defined (FEATURE_ODM_UI)
#include "OEMAppResCtl.h"
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

#include "Fs_public.h"


/*=========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*============================================================================
*                         DATA DECLARATIONS
============================================================================*/
//
// Module method declarations...
//

//Turn this define ON if you want to allocate a decode buffer instead of using
//the buffer in CMX.
//#define      JPEG_ALLOC_DECODE_BUF   1

#define JPEG_DEFAULT_BUFSIZE 4096
#define JPEG_BUF_INC_SIZE 2048
#define JPEG_THUMBNAIL_BUFSIZE 76800 //(320*240)


#ifdef FEATURE_JPEGD_CONCURRENCY
extern boolean clk_regime_is_on (clk_regime_type regime);
extern boolean voc_is_active(void);
#endif

#ifdef FEATURE_JPEG_DECODER_REV2
extern boolean jpeg_decoding;
#endif

#ifdef FEATURE_GEOTAGGING
#define OEM_BREW_DIR     "brew"
char *geotag_filename;
boolean Isgeotag=0;
#endif /* FEATURE_GEOTAGGING */

typedef struct _COEMJPEGViewer  COEMJPEGViewer;

typedef struct _COEMJPEGViewerSys
{
  COEMJPEGViewer  *pOEMJPEGViewer;
#ifdef FEATURE_JPEG_DECODER_REV2
  IYCbCr          *m_pIYCbCr;        // YCbCr image data
#endif
  IDIB            *m_pIDIB;          // Raw Bitmap image
  byte            *m_pRGBBuf;        // Buffer to hold the RGB output of the JPEG decoder
  dword            m_dwRGBBufSize;   // Size of the RGB output buffer
#ifdef JPEG_ALLOC_DECODE_BUF
  byte            *m_pDecodeBuf;     // Decoder buffer. Allocated only if
#endif                               // JPEG_ALLOC_DECODE_BUF is defined. Otherwise,
                                     // the CMX buffer is used.
  AEEObjectHandle  m_hObject;        // Object context ptr used by callback
  boolean          bDecoding;
} COEMJPEGViewerSys;

typedef enum _JPEGStateEnum
{
  JPEG_START_STATE,
  JPEG_READ_STATE,
  JPEG_COMPLETE_STATE,
  JPEG_ERROR_STATE,
  JPEG_MAX_STATE
} JPEGState;

typedef enum _JPEGColorFormat
{
  JPEG_COLOR_FORMAT_RGB565,
  JPEG_COLOR_FORMAT_YCBCR420LP,
  JPEG_COLOR_FORMAT_YCBCR422LP,
  JPEG_COLOR_FORMAT_YCRCB420LP,
  JPEG_COLOR_FORMAT_YCRCB422LP,  
  JPEG_COLOR_FORMAT_RGB888
} JPEGColorFormat;

//lint -save -e754
struct _COEMJPEGViewer
{

  DECLARE_VTBL(IImage)

  IDisplay                  *m_pDisplay;       // Display
  IShell                    *m_pShell;         // Shell
  AEEImageInfo               m_info;           // generic info

  uint32                     m_cls;            // our class
  int32                      m_nRefs;          // reference count
  AEERect                    m_rc;             // destination x,y,cx,cy
  int16                      m_xOffset;        // x offset into image
  int16                      m_yOffset;        // y offset into image
  AEERasterOp                m_rop;            // raster op
  IAStream                  *m_pStream;        // Data stream passed to this object
  PFNIMAGEINFO               m_pfnNotify;
  void                      *m_pUser;
  JPEGColorFormat            m_ColorFormat;    // select RGB565, YCbCr42x
  uint32                     m_nWritten;       // No of bytes written into IBItmap buffer
  byte                      *m_pJPEG;          // JPEG data
  byte                      *m_pJPEG_thumbnail;// Thumbnail data in JPEG stream
  dword                      m_dwJPEGBufSize;  // JPEG buffer size
  PACONTEXT                  m_pAppContext;    // Context of the app that instantiated this viewer
  dword                      m_nRead;          // No of bytes read fro the input stream
  dword                      m_nBytesConsumed; // No of bytes read by the JPEG decoder
  JPEGState                  m_ReadState;      // How much of the file has been read.
  AEECallback                m_Callback;       // Use callbacks to decode so we don't blow the stack
  uint32                     m_nDeviceClrs;    // No. of colors of the device
  jpeg_cmx_status_enum_type  m_JPEGcmxStatus;  // Status returned by JPEG engine. Used to notify user
  boolean                    m_thumbnailImage; // TRUE - decode and display thumbnail, else main image
  boolean                    m_bImageDecoded;  // TRUE if the image has been decoded
  boolean                    m_bAllocMem;      // If the OEM wants to allocate a buffer
                                               // to do JPEG decode instead of using
                                               // the CMX buffer already present
  exif_info_type             m_ExifInfo;       // expose exif info for IMediaUtil

  COEMJPEGViewerSys         *m_pOEMJPEGViewerSys;
#ifdef FEATURE_JPEGD_RESIZE
  jpegdResizeInfo            m_ResizeInfo;
#endif

#if defined (FEATURE_ACM)
#if defined (FEATURE_ODM_UI)
   IAppResCtl               *m_pIAppRes;       // IAppResCtl
#endif // FEATURE_ODM_UI
   int                       m_resType;        // ARM or DSP to be used for decoding
   int                       m_resState;       // Current state of resource acquisition
   ACM_Callback_data_type    m_cbACMdata;
   boolean                   m_boAlreadyInFreeData ; // Check to see if Free_data is in progress
                                                     //so we don't repeat
#endif // FEATURE_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif /* FEATURE_BMP_ACM */
#ifdef FEATURE_GEOTAGGING
  exif_position_type gpsInfo; 
#endif /* FEATURE_GEOTAGGING */
 
};
static COEMJPEGViewer *m_pJpegViewer;

static IBase       *COEMJPEGViewer_New(IShell *po, AEECLSID cls) ;
static uint32       COEMJPEGViewer_AddRef(IImage *po) ;
static uint32       COEMJPEGViewer_Release(IImage *po) ;
static void         COEMJPEGViewer_GetInfo(IImage *po, AEEImageInfo *pi) ;
static void         COEMJPEGViewer_SetParm(IImage *po, int nParm, int n1, int n2) ;
static void         COEMJPEGViewer_Draw(IImage *po, int x, int y) ;

static void         COEMJPEGViewer_DrawFrame(IImage *po,int nFrame, int x, int y) ;
static void         COEMJPEGViewer_Start(IImage *po, int x, int y) ;
static void         COEMJPEGViewer_Stop(IImage *po) ;
static void         COEMJPEGViewer_SetStream(IImage *po, IAStream *ps) ;
static void         COEMJPEGViewer_Notify(IImage *po, PFNIMAGEINFO pfn, void *pUser) ;
static boolean      COEMJPEGViewer_HandleEvent(IImage *po, AEEEvent e, uint16 w, uint32 dw) ;
static void         COEMJPEGViewer_FreeData(COEMJPEGViewer *pme) ;
static void         COEMJPEGViewer_FreeSystemData(COEMJPEGViewerSys *pmeSys);
static void         COEMJPEGViewer_CancelDecode(COEMJPEGViewerSys *pmeSys);
static void         COEMJPEGViewer_ReadableCB(COEMJPEGViewer *pme) ;


//JPEG Decode Functions
void                JPEGViewer_Init(IShell *ps);
int                 JPEGViewer_New(IShell *pIShell,AEECLSID ClsId,void **ppObj);
static void         JPEG_NotifyCaller(COEMJPEGViewer *pme, int iSuccess);
static void         JPEG_DecodeImage(void *po);
static void         JPEG_DecodeCB(jpeg_cmx_status_enum_type   command,
                                  void *po,
                                  void *server_data);

#ifdef FEATURE_GEOTAGGING
static int  JPEG_WriteToFile(COEMJPEGViewer *pme,exif_geotagging_output_type *outputInfo);
#endif /* FEATURE_GEOTAGGING */

static void         JPEG_NotifyStatus(void *po);
static void         JPEG_CreateIBitmap(COEMJPEGViewer *pme, uint16 x, uint16 y);
static void         JPEG_ProceedWithDecode(COEMJPEGViewer *pme, int resType);
  #ifdef FEATURE_QVPHONE
boolean             IS_Videophone(COEMJPEGViewer *pme);
  #endif

#if defined (FEATURE_ACM)
static int JPEGViewer_ACMInit(COEMJPEGViewer *pme);
#endif // FEATURE_ACM
#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
static int JPEGViewer_ACMAcquire(COEMJPEGViewer *pme);
static void JPEGViewer_ACMStatusChange(void *p);
static void JPEGViewer_ACMFree(COEMJPEGViewer *pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM

const AEEStaticClass gAEEJpegClasses[] =
{
  {AEECLSID_JPEG,                  ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_RGB_MAIN,         ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_RGB_THUMB,        ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCBCR420LP_MAIN,  ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCBCR420LP_THUMB, ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCBCR422LP_MAIN,  ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCBCR422LP_THUMB, ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCRCB420LP_MAIN,  ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCRCB420LP_THUMB, ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCRCB422LP_MAIN,  ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_YCRCB422LP_THUMB, ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_RGB888_MAIN,      ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {AEECLSID_JPEG_RGB888_THUMB,     ASCF_UPGRADE, 0, JPEGViewer_Init, JPEGViewer_New},
  {NULL}
};

extern void jpeg_cancel_decode(void);

IDIB *OEMJPEG_SetImageData(IImage *po, IDIB *dib);
#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase **) (void *) &p)

static __inline void RELEASEPPIF(IBase **p)
{
  if (*p)
  {
    (void)IBASE_Release(*p);
    *p = 0;
  }
}
#endif //RELEASEIF

//
// File/Database Module
//

#ifdef FEATURE_BREW_3_0
static OEMCriticalSection gJPEGCriticalSection;
//lint -save -e545, Suspicious use of &
static OEMCriticalSection *gJPEGCriticalSec = &gJPEGCriticalSection;
//lint -restore
#define OEMJPEG_MUTEX_LOCK()     OEMCriticalSection_Enter(gJPEGCriticalSec); {
#define OEMJPEG_MUTEX_UNLOCK()   } OEMCriticalSection_Leave(gJPEGCriticalSec);
  #else
#define OEMJPEG_MUTEX_LOCK()           OEMOS_MutexStart(); {
#define OEMJPEG_MUTEX_UNLOCK()         } OEMOS_MutexStop();
#endif

#ifndef MIN
#define MIN(x, y)       (((x) < (y)) ? (x) : (y))
#endif


#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
#define   ACM_VIEW_PICTURE_QDSP      0x1
#define   ACM_VIEW_PICTURE_ARM       0x2
#endif

static const VTBL(IImage) gCOEMJPEGViewerMethods =
{
  COEMJPEGViewer_AddRef,
  COEMJPEGViewer_Release,
  COEMJPEGViewer_Draw,
  COEMJPEGViewer_DrawFrame,
  COEMJPEGViewer_GetInfo,
  COEMJPEGViewer_SetParm,
  COEMJPEGViewer_Start,
  COEMJPEGViewer_Stop,
  COEMJPEGViewer_SetStream,
  COEMJPEGViewer_HandleEvent,
  COEMJPEGViewer_Notify
};

static  uint32         gnClrDepth;
static  boolean        JPEG_thumbnail=TRUE; /* inicate thumbnail image preference */
static  boolean        isJPEGEXIFHeaderValid = TRUE;
static  boolean        jpegdInCancel = FALSE;
static boolean isJpegAbort = FALSE;

/*===========================================================================

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void JPEGViewer_Init(IShell *ps)
{
  //Deregister any existing handle for the MIME types image/jpeg & image/jpg
  (void)ISHELL_RegisterHandler(ps, HTYPE_VIEWER, MT_JPEG, 0);
  (void)ISHELL_RegisterHandler(ps, HTYPE_VIEWER, MT_JPG, 0);

  //Register this class as the JPEG handler
  (void)ISHELL_RegisterHandler(ps, HTYPE_VIEWER, MT_JPEG, AEECLSID_JPEG);
  (void)ISHELL_RegisterHandler(ps, HTYPE_VIEWER, MT_JPG,  AEECLSID_JPEG);
}

/*===========================================================================

===========================================================================*/
int JPEGViewer_New(IShell *pIShell,AEECLSID ClsId,void **ppObj)
{
  IBase *pobj;

  if (jpegdInCancel)
  {
    MSG_HIGH("jpegdInCancel is TRUE", 0, 0, 0);
    return EITEMBUSY;
  }

  pobj = COEMJPEGViewer_New(pIShell, ClsId);
  *ppObj = pobj;
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
  if (pobj == NULL)
    return ENOMEMORY;

#if defined(FEATURE_ACM)
  if(JPEGViewer_ACMInit((COEMJPEGViewer*)pobj) != SUCCESS)
#else
  if(JPEGViewer_ACMInit(pIShell, (COEMJPEGViewer*)pobj) != SUCCESS)
#endif
    return EFAILED;
  else
    return SUCCESS;
#else
  return((int)(pobj == NULL ? ENOMEMORY : 0));
#endif // FEATURE_ACM || FEATURE_BMP_ACM
}

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION COEMJPEGViewer_New

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
IBase *COEMJPEGViewer_New(IShell *po, AEECLSID cls)
{
  COEMJPEGViewer *pme;
  COEMJPEGViewerSys *pmeSys;
  ACONTEXT *pac;

  MSG_HIGH("COEMJPEGViewer_New", 0, 0, 0);

  pac = AEE_EnterAppContext(0);
  pmeSys = MALLOC(sizeof(COEMJPEGViewerSys));
  AEE_LeaveAppContext(pac);

  if(!pmeSys)
    return NULL;

  pme = (COEMJPEGViewer *)(void *) AEE_OEM_NEWCLASS((IBaseVtbl *)&gCOEMJPEGViewerMethods,
                                                sizeof(COEMJPEGViewer));
  if(pme)
  {
    pmeSys->pOEMJPEGViewer = pme;
    pmeSys->bDecoding = FALSE;
    pmeSys->m_pIDIB = NULL;
    pmeSys->m_pRGBBuf = NULL;
    pmeSys->m_dwRGBBufSize = 0;

    pme->m_pOEMJPEGViewerSys = pmeSys;

    pme->m_nRefs = 1;
    pme->m_cls = cls;
    pme->m_pShell = po;
    (void)ISHELL_CreateInstance(po, AEECLSID_DISPLAY, (void **)(&pme->m_pDisplay));
    pme->m_rop = AEE_RO_COPY;

    if(AEEObjectMgr_Register(pmeSys, &pmeSys->m_hObject))
    {
      FREE(pme);
      FREE(pmeSys);
      pmeSys = NULL;
      return NULL;
    }
    //Don't try to do this every time 'cos the color depth is not
    //going to change for a device. But on SDK, they can choose different skins
#if !defined(AEE_SIMULATOR)
    if(!gnClrDepth)
#endif //!AEE_SIMULATOR
    {
      AEEDeviceInfo devInfo;
      ISHELL_GetDeviceInfo(po, &devInfo);
      gnClrDepth = devInfo.nColorDepth;
    }
    pme->m_nDeviceClrs = gnClrDepth;
    pme->m_pAppContext = AEE_GetAppContext();

    switch(cls)
    {
    case AEECLSID_JPEG_RGB_MAIN:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_RGB565;
      JPEG_thumbnail = FALSE;
      break;
    case AEECLSID_JPEG_RGB_THUMB:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_RGB565;
      JPEG_thumbnail = TRUE;
      break;
    case AEECLSID_JPEG_YCBCR420LP_MAIN:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCBCR420LP;
      JPEG_thumbnail = FALSE;
      break;
    case AEECLSID_JPEG_YCBCR420LP_THUMB:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCBCR420LP;
      JPEG_thumbnail = TRUE;
      break;
    case AEECLSID_JPEG_YCBCR422LP_MAIN:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCBCR422LP;
      JPEG_thumbnail = FALSE;
      break;
    case AEECLSID_JPEG_YCBCR422LP_THUMB:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCBCR422LP;
      JPEG_thumbnail = TRUE;
      break;
    case AEECLSID_JPEG_YCRCB420LP_MAIN:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCRCB420LP;
      JPEG_thumbnail = FALSE;
      break;
    case AEECLSID_JPEG_YCRCB420LP_THUMB:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCRCB420LP;
      JPEG_thumbnail = TRUE;
      break;
    case AEECLSID_JPEG_YCRCB422LP_MAIN:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCRCB422LP;
      JPEG_thumbnail = FALSE;
      break;
    case AEECLSID_JPEG_YCRCB422LP_THUMB:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_YCRCB422LP;
      JPEG_thumbnail = TRUE;
      break;
	  
    case AEECLSID_JPEG_RGB888_MAIN:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_RGB888;
      JPEG_thumbnail = FALSE;
      break;
    case AEECLSID_JPEG_RGB888_THUMB:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_RGB888;
      JPEG_thumbnail = TRUE;
      break;
    default:
      pme->m_ColorFormat = JPEG_COLOR_FORMAT_RGB565;
      JPEG_thumbnail = TRUE;
    }

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
    pme->m_boAlreadyInFreeData = FALSE ;
#endif // FEATURE_ACM || FEATURE_BMP_ACM

    return((IBase *)pme);
  }
  return(NULL);
}

/*===========================================================================

FUNCTION COEMJPEGViewer_AddRef

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
uint32 COEMJPEGViewer_AddRef(IImage *po)
{
  if(po)
    return(uint32)(++((COEMJPEGViewer *)po)->m_nRefs);
  return(0);
}

#ifdef FEATURE_GEOTAGGING
/*===========================================================================
FUNCTION JPEG_WriteToFile

DESCRIPTION
The header info and the main image data is written to the image file .

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/

static int JPEG_WriteToFile(COEMJPEGViewer *pme,exif_geotagging_output_type *outputInfo)
{
  int fileStatus, fileDescriptor, bytesWritten = 0;
  int status = EFAILED;

  (void)efs_unlink(geotag_filename);
  
  //creating a new file beacause the older one is deleted above.
  fileDescriptor = efs_open (geotag_filename, O_CREAT |O_WRONLY);
  
  if (fileDescriptor != (-1))
  {
    bytesWritten = efs_write(fileDescriptor, outputInfo->header_ptr,outputInfo->header_size);
    if((outputInfo->header_size != 0) && (bytesWritten > 0)) //making sure first write is not failed
      bytesWritten += efs_write(fileDescriptor, outputInfo->mainimage_ptr, outputInfo->mainimage_size);
    MSG_HIGH("Total bytes written = %d",bytesWritten,0,0);
    MSG_HIGH("Total bytes requested to write = %d",outputInfo->header_size + outputInfo->mainimage_size ,0,0);

    fileStatus = efs_close(fileDescriptor);
    if(fileStatus == 0)
	status = SUCCESS;
    else
	MSG_HIGH("File could not be closed",0,0,0);
  }

  FREEIF(geotag_filename);
  return status;
 }
#endif  /* FEATURE_GEOTAGGING */

/*===========================================================================

FUNCTION COEMJPEGViewer_Release

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
uint32  COEMJPEGViewer_Release(IImage *po)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;

  MSG_HIGH("COEMJPEGViewer_Release",0,0,0);

  if(pme && pme->m_nRefs > 0)
  {
    if(--pme->m_nRefs)
      return(uint32)(pme->m_nRefs);

    (void)IDISPLAY_Release(pme->m_pDisplay);

    COEMJPEGViewer_FreeData(pme);

    OEMJPEG_MUTEX_LOCK()

    if(!pme->m_pOEMJPEGViewerSys->bDecoding)
    {
      COEMJPEGViewer_FreeSystemData(pme->m_pOEMJPEGViewerSys);
      if (pme->m_pOEMJPEGViewerSys)
      {
        AEEObjectMgr_Unregister(pme->m_pOEMJPEGViewerSys->m_hObject);
        FREE(pme->m_pOEMJPEGViewerSys);
        pme->m_pOEMJPEGViewerSys = NULL;
      }
    }
    else
    {
      MSG_HIGH("Decoding in progress",0,0,0);
      jpegdInCancel = TRUE;
      COEMJPEGViewer_CancelDecode(pme->m_pOEMJPEGViewerSys);
      isJpegAbort = TRUE; //Flag to see if its an abort case
    }

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 
    if(!isJpegAbort)
    {
      FREE((void *) pme);
      pme = NULL;
    }

    OEMJPEG_MUTEX_UNLOCK()
  }
  return(0);
}

static void COEMJPEGViewer_FreeSystemData(COEMJPEGViewerSys *pmeSys)
{
#ifdef FEATURE_JPEG_DECODER_REV2
  if(pmeSys->m_pIYCbCr)
  {
    (void)IBITMAP_Release((IBitmap *)pmeSys->m_pIYCbCr);
    pmeSys->m_pIYCbCr = NULL;
  }
#endif

  if(pmeSys->m_pIDIB)
  {
    (void)IDIB_Release(pmeSys->m_pIDIB);
    pmeSys->m_pIDIB = NULL;
  }

  if(pmeSys->m_pRGBBuf)
  {
    FREE(pmeSys->m_pRGBBuf);
    pmeSys->m_pRGBBuf = NULL;
  }

#if defined(JPEG_ALLOC_DECODE_BUF)
#if !defined(AEE_SIMULATOR)
  FREEIF(pme->m_pDecodeBuf);
  pme->m_pDecodeBuf = NULL;
#endif //!AEE_SIMULATOR
#endif //JPEG_ALLOC_DECODE_BUF
}

static void COEMJPEGViewer_CancelDecode(COEMJPEGViewerSys *pmeSys)
{
  /*lint -save -e715 -e818*/
  OEMJPEG_MUTEX_LOCK()         //Begin Critical Section
  jpeg_cancel_decode();

  graph_jpeg_dec_image_abort();

#if defined(JPEG_ALLOC_DECODE_BUF)
#if !defined(AEE_SIMULATOR)
  graph_jpeg_set_buf(NULL, 0, NULL);
#endif //!AEE_SIMULATOR
#endif //JPEG_ALLOC_DECODE_BUF

  OEMJPEG_MUTEX_UNLOCK()
}


/*===========================================================================

FUNCTION COEMJPEGViewer_GetInfo

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_GetInfo(IImage *po, AEEImageInfo *pi)
{
  OEMJPEG_MUTEX_LOCK()         //Begin Critical Section
  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;

  if(pi)
    (void)MEMCPY(pi, &pme->m_info, sizeof(AEEImageInfo));

  OEMJPEG_MUTEX_UNLOCK()         //End Critical Section
}


/*===========================================================================

FUNCTION COEMJPEGViewer_SetParm

DESCRIPTION
   The only configurable parameters in the JPEG viewer are the raster operator,
   the size of the image, and the screen offset.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_SetParm(IImage *po, int nParm, int n1, int n2)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;
  exif_valueinfo_type value;
  AEEImageExtFunc *p1;
  OEMImageParmExt *pBuf;
#ifdef FEATURE_JPEGD_RESIZE
  jpegdResizeInfo *pResizeInfo;
#endif
#ifdef FEATURE_GEOTAGGING
  GeoTagInfo *temp_gpsInfo;
  char *pszFullPath;
  IFile *pIFile;
  IFileMgr  *m_pIFileMgr;
  int size,result;
#endif /* FEATURE_GEOTAGGING */
  boolean status = FALSE;
  int nRetVal = 0;

  switch(nParm)
  {
  case IPARM_ROP:
    pme->m_rop = (AEERasterOp)n1;
    break;

  case IPARM_SIZE:
    if(n1 < 0)
      n1 = 0;
    else
    {
      if(n1 > pme->m_info.cxFrame)
        n1 = pme->m_info.cxFrame;
    }
    if(n2 < 0)
      n2 = 0;
    else
    {
      if(n2 > pme->m_info.cy)
        n2 = pme->m_info.cy;
    }
    pme->m_rc.dx = (int16)n1;
    pme->m_rc.dy = (int16)n2;
    break;

  case IPARM_OFFSET:
    pme->m_xOffset = (int16)n1;
    pme->m_yOffset = (int16)n2;
    break;

  case IPARM_DISPLAY:
    {
      IDisplay *pIDisp = (IDisplay *) n1;
      if(pIDisp)
      {
        (void)IDISPLAY_Release(pme->m_pDisplay);
        pme->m_pDisplay = pIDisp;
        (void)IDISPLAY_AddRef(pme->m_pDisplay);
      }
      break;
    }

  case IPARM_SCALE:
    {
      IBitmap *pIOutBitmap = NULL;
      IBitmap *pIInBitmap = NULL;
      IDIB *pIOutDib = NULL;
      IDIB *pTempDib = pme->m_pOEMJPEGViewerSys->m_pIDIB;
      IIpl *pIIPL = NULL;

      if((n1 == pme->m_info.cx) && (n2 == pme->m_info.cy))
      {
        break;
      }

      if(IDISPLAY_CreateDIBitmap(pme->m_pDisplay, &pIOutDib, pTempDib->nDepth,
                                 (uint16)n1, (uint16)n2))
      {
        break;
      }
      pIOutDib->nColorScheme = pTempDib->nColorScheme;

      pIOutBitmap = IDIB_TO_IBITMAP(pIOutDib);
      pIInBitmap = IDIB_TO_IBITMAP(pTempDib);

      nRetVal = ISHELL_CreateInstance (pme->m_pShell, AEECLSID_IPL, (void **)&pIIPL);
      if(SUCCESS != nRetVal)
      {
        RELEASEIF(pIOutDib);
        break;
      }

      if(IIPL_ChangeSize (pIIPL, pIInBitmap, pIOutBitmap) != AEE_SUCCESS)
      {
        RELEASEIF(pIOutDib);
        RELEASEIF(pIIPL);
        break;
      }

      pTempDib = OEMJPEG_SetImageData( po, pIOutDib);
      RELEASEIF(pTempDib);
      RELEASEIF(pIIPL);
      break;
    }

  case IPARM_GETBITMAP:
#ifdef FEATURE_JPEG_DECODER_REV2
    if((pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR420LP) ||
       (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR422LP) ||
       (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB420LP) ||
       (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB422LP))
    {
      *(IYCbCr **)n1 = pme->m_pOEMJPEGViewerSys->m_pIYCbCr;
      *(int *)n2 = SUCCESS;
    }
    else
#endif
    {
      *(IDIB **)n1 = pme->m_pOEMJPEGViewerSys->m_pIDIB;
      *(int *)n2 = SUCCESS;
    }
    break;

  case IPARM_EXTFUNC:
    p1   = (AEEImageExtFunc *)n1;
    pBuf = (OEMImageParmExt *)p1->pBuf;
    switch(p1->id)
    {
    case AEECLSID_EXIF_GET_INFO:
      pBuf->pData = &pme->m_ExifInfo;

      if (isJPEGEXIFHeaderValid)
      *(int *)n2 = SUCCESS;
      else
        *(int *)n2 = EFAILED;

      break;

#ifdef FEATURE_JPEGD_RESIZE
    case JPEGD_RESIZE:
      pResizeInfo = (jpegdResizeInfo *)pBuf->pData;
      pme->m_ResizeInfo.resizeImageWidth  = pResizeInfo->resizeImageWidth;
      pme->m_ResizeInfo.resizeImageHeight = pResizeInfo->resizeImageHeight;
      pme->m_ResizeInfo.aspectRatioFlag   = pResizeInfo->aspectRatioFlag;
      pme->m_ResizeInfo.jpegdResizeFlag   = pResizeInfo->jpegdResizeFlag;
      *(int *)n2 = SUCCESS;
      break;
#endif

#ifdef FEATURE_GEOTAGGING
    case ADD_TAGS:
      temp_gpsInfo = (GeoTagInfo *) pBuf->pData;	
      pme->gpsInfo.altitude = temp_gpsInfo->gps_info->wAltitude;
      pme->gpsInfo.latitude = temp_gpsInfo->gps_info->dwLat;
      pme->gpsInfo.longitude=temp_gpsInfo->gps_info->dwLon;
      pme->gpsInfo.timestamp=temp_gpsInfo->gps_info->dwTimeStamp;

      if (ISHELL_CreateInstance(pme->m_pShell,AEECLSID_FILEMGR,
                            (void **)&m_pIFileMgr) != SUCCESS)
      {
        MSG_HIGH("FileMgr Creation failed", 0, 0, 0);
        *(int *)n2 = EFAILED;
      }
       result = IFILEMGR_ResolvePath(m_pIFileMgr,temp_gpsInfo->m_sCurrentFileName,NULL,&size);
       if (result != SUCCESS) 
	   {
	       MSG_HIGH("Resolving filepath failed", 0, 0, 0);
           IFILEMGR_Release(m_pIFileMgr);
           break;
       }
       pszFullPath = MALLOC((uint32)size);
       if (NULL == pszFullPath) 
	   {
         IFILEMGR_Release(m_pIFileMgr);
         result = ENOMEMORY;
         break;
       }
       // Resolve the path and copy it to the destination string.
       result = IFILEMGR_ResolvePath(m_pIFileMgr,temp_gpsInfo->m_sCurrentFileName,
                                          pszFullPath,&size);
	   
	size += strlen(OEM_BREW_DIR);
	geotag_filename =MALLOC((uint32)size);

       pIFile = IFILEMGR_OpenFile(m_pIFileMgr, temp_gpsInfo->m_sCurrentFileName, _OFM_READ);

       // convert to EFS name
       (void)OEMFS_GetNativePath(pszFullPath,geotag_filename,&size);
	FREEIF(pszFullPath);

       if(pIFile)
       {
          Isgeotag=TRUE;
  	   COEMJPEGViewer_SetStream(po, (IAStream *)pIFile);
          (void)IFILE_Release(pIFile);
          *(int *)n2 = SUCCESS;
  	}
	else
          *(int *)n2 = EFAILED;
	
       IFILEMGR_Release(m_pIFileMgr);
       break;
#endif /* FEATURE_GEOTAGGING */

    case AEECLSID_EXIF_GET_VALUE:
      if(exif_get_value(&value, &pme->m_ExifInfo, (uint16)pBuf->uID) ==
                        EXIF_GET_VALUE_SUCCESS)
      {
        pBuf->pData = value.offset;
        pBuf->uLen = value.count;
        *(int *)n2 = SUCCESS;
      }
      else
        *(int *)n2 = EFAILED;
      break;
    case AEECLSID_EXIF_SET_ASCII_VALUE:
      value.type = (int)EXIF_ASCII;
      value.count = pBuf->uLen;
      value.offset = pBuf->pData;
      if(exif_set_value(&pme->m_ExifInfo, &value, (uint16)pBuf->uID) == TRUE)
        *(int *)n2 = SUCCESS;
      else
        *(int *)n2 = EFAILED;
      break;
    case AEECLSID_EXTRACT_THUMBNAIL:
      /* At this point, thumbnail dimension is not available, so QVGA size */
      /* is presumed for worst case consideration */
      *((uint8 **)pBuf->pData) = (uint8 *) MALLOC(JPEG_THUMBNAIL_BUFSIZE);

      if(*((uint8 **)pBuf->pData))
      {
        pBuf->uLen  = JPEG_THUMBNAIL_BUFSIZE;
        if((status = (jpeg_extract_thumbnail(pme->m_pJPEG, *((uint8 **)pBuf->pData),
                                             &pBuf->uLen))) == TRUE)
          *(int *)n2 = SUCCESS;
        else
          *(int *)n2 = EFAILED;
      }
      else
      {
        pBuf->uLen = 0;
      }
      if(!status)
      {
        FREEIF(*((uint8 **)pBuf->pData));
        *((uint8 **)pBuf->pData) = NULL;
        pBuf->uLen = 0;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

/*===========================================================================

FUNCTION COEMJPEGViewer_Draw

DESCRIPTION
   This function draws the first frame only.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_Draw(IImage *po, int x, int y)
{
  COEMJPEGViewer_DrawFrame(po, 0, x, y);
}

/*===========================================================================

FUNCTION COEMJPEGViewer_DrawFrame

DESCRIPTION
   Draws the requested frame number. For JPEG, draws the image as there is only
   one frame in a JPEG.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_DrawFrame(IImage *po,int nFrame, int x, int y)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;

  if(pme->m_ReadState == JPEG_COMPLETE_STATE)
  {
#ifdef FEATURE_JPEG_DECODER_REV2
    if((pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR420LP) ||
       (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR422LP) ||
       (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB420LP) ||
       (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB422LP))
    {
      if(pme->m_pOEMJPEGViewerSys->m_pIYCbCr)
        IDISPLAY_BitBlt(pme->m_pDisplay, x, y, pme->m_rc.dx, pme->m_rc.dy,
                        (const void *) pme->m_pOEMJPEGViewerSys->m_pIYCbCr,
                        pme->m_xOffset, pme->m_yOffset, pme->m_rop);
    }
    else
#endif
    {
      if(pme->m_pOEMJPEGViewerSys->m_pIDIB)
        IDISPLAY_BitBlt(pme->m_pDisplay, x, y, pme->m_rc.dx, pme->m_rc.dy ,
                        (const void *) pme->m_pOEMJPEGViewerSys->m_pIDIB,
                        pme->m_xOffset, pme->m_yOffset, pme->m_rop);
    }
  }
}


/*===========================================================================

FUNCTION COEMJPEGViewer_Start

DESCRIPTION
   Draws the JPEG image. This method is provided to adhere to the IIMAGE interface
   specification. Also saves the trouble of calling an update on the display.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_Start(IImage *po, int x, int y)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;

  COEMJPEGViewer_Draw(po, x, y);
  IDISPLAY_Update(pme->m_pDisplay);

  return;
}

/*===========================================================================

FUNCTION COEMJPEGViewer_Stop

DESCRIPTION
   Stops animation

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_Stop(IImage *po)
{
  return;
}

/*===========================================================================

FUNCTION COEMJPEGViewer_SetStream

DESCRIPTION
   Sets up the stream to read the JPEG image from

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_SetStream(IImage *po, IAStream *ps)
{

  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;
  int32 nSize;

  if(!ps)
    return;

  pme->m_pStream = ps;
  (void)IASTREAM_AddRef(ps);
  pme->m_ReadState = JPEG_START_STATE;

  // See if we can pre-determine the size, and if it is less than JPEG_DEFAULT_BUFSIZE.
  //If it is, we can save on the memory we allocate.
  nSize = IASTREAM_Read(ps, NULL, 0);

  if(nSize > 0)
  {
    nSize++;
    pme->m_dwJPEGBufSize = (uint32)nSize;
  }
  else
    pme->m_dwJPEGBufSize = JPEG_DEFAULT_BUFSIZE;

  COEMJPEGViewer_ReadableCB(pme);
}

/*===========================================================================

FUNCTION COEMJPEGViewer_HandleEvent

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean COEMJPEGViewer_HandleEvent(IImage *po, AEEEvent e, uint16 w, uint32 d)
{
  return(FALSE);
}

/*===========================================================================

FUNCTION COEMJPEGViewer_Notify

DESCRIPTION
   This method provides the caller a mechanism by which they can be notified when
   the complete JPEG buffer has been received and parsed. This is useful for
   the caller to determine when to draw the frame.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_Notify(IImage *po, PFNIMAGEINFO pfn, void *pUser)
{
  OEMJPEG_MUTEX_LOCK()         //Begin Critical Section
  COEMJPEGViewer *pme = (COEMJPEGViewer *)po;

  if(pme->m_bImageDecoded)
  {
    if(pfn)
      pfn(pUser, po, &pme->m_info, SUCCESS);
  }
  else
  {
    pme->m_pfnNotify = pfn;
    pme->m_pUser = pUser;
  }

  OEMJPEG_MUTEX_UNLOCK()         //End Critical Section
}

/*===========================================================================

FUNCTION COEMJPEGViewer_FreeData

DESCRIPTION
   Frees all the memory allocated for this viewer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_FreeData(COEMJPEGViewer *pme)
{
#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
// ASH : Don't call _FreeData twice else it will result in a deadlock
// one _FreeData happens from ACM_Abort , other is when JPEG decoding
// is done or aborted
// and also at _Release

// Suppose the _FreeData happens due to _Release : in that case also
// if _FreeData is happening due to ACM_Abort or JPEG/Done/Abort
// second _FreeData will not get executed
// the next MUTEX in _Release will hold the next execution unless the _FreeData completes

  if(pme->m_boAlreadyInFreeData == TRUE)
  {
    return;
  }
  else
  {
    pme->m_boAlreadyInFreeData = TRUE ;
  }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

  OEMJPEG_MUTEX_LOCK()         //Begin Critical Section

  CALLBACK_Cancel(&pme->m_Callback);
  (void)MEMSET(&pme->m_info, 0, sizeof(AEEImageInfo));

  if(pme->m_Callback.pfnCancel)
    pme->m_Callback.pfnCancel(&pme->m_Callback);

  if(pme->m_pStream)
  {
    IASTREAM_Cancel(pme->m_pStream);
    (void)IASTREAM_Release(pme->m_pStream);
    pme->m_pStream = NULL;
  }

  if(pme->m_pJPEG)
  {
    FREE(pme->m_pJPEG);
    pme->m_pJPEG = NULL;
    pme->m_pJPEG_thumbnail = NULL;
  }

  pme->m_nRead = 0;
  pme->m_nWritten = 0;
  pme->m_bImageDecoded = FALSE;
  exif_terminate(&pme->m_ExifInfo);

#ifdef FEATURE_GEOTAGGING
  Isgeotag = FALSE;
  FREEIF(geotag_filename);
#endif /* FEATURE_GEOTAGGING */

#if defined (FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  JPEGViewer_ACMFree(pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM

  OEMJPEG_MUTEX_UNLOCK()         //End Critical Section

#if defined (FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  pme->m_boAlreadyInFreeData = FALSE ;
#endif // FEATURE_ACM || FEATURE_BMP_ACM

}


/*===========================================================================

FUNCTION COEMJPEGViewer_ReadableCB

DESCRIPTION
   This callback function is used to asynchronously read the stream for the
   JPEG image.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMJPEGViewer_ReadableCB(COEMJPEGViewer *pme)
{
  int32 nRead;
  byte *pRealloc;
#ifdef FEATURE_GEOTAGGING
  exif_geotagging_ret_type retVal;
  exif_geotagging_output_type outputPtr;
  int status=SUCCESS;
#endif /* FEATURE_GEOTAGGING */

  switch(pme->m_ReadState)
  {
  case JPEG_START_STATE:
    if(!pme->m_pJPEG)
    {
      pme->m_pJPEG = (byte *) MALLOC(pme->m_dwJPEGBufSize);

      if(!pme->m_pJPEG)
      {
        pme->m_ReadState = JPEG_ERROR_STATE;
        JPEG_NotifyCaller(pme, EFAILED);
        return;
      }
    }
    pme->m_ReadState = JPEG_READ_STATE;
    //lint -fallthrough

  case JPEG_READ_STATE:
    if(pme->m_dwJPEGBufSize == pme->m_nRead)
    {
      if((pRealloc = (byte *) REALLOC(pme->m_pJPEG,
                                      pme->m_dwJPEGBufSize + JPEG_BUF_INC_SIZE))
                                      == NULL)
      {
        pme->m_ReadState = JPEG_ERROR_STATE;
        JPEG_NotifyCaller(pme, EFAILED);
        return;
      }
      pme->m_pJPEG = pRealloc;
      pme->m_dwJPEGBufSize += JPEG_BUF_INC_SIZE;
    }

    nRead = IASTREAM_Read(pme->m_pStream, pme->m_pJPEG + pme->m_nRead,
                          pme->m_dwJPEGBufSize - pme->m_nRead);

    if(nRead == 0 || nRead == AEE_NET_ERROR)
    {
      pme->m_ReadState = JPEG_COMPLETE_STATE;
    }
    else
    {
      if(nRead != AEE_STREAM_WOULDBLOCK)
        pme->m_nRead += (uint32)nRead;
      IASTREAM_Readable(pme->m_pStream, (PFNNOTIFY)COEMJPEGViewer_ReadableCB, pme);
      return;
    }

    /* If valid file size */
    if((pme->m_nRead) > 0)
    {
      /* Search for thumbnail image in the JPEG stream */
		isJPEGEXIFHeaderValid = jpeg_exifread(&pme->m_ExifInfo, pme->m_pJPEG, (uint32)pme->m_nRead);

      if((isJPEGEXIFHeaderValid) ||(jpeg_jfifread(&pme->m_ExifInfo, pme->m_pJPEG, (uint32)pme->m_nRead)))
      {
        /* See if thumbnail has been requested and thumbnail is present */
        if((JPEG_thumbnail) && (pme->m_ExifInfo.thumbnail_present))
        {
          pme->m_thumbnailImage  = TRUE;
          pme->m_pJPEG_thumbnail = pme->m_ExifInfo.thumbnail_stream;
          pme->m_ReadState = JPEG_COMPLETE_STATE;
          nRead = (int32)pme->m_ExifInfo.thumbnail_stream_length;
        } /* thumbnail present */
      } /* jpeg_exifread() */
    }

    
    //lint -fallthrough

  case JPEG_COMPLETE_STATE:
    // release bit stream so that IImage instance can remain
    if(pme->m_pStream)
    {
      IASTREAM_Cancel(pme->m_pStream);
      (void)IASTREAM_Release(pme->m_pStream);
      pme->m_pStream = NULL;
    }
#ifdef FEATURE_GEOTAGGING
    if(Isgeotag)
    {   
      if (EXIF_GEOTAG_SUCCESS==exif_geotagging_init())
      {
        retVal = exif_insertGeotags( (exif_position_type *)&pme->gpsInfo,(uint8 *)pme->m_pJPEG,pme->m_nRead,
	  	                                      &outputPtr); 
	 if(retVal == EXIF_GEOTAG_SUCCESS)
        {
          status = JPEG_WriteToFile(pme,&outputPtr);
        }
        else
        {
          status =  EFAILED;
        }
      }
      else
          status =  EFAILED;

      Isgeotag =FALSE;
      exif_geotagging_terminate();
      JPEG_NotifyCaller(pme, status);
    }
    else
#endif /* FEATURE_GEOTAGGING */
    {
    pme->m_Callback.pfnNotify = (PFNNOTIFY) JPEG_DecodeImage;
    pme->m_Callback.pNotifyData = pme; // Applet Specific data
    pme->m_Callback.pfnCancel = NULL;
    AEE_ResumeCallback(&pme->m_Callback, 0);
    }
    break;

  default:
    return;
  }
}

/*===========================================================================

FUNCTION: JPEG_NotifyCaller

DESCRIPTION
   Notifies the caller if the decode process is complete, or has failed

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void JPEG_NotifyCaller(COEMJPEGViewer *pme, int iSuccess)
{
  if(pme->m_pfnNotify)
    pme->m_pfnNotify(pme->m_pUser, (IImage *) pme, &pme->m_info,iSuccess);
}

//===========================================================================
//   Routines required by JPEG decode library
//===========================================================================

  #ifdef FEATURE_QVPHONE
/*===========================================================================

FUNCTION: IS_Videophone

DESCRIPTION
   Determines if videophone is the client requesting the JPEG decoding.

DEPENDENCIES
  none

RETURN VALUE
 Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean      IS_Videophone(COEMJPEGViewer *pme)
{
  IVideoPhone *pIVideoPhone;
  boolean result;

  if (!pme)
    return FALSE;

  (void)ISHELL_CreateInstance(pme->m_pShell, AEECLSID_VIDEOPHONE,
                              (void **)&pIVideoPhone);
  result = AEEVP_IS_VIDEOCALL_CONNECTED(pIVideoPhone);
  (void)IVIDEOPHONE_Release(pIVideoPhone);

  return result;

}

#endif /* FEATURE_QVPHONE*/

#ifdef FEATURE_JPEGD_CONCURRENCY
/*===========================================================================

FUNCTION: IS_AudioVideoDSPEnabled

DESCRIPTION
   Determines if audio/video dsp resource is used during JPEG decoding.

DEPENDENCIES
  Video clkrgm functions and Audio voc_is_active API

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean IS_AudioVideoDSPEnabled()
{
  if (clk_regime_is_on(CLK_RGM_VIDEO_M) || voc_is_active())
    return TRUE;
  else
    return FALSE;
}
#endif

/*===========================================================================

FUNCTION: JPEG_ProceedWithDecode

DESCRIPTION
    Perform ARM or DSP decoding

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/

static void JPEG_ProceedWithDecode(COEMJPEGViewer *pme, int resType)
{
  jpeg_decodeInfo_type jpegdInfo;

  if (!pme)
    return;
  MSG_HIGH("JPEG_ProceedWithDecode",0, 0, 0);

  jpegdInfo.isSync = TRUE;
  pme->m_pOEMJPEGViewerSys->bDecoding = TRUE;

  switch(pme->m_ColorFormat)
  {
#ifdef FEATURE_JPEG_DECODER_REV2
  case JPEG_COLOR_FORMAT_YCBCR420LP:
    jpegdInfo.outFormat = JPEGD_YCbCr420lp;
    break;
  case JPEG_COLOR_FORMAT_YCBCR422LP:
    jpegdInfo.outFormat = JPEGD_YCbCr422lp;
    break;
  case JPEG_COLOR_FORMAT_YCRCB420LP:
    jpegdInfo.outFormat = JPEGD_YCrCb420lp;
    break;
  case JPEG_COLOR_FORMAT_YCRCB422LP:
    jpegdInfo.outFormat = JPEGD_YCrCb422lp;
    break;	
#endif
  case JPEG_COLOR_FORMAT_RGB888:
    jpegdInfo.outFormat = JPEGD_RGB888;
    break;
  case JPEG_COLOR_FORMAT_RGB565:
  default:
    jpegdInfo.outFormat = JPEGD_RGB565;
  }

#ifdef FEATURE_JPEG_ARM_STANDALONE_DECODER
#ifndef FEATURE_JPEG_DECODER_REV2
#ifndef FEATURE_ACM
  resType = ACM_VIEW_PICTURE_ARM;
#endif
#endif
#endif

#ifdef FEATURE_JPEGD_RESIZE
  jpegdInfo.resizeImageWidth  = pme->m_ResizeInfo.resizeImageWidth;
  jpegdInfo.resizeImageHeight = pme->m_ResizeInfo.resizeImageHeight;
  jpegdInfo.aspectRatioFlag   = pme->m_ResizeInfo.aspectRatioFlag;
  jpegdInfo.jpegdResizeFlag   = pme->m_ResizeInfo.jpegdResizeFlag;
#endif

  if (resType == ACM_VIEW_PICTURE_ARM)
  {
    MSG_LOW("ARM based",0, 0, 0);
    jpegdInfo.arm_based = TRUE;
    jpeg_ARM_based_decode((void *) pme->m_pOEMJPEGViewerSys->m_hObject,
                          (jpeg_cmx_cb_func_type) JPEG_DecodeCB, jpegdInfo);
  }
  else
  {
    MSG_LOW("DSP based",0, 0, 0);
    jpegdInfo.arm_based = FALSE;
    graph_jpeg_decode((void *) pme->m_pOEMJPEGViewerSys->m_hObject,
                      (jpeg_cmx_cb_func_type) JPEG_DecodeCB, jpegdInfo);
  }
}
#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
#ifdef FEATURE_CAMERA7500
/* =========================================================================*/
static void ph_status_cb
(
  void                  *data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
)
{
  int resType = ACM_VIEW_PICTURE_QDSP;
  if (cmd_err == CM_PH_CMD_ERR_IN_USE_S
#ifdef FEATURE_QVPHONE
        || (IS_Videophone(m_pJpegViewer))
#endif /* FEATURE_QVPHONE */
     )
  {
   resType = ACM_VIEW_PICTURE_ARM;
  }
  JPEG_ProceedWithDecode(m_pJpegViewer, resType);

}
#endif /* FEATURE_CAMERA7500 */
#endif /* !FEATURE_ACM && !FEATURE_BMP_ACM */
/*===========================================================================

FUNCTION: JPEG_DecodeImage

DESCRIPTION
   Calls the JPEG decoder to start decoding the jpeg data we just buffered

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void JPEG_DecodeImage(void *po)
{
#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
#ifndef FEATURE_CAMERA7500
  int resType = ACM_VIEW_PICTURE_QDSP;
  cm_ph_info_s_type phone_info;
  boolean phone_status;
#endif // FEATURE_CAMERA7500
#endif // !FEATURE_ACM && !FEATURE_BMP_ACM

  m_pJpegViewer = (COEMJPEGViewer *) po;

#if defined (FEATURE_ACM) || defined (FEATURE_BMP_ACM)
  JPEGViewer_ACMAcquire(m_pJpegViewer);  // Acquire ACM resource
  return;
#else // !FEATURE_ACM && !FEATURE_BMP_ACM

#ifdef FEATURE_CAMERA7500
  (void)cm_ph_cmd_get_ph_info(ph_status_cb,NULL,0);
#else
   phone_status = cmph_info_get( &phone_info);

  if (((phone_info.is_in_use) && phone_status)
#ifdef FEATURE_QVPHONE
        || (IS_Videophone(m_pJpegViewer))
#endif /* FEATURE_QVPHONE */
#ifdef FEATURE_JPEGD_CONCURRENCY
        || (IS_AudioVideoDSPEnabled())
#endif /* FEATURE_QVPHONE */
     )
  {
    resType = ACM_VIEW_PICTURE_ARM;
  }

  JPEG_ProceedWithDecode(m_pJpegViewer, resType);
#endif /* FEATURE_CAMERA7500 */
#endif // !FEATURE_ACM && !FEATURE_BMP_ACM
}

/*===========================================================================

FUNCTION: JPEG_DecodeCB

DESCRIPTION
   Callback function called by the JPEG decoder to give us status, info, decoded lines
   etc.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void JPEG_DecodeCB(jpeg_cmx_status_enum_type command, void *po,
                          void *server_data)
{
  COEMJPEGViewerSys *pmeSys;

  OEMJPEG_MUTEX_LOCK()         //Begin Critical Section

  pmeSys = (COEMJPEGViewerSys *)
           AEEObjectMgr_GetObject((AEEObjectHandle) po);

  MSG_HIGH("JPEG_DecodeCB, server_data = %u",server_data, 0, 0);
  if(pmeSys && isJpegAbort)
  {
    MSG_LOW("command = %d",command, 0, 0);
    switch(command)
    {
    case  JPEG_CMX_GET_DATA:
      {
        jpeg_cmx_get_data_type *data = (jpeg_cmx_get_data_type *) server_data;

        if(data->callback_ptr != NULL)
        {
          data->callback_ptr(data->client_data);
        }
      }
      break;
    case  JPEG_CMX_CONTINUE:
      {
        jpeg_cmx_continue_type *data = (jpeg_cmx_continue_type *) (server_data);

        if(data->callback_ptr != NULL)
        {
          data->callback_ptr(data->client_data);
        }
      }
      break;
    case  JPEG_CMX_GET_BUF:
      {
        jpeg_cmx_buf_req_type *cmx_buf = (jpeg_cmx_buf_req_type *)server_data;

        if((cmx_buf->buf_handle)&&(cmx_buf)) /* skip if invalid buf's handles */
        {
          *(cmx_buf->buf_handle) = NULL;
        }

        if((cmx_buf) && (cmx_buf->callback_ptr))
        {
          cmx_buf->callback_ptr(cmx_buf->client_data);
        }
      }
      break;
    case JPEG_CMX_DONE:
    case JPEG_CMX_ABORT:
    case JPEG_CMX_FAILURE:
    case JPEG_CMX_INVALID:
      COEMJPEGViewer_FreeSystemData(pmeSys);
      CALLBACK_Cancel(&pmeSys->pOEMJPEGViewer->m_Callback);
      pmeSys->pOEMJPEGViewer->m_JPEGcmxStatus  = command;
      pmeSys->pOEMJPEGViewer->m_Callback.pfnNotify = (PFNNOTIFY) JPEG_NotifyStatus;
      pmeSys->pOEMJPEGViewer->m_Callback.pNotifyData = (void *) pmeSys->pOEMJPEGViewer;
      pmeSys->pOEMJPEGViewer->m_Callback.pfnCancel = NULL;
#ifdef FEATURE_BREW_3_0
        AEE_ResumeCallback(&pmeSys->pOEMJPEGViewer->m_Callback,
                           pmeSys->pOEMJPEGViewer->m_pAppContext);
#else
        AEE_ResumeEx(&pmeSys->pOEMJPEGViewer->m_Callback, 0, 
                     pmeSys->pOEMJPEGViewer->m_pAppContext);
#endif /*FEATURE_BREW_3_0*/
      pmeSys->bDecoding = FALSE;
      FREE(pmeSys->pOEMJPEGViewer);
      pmeSys->pOEMJPEGViewer = NULL;
      (void)AEEObjectMgr_Unregister(pmeSys->m_hObject);
      FREE(pmeSys);
      pmeSys = NULL;
      jpegdInCancel = FALSE;
      isJpegAbort = FALSE;
      break;
    default:
      break;
    }
  }
  else if(pmeSys)
  {
    switch(command)
    {
    case JPEG_CMX_GET_FILE_SIZE:
      {
        unsigned long *file_size = (uint32 *) server_data;
        *file_size = pmeSys->pOEMJPEGViewer->m_nRead;
      }
      break;

    case  JPEG_CMX_GET_BUF:
      {
        jpeg_cmx_buf_req_type *cmx_buf = (jpeg_cmx_buf_req_type *)server_data;

        if((cmx_buf->buf_handle)&&(cmx_buf)) /* skip if invalid buf's handles */
        {
#ifdef FEATURE_JPEG_DECODER_REV2
          if((pmeSys->pOEMJPEGViewer->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR420LP) ||
             (pmeSys->pOEMJPEGViewer->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR422LP) ||
             (pmeSys->pOEMJPEGViewer->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB420LP) ||
             (pmeSys->pOEMJPEGViewer->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB422LP))
          {
            if(pmeSys->m_pIYCbCr)
              *(cmx_buf->buf_handle) = pmeSys->m_pIYCbCr->pLuma;
          }
          else // else 1
#endif
          {
            if((pmeSys->pOEMJPEGViewer->m_ColorFormat == JPEG_COLOR_FORMAT_RGB565) ||
               (pmeSys->pOEMJPEGViewer->m_ColorFormat == JPEG_COLOR_FORMAT_RGB888))
            {
              if(pmeSys->m_pIDIB)
              {
                *(cmx_buf->buf_handle) = pmeSys->m_pIDIB->pBmp;
              }
            }
            else // else 2
            {
              if(pmeSys->m_pRGBBuf)
              {
                FREE(pmeSys->m_pRGBBuf);
                pmeSys->m_pRGBBuf = NULL;
              }
              pmeSys->m_dwRGBBufSize = cmx_buf->num_bytes;
              pmeSys->m_pRGBBuf = MALLOC(pmeSys->m_dwRGBBufSize);

              if(pmeSys->m_pRGBBuf)
              {
                *(cmx_buf->buf_handle) = pmeSys->m_pRGBBuf;
              }
            } // end else 2
          } // end else 1
        }

        if((cmx_buf) && (cmx_buf->callback_ptr))
        {
          cmx_buf->callback_ptr(cmx_buf->client_data);
        }
      }
      break;

    case  JPEG_CMX_GET_DATA:
      {
        jpeg_cmx_get_data_type *data = (jpeg_cmx_get_data_type *) server_data;

        if((data->buffer)&&(data))
        {
          if(pmeSys->pOEMJPEGViewer->m_thumbnailImage)
          {
            (void)MEMCPY((void *) data->buffer,
                         (void *) (pmeSys->pOEMJPEGViewer->m_pJPEG_thumbnail +
                         data->position), (MIN(data->position + data->num_bytes,
                         pmeSys->pOEMJPEGViewer->m_nRead) - data->position));
          }
          else
          {
            (void)MEMCPY((void *) data->buffer,
                         (void *) (pmeSys->pOEMJPEGViewer->m_pJPEG +
                         data->position), (MIN(data->position + data->num_bytes,
                         pmeSys->pOEMJPEGViewer->m_nRead) - data->position));
          }
        }
        if(data != NULL)
        {
          pmeSys->pOEMJPEGViewer->m_nBytesConsumed += data->num_bytes;

          CALLBACK_Cancel(&pmeSys->pOEMJPEGViewer->m_Callback);
          pmeSys->pOEMJPEGViewer->m_Callback.pfnNotify   = (PFNNOTIFY) data->callback_ptr;
          pmeSys->pOEMJPEGViewer->m_Callback.pNotifyData = (void *) data->client_data;
          pmeSys->pOEMJPEGViewer->m_Callback.pfnCancel   = NULL;
          AEE_ResumeCallback(&pmeSys->pOEMJPEGViewer->m_Callback, 0);
        }
      }
      break;

    case  JPEG_CMX_DATA:
      break;

    case  JPEG_CMX_CONTINUE:
      {
        jpeg_cmx_continue_type *databuff = (jpeg_cmx_continue_type *) (server_data);
        CALLBACK_Cancel(&pmeSys->pOEMJPEGViewer->m_Callback);
        pmeSys->pOEMJPEGViewer->m_Callback.pfnNotify = (PFNNOTIFY) databuff->callback_ptr;
        pmeSys->pOEMJPEGViewer->m_Callback.pNotifyData = (void *) databuff->client_data;
        pmeSys->pOEMJPEGViewer->m_Callback.pfnCancel   = NULL;
        AEE_ResumeCallback(&pmeSys->pOEMJPEGViewer->m_Callback, 0);
      }
      break;

    case  JPEG_CMX_SPEC:
      {
        // Just read the parameters back
        jpeg_cmx_spec_type *buf = (jpeg_cmx_spec_type*) server_data;
        JPEG_CreateIBitmap(pmeSys->pOEMJPEGViewer, (uint16) buf->width,
                           (uint16) buf->height);
        pmeSys->pOEMJPEGViewer->m_rc.dx = (int16)buf->width;
        pmeSys->pOEMJPEGViewer->m_rc.dy = (int16)buf->height;
        pmeSys->pOEMJPEGViewer->m_info.cx = pmeSys->pOEMJPEGViewer->m_info.cxFrame =
                                            (uint16) buf->width;
        pmeSys->pOEMJPEGViewer->m_info.cy = (uint16) buf->height;
      }
      break;

    case JPEG_CMX_DONE:
    case JPEG_CMX_ABORT:
    case JPEG_CMX_FAILURE:
    case JPEG_CMX_INVALID:
       pmeSys->bDecoding = FALSE;
       if(command == JPEG_CMX_ABORT || command == JPEG_CMX_FAILURE ||
          command == JPEG_CMX_INVALID)
       {
         COEMJPEGViewer_FreeData(pmeSys->pOEMJPEGViewer);
         COEMJPEGViewer_FreeSystemData(pmeSys);
       }

       CALLBACK_Cancel(&pmeSys->pOEMJPEGViewer->m_Callback);
       pmeSys->pOEMJPEGViewer->m_JPEGcmxStatus  = command;
       pmeSys->pOEMJPEGViewer->m_Callback.pfnNotify   = (PFNNOTIFY) JPEG_NotifyStatus;
       pmeSys->pOEMJPEGViewer->m_Callback.pNotifyData = (void *) pmeSys->pOEMJPEGViewer;
       pmeSys->pOEMJPEGViewer->m_Callback.pfnCancel   = NULL;
#ifdef FEATURE_BREW_3_0
      AEE_ResumeCallback(&pmeSys->pOEMJPEGViewer->m_Callback,
                         pmeSys->pOEMJPEGViewer->m_pAppContext);
#else
      AEE_ResumeEx(&pmeSys->pOEMJPEGViewer->m_Callback, 0,
                   pmeSys->pOEMJPEGViewer->m_pAppContext);
#endif
       jpegdInCancel = FALSE;
       break;

    default:
      break;
    }
  }
  OEMJPEG_MUTEX_UNLOCK()         //End Critical Section
}

/*===========================================================================

FUNCTION: JPEG_NotifyStatus

DESCRIPTION
   This function is dispatched by the JPEG decoder callback so that notification
   to the app can happen in BREW's context.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void  JPEG_NotifyStatus(void *po)
{

  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
  MSG_HIGH("Status = %d",pme->m_JPEGcmxStatus, 0, 0);

  if(pme)
  {
#if defined (FEATURE_ACM) || defined(FEATURE_BMP_ACM)
      if(pme->m_resState == ACM_RESOURCE_ACQUIRED)
         JPEGViewer_ACMFree(pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM

    jpegdInCancel = FALSE;

    switch(pme->m_JPEGcmxStatus)
    {
    case  JPEG_CMX_DONE:
      pme->m_bImageDecoded = TRUE;
      JPEG_NotifyCaller(pme, SUCCESS);
      break;
    case  JPEG_CMX_ABORT:
    case  JPEG_CMX_FAILURE:
      JPEG_NotifyCaller(pme, EFAILED);
      break;
    case  JPEG_CMX_INVALID:
      JPEG_NotifyCaller(pme, EBADITEM);
      break;
    default:
      break;
    }
  }
}
/*===========================================================================

FUNCTION COEMJPEGViewer_CreateIBitmap

DESCRIPTION
   This callback function is used to asynchronously read the stream for the
   JPEG image. If the function blocks for data on the stream, it goes and parses
   the JPEG buffer for the first image, so that atleast that can be drawn.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void JPEG_CreateIBitmap(COEMJPEGViewer *pme, uint16 x, uint16 y)
{
  uint8 clrDepth;

  if(pme->m_nDeviceClrs <= 8)
    clrDepth = 8;
  else
    clrDepth = (uint8)pme->m_nDeviceClrs;

  switch(pme->m_ColorFormat)
  {
#ifdef FEATURE_JPEG_DECODER_REV2
  case JPEG_COLOR_FORMAT_YCBCR420LP:
    (void)YCbCr_New(IYCBCR_COLORSCHEME_420LP, x, y,
                    &pme->m_pOEMJPEGViewerSys->m_pIYCbCr);
    break;

  case JPEG_COLOR_FORMAT_YCBCR422LP:
    (void)YCbCr_New(IYCBCR_COLORSCHEME_422LP, x, y,
                    &pme->m_pOEMJPEGViewerSys->m_pIYCbCr);
    break;
  case JPEG_COLOR_FORMAT_YCRCB420LP:
    (void)YCbCr_New(IYCRCB_COLORSCHEME_420LP, x, y,
                    &pme->m_pOEMJPEGViewerSys->m_pIYCbCr);
    break;

  case JPEG_COLOR_FORMAT_YCRCB422LP:
    (void)YCbCr_New(IYCRCB_COLORSCHEME_422LP, x, y,
                    &pme->m_pOEMJPEGViewerSys->m_pIYCbCr);
	break;
#endif

  case JPEG_COLOR_FORMAT_RGB565:
  case JPEG_COLOR_FORMAT_RGB888:
    (void)IDISPLAY_CreateDIBitmap(pme->m_pDisplay,
                                  (IDIB**)&pme->m_pOEMJPEGViewerSys->m_pIDIB,
                                  clrDepth, x, y);

    if(pme->m_pOEMJPEGViewerSys->m_pIDIB)
    {
      //We can set the pitch correctly here
      pme->m_pOEMJPEGViewerSys->m_pIDIB->nPitch = (int16)(x * (clrDepth/8));
      pme->m_pOEMJPEGViewerSys->m_pIDIB->cx = x;
      pme->m_pOEMJPEGViewerSys->m_pIDIB->cy = y;
      //This is the device display color format. The JPEG bitmap will be blt to this format
      switch (clrDepth)
      {
      case 24:
        pme->m_pOEMJPEGViewerSys->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_888;
        break;
      case 18:
        pme->m_pOEMJPEGViewerSys->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_666;
        break;
      case 16:
        pme->m_pOEMJPEGViewerSys->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_565;
        break;
      case 8:
        pme->m_pOEMJPEGViewerSys->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_332;
        break;
      default:
        pme->m_pOEMJPEGViewerSys->m_pIDIB->nColorScheme = IDIB_COLORSCHEME_565;
        MSG_ERROR("Device Color Format. %d", clrDepth, 0, 0);
      }
    }
    break;

  default:
    MSG_ERROR("Unsupported JPEG_COLOR_FORMAT", 0, 0, 0);
  }
}

  #if !defined(AEE_SIMULATOR)
    #if defined(JPEG_ALLOC_DECODE_BUF)

static void JPEG_SetBuffer(COEMJPEGViewer *pme)
{
  pme->m_pDecodeBuf = (byte *) MALLOC(OEMCMX_JPEG_DECODE_BUFSIZE);
  if(pme->m_pDecodeBuf)
    graph_jpeg_set_buf((uint8 *) pme->m_pDecodeBuf,
                       OEMCMX_JPEG_DECODE_BUFSIZE, NULL);
}
    #endif //JPEG_ALLOC_DECODE_BUF
  #endif //!AEE_SIMULATOR

/* ==========================================================================
FUNCTION OEMJPEG_GETIMAGEDATA
DESCRIPTION
   Returns the pointer to the internal Device Independant Bitmap structure.
   The returned pointer is valid until IIMAGE_Release is called.
============================================================================= */
void *OEMJPEG_GetImageData(IImage *po)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
#ifdef FEATURE_JPEG_DECODER_REV2
  if((pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR420LP) ||
     (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCBCR422LP) ||
     (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB420LP) ||
     (pme->m_ColorFormat == JPEG_COLOR_FORMAT_YCRCB422LP))
    return pme->m_pOEMJPEGViewerSys->m_pIYCbCr;
  else
#endif
    return pme->m_pOEMJPEGViewerSys->m_pIDIB;
} /* END OEMJPEG_GetImageData */


/* ==========================================================================
FUNCTION OEMJPEG_ISTHUMBNAIL
DESCRIPTION
   Returns the boolean to the internal Device Independant Bitmap structure.
   The returned boolean is valid until IIMAGE_Release is called.
============================================================================= */
boolean OEMJPEG_IsThumbnail(IImage *po)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
  return pme->m_thumbnailImage;
} /* END OEMJPEG_ISTHUMBNAIL */

/* ==========================================================================
FUNCTION OEMJPEG_SetTHUMBNAIL
DESCRIPTION
   Set the decoding preference for thumbnail.
   TRUE  - If thumbnail presents, decode thumbnail otherwise decode main image
           this option will save time in album, slide-show and display on phone.
   FALSE - Decode main image only

============================================================================= */
void OEMJPEG_SetThumbnail(boolean flag)
{
  JPEG_thumbnail = flag;

} /* END OEMJPEG_THUMBNAIL */

/* ==========================================================================
FUNCTION OEMJPEG_GETIMAGEDATA
DESCRIPTION
   Replaces the internal Device Independant Bitmap structure.

   Returns a pointer to the previous Device Independant Bitmap structure,
   which must be released by the caller.
============================================================================= */
IDIB *OEMJPEG_SetImageData(IImage  *po, IDIB *dib)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
  IDIB *prev = pme->m_pOEMJPEGViewerSys->m_pIDIB;
  pme->m_pOEMJPEGViewerSys->m_pIDIB = dib;
  if(pme->m_pOEMJPEGViewerSys->m_pIDIB)
  {
    pme->m_ReadState = JPEG_COMPLETE_STATE;
    pme->m_info.cx = dib->cx;
    pme->m_info.cy = dib->cy;
    pme->m_info.cxFrame = dib->cx;
    pme->m_xOffset = 0;
    pme->m_yOffset = 0;
    pme->m_rop = AEE_RO_COPY;
  }
  return prev;
} /* END OEMJPEG_GetIDIB */

/*===========================================================================
FUNCTION: OEMJPEG_extractThumbnail
DESCRIPTION
  Extract the thumbnail image from the EXIF JPEG image.

  This function allocates the output buffer. It is the caller's
  responsibility to free the outPtr when data have been consumed.
============================================================================= */
boolean OEMJPEG_extractThumbnail(IImage *po, uint8 **outPtr, uint32 *out_buf_size)
{
  boolean status = FALSE;
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
  /* At this point, thumbnail dimension is not available, so QVGA size */
  /* is presumed for worst case consideration */
  *outPtr = (uint8 *) MALLOC(JPEG_THUMBNAIL_BUFSIZE);
  if(*outPtr)
  {
    *out_buf_size = JPEG_THUMBNAIL_BUFSIZE;
    status = jpeg_extract_thumbnail(pme->m_pJPEG, *outPtr, out_buf_size);
  }
  else
  {
    *out_buf_size = 0;
  }

  if(status)
  {
    return TRUE;
  }
  else
  {
    FREEIF(*outPtr);
    *outPtr = NULL;
    *out_buf_size = 0;
    return FALSE;
  }
} /* END OEMJPEG_extractThumbnail */

/* ==========================================================================
FUNCTION OEMJPEG_GetExifInfo
DESCRIPTION
   Returns a pointer to the JPEG exif info data.
   The returned pointer is valid until IIMAGE_Release is called.
============================================================================= */
void *OEMJPEG_GetExifInfo(IImage *po)
{
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
  return &pme->m_ExifInfo;
}

boolean OEMJPEG_GetExifValue(IImage *po, uint16 tag_id, void **data, uint32 *len)
{
  exif_valueinfo_type value;
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;
  if(exif_get_value(&value, &pme->m_ExifInfo, tag_id) == EXIF_GET_VALUE_SUCCESS)
  {
    *data = value.offset;
    *len  = value.count;
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void OEMJPEG_SetExifAsciiValue(IImage *po, uint16 tag_id, void *data, uint32 len)
{
  exif_valueinfo_type value;
  COEMJPEGViewer *pme = (COEMJPEGViewer *) po;

  value.type = (uint16)EXIF_ASCII;
  value.count = len;
  value.offset = data;
  (void)exif_set_value(&pme->m_ExifInfo, &value, tag_id);
}

#if defined (FEATURE_ACM)
/*===========================================================================

FUNCTION JPEGViewer_ACMInit

DESCRIPTION
   Initializes the ACM - Creates AppResCtrl interface.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS or EFAILED

SIDE EFFECTS
  none
===========================================================================*/
static int JPEGViewer_ACMInit(COEMJPEGViewer *pme)
{
#if defined (FEATURE_ODM_UI)
  IShell *pIShell = AEE_GetShell();
  pme->m_resType = ACM_RESOURCE_UNKNOWN;
  pme->m_resState = ACM_RESOURCE_UNKNOWN;
  if(ISHELL_CreateInstance(pIShell, AEECLSID_APPRESCTL, (void **)&pme->m_pIAppRes)
                          != SUCCESS)
  {
    MSG_ERROR("Failed to create AppResCtl", 0, 0, 0);
    return EFAILED;
  }
#else // !FEATURE_ODM_UI
  pme->m_resType  = ACM_RESOURCE_UNKNOWN;
  pme->m_resState = ACM_RESOURCE_UNKNOWN;
#endif // FEATURE_ODM_UI
  return SUCCESS;
}

/*===========================================================================

FUNCTION JPEGViewer_ACMAcquire

DESCRIPTION
   Acquire ACM resource

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS or EFAILED

SIDE EFFECTS
  none
===========================================================================*/
static int JPEGViewer_ACMAcquire(COEMJPEGViewer *pme)
{
  uint32 resStatus = ACM_RESOURCE_UNKNOWN;
  int nRet;
  boolean bUsed = FALSE;
  ACM_resource_state_type pdwstatus = ACM_RESOURCE_UNKNOWN;
#if !defined (SIRIUS_PLATFORM)
   ACONTEXT *pacRestore;
#endif

  // Determine if we should use ARM or DSP to decode
  pme->m_resType = ACM_VIEW_PICTURE_QDSP;
  OEMACM_StateMgr_IsResourceUsed(ACM_QDSP, &bUsed);
  if(bUsed)
  {
    // If DSP is used, try ARM
    OEMACM_StateMgr_IsResourceUsed(ACM_ARM_JPEG_DECODER, &bUsed);
    if(bUsed) // If ARM is used, error out...
    {
      return EFAILED;
    }
    pme->m_resType = ACM_VIEW_PICTURE_ARM;
  }

#if defined (FEATURE_ODM_UI)
  // First try acquiring from priority manager
  // to let ACM infer caller set GLOBAL_FORM_OWNER id
  IAPPRESCTL_SetParm(pme->m_pIAppRes, GLOBAL_FORM_OWNER,
                     AEE_GetAppContextCls((PACONTEXT)(pme->m_pAppContext)));
  nRet = IAPPRESCTL_Acquire(pme->m_pIAppRes, pme->m_resType, &resStatus);
  if(nRet != SUCCESS || resStatus == ACM_RESOURCE_BUSY)
  {
    MSG_ERROR("OEMJPEG - IAPPRESCTL_Acquire failure", 0, 0, 0);
    return EFAILED;
  }

  // Register for status changes
  pme->m_cbACMdata.pdata = pme;
  pme->m_cbACMdata.state = ACM_RESOURCE_UNKNOWN;
  if(OEMACM_StateMgr_OnStatusChangeNotify(pme->m_pIAppRes,
                                          (PFNNOTIFY)JPEGViewer_ACMStatusChange,
                                          &pme->m_cbACMdata) != SUCCESS)
  {
     MSG_ERROR("OEMJPEG - Can't register ACM callback", 0, 0, 0);
     return EFAILED;
  }

  //Get status from state manager
  if(OEMACM_StateMgr_GetResourceState(pme->m_pIAppRes, &pdwstatus) != SUCCESS)
  {
    MSG_ERROR("OEMJPEG - Error getting resource state ", 0, 0, 0);
    return EFAILED;
   }

  if(pdwstatus == ACM_RESOURCE_ACQUIRED)
  {
    MSG_HIGH("OEMJPEG - Acquired in Get resource", 0, 0, 0);
    pme->m_resState = ACM_RESOURCE_ACQUIRED;
    JPEG_ProceedWithDecode(pme, pme->m_resType);
    return SUCCESS;
  }
  return EFAILED;
#else // !FEATURE_ODM_UI
  // First try acquiring from priority manager
  // enter the application context so we can figure out the right
  // appID in OEMACM_PriorityMgr_Acquire
  pacRestore = AEE_EnterAppContext (pme->m_pAppContext);
  nRet = OEMACM_PriorityMgr_Acquire (pme, pme->m_resType, &pdwstatus);
  AEE_LeaveAppContext(pacRestore);

  if(nRet != SUCCESS || resStatus == ACM_RESOURCE_BUSY)
  {
    MSG_ERROR("OEMJPEG - OEMACM_PriorityMgr_Acquire failure", 0, 0, 0);
    return EFAILED;
  }

  // Register for status changes
  pme->m_cbACMdata.pdata = pme;
  pme->m_cbACMdata.state = ACM_RESOURCE_UNKNOWN;
  if(OEMACM_StateMgr_OnStatusChangeNotify(pme,
                                          (PFNNOTIFY)JPEGViewer_ACMStatusChange,
                                          &pme->m_cbACMdata) != SUCCESS)
  {
    MSG_ERROR("OEMJPEG - Can't register ACM callback", 0, 0 ,0);
    return EFAILED;
  }

  if(pdwstatus == ACM_RESOURCE_ACQUIRED)
  {
    MSG_HIGH("OEMJPEG - Acquired in Get resource", 0, 0, 0);
    pme->m_resState = ACM_RESOURCE_ACQUIRED;
    JPEG_ProceedWithDecode(pme, pme->m_resType);
    return SUCCESS;
  }
  return SUCCESS;
#endif // FEATURE_ODM_UI
}

/*===========================================================================

FUNCTION JPEGViewer_ACMStatusChange

DESCRIPTION
   Receive status changes from ACM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
static void JPEGViewer_ACMStatusChange(void *p)
{
  ACM_Callback_data_type *cbdata = (ACM_Callback_data_type *)p;
  COEMJPEGViewer *pme = (COEMJPEGViewer *)cbdata->pdata;

  if(cbdata->state == ACM_RESOURCE_ACQUIRED)
  {
    MSG_HIGH("OEMJPEG - Acquired in Notify",0,0,0);
    pme->m_resState = ACM_RESOURCE_ACQUIRED;
    JPEG_ProceedWithDecode(pme, pme->m_resType);
  }
  else
  {
    // if (cbdata->state == ACM_RESOURCE_BUSY) {
    MSG_HIGH("OEMJPEG - Free requested", 0, 0, 0);

    OEMJPEG_MUTEX_LOCK()
    if(pme->m_pOEMJPEGViewerSys->bDecoding)
    {
      COEMJPEGViewer_CancelDecode(pme->m_pOEMJPEGViewerSys);
    }
    OEMJPEG_MUTEX_UNLOCK()

    pme->m_resState=ACM_RESOURCE_UNKNOWN;
  }
}

/*===========================================================================
FUNCTION JPEGViewer_ACMFree

DESCRIPTION
   Free any ACM resource

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
static void JPEGViewer_ACMFree(COEMJPEGViewer *pme)
{
#if defined (FEATURE_ODM_UI)
  if(pme->m_pIAppRes)
  {
    IAPPRESCTL_Free(pme->m_pIAppRes, NULL);
    IAPPRESCTL_Release(pme->m_pIAppRes);
    pme->m_pIAppRes = NULL;
  }
#else // !FEATURE_ODM_UI
  OEMACM_PriorityMgr_Free (pme) ;
#endif // FEATURE_ODM_UI

  pme->m_resState = ACM_RESOURCE_UNKNOWN;
}
#endif // FEATURE_ACM

#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM


#endif //FEATURE_QCT_JPEG_DECODER
