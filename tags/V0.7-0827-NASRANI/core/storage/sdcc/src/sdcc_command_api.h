#ifndef __SDCC_COMMAND_API_H
#define __SDCC_COMMAND_API_H
/******************************************************************************
 * File: sdcc_common_api.h
 *
 * Services: 
 *    Public API header for raw command
 *
 * Description:
 *    This file contains the raw command function prototype.
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

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_command_api.h#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/18/08     sc      Initial version

=============================================================================*/

#include "comdef.h"
#include "sdcc_errno.h"

/* maximum command response buffer size */
/* Each command response buffer size is 32 bits. */
#define SDCC_MAX_RESPONSE_BUFFER_SIZE   4

/* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.7.1 for */
/* these 4 kinds of command types defined to control memory card */
typedef enum
{
   /* bc, no response */
   SDCC_MEMORY_BROADCAST_COMMAND,
   /* bcr */
   SDCC_MEMORY_BROADCAST_COMMAND_WITH_RESPONSE,
   /* ac, no data transfer on data lines */
   SDCC_MEMORY_ADDRESSED_COMMAND,
   /* adtc, data transfer on data lines */
   SDCC_MEMORY_ADDRESSED_DATA_TRANSFER_COMMAND
} sdcc_memory_command_type;

/* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.9 for */
/* response types for memory card */
typedef enum
{
   /* command does not require response */
   SDCC_RESPONSE_NONE,
   /* status of the card is coded in 32 bits */
   SDCC_R1,
   /* same as R1 with an optional busy signal transmitted on data line */
   SDCC_R1b,
   /* response to CID, CSD register */
   SDCC_R2,
   /* response to OCR register */
   SDCC_R3,
   SDCC_R4,
   SDCC_R5,
   SDCC_R6,
   /* response to SD card interface condition */
   SDCC_R7
} sdcc_response_type;

/* The following enumeration type is for command type adtc. */
/* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.7.1 for */
/* adtc command type. */
/* When command type is adtc, adtc_direction in command descriptor should */
/* be filled in. */
typedef enum
{
   SDCC_ADTC_READ, /* data transfer from memory device to host */
   SDCC_ADTC_WRITE /* data transfer from host to memory device */
} sdcc_adtc_direction_type;

/* The following enumeration type is for command type adtc. */
/* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.7.1 for */
/* adtc command type. */
/* When command type is adtc, adtc_memory_mode in command descriptor */
/* should be filled in. */
typedef enum
{
   /* data transfer in block mode and DMA would be used */
   /* Default block size is 512 bytes. */
   /* Block length can be set by sending CMD16 SET_BLOCKLEN command and then */
   /* calling function sdcc_set_block_length. */
   SDCC_ADTC_MEMORY_BLOCK_MODE,
   /* data transfer in byte mode and FIFO would be used */
   SDCC_ADTC_MEMORY_BYTE_MODE
} sdcc_adtc_memory_mode_type;

/* command descriptor to be populated by the SDCC client */
/* to pass in as an argument in function call sdcc_raw_command */
typedef struct _sdcc_command_descriptor
{
   /* index of the command between 0 and 63 */
   /* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.7.4 for */
   /* detailed command description */
   uint32                                  command_index;
   /* type of the command */
   /* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.7.1 */
   sdcc_memory_command_type                command_type;
   /* some commands need an argument (e.g. address) */
   /* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.7.4 for */
   /* detailed command description */
   uint32                                  command_argument;
   /* all response are sent via command line */
   /* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.9 for */
   /* response types for memory card */
   sdcc_response_type                      response_type;
} sdcc_command_descriptor_type;

/* command with data descriptor to be populated by the SDCC client */
/* to pass in as an argument in function call sdcc_raw_command_with_data */
typedef struct _sdcc_command_with_data_descriptor
{
   /* refer to the sdcc_command_descriptor_type above */
   sdcc_command_descriptor_type            command_descriptor;
   /* indicate the data transfer direction -- to or from the memory device */
   sdcc_adtc_direction_type                adtc_direction;
   /* memory access mode for data transfer -- block mode or byte mode */
   sdcc_adtc_memory_mode_type              adtc_memory_mode;
   /* If adtc_memory_mode is set to SDCC_ADTC_MEMORY_BLOCK_MODE for read or */
   /* write data transfer, the unit of adtc_data_length is number of blocks. */
   /* Each block has 512 bytes by default. */
   /* Block length can be set by sending CMD16 SET_BLOCKLEN command and then */
   /* calling function sdcc_set_block_length. */
   /* If adtc_memory_mode is set to SDCC_ADTC_MEMORY_BYTE_MODE for read or */
   /* write data transfer, the unit of adtc_data_length is number of bytes. */
   uint32                                  adtc_data_length;
} sdcc_command_with_data_descriptor_type;

/* contains the response to command via the command line from memory device */
typedef struct _sdcc_command_response
{
   /* Refer to SD2.0 Part 1 Physical Layer Specification, Section 4.9 for */
   /* response descriptions for memory card */
   uint32     command_response_buffer[SDCC_MAX_RESPONSE_BUFFER_SIZE];
} sdcc_command_response_type;


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
sdcc_raw_command(const int16                         drive_number,
                 const sdcc_command_descriptor_type *command_descriptor,
                 const boolean                       wait_for_program_done,
                 sdcc_command_response_type         *command_response);

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
*    transmission is to read data from the memory device.  The default block
*    length is 512 bytes and can be set by sending CMD16 SET_BLOCKLEN command
*    and then calling function sdcc_set_block_length.  The maximum number of
*    blocks for data transfer is target dependent.  This value is specified as
*    MCI_DATA_LENGTH in each target's Software Interface Manual.  The
*    adtc_data_length must not exceed the specified maximum number of blocks.
*    This function does not verify the validity of the
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
sdcc_raw_command_with_data(
   const int16                   drive_number,
   const sdcc_command_with_data_descriptor_type *command_with_data_descriptor,
   sdcc_command_response_type   *command_response,
   const uint32                 *buffer_data_to_card,
   uint32                       *buffer_data_from_card);

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
SDCC_STATUS sdcc_set_block_length(const int16   drive_number,
                                  const uint32  block_length);

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
void sdcc_set_port_active(const boolean active_state);

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
uint16 sdcc_get_relative_card_address(const int16 drive_number);

#endif /* __SDCC_COMMAND_API_H */

