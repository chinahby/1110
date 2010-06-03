#ifndef PMAPP_OTG_USB_POWER_CONSUMER_CONFIG_INFO_H
#define PMAPP_OTG_USB_POWER_CONSUMER_CONFIG_INFO_H

/*! \file 
 *  \n
 *  \brief  pmapp_otg_UsbPowerConsumerConfigInfo.h ----- USB Power Consumer config related class declaration
 *  \details This header file contains class declarations of USB power consumer configuration 
 *     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_UsbPowerConsumerConfigInfo.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/08   aab      Added support for usb host suspended notification
				    (usb_power_source_suspended_notification)
04/01/08   aab      Created

========================================================================== */

/*===========================================================================
                            INCLUDE FILES FOR MODULE
===========================================================================*/
#include "pmapp_otg.h"

#ifdef FEATURE_PMIC_USB_OTG


/*===========================================================================
                              CLASS DEFINITION
===========================================================================*/

/**************************************************************************
     class UsbPowerConsumerConfigInfo											  
**************************************************************************/
/*! \class UsbPowerConsumerConfigInfo 
*  \brief  This is a helper class for Usb Power Consumer Config.
*			It also holds all needed callback function definations and other related varables
*			needed for USB power consumer module.
*  \details There should only be one instance of this calss.
*		Instance of this class can be acquired through GetInstance().
*  \note none
*/

class UsbPowerConsumerConfigInfo  // pm_app_otg_usb_power_consumer_config_info_type;
{
public:
	/*! \brief Constructor
	*  \details none
	*/
	UsbPowerConsumerConfigInfo();

	/*! \brief Destructor
	*  \details none
	*/
	~UsbPowerConsumerConfigInfo();

	/*! \brief This member function returns instance of UsbPowerConsumerConfigInfo class
	*  \details UsbPowerConsumerConfigInfo is a singlton class. This method ensures that there is only one
	*    instance of this calss.
	*  \param none
	*  \return Instance of UsbPowerConsumerConfigInfo class
	*  \note GetInstance() needs to be called to get instance of this class
	*/
	static UsbPowerConsumerConfigInfo* GetInstance();

	/*! \brief Utility function used to initiate destruction of StateManager class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
    static void DestroyInstance();

	/*! \brief  This variable defines source connected notification callback function
	*  \details none
	*/
    pm_app_otg_usb_power_source_connected_notification_cb_type     source_connected_notification_cb;	///< source connected notification cb

	/*! \brief  This variable defines source disconnected notification callback function
	*  \details none
	*/
    pm_app_otg_usb_power_source_disconnected_notification_cb_type  source_disconnected_notification_cb; ///< source disconnected notification cb

	/*! \brief  This variable defines sink open notification callback function
	*  \details none
	*/
    pm_app_otg_usb_power_sink_open_notification_cb_type            sink_open_notification_cb;			///< sink open notification cb

	/*! \brief  This variable defines sink close notification callback function
	*  \details none
	*/
    pm_app_otg_usb_power_sink_close_notification_cb_type           sink_close_notification_cb;			///< sink close notification cb

	/*! \brief  This variable defines source suspend notification callback function
	*  \details none
	*/
	pm_app_otg_usb_power_source_suspended_notification_cb_type	 source_suspended_notification_cb;

	/*! \brief  This variable holds the max charging current in mAwhen APP OTG state machine is in suspend state (3mA)
	*  \details 3 mA (actually it is 2.5mA rounded up) upon USB bus suspend
	*/
	uint32 pm_app_otg_max_i_ma__upon_bus_suspend;		

	/*! \brief  This variable holds the max charging current in mA, when APP OTG state machine is in reset (100 mA)
	*  \details 100 mA max upon USB bus reset
	*/
	uint32 pm_app_otg_max_i_ma__upon_bus_reset;			 

	/*! \brief  This variable holds the max charging current in mA, when APP OTG state machine is in carkit state (1500 mA)
	*  \details 1500 mA max upon connection to a carkit
	*/
	uint32 pm_app_otg_max_i_ma__upon_carkit_connect;	

	/*! \brief  This variable holds the max charging current in mA, when APP OTG state machine is in USB Charger Connected state (1500 mA)
	*  \details 1500 mA max upon connection to USB charger
	*/
	uint32 pm_app_otg_max_i_ma__upon_charger_connect;	

private:
	/*! \brief This static variable is used to hold the singlton instance of UsbPowerConsumerConfigInfo class
	*   \details none
	*/
	static UsbPowerConsumerConfigInfo* mpUsbPowerConsumerConfigInfo_Instance;

};


#endif //FEATURE_PMIC_USB_OTG
#endif //PMAPP_OTG_USB_POWER_CONSUMER_CONFIG_INFO_H

