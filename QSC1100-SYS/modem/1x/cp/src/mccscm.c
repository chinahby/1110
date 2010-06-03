/*******************************************************************
*
*    Supplemental Channel Assignment Message Processing Routine
*
* GENERAL DESCRIPTION
*  This module contains functions to process SCAM messages
* received from a base station.  It also includes functions to
* process the data pattern testing information.  The SCAM processing is
* currently only for version 0.1 of MDR, and the DPT information is
* usable for versions 0.1 or 0.2 of MDR, and possibly also 1.0.
*
* Copyright (c) 1998-2005 by QUALCOMM Incorporated.  All Rights Reserved.
*
*      EDIT HISTORY FOR MODULE
*  This section contains comments describing changes made to the module.
* Notice that changes are listed in reverse chronlogical order.
*
* $PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccscm.c_v   1.5   01 Oct 2002 16:53:58   azafer  $
* $Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccscm.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
*
* when       who     what, where, why
* --------   ---     ----------------------------------------------------------
* 04/24/06   an      Fixed lint errors
* 10/10/05   fh      Removed including of dec.h. changed DEC_ constants to be
                     CAI_ constants      
* 07/11/05   fh      Cleaned up header file including from MUX:
*                      Changed rxc.h to rxcmc.h
* 08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
* 10/01/02   az      Mainlined FEATURE_IS2000.
* 06/14/01   va      Changed send_rej_order to match the new prototype.
* 05/17/01   lh      Merged the following from common archive:
*   04/10/01   lcc     Changes to conform to new prototype for snm_get_current_config.
*   01/08/01   jq      Made some naming changes to prepare for IS2000_P2.
*   12/19/00   jq      Fixed a problem with sup pilot checking for SCAM.
* 02/27/01   kk      Added new SO group and bitmap initialization.
* 09/20/00   jq      Cleaned all lint errors.
* 07/10/00   cah     Rid msg_file compiler warning for non 95B & Special MDR
*                    builds.
* 06/19/00   yll     Added MSM3300 support.
* 03/28/00   ry      Fixed a compiler error introduced from the previous merge
* 03/28/00   ry      Featurized initialization values of neg_par_so8025 based
                     on the MSM supported
* 03/13/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
*            mk      Added MSM3100 support.
* 02/16/00   lcc     Minor comment changes.
* 10/20/99   lcc     1. Corrected mux options for DPT service options.
                     2. Added MSM5000/IS2000 MDR support.
* 08/17/99   ram     More code review changes: corrected paranthesis usage,
                     used ASSERT to check for possible corrupt values,
                     Comments to indicate ends of functions.
* 08/02/99   ram     Removed the checking of return values in the
                     mccccl_code_channel_list.update_sups function since
                     the prototype was changed to a void.
                     Made the following changes from code review:
                     1)Changed ERR to MSG_HIGH for base station errors.
                     2)Used ASSERT for situations that should not
                     happen normaly except during testing.
                     3)Cleaned up error handling in scam processing
                     functions so that whenever possible errors are
                     handled within the lower functions and are
                     not cascaded to the calling functions.
                     4)Added some comments as needed.
                     In mccscm_scam_process rejected SCAM if the Pilot
                     PN's are not in the active set and it's not linked
                     to a GHDM. This is specified by IS95B.
* 07/26/99   jq      Fixed a few minor errors due to codereview. Changed
*                    mccscm_scam_timer_chk to mccscm_scam_timer_set
* 07/22/99   jq      Fixed a misplaced #ifdef for SCAM-GHDM linking
* 07/22/99   jq      Added SCAM-GHDM linking feature.
* 05/19/99   doj     Changed references to GHDM structure (gen_ho) to generic
                     handoff structure (ho).
* 04/21/99   ram     Added checking for reverse supplemental parameters
                     in mccscm_mdr_pre_process. This was commented out
                     earlier for internal testing.
                     In SCAM, checking for rev_parms_inc is now done only
                     if rev_included is TRUE.
* 04/06/99   ram     Removed the RXC call to change the RXDSP clock
                     to 2/3 TCXO when an MDR call is up.
                     this is no longer needed because RXDSP clock is always
                     at 2/3 TCXO.
* 03/30/99   ram     Added function to check if a previous start timer is
                     active when a new SCAM is Rx'ed. If so, we ignore the
                     second SCAM. Renamed mccscm_scam_process to mccscm_
                     scam_sup_process since it only processes the sup
                     channel info. Introduced a wrapper function called
                     mccscm_scam_process which does all the processing for
                     SCAM message. Updated based on code review comments.
* 03/15/99   ram     Added function to cancel previous duration timer when
                     in gets superceeded by a new SCAM/GHDM. Code review
                     changes, fix to mdr_pre_process function to check
                     number of supplementals only if code channels are specified
                     in the message. Parameter for mccscm_scam_process is
                     now caii_scam_type. scam_timer_msg is also caii_scam_type.
* 03/09/99   ram     Added function to process IS95B SCAM/GHDM duration timer.
                     Added function to process an request to stop transmitting
                     on supplementals. Included lint fixes given by nmn.
* 02/19/99   ram     Added function to process IS95B scam start timer.
                     Increased error checking for SCAM message. Lint
                     and code-review cleanup.
* 02/11/99   ram     Created mdr_pre_process to pre-process the MDR related
                     fields of SCAM and GHDM. Created scam_process function
                     to process SCAM message. Featurized code under
                     IS95B_MDR and SPECIAL_MDR.
* 01/19/99   lcc     Moved FEATURE_IS95B_MDR below #includes for header files.
                     Now it builds correctly with and without FEATURE_IS95B_MDR.
* 01/19/99   ram     Moved FEATURE_IS95B_MDR defines so that the file
*                    builds correctly when FEATURE_IS95B is turned off.
* 01/05/98   nmn     reincluded entire file when FEATURE_IS95B_MDR is defined.
* 01/05/98   nmn     unincludes entire file with FEATURE_IS95B_MDR undef'd
* 12/23/98   lcc     Added support for RS2 DPT frames.  Also corrected several
*                    DPT related problems.
* 12/22/98   nmn     improved output and fixed interpretation of CCHID
* 12/16/98   nmn     surrounded all DPT code with #defines, fixed & tested
*                    MDR_DPT_PRINT
* 12/11/98   nmn     Linted for 0.2, improved MDR_DPT_PRINT created, not
*                    implemented
* 12/08/98   lcc     Featurized DPT with FEATURE_MDR_DPT_MARKOV.
* 12/02/98   lcc     Added support for official DPT service options (mostly from
*                    Ram).
* 12/01/98   ram     Added code for MDR DPT service options initialization
* 11/23/98   nmn     DPT works sucessfully with two channels
* 11/19/98   nmn     added functionality for DPT using byte-length data
* 10/15/98   nmn     added separate functions to perform different parts
*                    of SCAM processing
* 08/27/98   nmn     finished 0.1 for integration (major SRCH changes, removed
*                    using_channels)
* 07/19/98   nmn     finished UT (including some changes, added using_channels)
* 07/17/98   nmn     began UT
* 07/16/98   nmn     finished turning comments into ERR and MSG output, ready
*                    for UT
* 07/13/98   nmn     linted file
* 07/10/98   nmn     changed global variables into those defined in cai.h
* 07/09/98   nmn     split file into separate .h file, turned magic numbers
*                    into variables, changed spacing, created more comments,
*                    added error and message calls
* 07/06/98   nmn     Initial document
*
*
*
*******************************************************************/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "mccscm.h"
#include "cai.h"
#include "caii.h"
#if (!(defined (FEATURE_IS95B_MDR)))
#undef MSG_FILE
#endif /* (!(defined (FEATURE_IS95B_MDR))) */
#include "err.h"
#include "assert.h"
#include "mccccl.h"
#include "mccdma.h"
#include "mccrxtx.h"
#include "mccsrch.h"
#include "msg.h"
#include "msm.h"
#include "qw.h"
#include "rex.h"
#include "rxcmc.h"
#include "snm.h"
#include "ts.h"
#ifdef FEATURE_IS95B_MDR
#include "mcctcsup.h"
#endif

