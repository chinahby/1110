#ifndef _DSRLP_H
#define _DSRLP_H

/*===========================================================================

  D A T A   S E R V I C E S   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
   Radio Link Protocol.

   This should be included by files which interface with RLP, like the
   MuxLayer and Call Processing.

   Note, order of fields in structs is optimized to save memory in 32bit
   ARM environment. This should be kept in mind when changing the ordering
   of fields.

Copyright (c) 1999-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp.h_v   1.29   31 Jan 2003 14:39:18   sarithay  $
  $Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/inc/dsrlp.h#1 $ $DateTime: 2008/04/11 07:35:12 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/05    gr     Fix to handle the race condition of simultanneous RLP 
                   init and cleanup while dropping voice during SVD scenario
09/08/04   vr      Added functionality to notify the pzid hysteresis
                   engine about the first ppp data exchange
07/12/04   gr      Added contiguous tx and rx idle frame count to RLP stats
8/25/03    junz    Change the RLP stats Get and Clear functions to fix data
                   aborts due to invalid SR_ID
05/21/03   vas     Added support for MMHDR data.Added dsrlp_reset and changed
                   index for AN auth RLP.
04/10/03   sy      Added enum dsrlp_tx_sig_enum_type to support SVD 
                   algorithm under REL_A feature.
01/31/03   sy      Changed funtion name dsrlp_next_turbo_pdu5_chain() to
                   dsrlp_get_next_dsm_item_chain().
12/18/02   atp     Implemented change in standard 707 for QOS.
12/02/02   sy      Added funtion dsrlp_insert_pdu5null_hdr_turbo.
10/03/02   vas     Added function dsrlp_is_hdr()
09/17/02   ak      Added header for dsrlp_is_inited(). (chked in js). 
07/17/02   js      Added dsrlp_next_turbo_pdu5_chain() to be used by MUX
                   to resolve a turbo encoded MUX PDU5 chain of DSM items.
07/15/02   ak      Under FEATURE_DATA_MM, FEATURE_HDR_AUTH_ID sr_id index
                   is available.
06/06/02   sy      Added max_frame_rate and data_on_ch fields to 
                   dsrlp_rev_frame_type structure so that mux can 
                   pass more information to RLP regarding the channel used
                   and maximum rate that RLP can use to build the frames. 
05/02/02   snn     Added a function dsrlp_eval_qos_blob and included snm.h. 
03/15/02   snn     Added function dsrlp_any_sig_to_send for SVD
03/01/02   vas     Added DSRLP_HDR_AN_AUTH_SR_ID_INDEX to support multiple 
                   simultaneous RLPs for HDR AN authentication.
02/25/02   atp     Moved definition for QOS default from dsrlp.c to dsrlp.h.
01/31/02   atp     Added support for QOS. 
01/28/02   sy      Changed the prototypes appropriate to the dsrlp.c file.
12/03/01   snn     Added extern definition for RLP3 Negotiations.
11/27/01   snn     Added support for modifying RLP3 nak policy. Added extern
                   definitions of the functions to set RLP Opts.
09/14/01   rama    SR_ID fix. Differentiate between SR_ID and RLP Instance. 
                   Set a mapping between SR_ID and RLP Instance.
09/04/01   hcg     Added RLP throughput logging TIMETEST support.
08/13/01   snn     Added enumeration type for Mux_PDU_5.
08/02/01   vas     Changed some fields in dsrlp_stats_info_type to be uint64.
                   Added new fields for HDR RLP.
06/13/01   sy      Added DSRLP_RATE_INACTIVE enum to support P2 Mode to 
                   show which FCH/DCCH is inactive.
05/24/01   hcg     Made changes to support HDR (1xEV).
05/11/01   ak      Further support for P2 and control/hold.  Some file
                   cleanup.
04/10/01   lcc     Added support for P2 mode.
02/22/01   ak      Extern for array which holds RLP Logging IDs.
11/29/00   ak      Added DSRLP_RATE_ERASURE_SCH, which indicates an erasure
                   on the SCH. Due to LTU CRC checks in MUX.
09/05/00   ttl     Added DSRLP_RATE_BLANK_MASK and DSRLP_CAT_BLANK_MASK for
                   Enabling the data TX on SCH when the FCH is muted for data.
07/08/00   ak      Added function to return if RLP has data to send.
06/22/00   igt     Added "state" field in dsrlp_stats_info_type and also
                   added dsrlp_stats_enum to support AT$QCRL3D, AT$QCRL3R
06/07/00   ak      Removed prototype for dsrlp_setup_watermarks, since that
                   was obsoleted by dsrlp_reg_srvc.
06/05/00   ak      Extern for dsrlp_reg_srvc(), which is how dsmgr registers
                   with RLP.
06/02/00   ak      Added void to new functions to remove compiler warnings.
05/30/00   igt     added dsrlp_get_max_ms_nak_rounds_fwd() and _rev()
05/10/00   igt     added reset_timestamp to statistics structure.
05/09/00   igt     added RLP_BLOB_used, curr_tx_nak_rounds to stat structure.
04/28/00   igt     added fields to dsrlp_stats_info_type
03/21/00   igt     add dsrlp_get_stats(), dsrlp_clear_stats() and stat struct
02/15/00   igt     Added signatures for BLOB process and creation functions
07/28/99   na/hcg  Created module.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
#include "dsm.h"
#include "log.h"
#ifdef FEATURE_IS2000_REL_A
#include "snm.h"
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Until the code can handle multiple sr_id's, a constant fixed value will
  be used for the RLP instance (rscb array index).
---------------------------------------------------------------------------*/
#define DSRLP_FIXED_SR_ID_INDEX         1

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */


/*---------------------------------------------------------------------------
  Enum of the different RLP versions that this code supports.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_REV_1=0,                        /* RLP Type I                      */
  DSRLP_REV_2,                          /* RLP Type II                     */
  DSRLP_REV_3,                          /* RLP Type III                    */
  DSRLP_REV_HDR,                        /* RLP for HDR                     */
  DSRLP_NUM_REVS                        /* Max num of RLP Versions- unused */
} dsrlp_ver_enum_type;

/*---------------------------------------------------------------------------
  Enum definition for different MuxPDU types.
  MuxPDU_1 - Rateset1 for FCH/DCCH/SCCH
  MuxPDU_2 - Rateset2 for FCH/DCCH/SCCH
  MuxPDU_3 - For SCH.
  MuxPDU_5 - For SCH.
    
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_MUX_PDU_1 = 0,
  DSRLP_MUX_PDU_2,
  DSRLP_MUX_PDU_3,
  DSRLP_MUX_PDU_5,
  DSRLP_MUX_PDU_MAX
} dsrlp_mux_pdu_enum_type;

/*---------------------------------------------------------------------------
  Enum definition for the service type (primary/secondary/ dont care).
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_PRIMARY_SRVC = 0,
  DSRLP_SECONDARY_SRVC,
  DSRLP_GENERIC_SRVC
} dsrlp_srvc_enum_type;

/*---------------------------------------------------------------------------
  Enum for the different frame rates supported by RLP.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_RATE_1 =0,
  DSRLP_RATE_7_8,
  DSRLP_RATE_3_4,
  DSRLP_RATE_1_2,
  DSRLP_RATE_7_16,
  DSRLP_RATE_3_8,
  DSRLP_RATE_1_4,
  DSRLP_RATE_3_16,
  DSRLP_RATE_1_8,
  DSRLP_RATE_1_16,
  DSRLP_RATE_ERASURE,                                  /* implies FCH      */
  DSRLP_RATE_BLANK,                                    /* implies FCH      */
  DSRLP_RATE_ERASURE_SCH,                              /* on the SCH       */
  DSRLP_RATE_INACTIVE,                                 /* inactive channel */
  DSRLP_RATE_DCCH_DTX                                  /* DCCH is DTX      */
} dsrlp_rate_enum_type;

#ifdef FEATURE_IS2000_REL_A
/*---------------------------------------------------------------------------
  Enum for different RLP signalling frame priorities on Reverse link. This
  is used by MUX to decide what rate it should give to RLP.
  
  Return priority to MUX in function dsrlp_any_sig_to_send().
  DSRLP_TX_NONE         = DSRLP_DATA_OR_IDLE_OR_FILL, 
                          No RLP signalling message to send
  DSRLP_TX_ACCEPT_DELAY = NOT USED FOR NOW; May be used for IDLE or FILL if 
                          needed. If any time , after profiling we find that
                          Voice is not giving even 1/8th rate for RLP, only
                          then we need to use this.
                          Send sig when channel bandwidth allows
  DSRLP_TX_NO_DELAY     = DSRLP_NAK,
                          High priority sig message, send ASAP   
  DSRLP_TX_CONTINUOUS   = DSRLP_SYNC_OR_ACK_OR_SYNC_ACK 
                          These can be delayed for the first time, but once
                                                  we send them, we need to continously send them. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_TX_NONE =0,             /* We have Data or Rexmit or Idle or Fill  */
  DSRLP_TX_ACCEPT_DELAY,        /* Not used for now                        */
  DSRLP_TX_NO_DELAY,            /* NAKs to be sent. Send NOW w/o delay     */
  DSRLP_TX_CONTINUOUS           /* used for SYNC/ACK/SYNC-ACK              */
} dsrlp_tx_sig_enum_type;
#endif /* FEATURE_IS2000_REL_A */

/*---------------------------------------------------------------------------
  Structure of RLP frames received on the forward link - each frame is put
  into this structure by the Mux and this is passed onto RLP.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean valid_frame;                  /* TRUE-good frame; FALSE- erasure */
  dsrlp_srvc_enum_type srvc_type;       /* Primary/Sec./NA                 */
  dsrlp_mux_pdu_enum_type mux_pdu_type; /* MuxPDU type                     */
  dsrlp_rate_enum_type    fr_rate;      /* Received frame rate             */
//  uint16 frame_len;                     /* Frame len( #bytes in frame_item)*/
  dsm_item_type *fr_item_ptr;           /* Ptr to frame payload            */
} dsrlp_fwd_frame_type;

/*---------------------------------------------------------------------------
  Type definition for all the frames passed to RLP by the mux layer.
---------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_P2
#define RLP_MAX_FWD_PDU   10 /* Max # of RLP frames passed by Mux to RLP    */
#else
#define RLP_MAX_FWD_PDU   9 /* Max # of RLP frames passed by Mux to RLP    */
#endif // FEATURE_IS2000_P2

typedef struct
{
  byte sr_id;           /* Service ID - identifies service                 */
  byte num_rlp_frames;  /* # of RLP frames in frame_array; 0 means erasure */
  uint64 fr_time;       /* MUX: system time - used for logging             */
  dsrlp_fwd_frame_type fwd_arr[ RLP_MAX_FWD_PDU];
                                              /* array of rx'ed RLP frames */
} dsrlp_fwd_frame_block_type;

/*---------------------------------------------------------------------------
  Enum for the return value dsrlp_rx_process_frames.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_RX_SYNC_TIMEOUT,                /* RLP timed out trying to SYNC    */
  DSRLP_RX_IDLE,                        /* Idle frame(s) rx'ed             */
  DSRLP_RX_NON_IDLE,                    /* Ctl/Data/Retx frame rx'ed       */
  DSRLP_RX_ERROR                        /* Errors in rx'ed frame(s)        */
} dsrlp_rx_retval_enum_type;

/*---------------------------------------------------------------------------
  Masks for different RLP rates. These masks are used in the fr_rate_mask
  field in dsrlp_rev_frame_struct. These masks are ored together by the Mux
  layer to tell RLP, which rates are available.
---------------------------------------------------------------------------*/
#define DSRLP_RATE_1_16_MASK  0x0001
#define DSRLP_RATE_1_8_MASK   0x0002
#define DSRLP_RATE_3_16_MASK  0x0004
#define DSRLP_RATE_1_4_MASK   0x0008
#define DSRLP_RATE_3_8_MASK   0x0010
#define DSRLP_RATE_7_16_MASK  0x0020
#define DSRLP_RATE_1_2_MASK   0x0040
#define DSRLP_RATE_3_4_MASK   0x0080
#define DSRLP_RATE_7_8_MASK   0x0100
#define DSRLP_RATE_1_MASK     0x0200
#define DSRLP_RATE_BLANK_MASK 0x0400

/*---------------------------------------------------------------------------
  Masks for the different frame categories that RLP can use to make a frame.
  The MUX layer uses these masks depending on the physical channel in use.
---------------------------------------------------------------------------*/
#define DSRLP_CAT_DATA_MASK   0x01      /* RLP can make new/rexmit data fr */
#define DSRLP_CAT_CTL_MASK    0x02      /* RLP can make init/NAK ctl frames*/
#define DSRLP_CAT_IDLE_MASK   0x04      /* RLP can make idle/fill frames   */
#define DSRLP_CAT_BLANK_MASK  0x08      /* RLP can make BLANK frames       */

/*---------------------------------------------------------------------------
  Structure of RLP frames to be transmitted on the reverse link. The fields
  with "MUX:" are set by the Mux layer, and the fields with "RLP:" are set
  by RLP.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean data_on_ch;                   /* MUX: Is data present on the ch ?*/ 
  uint16 max_frame_rate;                /* MUX: Max frame rate for RLP     */
  uint16 max_frame_size;                /* MUX: Max. # of bytes in frame   */
  uint16 fr_rate_mask;                  /* MUX: Avail. rates for this frame*/
  dsrlp_srvc_enum_type srvc_type;       /* MUX: Primary/Sec./NA            */
  dsrlp_mux_pdu_enum_type mux_pdu_type; /* MUX: MuxPDU type                */
  byte fr_cat_mask;                     /* MUX: Can send New/Ctl/Idle/Init */
  boolean idle_frame;                   /* RLP: idle fr- can DTX if TRUE   */
  dsrlp_rate_enum_type fr_rate;         /* RLP: Current frame rate         */
  boolean free_dsm_pkt;                 /* RLP: Free the dsm pkt after tx  */
  dsm_item_type *fr_item_ptr;           /* RLP: Ptr to frame payload       */
} dsrlp_rev_frame_type;

/*---------------------------------------------------------------------------
  Type definition for all the frames the MUX layer gets from RLP. This is
  passed once every frame time to RLP.
---------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_P2
#define RLP_MAX_REV_PDU   10 /* Max # of RLP frames passed by Mux to RLP    */
#else
#define RLP_MAX_REV_PDU   9 /* Max # of RLP frames passed by Mux to RLP    */
#endif // FEATURE_IS2000_P2

typedef struct
{
  byte sr_id;               /* MUX: Service Identifier                     */
  byte max_frames;          /* MUX: Max. # of RLP frames allowed in fr_arr */
  byte num_frames;          /* RLP: # of RLP frames in frame_array         */
  uint64 fr_time;           /* MUX: system time - used for logging         */
  dsrlp_rev_frame_type rev_arr[ RLP_MAX_REV_PDU];
                            /* RLP: Actual frame payload                   */
} dsrlp_rev_frame_block_type;

#ifdef FEATURE_DS_RLP3
/*---------------------------------------------------------------------------
  Array that holds the log id's for rlp logging.
---------------------------------------------------------------------------*/
extern const
           log_rlp_phy_channel_enum_type dsrlp_log_id_table[RLP_MAX_REV_PDU];
#endif
/*---------------------------------------------------------------------------
  Array that holds the len in bytes for every rate on FCH/DCCH.
---------------------------------------------------------------------------*/
extern const byte dsrlp_len_table[DSRLP_RATE_ERASURE][DSRLP_GENERIC_SRVC]
                          [DSRLP_MUX_PDU_3];

/*---------------------------------------------------------------------------
  Array that holds the len in bytes for every rate on FCH/DCCH.  This table
  however, is used in setting the "used" field in dsm items.  Needed because
  some rlp frames are not evenly divisible by 8.
---------------------------------------------------------------------------*/
extern const byte dsrlp_used_len_table[DSRLP_RATE_ERASURE]
                                      [DSRLP_GENERIC_SRVC]
                                      [DSRLP_MUX_PDU_3];



#define DSRLP_MAX_NAK_ROUNDS    3
#define DSRLP_MAX_NAKS_PER_ROUND 3 /* Maximum Naks that can be sent/round  */

#define DSRLP_BLOB_NOT_USED     0 /* RLP BLOB was not received             */
#define DSRLP_BLOB_NAK_SCHEME   1 /* RLP BLOB specified NAK scheme         */
#define DSRLP_BLOB_RTT          2 /* RLP BLOB specified RTT                */
#define DSRLP_BLOB_RTT_PLUS_NAK 3 /* RLP BLOB specified RTT & NAK scheme   */

/*---------------------------------------------------------------------------
  RLP statistics information definition. This structure contains all of the
  RLP statistical information for the currently active RLP session.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 at_reset_request_count;   /* # of resets requested by AT           */
  uint32 an_reset_ack_count;       /* # of Acks received from AN            */
  uint32 an_reset_request_count;   /* # times reset was requested by AN     */
  uint32 at_nak_bytes_requested;   /* Total # of bytes requested by AT NAKs */
  uint64 an_nak_bytes_requested;   /* Total # of bytes requested by AN NAKs */
  
  uint64 rx_dup_fr_cnt;            /* # of received duplicate frames        */
  uint64 rx_rexmit_fr_cnt;         /* # of received re-transmitted frames   */
  uint64 rx_new_data_fr_cnt;       /* total # of received new data frames   */ 

  uint64 rx_total_bytes;           /* total # of bytes received             */
  uint64 tx_total_bytes;           /* total # of bytes transmitted          */

  uint64 tx_rexmit_fr_cnt;         /* # of transmitted re-transmitted frames*/
  uint64 tx_new_data_fr_cnt;       /* total # of new transmitted data frames*/ 

  uint32 rx_idle_fr_cnt;           /* # of received idle frames             */
  uint32 rx_fill_fr_cnt;           /* # of received fill frames             */
  uint32 rx_blank_fr_cnt;          /* # of blank frames received            */
  uint32 rx_null_fr_cnt;           /* # of null frames received             */
  uint32 rx_20ms_fr_cnt;           /* # of received Fundamental data frames */
  
  uint32 tx_idle_fr_cnt;           /* # of transmitted idle frames          */
  uint32 tx_20ms_fr_cnt;           /* # of txed Fundamental data frames     */

  uint32 naks_received ;           /* total # of nak frames received        */
  
  uint32 re_xmits_not_found;       /* # of Rexmits not found                */

  uint32 rx_rlp_erasures;          /* # of rlp erasures received            */
  uint32 rx_mux_erasures;          /* # of mux erasures received            */
  uint16 largest_contig_erasure;   /* largest block of consecutive erasures */

  uint8  resets;                   /* # of resets                           */
  uint8  aborts;                   /* # of aborts    (nak aborts)           */

  uint32 last_rtt;                 /* round trip time                       */
  
  uint64 reset_timestamp;          /* Timestamp when these stats were reset */
  
  uint8  RLP_BLOB_used;            /* keep track of how RLP_BLOB is used    */
  uint8  curr_tx_nak_rounds;       /* # tx nak rounds used at this time     */
  
  uint32 tx_naks[DSRLP_MAX_NAK_ROUNDS];
                                   /* single, double etc NAKs sent array    */
  
  uint16 state;                    /* current state of the rlp              */

  uint32 rx_contig_idle_fr_cnt;    /* # of contiguous idle frames received  */
  uint32 tx_contig_idle_fr_cnt;    /* # of contig idle frames transmitted   */

} dsrlp_stats_info_type;


#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_IS2000_REL_A

/*---------------------------------------------------------------------------
   Default QOS parameter priority in case of non-assured mode, IS-707 A2
---------------------------------------------------------------------------*/
#define  DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ 0x0D
#define  DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED   0x0
#define  DSRLP_QOS_DEFAULT_BLOB_TYPE         0x0

/*---------------------------------------------------------------------------
  Parameters for non-assured mode QOS
---------------------------------------------------------------------------*/
typedef struct
{
  boolean pri_adj_incl; /* If non-assured priority adjustment field present*/
  byte pri_adj;                                     /* priority adjustment */
} dsrlp_qos_non_assured_params_s_type;

/*---------------------------------------------------------------------------
  Parameters for assured mode QOS
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
     FL
  -------------------------------------------------------------------------*/

  /* FL qos included indicator */
  boolean FL_qos_incl;

  /* FL priority */
  boolean FL_pri_adj_incl;
  byte FL_pri_adj;

  /* FL min user data rates */
  boolean FL_requested_data_rate_incl;
  boolean FL_acceptable_data_rate_incl;
  byte FL_requested_data_rate;
  byte FL_acceptable_data_rate;

  /* FL data loss rates */
  boolean FL_requested_loss_rate_incl;
  boolean FL_acceptable_loss_rate_incl;
  byte FL_requested_loss_rate;
  byte FL_acceptable_loss_rate;

  /* FL max delays */
  boolean FL_requested_delay_incl;
  boolean FL_acceptable_delay_incl;
  byte FL_requested_delay;
  byte FL_acceptable_delay;

  /*-------------------------------------------------------------------------
     RL
  -------------------------------------------------------------------------*/

  /* RL qos included indicator */
  boolean RL_qos_incl;

  /* RL priority */
  boolean RL_pri_adj_incl;
  byte RL_pri_adj;

  /* RL min user data rates */
  boolean RL_requested_data_rate_incl;
  boolean RL_acceptable_data_rate_incl;
  byte RL_requested_data_rate;
  byte RL_acceptable_data_rate;

  /* RL data loss rates */
  boolean RL_requested_loss_rate_incl;
  boolean RL_acceptable_loss_rate_incl;
  byte RL_requested_loss_rate;
  byte RL_acceptable_loss_rate;

  /* RL max delays */
  boolean RL_requested_delay_incl;
  boolean RL_acceptable_delay_incl;
  byte RL_requested_delay;
  byte RL_acceptable_delay;

} dsrlp_qos_assured_params_s_type;


/*---------------------------------------------------------------------------
  QOS parameters, assured or non-assured depends on mode
---------------------------------------------------------------------------*/
typedef union
{
  dsrlp_qos_non_assured_params_s_type non_assur_params;
  dsrlp_qos_assured_params_s_type assur_params;

} dsrlp_qos_params_u_type;

/*---------------------------------------------------------------------------
  Representation of QOS parameters as a structure
---------------------------------------------------------------------------*/
typedef struct
{
  boolean qos_incl; /* QOS included or not indicator. Other fields valid   */
                    /* only if this indicator is TRUE.                     */
  boolean max_blob_type_supp_incl; /* If max blob type supported field     */
  boolean blob_type_incl;          /* If blob type field present or not    */
  byte    max_blob_type_supported; /* Max blob type supported              */
  byte    blob_type;               /* Blob type                            */
  boolean assur_mode; /* true - assured mode, false - nonassured mode      */
  dsrlp_qos_params_u_type params; /* Actual parameters depend on mode      */

} dsrlp_qos_s_type;


/*---------------------------------------------------------------------------
  Maximum length of the QOS blob sent by mobile, including reserved bits.
  Values obtained from IS-707 Section 2.2.9.2
---------------------------------------------------------------------------*/
#define DSRLP_MOBILE_QOS_BLOB_LEN_MAX  11                 /*     87 bits   */
#define DSRLP_BS_QOS_BLOB_LEN_MAX       6                 /*     41 bits   */

/*---------------------------------------------------------------------------
  One place to hold all the different QOS values - default, desired,
  received from BS.
---------------------------------------------------------------------------*/
typedef struct
{
  dsrlp_qos_s_type defaults;      /* Default QOS values from IS-707 2.2.9  */
  dsrlp_qos_s_type desired;       /* QOS values desired by application     */
  dsrlp_qos_s_type rx_from_bs;    /* QOS values received from base-station */
  dsrlp_qos_s_type * curr_p;      /* Current values. Ptr to one of above   */

} dsrlp_qos_values_type;

extern dsrlp_qos_values_type dsrlp_qos_values;

#endif /* FEATURE_IS2000_REL_A */



/*===========================================================================


                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION       DSRLP_STARTUP

DESCRIPTION    This function is to be called only once per power-up
               of the phone.  This function initializes any structs
               which may be global across all RLP versions or instances.

               DSRLP_INIT is called per RLP instance, and so can be
               called many times, but this function must be called
               only once, and before any other RLP functions are called.


DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlp_startup(void);

/*===========================================================================

FUNCTION DSRLP_INIT

DESCRIPTION
  This function initializes the RLP session corresponding to the passed
  sr_id for the given version of RLP. All data structures are setup and
  RLP is reset to start new operation. This function should be called at the
  beginning of each call for each service.

  Input Parameters:
    rscb_index: Index in the rscb array where this record will be stored 
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.
    ver  : The RLP version - 1,2,3...
    octet_sequenced: This RLP will run in byte sequenced mode. This means
           that all new frames and retransmissions will have a single
           byte sequence space.
           Mode of operation cannot be changed in the middle of a RLP
           session.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_init
(
  byte rscb_index,                           /* index in the rscb array where
                                                this record will be stored */
  byte sr_id,                                /* Service ID                 */
  dsrlp_ver_enum_type ver,                   /* RLP version                */
  boolean octet_sequenced                    /* Byte sequenced operation?  */
);
/*===========================================================================

FUNCTION DSRLP_CLEANUP

DESCRIPTION
  This function cleansup the memory and resets the RLP state for a given
  sr_id. This should be called at the end of a RLP session.

  Input Parameters:
    sr_id: The Service ID for the particular service. Primary is usually 1
           and secondary is usually 2.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_cleanup
(
  byte sr_id,                                                /* Service ID */
  /* Flag to inform if RLP cleanup is for a call going NULL */                 
  boolean cleanup_for_going_null 
);
/*===========================================================================

FUNCTION DSRLP_RESET

DESCRIPTION
  Invokes the reset procedures for RLP. Simply calls the function
  dsrlpi_reset

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_reset
(
  byte sr_id                                 /* Service ID                 */
);
/*===========================================================================

FUNCTION DSRLP_PROCESS_RX_FRAMES

DESCRIPTION
  This function processes the RLP frame(s) received over the air. This is the
  interface between RLP receive and the multiplex sublayer. The multiplex
  sublayer can send all the frames it received in one frame time to RLP or
  split it up into multiple calls of this function.

  Input Parameters:

  The parameter to this function is a pointer to a structure which contains
  an array of frames that the mux sublayer wants RLP to process. Each element
  of this array includes all the information that RLP needs to process the
  frame independently. The information passed with each frame includes a
  pointer to the dsm item containing the frame, the muxPDU type, service type
  (primary or secondary).

  The fr_item_ptr can be a pointer to dsm packet chain (with multiple
  dsm items).

  This per frame data structure is passed to a number of functions within
  the RLP code.

DEPENDENCIES
  dsrlp_init() should have been previously invoked. rlp_establish_srvc_type()
  should have been previously invoked to set up the traffic type, rate set
  and RLP type in use.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_process_rx_frames
(
  dsrlp_fwd_frame_block_type *fwd_frame_block/* Ptr to rcvd RLP frames     */
);
/*===========================================================================

FUNCTION DSRLP_BUILD_TX_FRAMES

DESCRIPTION
  This function builds RLP frame(s) to be transmitted over the air.
  This is the interface between RLP transmit and the multiplex sublayer.
  The multiplex sublayer can either call this function once, to build all
  the frames or call this several times to build a few frames at a time.

  Input Parameters:

  The parameter to this function is a pointer to a structure which contains
  an array of frames that the mux sublayer wants RLP to process. Each element
  of this array includes all the information that RLP needs to process the
  frame independently.   This per frame data structure is passed to a number
  of functions within the RLP frame building code.The information passed
  with each frame includes fields which are set by the mux layer, and fields
  which are set by RLP, when RLP builds the frame.

  Fields set by Mux layer for entire se of frames asked.

    sr_id         : service id for this frame. For SCH frames, sr_id is in
                    the MuxPDU header. For FCH/DCCH frames, this is set by
                    the Mux layer after looking up the appropriate sr_id for
                    primary or secondary. MUX layer must ask for frames of
                    one sr_id in one invocation of this function.

  The fields set by the mux layer per frame:

    srvc_type     : For FCH/DCCH frames, the mux layer sets the srvc_type
                    to DSRLP_PRIMARY_SRVC or DSRLP_SECONDARY. For SCH/SCCH,
                    Mux layer sets the srvc_type to DSRLP_GENERIC_SRVC.

    mux_pdu_type  : Set to DSRLP_MUX_PDU_1 for rateset1 FCH/DCCH; set to
                    DSRLP_MUX_PDU_2 for rateset2 FCH/DCCH; set to
                    DSRLP_MUX_PDU_3 for SCH operation.

    fr_cat_mask   : For FCH/DCCH, Mux layer sets this mask to
                    ( DSRLP_CAT_DATA_MASK | DSRLP_CAT_CTL_MASK |
                      DSRLP_CAT_IDLE_MASK), because data, ctl/idle frames
                    can be sent on FCH/DCCH.
                    For SCH/SCCH, Mux layer sets this mask to
                    DSRLP_CAT_DATA_MASK, because only data frames can be
                    sent on SCH/SCCH.

    max_frame_size: This specifies the max. number of bytes avail. for RLP to
                    put in the frame. For MuxPDU type1 and 2, when multiple
                    frame rates are available, the Mux layer should set
                    this field to the size of the highest frame rate.
                    The Mux layer sets this field to the number of bytes
                    available for RLP to put itsheader and payload excluding
                    the type bits.
                    Basically, take the number of bits of RLP payload as
                    specified in the standard, subtract 8 from it, divide it
                    by 8 and round up to the next byte.
                    e.g for secondary rate1 frames on FCH rateset1,
                    max_frame_size = (168-8)/8 = 20 bytes.

                    For MuxPDU3, this will be set to MuxPDU size-1. e.g. for
                    SCH double size LTUs in Service Option 33, this will be
                    43 bytes.

    fr_rate_mask  : The mux layer ors in all the different frame rates
                    allowed for this frame in this mask. For FCH, the mask
                    can take on a whole range of values depending on primary
                    or secondary service. For DCCH/SCH/SCCH, the mask is
                    usually DSRLP_RATE_1_MASK.


  The fields set by the RLP:

    idle_frame    : RLP sets this to TRUE, if this frame can be DTXed by
                    the Mux layer (for DCCH/secondary operation). When this
                    is FALSE, the Mux layer has to send out this frame.

    fr_rate       : RLP sets this to the frame rate this frame should be
                    used by the Mux layer to send out this frame.

    fr_item_ptr   : Pointer to dsm packet chain (may have multiple dsm items)
                    RLP should ensure that the used fields are properly set
                    in the item, to the number of bits used by RLP rounded
                    up to the next byte.
                    The mux layer shall transmit the number of bytes in the
                    item chain. If there are any left over bits, the Mux
                    layer shall pad the bits with zeros.
                    RLP shall set the unused part of the item chain to zeros

DEPENDENCIES
  dsrlp_init() should have been called.

  Mux layer must ask for frames for only one sr_id in one invocation of this
  funciton.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_build_tx_frames
(
  dsrlp_rev_frame_block_type *rev_fr_blk     /* Ptr to frames to trasmit   */
);

/*===========================================================================

FUNCTION DSRLP_BUILD_RLP_BLOB

DESCRIPTION

  This function builds an RLP BLOB (Block of Bytes)

  Input Parameters:

  *blob_ptr:   is a pointer pointing to free space for new blob be built.


DEPENDENCIES
  None

RETURN VALUE
  *blob_ptr: is a pointer pointing to the blob that was build.

   blob_len: is returned by the function and contains the length of the new
             blob.

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_build_rlp_blob
(
  byte  *blob_ptr
);

/*===========================================================================

FUNCTION DSRLP_PROCESS_RLP_BLOB

DESCRIPTION
  This function processes the received RLP BLOB (Block of Bytes)

  Input Parameters:

  sr_id: used to find the appropriate session control block pointer

  *blob_ptr: points to the BLOB to be processed

  blob_len: specifies the length of the BLOB to be processed

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_process_rlp_blob
(
  byte   sr_id,
  byte  *blob_ptr,
  byte   blob_len
);

/*===========================================================================

FUNCTION DSRLP_GET_STATS

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  rscb_index:    used to find the appropriate session control block pointer

  stats_ptr_ptr: is a pointer to a pointer to the stats structure

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  stats_ptr_ptr:  is the pointer to the pointer to the stats structure
  return TRUE if the *stats_ptr_ptr points to a valid memory location,
  otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_stats
(
  byte                      rscb_index,
  dsrlp_stats_info_type   **stats_ptr_ptr
);


/*===========================================================================

FUNCTION DSRLP_CLEAR_STATS

DESCRIPTION
  This function takes an sr_id and uses that to get the appropriate session
  control block pointer and from there it will initialize the related
  structure with the statistics.

  Input Parameters:

  rscb_index:  used to find the appropriate session control block pointer


DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_clear_stats
(
    byte rscb_index
);


#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================

FUNCTION DSRLP_GET_MAX_MS_NAK_ROUNDS_FWD

DESCRIPTION
  This function the MAX number of nak rounds the mobile will support
  on the forward channel

  Input Parameters: None

DEPENDENCIES
  None

RETURN VALUE
  The max number of nak rounds

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_max_ms_nak_rounds_fwd(void);


/*===========================================================================

FUNCTION DSRLP_GET_MAX_MS_NAK_ROUNDS_REV

DESCRIPTION
  This function the MAX number of nak rounds the mobile will support
  on the reverse channel

  Input Parameters: None

DEPENDENCIES
  None

RETURN VALUE
  The max number of nak rounds

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_max_ms_nak_rounds_rev(void);


/*===========================================================================
FUNCTION      DSRLP_REG_SRVC

DESCRIPTION   Registers the watermarks and queues/callbacks for an RLP
              session.

DEPENDENCIES  A valid sr_id value must be passed into this function.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_reg_srvc
(
  byte                rscb_index,            /* index for session id       */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit frs */
  void                (*post_rx_func_ptr)(), /* rx callback for rx frames  */
  q_type              *post_rx_q_ptr         /* queue for rx'ed frames     */
);

/*===========================================================================
FUNCTION      DSRLP_REG_PZID_HYST_NOTIFY_FUNC

DESCRIPTION   This registers the function to notify PZID hysterisis engine
              if a new RLP packet is sent/received after going dormant.

              If a NULL function is passed, the "notify PZID" function is
              deregistered.

DEPENDENCIES  A valid index (corresponding to an sr_id) must be passed 
              into this function. 

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_reg_ppp_data_transfer_notify_func
(
  byte                rscb_index,            /* index for a session id     */
  void (*ppp_data_transfer_notify_func) (byte rlp_instance)
);

/*===========================================================================
FUNCTION      DSRLP_BYTES_DATA_TO_SEND

DESCRIPTION   Gives some indication of the # new bytes of data to send by
              RLP 3.

DEPENDENCIES  The passed in SR_ID is good.

RETURN VALUE  TRUE - new data or rexmits to be sent.  FALSE - no data to be
              rexmitted.

SIDE EFFECTS  None
===========================================================================*/
word dsrlp_bytes_data_to_send
(
  byte  rscb_index                       /* passed in SR_ID                */
);

/*===========================================================================
FUNCTION      DSRLP_HAS_DATA_TO_SEND

DESCRIPTION   Does RLP3 have stuff to send? (either naks, rexmits, new data,
              etc).

DEPENDENCIES  The passed in SR_ID is good.

RETURN VALUE  TRUE - RLP needs to send some data.

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_has_data_to_send
(
   byte  rscb_index                      /* passed in SR_ID                */
);
#ifdef TIMETEST_DSRLP_STATS 
/*===========================================================================

FUNCTION DSRLP_LOG_TIMETEST_THROUGHPUT

DESCRIPTION
  This function is a clock callback function to control logging of the
  total Rx and Tx RLP bytes to the TIMETEST port.  It is controlled by the
  dsrlp_timetest_cb clk_cb_type and is registered in dsrlp_init() to go off
  every 500 ms.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dsrlp_log_timetest_throughput
( 
  int4 ms 
);
#endif /* TIMETEST_DSRLP_STATS */

/*===========================================================================
FUNCTION      DSRLP_CHECK_SR_ID

DESCRIPTION   Check if the SR_ID for the service running on a particular RLP
              instance (rscb array index) is the same as in the MUX header in
			  the frame

DEPENDENCIES  The passed in RSCB_INDEX is good.

RETURN VALUE  TRUE if the SR_ID matches, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_check_sr_id
(
   byte rscb_index,                      /* RLP instance                   */
   dsm_item_type *frame_ptr              /* MUX frame                      */
);

/*===========================================================================
FUNCTION      DSRLP_GET_SR_ID

DESCRIPTION   Return the SR_ID for the service running on a particular RLP
              instance (rscb array index)

DEPENDENCIES  The passed in RSCB_INDEX is good. Also dsrlp_init must have 
              been invoked first for the returned SR_ID to be good.

RETURN VALUE  The SR_ID corresponding to the RLP instance

SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_get_sr_id
(
   byte rscb_index                       /* RLP instance                   */
);

/*===========================================================================

FUNCTION     DSRLP_GET_ALL_DEF_NAKS
DESCRIPTION  Returns the number of default rounds and the naks per round to be
             used when negotiating an RLP3 service option
DEPENDENCIES There must be sufficient space in the naks_per_round arrays
             to hold the returned data
RETURN VALUE Boolean: 
             True: the operation was success.
             False: The operation was not done. None of the values in the
                    passed pointers may be good.
                    Values are returned by argument pointers
SIDE EFFECTS None
===========================================================================*/
extern boolean dsrlp_get_all_def_naks
(
   byte *nak_rounds_fwdp,                /* Num of rounds for forward NAKs */
   byte *naks_per_round_fwdp,            /* Num of NAKs per round forward  */
   byte *nak_rounds_revp,                /* Num of rounds for reverse NAKs */
   byte *naks_per_round_revp             /* Num of NAKs per round reverse  */
);  
/*===========================================================================
  FUNCTION      DSRLP_SET_ALL_DEF_NAKS
 
  DESCRIPTION   Set the number of rounds and the naks per round to be
                used when negotiating an RLP3 service option
 
  DEPENDENCIES  Can't lengthen the number of rounds beyond max_rounds. 
                Its assumed that the calling routine confirmed that this 
                change is done only for RLP3 call.
  
  RETURN VALUE  TRUE: If we were able to set the values.
                FALSE: None of the values are changed, since some parameter
                is having a wrong value.
  
  SIDE EFFECTS  Changes don't take effect until next SO negotiation
===========================================================================*/
  extern boolean dsrlp_set_all_def_naks
  (
     byte *nak_rounds_fwdp,              /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,          /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,              /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp           /* Num of NAKs per round reverse  */
  );   
/*===========================================================================
  FUNCTION      DSRLP_GET_ALL_CUR_NAKS
 
  DESCRIPTION   Return the number of rounds and the naks per round currently
                in use (or last used) in an RLP3 service option call
  
  DEPENDENCIES  There must be sufficient space in the naks_per_round arrays
                to hold the returned data
                The passed in RSCB_INDEX is good. Also dsrlp_init must have 
                been invoked first for the returned SR_ID to be good.
  
  RETURN VALUE  Values are returned by argument pointers
  
  SIDE EFFECTS  None
  ===========================================================================*/
  extern boolean dsrlp_get_all_cur_naks
  (
     byte rscb_index,                      /* RLP instance                   */
     byte *nak_rounds_fwdp,                /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,            /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,                /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp             /* Num of NAKs per round reverse  */
  );

/*===========================================================================
  FUNCTION      DSRLP_SET_CUR_NAKS
 
  DESCRIPTION   Set the number of rounds and the naks per round to be
                used in the current call, overiding the defaults
  
  DEPENDENCIES  Each NAK round must be less than or equal to what was
                specified as the default when the call was originated.
                The passed in RSCB_INDEX is good. Also dsrlp_init must have 
                been invoked first for the returned SR_ID to be good.
  
  RETURN VALUE  None
  
  SIDE EFFECTS  None
 ===========================================================================*/
  extern boolean dsrlp_set_all_cur_naks
  (
     byte rscb_index,                     /* RLP instance                   */
     byte *nak_rounds_fwdp,               /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,           /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,               /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp            /* Num of NAKs per round reverse  */
  );

/*===========================================================================
  FUNCTION      DSRLP_GET_ALL_NEG_NAKS
 
  DESCRIPTION   This function will return the negotiated parameters. Note 
                that this function has to be called only for getting 
                the parameters. (we donot have a corresponding set_all_
                neg_naks function).
  
  DEPENDENCIES  The values returned here are valid only within a call.
  
  RETURN VALUE  Values are returned by argument pointers
                TRUE:  If we were able to get the values.
                FALSE: Some argument is wrong. So could not return values.
  
  SIDE EFFECTS  None
===========================================================================*/
  extern boolean dsrlp_get_all_neg_naks
  ( 
     byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
     byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
     byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
     byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
  );


#ifdef FEATURE_IS2000_REL_A

/*===========================================================================
  FUNCTION      DSRLP_BUILD_QOS_BLOB

  DESCRIPTION   This function creates a qos blob to be sent by mobile to BS

  DEPENDENCIES  Parameter passed in, ie array of bytes to hold blob should
                have enough space to hold blob.

  RETURN VALUE  Length of blob (0 if no blob needs to be sent, ie default)
                Blob is returned by argument pointer

  SIDE EFFECTS  None
===========================================================================*/
  extern byte dsrlp_build_qos_blob
  (
     byte *qos_blob_p                                    /* Resulting blob */
  );


/*===========================================================================
  FUNCTION      DSRLP_PROCESS_QOS_BLOB

  DESCRIPTION   This function processes the qos blob received from BS.

  DEPENDENCIES  None

  RETURN VALUE
              TRUE:  If we were able to get the values.
              FALSE: If blob didn't have qos params

  SIDE EFFECTS  The rx_from_bs Qos value struct is filled up
===========================================================================*/
  extern boolean dsrlp_process_qos_blob
  (
     const byte *qos_blob_p,                             /* Ptr to blob rx */
     byte blob_len
  );

/*===========================================================================
  FUNCTION      DSRLP_CALC_CURR_QOS_VALUES

  DESCRIPTION   This function calculates and stores current Qos values
                based on desired values requested and the values received
                from BS.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_calc_curr_qos_values(void);

/*===========================================================================
  FUNCTION      DSRLP_GET_CURR_QOS_VALUES

  DESCRIPTION   This function returns current QOS values

  DEPENDENCIES  None

  RETURN VALUE  Current values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_get_curr_qos_values
  (
     dsrlp_qos_s_type *dsrlp_qos_s_p           /* Curr Qos values returned */
  );

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_VALUES

  DESCRIPTION   This function returns desired QOS values

  DEPENDENCIES  None

  RETURN VALUE  Desired values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_get_desired_qos_values
  (
     dsrlp_qos_s_type *dsrlp_qos_s_p        /* Desired Qos values returned */
  );

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_VALUES

  DESCRIPTION   This function sets desired Qos values

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_set_desired_qos_values
  (
     dsrlp_qos_s_type *dsrlp_qos_s_p                 /* Desired Qos values */
  );

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function sets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
  extern void dsrlp_set_desired_qos_non_assur_pri
  (
     byte pri_adj_value            /* Desired value of priority adjustment */
  );

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function gets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  TRUE- If value of non-assured mode priority got successfully
                FALSE- If desired values not set, or if mode is assured
                Desired priority adjustment value returned as argument ptr.

  SIDE EFFECTS  None
===========================================================================*/
  extern boolean dsrlp_get_desired_qos_non_assur_pri
  (
     byte *pri_adj_value_p         /* Desired value of priority adjustment */
  );


/*===========================================================================
  FUNCTION      DSRLP_ARE_QOS_VALUES_SAME

  DESCRIPTION   This function takes as argument two qos structures and
                compares whether they are the same. (Eg: useful for
                checking if desired values equals default values)

  DEPENDENCIES  None

  RETURN VALUE
              TRUE:  If values are identical in both qos structures
              FALSE: If values are not identical

  SIDE EFFECTS  None
===========================================================================*/
  extern boolean dsrlp_are_qos_values_same
  (
     dsrlp_qos_s_type *dsrlp_qos_s_1_p,     /* Ptr to struct of qos params */
     dsrlp_qos_s_type *dsrlp_qos_s_2_p      /* Ptr to struct of qos params */
  );

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
  FUNCTION      DSRLP_ANY_SIG_TO_SEND

  DESCRIPTION   This function will return TRUE if any  important RLP 
                frames ( called as Signalling in name of fn) are to be 
                transmitted. 
                Important frames are:
                    a) SYNC/SYNC-ACK/AC
                    b) NAK's:
                    c) Retransmissions to be done
               
               If need be add IDLE and Fill frames also.

  DEPENDENCIES  
              This function should be called only if Data is secondary
              service. If Data is primary service this function should
              not be called.(because, entire channel is for data to 
              send)

  RETURN VALUE
              TRUE:  If RLP has control frames to send
              FALSE: If RLP has NO control frames to send

  SIDE EFFECTS  None
===========================================================================*/

extern boolean dsrlp_any_sig_to_send(dsrlp_tx_sig_enum_type *);
#endif /*FEATURE_IS2000_REL_A*/

/*===========================================================================
  FUNCTION      DSRLP_EVAL_QOS_BLOB

  DESCRIPTION   This function evaluates the qos blob rx from BS. For now,
                we donot do anything. So accept whatever the BS gives. 

  DEPENDENCIES  None

  RETURN VALUE
                SNM_ACCEPT_CFG: DS can accept anything that it receives from BS
                because, currently DS does not process it.  

  SIDE EFFECTS  
===========================================================================*/

extern snm_eval_status_type dsrlp_eval_qos_blob( byte   bs_blob_len,
                                          byte * bs_blob,
                                          boolean can_suggest_alt,
                                          byte * ret_alt_blob_len,
                                          byte * ret_alt_blob
                                         );

/*===========================================================================
  FUNCTION      DSRLP_GET_NEXT_DSM_ITEM_CHAIN

  DESCRIPTION   This function takes the pointer to the current beginning of
                a chain of DSM items, which contains RLP paypload to be
                transmitted by the MUX layer, as input, and returns pointer
                to the next item in chain. If there is no more item in
                chain, NULL is returned.


  DEPENDENCIES  None

  RETURN VALUE  Pointer to the next item in chain; if there is no more
                item in chain, return NULL.

  SIDE EFFECTS  None
===========================================================================*/
dsm_item_type *dsrlp_get_next_dsm_item_chain( 
	                                     dsm_item_type *cur_ptr_to_payload );

#endif /* FEATURE_IS2000_REL_A */
/*===========================================================================
  FUNCTION      DSRLP_IS_INITED

  DESCRIPTION   If RLP is already INIT'd, then returns TRUE.  Else returns
                FALSE.

                Takes an RLP instance as input.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_is_inited
(
  byte rscb_index                        /* RLP instance                   */
);

/*===========================================================================
  FUNCTION      DSRLP_INSERT_PDU5NULL_HDR_TURBO
  
  DESCRIPTION   

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/

extern void dsrlp_insert_pdu5null_hdr_turbo(dsm_item_type *out_pkt_ptr);

#endif  /* RLP3 || HDR */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

/*===========================================================================

FUNCTION DSRLP_GET_STATS_EX

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  sr_id:         used to find the appropriate session control block pointer

  route:         The route for which stats are needed
  
  stats_ptr_ptr: is a pointer to a pointer to the stats structure

DEPENDENCIES
  A valid sr_id value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  stats_ptr_ptr:  is the pointer to the pointer to the stats structure
  return TRUE if the *stats_ptr_ptr points to a valid memory location,
  otherwise FALSE

SIDE EFFECTS
  None
===========================================================================*/
boolean dsrlp_get_stats_ex(
  byte                      rscb_index,
  byte                      route,
  dsrlp_stats_info_type   **stats_ptr_ptr
);
#endif  /* _DSRLP_H */


