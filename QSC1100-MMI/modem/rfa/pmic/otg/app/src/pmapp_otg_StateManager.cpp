/*! \file 
*  \n
*  \brief  pmapp_otg_StateManager.cpp  ----- PM APP OTG state manager related class implementation
*  \details This file contains implementation of classes for OTG state Manager
*     used by the USB-OTG application developed for the Qualcomm Power Manager Chip Set.
*  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
*/

/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/otg2/main/latest/app/src/pmapp_otg_StateManager.cpp#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/18/09   aab      Updated StateManager::PerformCommonDualRoleDevInit(): 
		    		- Configure USB OTG transceiver to Full Speed during initialization of the driver.
		    		- This helps standard USB wall Charger detection more reliably.
11/26/08   aab      Added FEATURE_PMIC_KIP_HAN to support KIP/AN PMIC's regester defination variation
08/04/08   aab      Added FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
07/23/08   aab      Made changes to resolve compiler warnings
04/01/08   aab      Created

========================================================================== */

/* ==========================================================================

INCLUDE FILES FOR MODULE

========================================================================== */
#include "pmapp_otg_Interface.h"   

#ifdef FEATURE_PMIC_USB_OTG

/* ==========================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
//initialize static variables
StateManager* StateManager::mpStateManager_Instance=NULL;
StateFactory* StateFactory::mpStateFactory_Instance=NULL;

BaseState* StateManager::mpCurrState=NULL;  
BaseState* StateManager::mpPrevState=NULL;  

StateChangeEvt* StateChangeEvt::mpStateChangeEvt_Instance=NULL;

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
     class StateManager											  
**************************************************************************/

StateManager::StateManager()
{
	mpUsbBusListener = UsbBusListener::GetInstance();
	mpEvtGen = EvtGen::GetInstance();
	mpStateChangeEvt = StateChangeEvt::GetInstance();

	mpVbusValidStateMonitor = VbusValidStateMonitor::GetInstance();
	mpSessionValidStateMonitor = SessionValidStateMonitor::GetInstance();

	mpDpStateMonitor   = DpStateMonitor::GetInstance();
	mpDmStateMonitor   = DmStateMonitor::GetInstance();

	mpIdFltStateMonitor = IdFltStateMonitor::GetInstance();
	mpIdGndStateMonitor = IdGndStateMonitor::GetInstance();
	mpIdResStateMonitor = IdResStateMonitor::GetInstance();
}




void StateManager::InitStateManager()
{
	//instantiate all otg state objects
	//regester to EvtGen...(we are not using this observer pattern method for now)
	// mpEvtGen->AddListener(this); 

	StateFactory* pStateFactory = StateFactory::GetInstance();
	pStateFactory->StateFactoryInit();

	mpCurrState = pStateFactory->mpNull_State;
	mpPrevState = mpCurrState;
}



StateManager::~StateManager()
{
	//mpEvtGen->RemoveListener(this); we are not using this registeration method now; observer pattern implementation
	StateFactory::DestroyInstance();

	UsbBusListener::DestroyInstance();
	EvtGen::DestroyInstance();
	StateChangeEvt::DestroyInstance();

	VbusValidStateMonitor::DestroyInstance();
	SessionValidStateMonitor::DestroyInstance();

	DpStateMonitor::DestroyInstance();
	DmStateMonitor::DestroyInstance();

	IdFltStateMonitor::DestroyInstance();
	IdGndStateMonitor::DestroyInstance();
	IdResStateMonitor::DestroyInstance();
}



StateManager* StateManager::GetInstance()
{
	if(mpStateManager_Instance == NULL){
		mpStateManager_Instance = new StateManager;
	}
	return mpStateManager_Instance;
}

void StateManager::DestroyInstance()
{
	delete mpStateManager_Instance;
	mpStateManager_Instance = NULL;
}


void StateManager::ProcessCachedEvt() 
{
	// on_stack_count_ variables help in properly executing state
	// transitions if this function happens to be called recursively
	static volatile unsigned short int on_stack_count__water_mark = 0;
	BaseState* next_state_ptr = NULL;
	boolean cached_events_pending = false;

	INTLOCK();
	on_stack_count__water_mark ++;
	INTFREE();

	if ( 1 == on_stack_count__water_mark )
	{
		do
		{
			next_state_ptr = mpCurrState->CachedEvtHdlr();
            if(NULL == next_state_ptr)
            {
               ERR_FATAL("ERROR: Invalid OTG State", 0,0,0);
            }

			// cached event could have resulted in a state transition as well as other new cached events
			while ( next_state_ptr != mpCurrState )
			{
				mpCurrState = next_state_ptr;  // Continue to process_cached_evt() as long as state transitions occur
				mpCurrState->Entry(); //entry function of the next state
				next_state_ptr = mpCurrState->CachedEvtHdlr(); //process cached evt fn
				//generate state change event
				mpStateChangeEvt->SetHigh();  //when do we reset it back? Inside evt_debug class after processing the event
			}

			INTLOCK();
			if (on_stack_count__water_mark > 0)
			{
				on_stack_count__water_mark --;
			}
			cached_events_pending = (on_stack_count__water_mark > 0);
			INTFREE();

		} while ( cached_events_pending );
	} 
}




