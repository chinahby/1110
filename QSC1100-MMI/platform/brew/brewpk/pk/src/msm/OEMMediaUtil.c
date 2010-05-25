/* =========================================================================

FILE: AEEMediaUtil.c

SERVICES: IMedia utility functions

DESCRIPTION
  This file contains IMedia related utility functions that apps can use
  to develop BREW multimedia applications.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


           Copyright © 2000-2005 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
========================================================================= */


/*=========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
========================================================================= */
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_MULTIMEDIA)

#include "cmx.h"
#ifndef T_QSC6010
#include "ipl.h"
#endif /* T_QSC6010 */

#ifdef FEATURE_JPEG_ENCODER_REV2
#include "jpege.h"
#else
#ifdef FEATURE_JPEG_ENCODER
#include "jpeg.h"
#endif //FEATURE_JPEG_ENCODER
#endif //FEATURE_JPEG_ENCODER_REV2

#ifdef FEATURE_MEDIA_PNG_ENCODER
#include "pnge.h"
#endif

#include "AEEEncodeSpec.h"
#include "OEMHeap.h"
#include "OEMFS.h"
#include "OEMObjectMgr.h"
#include "OEMCriticalSection.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMFile.h"
#include "AEEMediaUtil.h"
#include "AEEStdLib.h"   

/*=========================================================================

                      DEFINES

========================================================================= */
#define OEMMM_ENTER_CRITICAL_SECTION()    OEMCriticalSection_Enter(&gMediaUtilCriticalSection); {
#define OEMMM_LEAVE_CRITICAL_SECTION()  ;} OEMCriticalSection_Leave(&gMediaUtilCriticalSection);

#if defined(FEATURE_AUDIO_POSTCARD)
#define FEATURE_PMD_ENCODE
#endif

/*=========================================================================

                      IMediaUtil DECLARATIONS

=========================================================================*/
struct IMediaUtil
{
   const AEEVTBL(IMediaUtil) *pvt;
   int32                m_nRefs;
   IShell *             m_pIShell;

   // EncodeMedia() info
   AEEObjectHandle      m_hObjEncode;
   AEEMediaEncodeResult *  m_per;

#ifdef FEATURE_JPEG_ENCODER_REV2
   //variables to hold the converted image data 
   ipl_image_type m_imgMain;
   ipl_image_type m_imgThumb;
#endif //FEATURE_JPEG_ENCODER_REV2

};

extern int     IMediaUtil_New(IShell * ps, AEECLSID cls, void **ppif);
//static void    IMediaUtil_Delete(IMediaUtil * pme);
static uint32  IMediaUtil_AddRef(IMediaUtil * pme);
static uint32  IMediaUtil_Release(IMediaUtil * pme);
static int     IMediaUtil_QueryInterface(IMediaUtil * pme, AEECLSID idReq, void ** ppo);
static int     IMediaUtil_CreateMedia(IMediaUtil * pme, AEEMediaData * pmd, IMedia ** ppm);
static int     IMediaUtil_EncodeMedia(IMediaUtil * pme, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb);

static const AEEVTBL(IMediaUtil) gvtIMediaUtil =
{
   IMediaUtil_AddRef,
   IMediaUtil_Release,
   IMediaUtil_QueryInterface,
   IMediaUtil_CreateMedia,
   IMediaUtil_EncodeMedia
};

static IMediaUtil gsMediaUtil = { 0 };

#if defined (FEATURE_PMD_ENCODE) || defined (FEATURE_JPEG_ENCODER_REV2) || defined (FEATURE_JPEG_ENCODER) || defined (FEATURE_MEDIA_PNG_ENCODER)
static OEMCriticalSection gMediaUtilCriticalSection;
#endif // defined (FEATURE_PMD_ENCODE) || defined (FEATURE_JPEG_ENCODER_REV2)

/*=========================================================================

                     IMediaUtil FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================

==================================================================*/
int IMediaUtil_New(IShell * ps, AEECLSID cls, void **ppif)
{
   IMediaUtil *   pme = &gsMediaUtil;

   if (!pme->m_nRefs)
   {
      pme->pvt = &gvtIMediaUtil;
      pme->m_nRefs = 1;
      pme->m_pIShell = ps;
   }

   if (ppif)
      *ppif = pme;

   return SUCCESS;
}

/*==================================================================

==================================================================*/
//static void IMediaUtil_Delete(IMediaUtil * pme)
//{
//}

/*==================================================================

==================================================================*/
static uint32 IMediaUtil_AddRef(IMediaUtil * pme)
{
   return 1;
}

/*==================================================================

==================================================================*/
static uint32 IMediaUtil_Release(IMediaUtil * pme)
{
   return 0;
}

/*==================================================================

==================================================================*/
static int IMediaUtil_QueryInterface(IMediaUtil * pme, AEECLSID idReq, void ** ppo)
{
   if (!ppo)
      return EBADPARM;

   if (NULL != IQI_SELF(pme, idReq, ppo, AEECLSID_MEDIAUTIL)) 
   {
      IQI_AddRef(*ppo);
      return SUCCESS;
   }

   return ECLASSNOTSUPPORT;
}

/*=========================================================================

========================================================================= */
static int IMediaUtil_CreateMedia(IMediaUtil * pme, AEEMediaData * pmd, IMedia ** ppm)
{
   return AEEMediaUtil_CreateMedia(pme->m_pIShell, pmd, ppm);
}

/*=========================================================================

                     ENCODING MEDIA

========================================================================= */
#if defined(FEATURE_BREW_CMX_V3)

static int  IMediaUtil_EncodeAudioPostcard(IMediaUtil * pme, AEEMediaEncodeResult * per, AEEMediaEncodeInfo * pei, AEECallback * pcb);
static int  IMediaUtil_EncodeJPEG(IMediaUtil * pme, AEEMediaEncodeResult * per, AEEMediaEncodeInfo * pei, AEECallback * pcb);
static int  IMediaUtil_EncodePNG(IMediaUtil * pme, AEEMediaEncodeResult * per, AEEMediaEncodeInfo * pei, AEECallback * pcb);
#ifdef FEATURE_PMD_ENCODE
static void IMediaUtil_CMXAudioPostcardCB(cmx_status_type status, const void *client_data);
#endif // FEATURE_PMD_ENCODE

#ifdef FEATURE_JPEG_ENCODER_REV2
static void IMediaUtil_EncodeJPEGCB(JPEGENC_CBrtnType *rtnData);
static int IMediaUtil_ConvertJPEGData(IMediaUtil * pme, uint16 inColor, uint8 *pData, JPEGENC_encodeSpecType *encInfo, boolean bMain);
#else
#ifdef FEATURE_JPEG_ENCODER
static void IMediaUtil_EncodeJPEGCB(jpege_status_enum_type status,
                                    void *handle, void *server_data);
static boolean IMediaUtil_MapColorFormat(uint16 inColor, jpege_format_type *outColor);
#endif //FEATURE_JPEG_ENCODER
#endif //FEATURE_JPEG_ENCODER_REV2

#ifdef FEATURE_MEDIA_PNG_ENCODER
LOCAL pnge_handle_type pngOutputDescHandle;
static void IMediaUtil_EncodePNGCB(PNGE_status_enum_type command, void *handle,
                                   void *pnge_data);
static boolean IMediaUtil_MapToPNGColorFormat(uint16 inColor, 
                                              PNGE_rawDataType* outColor);
#endif //FEATURE_MEDIA_PNG_ENCODER

static void IMediaUtil_ResetEncode(IMediaUtil * pme);

#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static void IMediaUtil_CancelEncodeCB(AEECallback * pcb);
static int  IMediaUtil_RegisterEncodeCB(IMediaUtil * pme, AEECallback * pcb);
static int  IMediaUtil_GetFullMediaFileName(IMediaUtil * pme, const char *inName, char *outName, int size);
#endif
/*=========================================================================

========================================================================= */
static int IMediaUtil_EncodeMedia(IMediaUtil * pme, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
   if (!per || !clsDest || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList)
      return EBADPARM;

   // First cancel pending callback, if any.
   if (pcb)
      CALLBACK_Cancel(pcb);

   IMediaUtil_ResetEncode(pme);

   switch(clsDest)
   {
   case AEECLSID_MEDIAPMD:
      return IMediaUtil_EncodeAudioPostcard(pme, per, pei, pcb);
   case AEECLSID_JPEG:
      return IMediaUtil_EncodeJPEG(pme, per, pei, pcb);
   case AEECLSID_PNG:
       return IMediaUtil_EncodePNG(pme, per, pei, pcb);
   default:
      return EUNSUPPORTED;
   }
}

/*=========================================================================

========================================================================= */
static int IMediaUtil_EncodeAudioPostcard(IMediaUtil * pme, AEEMediaEncodeResult * per, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
#if defined(FEATURE_PMD_ENCODE)
   cmx_handle_type *    ph[3] = { NULL, NULL, NULL };
   int                  i;
   int                  nRet = SUCCESS;
   cmx_cb_func_ptr_type pfn;
   void *               pUser;

   if (pei->nCount != 2)
      return EBADPARM;

   // Only files are supported.
   if (pei->pmdList[0].clsData != MMD_FILE_NAME || pei->pmdList[0].clsData != MMD_FILE_NAME
       || pei->pmdDest->clsData != MMD_FILE_NAME)
       return EUNSUPPORTED;

   //
   // Here you can verify if one media is JPEG and another is QCP...
   // Currently, assumes: first media is JPEG and second is PMD!
   //

   // First link new system callback to app callback and fill cancel info.
   // Then register system callback with ObjectMgr...
   if (pcb)
   {
      nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
      if (nRet)
         return nRet;
   }

   for (i = 0; i < 3; i++)
   {
      const char *cpszPath;

      ph[i] = MALLOC(sizeof(cmx_handle_type));
      if (!ph[i])
      {
         nRet = ENOMEMORY;
         goto Done;
      }

      ph[i]->source_type = ph[i]->efs.source_type = CMX_SOURCE_EFS;

      if (i < 2) {
         cpszPath = (const char *)pei->pmdList[i].pData;
      } else {
         cpszPath = (const char *)pei->pmdDest->pData;
      }

      nRet = IMediaUtil_GetFullMediaFileName(pme, cpszPath, ph[i]->efs.filename, sizeof(ph[i]->efs.filename));
      if (nRet)
        goto Done;
   }

   // Update and save the destination info...

   if (pcb)
   {
      per->clsMedia = AEECLSID_MEDIAPMD;
      per->pmdDest = pei->pmdDest;
      pme->m_per = per;
      
      pfn = IMediaUtil_CMXAudioPostcardCB;
      pUser = (void *)pme->m_hObjEncode;
   }
   else
   {
      pfn = NULL;
      pUser = NULL;
   }

   cmx_create_audiop(ph[0], ph[1], ph[2], pfn, pUser);

Done:
   for (i = 0; i < 3; i++)
   {
      FREEIF(ph[i]);
   }

   return nRet;
#else // FEATURE_PMD_ENCODE
   return EUNSUPPORTED;
#endif // FEATURE_PMD_ENCODE
}

#if defined(FEATURE_PMD_ENCODE)
/*=========================================================================

========================================================================= */
static void IMediaUtil_CMXAudioPostcardCB(cmx_status_type status, const void *client_data)
{
   OEMMM_ENTER_CRITICAL_SECTION()

   AEECallback *  pcb = (AEECallback *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
   IMediaUtil  *  pme;

   if (!pcb)
      goto CBExit;

   pme = (IMediaUtil *)pcb->pCancelData;

   pme->m_per->nResult = (status == CMX_SUCCESS) ? SUCCESS : EFAILED;
   pme->m_per->pEncodeSpec = NULL;
   pme->m_per->dwSpecSize = 0;

   if (status != CMX_SUCCESS)
   {
      pme->m_per->clsMedia = 0;
      pme->m_per->pmdDest = NULL;
   }

   ISHELL_Resume(pme->m_pIShell, pcb);

   IMediaUtil_ResetEncode(pme);

CBExit:
   OEMMM_LEAVE_CRITICAL_SECTION()
}
#endif // FEATURE_PMD_ENCODE

/*=========================================================================

========================================================================= */
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static void IMediaUtil_CancelEncodeCB(AEECallback * pcb)
{
   IMediaUtil  *  pme = (IMediaUtil *)pcb->pCancelData;

   if (!pme)
      return;

   pcb->pfnCancel = NULL;
   pcb->pCancelData = NULL;

   IMediaUtil_ResetEncode(pme);
}
#endif //#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) 
	   // || defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)

/*=========================================================================

========================================================================= */
static void IMediaUtil_ResetEncode(IMediaUtil * pme)
{
   if (!pme->m_per)
      return;

   pme->m_per = NULL;

   AEEObjectMgr_Unregister(pme->m_hObjEncode);
   pme->m_hObjEncode = 0;
}

/*=========================================================================

========================================================================= */
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static int     IMediaUtil_RegisterEncodeCB(IMediaUtil * pme, AEECallback * pcb)
{
  AEECallback * pcbSys;
  int           nRet = SUCCESS;

  pcbSys = AEE_LinkSysCallback(pcb);

  if (!pcbSys)
     return ENOMEMORY;

  pcbSys->pfnCancel = IMediaUtil_CancelEncodeCB;
  pcbSys->pCancelData = pme;
  nRet = AEEObjectMgr_Register(pcbSys, &pme->m_hObjEncode);

  return nRet;
}

/*=========================================================================

========================================================================= */
static int IMediaUtil_GetFullMediaFileName(IMediaUtil * pme, const char *inName, char *outName, int size)
{
  int   nRet = SUCCESS;
  int   nFullPathLen;
  char *pszFullPath;

  nRet = AEE_ResolvePath(inName, 0, &nFullPathLen);

  if (nRet) 
    return nRet;
         
  pszFullPath = (char *)sys_malloc(nFullPathLen);

  if ((char *)0 == pszFullPath) 
  {
    return ENOMEMORY;
  }
        
  AEE_ResolvePath(inName, pszFullPath, &nFullPathLen);
         
         
  nRet = AEE_CheckPathAccess(pszFullPath, AEEFP_READ, 0);
                  
  if (SUCCESS == nRet) 
  {
    // convert to EFS name
    nRet = OEMFS_GetNativePath(pszFullPath,
                               outName,
                               &size);
  }

  sys_free(pszFullPath);

  return nRet;
}

#endif //#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) 
	   // || defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
/*=========================================================================

========================================================================= */
#ifdef FEATURE_JPEG_ENCODER_REV2
static void IMediaUtil_EncodeJPEGCB(JPEGENC_CBrtnType *rtnData)
{
  OEMMM_ENTER_CRITICAL_SECTION()

  AEECallback *  pcb = (AEECallback *)AEEObjectMgr_GetObject((AEEObjectHandle)rtnData->clientId);
  IMediaUtil  *  pme;

  if (!pcb)
    goto CBExit;

  pme = (IMediaUtil *)pcb->pCancelData;

  if (rtnData)
  {
    pme->m_per->nResult = (rtnData->status == JPEGENC_IMG_DONE) ? SUCCESS : EFAILED;
    pme->m_per->pEncodeSpec = NULL;
    pme->m_per->dwSpecSize = 0;
  }

  if (pme->m_per->nResult != SUCCESS)
  {
    pme->m_per->clsMedia = 0;
    pme->m_per->pmdDest = NULL;
  }

  FREEIF(pme->m_imgMain.imgPtr);
  FREEIF(pme->m_imgThumb.imgPtr);

  ISHELL_Resume(pme->m_pIShell, pcb);

  IMediaUtil_ResetEncode(pme);

CBExit:
   OEMMM_LEAVE_CRITICAL_SECTION()
}

/*=========================================================================
FUNCTION      IMediaUtil_ConvertJPEGData
DESCRIPTION   Covert the input data to appropriate color format for JPEG encoding
========================================================================= */
static int IMediaUtil_ConvertJPEGData(IMediaUtil * pme, uint16 inColor, uint8 *pData, JPEGENC_encodeSpecType *encInfo, boolean bMain)
{
  JPEGENC_formatType  fmtType;
  ipl_image_type      inData;
  ipl_image_type      *pOutData = NULL;
  uint32              imageBytes = 0;
  ipl_status_type     ret = IPL_FAILURE;

  ZEROAT(&inData);
  inData.cFormat = inColor;
  inData.imgPtr = pData;

  if (bMain) //conversion is for main image
  {
    pOutData = &pme->m_imgMain;
    fmtType = encInfo->Main.Subsample;
    inData.dx = pOutData->dx = encInfo->Main.Width;
    inData.dy = pOutData->dy = encInfo->Main.Height;
    
  }
  else //conversion is for thumbnail image
  {
    pOutData = &pme->m_imgThumb;
    fmtType = encInfo->Thumbnail.Subsample;
    inData.dx = pOutData->dx = encInfo->Thumbnail.Width;
    inData.dy = pOutData->dy = encInfo->Thumbnail.Height;
  }

  if (fmtType == JPEGENC_H2V2) //convert destination color format to YCbCr420lp
  {
    if (pOutData->dy & 1)
    {
      pOutData->dy++;   // ycbcr420lp must have even height
    }
    imageBytes = (pOutData->dx * pOutData->dy * 3) / 2;
    pOutData->imgPtr  = MALLOC(imageBytes);
    if (!pOutData->imgPtr)
      return ENOMEMORY;

    pOutData->clrPtr = pOutData->imgPtr + (pOutData->dx * pOutData->dy);
    pOutData->cFormat = IPL_YCrCb420_LINE_PK;

    switch (inColor)
    {
    case IPL_YCbCr:
      ret = ipl_convert_ycbcr422_to_ycrcb420lp(&inData, pOutData);
      break;
    case IPL_YCrCb420_LINE_PK:
      FREE(pOutData->imgPtr);
      pOutData = &inData;
      ret = IPL_SUCCESS;
      break;
    case IPL_YCbCr422_LINE_PK:
      ret = ipl_convert_ycbcr422lp_to_ycbcr420lp(&inData, pOutData);
      break;
    case IPL_RGB565:
      ret = ipl_convert_rgb565_to_ycrcb420lp(&inData, pOutData);
      break;
    case IPL_RGB666:
      ret = ipl_convert_rgb666_to_ycbcr420lp(&inData, pOutData);
      break;
    case IPL_RGB444:
      ret = ipl_convert_rgb444_to_ycbcr420lp(&inData, pOutData);
      break;
    case IPL_YCrCb444_LINE_PK:
      ret = ipl_convert_ycbcr444lp_to_ycbcr420lp(&inData, pOutData);
      break;
    default:
      FREE(pOutData->imgPtr);
      break; 
    }
  }
  else if (fmtType == JPEGENC_H2V1) //convert destination color format to YCbCr422lp
  {
    imageBytes = inData.dx * inData.dy * 2;
    pOutData->imgPtr  = MALLOC(imageBytes);
    if (!pOutData->imgPtr)
      return ENOMEMORY;

    pOutData->clrPtr = pOutData->imgPtr + (pOutData->dx * pOutData->dy);
    pOutData->cFormat = IPL_YCrCb422_LINE_PK;

    switch (inColor)
    {
    case IPL_YCbCr:
      ret = ipl_convert_ycbcr422_to_ycrcb422lp(&inData, pOutData);
      break;
    case IPL_YCrCb420_LINE_PK:
      ret = ipl_convert_ycbcr420lp_to_ycbcr422lp(&inData, pOutData);
      break;
    case IPL_YCbCr422_LINE_PK:
      FREE(pOutData->imgPtr);
      pOutData = &inData;
      ret = IPL_SUCCESS;
      break;
    case IPL_RGB565:
      ret = ipl_convert_rgb565_to_ycrcb422lp(&inData, pOutData);
      break;
    default:
      FREE(pOutData->imgPtr);
      break; 
    }
  }

  if (ret == IPL_SUCCESS)
  {
    //Assign the output data to encInfo
    if (bMain) 
    {
      encInfo->Main.Fragmnt[0].Luma_ptr = pOutData->imgPtr;
      encInfo->Main.Fragmnt[0].Chroma_ptr = pOutData->clrPtr;
    }
    else
    {
      encInfo->Thumbnail.Data.Luma_ptr = pOutData->imgPtr;
      encInfo->Thumbnail.Data.Chroma_ptr = pOutData->clrPtr;
    }
    return SUCCESS;
  }

  return EUNSUPPORTED;
}
#else
#ifdef FEATURE_JPEG_ENCODER
/*=========================================================================

========================================================================= */
static void IMediaUtil_EncodeJPEGCB(jpege_status_enum_type status,
                                    void *handle, void *server_data)
{
  OEMMM_ENTER_CRITICAL_SECTION()

  AEECallback *  pcb = (AEECallback *)AEEObjectMgr_GetObject((AEEObjectHandle)handle);
  IMediaUtil  *  pme;

  if (!pcb)
    goto CBExit;

  pme = (IMediaUtil *)pcb->pCancelData;

  pme->m_per->nResult = (status == JPEGE_CB_DONE) ? SUCCESS : EFAILED;
  pme->m_per->pmdDest->dwSize = *((uint32 *)server_data); //num bytes of output in MEM mode or zero.
  pme->m_per->pEncodeSpec = NULL;
  pme->m_per->dwSpecSize = 0;

  if (status != JPEGE_CB_DONE)
  {
    pme->m_per->clsMedia = 0;
    pme->m_per->pmdDest = NULL;
  }

  ISHELL_Resume(pme->m_pIShell, pcb);

  IMediaUtil_ResetEncode(pme);

CBExit:
  OEMMM_LEAVE_CRITICAL_SECTION()

}

/*=========================================================================

========================================================================= */
static boolean IMediaUtil_MapColorFormat(uint16 inColor, jpege_format_type *outColor)
{
  boolean bMap = TRUE;
  switch (inColor)
  {
  case IPL_YCbCr:
    *outColor = JPEGE_YCBCR;
    break;
  case IPL_RGB565:
    *outColor = JPEGE_RGB565;
    break;
  case IPL_RGB666:
    *outColor = JPEGE_RGB666;
    break;
  case IPL_RGB444:
    *outColor = JPEGE_RGB444;
    break;
  case IPL_RGB888:
    *outColor = JPEGE_RGB888;
    break;
  default:
    bMap = FALSE;
    break;
  }
  return bMap;
}

#endif //FEATURE_JPEG_ENCODER
#endif // FEATURE_JPEG_ENCODER_REV2

/*=========================================================================

========================================================================= */
static int IMediaUtil_EncodeJPEG(IMediaUtil * pme, AEEMediaEncodeResult * per, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
#ifdef FEATURE_JPEG_ENCODER_REV2

  JPEGENC_encodeSpecType *encInfo;
  const char *cpszPath;
  ImageEncodeSpec *pEncodeSpec;
  int nRet = SUCCESS;

  if (!per || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList)
      return EBADPARM;

  // Only [in] buffers and [out]file are supported.
  if (pei->pmdList[0].clsData != MMD_BUFFER || pei->pmdDest->clsData != MMD_FILE_NAME)
    return EUNSUPPORTED;

  // First link new system callback to app callback and fill cancel info.
  // Then register system callback with ObjectMgr...
  if (pcb)
  {
    nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
    if (nRet)
       return nRet;
  }

  encInfo = MALLOC(sizeof(JPEGENC_encodeSpecType));
  if (!encInfo)
    return ENOMEMORY;

  ZEROAT(encInfo);

  // Form fully qualified path name to output file.
  cpszPath = (const char *) (pei->pmdDest->pData);
  nRet = IMediaUtil_GetFullMediaFileName(pme, cpszPath, 
            encInfo->Dest.handle.efs.filename, sizeof(encInfo->Dest.handle.efs.filename));
  if (nRet)
    goto Done;

  //Fill in the encoding infor.
  encInfo->ClientId = (uint32) pme->m_hObjEncode;
  encInfo->CallBack = IMediaUtil_EncodeJPEGCB;
  encInfo->Dest.device = JPEGENC_EFS;
  encInfo->Rotation = JPEGENC_NO_ROT;

  pEncodeSpec = (ImageEncodeSpec *)pei->pEncodeSpec;
  if (pEncodeSpec->bQuality)
    encInfo->Main.Quality = pEncodeSpec->size.nQuality;
  else
    encInfo->Main.Quality = 90;
  encInfo->Main.Width = encInfo->Main.OrigWidth = pEncodeSpec->wMainWidth;
  encInfo->Main.Height = encInfo->Main.OrigHeight = pEncodeSpec->wMainHight;
  encInfo->Main.Restart = 10;
  encInfo->Main.Subsample = JPEGENC_H2V2;
  encInfo->Main.FragCnt = 1;
  encInfo->Main.Fragmnt[0].Height = pEncodeSpec->wMainHight;
  encInfo->Main.Fragmnt[0].Reserved = 0;
  //fill the main image date
  if (IMediaUtil_ConvertJPEGData(pme, pEncodeSpec->wMainInColor, pei->pmdList[0].pData, encInfo, TRUE) != SUCCESS)
    goto Done;

  encInfo->HasThumbnail = ((pei->nCount > 1) && (pEncodeSpec->wThumbWidth > 0));

  if (encInfo->HasThumbnail)
  {
    if (pEncodeSpec->bQuality)
      encInfo->Thumbnail.Quality = pEncodeSpec->size.nQuality;
    else
      encInfo->Thumbnail.Quality = 90;
    encInfo->Thumbnail.Width = pEncodeSpec->wThumbWidth;
    encInfo->Thumbnail.Height= pEncodeSpec->wThumbHigh;
    encInfo->Thumbnail.Restart= 0;
    encInfo->Thumbnail.Subsample = JPEGENC_H2V1;
    encInfo->Thumbnail.Data.Height = pEncodeSpec->wThumbHigh;
    encInfo->Thumbnail.Data.Reserved = 0;
    //fill the thumbnail data.
    if (IMediaUtil_ConvertJPEGData(pme, pEncodeSpec->wThumbInColor, pei->pmdList[1].pData, encInfo, FALSE) != SUCCESS)
      goto Done;
  }

  // Update and save the destination info...
  if (pcb)
  {
    per->clsMedia = AEECLSID_JPEG;
    per->pmdDest = pei->pmdDest;
    pme->m_per = per;
  }

  if (pEncodeSpec->pHeaderBuf)
    nRet = (int)jpege_encode_exif(encInfo, (exif_info_type *)pEncodeSpec->pHeaderBuf);
  else
    nRet = (int)jpege_encode_jfxx(encInfo);

Done:
  if (nRet && pcb)
    IMediaUtil_CancelEncodeCB(pcb);

  FREEIF(encInfo);
  return nRet;

#else
#ifdef FEATURE_JPEG_ENCODER
  int nRet = SUCCESS;
  jpege_encode_type   encInfo;
  ImageEncodeSpec *pEncodeSpec = (ImageEncodeSpec *)pei->pEncodeSpec;

  if (!per || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList)
    return EBADPARM;

  // First link new system callback to app callback and fill cancel info.
  // Then register system callback with ObjectMgr...
  if (pcb)
  {
    nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
    if (nRet)
       return nRet;
  }

  ZEROAT(&encInfo);
  if (!IMediaUtil_MapColorFormat(pEncodeSpec->wMainInColor, &encInfo.format))
  {
    nRet = EUNSUPPORTED;
    goto Done;
  }

  encInfo.dx = pEncodeSpec->wMainWidth;
  encInfo.dy = pEncodeSpec->wMainHight;
  encInfo.in_data = (byte *)pei->pmdList[0].pData;

  if (pei->pmdDest->clsData == MMD_BUFFER)
  {
    encInfo.device = JPEGE_DEVICE_MEM;
    encInfo.device_spec.mem.buffer = (uint8 *)(pei->pmdDest->pData);
    encInfo.device_spec.mem.length = pei->pmdDest->dwSize;
  }
  else
  {
    const char *cpszPath = (const char *)(pei->pmdDest->pData);
    encInfo.device = JPEGE_DEVICE_EFS;
    nRet = IMediaUtil_GetFullMediaFileName(pme, cpszPath, 
            encInfo.device_spec.efs.filename, sizeof(encInfo.device_spec.efs.filename));
    if (nRet)
      goto Done;
  }

  if (pEncodeSpec->bQuality)
    encInfo.quality = pEncodeSpec->size.nQuality;
  else
    encInfo.max_file_size = pEncodeSpec->size.nMaxFileSize;

  // Update and save the destination info...
  if (pcb)
  {
    per->clsMedia = AEECLSID_JPEG;
    per->pmdDest = pei->pmdDest;
    pme->m_per = per;
  }
  
  if (JPEGE_SUCCESS != jpege_encode_fn(encInfo, (void *) pme->m_hObjEncode, IMediaUtil_EncodeJPEGCB))
  {
    nRet = EBADPARM;
  }

Done:
  if (nRet && pcb)
    IMediaUtil_CancelEncodeCB(pcb);
  return nRet;
#else
   return EUNSUPPORTED;
#endif //FEATURE_JPEG_ENCODER
#endif // FEATURE_JPEG_ENCODER_REV2
}

/*===========================================================================

FUNCTION: IMediaUtil_EncodePNG

DESCRIPTION: This function fills the appropriate data structures to encode the
input into PNG format. It registers the call back and calls pnge_encode which
queues the request to the GRAPH thread for PNG encode processing.

DEPENDENCIES
None.

RETURN VALUE
SUCCESS              If the request was successfully queued
Basic Error Code     On Failure

COMMENTS

SIDE EFFECTS
If a file with the same name exists, it is removed by this function before 
encoding begins.
==============================================================================*/
static int IMediaUtil_EncodePNG(IMediaUtil * pme, 
                                AEEMediaEncodeResult * per, 
                                AEEMediaEncodeInfo * pei, 
                                AEECallback * pcb)
{
#ifdef FEATURE_MEDIA_PNG_ENCODER

    PNGE_spec_type encInfo;
    ImageEncodeSpec *pEncodeSpec = NULL; 
    PNGE_returnCodeType nEncodeResult = PNGE_SUCCESS;
    int nRet = SUCCESS;
    char filePath[FS_FILENAME_MAX_LENGTH+1];

    // Sanity check for input parameters
    if (!per || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList || 
        !pme || !(pei->pmdList[0].pData) || !(pei->pEncodeSpec) ||
        !(pei->pmdDest->pData))
        return EBADPARM;

    pEncodeSpec = (ImageEncodeSpec *) pei->pEncodeSpec;

    // Only [in] buffers and [out]file and buffer are supported.
    if (pei->pmdList[0].clsData != MMD_BUFFER  ||
        ((pei->pmdDest->clsData != MMD_FILE_NAME) &&
        (pei->pmdDest->clsData != MMD_BUFFER)))
        return EUNSUPPORTED;

    // First link new system callback to app callback and fill cancel info.
    // Then register system callback with ObjectMgr...
    if (pcb)
    {
        nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
        if (nRet)
            return nRet;
    }

    ZEROAT(&encInfo);
    if (!IMediaUtil_MapToPNGColorFormat(pEncodeSpec->wMainInColor, 
        &encInfo.input_data))
    {
        nRet = EUNSUPPORTED;
        goto Done;
    }

    // Form fully qualified path name to output file.
    if (pei->pmdDest->clsData == MMD_BUFFER)
    {
        pngOutputDescHandle.device = PNGE_DEVICE_MEM;
        pngOutputDescHandle.buffer = (uint8 *)(pei->pmdDest->pData);
        pngOutputDescHandle.length = pei->pmdDest->dwSize;
    }
    else
    {
        const char* cpszPath = (const char *) (pei->pmdDest->pData);

        pngOutputDescHandle.device = PNGE_DEVICE_EFS;

        // Determine the absolute path for the file
        nRet = IMediaUtil_GetFullMediaFileName(pme, cpszPath, filePath, 
                                               FS_FILENAME_MAX_LENGTH);
        if (nRet)
            goto Done;

        // Copy the file name to the input params for pnge_encode
        STRNCPY(encInfo.filename, filePath, FS_FILENAME_MAX_LENGTH);
    }

    //Fill in the encoding information
    encInfo.image_height       = pEncodeSpec->wMainHight;
    encInfo.image_width        = pEncodeSpec->wMainWidth;
    encInfo.input_ptr          = pei->pmdList->pData;

    // Update and save the destination information
    if (pcb)
    {
        per->clsMedia = AEECLSID_PNG;
        per->pmdDest = pei->pmdDest;
        pme->m_per = per;
    }

    // Save the handle so that it can be retrieved in the call back
    pngOutputDescHandle.appHandle = (void *)pme->m_hObjEncode;

    // Call encode_pnge which will queue the request if all the parameters 
    // are valid. DO NOT add any code between the switch statement and Done:
    nEncodeResult = pnge_encode(encInfo, &pngOutputDescHandle, 
                                IMediaUtil_EncodePNGCB);
    switch(nEncodeResult)
    {
    case PNGE_SUCCESS:
        nRet = SUCCESS;
        break;
    case PNGE_INVALID_PARM:
        nRet = EBADPARM;
        break;
    case PNGE_INPUT_FORMAT_NOT_SUPPORTED:
        nRet = EINVALIDFORMAT;
        break;
    default:
        nRet = EFAILED;
        break;
    }

Done:
    // If there is an error, cancel the registered callback
    if (nRet && pcb)
        IMediaUtil_CancelEncodeCB(pcb);
    return nRet;

#else // FEATURE_MEDIA_PNG_ENCODER
    return EUNSUPPORTED;
#endif // FEATURE_MEDIA_PNG_ENCODER
}

#ifdef FEATURE_MEDIA_PNG_ENCODER
/*===========================================================================

FUNCTION: IMediaUtil_EncodePNGCB

DESCRIPTION: This is the callback function for PNG encoder. If a buffer is 
accepted by the PNG encoder, it issues this callback to give the result of the 
encoding. To inform the user of the result, ISHELL_Resume is called which 
invokes the user callback when the next time, the event loop is called

INPUT PARAM: encodeStatus: result of the attempt to encode to PNG format
      handle: ptr to pnge_handle_type which contains the output
      pnge_data: NULL for now - not used

DEPENDENCIES
None.

RETURN VALUE
None

COMMENTS

SIDE EFFECTS
NONE
==============================================================================*/
void IMediaUtil_EncodePNGCB(PNGE_status_enum_type encodeStatus, 
                            void *handle, 
                            void *pnge_data)
{
    OEMMM_ENTER_CRITICAL_SECTION()

    pnge_handle_type *pnge_handle = NULL;
    IMediaUtil* pme = NULL;
    AEECallback* pcb = NULL;

    pnge_handle = (pnge_handle_type *) handle;
    if (!pnge_handle)
        goto CBExit;

    // If  there is no user registered callback, exit
    pcb = (AEECallback *)AEEObjectMgr_GetObject(
                            (AEEObjectHandle)pnge_handle->appHandle);
    if (!pcb)
        goto CBExit;

    // Retrieve the IMediaUtil struct
    pme = (IMediaUtil *)pcb->pCancelData;
    if (!pme)
        goto CBExit;

    pme->m_per->nResult = (encodeStatus == PNGE_CB_DONE) ? SUCCESS : EFAILED;
    pme->m_per->pmdDest->clsData = AEECLSID_PNG;

    // If the output is a file, the file name is already saved in
    // pme->m_per->pmdDest->pData
    // If the output is a buffer, pnge_handle->buffer is the same as
    // pme->m_per->pmdDest->pData
    if (pnge_handle->device == PNGE_DEVICE_MEM)
        pme->m_per->pmdDest->dwSize = pnge_handle->length;
    else
    {
        pme->m_per->pmdDest->dwSize = 0;
    }

    pme->m_per->pEncodeSpec = NULL;
    pme->m_per->dwSpecSize = 0;

    if (encodeStatus != PNGE_CB_DONE)
    {
        pme->m_per->clsMedia = 0;
    }

    // Register the callback with the shell. The Shell invokes the callback
    // function the next time the event loop is called
    ISHELL_Resume(pme->m_pIShell, pcb);

    // Operation is complete. So, reset the saved parameters in IMediaUtil struct
    IMediaUtil_ResetEncode(pme);

CBExit:
    // Reset the pnge_handle variables
    pnge_handle->appHandle = NULL;
    pnge_handle->buffer = NULL;

    OEMMM_LEAVE_CRITICAL_SECTION()
}

/*===========================================================================

FUNCTION: IMediaUtil_MapToPNGColorFormat

DESCRIPTION: This function converts the input color which is in IPL color format
to a color supported by PNG as defined in PNGE_rawDataType enum. If the color 
is not supported by PNG encoder, it returns FALSE

DEPENDENCIES
None.

RETURN VALUE
TRUE        If the color format is supported by PNG
FALSE       Otherwise

COMMENTS

SIDE EFFECTS
outColor has the color as defined by PNGE_rawDataType enum
None
==============================================================================*/
static boolean IMediaUtil_MapToPNGColorFormat(uint16 inColor, 
                                              PNGE_rawDataType* outColor)
{
    boolean bMap = TRUE;
    switch (inColor)
    {
    case IPL_YCbCr:
        *outColor = YCbCr422;
        break;
    case IPL_RGB565:
        *outColor = RGB565;
        break;
    case IPL_RGB666:
        *outColor = RGB666;
        break;
    case IPL_RGB444:
        *outColor = RGB444;
        break;
    case IPL_BAYER_BGGR:
        *outColor = BAYER_BGGR;
        break;
    case IPL_BAYER_GBRG:
        *outColor = BAYER_GBRG;
        break;
    case IPL_BAYER_GRBG:
        *outColor = BAYER_GRBG;
        break;
    case IPL_BAYER_RGGB:
        *outColor = BAYER_RGGB;
        break;
    case IPL_RGB888:
        *outColor = RGB888;
        break;

    default:
        bMap = FALSE;
        break;
    }
    return bMap;
}

#endif //FEATURE_MEDIA_PNG_ENCODER

#else // CMX 2.x

/*=========================================================================

========================================================================= */
static int IMediaUtil_EncodeMedia(IMediaUtil * pme, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
   return EUNSUPPORTED;
}

#endif // defined(FEATURE_BREW_CMX_V3) 

#endif // defined(FEATURE_BREW_MULTIMEDIA)

/* ============= End of File =============================================== */

