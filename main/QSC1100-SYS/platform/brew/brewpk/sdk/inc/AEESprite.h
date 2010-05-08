#ifndef  AEESPRITE_H
#define  AEESPRITE_H
/*=================================================================================
FILE:          AEESprite.h

SERVICES:      Sprite drawing

DESCRIPTION:   This file provides definitions for the ISprite interface.  This
interface is used for drawing sprites.

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2002-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEETransform.h"
#include "AEEBitmap.h"
#include "AEE.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

// constants for sprite size field
#define SPRITE_SIZE_8X8       (0)
#define SPRITE_SIZE_16X16     (1)
#define SPRITE_SIZE_32X32     (2)
#define SPRITE_SIZE_64X64     (3)
#define SPRITE_SIZE_MAX       (4)

// For backwards compatibility, this #define with lowercase x remains.
#define SPRITE_SIZE_64x64     (3)

// special value for sprite size signaling end of command array
#define SPRITE_SIZE_END       (0xFF)

// constants for unTransform field
// To combine a flip with a rotate, xor the two values.  (This is to accommodate
// SPRITE_FLIP_Y which is SPRITE_FLIP_X | SPRITE_ROTATE_180.)
#define SPRITE_MATRIX_TRANSFORM (0x8000)

#define SPRITE_FLIP_X         TRANSFORM_FLIP_X
#define SPRITE_FLIP_Y         (TRANSFORM_FLIP_X | TRANSFORM_ROTATE_180)

#define SPRITE_ROTATE_90      TRANSFORM_ROTATE_90
#define SPRITE_ROTATE_180     TRANSFORM_ROTATE_180
#define SPRITE_ROTATE_270     TRANSFORM_ROTATE_270

#define SPRITE_SCALE_2        TRANSFORM_SCALE_2
#define SPRITE_SCALE_4        TRANSFORM_SCALE_4
#define SPRITE_SCALE_8        TRANSFORM_SCALE_8
#define SPRITE_SCALE_EIGHTH   TRANSFORM_SCALE_EIGHTH
#define SPRITE_SCALE_QUARTER  TRANSFORM_SCALE_QUARTER
#define SPRITE_SCALE_HALF     TRANSFORM_SCALE_HALF

// Constants for unLayer field
#define SPRITE_LAYER_0        (0)
#define SPRITE_LAYER_1        (1)
#define SPRITE_LAYER_2        (2)
#define SPRITE_LAYER_3        (3)
#define SPRITE_LAYER_MAX      (4)
#define SPRITE_LAYER_HIDDEN   (0xFF)

typedef struct {
   // These are the screen coordinate where the upper left corner of the 
   // sprite will be drawn.
   int16 x;
   int16 y;

   // Transformation
   //    bit 15: if set, do matrix transformation instead of
   //            simple transformations
   //    bits 5-3: scaling (see SPRITE_SCALE_ constants above)
   //    bit 2: if set, reflect over x-axis
   //    bits 1-0: rotation: 0 = 0 degrees, 1 = 90, 2 = 180, 3 = 270.
   //    all other bits reserved
   uint16 unTransform;

   // Index into the transformation matrix table.
   // This field is only relevant if bit 15 is set in unTransform.
   uint8 unMatrixTransform;

   // The upper left corner coordinate of the sprite in the source buffer
   // is determined by x = 0, y = unSpriteIndex * h, where h is the height
   // in pixels of the sprite.
   uint8 unSpriteIndex;

   // Constants to be defined for 8x8, 16x16, etc.  Only square sprites are
   // supported for now.
   uint8 unSpriteSize;

   // 0x00 for transparent, 0xFF for opaque, all other value reserved
   uint8 unComposite;

   // Valid values are 0-3.
   // Lower layers are draw first.
   // Special value of 0xFF mean don't draw this sprite.
   uint8 unLayer;

   // Reserved for future use.
   // These bytes MUST be set to zero.
   uint8 reserved[5];
} AEESpriteCmd;



// constants for tile flags
#define TILE_TRANFORM_SHIFT   (13)

#define TILE_FLIP_X           (TRANSFORM_FLIP_X << TILE_TRANFORM_SHIFT)
#define TILE_FLIP_Y           ((TRANSFORM_FLIP_Y | TRANSFORM_ROTATE_180) << TILE_TRANFORM_SHIFT)

#define TILE_ROTATE_90        (TRANSFORM_ROTATE_90 << TILE_TRANFORM_SHIFT)
#define TILE_ROTATE_180       (TRANSFORM_ROTATE_180 << TILE_TRANFORM_SHIFT)
#define TILE_ROTATE_270       (TRANSFORM_ROTATE_270 << TILE_TRANFORM_SHIFT)

#define TILE_TRANSPARENT      (0x1000)

#define TILE_INDEX_MASK       (0x03FF) // mask of bits used for tile index
#define TILE_INDEX_NOTHING    (0x03FF) // This index mean don't draw anything.
                                       // This is functionally equivalent to blitting
                                       // a tile that is completely transparent, but
                                       // this way is faster.

// constants for tile size field
#define TILE_SIZE_8X8         (0x00)
#define TILE_SIZE_16X16       (0x01)
#define TILE_SIZE_32X32       (0x02)
#define TILE_SIZE_64X64       (0x03)
#define TILE_SIZE_MAX         (4)

// Flags for tile maps.
// If MAP_FLAG_WRAP is set, map should wrap around.
// For instance, if a map is 16 tiles wide, column 0 would be displayed
// after column 15.
#define MAP_FLAG_WRAP         (0x00000001)

#define MAP_SIZE_1            (0)
#define MAP_SIZE_2            (1)
#define MAP_SIZE_4            (2)
#define MAP_SIZE_8            (3)
#define MAP_SIZE_16           (4)
#define MAP_SIZE_32           (5)
#define MAP_SIZE_64           (6)
#define MAP_SIZE_128          (7)
#define MAP_SIZE_256          (8)
#define MAP_SIZE_512          (9)
#define MAP_SIZE_1024         (10)

typedef struct {
   // Array of indices and other info.  Along with w and h, this specifies a
   //    two dimensional array that determines the relative locations where
   //    tiles will be drawn.
   // bits 15-13: reflect/rotate tile (identical to bits 2-0 in unTransform
   //    member of AEESpriteCmd)
   // bit 12: if set, blit tile transparently
   // bit 11-10: reserved
   // bits 9-0: index into tile buffer
   // NULL pointer signifies that this is end of array of backgrounds.
   uint16 *pMapArray;

   // currently only flag for wrap
   uint32 unFlags;

   // Reserved for future use.
   // These MUST be set to zero.
   // (Rotation info will probably go here in the future.)
   uint32 reserved[4];

   // These are the screen coordinate where the upper left corner of the 
   // background will be drawn.
   int32 x;
   int32 y;

   // Dimensions of tile map, specified units of tiles.
   uint16 w;
   uint16 h;

   // Constants to be defined for 8x8, 16x16, etc.
   uint8 unTileSize;

   // Reserved for future use.
   // These bytes MUST be set to zero.
   uint8 reserved2[3];

} AEETileMap;



typedef struct ISprite ISprite;


//=================================================================================
// FUNCTION DEFINITIONS and INLINE FUNCTION DEFINITIONS 
//=================================================================================


//*********************************************************************************
//
// ISprite Interface
//
//*********************************************************************************

AEEINTERFACE(ISprite)
{
   INHERIT_IQueryInterface(ISprite);

   // Sets the array that will be used for complex transforms.  This array is
   // indexed by the unMatrixTransform field of AEESpriteCmd.
   int   (*SetTransformTable) (ISprite *po, AEETransformMatrix *pTransformTable);

   // Sprites are stored separately based on their sizes. This function sets the
   // bitmap for each size of sprite.  The unSpriteSize value has the same format
   // as the unSpriteSize member of SpriteCmd0.  This buffer contains sprites of
   // the same size, in a vertical column.  For instance, if the sprite size
   // specified by unSpriteSize is 8x8, this bitmap is 8 pixels wide and 8*n pixes
   // high, where n is the number of sprites in the buffer.
   int   (*SetSpriteBuffer)   (ISprite *po, uint8 unSpriteSize, IBitmap *pbmSpriteBuffer);

   // This function sets the bitmap that contains the tiles of a particular size.
   // This works just like SetSpriteBuffer.
   int   (*SetTileBuffer)     (ISprite *po, uint8 unTileSize, IBitmap *pbmTileBuffer);

   int   (*SetDestination)    (ISprite *po, IBitmap *pbmDst);

   // Draw the sprites.
   // pCmds is an array of command structs
   int   (*DrawSprites)       (ISprite *po, AEESpriteCmd *pCmds);

   // Draw a set of tile maps.
   // pMap is an array of maps to draw in order.  There must be an extra entry
   // at the end of the array with the pMapArray member set to NULL.
   int   (*DrawTiles)         (ISprite *po, AEETileMap *pMaps);
};


// Access macros for ISprite
#define  ISPRITE_AddRef(p)                AEEGETPVTBL(p,ISprite)->AddRef(p)
#define  ISPRITE_Release(p)               AEEGETPVTBL(p,ISprite)->Release(p)
#define  ISPRITE_QueryInterface(p,i,p2)   AEEGETPVTBL(p,ISprite)->QueryInterface(p,i,p2)
#define  ISPRITE_SetTransformTable(p,t)   AEEGETPVTBL(p,ISprite)->SetTransformTable(p,t)
#define  ISPRITE_SetSpriteBuffer(p,s,b)   AEEGETPVTBL(p,ISprite)->SetSpriteBuffer(p,s,b)
#define  ISPRITE_SetTileBuffer(p,s,b)     AEEGETPVTBL(p,ISprite)->SetTileBuffer(p,s,b)
#define  ISPRITE_SetDestination(p,b)      AEEGETPVTBL(p,ISprite)->SetDestination(p,b)
#define  ISPRITE_DrawSprites(p,c)         AEEGETPVTBL(p,ISprite)->DrawSprites(p,c)
#define  ISPRITE_DrawTiles(p,m)           AEEGETPVTBL(p,ISprite)->DrawTiles(p,m)



/*=================================================================================
   DATA STRUCTURE DOCUMENTATION
===================================================================================
AEESpriteCmd

Description:
   This struct describes a sprite.

Definition:
   typedef struct {
      int16    x;
      int16    y;
      uint16   unTransform;
      uint8    unMatrixTransform;
      uint8    unSpriteIndex;
      uint8    unSpriteSize;
      uint8    unComposite;
      uint8    unLayer;
      uint8    reserved[5];
   } AEESpriteCmd;

Members:
   x, y:              Coordinate in destination where upper left corner of non-
                      transformed sprite will be drawn.
   unTransform:       Contains a combination of sprite transformation properties.
   unMatrixTransform: Index of transform in transform table.  A matrix transform
                      is only applied if the SPRITE_MATRIX_TRANSFORM flag is set in
                      the unTransform field.
   unSpriteIndex:     Index of sprite image to draw.  The upper left corner of
                      the source image is x = 0, y = unSpriteIndex * [sprite height].
   unSpriteSize:      Size of sprite.  See SPRITE_SIZE_ properties for valid values.
   unComposite:       Specfied whether sprite should be drawn opaquely or
                      transparently.  See Transform properties.
   unLayer:           Layer of sprite.  Lower layers are drawn first.  See
                      SPRITE_LAYER_ properties.
   reserved:          Reservered for future use.  It is VERY important that these
                      be set to zero.

Comments:
   None

See Also:
   ISPRITE_DrawSprites(), AEETransformMatrix, Sprite properties, Transform properties

===================================================================================
AEETileMap

Description:
   This struct describes a tile map.

Definition:
   typedef struct {
      uint16  *pMapArray;
      uint32   unFlags;
      uint32   reserved[4];
      int32    x;
      int32    y;
      uint16   w;
      uint16   h;
      uint8    unTileSize;
      uint8    reserved2[3];
   } AEETileMap;

Members:
   pMapArray:  Array of tile indices and properties.  This is a one-dimensional
               representation of the two-dimensional map.  The rows of the map
               are unwraped to make this one-dimensional array.  (The first
               element of the second row follows the last element of the first
               row.)  The bottom ten bits of each element are used for the index
               into the tile buffer.  (TILE_INDEX_MASK masks these bits.)  The
               special index value of TILE_INDEX_NOTHING means "don't draw a tile
               here."  The TILE_FLIP_, TILE_ROTATE_, and TILE_TRANSPARENT flags
               are applied to the elements of this array.

               The special value of pMapArray == NULL signifies that this is the
               last AEETileMap structure in the array passed to ISPRITE_DrawTiles().
               In this case, the other members of the AEETileMap structure are
               ignored.
   unFlags:    Flags that apply to the entire tile map.  Currently, only
               MAP_FLAG_WRAP is defined.
   reserved:   Reserved for future use.  It is VERY important that these be set
               to zero.
   x, y:       These are the screen coordinate where the upper left corner of the 
               background will be drawn.
   w, h:       Dimensions of tile map, specified in units of tiles.  These are not
               the literal dimensions.  Instead, the MAP_SIZE_ values are used
               here.
   unTileSize: Size of the tiles.  This is one of the TILE_SIZE_ values.
   reserved2:  Reserved for future use.  It is VERY important that these be set
               to zero.

Comments:
   None

See Also:
   ISPRITE_DrawTiles(), Tile properties, Tile map properties

===================================================================================
Sprite properties

Description:
   These properties apply to each sprite in the AEESpriteCmd structure.

Definition:
   SPRITE_SIZE_8X8
   SPRITE_SIZE_16X16
   SPRITE_SIZE_32X32
   SPRITE_SIZE_64x64
   SPRITE_SIZE_MAX
      Sprite sizes, for unSpriteSize field.

   SPRITE_SIZE_END 
      Special value for unSpriteSize signaling end of command array.

   SPRITE_FLIP_X
   SPRITE_FLIP_Y
   SPRITE_ROTATE_90
   SPRITE_ROTATE_180
   SPRITE_ROTATE_270
   SPRITE_SCALE_2
   SPRITE_SCALE_4
   SPRITE_SCALE_8
   SPRITE_SCALE_EIGHTH
   SPRITE_SCALE_QUARTER
   SPRITE_SCALE_HALF
      Simple transformation flags for unTransform field.  These may be combined.
      To combine a flip with a rotate, xor the two values.  (This is to
      accommodate SPRITE_FLIP_Y which is SPRITE_FLIP_X | SPRITE_ROTATE_180.)

   SPRITE_MATRIX_TRANSFORM
      This flags goes in unTransform, and specifies that a complex transform
      should be performed using the unMatrixTransform field.  When this flag
      is set, all other flags in unTransform are ignored.

   SPRITE_LAYER_0
   SPRITE_LAYER_1
   SPRITE_LAYER_2
   SPRITE_LAYER_3
   SPRITE_LAYER_HIDDEN
      Valid layers for unLayer field.

Comments:
   Note: All transformation use the sprite's center as the origin.

See Also:
   AEESpriteCmd

===================================================================================
Tile properties

Description:
   These flags apply to each tile, in the elements of the pMapArray array
   field of AEETileMap.

Definition:
   TILE_FLIP_X          Flip tile over X axis.
   TILE_FLIP_Y          Flip tile over Y axis.  (Note: this is a composite of FLIP_X
                        and ROTATE_180.)
   TILE_ROTATE_90       Rotate tile 90 degrees counter clockwise.
   TILE_ROTATE_180      Rotate tile 180 degrees counter clockwise.
   TILE_ROTATE_270      Rotate tile 270 degrees counter clockwise.
   TILE_TRANSPARENT     Tile should be drawn transparently.

   TILE_INDEX_MASK      Mask of bits used for tile index.
   TILE_INDEX_NOTHING   This special index value means don't draw anything for
                        this tile.  This is functionally equivalent to blitting a
                        tile that is completely transparent, but this is faster.

Comments:
   None

See Also:
   AEETileMap

===================================================================================
Tile map properties

Description:
   These properties apply to each tile map, in the AEETileMap structure.

Definition:
   TILE_SIZE_8X8
   TILE_SIZE_16X16
   TILE_SIZE_32X32
   TILE_SIZE_64X64
      Values for unTileSize field.

   MAP_FLAG_WRAP
      Flag for unFlags field.  (Currently the only flag.)
      If MAP_FLAG_WRAP is set, map should wrap around.  For instance, if a map is
      16 tiles wide, column 0 would be displayed after column 15.

   MAP_SIZE_1
   MAP_SIZE_2
   MAP_SIZE_4
   MAP_SIZE_8
   MAP_SIZE_16
   MAP_SIZE_32
   MAP_SIZE_64
   MAP_SIZE_128
   MAP_SIZE_256
   MAP_SIZE_512
   MAP_SIZE_1024
      Values for w and h fields.

Comments:
   None

See Also:
   AEETileMap

===================================================================================
   INTERFACE DOCUMENTATION
===================================================================================
ISprite Interface

Description:
   The ISprite interface is used to render sprites and tile maps.

   A sprite is simply a bitmap that has various additional properties associated
   with it.  These properties include the location where the sprite will be drawn,
   transformations, and transparency.  Sprites are useful in games for drawing
   animated characters and objects that move about the screen.  Sprites are drawn
   with the ISPRITE_DrawSprites() function, and ISPRITE_SetSpriteBuffer() is used to
   set the bitmaps that contain the sprite images.

   A tile map can be thought of as a 2D matrix of bitmaps (tiles), all of the same
   dimensions, drawn with no space between them.  A tile map is represented as an
   array of indices.  The location in the array determines where the tile will
   be drawn, and the index stored at that location in the array determines which
   tile will be drawn.  There are also additional properties associated with each
   tile location, such as transparency and simple transformations, as well as
   properties associated with the entire tile map, such as drawing offset, tile
   size, and wrapping.  Tile maps are useful in games for drawing backgrounds.
   A small tile set could be used to create a large and complex map or background.
   Tile maps are drawn with the ISPRITE_DrawTiles() function, and
   ISPRITE_SetTileBuffer() is used to set the bitmaps that contain the tile sets.

   ISPRITE_SetTransformTable() is used to set the table that will be used for
   applying arbitrary affine transformations to sprites.

   ISPRITE_SetDestination() sets the destination bitmap that will be used for all
   drawing operations.


==============================================================================
Function: ISPRITE_AddRef()

Description:
This function is inherited from IQI_AddRef().

==============================================================================
Function: ISPRITE_Release()

Description:
This function is inherited from IQI_Release().

==============================================================================
Function: ISPRITE_QueryInterface()

Description:
This function is inherited from IQI_QueryInterface().

==============================================================================
Function: ISPRITE_SetTransformTable()

Description:
   This function is used to tell the sprite engine where to lookup
   transformations for sprites with the SPRITE_MATRIX_TRANSFORM attribute set.
   The unMatrixTransform field of AEESpriteCmd specifies which table entry to
   use.

Prototype:
   int ISPRITE_SetTransformTable(ISprite *po, AEETransformMatrix *pTransformTable);

Parameters:
   po              [in]: Pointer to ISprite interface.

   pTransformTable [in]: Array of AEETransformMatrix structures.

Return Value:
   SUCCESS: if successful.

Comments:  
   The sprite engine does not make a copy of the table, nor will it ever
   try to free the table.  The caller is responsible to deallocating the table's
   memory at the correct time.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISPRITE_DrawSprites(), AEESpriteCmd

==============================================================================
Function: ISPRITE_SetSpriteBuffer()

Description:
   This function is used to tell the sprite engine where to find sprite
   images for a particular size of sprite.  The sprite engine maintains a
   separate buffer for each sprite size.  The bitmap buffer contains the
   sprite images arranged in a single column.  A value of 0 in the
   unSpriteIndex field of AEESpriteCmd refers to the topmost image in the
   buffer.  In the case of 16x16 sprites, index 0 refers to the 16x16 sqaure
   with the upper left hand corner at (0, 0).  Index 1 refers to the 16x16
   square with the upper left hand corner at (0, 16).

   Passing NULL for pbmSpriteBuffer will force the sprite engine to release
   the existing sprite buffer.

Prototype:
   int ISPRITE_SetSpriteBuffer(ISprite *po, uint8 unSpriteSize, IBitmap *pbmSpriteBuffer);

Parameters:
   po              [in]: Pointer to ISprite interface.

   unSpriteSize    [in]: Size of sprites in buffer.  Corresponds to
                         unSpriteSize field of AEESpriteCmd.

   pbmSpriteBuffer [in]: Bitmap to be used for sprite buffer.

Return Value:
   SUCCESS: if successful.
   EBADPARM: if unSpriteSize is invalid.

Comments:
   The sprite engine will call AddRef and Release appropriately on the
   bitmap.  This way, the caller is free to call Release on the bitmap and
   forget about it.  The bitmap will be deallocated when the sprite engine
   is done with the bitmap.

   The sprite buffers should be the same type of bitmaps as the destination
   bitmap.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISPRITE_SetTileBuffer(), ISPRITE_DrawSprites(), AEESpriteCmd

==============================================================================
Function: ISPRITE_SetTileBuffer()

Description:
   This function is used to tell the sprite engine where to find tile
   images for a particular size of tile.  The sprite engine maintains a
   separate buffer for each tile size.  The tiles are arranged in the bitmap
   buffer in a single column, just like sprite buffers.  Tile indexes also
   are used to locate tiles just like sprite indexes are used to locate
   sprites.

   Passing NULL for pbmTileBuffer will force the sprite engine to release
   the existing tile buffer.

Prototype:
   int ISPRITE_SetTileBuffer(ISprite *po, uint8 unTileSize, IBitmap *pbmTileBuffer);

Parameters:
   po              [in]: Pointer to ISprite interface.

   unTileSize      [in]: Size of tiles in buffer.  Corresponds to
                         unTileSize field of AEETileMap.

   pbmTileBuffer   [in]: Bitmap to be used for tile buffer.

Return Value:
   SUCCESS: if successful.
   EBADPARM: if unSpriteSize is invalid.

Comments:  
   The sprite engine will call AddRef and Release appropriately on the
   bitmap.  This way, the caller is free to call Release on the bitmap and
   forget about it.  The bitmap will be deallocated when the sprite engine
   is done with the bitmap.

   The tile buffers should be the same type of bitmaps as the destination
   bitmap.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISPRITE_SetSpriteBuffer(), ISPRITE_DrawTiles(), AEETileMap

==============================================================================
Function: ISPRITE_SetDestination()

Description:
   This function is used to tell the sprite engine where to render sprites
   and tiles.  ISPRITE_DrawSprites() and ISPRITE_DrawTiles both use this
   bitmap for the destination.

   Passing NULL for pbmDst will force the sprite engine to release the
   existing destination bitmap.

Prototype:
   int ISPRITE_SetDestination(ISprite *po, IBitmap *pbmDst);

Parameters:
   po     [in]: Pointer to ISprite interface.

   pbmDst [in]: Bitmap to be used for destination in drawing operations.

Return Value:
   SUCCESS: if successful.

Comments:  
   The sprite engine will call AddRef and Release appropriately on the
   bitmap.  This way, the caller is free to call Release on the bitmap and
   forget about it.  The bitmap will be deallocated when the sprite engine
   is done with the bitmap.

   The destination bitmap must support the ITranform interface in order
   for any sort of transformation to be applied.

   Typically, the destination bitmap is either the device bitmap or another
   bitmap that is compatible with the device bitmap.

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISPRITE_DrawSprites(), ISPRITE_DrawTiles()

==============================================================================
Function: ISPRITE_DrawSprites()

Description:
   This function causes the sprites in the pCmds array to be drawn.  The
   sprite engine will iterate through the array in order four times.  The
   first pass will only draw sprites that have the unLayer field set to 0.
   Subsequent passes will draw layers 1, 2, and 3, respectively.  The array
   is terminated by a dummy entry with unSpriteSize set to SPRITE_SIZE_END.
   All sprites are drawn to the bitmap specified by ISPRITE_SetDestination().

Prototype:
   int ISPRITE_DrawSprites(ISprite *po, AEESpriteCmd *pCmds);

Parameters:
   po     [in]: Pointer to ISprite interface.

   pCmds  [in]: Array of sprite commands.

Return Value:
   SUCCESS: if successful.
   EBADPARM: if pCmds is NULL.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISPRITE_SetDestination(), ISPRITE_SetTransformTable(), AEESpriteCmd

==============================================================================
Function: ISPRITE_DrawTiles()

Description:
   This functions causes the tile maps in the pMaps array to be drawn.  Each
   entry in the array describes one tile map.  The sprite engine will iterate
   through the array in order one time.  The array is terminated by a dummy
   entry with the pMapArray field set to NULL.  All tiles are drawn to the
   bitmap specified by ISPRITE_SetDestination().

Prototype:
   int ISPRITE_DrawTiles(ISprite *po, AEETileMap *pMaps);

Parameters:
   po     [in]: Pointer to ISprite interface.

   pMaps  [in]: Array of tile maps.

Return Value:
   SUCCESS: if successful.
   EBADPARM: if pCmds is NULL.

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.0

See Also:
   ISPRITE_SetDestination(), AEETileMap

=================================================================================*/



#endif /* AEESPRITE_H */

