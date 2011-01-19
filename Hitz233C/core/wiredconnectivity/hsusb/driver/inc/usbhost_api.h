/* =========================================================================
                       U S B H O S T _ A P I . H

  DESCRIPTION  :
    USB HOST API layer data structures and function prototypes.

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

    $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/usbhost_api.h#2 $
    $DateTime: 2008/03/17 01:32:32 $ $Author: ablay $

=============================================================================
when      who  what, where, why
--------  ---  --------------------------------------------------------------
10/27/06  rlp  Added support for data_report_id and data_unitExpo in the
                 data event structure.
10/20/06   sb  Feature added, APIs added for registration and de registration
                for notification on power violation.
09/07/06  rlp  HID V15 updates.
06/14/06  rlp  Added IOCTL support to get/set hid device idle rate.
               Added timestamp field to usbhost_hid_event_type.
06/12/06  sb   Added the function declaration for usbhost_api_init()
04/12/06  vs   task_table_type_s now has only sem_sig for internal host
                stack usage
04/13/06  rlp  Removed HID ifdef in PdevType. Always need access to these.
04/07/06  rlp  USBHOST_HID_USAGE_SLIDER is now a sampled data event.
04/06/06  rlp  Added matching keyboard usages from those omitted by TDI in
                 hid.h.
               Removed unused USBHOST_HID_USAGE_KEYBOARD.
               Enabled pdev_type USBHOST_PDEV_TYPE_HID_GAME_PAD.
02/23/06  rlp  Remove _POS from X_POS, Y_POS, Z_POS HID definitions.
02/22/06  rlp  Added new keyboard usage definitions.
02/07/06  rlp   Changed client_notify_cb param type from a device instance id
                  to a device handle.
02/06/06  rlp  Adding missing definition for USBHOST_HID_USAGE_SYSMENUSEL.
02/03/06  rlp  Added the following defines from usbhost_api.c to expose them
                 for client visibility:
                   USBHOST_INVALID_CLIENT_ID
                   USBHOST_INVALID_DEV_HNDL
                   USBHOST_CLIENT_DESC_SIZE
               Removed USBHOST_PDEV_TYPE_HID_GENERIC which is not used nor
                 required.
01/31/06  rlp  Fixed usage of device instance id versus a device handle.
               Added USBHOST_OPEN_NO_HANDLES, removed USBHOST_PDEV_TYPE_ALL.
               Added usbhost_ioctl_cmd_type.
01/25/06  rlp  HID related Lint changes prior to HID release.
01/17/06  rlp  Added HID support.  Performed minor code cleanup.
09/07/05   vs  Added struct usbhost_task_table_type
08/02/05   vs  File created.

---------------------------------------------------------------------------*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifndef USBHOST_API_H
#define USBHOST_API_H

#include "customer.h"

#if (defined(FEATURE_USB_HOST) || defined(FEATURE_HS_USB))

#include "assert.h"
#include "stdlib.h"
#include "string.h"
#include "rex.h"
#include "msg.h"
#include "qw.h"

#define USBHOST_INVALID_CLIENT_ID      0xffff
#define USBHOST_INVALID_DEV_HNDL       0
#define USBHOST_CLIENT_DESC_SIZE       32

/* Device instance for a device */
typedef uint32 usbhost_dev_instance_type;

/* Device handle for a device*/
typedef uint32 usbhost_dev_handle_type;

/* Client ID - uniue client ID assigned at registration time */
typedef uint32 usbhost_client_id_type;

/* usbhost_status_type - Return values from IOCTLS & HOST API functions */
typedef enum
{
  /* Ioctl / host API was successful */
  USBHOST_SUCCESS,
  /* Ioctl / Command was not successful */
  USBHOST_ERROR,
  /* Return value for mass storage specific CHECK_MEDIA IOCTL */
  USBHOST_ERROR_MEDIA_CHANGED,
  /* Return value for mass storage specific CHECK_MEDIA IOCTL */
  USBHOST_ERROR_MEDIA_NOT_PRESENT,
  /* Max type */
  USBHOST_MAX_STATUS
} usbhost_status_type;

/* usbhost_dev_status_type -
  used to indicate the notification type for devices */
typedef enum
{
  /* Device is inserted */
  USBHOST_DEV_CONNECTED,
  /* Devicve is plugged-out / removed */
  USBHOST_DEV_DISCONNECTED,
  /* Erroneous behaviour - not used currently */
  USBHOST_DEV_ERROR,
  /* Max type */
  USBHOST_MAX_DEV_STATUS
} usbhost_dev_status_type;

/* usbhost_dev_type - Class of devices */
typedef enum
{
  /* Character devices - asynchronous devices */
  USBHOST_DEV_TYPE_CHAR,
  /* Block devices     - synchronous devices */
  USBHOST_DEV_TYPE_BLOCK,
  /* Max type */
  USBHOST_MAX_DEV_TYPE
} usbhost_dev_type;

