/*! \file 
 *  \n
 *  \brief  vbatt_pmic_comparator.h ---- Header for new VBATT INTERFACE APIS
 *  \n
 *  \n This file contains vbatt function and variable definition to support 
 *  new vbatt interface with the Qualcomm Power Management ICs.
 *  \n
 *  \n &copy; Copyright 2008 QUALCOMM Incorporated, All Rights Reserved
 */
/* ======================================================================= */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/vbatt/main/latest/app/inc/vbatt_pmic_comparator.h#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/01/09   APU     Added support for VBATT filter.
01/26/09   APU     Added support for CHG events to the code.
09/09/08   APU     Added Doxygen Comments to the code.
08/30/08   APU     Added pm_vbatt_mimic_legacy_vbatt_read() to support
                   vbatt_read.  
08/25/08   APU     Added metacomments for RPCing the functions in this file.    
07/26/08   APU     Changed pm_is_table_in_use to boolean from uint8.    
07/08/08   APU     Tested with 6260, 7201 and added comments
01/02/08   APU     Created.
===========================================================================*/

#include "pm.h"                /* For pmic functions       */
#include "pm_vbatt.h"
#include "vbatt.h"
#include "vbatt_task.h"

/*===========================================================================

                     LOCAL TYPE DEFINITIONS

===========================================================================*/
/*===========================================================================

                   GLOBAL VARIABLE DEFINITIONS

===========================================================================*/
/*! \def MAX_CLIENTS_ALLOWED
    \brief Contains the max array size of client-data-holder
    "pm_array_of_vbatt_registrars"
*/
#define MAX_CLIENTS_ALLOWED                     30

typedef uint8 pm_batt_index ; 
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE pm_batt_index META-SPECIFICATION */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef void (*pm_vbatt_cb_type)(void*) ;
typedef void * pm_vbatt_void_ptr ;


/*! \enum vbatt_error_type
 *  \brief This enum contains error code related to VBATT APIs.
 *  \n Zero return code for all successfull transactions
 *  \n Non-Zero indicates some error condition
 */
typedef enum
{
    /*! 0 means successfull Call. Here it means registration was successful
         and a valid handle is returned to the client. 
         0 is the return code for all successfull transactions */                            
    VBATT_REGISTRATION_SUCCESSFUL   = 0                             , 
    /*! Here it means de-registration was successful and that client's 
         call-back functions will no longer be called */                            
    VBATT_DEREGISTRATION_SUCCESSFUL = VBATT_REGISTRATION_SUCCESSFUL ,       
    /*! It means modification was successful and the vbatt-driver has recorded
         new parameters for the client who had registered before. */                            
    VBATT_MODIFICATION_SUCCESSFUL   = VBATT_REGISTRATION_SUCCESSFUL ,
    /*! Client query has succeeded. */                            
    VBATT_INTERROGATION_SUCCESSFUL  = VBATT_REGISTRATION_SUCCESSFUL , 
    /*! Non-Zero means unsuccessful call. Here it means registration failed  
         because driver has ran out of memory.MAX_CLIENTS_ALLOWED needs to be
         increased and the code needs to be recompiled */                            
    VBATT_CLIENT_TABLE_FULL         = 1                             ,
    /*! One of the parameters to the function call was invalid. */                            
    VBATT_REG_PARAMS_WRONG          = 2                             ,
    /*! Client could not be deregistered because perhaps it does not exist */                            
    VBATT_DEREGISTRATION_FAILED     = 4                             ,
    /*! Client could not be modified because perhaps it does not exist */                            
    VBATT_MODIFICATION_FAILED       = 8                             ,
    /*!< Client could not be queried because perhaps it does not exist */                            
    VBATT_INTERROGATION_FAILED      = 16                            ,
    /*! Keeps the count of all errors. Any error code equal to or greater
         than this one means an unknown error. VBATT_LAST_ERROR should be 
         the last error code always with the highest value */                            
    VBATT_LAST_ERROR                = 32  
   
} vbatt_error_type ;



/*! \enum pm_vbatt_voltage_tracking_type
 *  \brief This enum tracks the different direction and CB-points that the VBATT 
 *   driver supports. 
 */
