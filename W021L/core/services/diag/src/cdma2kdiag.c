/*==========================================================================

                   D I A G N O S T I C     T A S K
                  P A C K E T   P R O C E S S I N G
                           1 X Integration

GENERAL DESCRIPTION
  This file contains routines that are called to process the arrival of
  packets on the sio link.  There is one procedure for each packet id
  and each procedure has the same job:

    1. Check packet for correctness.

    2. Perform processing required by the packet.

    3. Format a response packet.


Copyright (c) 2001-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            Edit History

$Header: //depot/asic/msmshared/services/diag/cdma2kdiag.c#44 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/06   as      Fixed critical lint errors.
06/20/06   as      Added featurization around cdma2kdiag_status_snapshot.
10/31/05   as      Fixed lint errors.
07/15/05   as      Cleanup cdma2kdiag_tmob featurization 
05/18/04   as      Added 10 sec timeout in AKEY to prevent attacker to query
                   the A-key more rapidly.
01/08/04   as      Moved prl specific functions to prldiag.c/h
12/03/03   yll     Changed the mob_cai_rev to reflect the NV setting.
10/17/03   ph      For Rel A builds, redirect to new MC subsystem command and
                   added support for the new subsystem command.
09/12/03   yll     Added featurization for ACP include file.
08/19/03   gr      Deleted the #ifndef FEATURE_EFS2 wrapping the call to
                   fsdiag_init. The old diag interface will be present even
                   if FEATURE_EFS2 is defined.
08/16/03   as      Removed FEATURE_SPECIAL_MDR
07/28/03   wd      Un-Remove oboslete FTM_RF_TEST_CMDS, some targets still 
                   rely on these commands.
07/15/03   jqi     Added Extended PRL support
01/24/03   wd      Remove obsolete FTM_RF_TEST_CMDS.
01/08/03   ma      featurization for GSM PLT
11/20/01   as      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#include "cdma2kdiag.h"
#include "diag.h"
#include "diagi.h"
#include "diagnv.h"
#include "diagpkt.h"
#include "diagtarget.h"
#include "mccdma.h"
#include "mobile.h"
#include "msg.h"
#include "parm.h"
#include "srch.h"
#include "rxc.h"
#include "txc.h"
#include "rf.h"
#include "snd.h"
#include "snm.h"


#ifndef FEATURE_GSM_PLT  
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
  DB item data buffer for db reads
--------------------------------------------------------------------------*/
LOCAL db_items_value_type diag_db;

/*--------------------------------------------------------------------------
  DTMF table
--------------------------------------------------------------------------*/
LOCAL snd_tone_type dtmf_tab[] = {
  SND_0,  SND_1,  SND_3,  SND_4,  SND_5,  SND_6,  SND_7,  SND_8,  SND_9,
  SND_POUND,  SND_STAR
};

/*--------------------------------------------------------------------------
  Local variable to keep track of significant events.  It is incremented by
  other tasks calling the diag_inc_event_cnt function, and is returned as
  part of the phone_state response packet.
--------------------------------------------------------------------------*/
LOCAL word diag_event_count = 0;

#ifdef FEATURE_HWTC  
#error code not present
#endif /* FEATURE_HWTC */

/*===========================================================================

FUNCTION CDMA2KDIAG_TAGRAPH

DESCRIPTION
  This returns the latest vocoder rates and powers for the strip chart
  on the camera view temporal analyzer

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * cdma2kdiag_tagraph (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
#ifndef FEATURE_HWTC
  parm_fer_type   fer_data;         /* data for FER parameters */
#endif
  diag_tagraph_rsp_type   *rsp_ptr;

  const int rsp_len = sizeof(diag_tagraph_rsp_type);

  
  rsp_ptr = (diag_tagraph_rsp_type *)diagpkt_alloc (DIAG_TAGRAPH_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_tagraph.", 0, 0, 0);
    return(rsp_ptr);
  }
  /*------------------------------------------------------------------------
    Get the rssi data from the searcher and the vocoder rate data from
    the vocoder.
  -------------------------------------------------------------------------*/
#ifndef FEATURE_HWTC
  rsp_ptr->rx_rssi  = srch_get_rx_agc();

#ifdef FEATURE_EXT_LINEARIZER
#error code not present
#else
  rsp_ptr->adj_rssi = srch_get_tx_adj();
#endif /* FEATURE_EXT_LINEARIZER */
#endif /* !FEATURE_HWTC */

  /*------------------------------------------------------------------------
    txc_rpt_voc_rate and rxc_rpt_voc_rate return an enumerated type which
    only contains a few entries, so the packet uses a byte for them.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HWTC
#error code not present
#else
  rsp_ptr->tx_voc   = (byte) txc_rpt_voc_rate();
  rsp_ptr->rx_voc   = (byte) rxc_rpt_voc_rate();

  parm_get_fer ( &fer_data );

  rsp_ptr->bad_frames   = fer_data.bad_frames;
  rsp_ptr->total_frames = fer_data.total_frames;
#endif /* FEATURE_HWTC */

  return (rsp_ptr);

} /* cdma2kdiag_tagraph */