/* usbhost_pdev_type - Device types */
typedef enum
{
  /* Generic device class */
  USBHOST_PDEV_TYPE_HID_GENERIC,
  /* Mouse device class */
  USBHOST_PDEV_TYPE_HID_MOUSE,
  /* Keyboard device class */
  USBHOST_PDEV_TYPE_HID_KEYBOARD,
  /* Joystick device class */
  USBHOST_PDEV_TYPE_HID_JOYSTICK,
  /* Reserved for future HID device */
  USBHOST_PDEV_TYPE_HID_GAME_PAD,
  /* Reserved for future HID device */
  USBHOST_PDEV_TYPE_HID_RESERVED2,
  /* Hub */
  USBHOST_PDEV_TYPE_HUB,
  /* Mass storage device class */
  USBHOST_PDEV_TYPE_MASS_STORAGE,
  /* Printer device class */
  USBHOST_PDEV_TYPE_PRINTER,
  /* Still image device class */
  USBHOST_PDEV_TYPE_STILLIMG,
  /* Max type */
  USBHOST_MAX_PDEV_TYPE
} usbhost_pdev_type;

/* usbhost_open_ret_type - If a device is unavailable
   - the client can choose to try again or keep it's request */
typedef enum
{
  /* Keep the request queued with the API */
  USBHOST_OPEN_RET_RESCHEDULE,
  /* Removed the request from the API - the client will try again */
  USBHOST_OPEN_RET_UNSCHEDULE,
  /* Max type */
  USBHOST_MAX_OPEN_RET
} usbhost_open_ret_type;

/* usbhost_open_status - Status of device signaled from the API */
typedef enum
{
  /* Device is available for use */
  USBHOST_OPEN_AVAILABLE,
  /* Device is unavailable for use */
  USBHOST_OPEN_NOT_AVAILABLE,
  /* Device is out of handles */
  USBHOST_OPEN_NO_HANDLES,
  /* Max type */
  USBHOST_MAX_OPEN_STATUS
} usbhost_open_status;

/* usbhost_close_rsp_type - The client's response type when
   the API requests the client to close a communication path
   with the device */
typedef enum
{
  /* Release the device - the client will call usbhost_close subsequently */
  USBHOST_CLOSE_RSP_RELEASE,
  /* Retain the handle - client chooses not to relinquish control */
  USBHOST_CLOSE_RSP_RETAIN,
  /* Max type */
  USBHOST_MAX_CLOSE_RSP
}usbhost_close_rsp_type;

/* Struct for the client pass it's set of signals and the dog rpt function */
typedef struct usbhost_task_table_type_s
{
  /* Signal for semaphore */
  uint32 sem_wait_sig;

  /* Deprecated signal */
  uint32 wait_sig;

  /* Signal for dog rpt function */
  uint32 dog_rpt_sig;

  /* Pointer to the dog rpt function */
  void (*dog_rpt_fn_ptr)(void);
} usbhost_task_table_type;

/* usbhost_pwr_violation_type - informs a client application of a power violation */
typedef enum
{
  /* maximum power at enumeration exceeds phone power capacity */ 
   USBHOST_PWR_MAX_NOT_SUPPORTED = 0,
  /* a current violation was detected */
   USBHOST_MAX_PWR_EXCEEDED,
  /* Maximum value for type */
   USBHOST_MAX_PWR
} usbhost_pwr_violate_type;

/* usbhost_pwr_violate_notification_type - identifies device and type of power violation */
typedef struct usbhost_pwr_violate_notification_type_s
{
  /* Device product identifier */
  uint16                                 d_productID;  /* four digit BCD */
  /* Device vendor identifier */
  uint16                                 d_vendorID;   /* four digit BCD */
  /* Power violation */
  usbhost_pwr_violate_type                violate;
} usbhost_pwr_violate_notification_type;

/* Device IOCTL Enumerations */
typedef enum
{
  USBHOST_IOCTL_RESERVED = 0,

  /* Available for all connected devices.  Returns information
  ** pertaining to the device such a product ID, manufacturer ID
  ** vendor ID, etc.
  */
  USBHOST_IOCTL_GET_DEV_INFO,

#if (defined(FEATURE_USB_HOST_MASS_STORAGE) || defined(FEATURE_HS_USB))
  /* Mass Storage device commands */
  USBHOST_IOCTL_STOR_CHECK_MEDIA,
  USBHOST_IOCTL_STOR_STORAGE_TYPE,
  USBHOST_IOCTL_STOR_MEDIA_REMOVABLE,
  USBHOST_IOCTL_STOR_MEDIA_WRITE_PROTECT,
  USBHOST_IOCTL_STOR_BLOCK_SIZE,
  USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS,
  USBHOST_IOCTL_STOR_BLOCK_READ,
  USBHOST_IOCTL_STOR_BLOCK_WRITE,
#endif /* FEATURE_USB_HOST_MASS_STORAGE || FEATURE_HS_USB */

#if (defined(FEATURE_USB_HOST_HID) || defined(FEATURE_HS_USB))
  /* HID device commands */
  USBHOST_IOCTL_HID_SET_CONFIGURATION,
  USBHOST_IOCTL_HID_GET_CONFIGURATION,
  USBHOST_IOCTL_HID_READ_SAMPLED,
  USBHOST_IOCTL_HID_GET_ATTRIB,
  USBHOST_IOCTL_HID_SET_IDLE_RATE,
  USBHOST_IOCTL_HID_GET_IDLE_RATE,
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
#endif /* FEATURE_USB_HOST_HID */
} usbhost_ioctl_cmd_type;

/*======================================================================

FUNCTION usbhost_power_vbus

DESCRIPTION
    API to acquire / relinquish the power to VBUS
	boolean acquire : TRUE : acquire VBUS
	                  FALSE : relinquish VBUS
	int8 session_time : If acquire is TRUE - then this indicates the
	    time till which VBUS will be powered to detect a device.
		A value of -1 applies power indefinitely.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=======================================================================*/
