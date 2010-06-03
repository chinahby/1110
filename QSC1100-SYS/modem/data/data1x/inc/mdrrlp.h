#ifndef MDRRLP_H
#define MDRRLP_H
/*===========================================================================

                        M E D I U M   D A T A   R A T E
                     R A D I O   L I N K   P R O T O C O L
		            H E A D E R   F I L E

DESCRIPTION
  This header file contains the interface definition for the Medium Data Rate
  Radio Link Protocol (RLP Type 2) specified in IS-707-A. The implementation
  is backward compatible with the Radio Link Protocol (RLP Type 1) specified
  in IS-707.  The software can be configured at run time to use either RLP
  Type 1 or RLP Type 2.

  For Medium Data Rate operation (RLP Type 2), this implementation supports
  up to 7 forward supplemental channels, for both Rate Set 1 and Rate Set 2.
  Reverse supplemental channels are not supported.

  Only RLP carried as primary traffic is supported. Support for RLP carried
  as secondary or signaling traffic is not provided.

  Copyright (c) 1998,1999 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/mdrrlp.h_v   1.0   23 Oct 2000 17:32:26   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/inc/mdrrlp.h#1 $ $DateTime: 2008/04/11 07:35:12 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/17/99   smp     Removed include files msg.h & err.h.
02/10/99   smp     Added PACKED qualifier to union inside rlp_gen_pri_fr_type,
                   and to rlp_gen_frame_type union.
09/22/98   smp     Initial version of MDR RLP header file, based on rlp.h
                   version 1.15.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_IS95B_MDR
                                  /* Only include this file for MDR builds */
#include "queue.h"
#include "dsm.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Definitions for various RLP frame sizes.
---------------------------------------------------------------------------*/
#define RLP_PRI_FRATE_BIT_SIZ          171   /* Primary Full Rate bit count*/
#define RLP_GEN_FRATE_BIT_SIZ          168   /* Generic Full Rate bit count*/
#define RLP_7_8TH_RATE_BIT_SIZ         152   /* 7/8ths Rate bit count      */
#define RLP_3_4TH_RATE_BIT_SIZ         128   /* 3/4ths Rate bit count      */
#define RLP_GEN_HRATE_BIT_SIZ           88   /* Generic Half Rate bit count*/
#define RLP_PRI_1_8TH_RATE_BIT_SIZ      16   /* Primary 1/8 Rate bit count */

#define RLP2_FULL_PRI_DATA_BYTES        32
#define RLP2_FULL_GEN_DATA_BYTES        30
#define RLP2_FULL_PRI_CTL_UNUSED_BYTES  27   /* (264/8 - 6)                */
#define RLP2_FULL_GEN_CTL_UNUSED_BYTES  27   /* (264/8 - 6)                */
#define RLP2_7_8_GEN_CTL_UNUSED_BYTES   24   /* (242/8 - 6)                */
#define RLP1_FULL_PRI_DATA_BYTES        20

#define RLP_TYPE2_INIT_UNUSED_BYTES     29   /* (264/8 - 4)                */
#define RLP_TYPE2_NAK_UNUSED_BYTES      18   /* (264/8 - 15)               */
#define RLP_TYPE2_NAK_MAX_LEN           12   /* w/o SEQ,CTL,NAK_TYPE,L_SEQ_HI*/

/*---------------------------------------------------------------------------
  Definitions for the TYPE field of primary channel full rate RLP frames.
---------------------------------------------------------------------------*/
#define RLP1_PRI_FULL_RATE_A         0x20/* Generic Primary Full Rate type */
#define RLP1_PRI_FULL_RATE_B_NEW     0x40/* Format B New Data Frame type   */
#define RLP1_PRI_FULL_RATE_B_REXMIT  0x60/* Format B Rexmit Data Frame type*/

#define RLP2_PRI_FULL_RATE_A         0x40/* Generic Primary Full Rate type */
#define RLP2_PRI_FULL_RATE_B_NEW     0x80/* Format B New Data Frame type   */
#define RLP2_PRI_FULL_RATE_B_REXMIT  0xC0/* Format B Rexmit Data Frame type*/  

/*---------------------------------------------------------------------------
  Definition for the storage size of individual items of raw data bytes. Any
  arrays storing data processed from RLP RX should be sized as a multiple of
  this value.
---------------------------------------------------------------------------*/
#define RLP_DATA_STORAGE_SIZ  RLP_PRI_FRATE_DATA_SIZ

/*---------------------------------------------------------------------------
  Type for RLP traffic types.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_TRAFFIC_TYPE_PRI = 0,  /* Primary Traffic   */
  RLP_TRAFFIC_TYPE_SEC       /* Secondary Traffic */
} rlp_traf_type_enum_type;

