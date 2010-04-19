/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C A L L   M A N A G E R   M M G S D I   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager MMGSDI Interface block, which groups
  all the functionality that is a associated with CM<->MMGSDI interfacing.




EXTERNALIZED FUNCTIONS
  *cm_mmgsdi_client_id_ptr
  cmmmgsdi_ok_to_refresh
  cmmmgsdi_refresh_files_read_proc
  cmmmgsdi_read
  cmmmgsdi_read_preferred_plmn_list
  cmmmgsdi_write_preferred_plmn_list
  cmmmgsdi_refresh_init
  cmmmgsdi_validate_received_file_list
  cmmmgsdi_update_rat_mode
  cmmmgsdi_cmals_mmgsdi_async_sim_write
  cmmmgsdi_cmals_mmgsdi_async_sim_read
  cm_mmgsdi_in_traffic_channel_w_mode
  cmmmgsdi_card_status_cb
  cmmmgsdi_client_id_reg_cb
  cmmmgsdi_init


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cm_init_after_task_start() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2008 - 2012 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmmmgsdi.c#14 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   rm      Changing signature of cmmmgsdi_rat_mode_init
01/04/09   am      fixing memset error.
03/31/09   rm      Updating correct number of files to read in 
                   cmmmgsdi_get_files_used_incall
03/11/09   sv      Correct Lint errors
02/25/09   sv      Correct Lint errors
02/13/09   rm      Putting cmmmgsdi_is_refresh_list_used_by_cm under 
                   FEATURE_GSTK
01/29/09   rm      RAT balancing phase-2 - Vote YES to EF_RAT refresh during
                   CS/PS calls - MMGSDI changes
01/08/09   rm      Reading EF_RAT contents in 'transparent' mode
12/08/08   am      Fixing Compilation Warning
11/25/08   am      Adding HIGH messages to trace the MMGSDI Refresh and
                   Card Status events.
11/25/08   am      Main lining, feature for 1X voting from MMGSDI
11/25/08   am      Fixing compilation warnings
11/21/08   am      Putting file_info and no_option under feature and 
                   defining cmmmgsdi_dummy() to get rid of compiler warnings
11/21/08   am      Fixing compilation error for Running off-target tests
11/18/08   am      fixing RAT balancing - Honor EF_RAT file changes while 
                   power up on MMGSDI side.
11/17/08   am      Using MMGSDI API in CM.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"

#if ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
#include "cmidbg.h"
#include "cmph.h"
/*lint -esym(766,cmtask.h)*/
#include "cmtask.h"
#include "cmmmgsdi.h"
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cmxll.h"
#endif /*  if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
#if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
#endif  /* #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/**--------------------------------------------------------------------------
**      Static data
**---------------------------------------------------------------------------
*/

#if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
#endif  /* #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cm_mmgsdi_client_id_ptr

DESCRIPTION

  Return a pointer to cm_mmgsdi_client_id, which contains the client ID 
  provided by MMGSDI to CM.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to client ID provided by MMGSDI to CM.

SIDE EFFECTS
  none

===========================================================================*/
mmgsdi_client_id_type  *cm_mmgsdi_client_id_ptr( void )
{
    static mmgsdi_client_id_type   cm_mmgsdi_client_id;

    /* - - - - - - - - - - - - - - - - - - - - - - - - -*/

    return &cm_mmgsdi_client_id;
}  /* cm_mmgsdi_client_id_ptr() */

#ifdef FEATURE_GSTK
/*==========================================================================

FUNCTION CMMMGSDI_GET_FILES_USED_INCALL

DESCRIPTION
  Returns the list of SIM files used by CM in a call

RETURN VALUE
  Pointer to list of SIM files.
  Length of SIM file list.

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_get_files_used_incall
(
  mmgsdi_refresh_file_list_type      *file_info_ptr
    /* File info */

)
{
  static mmgsdi_file_enum_type file_list[CM_MMGSDI_NUM_REGISTRED_FILES] = {
              MMGSDI_GSM_ACM,
              MMGSDI_GSM_ACM_MAX,
              MMGSDI_GSM_PUCT,
              MMGSDI_USIM_ACM,
              MMGSDI_USIM_ACM_MAX,
              MMGSDI_USIM_PUCT,
              MMGSDI_USIM_7F66_PROP1_RAT };

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  CM_ASSERT (file_info_ptr != NULL);

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  file_info_ptr->num_files = ARR_SIZE (file_list);

  file_info_ptr->file_list_ptr = file_list;

  return;

}  /* cmmmgsdi_get_files_used_incall () */

/*==========================================================================

FUNCTION CMMMGSDI_GET_FILES_FOR_REFRESH

DESCRIPTION
  Returns the list of SIM files that are registerd by CM 
  for refresh notification.

RETURN VALUE
  Pointer to list of SIM files.
  Length of SIM file list.

DEPENDENCIES
  None
===========================================================================*/
static void cmmmgsdi_get_files_for_refresh
(
  mmgsdi_refresh_file_list_type      *file_info_ptr
    /* File info */

)
{
  static mmgsdi_file_enum_type file_list[CM_MMGSDI_NUM_REFRESH_FILES] = {
              MMGSDI_GSM_ACM,
              MMGSDI_GSM_ACM_MAX,
              MMGSDI_GSM_PUCT,
              MMGSDI_USIM_ACM,
              MMGSDI_USIM_ACM_MAX,
              MMGSDI_USIM_PUCT };

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  CM_ASSERT (file_info_ptr != NULL);

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/

  file_info_ptr->num_files = ARR_SIZE (file_list);

  file_info_ptr->file_list_ptr = file_list;

  return;

}  /* cmmmgsdi_get_files_for_refresh () */

/*===========================================================================

FUNCTION CMMMGSDI_GENERIC_CB

DESCRIPTION
  Called by MMGSDI when it has completed servicing CM's REEQUEST. This is a
  generic handler for callbacks used for MMGSDI APIs

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(715,cnf)*/
static void cmmmgsdi_generic_cb(
    mmgsdi_return_enum_type       status,
    mmgsdi_cnf_enum_type          cnf,
    const mmgsdi_cnf_type         *cnf_ptr
)
{

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (status == MMGSDI_SUCCESS)
  {
    switch (cnf_ptr->refresh_cnf.orig_refresh_req)
    {
      case MMGSDI_REFRESH_ORIG_REG_REQ:
        CM_MSG_HIGH("MMGSDI suceed reg CM refsh evnt.Status %d, Confirmation %d",
               status, cnf, 0);
        break;

      case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
        CM_MSG_HIGH("Ok to refsh Ack succeed %d %d", cnf, status, 0);
        break;

      default:
        CM_MSG_ERROR("MMGSDI ret unexpected cnf.orig_refresh_req %d, Confirmation %d",
               cnf_ptr->refresh_cnf.orig_refresh_req, cnf, 0);
        break;

    }
  }
  else /* status != MMGSDI_SUCCESS */
  {
    switch (cnf_ptr->refresh_cnf.orig_refresh_req)
    {
      case MMGSDI_REFRESH_ORIG_REG_REQ:
        CM_MSG_HIGH("MMGSDI failed reg CM refsh evnt.Status %d, Confirmation %d",
               status, cnf, 0);
        break;

      case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
        CM_MSG_HIGH("Ok to refresh Ack fail %d %d", cnf, status, 0);
        break;

      default:
        CM_MSG_ERROR("MMGSDI ret unexpected cnf. Status %d, Confirmation %d",
               status, cnf, 0);
        break;
    }
  }  /* status != MMGSDI_SUCCESS */
}  /* cmmmgsdi_generic_cb() */
/*lint +esym(715,cnf)*/

/*===========================================================================

FUNCTION CMMMGSDI_OK_TO_REFRESH

DESCRIPTION
  This function called when Refresh Voting is needed. This function
  sends Refresh vote to MMGSDI.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_ok_to_refresh
(
  mmgsdi_slot_id_enum_type       slot,
  boolean                        ok_to_refresh_to_mmgsdi
)
{
  mmgsdi_option_type             no_option = {0};

  /* - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/

  if(mmgsdi_ok_to_refresh(
              *cm_mmgsdi_client_id_ptr(),
              slot,
              ok_to_refresh_to_mmgsdi,
              no_option,
              cmmmgsdi_generic_cb,
              0
              ) != MMGSDI_SUCCESS)
  {
    CM_MSG_ERROR("Fail put vote for ok to refresh in MMGSDI q",
             0, 0, 0);
  }
}  /* cmmmgsdi_ok_to_refresh() */

#endif /* #ifdef FEATURE_GSTK */


#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif  /*  (defined (FEATURE_WCDMA) || defined (FEATURE_GSM) ) */

#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#endif  /* (defined FEATURE_ALS) && (defined FEATURE_GSM) */

/*===========================================================================
FUNCTION CM_MMGSDI_IN_TRAFFIC_CHANNEL_W_MODE

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

  in_traffic is a boolean which indicates whether we are in traffic or not.
  mode is a indicates whether the call is GSM / UMTS or 1X.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  None
===========================================================================*/
void cm_mmgsdi_in_traffic_channel_w_mode(
  boolean in_traffic,
  sys_sys_mode_e_type mode)
{
  gsdi_in_tc_mode_enum_type gsdi_mode = GSDI_NOT_IN_TC;

  switch ( mode )
  {
    case SYS_SYS_MODE_AMPS:
    case SYS_SYS_MODE_CDMA:
      gsdi_mode = GSDI_IN_TC_1X;
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
      gsdi_mode = GSDI_IN_TC_GW;
      break;

    default:
      MSG_ERROR ("Unhandled Mode: %x",mode,0,0);
      return;
  }

  gsdi_cm_in_traffic_channel_w_mode(in_traffic, gsdi_mode);
}

