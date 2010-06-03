/*==============================================================================

  High Speed USB Ethernet Networking control Model (CDC/ECM) Function Driver API
   (Shim layer)
  
  GENERAL DESCRIPTION
    Wrappers to the stack's CDC/ECM FD API, since whenever a CDCECM API is 
    called, the giant mutex must be locked. 

  EXTERNALIZED FUNCTIONS
    hsu_ecm_fd_api_notify_conn_speed_change
    
  INITALIZATION AND SEQUENCING REQUIREMENTS
    The stack must be initialized before any function in this header file is used.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_ecm_fd_api.c#2 $
  $DateTime: 2008/09/15 01:05:29 $ $Author: ablay $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  09/20/06  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_ecm_fd_api.h"
#include "hsu_common.h"
#include "port.h"

#include "uw_device_cdc_ecm.h"
#include "uw_device_cdc.h"
#include "jtypes.h"


/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION      HSU_ECM_FD_API_NOTIFY_CONN_SPEED_CHANGE

DESCRIPTION   
  Locks the giant mutex, calls the stack's ecm_notify_conn_speed_change API, and 
  releases the giant mutex. 

DEPENDENCIES  
  None.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_ecm_fd_api_notify_conn_speed_change
(
  cdc_handle_t        handle,
  ecm_conn_speed_t *  conn_speed
)
{
  jresult_t jresult;

  /* Acquire the Giant Mutex. */
  jsafe_enter();

  jresult = ecm_notify_conn_speed_change(handle, conn_speed);
  
  /* Release the Giant Mutex. */
  jsafe_leave();
  
  return jresult;
} /* hsu_ecm_fd_api_notify_conn_speed_change */
