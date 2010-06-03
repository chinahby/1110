#ifndef CHG_COMMOM_H
#define CHG_COMMOM_H

/*! \file 
 *  \n
 *  \brief  chg_common.h ---- COMMON DEFINITION
 *  \n
 *  \details This header file contains various definitions that are commonly used 
 *  throughout the charger application
 *  \n
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/src/chg_common.h#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/09   hs      Added support for software controlled CV topoff.
06/03/09   hs      Added support for charging during call.
04/03/09   hs      Added support for unit tests.
03/26/09   hs      Removed support for software controlled charging.
02/11/09   hs      Added pulse charging support.
01/30/09   hs      Added battery removal info.
12/18/08   hs      Moved the definition of the autonomous charging state to the
                   PMIC lib.
11/18/08   hs      Modified the definition of chg_auto_charging_debug_info_type
09/29/08   vk      Added battery removal condition
09/28/08   vk      Fixed compiler warnings
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Added FEATURE_CHG_TASK.
04/16/08   hs      Moved charging related PMIC configurations to "chg_config.h"
06/15/07   hs      Created
===========================================================================*/

/*=========================================================================*/
#include "customer.h"                  /* FEATURE definitions    */

#if defined(FEATURE_CHG_TASK) || defined(T_WINNT)

#ifdef T_WINNT
#error code not present
#endif

