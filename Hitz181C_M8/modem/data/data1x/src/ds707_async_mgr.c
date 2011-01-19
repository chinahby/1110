/*===========================================================================

                        D S 7 0 7 _ A S Y N C _ M G R
DESCRIPTION
  This file contains the main functionality for IS707 async/fax mode-specific
  handlers. It mainly deals with call control.

EXTERNALIZED FUNCTIONS
  ds707_async_mgr_powerup_init()
    Called at powerup to initialize data structures, register with 3gdsmgr.

  ds707_async_get_active_iface_ptr()
    Function that returns pointer to 707 async iface if it's active.

  ds707_async_ps_tcp_enqueue()
    Function to queue data to be sent over async/fax air interface.

  ds707_async_process_cmd()
    Function that processes commands to Async/Fax controller.

  ds707_async_process_signals()
    Function that processes signals to Async/Fax controller.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_async_mgr_powerup_init() must be called at startup.

 Copyright (c) 2002-2008 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_mgr.c_v   1.39   18 Feb 2003 10:11:40   sramacha  $
    $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_async_mgr.c#4 $ $DateTime: 2009/01/29 05:35:22 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/17/08    ms     Compiler warning fix
02/07/08    sn     Removed FEATURE_SPECIAL_MDR.
10/29/07    sn     Replaced banned string APIs.
05/23/06    squ    Featurized async data
12/22/04    rsl    ATA doesn't answer call in JCDMA mode, fix in
                   bring_up_cmd_hdlr.
09/07/04    atp    In error case of CM return failure to call orig/answer,
                   deregister at rsp func before sending NO CARRIER (CR48892).
09/02/04    atp    Fixed JCDMA ATA dial string (CR48729).
02/10/04    ak     Fixed JCDMA 2-stage dial processing.  Specs for processing
                   are in function header.
01/21/04    sv     Replaced all references to DEF_MSS with PS707_ASYNC_DEF_MSS.
01/12/04    ak     Fixed issue where didn't copy answer string for JCDMA for
                   incoming data calls.
12/29/03    atp    Replaced word with dword in ds707_async_ps_tcp_enqueue().
10/28/03    ak     For JCDMA, see if async/fax calls are allowed.
06/12/03    rtp    Added back clk frequency changing during data calls.
05/12/03    atp    Removed change sio mode to autodt when registering handlers
                   with siolib fails.
05/02/03    usb    ps_iface_create() prototype change.
04/24/03    atp    Removed clk frequency changing during data calls.
                   Functionality moved to drivers.
04/18/03    ak     Updated switch_call_handler to take extra cmd_ptr param.
03/06/03    atp    Added call to register and deregister for clock frequency
                   changing when a data call is started and ended
02/18/03    rsl    Changes relating to cm end_params.
02/14/03    atp    Replaced use of FLOW_ALL for sio flow control.
02/13/03    jd     Removed PPP logging (now done in ps_pppi.c)
02/10/03    ak     Fixes for JCDMA - call ds707_rmsm_dial_str_cb with right
                   number params.  Remove unused local var from jcdma func.
01/23/03    atp    Added handlers for returning RLP stats, connection info.
01/20/03    ar     Added dsat_dial_modifier_info_type to
                   ds707_async_dial_str_cb interface.
01/21/03    ak     Updates for JCDMA 2-STAGE dial string processing.
01/09/03    atp    Replaced dsm_free_buffer() with dsm_free_packet().
12/12/02    ak     When answering an incoming call, set info_type to CDMA
12/10/02    ak     In 2-stage dialing, use MODE_CDMA instead of MODE_JCDMA.
12/02/02    atp    Added header, comments, etc.
12/02/02    atp    Fixed compiler warnings.
11/21/02    rsl    JCDMA 2-stage dialing related changes.
11/19/02    atp    PPP logging compilation fixes.
11/19/02    atp    For incoming call, when NW releases before answer, stop
                   ringing has to be done.
11/19/02    atp    Re-enable pkt iface in case of failure conditions before
                   async protocol establishment.
11/13/02    usb    Replaced DS_IS_FLOW_ENABLED with DS_FLOW_ALL_MASK
10/30/02    rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X.
10/16/02    ak     Updated to use changed func names in jcdma_m51.c
10/15/02    ak     Updated for 3G DSMGR handler table.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/13/02    ak     Updates for JCDMA - call pkt string based on M511 value &
                   force NO_CARRIER when call ends.
09/26/02    atp    Added enabling/disabling of 1x pkt iface.
09/24/02    usb    Added user_data to ppp tx and rx function callbacks
09/17/02    atp    Siolib registration moved earlier to dial_str_cb and
                   incoming_call_handlers.
09/11/02    ak     Compile fix.
09/10/02    atp    New register handler interface in atcop, dial str
                   registration removed from here. Sync up to changes in
                   signature of dial_str_cb(), call_answer_cb().
09/10/02    atp    New cb in ds3gsiolib for dtr with amp_d=0. We set it NULL.
08/29/02    atp    Resetting ds707_async_state.tc_state.call_id moved to
                   call_ended_hdlr() from bring_up_cmd_hdlr().
08/29/02    atp    Invalid cstate return possible from ds3g_get_call_state().
08/23/02    ak     Minor bug fixes for first MM_DATA compile.
08/22/02    atp    Implemented switch_calls_hdlr(), force_call_down_hdlr().
08/22/02    atp    Cleanup for shipping.
08/20/02    atp    Additional handlers (force_down, etc) to sync with ds3gmgr.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
08/15/02    atp    Added support for mode-specific SIO wmks.
08/14/02    atp    For incoming calls, alert user moved here from 3gdsmgr.
08/14/02    atp    When DTR changes, logic for checking at&d value
                   shifted to siolib.
08/14/02    atp    Added ATZ handler.
08/13/02    mvl    updated ps_iface state macros for name change
08/12/02    atp    Fixed mem leak in fax calls. Atcop expects input ptr
                   be freed in ds707_async_at_rsp_fmting_func_hdlr().
08/02/02    atp    No need to assert cd when traffic channel comes up.
07/31/02    usb    Removed include ps.h
07/30/02    atp    Changed dsat_fclass_val to dsat707_fclass_val to sync
                   with same change in atcop.
07/30/02    atp    Fixed call_ended_hdlr() in setting iface_ptr.
07/29/02    atp    Iface down ind after sending no carrier in tear_down.
07/28/02    atp    Added code for originating fax calls.
07/28/02    atp    Register unrecognized AT cmd handler at powerup,
                   unregister ATA handler at teardown.
07/24/02    atp    Added support for maintaining iface phys_link states.
07/24/02    atp    Moved alert_user to ds3gmgr.
07/21/02    atp    Added support for incoming calls.
07/21/02    atp    Dsm kind field in at_rsp_routing_func_hdlr().
07/16/02    ak     Updated for incoming_call_hdlr taking cmd_ptr.
07/16/02    atp    Added ds707_async_get_active_iface_ptr().
07/16/02    atp    Changed cm end call params to new info type.
07/16/02    aku    changes to support ds_flow_control.h change.
07/15/02    atp    Added more informative messages.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "AEEstd.h"
#include "cm.h"
#ifdef FEATURE_DIAG_DS_SIO_COUPLING
  #include "diag.h"
#endif /* FEATURE_DIAG_DS_SIO_COUPLING */
#include "ds_flow_control.h"
#include "ds3gmgr.h"
#include "ds3gmgrint.h"
#include "ds3gmshif.h"
#include "ds3gsiolib.h"
#include "ds3gtimer.h"
#include "ds707.h"
#include "ds707_async_617.h"
#include "ds707_async_defs.h"
#include "ds707_async_mgr.h"
#include "ds707_async_wmk.h"
#include "ds707_async_timer.h"
#include "ds707_pkt_mgr.h"
#include "ds707_rmsm.h"
#include "ds707_so_async.h"
#include "ds707_so_g3fax.h"
#include "ds707_wmk.h"
#include "dsat.h"
#include "dsati.h"
#include "dsat707extctab.h"
#include "dsat707util.h"
#include "dsat707vendctab.h"
#include "dsrlp.h"
#include "dstask.h"
#include "err.h"
#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif /* FEATURE_IS95B_MDR */
#include "msg.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "ps_tcp.h"
#include "ps707_async.h"
#include "queue.h"
#include "rex.h"
#include "sys.h"

#ifdef FEATURE_JCDMA_1X
#include "ds707_jcdma_m51.h"
  #include "ds707_so_jcdma.h"
#endif
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
#include "clkregim.h"
#endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */


/*===========================================================================
                             EXTERNS
===========================================================================*/
extern dsat_call_abort_cb_type ds707_async_call_answer_cb(void);


/*===========================================================================
                            TYPEDEFS
===========================================================================*/
typedef void (*void_func_ptr_type)(void);

/*---------------------------------------------------------------------------
  The time interval allowed for opening the TCP connection. The interval
  starts with the reception of CALL_CONNECTED.
  Note, the timeout interval was increased to 30 secs from 10 secs because
  the RLP establish timeout is 15secs and a few infrastructure vendors
  were having problems in their testing with a lower timeout.
---------------------------------------------------------------------------*/
#define DS707_ASYNC_PTCL_OPENING_TIMER_VAL 30000

/*---------------------------------------------------------------------------
  JCDMA 2-Stage dial character used to split the input dial string into two:
  The first part before the ',' is sent to CM.
  The second part after the ',' is sent to IWF.
----------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_1X
#define DS_2STAGE_DIAL_CHR ','
#endif

/*---------------------------------------------------------------------------
  Structure that holds dial string info.
---------------------------------------------------------------------------*/
typedef struct
{
#ifdef FEATURE_JCDMA_1X
  byte jcdma_dial_str_to_cm[MAX_LINE_SIZE];  /* dial string sent to CM in orig. message */
  byte jcdma_dial_str_to_iwf[MAX_LINE_SIZE]; /* dial string sent to IWF in send_config  */
#endif
  const byte *dial_str;                                    /* dial string  */
  boolean    dig_mode;                                     /* digit mode   */

} ds707_async_dial_info_type;


/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  Structure that holds state and other info for async call.
---------------------------------------------------------------------------*/
ds707_async_state_type   ds707_async_state;

/*---------------------------------------------------------------------------
  Structure that holds dial string info. Needs to be passed back to atcop
  after traffic channel and protocols established.
---------------------------------------------------------------------------*/
ds707_async_dial_info_type ds707_async_dial_info;

/*---------------------------------------------------------------------------
  Dial string for user answer (ATA). Needs to be passed back to atcop
  after traffic channel and protocols established.
---------------------------------------------------------------------------*/
const byte  ds707_async_ans_dial_str[2] = "A";

/*---------------------------------------------------------------------------
  Structure to register with 3G DSMGR.  Has all the callbacks related to
  traffic channel call control for 707 async mode-specific handler.
---------------------------------------------------------------------------*/
LOCAL ds3g_hdlr_func_tbl_type ds707_async_mgr_hdlr_func_tbl =
{
  ds707_async_orig_hdlr,              /* Call origination handler          */
  ds707_async_call_id_hdlr,           /* Call id assignment handler        */
  NULL,                               /* CALL_CONF handler                 */
  ds707_async_call_conn_hdlr,         /* Call connected handler            */
  NULL,                               /* RAB REL handler                   */
  NULL,                               /* RAB EST handler                   */
  NULL,                               /* RAB EST REJ handler               */
  NULL,                               /* RAB EST FAIL handler              */
  NULL,                               /* COMPLETE_LL_CONN handler          */
  NULL,                               /* Setup handler                     */
  ds707_async_incoming_call_hdlr,     /* Incoming call handler             */
  ds707_async_user_answer_hdlr,       /* User or TE answers call handler   */
  ds707_async_user_end_hdlr,          /* User or TE ends the call          */
  NULL,                               /* COMPLETE_LL_DISCONN handler       */
  ds707_async_call_ended_hdlr,        /* Call ended handler                */
  ds707_async_switch_calls_hdlr,      /* Switch calls handler              */
  ds707_async_force_call_down_hdlr,   /* Force orig call down when incoming*/
  ds707_async_get_call_conn_info_hdlr,/* Gets call info string (CDMA/WCDMA)*/
  ds707_async_get_data_count_hdlr,    /* Get data counters handler         */
  ds707_async_get_last_sess_count_hdlr/* Get last data call counters info  */
};

