/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    C D M A   M A I N   C O N T R O L
               B R O A D C A S T   S M S   S E R V I C E S

GENERAL DESCRIPTION
  This module contains functions for the Broadcast SMS services.

EXTERNALIZED FUNCTIONS

  MCCBCSMS_INIT -
    Initialize the broadcast SMS service and its related variables.

  MCCBCSMS_PROC_SRCH_RPT -
    This function process the Searcher's report, the wakeup report. It
    updates the Broadcast SMS related variables.

  MCCBCSMS_PROC_MSG -
    This function process the System Param Message, Extended System Param
    Message, and the MCRR Param Message for the Broadcast SMS related
    parameters. It checks to see if the Broadcast SMS configuration has
    been changed. If so, it updates the broadcast slot cycle and resets
    the related broadcast schedules.

  MCCBCSMS_SCHEDULE_BCAST -
    This function schedules the Broadcast SMS by finding the next Broadcast
    wakeup slot and sending them to the Searcher through sleep command.

  MCCBCSMS_COMP_ADDR -
    This function does necessary processing when receiving a broadcast
    message. It compares the BC address and checks if this is an expected
    broadcast message corresponding to the outstanding schedules. It also
    checks if it's a duplicate. If it's unique, it then inserts the
    broadcast message into the unique bcast list so that later broadcast
    pages and messages can perform the duplicate detection agaist it.

  MCCBCSMS_UNIV_PAGE_PARTIAL_ADDR_MATCH
    This function performs the Mobile Station Broadcast partial address match
    operation for the Universal Page Message. The result of match is directly
    saved in the input partial address structure.

  MCCBCSMS_PAGE_MATCH -
    This function performs broadcast page match operation.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  mccbcsms_init() needs to be called before calling any of the other
  functions.

Copyright (c) 2002-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccbcsms.c_v   1.15   02 Oct 2002 22:52:28   louiel  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccbcsms.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/09   ag      Fixed the issue of missing BCSMS when MS transitions from
                   system access to idle state.
09/10/07   yll     Fixed a problem that MS doesn't receive consecutive and
                   large broadcast messages that take 3 paging channel slots 
                   to send.
05/11/07   pg      More Lint Cleanup.
03/09/07   pg      removed dependency on WMS and make sure it only depends on 
                   CM for SMS.
05/30/06   fc      Fixed lint errors.
04/27/06   fc      Fixed lint errors.
04/26/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel A featurization.
04/18/06   fc      Changed mccbcsms_config_type to use 
                   cdma_bcast_bcch_info_type. 
02/07/06   fc      Removed duplicate function prototype.
01/06/06   fc      Cleanup on OOS changes.
07/15/05   fc      Fixed lint errors.
06/13/05   fc      Merged the following from MSMSHARED:
06/09/05   yll      Fixed the problem of not scheduling the repeat broadcast
                    slots.
05/10/05   fc      Merged the following from MSMSHARED:
04/13/05   yll      Fixed the problem of not scheduling the Enh. BCSMS after
                    releasing the call.
01/20/05   fh      Changed to check the duplication for each of the records in
                   BCSMS pages, before scheduling a wakeup for receiving it.       
12/05/04   yll     Added interface change for apiOne.
11/11/04   yll     Changed the BCCH_INDEX field in GPM/UPM to BCN number,
                   based on the standard change.
10/03/04   yll     Handle the case of receiving of broadcast message before 
                   initialization of the bcast_index.
08/19/04   bkm     Removed unnecessary extern function declaration.
08/18/04   yll     Clean up the MC/SRCH interface.
06/16/04   fc      Prototype change for mccidl_supervise_channel_monitoring.
04/06/04   yll     Calculate the start of the broadcast message to compare
                   against the scheduled broadcast data slot.
02/25/04   yll     After receiving the scheduled broadcast SMS, if there are
                   more broadcast messages on BCCH of same BCN, the mobile 
                   should continue monitoring.
10/03/03   yll     When in fast RAHO, don't send the BC_INFO to Search.
05/30/03   yll     When bcsms_enable changes, update the bcast_index to Srch
                   by sending a parm cmd.
05/27/03   yll     Changes for simultaneous monitoring of FCCCH and 2nd BCCH.
03/14/03   yll     When PCH Broadcast Page buffer is full, remove the expired 
                   entries.
02/14/03   yll     Fixed a problem in peeking the broadcast message address
                   due to a race condition between RXC and MC.
01/30/03   yll     Fixed a problem of missing broadcast scheduling on PCH.
01/09/03   yll     Changes to save the old bc schedule on PCH that is
                   previously sent to Search, so that it doesn't send
                   multiple times.
12/02/02   yll     Fixed the problem when RXC is configured to Non-pri BCCH
                   to match the msg header before CP receives the wakeup rpt.
11/26/02   ph      mcc_bc_match(), added a return for bad ovhd chan case or
                   when BCSMS not turned on in new common channels.
11/19/02   fc      Added support for FEATURE_CDSMS UPM partial broadcast
                   address match.
11/18/02   yll     Fixed a problem to reset bc pages when receiving (E)SPM.
10/28/02   yll     Moved the broadcast schedule info from the sleep command
                   to bc_info_f.
10/24/02   yll     Changed the mccbcsms_match to mccbcsms_pc_match and
                   mccbcsms_bcch_match functions.
10/02/02   yll     Linted the code.
09/23/02   yll     Fixed the problem where mobile awake for long duration
                   after receiving the Broadcast SMS message.
09/04/02   AP      Added support for FEATURE_CDSMS.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
06/27/02   fc      Changed UPM function names.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/12/02   yll     Changes for the code review comments.
05/22/02   yll     Save the enhanced broadcast page when bcast_index is not
                   initialized after power up.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed the function names.
02/10/02   yll     Create the module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "comdef.h"

#ifdef FEATURE_BROADCAST_SMS
#include "cai.h"
#include "err.h"
#include "msg.h"
#include "bit.h"
#include "qw.h"
#include "ts.h"
#include "caix.h"
#include "mccdma.h"
#include "mccbcsms.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* broadcast SMS enabled? It's set by MC_BCSMS_ENABLE_F from CM */
/* It is referenced in other modules. Will be obsolete.         */
boolean mcc_bc_sms_enable;

/* Broadcast SMS unique entries, for the duplicate detection.   */
LOCAL mccbcsms_unique_rec_type      mccbcsms_unique_recs;

/* Matched (enhanced) broadcast pages, for broadcast schedules  */
LOCAL mccbcsms_matched_page_type    mccbcsms_pages;

/* The broadcast config info. Updated w/ ESPM or MCRRPM message */
LOCAL mccbcsms_config_type          mccbcsms_config;

/* Previous bc page schedule on PCH sent to Search */
LOCAL qword             mccbcsms_old_pch_bc_slot;

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/* Previous enhanced bc page schedules on CCCH sent to Search */
LOCAL qword             mccbcsms_prev_fccch_ebc_slots[CAI_MAX_NON_PRI_BCCH_CHAN];
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#ifdef FEATURE_BROADCAST_SMS
/* Variable indicating whether a BCSMS is in progress or not */
boolean mcc_bcsms_in_progress = FALSE;

/* The starting slot of the BCSMS in progress */
qword mcc_bcsms_start_slot = {0, 0};
#endif//FEATURE_BROADCAST_SMS

/*===========================================================================

                                 FUNCTIONS

===========================================================================*/

/************************** Start Internal Functions ***********************/

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS

/*--------------------------------------------------------------------------
  The following functions are used only for the Broadcast SMS on the
  IS2000 Rel A Common Channel (F-CCCH and BCCH).
  -------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION MCCBCSMS_INIT_EBC_PAGES

DESCRIPTION
  This function initializes the Enhanced Broadcast Page Variables.
  It makes the free EBC page pool by linking the page record item together.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mccbcsms_init_ebc_pages( void )
{
  int               i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset((void *)&mccbcsms_pages, 0, sizeof(mccbcsms_pages));
  mccbcsms_pages.bc_type    = ENHANCED_BROADCAST_PAGE;

  /* Make the enh_bc_free_page pool */
  for (i = 0; i < MAX_ENH_BC_PAGES - 1; i++)
  {
    mccbcsms_pages.pages.ebc.page_recs[i].next_ptr =
        &mccbcsms_pages.pages.ebc.page_recs[i+1];
  }
  mccbcsms_pages.pages.ebc.first_free_page_ptr =
      &mccbcsms_pages.pages.ebc.page_recs[0];
  mccbcsms_pages.pages.ebc.last_free_page_ptr  =
      &mccbcsms_pages.pages.ebc.page_recs[MAX_ENH_BC_PAGES - 1];

} /* mccbcsms_init_ebc_pages */

/*===========================================================================

FUNCTION MCCBCSMS_INSERT_EBC_PAGE

DESCRIPTION
  This function inserts a EBC page into a link list sorted with bc_data_slot.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE
  TRUE if successful.
  FALSE if passed in parameters are invalid.

SIDE EFFECTS

===========================================================================*/
boolean mccbcsms_insert_ebc_page
(
  byte              bcn,
  enh_bc_page_type  *page_ptr
)
{
  enh_bc_page_type  *prev_ptr, *cur_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if bcn is valid */
  if ( bcn <= MCC_PRI_BCCH_BCN ||
       bcn > CAI_MAX_NON_PRI_BCCH_CHAN + MCC_PRI_BCCH_BCN )
  {
    MSG_ERROR("bcn is invalid %d", bcn, 0, 0);
    return FALSE;
  }

  /* If it's the begining of the list. */
  if ( mccbcsms_pages.pages.ebc.page_ptrs[bcn -2] == NULL )
  {
    mccbcsms_pages.pages.ebc.page_ptrs[bcn - 2] = page_ptr;
    page_ptr->next_ptr = NULL;
    return TRUE;
  }

  /* Find the position in the page list to insert the page record */
  /* page_ptrs is ordered by the bc_data_slot.                    */
  for ( prev_ptr = cur_ptr = mccbcsms_pages.pages.ebc.page_ptrs[bcn - 2];
        cur_ptr != NULL;
        prev_ptr = cur_ptr, cur_ptr = cur_ptr->next_ptr )
  {
    if (qw_cmp(page_ptr->data_slot_40, cur_ptr->data_slot_40) < 0)
    {
      break;
    }
  }

  if ( cur_ptr == mccbcsms_pages.pages.ebc.page_ptrs[bcn - 2] )
  {
    /* insert to the head of the list */
    page_ptr->next_ptr = cur_ptr;
    mccbcsms_pages.pages.ebc.page_ptrs[bcn - 2] = page_ptr;
  }
  else
  {
    prev_ptr->next_ptr = page_ptr;
    page_ptr->next_ptr = cur_ptr;
  }
  return TRUE;

} /* mccbcsms_insert_ebc_page */

/*===========================================================================

FUNCTION MCCBCSMS_REMOVE_EBC_PAGE

DESCRIPTION
  This function removes a EBC page from the link list, and returns the
  memory back to the free EBC page pool.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mccbcsms_remove_ebc_page
(
  enh_bc_page_type  **head,
  enh_bc_page_type  **pre,
  enh_bc_page_type  **cur
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( *head == *cur )
  {
    *head = (*cur)->next_ptr;

    /* Put the deleted item to the free pool */
    (*cur)->next_ptr = NULL;

    if (mccbcsms_pages.pages.ebc.first_free_page_ptr == NULL)
    {
      mccbcsms_pages.pages.ebc.first_free_page_ptr = *cur;
      mccbcsms_pages.pages.ebc.last_free_page_ptr  = *cur;
    }
    else
    {
      mccbcsms_pages.pages.ebc.last_free_page_ptr->next_ptr = *cur;
      mccbcsms_pages.pages.ebc.last_free_page_ptr           = *cur;
    }

    /* Re-adjust the pre, cur pointers */
    *pre = *cur = *head;
  }
  else
  {
    (*pre)->next_ptr = (*cur)->next_ptr;

    /* Put the deleted item to the free pool */
    (*cur)->next_ptr = NULL;
    if (mccbcsms_pages.pages.ebc.first_free_page_ptr == NULL)
    {
      mccbcsms_pages.pages.ebc.first_free_page_ptr = *cur;
      mccbcsms_pages.pages.ebc.last_free_page_ptr  = *cur;
    }
    else
    {
      mccbcsms_pages.pages.ebc.last_free_page_ptr->next_ptr = *cur;
      mccbcsms_pages.pages.ebc.last_free_page_ptr           = *cur;
    }

    *cur = (*pre)->next_ptr;
  }

} /* mccbcsms_remove_ebc_page */

