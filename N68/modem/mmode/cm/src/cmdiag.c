/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 C A L L   M A N A G E R   D I A G N O S T I C S   P A C K E T   M O D U L E

GENERAL DESCRIPTION
  This module contains function to support diagnostic packet processing.

  These functions support
  1. Call origination (CDMA subsystem).
  2. Call end (CDMA subsystem).
  3. Change of operating mode.


EXTERNALIZED FUNCTIONS

  cmdiag_init
    Registers the callback functions for call origination, call end and
    change of operating mode with the diagnostic monitor.



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmdiag_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 1991 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                           Edit History

$Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmdiag.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/09/09   aj      Added support for DIAG_MEAS_MODE_F and DIAG_MEAS_REQ_F 
02/15/09   np      Adding multiple country entry message
11/04/08   cl      Adding DIAG support for SUPS Command.
09/15/08   np      Added test code of Assisted dialing
08/20/08   st      Resolve Klockwork defects
12/07/07   ra      Added support for IMS VCC in HOMER files 
11/20/07   sv      Fixed CR#130667 - Reverted back to using reg->cnt for 
                   the dailed_digits length 
08/07/07   ic      Lint clean up
05/23/07   yc/ic   Fixed compile error
05/11/06   sk      Added support for originating 4GV_NB calls.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/09/06   ic      Adjusted for cm_num_init() / cm_num_copy() changes in cm.h
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/21/05   sk      Added "+" as a valid number for calls made through the
                   diag interface.
06/02/05   ic      Mainlined FEATURE_FACTORY_TESTMODE
04/26/05   pk      Added CMDIAG_MODE_POWER_OFF_F to the diag mode. This mode
                   powers down the phone. Command to be used in diag is
                   send_data 41 6 0
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
03/29/04   ic      Added support for FEATURE_36_DIGIT_DIAL_STRING.
                   In cmdiag_orig_req() replaced NV_MAX_DIAL_DIGITS with
                   CMDIAG_NV_MAX_DIAL_DIGITS.
                   NV_MAX_DIAL_DIGITS may be greater than 32 if
                   FEATURE_36_DIGIT_DIAL_STRING is defined and we want to make
                   sure we use 32 in cmdiag_orig_req()
03/15/04   ka      Merged from 6250. Fixed Lint errors.
02/06/04   ws      Initial jaguar/mainline merge.
11/04/03   ka      Fixed a problem in call end where the list
                   of calls to be ended was not being filled
                   in sequentially.
10/31/03   ws      Removed T_RF_ZRF6200 feature.
09/29/03   vt      Added logic to end all active calls instead of just one.
07/24/03   vt      removed FEATURE_SPECIAL_MDR
04/28/03   AT      General Lint Cleanup.
02/27/03   prk     Updated file header and removed compiler warnings.
02/20/03   vt      Compiles orig & end functions for CDMA only.
02/14/03   vt      Fixed a compile problem.
02/14/03   lad     Moved mode change functionality from DIAG.  CM handles all
                   mode information.
08/29/02   lad     Created file from former diagp.c.
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "customer.h"
#include "cm.h"
#include "comdef.h"
#include "cmcall.h"
#include "cmdiag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "msg.h"
#include "task.h"
#include "ftm.h"
#include "cmdbg.h" 
#include "cmutil.h"

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
/* Call Id made Local to access from orig & end. */
static cm_call_id_type call_id = 0;
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

