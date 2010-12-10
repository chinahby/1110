/*======================================================
FILE:  AEEGIFViewer.c

SERVICES:  AEE GIF Viewer Interface

GENERAL DESCRIPTION:
   AEE GIF Viewer Interface

  This interface is derived from the IImage class and provides
  methods to view still and animated GIF images. The GIF images
  are read from a stream into a buffer using the COEMGIFViewer_SetStream
  method. The GIF is read into a buffer and parsed using the GIF parsing functions.
  A linked list of pointers to each GIF image is created and the Draw and Start
  (animate) routines construct the Windows BMP file for each image before displaying.
  This  way, it is possible to display animated GIF images which contain a large number
  of frames without having to construct a BMP for each GIF frame, which can require
  prohibitive amount of memory.
  In order to support streaming from the network, etc. This viewer allows the user
  to display part of the first image(if only a part of it has been received), or the
  complete first image if there are more images to be received on the stream. No
  animation is allowed till all frames are received, nor is the user allowed to
  display anything other than the partial or complete first frame.

PUBLIC CLASSES AND STATIC FUNCTIONS:

                    COEMGIFViewer_AddRef
                    COEMGIFViewer_Release
                    COEMGIFViewer_GetInfo
                    COEMGIFViewer_SetParm
                    COEMGIFViewer_Draw
                    COEMGIFViewer_DrawFrame
                    COEMGIFViewer_Start
                    COEMGIFViewer_Stop
                    COEMGIFViewer_SetStream
                    COEMGIFViewer_Notify
                    COEMGIFViewer_HandleEvent

INITIALIZATION & SEQUENCING REQUIREMENTS:


The GIF decoder functions (GIF_Decoder, GIF_InitDecoder, and GIF_GetNextCode)
are based on
"DECODE.C - An LZW decoder for GIF", Copyright (C) 1987, by Steven A. Bennett
The copyright section of DECODE.C is given below:

"* DECODE.C - An LZW decoder for GIF
 * Copyright (C) 1987, by Steven A. Bennett
 *
 * Permission is given by the author to freely redistribute and include
 * this code in any program as long as this credit is given where due.
 *
 * In accordance with the above, I want to credit Steve Wilhite who wrote
 * the code which this is heavily inspired by..."

        Copyright © 1999-2001 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef _BUILD_AS_STATIC_EXTENSION_
#include "StdAfx.h"
#include "GIFViewerStaticExt.h"
#include "AEEModTable.h"
#endif //_BUILD_AS_STATIC_EXTENSION_

#include "BREWVersion.h"
#include "AEEGIFViewer_priv.h"

#include "AEEHeap.h"
#if defined(AEE_STATIC)
#include "AEE_OEM.h"
#include "OEMHeap.h"
#endif
#include "AEEStdLib.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#if MIN_BREW_VERSION(3,1)
#undef SECTION_AEEGIFVIEWER
#define SECTION_AEEGIFVIEWER
#endif

#ifndef RELEASEIF
#define RELEASEIF(p)       RELEASEPPIF((IBase**) (void *) &p)

static __inline void RELEASEPPIF(IBase **p) {
   if (*p) {
      IBASE_Release(*p);
      *p = 0;
   }
}

#endif //RELEASEIF

#ifndef ADDREFIF
#define ADDREFIF(p)        do { if (p) IBASE_AddRef((IBase*) (void *) p); } while(0)
#endif

typedef struct                       /**** Colormap entry structure ****/
{
  unsigned char  rgbBlue;          /* Blue value */
  unsigned char  rgbGreen;         /* Green value */
  unsigned char  rgbRed;           /* Red value */
  unsigned char  rgbReserved;      /* Reserved */
} RGBQUAD;


#define GIF_PACK_RGB565(out, r, g, b) (out) = ((((r)&0xF8)<<8)+(((g)&0xFC)<<3)+(((b)&0xF8)>>3));


//
// Module method declarations...
//
#ifdef _BUILD_AS_STATIC_EXTENSION_
void GIFViewer_Init(IShell * ps);
int  GIFViewer_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj);
#endif //_BUILD_AS_STATIC_EXTENSION_

static int          COEMGIFViewer_New(IModule *piModule, IShell * po, AEECLSID cls, void **ppo);
static uint32       COEMGIFViewer_AddRef(IImage * po) ;
static uint32       COEMGIFViewer_Release(IImage * po) ;
static void         COEMGIFViewer_GetInfo(IImage * po, AEEImageInfo * pi) ;
static void         COEMGIFViewer_SetParm(IImage * po, int nParm, int n1, int n2) ;
static void         COEMGIFViewer_Draw(IImage * po, int x, int y) ;

static void         COEMGIFViewer_DrawFrame(IImage * po,int nFrame, int x, int y) ;
static void         COEMGIFViewer_Start(IImage * po, int x, int y) ;
static void         COEMGIFViewer_Stop(IImage * po) ;
static void         COEMGIFViewer_Animate(COEMGIFViewer * pme) ;
static void         COEMGIFViewer_SetStream(IImage * po, IAStream * ps) ;
static void         COEMGIFViewer_Notify(IImage * po, PFNIMAGEINFO pfn, void * pUser) ;
static boolean      COEMGIFViewer_HandleEvent(IImage * po, AEEEvent e, uint16 w, uint32 dw) ;
static void         COEMGIFViewer_FreeData(COEMGIFViewer * pme) ;
static void         COEMGIFViewer_ReadableCB(COEMGIFViewer * pme) ;
static boolean      COEMGIFViewer_Realloc(COEMGIFViewer * pme) ;

static void         GIF_DispatchNotify(COEMGIFViewer * pme, uint32 errCode) ;
static void         GIF_NotifyUser(void * po) ;

//GIF Parsing Functions
static boolean      GIF_SetData(IImage * po) ;

static byte *       GIF_SkipOverExtBlocks(byte * pGIFData, AEE_GIF_GraphExt * pGraphExt) ;
static boolean      GIF_VerifyImageStart(byte *pImgBlock, AEE_GIF_LSD  * pLScrnDesc) ;
static boolean      GIF_AddImageToList(COEMGIFViewer * pme, byte * pbIdent,
                                       word wNumFrames,
                                       AEE_GIF_GraphExt GraphExt) ;
static void         GIF_FreeImageList(COEMGIFViewer * pme) ;
static AEE_GIF_Img* GIF_GetImageNode(COEMGIFViewer * pme, word wFrameNo) ;
#if defined(GIF_SHARE_MEM)
static void CleanupSharedMem(void) ;
#endif
static void COEMGIFViewer_ScaleImage(COEMGIFViewer * pMe, int n1, int n2, int x, int y, AEERasterOp rop);


//Functions to decode GIF image
static int16        GIF_Decoder(COEMGIFViewer * pme, word nFrameId, int* piBadCodeCount) ;

static boolean      GIF_AllocateDict(COEMGIFViewer * pme) ;
static void         GIF_FreeDict(COEMGIFViewer * pme) ;
static int16        GIF_InitDecoder(AEE_GIF_Decoder *pDecodeStruct,int16 size) ;

static int16        GIF_GetNextCode(COEMGIFViewer * pme,
                                    AEE_GIF_Decoder *pDecodeStruct,
                                    byte * pByteBuff, byte **pGIFImgData) ;

static int          GIF_GetByte(COEMGIFViewer * pme, byte **pGIFImgData) ;

static int          GIF_LineOut(COEMGIFViewer * pme, int len,
                                AEE_GIF_ImgHdr * pGifImgHdr,
                                AEE_GIF_Decoder * pDecodeStruct) ;
static void         GIF_SetColorMap(COEMGIFViewer * pme,
                                         AEE_GIF_GTEntry *  pGIFClrTab,
                                         uint16 numColors) ;

//
// Constant Data...
//
static const int32 code_mask[13] = {
   0,
   0x0001, 0x0003,
   0x0007, 0x000F,
   0x001F, 0x003F,
   0x007F, 0x00FF,
   0x01FF, 0x03FF,
   0x07FF, 0x0FFF
};

//
// File/Database Module
//

#if defined(GIF_SHARE_MEM)
static byte *              gpSuffix = NULL;
static uint16 *            gpPrefix = NULL;
static byte *              gpStack = NULL;
static byte *              gpByteBuff = NULL;
static int                 gnRefs = 0;
#endif

#if defined(AEE_STATIC)
#define GIF_MALLOC OEM_Malloc
#define GIF_FREE   OEM_Free
#else
#define GIF_MALLOC MALLOC
#define GIF_FREE   FREE
#endif

static const VTBL(IImage) gOEMGIFViewerMethods = {COEMGIFViewer_AddRef,
                                                  COEMGIFViewer_Release,
                                                  COEMGIFViewer_Draw,
                                                  COEMGIFViewer_DrawFrame,
                                                  COEMGIFViewer_GetInfo,
                                                  COEMGIFViewer_SetParm,
                                                  COEMGIFViewer_Start,
                                                  COEMGIFViewer_Stop,
                                                  COEMGIFViewer_SetStream,
                                                  COEMGIFViewer_HandleEvent,
                                                  COEMGIFViewer_Notify};

// Define this if you are intending to build this as a static extension.
// Otherwise this implementation file can be statically built and linked
// in the target build.
#ifdef _BUILD_AS_STATIC_EXTENSION_

// Create an AEEStaticClass array that lists the information about the interface
const AEEStaticClass gGIFViewerExtClasses[] = {
   { AEECLSID_GIF, ASCF_UPGRADE, 0, GIFViewer_Init, GIFViewer_New },
   { 0, 0, 0, 0, NULL},
};


/*===========================================================================

Function:  GetStaticClassEntries()

Description:
   This function is required to be exposed if this interface will be
   exposed as a static extension DLL. BREW Emulator loads all the DLLs
   that expose this function as a Static Extension DLL.

Prototype:
      const AEEStaticClass* GetStaticClassEntries(void)

Parameters: None

Return Value:
   return an array of AEEStaticClass

Comments:  None

Side Effects: None

==============================================================================*/
BREWSTATICEXTENSION_API const AEEStaticClass* GetStaticClassEntries(void)
{
   return gGIFViewerExtClasses;
}
#endif // _BUILD_AS_STATIC_EXTENSION_
/*===========================================================================

===========================================================================*/
void GIFViewer_Init(IShell * ps)
{
   AEEDeviceInfo devInfo;

   //Allow this handler to be registered only if the current language is korean
   ISHELL_GetDeviceInfo(ps, &devInfo);

   //Deregister any existing handle for this MIME type
   ISHELL_RegisterHandler(ps, HTYPE_VIEWER, MT_GIF, 0);
   //Register this class as the GIF handler
   ISHELL_RegisterHandler(ps, HTYPE_VIEWER, MT_GIF, AEECLSID_GIF);

}

