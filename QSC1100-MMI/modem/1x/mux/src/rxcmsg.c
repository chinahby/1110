/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    C D M A    R E C E I V E   M E S S A G E   D E L I V E R Y   L A Y E R

GENERAL DESCRIPTION
  This module delivers L3 messages to the calling entity (MC).

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                           Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcmsg.c_v   1.1   16 Jan 2002 15:16:42   ldefauw  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcmsg.c#2 $ $DateTime: 2008/07/18 10:34:36 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/08   jtm     Reduced debug string sizes for ULC effort.
01/20/06   fh      Added the support for AKA and MI.
08/11/05   bn      Added logic to relay the page match event to MC.
04/20/04   jrp     Fixed lint warnings.
12/06/01   lad     Created file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_IS2000_REL_A
#include "assert.h"
#include "err.h"
#include "mccrxtx.h"
#include "msg.h"
#include "rxcmsg.h"


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
)
{
  boolean success = FALSE;
  mccrxtx_cmd_type *msg_ptr;
  word status;

  ASSERT (ext_msg_ptr);

  msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q);

  if (msg_ptr == NULL) {
    ERR ("0 bufs on mcc_rxtx_free_q", 0, 0, 0);
  } else {
    status = caix_ext_to_int ((word)length, (word)chan, ext_msg_ptr, &msg_ptr->msg.msg);

    #if defined(FEATURE_IS2000_REL_C_MI) || defined(FEATURE_IS2000_REL_A_AES)
#error code not present
	#endif /* FEATURE_IS2000_REL_C_MI || FEATURE_IS2000_REL_A_AES */
	
    if ((status != CAIX_DONE_S)
#ifdef FEATURE_IS2000_REL_A
        /* If Ext. Encryption failure occurs, we want to send the message to
           MC anyway so it can take appropriate action by examining the 
           Ext. Encryption fields. */
        && (status != CAIX_INV_ENC_MODE_NOT_SUP_S)
#endif
      ) {
      MSG_MED ("Bad PC transl: status %d", status, 0, 0);
      /* Put item back on to free q */
      q_put (&mcc_rxtx_free_q, &msg_ptr->msg.hdr.cmd_hdr.link);
    } else {

#ifdef FEATURE_IS2000_REL_A
        if (status == CAIX_INV_ENC_MODE_NOT_SUP_S) {
          /* Ext. Encryption failure at parser, log and pass to MC. */
          MSG_HIGH ("mccrxtx: Ext. Encryption failure, pass to MC",
                    0, 0, 0);
          msg_ptr->msg.parser_decryption_failure = TRUE;
        } else {
          msg_ptr->msg.parser_decryption_failure = FALSE;
        }
#endif
        msg_ptr->msg.hdr.command = MCCRXTX_MSG_F;
        
        /* Note: the caix_chan_type and rxc_chan_type are set to the same 
           values.  MC thinks this is 'rxc_chan_type' even though we are 
           using 'caix_chan_type'.  The long term solution is for MC to use
           'caix_chan_type' type. */
        msg_ptr->msg.chn = (word)chan;

        /* message is from Paging Channel */
        qw_equ (msg_ptr->msg.frm.frame, *frame_num);

        msg_ptr->msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_free_q;
        msg_ptr->msg.hdr.cmd_hdr.task_ptr = NULL;
        msg_ptr->msg.page_match_counter_incremented = page_match;

        /* Put message on queue for MC */
        mccrxtx_cmd (msg_ptr);

        success = TRUE;

    }
  }

  return success;
}
#endif /* FEATURE_IS2000_REL_A */
