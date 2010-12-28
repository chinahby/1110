#ifndef _HSU_CDC_FD_API_H_
#define _HSU_CDC_FD_API_H_

/*==============================================================================

    High Speed USB CDC Function Driver API (Shim layer)
                  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Wrappers to the stack's CDC FD API, since whenever a CDC API is called, the 
    giant mutex must be locked. 
  
  EXTERNALIZED FUNCTIONS
    hsu_cdc_fd_api_cdc_read
    hsu_cdc_fd_api_cdc_write
    hsu_cdc_fd_api_send_encap_response
    hsu_cdc_fd_api_cdc_abort_read
    hsu_cdc_fd_api_cdc_abort_write

  INITALIZATION AND SEQUENCING REQUIREMENTS
    The stack must be initialized before any function in this header file is used.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_cdc_fd_api.h#1 $
  $DateTime: 2008/02/04 22:58:53 $ $Author: lmanor $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  09/20/06  ke  Initial version
    
==============================================================================*/
#include "uw_device_cdc.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/
/*==============================================================================

FUNCTION      HSU_CDC_FD_API_CDC_READ

DESCRIPTION   
  Locks the giant mutex, calls the stack's cdc_read API, and releases the giant 
  mutex. 

DEPENDENCIES  
  None.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_cdc_fd_api_cdc_read
(
  cdc_handle_t  handle, 
  void *        buffer, 
  juint32_t     buffer_size
);

/*==============================================================================

FUNCTION      HSU_CDC_FD_API_CDC_WRITE

DESCRIPTION   
  Locks the giant mutex, calls the stack's cdc_write API, and releases the giant 
  mutex. 

DEPENDENCIES  
  None.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_cdc_fd_api_cdc_write
(
  cdc_handle_t  handle, 
  void *        buffer, 
  juint32_t     num_of_bytes_to_write
);

/*==============================================================================

FUNCTION      HSU_CDC_FD_API_SEND_ENCAP_RESPONSE

DESCRIPTION   
  Locks the giant mutex, calls the stack's cdc_send_encap_response API, and 
  releases the giant mutex. 

DEPENDENCIES  
  None.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_cdc_fd_api_send_encap_response
(
  cdc_handle_t  handle, 
  void *        response,
  juint32_t     num_of_bytes_to_send
);

/*==============================================================================

FUNCTION      HSU_CDC_FD_API_ABORT_READ

DESCRIPTION   
  Locks the giant mutex, calls the stack's cdc_abort_read API, and 
  releases the giant mutex. 

DEPENDENCIES  
  None.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_cdc_fd_api_cdc_abort_read
(
  cdc_handle_t handle
);

/*==============================================================================

FUNCTION      HSU_CDC_FD_API_ABORT_WRITE

DESCRIPTION   
  Locks the giant mutex, calls the stack's cdc_abort_write API, and 
  releases the giant mutex. 

DEPENDENCIES  
  None.

RETURN VALUE  
  HSU_JSUCCESS (0) for success, other values for failure.

SIDE EFFECTS  
  None.

==============================================================================*/
jresult_t hsu_cdc_fd_api_cdc_abort_write
(
  cdc_handle_t handle
);

#endif /* _HSU_CDC_FD_API_H_ */