#ifndef FEATURE_HWTC
/*===========================================================================
FUNCTION CDMA2KDIAG_TA_PARM

DESCRIPTION
  Sets parameters for ta.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * cdma2kdiag_ta_parm (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  diag_ta_parm_req_type *req_ptr = (diag_ta_parm_req_type *) req_pkt_ptr;
  diag_ta_parm_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_ta_parm_rsp_type);

  /*-----------------------------------------------------------------------
    Tell the searcher about the new TA parameters.
  -----------------------------------------------------------------------*/
  srch_set_ta_parm (req_ptr->set_mask, req_ptr->win_siz);

  rsp_ptr =  (diag_ta_parm_rsp_type *)diagpkt_alloc(DIAG_TA_PARM_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_ta_parm.", 0, 0, 0);
  }

  return (rsp_ptr);

} /* cdma2kdiag_ta_parm */
#endif /* !FEATURE_HWTC */



#ifndef FEATURE_HWTC
/*===========================================================================
FUNCTION CDMA2KDIAG_SER_RESET

DESCRIPTION
  Processes a request to reset the SER statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * cdma2kdiag_ser_reset (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  diag_ser_reset_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_ser_reset_rsp_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rxc_ser_reset();

  rsp_ptr = (diag_ser_reset_rsp_type *)diagpkt_alloc(DIAG_SER_RESET_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_ser_reset.", 0, 0, 0);
  }

  return (rsp_ptr);

} /* cdma2kdiag_ser_reset */



/*===========================================================================
FUNCTION CDMA2KDIAG_SER_REPORT

DESCRIPTION
  Processes a request for SER statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * cdma2kdiag_ser_report (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  rxc_ser_type *ser_ptr;
  diag_ser_report_rsp_type *rsp_ptr;
  const int rsp_len = sizeof (diag_ser_report_rsp_type);

  rsp_ptr = (diag_ser_report_rsp_type *)diagpkt_alloc(DIAG_SER_REPORT_F, rsp_len);

  if(rsp_ptr)
  { 
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Cast the response buffer to the rxc_ser_type.  This works since the
    ** diag_ser_report_rsp_type, starting at the frame_cnt field, is the
    ** same as the rxc_ser_type.  Tell lint this is not a problem.
    */
    ser_ptr = (rxc_ser_type *)&rsp_ptr->frame_cnt; //lint !e740

    /* call service routine to report the ser results */
    rxc_ser_report( ser_ptr );
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_ser_report.", 0, 0, 0);
  }

  return (rsp_ptr);

} /* cdma2kdiag_ser_report */
#endif /* FEATURE_HWTC */



/*===========================================================================

FUNCTION CDMA2KDIAG_TEST

DESCRIPTION
  Processes a request for a test to be run.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * cdma2kdiag_test (
  PACKED void  *req_pkt_ptr,  /* pointer to request packet  */
  word pkt_len                /* length of request packet   */
)
{
  diag_test_req_type *req_ptr = (diag_test_req_type *) req_pkt_ptr;
  diag_test_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_test_rsp_type);

#if !defined(FEATURE_NEW_SOUND_API)
#ifndef FEATURE_HWTC
  snd_packets_type *snd_ptr;
