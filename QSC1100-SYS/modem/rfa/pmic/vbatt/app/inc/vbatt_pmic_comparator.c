/*! \file 
 *  \n
 *  \brief  vbatt_pmic_comparator.c ---- Implements the new VBATT INTERFACE APIS
 *  \n
 *  \n This C file contains vbatt functions to support new vbatt interface
 *  with the Qualcomm Power Management ICs.
 *  \n
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */
/* ======================================================================= */


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/vbatt/main/latest/app/inc/vbatt_pmic_comparator.c#8 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/09   APU     Added APIs to support the new VBATT info structure and 
                   also support the CHG callback functionality.
01/26/09   APU     Added support for CHG events to the code.
01/21/09   APU     Fixed direction error for CBs. 
10/22/08   APU     Fixed LINT warnings. 
09/09/08   APU     Added Doxygen Comments to the code. 
09/18/08   APU     Fixed a minor bug related to the voltage direction.
08/30/08   APU     Added pm_vbatt_mimic_legacy_vbatt_read() to support
                   vbatt_read.  
07/26/08   APU     Fixed a bug related with the client-array checking.
07/08/08   APU     Tested with 6260, 7201 and added comments
01/02/08   APU     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "msg.h"                  /* for messages */
#include "vbatt_pmic_comparator.h"

/*===========================================================================

                   GLOBAL VARIABLE DEFINITIONS

===========================================================================*/
/*===========================================================================

                     LOCAL TYPE DEFINITIONS

===========================================================================*/

/*! 
 \struct pm_array_of_vbatt_registrars  
 \brief Local array of type pm_vbatt_client_data_template for keeping client 
 information.
 \details This elements of this array keep the client information. One client
  uses one array element. MAX_CLIENTS_ALLOWED cane be increased or decreased 
  as per usage.
*/
static pm_vbatt_client_data_template 
pm_array_of_vbatt_registrars[MAX_CLIENTS_ALLOWED] ;   


/* 
 \var pm_vbatt_enable_disable_client_servicing
 \brief local boolean variable for enabling or disabling the client. 
 \details This variable can be used to disable or enable, the vbatt_task from servicing
 the vbatt clients. This can be helpful during debugging. Even if the 
 client-serving is disabled the vbatt voltage can still be read using 
 pm_comparator_vbatt_read(). The client-servicing is ON by default.  
*/
static boolean pm_vbatt_enable_disable_client_servicing ;
/* ============================================================================== */




/*===========================================================================

                   GLOBAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_vbatt_is_servicing_tables_enabled EXTERNAL FUNCTION

DESCRIPTION
    The driver can stop servicing the vbatt-clients. This can be used for debugging
    pm_vbatt_enable_disable_client_servicing is the variable that can turn or turn the
    client-servicing. This function is used to query if servicing is ON or OFF.
EXAMPLE:
    if ( pm_vbatt_is_servicing_tables_enabled () )
    { printf "Clients are being serviced." } ;
    else
    { printf "Clients are not being serviced." } ;

INPUT PARAMETERS
NONE

RETURN VALUE
 Return value type: boolean          
 TRUE : Servicing Enabled
 FALSE : Servicing Disabled 

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()

SIDE EFFECTS
  NONE
===========================================================================*/

boolean pm_vbatt_is_servicing_tables_enabled ( void )
{
    return pm_vbatt_enable_disable_client_servicing ;
}




/*===========================================================================

FUNCTION pm_vbatt_enable_table_servicing EXTERNAL FUNCTION

DESCRIPTION
    The driver can stop servicing the vbatt-clients. This can be used for debugging
    pm_vbatt_enable_disable_client_servicing is the variable that can turn or turn the
    client-servicing. This function is used to turn the table-servicing ON.
EXAMPLE:
    if ( pm_vbatt_is_servicing_tables_enabled () )
    { printf "Clients servicing switched ON." } ;

INPUT PARAMETERS
NONE

RETURN VALUE
 Return value type: boolean          
 TRUE : Servicing Enabled
 FALSE : Servicing Disabled 

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()

SIDE EFFECTS
  NONE
===========================================================================*/
boolean pm_vbatt_enable_table_servicing ( void )
{
    pm_vbatt_enable_disable_client_servicing = TRUE ;
    return pm_vbatt_enable_disable_client_servicing ;
}





