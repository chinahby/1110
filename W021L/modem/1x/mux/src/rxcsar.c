/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    CDMA2000 LAC Segmentation And Reassemply (SAR) Sublayer Processing

GENERAL DESCRIPTION
  This module processes physical layer frames for IS-2000 channels.  The
  client specifies the MAC parameters for the channel as well as functions
  to allocate and deliver message buffers once parsed.
  
  void *client_data is passed unchanged by this module to functions 
  implemeted by the client to provide closure on the operation.  The client
  may use this pointer to identify the instance of this service when the 
  routine is called.
      
INITIALIZATION AND SEQUENCING REQUIREMENTS
  A message descriptor must be initialized prior to configuring it for use
  in processing a frame.  Only do this once!  The same message pointer 
  should be used for the entire time that the channel is operating.  Partial
  messages will be stored in the message descriptor between frames and will 
  be needed to complete the message.

Copyright (c) 2001 through 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcsar.c_v   1.6   21 Oct 2002 13:19:42   sanjayr  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcsar.c#3 $ $DateTime: 2008/11/03 13:30:26 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/03/08   jtm     Modified rxcsar_len_state() to correct message length 
                   bit mask is being applied when EXT MSG size is used.
                   Added F3 messages for debugging.
09/08/08   jtm     Modified rxcsar_len_state() to prevent processing messages
                   with msg_lengths > the maximum message size we can 
                   CRC check and queue to prevent buffer overflow.
07/27/04   jrp     Added type cast.
04/20/04   jrp     Fixed lint warnings.
10/22/03   sr      Fixed compilation warning errors when using RVCT compiler 
03/20/03   bn/sr   Added logic to support simultaneously monitor BCCH and FCCCH
10/21/02   bn      Added support for NCC statistic counters
10/21/02   sr      Surrounded the file with #ifdef FEATURE_IS2000_REL_A_CC
04/22/02   lad     Finalized support for flush_cnt (used in idle handoff).
04/05/02   lad     Modified SAR interface to use const info in msg_desc.
04/01/02   lad     Added message logging.
01/17/02   lad     Fixed the module (initial rev not tested).  Replaced
                   mob_p_rev w/ 'ext_msg_length_ind'.  Added rxcsar_terminate()
11/06/01   lad     Created file.

===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_IS2000_REL_A_CC

#include "assert.h"
#include "bit.h"
#include "crc.h"
#include "log.h"
#include "mux_logtypes.h"
#include "msg.h"
#include "qw.h"
#include "rxcsar.h"

/*-------------------------------------------------------------------------
 * f-csch state machine variables                                          
 *-------------------------------------------------------------------------*/

/* The fcsch logical channel frame processing state machine.
 *
 * TERMINOLOGY:
 * Frame - A complete physical layer frame (output from decoder).
 * SCI - Syncronized Capsule Indicator - Bit(s) at the beginning of each 
 *       subframe used to inidcate the start of some messages.  If set to '1'
 *       the bits immediately following are the start of a LAC PDU.
 * subframe - Each frame contains 2 or more sub-frames, each beginning with an
 *            SCI, that contain message data in the form of LAC PDUs.
 *
 * STATE NAMES AND DESCRIPTIONS:
 * RXCSAR_SCI_STATE   - Checks if the SCI is set in the first subframe.
 * RXCSAR_LEN_STATE   - Extracts the message length (LAC length field) from
 *                      the start of a message.
 * RXCSAR_MSG_STATE   - Extracts the message from the frame (after LEN_STATE).
 * RXCSAR_FLUSH_STATE - Discard subframe and return to SCI state.
 * RXCSAR_SKIP_STATE  - Skip to the end of the message, if you know the 
 *                      MSG_LENGTH field is valid.  This is used when alloc() fails.  Once complete,
 *                      go to LEN_STATE.
 */
typedef enum {
  RXCSAR_SCI_STATE,
  RXCSAR_LEN_STATE,
  RXCSAR_MSG_STATE,
  RXCSAR_FLUSH_STATE,
  RXCSAR_SKIP_STATE
} rxcsar_csch_state;