#endif /* FEATURE_HWTC */
#endif
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
  diag_test_enum_type         test;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  test = (diag_test_enum_type) req_ptr->test_code;

  MSG_HIGH( "Got directed test: %d", test, 0, 0);

  db_get( DB_DMSS_STATE, &diag_db );    /* Read the state for permissions */

  switch ( test ) {
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    case DT_DTMF_ON_F:
    case DT_DTMF_OFF_F:

        if ( diag_db.dmss_state != DB_DMSS_OFFLINE_ANALOG_S ) {
          return ( diagpkt_err_rsp ( DIAG_BAD_MODE_F, req_pkt_ptr, pkt_len ) );
        }

#ifndef FEATURE_HWTC
#if defined(FEATURE_NEW_SOUND_API)
        if (test == DT_DTMF_ON_F) {
          snd_tone_start( SND_DEVICE_HFK, SND_METHOD_KEY_BEEP,
                          dtmf_tab[ req_ptr->parm.dtmf.dtmf ],
                          0, SND_APATH_LOCAL, NULL, NULL);
        } else {
          snd_tone_stop( NULL, NULL );
        }
#else
        if( ( snd_ptr = snd_get_packet()) != NULL ) {
          if (test == DT_DTMF_ON_F) {
            snd_ptr->hdr.cmd        = SND_TONE_START;
            snd_ptr->tone.apath     = SND_APATH_EXT;
            snd_ptr->tone.sclass    = SND_CLASS_VOICE_BEEP;
            snd_ptr->tone.dtmf_path = SND_DTMF_APATH_BOTH;
            snd_ptr->tone.tone.tone = dtmf_tab[ req_ptr->parm.dtmf.dtmf ];
            snd_ptr->tone.tone.duration_ms = 0;
            snd_ptr->hdr.task_ptr   = NULL;
            snd_cmd( snd_ptr );
          } else {
            snd_ptr->hdr.cmd = SND_TONE_STOP;
            snd_cmd( snd_ptr );
          }
        }
#endif /* FEATURE_NEW_SOUND_API */
#endif /* FEATURE_HWTC */
        break;

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

    case DT_SEND_NAM_F:
    case DT_SWITCH_NAM_F:
    case DT_SEND_MIN_F:
    case DT_VERSION_F:
    case DT_SEND_ESN_F:
    default:
        MSG_HIGH ( "Unrecognized directed test",
                    req_ptr->test_code, 0, 0 );
        return (diagpkt_err_rsp ( DIAG_BAD_MODE_F, req_pkt_ptr, pkt_len ) );

  }

  rsp_ptr = (diag_test_rsp_type *)diagpkt_alloc(DIAG_TEST_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_test.", 0, 0, 0);
    return(rsp_ptr);
  }

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

  rsp_ptr->test_code = test;
  return (rsp_ptr);
} /* cdma2kdiag_test */



/*===========================================================================

FUNCTION DIAG_INC_EVENT_CNT

DESCRIPTION
  This function increments a count of events that took place in the phone
  which might imply that the phone had changed enough that other kinds of
  status might want to be requested from diag by external devices.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void  diag_inc_event_cnt ()
{
  diag_event_count++;
}



/*===========================================================================

FUNCTION DIAG_GET_EVENT_CNT

DESCRIPTION
  This function returns the count of events maintained by this module.

DEPENDENCIES
  None.

RETURN VALUE
  diag_event_count.

SIDE EFFECTS
  None.

===========================================================================*/
word  diag_get_event_cnt ()
{
  return diag_event_count;
}




/*===========================================================================
FUNCTION CDMA2KDIAG_STATE

DESCRIPTION
  Processes a request for the DMSS to return the current operating state
  of the phone.  The states mostly correspond to IS95/A states, and are
  set by the appropriate master control into the db, where the value is
  retrieved by Diag.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * cdma2kdiag_state (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  diag_state_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_state_rsp_type);

  rsp_ptr = (diag_state_rsp_type *)diagpkt_alloc(DIAG_STATE_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_state.", 0, 0, 0);
    return(rsp_ptr);
  }

/*------------------------------------------------------------------------*/

  /* For Rel A, DIAG clients are redirected to use the subsystem command */
  /* (subsys 30, cmd_code 2) implemented in mclog.c                      */
  /* This redirection is indicated to clients through                    */
  /* DIAG_D_USE_SUBSYS_CMD_S. Upon receiving this code, clients must     */
  /* query the phone with the DIAG subsystem dispatch command using      */
  /* (subsys 30 and cmd_code 2). The format of this subsystem command    */
  /* is to be included in a DIAG Subsystem ICD due out soon.             */
  /* You may also refer to mclog.h for the req/rsp structure.            */

#ifdef FEATURE_IS2000_REL_A
  rsp_ptr->phone_state = DIAG_D_USE_SUBSYS_CMD_S;
#else
  /* Get the current phone state from MC */
  rsp_ptr->phone_state = mc_state();
#endif

  rsp_ptr->event_count = diag_event_count;

  MSG_LOW("Retrieved current state: %d, %d",
                     rsp_ptr->phone_state,
                     rsp_ptr->event_count, 0);

  return (rsp_ptr);

} /* cdma2kdiag_state */



#ifndef FEATURE_HWTC
/*===========================================================================
FUNCTION CDMA2KDIAG_PILOT_SETS

DESCRIPTION
  Processes a request for the DMSS to return all of the information regarding
  pilot sets at this time.  It returns the number pilots in each of active,
  candidate, and neighbor sets.  It also gives the remaining set pilot
  increment.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * cdma2kdiag_pilot_sets (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  srch_pilot_set_struct_type pilot_sets;  /* To hold data from srch */

  int i;                                  /* Index for copying structs */
  diag_pilot_sets_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_pilot_sets_rsp_type);