#ifdef __cplusplus
extern "C"
{
#endif /* end __cplusplus */
#ifndef T_WINNT
#include "pm.h"                /* For pm functions and type definitions    */
#endif
#include "charger.h"           /* For chg_cmd_type                       */
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/
/*===========================================================================

                               MACROS 

===========================================================================*/
/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \typedef chg_plvl_rtn_type
 *  \brief This type is used on 16-bit signed integer (int16) 
 */
typedef short chg_plvl_rtn_type;
/*! \enum chg_task_evt_id_type
 *  \brief This enum contains the event ID in the charger task
 */
typedef enum
{
    /*! IRQ event                                    */
    CHG_EVT__IRQ,
    /*! Timer event                                  */
    CHG_EVT__TIMER,
    /*! System event                                 */
    CHG_EVT__SYSTEM,
    /*! Invalid event                                */
    CHG_EVT__INVALID
} chg_task_evt_id_type;
/*! \enum chg_system_event_id_type
 *  \brief This enum contains the system event ID
 */
typedef enum
{
    /*! TX on                                        */
    CHG_SYSTEM__TX_ON,  
    /*! TX off                                       */
    CHG_SYSTEM__TX_OFF,  
    /*! FTM wants to turn on charging activities     */
    CHG_SYSTEM__FTM_SWITCH_ON_CHRGING,  
    /*! FTM wants to turn off charging activities    */
    CHG_SYSTEM__FTM_SWITCH_OFF_CHRGING,  
    /*! FTM wants to turn on charging activities     */
    CHG_SYSTEM__SWITCH_ON_AUTO_CHRGING,  
    /*! FTM wants to turn off charging activities    */
    CHG_SYSTEM__SWITCH_OFF_AUTO_CHRGING,  
    /*! USB driver wants to turn on trickle recovery */
    CHG_SYSTEM__TRICKLE_RECOVERY,
    /*! USB device is in suspend mode                */
    CHG_SYSTEM__USB_SUSPEND,
    /*! invalid system event id                      */
    CHG_SYSTEM__INVALID 
} chg_system_event_id_type;
/*! \enum chg_auto_charging_charger_type
 *  \brief This enum contains charger hardware type definition used in automatic charging
 */
typedef enum
{
    /*! No charger                   */
    CHG_AUTO_CHARGING__CHARGER_NONE,
    /*! USB charging port            */
    CHG_AUTO_CHARGING__CHARGER_UCP,
    /*! Standard USB Host Port       */
    CHG_AUTO_CHARGING__CHARGER_SHP,
    /*! Invalid charger type         */
    CHG_AUTO_CHARGING__CHARGER_INVALID
}chg_auto_charging_charger_type;
/*! \enum chg_parameter_type
 *  \brief This enum contains type definition of charging related parameters
 *  \details This type is needed to use the ADC interface to get various charging
 *  related readings.
 */
typedef enum 
{
    /*! The battery Voltage                          */
    CHG_PARAMETER__BATTERY_VOLT,  
    /*! The wall charger voltage                     */
    CHG_PARAMETER__WALL_CHARGER_VOLT,  
    /*! The USB charger Voltage                      */
    CHG_PARAMETER__USB_CHARGER_VOLT,  
    /*! The VPH PWR voltage                          */
    CHG_PARAMETER__VPH_PWR_VOLT,   
    /*! The charger transistor current               */
    CHG_PARAMETER__CHARGER_I,    
    /*! The battery ID.                              */
    CHG_PARAMETER__BATTERY_ID,     
    /*! The battery Temperature.                     */
    CHG_PARAMETER__BATT_THERM_DEGC,     
    /*! The charger Q Temperature.                   */
    CHG_PARAMETER__CHG_THERM_DEGC,     
    /*! Make sure that the ADC driver has at least
      one good reading.                              */
    CHG_PARAMETER__DUMMY_READ,          
    /*! Invalid parameter.                           */
    CHG_PARAMETER__INVALID                            
} chg_parameter_type;
/*! \enum chg_vbat_threshold_type
 *  \brief This enum contains Battery Threshold type 
 */
typedef enum
{
    /*! None.                                        */
    CHG_VBAT_TH_NONE,     
    /*! The lower battery threshold/limit triggered. */
    CHG_VBAT_TH_LOWER,     
    /*! The Upper battery Threshold/limit triggered. */
    CHG_VBAT_TH_UPPER         
} chg_vbat_threshold_type;
/*! \enum chg_debug_inf_type
 *  \brief This enum contains high level of debug information should we print.
 */
typedef enum
{
    CHG_DEBUG_INF_NONE,                  /*!< None.                             */
    CHG_DEBUG_INF_MINIMUM,               /*!< Minimum Messages.                 */
    CHG_DEBUG_INF_MAXIMUM,               /*!< Print all charger debug messages. */
    CHG_DEBUG_INF_INVALID
#ifdef FEATURE_L4LINUX
#error code not present
#endif
}chg_debug_inf_type;
/*! \struct chg_task_evt_type
 *  \brief This enum defines the format of the charging event 
 */
typedef struct
{
    /*! Task event ID.                             */
    chg_task_evt_id_type         event_id;
    /*! Timer token.                               */
    unsigned int                 timer_token;
    /*! IRQ ID.                                    */
    pm_irq_hdl_type              irq_id;
    /*! System event ID.                           */
    chg_system_event_id_type     system_event_id;
    /*! TX voter ID.                               */
    chg_tx_wait_voter_type       tx_voter_id;
} chg_task_evt_type;
/*! \struct chg_usb_imaxsel_type
 *  \brief This type is used to hold the USB transistor IMAXSEL setting
 */
typedef struct
{  
    /*! In milli Amps.                               */
    uint32                         mA;      
    /*! PMIC API parameter.                          */
    pm_chg_uchg_q_imax_type        raw;     
}chg_usb_imaxsel_type;
/*! \struct chg_auto_charging_debug_info_type
 *  \brief This struct contains auto charging debug information that would be output
 *  to various terminals.
 */
typedef struct
{
    /*! The battery voltage                          */
    chg_plvl_rtn_type                   battery_voltage;    
    /*! The battery temperature                      */
    chg_plvl_rtn_type                   battery_temperature;   
    /*! The charging current                         */
    chg_plvl_rtn_type                   charging_current;    
    /*! The current state name                       */
    pm_auto_chg_state_name_type         current_state;
    /*! The previous state name                      */
    pm_auto_chg_state_name_type         previous_state;
    /*! The charger hardware type                    */
    chg_auto_charging_charger_type      charger_type;
    /*! The end-of-charging IRQ triggered            */
    boolean                             end_of_charging_irq_triggered;  
    /*! The fali-of-charging IRQ triggered           */
    boolean                             fail_of_charging_irq_triggered;  
    /*! The change-of-state IRQ triggered            */
    boolean                             change_of_state_irq_triggered;    
    /*! The bad battery temperature IRQ triggered    */
    boolean                             bad_battery_temp_irq_triggered;  
    /*! The VBAT_DET IRQ triggered                   */
    boolean                             vbat_det_irq_triggered;
    /*! The pulse charging done IRQ triggered        */
    boolean                             pulse_done_irq_triggered;
    /*! The battery alarm IRQ triggered              */
    boolean                             batt_alarm_irq_triggered;
    /*! The valid/invalid charger  IRQ triggered     */
    boolean                             charger_irq_triggered;
    /*! The battery connect IRQ triggered           */
    boolean                             battery_connect_irq_triggered;
    /*! The VCP IRQ triggered                       */
    boolean                             vcp_irq_triggered;
    /*! The charger-in-temperature-limiting irq triggered */
    boolean                             chgtlimit_irq_triggered;
    /*! The battery has been removed                 */
    boolean                             battery_removed;
    /*! Pulse charging is on going                   */
    boolean                             pulse_charging_on;
    /*! CV topoff is on going                        */
    boolean                             cv_topoff_on;
    /*! The autonomous charging has finished         */
    boolean                             charging_finished;
    /*! The autonomous charging is in the final fast-charge cycle   */
    boolean                             charging_in_final_cycle;
    /*! The system is in call mode                   */
    boolean                             TX_on;
}chg_auto_charging_debug_info_type;

/*===========================================================================

                        VARIABLE DEFINITIONS 

===========================================================================*/
/*! \var chg_usb_OnOff_nv_item_val
 *  \brief This variable is used to save if NV is set to enable or disable
 *  USB charging  
 *  \li When it's 0, disable USB charging                              
 *  \li When it's 1, enable USB charging                               
 */
extern chg_cmd_type                 chg_usb_OnOff_nv_item_val;
/*! \var chg_usb_imaxsel
 *  \brief This variable is used to hold the USB transistor IMAXSEL setting
 */
extern chg_usb_imaxsel_type         chg_usb_imaxsel;
/*! 
 *  \brief This variable saves the type of USB OTG device attached to the phone. 
 *  \li When it's 0, it's PC USB      
 *  \li When it's 1, it's USB carkit        
 *  \li When it's 2, it's Wall USB charger       
 */
extern pm_app_otg_a_dev_type        chg_usb_otg_dev;
 
/*===========================================================================

                      PROTOTYPE FUNCTION DEFINITIONS

===========================================================================*/
/*! \brief This function returns the ADC readings necessary to do charging
 *  \details This function returns the ADC readings necessary to do charging.  
 *  These include: the battery voltage, the charger voltage, the charger current, 
 *  etc.
 *  \param parm The parameter used to use the ADC interface.
 *  \return An ADC reading.
 *  \note This function should be called when there is a need to get the ADC
 *  readings for the charger application.
 */ 
extern chg_plvl_rtn_type chg_get_parameter_level(chg_parameter_type parm);
/*! \brief This function enables/disables USB charging IRQs
 *  \details This function enables/disables USB charging by initializes the valid 
 *  and invalid IRQs.  
 *  \param cmd The command that enables/disables the USB charging.
 *  \li CHG_CMD_DISABLE Disable the USB charger.
 *  \li CHG_CMD_ENABLE Enable the USb charger.
 *  \return void.
 *  \note This function should be called when there is a need to turn on/off the
 *  USB charging.
 */ 
extern void chg_usb_irq_switch(chg_cmd_type cmd);
/*! \brief This function returns the state of debug messages
 *  \details This function returns the state of debug messages.  
 *  \param void.
 *  \return The debug information.
 *  \retval CHG_DEBUG_INF_NONE None.
 *  \retval CHG_DEBUG_INF_MINIMUM Minimum Messages.
 *  \retval CHG_DEBUG_INF_MAXIMUM Print all charger debug messages.
 *  \note This function should be called when there is a need to get the debug
 *  messages.
 */ 
extern chg_debug_inf_type    chg_get_debug_state(void);
/*! \brief This function enables/disables the debug messages
 *  \details This function enables/disables the debug messages.  
 *  \param cmd The command that enables/disables the debug messages.
 *  \li CHG_DEBUG_INF_NONE None.
 *  \li CHG_DEBUG_INF_MINIMUM Minimum Messages.
 *  \li CHG_DEBUG_INF_MAXIMUM Print all charger debug messages.
 *  \return void
 *  \note This function should be called when there is a need to set the debug
 *  messages.
 */ 
extern void chg_set_debug_state(chg_debug_inf_type cmd);
/*! \brief This function update the charging status
 *  \details This function update the charging status.  
 *  These include: the battery status, the charger status, the chargign state status, 
 *  etc.
 *  \param status The charging related status.
 *  \param mask The status mask.
 *  \return void.
 *  \note This function should be called when there is a need to update the charging 
 *  status.
 */ 
extern void chg_update_charging_status(uint8 status, uint8 mask);
/*! \brief This function puts a charging event to the charger task command
 *  queue
 *  \details This function puts a charging event to the charger task command
 *  queue. This function creates a command based on a charger event passed
 *  by parameter and posts the command to CHG task.
 *  \param event A charging event that's needed to be put on the charger
 *  command queue.
 *  \return void
 *  \note This function should be called when a new charging event is created in the
 *  task context.
 */ 
extern void chg_push_event_on_command_queue( chg_task_evt_type  event );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* FEATURE_CHG_TASK || T_WINNT */
#endif  /* CHG_COMMOM_H */

