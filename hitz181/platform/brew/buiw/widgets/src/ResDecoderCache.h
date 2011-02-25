/*
  ========================================================================

  FILE:  ResDecoderCache.h
  
  SERVICES: 

  GENERAL DESCRIPTION: Type declaration for getting cached IImageDecoder* 
  objects from ResFile.

  ========================================================================
  ========================================================================
    
               Copyright © 2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __RESDECODERCACHE_H__
#define __RESDECODERCACHE_H__

#include "AEE.h"

typedef struct ResDecoderCache ResDecoderCache;

int ResDecoderCache_Ctor(ResDecoderCache **ppResDecoderCache, IShell *piShell);
void ResDecoderCache_Dtor(ResDecoderCache *pResDecoderCache);
//
// ResDecoderCache_GetDecoder
// This function, provided a mimetype, will return an ImageDecoder object
// for that mime type. Internally, it will keep a cache of mimetype-ImageDecoder
// pairs, and will return an AddRef'ed IImageDecoder, if it exists in the cache
// or add it to the cache if it doesn't exist. 
// Note that, this way, caller re-uses a single decoder to decode multiple image
// streams. 
// Since IImageDecoder was introduced in Brew 3.1.2, this functionality works 
// only for Brew versions 3.1.2 and above.

int ResDecoderCache_GetDecoder(ResDecoderCache *pResDecoderCache, const char *pszMime, 
                            IBase **ppiImageDecoder, AEECLSID *pnClsId);

#endif //__RESDECODERCACHE_H__
