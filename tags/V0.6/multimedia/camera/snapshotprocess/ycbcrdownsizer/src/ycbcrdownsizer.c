/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

    YCBCR DOWNSIZER  MODULE  
  
GENERAL DESCRIPTION
  This module contains functions to apply ycbcr downsizer on image files.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*======*/

/* <EJECT> */
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/ycbcrdownsizer/main/latest/src/ycbcrdownsizer.c#1 $ $Author: karenp $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/10/07   sigu    ARM side changes for integrating the incall downsizer for 6260 target.  
08/07/06   sigu    Initial version.

=============================================================================*/
/* <EJECT> */
/*=============================================================================

                       INCLUDE FILES FOR MODULE


=============================================================================*/


  #include "target.h"             /* Target specific definitions            */
  #include "customer.h"           /* Customer specific definitions          */
  #include "comdef.h"             /* Common definitions                     */ 
  #include "ycbcrdownsizerqdsp.h"
  #include "ycbcrdownsizer.h"
  /*lint -e766 header files not used */
  #include "stdlib.h"
  #include "msg.h"                /* Message logging/reporting services     */
  #include "graph.h"              /* Graphics task typedefs and prototype   */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/*=============================================================================

                Lint Configuration

=============================================================================*/
/*lint -save */
/*lint -e818 parameter could be declared as pointing to const */

/*=============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
=============================================================================*/
static graph_context_req_type context_data;
static ycbcr_downsizer_cmd_info_type local_ycbcr_downsizer_info;
static int32 ycbcr_downsizer_session_id = 0;
static boolean ycbcr_downsizer_in_progress = FALSE;
static boolean ycbcr_downsizer_is_aborting = FALSE;
/*=============================================================================

                STATIC FUNCTION DECLARATIONS
=============================================================================*/
static void ycbcr_downsizer_cleanup (ycbcr_downsizer_msg_type cb_msg);
static void ycbcr_downsizer_setup_parameter (ycbcr_downsizer_data_type * cmd);
static void ycbcr_downsizer_process_cb (int32 client_id, int32 client_data );
static void ycbcr_downsizer_handle_qdsp_cb (ycbcr_downsizer_qdsp_msg_type msg );


/*=============================================================================
Function: YCBCR_DOWNSIZER_PROCESS_DATA

Description: This function will apply ycbcr_downsizer on data provided.

Return Value: 

  YCBCR_DOWNSIZER_ACCEPT_RET    : ycbcr_downsizer command is accepted. Caller must 
                                  wait for callback before freeing any memory location accessible 
                                  by cmd pointer.
  
  YCBCR_DOWNSIZER_CMD_REJECT_RET: ycbcr_downsizer command is NOT accepted, caller is responsible 
                                  to check this status and perform clean up. NO callback will
                                  come from ycbcr_downsizer.
   
Comments: The caller must not free or modify data located in the input buffer
          until callback from ycbcr_downsizer is received.

=============================================================================*/

ycbcr_downsizer_ret_type ycbcr_downsizer_process_data (ycbcr_downsizer_cmd_info_type * cmd )
{
  /* Do not accept command when no callback function is provided */
  if ((cmd != NULL))
  {
    /*Make a local copy of ycbcr_downsizer command info */
    local_ycbcr_downsizer_info = *cmd;
  }
  else
  {
    MSG_ERROR("ycbcr_downsizer: command and call back to ycbcr_downsizer must be defined", 0, 0, 0);
    return YCBCR_DOWNSIZER_CMD_REJECT_RET;
  }
  
  /* Make sure data is not NULL and ycbcr_downsizer operation is enable */
  /* Since we have callback function, ycbcr_downsizer command is accepted and any error */
  /* will send via callback */
  if ((cmd->data.in_Yaddress_frag0.address == NULL) || (cmd->data.in_CbCraddress_frag0.address == NULL) || 
     (cmd->data.out_Yaddress_frag0.address == NULL) || (cmd->data.out_CbCraddress_frag0.address == NULL))
  {
    MSG_HIGH("ycbcr_downsizer: Invalid ycbcr_downsizer command", 0, 0, 0);
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_INVALID_CMD_MSG);
    return YCBCR_DOWNSIZER_CMD_ACCEPT_RET;
  }
    
  /* Make sure that there is only one session of ycbcr_downsizer */
  ycbcr_downsizer_in_progress = TRUE;

  if (ycbcr_downsizer_qdsp_command(YCBCR_DOWNSIZER_QDSP_DOWNLOAD_CMD, NULL, ycbcr_downsizer_handle_qdsp_cb) != 
                          YCBCR_DOWNSIZER_QDSP_CMD_ACCEPT_RET)
  {
    MSG_HIGH("ycbcr_downsizer: Invalid ycbcr_downsizer command", 0, 0, 0);
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_INVALID_CMD_MSG);
    return YCBCR_DOWNSIZER_CMD_ACCEPT_RET;
  }
  return YCBCR_DOWNSIZER_CMD_ACCEPT_RET;
}

