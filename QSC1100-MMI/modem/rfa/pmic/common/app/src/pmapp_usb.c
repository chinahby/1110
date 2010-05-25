/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                 P M A P P S

GENERAL DESCRIPTION
  This file contains functions and corresponding variable 
  declarations to support applications for the Qualcomm Power Manager Chips
  Set.

Copyright (c) 2003,2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007,2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_usb.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/09/09   aab     - Added error checking on pm_app_register_for_vbus_sn_valid_status()
02/06/09   aab     - Added pm_vote_usb_pwr_sel_switch() API to control Vreg_5V/Vbus
                     switch from multiple application Used pm_vote_vreg_request_vote()
                      to get application voting id to control Boost Regulator
                   - Added pm_app_register_for_vbus_sn_valid_status():  It is a 
                     callback registeration function used to register for PMIC 
                     Vbus session valid and Invalid events.
                                      
10/21/08   aab     Initial Revision.
                   Added FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION
                   this feature is used to select USB_LDO power input(VREG_5V or USB VBus) based 
                   on USB Vbus session valid interrupt.  For HS-USB targets, the VBus line runs 
                   to the PMIC and session_valid interrupt is monitored contioniously and 
                   input of the the LDO is switched to Vbus line if we get RE interrupt.
                   If we get Session valid FE interrupt, the Vbus line is switched to VREG_5V   

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"                   

#if defined (FEATURE_PMIC)
#include "pmapp_usb.h"
#include "pmapp_irq_debouncer.h"
#endif


#if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_STANDALONE_MODEM)
#error code not present
#endif /* FEATURE_MULTIPROCESSOR && !FEATURE_STANDALONE_MODEM */



/*===========================================================================

                        TYPE DEFINITIONS
                        
===========================================================================*/
/*
vbus session valid state monitor state type
*/
typedef enum
{
   PM_APP_VBUS_SM_INFO__VBUS_SESSION_VALID,
   PM_APP_VBUS_SM_INFO__VBUS_SESSION_INVALID,
   PM_APP_VBUS_SM_INFO__INVALID
} pm_app_vbus_session_valid_sm_info_type;


/*
Vbus session vaid state monitor listener type
*/
typedef void (*pm_app_vbus_session_valid_sm_listener_type) (pm_app_vbus_session_valid_sm_info_type);



/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
static void pm_app_vbus_session_valid_sm_init(pm_app_vbus_session_valid_sm_listener_type listener);
static void pm_app_vbus_session_valid_sm_listener(pm_app_vbus_session_valid_sm_info_type info);


// debouncer related fns for the VBUS_Valid comparator
static void pm_app_vbus_session_valid_status_cb(pm_app_irq_debouncer_status_type status);

static void pm_app_vbus_session_valid_debouncer_delegate_fn1(
   struct pm_app_timer_struct_type    *param1_ptr,
   pm_app_timer_sess_id_type           param2,
   int32                               param3);

static void pm_app_vbus_session_valid_debouncer_delegate_fn2(void);




/*===========================================================================

                VARIABLES DEFINITIONS
                
===========================================================================*/
/* This table allows multiple application to control the same PM switch. Each item 
   represents a PM switch, each bit within an item represents an application. If
   all bits within an item are equal to 0 then no application requires the PM Switch
   to be turned ON, so it is OK to turn OFF the switch. 
*/
static uint32 pm_vote_usb_pwr_sel_switch_table[PM_USB_PWR_SEL_SWITCH_ID_INVALID]={0};

static boolean pm_app_vbus_session_valid_sm_initialized = FALSE;
static pm_app_vbus_session_valid_sm_listener_type  pm_app_vbus_session_valid_sm__my_listener = NULL;

static pm_app_irq_debouncer_instance_type  pm_app_vbus_session_valid_debouncer;
static const int32 pm_app_vbus_session_valid_debouncer_tmr__both_irq_off = 67;
static const int32 pm_app_vbus_session_valid_debouncer_tmr__comp_irq_on = 67;

static pm_app_usb_vbus_sn_valid_status_cb_type vbus_sn_valid_status_cb = NULL;



/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/
void pm_app_config_usb_ldo_input() //called from pm init
{
   // Power up from suspended state
   pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

   //initialize USB device detection
   pm_app_vbus_session_valid_sm_init(pm_app_vbus_session_valid_sm_listener);
}




static void pm_app_vbus_session_valid_sm_listener(pm_app_vbus_session_valid_sm_info_type info)
{
   switch (info)
   {
   case PM_APP_VBUS_SM_INFO__VBUS_SESSION_VALID:
           if(NULL != vbus_sn_valid_status_cb)
           {
              vbus_sn_valid_status_cb(PM_USB_VBUS_SN_VALID);
           }

           break;

   case PM_APP_VBUS_SM_INFO__VBUS_SESSION_INVALID:
            if(NULL != vbus_sn_valid_status_cb)
            {
               vbus_sn_valid_status_cb(PM_USB_VBUS_SN_INVALID);
            }
            break;
   case PM_APP_VBUS_SM_INFO__INVALID:
   default:
      MSG_ERROR("ERROR: pm_app_vbus_session_valid_sm_listener(), Parm 1 is out of range = %d.",info,0,0);
      break;
   };
}



void pm_app_vbus_session_valid_sm_init(pm_app_vbus_session_valid_sm_listener_type listener)
{
   pm_err_flag_type  init_status__vbus_valid = PM_ERR_FLAG__SUCCESS;

   if (!pm_app_vbus_session_valid_sm_initialized)
   {
      pm_app_vbus_session_valid_sm__my_listener = listener;

      init_status__vbus_valid =
         pm_app_irq_debouncer_init(
            &pm_app_vbus_session_valid_debouncer,
            PM_USB_SN_VALID_RE_IRQ_HDL,
            PM_USB_SN_VALID_FE_IRQ_HDL,
            PM_USB_SN_VALID_RT_ST,
            pm_app_vbus_session_valid_debouncer_tmr__both_irq_off,
            pm_app_vbus_session_valid_debouncer_tmr__comp_irq_on,
            pm_app_vbus_session_valid_status_cb,
            pm_app_vbus_session_valid_debouncer_delegate_fn1,
            pm_app_vbus_session_valid_debouncer_delegate_fn2);

      pm_app_vbus_session_valid_sm_initialized = ( (PM_ERR_FLAG__SUCCESS == init_status__vbus_valid) );

      if (!pm_app_vbus_session_valid_sm_initialized)
      {
         // debouncers failed to initialize properly, so reset
         (void) pm_app_irq_debouncer_reset(&pm_app_vbus_session_valid_debouncer);
      }
   }
}



static void pm_app_vbus_session_valid_status_cb(pm_app_irq_debouncer_status_type status)
{
   pm_app_vbus_session_valid_sm_info_type vbus_valid_info = PM_APP_VBUS_SM_INFO__VBUS_SESSION_INVALID;

   if (PM_APP_IRQ_DEBOUNCER_STATUS__HIGH == status)
   {
      vbus_valid_info = PM_APP_VBUS_SM_INFO__VBUS_SESSION_VALID;
   }

   // notify our listener about updates to vbus_valid condition ...
   pm_app_vbus_session_valid_sm__my_listener(vbus_valid_info);
}


static void pm_app_vbus_session_valid_debouncer_delegate_fn1(
   struct pm_app_timer_struct_type    *param1_ptr,
   pm_app_timer_sess_id_type           param2,
   int32                               param3)
{
   pm_app_irq_debouncer_process_delegate_fn1(
      &pm_app_vbus_session_valid_debouncer,
      pm_app_vbus_session_valid_debouncer_delegate_fn1,
      param1_ptr,
      param2,
      param3);
}



static void pm_app_vbus_session_valid_debouncer_delegate_fn2(void)
{
   pm_app_irq_debouncer_process_delegate_fn2(
      &pm_app_vbus_session_valid_debouncer,
      pm_app_vbus_session_valid_debouncer_delegate_fn2);
}



void pm_vote_usb_pwr_sel_switch( pm_usb_pwr_sel_switch_cmd_type cmd, 
                                 pm_usb_pwr_sel_switch_id_type switch_id,
                                 pm_vote_usb_pwr_sel_switch_app_type app_mask )
{
    pm_err_flag_type      err = PM_ERR_FLAG__SUCCESS;
    

    /* Make sure that this is a valid switch_id */
    if( switch_id < PM_USB_PWR_SEL_SWITCH_ID_INVALID )
    {
        INTLOCK();

        switch(cmd)
        {
            case PM_SWITCH_ON_CMD:
                /* if no application has voted to turned on this Switch yet */
                if ( pm_vote_usb_pwr_sel_switch_table[switch_id] == 0x0 )
                {
                    /* Enable the the switch. */
                   err = pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);  //Switch to VREG_5V 
                }

                /* Set the app bit mask. */
                pm_vote_usb_pwr_sel_switch_table[switch_id] |= (uint32)app_mask;
                break;
        
            case PM_SWITCH_OFF_CMD:
                /* Clear the app bit mask.*/
                pm_vote_usb_pwr_sel_switch_table[switch_id] &= ~((uint32)app_mask);

                /* if no other application needs this Switch set to VREG_5V side, switch it to Vbus side */
                if ( pm_vote_usb_pwr_sel_switch_table[switch_id] == 0x0 )
                {
                    err = pm_otg_pwr_sel(PM_OTG_PWR_SEL__VBUS); //Switch to VBUS 
                }
                break;

            default:
                MSG_ERROR("Parm 1 is out of range = %d.",cmd,0,0);
        }
        INTFREE();

        /* Find out if there was PMIC API error. */
        if (err != PM_ERR_FLAG__SUCCESS) 
        {
          MSG_ERROR("PMIC API ERROR(0x%x) DETECTED",err,0,0);
        }
    }
    else
    {
        MSG_ERROR("PMIC ERROR pm_vote_usb_pwr_sel_switch - invalid usb_pwr_id: 0x%x", switch_id,0,0);
    }
}



void pm_app_register_for_vbus_sn_valid_status(pm_app_usb_vbus_sn_valid_status_cb_type pm_vbus_sn_valid_status_cb)
{
   #ifndef FEATURE_PMIC_SELECT_USB_LDO_INPUT_BASED_ON_USB_DEVICE_CONNECTION
      pm_vbus_sn_valid_status_cb(PM_USB_FEATURE_NOT_DEFINED); 
   #else
      vbus_sn_valid_status_cb = pm_vbus_sn_valid_status_cb;
   #endif
}


