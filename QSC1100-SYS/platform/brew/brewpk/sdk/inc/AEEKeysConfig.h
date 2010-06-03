#ifndef __AEEKEYSCONFIG_H
#define __AEEKEYSCONFIG_H

/*=============================================================================
        Copyright 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===============================================================================

DESCRIPTION: Header for the IKeysConfig interface

=============================================================================*/

#include "AEEQueryInterface.h"
#include "AEEInterface.h"

#define AEEIID_KEYSCONFIG 0x01060dbc


#define INHERIT_IKeysConfig( iname ) \
   INHERIT_IQueryInterface( iname ); \
   int (*GetStickyKeys)(iname*, uint32 *pdwKeys); \
   int (*GetKeyState)(iname*, uint32 dwKey, boolean *pbState); \
   int (*SetKeyState)(iname*, uint32 dwKey, boolean bState) 


AEEINTERFACE_DEFINE( IKeysConfig );

static __inline uint32 IKEYSCONFIG_AddRef(IKeysConfig *pKeysConfig)
{
  return AEEGETPVTBL(pKeysConfig,IKeysConfig)->AddRef(pKeysConfig);
}

static __inline uint32 IKEYSCONFIG_Release(IKeysConfig *pKeysConfig)
{
  return AEEGETPVTBL(pKeysConfig,IKeysConfig)->Release(pKeysConfig);
}

static __inline int IKEYSCONFIG_QueryInterface(IKeysConfig *pKeysConfig, AEECLSID clsid, void **ppo)
{
  return AEEGETPVTBL(pKeysConfig,IKeysConfig)->QueryInterface(pKeysConfig,clsid,ppo); 
}

static __inline int IKEYSCONFIG_GetStickyKeys(IKeysConfig *pKeysConfig, uint32 *pdwKeys)
{
  return AEEGETPVTBL(pKeysConfig,IKeysConfig)->GetStickyKeys(pKeysConfig, pdwKeys);
}

static __inline int IKEYSCONFIG_GetKeyState(IKeysConfig *pKeysConfig, uint32 dwKey, boolean *pbState)
{
  return AEEGETPVTBL(pKeysConfig,IKeysConfig)->GetKeyState(pKeysConfig, dwKey, pbState);
}

static __inline int IKEYSCONFIG_SetKeyState(IKeysConfig *pKeysConfig, uint32 dwKey, boolean bState)
{
  return AEEGETPVTBL(pKeysConfig,IKeysConfig)->SetKeyState(pKeysConfig, dwKey, bState);
}


/*
======================================================================= 
  INTERFACES   DOCUMENTATION
======================================================================= 

IKeysConfig Interface

Description:
 This interface provides services to get and set the state of sticky keys
 on the device.

The following header file is required:

AEEKeysConfig.h

=============================================================================

IKEYSCONFIG_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 


See Also:
    IKEYSCONFIG_Release()    

=======================================================================

IKEYSCONFIG_Release()

Description:
    This function is inherited from IBASE_Release()

See Also:
    IKEYSCONFIG_AddRef()

=======================================================================

IKEYSCONFIG_QueryInterface()

Description: 
	This function is inherited from IQI_QueryInterface()

See Also: 
    None

=======================================================================

IKEYSCONFIG_GetStickyKeys()

Description: 
 Returns a bit mask of sticky keys present on the device. 
 For example if caps lock and num lock are present,
 a mask with KB_CAPSLOCK and KB_NUMLOCK set will be 
 returned.
 

Prototype:
int IKEYSCONFIG_GetStickyKeys(IKeysConfig *pIKeysConfig, uint32 *pdwKeys)

Parameters:
  pIKeysConfig: IKeysConfig interface object
  pdwKeys[OUT]: On return, is set to a bit mask of all available sticky keys

Return Value:
   AEE_SUCCESS  - *keys contains bitmask of all sticky keys
   EBADPARM     - if keys is NULL

See Also:
   None

=======================================================================

IKEYSCONFIG_GetKeyState()

Description:
 Returns the state of the queried sticky key (on or off).

Prototype:
int IKEYSCONFIG_GetKeyState(IKeysConfig *pIKeysConfig, uint32 dwKey, boolean *pbState)

Parameters:
  pIKeysConfig: IKeysConfig interface object
  dwKey: Modifier value of the key (KB_CAPSLOCK for ex).
  pbState[OUT]: On return *pbState is set to TRUE if key is on, FALSE otherwise.

Return Value:
	AEE_SUCCESS - *bState contains state of queried key
	EFAILED     - if state is not supported for this key (ex. non-sticky key)
	EBADPARM    - if bState is NULL

See Also:
   None

=======================================================================

IKEYSCONFIG_SetKeyState()

Description:
 Returns the state of the queried sticky key (on or off).

Prototype:
int IKEYSCONFIG_SetKeyState(IKeysConfig *pIKeysConfig, uint32 dwKey, boolean bState)

Parameters:
  pIKeysConfig: IKeysConfig interface object
  dwKey: Modifier value of the key (KB_CAPSLOCK for ex).
  bState: The state to set this modifier key to (TRUE = ON, FALSE = OFF)

Return Value:
	AEE_SUCCESS - key state was successfully set to the requested value
	EFAILED     - if state is not supported for this key (ex. non-sticky key)

See Also:
 None

=======================================================================
*/



#endif /*__AEEKEYSCONFIG_H*/
