#ifndef CHARGER_H
#define CHARGER_H

/*! \file 
 *  \n
 *  \brief  charger.h ---- CHARGING APPLICATION RELATED DECLARATION
 *  \details  This header file contains all of the definitions necessary for 
 *  other modules to interface with the battery charger utilities.
 *  \n &copy; Copyright 2007, 2008, 2009 QUALCOMM Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/chg_task/main/latest/app/inc/charger.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/09   hs      Added a new API that returns the status of the initialization
                   of the charger task.
09/07/08   vk      Added callback API chg_usb_suspended()
09/28/08   Vk      Fixed compiler warnings
06/25/08   hs      Added support for autonomous charging.
06/12/08   hs      Unit test support.
04/16/08   hs      Added charging UI interface.
06/11/07   hs      Created.
========================================================================== */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rex.h"               /* For REX related type definitions       */
#ifdef T_WINNT
#error code not present
#else
#include "pmapp_usb.h"         /* For pm_app_otg_a_dev_type type definition*/
#endif
/*===========================================================================

                              FORWARD DECLARATION

===========================================================================*/

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
/*! \enum chg_ui_event_type
 *  \brief This enum contains type definition for various charging UI events
 */
typedef enum
{
    CHG_UI_EVENT__IDLE,           /*!< Starting point, no charger.        */
    CHG_UI_EVENT__NO_POWER,       /*!< No/Weak Battery + Weak Charger.    */
    CHG_UI_EVENT__VERY_LOW_POWER, /*!< No/Weak Battery + Strong Charger.  */
    CHG_UI_EVENT__LOW_POWER,      /*!< Low Battery     + Strong Charger.  */
    CHG_UI_EVENT__NORMAL_POWER,   /*!< Enough Power for most applications.*/
    CHG_UI_EVENT__DONE,           /*!< Done charging, batt full.          */
    CHG_UI_EVENT__INVALID
#ifdef FEATURE_L4LINUX
#error code not present
#endif
}chg_ui_event_type;
/*! \enum chg_ui_charger_status_type
 *  \brief This enum contains defintions of the charger hardware related UI masks
 *  \details This enum contains definitions of the charger hardware related UI masks. 
 *  The Charging Application uses an unsigned byte to hold the charging UI status.
 *  \li The No.7 bit is not used. 
 *  \li The No. 6 and the No. 5 bits are used to hold the charger hardware status.
 *  \li The No. 4 bit is used to hold the charger hardware type.
 *  \li The No. 3 and the No. 2 bits are used to hold the battery status. 
 *  \li The No. 1 and the No. 0 bits are used to hold the battery voltage levels.
 */
typedef enum
{
    /*! The charger is removed( no charger is available) \b 00000000 . */
    CHARGER_IS_REMOVED = 0,  
    /*! The charger is bad \b 00100000  */
    CHARGER_IS_BAD     = 32, 
    /*! The charger is weak \b 01000000 */
    CHARGER_IS_WEAK    = 64, 
    /*! The charger is good \b 01100000 */
    CHARGER_IS_GOOD    = 96, 
    /*! Invalid charger status.         */
    CHARGER_STATUS_INVALID
}chg_ui_charger_status_type;
/*! \enum chg_ui_charger_hardware_type
 *  \brief This enum contains defintions of the charger hardware type related UI 
 *  masks
 *  \details This enum contains definitions of the charger hardware related type 
 *  UI maskes.  
 *  The Charging Application uses an unsigned byte to hold the charging UI status.
 *  \li The No.7 bit is not used. 
 *  \li The No. 6 and the No. 5 bits are used to hold the charger hardware status.
 *  \li The No. 4 bit is used to hold the charger hardware type.
 *  \li The No. 3 and the No. 2 bits are used to hold the battery status. 
 *  \li The No. 1 and the No. 0 bits are used to hold the battery voltage levels.
 */
