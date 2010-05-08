/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E A R C H E R   4   T R A N S L A T I O N S

GENERAL DESCRIPTION
   This file contains the translation functions used by external tasks
   (1X, HDR, AFLT, etc) to translate their structures into srch4's
   generic task structure.

EXTERNALIZED FUNCTIONS
   srch4_mdsp_get_app_valid     Returns status of searcher 4's MDSP App
   srch4_enable                 Enables srch4 per technology
   srch4_mdsp_app_reg_cb        Registers a callback to be called on MDSP
                                 enable/disable
   srch4_register_mdsp_app      Registers srch4 app with the MDSP

   srch4_flush_sample_ram       Pass through function that flushes the
                                 sample RAM
   srch4_get_est_srch_time      Returns the search time for a (512,1) srch
   srch4_trans_cleanup_dump     Called from the drivers so the trans layer
                                 can cleanup after a search dump

   srch4_set_win_center         Sets searcher 4's window center
   srch4_get_win_center         Gets searcher 4's window center
   srch4_slew_win_center        Slews searcher 4's window center

   srch4_XXX_register           Registers technology XXX with srch4, sets
                                 priority and callbacks
   srch4_XXX_deregister         De-registers technology XXX with srch4
   srch4_XXX_active             Checks to see if technology XXX's has a
                                 srch in progress
   srch4_XXX_start_search       Starts technology XXX's search
   srch4_XXX_abort_search       Aborts technology XXX's search
   srch4_XXX_add_task           Adds a single technology XXX task 1x to the
                                 search queue

   srch4_aflt_search_pending    Checks to see if any AFLT searches are pending
   srch4_aflt_continue_search   Programs more searches and starts another
                                 AFLT search

   srch4trans_init              Initializes srch4's translation layer

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2002 - 2008
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/srch4/main/latest/src/srch4trans.c#10 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/10/08   adw     Corrected LPQ featurization.
09/12/08   bb      Adding additional changes for "MDSP voting against sleep offline QPCH cases"
09/12/08   bb      Changes to fix "MDSP voting against sleep offline QPCH cases"
09/10/08   sst     Update srch4_rerequest_queue for 1x only and to use NUM_TECH
09/08/08   adw     Added SRCH4_USE_LPQ, SRCH4_AFLT_ENABLED & SRCH4_HDR_ENABLED.
06/20/08   mca     Merge from //depot
06/04/08   mca     Changed 7500 to use the clkrgm resource manager
05/27/08   mca     Modified srch4 differentiate between normal 1x and offtl
                   searching
04/29/08   sst     Update CLKRGM_SAMPSRV_CLK calls to be compatible with
                    T_CLKREGIM = 7600, 7625, 7800
04/10/08   trc     Correct featurization around clock resource mgmt voting
03/31/08   mca     Added clock resource management (CLKRGM_RESOURCE_CDMA_DEC)
03/03/08   va      Abort queues on Sample RAM flush.
02/14/08   sst     Use new clk regime client CLKRGM_CLIENT_SRCH4
01/11/08   sst     Fix featurization
01/08/08   sst     En/disable CLKRGM_SAMPSRV_CLK with srch4 enable
12/04/07   gs/ljl  Merged in multi-carrier support
10/02/07   sst     Update 1x calc of freq offset to account for Rot units
09/22/07   mt      HDR 1x pilot search optimization.
08/23/07   sst     Fix missed featurization
08/14/07   mca     Featurized hdr functions
04/23/07   sst     Split up QLIC Support and QLIC Algorithm using defines
04/12/07   sst     Added srch_fing_qlic_log_isr to srch4_mdsp_isr_calls
04/10/06   mt      Added 1x pilot search capability to srch4_hdr_add_task().
02/26/07   sst     Lint fixes
02/15/07   tjc     Lint fixes
02/07/07   sst     Corrected use of sleep_mask
01/19/07   sst     Modified srch4 mdsp app request/release functionality
10/23/06   jyw/awj Added interface to allow HDR fetch interpolated srch results
09/25/06   awj     Moved RxD energy translation to trans layer
09/08/06   awj     Condensed offset param in add task to streamline interface
08/21/06   aps/awj removed references to trunc parameter
08/21/06   va/rkc  Changed driver layers to return absolute position for PPM
07/19/06   aps     Changed SRCH4_RET_ACTIVE to SRCH4DRV_RET_ACTIVE in
                    srch4_aflt_continue_search()
07/19/06   awj     Copy non-coherent int length in copy_sect_to_task
07/03/06   awj     Moved pilot_rec_type to sect parms, removed parm translation
06/08/06   awj     Remove max_inx and max_eng from sector structures
05/31/06   awj     Further sect and srch4 updates
05/12/06   awj     Updated sector structure.
04/10/06   va      AFLT to use chain 0 or 1 depending on 1X chain.
01/16/06   awj     Lint cleanup
12/19/05   bt      Added srch4_1x_get_assigned_queue.
11/18/05   va      Added function to reset AFLT variables
11/14/05   awj     Lint compliance
10/06/05   sst     In srch4_enable don't print out a message if app not valid
                    and the tech is trying to disable the app
09/13/05   sst     Lint fixes
07/19/05   grl     Added chain param to sample ram flush function.
07/05/05   sst     Correct 1x max win size check
05/31/05   rng     Merged from SHDR Sandbox.
05/17/05   sst     Corrected casting of window size in 1x copy from sector
                    struct and allow use of FULL_CIRCLE def
04/20/05   sst     Created separate mdsp isr's and callbacks for srch4 and
                    srch_mdsp
02/17/05   sst     Moved reference to MDSP_APP_SRCH into srch_lib_ext.c/h to
                    facilite custumer compiles will differing Feature set
02/04/05   bt      Add scheduled sample ram flush.
01/17/05   sst     Added access function srch4_mdsp_get_app_valid()
12/15/04   kwo     Rework of DH interface
12/07/04   sst     Changed srch4_1x_add_task() and srch_sect_srch4_append/_r()
                    to remove offset and add receive diversity field parameters
11/17/04   sst     Update dependency field of srch4_mdsp_* functions
09/08/04   jcm     Rename searcher1 functions srch1_
09/07/04   sq      Changed srch4_hdr_abort_search definition
09/03/04   kwo     Added support for linear ecio
09/01/04   bt      Change SRCH4 MDSP app/module names to SRCH
08/26/04   ejv     Modified LINT-related featurization.
08/24/04   jcm     Add int locks when calling back user outside of int space
08/13/04   va      Changed function srch4_aflt_add_task to allow free_q and
                   win center to be passed as parameters.
08/09/04   ejv     Add new function srch4_1x_wait_and_discard.
08/09/04   sst     Changed mdsp_isr function to a generic isr
                   Update some messages to be compliant with non 3Q images
07/28/04   ddh     Fixed RVCT compiler warnings
07/27/04   sst     Removed hdrsrchdrv_update_mstr_x8() call from hdr_read_res*
07/14/04   sst     Fixed Lint Errors
07/14/04   jcm     Added srch4_1x_num_tasks_added() and
                    srch4_1x_num_tasks_req()
07/13/04   sst     Added SRCH4_NO_QUEUE = SRCH4_MAX_Q in srch4_queue_type
                   Changed SRCH4_MAX_TYPE to SRCH4_MAX_TECH in srch4_srch_type
                   Added SRCH4_NO_TECH = SRCH4_MAX_TECH in srch4_srch_type
                   Lint Cleanup
07/12/04   sst     Added registration callbacks (implemented for 1x only),
                    this allows for "delayed" assignment of queues when none
                    are available, or un-registered queues are "abort active"
                   Added queue_assn[] var to map queue to technology
07/08/04   sst     Always call dumb callback on start search, even if failure
                    (except for when queue is already active)
06/30/04   dna     Added a function srch4_disable_mdsp_app() that is to be
                    called from an SRM object when 1x and HDR both vote they
                    don't need the SRCH4 dsp image
06/25/04   jcm     Mainlined FEATURE_HDR, SRCH4_HDR_ENABLED,
                    SRCH4_AFLT_ENABLED
06/21/04   sst     Added active checking before mdsp can be disabled
                   Removed sample ram flush in srch4_1x_register()
06/04/04   sst     Check for NULL ptr in srch4_register_mdsp_app()
06/01/04   gs      Update calls to srch_afc_get_freq_err to use signed values.
05/28/04   jcm     Sector module integration
05/27/04   sst     Re-add of dropped change from 04/21/04
05/26/04   sst     Update comments for freq offsets
                   Added rotator error into freq offset when starting a srch
05/25/04   sst     Added parameter to abort_cb to be able to pass number
                    of partial results returned
05/04/04   sst/va  Merge from srch4 3Q dev branch (dev-changes)
04/26/04   sst     dev-Changed SRCH4DRV_RET_FREE to be queue specific
04/21/04   ejv     Cleanup LINT errors.
04/14/04   sst     dev-Added partial results param to srch4_abort_queue()
04/12/04   sst     dev-Created srch4trans var to hold all local variables
04/08/04   sst     dev-Created SRCH4_HAS_HPQ1,SRCH4_MDSP2_CODE_USES_MDSP1_IMAGE
                   dev-Created SRCH4_DEBUG, SRCH4_DEBUG_MSG()
03/25/04   sst     dev-Moved (de)registering support into tech specific funcs
03/25/04   va      dev-Added Support for AFLT register/Deregister.
03/19/04   va      dev-Do not enable MDSP if already active.
03/18/04   sst     dev-Added support for srch4 (de)registering
                   dev-Modified srch4_1x_start_search() parameters
                   dev-Updated priority handling
03/12/04   sst     dev-Updated priority assignment
03/08/04   va      Fixed Problem with saving PPM tasks in the pending queue
03/04/04   sst     dev-Changed return of srch4_enable() to be a boolean
                   dev-srch4_enable() nolonger includes GPS
                   dev-Created linked list of CBs for dis/enable of MDSP app
02/19/04   sst     Corrected lint errors
                   Removed srches_done parameter from srch4_**_read_result()
02/12/04   ddh     Corrected lint errors, changed return types to int32 where
                   int8 was used. Corrected error code translation to always
                   use correct enum values.
02/04/04   br      Added separate free queue for PPM search request buffers.
02/02/04   sst     Added synchronous field to 1X abort function
01/28/04   ejv     Mainline T_MSM6500.
01/21/04   sst     Added function srch4_get_est_srch_time()
01/06/04   sst     Added parameter age_searches to srch4_1x_add_task and
                    util_1x_copy_sect_to_task to facilitate proper use of
                    search sector aging
12/15/03   sst     Added a technology parameter to srch4_tasks_left()
11/05/03   bt      Changed prototype for srch4_abort_queue.
11/04/03   bt      Move MDSP interface from srch4drv into srch4trans and
                   change srch4 voting interface for MSM6500.
10/08/03   va      Use a dedicated variable for AFLT window center
10/06/03   aaj     Mainlined HDRSRCH_MULTIPATH_WIN_CENTER
09/24/03   ejv     Remove unnecessary comments for 6500.
09/16/03   sst/va  In srch4_aflt_add_task() change call to
                    srch4_get_win_center() to reference the global variable
                    srch4_win_center to remove SRCH2_SRCH4_OFFSET
                   Corrected return value for srch4_slew_win_center()
09/12/03   sst     Modified stored window_pos to remove introduced truncation
                    correction that is no longer needed
                   Re-corrected calc of index to use a mask
09/10/03   bt      Put HDR on the LPQ until we got 3 queues.
09/09/03   sst     Corrected calc of index when window crosses top of pn circle
09/04/03   sst     Correct for truncation error in window position (1x only)
08/20/03   aaj     Added support to send Rx Diversity Searches
08/20/03   sst     Fixed rounding error in calc of win_pos
                   Modified srch4_win_center to hide the SRCH2_SRCH4_OFFSET
                    from the rest of searcher
08/12/03   sst     Added missing return in srch4_hdr_start_search
                   Some code clean up
08/11/03   va      Use ppm_ptr for storing PPM results.
06/24/03   va      Init Search Result index to -1 since it is signed.
06/17/03   ejv     Use SRCH4_HPQ for HDR for now.  Set srch4_clk_bitmask in
                    srch4_vote_clk() for 6500.
06/15/03   rdh     Added srch4_reserved.
06/09/03   va      Init PPM search result index and energy before search.
05/29/03   kwo/sst Fixed task inc issue with TD
04/25/03   sst     Changed wrong index variable in srch4_1x_add_task()
04/22/03   sst     Fixed handling of tasks available check upon adding a
                    search to srch4_1x_add_task()
04/04/03   va      Return AFLT results in Cx16 resolution. Add 1x offset to
                    the PPM search offset.
03/24/03   sst     Modified peak position storing to reflect pos in Cx16
03/03/03   va      Added lost dump callback for AFLT & updated comments.
02/18/03   va      SRCH4 AFLT support.
02/14/03   bt      call abort callback if searcher not running.
02/12/03   bt      change sleep_1x_enable to select clk_src.
01/28/03   sst     Encapsulated srch_win_center to srch_hw.c/h
01/27/03   bt      Added srch4_vote_clk function.
01/13/03   sst     Added return values to start, add, and abort functions,
                    added func to translate drv return values to trans values,
                    added parameter to lost_dump_cb
12/20/02   sst     Release A fix
12/05/02   sst     Code Clean up
11/26/02   sst     Fixed diversity path 0 eng peak storage
11/14/02   sst     Using srch4 return enum values
11/11/02   sst     Code cleanup, Added clean up for un-programmed TD searches
                    Moved initializations for (h/l)pq_task_num to init func
                    In TD merging, check for unreturned peaks
11/07/02   sst     Added support for Jaguar, moved coh_tr out
                   Created skeletons for HDR and AFLT functions
                   Code cleanup
11/05/02   sst     Merge changes, removed debug
11/04/02   sst     Srch4 win center function moved here from srch4drv
10/30/02   sst     Added support for srch4's own window center var
10/29/02   sst     Initial checkin
10/22/02   sst     Created File

===========================================================================*/

/*==========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "customer.h"

#include "comdef.h"
#include "clkregim.h"
#include "clkrgm_modem.h"
#include "err.h"
#include "msg.h"
#include "msgcfg.h"

#include "srch4drv.h"
#include "srch4drv_t.h"
#include "srch4trans.h"
#include "srch4trans_t.h"

#include "srchi.h"
#include "srch_afc.h"
#include "srch_conv.h"
#include "srch_hw.h"
#include "srch_util.h"

#ifdef FEATURE_IS2000_REL_A_TD
#include "cai.h"
#endif /* FEATURE_IS2000_REL_A_TD */
#ifdef FEATURE_CDMA_RX_DIVERSITY
#include "srch_rx.h"
#endif /* FEATURE_CDMA_RX_DIVERSITY */
#ifdef SRCH4_HDR_ENABLED
#include "hdrsrchdrv.h"
#endif /* SRCH4_HDR_ENABLED */
#ifdef SRCH4_AFLT_ENABLED
#include "queue.h"
#endif /* SRCH4_AFLT_ENABLED */


/*===========================================================================
             static DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Misc definitions, structs, and variables
  -------------------------------------------------------------------------*/

/* translation between sector gain and searcher4 coherent truncation */
static const uint8 srch4_coh_tr[MAX_SECTOR_GAIN_VAL] =
{
  COH_TRUNC_5,
  COH_TRUNC_4,
  COH_TRUNC_3,
  COH_TRUNC_2
};

/*-------------------------------------------------------------------------
      MDSP related definitions and declarations
  -------------------------------------------------------------------------*/
/* rather than include all of the .h files, which shouldn't be externalized,
   explicitly extern the calls here */
static void srch4_mdsp_isr( void );
extern void srch4_dump_isr( void );

#ifdef FEATURE_ZZ2_1
extern void srchzz_qpch_offtl_qpch_isr( void );
#endif /* FEATURE_ZZ2_1 */
#ifdef FEATURE_SRCH_QLIC
extern void srch_fing_qlic_log_isr( void );
#endif /* FEATURE_SRCH_QLIC */

/* This is the ISR mapping from MDSP to srch4 interrupts */
static mdsp_isr_func_type srch4_mdsp_isr_table[3] =
{
  srch4_mdsp_isr,
  NULL,
  NULL
};

/* Define a way in which multiple ISR's can be called on a single interrupt
   from the MDSP (The minimum number of calls is "1", allowing a valid
   array size) */
#if ( defined( FEATURE_ZZ2_1 ) && defined ( FEATURE_SRCH_QLIC ) )
  #define SRCH4_MDSP_NUM_ISR_CALLS      3
#elif ( defined( FEATURE_ZZ2_1 ) || defined ( FEATURE_SRCH_QLIC ) )
#define SRCH4_MDSP_NUM_ISR_CALLS      2
#else
#define SRCH4_MDSP_NUM_ISR_CALLS      1
#endif

typedef struct
{
  void  (*call)( void );
} srch4_mdsp_isr_calls_type;

static srch4_mdsp_isr_calls_type
       srch4_mdsp_isr_calls[ SRCH4_MDSP_NUM_ISR_CALLS ] =
{
  srch4_dump_isr,
  #ifdef FEATURE_ZZ2_1
  srchzz_qpch_offtl_qpch_isr,
  #endif /* FEATURE_ZZ2_1 */
  #ifdef FEATURE_SRCH_QLIC
  srch_fing_qlic_log_isr,
  #endif /* FEATURE_SRCH_QLIC */

};

/* This is the pool of callbacks to use upon the MDSP setting the Srch4 app
   valid and invalid */
struct srch4_mdsp_app_cb_struct
{
  void                            (*cb_func)(boolean);
  struct srch4_mdsp_app_cb_struct  *next;
};
typedef struct srch4_mdsp_app_cb_struct srch4_mdsp_app_cb_struct_type;

