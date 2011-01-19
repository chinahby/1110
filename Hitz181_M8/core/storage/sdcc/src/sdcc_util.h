#ifndef __SDCC_UTIL_H
#define __SDCC_UTIL_H
/**********************************************************************
 * sdcc_util.h
 *
 * This file provides SDCC external definitions.
 *
 * Copyright (c) 2006-2008 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE


when         who     what, where, why
--------     ---     -----------------------------------------------
2008-10-31   vin     Support for SDCC power collapse
2008-08-25   vin     Moved sdcc_isr to this file, added sdio_isr
2008-06-19   vin     Added critical section locking
2008-06-19   vin     Removed SDCC Mux features
01/21/08     sc      Updated sdcc_set_blksz
10/15/07     umr     Move sdcc_find_card () to sdcc_api.h
04/11/07     sc      Ported in MMCPlus support
11/28/06     dng     Added the CLK_REGIME defines.
11/07/06     dng     Added the sdcc_set_tcxo_clk function prototype.
07/06/06     dng     Created
09/05/06     dng     Changed the function sdcc_process_dma to sdcc_process_interrupts.

==================================================================*/
#include "customer.h"
#include "comdef.h"


#ifdef FEATURE_DUAL_CLK_RGM
   #define CLK_REGIME_ENABLE  clk_regime_msm_enable
   #define CLK_REGIME_DISABLE  clk_regime_msm_disable
#else
   #define CLK_REGIME_ENABLE   clk_regime_enable
   #define CLK_REGIME_DISABLE  clk_regime_disable
#endif

extern void  mmu_invalidate_data_cache_lines(uint32* start_ptr,
                                             uint32  n_bytes);


/******************************************************************************
* Name: sdcc_blk_in_bits
*
* Description:
*    This function is to calculate the block size in terms of bits.
*
* Returns:
*    Returns the bits.
*
* Note:
******************************************************************************/
uint32 sdcc_blk_in_bits(uint32 size);


/******************************************************************************
* Name: sdcc_cmd_send_verify
*
* Description:
*    This function is to make sure the command is sent out and then
*    clear the corresponding bit in the status.
*
* Returns:
*    Returns error code.
*
* Note:
*    For slow clock(400KHz), this is needed.
******************************************************************************/
SDCC_STATUS sdcc_cmd_send_verify( void );


/******************************************************************************
* Name: sdcc_config_dma
*
* Description:
*    This function is to configure SDCC controller for DMA transfer.
*
* Note:
*    Use the defaults for now.
******************************************************************************/
void sdcc_config_dma( void );

/******************************************************************************
* Name: sdcc_config_clk
*
* Description:
*    This function is to configure the SDCC controller clock depending
*    on current mode.
*
******************************************************************************/
void sdcc_config_clk(
    SDCC_CARD_MODE   mode,
    SDCC_CARD_TYPE   card_type
);

/******************************************************************************
* Name: sdcc_config_rca
*
* Description:
*    This function is to set/get the RCA from the card. 
*
******************************************************************************/
SDCC_STATUS sdcc_config_rca( void );


/******************************************************************************
* Name: sdcc_get_resp
*
* Description:
*    This function reads out  the command response. 
*
* Arguments:
*       sdcc_cmd   pointer to command data structure
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_get_resp(sdcc_cmd_type *sdcc_cmd);


/******************************************************************************
* Name: sdcc_poll_status
*
* Description:
*    This function is to poll the SDCC status.
*
* Arguments:
*       sdcc_cmd   pointer to command data structure
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_poll_status(sdcc_cmd_type *sdcc_cmd);


/******************************************************************************
* Name: sdcc_poll_dma
*
* Description:
*    This function is to process DMA data transfer by polling.
*
* Arguments:
*
* Returns:
*    Returns error code.
*
* Note:
*    Not used.
******************************************************************************/
SDCC_STATUS sdcc_poll_dma( void );

/******************************************************************************
* Name: sdcc_setup_data_xfer
*
* Description:
*    This function sets up the controller prior to data transfer. 
*
* Arguments:
*       direction  data read or data write
*       n_sector   number of sectors to be read
*
* Returns:
*       TRUE  if sucessful
*       FALSE if failure
*
******************************************************************************/
uint32
sdcc_setup_data_xfer
(
    uint32    direction,
    uint16    units
);