void StateManager::InitEvtGenEngine(void)
{
    
#ifndef FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE
	// VBUS voltage monitoring state machine initialization
	//mpUsbBusListener is used to get a ptr to UsbBusListener object back (when the call back happens)
	mpVbusValidStateMonitor->VbusValidStateMonitorInit(mpUsbBusListener->VbusListener, mpUsbBusListener);	 // updates - pm_app_otg_evt__a_vbus_vld
	mpSessionValidStateMonitor->SessionValidStateMonitorInit(mpUsbBusListener->VbusListener, mpUsbBusListener);  // updates - pm_app_otg_evt__a_sess_vld and  pm_app_otg_evt__b_sess_vld
#endif //FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE


#ifndef FEATURE_PMIC_USB_ID_SIGNALING_UNAVAILABLE
	// ID voltage monitoring state machine initialization
	//mpIdStateMonitor->IdStateMonitorInit(mpUsbBusListener->IdListener, mpUsbBusListener);     // updates - pm_app_otg_evt__id  resistive_id_flag

	//TODO: try to make a single call that does the following three calls 
	mpIdFltStateMonitor->IdFltStateMonitorInit(mpUsbBusListener->IdListener, mpUsbBusListener);      // updates - pm_app_otg_evt__id  
	mpIdGndStateMonitor->IdGndStateMonitorInit(mpUsbBusListener->IdListener, mpUsbBusListener);      // updates - pm_app_otg_evt__id  
	mpIdResStateMonitor->IdResStateMonitorInit(mpUsbBusListener->IdListener, mpUsbBusListener);      // updates - pm_app_otg_evt__id  resistive_id_flag

#endif //FEATURE_PMIC_USB_ID_SIGNALING_UNAVAILABLE


#ifdef FEATURE_PMIC_USB_OTG_ALWAYS_UNCONDITIONAL_B_DEV_SESSION
    PmAppOtg* pPmAppOtg = PmAppOtg::GetInstance();
	pPmAppOtg->Start_B_DevSession();
#endif //FEATURE_PMIC_USB_OTG_ALWAYS_UNCONDITIONAL_B_DEV_SESSION
}




void StateManager::ResetEvtGenEngine(void)
{
	EvtUtil* pEvtUtil = EvtUtil::GetInstance();

#ifndef FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE
	// reset VBUS voltage monitoring state machine
	// updates - pm_app_otg_evt__a_vbus_vld
	//           pm_app_otg_evt__a_sess_vld
	//           pm_app_otg_evt__b_sess_vld
	mpVbusValidStateMonitor->VbusValidStateMonitorReset();
	mpSessionValidStateMonitor->SessionValidStateMonitorReset();

	pEvtUtil->a_vbus_vld_evt->Reset();
	pEvtUtil->a_sess_vld_evt->Reset();
	pEvtUtil->b_sess_vld_evt->Reset();


#endif //FEATURE_PMIC_USB_VBUS_SIGNALING_UNAVAILABLE

#ifndef FEATURE_PMIC_USB_ID_SIGNALING_UNAVAILABLE
	// ID voltage monitoring state machine             
	// updates - pm_app_otg_evt__id
	//           resistive_id_flag
    Util* pUtil = Util::GetInstance();
	mpIdFltStateMonitor->IdFltStateMonitorReset();
	mpIdGndStateMonitor->IdGndStateMonitorReset();
	mpIdResStateMonitor->IdResStateMonitorReset();

	pEvtUtil->id_evt->Reset();
	pUtil->resistive_id_flag = FALSE;
#endif //FEATURE_PMIC_USB_ID_SIGNALING_UNAVAILABLE

#ifdef FEATURE_PMIC_USB_OTG_ALWAYS_UNCONDITIONAL_B_DEV_SESSION
	pm_app_otg_stop_b_dev_session();
#endif //FEATURE_PMIC_USB_OTG_ALWAYS_UNCONDITIONAL_B_DEV_SESSION
}




void StateManager::CustomHwInitDoneCb(void* pObj) // Callback function for getting notified about completion of custom hardware initialization
{
	StateManager* pStateManager = static_cast< StateManager* >((void*)pObj);
	ASSERT( pStateManager != NULL );

	EvtUtil* pEvtUtil = EvtUtil::GetInstance();
	pEvtUtil->custom_hw_ready_evt->SetHigh();

	pStateManager->ProcessCachedEvt();
}




void StateManager::PerformCommonDualRoleDevInit(void) // initializations common to both dual role devices - A and B
{
	// reset OTG transceiver's mode
	(void) pm_otg_reset_mode();

    // Configure OTG transceiver to full speed during initialization of the driver.
    // This is done to take care of standard USB wall Charger detection issue
    (void) pm_otg_config_usb_mode(PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);

	// suspend it for maximum power savings while still being able to detect
	// connection of external OTG devices
	(void) pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND);

   #ifndef FEATURE_PMIC_MULTIPLEXED_USB_DP_DM
      	// pull down D+/- to prevent them from floating around
      	(void) pm_otg_config_interface(
      		PM_OTG_VBUS_CONFIG_CMD__NONE,
      		PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
      		PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
      		PM_OTG_ID_CONFIG_CMD__NONE );
   #else
      	// leave D+/- floating so that the external h/w block that is 
      	// multiplexed to use USB D+/- could use it for its own purpose without
      	// any interference from PMIC OTG xcvr
      	(void) pm_otg_config_interface(
      		PM_OTG_VBUS_CONFIG_CMD__NONE,
      		PM_OTG_DP_CONFIG_CMD__DISABLE_BOTH,
      		PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH,
      		PM_OTG_ID_CONFIG_CMD__NONE );
   #endif //FEATURE_PMIC_MULTIPLEXED_USB_DP_DM


   #ifndef FEATURE_PMIC_NONFUNCTIONAL_VREG_USB
	//Turn OFF VREG_USB LDO
      #ifdef FEATURE_PMIC_KIP_HAN
         (void) pm_vreg_control(PM_OFF_CMD, PM_VREG_USB3P3_M);
      #else //PMIC3
         (void) pm_vreg_control(PM_OFF_CMD, PM_VREG_USB_M);
      #endif //FEATURE_PMIC_KIP_HAN
   #endif //FEATURE_PMIC_NONFUNCTIONAL_VREG_USB

	// vote to turn OFF BOOST_5V
	pm_vote_vreg_switch(
		PM_OFF_CMD,
		PM_VREG_BOOST_ID,
		PM_VOTE_VREG_BOOST_APP__OTG);
}






/**************************************************************************
     class StateFactory											  
**************************************************************************/
StateFactory::StateFactory()
{
   mpNull_State = NULL;
   mpB_Idle_State = NULL;
   mpB_RemoteDevDetectWait_State = NULL;
   mpB_Peripheral_State = NULL;
   mpB_Suspend_State = NULL;
   mpChg_Conn_State = NULL;
   mpUnsought_Mode_State = NULL;

#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
   mpA_Idle_State = NULL;
   mpA_Wait_Vrise_State = NULL;
   mpA_Wait_Bcon_State = NULL;
   mpA_Host_State = NULL;
   mpA_Suspend_State = NULL;
   mpA_vbus_Err_State = NULL;
   mpA_Wait_Vfall_State = NULL;
   mpRs232dongle_Config_State = NULL;
   mpCarkitclient_Init_State = NULL;
   mpCarkitclient_Uart_State = NULL;
   mpCarkitclient_Audio_State = NULL;
   mpCarkitclient_Carkit_Irq_State = NULL;
#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION   
}


StateFactory::~StateFactory()
{
	delete mpNull_State;
    delete mpB_Idle_State;
	delete mpB_RemoteDevDetectWait_State;
	delete mpB_Peripheral_State;
	delete mpB_Suspend_State;
    delete mpChg_Conn_State;
	delete mpUnsought_Mode_State;

#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
    delete mpA_Idle_State;
    delete mpA_Wait_Vrise_State;
    delete mpA_Wait_Bcon_State;
    delete mpA_Host_State;
    delete mpA_Suspend_State;
    delete mpA_vbus_Err_State;
    delete mpA_Wait_Vfall_State;
    delete mpRs232dongle_Config_State;
	delete mpCarkitclient_Init_State;
	delete mpCarkitclient_Uart_State;
	delete mpCarkitclient_Audio_State;
	delete mpCarkitclient_Carkit_Irq_State;
#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION

}


