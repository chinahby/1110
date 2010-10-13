#ifndef PMAPP_OTG_CUSTOMHW_H
#define PMAPP_OTG_CUSTOMHW_H

/*! \file 
*  \brief  pmapp_otg_CustomHw.h ---- Custom hardware related class declaration
*  \details  This file contains functions prototypes and variable/type/constant 
*  declarations for USB-OTG application custom hardware developed for the
*  Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_CustomHw.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/08   aab      Made changes to properly initialize timer object
04/01/08   aab      Created
========================================================================== */



/*===========================================================================
INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"

extern "C"
{
	#include "pm.h"                    // for MPP APIs
	#include "pm66XXmpps.h"  
    #include "timer.h"
}

#ifdef FEATURE_PMIC_USB_OTG



/* =========================================================================
TYPE DEFINITIONS
========================================================================= */
/*!\brief Type of callback function used for notifying about the completion of custom h/w initialization
*  \details  
*/    
typedef void (*CustomHw_InitDoneNotificationCbType) (void*);



/*===========================================================================
CLASS DEFINITION
==========================================================================*/

/**************************************************************************
     class CustomHw											  
**************************************************************************/
/*! \class CustomHw 
*  \brief This class is used to initialize all custom circuitries external to PMIC USB-OTG
*      xcvr that are implemented for aiding the xcvr perform its USB-OTG functionality. 
*  \details 
*  \note none
*/
class CustomHw
{
public:

	/*! \brief Constructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	CustomHw();


	/*! \brief Destructor
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	~CustomHw();


	/*! \brief Utility function used to create a singlton instance of customHw object
	*  \details none
	*  \param none
	*  \return singlton instance of CustomHw class object
	*  \note none
	*/
	static CustomHw* GetInstance();


	/*! \brief Utility function used to initiate destruction of CustomHw class object
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void DestroyInstance();


	/*! \brief  This function initializes all custom circuitries external to PMIC USB-OTG
	*      xcvr that are implemented for aiding the xcvr perform its USB-OTG functionality.
	*  \details none
	*  \param    hw_init_done_cb
	*  - Callback function for notifying when the custom h/w has initialized itself properly.
	*  - Type: CustomHw_InitDoneNotificationCbType
	*  - Valid inputs: Valid non-NULL callback fn
	*	\param pObj :  pointer to class object that hw_init_done_cb function belongs
	*  \return none
	*  \note none
	*/

	void CustomHwInit( CustomHw_InitDoneNotificationCbType hw_init_done_cb, void* pObj );


	/*! \brief    This function resets all custom circuitries external to PMIC USB-OTG
	*	xcvr that were initialized earlier for aiding the xcvr perform its USB-OTG functionality.
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	void CustomHwReset(void);


	/*! \brief This function returns the amount of current in milli-Amps that the custom
	*		VBUS boost assist circuitry on board (if any) is rated to reliably source
	*		on VBUS into connected devices while being able to meet USB2.0 VBUS
	*		voltage requirements.
	*  \details none
	*  \param none
	*  \return Type: uint32; Source current rating in mA
	*  \note  Following fns must have been called before:  pm_app_otg_custom_hw_init()
	*/
	uint32 GetUsbVbusBoostAssistCapability(void);


	/*! \brief This function turns on the circuitry external to PMIC USB-OTG xcvr that
	*	could supplement more current on USB_VBUS than what could be sourced by the xcvr itself.
	*  \details none
	*  \param none
	*  \return none
	*  \note Following fns must have been called before:  pm_app_otg_custom_hw_init()
	*/
	void TurnOnUsbVbusBoostAssist(void);


	/*! \brief This function turns off the circuitry external to PMIC USB-OTG xcvr that
	*	could supplement more current on USB_VBUS than what could be sourced by the xcvr itself.
	*  \details none
	*  \param none
	*  \return none
	*  \note Following fns must have been called before: pm_app_otg_custom_hw_init()
	*/
	void TurnOffUsbVbusBoostAssist(void);


	/*! \brief This function is a timer callback function called when custom hardware initialization 
	*  \details none
	*  \param none
	*  \return none
	*  \note none
	*/
	static void CustomHwTimerCb(int32 time_ms, timer_cb_data_type token);

protected:


private:

	/*! \brief This var is used as a flag to indicate that initialization of custom hardware
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li CustomHwInit()
	*   \li CustomHwReset()
	*/
	boolean            mCustomHw_Initialized;

	/*! \brief This variable is used as a flag to indicate that Vreg 5V is connected to USB Vbus line
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li TurnOnUsbVbusBoostAssist()
	*   \li TurnOffUsbVbusBoostAssist()
	*/
	boolean            mCustomHwFlag_Vreg5v_ConnectedToUsbVbus;

	/*! \brief This variable holds pointer to the call back function that will 
	*			be called when custome hardware initialization timer expires
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li CustomHwInit()
	*   \li CustomHwReset()
	*   \li CustomHwTimerCb()
	*/
	CustomHw_InitDoneNotificationCbType mCustomHw_InitDoneNotificationCb;


	/*! \brief This variable is used to hold pointer to class object that the 
	*		callback function stored in mCustomHw_InitDoneNotificationCb belongs to
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li CustomHwInit()
	*   \li CustomHwTimerCb()
	*/
	void*			   mpCallingObj;

	/*! \brief This variable holds the length of time in ms needed to initialize custom hardware
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li CustomHwInit()
	*/
	int32              mCustomHw_InitDelay;    //250ms

	/*! \brief This variable holds the MPP port assigned (depending on feature defenation)
	*		that will be used to turn on/off USB Vbus Boost assist
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li CustomHwReset()
	*   \li GetUsbVbusBoostAssistCapability()
	*   \li TurnOnUsbVbusBoostAssist()
	*   \li TurnOffUsbVbusBoostAssist()
	*   \li CustomHwTimerCb()
	*/
	pm_mpp_which_type  mCustomHw_SwitchTogglerMpp;


	/*! \brief This static variable is used to hold the singlton instance of class CustomHw object
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li GetInstance()
	*   \li DestroyInstance()
	*/
	static CustomHw*	mpCustomHw_Instance;


	/*! \brief This variable holds the timer object pointer
	*   \details This variable will be used/updated by the following member function(s): 
	*   \li CustomHwInit()
	*   \li CustomHwReset()
	*/
	timer_type			mCuHwTimer;
};


#endif //FEATURE_PMIC_USB_OTG
#endif // PMAPP_OTG_CUSTOMHW_H
