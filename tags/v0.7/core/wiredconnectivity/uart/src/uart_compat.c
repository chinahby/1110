/*============================================================================

GENERAL DESCRIPTION

  This module contains code for compatability between different versions
  of the UART driver and the code it integrates with.

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 1999-2000, 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/uart_compat.c#2 $

when         user      what, where, why
-----------  --------  -------------------------------------------------------
15-Jan-2009  cmihalik  Initial revision.

============================================================================*/

#include "uart_compat.h"

//  Something not likely to conflict with any port id.
#define SIO_PORT_ILLEGAL   ((SIO_PORT_NULL)-100)

#ifdef IMAGE_QDSP6_PROC
rex_crit_sect_type uart_crit_sect;
#endif

//----------------------------------------------------------------------------
//  uart_sio_port_id_to_device_id
//----------------------------------------------------------------------------

uart_device_id_type
uart_sio_port_id_to_device_id( sio_port_id_type port_id )
{
  uart_device_id_type device_id = UART_DEVICE_INVALID;

  //  This code assumes the client and the driver are compiled
  //  with the same version of the sio.h header file.  If not,
  //  something smarter needs to be done here.
  //
  //  Cannot use a simple switch statement.  Why?  The SIO port
  //  definitions may be replaced with SIO_PORT_ILLEGAL, indicating
  //  the port definition is not valid for the given set of 
  //  feature defines.  You can end up with a given case listed 
  //  multiple times -- a compile error.  The nested if doesn't
  //  have that problem.

  if( SIO_PORT_UART_MAIN == port_id )
    device_id = UART_DEVICE_1;
  else if( SIO_PORT_UART_AUX == port_id )
    device_id = UART_DEVICE_2;
  else if( SIO_PORT_UART_THIRD == port_id )
    device_id = UART_DEVICE_3;
  else
    device_id = UART_DEVICE_INVALID;

  if( SIO_PORT_ILLEGAL == port_id )
    device_id = UART_DEVICE_INVALID;

  return( device_id );
}

