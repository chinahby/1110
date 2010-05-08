#ifndef  IOVERLAY_H
#define  IOVERLAY_H

/*=================================================================================
FILE:       IOverlay.h

SERVICES:   Overlay interfaces.
  
DESCRIPTION: 
      This interface provides definitions for overlay display.                          

        Copyright (c) 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/IOverlay.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

=================================================================================*/

#include "AEEInterface.h"
#include "AEEQueryInterface.h"


// temporary define for AEEBitmap.h
#ifndef IDIB_COLORSCHEME_YCBCR420 
#define IDIB_COLORSCHEME_YCBCR420 100
#endif

#define AEEIID_OVERLAY 0x01026897

/*-------------------------------------------------------------------------------*
 *                      Overlay Defines                                          *
 *-------------------------------------------------------------------------------*/

#define OVERLAY_MAX_ENUM_VALUE  0xffff

typedef enum
{
  OVERLAY_LAYER_0=0,
  OVERLAY_LAYER_1,
  OVERLAY_LAYER_2,      
  OVERLAY_LAYER_3,
  OVERLAY_ALL_LAYER = OVERLAY_MAX_ENUM_VALUE
}OverlayLayerType;

/* Overlay Flip And Rotate (FNR) Operation */
#define OVERLAY_NOP       0
#define OVERLAY_FLIP_LR   0x1  /* flip left to right */
#define OVERLAY_FLIP_UD   0x2  /* flip up and down */
#define OVERLAY_ROT_90    0x4  /* rotate 90 */
#define OVERLAY_ROT_180   (OVERLAY_FLIP_UD|OVERLAY_FLIP_LR) /* rotate 180 */
#define OVERLAY_ROT_270   (OVERLAY_ROT_90|OVERLAY_FLIP_UD|OVERLAY_FLIP_LR) /* rotate 270 */

/* Overlay Option defines */
#define OVERLAYOPT_END      0       /* end of option */
#define OVERLAYOPT_FNR      1       /* flip and rotate option */
#define OVERLAYOPT_ALPHA    2       /* alpha option */
#define OVERLAYOPT_TRANSPARENCY 3   /* Transparency option */

/*-------------------------------------------------------------------------------*
 *                      Data Types                                               *
 *-------------------------------------------------------------------------------*/

typedef struct _OverlayOpt
{
   int32  nId;   /* option id */
   int32  val;   /* option value */
} OverlayOpt;

typedef unsigned int OVERLAY_ID;

typedef struct IOverlay IOverlay;

/*===============================================================================*
 *                Overlay Interface                                              *
 *===============================================================================*/

AEEINTERFACE(IOverlay)		/* this will create struct IOverlay_Vtbl {} */
{
  INHERIT_IQueryInterface(IOverlay);
  OVERLAY_ID  (*Add) (IOverlay * po, IBitmap * pbm, AEERect *prcDst, AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts);
  OVERLAY_ID  (*Replace) (IOverlay * po, IBitmap * pbm, AEERect *prcDst, AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts, OVERLAY_ID oId);
  void        (*Remove) (IOverlay * po, OVERLAY_ID id);
  void        (*RemoveAll) (IOverlay * po, OverlayLayerType layer);
  void        (*Update) (IOverlay * po, OVERLAY_ID id);
  void        (*Enable) (IOverlay * po, OVERLAY_ID id, boolean ovOn);
  void        (*EnableAll) (IOverlay * po, boolean ovOn, OverlayLayerType layer);
  boolean     (*Verify) (IOverlay *po, OVERLAY_ID id);
  void        (*Clear) (IOverlay *po, OVERLAY_ID id);
};


/*-------------------------------------------------------------------------------*
 *                      Access Macros                                            *
 *-------------------------------------------------------------------------------*/