typedef enum
{
    /*!< When the voltage exceeds this specific voltage 
        Use not recommended. */
    VBATT_VOLTAGE_UP = 0 ,   

    /*!< When the voltage falls below this specific voltage 
       Use not recommended. */
    VBATT_VOLTAGE_DOWN ,     

    /*!< Indicates any voltage when the voltage exceeds this level 
       Use recommended. */
    VBATT_VOLTAGE_ABOVE_THIS_LEVEL ,  

    /*!< Indicates any voltage when the voltage falls below this level 
       Use recommended. */
    VBATT_VOLTAGE_BELOW_THIS_LEVEL ,

    /*!< If there is any activity VBATT or Charger get call the CR
         Use recommended. */
    VBATT_ALL_ACTIVITY ,

    /*!< When the voltage is equal to this specific voltage 
       Use not recommended. */
    VBATT_VOLTAGE_LEVEL ,

    /*!< Error condition. */
    VBATT_VOLTAGE_UNKNOWN
} pm_vbatt_voltage_tracking_type ;




/*! \struct pm_vbatt_voltage_tracking_type
 *  \brief This structure keeps the track of all client's information
 */
typedef struct
{
  /*! The voltage at which callback (CB) should be called. */
  pm_battery_voltage_type           desired_vbatt_voltage ;

  /*! The direction when the CB should be called. */
  pm_vbatt_voltage_tracking_type    pm_voltage_direction ;

  /*! The registered callback to be called when voltage and direction specs 
      are met. */
  pm_vbatt_cb_type                  pm_vb_cb_function ; /*  */

  /*! The void pointer argument for the above CB. */
  pm_vbatt_void_ptr                 void_ptr_of_cb_func ;

  /*! Client handle used for internal houskeeping. Obtained by a client 
     during registration. Differentiates one client from another. Once obtained
     the voltage, direction and CB can be changed.  */
  pm_batt_index                     client_handle ;

  /*! Marks the specific element of the array as in use IN_USE == TRUE. */
  boolean                           pm_is_table_in_use ; 
}pm_vbatt_client_data_template ;





/*! ===========================================================================
  
FUNCTION pm_vbatt_register                          EXTERNAL FUNCTION

\details
DESCRIPTION
  \n This function is used by the client to register itself. When all the 
  cconditions are met, the specified CB is called.

\example
  \n global uint8 test_handle1 = 
  \n pm_vbatt_register ( 
     \n 3500                 ,  Voltage in mV, between 2800 & 4300 
     \n voltage_direction    ,  UP or DOWN
     \n cb_func              ,  CB that will be called.
     \n func_void_ptr_arg    ,  void pointer argument of the CB function.
     \n &err                    Check for non-zero values for error-conditions.
                      ) ; 

    Translating to: Register me as vbatt-client and call my CB when the voltage 
    crosses 3500mV mark, in the direction specified. Use the void pointer arg
    supplied. err will let me know if the registration-attempt was successful. 
 
INPUT PARAMETERS
\n #   Parameter type:        &         Parameter name:       &    Range: 
\n 1.  pm_battery_voltage_type          vbatt_voltage ,            2800-4200 
\n 2.  pm_vbatt_voltage_tracking_type   voltage_direction ,        VBATT_VOLTAGE_ABOVE_THIS_LEVEL 
                                                                VBATT_VOLTAGE_BELOW_THIS_LEVEL are preferred.
\ n3.  pm_vbatt_cb_type                 cb_funcpointer ,           Non-Null
\ n4.  pm_vbatt_void_ptr                voidptr ,                  Optional
\ n5.  vbatt_error_type                 *err                       Consult error types.
     
RETURN VALUE
  \return pm_batt_index .
  \n pm_batt_index is a registration handle and an index to uniquely specify 
  a vbatt client out of many. While de-registering, a unique handle is 
  required for a successful deregistration-attempt. This handle should be 
  saved between calls.  
  \n The handle can be between 0 and (MAX_CLIENTS_ALLOWED - 1)  

\dependencies
 DEPENDENCIES
  \n The following function must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()
  
SIDE EFFECTS
  \n Interrupts are LOCKED while checking for free entries in the array to 
  provide immunity from task switching.

===========================================================================*/
pm_batt_index 
pm_vbatt_register ( 
              pm_battery_voltage_type vbatt_voltage , 
              pm_vbatt_voltage_tracking_type voltage_direction , 
              pm_vbatt_cb_type cb_funcpointer , 
              pm_vbatt_void_ptr voidptr ,
              vbatt_error_type *err /*~ PARAM err POINTER */ 
              ) ;   
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_register */
#endif /* FEATURE_HTORPC_METACOMMENTS */




