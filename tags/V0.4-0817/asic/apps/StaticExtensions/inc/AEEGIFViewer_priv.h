/*
  ============================================================================

FILE:  AEEGIFViewer_priv.h

SERVICES:  
        GIF Viewer interfaces and types for other AEE files 

GENERAL DESCRIPTION:

REVISION HISTORY: 
        Fri Nov 2, 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2001 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef AEEGIFVIEWER_PRIV_H
#define AEEGIFVIEWER_PRIV_H

#include "AEE.h"
#include "AEEMimeTypes.h"
#include "AEEStdLib.h"
#if defined(AEE_STATIC) 
#include "AEE_OEM.h"
#include "OEMFeatures.h"
#else
#include "AEEShell.h"
#endif
#include "AEEImage.h"
#include "GIFViewerExtension.h"

#include "AEEIBitmapScale.h"
#include "AEEBilinearScale.bid"

#ifndef IPARM_ANIMATIONEND
#define IPARM_ANIMATIONEND 333 // p1 = PFNNOTIFY, p2 = pUser - only supported by GIF types
#endif

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

//
// Use this constant to indicate whether there are shared resources for 
// all GIFs.  This helps to reduce memory...
//
#define GIF_SHARE_MEM      1

#define MIN_ANIMATION_RATE 150
#if defined(SIRIUS_PLATFORM) 
#error code not present
#endif
#if defined (FEATURE_SIRIUS_GIF_IMAGE_CPU_OPTIMIZATION)
#error code not present
#endif 

#define DEF_ANIMATION_RATE 150l
#define COEMIMAGE_MIN_READ 500

#define GIFEXTID     0x21  //GIF Extension Identifier
#define GIFIMGBLKID  0x2C  //GIF Image Block Identifier
#define GIFTERMID    0x3B  //GIF Terminator Identifier

#define GIFTEXTEXT   0x01  //Plain Text Extension
#define GIFGRCTLEXT  0xF9  //GIF Graphical Control Extension
#define GIFCMTEXT    0xFE  //Comment Extension
#define GIFAPPEXT    0xFF  //GIF Application Extension

//Maximum GIF codes (See GIF spec. for details)
#define MAX_GIFCODES   4095
#define STACK_SIZE     (sizeof(int16) * (MAX_GIFCODES + 1))

//GIF Disposal codes specified in the Graphics Application Header
#define GIFLEAVEASIS   1
#define GIFRESTOREBCK  2
#define GIFRESTOREORIG 3


//Error codes for the GIF decoder
#define OEMGIFERR_OUT_OF_MEMORY  -10
#define OEMGIFERR_BAD_CODE_SIZE  -20
#define OEMGIFERR_READ_ERROR     -1
#define OEMGIFERR_WRITE_ERROR    -2
#define OEMGIFERR_OPEN_ERROR     -3
#define OEMGIFERR_CREATE_ERROR   -4
#define OEMGIFERR_OTHER_ERROR    -5
   
#ifdef _MSC_VER 
#pragma pack(push,1)
#endif

// GIF Header
typedef PACKED struct _AEE_GIF_Hdr {
   byte szGifSig[3];      //GIF File Signature
   byte szGifVersion[3];  //GIF File Version
} AEE_GIF_Hdr;

//GIF Logical Screen Descriptor
typedef PACKED struct _AEE_GIF_LSD {
   word  wLScreenW;           //Logical Screen Width
   word  wLScreenH;           //Logical Screen Height
   byte  ColorTabSize:3;      //Global Color Table Size
   byte  ColorTabSrtFlag:1;   //Global Color Table Sort Flag
   byte  BitsPerPix:3;        //Bits per Pixel
   byte  ColorTabFlag:1;      //Global Color Table Flag
   byte  bBGColor;            //Background Color
   byte  bPixAspRatio;        //Pixel Aspect Ratio
} AEE_GIF_LSD;

//Global Table Entry
typedef PACKED struct _AEE_GIF_GTEntry {
   byte  bRedComp;   //Red Component;
   byte  bGreenComp; //Green Component
   byte  bBlueComp;  //Blue Component;
} AEE_GIF_GTEntry;

typedef PACKED struct _AEE_GIF_ImgHdr {
   word  wLOffset;            //Left Offset of the image within the logical Screen
   word  wTOffset;            //Offset of the image from the top of the logical Screen
   word  wImgWidth;           //Width of the image
   word  wImgHeight;          //Height of the image
   byte  LclClrTabSize:3;     //Local Color Table Size
   byte  Reserved:2;          //Reserved
   byte  LclClrTabSrtFlag:1;  //Local Color table Sort flag
   byte  IlaceFlag:1;         //Interlace Flag
   byte  LclClrTabFlag:1;     //Local Color Table flag
} AEE_GIF_ImgHdr;


typedef PACKED struct _AEE_GIF_GraphExt {
   byte    TranspFlag:1;
   byte    UIFlag:1;
   byte    Disposal:3;
   byte    Reserved:1;
   word    wDelay;
   byte    TranspColor;
} AEE_GIF_GraphExt;


typedef PACKED struct _AEE_GIF_Img  AEE_GIF_Img;

PACKED struct _AEE_GIF_Img {
   int16             iImageNo;
   AEE_GIF_GraphExt  GIFExt;
   byte *            pImg;
   AEE_GIF_Img *     pNext;
};

typedef PACKED struct _AEE_GIF_Decoder {
   int16    navail_bytes;  // # bytes left in block
   int16    nbits_left;    // # bits left in current byte
   byte *   pbytes;        // Pointer to next byte in block
   byte *   pLineBuf;      // Line buffer
   int16    curr_size;      // The current code size
   int16    clear;         // Value for a clear code 
   int16    ending;        // Value for a ending code
   int16    newcodes;      // First available code 
   int16    top_slot;       // Highest code for current size
   int16    slot;          // Last read code
   int16    curLineNo;     // Current line no. being decoded
   byte     curPassNo;     // Current Pass No. (for interlaced images)
   byte     b1;            // Current byte
} AEE_GIF_Decoder;


typedef enum _AEE_GIF_DECODE_STATE {
   AEE_GIF_NONE,
   AEE_GIF_COMPLETE
} AEE_GIF_DECODE_STATE;

#ifdef _MSC_VER 
#pragma pack(pop) 
#endif


#define BYTE_BUFF_SIZE        257

typedef struct _COEMGIFViewer     COEMGIFViewer;

struct _COEMGIFViewer             
{

   DECLARE_VTBL(IImage)

   IDisplay *                 m_pDisplay;   // Display
   IShell *                   m_pShell;     // Shell
   AEEImageInfo               m_info;       // generic info

   uint32                     m_cls;        // our class
   int32                      m_nRefs;      // reference count
   AEERect                    m_rc;         // destination x,y,cx,cy
   AEERect                    m_rcGIFFrame; // Gif frame x,y,dx,dy
   int16                      m_xOffset;    // x offset into image
   int16                      m_yOffset;    // y offset into image
   AEERasterOp                m_rop;        // raster op
   IAStream *                 m_pStream;    // Data stream passed to this object
   PFNIMAGEINFO               m_pfnNotify;
   void *                     m_pUser;
   byte *                     m_pGIF;       // GIF data
   byte *                     m_pMaxGIF;    // Position of last GIF data
   AEE_GIF_Img *              m_pImgList;   // Linked list of image addresses, transparent colors, etc
   dword                      m_dwGIFBufSize;
   dword                      m_nRead;      // No. of bytes read from the stream
   AEE_GIF_GTEntry *          m_pGIFClrTab; // Pointer to the GIF color table

#if !defined(GIF_SHARE_MEM)
   byte *                     m_pSuffix;    // Suffix
   uint16 *                   m_pPrefix;    // Prefix
   byte *                     m_pStack;     // Decoded Stack
   byte *                     m_pByteBuff;  // Byte buffer to decode into
#endif
   IDIB *                     m_pIDIB;      // Pointer to DIB structure in the IBitmap interface
   IDIB *                     m_pIOutDIB;   // Pointer to DIB structure to be shown.
   IBitmap *                  m_pIScreenBuffer;
   byte *                     m_pLineBuff;  // Buffer to output BMP line
   AEECallback                m_NotifyCBStruct; //AEECallback used to notify user
   RGBVAL                     m_clrMap[256];// Color map to map color indices to values
   RGBVAL                     m_clrBack;    // Background color before to restore

   int                        m_DecodeError; //Decode Error
   word                       m_wGIFNumClrs;// Number of colors in the GIF image
   int16                      m_nCurFrame;  // current frame
   int16                      m_nFrames;    // number of frames

   int16                      m_nRate;       //Rate at which the frames are to be animated
   AEE_GIF_DECODE_STATE       m_DecodeState; //How much of the file has been decoded. Useful
                                             //to display first frame of a partially downloaded
                                             //GIF.
   byte                       m_GIFBackgrnd; //Background color, if any, in the GIF
   boolean                    m_bRealloc;
   boolean                    m_bAnimating;  //Animation in progress
   boolean                    m_bIsClrTabGlobal; //Flag used to determine if the color table needs to be set
   boolean                    m_bImageRead;
   boolean                    m_bHasLclClrTabFrames;

   PFNNOTIFY                  pfnRedraw;
   void *                     pRedraw;
   PFNNOTIFY                  pfnAnimationEnd;
   void *                     pAnimationEnd;

   int16                      m_nLastFrameDrawn;
   uint16                     m_nLastCx;
   uint16                     m_nLastCy;
   boolean                    bStreamChanged;
   boolean                    bHaveDrawn;
   uint32                     dwFlags;       // behavior/compatibility flags
   IBitmapScale *             pScale;
};

#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */

#endif // AEEGIFVIEWER_PRIV_H