typedef enum
{
    /*! The charger is a wall charger \b 00000000  */
    CHARGER_IS_WALL = 0,  
    /*! The charger is a USB charger \b 00010000   */
    CHARGER_IS_USB  = 16, 
    /*! Invalid charger hardware status.           */
    CHARGER_TYPE_INVALID
}chg_ui_charger_hardware_type;
/*! \enum chg_ui_battery_status_type
 *  \brief This enum contains defintions of the battery related UI masks
 *  \details This enum contains definitions of the battery related UI masks.
 *  The Charging Application uses an unsigned byte to hold the charging UI status.
 *  \li The No.7 bit is not used. 
 *  \li The No. 6 and the No. 5 bits are used to hold the charger hardware status.
 *  \li The No. 4 bit is used to hold the charger hardware type.
 *  \li The No. 3 and the No. 2 bits are used to hold the battery status. 
 *  \li The No. 1 and the No. 0 bits are used to hold the battery voltage levels.
 */
typedef enum
{
    /*! The battery is removed/bad \b 00000000  */
    BATTERY_IS_BAD     = 0,  
    /*! The battery is cold \b 00000100         */
    BATTERY_IS_COLD    = 4,  
    /*! The battery is hot \b 00001000          */
    BATTERY_IS_HOT     = 8,  
    /*! The battery is good \b 00001100         */
    BATTERY_IS_GOOD    = 12, 
    /*! Invalid battery hardware status.        */
    BATTERY_STATUS_INVALID
}chg_ui_battery_status_type;
/*! \enum chg_ui_battery_level_type
 *  \brief This enum contains defintions of the battery voltage level related UI 
 *  masks
 *  \details This enum contains definitions of the battery voltage level related UI 
 *  masks.
 *  The Charging Application uses an unsigned byte to hold the charging UI status.
 *  \li The No.7 bit is not used. 
 *  \li The No. 6 and the No. 5 bits are used to hold the charger hardware status.
 *  \li The No. 4 bit is used to hold the charger hardware type.
 *  \li The No. 3 and the No. 2 bits are used to hold the battery status. 
 *  \li The No. 1 and the No. 0 bits are used to hold the battery voltage levels.
 */
typedef enum
{
    /*! The battery voltage is very low (less tan 3.2V) \b 00000000          */
    BATTERY_IS_VERY_LOW   = 0, 
    /*! The battery voltage is low (between 3.2V and 3.4V) \b 00000001       */
    BATTERY_IS_LOW        = 1, 
    /*! The battery voltage is normal ( between 3.4V and 4.2V ) \b 00000010  */
    BATTERY_IS_NORMAL     = 2, 
    /*! The battery voltage is up to ful (close to 4.2V) \b 00000011         */
    BATTERY_IS_FULL       = 3, 
    /*! Invalid battery voltage level.                                       */
    BATTERY_LEVEL_INVALID
}chg_ui_battery_level_type;
/*! \enum chg_ui_status_mask_type
 *  \brief This struct contains the definitions of the charging UI status masks
 *  \details This struct contains the definitions of the charging UI status masks.
 *  The Charging Application uses an unsigned byte to hold the charging UI status.
 *  \li The No.7 bit is not used. 
 *  \li The No. 6 and the No. 5 bits are used to hold the charger hardware status.
 *  \li The No. 4 bit is used to hold the charger hardware type.
 *  \li The No. 3 and the No. 2 bits are used to hold the battery status. 
 *  \li The No. 1 and the No. 0 bits are used to hold the battery voltage levels.
 */
typedef enum
{
    /*! The UI mask for the battery voltage levels \b 00000011    */
    BATTERY_LEVEL_MASK  = 3,  
    /*! The UI mask for the battery status \b 00001100            */
    BATTERY_STATUS_MASK = 12, 
    /*! The UI mask for the charger hardware types \b 00010000    */
    CHARGER_TYPE_MASK   = 16, 
    /*! The UI mask for the charger hardware status \b 01100000   */
    CHARGER_STATUS_MASK = 96,
    /*! Invalid UI status mask.                                   */
    CHG_UI_STATUS_MASK_INVALID
}
chg_ui_status_mask_type;
/*! \enum chg_cmd_type
 *  \brief This enum contains generic type definition used to enable/disable
 *  charger functions.
 */