/*===========================================================================

FUNCTION pm_vbatt_disable_table_servicing EXTERNAL FUNCTION

DESCRIPTION
    The driver can stop servicing the vbatt-clients. This can be used for debugging
    pm_vbatt_enable_disable_client_servicing is the variable that can turn or turn the
    client-servicing. This function is used to turn the table-servicing OFF.
EXAMPLE:
    if ( pm_vbatt_disable_table_servicing () )
    { printf "Clients servicing switched OFF." } ;

INPUT PARAMETERS
NONE

RETURN VALUE
 Return value type: boolean          
 TRUE : Servicing Enabled
 FALSE : Servicing Disabled 

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()

SIDE EFFECTS
  NONE
===========================================================================*/
boolean pm_vbatt_disable_table_servicing ( void )
{
    pm_vbatt_enable_disable_client_servicing = FALSE ;
    return pm_vbatt_enable_disable_client_servicing ;
}




/*===========================================================================

FUNCTION pm_vbatt_register                          EXTERNAL FUNCTION

DESCRIPTION
    This function is used by the client to register itself. When all the cconditions
    are met, the specified CB is called.

EXAMPLE:
    global uint8 test_handle1 = 
    pm_vbatt_register ( 
                        3500                 ,  Voltage in mV, between 2800 & 4300 
                        voltage_direction    ,  UP or DOWN
                        cb_func              ,  CB that will be called.
                        func_void_ptr_arg    ,  void pointer argument of the CB function.
                        &err                    Check for non-zero values for error-conditions.
                      ) ; 
    Translating to: Register me as vbatt-client and call my CB when the voltage 
    crosses 3500mV mark, in the direction specified. Use the void pointer arg
    supplied. err will let me know if the registration-attempt was successful. 
 
INPUT PARAMETERS
#   Parameter type:        &         Parameter name:       &    Range: 
1.  pm_battery_voltage_type          vbatt_voltage ,            2800-4200 
2.  pm_vbatt_voltage_tracking_type   voltage_direction ,        VBATT_VOLTAGE_ABOVE_THIS_LEVEL 
                                                                VBATT_VOLTAGE_BELOW_THIS_LEVEL are preferred.
3.  pm_vbatt_cb_type                 cb_funcpointer ,           Non-Null
4.  pm_vbatt_void_ptr                voidptr ,                  Optional
5.  vbatt_error_type                 *err                       Consult error types.
     
RETURN VALUE
 Return value type: pm_batt_index .
 pm_batt_index is a registration handle and an index to uniquely specify a vbatt client 
 out of many. While de-registering, a unique handle is required for a successful 
 deregistration-attempt. This handle should be saved between calls.  
 The handle can be between 0 and (MAX_CLIENTS_ALLOWED - 1)  

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()
  
SIDE EFFECTS
  Interrupts are LOCKED while checking for free entries in the array to provide 
  immunity from task switching.

===========================================================================*/