/*===========================================================================

FUNCTION CMDIAG_CONTROL

DESCRIPTION
  This procedure processes a request to change modes of operation in the
  DMSS.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void *cmdiag_control (PACKED void *req_pkt, uint16 pkt_len)
/*lint -esym(765,cmdiag_control)
** Can't be static, called by DIAG via cmdiag_tbl
*/ 
{

  cmdiag_mode_control_req_type *req = NULL;
  cmdiag_mode_control_rsp_type *rsp;
  const int rsp_len = sizeof (cmdiag_mode_control_rsp_type);

  sys_oprt_mode_e_type oprt_mode = SYS_OPRT_MODE_NONE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_mode_control_req_type *) req_pkt;

  MSG_MED ("Received mode change to %d", req->mode, 0, 0);

  switch (req->mode)
  {
    case CMDIAG_MODE_OFFLINE_A_F:

      #ifdef FEATURE_ACP
#error code not present
      #endif              

    case CMDIAG_MODE_OFFLINE_D_F:

      oprt_mode = SYS_OPRT_MODE_OFFLINE_CDMA;

      if (ftm_mode == FTM_MODE)
      {
        /* Instruct the task service to set the system to offline state. */
        task_offline ();
      }

      #if defined(FEATURE_HWTC) || defined(FEATURE_WCDMA_PLT)
#error code not present
      #endif
      break;

    case CMDIAG_MODE_FTM_F:
      oprt_mode = SYS_OPRT_MODE_FTM;
      break;

    case CMDIAG_MODE_ONLINE_F:
      oprt_mode = SYS_OPRT_MODE_ONLINE;
      break;

    case CMDIAG_MODE_LPM_F:
      oprt_mode = SYS_OPRT_MODE_LPM;
      break;

    case CMDIAG_MODE_RESET_F:
      oprt_mode = SYS_OPRT_MODE_RESET;
      break;

    case CMDIAG_MODE_POWER_OFF_F:
      oprt_mode = SYS_OPRT_MODE_PWROFF;
      break;

    default:
      MSG_HIGH ("Received invalid mode control %d", req->mode, 0, 0);
  }

  if (oprt_mode != SYS_OPRT_MODE_NONE)
  {
    #if !defined(FEATURE_HWTC) && !defined(FEATURE_WCDMA_PLT) && !defined(FEATURE_GSM_PLT)
    (void) cm_ph_cmd_oprt_mode (NULL, NULL, CM_CLIENT_ID_ANONYMOUS, oprt_mode);
    #endif

    rsp = (cmdiag_mode_control_rsp_type *)
      diagpkt_alloc (DIAG_CONTROL_F, rsp_len);

    if(rsp==NULL)
    {
        CM_MSG_LOW( "rsp_NULL",0,0,0);
        return NULL;
    }
    rsp->mode = req->mode;
  }
  else
  {
    rsp = (cmdiag_mode_control_rsp_type *)
      diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len);
  }

  return (rsp);
}              

