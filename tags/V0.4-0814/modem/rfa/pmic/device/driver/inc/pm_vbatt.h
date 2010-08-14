
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    VBATT NEW INTERNAL HEADER FILE

GENERAL DESCRIPTION
  This file contains internal vbatt functions to support vbatt interface 
  using PMIC comparator with the Qualcomm Power Management ICs.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm_vbatt.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/26/08   APU     Added the pm_disable_comparator_alarm and
                   pm_enable_comparator_alarm () commands.        			
07/08/08   APU     Tested with 6260, 7201 and added comments
01/02/08   APU     Created.
===========================================================================*/
#include "comdef.h"     /* Definitions for byte, word, etc. */

#define NUMBER_OF_ITERATIONS_DURING_INIT        15

#define PM_VBATT_COMPARATOR_MAX                 4300
#define PM_VBATT_COMPARATOR_MIN                 2800
#define PM_VBATT_RANGE                  (PM_VBATT_COMPARATOR_MAX-PM_VBATT_COMPARATOR_MIN)

typedef uint16 pm_battery_voltage_type ; 

typedef enum
{
    VBATT_INIT_PASSED = 0 ,
    VBATT_INIT_FAILED 
} vbatt_init_err_type ;       

typedef enum
{
    PMIC_COMPARATOR_VOLTAGE_UP = 0 ,
    PMIC_COMPARATOR_VOLTAGE_DOWN ,
    PMIC_COMPARATOR_VOLTAGE_UNCHANGED 
} pm_comp_window_movement_type ;



/*===========================================================================

FUNCTION pm_did_vbatt_change                            EXTERNAL FUNCTION

DESCRIPTION
  The interrupts informs the driver of any voltage changes. The vbatt task on 
  the other hand services the registered-clients. This function keeps track if 
  "all the clients" have yet been serviced or not. It returns TRUE if the 
  battery voltage has changed and not all clients have registered it.

EXAMPLE:                                               
  if ( pm_did_vbatt_change () )
  { printf ( "Clients have not yet been completely serviced after vbatt a 
  voltage change. " ) ; } 

INPUT PARAMETERS
  NONE.

RETURN VALUE
  Return value type: boolean
  TRUE: vbatt voltage has changed and some or all clients are in the pending 
  list to be serviced.
  FALSE:  After vbatt voltage changed, all clients' were serviced and their 
  callbacks have been called.

  DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  3) vbatt_init()
  4) vbatt_task_init() 
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
boolean
pm_did_vbatt_change ( void ) ;




/*===========================================================================

FUNCTION pm_vbatt_indicate_task_start                       EXTERNAL FUNCTION

DESCRIPTION
  This function tells the vbatt house-keeping structure that vbatt_task has 
  started and clients can be now serviced.

EXAMPLE:                                               
  if ( pm_vbatt_indicate_task_start () )
  { printf ( "VBATT task has successfully been started by you. " ) ; } 

INPUT PARAMETERS
  NONE.

RETURN VALUE
  Return value type: boolean
  TRUE: vbatt_task has started.
  FALSE: Staring of vbatt_task has failed for some reason.

  DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
boolean 
pm_vbatt_indicate_task_start ( void ) ;



/*===========================================================================

FUNCTION vbatt_init                                         EXTERNAL FUNCTION

DESCRIPTION
  This is the main initialization function for vbatt driver. It accomplishes 
  the following:
  1. Init vbatt data structres.
  2. Sets up PMIC VBATT Comparator.
  3. Sets up the pre-vbatt_task ISR.

NOTE
  1. Make sure that the return value is checked for success.      
  2. Make sure it is only called once, preferably before task initialization.
  EXAMPLE:                                               
  if ( vbatt_init () )
  { printf ( "VBATT Driver init failed ... " ) ; } 

INPUT PARAMETERS
  NONE.

RETURN VALUE
  Return value type: vbatt_init_err_type 
  VBATT_INIT_PASSED : Driver inititialization is successfull.       
  VBATT_INIT_FAILED FALSE: Driver Init failed. vbatt unusable.

  DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
vbatt_init_err_type 
vbatt_init ( void ) ;




/*===========================================================================

FUNCTION pm_set_comparator_window EXTERNAL FUNCTION

DESCRIPTION
  pm_set_comparator_window programs PM_BAT_ALRM_THRESH_R_W on the basis of
  pm_calculate_next_window.  

EXAMPLE:                                               
    pm_vbatt_comp_window_change_detector () ;
    pm_set_comparator_window () ;

INPUT PARAMETERS
  NONE.

RETURN VALUE
  NONE.

DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  3) vbatt_init()
  4) vbatt_task_init
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
void 
pm_set_comparator_window ( void ) ;



/*===========================================================================

FUNCTION pm_vbatt_comp_window_change_detector               EXTERNAL FUNCTION

DESCRIPTION
  pm_vbatt_comp_window_change_detector performs 2 functions:
  1. Checks for direction of voltage change.
  2. Modifies the appropriate data structure to inform that the 
  battery-voltage has changed.

EXAMPLE:                                               
    pm_vbatt_comp_window_change_detector () ;
    (void) pm_set_comparator_window () ;

INPUT PARAMETERS
  NONE.

RETURN VALUE
  NONE.

DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  3) vbatt_init()
  4) vbatt_task_init
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
void 
pm_vbatt_comp_window_change_detector ( void ) ;



/*===========================================================================

FUNCTION pm_vbatt_voltage_change_registered         EXTERNAL FUNCTION

DESCRIPTION
  Once the voltage is changed, the vbatt_task has the responsibilty of 
  servicing the clients. Before that has been done 
  pm_vbatt_status_voltage.pm_did_batt_voltage_change is set. After the 
  servicing is complete, this flag is reset. 
  This function resets this flag. The caller is vbatt_task, since it services 
  all clients and then calls this function.

EXAMPLE:                                               
  if ( pm_vbatt_voltage_change_registered () )
  { printf ( "VBATT task has serviced all cleints... " ) ; } 

INPUT PARAMETERS
  NONE.

RETURN VALUE
  Return value type: boolean
  TRUE: Servicing pending .
  FALSE: Servicing done.

  DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  3) vbatt_init()
  4) vbatt_task_init()
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
boolean 
pm_vbatt_voltage_change_registered ( void ) ;





/*===========================================================================
FUNCTION pm_comparator_vbatt_read                           EXTERNAL FUNCTION

DESCRIPTION
  This function returns the voltage incident at battery terminals in mV.
         
  EXAMPLE:
  pm_battery_voltage_type batt_voltage = pm_comparator_vbatt_read () ;                           
    
INPUT PARAMETERS
  NONE.

RETURN VALUE
  Return value type: pm_battery_voltage_type 
  Battery voltage between 2800 and 4300 in milli-volts.

DEPENDENCIES
  The following functions must have been called:
  1) rflib_init()
  2) pm_init()
  3) vbatt_init()
  4) vbatt_task_init() 
  
SIDE EFFECTS
  NONE. 
===========================================================================*/
pm_battery_voltage_type 
pm_comparator_vbatt_read ( void ) ;




/*===========================================================================

FUNCTION pm_disable_comparator_alarm                           EXTERNAL FUNCTION

DESCRIPTION
 The pm_disable_comparator_alarm clears the interrupt and also disables the 
 disables the Battery Alarm.

EXAMPLE:
 if ( ! pm_disable_comparator_alarm() )
 {
    printf ( "Battery Alarm Disabled " ) ;	
 }

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 uint8
 0   : Successful
 > 0 : Error Condition. 
 
DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()

SIDE EFFECTS
 NONE  
===========================================================================*/

uint8
pm_disable_comparator_alarm ( void ) ;



/*===========================================================================

FUNCTION pm_enable_comparator_alarm                           EXTERNAL FUNCTION

DESCRIPTION
 The pm_enable_comparator_alarm is used enable the Battery Alarm.

EXAMPLE:
 if ( ! pm_enable_comparator_alarm() )
 {
    printf ( "Battery Alarm Enabled " ) ;	
 }

INPUT PARAMETERS
 NONE.
 
RETURN VALUE
 uint8
 0   : Successful
 > 0 : Error Condition. 
 
DEPENDENCIES
 The following functions must have been called:
 1) pm_init()
 2) vbatt_init()

SIDE EFFECTS
 NONE  
===========================================================================*/
uint8 
pm_enable_comparator_alarm ( void ) ; 


