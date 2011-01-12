#ifndef MUX_LOGTYPES_H
#define MUX_LOGTYPES_H

/*===========================================================================

                      MUX Log Services Header File

General Description
  This file contains declarations for packets to be logged for the MUX player.

Copyright (c) 2001 through 2005 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                             Edit History

$PVCSPath: L:/src/asw/COMMON/vcs/mux_logtypes.h_v   1.7   20 Aug 2002 11:27:12   randyp  $
$Header: //source/qcom/qct/modem/1x/mux/rel/ulc/src/mux_logtypes.h#1 $ $DateTime: 2007/11/15 15:12:49 $ $Author: glenns $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.                                   
04/20/04   jrp     Fixed lint warnings.
11/15/02   jrp     Added band class to LOG_EA_PROBE_F packet.
08/20/02   jrp     Added log type for REACH probe, log code BD.
07/10/02   dlb2    Featurized previous change with MSMHW_EXTENDED_ENERGY_MATRIX.
04/24/02   dlb2    Increased size of ser field for sch frame info to 16 bits.
01/09/02   bn      Added the frame offset into RDA log
11/20/01   bn      Added RDA logging logic
09/05/01   bn      Created the REL A Logging header file for the MUX layer
                   
===========================================================================*/

#include "cai.h"
#include "log.h"
#include "nv.h"
#include "target.h"
#include "comdef.h"


#ifdef FEATURE_PLT
#error code not present
#endif

#include "log_codes.h"

/* This buffer can hold up to a duration up to 200 ms */
#define FRDCH_LOG_BUFFER_SIZE (256)
/*the threshold = the buffer size - the maximum of byte per record in the 
worst case analysic */
#define FRDCH_LOG_BUFFER_THRESHOLD (FRDCH_LOG_BUFFER_SIZE - 32)

/******************         COMMON MUX LOGGING TYPES            *****************/

typedef union
{
  struct
  {
    uint8 rpich     :1;
    uint8 fch       :1;
    uint8 dcch      :1;
    uint8 sch0      :1;
    uint8 scch      :3;
    uint8 reserved  :1;
  } individual_channels;
  uint8 mask; /* Use to init entire mask. */

} channels_info_type;

/*****************  FDCH FRAME INFORMATION **********************/
typedef struct
{
  uint8  reference_buf[1];

  /*
  Fill the Reference Buffer with the following:
  
  - If FCH is assigned, include 1 byte for FCH_CATEGORY
  
  - If DCCH is assigned, include 1 byte for DCCH_CATEGORY
  
  - If SCCH is assigned, include as many bytes as the number of assigned SCCH, for SCCH_FRAME_CATEGORY
  
  - If SCH is assigned, include 1 byte. The possible value for the SCH frame categories are:
    sufficient quality, insufficient quality or DTX
  
  - If SCH is assigned, and SCH frame category is insufficient quality and LTU number is greater than
    zero, then including 1 byte of LTU CRC */

} fdch_frame_type;

typedef struct
{
  uint8  for_sch_rc;    
  uint8  for_sch0_mux_option_hi;
  uint8  for_sch0_mux_option_lo;
  uint8  sch_frame_length;
  uint8  sch0_num_bits;
  uint8  num_mux_pdu_per_sch_frame;
  uint8  num_ltu_per_sch_frame;

} fwd_sch_information_type;

typedef struct
{
  channels_info_type  assigned_channels;
  uint8           num_frames;
  uint8           reference_buf[1];
  /*
  Fill the Reference Buffer with the following:
  
  - If FCH is assigned, include 1 byte for FOR_FCH_RC
  
  - If DCCH is assigned, include 1 byte for FOR_DCCH_RC
  
  - If SCH is assigned, include 1 instance of FWD_SCH_INFORMATION_TYPE
  
  - Include NUM_FRAMES instances of FDCH_FRAME_TYPE
  */  
} fdch_sub_record_type;


typedef struct
{
  log_hdr_type hdr;    // Log header (length, code, timestamp)
  uint8        frame_offset;
  uint8        num_sub_records;
  uint8        reference_buf[1];

  /*
  Fill the Reference Buffer with NUM_SUB_RECORDS instances of FDCH_SUB_RECORD_TYPE
  */
  
} log_fdch_information_type;


/*****************  RDCH FRAME INFORMATION  **********************/

typedef struct
{
  uint8 sr_id;  //THIS FIELD IS NOT INCLUDED IF MUX_PDU_CATEGORY INDICATES A DTX SCH FRAME

} r_sch_mux_pdu_information_type;


typedef struct
{
  uint8  reference_buf[1];

  /*
  Fill the Reference Buffer with the following:
  
  - If FCH is assigned, include 1 byte for FCH_FORMAT
  
  - If DCCH is assigned, include 1 byte for DCCH_FORMAT
  
  - If SCH is assigned, include NUM_MUX_PDU_PER_SCH_FRAME instances of R_SCH_MUX_PDU_INFORMATION_TYPE
      
  */
  
} rdch_frame_type;

typedef struct
{
  uint8  rev_sch_rc;
  uint8  rev_sch0_mux_option_hi;
  uint8  rev_sch0_mux_option_lo;
  uint8  sch_frame_length;
  uint8  sch0_num_bits; 
  uint8  num_mux_pdu_per_sch_frame;
} rev_sch_information_type;

typedef struct
{
  channels_info_type  assigned_channels;
  uint8           num_frames;
  uint8           reference_buf[1];
  /*
  Fill the Reference Buffer with the following:
  
  - If FCH is assigned, include 1 byte for REV_FCH_RC and 1 byte for FCH_GATING.
  
  - If DCCH is assigned, include 1 byte for REV_DCCH_RC
    
  - If SCH is assigned, include 1 instance of REV_SCH_INFORMATION_TYPE
  
  - Include NUM_FRAMES instances of RDCH_FRAME_TYPE
  */
  
} rdch_sub_record_type;



typedef struct
{
  log_hdr_type hdr;    // Log header (length, code, timestamp)
  uint8        frame_offset;
  uint8        num_sub_records;
  uint8        reference_buf[1];
  /*
  Fill the Reference Buffer with NUM_SUB_RECORDS instances of RDCH_SUB_RECORD_TYPE
  */
  
} log_puma_rdch_information_type;

/*****************  RDA FRAME INFORMATION **********************/

/* This buffer can hold up to a duration up to 200 ms */
#define RDA_LOG_BUFFER_SIZE (512)
/*the threshold = the buffer size - the maximum of byte per record in the 
worst case analysic */
#define RDA_LOG_BUFFER_THRESHOLD (RDA_LOG_BUFFER_SIZE - 72)

typedef struct
{
  uint8   quality_decision     :1;
  uint8   status               :2;
  uint8   reserve              :5;
} dcch_quadec_status_type;

typedef union
{
  struct
  {
    uint16   fch_rc       :3;
    uint16   dcch_rc      :3;
    uint16   sch_rc       :3;
    uint16   frame_offset :7;
  } chan_rc;
  uint16  mask;
} chan_rc_type;

typedef union
{
  struct
  {
    uint16  ltu_crc;
    uint16  ser;
    uint8   status; /* frame CRC */
    uint8   reserved[3];
  } dev_sch_conv;

  struct
  {
    uint16  min_llr;       /* Log Likelihood Ratio */
    uint8   num_iteration; /* Indicates the number of iterations run by the TD */
    uint8   crc_pass;
    uint8   reserved[4];
  } dev_sch_turbo;
} sch_code_type;

typedef struct
{
  uint32    norm_ren_energy; 
  uint16    ser;      /* symbol error rate */
  uint8     decision; /* Full, Haft, Quater, Eight, Erasure */
  uint8     status;

} fch_rda_sub_record_type;

typedef struct
{
  uint32                      norm_ren_energy; 
  uint32                      quality_threshold;
  uint16                      fch_nt_io;
  uint16                      pc_acc;
  uint16                      ser;      /* symbol error rate */
  uint8                       decision; /* Full, DTX, Erasure */
  dcch_quadec_status_type     quastat;

} dcch_rda_sub_record_type;

typedef struct
{
  uint32         norm_ren_energy; 
  sch_code_type  code_info;
  uint16         iq_acc;
  uint16         sch_nt_io;
  uint16         rate;
  uint8          decision;  /* Full, DTX, Erasure */
  uint8          code_type; /* turbo or conv */
} sch_rda_sub_record_type;

typedef struct
{
  chan_rc_type        rc;                    /* RC structure for all the channels and frame offset*/
  channels_info_type  assigned_channels;     /* Channels bit-mask */
  uint8               num_sub_records;
  uint8               reference_buf[1];      /* reference buffer */
  /*  For each 20 ms time framce:
  Fill the Reference Buffer with NUM_SUB_RECORDS instances of
  - If FCH is active then there are 4 fch_rda_sub_record_type 
  sub-records( Full, Haft, Quater, and Eight)
  - If DCCH is active then there is 1 dcch_rda_sub_record_type sub-record
  - If SCH is active then there is 1 sch_rda_sub_record_type sub-record
  */
} rda_sub_record_header_type;

typedef struct
{
  log_hdr_type                  hdr;      /* Log header (length, code, timestamp) */
  rda_sub_record_header_type    srheader;
} log_puma_rda_information_type;



/*****************  BCCH FRAME INFORMATION  **********************/
/* LOG_BCCH_FRAME_INFO_C */
typedef PACKED struct {
  log_hdr_type hdr;

  byte data_rate;
  byte code_rate;
  byte code_chan;

  byte num_frames;
  byte frame_quality[1]; /* Array size '(num_frames + 7) /8 */

} mux_bcch_frame_info_type;


/*****************  FCCCH FRAME INFORMATION  **********************/
/* LOG_BCCH_FRAME_INFO_C */
typedef PACKED struct {
  log_hdr_type hdr;

  byte data_rate;
  byte code_rate;
  byte code_chan;

  byte num_frames;
  byte frame_quality[1]; /* Array size '(num_frames + 7) /8 */

} mux_fccch_frame_info_type;


/*****************  FRAME OR MESSAGE DATA LOG  **********************/
/* Used for logging a raw message or frame data */
typedef PACKED struct {
  log_hdr_type hdr;

   byte msg[1]; /* Length to be determined programmatically */
} mux_msg_log_type;

/*****************  REACH FRAME INFORMATION  **********************/
typedef struct 
{
  log_hdr_type  hdr;
  
  /* Gain adjustment of the Enhanced Access Channel or Reverse Common 
   * Control Channel relative to the Reverse Pilot Channel.  This field
   * is received on the Enhanced Access Parameters Message.  It is a two's
   * complement value in units of 0.125 dB. */
  uint8         rlgain_com_pilot;    

  /* Access sub-attempt number of the access attempt where the access 
   * probe belongs to.  There is a sub-attempt for each BS to which an 
   * Access probe is sent. */
  uint8         sub_att_number;

  /* Sequence number of the access sub-attempt. */
  uint8         sequence_number;

  /* Probe number of the access sequence corresponding to the access probe. */
  uint8         access_prob_number;

  /* One less than the maximum number of access probes a mobile station is 
   * to transmit in a single access probe sequence. */
  uint8         each_num_step;

  /* Number of Persistence Tests Performed. */
  uint8         psist_delay;

  /* Enhanced Access Channel sequence backoff 
   * One less than the maximum number of slots a mobile station is to
   * delay due to random backoff between successive EA probe sequences. */
  uint8         sequence_bkoff;

  /* Enhanced Access Channel probe backoff. 
   * One less than the maximum number of slots a mobile station is to
   * delay due to random backoff between consecutive EA probes. */
  uint8         probe_bkoff;

  /* N, where the slot duration of the Enhanced Access Channel is (N+1) x 1.25ms. */
  uint8         each_slot;

  /* Forward Common Control Channel number. */
  uint8         fccch_id;

  /* Enhanced Access Channel identification */
  uint8         each_id;

  /* First slot offset for the Enhanced Access Channels.  This field is 
   * received in the Enhanced Access Parameters Message. */
  uint8         each_slot_offset1;

  /* Base station identification. */
  uint16        base_id;

  /* Relative slot offset for the Enhanced Access Channels.  This field is 
   * received in the Enhanced Access Parameters Message. */
  uint8         each_slot_offset2;

  /* Enhanced Access Channel rate selection for transmission.  This field 
   * can be coded as RATE_WORD, in Table 2-29 of IS-2000-3, which describes 
   * the rate and frame length and number of information bits per frame. */
  uint8         acc_rate;

  /* Received AGC power. */
  uint8         rx_agc;

  /* Transmit AGC power. */
  uint8         tx_agc;

  /* Transmit gain adjust. */
  uint8         tx_adj;

  /* Interference-correction */
  uint8         int_corr;

  /* floor(SYS_TIME/EACH_SLOTs), where SYS_TIME is the system time in units 
   * of 1.25ms, at which the preamble is to be transmitted.  This field is
   * the lower 32 bits of this 42 bit value. */
  uint32        acc_preamble_tx_low32bit;

  /* This field is the high 10 bits of the 42 bit value, whose lower 32 bits
   * is the acc_preamble_tx_low32bit field. */
  uint16        acc_preamble_tx_high16bit;

  /* Length in bits of the transmitted access probe. */
  uint16        pdu_length;

  /* CDMA band class (cellular or PCS) */
  uint8         band_class;

} mux_reach_frame_info_log_type;


#endif /* MUX_LOGTYPES_H */

