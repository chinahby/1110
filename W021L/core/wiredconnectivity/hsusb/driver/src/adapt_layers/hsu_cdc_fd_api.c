/*==============================================================================

  High Speed USB CDC Function Driver API
   (Shim layer)
  
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
    None.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_cdc_fd_api.c#2 $
  $DateTime: 2008/09/15 01:05:29 $ $Author: ablay $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  09/20/06  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_cdc_fd_api.h"
#include "hsu_common.h"
#include "port.h"

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
)
{
  jresult_t jresult;

  /* Acquire the Giant Mutex. */
  jsafe_enter();

  jresult = cdc_read(handle, buffer, buffer_size);
  
  /* Release the Giant Mutex. */
  jsafe_leave();
  
  return jresult;
} /* hsu_cdc_fd_api_cdc_read */

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
)
{
  jresult_t jresult;

  /* Acquire the Giant Mutex. */
  jsafe_enter();

  jresult = cdc_write(handle, buffer, num_of_bytes_to_write);

  /* Release the Giant Mutex. */
  jsafe_leave();

  return jresult;
} /* hsu_cdc_fd_api_cdc_write */


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
)
{
  jresult_t jresult;

  /* Acquire the Giant Mutex. */
  jsafe_enter();

  jresult = cdc_send_encap_response(handle, response, num_of_bytes_to_send);

  /* Release the Giant Mutex. */
  jsafe_leave();

  return jresult;
} /* hsu_cdc_fd_api_send_encap_response */

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
)
{
  jresult_t cdc_abort_read_result;

  /* Grab giant mutex */
  jsafe_enter();

  cdc_abort_read_result = cdc_abort_read(handle);

  /* Release giant mutex */
  jsafe_leave();

  return cdc_abort_read_result;
} /* hsu_cdc_fd_api_cdc_abort_read */

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
)
{
  jresult_t cdc_abort_write_result;

  /* Grab giant mutex */
  jsafe_enter();

  cdc_abort_write_result = cdc_abort_write(handle);

  /* Release giant mutex */
  jsafe_leave();

  return cdc_abort_write_result; 
} /* hsu_cdc_fd_api_cdc_abort_write */
