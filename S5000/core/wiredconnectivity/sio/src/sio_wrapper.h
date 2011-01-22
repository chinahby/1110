#ifndef SIO_WRAPPER_H
#define SIO_WRAPPER_H
/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                  W R A P P E R    F U N C T I O N S

                       H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //depot/asic/msmshared/services/sio/02.00.XX/sio_wrapper.h#1 $ $DateTime: 2008/10/19 17:40:04 $ $Author: vaghasia $

mm/dd/yy   who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/07   rh      Created module.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

/*===========================================================================

                        FEATURE FLAG MAPPING

===========================================================================*/

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION sio_wrapper_null_xxx                         EXTERNALIZED FUNCTION

DESCRIPTION
  These function are a set of null function that is runned when the registered
  driver do not register for a particular API

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
sio_status_type sio_wrapper_null_open (sio_open_type *open_ptr);

void sio_wrapper_null_close (sio_stream_id_type stream_id,
                        void (*close_func_ptr)(void));

void sio_wrapper_null_ioctl (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                      sio_ioctl_cmd_type    cmd,
                      sio_ioctl_param_type *param);

void sio_wrapper_null_transmit (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         dsm_item_type *tx_ptr);

void sio_wrapper_null_flush_tx (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         void (*flush_func_ptr)(void));

void sio_wrapper_null_disable_device (sio_port_id_type port_id);

void sio_wrapper_null_control_transmit (sio_stream_id_type stream_id,
                                 dsm_item_type     *tx_ptr);

void sio_wrapper_null_control_close (sio_stream_id_type stream_id,
                              void (*close_func_ptr)(void));

sio_status_type sio_wrapper_null_control_open (sio_open_type *open_ptr);

/*===========================================================================

FUNCTION sio_wrapper_init                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function serves as starting point for trigger the initialization 
  routine for each driver that uses the sio service

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void sio_wrapper_init(void);

#endif  /* SIO_WRAPPER_H */
