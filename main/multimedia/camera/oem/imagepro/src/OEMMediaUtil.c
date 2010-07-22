/* =========================================================================

FILE: AEEMediaUtil.c

SERVICES: IMedia utility functions

DESCRIPTION
  This file contains IMedia related utility functions that apps can use
  to develop BREW multimedia applications.

PUBLIC CLASSES:  
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A


           Copyright ?2000-2006 QUALCOMM Incorporated.
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
========================================================================= */
/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/oem/imagepro/rel/3.1.5/latest/src/OEMMediaUtil.c#7 $
 $DateTime: 2009/02/13 04:01:30 $ $Author: kgelli $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 01/16/09  KC     Fixed compiler warnings for ULC low tier with appropriate featurisations
 10/14/08  KC     Removed deprecated APIs STRCPY and STRNCPY and replaced them 
                  with STRLCPY
08/26/08   KC     Added a new boolean variable JpegBusyStatus to check if already
                  a jpeg session is in progress or not. If yes, then do not allow for a second encode                   
 02/07/08  KC    Added featurization for ULC low tier build. 
 11/02/07 KG     Abort Encoding when clear key is pressed in Qalbum save.
 09/05/06  vma    Assign chroma buffer pointer in PNG encoding in order to cope
                  with addition of separate chroma pointer in PNG encode spec
 08/31/06  NC     Cancel callback on failure.
 07/07/06  NC     Merge change from brew_3.1.4/pk/src/msm/OEMMediaUtil.c#12
 03/28/06  NC     Merge change from brew_3.1.4/pk/src/msm/OEMMediaUtil.c#9

===============================================================================*/

/*=========================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
========================================================================= */
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_MULTIMEDIA)

#include "cmx.h"
#include "ipl.h"

#ifdef FEATURE_JPEG_ENCODER_REV2
#undef FEATURE_JPEG_ENCODER_REV2
#endif

#ifdef FEATURE_JPEG_ENCODER_REV2
#include "jpege.h"
#else
#ifdef FEATURE_JPEG_ENCODER
#include "jpeg.h"
#endif //FEATURE_JPEG_ENCODER
#endif //FEATURE_JPEG_ENCODER_REV2

#ifdef FEATURE_PNG_ENCODER
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
#include "AEEBitmap.h"
#include "IYCbCr.h"
#ifdef FEATURE_ACM
#ifndef FEATURE_ODM_UI
#include "OEMACM.h"
#endif
#endif // FEATURE_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

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

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

struct IMediaUtil
{
   const AEEVTBL(IMediaUtil) *pvt;
   int32                m_nRefs;
   IShell *             m_pIShell;

   // EncodeMedia() info
   AEEObjectHandle      m_hObjEncode;
   AEEMediaEncodeResult *  m_per;
   AEECallback          *m_pcbSys;

#ifdef FEATURE_JPEG_ENCODER_REV2
   //variables to hold the converted image data 
   ipl_image_type m_imgMain;
   ipl_image_type m_imgThumb;
#endif //FEATURE_JPEG_ENCODER_REV2

#ifdef FEATURE_ACM
   AEEMediaEncodeInfo        *m_pei;
   ACM_Callback_data_type     m_cbACMdata;
   ACM_acquire_reason_code    m_reasonCode;
   int                        m_resState;
#endif // FEATURE_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM
#if defined(FEATURE_PNG_ENCODER) && (defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM))
   pnge_handle_type           pngOutputDescHandle;
#endif // FEATURE_PNG_ENCODER && (FEATURE_ACM || FEATURE_BMP_ACM)
};

extern int     IMediaUtil_New(IShell * ps, AEECLSID cls, void **ppif);
//static void    IMediaUtil_Delete(IMediaUtil * pme);
static uint32  IMediaUtil_AddRef(IMediaUtil * pme);
static uint32  IMediaUtil_Release(IMediaUtil * pme);
static int     IMediaUtil_QueryInterface(IMediaUtil * pme, AEECLSID idReq, void ** ppo);
static int     IMediaUtil_CreateMedia(IMediaUtil * pme, AEEMediaData * pmd, IMedia ** ppm);
static int     IMediaUtil_EncodeMedia(IMediaUtil * pme, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb);
static void    IMediaUtil_ResetEncode(IMediaUtil * pme);
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static void    IMediaUtil_CancelEncodeCB(IMediaUtil * pme);
static int     IMediaUtil_RegisterEncodeCB(IMediaUtil * pme, AEECallback * pcb);
static int     IMediaUtil_GetFullMediaFileName(IMediaUtil * pme, const char *inName, char *outName, int size);
#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
static void    IMediaUtil_CancelEncoding(IMediaUtil *pme);
#endif //!FEATURE_ACM && !FEATURE_BMP_ACM
#endif //FEATURE_PMD_ENCODE || FEATURE_JPEG_ENCODER_REV2 || 
       //FEATURE_JPEG_ENCODER || FEATURE_MEDIA_PNG_ENCODER

static const AEEVTBL(IMediaUtil) gvtIMediaUtil =
{
   IMediaUtil_AddRef,
   IMediaUtil_Release,
   IMediaUtil_QueryInterface,
   IMediaUtil_CreateMedia,
   IMediaUtil_EncodeMedia
};

#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
static IMediaUtil gsMediaUtil = { 0 };
#endif // !FEATURE_ACM && !FEATURE_BMP_ACM

#if (defined(FEATURE_JPEG_ENCODER_REV2) || defined (FEATURE_JPEG_ENCODER) || defined (FEATURE_PMD_ENCODE) || defined (FEATURE_PNG_ENCODER) )
static OEMCriticalSection gMediaUtilCriticalSection;
#endif

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
static int  IMediaUtil_ACMAcquire           (IMediaUtil * pme);
static void IMediaUtil_ACMFree              (IMediaUtil* pme);
static int  IMediaUtil_SaveEncodeInfo       (IMediaUtil * pme, AEEMediaEncodeInfo * pei);
static void IMediaUtil_FreeSavedEncodeInfo  (IMediaUtil * pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM

#if defined(FEATURE_ACM)
static void JPEGEncode_ACMStatusChange      (void* p);
static void PNGEncode_ACMStatusChange       (void* p);
#endif // FEATURE_ACM

#if (defined(FEATURE_JPEG_ENCODER_REV2) || defined (FEATURE_JPEG_ENCODER))
static int  JPEG_ProceedWithEncode (IMediaUtil*           pme, 
                                    AEEMediaEncodeResult* per, 
                                    AEEMediaEncodeInfo*   pei);
#endif

#ifdef FEATURE_PNG_ENCODER
static int PNG_ProceedWithEncode (IMediaUtil*             pme, 
                                  AEEMediaEncodeResult*   per, 
                                  AEEMediaEncodeInfo*     pei);
#endif

/* our underlying jpeg layer does not support multiple jpeg encoding instances */
#ifdef FEATURE_JPEG_ENCODER_REV2
static boolean JpegBusyStatus = FALSE; 
#endif

/*=========================================================================

                     IMediaUtil FUNCTION DEFINITIONS

=========================================================================*/

/*==================================================================

==================================================================*/
int IMediaUtil_New(IShell * ps, AEECLSID cls, void **ppif)
{
#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
   IMediaUtil *   pme = &gsMediaUtil;
#else // FEATURE_ACM || FEATURE_BMP_ACM
   IMediaUtil *   pme = MALLOC (sizeof (IMediaUtil));
   if (pme == NULL)
   {
      return ENOMEMORY;
   }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   if (!pme->m_nRefs)
   {
      pme->pvt = &gvtIMediaUtil;
      pme->m_pIShell = ps;
   }
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
   pme->m_nRefs++;
   pme->m_resState = ACM_RESOURCE_UNKNOWN;
#endif // FEATURE_ACM || FEATURE_BMP_ACM
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM

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
  if (!pme)
    return 0;

  return ++pme->m_nRefs;
}

/*==================================================================

==================================================================*/
static uint32 IMediaUtil_Release(IMediaUtil * pme)
{
  if (!pme)
    return 0;

  if(pme->m_nRefs > 0) {
    --pme->m_nRefs;
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
#ifndef FEATURE_ODM_UI
    if (pme->m_nRefs == 0) {
      if (pme->m_resState == ACM_RESOURCE_ACQUIRED) {
        // user should have cancelled the callback before calling
        // IMediaUitl_Release, 
        if (pme->m_reasonCode == ACM_EDIT_JPEG_QDSP) {
#ifdef FEATURE_JPEG_ENCODER_REV2
          jpeg_encoder_abort ((uint16) pme->m_hObjEncode);
#else 
#ifdef FEATURE_JPEG_ENCODER
          jpeg_abort_encode ();
#endif // FEATURE_JPEG_ENCODER
#endif // FEATURE_JPEG_ENCODER_REV2
        }
        else if (pme->m_reasonCode == ACM_EDIT_PNG_ARM) {
#ifdef FEATURE_PNG_ENCODER
          pnge_encode_abort ();
#endif // FEATURE_PNG_ENCODER
        }
      }
      else if (pme->m_resState == ACM_RESOURCE_ACQUIRING) {
        IMediaUtil_FreeSavedEncodeInfo (pme);
      }
      IMediaUtil_CancelEncodeCB (pme);
      IMediaUtil_ACMFree (pme);

#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

      FREE (pme);
      return 0;
    }
#endif // !FEATURE_ODM_UI
#else // FEATURE_ACM || FEATURE_BMP_ACM
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
      || defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)

    IMediaUtil_CancelEncoding(pme);
#endif
#endif // FEATURE_ACM || FEATURE_BMP_ACM
  }
  return(uint32)(pme->m_nRefs);
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
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 
   return ECLASSNOTSUPPORT;
}

/*=========================================================================

========================================================================= */
static int IMediaUtil_CreateMedia(IMediaUtil * pme, AEEMediaData * pmd, IMedia ** ppm)
{
#if defined(FEATURE_BMP_ACM)
#error code not present
#else 

   return AEEMediaUtil_CreateMedia(pme->m_pIShell, pmd, ppm);

#endif 
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

#ifdef FEATURE_PNG_ENCODER
#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
LOCAL pnge_handle_type pngOutputDescHandle;
#endif // !FEATURE_ACM && !FEATURE_BMP_ACM
static void IMediaUtil_EncodePNGCB(PNGE_status_enum_type command, void *handle,
                                   void *pnge_data);
static boolean IMediaUtil_MapToPNGColorFormat(uint16 inColor, 
                                              PNGE_rawDataType* outColor);
#endif //FEATURE_PNG_ENCODER

/*=========================================================================

========================================================================= */
static int IMediaUtil_EncodeMedia(IMediaUtil * pme, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
#if defined(FEATURE_BMP_ACM)
#error code not present
#endif 

   if (!per || !clsDest || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList)
      return EBADPARM;

   // First cancel pending callback, if any.
   if (pcb)
      CALLBACK_Cancel(pcb);

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  if (pme->m_pcbSys)
  {
    return EFAILED;
  }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

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

#if defined(FEATURE_ACM) && defined(FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   //
   // Here you can verify if one media is JPEG and another is QCP...
   // Currently, assumes: first media is JPEG and second is PMD!
   //

   // First link new system callback to app callback and fill cancel info.
   // Then register system callback with ObjectMgr...
   if (pcb)
   {
      nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
      if (nRet != SUCCESS)
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
      if (nRet != SUCCESS)
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

   if (nRet && pcb)
      IMediaUtil_CancelEncodeCB(pme);

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

   IMediaUtil* pme = (IMediaUtil *)AEEObjectMgr_GetObject((AEEObjectHandle)client_data);

   if (pme == NULL || pme->m_pcbSys == NULL)
      goto CBExit;

   pme->m_per->nResult = (status == CMX_SUCCESS) ? SUCCESS : EFAILED;
   pme->m_per->pEncodeSpec = NULL;
   pme->m_per->dwSpecSize = 0;

   if (status != CMX_SUCCESS)
   {
      pme->m_per->clsMedia = 0;
      pme->m_per->pmdDest = NULL;
   }

   ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);

   IMediaUtil_ResetEncode(pme);

CBExit:
   OEMMM_LEAVE_CRITICAL_SECTION()
}
#endif // FEATURE_PMD_ENCODE

/*=========================================================================

========================================================================= */
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static void IMediaUtil_CancelEncodeCB(IMediaUtil * pme)
{
   if (pme == NULL || pme->m_pcbSys == NULL)
      return;

   // unlink the system callback
   CALLBACK_Cancel (pme->m_pcbSys);

   // deregister with the AEE object mgr and set m_pcbSys to NULL
   IMediaUtil_ResetEncode(pme);
}
#endif //#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) 
	 // || defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)


#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static void IMediaUtil_CancelEncoding(IMediaUtil *pme)
{
   if (!pme)
     return;

   if (pme->m_per)
   {
     //Cancel encoding processing
     if (pme->m_per->clsMedia == AEECLSID_JPEG)
     {
#ifdef FEATURE_JPEG_ENCODER_REV2
       jpeg_encoder_abort ((uint16) pme->m_hObjEncode);
#else 
#ifdef FEATURE_JPEG_ENCODER
       jpeg_abort_encode ();
#endif // FEATURE_JPEG_ENCODER
#endif // FEATURE_JPEG_ENCODER_REV2
     }
     else if (pme->m_per->clsMedia == AEECLSID_PNG)
     {
#ifdef FEATURE_PNG_ENCODER
         pnge_encode_abort ();
#endif // FEATURE_PNG_ENCODER
     }
       
    //Reset callback variables. Its buffer could be freed by BREW lib on release. 
     pme->m_per = NULL;
     pme->m_hObjEncode = 0;
     pme->m_pcbSys = NULL;
   }
}
#endif //#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) 
	 // || defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
#endif //#if !defined(FEATURE_ACM) && !defined(FEATURE_BMP_ACM)

/*=========================================================================

========================================================================= */
static void IMediaUtil_ResetEncode(IMediaUtil * pme)
{
   if (!pme)
      return;

   pme->m_per = NULL;

   if (pme->m_pcbSys) // Should not hurt to add this check
   {
      AEEObjectMgr_Unregister(pme->m_hObjEncode);
      pme->m_hObjEncode = 0;
      pme->m_pcbSys = NULL;
   }
}

/*=========================================================================

========================================================================= */
#if defined(FEATURE_PMD_ENCODE) || defined(FEATURE_JPEG_ENCODER_REV2) \
	|| defined(FEATURE_JPEG_ENCODER) || defined(FEATURE_MEDIA_PNG_ENCODER)
static int     IMediaUtil_RegisterEncodeCB(IMediaUtil * pme, AEECallback * pcb)
{
  AEECallback * pcbSys;
  int           nRet = SUCCESS;

  if (!pme || !pcb)
    return EBADPARM;

  if (pme->m_pcbSys != NULL)
    return EFAILED;

  pcbSys = AEE_LinkSysCallback(pcb);

  if (!pcbSys)
     return ENOMEMORY;

  // We shall never attempt to overwrite the pfnCancel and pCancelData function
  // since BREW has alreday filled up those two fields properly
  pme->m_pcbSys = pcbSys;

  nRet = AEEObjectMgr_Register(pme, &pme->m_hObjEncode);
  if (nRet != SUCCESS)
  {
    // unlink the system callback
    CALLBACK_Cancel (pme->m_pcbSys);
    pme->m_pcbSys = NULL;
  }

  return nRet;
}

/*=========================================================================

========================================================================= */
static int IMediaUtil_GetFullMediaFileName(IMediaUtil * pme, const char *inName, char *outName, int size)
{
  int   nRet = SUCCESS;
  int   nFullPathLen;
  char *pszFullPath;

  if (!pme || !inName || !outName)
    return EBADPARM;

  nRet = AEE_ResolvePath(inName, 0, &nFullPathLen);

  if (nRet) 
    return nRet;
         
  pszFullPath = (char *)MALLOC(nFullPathLen);

  if (!pszFullPath) 
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

  FREE(pszFullPath);

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

  IMediaUtil  *  pme = NULL;

  MSG_HIGH ("JPEG callback", 0, 0, 0);

  JpegBusyStatus = FALSE;

  if (!rtnData)
    goto CBExit;

  //Release the buffers that are allocated in IMediaUtil_ConvertJPEGData
  if(rtnData && (rtnData->encodeSpec->Main.Fragmnt[0].Luma_ptr))
  {
    OEM_Free((JPEGENC_CBrtnType *)rtnData->encodeSpec->Main.Fragmnt[0].Luma_ptr); 
    rtnData->encodeSpec->Main.Fragmnt[0].Luma_ptr = NULL;
  }
  if(rtnData && (rtnData->encodeSpec->Thumbnail.Data.Luma_ptr))
  {
    OEM_Free((JPEGENC_CBrtnType *)rtnData->encodeSpec->Thumbnail.Data.Luma_ptr); 
    rtnData->encodeSpec->Thumbnail.Data.Luma_ptr=NULL;
  }
  
  pme = (IMediaUtil *)AEEObjectMgr_GetObject((AEEObjectHandle)rtnData->clientId);
  if (pme == NULL || pme->m_pcbSys == NULL)
    goto CBExit;

    pme->m_per->nResult = (rtnData->status == JPEGENC_IMG_DONE) ? SUCCESS : EFAILED;
    pme->m_per->pEncodeSpec = NULL;
    pme->m_per->dwSpecSize = 0;

  if (pme->m_per->nResult != SUCCESS)
  {
    pme->m_per->clsMedia = 0;
    pme->m_per->pmdDest = NULL;
  }

  ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);

  IMediaUtil_ResetEncode(pme);

CBExit:

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  IMediaUtil_ACMFree (pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM

   pme->m_imgMain.imgPtr = NULL;
   pme->m_imgThumb.imgPtr = NULL;

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

  if (!pme || !pData || !encInfo)
    return EBADPARM;

  ZEROAT(&inData);

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

  switch (inColor)
  {
  case IYCBCR_COLORSCHEME_420LP:
    inData.cFormat = IPL_YCbCr420_LINE_PK;
    inData.imgPtr = pData;
    inData.clrPtr = pData + inData.dx * inData.dy;
    break;
  case IYCBCR_COLORSCHEME_422LP:
    inData.cFormat = IPL_YCbCr422_LINE_PK;
    inData.imgPtr = pData;
    inData.clrPtr = pData + inData.dx * inData.dy;
    break;
  case IDIB_COLORSCHEME_565:
    inData.cFormat = IPL_RGB565;
    inData.imgPtr = pData;
    break;
  case IDIB_COLORSCHEME_666:
    inData.cFormat = IPL_RGB666;
    inData.imgPtr = pData;
    break;
  case IDIB_COLORSCHEME_888:
    inData.cFormat = IPL_RGB888;
    inData.imgPtr = pData;
    break;
  default:
    return EUNSUPPORTED;
  }

  switch (fmtType)
  {
  case JPEGENC_H2V2: //convert destination color format to YCbCr420lp
    if (pOutData->dx & 1)
    {
      pOutData->dx++;   // ycbcr420lp must have even width
    }
    if (pOutData->dy & 1)
    {
      pOutData->dy++;   // ycbcr420lp must have even height
    }
    imageBytes = (pOutData->dx * pOutData->dy * 3) / 2;
    pOutData->cFormat = IPL_YCrCb420_LINE_PK;
    break;
  case JPEGENC_H2V1: //convert destination color format to YCbCr422lp
    if (pOutData->dx & 1)
    {
      pOutData->dx++;   // ycbcr422lp must have even width
    }
    imageBytes = pOutData->dx * pOutData->dy * 2;
    pOutData->cFormat = IPL_YCrCb422_LINE_PK;
    break;
  default:
    return EUNSUPPORTED;
  }

  //allocate buffer to do color conversion
  FREEIF(pOutData->imgPtr);
  pOutData->imgPtr  =(unsigned char *)OEM_Malloc (imageBytes);
  if (!pOutData->imgPtr)
    return ENOMEMORY;

  pOutData->clrPtr = pOutData->imgPtr + (pOutData->dx * pOutData->dy);
#ifdef CUST_EDITION  //remove by miaoxiaoming
  //ret = ipl_convert_image(&inData, pOutData);
  ret = IPL_FAILURE;
#endif

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
#else // FEATURE_JPEG_ENCODER_REV2
#ifdef FEATURE_JPEG_ENCODER
/*=========================================================================

========================================================================= */
static void IMediaUtil_EncodeJPEGCB(jpege_status_enum_type status,
                                    void *handle, void *server_data)
{
  OEMMM_ENTER_CRITICAL_SECTION()

  AEECallback *  pcb;
  IMediaUtil  *  pme;

  if (!handle)
    goto CBExit;

  pme = (IMediaUtil  *)AEEObjectMgr_GetObject((AEEObjectHandle)handle);

  if (pme == NULL || pme->m_pcbSys == NULL)
    goto CBExit;

  pme->m_per->nResult = (status == JPEGE_CB_DONE) ? SUCCESS : EFAILED;
  pme->m_per->pmdDest->dwSize = *((uint32 *)server_data); //num bytes of output in MEM mode or zero.
  pme->m_per->pEncodeSpec = NULL;
  pme->m_per->dwSpecSize = 0;

  if (status != JPEGE_CB_DONE)
  {
    pme->m_per->clsMedia = 0;
    pme->m_per->pmdDest = NULL;
  }

  ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);
  IMediaUtil_ResetEncode(pme);

CBExit:

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  IMediaUtil_ACMFree (pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM
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

#if (defined(FEATURE_JPEG_ENCODER_REV2) || defined (FEATURE_JPEG_ENCODER))
/*=====================================================================
FUNCTION JPEG_ProceedWithEncode

  DESCRIPTION:
    This function calls the proper jpeg encoding function.

  RETURN VALUE:
    error code defined in AEEError.h
======================================================================*/
static int JPEG_ProceedWithEncode (IMediaUtil * pme, 
                                   AEEMediaEncodeResult * per, 
                                   AEEMediaEncodeInfo * pei)
{
#ifdef FEATURE_JPEG_ENCODER_REV2 //This section uses JPEG encoder Rev. 2

  JPEGENC_encodeSpecType *encInfo;
  const char *cpszPath;
  ImageEncodeSpec *pEncodeSpec;
  int nRet = SUCCESS;

  if(JpegBusyStatus)
  {
    return EUNSUPPORTED; //some jpeg encoding is already in progress
  }
  encInfo = MALLOC(sizeof(JPEGENC_encodeSpecType));
  if (!encInfo)
  {
    nRet = ENOMEMORY;
    goto Done;
  }

  ZEROAT(encInfo);

  // Form fully qualified path name to output file.
  cpszPath = (const char *) (pei->pmdDest->pData);
  nRet = IMediaUtil_GetFullMediaFileName(pme, cpszPath, 
                                         encInfo->Dest.handle.efs.filename, 
                                         sizeof(encInfo->Dest.handle.efs.filename));
  if (nRet != SUCCESS)
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
  encInfo->Main.Height = encInfo->Main.OrigHeight = pEncodeSpec->wMainHigh;
  encInfo->Main.Restart = 10;
  encInfo->Main.Subsample = JPEGENC_H2V2;
  encInfo->Main.FragCnt = 1;
  #ifndef FEATURE_CAMERA7500
    encInfo->Main.Fragmnt[0].Height = pEncodeSpec->wMainHigh;
    encInfo->Main.Fragmnt[0].Reserved = 0;
  #endif//FEATURE_CAMERA7500
  //fill the main image date
  if (pEncodeSpec->wMainInColor == IYCRCB_COLORSCHEME_420LP)
  {
    //Main image encode color format match JPEGENC_H2V2 requirement.
    //No color format convertion needed.
    encInfo->Main.Fragmnt[0].Luma_ptr = pei->pmdList[0].pData;
    encInfo->Main.Fragmnt[0].Chroma_ptr = encInfo->Main.Fragmnt[0].Luma_ptr + 
                             (pEncodeSpec->wMainWidth * pEncodeSpec->wMainHigh);
  }
  else
  {
    nRet = IMediaUtil_ConvertJPEGData(pme, pEncodeSpec->wMainInColor,
                                      pei->pmdList[0].pData, encInfo, TRUE);
    if (nRet != SUCCESS)
      goto Done;
  }
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
    #ifndef FEATURE_CAMERA7500
      encInfo->Thumbnail.Data.Height = pEncodeSpec->wThumbHigh;
      encInfo->Thumbnail.Data.Reserved = 0;
    #endif//FEATURE_CAMERA7500
    //fill the thumbnail data.
    if (pEncodeSpec->wMainInColor == IYCRCB_COLORSCHEME_422LP)
    {
      //Thumbnail image encode color format match JPEGENC_H2V1 requirement.
      //No color format convertion needed.
      encInfo->Thumbnail.Data.Luma_ptr = pei->pmdList[1].pData;
      encInfo->Thumbnail.Data.Chroma_ptr = encInfo->Thumbnail.Data.Luma_ptr + 
                         (pEncodeSpec->wThumbWidth * pEncodeSpec->wThumbHigh);
    }
    else
    {
      nRet = IMediaUtil_ConvertJPEGData(pme, pEncodeSpec->wThumbInColor,
                                        pei->pmdList[1].pData, encInfo, FALSE);
      if (nRet != SUCCESS)
        goto Done;
    }
  }
  JpegBusyStatus = TRUE; // Jpeg encoding started, Will set this flag to FALSE in Callback

  if (pEncodeSpec->pHeaderBuf)
    nRet = (int)jpege_encode_exif(encInfo, (exif_info_type *)pEncodeSpec->pHeaderBuf);
  else
    nRet = (int)jpege_encode_jfxx(encInfo);

  if (nRet != JPEGENC_SUCCESS)
  {
    MSG_HIGH ("jpeg encode failed, status = %d", nRet, 0, 0);
    nRet = EBADPARM;
  }
  else
  {
    nRet = SUCCESS;
  }

Done:
  if (nRet)
  {
    //Free data buffer in failure if it is allocated.
    FREEIF(pme->m_imgMain.imgPtr);
    pme->m_imgMain.imgPtr = NULL;
    FREEIF(pme->m_imgThumb.imgPtr);
    pme->m_imgThumb.imgPtr = NULL;
  }
  FREEIF(encInfo);
  return nRet;

#else
#ifdef FEATURE_JPEG_ENCODER //This section uses old JPEG encoder 
  int nRet = SUCCESS;
  jpege_encode_type   encInfo;
  ImageEncodeSpec *pEncodeSpec = (ImageEncodeSpec *)pei->pEncodeSpec;

  ZEROAT(&encInfo);
  if (!IMediaUtil_MapColorFormat(pEncodeSpec->wMainInColor, &encInfo.format))
  {
    nRet = EUNSUPPORTED;
    goto Done;
  }

  encInfo.dx = pEncodeSpec->wMainWidth;
  encInfo.dy = pEncodeSpec->wMainHigh;
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
    if (nRet != SUCCESS)
      goto Done;
  }

  if (pEncodeSpec->bQuality)
    encInfo.quality = pEncodeSpec->size.nQuality;
  else
    encInfo.max_file_size = pEncodeSpec->size.nMaxFileSize;

  nRet = jpege_encode_fn(encInfo, (void *) pme->m_hObjEncode, IMediaUtil_EncodeJPEGCB);
  if (nRet != JPEGE_SUCCESS)
  {
    MSG_HIGH ("jpeg encode failed, status = %d", nRet, 0, 0);
    nRet = EBADPARM;
  }
  else
  {
    nRet = SUCCESS;
  }

Done:
  return nRet;
#endif //FEATURE_JPEG_ENCODER
#endif // FEATURE_JPEG_ENCODER_REV2
}
#endif
/*=====================================================================
FUNCTION IMediaUtil_EncodeJPEG

  DESCRIPTION:
    This function fills the appropriate data structures and calls the JPEG encoder 
    to encode the image data into JPEG format.

  PARAMETERS:
    pme - Pointer to the IMediaUtil struct.
    per - Pointer to AEEMediaEncodeResult struct defined in AEEMediaUtil.h
    pei - Pointer to AEEMediaEncodeInfo struct defined in AEEMediaUtil.h
    pcb - Pointer to AEECallback struct defined in AEE.h

  RETURN VALUE:
    error code defined in AEEError.h
======================================================================*/
static int IMediaUtil_EncodeJPEG(IMediaUtil * pme, AEEMediaEncodeResult * per, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
  int nRet = EUNSUPPORTED;

#if (defined(FEATURE_JPEG_ENCODER_REV2) || defined (FEATURE_JPEG_ENCODER))
  if (!per || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList)
    return EBADPARM;

#if defined(FEATURE_JPEG_ENCODER_REV2)
  // Only [in] buffers and [out]file are supported.
  if (pei->pmdList[0].clsData != MMD_BUFFER || pei->pmdDest->clsData != MMD_FILE_NAME)
    return EUNSUPPORTED;
#endif // FEATURE_JPEG_ENCODER_REV2

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  // Are we encoding something else now (audio postcard, or jpeg/png)
  if (pme->m_pcbSys != NULL || pme->m_resState != ACM_RESOURCE_UNKNOWN)
  {
    return EFAILED;
  }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

  // First link new system callback to app callback and fill cancel info.
  // Then register system callback with ObjectMgr...
  if (pcb)
  {
    nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
    if (nRet)
    {
      return nRet;
    }
    else
    {
    per->clsMedia = AEECLSID_JPEG;
    per->pmdDest = pei->pmdDest;
    pme->m_per = per;
  }
  }
  
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
#ifndef FEATURE_ODM_UI
   pme->m_reasonCode = ACM_EDIT_JPEG_QDSP;
   nRet = IMediaUtil_ACMAcquire (pme);
   // resource acquiring failed 
   if (nRet != SUCCESS)
  {
     IMediaUtil_CancelEncodeCB(pme);
     return EFAILED;
   }
   // delayed acquiring
   else if (pme->m_resState == ACM_RESOURCE_ACQUIRING)
   {
     // save the pei information
     nRet = IMediaUtil_SaveEncodeInfo (pme, pei);
     if (nRet != SUCCESS)
     {
       IMediaUtil_CancelEncodeCB(pme);
       IMediaUtil_ACMFree (pme);
     }
     return nRet;
  }

   // resource acquired, fall through
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM || FEATURE_BMP_ACM
  
  nRet = JPEG_ProceedWithEncode (pme, per, pei);

  if (nRet)
  {
    IMediaUtil_CancelEncodeCB(pme);
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    IMediaUtil_ACMFree (pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM
  }
#endif // FEATURE_JPEG_ENCODER_REV2 || FEATURE_JPEG_ENCODER

  return nRet;
}

#ifdef FEATURE_PNG_ENCODER
/*=========================================================================

FUNCTION: PNG_ProceedWithEncode

DESCRIPTION: This function fills the appropriate data structures to encode the
input into PNG format. It registers the call back and calls pnge_encode which
queues the request to the GRAPH thread for PNG encode processing.

DEPENDENCIES
None

RETURN VALUE
SUCCESS              If the request was successfully queued
Basic Error Code     On Failure

COMMENTS

SIDE EFFECTS
If a file with the same name exists, it is removed by this function before 
encoding begins.
========================================================================= */
static int PNG_ProceedWithEncode (IMediaUtil * pme, 
                                AEEMediaEncodeResult * per, 
                                  AEEMediaEncodeInfo * pei)
{
    PNGE_spec_type encInfo;
    ImageEncodeSpec *pEncodeSpec = NULL; 
    PNGE_returnCodeType nEncodeResult = PNGE_SUCCESS;
    int nRet = SUCCESS;
    char filePath[FS_FILENAME_MAX_LENGTH+1];
  pnge_handle_type *pPngOutputDescHandle = NULL; 

    pEncodeSpec = (ImageEncodeSpec *) pei->pEncodeSpec;

    ZEROAT(&encInfo);
    if (!IMediaUtil_MapToPNGColorFormat(pEncodeSpec->wMainInColor, 
        &encInfo.input_data))
    {
        nRet = EUNSUPPORTED;
        goto Done;
    }
#if defined(FEATURE_ACM) | defined(FEATURE_BMP_ACM)
  pPngOutputDescHandle = &(pme->pngOutputDescHandle);
#else
  pPngOutputDescHandle = &pngOutputDescHandle;
#endif // FEATURE_ACM || FEATURE_BMP_ACM

    // Form fully qualified path name to output file.
    if (pei->pmdDest->clsData == MMD_BUFFER)
    {
    pPngOutputDescHandle->device = PNGE_DEVICE_MEM;
    pPngOutputDescHandle->buffer = (uint8 *)(pei->pmdDest->pData);
    pPngOutputDescHandle->length = pei->pmdDest->dwSize;
    }
    else
    {
        const char* cpszPath = (const char *) (pei->pmdDest->pData);

    pPngOutputDescHandle->device = PNGE_DEVICE_EFS;

        // Determine the absolute path for the file
        nRet = IMediaUtil_GetFullMediaFileName(pme, cpszPath, filePath, 
                                               FS_FILENAME_MAX_LENGTH);
        if (nRet != SUCCESS)
            goto Done;

        // Copy the file name to the input params for pnge_encode
        STRLCPY(encInfo.filename, filePath, FS_FILENAME_MAX_LENGTH);
    }

    //luma and chroma buffer is back to back in case of linepacked ycbcr data
    encInfo.chroma_ptr         = (uint8 *)(pei->pmdList->pData) + encInfo.image_height * encInfo.image_width;

    //Fill in the encoding information
    encInfo.image_height       = pEncodeSpec->wMainHigh;
    encInfo.image_width        = pEncodeSpec->wMainWidth;
    encInfo.input_ptr          = pei->pmdList->pData;

    // Save the handle so that it can be retrieved in the call back
    pPngOutputDescHandle->appHandle = (void *)pme->m_hObjEncode;

    // Call encode_pnge which will queue the request if all the parameters 
    // are valid. DO NOT add any code between the switch statement and Done:
    nEncodeResult = pnge_encode(encInfo, pPngOutputDescHandle, 
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
    return nRet;
}
#endif // FEATURE_PNG_ENCODER

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
  int nRet = EUNSUPPORTED;

#ifdef FEATURE_PNG_ENCODER
  // Sanity check for input parameters
  if (!per || !pei || !pei->pmdDest || !pei->nCount || !pei->pmdList || 
      !pme || !(pei->pmdList[0].pData) || !(pei->pEncodeSpec) ||
      !(pei->pmdDest->pData))
  {
    return EBADPARM;
  }

  // Only [in] buffers and [out]file and buffer are supported.
  if (pei->pmdList[0].clsData != MMD_BUFFER  ||
    ((pei->pmdDest->clsData != MMD_FILE_NAME) &&
     (pei->pmdDest->clsData != MMD_BUFFER)))
  {
    return EUNSUPPORTED;
  }

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
  // Are we encoding something else?
  if (pme->m_pcbSys != NULL || pme->m_resState != ACM_RESOURCE_UNKNOWN)
  {
    return EFAILED;
  }
#endif // FEATURE_ACM || FEATURE_BMP_ACM

  // First link new system callback to app callback and fill cancel info.
  // Then register system callback with ObjectMgr...
  if (pcb)
  {
    nRet = IMediaUtil_RegisterEncodeCB(pme, pcb);
    if (nRet)
    {
      return nRet;
    }
    else 
    {
      // Update and save the destination information
      per->clsMedia = AEECLSID_PNG;
      per->pmdDest = pei->pmdDest;
      pme->m_per = per;
    }
  }

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
#ifndef FEATURE_ODM_UI
   pme->m_reasonCode = ACM_EDIT_PNG_ARM;
   nRet = IMediaUtil_ACMAcquire (pme);

   // resource acquiring failed 
   if (nRet != SUCCESS)
   {
     IMediaUtil_CancelEncodeCB(pme);
     return EFAILED;
   }
   // delayed acquiring
   else if (pme->m_resState == ACM_RESOURCE_ACQUIRING)
   {
     // save the pei information
     nRet = IMediaUtil_SaveEncodeInfo (pme, pei);
     if (nRet != SUCCESS)
     {
       IMediaUtil_CancelEncodeCB(pme);
       IMediaUtil_ACMFree (pme);
     }
     return nRet;
   }
   // resource acquired, fall through
#endif // FEATURE_ODM_UI
#endif // FEATURE_ACM || FEATURE_BMP_ACM

  nRet = PNG_ProceedWithEncode (pme, per, pei);

  if (nRet)
  {
    IMediaUtil_CancelEncodeCB(pme);
#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    IMediaUtil_ACMFree (pme);
#endif // FEAGTURE_ACM || FEATURE_BMP_ACM
  }
#endif // FEATURE_PNG_ENCODER
  
 return nRet;
}

#ifdef FEATURE_PNG_ENCODER
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

    pnge_handle_type *pnge_handle = (pnge_handle_type *) handle;
    IMediaUtil* pme = NULL;

    if (!handle)
      goto CBExit;

    // If  there is no user registered callback, exit
    pme = (IMediaUtil *) AEEObjectMgr_GetObject(
                            (AEEObjectHandle)pnge_handle->appHandle);
    if (pme == NULL || pme->m_pcbSys == NULL)
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
    ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);

    // Operation is complete. So, reset the saved parameters in IMediaUtil struct
    IMediaUtil_ResetEncode(pme);

CBExit:

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
    IMediaUtil_ACMFree (pme);
#endif // FEATURE_ACM || FEATURE_BMP_ACM

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
    case IDIB_COLORSCHEME_565:
        *outColor = RGB565;
        break;
    case IDIB_COLORSCHEME_666:
        *outColor = RGB666;
        break;
    case IDIB_COLORSCHEME_888:
        *outColor = RGB888;
        break;
    case IYCRCB_COLORSCHEME_420LP:
        *outColor = YCrCb420LP;
        break;
    case IYCRCB_COLORSCHEME_422LP:
        *outColor = YCrCb422LP;
        break;
    default:
        bMap = FALSE;
        break;
    }
    return bMap;
}

#endif //FEATURE_PNG_ENCODER

#else // CMX 2.x

/*=========================================================================

========================================================================= */
static int IMediaUtil_EncodeMedia(IMediaUtil * pme, AEEMediaEncodeResult * per, AEECLSID clsDest, AEEMediaEncodeInfo * pei, AEECallback * pcb)
{
   return EUNSUPPORTED;
}

#endif // defined(FEATURE_BREW_CMX_V3) 


#if defined (FEATURE_BMP_ACM)
#error code not present
#endif // FEATURE_BMP_ACM


#if defined (FEATURE_ACM)

/*===========================================================================

FUNCTION IMediaUtil_ACMAcquire

DESCRIPTION
   Acquire ACM resource
   
DEPENDENCIES
  none

RETURN VALUE
  SUCCESS or EFAILED

SIDE EFFECTS
  none
===========================================================================*/
static int IMediaUtil_ACMAcquire (IMediaUtil *pme)
{
#ifdef FEATURE_ODM_UI
   pme->m_resState = ACM_RESOURCE_ACQUIRED;
   return SUCCESS;
#else
  ACM_resource_state_type  pdwstatus = ACM_RESOURCE_UNKNOWN;
  PFNNOTIFY                pcb;
  int                      nRet;
   
  if (pme->m_reasonCode == ACM_EDIT_JPEG_QDSP) 
  {
    pcb = (PFNNOTIFY) JPEGEncode_ACMStatusChange;
  }
  // ACM_EDIT_PNG_ARM
  else 
  {
    pcb = (PFNNOTIFY) PNGEncode_ACMStatusChange;
  }

  // First try acquiring from priority manager
  nRet = OEMACM_PriorityMgr_Acquire (pme, pme->m_reasonCode, &pdwstatus);
  if(nRet != SUCCESS || pdwstatus == ACM_RESOURCE_BUSY) 
  {
    return EFAILED;
  }

  // Register for status changes
  pme->m_cbACMdata.pdata = pme;  
  pme->m_cbACMdata.state = ACM_RESOURCE_UNKNOWN;
  if(OEMACM_StateMgr_OnStatusChangeNotify(pme, 
                                          pcb, 
                                          &pme->m_cbACMdata) != SUCCESS) 
  {
    OEMACM_PriorityMgr_Free (pme);
    return EFAILED;
  }

  if(pdwstatus == ACM_RESOURCE_ACQUIRED) 
  {
    pme->m_resState = ACM_RESOURCE_ACQUIRED;
  }
  else
  {
    pme->m_resState = ACM_RESOURCE_ACQUIRING;
  }
  return SUCCESS;
#endif // FEATURE_ODM_UI
}