/*===========================================================================
FUNCTION CMDIAG_ORIG_REQ

DESCRIPTION
  Processes a request to originate a call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void *cmdiag_orig_req (PACKED void *req_pkt, word pkt_len)
/*lint -esym(715,req_pkt,pkt_len) */
/*lint -esym(818, req_pkt) */
/*lint -esym(765,cmdiag_orig_req)
** Can't be static / const, called by DIAG via cmdiag_tbl
*/ 
{
  #ifndef FEATURE_HWTC

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  word i;           /* loop index */
  cm_call_type_e_type eCM_Call_Type;

  word cai_so;      /* The IS2000 service option */
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  #endif

  const int rsp_len = sizeof (cmdiag_orig_rsp_type);

  #if !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

  cmdiag_orig_req_type *req = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_orig_req_type *) req_pkt;

  /* Check the service option */
  switch (req->so)
  {
    case CMDIAG_SO_VOICE_96A:
      cai_so = CAI_SO_VOICE_IS96A;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    case CMDIAG_SO_VOICE_4GV_NB:
      cai_so = CAI_SO_VOICE_4GV_NB;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    #ifndef FEATURE_NO_13K_VOICE_SUPPORT
    case CMDIAG_SO_VOICE13:cai_so = CAI_SO_VOICE_13K;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;
    #endif

    case CMDIAG_SO_LOOPBACK:cai_so = CAI_SO_LOOPBACK;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_LOOPBACK_13K:cai_so = CAI_SO_LOOPBACK_13K;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    #ifdef FEATURE_IS2000
    case CMDIAG_SO_LOOPBACK_SO55:cai_so = CAI_SO_LOOPBACK_SO55;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    case CMDIAG_SO_MARKOV:cai_so = CAI_SO_MARKOV;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_MARKOV_13K:cai_so = CAI_SO_MARKOV_13K;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_RS2_MARKOV:cai_so = CAI_SO_RS2_MARKOV;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    case CMDIAG_SO_RS1_MARKOV:cai_so = CAI_SO_RS1_MARKOV;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;

    #ifdef FEATURE_IS2000
    case CMDIAG_SO_MARKOV_SO54:cai_so = CAI_SO_MARKOV_SO54;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    case CMDIAG_SO_EVRC:cai_so = CAI_SO_VOICE_EVRC;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;

    #ifdef FEATURE_SO_VOICE_SMV
    case CAI_SO_VOICE_SMV:cai_so = CAI_SO_VOICE_SMV;
      eCM_Call_Type = CM_CALL_TYPE_VOICE;
      break;
    #endif

    #if defined(FEATURE_IS95B_MDR)
    case CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS1:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS1_RRS1;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_PKT_DATA_FRS1_RRS2:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS1_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS1:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS1_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_PKT_DATA_FRS2_RRS2:
      cai_so = CAI_SO_MDR_PKT_DATA_FRS2_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_DPT_FRS1_RRS1:
      cai_so = CAI_SO_MDR_DPT_FRS1_RRS1;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;

    case CMDIAG_SO_MDR_DPT_FRS2_RRS2:
      cai_so = CAI_SO_MDR_DPT_FRS2_RRS2;
      eCM_Call_Type = CM_CALL_TYPE_DATA;
      break;
    #endif              /* defined(FEATURE_IS95B_MDR) */

    #if defined(FEATURE_IS2000_TDSO)
    /* Currently, DIAG can only originate a TDSO call via CM. */
    case CAI_SO_TDSO:case CAI_SO_FULL_TDSO:cai_so = req->so;
      eCM_Call_Type = CM_CALL_TYPE_TEST;
      break;
    #endif

    case CMDIAG_SO_SMS:cai_so = CAI_SO_SMS;
      eCM_Call_Type = CM_CALL_TYPE_SMS;
      break;

    case CMDIAG_SO_RS2_SMS:cai_so = CAI_SO_RS2_SMS;
      eCM_Call_Type = CM_CALL_TYPE_SMS;
      break;

    case CMDIAG_SO_DATA:
    case CMDIAG_SO_NONE:

    default:
      MSG_HIGH ("%d: Unsupported orig service option!", req->so, 0, 0);
      return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
  }

  /* Check the dial string for valid number of digits */
  if (req->cnt > CMDIAG_NV_MAX_DIAL_DIGITS )
  {
    MSG_MED ("%d: Invalid digit count!", req->cnt, 0, 0);
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len));
  }

  /* Check the dial string for valid digits */
  for (i = 0; i < req->cnt; i++)
  {
    /* Check for dial digits (0 - 9, *, #) */
    if ( !INRANGE(req->dialed_digits[i], '0', '9') &&
                 (req->dialed_digits[i] != '*') &&
                 (req->dialed_digits[i] != '#') &&
                 (req->dialed_digits[i] != '+'))
    {

      /* Was an invalid digit (not 0 - 9, *, #) */
      return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
    }
  }             /* for each key in the string */

  /* Set up parameters to call CM with orig request.
     We have validated the call digits and the SO. */

  /* Send request to CM */
  {
    cm_cdma_orig_params_s_type orig_params;
    cm_num_s_type called_number;
    cm_orig_alpha_s_type alpha;

  /*-----------------------------------------------------------------------*/

    orig_params.activate_code = CM_OTASP_ACT_CODE_NONE;
    orig_params.srv_opt = cai_so;
    #ifdef FEATURE_IS2000
    orig_params.drs_bit = FALSE;
    #endif
    #ifdef FEATURE_IS2000_REL_A
    orig_params.sr_id_included = FALSE;
    orig_params.qos_parms_incl = FALSE;
    #endif
    #ifdef FEATURE_HDR_HANDOFF
#error code not present
    #endif /* FEATURE_HDR_HANDOFF */

    cm_num_init (&called_number);
    cm_num_fill (&called_number, 
                 (const byte *) req->dialed_digits,
                 MIN(req->cnt,ARR_SIZE(req->dialed_digits)),
                 CM_DIGIT_MODE_4BIT_DTMF);
    memcpy (alpha.buf, "DIAG CALL", strlen ("DIAG CALL"));
    alpha.len = (byte) strlen ("DIAG CALL");

    (void) cm_mm_call_cmd_orig (NULL,
                         NULL,
                         CM_CLIENT_ID_ANONYMOUS,
                         eCM_Call_Type,
                         CM_SRV_TYPE_CDMA_SPECIFIC,
                         NULL,
                         &called_number,
                         &alpha, &orig_params, NULL, NULL, &call_id);
  }
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


  #else               /* FEATURE_HWTC defined */
#error code not present
  #endif              /* FEATURE_HWTC */

  #ifdef FEATURE_IMS_VCC
  { 
    cmdiag_orig_req_type *req         = NULL;
    cm_call_id_type       ret_call_id = CM_CALL_ID_INVALID;
  
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    CM_ASSERT(req_pkt != NULL); 

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    req = (cmdiag_orig_req_type *) req_pkt;

    if ( req->dialed_digits[0] == '*' &&
         req->dialed_digits[1] == '3' &&
         req->dialed_digits[2] == '2' &&
         req->dialed_digits[3] == '1' &&
         req->dialed_digits[4] == '0' )
    {
      (void)cm_mm_call_cmd_handover_req(
        NULL, NULL, CM_CLIENT_ID_ANONYMOUS, CM_CALL_TYPE_VOICE, 
        CM_CALL_STATE_CONV, &ret_call_id);        
    }
  }
  #endif

  return diagpkt_alloc (DIAG_ORIG_F, rsp_len);


}              
/*lint +esym(715,req_pkt,pkt_len) */
/*lint +esym(818, req_pkt) */

/*===========================================================================
FUNCTION CMDIAG_END_REQ

DESCRIPTION
  Processes a request to end a call.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void *cmdiag_end_req (PACKED void *req_pkt, word pkt_len)
/*lint -esym(715,pkt_len)*/
/*lint -esym(715,req_pkt)*/
/*lint -esym(818,req_pkt) nope as diagpkt_user_table_entry_type phohibits that */
/*lint -esym(765,cmdiag_end_req)
** Can't be static, called by DIAG via cmdiag_tbl
*/ 
{
  cmdiag_end_rsp_type *rsp;
  const int rsp_len = sizeof (cmdiag_end_rsp_type);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if !defined(FEATURE_HWTC) && !defined(FEATURE_GSM_PLT)
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  {
    int i;
    uint8 num_call_ids = 0;
    cm_end_params_s_type end_params[CM_CALL_ID_MAX];

    for (i = 0; i < CM_CALL_ID_MAX; i++)
    {
      if ( cm_call_id_is_allocated ((cm_call_id_type) i) )
      {

         end_params[num_call_ids].call_id = (cm_call_id_type) i;
         end_params[num_call_ids].info_type = CM_CALL_MODE_INFO_CDMA;
         end_params[num_call_ids].end_params.cdma_end.end_reason_included = FALSE;
         end_params[num_call_ids].end_params.cdma_end.end_reason = CAI_REL_NORMAL;
         MSG_HIGH ("CMDIAG::END_REQ: Adding call_id %d to the end list",
           end_params[i].call_id, 0, 0);
         num_call_ids++;
      } /* if ( cm_call_id_is_allocated ((cm_call_id_type) i) ) */
    } /* for (i = 0; i < CM_CALL_ID_MAX; i++) */

    if ( num_call_ids > 0 )
    { /*lint -esym(645, end_params) */
      (void) cm_mm_call_cmd_end( NULL, NULL, (cm_client_id_type) -1,
                               num_call_ids, end_params );
      /*lint +esym(645, end_params) */
    }
  }
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  #endif /* FEATURE_HWTC */

  rsp = (cmdiag_end_rsp_type *) diagpkt_alloc (DIAG_END_F, rsp_len);

  return (rsp);
}               /* cmdiag_end_req */
/*lint +esym(715,pkt_len)*/
/*lint +esym(715,req_pkt)*/
/*lint +esym(818,req_pkt) nope as diagpkt_user_table_entry_type phohibits that */

