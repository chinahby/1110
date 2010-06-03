#ifndef _PICTBRIDGE_API_H_
#define _PICTBRIDGE_API_H_

/*==============================================================================

  PictBridge API --- H E A D E R   F I L E
  
  GENERAL DESCRIPTION
  Contains data structures and API for DPS wrapper functions providing generic
  PictBridge API that does not depend on a particular implementation.
  
  EXTERNALIZED FUNCTIONS
  pictbridge_connect_printer
  pictbridge_disconnect_printer
  pictbridge_register_callback  

  pictbridge_configure_print_service

  pictbridge_get_capability
  pictbridge_get_job_status
  pictbridge_get_device_status

  pictbridge_start_job
  pictbridge_abort_job
  pictbridge_continue_job
      
  INITIALIZATION AND SEQUENCING REQUIREMENTS
  pictbridge_connect_printer must be called prior to usage of any other functions
  exposed by this API.
    
  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/pictbridge_api.h#1 $
    
    when     who  what, where, why
    -------- ---  ---------------------------------------------------------
    11/16/06 dhk  First revision of file    
==============================================================================*/

/*==============================================================================

  INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_dps_defs.h"

/*==============================================================================
Typedefs
==============================================================================*/


/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/


/*==============================================================================

FUNCTION pictbridge_connect_printer

DESCRIPTION
Initiate spoof disconnect and re-enumrate the device as a Still Image Capture
Device (SICD).  The process could take several seconds.

Send printer connected notification to the client via registered callback
function. 

DEPENDENCIES
None

RETURN VALUE

SIDE EFFECTS


==============================================================================*/
uint32 pictbridge_connect_printer(void);


/*==============================================================================
FUNCTION pictbridge_disconnect_printer

DESCRIPTION
Initiate spoof disconnect and restore the original enumeration.  
The process could take several seconds.

DEPENDENCIES
None

RETURN VALUE

SIDE EFFECTS


==============================================================================*/
uint32 pictbridge_disconnect_printer(void);


/*==============================================================================

FUNCTION pictbridge_connect_printer

DESCRIPTION
Give OEMs a chance to specify vendor name, product name, and serial number.

This function can be called before receiving the printer connected notification.
Exchaging DPS_ConfigurePrintService with the printer will be performed 
automatically after getting connected to the printer successfully.

In DPS term, after DPS discovery is finished, DPS_ConfigurePrintService will
be done automatically.  If pictbridge_connect_printer is not called until DPS
discovery is finished, the default vendor name, product name, and serial number
will be sent to the printer.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

==============================================================================*/
uint32 pictbridge_configure_print_service
(
    const char * vendor_name,
    const char * product_name,
    const char * serial_number
);


/* PictBridge dynamic composition switching operation status */
typedef enum
{
    PB_RDM_DONE,
    PB_RDM_NOT_ALLOWED,    
    PB_RDM_DEVMAP_BUSY,
    PB_RDM_APP_BUSY    
} pb_rdm_change_status_t;


typedef enum
{
    PICTBRIDGE_DPS_DISCOVERY,
    PICTBRIDGE_PRINTER_CFG,
    PICTBRIDGE_JOB_STUTUS,
    PICTBRIDGE_DEVICE_STATUS,
    PICTBRIDGE_GET_CAPABILITY,
    PICTBRIDGE_GET_JOB_STUTUS,
    PICTBRIDGE_GET_DEVICE_STATUS,
    PICTBRIDGE_RDM_CHANGE_STATUS,
    PICTBRIDGE_PRINTER_CONNECT,
    PICTBRIDGE_PRINTER_DISCONNECT
} pictbridge_notifiy_t;

typedef void (*pictbridge_notify_cb_t)(void * pUser, pictbridge_notifiy_t type, void * data);

uint32 pictbridge_register_callback
(
    void * pClientData,
    pictbridge_notify_cb_t pictbridge_cb
);

/* Very thin pass-through wrapper functions */
int pictbridge_start_job(hsu_dps_job_config_t *config, hsu_dps_job_info_t *info, int count);
int pictbridge_abort_job(uint32 abort_style);
int pictbridge_continue_job (void);

int pictbridge_get_capability(hsu_dps_capability_t cap_type, uint32 paper_size);
int pictbridge_get_job_status(void);
int pictbridge_get_device_status(void);

boolean pictbridge_handle_mode_change(pb_rdm_change_status_t status);

#endif /* _PICTBRIDGE_API_H_ */
