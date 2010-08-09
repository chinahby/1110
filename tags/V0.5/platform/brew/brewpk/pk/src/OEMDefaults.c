/*=============================================================================

FILE:      OEMDefaults.c

SERVICES:

GENERAL DESCRIPTION:
   This files is used to map default class IDs to specific class IDs.  For
   instance, AEECLSID_GRAPHICS could be configured to point the the BREW
   software implmementation of IGraphics (AEECLSID_GRAPHICS_BREW) or to
   another hardware implementation.  This file is used to setup that
   mapping.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"
#include "AEEModTable.h"
#include "AEEMimeTypes.h"
#include "AEEGraphics_BREW.bid"
#include "IImageDecoder.h"
#include "JPEGDecoder.bid"
#include "AEEJPEGDecoder_BREW.bid"
#include "AEEJPEG_BREW.bid"
#include "AEEPNGDecoder_BREW.bid"
#include "AEEPNG_BREW.bid"
#include "PNGDecoder.bid"
#include "AEEBCI_BREW.bid"

#if defined(FEATURE_JPEG_DECODER) && defined(FEATURE_QCT_JPEG_DECODER)
#include "JPEG.BID"
#endif

// This is a private class ID used to register the init function.
#define AEECLSID_OEMDEFAULTS 0x0103026f

//=============================================================================
//
//
//=============================================================================
static void OEMDefaults_Init(IShell *piShell)
{

#if defined(FEATURE_JPEG_DECODER) 
   {
      int ii;
      static const char *pMimeTypes[] = {
         "image/jpg",
         "jpg",
         "image/jpeg",
         "jpeg"
      };
      for(ii = 0; ii < ARRAY_SIZE(pMimeTypes); ++ii) {
         //decoder
         ISHELL_RegisterHandler(piShell, AEEIID_IMAGEDECODER, pMimeTypes[ii], 0);
         ISHELL_RegisterHandler(piShell, AEEIID_IMAGEDECODER, pMimeTypes[ii], AEECLSID_JPEGDECODER);

         //viewer
         ISHELL_RegisterHandler(piShell, HTYPE_VIEWER, pMimeTypes[ii], 0);
         ISHELL_RegisterHandler(piShell, HTYPE_VIEWER, pMimeTypes[ii], AEECLSID_JPEG);

         ISHELL_RegisterHandler(piShell, AEECLSID_VIEW, pMimeTypes[ii], 0);
         ISHELL_RegisterHandler(piShell, AEECLSID_VIEW, pMimeTypes[ii], AEECLSID_JPEG);
      }
   }
#endif //(FEATURE_JPEG_DECODER) 
#if defined(FEATURE_BREW_PNG_DECODE) 
#define FT_PNG                "png"
   {
      int i;
      static const char *pMimeTypes[] = {
         MT_PNG,
         FT_PNG,
      };
      for(i = 0; i < ARRAY_SIZE(pMimeTypes); ++i) {
         //decoder
         ISHELL_RegisterHandler(piShell, AEEIID_IMAGEDECODER, pMimeTypes[i], 0);
         ISHELL_RegisterHandler(piShell, AEEIID_IMAGEDECODER, pMimeTypes[i], AEECLSID_PNGDECODER);
         //viewer
         ISHELL_RegisterHandler(piShell, HTYPE_VIEWER, pMimeTypes[i], 0);
         ISHELL_RegisterHandler(piShell, HTYPE_VIEWER, pMimeTypes[i], AEECLSID_PNG);

         ISHELL_RegisterHandler(piShell, AEECLSID_VIEW, pMimeTypes[i], 0);
         ISHELL_RegisterHandler(piShell, AEECLSID_VIEW, pMimeTypes[i], AEECLSID_PNG);
      }
   }
#endif //(FEATURE_BREW_PNG_DECODE) 
#if defined(FEATURE_BCI_DECODE)
   {
      //Deregister any existing handle for this MIME type
      ISHELL_RegisterHandler(piShell, HTYPE_VIEWER, MT_BCI, 0);
      //Register this class as the BCI handler
      ISHELL_RegisterHandler(piShell, HTYPE_VIEWER, MT_BCI, AEECLSID_BCI);
   }
#endif //(FEATURE_BCI_DECODE)
}


//=============================================================================
//
//
//=============================================================================
static int OEMDefaults_CreateInstance(IShell *piShell, AEECLSID cls, void **ppif)
{
   AEECLSID clsSpecific;

   switch (cls) {
#if defined(FEATURE_GRAPHICS)
   case AEECLSID_GRAPHICS:
      clsSpecific = AEECLSID_GRAPHICS_BREW;
      break;
#endif

#if defined(FEATURE_JPEG_DECODER) 
   case AEECLSID_JPEG:
      clsSpecific = AEECLSID_JPEG_BREW;
      break;
   case AEECLSID_JPEGDECODER:
      clsSpecific = AEECLSID_JPEGDECODER_BREW;
      break;
#endif

#if defined(FEATURE_BREW_PNG_DECODE) 
   case AEECLSID_PNG:
      clsSpecific = AEECLSID_PNG_BREW;
      break;
   case AEECLSID_PNGDECODER:
      clsSpecific = AEECLSID_PNGDECODER_BREW;
      break;
#endif //(FEATURE_BREW_PNG_DECODE) 
#if defined(FEATURE_BCI_DECODE)
   case AEECLSID_BCI:
      clsSpecific = AEECLSID_BCI_BREW;
      break;
#endif //(FEATURE_BCI_DECODE)
   default:
      return ECLASSNOTSUPPORT;
   }

   return ISHELL_CreateInstance(piShell, clsSpecific, ppif);
}


extern const AEEStaticClass gascOEMDefaults[] = {
   {AEECLSID_OEMDEFAULTS,  ASCF_UPGRADE,  0, OEMDefaults_Init, OEMDefaults_CreateInstance},
#if defined(FEATURE_GRAPHICS)
   {AEECLSID_GRAPHICS,     ASCF_UPGRADE,  0, 0,                OEMDefaults_CreateInstance},
#endif
#if defined(FEATURE_JPEG_DECODER)
   {AEECLSID_JPEG,            ASCF_UPGRADE,  0, 0,              OEMDefaults_CreateInstance},
   {AEECLSID_JPEGDECODER,     ASCF_UPGRADE,  0, 0,              OEMDefaults_CreateInstance},
#endif
#if defined(FEATURE_BREW_PNG_DECODE)
   {AEECLSID_PNG,            ASCF_UPGRADE,  0, 0,              OEMDefaults_CreateInstance},
   {AEECLSID_PNGDECODER,     ASCF_UPGRADE,  0, 0,              OEMDefaults_CreateInstance},
#endif
#if defined(FEATURE_BCI_DECODE)
   {AEECLSID_BCI,            ASCF_UPGRADE,  0, 0,              OEMDefaults_CreateInstance},
#endif
   NULL
};
