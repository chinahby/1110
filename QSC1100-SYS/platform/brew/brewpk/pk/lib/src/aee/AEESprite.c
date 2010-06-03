/*======================================================
FILE:       AEESprite.c

SERVICES:   ISprite Implementation

GENERAL DESCRIPTION:
            BREW sprite engine.

        Copyright © 2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "OEMHeap.h"
#include "AEESprite_priv.h"
#include "AEESprite.h"
#include "AEEStdLib.h"

static const uint8   aSpriteSize[]     = {8, 16, 32, 64};
static const uint8   aTileSize[]       = {8, 16, 32, 64};
static const uint8   aTileSizeMask[]   = {0x07, 0x0F, 0x1F, 0x3F};
static const uint8   aTileSizeBits[]   = {3, 4, 5, 6};
static const uint16  aMapSizeMask[]    = {0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
                                          0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF, 0x03FF};
struct ISprite {
   const AEEVTBL(ISprite) *pvt;
   uint32                  m_uRefs;
   IBitmap                *m_rgSpriteBufs[SPRITE_SIZE_MAX];
   IBitmap                *m_rgTileBufs[TILE_SIZE_MAX];
   IBitmap                *m_pbmDst;
   ITransform             *m_ptrDst;
   AEETransformMatrix     *m_pMatrices;
};

/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E C L A R A T I O N S
 ************************************************************************/

static uint32  Sprite_AddRef(ISprite *po);
static uint32  Sprite_Release(ISprite *po);
static int     Sprite_QueryInterface(ISprite *po, AEECLSID clsid, void **ppNew);
static int     Sprite_SetTransformTable(ISprite *po, AEETransformMatrix *pTransformTable);
static int     Sprite_SetSpriteBuffer(ISprite *po, uint8 unSpriteSize, IBitmap *pbmSpriteBuffer);
static int     Sprite_SetTileBuffer(ISprite *po, uint8 unTileSize, IBitmap *pbmTileBuffer);
static int     Sprite_SetDestination(ISprite *po, IBitmap *pbmDst);
static int     Sprite_DrawSprites(ISprite *po, AEESpriteCmd *pCmds);
static int     Sprite_DrawSpritesSimple(ISprite *po, AEESpriteCmd *pCmds);
static int     Sprite_DrawTiles(ISprite *po, AEETileMap *pMaps);
static int     Sprite_DrawTilesSimple(ISprite *po, AEETileMap *pMaps);

static const VTBL(ISprite) gOEMSpriteFuncs = {
   Sprite_AddRef,
   Sprite_Release,
   Sprite_QueryInterface,
   Sprite_SetTransformTable,
   Sprite_SetSpriteBuffer,
   Sprite_SetTileBuffer,
   Sprite_SetDestination,
   Sprite_DrawSprites,
   Sprite_DrawTiles
};

static const VTBL(ISprite) gOEMSpriteSimpleFuncs = {
   Sprite_AddRef,
   Sprite_Release,
   Sprite_QueryInterface,
   Sprite_SetTransformTable,
   Sprite_SetSpriteBuffer,
   Sprite_SetTileBuffer,
   Sprite_SetDestination,
   Sprite_DrawSpritesSimple,
   Sprite_DrawTilesSimple
};


extern int
Sprite_New(IShell *pShell, AEECLSID cls, void **ppif)
{
   if (cls == AEECLSID_SPRITE) {
      ISprite *pMe = NULL;

      if ((pMe = (ISprite*)sys_malloc(sizeof(ISprite))) == NULL) {
         return ENOMEMORY;
      }

      MEMSET(pMe, 0, sizeof(ISprite));

      INIT_VTBL(pMe, ISprite, gOEMSpriteFuncs);
      pMe->m_uRefs = 1;

      *ppif = pMe;
      return SUCCESS;
   } else {
      return EUNSUPPORTED;
   }
}



/************************************************************************
 ** I N T E R F A C E   F U N C T I O N   D E F I N I T I O N S
 ************************************************************************/

static uint32
Sprite_AddRef(ISprite *po)
{
   return (po->m_uRefs)++;
}


