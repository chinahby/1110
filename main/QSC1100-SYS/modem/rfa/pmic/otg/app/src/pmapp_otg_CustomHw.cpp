/*! \file 
*  \n
*  \brief  pmapp_otg_CustomHw.cpp  ---- Custom hardware related class implementation
*  \details This file contains the implementation of custom hardware related functionality 
*  developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_CustomHw.cpp#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/08   aab      Added timer_group_type defination
08/08/08   aab      Made changes to properly initialize timer object
07/24/08   aab      Included pmapp_ads_compat.h to resolve ads120 compiler warnings
07/23/08   aab      Made changes to resolve ADS120 compiler warnings
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "pmapp_otg_CustomHw.h"
#include "pmapp_ads_compat.h"

extern "C"
{
#include "err.h"               /*!< For MSG_ERROR() macro */
#include "assert.h"            /*!< For ASSERT() */
}

#ifdef FEATURE_PMIC_USB_OTG


/* ==========================================================================
DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
static timer_group_type  pm_app_otg_timer_group;
CustomHw* CustomHw::mpCustomHw_Instance=NULL;

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
     class CustomHw											  
**************************************************************************/
CustomHw::CustomHw()
{
	mCustomHw_Initialized = FALSE;
	mCustomHwFlag_Vreg5v_ConnectedToUsbVbus = FALSE;
	mCustomHw_InitDoneNotificationCb = NULL;
	mpCallingObj = NULL;
	mCustomHw_InitDelay = 250;    //initialization delay for 250ms
	mCustomHw_SwitchTogglerMpp = PM_MPP_INVALID;
}


CustomHw::~CustomHw()
{
}


CustomHw* CustomHw::GetInstance()
{
	if(NULL == mpCustomHw_Instance )
	{
		mpCustomHw_Instance = new CustomHw;
	}
	return mpCustomHw_Instance;
}

void CustomHw::DestroyInstance()
{
	delete mpCustomHw_Instance;
	mpCustomHw_Instance = NULL;
}



void CustomHw::CustomHwInit( CustomHw_InitDoneNotificationCbType hw_init_done_cb, void* pCallingObj)
{
	pm_err_flag_type  pm_api_status = PM_ERR_FLAG__SUCCESS;

	if ( !mCustomHw_Initialized )
	{
		mCustomHw_InitDoneNotificationCb = hw_init_done_cb;
		mpCallingObj = pCallingObj;

#if defined(FEATURE_PMIC_MPP2_CONTROLS_USB_VBUS_BOOST_ASSIST)
		mCustomHw_SwitchTogglerMpp = PM_MPP_2;
#elif defined(FEATURE_PMIC_MPP7_CONTROLS_USB_VBUS_BOOST_ASSIST)
#error code not present
#elif defined(FEATURE_PMIC_MPP16_CONTROLS_USB_VBUS_BOOST_ASSIST)
		mCustomHw_SwitchTogglerMpp = PM_MPP_16;
#elif defined(FEATURE_PMIC_MPP18_CONTROLS_USB_VBUS_BOOST_ASSIST)
#error code not present
#endif

		if (PM_MPP_INVALID != mCustomHw_SwitchTogglerMpp)
		{
			// set MPPx as digital output to control the transistor switching
			// VREG_5V output onto USB_VBUS; initially drive it LOW to turn off
			// the switch
			pm_api_status = pm_mpp_config_digital_output(
				mCustomHw_SwitchTogglerMpp,
				PM_MPP__DLOGIC__LVL_VDD,
				PM_MPP__DLOGIC_OUT__CTRL_LOW);
			if (PM_ERR_FLAG__SUCCESS != pm_api_status)
			{
				MSG_HIGH("CustomHw_process_new_state_entry - MPPx config failed! [%d]", pm_api_status, 0, 0);
			}

			// speed up the process of pulling USB_VBUS low by activating the DISCHG resistor
			(void) pm_otg_config_interface(
				PM_OTG_VBUS_CONFIG_CMD__DISCHG,
				PM_OTG_DP_CONFIG_CMD__NONE,
				PM_OTG_DM_CONFIG_CMD__NONE,
				PM_OTG_ID_CONFIG_CMD__NONE);
		}

        #ifndef T_WINNT      
		// start a timer for the longest time it could take for USB_VBUS to go to 0V - TBD
        timer_def2(&mCuHwTimer, &pm_app_otg_timer_group );

		timer_reg(&mCuHwTimer,					/* Timer to set */
			(timer_t2_cb_type)CustomHwTimerCb,/* Function to call at timer expiry */
			(timer_cb_data_type)this,	/* Arbitrary data for func(time_ms, data). */
			mCustomHw_InitDelay,		/* Time (in ms) until first timer expiry */
			0);							/* Period (in ms) between repeated expiries (0 = not periodic) */
         #endif

		mCustomHw_Initialized = TRUE;
	}
}




void CustomHw::CustomHwReset(void)
{
	if ( mCustomHw_Initialized )
	{
        #ifndef T_WINNT
		    timer_clr( &mCuHwTimer, T_NONE );
        #endif

		mCustomHw_InitDoneNotificationCb = NULL;

		TurnOnUsbVbusBoostAssist();
		mCustomHw_SwitchTogglerMpp = PM_MPP_INVALID;

		mCustomHw_Initialized = FALSE;
	}
}




uint32 CustomHw::GetUsbVbusBoostAssistCapability(void)
{
	static const uint32 VBUS_BOOST_ASSIST_RATING_MA = 100;

	return (PM_MPP_INVALID == mCustomHw_SwitchTogglerMpp)
		? 0 // custom h/w not configured
		: VBUS_BOOST_ASSIST_RATING_MA; // custom h/w configured
}



void CustomHw::TurnOnUsbVbusBoostAssist(void)
{   
	if (PM_MPP_INVALID != mCustomHw_SwitchTogglerMpp)
	{
		if (!mCustomHwFlag_Vreg5v_ConnectedToUsbVbus)
		{
			// drive MPPx HIGH
			(void) pm_mpp_config_digital_output(
				mCustomHw_SwitchTogglerMpp,
				PM_MPP__DLOGIC__LVL_VDD,
				PM_MPP__DLOGIC_OUT__CTRL_HIGH);
			mCustomHwFlag_Vreg5v_ConnectedToUsbVbus = TRUE;
		}
	}
}



void CustomHw::TurnOffUsbVbusBoostAssist(void)
{
	if (PM_MPP_INVALID != mCustomHw_SwitchTogglerMpp)
	{
		if (mCustomHwFlag_Vreg5v_ConnectedToUsbVbus)
		{
			// drive MPPx LOW
			(void) pm_mpp_config_digital_output(
				mCustomHw_SwitchTogglerMpp,
				PM_MPP__DLOGIC__LVL_VDD,
				PM_MPP__DLOGIC_OUT__CTRL_LOW);
			mCustomHwFlag_Vreg5v_ConnectedToUsbVbus = FALSE;
		}
	}
}




void CustomHw::CustomHwTimerCb(int32 time_ms, timer_cb_data_type token)
{
	CustomHw* pCustomHw = static_cast< CustomHw* >((void*)token);
	ASSERT( pCustomHw != NULL );

    if (time_ms == pCustomHw->mCustomHw_InitDelay) {
       //delay value matches the actual setup value
    }
    

    #ifndef T_WINNT
	     timer_clr( &(pCustomHw->mCuHwTimer), T_NONE );
    #endif

	if (PM_MPP_INVALID != pCustomHw->mCustomHw_SwitchTogglerMpp)
	{
		// disable the USB_VBUS DISCHG resistor
		(void) pm_otg_config_interface(
			PM_OTG_VBUS_CONFIG_CMD__DISABLE_DISCHG,
			PM_OTG_DP_CONFIG_CMD__NONE,
			PM_OTG_DM_CONFIG_CMD__NONE,
			PM_OTG_ID_CONFIG_CMD__NONE);
	}

	pCustomHw->mCustomHw_InitDoneNotificationCb(pCustomHw->mpCallingObj);
}


#endif //FEATURE_PMIC_USB_OTG