/*=============================================================================

Function: YCBCR_DOWNSIZER_HANDLE_QDSP_CB

Description: This function will handle callback from ycbcr_downsizer_qdsp interface.

Return Value: None
   
Comments: None

Side Effects: None

=============================================================================*/
static void ycbcr_downsizer_handle_qdsp_cb ( ycbcr_downsizer_qdsp_msg_type msg )
{
  if (ycbcr_downsizer_is_aborting)
  {
    /* only queue abort operation */
    if (msg == YCBCR_DOWNSIZER_QDSP_IDLE_COMPLETE_MSG)
    {
      context_data.client_data = (int32) YCBCR_DOWNSIZER_ABORT_OP;
      /* Queue the abort command on the GRAPH task */
      context_data.function_ptr = ycbcr_downsizer_process_cb;
      context_data.client_ID = ycbcr_downsizer_session_id;
      graph_context_req( context_data );
    }
  }
  else
  {
    /* Base on the information from the DSP, set the ycbcr_downsizer state */
    switch (msg)
    {
      case YCBCR_DOWNSIZER_QDSP_IMAGE_READY_MSG:
        /* DSP image is ready.   */
        context_data.client_data = (int32) YCBCR_DOWNSIZER_START_OP;
        break;

      case YCBCR_DOWNSIZER_QDSP_DATA_READY_MSG:
      case YCBCR_DOWNSIZER_QDSP_IDLE_COMPLETE_MSG:
        /*ycbcr downsizer data is ready or else idle complete*/
        context_data.client_data = (int32) YCBCR_DOWNSIZER_DONE_OP;
        break;

      case YCBCR_DOWNSIZER_QDSP_MODULE_DISABLE_MSG:
        context_data.client_data = (int32) YCBCR_DOWNSIZER_DISABLE_OP;
        break;

      default:
        context_data.client_data = (int32) YCBCR_DOWNSIZER_MAX_OP;
        MSG_ERROR("ycbcr_downsizer: DSP is lost", 0, 0, 0); 
    }

    /* Queue the encoding command on the GRAPH task */
    context_data.function_ptr = ycbcr_downsizer_process_cb;
    context_data.client_ID = ycbcr_downsizer_session_id;

    graph_context_req( context_data);

  }

}

/*=============================================================================

Function: YCBCR_DOWNSIZER_PROCESS_CB

Description:  This function executes in the GRAPH task and it behaves as a
              state machine for the ycbcr_downsizer.
              
Return Value: None
   
Comments: 

YCBCR_DOWNSIZER_DOWNLOAD_OP  callback from ycbcr_downsizer_process_data to request image download.
YCBCR_DOWNSIZER_START_OP     callback from dsp informing that dps image is ready.
YCBCR_DOWNSIZER_DONE_OP     callback from dsp informing that idle command is completed.

Side Effects: None

=============================================================================*/
static void ycbcr_downsizer_process_cb ( int32 client_id, int32 client_data)
{

  static ycbcr_downsizer_data_type ycbcr_downsizer_start_cmd_data;  
  ycbcr_downsizer_operation_type operation = (ycbcr_downsizer_operation_type) client_data;

  if (client_id != ycbcr_downsizer_session_id)
  {
    MSG_ERROR("ycbcr_downsizer: invalid callback session ID",0,0,0);
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_INVALID_CMD_MSG);
    return;
  }

  switch (operation)
  {
  case YCBCR_DOWNSIZER_START_OP:
    /* one time parameter setup require for each ycbcr_downsizer session */
    ycbcr_downsizer_setup_parameter (&ycbcr_downsizer_start_cmd_data);

    if (ycbcr_downsizer_qdsp_command( YCBCR_DOWNSIZER_QDSP_START_CMD, &ycbcr_downsizer_start_cmd_data, NULL) != 
                          YCBCR_DOWNSIZER_QDSP_CMD_ACCEPT_RET)
    {
      ycbcr_downsizer_cleanup(YCBCR_DOWNSIZER_FAIL_QDSP_MSG);
    }
    break;

  case YCBCR_DOWNSIZER_DONE_OP:
    /* if we get this far, ycbcr_downsizer success */
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_SUCCESS_MSG);
    break;

  case YCBCR_DOWNSIZER_ABORT_OP:
    /* clean up before abort */
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_ABORT_MSG);
    break;

  case YCBCR_DOWNSIZER_PROCESS_ABORT_OP:
    ycbcr_downsizer_is_aborting = TRUE;

    /* if ycbcr_downsizer_qdsp reject the command, ycbcr_downsizer is not in active or idle state */
    if (ycbcr_downsizer_qdsp_command(YCBCR_DOWNSIZER_QDSP_IDLE_CMD, NULL, NULL) != YCBCR_DOWNSIZER_QDSP_CMD_ACCEPT_RET)
    {
      ycbcr_downsizer_cleanup(YCBCR_DOWNSIZER_ABORT_MSG);
    }
    break;

  case YCBCR_DOWNSIZER_DISABLE_OP:
    /* we received request from ycbcr_downsizer_qdsp to disable */
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_SUCCESS_MSG);
    break;

  default:
    MSG_HIGH("ycbcr_downsizer: unrecognize operation",0,0,0);
    ycbcr_downsizer_cleanup (YCBCR_DOWNSIZER_INVALID_CMD_MSG);
  }    

}


