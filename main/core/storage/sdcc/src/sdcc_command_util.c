/******************************************************************************
 * File: sdcc_common_util.c
 *
 * Services: 
 *    Utility for raw command API
 *
 * Description:
 *    This file contains the raw command utility function definitions.
 *
 * Copyright (c) 2008 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_command_util.c#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
03/05/08     sc      Removed SDCC_PARAM_NOT_REF macro
01/21/08     sc      Initial version

=============================================================================*/

#include "sdcc_command_util.h"
#include "sdcc_util.h"


/* map the raw command response type to internal response type */
#define SDCC_CMD_RESP_TYPE(resp_type) \
   (SDCC_RESPONSE_NONE == (resp_type)) ? SDCC_RESP_NONE : \
      ((SDCC_R2 == (resp_type)) ? SDCC_RESP_LONG : SDCC_RESP_SHORT)


/******************************************************************************
* Name: sdcc_dma_usable
*
* Description:
*    This function determines if DMA can be enabled by examining the passed in
*    parameters.  If the data transfer is in block mode and the data_length
*    exceeds the maximum number of blocks, DMA cannot be used.  If the buffer
*    address is not aligned with the DMA requirement, DMA cannot be used.
*
* Parameters:
*    data_length     [IN] : length of the data in number of blocks
*                           Each block has 512 bytes.
*    buffer          [IN] : pointer to the data buffer
*
* Returns:
*    TRUE if DMA is used; otherwise FALSE
*
******************************************************************************/
boolean
sdcc_dma_usable
(
   const uint32 data_length,
   const uint32 *buffer
)
{
   boolean use_dma = FALSE;

   if ( SDCC_MEM_BLK_MODE == sdcc_pdata.block_mode &&
        data_length > SDCC_MAX_NO_BLOCKS )
   {
      /* Requested number of blocks for data transfer exceeds the maximum */
      /* number of blocks that SDCC can handle.  As such, print a debug */
      /* message and move on to use FIFO. */
      DPRINTF(("Number of sectors requested %d exceeds max number of blocks\n",
               data_length));
   }
#ifndef T_QSC1100
   else if ( TRUE == sdcc_pdata.enable_dma && IS_ALIGNED(buffer) )
   {
      use_dma = TRUE;
   }
#endif
   else
   {
      /* print a debug message indicate using FIFO */
      DPRINTF(("FIFO is used with buffer address 0x%x\n", buffer));
   }

   return use_dma;
} /* sdcc_dma_usable */

/******************************************************************************
* Name: sdcc_config_dma_ctrl
*
* Description:
*    This function configures the DMA controller according to the passed in
*    parameters.  The MCI_DATA_CTL register configuration is target dependent.
*    This setting can be found in target's Software Interface Manual.
*
* Parameters:
*    drive_number        [IN] : a valid drive number between 0 and the total
*                               number of card slots - 1
*    data_length         [IN] : length of the data
*                               Unit is in block if data transfer is in block
*                               mode.  Each block has 512 bytes.
*                               Unit is in byte if data transfer is in byte
*                               mode.
*    data_xfer_direction [IN] : data transfer direction
*                               SDCC_DATA_WRITE: from host to memory device or
*                               SDCC_DATA_READ: from memory device to host
*    enable_dma          [IN] : indicate if DMA is enabled
*
* Returns:
*    void
*
******************************************************************************/
void
sdcc_config_dma_ctrl
(
   const int16   drive_number,
   const uint32  data_length,
   const uint32  data_xfer_direction,
   const boolean enable_dma
)
{
   uint32 n_bits    = 0;
   uint32 data_ctrl = 0;

   (void)drive_number;

   if ( SDCC_MEM_BYTE_MODE == sdcc_pdata.block_mode )
   {
      /* calculate the number of bits in the data length requested */
      n_bits = sdcc_blk_in_bits(data_length);
   }
   else
   {
      /* calculate the number of bits in the default block length */
      n_bits = sdcc_blk_in_bits(sdcc_pdata.mem.block_len);
   }

   if ( TRUE == enable_dma )
   {
      data_ctrl  = MCI_DATA_DMA_ENABLED;
   }
   data_ctrl |= (n_bits << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT);
   data_ctrl |= MCI_DATA_DPSM_ENABLED |
      ((SDCC_DATA_WRITE == data_xfer_direction) ? MCI_DATA_WRITE : MCI_DATA_READ);

   /* MCI_DATA_CTL is a 8-bit register in 6K targets */
   HWIO_OUT(MCI_DATA_CTL, (uint8)data_ctrl);
} /* sdcc_config_dma_ctrl */

