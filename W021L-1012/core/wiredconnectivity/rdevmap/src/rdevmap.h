#ifndef RDEVMAP_H
#define RDEVMAP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R U N T I M E   D E V I C E   M A P P E R   S E R V I C E S 

                       H E A D E R    F I L E
                            

GENERAL DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Runtime Device Mapper (RDM).  
  
EXTERNALIZED FUNCTIONS

  rdm_init()
    Initializes the Runtime Device Mapper Services. This function must
    be called before other functions of this module.

  rdm_get_prev_ports()
    Read in the port mappings last know by RDM from EFS.

  rdm_issue_opens()
    RDM issues opens to SRVCs based on last mapping before power down. If there
    is no EFS, the default mapping of DS->U1 and DIAG->U2 (if U2 defined) is
    performed.

  rdm_assign_port()
    Request RDM assign a specified application to the specified device.

  rdm_assign_port_tmp()
    Similar to rdm_assign_port(), except the change is not recorded in NV.

  rdm_close_device()
    This function will close a device used by a service.

  rdm_notify()
    Notify RDM of request completion/status.

  rdm_get_device()
    Query RDM for the device that is currently assigned to the specified
    service (if any).

  rdm_get_multi_dev()
    Query the Runtime Device Mapper for the ports/devices currently being 
    used by the specified application/service.

  rdm_get_service()
    Query the Runtime Device Mapper for the application/service that is 
    currently assigned to the specified port/device (if any).

  rdm_get_valid_ports()
    Query the RDM for the valid devices the specified application can map to.

  rdm_register_open_func()
    Function for Service Tasks to register their open function. This open 
    function is what RDM calls to instruct a Service to open a specified port.

  rdm_register_close_func()
    Function for Service Tasks to register their close function. This close 
    function is what RDM calls to instruct a Service to close it's open port.

  rdm_register_func_multi_dev()
    Function for Service Tasks to register their open/close functions.  The 
    functions are what RDM calls to instruct a Service to open/close device.

  rdm_get_device_str()
    Query the RDM for the ASCII string that corresponds to the 
    rdm_device_enum_type specified.

  rdm_is_srv_holding_dev()
    Test if a service is using a device.

  rdm_set_srv_compat_mask()
    Set device compatible mask for the service from 
    (RDM_LAST_STATIC_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which 
    device this service can use.

  rdm_is_srv_dev_compat()
    Test if a service and a device is compatible. That is whether the service
    can use the device.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  rdm_init() must be called prior to any of the other functions of
  this module.
                 
Copyright (c) 2000-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/rdevmap.h_v   1.11.1.1   07 May 2003 18:43:06   wxie  $
  $Header: //depot/asic/msmshared/services/rdevmap/02.00.XX/rdevmap.h#1 $ $DateTime: 2008/10/19 19:39:23 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/07   ar      Added RDM_SMD_DATA11_DEV.
10/11/06   rh      Adding support for FEATURE_HS_USB
08/23/06   sg      Added rdm service for brew extended device.
06/26/06   gd      Cleaned up code - removed usbdc.h inclusion which is not required.
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
06/19/06   sb      Added the function to control clk and modem ISR for USB
06/12/06   dsn     Added new BT SoC service type, exported apis for MSM7500
05/01/06   vp      Changes for enableing multiple ports for data.
01/04/06   ptm     Restore SMEM code, removed RPCMP and DIAG_MP services and
                   devices.
08/19/05   cp      Support for APS virtual port
06/05/05   TMR     Merged in MSM7500 code
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
01/11/05   cc      Duplicate USB  port entries with old names in 
                   rdm_device_enum_type for better backwards compatibility.
12/23/04   cc      Rename USB_CDC, USB_DIAG, USB_NMEA device to be USB_MDM, USB_SER1
                   and USB_SER2 respectively. 
11/24/04   cc      Added support for RDM_USB_NET_WWAN_DEV and RDM_DATA_WWAN_SRVC.
10/20/04   ptm     added RPC MP service.
10/20/04   adm     added DIAG MP service and SMEM DIAG and RPC devices.
09/22/04   ar      Merge serializer support under FEATURE_DATA_SERIALIZER.
04/20/04   pn      Added support for BT_SPP port. 
04/20/04   pn      Added support for BT_SPP port.
07/01/03   cc      added RDM_BREW_SRVC into serice enum type as an OEM service. 
04/08/02   wxie    Protect RDM state in critical section, add capability 
                   to open multiple ports for a service, add new service 
                   easily, and other internal RDM changes.
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
07/15/02   ry      Added support for the LSM_CSD port
06/17/02   cc      Removes feature wrapping around RDM_RPC_SRVC in 
                   rdm_service_enum_type. This is a part of CR23030 fix.
02/06/02   cc      Add third UART supporting. 
01/09/02   cc      Add ONCRPC service supporting.
12/04/01   cc      Change the function name rdm_data_got_at$qcdmg() 
                   to rdm_data_got_atqcdmg() to avoid future headach.
08/06/01   cc      Merge gpsOne rdevmap.h -r 1.2 to add NMEA supporting. 
03/26/01   dl      Add support for SMEM_DS port.
12/21/00   dwp     Add support for EFS.
11/27/00   dwp     Comment updates.
11/16/00   dwp     Added UI interfaces and updated comments.
11/14/00   dwp     Created module.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "customer.h"              /* Customer Specific Definitions        */
#include "sio.h"                   /* Serial I/O Services.                 */

#ifdef FEATURE_RUNTIME_DEVMAP /* { */

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Type that defines callback status from the Runtime Device Mapper
---------------------------------------------------------------------------*/
typedef enum
{
  RDM_NOT_ALLOWED_S = -1,  /* The port, app combination is not allowed       */
  RDM_DONE_S,              /* RDM will attempt or the app has completed      */
  RDM_DEVMAP_BUSY_S,       /* RDM is currently unable to service request     */
  RDM_APP_BUSY_S,          /* The app is currently unable to service request */
  RDM_RESTRICTED_S,        /* The request is made in the wrong way,
                              That is, function is call in ISR when
                              it should be called in task context.           */
  RDM_ASSIGN_STATUS_MAX_S  /* Last value indicator (must be last)            */
} rdm_assign_status_type;

/*---------------------------------------------------------------------------
  Type that defines the Services that can utilize a serial port
---------------------------------------------------------------------------*/
typedef enum
{
  RDM_NULL_SRVC = -1,        /* The NULL (no) service                   */
  RDM_FIRST_SRVC = 0,        /* The number for the first valid service.
                                It must be 0                            */
  RDM_DIAG_SRVC = RDM_FIRST_SRVC,
                             /* DIAG Task                               */
  RDM_DATA_SRVC,             /* Data Service Task                       */
  RDM_BT_HCI_SRVC,           /* Bluetooth HCI test service                          */
  RDM_BT_SOC_SRVC,           /* Bluetooth external SoC service          */
  RDM_MMC_SRVC,              /* MMC over USB                            */
  RDM_NMEA_SRVC,             /* NMEA service                            */
  RDM_RPC_SRVC,              /* ONCRPC task                             */
  RDM_DATA_WWAN_SRVC,        /* Second DATA service for WWAN            */
  RDM_LAST_KNOWN_SRVC = RDM_DATA_WWAN_SRVC,
  RDM_BREW_SRVC,
  RDM_BREW_EXTENDED_SRVC,    /* BREW extended device service            */
#ifdef srIMON
#error code not present
#endif
  RDM_SRVC_MAX = RDM_LAST_KNOWN_SRVC + 10
                             /* Last value indicator (must be last)
                                Allow 10 spare slots after the last
                                known service. The OEMs can insert a 
                                static service in service enum in the 
                                future. The new static service should 
                                work at it is without recompiling the 
                                DMSS code.                              */

} rdm_service_enum_type;

/*---------------------------------------------------------------------------
  Type that defines the serial port devices that can be used by any one of
  several service tasks

  Note
  device_to_port_id_table and device_to_str_table in rdevmap.c depends
  on the sequence of this enum type. Remember to update the two table
  when changing this type.
---------------------------------------------------------------------------*/
typedef enum
{
  RDM_SRVC_NOT_ALLOWED  = -1,/* Disallowed SRVC/DEV pair                */
  RDM_FIRST_DEV = 0,         /* First index for device array            */
  RDM_NULL_DEV          = RDM_FIRST_DEV, 
                             /* The NULL device, null device means 
                                a service does not have a devic opened  */
  RDM_UART1_DEV,             /* Main UART device                        */
  RDM_UART2_DEV,             /* AUX UART device                         */
  RDM_UART3_DEV,             /* Third UART device                       */
  RDM_USB_MDM_DEV,           /* USB CDC device                          */
  RDM_USB_CDC_DEV = RDM_USB_MDM_DEV,
  RDM_USB_MMC_DEV,           /* MMC over USB device                     */
  RDM_USB_SER1_DEV,          /* DIAG over USB device                    */
  RDM_USB_DIAG_DEV = RDM_USB_SER1_DEV,
  RDM_BT_NA_DEV,             /* BT device for Network Access App via DS */
  RDM_SMEM_DS_DEV,           /* Shared memory data service              */
  RDM_LSM_CSD_DEV,           /* LSM device for DS - LSM to SMLC via CSD */
  RDM_USB_SER2_DEV,          /* USB NMEA device                         */
  RDM_USB_NMEA_DEV = RDM_USB_SER2_DEV,
  RDM_BT_SPP_DEV,            /* BT device for SPP                       */
  RDM_USB_NET_WWAN_DEV,      /* USN NETowork WWAN device                */
  RDM_SMD_DS_DEV,            /* Shared memory data service              */
  RDM_SMD_DATA1_DEV,         /* MultiPort Data Service, port 1          */
  RDM_SMD_DATA2_DEV,         /* MultiPort Data Service, port 2          */
  RDM_SMD_DATA3_DEV,         /* MultiPort Data Service, port 3          */
  RDM_SMD_DATA4_DEV,         /* MultiPort Data Service, port 4          */
  RDM_SMD_DATA11_DEV,        /* MultiPort Data Service, port 11 (QVP data) */
  RDM_APS_SERIAL_VSP_DEV,    /* APS serial server on a virtual serial port */
  RDM_SMD_NMEA_DEV,          /* RDM NMEA port                           */
  RDM_USB_BREW_EXTENDED_DATA_DEV,         
  RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV,
  RDM_PLACE_HOLDER,          /* Single place holder to adj. for 0 start */
  RDM_DEV_MAX                /* Last value indicator (must be last)     */
} rdm_device_enum_type;

/*---------------------------------------------------------------------------
  Command type directives
---------------------------------------------------------------------------*/
typedef enum
{
  RDM_CLOSE_PORT,                 /* Close a serial port */
  RDM_OPEN_PORT                   /* Open a serial port  */
} rdm_command_enum_type;

/*---------------------------------------------------------------------------
  Type used by rdm_get_valid_ports() and used by the UI.
  A rdm_service_display_type is used for each device type in 
  rdm_device_enum_type for a single service type in rdm_service_enum_type.
  Then the UI can determine what devices are currently allowed mapping
  options for a given service. Also if the service is currently mapped
  to one of the devices. This allows a check mark to be displayed.
---------------------------------------------------------------------------*/
typedef struct
{
  rdm_device_enum_type  device;  /* a particular device */
  boolean               display; /* should the UI display this device */
  boolean               current; /* is this device currently mapped */
} rdm_service_display_type;

/*---------------------------------------------------------------------------
  Type of callback function to inform port assignment status
---------------------------------------------------------------------------*/
typedef void (*rdm_assign_port_cb_type)
(
  /* the assign port status                     */
  rdm_assign_status_type,
 
  /* the service asking for the assignment      */
  rdm_service_enum_type,

  /* the desired port for the service,
     If it is RDM_NULL_DEV, the service has 
     asked for closing the port                 */
  rdm_device_enum_type
);

/*---------------------------------------------------------------------------
  These are the function pointer types which will be used for registering
  the Services Open and Close routines.
--------------------------------------------------------------------------*/
typedef void (*rdm_service_close_func_ptr_type)(void);
typedef void (*rdm_service_open_func_ptr_type)(sio_port_id_type);

/*---------------------------------------------------------------------------
  The function pointer type which will be used for registering
  the Services Open and Close routines for multiple device open and close.
--------------------------------------------------------------------------*/
typedef void (*rdm_srv_func_ptr_multi_dev_type)(
  /* port to open or close              */
  sio_port_id_type      port,

  /* a user data, service register it to the RDM with open/close function
     pointer. RDM pass this pointer back to the service when calling
     open or closing function.
  */
  void *                data
);

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION RDM_INIT                                     EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes the Runtime Device Mapper Services. This function must
  be called before other functions of this module.

DEPENDENCIES
  Should be called just once.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rdm_init (void); 


/*===========================================================================

FUNCTION  RDM_GET_PREV_PORTS                            EXTERNALIZED FUNCTION

DESCRIPTION
DESCRIPTION
  Read in the port mappings last know by RDM from EFS. 
  
  It is here that the compile time defaults for port mapping may be 
  overwritten by what is read from EFS at power up.

DEPENDENCIES
  FEATURE_EFS is defined.

RETURN VALUE
  None

SIDE EFFECTS
  Port mappings prior to reset will be restored.
===========================================================================*/
void rdm_get_prev_ports(void);

/*===========================================================================

FUNCTION  RDM_ISSUE_OPENS                               EXTERNALIZED FUNCTION

DESCRIPTION
  RDM issues opens to SRVCs at the power up time based on last mapping 
  before power down.

  Services using the RDM should not just open a port when the MSM boots.
  RDM will call the registered open routines for the SRVC's if they should
  open a port at boot.

  If EFS is being used, whatever the previous mapping was will be reinstated.
  If there was no previous mapping, DS will open UART1, and DIAG will open
  UART2 if it is defined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If EFS is defined, the last valid port mapping prior to reset is 
  re-instated
===========================================================================*/
void rdm_issue_opens(void);
/*~ FUNCTION rdm_issue_opens */

#ifdef FEATURE_USB_ON_APPS_PROC
/*===========================================================================

FUNCTION usbotg_turn_on_modem_isr_control               EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets modem usb ISR and calls USB clock disable.
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void usbotg_turn_on_modem_isr_control(void);
#endif /*FEATURE_USB_ON_APPS_PROC*/

/*===========================================================================

FUNCTION RDM_ASSIGN_PORT                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function initiates the DEVMAP state machine to change the current 
  serial port configuration to give the specified serial port to the 
  specified Service task. This function will perform checks to ensure that 
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using. 
  Please use rdm_close_device if you want to specify which port to close.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean 
    True : if the mapping is valid and will be attempted.
           cbptr is called immediatly before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status 
           otherwise.
    False: if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.

SIDE EFFECTS
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_assign_port
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of 
     the assign action                                  */  
  rdm_assign_port_cb_type       cbptr
);

/*~ FUNCTION rdm_assign_port */

/*===========================================================================

FUNCTION RDM_ASSIGN_PORT_TMP                            EXTERNALIZED FUNCTION

DESCRIPTION
  Same function as rdm_assign_port(), except the change is not recorded in 
  NV.  Thus, upon the nex reboot, the setting is resetted to last known 
  setting.
  
DEPENDENCIES
  None

SIDE EFFECTS
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_assign_port_tmp
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of 
     the assign action                                  */  
  rdm_assign_port_cb_type       cbptr
);

/*~ FUNCTION rdm_assign_port_tmp */


/*===========================================================================

FUNCTION rdm_close_device                               EXTERNALIZED FUNCTION

DESCRIPTION
  This function will close a device used by a service and map the device
  to NULL service.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean 
    True : the closing action is made
    False: If the device or service is not valid or
           If the service is not using the device or
           If the function is call in an ISR

SIDE EFFECTS
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_close_device
(
  /* Service Task request closing of the device         */
  rdm_service_enum_type         service,

  /* Device/port to be closed                           */
  rdm_device_enum_type          device,

  /* Optional callback function to inform the result of
     the closing action                                 */  
  rdm_assign_port_cb_type       cbptr
);
/*~ FUNCTION rdm_close_device */

/*===========================================================================

FUNCTION RDM_NOTIFY

DESCRIPTION
  This function will notify to the DEVMAP state machine the specified result
  of a requested serial port change. This function performs a portion of the
  DEVMAP state machine.

  This function can be re-entrant

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rdm_notify
(
  rdm_service_enum_type  service,    /* Service Task desiring a new port */
  rdm_assign_status_type status      /* status of requested task change */
);
/*~ FUNCTION rdm_notify */

/*===========================================================================

FUNCTION RDM_GET_DEVICE                                 EXTERNALIZED FUNCTION

DESCRIPTION
  Query the Runtime Device Mapper for the port/device currently being used
  by the specified application/service.

DEPENDENCIES
  None

RETURN VALUE
  rdm_device_enum_type : the device which the service holds.

SIDE EFFECTS
  None

===========================================================================*/
rdm_device_enum_type rdm_get_device
(
  rdm_service_enum_type service
                         /* What Service to determine what  */
                         /* Device (if any) it has open */
);
/*~ FUNCTION rdm_get_device */

/*===========================================================================

FUNCTION RDM_GET_MULTI_DEV                              EXTERNALIZED FUNCTION

DESCRIPTION
  Query the Runtime Device Mapper for the port/devices currently being used
  by the specified application/service.

  It returns a device mask for the devices the service holds. The caller can
  test if the service is using a dev (for example, RDM_USB_DIAG_DEV) by
  if (dev_mask & (1 << RDM_USB_DIAG_DEV)).

DEPENDENCIES
  None

RETURN VALUE
  device mask.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rdm_get_multi_dev
(
  /* Service to query what devices it may be using, if any */
  rdm_service_enum_type service 
);

/*===========================================================================

FUNCTION RDM_GET_SERVICE                                EXTERNALIZED FUNCTION

DESCRIPTION
  Query the Runtime Device Mapper for the application/service that is 
  currently holding to the specified port/device (if any).

DEPENDENCIES
  None

RETURN VALUE
  rdm_service_enum_type

SIDE EFFECTS
  None

===========================================================================*/
rdm_service_enum_type rdm_get_service
(
  /* Device to see what service is using it, if any */
  rdm_device_enum_type device 
);                       
/*~ FUNCTION rdm_get_service */

/*===========================================================================

FUNCTION  RDM_GET_VALID_PORTS                           EXTERNALIZED FUNCTION

DESCRIPTION
  Query the RDM for the list of valid ports for the specified service.

DEPENDENCIES
  The caller must have allocated space of RDM_DEV_MAX to the 
  dev_displays array.

RETURN VALUE
  Boolean TRUE if the service is valid, else FALSE.

SIDE EFFECTS
  None
===========================================================================*/
boolean rdm_get_valid_ports
(
  /* The service that getting the valid ports           */
  rdm_service_enum_type    service,

  /* array of rdm_service_display of length RDM_DEV_MAX */
  rdm_service_display_type devices[]
);
/*~ FUNCTION rdm_get_valid_ports */
/*~ PARAM INOUT devices ARRAY RDM_DEV_MAX */

/*===========================================================================

FUNCTION RDM_REGISTER_OPEN_FUNC                         EXTERNALIZED FUNCTION

DESCRIPTION
  Function for Service Tasks to register their open function. This open 
  function is what RDM calls to instruct a Service to open a specified port.

  The function is used in conjunction with rdm_register_close_func.

  The service can only hold one device at a time if it registers
  its open function by this function. If the service holds dev1 and requests
  RDM to map dev2 to it, RDM will take dev1 from the service and then assign
  dev2 to the service.

  Please use rdm_register_func_multi_dev if you want multi-dev support.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rdm_register_open_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_open_func_ptr_type open_func     /* the routine to be called */
);
/*~ FUNCTION rdm_register_open_func */

/*===========================================================================

FUNCTION RDM_REGISTER_CLOSE_FUNC                        EXTERNALIZED FUNCTION

DESCRIPTION
  Function for Service Tasks to register their close function. This close 
  function is what RDM calls to instruct a Service to close it's open port.

  The function is used in conjunction with rdm_register_open_func.

  By using this function, a service can have only one port open, hence
  no sio_port_id_type argument in the close function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rdm_register_close_func
(
  rdm_service_enum_type service,       /* Service registering the function */
  rdm_service_close_func_ptr_type  close_func  /* the routine to be called */
);
/*~ FUNCTION rdm_register_close_func */

/*===========================================================================

FUNCTION RDM_REGISTER_FUNC_MULTI_DEV                    EXTERNALIZED FUNCTION

DESCRIPTION
  Function for Service Tasks to register their open/close functions.  The 
  functions are what RDM calls to instruct a Service to open/close device.

  This function supports multiple ports functionality, ie a service can
  open more than one device at a time.

  It is newer than rdm_register_open_func and rdm_register_close_func, 
  which register the opening/closing function when RDM only supports one 
  port for one service. rdm_register_open_func and rdm_register_close_func 
  are maintained for back compatibility.

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE : the registration succeeds
    FALSE: the registration fails. The following reasons could cause failure.
           1) open or close function is NULL
           2) service is out of range

SIDE EFFECTS
  None
===========================================================================*/
boolean rdm_register_func_multi_dev
(
  /* the service who registers the open and close function      */
  rdm_service_enum_type                 service,

  /* function for opening device                                */
  rdm_srv_func_ptr_multi_dev_type       open_func,

  /* function for closing device                                */
  rdm_srv_func_ptr_multi_dev_type       close_func,

  /* user data, the pointer will be passed back to the service  
     It can be set to NULL.                                     */
  void *                                data
);
/*~ FUNCTION rdm_register_func_multi_dev */

/*===========================================================================

FUNCTION RDM_GET_DEVICE_STR                             EXTERNALIZED FUNCTION

DESCRIPTION
  Query the RDM for the ASCII string that corresponds to the 
  rdm_device_enum_type specified.

  Note:This is used by the UI, and only 13 bytes of storage will be
  available in *str. One byte is for the NULL.

DEPENDENCIES
  The device must be in rdm_device_enum_type. The str has pre-allocated
  space of at least 13 chars including null-terminator.

RETURN VALUE
  Boolean TRUE if the device is valid, else FALSE.

SIDE EFFECTS
  The contents of pointer str will be written to. If the specified device
  is not valid, str is set to NULL.

===========================================================================*/
boolean rdm_get_device_str
(
  rdm_device_enum_type  device,            /* device to look up            */
  char                  *str               /* where to write the string    */
);
/*~ FUNCTION rdm_get_device_str */
/*~ PARAM OUT str STRING 13 */

/*===========================================================================

FUNCTION RDM_IS_SRV_HOLDING_DEV                         EXTERNALIZED FUNCTION

DESCRIPTION
  Test if a service is using a device.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE if the service is holding the device or
         if the service does not hold any device and the second
            parameter is RDM_NULL_DEV
    FALSE if the service is not holding the device or
          if the parameters are not right

SIDE EFFECTS
  None

===========================================================================*/
boolean rdm_is_srv_holding_dev
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
);
/*~ FUNCTION rdm_is_srv_holding_dev */

/*===========================================================================

FUNCTION rdm_set_srv_compat_mask                        EXTERNALIZED FUNCTION

DESCRIPTION
  Set device compatible mask for the service from 
  (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which 
  device this service can use.

  It provides an interface to dynamically set the compatiblity mask. Don't
  use this function for services of from RDM_DIAG_SRVC to 
  RDM_LAST_KNOWN_SRVC (RDM_RPC_SRVC). The device compatibility table 
  are set for those serivce at compile time.

  The caller can set dev_mask as this: if a service can use devices
  RDM_UART1_DEV and RDM_USB_DIAG_DEV.  The dev_mask is 
  (1 << RDM_NULL_DEV) | (1 << RDM_UART1_DEV) | (1 << RDM_USB_DIAG_DEV).
  RDM_NULL_DEV is here since RDM can always map NULL DEV to the service, 
  meaning closing the device of the service.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE successfully set
    FALSE if the service is incorrect.

SIDE EFFECTS
  None

===========================================================================*/
boolean rdm_set_srv_compat_mask
(
  rdm_service_enum_type service,        /* service to set       */
  uint32                dev_mask        /* dev mask to set      */
);
/*~ FUNCTION rdm_set_srv_compat_mask */

/*===========================================================================

FUNCTION RDM_IS_SRV_DEV_COMPAT                          EXTERNALIZED FUNCTION

DESCRIPTION
  Test if a service and a device is compatible. That is whether the service
  can use the device.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE if the service and the device is compatible, 
    FALSE if the service and the device is not compatible or
          if the parameters are not right

SIDE EFFECTS
  None

===========================================================================*/
boolean rdm_is_srv_dev_compat
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
);
/*~ FUNCTION rdm_is_srv_dev_compat */
#endif /* } FEATURE_RUNTIME_DEVMAP */

#endif /* RDEVMAP_H */

