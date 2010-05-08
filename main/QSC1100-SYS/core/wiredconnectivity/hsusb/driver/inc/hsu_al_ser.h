#ifndef _HSU_AL_SER_H_
#define _HSU_AL_SER_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API for the serial functions' adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_init
hsu_al_ser_open
hsu_al_ser_close
hsu_al_ser_transmit
hsu_al_ser_flush_tx
hsu_al_ser_ioctl
hsu_al_ser_handle_set_configuration
hsu_al_ser_handle_obex_uninit
hsu_al_ser_handle_acm_uninit


INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_init must be called prior to performing any other operations

Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_al_ser.h#7 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/08/06 esh  First revision of file    
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "sio.h"
#include "dsm.h"

#include "uw_device_cdc.h"
/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ser_init

DESCRIPTION
Initializes the data structures for later usage.

DEPENDENCIES
None

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise

SIDE EFFECTS


===========================================================================*/
boolean hsu_al_ser_init
(
  void
);

/*===========================================================================

FUNCTION hsu_al_ser_open

DESCRIPTION
This function opens a stream across a serial interface on the USB.  

DEPENDENCIES

RETURN VALUE
SIO_DONE_S:  SIO stream as specified in open_ptr has been successfully 
opened.
SIO_BADP_S:  Otherwise

SIDE EFFECTS
None

===========================================================================*/
sio_status_type hsu_al_ser_open
( 
  sio_open_type *open_ptr                 /* Configuration of stream */
);

/*===========================================================================

FUNCTION hsu_al_ser_close

DESCRIPTION
This procedure does all the book-keeping necessary to close a stream.  
Optional close callback can be passed to this function, if app wants
to be notified when this layer has done all the bookkeeping needed for close.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
close_done_cb will be called when this layer wants to signal to caller
that it has done all the bookkeeping needed for close.

===========================================================================*/
void hsu_al_ser_close
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  sio_port_id_type   port,                    /* Port ID                   */
  sio_vv_func_ptr_type close_done_cb          /* Function to call when AL has 
                                              completed all the bookkeeping 
                                              needed for close.            */
);

/*===========================================================================

FUNCTION hsu_al_ser_transmit

DESCRIPTION
This function will transmit a packet or if transmitter is busy, schedule
the transmission of a packet. Note that following this invocation, the caller
is no longer the owner of the packet and must not use it anymore.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_transmit
( 
  sio_stream_id_type stream_id,    /* Stream ID                            */
  sio_port_id_type      port_id,   /* SIO Port ID                          */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);

/*===========================================================================

FUNCTION hsu_al_ser_flush_tx

DESCRIPTION
This function shall signal the caller when the last packet to transfer has
successfully reached its destination, but only if the caller supplied a 
callback to be invoked when the above occurs.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
flush_tx_done_cb invoked when above occurs.

===========================================================================*/
void hsu_al_ser_flush_tx
( 
  sio_stream_id_type stream_id,              /* Stream ID                  */
  sio_port_id_type port_id,                  /* SIO Port ID                */
  sio_vv_func_ptr_type flush_tx_done_cb      /* Function to call when AL has 
                                             transmitted successfully all 
                                             pending data                  */
);

/*===========================================================================

FUNCTION hsu_al_ser_ioctl

DESCRIPTION
This function allows passing an I/O control to the serial I/F, represented by 
port id/stream id.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_ioctl
(
  sio_stream_id_type stream_id,              /* Stream ID                  */
  sio_port_id_type port_id,                  /* SIO Port ID                */
  sio_ioctl_cmd_type ioctl_cmd,              /* SIO I/O control command    */
  sio_ioctl_param_type* ioctl_param_ptr      /* SIO I/O control command's 
                                                parameter                  */
);

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE UNIT TESTS (ut)
==============================================================================*/
#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */


#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* defined(FEATURE_HSU_TEST) && !defined(T_REXNT) */


#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_SET_CONFIGURATION

DESCRIPTION
This function is called when the enabled callback is received.
This function sets the FD handle according to the current configuration FD, for
a given core_index.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/

void hsu_al_ser_handle_set_configuration(uint8 core_index);

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_OBEX_UNINIT

DESCRIPTION
This function is called when OBEX CDC FD notifies it is being 
un-initialized

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ser_handle_obex_uninit(cdc_appctx_t ctx);

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_ACM_UNINIT

DESCRIPTION
This function is called when ACM CDC FD notifies it is being 
un-initialized

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ser_handle_acm_uninit(void);

#endif /* _HSU_AL_SER_H_ */

