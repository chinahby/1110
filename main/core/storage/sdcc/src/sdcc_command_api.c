/******************************************************************************
 * File: sdcc_common_api.c
 *
 * Services: 
 *    Public API for raw command
 *
 * Description:
 *    This file contains the raw command function definition.
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

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_command_api.c#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
03/05/08     sc      Removed SDCC_PARAM_NOT_REF macro
01/18/08     sc      Initial version

=============================================================================*/

#include "sdcc_command_api.h"
#include "sdcc_command_util.h"
#include "sdcc_api.h"
#include "sdcc_priv.h"
#include "sdcc_util.h"

#ifdef T_QSC1110
/******************************************************************************
* Name: sdcc_raw_command
*
* Description:
*    This function sends the command per the command_descriptor parameter to
*    the memory device indicated in the drive_number parameter.  The fields
*    such as command_index, command_type, command_argument and response_type
*    in the command_descriptor structure must be populated according to the
*    SD or MMC specification that corresponds to the memory device in use.
*    Parameter wait_for_program_done should be set if waiting for card to
*    complete programming the data is required after data transfer.  Parameter
*    command_response will contain the memory device response to the command.
*    This function does not verify the validity of the command_descriptor.
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
*                                  from memory device
*
* Returns:
*    Returns SDCC_NO_ERROR on success.  Otherwise, error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_raw_command
(
   const int16                         drive_number,
   const sdcc_command_descriptor_type *command_descriptor,
   const boolean                       wait_for_program_done,
   sdcc_command_response_type         *command_response
)
{
   /* sanity check of card type */
   if ( SDCC_CARD_UNKNOWN == sdcc_pdata.card_type )
   {
      return SDCC_ERR_CARD_UNDETECTED;
   }

   /* sanity check the passed in drive number */
   if ( (uint16) drive_number >= SDCC_MAX_NUM_CONTROLLER )
   {
      DPRINTF(("Invalid drive number %d\n", drive_number));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* sanity check the passed in command descriptor before de-referencing */
   if ( !command_descriptor )
   {
      DPRINTF(("Invalid command descriptor pointer\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* sanity check the passed in command response pointer */
   if ( !command_response )
   {
      DPRINTF(("Invalid command response pointer\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   return sdcc_raw_command_core(drive_number, command_descriptor,
      wait_for_program_done, command_response);
} /* sdcc_raw_command */

/******************************************************************************
* Name: sdcc_raw_command_with_data
*
* Description:
*    This function sends the command per the command_with_data_descriptor
*    parameter to the memory device indicated in the drive_number parameter.
*    The fields such as command_index, command_type, command_argument and
*    response_type in the sub-structure command_descriptor must be populated
*    according to the SD or MMC specification that corresponds to the memory
*    device in use.  Fields adtc_direction, adtc_memory_mode and 
*    adtc_data_length must be populated for data transfer.  Parameter
*    command_response will contain the memory device response to the command.
*    The buffer_data_to_card must contain data if the data transmission is to
*    write data to the memory device.  The buffer_data_from_card must be big
*    enough to hold the data read from the memory device if the data
*    transmission is to read data from the memory device.  The block length is
*    fixed to 512 bytes and should not be changed for all data transfer.  The
*    maximum number of blocks for data transfer is target dependent.  This
*    value is specified as MCI_DATA_LENGTH in each target's Software Interface
*    Manual.  The adtc_data_length must not exceed the specified maximum number
*    of blocks.  This function does not verify the validity of the
*    command_with_data_descriptor nor the data in the buffer_data_to_card.
*
* Parameters:
*    drive_number                   [IN] : a valid drive number between 0 and
*                                          the total number of card slots - 1
*    command_with_data_descriptor   [IN] : pointer to the command with data
*                                          descriptor
*    command_response              [OUT] : pointer to command response buffer
*                                          contains the response to command via
*                                          the command line from memory device
*    buffer_data_to_card            [IN] : pointer to buffer for outgoing data
*                                          to the memory device
*    buffer_data_from_card         [OUT] : pointer to buffer for incoming data
*                                          from the memory device
*
* Returns:
*    Returns SDCC_NO_ERROR on success.  Otherwise, error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_raw_command_with_data
(
   const int16                   drive_number,
   const sdcc_command_with_data_descriptor_type *command_with_data_descriptor,
   sdcc_command_response_type   *command_response,
   const uint32                 *buffer_data_to_card,
   uint32                       *buffer_data_from_card
)
{
   SDCC_STATUS    rc           = SDCC_ERR_UNKNOWN;
   uint32         length       = 0;
   uint32         sdcc_data_xfer_dir;
   boolean        use_dma      = FALSE;
   uint32        *buff_ptr     = 0;
   sdcc_cmd_type  sdcc_cmd;

   /* sanity check of card type */
   if ( SDCC_CARD_UNKNOWN == sdcc_pdata.card_type )
   {
      return SDCC_ERR_CARD_UNDETECTED;
   }

   /* sanity check the passed in drive number */
   if ( (uint16) drive_number >= SDCC_MAX_NUM_CONTROLLER )
   {
      DPRINTF(("Invalid drive number %d\n", drive_number));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* sanity check the passed in command descriptor before de-referencing */
   if ( !command_with_data_descriptor )
   {
      DPRINTF(("Invalid command descriptor pointer\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* sanity check the passed in command response pointer */
   if ( !command_response )
   {
      DPRINTF(("Invalid command response pointer\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   if ( SDCC_ADTC_READ == command_with_data_descriptor->adtc_direction )
   {
      /* sanity check the buffer_data_from_card pointer */
      if ( !buffer_data_from_card )
      {
         DPRINTF(("Invalid passed in buffer for read data transfer\n"));
         return SDCC_ERR_INVALID_PARAM;
      }
      buff_ptr = buffer_data_from_card;
      /* map to internal data xfer direction */
      sdcc_data_xfer_dir = SDCC_DATA_READ;

   }
   else if ( SDCC_ADTC_WRITE == command_with_data_descriptor->adtc_direction )
   {
      /* sanity check the buffer_data_to_card pointer */
      if ( !buffer_data_to_card )
      {
         DPRINTF(("Invalid passed in buffer for write data transfer\n"));
         return SDCC_ERR_INVALID_PARAM;
      }
      buff_ptr = (uint32*) buffer_data_to_card;
      /* map to internal data xfer direction */
      sdcc_data_xfer_dir = SDCC_DATA_WRITE;
   }
   else
   {
      DPRINTF(("Invalid ADTC data transfer direction\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* sdcc_setup_data_xfer checks the sdcc_pdata.block_mode to */
   /* decide DMA enabling */
   if ( SDCC_ADTC_MEMORY_BLOCK_MODE == 
        command_with_data_descriptor->adtc_memory_mode )
   {
      sdcc_pdata.block_mode = SDCC_MEM_BLK_MODE;
   }
   else if ( SDCC_ADTC_MEMORY_BYTE_MODE ==
             command_with_data_descriptor->adtc_memory_mode )
   {
      sdcc_pdata.block_mode = SDCC_MEM_BYTE_MODE;
   }
   else
   {
      DPRINTF(("Invalid ADTC memory mode\n"));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* 2nd argument is uint16 for 6K targets */
   length = sdcc_setup_data_xfer( sdcc_data_xfer_dir, 
      (uint16)command_with_data_descriptor->adtc_data_length );
   if ( !length )
   {
      if ( SDCC_DATA_WRITE == sdcc_data_xfer_dir )
      {
         sdcc_pdata.errno = SDCC_ERR_WRITE_FIFO;
      }
      else
      {
         sdcc_pdata.errno = SDCC_ERR_READ_FIFO;
      }
      /* reset the internal block_mode to SDCC_MEM_BLK_MODE */
      sdcc_pdata.block_mode = SDCC_MEM_BLK_MODE;
      return sdcc_pdata.errno;
   }

   /* examine if DMA can be enabled */
   use_dma = sdcc_dma_usable(command_with_data_descriptor->adtc_data_length,
      buff_ptr);

   /* enable DMA controller first before sending command */
   /* if it is a read data transfer */
   if ( SDCC_DATA_READ == sdcc_data_xfer_dir )
   {
      if ( TRUE == use_dma )
      {
         /* make sure to sync with the DMA memory */
         mmu_invalidate_data_cache_lines(buff_ptr, length);
         HWIO_OUT(MCI_START_ADDR, buff_ptr);
      }
      sdcc_config_dma_ctrl(drive_number, length, sdcc_data_xfer_dir, use_dma);
   }

   /* send adtc command without waiting for the program done signal */
   rc = sdcc_raw_command_core(drive_number, 
      &(command_with_data_descriptor->command_descriptor), FALSE, command_response);
   if ( SDCC_NO_ERROR != rc )
   {
      DPRINTF(("Error %d occurs in adtc command %d\n", rc,
         command_with_data_descriptor->command_descriptor.command_index));
   }
   else
   {
      if ( SDCC_DATA_WRITE == sdcc_data_xfer_dir )
      {
         if ( TRUE == use_dma )
         {
            HWIO_OUT(MCI_START_ADDR, buff_ptr);
         }
         /* enable DMA controller after sending command */
         /* if it is a write data transfer */
         sdcc_config_dma_ctrl(drive_number, length, sdcc_data_xfer_dir,
            use_dma);
      }

      /* process data transfer */
      if ( TRUE == use_dma )
      {
         memset(&sdcc_cmd, 0, sizeof(sdcc_cmd));
         sdcc_map_command_descriptor_type_to_cmd_type(
            &(command_with_data_descriptor->command_descriptor), FALSE, &sdcc_cmd);
         rc = sdcc_process_interrupts(&sdcc_cmd);
      }
      else
      {
         if ( SDCC_DATA_READ == sdcc_data_xfer_dir )
         {
            rc = sdcc_read_fifo((byte*)buff_ptr, length);
         }
         else
         {
            rc = sdcc_write_fifo((byte*)buff_ptr, length);
         }
      }
   }

   if ( SDCC_NO_ERROR != rc )
   {
      DPRINTF(("adtc command %d failed\n", 
         command_with_data_descriptor->command_descriptor.command_index));
   }

   /* reset the internal block_mode to SDCC_MEM_BLK_MODE */
   sdcc_pdata.block_mode = SDCC_MEM_BLK_MODE;
   sdcc_pdata.errno = rc;

   return rc;
} /* sdcc_raw_command_with_data */

/******************************************************************************
* Name: sdcc_set_block_length
*
* Description:
*    This function must be called if there is no issue after setting the block
*    length on the memory device via function sdcc_raw_command indicated in the
*    drive_number parameter.  Caller must check if BLOCK_LEN_ERROR bit is set
*    on the command response after sending CMD16 SET_BLOCKLEN command via
*    function sdcc_raw_command.  Refer to section 4.10.1 on SD2.0 Part 1
*    Physical Layer Specification or section 4.11 on MMC Version 4.2 System
*    Specification about the BLOCK_LEN_ERROR bit.  This function sets the
*    internal block length data as parameter block_length.  This block length
*    (in bytes) is set for all block data transfer commands.
*
* Parameters:
*    drive_number        [IN] : a valid drive number between 0 and the total
*                               number of card slots - 1
*    block_length        [IN] : block length in bytes to be set
*
* Returns:
*    Returns SDCC_NO_ERROR on success.  Otherwise, error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_set_block_length
(
   const int16   drive_number,
   const uint32  block_length
)
{
   (void)drive_number;

   /* sanity check the passed in block length */
   if ( 0 == block_length )
   {
      DPRINTF(("Invalid block length %d", block_length));
      return SDCC_ERR_INVALID_PARAM;
   }

   /* set the internal data block_len */
   sdcc_pdata.mem.block_len = block_length;
   return SDCC_NO_ERROR;
} /* sdcc_set_block_length */

/******************************************************************************
* Name: sdcc_set_port_active
*
* Description:
*    This function sets the port to active per parameter active_state.  If
*    active_state is TRUE, the clock to device will always be on.  Otherwise,
*    it allows the clock to be pulled low to save power.  Before transferring
*    data or waiting for program done signal, this function must be called with
*    TRUE to set the clock on.  After data transfer is complete or program done
*    signal is processed, this function must be called with FALSE to allow
*    power saving.
*
* Parameters:
*    active_state     [IN] : TRUE if to keep the bus speed constant
*                            FALSE otherwise
*
* Returns:
*    None
*
******************************************************************************/
void
sdcc_set_port_active
(
   const boolean active_state
)
{
   sdcc_set_tcxo_clk(active_state);
} /* sdcc_set_port_active */

/******************************************************************************
* Name: sdcc_get_relative_card_address
*
* Description:
*    This function gets the relative card address of the memory device
*    indicated in the drive_number parameter from the driver internal data.
*    This relative card address value was saved into the internal data when
*    CMD3 SEND_RELATIVE_ADDR for SD memory device or CMD3 SET_RELATIVE_ADDR
*    for MMC memory device was sent to the memory device during card
*    identification mode.
*
* Parameters:
*    drive_number        [IN] : a valid drive number between 0 and the total
*                               number of card slots - 1
*
* Returns:
*    Relative card address of the memory device indicated in the drive_number
*    parameter
*
******************************************************************************/
uint16
sdcc_get_relative_card_address
(
   const int16 drive_number
)
{
   (void)drive_number;

   /* get the internal data rca */
   return sdcc_pdata.rca[sdcc_pdata.curr_sd_drv];
} /* sdcc_get_relative_card_address */
#endif //#ifdef T_QSC1110
