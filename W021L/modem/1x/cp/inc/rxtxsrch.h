#ifndef RXTXSRCH_H
#define RXTXSRCH_H
/*===========================================================================

                   R X T X    S R C H    H E A D E R    F I L E

DESCRIPTION
  This file contains the interface between the rxtx (layer 2) task and the 
  Search task.

Copyright (c) 2004-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/rxtxsrch.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/04   yll     Created file. 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

  
/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_RXTX_TC_MSG_NOTIFY

/*===========================================================================

FUNCTION RXTXSRCH_REG_RX_MSG_NOTIFIER

DESCRIPTION
  This registers a function to notify the received message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxtxsrch_reg_rx_msg_notifier
(
  void ( *rx_msg_notifier )( void )
);

/*===========================================================================

FUNCTION RXTXSRCH_REG_TX_MSG_NOTIFIER

DESCRIPTION
  This registers a function to notify the sent message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxtxsrch_reg_tx_msg_notifier
(
  void ( *tx_msg_notifier )( uint16 retry_count )
);

#endif /* FEATURE_RXTX_TC_MSG_NOTIFY */

#endif /* RXTXSRCH_H */