/*---------------------------------------------------------------------------
  Structure to register with 3G SIOLIB.  Has callbacks related to SIO for
  707 async mode-specific handler.
---------------------------------------------------------------------------*/
LOCAL ds3g_siolib_func_tbl_type ds707_async_sio_func_tbl =
{
  ds707_async_esc_seq_hdlr,           /* Esc (+++)  handler                */
  ds707_async_dtr_changed_hdlr,       /* DTR changed handler               */
  NULL,                               /* DTR changed with amp_d=0          */
  NULL,                               /* Return to ONLINE DATA             */
  NULL,                               /* Return to ONLINE DATA complete    */
  NULL,                               /* Return to ONLINE CMD              */
  (void_func_ptr_type)ds707_async_call_answer_cb /* Auto answer depending on ATS0 */
};

/*---------------------------------------------------------------------------
  Data types needed to communicate with SIO.
---------------------------------------------------------------------------*/
sio_ioctl_param_type ds707_async_sio_ioctl_param;

#ifdef FEATURE_JCDMA_1X
/*---------------------------------------------------------------------------
  Used for jcdma 2-stage dialing, when determining if the chopped up string
  is DIGIT or ASCII mode. The function which does this needs an output
  buffer.  This is declared file static so as to not impact the stack-size
  needed by JCDMA
---------------------------------------------------------------------------*/
LOCAL  byte jcdma_dummy_dial_str[MAX_LINE_SIZE];
#endif

/*===========================================================================
                       INTERNAL MACRO DEFINITIONS
===========================================================================*/


/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

#ifdef FEATURE_JCDMA_1X
/*===========================================================================
FUNCTION      DS707_ASYNCI_INITIATE_JCDMA_CALL

DESCRIPTION   Initiates JCDMA async. call with special dial string processing.
              The input string requires some processing however:

  Legend:        
    x       = any character, including comma
    a, b, c = any character except comma
    
    [x+] = one or more consecutive occurences of x
    [x*] = zero or more consecutive occurences of x
    [^x] = not 'x'
    
  Examples:    
    [a+] = 1 or more occurences of random characters, but not the comma.
    [x*] = zero or more random characters ("ab243adc" or "aaaaaaa")
    [,+] = 1 or more consecutive comma's
    
  Rule 1: Comma is first character -> NO CARRIER
    
  User Input           _to CM/BS_            _to IWF_
    
  ATD                  NO CARRIER             
  ATD[,+]              NO CARRIER
  ATD[,+][x*]          NO CARRIER 

  Rule 2:  String to CM/BS does not include comma
  Rule 3:  String to IWF only includes comma if there are more characters
           after the first comma.
    
  ATD[a+]              [a+]                  [a+]
  ATD[a+][,+]          [a+]                  [a+]
  ATD[a+][,+][x+]      [a+]                  [a+][,+][x+]
              
 Any leading D's, T's, or P's are stripped.  An ATDT is pre-prended for the
 IWF in dsat_send_config().

DEPENDENCIES  The dial string from ATCOP is null terminated.

RETURN VALUE  True = call was initiated.  FALSE = call will fail.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_asynci_initiate_jcdma_call
(
  void
)
{
  char *cp;
  const byte *byte_ptr;
  boolean chars_post_comma = FALSE;
  boolean ds3g_status      = DS3G_FAILURE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    First, memset the jcdma dial strings to all NULL's
  -------------------------------------------------------------------------*/
  memset((void *)ds707_async_dial_info.jcdma_dial_str_to_cm,
         0,
         MAX_LINE_SIZE);

  memset((void *)ds707_async_dial_info.jcdma_dial_str_to_iwf,
         0,
         MAX_LINE_SIZE);

  /*-------------------------------------------------------------------------
    Copy the dial string into the two structures.  Remove any leading "D",
    "T" or "P".  Do this by finding the first byte which is not the
    mentioned letters.
  -------------------------------------------------------------------------*/
  byte_ptr = ds707_async_dial_info.dial_str;
  while ((*byte_ptr == 'D') || (*byte_ptr == 'd') ||
         (*byte_ptr == 'T') || (*byte_ptr == 't') || 
         (*byte_ptr == 'P') || (*byte_ptr == 'p')
        )
  {
    byte_ptr++;
  }

  if ( (*byte_ptr == NULL) || (*byte_ptr == DS_2STAGE_DIAL_CHR) )
  {
    /*-----------------------------------------------------------------------
      String is empty or first character is a comma.  According to spec,
      call cannot go through.  Return NO CARRIER.
    -----------------------------------------------------------------------*/
    return(ds3g_status);
  }

  /*-------------------------------------------------------------------------
    String has at least length one.  Copy into JCDMA dial string buf's.
  -------------------------------------------------------------------------*/
  std_strlcpy((char *)ds707_async_dial_info.jcdma_dial_str_to_cm,
              (const char *)byte_ptr, MAX_LINE_SIZE);
  std_strlcpy((char *)ds707_async_dial_info.jcdma_dial_str_to_iwf,
              (const char *)byte_ptr, MAX_LINE_SIZE);

  if ((cp = strchr((const char *)(ds707_async_dial_info.jcdma_dial_str_to_cm),
                   DS_2STAGE_DIAL_CHR)) != NULL )
  {
    MSG_MED("JCDMA 2-stage dial string detected", 0, 0, 0);

    /*-----------------------------------------------------------------------
      See if the first comma is also the last character in the string.
    -----------------------------------------------------------------------*/
    if (cp[1] != NULL)
    {
      chars_post_comma = TRUE;
    }

    /*-----------------------------------------------------------------------
       Truncate dial_str at the point just after the special JCDMA 2 stage
       dial string (not including the comma). This is the dial string that
       is sent in the origination message.

       Since the string has been truncated, also need to re-calculate the
       digits type.
    -----------------------------------------------------------------------*/
    *cp = '\0';

    ds707_async_dial_info.dig_mode =
               dsat_proc_dial_str(ds707_async_dial_info.jcdma_dial_str_to_cm,
                                  jcdma_dummy_dial_str
                                 );


    /*-----------------------------------------------------------------------
      If the original string ends in a single comma, then the IWF string is
      the same as the string to CM.  Otherwise, the string to the IWF is the
      original string.
    -----------------------------------------------------------------------*/
    if (chars_post_comma == FALSE)
    {
      std_strlcpy((char *)ds707_async_dial_info.jcdma_dial_str_to_iwf,
                  (const char *)ds707_async_dial_info.jcdma_dial_str_to_cm, 
                  MAX_LINE_SIZE);
    }
  }

  /*-----------------------------------------------------------------------
    Note that if the string did not contain a comma, we fell through to
    this spot, where the original dial string is sent on unchanged to both
    IWF and CM.
  -----------------------------------------------------------------------*/
  MSG_HIGH( "Initiating MtoL JCDMA async/fax call", 0, 0, 0);
  ds3g_status = ds3g_msh_initiate_call
                                 (DS_CALL_TYPE_CKT,
                                  SYS_SYS_MODE_CDMA,
                                  ds707_async_dial_info.jcdma_dial_str_to_cm,
                                  ds707_async_dial_info.dig_mode,
                                  NULL
                                 );

  return(ds3g_status);
} /* ds707_asynci_initiate_jcdma_call() */
#endif /* FEATURE_JCDMA_1X */

/*===========================================================================
FUNCTION      DS707_ASYNCI_SET_RLP_DEFAULTS

DESCRIPTION   Sets up the async_state_info with the default RLP Tx and Rx
              watermarks, queues and functions.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_asynci_set_rlp_defaults
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ds707_async_state.tc_state.tx_watermark_ptr = &ds707_wmk_um_SN_rev_wmk;
  ds707_async_state.tc_state.post_rx_func_ptr = ds707_async_post_rlp_rx_func;
  ds707_async_state.tc_state.post_rx_q_ptr    = &ds707_wmk_um_SN_fwd_q;

} /* ds707_asynci_set_rlp_defaults() */


/*===========================================================================
FUNCTION      DS707_ASYNCI_REG_RLP_SRVC

DESCRIPTION   Registers RLP Tx and Rx watermarks, queues and functions.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_asynci_reg_rlp_srvc
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*-------------------------------------------------------------------------
    Set up um watermark.
  -------------------------------------------------------------------------*/
  ds707_async_wmk_setup_um_wmk();

  rlp_reg_srvc(
               ds707_async_state.tc_state.tx_watermark_ptr,
               ds707_async_state.tc_state.post_rx_func_ptr,
               ds707_async_state.tc_state.post_rx_q_ptr
              );

  dsrlp_reg_srvc(
                 0,                            /* sr_id not used in async  */
                 ds707_async_state.tc_state.tx_watermark_ptr,
                 ds707_async_state.tc_state.post_rx_func_ptr,
                 ds707_async_state.tc_state.post_rx_q_ptr
                );

} /* ds707_asynci_reg_rlp_srvc() */


/*===========================================================================
FUNCTION      DS707_ASYNCI_IS_PKT_CALL_UP

DESCRIPTION   Checks if any pkt call is up. This function should be called
              whenever async is asked to come up either due to dial from
              laptop or incoming page.

DEPENDENCIES  None.

RETURN VALUE  True if any instance of a pkt call is up, otherwise false.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_asynci_is_pkt_call_up
(
  void
)
{
  int                  i;
  ps_iface_type       *iface_ptr;                /* ptr to pkt iface       */
  ps_iface_state_enum_type iface_state;          /* state of iface         */
  boolean flag = FALSE;                          /* return value           */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0; i < DS707_MAX_PKT_CALLS; i++)
  {
    iface_ptr   = ds707_pkt_get_iface_ptr ( i );
    iface_state = ps_iface_state ( iface_ptr );
    if (  ( iface_state == IFACE_UP ) || ( iface_state == IFACE_ROUTEABLE ) )
    {
      flag = TRUE;
      break;
    }
  }
  return flag;

} /* ds707_asynci_is_pkt_call_up() */


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
                        DS707 ASYNC MGR INITIALIZATION
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_MGR_POWERUP_INIT

DESCRIPTION   Called once at mobile power-up.  Registers the appropriate
              functionality with 3G DSMGR.

DEPENDENCIES  Assumes common watermarks, RLP, etc are initialized elsewhere.
              The things initialized here are specific to 707 async.

RETURN VALUE  Rex signals this mode specific handler is interested in.