/*=============================================================================

Function: YCBCR_DOWNSIZER_SETUP_PARAMETER

Description:  This function setup one time parameter of a single ycbcr_downsizer session.
              
Return Value: None
   
Comments: None 

Side Effects: None

=============================================================================*/

static void ycbcr_downsizer_setup_parameter ( ycbcr_downsizer_data_type * cmd )
{
  /* clean up start command buffer */
  memset (cmd, 0, sizeof(ycbcr_downsizer_data_type));

  /* setup input data configuration */
  cmd->in_image.width = local_ycbcr_downsizer_info.data.in_image.width;          
  cmd->in_image.height = local_ycbcr_downsizer_info.data.in_image.height;
  cmd->row_in_increment.Y_increment = local_ycbcr_downsizer_info.data.row_in_increment.Y_increment;
  cmd->row_in_increment.CbCr_increment = local_ycbcr_downsizer_info.data.row_in_increment.CbCr_increment;
  cmd->in_frag_n_format.num_fragments = local_ycbcr_downsizer_info.data.in_frag_n_format.num_fragments;
  cmd->in_frag_n_format.format = local_ycbcr_downsizer_info.data.in_frag_n_format.format;
  cmd->in_lines_frag0_1.frag0 = local_ycbcr_downsizer_info.data.in_lines_frag0_1.frag0;
  cmd->in_lines_frag0_1.frag1 = local_ycbcr_downsizer_info.data.in_lines_frag0_1.frag1;
  cmd->in_lines_frag2_3.frag0 = local_ycbcr_downsizer_info.data.in_lines_frag2_3.frag0;
  cmd->in_lines_frag2_3.frag1 = local_ycbcr_downsizer_info.data.in_lines_frag2_3.frag1;
  cmd->in_Yaddress_frag0.address = local_ycbcr_downsizer_info.data.in_Yaddress_frag0.address;
  cmd->in_CbCraddress_frag0.address = local_ycbcr_downsizer_info.data.in_CbCraddress_frag0.address ;
  cmd->in_Yaddress_frag1.address = local_ycbcr_downsizer_info.data.in_Yaddress_frag1.address;
  cmd->in_CbCraddress_frag1.address  = local_ycbcr_downsizer_info.data.in_CbCraddress_frag1.address ;
  cmd->in_Yaddress_frag2.address = local_ycbcr_downsizer_info.data.in_Yaddress_frag2.address;
  cmd->in_CbCraddress_frag2.address = local_ycbcr_downsizer_info.data.in_CbCraddress_frag2.address;
  cmd->in_Yaddress_frag3.address = local_ycbcr_downsizer_info.data.in_Yaddress_frag3.address;
  cmd->in_CbCraddress_frag2.address = local_ycbcr_downsizer_info.data.in_CbCraddress_frag2.address;
  /* setup output data configuration */
  cmd->out_image.width = local_ycbcr_downsizer_info.data.out_image.width;
  cmd->out_image.height = local_ycbcr_downsizer_info.data.out_image.height;
  cmd->row_out_increment.Y_increment = local_ycbcr_downsizer_info.data.row_out_increment.Y_increment;
  cmd->row_out_increment.CbCr_increment = local_ycbcr_downsizer_info.data.row_out_increment.CbCr_increment;
  cmd->out_frag_n_format.num_fragments = local_ycbcr_downsizer_info.data.out_frag_n_format.num_fragments;
  cmd->out_frag_n_format.format = local_ycbcr_downsizer_info.data.out_frag_n_format.format;
  cmd->out_lines_frag0_1.frag0 = local_ycbcr_downsizer_info.data.out_lines_frag0_1.frag0;
  cmd->out_lines_frag0_1.frag1 = local_ycbcr_downsizer_info.data.out_lines_frag0_1.frag1;
  cmd->out_lines_frag2_3.frag0 = local_ycbcr_downsizer_info.data.out_lines_frag2_3.frag0;
  cmd->out_lines_frag2_3.frag1 = local_ycbcr_downsizer_info.data.out_lines_frag2_3.frag1;
  cmd->out_Yaddress_frag0.address = local_ycbcr_downsizer_info.data.out_Yaddress_frag0.address;
  cmd->out_CbCraddress_frag0.address = local_ycbcr_downsizer_info.data.out_CbCraddress_frag0.address;
  cmd->out_Yaddress_frag1.address = local_ycbcr_downsizer_info.data.out_Yaddress_frag1.address;
  cmd->out_CbCraddress_frag1.address = local_ycbcr_downsizer_info.data.out_CbCraddress_frag1.address;
  cmd->out_Yaddress_frag2.address = local_ycbcr_downsizer_info.data.out_Yaddress_frag2.address;
  cmd->out_CbCraddress_frag2.address = local_ycbcr_downsizer_info.data.out_CbCraddress_frag2.address;
  cmd->out_Yaddress_frag3.address = local_ycbcr_downsizer_info.data.out_Yaddress_frag3.address;
  cmd->out_CbCraddress_frag1.address = local_ycbcr_downsizer_info.data.out_CbCraddress_frag1.address;
}


