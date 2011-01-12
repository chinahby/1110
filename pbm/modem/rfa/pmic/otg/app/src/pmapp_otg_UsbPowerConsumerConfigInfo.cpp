/*! \file 
 *  \n
 *  \brief  pmapp_otg_UsbPowerConsumerConfigInfo.cpp ----- USB Power Consumer config related class implementation
 *  \details  This file contains implementation methods of USB power consumer configuration 
 *     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_UsbPowerConsumerConfigInfo.cpp#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/08   aab      Changed USB wall charger max current limit from 1500 to 950 mA (Per CR-161609)
04/01/08   aab      Updated usb suspend charging current max limit to 2mA (standard is below 2.5mA)
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_UsbPowerConsumerConfigInfo.h"  

#ifdef FEATURE_PMIC_USB_OTG

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/**************************************************************************
     class UsbPowerConsumerConfigInfo											  
**************************************************************************/
UsbPowerConsumerConfigInfo::UsbPowerConsumerConfigInfo()
{
	pm_app_otg_max_i_ma__upon_bus_suspend = 2;			// 2 mA (actually it is 2.5mA rounded down) upon USB bus suspend
	pm_app_otg_max_i_ma__upon_bus_reset = 100;			// 100 mA max upon USB bus reset
	pm_app_otg_max_i_ma__upon_carkit_connect = 1500;	// 1500 mA max upon connection to a carkit
	pm_app_otg_max_i_ma__upon_charger_connect = 950;	// 950 mA max upon connection to USB charger (CR161609: changed from 1500)

	mpUsbPowerConsumerConfigInfo_Instance = NULL;
}


UsbPowerConsumerConfigInfo::~UsbPowerConsumerConfigInfo()
{
}


UsbPowerConsumerConfigInfo* UsbPowerConsumerConfigInfo::GetInstance()
{
	if(NULL == mpUsbPowerConsumerConfigInfo_Instance){
		mpUsbPowerConsumerConfigInfo_Instance = new UsbPowerConsumerConfigInfo;
	}
	return mpUsbPowerConsumerConfigInfo_Instance;
}



void UsbPowerConsumerConfigInfo::DestroyInstance()
{
         delete mpUsbPowerConsumerConfigInfo_Instance;
         mpUsbPowerConsumerConfigInfo_Instance = NULL;
}



#endif //FEATURE_PMIC_USB_OTG
