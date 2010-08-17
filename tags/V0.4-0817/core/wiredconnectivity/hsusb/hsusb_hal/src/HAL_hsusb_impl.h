#ifndef HAL_HSUSB_IMPL
#define HAL_HSUSB_IMPL
/*
===========================================================================

FILE:         HAL_hsusb_impl.h

DESCRIPTION:  
This is the HAL internal interface for the HS-USB core.

===========================================================================

===========================================================================
Copyright © 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "msm.h"  /* for types definition and HWIO macros */
#include "HAL_hsusb.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
* HAL_HSUSB_CoreIfType
*
* There is an USB core common interface.
*
*/
typedef struct HAL_HSUSB_CoreIfType
{
  uint16                  Version;
  /* The following functions are documented in HAL_hsusb_impl.c 
  where they have a default implementation */
  void    (*Reset)(uint8 instance);
  HAL_HSUSB_ErrorType (*ReadUlpiPhy)(uint8 instance, uint8 port, uint8 reg, uint8* data);
  HAL_HSUSB_ErrorType (*WriteUlpiPhy)(uint8 instance, uint8 port, uint8 reg, uint8 data);
  HAL_HSUSB_AhbBurstMode (*GetAhbBurstMode)(uint8 instance);
  void (*SetAhbBurstMode)(uint8 instance, HAL_HSUSB_AhbBurstMode eMode);
  boolean (*GetSe0GlitchFixCtrl)(uint8 instance);
  void    (*SetSe0GlitchFixCtrl)(uint8 instance, boolean state);
  boolean (*GetFs3Wire2WireSelect)(uint8 instance);
  void    (*SetFs3Wire2WireSelect)(uint8 instance, boolean state);
  boolean (*GetXtorBypass)(uint8 instance);
  void    (*SetXtorBypass)(uint8 instance, boolean state);
  boolean (*GetPostedWrites)(uint8 instance);
  void    (*SetPostedWrites)(uint8 instance, boolean state);
  boolean (*GetGrantStolen)(uint8 instance);
  void    (*SetGrantStolen)(uint8 instance, boolean state);
  HAL_HSUSB_HprotMode (*GetHprotMode)(uint8 instance);
  void    (*SetHprotMode)(uint8 instance, HAL_HSUSB_HprotMode eMode);
  boolean (*GetIncrOverride)(uint8 instance);
  void    (*SetIncrOverride)(uint8 instance, boolean state);
  boolean (*GetAsyncBridgesBypass)(uint8 instance);
  void    (*SetAsyncBridgesBypass)(uint8 instance, boolean state);
  HAL_HSUSB_TestBusSelectorType (*GetTestBusSelector)(uint8 instance);
  void    (*SetTestBusSelector)(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode);
  boolean (*GetTxBufCollisionMode)(uint8 instance);
  void    (*SetTxBufCollisionMode)(uint8 instance, boolean state);
  boolean (*GetStreamRxBypassMode)(uint8 instance);
  void    (*SetStreamRxBypassMode)(uint8 instance, boolean state);
  boolean (*GetStreamTxAutoMode)(uint8 instance);
  void    (*SetStreamTxAutoMode)(uint8 instance, boolean state);
  boolean (*GetHostSimTimersStdMode)(uint8 instance);
  void    (*SetHostSimTimersStdMode)(uint8 instance, boolean state);
  boolean (*GetHostSimTimersSuspMode)(uint8 instance);
  void    (*SetHostSimTimersSuspMode)(uint8 instance, boolean state);
} HAL_HSUSB_CoreIfType;

/*
* HAL_HSUSB_PortInfoType
*/
typedef struct
{
  HAL_HSUSB_PhyType       PhyType;
} HAL_HSUSB_PortInfoType;

/*
* HAL_HSUSB_CoreInfoType
*/
typedef struct
{
  HAL_HSUSB_CoreIfType*   If;
  uint32                  BaseAddress;
  HAL_HSUSB_ErrorType     LastError;
  HAL_HSUSB_PortInfoType* PortInfo;
  uint8                   PortsQty;
} HAL_HSUSB_CoreInfoType;

/*
* HAL_HSUSB_CoresType
*/
typedef struct
{
  HAL_HSUSB_CoreInfoType* CoreInfo;
  uint8                   MaxCoresQty;
  boolean                 Initialized;
} HAL_HSUSB_CoresType;

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Cores object.
*/
extern HAL_HSUSB_CoresType HAL_HSUSB_CoresObj;

#endif /* HAL_HSUSB_IMPL */