static uint32
Sprite_Release(ISprite *po)
{
   int i;

   if (--po->m_uRefs != 0) {
      return po->m_uRefs;
   }

   for (i = SPRITE_SIZE_MAX - 1; i >= 0; i--) {
      if (po->m_rgSpriteBufs[i] != NULL) {
         IBITMAP_Release(po->m_rgSpriteBufs[i]);
      }
   }

   for (i = TILE_SIZE_MAX - 1; i >= 0; i--) {
      if (po->m_rgTileBufs[i] != NULL) {
         IBITMAP_Release(po->m_rgTileBufs[i]);
      }
   }

   if (po->m_pbmDst != NULL) {
      IBITMAP_Release(po->m_pbmDst);
   }

   if (po->m_ptrDst != NULL) {
      ITRANSFORM_Release(po->m_ptrDst);
   }

   // Ref count is zero. So, release memory associated with this object.
   sys_free(po);

   return 0;
}


static int
Sprite_QueryInterface(ISprite *po, AEECLSID clsid, void **ppNew)
{
   if (clsid == AEECLSID_SPRITE) {
      *ppNew = (void*)po;
      Sprite_AddRef(po);
      return SUCCESS;
   }

   *ppNew = NULL;
   return EUNSUPPORTED;
}


static int
Sprite_SetTransformTable(ISprite *po, AEETransformMatrix *pTransformTable)
{
   po->m_pMatrices = pTransformTable;
   return SUCCESS;
}


static int
Sprite_SetSpriteBuffer(ISprite *po, uint8 unSpriteSize, IBitmap *pbmSpriteBuffer)
{
   if (unSpriteSize < SPRITE_SIZE_MAX) {
      if (po->m_rgSpriteBufs[unSpriteSize] != NULL) {
         IBITMAP_Release(po->m_rgSpriteBufs[unSpriteSize]);
      }
      if (pbmSpriteBuffer != NULL) {
         IBITMAP_AddRef(pbmSpriteBuffer);
      }
      po->m_rgSpriteBufs[unSpriteSize] = pbmSpriteBuffer;
      return SUCCESS;
   }

   return EBADPARM;
}


static int
Sprite_SetTileBuffer(ISprite *po, uint8 unTileSize, IBitmap *pbmTileBuffer)
{
   if (unTileSize < TILE_SIZE_MAX) {
      if (po->m_rgTileBufs[unTileSize] != NULL) {
         IBITMAP_Release(po->m_rgTileBufs[unTileSize]);
      }
      if (pbmTileBuffer != NULL) {
         IBITMAP_AddRef(pbmTileBuffer);
      }
      po->m_rgTileBufs[unTileSize] = pbmTileBuffer;
      return SUCCESS;
   }

   return EBADPARM;
}


static int
Sprite_SetDestination(ISprite *po, IBitmap *pbmDst)
{
   int nResult;
   ITransform *pITransform = NULL;

   if (pbmDst != NULL) {
      IBITMAP_AddRef(pbmDst);
      nResult = IBITMAP_QueryInterface(pbmDst, AEECLSID_TRANSFORM, (void**)&pITransform);
      if (nResult == SUCCESS) {
         // Switch to standard implementation.
         INIT_VTBL(po, ISprite, gOEMSpriteFuncs);
      } else {
         // No transform support.  Switch to simple implementation.
         INIT_VTBL(po, ISprite, gOEMSpriteSimpleFuncs);
      }
   }

   if (po->m_pbmDst != NULL) {
      IBITMAP_Release(po->m_pbmDst);
   }
   if (po->m_ptrDst != NULL) {
      ITRANSFORM_Release(po->m_ptrDst);
   }

   po->m_ptrDst = pITransform;
   po->m_pbmDst = pbmDst;

   return SUCCESS;
}