#ifdef FEATURE_IS95B_MDR
/* Shared data between GHDM and SCAM, necessary for linking */
mccscm_link_type mccscm_link_data;
#endif /* FEATURE_IS95B_MDR */

#if defined (FEATURE_IS95B_MDR)

/* Local MDR info used for both 95B MDR and special P_REV 3+ MDR */

/*******************************************************************
*
*                               LOCAL DATA
*
*******************************************************************/

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif // FEATURE_MDR_DPT_MARKOV


/* Define a local structure with the MDR Fields that we need to use */
typedef struct
{
  /* Forward Supplemental Channel Parameters */
  boolean for_included;        /* FWD supp channel config included indicator */
  byte for_sup_config;         /* FWD supp channel config indicator          */
  boolean use_for_dur;         /* Use forward duration indicator             */
  byte for_dur;                /* Forward duration                           */
  byte num_for_sup;            /* Number of FWD supp code channel            */

  /* To check if Reverse channel info is included */
  boolean rev_included;        /* Reverse Supplemental Code Channel config   */
  boolean rev_parms_inc;       /* Reverse additional parameters included     */

  /* Reverse Supplemental Channel Parameters */

#ifdef FEATURE_IS95B_MDR_REV

  boolean use_retry_delay;     /* Assign or retry indicator                  */
  byte retry_delay;            /* SCRM retry delay                           */
  byte rev_dtx_duration;       /* Reverse Discontinuous Transmission Duration*/
  boolean expl_rev_start_tm;   /* Explicit reverse supp code chan assign
                                  start time                                 */
  byte rev_start_tm;           /* Explicit start time                        */
  boolean use_rev_dur;         /* Use reverse duration indicator             */
  byte rev_dur;                /* Reverse duration                           */
  boolean use_rev_hdm_seq;     /* Use reverse GHDM sequence # indicator      */
  byte rev_hdm_seq;            /* Sequence # of the reverse linked GHDM      */
  byte num_rev_codes;          /* Number of reverse sup code channels        */
  boolean use_t_add_abt;       /* Reverse use T_ADD abort indicator          */
  boolean use_scrm_seq;        /* Use SCRM sequence # indicator              */
  byte scrm_seq_num;           /* SCRM sequence number                       */
  byte t_mulchan;              /* SCRM pilot strength reporting offset       */
  byte begin_pre;              /* Number of preamble frames                  */
  byte resume_pre;             /* Number of preamble at resumption of xmit   */

#endif /* FEATURE_IS95B_MDR_REV */

}mccscm_mdr_parms_type;


#ifdef FEATURE_IS95B_MDR
caii_scam_type scam_timer_msg; /* To maintain a copy of the message, in case
                                 the message has a start time associated with it */
#endif /* FEATURE_IS95B_MDR */

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif // FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif // FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV

#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif //FEATURE_MDR_DPT_MARKOV


#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif // FEATURE_MDR_DPT_MARKOV


