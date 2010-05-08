/*==============================================================================

HSU_POLLING.C


GENERAL DESCRIPTION
This file defines functions that are used for HS USB polling mode.


EXTERNALIZED FUNCTIONS
hsu_polling_init
hsu_polling_read
hsu_polling_write
hsu_polling_handle_interrupt
hsu_polling_is_active


INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_polling_init should be called to use other functions in the file.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
11/25/08  tk     Removed compiler warnings
10/28/08  tk     Initial version

==============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "boothw.h"
#include "hsu_common.h"
#include "hsu_polling.h"
#include "jusb_core_fd.h"
#include <string.h>

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS 

==============================================================================*/

/*==============================================================================
Local Constants and Macros
==============================================================================*/

#define hsu_polling_hwdog_kick() BOOTHW_KICK_WATCHDOG()

/*==============================================================================
  Forward Declarations
==============================================================================*/
  
static fd_desc_t *hsu_polling_null_get_pipe_config(context_t context);
static jresult_t hsu_polling_null_enable(context_t context);
static void hsu_polling_null_disable(context_t context);
static jresult_t hsu_polling_null_suspend(context_t context);
static jresult_t hsu_polling_null_resume(context_t context);
static jresult_t hsu_polling_null_uninit(context_t context);
  

/*==============================================================================
  Local Constants
==============================================================================*/
#define HSU_POLLING_NUM_OF_HANDLES 2

/*==============================================================================
  Local Typedefs
==============================================================================*/
/* object type for hsu polling module which contains all of the global variables 
   for hsu polling module */
typedef struct {
  jbool_t enabled;
} hsu_polling_type;
struct hsu_polling_handle_s;

/*==============================================================================
  Local Variables
==============================================================================*/
/*global variable for hsu_polling */
static hsu_polling_type hsu_polling = {
  FALSE
};

/* stub fd operation table */
static fd_ops_t hsu_polling_null = {
 hsu_polling_null_get_pipe_config,
 hsu_polling_null_enable,
 hsu_polling_null_disable,
 hsu_polling_null_suspend,
 hsu_polling_null_resume,
 hsu_polling_null_uninit
};

/* hsu polling handle pool */
static hsu_polling_handle_type hsu_polling_handles[HSU_POLLING_NUM_OF_HANDLES];


/*==============================================================================
  External Functions
==============================================================================*/
extern jbool_t hsu_os_bus_poll_interrupt(void);
extern void hsu_os_bus_handle_interrupt(void);
void hsu_os_bus_init_polling(juint8_t core_id);

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION hsu_polling_null_get_pipe_config
DESCRIPTION 
 The function does nothing. It is to make the core work in polling mode, 
 replacing the callbacks for normal mode fd.

DEPENDENCIES
 None
 
RETURN VALUE
 NULL
 
SIDE EFFECTS
 None
===========================================================================*/
static fd_desc_t *hsu_polling_null_get_pipe_config(context_t context)
{
  return NULL;
}

/*===========================================================================
FUNCTION hsu_polling_null_get_pipe_config
DESCRIPTION
  The function does nothing. It is to make the core work in polling mode, 
  replacing the ncall backs for normal mode fd.

DEPENDENCIES
 None
 
RETURN VALUE
 0
 
SIDE EFFECTS
 None
===========================================================================*/
static jresult_t hsu_polling_null_enable(context_t context)
{
  return 0;
}

/*===========================================================================
FUNCTION hsu_polling_null_get_pipe_config
DESCRIPTION
  The function does nothing. It is to make the core work in polling mode, 
  replacing the callbacks for normal mode fd.

DEPENDENCIES
 None
 
RETURN VALUE
 None
 
SIDE EFFECTS
 None
===========================================================================*/
static void hsu_polling_null_disable(context_t context)
{
  return;
}

/*===========================================================================
FUNCTION hsu_polling_null_get_pipe_config
DESCRIPTION
  The function does nothing. It is to make the core work in polling mode, 
  replacing the callbacks for normal mode fd.

DEPENDENCIES
 None
 
RETURN VALUE
 0
 
SIDE EFFECTS
 None
===========================================================================*/
static jresult_t hsu_polling_null_suspend(context_t context)
{
  return 0;
}

/*===========================================================================
FUNCTION hsu_polling_null_get_pipe_config
DESCRIPTION
  The function does nothing. It is to make the core work in polling mode, 
  replacing the callbacks for normal mode fd.

DEPENDENCIES
 None
 
RETURN VALUE
 0
 
SIDE EFFECTS
 None
===========================================================================*/
static jresult_t hsu_polling_null_resume(context_t context)
{
  return 0;
}

/*===========================================================================
FUNCTION hsu_polling_null_get_pipe_config
DESCRIPTION
  The function does nothing. It is to make the core work in polling mode, 
  replacing the callbacks for normal mode fd.

DEPENDENCIES
 None
 
RETURN VALUE
 0
 
SIDE EFFECTS
 None
===========================================================================*/
static jresult_t hsu_polling_null_uninit(context_t context)
{
  return 0;
}

/*===========================================================================

FUNCTION hsu_polling_rxcb

DESCRIPTION
This function is called by the core to notify the read status of core_read_data
function call.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_polling_rxcb(struct request_t * req)
{
  hsu_polling_handle_type *handle;
  
  handle = (hsu_polling_handle_type *)req->context;

  if(req->status == REQUEST_COMPLETED)
  {
    /* if read opearion is completed successfully, update the rx_state to 
       HSU_POLLING_COMPLETED */
    handle->rx_state = HSU_POLLING_COMPLETED;
    
    if( !handle->is_buff_pmem )
    {
      /* when PMEM is not used, copy internal buffer to read buffer */
      memcpy(req->caller_private, req->buffer.vaddr,req->bytes_transferred);
    }

    /* update rx_size with the length of recieved data */
    handle->rx_size = req->bytes_transferred;
  }
  else
  {
    /* all other case than REQUEST_COMPLETED belong to error condition. */
    handle->rx_state = HSU_POLLING_ERROR;
  }

  /* call rx callback function */
  if(handle->rxcb)
  {
    handle->rxcb(handle);
  }

  /* release the instance for req variable */
  core_request_free(req);
}

/*===========================================================================

FUNCTION hsu_polling_txcb

DESCRIPTION
This function is called by the core to notify the write status of 
core_send_data function call.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_polling_txcb(struct request_t * req)
{
  hsu_polling_handle_type *handle;
  
  handle = (hsu_polling_handle_type *)req->context;

  /* update rx_state */
  if(req->status == REQUEST_COMPLETED)
  {
    handle->tx_state = HSU_POLLING_COMPLETED;
    handle->tx_size = req->bytes_transferred;
  }
  else
  {
    handle->tx_state = HSU_POLLING_ERROR;
  }

  /* call tx callback */
  if(handle->txcb) 
  {
    handle->txcb(handle);
  }

  /* release the req */
  core_request_free( req );
}

/*===========================================================================

FUNCTION hsu_polling_start_read

DESCRIPTION
  This function do read-priming by calling core_read_data. hsu_polling_rxcb, 
  call back function will be called when read is completed by OUT tocken on 
  the assigned out pipe. 

DEPENDENCIES
  The paremeter handle is from hsu_polling_init. The buffer should be from PMEM 
  area if HSU_POLLING_BUFFER_DMA_ABLE is enabled. 

RETURN VALUE
  0 is success and others are failure.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_polling_start_read
(  
  hsu_polling_handle_type *handle, 
  void *buffer, 
  juint32_t size
)
{
  request_t *req = NULL;
  jresult_t rc =0;

  if( handle->is_buff_pmem )
  {
    req = core_request_alloc(size, buffer);
  }
  else
  {
    req = core_request_alloc(size, NULL);
  }
  
  if (!req)
  {
    return JENOMEM;
  }

  /* Save application pointer */
  req->caller_private = buffer;

  req->complete = hsu_polling_rxcb;
  req->context = handle;
  req->transfer_size = size;

  if( handle->out_pipe == NULL ) 
  {
    handle->out_pipe = core_get_pipe_desc( handle->core_ctx, 
                                           DIRECTION_OUT,
                                           handle->out_ep_num);
  }
    
  rc = core_read_data(handle->core_ctx, handle->out_pipe, req);
  if (rc)
  {
    core_request_free(req);
  }
  
  return rc;
}


