#ifndef UART_INIT_H
#define UART_INIT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             U A R T   I N I T              
                             
            Initialization of UART device driver

GENERAL DESCRIPTION

  This module contains the code used by the device manager (SIO) to initialize
  the UART driver.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  The only external function, sio_uart_driver_init, is called by the device 
  manager (SIO) to perform device driver initialization.  The name of this 
  function cannot change since SIO depends on it.

Copyright (c) 1999-2000, 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/uart_init.h#1 $

when         who       what, where, why
-----------  --------  ------------------------------------------------------
11-Nov-2008  cmihalik  Initial revision.

===========================================================================*/

#include "target.h"
#include "comdef.h"
#include "customer.h"
#include "assert.h"

#include "sio.h"
#include "sio_priv.h"

/*===========================================================================

FUNCTION       sio_uart_driver_init

DESCRIPTION    This is the UART device driver initialization function.  It's
               called by the device manager (SIO) to initialize the 
               UART driver.

DEPENDENCIES   None.

RETURN VALUE   1.

SIDE EFFECTS   None.
===========================================================================*/

uint32         sio_uart_driver_init( sio_register_device_driver_cb_type register_driver_cb_fn );

#endif