/*! ===========================================================================

FUNCTION pm_vbatt_get_client_info                       EXTERNAL FUNCTION

\details 
DESCRIPTION
  \n This function is used by the client to query the parameters with which 
  are currently registered with the vbatt driver. This is useful as the 
  client does not have to maintain it's own copy of parameters. Handle still
  needs to be saved and maintained by the client.

\example
  \n err = 
  \n pm_vbatt_get_client_info ( 
  \n   test_handle1         ,  handle obtained during registration.
  \n   3500                 ,  Voltage in mV, between 2800 & 4300 
  \n   voltage_direction    ,  UP or DOWN
  \n   cb_func              ,  CB that will be called.
  \n   func_void_ptr_arg       void pointer argument of the CB function.
  \n                  ) ; 
 
INPUT PARAMETERS
\n #   Parameter type:        &         Parameter name:       &    Range: 
\n 1.  uint8                            test_handle1               0 - (MAX_CLIENTS_ALLOWED - 1)        
\n 2.  pm_battery_voltage_type          vbatt_voltage ,            New voltage from 2800-4200 in mV 
\n 3.  pm_vbatt_voltage_tracking_type   voltage_direction ,        New direction 
                                                                VBATT_VOLTAGE_ABOVE_THIS_LEVEL 
                                                                VBATT_VOLTAGE_BELOW_THIS_LEVEL 
                                                                are preferred.
\n 4.  pm_vbatt_cb_type                 cb_funcpointer ,           New CB, non-Null
\n 5.  pm_vbatt_void_ptr                voidptr ,                  Optional
     
RETURN VALUE
  \return vbatt_error_type 
  \n 0 : Call Successfull.
  \n Non-0 : Call failed, see error codes
  \n test_handle1 is a registration handle and an index to uniquely specify a 
  vbatt client out of many. While querying a client, a unique handle is 
  required for a successful query.

DEPENDENCIES
  \n The following functions must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()
  \n 4) pm_vbatt_register() 
  
SIDE EFFECTS
  \n NON\n

===========================================================================*/
vbatt_error_type 
pm_vbatt_get_client_info ( 
pm_batt_index handle ,
pm_battery_voltage_type *vbatt_voltage , /*~ PARAM vbatt_voltage POINTER */ 
pm_vbatt_voltage_tracking_type *voltage_direction , /*~ PARAM voltage_direction POINTER */  
pm_vbatt_cb_type cb_funcpointer , 
pm_vbatt_void_ptr voidptr 
                          ) ; 
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_get_client_info */
#endif /* FEATURE_HTORPC_METACOMMENTS */





/*! ===========================================================================

FUNCTION pm_vbatt_modify_client             EXTERNAL FUNCTION

\details 
DESCRIPTION
    \n This function is used by the client to modify the conditons for calling the CB.
    When all the conditions are met, the specified CB is called. This function can
    only be called if client already has a valid handle.

\example
  \n err =                   //Check for the error.
    \n pm_vbatt_modify_client ( 
    \n test_handle1         ,  handle obtained during registration.
    \n 3500                 ,  Voltage in mV, between 2800 & 4300 
    \n voltage_direction    ,  UP or DOWN
    \n cb_func              ,  CB that will be called.
    \n func_void_ptr_arg       void pointer argument of the CB function.
                      ) ; 
  \nif ( err )
  \n   { printf "Client Modification has failed " ; }
  \nelse
  \n   { printf "Client modified successfully"}

INPUT PARAMETERS
\n #   Parameter type:        &         Parameter name:       &    Range: 
\n 1.  uint8                            test_handle1               0 - (MAX_CLIENTS_ALLOWED - 1)        

\n 2.  pm_battery_voltage_type          vbatt_voltage ,            New voltage from 2800-4200 in mV 
\n 3.  pm_vbatt_voltage_tracking_type   voltage_direction ,        New direction 
                                                                VBATT_VOLTAGE_ABOVE_THIS_LEVEL 
                                                                VBATT_VOLTAGE_BELOW_THIS_LEVEL 
                                                                are preferred.
\n 4.  pm_vbatt_cb_type                 cb_funcpointer ,           New CB, non-Null
\n 5.  pm_vbatt_void_ptr                voidptr ,                  Optional
     
RETURN VALUE
 \return vbatt_error_type 
 \n 0 : Call Successfull.
 \n Non-0 : Call failed, see error codes
 test_handle1 is a registration handle and an index to uniquely specify a vbatt client 
 out of many. While modifying a client, a unique handle is required for a successful 
 modification. 

DEPENDENCIES
  \n The following functions must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()
  \n4) pm_vbatt_register() 
  
SIDE EFFECTS
  \n NON\n

===========================================================================*/
vbatt_error_type 
pm_vbatt_modify_client (  pm_batt_index handle ,
                          pm_battery_voltage_type vbatt_voltage , 
                          pm_vbatt_voltage_tracking_type voltage_direction , 
                          pm_vbatt_cb_type cb_funcpointer ,
                          pm_vbatt_void_ptr voidptr
                     ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_modify_client */
#endif /* FEATURE_HTORPC_METACOMMENTS */





/*! ===========================================================================

FUNCTION pm_vbatt_deregister_client                          EXTERNAL FUNCTION

\details
DESCRIPTION
  \nThis function is used by the client to de-register itself. The client 
  may no-longer need the services of the vbatt-driver. One such case can be
  the Blue-Tooth-client (BT). BT may not need to monitor the battery voltage
  if BT is turned off. This frees up resources for other clients. 

\example
  \n err = pm_vbatt_deregister_client (test_handle1) ;

INPUT PARAMETERS
\n -   Parameter type:   Parameter name: &    Range: 
\n 1.  uint8             test_handle1         0 to (MAX_CLIENTS_ALLOWED - 1)

  \return vbatt_error_type 
  \n 0 : Call Successfull.
  \n Non-0 : Call failed, see error codes
  \n pm_batt_index is a registration handle and an index to uniquely specify a 
  vbatt client out of many. While de-registering, a unique handle is required 
  for a successful deregistration-attempt. This handle should be saved between
  calls. The handle can be between 0 and (MAX_CLIENTS_ALLOWED - 1)   

DEPENDENCIES
  \n The following functions must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()
  \n 4) pm_vbatt_register() 

SIDE EFFECTS
  \n Interrupts are LOCKED while freeing the client in the array to provide 
  immunity from task switching.\n

===========================================================================*/
vbatt_error_type 
pm_vbatt_deregister_client ( pm_batt_index handle ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_deregister_client */
#endif /* FEATURE_HTORPC_METACOMMENTS */




/*! ===========================================================================

FUNCTION pm_service_vbatt_tables                           EXTERNAL FUNCTION

\details
DESCRIPTION
  \n This function is used to call client callbacks.

INPUT PARAMETERS
  \n NON

\return 
  \n NON

DEPENDENCIES
  \n The following function must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()

  SIDE EFFECTS
  \n Interrupts are LOCKED while making a local copy of CB function and it's void
  pointer argument.\n

===========================================================================*/
void 
pm_service_vbatt_tables ( void ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_service_vbatt_tables */
#endif /* FEATURE_HTORPC_METACOMMENTS */




/*! ===========================================================================

FUNCTION pm_vbatt_is_servicing_tables_enabled             EXTERNAL FUNCTION

\details
DESCRIPTION
    \n The driver can stop servicing the vbatt-clients. This can be used for 
    debugging pm_vbatt_enable_disable_client_servicing is the variable that
    can turn or turn the client-servicing. This function is used to query if
    servicing is ON or OFF.

\example
  \n if ( pm_vbatt_is_servicing_tables_enabled () )
  \n   { printf "Clients are being serviced." } ;
  \n else
  \n   { printf "Clients are not being serviced." } ;

INPUT PARAMETERS
\n NONE

  \return boolean          
  \n TRUE : Servicing Enabled
  \n FALSE : Servicing Disabled 

DEPENDENCIES
  \n The following function must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()

SIDE EFFECTS
  \n NON \n

===========================================================================*/
boolean 
pm_vbatt_is_servicing_tables_enabled ( void ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_is_servicing_tables_enabled */
#endif /* FEATURE_HTORPC_METACOMMENTS */




/*! ===========================================================================

FUNCTION pm_vbatt_enable_table_servicing EXTERNAL FUNCTION

\details 
DESCRIPTION
  \n The driver can stop servicing the vbatt-clients. This can be used for
  debugging pm_vbatt_enable_disable_client_servicing is the variable that
  can turn or turn the client-servicing. This function is used to turn the
  table-servicing ON.

\example
  \n if ( pm_vbatt_is_servicing_tables_enabled () )
  \n   { printf "Clients servicing switched ON." } ;

INPUT PARAMETERS
  \n NONE

 \return boolean          
 \n TRUE : Servicing Enabled
 \n FALSE : Servicing Disabled 

DEPENDENCIES
  \n The following function must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()

SIDE EFFECTS
  \n NONE\n

===========================================================================*/
boolean 
pm_vbatt_enable_table_servicing ( void ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_enable_table_servicing */
#endif /* FEATURE_HTORPC_METACOMMENTS */





/*! ===========================================================================

FUNCTION pm_vbatt_disable_table_servicing EXTERNAL FUNCTION

\details
DESCRIPTION
  \n The driver can stop servicing the vbatt-clients. This can be used for 
  debugging pm_vbatt_enable_disable_client_servicing is the variable that 
  can turn or turn the client-servicing. This function is used to turn the 
  table-servicing OFF.

\example
  \n if ( pm_vbatt_disable_table_servicing () )
  \n   { printf "Clients servicing switched OFF." } ;

INPUT PARAMETERS
  \n NONE

 \return boolean          
 \n TRUE : Servicing Enabled
 \n FALSE : Servicing Disabled 

DEPENDENCIES
  \n The following function must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()

SIDE EFFECTS
  \n NON \n

===========================================================================*/
boolean 
pm_vbatt_disable_table_servicing ( void ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_disable_table_servicing */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/*! ===========================================================================

FUNCTION pm_vbatt_read pm_vbatt_get_client_info                       EXTERNAL FUNCTION

\details 
DESCRIPTION
  \n This function is used by the clients on the ARM11 side to query the vbatt 
    voltage. 

\example
  \n uint16 battery_voltage = pm_vbatt_read  ( ) ; 
  \n printf ( "Battery voltage detected by the PMIC is: %d", battery_voltage ) ;
 
INPUT PARAMETERS
  \n NONE
    
RETURN VALUE
 \return uint16 
 \n Valid voltage ranges from 2800 to 4300 mv representing 2300 and 4300.

DEPENDENCIES
  \n The following functions must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()
  
SIDE EFFECTS
  \n NON \n

===========================================================================*/
uint16 
pm_vbatt_read ( void ) ; 
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_read */
#endif /* FEATURE_HTORPC_METACOMMENTS */



/*! ===========================================================================

FUNCTION pm_vbatt_mimic_legacy_vbatt_read           EXTERNAL FUNCTION

\details
DESCRIPTION
    \nThis function is used in lieu of the legacy vbatt_read() in vbatt.c file.
    The value returned is akin to one returned by vbatt_read and is scaled
    similarly.

\example
  \n uint8 battery_voltage =  pm_vbatt_mimic_legacy_vbatt_read ( ) ; 
  \n printf ( "Battery voltage detected by the PMIC is: %d", battery_voltage ) ;
 
INPUT PARAMETERS
  \n NONE
    
RETURN VALUE
  \return uint8 
  \n Valid voltage ranges from 157 to 206 representing 2300mV and 4300mV.

DEPENDENCIES
  \n The following functions must have been called:
  \n 1) pm_init()
  \n 2) vbatt_init()
  \n 3) vbatt_task_init()
  
SIDE EFFECTS
  \n NON \n

===========================================================================*/

uint8 
pm_vbatt_mimic_legacy_vbatt_read ( void ) ;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION pm_vbatt_mimic_legacy_vbatt_read */
#endif /* FEATURE_HTORPC_METACOMMENTS */


void 
sample_cb_func1 ( void *voidptr ) ;