SIDE EFFECTS  None.
===========================================================================*/
rex_sigs_type ds707_async_mgr_powerup_init
(
  void
)
{
  ps_iface_type             *iface_ptr;        /* ptr to async iface       */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ( "ds707_async_mgr_powerup_init()", 0, 0, 0 );

  /*-------------------------------------------------------------------------
    Register with 3G DSMGR.
  -------------------------------------------------------------------------*/
  ds3g_register_mode_sp_handler ( SYS_SYS_MODE_CDMA,
                                  DS_CALL_TYPE_CKT,
                                  &ds707_async_mgr_hdlr_func_tbl
                                );

  /*-------------------------------------------------------------------------
    Initialize dial string info.
  -------------------------------------------------------------------------*/
  ds707_async_dial_info.dial_str = NULL;
  ds707_async_dial_info.dig_mode = TRUE;


  /*-------------------------------------------------------------------------
    Set up the various service options with MC/CM.
  -------------------------------------------------------------------------*/
  ds707_so_async_init();
  ds707_so_g3fax_init();

  /*-------------------------------------------------------------------------
    Initialize async state.
  -------------------------------------------------------------------------*/
  ds707_async_state.tc_state.call_id   = CM_CALL_ID_INVALID;
  ds707_async_state.protocol_state     = DS707_ASYNC_PROTOCOL_CLOSED;
  ds707_async_state.no_carrier_txed    = FALSE;
  ds707_async_state.ok_result_txed     = FALSE;
  ds707_async_state.ps_closed_properly = FALSE;
  ds707_async_state.um_flow_enable     = TRUE;

  /*-------------------------------------------------------------------------
    Initialize RLP for this async_state.
  -------------------------------------------------------------------------*/
  ds707_asynci_set_rlp_defaults();

  /*-------------------------------------------------------------------------
    Initialize timers, watermarks, queues.
  -------------------------------------------------------------------------*/
  ds707_async_timer_init();
  ds707_async_wmk_init();
  q_init ( &ps707_async_state.tcp_send_q );

  /*-------------------------------------------------------------------------
    Initialize the PS_IFACE info for async instance.
  -------------------------------------------------------------------------*/
  iface_ptr     = &(ds707_async_state.ps_iface);
  ps_iface_create (iface_ptr, CDMA_SN_IFACE, NULL);

  /*-------------------------------------------------------------------------
    Initialize PS task related initializations.
  -------------------------------------------------------------------------*/
  ps707_async_powerup_init();

  /*-------------------------------------------------------------------------
    These bring up/tear down the ps iface.
  -------------------------------------------------------------------------*/
  iface_ptr->bring_up_cmd_f_ptr   = ds707_async_iface_bring_up;
  iface_ptr->tear_down_cmd_f_ptr  = ds707_async_iface_tear_down;

  /*-------------------------------------------------------------------------
    These bring up/tear down the traffic channel.
  -------------------------------------------------------------------------*/
  iface_ptr->phys_link_up_cmd_f_ptr   = NULL;
  iface_ptr->phys_link_down_cmd_f_ptr = ds707_async_phys_link_tear_down;


  /*-------------------------------------------------------------------------
    Enable 707 async iface.
  -------------------------------------------------------------------------*/
  ps_iface_enable_ind(iface_ptr);

  /*-------------------------------------------------------------------------
    Register unrecognized at cmd callback with atcop.
  -------------------------------------------------------------------------*/
  dsatcmdp_reg_unrec_cmd_handler (
                                   dsat707_unrec_func_ptr,
                                   CDMA_CMD_MODE
                                 );

  /*-------------------------------------------------------------------------
    Currently no signals we are interested in.
  -------------------------------------------------------------------------*/
  return((rex_sigs_type)0);

} /* ds707_async_mgr_powerup_init() */



/*===========================================================================
                           3G DSMGR HANDLERS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_ASYNC_ORIG_HDLR

DESCRIPTION   Called from 3G DSMGR when originating an async data call.

DEPENDENCIES  Watermarks to RLP already set up correctly.  Passed in ptrs are
              not NULL.

RETURN VALUE  DS3G_SUCCESS - call can be made.
              DS3G_FAILURE - call can't be made.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_orig_hdlr
(
  const byte                     *dial_string,
  uint8                          dial_string_len,
  cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
  boolean                        *cdma_params_changed,
  cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
  boolean                        *gw_cs_params_changed,
  cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
  boolean                        *gw_ps_params_changed,
  cm_srv_type_e_type             *cm_srv_type,
  void                           *mode_spec_info_ptr
)
{
  ds707_so_type                orig_so;          /* SO to orig with        */
  ds707_rateset_type           rateset;          /* originating rateset    */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ( "ds707_async_orig_hdlr()", 0, 0, 0 );

#ifdef FEATURE_DS_IS707A_CMUX
  rateset = (ds707_rateset_type)
                       (dsat707_cmux_rev_table[(dsat707_cmux_val[1])] - 1);
#else
  rateset = (ds707_rateset_type)(dsat707_cmux_val - 1);
#endif

  if ( dsat_fclass_val == 0 )
  {
    orig_so = ds707_so_async_get_orig (rateset);
  }
  else
  {
    orig_so = ds707_so_g3fax_get_orig ( rateset );
  }

  cdma_orig_params_ptr->srv_opt        = (uint16) orig_so;
  cdma_orig_params_ptr->activate_code  = CM_OTASP_ACT_CODE_NONE;
  cdma_orig_params_ptr->drs_bit        = 1; /* maybe NA in async           */
  cdma_orig_params_ptr->sr_id_included = FALSE;
  cdma_orig_params_ptr->qos_parms_incl = FALSE;
  *cdma_params_changed                 = TRUE;
  *cm_srv_type                         = CM_SRV_TYPE_CDMA_SPECIFIC;

  MSG_HIGH ("Orig 707 async/fax  SO %d MDR %d", orig_so,dsat707_qcmdr_val,0);

  return(DS3G_SUCCESS);

} /* ds707_async_orig_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ID_HDLR

DESCRIPTION   Called after call originates.  Gives the CM call id to this
              mode specific handler.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_call_id_hdlr
(
  cm_call_id_type        cm_call_id,
  void                  *mode_spec_info_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ("ds707_async_call_id_hdlr()", 0, 0, 0);

  ds707_async_state.tc_state.call_id = cm_call_id;

} /* ds707_async_call_id_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_CONN_HDLR

DESCRIPTION   Called by 3G DSMGR when traffic channel connects.

DEPENDENCIES  None.

RETURN VALUE  DS3G_SUCCESS - call connected ok.
              DS3G_FAILURE - call failed.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_call_conn_hdlr
(
  cm_call_id_type           call_id,
  cm_call_mode_info_s_type  *mode_info_ptr,
  boolean                   *delay_connect,
  cm_end_params_s_type      *end_params
)
{
  boolean        ret_val = DS3G_FAILURE;   /* success/failure return value */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_async_call_conn_hdlr()", 0, 0, 0);

  *delay_connect = FALSE;   /* Always FALSE.  For wcdma/gsm only           */

  if (ds707_async_state.tc_state.call_id != call_id)
  {
    ERR("Call conn with unknown id %d",call_id, 0, 0);
    end_params->call_id   = call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    ret_val = DS3G_FAILURE;
  }
  else
  {

     #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
        clk_regime_register_for_cpu_resource
         (
           CLKRGM_CDMA_DATA_APPS
        );

     #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */



    /*-----------------------------------------------------------------------
      Call came up and it's all good. Assumption is that RLP was configured
      either on MS origination or when receiving PAGE from BS.
    -----------------------------------------------------------------------*/
    ds707_async_state.tc_state.so = mode_info_ptr->info.cdma_call.srv_opt;

    /*-----------------------------------------------------------------------
      DS707 Async service, Open TCP request to PS Task.
    -----------------------------------------------------------------------*/
    MSG_HIGH( "707 Async/Fax starting, so=%d",
              ds707_async_state.tc_state.so, 0, 0);

    if (ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_CLOSED)
    {
      MSG_LOW (" Changing async ptcl state from %d to %d",
                ds707_async_state.protocol_state,
                (int) DS707_ASYNC_PROTOCOL_OPENING,
                0);
      ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_OPENING;

      /*---------------------------------------------------------------------
        Update iface phys link state.
      ---------------------------------------------------------------------*/
      ps_iface_phys_link_up_ind ( &(ds707_async_state.ps_iface) );

      /*---------------------------------------------------------------------
        Send cmd to PS task to open TCP actively or passively depending
        on whether this was an outgoing or incoming call. Deduced from dial
        string ATD or ATA.
      ---------------------------------------------------------------------*/
      if ( ds707_async_dial_info.dial_str[0] != 'A' )
      {
        ps_send_cmd ( PS_707_ASYNC_TCP_CTL_OPEN_ACTIVE_CMD, NULL );
      }
      else
      {
        ps_send_cmd ( PS_707_ASYNC_TCP_CTL_OPEN_PASSIVE_CMD, NULL );
      }

      /*---------------------------------------------------------------------
        Now start a timer. If we haven't received a opened from
        psmgr till the timer goes off, we abort the call.
      ---------------------------------------------------------------------*/
      ds3gi_start_timer ( DS3G_TIMER_ASYNC_PTCL_OPENING,
                          DS707_ASYNC_PTCL_OPENING_TIMER_VAL );

      ret_val = DS3G_SUCCESS;
    }
    else
    {
      ERR ("Call conn when Protocols open",0,0,0);
    }
  }
  return(ret_val);

} /* ds707_async_call_conn_hdlr */


/*===========================================================================
FUNCTION      DS707_ASYNC_INCOMING_CALL_HDLR

DESCRIPTION   Called when CM receives a page from BS.

DEPENDENCIES  None.

RETURN VALUE  Tells ds3gmgr to alert (ring) user or reject call if error.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_incom_e_type ds707_async_incoming_call_hdlr
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         cm_call_id,
  cm_ans_params_s_type   *ans_params, /* Answer params sent to Call Mgr    */
  cm_end_params_s_type   *end_params  /* end params if call rejected       */
)
{
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
  boolean               accept_call = TRUE;   /* JCDMA may change this val */
#ifdef FEATURE_JCDMA_1X
  uint16                inc_srv_opt;
#endif /* FEATURE_JCDMA_1X */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ( "ds707_async_incoming_call_hdlr()", 0, 0, 0);

#ifdef FEATURE_JCDMA_1X
  inc_srv_opt = cmd_ptr->cmd.call_info.mode_info.info.cdma_call.srv_opt;
  accept_call = ds707_so_jcdma_verify_inc_async_fax_so(inc_srv_opt);
#endif /* FEATURE_JCDMA_1X */

  /*-------------------------------------------------------------------------
    At this point we assume the traffic cop (TBD: TD/3gdsmgr/atcop
    has made the decision based on coverage, incoming srv option, etc
    that an async call is ok. If not add condition here and the else
    part becomes valid. For now, we just do a check that no pkt call
    is up.
  -------------------------------------------------------------------------*/
  if (( ds707_asynci_is_pkt_call_up() == FALSE ) &&
      (accept_call == TRUE)
     )
  {
    /*-----------------------------------------------------------------------
      Disable 1x pkt iface.
    -----------------------------------------------------------------------*/
    pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
    ps_iface_disabled_ind ( pkt_iface_ptr );

    ds707_async_state.tc_state.call_id = cm_call_id;
    ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
    ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_CS_DATA;

    /*-----------------------------------------------------------------------
      Register ATA callback with atcop. ATH handler is NULL as ATH is handled
      at IWF in async.
    -----------------------------------------------------------------------*/
    dsat_register_handlers (
                             ds707_async_call_answer_cb,
                             NULL
                           );

    /*-----------------------------------------------------------------------
      Register for desired events (dtr signal) with siolib.  If the
      registration fails drop the call since siolib is already in use.
    -----------------------------------------------------------------------*/
    if( ds3g_siolib_register_callback_func ( &ds707_async_sio_func_tbl)
        == FALSE )
    {
      ERR ("ds3g_siolib_register_callback_func() returned fail",0,0,0);
      ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_ENABLE );
      ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
      ps_iface_enable_ind ( pkt_iface_ptr );
      return ( DS3G_REJECT_CALL );
    }

    /*-----------------------------------------------------------------------
      Register RLP.
    -----------------------------------------------------------------------*/
    ds707_asynci_reg_rlp_srvc();

    /*-----------------------------------------------------------------------
      Alert user.
    -----------------------------------------------------------------------*/
    ds3g_siolib_state.ring_type = DSAT_RING;
    ds3g_siolib_alert_user( ds3g_siolib_state.ring_type, FALSE );

    return ( DS3G_ALERT_USER );
  }
  else
  {
      end_params->call_id = cm_call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    return ( DS3G_REJECT_CALL );
  }
} /* ds707_async_incoming_call_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_USER_ANSWER_HDLR

DESCRIPTION   Called by CM after it gets the go-ahead to answer call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_user_answer_hdlr
(
  cm_call_id_type        cm_call_id,
  cm_ans_params_s_type  *ans_parameters /* Answer params sent to Call Mgr  */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ( "ds707_async_user_answer_hdlr()", 0, 0, 0);

  ans_parameters->info_type = CM_CALL_MODE_INFO_CDMA;
  ans_parameters->ans_params.cdma_ans.call_type = CM_CALL_TYPE_CS_DATA;
  ds707_async_state.tc_state.call_id = cm_call_id;

} /* ds707_async_user_answer_hdlr() */



