/*
================================================================================

  FILE:  IOverlay2.h
  
  SERVICES: Defines the OverlayMgr/Overlay interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/inc/IOverlay2.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

================================================================================
===============================================================================
*/
#ifndef __IOVERLAY2_H__
#define __IOVERLAY2_H__



#include "AEE.h"
#include "AEEBitmap.h"
#include "AEEModel.h"


////////////////////////////////////////////////////////////////////////////////
// Overlay Interface IDs

#define AEEIID_OVERLAY2MGR        0x0102f8f5
#define AEEIID_OVERLAY2    	      0x0102edf1
#define AEEIID_OVERLAY2_DIRECT    0x0102edf2


////////////////////////////////////////////////////////////////////////////////
// Overlay data structures

typedef struct IOverlay2Mgr IOverlay2Mgr;
typedef struct IOverlay2 IOverlay2;

#define NOT_MATCH_ENABLED  0x0001  //Do not match Enabled Overlays
#define NOT_MATCH_DISABLED 0x0002  //Do not match Disabled Overlays

//Overlay Layer Type
#define OV2_LAYER_BASE_LAYER 0x00
#define OV2_LAYER_TOP_LAYER 0xFF
typedef uint32 OverlayLayerType2;

//Data structure to specify enumerate parameters
typedef struct
{
   AEECLSID *         pDisplayList; //A list of displays to match 
   uint32             nDisplays;    //Number of displays in the list
   OverlayLayerType2 * pLayerList;   //A list of Layers to match
   uint32             nLayers;      //Number of Layers in the list
   AEECLSID *         pClassList;   //A list of applications to match
   uint32             nClasses;     //Number of application in the list
   uint32 *           pIDList;      //A list of Overlay IDs to match
   uint32             nIDs;         //Number of Overlay IDs in the list
   uint32             dwFlags;      //Flags for additional search parameters
} OverlayEnumInfoType2;


typedef union
{
   uint32 uint_data;
   float  float_data;
   void * ptr_data;
   OverlayLayerType2 layer_data;
   RGBVAL rgb_data;
   AEECLSID clsid_data;
} OverlayTypeUnion2;

//Value that describes which enum to use
enum
{
   OV2_UNION_TYPE_UINT,
   OV2_UNION_TYPE_FLOAT,
   OV2_UNION_TYPE_PTR,
   OV2_UNION_TYPE_LAYER,
   OV2_UNION_TYPE_RGB,
   OV2_UNION_TYPE_CLSID,
   OV2_UNION_TYPE_NOTYPE
};

typedef uint8 OverlayUnionMemberType2;

#define OVERLAY_PARAM_CONTINUOUS 0x0000
#define OVERLAY_PARAM_DISCRETE   0x0001

typedef struct
{
   uint16 wFlags;
   uint32 dwValues;
   OverlayUnionMemberType2 OverlayUnionMember2;
   OverlayTypeUnion2 * pdwParams2;
} OverlayCapabilitiesSubType2;

typedef struct
{
   uint16 wParams;
   OverlayCapabilitiesSubType2 paramInfo2[1];
} OverlayCapabilitiesType2;

//OverlayParamType definition
#define OV2_PARM_BITMAP          1
#define OV2_PARM_SRC_REGION      2

#define OV2_PARM_DEST_REGION     3
    #define OV2_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT   0x00000000
    #define OV2_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT  0x00000001
    #define OV2_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER 0x00000002
    #define OV2_FLAG_DEST_REGION_ALIGN_VERT_UPPER   0x00000000
    #define OV2_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM  0x00010000
    #define OV2_FLAG_DEST_REGION_ALIGN_VERT_CENTER  0x00020000

#define OV2_PARM_DISPLAY         4
#define OV2_PARM_LAYER           5
#define OV2_PARM_TRANSPARENCY    6
#define OV2_PARM_ALPHA_BLEND_PCT 7
#define OV2_PARM_LETTER_BOX      8
#define OV2_PARM_ENABLE          9
#define OV2_PARM_AUTO_DISABLE    10
#define OV2_PARM_ROTATE          11

#define OV2_PARM_REFLECT         12
    #define OV2_FLAG_REFLECT_X_AXIS    0x00000001
    #define OV2_FLAG_REFLECT_Y_AXIS    0x00010000

#define OV2_PARM_SCALE_X         13
#define OV2_PARM_SCALE_Y         14
    #define OV2_FLAG_SCALE_TO_FIT      0x00000001
    #define OV2_FLAG_SCALE_KEEP_ASPECT 0x00000002
    #define OV2_FLAG_SCALE_NO_CROP     0x00000004

#define OV2_PARM_OWNER           15
#define OV2_PARM_GROUPID         16
#define OV2_PARM_OVERLAYID       17
#define OV2_PARM_DEST_REGION_SCREEN 18
#define OV2_PARM_VSYNC           19
#define OV2_PARM_BACK_BITMAP     20
#define OV2_PARM_NO_AUTOCLEAR    21
#define OV2_PARAM_MAX_VALUE      22

typedef uint32 OverlayParamType2;

//This value will be used
#define OV2_SETTYPE_HARD  0x0000
//This value will be used unless we have an OV2_SETTYPE_HARD
#define OV2_SETTYPE_SOFT  0x0001
//This causes the state of if an OV2_SETTYPE_HARD to be reset
#define OV2_SETTYPE_RESET 0x0002
//This is used to prod the interface, will bypass these checks 
#define OV2_SETTYPE_SILENT 0x0004


//Model events and data types
#define EVT_MDL_OV2_PARAMETER_CHANGED          (EVT_MDL_DEV_START + 0x1000)
#define EVT_MDL_OV2_PARAMETER_CHANGED_INDIRECT (EVT_MDL_DEV_START + 0x1001)
#define EVT_MDL_OV2_PARAMETER_CHANGED_SUGGEST  (EVT_MDL_DEV_START + 0x1002)
#define EVT_MDL_OV2_CHANGES_COMMITTED          (EVT_MDL_DEV_START + 0x1003)
#define EVT_MDL_OV2_ASYNC_UPDATE_COMPLETE      (EVT_MDL_DEV_START + 0x1004)

typedef struct OverlayAsyncUpdateCompleteType2
{
   uint32 dwUser;
   uint32 dwUnique;
   int    nErr;
} OverlayAsyncUpdateCompleteType2;

typedef struct OverlayParamChangedType2
{
   OverlayParamType2 nParamID2;
   OverlayTypeUnion2 p1;
   OverlayTypeUnion2 p2;
} OverlayParamChangedType2;

////////////////////////////////////////////////////////////////////////////////
// IOverlay2Mgr interface


#define INHERIT_IOverlay2Mgr(iname) \
   INHERIT_IQueryInterface(iname); \
   IOverlay2 * (*CreateOverlay) (iname *p, IBitmap * pBmp, AEECLSID display, \
                                                          AEEPoint destPoint); \
   int        (*EnumInit)      (iname *p, OverlayEnumInfoType2 * pEnumInfo2); \
   IOverlay2 * (*EnumNext)      (iname *p); \
   int        (*GetCapabilities) (iname *p, OverlayParamType2 paramID2, \
                                            uint32 * pSize, \
                                            OverlayCapabilitiesType2 * pBuff2)

AEEINTERFACE(IOverlay2Mgr) {
   INHERIT_IOverlay2Mgr(IOverlay2Mgr);
};

static __inline int IOVERLAY2MGR_AddRef(IOverlay2Mgr * pMe)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->AddRef(pMe);
}

static __inline int IOVERLAY2MGR_Release(IOverlay2Mgr * pMe)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->Release(pMe);
}

static __inline int IOVERLAY2MGR_QueryInterface(IOverlay2Mgr * pMe, AEECLSID cls, 
                                               void** ppo)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->QueryInterface(pMe, cls, ppo);
}

static __inline IOverlay2 * IOVERLAY2MGR_CreateOverlay(IOverlay2Mgr * pMe, 
                                                     IBitmap * pBmp,
                                                     AEECLSID display,
                                                     AEEPoint destPoint)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->CreateOverlay(pMe, pBmp, display, 
                                                        destPoint);
}

static __inline int IOVERLAY2MGR_EnumInit(IOverlay2Mgr * pMe, 
                                         OverlayEnumInfoType2 * pEnumInfo2)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->EnumInit(pMe, pEnumInfo2);
}

static __inline IOverlay2 * IOVERLAY2MGR_EnumNext(IOverlay2Mgr * pMe)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->EnumNext(pMe);
}

static __inline int IOVERLAY2MGR_GetCapabilities(IOverlay2Mgr * pMe,
                                                OverlayParamType2 paramID2,
                                                uint32 * pSize,
                                                OverlayCapabilitiesType2 * pBuff2)
{
   return AEEGETPVTBL((pMe),IOverlay2Mgr)->GetCapabilities(pMe, paramID2, pSize, 
                                                          pBuff2);
}

////////////////////////////////////////////////////////////////////////////////
// IOverlay2 interface


#define INHERIT_IOverlay2(iname) \
   INHERIT_IQueryInterface(iname); \
   int        (*GetCapabilities) (iname *p, OverlayParamType2 paramID2, \
                                            uint32 * pSize, \
                                            OverlayCapabilitiesType2 * pBuff2); \
   int        (*SetParam)        (iname *p, OverlayParamType2 paramID2, \
                                            OverlayTypeUnion2 p12, \
                                            OverlayTypeUnion2 p22, \
                                            uint8 nSetType); \
   int        (*GetParam)        (iname *p, OverlayParamType2 paramID2, \
                                            OverlayTypeUnion2 * pP12, \
                                            OverlayTypeUnion2 * pP22); \
   int        (*Update)          (iname *p, AEERect dirtyRect); \
   int        (*Clear)           (iname *p); \
   int        (*AddListener)     (iname *p, ModelListener *pListener); \
   uint32     (*UpdateAsync)     (iname *p, AEERect dirtyRect, uint32 dwUser)


AEEINTERFACE(IOverlay2) {
   INHERIT_IOverlay2(IOverlay2);
   
};

