#ifndef HSU_HOST_API_I_H
#define HSU_HOST_API_I_H

/*===========================================================================

             
                 hsu_host_api_i.h

    DESCRIPTION :                                       
    High Speed USB HOST API internal data-structures and functions.
    
    Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
    
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: 
  $DateTime:

  
=============================================================================
when      who  what, where, why
--------  ---  --------------------------------------------------------------
28/06/06  sk   File created. 

---------------------------------------------------------------------------*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "usbhost_api.h"

#include "comdef.h"

/* Maximum number of devices that can be connected in host mode at a time */
#define USBHOST_MAX_DEVICES            10

/* Maximum number of clients currently supported */
#define USBHOST_MAX_CLIENTS            16

/*************************************************************************
**
** PRIMARY HID DEFINITIONS AND SUPPORT STRUCTURES
**
**************************************************************************/

/*************************************************************************
** Typedefs and Enumerations 
**************************************************************************/

/* Maximum number of device handles allowed by the host stack */
#define USBHOST_MAX_DEV_HANDLES         20

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

/* Structure for client */
typedef struct usbhost_client_data_s 
{
  /* Unique client id */
  usbhost_client_id_type client_id;

  /* Signal to be used for asynchronous IOCTL operations */
  uint32 completion_sig;

  /* Signal for dog rpt function */
  uint32 dog_rpt_sig;

  /* Pointer to the dog rpt function */
  void (*dog_rpt_fn_ptr)(void);

  /* Pointer to the tcb of the task that called usbhost_register
  ** (and was assigned a client id).
  */
  rex_tcb_type* registered_task;

  /* Type of device that is to be associated with the client entry */
  usbhost_pdev_type      pdev_type;

  /* device notification callback function pointer */
  void (* dev_notify_cb)
    (
      usbhost_pdev_type,
      usbhost_dev_status_type,
      usbhost_dev_instance_type,
      void *
    );

  /* Open notify callback function pointer */
  usbhost_open_ret_type (* open_notify_cb)
    (
      usbhost_dev_instance_type,
      usbhost_dev_handle_type,
      usbhost_open_status,
      char *,
      void *
    );

  /* Close request callback function pointer */
  void (* close_req_cb)
    (
      usbhost_dev_handle_type,
      char *,
      usbhost_client_id_type,
      void *
    );

  /* Client description */
  char client_desc[USBHOST_CLIENT_DESC_SIZE];

  /* Scratchpad for the client - this is passed back on each 
     callback invocation */
  void * param;
} usbhost_client_data_type;


/* Linked list structure for clients */
typedef struct usbhost_client_entry_s 
{
  boolean entry_is_empty;

  /* Pointer to data field (shared by entries in client pool
  ** and in the pending client pool.
  */
  usbhost_client_data_type* data;

  /* Pointer to next client entry in the linked list */
  struct usbhost_client_entry_s * next;
} usbhost_client_entry_type;

/* Structure to maintain device handles */
typedef struct usbhost_dev_handle_entry_s
{
  /* the allocated device handle */
  usbhost_dev_handle_type         dev_hndl;

  /* pointer to the client (data) that owns the handle */
  usbhost_client_data_type *      client_data;
} usbhost_dev_handle_entry_type;

/* Structure for a device entry */
typedef struct usbhost_dev_entry_type_s 
{
  /* Indicates whether this is an available entry in the device table */
  boolean entry_is_empty;

  /* device instance identifier */
  usbhost_dev_instance_type       dev_inst_id;

  /* Block or Char device */  
  usbhost_dev_type                dev_type;

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */

  /* Highest handle used */
  uint32                          dev_hndl_max;

  /* Device handle table */
  usbhost_dev_handle_entry_type   dev_hndl_tbl[USBHOST_MAX_DEV_HANDLES];

  /* Current open device handle */
  usbhost_dev_handle_type         curr_open_hndl;

  /* Physical device type (mouse, keyboard, hub etc.) */
  usbhost_pdev_type               pdev_type; 

  /* Pointer to the current client  */
  usbhost_client_entry_type *     curr_client; 

  /* Sync access to the device */
  rex_crit_sect_type              device_lock;

  /* Set by a class-specific adaptation layer, in the call to
  ** usbhost_add_device().
  */
  void*                           class_specific_data;
} usbhost_dev_entry_type;

typedef usbhost_status_type 
(*usbhost_ioctl_func_type)(/* Information regarding the device */
                           const usbhost_dev_entry_type* dev_entry_ptr,
                           /* Device specific command */
                           usbhost_ioctl_cmd_type command,
                           /* IOCTL specific parameter
                           ** passed from the application
                           */
                           void * param);

typedef void* (*usbhost_get_usbd_handle_func_type)(
                                  const usbhost_dev_entry_type* dev_entry_ptr);

#define USBHOST_GLOBAL_LOCK_INIT() rex_init_crit_sect (&usbhost_crit_sect)
#define USBHOST_GLOBAL_LOCK() rex_enter_crit_sect (&usbhost_crit_sect)
#define USBHOST_GLOBAL_UNLOCK() rex_leave_crit_sect(&usbhost_crit_sect)

/*===========================================================================

FUNCTION usbhost_set_ioctl_func

DESCRIPTION
This function sets the function pointer to the function
that handles IOCTLs for a specific device types. This function
should be called by the adaptation layer that handles each device type.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void usbhost_set_ioctl_func
(
 /* Physical device type (e.g., mass storage) */
 usbhost_pdev_type pdev_type, 
 /* Pointer to a function that handles IOCTL commands
 ** for devices of the specified type.
 */
 usbhost_ioctl_func_type func
);

/*===========================================================================

FUNCTION usbhost_set_usbd_handle_func

DESCRIPTION
This function sets the function pointer to the function
that retrieves the usbd handle for a device.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void usbhost_set_usbd_handle_func
(
  /* Physical device type (e.g., mass storage) */
  usbhost_pdev_type pdev_type, 
  /* Pointer to a function */
  usbhost_get_usbd_handle_func_type func
);

/*===========================================================================

FUNCTION usbhost_add_device

DESCRIPTION
This function adds an entry to the device table. It is called by each 
adaptation layer that receives a notification of a new device from the stack.

DEPENDENCIES
None.

RETURN VALUE
If successful, a pointer to the new entry. Otherwise, NULL.

SIDE EFFECTS
None.
===========================================================================*/
usbhost_dev_entry_type* usbhost_add_device
(
  usbhost_dev_type  dev_type, 
  usbhost_pdev_type pdev_type, 
#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif /* FEATURE_BT_EXTPF_HID_HOST */
  void*             class_specific_data
);

/*===========================================================================

FUNCTION usbhost_remove_device

DESCRIPTION
This function removes an entry from the device table. It is called by each 
adaptation layer that receives a notification (from the stack) of the removal
of a device.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void usbhost_remove_device
(
  usbhost_dev_entry_type* dev_entry_ptr
);

#endif /* HSU_HOST_API_I_H */