/*===========================================================================

FUNCTION MCCSCM_MDR_PRE_PROCESS

DESCRIPTION
  This function pre-processes an MDR supplemental channel message
  (either SCAM or GHDM) and makes sure all the relevant fields are
  within expected range. If not, the message may have to be rejected
  and if so, this function will return the reject code to the calling
  function.  It also checks if an MDR service option connection
  has been established and the negotiated mux options allow the
  number of supplementals assigned by the message.

DEPENDENCIES
  FEATURE_IS95B_MDR must be turned on.
  The function should be called before the GHDM action time
  processing and before the SCAM start/stop time processing.

RETURN VALUE
  A value of type mccscm_stat_enum_type to indicate sucess or reason
  of failiure. The reason of failiure may be a reject reason. If so,
  the message has to be rejected.

SIDE EFFECTS
  None.

===========================================================================*/
mccscm_stat_enum_type mccscm_mdr_pre_process
(
  caii_rx_msg_type *msg_ptr /* Pointer to the GHDM or SCAM message */
)
{
  const caii_srv_cfg_type *mdr_srv;/* To store the current srvc cfg*/
  mccscm_mdr_parms_type mdr; /* Hold params for preprocessing      */

  /* We need to check if the message is a SCAM or a GHDM and then
  copy the necessary fields into our local buffer */

  if( msg_ptr->ho.msg_type == CAI_GEN_HO_DIR_MSG )
  {
    /* If it's a GHDM, use the GHDM msg format */

    mdr.for_included = msg_ptr->ho.sup.for_included;
    mdr.rev_included = msg_ptr->ho.sup.rev_included;
    mdr.for_sup_config = msg_ptr->ho.sup.for_sup_config;
    mdr.use_for_dur = msg_ptr->ho.sup.use_for_dur;
    mdr.for_dur = msg_ptr->ho.sup.for_dur;
    mdr.num_for_sup = msg_ptr->ho.sup.num_for_sup;
    mdr.rev_parms_inc = msg_ptr->ho.sup.rev_parms_inc;
  }

#ifdef FEATURE_IS95B_MDR
  else if( msg_ptr->scam.fix.msg_type == CAI_SUP_CHN_ASN_MSG)
  {
    /* If it's a SCAM, use the SCAM msg format */

    mdr.for_included = msg_ptr->scam.fix.for_inc;
    mdr.rev_included = msg_ptr->scam.fix.rev_included;
    mdr.for_sup_config = msg_ptr->scam.fix.for_sup_config;
    mdr.use_for_dur = msg_ptr->scam.fix.use_for_dur;
    mdr.for_dur = msg_ptr->scam.fix.for_dur;
    mdr.num_for_sup = msg_ptr->scam.fix.num_sup;
    if( mdr.rev_included  != FALSE )
    {
      /* For SCAM, rev_parms_inc is not included unless
      rev_included is TRUE */
      mdr.rev_parms_inc = msg_ptr->scam.fix.rev_parms_inc;
    }
    else
    {
      /* The field was not included so zero it out */
      mdr.rev_parms_inc = FALSE;
    }
  }
#endif /* FEATURE_IS95B_MDR */

  else
  {
    //This should never happen but we don't have to error fatal.
    //The mobile may be able to recover from this
    ERR("Sup chan processing called by msg not SCAM or GHDM",0,0,0);
    return( MDR_UNKNOWN_S ); /* Unknown message */
  }

  /* Look for incorrect values in the fields */
  if(( mdr.rev_included != FALSE ) || (mdr.rev_parms_inc != FALSE))
  {
    /* Reverse information is included, we have to reject
    the message since we don't support reverse channels */
    MSG_HIGH( "Unsupported request: Reverse sups", 0, 0, 0);
    return( MDR_REJ_CAP_S );
  }

  if( mdr.for_included != 1 )
  {
    /* Forward supplementals are not included, nothing
    to do here - indicate that and return */
    MSG_HIGH( "No FWD SUP info in msg", 0, 0, 0);
    return( MDR_DONE_S );
  }
  if((mdr.for_sup_config == CAI_FOR_SUP_SPEC_STOP) ||
     (mdr.for_sup_config == CAI_FOR_SUP_SPEC_START) )
  {
    /* Only if code channel list needs to be updated,
    the following tests will be done */
    if(mdr.num_for_sup > CAI_MAX_NUM_SUP)
    {
      /* Number of supplementals given is invalid */
      MSG_HIGH("Invalid # of fwd. sups", 0, 0, 0);
      return( MDR_REJ_CAP_S );
    }

    mdr_srv = snm_get_current_config();

    /* Check if the number of supplelementals is not more than
    the maximum allowed by the negotiated mux options */
    if(
       (mdr_srv->con[0].so == CAI_SO_MDR_PKT_DATA_FRS1_RRS1)
#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif /* FEATURE_MDR_DPT_MARKOV */
      )
    {
      /* If it is a MDR rate set 1 call (Packet data or DPT)
      max number of sups = (n-1)/2, where n is the max mux option */
      if( mdr.num_for_sup > ((mdr_srv->fwd_mux - 1)/2))
      {
        MSG_HIGH("Too many sups assigned by the message", 0, 0, 0);
        return( MDR_REJ_CAP_S );
      }
    }
    else if(
       (mdr_srv->con[0].so == CAI_SO_MDR_PKT_DATA_FRS2_RRS2)
#ifdef FEATURE_MDR_DPT_MARKOV
#error code not present
#endif /* FEATURE_MDR_DPT_MARKOV */
           )
    {
      /* If it is a MDR rate set 2 call (Packet data or DPT)
      max number of sups = (n-2)/2, where n is the max mux option */
      if( mdr.num_for_sup > ((mdr_srv->fwd_mux - 2)/2))
      {
        MSG_HIGH("Too many sups assigned by the message", 0, 0, 0);
        return( MDR_REJ_CAP_S );
      }
    }
    else
    {
      MSG_HIGH("MDR msg rx'ed when Non-MDR SO connected", 0, 0, 0);
      return( MDR_REJ_CAP_S );
    }/* end if mdr_srv->con[0].so == ... */

  }/* end if ((mdr.for_sup_config == CAI_FOR_SUP_SPEC_STOP) ||
     (mdr.for_sup_config == CAI_FOR_SUP_SPEC_START) ) */

  return( MDR_DONE_S );
  
}/* end function mccscm_mdr_pre_process */  /*lint !e818 */