#define IOVERLAY_AddRef(p)                   AEEGETPVTBL(p,IOverlay)->AddRef(p)
#define IOVERLAY_Release(p)                  AEEGETPVTBL(p,IOverlay)->Release(p)
#define IOVERLAY_QueryInterface(p,a,b)       AEEGETPVTBL(p,IOverlay)->QueryInterface(p,a,b)
#define IOVERLAY_Add(p,a,b,c,d,e)            AEEGETPVTBL(p,IOverlay)->Add(p,a,b,c,d,e)
#define IOVERLAY_Replace(p,a,b,c,d,e,f)      AEEGETPVTBL(p,IOverlay)->Replace(p,a,b,c,d,e,f)
#define IOVERLAY_Remove(p,a)                 AEEGETPVTBL(p,IOverlay)->Remove(p,a)
#define IOVERLAY_RemoveAll(p,a)              AEEGETPVTBL(p,IOverlay)->RemoveAll(p,a)
#define IOVERLAY_Update(p,a)                 AEEGETPVTBL(p,IOverlay)->Update(p,a)
#define IOVERLAY_Enable(p,a,b)               AEEGETPVTBL(p,IOverlay)->Enable(p,a,b)
#define IOVERLAY_EnableAll(p,a,b)            AEEGETPVTBL(p,IOverlay)->EnableAll(p,a,b)
#define IOVERLAY_Verify(p,a)                 AEEGETPVTBL(p,IOverlay)->Verify(p,a)
#define IOVERLAY_Clear(p,a)                  AEEGETPVTBL(p,IOverlay)->Clear(p,a)