/*------------------------------------------------------------------------*/

  /* Call srch function to fill in response here */
  srch_get_pilot_set ( &pilot_sets );

  /* fill response packet */
  rsp_ptr = (diag_pilot_sets_rsp_type *)diagpkt_alloc(DIAG_PILOT_SETS_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_pilot_sets.", 0, 0, 0);
    return(rsp_ptr);
  }

  rsp_ptr->pilot_inc  = pilot_sets.pilot_inc;
  rsp_ptr->active_cnt = pilot_sets.active_cnt;
  rsp_ptr->cand_cnt   = pilot_sets.cand_cnt;
  rsp_ptr->neigh_cnt  = pilot_sets.neigh_cnt;

  for (i = 0;
       i < (pilot_sets.active_cnt + pilot_sets.cand_cnt + pilot_sets.neigh_cnt);
       i++) {
    rsp_ptr->pilot_sets[i].pilot_pn =
                                      pilot_sets.pilot_sets[i].pilot_pn;
    rsp_ptr->pilot_sets[i].pilot_strength =
                                      pilot_sets.pilot_sets[i].pilot_strength;
  }

  return (rsp_ptr);

} /* cdma2kdiag_pilot_sets */
#endif /* !FEATURE_HWTC */



#ifndef FEATURE_HWTC
#if defined(FEATURE_IS95B_MDR)
/*===========================================================================
FUNCTION CDMA2KDIAG_GET_WALSH_CODES

DESCRIPTION
        Returns the walsh codes to DM for display.

DEPENDENCIES

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * cdma2kdiag_get_walsh_codes (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  diag_sup_walsh_codes_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_sup_walsh_codes_rsp_type);

  rsp_ptr = (diag_sup_walsh_codes_rsp_type *)diagpkt_alloc (DIAG_SUP_WALSH_CODES_F, rsp_len);

  if(rsp_ptr)
  {
    INTLOCK();
    srch_aset_walsh ((srch_walsh_struct_type *) &rsp_ptr->num_aset);
    /* This search structure needs to match the walsh code response
     structure with diag */
    INTFREE();
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_get_walsh_codes.", 0, 0, 0);
  }
  /* Return the length of the packet.*/
  return (rsp_ptr);
} /* cdma2kdiag_get_walsh_codes */



/*===========================================================================

FUNCTION CDMA2KDIAG_SET_MAX_SUP_CH

DESCRIPTION
  This procedure processes a request to set the maximum number of supplemental
  channels to be allowed in MDR.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACKED void * cdma2kdiag_set_max_sup_ch (
  PACKED void *req_pkt_ptr,      /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  diag_set_max_sup_ch_req_type *req_ptr = (diag_set_max_sup_ch_req_type *) req_pkt_ptr;
  diag_set_max_sup_ch_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_set_max_sup_ch_rsp_type);

  /*---------------------------------------------------------
   Call function to set # of channels and check for success.
  ----------------------------------------------------------*/
  if (snm_set_max_for_sups(req_ptr->service_option, req_ptr->num_sup_ch)
        == SNM_SUCCESS)
  {
    return (diagpkt_err_rsp( DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  rsp_ptr = (diag_set_max_sup_ch_rsp_type *)diagpkt_alloc (DIAG_SET_MAX_SUP_CH_F, rsp_len);
  
  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_set_max_sup_ch.", 0, 0, 0);
  }

  return (rsp_ptr);

} /* cdma2kdiag_set_max_sup_ch */
#endif /* defined(FEATURE_IS95B_MDR) */
#endif /* !FEATURE_HWTC */



/*===========================================================================
FUNCTION CDMA2KDIAG_AKEY_VERIFY

DESCRIPTION


  This procedure verifies an entry of the AKEY.  This function reads the
  AKey from NV, and compares it to what was sent in the packet.
  If NAM value is something other than 0xff, the AKEY for given NAM is changed.
  If NAM value is 0xff, AKEY for the current nam is changed.

  If they are the same, a value of TRUE is returned to the external device.
  If they are NOT the same, a value of FALSE is returned to the external
  device, and a rex_timed_wait is called, to make DIAG pause
  for 10 seconds, to deter hackers who are trying all possible AKEYs.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  May cause DIAG to timeout for 10 seconds.

===========================================================================*/
PACKED void * cdma2kdiag_akey_verify (
  PACKED void *req_pkt_ptr,
  word pkt_len
)
{
  int i;                            /* loop counter */

  qword challenge_A_key;            /* A_key converted from request packet. */
  qword existing_A_key;             /* A_key from NV request packet. */

  diag_akey_verify_req_type *req_ptr =
    (diag_akey_verify_req_type *) req_pkt_ptr;
  diag_akey_verify_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_akey_verify_rsp_type);

 /*------------------------------------------------------------------------*/

  rsp_ptr = (diag_akey_verify_rsp_type *)diagpkt_alloc(DIAG_AKEY_VERIFY_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_akey_verify.", 0, 0, 0);
    return(rsp_ptr);
  }

  /* Assume failure. */
  rsp_ptr->akey_ok = FALSE;

  /* Send the nv read request, and report the status of the
  ** nv operation back to external device.
  */

  qw_set( challenge_A_key, 0L, 0L );
  qw_set( existing_A_key, 0L, 0L );

  /* -------------------------------------------------------------
  ** Convert A_key to 64-bit representation.  The last digits in
  ** the array are a checksum, which is not used in this function.
  ** ------------------------------------------------------------- */

#define  DIAG_CAVE_DATA_DIGITS       20

  for (i = 0; i < DIAG_CAVE_DATA_DIGITS; i++)
  {
    qw_mul( challenge_A_key, challenge_A_key, 10L );
    qw_inc( challenge_A_key, (dword)(req_ptr->akey[i] - '0'));
  }

#define DIAG_CURR_NAM 0xFF

  do
  {
    if (req_ptr->nam == DIAG_CURR_NAM)
    {
      if (diag_nv_read (NV_CURR_NAM_I, &nitem) == NV_DONE_S)
      {
        nitem.a_key.nam = nitem.curr_nam;
      }
      else
      {
        MSG_HIGH("Unable to read current NAM from NV", 0, 0, 0);

        /* We cannot find out which AKey to read, so we must fail; */
        break;
      } /* nv_read CURR_NAM */
    }
    else
    {
      nitem.a_key.nam = req_ptr->nam;
    }

    if (diag_nv_read (NV_A_KEY_I, &nitem) != NV_DONE_S)
    {
      MSG_HIGH("Failed to read A-Key", 0, 0, 0);
      /* Since we couldn't check it, assume it was wrong (but don't make diag
      ** timeout)
      */
      break;
    }
    else
    {
      /* Start out assuming it is ok */

      qw_equ_misaligned(existing_A_key, nitem.a_key.key);

      if (!qw_cmp(challenge_A_key, existing_A_key))
      {
        rsp_ptr->akey_ok = TRUE;
      }
      else
      {
        /* If failed match, make DIAG timeout 10 seconds (just like SPC) */
        rsp_ptr->akey_ok = FALSE;
        diagpkt_commit(rsp_ptr);
        diag_timeout();
        diagbuf_flush();
      }
    } /* nv read AKEY */
  } while (0);

  return (rsp_ptr);

} /* cdma2kdiag_akey_verify */