#ifdef FEATURE_IS95B_MDR
/*===========================================================================

FUNCTION MCCSCM_CHK_PREV_START_TIMER

DESCRIPTION
  This function checks if a previously set start timer
  or the corrosponding signal is active.

DEPENDENCIES
  FEATURE_IS95B_MDR must be turned on,
  and a MDR call must be in progress. This function assumes that
  the calling function is a SCAM processing function
  for MDR forward supplementals.

RETURN VALUE
  TRUE if there is a previously set timer still active,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccscm_chk_prev_start_timer( void )
{
  /* First check for the previous timer */
  if( rex_get_timer( &mcc_scam_start_timer ))
  {
    return( TRUE ); /* There is an active timer */
  }
  /* Now also check for the signal. This is done to prevent
  any race conditions arising from the timer already having
  expired before we get to this point */
  if( ((rex_get_sigs(&mc_tcb)) & MCC_SCAM_TIMER_SIG )
      == MCC_SCAM_TIMER_SIG )
  {
    return( TRUE );
  }
  return( FALSE );
}
#endif /* FEATURE_IS95B_MDR */

/*===========================================================================

FUNCTION MCCSCM_CANCEL_PREV_DUR_TIMER

DESCRIPTION
  This function cancels a previously set duration timer and
  the corrosponding signal for a SCAM or GHDM processing message.
  This function needs to be called to clear a duration timer
  set by a previous SCAM or a GHDM message whenever a new SCAM
  or GHDM superseeds the old message's duration time.

DEPENDENCIES
  FEATURE_IS95B_MDR must be turned on,
  and a MDR call must be in progress. This function assumes that
  the calling function is either a SCAM or GHDM processing function
  for MDR forward supplementals.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccscm_cancel_prev_dur_timer( void )
{
  /* First cancel the previous timer. We don't have to
  check if there was a previous timer since the rex_clr_timer
  call does not do anything in that case */
  ( void )rex_clr_timer( &mcc_mdr_duration_timer );

  /* Now also cancel the signal. This is done to prevent
  any race conditions arising from the timer already having
  expired before we get to this point */
  ( void )rex_clr_sigs( &mc_tcb, MCC_MDR_DURATION_TIMER_SIG );
}

#ifdef FEATURE_IS95B_MDR

/*===========================================================================

FUNCTION MCCSCM_SCAM_TIMER_SET

DESCRIPTION
  This function starts a timer for the required time.

  The start time from the message is the actual system time at
  which the SCAM processing must start. This function uses the
  current system time and the system time given by the message to
  find out the actual time in milliseconds to wait before processing
  the rest of the message.

DEPENDENCIES
  FEATURE_IS95B_MDR must be turned on, and a MDR call must be in
  progress.

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void mccscm_scam_timer_set
(
  caii_scam_type *msg_ptr /* Pointer to the SCAM message */
)
{
  qword curr_time;
    /* Current time */
  word phase_adjust;
    /* 20ms offset into 80ms superframe */
  word scam_start_time;
    /* Start time given by the message */
  word temp_time;
    /* Current time in 80 msec units modulo 64 */
  word wait_time;
    /* Amount of time to set start time for */

  /* Get the start time from the message.
  This is the system time in 80 ms units
  Modulo 64 at which the mobile should
  start decoding supplementals ie - maximum size is 6 bits*/

  scam_start_time = msg_ptr->fix.for_start_tm;
                  /* This gives us start time in 80 ms units */

  /* Now, calculate the actual time to wait(in ms)
  by getting the current system time
  in 80 ms units and subtracting the
  start time from the current time */

  /* Get current time in 20 ms units */
  ts_get_20ms_frame_time( curr_time );

  /* Convert current time into 80 ms units
  Note that the qw_div function puts the reminder of the
  division in phase_adjust, and the quotient in curr_time */
  /* This means phase_adjust has the sub 80 ms chunk of system time */

  phase_adjust = qw_div( curr_time, curr_time, 4U );

  /*Now, get the modulo 64 value of curr_time. This is essentially
  the least significant 6 bits of the system time */
  temp_time = qw_div( curr_time, curr_time, 64U );


  /* Now, calculate wait time in milliseconds. This is the
  start time%64 minus the current system time%64 * 80 */

  if (temp_time < scam_start_time)
  {
    wait_time = ( word )((scam_start_time - temp_time) * 80);
    /* Note - total size won't be more than 13 bits
    since maximum size of (scam_start_time - temp_time) is 6 bits */
  }
  else
  {
    /* Compensate by adding 64, since start time has rolled over 1 bit */
    wait_time = ( word )((64 + scam_start_time - temp_time) * 80);
    /* Note - total size won't be more than 14 bits
    since maximum size of (scam_start_time - temp_time) is 6 bits */
  }

  /* Compensate for 20ms frame offset into 80ms superframe
  phase_adjust can be 0, 1, 2 or 3 depending on the offset */
  wait_time -= ( word )(phase_adjust * 20);
  /* Note - phase_adjust varies from 0 to 3, so maximum value evaluated
  by the expression is 8 bits in size */

  /* Finally, take 10 ms off the wait time so that Call Processing
  will have enough time to send the supplemental channel information
  to searcher. We want to compensate for interrupts, high-priority
  tasks etc that might have taken over, plus the delay in sending the
  info to searcher */
  if( wait_time > 10 )
  {
    wait_time -= 10;
  }

  /* Set the start time timer */
  (void) rex_set_timer( &mcc_scam_start_timer,  wait_time );

  /* Copy the message to a regional buffer, so that we will have
  a copy when the timer expires */
  scam_timer_msg = *msg_ptr;
  
}/* end function mccscm_scam_timer_set */  /*lint !e818 */

