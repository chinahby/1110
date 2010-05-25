#ifndef AMRSUP_H
#define AMRSUP_H

/*===========================================================================

                    A M R   F R A M I N G   P R O C E S S I N G
                    S U P P L E M E N T A L   F U N C T I O N S

DESCRIPTION
  This header file contains definitions used by the AMR framing functions.

REFERENCES
  3G TS 26.093 - "AMR Speech Codec; Source Controlled Rate Operation"
  3G TS 26.101 - "AMR Speech Codec; Frame structure"

Copyright (c) 2003 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/amrsup/main/latest/src/amrsup.h#1 $ $DateTime: 2008/05/19 03:40:20 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/07   kan     Added AMR-WB changes 
07/22/05   ymc     Added FEATURE_AMR_IF1 support.
           ymc     Updated Copyright.
08/18/04   ymc     Added crc_err in amrsup_de_framing.
           ymc     Updated Copyright.
           ymc     Added amrsup_move_valid_bits, amrsup_get_frame_type, 
                   amrsup_frame_class_bits, amrsup_frame_len_bits.
06/29/03   ymc     Added amrsup_frame_len.
           ymc     Added frame length return for amrsup_if2_framing and
                   amrsup_if2_de_framing.
04/17/03    sm     Initial revision taken from mvsamr.h.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#include "msg.h"

/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/
/* AMR frame type definitions */
typedef enum {
  AMRSUP_SPEECH_GOOD,          /* Good speech frame              */
  AMRSUP_SPEECH_DEGRADED,      /* Speech degraded                */
  AMRSUP_ONSET,                /* onset                          */
  AMRSUP_SPEECH_BAD,           /* Corrupt speech frame (bad CRC) */
  AMRSUP_SID_FIRST,            /* First silence descriptor       */
  AMRSUP_SID_UPDATE,           /* Comfort noise frame            */
  AMRSUP_SID_BAD,              /* Corrupt SID frame (bad CRC)    */
  AMRSUP_NO_DATA,              /* Nothing to transmit            */
  AMRSUP_SPEECH_LOST,          /* Lost speech in downlink        */
  AMRSUP_FRAME_TYPE_MAX
} amrsup_frame_type;

/* AMR frame mode (frame rate) definitions */
typedef enum {
  AMRSUP_MODE_0475,    /* 4.75 kbit /s */
  AMRSUP_MODE_0515,    /* 5.15 kbit /s */
  AMRSUP_MODE_0590,    /* 5.90 kbit /s */
  AMRSUP_MODE_0670,    /* 6.70 kbit /s */
  AMRSUP_MODE_0740,    /* 7.40 kbit /s */
  AMRSUP_MODE_0795,    /* 7.95 kbit /s */
  AMRSUP_MODE_1020,    /* 10.2 kbit /s */
  AMRSUP_MODE_1220,    /* 12.2 kbit /s */
  AMRSUP_MODE_0660,      /* AMR-WB 6.60 kbps */
  AMRSUP_MODE_0885,      /* AMR-WB 8.85 kbps */
  AMRSUP_MODE_1265,      /* AMR-WB 12.65 kbps */
  AMRSUP_MODE_1425,      /* AMR-WB 14.25 kbps */
  AMRSUP_MODE_1585,      /* AMR-WB 15.85 kbps */
  AMRSUP_MODE_1825,      /* AMR-WB 18.25 kbps */
  AMRSUP_MODE_1985,      /* AMR-WB 19.85 kbps */
  AMRSUP_MODE_2305,      /* AMR-WB 23.05 kbps */
  AMRSUP_MODE_2385,      /* AMR-WB 23.85 kbps */
  AMRSUP_MODE_MAX
} amrsup_mode_type;
  
/* The AMR classes
*/
typedef enum  {
  AMRSUP_CLASS_A,
  AMRSUP_CLASS_B,
  AMRSUP_CLASS_C
} amrsup_class_type;

/* The maximum number of bits in each class */
#define AMRSUP_CLASS_A_MAX 81
#define AMRSUP_CLASS_B_MAX 405
#define AMRSUP_CLASS_C_MAX 60

/* The size of the buffer required to hold the vocoder frame */
#define AMRSUP_VOC_FRAME_BYTES  \
  ((AMRSUP_CLASS_A_MAX + AMRSUP_CLASS_B_MAX + AMRSUP_CLASS_C_MAX + 7) / 8)

/* Size of each AMR class to hold one frame of AMR data */
#define AMRSUP_CLASS_A_BYTES ((AMRSUP_CLASS_A_MAX + 7) / 8)
#define AMRSUP_CLASS_B_BYTES ((AMRSUP_CLASS_B_MAX + 7) / 8)
#define AMRSUP_CLASS_C_BYTES ((AMRSUP_CLASS_C_MAX + 7) / 8)


/* Number of bytes for an AMR IF2 frame */
#define AMRSUP_IF2_FRAME_BYTES 32

/* Frame types for 4-bit frame type as in 3GPP TS 26.101 v3.2.0, Sec.4.1.1 */
typedef enum {
  AMRSUP_FRAME_TYPE_INDEX_0475    = 0,    /* 4.75 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0515    = 1,    /* 5.15 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0590    = 2,    /* 5.90 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0670    = 3,    /* 6.70 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0740    = 4,    /* 7.40 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0795    = 5,    /* 7.95 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1020    = 6,    /* 10.2 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1220    = 7,    /* 12.2 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_AMR_SID = 8,    /* SID frame       */
  AMRSUP_FRAME_TYPE_INDEX_0660    = 0,    /* 6.60  kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0885    = 1,    /* 8.85  kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1265    = 2,    /* 12.65 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1425    = 3,    /* 14.25 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1585    = 4,    /* 15.85 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1825    = 5,    /* 18.25 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1985    = 6,    /* 19.85 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_2305    = 7,    /* 23.05 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_2385    = 8,    /* 23.85 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_AWB_SID = 9,    /* AWB SID frame   */
  AMRSUP_FRAME_TYPE_INDEX_AWB_SPL = 14,   /* AWB Speech Lost frame */
  AMRSUP_FRAME_TYPE_INDEX_NO_DATA = 15,   /* No data         */
  AMRSUP_FRAME_TYPE_INDEX_MAX,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF = AMRSUP_FRAME_TYPE_INDEX_MAX
} amrsup_frame_type_index_type;

#define AMRSUP_FRAME_TYPE_INDEX_MASK         0x0F /* All frame types */
#define AMRSUP_FRAME_TYPE_INDEX_SPEECH_MASK  0x07 /* Speech frame    */

typedef enum {
  AMRSUP_CODEC_AMR_NB,
  AMRSUP_CODEC_AMR_WB,
  AMRSUP_CODEC_MAX
} amrsup_codec_type;

/* IF1-encoded frame info */
typedef struct {
  amrsup_frame_type_index_type frame_type_index;
  boolean fqi;    /* frame quality indicator: TRUE: good frame, FALSE: bad */
  amrsup_codec_type amr_type;   /* AMR-NB or AMR-WB */
} amrsup_if1_frame_info_type;


#ifdef FEATURE_MVS
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_MVS */


/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

#ifdef FEATURE_MVS
#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_MVS */


/* <EJECT> */
#ifdef FEATURE_AMR_IF1
/*===========================================================================

FUNCTION amrsup_if1_framing

DESCRIPTION
  Performs the transmit side framing function.  Generates AMR IF1 ordered data
  from the vocoder packet and frame type.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of encoded frame.
  if1_frame : IF1-encoded frame.
  if1_frame_info : holds frame information of IF1-encoded frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_if1_framing(
  uint8                      *vocoder_packet,
  amrsup_frame_type          frame_type,
  amrsup_mode_type           amr_mode,
  uint8                      *if1_frame,
  amrsup_if1_frame_info_type *if1_frame_info
);
  
/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_if1_de_framing

DESCRIPTION
  Performs the receive side de-framing function. Generates a vocoder packet
  and frame type information from the AMR IF1 input data.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of decoded frame.
  vocoder_packet : decoded packet
  frame_type     : AMR frame type of decoded packet
  amr_mode       : AMR frame rate of decoded packet

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_if1_de_framing(
  uint8                      *vocoder_packet,
  amrsup_frame_type          *frame_type,
  amrsup_mode_type           *amr_mode,
  uint8                      *if1_frame,
  amrsup_if1_frame_info_type *if1_frame_info
);

#endif  /* FEATURE_AMR_IF1 */

/* <EJECT> */

#ifdef FEATURE_AMR_IF2
/*===========================================================================

FUNCTION amrsup_if2_framing

DESCRIPTION
  Performs the transmit side framing function.  Generates AMR IF2 ordered data
  from the vocoder packet and frame type.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of encoded frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_if2_framing(
  uint8              *vocoder_packet,
  amrsup_frame_type  frame_type,
  amrsup_mode_type   amr_mode,
  uint8              *if2_frame
);


/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_if2_de_framing

DESCRIPTION
  Performs the receive side de-framing function. Generates a vocoder packet
  and frame type information from the AMR IF2 input data.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of decoded frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_if2_de_framing(
  uint8              *vocoder_packet,
  amrsup_frame_type  *frame_type,
  amrsup_mode_type   *amr_mode,
  uint8              *if2_frame
);
#endif /* FEATURE_AMR_IF2 */


/* <EJECT> */
#ifdef FEATURE_AUDFMT_AMR
/*===========================================================================

FUNCTION amrsup_audfmt_framing

DESCRIPTION
  Performs the transmit side framing function. Generates class-divided and
  ordered data from the vocoder packet and frame type input data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void amrsup_audfmt_framing(
  uint8              *vocoder_packet,
  amrsup_frame_type  frame_type,
  amrsup_mode_type   amr_mode,
  uint8              *amr_data
);


/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_audfmt_de_framing

DESCRIPTION
  Performs the receive side de-framing function. Generates a vocoder packet
  and frame type information from the class-divided and ordered input data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void amrsup_audfmt_de_framing(
  uint8              *vocoder_packet,
  amrsup_frame_type  frame_type,
  amrsup_mode_type   amr_mode,
  uint8              *amr_data
);
#endif /* FEATURE_AUDFMT_AMR */


/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_frame_class_bits

DESCRIPTION
  This function will determine number of bits of certain class
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bits of AMR frame in certain class

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_frame_class_bits(
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode,
  amrsup_class_type amr_class
);


/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_frame_len_bits

DESCRIPTION
  This function will determine number of bits of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bits of AMR frame

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_frame_len_bits(
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode
);


/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_frame_len

DESCRIPTION
  This function will determine number of bytes of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of AMR frame

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_frame_len(
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode
);

#endif  /* AMRSUP_H */

