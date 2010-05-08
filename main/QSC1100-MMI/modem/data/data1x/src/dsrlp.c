/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L   E X T E R N A L   F I L E

GENERAL DESCRIPTION
  This file contains functions that implement the external interfaces for
  the RLP modules.
  Function prototypes for the external functions in this file are given in
  dsrlpi.h

  The naming convention in this file is:
  All external functions start with dsrlp..
  There are no internal functions in this file.
  All internal variables (in file scope) start with dsrlp..

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Before calling any DSRLP functions, dsm_init() must have been called
  to initialize the memory pool.

  Here is how interface sequencing will work:
    At the beginning of the RLP session, call:
      dsrlp_init( sr_id, ver) with sr_id and version.
      dsrlp_setup_io( sr_id, tx_wm, rx_wm) with sr_id and the input
         and output watermarks.

   Every 20 ms to transmit a frame Mux laye calls,
     dsrlp_build_tx_frames( ..) - one call per sr_id;

   Every 20 ms after getting a frame from the physical layer, Mux layer calls,
     dsrlp_process_rx_frames(...) - one call per sr_id.

   At the end of the call:
     dsrlp_cleanup( sr_id) - will cleanup memory and data structures.

  Copyright (c) 1999 - 2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp.c_v   1.63   12 Feb 2003 17:30:14   kdevara  $
  $Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/src/dsrlp.c#3 $ $DateTime: 2009/02/01 22:28:06 $ $Author: ssaha $

when       who        what, where, why
--------   ---        -------------------------------------------------------
01/28/09   ss         Klocwork Fix.
04/08/08   sn         Fixed klockwork errors.
10/18/07   sn         Assigned zero to variable 'new_byte_value'.
05/11/06   squ        In dsrlp_build_tx_frames function, check if there is a
                      pending ack to send.
04/17/06   squ        In the 20 ms tx frame, added code to check ack_to_send 
                      flag in case a pending ack need to send.
08/30/05   gr         Fix to handle the race condition of simultanneous RLP
                      init and cleanup while dropping voice during SVD scenario
09/08/04   vr         Added functionality to notify the pzid hysteresis
                      engine about the first ppp data exchange
07/12/04   gr         Added functionality to update the contiguous tx and rx
                      idle frame count. Also modified the fill rate to be a
                      variable that can be set/retrieved via IOCTL. 
10/13/03   vas        Changed dsrlp_is_inited() to not return FALSE if ver 
                      is HDR. It returns the value based of HDR is initialized
                      hdrrlp_init is called from dsrlp_init.
09/18/03   kvd        Changed timetest Logging msg to MEDIUM from HIGH.
8/25/03    junz       Change the RLP stats Get and Clear functions to fix data
                      aborts due to invalid SR_ID
08/08/03   kvd        Made changes to dsrlp_log_timetest_throughput to log rlp
                      stats for all rlps (currently 1X & HDR)
07/10/03   vas        Added code to check for fixed sr id values for HDR.
05/21/03   vas        Made changes to support Multimode HDRRLP
04/21/03   ik/js      Modifications for quque defeaturization. 
04/10/03   sy         Added support for SVD algorithm to 
                      dsrlp_any_sig_to_send().
03/18/03   js         Removed the unnecessary return from 
                      dsrlp_process_rlp_blob(). 
03/07/03   js         Added RLP events. 
02/12/03   kvd        Added F3 messages indicating the rlp cleanup in 
                      dsrlp_cleanup after hdrrlp_cleanup since the F3 msg
                      inside hdrrlp_register_stream_callback is deleted now,
                      as it's inside INTLOCK.
01/31/03   sy         Changed funtion name dsrlp_next_turbo_pdu5_chain() to
                      dsrlp_get_next_dsm_item_chain().
01/24/02   js         Fixed a typographical error. 
12/18/02   atp        Implemented change in standard 707 for QOS.
12/18/02   kvd        Modified dsrlp_is_inited  to check for HDr version
12/02/02   snn/sy     dsrlp_insert_pdu5null_hdr_turbo function will insert Null 
                      header for the Turbo encoded PDUs.
10/15/02   js         Optimized f3 messages to print out RLP stream identifier. 
10/16/02   js         Fixed small typographical error
10/15/02   ak         Added code to call MM DATA control/hold function.
10/03/02   vas        Added function dsrlp_is_hdr()
09/17/02   ak         Added function dsrlp_is_inited() which returns whether
                      DS RLP is initialized. (chked in by js)
08/08/02   js         Fixed minor compile-time warning. 
08/07/02   vas        Move initialization of rx_sig_msg_q earlier in 
                      dsrlp_init
07/17/02   js         Added dsrlp_next_turbo_pdu5_chain() to be used by MUX
                      to resolve a turbo encoded MUX PDU5 chain of DSM items. 
07/15/02   ak         Added featurization under FEATURE_DATA_MM for new
                      architecture.
05/02/02   snn        Added a function dsrlp_eval_qos_blob and included snm.h. 
03/18/02   vas        Call hdrrlp_init at the end of dsrlp_init to ensure all
                      RLP initialization is completed before callbacks are 
                      registered with hdrrx/hdrpcp (called thru hdrrlp_init).
03/15/02   snn        Added function dsrlp_any_sig_to_send() for SVD.
02/28/02   vas        Changed dsrlp_init prototype to take rscb_index parameter
02/25/02   atp        Moved definition for QOS default from dsrlp.c to dsrlp.h.
02/20/02   snn        Added a message in check_sr_id function if the check fails.
                      This was necessary for sanity check in SVD builds. 
02/14/02   hcg        Moved #include of timetest.h for TIMETEST-based RLP throughput 
                      logging to prevent compilation problems.
02/04/02   ss         Made changes to use new DSM interface functions for
                      registering/handling memory events.
01/31/02   vr         Removed ASSERT is dsrlp_cleanup
01/31/02   atp        Added support for QOS.
01/28/02   sy         Changed the input to dsrlp_cleanup() from sr_id_index
                      to sr_id to make it consistent with with dsrlp_init().
12/13/01   snn        Fixed code to accept parameters greater than NAK_PER_ROUND
                      to be greater than {1,2,3}. It was always accepting parameters
                      greater than 1,2,3 but recent changes broke it. Fixed it back.
12/06/01   atp        Reworded error msg in dsrlp_build_tx_frames
12/03/01   snn        Added support for RLP options to return negotiated
                      parameters.
11/27/01   snn        Added support for modifying RLP3 nak policy.
10/31/01   vas        Changed DSRLP_FIXED_SR_ID to DSRLP_FIXED_SR_ID_INDEX in
                      function for logging RLP throughput. Change in timetest
                      throughput test to differentiate HDR & 1x call.
10/05/01   ak         SCRM function called in build_tx frames changed its 
                      name.
09/21/01   ak         Put a check in build_frames to make sure that dsm_items
                      were freed in txc.
09/16/01   vas        Used rscb_index instead of sr_id to call hdrrlp_cleanup
09/14/01   rama       SR_ID fix. Differentiate between SR_ID and RLP Instance.
                      Set a mapping between SR_ID and RLP Instance.
09/04/01   hcg        Added RLP throughput logging TIMETEST support.
08/17/01   vas        Added call to hdrrlp_init() from dsrlp_init()
08/02/01   ak         If resetting, get out build_tx_frames pronto.  Also
                      initialize build_tx_frames return struct up front.
07/02/01   ak         Always remove from reseq queue at end of process_rx_
                      frames.
06/20/01   vas        Added calls to hdrrlp_startup from dsrlp_startup & to
                      hdrrlp_cleanup from dsrlp_cleanup.
06/13/01   sy         Added extra enum for P2 mode to tell which one of the
                      FCH/DCCH is inactive.	This helps in calculating the
                      round trip time correctly.
05/27/01    snn       updated a comment.
05/24/01    hcg       Made changes to support HDR (1xEV).  Updates to support
                      new memory event callback registration mechanism.
05/21/01   sy         Making sure to call reset function if reset flag is set,
                      every 20msec.Added frame type DSRLPI_RESET to free all
                      frames before calling reset function.
05/11/01   ak         Further support for Control/Hold.  Some file cleanup.
                      Added checks to see if wmk ptr and rx queue ptrs were
                      NULL.  Remove limited # of items from reseq queue now.
04/10/01   lcc/ak     Added support for P2 mode.
04/01/01   ak         Only ask for SCRM now when lot of bytes to send.  Dont
                      ask to cancel when zero bytes left.
02/22/01   ak         Made an array which holds the RLP Logging ID for FCH
                      and SCH.
02/21/01   ak         Support for checking rx'ed new frames whose seq < V(S)
02/16/01   igt        New rlp BLOB processing: if during a call will force
                      a dsrlpi_reset() and also go through sync/ack procedures.
02/05/01   ak         Age NAK list only on MUX Null frames.  No longer age
                      NAKs on erasures.  Now remove ack'd frames from TXQ in
                      TX context.
01/15/01   igt        Added code to keep track of how much time spent to
                      sync by updating sync_delay and timing out.
12/07/00   ak         Do not age NAK timers on rexmits.  Only new data and
                      Idles.
12/06/00   ak         Changes to force FILL frames every 80 frames so that
                      other side gets our V(N) appropriately.
11/29/00   ak         Handle new DSRLP_RATE_ERASURE_SCH, due to LTU CRC
                      checks in MUX.
11/15/00   igt        remove calls to dsrlptxq_put_txq() and dsrlptxq_get_txq()
                      and changed the location of the call to dsrlptxq_init()
09/15/00   ak         In dsrlp_cleanup, set ver_ptr to NULL before releasing
                      txq and dsrlpdel arrays, to avoid end-of-call sync
                      problems with rxc.c
09/05/00   ttl        Enabled the data TX on SCH when the FCH is muted for data.
08/06/00   ak         Moved SCRM callback to after frames are xmitted, as
                      that is more efficient.  Also now process IDLEs after
                      all other new + delayed frames.
07/31/00   igt        If rtt !=0 but reset=1 in a BLOB, it will not sync/ack
                      Also added #ifndef MSM5000_IRAM_REV
07/24/00   ak         Added a call to SCRM when no data to send or lots of
                      data to send.
07/08/00   ak         Added function has_data_to_send() to indicate if new
                      data or rexmit data is awaiting transmission.
06/22/00   igt        Added dsrlp_stat_strings[], dsrlp_stats_name_ptr[]
                      and function dslrp_get_stats_at() to support AT$QCRL3D
                      and AT$QCRL3R. Also update "state" in dsrlp_get_stats().
06/08/00   ak         Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/05/00   ak         Added support for RLP now using a callback rx function
                      instead of a watermark.  Also now indicate to dsmgr
                      if any data frames received in last 20msec.
05/30/00   igt        added dsrlp_get_max_ms_nak_rounds_fwd() and _rev()
05/15/00   na/ak      Support for delayed frame detection.
05/10/00   ak         Improved mux erasure reporting.  support for sending
                      fill frames.
05/10/00   igt        dsrlp_clear_stats() now updates 'reset_timestamp' field
05/09/00   igt        update RLP_BLOB_used and curr_tx_nak_rounds stats.
04/28/00   igt        Set the resets statistic back to zero after call to
                      dsrlp_init(). Also update last_rtt, rx_blank_fr_cnt,
                      rx_rexmit_fr_cnt, rx_idle_fr_cnt, rx_rlp_erasures,
                      rx_mux_erasures, largest_contig_erasure, rx_null_fr_cnt
                      aborts, tx_rexmit_fr_cnt and tx_idle_fr_cnt statistics
04/19/00   ak         Modified erasure handling to account for mux erasures,
                      and also getting erasures in the INIT stages.  Always
                      age NAK list now.
04/13/00   ak         Added some debug when bad frames rxed in init stage.
                      Also keep track of mux-level erasures.
03/22/00   igt        fixed a compiler warning
03/21/00   igt        added stats support for SO33 screen
02/18/00   ak         Provide callbacks for when memory gets low in the
                      system, and take appropriate actions in the rx process
                      function.
02/15/00   igt        Added new BLOB processing and creation functions
01/14/00   ak         Added new table used in setting the 'used' field in
                      dsm items.  Fixed rexmit_data to use this table.
01/12/00   na         Fixed bug where while transmitting Idles, num_frames
                      was not being incremented.
01/11/00   ak         In dsrlp_init, added setting of nak params to default
                      values.  Due to RLP_BLOB in rlp3, (and static values
                      for rlp 1 & 2), these values are no longer reset in
                      SYNC/ACK handshake or during dsrlpi_reset.
01/10/00   na         RLP state was not being set to ESTABLISHED. Fixed this.
01/04/00   na         Fixed an extraneous ASSERT; Fixed problem with recog.
                      Erasures
08/09/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))

#ifdef TIMETEST
#include "timetest.h"

#ifdef TIMETEST_DSRLP_STATS 
#include "clk.h"
#endif /* TIMETEST_DSRLP_STATS */
#endif /* TIMETEST */

#include "ds.h"
#include "dsrlp.h"
#include "dsrlpi.h"
#include "dsrlpnak.h"
#include "dsrlptxq.h"
#include "dsrlprtx.h"
#include "dsrlprsq.h"
#include "dsrlpdel.h"
#include "assert.h"
#include "memory.h"
#include "msg.h"
#include "err.h"
#include "bit.h"
#include "dsm.h"
#include "ts.h"
#include "log.h"

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_DATA_MM
  #include "ds707_chs.h"
  #include "ds707_scrm.h"
  #include "ds707_cta_rlp.h"
#endif /* FEATURE_DATA_MM */

#include "dsrlp3.h"
#include "rlp_event_defs.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*
  Contains common code for each version of RLP.  Version-specific calls will
  be handled through a function pointer table, which will call into the
  version-specific RLP files.
*/

/*---------------------------------------------------------------------------
  This boolean, when TRUE, causes the Rx task to go ahead and prematurely
  age the oldest nak in the nak list, hopefully causing a hole to disappear
  and many blocked items to be sent up to the higher layer.  Set TRUE in
  the dsm callback dsrlp_mem_event when the Few Items Left memory event 
  occurs.  Does this for all active RLP sessions.
---------------------------------------------------------------------------*/
boolean     mem_warning_event = FALSE;

/*---------------------------------------------------------------------------
  When TRUE, causes a reset to all RLP's.  This is because the number of
  remaining dsm small items has reached a dangerous level, and so a reset
  is the only remedy.  Set to TRUE in dsm callback dsrlp_mem_event when the
  Do Not Exceed memory event occurs.
---------------------------------------------------------------------------*/
boolean     mem_critical_event    = FALSE;

#ifdef TIMETEST_DSRLP_STATS
/*---------------------------------------------------------------------------
  Callbacks for logging TIMETEST throughput data.
---------------------------------------------------------------------------*/
clk_cb_type dsrlp_timetest_cb;
#endif /* TIMETEST_DSRLP_STATS */

#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

/*===========================================================================
               G L O B A L  V A R I A B L E S
===========================================================================*/
#ifdef FEATURE_DS_RLP3
/*---------------------------------------------------------------------------
  Array that holds the log id's for rlp logging.
---------------------------------------------------------------------------*/
const log_rlp_phy_channel_enum_type dsrlp_log_id_table[RLP_MAX_REV_PDU] =
{
  LOG_RLP_FCH,     /* index 0 = FCH                                        */
  #ifdef FEATURE_IS2000_P2
  LOG_RLP_DCCH,    /* index 0 = DCCH                                       */
  #endif /* FEATURE_IS2000_P2 */
  LOG_RLP_SCH_0,   /* remainder are all on the SCH                         */
  LOG_RLP_SCH_0,
  LOG_RLP_SCH_0,
  LOG_RLP_SCH_0,
  LOG_RLP_SCH_0,
  LOG_RLP_SCH_0,
  LOG_RLP_SCH_0,
  LOG_RLP_SCH_0
};

/*---------------------------------------------------------------------------
  The following table contains the length in bytes of all FCH/DCCH/SCH1x 
  frames.
  The table is indexed by frame rate, type of service (primary/secondary) and
  rateset (where rateset1 is first entry, and rateset2 is second entry).
  The entries represent the number of bytes available for RLP excluding the
  type bits. The bits are rounded down to get number of bytes.
  This table is valid for MuxPDU1 and MuxPDU2.
  An entry of 0 means, this rate is not applicable.
---------------------------------------------------------------------------*/
const byte dsrlp_len_table[DSRLP_RATE_ERASURE][DSRLP_GENERIC_SRVC]
                          [DSRLP_MUX_PDU_3] =
{
  21, 33,     20, 32,                    /* Rate 1 -   full rate           */
  0,  0,      19, 30,                    /* Rate 7/8 - secondary only      */
  0,  0,      16, 26,                    /* Rate 3/4 - secondary only      */
  10, 15,     11, 17,                    /* Rate 1/2 - half rate           */
  0,  0,       0, 15,                    /* Rate 7/16- secondary only      */
  0,  0,       0, 12,                    /* Rate 3/8 - secondary only      */
  5,  6,       0,  8,                    /* Rate 1/4 - quarter rate        */
  0,  0,       0,  6,                    /* Rate 3/16- secondary only      */
  2,  2,       0,  4,                    /* Rate 1/8 - eigth rate          */
  0,  0,       0,  2,                    /* Rate 1/16- secondary only      */
};

/*---------------------------------------------------------------------------
  The following table contains the length in bytes of all FCH/DCCH frames.
  The table is indexed by frame rate, type of service (primary/secondary) and
  rateset (where rateset1 is first entry, and rateset2 is second entry).

  The entries represent the value that should go into the used field of the
  dsm item holding the frame info.

  An entry of 0 means, this rate is not applicable.
---------------------------------------------------------------------------*/
const byte dsrlp_used_len_table[DSRLP_RATE_ERASURE][DSRLP_GENERIC_SRVC]
                               [DSRLP_MUX_PDU_3] =
{
  22, 34,     21, 33,                    /* Rate 1 -   full rate           */
  0,  0,      19, 31,                    /* Rate 7/8 - secondary only      */
  0,  0,      16, 26,                    /* Rate 3/4 - secondary only      */
  10, 16,     11, 18,                    /* Rate 1/2 - half rate           */
  0,  0,       0, 16,                    /* Rate 7/16- secondary only      */
  0,  0,       0, 13,                    /* Rate 3/8 - secondary only      */
  5,  7,       0,  9,                    /* Rate 1/4 - quarter rate        */
  0,  0,       0,  7,                    /* Rate 3/16- secondary only      */
  2,  3,       0,  4,                    /* Rate 1/8 - eigth rate          */
  0,  0,       0,  3,                    /* Rate 1/16- secondary only      */
};

/*---------------------------------------------------------------------------
  
  RLP3 Naking policy is decided by 3 different variables. The variables are 
    a) Default Values                                    
    b) Desired Values
    c) Negotiated values
  
  Default Value: This value is used only while negotiating with the Base 
  station during Service Negotiation (i.e. processing/sending RLP BLOB).
  Initialize it to 3 & {1,2,3} for forward Naks and Naks/round.
  
  Desired Value: This is the value which the application wants to use in its
  current RLP3 data call. Initialize this to Default Value.
  
  Negotiated Value: This is the value negotiated by BS and MS. It is minimum
  of what BS sends, and MS default values. During power on initialization 
  these values are initialized to Maximum value. 
  
  In addition to the above values,we have: Working value which the RLP 
  implementation uses. The working value is decided by following logic:
   if (negotiation is done)
   {
       Negotiated Value = MIN (BS Value, MS Default Values); 
       Working value = MIN(Negotiated Value, Desired Value).       
   }
   else
   {
       Working value = Desired value.
   }
   
  To protect the RLP state, we change the variables only at end of the RLP 
  Tx. So, we use the variable update_state_var for doing it.
      
  default_nak_rounds_fwd: Def. NAK rounds for BS   
  default_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS]: 
                         Def. NAKs per round for BS
  
  default_nak_rounds_rev: Default NAK rounds for MS 
  default_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS]:
  
  negotiated_nak_rounds_fwd:  Def. NAK rounds for BS   
  negotiated_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS]: 
	 	 			  Def. NAKs per round for BS
  negotiated_nak_rounds_rev: Default NAK rounds for MS 
  negotiated_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];                                                              
---------------------------------------------------------------------------*/
struct 
{
  