/******************************************************************************
* Name: sdcc_command
*
* Description:
*    This function is to send the command and poll the status. 
*
* Arguments:
*    sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_command(sdcc_cmd_type   *sdcc_cmd);


/******************************************************************************
* Name: sdcc_command_sdio
*
* Description:
*    This function is to send the command and poll the status. 
*
* Arguments:
*    sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_command_sdio(sdcc_cmd_type   *sdcc_cmd);

/******************************************************************************
* Name: sdcc_send_cmd
*
* Description:
*    This function is to send out the command. 
*
* Arguments:
*    sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*       None
*       
*
******************************************************************************/
void sdcc_send_cmd(sdcc_cmd_type *sdcc_cmd);


/******************************************************************************
* Name: sdcc_send_status
*
* Description:
*    This function is to let the device(currently selected device) send
*    the device status. 
*
* Arguments: None
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_CARD_STATUS sdcc_send_status( void );

/******************************************************************************
* Name: sdcc_wait_prog_done
*
* Description:
*    This function is to poll the status to make sure the device is not
*    busy. CMD12(stop) and CMD38(erase) need to call this to make sure
*    the device programming has finished.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_wait_prog_done( void );


/******************************************************************************
* Name: sdcc_process_interrupts
*
* Description:
*    This function is to process all sdcc interrupts.
*
* Arguments:
*     sdcc_cmd   pointer to sdcc_cmd.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_process_interrupts( sdcc_cmd_type   *sdcc_cmd);

/******************************************************************************
* Name: sdcc_enable_int
*
* Description:
*    This function is to enable the SDCC host interrupts.
*
* Arguments:
*    int_mask   the interrupts to be turned on
* Retuns:
*
* Notes:
*    This function is in interrupt context. 
******************************************************************************/
void sdcc_enable_int( uint32 int_mask );

/******************************************************************************
* Name: sdcc_disable_int
*
* Description:
*    This function is to disable the SDCC host interrupts.
*
* Arguments:
*   int_mask    the interrupts to be masked off
* Retuns:
*
* Notes:
*    This function is in interrupt context. 
******************************************************************************/
void sdcc_disable_int( uint32 int_mask );


/******************************************************************************
* Name: sdcc_wait_card_ready
*
* Description:
*    This function is to check the card busy status by calling "send
*    status" command.
*
* Arguments:
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_wait_card_ready(void);

/******************************************************************************
* Name: sdcc_select_card
*
* Description:
*    This function is to select/deselect the specified card.
*
* Arguments:
*    select  select if TRUE, deselect if FALSE
*    rca     card RCA address.
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_select_card
(
  uint16    rca,
  uint32    select
);


/******************************************************************************
* Name: sdcc_init_memory_device
*
* Description:
*    This function sends CMD2 to get the device ID and product related
*    information. It then configures the device Relative Card Address and
*    completes the card identification process.
*
* Returns:
*    Returns error code.
*
* Note:
*   Card is in Identification mode. 
*****************************************************************************/
SDCC_STATUS sdcc_init_memory_device(void);

/******************************************************************************
* Name: sdcc_get_memory_info.
*
* Description:
*    This function is to get the specific information related to the
*    memory device via CMD9 and/or CMD10 before it goes into data
*    transfer mode. 
*
* Arguments:
*
* Returns:
*    Returns error code.
*
* Note:
*    Card is in stand-by mode.
******************************************************************************/
SDCC_STATUS sdcc_get_memory_info(void);

/******************************************************************************
* Name: sdcc_config_memory_device.
*
* Description:
*    This function is to programm the card into data transfer mode. It
*    selects the card, sets up the bus width and block data transfer mode. 
*
* Arguments:
*
* Returns:
*    Returns error code.
*
* Note:
*    Card is in data transfer mode.
******************************************************************************/
SDCC_STATUS sdcc_config_memory_device(void);

/******************************************************************************
* Name: sdcc_set_blksz
*
* Description:
*    This function is to set the specified block length if block mode is
*    supported.  The default block length is fixed to 512Bytes.  If
*    block length is set larger than 512Bytes, the card sets the 
*    BLOCK_LEN_ERROR bit.
*
* Arguments:
*       block_length   block length in bytes
*
* Returns:
*    returns status code.
*
******************************************************************************/
SDCC_STATUS sdcc_set_blksz ( uint32 block_length );

/******************************************************************************
* Name: sdcc_complete_data_xfer
*
* Description:
*    This function is to finish up data transfer after the data has been
*    successfully transferred. If this is a multi-block data transfer,
*    we need to send CMD12 to stop the data transfer. 
*
* Arguments:
*       direction   read or write
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS sdcc_complete_data_xfer(uint32   direction,
                                    boolean  multi_block);

/******************************************************************************
 * Name: sdcc_send_app_command
 *
 * Description:
 *    This function is to send an App-command to the memory device.
 *
 * Arguments:
 *    cmd_ptr          [IN] : pointer to a structure describing the
 *                            App-Command to send.  The command should
 *                            be one of the SD_ACMDXXXX enums.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_send_app_command( sdcc_cmd_type *cmd_ptr );

/******************************************************************************
 * Name: sdcc_do_transfer
 *
 * Description:
 *    This is a generic function to perform a read or write data transfer on
 *    the memory device.  The flags value contains a flag for whether we are
 *    sending a read or write transfer.
 *
 * Arguments:
 *    xfer_cmd_ptr         [IN] : command to send to the device to perform 
 *                                the read or write
 *    xfer_flags           [IN] : SDCC_TRANSFER_FLAG_IS_APP_COMMAND - Set this
 *                                bit to send the command as an app-command
 *                                SDCC_TRANSFER_FLAG_IS_READ_TRANSFER - Set
 *                                this bit for read transfers, clear it for
 *                                write transfers
 *    buff             [IN/OUT] : buffer of data to read or write
 *    xfer_size            [IN] : When in block mode, this is the number of
 *                                blocks to transfer.  When in byte mode, it
 *                                is the number of bytes to transfer.
 *
 * Returns:
 *    Returns error code
 *
 *****************************************************************************/
SDCC_STATUS sdcc_do_transfer( sdcc_cmd_type     *xfer_cmd_ptr,
                              uint32             xfer_flags,
                              byte              *buff,
                              uint16             xfer_size );

/******************************************************************************
* Name: sdcc_decode_csd
*
* Description:
*    This function is to parse out the CSD data.
*
* Arguments:
*       data  pointer to data read in from response registers.
*       csd   pointer to the stucture to save the csd data to.
*
******************************************************************************/
void
sdcc_decode_csd(
    const uint32   *data,
    sdcc_csd_type  *csd
);

/******************************************************************************
* Name: sdcc_read_fifo
*
* Description:
*    This function is to process "read" in non-dma mode. It reads data
*    out of the FIFO.
*
* Arguments:
*    buff        pointer of the data buffer
*    length      data length
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_read_fifo
(
    byte      *buff,
    uint16     length
);

/******************************************************************************
* Name: sdcc_write_fifo
*
* Description:
*    This function is to process "write" in non-dma mode. It writes data
*    directly to FIFO. 
*
* Arguments:
*    buff        pointer of the data buffer
*    length      data length
*
* Returns:
*    Returns error code.
*
******************************************************************************/
SDCC_STATUS
sdcc_write_fifo
(
    byte     *buff,
    uint16    length
);
/******************************************************************************
* Name:sdcc_set_tcxo_clk
*
* Description:
*    This function is set the TCXO clock based on the input.

*
* Arguments:
*        boolean set_status
         TRUE  : maintain the TCXO clock high
         FALSE : allow sleep to pull the TCXO clock low to save power
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_set_tcxo_clk(boolean set_status);

/******************************************************************************
* Name: sdcc_enter_crit_sect
*
* Description:
*    Enable exclusive access to SD controller
*
* Arguments:
*    None
* Returns:
*    None
*
******************************************************************************/
void sdcc_enter_crit_sect(void);

/******************************************************************************
* Name: sdcc_leave_crit_sect
*
* Description:
*   Disable exclusive access to the SD controller
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_leave_crit_sect(void);

/******************************************************************************
* Name: sdcc_init_crit_sect
*
* Description:
*    Create and initialize a critical section for exclusive access to 
*    the SD controller. If critical section already exists, do nothing
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_init_crit_sect(void);

/******************************************************************************
* Name: sdcc_isr
*
* Description:
*    This function is to process sdcc interrupts..
*
* Arguments:
* Retuns:
* Notes:
    This function is in interrupt context. 
******************************************************************************/
void sdcc_isr(void);

/******************************************************************************
* Name: sdio_isr
*
* Description:
*    This function processes the SDIO interrupt
*
* Arguments:
* Retuns:
* Notes:
    This function is in interrupt context. 
******************************************************************************/
void sdio_isr(void);

/******************************************************************************
* Name: sdcc_silent_reinit
*
* Description: 
*    Initializes the card.
*    This function is called if the driver determined that the slot was 
*    shutdown by sdcc_activity_timeout_event() and the card was not physically
*    removed after that. 
*
* Arguments:
*    None
*
* Returns:
*    TRUE if successful, FALSE otherwise
*
******************************************************************************/
boolean sdcc_silent_reinit(void);

/******************************************************************************
* Name: sdcc_slot_isr
*
* Description: This ISR is called when either a card is inserted or removed
*    
*
* Arguments:
*    None
*
* Returns:
*    None
*
******************************************************************************/
void sdcc_slot_isr(void);

#endif /* __SDCC_UTIL_H */

