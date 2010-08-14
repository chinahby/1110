/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  YCBCR_DOWNSIZER QDSP DRIVER INTERFACE FOR MSM6260

GENERAL DESCRIPTION
  This module contains functions of ycbcr_downsizer driver for the QDSP

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  See header file for more details

Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/ycbcrdownsizer/main/latest/src/ycbcrdownsizerqdsp.c#2 $ $Author: c_caluru $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/28/07   ven     Fixed a problem in switching between stand alone and in call downsizer
02/06/07   sigu    Corrected a problem associated with DS05_Module state machine.
                   Now after terminating the module, S/W state of ycbcr_downsizer_module_state
                   will be YCBCR_DOWNSIZER_MODULE_STATE_DISABLED, there by we can re use it many times.
01/18/07   sigu    Corrected a problem associated with the improper termination 
                   of the downsizer module. Now we are using state machine to 
                   hold the state of the DSP module.
01/10/07   sigu    ARM side changes for integrating incall downsizer for 6260 target.
08/07/06   sigu    Initial version.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/

/*lint -e766 header files not used */
#include "msg.h"                     /* Message logging/reporting services                      */
#include "qdsp.h"                    /* QDSP services                                           */
#include "clk.h"                     /* Message logging/reporting services                      */
#include "ycbcrdownsizerqdsp.h"      /* ycbcr_downsizer_qdsp driver typedefs and prototype      */
#include "qdspext.h"
#include "camctrl.h"

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED

/* <EJECT> */
/*===========================================================================
    
                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#define   Q2A_DOWNSIZE_DONE         0x0001
#define   Q2A_DOWNSIZE_STOPPED      0x0001
#define   Q2A_ILLEGAL_CMD           0x0001

#define   YCBCR_DOWNSIZER_START_CMD_CODE       0x0200
#define   YCBCR_DOWNSIZER_IDLE_CMD_CODE        0x0201

/*===========================================================================
 External Variable declaration
===========================================================================*/

extern qdsp_module_type module_list[CAMQDSP_SizeOfImageType];
extern camctrl_tbl_type camctrl_tbl;
/*===========================================================================
 Local Variable declaration
===========================================================================*/
/* callback function pointer provided by ARM in initialize function */
static ycbcr_downsizer_qdsp_cb_type caller_cb_fun = NULL;

typedef enum
{
  YCBCR_DOWNSIZER_MODULE_STATE_DISABLING,
  YCBCR_DOWNSIZER_MODULE_STATE_DISABLED,
  YCBCR_DOWNSIZER_MODULE_STATE_ENABLING,
  YCBCR_DOWNSIZER_MODULE_STATE_ENABLED
} ycbcr_downsizer_module_state_type;

static ycbcr_downsizer_module_state_type ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_DISABLED;

/*=============================================================================

                STATIC FUNCTION DECLARATIONS

=============================================================================*/
static void ycbcr_downsizer_qdsp_download_cb(CAMQDSP_ResponseType *DSPMessage);

/*=============================================================================

Function: YCBCR_DOWNSIZER_QDSP_DOWNLOAD_CB

Description: This function will handle callback from ycbcr_downsizer_qdsp interface
on a ycbcr_downsizer image download.

Return Value: None
   
Comments: None

Side Effects: None

=============================================================================*/
static void ycbcr_downsizer_qdsp_download_cb(CAMQDSP_ResponseType *DSPMessage)
{
  if(DSPMessage->responseMsg  == CAMQDSP_DOWNLOAD_DONE)
  {
    if (ycbcr_downsizer_module_state == YCBCR_DOWNSIZER_MODULE_STATE_ENABLING)
    {
      ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_ENABLED;
      if (caller_cb_fun)
      {
        caller_cb_fun ( YCBCR_DOWNSIZER_QDSP_IMAGE_READY_MSG );
      }
    }
    else if (ycbcr_downsizer_module_state == YCBCR_DOWNSIZER_MODULE_STATE_DISABLING)
    {
      CAMQDSP_Terminate();
      ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_DISABLED;
    }
  }
} /*ycbcr_downsizer_qdsp_download_cb*/

