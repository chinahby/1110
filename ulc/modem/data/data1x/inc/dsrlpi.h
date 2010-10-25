#ifndef _DSRLPI_H
#define _DSRLPI_H
/*===========================================================================

                     R A D I O   L I N K   P R O T O C O L
                  I N T E R N A L    H E A D E R     F I L E

DESCRIPTION
  This header file contains the internal interface definitions used in the
  framework for the Radio Link Protocol (RLP).  The framework is designed to
  support all flavors of RLP (RLP-1, RLP-2, RLP-3, RLP-HDR). This file should
  only be included by the RLP modules and not by external files.
  
  Naming convention:
  All functions in this file are external to dsrlpi.c and start as dsrlpi_...
  All variables declared here are external to dsrlpi.c and start as dsrlpi_..

  Copyright (c) 1999-2006 by QUALCOMM, Incorporated.       All Rights Reserved.
  ===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/inc/dsrlpi.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/06   squ     Added flag ack_to_sned to the struct dsrlpi_state_info_type. 
08/30/05    gr     Fix to handle the race condition of simultanneous RLP 
                   init and cleanup while dropping voice during SVD scenario
09/08/04   vr      Added notify_ppp_data_transfer_func_ptr in the RLP cfg info
07/10/03   vas     HDRRLP assigned SR ID values that dont clash with 1x values 
06/18/03   vas     Increased MAX_SESSIONS for RLP to accomodate for 3 RLPs in 
                   MM architecture
05/21/03   vas     Increased the number of elements in RSCB array for MM data
03/18/02   vas     Added NULL state to RLP state enum type. Needed for HDRRLP
03/01/02   hcg     Increased MAX_SESSIONS to support multiple simultaneous
                   RLPs.
02/15/02   hcg     Added fail-safe reset state timer to RLP control block to
                   protect against potential reset deadlock situation for
                   1xEV.  
02/14/02   hcg     Added boolean and Rex timer to RLP control block to support
                   flush timer for 1xEV.
10/17/01   vas     Added stats logging timer to dsrlpi_sess_ctl_blk_type.
09/14/01   rama    SR_ID fix. Differentiate between SR_ID and RLP instance. 
                   Set a mapping between SR_ID and RLP Instance.
08/13/01   snn     Modified the extern for dsrlpi_type_mask_table.  
08/02/01   ak      Add a flag to indicate when rlp is being reset.
07/02/01   ak      Changed MAX_CHAINS to 3 * max_fwd_rlp frames.
06/04/01   igt     Added two pointer fields reseq_last_insert_chain_ptr and 
                   reseq_last_insert_item_ptr in the dsrlpi_state_info_type to 
                   facilitate fast inserts into resequencing q in edge cases.
05/24/01   hcg     Made changes to support HDR (1xEV).  Made size of the 
                   transmit queue part of version specific constants.
05/21/01   sy      Added a new frame type DSRLPI_RESET and a new varibale in
                   reset_rlp in dsrlpi_state_info_type.
05/11/01   ak      MAX CHAINS now set to 75 items.
04/16/01   ak      Added pointer to state struct for quick access into txq.
02/21/01   ak      Support for checking against old frames and also for
                   potentially mis-RDA'd frames which cause big NAK gaps.
01/15/01   igt     #def DSRLPI_MAX_RTT_SETUP and added sync_delay in
                   dsrlpi_state_info_type.
01/04/01   igt     Added two pointer fields reseq_last_chain_ptr and 
                   reseq_last_item_ptr in the dsrlpi_state_info_type to 
                   facilitate fast inserts into resequencing q in edge cases.
12/06/00   ak      Changes to force FILL frames every 80 frames so that
                   other side gets our V(N) appropriately.
11/15/00   igt     remove txq_id field from dsrlpi_state_info_type struct 
06/05/00   ak      New category DSRLPI_FILL, so that RLP 3 can know if data
                   was received in last 20 msec (for dormancy detection).
05/15/00   na/ak   Support for delayed frame detection.
05/10/00   ak      Added fields to support sendig fill frames and better
                   mux erasure counts.  Also added tx_fill to fcn ptr table.
04/28/00   igt     moved and renamed DSRLPI_MAX_NAK_ROUNDS
04/19/00   ak      Added new frame type enums, and added rlp_rx_mux_erasure
                   in the function ptr table.
04/13/00   ak      Made frame type enum explicit on enum values, to help in
                   debugging.  Also added externs for bytes used to record
                   first 3 bytes of each frame.
04/12/00   na      Changed Max. NAK rounds to 3.
03/21/00   igt     moved and renamed dsrlpi_stats_info_type
02/15/00   igt     Extended the dsrlpi_ver_const_type to support BLOBs
01/14/00   ak      Removed typebit_pos array extern.
01/07/00   ak      made get_best_fr_rate public.
01/06/00   na      Increased dsrlpi_type_mask_table to accomodate genric
                   service type.
07/28/99   na/hcg  Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#ifdef FEATURE_DS
#include "dsrlp.h"
#include "dsm.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  This is the modulus used when representing the sequence numbers as 
  32-bit numbers.
---------------------------------------------------------------------------*/
#define DSRLPI_32BIT_SEQ_MODULUS     0x10000000   /* 2 to the 28th power   */

