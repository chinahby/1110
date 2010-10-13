#ifndef __SDCC_COMMAND_UTIL_H
#define __SDCC_COMMAND_UTIL_H
/******************************************************************************
 * File: sdcc_common_util.h
 *
 * Services: 
 *    Utility for raw command API
 *
 * Description:
 *    This file contains the raw command utility function prototypes.
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

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_command_util.h#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/21/08     sc      Initial version

=============================================================================*/

#include "sdcc_command_api.h"
#include "sdcc_priv.h"


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
boolean sdcc_dma_usable(const uint32 data_length, const uint32 *buffer);

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
void sdcc_config_dma_ctrl(const int16   drive_number,
                          const uint32  data_length,
                          const uint32  data_xfer_direction,
                          const boolean enable_dma);

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
void sdcc_map_command_descriptor_type_to_cmd_type(
   const sdcc_command_descriptor_type *command_descriptor,
   const boolean                       wait_for_program_done,
   sdcc_cmd_type                      *sdcc_cmd);

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
sdcc_raw_command_core(const int16                         drive_number,
                      const sdcc_command_descriptor_type *command_descriptor,
                      const boolean                       wait_for_program_done,
                      sdcc_command_response_type         *command_response);

#endif /* __SDCC_COMMAND_UTIL_H */

