#ifndef YCBCR_QDSP_H
#define YCBCR_QDSP_H
/*=============================================================================
FILE:       YCbCr_QDSP.h

SERVICES:   YCBCR_DOWNSIZER QDSP DRIVER INTERFACE

DESCRIPTION:
   This file provides necessary interfaces for the ycbcr_downsizer to interface with
   QDSP.
   

INITIALIZATION AND SEQUENCING REQUIREMENTS:

Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.

$Header: //source/qcom/qct/multimedia/camera/6k/snapshotprocess/ycbcrdownsizer/main/latest/inc/ycbcrdownsizerqdsp.h#2 $ $Author: c_caluru $

============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/18/07   sigu    Corrected a typo in STATE_YCBCR_DOWNSIZE_NOT_READY.
01/10/07   sigu    ARM side changes for integrating the incall downsizer for 6260 target.
08/07/06   sigu    Initial version.

===========================================================================*/


/*---------------------------------------------------------------------------
      Include Files
----------------------------------------------------------------------------*/
#include "target.h"
#include "customer.h"
#include "comdef.h"
#include "camqdsp.h"         /* CAMQDSP driver typedefs and prototype      */

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/*---------------------------------------------------------------------------
      Defines for category type
----------------------------------------------------------------------------*/
/*                                                                          */
/* The following are definition for ycbcr_downsizer                         */
/*                                                                          */

/*
** Define the enum type for the ycbcr_downsizer states
*/

typedef enum
{
  STATE_YCBCR_DOWNSIZE_IDLE       = 0x0000,
  STATE_YCBCR_DOWNSIZE_ACTIVE     = 0x0001,
  STATE_YCBCR_DOWNSIZE_NOT_READY  = 0xFFFF
} ycbcr_downsizer_qdsp_state_type;

/*
** Define the enum type for the DSP messages 
*/
typedef enum
{
  YCBCR_DOWNSIZER_QDSP_IMAGE_READY_MSG,
  YCBCR_DOWNSIZER_QDSP_DATA_READY_MSG,
  YCBCR_DOWNSIZER_QDSP_IDLE_COMPLETE_MSG,
  YCBCR_DOWNSIZER_QDSP_MODULE_DISABLE_MSG,
  YCBCR_DOWNSIZER_QDSP_ILLEGAL_CMD_MSG
} ycbcr_downsizer_qdsp_msg_type;

/*
** ycbcr_downsizer return type
*/
typedef enum
{
  YCBCR_DOWNSIZER_QDSP_CMD_ACCEPT_RET,
  YCBCR_DOWNSIZER_QDSP_CMD_REJECT_RET
} ycbcr_downsizer_qdsp_ret_type;

/*
** Define the enum type for the ARM's commands 
*/
typedef enum
{
  YCBCR_DOWNSIZER_QDSP_DOWNLOAD_CMD,
  YCBCR_DOWNSIZER_QDSP_START_CMD,
  YCBCR_DOWNSIZER_QDSP_IDLE_CMD,
  YCBCR_DOWNSIZER_QDSP_DISABLE_CMD
} ycbcr_downsizer_qdsp_cmd_type;


/*
** Define the ycbcr_downsizer command register
** This command type is to be copied to ycbcr_downsizer_CommandReg
*/
typedef struct
{
  uint16 size;
  uint16 code;
} ycbcr_downsizer_qdsp_cmd_reg_type;

typedef union
{
  ycbcr_downsizer_qdsp_cmd_reg_type cmd;
  uint32 reg;
} ycbcr_downsizer_qdsp_reg_type;

/*
** QDSP message processing callback function structure
*/
typedef void (* ycbcr_downsizer_qdsp_cb_type) (ycbcr_downsizer_qdsp_msg_type msg);

/*===========================================================================*/
/* FUNCTION DEFINITIONS                                                      */
/*===========================================================================*/

/*===========================================================================

FUNCTION    YCBCR_DOWNSIZER_QDSP_COMMAND

DESCRIPTION   
  This function is used to command the QDSP software.
  It is only valid after the QDSP download is successful.
  This is a synchronous operation.

RETURN VALUE
  YCBCR_DOWNSIZER_QDSP_OK_RET    - cmd sucessful
  YCBCR_DOWNSIZER_QDSP_FAIL_RET  - problem with QDSP command


SIDE EFFECTS
  None.

============================================================================*/
extern ycbcr_downsizer_qdsp_ret_type ycbcr_downsizer_qdsp_command(ycbcr_downsizer_qdsp_cmd_type cmd_code, void * cmd_ptr, ycbcr_downsizer_qdsp_cb_type callback);

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
extern ycbcr_downsizer_qdsp_state_type ycbcr_downsizer_qdsp_get_current_state (void);

/*===========================================================================

FUNCTION ycbcr_downsizer_Q2A_ISR

DESCRIPTION

  Depends on the current stage of operation, this function notifies ARM the
  state of QDSP, including YCBCR_DOWNSIZER_DATA READY, YCBCR_DOWNSIZER IDLE COMPLETE and YCBCR_DOWNSIZER ILLEGAL 
  messages.

DEPENDENCIES
  This is a pseudo interrupt service routione called by camqdsp_isr.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void ycbcr_downsizer_q2a_isr (void);

#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */

#endif /* YCBCR_QDSP_H */