typedef enum
{
    CHG_CMD_DISABLE, /*!< Disable */
    CHG_CMD_ENABLE,  /*!< Enable  */
    CHG_CMD_INVALID  /*!< Invalid */
#ifdef FEATURE_L4LINUX
#error code not present
#endif
}chg_cmd_type;
/*! \enum chg_tx_wait_voter_type
 *  \brief This enum contains definitions of various applicatons that can
 *  vote to place the charging in/out TX WAIT state
 */
typedef enum
{
    CHG_TX_VOTE__RF  = (1 << 0),      /*!< voter = RF                   */       
    CHG_TX_VOTE__GPS = (1 << 1),      /*!< voter = GPS                  */
    CHG_TX_VOTE__VBATDET = (1 << 2),  /*!< voter = CHG_TX_VOTE__VBATDET */
    CHG_TX_VOTE__INVALID             /*!< Invalid voter                */
#ifdef FEATURE_L4LINUX
#error code not present
#endif
}chg_tx_wait_voter_type;
/*! \struct chg_tx_wait_vote_info_type
 *  \brief This struct contains the format definitions of TX WAIT voters
 */
typedef struct
{
    /*! TX_WAIT votor ID                              */
    chg_tx_wait_voter_type        voter_id;
    /*! The system TX on status                       */
    boolean                       tx_on;          
}chg_tx_wait_vote_info_type;

/*===========================================================================

                        VARIABLE DEFINITIONS 

===========================================================================*/
/*! \brief Used to tell applications outside PMIC about the charger task status 
 *  \details This variable will be uesd by other modulus outside PMIC to aquire
 *  charger task status.
 *  \li When it's TRUE, the charger task has been initialized.         
 *  \li When it's FALSE, this charger task has not been initialized.   
 */

extern boolean                        chg_is_task_initialized;
/*! \brief Used to hold the system TX information 
 *  \details This variable will be used to hold the system TX information, such as
 *  TX ON status, TX_WAIT voter ID. 
 */
extern chg_tx_wait_vote_info_type      chg_system_tx_informaton;
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*! \brief This function is used to put charging state in/out of Tx Wait state
 *  \details This function is used by different apps to vote, if charger task
 *  has been started, to put it in/out of TX WAIT state.  
 *  \param cmd The command that enables/disables TX WAIT state
 *  \param voter Applications that can vote to place the charger in/out TX WAIT 
 *  state
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_tx_wait_vote(chg_cmd_type    cmd, chg_tx_wait_voter_type  voter);

