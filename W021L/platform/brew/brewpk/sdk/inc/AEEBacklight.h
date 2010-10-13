#ifndef AEEBACKLIGHT_H
#define AEEBACKLIGHT_H
/*=============================================================================
 
FILE: AEEBacklight.h

SERVICES: The IBacklight interface manages the Backlight.  

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IBacklight

Copyright © 2004-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "AEECLSID_BACKLIGHT.bid"
#include "AEE.h"
#include "AEEBitmap.h"
#ifdef CUST_EDITION	
#ifndef WIN32
#include "Hs_mb6550.h"
#endif
#include "OEMFeatures.h"
#endif /*CUST_EDITION*/
#define AEECLSID_BACKLIGHT_DISPLAY1      	AEECLSID_BACKLIGHT   	// display 1 backlight
#define AEECLSID_BACKLIGHT_DISPLAY2      	0x0103035d           	// display 2 backlight
#define AEECLSID_BACKLIGHT_DISPLAY3      	0x0103035e           	// display 3 backlight
#define AEECLSID_BACKLIGHT_DISPLAY4      	0x0103035f           	// display 4 backlight
#define AEECLSID_BACKLIGHT_KEYPAD    		0x010308a4    		// keypad backlight
#ifdef CUST_EDITION	
typedef enum _SIG_LED_TYPE
{
    SIG_LED_NONE,
    SIG_LED_SMS,
    SIG_LED_INCOMING_CALL,
    SIG_LED_MISSED_CALL,
    SIG_LED_NETWORK,
    SIG_LED_ALARM,
} sig_led_type;
#endif /*CUST_EDITION*/
/*=============================================================================

                    CLASS DEFINITIONS

=============================================================================*/
typedef struct
{
    uint32 dwSize;
    uint32 dwMinBrightnessLevel;
    uint32 dwMaxBrightnessLevel;
    uint32 dwDefaultBrightnessLevel;
    RGBVAL dwBacklightColor;
} AEEBacklightInfo;

typedef struct IBacklight         IBacklight;

AEEINTERFACE(IBacklight)
{
	INHERIT_IQueryInterface(IBacklight);
	int (*Enable)(IBacklight *p);
	int (*Disable)(IBacklight *p);
	boolean (*IsEnabled)(IBacklight *p);
	int (*GetBacklightInfo)(IBacklight *p, AEEBacklightInfo * pBacklightInfo);
	int (*GetBrightnessLevel)(IBacklight *p, uint32* pdwBrightnessLevel);
	int (*SetBrightnessLevel)(IBacklight *p, uint32 dwBrightnessLevel);
#ifdef CUST_EDITION	   
	void (*TurnOn)(IBacklight *p);   
	void (*TurnOff)(IBacklight *p);
#ifdef FEATURE_LED_CONTROL
	int (*SigLedEnable)(IBacklight *p, sig_led_type SigLed_type);   
	int (*SigLedDisable)(IBacklight *p); 
#endif
//wangliang add!  2010-06-10
#ifdef FEATURE_TORCH_SUPPORT
	void (*TurnOnTorch)(IBacklight *p);   
   	void (*TurnOffTorch)(IBacklight *p);
#endif
#endif /*CUST_EDITION*/

};

#define IBACKLIGHT_AddRef(p)                       AEEGETPVTBL((p),IBacklight)->AddRef((p))
#define IBACKLIGHT_Release(p)                      AEEGETPVTBL((p),IBacklight)->Release((p))
#define IBACKLIGHT_QueryInterface(p,clsid,pp)      AEEGETPVTBL((p),IBacklight)->QueryInterface((p),(clsid),(pp))
#define IBACKLIGHT_Enable(p)                       AEEGETPVTBL((p),IBacklight)->Enable((p))
#define IBACKLIGHT_Disable(p)                      AEEGETPVTBL((p),IBacklight)->Disable((p))
#define IBACKLIGHT_IsEnabled(p)                    AEEGETPVTBL((p),IBacklight)->IsEnabled((p))
#define IBACKLIGHT_GetBacklightInfo(p,pbi)         AEEGETPVTBL((p),IBacklight)->GetBacklightInfo((p),(pbi))
#define IBACKLIGHT_GetBrightnessLevel(p,pdwbl)     AEEGETPVTBL((p),IBacklight)->GetBrightnessLevel((p),(pdwbl))
#define IBACKLIGHT_SetBrightnessLevel(p,dwbl)      AEEGETPVTBL((p),IBacklight)->SetBrightnessLevel((p),(dwbl))
#ifdef CUST_EDITION	
#define IBACKLIGHT_TurnOn(p)                       AEEGETPVTBL((p),IBacklight)->TurnOn((p))
#define IBACKLIGHT_TurnOff(p)                      AEEGETPVTBL((p),IBacklight)->TurnOff((p))
#ifdef FEATURE_LED_CONTROL
#define IBACKLIGHT_SigLedEnable(p,profile)         AEEGETPVTBL((p),IBacklight)->SigLedEnable((p),(profile))
#define IBACKLIGHT_SigLedDisable(p)                AEEGETPVTBL((p),IBacklight)->SigLedDisable((p))
#endif
//wangliang add!  2010-06-10
#ifdef FEATURE_TORCH_SUPPORT
#define IBACKLIGHT_TurnOnTorch(p)         		AEEGETPVTBL((p),IBacklight)->TurnOnTorch((p))
#define IBACKLIGHT_TurnOffTorch(p)				AEEGETPVTBL((p),IBacklight)->TurnOffTorch((p))
#endif
#endif /*CUST_EDITION*/

