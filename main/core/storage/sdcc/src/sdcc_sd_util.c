/******************************************************************************
 * File: sdcc_sd_util.c
 *
 * Services: 
 *    This file provides SDCC services for SD device.
 *
 * Description:
 *    This file contains the SD specific utility functions.
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

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_sd_util.c#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/22/08     sc      Initial version

=============================================================================*/

#include "sdcc_sd_util.h"
#include "sdcc_priv.h"
#include "sdcc_util.h"


/******************************************************************************
* Name: sdcc_get_scr
*
* Description:
*    This function reads the SD card configuration register.
*
* Arguments:
*    scr             [OUT] : SD configuration register
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS sdcc_get_scr( sdcc_scr_type *scr )
{
   SDCC_STATUS      rc         = SDCC_NO_ERROR;
   sdcc_cmd_type    sdcc_cmd;
   byte             scr_buff[8];

   memset(scr_buff, 0, 8);
   sdcc_cmd.cmd       = SD_ACMD51_SEND_SCR;
   sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   sdcc_pdata.block_mode = SDCC_MEM_BYTE_MODE;

   /* size of SCR register is 64 bits */
   rc = sdcc_do_transfer(&sdcc_cmd, 
                         (SDCC_READ_TRANSFER_FLAG | SDCC_APP_COMMAND_FLAG),
                         scr_buff, 8/*bytes*/);

   sdcc_pdata.block_mode = SDCC_MEM_BLK_MODE;
   if ( SDCC_NO_ERROR != rc )
   {
      DPRINTF(("Failed in getting SCR register\n"));
      return rc;
   }

   (void) sdcc_decode_scr( scr_buff, scr );

   /* save the sd_spec version */
   sdcc_pdata.mem.spec_vers = scr->sd_spec;

   return rc;
} /* sdcc_get_scr */

/******************************************************************************
* Name: sdcc_decode_scr
*
* Description:
*    This function is to parse out the SD Card Configuration register data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains scr data
*    scr          [OUT] : pointer to the structure to save the scr data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_decode_scr
(
   const byte      *data,
   sdcc_scr_type   *scr
)
{
   uint8 value;

   if ( NULL == scr )
   {
      return FALSE;
   }

   /* in data[0] */
   value = *data;
   /* scr_structure: SCR structure --> scr[63:60] */
   scr->scr_structure = (uint8)(value >> 4);
   /* sd_spec: SD memory card spec version --> scr[59:56] */
   scr->sd_spec = (uint8)(value & 0x0F);

   /* in data[1] */
   value = *(data + 1);
   /* data_stat_after_erase: data status after erases --> scr[55:55] */
   scr->data_stat_after_erase = (uint8)(value >> 7);
   /* sd_security: SD security support --> scr[54:52] */
   scr->sd_security = (uint8)((value & 0x70) >> 4);
   /* sd_bus_widths: DAT bus widths supported --> scr[51:48] */
   scr->sd_bus_widths = (uint8)(value & 0x0F);

   return TRUE;
} /* sdcc_decode_scr */

/******************************************************************************
* Name: sdcc_set_sd_bus_width
*
* Description:
*    This function sets the SD data bus width per bus_width by sending ACMD6 to
*    the SD card.
*
* Arguments:
*    bus_width     [IN] : bus width to set
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_set_sd_bus_width
(
   sdcc_sd_bus_width_type  bus_width
)
{
   SDCC_STATUS     rc = SDCC_ERR_UNKNOWN;
   sdcc_cmd_type   sdcc_cmd;

   if ( SDCC_SD_BUS_WIDTH_1BIT != bus_width &&
        SDCC_SD_BUS_WIDTH_4BIT != bus_width )
   {
      DPRINTF(("Invalid bus_width %d setting\n", bus_width));
      return SDCC_ERR_INVALID_PARAM;
   }
#ifndef T_QSC1100
   HWIO_OUTM(MCI_CLK,
             HWIO_FMSK(MCI_CLK, WIDEBUS),
             (bus_width << HWIO_MCI_CLK_WIDEBUS_SHFT));
#endif
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;
   
   sdcc_cmd.cmd       = SD_CMD55_APP_CMD;
   sdcc_cmd.cmd_arg   = (sdcc_pdata.rca[sdcc_pdata.curr_sd_drv]) << 16;
   rc = sdcc_command(&sdcc_cmd);
   
   sdcc_cmd.cmd       = SD_ACMD6_SET_BUS_WIDTH;
   sdcc_cmd.cmd_arg   = bus_width << 1;
   
   rc = sdcc_command(&sdcc_cmd);
   if ( SDCC_NO_ERROR != rc )
   {
      DPRINTF(("Set bus width %d error %d on SD card\n", bus_width, rc));
   }
#ifdef T_QSC1100
   else
   {
     sdcc_pdata.wide_bus = (SDCC_SD_BUS_WIDTH_4BIT == bus_width)?TRUE:FALSE;
   }
#endif
   return rc;
} /* sdcc_set_sd_bus_width */