/*===========================================================================
FUNCTION CMMMGSDI_CARD_STATUS_CB

DESCRIPTION
  Callback function for MMGSDI generated event.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_card_status_cb
(
  const mmgsdi_event_data_type *event
)
{
  #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
  #else /* !(defined (FEATURE_WCDMA) || defined (FEATURE_GSM)) */ 
  #ifdef FEATURE_GSTK
  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_mc_rpt_type     *mc_rpt_ptr;
  #endif /*  (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
  #endif /* ifdef FEATURE_GSTK */
  #endif /* !(defined (FEATURE_WCDMA) || defined (FEATURE_GSM)) */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if (event->evt == MMGSDI_CARD_ERROR_EVT ||
      event->evt == MMGSDI_CARD_REMOVED_EVT ||
      event->evt == MMGSDI_CARD_INIT_COMPLETED_EVT
      #ifdef FEATURE_GSTK
                             ||
      ( (event->evt == MMGSDI_REFRESH_EVT) 
                             &&
        (event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_INIT ||
         event->data.refresh.stage == MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN ||
         event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START) )

      #endif  /* FEATURE_GSTK */
     )
  {
    #ifdef FEATURE_GSTK
    if (event->evt == MMGSDI_REFRESH_EVT)
    {
      CM_MSG_HIGH("MMGSDI_REFRESH_EVT, mode %d stage %d",
                   event->data.refresh.mode, event->data.refresh.stage, 0);
      #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
      #else  /* !(defined (FEATURE_WCDMA) || defined (FEATURE_GSM)) */
      #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      {
        mc_rpt_ptr = cm_mc_rpt_get_buf_else_err_fatal();
        memcpy(&(mc_rpt_ptr->voting_mmgsdi.refresh),
               &(event->data.refresh),
               sizeof(mmgsdi_refresh_evt_info_type));

        mc_rpt_ptr->voting_mmgsdi.hdr.cmd = CM_MMGSDI_REFRESH_FCN_IND_F;
        /* Queue the report to CM */
        cm_mc_rpt(mc_rpt_ptr);

      }
      #endif  /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
      #endif  /* !(defined (FEATURE_WCDMA) || defined (FEATURE_GSM)). */
    }
    #endif  /* FEATURE_GSTK */

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM) )
#error code not present
    #endif  /* #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM) ) */
  }  /* else if (event->evt... */
  else
  {
    CM_MSG_HIGH("unsupported event: %d", event->evt, 0, 0);
  }
}  /* cmmmgsdi_card_status_cb() */

/*===========================================================================
FUNCTION CMMMGSDI_EVENT_REG_CB

DESCRIPTION
  Callback function from MMGSDI to acknowledge the CM registration for events

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
/*lint -esym(715,cnf)*/
/*lint -esym(715,cnf_ptr)*/
static void cmmmgsdi_event_reg_cb(
  mmgsdi_return_enum_type   status,
  mmgsdi_cnf_enum_type      cnf,
  const mmgsdi_cnf_type     *cnf_ptr
  )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (status != MMGSDI_SUCCESS)
  {
    CM_MSG_ERROR("MMGSDI fail reg CM evnt handler. Status %d, Confirmation %d",
                  status, cnf, 0);/*lint !e641 */
  }
}  /* cmmmgsdi_event_reg_cb() */
/*lint +esym(715,cnf)*/
/*lint +esym(715,cnf_ptr)*/

/*===========================================================================
FUNCTION CMMMGSDI_CLIENT_ID_REG_CB

DESCRIPTION
  Callback function from MMGSDI to provide client ID. CM also register with
  MMGSDI for events in this funtion.

DEPENDENCIES
  mmgsdi_client_id_reg() should be called before.

SIDE EFFECTS
  None
===========================================================================*/
void cmmmgsdi_client_id_reg_cb(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
  )
{
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;
  static boolean                 cm_reg_for_mmgsdi_event_yet = FALSE;
  #ifdef FEATURE_GSTK
  mmgsdi_refresh_file_list_type  file_info;
  mmgsdi_option_type             no_option = {0};
  #endif /* FEATURE_GSTK */
  mmgsdi_client_id_type          *cm_mmgsdi_client_id_provided;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (status != MMGSDI_SUCCESS ||
      cnf != MMGSDI_CLIENT_ID_REG_CNF ||
      cnf_ptr == NULL)
  {
    CM_ERR_FATAL("MMGSDI failed registering CM. Status %d, Confirmation %d",
                  (uint8) status, (int)cnf, 0);
  }
  else
  {
    cm_mmgsdi_client_id_provided = cm_mmgsdi_client_id_ptr();
    memcpy(cm_mmgsdi_client_id_provided,
           &(cnf_ptr->client_id_reg_cnf.response_header.client_id),
           sizeof(mmgsdi_client_id_type));
    CM_MSG_HIGH("MMGSDI reg. CM with Client id : cm_mmgsdi_client_id %d",
                 *cm_mmgsdi_client_id_ptr(), 0, 0);

    /* Registers CM to MMGSDI for event if it is not registered before */
    if(cm_reg_for_mmgsdi_event_yet == FALSE)
    {
      mmgsdi_status = mmgsdi_client_evt_reg(
                              *cm_mmgsdi_client_id_ptr(),
                              cmmmgsdi_card_status_cb,
                              cmmmgsdi_event_reg_cb,
                              0
                              );

      /* This should not happen, raise an error */
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        CM_ERR_FATAL("MMGSDI CM registration for evnt cmd fail : STATUS %d",
                      mmgsdi_status, 0, 0);
      }

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifdef FEATURE_GSTK

      /* Clear file info */
      memset (&file_info, 0, sizeof(file_info));

      /* Get the SIM files that CM works on
      */
      cmmmgsdi_get_files_used_incall (&file_info);

      mmgsdi_status = mmgsdi_register_for_refresh(
                                  *cm_mmgsdi_client_id_ptr(),
                                  MMGSDI_SLOT_1,
                                  file_info,
                                  TRUE,
                                  no_option,
                                  cmmmgsdi_generic_cb,
                                  0
                                  );

      /* This should not happen, raise an error */
      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
        CM_ERR_FATAL( "MMGSDI CM reg for refsh evnt cmd failed : STATUS %d",
        mmgsdi_status, 0, 0);
      }
      cm_reg_for_mmgsdi_event_yet = TRUE;
      #endif /* FEATURE_GSTK */
    }
  }
}  /* cmmmgsdi_client_id_reg_cb() */

/*===========================================================================
FUNCTION CMMMGSDI_INIT

DESCRIPTION
  Initilize CM MGSDI interface.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmmmgsdi_init(void)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Register the MMGSDI event callback function */
  mmgsdi_status = mmgsdi_client_id_reg(cmmmgsdi_client_id_reg_cb, 0);

  /* This should not happen, raise an error */
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR( "MMGSDI CM reg cmd failed : STATUS %d",
                mmgsdi_status, 0, 0 );
  }
}  /* cmmmgsdi_init() */

#ifdef FEATURE_GSTK
/*==============================================================================

FUNCTION NAME CMGSDI_IS_REFRESH_LIST_USED_BY_CM

DESCRIPTION
  Returns the list of SIM files 

RETURN VALUE

  TRUE   - Given list contains files that CM uses during a call
  FALSE  - Given list does NOT CONTAIN files that CM uses during a call.  

DEPENDENCIES
  None
==============================================================================*/
boolean cmmmgsdi_is_refresh_list_used_by_cm  
(

  const mmgsdi_file_enum_type  *file_list_p, 
    /* List of files */

  uint8                       file_list_len
    /* Length of the file list */

)
{
  mmgsdi_refresh_file_list_type    file_info;  
    /* Structure holds information about SIM files CM interacts with */

  int                  file_list       = 0;
    /* counter */

  uint32               cm_file_list    = 0;
    /* counter */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Clear file info */
  memset (&file_info, 0, sizeof(file_info));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the SIM files that CM works on
  */
  cmmmgsdi_get_files_for_refresh (&file_info);  

  for (file_list=0; file_list< file_list_len ; ++file_list)
  {

    for (cm_file_list=0; cm_file_list< file_info.num_files; ++cm_file_list)
    {
      /* Match detected between given list and CM's list, given file is
      ** being used by CM
      */
      if (file_list_p[file_list] == file_info.file_list_ptr[cm_file_list])
      {
        return TRUE;
      }

    } /* for (cm_file_list= ... */
 
  } /* for (file_list=0 ... */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* No match detected */
  return FALSE; 

} /* cm_gsdi_is_refresh_list_used_by_cm () */

#endif /* #ifdef FEATURE_GSTK */

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#endif

#else /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */

/*===========================================================================
FUNCTION cmmmgsdi_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmmmgsdi_dummy( void )
/*lint -esym(714,cmmmgsdi_dummy) */
/*lint -esym(765,cmmmgsdi_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif  /* !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */

