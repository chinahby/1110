#ifndef _OI_BT_STACK_INIT_H
#define _OI_BT_STACK_INIT_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_bt_stack_init.h#1 $ 

$Log: $

===========================================================================*/
/**
 * @file  
 *
 * This file provides the interface for initialization and control of the protocol stack.
 * 
 * Initialization functions for the individual protocol stack and support modules of 
 * BLUEmagic 3.0 software are collected together by this file (oi_bt_stack_init.h). 
 * In the default implementation, module initialization is performed by oi_bt_stack_init_default.c.
 * Configuration parameters for all modules primarily control the amount of data memory space (RAM)
 * that will be allocated for buffers, queues, et cetera. The initialization parameter values 
 * should be modified according to the needs of the application.
 *
 * Configuration data structures are defined in oi_bt_stack_config.h. Configuration values are set
 * in oi_bt_stack_config_default.c; documentation for this file contains recommendations for parameter settings.
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_status.h"
#include "oi_stddefs.h"
#include "oi_bt_stack_config.h"


/**
    This function resets all initialization flags.
    
    Do not call this function unless you are sure that you know what you are doing. 
    
    This function should only be called as part of a global, reset/restart process.
*/
extern void OI_InitFlags_ResetAllFlags(void) ;

/**
    This function initializes the configuration table.
    
    Do not call this function unless you are sure that you know what you are doing. 
    
    Initialization sets all configuration pointers to their default values.
    This function should be called before initializing any profiles or core protocol stack
    components.
*/
void OI_ConfigTable_Init(void) ;

/**
 * A callback function of this type is used to indicate when the BLUEmagic 3.0 protocol stack 
 * requires service by the application's dispatch thread/routine.
 *
 * @param msec      number of milliseconds before the protocol stack requires service
 */
typedef void(*OI_DISPATCH_SERVICE_REQUEST_HANDLER)(OI_UINT msec);


/**
 * This function returns the version string for the BLUEmagic 3.0 protocol stack.
 */
OI_CHAR *OI_Stack_Version(void);

/** 
 * This function initializes the transport interface.
 *
 * This function establishes a connection to a Bluetooth module and sets up reads for each of
 * the Bluetooth input packet types.
 */
extern OI_STATUS OI_HciIfc_Init(void);


/** 
 * This function returns the transport initialization status.
 *
 *  @return      status of the initialization
 *                - OI_OK: initialization is successful and complete.
 *                - OI_STATUS_INITIALIZATION_PENDING: Initialization is still in progress.
 *                - Any other value indicates initialization failure and is likely to be a fatal 
 *                  error that will prevent the stack from operating properly.
 */
extern OI_STATUS OI_HciIfc_InitStatus(void) ;

/**
 * This function initializes the core support library.
 */
extern OI_STATUS OI_Support_Init(void);

/** 
 * This function initializes the CThru Dispatcher.
 *
 * @param Config         @deprecated configuration parameters for the Dispatcher:
 *                       This parameter is deprecated and should be specified as NULL.
 *                       The Dispatcher will use configuration from the configuration table.
 *                       Parameter is still supported for backward compatibility.
 *
 * @param handler        parameter passed in via Stack_Init(), registered with the
 *                       stack to allow the stack to make service requests to the runtime environment 
 *
 * @return               OI_OK, if successful.
 */
extern void OI_Dispatch_Init(const OI_CONFIG_DISPATCH *Config,
                             OI_DISPATCH_SERVICE_REQUEST_HANDLER handler);

/** 
 * 
 * This function invokes all callback functions that are ready to run. 
 * See the @ref dispatch_docpage section for details.
 *
 * @return               a count of the number of callback functions that were called, a value that can
 *                       be used to test if any callback functions were called
 */
extern OI_INT16 OI_Dispatch_Run(void);


/** 
 * This function initializes the L2CAP layer. See oi_l2cap.h for more details.
 *      
 * @param params         @deprecated configuration parameters for L2CAP:
 *                       This parameter is deprecated and should be specified as NULL.
 *                       L2CAP will use configuration from the configuration table.
 *                       Parameter is still supported for backward compatibility.
 *
 * @return               OI_OK if successful.
 */
extern OI_STATUS OI_L2CAP_Init(const OI_CONFIG_L2CAP *params);


/** 
 * This function initialize the RFCOMM layer. See oi_rfcomm.h for more details.
 *
 * @param params         runtime configuration parameters for RFCOMM
 *
 * @return               OI_OK if successful
 */
extern OI_STATUS OI_RFCOMM_Init(const OI_CONFIG_RFCOMM *params);


/** 
 * This function initializes the Memory Manager. See the @ref memMgr_docpage section 
 * for more details.
 *
 * This function will fail if the actual memory required by the Memory Manager is 
 * greater than the defined heapSize (@ref OI_MEMMGR_HEAP_CONFIG) or if there are too 
 * many pools declared (more than MEMMGR_MAX_POOL_COUNT, which is defined internal 
 * to the protocol stack and is currently 1000).
 *
 *   @param pConfig     a pointer to the Memory Manager configuration parameters
 *
 *   @return            status of Memory Manager configuration:
 *                      - OI_OK: successful
 *                      - Any other value indicates initialization failure and is likely to be a fatal error that
 *                        will prevent the stack from operating properly.
 */
extern OI_STATUS OI_MEMMGR_Init(const OI_CONFIG_MEMMGR *pConfig);



/** 
 * This function initializes the SDP server and SDP database. See oi_sdpdb.h for more details. 
 *
 * @param SDPServerConfig   @deprecated configuration parameters for the SDP server:
 *                          This parameter is deprecated and should be specified as NULL.
 *                          Configuration from the configuration table will be used.
 *                          Parameter is still supported for backward compatibility.
 *
 * @return                  OI_OK if successful
 */
extern OI_STATUS OI_SDPServer_Init(const OI_CONFIG_SDP_SRV *SDPServerConfig);


/** 
 * This function initializes the Policy Manager software module.
 *
 * @param   pConfig      @deprecated configuration parameters for the connection policy manager:
 *                       This parameter is deprecated and should be specified as NULL.
 *                       Configuration from the configuration table will be used.
 *                       Parameter is still supported for backward compatibility.
 *
 * @return  OI_OK if successful
 */
extern OI_STATUS OI_POLICYMGR_Init(const OI_CONFIG_POLICYMGR *pConfig);

/** 
 * This function initializes the security database software module.
 *
 * @param   pConfig     @deprecated array of security policies for services: The array is
 *                      terminated by an element whose service UUID is null.
 *                      This parameter is deprecated and should be specified as NULL.
 *                      Configuration from the configuration table will be used.
 *                      Parameter is still supported for backward compatibility.
 *
 * @return  OI_OK if successful
 */
extern OI_STATUS OI_SECURITYDB_Init(const OI_CONFIG_POLICYMGR *pConfig);


/** 
 * This function initializes the Device Manager. See oi_devmgr.h and @ref devMgr_docpage for more details.
 *
 *  @param pConfig     @deprecated Device Manager configuration parameters:
 *                     This parameter is deprecated and should be specified as NULL.
 *                     Configuration from the configuration table will be used.
 *                     Parameter is still supported for backward compatibility.
 *
 *  @return            status of the Device Manager initialization: 
 *                     - OI_OK: successful
 *                       - Any other value indicates initialization failure and is likely to be a fatal 
 *                      error that will prevent the stack from operating properly.
 */
extern OI_STATUS OI_DEVMGR_Init(const OI_CONFIG_DEVMGR *pConfig) ;


/** 
 * This function initializes the Security Manager. See @ref secMgr_docpage for more details.

    @note The initialization function only initializes the Security Manager software
    (allocating memory, et cetera). This initialization function does not directly configure, 
    enable, or disable security mechanisms. The Security Manager configuration function 
    (OI_SECMGR_Configure()) must be called to set security policy.
 
    @param pConfig    @deprecated Security Manager initialization parameters:
 *                    This parameter is deprecated and should be specified as NULL.
 *                    Configuration from the configuration table will be used.
 *                    Parameter is still supported for backward compatibility.

    @return           status of the Security Manager initialization: 
 *                     - OI_OK: successful
 *                       - Any other value indicates initialization failure and is likely to be a fatal 
 *                      error that will prevent the stack from operating properly.
*/
extern OI_STATUS  OI_SECMGR_Init(const OI_CONFIG_SECMGR *pConfig) ;


/** 
 * This function initializes the HCI command layer. 
 * 
 * The functions OI_HCIAPI_Init_Vars() and OI_HCIAPI_Init_Device() are called in turn 
 * by this function. The former initializes variables in the HCI protocol layer; 
 * the latter sends HCI commands to initialize the Bluetooth device. The initialization 
 * steps are broken into separate functions to allow for the rare case in which a Bluetooth 
 * device must be sent vendor-specific HCI commands before the device itself is initialized. 
 * 
 * Most application should simply call the OI_HCIAPI_Init() function. 
 * 
 * If you believe that you need to insert the sending of a vendor-specific command between 
 * the two initialization function calls, contact technical support at Open Interface and 
 * we will provide you with a sample initialization file that demonstrates how to do so properly.
 *
 *  @param config     @deprecated HCI initialization parameters:
 *                    This parameter is deprecated and should be specified as NULL.
 *                    Configuration from the configuration table will be used.
 *                    Parameter is still supported for backward compatibility.
 *
 *  @return           status of the HCI initialization: 
 *                     - OI_OK: Initialization was successful and complete
 *                     - OI_STATUS_INITIALIZATION_PENDING: Initialization is in progress but not
 *                       yet complete. The caller must wait until HCI initialization has completed.
 *                       The caller must poll for initialization status by calling OI_HCIAPI_InitStatus().
 *                     - Any other value indicates initialization failure and is likely to be a fatal 
 *                       error that will prevent the stack from operating properly.
 *   
 */
extern OI_STATUS OI_HCIAPI_Init(const OI_CONFIG_HCI*  config) ;

/** 
 * This function initializes the HCI command layer variables.
 *
 *  @param config     @deprecated HCI initialization parameters:
 *                    This parameter is deprecated and should be specified as NULL.
 *                    Configuration from the configuration table will be used.
 *                    Parameter is still supported for backward compatibility.
 *
 *  @return           status of the HCI variable initialization: 
 *                     - OI_OK: variable initialization was successful.
 *                       The caller must make a call to OI_HCIAPI_Init_Device() to issue an
 *                       asynchronous series of HCI commands to the device or baseband to
 *                       complete the process, and then wait until HCI initialization has completed.
 *                       The caller must then poll for initialization status by calling OI_HCIAPI_InitStatus().
 *                     - Any other value indicates initialization failure and is likely to be a fatal 
 *                       error that will prevent the stack from operating properly.
 *   
 */
extern OI_STATUS OI_HCIAPI_Init_Vars(const OI_CONFIG_HCI*  config) ;

/** 
 * This function establishes communication between the HCI layer and the external device or
 * the baseband software library. It asynchronously issues a series of HCI commands to establish
 * a known starting point in the communication exchange.
 *
 *  @return        status of the initialization
 *                 - OI_OK: initialization is successful and complete
 *                 - OI_STATUS_INITIALIZATION_PENDING: Device Initialization is in progress
 *                   but not yet complete. The caller must poll for initialization status by
 *                   calling OI_HCIAPI_InitStatus().
 *                 - Any other value indicates initialization failure and is likely to be a fatal 
 *                   error that will prevent the stack from operating properly.
 */
extern OI_STATUS OI_HCIAPI_Init_Device(void) ;