/*===========================================================================
FUNCTION      DS707_ASYNC_USER_END_HDLR

DESCRIPTION   Called by 3G DSMGR before telling CM to end a call.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_user_end_hdlr
(
  cm_call_id_type        call_id,
  cm_end_params_s_type  *end_params,
  boolean               *delay_disconnect,
  void                  *mode_spec_info_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW ( "ds707_async_end_call_hdlr()", 0, 0, 0);

  if (ds707_async_state.tc_state.call_id != call_id)
  {
    ERR("CALL IDs dont match. Still continue", 0, 0, 0);
  }

  *delay_disconnect = FALSE;
  end_params->call_id = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_CDMA;
  end_params->end_params.cdma_end.end_reason_included = TRUE;
  end_params->end_params.cdma_end.end_reason = CAI_REL_NORMAL;

} /* ds707_async_user_end_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ENDED_HDLR

DESCRIPTION   Called by 3G DSMGR when a call release has been received from
              CM.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_call_ended_hdlr
(
  ds_cmd_type            *cmd_ptr,
  boolean                 call_was_connected
)
{
  ps_iface_type       *iface_ptr;                    /* ptr to async iface */
  ps_iface_type       *pkt_iface_ptr;                /* ptr to pkt iface   */
  int16               tear_down_errno;  /* errno set by ps_iface_tear_down */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW( "ds707_async_call_ended_hdlr()", 0, 0, 0);

  #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
     clk_regime_deregister_for_cpu_resource
      (
        CLKRGM_CDMA_DATA_APPS
     );

  #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */



  iface_ptr = &(ds707_async_state.ps_iface);

  /*-------------------------------------------------------------------------
    Update carrier detect pin state.
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );

  /*-------------------------------------------------------------------------
    Update iface phys link state.
  -------------------------------------------------------------------------*/
  ps_iface_phys_link_down_ind ( iface_ptr );

  if (   ( ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPEN )
      || ( ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPENING )
      || ( ps_iface_state ( iface_ptr ) == IFACE_UP )
      || ( ps_iface_state ( iface_ptr ) == IFACE_COMING_UP  )
     )
  {
    ps_iface_tear_down_cmd ( iface_ptr, &tear_down_errno, NULL );
  }
  else
  {
    MSG_HIGH( "Sending ps_iface_down_ind", 0, 0, 0);
    ps_iface_down_ind ( iface_ptr );

    /*-----------------------------------------------------------------------
      Enable 1x pkt iface.
    -----------------------------------------------------------------------*/
    pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
    ps_iface_enable_ind ( pkt_iface_ptr );
    ds3g_siolib_stop_alerting();
    dsat_deregister_handlers();
    ds3g_siolib_deregister_callback_func ( &ds707_async_sio_func_tbl );
    ds3g_siolib_change_mode (
                               SIO_DS_AUTODETECT_MODE,
                               NULL,
                               NULL,
                               NULL
                            );
    ds3g_siolib_set_inbound_flow ( DS_FLOW_ALL_MASK, DS_FLOW_ENABLE );
  }

  ds707_async_state.tc_state.call_id   = CM_CALL_ID_INVALID;

} /* ds707_async_call_ended_hdlr() */



/*===========================================================================
FUNCTION      DS707_ASYNC_SWITCH_CALLS_HDLR

DESCRIPTION   Called when CM receives an incoming page for the same type of
              call as what we are originating.

              In this case, we want to end the origination, but accept the
              incoming page.  We must then switch our call_id to the
              incoming call id.

DEPENDENCIES  None.

RETURN VALUE  Tells ds3gmgr alerting user or reject call if error, for
              incoming call.

SIDE EFFECTS  None.
===========================================================================*/
ds3g_incom_e_type ds707_async_switch_calls_hdlr
(
  ds_cmd_type           *cmd_ptr,        /* Info from CM                   */
  cm_call_id_type        inc_call_id,    /* incoming call id               */
  cm_call_id_type        exist_call_id,  /* outgoing call id               */
  cm_ans_params_s_type  *ans_params,     /* parms for call to be accepted  */
  cm_end_params_s_type  *end_params_orig,/* parms for call to be ended     */
  cm_end_params_s_type  *end_params_inc  /* if incoming call to be ended   */
)
{
  ps_iface_type        *iface_ptr;                   /* ptr to async iface */
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
  ps_iface_state_enum_type  iface_state;    /* down/comingup/up/going down */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_switch_calls_hdlr()", 0, 0, 0);

  iface_ptr = &(ds707_async_state.ps_iface);
  iface_state =  ps_iface_state ( iface_ptr );
  pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );

  /*-------------------------------------------------------------------------
    If we have a call in "coming-up state", which means origination, it is
    going to be ended by 3GDSMGR. So clean it up.
  -------------------------------------------------------------------------*/
  if (
       ( iface_state == IFACE_COMING_UP ) &&
       ( ds707_async_state.tc_state.call_id == exist_call_id )
     )
  {
    end_params_orig->call_id = exist_call_id;
    end_params_orig->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params_orig->end_params.cdma_end.end_reason_included = TRUE;
    end_params_orig->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;

    ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
    ps_iface_phys_link_down_ind ( iface_ptr );
    ds707_async_iface_tear_down_cmd_hdlr();
    ps_iface_down_ind ( iface_ptr );
    ps_iface_enable_ind ( pkt_iface_ptr );
  }
  else
  {
    ERR("CALL IDs dont match. Still continue switch_calls", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
    Accept incoming call.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    At this point we assume the traffic cop (TBD: TD/3gdsmgr/atcop
    has made the decision based on coverage, incoming srv option, etc
    that an async call is ok. If not add condition here and the else
    part becomes valid. For now, we just do a check that no pkt call
    is up.
  -------------------------------------------------------------------------*/
  if ( ds707_asynci_is_pkt_call_up() == FALSE )
  {
    /*-----------------------------------------------------------------------
      Disable 1x pkt iface.
    -----------------------------------------------------------------------*/
    ps_iface_disabled_ind ( pkt_iface_ptr );

    /*-----------------------------------------------------------------------
      Register ATA callback with atcop. ATH handler is NULL as ATH is handled
      at IWF in async.
    -----------------------------------------------------------------------*/
    dsat_register_handlers (
                             ds707_async_call_answer_cb,
                             NULL
                           );

    /*-----------------------------------------------------------------------
      Register for desired events (dtr signal) with siolib.  If the
      registration fails drop the call since siolib is already in use.
    -----------------------------------------------------------------------*/
    if( ds3g_siolib_register_callback_func ( &ds707_async_sio_func_tbl)
        == FALSE )
    {
      ERR ("ds3g_siolib_register_callback_func() returned fail",0,0,0);
      ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_ENABLE );
      ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
      ps_iface_enable_ind ( pkt_iface_ptr );
      return ( DS3G_REJECT_CALL );
    }

    /*-----------------------------------------------------------------------
      Register RLP.
    -----------------------------------------------------------------------*/
    ds707_asynci_reg_rlp_srvc();

    /*-----------------------------------------------------------------------
      Alert user.
    -----------------------------------------------------------------------*/
    ds3g_siolib_state.ring_type = DSAT_RING;
    ds3g_siolib_alert_user( ds3g_siolib_state.ring_type, FALSE );

    ds707_async_state.tc_state.call_id = inc_call_id;
    ans_params->info_type = CM_CALL_MODE_INFO_CDMA;
    ans_params->ans_params.cdma_ans.call_type = CM_CALL_TYPE_CS_DATA;
    return ( DS3G_ALERT_USER );
  }
  else
  {
    end_params_inc->call_id = inc_call_id;
    end_params_inc->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params_inc->end_params.cdma_end.end_reason_included = TRUE;
    end_params_inc->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;
    return ( DS3G_REJECT_CALL );
  }
} /* ds707_async_switch_calls_hdlr */


/*===========================================================================
FUNCTION      DS707_ASYNC_FORCE_CALL_DOWN_HDLR

DESCRIPTION   Called by 3G DSMGR when there's a simultaneous incoming and
              outgoing call. The outgoing call has to be torn down and needs
              end params which are supplied in this function. The call ended
              handler won't be called later in this case.

DEPENDENCIES  None.

RETURN VALUE  TRUE - Indicates there was a call to be torn down.
              FALSE- Indicates there were no active/originating calls.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_force_call_down_hdlr
(
  cm_call_id_type      exist_call_id,/*Call id of outgoing call to be ended*/
  cm_end_params_s_type *end_params   /*End params to be filled in function */
)
{
  ps_iface_type        *iface_ptr;                   /* ptr to async iface */
  ps_iface_state_enum_type  iface_state;    /* down/comingup/up/going down */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_switch_calls_hdlr()", 0, 0, 0);

  iface_ptr = &(ds707_async_state.ps_iface);
  iface_state =  ps_iface_state ( iface_ptr );

  /*-------------------------------------------------------------------------
    If we have a call in "coming-up state", which means origination, it is
    going to be ended by 3GDSMGR. So clean it up.
  -------------------------------------------------------------------------*/
  if (
       ( iface_state == IFACE_COMING_UP ) &&
       ( ds707_async_state.tc_state.call_id == exist_call_id )
     )
  {
    end_params->call_id = exist_call_id;
    end_params->info_type = CM_CALL_MODE_INFO_CDMA;
    end_params->end_params.cdma_end.end_reason_included = TRUE;
    end_params->end_params.cdma_end.end_reason = CAI_REL_SRV_INACT;

    ds3g_siolib_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF );
    ps_iface_phys_link_down_ind ( iface_ptr );
    ds707_async_iface_tear_down_cmd_hdlr();
    ps_iface_down_ind ( iface_ptr );
    #ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
      clk_regime_deregister_for_cpu_resource
       (
         CLKRGM_CDMA_DATA_APPS
      );

    #endif /* FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING */

    return ( TRUE );
  }
  else
  {
    ERR("CALL IDs dont match in force_call_down()", 0, 0, 0);
    return ( FALSE );
  }
} /* ds707_async_force_call_down_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_GET_CALL_CONN_INFO_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes ds_get_call_information()
              to get call info Eg: "CDMA" as a NULL terminated string.

DEPENDENCIES  None.

RETURN VALUE  Indicates if call connection info is valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_async_get_call_conn_info_hdlr
(
  cm_call_id_type call_id,
  byte            *system_information,
  byte            max_system_information_length
)
{
  static char               *sys_info     = "CDMA";
  boolean                    ret_val      = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_async_get_call_conn_info_hdlr()",0,0,0);


  if ( ds707_async_state.tc_state.call_id != call_id )
  {
    ERR("get callconn info unknown id %d",call_id, 0, 0);
    ret_val = FALSE;
  }
  else
  {
    if (max_system_information_length > strlen (sys_info))
    {
      std_strlcpy ((char *)system_information, sys_info, 
                   max_system_information_length);
      ret_val = TRUE;
    }
    else
    {
      ERR (" Get callconn info: Strlen exceeded",0,0,0 );
      ret_val = FALSE;
    }
  }

  return ret_val;

} /* ds707_async_get_call_conn_info_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_GET_DATA_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_current_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean  ds707_async_get_data_count_hdlr
(
  cm_call_id_type    call_id,
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
)
{
  boolean                    ret_val         = FALSE;
  rlp_ext_stats_type         rlp_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_async_get_data_count_hdlr()",0,0,0);

  if ( ds707_async_state.tc_state.call_id != call_id )
  {
    ERR("get data count unknown id %d",call_id, 0, 0);
    ret_val = FALSE;
  }
  else
  {
    rlp_get_stats ( &rlp_stats );
    *rx_from_network_byte_cnt = rlp_stats.rx_data_cnt;
    *tx_to_network_byte_cnt   = rlp_stats.tx_data_cnt;
    ret_val = TRUE;
  }

  return ret_val;
} /* ds707_async_get_data_count_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_GET_LAST_SESS_COUNT_HDLR

DESCRIPTION   Called by 3G DSMGR when UI invokes
              ds_get_last_session_data_counters() to get total count of
              bytes transmitted and received by RLP for the last call which
              is being released or current call.

DEPENDENCIES  None.

RETURN VALUE  boolean:  indicates if the count is valid.

SIDE EFFECTS  None.
===========================================================================*/
boolean  ds707_async_get_last_sess_count_hdlr
(
  uint32            *rx_from_network_byte_cnt,
  uint32            *tx_to_network_byte_cnt
)
{
  boolean                    ret_val         = FALSE;
  rlp_ext_stats_type         rlp_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("ds707_async_get_last_sess_count_hdlr()",0,0,0);

  rlp_get_stats ( &rlp_stats );
  *rx_from_network_byte_cnt = rlp_stats.rx_data_cnt;
  *tx_to_network_byte_cnt   = rlp_stats.tx_data_cnt;
  ret_val = TRUE;

  return ret_val;
} /* ds707_async_get_last_sess_count_hdlr() */


/*===========================================================================
                          IFACE HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_BRING_UP

DESCRIPTION   Called via the async IFACE.  Brings up the async iface.

DEPENDENCIES  None.

RETURN VALUE  0 if success.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_async_iface_bring_up
(
  ps_iface_type    *iface_ptr,                   /* Pointer to async iface */
  void             *unused
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id         = DS_707_ASYNC_IFACE_BRING_UP_CMD;
    ds_put_cmd(cmd_ptr);
  }

  return(0);

} /* ds707_async_iface_bring_up() */


/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_BRING_UP_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface bring up
              command on the async iface.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_iface_bring_up_cmd_hdlr
(
  void
)
{
  ds3g_siolib_status_e_type siolib_status = DS3G_SIOLIB_OK;
  boolean ds3g_status = DS3G_SUCCESS;
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_iface_bring_up_cmd_hdlr()", 0, 0, 0);

  pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );

  /*-------------------------------------------------------------------------
    Initialize async state variables and flags.
  -------------------------------------------------------------------------*/
  ds707_async_state.protocol_state     = DS707_ASYNC_PROTOCOL_CLOSED;
  ds707_async_state.no_carrier_txed    = FALSE;
  ds707_async_state.ok_result_txed     = FALSE;
  ds707_async_state.ps_closed_properly = FALSE;
  ds707_async_state.um_flow_enable     = TRUE;

  /*-------------------------------------------------------------------------
    Function to initialize 617 counters.
  -------------------------------------------------------------------------*/
  ps707_async_call_init();

  /*-------------------------------------------------------------------------
    Set up um watermarks.
  -------------------------------------------------------------------------*/
  ds707_async_wmk_setup_um_wmk();
  ds707_async_wmk_setup_sio_wmk();

  /*-------------------------------------------------------------------------
    Set up SIO for rawdata processing, which will also set up sio watermark.
  -------------------------------------------------------------------------*/
  siolib_status = ds3g_siolib_change_mode (
                                           SIO_DS_RAWDATA_MODE,
                                           NULL,
                                           &ds707_wmk_async_to_sio_tx_wmk,
                                           ds707_async_ps_tcp_enqueue
                                          );
  if ( siolib_status != DS3G_SIOLIB_OK )
  {
    ERR ("ds3g_siolib_change_mode() returned fail",0,0,0);
    ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
    return;
  }

  ds3g_siolib_set_call_coming_up( TRUE );

  /*-------------------------------------------------------------------------
    Register callbacks with atcop for formatting responses with 617 codes
    and routing them to the air link.
  -------------------------------------------------------------------------*/
  dsat_reg_rsp_fmter  ( ds707_async_at_rsp_fmting_func_hdlr  );
  dsat_reg_rsp_router ( ds707_async_at_rsp_routing_func_hdlr );
  dsat_reg_atz_cb     ( ds707_async_atz_hdlr );

  /*-------------------------------------------------------------------------
    Register ATA callback with atcop. ATH handler is NULL as ATH is handled
    at IWF in async.
  -------------------------------------------------------------------------*/
  dsat_register_handlers (
                           ds707_async_call_answer_cb,
                           NULL
                         );

  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_DISABLE );

  /*-------------------------------------------------------------------------
    Depending on whether this is outgoing or incoming call (deduced from
    ATD or ATA), call appropriate ds3gmgr fn for setting up traffic channel.
  -------------------------------------------------------------------------*/
  if ( ds707_async_dial_info.dial_str[0] != 'A' )
  {
#ifdef FEATURE_JCDMA_1X
    ds3g_status = ds707_asynci_initiate_jcdma_call();
#else
    MSG_HIGH( "Initiating MtoB 707 async/fax call", 0, 0, 0);
    ds3g_status = ds3g_msh_initiate_call (
                                          DS_CALL_TYPE_CKT,
                                          SYS_SYS_MODE_CDMA,
                                          ds707_async_dial_info.dial_str,
                                          ds707_async_dial_info.dig_mode,
                                          NULL
                                         );
#endif
  }
  else
  {
    MSG_HIGH( "Answering BtoM 707 async/fax call", 0, 0, 0);
    ds3g_status = ds3g_msh_answer_call (ds707_async_state.tc_state.call_id );
  }

  if ( ds3g_status == DS3G_FAILURE )
  {
    ERR ( "ds3g_msh_initiate_call()/answer_call() returned fail", 0, 0, 0);
    ds3g_siolib_set_call_coming_up( FALSE );
    dsat_reg_rsp_fmter      ( NULL );
    dsat_reg_rsp_router     ( NULL );
    dsat_reg_atz_cb         ( NULL );
    dsat_deregister_handlers();

    dsat_send_result_code( DSAT_NO_CARRIER );
    ds707_async_state.no_carrier_txed = TRUE;

    ds3g_siolib_deregister_callback_func ( &ds707_async_sio_func_tbl );
    ds3g_siolib_change_mode (
                             SIO_DS_AUTODETECT_MODE,
                             NULL,
                             NULL,
                             NULL
                            );
    ds3g_siolib_set_inbound_flow ( DS_FLOW_ALL_MASK, DS_FLOW_ENABLE );
    ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
    ps_iface_enable_ind ( pkt_iface_ptr );
    return;
  }

} /* ds707_async_iface_bring_up_cmd_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_TEAR_DOWN

DESCRIPTION   Called via the async IFACE.  Tears down the async iface.

DEPENDENCIES  None.

RETURN VALUE  0 if success.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_async_iface_tear_down
(
  ps_iface_type    *iface_ptr,
  void             *not_used
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id         = DS_707_ASYNC_IFACE_TEAR_DOWN_CMD;
    ds_put_cmd(cmd_ptr);
  }
  return(0);
} /* ds707_async_iface_tear_down() */


/*===========================================================================
FUNCTION      DS707_ASYNC_IFACE_TEAR_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to an iface down command
              on the 1X iface.

DEPENDENCIES  Called in DS TASK context.  Called only when PS_IFACE is up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_iface_tear_down_cmd_hdlr
(
  void
)
{
  dsm_item_type *item_ptr;
  ds3gi_call_state_e_type cstate3g;               /* Call state of 3GDSMGR */
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_iface_tear_down_cmd_hdlr()", 0, 0, 0);

  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_DISABLE );

  /*-------------------------------------------------------------------------
    Deregister atcop callbacks.
  -------------------------------------------------------------------------*/
  dsat_reg_rsp_fmter      ( NULL );
  dsat_reg_rsp_router     ( NULL );
  dsat_reg_atz_cb         ( NULL );
  dsat_deregister_handlers();

  /*-------------------------------------------------------------------------
    Cancelling the timer which was started when dsmgr asked psmgr
    to open the protocols.
  -------------------------------------------------------------------------*/
  if (ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPENING)
  {
    ds3gi_stop_timer ( DS3G_TIMER_ASYNC_PTCL_OPENING );
  }

  /*-------------------------------------------------------------------------
    Set protocol state to closed and do related processing. If protocol
    state is already closed, no change.
  -------------------------------------------------------------------------*/
  if ( ds707_async_state.protocol_state != DS707_ASYNC_PROTOCOL_CLOSED )
  {
    MSG_LOW ( "Changing protocol state from %d to %d",
              ds707_async_state.protocol_state,
              (int) DS707_ASYNC_PROTOCOL_CLOSING,
              0);
    ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_CLOSING;
  }

  /*-------------------------------------------------------------------------
    Bring down protocols.
  -------------------------------------------------------------------------*/
  if (ds707_async_state.protocol_state !=  DS707_ASYNC_PROTOCOL_CLOSED)
  {
    /*-----------------------------------------------------------------------
      Rawdata call abruptly dropped. Bring down the protocols.
      Send cmd to PS task to abort protocols.
    -----------------------------------------------------------------------*/
    ps_send_cmd ( PS_707_ASYNC_PTCL_ABORT_CMD, NULL );
  }

  /*-------------------------------------------------------------------------
    Ask ds3gmgr to tear down traffic channel.
  -------------------------------------------------------------------------*/
  cstate3g = ds3g_get_call_state(ds707_async_state.tc_state.call_id);
  if (
         ( cstate3g != DS3G_INVALID_CALL_STATE )
      && ( cstate3g != DS3G_IDLE_CALL_STATE    )
      && ( cstate3g != DS3G_END_CALL_STATE     )
     )
  {
    ds3g_msh_hangup_call ( ds707_async_state.tc_state.call_id, NULL );
  }

  /*-------------------------------------------------------------------------
    Reset RLP and all queues, etc used for the flow of payload.
  -------------------------------------------------------------------------*/
  rlp_reset();

  /*-------------------------------------------------------------------------
    Remove all data items from watermarks, queues.
  -------------------------------------------------------------------------*/
  dsm_empty_queue (&ds707_wmk_um_SN_rev_wmk);
  dsm_empty_queue (&ds707_wmk_async_to_sio_tx_wmk);

  while(( item_ptr = ps707_async_tcp_dequeue()) != NULL)
  {
    dsm_free_packet( &item_ptr);
  }

  ds707_async_dial_info.dial_str = NULL;
  ds707_async_dial_info.dig_mode = FALSE;

  ds3g_siolib_set_call_coming_up ( FALSE );

  ds3g_siolib_deregister_callback_func ( &ds707_async_sio_func_tbl );

  ds3g_siolib_change_mode (
                           SIO_DS_AUTODETECT_MODE,
                           NULL,
                           NULL,
                           NULL
                          );
#ifdef FEATURE_JCDMA_1X
  /*-------------------------------------------------------------------------
    Japanese IWF's do not support IS-617, and so won't send NO CARRIER at
    the end of a call, so mobile must do it.  Represent this by saying that
    protocols did not close gracefully.  Do not set no_carrier_txed to FALSE,
    as that may cause 2 NO CARRIERs to be sent to mobile.
  -------------------------------------------------------------------------*/
  ds707_async_state.ps_closed_properly = FALSE;
#endif

  /*-------------------------------------------------------------------------
    No-carrier needs to be sent to serial port only if protocols have
    not been closed properly. If protocols were closed properly
    the IWF would have sent a NO CARRIER in a 617 message.
    Also, make sure that NO CARRIER was not txmitted before this
    point, so that a double no carrier is not sent.
  -------------------------------------------------------------------------*/
  if (
        ( ds707_async_state.ps_closed_properly == FALSE ) &&
        ( ds707_async_state.no_carrier_txed == FALSE )
     )
  {
    dsat_send_result_code( DSAT_NO_CARRIER );
    ds707_async_state.no_carrier_txed = TRUE;
    ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
  }

  ds3g_siolib_set_inbound_flow
      ( (DS_FLOW_707_CKT_MASK | DS_FLOW_707_CKT_TCP_MASK), DS_FLOW_ENABLE );

  /*-------------------------------------------------------------------------
    Enable 1x pkt iface.
  -------------------------------------------------------------------------*/
  pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
  ps_iface_enable_ind ( pkt_iface_ptr );

} /* ds707_async_iface_tear_down_cmd_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_PHYS_LINK_TEAR_DOWN

DESCRIPTION   Called via the async IFACE.  Tears down the traffic channel.
              Sends a message to the DS task which will cause the
              channel to be torn down.

DEPENDENCIES  None.

RETURN VALUE  0 if success.

SIDE EFFECTS  None.
===========================================================================*/
int ds707_async_phys_link_tear_down
(
  ps_iface_type     *iface_ptr,
  void              *not_used
)
{
  ds_cmd_type     *cmd_ptr;                      /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_phys_link_tear_down()", 0, 0, 0);

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 cant get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD;
    ds_put_cmd(cmd_ptr);
  }
  return(0);

} /* ds707_async_phys_link_tear_down() */


/*===========================================================================
FUNCTION      DS707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD_HDLR

DESCRIPTION   Called in DS TASK context in response to a phys link tear down
              command on the async iface.

DEPENDENCIES  Called in DS TASK context.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_phys_link_tear_down_cmd_hdlr
(
  void
)
{
  ds3gi_call_state_e_type cstate3g;               /* Call state of 3GDSMGR */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Ask ds3gmgr to tear down traffic channel.
  -------------------------------------------------------------------------*/
  cstate3g = ds3g_get_call_state(ds707_async_state.tc_state.call_id);
  if (
         ( cstate3g != DS3G_INVALID_CALL_STATE )
      && ( cstate3g != DS3G_IDLE_CALL_STATE    )
      && ( cstate3g != DS3G_END_CALL_STATE     )
     )
  {
    ds3g_msh_hangup_call ( ds707_async_state.tc_state.call_id, NULL );
  }
} /* ds707_async_phys_link_tear_down_cmd_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_GET_ACTIVE_IFACE_PTR

DESCRIPTION   Function that returns pointer to 707 async iface if it's
              active.

DEPENDENCIES  None.

RETURN VALUE  Ptr to 707 async ps_iface if active, else NULL.

SIDE EFFECTS  None.
===========================================================================*/
ps_iface_type* ds707_async_get_active_iface_ptr
(
  void
)
{
  ps_iface_type        *iface_ptr;                   /* ptr to async iface */
  ps_iface_state_enum_type  iface_state;    /* down/comingup/up/going down */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  iface_ptr = &(ds707_async_state.ps_iface);
  iface_state =  ps_iface_state ( iface_ptr );

  if ( ( iface_state == IFACE_UP ) || ( iface_state == IFACE_COMING_UP ) )
  {
    return iface_ptr;
  }
  else
  {
    return NULL;
  }
} /* ds707_async_get_active_iface_ptr() */


/*===========================================================================
                     DIAL STRING CALLBACKS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_DIAL_STR_CB()

DESCRIPTION   This function processes the dial string it has registered for
              and initiates the appropriate data call over IS707_ASYNC_IFACE.

RETURN VALUE  Ptr to call_abort_cb().

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
dsat_call_abort_cb_type ds707_async_dial_str_cb
(
  const byte *dial_string,                  /* Dial string to be processed */
  boolean    digit_mode, /* Flag tells if dial string contains digits only */
  dsat_dial_modifier_info_type  * modifiers       /* Dial sting modifiers  */
)
{
  ps_iface_type        *iface_ptr;                   /* ptr to async iface */
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
  int16                 bring_up_errno;  /* errno set by ps_iface_bring_up */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED ( "ds707_async_dial_str_cb()", 0, 0, 0);

#ifdef FEATURE_JCDMA_1X
  /*-------------------------------------------------------------------------
    If M511 indicates PKT, then all dial strings do Packet data call.
    Call the IS-707 pkt dial string handler, and return it's value. Async
    is then done.  Otherwise continue on...
  -------------------------------------------------------------------------*/
  if (ds707_jcdma_m51_make_pkt_call() == TRUE)
  {
    MSG_HIGH("M511 indicates PKT Call",0,0,0);
    return(ds707_rmsm_dial_str_cb(dial_string, digit_mode, NULL));
  }
#endif /* FEATURE_JCDMA_1X */

  /*-------------------------------------------------------------------------
    At this point we assume the traffic cop (TBD: TD/3gdsmgr/atcop
    has made the decision based on coverage, dial_string, etc, that it's a
    707-async call that has to be originated. For now, we just do a check
    that no pkt call is up.
  -------------------------------------------------------------------------*/

  if ( ds707_asynci_is_pkt_call_up() == FALSE )
  {
    ds707_async_dial_info.dial_str = dial_string;
    ds707_async_dial_info.dig_mode = digit_mode;

    /*-----------------------------------------------------------------------
      Disable 1x pkt iface.
    -----------------------------------------------------------------------*/
    pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
    ps_iface_disabled_ind ( pkt_iface_ptr );

    /*-----------------------------------------------------------------------
      Register for desired events (dtr signal) with siolib.  If the
      registration fails drop the call since siolib is already in use.
    -----------------------------------------------------------------------*/
    if( ds3g_siolib_register_callback_func ( &ds707_async_sio_func_tbl)
        == FALSE )
    {
      ERR ("ds3g_siolib_register_callback_func() returned fail",0,0,0);
      ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_MASK, DS_FLOW_ENABLE );
      ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
      ps_iface_enable_ind ( pkt_iface_ptr );
      return ds707_async_call_abort_cb;
    }

    /*-----------------------------------------------------------------------
      Register RLP.
    -----------------------------------------------------------------------*/
    ds707_asynci_reg_rlp_srvc();

    iface_ptr = &(ds707_async_state.ps_iface);
    ps_iface_bring_up_cmd ( iface_ptr, &bring_up_errno, NULL );
  }
  else
  {
    ERR ( "Pkt call already up", 0, 0, 0 );
    dsat_send_result_code( DSAT_NO_CARRIER );
  }

  return ds707_async_call_abort_cb;

} /* ds707_async_dial_str_cb() */


/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ANSWER_CB()

DESCRIPTION   This function is registered with atcop for handling ATA during
              an incoming call.

RETURN VALUE  Ptr to call abort cb.

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
dsat_call_abort_cb_type ds707_async_call_answer_cb
(
  void
)
{
  ps_iface_type        *iface_ptr;                   /* ptr to async iface */
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
  int16                 bring_up_errno;  /* errno set by ps_iface_bring_up */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED ( "ds707_async_call_answer_cb()", 0, 0, 0);

  /*-------------------------------------------------------------------------
    At this point we assume the traffic cop (TBD: TD/3gdsmgr/atcop
    has made the decision based on coverage, dial_string, etc, that it's a
    707-async call that has to be originated. For now, we just do a check
    that no pkt call is up.
  -------------------------------------------------------------------------*/

  if ( ds707_asynci_is_pkt_call_up() == FALSE )
  {
#ifdef FEATURE_JCDMA_1X
    std_strlcpy((char *)ds707_async_dial_info.jcdma_dial_str_to_iwf,
                (const char *)ds707_async_ans_dial_str, 
                MAX_LINE_SIZE);
#endif
    ds707_async_dial_info.dial_str = ds707_async_ans_dial_str;
    ds707_async_dial_info.dig_mode = FALSE;

    /*-----------------------------------------------------------------------
      Disable 1x pkt iface.
    -----------------------------------------------------------------------*/
    pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
    ps_iface_disabled_ind ( pkt_iface_ptr );

    /*-----------------------------------------------------------------------
      Stop alerting the user, since the user has answered the call.
    -----------------------------------------------------------------------*/
    ds3g_siolib_stop_alerting();

    iface_ptr = &(ds707_async_state.ps_iface);
    ps_iface_bring_up_cmd ( iface_ptr, &bring_up_errno, NULL );
  }
  else
  {
    ERR ( "Pkt call already up", 0, 0, 0 );
    dsat_send_result_code( DSAT_NO_CARRIER );
  }

  return ds707_async_call_abort_cb;

} /* ds707_async_call_answer_cb() */


/*===========================================================================
FUNCTION      DS707_ASYNC_CALL_ABORT_CB()

DESCRIPTION   This function is registered as abort handler with ATCOP. If
              ATCOP receives an AT cmd while processing an abortable cmd,
              this function gets called.  It is registered along with the
              dial string handler. It posts appropriate event to async_mgr
              which initiates the call termination.

RETURN VALUE  None.

DEPENDENCIES  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_call_abort_cb
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_call_abort_cb()", 0, 0, 0);

  ds3g_msh_hangup_call ( ds707_async_state.tc_state.call_id, NULL );

} /* ds707_async_call_abort_cb() */


/*===========================================================================
                           3G SIOLIB HANDLERS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_ESC_SEQ_HDLR

DESCRIPTION   Handler registered with SIO to be called when "+++" is entered
              on laptop.  Sends EIA 617 "escape to online command" message
              to IWF.

DEPENDENCIES  None.

RETURN VALUE  ONLINE_DATA mode, as mobile doesn't go to ONLINE_CMD mode in
              case of 707 async.

SIDE EFFECTS  None.
===========================================================================*/
dsat_mode_enum_type ds707_async_esc_seq_hdlr
(
  void
)
{
  dsm_item_type *item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED( "ds707_async_esc_seq_hdlr()", 0, 0, 0);

  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (item_ptr == NULL)
  {
    ERR_FATAL("Running out of buffers",0,0,0);
  }

  item_ptr->data_ptr[0] = EIA617_EM;
  item_ptr->data_ptr[1] = EIA617_EXTEND1_M2B;
  item_ptr->data_ptr[2] = EIA617_LEN_OFFSET + 1;
  item_ptr->data_ptr[3] = EIA617_ESCAPE;

  item_ptr->used = 4;
  item_ptr->kind = DSM_PS_APP_PAYLOAD;

  ds707_async_ps_tcp_enqueue( &item_ptr);

  return DSAT_ONLINE_DATA;

} /* ds707_async_esc_seq_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_DTR_CHANGED_HDLR

DESCRIPTION   Handler registered with SIO to be called when there is a
              change in dtr state.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_dtr_changed_hdlr
(
  boolean dtr_status
)
{
  ds3gi_call_state_e_type cstate3g;               /* Call state of 3GDSMGR */
  ps_iface_type          *iface_ptr;                 /* ptr to async iface */
  int16                  tear_down_errno; /*errno set by ps_iface_tear_down*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_dtr_changed_hdlr()", 0, 0, 0);

  cstate3g = ds3g_get_call_state(ds707_async_state.tc_state.call_id);
  if (
         ( cstate3g != DS3G_INVALID_CALL_STATE )
      && (
             ( cstate3g == DS3G_ORIG_CALL_STATE )
          || ( cstate3g == DS3G_CALL_ACTIVE_CALL_STATE )
         )
     )
  {
    MSG_MED ( "DTR change. amp_d_val=%d", dsat_amp_d_val, 0, 0);

    /*-----------------------------------------------------------------------
      If protocols are open and the traffic channel is up, then close the
      protocols gracefully, otherwise abort the protocols.
    -----------------------------------------------------------------------*/
    if(( ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPEN ) &&
       ( cstate3g == DS3G_CALL_ACTIVE_CALL_STATE ))
    {
      MSG_LOW ( "Changing protocol state from %d to %d",
                ds707_async_state.protocol_state,
                (int) DS707_ASYNC_PROTOCOL_CLOSING,
                0);
      ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_CLOSING;

      ds3g_siolib_set_inbound_flow( DS_FLOW_707_CKT_MASK, DS_FLOW_DISABLE );

      /*---------------------------------------------------------------------
        Send cmd to ps to close protocols gracefully.
      ---------------------------------------------------------------------*/
      ps_send_cmd ( PS_707_ASYNC_PTCL_CLOSE_CMD, NULL );
    }
    /*-----------------------------------------------------------------------
      If traffic channel is originating and protocols are already closed,
      we can tear down iface immediately. This can happen if a user starts
      a call and immediately brings it down.
    -----------------------------------------------------------------------*/
    else if (
         (ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_CLOSED )
      && ( cstate3g == DS3G_ORIG_CALL_STATE )
            )
    {
      iface_ptr = &(ds707_async_state.ps_iface);
      ps_iface_tear_down_cmd ( iface_ptr, &tear_down_errno, NULL );
    }
    else
    {
      MSG_LOW ( "Changing protocol state from %d to %d",
                ds707_async_state.protocol_state,
                (int) DS707_ASYNC_PROTOCOL_CLOSING,
                0);
      ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_CLOSING;

      ds3g_siolib_set_inbound_flow( DS_FLOW_707_CKT_MASK, DS_FLOW_DISABLE );

      /*---------------------------------------------------------------------
        Send cmd to ps to abort protocols.
      ---------------------------------------------------------------------*/
      ps_send_cmd ( PS_707_ASYNC_PTCL_ABORT_CMD, NULL );
    }
  }
  else
  {
    MSG_MED ( "Ignore DTR.", 0, 0, 0);
  }
} /* ds707_async_dtr_changed_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_AT_RSP_FMTING_FUNC_HDLR

DESCRIPTION   Handler registered with atcop to be called when an AT response
              needs to be formatted in online mode. This function does the
              insertion of 617 codes.

DEPENDENCIES  This should only be called while in online data mode, when
              AT cmds are reflected to IWF.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_at_rsp_fmting_func_hdlr
(
  dsm_item_type **in_ptr_ptr,         /* Raw response  in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in
                                         in_ptr_ptr                        */
  boolean is_result_code,                /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr,        /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
  dsat_result_enum_type result        /* Result code                       */
)
{
  byte    c;                     /* Location to store character            */
  byte   *src_ptr;               /* Pointer to source (input) buffer       */
  byte   *dest_ptr;              /* Pointer to destination (output) buffer */
  byte   *dest_end_ptr;          /* Pointer to end of destination buffer   */
  dsm_item_type *in_ptr;         /* Pointer to input dsm item              */
  byte   *l_ptr;                 /* Pointer to length byte in EIA_617 msg  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_at_rsp_fmting_func_hdlr()", 0, 0, 0);

  /*-------------------------------------------------------------------------
    If input pointer is NULL, for a regular response, there is nothing to do
    so exit.
  -------------------------------------------------------------------------*/
  if (
         ( is_result_code == FALSE )
      && ( in_ptr_ptr == NULL || *in_ptr_ptr == NULL )
     )

  {
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize pointer to input dsm item and verify output pointer is
    valid.  Exit if output pointer is invalid.
  -------------------------------------------------------------------------*/
  in_ptr = *in_ptr_ptr;

  /*-------------------------------------------------------------------------
   Input item is always freed, so initialize the return value here.
  -------------------------------------------------------------------------*/
  *in_ptr_ptr = NULL;

  ASSERT( out_ptr_ptr != NULL && *out_ptr_ptr != NULL );
  if ( out_ptr_ptr == NULL || *out_ptr_ptr == NULL )
  {
    dsm_free_packet( &in_ptr );
    return;
  }

  /*-------------------------------------------------------------------------
      Initialize source data pointers.
  -------------------------------------------------------------------------*/
  src_ptr = in_ptr->data_ptr;

  /*-------------------------------------------------------------------------
    Initialize destination pointers.
  -------------------------------------------------------------------------*/
  dest_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used];
  dest_end_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->size];
  ASSERT( dest_ptr < dest_end_ptr );

  /*-------------------------------------------------------------------------
    Check to see if we have enough room in the buffer. Otherwise, queue
    current item and get a new one.
  -------------------------------------------------------------------------*/
  if (     ((*out_ptr_ptr)->used)
       > ( ((*out_ptr_ptr)->size) - MIN_ROOM )
     )
  {
    dsatcmdp_queue_cmd_results( *out_ptr_ptr );

    *out_ptr_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
    if ( *out_ptr_ptr == NULL )
    {
      ERR( "Running out of buffers",0,0,0 );
      if ( in_ptr != NULL )
      {
        dsm_free_packet( &in_ptr );
      }
      return;
    }

    dest_ptr = (*out_ptr_ptr)->data_ptr;
    dest_end_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->size];
  }

  /*-------------------------------------------------------------------------
    Format EIA 617 inband message.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
      Send empty message for leading <CR><LF> if verbose
  -------------------------------------------------------------------------*/
  if ( dsat_v_val == 1 )
  {
    *dest_ptr++ = EIA617_EM;
    *dest_ptr++ = EIA617_EXTEND1_M2B;
    *dest_ptr++ = EIA617_LEN_OFFSET + 1;
    *dest_ptr++ = EIA617_RESP_2B;
  }

  *dest_ptr++ = EIA617_EM;
  *dest_ptr++ = EIA617_EXTEND1_M2B;
   l_ptr  = dest_ptr++;             /*  Remember location of length byte   */
  *l_ptr = EIA617_LEN_OFFSET + 1;   /*  Initial length includes sub-code   */

  if ( is_result_code == TRUE )
  {
    switch (result)
    {
      case DSAT_UNREC:
        *dest_ptr++ = EIA617_UNREC;
        break;

      case DSAT_PARAM:
        *dest_ptr++ = EIA617_PARAM;
        break;

      default:
        *dest_ptr++ = EIA617_VALID;
        break;
    }
  }
  else                            /*  Data response */
  {
    *dest_ptr++ = EIA617_RESP_2B;
  }

  /*-------------------------------------------------------------------------
    Q1 (quiet) turns off result text, but not data response text
  -------------------------------------------------------------------------*/
  if ( dsat_q_val == 0 || ( is_result_code == FALSE) )
  {
    while ( (c = *src_ptr++) != 0)
    {
      if (c != '\n')
      {
        *dest_ptr++ = c;
        ++(*l_ptr);
        ASSERT (
                  ( dest_ptr - ((*out_ptr_ptr)->data_ptr) )
                < ( (*out_ptr_ptr)->size                  )
               );

      }

      if (c == '\n' || *l_ptr == 0x7E)
      {
        *dest_ptr++ = EIA617_EM;
        *dest_ptr++ = EIA617_EXTEND1_M2B;
        l_ptr = dest_ptr++;
        *l_ptr = EIA617_LEN_OFFSET + 1; /*Initial length includes sub-code */
        *dest_ptr++ = EIA617_RESP_2B;
      }
    }
  }

  /*-------------------------------------------------------------------------
    Update the number of bytes used in output buffer.
  -------------------------------------------------------------------------*/
  (*out_ptr_ptr)->used +=  (word)
                           (dest_ptr -
                          &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used]);

  dsm_free_packet( &in_ptr );

} /* ds707_async_at_rsp_fmting_func_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_AT_RSP_ROUTING_FUNC_HDLR

DESCRIPTION   Handler registered with atcop to be called when an AT response
              needs to be sent in online mode. This function does the
              enqueuing to Um.

DEPENDENCIES  This should only be called while in online data mode, when
              AT cmds are reflected to IWF.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_at_rsp_routing_func_hdlr
(
  dsm_item_type *rsp_ptr,              /* Final response to be sent        */
  const dsat_mode_enum_type at_state,  /* Mode (cmd/online_data/online_cmd */
  const boolean rearm_autodetect       /* flag to rearm autodetect         */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_at_rsp_routing_func_hdlr()", 0, 0, 0);

  if ( at_state == DSAT_ONLINE_DATA )
  {
    rsp_ptr->kind = DSM_PS_APP_PAYLOAD;
    ds707_async_ps_tcp_enqueue ( &rsp_ptr );
  }
  else
  {
    ds3g_siolib_setup_tx ( rsp_ptr, rearm_autodetect );
  }

} /* ds707_async_at_rsp_routing_func_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_ATZ_HDLR

DESCRIPTION   Handler registered with atcop to be called when an ATZ command
              comes in. Sends cmd to have atz handling be done in DS task.

DEPENDENCIES  This should only be called while in online data mode, when
              AT cmds are reflected to IWF.

RETURN VALUE  None.

SIDE EFFECTS  Results in call being hung up.
===========================================================================*/
void ds707_async_atz_hdlr
(
  void
)
{
  ds_cmd_type      *cmd_ptr;                     /* DS Task msg            */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (cmd_ptr = ds_get_cmd_buf()) == NULL )
  {
    ERR_FATAL( "DS707 can't get cmd buf from DS task", 0, 0, 0 );
  }
  else
  {
    cmd_ptr->hdr.cmd_id         = DS_707_ASYNC_ATZ_CMD;
    ds_put_cmd(cmd_ptr);
  }

} /* ds707_async_atz_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_ATZ_CMD_HDLR

DESCRIPTION   Called in DS task context in response to an ATZ cmd called
              from atcop.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Hangs up call.
===========================================================================*/
void ds707_async_atz_cmd_hdlr
(
  void
)
{
  dsat_num_item_type retained_ipr_val;    /* temp var to hold dsat_ipr_val */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_atz_cmd_hdlr()", 0, 0, 0);


  /*-------------------------------------------------------------------------
    Data Port's Baud Rate should not be changed in ATZ command. Retain the
    IPR value and restore it later for ATZ command.
  -------------------------------------------------------------------------*/
  retained_ipr_val = dsat_ipr_val;

  /*-------------------------------------------------------------------------
    Initialize all AT parameters to their default values.
  -------------------------------------------------------------------------*/
  dsatcmdp_init_config ();

  dsat_ipr_val = retained_ipr_val;

  /*-------------------------------------------------------------------------
    Set the diag port with the new/reset baud rate.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DIAG_DS_SIO_COUPLING
  diag_record_dm_baudrate ( (sio_bitrate_type) dsat_qcdmr_val );
#endif /* FEATURE_DIAG_DS_SIO_COUPLING */

  /*-------------------------------------------------------------------------
    Hangup call.
  -------------------------------------------------------------------------*/
  ds3g_msh_hangup_call ( ds707_async_state.tc_state.call_id, NULL );

} /* ds707_async_atz_cmd_hdlr() */


/*===========================================================================
                          ENQUEUE DATA
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_PS_TCP_ENQUEUE

DESCRIPTION   This function adds the passed dsm item to the tcp_send_q.
              It can be called at ISR level by the serial driver to send
              serial data or at the DS task level by atcop to send 617
              reflected commands and results.

DEPENDENCIES  The value of the passed item ptr becomes NULL after this.

RETURN VALUE  None.

SIDE EFFECTS  Changes ps707_async_tcp_send_q and ps707_async_tcp_send_q_cnt.
===========================================================================*/
void ds707_async_ps_tcp_enqueue
(
  dsm_item_type **item_ptr  /* Item containing data to be sent over Um TCP */
)
{
  word             initial_cnt =0;      /* initial byte count of the queue */
  dword            isave;              /* Saves PSW during interrupt locks */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   if ( (*item_ptr) == NULL )
   {
     ERR_FATAL("DSM item ptr null", 0, 0, 0);
   }

  /*-------------------------------------------------------------------------
    Compare the current count of the tcp_send_q and the data that
    TCP has not yet sent. If there is no space, discard the item.
    Note that serial data can expand in the TCP sendq because of escaping.
  -------------------------------------------------------------------------*/
  if(
     ( ps707_async_state.tcp_send_q_cnt + Tcb.sndcnt
       - (Tcb.snd.nxt-Tcb.snd.una) )
     > 8*PS707_ASYNC_DEF_MSS
    )
  {
    MSG_MED (" Tcp sendq full, discarding packet", 0 , 0, 0);
    dsm_free_packet ( item_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    Now, do buffer based flow control. If we are running out of buffers then
    start dropping them.
  -------------------------------------------------------------------------*/
  if ((DSM_DONT_EXCEED_ITEMS(DSM_DS_SMALL_ITEM_POOL) == TRUE) ||
      (DSM_DONT_EXCEED_ITEMS(DSM_DS_LARGE_ITEM_POOL) == TRUE))
  {
    ERR(" Running out of buffers - discarding serial pkt.", 0 , 0, 0);
    dsm_free_packet ( item_ptr);
    return;
  }

#ifdef FEATURE_DSM_MEM_CHK
  (*item_ptr)->tracer = DSM_TCP_SNDQ;
#endif /* FEATURE_DSM_MEM_CHK */

  /*-------------------------------------------------------------------------
    Increment the current queue count and put the item at the tail of queue.
  -------------------------------------------------------------------------*/
  INTLOCK_SAV( isave);
  initial_cnt = ps707_async_state.tcp_send_q_cnt;
  q_put( &ps707_async_state.tcp_send_q, &(*item_ptr)->link);
  ps707_async_state.tcp_send_q_cnt += (*item_ptr)->used;
  INTFREE_SAV( isave);

  /*-------------------------------------------------------------------------
    If queue is empty, signal the PS task to dequeue item.
  -------------------------------------------------------------------------*/
  if ( initial_cnt == 0)
  {
    PS_SET_SIGNAL ( PS_707_ASYNC_PROCESS_SEND_Q_SIGNAL );
  }

  /*-------------------------------------------------------------------------
    Compare the current count of the tcp_send_q and the data that
    TCP has not yet sent, to a specified amount. If the former exceeds the
    latter, deassert CTS.
  -------------------------------------------------------------------------*/
  if(
        (ps707_async_state.tcp_send_q_flow_enable == TRUE)
     && (  (ps707_async_state.tcp_send_q_cnt+
           Tcb.sndcnt-(Tcb.snd.nxt-Tcb.snd.una))
          > 5*PS707_ASYNC_DEF_MSS
        )
    )
  {
    ds3g_siolib_set_inbound_flow ( DS_FLOW_707_CKT_TCP_MASK, DS_FLOW_DISABLE );
    ps707_async_state.tcp_send_q_flow_enable = FALSE;
  }

  *item_ptr = NULL;

} /* ds707_async_ps_tcp_enqueue() */



/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_ASYNC_PROCESS_CMD

DESCRIPTION   Called by 3G DSMGR, to handle commands specific to this
              mode handler.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Can result in various actions, such as call bring up/tear
              down, etc.
===========================================================================*/
void ds707_async_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(ds_cmd_ptr->hdr.cmd_id)
  {
    case DS_707_ASYNC_IFACE_BRING_UP_CMD:
      ds707_async_iface_bring_up_cmd_hdlr();
      break;

    case DS_707_ASYNC_IFACE_TEAR_DOWN_CMD:
      ds707_async_iface_tear_down_cmd_hdlr();
      break;

    case DS_707_ASYNC_PHYS_LINK_TEAR_DOWN_CMD:
      ds707_async_phys_link_tear_down_cmd_hdlr();
      break;

    case DS_707_ASYNC_PTCL_OPENING_TIMER_EXPIRED_CMD:
      ds707_async_ptcl_opening_timer_expired_cmd_hdlr();
      break;

    case DS_707_ASYNC_PTCL_OPENED_CMD:
      ds707_async_ptcl_opened_cmd_hdlr();
      break;

    case DS_707_ASYNC_PTCL_CLOSED_CMD:
      ds707_async_ptcl_closed_cmd_hdlr();
      break;

    case DS_707_ASYNC_ATZ_CMD:
      ds707_async_atz_cmd_hdlr();
      break;

    default:
      break;
  }
} /* ds707_async_process_cmd() */


/*===========================================================================
FUNCTION      DS707_ASYNC_PROCESS_SIGNALS

DESCRIPTION   Processes sigals set by other parts of the system.  Currently,
              no signals should be set for this mode handler.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_process_signals
(
  rex_sigs_type   sigs
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(0);
  ERR("No signals for 707 ASYNC MODE HDLR %d",sigs,0,0);

} /* ds707_async_process_signals */


/*===========================================================================
FUNCTION      DS707_ASYNC_PTCL_OPENING_TIMER_EXPIRED_CMD_HDLR

DESCRIPTION   Called when timer expires for protocols to open (PPP + TCP
              conn establishment) after physical channel is established.
              Tear down call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_ptcl_opening_timer_expired_cmd_hdlr
(
  void
)
{
  ds3gi_call_state_e_type cstate3g;               /* Call state of 3GDSMGR */
  ps_iface_type       *iface_ptr;                    /* ptr to async iface */
  int16               tear_down_errno;  /* errno set by ps_iface_tear_down */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_HIGH ("ds707_async_ptcl_opening_timer_expired_cmd_hdlr()", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Set protocol state to closed and do related processing.
  -------------------------------------------------------------------------*/
  MSG_LOW ( "Changing protocol state from %d to %d",
            ds707_async_state.protocol_state,
            DS707_ASYNC_PROTOCOL_CLOSED,
            0);
  ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_CLOSED;

  cstate3g = ds3g_get_call_state(ds707_async_state.tc_state.call_id);
  if (
         ( cstate3g != DS3G_INVALID_CALL_STATE )
      && (
             ( cstate3g == DS3G_ORIG_CALL_STATE )
          || ( cstate3g == DS3G_CALL_ACTIVE_CALL_STATE )
         )
     )
  {
    iface_ptr = &(ds707_async_state.ps_iface);
    ps_iface_tear_down_cmd ( iface_ptr, &tear_down_errno, NULL );
  }
} /* ds707_async_ptcl_opening_timer_expired_cmd_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_PTCL_OPENED_CMD_HDLR

DESCRIPTION   Called when DS gets cmd from PS saying protocol stack opened,
              ie TCP connection with IWF successfully established.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_ptcl_opened_cmd_hdlr
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Protocols now Open.
  -------------------------------------------------------------------------*/
  MSG_LOW ( "ds707_async_ptcl_opened_cmd_hdlr()", 0, 0, 0);

  if ( ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPENING )
  {
    MSG_LOW( "Changing async ptcl state from %d to %d",
              ds707_async_state.protocol_state,
              (int) DS707_ASYNC_PROTOCOL_OPEN,
              0);
    ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_OPEN;

    /*-----------------------------------------------------------------------
      Cancelling the timer which was started when dsmgr asked psmgr
      to open the protocols.
    -----------------------------------------------------------------------*/
    ds3gi_stop_timer ( DS3G_TIMER_ASYNC_PTCL_OPENING );

    /*-----------------------------------------------------------------------
      Upload AT config to IWF.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_1X
    dsat707_send_config ( ds707_async_dial_info.jcdma_dial_str_to_iwf );
#else
    dsat707_send_config ( ds707_async_dial_info.dial_str );
#endif

    ds3g_siolib_set_inbound_flow
      ( (DS_FLOW_707_CKT_MASK | DS_FLOW_707_CKT_TCP_MASK), DS_FLOW_ENABLE );
  }
  else
  {
    ERR("Can't receive open if ds707_async_state not opening", 0, 0, 0);
  }

} /* ds707_async_ptcl_opened_cmd_hdlr() */


/*===========================================================================
FUNCTION      DS707_ASYNC_PTCL_CLOSED_CMD_HDLR

DESCRIPTION   Called when DS gets cmd from PS saying protocol stack closed,
              ie TCP connection with IWF disconnected.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_ptcl_closed_cmd_hdlr
(
  void
)
{
  ds3gi_call_state_e_type cstate3g;               /* Call state of 3GDSMGR */
  ps_iface_type       *iface_ptr;                    /* ptr to async iface */
  ps_iface_type       *pkt_iface_ptr;                /* ptr to pkt iface   */
  int16               tear_down_errno;  /* errno set by ps_iface_tear_down */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ( "ds707_async_ptcl_closed_cmd_hdlr()", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Cancelling the state timer which was started when dsmgr asked
    psmgr to open the protocols.
  -------------------------------------------------------------------------*/
  if ( ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPENING )
  {
    ds3gi_stop_timer ( DS3G_TIMER_ASYNC_PTCL_OPENING );
  }

  /*-------------------------------------------------------------------------
    If protocol state was OPEN, when dsmgr got a PTCL_CLOSED_CMD,
    it means that the IWF brought down the protocols.
  -------------------------------------------------------------------------*/
  if ( ds707_async_state.protocol_state == DS707_ASYNC_PROTOCOL_OPEN )
  {
    ds707_async_state.ps_closed_properly = TRUE;
  }

  /*-------------------------------------------------------------------------
    Set protocol state to closed and do related processing.
  -------------------------------------------------------------------------*/
  MSG_LOW( "Changing protocol state from %d to %d",
            ds707_async_state.protocol_state,
            (int) DS707_ASYNC_PROTOCOL_CLOSED,
            0);
  ds707_async_state.protocol_state = DS707_ASYNC_PROTOCOL_CLOSED;


  cstate3g = ds3g_get_call_state(ds707_async_state.tc_state.call_id);
  if (
         ( cstate3g != DS3G_INVALID_CALL_STATE )
      && (
             ( cstate3g == DS3G_ORIG_CALL_STATE )
          || ( cstate3g == DS3G_CALL_ACTIVE_CALL_STATE )
         )
     )
  {
    iface_ptr = &(ds707_async_state.ps_iface);
    ps_iface_tear_down_cmd ( iface_ptr, &tear_down_errno, NULL );

  }
  else
  {
    MSG_HIGH ( "Sending ps_iface_down_ind", 0, 0, 0 );
    ps_iface_down_ind ( &(ds707_async_state.ps_iface) );
    /*-----------------------------------------------------------------------
      Enable 1x pkt iface.
    -----------------------------------------------------------------------*/
    pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
    ps_iface_enable_ind ( pkt_iface_ptr );
  }

} /* ds707_async_ptcl_closed_cmd_hdlr() */



/*===========================================================================
FUNCTION      DS707_ASYNC_SETUP_RLP

DESCRIPTION   Sets up RLP with a TX watermark and RLP RX func and RLP RX
              queue.  If the inputs are NULL, then use defaults as
              defined by this module.  Else use the ones passed into this
              function.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_setup_rlp
(
  dsm_watermark_type  *tx_watermark_ptr,         /* Tx wm ptr for txmit frs*/
  void                (*post_rx_func_ptr)(void), /* rx cb    for rx frames */
  q_type              *post_rx_q_ptr             /* queue for rx'ed frames */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ((tx_watermark_ptr == NULL) &&
      (post_rx_func_ptr == NULL) &&
      (post_rx_q_ptr    == NULL)
     )
  {
    ds707_asynci_set_rlp_defaults();
  }
  else
  {
    ds707_async_state.tc_state.tx_watermark_ptr = tx_watermark_ptr;
    ds707_async_state.tc_state.post_rx_func_ptr = post_rx_func_ptr;
    ds707_async_state.tc_state.post_rx_q_ptr    = post_rx_q_ptr;
  }
} /* ds707_async_setup_rlp() */


/*===========================================================================
FUNCTION      DS707_ASYNC_POST_RLP_RX_FUNC()

DESCRIPTION   This function performs the RLP post-RX processing function
              for the IS99 service. The PPP RX signal is set which indicates
              to the PS task there are RLP data frame(s) ready for PPP
              processing.

              This function runs in the context of the RX task.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_post_rlp_rx_func
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PS_SET_SIGNAL(PS_PPP_UM_SN_RX_SIGNAL);

} /* ds707_async_post_rlp_rx_func() */

/*===========================================================================
FUNCTION      DS707_ASYNC_RX_UM_SN_DATA

DESCRIPTION   Called by PPP to get forward-link data from RLP.

DEPENDENCIES  None.

RETURN VALUE  A pointer to a DSM item.  DSM item may be a packet chain.
              If there is no data to return, then returns NULL.

SIDE EFFECTS  None.
===========================================================================*/
dsm_item_type* ds707_async_rx_um_SN_data
(
  void *user_data
)
{
  ds707_async_tc_state_type * tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tc_state_ptr = &(ds707_async_state.tc_state);
  return ((dsm_item_type *) q_get ( tc_state_ptr->post_rx_q_ptr ));

} /* ds707_async_rx_um_SN_data() */

/*===========================================================================
FUNCTION      DS707_ASYNC_TX_UM_SN_DATA

DESCRIPTION   Called by PPP to transmit rev-link data over RLP to the base
              station.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_tx_um_SN_data
(
  dsm_item_type   **dsm_item_ptr_ptr,
  void *user_data
)
{
  ds707_async_tc_state_type *tc_state_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tc_state_ptr = &(ds707_async_state.tc_state);
  dsm_enqueue(tc_state_ptr->tx_watermark_ptr, dsm_item_ptr_ptr);

} /* ds707_async_tx_um_SN_data() */

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */
