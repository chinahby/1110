/*=============================================================================

               O E M  S e c o n d a r y  B o o t  L o a d e r
                       USB Battery Charging Handler 

GENERAL DESCRIPTION
    This file contains the functions that will handle the scenario when the
phone boots from USB port and finds that battery is too low to assist the
boot process. The phone will attempt to charge the battery first before
continuing to boot.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
=============================================================================*/


/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/romboot/osbl/osbl_usb_auto_chg_init.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
14/02/09   tnk     Added support for  USB disconnect on a non-enumerated case 
12/15/08   HS      Added support for NiMH battery charging.
12/10/08   HS      Added autonomous charging configurations.
10/31/08   APU     Basic Charging is working. Will verify the user cases now.
10/29/08   APU     Created.
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "pm.h"
#include "pmsbl.h"
#include "osbl_usb_chg_init.h"
#include "qhsusb_al_chg_api.h"

/* In quanta of OSBL_POLL_DELAY  totally 20 mins taking suspend case into account. Normal charging time is <5mins*/
#define TIME_OUT_PERIOD                           400000
/*! \def OSBL_TCON_ISUSP
 *  \brief The TCON_ISUSP After a Portable Device with a Dead Battery, Weak Battery or no battery connects, 
 *  it is allowed to draw up to IUNIT for TCON_ISUSP.  After TCON_ISUSP, the device is required to comply 
 *  with the USB 2.0 specification rules for going into the suspended state.
 *  \note  This value is in quanta of OSBL_POLL_DELAY 10 secs
 */
#define OSBL_TCON_ISUSP                           3333
/*! \def OSBL_IUNIT
 *  \brief The IUNIT After a Portable Device with a Dead Battery, Weak Battery or no battery connects, 
 *  it is allowed to draw up to IUNIT for TCON_ISUSP.  After TCON_ISUSP, the device is required to comply 
 *  with the USB 2.0 specification rules for going into the suspended state.
 *  \note THis setting should only be used before the enumeration complete
 */
#define OSBL_IUNIT                                100
/*! \def OSBL_POLL_DELAY
 *  \brief The delay used to poll the USB stack, in ms.
 */
#define OSBL_POLL_DELAY                           3

/*! \def OBSL_CHG_VBATDET
 *  \brief The VBATDET setting for USB dead battery charging using autonomous charger
 */
#define OBSL_AUTO_CHG_VBATDET                     4100
/*! \def OBSL_AUTO_CHG_VMAXSEL
 *  \brief The VMAXSEL setting for USB dead battery charging using autonomous charger
 */
#define OBSL_AUTO_CHG_VMAXSEL                     4200
/*! \def OBSL_AUTO_CHG_IMAXSEL
 *  \brief The IMAXSEL setting for USB dead battery charging using autonomous charger
 */
#ifdef FEATURE_BATTERY_CHARGER_USE_NIMH
#define OBSL_AUTO_CHG_IMAXSEL                     300
#else
#define OBSL_AUTO_CHG_IMAXSEL                     450
#endif /* FEATURE_BATTERY_CHARGER_USE_NIMH */
/*! \def OBSL_AUTO_CHG_IMAXSEL_MIN
 *  \brief The IMAXSEL setting for USB dead battery charging using autonomous charger
 *  \note THis setting should only be used before the enumeration or enumeration failed
 */
#define OBSL_AUTO_CHG_IMAXSEL_MIN                 100
/*! \def OBSL_AUTO_CHG_TRICKLE_HIGH_CURRENT
 *  \brief The trickle charger current setting (100mA)for USB dead battery charging  
 *  using autonomous charger when the USB enumeration has done successfully
 *  \note This setting should only be used when the USB device has been enumerated 
 *  successfully.
 */
#define OBSL_AUTO_CHG_TRICKLE_HIGH_CURRENT        100
/*! \def OBSL_AUTO_CHG_TRICKLE_LOW_CURRENT
 *  \brief The trickle charger current setting (40mA) for USB dead battery charging 
 *  using autonomous charger when the USB enumeration has failed
 *  \note This setting should only be used when the USB device has not been enumerated 
 *  successfully.
 */
#define OBSL_AUTO_CHG_TRICKLE_LOW_CURRENT         40
/*! \def OSBL_AUTO_CHG_ITERM
 *  \brief The end-of-charging current using autonomous charger (10mA)
 */
#define OSBL_AUTO_CHG_ITERM                       10
/*! \def OSBL_AUTO_CHG_ITERM
 *  \brief The maximum automatic fast charging time ( 2hours)
 */
#define OSBL_AUTO_CHG_T                           120
/*=============================================================================
 ENUM: osbl_usb_stimuli_type

 DESCRIPTION: This enum contains responses based on tbe USB state which 
 dictates the behaviour of the FSM. On the basis of one of these elements
 the FSM decides what to do next.
=============================================================================*/
typedef enum
{
    /* Initial state */
    INIT_STATE ,

    /* USB enumerated, tell FSB to start charging */
    READY_FOR_AUTO_CHG ,

    /* Ready for trickle charging atleast, fast charging only after 
       enumeration */
    READY_FOR_TRICKLE_CHARGING ,

    /* Execute main loop atleast once */
    RETRY_ONCE ,

    /* Execute the main loop n number of times */
    RETRY_MULTIPLE_TIMES ,

    /* Perhaps a wall charger was detected */
    READY_TO_BOOT ,

    /* Perhaps charging has started and USB is still connected */
    MAINTAIN_PREV_STATE_N_WAIT ,

    /* Unexpected stimulus */
    UNKNOWN_STATE ,

    /* Bad or no USB connection */
    FAILURE_DO_POWER_DOWN
} osbl_usb_stimuli_type ;