static int
Sprite_DrawSpritesSimple(ISprite *po, AEESpriteCmd *pCmds)
{
   uint8 unLayer;
   AEESpriteCmd *pSpriteCmds;
   int nResult = SUCCESS;

   if (pCmds == NULL) {
      nResult = EBADPARM;
      goto Done;
   }

   if (po->m_pbmDst == NULL) {
      nResult = EFAILED;
      goto Done;
   }

   for (unLayer = 0; unLayer < SPRITE_LAYER_MAX; unLayer++) {
      pSpriteCmds = pCmds;
      while (pSpriteCmds->unSpriteSize != SPRITE_SIZE_END) {
         if (pSpriteCmds->unSpriteSize < SPRITE_SIZE_MAX) {
            if (pSpriteCmds->unLayer == unLayer) {
               AEERasterOp ro;
            
               if (pSpriteCmds->unComposite == COMPOSITE_KEYCOLOR) {
                  ro = AEE_RO_TRANSPARENT;
               } else {
                  ro = AEE_RO_COPY;
               }
               
               IBITMAP_BltIn(po->m_pbmDst, pSpriteCmds->x, pSpriteCmds->y,
                             aSpriteSize[pSpriteCmds->unSpriteSize], 
                             aSpriteSize[pSpriteCmds->unSpriteSize],
                             po->m_rgSpriteBufs[pSpriteCmds->unSpriteSize],
                             0, 
                             aSpriteSize[pSpriteCmds->unSpriteSize] * pSpriteCmds->unSpriteIndex,
                             ro);
            }
         } else {
            // We'll return EBADPARM when we're done, but we still want to draw
            // the other sprites.
            nResult = EBADPARM;
         }
         pSpriteCmds++;
      }
   }

Done:
   return nResult;
}


static int
Sprite_DrawSprites(ISprite *po, AEESpriteCmd *pCmds)
{
   uint8 unLayer;
   AEESpriteCmd *pSpriteCmds;
   int nResult = SUCCESS;

   if (pCmds == NULL) {
      nResult = EBADPARM;
      goto Done;
   }

   if (po->m_ptrDst == NULL) {
      nResult = EFAILED;
      goto Done;
   }

   for (unLayer = 0; unLayer < SPRITE_LAYER_MAX; unLayer++) {
      pSpriteCmds = pCmds;
      while (pSpriteCmds->unSpriteSize != SPRITE_SIZE_END) {
         if (pSpriteCmds->unSpriteSize < SPRITE_SIZE_MAX) {
            if (pSpriteCmds->unLayer == unLayer) {
               if (pSpriteCmds->unTransform & SPRITE_MATRIX_TRANSFORM) {
                  if (po->m_pMatrices != NULL) {
                     ITRANSFORM_TransformBltComplex(po->m_ptrDst, pSpriteCmds->x, pSpriteCmds->y,
                                                    po->m_rgSpriteBufs[pSpriteCmds->unSpriteSize],
                                                    0, aSpriteSize[pSpriteCmds->unSpriteSize] * pSpriteCmds->unSpriteIndex,
                                                    aSpriteSize[pSpriteCmds->unSpriteSize], aSpriteSize[pSpriteCmds->unSpriteSize],
                                                    po->m_pMatrices + pSpriteCmds->unMatrixTransform, pSpriteCmds->unComposite);
                  } else {
                     // We'll return EBADPARM when we're done, but we still want to draw
                     // the other sprites.
                     nResult = EBADPARM;
                  }
               } else {
                  ITRANSFORM_TransformBltSimple(po->m_ptrDst, pSpriteCmds->x, pSpriteCmds->y,
                                                po->m_rgSpriteBufs[pSpriteCmds->unSpriteSize],
                                                0, aSpriteSize[pSpriteCmds->unSpriteSize] * pSpriteCmds->unSpriteIndex,
                                                aSpriteSize[pSpriteCmds->unSpriteSize], aSpriteSize[pSpriteCmds->unSpriteSize],
                                                pSpriteCmds->unTransform, pSpriteCmds->unComposite);
               }
            }
         } else {
            // We'll return EBADPARM when we're done, but we still want to draw
            // the other sprites.
            nResult = EBADPARM;
         }
         pSpriteCmds++;
      }
   }

Done:
   return nResult;
}


static int
Sprite_DrawTilesSimple(ISprite *po, AEETileMap *pMaps)
{
   AEEBitmapInfo bi;
   int nResult = SUCCESS;

   if (pMaps == NULL) {
      nResult = EBADPARM;
      goto Done;
   }

   if (po->m_pbmDst == NULL) {
      nResult = EFAILED;
      goto Done;
   }

   IBITMAP_GetInfo(po->m_pbmDst, &bi, sizeof(bi));

   for (; pMaps->pMapArray != NULL; pMaps++) {
      uint8 unTileSize = pMaps->unTileSize;
      uint16 w = pMaps->w;
      uint16 h = pMaps->h;
      IBitmap *pbmTileBuf;

      if (w > MAP_SIZE_1024 || h > MAP_SIZE_1024 || unTileSize >= TILE_SIZE_MAX) {
         nResult = EBADPARM;
         continue;
      }

      pbmTileBuf = po->m_rgTileBufs[unTileSize];

      if (pMaps->unFlags & MAP_FLAG_WRAP) {

         // Map wraps

         int i, j;
         uint32 x = (unsigned)pMaps->x;
         uint32 y = (unsigned)pMaps->y;

         int xStart = -((x & aTileSizeMask[unTileSize]) != 0);
         int xEnd = (bi.cx + aTileSize[unTileSize] - 1) / aTileSize[unTileSize];
         int yStart = -((y & aTileSizeMask[unTileSize]) != 0);
         int yEnd = (bi.cy + aTileSize[unTileSize] - 1) / aTileSize[unTileSize];

         for (j = yStart; j < yEnd; j++) {
            for (i = xStart; i < xEnd; i++) {
               uint16 unTile = pMaps->pMapArray[(1 << w) * ((j - (y >> aTileSizeBits[unTileSize])) & aMapSizeMask[h]) +
                                                ((i - (x >> aTileSizeBits[unTileSize])) & aMapSizeMask[w])];
               if ((unTile & TILE_INDEX_MASK) != TILE_INDEX_NOTHING) {
                  AEERasterOp ro;
                  
                  if (unTile & TILE_TRANSPARENT) {
                     ro = AEE_RO_TRANSPARENT;
                  } else {
                     ro = AEE_RO_COPY;
                  }

                  IBITMAP_BltIn(po->m_pbmDst,
                                i * aTileSize[unTileSize] + (x & aTileSizeMask[unTileSize]),
                                j * aTileSize[unTileSize] + (y & aTileSizeMask[unTileSize]),
                                aTileSize[unTileSize], aTileSize[unTileSize],
                                pbmTileBuf, 0, aTileSize[unTileSize] * (unTile & TILE_INDEX_MASK),
                                ro);
               }
            }
         }

      } else {

         // No wrap.  Do not draw tiles outside of tile map dimensions.

         unsigned i, j;
         int32 x = pMaps->x;
         int32 y = pMaps->y;

         if (y + (1 << (h + unTileSize + 3)) > 0 && x + (1 << (w + unTileSize + 3)) > 0) {

            unsigned xStart = x > 0 ? 0 : (-x) / aTileSize[unTileSize];
            unsigned xEnd = MIN((unsigned)1 << w, ((unsigned)(bi.cx - x + aTileSizeMask[unTileSize]) / aTileSize[unTileSize]));
            unsigned yStart = y > 0 ? 0 : (-y) / aTileSize[unTileSize];
            unsigned yEnd = MIN((unsigned)1 << h, ((unsigned)(bi.cy - y + aTileSizeMask[unTileSize]) / aTileSize[unTileSize]));

            for (j = yStart; j < yEnd; j++) {
               for (i = xStart; i < xEnd; i++) {
                  uint16 unTile = pMaps->pMapArray[(1 << w) * j + i];
                  if ((unTile & TILE_INDEX_MASK) != TILE_INDEX_NOTHING) {
                     AEERasterOp ro;
                     
                     if (unTile & TILE_TRANSPARENT) {
                        ro = AEE_RO_TRANSPARENT;
                     } else {
                        ro = AEE_RO_COPY;
                     }
                     
                     IBITMAP_BltIn(po->m_pbmDst, i * aTileSize[unTileSize] + x, j * aTileSize[unTileSize] + y,
                                   aTileSize[unTileSize], aTileSize[unTileSize],
                                   pbmTileBuf, 0, aTileSize[unTileSize] * (unTile & TILE_INDEX_MASK),
                                   ro);
                  }
               }
            }

         }

      }
   }

Done:
   return nResult;
}