#if !defined (FEATURE_HWTC) && !defined (IMAGE_APPS_PROC)
/*===========================================================================

FUNCTION CDMA2KDIAG_STATUS_SNAPSHOT
DESCRIPTION
  This procedure configures the event reporting mechanism.

===========================================================================*/
PACKED void * cdma2kdiag_status_snapshot (
  PACKED void *req_pkt,
  word pkt_len
)
{
  diag_status_snapshot_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_status_snapshot_rsp_type);

  rsp_ptr = (diag_status_snapshot_rsp_type *)diagpkt_alloc(DIAG_STATUS_SNAPSHOT_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_status_snapshot.", 0, 0, 0);
    return(rsp_ptr);
  }

  rsp_ptr->esn = diagnv.esn;

  /* Collect the FULL IMSI */
  rsp_ptr->full_imsi.imsi_s1 = cdma.imsi_s1;
  rsp_ptr->full_imsi.imsi_s2 = cdma.imsi_s2;
  rsp_ptr->full_imsi.imsi_s = QW_CVT_Q2N(cdma.imsi_s);
  rsp_ptr->full_imsi.imsi_11_12 = cdma.imsi_11_12;
  rsp_ptr->full_imsi.mcc = cdma.mcc;
  rsp_ptr->full_imsi.imsi_addr_num = cdma.imsi_addr_num;

  db_get ( DB_SID, &diag_db );
  rsp_ptr->sid = diag_db.sid;

  db_get ( DB_NID, &diag_db );
  rsp_ptr->nid = diag_db.nid;

  db_get ( DB_BS_P_REV, &diag_db );
  rsp_ptr->p_rev = diag_db.bs_p_rev;

  rsp_ptr->prev_in_use = mc_get_p_rev_in_use();
  rsp_ptr->mob_p_rev = cdma.mob_cai_rev;

  rsp_ptr->band_class = mc_get_band_class();

  db_get ( DB_CHANNEL, &diag_db );
  rsp_ptr->frequency = diag_db.channel;

  db_get ( DB_RF_MODE, &diag_db );
  rsp_ptr->operation_mode = diag_db.rf_mode;

  rsp_ptr->state = (byte) (cdma.curr_state >> 8);
  rsp_ptr->sub_state = (byte) cdma.curr_state;

  return (rsp_ptr);

} /* cdma2kdiag_status_snapshot */
#endif /* FEATURE_HWTC */


#ifndef FEATURE_HWTC
/*===========================================================================

FUNCTION CDMA2KDIAG_TMOB

DESCRIPTION
  Processes a request a Test Mobile Function

DEPENDENCIES
  None.  Only active when compiled for Test Mobile (TMOBILE defined).
  Otherwise will generate a DIAG_BAD_CMD_F response.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS

===========================================================================*/
PACKED void * cdma2kdiag_tmob (
  PACKED void *req_pkt_ptr,  /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{

/*------------------------------------------------------------------------*/

/*  Only allow if Test commands desired: TMOBILE or Pilot Scanner
    need the handlers for Test commands. */
#if !defined(TMOBILE)

#if defined(FEATURE_FACTORY_TESTMODE) && !defined(FEATURE_PLT)

   diag_ftm_req_type *req_ptr = (diag_ftm_req_type *) req_pkt_ptr;
   if (req_ptr->sub_cmd >= FTM_FIRST_CMD &&
       req_ptr->sub_cmd < FTM_MAX_CMD)
   {
     diag_ftm_rsp_type *rsp_ptr;
     byte status;

#if defined(FEATURE_FTM_RF) && !defined(FEATURE_FTM_TASK)
     if(req_ptr->sub_cmd != FTM_RF_TEST_CMDS)
#endif
     {
       const word rsp_len = sizeof (diag_ftm_rsp_type);

       status = (byte)ftm_cmd_dispatcher((byte)req_ptr->sub_cmd,
                        req_ptr->length,
                       (ftm_packed_param_type *) &req_ptr->request[0]);

       rsp_ptr = (diag_ftm_rsp_type *)diagpkt_alloc (DIAG_FTM_CMD_F, rsp_len);

       if(rsp_ptr == NULL)
       {
         MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_tmob.", 0, 0, 0);
         return(rsp_ptr);
       }

       rsp_ptr->sub_cmd = req_ptr->sub_cmd;
       rsp_ptr->status = status;
     }
#if defined(FEATURE_FTM_RF) && !defined(FEATURE_FTM_TASK)
     else
     {
       diag_ftm_req_type *ftm_rf_rsp;

       status = ftm_cmd_dispatcher(req_ptr->sub_cmd,
                    sizeof(ftm_packed_param_type), /* pc does not inclued length */
                    (ftm_packed_param_type *) &req_ptr->request[0]);

       /* Note: request and response are same format */
       ftm_rf_rsp = (diag_ftm_req_type *)diagpkt_alloc (DIAG_FTM_CMD_F, FPOS (diag_ftm_req_type, request) +
         sizeof(ftm_packed_param_type));
       if(ftm_rf_rsp == NULL)
       {
         MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_tmob.", 0, 0, 0);
         return(ftm_rf_rsp);
       }
       ftm_rf_rsp->sub_cmd = req_ptr->sub_cmd;
       ftm_rf_rsp->length = req_ptr->length;
       memcpy ((void *) &ftm_rf_rsp->request[0], (void *) &req_ptr->request[0],
               sizeof (ftm_packed_param_type));

       rsp_ptr = (diag_ftm_rsp_type *) ftm_rf_rsp; /* For the return value */
     }
#endif /* FEATURE_FTM_RF && !FEATURE_FTM_TASK*/

     return (rsp_ptr);
   }
   return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_ptr, pkt_len));
#else
   return (diagpkt_err_rsp (DIAG_BAD_CMD_F, req_pkt_ptr, pkt_len));
#endif

#else
#error code not present
#endif /* TMOBILE */
} /* cdma2kdiag_tmob */



#else /* FEATURE_HWTC defined */
#error code not present
#endif /* FEATURE_HWTC */



#ifdef FEATURE_HWTC
#error code not present
#endif /* FEATURE_HWTC */



/*===========================================================================

FUNCTION CDMA2KDIAG_SSD_VERIFY

DESCRIPTION


  This procedure verifies an entry of the SSD.  This function reads the
  SSDs from NV, and compares it to what was sent in the packet.
  If NAM value is something other than 0xff, the SSDs for given NAM are
  checked.  If NAM value is 0xff, SSDs for the current nam are checked.

  If they are the same, a value of TRUE is returned to the external device.
  If they are NOT the same, a value of FALSE is returned to the external
  device, and a rex_timed_wait is called, to make DIAG pause
  for 10 seconds, to deter hackers who are trying all possible SSDs.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  May cause DIAG to timeout for 10 seconds.

===========================================================================*/
PACKED void * cdma2kdiag_ssd_verify (
  PACKED void *req_pkt_ptr,
  word pkt_len
)
{
  DIAG_SSD_VERIFY_F_req_type *req_ptr =
    (DIAG_SSD_VERIFY_F_req_type *) req_pkt_ptr;
  DIAG_SSD_VERIFY_F_rsp_type *rsp_ptr;
  const int rsp_len = sizeof( DIAG_SSD_VERIFY_F_rsp_type );

 /*------------------------------------------------------------------------*/

  rsp_ptr = (DIAG_SSD_VERIFY_F_rsp_type *)diagpkt_alloc (DIAG_SSD_VERIFY_F, rsp_len);

  if(rsp_ptr == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in cdma2kdiag_ssd_verify.", 0, 0, 0);
    return(rsp_ptr);
  }

  /* Assume failure. */
  rsp_ptr->ssd_ok = FALSE;

  /* Send the nv read request, and report the status of the
  ** nv operation back to external device.
  */

#define DIAG_SSD_CURR_NAM 0xFF

  do {
    if (req_ptr->nam == DIAG_SSD_CURR_NAM) {
      if (diag_nv_read(NV_CURR_NAM_I, &nitem) == NV_DONE_S) {
         if (req_ptr->ssd_select == DIAG_SSD_A) {
           nitem.ssd_a.nam = nitem.curr_nam;
         }
         else if (req_ptr->ssd_select == DIAG_SSD_B) {
           nitem.ssd_b.nam = nitem.curr_nam;
         }
         else {
           MSG_ERROR(" Bad SSD Select entry ",0,0,0);
           return 0;
         }
      }
      else {
        MSG_HIGH("Unable to read current NAM from NV", 0, 0, 0);

        /* We cannot find out which AKey to read, so we must fail; */
        break;
      } /* nv_read CURR_NAM */
    }
    else {
      if (req_ptr->ssd_select == DIAG_SSD_A) {
        nitem.ssd_a.nam = req_ptr->nam;
      }
      else if (req_ptr->ssd_select == DIAG_SSD_B) {
        nitem.ssd_b.nam = req_ptr->nam;
      }
      else {
        MSG_ERROR(" Bad SSD Select entry ",0,0,0);
        return 0;
      }
    }

    if (req_ptr->ssd_select == DIAG_SSD_A) {
      /* Read SSD_A from NV and check against challenge */
      if ( diag_nv_read ( NV_SSD_A_I, &nitem ) != NV_DONE_S ) {
        MSG_HIGH("Failed to read SSD_A", 0, 0, 0);
        /* Since we couldn't check it, assume it was wrong (but don't make diag
        ** timeout)
        */
        break;
      }
      else {
        if( qw_cmp( req_ptr->ssd_a, nitem.ssd_a.ssd ) == 0 ) {
          rsp_ptr->ssd_ok = TRUE;
        }
        else {
          /* If failed match, make DIAG timeout 10 seconds (just like SPC) */
          rsp_ptr->ssd_ok = FALSE;
          break;
        }
      } /* nv read SSD_A */
    }
    else if( req_ptr->ssd_select == DIAG_SSD_B) {
      /* Read SSD_B from NV and check against challenge */
      if ( diag_nv_read ( NV_SSD_B_I, &nitem ) != NV_DONE_S ) {
        MSG_HIGH("Failed to read SSD_B", 0, 0, 0);
        /* Since we couldn't check it, assume it was wrong (but don't make diag
        ** timeout)
        */
        break;
      }
      else {
        if (qw_cmp( req_ptr->ssd_b, nitem.ssd_b.ssd ) == 0)
        {
          rsp_ptr->ssd_ok = TRUE;
        }
        else
        {
          rsp_ptr->ssd_ok = FALSE;
          break;
        }
      } /* nv read SSD_B */
    }
    else {
      MSG_ERROR(" Bad SSD Select entry ",0,0,0);
      return 0;
    }

  } while (0);

  if (rsp_ptr->ssd_ok == FALSE)
  {
    diagpkt_commit (rsp_ptr);
    diag_timeout ();
    rsp_ptr = NULL;
  }

  return (rsp_ptr);

} /* cdma2kdiag_ssd_verify */


#ifdef FEATURE_DIAG_COUPLED_ARCHITECTURE

#define DISPATCH_DECLARE(func) \
extern PACKED void * func(PACKED void *req_pkt, word len)

DISPATCH_DECLARE (mclog_stat);
DISPATCH_DECLARE (mclog_get_is2k_status);
DISPATCH_DECLARE (mclog_markov);
DISPATCH_DECLARE (mclog_markov_reset);
DISPATCH_DECLARE (parm_diag_retrieve);
DISPATCH_DECLARE (parm_diag_legacy_get);
DISPATCH_DECLARE (parm_diag_set);
DISPATCH_DECLARE (parm_diag_sup_fer);

#if defined (FEATURE_IS2000_TDSO)
DISPATCH_DECLARE (tdso_clear_stats);
#endif

#if defined (FEATURE_RLP_LOGGING)
DISPATCH_DECLARE (rxc_clear_rlp_statistics);
#endif

#ifdef FEATURE_DIAG_RPC
#error code not present
#endif

#endif

