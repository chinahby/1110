#ifndef SIORS232_MDD_DATA_H
#define SIORS232_MDD_DATA_H

/*===========================================================================

           S I O   D E V I C E   L A Y E R   M D D   (D A T A)   H E A D E R

DESCRIPTION
   This is the header file for the common code used by the SIO UART driver. 

Copyright (c) 2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_mdd_data.h#1 $

when         who       what, where, why
-----------  --------  ----------------------------------------------------------
07-Jul-2008  cmihalik  Initial revision.
===========================================================================*/

#include "siors232_mdd.h"

void                siors_mdd_rx_rdata_nesc_isr( sio_mdd_port_type* mdd_port );
void                siors_mdd_rx_rdata_esc_isr( sio_mdd_port_type* mdd_port );
void                siors_mdd_rx_autodetect_isr( sio_mdd_port_type* mdd_port );
void                siors_mdd_setup_uart_autodetect( sio_mdd_port_type* mdd_port );

#endif /* SIORS232_MDD_AD_H */

