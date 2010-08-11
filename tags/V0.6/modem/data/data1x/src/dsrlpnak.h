#ifndef DSRLPNAK_H
#define DSRLPNAK_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      R A D I O   L I N K   P R O T O C O L  N A K  P R O C E S S I N G

GENERAL DESCRIPTION
  This file contains generic nak-list processing functions.  It is not
  specific to any one RLP type.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

 Copyright (c) 1999 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlpnak.h_v   1.2   12 Feb 2003 17:35:52   kdevara  $
  $Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/src/dsrlpnak.h#1 $ $DateTime: 2007/11/06 01:21:33 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/12/03   kvd        Added new function prototype, dsrlpnak_get_freenak_count()
08/07/02   vr         Added functgion dsrlp_get_all_nak for HDR
01/07/00   ak         Fixed _insert and _remove to return boolean values.
08/05/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_DS

#include "dsrlp.h"
#include "dsrlpi.h"


/*===========================================================================

             PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Number of nak entries that will exist for all RLP instances on the mobile.
  A pool of this many nak entries will be maintained in the global nak 
  entry pool.
---------------------------------------------------------------------------*/
#define  DSRLPNAK_MAX_NAK_ENTRIES    64

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                       MACRO DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLPNAK_GET_FIRST_SEQ

DESCRIPTION    Returns the first seq for a nak entry.

               PARAMS:  nak_id = id returned by DSRLPNAK_GET_NAKS.

DEPENDENCIES   Only called by the TX TASK
               
RETURN VALUE   The first_seq in the nak entry.

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPNAK_GET_FIRST_SEQ(nak_id) \
       (((dsrlpi_nak_list_entry_type *)nak_id)->first_seq)

/*===========================================================================

FUNCTION       DSRLPNAK_GET_LAST_SEQ

DESCRIPTION    Returns the last seq for a nak entry.

               PARAMS:  nak_id = id returned by DSRLPNAK_GET_NAKS.

DEPENDENCIES   Only called by the TX TASK
               
RETURN VALUE   The last_seq in the nak entry.

SIDE EFFECTS   NONE
===========================================================================*/
#define DSRLPNAK_GET_LAST_SEQ(nak_id) \
       (((dsrlpi_nak_list_entry_type *)nak_id)->last_seq)


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                      FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLPNAK_INIT

DESCRIPTION    Called at initialization, this takes all nak entries from
               the array and puts them onto the free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpnak_init(void);

/*===========================================================================

FUNCTION       DSRLPNAK_RESET

DESCRIPTION    Called when an RP instance is reset.  This takes any nak list
               entries from that RLP instance and puts them onto the 
               global free queue.
               
DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpnak_reset
(
  dsrlpi_sess_ctl_blk_type *rscb_ptr         /* current RLP session ctl blk*/
);

#ifdef FEATURE_DS_RLP3_OBSOLETE
#error code not present
#endif /* FEATURE_DS_RLP3_OBSOLETE */

/*===========================================================================

FUNCTION       DSRLPNAK_AGE_LIST

DESCRIPTION    Go through the list and age the nak list entries.  If any
               of the timers have expired, then set a flag so that the
               RLP Tx routine knows to send more naks out.
               
DEPENDENCIES   rscb_ptr != NULL

RETURN VALUE   TRUE if V(N) has changed.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpnak_age_list
(
  dsrlpi_sess_ctl_blk_type *rscb_ptr         /* current RLP session ctl blk*/
);

/*===========================================================================

FUNCTION       DSRLPNAK_INSERT

DESCRIPTION    Insert this new entry into the nak list.  Inserted according
               to sequence number.  Nak entry is filled out with first nak
               round information.
               
               If entry already exists, then nothing is done.
               
               Insert(a,b):  a is the first missing frame, inclusive.
               b is the 32-bit sequence number of the frame just received,
               as calculated by frame determination.  This means that the
               second number is exclusive to the set of missing frames.
               
               Possible insertions are:
               
               Insert            Results in these being inserted
               3  -> 5                3 -> 5
               
               3  -> 3f               3 -> 3f
               
               3  -> 4a               3 -> 4, 4 -> 4a
               
               3a -> 4                3a -> 4
               
               3a -> 4g               3a -> 4, 4 -> 4g
               
               3a -> 6b               3a -> 4, 4 -> 6, 6 -> 6b
               
               3  -> 6b               3 -> 6, 6 -> 6b
               
               This maintains the rule that if segmented frames are being
               NAK'd, then an entry contains the NAK info for the bytes
               within one 8-bit SEQ space.
               
DEPENDENCIES   Input rscb_ptr is non-null.

RETURN VALUE   TRUE if the information could be inserted, else FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpnak_insert
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,      /* curr RLP session ctl blk */
  uint32                        first_num,     /* first missing seq num    */
  uint32                        last_num       /* last missing seq num     */
);