#define RXCSAR_SCI_SET 1
#define RXCSAR_MSG_ERR_CNT_THRESHOLD 2

/* Internal data structure, residing in the 'opaque_data' section of the 
   client's msg descriptor, contains implementation-specific information that
   is not necessary for (and therefore hidden from) the client. */
typedef struct {
  rxcsar_msg_desc_type *msg_ptr; /* Passed in from client */

  rxcsar_csch_state state; /* Message processing state */
  
  qword frame_num; /* Frame number (CDMA system time) */
 
  unsigned int length; /* total number of bits in data */
  void *data;          /* Message body */
  
  unsigned int index; /* Index, in bits, of the next unwritten bit. */
  
  
//XXX  unsigned int crc_seed; /* CRC30 seed/var for stepwise CRC calc */
  
  unsigned int err_cnt; /* Number of consecutive messages with bad CRCs */
  unsigned int flush_cnt; /* Number of subframes to flush */
} rxcsar_msg_info_type;


/*--------------------------------------------------------------------------
 * Private Function Prototypes
 *--------------------------------------------------------------------------*/
static void rxcsar_init_msg_state (rxcsar_msg_info_type *msg_info);

static boolean rxcsar_process_subframe (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int frame_inx,  /* bit offset of subframe */
  rxcsar_msg_info_type *msg_info
);

static rxcsar_csch_state rxcsar_len_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of subframe */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
);

static rxcsar_csch_state rxcsar_msg_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
);

static rxcsar_csch_state rxcsar_flush_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
);

static rxcsar_csch_state rxcsar_skip_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
);

/*=========================================================================*/
void
rxcsar_init_msg_descriptor (
  rxcsar_msg_desc_type *msg_ptr
)
{
  rxcsar_msg_info_type *msg_info = NULL;

  /* Assert sanity checks. */
  ASSERT (msg_ptr && msg_ptr->static_data->alloc && 
          msg_ptr->static_data->deliver && msg_ptr->static_data->discard && 
          msg_ptr->static_data->event && 
          sizeof(rxcsar_msg_info_type) <= sizeof(msg_ptr->opaque_data)); //lint !e506 Constant value Boolean (sizeof<=sizeof)

  msg_ptr->client_data = NULL; //lint !e613 ASSERT checks msg_ptr non-null
  
  msg_info = (rxcsar_msg_info_type *) msg_ptr->opaque_data; //lint !e740 Unusual pointer cast

  /* msg_info points to its corresponding msg_descriptor. */
  msg_info->msg_ptr = msg_ptr;

  rxcsar_init_msg_state (msg_info);
}


/*=========================================================================*/
boolean
rxcsar_process_fcsch_frame (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  rxcsar_msg_desc_type *msg_ptr
)
{
  unsigned int frame_inx = 0;
  unsigned int frame_size, subframe_size;
  rxcsar_msg_info_type *msg_info = NULL;
  boolean success = TRUE;

  /* Assert sanity checks */
  ASSERT (frame_ptr && msg_ptr && msg_ptr->static_data->alloc && 
          msg_ptr->static_data->deliver && msg_ptr->static_data->discard && 
          msg_ptr->static_data->event);

  msg_info = (rxcsar_msg_info_type *) msg_ptr->opaque_data; //lint !e740 Unusual pointer cast

  //REVIEW: Is a sanity check to enforce initialization necessary here?

  if (frame_ptr->data) { //lint !e613 ASSERT checks frame_ptr non-null
    frame_size = frame_ptr->frame_size; //lint !e613 ASSERT checks frame_ptr non-null
    subframe_size = frame_ptr->subframe_size; //lint !e613 ASSERT checks frame_ptr non-null
    
    /* Loop through each subframe and process it. */
    for (frame_inx = 0; success && frame_inx < frame_size;
         frame_inx += subframe_size) {

      if (frame_inx + subframe_size > frame_size) {
        success = FALSE;
      } else 
      {
        frame_ptr->total_subframe++; //lint !e613 ASSERT checks frame_ptr non-null
        success = rxcsar_process_subframe (frame_ptr, frame_inx, 
          (rxcsar_msg_info_type *) msg_ptr->opaque_data); //lint !e740 Unusual pointer cast
      }
    }
  } else {
    success = FALSE;
  }

  if (!success) {
    /* Something went wrong, start over. */
    rxcsar_init_msg_state (msg_info);
  }

  return success;
}


/*=========================================================================*/
void
rxcsar_terminate (
  rxcsar_msg_desc_type *msg_ptr, 
  uint8 subframe_flush_cnt
)
{
  rxcsar_msg_info_type *msg_info;

  ASSERT (msg_ptr);

  msg_info = (rxcsar_msg_info_type *) msg_ptr->opaque_data; //lint !e740 Unusual pointer cast

  if (msg_info->data) 
  {
    msg_ptr->static_data->discard (msg_ptr->client_data, msg_info->data);
  }
  /* this should be set to NULL so that we won't return the same item again */
  msg_info->data = NULL;
  msg_info->flush_cnt = subframe_flush_cnt;
  msg_info->state = (subframe_flush_cnt > 0) ? RXCSAR_FLUSH_STATE : RXCSAR_SCI_STATE;
}


/*===========================================================================

FUNCTION RXCSAR_PROCESS_SUBFRAME

DESCRIPTION
  This function processes a subframe for a IS-2000 f-csch (LAC layer).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean
rxcsar_process_subframe (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int frame_inx,  /* bit offset of subframe */
  rxcsar_msg_info_type *msg_info
)
{
  unsigned int subframe_size;
  unsigned int subframe_end_inx; /* Index of the next subframe.*/
  byte sci = 0;
  boolean success = TRUE;

  ASSERT (frame_ptr);
  ASSERT (msg_info);
  
  subframe_size = frame_ptr->subframe_size;
  subframe_end_inx = frame_inx + subframe_size;

  /* Regardless of state, always check for SCI. */
  sci = b_unpackb ((byte *) frame_ptr->data, (word) frame_inx, (word)
    frame_ptr->sci_size);

  if (msg_info->state != RXCSAR_FLUSH_STATE && sci == RXCSAR_SCI_SET) 
  {
    MSG_MED("Got SCI bit", 0,0,0);

    /* If a message is in progreess, discard it.  
       SCI always indicates a fresh start.  */
    msg_info->state = RXCSAR_LEN_STATE;
    
  } 
  else if (msg_info->state == RXCSAR_SCI_STATE) 
  {
    /* While looking for SCI, it was not found.  Flush this subframe. */
    msg_info->state = RXCSAR_FLUSH_STATE;
  }

  frame_inx += frame_ptr->sci_size; /* Adjust for SCI */

  while (frame_inx < subframe_end_inx) {
    switch (msg_info->state) {
      case RXCSAR_LEN_STATE:
      {
        /* Ignores return value equal to msg_info->state */
        (void) rxcsar_len_state (frame_ptr, &frame_inx, subframe_end_inx, msg_info);
        break;
      }

      case RXCSAR_MSG_STATE:
      {
        /* Ignores return value equal to msg_info->state */
        (void) rxcsar_msg_state (frame_ptr, &frame_inx, subframe_end_inx, msg_info);
        break;
      }

      case RXCSAR_FLUSH_STATE:
      {
        /* Ignores return value equal to msg_info->state */
        (void) rxcsar_flush_state (frame_ptr, &frame_inx, subframe_end_inx, msg_info);
        break;
      }

      case RXCSAR_SKIP_STATE:
      {
        /* Ignores return value equal to msg_info->state */
        (void) rxcsar_skip_state (frame_ptr, &frame_inx, subframe_end_inx, msg_info);
        break;
      }
      default:
      {
        /* If something went wrong, flush the subframe which will then start 
           over at SCI. */

        MSG_ERROR("Invalid SAR State....Flush subframe", 0,0,0);
        msg_info->state = RXCSAR_FLUSH_STATE;
        success = FALSE;
      }
    }
  }

  if (!success) 
  {
    rxcsar_init_msg_state (msg_info);
  }

  return success;
}


/*===========================================================================

FUNCTION RXCSAR_INIT_MSG_STATE

DESCRIPTION
  This function initializes the message buffer's state.  This is done at 
  init time as well as when a bad CRC or bad frame is detected.  This
  is the "reset button" for the message processing state machine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void rxcsar_init_msg_state (
  rxcsar_msg_info_type *msg_info
)
{
  rxcsar_msg_desc_type *msg_ptr = NULL;
  
  ASSERT (msg_info);

  msg_ptr = msg_info->msg_ptr;

  ASSERT(msg_ptr);

  /* Initial state is to hunt for SCI. */
  msg_info->state = RXCSAR_SCI_STATE;

  /* If the discard routine is defined, and a message has been allocated, 
     discard it. */
  if (msg_info->data) {
    msg_ptr->static_data->discard (msg_ptr->client_data, msg_info->data);
  }
  
  msg_info->length = 0;
  msg_info->data = NULL;
  
//XXX  msg_info->crc_seed = CRC_30_STEP_SEED;
  msg_info->index = 0;

  msg_info->err_cnt = 0;
  msg_info->flush_cnt = 0;
}


/*===========================================================================

FUNCTION RXCSAR_LEN_STATE

DESCRIPTION
  This function processes the LAC PDU length field and places the value into
  the msg_descriptor.  This routine handles the case where the subframe
  ends in the middle of a MSG_LENGTH field (in the case of 
  EXT_MSG_LENGTH ==1) and will continue with the next subframe.

DEPENDENCIES
  Assumes *frame_inx_ptr < subframe_end_inx

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static rxcsar_csch_state rxcsar_len_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
)
{
  rxcsar_msg_desc_type *msg_ptr = NULL;
  unsigned int frame_inx = 0; /* Use int instead of uint16 to optimize arithmetic.
                                 This code assumes no rollover will occur. */
  unsigned int msg_length = 0;
  unsigned int min_msg_length = 0;
  byte temp = 0; /* Used to check the ext_msg_length_ind field */
  word num_bytes = 0; /* # of bytes for MSG_LENGTH */
  word num_bits = 0;

  ASSERT (frame_ptr);
  ASSERT (frame_inx_ptr);
  ASSERT (msg_info);

  frame_inx = *frame_inx_ptr;

  if (frame_inx < subframe_end_inx) 
  {

    msg_ptr = msg_info->msg_ptr;

    /* Default: EXT_MSG_LENGTH_IND is not set */
    min_msg_length = msg_ptr->static_data->min_msg_len;

    num_bytes = 1; /* Default.  Check if more is needed */
    
    /* Check if EXT_MSG_LENGTH is supported.  If so, check if set 
       (indicates 15 bit MSG_LENGTH field instead of 7) .  This section 
       refers to 3GPP2 C.S0004-A-1 2.0 section 2.1.2.4.2 */
    if (msg_ptr->static_data->ext_msg_length_ind) 
    {
      /* EXT_MSG_LENGTH_IND is 1 bit, and always will be. */
      temp = b_unpackb (frame_ptr->data, (word) frame_inx, 1);
      if (temp) 
      {
        num_bytes = 2;
        min_msg_length = 128; /* Max val in 7 bits is 127 */
      }
    }
    num_bits = num_bytes * 8; //lint !e734 Loss of precision (assignment) (19 bits to 16 bits)

    if (frame_inx + num_bits <= subframe_end_inx) 
    {
      msg_length = b_unpackd (frame_ptr->data, (word) frame_inx, num_bits);

      /* Check for minimum and maximum message length requirements.
      There may be a possibility that we received a noisy frame 
      that passed hardware CRC but was still handed to us. The frame 
      could contain bogus information about its length. If the 
      length value is bad we may overflow buffers later on 
      (i.e. rxc_bcch_crc_temp_array).
      
      The maximum message size is 127 bytes for 7bit message field and
         32767 bytes for 15bit message field. 
      
      The min and max fields are in bytes, not bits*/
      
      /* For extended messages, only 15bits are used so mask off the 
       remaining bits so the correct length value is obtained. */
      if (num_bytes == 2) 
      {
        msg_length &= ~0x8000;
      }

      if (msg_length >= min_msg_length && msg_length <= RXCSAR_MAX_EXPECTED_MSG_LENGTH) 
      {
        b_copy (frame_ptr->data, (word) frame_inx, msg_ptr->crc_comp_ptr, 0, num_bits);
        msg_ptr->crc_comp_length_numbits = num_bits;

        frame_inx += num_bits;
        msg_length -= num_bytes;
        /* If already allocated, this is unexpected.  Discard the buffer. */
        /* Potential memory leak! */
        if (msg_info->data != NULL) 
        {
          MSG_HIGH("Message buffer already allocated", 0,0,0);
          msg_ptr->static_data->discard (msg_ptr->client_data, msg_info->data);
          msg_info->data = NULL;
        }
  
        msg_info->data = msg_ptr->static_data->alloc (msg_ptr->client_data, msg_length);
        if (msg_info->data == NULL) 
        {
          MSG_HIGH("No buffer allocated...Skip frame", 0,0,0);
          msg_info->state = RXCSAR_SKIP_STATE;
        } 
        else 
        {

          MSG_MED("Processing Msg - length = %d, min = %d, max = %d",
                   (msg_length + num_bytes),min_msg_length,RXCSAR_MAX_EXPECTED_MSG_LENGTH);


          /* Now that the LEN_STATE is complete, 'length' refers to bits */
          msg_length *= 8;
          /* total number of bits, including CRC and MSG_LENGTH */
          msg_ptr->crc_comp_numbits = (uint16) (msg_length + num_bits);
          msg_info->length = msg_length;
          if (frame_ptr->frame_num) 
          {
            qw_equ (msg_info->frame_num, *frame_ptr->frame_num);
          }
          msg_info->state = RXCSAR_MSG_STATE;
        }
      } 
      else 
      {

        MSG_HIGH("Flush Subframe - length = %d, min = %d, max = %d",
                   msg_length,min_msg_length,RXCSAR_MAX_EXPECTED_MSG_LENGTH);

        msg_info->state = RXCSAR_FLUSH_STATE;
      }
    } 
    else 
    {

      MSG_HIGH("End of subframe exceeded...Flush subframe", 0,0,0);
      msg_info->state = RXCSAR_FLUSH_STATE;
    }
  } 
  else 
  {

    MSG_HIGH("End of subframe exceeded...Flush subframe", 0,0,0);
    msg_info->state = RXCSAR_FLUSH_STATE;
  }
  *frame_inx_ptr = frame_inx;
  return msg_info->state;
}


/*===========================================================================

FUNCTION RXCSAR_MSG_STATE

DESCRIPTION
  This function processes the LAC PDU message body and completes CRC 
  calculation of the LAC PDU.  If successful, the message is delivered
  to the client.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static rxcsar_csch_state rxcsar_msg_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
)
{
  rxcsar_msg_desc_type *msg_ptr = NULL;
  unsigned int frame_inx;
  word num_bits;
  mux_msg_log_type *log;

  ASSERT (frame_ptr && frame_inx_ptr && msg_info);

  frame_inx = *frame_inx_ptr; //lint !e613 ASSERT checks frame_inx_ptr non-null

  msg_ptr = msg_info->msg_ptr;

  /* calculate number of bits to copy into message buffer for this iteration */
  num_bits = (word) MIN ((msg_info->length - msg_info->index),
                  (subframe_end_inx - frame_inx));

  b_copy (frame_ptr->data, (uint16) frame_inx, msg_info->data,
    (uint16) msg_info->index, (uint16) num_bits); //lint !e613 ASSERT checks msg_info non-null

  frame_inx += num_bits;
  msg_info->index += num_bits;

  if (msg_info->index >= msg_info->length) 
  {
    /* Check CRC of message. */
    uint32 crc;
    b_copy (msg_info->data, 0, msg_ptr->crc_comp_ptr, msg_ptr->crc_comp_length_numbits, 
      (uint16) msg_info->length);

    crc = crc_30_calc (msg_ptr->crc_comp_ptr, msg_ptr->crc_comp_numbits);
    if (crc == CRC_30_OK) 
    {

      MSG_MED("CRC Good", 0,0,0);
      if ((int16)msg_ptr->static_data->log_code != -1)
      {
        log = (mux_msg_log_type*) log_alloc_ex (msg_ptr->static_data->log_code, 
          FPOS (mux_msg_log_type, msg) + (msg_ptr->crc_comp_numbits + 7)/8);
  
        if (log) 
        {
          memcpy ((void *) log->msg, msg_ptr->crc_comp_ptr, (msg_ptr->crc_comp_numbits + 7)/8);
          log_commit (log);
        }
      }

      msg_ptr->static_data->deliver (msg_ptr->client_data, msg_info->length - 30, 
                        msg_info->data, &msg_info->frame_num); //lint !e545 pointer to frmae_num for deliver function

      msg_info->state = RXCSAR_LEN_STATE;
    } 
    else 
    {
      /* CRC did not pass.  Report event and discard message buffer. */
      msg_ptr->static_data->event (msg_ptr->client_data, RXCSAR_CRC_FAIL_S);

      msg_ptr->static_data->discard (msg_ptr->client_data, msg_info->data);
      MSG_HIGH("CRC Failed", 0,0,0);

      if (++msg_info->err_cnt > RXCSAR_MSG_ERR_CNT_THRESHOLD) 
      {
        msg_info->err_cnt = 0;
        msg_info->state = RXCSAR_FLUSH_STATE;
      } 
      else 
      {
        msg_info->state = RXCSAR_LEN_STATE;
      }
    }

    /* Indicate the message data is no longer valid */
    //REVIEW: If discard or deliver are not defined, then we have a possible 
    //memory leak here.  Not sure how to handle that.
    //TODO: Make this a function
    msg_info->data = NULL;
    msg_info->length = 0;
    msg_info->index = 0;
  }
  *frame_inx_ptr = frame_inx; //lint !e613 ASSERT has checked the pointer non-null
  return msg_info->state;
}


/*===========================================================================

FUNCTION RXCSAR_FLUSH_STATE

DESCRIPTION
  This function flushes the subframes until flush_cnt is 0, upon which 
  the SCI state is entred.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static rxcsar_csch_state rxcsar_flush_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
)
{
  /* Unused Parameter */
  (void) frame_ptr;

  ASSERT(msg_info);
  ASSERT(frame_inx_ptr);

  *frame_inx_ptr = subframe_end_inx; /* Flush to end of subframe */

  /* Only go back to SCI state if you have completed the specified number of 
     flushes.  Idle handoff sometimes needs multiple flushes to occur prior
     to returning to SCI state. */
  if (msg_info->flush_cnt > 0) {
    msg_info->flush_cnt--;
  }
  
  if (msg_info->flush_cnt == 0) {
    msg_info->state = RXCSAR_SCI_STATE;
  }

  return msg_info->state;
}


/*===========================================================================

FUNCTION RXCSAR_SKIP_STATE

DESCRIPTION
  This function skips the current message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static rxcsar_csch_state rxcsar_skip_state (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  unsigned int *frame_inx_ptr,   /* bit offset of frame_ptr->data */
  unsigned int subframe_end_inx, /* bit index of last bit of subframe */
  rxcsar_msg_info_type *msg_info
)
{
  unsigned int num_bits;
  unsigned int frame_inx, msg_index, msg_length;

  /* Unused Parameter */
  (void) frame_ptr;

  ASSERT(msg_info);
  ASSERT(frame_inx_ptr);

  frame_inx = *frame_inx_ptr;
  msg_index = msg_info->index;
  msg_length = msg_info->length;

  num_bits = MIN ((msg_length - msg_index),
                  (subframe_end_inx - frame_inx));

  frame_inx += num_bits;  /* increment index into subframe */
  msg_index += num_bits;  /* increment index into message buffer */
  
  *frame_inx_ptr = frame_inx;
  msg_info->index = msg_index;

  if (msg_index >= msg_length) {
    msg_info->state = RXCSAR_LEN_STATE;
  }

  return msg_info->state;
}
#endif /* FEATURE_IS2000_REL_A_CC */