/*=============================================================================

Function: YCBCR_DOWNSIZER_ABORT

Description: This function will try to abort the current ycbcr_downsizer session.

Return Value:

  YCBCR_DOWNSIZER_CMD_ACCEPT_RET : ycbcr_downsizer command is accepted. Caller must 
                       wait for callback before freeing any memory location accessible 
                       by cmd pointer.
  
  YCBCR_DOWNSIZER_CMD_REJECT_RET : command is NOT accepted, caller is responsible to 
                       check this status and perform clean up. NO callback will come 
                       from ycbcr_downsizer.
   

   
Comments: The caller must not free or modify data located in the input buffer
          until callback from ycbcr_downsizer is received.

=============================================================================*/

ycbcr_downsizer_ret_type ycbcr_downsizer_abort ( void )
{
  if (ycbcr_downsizer_in_progress == FALSE)
  {
    MSG_ERROR("ycbcr_downsizer: cannot abort. ycbcr_downsizer not in progress", 0, 0, 0);
    return YCBCR_DOWNSIZER_CMD_REJECT_RET;
  }

  /* We are already processing abort */
  if (ycbcr_downsizer_is_aborting)
  {
    return YCBCR_DOWNSIZER_CMD_ACCEPT_RET;
  }

  context_data.client_data = (int32) YCBCR_DOWNSIZER_PROCESS_ABORT_OP;

  /* Queue the abort command on the GRAPH task */
  context_data.function_ptr = ycbcr_downsizer_process_cb;
  context_data.client_ID = ycbcr_downsizer_session_id;

  graph_context_req( context_data );


  return YCBCR_DOWNSIZER_CMD_ACCEPT_RET;
}

/*=============================================================================

Function: YCBCR_DOWNSIZER_CLEANUP

Description:  This function disables the DSP, free all memory and reset the 
              static global variables.
              
Return Value: None
   
Comments: None 

Side Effects: None

=============================================================================*/
static void ycbcr_downsizer_cleanup ( ycbcr_downsizer_msg_type cb_msg )
{
  ycbcr_downsizer_cbmsg_type ret_msg;

  ret_msg.client_id = local_ycbcr_downsizer_info.client_id;
  ret_msg.status = cb_msg;

  /* We can take another ycbcr_downsizer session now */
  ycbcr_downsizer_in_progress = FALSE;
  /* clear abort flag */
  ycbcr_downsizer_is_aborting = FALSE;


  /* We are done with dsp image, request to disable it */
  (void)ycbcr_downsizer_qdsp_command( YCBCR_DOWNSIZER_QDSP_DISABLE_CMD, NULL, NULL);

  /* Notify the caller that ycbcr_downsizer operation is completed */
  if (local_ycbcr_downsizer_info.cb_fun)
  {
    (local_ycbcr_downsizer_info.cb_fun) ( ret_msg );
  }

  memset (&local_ycbcr_downsizer_info, 0, sizeof(ycbcr_downsizer_cmd_info_type));
}

/*lint -restore */
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */
