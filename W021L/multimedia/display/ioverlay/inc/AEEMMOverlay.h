/*
================================================================================

  FILE:  AEEMMOverlay.h
  
  SERVICES: Defines the MMOverlayMgr/MMOverlay interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2006 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/AEEMMOverlay.h#3 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $
    
================================================================================
===============================================================================
*/
#ifndef __AEEMMOVERLAY_H__
#define __AEEMMOVERLAY_H__

#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEStdLib.h"

////////////////////////////////////////////////////////////////////////////////
// MMOverlay Interface IDs

#define AEEIID_MMOVERLAYMGR        0x0103a204
#define AEEIID_MMOVERLAY    	     0x0103a203


////////////////////////////////////////////////////////////////////////////////
// MMOverlay data structures

#define NOT_MATCH_ENABLED  0x0001  //Do not match Enabled Overlays
#define NOT_MATCH_DISABLED 0x0002  //Do not match Disabled Overlays

//MMOverlay Layer Type
#define OV_LAYER_BASE_LAYER 0x00
#define OV_LAYER_TOP_LAYER 0xFF
typedef uint32 MMOverlayLayerType;

//Value that describes which enum to use
enum
{
   OV_UNION_TYPE_UINT,
   OV_UNION_TYPE_FIXEDPOINT,
   OV_UNION_TYPE_PTR,
   OV_UNION_TYPE_LAYER,
   OV_UNION_TYPE_RGB,
   OV_UNION_TYPE_CLSID,
   OV_UNION_TYPE_NOTYPE
};

typedef uint8 OverlayMemberType;

#define OVERLAY_PARAM_CONTINUOUS 0x0000
#define OVERLAY_PARAM_DISCRETE   0x0001

typedef struct
{
   uint16 wFlags;
   uint32 dwValues;
   OverlayMemberType OverlayUnionMember;
   uint32 * pdwParams;
} OverlayCapabilitiesSubType;

typedef struct
{
   uint16 wParams;
   OverlayCapabilitiesSubType paramInfo[1];
} OverlayCapabilitiesType;

//OverlayParamType definition
#define OV_PARM_BITMAP          0x0103ab72
#define OV_PARM_SRC_REGION      0x0103ab73

#define OV_PARM_DEST_REGION     0x0103ab74
    #define OV_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT   0x00000000
    #define OV_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT  0x00000001
    #define OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER 0x00000002
    #define OV_FLAG_DEST_REGION_ALIGN_VERT_UPPER   0x00000000
    #define OV_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM  0x00010000
    #define OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER  0x00020000

#define OV_PARM_DISPLAY         0x0103ab75
#define OV_PARM_LAYER           0x0103ab76
#define OV_PARM_TRANSPARENCY    0x0103ab77
#define OV_PARM_ALPHA_BLEND_PCT 0x0103ab78
#define OV_PARM_ENABLE          0x0103ab7a
#define OV_PARM_AUTO_DISABLE    0x0103ab7b
#define OV_PARM_ROTATE          0x0103ab7c

#define OV_PARM_REFLECT         0x0103ab7d
    #define OV_FLAG_REFLECT_X_AXIS    0x00000001
    #define OV_FLAG_REFLECT_Y_AXIS    0x00010000

#define OV_PARM_SCALE_X         0x0103ab7e
#define OV_PARM_SCALE_Y         0x0103ab7f
    #define OV_FLAG_SCALE_TO_FIT      0x00000001
    #define OV_FLAG_SCALE_KEEP_ASPECT 0x00000002
    #define OV_FLAG_SCALE_NO_CROP     0x00000004

#define OV_PARM_DEST_REGION_SCREEN 0x0103ab80
#define OV_PARM_VSYNC           0x0103ab81
#define OV_PARM_MINIMIZE_CPU    0x0103ab82
#define OV_PARM_SYSTEM_OVERLAY  0x010415f8
#define OV_PARM_NO_AUTOCLEAR    0x010419c0
#define OV_PARM_ALLOW_NEGATIVE_REGION  0x01070993
#define OV_PARM_EDGE_SCALING    0x0107357b

typedef uint32 OverlayParamType;

//Model events and data types
#define EVT_MDL_OV_PARAMETER_CHANGED          (EVT_MDL_DEV_START + 0x1000)
#define EVT_MDL_OV_CHANGES_COMMITTED          (EVT_MDL_DEV_START + 0x1003)
#define EVT_MDL_OV_ASYNC_UPDATE_COMPLETE      (EVT_MDL_DEV_START + 0x1004)

typedef struct OverlayAsyncUpdateCompleteType
{
   uint32 dwUser;
   uint32 dwUnique;
   int    nErr;
} OverlayAsyncUpdateCompleteType;

typedef struct OverlayParamChangedType
{
   OverlayParamType nParamID;
   uint32 p1;
   uint32 p2;
} OverlayParamChangedType;

////////////////////////////////////////////////////////////////////////////////
// IMMOverlay interface


#define INHERIT_IMMOverlay(iname) \
   INHERIT_IQueryInterface(iname); \
   int        (*GetCapabilities) (iname *p, OverlayParamType paramID, \
                                            int32 * pSize, \
                                            OverlayCapabilitiesType * pBuff); \
   int        (*SetParam)        (iname *p, OverlayParamType paramID, \
                                            uint32 p1, \
                                            uint32 p2); \
   int        (*GetParam)        (iname *p, OverlayParamType paramID, \
                                            uint32 * pP1, \
                                            uint32 * pP2); \
   int        (*Update)          (iname *p, const AEERect *pDirtyRect); \
   uint32     (*UpdateAsync)     (iname *p, const AEERect *pDirtyRect, uint32 dwUser); \
   int        (*Clear)           (iname *p)


AEEINTERFACE_DEFINE(IMMOverlay);

static __inline uint32 IMMOVERLAY_AddRef(IMMOverlay * pMe)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->AddRef(pMe);
}

static __inline uint32 IMMOVERLAY_Release(IMMOverlay * pMe)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->Release(pMe);
}

static __inline int IMMOVERLAY_QueryInterface(IMMOverlay * pMe, AEECLSID cls, 
                                              void** ppo)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->QueryInterface(pMe, cls, ppo);
}

static __inline int IMMOVERLAY_GetCapabilities(IMMOverlay * pMe,
                                               OverlayParamType paramID,
                                               int32 * pSize,
                                               OverlayCapabilitiesType * pBuff)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->GetCapabilities(pMe, paramID, pSize, 
                                                         pBuff);
}

static __inline int IMMOVERLAY_SetParam(IMMOverlay * pMe,
                                        OverlayParamType paramID, 
                                        uint32 p1, 
                                        uint32 p2)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->SetParam(pMe, paramID, p1, p2);
}

static __inline int IMMOVERLAY_GetParam(IMMOverlay * pMe,
                                        OverlayParamType paramID, 
                                        uint32 * pP1, 
                                        uint32 * pP2)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->GetParam(pMe, paramID, pP1, pP2);
}

static __inline int IMMOVERLAY_Update(IMMOverlay * pMe, const AEERect *prc)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->Update(pMe, prc);
}

static __inline uint32 IMMOVERLAY_UpdateAsync(IMMOverlay * pMe, const AEERect *prc, uint32 dwUser)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->UpdateAsync(pMe, prc, dwUser);
}

static __inline int IMMOVERLAY_Clear(IMMOverlay * pMe)
{
   return AEEGETPVTBL((pMe),IMMOverlay)->Clear(pMe);
}

////////////////////////////////////////////////////////////////////////////////
// IMMOverlayMgr interface


#define INHERIT_IMMOverlayMgr(iname) \
   INHERIT_IQueryInterface(iname); \
   int  (*CreateOverlay) (iname *p, IBitmap * pBmp, AEECLSID display, \
                                                    AEEPoint destPoint, \
                                                    IMMOverlay **ppOverlay); \
   int  (*GetCapabilities) (iname *p, OverlayParamType paramID, \
                                      int32 * pSize, \
                                      OverlayCapabilitiesType * pBuff)

AEEINTERFACE_DEFINE(IMMOverlayMgr);

static __inline uint32 IMMOVERLAYMGR_AddRef(IMMOverlayMgr * pMe)
{
   return AEEGETPVTBL((pMe),IMMOverlayMgr)->AddRef(pMe);
}

static __inline uint32 IMMOVERLAYMGR_Release(IMMOverlayMgr * pMe)
{
   return AEEGETPVTBL((pMe),IMMOverlayMgr)->Release(pMe);
}

static __inline int IMMOVERLAYMGR_QueryInterface(IMMOverlayMgr * pMe, AEECLSID cls, 
                                                 void** ppo)
{
   return AEEGETPVTBL((pMe),IMMOverlayMgr)->QueryInterface(pMe, cls, ppo);
}

static __inline int IMMOVERLAYMGR_CreateOverlay(IMMOverlayMgr * pMe, 
                                                IBitmap * pBmp,
                                                AEECLSID display,
                                                AEEPoint destPoint,
                                                IMMOverlay **ppOverlay)
{
   return (int) AEEGETPVTBL((pMe),IMMOverlayMgr)->CreateOverlay(pMe, pBmp, display, 
                                                          destPoint, ppOverlay);
}

static __inline int IMMOVERLAYMGR_GetCapabilities(IMMOverlayMgr * pMe,
                                                  OverlayParamType paramID,
                                                  int32 * pSize,
                                                  OverlayCapabilitiesType * pBuff)
{
   return AEEGETPVTBL((pMe),IMMOverlayMgr)->GetCapabilities(pMe, paramID, pSize, 
                                                            pBuff);
}


/*
================================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================
OverlayCapabilitiesType

Description:
   This data type is used to return the list of all valid values for a
   given parameter ID to IMMOVERLAY_SetParam.
   
   
Definition:
typedef struct
{
   uint16 wParams;
   OverlayCapabilitiesSubType paramInfo[1];
} OverlayCapabilitiesType;

Members:
===pre>
   wParams  :  Number of parameters that are defined for a given parameter ID.
               The expected value for this can be found in the documentation
               for IMMOVERLAY_SetParam().
   paramInfo:  Array of size wParams of OverlayCapabilitiesSubType. This type
               describes the possible values for each parameter.

===/pre>

Comments:

See Also:
   IMMOVERLAYMGR_GetCapabilities()
   IMMOVERLAY_GetCapabilities()
   IMMOVERLAY_SetParam()
   OverlayCapabilitiesSubType

================================================================================
OverlayCapabilitiesSubType

Description:
   This data type is used to return the list of all valid values for a
   given parameter ID to IMMOVERLAY_SetParam.
   
   
Definition:
typedef struct
{
   uint16 wFlags;
   uint32 dwValues;
   OverlayMemberType OverlayMember;
   uint32 * pdwParams;
} OverlayCapabilitiesSubType;

Members:
===pre>
   wFlags            : Flag for each parameter that is defined for a given 
                       parameter ID. Current valid values are
                       OVERLAY_PARAM_CONTINUOUS if the values for this parameter
                       are represented as every value from x-y, and
                       OVERLAY_PARAM_DISCRETE if every valid value is returned.

   dwValues          : Number of values that are supported by each parameter for
                       a given parameter ID.

   OverlayMember     : What type is this parameter (fixed point, uint, pointer)

   pdwParams         : Pointer to an Array of all of the valid values for each
                       parameter for a given parameter ID.
===/pre>

Comments:

See Also:
   IMMOVERLAYMGR_GetCapabilities()
   IMMOVERLAY_GetCapabilities()
   IMMOVERLAY_SetParam()
   OverlayCapabilitiesType

================================================================================
OverlayParamType

Description:
   This data type is used to specify the parameter to get/set.
   
   
Definition:

typedef uint32 OverlayParamType;

Members:
===pre>
   None.
===/pre>

Comments:
This table describes the parameters that can be set/get through this interface.

nParamID              Ops         nParams              p1/p2
                             (GetCapabilities)
--------------------------------------------------------------------------------
OV_PARM_BITMAP       Get/Set        N/A                  p1
                                                 Pointer to IBitmap
                                                 (Can not be retrieved if you
                                                  are not the owner)

                                                         p2
                                                         N/A

OV_PARM_SRC_REGION   Get/Set        N/A                  p1
                                                 Rectangle describing the region
                                                 of the bitmap to display.
                                                 Default: Entire Rectangle

                                                         p2
                                                         N/A

OV_PARM_DEST_REGION  Get/Set        N/A                  p1
                                                 Rectangle describing the region
                                                 to display on the device. 
                                                 These coordinates are in the 
                                                 applications space.
                                                 If dx/dy are set to zero, the
                                                 image will not be cropped.

                                                 Default: x/y Set by create.
                                                          dx/dy = zero

                                                         p2
                                                 Flags the describe how to align
                                                 the image in rectangle 
                                                 described in p1.

                                                 Default: 
                                           OV_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT|
                                           OV_FLAG_DEST_REGION_ALIGN_VERT_UPPER

OV_PARM_DISPLAY      Get/Set/Caps    1                   p1
                                                 ClassID of the display
                                                 Default: Set by create.

                                                         p2
                                                         N/A

OV_PARM_LAYER        Get/Set/Caps    1                   p1
                                                 (Type MMOverlayLayerType)Layer ID
                                                 Default: OV_LAYER_TOP_LAYER

                                                         p2
                                                         N/A

OV_PARM_TRANSPARENCY Get/Set/Caps    2                   p1
                                                 Specifies if the color
                                                 specified in p2 should be
                                                 made transparent.
                                                 1 = Transparency On
                                                 0 = Transparency Off
                                                 Default: 0

                                                         p2
                                                 (Type RGBVAL) specifies the
                                                 color to be made transparent.
                                                 Default: RGB_MASK_COLOR

OV_PARM_ALPHA_BLEND_PCT
                     Get/Set/Caps    1                   p1
                                                 (Type fixed point)
                                                 (This is such that 256 = 1.0)
                                                 Specifies alpha percentage of
                                                 this image.
                                                 0.0 is equal to 0% and
                                                 1.0 is equal to 100%
                                                 Default: 1.0

                                                         p2
                                                         N/A

OV_PARM_ENABLE       Get/Set        N/A                  p1
                                                 Specifies if the overlay is
                                                 enabled. If the overlay is
                                                 disabled, then it will not be
                                                 visable on the screen.
                                                 This parameter behaves 
                                                 differently than other
                                                 parameters in that changes to
                                                 it are reflected before the
                                                 next IMMOVERLAY_Update.
                                                 1 = Overlay Enabled
                                                 0 = Overlay Disabled
                                                 Default: 1

                                                         p2
                                                         N/A

OV_PARM_AUTO_DISABLE Get/Set        N/A                  p1
                                                 (Protected Parameter)
                                                 If this parameter is set to 1
                                                 then the overlay will be
                                                 disabled if the application 
                                                 that created it is not the top 
                                                 visable state.
                                                 When the application is brought
                                                 back to top visable then the
                                                 overlay will be restored to its
                                                 previous state.
                                                 1 = Auto-Disable on
                                                 0 = Auto-Disable off
                                                 Default: 1

                                                         p2
                                                         N/A

OV_PARM_ROTATE       Get/Set/Caps    1                   p1
                                                 (Type uint) Specifies the 
                                                 amount to rotate the image in
                                                 the clockwise direction.
                                                 Default:0

                                                         p2
                                                         N/A

OV_PARM_REFLECT      Get/Set/Caps    1                   p1
                                                 Specifies if the image should
                                                 be reflected around a given 
                                                 axis. Value is a bitmask.
                                                 OV_FLAG_REFLECT_X_AXIS: Flip 
                                                    over the x-axis.
                                                 OV_FLAG_REFLECT_Y_AXIS: Flip
                                                    over the y-axis.
                                                 Default: 0

                                                         p2
                                                         N/A

OV_PARM_SCALE_X      Get/Set/Caps    2                   p1
OV_PARM_SCALE_Y      Get/Set/Caps    2           Bitmask that modifies the scale
                                                 operation.

                                                 OV_FLAG_SCALE_TO_FIT: Sets the
                                                 scaling factor to the maximium
                                                 that will fit in the
                                                 destination region.

                                                 OV_FLAG_SCALE_KEEP_ASPECT: Sets
                                                 the scaling factor of the other
                                                 dimension to be the same as the
                                                 one specified.

                                                 OV_FLAG_SCALE_NO_CROP: If
                                                 either dimension of the
                                                 destination region would be
                                                 exceeded by this operation,
                                                 the scaling factor will be
                                                 reduced to prevent this.

                                                 Default: 
                                                 OV_FLAG_SCALE_KEEP_ASPECT|
                                                 OV_FLAG_SCALE_TO_FIT|
                                                 OV_FLAG_SCALE_NO_CROP

                                                         p2
                                                 (Type fixed point)
                                                 (This is such that 256 = 1.0)
                                                 The scaling factor
                                                 for this dimension. If
                                                 OV_FLAG_SCALE_TO_FIT is set
                                                 this value is meaningless.

                                                 Default: 1.0


OV_PARM_DEST_REGION_SCREEN Get        N/A                  p1
                                                 Rectangle describing the region
                                                 to display on the device. 
                                                 These coordinates are in the 
                                                 screen space.
                                                 If dx/dy are set to zero, the
                                                 image will not be cropped.

                                                         p2
                                                 Rectangle describing the region
                                                 the the application is using for
                                                 the screen

OV_PARM_VSYNC     Get/Set/Caps       1                   p1

                                                 If VSync is enabled the physical
                                                 display will synchronize updates
                                                 with the screen.

                                                 0: VSync Disabled
                                                 1: VSync Enabled

                                                 Default: 0

                                                         p2
                                                         N/A

OV_PARM_MINIMIZE_CPU  Get/Set/Caps   1                   p1

                                                 If Minimize CPU is enabled the
                                                 Overlay will modify behavior
                                                 to reduce CPU utilization.
                                                 This may cause updates to take
                                                 longer, and may alter the
                                                 capabilities of the Overlay

                                                 0: Minimize CPU Disabled
                                                 1: Minimize CPU Enabled

                                                 Default: 1

                                                         p2
                                                         N/A

OV_PARM_SYSTEM_OVERLAY  Get/Set/Caps   1                 p1

                                                 If system overlay is enabled the
                                                 Overlay will modify behavior
                                                 of IDisplay_Update to instead
                                                 update this Overlay if it is
                                                 enabled. Multiple MMOverlays
                                                 can be marked as system overlays.
                                                 Only Overlays that refer to the
                                                 device bitmap should be marked
                                                 as system.

                                                 0: Not a system overlay
                                                 1: System Overlay

                                                 Default: 0

                                                         p2
                                                         N/A

OV_PARM_NO_AUTOCLEAR Get/Set/Caps    1                   p1

                                                 If this value is set to 1 then
                                                 the Overlay will not clear
                                                 itself when the destination
                                                 region changes

                                                 0: AutoClear Enabled
                                                 1: AutoClear Disabled

                                                 Default: 0

                                                         p2
                                                         N/A

OV_PARM_ALLOW_NEGATIVE_REGION Get/Set/Caps    1          p1

                                                 If this value is set to 1 then
                                                 the Overlay will not restrict itself
                                                 to positive destaination regions.

                                                 0: Negative regions not allowed
                                                 1: Negative regions allowed.

                                                 Default: 0

                                                         p2
                                                         N/A


OV_PARM_EDGE_SCALING          Get/Set/Caps    1          p1

                                                 If this value is set to 1 then
                                                 the edge scaling will be used if
                                                 the system supports it with the
                                                 current scaling factors.

                                                 0: Edge scaling will not be used.
                                                 1: Edge scaling will be used..

                                                 Default: 0

                                                         p2
                                                         N/A


See Also:
   IMMOVERLAYMGR_GetCapabilities()
   IMMOVERLAY_GetCapabilities()
   IMMOVERLAY_SetParam()
   IMMOVERLAY_GetParam()
   IMMOVERLAY_Update()

================================================================================
OverlayParamChangedType

Description:
   A pointer to this data type will be used as the dwParam of 
   EVT_MDL_OV_PARAMETER_CHANGED.


Definition:
typedef struct
{
   OverlayParamType nParamID;
   uint32 p1;
   uint32 p2;
} OverlayParamChangedType;


Members:
===pre>
nParamID :  Parameter we are interested in.
p1       :  Depends on nParamID.
p2       :  Depends on nParamID.
===/pre>

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IMMOverlay supported parameters. This will provide more information on
   p1 and p2.

See Also:
   IMMOVERLAY_SetParam()
   IMMOVERLAY_GetParam()
   OverlayParamType

================================================================================
OverlayAsyncUpdateCompleteType

Description:
   A pointer to this data type will be used as the dwParam of 
   EVT_MDL_OV_ASYNC_UPDATE_COMPLETE.


Definition:
typedef struct OverlayAsyncUpdateCompleteType
{
   uint32 dwUser;
   uint32 dwUnique;
   int    nErr;
} OverlayAsyncUpdateCompleteType;



Members:
===pre>
dwUser   :  The user ID that was passed to the IMMOVERLAY_UpdateAsync Function.
dwUnique :  The unique update ID that returned from the IMMOVERLAY_UpdateAsync 
            function.
nErr     :  The status of the update operation.
===/pre>

Comments:  
   None.

See Also:
   IMMOVERLAY_UpdateAsync()

================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IMMOverlayMgr Interface

Description:
   IMMOverlayMgr is the interface for managing overlays. It provides 
   functionality to create new overlays, and to get the capabilities of the
   overlay system.

Required header files:
   None.
   
================================================================================

IMMOVERLAYMGR_AddRef()

This function is inherited from IQI_AddRef().

================================================================================

IMMOVERLAYMGR_Release()

This function is inherited from IQI_Release().

================================================================================

IMMOVERLAYMGR_QueryInterface()

This function is inherited from IQI_QueryInterface(). 

================================================================================

IMMOVERLAYMGR_CreateOverlay()

Description: 
   This function creates an overlay based on the parameters specified.

Prototype:

   int IMMOVERLAYMGR_CreateOverlay(IMMOverlayMgr * pMe, 
                                   IBitmap * pBmp,
                                   AEECLSID display,
                                   AEEPoint destPoint,
                                   IMMOverlay **ppOverlay);

Parameters:
===pre>
   p      :  Pointer to the IMMOverlayMgr interface object.
   pBmp   :  Pointer to the IBitmap interface that will be displayed
   display:  ClassID of the display that this overlay will be created on.
   destPoint: Where on the display should this overlay be placed. This
              specifies where the upper left corner of the bitmap
              will be placed.
   ppOverlay: Pointer to an IMMOverlay pointer that will be initalized in this
              call
===/pre>

Return Value:
   SUCCESS          - Operation successful
   ENOMEMORY        - Insufficient memory
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The Overlay that is returned will have some of its parameters set. See the
   documentation for IMMOVERLAY_SetParam for more information.

Side Effects: 
   None

See Also:
   IMMOVERLAY_SetParam()
================================================================================

IMMOVERLAYMGR_GetCapabilities()

Description: 
   This function retrieves the list of possible values for a parameter to
   IMMOVERLAY_SetParam. These values are to give an idea of what the device
   supports. An overlay may not be able to set all of these values at any given
   time. The function IMMOVERLAY_GetCapabilities() will serve this function.

Prototype:

   int IMMOVERLAYMGR_GetCapabilities(IMMOverlayMgr *p, OverlayParamType paramID,
                                       int32 * pSize,
                                       OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlayMgr interface object.
   paramID  :  Parameter we are interested in.
   pSize    :  Pointer to a variable the will return the size of the buffer
               needed.
   pBuff    :  Pointer to a buffer to fill the possible values in. If this is
               NULL pSize will be set, but the buffer will not be used.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IMMOverlayMgr supported capability parameters.

Side Effects: 
   None

See Also:
   IMMOVERLAY_GetCapabilities()
   IMMOVERLAY_SetParam()
   IMMOVERLAY_GetParam()
   
================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IMMOverlay Interface

Description:
   IMMOverlay is the interface for contolling an overlay. An overlay represents
   one IBitmap that has been placed onto the screen.

   Usage:
   The first thing that is needed to create an IMMOverlay is to create an
   instance of IMMOverlayMgr. Once you have the IMMOverlayMgr object that is 
   needed to call IMMOVERLAYMGR_CreateOverlay. This function takes a bitmap,
   a class ID of a display and a point representing where to place the upper
   left corner of the bitmap on the screen. The rest of the parameters will be
   set at their default values.

   If the default values are acceptable the only other step that is needed is 
   to call IMMOVERLAY_Update to display the image on the screen.

   If the default values are not desired the user will have to call 
   IMMOVERLAY_SetParam to change them to the desired values.

   Fixed point varibles are implemented such that 1.0 = 256

   Example Usage:

   //External varibles
   IShell *pShell;
   IBitmap *pBitmap;
   IMMOverlay *pOverlay;

   //Function
   IMMOverlayMgr *pOverlayMgr;

   //Create an IMMOVERLAYMGR
   if(SUCCESS == 
   ISHELL_CreateInstance(pShell, AEECLSID_MMOVERLAYMGR, (void **)&pOverlayMgr))
   {
      //Create an overlay
      AEEPoint destPoint;
      destPoint.x = 0;
      destPoint.y = 0; //Create in upper left corner of screen

      if(SUCCESS == IMMOVERLAYMGR_CreateOverlay(pOverlayMgr,
                                 pBitmap, //The bitmap that will be displayed
                                 AEECLSID_DISPLAY1, //Place overlay on display 1
                                 destPoint,
                                 &pOverlay))
      {
         //Modify parameters

         //Change to be set on the background layer
         IMMOVERLAY_SetParam(pOverlay, OV_PARM_LAYER, OV_LAYER_BASE_LAYER, 0);

         //Change Alpha to 50% (128)
         IMMOVERLAY_SetParam(pOverlay, OV_PARM_ALPHA_BLEND_PCT, 128, 0);

         //Update the Overlay, displays on the screen
         IMMOVERLAY_Update(pOverlay, NULL);
      }
   }


   If the user is displaying a video clip on an overlay the typical use will be
   to create the overlay on the first frame and then modify only the
   OV_PARM_BITMAP parameter on subsequent frames and then call 
   IMMOVERLAY_Update();

Required header files:
   None.
   
================================================================================

IMMOVERLAY_AddRef()

This function is inherited from IQI_AddRef().

================================================================================

IMMOVERLAY_Release()

This function is inherited from IQI_Release().

================================================================================

IMMOVERLAY_QueryInterface()

This function is inherited from IQI_QueryInterface(). 

================================================================================

IMMOVERLAY_GetCapabilities()

Description: 
   This function retrieves the list of possible values for a parameter to
   IMMOVERLAY_SetParam. These values represent what the given overlay supports at
   the current moment. The values that are supported can change at any time.

Prototype:

   int IMMOVERLAY_GetCapabilities(IMMOverlay *p, OverlayParamType paramID,
                                             int32 * pSize,
                                             OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   paramID  :  Parameter we are interested in.
   pSize    :  Pointer to a variable the will return the size of the buffer
               needed.
   pBuff    :  Pointer to a buffer to fill the possible values in. If this is
               NULL pSize will be set, but the buffer will not be used.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IMMOverlay supported capability parameters.

Side Effects: 
   None

See Also:
   IMMOVERLAYMGR_GetCapabilities()
   IMMOVERLAY_SetParam()
   IMMOVERLAY_GetParam()

================================================================================

IMMOVERLAY_SetParam()

Description: 
   This function sets the specified IMMOverlay Interface object parameter. 

Prototype:

   int IMMOVERLAY_SetParam(IMMOverlay *p, OverlayParamType paramID,
                                      uint32 p1, uint32 p2);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   paramID  :  Parameter we are interested in.
   p1       :  Depends on paramID.
   p2       :  Depends on paramID.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IMMOverlay supported settable parameters. Only the app that created
   this overlay, apps that are in the group that was set by the owner, or system
   level apps can change the overlay parameters. With the exeception of 
   OV_PARM_ENABLE the parameter change will not take affect until the next
   IMMOVERLAY_Update.

Side Effects: 
   If the values that are specified are valid, but can not be set at the current
   time, the closest value that is possible will be used and the listeners
   specified in the IModel interface of IMMOverlay will be called.

See Also:
   IMMOVERLAY_GetCapabilities()
   IMMOVERLAY_GetParam()

================================================================================

IMMOVERLAY_GetParam()

Description: 
   This function gets the specified IMMOverlay Interface object parameter.

Prototype:

   int IMMOVERLAY_GetParam(IMMOverlay *p, OverlayParamType paramID,
                                      uint32 * pP1,
                                      uint32 * pP2);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   paramID  :  Parameter we are interested in.
   pP1      :  Depends on paramID.
   pP2      :  Depends on paramID.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IMMOverlay supported gettable parameters.

Side Effects: 
   None

See Also:
   IMMOVERLAY_GetCapabilities()
   IMMOVERLAY_SetParam()

================================================================================

IMMOVERLAY_Update()

Description: 
   This function updates the IMMOverlay on the screen. This is an alternative
   update path to IDISPLAY_Update and using both paths in an application may
   cause undesired behavior.

Prototype:

   int IMMOVERLAY_Update(IMMOverlay *p, const AEERect * pRect);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   pRect    :  Pointer to the rectangle that describes the dirty rectangle.
               If the dx/dy values are 0 the entire Overlay will be redrawn.
               These coordinates are relative to the Bitmap that is passed in.
               If this parameter is NULL the entire bitmap will be updated.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   This function also commits parameter changes that have occured due to
   calls to IMMOVERLAY_SetParam().
   After the first update the overlay will be registered with the overlay
   subsystem and updates to other IMMOVERLAYs may cause this this overlay
   to be partially redrawn. Therefore, caution should be exercised when
   modifying a bitmap that is registered with an IMMOVERLAY.

Side Effects: 
   None.

See Also:
   IMMOVERLAY_UpdateAsync()
   IMMOVERLAY_SetParam()

================================================================================

IMMOVERLAY_UpdateAsync()

Description: 
   This function updates the IMMOverlay asynchronously on the screen. 

Prototype:

   uint32 IMMOVERLAY_UpdateAsync(IMMOverlay *p, const AEERect *pRect,
                                 uint32 dwUser);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
   pRect    :  Pointer to the rectangle that describes the dirty rectangle.
               If the dx/dy values are 0 the entire Overlay will be redrawn.
               These coordinates are relative to the Bitmap that is passed in.
               If this parameter is NULL the entire bitmap will be updated.
   dwUser   :  A user specified value. This will only be passed to the listener
               event for this update.
===/pre>

Return Value:
   A unique ID assigned for this update. If this value is 0xFFFFFFFF an error
   occured and the update will not occur.

Comments:  
   This function also commits parameter changes that have occured due to
   calls to IMMOVERLAY_SetParam().
   After the first update the overlay will be registered with the overlay
   subsystem and updates to other IMMOVERLAYs may cause this this overlay
   to be partially redrawn. Therefore, caution should be exercised when
   modifying a bitmap that is registered with an IMMOVERLAY.

Side Effects: 
   This call will result cause the listeners specified in the 
   IModel interface of IMMOverlay will be called with the event 
   EVT_MDL_OV_ASYNC_UPDATE_COMPLETE.

See Also:
   IMMOVERLAY_SetParam()
   IMMOVERLAY_Update()
================================================================================

IMMOVERLAY_Clear()

Description: 
   This function clears the IMMOverlay from the screen. The overlay will still
   be registered and if another update is called the overlay may be partially
   redrawn

Prototype:

   int IMMOVERLAY_Clear(IMMOverlay *p);

Parameters:
===pre>
   p        :  Pointer to the IMMOverlay interface object.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EFAILED          - Generic failure

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None.

================================================================================
*/

#endif /* __AEEMMOVERLAY_H__ */
