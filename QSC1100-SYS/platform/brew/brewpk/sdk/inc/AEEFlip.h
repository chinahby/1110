#ifndef AEEFLIP_H
#define AEEFLIP_H

/*============================================================================
FILE: AEEFlip.h

SERVICES:  Services for flips on a phone

GENERAL DESCRIPTION: 
   This file provides definitions for the interface to flips on a phone

Copyright © 2005-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

#define AEEIID_IFLIP    0x01030214

#include "AEE.h"
#include "AEEDeviceNotifier.h"
#include "AEEVCodes.h"
//
// Flip Parms
//
// Set/Get IFlip parms using IFLIP_SetParm()/IFLIP_GetParm().
//
// IMPORTANT NOTE: 
//    For documentation of these parameters, please refer to 
//    "Flip Parameters (FLIP_PARM_XXX)" section under 
//    documentation (scroll down a few pages).
//
#define FLIP_PARM_BASE               1                       // Base used by IFlip
#define FLIP_PARM_USER_BASE          0x8000                  // Base for extension

#define FLIP_PARM_IS_SUPPORT FLIP_PARM_BASE
#define FLIP_PARM_DISPLAYLIST FLIP_PARM_BASE +1
#define FLIP_PARM_KEYLIST FLIP_PARM_BASE + 2

// Standard Flip Capabilities
// 
#define AEEFLIP_CAP_KEYGUARD     NMASK_DEVICENOTIFIER_KEYGUARD
#define AEEFLIP_CAP_FLIP         NMASK_DEVICENOTIFIER_FLIP
#define AEEFLIP_CAP_SCR_ROTATE   NMASK_DEVICENOTIFIER_SCR_ROTATE

// flip capability structure
typedef uint32 AEEFlipCapability;

//flip position codes for position desired - current/min/max
#define FLIP_POSITION_CURRENT 1
#define FLIP_POSITION_MIN 2
#define FLIP_POSITION_MAX 3

// flip position type enum
typedef enum 
{
   AEEFLIP_POSTYPE_NONE = -1, 

   AEEFLIP_POSTYPE_ANGLE = 0,
   /*x,y,z values in AEEFlipInfo are angles, in degrees. 
    See AEEFlipInfo documentation*/

   AEEFLIP_POSTYPE_LINEAR = 1,
   /*x,y,z values in AEEFlipInfo are in linear units (or) 
   a percentage of linear movement range.
   See AEEFlipInfo documentation*/

   AEEFLIP_POSTYPE_BIG = 0x10000000
   /* Forcing enum to be 32 bit */
} AEEFlipPositionType;

//data structure to retrieve information about a flip
typedef struct _AEEFlipInfoEx
{
   int16 dwStructSize;
   AEEFlipInfo flipInfo;
   AEEFlipCapability flipCapability;
   AEEFlipPositionType flipPositionType;
} AEEFlipInfoEx;

//******************************************************************************
//
// IFlip Interface
//
//******************************************************************************

#define INHERIT_IFlip(iname) \
   INHERIT_IQueryInterface(iname); \
   int       (*GetFlipInfoEx)(iname * po, AEEFlipInfoEx * pFlipInfoEx); \
   int       (*GetFlipIDList)(iname * po, AEEFlipInfo * pFlipInfoList, int16 * pnCount); \
   int       (*GetFlipPosition)(iname * po, AEEFlipInfo * pFlipInfo, int nPositionDesired); \
   int       (*SetParm)(iname * po, AEEFlipInfo * pFlipInfo, int16 nParmID, int32 p1, int32 p2); \
   int       (*GetParm)(iname * po, AEEFlipInfo * pFlipInfo, int16 nParmID, int32 * pP1, int32 * pP2) 

AEEINTERFACE_DEFINE(IFlip);
   