#if !defined(IMAGE_APPS_PROC)
#ifndef FEATURE_HWTC
static const diagpkt_user_table_entry_type cdma2kdiag_tbl[] =
{
#if defined (FEATURE_DIAG_COUPLED_ARCHITECTURE)
  {DIAG_STATUS_F, DIAG_STATUS_F, mclog_stat},
  {DIAG_IS2000_STATUS_F, DIAG_IS2000_STATUS_F, mclog_get_is2k_status},
  {DIAG_MARKOV_F, DIAG_MARKOV_F, mclog_markov},
  {DIAG_MARKOV_RESET_F, DIAG_MARKOV_RESET_F, mclog_markov_reset},
  {DIAG_PARM_RETRIEVE_F, DIAG_PARM_RETRIEVE_F, parm_diag_retrieve},
  {DIAG_PARM_GET2_F, DIAG_PARM_GET2_F, parm_diag_legacy_get},
  {DIAG_PARM_SET_F, DIAG_PARM_SET_F, parm_diag_set},

#if defined(FEATURE_IS95B_MDR)
  {DIAG_SUP_FER_F, DIAG_SUP_FER_F, parm_diag_sup_fer},
  {DIAG_PARM_GET_IS95B_F, DIAG_PARM_GET_IS95B_F, parm_diag_sup_fer},
#endif

#if defined (FEATURE_IS2000_TDSO)
  {DIAG_TDSO_STAT_RESET_F, DIAG_TDSO_STAT_RESET_F, tdso_clear_stats},
#endif

#if defined (FEATURE_RLP_LOGGING)
  {DIAG_RLP_STAT_RESET_F, DIAG_RLP_STAT_RESET_F, rxc_clear_rlp_statistics},
#endif

#endif /* FEATURE_DIAG_COUPLED_ARCHITECTURE */

  {DIAG_TAGRAPH_F, DIAG_TAGRAPH_F, cdma2kdiag_tagraph},
  {DIAG_TA_PARM_F, DIAG_TA_PARM_F, cdma2kdiag_ta_parm},
  {DIAG_SER_REPORT_F, DIAG_SER_REPORT_F, cdma2kdiag_ser_report},
  {DIAG_SER_RESET_F, DIAG_SER_RESET_F, cdma2kdiag_ser_reset},
  {DIAG_TEST_F, DIAG_TEST_F, cdma2kdiag_test},
  {DIAG_STATE_F, DIAG_STATE_F, cdma2kdiag_state},
  {DIAG_PILOT_SETS_F, DIAG_PILOT_SETS_F, cdma2kdiag_pilot_sets},

  {DIAG_AKEY_VERIFY_F, DIAG_AKEY_VERIFY_F, cdma2kdiag_akey_verify},
#ifndef IMAGE_APPS_PROC
  {DIAG_STATUS_SNAPSHOT_F, DIAG_STATUS_SNAPSHOT_F, cdma2kdiag_status_snapshot},
#endif /* IMAGE_APPS_PROC */
  {DIAG_TMOB_F, DIAG_TMOB_F, cdma2kdiag_tmob},

#if defined(FEATURE_IS95B_MDR)
  {DIAG_SUP_WALSH_CODES_F, DIAG_SUP_WALSH_CODES_F, cdma2kdiag_get_walsh_codes},
  {DIAG_SET_MAX_SUP_CH_F, DIAG_SET_MAX_SUP_CH_F, cdma2kdiag_set_max_sup_ch},
#endif /* defined(FEATURE_IS95B_MDR) */

#ifdef FEATURE_DIAG_RPC
#error code not present
#endif

  {DIAG_SSD_VERIFY_F, DIAG_SSD_VERIFY_F, cdma2kdiag_ssd_verify},

};

#else
#error code not present
#endif
#endif


#ifdef __cplusplus
  
  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, cdma2kdiag_tbl);

} /* extern "C" */

#else

#if defined (FEATURE_DIAG_MANUAL_DISPATCH_TABLE)
extern void fsdiag_init (void);
extern void hsdiag_init (void);
extern void nvdiag_init (void);
extern void vocdiag_init (void);
#ifdef FEATURE_GPSONE
#error code not present
#endif


#endif /* FEATURE_DIAG_MANUAL_DISPATCH_TABLE */

void cdma2kdiag_init (void)
{
#if !defined(IMAGE_APPS_PROC)
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, cdma2kdiag_tbl);
#endif

#if defined (FEATURE_DIAG_MANUAL_DISPATCH_TABLE)
  fsdiag_init ();
  hsdiag_init ();
#if !defined(IMAGE_APPS_PROC)
  nvdiag_init ();
#endif
  #if !defined (FEATURE_HWTC) && !defined(IMAGE_APPS_PROC)
  vocdiag_init ();
  #endif /* !defined (FEATURE_HWTC) && !defined(IMAGE_APPS_PROC) */

#if !defined(IMAGE_APPS_PROC) || (defined(IMAGE_APPS_PROC) && !defined(FEATURE_MODEM_PDCOMM))
#if defined (FEATURE_GPSONE)
#error code not present
#endif
#endif

#endif /* FEATURE_DIAG_MANUAL_DISPATCH_TABLE */

}

#endif /* __cplusplus */

#endif