StateFactory* StateFactory::GetInstance()
{
	if(NULL == mpStateFactory_Instance )
	{
		mpStateFactory_Instance = new StateFactory;
	}
	return mpStateFactory_Instance;
}


void StateFactory::DestroyInstance()
{
	delete mpStateFactory_Instance;
	mpStateFactory_Instance = NULL;
}


void StateFactory::StateFactoryInit()
{
	mpNull_State = GetStateInstance(PM_APP_OTG_STATE__NULL);
    mpB_Idle_State = GetStateInstance(PM_APP_OTG_STATE__B_IDLE);
	mpB_RemoteDevDetectWait_State = GetStateInstance(PM_APP_OTG_STATE__B_REMOTE_DEV_DETECT_WAIT);
	mpB_Peripheral_State = GetStateInstance(PM_APP_OTG_STATE__B_PERIPHERAL);
	mpB_Suspend_State = GetStateInstance(PM_APP_OTG_STATE__B_SUSPEND);
    mpChg_Conn_State = GetStateInstance(PM_APP_OTG_STATE__CHG_CONN);
	mpUnsought_Mode_State = GetStateInstance(PM_APP_OTG_STATE__UNSOUGHT_MODE);

#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
    //host mode
    mpA_Idle_State = GetStateInstance(PM_APP_OTG_STATE__A_IDLE);
	mpA_Wait_Vrise_State = GetStateInstance(PM_APP_OTG_STATE__A_WAIT_VRISE);
	mpA_Wait_Bcon_State = GetStateInstance(PM_APP_OTG_STATE__A_WAIT_BCON);
	mpA_Host_State = GetStateInstance(PM_APP_OTG_STATE__A_HOST);
	mpA_Suspend_State = GetStateInstance(PM_APP_OTG_STATE__A_SUSPEND);
	mpA_vbus_Err_State = GetStateInstance(PM_APP_OTG_STATE__A_VBUS_ERR);
	mpA_Wait_Vfall_State = GetStateInstance(PM_APP_OTG_STATE__A_WAIT_VFALL);

    mpRs232dongle_Config_State = GetStateInstance(PM_APP_OTG_STATE__RS232DONGLE_CONFIG);

    //carkit mode
	mpCarkitclient_Init_State = GetStateInstance(PM_APP_OTG_STATE__CARKITCLIENT_INIT);
	mpCarkitclient_Uart_State = GetStateInstance(PM_APP_OTG_STATE__CARKITCLIENT_UART);
	mpCarkitclient_Audio_State = GetStateInstance(PM_APP_OTG_STATE__CARKITCLIENT_AUDIO);
	mpCarkitclient_Carkit_Irq_State = GetStateInstance(PM_APP_OTG_STATE__CARKITCLIENT_CARKIT_IRQ);
#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION

}