#define IFLIP_AddRef(p)                      AEEGETPVTBL(p,IFlip)->AddRef(p)
#define IFLIP_Release(p)                     AEEGETPVTBL(p,IFlip)->Release(p)
#define IFLIP_QueryInterface(p,i,p2)         AEEGETPVTBL(p,IFlip)->QueryInterface(p,i,p2)
#define IFLIP_GetFlipInfoEx(p,pf)            AEEGETPVTBL(p,IFlip)->GetFlipInfoEx(p,pf)
#define IFLIP_GetFlipIDList(p,paf,pn)        AEEGETPVTBL(p,IFlip)->GetFlipIDList(p,paf,pn)
#define IFLIP_GetFlipPosition(p,pf)          AEEGETPVTBL(p,IFlip)->GetFlipPosition(p,pf,FLIP_POSITION_CURRENT)
#define IFLIP_GetMinFlipPosition(p,pf)       AEEGETPVTBL(p,IFlip)->GetFlipPosition(p,pf,FLIP_POSITION_MIN)
#define IFLIP_GetMaxFlipPosition(p,pf)       AEEGETPVTBL(p,IFlip)->GetFlipPosition(p,pf,FLIP_POSITION_MAX)
#define IFLIP_SetParm(p,pf,nParmID,p1,p2)    AEEGETPVTBL(p,IFlip)->SetParm(p,pf,nParmID,p1,p2)
#define IFLIP_GetParm(p,pf,nParmID,pp1,pp2)  AEEGETPVTBL(p,IFlip)->GetParm(p,pf,nParmID,pp1,pp2)
#define IFLIP_IsSupport(p,pf,nParmID,pb)     AEEGETPVTBL(p,IFlip)->GetParm(p, pf, FLIP_PARM_IS_SUPPORT,nParmID,pb)
#define IFLIP_GetDisplayList(p,pac,pn)    AEEGETPVTBL(p,IFlip)->GetParm(p,NULL,FLIP_PARM_DISPLAYLIST,pac,pn)
#define IFLIP_GetKeyList(p,pak,pn)        AEEGETPVTBL(p,IFlip)->GetParm(p,NULL,FLIP_PARM_KEYLIST,pak,pn)