/*! \enum chg_ui_event_type
 *  \brief This enum contains type definition for various charging UI events
 */
typedef enum
{
    OSBL_PM_CHG_UI_EVENT__IDLE,           /*!< Starting point, no charger.        */
    OSBL_PM_CHG_UI_EVENT__NO_POWER,       /*!< No/Weak Battery + Weak Charger.    */
    OSBL_PM_CHG_UI_EVENT__VERY_LOW_POWER, /*!< No/Weak Battery + Strong Charger.  */
    OSBL_PM_CHG_UI_EVENT__LOW_POWER,      /*!< Low Battery     + Strong Charger.  */
    OSBL_PM_CHG_UI_EVENT__NORMAL_POWER,   /*!< Enough Power for most applications.*/
    OSBL_PM_CHG_UI_EVENT__DONE,           /*!< Done charging, batt full.          */
    OSBL_PM_CHG_UI_EVENT__INVALID,
#ifdef FEATURE_L4LINUX
#error code not present
#endif
}osbl_pm_chg_ui_event_type;
/*
    ========================================
    |STATE           |Corresponding Voltage|
    ========================================
    |NO_POWER:       |  no battery         |
    |VERY_LOW_POWER: |  < 3.2V             |
    |LOW_POWER:      |  > 3.2 but < 3.4    |
    |NORMAL_POWER    |  >= 3.4             |
   =========================================
*/

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



/*! \enum chg_auto_charging_state_name_type
 *  \brief This enum contains names for each charging state in the automatic 
 *  charging scheme
 */
typedef enum 
{
    /*! Charger state machine entry point.                            */
    OSBL_PM_CHG_AUTO__INIT_ST,     
    /*! Power on from the charger state.                              */
    OSBL_PM_CHG_AUTO__PWR_ON_CHGR_ST,
    /*! Auto rickle charging state.                                   */
    OSBL_PM_CHG_AUTO__AUTO_TRICKLE_ST,  
    /*!Power on from the battery state.                               */
    OSBL_PM_CHG_AUTO__PWR_ON_BATT_ST,
    /*! Auto trickle charging has failed state.                       */
    OSBL_PM_CHG_AUTO__AUTO_TRICKLE_FAILED_ST,
    /*! Transition state, gives time to analog circuitry to settle.   */
    OSBL_PM_CHG_AUTO__TRANS_ST,
    /*! Power on from battery and charger after auto trickle charing. */                          
    OSBL_PM_CHG_AUTO__POST_AUTO_TRICKLE_ST,
    /*! Fast charging state.                                          */
    OSBL_PM_CHG_AUTO__FAST_ST,   
    /*! Trickle charging state.                                       */
    OSBL_PM_CHG_AUTO__TRICKLE_ST,
    /*! Charging failed state.                                        */
    OSBL_PM_CHG_AUTO__FAILED_ST,
    /*! End-of-charging state -- charging succeed.                    */
    OSBL_PM_CHG_AUTO__DONE_ST,
    /*! Temperary state: Low current liming charging                  */
    OSBL_PM_CHG_AUTO__LOW_ILIM_ST, 
    /*! USB suspend mode.                                             */
    OSBL_PM_CHG_AUTO__USB_SUSP_ST,
    /*! Invalid state.                                                */
    OSBL_PM_CHG_AUTO__INVALID_ST           
} osbl_pm_chg_auto_charging_state_name_type ;
/*! \enum chg_auto_charging_charger_type
 *  \brief This enum contains charger hardware type definition used in automatic charging
 */


typedef enum
{
    BAD_BATTERY = 0   ,
    DEAD_BATTERY      ,
    WEAK_BATTERY      ,
    NORMAL_BATTERY    ,
    UNKNOWN_BATTERY  
} osbl_batt_types_for_boot ;

/*=============================================================================
 STRUCT: osbl_pm_chg_batt_fsm_var_type

 DESCRIPTION: This structure contains battery states based on tbe HW CHG state 
 machine dictates the behaviour of the  SW-FSM. On the basis of one of these 
 elements the FSM decides what to do next.
=============================================================================*/

typedef struct
{
    osbl_pm_chg_ui_event_type osbl_pm_batt_status ;
    boolean  osbl_pm_is_charging ;
    boolean  osbl_pm_is_batt_valid ;
} osbl_pm_chg_batt_fsm_var_type ;



/*===========================================================================

FUNCTION osbl_shp_autocharging_setup

DESCRIPTION
  Sets the PIMC in Auto charging mode.

DEPENDENCIES
  Should only be called from and within boot-code.
  
PARAMETER
  Trickle charger current

RETURN VALUE
  Error flag

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
===========================================================================*/
pm_err_flag_type
osbl_shp_autocharging_setup ( uint32 imaxsel, uint32 trickle_current_setting ) ;

/*===========================================================================

FUNCTION osbl_pm_charging_FSM

DESCRIPTION
 This is a SW FSM that works on USB status and the battery status. This 
 function only returns when we have battery is charged to have enough 
 juice or a wall charger connected to it or the phone powers down or 
 timeout happens.

DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
===========================================================================*/
void 
osbl_pm_charging_FSM ( void ) ;



/*=============================================================================

FUNCTION osbl_pm_auto_low_batt_chg

DESCRIPTION
  Core algorithm for autonomous charging. This function implements the core
  charging algorithm after configuring the PMIC. If wall-charger is detected 
  the phone boots else if PC USB is detected does the dead/fast USB charging
  depending upon the battery voltage.


DEPENDENCIES
  Should only be called from and within boot-code.
  
RETURN VALUE
  VOID

SIDE EFFECTS
  Runs in single threaded fashion and have clk-wait delay. May slow down the
  BOOT process. 
=============================================================================*/
void
osbl_pm_auto_low_batt_chg( void ) ;