BaseState* StateFactory::GetStateInstance(StateEventIdType mStateId)
{
	BaseState* state_ptr;

	switch(mStateId)
	{

#ifndef FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION
	case PM_APP_OTG_STATE__A_IDLE:
		if(NULL == mpA_Idle_State)
			state_ptr = new A_Idle_State;
		else
			state_ptr = mpA_Idle_State;
		break;

	case PM_APP_OTG_STATE__A_WAIT_VRISE:
		if(NULL == mpA_Wait_Vrise_State)
			state_ptr = new A_Wait_Vrise_State;
		else
			state_ptr = mpA_Wait_Vrise_State;
		break;

	case PM_APP_OTG_STATE__A_WAIT_BCON:
		if(NULL == mpA_Wait_Bcon_State)
			state_ptr = new A_Wait_Bcon_State;
		else
			state_ptr = mpA_Wait_Bcon_State;
		break;

	case PM_APP_OTG_STATE__A_HOST:
		if(NULL == mpA_Host_State)
			state_ptr = new A_Host_State;
		else
			state_ptr = mpA_Host_State;
		break;

	case PM_APP_OTG_STATE__A_SUSPEND:
		if(NULL == mpA_Suspend_State)
			state_ptr = new A_Suspend_State;
		else
			state_ptr = mpA_Suspend_State;
		break;

	case PM_APP_OTG_STATE__A_VBUS_ERR:
		if(NULL == mpA_vbus_Err_State)
			state_ptr = new A_Vbus_Err_State;
		else
			state_ptr = mpA_vbus_Err_State;
		break;

	case PM_APP_OTG_STATE__A_WAIT_VFALL:
		if(NULL == mpA_Wait_Vfall_State)
			state_ptr = new A_Wait_Vfall_State;
		else
			state_ptr = mpA_Wait_Vfall_State;
		break;


    case PM_APP_OTG_STATE__RS232DONGLE_CONFIG:
        if(NULL == mpRs232dongle_Config_State)
            state_ptr = new Rs232Dongle_Config_State;
        else
            state_ptr = mpRs232dongle_Config_State;
        break;

    case PM_APP_OTG_STATE__CARKITCLIENT_INIT:
        if(NULL == mpCarkitclient_Init_State)
            state_ptr = new Carkitclient_Init_State;
        else
            state_ptr = mpCarkitclient_Init_State;
        break;

    case PM_APP_OTG_STATE__CARKITCLIENT_UART:
        if(NULL == mpCarkitclient_Uart_State)
            state_ptr = new Carkitclient_Uart_State;
        else
            state_ptr = mpCarkitclient_Uart_State;
        break;

    case PM_APP_OTG_STATE__CARKITCLIENT_AUDIO:
        if(NULL == mpCarkitclient_Audio_State)
            state_ptr = new Carkitclient_Audio_State;
        else
            state_ptr = mpCarkitclient_Audio_State;
        break;

    case PM_APP_OTG_STATE__CARKITCLIENT_CARKIT_IRQ:
        if(NULL == mpCarkitclient_Carkit_Irq_State)
            state_ptr = new Carkitclient_Carkit_Irq_State;
        else
            state_ptr = mpCarkitclient_Carkit_Irq_State;
        break;

#endif //FEATURE_PMIC_PERIPHERAL_MODE_ONLY_OPERATION

	case PM_APP_OTG_STATE__NULL:
		if(NULL == mpNull_State)
			state_ptr = new Null_State;
		else
			state_ptr = mpNull_State;
		break;

	case PM_APP_OTG_STATE__B_IDLE:
		if(NULL == mpB_Idle_State)
			state_ptr = new B_Idle_State;
		else
			state_ptr = mpB_Idle_State;
		break;

	case PM_APP_OTG_STATE__B_REMOTE_DEV_DETECT_WAIT:
		if(NULL == mpB_RemoteDevDetectWait_State)
			state_ptr = new B_RemoteDevDetectWait_State;
		else
			state_ptr = mpB_RemoteDevDetectWait_State;
		break;

	case PM_APP_OTG_STATE__B_PERIPHERAL:
		if(NULL == mpB_Peripheral_State)
			state_ptr = new B_Peripheral_State;
		else
			state_ptr = mpB_Peripheral_State;
		break;

	case PM_APP_OTG_STATE__B_SUSPEND:
		if(NULL == mpB_Suspend_State)
			state_ptr = new B_Suspend_State;
		else
			state_ptr = mpB_Suspend_State;
		break;

	case PM_APP_OTG_STATE__CHG_CONN:
		if(NULL == mpChg_Conn_State)
			state_ptr = new Chg_Conn_State;
		else
			state_ptr = mpChg_Conn_State;
		break;

	case PM_APP_OTG_STATE__UNSOUGHT_MODE:
		if(NULL == mpUnsought_Mode_State)
			state_ptr = new Unsought_Mode_State;
		else
			state_ptr = mpUnsought_Mode_State;
		break;

	case PM_APP_OTG_STATE__INVALID:
		state_ptr = NULL;
		break;

	default:
		state_ptr = NULL;
		break;
	}

	return state_ptr;
}




/**************************************************************************
     class StateChangeEvt											  
**************************************************************************/
StateChangeEvt::StateChangeEvt()
{
	mInfoAvailable=false;
	mHappened=false;
	mpPrevState=NULL;
	mpCurrState=NULL;
} 

StateChangeEvt::~StateChangeEvt()
{
}


StateChangeEvt* StateChangeEvt::GetInstance()
{
	if(mpStateChangeEvt_Instance == NULL){
		mpStateChangeEvt_Instance = new StateChangeEvt;
	}
	return mpStateChangeEvt_Instance;
}

void StateChangeEvt::DestroyInstance()
{
	delete mpStateChangeEvt_Instance;
	mpStateChangeEvt_Instance = NULL;
}



void StateChangeEvt::SetHigh()//override the base class implementation
{
	mInfoAvailable=true;
	if(mHappened==false)
	{
		mHappened=true; 
		mpEvtGen->NotifyStateChage();
	}
}


void StateChangeEvt::SetLow() //override the base class implementation
{
	mInfoAvailable=true;
	if(mHappened==true)
	{
		mHappened=false; 
		mpEvtGen->NotifyStateChage();
	}
}


#endif //FEATURE_PMIC_USB_OTG
