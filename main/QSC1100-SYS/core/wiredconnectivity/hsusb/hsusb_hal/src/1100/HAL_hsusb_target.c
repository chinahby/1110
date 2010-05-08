/*
===========================================================================

FILE:         HAL_hsusb_target.c

DESCRIPTION:  
This is the HAL implementation for the QCS1100.

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

#include "HAL_hsusb_impl_2r10.h"

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

#ifdef FEATURE_HSU_TEST
#error code not present
#endif  /* FEATURE_HSU_TEST */

/* usb_otg_hs_x2r10 core interface */
static HAL_HSUSB_CoreIfType HAL_HSUSB_CoreIf2r10;

/* USB core 0 ports. */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore0[] =
{
  {HAL_HSUSB_PHY_IF_SERIAL, HAL_HSUSB_PHY_INTEGRATED}
};

/*
* HS-USB HAL CoreInfo table.
*/
static HAL_HSUSB_CoreInfoType HAL_HSUSB_CoreInfo[] =
{
  /* High-Speed USB core. */
  {
    &HAL_HSUSB_CoreIf2r10,
#ifdef FEATURE_HSU_TEST
#error code not present
#else  /* FEATURE_HSU_TEST */
    USBH_BASE,
#endif  /* FEATURE_HSU_TEST */
    HAL_HSUSB_Success,
    &HAL_HSUSB_PortInfoCore0[0],
    sizeof(HAL_HSUSB_PortInfoCore0)/sizeof(HAL_HSUSB_PortInfoType)
  }
};

/*
* HS-USB HAL Cores object.
*/
HAL_HSUSB_CoresType HAL_HSUSB_CoresObj =
{
  &HAL_HSUSB_CoreInfo[0],
  sizeof(HAL_HSUSB_CoreInfo)/sizeof(HAL_HSUSB_CoreInfoType),
  FALSE
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Constructor.
*/
void HAL_hsusb_ConstructImplInstance()
{
  HAL_hsusb_ConstructImpl2r10(&HAL_HSUSB_CoreIf2r10);
}
