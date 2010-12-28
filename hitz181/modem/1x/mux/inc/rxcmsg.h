/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    C D M A    R E C E I V E   M E S S A G E   D E L I V E R Y   L A Y E R

GENERAL DESCRIPTION
  This module delivers L3 messages to the calling entity (MC).

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcmsg.h_v   1.0   06 Dec 2001 12:39:02   ldefauw  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/inc/rxcmsg.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/11/05   bn      Added logic to relay the page match event to MC.
12/06/01   lad     Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_IS2000_REL_A
#include "cai.h"
#include "caix.h"


/*===========================================================================

FUNCTION RXCMSG_DELIVER

DESCRIPTION
  This function delivers a message to the mcc subtask from the rxtx task.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating success.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rxcmsg_deliver (
  caix_chan_type chan,
  cai_rx_msg_type *ext_msg_ptr, 
  unsigned int length,
  qword *frame_num,
  boolean page_match
);
#endif /* FEATURE_IS2000_REL_A */