#endif // AEEBACKLIGHT_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEBacklightInfo

Description:
   Backlight Information

Definition:
typedef struct
{
    uint32 dwSize;
    uint32 dwMinBrightnessLevel;
    uint32 dwMaxBrightnessLevel;
    uint32 dwDefaultBrightnessLevel;
    RGBVAL dwBacklightColor;
} AEEBacklightInfo;

Members:
    dwSize : Size of the back light structure
    dwMinBrightnessLevel : Minimum Brightness Level
    dwMaxBrightnessLevel : Maximum Brightness Level
    dwDefaultBrightnessLevel : Default Brightness Level
    dwBacklightColor : Backlight Color

Comments:
   None

See Also:
   None

=======================================================================
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IBacklight

Description:
    IBacklight interface enables applications control various back lights 
available on a device.

   A device can have multiple components which support back light e.g.
primary display, secondary display and key pad. Moreover, each device
component supporting back light might support multiple back lights,
possibly of different colors, e.g. primary display might support red, green, 
blue and yellow color back lights. For controlling a back light, 
applications create IBacklight interface by specifying back light class ID 
associated with the back light. IBacklight methods let application control
on/off status and brightness of a backlight.

   IBacklight provides enable, disable and set brightness methods. It doesn't 
enable applications decide how long a backlight will stay enabled and doesn't 
protect backlight enabled by one application from being disabled by another 
application.

   Classes implementing IBacklight interface are protected. An application 
intending to use a IBacklight class must explicitly specify so in application
MIF file.

=============================================================================

IBACKLIGHT_AddRef()

Description:
   This method increments the reference count of the IBacklight Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 IBACKLIGHT_AddRef(IBacklight * po)

Parameters:

   po [in]: Pointer to the IBacklight Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   IBACKLIGHT_Release()

=============================================================================

IBACKLIGHT_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IBACKLIGHT_Release(IBacklight * po)

Parameters:

   po [in]: Pointer to the IBacklight Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   IBACKLIGHT_AddRef()

==============================================================================

Function: IBACKLIGHT_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:

   int IBACKLIGHT_QueryInterface(IBacklight *po, AEECLSID id, void **p)

Parameters:

   po [in]: Pointer to IBacklight interface

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:

   Return SUCCESS on success,
   ECLASSNOTSUPPORT if class ID not supported

Comments:

   On failure, QueryInterface() must set *p to NULL.

Side Effects:
   None

See Also:
   None

=============================================================================

IBACKLIGHT_Enable()

Description:
   This method turns back light on.

Prototype:
   int IBACKLIGHT_Enable(IBacklight * po)

Parameters:

   po [in]: Pointer to the IBacklight Interface object

Return Value:
   SUCCESS - If successful
   Error Code - Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   IBACKLIGHT_Disable()

=============================================================================

IBACKLIGHT_Disable()

Description:
   This method turns back light off.

Prototype:
   int IBACKLIGHT_Disable(IBacklight * po)

Parameters:

   po [in]: Pointer to the IBacklight Interface object

Return Value:
   SUCCESS - If successful
   Error Code - Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   IBACKLIGHT_Enable()

=============================================================================

IBACKLIGHT_IsEnabled()

Description:
   This method checks if back light is on.

Prototype:
   boolean IBACKLIGHT_IsEnabled(IBacklight * po)

Parameters:

   po [in]: Pointer to the IBacklight Interface object

Return Value:
   TRUE - If back light is on
   FALSE - Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   IBACKLIGHT_Enable(), IBACKLIGHT_Disable()

=============================================================================

IBACKLIGHT_GetBacklightInfo()

Description:
   This method provides information about various parameters associated with the 
   back light.

Prototype:
   int IBACKLIGHT_GetBacklightInfo(IBacklight *p, AEEBacklightInfo * pBacklightInfo)

Parameters:

   po [in]: Pointer to the IBacklight Interface object
   pBacklightInfo [out]: Pointer to back light information structure

Return Value:
   SUCCESS - If successful
   Error Code - Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   AEEBacklightInfo

=============================================================================

IBACKLIGHT_GetBrightnessLevel()

Description:
   This method provides information about the current brightness level of the 
   back light.

Prototype:
   int IBACKLIGHT_GetBrightnessLevel(IBacklight *p, uint32* pdwBrightnessLevel)

Parameters:

   po [in]: Pointer to the IBacklight Interface object
   pdwBrightnessLevel [out]: Pointer to brightness level

Return Value:
   SUCCESS - If successful
   Error Code - Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   AEEBacklightInfo

=============================================================================

IBACKLIGHT_SetBrightnessLevel()

Description:
   This method sets the brightness level of the back light.

Prototype:
   int IBACKLIGHT_SetBrightnessLevel(IBacklight *p, uint32 dwBrightnessLevel)

Parameters:

   po [in]: Pointer to the IBacklight Interface object
   dwBrightnessLevel [in]: Brightness Level

Return Value:
   SUCCESS - If successful
   Error Code - Otherwise

Comments:
   None

Side Effects:
   None

See Also:
   AEEBacklightInfo

=====================================================================*/