/** 
 * This function reads HCI command layer initialization status.
 *  @return        status of the initialization
 *                 - OI_OK: initialization is successful and complete
 *                 - OI_STATUS_INITIALIZATION_PENDING: initialization is still in progress.
 *                 - Any other value indicates initialization failure and is likely to be a fatal 
 *                   error that will prevent the stack from operating properly.
 */
extern OI_STATUS OI_HCIAPI_InitStatus(void) ;

/**
 * 
 * This function initializes the entire BLUEmagic 3.0 protocol stack and its support modules.
 *
 * @param handler   This is a function in the runtime environment that is called by the BLUEmagic 3.0 
 *                  protocol stack to request immediate or future service from the runtime environment.
 *                  The stack initialization function simply passes this function on to OI_Dispatch_Init().
 * @note This function is not part of the BLUEmagic 3.0 protocol stack per se; this function implementation
 *          must be provided by the user of the stack.  A default implementation of this function
 *          is provided in the SDK.
 */
OI_STATUS OI_BT_Stack_Init(OI_DISPATCH_SERVICE_REQUEST_HANDLER handler);

/**
 * 
 * This function initializes the entire BLUEmagic 3.0 protocol stack and its support modules.
 *
 *  Customization:
 *      The customCB is called after the configuration table is initialized with the default values (typically
 *      specified in oi_bt_stack_config_default.c), but before any core stack components (HCI, L2CAP, etc)
 *      have been initialized.
 *
 *      In the customCB, the application may specify its custom configuration(s) with the API's provided in
 *      oi_config_table.h.  The customCB must not call any BM3 api other OI_CONFIG_TABLE_PUT() and/of 
 *      OI_CONFIG_TABLE_GET().  Please refer to oi_config_table.h file for details of these api's.
 *
 * @param handler   This is a function in the runtime environment that is called by the BLUEmagic 3.0 
 *                  protocol stack to request immediate or future service from the runtime environment.
 *                  The stack initialization function simply passes this function on to OI_Dispatch_Init().
 *
 * @param customCB  This is the callback that is to be invoked at the appropriate point during initialization.
 *                  Specify NULL if there is no customization required
 *
 * @note This function is not part of the BLUEmagic 3.0 protocol stack, per se; this function implementation
 *          must be provided by the user of the stack. A default implementation of this function
 *          is provided in the SDK.
 */
OI_STATUS OI_BT_Stack_Init_Custom(OI_DISPATCH_SERVICE_REQUEST_HANDLER handler, void (*customCB)(void));


/**
 * This function shuts down the transport interface, disconnecting from the Bluetooth module.
 * The Bluetooth module is not reset.
 */
OI_STATUS OI_HCIIfc_Terminate(void);



typedef void(*OI_DEVICE_RESET_COMPLETE_CB)(OI_STATUS status); 

/**
    This function resets the Bluetooth device.
    
    This function is provided for exceptional circumstance when the Bluetooth device or
    baseband needs to be reset. Device reset is normally initiated from within the stack
    in the event of a fatal error in the HCI transport.
    
    Device reset causes an HCI reset command to be sent to the device. Subsequently,
    all existing connections are dropped. The device reset process re-initializes the device
    in the same way as at power-on startup. All device parameter settings are then re-written 
    to the device.
    
    @note       If the device reset process fails, the device will be in some indeterminate
                state and all calls to HCI will be rejected (except OI_DeviceReset()).
    
    @param cmdCompleteCB    callback to invoke when the reset process has completed
    
    @return                 OI_OK if reset is initiated; the callback will be called when the 
                                reset process has completed.
                            Any other return value means that reset will not be initiated; the 
                                callback will not be called.
*/

OI_STATUS   OI_DeviceReset(OI_DEVICE_RESET_COMPLETE_CB  cmdCompleteCB) ;
                    
/*****************************************************************************/
#endif /* _OI_BT_STACK_INIT_H */