/*===========================================================================

FUNCTION    YCBCR_DOWNSIZER_QDSP_COMMAND

DESCRIPTION   
  This function is used to command the QDSP software.
  It is only valid after the QDSP download is successful.
  This is a synchronous operation.

RETURN VALUE
  YCBCR_DOWNSIZER_QDSP_CMD_ACCEPT_RET    - cmd sucessful
  YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET    - problem with QDSP command


SIDE EFFECTS
  None.

============================================================================*/
ycbcr_downsizer_qdsp_ret_type ycbcr_downsizer_qdsp_command (ycbcr_downsizer_qdsp_cmd_type cmd_code, 
                                     void * cmd_ptr, 
                                     ycbcr_downsizer_qdsp_cb_type callback)
{
  ycbcr_downsizer_qdsp_reg_type ycbcr_downsizer_cmd_reg;
  ycbcr_downsizer_qdsp_state_type ycbcr_downsizer_state = ycbcr_downsizer_qdsp_get_current_state();

  switch (cmd_code)
  {
    case YCBCR_DOWNSIZER_QDSP_DOWNLOAD_CMD:
    ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_ENABLING;
    caller_cb_fun = callback;
    if (  camctrl_tbl.use_incall_downsizer == FALSE)
    {
#if defined QDSP_MODULE_SA_DS05_DEFINED
      module_list[CAMQDSP_YCbCr_downsizer] = QDSP_MODULE_SA_DS05; 
#endif /* QDSP_MODULE_SA_DS05_DEFINED */
    }
    else
    {
#if defined QDSP_MODULE_DS05_DEFINED
      module_list[CAMQDSP_YCbCr_downsizer] = QDSP_MODULE_DS05;
#endif /* QDSP_MODULE_DS05_DEFINED */
    }
    if(CAMQDSP_Initialize (ycbcr_downsizer_qdsp_download_cb, CAMQDSP_YCbCr_downsizer) != CAMQDSP_SUCCESS)
    {
      ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_DISABLED;
      return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
    }
  break;

  case YCBCR_DOWNSIZER_QDSP_START_CMD:
    if (ycbcr_downsizer_state != STATE_YCBCR_DOWNSIZE_IDLE)
    {
      MSG_ERROR ("ycbcr_downsizer_qdsp: Invalid state",0,0,0);
      return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
    }

    if ( qdsp_read(QDSP_ycbcrDownsizeCommandReg) )
    {
      MSG_ERROR("YCBCR_DOWNSIZER_QDSP: Command register was not 0", 0, 0, 0);
      return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
    }

    if (cmd_ptr)
    {
      MSG_MED("YCBCR_DOWNSIZER_QDSP: Prepare to send YCBCR_DOWNSIZER_START command", 0, 0, 0);
      /*lint -save -e740 casting cmd ptr to uint32 ptr */
      /* Copy the command buffer data to QDSP */
      qdsp_block_write32(QDSP_ycbcrDownsizeCommandBuf, 0,(uint32 *)cmd_ptr, 
                         26, FALSE);
      /*lint -restore */
      ycbcr_downsizer_cmd_reg.cmd.code = YCBCR_DOWNSIZER_START_CMD_CODE;
      ycbcr_downsizer_cmd_reg.cmd.size = 38;
      qdsp_write( QDSP_ycbcrDownsizeCommandReg, ycbcr_downsizer_cmd_reg.reg);
      /* Assert ARM to DSP interrupt */
      qdsp_up_to_dsp_irq(); 
    }
    else
    {
      MSG_ERROR("YCBCR_DOWNSIZER_QDSP: start_cmd with null cmd buffer pointer ", 0 , 0, 0);
      return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
    }
  break;


  case YCBCR_DOWNSIZER_QDSP_IDLE_CMD:
    if ((ycbcr_downsizer_state != STATE_YCBCR_DOWNSIZE_ACTIVE) &&
       (ycbcr_downsizer_state != STATE_YCBCR_DOWNSIZE_IDLE))
    {
      return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
    }
    
    if ( qdsp_read(QDSP_ycbcrDownsizeCommandReg) )
    {
      MSG_ERROR("YCBCR_DOWNSIZER_QDSP: Command register was not 0", 0, 0, 0);
      return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
    }
    else
    {
      MSG_MED("YCBCR_DOWNSIZER_QDSP: Prepare to send YCBCR_DOWNSIZER__IDLE command", 0, 0, 0);
      ycbcr_downsizer_cmd_reg.cmd.code = YCBCR_DOWNSIZER_IDLE_CMD_CODE;
      ycbcr_downsizer_cmd_reg.cmd.size = 0;
      /* Set idle command ID */
      qdsp_write( QDSP_ycbcrDownsizeCommandReg, ycbcr_downsizer_cmd_reg.reg);
      /* Assert ARM to DSP interrupt */
      qdsp_up_to_dsp_irq();
    }
  break;

  case YCBCR_DOWNSIZER_QDSP_DISABLE_CMD:
    MSG_MED("YCBCR_DOWNSIZER_QDSP: Prepare to DISABLE dsp module", 0, 0, 0);
    if (ycbcr_downsizer_module_state == YCBCR_DOWNSIZER_MODULE_STATE_ENABLED)
    {
      CAMQDSP_Terminate();
      /*Change the state to be disabled, so that we can have more sessions with downsizer*/
      ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_DISABLED;
    }
    else if (ycbcr_downsizer_module_state == YCBCR_DOWNSIZER_MODULE_STATE_ENABLING)
    {
      ycbcr_downsizer_module_state = YCBCR_DOWNSIZER_MODULE_STATE_DISABLING;
    }
  break;

  default:
    MSG_ERROR("YCBCR_DOWNSIZER_QDSP: QDSP Command not recognized", 0, 0, 0);
    return YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET;
  }

  return YCBCR_DOWNSIZER_QDSP_CMD_ACCEPT_RET;
} /* ycbcr_downsizer_qdsp_command */

/*===========================================================================

FUNCTION YCBCR_DOWNSIZER_Q2A_ISR

DESCRIPTION

  Depends on the current stage of operation, this function notifies ARM the
  state of QDSP, including ycbcr_downsizer data ready, ycbcr_downsizer idle complete and 
  ycbcr_downsizer illegal messages.

DEPENDENCIES
  This is a pseudo interrupt service routione called by camqdsp_isr.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ycbcr_downsizer_q2a_isr (void)
{
  ycbcr_downsizer_qdsp_reg_type cmd_reg;

  /* Check DSP ycbcr_downsizer's data ready */
  cmd_reg.reg = qdsp_read(QDSP_ycbcrDownsizeMsgDoneReg);
  
  if ( cmd_reg.cmd.code == Q2A_DOWNSIZE_DONE )
  {
    if (caller_cb_fun)
    {
      caller_cb_fun ( YCBCR_DOWNSIZER_QDSP_DATA_READY_MSG );
    }
    qdsp_write(QDSP_ycbcrDownsizeMsgDoneReg, 0);
    MSG_MED ("YCBCR_DOWNSIZER_QDSP: new buffer ready",0,0,0);
    return;
  }

  /* Check DSP ycbcr_downsizer's idle complete message */
  cmd_reg.reg = qdsp_read(QDSP_ycbcrDownsizeMsgStoppedReg);

  if ( cmd_reg.cmd.code == Q2A_DOWNSIZE_STOPPED )
  {
    if (caller_cb_fun)
    {
      caller_cb_fun( YCBCR_DOWNSIZER_QDSP_IDLE_COMPLETE_MSG );
    }
    qdsp_write(QDSP_ycbcrDownsizeMsgStoppedReg, 0);
    MSG_MED ("YCBCR_DOWNSIZER_QDSP: ycbcr_downsizer enters idle state",0,0,0);
    return;
  }

  /* Check DSP ycbcr_downsizer's illegal message */
  cmd_reg.reg = qdsp_read(QDSP_ycbcrDownsizeMsgIllegalCmdReg);

  if ( cmd_reg.cmd.code == Q2A_ILLEGAL_CMD )
  {
    if (caller_cb_fun)
    {
    caller_cb_fun ( YCBCR_DOWNSIZER_QDSP_ILLEGAL_CMD_MSG );
    }
  qdsp_write(QDSP_ycbcrDownsizeMsgIllegalCmdReg, 0);
  MSG_ERROR ("YCBCR_DOWNSIZER_QDSP: DSP says illegal command",0,0,0);
  }

} /* ycbcr_downsizer_q2a_isr */


/*===========================================================================

FUNCTION YCBCR_DOWNSIZER_QDSP_GET_CURRENT_STATE

DESCRIPTION
  Return the current state of ycbcr_downsizer.
  
DEPENDENCIES
  None.

RETURN VALUE
  Current state of ycbcr_downsizer.

SIDE EFFECTS
  None.
===========================================================================*/
ycbcr_downsizer_qdsp_state_type ycbcr_downsizer_qdsp_get_current_state (void)
{
  /* Get DSP ycbcr_downsizer's current state if module is enabled. */
  if (ycbcr_downsizer_module_state == YCBCR_DOWNSIZER_MODULE_STATE_ENABLED)
  {
    return ((ycbcr_downsizer_qdsp_state_type) qdsp_read(QDSP_dsState));
  }
  else
  {
    return STATE_YCBCR_DOWNSIZE_NOT_READY;
  }
} /*ycbcr_downsizer_qdsp_get_current_state*/

#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */
