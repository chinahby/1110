/*======================================================
FILE:  OEMStockFonts.c

SERVICES:
   Static font example.

GENERAL DESCRIPTION:
   This file provides an example for using the BREW bit
   font mechanism to provide OEM fonts.

PUBLIC CLASSES AND STATIC FUNCTIONS:

   OEMStockFonts_New()


INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"
#include "AEEError.h"
#include "AEEModTable.h"
#include "AEEBitFont.h"
#include "samplefont.bid"

#if defined(_MSC_VER)
   #define ALIGN4 __declspec(align(4))
#elif defined(__ARMCC_VERSION)
   #define ALIGN4 __align(4)
#else
   #error "Unsupported compiler"
#endif


static int OEMStockFonts_New(IShell * piShell, AEECLSID cls, void **ppif);


// This needs to be included in the gpICList table in OEMModTable.c
// to allow these fonts to be instantiated.
const AEEStaticClass gOEMStockFontsClasses[] = {
   {AEECLSID_SAMPLEFONT,      ASCF_UPGRADE, 0, 0, OEMStockFonts_New},
   NULL
};

//----  Font data
//
// Here bitmap data is included as byte arrays from which the stock font
// objects are created.
//


static const ALIGN4 byte abySampleFont[] = {
#include "samplefont.bmp.h"
};



//------  New function for AEEStaticClass entries:
//


static OEMStockFonts_New(IShell * piShell, AEECLSID cls, void **ppif)
{
   const byte *pby;
   int cb;

   switch (cls) {      
   case AEECLSID_SAMPLEFONT:
      pby = abySampleFont;
      cb = sizeof(abySampleFont);
      break;

     default:
        return ECLASSNOTSUPPORT;
   }

   return AEEBitFont_NewFromBBF(pby, cb, AEEBITFONT_STATICDATA, 0, (IFont**) ppif);
}