pm_batt_index 
pm_vbatt_register ( 
                        pm_battery_voltage_type vbatt_voltage , 
                        pm_vbatt_voltage_tracking_type voltage_direction , 
                        pm_vbatt_cb_type cb_funcpointer ,
                        pm_vbatt_void_ptr voidptr ,
                        vbatt_error_type *err 
                  ) 
{

    pm_batt_index client_counter = 0 ;
    /* Check for args first and get out if they dont conform */
    if (     ( vbatt_voltage <= PM_VBATT_COMPARATOR_MAX ) &&
             ( vbatt_voltage >= PM_VBATT_COMPARATOR_MIN ) &&
             ( voltage_direction <= VBATT_VOLTAGE_LEVEL ) &&
             ( cb_funcpointer != NULL ) )
    {

        boolean free_entry_found = FALSE ;
        *err = VBATT_REGISTRATION_SUCCESSFUL ;

        /* Since registration happens in task context and if 2 clients in 2 different tasks 
          are trying to register, we ought to avoid that. */
        INTLOCK();
        while ( ( client_counter < MAX_CLIENTS_ALLOWED ) && ( !free_entry_found ) ) 
        {
            /* Since this is always checked in interrupt-locked or atomic context, 
               no task can hijack this check and will always happen without 
               'pm_is_table_in_use' getting corrupted  */
            if ( pm_array_of_vbatt_registrars[client_counter].pm_is_table_in_use == FALSE )
            { 
                free_entry_found = TRUE ; 
                pm_array_of_vbatt_registrars[client_counter].pm_is_table_in_use = TRUE ;
                /* Free entry is found successfully, mark the element is use */
            }
            else
            {
                client_counter++ ;
            }  
        }
        /* If free entry was found, the client_counter will have the index at this stage */
        INTFREE();
 
        /* Could not find a single entry, Registration failed....  */
        if ( ! free_entry_found )
        {
            *err = VBATT_CLIENT_TABLE_FULL ;
            /* Error Fatal , Print a message debug here */
        }/* Register the client into the database, client_counter is the index */
        else if ( client_counter < MAX_CLIENTS_ALLOWED ) /* To satisfy Lint */
        {
            /* Copy the rest of the elements */
            pm_array_of_vbatt_registrars[client_counter].desired_vbatt_voltage = vbatt_voltage ;
            pm_array_of_vbatt_registrars[client_counter].pm_voltage_direction = 
            voltage_direction ;
            pm_array_of_vbatt_registrars[client_counter].pm_vb_cb_function = cb_funcpointer ;
            pm_array_of_vbatt_registrars[client_counter].void_ptr_of_cb_func = voidptr ;
            pm_array_of_vbatt_registrars[client_counter].client_handle = client_counter ;
        }
    }
    else
    {
        /*MSG ERR, Wrong or Invalid args */
        *err = (vbatt_error_type)VBATT_REG_PARAMS_WRONG ;         
    }

    /* This becomes the client-handle. This handle can be now used with 
       pm_vbatt_modify_client () to modify client conditions */
    return client_counter ;
}


/*===========================================================================

FUNCTION pm_vbatt_deregister_client                          EXTERNAL FUNCTION

DESCRIPTION
    This function is used by the client to de-register itself. The client may no-longer 
    need the services of the vbatt-driver. One such case can be the Blue-Tooth-client (BT).
    BT may not need to monitor the battery voltage if BT is turned off. This frees up 
    resources for other clients. 

EXAMPLE:
    err = pm_vbatt_deregister_client (test_handle1) ;

INPUT PARAMETERS
-   Parameter type:        &         Parameter name:       &    Range: 
1.  uint8                            test_handle1               0 - (MAX_CLIENTS_ALLOWED - 1)

RETURN VALUE
 Return value type: vbatt_error_type 
 0 : Call Successfull.
 Non-0 : Call failed, see error codes
 pm_batt_index is a registration handle and an index to uniquely specify a vbatt client 
 out of many. While de-registering, a unique handle is required for a successful 
 deregistration-attempt. This handle should be saved between calls.  
 The handle can be between 0 and (MAX_CLIENTS_ALLOWED - 1)   

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()
  4) pm_vbatt_register() 

SIDE EFFECTS
  Interrupts are LOCKED while freeing the client in the array to provide 
  immunity from task switching.

===========================================================================*/
vbatt_error_type 
pm_vbatt_deregister_client ( pm_batt_index handle )
{
    vbatt_error_type err = VBATT_DEREGISTRATION_SUCCESSFUL ;
    if ( handle < MAX_CLIENTS_ALLOWED )
    {
        /* We need to protect this in case, we are out of clients and we are about 
        to free a client but we got interrupted just when we are freeing one. */
        INTLOCK() ;
        pm_array_of_vbatt_registrars[handle].pm_is_table_in_use = FALSE ;
        INTFREE() ;

        /* Set all the elements to Zero.*/
        memset( (&pm_array_of_vbatt_registrars[handle]) , 0 ,
                (sizeof(pm_vbatt_client_data_template)) ) ;
    }
    else /* Could not find the handle */
    {
        err = VBATT_DEREGISTRATION_FAILED ;
    }

    return err ;
}