/*===========================================================================

===========================================================================*/

int GIFViewer_New(IShell * pIShell,AEECLSID ClsId,void ** ppObj)
{
   return ((AEECLSID_GIF == ClsId) ? COEMGIFViewer_New(0, pIShell, ClsId, ppObj)
                                   : ECLASSNOTSUPPORT);
}

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION COEMGIFViewer_New

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
/* GIFViewer_CreateInstance
*/
#if defined(AEE_STATIC)
int GIFViewer_CreateInstance(IModule *piModule, IShell *piShell, AEECLSID clsid, void **ppo)
#else
int AEEClsCreateInstance(AEECLSID clsid,IShell * piShell,IModule * piModule, void ** ppo)
#endif
{
   return ((AEECLSID_GIF == clsid) ? COEMGIFViewer_New(piModule, piShell, clsid, ppo)
                                   : ECLASSNOTSUPPORT);
}

static int COEMGIFViewer_New(IModule *piModule, IShell * po, AEECLSID cls, void **ppo)
{
   int nErr;
   COEMGIFViewer * pme;

#if defined(GIF_SHARE_MEM)

   // See if we need to allocate the globals.  If so, do it.  Check for errors
   // beforehand!!!!!

   if(!gpStack || !gpByteBuff || !gpPrefix || !gpSuffix){

       IHeap *pIHeap = NULL;        //for dymanic memory checking
       if (ISHELL_CreateInstance(po, AEECLSID_HEAP, (void **)&pIHeap)
           != AEE_SUCCESS) {
          return(ENOMEMORY);
       }

       if (!IHEAP_CheckAvail(pIHeap, ((MAX_GIFCODES + 1) * 5) + BYTE_BUFF_SIZE + 50)) {
          IHEAP_Release(pIHeap);
          return(ENOMEMORY);
       }
       IHEAP_Release(pIHeap);

      if(!gpStack)
         gpStack = (byte *)GIF_MALLOC(STACK_SIZE);

      if(!gpByteBuff)
         gpByteBuff = (byte *)GIF_MALLOC(BYTE_BUFF_SIZE);

      if(!gpPrefix)
         gpPrefix = (uint16 *)GIF_MALLOC((MAX_GIFCODES + 1) * sizeof(uint16));

      if(!gpSuffix)
         gpSuffix = (byte *) GIF_MALLOC(MAX_GIFCODES + 1);

      if(!gpStack || !gpByteBuff || !gpPrefix || !gpSuffix){
         CleanupSharedMem();
         return(ENOMEMORY);
      }
   }
#endif
   pme = (COEMGIFViewer *)MALLOC(sizeof(COEMGIFViewer));

   if (!pme) {
      *ppo = 0;
      return ENOMEMORY;
   }

   nErr = ISHELL_CreateInstance(po, AEECLSID_DISPLAY,(void **)(&pme->m_pDisplay));
   if(AEE_SUCCESS == nErr){
      INIT_VTBL(pme, IImage,gOEMGIFViewerMethods);
      pme->m_nRefs         = 1;
      pme->m_cls           = cls;
      pme->m_pShell        = po;
      ISHELL_AddRef(pme->m_pShell);

      pme->m_rop           = AEE_RO_COPY;
      pme->m_nRate         = (int16)DEF_ANIMATION_RATE;
      pme->m_nCurFrame     = -1;
      pme->m_DecodeState   = AEE_GIF_NONE;
      pme->m_playtimes     = -1;

      pme->pfnRedraw = NULL;
      pme->pRedraw = NULL;
#if defined(GIF_SHARE_MEM)
      gnRefs++;
#endif

      (void)ISHELL_CreateInstance (po, AEECLSID_BilinearScale, (void **)&pme->pScale);

   } else {
      FREEIF(pme);
   }

   *ppo = pme;

   return nErr;
}

/*===========================================================================

FUNCTION COEMGIFViewer_AddRef

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
uint32 COEMGIFViewer_AddRef(IImage * po)
{
   if(po)
      return(++((COEMGIFViewer *)po)->m_nRefs);
   return(0);
}

/*===========================================================================

FUNCTION COEMGIFViewer_Release

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
uint32   COEMGIFViewer_Release(IImage * po)
{
   COEMGIFViewer *         pme = (COEMGIFViewer *)po;

   if(pme && pme->m_nRefs > 0){
      if(--pme->m_nRefs)
         return(pme->m_nRefs);

      IDISPLAY_Release(pme->m_pDisplay);

      //ISHELL_CancelTimer((IShell *)pme->m_pShell, (PFNNOTIFY)(COEMGIFViewer_Animate), pme);
      COEMGIFViewer_Stop((IImage *) pme);
      COEMGIFViewer_FreeData(pme);
      RELEASEIF(pme->m_pShell);

      RELEASEIF(pme->m_pIScreenBuffer);
      RELEASEIF(pme->pScale);

#if defined(GIF_SHARE_MEM)
      if(!--gnRefs)
         CleanupSharedMem();
#endif

      FREE((void *) pme);
   }
   return(0);
}


/*===========================================================================

FUNCTION COEMGIFViewer_GetInfo

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_GetInfo(IImage * po, AEEImageInfo * pi)
{
   COEMGIFViewer * pme = (COEMGIFViewer *)po;

   if(pi)
      MEMCPY(pi, &pme->m_info, sizeof(AEEImageInfo));
}

/*==========================================================================
   COEMGIFViewer_ExtFunc
  ==========================================================================*/
static int COEMGIFViewer_ExtFunc(COEMGIFViewer *pme, AEECLSID id, void *pBuf, int nBufSize)
{
   if ((IPARM_COMPATFLAGS == id) && (sizeof(uint32) == nBufSize)) {
      pme->dwFlags = *((uint32*)pBuf);
      return SUCCESS;
   }

   return EUNSUPPORTED;
}