/*===========================================================================

FUNCTION hsu_polling_start_write

DESCRIPTION
  This function start sending by calling core_send_data. The hsu_polling_txcb, 
  call back function will be called when sending is completed by IN tocken on 
  the assigned in pipe. 

DEPENDENCIES
  The paremeter handle is from hsu_polling_init. The buffer should be from PMEM 
  area if HSU_POLLING_BUFFER_DMA_ABLE is enabled. 

RETURN VALUE
  0 is success and others are failure.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_polling_start_write
(
  hsu_polling_handle_type *handle, 
  void *buffer, 
  juint32_t size
)
{
  request_t *req = NULL;
  jresult_t rc = 0;

  if( handle->is_buff_pmem )
  {
    req = core_request_alloc(size, buffer);
  }
  else
  {
    req = core_request_alloc(size, NULL);
  }

  if (!req)
  {
    return JENOMEM;
  }

  /* Save application pointer, and copy its buffer if necessary */
  req->caller_private = buffer;
  
  if( !handle->is_buff_pmem )
  {
    /* copy the buffer to allocated buffer */
    memcpy(req->buffer.vaddr, buffer, size);
  }
  
  req->zero = 1;
  req->complete = hsu_polling_txcb;
  req->context = handle;
  req->transfer_size = size;

  if( handle->in_pipe == NULL ) 
  {
    /* handle->in_pipe can be NULL when hsu_polling_init is called before USB 
       core comes to the configured state. */
    handle->in_pipe = core_get_pipe_desc(handle->core_ctx, 
                                         DIRECTION_IN,
                                         handle->in_ep_num);
  }

  /* write priming */
  rc = core_send_data(handle->core_ctx, handle->in_pipe, req);
  
  if (rc < 0)
  {
    core_request_free(req);
  }
  
  return rc;
}

/*===========================================================================

FUNCTION hsu_polling_init

DESCRIPTION
  The function initializes the polling mode. It disables the interrupt and task 
  switching and modifies core vairalbles for polling mode operation. It will 
  return the handle for other APIs from hsu_polling.

  core idx : the index for HS USB core. 
  in_ep_num : IN endpoint number in a bulk pipe. Range is  1 ~ 16.
  out_ep_num : OUT endpoint number in a bulk pipe, Range is 1 ~ 16.


DEPENDENCIES
None.

RETURN VALUE
hsu polling handle.

SIDE EFFECTS
  interrupt and task switching are disabled.
  
===========================================================================*/
hsu_polling_handle_type *hsu_polling_init
(
  juint8_t core_idx,
  juint8_t in_ep_num,
  juint8_t out_ep_num,
  hsu_polling_rx_callback rxcb,
  hsu_polling_tx_callback txcb,
  jbool_t is_buff_pmem
)
{
  int i;
  hsu_polling_handle_type *ret;
  hsu_polling_core_t *core_ctx;

  /* find valid handle and return the pointer with filling conent */
  for(i=0; i<HSU_POLLING_NUM_OF_HANDLES; i++)
  {
    if(!hsu_polling_handles[i].active)
    {
      /* disable task switching and interrupt */
      rex_int_lock();
      rex_task_lock();
      
      /* set enable flag */
      hsu_polling.enabled = TRUE;
        
      core_ctx = core_get_core_context(core_idx);
      ASSERT(core_ctx);

      /* make the current function driver call backs inacitve */
      core_set_fd_ops(core_ctx, &hsu_polling_null);


      /* fill the handle */
      ret = &hsu_polling_handles[i];
      ret->active = TRUE;
      ret->rx_state = HSU_POLLING_IDLE;
      ret->tx_state = HSU_POLLING_IDLE;
      ret->core_ctx = core_ctx;
      ret->in_ep_num = in_ep_num;
      ret->out_ep_num = out_ep_num;
      ret->is_core_configured = core_is_configured(core_ctx);
      if(ret->is_core_configured)
      {
        ret->in_pipe = core_get_pipe_desc(core_ctx,DIRECTION_IN,in_ep_num);
        ret->out_pipe = core_get_pipe_desc(core_ctx,DIRECTION_OUT,out_ep_num);
      }
      else
      {
        ret->in_pipe = NULL;
        ret->out_pipe = NULL;
        /* initialize hw for polling mode operation  */
        hsu_os_bus_init_polling(core_idx);
        /* make sure that the core is enabled. 7601 target needs it for it does
           not get the vbus connect event from link and miss a event to enable 
           the run bit */
        core_enable_by_idx(core_idx);
      }
      
      ret->rxcb = rxcb;
      ret->txcb = txcb;
      ret->is_buff_pmem = is_buff_pmem;
      return ret;
    }
  }

  return NULL;
}

/*===========================================================================

FUNCTION hsu_polling_handle_interrupt

DESCRIPTION
  The function reads the interupt status register and call the corresponding 
  interrupt handlers. It kicks hardware watch dog to prevent dog reset. 

DEPENDENCIES
  hsu_polling_init must have been called in advance. 

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_polling_handle_interrupt(void)
{
  while(hsu_os_bus_poll_interrupt())
  {
    hsu_polling_hwdog_kick();
    hsu_os_bus_handle_interrupt();
  }
  hsu_polling_hwdog_kick();
}

/*===========================================================================

FUNCTION hsu_polling_read

DESCRIPTION
  It receives the data from host. The buffer can be should be PMEM if 
  is_buff_pmem of parameter for hsu_polling_init has been set as TRUE. 
  The size variable  is the size of the buffer. The data in the buffer is only 
  valid when the return value is not 0. 

DEPENDENCIES
  hsu_polling_init must have been called in advance. 

RETURN VALUE
 the size of valid read.

SIDE EFFECTS
None.
===========================================================================*/
juint32_t hsu_polling_read
(
  hsu_polling_handle_type *handle, 
  void *buffer, 
  juint32_t size
)
{
  jresult_t result;

  if(!core_is_configured(handle->core_ctx)) 
  {
    /* Once the core state transits to non configured state, it mean USB lose 
       connection, pipe aborting is required and rx state needs to be reset to 
       IDLE state. */
    handle->rx_state = HSU_POLLING_IDLE;
    
    if(handle->is_core_configured)
    {
      core_abort_pipe(handle->core_ctx, handle->in_pipe);
      core_abort_pipe(handle->core_ctx, handle->out_pipe);
      handle->is_core_configured = FALSE;
    }
    
    return 0;
  }
  
  if(!handle->is_core_configured)
  {
    /* Being at this point means core is configured. */
    handle->is_core_configured = TRUE;
  }
  
  switch(handle->rx_state)
  {
    case HSU_POLLING_IDLE:
      /* be ready to read */
      result = hsu_polling_start_read(handle, buffer, size);

      if(result == 0)
      { 
       /* '0' value means 'there is no data to read at this point and be ready 
          for next read. The rx state transit to START state and wait for next 
          OUT token to fill the data with ZLP. ZLP will be handled by 
          core_read_data and total length will be managed by core.  */ 
          handle->rx_state = HSU_POLLING_START;
      }
      
      return 0;
      
    case HSU_POLLING_START:
      return 0;
      
    case HSU_POLLING_COMPLETED:
      /* Data arrived from host and the length of RX size is filled in 
         hsu_polling_rxcb */
      handle->rx_state = HSU_POLLING_IDLE;
      return handle->rx_size;
      
    default : ASSERT(0);
  }

  return 0;
}

/*===========================================================================

FUNCTION hsu_polling_write

DESCRIPTION
  The function send data to host. It only returns when the tranmission is 
  completed. The buffer can be should be PMEM if is_buff_pmem of parameter 
  for hsu_polling_init has been set as TRUE. The size variable  is the   size of  
  valid data. The return value is the size of data sent.

DEPENDENCIES
  hsu_polling_init must have been called in advance. 

RETURN VALUE
  The size of data sent. 

SIDE EFFECTS
None.
===========================================================================*/
juint32_t hsu_polling_write
(
  hsu_polling_handle_type *handle, 
  void *buffer, 
  juint32_t size
)
{
  if(!core_is_configured(handle->core_ctx)) 
  {
    handle->tx_state = HSU_POLLING_IDLE;
    
    if(handle->is_core_configured)
    {
      core_abort_pipe(handle->core_ctx, handle->in_pipe);
      core_abort_pipe(handle->core_ctx, handle->out_pipe);
      handle->is_core_configured = FALSE;
    }
    
    return 0;
  }
  
  if(!handle->is_core_configured)
  {
    handle->is_core_configured = TRUE;
  }
  
  handle->tx_state = HSU_POLLING_START;
  
  if( hsu_polling_start_write(handle, buffer, size) )
  { 
    /* if the return value of hsu_polling_start_write is not 0, the attempt for 
       this transer fails */
    return 0;
  }

  do {
    hsu_polling_handle_interrupt();
  } while(core_is_configured(handle->core_ctx) && 
          handle->tx_state == HSU_POLLING_START);
  
  if(handle->tx_state == HSU_POLLING_COMPLETED) 
     return handle->tx_size;

  handle->tx_state = HSU_POLLING_ERROR;
  
  return 0;
}

/*===========================================================================

FUNCTION hsu_polling_is_active

DESCRIPTION
  It tells if polling mode is enabled or not. The polling mode is enabled once 
  hsu_polling_init has been called. 

DEPENDENCIES
None.

RETURN VALUE
  TRUE : polling mode
  FALSE : normal mode. 

SIDE EFFECTS
None.
===========================================================================*/
jbool_t hsu_polling_is_active(void)
{
  return hsu_polling.enabled;
}