static __inline int IOVERLAY2_AddRef(IOverlay2 * pMe)
{
   return AEEGETPVTBL((pMe),IOverlay2)->AddRef(pMe);
}

static __inline int IOVERLAY2_Release(IOverlay2 * pMe)
{
   return AEEGETPVTBL((pMe),IOverlay2)->Release(pMe);
}

static __inline int IOVERLAY2_QueryInterface(IOverlay2 * pMe, AEECLSID cls, 
                                               void** ppo)
{
   return AEEGETPVTBL((pMe),IOverlay2)->QueryInterface(pMe, cls, ppo);
}

static __inline int IOVERLAY2_GetCapabilities(IOverlay2 * pMe,
                                             OverlayParamType2 paramID2,
                                             uint32 * pSize,
                                             OverlayCapabilitiesType2 * pBuff2)
{
   return AEEGETPVTBL((pMe),IOverlay2)->GetCapabilities(pMe, paramID2, pSize, 
      pBuff2);
}

static __inline int IOVERLAY2_SetParam(IOverlay2 * pMe,
                                      OverlayParamType2 paramID2, 
                                      OverlayTypeUnion2 p1, 
                                      OverlayTypeUnion2 p2)
{
   return AEEGETPVTBL((pMe),IOverlay2)->SetParam(pMe, paramID2, p1, p2, 
                                              OV2_SETTYPE_HARD);
}

static __inline int IOVERLAY2_SetParamSuggested(IOverlay2 * pMe,
                                               OverlayParamType2 paramID2, 
                                               OverlayTypeUnion2 p1, 
                                               OverlayTypeUnion2 p2, 
                                               boolean bReset)
{
   uint8 nSetType = (bReset?OV2_SETTYPE_RESET:0)|OV2_SETTYPE_SOFT;
   return AEEGETPVTBL((pMe),IOverlay2)->SetParam(pMe, paramID2, p1, p2, nSetType);
}
                                         
static __inline int IOVERLAY2_GetParam(IOverlay2 * pMe,
                                      OverlayParamType2 paramID2, 
                                      OverlayTypeUnion2 * pP1, 
                                      OverlayTypeUnion2 * pP2)
{
   return AEEGETPVTBL((pMe),IOverlay2)->GetParam(pMe, paramID2, pP1, pP2);
}

static __inline int IOVERLAY2_Update(IOverlay2 * pMe, AEERect rc)
{
   return AEEGETPVTBL((pMe),IOverlay2)->Update(pMe, rc);
}

static __inline int IOVERLAY2_Clear(IOverlay2 * pMe)
{
   return AEEGETPVTBL((pMe),IOverlay2)->Clear(pMe);
}

static __inline int IOVERLAY2_AddListener(IOverlay2 * pMe, 
                                         ModelListener *pListener)
{
   return AEEGETPVTBL((pMe), IOverlay2)->AddListener(pMe, pListener);
}

static __inline uint32 IOVERLAY2_UpdateAsync(IOverlay2 * pMe, AEERect rc, uint32 dwUser)
{
   return AEEGETPVTBL((pMe),IOverlay2)->UpdateAsync(pMe, rc, dwUser);
}



#endif /* __IOVERLAY2_H__ */

/*
================================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================
OverlayEnumInfoType

Description:
   This data type specifies the structure that is used to restict the scope of
   what is returned by the enumerate operation. Anything that is not specified
   is considered a match all input. The intersection of all specified parameters
   is what will be matched.
   
   
Definition:
typedef struct
{
   AEECLSID *         pDisplayList; //A list of displays to match 
   uint32             nDisplays;    //Number of displays in the list
   OverlayLayerType * pLayerList;   //A list of Layers to match
   uint32             nLayers;      //Number of Layers in the list
   AEECLSID *         pClassList;   //A list of applications to match
   uint32             nClasses;     //Number of application in the list
   uint32 *           pIDList;      //A list of Overlay IDs to match
   uint32             nIDs;         //Number of Overlay IDs in the list
   uint32             dwFlags;      //Flags for additional search parameters
} OverlayEnumInfoType;


Members:
===pre>
   pDisplayList:          A list of displays to match 
   nDisplays:             Number of displays in the list
   pLayerList:            A list of Layers to match
   nLayers:               Number of Layers in the list
   pClassList:            A list of applications to match
   nClasses:              Number of application in the list
   pIDList:               A list of Overlay IDs to match
   nIDs:                  Number of Overlay IDs in the list
   dwflags:               Flags for additional search parameters
===/pre>

Comments:

See Also:
   IOVERLAY2MGR_EnumInit()

================================================================================
OverlayCapabilitiesType

Description:
   This data type is used to return the list of all valid values for a
   given parameter ID to IOVERLAY_SetParam.
   
   
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
               for IOVERLAY_SetParam().
   paramInfo:  Array of size wParams of OverlayCapabilitiesSubType. This type
               describes the possible values for each parameter.

===/pre>

Comments:

See Also:
   IOVERLAY2MGR_GetCapabilities()
   IOVERLAY_GetCapabilities()
   IOVERLAY_SetParam()
   OverlayCapabilitiesSubType

================================================================================
OverlayCapabilitiesSubType

Description:
   This data type is used to return the list of all valid values for a
   given parameter ID to IOVERLAY_SetParam.
   
   
Definition:
typedef struct
{
   uint16 wFlags;
   uint32 dwValues;
   OverlayUnionMemberType OverlayUnionMember;
   OverlayTypeUnion * pdwParams;
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

   OverlayUnionMember: What type is this parameter (float, uint, pointer)

   pdwParams         : Pointer to an Array of all of the valid values for each
                       parameter for a given parameter ID.
===/pre>

Comments:

See Also:
   IOVERLAY2MGR_GetCapabilities()
   IOVERLAY_GetCapabilities()
   IOVERLAY_SetParam()
   OverlayCapabilitiesType

================================================================================
OverlayParamType

Description:
   This data type is used to specify the parameter to get/set.
   
   
Definition:
#define OV2_PARM_BITMAP          1
#define OV2_PARM_SRC_REGION      2

#define OV2_PARM_DEST_REGION     3
    #define OV2_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT   0x00000000
    #define OV2_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT  0x00000001
    #define OV2_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER 0x00000002
    #define OV2_FLAG_DEST_REGION_ALIGN_VERT_TOP     0x00000000
    #define OV2_FLAG_DEST_REGION_ALIGN_VERT_RIGHT   0x00010000
    #define OV2_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM  0x00020000

#define OV2_PARM_DISPLAY         4

#define OV2_PARM_LAYER           5
   #define OV2_LAYER_BG 0x00
   #define OV2_LAYER_FG 0xFF

#define OV2_PARM_TRANSPARENCY    6
#define OV2_PARM_ALPHA_BLEND_PCT 7
#define OV2_PARM_LETTER_BOX      8
#define OV2_PARM_ENABLE          9
#define OV2_PARM_AUTO_DISABLE    10
#define OV2_PARM_ROTATE          11

#define OV2_PARM_REFLECT         12
    #define OV2_FLAG_REFLECT_X_AXIS    0x00000001
    #define OV2_FLAG_REFLECT_Y_AXIS    0x00010000

#define OV2_PARM_SCALE_X         13
#define OV2_PARM_SCALE_Y         14
    #define OV2_FLAG_SCALE_TO_FIT      0x00000001
    #define OV2_FLAG_SCALE_KEEP_ASPECT 0x00000002
    #define OV2_FLAG_SCALE_NO_CROP     0x00000004

#define OV2_PARM_OWNER           15
#define OV2_PARM_GROUPID         16
#define OV2_PARM_OVERLAYID       17
#define OV2_PARM_MAX_VALUE       17


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
OV2_PARM_BITMAP       Get/Set        N/A                  p1
                                                 Pointer to IBitmap
                                                 (Can not be retrieved if you
                                                  are not the owner)

                                                         p2
                                                         N/A

OV2_PARM_SRC_REGION   Get/Set        N/A                  p1
                                                 Rectangle describing the region
                                                 of the bitmap to display.
                                                 Default: Entire Rectangle

                                                         p2
                                                         N/A

OV2_PARM_DEST_REGION  Get/Set        N/A                  p1
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
                                           OV2_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT|
                                           OV2_FLAG_DEST_REGION_ALIGN_VERT_UPPER

OV2_PARM_DISPLAY      Get/Set/Caps    1                   p1
                                                 ClassID of the display
                                                 Default: Set by create.

                                                         p2
                                                         N/A

OV2_PARM_LAYER        Get/Set/Caps    1                   p1
                                                 (Type OverlayLayerType)Layer ID
                                                 Default: OV2_LAYER_TOP_LAYER

                                                         p2
                                                         N/A

OV2_PARM_TRANSPARENCY Get/Set/Caps    2                   p1
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

OV2_PARM_ALPHA_BLEND_PCT
                     Get/Set/Caps    1                   p1
                                                 (Type Float)
                                                 Specifies alpha percentage of
                                                 this image.
                                                 0.0 is equal to 0% and
                                                 1.0 is equal to 100%
                                                 Default: 1.0

                                                         p2
                                                         N/A

OV2_PARM_LETTER_BOX   Get/Set/Caps    2                   p1
                                                 Specifies if the color
                                                 specified in p2 should fill
                                                 the rest of the destination
                                                 region.
                                                 1 = Letterbox On
                                                 0 = Letterbox Off
                                                 Default: 0

                                                         p2
                                                 (Type RGBVAL) specifies the
                                                 color to fill the rest of the
                                                 region.
                                                 Default: RGB_BLACK

OV2_PARM_ENABLE       Get/Set        N/A                  p1
                                                 Specifies if the overlay is
                                                 enabled. If the overlay is
                                                 disabled, then it will not be
                                                 visable on the screen.
                                                 1 = Overlay Enabled
                                                 0 = Overlay Disabled
                                                 Default: 1

                                                         p2
                                                         N/A

OV2_PARM_AUTO_DISABLE Get/Set        N/A                  p1
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

OV2_PARM_ROTATE       Get/Set/Caps    1                   p1
                                                 (Type uint) Specifies the 
                                                 amount to rotate the image in
                                                 the clockwise direction.
                                                 Default:0

                                                         p2
                                                         N/A

OV2_PARM_REFLECT      Get/Set/Caps    1                   p1
                                                 Specifies if the image should
                                                 be reflected around a given 
                                                 axis. Value is a bitmask.
                                                 OV2_FLAG_REFLECT_X_AXIS: Flip 
                                                    over the x-axis.
                                                 OV2_FLAG_REFLECT_Y_AXIS: Flip
                                                    over the y-axis.
                                                 Default: 0

                                                         p2
                                                         N/A

OV2_PARM_SCALE_X      Get/Set/Caps    2                   p1
OV2_PARM_SCALE_Y      Get/Set/Caps    2           Bitmask that modifies the scale
                                                 operation.

                                                 OV2_FLAG_SCALE_TO_FIT: Sets the
                                                 scaling factor to the maximium
                                                 that will fit in the
                                                 destination region.

                                                 OV2_FLAG_SCALE_KEEP_ASPECT: Sets
                                                 the scaling factor of the other
                                                 dimension to be the same as the
                                                 one specified.

                                                 OV2_FLAG_SCALE_NO_CROP: If
                                                 either dimension of the
                                                 destination region would be
                                                 exceeded by this operation,
                                                 the scaling factor will be
                                                 reduced to prevent this.

                                                 Default: 
                                                 OV2_FLAG_SCALE_KEEP_ASPECT|
                                                 OV2_FLAG_SCALE_TO_FIT|
                                                 OV2_FLAG_SCALE_NO_CROP

                                                         p2
                                                 (Type float) The scaling factor
                                                 for this dimension. If
                                                 OV2_FLAG_SCALE_TO_FIT is set
                                                 this value is meaningless.

                                                 Default: 1.0

 
OV2_PARM_OWNER        Get            N/A                  p1
                                                 Class ID of the application
                                                 that created this overlay.

                                                         p2
                                                         N/A

OV2_PARM_GROUPID      Get/Set        N/A                  p1
                                                 GroupID that will allow 
                                                 additional apps to access 
                                                 this overlay. Can only be set
                                                 by the overlay owner or an app
                                                 with system privileges.

                                                 Default: 0

                                                         p2
                                                         N/A

OV2_PARM_OVERLAYID    Get/Set        N/A                  p1
                                                 A Non-unique ID for this
                                                 overlay. Values below 
                                                 0x00010000 are reserved for the
                                                 OEM.

                                                 Default: 0xFFFFFFFF

                                                         p2
                                                         N/A

OV2_PARM_DEST_REGION_SCREEN Get        N/A                  p1
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

OV2_PARM_VSYNC     Get/Set/Caps       1                   p1

                                                 If VSync is enabled the physical
                                                 display will synchronize updates
                                                 with the screen.

                                                 0: VSync Disabled
                                                 1: VSync Enabled

                                                 Default: 0

                                                         p2
                                                         N/A

OV2_PARM_BACK_BITMAP  Get/Set        N/A                  p1
                                                 Pointer to IBitmap.
                                                 Setting this bitmap will modify
                                                 the behavior of IOVERLAY_Update.
                                                 If this bitmap is set
                                                 to a non-zero value then.
                                                 On an update this bitmap will
                                                 be copied into OV2_PARM_BITMAP
                                                 and then OV2_PARM_BITMAP will be
                                                 registered and updated on the
                                                 screen.

                                                         p2
                                                         N/A

OV2_PARM_NO_AUTOCLEAR Get/Set/Caps    1                   p1

                                                 If this value is set to 1 then
                                                 the Overlay will not clear
                                                 itself when the destination
                                                 region changes

                                                 0: AutoClear Enabled
                                                 0: AutoClear Disabled

                                                 Default: 0

                                                         p2
                                                         N/A

See Also:
   IOVERLAY2MGR_GetCapabilities()
   IOVERLAY_GetCapabilities()
   IOVERLAY_SetParam()
   IOVERLAY_GetParam()
   IOVERLAY_Update()

================================================================================
OverlayParamChangedType

Description:
   A pointer to this data type will be used as the dwParam of 
   EVT_MDL_OV2_PARAMETER_CHANGED, EVT_MDL_OV2_PARAMETER_CHANGED_INDIRECT, or 
   EVT_MDL_OV2_PARMAETER_CHANGED_SUGGEST.


Definition:
typedef struct
{
   OverlayParamType nParamID;
   OverlayTypeUnion p1;
   OverlayTypeUnion p2;
} OverlayParamChangedType;


Members:
===pre>
nParamID :  Parameter we are interested in.
p1       :  Depends on nParamID.
p2       :  Depends on nParamID.
===/pre>

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IOverlay2 supported parameters. This will provide more information on
   p1 and p2.

See Also:
   IOVERLAY_SetParam()
   IOVERLAY_GetParam()
   IOVERLAY_AddListener()
   OverlayParamType

================================================================================
OverlayAsyncUpdateCompleteType

Description:
   A pointer to this data type will be used as the dwParam of 
   EVT_MDL_OV2_ASYNC_UPDATE_COMPLETE.


Definition:
typedef struct OverlayAsyncUpdateCompleteType
{
   uint32 dwUser;
   uint32 dwUnique;
   int    nErr;
} OverlayAsyncUpdateCompleteType;



Members:
===pre>
dwUser   :  The user ID that was passed to the IOVERLAY_UpdateAsync Function.
dwUnique :  The unique update ID that returned from the IOVERLAY_UpdateAsync 
            function.
nErr     :  The status of the update operation.
===/pre>

Comments:  
   None.

See Also:
   IOVERLAY_UpdateAsync()
   IOVERLAY_AddListener()

================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IOverlayMgr Interface

Description:
   IOverlay2Mgr is the interface for managing overlays. It provides functionality
   to create new overlays, to enumerate exisiting overlays, and to get the 
   capabilities of the overlay system.

Required header files:
   None.
   
================================================================================

IOVERLAY2MGR_AddRef()

Description:

   This function increments the reference count of the IOverlay2Mgr interface
   object, allowing the interface to be shared by multiple callers.  The object
   is freed when the reference count reaches 0 (zero).

Prototype:

   uint32 IOVERLAY2MGR_AddRef(IOverlay2Mgr *p);

Parameters:
===pre>
   p:  Pointer to the IOverlay2Mgr interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IOverlay2Mgr interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IOverlay2Mgr_Release()
   
================================================================================

IOVERLAY2MGR_Release()

Description:

   This function decrements the reference count of the IOverlay2Mgr interface
   object.  The object is freed from memory and is no longer valid once its 
   reference count reaches 0 (zero).

Prototype:

   uint32 IOVERLAY2MGR_Release(IOverlay2Mgr *p);

Parameters:
===pre>
   p:  Pointer to the IOverlay2Mgr interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IOverlay2Mgr interface object.
   If the returned reference count is 0 (zero), the object has been freed and
   will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IOVERLAY2MGR_AddRef()
   
================================================================================

IOVERLAY2MGR_QueryInterface()

Description: 
   This function asks an object for another API contract from the object in
   question.

Prototype:

   int IOVERLAY2MGR_QueryInterface(IOverlay2Mgr *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IOverlay2Mgr interface object.
   c:  Requested class ID exposed by the object
   d:  Pointer to the returned IOverlay2Mgr interface object.
===/pre>

Return Value:
   SUCCESS          - Interface found
   ENOMEMORY        - Insufficient memory
   ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
   The IOverlay2Mgr pointer in 'p' is set to the new interface (with refcount
   positive), or NULL if the ClassID is not supported by the object.  'p' must
   _not_ be set to NULL.

Side Effects: 
   None

See Also:
   IQI_QueryInterface()
   
================================================================================

IOVERLAY2MGR_CreateOverlay()

Description: 
   This function creates an overlay based on the parameters specified.

Prototype:

   int IOVERLAY2MGR_CreateOverlay(IOverlay2Mgr *p, IBitmap * pBmp,
                                                 AEECLSID display,
                                                 AEEPoint destPoint);

Parameters:
===pre>
   p      :  Pointer to the IOverlay2Mgr interface object.
   pBmp   :  Pointer to the IBitmap interface that will be displayed
   display:  ClassID of the display that this overlay will be created on.
   destPoint: Where on the display should this overlay be placed.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   ENOMEMORY        - Insufficient memory
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The Overlay that is returned will have some of its parameters set. See the
   documentation for IOVERLAY_SetParam for more information.

Side Effects: 
   None

See Also:
   IOVERLAY_SetParam()
================================================================================

IOVERLAY2MGR_EnumInit()

Description: 
   This function initializes the enumeration of the existing overlays.

Prototype:

   int IOVERLAY2MGR_EnumInit(IOverlayMgr *p, OverlayEnumInfoType * pEnumInfo);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2Mgr interface object.
   pEnumInfo:  Pointer to the OverlayEnumInfo structure that specifies the
               search criteria. 
               If this value is NULL all overlays will be returned.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   ENOMEMORY        - Insufficient memory
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   None.

Side Effects: 
   None

See Also:
   IOVERLAY2MGR_EnumNext()
================================================================================

IOVERLAY2MGR_EnumNext()

Description: 
   This function gets the next overlay in the set that was match by EnumInit.

Prototype:

   IOverlay2 * IOVERLAY2MGR_EnumNext(IOverlay2Mgr *p);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2Mgr interface object.
===/pre>

Return Value:
   A pointer to the next IOverlay2 that was matched.

Comments:  
   The returned IOverlay2 should be released when the caller of
   IOVERLAY2MGR_EnumNext is done with it.

Side Effects: 
   None

See Also:
   IOVERLAY2MGR_EnumInit()
   
================================================================================

IOVERLAY2MGR_GetCapabilities()

Description: 
   This function retrieves the list of possible values for a parameter to
   IOVERLAY_SetParam. These values are to give an idea of what the device
   supports. An overlay may not be able to set all of these values at any given
   time. The function IOVERLAY_GetCapabilities() will serve this function.

Prototype:

   int IOVERLAY2MGR_GetCapabilities(IOverlay2Mgr *p, OverlayParamType paramID,
                                       uint32 * pSize,
                                       OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2Mgr interface object.
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
   list of IOverlay2Mgr supported capability parameters.

Side Effects: 
   None

See Also:
   IOVERLAY_GetCapabilities()
   IOVERLAY_SetParam()
   IOVERLAY_GetParam()
   
================================================================================
 INTERFACE DOCUMENTATION
================================================================================

IOverlay2 Interface

Description:
   IOverlay2 is the interface for contolling an overlay. An overlay represents
   one IBitmap that has been placed onto the screen.

Required header files:
   None.
   
================================================================================

IOVERLAY_AddRef()

Description:

   This function increments the reference count of the IOverlay2 interface
   object, allowing the interface to be shared by multiple callers.  The object
   is freed when the reference count reaches 0 (zero).

Prototype:

   uint32 IOVERLAY_AddRef(IOverlay2 *p);

Parameters:
===pre>
   p:  Pointer to the IOverlay2 interface object
===/pre>

Return Value:
   Returns the incremented reference count for the IOverlay2 interface object.

Comments:
   None

Side Effects:
   None

See Also:
   IOVERLAY_Release()
   
================================================================================

IOVERLAY_Release()

Description:

   This function decrements the reference count of the IOverlay2 interface 
   object.  The object is freed from memory and is no longer valid once its
   reference count reaches 0 (zero).

Prototype:

   uint32 IOVERLAY_Release(IOverlay2 *p);

Parameters:
===pre>
   p:  Pointer to the IOverlay2 interface object
===/pre>

Return Value:
   Returns the decremented reference count for the IOverlay2 interface object.
   If the returned reference count is 0 (zero), the object has been freed and
   will no longer be valid.

Comments:
   None

Side Effects:
   None

See Also:
   IOVERLAY_AddRef()
   
================================================================================

IOVERLAY_QueryInterface()

Description: 
   This function asks an object for another API contract from the object in
   question.

Prototype:

   int IOVERLAY_QueryInterface(IOverlay2 *p, AEECLSID c, void **d)

Parameters:
===pre>
   p:  Pointer to the IOverlay2 interface object.
   c:  Requested class ID exposed by the object
   d:  Pointer to the returned IOverlay2Mgr interface object.
===/pre>

Return Value:
   SUCCESS          - Interface found
   ENOMEMORY        - Insufficient memory
   ECLASSNOTSUPPORT - Requested interface is unsupported
   EPRIVLEVEL       - Not allowed to get the interface

Comments:  
   The IOverlay2 pointer in 'p' is set to the new interface (with refcount
   positive), or NULL if the ClassID is not supported by the object.  'p' must
   _not_ be set to NULL.

   The ClassIDs that are supported are:

   AEEIID_OVERLAY: This returns a new object that is a pointer to the wrapper
                   class.
   AEEIID_OVERLAY_DIRECT: This returns a pointer to the underlying overlay. Only
                          available to the owner.

   AEEIID_MODEL: This returns a pointer to the model that is associated with 
                 this overlay.

Side Effects: 
   None

See Also:
   IQI_QueryInterface()
   OverlayParamChangedType


================================================================================

IOVERLAY_GetCapabilities()

Description: 
   This function retrieves the list of possible values for a parameter to
   IOVERLAY_SetParam. These values represent what the given overlay supports at
   the current moment. The values that are supported can change at any time.

Prototype:

   int IOVERLAY_GetCapabilities(IOverlay2 *p, OverlayParamType paramID,
                                             uint32 * pSize,
                                             OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
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
   list of IOverlay2 supported capability parameters.

Side Effects: 
   None

See Also:
   IOVERLAY2MGR_GetCapabilities()
   IOVERLAY_SetParam()
   IOVERLAY_GetParam()

================================================================================

IOVERLAY_SetParam()

Description: 
   This function sets the specified IOverlay2 Interface object parameter. 

Prototype:

   int IOVERLAY_SetParam(IOverlay2 *p, OverlayParamType paramID,
                                      OverlayTypeUnion p1, OverlayTypeUnion p2);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
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
   list of IOverlay2 supported settable parameters. Only the app that created
   this overlay, apps that are in the group that was set by the owner, or system
   level apps can change the overlay parameters.

Side Effects: 
   If the values that are specified are valid, but can not be set at the current
   time, the closest value that is possible will be used and the listeners
   specified in the IModel interface of IOverlay2 will be called.

See Also:
   IOVERLAY_GetCapabilities()
   IOVERLAY_GetParam()
   IOVERLAY_SetParamSuggested()

================================================================================

IOVERLAY_SetParamSuggested()

Description: 
   This function sets the specified IOverlay2 Interface object parameter.
   However, if an IOVERLAY_SetParam has occured this value will not be used
   until bReset is set to true.

Prototype:

   int IOVERLAY_SetParamSuggested(IOverlay2 *p, OverlayParamType paramID,
                                               OverlayTypeUnion p1,
                                               OverlayTypeUnion p2,
                                               boolean bReset);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
   paramID  :  Parameter we are interested in.
   p1       :  Depends on paramID.
   p2       :  Depends on paramID.
   bReset   :  If TRUE renable the use of this function.

===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   The table in "comments section of OverlayParamType" documentation gives the
   list of IOverlay2 supported settable parameters. Only the app that created
   this overlay, apps that are in the group that was set by the owner, or system
   level apps can change the overlay parameters.

Side Effects: 
   If this call results in a change of a value the listeners specified in the 
   IModel interface of IOverlay2 will be called.

See Also:
   IOVERLAY_GetCapabilities()
   IOVERLAY_GetParam()
   IOVERLAY_SetParam()

================================================================================

IOVERLAY_GetParam()

Description: 
   This function gets the specified IOverlay2 Interface object parameter.

Prototype:

   int IOVERLAY_GetParam(IOverlay2 *p, OverlayParamType paramID,
                                      OverlayTypeUnion * pP1,
                                      OverlayTypeUnion * pP2);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
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
   list of IOverlay2 supported gettable parameters.

Side Effects: 
   None

See Also:
   IOVERLAY_GetCapabilities()
   IOVERLAY_SetParam()

================================================================================

IOVERLAY_Update()

Description: 
   This function updates the IOverlay2 on the screen. 

Prototype:

   int IOVERLAY_Update(IOverlay2 *p, AEERect rect);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
   rect     :  Rectangle that describes the dirty rectangle.
               If the dx/dy values are 0 the entire Overlay will be redrawn.
               These coordinates are relative to the Bitmap that is passed in.
===/pre>

Return Value:
   SUCCESS          - Operation successful
   EBADPARM         - One or more of the parameters passed in is not valid
   EFAILED          - Generic failure

Comments:  
   None.

Side Effects: 
   None.

See Also:
   None.

================================================================================

IOVERLAY_Clear()

Description: 
   This function clears the IOverlay2 from the screen. 

Prototype:

   int IOVERLAY_Clear(IOverlay2 *p);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
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

IOVERLAY_AddListener()

Description:
This function allows objects to attach a listener to the given interface.  The 
listener includes a callback function that BREW will call whenever the interface
has changed state, thereby notifying the owner of the listener that data has
changed.
The list of events that can be sent are listed in the comments section.

Prototype:
int IOVERLAY_AddListener(IOverlay2 *pMe, ModelListener *pListener); 

Parameters:
===pre>
p        :  Pointer to the IOverlay2 interface object.
pListener:  Pointer to the ModelListener that will be attached to the object.  
The ModelListener will contain the PFNLISTENER callback function 
BREW will call to notify the registering object of changes/events 
to the IOverlay2.
===/pre>

Return Value:
None

Comments:  
This is a table of valid events from this interface.

EVENT             Description                          dwParam
--------------------------------------------------------------------------------
EVT_MDL_OV2_PARAMETER_CHANGED
                  A Parameter has changed, this can    Pointer to an 
                  be due to any reason.                OverlayParamChangedType
                                                       structure.

EVT_MDL_OV2_PARAMETER_CHANGED_INDIRECT 
                  A Parameter has changed, and was     Pointer to an 
                  not caused by a call to              OverlayParamChangedType
                  IOVERLAY_SetParam or                 structure.
                  IOVERLAY_SetParamSuggested.

EVT_MDL_OV2_PARAMETER_CHANGED_SUGGEST
                  A Parameter has changed, this was    Pointer to an 
                  caused by a call to                  OverlayParamChangedType
                  IOVERLAY_SetParamSuggested.          structure.

EVT_MDL_OV2_ASYNC_UPDATE_COMPLETE
                  An async update has finished.        Pointer to an
                                                       OverlayAsyncUpdateCompleteType
                                                       structure.

Side Effects: 
None

See Also:
ModelListener
OverlayParamChangedType
OverlayAsyncUpdateCompleteType
PFNLISTENER

================================================================================

IOVERLAY_UpdateAsync()

Description: 
   This function updates the IOverlay2 asynchronously on the screen. 

Prototype:

   uint32 IOVERLAY_UpdateAsync(IOverlay2 *p, AEERect rect, uint32 dwUser);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
   rect     :  Rectangle that describes the dirty rectangle.
               If the dx/dy values are 0 the entire Overlay will be redrawn.
               These coordinates are relative to the Bitmap that is passed in.
   dwUser   :  A user specified value. This will only be passed to the listener
               event for this update.
===/pre>

Return Value:
   A unique ID assigned for this update. If this value is 0xFFFFFFFF an error
   occured and the update will not happen.

Comments:  
   None.

Side Effects: 
   This call will result cause the listeners specified in the 
   IModel interface of IOverlay2 will be called with the event 
   EVT_MDL_OV2_ASYNC_UPDATE_COMPLETE.

See Also:
   IOverlay_AddListener

================================================================================
*/