/*===========================================================================

FUNCTION CMDIAG_CNTRY_INFO_REQ

DESCRIPTION
  This procedure processes a request to country network information for
  Assisted dialing. This is just test purpose. Not necessary to check in.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void *cmdiag_cntry_info_req (PACKED void *req_pkt, uint16 pkt_len)
/*lint -esym(715,req_pkt,pkt_len) */
/*lint -esym(818,req_pkt) */
/*lint -esym(765,cmdiag_cntry_info_req)*/
/*Can't be static, called by DIAG via cmdiag_tbl */ 
/*lint -esym(550, req_mcc)*/
{
  cmdiag_cntry_info_rsp_type  *rsp;

  const int rsp_len = sizeof (cmdiag_cntry_info_rsp_type);

#if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)

  cmdiag_cntry_info_req_type  *req = NULL;

  cmutil_multi_country_specific_record_s country_info;

  int  loop_found_entry;

  cm_country_code_type req_mcc;
  /* Local variable for Request MCC */

  
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  req = (cmdiag_cntry_info_req_type *) req_pkt;

  MSG_MED ("Received mcc fro cntry info req= %d", req->mcc, 0, 0);

  req_mcc = req->mcc; 
  
  rsp = (cmdiag_cntry_info_rsp_type *)
      diagpkt_alloc (DIAG_CNTRY_INFO_F, rsp_len);

  if(rsp==NULL)
  {
        CM_MSG_LOW( "rsp_NULL",0,0,0);
        return NULL;
  }
  if ( cm_util_country_info_for_dialing(req->mcc,&country_info)==
      CMUTIL_COUNTRY_INFO_RESULT_FOUND)
  {
     rsp->sid1 = country_info.country_entry[0].sid1;
     rsp->sid2 = country_info.country_entry[0].sid2;
     rsp->country_code = country_info.country_entry[0].country_code;   

     memcpy((char *)rsp->inter_dialed_digits,
           (char *)country_info.country_entry[0].inter_dialed_digits,
           CMUTIL_IDD_DIGIT_LEN);
     
     /* idd digits start after last F */
     memcpy((char *)rsp->special_prefix_code,
          (char *)country_info.country_entry[0].special_prefix_code,
          CMUTIL_SPC_DIGIT_LEN); 
     
     /* spc */
     memcpy((char *)rsp->national_direct_dial,
           (char *)country_info.country_entry[0].national_direct_dial,
           CMUTIL_NDD_DIGIT_LEN); 
     
     /* ndd */

     rsp->is_network_soln=
                    (byte)country_info.country_entry[0].is_network_soln; 
     /* network solution */
     
     rsp->mobile_country_code= 
              country_info.country_entry[0].mobile_country_code ;   
     /* Mobile country code */

    for (loop_found_entry=0; loop_found_entry< country_info.entry_number; 
          ++loop_found_entry)
    {
      
      CM_MSG_HIGH ("Diag : Assisted Dialing Api Testing", 0, 0, 0);
        
      CM_MSG_HIGH ("Diag: req_mcc= %d found=%d", req_mcc, 
        country_info.country_entry[loop_found_entry].mobile_country_code, 0);

      CM_MSG_HIGH ("Diag:sid1=%d sid2=%d country_code=%d", 
         country_info.country_entry[loop_found_entry].sid1,
         country_info.country_entry[loop_found_entry].sid2, 
         country_info.country_entry[loop_found_entry].country_code);

      CM_MSG_HIGH ("Diag: idd=%s spc=%s ndd=%s", 
         country_info.country_entry[loop_found_entry].inter_dialed_digits, 
         country_info.country_entry[loop_found_entry].special_prefix_code, 
         country_info.country_entry[loop_found_entry].national_direct_dial);

      CM_MSG_HIGH ("Diag: network solution available=%d",
          country_info.country_entry[loop_found_entry].is_network_soln,0,0);
    } 
      
  }
#else
  rsp = (cmdiag_cntry_info_rsp_type *)
  diagpkt_alloc (DIAG_CNTRY_INFO_F, rsp_len);
#endif
  return (rsp);
  
}              
/*lint -esym(715,pkt_len)*/
/*lint +esym(715,req_pkt)*/

#ifdef FEATURE_CM_SS_MEAS
#error code not present
#endif /* FEATURE_CM_SS_MEAS */



/*===========================================================================
FUNCTION CMDIAG_SUPS_REQ

DESCRIPTION
  Processes a Suplementary Service Request

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void *cmdiag_sups_req (PACKED void *req_pkt, word pkt_len)
/*lint -esym(715,req_pkt,pkt_len) */
/*lint -esym(818,req_pkt) */
/*lint -esym(765,cmdiag_sups_req)
** Can't be static / const, called by DIAG via cmdiag_tbl
*/ 
{
  const int rsp_len = sizeof (cmdiag_sups_rsp_type);
  byte i;
  cmdiag_sups_req_type *req                    = NULL;
  cm_call_sups_params_s_type        sups_param = {0};
  cmcall_s_type         *call_ptr;
  cm_call_sups_type_e_type sups_type;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(req_pkt != NULL); 
  req = (cmdiag_sups_req_type *) req_pkt;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Get Call ID */
  call_ptr = cmcall_get_call_obj_in_call_state(CM_CALL_STATE_CONV);
  if (call_ptr != NULL)
  {
    /* Supplementary Service is originated based on an existing call id */
    sups_param.call_id = call_ptr->call_id;
  }
  else 
  {
    /* In case of there's no existing call, Initialize Call Id = default value */
    sups_param.call_id = 0;
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* ------------Initialize the number field ----------*/
  /* Check the dial string for valid number of digits */
  if (req->cnt > CMDIAG_NV_MAX_DIAL_DIGITS )
  {
    MSG_MED ("%d: Invalid digit count!", req->cnt, 0, 0);
    return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req, pkt_len));
  }

  /* Check the dial string for valid digits */
  for (i = 0; i < req->cnt; i++)
  {
    /* Check for dial digits (0 - 9, *, #) */
    if ( !INRANGE(req->dialed_digits[i], '0', '9') &&
                 (req->dialed_digits[i] != '*') &&
                 (req->dialed_digits[i] != '#') &&
                 (req->dialed_digits[i] != '+'))
    {

      /* Was an invalid digit (not 0 - 9, *, #) */
      return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
    }
  }             /* for each key in the string */

  /* Copy the numbers from diag packets over */
  cm_num_init(&sups_param.sups_number);
  cm_num_fill (&sups_param.sups_number, 
                 (const byte *) req->dialed_digits,
                 MIN(req->cnt,ARR_SIZE(req->dialed_digits)),
                 CM_DIGIT_MODE_4BIT_DTMF);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get sups type */
  sups_type = (cm_call_sups_type_e_type)req->sups_type;

  switch (sups_type)
  {

    case CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB:
    case CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL:
    case CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL:
    case CM_CALL_SUPS_TYPE_HOLD_CALL:
    case CM_CALL_SUPS_TYPE_SELECT_CALL_X:
    case CM_CALL_SUPS_TYPE_MULTIPARTY_CALL:
    case CM_CALL_SUPS_TYPE_CALL_DEFLECTION:
    case CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER:
    case CM_CALL_SUPS_TYPE_CALL_FORWARD:
    case CM_CALL_SUPS_TYPE_EMERGENCY_CALL:
    case CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD:
    case CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD:
    case CM_CALL_SUPS_TYPE_SIMPLE_FLASH:
    case CM_CALL_SUPS_TYPE_ACT_CCBS_REQ:

      (void) cm_mm_call_cmd_sups(NULL, NULL, CM_CLIENT_ID_ANONYMOUS,
        sups_type,&sups_param);
      break;

    case CM_CALL_SUPS_TYPE_NONE:
    case CM_CALL_SUPS_TYPE_MAX:
    default:
      CM_MSG_HIGH(" Invalid options sups_type = %d", sups_type, 0 ,0);
      break;
  }

  return diagpkt_alloc (DIAG_SUPS_REQ_F, rsp_len);


}              /*cmdiag_sups_req*/
/*lint +esym(715,req_pkt,pkt_len) */
/*lint +esym(818, req_pkt) */


/* Dispatch table to register with the diagnostics packet service. */
static const diagpkt_user_table_entry_type cmdiag_tbl[] = {
  {DIAG_ORIG_F, DIAG_ORIG_F, cmdiag_orig_req},
  {DIAG_END_F, DIAG_END_F, cmdiag_end_req},
  {DIAG_CONTROL_F, DIAG_CONTROL_F, cmdiag_control},
  {DIAG_CNTRY_INFO_F, DIAG_CNTRY_INFO_F, cmdiag_cntry_info_req},
  {DIAG_SUPS_REQ_F, DIAG_SUPS_REQ_F, cmdiag_sups_req},
  #ifdef FEATURE_CM_SS_MEAS
#error code not present
  #endif
};


#ifdef __cplusplus

DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, cmdiag_tbl);

#else

void cmdiag_init (void)
{
  /*
  ** 717: do ... while(0) used by DIAG so we supress it
  */
  /*lint -save -e717 */
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, cmdiag_tbl);
  /*lint -restore */
}

#endif

#ifdef __cplusplus
}
#endif