/*===========================================================================

FUNCTION MCCSCM_SCAM_SUP_PROCESS

DESCRIPTION
  This function processes the supplemental channel info given by SCAM
  message and takes the necessary actions.
  The actions include sending a command to SRCH task to start or stop
  supplemental channels and also updating the local code channel
  list information maintained by CP task if necessary. Duration timer
  is also started if necessary.

  Only Forward traffic channel supplementals are supported.

DEPENDENCIES
  FEATURE_IS95B_MDR must be turned on, and a MDR call must be in
  progress. The function should be called after the
  after the SCAM start time processing.

RETURN VALUE
  A value of type mccscm_stat_enum_type indicating success or
  failiure.

SIDE EFFECTS
  None.

===========================================================================*/
mccscm_stat_enum_type mccscm_scam_sup_process
(
  caii_scam_type *msg_ptr /* Pointer to the SCAM message */
)
{
  byte config=SUP_CONFIG_ERR;/* To hold the for_sup_config value   */
  byte num_of_pilots=0; /* To store the number of active pilots    */
  mccscm_link_type* lnk_ptr=&mccscm_link_data;// use ptr to safe some typing
  mccscm_timer_type for_start_time_mode = MCCSCM_NONVALID_TM;
       // type of start timer in use, init'd to a non-valid number

  /* Store the linking information shared by SCAM and GHDM */
  lnk_ptr->use_for_hdm_seq = msg_ptr->fix.use_for_hdm_seq;
  lnk_ptr->for_linked_hdm_seq = msg_ptr->fix.for_hdm_seq;

  /* we only need the last bit of sup_config */
  lnk_ptr->scam_for_order = msg_ptr->fix.for_sup_config & 1;
  lnk_ptr->scam_for_dur_mode = msg_ptr->fix.use_for_dur;
  lnk_ptr->for_dur = msg_ptr->fix.for_dur;


  /* Normally we should also be processing reverse supplemenals
  here, but they are also not supported in the initial release.
  so go to the forward supplementals section */

  /* Check if forward information is included in message.
  If so, for_sup_config will be included in the message */
  if( msg_ptr->fix.for_inc == 1 )
  {
    if (msg_ptr->fix.expl_for_start_tm)
    {
      for_start_time_mode = MCCSCM_EXPLICIT_TM;
    }
    else if (msg_ptr->fix.use_for_hdm_seq &&
             msg_ptr->fix.for_hdm_seq != cdma.hdm_seq)
    /* SCAM is linked to GHDM, but GHDM hasn't arrived */
    {
      for_start_time_mode = MCCSCM_NULL_TM;
    }
    else
    /* SCAM is linked to GHDM, and GHDM has arrived */
    /* or the start timer is implicit */
    {
      for_start_time_mode = MCCSCM_IMPLICIT_TM;
    }

    if (for_start_time_mode == MCCSCM_NULL_TM)
    {
      /* If SCAM is linked to GHDM, we don't want to update the supplemental
       * channel info, in case the linked GHDM doesn't arrive, so we don't
       * lose the current supplemental info. The update will be done when
       * GHDM arrives. */
      scam_timer_msg = *msg_ptr;
      MSG_MED("SCAM linked to GHDM rec'd and processed",0,0,0);

      /* if SCAM is linked to GHDM, we don't need to do anything else */
      return(MDR_DONE_S);
    }

    /* If for_sup_config is included in the message */
    /* note the for_sup_config value */
    config = msg_ptr->fix.for_sup_config;

    /* Check if the code channel list needs to be updated
    This is done only if the message contains new supplemental
    channel information */
    if( (config == CAI_FOR_SUP_SPEC_START) ||
        (config == CAI_FOR_SUP_SPEC_STOP)    )
    {
      /* Update the code channel list information
      by calling the appropriate CCL function */

      mccccl_code_channel_list.update_sups(
            (msg_ptr->fix.num_sup_plt), /* Number of pilots with sups */
            (msg_ptr->fix.num_sup),     /* Number of fwd sups         */
            (msg_ptr->var)              /* Pointer to code channel
                                        info for all pilots           */
                                          );
    }

    /* We should cancel any previous duration timer that may have been set
    since a new SCAM from the Base Station superceeds the duration
    timer of the old SCAM. If no duration timer is set, this function doesn't
    do anything */
    mccscm_cancel_prev_dur_timer();

    /* Now check if we need to stop the supplementals, or
    start the supplementals. Based on that we'll prepare
    the SRCH command with or without the supplementals */
    if( (config == CAI_FOR_SUP_STOP) ||
        (config == CAI_FOR_SUP_SPEC_STOP) )
    {
      /* We need to stop the supplementals */
      /* Prepare the supplemental channel part of the
      SRCH command using generate_aset_sups function. Indicate
      that we don't want the sups to be sent to SRCH */

      num_of_pilots =
      mccccl_code_channel_list.generate_aset_sups(
         mcc_srch_buf.tc_sup.sup_pilots,      /* Pointer to pilot info*/
         FALSE                                /* No SCCH needed    */
                                                 );


      /* Note that USE_FOR_DURATION cannot be set to TRUE
      if the FOR_SUP_CONFIG is 00 or 10 (stop supplementals)
      so we don't check if a duration timer is needed */

    }

    else if( (config == CAI_FOR_SUP_START) ||
             (config == CAI_FOR_SUP_SPEC_START) )
    {
      /* We need to start supplementals */
      /* Prepare the SRCH command accordingly */

      num_of_pilots =
      mccccl_code_channel_list.generate_aset_sups(
         mcc_srch_buf.tc_sup.sup_pilots,      /* Pointer to pilot info*/
         TRUE                                 /* SCCH needed    */
                                                 );
      /* Whenever we start supplementals, we have to check if we have
      a finite duration time. If so, we will set the duration timer
      right before sending the SRCH command. */

      if( msg_ptr->fix.use_for_dur )
      {
        /* The function requires to know if we have an implicit
        start time. Pass that information also */
        (void)mccscm_duration_timer_chk( msg_ptr->fix.for_dur,
             ( (for_start_time_mode==MCCSCM_IMPLICIT_TM) ? TRUE : FALSE ) );
      }
    } /* end if( (config == CAI_FOR_SUP_STOP) ||
         (config == CAI_FOR_SUP_SPEC_STOP) ) */

    //ASSERT that number of pilots are greater than 0. We
    //should never get 0 pilots.
    ASSERT( num_of_pilots > 0 );

    /* Now fill up the rest of the SRCH command and send it */
    mcc_srch_buf.tc_sup.hdr.cmd = SRCH_TC_UPDATE_SUP_F; /* Cmd Header */
    mcc_srch_buf.tc_sup.num_sup_pilots = num_of_pilots; /* # of pilots*/

    /* Now, send the command to SRCH */
    mcc_srch_cmd( &mcc_srch_buf );

    // If IS2000 is defined, then we need to inform the deinterleaver/decoder
    // the number of supplemental channels also.
    mccscm_set_dec_sup( &mcc_srch_buf );
  }
  else  /* if not ( msg_ptr->fix.for_inc == 1 ) */
  {
    /* no forward supplemental information in the message */
    MSG_MED(" Rx'ed SCAM with FOR_INCLUDED == FALSE", 0, 0, 0);
  }/* end if ( msg_ptr->fix.for_inc == 1 ) */

  return( MDR_DONE_S );
  
} /* end function mccscm_scam_sup_process */

/*===========================================================================

FUNCTION MCCSCM_SCAM_PROCESS

DESCRIPTION
  This function processes a SCAM message. This includes pre-
  processing and checking for errors, starting a start timer
  if necessary, and processing the sup channel info if
  there is no explicit start time provided.

  If there is a reason to reject the SCAM message, this
  function will send the reject order with the appropriate reject
  reason.

  Only Forward traffic channel supplementals are supported.

DEPENDENCIES
  FEATURE_IS95B_MDR must be turned on, and a MDR call must be in
  progress. The function should be called before the
  after the SCAM start/stop time processing, since the pre-processing
  happens before the start time of the message. However, if the
  start time is implicit, we can process the message right away, so
  the function will call the supplemental processing function.

RETURN VALUE
  A value of type mccscm_stat_enum_type indicating success or
  failiure.

SIDE EFFECTS
  None.

===========================================================================*/
mccscm_stat_enum_type mccscm_scam_process
(
  caii_rx_msg_type *msg_ptr /* Pointer to the SCAM message */
)
{
  mccscm_stat_enum_type reject_reason; /* Hold the reject reason */

  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  /* Some fields of the message can be pre-processed, ie, processed
  before the start time provided with the message. Pre-processing
  includes checking if all fields are within valid range and rejecting
  the message if it needs to be rejected */

  /* If forward sup information is included, we need to check
  for certain reject reasons specified by the standard */
  if( msg_ptr->scam.fix.for_inc != FALSE )
  {
    /* Only if we're starting supplementals, we need
    to test for these errors. */
    if( (msg_ptr->scam.fix.for_sup_config == CAI_FOR_SUP_START) ||
        (msg_ptr->scam.fix.for_sup_config == CAI_FOR_SUP_SPEC_START) )
    {
      /* Check if USE_FOR_HDM_SEQ and EXPL_FOR_START_TIME are set to 1.
      This means the message must be rejected due to unacceptable msg
      structure. */
      if( (msg_ptr->scam.fix.expl_for_start_tm != FALSE) &&
        (msg_ptr->scam.fix.use_for_hdm_seq != FALSE) )
      {
        MSG_HIGH( "Rejecting SCAM: rej code %x", MDR_REJ_STRUCT_S, 0, 0 );
        rej_parms.rej_msg_type = CAI_SUP_CHN_ASN_MSG;
        rej_parms.rej_code = MDR_REJ_STRUCT_S;  /*lint !e641 */
        send_rej_order( &rej_parms);
        return( MDR_FAIL_S );
      }
      /* If the SCAM is not linked to a GHDM and the pilot
      PN's included in the message are not in the active set, we need
      to reject with "message field not in valid range" reject reason */
      if( msg_ptr->scam.fix.use_for_hdm_seq == FALSE &&
          msg_ptr->scam.fix.for_sup_config == CAI_FOR_SUP_SPEC_START)
      {
        if( !(mccccl_code_channel_list.sup_pilots_are_valid(
                (msg_ptr->scam.fix.num_sup_plt), /* Number of pilots */
                (msg_ptr->scam.var)              /* Pointer to code channel
                                                 info for all pilots */
                                                           )) )
        {
          MSG_HIGH( "Rejecting SCAM: rej code %x", MDR_REJ_FIELD_S, 0, 0 );
          rej_parms.rej_msg_type = CAI_SUP_CHN_ASN_MSG;
          rej_parms.rej_code = MDR_REJ_FIELD_S;  /*lint !e641 */
          send_rej_order( &rej_parms);
          return( MDR_FAIL_S );
        }
      }
    }
  }/* end if (msg_ptr->scam.fix.for_inc != false) */

  /* Pre-process the common MDR fields */
  reject_reason = mccscm_mdr_pre_process( msg_ptr );
  if( reject_reason != MDR_DONE_S )
  {
    /* Check if the return value is an actual reject code
    We use only four reject reasons for the SCAM message */
    if( (reject_reason == MDR_REJ_CAP_S) ||
        (reject_reason == MDR_REJ_FIELD_S) ||
        (reject_reason == MDR_REJ_STRUCT_S) ||
        (reject_reason == MDR_REJ_STATE_S) )
    {
      /* We got a reject code. Reject the message with that code.  */
      MSG_HIGH( "Rejecting SCAM: rej code %x", reject_reason, 0, 0 );
      rej_parms.rej_msg_type = CAI_SUP_CHN_ASN_MSG;
      rej_parms.rej_code = (byte) reject_reason;
      send_rej_order( &rej_parms);
      return( MDR_FAIL_S );
    }
    else
    {
      //This should never happen - an unknown failiure means we
      //this function was called incorrectly or it's software madness.
      //but we don't have to error fatal. The mobile can recover
      //from this since the SCAM will be re sent. We'll assert
      //to make sure the value is not corrupted
      ASSERT((reject_reason == MDR_FAIL_S) || (reject_reason == MDR_UNKNOWN_S));
      return( MDR_FAIL_S );
    }
  }/* end if !MDR_DONE_S */
  else
  {
    /* Message is ok, now check if there is a explicit start timer
    associated with it. If so, we wait for the start time to
    start processing the supplemental assignments. */

    /* First check if the timer is already active. This means
    a new SCAM was received before the old SCAM's start timer
    expired. We superceed the old SCAM with the new SCAM */
    /* if another start timer is pending, we want to clear it */
    if( mccscm_chk_prev_start_timer() )
    {
      /* Let's indicate this and clear the older start timer */
      MSG_HIGH( "Got New SCAM before SCAM start tmr expired, tmr clr'd",0,0,0);
      ( void )rex_clr_timer( &mcc_scam_start_timer );

      /* The signal for the SCAM start timer has already been set, so we need
       * to clear the signal */
      (void)rex_clr_sigs(&mc_tcb, MCC_SCAM_TIMER_SIG);

      /* It's ok, if the 2nd SCAM arrives when the first SCAM is already in
       * process, ie. the duration timer is set, since we will deal with this
       * later when the 2nd start timer expires */
    }

    /* Check if the message has an explicit forward
    start time. If, so call the start timer checking function */
    /* Again, explicit start time is included only if we should
    start supplementals */
    if( ((msg_ptr->scam.fix.for_sup_config == CAI_FOR_SUP_START) ||
         (msg_ptr->scam.fix.for_sup_config == CAI_FOR_SUP_SPEC_START))
         && (msg_ptr->scam.fix.expl_for_start_tm != FALSE) )
    {
      mccscm_scam_timer_set( &(msg_ptr->scam) );
    }
    else
    {
      /* We either are stopping supplementals or we have explicit
      start time set to FALSE. So, we can process the SCAM right away
      this includes updating the code-channel-list and sending the
      command to SRCH to start or stop supplementals */
      MSG_MED( "Processing SCAM Msg", 0, 0, 0 );
      (void) (mccscm_scam_sup_process(&(msg_ptr->scam)));
      //No need to check the return value since errors are
      //handled within the function itself.
    }
    return( MDR_DONE_S );
  }
  
}/* end function mccscm_scam_process */
#endif /* FEATURE_IS95B_MDR */