/*===========================================================================

FUNCTION pm_vbatt_modify_client             EXTERNAL FUNCTION

DESCRIPTION
    This function is used by the client to modify the conditons for calling the CB.
    When all the conditions are met, the specified CB is called. This function can
    only be called if client already has a valid handle.

EXAMPLE:
    err =                                           Check for the error.
    pm_vbatt_modify_client ( 
                            test_handle1         ,  handle obtained during registration.
                            3500                 ,  Voltage in mV, between 2800 & 4300 
                            voltage_direction    ,  UP or DOWN
                            cb_func              ,  CB that will be called.
                            func_void_ptr_arg       void pointer argument of the CB function.
                      ) ; 
    if ( err )
    { printf "Client Modification has failed " ; }
    else
    { printf "Client modified successfully"}

INPUT PARAMETERS
#   Parameter type:        &         Parameter name:       &    Range: 
1.  uint8                            test_handle1               0 - (MAX_CLIENTS_ALLOWED - 1)        
2.  pm_battery_voltage_type          vbatt_voltage ,            New voltage from 2800-4200 in mV 
3.  pm_vbatt_voltage_tracking_type   voltage_direction ,        New direction 
                                                                VBATT_VOLTAGE_ABOVE_THIS_LEVEL 
                                                                VBATT_VOLTAGE_BELOW_THIS_LEVEL 
                                                                are preferred.
4.  pm_vbatt_cb_type                 cb_funcpointer ,           New CB, non-Null
5.  pm_vbatt_void_ptr                voidptr ,                  Optional
     
RETURN VALUE
 Return value type: vbatt_error_type 
 0 : Call Successfull.
 Non-0 : Call failed, see error codes
 test_handle1 is a registration handle and an index to uniquely specify a vbatt client 
 out of many. While modifying a client, a unique handle is required for a successful 
 modification. 

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()
  4) pm_vbatt_register() 
  
SIDE EFFECTS
  NON
===========================================================================*/

vbatt_error_type 
pm_vbatt_modify_client ( 
                          pm_batt_index handle ,
                          pm_battery_voltage_type vbatt_voltage , 
                          pm_vbatt_voltage_tracking_type voltage_direction , 
                          pm_vbatt_cb_type cb_funcpointer ,
                          pm_vbatt_void_ptr voidptr
                     )
{
    vbatt_error_type err = VBATT_MODIFICATION_SUCCESSFUL ;
    if (     ( vbatt_voltage <= PM_VBATT_COMPARATOR_MAX ) &&
             ( vbatt_voltage >= PM_VBATT_COMPARATOR_MIN ) &&
             ( voltage_direction <= VBATT_VOLTAGE_LEVEL ) &&
             ( cb_funcpointer != NULL )                   &&
             ( handle < MAX_CLIENTS_ALLOWED ) )
    {
        pm_array_of_vbatt_registrars[handle].desired_vbatt_voltage = vbatt_voltage ;
        pm_array_of_vbatt_registrars[handle].pm_voltage_direction = voltage_direction ;
        pm_array_of_vbatt_registrars[handle].pm_vb_cb_function = cb_funcpointer ;
        pm_array_of_vbatt_registrars[handle].void_ptr_of_cb_func = voidptr ;
    }
    else /* Could not find the handle */
    {
        err = VBATT_DEREGISTRATION_FAILED ;
    }

    return err ;
}