/*---------------------------------------------------------------------------
  Enum of the different frame types. Each type has a separate processing 
  module. The DONE_FRAME signifies completion of frame processing.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLPI_DONE    = 0, /* Finished processing this received frame           */
  DSRLPI_NEW     = 1, /* frame with new sequence number                    */
  DSRLPI_REXMIT   = 2,    /* re-xmit frame -                               */
  DSRLPI_NAK      = 3,    /* NAK control frame -                           */
  DSRLPI_INIT    = 4, /* RLP Init - SYNC or SYNC/ACK control frame         */
  DSRLPI_IDLE    = 5, /* Idle frame - No data was sent                     */
  DSRLPI_MUX_ERASURE = 6, /* Mux level erasure                             */
  DSRLPI_RLP_ERASURE = 7, /* RLP level erasure                             */
  DSRLPI_NULL    = 8, /* 1/8 rate null                                     */
  DSRLPI_FILL    = 9, /* Fill frame                                        */
  DSRLPI_RESET    = 10,   /* free frames before reset                      */
  DSRLPI_DCCH_DTX = 11    /* DTX on the DCCH                               */
} dsrlpi_fr_det_enum_type;

/*---------------------------------------------------------------------------
  Bit mask positions for different frame categories. This is used in 
  augmenting sub-categories of the major frame processing type in the 
  dsrlpi_fr_det_enum_type. Frame determination extracts this extra piece of 
  information.
---------------------------------------------------------------------------*/
#define CURR_FRAME_SYNC_MASK               0x01  /* SYNC frame indication  */
#define CURR_FRAME_ACK_MASK                0x02  /* ACK frame indication   */
#define CURR_FRAME_LST_BIT_MASK            0x04  /* LST bit indication set */
#define CURR_FRAME_RESET_MASK              0x08  /* RST bit indication set */
#define CURR_FRAME_SEQ_MASK                0x10  /* 8-bit SEQ OTA present  */
#define CURR_FRAME_SEQ_HI_MASK             0x20  /* 4-bit SEQ_HI present   */
#define CURR_FRAME_S_SEQ_MASK              0x40  /* 12-bit S_SEQ present   */
#define CURR_FRAME_IN_DELQ_MASK            0x80  /* from delay fr arrays   */
/*---------------------------------------------------------------------------
  Mask to detect whether a frame is a sync, sync/ack or ack.
---------------------------------------------------------------------------*/
#define CURR_FRAME_SYNC_ACK_DETECT_MASK    0x03

/*---------------------------------------------------------------------------
  Type for NAK list entry. The NAK list is used to keep track of missing
  RLP data frames, and the associated retransmission/abort timers.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type    link;       /* Queue link used to queue/dequeue the entry */
 // boolean        tx_done;    /* TRUE => NAK transmission complete          */
  uint32         first_seq;  /* 1st missing Sequence number                */
  uint32         last_seq;   /* last missing Sequence number               */
  byte           age;        /* Delta/Age of the NAK list entry, in frames */
  byte           curr_rnd;   /* Which round are we on for this NAK         */
  byte           naks_left;  /* How many NAKs left in this round           */
} dsrlpi_nak_list_entry_type;

/*---------------------------------------------------------------------------
  Type for information extracted from the received frame. Used for frame 
  type based processing. Information includes seq number, frame category, etc
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 seq_num;                                /* Frame seq num          */
  uint32 fr_cat;                                 /* Frame category bitmask */
  dsrlp_mux_pdu_enum_type mux_pdu_type;          /* mux pdu of this frame  */
} dsrlpi_curr_fr_info_type;

/*---------------------------------------------------------------------------
  RLP version dependendent const definitions. This struct contains the static
  information for each revision of RLP.  It includes a function pointer 
  table, which is used to call into the RLP version-specific functions for
  frame processing. 
---------------------------------------------------------------------------*/


typedef struct
{
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table; /* Ptr table to RLP functions*/
  uint16 max_conseq_erasures;                 /* Max # of conseq erasures  */
  uint32 seq_modulus;                         /* 2^num-bits in seq. no.    */

  uint16 txq_size;                            /* Max size of tx queue      */

  byte   max_nak_rounds_fwd;                  /* Max. NAK rounds for BS    */
  byte   max_nak_rounds_rev;                  /* Max. NAK rounds for MS    */
  
  byte   desired_nak_rounds_fwd;              /* Def. NAK rounds for BS    */
  byte   desired_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];
                                              /* Def. NAKs per round for BS*/
  byte   desired_nak_rounds_rev;              /* Default NAK rounds for MS */
  byte   desired_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];
                                              /* Def. NAKs per round for MS*/
  boolean seg_allowed;                        /* frames can be segmented?  */
} dsrlpi_ver_const_type;

/*---------------------------------------------------------------------------
  This is the max number of dsm item chains to be removed with a single call
  to the dsrlprsq_remove() from the resequencing Q. The reason is every dsm
  item chain causes a separate dsm_enqueue() operation.
  
  Make sure to remove more chains than can be received in one frame time.
---------------------------------------------------------------------------*/
#define DSRLPI_MAX_CHAINS_TO_REMOVE  (RLP_MAX_FWD_PDU * 3)

/*---------------------------------------------------------------------------
  RLP configuration definition. This structure contains all RLP 
  configuration information and is updated at the beginning of an RLP 
  session.
---------------------------------------------------------------------------*/
typedef struct 
{
  dsrlp_ver_enum_type ver;                    /* Current RLP version       */
  uint32  dsrlp_rsq_max_chain_cnt;            /* Max  to remove from rsq   */
  boolean octet_sequenced;                    /* Octet sequenced operation?*/
  dsm_watermark_type *tx_wm_ptr;              /* Output watermark for txmit*/
  void (*rx_func_ptr)();                      /* rx callback               */
  void (*notify_ppp_data_transfer_func_ptr)(byte rlp_instance);
                                              /* Notify PZID hyst engine?  */
  boolean ppp_data_transfer_after_dormancy;   /* Was PPP data transferred
                                              after waking up from dormancy*/
  q_type *rx_q_ptr;                           /* queue for rxed frames     */
} dsrlpi_cfg_info_type;

/*---------------------------------------------------------------------------
  Enum of the RLP states
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLPI_SYNC_STATE = 0,                      /* Tx SYNCs                  */
  DSRLPI_SYNC_ACK_STATE,                      /* Tx SYNC/ACKs              */
  DSRLPI_ACK_STATE,                           /* Tx ACKs - calc. RTT       */             
  DSRLPI_RT_MEAS_STATE,                       /* Tx idle/data - calc. RTT  */
  DSRLPI_ESTABLISHED_STATE,                   /* Steady state operation    */
  DSRLPI_RESET_STATE,                         /* Reset                     */
  DSRLPI_NULL_STATE                           /* Null state. No (HDR) RLP  */
} dsrlpi_state_enum_type;

/*---------------------------------------------------------------------------
  #defines used in RLP 3 mostly.  The Round trip frame counter is used to
  make sure some idles are sent on the DCCH, so that the base station can
  calculate it's RLP Delay.  The idle timer makes sure some frames does go
  out the pipe...but only up to the frame transmission counter limit.
---------------------------------------------------------------------------*/
#define  DSRLPI_RND_TRIP_FR_CNTR_DEF     4    /* rnd trip frame counter def*/
                                              /* value.  must be >= 4      */
#define  DSRLPI_IDLE_TIMER_DEF           5    /* idle timer default value  */
#define  DSRLPI_IDLE_FR_XMIT_CNTR_DEF    3    /* idle frame xmission cntr  */
                                              /* default value.            */
/*---------------------------------------------------------------------------
  Count of how many FCH frames can be sent before forcing a FILL frame. This
  is so the peer's V(N) updates correctly.  Sending a NAK is like sending
  a FILL,as that also updates the other side to our V(N).
  
  Define the number as 80;  Since seq. number ambiguity starts at 255, this
  allows 3 tries to get the V(N) info to the other side.
---------------------------------------------------------------------------*/
#define DSRLPI_LAST_FILL_NAK_GAP   80

/*---------------------------------------------------------------------------
  Maximum time (in frames) to synch up RLP. Set real high for MTOM
  Korea builds.                     
---------------------------------------------------------------------------*/
#define DSRLPI_MAX_RTT_SETUP       900 

/*---------------------------------------------------------------------------
  RLP state information definition. This structure contains all of the RLP 
  state information for the currently active RLP session.
---------------------------------------------------------------------------*/
typedef struct
{
  dsrlpi_state_enum_type state;   /* RLP State                             */

  /* Receive variables */

  //  boolean first_sync_rcvd;    /* Received a SYNC since last reset      */
  boolean ack_to_send;            /* pending ack (recv sync_ack) to send   */
  uint32  expected_seq_num;       /* V(R), Next sequence number expected   */
  uint32  needed_seq_num;         /* V(N), Next sequence number needed     */
  uint32  peer_vn;                /* Other side's V(N)                     */
  word    contig_erase;           /* Contiguous erasures                   */
  word    rlp_delay;              /* RLP_DELAY, Round trip time in frames  */
  word    sync_delay;             /* keep track how long it takes to sync  */
  //  boolean sync_rcvd;          /* Last RX had SYNC, next TX needs ACK   */
  boolean hole_active;            /* TRUE when RX stream has a hole        */

  /* Transmit variables */
  byte    saved_frames_index;     /* index into saved frames array         */
  byte    saved_frames_cnt;       /* num of saved data frames in buf       */
  uint32  next_seq_num;           /* V(S), Next sequence number to use     */
  boolean naks_to_send;           /* Does RLP have NAKs to send            */

  boolean rst_to_send;            /* pending RST signaling msg to send     */
  boolean rst_ack_to_send;        /* pending RST ACK signaling msg to send */
  boolean mob_initiated_reset;    /* mobile initiated the reset procedures */
  boolean msg_to_process;         /* RLP message on receive sig queue      */
  boolean abort_timer_active;     /* if the RX NAK abort timer is active   */
  boolean flush_timer_expired;    /* if flush timer exp, build flush pkt   */

  q_type  rx_sig_msg_q;           /* RLP receive signaling message queue   */

  dsm_item_type* tx_item_ptr;     /* Ptr holding item last dequeued for tx */
  boolean idle_timer_active;      /* if the idle timer is active           */
  byte    idle_timer;             /* used when xmitting idle/blank frames  */
  byte    idle_fr_xmit_cntr;      /* #idle frames sent due to idle timer   */
                                  /* expiration                            */
  byte    rnd_trip_fr_cntr;       /* used to determine if idle frame should*/
                                  /* be sent as a data frame               */
  word    last_fill_nak_cnt;      /* number fr's since a nak or gap sent   */
  uint32  mux_erasure_cnt;        /* mux erasures rx'ed in past 100 fch frs*/

  byte    nak_rounds_fwd;
  byte    naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];
  byte    nak_rounds_rev;
  byte    naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];

  q_type  nak_q;                  /* NAK list for this version             */
  q_type  reseq_q;                /* Resequencing q for this version       */
  boolean reseq_q_more_data;      /* more data chains for rx watermark     */
  
  dsm_item_type *reseq_last_chain_ptr;/* last dsm  chain on q              */
  dsm_item_type *reseq_last_item_ptr; /* last DSM item of last chain on q  */
  dsm_item_type *reseq_last_insert_chain_ptr;/* point to most recent insert*/
  dsm_item_type *reseq_last_insert_item_ptr; /* last DSM item of chain     */

  q_type          tx_q;           /* The transmit q for this version       */
  dsm_item_type*  tx_q_recent;    /* pts to last accessed itme from txq    */

  uint32  rtxq;                   /* retransmit queue pointer              */
  uint32  del_fr_id;              /* delayed frame arrays id               */

  word    old_frames;             /* number old frames rx'ed consecutively */
  boolean big_nak_gap;            /* prev new/idle resulted in big nak gap */
  boolean reset_rlp;              /* Reset flag for this RLP               */
  boolean reset_active;           /* is a reset occuring right now?        */
} dsrlpi_state_info_type;


/*---------------------------------------------------------------------------
  RLP session control block definition. This structure contains all RLP 
  session information and is comprised of configuration, RLP state, and
  statistical information.
---------------------------------------------------------------------------*/
typedef struct     
{
  byte                   sr_id;   /* Service ID for this RLP               */
  dsrlpi_cfg_info_type   cfg;     /* configuration information             */
  dsrlpi_state_info_type state;   /* state information                     */
  dsrlp_stats_info_type  stats;   /* statistical information               */
  dsrlpi_ver_const_type* ver_ptr; /* Ptr to version control block          */

/*---------------------------------------------------------------------------
   Debug flag to indicate if RLP is currently cleaning up while going NULL   
---------------------------------------------------------------------------*/
  boolean                cleanup_for_going_null; 
                                         
#ifdef FEATURE_HDR 
#error code not present
#endif /* FEATURE_HDR */

} dsrlpi_sess_ctl_blk_type;                     

/*---------------------------------------------------------------------------
  RLP function pointer table.  
  Each function pointer represents specific processing per RLP version.
---------------------------------------------------------------------------*/
struct dsrlpi_fcn_ptr_table
{
  dsrlpi_fr_det_enum_type (*rx_frame_det)(dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_fwd_frame_type   *fwd_fr_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr); 

  dsrlpi_fr_det_enum_type (*rx_new_data) (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsm_item_type    **fr_item_pkt_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_rexmit)   (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_fwd_frame_type *fwd_fr_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr);

  dsrlpi_fr_det_enum_type (*rx_init)     (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr); 

  dsrlpi_fr_det_enum_type (*rx_idle)     (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                    dsrlpi_curr_fr_info_type *curr_info_ptr); 

  dsrlpi_fr_det_enum_type (*rx_rlp_erasure)
                                         (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_fwd_frame_type *fwd_fr_ptr);

  dsrlpi_fr_det_enum_type (*rx_mux_erasure)
                                         (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_fwd_frame_type *fwd_fr_ptr);

  boolean (*tx_new_data)                 (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_rexmit)                   (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_nak)                      (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_init)                     (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);

  boolean (*tx_idle)                     (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);
  boolean (*tx_fill)                     (dsrlpi_sess_ctl_blk_type *rscb_ptr,
                                          dsrlp_rev_frame_type *rev_fr_ptr);
};

/*---------------------------------------------------------------------------
  Array of session control blocks. There is one session control block, per
  sr_id. At present, the RLP code supports 2 RLP sessions running at the same
  time. These will have index of 1 and 2 in the array. (index 0 is reserved).
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
#define DSRLPI_MAX_RSCB_ELEMENTS  4 
#else
#define DSRLPI_MAX_RSCB_ELEMENTS  3
#endif /* FEATURE_DATA_MM */
extern  dsrlpi_sess_ctl_blk_type    dsrlpi_rscb_array[ DSRLPI_MAX_RSCB_ELEMENTS];

/*---------------------------------------------------------------------------
  Mapping betwen the service provider's sr_id and index in dsrlpi_rscb_array. 
  Internally, the code supports 2 RLP sessions with array indices 1 and 2.
  However, sr_id is a 3 bit field in MUX PDU3 and the sr_id for a given call
  assigned by the service provider can be anywhere from 1 to 6 (7 is reserved).
  
  For HDR, the sr_id is not assigned by the basestation, but rather they are 
  internally usd values. Since we do not want collision with the CDMA sr id
  values, values out of cdma range (7 and 8) are used for HDR.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM

#define DSRLPI_MAX_CDMA_SR_ID_VALUE 7
#define DSRLP_HDR_SN_RLP_SR_ID_VALUE   (DSRLPI_MAX_CDMA_SR_ID_VALUE)
#define DSRLPI_HDR_AN_AUTH_SR_ID_VALUE (DSRLPI_MAX_CDMA_SR_ID_VALUE + 1)
#define DSRLPI_MAX_SP_SR_ID_VALUE      (DSRLPI_MAX_CDMA_SR_ID_VALUE + 2)

#else /* FEATURE_DATA_MM */
#define DSRLPI_MAX_SP_SR_ID_VALUE 7
#endif /* FEATURE_DATA_MM */

#define DSRLPI_INVALID_RSCB_INDEX DSRLPI_MAX_RSCB_ELEMENTS
extern byte dsrlpi_sr_id_rscb_map[ DSRLPI_MAX_SP_SR_ID_VALUE];

/*---------------------------------------------------------------------------
  Maximum number of RLP sessions allowable at one time.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
#define DSRLPI_MAX_SESSIONS 3
#else
#define DSRLPI_MAX_SESSIONS 2
#endif /* FEATURE_DATA_MM */

/*---------------------------------------------------------------------------
  Type and table that contains primary and secondary channel full rate mask, 
  TYPE, and Payload fields values. 
---------------------------------------------------------------------------*/
typedef struct
{
  byte type_mask;         /* Field that contains the TYPE field mask       */
  byte format_a;          /* TYPE field Format A value                     */
  byte format_bc_new;     /* TYPE field Format B/C new data frames         */
  byte format_bc_rexmit;  /* TYPE field Format B/C rexmit data frames      */
  byte format_d;          /* TYPE field Format D value                     */
} dsrlpi_type_mask_table_type;

/*---------------------------------------------------------------------------
  Table is indexed by MuxPDU type and service class (primary or secondary).  
---------------------------------------------------------------------------*/
extern const dsrlpi_type_mask_table_type dsrlpi_type_mask_table[4][3];

/*---------------------------------------------------------------------------
  Bitmask used to say when a 32-bit number is generated, whether there
  was a 4-bit SEQ_HI, 8-bit SEQ, or 12-bit S_SEQ included in the info.
  For example, in an unsegmented data frame, only the 8-bit SEQ is put into
  the header.  Therefore, for the functions called, it is not known if
  the 4-bit SEQ_HI is really 0000, or if it is 0000 because frame
  determination did not know what the value wa.
---------------------------------------------------------------------------*/
#define DSRLPI_SEQ_HI_MASK   0x01
#define DSRLPI_SEQ_MASK      0x02
#define DSRLPI_S_SEQ_MASK    0x04  

#define DSRLPI_BIG_NAK_GAP   (3 * (8 + 1))

/*---------------------------------------------------------------------------
  This constant is used to decide whether RLP should get a SMALL ITEM or a 
  LARGE ITEM. The size of the requested item is right shifted (divided by 4)
  by this number. If the result is larger than DSM_SMALL_ITEM_SIZ, a small
  item is allocated otherwise a large item is allocated.
---------------------------------------------------------------------------*/
#define DSRLPI_ITEM_SIZ_DIVISOR 2

/*---------------------------------------------------------------------------
  Bytes used in printing out RLP-level erasures, to help in debugging.
---------------------------------------------------------------------------*/
extern byte dsrlpi_ctl_byte1,dsrlpi_ctl_byte2,dsrlpi_ctl_byte3;
extern byte dsrlpi_ctl_used;


/*===========================================================================

MACRO          DSRLPI_SET_SEQ_HI_MASK

DESCRIPTION    Sets the SEQ_HI in the input var.

PARAMETERS     a byte.  But this is an expression, and assumes that the call
               looks like this:
               
               x = DSRLPI_SET_SEQ_HI_MASK(x);
                                           
DEPENDENCIES   None.

RETURN VALUE   The changed byte.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_SEQ_HI_MASK(x) \
       (x | CURR_FRAME_SEQ_HI_MASK)

/*===========================================================================

MACRO          DSRLPI_HAS_SEQ_HI_MASK

DESCRIPTION    Tells if the input byte has the seq_hi mask bit set.

PARAMETERS     a byte.
                                           
DEPENDENCIES   None.

RETURN VALUE   TRUE if the bit was set, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_SEQ_HI_MASK(x) \
       ((x & CURR_FRAME_SEQ_HI_MASK) ? TRUE : FALSE)


/*===========================================================================

MACRO          DSRLPI_SET_SEQ_MASK

DESCRIPTION    Sets the SEQ bit in the input var.

PARAMETERS     a byte.  But this is an expression, and assumes that the call
               looks like this:
               
               x = DSRLPI_SET_SEQ_MASK(x);
                                           
DEPENDENCIES   None.

RETURN VALUE   The changed byte.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_SEQ_MASK(x) \
       (x | CURR_FRAME_SEQ_MASK)

/*===========================================================================

MACRO          DSRLPI_HAS_SEQ_MASK

DESCRIPTION    Tells if the input byte has the seq mask bit set.

PARAMETERS     a byte.
                                           
DEPENDENCIES   None.

RETURN VALUE   TRUE if the bit was set, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_SEQ_MASK(x) \
       ((x & CURR_FRAME_SEQ_MASK) ? TRUE : FALSE)

/*===========================================================================

MACRO          DSRLPI_SET_S_SEQ_MASK

DESCRIPTION    Sets the S_SEQ bit in the input var.

PARAMETERS     a byte.  But this is an expression, and assumes that the call
               looks like this:
               
               x = DSRLPI_SET_S_SEQ_MASK(x);
                                           
DEPENDENCIES   None.

RETURN VALUE   The changed byte.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_S_SEQ_MASK(x) \
       (x | CURR_FRAME_S_SEQ_MASK)

/*===========================================================================

MACRO          DSRLPI_HAS_S_SEQ_MASK

DESCRIPTION    Tells if the input byte has the s_seq mask bit set.

PARAMETERS     a byte.
                                           
DEPENDENCIES   None.

RETURN VALUE   TRUE if the bit was set, false otherwise

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_S_SEQ_MASK(x) \
       ((x & CURR_FRAME_S_SEQ_MASK) ? TRUE : FALSE)

/*===========================================================================

MACRO          DSRLPI_SET_LAST_SEG_MASK

DESCRIPTION    Sets the last segment bit indicator in the fr_cat mask.

PARAMETERS     a 32-bit uint.
                                           
DEPENDENCIES   None.

RETURN VALUE   NONE

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_SET_LAST_SEG_MASK(x) \
       (x | CURR_FRAME_LST_BIT_MASK)

/*===========================================================================

MACRO          DSRLPI_HAS_LAST_SEG_MASK

DESCRIPTION    Sees if the last seg bit is set.

PARAMETERS     a 32-bit uint.
                                           
DEPENDENCIES   None.

RETURN VALUE   TRUE if it is set, FALSE otherwise.

SIDE EFFECTS   None.

===========================================================================*/
#define DSRLPI_HAS_LAST_SEG_MASK(x) \
       (((x & CURR_FRAME_LST_BIT_MASK) || !( x & CURR_FRAME_S_SEQ_MASK)) \
        ? TRUE : FALSE)

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION       DSRLPI_GET_BEST_FR_RATE

DESCRIPTION    This function returns the smallest available rate that a frame
               of the given length can be transmitted at. RLP conveys this
               rate to the Mux layer.
               
DEPENDENCIES   Assumes that non-NULL pointers are passed in.
               This will only work for MuxPDU type1 and MuxPDU type2.

RETURN VALUE   Returns the rate at which this frame should be sent at.

SIDE EFFECTS   NONE
===========================================================================*/
dsrlp_rate_enum_type dsrlpi_get_best_fr_rate
(
   dsrlp_rev_frame_type    *rev_fr_ptr,  /*  contains rev frame info.      */
   word                     fr_len       /* Num of bytes in frame          */
);

/*===========================================================================

FUNCTION       DSRLPI_RX_INIT_FRAME

DESCRIPTION    This function handles incoming SYNC, SYNC/ACK, and ACK frames
               and increments/resets the RLP initialization state machine
               accordingly.

DEPENDENCIES   Assumes that the incoming frame is either a SYNC, SYNC/ACK,
               or ACK.

RETURN VALUE   Always returns success DSRLPI_DONE.

SIDE EFFECTS   Depending on the state and incoming frame, the round trip
               delay timer may be incremented.
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_rx_init_frame
(
  dsrlpi_sess_ctl_blk_type  *rscb_ptr,       /* rlp-specific control block */
  dsrlpi_curr_fr_info_type  *frame_info_ptr  /* current frame information  */
);

/*===========================================================================

FUNCTION       DSRLPI_RX_NEW_DATA

DESCRIPTION    This function receives new data (segmented or unsegmented)
               and processes it.  If there is skew in the incoming seq
               number, then some Nak generation may occur.
               
               This code is same for all versions of RLP.
               
DEPENDENCIES   CRC checks done by frame determination.
               Input pointers are non-NULL.  
               The dsm_item embedded within fwd_fr_ptr contains only the RLP
                frame's data payload, and does not contain any header info
                or CRC info.

RETURN VALUE   Always returns DONE.

SIDE EFFECTS   V(R) and/or V(N) may get updated.  Entries may be inserted
               into the NAK list.  Entries may be added or removed from
               the resequencing buffer.
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_rx_new_data
(
  dsrlpi_sess_ctl_blk_type *rscb_ptr,        /* Ptr to session ctl block   */
  dsm_item_type            **fr_item_pkt_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr    /* frame info ptr             */
);


/*===========================================================================

FUNCTION       DSRLPI_PROCESS_REXMIT_FRAME

DESCRIPTION    This function will process re-tansmitted frames. It will 
               first remove them from the nak list and then insert into the
               resequencing queue. After that any frames that can be passed 
               to higher layers will be moved from the resequencing q into 
               the rx watermark structure.
              
DEPENDENCIES   Non-null pointers passed into this function

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_process_rexmit_frame
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,   /* Ptr to session ctl block    */
  dsrlp_fwd_frame_type         *fwd_fr_ptr, /* ptr to received RLP frame   */
  dsrlpi_curr_fr_info_type     *fr_info_ptr /* current frame info          */
);
/*===========================================================================

FUNCTION DSRLPI_INIT_CONSTANTS

DESCRIPTION
  This function is called from dsrlp_init. All the version specific constants
  including function pointers are initialized here.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsrlpi_init_constants
(
  dsrlpi_sess_ctl_blk_type  *rscb_ptr,       /* rlp-specific control block */
  dsrlp_ver_enum_type        ver             /* RLP version                */
);
/*===========================================================================

FUNCTION       DSRLPI_RESET

DESCRIPTION    Resets RLP. This involves clearing various values and
               resetting the RLP state machine.
               
               Note, stats.resets counter is incremented outside this 
               function, because it is only incremented for RLP resets in 
               the middle of a call (error conditions).

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpi_reset
(
  dsrlpi_sess_ctl_blk_type       *rscb_ptr   /* rlp-specific control block */
);

/*===========================================================================

FUNCTION        DSRLPI_CRC_GEN

DESCRIPTION    This function will generate the CRC for the passed byte stream
               and store the CRC at the end of the passed stream.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void dsrlpi_crc_gen
(
  byte *buf,          /* pointer to Frame buffer to put CRC at the end     */
  word len            /* length of the Frame buffer, including CRC bytes   */
);

/*===========================================================================

FUNCTION       DSRLPI_GET_MIN_FRAME_AND_LEN

DESCRIPTION    This function returns the minimum frame size supported by
               a reverse frame and also returns the length (in bytes) of
               that frame.  Values are returned via passed-in ptrs.
               
               The user can specify a minimum size (i.e, the returned
               frame size must be at least a certain frame rate).
               
               If the proper frame is not found, then the input parameters
               are unchanged.
               
DEPENDENCIES   Assumes that the frame rate bitmasks are defined such 
               that the smallest frame rate is 0x01, and each successively
               larger frame rate is that value shifted one bit to the left.
               
               Assumes that non-NULL pointers are passed in.
               
               Assumes that the input min_fr_rate is equivalent to one
               of the frame rate #defines.

RETURN VALUE   TRUE if the frame info could be generated.  FALSE if the
               frame info could not be generated (for example, the 
               requested minimum size was not supported by the rev frame).

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_get_min_frame_and_len
(
   dsrlp_rev_frame_type    *rev_fr_ptr,  /*  contains rev frame info.      */
                                         /* fr_rate_ptr updated in function*/
   uint16                   min_fr_rate, /*  min frame rate needed, using  */
                                         /*  bitmask                       */
   byte                    *fr_len       /*  returned frame len (bytes)    */
);
/*===========================================================================

FUNCTION       DSRLPI_TX_NEW_DATA_FRAME

DESCRIPTION    This function constructs an new data frame from payload 
               available in the transmit watermark.
               
               Only non segmented new frames are constructed.
               
               This code is specific to RLP 1, RLP2 and RLP3.
               
DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.
               
               rscb_ptr->state.idle_timer is only decremented in this
               function.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_tx_new_data_frame
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,   /* Ptr to session ctl block    */
  dsrlp_rev_frame_type         *rev_fr_ptr  /* Ptr to frame to be built    */
);

/*===========================================================================

FUNCTION       DSRLPI_REXMIT_DATA_FRAME

DESCRIPTION    This function constructs an new data frame from payload 
               available in the transmit watermark.
               
               Only non segmented new frames are constructed.
               
               This code is specific to RLP 1, RLP2 and RLP3.
               
DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.
               
               rscb_ptr->state.idle_timer is only decremented in this
               function.
               
               This function assumes RLP is in DSRLPI_ESTABLISHED_STATE
               or DSRLPI_RT_MEAS_STATE. This function should not be called
               if RLP cannot transmit new data frames.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_rexmit_data_frame
(
  dsrlpi_sess_ctl_blk_type     *rscb_ptr,   /* Ptr to session ctl block    */
  dsrlp_rev_frame_type         *rev_fr_ptr  /* Ptr to frame to be built    */
);

/*===========================================================================

FUNCTION       DSRLPI_GET_RCSB_INDEX_FOR_SR_ID

DESCRIPTION    Get the RLP instance (rscb array index) used by the service
               corresponding to the given sr_id
               
DEPENDENCIES   None

RETURN VALUE   The RLP instance, if the sr_id is mapped, an invalid index
               otherwise

SIDE EFFECTS   None
===========================================================================*/
byte dsrlpi_get_rscb_index_for_sr_id
(
  byte sr_id                              /* Passed in SR_ID               */
);

/*===========================================================================

FUNCTION       DSRLPI_SET_RCSB_INDEX_FOR_SR_ID

DESCRIPTION    Set the mapping between the RLP instance and SR_ID used by a
               service.
               
DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Invalidates the old mapping, if any between an SR_ID and the
               RLP instance.
===========================================================================*/
void dsrlpi_set_rscb_index_for_sr_id
(
  byte sr_id,                            /* SR_ID for the service          */
  byte rscb_index                        /* RLP instance for the service   */
);

/*===========================================================================

FUNCTION       DSRLPI_SRIDMAP_INIT

DESCRIPTION    Initialize the mapping between SR_IDs and RLP instances to
               invalid values.
               
DEPENDENCIES   This dunction should be called once at startup.

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/
void  dsrlpi_sridmap_init
(
  void
);
#endif  /* FEATURE_DS */

#endif  /* _DSRLP_H */