static srch4_mdsp_app_cb_struct_type
             srch4_mdsp_app_cb_pool[ SRCH4_MDSP_APP_CB_POOL_SIZE ];

/*-------------------------------------------------------------------------
      srch4trans structs and variables
  -------------------------------------------------------------------------*/
/* ----------  Misc variable structure  ------------ */
typedef struct
{
  int32               win_center;      /* srch4's own window center in Cx8
                                           in the range 0 ... 262143         */
} srch4trans_misc_struct_type;

/* ----------  MDSP variable structure  ------------ */
typedef struct
{
  uint32              req_mask;        /* technology mdsp request votes      */
  uint32              sleep_mask;      /* technology no mdsp_sleep votes     */
  boolean             app_valid;       /* srch4 app is valid/useable         */
  boolean             app_registered;  /* srch4 app is registered            */
  rex_tcb_type       *app_task_tcb;    /* for downloading DSP image          */
  rex_sigs_type       app_task_sig;    /* for downloading DSP image          */
  srch4_mdsp_app_cb_struct_type        /* callbacks upon app valid/not valid */
                     *app_cb_head;
  srch4_mdsp_app_cb_struct_type        /* callbacks upon app valid/not valid */
                     *app_cb_tail;
} srch4trans_mdsp_struct_type;

/* ----------  Queue variable structures  ---------- */
/* technology queue assignments, priority, callbacks, etc */
typedef struct
{
  srch4_queue_type     queue;                 /* assigned queue to use       */
  srch4_priority_type  priority;              /* priority                    */
  int16                freq_offset;           /* freq offset                 */
  void               (*dump_cb_func)(void *); /* ptr to a dump cb function   */
  void               (*lost_cb_func)(void *); /* ptr to a lost cb function   */
  void               (*reg_cb_func)(void *);  /* ptr to the registered cb    */
  void                *reg_cb_data;           /* register cb data            */
} srch4_tech_assignment_struct_type;

static srch4_tech_assignment_struct_type srch4_tech_assgn[ SRCH4_NUM_TECH ];

typedef struct
{
  srch4_tech_assignment_struct_type    /* tech info array (prio,CB's,queue)  */
                      *tech;
  srch4_srch_type     queue_assn[ SRCH4_MAX_Q ];
                                       /* tech assigned to a queue           */
  uint32              task_num[ SRCH4_MAX_Q ];
                                       /* num of tasks in a queue (idx)      */
} srch4trans_queue_struct_type;

/* ----------  1X variable structure  -------------- */
typedef struct
{
  /*lint -esym(754,not_used)   currently not used */
  uint32              not_used;        /* currently no variables             */
} srch4trans_1x_struct_type;

/* ----------  HDR variable structure  ------------- */
#ifdef SRCH4_HDR_ENABLED
typedef struct
{
  /*lint -esym(754,not_used)   currently not used */
  uint32              not_used;        /* currently no variables             */
} srch4trans_hdr_struct_type;
#endif /* SRCH4_HDR_ENABLED */

/* ----------  AFLT variable structure  ------------ */
#ifdef SRCH4_AFLT_ENABLED
static srch4_task_struct_type srch4_aflt_tasks_pending[SRCH4_MAX_AFLT_TASKS];

typedef struct
{
  uint8               task_num;        /* variable for simplification        */
  srch4_task_struct_type               /* ptr to array of pending tasks      */
                     *tasks_pending;
  void               *cb_data;         /* callback data                      */
} srch4trans_aflt_struct_type;
#endif /* SRCH4_AFLT_ENABLED */

/*-------------------------------------------------------------------------*/

typedef struct
{
  /*lint -esym(754,cdma_1x,hdr)   currently not used */

  srch4trans_misc_struct_type    misc;
  srch4trans_mdsp_struct_type    mdsp;
  srch4trans_queue_struct_type   queue;
  srch4trans_1x_struct_type      cdma_1x;
  #ifdef SRCH4_HDR_ENABLED
  srch4trans_hdr_struct_type     hdr;
  #endif /* SRCH4_HDR_ENABLED */
  #ifdef SRCH4_AFLT_ENABLED
  srch4trans_aflt_struct_type    aflt;
  #endif /* SRCH4_AFLT_ENABLED */
} srch4trans_struct_type;

/* initial values */
static srch4trans_struct_type srch4trans =
{
  /* Misc vars */
  {
    0,                          /* win_center:                               */
  },

  /* MDSP vars */
  {
    0,                          /* req_mask:       no initial tasks          */
    0,                          /* sleep_mask:     no initial tasks          */
    FALSE,                      /* app_valid:      initially not valid       */
    FALSE,                      /* app_registered: initially not registered  */
    NULL,                       /* app_task_tch:                             */
    0,                          /* app_task_sig:                             */
    NULL,                       /* app_cb_head:    inited in srch4trans_init */
    NULL,                       /* app_cb_tail:    inited in srch4trans_init */
  },

  /* Queue vars */
  {
    &(srch4_tech_assgn[0]),     /* tech:            init to srch4_tech_assgn */
    {                           /* queue_assn                                */
      SRCH4_NO_TECH,            /*  HPQ0:                                    */
      #ifdef SRCH4_USE_LPQ
      SRCH4_NO_TECH,            /*  LPQ:                                     */
      #endif
      #ifdef SRCH4_HAS_HPQ1
      SRCH4_NO_TECH,            /*  HPQ1:                                    */
      #endif
    },
    {                           /* task_num                                  */
      0,                        /*  HPQ0:                                    */
      #ifdef SRCH4_USE_LPQ
      0,                        /*  LPQ:                                     */
      #endif
      #ifdef SRCH4_HAS_HPQ1
      0,                        /*  HPQ1:                                    */
      #endif
    },
  },

  /* CDMA 1X vars */
  {
    0,                          /* not_used: currently no variables          */
  },

  #ifdef SRCH4_HDR_ENABLED
  /* HDR vars */
  {
    0,                          /* not_used: currently no variables          */
  },
  #endif

  #ifdef SRCH4_AFLT_ENABLED
  /* AFLT vars */
  {
    0,                          /* task_num:                                 */
    &(srch4_aflt_tasks_pending[0]),
                                /* tasks_pending:                            */
    NULL,                       /* cb_data:                                  */
  },
  #endif

}; /* srch4trans var init */

#define SRCH4_CHK_QUEUE_TECH( i )  ( srch4trans.queue.queue_assn[ i ] & 0xF )

/*===========================================================================
              FUNCTION PROTOTYPES FOR MODULE
===========================================================================*/

static void srch4_1x_read_result
(
  srch4_task_struct_type *task_res       /* array of task struct (results) */
);

static void srch4_rerequest_queue( void );

/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/
/*===========================================================================

           M D S P    A P P    R E L A T E D    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_MDSP_GET_APP_VALID

DESCRIPTION    This function returns TRUE the SRCH4 MDSP app is registered
                and enabled.

DEPENDENCIES   None

RETURN VALUE   Boolean indicating if SRCH4's MDSP App is valid.

SIDE EFFECTS   None

===========================================================================*/
boolean srch4_mdsp_get_app_valid( void )
{
  /*-----------------------------------------------------------------------*/

  return( srch4trans.mdsp.app_valid );

} /* srch4_mdsp_get_app_valid */


/*===========================================================================

FUNCTION       SRCH4_MDSP_APP_REG_CB
DESCRIPTION    This function registers mdsp callbacks to be called upon
                enable/disable of the srch4 mdsp app
DEPENDENCIES   The registered callback function must handle the case where
                the MDSP app is disabled and searches were pending. In this
                case the searches will be aborted without notification to
                the corresponding task, though this callback will be called

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
void srch4_mdsp_app_reg_cb
(
  void     (*callback)( boolean ),
  boolean    reg_cb
)
{
  srch4_mdsp_app_cb_struct_type *curr_ptr = NULL, *prev_ptr = NULL;
                                           /* ptrs for simplifications      */

  /*-----------------------------------------------------------------------*/

  /*lint -save -e613 -e794  Possible/Conceivable use of null pointer
                            curr_ptr/prev_ptr */
  curr_ptr = srch4trans.mdsp.app_cb_head;
  prev_ptr = srch4trans.mdsp.app_cb_head;

  /* Is the head valid? */
  if ( curr_ptr == NULL )
  {
    ERR_FATAL( "MDSP callback pool and head ptr invalid!", 0, 0, 0 );
  }

  /* step through the list looking for a match or the next empty slot */
  while ( ( curr_ptr->next    != NULL ) &&
          ( curr_ptr->cb_func != callback ) &&
          ( curr_ptr->cb_func != NULL ) )
  {
    prev_ptr = curr_ptr;
    curr_ptr = curr_ptr->next;
  }

  if ( curr_ptr->cb_func == callback )
  {
    /* trying to re-add a callback */
    if ( reg_cb )
    {
      MSG_HIGH( "Tried to re-add an existing srch4_mdsp_app cb", 0, 0, 0 );
    }
    else /* de-register the callback */
    {
      /* "remove" this link from the list and place it on the tail */
      /* unless this is the head */
      if ( curr_ptr == srch4trans.mdsp.app_cb_head )
      {
        srch4trans.mdsp.app_cb_head       = curr_ptr->next;
        srch4trans.mdsp.app_cb_tail->next = curr_ptr;
        srch4trans.mdsp.app_cb_tail       = curr_ptr;
      }
      /* unless this is the tail */
      else if ( curr_ptr->next == NULL )
      {
        /* do nothing, just clear the cb and the next ptr */
      }
      /* this is in the middle of the list */
      else
      {
        prev_ptr->next                    = curr_ptr->next;
        srch4trans.mdsp.app_cb_tail->next = curr_ptr;
        srch4trans.mdsp.app_cb_tail       = curr_ptr;
      }

      /* clear this link */
      curr_ptr->cb_func = NULL;
      curr_ptr->next    = NULL;

      MSG_MED( "Removed a cb from srch4_mdsp_app", 0, 0, 0 );
    }
  }
  else if ( curr_ptr->cb_func == NULL ) /* register a new callback */
  {
    if ( reg_cb )
    {
      curr_ptr->cb_func = callback;
      MSG_MED( "Added a cb to srch4_mdsp_app", 0, 0, 0 );

    }
    else /* we didn't find the callback to be deregistered */
    {
      MSG_HIGH( "Tried to remove a nonexistent srch4_mdsp_app cb", 0, 0, 0 );
    }
  }
  else /* (curr_ptr->next == NULL) no more empty slots */
  {
    MSG_FATAL( "srch4_mdsp_app cb pool too small (%d)",
               SRCH4_MDSP_APP_CB_POOL_SIZE, 0, 0 );
  }

  /*lint -restore */
} /* srch4_mdsp_app_reg_cb */


/*===========================================================================

FUNCTION       SRCH4_MDSP_APP_CALL_CB
DESCRIPTION    This function calls all registered mdsp callbacks
DEPENDENCIES   None.

RETURN VALUE   None.
SIDE EFFECTS   Possible task switches

===========================================================================*/
static void srch4_mdsp_app_call_cb
(
  boolean    enabled                        /* enabled? disabled?          */
)
{
  srch4_mdsp_app_cb_struct_type *curr_ptr;

  uint32 cb_cnt = 0;

  /*-----------------------------------------------------------------------*/

  /* assign the head ptr to the current ptr */
  curr_ptr = srch4trans.mdsp.app_cb_head;

  /* loop through the list, calling each registered callback function with
     the enabled parameter */
  while ( curr_ptr != NULL )
  {
    if ( curr_ptr->cb_func != NULL )
    {
      /* call the CB with the enabled/disabled flag */
      (*curr_ptr->cb_func)( enabled );
      cb_cnt++;
    }
    else  /* end of "valid" list */
    {
      break;
    }
    /* go to the next link */
    curr_ptr = curr_ptr->next;
  }

  MSG_MED( "called %d Srch4 App cb's passed %d", cb_cnt, enabled, 0 );

} /* srch4_mdsp_app_call_cb */


/*===========================================================================

FUNCTION       SRCH4_MDSP_APP_EVENT_HANDLER
DESCRIPTION    This function is the event handler for the MDSP.
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None.
SIDE EFFECTS   Registered MDSP app callbacks will be called
               All pending searches will be aborted without callbacks or
                any notifications to tasks. This must be taken care of in
                the task specific MDSP app callback.

===========================================================================*/
static void srch4_mdsp_app_event_handler
(
  mdsp_event_data_type *event_ptr           /* mdsp event data             */
)
{
  /*-----------------------------------------------------------------------*/

  if ( event_ptr == NULL )
  {
    ERR( "srch4_mdsp_app_event_handler got NULL event_ptr ptr", 0, 0, 0 );

    return;
  }

  switch ( event_ptr->event )
  {
    case MDSP_EV_COMMAND:
    {
      if ( event_ptr->cmd.cmd_status == MDSP_CMD_ACCEPT )
      {
        MSG_LOW( "MDSP_COMMAND ACCEPTED app=%d", event_ptr->cmd.app_id, 0, 0 );
      }
      else
      {
        ERR_FATAL( "MDSP_COMMAND FAILED app=%d", event_ptr->cmd.app_id, 0, 0 );
      }
      break;
    }
    case MDSP_EV_MODULE:
    {
      mdsp_ev_mod_type *mod_ptr = (mdsp_ev_mod_type*)event_ptr;

      switch ( mod_ptr->mod_status )
      {
        case MDSP_MOD_READY:                /* APP is now available */
        {
          MSG_HIGH( "MDSP_MOD_READY app=%d, image=%d",
                    mod_ptr->app_id, (int)(mod_ptr->info.ready.image), 0 );

          if ( !srch4trans.mdsp.app_registered )
          {
            /* This is the first MDSP_MOD_READY event after registering
               the app. Remember this so we don't have to register again */
            srch4trans.mdsp.app_registered = TRUE;

            /* For the first MDSP_MOD_READY event we want to enable
               dsp_sleep for the SRCH4 app (to conserve power) until the
               first technology calls srch4_enable */
            mdsp_dsp_sleep( MDSP_APP_SRCH );
          }

          if ( ( srch4trans.mdsp.app_task_tcb != NULL ) &&
               ( srch4trans.mdsp.app_task_sig != 0 ) )
          {
            /* If we have task/signal information then use it and clear
               it so we don't send signals on every MDSP_MOD_READY event */
            (void) rex_set_sigs( srch4trans.mdsp.app_task_tcb,
                                 srch4trans.mdsp.app_task_sig );
            srch4trans.mdsp.app_task_tcb = NULL;
            srch4trans.mdsp.app_task_sig = 0;
          }

          /* Remember that the srch4 app is ready */
          srch4trans.mdsp.app_valid = TRUE;

          /* notify all tasks that the MDSP is available */
          srch4_mdsp_app_call_cb( TRUE );

          /* check to see if any technologies are waiting for queues */
          srch4_rerequest_queue();

          break;
        }
        case MDSP_MOD_DISABLE:              /* APP is no longer available */
        {
          MSG_HIGH( "MDSP_MOD_DISABLE app=%d", mod_ptr->app_id, 0, 0 );
          /* Remember that the srch4 app is not ready */

          srch4trans.mdsp.app_valid = FALSE;

          #ifdef SRCH4_AFLT_ENABLED
          /* Reset any AFLT pending tasks & callback data */
          srch4trans.aflt.task_num  = 0;
          srch4trans.aflt.cb_data   = NULL;
          #endif /* SRCH4_AFLT_ENABLED */

          /* stop everything srch4 NOW */
          srch4_halt();

          /* notify all tasks that the MDSP is NOT available */
          srch4_mdsp_app_call_cb( FALSE );

          mdsp_disable_event_rsp( mod_ptr->app_id );
          break;
        }
        case MDSP_MOD_ENABLE_FAILED:
        {
          ERR_FATAL( "MDSP_MOD_ENABLE_FAILED app=%d", mod_ptr->app_id, 0, 0 );
          break;             /*lint !e527 Unreachable */
        }
        default:
        {
          ERR_FATAL( "Invalid mod status (%d)", mod_ptr->mod_status, 0, 0 );
          break;             /*lint !e527 Unreachable */
        }
      }  /* switch mod_ptr->mod_status */
      break;
    }
    default:
    {
      ERR_FATAL( "Illegal MDSP Event type (%d)", event_ptr->event, 0, 0 );
      break;                 /*lint !e527 Unreachable */
    }
  } /* switch event_ptr->event */

} /* srch4_mdsp_app_event_handler */


/*===========================================================================

FUNCTION       SRCH4_REGISTER_MDSP_APP
DESCRIPTION    This function registers the SRCH4 MDSP application.
DEPENDENCIES   This is legacy support until the use of this interface is
                removed from tmc.c

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
void srch4_register_mdsp_app
(
  rex_tcb_type *task_tcb,                 /* task requesting searcher 4    */
  rex_sigs_type task_sig                  /* signal to set upon completion */
)
{
  /*-----------------------------------------------------------------------*/

  srch4_request_mdsp_app( SRCH4_1X_MASK, task_tcb, task_sig );

} /* srch4_register_mdsp_app */

/*===========================================================================

FUNCTION       SRCH4_REQUEST_MDSP_APP
DESCRIPTION    This function registers the SRCH4 MDSP application.
DEPENDENCIES   MDSP task must be running.
               If app is already registered and enabled, then only the
               specified signal will be sent. Any registered callbacks
               through srch4_mdsp_app_reg_cb() will NOT be called.

RETURN VALUE   None.
SIDE EFFECTS   None.

===========================================================================*/
void srch4_request_mdsp_app
(
  srch4_tech_type tech_type,              /* mask for technology           */
  rex_tcb_type   *task_tcb,               /* task requesting searcher 4    */
  rex_sigs_type   task_sig                /* signal to set upon completion */
)
{
  /*-----------------------------------------------------------------------*/

  /* Was this already called, and still waiting for signals to be set?
     The tcb and sig should be NULL and 0 if all is good. If this error
     is reached, then some task will be waiting for a signal that will
     never be sent. */
  if ( srch4trans.mdsp.app_task_tcb != NULL )
  {
    ERR( "srch4_register_mdsp_app called while awaiting prev register (sig=%d)",
         srch4trans.mdsp.app_task_sig, 0, 0 );
  }

  /* check if this technology has already requested the srch 4 app */
  if ( ( srch4trans.mdsp.req_mask & (uint32)tech_type ) )
  {
    /* in this case proceed on as normal */
    return;
  }
  else /* this is the first request for the srch 4 app for this technology */
  {
    /* update the enable bit mask to record which technology requested srch4 */
    srch4trans.mdsp.req_mask |= (uint32)tech_type;

    /* if the app hasn't been registered, do it now */
    if ( !srch4trans.mdsp.app_registered )
    {
      mdsp_register_app( MDSP_APP_SRCH,
                         srch4_mdsp_isr_table,
                         srch4_mdsp_app_event_handler,
                         ( ( ( uint16 ) MDSP_EV_COMMAND ) |
                           ( ( uint16 ) MDSP_EV_MODULE ) ) );

      MSG_HIGH( " MDSP SRCH4 APP -> Req:Register (mask=0x%4x, en_mask=0x%4x)",
                tech_type, srch4trans.mdsp.req_mask, 0 );
    }

    /* Enable the app if needed */
    if (!srch4trans.mdsp.app_valid)
    {
      /* Enable processing will set the task signal */
      srch4trans.mdsp.app_task_tcb = task_tcb;
      srch4trans.mdsp.app_task_sig = task_sig;
      mdsp_enable( MDSP_APP_SRCH, MDSP_MODULE_SRCH );
      MSG_HIGH( " MDSP SRCH4 APP -> Req: Enable  (mask=0x%4x, en_mask=0x%4x)",
                tech_type, srch4trans.mdsp.req_mask, 0 );
    }
    else
    {
      if ( task_tcb != NULL )
      {
        /* Set the task signal to unblock it */
        (void) rex_set_sigs( task_tcb, task_sig );
      }
      MSG_HIGH( " MDSP SRCH4 APP -> Req: called  (mask=0x%4x, en_mask=0x%4x)",
                tech_type, srch4trans.mdsp.req_mask, 0 );
    } /* app valid check */
  } /* technology check */

} /* srch4_request_mdsp_app */


/*===========================================================================

FUNCTION       SRCH4_RELEASE_MDSP_APP
DESCRIPTION    Disables the SRCH4 module entirely (not just for sleep)

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
void srch4_release_mdsp_app
(
  srch4_tech_type   tech                  /* technology                    */
)
{
  /*-----------------------------------------------------------------------*/

  /* update the enable bit mask to record which technology deregistered
     for srch4 */
  srch4trans.mdsp.req_mask &= (uint32)~tech;

  /* only disable if the mask is now "0" and the mask was not previously "0" */
  if ( srch4trans.mdsp.req_mask == 0 )
  {
    if (srch4trans.mdsp.app_registered)
    {
      mdsp_disable( MDSP_APP_SRCH );
      srch4trans.mdsp.app_valid = FALSE;
      MSG_HIGH( " MDSP SRCH4 APP -> REL:Disable  (mask=0x%4x, en_mask=0x%4x)",
                tech, srch4trans.mdsp.req_mask, 0 );

    }
  } /* mask checks */
  else
  {
    MSG_HIGH( " MDSP SRCH4 APP -> REL: called  (mask=0x%4x, en_mask=0x%4x)",
              tech, srch4trans.mdsp.req_mask, 0 );
  }

} /* srch4_release_mdsp_app */


/*===========================================================================

FUNCTION       SRCH4_ENABLE
DESCRIPTION    This function enables which technology needs searcher 4. If no
               technology needs searcher 4 it will vote for DSP sleep.
DEPENDENCIES   Only one technology's bit shall be set via a given call

RETURN VALUE   True if app is awake and valid
               False if app is not valid or if app was put to sleep
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_enable
(
  srch4_tech_type tech,                    /* technology                   */
  boolean         enable                   /* whether to enable            */
)
{

  uint32   temp_mask;                      /* previous bit mask            */
  boolean  valid      = FALSE;             /* return value                 */

  /* save interrupts */
  uint32                  isave;
  /*-----------------------------------------------------------------------*/

  INTLOCK_SAV( isave );

  /* store off the previous bit mask */
  temp_mask = srch4trans.mdsp.sleep_mask;

  /* Make sure that searcher 4 is not active */
  if ( ( (uint32)tech & (uint32)SRCH4_1X_MASK ) && !enable )
  {
    if ( srch4_1x_active() )
    {
      /* 1x is active, do not disable for 1x */
      MSG_HIGH( "1X tried to disable SRCH4 MDSP while a srch was active",
                0, 0, 0 );
      INTFREE_SAV( isave );

      return( TRUE );
    }
  }
  #ifdef SRCH4_HDR_ENABLED
  else if ( ( (uint32)tech & (uint32)SRCH4_HDR_MASK ) && !enable )
  {
    if ( srch4_hdr_active() )
    {
      /* HDR is active, do not disable for HDR */
      MSG_HIGH( "HDR tried to disable SRCH4 MDSP while a srch was active",
                0, 0, 0 );
      INTFREE_SAV( isave );

      return( TRUE );
    }
  }
  #endif /* SRCH4_HDR_ENABLED */
  #ifdef SRCH4_AFLT_ENABLED
  else if ( ( (uint32)tech & (uint32)SRCH4_AFLT_MASK ) && !enable )
  {
    if ( srch4_aflt_active() )
    {
      /* AFLT is active, do not disable for AFLT */
      MSG_HIGH( "AFLT tried to disable SRCH4 MDSP while a srch was active",
                0, 0, 0 );
      INTFREE_SAV( isave );

      return( TRUE );
    }
  }
  #endif /* SRCH4_AFLT_ENABLED */

  /* Note: flags can still be enabled or disabled whether or not
     MDSP_APP_SRCH app is valid, this way when it becomes valid,
     the proper flags are set */
  /* Do we want to enable or disable the clock for this technology? */
  if ( enable )
  {
    /* set the bit */
    srch4trans.mdsp.sleep_mask |= (uint32)tech;
  }
  else
  {
    /* clear the bit */
    srch4trans.mdsp.sleep_mask &= ~( (uint32)tech );
  }

  /* is the app valid, and available? */
  if ( srch4trans.mdsp.app_valid)
  {
    /* Did all bits get cleared this time? Don't look at the GPS bit as
       it is not used for putting the SRCH_APP to sleep */
    if ( srch4trans.mdsp.sleep_mask == 0 )
    {
      if ( temp_mask != 0 )               /* previously enabled */
      {
        mdsp_dsp_sleep( MDSP_APP_SRCH );

        /* Turn off RXCx8 */
        #if defined( T_MSM7600 ) ||                 \
            defined( FEATURE_SRCH_CLKREGIM_RM ) ||    \
            defined( T_MSM7500 )
        clk_regime_resource_disable( CLKRGM_CLIENT_SRCH4,
                                     CLKRGM_RESOURCE_CDMA_DEC);
        #endif /* Targets */

        MSG_HIGH( " MDSP SRCH4 APP -> sleep    (mask=0x%4x, slp_mask=0x%4x)",
                  tech, srch4trans.mdsp.sleep_mask, 0 );
      }
      valid = FALSE;
    }
    else /* awaken */
    {
      /* Did we set the first one this time? */
      if ( temp_mask == 0 )               /* previously asleep */
      {
        /* Turn on RXCx8 */
        #if defined( T_MSM7600 ) ||                 \
            defined( FEATURE_SRCH_CLKREGIM_RM ) ||    \
            defined( T_MSM7500 )
        clk_regime_resource_enable( CLKRGM_CLIENT_SRCH4,
                                    CLKRGM_RESOURCE_CDMA_DEC);
        #endif /* Targets */

        mdsp_dsp_wakeup( MDSP_APP_SRCH );

        MSG_HIGH( " MDSP SRCH4 APP -> wakeup   (mask=0x%4x, slp_mask=0x%4x)",
                  tech, srch4trans.mdsp.sleep_mask, 0 );
      }
      valid = TRUE;
    }
  }
  else /* srch4 app is not valid */
  {
    if ( enable )
    {
      MSG_ERROR( " MDSP SRCH4 APP -> app not valid (mask=0x%4x, slp_mask=0x%4x)",
                 tech, srch4trans.mdsp.sleep_mask, 0 );
    }
    valid = FALSE;
  }

  INTFREE_SAV( isave );

  return( valid );

} /* srch4_enable */


/*===========================================================================

FUNCTION       SRCH4_MDSP_ISR
DESCRIPTION    This function is the registered callback for an MDSP interrupt.
               This function in turn calls all registered isr calls in
               srch4_mdsp_isr_calls
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4_mdsp_isr( void )
{

  uint32  i;                     /* loop counter */

  /*-----------------------------------------------------------------------*/

  for ( i = 0; i < SRCH4_MDSP_NUM_ISR_CALLS; i++ )
  {
    /* do this check in case there are no callbacks (min size of an
       array is "1") */
    if ( srch4_mdsp_isr_calls[i].call != NULL )
    {
      srch4_mdsp_isr_calls[i].call();
    }
  }

} /* srch4_mdsp_isr */

/*===========================================================================

           S R C H 4 T R A N S    U T I L I T Y    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4TRANS_PRINT_QUEUE_STATUS
DESCRIPTION    This function prints out the queue status
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void srch4trans_print_queue_status( void )
{
  /*-----------------------------------------------------------------------*/

  #ifdef SRCH4_HAS_HPQ1
    MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      " Queues: HPQ0=%s, HPQ1=%s, LPQ=%s",
    SRCH4_TECH_STRING( SRCH4_CHK_QUEUE_TECH( SRCH4_HPQ0 ) ),
    SRCH4_TECH_STRING( SRCH4_CHK_QUEUE_TECH( SRCH4_HPQ1 ) ),
    SRCH4_TECH_STRING( SRCH4_CHK_QUEUE_TECH( SRCH4_LPQ  ) ) );
  #elif defined( SRCH4_USE_LPQ )
    MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      " Queues: HPQ0=%s, LPQ=%s",
    SRCH4_TECH_STRING( SRCH4_CHK_QUEUE_TECH( SRCH4_HPQ0 ) ),
    SRCH4_TECH_STRING( SRCH4_CHK_QUEUE_TECH( SRCH4_LPQ  ) ) );
  #else
  MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
      " Queues: HPQ0=%s",
    SRCH4_TECH_STRING( SRCH4_CHK_QUEUE_TECH( SRCH4_HPQ0 ) ) );
  #endif

} /* srch4trans_print_queue_status */


/*===========================================================================

FUNCTION       SRCH4TRANS_RET_TRANS
DESCRIPTION    This function "translates" srch4drv return values into
               srch4trans return values.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
static int32 srch4trans_ret_trans
(
  int32 value       /* srch4drv's return value */
)
{
  /*-----------------------------------------------------------------------*/

  switch ( value )
  {
    case SRCH4DRV_RET_FULL:
    {
      return( SRCH4_RET_FULL );
    }
    case SRCH4DRV_RET_ACTIVE:
    {
      return( SRCH4_RET_ACTIVE );
    }
    case SRCH4DRV_RET_UNCLEAN:
    {
      return( SRCH4_RET_UNCLEAN );
    }
    case SRCH4DRV_RET_INVALID_PARAM:
    {
      return( SRCH4_RET_INVALID_PARAM );
    }
    case SRCH4DRV_RET_INVALID_QUEUE:
    {
      return( SRCH4_RET_INVALID_QUEUE );
    }
    case SRCH4DRV_RET_TECH_MISMATCH:
    {
      return( SRCH4_RET_TECH_MISMATCH );
    }
    case SRCH4DRV_RET_FATAL:
    {
      return( SRCH4_RET_FATAL );
    }
    case SRCH4DRV_RET_OK:
    {
      return( SRCH4_RET_OK );
    }
    default:
    {
      /* all valid cases should be accounted for */
      return( SRCH4_RET_OK );
    }
  }

} /* srch4trans_ret_trans */


#ifdef SRCH4_AFLT_ENABLED
/*===========================================================================

FUNCTION       SRCH4TRANS_AFLT_ABORT_CB
DESCRIPTION    This function calls the lost dump callback for AFLT.

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   AFLT state machine gets a lost dump.

===========================================================================*/
static void srch4trans_aflt_abort_cb(uint32 num_results, void *sect)
{
  srch4trans.queue.tech[ SRCH4_AFLT ].lost_cb_func( sect );
}
#endif /* SRCH4_AFLT_ENABLED */


/*===========================================================================

FUNCTION       SRCH4TRANS_1X_ABORT_CB
DESCRIPTION    This function calls the lost dump callback for 1X.

DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   1X state machine gets a lost dump.

===========================================================================*/
static void srch4trans_1x_abort_cb(uint32 num_results, void *sect)
{
  srch4trans.queue.tech[ SRCH4_1X ].lost_cb_func( sect );
}


/*===========================================================================

FUNCTION       SRCH4_FLUSH_SAMPLE_RAM
DESCRIPTION    This function flushes the sample ram in the DSP if no searches
                are active
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   Sample ram in DSP is flushed

===========================================================================*/
int32 srch4_flush_sample_ram
(
  sample_server_rx_chain_type samp_serv_chain, /* The sample server chain */
  uint16                      position,        /* position in chips */
  boolean                     scheduled        /* do a scheduled flush? */
)
{
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                      /* return value from the drivers */

  /*-----------------------------------------------------------------------*/

  #ifdef SRCH4_AFLT_ENABLED
  /* If one of the queues is active. Abort it. */
  if (srch4_aflt_active())
  {
    srch4_aflt_abort_search(srch4trans_aflt_abort_cb, NULL);
    MSG_HIGH("Flush Sample Ram, Aborting AFLT search",0,0,0);
  }
  #endif /* SRCH4_AFLT_ENABLED */
  if (srch4_1x_active()) 
  {
    srch4_1x_abort_search(srch4trans_1x_abort_cb, NULL, TRUE, FALSE);
    MSG_HIGH("Flush Sample Ram, Aborting 1X search",0,0,0);
  }
  /* HDR search may need to be aborted in future */

  drv_ret_val = srch4_flush_s_ram( samp_serv_chain, position, scheduled );

  return( srch4trans_ret_trans( drv_ret_val ) );

} /* srch4_flush_sample_ram */


/*===========================================================================

FUNCTION       SRCH4_GET_EST_SRCH_TIME
DESCRIPTION    This function returns the estimated search time of a single
                search based on the parameters given
DEPENDENCIES   Search Time estimates are for HPQ only
               To estimate total srch time, the queue's overhead
                (SRCH4_SW_QUEUE_OVHD_US) must be added
               Must be passed coh_len/non_coh_len pairs [512,1]

RETURN VALUE   Estimated search time in us (value of "~0" indicates an error)
SIDE EFFECTS   None

===========================================================================*/
uint32 srch4_get_est_srch_time
(
  uint16    window_size,               /* window size (chips x8)   */
  uint16    coherent_len,              /* coherent length          */
  uint8     non_coherent_len           /* non-coherent length      */
)
{
  uint32    est_time = ~0;             /* calculated srch time est */
  uint16    win_siz;                   /* window size (chips x8)   */

  /*-----------------------------------------------------------------------*/

  /* srch parameters of M=512, N=1 */
  if( ( coherent_len == 512 ) && ( non_coherent_len == 1 ))
  {
    /* convert window size in cx8 units to win_siz in 8 chip units
     * scaled to an 8 chip boundary  ((window_size+7*8)/64)
     */
    win_siz = (window_size + 56) >> 6;

    /* experimentally the search time was found to be (in us) for M=512, N=1
        srch_time = window_size * 1.8 + 30
       note:
         (win_siz * 231) >> 7 is 1.805
         rounding in the multiplier may introduce an error of Xus */
    est_time = ( win_siz * 16 ) - ( win_siz * 2 ) + ( win_siz / 2 ) +
               SRCH4_SW_TASK_OVHD_US;

    /* Also there is software overhead of 187 us (SRCH4_SW_QUEUE_OVHD_US)
        that has not been added */

    if ( est_time == (uint32)~0 )
    {
      est_time--;
    }
  }
  else
  {
    MSG_HIGH( "SRCH4 - Estimating time for invalid N(%d) M(%d), invalid resp",
              non_coherent_len, coherent_len, 0 );
  }

  /* return the calculated estimated search time or an error */
  /* Note: To estimate total srch time, the queue's overhead
                (SRCH4_SW_QUEUE_OVHD_US) must be added       */
  return( est_time );

} /* srch4_get_est_srch_time */


/*===========================================================================

FUNCTION       SRCH4_TRANS_CLEANUP_DUMP
DESCRIPTION    This is used to do general cleanup for queues after a status
               dump is received (abort, lost, dump)
DEPENDENCIES   Note this function will be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_trans_cleanup_dump
(
  srch4_srch_type tech                  /* technology that just dumped */
)
{
  /*-----------------------------------------------------------------------*/

  NOTUSED( tech );

} /* srch4_trans_cleanup_dump */


/*===========================================================================

           T E C H N O L O G Y   M A P P I N G    F U N C T I O N S

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4_REQUEST_QUEUE_ABORT_CALLBACK
DESCRIPTION    This is an abort callback, used when a deregister is called
                while a search is active. This callback function will call
                srch4_rerequest_queue() to assign any waiting technology a
                queue.
DEPENDENCIES   Note this function will be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   A queue could be assigned

===========================================================================*/
static void srch4_request_queue_abort_callback
(
  uint32            num_searched,              /* Num of sectors returned */
  void*             cb_data                    /* User callback data      */
)
{
  /*-----------------------------------------------------------------------*/

  NOTUSED( num_searched );
  NOTUSED( cb_data );

  srch4_rerequest_queue();

} /* srch4_request_queue_abort_callback */



/*===========================================================================

FUNCTION       SRCH4_REQUEST_QUEUE
DESCRIPTION    This is used to request a queue for a technology
DEPENDENCIES   Note this function can be called in ISR context

RETURN VALUE   The assigned queue
SIDE EFFECTS   The specified technology's queue is assigned

===========================================================================*/
static srch4_queue_type srch4_request_queue
(
  srch4_srch_type   technology                 /* tech requesting a queue */
)
{
  srch4_srch_type   tech;                      /* tech requesting a queue */
  srch4_queue_type *requestor;                 /* ptr for brevity         */
  srch4_queue_type  i;                         /* loop var for queues     */

  /*-----------------------------------------------------------------------*/

  /* the SRCH4_1X_OFFTL_MASK tech, shares with SRCH4_1X, so this slaves
     them to each other, but change a 1X request to it's mask */
  if ( technology == SRCH4_1X )
  {
    technology = SRCH4_1X_NORM_MASK;
  }
  tech = technology & 0xF;

  /* check for a valid technology */
  if ( !( ( tech == SRCH4_1X )
          #ifdef SRCH4_HDR_ENABLED
          ||
          ( tech == SRCH4_HDR_ACCUM ) ||
          ( tech == SRCH4_HDR_SHOULDERS )
          #endif
          #ifdef SRCH4_AFLT_ENABLED
          ||
          ( tech == SRCH4_AFLT )
          #endif
     )
     )
  {
    ERR( " Invalid tech(%d) sent request for a queue", tech, 0, 0 );
    return( SRCH4_NO_QUEUE );
  }

  /* assign pointers for brevity */
  requestor = &srch4trans.queue.tech[ tech ].queue;

  /* Check if it's not already assigned a queue */
  if( *requestor == SRCH4_NO_QUEUE )
  {
    #ifdef SRCH4_AFLT_ENABLED
    /* AFLT can only be assigned to the LPQ */
    if( tech == SRCH4_AFLT )
    {
      i = SRCH4_LPQ;

      /* check to ensure that this queue is not already assigned to another
         technology and that there is not an active search on this queue */
      if ( ( SRCH4_CHK_QUEUE_TECH( i ) == SRCH4_NO_TECH ) &&
           ( srch4_queue_available( i ) ) )
      {
        *requestor  = i;
        srch4trans.queue.queue_assn[ i ] = tech;
      }
      else
      {
        ERR( "AFLT requested LPQ and did not get it", 0, 0, 0 );
      }
    }
    else /* tech != AFLT */
    #endif /* SRCH4_AFLT_ENABLED */
    {
      /* check which queues are taken, then assign a free queue to the
         requestors */
      for( i = SRCH4_HPQ0; i < SRCH4_MAX_Q; i++ )
      {
        #ifdef SRCH4_USE_LPQ
        /* the LPQ can not be assigned to anything besides AFLT, so jump
           over it here */
        if( !( i == SRCH4_LPQ ) )
        #endif /* SRCH4_USE_LPQ */
        {
          /* check to ensure that this queue is not already assigned to another
             technology */
          if ( ( SRCH4_CHK_QUEUE_TECH( i ) == SRCH4_NO_TECH ) &&
               ( srch4_queue_available( i ) ) )
          {
            *requestor  = i;
            srch4trans.queue.queue_assn[ i ] = technology;

            break;
          }
        }
      } /* for */

      if ( i == SRCH4_NO_QUEUE )
      {
        if ( srch4trans.queue.tech[ tech ].reg_cb_func == NULL )
        {
          ERR( "Tech: %d queue request, none avail", tech, 0, 0 );
        }
        else
        {
          MSG_HIGH( "Tech: %d queue request, none avail, cb on assn installed",
                    tech, 0, 0 );
        }
      }
    } /* tech == AFLT */
  }
  else
  {
    MSG_HIGH(" re-request of a queue 0x%x->0x%x ",
              srch4trans.queue.queue_assn[ *requestor ],
              (srch4trans.queue.queue_assn[ *requestor ] | technology), 0 );
    /* confirm the assignment, and slave 1X_OFFTL_QPCH to 1X */
    srch4trans.queue.queue_assn[ *requestor ] |= technology;
  }

  return( *requestor );

} /* srch4_request_hpq */


/*===========================================================================

FUNCTION       SRCH4_RELEASE_QUEUE
DESCRIPTION    This is used to release a queue that a technology has
                been assigned
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   The input technology's queue is unassigned
               If the queue was active, it will be aborted, w/o a callback

===========================================================================*/
static void srch4_release_queue
(
  srch4_srch_type  technology           /* technology releasing it's queue */
)
{
  srch4_srch_type  tech;                /* technology releasing it's queue */
  srch4_queue_type released_queue;      /* queue that was released */

  /*-----------------------------------------------------------------------*/

  /* the SRCH4_1X_OFFTL_MASK tech, shares with SRCH4_1X, so this slaves
     them to each other, but change a 1X request to it's mask */
  if ( technology == SRCH4_1X )
  {
    technology = SRCH4_1X_NORM_MASK;
  }
  tech = technology & 0xF;

  /* check for a valid technology */
  if ( !( ( tech == SRCH4_1X )
          #ifdef SRCH4_HDR_ENABLED
          ||
          ( tech == SRCH4_HDR_ACCUM ) ||
          ( tech == SRCH4_HDR_SHOULDERS ) 
          #endif
          #ifdef SRCH4_AFLT_ENABLED
          ||
          ( tech == SRCH4_AFLT ) 
          #endif
        )
     )
  {
    ERR( " Invalid tech(%d) sent request to release a queue", tech, 0, 0 );
    return;
  }

  /* save released queue, and reset the tech's queue var */
  released_queue = srch4trans.queue.tech[ tech ].queue;

  /* if a queue wasn't owned, exit */
  if ( released_queue == SRCH4_NO_QUEUE )
  {
    return;
  }

  /* need to be careful on the release queue of SRCH4_1X || SRCH4_1X_OFFTL_MASK
     Since they are slaves to each other, keep track of when either is reg'ed */
  if ( ( technology == SRCH4_1X_NORM_MASK ) &&
       ( srch4trans.queue.queue_assn[ released_queue ] & SRCH4_1X_OFFTL_MASK ) )
  {
    srch4trans.queue.queue_assn[ released_queue ] &= ~SRCH4_1X_NORM_MASK;

    /* release queue, if srch4 queue is holding by SRCH4_1X client */ 
    if (srch4trans.queue.queue_assn[ released_queue ] != SRCH4_1X) 
    {
       MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                     "1X requested queue release while 1X OFFTL QPCH reg'ed (%s)",
                     srch4_queue_str[ released_queue ] );
       return;
    }
  }
  if ( ( technology == SRCH4_1X_OFFTL_MASK ) &&
       ( SRCH4_CHK_QUEUE_TECH( released_queue ) == SRCH4_1X ) )
  {
    srch4trans.queue.queue_assn[ released_queue ] &= ~SRCH4_1X_OFFTL_MASK;

    /* release queue, if srch4 queue is holding by SRCH4_1X client */ 
    if (srch4trans.queue.queue_assn[ released_queue ] != SRCH4_1X) 
    {
       MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                     "1X OFFTL QPCH requested queue release while 1X reg'ed (%s)",
                     srch4_queue_str[ released_queue ] );

        return;
    }
  }

  srch4trans.queue.tech[ tech ].queue           = SRCH4_NO_QUEUE;
  srch4trans.queue.queue_assn[ released_queue ] = SRCH4_NO_TECH;

  MSG_SPRINTF_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
                 "A Queue was released  by tech %s, released %s",
                 SRCH4_TECH_STRING( tech ),
                 srch4_queue_str[ released_queue ] );

  /* is the queue active or does it have outstanding programmed tasks?
     if so, stop the searches or clear the queues */
  if ( srch4_tasks_left( tech, released_queue ) >= SRCH4DRV_RET_UNCLEAN )
  {
    /* this is a sync abort, so by the time this is released, srch4 will
       be idle (at least for this technology) */
    (void) srch4_abort_queue( tech, released_queue,
                              srch4_request_queue_abort_callback, NULL,
                              TRUE, FALSE );
  }
  else
  {
    /* since a queue was "actually" released and free'd from the
       former technology, check to see if any technologies
       are waiting for queues */
    srch4_rerequest_queue();
  }

} /* srch4_release_queue */


/*===========================================================================

FUNCTION       SRCH4_REREQUEST_QUEUE
DESCRIPTION    This is used to check if any technologies have requested a
               queue, but wasn't assigned one, and have registered a callback
               to be informed when a queue is assigned to them
DEPENDENCIES   An empty queue will be assigned according to the order of the
               enum srch4_srch_type. This may need to be re-investigated to
               assign based on search priority
               Note this function can be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   If a queue is assigned, it's registered reg_cb_func will be
               called

===========================================================================*/
static void srch4_rerequest_queue( void )
{
  /* only do the check it there are more than one technology present */
  #if ( defined( SRCH4_HDR_ENABLED ) || defined( SRCH4_AFLT_ENABLED ) )

  srch4_srch_type  i;                   /* loop var                */

  /*-----------------------------------------------------------------------*/

  /* check to see if any technologies are waiting to be assigned a queue */
  for ( i = SRCH4_1X; i < SRCH4_NUM_TECH; i++ )
  {
    if ( srch4trans.queue.tech[ i ].reg_cb_func != NULL )
    {
      /* call the request queue function, with the technology type */
      /* request a queue */
      if ( srch4_request_queue( i ) == SRCH4_NO_QUEUE )
      {
        MSG_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
               "Failed another registration attempt for %d",
               i );
      }
      else
      {
        MSG_2( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
               "%d registered with priority %d",
               i, ( ( uint32 ) srch4trans.queue.tech[ i ].priority >> 3 ) );

        /* notify the technology that it's queue is now ready */
        srch4trans.queue.tech[ i ].reg_cb_func(
                                     srch4trans.queue.tech[ i ].reg_cb_data );

        /* NULL out the reg_cb_func to ensure it's not called again */
        srch4trans.queue.tech[ i ].reg_cb_func = NULL;
      }
    }
  }
  #endif /* SRCH4_HDR_ENABLED || SRCH4_AFLT_ENABLED */

} /* srch4_rerequest_queue */


/*===========================================================================

           S R C H _ W I N _ C E N T E R    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_SET_WIN_CENTER
DESCRIPTION    This function sets searcher 4's window center value
DEPENDENCIES   Window center is in ChipX8

RETURN VALUE   None
SIDE EFFECTS   srch4trans.win_center is set

===========================================================================*/
void srch4_set_win_center
(
  int32 win_center                       /* new window center (ChipX8) */
)
{
  /*-----------------------------------------------------------------------*/

  /* set the new win center */
  srch4trans.misc.win_center  = win_center;

  /* Make the window center positive, in the range 0 ... 262143 */
  srch4trans.misc.win_center &= PN_CIRCLE_MASK;

} /* srch4_set_win_center */


/*===========================================================================

FUNCTION       SRCH4_GET_WIN_CENTER
DESCRIPTION    This function gets searcher 4's window center value (chipX8)
DEPENDENCIES   None

RETURN VALUE   srch4's window center (chipX8) (in the range 0 ... 262143)
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_get_win_center( void )
{
  /*-----------------------------------------------------------------------*/

  /* return the current window center value */
  return( srch4trans.misc.win_center );

} /* srch4_get_win_center */


/*===========================================================================

FUNCTION       SRCH4_SLEW_WIN_CENTER
DESCRIPTION    This function slews searcher 4's window center value and
                returns the new window center (chipX8)
DEPENDENCIES   None

RETURN VALUE   New window center value for srch4  (chipX8)
SIDE EFFECTS   srch4trans.misc.win_center is slewed

===========================================================================*/
int32 srch4_slew_win_center
(
  int32 slew_value                       /* slew amount */
)
{
  /*-----------------------------------------------------------------------*/

  /* add the corresponding slew */
  srch4trans.misc.win_center += slew_value;

  /* Make the window center positive, in the range 0 ... 262143 */
  srch4trans.misc.win_center &= PN_CIRCLE_MASK;

  /* return the new window center value */
  return( srch4trans.misc.win_center );

} /* srch4_slew_win_center */


/*===========================================================================

      1 X    T R A N S L A T I O N    U T I L I T Y    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       UTIL_1X_COPY_RESULTS_TO_SECT
DESCRIPTION    Copies the results of the search to a results buffer in the
               sector structure.
DEPENDENCIES   Note this function will be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
static void util_1x_copy_results_to_sect
(
  srch4_task_struct_type    *task_res,   /* array of task struct (results) */
  srch_sect_result_type     *results,    /* results pointer                */
  srch_sect_parm_type       *parms       /* parms for lecio conversion     */
)
{
  uint32                     peak_inx;   /* loop index                     */
  uint8                      trunc;      /* truncation factor              */
  uint16                     peak_eng;   /* peak energy                    */

  /*-----------------------------------------------------------------------*/

  if ( ( task_res == NULL ) || ( results == NULL ) || ( parms == NULL ) )
  {
    ERR( "util_1x_copy_results_to_sect_path got NULL ptr", 0, 0, 0 );

    return;
  }

  trunc = srch_conv_mnrd_to_trunc( parms->coh_int,
                                   parms->non_coh_int );

  /* loop over all peaks */
  for( peak_inx = 0; peak_inx < SRCH4_NUM_PEAKS; peak_inx++ )
  {
    /* Take care of non returned peaks */
    if ( ( task_res->peak_eng[peak_inx] == 0 ) &&
        ( peak_inx > 0 ) )
    {
      /* no peak was returned, so steal the pos and inx from the
         previous peak. There will always be at least 1 peaks, and
         this will only happen for window sizes less than 12 chips */
      results->max_pos[peak_inx] = results->max_pos[peak_inx-1];

      /* set peak lecio to 0 */
      results->max_lecio[peak_inx] = 0;
    }
    else
    {
      /* store peak position (convert to Cx8) */
      results->max_pos[peak_inx] = (int32) task_res->peak_pos[peak_inx] /
                                   CHIPX16_PER_EIGHTH_CHIP;

      if (task_res->diversity == SRCH4_ANT_DUAL)
      {
        /* normalize the peak energy for diversity searches to match
           the truncation value for a non-diversity search. This assumes
           that the trunc value for a diversity search is always T+1 when
           compared to that of a non-diversity search, so the energy must
           be increased by 2^2 */
        peak_eng = task_res->peak_eng[peak_inx] << 2;
      }
      else
      {
        peak_eng = task_res->peak_eng[peak_inx];
      }

      /* store peak lecio */
      results->max_lecio[peak_inx] =
        srch_conv_eng_to_lecio( peak_eng,
                                parms->coh_int,
                                parms->non_coh_int,
                                trunc );

    }
  } /* for peak_inx */

} /* util_1x_copy_results_to_sect */


#ifdef FEATURE_IS2000_REL_A_TD
/*===========================================================================

FUNCTION       UTIL_1X_MERGE_TD_RESULTS
DESCRIPTION    This function merges the common and diversity paths from
                a transmit diversity search.
DEPENDENCIES   Sector must have a TD pilot

RETURN VALUE   None
SIDE EFFECTS   This updates the sector structure with the merged results.

===========================================================================*/
static void util_1x_merge_td_results
(
  srch4_task_struct_type *task_res,       /* array of task struct (results)*/
  srch_sect_result_type  *results,        /* sector results                */
  srch_sect_parm_type    *parms           /* sector parameters             */
)
{
  uint32  td_power = 0;                   /* Index of TD power level        */
  uint32  i = 0, j = 0, k = 0;            /* merging loop indexes           */
  uint16  m_pos[2*SRCH_MAX_PEAKS];        /* merged positions               */
  uint16  m_lecio[2*SRCH_MAX_PEAKS];      /* merged linear ec/ios           */
  uint16  t_lecio;                        /* temporary lecios               */
  uint32  t_index;                        /* temporary index to array       */
  boolean com_used[SRCH_MAX_PEAKS];       /* used to eliminate common peaks */
                                          /*  from merge consideration      */
  boolean div_used[SRCH_MAX_PEAKS];       /* used to eliminate diversity    */
                                          /*  peaks from merge consideration*/
  int32   div_pos;                        /* diversity position for results */
  uint8   trunc;                          /* truncation factor              */

  /*-----------------------------------------------------------------------*/

  if ( ( task_res == NULL ) || ( results == NULL ) || ( parms == NULL ) )
  {
    ERR("Trying to process invalid sector", 0, 0, 0);

    return;
  }

  trunc = srch_conv_mnrd_to_trunc( parms->coh_int,
                                   parms->non_coh_int );

  /* Blank the merged results arrays */
  memset( m_pos, 0, 2*SRCH_MAX_PEAKS*sizeof(uint16) );
  memset( m_lecio, 0, 2*SRCH_MAX_PEAKS*sizeof(uint16) );

  /* initialize used variables */
  for( i = 0; i < SRCH4_NUM_PEAKS; i++ )
  {
    com_used[i] = FALSE;
    div_used[i] = FALSE;
  } /* for peak_inx */

  /* get the TD power multiplier */
  /*lint -save -e504 Unusual shift operation */
  td_power = 1 << (parms->pilot_rec.data.rec0.td_power_level ^ 0x03);
  /*lint -restore */

  /* don't merge peaks with max energies of 0 (non-returned peaks) */
  /* for all common peaks */
  for ( i = 0; i < SRCH4_NUM_PEAKS; i++ )
  {
    if ( results->max_lecio[i] != 0 )
    {
      /* for all diversity peaks */
      for ( j = 0; j < SRCH4_NUM_PEAKS; j++ )
      {
        /* calculate the peak pos and mask it to get a value within range */
        div_pos = ( task_res->peak_pos[j] / CHIPX16_PER_EIGHTH_CHIP ) &
                  PN_CIRCLE_MASK;

        /* check for proximity between peaks */
        /* don't merge peaks with max energies of 0 (non-returned peaks) */
        if ( ( ( ABS( results->max_pos[i] - div_pos ) & PN_CIRCLE_MASK )
               <= SRCH4_TD_PROXIMITY ) &&
             ( task_res->peak_eng[j] != 0 ) )
        {
          /* average the path positions */
          m_pos[k] = ( results->max_pos[i] +
                       task_res->peak_pos[j] ) / 2;

          /* combine path lecios as (P1 + 1/a * P2) /2 */
          m_lecio[k] = ( results->max_lecio[i] +
                         ( td_power * srch_conv_eng_to_lecio(
                                        task_res->peak_eng[j],
                                        parms->coh_int,
                                        parms->non_coh_int,
                                        trunc ) ) ) / 2;

          com_used[i] = TRUE;
          div_used[j] = TRUE;

          k++;
        }
      } /* for diversity paths */
    } /* if common_path energy != 0 */
  } /* for common paths */

  /* add paths that were not merged */
  for ( i = 0; i < SRCH4_NUM_PEAKS; i++ )
  {
    if ( !com_used[i] )
    {
      /* Add to the merged path list, with scaled energies */
      m_pos[k] = results->max_pos[i];
      m_lecio[k] = results->max_lecio[i] / 2;

      k++;
    }
    if ( !div_used[i] )
    {
      /* Add to the merged path list, with scaled energies */
      m_pos[k] = task_res->peak_pos[i];
      m_lecio[k] = srch_conv_eng_to_lecio( td_power * task_res->peak_eng[i],
                                           parms->coh_int,
                                           parms->non_coh_int,
                                           trunc ) / 2;

      k++;
    }
  }

  /* store the strongest merged peaks */
  for ( j=0; j<SRCH_MAX_PEAKS; j++ )
  {
    t_lecio = m_lecio[0];
    t_index = 0;

    /* find the (next) strongest peak */
    for ( i = 1; i < k; i++ )
    {
      if ( m_lecio[i] > t_lecio )
      {
        t_lecio = m_lecio[i];
        t_index = i;
      }
    }

    /* store the strongest merged peaks into the sect struct */
    results->max_pos[j] = m_pos[t_index];
    results->max_lecio[j] = m_lecio[t_index];

    /* zero out this energy so we can get the next strongest peak */
    m_lecio[t_index] = 0;
  }

} /* util_1x_merge_td_results */
#endif /* FEATURE_IS2000_REL_A_TD */


/*===========================================================================

FUNCTION       UTIL_1X_COPY_SECT_TO_TASK
DESCRIPTION    Copies the sector structure's search parameters to the task
                structure
DEPENDENCIES   None.

RETURN VALUE   None
SIDE EFFECTS   A task is filled with the sector's parameters

===========================================================================*/
static void util_1x_copy_sect_to_task
(
  srch_sect_parm_type     *parms,        /* sector to merge             */
  srch_sect_result_type   *results,      /* sector to merge             */
  srch4_task_struct_type  *task,         /* ptr to HPQ/LPQ task struct  */
  uint16                   walsh,        /* walsh code                  */
  boolean                  rd            /* flag to indicate use of RD  */
)
{
  int32                    pn_pos = 0;    /* for pn_pos calcs              */
  uint32                   pn_pos_n = 0;  /* for pn_pos calcs (normalized) */
  uint8                    trunc;         /* truncation factor             */

  /*-----------------------------------------------------------------------*/

  if ( ( parms == NULL ) || ( results == NULL ) || ( task == NULL ) )
  {
    ERR( "util_1x_copy_sect_to_task got NULL parms, results, or task ptr",
         0, 0, 0 );

    return;
  }

  trunc = srch_conv_mnrd_to_trunc( parms->coh_int,
                                   parms->non_coh_int );

  #ifndef FEATURE_CDMA_RX_DIVERSITY
  NOTUSED( rd );
  #endif

  /* store the sector pointer */
  task->parm_ptr = parms;
  task->result_ptr = results;

  /* calculate the pn_pos */
  pn_pos   = srch4_get_win_center() - ( parms->win_size / 2 );

  /* apply the window offset and any additional offsets */
  pn_pos  += parms->win_pos_offset;

  /* prepare this for rounding and get this to a range from 0 ... 262143 */
  pn_pos_n  = ( pn_pos + CHIPX8_PER_HALF_CHIP ) & PN_CIRCLE_MASK;

  /* round the PN position to Chip resolution for the MDSP
     this will move the search window up to 1/2 chip for the expected
     search window */
  pn_pos_n /= CHIPX8_PER_CHIP;

  /* Calculate the programmed window position
     note: win_siz is expected to be on a chip boundary */
  task->window_pos       = ( pn_pos_n * CHIPX8_PER_CHIP ) +
                           (uint32) parms->win_size;

  /* ensure the window position is from 0 ... 262143 */
  /* this will be stored into the sector structure upon getting the results */
  task->window_pos      &= PN_CIRCLE_MASK;

  /* store the PN position, this is the start of the window */
  task->pn_pos           = (uint16) pn_pos_n;

  /* catch the special case window sizes */
  if ( ( parms->win_size == FULL_CIRCLE ) ||
       ( parms->win_size >
         ( SRCH4_MAX_WINDOW_SIZE * CHIPX8_PER_CHIP ) ) )
  {
    task->window_size    = (uint16)SRCH4_MAX_WINDOW_SIZE;
  }
  else
  {
    task->window_size    = (uint16)( parms->win_size / CHIPX8_PER_CHIP );
  }

  /* Convert the sector structure into a task structure */
  task->pilot_inx        = (uint16)(parms->pilot);
  task->phase            = 0;
  #ifdef T_MDM7800
  task->carrier          = SRCH4_CARRIER_0;
  #endif /* T_MDM7800 */
  task->coherent_len     = parms->coh_int;
  task->coherent_trunc   = srch4_coh_tr[TRUNC2GAIN(trunc)];
  task->non_coherent_len = parms->non_coh_int;
  task->qof              = 0;
  task->walsh            = walsh;
  task->burst_len        = 0;
  task->pilot_set_mask   = parms->pilot_set_mask;

  #ifdef FEATURE_CDMA_RX_DIVERSITY
  /* for diversity support */
  switch ( srch_rx_get_device( DEMOD_CHAIN ) )
  {
    case RFCOM_RECEIVER_1:
    {
      task->diversity    = SRCH4_ANT_1;
      break;
    }
    case RFCOM_RECEIVER_DIV:
    {
      /* check if Receive diversity is NOT to be used */
      if ( rd )
      {
        /* use the assigned Receive diversity */
        task->diversity = SRCH4_ANT_DUAL;
        /* The trunc value for a diversity search is always T+1 when
           compared to that of a non-diversity search */
        task->coherent_trunc++;
      }
      else
      {
        /* don't use RD, so assign search to ANT_0,
           since both are reserved for 1x */
        task->diversity = SRCH4_ANT_0;
      }
      break;
    }
    case RFCOM_TRANSCEIVER_0:
    default:
    {
      task->diversity    = SRCH4_ANT_0;
    }
  }/* switch */
  #else /* FEATURE_CDMA_RX_DIVERSITY */
  task->diversity        = SRCH4_ANT_0;
  #endif /* FEATURE_CDMA_RX_DIVERSITY */

  /* assign the 1X processing call back function */
  task->process_results  = srch4_1x_read_result;

} /* util_1x_copy_sect_to_task */


/*===========================================================================

           1 X    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_1X_REGISTER
DESCRIPTION    This function registers 1x with searcher 4. Once registered
                1x will "own" a queue, and it's callbacks will be set.
DEPENDENCIES   If reg_cb_func() is not specified, this is a one shot attempt
                at registering for a queue, if reg_cb_func() is defined, then
                a queue will be assigned to 1X sometime in the future.

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_1x_register
(
  srch4_priority_type priority,         /* search priority                 */
  int16  freq_offset,                   /* freq offset ( 4.6875 Hz/LSB )   */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void (*reg_cb_func)(void *),          /* pointer to a callback function  */
                                        /*  to be called upon successful   */
                                        /*  assignment to a queue          */
  void  *reg_cb_data                    /* register callback data          */
)
{

  boolean          app_valid;           /* mdsp app properly enabled?      */
  srch4_queue_type q;                   /* the queue that was assigned     */

  /*-----------------------------------------------------------------------*/

  /* enable searcher 4 in the MDSP */
  app_valid = srch4_enable( SRCH4_1X_MASK, TRUE );

  /* always save off the 1X parameters */
  srch4trans.queue.tech[ SRCH4_1X ].priority     = priority;
  srch4trans.queue.tech[ SRCH4_1X ].freq_offset  = freq_offset;
  srch4trans.queue.tech[ SRCH4_1X ].dump_cb_func = dump_cb_func;
  srch4trans.queue.tech[ SRCH4_1X ].lost_cb_func = lost_cb_func;

  /* check if we are re-registering, but have yet to call the reg_cb_func */
  if ( srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func == NULL )
  {
    /* first time register or valid re-register */
    srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func  = reg_cb_func;
    srch4trans.queue.tech[ SRCH4_1X ].reg_cb_data  = reg_cb_data;
  }
  else if ( srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func != reg_cb_func )
  {
    /* re-registering when a callback was still outstanding */
    ERR( "Re-registering 1X with an outstanding reg_cb_func, not updating",
         0, 0, 0 );
  }

  if( app_valid )
  {
    /* request a queue */
    q = srch4_request_queue( SRCH4_1X );

    if ( q == SRCH4_NO_QUEUE )
    {
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech %s, none available",
        srch4_tech_str[SRCH4_1X] );
      srch4trans_print_queue_status();

      return( SRCH4_RET_NO_QUEUES );

    }
    else
    {
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech %s, assigned %s, priority %s",
        srch4_tech_str[SRCH4_1X],
        srch4_queue_str[q],
        srch4_prio_str[ (uint32)( priority ) >> 3 ] );
      srch4trans_print_queue_status();
    }

    if ( srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func != NULL )
    {
      /* notify the technology that it's queue is now ready */
      srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func( reg_cb_data );

      /* NULL out the reg_cb_func to ensure it's not called again */
      srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func = NULL;
    }

    return( SRCH4_RET_OK );
  }
  else
  {
    return( SRCH4_RET_APP_NOT_VALID );
  }

}  /* srch4_1x_register */


/*===========================================================================

FUNCTION       SRCH4_1X_DEREGISTER
DESCRIPTION    This function deregisters 1x from it's assigned queue
               This function can also be used to unregister a reg_cb_func on
                a failed registration and before the queue has been assigned
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_1x_deregister ( void )
{
  /*-----------------------------------------------------------------------*/

  /* deregister searcher 4's 1X parameters */
  srch4trans.queue.tech[ SRCH4_1X ].priority     = SRCH4_PRIO_MIN;
  srch4trans.queue.tech[ SRCH4_1X ].freq_offset  = 0;
  srch4trans.queue.tech[ SRCH4_1X ].dump_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_1X ].lost_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_1X ].reg_cb_func  = NULL;
  srch4trans.queue.tech[ SRCH4_1X ].reg_cb_data  = NULL;

  /* release the assigned queue */
  srch4_release_queue( SRCH4_1X );

  /* disable searcher 4 in the MDSP if SRCH4_1X_NORM_MASK && SRCH4_1X_OFFTL_MASK
     are both disabled ) */
  if ( srch4trans.queue.tech[ SRCH4_1X ].queue == SRCH4_NO_QUEUE )
  {
  (void) srch4_enable( SRCH4_1X_MASK, FALSE );
  }

}  /* srch4_1x_deregister */


/*===========================================================================
FUNCTION       SRCH4_1X_OFFTL_REGISTER
DESCRIPTION    This function registers 1x offtl with searcher 4. Once registered
                1x offtl will "own" a queue (shared with normal 1x), the offtl
                registration does not set callbacks
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_1x_offtl_register( void )
{

  boolean          app_valid;           /* mdsp app properly enabled?      */
  srch4_queue_type q;                   /* the queue that was assigned     */

  /*-----------------------------------------------------------------------*/

  MSG_LOW("1x offtl_qpch reg srch4 (prev %d)",
            srch4trans.queue.tech[ SRCH4_1X_OFFTL_MASK ].queue,0,0 );

  /* enable searcher 4 in the MDSP */
  app_valid = srch4_enable( SRCH4_1X_MASK, TRUE );

  if ( app_valid )
  {
    /* request a queue */
    q = srch4_request_queue( SRCH4_1X_OFFTL_MASK );

    if ( q == SRCH4_NO_QUEUE )
    {
      MSG( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech 1X_OFFTL, none available" );

      srch4trans_print_queue_status();

      return( SRCH4_RET_NO_QUEUES );
    }
    else
    {
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech 1X_OFFTL, assigned %s",
        srch4_queue_str[q] );

      srch4trans_print_queue_status();
    }

    return( SRCH4_RET_OK );
  }
  else
  {
    return( SRCH4_RET_APP_NOT_VALID );
  }

}  /* srch4_1x_offtl_register */


/*===========================================================================

FUNCTION       SRCH4_1X_OFFTL_DEREGISTER
DESCRIPTION    This function deregisters 1x offtl from it's assigned queue
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_1x_offtl_deregister ( void )
{
  /*-----------------------------------------------------------------------*/

  MSG_LOW("1x offtl_qpch dereg srch4 %d",
            srch4trans.queue.tech[ SRCH4_1X_OFFTL_MASK ].queue,0,0 );

  /* release the assigned queue */
  srch4_release_queue( SRCH4_1X_OFFTL_MASK );

  /* disable searcher 4 in the MDSP
     (only if SRCH4_1X_NORM_MASK && SRCH4_1X_OFFTL_MASK are both disabled ) */
  if ( srch4trans.queue.tech[ SRCH4_1X ].queue == SRCH4_NO_QUEUE )
  {
    (void) srch4_enable( SRCH4_1X_MASK, FALSE );
  }

}  /* srch4_1x_offtl_deregister */


/*===========================================================================

FUNCTION       SRCH4_1X_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns TRUE if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_1x_active( void )
{
  /*-----------------------------------------------------------------------*/

  if ( srch4trans.queue.tech[ SRCH4_1X ].queue != SRCH4_NO_QUEUE )
  {
    if ( srch4_tasks_left( SRCH4_1X,
                           srch4trans.queue.tech[ SRCH4_1X ].queue ) ==
         SRCH4DRV_RET_ACTIVE )
    {
      /* searcher queue is active */
      return( TRUE );
    }
  }

  /* return false here, either the technology is not currently assigned
     a queue or the assigned queue is not active*/
  return( FALSE );

} /* srch4_1x_active */


/*===========================================================================

FUNCTION       SRCH4_1X_START_SEARCH
DESCRIPTION    This function gets the queue and sends the command to start
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_1x_start_search
(
  void *(cb_data)                       /* callback data                   */
)
{
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                        /* return value from the drivers   */

  int16  freq_offset;                   /* srch4 freq offset with rot accum*/
  int16  vco_acc, rot_acc;              /* VCO and finger rotator accum    */

  /*-----------------------------------------------------------------------*/

  if ( ( srch4trans.queue.tech[ SRCH4_1X ].dump_cb_func == NULL ) ||
       ( ( srch4trans.mdsp.sleep_mask & (uint32)SRCH4_1X_MASK ) == 0 ) ||
       ( ( srch4trans.mdsp.req_mask   & (uint32)SRCH4_1X_MASK ) == 0 ) ||
       ( srch4trans.mdsp.app_registered == FALSE )
     )
  {
    SRCH4_DEBUG_MSG( "MDSP not enabled and/or no CBs assgned on 1x start",
                     0, 0, 0 );
  }

  /* read the rotator accumulator value */
  srch_afc_get_freq_error( &vco_acc, &rot_acc );

  /* apply to freq offset
     note: Sensitivity of the srch4 freq offset is 4.6875Hz/LSB but the
           finger rotator is 1.1719Hz/LSB * SRCH_AFC_ROT_FACTOR so divide
           the returned value from srch_afc_get_freq_error by 4, then
           multiply by the ROT_FACTOR to make them equal */
  freq_offset = srch4trans.queue.tech[ SRCH4_1X ].freq_offset -
                  ( ( rot_acc / 4 ) * SRCH_AFC_ROT_FACTOR );

  /* note: priority shifted by three bits to remove technology identifier */
  drv_ret_val = srch4_start_search(
            SRCH4_1X,
            srch4trans.queue.tech[ SRCH4_1X ].queue,
            freq_offset,
            ( ( uint16 ) srch4trans.queue.tech[ SRCH4_1X ].priority >> 3 ),
            srch4trans.queue.tech[ SRCH4_1X ].dump_cb_func,
            srch4trans.queue.tech[ SRCH4_1X ].lost_cb_func,
            cb_data );

  /* check the return value from the drivers */
  if ( drv_ret_val == SRCH4DRV_RET_OK )
  {
    /* reset the local counters upon starting a search only if the
       search was actually started! */
    srch4trans.queue.task_num[ srch4trans.queue.tech[ SRCH4_1X ].queue ] = 0;
  }
  else if ( drv_ret_val != SRCH4DRV_RET_ACTIVE )
  {
    /* call the callback to ensure that no technologies get "hung" while
       waiting for a callback that will never get called because the search
       was never started */

    /* call the callback on all cases except when a search was active
       this ensures that a tech doesn't get an "early" callback
       the drivers print out an error in the active case */
    if ( srch4trans.queue.tech[ SRCH4_1X ].dump_cb_func != NULL )
    {
      /* this call is not in interrupt space like a normal abort
         so call back user with intlocks */
      INTLOCK();
      (*srch4trans.queue.tech[ SRCH4_1X ].dump_cb_func)( cb_data );
      INTFREE();
    }
  }

  return( srch4trans_ret_trans( drv_ret_val ) );

}  /* srch4_1x_start_search */

/*===========================================================================

FUNCTION SRCH4_1X_WAIT_AND_DISCARD

DESCRIPTION
      This function is called to wait for the end of a search and
      then discard it. In case the search has already finished it
      will just clear the signal

DEPENDENCIES
      None

RETURN VALUE
      None

SIDE EFFECTS
      sig_mask sigs in srch_tcb are cleared

===========================================================================*/
void srch4_1x_wait_and_discard
(
  rex_sigs_type sig_mask  /* Which signal(s) to wait for */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( srch4_1x_active() )
  {
    (void) srch_wait( sig_mask );
  }

  /* clear the lost dump signal */;
  (void) rex_clr_sigs( &srch_tcb, ( sig_mask ) );
    /* there is a race condition existent where searcher 4 has already
       dumped before this function is called, thus we would have
       skipped clearing of the signals */

} /* srch4_1x_wait_and_discard */


/*===========================================================================

FUNCTION       SRCH4_1X_ABORT_SEARCH
DESCRIPTION    This function gets the queue and sends the command to abort
                the specified queue's searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_1x_abort_search
(
  void    (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void    *(cb_data),                 /* callback data                   */
  boolean   sync,                     /* synchronous abort flag          */
  boolean   part_res                  /* partial results flag            */
)
{
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                      /* return value from the drivers */

  /*-----------------------------------------------------------------------*/

  drv_ret_val = srch4_abort_queue( SRCH4_1X,
                                   srch4trans.queue.tech[ SRCH4_1X ].queue,
                                   abort_cb_func,
                                   cb_data,
                                   sync,
                                   part_res );

  /* No search active, no abort, just call the callback to signal that
     the abort is done */
  if ( ( drv_ret_val != SRCH4DRV_RET_OK ) &&
       ( abort_cb_func != NULL ) )
  {
    /* this call is not in interrupt space like a normal abort
       so call back user with intlocks */
    INTLOCK();
    abort_cb_func( 0, cb_data );
    INTFREE();
  }

  return( srch4trans_ret_trans( drv_ret_val ) );

}  /* srch4_1x_abort_search */


/*===========================================================================

FUNCTION       SRCH4_1X_READ_RESULT
DESCRIPTION    This function reads and translates the task result buffer
                into the search's respective cdma sector structure.
DEPENDENCIES   Note this function will be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   Updates the sector structure with the results.

===========================================================================*/
static void srch4_1x_read_result
(
  srch4_task_struct_type *task_res        /* array of task struct (results)*/
)
{
  srch_sect_parm_type    *parms;          /* sector pointer for results    */
  srch_sect_result_type  *results;        /* sector pointer for results    */

  /*-----------------------------------------------------------------------*/

  if ( task_res == NULL )
  {
    ERR("Trying to read invalid task", 0, 0, 0);
  }
  else if ( ( task_res->result_ptr == NULL ) ||
            ( task_res->parm_ptr == NULL ) )
  {
    ERR("Trying to read task to invalid 1x result/parm structure", 0, 0, 0);
  }
  else
  {
    /* depreciate void pointer back to sector results struct */
    parms = task_res->parm_ptr;
    results = task_res->result_ptr;

    #ifdef FEATURE_IS2000_REL_A_TD
    /* check if this is a transmit diversity pilot */
    if ( task_res->walsh != SRCH_COM_TD_WALSH )
    {
    #endif /* FEATURE_IS2000_REL_A_TD */
      /* store the calculated window position
         (this will be on a chip boundary) */
      results->win_pos = (int32) task_res->window_pos;

      /* store search results into the results */
      util_1x_copy_results_to_sect( task_res, results, parms );

    #ifdef FEATURE_IS2000_REL_A_TD
    }
    else /* transmit diversity pilot */
    {
      /* merge the TD results */
      util_1x_merge_td_results( task_res, results, parms );
    } /* transmit diversity pilot check */
    #endif /* FEATURE_IS2000_REL_A_TD */

    /* update the lecio fields */
    srch_util_calc_lecio( results, parms );

    /* add the peak data to the random buffer */
    srch_peak_fill_rand_buf( results, SRCH4_NUM_PEAKS );

  } /* sect_ptr NULL check */

} /* srch4_1x_read_result */


/*===========================================================================

FUNCTION       SRCH4_1X_ADD_TASK
DESCRIPTION    This function adds a 1x sector to a 1x srch4 task structure.
DEPENDENCIES   None

RETURN VALUE   Number of tasks that can still be programmed
               Negative value if an error occurred
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_1x_add_task
(
  srch_sect_parm_type    *parms,             /* search parameters           */
  srch_sect_result_type  *results,           /* search results              */
  boolean                 receive_diversity, /* flag to indicate use of RD  */
  boolean                 transmit_diversity /* flag to indicate use of TD  */
)
{
  srch4_task_struct_type *task;              /* ptr to HPQ/LPQ task struct  */
  srch4_queue_type        queue;             /* queue that 1x is assigned   */
  int32                   tasks_left;        /* num of spaces left in queue */
  int32                   drv_ret_val = SRCH4DRV_RET_FATAL;
                                             /* ret value from the drivers  */

  /*-----------------------------------------------------------------------*/

  #ifndef FEATURE_IS2000_REL_A_TD
  NOTUSED( transmit_diversity );
  #endif /* !FEATURE_IS2000_REL_A_TD */

  /* check for a valid sector */
  if ( ( parms == NULL ) || ( results == NULL ) )
  {
    ERR( "Addition of invalid 1x sector to task", 0, 0, 0 );
    return( SRCH4_RET_INVALID_PARAM );
  }

  #ifdef FEATURE_IS2000_REL_A_TD
  /* Clear transmit diversity in the case that this is not a TD pilot */
  if ( transmit_diversity &&
       ( ( parms->pilot_rec.type != CAI_PILOT_REC_1X_COM_TD ) &&
         ( parms->pilot_rec.type != CAI_PILOT_REC_1X_AUX_TD ) ) )
  {
    transmit_diversity = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_A_TD */

  queue = srch4trans.queue.tech[ SRCH4_1X ].queue;

  /* check if a queue has been assigned to this technology */
  if ( queue == SRCH4_NO_QUEUE )
  {
    ERR( "1x NOT registered", 0, 0, 0 );
    return( SRCH4_RET_NO_QUEUES );
  }

  /* Check destination queue to insure it's inactive */
  tasks_left = srch4_tasks_left( SRCH4_1X, queue );

  /* check to make sure we have enough tasks left */
  if ( tasks_left == SRCH4DRV_RET_ACTIVE )
  {
    /* searcher is active (or something is wrong) */
    MSG_HIGH( "Srch4 active, cannot program searches", 0, 0, 0 );
    return( SRCH4_RET_ACTIVE );
  }
  if ( tasks_left == SRCH4DRV_RET_UNCLEAN )
  {
    /* searcher is active and we have a lost/abort flag still
       set. this probably means we lost a dump and it wasn't
       cleaned up properly */
    MSG_HIGH( "Srch4 was lost/aborted w/o cleanup (%s)",
              srch4_queue_str[ queue ], 0, 0 );

    /* attempt to clean up */
    (void) srch4_1x_abort_search( NULL, NULL, FALSE, FALSE );

    /* task doesn't really need to know, just tell it the search
       is active */
    return( SRCH4_RET_ACTIVE );
  }
  else if ( tasks_left == SRCH4DRV_RET_INVALID_QUEUE )
  {
    /* A queue is not assigned (this is a second catch) */
    MSG_HIGH( "1x not assigned queue, can't program", 0, 0, 0 );
    return( SRCH4_RET_INVALID_QUEUE );
  }
  else if ( tasks_left == 0 )
  {
    MSG_HIGH( "Srch4-Too many 1x tasks(no spaces left)", 0, 0, 0 );
    return( SRCH4_RET_FULL );
  }
  else if ( tasks_left == SRCH4DRV_RET_TECH_MISMATCH )
  {
    /* This queue is in use by another technology */
    MSG_HIGH("1x's req queue is in use by another tech", 0, 0, 0);
    return( SRCH4_RET_TECH_MISMATCH );
  }
  #ifdef FEATURE_IS2000_REL_A_TD
  else if ( ( tasks_left == 1 ) &&
            transmit_diversity )
    /* TD requires two searches, make sure we're doing a TD search,
       and the pilot is a diversity pilot */
  {
    MSG_HIGH( "Srch4-Too many 1x TD tasks(1 space left)",
               tasks_left, 0, 0 );
    return( SRCH4_RET_FULL );
  }
  #endif /* FEATURE_IS2000_REL_A_TD */
  else if (    ( tasks_left == SRCH4DRV_RET_HPQ0_FREE )
            #ifdef SRCH4_HAS_HPQ1
            || ( tasks_left == SRCH4DRV_RET_HPQ1_FREE )
            #endif
            #ifdef SRCH4_USE_LPQ
            || ( tasks_left == SRCH4DRV_RET_LPQ_FREE )
            #endif
          )
  {
    tasks_left = -1*tasks_left;
  }

  /* grab the corresponding task struct to store this into */
  task = &( srch4_get_tasks( queue ) )[ srch4trans.queue.task_num[ queue ] ];

  /* copy search parameters to the task struct */
  util_1x_copy_sect_to_task( parms,
                             results,
                             task,
                             0,
                             receive_diversity );

  /* add the task to the proper queue, increment index */
  drv_ret_val = srch4_add_task( SRCH4_1X, queue, task );

  /* check the return value from the drivers */
  if ( drv_ret_val == SRCH4DRV_RET_OK )
  {
    /* Increment task buffer indices */
    srch4trans.queue.task_num[ queue ]++;
  }
  else
  {
    /* some error has occurred */
    return( srch4trans_ret_trans( drv_ret_val ) );
  }

  #ifdef FEATURE_IS2000_REL_A_TD
  if ( transmit_diversity )
  {
    /* grab the corresponding task struct to store this into */
    task = &(srch4_get_tasks( queue ))[ srch4trans.queue.task_num[ queue ] ];

    util_1x_copy_sect_to_task( parms,
                               results,
                               task,
                               SRCH_COM_TD_WALSH,
                               receive_diversity );

    /* add the task to the proper queue, increment index */
    drv_ret_val = srch4_add_task( SRCH4_1X, queue, task );

    /* check the return value from the drivers */
    if ( drv_ret_val == SRCH4DRV_RET_OK )
    {
      /* Increment task buffer indices */
      srch4trans.queue.task_num[ queue ]++;

      /* decrement the number of empty tasks */
      tasks_left--;
    }
    else
    {
      /* some error has occurred */
      return( srch4trans_ret_trans( drv_ret_val ) );
    }
  }
  #endif /* FEATURE_IS2000_REL_A_TD */

  /* decrement the number of empty tasks */
  tasks_left--;

  return( tasks_left );

} /* srch4_1x_add_task */


/*===========================================================================

FUNCTION       SRCH4_1X_NUM_TASKS_ADDED
DESCRIPTION    This function returns the number of tasks that have been
               successfully added and are not contained in the srch4 queue.
DEPENDENCIES   None

RETURN VALUE   Number of tasks in srch4 queue.
SIDE EFFECTS   None

===========================================================================*/
uint32 srch4_1x_num_tasks_added(void)
{

  int32                   tasks;             /* num tasks in the queue     */

  /*-----------------------------------------------------------------------*/

  tasks = srch4_tasks_left( SRCH4_1X,
                            srch4trans.queue.tech[ SRCH4_1X ].queue );

  /* if no error occured reading tasks left from driver */
  if ( tasks >= 0 )
  {
    /* This constant is only valid for the HPQs */
    return ( SRCH4_MAX_NUM_TASKS - (uint32)tasks );
  }

  return( 0 );

} /* srch4_1x_num_tasks_added */


/*===========================================================================

FUNCTION       SRCH4_1X_NUM_TASKS_REQ
DESCRIPTION    This function returns the number of tasks required by a
               specified sector.
DEPENDENCIES   None

RETURN VALUE   Number of tasks required if sector is added to a srch4
               queue.

SIDE EFFECTS   None

===========================================================================*/
uint32 srch4_1x_num_tasks_req
(
  srch_sect_struct_type* sector,            /* sector in question          */
  boolean                transmit_diversity /* flag to indicate use of TD  */
)
{
  /*-----------------------------------------------------------------------*/

  if ( sector == NULL )
  {
    ERR( "srch4_1x_num_tasks_req got NULL sector ptr", 0, 0, 0 );

    return( 1 );
  }

  #ifndef FEATURE_IS2000_REL_A_TD
  NOTUSED( sector );
  NOTUSED( transmit_diversity );
  #else /* !FEATURE_IS2000_REL_A_TD */
  if ( transmit_diversity &&
       ( ( sector->search_parms.pilot_rec.type == CAI_PILOT_REC_1X_COM_TD) ||
         ( sector->search_parms.pilot_rec.type == CAI_PILOT_REC_1X_AUX_TD) ) )
  {
    return( 2 );
  }
  #endif /* FEATURE_IS2000_REL_A_TD */

  return( 1 );

} /* srch4_1x_num_tasks_req */


/*===========================================================================

FUNCTION       SRCH4_1X_GET_ASSIGNED_QUEUE
DESCRIPTION    This function returns the assigned queue for 1X.
DEPENDENCIES   None

RETURN VALUE   which queue is assigned.

SIDE EFFECTS   None

===========================================================================*/
srch4_queue_type srch4_1x_get_assigned_queue( void )
{
  /*-----------------------------------------------------------------------*/

  return(srch4trans.queue.tech[ SRCH4_1X ].queue);

} /* srch4_1x_get_assigned_queue */



#ifdef SRCH4_HDR_ENABLED
/*===========================================================================

           H D R    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_HDR_REGISTER
DESCRIPTION    This function registers hdr with searcher 4. Once registered
                hdr will "own" a queue, and it's priority will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_hdr_register
(
  srch4_priority_type priority          /* search priority                 */
)
{

  boolean  app_valid = FALSE;           /* mdsp app properly enabled?      */
  srch4_queue_type q;                   /* the queue that was assigned     */

  /*-----------------------------------------------------------------------*/

  /* enable searcher 4 in the MDSP */
  app_valid = srch4_enable( SRCH4_HDR_MASK, TRUE );

  /* register searcher 4's HDR priority */
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].priority = priority;

  /* set HDR's register callbacks to NULL */
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].reg_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].reg_cb_data = NULL;

  if( app_valid )
  {
    /* request a queue */
    q = srch4_request_queue( SRCH4_HDR_SHOULDERS );

    if ( q == SRCH4_NO_QUEUE )
    {
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech %s, none available",
        srch4_tech_str[SRCH4_HDR_SHOULDERS] );
      srch4trans_print_queue_status();

      return( SRCH4_RET_NO_QUEUES );

    }
    else
    {
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech %s, assigned %s, priority %s",
        srch4_tech_str[SRCH4_HDR_SHOULDERS],
        srch4_queue_str[q],
        srch4_prio_str[ (uint32)( priority ) >> 3 ] );
      srch4trans_print_queue_status();
    }

    return( SRCH4_RET_OK );
  }
  else
  {
    return( SRCH4_RET_APP_NOT_VALID );
  }

}  /* srch4_hdr_register */


/*===========================================================================

FUNCTION       SRCH4_HDR_DEREGISTER
DESCRIPTION    This function deregisters hdr from it's assigned queue
               This function can also be used to unregister a reg_cb_func on
                a failed registration and before the queue has been assigned
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_hdr_deregister ( void )
{
  /*-----------------------------------------------------------------------*/

  /* deregister searcher 4's 1X parameters */
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].priority     = SRCH4_PRIO_MIN;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].freq_offset  = 0;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].dump_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].lost_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].reg_cb_func  = NULL;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].reg_cb_data  = NULL;

  /* release the assigned queue */
  srch4_release_queue( SRCH4_HDR_SHOULDERS );

  /* disable searcher 4 in the MDSP */
  (void) srch4_enable( SRCH4_HDR_MASK, FALSE );

}  /* srch4_hdr_deregister */


/*===========================================================================

FUNCTION       SRCH4_HDR_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns True if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_hdr_active( void )
{
  /*-----------------------------------------------------------------------*/

  if ( srch4trans.queue.tech[SRCH4_HDR_SHOULDERS].queue != SRCH4_NO_QUEUE )
  {
    if ( srch4_tasks_left(
           SRCH4_HDR_SHOULDERS,
           srch4trans.queue.tech[SRCH4_HDR_SHOULDERS].queue ) ==
         SRCH4DRV_RET_ACTIVE )
    {
      /* searcher queue is active */
      return( TRUE );
    }
  }

  /* return false here, either the technology is not currently assigned
     a queue or the assigned queue is active*/
  return( FALSE );

} /* srch4_hdr_active */


/*===========================================================================

FUNCTION       SRCH4_HDR_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
                queue
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_hdr_start_search
(
  int16  freq_offset,                   /* freq offset ( 4.6875 Hz/LSB )   */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
  void *(cb_data)                       /* callback data                   */

#ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
#endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */
)
{
  uint16 priority; /* actual priority */
  int32  drv_ret_val = SRCH4DRV_RET_FATAL;
                                        /* return value from the drivers */

  /*-----------------------------------------------------------------------*/

  /* adjust priority to actual priority (remove technology identifier) */
  priority = (uint16)srch4trans.queue.tech[SRCH4_HDR_SHOULDERS].priority >> 3;

  /* note: priority shifted by three bits to remove technology identifier */
  drv_ret_val = srch4_start_search(
#ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
#else
            SRCH4_HDR_SHOULDERS,
#endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */
            srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].queue,
            freq_offset,
            (uint16)( priority ),
            dump_cb_func,
            lost_cb_func,
            cb_data );

  /* store the parameters for debugging's sake */
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].freq_offset  = freq_offset;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].dump_cb_func = dump_cb_func;
  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].lost_cb_func = lost_cb_func;

  if ( drv_ret_val == SRCH4DRV_RET_OK )
  {
    /* reset the local counters upon starting a search only if the
       search was actually started! */
    srch4trans.queue.task_num[
      srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].queue ] = 0;
  }
  else if ( drv_ret_val != SRCH4DRV_RET_ACTIVE )
  {
    /* call the callback to ensure that no technologies get "hung" while
       waiting for a callback that will never get called because the search
       was never started */

    /* call the callback on all cases except when a search was active
       this ensures that a tech doesn't get an "early" callback
       the drivers print out an error in the active case */
    if ( srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].dump_cb_func != NULL )
    {
      /* this call is not in interrupt space like a normal abort
         so call back user with intlocks */
      INTLOCK();
      (*srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].dump_cb_func)( cb_data );
      INTFREE();
    }
  }

  return( srch4trans_ret_trans( drv_ret_val ) );

}  /* srch4_hdr_start_search */


/*===========================================================================

FUNCTION       SRCH4_HDR_ABORT_SEARCH
DESCRIPTION    This function aborts a searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_hdr_abort_search
(
  void (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void    *(cb_data),                 /* callback data                   */
  boolean   sync,                     /* synchronous abort flag          */
  boolean   part_res                  /* partial results flag            */
)
{
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                      /* return value from the drivers */

  /*-----------------------------------------------------------------------*/

  drv_ret_val = srch4_abort_queue(
                  SRCH4_HDR_SHOULDERS,
                  srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].queue,
                  abort_cb_func,
                  cb_data,
                  sync,
                  part_res );

  /* No search active, no abort, just call the callback to signal that
     the abort is done */
  if ( ( drv_ret_val != SRCH4DRV_RET_OK ) &&
       ( abort_cb_func != NULL ) )
  {
    /* this call is not in interrupt space like a normal abort
       so call back user with intlocks */
    INTLOCK();
    abort_cb_func( 0, cb_data );
    INTFREE();
  }

  return( srch4trans_ret_trans( drv_ret_val ) );

}  /* srch4_hdr_abort_search */


/*===========================================================================

FUNCTION       SRCH4_HDR_READ_RESULT
DESCRIPTION    This function reads and translates the task result buffer
               into the search's respective HDR sector structure.
DEPENDENCIES   Note this function will be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   This updates the hdr sector structure with the results.

===========================================================================*/
static void srch4_hdr_read_result
(
  srch4_task_struct_type *task_res        /* array of task struct (results)*/
)
{
  uint32 i;

  /*-----------------------------------------------------------------------*/

  if ( task_res == NULL )
  {
    ERR( "srch4_hdr_read_result got NULL task_res ptr", 0, 0, 0 );

    return;
  }

  for (i=0; i<HDR_MAX_SRCH_PEAKS; i++)
  {
    ((hdrsrchdrv_srch_res_struct_type *)(task_res->result_ptr))->eng[i]
        = task_res->peak_eng_raw[i];

    /* Raw (uninterpolated) peak energies */
    ((hdrsrchdrv_srch_res_struct_type *)(task_res->result_ptr))->posx2[i]
        = (int16) task_res->peak_posx2_raw[i];

    /* Interpolated chipx8 positions and energies */
    ((hdrsrchdrv_srch_res_struct_type *)(task_res->result_ptr))->eng_int[i]
        = task_res->peak_eng[i];

    /* Truncate from Cx16 to Cx8 here; no +1 due to the possible bias */
    ((hdrsrchdrv_srch_res_struct_type *)(task_res->result_ptr))->posx8_int[i]
        = (int32)((task_res->peak_pos[i] - 0  ) / CHIPX16_PER_EIGHTH_CHIP);
  }

  return;

} /* srch4_hdr_read_result */


#ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
#endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */


/*===========================================================================

FUNCTION       SRCH4_HDR_ADD_TASK
DESCRIPTION    This function adds an HDR sector to a task structure.
DEPENDENCIES   None

RETURN VALUE   If successfully added returns number of tasks on queue, else
               returns an error code, which is a negative number..
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_hdr_add_task
(
  hdrsrchdrv_srch_list_struct_type  *slist

  #ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
  #endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */
)
{
  srch4_task_struct_type *task;               /* ptr to HPQ/LPQ task struct  */
  srch4_queue_type        queue;              /* queue that HDR is assigned  */
  int32                   tasks_left = 0;     /* num of spaces left in queue */
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                              /* ret value from the drivers  */

  /*-----------------------------------------------------------------------*/

  /* check for a valid sector */
  if ( slist == NULL )
  {
    ERR("Addition of invalid HDR sector to task", 0, 0, 0);
    return( SRCH4_RET_INVALID_PARAM );
  }

  /* check if a queue has been assigned to this technology */
  queue = srch4trans.queue.tech[ SRCH4_HDR_SHOULDERS ].queue;
  if ( queue == SRCH4_NO_QUEUE )
  {
    queue = srch4_request_queue( SRCH4_HDR_SHOULDERS );
    if ( queue == SRCH4_NO_QUEUE )
    {
      ERR( "No available queues (HDR)", 0, 0, 0 );
      return( SRCH4_RET_NO_QUEUES );
    }
  }

  /* get the number of tasks available to be programmed */
  #ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
  #else
  tasks_left = srch4_tasks_left( SRCH4_HDR_SHOULDERS, queue );
  #endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */

  /* check to make sure we have enough tasks left */
  if ( tasks_left == SRCH4DRV_RET_ACTIVE )
  {
    /* searcher is active */
    MSG_HIGH("Srch4 is active, cannot program searches", 0, 0, 0);
    return( SRCH4_RET_ACTIVE );
  }
  else if ( tasks_left == SRCH4DRV_RET_INVALID_QUEUE )
  {
    /* A queue is not assigned */
    MSG_HIGH("HDR not assigned queue, can't program", 0, 0, 0);
    return( SRCH4_RET_INVALID_QUEUE );
  }
  else if ( tasks_left == SRCH4DRV_RET_TECH_MISMATCH )
  {
    /* This queue is in use by another technology */
    MSG_HIGH("HDR's req queue is in use by another tech", 0, 0, 0);
    return( SRCH4_RET_TECH_MISMATCH );
  }
  else if ( tasks_left == SRCH4DRV_RET_FULL )
  {
    MSG_HIGH("Srch4 - Too many HDR tasks added(%d spaces left)",
               tasks_left, 0, 0);
    return( SRCH4_RET_FULL );
  }
  else if (    ( tasks_left == SRCH4DRV_RET_HPQ0_FREE )
            #ifdef SRCH4_HAS_HPQ1
            || ( tasks_left == SRCH4DRV_RET_HPQ1_FREE )
            #endif
            #ifdef SRCH4_USE_LPQ
            || ( tasks_left == SRCH4DRV_RET_LPQ_FREE )
            #endif
          )
  {
    tasks_left = -1 * tasks_left;
  }

  /* grab the corresponding task struct to store this into */
  task = &( srch4_get_tasks( queue ) )[ srch4trans.queue.task_num[ queue ] ];

  task->result_ptr       = slist->res_ptr;
  task->pn_pos           = ((slist->rtc_offsetx2) >> 1);
  task->pilot_inx        = slist->pn_offset;
  task->window_size      = slist->win_size;
  task->coherent_len     = slist->coh_int;
  task->coherent_trunc   = slist->coh_trunc;
  task->non_coherent_len = slist->noncoh_int;
  #ifdef T_MDM7800
  task->carrier          = slist->carrier;
  #endif /* T_MDM7800 */

  /* for future target support */
  task->diversity        = ( slist->dual_ant ? SRCH4_ANT_DUAL :
                             (slist->ant_sel? SRCH4_ANT_1 : SRCH4_ANT_0) );
  task->burst_len        = slist->burst_length;

  /* assign the processing call back function */
  task->process_results  = srch4_hdr_read_result;

  #ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
  #endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */

  /* add the task to the proper queue, increment index */
  #ifdef FEATURE_HDRSRCH_1X_PILOT_SEARCH
#error code not present
  #else
  drv_ret_val = srch4_add_task( SRCH4_HDR_SHOULDERS, queue, task );
  #endif /* FEATURE_HDRSRCH_1X_PILOT_SEARCH */

  /* check the return value from the drivers */
  if ( drv_ret_val == SRCH4DRV_RET_OK )
  {
    /* Increment task buffer indices */
    srch4trans.queue.task_num[ queue ]++;
  }
  else
  {
    /* some error has occurred */
    return( srch4trans_ret_trans( drv_ret_val ) );
  }

  /* decrement the number of empty tasks */
  tasks_left--;

  return( tasks_left );

} /* srch4_hdr_add_task */
#endif /* SRCH4_HDR_ENABLED */


#ifdef SRCH4_AFLT_ENABLED
/*===========================================================================

           A F L T    T R A N S L A T I O N    F U N C T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH4_AFLT_RESET
DESCRIPTION    This function resets pending tasks and callback data for AFLT
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_aflt_reset (void)
{
  /*-----------------------------------------------------------------------*/

  srch4trans.aflt.task_num     = 0;
  srch4trans.aflt.cb_data      = NULL;
}  /* srch4_aflt_reset */


/*===========================================================================

FUNCTION       SRCH4_AFLT_REGISTER
DESCRIPTION    This function registers aflt with searcher 4. Once registered
                aflt will "own" a queue, and it's callbacks will be set.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_aflt_register
(
  srch4_priority_type priority,         /* search priority                 */
  int16  freq_offset,                   /* freq offset ( 4.6875 Hz/LSB )   */
  void (*dump_cb_func)(void *),         /* pointer to a callback function  */
                                        /*  to be called upon status dump  */
  void (*lost_cb_func)(void *)          /* pointer to a callback function  */
                                        /*  to be called upon lost dump    */
)
{
  boolean  app_valid = FALSE;           /* mdsp app properly enabled?      */
  srch4_queue_type q;                   /* the queue that was assigned     */

  /*-----------------------------------------------------------------------*/

  /* enable searcher 4 in the MDSP */
  app_valid = srch4_enable( SRCH4_AFLT_MASK, TRUE );

  /* register searcher 4's AFLT parameters */
  srch4trans.queue.tech[ SRCH4_AFLT ].priority     = priority;
  srch4trans.queue.tech[ SRCH4_AFLT ].freq_offset  = freq_offset;
  srch4trans.queue.tech[ SRCH4_AFLT ].dump_cb_func = dump_cb_func;
  srch4trans.queue.tech[ SRCH4_AFLT ].lost_cb_func = lost_cb_func;

  /* set AFLT's register callbacks to NULL */
  /* further investigation is needed before this is implemented for AFLT */
  srch4trans.queue.tech[ SRCH4_AFLT ].reg_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_AFLT ].reg_cb_data = NULL;

  if( app_valid )
  {
    /* request a queue */
    q = srch4_request_queue( SRCH4_AFLT );

    if ( q == SRCH4_NO_QUEUE )
    {
      MSG_SPRINTF_1( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech %s, none available",
        srch4_tech_str[SRCH4_AFLT] );
      srch4trans_print_queue_status();

      return( SRCH4_RET_NO_QUEUES );

    }
    else
    {
      MSG_SPRINTF_3( MSG_SSID_1X_SRCH, MSG_DBG_SRCH4,
        "A Queue was requested by tech %s, assigned %s, priority %s",
        srch4_tech_str[SRCH4_AFLT],
        srch4_queue_str[q],
        srch4_prio_str[ (uint32)( priority ) >> 3 ] );
      srch4trans_print_queue_status();
    }

    return( SRCH4_RET_OK );
  }
  else
  {
    return( SRCH4_RET_APP_NOT_VALID );
  }

}  /* srch4_aflt_register */


/*===========================================================================

FUNCTION       SRCH4_AFLT_DEREGISTER
DESCRIPTION    This function deregisters AFLT from it's assigned queue
               This function can also be used to unregister a reg_cb_func on
                a failed registration and before the queue has been assigned
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4_aflt_deregister ( void )
{
  /*-----------------------------------------------------------------------*/

  /* Deregister searcher 4's AFLT parameters */
  srch4trans.queue.tech[ SRCH4_AFLT ].priority     = SRCH4_PRIO_MIN;
  srch4trans.queue.tech[ SRCH4_AFLT ].freq_offset  = 0;
  srch4trans.queue.tech[ SRCH4_AFLT ].dump_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_AFLT ].lost_cb_func = NULL;
  srch4trans.queue.tech[ SRCH4_AFLT ].reg_cb_func  = NULL;
  srch4trans.queue.tech[ SRCH4_AFLT ].reg_cb_data  = NULL;

  /* release the assigned queue */
  srch4_release_queue( SRCH4_AFLT );

  /* disable searcher 4 in the MDSP */
  (void) srch4_enable( SRCH4_AFLT_MASK, FALSE );

}  /* srch4_aflt_deregister */


/*===========================================================================

FUNCTION       SRCH4_AFLT_SEARCH_PENDING
DESCRIPTION    This function checks if there are any pending AFLT searches
DEPENDENCIES   None

RETURN VALUE   Returns True if AFLT searches are pending
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_aflt_search_pending( void )
{
  /*-----------------------------------------------------------------------*/

  return (srch4trans.aflt.task_num != 0);

} /* srch4_aflt_search_pending */


/*===========================================================================

FUNCTION       SRCH4_AFLT_ACTIVE
DESCRIPTION    This function checks the status of the assigned queue
DEPENDENCIES   None

RETURN VALUE   Returns True if assigned queue is active
SIDE EFFECTS   None

===========================================================================*/
boolean srch4_aflt_active( void )
{
  /*-----------------------------------------------------------------------*/

  if ( srch4trans.queue.tech[ SRCH4_AFLT ].queue != SRCH4_NO_QUEUE )
  {
    if ( srch4_tasks_left( SRCH4_AFLT,
                           srch4trans.queue.tech[ SRCH4_AFLT ].queue ) ==
         SRCH4DRV_RET_ACTIVE )
    {
      /* searcher queue is active */
      return( TRUE );
    }
  }

  /* return false here, either the technology is not currently assigned
     a queue or the assigned queue is not active*/
  return( FALSE );

} /* srch4_aflt_active */


/*===========================================================================

FUNCTION       SRCH4_AFLT_START_SEARCH
DESCRIPTION    This function commands searcher4 to sweep using the assigned
                queue
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_aflt_start_search
(
  void *(cb_data)                       /* callback data                   */
)
{
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                      /* return value from the drivers */

  /*-----------------------------------------------------------------------*/

  /* save the dump/lost function if you have to continue the search */
  if (srch4trans.aflt.task_num > 0)
  {
    srch4trans.aflt.cb_data = cb_data;
  }

  if (srch4trans.mdsp.app_valid)
  {
    /* note: priority shifted by three bits to remove technology identifier */
    drv_ret_val = srch4_start_search(
                  SRCH4_AFLT,
                  srch4trans.queue.tech[ SRCH4_AFLT ].queue,
                  srch4trans.queue.tech[ SRCH4_AFLT ].freq_offset,
                  (uint16)srch4trans.queue.tech[ SRCH4_AFLT ].priority >> 3,
                  srch4trans.queue.tech[ SRCH4_AFLT ].dump_cb_func,
                  srch4trans.queue.tech[ SRCH4_AFLT ].lost_cb_func,
                  cb_data );

    /* check the return value from the drivers */
    if ( drv_ret_val == SRCH4DRV_RET_OK )
    {
      /* reset the local counters upon starting a search only if the
         search was actually started! */
      srch4trans.queue.task_num[ srch4trans.queue.tech[ SRCH4_AFLT ].queue ] = 0;
    }
    else if ( drv_ret_val != SRCH4DRV_RET_ACTIVE )
    {
      /* call the callback to ensure that no technologies get "hung" while
         waiting for a callback that will never get called because the search
         was never started */

      /* call the callback on all cases except when a search was active
         this ensures that a tech doesn't get an "early" callback
         the drivers print out an error in the active case */
      if ( srch4trans.queue.tech[ SRCH4_AFLT ].dump_cb_func != NULL )
      {
        /* this call is not in interrupt space like a normal abort
           so call back user with intlocks */
        INTLOCK();
        (*srch4trans.queue.tech[ SRCH4_AFLT ].dump_cb_func)( cb_data );
        INTFREE();
      }
    }
  }
  else
  {
    return SRCH4_RET_APP_NOT_VALID;
  }

  return( srch4trans_ret_trans( drv_ret_val ) );

}  /* srch4_aflt_start_search */


/*===========================================================================

FUNCTION       SRCH4_AFLT_ABORT_SEARCH
DESCRIPTION    This function aborts a searcher4 sweep.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_aflt_abort_search
(
  void (*abort_cb_func)( uint32, void * ),
                                      /* pointer to a callback function  */
                                      /*  to be called upon abort dump   */
  void *(cb_data)                     /* callback data                   */
)
{
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                      /* return value from the drivers */

  /*-----------------------------------------------------------------------*/

  drv_ret_val = srch4_abort_queue( SRCH4_AFLT,
                                   srch4trans.queue.tech[ SRCH4_AFLT ].queue,
                                   abort_cb_func,
                                   cb_data,
                                   FALSE,
                                   FALSE );
  srch4_aflt_reset();

  /* No search active, no abort, just call the callback to signal that
     the abort is done */
  if ( ( drv_ret_val != SRCH4DRV_RET_OK ) &&
       ( abort_cb_func != NULL ) )
  {
    /* this call is not in interrupt space like a normal abort
       so call back user with intlocks */
    INTLOCK();
    abort_cb_func( 0, cb_data );
    INTFREE();
  }

  return( srch4trans_ret_trans( drv_ret_val ) );

}  /* srch4_aflt_abort_search */


/*===========================================================================

FUNCTION       SRCH4_AFLT_READ_RESULT
DESCRIPTION    This function reads and translates the task result buffer
               into the search's respective AFLT sector structure.
DEPENDENCIES   Note this function will be called in ISR context

RETURN VALUE   None
SIDE EFFECTS   This updates the gps sector structure with the results.

===========================================================================*/
static void srch4_aflt_read_result
(
  srch4_task_struct_type *task_res        /* array of task struct (results)*/
)
{
  srch_ppm_pilot_struct_type *sect;         /* sector pointer for results  */
  uint32                      peak_inx = 0; /* loop index                  */

  /*-----------------------------------------------------------------------*/

  if ( task_res == NULL )
  {
    ERR( "Trying to read invalid task", 0, 0, 0 );
  }
  else if ( task_res->result_ptr == NULL )
  {
    ERR( "Trying to read task to invalid AFLT sector", 0, 0, 0 );
  }
  else
  {
    /* depreciate void pointer back to sector struct */
    sect = task_res->result_ptr;

    /* store the number of peaks */
    sect->num_peaks = SRCH4_NUM_PEAKS;

    /* loop over all peaks */
    for( peak_inx = 0; peak_inx < SRCH4_NUM_PEAKS; peak_inx++ )
    {
      /* Take care of non returned peaks */
      if ( ( task_res->peak_eng[peak_inx] == 0 ) &&
           ( peak_inx > 2 ) )
      {
        /* no peak was returned, so steal the pos and inx from the
           previous peak. There will always be at least 3 peaks, and
           this will only happen for window sizes less than 12 chips */
        sect->max_pos[peak_inx] = sect->max_pos[peak_inx-1];

        /* set peak energy to 0 */
        sect->max_eng[peak_inx] = 0;
      }
      else
      {
        /* calculate and store peak inx position */
        sect->max_pos[peak_inx] = task_res->peak_pos[peak_inx];

        /* store peak energy */
        sect->max_eng[peak_inx] = task_res->peak_eng[peak_inx];
      }
      /* Antenna on which PPM searching happened */
      sect->antenna = (uint8)task_res->diversity;
    } /* for peak_inx */
  } /* sect_ptr NULL check */

} /* srch4_aflt_read_result */


/*===========================================================================

FUNCTION       SRCH4_AFLT_CONTINUE_SEARCH
DESCRIPTION    This function programs the searches on the pending queue and
               starts the searcher again.
DEPENDENCIES   None

RETURN VALUE   srch4trans error return value
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_aflt_continue_search( void )
{
  int32                   tasks_left = 0; /* num of spaces left in queue    */
  srch4_task_struct_type *task  = NULL;   /* ptr to HPQ/LPQ task struct     */
  srch4_queue_type        queue;          /* queue that AFLT is assigned*/
  int32                   tasks_added = 0;/* tasks re-programmed            */
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                          /* return value from the drivers  */

  /*-----------------------------------------------------------------------*/

  /* store assigned queue for brevity */
  queue = srch4trans.queue.tech[ SRCH4_AFLT ].queue;

  /* check if a queue has been assigned to this technology */
  if ( queue == SRCH4_NO_QUEUE )
  {
    ERR( "AFLT NOT registered", 0, 0, 0 );
    return( SRCH4_RET_NO_QUEUES );
  }

  /* Program the remaining searches on the aflt task queue */
  while ( srch4trans.aflt.task_num > 0 )
  {
    /* Check destination queue to insure it's inactive */
    tasks_left = srch4_tasks_left( SRCH4_AFLT, queue );

    /* check to make sure we have enough tasks left */
    if ( tasks_left == SRCH4DRV_RET_ACTIVE )
    {
      /* searcher is active */
      MSG_HIGH( "Srch4 is active, cannot program searches", 0, 0, 0 );
      return SRCH4_RET_ACTIVE;
    }
    else if ( tasks_left == SRCH4DRV_RET_TECH_MISMATCH )
    {
      /* This queue is in use by another technology */
      MSG_HIGH("AFLT's req queue is in use by another tech", 0, 0, 0);
      return( SRCH4_RET_TECH_MISMATCH );
    }
    else if (    ( tasks_left == SRCH4DRV_RET_HPQ0_FREE )
              #ifdef SRCH4_HAS_HPQ1
              || ( tasks_left == SRCH4DRV_RET_HPQ1_FREE )
              #endif
              #ifdef SRCH4_USE_LPQ
              || ( tasks_left == SRCH4DRV_RET_LPQ_FREE )
              #endif
            )
    {
      tasks_left = -1*tasks_left;
    }
    else if (tasks_left == 0)
    {
      break;
    }

    if (tasks_left > 0)
    {
      /* grab the corresponding task struct to store this into */
      task = &( srch4_get_tasks( queue ) )[ srch4trans.queue.task_num[queue] ];

      memcpy( task,
              &( srch4trans.aflt.tasks_pending[ srch4trans.aflt.task_num-1 ] ),
              sizeof( srch4_task_struct_type ) );

      /* add the task to the proper queue, increment index */
      drv_ret_val = srch4_add_task( SRCH4_AFLT, queue, task );

      /* check the return value from the drivers */
      if ( drv_ret_val == SRCH4DRV_RET_OK )
      {
        /* Increment task buffer indices */
        srch4trans.queue.task_num[ queue ]++;
      }
      else
      {
        /* some error has occurred */
        return( srch4trans_ret_trans( drv_ret_val ) );
      }

      /* decrement the number of empty tasks */
      tasks_left--;
      srch4trans.aflt.task_num--;
      tasks_added++;
    }
  }

  if ( tasks_added > 0 )
  {
    /* start the search */
    return( srch4_aflt_start_search( srch4trans.aflt.cb_data ) );
  }
  else
  {
    return( srch4trans_ret_trans( drv_ret_val ) );
  }

} /* srch4_aflt_continue search */


/*===========================================================================

FUNCTION       SRCH4_AFLT_ADD_TASK
DESCRIPTION    This function adds an AFLT sector to a task structure.
DEPENDENCIES   None

RETURN VALUE   If successfully added returns number of tasks on queue, else
               returns an error code, which is a negative number..
SIDE EFFECTS   None

===========================================================================*/
int32 srch4_aflt_add_task
(
  q_type  *aflt_q_ptr,                       /* queue of aflt sectors to add*/
  q_type  *free_q_ptr,                       /* queue of free aflt sectors  */
  int32   ppm_win_center                     /* Win center reference for PPM*/
)
{
  srch4_task_struct_type *task       = NULL;  /* ptr to HPQ/LPQ task struct */
  srch4_queue_type        queue;              /* queue that AFLT is assigned*/
  int32                   tasks_left = 0;     /* num of spaces left in queue*/
  int32                   pn_pos     = 0;     /* for pn_pos calcs           */
  uint32                  pn_pos_n   = 0;     /* for pn_pos calcs (norm)    */
  uint32                  peak_inx   = 0;     /* loop index                 */

  srch1_cmd_struct_type   *aflt;              /* aflt search to add         */
  int32 drv_ret_val = SRCH4DRV_RET_FATAL;
                                              /* ret value from the drivers */

  /*------------------------------------------------------------------------*/

  /* store assigned queue for brevity */
  queue = srch4trans.queue.tech[ SRCH4_AFLT ].queue;

  /* check for a valid parameter */
  if ( aflt_q_ptr == NULL)
  {
    ERR( "Addition of invalid AFLT struct to task struct", 0, 0, 0 );
    return( SRCH4_RET_INVALID_PARAM );
  }

  /* check if a queue has been assigned to this technology */
  if ( queue == SRCH4_NO_QUEUE )
  {
    ERR( "AFLT NOT registered", 0, 0, 0 );
    return( SRCH4_RET_NO_QUEUES );
  }

  /* Check destination queue to insure it's inactive */
  tasks_left = srch4_tasks_left( SRCH4_AFLT, queue );

  /* check to make sure we have enough tasks left */
  if ( tasks_left == SRCH4DRV_RET_ACTIVE )
  {
    /* searcher is active */
    MSG_HIGH( "Srch4 is active, cannot program searches", 0, 0, 0 );
    return( SRCH4_RET_ACTIVE );
  }
  else if ( tasks_left == SRCH4DRV_RET_INVALID_QUEUE )
  {
    /* A queue is not assigned */
    MSG_HIGH( "AFLT not assigned queue, can't program", 0, 0, 0 );
    return( SRCH4_RET_INVALID_QUEUE );
  }
  else if ( tasks_left == SRCH4DRV_RET_TECH_MISMATCH )
  {
    /* This queue is in use by another technology */
    MSG_HIGH("AFLT's req queue is in use by another tech", 0, 0, 0);
    return( SRCH4_RET_TECH_MISMATCH );
  }
  else if (    ( tasks_left == SRCH4DRV_RET_HPQ0_FREE )
            #ifdef SRCH4_HAS_HPQ1
            || ( tasks_left == SRCH4DRV_RET_HPQ1_FREE )
            #endif
            #ifdef SRCH4_USE_LPQ
            || ( tasks_left == SRCH4DRV_RET_LPQ_FREE )
            #endif
          )
  {
    tasks_left = 8; /* -1 * tasks_left; */
  }
  else if ( tasks_left < 0 )
  {
    return( srch4trans_ret_trans( tasks_left ) );
  }

  while ( (aflt = (srch1_cmd_struct_type *)
           q_get ( aflt_q_ptr )) != NULL )
  {
    if (tasks_left > 0)
    {
      /* grab the corresponding task struct to store this into */
      task = &(srch4_get_tasks( queue ) )[ srch4trans.queue.task_num[ queue ] ];
    }
    else if (tasks_left == 0)
    {
      ASSERT( srch4trans.aflt.task_num < SRCH4_MAX_AFLT_TASKS );
      task = &srch4trans.aflt.tasks_pending[ srch4trans.aflt.task_num ];
      MSG_LOW("Srch4 - Adding to pending queue(%d tasks)",
                 srch4trans.aflt.task_num, 0, 0);
    }
    else
    {
      return( srch4trans_ret_trans( tasks_left ) );
    }

    /* store the sector pointer */
    task->result_ptr = aflt->ppm_ptr;

    /* loop over all peaks */
    for( peak_inx = 0; peak_inx < SRCH_MAX_PEAKS; peak_inx++ )
    {
      aflt->ppm_ptr->max_pos[peak_inx] = (uint32)~0;
      aflt->ppm_ptr->max_eng[peak_inx] = 0;
    }

    pn_pos    = ppm_win_center - ( aflt->winsiz / 2 );

    /* apply the window offset */
    pn_pos   += aflt->offset;

    /* get this to a range from 0 ... 262143 */
    pn_pos_n  = pn_pos & PN_CIRCLE_MASK;
    /* change to Chips */
    pn_pos_n /= CHIPX8_PER_CHIP;

    task->pn_pos           = (uint16) pn_pos_n;

    /* triage wants the window position at the end of the search.
       Although there is no real window position in searcher4 we
       will make it up */
    task->window_pos       = ( pn_pos_n * CHIPX8_PER_CHIP ) +
                             (uint32) aflt->winsiz;

    /* ensure the window position is from 0 ... 262143 */
    task->window_pos      &= PN_CIRCLE_MASK;

    /* Convert the sector structure into a task structure */
    task->pilot_inx        = (uint16) aflt->pilot;
    task->window_size      = (uint16) ( aflt->winsiz / CHIPX8_PER_CHIP );
    task->phase            = 0;
    
    #ifdef T_MDM7800
    task->carrier          = SRCH4_CARRIER_0;
    #endif /* T_MDM7800 */
    
    task->coherent_trunc   = aflt->coh_tr;
    task->non_coherent_len = aflt->nc_int + 1;
    task->qof              = 0;
    task->walsh            = 0;

    /* AFLT data is in 16 chip units */
    task->coherent_len     = aflt->chps * 16;

    task->diversity        = SRCH4_ANT_0;
    #ifdef FEATURE_CDMA_RX_DIVERSITY
    /* for diversity support */
    switch ( srch_rx_get_device( DEMOD_CHAIN ) )
    {
      case RFCOM_RECEIVER_1:
      {
        task->diversity    = SRCH4_ANT_1;
        break;
      }
      case RFCOM_RECEIVER_DIV:
      case RFCOM_TRANSCEIVER_0:
      default:
      {
        task->diversity    = SRCH4_ANT_0;
        break;
      }
    }/* switch */
    #else /* FEATURE_CDMA_RX_DIVERSITY */
    task->diversity        = SRCH4_ANT_0;
    #endif /* FEATURE_CDMA_RX_DIVERSITY */

    task->burst_len        = 0;

    /* assign the processing call back function */
    task->process_results  = srch4_aflt_read_result;

    if (tasks_left > 0)
    {
      /* add the task to the proper queue, increment index */
      drv_ret_val = srch4_add_task( SRCH4_AFLT, queue, task );

      /* check the return value from the drivers */
      if ( drv_ret_val == SRCH4DRV_RET_OK )
      {
        /* Increment task buffer indices */
        srch4trans.queue.task_num[ queue ]++;
      }
      else
      {
        /* some error has occurred */
        q_put ( free_q_ptr, &aflt->link );
        return( srch4trans_ret_trans( drv_ret_val ) );
      }

      /* decrement the number of empty tasks */
      tasks_left--;
    }
    else /* tasks_left == 0 */
    {
      srch4trans.aflt.task_num++;
    } /* tasks_left > 0  */

    q_put ( free_q_ptr, &aflt->link );
  }

  return( tasks_left );

} /* srch4_aflt_add_task */

#endif /* SRCH4_AFLT_ENABLED */

/*===========================================================================

           S R C H 4 T R A N S    I N I T    F U N C T I O N

===========================================================================*/
/*===========================================================================

FUNCTION       SRCH4TRANS_INIT
DESCRIPTION    This function initializes the srch4 translation layer.
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
void srch4trans_init(void)
{
  srch4_queue_type q;                        /* loop counter through queues */
  srch4_srch_type  t;                        /* loop counter through queues */
  uint32           j;                        /* counter var                 */

  /*-----------------------------------------------------------------------*/

  srch4_init();                              /* init searcher 4           */

  /* clear all the queue task cnts for HPQ0, HPQ1, LPQ */
  /* init the queues to be not assigned to any technologies */
  for ( q = SRCH4_HPQ0; q < SRCH4_MAX_Q; q++ )
  {
    srch4trans.queue.task_num[ q ]   = 0;
    srch4trans.queue.queue_assn[ q ] = SRCH4_NO_TECH;
  }

  /* init tech vars for technologies (1X, HDR, AFLT) */
  for ( t = SRCH4_1X; t < SRCH4_NUM_TECH; t++ )
  {
    srch4trans.queue.tech[ t ].queue        = SRCH4_NO_QUEUE;
    srch4trans.queue.tech[ t ].priority     = SRCH4_PRIO_MIN;
    srch4trans.queue.tech[ t ].freq_offset  = 0;
    srch4trans.queue.tech[ t ].dump_cb_func = NULL;
    srch4trans.queue.tech[ t ].lost_cb_func = NULL;
    srch4trans.queue.tech[ t ].reg_cb_func  = NULL;
    srch4trans.queue.tech[ t ].reg_cb_data  = NULL;
  }

  #ifdef SRCH4_AFLT_ENABLED
  /* initialize AFLT variables */
  srch4_aflt_reset();
  #endif

  /* initialize MDSP callback pool */

  /* set the MDSP app cb head and tail */
  srch4trans.mdsp.app_cb_head =
                    &srch4_mdsp_app_cb_pool[ 0 ];
  srch4trans.mdsp.app_cb_tail =
                    &srch4_mdsp_app_cb_pool[ SRCH4_MDSP_APP_CB_POOL_SIZE - 1 ];

  /* initialize entries in the MDSP callback pool */
  for ( j = 0; j < ( SRCH4_MDSP_APP_CB_POOL_SIZE - 1 ); j++ )
  {
    srch4_mdsp_app_cb_pool[ j ].cb_func = NULL;
    srch4_mdsp_app_cb_pool[ j ].next    = &srch4_mdsp_app_cb_pool[ j + 1 ];
  }

  srch4_mdsp_app_cb_pool[ SRCH4_MDSP_APP_CB_POOL_SIZE - 1 ].next = NULL;

} /* srch4trans_init */

