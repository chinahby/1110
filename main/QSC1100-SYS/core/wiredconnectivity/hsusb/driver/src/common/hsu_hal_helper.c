/*
===========================================================================

FILE:         hsu_hal_helper.c

DESCRIPTION:  
This is the USB cores' base addresses of all MSM targets for HAL.

===========================================================================

===========================================================================
Copyright © 2009 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "msm.h"  /* for types definition and HWIO macros */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
* HS-USB Cores' base addresses.
*/
#if defined (FEATURE_HSU_TEST)
/* Dummy memory space definition for simulating High-Speed USB 
** core register space in testing mode.
*/
static uint8 HAL_HSUSB_CoreDummySpace[0x300] = {0};
uint32 usb_core_base[] = 
{
  (uint32)&HAL_HSUSB_CoreDummySpace[0]
, (uint32)&HAL_HSUSB_CoreDummySpace[0] 
, (uint32)&HAL_HSUSB_CoreDummySpace[0]
};

#elif defined (T_QSC1100)
uint32 usb_core_base[] = 
{
  USBH_BASE
};
#ifndef hw_msm_id

uint32 hw_msm_id(void)
{
  return 34/*HW_QST1100*/;
}
#endif /* !hw_msm_id */

#elif defined(T_QSC6695)
uint32 usb_core_base[] = 
{
  USBH_BASE
, USB_2ND_BASE
, HSIC_BASE
};

#elif defined(T_QSC6270)
uint32 usb_core_base[] = 
{
  USBH_BASE
, USB_2ND_BASE
};

#elif (defined (T_MSM6246) ||  defined(T_MSM6290))
uint32 usb_core_base[] = 
{
  USBH_BASE
, USB_HS_MUSIM_REG_BASE
};

#elif (defined(T_MSM7800) || defined(T_MSM8650) || defined(T_MSM7630))
uint32 usb_core_base[] = 
{
  USBH_BASE
, USBH2_BASE
, USBH3_BASE
};

#elif (defined(T_MDM8200) || defined(T_MDM8900))
uint32 usb_core_base[] = 
{
  USBH_BASE
, USBF_BASE
};

#elif (defined(T_MSM7600) || defined(T_MSM7625))
uint32 usb_core_base[] = 
{
  USBH_BASE
, USB_OTG_BASE
};

#else
#error "The USB cores are undefined for this target."

#endif