/*===========================================================================

FUNCTION pm_service_vbatt_tables                           EXTERNAL FUNCTION

DESCRIPTION
    This function is used to call client callbacks.

INPUT PARAMETERS
NON

RETURN VALUE
NON

DEPENDENCIES
  The following function must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()

  SIDE EFFECTS
  Interrupts are LOCKED while making a local copy of CB function and it's void
  pointer argument.

===========================================================================*/

void pm_service_vbatt_tables ( void ) 
{
    static pm_battery_voltage_type pm_prev_vbatt_voltage ;
    if ( pm_did_vbatt_change () )
    {
        uint8 table_count ;
        pm_vbatt_voltage_tracking_type pm_vbatt_voltage_direction =
             VBATT_VOLTAGE_LEVEL ;

        pm_battery_voltage_type pm_working_tmp_voltage = 
            pm_comparator_vbatt_read () ;

		/* Update the parameters of the battery by reading various ADCs */
		( void ) vbatt_update_adc_battery_params ( ) ;

        /* New voltage greater than the old one */
        if ( pm_working_tmp_voltage > pm_prev_vbatt_voltage )
        {
            pm_vbatt_voltage_direction = VBATT_VOLTAGE_UP ;
        }
        else if ( pm_working_tmp_voltage < pm_prev_vbatt_voltage )
        {
            pm_vbatt_voltage_direction = VBATT_VOLTAGE_DOWN ;
        }

        /* Traverse through all the tables and check to see if any CBs need to be called */
        for ( table_count = 0 ; table_count < MAX_CLIENTS_ALLOWED ; table_count++ ) 
        {
            /* 3 things, in-use, desired voltage and proper direction */
            if ( pm_array_of_vbatt_registrars[table_count].pm_is_table_in_use )
            {
                boolean cb_criteria_met = FALSE ;
                switch ( pm_array_of_vbatt_registrars[table_count].pm_voltage_direction )  
                {
                    case VBATT_VOLTAGE_UP:
                        /* 
                        Example: if pm_working_tmp_voltage = 3550 mv and desired_vbatt_voltage is 
                        3500mV it is OK. All ranges from 3500 upto 3599 are acceptable */
                        if  ( 
                              /* Proper Voltage */
                              ( ( pm_working_tmp_voltage >= 
                                pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage ) &&
                              ( pm_working_tmp_voltage < 
                                (pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage 
                                 + 99) ) ) 
                            )
                        {
                            cb_criteria_met = TRUE ;
                        }
                        break ;

                    case VBATT_VOLTAGE_DOWN:
                        /* 
                        Example: pm_working_tmp_voltage = 3550 mv and desired_vbatt_voltage is 
                        3500mV it is OK. All ranges from 3500 down to 3401 are acceptable */
                        if  ( 
                              ( ( pm_working_tmp_voltage <= 
                                pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage ) &&
                              ( pm_working_tmp_voltage > 
                                (pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage 
                                 - 99) )) 
                            )
                        {
                            cb_criteria_met = TRUE ;
                        }
                        break ;

                    case VBATT_VOLTAGE_ABOVE_THIS_LEVEL:
                        /* Example: If pm_working_tmp_voltage voltage is greater than 
                        desired_vbatt_voltage, which is say, 3600mV. Then this criteria will
                        be satisfied for all the voltages from 3600mV to 4300 mV  i and callback
                        will be called for 3650mV, 3750mV, 3850mV etc..
                        */
                        if ( (pm_working_tmp_voltage 
                             >= pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage ) 
                             &&  ( pm_vbatt_voltage_direction == VBATT_VOLTAGE_UP ) )
                        {
                            cb_criteria_met = TRUE ;
                        }
                        break ;
 
                        
                    case VBATT_VOLTAGE_BELOW_THIS_LEVEL:
                        /* Example: If pm_working_tmp_voltage voltage is below than 
                        desired_vbatt_voltage, which is say, 3600mV. Then this criteria will
                        be satisfied for all the voltages from 3600mV to 2800 mV  */
                        if ( (pm_working_tmp_voltage 
                             <= pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage ) 
                             &&  ( pm_vbatt_voltage_direction == VBATT_VOLTAGE_DOWN ) )
                        {
                            cb_criteria_met = TRUE ;
                        }
                        break ;

                    case VBATT_VOLTAGE_LEVEL:
                        if ( pm_array_of_vbatt_registrars[table_count].desired_vbatt_voltage 
                               == pm_working_tmp_voltage )  
                        {
                            cb_criteria_met = TRUE ;
                        }
                        break ;

                    /* Voltage changing for any reason; either a voltage-change
                       or a charger event */
                    case VBATT_ALL_ACTIVITY:
                        {
                            cb_criteria_met = TRUE ;
                        }
                        break ;

                    default:
                        cb_criteria_met = FALSE ;
                }


                if  ( cb_criteria_met ) 
                {
                    pm_vbatt_cb_type func ;
                    pm_vbatt_void_ptr vptr ;

                    /* We dont want anyone de-registering or modifying the client 
                      while we are trying to call CB */
                    INTLOCK() ; 
                    func = 
                        pm_array_of_vbatt_registrars[table_count].pm_vb_cb_function ;
                    vptr =  
                        pm_array_of_vbatt_registrars[table_count].void_ptr_of_cb_func ;
                    INTFREE() ;

                    if ( func != NULL ) 
                    {
                        /* Call the callback */
                        func ( vptr ) ;
                    }
                }/* cb_criteria_met */
            }/* if ( pm_array_of_vbatt_registrars[table_count].pm_is_table_in_use ) */
        }/* Continue till table-traversed or 0-volt-entry or matching-voltage */
        pm_prev_vbatt_voltage = pm_working_tmp_voltage ;
    } /* if ( pm_did_vbatt_change () )  */
}