/*=====================================================================
DATA STRUCTURES DOCUMENTATION
=======================================================================
=======================================================================

FLIP_PARM_

Description:
   FLIP_PARM_XXX is the format for Flip Parameters.

=======================================================================

Flip Parameters

Description:
These parameters(FLIP_PARM_XXX) allow setting/getting of flip parameters.

Flip parameters are of two types:
   - Specific to a flip. Parms of this type need a Flip ID specified via the API.
   - Not Specific to a flip (that is, applicable to all flips). Parms of this type
     do not need a Flip ID specified via the API.

Flip parms are used in IFLIP_SetParm() and IFLIP_GetParm() APIs. 
If the flip needs to be specified for the parm, then the caller of IFLIP_SetParm() 
and IFLIP_GetParm() specifies the flip by filling out the ID field of the argument, 
pFlipInfo. Use the macro DEVNOTIFY_MAKE() to set the flip ID in pFlipInfo. 
If the parm applies to all flips (that is, it is not specific to a flip), and if
pFlipInfo is present as an argument in the API, then the caller should call the 
API with pFlipInfo set to NULL.

===pre>
FLIP_PARM_IS_SUPPORT
Get:  This parm is used to check if a specific parm is supported (or) not. The parm
      to be checked is specified in pp1.
      If pp1 contains a parm that is specific to a flip, then the FLIP_PARM_IS_SUPPORT 
      parm checks if the specific flip supports the parm. 
      If pp1 contains a parm that is not specific to a flip, then the FLIP_PARM_IS_SUPPORT 
      parm checks if the device as a whole, supports the parm.

      pp1[in] = ParmID
      pp2[out] = Pointer to boolean: TRUE/FALSE => Supported/Unsupported.

Set: Not supported.

FLIP_PARM_DISPLAYLIST
Get:  This parm is used to retrieve the display class IDs of the displays accessible,
      based on the current positions of all flips.
      Parm applies to all flips, and is not specific to a flip.
      See IFLIP_GetDisplayList() for more information.
      pp1[out] = AEECLSID *, array of AEECLSID.
      pp2[out] = int *, indicates the size of array of AEECLSID passed in pp1.
Set:  Not supported.


FLIP_PARM_KEYLIST
Get:  This parm is used to retrieve the key codes of the keys accessible,
      based on the current positions of all flips.
      Parm applies to all flips, and is not specific to a flip.
      See IFLIP_GetKeyList() for more information.
      pp1[out] = AVKType *, array of AVKType. 
                 The AVKType values returned can be in the BREW range 
                 (or) in the OEM range. 
      pp2[out] = int *, indicates the size of array of AVKType passed in pp1.
Set: Not supported.
===/pre>
=======================================================================

AEEFlipCapability

Description:
   This is a 32-bit unsigned integer used to store/indicate the 
   capabilities of a flip.

Definition:
   typedef uint32 AEEFlipCapability;

Members:
   None

Comments:
   The various flip capability values are:
   
      AEEFLIP_CAP_KEYGUARD - the flip can raise keyguard events and notifications.
	  
      AEEFLIP_CAP_FLIP - the flip can raise flip events and notifications.
	  
      AEEFLIP_CAP_SCR_ROTATE - the flip can raise screen rotate events and notifications.

See Also:
   AEEFlipInfoEx
   IFLIP_GetFlipInfoEx

=======================================================================

AEEFlipPositionType

Description:
   This ENUM specifies how to interpret the x-axis, y-axis, z-axis
   fields in the AEEFlipInfo structure. 

Definition:
   typedef enum 
   {
   AEEFLIP_POSTYPE_NONE = -1, 
   AEEFLIP_POSTYPE_ANGLE = 0,
   AEEFLIP_POSTYPE_LINEAR = 1,
   AEEFLIP_POSTYPE_BIG = 0x10000000
   } AEEFlipPositionType;

Members:

   AEEFLIP_POSTYPE_NONE : 
	Invalid value, used for initialization purposes only
	
   AEEFLIP_POSTYPE_ANGLE : 
	The x-axis, y-axis, z-axis fields in AEEFlipInfo are
	angles represented in 2 degree increments
	
   AEEFLIP_POSTYPE_LINEAR : 
	The x-axis, y-axis, z-axis fields in AEEFlipInfo are in linear units (or)
	an integral percentage of the total linear movement range of the flip.
	(IFLIP_GetMinPosition() and IFLIP_GetMaxPosition() return linear units, 
	all other flip interfaces return a percentage of the total linear movement range).
                            
   AEEFLIP_POSTYPE_BIG : 
	Value for forcing enum to be 32 bit

Comments:
The figures x-axis flip, y-axis flip, z-axis flip and y-slide flip illustrate flip movement and 
their relation to AEEFlipInfo.

See Also:
   AEEFlipInfo
   AEEFlipInfoEx

=====================================================================

AEEFlipInfoEx

Description:
   This structure contains the members used in describing a flip

Definition:
   typedef struct _AEEFlipInfoEx
   {
   int16 wStructSize;
   AEEFlipInfo flipInfo;
   AEEFlipCapability flipCapability;
   AEEFlipPositionType flipPositionType;
   } AEEFlipInfoEx;

Members:
   wStructSize: 
	size of the entire AEEFlipInfoEx structure, in bytes.
	
   flipInfo : 
	AEEFlipInfo of the flip.
	
   flipCapability : 
	AEEFlipCapability of the flip.
	
   flipPositionType : 
	specifies how to interpret the x-axis, y-axis, z-axis
	fields in the AEEFlipInfo structure of the flip. 
   
Comments:
The figures x-axis flip, y-axis flip, z-axis flip and y-slide flip illustrate flip movement and 
their relation to AEEFlipInfo.

See Also:
   AEEFlipInfo
   AEEFlipCapability
   AEEFlipPositionType
   IFLIP_GetFlipInfoEx

=======================================================================
INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IFlip

   The Flip interface enables an application to find out the number of flips present on 
   a device. The application can also find out information about flips such as: 
    - Flip ID
    - Minimum and maximum positions each flip can take, and current position of each flip. 
      Flips can be one of two types , either the type that moves in an angular fashion along one or more of the
      three axes, or, the type that slides linearly. The flipPositionType member of AEEFlipInfoEx structure 
      indicates the type of the flip. The figures x-axis flip, y-axis flip, z-axis flip and y-slide flip illustrate flip movement and 
      their relation to AEEFlipInfo. See AEEFlipInfo documentation also).
    - Displays accessible to the user based on current positions of all flips.
    - Keys accessible to the user based on current positions of all flips.

   Each flip on the device has a unique flip ID that identifies the flip. 
   A flip ID of zero usually denotes the primary flip, for example, 
   the primary LCD screen of a clamshell device.

   If the flip needs to be specified, then a pointer to AEEFlipInfo
   is present as an argument in the associated API function call.
   The caller should specify the flip by filling out the ID field of the 
   AEEFlipInfo structure. The macro DEVNOTIFY_MAKE() should be used to set 
   the flip ID in the AEEFlipInfo structure.

   If the API is applicable to all flips, then the caller should 
   not specify the flip ID field, even if a pointer to AEEFlipInfo is 
   present as an argument in the API function. The caller should
   pass in a value of NULL for that pointer.

   Creation of the IFlip interface should not succeed if the device does not
   have a flip.

The following header file is required:~
   AEEFlip.h

=======================================================================

IFLIP_GetFlipInfoEx()

Description:
   This function retrieves the information about the specified 
   flip in the AEEFlipInfox structure.

   The AEEFlipInfoEx structure is passed in the pFlipInfoEx argument. 
   The caller allocates the structure and indicates the structure size 
   in wStructSize. The caller also specifies the flip for which the 
   information is sought, in the ID portion of the flipInfo member
   contained in pFlipInfoEx.

   Upon successful return, pFlipInfoEx is filled out with information 
   about the flip. If the caller supplies a wStructSize that is not
   supported, the function returns EVERSIONNOTSUPPORT.

   Use the macro DEVNOTIFY_MAKE() to set the flip ID in the 
   flipInfo member of pFlipInfoEx. Use the DEVNOTIFY_GET_X, 
   DEVNOTIFY_GET_Y and DEVNOTIFY_GET_Z macros to retrieve values 
   from the flipInfo member of pFlipInfoEx, if this function
   returns SUCCESS.

Prototype:
   int IFLIP_GetFlipInfoEx
   (
   IFlip * pIFlip,
   AEEFlipInfoEx * pFlipInfoEx
   )

Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.
   pFlipInfoEx [out] : Pointer to the AEEFlipInfoEx structure. 
                       The pFlipInfo member of this structure 
                       specifies the flip, by its flip ID.
                       If the function returns SUCCESS,
                       pFlipInfoEx contains the values for the 
                       specified flip.

Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid argument.
   EFAILED: other failure.
   EVERSIONNOTSUPPORT: if the supplied wStructSize is not supported.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEFlipInfoEx

=======================================================================

IFLIP_GetFlipIDList()

Description:
   This function retrieves the flip IDs of the flips present on the device.

   The flip IDs are returned in the AEEFlipInfo array that is passed in the 
   pFlipInfoList argument. The caller allocates the array and indicates 
   the array size in pnCount. 

   The caller can pass in a NULL value of pFlipInfoList in order to 
   retrieve just the number of flips present on the device. In this case,
   pnCount should contain a valid int16 pointer.

   If the function returns SUCCESS (or) EBUFFERTOOSMALL, pnCount contains 
   the number of flips present on the device. The function returns 
   EBUFFERTOOSMALL if the size of the pFlipInfoList array is smaller than 
   the number of flips on the device. If the function returns SUCCESS, the 
   the pFlipInfoList array contains the flip IDs.

   Use the macro DEVNOTIFY_GET_ID() to retrieve the flip ID from individual 
   array elements.

Prototype:
   int IFLIP_GetFlipIDList
   (
   IFlip * pIFlip,
   AEEFlipInfo * pFlipInfoList,
   int16 * pnCount
   )

Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.
   pFlipInfoList [out] : Pointer to the array of AEEFlipInfo structures. 
                         Can be NULL. 
                         If the function returns SUCCESS, and
                         pFlipInfoList is not NULL, it contains the flip IDs
                         of the flips present on the device.
   pnCount [in][out] : Caller indicates the size of the array of AEEFlipInfo 
                       structures passed in via pFlipInfoList. 
                       If the function returns SUCCESS (or) 
                       EBUFFERTOOSMALL, pnCount contains the number of 
                       flips on the device, and is always a non-zero, positive 
                       number.

Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid argument.
   EFAILED: other failure.
   EBUFFERTOOSMALL: 
	if the array size of pFlipInfoList passed in is smaller 
	than the number of flips present on the device.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEFlipInfo

=======================================================================

IFLIP_GetFlipPosition()

Description:
   This function returns the current position of the specified flip.

   The flip for which the position is sought, should be specified in the 
   ID portion of the AEEFlipInfo structure that is passed in via pFlipInfo. 
   A ID value of 0 usually denotes the primary flip. 
   
   Use the DEVNOTIFY_MAKE macro to set the ID portion in pFlipInfo. 
   Use the DEVNOTIFY_GET_X, DEVNOTIFY_GET_Y and DEVNOTIFY_GET_Z 
   macros to retrieve values from pFlipInfo, if this function
   returns SUCCESS.

Prototype:
   int IFLIP_GetFlipPosition
   (
   IFlip * pIFlip,
   AEEFlipInfo * pFlipInfo
   )

Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.
   pFlipInfo [in][out] : Pointer to the AEEFlipInfo structure. 
                         Caller fills in the flip ID field.

Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid argument.
   EUNSUPPORTED: if the flip ID is invalid.
   EFAILED: other failure.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEFlipInfo

=======================================================================

IFLIP_GetMinFlipPosition()

Description:
   This returns the minimum position that the specified flip can take, 
   that is, the position of the flip when it is fully closed.

   The flip for which this information is sought, should be specified in the 
   ID portion of the AEEFlipInfo structure that is passed in via pFlipInfo. 
   A ID value of 0 usually denotes the primary flip. 

   Use the DEVNOTIFY_MAKE macro to set the ID portion in pFlipInfo.
   Use the DEVNOTIFY_GET_X, DEVNOTIFY_GET_Y and DEVNOTIFY_GET_Z 
   macros to retrieve values from pFlipInfo, if this function returns SUCCESS.

Prototype:
   int IFLIP_GetMinFlipPosition
   (
   IFlip * pIFlip,
   AEEFlipInfo * pFlipInfo
   )

Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.
   pFlipInfo [in][out] : Pointer to the AEEFlipInfo structure. 
   Caller fills in the flip ID field.

Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid argument.
   EUNSUPPORTED: if the flip ID is invalid.
   EFAILED: other failure.

Comments:
   When the flip is fully closed, IFLIP_GetMinFlipPosition and 
   IFLIP_GetFlipPosition represent the same position for that flip.
   (Note that IFLIP_GetFlipPosition may represent the position as a 
   percentage. See AEEFlipInfo for more details).

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEFlipInfo

=======================================================================

IFLIP_GetMaxFlipPosition()

Description:
   This returns the maximum position that the specified flip can take, 
   that is, the position of the flip when it is fully open.

   The flip for which this information is sought, should be specified in the 
   ID portion of the AEEFlipInfo structure that is passed in via pFlipInfo. 
   A ID value of 0 usually denotes the primary flip. 
   
   Use the DEVNOTIFY_MAKE macro to set the ID portion in pFlipInfo. Use 
   the DEVNOTIFY_GET_X, DEVNOTIFY_GET_Y and DEVNOTIFY_GET_Z macros to retrieve
   values from pFlipInfo, if this function returns SUCCESS.

Prototype:
   int IFLIP_GetMaxFlipPosition
   (
   IFlip * pIFlip,
   AEEFlipInfo * pFlipInfo
   )

Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.
   pFlipInfo [in][out] : Pointer to the AEEFlipInfo structure. 
                         Caller fills in the flip ID field.

Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid argument.
   EUNSUPPORTED: if the flip ID is invalid.
   EFAILED: other failure.

Comments:
   When the flip is fully open, IFLIP_GetMaxFlipPosition and 
   IFLIP_GetFlipPosition represent the same position for that flip.
   (Note that IFLIP_GetFlipPosition may represent the position as a 
   percentage. See AEEFlipInfo for more details).

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEFlipInfo

=======================================================================

IFLIP_IsSupport()

Description:
   This function checks if specified flip parameter (FLIP_PARM_XXX)
   is supported by IFlip Interface.

   If nParmID is a parm which is specific to a flip, then a valid 
   flip ID needs to be specified in pFlipInfo.

  If nParmID is a parm that is not specific to a flip, then pFlipInfo
  needs to be NULL. For example, FLIP_PARM_IS_SUPPORT, FLIP_PARM_KEYLIST, 
  FLIP_PARM_DISPLAYLIST.

Prototype:
   int IFLIP_IsSupport
   (
   IFLIP * pIFlip,
   AEEFlipInfo * pFlipInfo,
   int16 nParmID, 
   boolean * pbSupport
   )

Parameters:
   pIFlip [in] :  Pointer to IFlip Interface.
   pFlipInfo [in] : Pointer to the AEEFlipInfo structure. 
                    Caller fills in the flip ID field.
                    Pass in a NULL value if nParm ID is a
                    parm that is not specific to a flip, for example,
                    FLIP_PARM_IS_SUPPORT, FLIP_PARM_KEYLIST, 
                    FLIP_PARM_DISPLAYLIST.
   nParmID [in] :  FLIP_PARM_XXX. See Flip Parameters.
   pbSupport [out] :  Pointer to boolean. TRUE/FALSE => Supported/Unsupported.
   
Return Value:
   SUCCESS: Successful.
   EBADPARM: Bad parm (in function argument).   
             Non-NULL pFlipInfo where NULL was expected.
   EUNSUPPORTED: Invalid Flip ID in pFlipInfo (if pFlipInfo is not NULL).
   EFAILED: General failure.

Comments:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   IFLIP_GetParm()
   IFLIP_SetParm()

=======================================================================

IFLIP_SetParm()

Description:
   This function sets the flip parameter information. The flip parameter 
   should be specified in the nParmID argument. 

   If flip needs to be specified for nParmID, then fill in the 
   ID portion of the AEEFlipInfo structure that is passed in via pFlipInfo. 
   An ID value of 0 usually denotes the primary flip. Use the DEVNOTIFY_MAKE macro 
   to set the ID portion in pFlipInfo. 

   If flip need not be specified for nParmID, then pass in a value of 
   NULL for pFlipInfo.

Prototype:
   int IFLIP_SetParm
   (
   IFlip * pIFlip,
   AEEFlipInfo * pFlipInfo,
   int16 nParmID,
   int32 p1,
   int32 p2
   )

Parameters:
   pIFlip [in] :  Pointer to IFlip Interface.
   pFlipInfo [in] : Pointer to the AEEFlipInfo structure. 
                    Caller fills in the flip ID field.
                    Pass in a NULL value if nParm ID is a
                    parm that is not specific to a flip
   nParmID [in] :  FLIP_PARM_XXX. See Flip Parameters.
   p1 :  Depends on nParmID.
   p2 :  Depends on nParmID.

Return Value:
   SUCCESS: Successful. 
   EBADPARM: Bad parm (in function argument).
             Non-NULL pFlipInfo where NULL was expected.
   EUNSUPPORTED: IFLIP_SetParm not supported for this nParmID
                 (or) flip ID is invalid
                 (or) nParmID is not supported by the class.
   EFAILED: General failure.

Comments:
   See Flip Parameters for parameter details.

Version:
   Introduced BREW Client 3.1.4

See Also:
   IFLIP_GetParm()
   AEEFlipInfo

=======================================================================

IFLIP_GetParm()

Description:
   This function gets the flip parameter information. The flip parameter
   should be specified in the nParmID argument.

   If flip needs to be specified for nParmID, the fill in the 
   ID portion of the AEEFlipInfo structure that is passed in via pFlipInfo.
   An ID value of 0 usually denotes the primary flip. Use the DEVNOTIFY_MAKE 
   macro to set the ID portion in pFlipInfo. 

   If flip need not be specified for nParmID, then pass in a value of 
   NULL for pFlipInfo.

Prototype:
   int IFLIP_GetParm
   (
   IFlip * pIFlip,
   AEEFlipInfo * pFlipInfo,
   int16 nParmID, 
   int32 * pP1, 
   int32 * pP2
   )

Parameters:
   pIFlip [in]:  Pointer to IFlip Interface.
   pFlipInfo [in] : Pointer to the AEEFlipInfo structure. 
                    Caller fills in the flip ID field.
                    Pass in a NULL value if nParm ID is a
                    parm that is not specific to a flip, for example,
                    FLIP_PARM_IS_SUPPORT, FLIP_PARM_KEYLIST, 
                    FLIP_PARM_DISPLAYLIST. An exception to this is the 
                    case when nParmID is FLIP_PARM_IS_SUPPORT, and 
                    if pP1 is a parm ID that is specific to a flip. 
                    In this case, pFlipInfo should not be 
                    NULL.
   nParmID:  FLIP_PARM_XXX. See Flip Parameters.
   pP1:  Depends on nParmID.
   pP2:  Depends on nParmID.

Return Value:
   SUCCESS: Successful. 
   EBADPARM: Bad parm (in function argument).
             Non-NULL pFlipInfo where NULL was expected.
   EUNSUPPORTED: IFLIP_GetParm not supported for this nParmID
               (or) flip ID is invalid
               (or) nParmID is not supported by the class.
   EBUFFERTOOSMALL: Buffer specified by caller is too small.
   EFAILED: General failure.

Comments:
   See Flip Parameters for parameter details.

Version:
Introduced BREW Client 3.1.4

See Also:
   IFLIP_SetParm()
   AEEFlipInfo

=======================================================================

IFLIP_GetDisplayList()

Description:
   This function retrieves the display class IDs of the displays 
   accessible to user based on the current positions of all flips. 
   
   The display class IDs are returned in the AEECLSID array 
   that is passed in the pClsList argument. The caller allocates the
   array and indicates the array size in pnCount. 

   The caller can pass in a NULL value of pClsList in order to retrieve 
   just the number of displays accessible. In this case, pnCount should 
   contain a valid int16 pointer.

   If the function returns SUCCESS (or) EBUFFERTOOSMALL, pnCount contains 
   the number of displays accessible. The function returns EBUFFERTOOSMALL 
   if the size of the pClsList array is smaller than the number of 
   displays accessible. 
   
   If pnCount = 0, it means there are no displays accessible. If the 
   function returns SUCCESS, and pnCount > 0, then pnCount is the 
   number of displays accessible. If the caller passed in a valid AEECLSID
   array via pClsList, then pClsList contains the display class IDs of the 
   displays accessible.

Prototype:
   int IFLIP_GetDisplayList
   (
   IFlip * pIFlip,
   AEECLSID * pClsList,
   int16 * pnCount
   )

Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.
   pClsList [out] : Pointer to the array of AEECLSID.
                    Can be NULL.
                    If the function returns SUCCESS, and
                    pClsList is not NULL, it contains the display class IDs
                    of the displays accessible.            
   pnCount: [in] [out] : Caller indicates the size of the array of AEECLSID 
                         passed in via pClsList. If the function returns SUCCESS,
                         this parameter contains the number of displays accessible.

Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid parameter.
   EBUFFERTOOSMALL: buffer specified by caller is too small.
   EFAILED: other failure.

Comments:
   See IDisplay interface documentation for various display class IDs.

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEEFlipInfo
   AEECLSID
   IDisplay

=======================================================================

IFLIP_GetKeyList()

Description:
   This function retrieves the key codes of the keys accessible to user 
   based on the current positions of all flips.

   The key codes are returned in the AVKType array that is passed in the 
   pKeyList argument. The caller allocates the array and indicates 
   the array size in pnCount. 

   The caller can pass in a NULL value of pKeyList in order to retrieve 
   just the number of keys accessible. In this case, pnCount should 
   contain a valid int16 pointer.

   If the function returns SUCCESS (or) EBUFFERTOOSMALL, pnCount contains 
   the number of the number of keys accessible. The function returns 
   EBUFFERTOOSMALL if the size of the pKeyList array is smaller than 
   the number of keys accessible. If the function returns SUCCESS, and 
   pnCount = 0, it means there are no keys accessible. 
   If the function returns SUCCESS, and pnCount > 0, then pnCount is the 
   number of keys accessible. If the caller passed in a valid AVKType array via 
   pKeyList, then pKeyList contains the key codes of the keys accessible.

   Prototype:
   int IFLIP_GetKeyList
   (
   IFlip * pIFlip,
   AVKType * pKeyList,
   int16 * pnCount
   )

   Parameters:
   pIFlip [in] : Pointer to the IFlip Interface object.

   pKeyList [out] : Pointer to the array of AVKType.
                    Can be NULL.
                    If the function returns SUCCESS, and
                    pKeyList is not NULL, it contains the key codes
                    of the keys accessible based on current position of
                    the specified flip.

   pnCount [in][out] : Caller indicates the size of the array of AVKType 
                       passed in via pKeyList. If the function returns SUCCESS
                       (or)EBUFFERTOOSMALL, this argument contains the number 
                       of keys accessible based on current position of the 
                       specified flip. 

   Return Value:
   SUCCESS: if successful.
   EBADPARM: invalid argument.
   EBUFFERTOOSMALL: buffer specified by caller is too small.
   EFAILED: other failure.

   Comments:
   See AEEVcodes.h for the complete key code enumeration.

   Version:
   Introduced BREW Client 3.1.4

   See Also:
   AEEFlipInfo
   KeySupportType
   AEE_DEVICEITEM_KEY_SUPPORT
====================================================================
x-axis flip
Following is the IFlip x-axis rotation:

===img src="../../../../images/x.gif">
====================================================================
   y-axis flip
Following is the IFlip y-axis rotation:

===img src="../../../../images/y.gif">
====================================================================
   z-axis flip
Following is the IFlip z-axis rotation:

===img src="../../../../images/z.gif">
====================================================================
   y-slide flip
Following is the IFlip slide y-axis movement:

===img src="../../../../images/slide.gif">


=====================================================================*/

#endif /* AEEFLIP_H */


