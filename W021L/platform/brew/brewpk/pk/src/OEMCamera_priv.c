/*===========================================================================

FILE: OEMCamera_priv.c

SERVICES: ICamera interface implementation specific to QCamera

DESCRIPTION
   This file implements BREW ICamera functions for QCamera.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "BREWVersion.h"
#include "OEMFeatures.h"
#if defined(FEATURE_BREW_CAMERA)
#include "camera.h"
#include "cmx.h"

#include "AEECamera.h"
#include "OEMCameraDMSS.h"
#include "OEMCamera_Priv.h"
#include "OEMObjectMgr.h"
#include "OEMCamera.h"

#include "OEMFS.h"
#include "AEE_OEM.h"
#if MIN_BREW_VERSION(3,1)
#include "AEE_OEMDispatch.h"
#endif
#include "AEEFile.h"
#include "AEEError.h"
#include "AEEMediaFormats.h"
#include "AEEBitmap.h"
#include "AEEPosDet.h"
#include "AEEStdLib.h"
#include "AEEHandle.h"

extern void ICamera_CameraSystemNotify(ICamera * pme, AEECameraNotify * pcn);
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
extern void ICamera_SetSnapshotProperties(OEMCamera * pme, camera_encode_properties_type * p);
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */


int ICamera_SaveAppPicture(ICamera * pICam, uint16 nWidth, uint16 nHeight,
                                  void *pBuff)
{
   int                  nCamRet;
   camera_handle_type   h;
   CameraRsp *          pRsp;
   camera_frame_type    frame;
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
   camera_encode_properties_type   p;
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */
   
   // Temp hack till stub layer is introduced.
   OEMCamera* pme;
   pme = pICam->m_hInstance;
   
   if(!pme)
	   return EFAILED;
   
#ifdef FEATURE_CAMERA_ENCODE_PROPERTIES
   ICamera_SetSnapshotProperties(pme, &p);
   camera_set_encode_properties(&p);
#endif /* FEATURE_CAMERA_ENCODE_PROPERTIES */

   nCamRet = OEMCamera_SetSnapshotDestination(pme, &h);
   if (SUCCESS != nCamRet)
      return nCamRet;

   pRsp = OEMCamera_AllocCmdCameraRsp(pme, CAM_CMD_ENCODESNAPSHOT, 0);
   if (!pRsp)
      return EITEMBUSY;

   frame.buffer = pBuff;
   frame.dx = frame.captured_dx = nWidth;
   frame.dy = frame.captured_dy = nHeight;
   frame.rotation = 0;
   frame.format = CAMERA_YCBCR;
   nCamRet = camera_encode_picture(&frame, &h,
      OEMCamera_CameraLayerCB, (void *)pRsp->hObject);

   return OEMCamera_AEEError(nCamRet);
}