/*---------------------------------------------------------------------------
  Type for Rate Sets. NOTE: Used as an index.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_RATE_SET_1 = 0,  /* Rate Set 1 (9.6 kbps)  */
  RLP_RATE_SET_2       /* Rate Set 2 (14.4 kbps) */
} rlp_rate_set_enum_type;

/*---------------------------------------------------------------------------
  Type for RLP Types.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_TYPE_I,  /* IS-707 RLP */
  RLP_TYPE_II  /* IS-707-A (Medium Data Rate) RLP */
} rlp_type_enum_type;

/*---------------------------------------------------------------------------
  Type for return value from RLP RX processing function.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_RX_SYNCH_TIMEOUT,          /* RLP timed out while attempting to SYNC */
  RLP_RX_IDLE,                   /* Idle frame(s) rx'ed                    */
  RLP_RX_NON_IDLE,               /* At least 1 Ctl/Data/Retx frame rx'ed   */
  RLP_RX_ERROR                   /* Errors in rx'ed frame(s)               */
} rlp_rx_retval_enum_type;

typedef enum
{
  RLP_RX_DATA_AVAILABLE,               /* RLP RX processing yielded data   */
  RLP_RX_NO_DATA_AVAILABLE,            /* RLP RX processing yielded no data*/
  RLP_RX_FRAME_ERR                     /* RX frame error occurred          */
} rlp_rx_status_enum_type;

/*---------------------------------------------------------------------------
  Type for RLP State.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_SYNC_STATE = 0,                /* Initial state                      */
  RLP_RT_MEAS_STATE,                 /* ACK received from the other side   */
  RLP_ESTABLISHED_STATE              /* Data/Idle received from other side */
} rlp_state_enum_type;

/*---------------------------------------------------------------------------
  Type to convey RLP stats to an external unit.
---------------------------------------------------------------------------*/
typedef struct
{
  word                rx_data_cnt;              /* count of RX data frames */
  word                tx_data_cnt;              /* count of TX data frames */
  byte                nak_cnt;                  /* count of NAKs sent      */
  byte                rexmit_cnt;               /* count of REXMITS sent   */
  rlp_state_enum_type state;                    /* current RLP state       */
  word                rx_frame_cnt;             /* count of all RX frames  */
  word                tx_frame_cnt;             /* count of all TX frames  */
} rlp_ext_stats_type;

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*---------------------------------------------------------------------------
  Type for frame rates. These rates are for both Rate Set 1 and Rate Set 2.
  NOTE: These frame rates are used as indexes into arrays, DO NOT ALTER.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_EIGHTH_RATE = 0,                /* Idle frame, Primary channel only  */
  RLP_3_16TH_RATE,                    /* 3/16th rate, Rate Set 2 only      */
  RLP_QUARTER_RATE,                   /* 1/4 rate frame, not used for RLP  */
  RLP_3_8TH_RATE,                     /* 3/8th rate, Rate Set 2 only       */
  RLP_7_16TH_RATE,                    /* 7/16th rate, Rate Set 2 only      */
  RLP_HALF_RATE,                      /* 1/2 rate frame                    */
  RLP_3_4TH_RATE,                     /* Secondary/signaling channels only */
  RLP_7_8TH_RATE,                     /* Secondary/signaling channels only */
  RLP_FULL_RATE,                      /* Full rate frame                   */
  RLP_ERASURE,                        /* Erasure frame                     */  
  RLP_BLANK                           /* No RLP data                       */         
} rlp_rate_enum_type;

/*---------------------------------------------------------------------------
  Maximum number of forward supplemental channels supported.
---------------------------------------------------------------------------*/
#define RLP_MAX_FWD_SUP        7

/*---------------------------------------------------------------------------
  Type for Forward Traffic Channel Frame Block. This structure is used by
  RXC (i.e. the Mux Layer) to pass up RLP frames received over the forward
  traffic channel. Note that the first element of the fwd_frames[] array is
  always the Fundamental RLP frame, and hence must always be present.
---------------------------------------------------------------------------*/
typedef struct
{
  byte num_sup;                         /* Number of supp RLP frames rx'ed */
  struct
  {
    rlp_rate_enum_type   frame_rate;    /* Frame rate of the rx'ed RLP frame*/
    dsm_item_type       *rlp_item_ptr;  /* Pointer to the rx'ed RLP frame   */
  } fwd_frames[1+RLP_MAX_FWD_SUP];
} rlp_fwd_frame_block_type;

/*---------------------------------------------------------------------------
  Type for RLP frame sequence number. Note that this is 16 bits wide so that
  it can accomodate either the RLP Type 1 8-bit sequence number or the RLP
  Type 2 12-bit sequence number.
---------------------------------------------------------------------------*/  
typedef word rlp_seq_num_type;

#ifndef T_ARM
#pragma pack(1)   /* Force byte packing */
#endif
/*---------------------------------------------------------------------------
  Types for the various IS-707 RLP frames.
---------------------------------------------------------------------------*/

typedef PACKED struct                           /* Generic Control frame type     */
{
  byte seq_num;                                /* Sequence number          */
  byte ctl_len;                                /* Control & length fields  */
  byte first_seq;                              /* 1st seq # for NAKs       */
  byte last_seq;                               /* last seq # for NAKs      */
  word crc;                                    /* Frame check Seq. CRC     */
  byte unused [RLP2_FULL_PRI_CTL_UNUSED_BYTES];/* unused bytes field       */
} rlp_gen_ctl_type;

typedef PACKED struct                           /* Generic Data frame type        */
{
  byte seq_num;                                /* Sequence number          */
  byte ctl_len;                                /* Control & length fields  */
  byte data [RLP2_FULL_GEN_DATA_BYTES];        /* data (payload) bytes     */
} rlp_gen_data_type;

typedef PACKED struct                           /* Generic Primary Full rate type */
{
  PACKED union
  {
    rlp_gen_data_type  data;                   /* generic data frame type  */
    rlp_gen_ctl_type   ctl;                    /* generic ctrl frame type  */
  } info;
  byte                 type;                   /* frame type indicator     */
} rlp_gen_pri_fr_type;

typedef PACKED struct                           /* Idle (Eighth Rate) Frame Type  */
{
  byte   seq_num;                              /* Next Sequence number     */
  byte   nr_crc;                               /* Nordstrom-Robinson CRC   */
  byte   rs2_filler;                           /* 4 bits: Rate Set 2 only  */
} rlp_pri_idle_type;

typedef PACKED struct                           /* Full Rate Data Frame Type      */
{
  byte   seq_num;                              /* Sequence number of Frame */
  byte   data[RLP2_FULL_PRI_DATA_BYTES];       /* Data bytes               */
  byte   type;                                 /* frame type indicator     */
} rlp_pri_fr_data_type;

/*---------------------------------------------------------------------------
  Types for the various IS-707-A RLP frames.
---------------------------------------------------------------------------*/

typedef PACKED struct                           /* Initialization Frame Type      */
{
  byte seq_num;                                /* Sequence Number          */
  byte ctl_encr_mode;                          /* Control & Encryption Mode*/
  word crc;                                    /* Frame Check Sequence     */
  byte unused[RLP_TYPE2_INIT_UNUSED_BYTES];    /* Unused bytes             */
} rlp_type2_init_type;

typedef PACKED struct                           /* NAK Frame Type                 */
{
  byte seq_num;                                /* Sequence Number          */
  byte ctl_nak_type;                           /* Control & NAK Type       */
  byte lseq_hi_map_cnt;                        /* L_SEQ 4 MSBs & Map Count */
  byte nak_body[RLP_TYPE2_NAK_MAX_LEN];        /* NAK body (var length)    */
  byte unused[RLP_TYPE2_NAK_UNUSED_BYTES];     /* Unused bytes             */
} rlp_type2_nak_type;

typedef PACKED union                 /* Union of all the RLP Frame Types   */
{
  byte                 seq_num;            /* Generic to get sequence #    */
  rlp_gen_data_type    gen_data;           /* Generic Data frame           */
  rlp_gen_ctl_type     gen_ctl;            /* Generic Control frame        */
  rlp_gen_pri_fr_type  pri_gen;            /* Generic Primary full rate    */
  rlp_pri_fr_data_type pri_fr_data;        /* Primary Full Rate Data frame */
  rlp_pri_idle_type    pri_idle;           /* Eighth Rate (Idle) frame     */
  rlp_type2_init_type  type2_init;         /* Type 2 Initialization frame  */
  rlp_type2_nak_type   type2_nak;          /* Type 2 NAK frame             */
} rlp_gen_frame_type;

/*--------------------------------------------------------------------------
  Complete frame information type. 
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  rlp_rate_enum_type rate;              /* Frame rate                      */
  boolean            pri_frame;         /* Primary channel frame indicator */
  rlp_gen_frame_type frame;             /* Actual RLP frame information    */
} rlp_frame_type;

/*--------------------------------------------------------------------------
 Type for providing RLP frames with rate and channel type to RLP RX processing.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type        link;              /* Link for queuing/unqueuing      */
  rlp_rate_enum_type rate;              /* Frame rate                      */
  boolean            pri_frame;         /* Primary channel frame indicator */
  dsm_item_type      *item_ptr;         /* Pointer to item with RLP frame  */
 } rlp_rx_frame_type;

#ifndef T_ARM
#pragma pack()   /* return to default packing */
#endif


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



/*===========================================================================

FUNCTION RLP_TX_GET_NEXT_FRAME

DESCRIPTION
  This function retrieves the next IS-707 RLP data frame to transmit on the 
  traffic channel. This function can operate on either primary or secondary
  CDMA channels.

DEPENDENCIES
  rlp_init should have been previously invoked to initialize the internal
  RLP data structures. rlp_establish_srvc_type() should have been previously
  invoked to set up the RLP Type, Rate Set and Traffic Type in use.

RETURN VALUE
  An enumerated value representing the CDMA frame rate for the RLP TX frame.

SIDE EFFECTS
  1) The 'flow_enable_func_ptr' function will be invoked each time the ring
     buffer goes below the lo water mark.
  2) The 'empty_rx_buf_func_ptr' function will be invoked each time the ring
     buffer becomes empty.
===========================================================================*/

extern rlp_rate_enum_type rlp_tx_get_next_frame
(
  byte               **frame_ptr,    /* address of ptr for the TX frame    */
  boolean            primary_frame,  /* Primary channel yes/no indicator   */
  rlp_rate_enum_type allowed_rate    /* required rate (for sig/sec. chans) */
);

/*===========================================================================

FUNCTION RLP_RX_PROCESS_FRAMES

DESCRIPTION
  This function processes the RLP frame(s) received over the air, as per the
  procedures specified in IS-707 (for RLP Type 1) or IS-707-A (for RLP Type
  2). This function should be invoked once every 20ms and supplied with one
  or more received RLP frames to be processed. Note that the number of
  supplemental RLP frames supplied may be zero, but the fundamental RLP frame
  must always be present, although the fundamental frame rate may indicate a
  BLANK frame or ERASURE.

DEPENDENCIES
  rlp_init() should have been previously invoked to initialize the internal
  RLP data structures. rlp_establish_srvc_type() should have been previously
  invoked to set up the RLP Type, Rate Set and Traffic Type in use.

RETURN VALUE
  The status resulting from the RLP RX processing.

SIDE EFFECTS
  None
===========================================================================*/

extern rlp_rx_retval_enum_type rlp_rx_process_frames
(
  rlp_fwd_frame_block_type  *fwd_frame_block
);

/*===========================================================================

FUNCTION RLP_ESTABLISH_SRVC_TYPE

DESCRIPTION
  This function should invoked to establish the service configuration to be
  used for the RLP session. The service configuration includes the Traffic
  Type (primary or secondary), the Rate Set (1 or 2) and the RLP Type (IS-707
  IS-707-A). This function should be invoked at least once at the beginning
  of each RLP session, and subsequently, each time any of the configuration
  parameters changes. This function is typically invoked by MC at the end of
  service negotiation with the base station.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will cause an RLP reset that will reset the RLP state.
===========================================================================*/

void rlp_establish_srvc_type
(
  rlp_traf_type_enum_type    traffic_typ,  /* Primary or Secondary Traffic */
  rlp_rate_set_enum_type     rate_set,     /* Rate Set 1 or Rate Set 2     */
  rlp_type_enum_type         rlp_typ       /* Type 1 = 707, Type 2 = 707-A */
);

/*===========================================================================

FUNCTION RLP_RESET

DESCRIPTION
  This function performs the work needed to reset the RLP state information 
  in preparation for the next RLP call.

  This function should be called upon completion of a data call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_reset( void);

/*===========================================================================

FUNCTION RLP_INIT

DESCRIPTION
  This function initializes the RLP state machine. Upon completion of this
  function the RLP protocol unit will be ready for operation.

DEPENDENCIES
  This function must be called prior to any other RLP function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_init( void);

/*===========================================================================

FUNCTION RLP_REG_SRVC

DESCRIPTION
  This function prepares the RLP unit to operate using the passed parameters
  for interfacing to the proper data services unit.

DEPENDENCIES
  No parameter field can be NULL.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_reg_srvc
(
  dsm_watermark_type *tx_watermark_ptr,     /* ptr to TX watermark for flow*/
  void               (*post_rx_func_ptr)(), /* func ptr for Post-RX procsg */
  q_type             *post_rx_q_ptr         /* ptr to queue to put RX data */
);

/*===========================================================================

FUNCTION RLP_GET_STATS

DESCRIPTION
  This function returns the current external statistics to the calling 
  function by storing the current items from internal RLP stat counters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_get_stats
(
  rlp_ext_stats_type *stats_ptr      /* pointer to item to load with stats */
);


#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */


#endif /* FEATURE_IS95B_MDR */

#endif /* MDRRLP_H */