/*===========================================================================

FUNCTION pm_vbatt_get_client_info                       EXTERNAL FUNCTION

DESCRIPTION
    This function is used by the client to query the parameters with which are
    currently registered with the vbatt driver. This is useful as the client does
    not have to maintain it's own copy of parameters. Handle still needs to be 
    saved and maintained by the client.

EXAMPLE:
    err = 
    pm_vbatt_get_client_info ( 
                            test_handle1         ,  handle obtained during registration.
                            3500                 ,  Voltage in mV, between 2800 & 4300 
                            voltage_direction    ,  UP or DOWN
                            cb_func              ,  CB that will be called.
                            func_void_ptr_arg       void pointer argument of the CB function.
                      ) ; 
 
INPUT PARAMETERS
#   Parameter type:        &         Parameter name:       &    Range: 
1.  uint8                            test_handle1               0 - (MAX_CLIENTS_ALLOWED - 1)        
2.  pm_battery_voltage_type          vbatt_voltage ,            New voltage from 2800-4200 in mV 
3.  pm_vbatt_voltage_tracking_type   voltage_direction ,        New direction 
                                                                VBATT_VOLTAGE_ABOVE_THIS_LEVEL 
                                                                VBATT_VOLTAGE_BELOW_THIS_LEVEL 
                                                                are preferred.
4.  pm_vbatt_cb_type                 cb_funcpointer ,           New CB, non-Null
5.  pm_vbatt_void_ptr                voidptr ,                  Optional
     
RETURN VALUE
 Return value type: vbatt_error_type 
 0 : Call Successfull.
 Non-0 : Call failed, see error codes
 test_handle1 is a registration handle and an index to uniquely specify a vbatt client 
 out of many. While querying a client, a unique handle is required for a successful query.

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()
  4) pm_vbatt_register() 
  
SIDE EFFECTS
  NON
===========================================================================*/
vbatt_error_type 
pm_vbatt_get_client_info ( 
                          pm_batt_index handle ,
                          pm_battery_voltage_type *vbatt_voltage , 
                          pm_vbatt_voltage_tracking_type *voltage_direction , 
                          pm_vbatt_cb_type cb_funcpointer ,
                          pm_vbatt_void_ptr voidptr
                     )
{
    vbatt_error_type err = VBATT_INTERROGATION_SUCCESSFUL ;
    if ( handle < MAX_CLIENTS_ALLOWED )
    {
        *vbatt_voltage = pm_array_of_vbatt_registrars[handle].desired_vbatt_voltage ;
        *voltage_direction = pm_array_of_vbatt_registrars[handle].pm_voltage_direction ;
        cb_funcpointer = pm_array_of_vbatt_registrars[handle].pm_vb_cb_function ;
        voidptr = pm_array_of_vbatt_registrars[handle].void_ptr_of_cb_func ;
    }
    else /* Could not find the handle */
    {
        err = VBATT_INTERROGATION_FAILED ;    
    }
    return err ;
}