  byte    default_nak_rounds_fwd;              
  byte    default_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];
  byte    default_nak_rounds_rev;              
  byte    default_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];
          
  byte    negotiated_nak_rounds_fwd;             
  byte    negotiated_naks_per_round_fwd[ DSRLP_MAX_NAK_ROUNDS];									   
  byte    negotiated_nak_rounds_rev; 
  byte    negotiated_naks_per_round_rev[ DSRLP_MAX_NAK_ROUNDS];

  byte    rscb_index;
  boolean update_state_var;
} dsrlp3_opt;

uint8 dsrlp_last_fill_nak_gap = DSRLPI_LAST_FILL_NAK_GAP;

#ifdef FEATURE_IS2000_REL_A
/*---------------------------------------------------------------------------
   Global variable that holds all QOS values - default, desired, rx from BS
---------------------------------------------------------------------------*/
dsrlp_qos_values_type dsrlp_qos_values;
#endif /* FEATURE_IS2000_REL_A */

#endif /* FEATURE_DS_RLP3 */

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
#ifdef FEATURE_DS_RLP3
extern dsrlpi_ver_const_type dsrlp3_constants;
extern void dsrlp3_opt_init(void);
extern void dsrlp_opt_update_state(void);
#ifdef FEATURE_IS2000_REL_A
extern void dsrlp_qos_init(void);
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_DS_RLP3 */

#ifdef TIMETEST_DSRLP_STATS
typedef struct{
  uint64    last_rx_bytes; /* last updated total rx bytes */
  uint64    last_tx_bytes; /* last updated total tx bytes */
} dsrlp_throuput_data_type;

#endif /* TIMETEST_DSRLP_STATS */

/*===========================================================================

FUNCTION       DSRLP_MEM_EVENT

DESCRIPTION    Called when the DSM pool reaches a registered memory level for 
               small items.

PARAMETERS
  dsm_mem_pool_id_enum type   pool_id  -  Memory pool ID
  dsm_mempool_level_enum_type event    -  Memory Level
  dsm_mem_op_enum_type        mem_op   -  Type of memory operation

RETURN VALUE   
  None

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
void dsrlp_mem_event
(
  dsm_mempool_id_enum_type pool_id,
  dsm_mem_level_enum_type level,
  dsm_mem_op_enum_type mem_op
)
{
  INTLOCK();
  if (level == DSM_MEM_LEVEL_LINK_LAYER_DNE)
  {
    mem_critical_event = TRUE;
  }
  else if (level == DSM_MEM_LEVEL_LINK_LAYER_FEW)
  {
    mem_warning_event = TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Sanity check.  We should not have received notification for anything
      but the Do Not Exceed and Few Items Left memory events.
    -----------------------------------------------------------------------*/
    ASSERT(0);
  }
  INTFREE();
}


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
void dsrlp_startup
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Reset the entire RLP control block array to 0.
    Initialize the NAK list free queue.
    Initialize the Transmit queues (saved frame arrays).
    Initialize the Retransmit free queue.
  -------------------------------------------------------------------------*/
  memset( dsrlpi_rscb_array, 0, sizeof( dsrlpi_rscb_array));
  dsrlpnak_init();

  dsrlprtx_init();

  /*-------------------------------------------------------------------------
     Set the RLP last fill NAK gap to the default value. 
     It can be changed via the IOCTL interface 
  -------------------------------------------------------------------------*/
  dsrlp_last_fill_nak_gap = DSRLPI_LAST_FILL_NAK_GAP;

#ifdef FEATURE_DS_RLP3
  dsrlpdel_init();
#ifdef FEATURE_IS2000_REL_A
  dsrlp_qos_init();
#endif /* FEATURE_IS2000_REL_A */
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

  dsrlp3_opt_init();
#ifdef TIMETEST_DSRLP_STATS
  /*-------------------------------------------------------------------------
    Register the clock callback for TIMETEST throughput profiling.
  -------------------------------------------------------------------------*/
  clk_def(&dsrlp_timetest_cb);
#endif /* TIMETEST_DSRLP_STATS */

  dsrlpi_sridmap_init();

} /* dsrlp_startup() */

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
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr = NULL;  /* rlp-specific control block */
  byte                                   i;  /* Loop counter               */
  event_rlp_payload_type rlp_event_payload;  /* payload for RLP evnt report*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id. Initialize the state info to zero.
    The cfg. info is not reset, because the watermarks might have been set
    before a call to dsrlp_init.
    Statistics are not reset, because we might be collecting statistics
    across multiple calls.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
  ASSERT((sr_id < DSRLPI_MAX_CDMA_SR_ID_VALUE) ||
      ((sr_id == DSRLP_HDR_SN_RLP_SR_ID_VALUE) && (ver == DSRLP_REV_HDR)) ||
      ((sr_id == DSRLPI_HDR_AN_AUTH_SR_ID_VALUE) && (ver == DSRLP_REV_HDR)))
#else /* FEATURE_DATA_MM */
  ASSERT(sr_id < DSRLPI_MAX_SP_SR_ID_VALUE);
#endif /* FEATURE_DATA_MM */

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);
  if( rscb_ptr->ver_ptr != NULL)
  {
    MSG_HIGH("dsrlp_init called multiple times RLP: %d", sr_id, 0, 0);
    return;
  }

  if (rscb_ptr->cleanup_for_going_null == TRUE)
  {  
     MSG_HIGH("RLP init called when in the middle of cleanup: %d", sr_id, 0, 0);
     return;
  }
  /*-------------------------------------------------------------------------
     Set the RLP last fill NAK gap to the default value. 
     It can be changed via the IOCTL interface 
  -------------------------------------------------------------------------*/
  dsrlp_last_fill_nak_gap = DSRLPI_LAST_FILL_NAK_GAP;

  /*---------------------------------------------------------------------
     Report RLP establishing event
  ---------------------------------------------------------------------*/
  rlp_event_payload.event_rlp_type = event_rlp_establishing;
  rlp_event_payload.info = rscb_ptr->sr_id;
  event_report_payload (EVENT_RLP, 
                        sizeof(event_rlp_payload_type),
                        &rlp_event_payload);

  memset( &(rscb_ptr->state), 0, sizeof( dsrlpi_state_info_type));
  INTLOCK();
  rscb_ptr->sr_id = sr_id;

  /*Set the mapping between the service provider's sr_id and the 
    index in the rscb array we use for that sr_id */
  dsrlpi_set_rscb_index_for_sr_id( sr_id, rscb_index);
  INTFREE();

  /*-------------------------------------------------------------------------
    Must initialize the nak_q embedded within the rscb_ptr structure.
    Also initialize for a transmit queue and a retransmit queue.
    These all must be done before the dsrlpi_reset below.
  -------------------------------------------------------------------------*/
  q_init(&rscb_ptr->state.nak_q);

#ifdef FEATURE_DS_RLP3
  rscb_ptr->state.del_fr_id = dsrlpdel_get_array();
  ASSERT(rscb_ptr->state.del_fr_id != DSRLPDEL_NO_ARRAY);
#endif

  /*-----------------------------------------------------------------------
    Only register the memory callbacks for non-HDR calls.
  -----------------------------------------------------------------------*/
  if (ver != DSRLP_REV_HDR)
  {
    dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL, 
                         DSM_MEM_LEVEL_LINK_LAYER_DNE,
                         DSM_MEM_OP_NEW, 
                         dsrlp_mem_event);

    dsm_reg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL, 
                         DSM_MEM_LEVEL_LINK_LAYER_FEW,
                         DSM_MEM_OP_NEW, 
                         dsrlp_mem_event);
  } /* ver != DSRLP_REV_HDR */

  /*-------------------------------------------------------------------------
    Initialize the Retransmit List and Resequencing Queue.
  -------------------------------------------------------------------------*/
  dsrlprtx_init_ptr(rscb_ptr);
  dsrlprsq_init( rscb_ptr);

  dsrlptxq_init( rscb_ptr);

  /*-------------------------------------------------------------------------
    Initialize the data structures for the given RLP session. This involves
    setting up the version specific constants.
  -------------------------------------------------------------------------*/
  dsrlpi_init_constants( rscb_ptr, ver);
  rscb_ptr->cfg.ver             = ver;
  rscb_ptr->cfg.octet_sequenced = octet_sequenced;

  /*------------------------------------------------------------------------
     Forward NAK parameter defaults.  These are done here since either
     they are a default (RLP 1 & 2) or negotiated via BLOB (RLP 3).  In
     either case, they are not changed by dsrlpi_reset.
  -------------------------------------------------------------------------*/
  rscb_ptr->state.nak_rounds_fwd = rscb_ptr->ver_ptr->desired_nak_rounds_fwd;
  if( rscb_ptr->state.nak_rounds_fwd > DSRLP_MAX_NAK_ROUNDS )
  {
    rscb_ptr->state.nak_rounds_fwd = DSRLP_MAX_NAK_ROUNDS;	/* Klockwork fix */
  }

  for (i = 0; i < rscb_ptr->state.nak_rounds_fwd; i++)
  {
    if(rscb_ptr->cfg.ver != DSRLP_REV_3) 
    {
      rscb_ptr->state.naks_per_round_fwd[i] =
                                rscb_ptr->ver_ptr->desired_naks_per_round_fwd[i];
    }
    else
    {
      rscb_ptr->state.naks_per_round_fwd[i] = 
                 MIN(rscb_ptr->ver_ptr->desired_naks_per_round_fwd[i],
                     dsrlp3_opt.default_naks_per_round_fwd[i]);         
    }
  }

  /*-------------------------------------------------------------------------
    By now the default parameters that the applications wants to use exist in
    dsrlp3_opt.default parameters. So the negotiated can be either max values
	i.e. {3,3,3} or lesser. We program the negotiated values with default 
	values.
	If negotiation occurs then negotiated may be different than default.
    If negotiation did not happen, still negotiated is same as default.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.negotiated_nak_rounds_fwd = dsrlp3_opt.default_nak_rounds_fwd;
  for (i=0; i< (dsrlp3_opt.negotiated_nak_rounds_fwd); i++)
  {
     dsrlp3_opt.negotiated_naks_per_round_fwd[i] = 
         dsrlp3_opt.default_naks_per_round_fwd[i] ;
  }

  /*------------------------------------------------------------------------
     Reverse NAK parameter defaults
  -------------------------------------------------------------------------*/
  rscb_ptr->state.nak_rounds_rev = rscb_ptr->ver_ptr->desired_nak_rounds_rev;
  if( rscb_ptr->state.nak_rounds_rev > DSRLP_MAX_NAK_ROUNDS )
  {
    rscb_ptr->state.nak_rounds_rev = DSRLP_MAX_NAK_ROUNDS;	/* Klockwork fix */
  }

  for (i = 0; i < rscb_ptr->state.nak_rounds_rev; i++)
  {
   if (rscb_ptr->cfg.ver != DSRLP_REV_3) 
    {
      rscb_ptr->state.naks_per_round_rev[i] =
                                        rscb_ptr->ver_ptr->desired_naks_per_round_rev[i];
    }
    else
    {
      rscb_ptr->state.naks_per_round_rev[i] = 
                MIN(rscb_ptr->ver_ptr->desired_naks_per_round_rev[i],
                    dsrlp3_opt.default_naks_per_round_rev[i]);        
    }
  }

  /*-------------------------------------------------------------------------
    By now the default parameters that the applications wants to use exist in
    dsrlp3_opt.default parameters. So the negotiated can be either default or 
    lesser. So, we program the negotiated values with default values. 
    If negotiation occurs then negotiated may be different than default.
    If negotiation did not happen, still negotiated is same as default.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.negotiated_nak_rounds_rev = dsrlp3_opt.default_nak_rounds_rev;
  for (i=0; i< (dsrlp3_opt.negotiated_nak_rounds_rev); i++)
  {
     dsrlp3_opt.negotiated_naks_per_round_rev[i] = 
         dsrlp3_opt.default_naks_per_round_rev[i];
  }

#ifdef FEATURE_HDR
#error code not present
#endif

  /*-------------------------------------------------------------------------
    Reset RLP. This will cleanup all the memory and initialiaze all the state
    variables.
  -------------------------------------------------------------------------*/
  dsrlpi_reset( rscb_ptr);


  /*-------------------------------------------------------------------------
    Set this back to 0 in here so it will not accumulate from call to call.
    This cannot be set to 0 in dsrlpi_reset() as that function is called
    even between init state changes say from S/A to Sync.
  -------------------------------------------------------------------------*/
  rscb_ptr->state.sync_delay  = 0;


  /*-------------------------------------------------------------------------
    Set the stats.resets counter back to zero. This ensures that this call
    to dsrlpi_reset() will not count as a reset in the  middle of a call,
    which the statistic keeps track of.
  -------------------------------------------------------------------------*/
  rscb_ptr->stats.resets = 0;

  /*-------------------------------------------------------------------------
    Clear the statistics structure. Safe to do this last.
  -------------------------------------------------------------------------*/
  dsrlp_clear_stats(rscb_index);

#ifdef TIMETEST_DSRLP_STATS
  /*-------------------------------------------------------------------------
    Register the clock callback for TIMETEST throughput profiling.  Gets
    called periodically every 500 ms.
  -------------------------------------------------------------------------*/
  clk_reg(&dsrlp_timetest_cb,
          dsrlp_log_timetest_throughput,
          500,
          500,
          TRUE);

#endif /* TIMETEST_DSRLP_STATS */

#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */
  
  MSG_HIGH("dsrlp_init done for rscb_index: %d, sr_id: %d",rscb_index, sr_id,0);

} /* dsrlp_init() */

/*===========================================================================

FUNCTION DSRLP_CLEANUP

DESCRIPTION
  This function cleansup the memory and resets the RLP state for a given
  rscb_index (corresponding to an sr_id). This should be called at the end 
  of a RLP session.

  Input Parameters:
    rscb_index: The index in the rscb array for the particular service. 

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
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  byte                           rscb_index; /* Index in rscb array        */ 
  dsrlp_stats_info_type   *stats_ptr;        /* local stats ptr            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------
    Get the index of in the rscb array corresponding to sr_id 
  --------------------------------------------------------------------*/
  rscb_index = dsrlpi_get_rscb_index_for_sr_id(sr_id);

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/
  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    MSG_HIGH("dsrlp_cleanup called with invalid sr_id: %d",sr_id,0,0);
    return;
  }

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  if( rscb_ptr->ver_ptr != NULL)
  {
  /*-----------------------------------------------------------------------
      Reset RLP. This will cleanup all the memory and initialiaze all the
      state variables.  INTLOCK This and the ver_ptr = NULL, so no data
      can get sent up to the RLP world by TXC as the call dies.
    -----------------------------------------------------------------------*/
    INTLOCK();
    rscb_ptr->cleanup_for_going_null = cleanup_for_going_null;
    
    dsrlpi_reset( rscb_ptr);

    /*-----------------------------------------------------------------------
      Reset the ver_ptr back to NULL, as this is used to tell if a particular
      rscb is being used/not used.

      Do this before releasing the txq and the delayed frame arrays, as
      the absence of those causes potential ERR_FATALs, if rxc still pushes
      data up to RLP as the call ends.
    -----------------------------------------------------------------------*/
    if ( rscb_ptr->cfg.ver != DSRLP_REV_HDR)
    {
      dsm_unreg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL, 
                             DSM_MEM_LEVEL_LINK_LAYER_DNE,
                             DSM_MEM_OP_NEW);

      dsm_unreg_mem_event_cb(DSM_DS_SMALL_ITEM_POOL, 
                             DSM_MEM_LEVEL_LINK_LAYER_FEW,
                             DSM_MEM_OP_NEW);
    } /* ver != DSRLP_REV_HDR */
#ifdef FEATURE_HDR
#error code not present
#endif /*FEATURE_HDR*/

    rscb_ptr->ver_ptr = NULL;
    INTFREE();


    MSG_HIGH( "RLP NAK Q Stats = %d %d %d",
              q_cnt( &dsrlpi_rscb_array[1].state.nak_q ),
              q_cnt( &dsrlpi_rscb_array[2].state.nak_q ),
              dsrlpnak_get_freenak_count());


    /*--------------------------------------------------------------------
	   Initialize the mapping between sr_id to invalid values 
    --------------------------------------------------------------------*/
    dsrlpi_sr_id_rscb_map[sr_id] = DSRLPI_INVALID_RSCB_INDEX;

    /*--------------------------------------------------------------------
      Now release the delayed frame arrays.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DS_RLP3
    dsrlpdel_put_array(rscb_ptr->state.del_fr_id);
#endif
  }

#ifdef TIMETEST_DSRLP_STATS
  clk_dereg(&dsrlp_timetest_cb);
#endif /* TIMETEST_DSRLP_STATS */

  /*--------------------------------------------------------------------
    Now clear the tx and rx contiguous idle frame count
  -----------------------------------------------------------------------*/
  stats_ptr     = &rscb_ptr->stats;
  stats_ptr->tx_contig_idle_fr_cnt = 0;
  stats_ptr->rx_contig_idle_fr_cnt = 0;

  rscb_ptr->cleanup_for_going_null = FALSE;
  MSG_HIGH( "dsrlp_cleanup complete",0, 0 ,0 );
} /* dsrlp_cleanup() */



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
  byte sr_id       /* Service ID */
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */
  byte                           rscb_index; /* Index in rscb array        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*--------------------------------------------------------------------
    Get the index of in the rscb array corresponding to sr_id
  --------------------------------------------------------------------*/
  rscb_index = dsrlpi_get_rscb_index_for_sr_id(sr_id);

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    rscb_index (corresponding to an sr_id).
  -------------------------------------------------------------------------*/
  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    MSG_HIGH("dsrlp_cleanup called with invalid sr_id",0,0,0);
    return;
  }

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);
  dsrlpi_reset( rscb_ptr );
} /* dsrlp_reset() */

#ifndef MSM5000_IRAM_FWD
#ifdef FEATURE_DS_RLP3
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

  The statistics updating in this function assume that it will be called
  once per 20ms period. Alternatively we can use a new interface that will
  tell whether a call corresponds to a Fundamental frame.

  The init state machine requires that MUX-level erasures are only sent to
  the RLP layer when the erasure occurs on the FCH/DCCH.  Erasures on the
  SCH's should not be sent up by the MUX.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_process_rx_frames
(
  dsrlp_fwd_frame_block_type *fwd_frame_block/* Ptr to rcvd RLP frames     */
)
{
  dsrlpi_sess_ctl_blk_type*rscb_ptr;         /* rlp-specific control block */
  dsrlp_fwd_frame_type    *fwd_frame_ptr;    /* info for one rlp frame     */
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table;/* Ptr table to RLP functions */
  dsrlpi_fr_det_enum_type  frame_type;       /* New/Rexmit/NAK/Idle/Init ? */
  byte                     frame_no;         /* Frame counter              */
  dsrlpi_curr_fr_info_type curr_fr_info;     /* Post processed frame data  */
  dsrlpi_curr_fr_info_type idle_fr_info;     /* Post processed idle frame  */
  boolean                  idle_fr_rxed;     /* Idle fr rxed this 20 msec? */
  dsrlpi_state_info_type *state_ptr;         /* local state ptr            */
  dsrlp_stats_info_type   *stats_ptr;        /* local stats ptr            */
  int                      i;                /* for...loop invariant       */
  boolean                  rx_data_frs;      /* received any data frs?     */
  static uint16            current_erasure=0;/* help count contig erasures */
  boolean                  age_nak_list = FALSE;/*should nak list be aged? */
  word                     curr_rlp_delay;   /* rlp_delay on fcn entry     */
  event_rlp_payload_type   rlp_event_payload;/* payload for RLP events     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to rxc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.
    
    This may also happen if the user has not setup both the rx_func_ptr
    and rx_queue.

	NOTE: Although the variable is named fwd_frame_block->sr_id, it is
	actually the RLP instance for this service (Currently only one RLP
	instance is supported)
  -------------------------------------------------------------------------*/
  rscb_ptr      = &( dsrlpi_rscb_array[fwd_frame_block->sr_id]);
  
  /*-------------------------------------------------------------------------
    Increase the 20 ms counter that counts # fundamental frames. This assumes
    that this function is called once per 20ms period and not multiple times.
  -------------------------------------------------------------------------*/
  rscb_ptr->stats.rx_20ms_fr_cnt++;

  if ((rscb_ptr->ver_ptr == NULL) ||
      (rscb_ptr->cfg.rx_func_ptr == NULL) ||
      (rscb_ptr->cfg.rx_q_ptr == NULL)
     )
  {
    for (i = 0; i < fwd_frame_block->num_rlp_frames; i++)
    {
      dsm_free_packet(&(fwd_frame_block->fwd_arr[i].fr_item_ptr));
    }
    mem_critical_event = FALSE;
    mem_warning_event = FALSE;
    return;
  }

  /*-------------------------------------------------------------------------
    If PPP data was received after dormancy and notification was requested,
    notify the required module (PZID hysterisis).
  -------------------------------------------------------------------------*/
  if((rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr != NULL) &&
     (rscb_ptr->cfg.ppp_data_transfer_after_dormancy == TRUE))
  {
    rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr(DSRLP_FIXED_SR_ID_INDEX);
  }
      
  /*-------------------------------------------------------------------------
    Indicate that no IDLE frames were rxed.  If an Idle frame is rxed this
    20 msec, then it is processed last.  This helps to account for delayed
    frames on the SCH.
  -------------------------------------------------------------------------*/
  idle_fr_rxed = FALSE;

  /*-------------------------------------------------------------------------
    In order to support dormant mode, need to know if any data/ctl frames
    were received this 20msec period.  The following boolean will be set
    to TRUE if any data frames are received.
  -------------------------------------------------------------------------*/
  rx_data_frs = FALSE;

  /*-------------------------------------------------------------------------
    Check the memory event variables. If they are set, then some action
    may be required, over all RLP sessions.  Normal processing can then
    proceed.  In the case of a reset, the received frames will be dumped
    as bad frames, unless they happen to be sync frames.
  -------------------------------------------------------------------------*/
  if (mem_critical_event == TRUE)
  {
    /*-----------------------------------------------------------------------
      Small items are in short supply.  Reset all RLP instances; otherwise
      data will start to be dropped on the floor, eventually resulting in
      rlp resets anyways.
    -----------------------------------------------------------------------*/
    for(i = 0; i < DSRLPI_MAX_RSCB_ELEMENTS; i++)
    {
      if (dsrlpi_rscb_array[i].ver_ptr != 0)
      {
        MSG_HIGH("mem critical - resetting RLP %d",i,0,0);
        dsrlpi_reset(&(dsrlpi_rscb_array[i]));
       /*--------------------------------------------------------------------
          Report RLP reset event
       --------------------------------------------------------------------*/
       rlp_event_payload.event_rlp_type = event_rlp_reset;
       rlp_event_payload.info = rscb_ptr->sr_id;
       event_report_payload (EVENT_RLP, 
                             sizeof(event_rlp_payload_type),
                             &rlp_event_payload);
  
      }
    }
    mem_critical_event = FALSE;
  }
  else if (mem_warning_event == TRUE)
  {
#ifdef FEATURE_DS_RLP3_OBSOLETE
#error code not present
#endif /* FEATURE_DS_RLP3_OBSOLETE */
    mem_warning_event = FALSE;
  }

  /*-------------------------------------------------------------------------
    Using the sr_id, get the session control block and fcn_ptr_table.
  -------------------------------------------------------------------------*/
  state_ptr     = &rscb_ptr->state;
  stats_ptr     = &rscb_ptr->stats;
  fcn_ptr_table = rscb_ptr->ver_ptr->fcn_ptr_table;
  ASSERT( fcn_ptr_table != NULL);

  /*-------------------------------------------------------------------------
    Record the current rlp_delay (the RTT).  At the end of this function,
    make sure that the rlp_delay has only incremented by one (max).  This
    covers the case where we receive SYNC frames on both FCH & DCCH in a
    VP2 call.
  -------------------------------------------------------------------------*/
  curr_rlp_delay = state_ptr->rlp_delay;

  /*-------------------------------------------------------------------------
    Check if the maximum time to achieve synchronization has been exceeded.
    Discard the received frame(s), end call  and return, if so.
  -------------------------------------------------------------------------*/
  if(((state_ptr->state == DSRLPI_SYNC_STATE)    ||
      (state_ptr->state == DSRLPI_SYNC_ACK_STATE)||
      (state_ptr->state == DSRLPI_ACK_STATE))
     &&
     (++(state_ptr->sync_delay) == DSRLPI_MAX_RTT_SETUP))
  {
    ERR("SYNC state Timeout", 0, 0, 0);
    /*-----------------------------------------------------------------------
      Free the buffers for all the rx'ed RLP frames, reset RLP and return.
    -----------------------------------------------------------------------*/
    for(i = 0; i < fwd_frame_block->num_rlp_frames; i++)
    {
      dsm_free_packet(&(fwd_frame_block->fwd_arr[i].fr_item_ptr));
    }

    /*-----------------------------------------------------------------------
      Reset RLP. This will cleanup all the memory and initialiaze all state
      variables. This also updates the "resets" stat.
    -----------------------------------------------------------------------*/
    dsrlpi_reset( rscb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->sr_id;
    event_report_payload (EVENT_RLP, 
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);

   

    /*-----------------------------------------------------------------------
      Call this function in dsmgr.c to end the current call
    -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_MM
    ds707_cta_rlp_sync_timeout();
#else
    ds_end_call_upon_sync_timeout();
#endif /* FEATURE_DATA_MM */

    /*-----------------------------------------------------------------------
      Set the counter back to 0 as we now have reached the limit and the
      call will be dropped
    -----------------------------------------------------------------------*/
    state_ptr->sync_delay = 0;

    /*-----------------------------------------------------------------------
      Now just return, nothing else to do here
    -----------------------------------------------------------------------*/
    return;
  }

  /*-------------------------------------------------------------------------
    Within a frame block we can receive multiple RLP frames.  Loop
    until all frames have been processed
  -------------------------------------------------------------------------*/
  for (frame_no=0;
       frame_no < fwd_frame_block->num_rlp_frames;
       frame_no++)
  {
    /*-----------------------------------------------------------------------
      Get a frame.  Fundamental channel frames are at index 0,
      while supplemental channel frames are at index > 0.
      Initialize curr_fr_info for this frame.
    -----------------------------------------------------------------------*/
    fwd_frame_ptr = &( fwd_frame_block->fwd_arr[frame_no]);
    memset( &curr_fr_info, 0, sizeof( curr_fr_info));

    /*-----------------------------------------------------------------------
      If the frame is valid (determined by conditional above or by
      MUX layer), then determine its frame type using the RLP
      specific frame determination function.
    -----------------------------------------------------------------------*/
    if( (fwd_frame_ptr->fr_rate != DSRLP_RATE_ERASURE) &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_BLANK)   &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_ERASURE_SCH) &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_DCCH_DTX)    &&
        (fwd_frame_ptr->fr_rate != DSRLP_RATE_INACTIVE))
    {
      frame_type= fcn_ptr_table->rx_frame_det( rscb_ptr, fwd_frame_ptr,
                                               &curr_fr_info);

      /*---------------------------------------------------------------------
        If RLP is not established, see if the received frame was Idle or New.
        In this case RLP state moves from ACK or RT_MEAS to ESTABLISHED.
        A frame other than IDLE or New will be discarded, and state reset
        to sending SYNCs.
      ---------------------------------------------------------------------*/
      if ( rscb_ptr->state.state != DSRLPI_ESTABLISHED_STATE)
      {
        /*-------------------------------------------------------------------
          Do not go dormant if RLP has not synced up
        -------------------------------------------------------------------*/
        rx_data_frs = TRUE;

        if ((state_ptr->state == DSRLPI_ACK_STATE) ||
            (state_ptr->state == DSRLPI_RT_MEAS_STATE))
        {
          /*-----------------------------------------------------------------
            In this state, either will transition to the est'd state, or
            need to increment the rlp_delay count (in the case of erasures.
            Otherwise, down below, the frame will be passed to rx_init
            and processed accordingly.
          -----------------------------------------------------------------*/
          if (frame_type == DSRLPI_RLP_ERASURE)
          {
            state_ptr->rlp_delay++;
          }
          else if (frame_type != DSRLPI_INIT)
          {
            state_ptr->state= DSRLPI_ESTABLISHED_STATE;

            /*---------------------------------------------------------------
              Set this back to zero now that we are out of all 3 init states
            ---------------------------------------------------------------*/
            state_ptr->sync_delay = 0;

            MSG_HIGH( "RLP (sr_id=%d) SYNCED UP with RTT=%d",
                      rscb_ptr->sr_id, state_ptr->rlp_delay, 0);
            stats_ptr->last_rtt = state_ptr->rlp_delay;
          }
        }
        else if( frame_type != DSRLPI_INIT)
        {
          /*-----------------------------------------------------------------
            in the sync or sync-ack state, and so are expecting some kind of
            init frame.
          -----------------------------------------------------------------*/
          MSG_HIGH( "Got frame_type=%d, when state=%d. Discarding frame. RLP:%d",
                    frame_type, state_ptr->state, rscb_ptr->sr_id);
          /*-----------------------------------------------------------------
            If the frame type is an RLP erasure or a NULL, then let the
            handling below take care of it, and do not reset RLP.  Otherwise,
            just free the packet and return.  In the latter case, since the
            frame was not what was expected, go ahead and reset RLP, so that
            RLP returns to the SYNC state (with all variables set correctly).
          -----------------------------------------------------------------*/
          if ((frame_type != DSRLPI_RLP_ERASURE) &&
              (frame_type != DSRLPI_NULL))
          {
            MSG_HIGH("RLP: %d",rscb_ptr->sr_id,0,0);
            MSG_HIGH("Used = 0x%x Bytes = 0x%x 0x%x",
                      dsrlpi_ctl_used,dsrlpi_ctl_byte1,dsrlpi_ctl_byte2);
            if (state_ptr->state != DSRLPI_SYNC_STATE)
            {
              rscb_ptr->state.reset_rlp = TRUE;           /* Set the reset flag */
            }
            dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
            frame_type = DSRLPI_RESET;
          }
        }
      } /* if RLP not ESTABLISHED */
    }   /* if not ERASURE */
    else if (fwd_frame_ptr->fr_rate == DSRLP_RATE_ERASURE_SCH)
    {
      /*---------------------------------------------------------------------
        erasure on the SCH... discard frame and do no further processing.
        Do not call this an MUX_ERASURE, as that implies a MUX_ERASURE on
        the FCH.

        Free dsm item for safety.
      ---------------------------------------------------------------------*/
      dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
      frame_type = DSRLPI_DONE;
    }
    else if (fwd_frame_ptr->fr_rate == DSRLP_RATE_INACTIVE)   
    {
      /*---------------------------------------------------------------------
       blank item on the inactive channel, discard frame and do no further 
       processing. Free dsm item for safety.
      ---------------------------------------------------------------------*/
      dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
      frame_type = DSRLPI_DONE;
    }
    else
    {
      /*---------------------------------------------------------------------
        Received either an erasure or blank from MUX.  Specify frame type
        as such, so correctly processed below.  Increment the rlp_delay
        if in that state.
      ---------------------------------------------------------------------*/
      if ((fwd_frame_ptr->fr_rate == DSRLP_RATE_BLANK) ||
          (fwd_frame_ptr->fr_rate == DSRLP_RATE_DCCH_DTX))
      {
        stats_ptr->rx_blank_fr_cnt++;
      }
      if ((state_ptr->state == DSRLPI_ACK_STATE) ||
          (state_ptr->state == DSRLPI_RT_MEAS_STATE))
      {
        state_ptr->rlp_delay++;
      }
      if (fwd_frame_ptr->fr_rate == DSRLP_RATE_DCCH_DTX)
      {
        frame_type = DSRLPI_DCCH_DTX;
      }
      else
      {
        frame_type = DSRLPI_MUX_ERASURE;
      }
    }

    /*-----------------------------------------------------------------------
      Start processing the frame.  Since this can be an iterative
      process per frame, we keep looping til we resolve being done
      with this frame.  For example, a new data frame could have a
      CRC error, and then it turns into a ERASURE...
    -----------------------------------------------------------------------*/
    while (frame_type != DSRLPI_DONE)
    {
      if (frame_type != DSRLPI_MUX_ERASURE)
      {
        current_erasure = 0;
      }/* if */

      switch (frame_type)
      {
        case DSRLPI_NEW:
          rx_data_frs = TRUE;
          frame_type= fcn_ptr_table->rx_new_data( rscb_ptr,
                                                  &(fwd_frame_ptr->fr_item_ptr),
                                                  &curr_fr_info);
          /*---------------------------------------------------------------
            Increase the count of new frames received. Ignore rare case it
            is not immediately DSRLPI_DONE.
          ---------------------------------------------------------------*/
          stats_ptr->rx_new_data_fr_cnt++;
          rscb_ptr->state.last_fill_nak_cnt++;
          age_nak_list = TRUE;
          break;

        case DSRLPI_REXMIT:
          rx_data_frs = TRUE;
          frame_type= fcn_ptr_table->rx_rexmit( rscb_ptr, fwd_frame_ptr,
                                                &curr_fr_info);
          /*---------------------------------------------------------------
            Increase the count of re-transmitted  frames received.
          ---------------------------------------------------------------*/
          stats_ptr->rx_rexmit_fr_cnt++;
          break;

        case DSRLPI_INIT:
          rx_data_frs = TRUE;
          frame_type= fcn_ptr_table->rx_init( rscb_ptr, &curr_fr_info);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          break;

        case DSRLPI_IDLE:
          /*-----------------------------------------------------------------
            Increase the count of idle frames received and delete the dsm
            item.  Save the frame info, as idle frames are always processed
            last.  This is to account for potential delayed frames.
          -----------------------------------------------------------------*/
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          stats_ptr->rx_idle_fr_cnt++;

          idle_fr_rxed = TRUE;
          idle_fr_info = curr_fr_info;
          age_nak_list = TRUE;
          frame_type = DSRLPI_DONE;
          break;

        case DSRLPI_RLP_ERASURE:
          frame_type = fcn_ptr_table->rx_rlp_erasure( rscb_ptr, fwd_frame_ptr);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          rscb_ptr->stats.rx_rlp_erasures++;
          break;

        case DSRLPI_MUX_ERASURE:
          frame_type = fcn_ptr_table->rx_mux_erasure( rscb_ptr, fwd_frame_ptr);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          rscb_ptr->stats.rx_mux_erasures++;

          if (rscb_ptr->stats.largest_contig_erasure < ++current_erasure)
          {
            rscb_ptr->stats.largest_contig_erasure = current_erasure;
          }
          break;

        case DSRLPI_NULL:
          frame_type = DSRLPI_DONE;
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          rscb_ptr->stats.rx_null_fr_cnt++;
          age_nak_list = TRUE;
          break;

        case DSRLPI_NAK:
          rx_data_frs = TRUE;
          frame_type = DSRLPI_DONE;
          break;

        case DSRLPI_FILL:
          frame_type = DSRLPI_DONE;
          break;
  
        case DSRLPI_RESET:
          while((++frame_no) < fwd_frame_block->num_rlp_frames)
          {
            dsm_free_packet(&(fwd_frame_block->fwd_arr[frame_no].fr_item_ptr));
          }
          frame_type = DSRLPI_DONE;
          break;
        
        case DSRLPI_DCCH_DTX:
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          age_nak_list = TRUE;
          frame_type = DSRLPI_DONE;
          break;
 
        default:
          ASSERT( 0);
      }

    } /* while (frame_type != DSRLPI_DONE) */

  } /* for */

  /*-------------------------------------------------------------------------
    Clear out the delayed frames (if any) from the previous 20 msec
  -------------------------------------------------------------------------*/
  dsrlpdel_remove(rscb_ptr);

  /*-------------------------------------------------------------------------
    If an idle was received, process it.
  -------------------------------------------------------------------------*/
  if (idle_fr_rxed == TRUE)
  {
    (void)fcn_ptr_table->rx_idle(rscb_ptr, &idle_fr_info);
  }

  if (((rscb_ptr->stats.rx_20ms_fr_cnt % 100) == 0) &&
      (rscb_ptr->stats.rx_20ms_fr_cnt > 0)          &&
      (rscb_ptr->state.mux_erasure_cnt > 0))
  {
    MSG_HIGH("rx'd %d erases & blanks fr mux in 100 fr times RLP:%d",
             rscb_ptr->state.mux_erasure_cnt,rscb_ptr->sr_id,0);
    rscb_ptr->state.mux_erasure_cnt = 0;
  }

  /*-------------------------------------------------------------------------
    Age the nak list if new data/idle/mux erasures were received. V(N) could
    have changed when the NAK list is aged, because a NAK might have been
    aborted. In this case, the resequencing queue needs to be cleared.
  -------------------------------------------------------------------------*/
  if (age_nak_list == TRUE)
  {
    if ( dsrlpnak_age_list( rscb_ptr) == TRUE)
    {
      MSG_HIGH(" NAK ABORT new V(N)=%x, V(R)=%x RLP:%d",
                 rscb_ptr->state.needed_seq_num,
                 rscb_ptr->state.expected_seq_num,
                 rscb_ptr->sr_id);
      rscb_ptr->stats.aborts++;
      /*---------------------------------------------------------------------
        Report the RLP NAK abort event 
      ---------------------------------------------------------------------*/
      rlp_event_payload.event_rlp_type = event_rlp_nak_abort;
      rlp_event_payload.info = rscb_ptr->sr_id;
      event_report_payload (EVENT_RLP, 
                            sizeof(event_rlp_payload_type),
                            &rlp_event_payload);
      
    }
  }

  if (rscb_ptr->state.old_frames >= 5)
  {
    ERR("Reset due to too many old frames from BS",0,0,0);
    rscb_ptr->state.reset_rlp = TRUE;          /*Set the reset flag*/
  }

  /*-------------------------------------------------------------------------
    Make sure that rlp_delay hasn't been over-incremented.  If so, set
    rlp_delay such that it is only incremented by 1.
  -------------------------------------------------------------------------*/
  if (state_ptr->rlp_delay > (curr_rlp_delay + 1))
  {
    state_ptr->rlp_delay = curr_rlp_delay + 1;
  }

  /*-------------------------------------------------------------------------
   Check if reset flag is set by tx, if it does then reset rlp else continue 
   processing the rx.  
  -------------------------------------------------------------------------*/
  if (rscb_ptr->state.reset_rlp == TRUE)
  {
    dsrlpi_reset(rscb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->sr_id;
    event_report_payload (EVENT_RLP, 
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);
    return;
  }

  /*-------------------------------------------------------------------------
    See if anything left to remove from resequencing queue.
  -------------------------------------------------------------------------*/
  dsrlprsq_remove(rscb_ptr, DSRLPI_MAX_CHAINS_TO_REMOVE);

  /*-------------------------------------------------------------------------
    Tell DSMGR if any data frames were received, so that dormant mode
    processing can occur.
  -------------------------------------------------------------------------*/
  ds_rlp_data_rxed(rx_data_frs);

  /*-------------------------------------------------------------------------
    Update the stats with the count of contiguous IDLE frames received. 
    Reset the counter if a valid data/ctl frame is received
  -------------------------------------------------------------------------*/
  if(rx_data_frs == TRUE)
  {
    stats_ptr->rx_contig_idle_fr_cnt = 0;
  }
  else
  {
    stats_ptr->rx_contig_idle_fr_cnt++; 
  }

#if defined(FEATURE_IS2000_CHS) && defined(FEATURE_DATA_MM)
  ds707_chs_rxed_data(rx_data_frs);
#endif

}  /* dsrlp_process_rx_frames() */
#endif /* FEATURE_DS_RLP3 */
#endif /* MSM5000_IRAM_FWD */


#ifndef MSM5000_IRAM_REV
#ifdef FEATURE_DS_RLP3
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
  function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_build_tx_frames
(
  dsrlp_rev_frame_block_type *rev_fr_blk     /* Ptr to frames to trasmit   */
)
{
  byte    fr_num;                            /* Ctr to loop through frames */
  dsrlp_rev_frame_type  *rev_fr_ptr;         /* Ptr to a specific tx frame */
  boolean frame_built = FALSE;               /* frame has been built       */
  struct dsrlpi_fcn_ptr_table *fcn_ptr_table;/* Ptr table to RLP functions */
  dsrlpi_sess_ctl_blk_type*rscb_ptr;         /* rlp-specific control block */
  dsrlp_stats_info_type    *stats_ptr;       /* local stats pointer        */
  boolean non_idle_blank = TRUE;             /* frames wasnt Idle or Blank */
  boolean missed_txc = FALSE;
  boolean tx_data_frs = FALSE;               /* Sent relevant data?        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set the vars in case we return immediately.
  -------------------------------------------------------------------------*/
  rev_fr_blk->num_frames = 0;
  for(fr_num = 0; fr_num< rev_fr_blk->max_frames; fr_num++)
  {
    if ((rev_fr_blk->rev_arr[fr_num].free_dsm_pkt == TRUE) &&
        (rev_fr_blk->rev_arr[fr_num].fr_item_ptr  != NULL))
    {
      missed_txc = TRUE;
      dsm_free_packet(&(rev_fr_blk->rev_arr[fr_num].fr_item_ptr));
    }
    else
    {
    rev_fr_blk->rev_arr[fr_num].fr_item_ptr  = NULL;
    }
    rev_fr_blk->rev_arr[fr_num].free_dsm_pkt = TRUE;
    rev_fr_blk->rev_arr[fr_num].fr_rate      = DSRLP_RATE_BLANK;
  }
  if (missed_txc == TRUE)
  {
    ERR("RLP didn't build frames in time?",0,0,0);
  }

  /*-------------------------------------------------------------------------
    The call may have ended, but the message has not yet gotten to txc.
    So go ahead and check to see if the ver_ptr is null, and if so, then
    dump the rxc frames.

	NOTE: Although the variable is named rev_fr_blk->sr_id, it is actually 
	the RLP instance for this service (currently only one RLP instance is
	supported). When actaully building the individual frames, the correct
	sr_id value stored in rscb_ptr is used for the mux header.
  -------------------------------------------------------------------------*/
  rscb_ptr      = &( dsrlpi_rscb_array[ rev_fr_blk->sr_id]);
  if (rscb_ptr->ver_ptr == NULL)
  {
    return;
  }

  if (rscb_ptr->state.reset_active == TRUE)
  {
    MSG_HIGH("RLP RESET during TX, get out RLP:%d",rscb_ptr->sr_id,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Using the sr_id, get the session control block and fcn_ptr_table.
  -------------------------------------------------------------------------*/
  stats_ptr     = &rscb_ptr->stats;
  fcn_ptr_table = rscb_ptr->ver_ptr->fcn_ptr_table;
  ASSERT( fcn_ptr_table != NULL);

  /*-------------------------------------------------------------------------
    Increase the counter of 20ms periods. This assumes that this function
    will only be called once per 20ms period, or we can use a new interface
    that lets us tell if this is not the case.
  -------------------------------------------------------------------------*/
  stats_ptr->tx_20ms_fr_cnt++;

  /*-------------------------------------------------------------------------
    Make sure that the tx watermark has been set up.  If not, then just
    punt out of this function.  This means that RLP will take a little 
    longer in sync'ing up, etc... but since RLP has no data to transmit,
    it has no effect.  Mux will react by sending 1/8th rate NULLs.
  -------------------------------------------------------------------------*/
  if (rscb_ptr->cfg.tx_wm_ptr == NULL)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Before building any frames, first go ahead and cleanup the saved frames
    queue, of any frames that are acked by the BS.
  -------------------------------------------------------------------------*/
  dsrlptxq_remove_acked(rscb_ptr,rscb_ptr->state.peer_vn);

  /*-------------------------------------------------------------------------
    Go through all the frames in the frame array and try to build them.
  -------------------------------------------------------------------------*/
  for( fr_num =0; fr_num< rev_fr_blk->max_frames; fr_num++)
  {
    /*-----------------------------------------------------------------------
      Get a pointer to the specific frame that is going to be build.
    -----------------------------------------------------------------------*/
    rev_fr_ptr               = &( rev_fr_blk->rev_arr[fr_num]);
    rev_fr_ptr->idle_frame   = FALSE;
    rev_fr_ptr->free_dsm_pkt = TRUE;
    rev_fr_ptr->fr_item_ptr  = NULL;
    frame_built              = FALSE;

    /*-----------------------------------------------------------------------
      If the state is not established, then RLP needs to send SYNC/ACK frames
      While RLP is sending SYNC/ACK frames, it cannot send any other frames,
      so just exit out of loop.
    -----------------------------------------------------------------------*/
    if ( ( ((rscb_ptr->state).state   != DSRLPI_ESTABLISHED_STATE) &&
           ((rscb_ptr->state).state   != DSRLPI_RT_MEAS_STATE)        
         ) || ((rscb_ptr->state).ack_to_send == TRUE) 
       )
    {
      if( (rev_fr_ptr->fr_cat_mask) &  DSRLP_CAT_CTL_MASK)
      {
        ASSERT( fcn_ptr_table->tx_init != NULL);
        frame_built= fcn_ptr_table->tx_init( rscb_ptr, rev_fr_ptr);
        if ( frame_built == TRUE)
        {
          tx_data_frs = TRUE;
          fr_num++;
        }
        break;
      }
    }

    /*-----------------------------------------------------------------------
      If this frame has been blanked out.
    -----------------------------------------------------------------------*/
    if( (frame_built == FALSE) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_BLANK_MASK))
    {
      //frame_built= TRUE;
      continue;
    }

    /*-----------------------------------------------------------------------
      For P2 mode, it is possible that FCH is BLANK'd, but DCCH is not.
      Do this check just to be sure code does not run amok.  If true,
      then the check for BLANK_MASK will catch that case.
    -----------------------------------------------------------------------*/
    if ( ((rscb_ptr->state).state   != DSRLPI_ESTABLISHED_STATE) &&
         ((rscb_ptr->state).state   != DSRLPI_RT_MEAS_STATE))
    {
      ERR("Did not send SYNC/ACK or FCH/DCCH",0,0,0);
      break;
    }

    /*-----------------------------------------------------------------------
      If there were NAKs waiting to be sent, send a NAK.
    -----------------------------------------------------------------------*/
    if( (frame_built == FALSE) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_CTL_MASK) &&
        ((rscb_ptr->state).naks_to_send == TRUE))
    {
      ASSERT( fcn_ptr_table->tx_nak != NULL);
      frame_built= fcn_ptr_table->tx_nak( rscb_ptr, rev_fr_ptr);

      if (frame_built == TRUE)
      {
        tx_data_frs = TRUE;
        rscb_ptr->state.last_fill_nak_cnt = 0;
      }
    }

    /*-----------------------------------------------------------------------
      Send a fill frame if the other side has not been updated with our
      V(N) in a while.  Do this after the NAK, as that can also sends our
      V(N) to the other side.
    -----------------------------------------------------------------------*/
    if ((frame_built == FALSE) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_IDLE_MASK) &&
        (rscb_ptr->state.last_fill_nak_cnt >= dsrlp_last_fill_nak_gap))
    {
      ASSERT( fcn_ptr_table->tx_fill != NULL);
      frame_built= fcn_ptr_table->tx_fill( rscb_ptr, rev_fr_ptr);

      if (frame_built == TRUE)
      {
        rscb_ptr->state.last_fill_nak_cnt = 0;
        MSG_MED("Forced a FILL frame. RLP:%d",rscb_ptr->sr_id,0,0);
      }
      else
      {
        ERR("Could not construct FILL frame RLP:%d", rscb_ptr->sr_id,0, 0);
      }
    }

    /*-----------------------------------------------------------------------
      If there are retransmit frames queued, then build a retransmitted data
      frame.
    -----------------------------------------------------------------------*/
    if( ( frame_built == FALSE) &&
        ( DSRLPRTX_HAS_REXMITS(rscb_ptr)) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_DATA_MASK))
    {
      ASSERT( fcn_ptr_table->tx_rexmit != NULL);
      frame_built= fcn_ptr_table->tx_rexmit( rscb_ptr, rev_fr_ptr);
      stats_ptr->tx_rexmit_fr_cnt++;
      tx_data_frs = TRUE;
    }

    /*-----------------------------------------------------------------------
      If there are data available in the transmit watermark, then build a
      new frame.
    -----------------------------------------------------------------------*/
    if( (frame_built == FALSE) &&
        ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_DATA_MASK) &&
        ( ((rscb_ptr->state).tx_item_ptr != NULL) ||
          (((rscb_ptr->cfg).tx_wm_ptr    != NULL) &&
           ((rscb_ptr->cfg).tx_wm_ptr->current_cnt != 0)) ))
    {
      ASSERT( fcn_ptr_table->tx_new_data != NULL);
      frame_built= fcn_ptr_table->tx_new_data( rscb_ptr, rev_fr_ptr);

      /*---------------------------------------------------------------------
        Set the flag if this is the first new packet transmitted out of 
        dormancy.
      ---------------------------------------------------------------------*/
      rscb_ptr->cfg.ppp_data_transfer_after_dormancy = TRUE;

      /*---------------------------------------------------------------------
        Increase the counter of new frames transmitted. Ignore rare case
        frame_built = FALSE.
      ---------------------------------------------------------------------*/
      stats_ptr->tx_new_data_fr_cnt++;
      tx_data_frs = TRUE;
    }

    /*-----------------------------------------------------------------------
      If no frame was built, see if an idle frame can be sent. Otherwise send
      a blank indication to the Mux layer.
    -----------------------------------------------------------------------*/
    if( frame_built == FALSE)
    {
      /*---------------------------------------------------------------------
        If RLP could build an idle, the tx_idle() function will return TRUE.
        In this case, set num_frames to include the idle. If the tx_idle()
        function returned FALSE, then num_frames is not incremented, and
        the Mux layer sends a BLANK frame
      ---------------------------------------------------------------------*/
      non_idle_blank = FALSE;

      //Initialize frame rate to blank in case non-idle frame building fails 
      //due to reasons such as no-DSM memory 
      rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
      if ((rev_fr_ptr->fr_cat_mask) & DSRLP_CAT_IDLE_MASK)
      {
        /*-------------------------------------------------------------------
          Note, if an idle frame is built, frame_built is still false,
          because it will be used to change idle_timer values.
        -------------------------------------------------------------------*/
        ASSERT( fcn_ptr_table->tx_idle != NULL);
        frame_built = fcn_ptr_table->tx_idle( rscb_ptr, rev_fr_ptr);

        if ( frame_built == TRUE)
        {
          fr_num++;
        }
        else
        {
          //Even no idle frame, so tell MUX the frame is blank
          rev_fr_ptr->fr_rate = DSRLP_RATE_BLANK;
          ERR("Could not make FCH IDLE frame",0,0,0);
        }
      } /* if idle mask */
      break;
    }
  } /* for - looping through all frames */

  
  /*-------------------------------------------------------------------------
    RLP Options may change. Check if the state variables have to be updated.
    if yes, then update the variables.
  -------------------------------------------------------------------------*/
  if (dsrlp3_opt.update_state_var == TRUE) 
  {
    dsrlp_opt_update_state();
  }

  rev_fr_blk->num_frames = fr_num;
  if ( non_idle_blank == TRUE)
  {
    /*---------------------------------------------------------------------
      If a non-idle/non-blank frame was sent, reset the idle timer to its
      initial values. The idle timer is used for sending idle frames on
      channels that can be DTXed to ensure that the receiver knows about
      the last data frames sent in a burst.
    ---------------------------------------------------------------------*/
    rscb_ptr->state.idle_timer_active = TRUE;
    rscb_ptr->state.idle_timer        = DSRLPI_IDLE_TIMER_DEF;
    rscb_ptr->state.idle_fr_xmit_cntr = 0;
  }

  /*-------------------------------------------------------------------------
    Update the stats with the count of contiguous IDLE frames transmitted. 
    Reset the counter if a valid data/ctl frame is transmitted
  -------------------------------------------------------------------------*/
  if(tx_data_frs == TRUE)
  {
    stats_ptr->tx_contig_idle_fr_cnt = 0;
  }
  else
  {
    stats_ptr->tx_contig_idle_fr_cnt++; 
  }

#ifdef FEATURE_IS2000_R_SCH
  #ifdef FEATURE_DATA_MM
    ds707_scrm_rlp_cback(rscb_ptr->cfg.tx_wm_ptr->current_cnt);
  #else
    ds_scrm_rlp_cback(rscb_ptr->cfg.tx_wm_ptr->current_cnt);
  #endif /* FEATURE_DATA_MM */
#endif

#ifdef FEATURE_IS2000_CHS
  if ( (rscb_ptr->cfg.tx_wm_ptr->current_cnt > 0)      ||
       (DSRLPRTX_HAS_REXMITS(rscb_ptr))                ||
       (rscb_ptr->state.naks_to_send == TRUE)          ||
       (rscb_ptr->state.state == DSRLPI_RT_MEAS_STATE) ||
       (rscb_ptr->state.state == DSRLPI_ACK_STATE)     ||
	   (rscb_ptr->state.ack_to_send == TRUE )
     )
  {
#ifdef FEATURE_DATA_MM
    ds707_chs_leave_chs();
#else
    ds_rrm_send_msg();
#endif /* FEATURE_DATA_MM */
  }

#ifdef FEATURE_DATA_MM
  ds707_chs_txed_data(tx_data_frs);
#endif /* FEATURE_DATA_MM */

#endif
} /* dsrlp_build_tx_frames() */
#endif /* FEATURE_DS_RLP3 */
#endif /* MSM5000_IRAM_REV */

#ifdef FEATURE_DS_RLP3
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
#define MAX_BLOB_LENGTH       16

byte dsrlp_build_rlp_blob
(
  byte  *tx_blob_ptr
)
{

  byte   blob_len;               /* holds the calculated BLOB length       */
  byte   new_byte_value;         /* holds the value of the next byte       */
  byte   max_nak_rounds_fwd;     /* max # of forward nak rounds            */
  byte   max_nak_rounds_rev;     /* max # of reverse nak rounds            */
  byte   desired_nak_rounds_fwd; /* desired # of forward nak rounds        */
  byte   desired_nak_rounds_rev; /* desired # of reverse nak rounds        */

  word   field_length  = 0;      /* holds the field length for next field  */
  word   field_position= 0;      /* holds a cumulative number of bytes used*/
  word   i;                      /* loop variable                          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(tx_blob_ptr != NULL);
  memset(tx_blob_ptr, 0, MAX_BLOB_LENGTH);


  /*-------------------------------------------------------------------------
    Prepare RLP_BLOB_TYPE field with value 001.
  -------------------------------------------------------------------------*/
  field_length       = 3;
  new_byte_value     = 0x01;
  b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);

  /*-------------------------------------------------------------------------
    Prepare RLP_VERSION field with value 011.
  -------------------------------------------------------------------------*/
  field_position    += field_length;
  field_length       = 3;
  new_byte_value     = 0x03;
  b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);

  /*-------------------------------------------------------------------------
    Prepare RESET_VAR field with value 1 to always force an RLP reset
  -------------------------------------------------------------------------*/
  field_position    += field_length;
  field_length       = 1;
  new_byte_value     = 0x01;
  b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare MAX_MS_NAK_ROUNDS_FWD field by consulting the constants data
    structure. We do not use the rscb_ptr here as it may not be initialized
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  max_nak_rounds_fwd     = dsrlp3_constants.max_nak_rounds_fwd;
  b_packb(max_nak_rounds_fwd, tx_blob_ptr, field_position, field_length);

  /*-------------------------------------------------------------------------
    Prepare MAX_MS_NAK_ROUNDS_REV field by consulting the constants data
    structure. We do not use the rscb_ptr here as it may not be initialized
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  max_nak_rounds_rev     = dsrlp3_constants.max_nak_rounds_rev;
  b_packb(max_nak_rounds_rev, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare NAK_ROUNDS_FWD field. The MS will just enter the desirable values
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_fwd = dsrlp3_opt.default_nak_rounds_fwd;
  b_packb(desired_nak_rounds_fwd, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare NAK_ROUNDS_REV field. The MS will just enter the desirable values
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_rev = dsrlp3_opt.default_nak_rounds_rev;
  b_packb(desired_nak_rounds_rev, tx_blob_ptr, field_position, field_length);


  /*-------------------------------------------------------------------------
    Prepare NAK_PER_ROUND_FWD field. The MS will enter the desirable values
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_fwd; i++ )
  {
    field_position    += field_length;
    field_length       = 3;
    new_byte_value     = dsrlp3_opt.default_naks_per_round_fwd[i];
    b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);
  }/* for */

  /*-------------------------------------------------------------------------
    Prepare NAK_PER_ROUND_REV field. The MS will enter the desirable values
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_rev; i++ )
  {
    field_position    += field_length;
    field_length       = 3;
    new_byte_value     = dsrlp3_opt.default_naks_per_round_rev[i];
    b_packb(new_byte_value, tx_blob_ptr, field_position, field_length);
  }/* for */


  /*-------------------------------------------------------------------------
    Calculate the BLOB length in bytes and assign
  -------------------------------------------------------------------------*/
  field_position += field_length;

  if ( field_position % 8 == 0)
  {
      blob_len = field_position / 8;

  }  else {

      blob_len = (byte)(field_position/8) + 1;
  }

  return(blob_len);

}/* dsrlp_build_rlp_blob() */
#endif /* FEATURE_DS_RLP3 */


#ifdef FEATURE_DS_RLP3
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
)
{

  byte rscb_index;
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;/* session control block pointer     */

  byte    desired_nak_rounds_fwd;     /* desired forward nack rounds       */
  byte    desired_nak_rounds_rev;     /* desired reverse nack rounds       */
  byte    naks_per_round_fwd;         /* nacks per round on forward        */
  byte    naks_per_round_rev;         /* nacks per round on reverse        */
  byte    new_byte;                   /* holds the new received byte       */
  byte    rtt_byte=0;                 /* hold the rtt to be able to restore*/


  word    field_length       = 0;     /* holds the length of the field     */
  word    field_position     = 0;     /* cumulative count of bytes         */
  word    i                  = 0;     /* loop variable                     */
  word    max_to_set_fwd     = 0;     /* helps use only relevant fields    */
  word    max_to_set_rev     = 0;     /* helps use only relevant fields    */

  boolean no_sync_ack        = FALSE; /* do we need to do sync/ack ?       */
  boolean BLOB_during_call   = FALSE; /* did we rx the BLOB during a call? */

  event_rlp_payload_type     rlp_event_payload;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_index = dsrlpi_get_rscb_index_for_sr_id( sr_id );
  if (rscb_index == DSRLPI_INVALID_RSCB_INDEX)
  {
     /* Cannot process an entry that doesn't exist! */
     return FALSE;
  }

  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);
  ASSERT(rscb_ptr->cfg.ver == DSRLP_REV_3);

  /*-------------------------------------------------------------------------
    Determine if we are receiving this BLOB during a call. If this is the
    case we want to process all parameters and then force a dsrlpi_reset()
    and not force the state to _ESTABLISHED so that sync/ack happens.
  -------------------------------------------------------------------------*/
  if (rscb_ptr->state.state == DSRLPI_ESTABLISHED_STATE)
  {
      BLOB_during_call = TRUE;
  }

  /*-------------------------------------------------------------------------
    Process RLP_BLOB_TYPE field with value 001.
  -------------------------------------------------------------------------*/
  field_length = 3;
  new_byte     = b_unpackb(blob_ptr, field_position, field_length);
  if (new_byte != 0x01)
  {
    MSG_HIGH("BLOB::RLP_BLOB_TYPE has invalid entry %d RLP:%d", 
             new_byte, sr_id, 0);
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    Process RLP_VERSION field with value 011.
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length    = 3;
  new_byte        = b_unpackb(blob_ptr, field_position, field_length);
  if (new_byte != 0x03)
  {
    MSG_HIGH("BLOB::RLP_VERSION  has invalid entry %d RLP:%d", 
              new_byte, sr_id, 0);
    return(FALSE);
  }

  /*-------------------------------------------------------------------------
    We know that this BLOB will at least specify a NAK scheme
  -------------------------------------------------------------------------*/
  rscb_ptr->stats.RLP_BLOB_used = DSRLP_BLOB_NAK_SCHEME;

  /*-------------------------------------------------------------------------
    Process RTT field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length    = 4;
  new_byte        = b_unpackb(blob_ptr, field_position, field_length);
  if (new_byte == 0)
  {
    /*-----------------------------------------------------------------------
      SYNC exchange procedures required to establish a value for RLP_DELAY
    -----------------------------------------------------------------------*/
    dsrlpi_reset(rscb_ptr);

    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->sr_id;
    event_report_payload (EVENT_RLP, 
                          sizeof(rlp_event_payload),
                          &rlp_event_payload);
  }
  else
  {
    /*-----------------------------------------------------------------------
      NO SYNC exchange procedures required, use this value for RLP_DELAY
    -----------------------------------------------------------------------*/
    no_sync_ack = TRUE;
    rscb_ptr->state.rlp_delay = rtt_byte = new_byte;
    rscb_ptr->state.state     = DSRLPI_ESTABLISHED_STATE;
    MSG_MED("process blob, rlp move to est state",0,0,0);
    /*---------------------------------------------------------------------
       Report RLP established event
    ---------------------------------------------------------------------*/
   rlp_event_payload.event_rlp_type = event_rlp_established;
   rlp_event_payload.info = rscb_ptr->sr_id;
   event_report_payload (EVENT_RLP, 
                         sizeof(event_rlp_payload_type),
                         &rlp_event_payload);


    /*-----------------------------------------------------------------------
      Set this back to zero now that we are out of all 3 init states
    -----------------------------------------------------------------------*/
    rscb_ptr->state.sync_delay= 0;

    MSG_MED("BLOB::RTT value is %d RLP:%d", new_byte, sr_id, 0);

    /*-----------------------------------------------------------------------
      This BLOB specifies RTT as well as a  NAK scheme
    -----------------------------------------------------------------------*/
    rscb_ptr->stats.RLP_BLOB_used = DSRLP_BLOB_RTT_PLUS_NAK;
  }


  /*-------------------------------------------------------------------------
    Process RESET_VAR field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length    = 1;
  new_byte        = b_unpackb(blob_ptr, field_position, field_length);
  if (new_byte == 0x1)
  {
    MSG_HIGH("BLOB::RESET_VAR field is %d. Forcing RESET RLP:%d", 
             new_byte, sr_id, 0);
    dsrlpi_reset(rscb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->sr_id;
    event_report_payload (EVENT_RLP, 
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);

    /*-----------------------------------------------------------------------
      Avoid sync exchange procedures if we got an RTT > 0
    -----------------------------------------------------------------------*/
    if (no_sync_ack)
    {
      rscb_ptr->state.rlp_delay = rtt_byte;
      rscb_ptr->state.state     = DSRLPI_ESTABLISHED_STATE;
      MSG_MED("process blob, rlp move to est state",0,0,0);
      /*---------------------------------------------------------------------
         Report RLP established event
      ---------------------------------------------------------------------*/
      rlp_event_payload.event_rlp_type = event_rlp_established;
      rlp_event_payload.info = rscb_ptr->sr_id;
      event_report_payload (EVENT_RLP, 
                            sizeof(event_rlp_payload_type),
                            &rlp_event_payload);



      /*---------------------------------------------------------------------
        Set this back to zero now that we are out of all 3 init states
      ---------------------------------------------------------------------*/
      rscb_ptr->state.sync_delay= 0;

    }

  }

  /*-------------------------------------------------------------------------
    Process NAK_ROUNDS_FWD field
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_fwd = b_unpackb(blob_ptr, field_position, field_length);

  if (desired_nak_rounds_fwd <= rscb_ptr->ver_ptr->max_nak_rounds_fwd )
  {
    /*-----------------------------------------------------------------------
      In this case set the current to the value we just received
    -----------------------------------------------------------------------*/
    rscb_ptr->state.nak_rounds_fwd = desired_nak_rounds_fwd;

    MSG_MED("NAK_ROUNDS_FWD is SET to %d. RLP: %d",desired_nak_rounds_fwd, sr_id, 0);
    max_to_set_fwd       = desired_nak_rounds_fwd;

  }
  else
  {
    /*-----------------------------------------------------------------------
      Best we can do is set the desired to be equal to the max
    -----------------------------------------------------------------------*/
    max_to_set_fwd = rscb_ptr->state.nak_rounds_fwd =
                          rscb_ptr->ver_ptr->max_nak_rounds_fwd;

    MSG_HIGH("Requested NAK_ROUNDS_FWD %d is INVALID. RLP:%d",
             desired_nak_rounds_fwd, sr_id, 0);
    MSG_MED("NAK_ROUNDS_FWD is SET to %d. RLP:%d", max_to_set_fwd, sr_id, 0);

  }
   /*------------------------------------------------------------------------ 
    Update the negotiated options field with lowest of BS and MS requested
    values.
    Note that the value that RLP will use, is minimum of (negotiated and 
    active/current value). So write that value to the "working" variable.
	Now we have MIN(Negotiate,Active) as working variable.
  ------------------------------------------------------------------------*/
  dsrlp3_opt.negotiated_nak_rounds_fwd = rscb_ptr->state.nak_rounds_fwd;
  max_to_set_fwd = MIN(dsrlp3_constants.desired_nak_rounds_fwd,
                       dsrlp3_opt.negotiated_nak_rounds_fwd);
  rscb_ptr->state.nak_rounds_fwd = max_to_set_fwd;

  /*-------------------------------------------------------------------------
    Process NAK_ROUNDS_REV field
  -------------------------------------------------------------------------*/
  field_position        += field_length;
  field_length           = 3;
  desired_nak_rounds_rev = b_unpackb(blob_ptr, field_position, field_length);

  if (desired_nak_rounds_rev <= rscb_ptr->ver_ptr->max_nak_rounds_rev )
  {
    /*-----------------------------------------------------------------------
      In this case set the desired to the value we just received
    -----------------------------------------------------------------------*/
    rscb_ptr->state.nak_rounds_rev = desired_nak_rounds_rev;
    MSG_MED("NAK_ROUNDS_REV is SET to %d. RLP:%d", 
            desired_nak_rounds_rev, sr_id, 0);
    max_to_set_rev       = desired_nak_rounds_rev;

  }
  else
  {
    /*-----------------------------------------------------------------------
      Best we can do is set the desired to be equal to the max
    -----------------------------------------------------------------------*/
    max_to_set_rev = rscb_ptr->state.nak_rounds_rev =
                          rscb_ptr->ver_ptr->max_nak_rounds_rev;
    MSG_HIGH("Requested NAK_ROUNDS_REV %d is INVALID. RLP: %d",
              desired_nak_rounds_rev, sr_id, 0);
    MSG_MED("NAK_ROUNDS_REV is SET to %d.\n", max_to_set_rev, 0, 0);
  }

  /*------------------------------------------------------------------------- 
    Update the negotiated options in reverse direction with lowest of BS 
    and MS. rscb_ptr->state.nak_rounds_rev has the lowest value between BS 
    and MS. So copy it to negotiated value.
    Note that the value that RLP will use as working variable , is minimum 
    of (negotiated and current/active value). So write that value to the 
    "working" variable. Now we have MIN(Negotiate,Active) as working 
    variable.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.negotiated_nak_rounds_rev = rscb_ptr->state.nak_rounds_rev;
  max_to_set_rev= MIN(dsrlp3_constants.desired_nak_rounds_rev,
                      dsrlp3_opt.negotiated_nak_rounds_rev);
  rscb_ptr->state.nak_rounds_rev = max_to_set_rev;

  /*-------------------------------------------------------------------------
    Update the statistic for current tx naks rounds
  -------------------------------------------------------------------------*/
  ASSERT( max_to_set_rev <= DSRLP_MAX_NAK_ROUNDS);
  rscb_ptr->stats.curr_tx_nak_rounds = max_to_set_rev;

  /*-------------------------------------------------------------------------
    Process NAK_PER_ROUND_FWD fields
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_fwd; i++)
  {

    field_position    += field_length;
    field_length       = 3;
    naks_per_round_fwd = b_unpackb(blob_ptr, field_position, field_length);

    if(i < max_to_set_fwd)
    {

      /*---------------------------------------------------------------------
        Obtain the Negotiated Value: Negotiated value = BS requested value
        We choose that Negotiated value is same as BS value. We use this
        value as our upper ceiling.
      ---------------------------------------------------------------------*/
      dsrlp3_opt.negotiated_naks_per_round_fwd[i] = naks_per_round_fwd; 

      /*---------------------------------------------------------------------
        Obtain the value to be used as Working Value:
         Working Vale = Min(Negotiated Value,Active Value)
      ---------------------------------------------------------------------*/
      rscb_ptr->state.naks_per_round_fwd[i] = 
          MIN(dsrlp3_opt.negotiated_naks_per_round_fwd[i],
              dsrlp3_constants.desired_naks_per_round_fwd[i]); 

      MSG_MED("NAKS_PER_ROUND_FWD is SET to %d. RLP: %d", 
              rscb_ptr->state.naks_per_round_fwd[i], sr_id, 0);

    }/* if */

  }/* for */


  /*-------------------------------------------------------------------------
    Process NAK_PER_ROUND_REV fields
  -------------------------------------------------------------------------*/
  for (i=0; i < desired_nak_rounds_rev; i++)
  {

    field_position    += field_length;
    field_length       = 3;
    naks_per_round_rev = b_unpackb(blob_ptr, field_position, field_length);

    if(i < max_to_set_rev)
    {
      /*---------------------------------------------------------------------
        Obtain the Negotiated Value: Negotiated value = BS requested Value
        We choose that Negotiated value is same as BS value. We use this
        value as our upper ceiling.
      ---------------------------------------------------------------------*/
      dsrlp3_opt.negotiated_naks_per_round_rev[i] = naks_per_round_rev; 


      /*---------------------------------------------------------------------
        Obtain the value to be used as Working Value:         
          Working Vale = MIN(Negotiated Value,Active Value);
      ---------------------------------------------------------------------*/
      rscb_ptr->state.naks_per_round_rev[i] = 
          MIN(dsrlp3_opt.negotiated_naks_per_round_rev[i],
              dsrlp3_constants.desired_naks_per_round_rev[i]);
      
      MSG_MED("NAKS_PER_ROUND_REV is SET to %d. RLP:%d", 
               rscb_ptr->state.naks_per_round_rev[i], sr_id, 0);

    }/* if */

  }/* for */

  /*-------------------------------------------------------------------------
    We are receiving this BLOB during a call. Force a dsrlpi_reset()
    and not force the state to _ESTABLISHED so that sync/ack happens.
  -------------------------------------------------------------------------*/
  if (BLOB_during_call)
  {
    dsrlpi_reset(rscb_ptr);
    /*-----------------------------------------------------------------------
       Report RLP reset event
    -----------------------------------------------------------------------*/
    rlp_event_payload.event_rlp_type = event_rlp_reset;
    rlp_event_payload.info = rscb_ptr->sr_id;
    event_report_payload (EVENT_RLP, 
                          sizeof(event_rlp_payload_type),
                          &rlp_event_payload);

  }

  /*-------------------------------------------------------------------------
    if the code is here then everything is fine with the BLOB
  -------------------------------------------------------------------------*/
  return(TRUE);

}/* dsrlp_process_rlp_blob() */
#endif /* FEATURE_DS_RLP3 */


/*===========================================================================

FUNCTION DSRLP_GET_STATS

DESCRIPTION
  This function gets an sr_id and then returns a pointer to the appropriate
  structure containing the related statistics.

  Input Parameters:

  sr_id:         used to find the appropriate session control block pointer

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
boolean dsrlp_get_stats(
  byte                      rscb_index,
  dsrlp_stats_info_type   **stats_ptr_ptr
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  boolean status = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rscb_index >= DSRLPI_MAX_RSCB_ELEMENTS)
  {
    MSG_HIGH("Using Invalid SR_ID %d to Retrieve RLP Stats", rscb_index, 0, 0);
    
    /*----------------------------------------------------------------------
    Hard Code it to 1 to avoid potential memory corruption if the client does
    not check the return value
    -----------------------------------------------------------------------*/
    rscb_index = 1;
    status = FALSE;
  }
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id_index.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);


  /*-------------------------------------------------------------------------
    Update the rlp "state" field in the stats stucture now.
  -------------------------------------------------------------------------*/
  rscb_ptr->stats.state = rscb_ptr->state.state;

  /*-------------------------------------------------------------------------
    Pass a pointer to the stats structure to the ouside world
  -------------------------------------------------------------------------*/
  *stats_ptr_ptr = &rscb_ptr->stats;

  return status;

}/* dsrlp_get_stats() */



/*===========================================================================

FUNCTION DSRLP_CLEAR_STATS

DESCRIPTION
  This function takes an index in the rscb array (corresponding to an sr_id)
  control block pointer and from there it will initialize the related
  structure with the statistics.

  Input Parameters:

  rscb_index:         used to find the appropriate session control block pointer


DEPENDENCIES
  A valid rscb_index value must be passed into this function.

  Also the session control block pointed to by the rscb_ptr should have been
  initialized and have valid values as this function uses that structure.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_clear_stats(byte rscb_index)
{
    dsrlpi_sess_ctl_blk_type  *rscb_ptr;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rscb_index >= DSRLPI_MAX_RSCB_ELEMENTS)
  {
    MSG_HIGH("Using Invalid SR_ID %d to Clear RLP Stats", rscb_index, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  memset( &rscb_ptr->stats, 0, sizeof(dsrlp_stats_info_type) );

  /*-------------------------------------------------------------------------
    Set these fields  to their defaults at this time.
  -------------------------------------------------------------------------*/
  rscb_ptr->stats.curr_tx_nak_rounds = rscb_ptr->ver_ptr->max_nak_rounds_rev;
  rscb_ptr->stats.RLP_BLOB_used      = DSRLP_BLOB_NOT_USED;

  /*-------------------------------------------------------------------------
    Note the time
  -------------------------------------------------------------------------*/
  ts_get( (unsigned long *) (&rscb_ptr->stats.reset_timestamp) );

}/* dsrlp_clear_stats() */



#ifdef FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_DS_RLP3
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
byte dsrlp_get_max_ms_nak_rounds_fwd()
{
   return(dsrlp3_constants.max_nak_rounds_fwd);
}
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_DS_RLP3
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
byte dsrlp_get_max_ms_nak_rounds_rev()
{
    return(dsrlp3_constants.max_nak_rounds_rev);
}
#endif /* FEATURE_DS_RLP3 */

/*===========================================================================
FUNCTION      DSRLP_REG_SRVC

DESCRIPTION   Registers the watermarks and queues/callbacks for an RLP
              session.

DEPENDENCIES  A valid index (corresponding to an sr_id) must be passed 
              into this function. All input ptrs must be non-NULL.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsrlp_reg_srvc
(
  byte                rscb_index,            /* index for a session id     */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit frs */
  void                (*post_rx_func_ptr)(), /* rx callback for rx frames  */
  q_type              *post_rx_q_ptr         /* queue for rx'ed frames     */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);

  ASSERT(rscb_ptr         != NULL);
  ASSERT(tx_watermark_ptr != NULL);
  ASSERT(post_rx_func_ptr != NULL);
  ASSERT(post_rx_q_ptr    != NULL);

  /*-------------------------------------------------------------------------
    For the given RLP session, set the input (payload for txmit) and output
    (payload after receive) watermarks.
    Note, changing watermarks does not cause a RLP reset.
  -------------------------------------------------------------------------*/
  rscb_ptr->cfg.tx_wm_ptr   = tx_watermark_ptr;
  rscb_ptr->cfg.rx_func_ptr = post_rx_func_ptr;
  rscb_ptr->cfg.rx_q_ptr    = post_rx_q_ptr;
} /* dsrlp_reg_srvc */


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
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity check - Since this should only be called for 1X calls, this must
    always be called with the fixed SR_ID index. If this assumption changes
    in the future, remove this ASSERT.
  -------------------------------------------------------------------------*/
  ASSERT(rscb_index == DSRLP_FIXED_SR_ID_INDEX);

  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[rscb_index]);
  ASSERT(rscb_ptr         != NULL);

  TASKLOCK();
  rscb_ptr->cfg.notify_ppp_data_transfer_func_ptr = 
                                               ppp_data_transfer_notify_func;
  /*-----------------------------------------------------------------------
   Since this function will only be registered when the call goes dormant,
   set the PPP data transfer flag to FALSE to indicate that no data
   transfer has happened since dormancy. It will be set to TRUE when data
   is received or transmitted by RLP3. 

   If deregistering this function, set the flag to FALSE for cleanup.
  -----------------------------------------------------------------------*/
  rscb_ptr->cfg.ppp_data_transfer_after_dormancy = FALSE;
  TASKFREE();
}


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
  byte  rscb_index                       /* passed in SR_ID  index         */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rscb_index < DSRLPI_MAX_RSCB_ELEMENTS)
  {
    rscb_ptr = &dsrlpi_rscb_array[rscb_index];
    ASSERT(rscb_ptr != NULL);
  }
  else
  {
    /*-----------------------------------------------------------------------
       invalid SR_ID...return FALSE
    -----------------------------------------------------------------------*/
    ERR("BAD SR ID",0,0,0);
    return(0);
  }

  if ( ((rscb_ptr->cfg.tx_wm_ptr != NULL) && 
        (rscb_ptr->cfg.tx_wm_ptr->current_cnt > 0)
       )                                                ||
       (DSRLPRTX_HAS_REXMITS(rscb_ptr))
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}

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
   byte  rscb_index                /* passed in index for the SR_ID        */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (rscb_index >= DSRLPI_MAX_RSCB_ELEMENTS)
  {
    /*-----------------------------------------------------------------------
       invalid SR_ID...return FALSE
    -----------------------------------------------------------------------*/
    ERR("BAD SR ID",0,0,0);
    return(FALSE);
  }
  else
  {
    rscb_ptr = &( dsrlpi_rscb_array[ rscb_index ]);
    ASSERT(rscb_ptr != NULL);
  }

  /*-------------------------------------------------------------------------
    Conditions to check for - there is new data to send
                            - there are rexmits to send
                            - it is time to send a NAK
                            - We are calculating RTT during SYNC-ACK 
  -------------------------------------------------------------------------*/
  if ( ((rscb_ptr->cfg.tx_wm_ptr != NULL) && 
        (rscb_ptr->cfg.tx_wm_ptr->current_cnt > 0)
       )                                               ||
       (DSRLPRTX_HAS_REXMITS(rscb_ptr))                ||
       (rscb_ptr->state.naks_to_send == TRUE)          ||
       (rscb_ptr->state.state == DSRLPI_RT_MEAS_STATE) ||
       (rscb_ptr->state.state == DSRLPI_ACK_STATE)     ||
	   (rscb_ptr->state.ack_to_send == TRUE )
     )
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
}

#ifdef TIMETEST_DSRLP_STATS

/*===========================================================================

FUNCTION DSRLP_LOG_TIMETEST_THROUGHPUT_DATA

DESCRIPTION
  This function  logs RLP throughput statistics; called from
  dsrlp_log_timetest_throughput_hdr for SN RLP, every 500 ms.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void dsrlp_log_timetest_throughput_data
( 
  uint16 rlp_type
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* rlp-specific control block  */
  static dsrlp_throuput_data_type rlp_data_1x;
#ifdef FEATURE_HDR
#error code not present
#endif
  uint16    tx_div,rx_div;                  /* rx/tx scale factors         */
  uint32    rx_delta, tx_delta;             /* bytes rx/tx in interval     */
  uint8     rx_throuput, tx_throuput;
  dsrlp_throuput_data_type rlp_data_local;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Get the total Rx and Tx bytes and calculate the delta.  This delta will
    be written out to the TIMETEST port.  

    Note, for RX, for HDR this value will be counted in 1000's of bytes 
    (K's of bytes) whereas for 1X it will be in 100's of bytes.  
    This is to accomodate the higher forward link rates of HDR, while using 
    only a fixed 1 byte value to write out to TIMETEST.  

    For both HDR and 1X, the Tx value will always be in 100's of bytes. 
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Calculate Rx divisor based on what current type of call.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_HDR
#error code not present
#endif
  if(rlp_type == TIMETEST_DSRLP_IS2K)
  {
    rscb_ptr = &dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX];
    rx_div = 100;
    memcpy(&rlp_data_local,&rlp_data_1x,sizeof(dsrlp_throuput_data_type));
  }
  else
  {
     MSG_ERROR("invalid rlp_type in timetest rlp logging",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Calculate interval Rx bytes and log to TIMETEST port.
  -------------------------------------------------------------------------*/
  rx_delta = rscb_ptr->stats.rx_total_bytes - rlp_data_local.last_rx_bytes;
  rlp_data_local.last_rx_bytes = rscb_ptr->stats.rx_total_bytes;

  /*-------------------------------------------------------------------------
    Calculate interval Tx bytes and log to TIMETEST port.
  -------------------------------------------------------------------------*/
  tx_div = 100;
  tx_delta = rscb_ptr->stats.tx_total_bytes - rlp_data_local.last_tx_bytes;
  rlp_data_local.last_tx_bytes = rscb_ptr->stats.tx_total_bytes;

  MSG_MED("Log RLP TIMETEST rx: %d tx: %d", (uint8)(rx_delta/rx_div), 
                           (uint8)(tx_delta/tx_div),0);

  rx_throuput = rx_delta / rx_div;
  tx_throuput = tx_delta / tx_div;

  /*-------------------------------------------------------------------------
    if no data transferred in this interval donot write to timetest port
  -------------------------------------------------------------------------*/

  if( ! (( rx_throuput == 0) && ( tx_throuput == 0)))
  {

    TIMETEST_DSRLP_BYTES( rlp_type,
                          (uint8)(rx_throuput), 
                          (uint8)(tx_throuput) );
  }

#ifdef FEATURE_HDR
#error code not present
#endif
  if(rlp_type == TIMETEST_DSRLP_IS2K)
  {
    memcpy(&rlp_data_1x,&rlp_data_local,sizeof(dsrlp_throuput_data_type));
  }

}

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

void dsrlp_log_timetest_throughput
( 
  int4 ms 
)
{
#ifdef FEATURE_HDR
#error code not present
#endif
  dsrlp_log_timetest_throughput_data(TIMETEST_DSRLP_IS2K);
}

#endif


/*===========================================================================
FUNCTION      DSRLP_CHECK_SR_ID

DESCRIPTION   Check if the SR_ID for the service running on a particular RLP
              instance (rscb array index) is the same as in the MUX header in
			  the frame

DEPENDENCIES  The passed in RSCB_INDEX is good.

RETURN VALUE  TRUE if the SR_ID matches, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_check_sr_id(byte rscb_index, dsm_item_type *frame_ptr)
{
	if (((frame_ptr->data_ptr[ 0]) & 0xe0) == dsrlpi_rscb_array[rscb_index].sr_id << 5)
	{
		return TRUE;
	}
	else
	{
        /*------------------------------------------------------------------
          Note that for 1x rate this message may show up and you can ignore 
          it.For 2x,4x,8x,16x this should not happen. 
        ------------------------------------------------------------------*/
        MSG_HIGH(" SR_ID mismatch BS:%d & MS:%d ",
                               (((frame_ptr->data_ptr[ 0]) & 0xe0) >> 5),
                               dsrlpi_rscb_array[rscb_index].sr_id,
                               0);

		return FALSE;
	}
}

/*===========================================================================
FUNCTION      DSRLP_GET_SR_ID

DESCRIPTION   Return the SR_ID for the service running on a particular RLP
              instance (rscb array index)

DEPENDENCIES  The passed in RSCB_INDEX is good. Also dsrlp_init must have 
              been invoked first for the returned SR_ID to be good.

RETURN VALUE  The SR_ID corresponding to the RLP instance

SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_get_sr_id(byte rscb_index)
{
   return dsrlpi_rscb_array[rscb_index].sr_id;
}
              

/*===========================================================================
FUNCTION      DSRLP3_OPT_INIT

DESCRIPTION   Initializes all the RLP option fields 

DEPENDENCIES  Should be initialized before making the first data call. So
              power on initialization is the best place.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void dsrlp3_opt_init
(
  void
)
{
  int i;                                                  /* loop variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize the default and negotiated nak rounds forward to the MAX
    nak rounds permitted by MS RLP implementation.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_fwd    = DSRLP_MAX_NAK_ROUNDS;
  dsrlp3_opt.negotiated_nak_rounds_fwd = DSRLP_MAX_NAK_ROUNDS;
      
  /*-------------------------------------------------------------------------
	We are using the "for" loop, to copy contents of the dsrlp3_option 
    variables because currently the default values are 1,2 & 3. To play safe, 
    we could have copied the default values explicitly here but
    its assumed that the values we are always interested initially in default 
    are 1,2,3 and so we optimized it. 
    
    For negotiated value, initialize to maximum possible value. After 
    negotiation is done then this value will be modified. This is needed 
    because if negotiation is not done then:
     working value = MIN(negotiated,desired). So desired should be less than
    DSRLP_MAX_NAKS_PER_ROUND for any NAK round.
  -------------------------------------------------------------------------*/
  for (i=0; i<DSRLP_MAX_NAK_ROUNDS;i++) 
  {
    dsrlp3_opt.default_naks_per_round_fwd[i]    = i+1;
    dsrlp3_opt.negotiated_naks_per_round_fwd[i] = DSRLP_MAX_NAKS_PER_ROUND;
  }
	 
  /*-------------------------------------------------------------------------
	Initialize the default and negotiated nak rounds reverse to the MAX nak 
    rounds permitted by MS RLP implementation.
  -------------------------------------------------------------------------*/	 
  dsrlp3_opt.default_nak_rounds_rev    = DSRLP_MAX_NAK_ROUNDS; 
  dsrlp3_opt.negotiated_nak_rounds_rev = DSRLP_MAX_NAK_ROUNDS; 

  /*-------------------------------------------------------------------------
	We are using the "for" loop, to copy contents of the dsrlp3_option 
    variables because currently the default values are 1,2 & 3. To play safe, 
    we could have copied the values explicitly (without a "for" loop) here 
    but its assumed that the values we are always interested initially are 
    1,2,3 and so we optimized it.
    
    For negotiated value, initialize to maximum possible value. After 
    negotiation is done then this value will be modified. This is needed 
    because if negotiation is not done then:
     working value = MIN(negotiated,desired). So desired should be less than
    DSRLP_MAX_NAKS_PER_ROUND for any NAK round.    
  -------------------------------------------------------------------------*/
  for (i=0; i<DSRLP_MAX_NAK_ROUNDS;i++) 
  {
    dsrlp3_opt.default_naks_per_round_rev[i]    = i+1;
    dsrlp3_opt.negotiated_naks_per_round_rev[i] = DSRLP_MAX_NAKS_PER_ROUND;
  }  

  dsrlp3_opt.update_state_var = FALSE;
} /* dsrlp3_opt_init */

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
boolean dsrlp_get_all_def_naks
(
   byte *nak_rounds_fwdp,                /* Num of rounds for forward NAKs */
   byte *naks_per_round_fwdp,            /* Num of NAKs per round forward  */
   byte *nak_rounds_revp,                /* Num of rounds for reverse NAKs */
   byte *naks_per_round_revp             /* Num of NAKs per round reverse  */
)
{
   int i;                                                  /* loop Counter */  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /*-------------------------------------------------------------------------
   Confirm that all the input paramters are in the range. If they are not
   In range then return FALSE.
 -------------------------------------------------------------------------*/
 if ( (nak_rounds_fwdp == NULL)     ||
      (naks_per_round_fwdp == NULL) ||
      (nak_rounds_revp == NULL)     ||
      (naks_per_round_revp == NULL)
    )
 {
 //   ASSERT(0); Only for testing.
    return FALSE;
 }

 /*-------------------------------------------------------------------------
   Copy the values to the passed arguments for fwd parameters.
 -------------------------------------------------------------------------*/
   *nak_rounds_fwdp = dsrlp3_opt.default_nak_rounds_fwd;
   for (i=0; i<dsrlp3_opt.default_nak_rounds_fwd; i++)
   {
      naks_per_round_fwdp[i] = dsrlp3_opt.default_naks_per_round_fwd[i];
   }
   
   /*-------------------------------------------------------------------------
     Copy the values to the passed arguments for Rev parameters.
   -------------------------------------------------------------------------*/
   *nak_rounds_revp = dsrlp3_opt.default_nak_rounds_rev;
   for (i=0; i<dsrlp3_opt.default_nak_rounds_rev; i++)
   {
      naks_per_round_revp[i] = dsrlp3_opt.default_naks_per_round_rev[i];
   }
   return TRUE;
 } /* dsrlp_get_all_def_naks */
  
/*===========================================================================
  FUNCTION      DSRLP_SET_ALL_DEF_NAKS
 
  DESCRIPTION   Set the number of rounds and the naks per round to be
                used when negotiating an RLP3 service option
 
  DEPENDENCIES  Can't lengthen the number of rounds beyond max_rounds. 
                
  RETURN VALUE  TRUE:  If we were able to set the values.
                FALSE: None of the values are changed, since some parameter
                       is having a wrong value.
                Values are returned through arguments.
                
  SIDE EFFECTS  Changes don't take effect until next SO negotiation
===========================================================================*/
boolean dsrlp_set_all_def_naks
(
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Confirm that all the input paramters are in the range. If they are not
    in range then return FALSE.
    Confirm that none of the pointers are NULL. 
    Confirm that the contents of the pointers are within valid range. The 
    valid values are given below:
	
     nak_rounds_fwd:     Should not be NUL
     nak_rounds_fwd:     Should not be greater than MAX_NAK_ROUNDS
     naks_per_round_fwd: Should not be NULL
     nak_rounds_rev:     Should not be NULL
     nak_rounds_rev:     Should not be Greater than MAX_NAK_ROUNDS
     naks_per_round_rev: Should not be NULL   
  -------------------------------------------------------------------------*/
  if( 
     ( nak_rounds_fwdp == NULL)                 ||
     ( *nak_rounds_fwdp > DSRLP_MAX_NAK_ROUNDS) ||
     ( naks_per_round_fwdp == NULL)             ||
     ( nak_rounds_revp == NULL)                 ||
     ( *nak_rounds_revp > DSRLP_MAX_NAK_ROUNDS) ||
     ( naks_per_round_revp == NULL)
    ) 
    {
    //   ASSERT(0);  Only for testing
       return FALSE;
    }
            
  /*-------------------------------------------------------------------------
    Confirm that the values of NAKS PER ROUND are within the range.
  -------------------------------------------------------------------------*/
  for (i=0; i<(*nak_rounds_fwdp); i++)
  {
     if (naks_per_round_fwdp[i] > DSRLP_MAX_NAKS_PER_ROUND)
        return FALSE;
  }  

  for (i=0; i<(*nak_rounds_revp); i++)
  {
     if (naks_per_round_revp[i] > DSRLP_MAX_NAKS_PER_ROUND)
        return FALSE;
  }  


  /*-------------------------------------------------------------------------
    Update the structure dsrlp3_opt and the values given here will be 
    requested by Mobile in the next Service Negotiation. But if the peer 
    RLP requests for values lower than these, then the negotiated values 
    are different than the modified values. 
    
    Update the Nak Rounds and Naks per round values that should be 
    used in the forward channel.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_fwd = *nak_rounds_fwdp;
  for (i=0; i<(*nak_rounds_fwdp); i++)
  {
    dsrlp3_opt.default_naks_per_round_fwd[i] = naks_per_round_fwdp[i];
  }

  /*-------------------------------------------------------------------------
    Update the Nak Rounds and Naks per round values that should be 
    used in the Reverse channel.
  -------------------------------------------------------------------------*/
  dsrlp3_opt.default_nak_rounds_rev = (*nak_rounds_revp);
  for (i=0; i< (*nak_rounds_revp); i++)
  {
     dsrlp3_opt.default_naks_per_round_rev[i] = naks_per_round_revp[i];
  }
  return TRUE;
  
} /* dsrlp_set_all_def_naks */
   
/*===========================================================================
  FUNCTION      DSRLP_GET_CUR_NAKS
 
  DESCRIPTION   Return the number of rounds and the naks per round currently
                in use (or last used) in an RLP3 service option call
  
  DEPENDENCIES  There must be sufficient space in the naks_per_round arrays
                to hold the returned data
                For current implementation rscb_index can have only 
                DSRLP_FIXED_SR_ID_INDEX
  
  RETURN VALUE  Values are returned by argument pointers
                TRUE:  If we were able to get the values.
                FALSE: Some argument is wrong. So could not return values.
  
  
  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_get_all_cur_naks
(
  byte rscb_index,                       /* RLP instance                   */
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* rlp-specific control block */
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Confirm that all the input paramters are in the range. If they are not
    In range then return FALSE.
  -------------------------------------------------------------------------*/
  if ( 
      (rscb_index >= DSRLPI_MAX_RSCB_ELEMENTS)  ||
      (nak_rounds_fwdp == NULL)                ||
      (naks_per_round_fwdp == NULL)            ||
      (nak_rounds_revp == NULL)                ||
      (naks_per_round_revp == NULL)
     )
  {
     return FALSE;
  }
     
  /*------------------------------------------------------------------------- 
    Obtain the pointer of the RLP parameters array 
    Note: For current implementation rscb_index can have only 
    DSRLP_FIXED_SR_ID_INDEX
  -------------------------------------------------------------------------*/
  rscb_ptr = &dsrlpi_rscb_array[rscb_index];
  ASSERT(rscb_ptr != NULL);
  
  /*-------------------------------------------------------------------------
    Obtain the NAK rounds and Naks per round in forward direction. 
       
    It is assumed that the caller of this function will read only elements 
    upto naks_per_round_fwd[nak_rounds_fwd-1] in the array so, we fill
    only those values in the array.
  -------------------------------------------------------------------------*/
  *nak_rounds_fwdp = rscb_ptr->state.nak_rounds_fwd;
  for (i=0; i<rscb_ptr->state.nak_rounds_fwd; i++)
  {
     naks_per_round_fwdp[i] = rscb_ptr->state.naks_per_round_fwd[i];
  }

  /*-------------------------------------------------------------------------
    Obtain the NAK rounds and Naks per round in Reverse direction. 
  
    It is assumed that the caller of this function will read only elements 
    upto naks_per_round_fwd[nak_rounds_fwd-1] in the array ans so, we fill
    only those values in the array.
  -------------------------------------------------------------------------*/
  *nak_rounds_revp = rscb_ptr->state.nak_rounds_rev;
  for (i=0; i<rscb_ptr->state.nak_rounds_rev; i++)
  {
    naks_per_round_revp[i] = rscb_ptr->state.naks_per_round_rev[i];
  }
     
  /*-------------------------------------------------------------------------
    Since we give the information that caller requested, return TRUE
  -------------------------------------------------------------------------*/
  return TRUE;

} /* dsrlp_get_all_naks */
 
/*===========================================================================
  FUNCTION      DSRLP_SET_ALL_CUR_NAKS
 
  DESCRIPTION   Set the number of rounds and the naks per round to be
                used in the current call, overiding the defaults or 
                negotiated values. These values are used over the next call
                also if it is less than negotiated value.
  
  DEPENDENCIES  Each NAK round must be less than or equal to what was
                specified as the default when the call was originated.
                The passed in RSCB_INDEX is good. Also dsrlp_init must have 
                been invoked first for the returned SR_ID to be good.
  
  RETURN VALUE  TRUE:  If we were able to set the values.
                FALSE: None of the values are changed, since some parameter
                       is having a wrong value.
                Values are returned through arguments.
  
  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_set_all_cur_naks
(
  byte rscb_index,                       /* RLP instance                   */
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Confirm that all the input paramters are in the range. If they are not
    in range then return FALSE.
    Check for nak_rounds_fwd to be > dsrlp3_constants max_nak_rounds instead 
    of the DSRLP_MAX_NAK_ROUNDS because, in a call, 
    We should not set a value higher than negotiated value. Same for reverse 
    rounds also.
	
	rscb_index:         Should be less than Max_RSCB_Elements
	nak_rounds_fwd:     Should not be NULL
	*nak_rounds_fwd:    Should be less than Maximum allowed and negotiated
                            values. Though we can assume that negotiated value
                            will be less than MAX, we put extra check to test 
                            for MAX also.
	naks_per_round_fwd: Should not be NULL
	nak_rounds_rev:     Should not be NULL
	*nak_rounds_rev:    Should be less than Max value and Negotiated value
    naks_per_round_rev: Should not be NULL
  -------------------------------------------------------------------------*/
  if(
     (rscb_index >= DSRLPI_MAX_RSCB_ELEMENTS)                    ||
     (nak_rounds_fwdp == NULL)                                   ||
     (*nak_rounds_fwdp > dsrlp3_constants.max_nak_rounds_fwd)    ||
     (*nak_rounds_fwdp > dsrlp3_opt.negotiated_nak_rounds_fwd)   ||
     (naks_per_round_fwdp == NULL)                               ||
     (nak_rounds_revp == NULL)                                   ||
     (*nak_rounds_revp > dsrlp3_constants.max_nak_rounds_rev)    ||
     (*nak_rounds_revp > dsrlp3_opt.negotiated_nak_rounds_rev)   ||
     (naks_per_round_revp == NULL)
    )
    {
      return FALSE;
    } 
  /*-------------------------------------------------------------------------
    Confirm that the values are within the range.
  -------------------------------------------------------------------------*/
  for (i=0; i<(*nak_rounds_fwdp); i++)
  {
     if( (naks_per_round_fwdp[i] > DSRLP_MAX_NAKS_PER_ROUND) ||
         (naks_per_round_fwdp[i] > dsrlp3_opt.negotiated_naks_per_round_fwd[i])
       )
     {
         return FALSE;
     }  
  }  

  for (i=0; i<(*nak_rounds_revp); i++)
  {
     if ( (naks_per_round_revp[i] > DSRLP_MAX_NAKS_PER_ROUND) ||
          (naks_per_round_fwdp[i] > dsrlp3_opt.negotiated_naks_per_round_rev[i])
        )
     {
         return FALSE;
     }                
  }  

  /*-------------------------------------------------------------------------
    Obtain the rscb_index 
  -------------------------------------------------------------------------*/
  dsrlp3_opt.rscb_index = rscb_index;
  
  /*-------------------------------------------------------------------------
    Update the Forward NAK rounds for current call and also for future calls
  -------------------------------------------------------------------------*/
  dsrlp3_constants.desired_nak_rounds_fwd = *nak_rounds_fwdp;

  /*-------------------------------------------------------------------------
    Update the Forward NAKs per round for current call and also for future 
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i< (*nak_rounds_fwdp); i++)
  {
    dsrlp3_constants.desired_naks_per_round_fwd[i] = naks_per_round_fwdp[i];
  }
     
  /*-------------------------------------------------------------------------
    Update the Reverse NAK rounds for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  dsrlp3_constants.desired_nak_rounds_rev = (*nak_rounds_revp);
     
  /*-------------------------------------------------------------------------
    Update the Reverse NAKs per round for current call and also for future 
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i<(*nak_rounds_revp); i++)
  {
    dsrlp3_constants.desired_naks_per_round_rev[i] = naks_per_round_revp[i];
  }        

  dsrlp3_opt.update_state_var = TRUE;
  return TRUE;

} /* dsrlp_set_all_cur_naks */

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
boolean dsrlp_get_all_neg_naks
( 
  byte *nak_rounds_fwdp,                 /* Num of rounds for forward NAKs */
  byte *naks_per_round_fwdp,             /* Num of NAKs per round forward  */
  byte *nak_rounds_revp,                 /* Num of rounds for reverse NAKs */
  byte *naks_per_round_revp              /* Num of NAKs per round reverse  */
)
{
  int i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Confirm that all the input paramters are in the range. If they are not
    In range then return FALSE.
  -------------------------------------------------------------------------*/
  if ( 
      (nak_rounds_fwdp     == NULL)     ||
      (naks_per_round_fwdp == NULL)     ||
      (nak_rounds_revp     == NULL)     ||
      (naks_per_round_revp == NULL)
     )
  {
     return FALSE;
  }
     
  
  /*-------------------------------------------------------------------------
    Copy the parameters for forward options
  -------------------------------------------------------------------------*/
  *nak_rounds_fwdp = dsrlp3_opt.negotiated_nak_rounds_fwd;
  for (i=0; i< (*nak_rounds_fwdp); i++)
  {
     naks_per_round_fwdp[i] = dsrlp3_opt.negotiated_naks_per_round_fwd[i];
  }

  /*-------------------------------------------------------------------------
    Copy the parameters for reverse options
  -------------------------------------------------------------------------*/
  *nak_rounds_revp = dsrlp3_opt.negotiated_nak_rounds_rev;
  for (i=0; i< (*nak_rounds_revp); i++)
  {
     naks_per_round_revp[i] = dsrlp3_opt.negotiated_naks_per_round_rev[i];
  }
     
  /*-------------------------------------------------------------------------
    Since we give the information that caller requested, return TRUE
  -------------------------------------------------------------------------*/
  return TRUE;

} /* dsrlp_get_all_neg_naks */


/*===========================================================================
  FUNCTION      DSRLP_OPT_UPDATE_STATE
 
  DESCRIPTION  Update the state variables for the NAK scheme.  
  
  DEPENDENCIES  None
  
  RETURN VALUE  None
  
  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_opt_update_state(void)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;       /* rlp-specific control block */
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Obtain the rscb_ptr and confirm that its not NULL
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[dsrlp3_opt.rscb_index]);
  ASSERT(rscb_ptr != NULL);
  
  /*-------------------------------------------------------------------------
    Update the Forward NAK rounds for current call and also for future calls
  -------------------------------------------------------------------------*/
  rscb_ptr->state.nak_rounds_fwd = dsrlp3_constants.desired_nak_rounds_fwd;

  /*-------------------------------------------------------------------------
    Update the Forward NAKs per round for current call and also for future 
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i< dsrlp3_constants.desired_nak_rounds_fwd; i++)
  {
    rscb_ptr->state.naks_per_round_fwd[i] = 
        dsrlp3_constants.desired_naks_per_round_fwd[i];
	MSG_MED("NAKS_PER_ROUND_FWD is SET to %d.\n", 
			 rscb_ptr->state.naks_per_round_fwd[i], 0, 0);

  }
     
  /*-------------------------------------------------------------------------
    Update the Reverse NAK rounds for current call and also for future
    calls.
  -------------------------------------------------------------------------*/
  rscb_ptr->state.nak_rounds_rev = dsrlp3_constants.desired_nak_rounds_rev;
     
  /*-------------------------------------------------------------------------
    Update the Reverse NAKs per round for current call and also for future 
    calls.
  -------------------------------------------------------------------------*/
  for (i=0; i<dsrlp3_constants.desired_nak_rounds_rev; i++)
  {
    rscb_ptr->state.naks_per_round_rev[i] = 
        dsrlp3_constants.desired_naks_per_round_rev[i];
	MSG_MED("NAKS_PER_ROUND_REV is SET to %d.\n", 
			 rscb_ptr->state.naks_per_round_rev[i], 0, 0);
  }        

  dsrlp3_opt.update_state_var = FALSE;

} /* dsrlpi_opt_update_state */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================
FUNCTION      DSRLP_QOS_INIT

DESCRIPTION   Initializes the RLP QOS fields

DEPENDENCIES  Should be initialized before making the first data call. So
              power on initialization is the best place.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void dsrlp_qos_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Default values. In case of non-assured mode, default value of priority
   adjustment is subscription priority value as defined in IS-707 A2.12, ie
   no adjustment to priority.
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.defaults.qos_incl= TRUE;
  dsrlp_qos_values.defaults.max_blob_type_supp_incl = TRUE;
  dsrlp_qos_values.defaults.blob_type_incl =          TRUE;
  dsrlp_qos_values.defaults.max_blob_type_supported =
                                        DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_values.defaults.assur_mode = FALSE;
  dsrlp_qos_values.defaults.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_values.defaults.params.non_assur_params.pri_adj
   = DSRLP_QOS_NON_ASSUR_DEFAULT_PRI_ADJ;

  /*-------------------------------------------------------------------------
   Desired values not to be used unless requested by application,
   so qos_incl set to FALSE,  but for safety, other fields
   are initialized to default values anyway
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.desired.qos_incl = FALSE;
  dsrlp_qos_values.desired.max_blob_type_supp_incl = TRUE;
  dsrlp_qos_values.desired.blob_type_incl =          TRUE;
  dsrlp_qos_values.desired.max_blob_type_supported =
                                        DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_values.desired.blob_type = DSRLP_QOS_DEFAULT_BLOB_TYPE;
  dsrlp_qos_values.desired.assur_mode
  = dsrlp_qos_values.defaults.assur_mode;
  dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_values.desired.params.non_assur_params.pri_adj
  = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj;

  /*-------------------------------------------------------------------------
   Rx from bs values not to be used unless actually received from
   base-station, so qos_incl set to FALSE, but for safety, other fields
   are initialized to default values anyway
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.rx_from_bs.qos_incl = FALSE;
  dsrlp_qos_values.rx_from_bs.max_blob_type_supp_incl = TRUE;
  dsrlp_qos_values.rx_from_bs.blob_type_incl =          TRUE;
  dsrlp_qos_values.rx_from_bs.max_blob_type_supported =
                                        DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_values.rx_from_bs.blob_type = DSRLP_QOS_DEFAULT_BLOB_TYPE;
  dsrlp_qos_values.rx_from_bs.assur_mode
  = dsrlp_qos_values.defaults.assur_mode;
  dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj
  = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj;

  /*-------------------------------------------------------------------------
   Curr QOS values ptr is initialized to point to default
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.curr_p  =
                      &(dsrlp_qos_values.defaults);

} /* dsrlp_qos_init() */

/*===========================================================================
  FUNCTION      DSRLP_BUILD_QOS_BLOB

  DESCRIPTION   This function creates a qos blob to be sent by mobile to BS

  DEPENDENCIES  None

  RETURN VALUE  Length of blob (0 if no blob needs to be sent, ie default)
                Blob is returned by argument pointer

  SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_build_qos_blob
(
   byte *qos_blob_p                                      /* Resulting blob */
)
{
  byte   blob_len = 0;           /* holds the calculated BLOB length        */
  byte   new_byte_value = 0;     /* holds the value of the next byte        */
  word   field_length  = 0;      /* holds the field length for next field   */
  word   field_position = 0;     /* holds a cumulative number of bytes used */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(qos_blob_p != NULL);

  /*-------------------------------------------------------------------------
    Only if desired values are set, blob has to be sent.
  -------------------------------------------------------------------------*/
  if ((dsrlp_qos_values.desired.qos_incl) == TRUE)
  {
    if (dsrlp_are_qos_values_same
           (
             &(dsrlp_qos_values.defaults),
             &(dsrlp_qos_values.desired)
           )
        )
     {
       /*--------------------------------------------------------------------
         If desired values set but there's no diff betw default,
         blob doesn't have to be sent.
       --------------------------------------------------------------------*/
       blob_len = 0;
     }
     else
     {
       /*--------------------------------------------------------------------
         Blob needs to be built from desired values.
       --------------------------------------------------------------------*/
       memset(qos_blob_p, 0, DSRLP_MOBILE_QOS_BLOB_LEN_MAX);

       field_position    += field_length;

       /*--------------------------------------------------------------------
         Prepare QOS_BLOB_TYPE_INCL field (2 bits).
       --------------------------------------------------------------------*/
       field_length       = 2;
       if (   ( dsrlp_qos_values.desired.max_blob_type_supp_incl == FALSE )
           && (dsrlp_qos_values.desired.blob_type_incl == FALSE ) )
       {
         new_byte_value = 0x0;
       }
       else if (( dsrlp_qos_values.desired.max_blob_type_supp_incl == FALSE )
            &&  (dsrlp_qos_values.desired.blob_type_incl == TRUE ) )
       {
         new_byte_value = 0x1;
       }
       else if (( dsrlp_qos_values.desired.max_blob_type_supp_incl == TRUE )
            &&  (dsrlp_qos_values.desired.blob_type_incl == FALSE ) )
       {
         new_byte_value = 0x2;
       }
       else if (( dsrlp_qos_values.desired.max_blob_type_supp_incl == TRUE )
            &&  (dsrlp_qos_values.desired.blob_type_incl == TRUE ) )
       {
         new_byte_value = 0x3;
       }
       b_packb
           ( new_byte_value,
             qos_blob_p,
             field_position,
             field_length
           );
      field_position    += field_length;


       /*--------------------------------------------------------------------
         Prepare MAX_QOS_BLOB_TYPE_SUPPORTED field (4 bits).
       --------------------------------------------------------------------*/
       if ( dsrlp_qos_values.desired.max_blob_type_supp_incl == TRUE )
       {
         field_length       = 4;
         new_byte_value = dsrlp_qos_values.desired.max_blob_type_supported;
         b_packb
             ( new_byte_value,
               qos_blob_p,
               field_position,
               field_length
             );
       }
       field_position    += field_length;

       /*--------------------------------------------------------------------
         Prepare QOS_BLOB_TYPE field (4 bits).
       --------------------------------------------------------------------*/
       if ( dsrlp_qos_values.desired.blob_type_incl == TRUE )
       {
         field_length       = 4;
         new_byte_value = dsrlp_qos_values.desired.blob_type;
         b_packb
             ( new_byte_value,
               qos_blob_p,
               field_position,
               field_length
             );
       }
       field_position    += field_length;


       if ( dsrlp_qos_values.desired.assur_mode == TRUE)
       {
         /*------------------------------------------------------------------
           Assured mode not yet implemented.
          -----------------------------------------------------------------*/
         blob_len = 0;
       }
       else  
       {
         /*------------------------------------------------------------------
           Non-assured mode 
          -----------------------------------------------------------------*/

          /*-----------------------------------------------------------------
            Prepare MODE field (1 bit) with value 0.
          -----------------------------------------------------------------*/
          field_length       = 1;
          new_byte_value     = 0x0;
          b_packb
              ( new_byte_value,
                qos_blob_p,
                field_position,
                field_length
              );
          field_position    += field_length;

          /*-----------------------------------------------------------------
            Prepare NON_ASSURED_PRI_ADJ_INCL field ( 1 bit ).
          -----------------------------------------------------------------*/
          field_length       = 1;
          if ( dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl)
          {
            new_byte_value     = 0x1;
          }
          else
          {
            new_byte_value     = 0x0;
          }
          b_packb
              ( new_byte_value,
                qos_blob_p,
                field_position,
                field_length
              );
          field_position    += field_length;

          /*-----------------------------------------------------------------
            Prepare NON_ASSURED_PRI_ADJ field (4 bits)
          -----------------------------------------------------------------*/
          if ( dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl)
          {
            field_length       = 4;
            new_byte_value     =
                   dsrlp_qos_values.desired.params.non_assur_params.pri_adj;
            b_packb
                ( new_byte_value,
                  qos_blob_p,
                  field_position,
                  field_length
                );
            field_position += field_length;
          }
       }

      /*---------------------------------------------------------------------
        Calculate the BLOB length in bytes
       --------------------------------------------------------------------*/
       if ( field_position % 8 == 0)
       {
         blob_len = field_position / 8;
       }
       else
       {
         blob_len = (byte)(field_position/8) + 1;
       }

     } /* blob needs to be built from desired values */

  } /* desired values have been set */

  return blob_len;

} /* dsrlp_build_qos_blob() */


/*===========================================================================
  FUNCTION      DSRLP_PROCESS_QOS_BLOB

  DESCRIPTION   This function processes the qos blob rx from BS

  DEPENDENCIES  None

  RETURN VALUE
              TRUE:  If we were able to get the values.
              FALSE: If blob didn't have qos params.

  SIDE EFFECTS  The rx_from_bs Qos value struct is filled up.
===========================================================================*/
boolean dsrlp_process_qos_blob
(
   const byte *rx_qos_blob_p,                               /* Ptr to blob */
   byte blob_len
)
{
  byte    temp_qos_blob[DSRLP_BS_QOS_BLOB_LEN_MAX];
  word    field_length       = 0;     /* holds the length of the field     */
  word    field_position     = 0;     /* cumulative count of bytes         */
  byte    new_byte;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    For safety, ensure blob length is within acceptable range
  -------------------------------------------------------------------------*/
  if (  ( (int) blob_len == 0 )
     || ( blob_len > (byte) DSRLP_BS_QOS_BLOB_LEN_MAX ) )
  {
    return FALSE;
  }

  ASSERT(rx_qos_blob_p != NULL);

  /*-------------------------------------------------------------------------
    Copy blob into a temp variable that we can work with as blob is a ptr to
    a const param
  -------------------------------------------------------------------------*/
  memcpy (
           (void *) temp_qos_blob,
           rx_qos_blob_p,
           (size_t) blob_len
         );

  dsrlp_qos_values.rx_from_bs.max_blob_type_supp_incl = TRUE;
  dsrlp_qos_values.rx_from_bs.blob_type_incl = TRUE;

  /*-------------------------------------------------------------------------
    Process MAX_QOS_BLOB_TYPE_SUPPORTED field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length = 4;
  new_byte     = b_unpackb(temp_qos_blob, field_position, field_length);
  dsrlp_qos_values.rx_from_bs.max_blob_type_supported = new_byte;

  /*-------------------------------------------------------------------------
    Process QOS_BLOB_TYPE field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length = 4;
  new_byte     = b_unpackb(temp_qos_blob, field_position, field_length);
  dsrlp_qos_values.rx_from_bs.blob_type = new_byte;

  /*-------------------------------------------------------------------------
    Process ASSURED_MODE field
  -------------------------------------------------------------------------*/
  field_position += field_length;
  field_length = 1;
  new_byte     = b_unpackb(temp_qos_blob, field_position, field_length);
  if ( new_byte == 1 )
  {
     /*----------------------------------------------------------------------
       Assured mode not yet implemented
     ----------------------------------------------------------------------*/
    dsrlp_qos_values.rx_from_bs.assur_mode = TRUE;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_values.rx_from_bs.assur_mode = FALSE;
    /*-----------------------------------------------------------------------
      If BS has sent a QOS blob and it's non-assured mode, we assume
      BS accepted the requested value of priority. If there wasn't a 
      requested value, then we revert to default value.
    -----------------------------------------------------------------------*/
    if ( dsrlp_qos_values.desired.qos_incl == TRUE )
    {
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj_incl
        = dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl;
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj
        = dsrlp_qos_values.desired.params.non_assur_params.pri_adj;
    }
    else
    {
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj_incl
        = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj_incl;
      dsrlp_qos_values.rx_from_bs.params.non_assur_params.pri_adj
        = dsrlp_qos_values.defaults.params.non_assur_params.pri_adj;
    }
  }

  return TRUE;
} /* dsrlp_process_qos_blob() */

/*===========================================================================
  FUNCTION      DSRLP_CALC_CURR_QOS_VALUES

  DESCRIPTION   This function calculates and stores current qos values
                based on desired values requested and the values received
                from BS.

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_calc_curr_qos_values(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
   Logic is that if BS hasn't sent QOS blob, default values are assumed.
   But if BS has sent QOS blob, those are considered the current values.
   Refer IS-707 A-2.12
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
   curr values ptr initialized to point to default values
  -------------------------------------------------------------------------*/
  dsrlp_qos_values.curr_p  =
                      &(dsrlp_qos_values.defaults);


  /*-------------------------------------------------------------------------
   If qos blob has been received from BS, curr values ptr should point
   to rx_from_bs values.
  -------------------------------------------------------------------------*/
  if ( dsrlp_qos_values.rx_from_bs.qos_incl == TRUE )
  {
    dsrlp_qos_values.curr_p  =
                      &(dsrlp_qos_values.rx_from_bs);
  }

} /* dsrlp_calc_curr_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_GET_CURR_QOS_VALUES

  DESCRIPTION   This function returns current QOS values

  DEPENDENCIES  None

  RETURN VALUE  Current values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_get_curr_qos_values
(
   dsrlp_qos_s_type *dsrlp_qos_s_p             /* Curr Qos values returned */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_qos_s_p->qos_incl =
    (dsrlp_qos_values.curr_p)->qos_incl;
  dsrlp_qos_s_p->max_blob_type_supp_incl =
    (dsrlp_qos_values.curr_p)->max_blob_type_supp_incl;
  dsrlp_qos_s_p->blob_type_incl =
    (dsrlp_qos_values.curr_p)->blob_type_incl;
  dsrlp_qos_s_p->max_blob_type_supported =
    (dsrlp_qos_values.curr_p)->max_blob_type_supported;
  dsrlp_qos_s_p->blob_type =
    (dsrlp_qos_values.curr_p)->blob_type;
  dsrlp_qos_s_p->assur_mode =
    (dsrlp_qos_values.curr_p)->assur_mode;

  if ( dsrlp_qos_s_p->assur_mode == TRUE )
  {
    /*-----------------------------------------------------------------------
      Assured mode not yet implemented
    -----------------------------------------------------------------------*/
  }
  else
  {
    /*-----------------------------------------------------------------------
      Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_s_p->params.non_assur_params.pri_adj_incl =
    (dsrlp_qos_values.curr_p)->params.non_assur_params.pri_adj_incl;
    dsrlp_qos_s_p->params.non_assur_params.pri_adj =
    (dsrlp_qos_values.curr_p)->params.non_assur_params.pri_adj;
  }

} /* dsrlp_get_curr_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_GET_DESIRED_QOS_VALUES

  DESCRIPTION   This function returns desired QOS values

  DEPENDENCIES  None

  RETURN VALUE  Desired values of Qos  are returned as an argument pointer

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_get_desired_qos_values
(
   dsrlp_qos_s_type *dsrlp_qos_s_p          /* Desired Qos values returned */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_qos_s_p->qos_incl =
    dsrlp_qos_values.desired.qos_incl;
  dsrlp_qos_s_p->max_blob_type_supp_incl =
    dsrlp_qos_values.desired.max_blob_type_supp_incl;
  dsrlp_qos_s_p->blob_type_incl =
    dsrlp_qos_values.desired.blob_type_incl;
  dsrlp_qos_s_p->max_blob_type_supported =
    dsrlp_qos_values.desired.max_blob_type_supported;
  dsrlp_qos_s_p->blob_type =
    dsrlp_qos_values.desired.blob_type;
  dsrlp_qos_s_p->assur_mode =
    dsrlp_qos_values.desired.assur_mode;

  if ( dsrlp_qos_s_p->assur_mode == TRUE )
  {
    /*-----------------------------------------------------------------------
        Assured mode not yet implemented
    -----------------------------------------------------------------------*/
  }
  else
  {
    /*-----------------------------------------------------------------------
        Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_s_p->params.non_assur_params.pri_adj_incl =
     dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl;
    dsrlp_qos_s_p->params.non_assur_params.pri_adj =
     dsrlp_qos_values.desired.params.non_assur_params.pri_adj;
  }

} /* dsrlp_get_desired_qos_values() */

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_VALUES

  DESCRIPTION   This function sets desired Qos values

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_set_desired_qos_values
(
   dsrlp_qos_s_type *dsrlp_qos_s_p                   /* Desired Qos values */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  dsrlp_qos_values.desired.qos_incl
    = dsrlp_qos_s_p->qos_incl;
  dsrlp_qos_values.desired.max_blob_type_supp_incl =
    dsrlp_qos_s_p->max_blob_type_supp_incl;
  dsrlp_qos_values.desired.blob_type_incl =
    dsrlp_qos_s_p->blob_type_incl;
  dsrlp_qos_values.desired.max_blob_type_supported =
    dsrlp_qos_s_p->max_blob_type_supported;
  dsrlp_qos_values.desired.blob_type =
    dsrlp_qos_s_p->blob_type;
  dsrlp_qos_values.desired.assur_mode
    = dsrlp_qos_s_p->assur_mode;

  if ( dsrlp_qos_s_p->assur_mode == TRUE )
  {
    /*-----------------------------------------------------------------------
        Assured mode not yet implemented
    -----------------------------------------------------------------------*/
  }
  else
  {
    /*-----------------------------------------------------------------------
        Non-assured mode
    -----------------------------------------------------------------------*/
    dsrlp_qos_values.desired.params.non_assur_params.pri_adj_incl
      = dsrlp_qos_s_p->params.non_assur_params.pri_adj_incl;
    dsrlp_qos_values.desired.params.non_assur_params.pri_adj
      = dsrlp_qos_s_p->params.non_assur_params.pri_adj;
  }
} /* dsrlp_set_desired_qos_values() */

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
boolean dsrlp_get_desired_qos_non_assur_pri
(
   byte *pri_adj_value_p           /* Desired value of priority adjustment */
)
{
  dsrlp_qos_s_type dsrlp_qos_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_get_desired_qos_values ( &dsrlp_qos_s );

  /*------------------------------------------------------------------------
    If desired values weren't set or if they were set but they were assured
    mode, return error.
  ------------------------------------------------------------------------*/
  if (
      ( dsrlp_qos_s.qos_incl == FALSE ) ||
      ( dsrlp_qos_s.assur_mode == TRUE )
     )
  {
     return FALSE;
  }
  if ( dsrlp_qos_s.params.non_assur_params.pri_adj_incl == FALSE )
  {
    return FALSE;
  }

  *pri_adj_value_p = dsrlp_qos_s.params.non_assur_params.pri_adj;

  return TRUE;

} /* dsrlp_get_desired_qos_non_assur_pri() */

/*===========================================================================
  FUNCTION      DSRLP_SET_DESIRED_QOS_NON_ASSUR_PRI

  DESCRIPTION   This function sets desired value of QOS non-assured mode
                priority adjustment

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_set_desired_qos_non_assur_pri
(
   byte pri_adj_value              /* Desired value of priority adjustment */
)
{
  dsrlp_qos_s_type dsrlp_qos_s;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  dsrlp_qos_s.qos_incl = TRUE;
  dsrlp_qos_s.assur_mode = FALSE;
  dsrlp_qos_s.max_blob_type_supp_incl = TRUE;
  dsrlp_qos_s.blob_type_incl = TRUE;
  dsrlp_qos_s.max_blob_type_supported = DSRLP_QOS_MAX_BLOB_TYPE_SUPPORTED;
  dsrlp_qos_s.blob_type = DSRLP_QOS_DEFAULT_BLOB_TYPE;
  dsrlp_qos_s.params.non_assur_params.pri_adj_incl = TRUE;
  dsrlp_qos_s.params.non_assur_params.pri_adj = pri_adj_value;

  dsrlp_set_desired_qos_values ( &dsrlp_qos_s );
} /* dsrlp_set_desired_qos_non_assur_pri() */


/*===========================================================================
  FUNCTION      DSRLP_ARE_QOS_VALUES_SAME

  DESCRIPTION   This function takes as argument two qos structures and
                compares whether they are the same. (Eg: useful for
                checking if desired values equals default values)

  DEPENDENCIES  If either argument has field qos_incl false, ie it doesn't
                care about Qos, function returns TRUE

  RETURN VALUE
              TRUE:  If values are identical in both qos structures
              FALSE: If values are not identical

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_are_qos_values_same
(
   dsrlp_qos_s_type *dsrlp_qos_s_1_p,       /* Ptr to struct of qos params */
   dsrlp_qos_s_type *dsrlp_qos_s_2_p        /* Ptr to struct of qos params */
)
{
  boolean result = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Only meaningful if both of them have qos included
  -------------------------------------------------------------------------*/
  if (
      (dsrlp_qos_s_1_p->qos_incl) &&
      (dsrlp_qos_s_2_p->qos_incl)
     )
  {
    if (
        (dsrlp_qos_s_1_p->assur_mode) !=
        (dsrlp_qos_s_2_p->assur_mode)
       )
    {
      /*---------------------------------------------------------------------
        One is assured, and the other is non-assured
      ---------------------------------------------------------------------*/
      result = FALSE;
    }
    else
    {
      if ( (dsrlp_qos_s_1_p->assur_mode) == FALSE ) 
      {
        /*-------------------------------------------------------------------
           Non-assured mode 
        -------------------------------------------------------------------*/

        /*-------------------------------------------------------------------
          Check value of priority adj field 
         ------------------------------------------------------------------*/
        if (
             ( dsrlp_qos_s_1_p->params.non_assur_params.pri_adj_incl == TRUE)
           &&( dsrlp_qos_s_2_p->params.non_assur_params.pri_adj_incl == TRUE)
           )
        {
          if ((dsrlp_qos_s_1_p->params.non_assur_params.pri_adj) ==
              (dsrlp_qos_s_2_p->params.non_assur_params.pri_adj))
            result = TRUE;
          else
            result = FALSE;
        }
        else
        {
          result = FALSE;
        }
      }
      else 
      {
         /*------------------------------------------------------------------
           Assured mode not yet implemented
         ------------------------------------------------------------------*/
        result = FALSE;
      }
    }
  }
  return result;
} /* dsrlp_are_qos_values_same() */

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

boolean dsrlp_any_sig_to_send(dsrlp_tx_sig_enum_type *priority)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr;        /* rlp-specific control block */
  boolean                  ret_val  = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  /*-------------------------------------------------------------------------
      Find out if MS RLP has established. 
           If no, then we are in process of establishing. So, DS should get a 
           chance on FCH to send signaling data continously to calculate RTT.
           
      If MS has already established RLP, 
           then see if we have to send any Naks. We should be able to transmit
           the NAK's immediately (1/2 rate is sufficient).
           
  -------------------------------------------------------------------------*/
  if (rscb_ptr->state.state != DSRLPI_ESTABLISHED_STATE)
  {
     *priority = DSRLP_TX_CONTINUOUS;
     ret_val = TRUE;
     MSG_MED(" RLP Data : SYNC :%d , RTT:%d", rscb_ptr->state.state,
                                        rscb_ptr->state.rlp_delay,0);
  }
  else if (rscb_ptr->state.ack_to_send == TRUE) 
  {
     *priority = DSRLP_TX_NO_DELAY;
     ret_val = TRUE;
     MSG_MED(" RLP Data : ack_to_send = TRUE",0,0,0);
  }
  else if(rscb_ptr->state.naks_to_send == TRUE)
  {
     *priority = DSRLP_TX_NO_DELAY;
     ret_val = TRUE;
  }
  else if(rscb_ptr->state.idle_timer == 0)
  {
   /*--------------------------------------------------------------------------
     If the idle timer expires we want to send the idle frame right away.
    ---------------------------------------------------------------------------*/
     *priority = DSRLP_TX_NO_DELAY;
     ret_val = TRUE;
  }
  else 
  {
     *priority =  DSRLP_TX_NONE;
  }
  return (ret_val);

} /* dsrlp_any_sig_to_send() */

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
snm_eval_status_type dsrlp_eval_qos_blob( byte   bs_blob_len,
                                          byte * bs_blob,
                                          boolean can_suggest_alt,
                                          byte * ret_alt_blob_len,
                                          byte * ret_alt_blob
                                         )
{
    return (SNM_ACCEPT_CFG);
}

/*===========================================================================
  FUNCTION      DSRLP_GET_NEXT_DSM_ITEM_CHAIN

  DESCRIPTION   This function takes the pointer to the current beginning of
                a chain of DSM items, which contains RLP payload to be
                transmitted by the MUX layer, as input, and returns pointer
                to the next item in chain. If there is no more item in
                chain, NULL is returned.


  DEPENDENCIES  None

  RETURN VALUE  Pointer to the next item in chain; if there is no more
                item in chain, return NULL.

  SIDE EFFECTS  None
===========================================================================*/
dsm_item_type *dsrlp_get_next_dsm_item_chain( dsm_item_type *cur_ptr_to_payload )
{
   /*------------------------------------------------------------------------
      Assert that the pointer to the current head of the chain should not
      be NULL.
   ------------------------------------------------------------------------*/
   if (NULL == cur_ptr_to_payload)
   {
      ASSERT (0);
      return (NULL);
   }
   /*-----------------------------------------------------------------------
      If there is more DSM item in chain, return the pointer; otherwise
      return NULL to indicate the end of the chain
   -----------------------------------------------------------------------*/
   return (cur_ptr_to_payload->pkt_ptr);
} /* dsrlp_get_next_dsm_item_chain */
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
)
{
  dsrlpi_sess_ctl_blk_type       *rscb_ptr;  /* rlp-specific control block */

  ASSERT(rscb_index < DSRLPI_MAX_RSCB_ELEMENTS);
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  if( rscb_ptr->ver_ptr == NULL)
  {
    return(FALSE);
  }

  else
  {
    return(TRUE);
  }
}

/*===========================================================================
  FUNCTION      DSRLP_INSERT_PDU5NULL_HDR_TURBO

  DESCRIPTION   This function will insert Null header for the Turbo
                encoded PDUs.

  DEPENDENCIES  

  RETURN VALUE  

  SIDE EFFECTS  
===========================================================================*/

void dsrlp_insert_pdu5null_hdr_turbo(dsm_item_type *out_pkt_ptr)
{               
    byte temp_data_arr[3];       /* Array that will hold the first 3 bytes 
                                    of outgoing Mux PDU5. Size is 3 bytes 
                                    because max length of PDU5 can be  
                                    obtained from 3 bytes                  */
    byte data_len;               /* Indicates # of bytes copied from PDU   */
    uint16 pdu5_len  = 0;        /* Length of the PDU */
    uint16 pdu5_temp = 0;        /* Temp variable used to calculate Length */
    uint16 offset    = 0;        /* variable used to hold offset where the 
                                    PDU payload starts. For 8 bit length 
                                    offset has a value of 2; for 16bit it 
                                    has offset of 3                        */
    byte pkt_len_indicator;      /* Length indicator is in the first 
                                    location 
                                    If length indicator == 00 
                                     then we have fixed length. 
                                    else if (length indicator == 01)  
                                     then we have 8 bits length
                                    else if (length indicator == 10) 
                                     then we have 16 bits length           */
    dsm_item_type *temp_pkt_ptr; /* used to hold input pkt ptr for 
                                    processing                             */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     Make a copy; So that we operate on "copied ptr" and avoid any
       changes to passed pointer.
    -----------------------------------------------------------------------*/
    temp_pkt_ptr = out_pkt_ptr;

    /*----------------------------------------------------------------------- 
      Extract the data from the passed pointer. Since the length information
      is only in first 3 bytes; extract only first 3 bytes.
      Confirm that we got the 3 bytes. If we did not get them, some thing is
      amiss; return after printing error.
    -----------------------------------------------------------------------*/
    data_len = dsm_extract(temp_pkt_ptr,0,temp_data_arr,3);
    if (data_len !=3) 
    {
        MSG_ERROR("Not enough data to insert Null Hdr",0,0,0);
        return;
    }
    
    /*----------------------------------------------------------------------- 
      Get the Length Indicator for this PDU. The length indicator is the bits
      5,6 from MS Byte i.e. xxxx LLxx. The LL bits are length indicator
    -----------------------------------------------------------------------*/
    pkt_len_indicator = (temp_data_arr[0] & RLP3_MUXPDU5_LEN_IND_MASK); 

    /*----------------------------------------------------------------------- 
      See if the length indicator is xxxx01xx i.e. 1 byte.
      If yes, then extract the length from the array into pdu5_len.
    -----------------------------------------------------------------------*/
    if (pkt_len_indicator == 0x04) 
    {
        MSG_LOW("Frame with Len Indicator 01 ",0,0,0); 
        /*-------------------------------------------------------------------
          Extract the length (x: Don't care; L: Length)
          temp_data_array[0]: xxxx xxLL 
          temp_data_array[1]: LLLL LLxx
          Combine both the length segments into one variable, pdu5_len
        -------------------------------------------------------------------*/
        pdu5_temp = (temp_data_arr[0] & 0x03);
        pdu5_len  = (pdu5_temp << 6);
        pdu5_temp = temp_data_arr[1];
        pdu5_len  |= (pdu5_temp >> 2);

        /*-------------------------------------------------------------------
          Since the length is total number of bytes after the length
          field, we should have an offset so that we poke NULL PDU HDR
          (0xE0) at the right place
          Since we have two bytes before start of MuxPDU5 payload, the
          offset is 2.
        -------------------------------------------------------------------*/
        offset    = 2;
    }
    else if (pkt_len_indicator == 0x08) 
    {
        
        MSG_LOW("Frame with Len Indicator 10 ",0,0,0); 

        /*-----------------------------------------------------------------
          Now obtain the length of the PDU5 Extract the length
           (x: Don't care; L: Length)
          temp_data_array[0]: xxxx xxLL 
          temp_data_array[1]: LLLL LLLL
          temp_data_array[2]: LLLL LLxx
         Combine both the length segments into one variable, pdu5_len
        -----------------------------------------------------------------*/
        pdu5_temp = (temp_data_arr[0] & 0x03); 
        pdu5_len  = (pdu5_temp << 14);
        pdu5_temp = temp_data_arr[1];
        pdu5_len  |= (pdu5_temp << 6);
        pdu5_temp = (temp_data_arr[2] & 0xFF);
        pdu5_len  |= (pdu5_temp >> 2);

        /*-------------------------------------------------------------------
          Since the length is total number of bytes after the length
          field, we should have an offset so that we poke NULL PDU HDR
          (0xE0) at the right place
          Since we have two bytes before start of MuxPDU5 payload, the
          offset is 3.
        -------------------------------------------------------------------*/
        offset    = 3;
    }
    else if (pkt_len_indicator == 0x0C) 
    {
       /*-------------------------------------------------------------------- 
          Should not come here. Since we came here anyway, let's print out 
          error and return.
          Expect that some other entity should catch this problem and fix
          the code.
       --------------------------------------------------------------------*/
       MSG_ERROR("Invalid Len Indicator %x",pkt_len_indicator,0,0);
       return;
    }
    else
    {
        /*------------------------------------------------------------------- 
          Nothing to do return 
        -------------------------------------------------------------------*/
       return;
    }

    /*----------------------------------------------------------------------- 
      The exact place where null pdu header should be added is given by
      Offset + PDU5_len ;
    -----------------------------------------------------------------------*/
    offset += pdu5_len;

    /*----------------------------------------------------------------------- 
      By now we obtained the Length of the PDU. So we should poke the Null 
      PDU Header byte (0xE0) at offset
      
      While (offset > size), 
         obtain next ptr and reduce the offset. 
         
      Note,  look at used field but should look at the size field.
    -----------------------------------------------------------------------*/
    temp_pkt_ptr = out_pkt_ptr;
    while ( (temp_pkt_ptr != NULL) && (offset >= temp_pkt_ptr->size)) 
    {
        temp_pkt_ptr = temp_pkt_ptr->pkt_ptr;
        offset -= out_pkt_ptr->size;
    }

    /*----------------------------------------------------------------------- 
      Insert the Null PDU header
      For safety, confirm that the pkt_ptr is not NULL
    -----------------------------------------------------------------------*/
    if (temp_pkt_ptr != NULL) 
    {
        temp_pkt_ptr->data_ptr[offset] = 0xE0;
    }
    else
    {
        MSG_ERROR("offset:%x,Out_ptr:%x;Should not come here",offset,out_pkt_ptr,0);
    }         
} /* dsrlp_insert_pdu5null_hdr_turbo */


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
)
{
  dsrlpi_sess_ctl_blk_type  *rscb_ptr;
  boolean status = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(rscb_index >= DSRLPI_INVALID_RSCB_INDEX)
  {
    MSG_HIGH("Using Invalid SR_ID %d to Retrieve RLP Stats", rscb_index, 0, 0);
    
    /*----------------------------------------------------------------------
    Hard Code it to 1 to avoid potential memory corruption if the client does
    not check the return value
    -----------------------------------------------------------------------*/
    rscb_index = 1;
    status = FALSE;
  }
  /*-------------------------------------------------------------------------
    Get the RLP session control block pointer corresponding to the passed
    sr_id_index.
  -------------------------------------------------------------------------*/
  rscb_ptr = &( dsrlpi_rscb_array[ rscb_index]);

  /*-------------------------------------------------------------------------
    Update the rlp "state" field in the stats stucture now.
  -------------------------------------------------------------------------*/
  
  rscb_ptr->stats.state = rscb_ptr->state.state;

  /*-------------------------------------------------------------------------
    Pass a pointer to the stats structure to the ouside world
  -------------------------------------------------------------------------*/
  *stats_ptr_ptr = &rscb_ptr->stats;

  return status;
}/* dsrlp_get_stats_ex() */

#endif /* RLP 3 || HDR defined */
              