void usbhost_power_vbus
(
  int8    session_time,
  boolean acquire
);



typedef void (*FUNCPTR_APPLN_ERR_NOTIFY) (usbhost_pwr_violate_notification_type *info);

/* Number of applcation that can be supported */
#define USBHOST_MAX_NUM_APPLN_REG_FOR_ERR_NOTIFY 10


/*======================================================================

FUNCTION usbhost_register_pwr_violate

DESCRIPTION
  Allows applications to register for error notification during 
  enumeration.

DEPENDENCIES
  Allows only USBHOST_MAX_NUM_APPLN_REG_FOR_ERR_NOTIFY to register.

RETURN VALUE
  handle to be used during un-registeration

SIDE EFFECTS
  None.

=======================================================================*/
usbhost_client_id_type usbhost_register_pwr_violate(
  void (* power_violate_notify_cb)   (usbhost_pwr_violate_notification_type *pwr_violate_info)
);

/*======================================================================

FUNCTION usbhost_deregister_pwr_violate

DESCRIPTION
  Allows applications to register for error notification during 
  enumeration.

DEPENDENCIES
  Allows only USBHOST_MAX_NUM_APPLN_REG_FOR_ERR_NOTIFY to register.

RETURN VALUE
  handle to be used during un-registeration

SIDE EFFECTS
  None.

=======================================================================*/
usbhost_status_type usbhost_deregister_pwr_violate(
  usbhost_client_id_type client_id
);


/*======================================================================

FUNCTION usbhost_notify_appln_pwr_violation

DESCRIPTION
  Notify the registered applciations about the pwr Violation.

DEPENDENCIES

RETURN VALUE
  Success/error

SIDE EFFECTS
  None.

=======================================================================*/
void usbhost_notify_appln_pwr_violation(
  usbhost_pwr_violate_notification_type *pwr_violate_info);

/*======================================================================

FUNCTION usbhost_register

DESCRIPTION
  Register function : the client uses this function to register with the API

  Actions performed  :

  1. add the client to the table

  2. Check for devices of the same type ; if there are any connected then
    call the callback function to notify the client of the presence of all
    devices of that type.

  Parameters :
    pdev_type - Device type for which the client is registering
    client_desc - A description string of the client
    dev_notify_cb - clients's callback function pointer used for notiying
      the cliensts of device instances.

DEPENDENCIES
  None.

RETURN VALUE
   Returns a unique client ID if registrations was successful, else
   returns an invalid client ID.
SIDE EFFECTS
  None.

=======================================================================*/
usbhost_client_id_type usbhost_register
(
  /* Device type */
  usbhost_pdev_type pdev_type,

  /* Client description string */
  const char * client_desc,

  /* Set of signals and dog rpt function specified by the client */
  usbhost_task_table_type client_sigs,

  /* Callback function pointer for device notifications*/
  void (* dev_notify_cb)
    /* Device type */
    (usbhost_pdev_type dev_type,
    /* Device status - connected / disconnected */
    usbhost_dev_status_type dev_stat,
    /* Device instance */
    usbhost_dev_instance_type dev_inst,
    /* Client scratchpad */
    void * param
    ),

  /* Void * pointer for client to specify a pointer */
  /* This pointer will be passed back on each callback function */
  void * param
);

/*======================================================================

FUNCTION usbhost_deregister

DESCRIPTION
  Dergister function : the client uses this function to deregister with the API

  Actions performed  :
  1. Check if client exists in the table
     If so, remove the client from the table ; free up memory
  2. If the client is in the pending queue for the device type
        it is deregistering for, then remove it.

  Parameters :
   client_id  - client ID for the client assigned at registration time
   pdev_type  - Device type for which the client is deregistering

DEPENDENCIES
  None.

RETURN VALUE
  USBHOST_SUCCESS - if deregistration was successful
  USBHOST_ERROR   - if deregistration was not succesful


SIDE EFFECTS
  None.

=======================================================================*/
usbhost_status_type usbhost_deregister
(
  /* Client ID */
  usbhost_client_id_type client_id,

  /* Device type */
  usbhost_pdev_type pdev_type
);

/*======================================================================

FUNCTION usbhost_req_open

DESCRIPTION
  After the client has successfully registered and has been notified
  of the the presence of a device of the same type, the client
  uses this function to open a communication path with the device

  Actions performed  :
  1. Check if it's a valid dev_hndl
  2. Check if the client has registered for the devic type
  3. If there is no other device then call the open_notify_fn ;
  4. If there is another client using the device then
               - remember that a client is waiting on a particular device
               - send close_notify to the current client

   Parameters :
    client id - client ID for the client assigned at registration time
    dev_inst  - Device instance for which the client is requesting to
                get a handle
    open_notify_cb - Callback function specified by the client. This is
               invoked by the API when it needs to notify the client
               of the device being available or not available.
    close_req_cb - Callback function specified by the client. This is
               invoked by the API when it requests the client to close
               a communication path (handle) with a device.

DEPENDENCIES
  None.

RETURN VALUE
  USBHOST_SUCCESS - if open was processed successfully
  USBHOST_ERROR   - if open was not processed succesfully


SIDE EFFECTS
  None.

=======================================================================*/
usbhost_status_type usbhost_req_open
(
  /* Client ID */
  usbhost_client_id_type client_id,

  /* Device Instance */
  usbhost_dev_instance_type dev_inst,

  /* Open callback function */
  usbhost_open_ret_type (* open_notify_cb)
    (
      /* Device instance for which the client is being notified */
      usbhost_dev_instance_type,

      /* Device handle allocated for the communication path */
      usbhost_dev_handle_type,

      /* status of notification - whether available or not */
      usbhost_open_status,

      /* If not available - the client description of the client
         which currently holds the handle and is not giving up the
         handle */
      char *,

      /* Client scratchpad */
      void * param
    ),

  /* Close callback function */
  void (* close_req_cb)
    (
      /* Device handle of the device the API is requesting the client to
      close */
      usbhost_dev_handle_type,

      /* The client description of the client which is wanting to gain
         access to the device */
      char *,

      /* The client ID of the client which is wanting to gain access
         to the device */
      usbhost_client_id_type,

      /* Client scratchpad */
       void * param
    )
);