/*===========================================================================

FUNCTION MCCSCM_DURATION_TIMER_CHK

DESCRIPTION
  This function starts a MDR duration timer for the specified time. If
  the duration timer is already on, it cancels the duration timer and
  starts a new duration timer based on the given message.

  The duration time provided by the BS is a absolute value in 80ms
  units. Based on whether we have an implicit start time or not, we
  need to adjust the duration timer so that we don't stop too early and
  miss data frames.

DEPENDENCIES
  A MDR call must be in progress.

RETURN VALUE
  MDR_DONE_S indicating that the timer is set as needed. No
  other return values are needed here.

SIDE EFFECTS
  None.

===========================================================================*/
mccscm_stat_enum_type mccscm_duration_timer_chk
(
  byte duration,         /* An unsigned 8 bit value which represents
                         the duration time in 80ms units */

  boolean implicit_start /* Indication of whether the message
                         had an implicit action time or start time */
)
{
  qword curr_time;
    /* Current time */
  word phase_adjust;
    /* 20ms offset into 80ms superframe */
  word wait_time; /* Amount of time to set duration time for */

  /* Calculate the actual time to wait(in ms)
  by multiplying the given duration by 80 ms */
  wait_time = (word)( duration * 80 ); /* Wait time in milliseconds */
  /* Note: the maximum size of duration_time is 8 bits, so
  wait time won't be longer than 16 bits. */

  if( implicit_start )
  {
    /* We have to adjust the duration time based on the 80 ms
    superframe boundary in order to not miss any data frames
    by stopping too early. This is because if the message
    has an implicit start or action time, we might potentially
    start decoding upto 1 super frame and upto 3 20 ms frames earlier than
    necessary */

    /* The worst case adjustment needed is -
    actual duration + 80 ms superframe + up to 3 20 ms frames */

    /* Get current time in 20 ms units */
    ts_get_20ms_frame_time( curr_time );

    /* Convert current time into 80 ms units
    Note that the qw_div function puts the reminder of the
    division in phase_adjust, and the quotient in curr_time */
    /* This means phase_adjust has the sub 80 ms chunk of system time */

    phase_adjust = qw_div( curr_time, curr_time, 4U );

    /* Now add the worst case time needed after duration */

    wait_time = (word) (wait_time + 80 + phase_adjust*20);
    /* It still won't be longer than 16 bits */
  }

  /* We don't have to adjust for sub-20ms duration since the SRCH
  task stops decoding only at 20 ms frame boundaries even if the
  command is sent earlier */

  /* Set the duration timer */
  (void) rex_set_timer( &mcc_mdr_duration_timer,  wait_time );

  /* Notice that rex_set_timer will clear any previous setting of
  this timer and will also clear the signal associated with this
  timer */

  return( MDR_DONE_S );
}/* end function mccscm_duration_timer_chk */



/*===========================================================================

FUNCTION MCCSCM_PROCESS_STOP_REQ

DESCRIPTION
  This function process a request to stop processing supplementals.
  The function needs to be invoked when the duration timer expires
  so that the mobile can stop processing supplementals at that time.
  This function may also be invoked at the end of an MDR call if the
  supplementals are active.

  This function sends a SRCH command to stop processing supplementals.
  It will not affect the code channel list maintenance, ie. all the
  supplemental channel walsh codes in the current actice set will be retained.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None. Note: This function does not modify the code channel list.
===========================================================================*/
void mccscm_process_stop_req( void )
{
  byte num_of_pilots=0; /* To store the number of active pilots    */

  /* Prepare the supplemental channel part of the
  SRCH command using generate_aset_sups function. Indicate
  that we don't want the sups to be sent to SRCH */
  MSG_HIGH("STOPPING SUPS - Duration expired", 0, 0, 0);
  num_of_pilots =
  mccccl_code_channel_list.generate_aset_sups(
     mcc_srch_buf.tc_sup.sup_pilots,      /* Pointer to pilot info*/
     FALSE                                /* SCCH not used */
                                             );
  if( num_of_pilots == 0 ) /* If there are no active pilots */
  {
    MSG_MED("No active pilots, skipping cmd to stop sups", 0, 0, 0);
    return;
  }

  /* Now fill up the rest of the SRCH command */
  mcc_srch_buf.tc_sup.hdr.cmd = SRCH_TC_UPDATE_SUP_F; /* Cmd Header */
  mcc_srch_buf.tc_sup.num_sup_pilots = num_of_pilots; /* # of pilots*/
  /* Now, send the command to SRCH */
  mcc_srch_cmd( &mcc_srch_buf );

  // If IS2000 is defined, then we need to inform the deinterleaver/decoder
  // the number of supplemental channels also.
  mccscm_set_dec_sup( &mcc_srch_buf );
}

/*===========================================================================

FUNCTION MCCSCM_SET_DEC_SUP

DESCRIPTION
  This function sends a command to RXC which will program the deinterleaver
  and decoder to decode the number of supplementals requested.  This function
  must be called right after sending the command to searcher to update
  supplementals.  Since the information is already contained in the searcher
  command and this function is always called after, this command will just
  take the srch command as input parameter.

DEPENDENCIES
  mcc_srch_buf must contain the just executed SRCH command (this is the case
  since SRCH does not corrupt the command packet).

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mccscm_set_dec_sup
(
  srch_cmd_type *srch_command
)
{
  byte i;

  // Set up command for RXC
  mcc_rx_buf.scch.hdr.command = RXC_SCCH_F;
  // Since some pilots may not contain pilot, we need to look for the first non-zero
  // num_sup_channels (if it exists).
  if ( srch_command->hdr.cmd == SRCH_TC_UPDATE_SUP_F )
  {
    // This takes care of SCAM initiated SCCH
    for ( mcc_rx_buf.scch.num_sup = i = 0; i < srch_command->tc_sup.num_sup_pilots; i++ )
    {
      if ( srch_command->tc_sup.sup_pilots[i].num_sup_channels > 0 )
      {
        mcc_rx_buf.scch.num_sup = srch_command->tc_sup.sup_pilots[i].num_sup_channels;
        break;
      }
    }
  }
  else
  {
    // This takes care of GHDM initiated SCCH
    for ( mcc_rx_buf.scch.num_sup = i = 0; i < srch_command->tc_aset.aset_cnt; i++ )
    {
      if ( srch_command->tc_aset.aset[i].scch_info.num > 0 )
      {
        mcc_rx_buf.scch.num_sup = srch_command->tc_aset.aset[i].scch_info.num;
        break;
      }
    }
  }
  // If none of the pilots contains supplementals, then mcc_rx_buf.scch.num_sup will be 0

  // Now send the command to RXC
  mcc_rxc_cmd( &mcc_rx_buf );
  
}  /*lint !e818 */

#endif // defined (FEATURE_IS95B_MDR)

