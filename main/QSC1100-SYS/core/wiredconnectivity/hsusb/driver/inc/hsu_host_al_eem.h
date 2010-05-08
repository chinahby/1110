#ifndef _HSU_HOST_AL_EEM_H_
#define _HSU_HOST_AL_EEM_H_

/*==============================================================================

High Speed USB Host CDC/EEM Adaptation Layer 
H E A D E R   F I L E

GENERAL DESCRIPTION
Defines externalized data and functions for the EEM Adaptation Layer, for the
use of HS-USB stack & SIO 

EXTERNALIZED FUNCTIONS
hsu_host_al_eem_init
hsu_host_al_eem_data_open
hsu_host_al_eem_data_close
hsu_host_al_eem_data_transmit
hsu_host_al_eem_ioctl
hsu_host_al_eem_data_flush_tx

INITALIZATION AND SEQUENCING REQUIREMENTS
The HS-USB stack must be initialized and call hsu_host_al_eem_init, before all 
other functions in this header file can be used.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_host_al_eem.h#2 $
$DateTime: 2009/02/04 04:55:04 $ $Author: smalichi $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/19/08  ab  Initial version

==============================================================================*/
#include "sio.h"

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

FUNCTION      HSU_HOST_AL_EEM_INIT

DESCRIPTION   
This function initializes the adaptation layer. It is called by the 
HSUSB hsu_config_selector_init before the host stack is initialized. 

DEPENDENCIES  
This function should be called before any other function in this header is 
called.

RETURN VALUE  
TRUE for success, FALSE for failure.

SIDE EFFECTS  
None. 

==============================================================================*/
boolean hsu_host_al_eem_init(void);

/*==============================================================================

FUNCTION      HSU_HOST_AL_EEM_DATA_OPEN

DESCRIPTION   
Called by the application to allocate and configure the data plane for the 
EEM.

DEPENDENCIES  
hsu_host_al_eem_init() must have been called.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter

SIDE EFFECTS  
None.

==============================================================================*/
sio_status_type hsu_host_al_eem_data_open
(
 sio_open_type * open_ptr
);

/*==============================================================================

FUNCTION          HSU_AL_HOST_EEM_DATA_CLOSE

DESCRIPTION
This function closes the EEM data channel. All pending TX/RX will be canceled.
No TX flush is performed, so the function will return after the channel has 
been successfully closed, without any callback mechanism.

DEPENDENCIES
None. 

RETURN VALUE
None. 

SIDE EFFECTS
The RX and TX WMQs are emptied and pending TX/RX are canceled.

==============================================================================*/
#ifdef FEATURE_HSUSB_WRAPPER_FREE
void hsu_host_al_eem_data_close
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id, /* Not used */  
  void               (*close_func_ptr)(void) /* Not used */   
);
#else
void hsu_host_al_eem_data_close
( 
  sio_stream_id_type stream_id,
  void               (*close_func_ptr)(void) /* Not used */   
);
#endif /* FEATURE_HSUSB_WRAPPER_FREE */

/*==============================================================================

FUNCTION                  HSU_HOST_AL_EEM_DATA_TRANSMIT 

DESCRIPTION
Called by the application, when it wishes to send an Ethernet frame on the data
channel. The data is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer and then gives it 
to the USB stack. 

DEPENDENCIES
hsu_host_al_eem_init() must have been called.

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
void hsu_host_al_eem_data_transmit
( 
 sio_stream_id_type  stream_id,
 sio_port_id_type    port_id,
 dsm_item_type *     tx_ptr 
);

/*==============================================================================

FUNCTION      HSU_HOST_AL_EEM_IOCTL

DESCRIPTION   
This function is used to control various features of the adaptation layer.  
The behavior of this function depends on the cmd & params issued.
Supported IOCTLs:
- SIO_IOCTL_ENABLE_DTR_EVENT_EXT
- SIO_IOCTL_DISABLE_DTR_EVENT_EXT
- SIO_IOCTL_DTE_READY_ASSERTED
- SIO_IOCTL_INBOUND_FLOW_ENABLE
- SIO_IOCTL_INBOUND_FLOW_DISABLE

DEPENDENCIES  
hsu_host_al_eem_init() must have been called.
When the command is SIO_IOCTL_ENABLE_DTR_EVENT - 'param' cannot be NULL.

RETURN VALUE  
None.

SIDE EFFECTS  
Depends on the command issued.
==============================================================================*/
void hsu_host_al_eem_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
);

/*==============================================================================

FUNCTION                HSU_HOST_AL_EEM_DATA_FLUSH_TX 

DESCRIPTION
This function flushes the pending DSM chains from the TX WMQ. The function 
will return synchronously, when the flush operation has finished.
The EEM-AL will try to flash the Tx WMQ and pending Tx buffer, but if not 
finishes within a predefined time, it will simply cancel the pending Tx 
buffer, and return. 

DEPENDENCIES
hsu_host_al_eem_init() must have been called.

RETURN VALUE
None.

SIDE EFFECTS
The TX WMQ is empty and no pending TX/RX.

==============================================================================*/
void hsu_host_al_eem_data_flush_tx
( 
 sio_stream_id_type  stream_id,   
 sio_port_id_type    port_id,
 void                (*flush_func_ptr)(void)  
);

#endif /* _HSU_HOST_AL_EEM_H_ */