/*======================================================================

FUNCTION usbhost_close

DESCRIPTION
  After the client has successfully registered and has a
  communication path open - it can choose to close the
  communcation path

  Actions performed  :
  1. Check if it's a valid client
     If so set the curr_client to NULL ;
  2. See if there is any existing client waiting for the device ;
     pick the first in queue and call it's open_notify function

  Parameters :
   client_id - client ID for the client assigned at registration time
   dev_hndl  - Device handle of the device the client is closing


DEPENDENCIES
  None.

RETURN VALUE
  USBHOST_SUCCESS - if close was successful
  USBHOST_ERROR   - if close was not succesful


SIDE EFFECTS
  None.

=======================================================================*/
usbhost_status_type usbhost_close
(
  /* Client ID */
  usbhost_client_id_type client_id,

  /* Device handle */
  usbhost_dev_handle_type dev_hndl
);

/*======================================================================

FUNCTION usbhost_ioctl

DESCRIPTION
  IOCTL function to communicate with the device

  Parameters :
   client_id - client ID for the client assigned at registration time
   dev_hndl  - Device handle
   command   - The device specific IOCTL command
   parm      - IOCTL specific paramater

DEPENDENCIES
  None.

RETURN VALUE
  USBHOST_SUCCESS - if ioctl was successful
  USBHOST_ERROR   - if ioctl was not succesful
  Other device specific return types

SIDE EFFECTS
  None.

=======================================================================*/
usbhost_status_type usbhost_ioctl
(
  /* Client ID */
  usbhost_client_id_type client_id,

  /* Device handle */
  usbhost_dev_handle_type dev_hndl,

  /* Device specific command */
  usbhost_ioctl_cmd_type command,

  /* IOCTL specific parameter */
  void * parm
);

/*======================================================================

FUNCTION usbhost_close_req_rsp

DESCRIPTION
  This function is called by the client to notify the API whether
  it is willing to relinquish control of the device or not.

  Parameters :
  client_id  - client ID for the client assigned at registration time
  client_id_close - client ID for the client requesing access to the
    device.
  dev_hndl   - Device handle
  response   - Client's response to the API whether to relinquish control
    of the device or not.

DEPENDENCIES

RETURN VALUE
  USBHOST_SUCCESS - if notification was valid
  USBHOST_ERROR   - if notification was invalid

SIDE EFFECTS
  None.

=======================================================================*/
usbhost_status_type usbhost_close_req_rsp
(
  /* Client ID */
  usbhost_client_id_type client_id,

  /* Client ID of the client requesting access to the device */
  usbhost_client_id_type client_id_close,

  /* Device handle */
  usbhost_dev_handle_type dev_hndl,

  /* Client's response to the API */
  usbhost_close_rsp_type response
);


/*======================================================================

FUNCTION usbhost_api_init

DESCRIPTION
    Initialized the data structures for the API. Called from tmc.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=======================================================================*/
void usbhost_api_init
(
  void
);

/* Device specific data structures */

typedef struct _usbhost_IoctlTransfer
{
  void  *buffer;
  uint32 length;
  uint32 startBlock;
  uint32 numberBlocks;
} usbhost_IoctlTransfer;


/* IOCTL structure to retreive device/connection information via the
** USBHOST_IOCTL_GET_DEV_INFO command.
*/
typedef struct _usbhost_IoctlDevInfo
{
  /* Device product identifier */
  uint16                    d_productID;

  /* Device vendor identifier */
  uint16                    d_vendorID;

  /* Manufacturer identifer */
  uint8                     d_manufacturerID;

  /* Device class/subclass/protocol */
  uint8                     d_class;
  uint8                     d_subclass;
  uint8                     d_protocol;
} usbhost_ioctl_devInfo_type;

typedef usbhost_ioctl_devInfo_type usbhost_IoctlDevinfo;


#if (defined(FEATURE_USB_HOST_HID) || defined(FEATURE_HS_USB))

/*************************************************************************
**
** PRIMARY HID DEFINITIONS AND SUPPORT STRUCTURES
**
**************************************************************************/


/*************************************************************************
** Defines
**************************************************************************/

/*
** Macro to check a data event report's data type
**
**   Sample usage:  check if the field data type is relative
**
**     U8 data_type;
**
**     data_type = USBHOST_HID_GetDataType(event->data_flag,
**                                         USBHOST_HID_DATATYPE_RELATIVE);
**
**     if (data_type == USBHOST_HID_DATATYPE_RELATIVE)
**     {
**       // process relative data
**     }
**
*/
#define USBHOST_HID_GetDataType(data_flag, data_type) HID_CheckTypeData(data_flag, data_type)

/*
** Macro to extract systems of units and unit exponents from a data report.
** (internally calls the Oxford API to retrieve the value).
**
** Sample usage:  get the system of units
**
**     U8 system_units;
**
**     system_units = USBHOST_HID_GetUnit(event->data_unit, USBHOST_HID_UNIT_SYSTEM);
**
**     switch (system_units)
**     {
**       case USBHOST_HID_SYSTEM_NONE:
**         // blah
**         break;
**       case USBHOST_HID_SYSTEM_SI_LINEAR:
**         // blah
**         break;
**       // etc
**     }
**
** Sample usage:  get the exponent of length in the system units
**
**     U8 length_expo;
**
**     length_expo = USBHOST_HID_GetUnit(event->data_unitExpo, USBHOST_HID_UNIT_LENGTH);
**
*/
#define USBHOST_HID_GetUnit(unit, index) HID_GetUnit(unit, index)


/* HID Unit Indices */
#define USBHOST_HID_UNIT_SYSTEM             0
#define USBHOST_HID_UNIT_LENGTH             1
#define USBHOST_HID_UNIT_MASS               2
#define USBHOST_HID_UNIT_TIME               3
#define USBHOST_HID_UNIT_TEMPERATURE        4
#define USBHOST_HID_UNIT_CURRENT            5
#define USBHOST_HID_UNIT_LUMINOUS_INTENSITY 6

/* HID Unit Systems */
#define USBHOST_HID_SYSTEM_NONE             0x0
#define USBHOST_HID_SYSTEM_SI_LINEAR        0x1
#define USBHOST_HID_SYSTEM_SI_ROTATION      0x2
#define USBHOST_HID_SYSTEM_ENGLISH_LINEAR   0x3
#define USBHOST_HID_SYSTEM_ENGLISH_ROTATION 0x4


/* HID Field Data Types */
#define USBHOST_HID_DATATYPE_DATA         0x00010000
#define	USBHOST_HID_DATATYPE_CONSTANT     0x00010001
#define	USBHOST_HID_DATATYPE_ARRAY        0x00020000
#define	USBHOST_HID_DATATYPE_VARIABLE     0x00020002
#define	USBHOST_HID_DATATYPE_ABSOLUTE     0x00040000
#define	USBHOST_HID_DATATYPE_RELATIVE     0x00040004
#define	USBHOST_HID_DATATYPE_NOWRAP       0x00080000
#define	USBHOST_HID_DATATYPE_WRAP         0x00080008
#define	USBHOST_HID_DATATYPE_LINEAR       0x00100000
#define	USBHOST_HID_DATATYPE_NONLINEAR    0x00100010
#define	USBHOST_HID_DATATYPE_PREFERED     0x00200000
#define	USBHOST_HID_DATATYPE_NONPREFERED  0x00200020
#define	USBHOST_HID_DATATYPE_NONULL       0x00400000
#define	USBHOST_HID_DATATYPE_NULL         0x00400040
#define	USBHOST_HID_DATATYPE_BITFIELD     0x01000000
#define	USBHOST_HID_DATATYPE_BUFFERED     0x01000100

#define USBHOST_HID_CAPS_LOCK_ON          0x0001
#define USBHOST_HID_SCROLL_LOCK_ON        0x0002
#define USBHOST_HID_NUM_LOCK_ON           0x0004
#define USBHOST_HID_SHIFT_KEY_ACTIVE      0x0008
#define USBHOST_HID_CTRL_KEY_ACTIVE       0x0010
#define USBHOST_HID_ALT_KEY_ACTIVE        0x0020
#define USBHOST_HID_GUI_KEY_ACTIVE        0x0040

/*************************************************************************
** Typedefs and Enumerations
**************************************************************************/

/* HID Usage Identifiers */
typedef enum
{
  USBHOST_HID_USAGE_NONE = 0,
  USBHOST_HID_USAGE_BUTTON,
  USBHOST_HID_USAGE_DIAL,
  USBHOST_HID_USAGE_WHEEL,
  USBHOST_HID_USAGE_HATSWITCH,
  USBHOST_HID_USAGE_BYTECOUNTER,
  USBHOST_HID_USAGE_MOTIONWAKEUP,
  USBHOST_HID_USAGE_START,
  USBHOST_HID_USAGE_SELECT,
  USBHOST_HID_USAGE_SYSCONTRL,
  USBHOST_HID_USAGE_SYSPOWERDOWN,
  USBHOST_HID_USAGE_SYSSLEEP,
  USBHOST_HID_USAGE_SYSWAKE,
  USBHOST_HID_USAGE_SYSCONMENU,
  USBHOST_HID_USAGE_SYSMAINMENU,
  USBHOST_HID_USAGE_SYSAPPMENU,
  USBHOST_HID_USAGE_SYSMENUHELP,
  USBHOST_HID_USAGE_SYSMENUEXIT,
  USBHOST_HID_USAGE_SYSMENUSEL,
  USBHOST_HID_USAGE_SYSMENURIGHT,
  USBHOST_HID_USAGE_SYSMENULEFT,
  USBHOST_HID_USAGE_SYSMENUUP,
  USBHOST_HID_USAGE_SYSMENUDOWN,
  USBHOST_HID_USAGE_DPADUP,
  USBHOST_HID_USAGE_DPADDOWN,
  USBHOST_HID_USAGE_DPADRIGHT,
  USBHOST_HID_USAGE_DPADLEFT,

  /* Sampled Data Usages Go Here - ** DO NOT ALTER POSITIONING */

  USBHOST_HID_USAGE_X,
  USBHOST_HID_USAGE_Y,
  USBHOST_HID_USAGE_Z,
  USBHOST_HID_USAGE_RX,
  USBHOST_HID_USAGE_RY,
  USBHOST_HID_USAGE_RZ,
  USBHOST_HID_USAGE_VX,
  USBHOST_HID_USAGE_VY,
  USBHOST_HID_USAGE_VZ,
  USBHOST_HID_USAGE_VBRX,
  USBHOST_HID_USAGE_VBRY,
  USBHOST_HID_USAGE_VBRZ,
  USBHOST_HID_USAGE_VNO,
  USBHOST_HID_USAGE_SLIDER,

  /* End of Sampled Data Usages */

  /* Keyboard device usages */
  USBHOST_HID_USAGE_KEY_A,
  USBHOST_HID_USAGE_KEY_B,
  USBHOST_HID_USAGE_KEY_C,
  USBHOST_HID_USAGE_KEY_D,
  USBHOST_HID_USAGE_KEY_E,
  USBHOST_HID_USAGE_KEY_F,
  USBHOST_HID_USAGE_KEY_G,
  USBHOST_HID_USAGE_KEY_H,
  USBHOST_HID_USAGE_KEY_I,
  USBHOST_HID_USAGE_KEY_J,
  USBHOST_HID_USAGE_KEY_K,
  USBHOST_HID_USAGE_KEY_L,
  USBHOST_HID_USAGE_KEY_M,
  USBHOST_HID_USAGE_KEY_N,
  USBHOST_HID_USAGE_KEY_O,
  USBHOST_HID_USAGE_KEY_P,
  USBHOST_HID_USAGE_KEY_Q,
  USBHOST_HID_USAGE_KEY_R,
  USBHOST_HID_USAGE_KEY_S,
  USBHOST_HID_USAGE_KEY_T,
  USBHOST_HID_USAGE_KEY_U,
  USBHOST_HID_USAGE_KEY_V,
  USBHOST_HID_USAGE_KEY_W,
  USBHOST_HID_USAGE_KEY_X,
  USBHOST_HID_USAGE_KEY_Y,
  USBHOST_HID_USAGE_KEY_Z,
  USBHOST_HID_USAGE_KEY_1,
  USBHOST_HID_USAGE_KEY_2,
  USBHOST_HID_USAGE_KEY_3,
  USBHOST_HID_USAGE_KEY_4,
  USBHOST_HID_USAGE_KEY_5,
  USBHOST_HID_USAGE_KEY_6,
  USBHOST_HID_USAGE_KEY_7,
  USBHOST_HID_USAGE_KEY_8,
  USBHOST_HID_USAGE_KEY_9,
  USBHOST_HID_USAGE_KEY_0,
  USBHOST_HID_USAGE_KEY_ENTER,
  USBHOST_HID_USAGE_KEY_ESCAPE,
  USBHOST_HID_USAGE_KEY_BACKSPACE,
  USBHOST_HID_USAGE_KEY_TAB,
  USBHOST_HID_USAGE_KEY_SPACE,
  USBHOST_HID_USAGE_KEY_MINUS,
  USBHOST_HID_USAGE_KEY_EQUAL,
  USBHOST_HID_USAGE_KEY_LEFT_BRACKET,
  USBHOST_HID_USAGE_KEY_RIGHT_BRACKET,
  USBHOST_HID_USAGE_KEY_BACKSLASH,
  USBHOST_HID_USAGE_KEY_SEMICOLON,
  USBHOST_HID_USAGE_KEY_QUOTE,
  USBHOST_HID_USAGE_KEY_GRAVE,
  USBHOST_HID_USAGE_KEY_COMMA,
  USBHOST_HID_USAGE_KEY_PERIOD,
  USBHOST_HID_USAGE_KEY_FORWARD_SLASH,
  USBHOST_HID_USAGE_KEY_CAPS_LOCK,
  USBHOST_HID_USAGE_KEY_F1,
  USBHOST_HID_USAGE_KEY_F2,
  USBHOST_HID_USAGE_KEY_F3,
  USBHOST_HID_USAGE_KEY_F4,
  USBHOST_HID_USAGE_KEY_F5,
  USBHOST_HID_USAGE_KEY_F6,
  USBHOST_HID_USAGE_KEY_F7,
  USBHOST_HID_USAGE_KEY_F8,
  USBHOST_HID_USAGE_KEY_F9,
  USBHOST_HID_USAGE_KEY_F10,
  USBHOST_HID_USAGE_KEY_F11,
  USBHOST_HID_USAGE_KEY_F12,
  USBHOST_HID_USAGE_KEY_PRINT_SCREEN,
  USBHOST_HID_USAGE_KEY_SCROLL_LOCK,
  USBHOST_HID_USAGE_KEY_PAUSE,
  USBHOST_HID_USAGE_KEY_INSERT,
  USBHOST_HID_USAGE_KEY_HOME,
  USBHOST_HID_USAGE_KEY_PAGE_UP,
  USBHOST_HID_USAGE_KEY_DELETE,
  USBHOST_HID_USAGE_KEY_END,
  USBHOST_HID_USAGE_KEY_PAGE_DOWN,
  USBHOST_HID_USAGE_KEY_RIGHT,
  USBHOST_HID_USAGE_KEY_LEFT,
  USBHOST_HID_USAGE_KEY_DOWN,
  USBHOST_HID_USAGE_KEY_UP,
  USBHOST_HID_USAGE_KEY_NUM_LOCK,
  USBHOST_HID_USAGE_KEY_PAD_DIVIDE,
  USBHOST_HID_USAGE_KEY_PAD_MULTIPLY,
  USBHOST_HID_USAGE_KEY_PAD_SUBTRACT,
  USBHOST_HID_USAGE_KEY_PAD_ADD,
  USBHOST_HID_USAGE_KEY_PAD_ENTER,
  USBHOST_HID_USAGE_KEY_PAD_1,
  USBHOST_HID_USAGE_KEY_PAD_2,
  USBHOST_HID_USAGE_KEY_PAD_3,
  USBHOST_HID_USAGE_KEY_PAD_4,
  USBHOST_HID_USAGE_KEY_PAD_5,
  USBHOST_HID_USAGE_KEY_PAD_6,
  USBHOST_HID_USAGE_KEY_PAD_7,
  USBHOST_HID_USAGE_KEY_PAD_8,
  USBHOST_HID_USAGE_KEY_PAD_9,
  USBHOST_HID_USAGE_KEY_PAD_0,
  USBHOST_HID_USAGE_KEY_PAD_DECIMAL,
  USBHOST_HID_USAGE_KEY_PAD_EQUAL,
  USBHOST_HID_USAGE_KEY_LEFT_CONTROL,
  USBHOST_HID_USAGE_KEY_LEFT_SHIFT,
  USBHOST_HID_USAGE_KEY_LEFT_ALT,
  USBHOST_HID_USAGE_KEY_LEFT_GUI,
  USBHOST_HID_USAGE_KEY_RIGHT_CONTROL,
  USBHOST_HID_USAGE_KEY_RIGHT_SHIFT,
  USBHOST_HID_USAGE_KEY_RIGHT_ALT,
  USBHOST_HID_USAGE_KEY_RIGHT_GUI,
  USBHOST_HID_USAGE_KEY_NON_US_BACKSLASH,
  USBHOST_HID_USAGE_KEY_APPL,
  USBHOST_HID_USAGE_KEY_POWER,

  /* LED device usages */
  USBHOST_HID_USAGE_LED_NUMLOCK,
  USBHOST_HID_USAGE_LED_CAPSLOCK,
  USBHOST_HID_USAGE_LED_SCROLLLOCK,

#if 0 /* Reserved for future device support and release */

  /* Simulation Device Usages */
  USBHOST_HID_USAGE_FLIGHT_CTRL_STICK,
  USBHOST_HID_USAGE_FLIGHT_CTRL,
  USBHOST_HID_USAGE_CYCLIC_CTRL,
  USBHOST_HID_USAGE_CYCLIC_TRIM,
  USBHOST_HID_USAGE_FLIGHT_YOKE,
  USBHOST_HID_USAGE_TRACK_CTRL,
  USBHOST_HID_USAGE_AILERON,
  USBHOST_HID_USAGE_AILERON_TRIM,
  USBHOST_HID_USAGE_ANTITORQUE_CTRL,
  USBHOST_HID_USAGE_AUTOPILOT_ENA,
  USBHOST_HID_USAGE_CHAFF_REL,
  USBHOST_HID_USAGE_COLLECTIVE_CTRL,
  USBHOST_HID_USAGE_DIVE_BRAKE,
  USBHOST_HID_USAGE_E_COUNTERMEASURE,
  USBHOST_HID_USAGE_ELEVATOR,
  USBHOST_HID_USAGE_ELEVATOR_TRIM,
  USBHOST_HID_USAGE_RUDDER,
  USBHOST_HID_USAGE_THROTTLE,
  USBHOST_HID_USAGE_FLIGHT_COMMU
  USBHOST_HID_USAGE_FLARE_REL,
  USBHOST_HID_USAGE_LANDING_GEAR,
  USBHOST_HID_USAGE_TOE_BRAKE,
  USBHOST_HID_USAGE_TRIGGER,
  USBHOST_HID_USAGE_WEAPON_ARM,
  USBHOST_HID_USAGE_WEAPON_SEL,
  USBHOST_HID_USAGE_WING_FLAPS,
  USBHOST_HID_USAGE_ACCELERATOR,
  USBHOST_HID_USAGE_BRAKE,
  USBHOST_HID_USAGE_CLUTCH,
  USBHOST_HID_USAGE_SHIFTER,
  USBHOST_HID_USAGE_STEERING,
  USBHOST_HID_USAGE_TURRET_DIR,
  USBHOST_HID_USAGE_BARREL_ELEVATION,
  USBHOST_HID_USAGE_DIVE_PLANE,
  USBHOST_HID_USAGE_BALLAST,
  USBHOST_HID_USAGE_BICYCLE_CRANK,
  USBHOST_HID_USAGE_HANDLE_BARS,
  USBHOST_HID_USAGE_FRONT_BRAKE,
  USBHOST_HID_USAGE_REAR_BRAKE,
  USBHOST_HID_USAGE_REAR_BRAKE,
#endif

  USBHOST_HID_USAGE_MAX
} usbhost_hid_usage_type;

/**********************************************************************
** Structures
***********************************************************************/

#define USBHOST_HID_CLIENT_CALLBACK_TYPE(HID_ClientCallBack) \
void HID_ClientCallback \
(\
	usbhost_dev_handle_type        dev_hndl,\
	usbhost_pdev_typ_type          pdev_type,\
	usbhost_hid_event_type *	   event\
);

/* The following structure is used as the data I/O parameter for the
** USBHOST_IOCTL_HID_READ_SAMPLED HID IOCTL command as well as the event
** structure used to send usage event to a client via the immediate
** client notification callback.
*/
typedef struct usbhost_hid_event_s
{
  /* Indicates which specific usage the event is for:
  ** e.g.
  **   USBHOST_HID_USAGE_BUTTON,
  **   USBHOST_HID_USAGE_DIAL,
  **   USBHOST_HID_USAGE_WHEEL,…
  **
  ** It is the responsibility of the client application to apply
  ** the appropriate context for each usage as per application
  ** requirements.
  */
  usbhost_hid_usage_type           usage;

  /* Sequence number for the last update.  Only applies to sampled
  ** data events.
  */
  uint32                           seq_num;

  /* Usage data value.
  ** For example, if the usage is USBHOST_HID_USAGE_BUTTON, this
  ** field will indicate the specific button pressed (e.g. 2)
  */
  int32                            data_val;

  /* Minimum data value */
  int32                            data_min;

  /* Maximum data value */
  int32                            data_max;

  /* Unit - indicates the usage units. Not defined at this time */
  uint32                           data_unit;

  /* Unit Exponent */
  uint32                           data_unitExpo;

  /* Data attribute flags - defines the usage type. Used by the client
  ** application as needed.  Consult [U1], [U2] for additional
  ** information. Bitmask field with the following values:
  **
  **  USBHOST_HID_DATATYPE_DATA
  **  USBHOST_HID_DATATYPE_CONSTANT
  **  USBHOST_HID_DATATYPE_ARRAY
  **  USBHOST_HID_DATATYPE_VARIABLE
  **  USBHOST_HID_DATATYPE_ABSOLUTE
  **  USBHOST_HID_DATATYPE_RELATIVE
  **  USBHOST_HID_DATATYPE_NOWRAP
  **  USBHOST_HID_DATATYPE_WRAP
  **  USBHOST_HID_DATATYPE_LINEAR
  **  USBHOST_HID_DATATYPE_NONLINEAR
  **  USBHOST_HID_DATATYPE_PREFERED
  **  USBHOST_HID_DATATYPE_NONPREFERED
  **  USBHOST_HID_DATATYPE_NONULL
  **  USBHOST_HID_DATATYPE_NULL
  **  USBHOST_HID_DATATYPE_BITFIELD
  **  USBHOST_HID_DATATYPE_BUFFERED
  */
  uint32                           data_flag;

  /* Data specific modifier if applicable.  Bit-masked field with the
  ** following values (when set (1) value indicates modifier is in use):
  **
  ** USBHOST_HID_CAPS_LOCK_ON       0x0001 : caps lock is ON
  ** USBHOST_HID_SCROLL_LOCK_ON     0x0002 : scroll lock is ON
  ** USBHOST_HID_NUM_LOCK_ON        0x0004 : num lock is ON
  ** USBHOST_HID_SHIFT_KEY_ACTIVE   0x0008 : a SHIFT key is active
  ** USBHOST_HID_CTRL_KEY_ACTIVE    0x0010 : a CTRL key is active
  ** USBHOST_HID_ALT_KEY_ACTIVE     0x0020 : a ALT key is active
  ** USBHOST_HID_GUI_KEY_ACTIVE     0x0040 : a GUI key is active
  */
  uint32                           data_modifier;

  /* Indicates the make or break status if applicable. For example, when a
  ** mouse button is held down 'make' will be enabled.  When it is released
  ** 'break' will be enabled.  'break' is always the default status and it is
  ** up to the client application to maintain proper state control.
  */
  uint32                           data_mkbrk;

  /* Timestamp of the event at the point the event is passed to the client */
  qword                            timestamp;

  /* Device data report identifier */
  uint32                           data_report_id;
} usbhost_hid_event_type;


/* The following structure is used as the data I/O parameter for the
** USBHOST_IOCTL_HID_GET_ATTRIB command.  This commands allows a client
** to retrieve HID device specific attributes such as the max number of
** supported devices, min/max x,y,z positions etc.
*/
typedef struct usbhost_hid_attrib_s
{
  /* Max number of supported buttons */
  uint16                           max_buttons;

  /* Min/Max X position values */
  int32                            min_x;
  int32                            max_x;

  /* Min/Max Y position values */
  int32                            min_y;
  int32                            max_y;

  /* Min/Max Z position values */
  int32                            min_z;
  int32                            max_z;
} usbhost_hid_attrib_type;

/* The following structure is used as the data I/O parameter for the
** USBHOST_IOCTL_HID_SET_CONFIGURATION and USBHOST_IOCTL_HID_GET_CONFIGURATION
** HID IOCTL commands.
*/
typedef struct usbhost_ioctl_hid_cfg_s
{
  /* For SET_CONFIGURATION client must populate client_notify_cb with
  ** an appropriate callback function. For GET_CONFIGURATION, HID driver
  ** will return the current callback function via client_notify_cb.
  */
  void (* client_notify_cb) (
          usbhost_dev_handle_type   dev_hndl,
	  usbhost_pdev_type         pdev_type,
	  usbhost_hid_event_type *  event
       );

} usbhost_ioctl_hid_cfg_type;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

#endif /* FEATURE_USB_HOST_HID || FEATURE_HS_USB */

#endif /* FEATURE_USB_HOST || FEATURE_HS_USB */

#endif /* USBHOST_API_H */






