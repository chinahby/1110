/*===========================================================================

                R U I M    C U S T O M    G E N E R I C
                  C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This module contains the state machine of the customer generic command
  processing with the RUIM.

Copyright (c) 1995, 2000,2001,2009 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/uimgnusr.c_v   1.1   25 May 2001 18:18:30   ckrishna  $
$Header: //source/qcom/qct/modem/uim/uimdrv/rel/07H1_2/src/uimgnusr.c#2 $ $DateTime: 2009/03/25 00:48:02 $ $Author: suberoy $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/09   sun     Added support for Virtuim
04/03/06   pv      Lint Fixes
11/14/05   pv      Lint Fixes
02/20/04   ts      Fixed lint errors.
02/23/01   ts      Created module for the generic module of the new UIM server.
                   Created from ruimusr.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"

#ifdef FEATURE_UIM
#include "uimgen.h"
#include "err.h"

/* <EJECT> */
/*===========================================================================

                Lint message inhibitions

===========================================================================*/
/*lint -esym(715,cmd_ptr) -esym(715,rsp_ptr) -esym(715,cmd)
  This is stub code for a user to customize as needed.  These symbols must
  match the prototype, but are no used in this stub code.*/
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

//LOCAL byte uim_gen_usr_sel_st_ctr = 0;

LOCAL uim_generic_state_type uim_gen_usr_state;

LOCAL uim_cmd_status_type uim_gen_usr_status;

//LOCAL uim_req_buf_type *uim_gen_usr_req_ptr;
    /* Pointer to request buffer */
//LOCAL uim_rsp_buf_type *uim_gen_usr_rsp_ptr;
    /* Pointer to response buffer */
//LOCAL uim_resp_callback_type uim_gen_usr_callback_fn;
    /* Callback function pointer */

/* <EJECT> */

/*===========================================================================

FUNCTION UIM_CUSTOM_GENERIC_CMD_PROCESSOR

DESCRIPTION
  This procedure processes the custom commands( implemented by the customers)
  sent to the UIM server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void uim_custom_generic_cmd_processor
(
  uim_cmd_type *cmd_ptr,
    /* Pointer to received command */
  uim_req_buf_type const *req_ptr,
  /* Parameter in place for future use.  Tell lint to ignore it. */
  /*lint -esym(715,req_ptr) */
    /* Pointer to request buffer */
  uim_rsp_buf_type const *rsp_ptr,
    /* Pointer to response buffer */
  uim_resp_callback_type callback_fn
  /* Parameter in place for future use.  Tell lint to ignore it. */
  /*lint -esym(715,callback_fn) */
  /* Callback function pointer */
)
{
  /* Make them global to this module */
  //uim_gen_usr_req_ptr     = req_ptr;

  //uim_gen_usr_rsp_ptr     = rsp_ptr;

  //uim_gen_usr_callback_fn = callback_fn;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_custom_generic_cmd_processor cmd_ptr is NULL",0,0,0);
  }

  switch (cmd_ptr->hdr.command)
  {

    default:

      uim_gen_usr_state = UIM_DONE_ST;

//      uim_set_cmd_state(uim_gen_usr_state);

      uim_generic_command(cmd_ptr);

      break;
  }
} /* uim_custom_generic_cmd_processor */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_GENERIC_COMMAND

DESCRIPTION
  This procedure is the state machine for the custom UIM commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void uim_custom_generic_command
(
  uim_cmd_type const *cmd_ptr
)
{

  switch(uim_gen_usr_state)
  {
    /* For a valid state, fill the request buffer here and call
       uim_send_command() */


    default:

      /* Invalid State */
       uim_gen_usr_state = UIM_DONE_ST;

//       uim_set_cmd_state(uim_gen_usr_state);

//       uim_set_sel_st_ctr(  uim_gen_usr_sel_st_ctr);

    break;
  }

} /* uim_custom_generic_command */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_GENERIC_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the RUIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

uim_cmd_status_type uim_custom_generic_command_response
(
 uim_rsp_buf_type const *rsp_ptr,
 uim_cmd_type     const *cmd
)
{
  uim_gen_usr_status = UIM_CMD_SUCCESS;

  switch(uim_gen_usr_state)
  {

    default:

      /* set the next state */
       uim_gen_usr_state = UIM_DONE_ST;

//       uim_set_cmd_state(uim_gen_usr_state);

    break;
  }

  return(uim_gen_usr_status);

} /* uim_custom_generic_command_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CUSTOM_PROCESS_GENERIC_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the RUIM
  at the completion of the command processing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void uim_custom_process_generic_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type     const *cmd_ptr
)
{

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_custom_process_generic_response cmd_ptr is NULL",0,0,0);
  }
  switch(cmd_ptr->hdr.command)
  {

    default:
       // do nothing
    break;

  }

} /* uim_custom_process_generic_response */

/*========================================================================

FUNCTION UIM_CUSTOM_SEND_GENERIC_ERR_RPT

DESCRIPTION
  This function sends an error report for the command that is being requested.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_custom_send_generic_err_rpt
(
  uim_cmd_type const *cmd_ptr
)
{
  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_custom_send_generic_err_rpt cmd_ptr is NULL",0,0,0);
  }
  switch (cmd_ptr->hdr.command)
  {

    default:

      /* Implemented by user */

    break;
  }

} /* uim_custom_send_generic_err_rpt */

/*===========================================================================

                Lint message inhibitions turned back on

===========================================================================*/
/*lint +esym(715,cmd_ptr) +esym(715,rsp_ptr) +esym(715,cmd)
  This is stub code for a user to customize as needed.  These symbols must
  match the prototype, but are no used in this stub code.*/
#endif /* FEATURE_UIM */