/*===========================================================================

FUNCTION JPEGEncode_ACMStatusChange

DESCRIPTION
   Receive status changes from ACM regarding JPEG encoding.
   
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
static void JPEGEncode_ACMStatusChange (void* p)
{
  ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
  IMediaUtil             *pme    = (IMediaUtil*)cbdata->pdata;
  int nRet = EUNSUPPORTED;

  if(cbdata->state == ACM_RESOURCE_ACQUIRED) 
  {
    MSG_HIGH ("OEMMediaUtil - JPEG encoder delayed acquiring",0,0,0);	
#if (defined(FEATURE_JPEG_ENCODER_REV2) || defined (FEATURE_JPEG_ENCODER))
    nRet = JPEG_ProceedWithEncode (pme, pme->m_per, pme->m_pei);
#endif
    if ( nRet != SUCCESS)
    {
      // callback to user
      pme->m_per->nResult = EFAILED;
      ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);
      IMediaUtil_ResetEncode (pme);
      IMediaUtil_ACMFree (pme);
    }
    else
    {
      pme->m_resState = ACM_RESOURCE_ACQUIRED;
    }
  }
  // force release
  else 
  {
    MSG_HIGH("OEMMediaUtil - JPEG encoder force release",0,0,0);	

    if (pme->m_resState == ACM_RESOURCE_ACQUIRED)
    {
#ifdef FEATURE_JPEG_ENCODER_REV2
      jpeg_encoder_abort ((uint16) pme->m_hObjEncode);
#else 
#ifdef FEATURE_JPEG_ENCODER
      jpeg_abort_encode ();
#endif // FEATURE_JPEG_ENCODER
#endif // FEATURE_JPEG_ENCODER_REV2
    }
    else if (pme->m_resState == ACM_RESOURCE_ACQUIRING)
    {
      // callback to user
      pme->m_per->nResult = EFAILED;
      ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);
      IMediaUtil_ResetEncode (pme);
      IMediaUtil_ACMFree (pme);
    }
  }
  IMediaUtil_FreeSavedEncodeInfo (pme);
}

/*===========================================================================

FUNCTION PNGEncode_ACMStatusChange

DESCRIPTION
   Receive status changes from ACM regarding PNG encoding.
   
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
static void PNGEncode_ACMStatusChange (void* p)
{
  ACM_Callback_data_type *cbdata = (ACM_Callback_data_type*)p;
  IMediaUtil             *pme    = (IMediaUtil*)cbdata->pdata;
  int nRet = EUNSUPPORTED;

  if(cbdata->state == ACM_RESOURCE_ACQUIRED) 
  {
    MSG_HIGH ("OEMMediaUtil - PNG encoder delayed acquiring",0,0,0);	
#ifdef FEATURE_PNG_ENCODER
    nRet = PNG_ProceedWithEncode(pme,pme->m_per,pme->m_pei);
#endif
    if ( nRet!= SUCCESS)
    {
      // callback to user
      pme->m_per->nResult = EFAILED;
      ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);
      IMediaUtil_ResetEncode (pme);
      IMediaUtil_ACMFree (pme);
    }
    else
    {
      pme->m_resState = ACM_RESOURCE_ACQUIRED;
    }
  }
  // delayed acquiring
  else // if (cbdata->state == ACM_RESOURCE_BUSY)
  {
    MSG_HIGH("OEMMediaUtil - PNG encoder force released",0,0,0);

#ifdef FEATURE_PNG_ENCODER
    if (pme->m_resState == ACM_RESOURCE_ACQUIRED)
    {
      pnge_encode_abort ();
    }
    else
#endif // FEATURE_PNG_ENCODER

    if (pme->m_resState == ACM_RESOURCE_ACQUIRING)
    {
      // callback to user
      pme->m_per->nResult = EFAILED;
      ISHELL_Resume(pme->m_pIShell, pme->m_pcbSys);
      IMediaUtil_ResetEncode (pme);
      IMediaUtil_ACMFree (pme);
    }
  }
  IMediaUtil_FreeSavedEncodeInfo (pme);
}

/*===========================================================================
FUNCTION IMediaUtil_ACMFree

DESCRIPTION
   Free any ACM resource for this IMediaUtil interface.
   
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
static void IMediaUtil_ACMFree (IMediaUtil* pme)
{
#ifndef FEATURE_ODM_UI
  if (pme != NULL && pme->m_resState != ACM_RESOURCE_UNKNOWN)
  {
    OEMACM_PriorityMgr_Free (pme);
    pme->m_resState = ACM_RESOURCE_UNKNOWN;
  }
#endif // FEATURE_ODM_UI
}
#endif // FEATURE_ACM

#if defined(FEATURE_ACM) || defined(FEATURE_BMP_ACM)
/*===========================================================================
FUNCTION IMediaUtil_SaveEncodeInfo

DESCRIPTION
   Saves the AEEMediaEncodeInfo to be used for delayed acquiring
   
DEPENDENCIES
  The encode buffer is not saved. And it is assumed to stay valid.

RETURN VALUE
  SUCCESS or other AEEError Code
===========================================================================*/
static int IMediaUtil_SaveEncodeInfo (IMediaUtil *pme, AEEMediaEncodeInfo * pei)
{
  AEEMediaEncodeInfo *m_pei;
  AEEMediaData       *pmd, *m_pmd;
  int                 totalSize, i;
  char               *ptr;
  
  pme->m_pei = NULL;

  totalSize = sizeof (AEEMediaEncodeInfo) + pei->nCount * sizeof (AEEMediaData) +
              pei->dwSpecSize;

  // now total up the length of the file names
  pmd = pei->pmdList;
  for (i = 0; i < pei->nCount; i++)
  {
    if (pmd->clsData == MMD_FILE_NAME && pmd->pData != NULL) 
    {
      totalSize += (STRLEN ((char *) pmd->pData) + 1);
    }
    pmd++;
  }

  ptr = MALLOC (totalSize);
  if (ptr == NULL)
  {
    return ENOMEMORY;
  }

  m_pei = (AEEMediaEncodeInfo *)ptr;
  pme->m_pei = m_pei;
  MEMCPY (m_pei, pei, sizeof (AEEMediaEncodeInfo));
  ptr += sizeof (AEEMediaEncodeInfo);
  
  m_pei->pmdList = (AEEMediaData *) ptr;
  MEMCPY (m_pei->pmdList, pei->pmdList, pei->nCount * sizeof (AEEMediaData));
  ptr += (pei->nCount * sizeof (AEEMediaData));

  m_pei->pEncodeSpec = ptr;
  MEMCPY (m_pei->pEncodeSpec, pei->pEncodeSpec, pei->dwSpecSize);
  ptr += pei->dwSpecSize;

  pmd   = pei->pmdList;
  m_pmd = m_pei->pmdList;
  for (i = 0; i < pei->nCount; i++)
  {
    if (pmd->clsData == MMD_FILE_NAME && pmd->pData != NULL) 
    {
      m_pmd->pData = ptr;
      STRLCPY ((char*) m_pmd->pData, (char *) pmd->pData,sizeof(m_pmd->pData));
      ptr += (STRLEN ((char *) pmd->pData) + 1);
    }
    pmd++;
    m_pmd++;
  }

  return SUCCESS;
}

/*===========================================================================
FUNCTION IMediaUtil_FreeSavedEncodeInfo

DESCRIPTION
   Frees the saved input AEEMediaEncodeInfo for delayed acquiring.
   
DEPENDENCIES
  Buffer is not saved. 

RETURN VALUE
  SUCCESS or other AEEError Code
===========================================================================*/
static void IMediaUtil_FreeSavedEncodeInfo (IMediaUtil *pme)
{
  FREEIF (pme->m_pei);
}


#endif // FEATURE_ACM || FEATURE_BMP_ACM
#endif // defined(FEATURE_BREW_MULTIMEDIA)
/* ============= End of File =============================================== */

