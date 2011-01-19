/*
  ========================================================================

  FILE:       ResDecoderCache.c

  SERVICES:   ResDecoderCache implementation 

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/

#include "AEEStdlib.h"
#include "AEEShell.h"
#include "IImageDecoder.h"
#include "ResDecoderCache.h"
#include "wutil.h"

typedef struct 
{
   char *pszMime;
   IBase *piImageDecoder;
   AEECLSID clsId;
} DecoderCache;

struct ResDecoderCache
{
   IShell *piShell;
   DecoderCache *pDecoders;
   int nNumDecoders;
};


///////////////////////////////////////////////////////

/* ResDecoderCache_GetDecoder
|| This function, provided a mimetype, will return an ImageDecoder object
|| for that mime type. Internally, it keeps a cache of mimetype-ImageDecoder
|| pairs, and returns an AddRef'ed IImageDecoder, if it exists in the cache
|| or add it to the cache if it doesn't exist. 
|| Note that, this way, caller re-uses a single decoder to decode multiple image
|| streams. 
|| Since IImageDecoder was introduced in Brew 3.1.2, this functionality works 
|| only for Brew versions 3.1.2 and above.
*/

int ResDecoderCache_GetDecoder(ResDecoderCache *pResDecoderCache, const char *pszMime, 
                               IBase **ppiImageDecoder, AEECLSID *pnClsId) 
{
   int nErr = EFAILED;
   int i;
   AEECLSID clsId = 0;
   DecoderCache *pDecoders;
   if(NULL == pResDecoderCache) {
      return nErr;
   }
   if(pnClsId) {
      *pnClsId = 0;
   }

   for(i = pResDecoderCache->nNumDecoders-1; i>=0 ; i--) {
      if(pResDecoderCache->pDecoders[i].piImageDecoder) {
         if(!STRCMP(pszMime, pResDecoderCache->pDecoders[i].pszMime)) {
            *ppiImageDecoder = pResDecoderCache->pDecoders[i].piImageDecoder;
            IBASE_AddRef(pResDecoderCache->pDecoders[i].piImageDecoder);
            if(pnClsId) {
               *pnClsId = pResDecoderCache->pDecoders[i].clsId;
            }

            return SUCCESS;
         }
      }
   }
   pDecoders = REALLOC(pResDecoderCache->pDecoders, 
      sizeof(DecoderCache)*(uint32)(pResDecoderCache->nNumDecoders+1));
   
   if(!pDecoders) {
      // Unable to allocate memory at this point.
      // Just keep the existing decoders and return gracefully.
      return EFAILED;
   }
   pResDecoderCache->pDecoders = pDecoders;
   i = pResDecoderCache->nNumDecoders;
   pResDecoderCache->nNumDecoders++;
   
   if(NULL == pResDecoderCache->pDecoders) {
      return nErr;
   }
   clsId = ISHELL_GetHandler(pResDecoderCache->piShell, AEEIID_IMAGEDECODER, pszMime);
   if(clsId) {
      nErr = ISHELL_CreateInstance(pResDecoderCache->piShell, clsId, 
         (void**) ppiImageDecoder);
      if(!nErr) {
         pResDecoderCache->pDecoders[i].pszMime = STRDUP(pszMime);
         if(pResDecoderCache->pDecoders[i].pszMime) {
            pResDecoderCache->pDecoders[i].piImageDecoder = *ppiImageDecoder;
            IBASE_AddRef(pResDecoderCache->pDecoders[i].piImageDecoder);
            pResDecoderCache->pDecoders[i].clsId = clsId;
         }
         if(pnClsId) {
           *pnClsId = clsId;
         }

      }
   }
   return nErr;
}



int ResDecoderCache_Ctor(ResDecoderCache **ppResDecoderCache, IShell *piShell)
{
   *ppResDecoderCache = (ResDecoderCache*) MALLOC(sizeof(ResDecoderCache));
   if(NULL == *ppResDecoderCache) {
      return ENOMEMORY;
   }
   (*ppResDecoderCache)->piShell = piShell; 
   ISHELL_AddRef(piShell);
   return SUCCESS;
}

void ResDecoderCache_Dtor(ResDecoderCache *pResDecoderCache)
{
   int i;
   if(NULL == pResDecoderCache) {
      return;
   }
   if(pResDecoderCache->nNumDecoders) {
      for(i=pResDecoderCache->nNumDecoders-1;i>=0;i--) {
         RELEASEIF(pResDecoderCache->pDecoders[i].piImageDecoder);
         FREEIF(pResDecoderCache->pDecoders[i].pszMime);
      }
      FREEIF(pResDecoderCache->pDecoders);
   }
   RELEASEIF(pResDecoderCache->piShell);
   FREE(pResDecoderCache);
}