#ifdef FEATURE_CMX
static void ICamera_png_event_cb (
                       cmx_status_type status,
                       const void      *client_data,
                       void            *server_data,
                       uint32          num_bytes
                       ) 
{
   // not needed //CameraRsp rsp = {0};
   cmx_png_spec_type *png_spec;
   static int16 read_data_block = -1;
   static int16 block = 0;
   static int16 read_palette = 0;
   static boolean read_transparent = FALSE;   
   
   OEMCamera* pme;
   /* CR 73909: pICamera may already be released! 
      validate the object first before do anything!*/
   ICamera * pICamera = (ICamera *) AEEObjectMgr_GetObject((AEEObjectHandle)client_data);
   if (!pICamera)
      return;

   pme = pICamera->m_hInstance;
   
   switch (status) 
   {
   case CMX_SUCCESS:
      block = 0;
      read_data_block = -1;
      read_palette = 0;
      read_transparent = FALSE;
      if (pme->m_image.pPalette != NULL)
      {
         FREE(pme->m_image.pPalette);
         pme->m_image.pPalette = NULL;
      }
      
      //MSG_LOW("PNG command accepted", 0,0,0);
      break;
      
   case CMX_ABORTED:
      {
         if (pme)
         {
            CameraRsp Rsp;
            Rsp.bInUse = TRUE;
            Rsp.camNotify.nCmd = CAM_CMD_OVERLAY_IMAGE;
            Rsp.camNotify.nSubCmd = 0;
            Rsp.camNotify.nStatus = CAM_STATUS_ABORT;
            Rsp.camNotify.dwSize = pme->m_nOIBytesRecv;
            Rsp.camNotify.pData = &pme->m_image;
            ICamera_CameraSystemNotify(pICamera, &Rsp.camNotify);
         }
      }
      //MSG_HIGH("Aborted PNG.",0,0,0);
      break;
      
   case CMX_CONTINUE:
      //MSG_LOW("Continue PNG.",0,0,0);
      if (read_data_block == -1)
      {
         if (read_palette > 0)
         {
            if (pme->m_image.pPalette)
            {
               MEMCPY(pme->m_image.pPalette, server_data, num_bytes);
               read_palette -= (unsigned short)num_bytes;
            }
            read_palette = FALSE;
         }
         else if (read_transparent)
         {
            if (pme->m_image.nColorType == 3)
            {
               pme->m_image.nTransparentColor = ((unsigned char *)server_data)[0];
            }
            else if (pme->m_image.nColorType == 2)
            {
               MEMCPY(&pme->m_image.nTransparentColor, server_data, 3);
            }
            read_transparent = FALSE;
         }
         else if (MEMCMP(server_data, "tran", 4) == 0)
         {
            read_transparent = TRUE;
         }
         else if (MEMCMP(server_data, "plte", 4) == 0)
         {
            pme->m_image.nPaletteSize = atoi(((char *)server_data) + 5);
            pme->m_image.pPalette = MALLOC(pme->m_image.nPaletteSize);
            read_palette = pme->m_image.nPaletteSize;
         }
         else if (MEMCMP(server_data, "bkgd", 4) == 0)
         {
            // we dont use the background color
         }
         else if (num_bytes == 1 && ((byte *)server_data)[0] == 0x0a)
         {
            // ignore this block
         }
         else if (block > 0)
         {
            read_data_block = block;
         }
      }
      if (read_data_block > 0 && block >= read_data_block)
      {
         if (pme->m_nOIBytesRecv + num_bytes <=  pme->m_nOIBufSize)
         {
            MEMCPY(&pme->m_image.pImage[pme->m_nOIBytesRecv], server_data, num_bytes);
            pme->m_nOIBytesRecv += num_bytes;
         }
      }
      block++;
      //#if defined (PNG_OUT) || defined (SAF_OUT)
      //   img_num_bytes = num_bytes;
      //   memcpy((uint8 *) img_data, server_data, num_bytes);
      //#endif
      //ui_add_event ((word) (UI_MM_EVENT(MM_EVENT_CONTINUE)));
      break;
      
   case CMX_PNG_TEXT:
      if ((num_bytes != 0) && (server_data != NULL)) 
      {
         //MSG_LOW("PNG Text callback",0,0,0);
      }
      break;
      
   case CMX_PNG_SPEC:
      png_spec = (cmx_png_spec_type *) server_data;
      //MSG_LOW("PNG Specs ====================", 0,0,0);
      //MSG_LOW("width: %ld, height: %ld.", png_spec->width,png_spec->height,0);
      //MSG_LOW("Depth: %d, color: %d, comp: %d", png_spec->bit_depth, png_spec->color_type, png_spec->compression);
      //MSG_LOW("Filter: %d, Interlace: %d", png_spec->filter, png_spec->interlace, 0);
      pme->m_image.nWidth     = (uint16)png_spec->width;
      pme->m_image.nHeight    = (uint16)png_spec->height;
      pme->m_image.cBPP       = (uint8)png_spec->bit_depth;
      pme->m_image.nColorType = (uint16)png_spec->color_type;
      pme->m_image.nTransparentColor = 0xFFFFFFFF;
      break;
      
   case CMX_DONE:
      {
         if (num_bytes > 0 && pme->m_nOIBytesRecv + num_bytes <=  pme->m_nOIBufSize)
         {
            MEMCPY(&pme->m_image.pImage[pme->m_nOIBytesRecv], server_data, num_bytes);
            pme->m_nOIBytesRecv += num_bytes;
         }
         
         if (pme)
         {
            CameraRsp Rsp;
            Rsp.bInUse = TRUE;
            Rsp.camNotify.nCmd = CAM_CMD_OVERLAY_IMAGE;
            Rsp.camNotify.nSubCmd = 0;
            Rsp.camNotify.nStatus = CAM_STATUS_DONE;
            Rsp.camNotify.dwSize = pme->m_nOIBytesRecv;
            Rsp.camNotify.pData = &pme->m_image;
            ICamera_CameraSystemNotify(pICamera, &Rsp.camNotify);
         }
      }
      break;
      
   case CMX_FAILURE:
      {
         if (pme)
         {
            CameraRsp Rsp;
            Rsp.bInUse = TRUE;
            Rsp.camNotify.nCmd = CAM_CMD_OVERLAY_IMAGE;
            Rsp.camNotify.nSubCmd = 0;
            Rsp.camNotify.nStatus = CAM_STATUS_FAIL;
            Rsp.camNotify.dwSize = pme->m_nOIBytesRecv;
            Rsp.camNotify.pData = &pme->m_image;
            ICamera_CameraSystemNotify(pICamera, &Rsp.camNotify);
         }
      }
      //MSG_HIGH("PNG Failure!!!",0,0,0);
      break;
   }
}
#endif /* FEATURE_CMX */

boolean ICamera_LoadOverlayImage(ICamera * pICam, char *fileName, byte *buffer, uint32 bufSize)
{
	// temp hack till stub layer is introduced.
   OEMCamera* pme;
   pme = pICam->m_hInstance;

#ifdef FEATURE_CMX
   {
      cmx_handle_type cmx_handle;
      char* dot_ptr;
      boolean is_png_file;
      
      cmx_handle.efs.source_type = CMX_SOURCE_EFS;
   #if MIN_BREW_VERSION(3,0)
      {
         extern int AEEFile_BuildPath(const char *cpszIn, char **ppszOut);
         char* pFileName = NULL;
         int nLen = sizeof(cmx_handle.efs.filename);
         if(AEE_SUCCESS != AEEFile_BuildPath(fileName, &pFileName))
         {
            return EFAILED;
         }
         if(AEE_SUCCESS!=OEMFS_GetNativePath(pFileName, (char*)&cmx_handle.efs.filename, &nLen))
         {
            FREE(pFileName);
            return EFAILED;
         }
         FREE(pFileName);
      }
   #else
      strcpy(cmx_handle.efs.filename, "brew/");
      strcat(cmx_handle.efs.filename, fileName);
   #endif
      dot_ptr = strrchr( cmx_handle.efs.filename, '.');
      if (dot_ptr)
         {
         dot_ptr++;
         }
      is_png_file = (dot_ptr && !strcmp( dot_ptr, "png" )) ? TRUE : FALSE;
      if (is_png_file) 
      {
   	  if(!pme)
   	   return EFAILED;
         pme->m_image.pImage = buffer;
         pme->m_nOIBufSize   = bufSize;
         pme->m_nOIBytesRecv = 0;

         /* CR73909: registers the ICamera object and returns its object handle,
            as the OEMCamera's object handle, then passing in the object
            handler as the user data */ 
         AEEObjectMgr_Unregister(pme->m_hObject);
         if (AEEObjectMgr_Register(pICam, &pme->m_hObject))
	           return FALSE;

         cmx_png_decode(&cmx_handle,
                    (cmx_image_cb_func_ptr_type) ICamera_png_event_cb,
                    (void *)pme->m_hObject);
      }
      return is_png_file;
   }
#else /* FEATURE_CMX */
   return TRUE;
#endif /* FEATURE_CMX */
}

#ifdef FEATURE_CAMERA_INCALL
int ICamera_GetInCallParm(ICamera * pme, int32 * pP1)
{
   int               nRet = SUCCESS;
   if (pP1)
   {
      int         nCamRet;
      AEEParmInfo pi;

      MEMSET(&pi, (int)0, sizeof(AEEParmInfo));
      nCamRet = (int)camera_get_parm(CAMERA_PARM_INCALL, (camera_parm_info_type *)&pi);
      if (!nCamRet)
      {
         if (pP1)
            *pP1 = pi.nCurrent;
      }
      else
         nRet = OEMCamera_AEEError(nCamRet);
   }
   return nRet;
}
#endif 
#endif /* FEATURE_BREW_CAMERA */