/*===========================================================================

FUNCTION       DSRLPNAK_NEXT_NAK_RND

DESCRIPTION    Update the current nak entry to the next round's nak info.
               If this is already the last round, then may result in the
               entry being removed from the list.
               
DEPENDENCIES   both input ptrs are non-NULL.

RETURN VALUE   NONE

SIDE EFFECTS   The nak_entry_ptr can be deleted from the nak list.
===========================================================================*/
void dsrlpnak_next_nak_rnd
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,      /* curr RLP session ctl blk */
  dsrlpi_nak_list_entry_type   *nak_entry_ptr  /* nak list entry           */
);

/*===========================================================================

FUNCTION       DSRLPNAK_REMOVE

DESCRIPTION    Removes a nak entry from the nak list.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the RX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
               nak_id must be the value returned from dsrlpnak_get_seq_hi,
               which returned TRUE.
               
RETURN VALUE   TRUE if the remove was successful.  TRUE is returned even
               when the nak item was not in the list.  FALSE is
               returned when there was an internal memory error with
               allocating nak items.  This indicates the calling fcn
               should reset RLP.

SIDE EFFECTS   V(N) may be updated.
===========================================================================*/
boolean dsrlpnak_remove
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,      /* curr RLP session ctl blk */
  dsrlpi_curr_fr_info_type     *fr_info_ptr,   /* ptr to curr frame info   */
  word                          length,        /* byte-length of data      */
  uint32                        nak_id         /* ref pt from get_seq_hi   */
);

/*===========================================================================

FUNCTION       DSRLPNAK_NAK_WAS_SENT

DESCRIPTION    Tells the nak list if a particular nak was sent over the air.
               Results in decrementing the naks_left counter.  In the case
               where naks_left counter == 0, nothing is done.  The nak_entry
               is "reset" in the Rx Task, in the dsrlpnak_age_list function.
               This scheme prevents any mutual exclusion issues between the
               RX and TX task. 

               The input paramenter is the id given by DSRLP_NAK_GET_NAKS().

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpnak_nak_was_sent
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,      /* curr RLP session ctl blk */
  uint32                        nak_id         /* nak id of the nak that   */
                                               /* was sent                 */
);

/*===========================================================================

FUNCTION       DSRLPNAK_GET_NAK

DESCRIPTION    Returns an id for the next nak entry which requires a NAK to
               be sent (i.e, age == 0, and naks_left > 0).
               
               The entry is returned as a uint32's.  The TX task is assumed
               to only use functions/macros found in dsrlpnak.c/.h to
               access nak entry info.  As such, it is not important for the
               calling task to know what type the nak_id is.  
               
               Input requires the user to input "curr_nak_id".  If this
               value is zero, then the very first nak entry (requiring
               nak transmission) is returned.  Else, this id refers to a
               nak list entry; in this case the id of nak entry after the
               referenced entry is returned.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the TX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.  
               
RETURN VALUE   An id for the next nak entry which requires a nak xmission.
               If the return value is zero, then there are no more nak
               entries requiring transmission.

SIDE EFFECTS   NONE
===========================================================================*/
uint32 dsrlpnak_get_nak
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,      /* curr RLP session ctl blk */
  uint32                        curr_nak_id,   /* current nak id           */
  boolean                       segmented      /* TRUE = next segmented    */
);

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION       DSRLPNAK_GET_SEQ_HI

DESCRIPTION    Given an input sequence number (with unknown SEQ_HI), 
               finds the SEQ_HI for that number.
               
               The fr_info_ptr's seq_num is updated with the found seq
               number (if it is found).  The input nak_id is updated with
               a reference point, so that if dsrlpnak_remove is called
               with it, it can be more efficient.

DEPENDENCIES   Input ptrs are non-null.
               
               This can only be called by the RX task.  Must not be called
               by any other task, as then mutual exclusion issues will
               arise.  Assumed that TX task has higher priority than the
               RX task.
               
RETURN VALUE   TRUE if a SEQ_HI could be found, else FALSE.  If the input
               fr_info_ptr already contains a sequence number with a known 
               SEQ_HI, then still return TRUE.

SIDE EFFECTS   V(N) may be updated.
===========================================================================*/
boolean dsrlpnak_get_seq_hi
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,      /* curr RLP session ctl blk */
  dsrlpi_curr_fr_info_type     *fr_info_ptr,   /* ptr to curr frame info   */
  word                          length,        /* byte-length of data      */
  uint32                       *nak_id         /* filled with ref pt       */
);
#endif /* FEATURE_DS */

/*===========================================================================

FUNCTION       DSRLPNAK_GET_FREENAK_COUNT

DESCRIPTION    returns the no. of items in the nak_free Q. This number is
               useful to print in a debug message at the end of rlp cleanup
               to detect nak item leaks

DEPENDENCIES   None

RETURN VALUE   returns the no. if items in nak_free Q

SIDE EFFECTS   None
===========================================================================*/

uint8 dsrlpnak_get_freenak_count(void);

#endif /* DSRLPNAK_H */