/*===========================================================================

FUNCTION COEMGIFViewer_SetParm

DESCRIPTION
   The only configurable parameters in the GIF viewer are the size of the
   image and the screen offset.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_SetParm(IImage * po, int nParm, int n1, int n2)
{
    COEMGIFViewer * pme = (COEMGIFViewer *)po;

    switch(nParm){
    case IPARM_PLAYTIMES: // Gemsea Add
      pme->m_playtimes = n1;
      break;
    case IPARM_ROP:
      if(AEE_RO_MASK != (AEERasterOp)n1)
      pme->m_rop = (AEERasterOp)n1;
      break;

    case IPARM_REDRAW:
      pme->pfnRedraw = (PFNNOTIFY)n1;
      pme->pRedraw = (void *)n2;
      break;

    case IPARM_ANIMATIONEND:
      pme->pfnAnimationEnd = (PFNNOTIFY)n1;
      pme->pAnimationEnd = (void *)n2;
      break;

      case IPARM_SIZE:
   // CR33482 - Validate input parameters for size.  Do not allow it to exceed the image size!

         if(n1 < 0)
            n1 = 0;
         else{
            if(n1 > pme->m_info.cxFrame)
               n1 = pme->m_info.cxFrame;
         }
         if(n2 < 0)
            n2 = 0;
         else{
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
            IDISPLAY_Release(pme->m_pDisplay);
            pme->m_pDisplay = pIDisp;
            IDISPLAY_AddRef(pme->m_pDisplay);
         }
         break;
      }

      case IPARM_SCALE:
        if(n1 > 0 && n2>0)
        {
        pme->m_info.cx = n1;
        pme->m_info.cy = n2;
        pme->m_rc.dx = n1;
        pme->m_rc.dy = n2;
        }
         break;
      case IPARM_EXTFUNC:
        *(int*)n2 = COEMGIFViewer_ExtFunc(pme, ((AEEImageExtFunc*)n1)->id,
                                               ((AEEImageExtFunc*)n1)->pBuf,
                                               ((AEEImageExtFunc*)n1)->nBufSize);
        break;
   }
}

/*===========================================================================

FUNCTION COEMGIFViewer_Draw

DESCRIPTION
   This function draws the first frame only.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_Draw(IImage * po, int x, int y)
{
   COEMGIFViewer *   pme = (COEMGIFViewer *)po;
   //We do this so that draw relocates an ongoing animation.
   if(pme->m_bAnimating) {
      pme->m_rc.x = x;
      pme->m_rc.y = y;
   }
   else
   {
      //<SR 823709 added
      pme->m_rc.x = x;
      pme->m_rc.y = y;
      //823709 added*>
      COEMGIFViewer_DrawFrame(po, 0, x, y);
   }
}

/*===========================================================================

FUNCTION COEMGIFViewer_DrawFrame

DESCRIPTION
   Draws the requested frame number. The image to be drawn may require
   one or more preceding frames to be drawn. This function takes care
   of that based on the disposal mechanism of the previous frame.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_DrawFrame(IImage * po,int nFrame, int x, int y)
{
   COEMGIFViewer *   pme = (COEMGIFViewer *)po;
   AEE_GIF_Img *     pImgNode;
   AEE_GIF_Img *     pPrevImgNode;
   byte              byDisposal;
   boolean           bAnimationEnd = FALSE;
   int               BadCodeCount;
   uint32            rop = pme->m_rop;
   int               prevImageNo;
   AEE_GIF_ImgHdr *  pGifImgHdr;
   boolean           bImageParametersChanged = FALSE;

   if (!pme->m_pImgList)
      return;


   //For partial decodes, SetData has already decoded the fisrt frame
   if(pme->m_DecodeState != AEE_GIF_COMPLETE) {
      //If we haven't received all the frames yet, we can't show anything
      return;
   }

   if ( nFrame == -1 )
     nFrame = pme->m_nCurFrame;

   if(nFrame < 0 || nFrame >= pme->m_nFrames)
      nFrame = 0;

   if(!nFrame)
      prevImageNo = pme->m_nFrames -1;
   else
      prevImageNo = nFrame - 1;

   if ( ( pme->m_nLastFrameDrawn != nFrame ) ||
        ( pme->m_nLastCx != pme->m_info.cx ) ||
        ( pme->m_nLastCy != pme->m_info.cy ) ||
        ( nFrame == 0 ) ||   //CR 103691/SR 866833
        ( pme->bStreamChanged )
       )
   {
      bImageParametersChanged = TRUE;
      pme->bStreamChanged = FALSE;
      pme->m_nLastFrameDrawn = nFrame;
      pme->m_nLastCx = pme->m_info.cx;
      pme->m_nLastCy = pme->m_info.cy;
   }

   pPrevImgNode = GIF_GetImageNode(pme, (word) prevImageNo);
   if (!pPrevImgNode)
      return;

   byDisposal = pPrevImgNode->GIFExt.Disposal;

   pImgNode = GIF_GetImageNode(pme, (word) nFrame);
   if (!pImgNode || pImgNode->iImageNo != nFrame)
      return;

   pGifImgHdr = (AEE_GIF_ImgHdr *) pPrevImgNode->pImg;

   if (pImgNode->GIFExt.TranspFlag) {
      if (!pme->m_bAnimating ||
          (pme->m_bAnimating && (0 == (pme->dwFlags & GIFCF_USEBGCOLOR)))) {
         rop = AEE_RO_TRANSPARENT; // SR1003430/CR135015, set transparent operation here
      }
      else {
         rop = AEE_RO_COPY;
      }
   }
   else {
      rop = AEE_RO_COPY; // SR1003430/CR135015: restore to COPY operation if no transparent color
   }

   if (0 == nFrame) {
      if (!pme->bHaveDrawn && pme->m_bHasLclClrTabFrames &&
            (0 == (pme->dwFlags & GIFCF_USEBGCOLOR))) {
         IBitmap* pibSrc = IDISPLAY_GetDestination(pme->m_pDisplay);
         if(!pme->m_pIScreenBuffer) {
            if (pibSrc) {
               IBITMAP_CreateCompatibleBitmap(pibSrc,
                     &pme->m_pIScreenBuffer,
                     pme->m_rc.dx,
                     pme->m_rc.dy);
               if(pImgNode->GIFExt.TranspFlag && pme->m_pIScreenBuffer) {
                  // copy the BG bits over
                  IBITMAP_BltIn(pme->m_pIScreenBuffer,
                        0, 0, pme->m_rc.dx, pme->m_rc.dy, pibSrc, x, y,
                        AEE_RO_COPY);
               }
            }
         }
         RELEASEIF(pibSrc);
      }
   }

   //Set the delay value here for animation.
   if (pImgNode) {
      pme->m_nRate = pImgNode->GIFExt.wDelay * 10;
      if (pme->m_nRate < MIN_ANIMATION_RATE)
         pme->m_nRate = MIN_ANIMATION_RATE;
   }
   else
   {

      pme->m_nRate = MIN_ANIMATION_RATE;
   }

     #if defined (FEATURE_SIRIUS_GIF_IMAGE_CPU_OPTIMIZATION)
#error code not present
     #endif

   switch (byDisposal) {
      case 0: //go through CR 103691 / SR866833
      case GIFLEAVEASIS:
         // add for SR 784717 / CR 84733
         if (!nFrame)
         {
            if (pme->m_bAnimating){ // SR 893211/CR 109481: only erase for animation GIF
               AEERect rect = pme->m_rc;
               rect.x = x;
               rect.y = y;
               IDISPLAY_EraseRect(pme->m_pDisplay, &rect);
            }
            if(pImgNode->GIFExt.TranspFlag)
            MEMSET((void *) pme->m_pIDIB->pBmp, pImgNode->GIFExt.TranspColor, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
            else
            MEMSET((void *) pme->m_pIDIB->pBmp, pme->m_GIFBackgrnd, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
         }
         else if (pme->m_bHasLclClrTabFrames)
         {
            if(pImgNode->GIFExt.TranspFlag)
               MEMSET((void *) pme->m_pIDIB->pBmp, pImgNode->GIFExt.TranspColor, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
            else
               MEMSET((void *) pme->m_pIDIB->pBmp, pme->m_GIFBackgrnd, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
         }

         break;

      case GIFRESTOREBCK:
         //Restore the background color
         //IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_BACKGROUND, pme->m_clrBack);
         {
#if 0
            /* Fix for CR 70222(SR 752195): we should only erase m_rc - the display area*/
            AEERect rect;
            rect.x = x;
            rect.y = y;
            rect.dx = pme->m_info.cx;
            rect.dy = pme->m_info.cy;
            IDISPLAY_EraseRect(pme->m_pDisplay, &rect);
#endif
               AEERect rect = pme->m_rc;
            /* fix for above SR#770961/CR#80056 and SR#762281 and 831949:
               we should just erase the m_rc,
               no need to calculate the scaled rectangle
            */

            if (!pme->pfnRedraw && pme->m_bAnimating)
            {
               /*CR 107064(SR 873985): Widget will register a redraw function, so
               we don't need to make erase the rectangle,
               for non-widget based application we still want to erase the rect to
               avoid the overlaped frames.
               */
               IDISPLAY_EraseRect(pme->m_pDisplay, &rect);
            }
#if 0
            if ( bImageParametersChanged )
            {
            if(pImgNode->GIFExt.TranspFlag)
                MEMSET((void *) pme->m_pIDIB->pBmp, pImgNode->GIFExt.TranspColor, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
            else
                MEMSET((void *) pme->m_pIDIB->pBmp, pme->m_GIFBackgrnd, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
            }
#else
            // fix for SR 770961: err2.gif
            if ( bImageParametersChanged )
            {
               byte backcolor = pme->m_GIFBackgrnd;

               if(pImgNode->GIFExt.TranspFlag &&
                     0 == (pme->dwFlags & GIFCF_USEBGCOLOR))
               {
                     backcolor = pImgNode->GIFExt.TranspColor;
               }

               if (nFrame && pGifImgHdr &&
                  pImgNode->GIFExt.TranspColor == pPrevImgNode->GIFExt.TranspColor) // CR107064(SR873985)
               {
                     int height = pGifImgHdr->wImgHeight;
                     byte *pBmp = pme->m_pIDIB->pBmp + pGifImgHdr ->wTOffset * pme->m_pIDIB->cx + pGifImgHdr ->wLOffset;

                     while(height--)
                     {
                         MEMSET((void *)pBmp, backcolor, pGifImgHdr->wImgWidth);
                         pBmp += pme->m_pIDIB->cx;
                     }
               }
               else
               {
                   MEMSET((void *) pme->m_pIDIB->pBmp, backcolor, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
               }
            }
#endif
         }
         break;

      case GIFRESTOREORIG:
        if ( bImageParametersChanged )
        {
         if(pImgNode->GIFExt.TranspFlag)
            MEMSET((void *) pme->m_pIDIB->pBmp, pImgNode->GIFExt.TranspColor, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
         else
            MEMSET((void *) pme->m_pIDIB->pBmp, pme->m_GIFBackgrnd, pme->m_rcGIFFrame.dx * pme->m_rcGIFFrame.dy);
        }
        /* add for CR 91304/SR 810397 */
        /* for the first frame, the display should be cleaned, and no previous frame for decoding */
        if (!nFrame)
        {
            IDISPLAY_EraseRect(pme->m_pDisplay, &(pme->m_rc));
        }
        else
        {
           //Restores to the state of a previous, undisposed frame, so we need
           //to search
           uint16 restoreImageNo = (uint16) prevImageNo;

           if (restoreImageNo) restoreImageNo -= 1;

           while (restoreImageNo)
           {
              AEE_GIF_Img* pRestoreImgNode =
                 GIF_GetImageNode(pme, (word) restoreImageNo);

              if (GIFRESTOREORIG != pRestoreImgNode->GIFExt.Disposal)
                 break;

              restoreImageNo -= 1;
           }
           // SR 932673 - if we reach to the first frame, just erase
           if (!restoreImageNo)
           {
             IDISPLAY_EraseRect(pme->m_pDisplay, &(pme->m_rc));
           }
           else if ((GIF_Decoder(pme, (uint16) prevImageNo, &BadCodeCount) != 0))
            return;
        }
         break;

      default:
         break;
   }

   if ( bImageParametersChanged  )
   {
      if ((GIF_Decoder(pme, (uint16) nFrame, &BadCodeCount) != 0))
         return;
      COEMGIFViewer_ScaleImage(pme, pme->m_info.cx, pme->m_info.cy, x, y, rop);
   }
   if ( pme->m_bAnimating && pme->m_nFrames > 1)
   {
     pme->m_nCurFrame++;

     if (pme->m_nCurFrame >= pme->m_nFrames)
     {
       pme->m_nCurFrame = 0;
       bAnimationEnd = TRUE;
     }
   }

   if (pImgNode->GIFExt.TranspFlag) {
      pme->m_pIDIB->ncTransparent = pImgNode->GIFExt.TranspColor;
   }

   if(pme->m_rcGIFFrame.dx && pme->m_pIOutDIB) {
      if(pme->m_bHasLclClrTabFrames && pme->m_pIScreenBuffer) {
         IBITMAP_BltIn(pme->m_pIScreenBuffer,
               0, 0, pme->m_rc.dx, pme->m_rc.dy,
               IDIB_TO_IBITMAP(pme->m_pIOutDIB),
               pme->m_xOffset, pme->m_yOffset, rop);
         IDISPLAY_BitBlt(pme->m_pDisplay, x, y, pme->m_rc.dx, pme->m_rc.dy,
                         (const void *) pme->m_pIScreenBuffer,
                         0, 0, AEE_RO_COPY);
      }
      else {
         IDISPLAY_BitBlt(
              pme->m_pDisplay, x, y, pme->m_rc.dx, pme->m_rc.dy ,
              (const void *) IDIB_TO_IBITMAP(pme->m_pIOutDIB),
              pme->m_xOffset, pme->m_yOffset,
              rop
              );
      }
   }

   if(bAnimationEnd == TRUE)
   {
        if(pme->m_playtimes>0)
        {
            pme->m_playtimes--;
        }
        if(pme->pfnAnimationEnd)
        {
            PFNNOTIFY pfn = pme->pfnAnimationEnd;
            pme->pfnAnimationEnd = NULL;
            IIMAGE_AddRef((IImage*)pme);
            pfn(pme->pAnimationEnd);
            if (0 == IIMAGE_Release((IImage*)pme))
            {
                return;
            }
            pme->pfnAnimationEnd = pfn;
        }
   }
   pme->bHaveDrawn = 1;
}


/*===========================================================================

FUNCTION COEMGIFViewer_Start

DESCRIPTION
   Starts animation of GIF frames.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_Start(IImage * po, int x, int y)
{
   COEMGIFViewer * pme = (COEMGIFViewer *)po;

   //We don't animate till we have all the frames
   if (pme->m_DecodeState != AEE_GIF_COMPLETE)
      return;

   pme->m_rc.x = x;
   pme->m_rc.y = y;
   pme->m_nCurFrame = 0;

   //Get the system background color
   pme->m_clrBack   = NTOHL(IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_BACKGROUND, RGB_NONE));
  //Restore the system background color
   IDISPLAY_SetColor(pme->m_pDisplay, CLR_USER_BACKGROUND, RGB_NONE);
   // Draw it at least one time....

   if(pme->m_nFrames > 1) {
     pme->m_bAnimating = TRUE;
     if (pme->m_bHasLclClrTabFrames && (0 == (pme->dwFlags & GIFCF_USEBGCOLOR))) {
         IBitmap* pibSrc = IDISPLAY_GetDestination(pme->m_pDisplay);
         if(!pme->m_pIScreenBuffer) {
            if (pibSrc) {
               IBITMAP_CreateCompatibleBitmap(pibSrc,
                     &pme->m_pIScreenBuffer,
                     pme->m_rc.dx,
                     pme->m_rc.dy);
               if(pme->m_pIScreenBuffer) {
                  // copy the BG bits over
                  IBITMAP_BltIn(pme->m_pIScreenBuffer,
                        0, 0, pme->m_rc.dx, pme->m_rc.dy, pibSrc, x, y,
                        AEE_RO_COPY);
               }
            }
         }
         RELEASEIF(pibSrc);
      }
   }

   COEMGIFViewer_Animate(pme);
}

/*===========================================================================

FUNCTION COEMGIFViewer_Stop

DESCRIPTION
   Stops animation

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_Stop(IImage * po)
{
   COEMGIFViewer * pme = (COEMGIFViewer *)po;

   pme->m_bAnimating = FALSE;

   ISHELL_CancelTimer((IShell *)pme->m_pShell, (PFNNOTIFY)(COEMGIFViewer_Animate), pme);
}

/*===========================================================================

FUNCTION COEMGIFViewer_Animate

DESCRIPTION
   Draws next frame, updates display, and then sets a timer to repeat

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_Animate(COEMGIFViewer * pme)
{

  if ( pme->pfnRedraw )
  {
    PFNNOTIFY pfn = pme->pfnRedraw;
    pme->pfnRedraw = NULL;
    IIMAGE_AddRef((IImage*)pme);
    //BRIDLE_ASSERT_SAFE_PFN(pfn);
    pfn(pme->pRedraw);
    if (0 == IIMAGE_Release((IImage*)pme))
    {
      return;
    }
    pme->pfnRedraw = pfn;
  }
  else
  {
    COEMGIFViewer_DrawFrame((IImage *)pme, pme->m_nCurFrame, pme->m_rc.x, pme->m_rc.y);
    IDISPLAY_Update(pme->m_pDisplay);
  }

  if(pme->m_nFrames > 1 && pme->m_playtimes != 0)
  {
     ISHELL_SetTimer(pme->m_pShell, (uint32)pme->m_nRate, (PFNNOTIFY)(COEMGIFViewer_Animate),pme);
  }

}

/*===========================================================================

FUNCTION COEMGIFViewer_SetStream

DESCRIPTION
   Sets up the stream to read the GIF image from

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_SetStream(IImage * po, IAStream * ps)
{
   COEMGIFViewer * pme = (COEMGIFViewer *)po;
   int32       nSize;

   pme->bStreamChanged = FALSE;

   if (!ps)
      return;

   COEMGIFViewer_FreeData(pme);

   //Reset the data associated with a previous image, if any
   pme->m_nCurFrame     = -1;
   pme->m_DecodeState   = AEE_GIF_NONE;

   MEMSET((void *) &pme->m_rc, 0, sizeof(AEERect));
   pme->m_pMaxGIF         = NULL;
   pme->m_nRead           = 0;
   pme->m_nFrames         = 0;
   pme->m_bIsClrTabGlobal = FALSE;
   pme->m_bImageRead      = FALSE;
   pme->m_dwGIFBufSize    = 0;

   pme->m_pStream = ps;
   IASTREAM_AddRef(ps);

   // See if we can pre-determine the size.  If so, this will prevent a reallocation...

   nSize = IASTREAM_Read(ps, NULL, 0);
   if(nSize > 0){
      nSize++;
      pme->m_pGIF = MALLOC(nSize);

   // If the malloc fails, defer the processing.  It may be able to complete
   // successfully later...

      if(!pme->m_pGIF){
         IASTREAM_Readable(ps,(PFNNOTIFY)(COEMGIFViewer_ReadableCB),pme);
         return;
      }
      pme->m_dwGIFBufSize = nSize;
   }

   COEMGIFViewer_ReadableCB(pme);
}

/*===========================================================================

FUNCTION COEMGIFViewer_HandleEvent

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean COEMGIFViewer_HandleEvent(IImage * po, AEEEvent e, uint16 w, uint32 d)
{
   return(FALSE);
}

/*===========================================================================

FUNCTION COEMGIFViewer_Notify

DESCRIPTION
   This method provides the caller a mechanism by which they can be notified when
   the complete GIF buffer has been received and parsed. This is useful for
   the caller to determine when to start animation.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_Notify(IImage * po, PFNIMAGEINFO pfn, void * pUser)
{
   COEMGIFViewer * pme = (COEMGIFViewer *)po;

   pme->m_pfnNotify = pfn;
   pme->m_pUser     = pUser;
   /* CR 45597: m_bImageRead was set to TRUE at the begining of SetData (),
      that is too early, and following SUCCESS notify will send application
      wrong notification and cause the drawing to start pre-maturely.
      So add another checking for AEE_GIF_COMPLETE which is set at the
      send of SetData().
   */
   if(pme->m_bImageRead && pme->m_DecodeState == AEE_GIF_COMPLETE){
      if(pfn)
      GIF_DispatchNotify(pme, AEE_SUCCESS);
   }
}

/*===========================================================================

FUNCTION CleanupSharedMem

DESCRIPTION
   This function frees all the shared memory associated with the GIF viewer.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
#if defined(GIF_SHARE_MEM)
static void CleanupSharedMem(void)
{
   if(gpStack){
      GIF_FREE(gpStack);
      gpStack = NULL;
   }

   if(gpByteBuff){
      GIF_FREE(gpByteBuff);
      gpByteBuff = NULL;
   }

   if(gpPrefix){
      GIF_FREE(gpPrefix);
      gpPrefix = NULL;
   }

   if(gpSuffix){
      GIF_FREE(gpSuffix);
      gpSuffix = NULL;
   }
}
#endif

/*===========================================================================

FUNCTION COEMGIFViewer_FreeData

DESCRIPTION
   Frees all the memory allocated for this viewer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_FreeData(COEMGIFViewer * pme)
{
   CALLBACK_Cancel(&pme->m_NotifyCBStruct);

   MEMSET((void *) &pme->m_info,0,sizeof(AEEImageInfo));

   GIF_FreeDict(pme);

   pme->m_nRead = 0;

   if(pme->m_pStream){
      IASTREAM_Release(pme->m_pStream);
      pme->m_pStream = NULL;
   }


   if(pme->m_pIDIB) {
      IDIB_Release(pme->m_pIDIB);
      pme->m_pIDIB = NULL;
   }

   if(pme->m_pIOutDIB) {
      IDIB_Release(pme->m_pIOutDIB);
      pme->m_pIOutDIB = NULL;
   }

   if(pme->m_pGIF){
      FREE(pme->m_pGIF);
      pme->m_pGIF = NULL;
   }

   GIF_FreeImageList(pme);
}

/*===========================================================================

FUNCTION COEMGIFViewer_Realloc

DESCRIPTION
   This function reallocates the input GIF buffer when it is not known upfront
   how big the image is (such as when it is coming on the network)

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean COEMGIFViewer_Realloc(COEMGIFViewer * pme)
{
   byte *pOldGIF = pme->m_pGIF;

   pme->m_dwGIFBufSize += COEMIMAGE_MIN_READ;
   pme->m_pGIF = REALLOC(pme->m_pGIF, pme->m_dwGIFBufSize);

   // CR 98612: if realloc failed, we have to free the old memory
   if (!pme->m_pGIF)
      FREE(pOldGIF);

   return(pme->m_pGIF != NULL);
}

/*===========================================================================

FUNCTION COEMGIFViewer_ReadableCB

DESCRIPTION
   This callback function is used to asynchronously read the stream for the
   GIF image. If the function blocks for data on the stream, it goes and parses
   the GIF buffer for the first image, so that atleast that can be drawn.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void COEMGIFViewer_ReadableCB(COEMGIFViewer * pme)
{
   int32    nRead,nAvail;
   byte *   pRead;

   while(1){
      nAvail = pme->m_dwGIFBufSize - pme->m_nRead;
      if(!nAvail){
         if(!COEMGIFViewer_Realloc(pme))
         {
            // CR 98612, if _Realloc failed then bail out
            COEMGIFViewer_FreeData(pme);
            GIF_DispatchNotify(pme, ENOMEMORY);
            return;
         }
         //   break;
         nAvail = COEMIMAGE_MIN_READ;
      }
      pRead = (byte *)pme->m_pGIF + pme->m_nRead;
      nRead = IASTREAM_Read(pme->m_pStream,pRead,nAvail);

   // No data coming? Just get out and let the callback deal with it.

      if(nRead == AEE_STREAM_WOULDBLOCK)
         break;

   // Finished?

      if(!nRead){
         //pRead = (byte *)pme->m_pGIF;
         //pme->m_pGIF = NULL;
         GIF_SetData((IImage *)pme);
         return;
      }
      pme->m_nRead += nRead;
   }
   IASTREAM_Readable(pme->m_pStream, (PFNNOTIFY)COEMGIFViewer_ReadableCB, pme);
}

//======================================================================================
//   Routines to Parse GIF image buffer
//======================================================================================

/*===========================================================================

FUNCTION GIF_SetData

DESCRIPTION
   This function is the heart of the GIF parsing, and it reads the GIF headers,
   the extension blocks (and extracts the transparency and disposal information),
   and builds a linked list of GIF image starting points.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean GIF_SetData(IImage * po)
{
   COEMGIFViewer *      pme = (COEMGIFViewer *)po;
   byte *               pGIFData;
   AEE_GIF_LSD     *    pLScrnDesc;
   //AEE_GIF_GTEntry *    pGIFClrTab = NULL;
   byte  *              pbIdent;
   //AEE_GIF_ImgHdr  *    pImgHdr = NULL;
   word                 wImgWidth;
   AEE_GIF_GraphExt     GraphExt;
   boolean              bFirstImg = FALSE;
   AEEDeviceInfo        devInfo;

   pme->bHaveDrawn = 0;
   pGIFData = pme->m_pGIF;
   pme->m_bImageRead = TRUE;
   pme->m_bHasLclClrTabFrames = FALSE;

   if (!pGIFData || pme->m_nRead < (sizeof(AEE_GIF_Hdr) + sizeof(AEE_GIF_LSD))) {
      GIF_DispatchNotify(pme, EFAILED);
      return FALSE;
   }

   //Verify that it is indeed a GIF buffer. If not, return FALSE
   if(STRNCMP("GIF", (const char *) pGIFData, 3)) {
      GIF_DispatchNotify(pme, EFAILED);
      return FALSE;
   }


   //We shouldn't go beyond this point in the input GIF memory
   pme->m_pMaxGIF = pGIFData + pme->m_nRead;
   //We already know its a GIF image, so we'll just skip the file header
   pGIFData += sizeof(AEE_GIF_Hdr);

   //Set the Logical Screen Descriptor ptr
   pLScrnDesc = (AEE_GIF_LSD *) pGIFData;


   //Move the GIFData ptr to the start of color tables
   pGIFData += sizeof(AEE_GIF_LSD);

   //If there is a global color table, we will use that for BMP.
   if (pLScrnDesc->ColorTabFlag) {
      pme->m_pGIFClrTab = (AEE_GIF_GTEntry *) pGIFData;

      pme->m_GIFBackgrnd = pLScrnDesc->bBGColor;
      //Skip over the color table for now. We'll convert and write after we
      //have allocated a buffer.
      pme->m_wGIFNumClrs = 1 << (pLScrnDesc->ColorTabSize +1);
      pGIFData += pme->m_wGIFNumClrs * sizeof(AEE_GIF_GTEntry);
   }
   else {
      pme->m_pGIFClrTab  = NULL;
      pme->m_wGIFNumClrs = 256;
   }

   pbIdent = (byte *) pGIFData;

   //So now we count the number of images.
   MEMSET((void *) &GraphExt, 0, sizeof(AEE_GIF_GraphExt));

   while (pbIdent <  pme->m_pMaxGIF) {
      if ((*(pbIdent - 1)) == 0 || !bFirstImg) {

         //Skip over the extension blocks for each image, but get the transparent color, if any
         if (*pbIdent == GIFEXTID) {
            pbIdent = GIF_SkipOverExtBlocks(pbIdent, &GraphExt);
         }
         if (*pbIdent == GIFIMGBLKID) {
            //We have to make sure that this is truly the beginning of an image block
            //and not just image data. How we verify is really a hack.
            if(GIF_VerifyImageStart(pbIdent, pLScrnDesc)) {
               if (!GIF_AddImageToList(pme, pbIdent + 1, pme->m_nFrames, GraphExt))
                  break;
               else
                  MEMSET((void *) &GraphExt, 0, sizeof(AEE_GIF_GraphExt));
               pme->m_nFrames++;
            }
         }


      }
      bFirstImg = TRUE;
      pbIdent++;

   }

   if (pme->m_nFrames < 1) {
      COEMGIFViewer_FreeData(pme);
      GIF_DispatchNotify(pme, EFAILED);
      return FALSE;
   }


   //Allocate the right amount of memory for the BMP.
   wImgWidth = pLScrnDesc->wLScreenW;

   //We have to round off to next multiple of 4 for the BMP to be happy
   //if (wImgWidth % 4)
   //   wImgWidth = (wImgWidth/4 + 1) * 4;

   //Populate the image info.
   pme->m_info.cy = pLScrnDesc->wLScreenH;
   pme->m_info.cxFrame = pme->m_info.cx = wImgWidth;
   pme->m_info.nColors = 256;
   if (pme->m_nFrames > 1) {
      pme->m_info.bAnimated = TRUE;
      pme->m_nRate = pme->m_pImgList->GIFExt.wDelay * 10;
      if (pme->m_nRate < MIN_ANIMATION_RATE)
         pme->m_nRate = MIN_ANIMATION_RATE;
   }
   else
      pme->m_info.bAnimated = FALSE;

   pme->m_rc.dx = pme->m_info.cx;
   pme->m_rc.dy = pme->m_info.cy;

   pme->m_rcGIFFrame.dx = pme->m_info.cx;
   pme->m_rcGIFFrame.dy = pme->m_info.cy;

   //First time, we construct the IBitmap interface here.
   IDISPLAY_CreateDIBitmap( pme->m_pDisplay,
                            (IDIB **) &pme->m_pIDIB,
                            8,
                            (uint16) pme->m_rcGIFFrame.dx,
                            (uint16) pme->m_rcGIFFrame.dy
                          );

   if(!pme->m_pIDIB) {
      COEMGIFViewer_FreeData(pme);
      GIF_DispatchNotify(pme, EFAILED);
      return FALSE;
   }
   else {
      //We can set the pitch correctly here
      pme->m_pIDIB->nPitch = pme->m_rcGIFFrame.dx;
      pme->m_pIDIB->pRGB = pme->m_clrMap;
   }


   ISHELL_GetDeviceInfo(pme->m_pShell, &devInfo);

   //Allocate the GIF decode dictionary space here
   if (!GIF_AllocateDict(pme)) {
      COEMGIFViewer_FreeData(pme);
      GIF_DispatchNotify(pme, EFAILED);
      return FALSE;
   }

   //run through the frames and check for local color tables
   if (pme->m_nFrames) {
      AEE_GIF_Img *p;
      for (p=pme->m_pImgList; p; p = p->pNext) {
         if (((AEE_GIF_ImgHdr*)p->pImg)->LclClrTabFlag) {
            pme->m_bHasLclClrTabFrames = TRUE;
            break;
         }
      }
   }

   pme->m_DecodeState = AEE_GIF_COMPLETE;


   GIF_DispatchNotify(pme, AEE_SUCCESS);
   return(TRUE);
}

/*===========================================================================

FUNCTION: GIF_DispatchNotify

DESCRIPTION


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void GIF_DispatchNotify(COEMGIFViewer * pme, uint32 errCode) {

   pme->m_DecodeError = errCode;

      CALLBACK_Cancel(&pme->m_NotifyCBStruct);
      CALLBACK_Init(&pme->m_NotifyCBStruct, (PFNNOTIFY) GIF_NotifyUser, (void *) pme);
      ISHELL_Resume(pme->m_pShell, &pme->m_NotifyCBStruct);
}

/*===========================================================================

FUNCTION: GIF_NotifyUser

DESCRIPTION
   Notifies the caller if the decode process is complete, or has failed

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void GIF_NotifyUser(void * po) {

   COEMGIFViewer * pme = (COEMGIFViewer *) po;

   if(pme->m_pfnNotify) {
      pme->m_pfnNotify(pme->m_pUser, (IImage *) pme, &pme->m_info,pme->m_DecodeError);
   }
}

/*===========================================================================

FUNCTION: GIF_SkipOverExtBlocks

DESCRIPTION
   Goes over extension blocks, and extracts transparency and disposal information

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static byte * GIF_SkipOverExtBlocks(byte * pGIFData, AEE_GIF_GraphExt *  pGraphExt) {

   byte bLen;


   //Skip the block identifier. We've already checked that
   pGIFData++;

   //Read the type first
   switch(*pGIFData) {
      case GIFGRCTLEXT:
         if (*(pGIFData + 1) != 4)
            return pGIFData;

         if (pGraphExt)
               MEMCPY((void *) pGraphExt, (void *) (pGIFData + 2), sizeof(AEE_GIF_GraphExt));
         break;

      case GIFTEXTEXT:
         if(*(pGIFData + 1) != 12)
            return pGIFData;
         break;

      case GIFAPPEXT:
         if(*(pGIFData + 1) != 11)
            return pGIFData;
         break;

      case GIFCMTEXT:
         while(*pGIFData != 0)
            pGIFData++;
         return (pGIFData);

      default:
         return pGIFData;
   }

   bLen = *(++pGIFData);

   while(bLen) {
      pGIFData += bLen + 1;

      bLen = *pGIFData;
   }
   return pGIFData;
}

/*===========================================================================

FUNCTION: GIF_VerifyImageStart

DESCRIPTION
   This function just ensures that we indeed have a new image by doing
   some sanity checks on the GIF header.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean GIF_VerifyImageStart(byte *pImgBlock, AEE_GIF_LSD  * pLScrnDesc) {

   AEE_GIF_ImgHdr *pImgHdr = (AEE_GIF_ImgHdr *) ++pImgBlock;

   if (pImgHdr->wImgHeight <= pLScrnDesc->wLScreenH) {
      if(pImgHdr->wImgWidth <= pLScrnDesc->wLScreenW) {
         if(pImgHdr->wLOffset < pLScrnDesc->wLScreenW) {
            if(pImgHdr->wTOffset < pLScrnDesc->wLScreenH) {
               return TRUE;
            }
         }
      }
   }

   return FALSE;
}

/*===========================================================================

FUNCTION: GIF_AddImageToList

DESCRIPTION
   This function adds the new image location to a linked list. It also
   checks if the linked list is non-empty because of the partial decode
   that may have been done before the image was completely read from the
   stream

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean GIF_AddImageToList(COEMGIFViewer * pme, byte * pbIdent, word wFrameNo, AEE_GIF_GraphExt GraphExt)
{

   AEE_GIF_Img * pImgNode = NULL;
   AEE_GIF_Img * currNode = pme->m_pImgList;
   boolean       bExist   = FALSE;

   if(wFrameNo == 0) {
      if (pme->m_pImgList) {
         pImgNode = pme->m_pImgList;
         bExist   = TRUE;
      }
   }

   if(!pImgNode) {
      pImgNode = (AEE_GIF_Img *) MALLOC(sizeof(AEE_GIF_Img));
      if (!pImgNode)
         return FALSE;
   }

   pImgNode->iImageNo  = wFrameNo;
   pImgNode->GIFExt    = GraphExt;
   pImgNode->pImg      = pbIdent;
   pImgNode->pNext     = NULL;

   if(!bExist) {
      if(!currNode)
         pme->m_pImgList = pImgNode;
      else {
         while(currNode->pNext)
            currNode = currNode->pNext;
         currNode->pNext = pImgNode;
      }
   }
   return TRUE;
}

/*===========================================================================

FUNCTION: GIF_FreeImageList

DESCRIPTION
   This function frees the memory associated with the linked list created
   for the individual GIF images.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void GIF_FreeImageList(COEMGIFViewer * pme)
{
   AEE_GIF_Img * pNode, * pNext;

   pNode = pme->m_pImgList;
   while(pNode){
      pNext = pNode->pNext;
      FREE((void *) pNode);
      pNode = pNext;
   }
   pme->m_pImgList = NULL;
}

/*===========================================================================

FUNCTION: GIF_GetImageNode

DESCRIPTION
   This function retrieves the GIF image info node assciated with the
   given frame ID.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static AEE_GIF_Img * GIF_GetImageNode(COEMGIFViewer * pme, word wFrameNo) {

   AEE_GIF_Img * currNode = pme->m_pImgList;

   while(currNode) {
      if (currNode->iImageNo == wFrameNo)
         return(currNode);

      currNode = currNode->pNext;
   }
   return NULL;
}

//======================================================================================
//   Routines to Construct BMP buffer
//======================================================================================
/*===========================================================================

FUNCTION: GIF_SetColorMap

DESCRIPTION
   This function sets the color table of the BMP based on the local or the
   global color table of the GIF image dependong on the image header.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void GIF_SetColorMap(COEMGIFViewer * pme, AEE_GIF_GTEntry *  pGIFClrTab, uint16 numColors)
{
   uint16 count;
   uint32  red, green, blue;
   MEMSET((void *) pme->m_clrMap, 0, sizeof(pme->m_clrMap));

   for (count = 0; count < numColors; count++) {
      red   = pGIFClrTab->bRedComp;
      green = pGIFClrTab->bGreenComp;
      blue  = pGIFClrTab->bBlueComp;

      pme->m_clrMap[count]  = NTOHL(MAKE_RGB(red, green, blue));
      pGIFClrTab++;
   }

   pme->m_pIDIB->cntRGB = numColors;

   IDIB_FlushPalette(pme->m_pIDIB);

   return;
}

//======================================================================================
//   Routines to Decode GIF image
//======================================================================================
/*===========================================================================

FUNCTION: GIF_Decoder

DESCRIPTION
   This function decodes lines of the GIF image. This is the main function
   that does the decoding.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int16 GIF_Decoder(
                           COEMGIFViewer * pme,
                           word nFrameId,
                           int* piBadCodeCount
                       ) {

   register byte *   sp, *bufptr;
   AEE_GIF_ImgHdr *  pGifImgHdr;
   byte *            pGifData;
   register int16    code, fc, oc, bufcnt;
   int16             c, size, ret;
   int               bad_code_count = 0;
   int               slot_code_count = 0;
   byte *            byte_buff;
   AEE_GIF_Decoder   decodeStruct;
   word              nGIFColors;
   AEE_GIF_GTEntry * pGIFClrTab;
   AEE_GIF_Img *     pImgNode = GIF_GetImageNode(pme, nFrameId);
   byte *            pSuffix;    // Suffix
   uint16 *          pPrefix;    // Prefix


#if defined(GIF_SHARE_MEM)
   if(!gpStack || !gpPrefix || !gpSuffix)
      return OEMGIFERR_OTHER_ERROR;

   byte_buff = gpByteBuff;
   pPrefix = gpPrefix;
   pSuffix = gpSuffix;
   sp = gpStack;
#else
   byte_buff = pme->m_pByteBuff;
   pPrefix = pme->m_pPrefix;
   pSuffix = pme->m_pSuffix;
   sp = pme->m_pStack;
#endif

   if (!pImgNode || !byte_buff)
      return OEMGIFERR_OTHER_ERROR;

   MEMSET((void *) byte_buff, 0, BYTE_BUFF_SIZE);
   //if ((byte_buff = (byte *) MALLOC(257)) == NULL)
   //   return OEMGIFERR_OTHER_ERROR;

   pGifImgHdr = (AEE_GIF_ImgHdr *) pImgNode->pImg;
   pGifData   = pImgNode->pImg + sizeof(AEE_GIF_ImgHdr);

   decodeStruct.navail_bytes = 0;
   decodeStruct.nbits_left   = 0;
   decodeStruct.curLineNo    = pGifImgHdr->wTOffset;
   decodeStruct.curPassNo    = 0;
   decodeStruct.pLineBuf     = pme->m_pLineBuff;

   //Set the BMP color table based on the local or global table
   if (pGifImgHdr->LclClrTabFlag) {
      nGIFColors = 1 << (pGifImgHdr->LclClrTabSize + 1);
      pGIFClrTab = (AEE_GIF_GTEntry *) pGifData;
      pGifData   += nGIFColors * sizeof(AEE_GIF_GTEntry);
      GIF_SetColorMap(pme, pGIFClrTab, nGIFColors);
      if (pImgNode->GIFExt.TranspFlag) {
         pme->m_pIDIB->ncTransparent = pImgNode->GIFExt.TranspColor;
         pme->m_rop = AEE_RO_TRANSPARENT;
      }
      else
         pme->m_rop = AEE_RO_COPY;

      pme->m_bIsClrTabGlobal = FALSE;
   }
   else {
      if (!pme->m_bIsClrTabGlobal) {
         if(pme->m_pGIFClrTab) {
            nGIFColors = pme->m_wGIFNumClrs;
            pGIFClrTab = pme->m_pGIFClrTab;
            GIF_SetColorMap(pme, pGIFClrTab, nGIFColors);
            if (pImgNode->GIFExt.TranspFlag) {
               pme->m_pIDIB->ncTransparent = pImgNode->GIFExt.TranspColor;
               pme->m_rop = AEE_RO_TRANSPARENT;
            }
            else
               pme->m_rop = AEE_RO_COPY;
            pme->m_bIsClrTabGlobal = TRUE;
         }
         else {
            return OEMGIFERR_OTHER_ERROR;
         }
      }
   }

   pme->m_nCurFrame = nFrameId;

   MEMSET((void *) pSuffix, 0, MAX_GIFCODES + 1);
   MEMSET((void *) pPrefix, 0, (MAX_GIFCODES + 1) * sizeof(uint16));

   if ((size = GIF_GetByte(pme, &pGifData)) < 0) {
      //FREE((void *) byte_buff);
      return(size);
   }

   if (size < 2 || 9 < size) {
      //FREE((void *) byte_buff);
      return(OEMGIFERR_BAD_CODE_SIZE);
   }
   GIF_InitDecoder(&decodeStruct, size);

   /* Initialize in case they forgot to put in a clear code.
   * (This shouldn't happen, but we'll try and decode it anyway...)
   */
   oc = fc = 0;

   /* Reset allocated memory for the decode buffer
   */
   MEMSET((void *) decodeStruct.pLineBuf, 0, pme->m_rcGIFFrame.dx);
/*********
   if ((decodeStruct.pLineBuf = (byte *)MALLOC(pGifImgHdr->wImgWidth + 1)) == NULL) {
      FREE((void *) byte_buff);
      return(OEMGIFERR_OUT_OF_MEMORY);
   }
*********/

      /* Set up the stack pointer and decode buffer pointer
   */

   bufptr = decodeStruct.pLineBuf;
   bufcnt = pGifImgHdr->wImgWidth;

   /* This is the main loop.  For each code we get we pass through the
   * linked list of prefix codes, pushing the corresponding "character" for
   * each code onto the stack.  When the list reaches a single "character"
   * we push that on the stack too, and then start unstacking each
   * character for output in the correct order.  Special handling is
   * included for the clear code, and the whole thing ends when we get
   * an ending code.
   */
   while ((c = GIF_GetNextCode(pme, &decodeStruct, byte_buff, &pGifData)) != decodeStruct.ending)
   {

   /* If we had a file error, return without completing the decode
      */
      if (c < 0)
      {
         //Ramesh OEM_Free(buf);
         //FREE((void *) byte_buff);
         //FREE(decodeStruct.pLineBuf);
         return(0);
      }

      /* If the code is a clear code, reinitialize all necessary items.
      */
      if (c == decodeStruct.clear)
      {
         decodeStruct.curr_size = size + 1;
         decodeStruct.slot = decodeStruct.newcodes;
         decodeStruct.top_slot = 1 << decodeStruct.curr_size;

         slot_code_count += decodeStruct.slot;

         /* Continue reading codes until we get a non-clear code
         * (Another unlikely, but possible case...)
         */
         while ((c = GIF_GetNextCode(pme, &decodeStruct, byte_buff, &pGifData)) == decodeStruct.clear)
            ;

            /* If we get an ending code immediately after a clear code
            * (Yet another unlikely case), then break out of the loop.
         */
         if (c == decodeStruct.ending)
            break;

            /* Finally, if the code is beyond the range of already set codes,
            * (This one had better NOT happen...  I have no idea what will
            * result from this, but I doubt it will look good...) then set it
            * to color zero.
         */
         if (c >= decodeStruct.slot)
            c = 0;

         oc = fc = c;

         /* And let us not forget to put the char into the buffer... And
         * if, on the off chance, we were exactly one pixel from the end
         * of the line, we have to send the buffer to the GIF_LineOut()
         * routine...
         */
         *bufptr++ = ((byte) c); //pme->m_clrMap[((byte) c)];
         if (--bufcnt == 0)
         {
            if ((ret = GIF_LineOut(pme, pGifImgHdr->wImgWidth, pGifImgHdr, &decodeStruct)) < 0)
            {
               //FREE((void *) byte_buff);
               //FREE(decodeStruct.pLineBuf);
               //return(ret);
               //All lines are decoder completed, return immediately. Changed by Oscar
               return 0;
            }
            bufptr = decodeStruct.pLineBuf;
            bufcnt = pGifImgHdr->wImgWidth;
         }
      }
      else
      {

      /* In this case, it's not a clear code or an ending code, so
      * it must be a code code...  So we can now decode the code into
      * a stack of character codes. (Clear as mud, right?)
         */
         code = c;

         /* Here we go again with one of those off chances...  If, on the
         * off chance, the code we got is beyond the range of those already
         * set up (Another thing which had better NOT happen...) we trick
         * the decoder into thinking it actually got the last code read.
         * (Hmmn... I'm not sure why this works...  But it does...)
         */
         if (code >= decodeStruct.slot)
         {
            if (code > decodeStruct.slot)
               ++bad_code_count;

            /* SR 756707/CR 73915: if too many errors, abort*/
            if (bad_code_count >= slot_code_count>>1)
               break;

            code = oc;
#if !defined(GIF_SHARE_MEM)
            if (sp - pme->m_pStack < STACK_SIZE)
#else
            if (sp - gpStack < STACK_SIZE) /*CR 68313*/
#endif
            {
               *sp++ = (byte) fc;
            }
         }

         /* Here we scan back along the linked list of prefixes, pushing
         * helpless characters (ie. suffixes) onto the stack as we do so.
         */
         while (code >= decodeStruct.newcodes)
         {
            /* Some image has bad data, so we need to protect from overflow CR 68313*/
#if !defined(GIF_SHARE_MEM)
            if (sp - pme->m_pStack == STACK_SIZE)
#else
            if (sp - gpStack == STACK_SIZE)
#endif
            {
               break;
            }
            *sp++ = pSuffix[code];
            code = pPrefix[code];
         }

         /* Push the last character on the stack, and set up the new
         * prefix and suffix, and if the required slot number is greater
         * than that allowed by the current bit size, increase the bit
         * size.  (NOTE - If we are all full, we *don't* save the new
         * suffix and prefix...  I'm not certain if this is correct...
         * it might be more proper to overwrite the last code...
         */
#if !defined(GIF_SHARE_MEM)
         if (sp - pme->m_pStack < STACK_SIZE)
#else
         if (sp - gpStack < STACK_SIZE) /*CR 68313*/
#endif
         {
             *sp++ = (byte) code;
         }
         //Change for deferred clean code by Oscar
         fc = code;
         if (decodeStruct.slot < decodeStruct.top_slot)
         {
            //fc = code;
            pSuffix[decodeStruct.slot] = (byte) fc;
            pPrefix[decodeStruct.slot++] = oc;
            //oc = c;
         }
         oc = c;

         if (decodeStruct.slot >= decodeStruct.top_slot)
            if (decodeStruct.curr_size < 12)
            {
               decodeStruct.top_slot <<= 1;
               ++decodeStruct.curr_size;
            }

            /* Now that we've pushed the decoded string (in reverse order)
            * onto the stack, lets pop it off and put it into our decode
            * buffer...  And when the decode buffer is full, write another
            * line...
            */

#if !defined(GIF_SHARE_MEM)
            while (sp > pme->m_pStack)
#else
            while(sp > gpStack)
#endif
            {
               *bufptr++ = (*(--sp)); //pme->m_clrMap[(*(--sp))];
               if (--bufcnt == 0)
               {
                  if ((ret = GIF_LineOut(pme, pGifImgHdr->wImgWidth, pGifImgHdr, &decodeStruct)) < 0)
                  {
                     //FREE((void *) byte_buff);
                     //FREE(decodeStruct.pLineBuf);
                     //return(ret);
                     //All lines are decoder completed, return immediately. Changed by Oscar
                     return 0;
                  }
                  bufptr = decodeStruct.pLineBuf;
                  bufcnt = pGifImgHdr->wImgWidth;
               }
            }
      }
   }
   ret = 0;
   if (bufcnt != pGifImgHdr->wImgWidth)
   //ret = GIF_LineOut(pme, pGifImgHdr->wImgWidth - bufcnt, pGifImgHdr, &decodeStruct);
   GIF_LineOut(pme, pGifImgHdr->wImgWidth - bufcnt, pGifImgHdr, &decodeStruct);
   //FREE(decodeStruct.pLineBuf);
   if(piBadCodeCount)
      *piBadCodeCount = bad_code_count;

   //pme->m_nCurFrame = nFrameId;
   //FREE((void *) byte_buff);
   return(ret);
}

/*===========================================================================

FUNCTION: GIF_AllocateDict

DESCRIPTION
   This function allocates all the buffers needed for the dictionary
   and stacks for decoding the GIF image

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static boolean  GIF_AllocateDict(COEMGIFViewer * pme)
{
   uint32   dwSize;

#if !defined(GIF_SHARE_MEM)
   dwSize = BYTE_BUFF_SIZE + pme->m_rcGIFFrame.dx + ((MAX_GIFCODES + 1) * 5) + 50;
#else
   dwSize = pme->m_rcGIFFrame.dx;
#endif
   if (dwSize) {
       IHeap *pIHeap = NULL;        //for dymanic memory checking
       if (ISHELL_CreateInstance(pme->m_pShell, AEECLSID_HEAP, (void **)&pIHeap)
           != AEE_SUCCESS) {
          return(FALSE);
       }

       if (!IHEAP_CheckAvail(pIHeap, dwSize)) {
          IHEAP_Release(pIHeap);
          return(FALSE);
       }
       IHEAP_Release(pIHeap);
   }

#if !defined(GIF_SHARE_MEM)
   if (!pme->m_pByteBuff) {
      if ((pme->m_pByteBuff = (byte *) MALLOC(BYTE_BUFF_SIZE)) == NULL) {
         GIF_FreeDict(pme);
         return FALSE;
      }
   }
   if (!pme->m_pStack) {
      if ((pme->m_pStack = (byte *) MALLOC(STACK_SIZE)) == NULL) {
         GIF_FreeDict(pme);
         return FALSE;
      }
   }
   if (!pme->m_pPrefix) {
      if ((pme->m_pPrefix = (uint16 *) MALLOC((MAX_GIFCODES + 1) * sizeof(uint16))) == NULL) {
         GIF_FreeDict(pme);
         return FALSE;
      }
   }

   if(!pme->m_pSuffix) {
      if ((pme->m_pSuffix = (byte *) MALLOC(MAX_GIFCODES + 1)) == NULL) {
         GIF_FreeDict(pme);
         return FALSE;
      }
   }
#endif
   if (!pme->m_pLineBuff) {
      if ((pme->m_pLineBuff = (byte *) MALLOC(pme->m_rcGIFFrame.dx)) == NULL) {
         GIF_FreeDict(pme);
         return FALSE;
      }
   }

   return TRUE;
}

/*===========================================================================

FUNCTION: GIF_FreeDict

DESCRIPTION
   This function frees the memory allocated by GIF_AllocateDict

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void GIF_FreeDict(COEMGIFViewer * pme)
{
#if !defined(GIF_SHARE_MEM)
   if (pme->m_pByteBuff) {
      FREE(pme->m_pByteBuff);
      pme->m_pByteBuff = NULL;
   }
   if(pme->m_pStack) {
      FREE(pme->m_pStack);
      pme->m_pStack = NULL;
   }
   if(pme->m_pSuffix) {
      FREE(pme->m_pSuffix);
      pme->m_pSuffix = NULL;
   }

   if(pme->m_pPrefix) {
      FREE(pme->m_pPrefix);
      pme->m_pPrefix = NULL;
   }

#endif

   if (pme->m_pLineBuff) {
      FREE(pme->m_pLineBuff);
      pme->m_pLineBuff = NULL;
   }

}


/*===========================================================================

FUNCTION: GIF_GetByte

DESCRIPTION
   This function retrieves the next byte for the image from the GIF image buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int GIF_GetByte(COEMGIFViewer * pme, byte **pGIFImgData) {

   int retVal;

   if (*pGIFImgData <  pme->m_pMaxGIF) {
      retVal = (int) *(*pGIFImgData);
      (*pGIFImgData)++;
      return retVal;
   }
   else
      return OEMGIFERR_READ_ERROR;
}

/*===========================================================================

FUNCTION: GIF_LineOut

DESCRIPTION
   This function outputs a line of BMP data. It takes care of interleaved
   GIF images and places the lines accordingly.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int GIF_LineOut(COEMGIFViewer * pme, int len, AEE_GIF_ImgHdr * pGifImgHdr, AEE_GIF_Decoder * pDecodeStruct) {

   dword dwOffset;

   AEE_GIF_Img * pImgNode;
   int           nCount = 0;
   int           nStart;

   //All lines are decoder completed, return immediately. Changed by Oscar
   if (pDecodeStruct->curLineNo >= pme->m_rcGIFFrame.dy)
        //return 0;
        return -1;
   if( (len+pGifImgHdr->wLOffset) > pme->m_pIDIB->cx )
   {
      len = pme->m_pIDIB->cx - pGifImgHdr->wLOffset;
   }
   dwOffset = pme->m_rcGIFFrame.dx * pDecodeStruct->curLineNo  + pGifImgHdr->wLOffset;

   pImgNode = GIF_GetImageNode(pme, pme->m_nCurFrame);
   //if (pImgNode && pImgNode->GIFExt.TranspFlag)
   // SR 753763; is background color white but TranspColor color is black.
   if (pImgNode && pImgNode->GIFExt.TranspFlag && pImgNode->GIFExt.TranspColor)
   {
      do
      {
         nStart = nCount;

         while ((nCount < len) && (pDecodeStruct->pLineBuf[nCount] != pImgNode->GIFExt.TranspColor))
         {
            nCount++;
         }

         if ((nCount - nStart) > 0)
         {
              if(pme->m_rop == AEE_RO_TRANSPARENT)
              {
                byte *pInByte, *pOutByte;
                int i = nCount - nStart;
                byte trans = (byte)pme->m_pIDIB->ncTransparent;

                pInByte = &pDecodeStruct->pLineBuf[nStart];
                pOutByte = pme->m_pIDIB->pBmp + dwOffset + nStart;

                while(i--)
                {
                  if(*pInByte != trans)
                  {
                     *pOutByte = *pInByte;
                  }
                  pInByte++;
                  pOutByte++;
                }
              }
              else
              {
               MEMCPY(pme->m_pIDIB->pBmp + dwOffset + nStart, &pDecodeStruct->pLineBuf[nStart], nCount - nStart);
            }
         }

         while ((nCount < len) && (pDecodeStruct->pLineBuf[nCount] == pImgNode->GIFExt.TranspColor))
         {
            nCount++;
         }
      } while (nCount < len);
   }
   else
   {
        if(pme->m_rop == AEE_RO_TRANSPARENT)
        {
          byte *pInByte, *pOutByte;
          int i = len;
          byte trans = (byte)pme->m_pIDIB->ncTransparent;

          pInByte = pDecodeStruct->pLineBuf;
          pOutByte = pme->m_pIDIB->pBmp + dwOffset;

          while(i--)
          {
            if(*pInByte != trans)
            {
               *pOutByte = *pInByte;
            }
            pInByte++;
            pOutByte++;
          }
        }
        else
        {
         MEMCPY(pme->m_pIDIB->pBmp + dwOffset, pDecodeStruct->pLineBuf, len);
      }
   }

   if (pGifImgHdr->IlaceFlag) {

      switch(pDecodeStruct->curPassNo) {
         case 0:
         pDecodeStruct->curLineNo = pDecodeStruct->curLineNo + 8;
         if (pDecodeStruct->curLineNo >= (pGifImgHdr->wTOffset + pGifImgHdr->wImgHeight)) {
            pDecodeStruct->curPassNo = 1;
            pDecodeStruct->curLineNo = pGifImgHdr->wTOffset + 4;
         }
         break;
         case 1:
         pDecodeStruct->curLineNo = pDecodeStruct->curLineNo + 8;
         if (pDecodeStruct->curLineNo >= (pGifImgHdr->wTOffset + pGifImgHdr->wImgHeight)) {
            pDecodeStruct->curPassNo = 2;
            pDecodeStruct->curLineNo = pGifImgHdr->wTOffset + 2;
         }
         break;
         case 2:
         pDecodeStruct->curLineNo = pDecodeStruct->curLineNo + 4;
         if (pDecodeStruct->curLineNo >= (pGifImgHdr->wTOffset + pGifImgHdr->wImgHeight)) {
            pDecodeStruct->curPassNo = 3;
            pDecodeStruct->curLineNo = pGifImgHdr->wTOffset + 1;
         }
         break;
         case 3:
         pDecodeStruct->curLineNo = pDecodeStruct->curLineNo + 2;
         //if (pDecodeStruct->curLineNo >= (pGifImgHdr->wTOffset + pGifImgHdr->wImgHeight))
            //return 0;
      }
   }

   else {
      pDecodeStruct->curLineNo++;
   }

    //return 0;
    //All lines are decoder completed, return immediately. Changed by Oscar
   if (pDecodeStruct->curLineNo >= (pGifImgHdr->wTOffset + pGifImgHdr->wImgHeight))
      return -1;
   else
         return 0;
}

/*===========================================================================

FUNCTION: GIF_InitDecoder

DESCRIPTION
   This function initializes the decoder for reading a new image.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int16 GIF_InitDecoder(AEE_GIF_Decoder *pDecodeStruct,int16 size) {

   pDecodeStruct->curr_size = size + 1;
   pDecodeStruct->top_slot = 1 << pDecodeStruct->curr_size;
   pDecodeStruct->clear = 1 << size;
   pDecodeStruct->ending = pDecodeStruct->clear + 1;
   pDecodeStruct->slot = pDecodeStruct->newcodes = pDecodeStruct->ending + 1;
   pDecodeStruct->navail_bytes = pDecodeStruct->nbits_left = 0;
   return(0);
}

/*===========================================================================

FUNCTION: GIF_GetNextCode

DESCRIPTION
   This function gets the next code from the GIF file.  Returns the code,
   or else a negative number.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int16 GIF_GetNextCode(COEMGIFViewer * pme, AEE_GIF_Decoder *pDecodeStruct,
                           byte * pByteBuff, byte **pGIFImgData){

   int16 i, x;
   dword ret;

   if (pDecodeStruct->nbits_left == 0)
   {
      if (pDecodeStruct->navail_bytes <= 0)
      {

      /* Out of bytes in current block, so read next block
         */
         pDecodeStruct->pbytes = pByteBuff;
         if ((pDecodeStruct->navail_bytes = GIF_GetByte(pme, pGIFImgData)) < 0)
            return(pDecodeStruct->navail_bytes);
         else if (pDecodeStruct->navail_bytes)
         {
            for (i = 0; i < pDecodeStruct->navail_bytes; ++i)
            {
               if ((x = GIF_GetByte(pme, pGIFImgData)) < 0)
                  return(x);
               pByteBuff[i] = (byte) x;
            }
         }
      }
      pDecodeStruct->b1 = *pDecodeStruct->pbytes++;
      pDecodeStruct->nbits_left = 8;
      --pDecodeStruct->navail_bytes;
   }

   ret = pDecodeStruct->b1 >> (8 - pDecodeStruct->nbits_left);
   while (pDecodeStruct->curr_size > pDecodeStruct->nbits_left)
   {
      if (pDecodeStruct->navail_bytes <= 0)
      {

      /* Out of bytes in current block, so read next block
         */
         pDecodeStruct->pbytes = pByteBuff;
         if ((pDecodeStruct->navail_bytes = GIF_GetByte(pme, pGIFImgData)) < 0)
            return(pDecodeStruct->navail_bytes);
         else if (pDecodeStruct->navail_bytes)
         {
            for (i = 0; i < pDecodeStruct->navail_bytes; ++i)
            {
               if ((x = GIF_GetByte(pme, pGIFImgData)) < 0)
                  return(x);
               pByteBuff[i] = (byte) x;
            }
         }
      }
      pDecodeStruct->b1 = *pDecodeStruct->pbytes++;
      ret |= pDecodeStruct->b1 << pDecodeStruct->nbits_left;
      pDecodeStruct->nbits_left += 8;
      --pDecodeStruct->navail_bytes;
   }
   pDecodeStruct->nbits_left -= pDecodeStruct->curr_size;
   ret &= code_mask[pDecodeStruct->curr_size];


   return((int16)(ret));
}