/*===========================================================================

FUNCTION MCCBCSMS_EXTRACT_EBC_PAGE

DESCRIPTION
  This function extracts a EBC page from the link list WITHOUT returning the
  memory back to the free EBC page pool.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE
  Pointer to the EBC page that is extracted.

SIDE EFFECTS

===========================================================================*/
enh_bc_page_type *mccbcsms_extract_ebc_page
(
  enh_bc_page_type  **head,
  enh_bc_page_type  **pre,
  enh_bc_page_type  **cur
)
{
  enh_bc_page_type  *tmp_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( *head == *cur )
  {
    *head = (*cur)->next_ptr;

    tmp_ptr = *cur;
    tmp_ptr->next_ptr = NULL;

    /* Re-adjust the pre, cur pointers */
    *pre = *cur = *head;
  }
  else
  {
    (*pre)->next_ptr = (*cur)->next_ptr;

    tmp_ptr = *cur;
    tmp_ptr->next_ptr = NULL;

    *cur = (*pre)->next_ptr;
  }

  return tmp_ptr;

} /* mccbcsms_extract_ebc_page */

/*===========================================================================

FUNCTION MCCBCSMS_FIND_EBC_PAGE

DESCRIPTION
  This function searches for a corresponding EBC page record that matches
  the bcn, burst_type, bc_addr and data_slot.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE
  If found, it returns a pointer to the EBC page.
  Otherwise, NULL.

SIDE EFFECTS

===========================================================================*/
/*lint -e818*/
enh_bc_page_type *mccbcsms_find_ebc_page
(
  byte  bcn,
  byte  burst_type,
  byte  *bc_addr,
  qword data_slot_40    /* in 40 ms unit */
)
{
  enh_bc_page_type  *tmp_ptr = NULL;
  boolean           found_page = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  /* Check if bcn is valid */
  if ( bcn <= MCC_PRI_BCCH_BCN ||
       bcn > CAI_MAX_NON_PRI_BCCH_CHAN + MCC_PRI_BCCH_BCN )
  {
    MSG_ERROR("bcn is invalid %d", bcn, 0, 0);
    return NULL;
  }

  /* Multiple EBC pages may have the same broadcast data    */
  /* slot schedules. We need to check all the EBC pages     */
  /* data slots are <= the passed in msg_slot_40, because   */
  /* some EBC pages may be expired already.                 */
  tmp_ptr = mccbcsms_pages.pages.ebc.page_ptrs[bcn - 2];

  /* Skip the "old" EBC pages. */
  while (tmp_ptr &&
         qw_cmp(tmp_ptr->data_slot_40, data_slot_40) < 0)
  {
    tmp_ptr = tmp_ptr->next_ptr;
  }

  while (tmp_ptr &&
         qw_cmp(tmp_ptr->data_slot_40, data_slot_40) == 0)
  {
    if ((memcmp(tmp_ptr->bc_addr, bc_addr, CAII_BC_ADDR_LEN) == 0) &&
        (burst_type == tmp_ptr->burst_type) )
    {
      found_page = TRUE;
      break;
    }
    else
    {
      tmp_ptr = tmp_ptr->next_ptr;
    }
  } /* while */

  if ( found_page )
  {
    return tmp_ptr;
  }
  else
  {
    return NULL;
  }

} /* mccbcsms_find_ebc_page */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCBCSMS_DELETE_DUP_EBC_PAGES

DESCRIPTION
  This function deletes all the EBC pages considered duplicate according to
  IS2000 2.6.2.1.1.3.6: the EBC pages with the same bc_addr and burst_type
  that are received with certain expiration period.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
/*lint -e818*/
void mccbcsms_delete_dup_ebc_pages
(
  byte  *bc_addr,
  byte  burst_type,
  qword recv_slot_40    /* in 40 ms unit */
)
{
  int               i;
  enh_bc_page_type  *pre_ptr, *cur_ptr;
  word              recv_offset;
  qword             expire_slot_40;
  qword             tmp_recv_slot_40;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  /* According to the spec, IS2000, 2.6.2.1.1.3.6:-                  */
  /* After receiving an enhanced broadcast page and a corresponding  */
  /* broadcast message when BCAST_INDEXs is not equal to '000', the  */
  /* mobile station should discard all further enhanced broadcast    */
  /* pages containing the same BURST_TYPE and having the same        */
  /* broadcast address that are received within 4 x (B + 7) slots of */
  /* the first slot in the broadcast paging cycle in which the       */
  /* enhanced broadcast page was received.  (B + 7 is the duration of*/
  /* the broadcast paging cycle as defined in 2.6.2.1.1.3.3.1).      */
  /* The mobile station should ignore broadcast messages for which a */
  /* corresponding enhanced broadcast page was not received.         */

  if ( mccbcsms_config.bcast_slot_cycle != 0 )
  {
    /* Get the first slot of this broadcast slot cycle which the  */
    /* enhanced broadcast page is received in the recv_slot. Note */
    /* bcast_slot_cycle is in 80ms, so need to convert to 40ms.   */
    recv_offset = qw_div(expire_slot_40, recv_slot_40,
                         mccbcsms_config.bcast_slot_cycle<<1);  //lint !e734
    qw_equ(expire_slot_40, recv_slot_40);
    qw_dec(expire_slot_40, recv_offset);
    qw_inc(expire_slot_40, mccbcsms_config.bcast_slot_cycle << 3);
      /* 4 * bcast_slot_cycle, multiply 2 to get the 40ms units */
  }
  else
  {
    /* Multi-slotted mode (bcast_index == 0), the expiration period */
    /* is 16 x 2 ^ max_slot_cycle_index, as per IS-2000.            */
    qw_equ(expire_slot_40, recv_slot_40);
    qw_inc(expire_slot_40, 16L<<(cur_bs_ptr->csp.sp.max_slot_cycle_index+1));
      /* multiply 2 to get the 40ms units */
  }
  MSG_LOW("delete ebcpg, expire slt: 0x%x",
           (qw_lo(expire_slot_40) & 0xFF), 0, 0);

  for ( i = 0; i < mccbcsms_config.enbcsms_info.num_bcch_bcast; i++ )
  {
    pre_ptr = cur_ptr = mccbcsms_pages.pages.ebc.page_ptrs[i];

    while ( cur_ptr != NULL )
    {
      if ( memcmp(bc_addr, cur_ptr->bc_addr, CAII_BC_ADDR_LEN) == 0 &&
           burst_type == cur_ptr->burst_type )
      {
        if ( qw_cmp(cur_ptr->data_slot_40, expire_slot_40) <= 0 )
        {
          /* If data_slot is within the duplicate range,    */
          /* the recv_slot must be with this range too.     */
          /* Remove the duplicate bc page record and put    */
          /* it into free pool                              */
          MSG_MED("remove ebc page, data_slot 0x%x",
                   (qw_lo(cur_ptr->data_slot_40) & 0xFF), 0, 0);
          mccbcsms_remove_ebc_page(&mccbcsms_pages.pages.ebc.page_ptrs[i],
                                   &pre_ptr, &cur_ptr);

          /* No need to advance the pre_ptr and cur_ptr. They   */
          /* are done in the mcc_remove_ebc_page already.       */
        }
        else
        {
          /* data_slot is not in the duplicate range, but   */
          /* the recv_slot might be. Calculate the recv_slot*/
          qw_equ(tmp_recv_slot_40, cur_ptr->data_slot_40);
          qw_dec(tmp_recv_slot_40, cur_ptr->time_offset_40 + 1 );

          if ( qw_cmp(tmp_recv_slot_40, expire_slot_40) <= 0 )
          {
            /* Remove the duplicate bc page record and put  */
            /* it into free pool                            */
            MSG_MED("remove ebc page, slot 0x%x data 0x%x",
                     (qw_lo(tmp_recv_slot_40) & 0xFF),
                     (qw_lo(cur_ptr->data_slot_40) & 0xFF), 0);
            mccbcsms_remove_ebc_page(&mccbcsms_pages.pages.ebc.page_ptrs[i],
                                     &pre_ptr, &cur_ptr);

            /* No need to advance the pre_ptr and cur_ptr. They   */
            /* are done in the mcc_remove_ebc_page already.       */
          }
          else
          {
            /* advance the pre_ptr and cur_ptr */
            pre_ptr = cur_ptr;
            cur_ptr = cur_ptr->next_ptr;
          }
        }
      }
      else
      {
        /* advance the pre_ptr and cur_ptr */
        pre_ptr = cur_ptr;
        cur_ptr = cur_ptr->next_ptr;
      }
    } /* while */
  } /* for */

} /* mccbcsms_delete_dup_ebc_pages */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCBCSMS_DELETE_EXPIRED_EBC_PAGES

DESCRIPTION
  This function deletes all the expired EBC pages. An EBC page is expired
  when both its bc_data_slot and repeat_bc_data_slot are older than the
  passed in cur_slot.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void mccbcsms_delete_expired_ebc_pages
(
  qword     cur_slot_40         /* in 40 ms unit */
)
{
  enh_bc_page_type  *pre_ptr, *cur_ptr;
  enh_bc_page_type  *tmp_ptr;
  qword             repeat_slot_40;
  int               i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Re-sorting the list: If the bc_data_slot has expired   */
  /* (passed the cur_slot), then check its repeat_bc_data_  */
  /* slot. If repeat slot is expired also, remove this node */
  /* from the list. If repeat slot has not expired yet,     */
  /* assign the data slot to be equal to the repeat slot.   */
  /* Extract this node and insert into the appropriate place*/

  /* If any nodes need to be removed or resorted, they must */
  /* be the front nodes of the list.                        */

  for ( i = 0; i < mccbcsms_config.enbcsms_info.num_bcch_bcast; i++ )
  {
    pre_ptr = cur_ptr = mccbcsms_pages.pages.ebc.page_ptrs[ i ];
    while ( cur_ptr && 
            (qw_cmp(cur_ptr->data_slot_40, cur_slot_40) < 0) )
    {
      /* As per IS-2000, the mobile station shall monitor the Broadcast */
      /* Control Channel slot which begins 40 ms x (1 + REPEAT_TIME     */
      /* _OFFSET) later than the Broadcast Control Channel slot in      */
      /* which the first transmission began.                            */
      if (cur_ptr->repeat_time_offset_40 < CAI_INV_REPEAT_TIME_OFFSET)
      {
        qw_equ(repeat_slot_40, cur_ptr->data_slot_40);
        qw_inc(repeat_slot_40, cur_ptr->repeat_time_offset_40 + 1);
      }
      else
      {
        /* If repeat_time_offset is not included, set it to 0 */
        qw_set(repeat_slot_40, 0L, 0L);
      }

      if (qw_cmp(repeat_slot_40, cur_slot_40) < 0)
      {
        /* Both data_slot and repeat_data_slot are expired, remove it */
        MSG_HIGH("ebcpg removed: data 0x%x repeat 0x%x cur 0x%x",
                  (qw_lo(cur_ptr->data_slot_40) & 0xFF), 
                  (qw_lo(repeat_slot_40) & 0xFF), 
                  (qw_lo(cur_slot_40) & 0xFF) );
        mccbcsms_remove_ebc_page(&mccbcsms_pages.pages.ebc.page_ptrs[i],
                                 &pre_ptr,
                                 &cur_ptr);
        /* No need to advance the pre_ptr and cur_ptr. They   */
        /* are done in the mcc_remove_bc_page already.        */
      }
      else
      {
        /* data_slot expires, but repeat_data_slot hasn't. Assign the */
        /* data_slot to be the repeat_data_slot, and resort the list. */
        MSG_HIGH("Skip ebc data slot, try repeat slot", 0, 0, 0);
        MSG_MED("Data slot 0x%x, repeat slot 0x%x",
                 (qw_lo(cur_ptr->data_slot_40) & 0xFF), 
                 (qw_lo(repeat_slot_40) & 0xFF), 0);

        qw_equ(cur_ptr->data_slot_40, repeat_slot_40);
        cur_ptr->repeat_time_offset_40 = CAI_INV_REPEAT_TIME_OFFSET;
        tmp_ptr = mccbcsms_extract_ebc_page(
                      &mccbcsms_pages.pages.ebc.page_ptrs[i],
                      &pre_ptr, &cur_ptr);
        /* No need to advance the pre_ptr and cur_ptr. They   */
        /* are done in the mcc_extract_bc_page already.       */

        (void)mccbcsms_insert_ebc_page((byte)i + 2, tmp_ptr);    
      }
    } /* while */
  } /* for */

} /* mccbcsms_delete_expired_ebc_pages */

/*===========================================================================

FUNCTION MCCBCSMS_GET_FREE_EBC_PAGE

DESCRIPTION
  This function tries to get the memory from the free EBC page pool. If
  successful, it will initialize the EBC page with the appropriate value.
  If there is no free EBC page pool left, it will try to delete the expired
  EBC pages, and free their memory.

DEPENDENCIES
  mccbcsms_init_ebc_pages() must be called before calling this function.

RETURN VALUE
  If there are free EBC page memory available, it returns a valid pointer.
  Otherwise, it returns NULL.

SIDE EFFECTS

===========================================================================*/
/*lint -e818*/
enh_bc_page_type *mccbcsms_get_free_ebc_page
(
  byte  *bc_addr,
  byte  burst_type,
  qword recv_slot_40,           /* recv slot, in 40 ms */
  word  time_offset_40,         /* time offset, in 40 ms */
  byte  repeat_time_offset_40   /* repeat time offset, in 40 ms */
)
{
  enh_bc_page_type  *page_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  if ( mccbcsms_pages.pages.ebc.first_free_page_ptr == NULL )
  {
    /* No free page pool, delete the expired EBC pages, if any. */
    mccbcsms_delete_expired_ebc_pages( recv_slot_40 );
    MSG_HIGH(" Delete expired EBC pgs for new pgs", 0, 0, 0 );
  }

  page_ptr = mccbcsms_pages.pages.ebc.first_free_page_ptr;
  if ( page_ptr )
  {
    /* Advance the first_free_page_ptr and last_free_page_ptr, if needed */
    if (mccbcsms_pages.pages.ebc.first_free_page_ptr ==
        mccbcsms_pages.pages.ebc.last_free_page_ptr)
    {
      mccbcsms_pages.pages.ebc.first_free_page_ptr = NULL;
      mccbcsms_pages.pages.ebc.last_free_page_ptr  = NULL;
    }
    else
    {
      mccbcsms_pages.pages.ebc.first_free_page_ptr =
          mccbcsms_pages.pages.ebc.first_free_page_ptr->next_ptr;
    }

    /* Copy the data into the free page record. */

    memcpy(page_ptr->bc_addr, bc_addr, CAII_BC_ADDR_LEN);

    page_ptr->burst_type = burst_type;
    page_ptr->time_offset_40 = time_offset_40;

    /* As per IS-2000, the mobile station shall monitor the Broadcast */
    /* Control Channel slot which begins 40 ms x (1 + TIME_OFFSET)    */
    /* later than the beginning of the slot in which the message      */
    /* containing the enhanced broadcast page began                   */
    qw_equ(page_ptr->data_slot_40, recv_slot_40);
    qw_inc(page_ptr->data_slot_40, time_offset_40 + 1);

    /* As per IS-2000, the mobile station shall monitor the Broadcast */
    /* Control Channel slot which begins 40 ms x (1 + REPEAT_TIME     */
    /* _OFFSET) later than the Broadcast Control Channel slot in      */
    /* which the first transmission began.                            */
    page_ptr->repeat_time_offset_40 = repeat_time_offset_40;

    page_ptr->next_ptr = NULL;
  }

  return page_ptr;

} /* mccbcsms_get_free_ebc_page */
/*lint +e818*/

/*===========================================================================

FUNCTION MCCBCSMS_EBC_CONFIG_CHANGED

DESCRIPTION
  This function checks to see if a MC-RR message contains the broadcast
  scheduling info and non-primary BCCH channel config info that are different
  from the current one. If EBC config has been changed, the EBC scheduling
  needs to be reset.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if EBC config has changed.
  FALSE, if EBC config hasn't changed.

SIDE EFFECTS

===========================================================================*/
/*lint -e818*/
boolean mccbcsms_ebc_config_changed
(
  cdma_bs_type              *new_bs_config      /* Current BC config info. */
)
{
  boolean   config_changed = FALSE;
  int       i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mccbcsms_config.ovhd_chan != CAIX_BCCH)
  {
    if (mccbcsms_config.ovhd_chan == CAIX_PC)
    {
      MSG_MED("Bcsms: HO PCH->BCCH", 0, 0, 0);
    }
    config_changed = TRUE;
  }
  else if (mccbcsms_config.bcast_index !=
           new_bs_config->csp.esp.bcast_index )
  {
    MSG_MED("bcast_index changed old %d new %d",
             mccbcsms_config.bcast_index,
             new_bs_config->csp.esp.bcast_index, 0);
    config_changed = TRUE;
  }
  else if (mccbcsms_config.enbcsms_info.num_bcch_bcast !=
           new_bs_config->csp.bcast_bcch_info.num_bcch_bcast )
  {
    MSG_MED("num_bcch changed old %d new %d",
             mccbcsms_config.enbcsms_info.num_bcch_bcast,
             new_bs_config->csp.bcast_bcch_info.num_bcch_bcast, 0);
    config_changed = TRUE;
  }
  else
  {
    for (i = 0; i < new_bs_config->csp.bcast_bcch_info.num_bcch_bcast; i++)
    {
      if((mccbcsms_config.enbcsms_info.bcch_info[i].bcch_code_chan       !=
          new_bs_config->csp.bcast_bcch_info.bcch_info[i].bcch_code_chan) ||
         (mccbcsms_config.enbcsms_info.bcch_info[i].brat                 !=
          new_bs_config->csp.bcast_bcch_info.bcch_info[i].brat          ) ||
         (mccbcsms_config.enbcsms_info.bcch_info[i].bcch_code_rate       !=
          new_bs_config->csp.bcast_bcch_info.bcch_info[i].bcch_code_rate)
        )
      {
        MSG_MED("bcast_bcch_info[%d] changed", i, 0, 0);
        config_changed = TRUE;
        break;
      }
    }
  }

  return config_changed;

} /* mccbcsms_ebc_config_changed */
/*lint +e818*/
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

/*--------------------------------------------------------------------------
  The following functions are used only for the Broadcast SMS on the
  Paging Channel.
  -------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION MCCBCSMS_CAL_BC_REC_INDEX

DESCRIPTION
  This function calculates the index that the new broadcast schedule record
  should be inserted in the sorted list. It go through the existing list and
  compare the bc_wake_slot against the list entries to found where to insert.

DEPENDENCIES
  None.

RETURN VALUE
  The index where the entry is to be inserted.

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e818*/
int mccbcsms_cal_bc_rec_index
(
  bc_page_type * bc_rec,
  qword bc_wakeup_slot      /* In 80ms unit */
)
{
  /* the index where the entry is to be inserted */
  int t_index = mccbcsms_pages.pages.bc.tot_bc_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if the list is empty then append */
  while ( t_index != 0 )
  {
    /* if wakeup_slot < bc_wakeup_slot */
    if ( qw_cmp(bc_rec[t_index-1].wakeup_slot,bc_wakeup_slot)<0 )
    {
      break;
    }
    else
    {
      bc_rec[t_index] = bc_rec[t_index-1];
      t_index--;
    }
  }

  return (t_index);

} /* mccbcsms_cal_bc_rec_index */
/*lint +e818*/

/*--------------------------------------------------------------------------
  The following functions are used for the Broadcast SMS on both the
  Paging Channel and IS2000 Rel A Common Channel (BCCH/FCCCH).
  -------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION MCCBCSMS_DETECT_DUPLICATE

DESCRIPTION
  This function checks for duplicate broadcast message based on the
  BURST_TYPE and BC_ADDR.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a duplicate is found, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
/*lint -e818*/
boolean mccbcsms_detect_duplicate
(
  /* all the info needed to determine whether a dup is received */
  byte burst_type,
  byte *bc_addr,
  dword slot_stamp,         /* slot stamp in 80ms */
  dword bc_expire_cycle     /* expiration cycle, in 80ms */
)
{
  int t_index ; /* temp index */
  int i=0; /* temp counter */
  dword t_expire_slot; /* temp slot stamp for calculate the expire slot */
  boolean check_addr=FALSE;
  /* set true if the msg we are checking have time stamps within
   * 4*bc_slot_cycle, so we need to check whether the bc_addr also match */

  dup_detect_type * list_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  t_index = mccbcsms_unique_recs.list_begin;

  while ( mccbcsms_unique_recs.tot_entries != 0 &&
          i < mccbcsms_unique_recs.tot_entries    )
  {
    list_ptr = &mccbcsms_unique_recs.unique_entries[t_index];
    t_expire_slot = list_ptr->slot_stamp + bc_expire_cycle ;
    if (!check_addr)
    {
      if (list_ptr->slot_stamp > slot_stamp)
      {
        /* because we use dword for time stamp, if the current time is
         * smaller than the old time slot_stamp has wrapped around */
        if ((t_expire_slot < list_ptr->slot_stamp) &&
            (t_expire_slot >= slot_stamp))
        {
          /* if expire time wraps beyond the current time, we might have a
           * duplicate */
          check_addr = TRUE; /* need to check bc_addr and burst_type */
        }
      }
      else if ( t_expire_slot >= slot_stamp)
      {
        check_addr = TRUE; /* need to check bc_addr and burst_type */
      }

      /* Since the list is sorted based on arrival, we should only compare
       * the most recent msgs within 4*bc_slot_cycle, other msgs should have
       * already expired and should not be considered anymore, so we move the
       * list_begin index */
      if (!check_addr)
      {
        t_index =
        mccbcsms_unique_recs.list_begin =
            (mccbcsms_unique_recs.list_begin + 1) % MAX_BC_DUP_DETECT; /*lint !e573 !e737 !e834 */
        mccbcsms_unique_recs.tot_entries--;
      }
    }
    if (check_addr)
    {
 
      if (list_ptr->burst_type == burst_type &&
          memcmp(list_ptr->bc_addr, bc_addr, CAII_BC_ADDR_LEN) == 0)
      {
        /* we have determined a duplicate */
        MSG_MED("Dup BCSMS, old 0x%x, new 0x%x",
        list_ptr->slot_stamp, slot_stamp, 0);
        return (TRUE);
      }
    }
    t_index = (t_index+1) % MAX_BC_DUP_DETECT;  /*lint !e573 !e737 !e834 */
    i++;
    /* continue checking the rest of the list for dups */
  }
  MSG_LOW("BCDUP check, Slot=0x%lx, Total=%d",
          slot_stamp, mccbcsms_unique_recs.tot_entries, 0);

  return (FALSE);

} /* mccbcsms_detect_duplicate */

/*===========================================================================

FUNCTION MCCBCSMS_INSERT_UNIQUE

DESCRIPTION
  This function inserts the broadcast msg time stamp into the
  bc_unqiue_entries[] based on BURST_TYPE and BC_ADDR.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  bc_unqiue_entries[] is updated

===========================================================================*/
void mccbcsms_insert_unique
(
  /* all the info needed to determine whether a dup is received */
  byte burst_type,
  byte *bc_addr,
  dword slot_stamp      /* slot stamp, in 80ms */
)
{
  int t_index ; /* temp index */
  dup_detect_type * list_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCC_CHK_NULL_PTR(bc_addr);

  if (mccbcsms_unique_recs.tot_entries == MAX_BC_DUP_DETECT) /*lint !e737 !e834 */
  {
    /* We have wrapped around. Douh! List overflow should be very
     * unlikely if the BC traffic model is correct. */
    mccbcsms_unique_recs.list_begin =
        (mccbcsms_unique_recs.list_begin + 1) % MAX_BC_DUP_DETECT;/*lint !e573 !e737 !e834 */
    mccbcsms_unique_recs.tot_entries--;
    MSG_LOW("BCDUP minus",0,0,0);
  }

  t_index =
      (mccbcsms_unique_recs.list_begin + mccbcsms_unique_recs.tot_entries)
      % MAX_BC_DUP_DETECT;                                /*lint !e573 !e737 !e834 */
  list_ptr = &mccbcsms_unique_recs.unique_entries[t_index];
  list_ptr->burst_type = burst_type;

  memcpy(list_ptr->bc_addr, bc_addr, CAII_BC_ADDR_LEN);

  list_ptr->slot_stamp = slot_stamp;
  mccbcsms_unique_recs.tot_entries++;
  MSG_LOW("BCDUP add, Slot=0x%lx, Total=%d",
          slot_stamp, mccbcsms_unique_recs.tot_entries, 0);

} /* mccbcsms_insert_unique */
/*lint +e818*/

/************************** End Internal Functions *************************/

