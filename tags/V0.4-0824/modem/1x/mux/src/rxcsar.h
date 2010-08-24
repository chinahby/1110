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

Copyright (c) 2001 through 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                              Edit History

$PVCSPath: O:/src/asw/COMMON/vcs/rxcsar.h_v   1.4   21 Oct 2002 13:19:22   sanjayr  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/rxcsar.h#2 $ $DateTime: 2008/09/09 07:26:45 $ $Author: joshuam $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/08/08   jtm     Added RXCSAR_MAX_EXPECTED_MSG_LENGTH. 
03/20/03   bn,sr   Added logic to support simultaneously monitor BCCH and FCCCH
12/10/01   bn      Moved the subframe counter field to the end of the structure
                   to avoid the alignment problem 
10/21/02   bn      Added support for NCC statistic counters
10/21/02   sr      Surrounded the file with #ifdef FEATURE_IS2000_REL_A_CC
04/05/02   lad     Modified SAR interface to use const info in msg_desc.
04/01/02   lad     Added message logging.
01/17/02   lad     Updated API.  Added rxcsar_terminate().
11/06/01   lad     Created file.

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_IS2000_REL_A_CC

#include "log.h"
#include "qw.h"

/* Defines the maximum message size we expect from the BS */
#define RXCSAR_MAX_EXPECTED_MSG_LENGTH 512

/*===========================================================================

FUNCTION TYPE RXCSAR_MSG_ALLOC

DESCRIPTION
  This function, provided via reference by the caller, allocates a
  buffer into which L2 will copy a message from a MAC layer frame.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to allocated data buffer.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void * (*rxcsar_msg_alloc) (
  void *client_data,
  unsigned int length /* In bytes */
);


/*===========================================================================

FUNCTION TYPE RXCSAR_MSG_DELIVER

DESCRIPTION
  This function, provided via reference by the caller, delivers a completed
  message to the client.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*rxcsar_msg_deliver) (
  void *client_data,
  unsigned int msg_bit_length, /* Message length in bits. */
  void *data, /* from corresponding rxcsar_msg_alloc() call */
  qword *frame_num
);


/*===========================================================================

FUNCTION TYPE RXCSAR_MSG_DISCARD

DESCRIPTION
  This function, provided via reference by the caller, discards a
  buffer returned from rxcsar_msg_alloc().

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef void (*rxcsar_msg_discard) (
  void *client_data,
  void *data /* from corresponding rxcsar_msg_alloc() call */
);

/*===========================================================================

FUNCTION TYPE RXCSAR_MSG_EVENT

DESCRIPTION
  This function, provided via reference by the caller, reports an event
  (such as a CRC failure) that may occur during processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
typedef enum {
  RXCSAR_CRC_FAIL_S
} rxcsar_msg_event_type;

typedef void (*rxcsar_msg_event) (
  void *client_data,
  rxcsar_msg_event_type event
);


/* Physical layer frame descriptor type. */
#define RXCSAR_FRAME_DESC_OPAQUE_DATA_SIZE 8 /* count of integers */
typedef struct {
  qword *frame_num; /* Frame number. */
  
  unsigned int frame_size; /* Number of bits in a physical layer frame */
  void *data;              /* Frame data */
  unsigned int sci_size;      /* Size, in bits, of an SCI for this frame */
  unsigned int total_subframe;

  /* Since this system does not have dynamic memory allocation, an opaque
     storage buffer is allocated here for internal use. */
  unsigned int opaque_data[RXCSAR_FRAME_DESC_OPAQUE_DATA_SIZE];
  unsigned int subframe_size; /* Size, in bits, of a subframe, including SCI */

} rxcsar_fcsch_frame_desc_type;


#define RXCSAR_MSG_DESC_OPAQUE_DATA_SIZE 12 /* Count of integers */

/* Constant data in the message descriptor */
typedef struct {
  /* Code under which to log the raw message.  Set to -1 if unused. */
  log_code_type log_code; 

  boolean ext_msg_length_ind; /* Is Extended Message Length field supported? */
  unsigned int min_msg_len; /* In bytes */

  const rxcsar_msg_alloc   alloc;
  const rxcsar_msg_deliver deliver;
  const rxcsar_msg_discard discard;
  const rxcsar_msg_event   event;

} rxcsar_msg_const_desc_type;

typedef struct {
  void *client_data; /* Pointer to client-defined structure, passed unmodified
                      * in buffer management routines to provide closure to 
                      * async processing. */

  const rxcsar_msg_const_desc_type *static_data;

  byte         *crc_comp_ptr;
  uint16       crc_comp_numbits;
  uint16       crc_comp_length_numbits;

  /* private data is opaque to the caller */
  unsigned int opaque_data[RXCSAR_MSG_DESC_OPAQUE_DATA_SIZE];
} rxcsar_msg_desc_type;


/*===========================================================================

FUNCTION RXCSAR_INIT_MSG_DESCRIPTOR

DESCRIPTION
  This procedure initializes a message descriptor.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcsar_init_msg_descriptor (
  rxcsar_msg_desc_type *msg_ptr
);


/*===========================================================================

FUNCTION RXCSAR_PROCESS_FCSCH_FRAME

DESCRIPTION
  This procedure performs MAC processing on physical layer frames, defined by
  f-csch (LAC) logical channel frame given MAC physical layer frames.

DEPENDENCIES
  'msg_ptr' may be a continuation of a previous frame.  If multiple frames
  are to be processed, it is done by calling this function repeatedly with
  same 'msg_ptr' structure.  This is required since messages can span a 
  frame boundary.  When collection of messages in complete, terminate the 
  process by calling 'rxcsar_terminate()'

RETURN VALUE
  Success of frame processing.

SIDE EFFECTS
  None.

===========================================================================*/
boolean
rxcsar_process_fcsch_frame (
  rxcsar_fcsch_frame_desc_type *frame_ptr,
  rxcsar_msg_desc_type *msg_ptr
);


/*===========================================================================

FUNCTION RXCSAR_TERMINATE

DESCRIPTION
  This procedure terminates a frame processing session by freeing up any
  allocated memory.  This is necessary when processing is complete to 
  avoid memory leaks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void
rxcsar_terminate (
  rxcsar_msg_desc_type *msg_ptr,
  uint8 subframe_flush_cnt /* Number of future subframes to flush */
);
#endif /* FEATURE_IS2000_REL_A_CC */