/*============================================================================
   DATA TYPES DOCUMENTATION
==============================================================================   
==============================================================================
OverlayOpt

Description:
  This structure is to compose an optional parameter to the interface.
  It holds an id-value pair.
  
Definition: 
typedef struct _OverlayOpt
{
   int32  nId;   
   int32  val;  
} OverlayOpt;


Members:
  nId:  option id
  val:  option value 

Comments:
  nId (=OVERLAYOPT_END) must be specified at the end of OverlayOpt array and val 
  member needs to be typecast to int32.
    
See Also:
  None
==============================================================================

==============================================================================
   INTERFACES DOCUMENTATION
==============================================================================
Interface Name: IOverlay

Description: 
   This interface provides the IOverlay services.  
==============================================================================

Function: IOVERLAY_AddRef

Description:
   This function increments the reference count for the IOverlay class.

Prototype:
   uint32 IOVERLAY_AddRef(IOverlay *pIOverlay);
  
Parameters:
   pIOverlay: Pointer to a IOverlay interface whose reference count
              need to be incremented.

Return Value:
   The updated reference count.

Comments:
   None.


==============================================================================

Function: IOVERLAY_Release()

Description:
   This function decrements the reference count for the 3D graphics object and 
   does appropriate cleanup if the reference count reaches zero.

Prototype:
   uint32 IOVERLAY_Release(IOverlay * pIOverlay);

Parameters:
   pIOverlay: Pointer to the IOverlay interface whose reference count needs 
   to be decremented.

Return Value:
   The updated reference count

Comments:  
   None


==============================================================================
Function: IOVERLAY_QueryInterface()

Description:
   This function retrievs a pointer to the interface you query, according to the
   input class ID. This function can be used to query an extended version of IOverlay.
   This supports compatibility between versions.

Prototype:
   int IOVERLAY_QueryInterface(IOverlay *pIOverlay, AEECLSID id, void **p); 

Parameters:
   pIOverlay [in]: Pointer to IOverlay interface.

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the value
            passed back is NULL, the interface or data that we query are not available.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments:
   If "p" passes back a NULL pointer, the data or interface that we queried is not
   available.
      
==============================================================================
Function: IOVERLAY_Add()

Description:
   This function is to add the overlay image to the specified location and layer.

Prototype:
   OVERLAY_ID  IOVERLAY_Add (IOverlay * po, IBitmap * pbm, AEERect *prcDst, AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts)

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   pbm         [in]: IBitmap pointer
   dstSrc      [in]: destination rectangular   
   prcSrc      [in]: source rectangular
   layer       [in]: layer number
   pOpts       [in]: overlay optional info array

Return Value:
   Returns OVERLAY_ID if sucessful, otherwise returns NULL

Comments:
   None
   
==============================================================================
Function: IOVERLAY_Replace()

Description:
   This function is to replace the previously added overlay with the new overlay

Prototype:
   OVERLAY_ID  IOVERLAY_Replace (IOverlay * po, IBitmap * pbm, AEERect *prcDst, AEERect *prcSrc, OverlayLayerType layer, OverlayOpt  *pOpts, OVERLAY_ID oId)

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   pbm         [in]: IBitmap pointer
   dstSrc      [in]: destination rectangular   
   prcSrc      [in]: source rectangular
   layer       [in]: layer number
   pOpts       [in]: overlay optional info array
   oId         [in]: previously added overlay id

Return Value:
   Returns OVERLAY_ID if sucessful, otherwise returns NULL

Comments:
   None
    
==============================================================================
Function: IOverlay_Remove()

Description:
   This function is to remove the overlay.

Prototype:
   void        IOVERLAY_Remove(IOverlay * po, IOVERALY_ID id);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   id          [in]: Previously added overlay id
   
Return Value:
   None

Comments:
   This function only removes the overlay from the overlay list
   This function doesn't reflect the change on the screen
   
==============================================================================
Function: IOverlay_RemoveAll()

Description:
   This function is to remove all overlays in the layer.

Prototype:
   void        IOVERLAY_RemoveAll(IOverlay * po, OverlayLayerType layer);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   layer       [in]: IOverlay layer number or IOVERLAY_ALL_LAYER for all layers
                     
   
Return Value:
   None

Comments:
   This function only removes the overlay from the overlay list
   This function doesn't reflect the change on the screen

==============================================================================
Function: IOverlay_Update()

Description:
   This function is to update the overlay on the screen

Prototype:
   void        IOVERLAY_Update(IOverlay * po, IOVERALY_ID id);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   id          [in]: Previously added overlay id
   
Return Value:
   None

Comments:
   Overlays in other layers also get reflected in this update
   
==============================================================================
Function: IOverlay_Enable()

Description:
   This function is to enable or disable the overlay.

Prototype:
   void        IOVERLAY_Enable (IOverlay * po, OVERLAY_ID id, boolean ovOn);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   id          [in]: Previously added overlay id
   ovOn        [in]: TRUE or FALSE (TRUE=Enable, FALSE=Disable)
                     
   
Return Value:
   None

Comments:
   Overlay is enabled in default.
   This function doesn't reflect the change on the screen

==============================================================================
Function: IOverlay_EnableAll()

Description:
   This function is to enable or disable all overlays in the layer

Prototype:
   void        IOVERLAY_EnableAll (IOverlay * po, boolean ovOn, OverlayLayerType layer);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   ovOn        [in]: TRUE or FALSE (TRUE=Enable, FALSE=Disable)
   layer       [in]: IOverlay layer number or IOVERLAY_ALL_LAYER for all layers   
                     
   
Return Value:
   None

Comments:
   Overlay is enabled in default.
   This function doesn't reflect the change on the screen

==============================================================================
Function: IOverlay_Verify()

Description:
   This function is to validate the overlay id

Prototype:
   boolean     IOVERLAY_Verify (IOverlay *po, OVERLAY_ID id);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   id          [in]: overlay id
                   
   
Return Value:
   TRUE if it's a valid id otherwise returns FALSE

Comments:
   None

==============================================================================
Function: IOverlay_Clear()

Description:
   This function is to clear the overlay on the screen

Prototype:
   void        IOVERLAY_Verify (IOverlay *po, OVERLAY_ID id);

Parameters:  
   po          [in]: Pointer to IOverlay interface object
   id          [in]: overlay id                
      
Return Value:
   None

Comments:
   This function doesn't remove the overlay from the list.
                        
============================================================================*/
#endif	// IOVERLAY_H
