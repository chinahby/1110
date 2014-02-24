/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         USAGE EXAMPLES FOR THE VBATT NEW INTERFACE

GENERAL DESCRIPTION
  This file shows as to how to use the new vbatt-driver's interface which in  
  turn uses Qualcomm Power Management ICs. This file should not be compiled as 
  it uses both RAM and ROM.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/vbatt/main/latest/app/inc/vbatt_sample_interface.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/08   APU     Tested with 6260, 7201 and added comments
01/02/08   APU     Created.
===========================================================================*/

#include "vbatt_pmic_comparator.h"
#include "pm.h"

extern uint8 pmdebvar ;  
pm_batt_index test_handle1 ; 
int ivar ; 

void pm_sample_vbatt_cb ( void * ) ;

void pm_sample_vbatt_cb1 ( void* ) ;


/*=======================================================================================
Function: 
    pm_sample_use_new_vbatt_interface 

DESCRIPTION:
    This function shows the use of vbatt interface which uses a PMIC comparator. The 
    function has all the important function-calls demonstrating the usage of each of them.

PARAMETERS:
    NONE

RETURN: 
    0 for success.
    non-zero for failure.

DEPENDENCIES:
    1> vbatt_task should have started for servicing the clients.
=========================================================================================*/
vbatt_error_type 
pm_sample_use_new_vbatt_interface ( void )
{
    /* A Client is interested in 3.3 when the voltage goes up */
    vbatt_error_type err ;
    pm_vbatt_void_ptr func_void_ptr_arg ;

    /* Please let mw know, if the voltage is affected and is above the desired voltage of 3300mV */
    pm_vbatt_voltage_tracking_type  voltage_direction = VBATT_VOLTAGE_ABOVE_THIS_LEVEL ;
    pm_battery_voltage_type desired_volt = 3300 ; /* In mV */

    pm_vbatt_cb_type cb_func = &pm_sample_vbatt_cb ;
    pmdebvar = 19 ;

    /* test_handle1 and ivar are global variables and dont loose 
       their address and value  between function calls */
    ivar = 56 ;
    func_void_ptr_arg = (void *) &ivar ;

    test_handle1 = 
    pm_vbatt_register ( 
                        desired_volt         , 
                        voltage_direction    , 
                        cb_func              ,
                        func_void_ptr_arg    ,
                        &err              
                      ) ; 

    if ( err != VBATT_REGISTRATION_SUCCESSFUL )
    {
        /* PRINT Registration has failed */

    }
    else /* We can only use the other functions if the registration
            was successful and we obtained ab handle*/
    {
        /* First time registration, use the handle subsequently. 
           Client now wants to use different votage direction, voltage and  
           and different callback. One can use of the same handle.
       */
        desired_volt = 3300 ;
        voltage_direction = VBATT_VOLTAGE_BELOW_THIS_LEVEL ;    
        cb_func = &pm_sample_vbatt_cb1 ;
        err = pm_vbatt_modify_client ( 
                                    test_handle1 ,
                                    desired_volt , 
                                    VBATT_VOLTAGE_UP , 
                                    cb_func ,
                                    func_void_ptr_arg
                                  ) ;
        if ( err != VBATT_MODIFICATION_SUCCESSFUL )
        {
            /* PRINT MOdification has failed */
        }


        /* Access the information of the client in read only context */
        /* All the variables except handle will be overwritten by the pmic-vbatt driver. */
        err = pm_vbatt_get_client_info ( 
                                        test_handle1 ,
                                        &desired_volt , 
                                        &voltage_direction , 
                                        cb_func ,
                                        func_void_ptr_arg
                                     ) ; 
        if ( err != VBATT_INTERROGATION_SUCCESSFUL ) 
        {
            /* Print that the voltage client is invalid */
        }

        /* Delete a client if no longer required */
        err = pm_vbatt_deregister_client (test_handle1) ;
        if ( err != VBATT_DEREGISTRATION_SUCCESSFUL ) 
        {
            /* Print a message that the deletion has failed */
        }
    }
    return err ;
}


void pm_sample_vbatt_cb ( void *ptr )
{ 
    pm_battery_voltage_type vbatt_voltage =  
    pm_comparator_vbatt_read ( ) ;

    /*
        if ( vbatt_voltage is something )
        {
            Do something here 


        }

    */

}

void pm_sample_vbatt_cb1 ( void *ptr ) 
{ 
}