/*! \brief This function is used by other apps outside PMIC (e.g. FTM) to 
 *  start or stop charging activities
 *  \details This function is used by different apps to start or stop, if charger 
 *  task has been started, the charging activities.  
 *  \param cmd A command that starts or stops the charging activities
 *  \li CHG_CMD_ENABLE To turn on charging
 *  \li CHG_CMD_DISABLE To turn off charging
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_charging_ftm_switch(chg_cmd_type cmd);

/*! \brief This function is used by other apps outside PMIC (e.g. FTM) to 
 *  start or stop autonomous charging activities
 *  \details This function is used by different apps to start or stop, if charger 
 *  task has been started, the autonomous charging activities.  
 *  \param cmd A command that starts or stops the charging activities
 *  \li CHG_CMD_ENABLE To turn on charging
 *  \li CHG_CMD_DISABLE To turn off charging
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_auto_charging_switch(chg_cmd_type cmd);
/*! \brief This function enables/disables USB charging
 *  \param cmd The command that enables/disables USB charging
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_usb_charger_switch(chg_cmd_type cmd);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_usb_charger_switch */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function returns the charging status, including the battery 
 *  and the charger
 *  \param void
 *  \return An unsigned char that contains the charging status.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern uint8 chg_get_charging_status(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_get_charging_status */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function is a call back function registered with the HS USB
 *  driver code that is called by the SB Transceiver driver when a valid
 *  USB charger accessory is connected
 *  \param otg_dev USB charger accessory type
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_usb_charger_connected(pm_app_otg_a_dev_type otg_dev);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_usb_charger_connected */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function is a call back function registered with the HS USB
 *  driver code that is called by the SB Transceiver driver when a valid
 *  USB charger accessory is disconnected
 *  \param none
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_usb_charger_disconnected(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_usb_charger_connected */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function is a call back function registered with the USB 
 *  Transceiver driver that is called when we are allowed to draw current 
 *  from the USB charger accessory
 *  \param i_ma How much current we are allowed to draw from the USB charger 
 *  accessory
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_usb_i_is_available(uint32 i_ma);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_usb_charger_connected */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function is called by the USB Transceiver driver. It indicates
 *  that the USB devise can not supply current. Because it is not able 
 *  (ex; suspend mode) or is not attached
 *  \param none
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_usb_i_is_not_available(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_usb_charger_connected */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function is called by the USB Transceiver driver. It indicates
 *  that the USB device is in suspend mode and can not supply current. 
 *  \param usbSuspended Enable/disable USB suspend mode
 *  \return void.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern void chg_usb_suspended(boolean usbSuspended);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_usb_suspended */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function returns TRUE/FALSE depending if a valid charger is charging 
    the battery or not
 *  \param none
 *  \return a boolean which tells if the charger is charging the battery.
 *  \li TRUE The charger is charging the battery.
 *  \li FALSE The charger is not charging the battery.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern boolean chg_is_charging(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_is_charging */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function returns TRUE/FALSE depending if a valid charger accessory 
    connected or not
 *  \param none
 *  \return a boolean which tells if the charger is valid or not.
 *  \li TRUE Valid charger accessory attached.
 *  \li FALSE Invalid or no charger accessory connected.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern boolean chg_is_charger_valid(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_is_charger_valid */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function returns TRUE/FALSE depending if we have a valid battery 
 *  connected or not.
 *  \param none
 *  \return a boolean which tells if the battery is valid or not.
 *  \li TRUE The battery is valid.
 *  \li FALSE The battery is invalid.
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern boolean chg_is_battery_valid(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_is_battery_valid */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief returns the last charger UI event. The charger UI events indicate
 *  the charging status.
 *  \param none
 *  \return an enum which tells the charging status.
 *  \li CHG_UI_EVENT__IDLE Starting point, no charger.
 *  \li CHG_UI_EVENT__NO_POWER No/Weak Battery + Weak Charger.
 *  \li CHG_UI_EVENT__VERY_LOW_POWER No/Weak Battery + Strong Charger.
 *  \li CHG_UI_EVENT__LOW_POWER Low Battery + Strong Charger.
 *  \li CHG_UI_EVENT__NORMAL_POWER Enough Power for most applications.
 *  \li CHG_UI_EVENT__DONE Done charging, batt full. 
 *  \note This function should only be called after the charger task has been
 *  initialized.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 */ 
extern chg_ui_event_type chg_ui_event_read(void);
//charger metacomments required for remoting charger API
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_ui_event_read */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*! \brief This function returns the status of the initialization of the charger task.
 *  \param none
 *  \return a boolean which tells if the battery is valid or not.
 *  \li TRUE The charger task has been initialized.
 *  \li FALSE The charger task has not been initialized yet.
 *  \note This API should be used by other subsystems who want to make sure that they get
 *  the correct charging related status by calling this API first.
 *
 *  <b>Dependencies</b>
 *  \li chg_task_init()
 *  \li chg_irq_init()
 */ 
extern boolean chg_is_application_initialized(void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION chg_is_application_initialized */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#endif/* CHARGER_H */

