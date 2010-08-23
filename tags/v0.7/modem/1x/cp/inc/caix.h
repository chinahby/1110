#ifndef CAIX_H 
#define CAIX_H
/*===========================================================================

            C D M A   T R A N S L A T I O N    S E R V I C E S    
                    D A T A    D E C L A R A T I O N S 

DESCRIPTION
  This file contains the data structures needed for use in the
  CDMA translation services.
  

Copyright (c) 1990-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/caix.h_v   1.8   01 Oct 2002 15:01:32   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/caix.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/07   pg      Lint Cleanup.
01/30/07   sb      Changed function prototype of caix_get_message_type.
01/18/07   sb      Fixed function prototype.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/29/06   fh      Added a new parameter to caix_int_to_ext to indicate whether
                   the function call is for calculating access channel message 
                   size or for parsing before transmitting the message.
06/12/06   fc      Changes for UMTS.
06/01/06   fc      Cleanup umts interface.
05/17/06   fh      Merged in new development of AKA/AES features
05/16/06   fh      Changes under AES feature: the parser function caix_int_to_ext
                   carries an extra argument to indicate whether to just
                   get the buffer length for access channel messages, or parsing
                   and AES encrypting the whole message.
04/26/06   ht      Added L2 Ack support when parsing failed.
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
07/12/05   ht      Added MEID Addressing support
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
06/03/04   fc      P_REV changes.
04/13/04   bkm     Added IS2000 Release D support.
09/19/03   fc      Added enum for ECAM buffer size sanity check.
07/21/04   sj      Added support for umts to 1x handover.
06/30/03   fc      Added support for IS2000 Release C messages.
02/05/03   fc      Added IS2000 Release B f-csh changes.
02/05/03   fc      Cleanup for f-csh forward compatability.
11/07/02   az      Mainlined FEATURE_IS95B
10/01/02   az      Mainlined FEATURE_IS2000
06/14/02   sb      Do not define xlate_ext_pc_hdr in SILK, causes compile error.
06/12/02   yll     Added xlate_ext_pc_hdr() prototype.
05/22/02   fc      Changes for message pack unit test.
05/20/02   fc      Added support for Universal Page Message.
01/07/02   sb      Only include cai.h and caii.h if we are not in SILK.
11/19/01   fc      Included cai.h and caii.h.
12/21/00   kk/lh   Added IS-2000 release A support. 
06/19/00   yll     Added packing the MSID to be used by PDSM IS801 services.
12/08/99   jw      Basic IS2000 message support.
02/12/99   lcc     Added some IS95B support related to pilot reporting.
11/23/98   ram     merged in IS95B P_REV support from
                   MDR archive. Updated Copyright notice.
01/23/91   jai     Changed name of file to caix.h
10/30/90   jai     Created file.

===========================================================================*/

#ifndef FEATURE_MSG_PACK_UNIT_TEST
#include "cai.h"
#include "caii.h"
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Channels */
typedef enum
{
  CAIX_SC,     /* Sync Channel */
  CAIX_PC,     /* Paging Channel */
  CAIX_AC,     /* Access Channel */
  CAIX_TC      /* Traffic Channel */
#ifdef FEATURE_IS2000_REL_A
  ,
  /* Release A physical channels */
  CAIX_BCCH,    /* Broadcast Control Channel */
  CAIX_FCCCH,   /* Forward Common Control Channel */
  CAIX_REACH,   /* Reverse Enhanced Access Channel */
  CAIX_RCCCH,   /* Reverse Common Control Channel */

  /* Release A logical Channels */
  CAIX_FCSCH,   /* Forward Common Signaling Channel (logical channel ) */
  CAIX_RCSCH,   /* Reverse Common Signaling Channel */
  CAIX_FDSCH,   /* Forward Dedicated Signaling Channel */
  CAIX_RDSCH    /* Reverse Dedicated Signaling Channel */

#endif
} caix_chan_type;

/* Return status */
enum
{
  CAIX_DONE_S = 0,            /* Message has been translated ok */
  CAIX_INV_LEN_S,             /* Message length was invalid */
  CAIX_INV_MSG_S,             /* Received message type was invalid */
  CAIX_INV_CHN_S,             /* Received Channel was invalid */
  CAIX_INV_ENC_MODE_NOT_SUP_S /* Encryption Mode not supported */
#ifdef FEATURE_IS2000_REL_A
  , CAIX_AES_FAIL_DO_NOTHING
  , CAIX_AES_FAIL_DO_RESYNC
  , CAIX_AES_FAIL_DO_SYS_DET
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_A_CC
  , CAIX_INV_PAGE_REC_S       /* Received page record was invalid */
#endif /* FEATURE_IS2000_REL_A_CC */
#ifdef FEATURE_IS2000_REL_C
  , CAIX_MI_FAIL_DO_NOTHING   /* MI failure.. Do nothing */
  , CAIX_MI_FAIL_DO_KEY_RESET /* MI failure.. establish new keys */
  , CAIX_MI_FAIL_DO_RESYNC    /* MI failure.. establish new cryto sync */
  , CAIX_MI_FAIL_DO_SYS_DET   /* MI failure.. go to system determination */
#ifdef FEATURE_IS2000_REL_D
#error code not present
#endif /* FEATURE_IS2000_REL_D */
#endif /* FEATURE_IS2000_REL_C */
#ifdef FEATURE_MSG_PACK_UNIT_TEST
#error code not present
#endif /* FEATURE_MSG_PACK_UNIT_TEST */
};

/* Layer2 fields packing */
typedef enum
{
  CAIX_L2_FIELDS_PACK     = 0, /* Layer 2 fields are packed. */
  CAIX_L2_FIELDS_NOT_PACK = 1  /* Lsyer 2 fields are not packed. */
} caix_l2_fields_pack_type;

enum
{
  /* the following constant is used to indicate that the function call
   * of caix_int_to_ext is to really parsing the message before 
   * transmission. This means the binary stream will go through the full
   * AES procedure
   */
  CAIX_PARSE_MSG = 0, 

  /* the following constant is used to indicate that the function call
   * of caix_int_to_ext is to get the message length only
   */
  CAIX_CALC_MSG_LENGTH
};

#define CAIX_MIN_LEN_FOR_L2_ACK     3  /* bytes */
        /* msgtype (1 byte), 
         * msg_seq (3 bits),
         * ack_seq (3 bits), 
         * ack_req (1 bit),
         * encryption (2 bits)
         * total 2+ bytes (rounded up to 3 bytes)
         */

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/* Procedure which translates messgaes from external to internal format */
extern word caix_ext_to_int
       (
       word len,
         /* message length in bits */
       word chn,                    
         /* channel message was received on */
       cai_rx_msg_type *rx_msg_ptr,    
         /* pointer to received message to be translated */
       caii_rx_msg_type *int_msg_ptr   
         /* pointer to place where translated message should be placed */
       );

/* Procedure which translates messgaes from internal to external format */
extern word caix_int_to_ext
       (
       word *len_ptr,
         /* pointer to where message length, in bits should be placed */
       word chn,                   
         /* channel message is to be transmitted on */
       caii_tx_msg_type *int_msg_ptr,  
         /* pointer to internal message to be tanslated */
       cai_tx_msg_type *tx_msg_ptr,
         /* pointer to place where translated message should be placed */
       uint8 parsing_reason
       /* This flag indicates the true reason of why this function is 
        * getting called. Currently there are two reasons defined:
        *
        * CAIX_CAL_AC_MSG_LENGTH:
        * this reason is used to indicate that the function call
        * of caix_int_to_ext is to get the message length only
        *
        * CAIX_PARSE_AC_MSG:
        * this reason is used to indicate that the function call
        * of caix_int_to_ext is to really parsing the message before 
        * transmission. This means the binary stream will go through the full
        * AES procedure
        */
       );

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#ifndef FEATURE_MSG_PACK_UNIT_TEST
extern void xlate_ext_pc_hdr
(
  word *buf_pos,
    /* Position of header fields relative to start of buf_ptr */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_pc_hdr_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

extern void xlate_ext_tc_hdr
(
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  caii_tc_hdr_type *int_ptr
    /* Pointer to place where translated message should be placed */
);
#endif /* ifndef FEATURE_MSG_PACK_UNIT_TEST */

#define CAIX_P_REV_UNKNOWN  0
  /* Used to tell layer 3 that a message is in old format ( Pre 95B ) */

#define CAIX_P_REV_IS95B    4
  /* Minimum P_REV for IS95B phones */

#define CAIX_P_REV_IS2000   6
  /* Minimum P_REV for IS2000 phones */

#define CAIX_P_REV_IS2000_REL_A 7
  /* Minimum P_REV for IS2000 Release A capable phones */

#define CAIX_P_REV_IS2000_REL_B 8
  /* Minimum P_REV for IS2000 Release B capable phones */

#define CAIX_P_REV_IS2000_REL_C 9
  /* Minimum P_REV for IS2000 Release C capable phones */

#define CAIX_P_REV_IS2000_REL_C_MI 10
  /* Minimum P_REV for IS2000 Release C message integrity capable phones */

#define CAIX_P_REV_IS2000_REL_D 11
  /* Minimum P_REV for IS2000 Release D capable phones */

void caix_set_p_rev_in_use(byte p_rev_in_use);

#ifndef FEATURE_MSG_PACK_UNIT_TEST
#ifdef FEATURE_IS2000_REL_A_CC
/*===========================================================================

FUNCTION XLATE_EXT_UNIV_PAGE_INTERLEAVED_ADDR_FIELD

DESCRIPTION
  This function translates interleaved address fields of Universal Page from
  external to internal format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

word xlate_ext_univ_page_interleaved_addr_field
(
  caii_univ_page_field_type msg_field,
    /* Translation state */
  byte *buf_ptr,
    /* Pointer to received message to be translated */
  word *offset,
    /* Offset from the buffer pointer to start translation */
  caii_univ_page_interleaved_addr_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/*===========================================================================

FUNCTION XLATE_EXT_UNIV_PAGE_RECORD

DESCRIPTION
  This function translates a Universal Page Record from external to internal
  format.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern word xlate_ext_univ_page_record
(
  byte *buf_ptr,
    /* Pointer to received universal page records block to be translated */
  word *offset,
    /* Pointer to bit offset to start translation of received page record */
  caii_univ_page_rec_type *int_ptr
    /* Pointer to place where translated message should be placed */
);

/*===========================================================================

FUNCTION XLATE_EXT_SEGMENTED_UNIV_PAGE

DESCRIPTION
  This function translates segmented Universal Page from external to 
  internal format. Only common fields and interleaved address fields (other 
  than address block) are translated in this routine.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  None

===========================================================================*/

extern word xlate_ext_segmented_univ_page
(
  caii_seg_univ_page_type *seg_ptr
    /* Pointer to segmented Universal Page assembly buffer */
);
#endif /* FEATURE_IS2000_REL_A_CC */
#endif /* !FEATURE_MSG_PACK_UNIT_TEST */

#ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */

#ifdef FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X
#error code not present
#endif /* FEATURE_VOIP_ON_HDR_HANDOFF_TO_1X */

/*===========================================================================

FUNCTION CAIX_GET_MESSAGE_TYPE

DESCRIPTION
  This function peeks at the first byte of a bitstream to determine the 
  message type.

DEPENDENCIES
  None

RETURN VALUE
  Errors that were encountered while translating the message.

SIDE EFFECTS
  Msg_type will hold the value of the message type.

===========================================================================*/
word caix_get_message_type
(
  byte * msg_type,
  /* This will hold the message type of the message */
  word chn,
  /* Channel message was received on */
  byte *buf_ptr
  /* Pointer to received message to be translated */
);

#endif /* CAIX_H */