/*===========================================================================

FUNCTION MCCBCSMS_INIT

DESCRIPTION
  Initialize the broadcast SMS service and its related variables.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccbcsms_init (void)
{
  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  int   i;
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
  mccbcsms_pages.bc_type            = BROADCAST_PAGE;
  mcc_bc_sms_enable                 = FALSE;

  mccbcsms_config.bcsms_enable      = FALSE;
  mccbcsms_config.bcast_init        = FALSE;
  mccbcsms_config.ovhd_chan         = CAIX_SC;  /* Start with Sync */
  mccbcsms_config.bcast_index       = 0xFF;
  mccbcsms_config.bcast_slot_cycle  = 0;
  mccbcsms_config.cancel_bcast      = FALSE;

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  memset( &mccbcsms_config.enbcsms_info, 0,
          sizeof(mccbcsms_config.enbcsms_info)); /*lint !e419 */
  for (i = 0; i < CAI_MAX_NON_PRI_BCCH_CHAN; i++ )
  {
    qw_set(mccbcsms_prev_fccch_ebc_slots[i], 0, 0 );
  }
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  qw_set(mccbcsms_old_pch_bc_slot, 0, 0);
  mcc_bcsms_in_progress = FALSE;
  qw_set(mcc_bcsms_start_slot, 0, 0);

} /* mccbcsms_init */

/*===========================================================================

FUNCTION MCCBCSMS_RESET_BC_PAGES

DESCRIPTION
  Reset the (enhanced) broadcast pages and schedules.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccbcsms_reset_bc_pages (void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch ( mccbcsms_config.ovhd_chan )
  {
    case CAIX_SC:
      /* Just completed BCSMS initialization and not yet rxed ESPM or MCRRPM.
         Treat it as PC. */
    case CAIX_PC:
      mccbcsms_pages.bc_type  = BROADCAST_PAGE;
      /* the next BC Msg index in page_recs to be received */
      mccbcsms_pages.pages.bc.nxt_rec_index     = 0;
      /* total number of msg bursts to be received */
      mccbcsms_pages.pages.bc.tot_bc_msg        = 0;
      break;

    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    case CAIX_BCCH:
      /* Initialize the Enhanced Broadcast SMS services */
      mccbcsms_init_ebc_pages();
      break;
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

    default:
      MSG_ERROR(" Unrecognized ovhd_chan %d", mccbcsms_config.ovhd_chan, 0, 0);
      break;

  } /* switch */

} /* mccbcsms_reset_bc_pages */

/*===========================================================================

FUNCTION MCCBCSMS_PROC_MSG

DESCRIPTION
  This function process the System Param Message, Extended System Param
  Message, and the MCRR Param Message for the Broadcast SMS related
  parameters. It checks to see if the Broadcast SMS configuration has
  been changed. If so, it updates the broadcast slot cycle and resets
  the related broadcast schedules.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mccbcsms_proc_msg
(
  byte              msg_type,
  cdma_bs_type      *new_bs_config
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (msg_type)
  {
    case CAI_SYS_PARM_MSG:
      /* -------------------------
      ** System Parameters Message
      ** ------------------------- */
      /* This is called when the System Param Message indicates */
      /* that no Ext. System Param Message is available in this */
      /* Paging Channel (EXT_SYS_PARAMETER field set to '0'.    */
      /* According to IS-95A, 6.6.2.2.1.1, set the bcast_index  */
      /* to MAX_SLOT_CYCLE_INDEX.                               */

      if ( new_bs_config->csp.sp.ext_sys_parameter )
      {
        break;
      }
      else
      {
        new_bs_config->csp.esp.bcast_index =
            new_bs_config->csp.sp.max_slot_cycle_index;
        MSG_HIGH("SPM w/o ESPM, bcast_index=max_sci", 0, 0, 0);
      }

      /* Fall Through */
      /*lint -fallthrough */

    case CAI_EXT_SYS_PARM_MSG:
      /* ----------------------------------
      ** Extended System Parameters Message
      ** ---------------------------------- */

      if (mccbcsms_config.ovhd_chan != CAIX_PC ||
          !mccbcsms_config.bcast_init ||
          mccbcsms_config.bcast_index != new_bs_config->csp.esp.bcast_index)
      {
        mccbcsms_config.bcast_init  = TRUE;
        mccbcsms_config.ovhd_chan   = CAIX_PC;
        mccbcsms_config.bcast_index = new_bs_config->csp.esp.bcast_index;

        /* broadcast slot cycle = 2^bcast_index x 16 + 3 */
        if (new_bs_config->csp.esp.bcast_index != 0)
        {
          mccbcsms_config.bcast_slot_cycle =
              (16L << (new_bs_config->csp.esp.bcast_index)) + 3;  /*lint !e734 */
        }
        else
        {
          mccbcsms_config.bcast_slot_cycle = 0;
        }

        /* Reset the enhanced broadcast page structure. */
        mccbcsms_reset_bc_pages();

        /* Init the dup list */
        mccbcsms_unique_recs.list_begin     = 0;
        mccbcsms_unique_recs.tot_entries    = 0;

        if (mccidl_is_fast_raho())
        {
          /* Fast RAHO, remember to cancel the bcast */
          mccbcsms_config.cancel_bcast = TRUE;
          MSG_HIGH("Fast RAHO: don't send cmd to search", 0, 0, 0);
        }
        else
        {
          /* Cancel the broadcast schedule to Search */
          mccbcsms_cancel_bcast();
        }
      }
      MSG_MED("(E)SPM, bcast_index=%d, bc_enable=%d",
              new_bs_config->csp.esp.bcast_index,
              mccbcsms_config.bcsms_enable, 0);
      break;

    #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
    case CAI_MC_RR_PARM_MSG:
      /* --------------------------------------
      ** IS2000 Rel A MC-RR Parameters Message
      ** ------------------------------------- */

      /* If bcast_index has changed, or the BCCH channel config */
      /* has changed, we need to reset the bcast slot cycle.    */
      if (mccbcsms_ebc_config_changed(new_bs_config))
      {
        /* Save the broadcast configuration */
        mccbcsms_config.ovhd_chan   = CAIX_BCCH;
        mccbcsms_config.bcast_init  = TRUE;
        mccbcsms_config.bcast_index = new_bs_config->csp.esp.bcast_index;
        mccbcsms_config.enbcsms_info = new_bs_config->csp.bcast_bcch_info;

        /* broadcast_slot_cycle in IS2000 Rel A Common Channel is:
         * slot cycle = 2^(bcast_index + 1) x 16 + 7 */
        if (new_bs_config->csp.esp.bcast_index != 0)
        {
          mccbcsms_config.bcast_slot_cycle =
              (16L << (mccbcsms_config.bcast_index + 1)) + 7;  /*lint !e734 */
        }
        else
        {
          mccbcsms_config.bcast_slot_cycle = 0;
        }

        /* Reset the enhanced broadcast page structure. */
        mccbcsms_reset_bc_pages();

        /* Init the dup list */
        mccbcsms_unique_recs.list_begin     = 0;
        mccbcsms_unique_recs.tot_entries    = 0;

        if (mccidl_is_fast_raho())
        {
          /* Fast RAHO, remember to cancel the bcast */
          mccbcsms_config.cancel_bcast = TRUE;
          MSG_HIGH("Fast RAHO: don't send cmd to search", 0, 0, 0);
        }
        else
        {
          /* Cancel the broadcast schedule to Search */
          mccbcsms_cancel_bcast();
        }
      }
      MSG_MED("MCRRM, bcast_index=%d, bc_enable=%d",
              new_bs_config->csp.esp.bcast_index,
              mccbcsms_config.bcsms_enable, 0);
      break;
    #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

    default:
      MSG_ERROR("Unrecognized msg type %d", msg_type, 0, 0);
      break;
  } /* end switch */

} /* mccbcsms_proc_msg */

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*===========================================================================

FUNCTION MCCBCSMS_PROC_SRCH_RPT

DESCRIPTION
  This function process the Searcher's report, the wakeup report. It
  updates the Broadcast SMS related variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -e818*/
void mccbcsms_proc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (rpt_ptr->hdr.rpt)
  {
    case SRCH_WAKEUP_R:

      if ( mccbcsms_config.ovhd_chan == CAIX_BCCH )
      {
        if (( rpt_ptr->wakeup.cur_slot_mask & BC_DATA_SLOT) != 0 )
        {
          int i;

          /* If this is the BCCH data slot, store the monitored BCNs */
          for ( i = 0;
                ((i < CAI_MAX_NON_PRI_BCCH_MONITOR) &&
                 (rpt_ptr->wakeup.bcn[i] != 0));
                i++ )
          {
            mccbcsms_pages.pages.ebc.monitor_bcn[i] = rpt_ptr->wakeup.bcn[i];
            MSG_MED("Srch_wakeup_r: bcn %d",
                    mccbcsms_pages.pages.ebc.monitor_bcn[i], 0, 0);
          }
          mccbcsms_pages.pages.ebc.num_non_pri_bcch_monitor = i; //lint !e734

          if (mccbcsms_pages.pages.ebc.num_non_pri_bcch_monitor == 0)
          {
            /* No valid BCN is included, assume one */
            MSG_ERROR("Srch_wakeup_r: no valid BCN", 0, 0, 0);
            mccbcsms_pages.pages.ebc.num_non_pri_bcch_monitor = 1;
          }
        } /* cur_slot_mask & BC_DATA_SLOT */
      } /* ovhd_chan == BCCH */
      break;

    default:
      MSG_ERROR("Wrong srch rpt type %d", rpt_ptr->hdr.rpt, 0, 0);
      break;
  } /* switch */

} /* mccbcsms_proc_srch_rpt */
/*lint +e818*/
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

/*===========================================================================

FUNCTION MCCBCSMS_ENABLE

DESCRIPTION
  This function enables or disables the broadcast SMS depends on the input.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mccbcsms_enable
(
  boolean bcsms_enable
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (mccbcsms_config.bcsms_enable != bcsms_enable)
  {
    mccbcsms_config.bcsms_enable =
    mcc_bc_sms_enable            = bcsms_enable;

    if (mccbcsms_config.bcsms_enable)
    {
      mccbcsms_reset_bc_pages();

      /* Don't reset the unique bcast list variables, because   */
      /* they contain the time stamp, which might be valid if   */
      /* bcsms happens to be enabled again.                     */
    }
    else
    {
      /* BCSMS is disabled. Cancel the broadcast schedule */
      mccbcsms_cancel_bcast();
    }

    /* If bcsms_enable changed, CP may need to update the */
    /* bcast_index value to Search, by sending a parm cmd */
    mccidl_send_srch_parms();
  }
} /* mccbcsms_enable */

/*===========================================================================

FUNCTION MCCBCSMS_IS_ENABLED

DESCRIPTION
  This function returns if the Broadcast SMS is enabled or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if Broadcast SMS is enabled.
  FALSE, if disabled.

SIDE EFFECTS

===========================================================================*/
boolean mccbcsms_is_enabled( void )
{
  return mccbcsms_config.bcsms_enable;

} /* mccbcsms_is_enabled */

/*===========================================================================

FUNCTION MCCBCSMS_IS_BCAST_INIT

DESCRIPTION
  This procedure checks wheter broadcast info has been initialized.

DEPENDENCIES
  None

RETURN VALUE
  Returns bcast_init.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccbcsms_is_bcast_init(void)
{
  return (mccbcsms_config.bcast_init);

} /* mccbcsms_is_bcast_init */

/*===========================================================================

FUNCTION MCCBCSMS_IS_BC_PAGE_SLOT

DESCRIPTION
  This function returns whether the given frame is a broadcast page slot. A
  paging slot is a broadcast slot, if it's the first or second slot within
  the current broadcast cycle.
  It takes the frame when the message is received as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it's a broadcast page slot

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_is_bc_page_slot
(
  qword msg_frame_20        /* Msg frame, in 20 ms */
)
{
  qword tmp_qw;
  word  bc_slot_pos;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( mccbcsms_config.bcsms_enable && mccbcsms_config.bcast_slot_cycle != 0)
  {
    qw_equ(tmp_qw, msg_frame_20);
    qw_shift(tmp_qw, -2);       /* Convert to 80 ms */

    bc_slot_pos = qw_div(tmp_qw, tmp_qw, mccbcsms_config.bcast_slot_cycle);

    /* the current slot is a broadcast slot if it is within the first two
     * slots of a broadcast cycle */
    return (bc_slot_pos==0 || bc_slot_pos==1);
  }
  else
  {
    /* multi-slotted mode */
    return FALSE;
  }

} /* mccbcsms_is_bc_page_slot */