/******************************************************************************
* Name: sdcc_map_command_descriptor_type_to_cmd_type
*
* Description:
*    This function maps each fields in sdcc_command_descriptor_type and 
*    parameter wait_for_program_done to the fields in the internal 
*    data sdcc_cmd_type.
*
* Parameters:
*    command_descriptor     [IN] : pointer to the command descriptor
*    wait_for_program_done  [IN] : flag to indicate if wait for program done
*                                  signal is required.  For example, this flag
*                                  can be set to TRUE for STOP TRANSMISSION
*                                  command after multiple block write data
*                                  transfer.
*    sdcc_cmd              [OUT] : pointer to the internal data structure
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_map_command_descriptor_type_to_cmd_type
(
   const sdcc_command_descriptor_type *command_descriptor,
   const boolean                       wait_for_program_done,
   sdcc_cmd_type                      *sdcc_cmd
)
{
   sdcc_cmd->cmd = command_descriptor->command_index;
   sdcc_cmd->cmd_arg = command_descriptor->command_argument;
   sdcc_cmd->resp_type = (SDCC_CARD_RESP) SDCC_CMD_RESP_TYPE(
      command_descriptor->response_type);
   if ( TRUE == wait_for_program_done )
   {
      sdcc_cmd->prog_scan = MCI_CMD_PROG_ENABLED;
   }
   else
   {
      sdcc_cmd->prog_scan = 0;
   }
   sdcc_cmd->status = 0;
} /* sdcc_map_command_descriptor_type_to_cmd_type */

/******************************************************************************
* Name: sdcc_raw_command_core
*
* Description:
*    This function maps the fields in command_descriptor structure and 
*    parameter wait_for_program_done to the fields of the internal data
*    structure sdcc_cmd_type.  It then sends the command to the memory device
*    indicated in the drive_number parameter.  Parameter command_response will
*    contain the response to the command.  This function does not verify the
*    validity of the command_descriptor.
*
* Parameters:
*    drive_number           [IN] : a valid drive number between 0 and the total
*                                  number of card slots - 1
*    command_descriptor     [IN] : pointer to the command descriptor
*    wait_for_program_done  [IN] : flag to indicate if wait for program done
*                                  signal is required.  For example, this flag
*                                  can be set to TRUE for STOP TRANSMISSION
*                                  command after multiple block write data
*                                  transfer.
*    command_response      [OUT] : pointer to command response buffer contains
*                                  the response to command via the command line
*
* Returns:
*    Returns SDCC_NO_ERROR on success.  Otherwise, error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_raw_command_core
(
   const int16                         drive_number,
   const sdcc_command_descriptor_type *command_descriptor,
   const boolean                       wait_for_program_done,
   sdcc_command_response_type         *command_response
)
{
   SDCC_STATUS       rc = SDCC_ERR_UNKNOWN;
   sdcc_cmd_type     sdcc_cmd;

   (void)drive_number;

   /* populate the internal command structure and */
   /* just send the command to the memory device */
   memset(&sdcc_cmd, 0, sizeof(sdcc_cmd));
   sdcc_map_command_descriptor_type_to_cmd_type(command_descriptor,
      wait_for_program_done, &sdcc_cmd);

   rc = sdcc_command(&sdcc_cmd);
   /* populate the response from internal data structure to command_response */
   memcpy(command_response->command_response_buffer, sdcc_cmd.resp, 
      sizeof(command_response->command_response_buffer));

   if ( SDCC_NO_ERROR != rc )
   {
      DPRINTF(("Error %d occurs in command %d\n", rc, 
         command_descriptor->command_index));
   }
   return rc;
} /* sdcc_raw_command_core */