/*===========================================================================

FUNCTION pm_vbatt_read pm_vbatt_get_client_info                       EXTERNAL FUNCTION

DESCRIPTION
    This function is used by the clients on the ARM11 side to query the vbatt 
    voltage. 

EXAMPLE:
    uint16 battery_voltage = pm_vbatt_read  ( ) ; 
    printf ( "Battery voltage detected by the PMIC is: %d", battery_voltage ) ;
 
INPUT PARAMETERS
    NONE
    
RETURN VALUE
 Return value type: uint16 
 Valid voltage ranges from 2800 to 4300 mv representing 2300 and 4300.

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()
  
SIDE EFFECTS
  NON
===========================================================================*/
uint16 
pm_vbatt_read ( void ) 
{       
    pm_battery_voltage_type vbatt_voltage_in_mv =  
    pm_comparator_vbatt_read ( ) ;
    
    return ( (uint16) vbatt_voltage_in_mv ) ;
}




/*===========================================================================

FUNCTION PM_VBATT_MIMIC_LEGACY_VBATT_READ               EXTERNAL FUNCTION

DESCRIPTION
    This function is can be used to return scaled values just like the old
    vbatt driver returned. 

EXAMPLE:
    uint16 battery_voltage = pm_vbatt_read  ( ) ; 
    printf ( "Battery voltage detected by the PMIC is: %d", battery_voltage ) ;
 
INPUT PARAMETERS
    NONE
    
RETURN VALUE
 Return value type: uint8 
 Valid voltage ranges from 2800 to 4300 mv represented by 159 through 206.

DEPENDENCIES
  The following functions must have been called:
  1) pm_init()
  2) vbatt_init()
  3) vbatt_task_init()
  
SIDE EFFECTS
  NON
===========================================================================*/

uint8 
pm_vbatt_mimic_legacy_vbatt_read ( void ) 
{
    uint16 unscaled_vbatt_reading = 
    pm_comparator_vbatt_read () ;


    /* 
        Scale used for legacy vbatt:
        VMAX: 157 will correspond to 2800 mV and
        VMIN: 157 + 49 = 206 to 4300 mV
    */                                                 

    /* Example 3550 will yeid 3550 - 2800 = 750 */
    unscaled_vbatt_reading -= PM_VBATT_COMPARATOR_MIN ;     

    /* 750 is say m% of the range but scaled to 49 parts 
     * 750 will then translate to 
     * 750 X 49 = 36750 & 36750/1500 = 24.5 ~ 25 
     */
    unscaled_vbatt_reading = ( unscaled_vbatt_reading * 49 ) / PM_VBATT_RANGE ;     

    /* Final vbatt return value then is: min_vbatt + scaled value 
     * So in this case: 157 + 25 = 182
     */
   unscaled_vbatt_reading += 157 ;

   return ( (uint8) ( unscaled_vbatt_reading ) ) ;
}