/*===========================================================================

FUNCTION MCCBCSMS_IS_BC_DATA_SLOT

DESCRIPTION
  This function returns whether the given frame is a scheduled broadcast
  slot to receive data burst.
  It takes the frame when the message is received as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it's a broadcast data slot

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_is_bc_data_slot
(
  qword msg_frame_20        /* Msg frame, in 20 ms */
)
{
  boolean   is_bc_slot = FALSE;
  qword     tmp_qw;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if a broadcast msg has been scheduled and the current slot is
   * the first slot of the scheduled wakeup slots. */
  if (mccbcsms_config.ovhd_chan == CAIX_PC)
  {
    byte      nxt_rec_index;

    if (mccbcsms_config.bcsms_enable &&
        mccbcsms_pages.pages.bc.tot_bc_msg != 0 &&
        mccbcsms_pages.pages.bc.nxt_rec_index != 0 &&
        mccbcsms_config.bcast_slot_cycle != 0 )
    {
      /* Set nxt_rec_index to the index of the scheduled BCSMS */
      nxt_rec_index = mccbcsms_pages.pages.bc.nxt_rec_index - 1;

      qw_equ(tmp_qw, msg_frame_20);
      qw_shift(tmp_qw, -2);       /* 80 ms slot */
    
      /* We need to skip the BCSMS whose wakeup slot has passed */
      while ((nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg)
             &&
              (qw_cmp(tmp_qw, mccbcsms_pages.pages.bc.page_recs
                 [nxt_rec_index].wakeup_slot) > 0)
            )
      {
        nxt_rec_index++;
        MSG_MED("Wakeup slot for BCSMS num %d already passed before slot 0x%x",
                nxt_rec_index, qw_lo(msg_frame_20), 0);
      }
      /* The message pointed to by mccbcsms_pages.pages.bc.nxt_rec_index is
       * the one for which current slot could be a start slot */ 
  
      is_bc_slot =
          !qw_cmp(mccbcsms_pages.pages.bc.page_recs
                      [nxt_rec_index].wakeup_slot, tmp_qw );
    }
    else
    {
      is_bc_slot = FALSE;
    }
  }

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  else /* ovhd_chan == BCCH */
  {
    int                 i;
    enh_bc_page_type    *tmp_ptr;

    if (mccbcsms_config.bcsms_enable)
    {
      qw_equ(tmp_qw, msg_frame_20);
      qw_shift(tmp_qw, -1);     /* 40 ms units */

      for (i = 0; i < mccbcsms_config.enbcsms_info.num_bcch_bcast; i++)
      {
        tmp_ptr = mccbcsms_pages.pages.ebc.page_ptrs[i];
        if (tmp_ptr)
        {
          if (qw_cmp(tmp_qw, tmp_ptr->data_slot_40) == 0)
          {
            is_bc_slot = TRUE;
            break;
          }
        }
      }
    }
    else
    {
      is_bc_slot = FALSE;
    }
  }
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  return (is_bc_slot);

} /* mccbcsms_is_bc_data_slot */

/*===========================================================================

FUNCTION MCCBCSMS_PC_MATCH

DESCRIPTION
  This function parse the *bcast_hdr received on the Paging channel to see
  if it is a BC message header that contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *pc_hdr contains
  enough useful bits (at least CAII_BC_HDR_LEN bits). And the current slot
  is the scheduled BC SMS slot.

RETURN VALUE
  TRUE if it contains the expected BC_ADDR, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mccbcsms_pc_match
(
  byte * bcast_hdr
)
{
  word msg_pos;
  caii_pc_hdr_type bc_hdr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( b_unpackb( bcast_hdr,
                  FPOS( cai_gen_type, msg_type ), /*lint !e734 */
                  FSIZ( cai_gen_type, msg_type )
                 ) != CAI_PC_BURST_MSG )
  {
    /* if the MSG_TYPE is not Data Burst Message, it's not a BC SMS */
    return FALSE;
  }
  msg_pos = FSIZ(cai_pc_hdr_type, msg_type);
  (void) xlate_ext_pc_hdr(&msg_pos, bcast_hdr, &bc_hdr);
  /* now we have the parsed header, we need to look the contents */
  if ((bc_hdr.addr_type == CAI_BCAST_ADDR_TYPE) &&
      (cm_wms_bc_addr_match(bc_hdr.addr.type5.bc_addr))
     )
  {
    /* we have a BCAST address type and BC_ADDR also matches, so it's the one */
    MSG_MED("BCSMS hdr match",0,0,0);
    return TRUE;
  }
  else
  {
    MSG_MED("BCSMS hdr not match",0,0,0);
    return FALSE;
  }

} /* mccbcsms_pc_match */

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*===========================================================================

FUNCTION MCCBCSMS_BCCH_MATCH

DESCRIPTION
  This function parse the *bcast_hdr received on the BCCH to see
  if it is a BC message header that contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *bcast_hdr contains
  enough useful bits (at least CAII_BC_HDR_LEN bits). And the current slot
  is the scheduled BC SMS slot.

RETURN VALUE
  The message length if this is a matched Broadcast message
  Otherwise, 0 is returned.

SIDE EFFECTS
  None

===========================================================================*/
uint16 mccbcsms_bcch_match
(
  byte * bcast_hdr
)
{
  typedef PACKED struct
  {
    byte sci[1];
    byte ext_msg_len_ind[1];
    byte msg_len[7];
    byte ext_msg_len[15];
    byte pd[2];
    byte msg_id[6];
  } mccbcsms_bcch_hdr_type;

  uint16            msg_len = 0;
  uint16            msg_pos = 0;
  boolean           bc_match = FALSE;
  boolean           is_ext_msg_len;
  byte              msg_id;
  caii_pc_hdr_type  bc_hdr;

/*-------------------------------------------------------------------------*/

  /* Get the SCI bit, and has to be 1 */
  if (b_unpackb( bcast_hdr, msg_pos, FSIZ(mccbcsms_bcch_hdr_type, sci)))
  {
    msg_pos += FSIZ(mccbcsms_bcch_hdr_type, sci);
  }
  else
  {
    /* SCI is 0, invalid message - should not happen */
    MSG_ERROR("SCI = 0", 0, 0, 0);
    return 0;
  }

  /* Get the ext_msg_len_ind bit */
  is_ext_msg_len = b_unpackb( bcast_hdr, msg_pos,
                              FSIZ(mccbcsms_bcch_hdr_type, ext_msg_len_ind));
  msg_pos += FSIZ(mccbcsms_bcch_hdr_type, ext_msg_len_ind);

  if ( is_ext_msg_len )
  {
    msg_len = b_unpackw( bcast_hdr, msg_pos,
                         FSIZ(mccbcsms_bcch_hdr_type, ext_msg_len));
    msg_pos += FSIZ(mccbcsms_bcch_hdr_type, ext_msg_len);
  }
  else
  {
    msg_len = b_unpackb( bcast_hdr, msg_pos,
                         FSIZ(mccbcsms_bcch_hdr_type, msg_len));
    msg_pos += FSIZ(mccbcsms_bcch_hdr_type, msg_len);
  }

  /* Check if the MSG_TYPE is Data Burst Message */
  msg_pos += FSIZ(mccbcsms_bcch_hdr_type, pd);
  msg_id = b_unpackb( bcast_hdr, msg_pos, FSIZ(mccbcsms_bcch_hdr_type, msg_id));
  if (msg_id != CAI_PC_BURST_MSG )
  {
    MSG_ERROR("Msg %d not DBM", msg_id, 0, 0);
    return 0;
  }
  msg_pos += FSIZ(mccbcsms_bcch_hdr_type, msg_id);

  /* Now xlate the broadcast data burst hdr fields */
  (void) xlate_ext_pc_hdr(&msg_pos, bcast_hdr, &bc_hdr);

  /* now we have the parsed header, we need to look the contents */
  if (bc_hdr.addr_type == CAI_BCAST_ADDR_TYPE)
  {
    if (cm_wms_bc_addr_match(bc_hdr.addr.type5.bc_addr))
    {
      bc_match = TRUE;
    }
  } /* if addr_type == CAI_BCAST_ADDR_TYPE */
  else
  {
    MSG_ERROR("Not a bcast addr type %d", bc_hdr.addr_type, 0, 0);
  }

  if (bc_match)
  {
    MSG_MED("BCCH BCSMS hdr match, len %d", msg_len, 0, 0);
    return msg_len;
  }
  else
  {
    MSG_MED("BCCH BCSMS hdr not match", 0, 0, 0);
    return 0;
  }

} /* mccbcsms_bcch_match */
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

/*===========================================================================

FUNCTION MCC_BC_MATCH

DESCRIPTION
  This function parse the *bcast_hdr to see if it is a BC message header that
  contains the expected BC_ADDR.

DEPENDENCIES
  It depends on the calling function to made sure that *bcast_hdr contains
  enough useful bits (at least CAII_BC_HDR_LEN bits). And the current slot
  is the scheduled BC SMS slot.

RETURN VALUE
  TRUE if it contains the expected BC_ADDR, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
uint16 mcc_bc_match(byte * bcast_hdr)
{
  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  if (mccbcsms_config.ovhd_chan == CAIX_BCCH)
  {
    return mccbcsms_bcch_match(bcast_hdr);
  }
  else
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */
  {
    mcc_bcsms_in_progress = mccbcsms_pc_match(bcast_hdr);

    if(mcc_bcsms_in_progress)
    {
      ts_get_20ms_frame_time( mcc_bcsms_start_slot); // 20ms frame
      qw_shift(mcc_bcsms_start_slot, -2);  // 80ms slot
      MSG_MED("BCSMS started in slot 0x%x:%x",qw_hi(mcc_bcsms_start_slot), 
              qw_lo(mcc_bcsms_start_slot), 0);
    }
    else
    {
      qw_set(mcc_bcsms_start_slot, 0, 0);
    }
    return (mcc_bcsms_in_progress);
  }

} /* mcc_bc_match */

/*===========================================================================

FUNCTION MCCBCSMS_SCHEDULE_BCAST

DESCRIPTION
  This function schedules the Broadcast SMS by finding the next Broadcast
  wakeup slot and sending them to the Searcher through SRCH_BC_INFO_F.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_schedule_bcast(void)
{
  qword     cur_slot;
  boolean   update_schedule = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do nothing if BCSMS is not enabled. */
  if ( !mccbcsms_config.bcsms_enable )
  {
    return;
  }

  /* get the current time */
  ts_get_20ms_frame_time(cur_slot);

  if (mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    qw_shift(cur_slot, -2); /* cur_time >> 2 to get 80 ms slot number */
    
    while ((mccbcsms_pages.pages.bc.nxt_rec_index <
            mccbcsms_pages.pages.bc.tot_bc_msg     )
           &&
            (qw_cmp(cur_slot, mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot) >= 0)
          )
    {
      /* We need to ignore the wakeup slots that already passed */
      mccbcsms_pages.pages.bc.nxt_rec_index++;
      MSG_MED("Ignore BC SMS page num %d",
              mccbcsms_pages.pages.bc.nxt_rec_index, 0,0);
    }

    if ((mccbcsms_pages.pages.bc.nxt_rec_index > 0)
        &&
        (qw_cmp(cur_slot, mccbcsms_pages.pages.bc.page_recs
             [mccbcsms_pages.pages.bc.nxt_rec_index - 1].wakeup_slot) < 0)
       )
    {
      /* if after scheduling the last bc, the MS didn't goto sleep,
       * start_sleep() will be repeatedly called. in this case, we can't
       * increment the record index */
      mccbcsms_pages.pages.bc.nxt_rec_index--;
    }

    /* If the current top records are duplicated with the received
     * ones, do not schedule the wake up for those records. Keeps searching
     * until a non-duplicated record is found, or no more records to be checked */
    if (mccbcsms_config.bcast_slot_cycle != 0)
    {
      /* the duplication cycle need be in unit of 80ms as required by 
       * the fucntion mccbcsms_detect_duplicate */
      dword dup_cycle = (mccbcsms_config.bcast_slot_cycle << 2);

      /* get the current slot for duplication timing check */
      dword current_slot = qw_lo(cur_slot); 

      while (mccbcsms_pages.pages.bc.nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg )
      {
        /* Check if the record pointed by the index is duplicated with the received ones */
        if (! mccbcsms_detect_duplicate(CAI_SHORT_MSG_SERVICES,
                                        mccbcsms_pages.pages.bc.page_recs[mccbcsms_pages.pages.bc.nxt_rec_index].bc_addr,
                                        current_slot,     /* 80ms unit */
                                        dup_cycle))       /* 80ms unit */
        {
          /* stop the loop once a non-duplicated record is found, and schedule this
           * record at the end */
          MSG_MED("No duplication found for BC SMS page num=%d",
                  (mccbcsms_pages.pages.bc.nxt_rec_index + 1), 0,0);
          break;
        }
        else
        {
          MSG_MED("No wakeup schedule for duplicated BC SMS page num=%d",
                  (mccbcsms_pages.pages.bc.nxt_rec_index + 1), 0,0);
          /* if the current one is a duplicated one, continue to check the next one
           * until a non-duplicated record is found, or there is no more to check */
          mccbcsms_pages.pages.bc.nxt_rec_index++;
        }
      } /* while loop */
    } /* if (mccbcsms_config.bcast_slot_cycle != 0) */ 

    /* looking into mccbcsms_page_recs[] */
    if (mccbcsms_pages.pages.bc.nxt_rec_index <
        mccbcsms_pages.pages.bc.tot_bc_msg      )
    {
      if (qw_cmp(mccbcsms_old_pch_bc_slot,
                 mccbcsms_pages.pages.bc.page_recs
                   [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot) != 0)
      {
        /* If the old schedule changes, update Search by SRCH_BC_INFO_F */
        update_schedule = TRUE;
        mcc_srch_buf.bc.monitor_bcch = FALSE;
        qw_equ(mcc_srch_buf.bc.next_bcast_slot.time,
               mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot);
        MSG_MED("BC_INFO 0x%x (40ms) 0x%x (80ms)",
                (qw_lo(mcc_srch_buf.bc.next_bcast_slot.time) & 0xFF),
                ((qw_lo(mcc_srch_buf.bc.next_bcast_slot.time) >> 1) & 0xFF),
                0);

        /* Save the old bc schedule */
        qw_equ(mccbcsms_old_pch_bc_slot,
               mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot);
      }
      /* Update the index */
      mccbcsms_pages.pages.bc.nxt_rec_index++;
    }
    else
    {
      /* all the broadcast msgs are finished */
      mccbcsms_pages.pages.bc.tot_bc_msg = 0;
      mccbcsms_pages.pages.bc.nxt_rec_index = 0;
      MSG_MED("All BC page done", 0, 0, 0);
    }
  } /* ovhd_chan == PCH */

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  else    /* ovhd_chan == BCCH */
  {
    int                 i;
    enh_bc_page_type    *tmp_ptr;
    boolean             next_bcast = FALSE;

    qw_shift(cur_slot, -1); /* cur_time >> 1 to get 40ms number */

    /* First, remove all the expired EBC pages. */
    mccbcsms_delete_expired_ebc_pages( cur_slot );

    for ( i = 0; i < mccbcsms_config.enbcsms_info.num_bcch_bcast; i++ )
    {
      /* If this data slot is greater than the previous sent schedule, update
      ** search with this data slot
      */
      if ((mccbcsms_pages.pages.ebc.page_ptrs[i] != NULL ) &&
          (qw_cmp ( mccbcsms_prev_fccch_ebc_slots[i],
                    mccbcsms_pages.pages.ebc.page_ptrs[i]->data_slot_40 ) < 0 ))
      {
        MSG_MED("prev[%d] 0x%x < cur 0x%x", i, 
                 qw_lo(mccbcsms_prev_fccch_ebc_slots[i]), 
                 qw_lo(mccbcsms_pages.pages.ebc.page_ptrs[i]->data_slot_40));

        update_schedule = TRUE;
        break;
      }
    }

    if (update_schedule)
    {
      /* Clear the buffer */
      memset((void *)&mcc_srch_buf.bc.next_bcast_slot.bcch,
             0,
             sizeof(mcc_srch_buf.bc.next_bcast_slot.bcch));

      mcc_srch_buf.bc.monitor_bcch = TRUE;

      for ( i = 0; i < mccbcsms_config.enbcsms_info.num_bcch_bcast; i++ )
      {
        /* Now, the first entry of the page_ptrs is the     */
        /* earliest BC data slot to wakeup for this bcn     */
        tmp_ptr = mccbcsms_pages.pages.ebc.page_ptrs[i];

        if (tmp_ptr != NULL)
        {
          qw_equ(mcc_srch_buf.bc.next_bcast_slot.bcch.bcast_slot[i].time,
                 tmp_ptr->data_slot_40);
          mcc_srch_buf.bc.next_bcast_slot.bcch.bcast_slot[i].walsh_chn =
                 mccbcsms_config.enbcsms_info.bcch_info[i].bcch_code_chan;
          mcc_srch_buf.bc.next_bcast_slot.bcch.bcast_slot[i].rate =
                 mccbcsms_config.enbcsms_info.bcch_info[i].brat;
          mcc_srch_buf.bc.next_bcast_slot.bcch.bcast_slot[i].c_rate =
                 mccbcsms_config.enbcsms_info.bcch_info[i].bcch_code_rate;
          next_bcast = TRUE;

          MSG_MED("nxt ebc data 0x%x bcn %d from cur 0x%x",
                   (qw_lo(tmp_ptr->data_slot_40) & 0xFF),
                   (i + 2),
                   ((qw_lo(tmp_ptr->data_slot_40) - qw_lo(cur_slot)) & 0xFF));

          /* Save the schedule */
          qw_equ ( mccbcsms_prev_fccch_ebc_slots[i], tmp_ptr->data_slot_40 );
        }
        else
        {
          MSG_MED("No ebc schedule for bcn %d", i + 2, 0, 0);
        }
      } /* for */

      if ( next_bcast )
      {
        /* Set the num_bcch to the total number of BCCH BCAST. If there is */
        /* no bcast schedule on this BCN, the time will be set to 0.       */
        mcc_srch_buf.bc.next_bcast_slot.bcch.num_bcch =
            mccbcsms_config.enbcsms_info.num_bcch_bcast;
      }
      else
      {
        mcc_srch_buf.bc.next_bcast_slot.bcch.num_bcch = 0;
      }
    } /* update schedule */
  } /* ovhd_chan == BCCH */
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  /* Send the BC Info command to Search */
  if (update_schedule)
  {
    MSG_MED("BC_INFO to srch", 0, 0, 0);
    mcc_srch_buf.bc.hdr.cmd = SRCH_BC_INFO_F;
    mcc_srch_cmd(&mcc_srch_buf);
  }
  else
  {
    MSG_MED("BC Schedule no change", 0, 0, 0);
  }

} /* mccbcsms_schedule_bcast */



/*===========================================================================

FUNCTION MCCBCSMS_SCHED_BCAST_UPON_WAKEUP

DESCRIPTION
  This function schedules the Broadcast SMS upon wakeup, by finding the 
  next Broadcast wakeup slot and sending them to the Searcher through 
  SRCH_BC_INFO_F.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_sched_bcast_upon_wakeup
(
  qword cur_time_20ms /* in 20ms */
)
{
  qword     cur_slot;
  boolean   update_schedule = FALSE;
  byte      nxt_rec_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do nothing if BCSMS is not enabled. */
  if ( !mccbcsms_config.bcsms_enable )
  {
    return;
  }

  qw_equ(cur_slot, cur_time_20ms);

  if (mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    qw_shift(cur_slot, -2); /* cur_time >> 2 to get 80 ms slot number */
    
    nxt_rec_index = mccbcsms_pages.pages.bc.nxt_rec_index;

    while ((nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg )
           &&
           (qw_cmp(cur_slot, 
              mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot)
            >= 0)
          )
    {
      /* We need to ignore the wakeup slots that already passed */
      nxt_rec_index++;
    }

    /* looking into mccbcsms_page_recs[] */
    if (nxt_rec_index < mccbcsms_pages.pages.bc.tot_bc_msg )
    {
      if (qw_cmp(mccbcsms_old_pch_bc_slot,
                 mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot)
          != 0)
      {
        /* If the old schedule changes, update Search by SRCH_BC_INFO_F */
        update_schedule = TRUE;
        mcc_srch_buf.bc.monitor_bcch = FALSE;
        qw_equ(mcc_srch_buf.bc.next_bcast_slot.time,
               mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot);
        MSG_MED("BC_INFO to srch 0x%x (40ms) 0x%x (80ms)",
                (qw_lo(mcc_srch_buf.bc.next_bcast_slot.time) & 0xFF),
                ((qw_lo(mcc_srch_buf.bc.next_bcast_slot.time) >> 1) & 0xFF),
                0);

        /* Save the old bc schedule */
        qw_equ(mccbcsms_old_pch_bc_slot,
               mccbcsms_pages.pages.bc.page_recs[nxt_rec_index].wakeup_slot);
      }
    }
  } /* ovhd_chan == PCH */

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  else    /* ovhd_chan == BCCH */
  {
    /* If the similar issue happens on the Rel A BCCH, we need to 
     * consider the fix here also. */
  }
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  /* Send the BC Info command to Search */
  if (update_schedule)
  {
    mcc_srch_buf.bc.hdr.cmd = SRCH_BC_INFO_F;
    mcc_srch_cmd(&mcc_srch_buf);
  }

} /* mccbcsms_sched_bcast_upon_wakeup */



/*===========================================================================

FUNCTION MCCBCSMS_CANCEL_BCAST

DESCRIPTION
  This function cancels the previous broadcast schedule to Search.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_cancel_bcast(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mcc_srch_buf.bc.hdr.cmd = SRCH_BC_INFO_F;

  if (mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    mcc_srch_buf.bc.monitor_bcch = FALSE;
    qw_set(mcc_srch_buf.bc.next_bcast_slot.time, 0, 0);
  }
  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  else    /* ovhd_chan == BCCH */
  {
    mcc_srch_buf.bc.monitor_bcch = TRUE;
    mcc_srch_buf.bc.next_bcast_slot.bcch.num_bcch = 0;
    /* Clear the buffer */
    memset((void *)&mcc_srch_buf.bc.next_bcast_slot.bcch,
           0,
           sizeof(mcc_srch_buf.bc.next_bcast_slot.bcch));
  }
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  mcc_srch_cmd(&mcc_srch_buf);

} /* mccbcsms_cancel_bcast */

/*===========================================================================

FUNCTION MCCBCSMS_POST_FRAHO_PROC

DESCRIPTION
  This function performes the actions suppressed by the fast RAHO.
  If the bcast_index or non-primary BCCH channel configuration has 
  been changed during fast RAHO, it sends the canceling of bcast 
  schedule to Search. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void mccbcsms_post_fraho_proc(void)
{
  /* Check if we need to cancel the broadcast schedule */
  if (mccbcsms_config.cancel_bcast)
  {
    MSG_MED("Wakeup, cancel bcast to search", 0, 0, 0);
    mccbcsms_cancel_bcast();
    mccbcsms_config.cancel_bcast = FALSE;
  } 
}

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*===========================================================================

FUNCTION MCCBCSMS_GET_BCN

DESCRIPTION
  This function gets the currently monitored first BCN

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
byte mccbcsms_get_bcn(void)
{
  return mccbcsms_pages.pages.ebc.monitor_bcn[0];

} /* mccbcsms_get_bcn */

/*===========================================================================

FUNCTION MCCBCSMS_CHECK_IMMED_EBC_SCHED

DESCRIPTION
  This function checks if there are any enhanced broadcast schedulings in 
  the current time_offset or the one that immediately follows.

DEPENDENCIES
  None.

RETURN VALUE
  TURE, if there are some enhanced broadcast schedulings.
  FALSE, otherwise.

SIDE EFFECTS

===========================================================================*/
boolean mccbcsms_check_immed_ebc_sched(void)
{
  int                 i;
  enh_bc_page_type    *tmp_ptr;
  qword               tmp_qw;
  boolean             continue_monitoring = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ts_get_20ms_frame_time(tmp_qw);
  qw_shift(tmp_qw, -1); /* tmp_qw >> 1 to get 40 ms number */
  
  for (i = 0; i < mccbcsms_pages.pages.ebc.num_non_pri_bcch_monitor; i++)
  {
    tmp_ptr = mccbcsms_pages.pages.ebc.page_ptrs
                [mccbcsms_pages.pages.ebc.monitor_bcn[i]-2];

    /* Skip the expired ones */
    while (tmp_ptr && (qw_cmp(tmp_qw, tmp_ptr->data_slot_40) > 0))
    {
      tmp_ptr = tmp_ptr->next_ptr;
    }

    if (tmp_ptr)
    {
      if (qw_cmp(tmp_qw, tmp_ptr->data_slot_40) == 0)
      {
        MSG_HIGH("EBC pending on cur frm, cont monitoring", 0, 0, 0);
        continue_monitoring = TRUE;
        break;
      }
      else
      {
        /* Check the time offset that immediately follows the*/
        /* current one. We don't have enough time to go      */
        /* through the sleep and wakeup cycle during 40ms    */
        /* time frame.                                       */
        qw_inc(tmp_qw, 1);
        if (qw_cmp(tmp_qw, tmp_ptr->data_slot_40) == 0)
        {
          MSG_HIGH("EBC nxt frame, cont monitorng", 0, 0, 0);
          continue_monitoring = TRUE;
          break;
        }
      }
    } /* tmp_ptr */
  } /* for */

  return continue_monitoring;

} /* mccbcsms_check_immed_ebc_sched */

#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

/*===========================================================================

FUNCTION MCCBCSMS_COMP_ADDR

DESCRIPTION
  This function does necessary processing when receiving a broadcast message.
  It compares the BC address and checks if this is an expected broadcast
  message corresponding to the outstanding schedules. It also checks if it's
  a duplicate. If it's unique, it then inserts the broadcast message into
  the unique bcast list so that later broadcast pages and messages can
  perform the duplicate detection agaist it.

  This function is called from mcc_comp_addr().

  Please note the processings for the Paging Ch and BCCH/FCCCH are different.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if a valid broadcast address is found and the broadcast message is
  unique. FALSE otherwise.

SIDE EFFECTS
  It updates the bc_unique_entries.

===========================================================================*/
boolean mccbcsms_comp_addr
(
  caii_pc_hdr_type *msg_hdr,
    /* Header of paging channel message to match address of */
  qword msg_frame_20
    /* Msg frame, in 20 ms */
)
{
  boolean addr_found = FALSE;
  qword tmp_qw;
  dword current_slot;       /* for inserting to the dup list */
  dword dup_cycle;          /* dup detection cycle, in 80ms  */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!mccbcsms_config.bcsms_enable)
  {
    /* we don't look at the addr if the feature is disabled */
    return FALSE;
  }

  /* If we receive a broadcast SMS before initializing the bcast */
  /* index, don't perform the dup detection. Instead we simply   */
  /* perform the address matching and return                     */
  if ( !mccbcsms_config.bcast_init )
  {
    MSG_HIGH("Received a BCSMS before Overhead", 0, 0, 0);

    if (cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr))
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  /* Calculate the duplication detection cycle */
  if (mccbcsms_config.bcast_slot_cycle != 0)
  {
    dup_cycle = mccbcsms_config.bcast_slot_cycle << 2;
  }
  else
  {
    dup_cycle = 16L<<cur_bs_ptr->csp.sp.max_slot_cycle_index;
  }

  /* Perform the broadcast address matching for the Paging channel */
  if ( mccbcsms_config.ovhd_chan == CAIX_PC )
  {
    qw_equ(tmp_qw, msg_frame_20);
    qw_shift(tmp_qw, -2);       /* Convert to slot in 80ms */
    current_slot = qw_lo(tmp_qw);

    if ( ( mccbcsms_pages.pages.bc.tot_bc_msg > 0 )     &&
         ( mccbcsms_pages.pages.bc.nxt_rec_index != 0 ) &&
         ( mcc_is_ovhd_cfg_okay() )                         )
    {
      /* if we wakeup at the scheduled slot and match the bc_addr */
      if ( mccbcsms_config.bcast_slot_cycle != 0 &&
           mccbcsms_is_bc_data_slot(msg_frame_20) )
      {
        /* Multiple GPM may have the same broadcast data    */
        /* slot schedules. We need to check all of them.    */

        int i;

        for (i = mccbcsms_pages.pages.bc.nxt_rec_index;
             ((i <= mccbcsms_pages.pages.bc.tot_bc_msg) &&
              (qw_cmp(mccbcsms_pages.pages.bc.page_recs[i-1].wakeup_slot,
                      tmp_qw) == 0));
             i++ )
        {
          if (memcmp(mccbcsms_pages.pages.bc.page_recs[i-1].bc_addr,
                     msg_hdr->addr.type5.bc_addr, CAII_BC_ADDR_LEN) == 0)
          {
            /* if the currently expected BC_ADDR is the same as the */
            /* one arrives, then we have a match                    */
            MSG_MED("Rec'd scheduled BCSMS, page num=%d", i, 0, 0);

            /* the expiration cycle for periodic broadcast is
               4*bcast_slot_cycle */
            if (!mccbcsms_detect_duplicate( CAI_SHORT_MSG_SERVICES,
                                            msg_hdr->addr.type5.bc_addr,
                                            current_slot,     /* 80ms unit */
                                            dup_cycle))       /* 80ms unit */
            {
              mccbcsms_insert_unique( CAI_SHORT_MSG_SERVICES,
                                      msg_hdr->addr.type5.bc_addr,
                                      current_slot);          /* 80ms unit */
              addr_found = TRUE;
            }

            /* Update sleep slot mask */
            mccidl_update_sleep_slot_mask(BC_DATA_SLOT);

            if (mccidl_assert_sleep_ok(msg_frame_20))
            {
              MSG_MED("BC SMS Rec'd, Sleep OK",0,0,0);
            }
            else
            {
              MSG_MED("Sleep_ok not asserted",0,0,0);
            }
            break;
          }
        } /* for */
      } /* if mccbcsms_config.bcast_slot_cycle != 0 */
      else
      {
        /* Originally we don't support multi-slotted and periodic mode
         * broadcast simultaneously, but per Lucent's request, I will put in
         * the code to support this, although even Lucent doesn't know if
         * they will eventually implement such broadcast mode */

        /* we received a BC that's not expected in periodic mode broadcast */
        /* for multislotted mode broadcast, the expiration cycle is the
         * maximum slot cycle */
        if ((cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr)) &&
            (!mccbcsms_detect_duplicate( CAI_SHORT_MSG_SERVICES,
                                         msg_hdr->addr.type5.bc_addr,
                                         current_slot,
                                         dup_cycle
                                 )
            ))
        {
          addr_found = TRUE;
          mccbcsms_insert_unique( CAI_SHORT_MSG_SERVICES,
                                  msg_hdr->addr.type5.bc_addr,
                                  current_slot);
          MSG_MED ("Rec'd mslotd BCSMS.", 0, 0, 0);
          /* it's a multi-slotted bc sms */
        }
      }
    }
    else
    {
      if (cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr))
      {
        if (!mccbcsms_detect_duplicate( CAI_SHORT_MSG_SERVICES,
                                        msg_hdr->addr.type5.bc_addr,
                                        current_slot,
                                        dup_cycle))
        {
          addr_found = TRUE;
          mccbcsms_insert_unique( CAI_SHORT_MSG_SERVICES,
                                  msg_hdr->addr.type5.bc_addr,
                                  current_slot);
        }
        if (mccbcsms_config.bcast_slot_cycle != 0)
        {
          MSG_MED ("Rec'd nonsch'd BCSMS, BC_ADDR match=%d",addr_found,0,0);
        }
        else
        {
          MSG_MED ("Rec'd mslotd BCSMS, BC_ADDR match=%d",addr_found, 0, 0);
        }
      }
    }
  } /* ovhd_chan == PC */

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  /* Perform the broadcast address matching on the BCCH */
  else /* ovhd_chan == BCCH */
  {
    enh_bc_page_type  *page_ptr = NULL;
    int               i;
    cai_bcch_rate_type  bcch_rate = mccbcsms_config.enbcsms_info.bcch_info  /*lint !e641 */
                        [mccbcsms_pages.pages.ebc.monitor_bcn[0]-2].brat;

    /* Calculate the time stamp for the beginning of the */
    /* broadcast message, this will be matched to the    */
    /* broadcast data slot. msg_frame_20 is the time set */
    /* by RXC when it receives the first frame. A frame  */
    /* can be 40, 80 or 160ms long depending on the BCCH */
    /* data rate. RXC will set the time as follows:      */ 
    /*                                                   */
    /*   0        1        2        3        4 (40ms)    */
    /*   |      a |      b |        |      c |           */
    /*   ------------------------------------            */
    /*  a - bcch_rate = 2  40ms frame                    */
    /*  b - bcch_rate = 1, 80ms frame                    */
    /*  c - bcch_rate = 0, 160ms frame.                  */

    qw_equ(tmp_qw, msg_frame_20);
    qw_shift(tmp_qw, -1);           /* make it in 40ms */
    qw_dec(tmp_qw, ((4 / (1 << bcch_rate)) - 1));  /*lint !e641 */
    current_slot = qw_lo(tmp_qw);
    current_slot /= 2;              /* make it in 80ms */

    /* We received a broadcast message on secondary BCCH, find  */
    /* the corresponding enhanced broadcast page by searching   */
    /* for all the potential BCNs. Later on, if CP can get the  */
    /* BCN for the messages received from MUX, we only need to  */
    /* look for one particular BCN then.                        */
    for (i = 0; i < mccbcsms_config.enbcsms_info.num_bcch_bcast; i++)
    {
      page_ptr =
          mccbcsms_find_ebc_page( i + 2,  /*lint !e734 */
                                  CAI_SHORT_MSG_SERVICES,
                                  msg_hdr->addr.type5.bc_addr,
                                  tmp_qw );         /* 40ms */
      if ( page_ptr )   break;
    }

    if (page_ptr || (mccbcsms_pages.pages.ebc.num_non_pri_bcch_monitor > 0))
    {
      if (page_ptr)
      {
        MSG_MED("Recv schd bcsms @ 0x%x", (qw_lo(tmp_qw) & 0xFF), 0, 0);
      }
      else
      {
        MSG_MED("Recv bcmsg addr 0x%x 0x%x @ 0x%x", 
                 msg_hdr->addr.type5.bc_addr[0],
                 ((((uint32)msg_hdr->addr.type5.bc_addr[1]) << 24) |
                  (((uint32)msg_hdr->addr.type5.bc_addr[2]) << 16) |
                  (((uint32)msg_hdr->addr.type5.bc_addr[3]) << 8 ) |
                  ((uint32)msg_hdr->addr.type5.bc_addr[4])          ),
                 (qw_lo(tmp_qw) & 0xFF));
      }

      /* Found the corresponding ebc page, We have done the dup */
      /* detection for the ebc page already, and if BS acts     */
      /* reasonably, every broadcast message should be scheduled*/
      /* by a ebc page. So we don't need to dup detection here. */
      /* Just copy the bc_addr and slot info to the unique list */
      mccbcsms_insert_unique(CAI_SHORT_MSG_SERVICES,
                             msg_hdr->addr.type5.bc_addr,
                             current_slot );        /* 80ms */

      /* Remove this page and other duplicate pages */
      mccbcsms_delete_dup_ebc_pages(msg_hdr->addr.type5.bc_addr,
                                    CAI_SHORT_MSG_SERVICES,
                                    tmp_qw);        /* 40ms */
      addr_found = TRUE;

      /* Check if there are any immediate broadcast SMS schedules   */
      if (!mccbcsms_check_immed_ebc_sched())
      {
        mccidl_supervise_channel_monitoring(cdma.curr_state, NULL, TRUE);
        mccbcsms_pages.pages.ebc.num_non_pri_bcch_monitor = 0;
      }
    }
    else
    {
      /* Received an unexpected BCSMS message. According to the */
      /* standard IS-2000 Rel A, 2.6.2.1.1.3.6, The mobile      */
      /* station should ignore broadcast messages for which a   */
      /* corresponding enhanced broadcast page was not received */
      MSG_HIGH("Unschd bcmsg addr 0x%x 0x%x @ 0x%x", 
               msg_hdr->addr.type5.bc_addr[0],
               ((((uint32)msg_hdr->addr.type5.bc_addr[1]) << 24) |
                (((uint32)msg_hdr->addr.type5.bc_addr[2]) << 16) |
                (((uint32)msg_hdr->addr.type5.bc_addr[3]) << 8 ) |
                ((uint32)msg_hdr->addr.type5.bc_addr[4])          ),
               (qw_lo(tmp_qw) & 0xFF));

      /* However, someone may be interested in this unscheduled */
      /* broadcast message. We will handle it anyway...         */
      #ifdef FEATURE_IS2000_REL_A_CC_BCSMS_PROC_UNSCHED
      if (cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr))
      {
        /* Since this is an unscheduled broadcast message, we   */
        /* need to check if it is a duplicate.                  */
        if (!mccbcsms_detect_duplicate(CAI_SHORT_MSG_SERVICES,
                                       msg_hdr->addr.type5.bc_addr,
                                       current_slot,
                                       dup_cycle        /* 80ms */
                                      ))
        {
          addr_found = TRUE;

          /* Do not use this unscheduled message as a unique    */
          /* entry to perform the dup detection for future pages*/
          MSG_HIGH("Handle the unscheduled BCSMS", 0, 0, 0);
        }
      }
      #endif /* FEATURE_IS2000_REL_A_CC_BCSMS_PROC_UNSCHED */
    }

  } /* ovhd_chan == BCCH */
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  if (addr_found)
  {
    /* Received a broadcast message. Check if we need to update */
    /* Search with new broadcast info.                          */
    mccbcsms_schedule_bcast();
  }

  return addr_found;

} /* mccbcsms_comp_addr */

/*===========================================================================

FUNCTION MCCBCSMS_PAGE_MATCH

DESCRIPTION
  This function performs broadcast page match operation.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE broadcast or enhanced broadcast page match.
  Otherwise, FALSE.

SIDE EFFECTS

===========================================================================*/
boolean mccbcsms_page_match
(
  mccbcsms_bc_info_type *bc_ptr /* Pointer to broadcast info */
)
{
  qword    bc_wakeup_slot;     /* tmp variable to cal. the wakeup slot */
  boolean  bc_found = FALSE;
  qword    msg_slot;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  qw_equ(msg_slot, bc_ptr->frame);
  qw_shift(msg_slot, -2);       /* msg slot in 80 ms */

  if (bc_ptr->bc_type == BROADCAST_PAGE)
  {
    /* Broadcast Page */

    if (bc_ptr->burst_type == CAI_SHORT_MSG_SERVICES)
    {
      if (!OVHD_CHAN_IS_PCH)
      {
        MSG_ERROR("Received broadcast page on F-CCCH", 0, 0, 0);
        return (FALSE);
      }

      if (mccbcsms_config.bcsms_enable && !mccbcsms_config.bcast_init)
      {
        /* bc_index is not initialized, we can't process the GPM right now */
        return (TRUE);
      }

      if (!mccbcsms_config.bcsms_enable || mccbcsms_config.bcast_slot_cycle == 0)
      {
        MSG_MED ("Broadcast SMS page ignored, feature not enabled",0,0,0);
        return (FALSE);
      }

      if (mccbcsms_pages.pages.bc.tot_bc_msg == MAX_BC_PER_PAGE_MSG) /*lint !e834 */
      {
        /* Check if any broadcast data slots have been expired. */
        while ((mccbcsms_pages.pages.bc.nxt_rec_index <
                mccbcsms_pages.pages.bc.tot_bc_msg     )
               &&
               (qw_cmp(msg_slot, mccbcsms_pages.pages.bc.page_recs
                 [mccbcsms_pages.pages.bc.nxt_rec_index].wakeup_slot) > 0))
        {
          /* We need to ignore the wakeup slots that already passed */
          mccbcsms_pages.pages.bc.nxt_rec_index++;
          MSG_MED("Ignore BC SMS page num %d",
                  mccbcsms_pages.pages.bc.nxt_rec_index, 0,0);
        }
        if ( mccbcsms_pages.pages.bc.nxt_rec_index
             ==
             mccbcsms_pages.pages.bc.tot_bc_msg )
        {
          mccbcsms_pages.pages.bc.nxt_rec_index = 0;
          mccbcsms_pages.pages.bc.tot_bc_msg    = 0;
        }

        if (mccbcsms_pages.pages.bc.tot_bc_msg == MAX_BC_PER_PAGE_MSG)  /*lint !e834 */
        {
          /* ------------------------------------------------------------------
          ** We should never get here with the current understanding of the
          ** standard, however if multiple GPMs that occupied more than 2 slots
          ** are allowed, MAX_BC_PER_PAGE_MSG might need to be changed.
          ** ------------------------------------------------------------------ */
          MSG_ERROR("BCast page buffer overflow, discard remain pages", 0, 0, 0);
          return (FALSE);
        }
      }

      if (bc_ptr->addr_len == CAII_BC_ADDR_LEN)
      {
        if (cm_wms_bc_addr_match(bc_ptr->bc_addr))
        {
          /* --------------------------------------------------------------
          ** We have a broadcast SMS match the next slot to wake up is
          ** calculated by adding 3 x bc_page_number to the reference slot.
          ** -------------------------------------------------------------- */
          qw_equ(bc_wakeup_slot, msg_slot);
          qw_inc(bc_wakeup_slot, bc_ptr->page.bc.num_bc_page * 3);  //lint !e732

          /* ---------------------------------------------------------------
          ** We need to check for duplicates, it's different from the normal
          ** page msg duplicate detection, since BC page doesn't have
          ** msg_seq.
          **
          ** When check for duplicate, we should use the page slot of
          ** the future Data Burst not the current page slot.
          ** --------------------------------------------------------------- */
          if (!mccbcsms_detect_duplicate(bc_ptr->burst_type,
                                         bc_ptr->bc_addr,
                                         qw_lo(bc_wakeup_slot),     /* 80ms */
                                         4 * mccbcsms_config.bcast_slot_cycle))
                                                                    /* 80ms */
          {
            int t_index;

            bc_found = TRUE;

            /* --------------------------------------
            ** If not duplicate, start sorted insert.
            ** -------------------------------------- */

            /* First, calculate the index where we should insert the record */
            t_index =
                mccbcsms_cal_bc_rec_index(mccbcsms_pages.pages.bc.page_recs,
                                          bc_wakeup_slot);
            memcpy(mccbcsms_pages.pages.bc.page_recs[t_index].bc_addr,
                   bc_ptr->bc_addr,
                   CAII_BC_ADDR_LEN);
            mccbcsms_pages.pages.bc.page_recs[t_index].burst_type =
                bc_ptr->burst_type;

            /* Assign the next wake up slot */
            qw_equ(mccbcsms_pages.pages.bc.page_recs[t_index].wakeup_slot,
                   bc_wakeup_slot);
            mccbcsms_pages.pages.bc.tot_bc_msg++;

            /* Reset the next BC Msg index to be received */
            mccbcsms_pages.pages.bc.nxt_rec_index = 0;

            MSG_MED("BC rec index=%d, wake_slot=0x%x",
                    t_index, qw_lo(bc_wakeup_slot), 0);
          }
        }
      }
      /* we do nothing if addr_len is not correct, since very unlikely. */
    }
    else
    {
      /* Unsupported burst type */
      MSG_ERROR("Unsupported burst %d", bc_ptr->burst_type,0,0);
    }
  }   /* Broadcast Page */

  #ifdef FEATURE_IS2000_REL_A_CC_BCSMS
  else if (bc_ptr->bc_type == ENHANCED_BROADCAST_PAGE)
  {
    /* Enhanced Broadcast Page */

    if (bc_ptr->burst_type == CAI_SHORT_MSG_SERVICES)
    {
      enh_bc_page_type *page_ptr;
      dword            expire_cycle;
      qword            t_data_slot;

      if (!OVHD_CHAN_IS_BCCH)
      {
        MSG_ERROR("Received enhanced broadcast page on PCH", 0, 0, 0);
        return (FALSE);
      }

      if (mccbcsms_config.bcsms_enable && !mccbcsms_config.bcast_init)
      {
        /* bc_index is not initialized, we can't process the GPM right now */
        return (TRUE);
      }

      if (!mccbcsms_config.bcsms_enable)
      {
        MSG_MED ("Broadcast SMS page ignored, feature not enabled",0,0,0);
        return (FALSE);
      }

      if (bc_ptr->addr_len == CAII_BC_ADDR_LEN)
      {
        if (cm_wms_bc_addr_match(bc_ptr->bc_addr))
        {
          /* If multislotted mode, expire cycle is calculated */
          /* using the max_slot_cycle_index.                  */
          if (mccbcsms_config.bcast_slot_cycle == 0)
          {
            expire_cycle = 16L<<cur_bs_ptr->csp.sp.max_slot_cycle_index;
          }
          else
          {
            expire_cycle = 4 * mccbcsms_config.bcast_slot_cycle;
          }

          /* Check if a duplicate enhanced broadcast page which a   */
          /* broadcast message was received. The function compares  */
          /* against the mccbcsms_unique_recs.                      */
          if( !mccbcsms_detect_duplicate( bc_ptr->burst_type,
                                          bc_ptr->bc_addr,
                                          qw_lo(msg_slot), /* 80 ms */
                                          expire_cycle ))  /* 80 ms */
          {
            /* Convert the msg_slot from 80ms units to 40ms units.  */
            /* This guarantees that the msg_slot is aligned with 80 */
            /* ms boundary, as it should be in F-CCCH.              */
            qw_shift(msg_slot, 1);
            qw_equ(t_data_slot, msg_slot);
            qw_inc(t_data_slot, bc_ptr->page.enh_bc.time_offset + 1);

            /* Check if a duplicate enhanced broadcast page which a */
            /* broadcast message wasn't received yet. This happens  */
            /* when in the multi-slotted mode the mobile happens to */
            /* stay awake for several slots for whatever reasons    */

            if ( !mccbcsms_find_ebc_page(bc_ptr->page.enh_bc.bcn,
                                         bc_ptr->burst_type,
                                         bc_ptr->bc_addr,
                                         t_data_slot ) )
            {
              page_ptr = mccbcsms_get_free_ebc_page(
                             bc_ptr->bc_addr,
                             bc_ptr->burst_type,
                             msg_slot,                    /* 40 ms */
                             bc_ptr->page.enh_bc.time_offset,
                             bc_ptr->page.enh_bc.repeat_time_offset);

              if ( page_ptr )
              {
                bc_found = TRUE;
                (void)mccbcsms_insert_ebc_page( bc_ptr->page.enh_bc.bcn,
                                          page_ptr );      

                if ( bc_ptr->page.enh_bc.repeat_time_offset < 
                     CAI_INV_REPEAT_TIME_OFFSET               )
                {
                  MSG_MED("ebcpg: bcn %d offset %d rept %d",
                          bc_ptr->page.enh_bc.bcn,
                          bc_ptr->page.enh_bc.time_offset,
                          bc_ptr->page.enh_bc.repeat_time_offset);
                }
                else
                {
                  MSG_MED("ebcpg: bcn %d offset %d no rept",
                          bc_ptr->page.enh_bc.bcn,
                          bc_ptr->page.enh_bc.time_offset, 0);
                }
                MSG_MED("bcdata 0x%x 40ms, bc_addr 0x%x %x", 
                        (qw_lo(t_data_slot) & 0xFF),
                        bc_ptr->bc_addr[0],
                        ((((uint32)bc_ptr->bc_addr[1]) << 24) |
                         (((uint32)bc_ptr->bc_addr[2]) << 16) |
                         (((uint32)bc_ptr->bc_addr[3]) << 8 ) |
                         ((uint32)bc_ptr->bc_addr[4])          ));
              }
              else
              {
                MSG_ERROR("No space to store the bc page!!", 0, 0, 0);
              }
            } /* mccbcsms_find_ebc_page */
            else
            {
              MSG_HIGH("Dup EBC page", 0, 0, 0);
            }
          } /* mccbcsms_detect_duplicate */
          else
          {
            MSG_HIGH(" Rcvd a dup EBC page w msg", 0, 0, 0 );
          }

        } /* uasms_bc_addr_match */

      } /* addr_len == CAII_BC_ADDR_LEN */

      /* we do nothing if addr_len is not correct, since very unlikely. */
    }
    else
    {
      /* Unsupported burst type */
      MSG_ERROR("Unsupported burst %d", bc_ptr->burst_type, 0, 0);
    }
  } /* Enhanced Broadcast Page */
  #endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

  return (bc_found);

} /* mccbcsms_page_match */

#ifdef FEATURE_IS2000_REL_A_CC_BCSMS
/*===========================================================================

FUNCTION MCCBCSMS_MATCH_UPM_PARTIAL_ADDR

DESCRIPTION
  This function performs the Mobile Station Broadcast partial address match
  operation for the Universal Page Message. The result of match is directly
  saved in the input partial address structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccbcsms_match_upm_partial_addr
(
  caii_univ_page_interleaved_addr_type *addr_ptr
   /* Pointer to partial address structure */
)
{
  if (!mccbcsms_config.bcsms_enable || !addr_ptr->bcast_included)
  {
    addr_ptr->bcast_match = FALSE;
  }
  else if (addr_ptr->bcast_bit_matched < addr_ptr->iaddr_portion_rxed)
  {
    cm_wms_bc_partial_addr_match(addr_ptr);
  }

} /* mccbcsms_match_upm_partial_addr */
#endif /* FEATURE_IS2000_REL_A_CC_BCSMS */

#endif /* FEATURE_BROADCAST_SMS */
