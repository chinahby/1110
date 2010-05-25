#ifndef _BTHCI_H
#define _BTHCI_H

#ifdef FEATURE_BT

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        B L U E T O O T H    H C I    I N T E R N A L    H E A D E R

DESCRIPTION
  This file contains declarations internal to the Bluetooth HCI module.

Copyright (c) 2000-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/bthci.h_v   1.1   20 Apr 2001 16:24:40   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/protected/bthci.h#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/bthci.h_v  $
 *   #Rev 1.6   27 Jun 2006            DSN
 * Added featurization to support HostController SoC solution.
 *
 * 
 *    Rev 1.5   20 Apr 2001 16:24:40   waynelee
 * Changed symbols from bt_hci_ to bt_hc_
 * 
 *    Rev 1.4   29 Jan 2001 17:07:06   propach
 * Initial revision.
 * 
 *    Rev 1.3   17 Aug 2000 13:43:42   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.2   03 Aug 2000 08:41:40   waynelee
 * Adding Low Power Mode Support.  Code for UPF-3
 *
 *    Rev 1.1   02 Aug 2000 09:39:40   propach
 * Added QBTS support for BT.  Added to API.
 *
 *    Rev 1.0   04 Jul 2000 19:19:40   ryuen
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "bt.h"
#ifndef FEATURE_BT_SOC 
#include "bthcev.h"
#endif /* FEATURE_BT_SOC */

/*===========================================================================

             DATA DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                  External Data to HCI/LMP/BB Module                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

          FUNCTION DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for functions, local and global.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                External Functions to HCI/LMP/BB Module                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION
  bt_hc_ev_event_enabled

DESCRIPTION
  Returns true if the BT hci event is enabled, i.e. if the event is not
  filtered out and should be sent.

===========================================================================*/
extern boolean        /* TRUE if enabled; FALSE otherwise */
bt_hc_ev_event_enabled
(
  bt_event_type ev    /* the BT hci event to check        */
);

/*===========================================================================

FUNCTION
  bt_hc_send_event

DESCRIPTION
  Sends the BT HCI event if this event is not filtered out by calling
  the registered event callback function.

===========================================================================*/
extern void bt_hc_send_event
(
  bt_ev_msg_type*  ev_msg_ptr    /* ptr to BT HCI event to send            */
);

#endif /* FEATURE_BT */

#endif /*  _BTHCI_H */