static int
Sprite_DrawTiles(ISprite *po, AEETileMap *pMaps)
{
   AEEBitmapInfo bi;
   int nResult = SUCCESS;

   if (pMaps == NULL) {
      nResult = EBADPARM;
      goto Done;
   }

   if (po->m_ptrDst == NULL) {
      nResult = EFAILED;
      goto Done;
   }

   IBITMAP_GetInfo(po->m_pbmDst, &bi, sizeof(bi));

   for (; pMaps->pMapArray != NULL; pMaps++) {
      uint8 unTileSize = pMaps->unTileSize;
      uint16 w = pMaps->w;
      uint16 h = pMaps->h;
      IBitmap *pbmTileBuf;

      if (w > MAP_SIZE_1024 || h > MAP_SIZE_1024 || unTileSize >= TILE_SIZE_MAX) {
         nResult = EBADPARM;
         continue;
      }

      pbmTileBuf = po->m_rgTileBufs[unTileSize];

      if (pMaps->unFlags & MAP_FLAG_WRAP) {

         // Map wraps

         int i, j;
         uint32 x = (unsigned)pMaps->x;
         uint32 y = (unsigned)pMaps->y;

         int xStart = -((x & aTileSizeMask[unTileSize]) != 0);
         int xEnd = (bi.cx + aTileSize[unTileSize] - 1) / aTileSize[unTileSize];
         int yStart = -((y & aTileSizeMask[unTileSize]) != 0);
         int yEnd = (bi.cy + aTileSize[unTileSize] - 1) / aTileSize[unTileSize];

         for (j = yStart; j < yEnd; j++) {
            for (i = xStart; i < xEnd; i++) {
               uint16 unTile = pMaps->pMapArray[(1 << w) * ((j - (y >> aTileSizeBits[unTileSize])) & aMapSizeMask[h]) +
                                                ((i - (x >> aTileSizeBits[unTileSize])) & aMapSizeMask[w])];
               if ((unTile & TILE_INDEX_MASK) != TILE_INDEX_NOTHING) {
                  ITRANSFORM_TransformBltSimple(po->m_ptrDst,
                                                i * aTileSize[unTileSize] + (x & aTileSizeMask[unTileSize]),
                                                j * aTileSize[unTileSize] + (y & aTileSizeMask[unTileSize]),
                                                pbmTileBuf, 0, aTileSize[unTileSize] * (unTile & TILE_INDEX_MASK),
                                                aTileSize[unTileSize], aTileSize[unTileSize],
                                                (uint8)(unTile >> TILE_TRANFORM_SHIFT),
                                                (uint8)(unTile & TILE_TRANSPARENT ? COMPOSITE_KEYCOLOR : COMPOSITE_OPAQUE));
               }
            }
         }

      } else {

         // No wrap.  Do not draw tiles outside of tile map dimensions.

         unsigned i, j;
         int32 x = pMaps->x;
         int32 y = pMaps->y;

         if (y + (1 << (h + unTileSize + 3)) > 0 && x + (1 << (w + unTileSize + 3)) > 0) {

            unsigned xStart = x > 0 ? 0 : (-x) / aTileSize[unTileSize];
            unsigned xEnd = MIN((unsigned)1 << w, ((unsigned)(bi.cx - x + aTileSizeMask[unTileSize]) / aTileSize[unTileSize]));
            unsigned yStart = y > 0 ? 0 : (-y) / aTileSize[unTileSize];
            unsigned yEnd = MIN((unsigned)1 << h, ((unsigned)(bi.cy - y + aTileSizeMask[unTileSize]) / aTileSize[unTileSize]));

            for (j = yStart; j < yEnd; j++) {
               for (i = xStart; i < xEnd; i++) {
                  uint16 unTile = pMaps->pMapArray[(1 << w) * j + i];
                  if ((unTile & TILE_INDEX_MASK) != TILE_INDEX_NOTHING) {
                     ITRANSFORM_TransformBltSimple(po->m_ptrDst, i * aTileSize[unTileSize] + x, j * aTileSize[unTileSize] + y,
                                                   pbmTileBuf, 0, aTileSize[unTileSize] * (unTile & TILE_INDEX_MASK),
                                                   aTileSize[unTileSize], aTileSize[unTileSize],
                                                   (uint8)(unTile >> TILE_TRANFORM_SHIFT),
                                                   (uint8)(unTile & TILE_TRANSPARENT ? COMPOSITE_KEYCOLOR : COMPOSITE_OPAQUE));
                  }
               }
            }

         }

      }
   }

Done:
   return nResult;
}