IDIB *COEMGIFViewer_SetOutputImageData(COEMGIFViewer * pme, IDIB *dib)
{
  IDIB *prev = pme->m_pIOutDIB;
   pme->m_pIOutDIB = dib;
   if (pme->m_pIOutDIB)
   {
      pme->m_info.cx      = dib->cx;
      pme->m_info.cy      = dib->cy;
      pme->m_info.cxFrame = dib->cx;
//      pme->m_xOffset    = 0;
//      pme->m_yOffset    = 0;
//      pme->m_rop        = AEE_RO_COPY;
   }
   return prev;
} /* END OEMJPEG_GetIDIB */

static void COEMGIFViewer_ScaleImage(COEMGIFViewer * pMe, int n1, int n2, int x, int y, AEERasterOp rop)
{
   IBitmap * pITmpBitmap = 0;
   IBitmap * pBitmap = 0;
   IBitmap * pbmDst = 0;
   IDIB    * pIOutDib = 0;
   IDIB    * pTempDib = 0;
   int       nNewOutputImage = TRUE;
   int       nErr;
   int       nTrans = AEEBITMAPSCALE_OPAQUE;
   AEERect   rcSrc;
   AEERect   rcDst;
   AEERect   rcClip;
   AEEBitmapInfo bi;

   if (n1 == pMe->m_pIDIB->cx && 
       n2 == pMe->m_pIDIB->cy) {
      // replace the output dib with the main dib with increase ref count
      pIOutDib = pMe->m_pIDIB;
      ADDREFIF(pIOutDib);
      ADDREFIF(pIOutDib);
      
      // The output image has to be replaced. It may be possible that 
      // this is the zoomed out or the frames are changing.
      nNewOutputImage = TRUE;
   } else {
      
      if ((IBitmapScale*)0 == pMe->pScale) {
         goto bail;
      }

      if ((IDIB*)0 != pMe->m_pIOutDIB) {
         if (pMe->m_pIOutDIB->cx == n1 && 
             pMe->m_pIOutDIB->cy == n2) {
            nNewOutputImage = FALSE;
         }
      }

      if (TRUE == nNewOutputImage) {
         pbmDst = IDisplay_GetDestination(pMe->m_pDisplay);
         if ((IBitmap*)0 == pbmDst) {
            goto bail;
         }

         nErr = IBitmap_CreateCompatibleBitmap(pbmDst, &pITmpBitmap, n1, n2);
         if (AEE_SUCCESS != nErr) {
            goto bail;
         }
      } else {
         pITmpBitmap = IDIB_TO_IBITMAP(pMe->m_pIOutDIB);
      }

      if (AEE_RO_TRANSPARENT == rop) {
         nTrans = AEEBITMAPSCALE_KEYCOLOR;
      } else if (AEE_RO_BLEND == rop) {
         nTrans = AEEBITMAPSCALE_ALPHA;
      }

      pBitmap = (IBitmap*)pMe->m_pIDIB;
      ADDREFIF(pBitmap);

      nErr = IBitmap_GetInfo(pBitmap, &bi, sizeof(bi));
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }

      rcSrc.x = pMe->m_xOffset;
      rcSrc.y = pMe->m_yOffset;
      rcSrc.dx = (uint16)bi.cx;
      rcSrc.dy = (uint16)bi.cy;

      rcDst.x = x;
      rcDst.y = y;
      rcDst.dx = n1;
      rcDst.dy = n2;

      IDisplay_GetClipRect(pMe->m_pDisplay, &rcClip);

      nErr = IBitmapScale_Scale(pMe->pScale, 
                  pBitmap, &rcSrc, pITmpBitmap, &rcDst, &rcClip, nTrans);
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }
      
      nErr = IBitmap_QueryInterface(pITmpBitmap, AEECLSID_DIB, (void**) &pIOutDib);
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }
      ADDREFIF(pIOutDib);
   }

   // If the there is new output image to be shown, replace the dib.
   if (TRUE == nNewOutputImage) {
      pTempDib = COEMGIFViewer_SetOutputImageData( pMe, pIOutDib);
      RELEASEIF(pTempDib);
   }

bail:
   RELEASEIF(pITmpBitmap);
   RELEASEIF(pBitmap);
   RELEASEIF(pbmDst);
   RELEASEIF(pIOutDib);
   RELEASEIF(pTempDib);
}

