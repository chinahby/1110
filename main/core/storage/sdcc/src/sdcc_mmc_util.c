/**********************************************************************
 * File: sdcc_mmc_util.c
 *
 * Services: 
 *    This file provides SDCC services for MMC device.
 *
 * Description:
 *    This file contains the MMC specific utility function.
 *
 *
 * Copyright (c) 2007-2008 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_mmc_util.c#2 $
$DateTime: 2008/06/18 10:12:11 $ $Author: shermenc $

when       who     what, where, why
----------------------------------------------------------------------
2008-06-11 sc      Ported MMC card size calculation fix
03/05/08   sc      Removed SDCC_PARAM_NOT_REF macro
04/12/07   sc      Initial version

===========================================================================*/
#include "sdcc_api.h"
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "sdcc_mmc_util.h"


/*lint -save -e641 Suppress 'Converting enum to int' warning */
/******************************************************************************
* Name: sdcc_config_mmc_bus_width
*
* Description:
*    This function configures the MMC bus width.
*
* Arguments:
*    driveno          [IN] : drive number
*    bus_width        [IN] : MMC bus width
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_config_mmc_bus_width
(
   int16               driveno,
   SDCC_MMC_BUSWIDTH   bus_width
)
{
   (void)driveno;

   if ( SDCC_MMC_BUSWIDTH_1BIT != bus_width &&
        SDCC_MMC_BUSWIDTH_4BIT != bus_width
#ifdef T_MSM7600
#error code not present
#endif
      )
   {
      /* invalid mmc bus width */
      return FALSE;
   }
   /* program the bus width for the mmc card */
   HWIO_OUTM(MCI_CLK,
             HWIO_FMSK(MCI_CLK, WIDEBUS),
             (bus_width << HWIO_MCI_CLK_WIDEBUS_SHFT));

   return TRUE;
} /* sdcc_config_mmc_bus_width */

/******************************************************************************
* Name: sdcc_config_mmc_modes_segment
*
* Description:
*    This function is to programm the modes segment in EXT_CSD register of 
*    the MMC card.  It gets the EXT_CSD information and sends SWITCH CMD6 to 
*    configure the HS_TIMING and BUS_WIDTH.  It also calculates the card size
*    for MMC card that is higher than 2GB density.
*
* Arguments:
*    None
*
* Returns:
*    Returns error code
*
* Note:
*    Card is in data transfer mode.
*
******************************************************************************/
SDCC_STATUS 
sdcc_config_mmc_modes_segment( void )
{
   SDCC_STATUS         rc          = SDCC_NO_ERROR;
   sdcc_cmd_type       sdcc_cmd;
   SDCC_MMC_BUSWIDTH   bus_width;
   byte                data[512];
   sdcc_ext_csd_type   ext_csd;

   /*----------------------------------------------------------------------*/

   /* If MMC denotes a System Specification version 4.0 or higher, */
   /* this indicates the support of the SEND_EXT_CSD command (CMD8). */
   /* Thus, only proceed to get extended CSD register if MMC spec vers */
   /* is 4.0 or higher.  Otherwise, simply return with success. */
   if ( sdcc_pdata.mem.spec_vers < 4 )
   {
      return rc;
   }

   /* get the MMC EXT_CSD information */
   memset(data, 0, 512);
   memset(&ext_csd, 0, sizeof(sdcc_ext_csd_type));
   rc = sdcc_get_ext_csd(data);

   if ( SDCC_NO_ERROR == rc )
   {
      /* decode ext_csd */
      (void) sdcc_decode_ext_csd(data, &ext_csd);

      /* sanity check for high density MMC card that the sec_count */
      /* should not be 0 */
      if ( SDCC_CARD_MMCHC == sdcc_pdata.card_type && 
           0 == ext_csd.sec_count )
      {
         rc = SDCC_ERR_UNKNOWN;
         return rc;
      }

      if ( 0 != ext_csd.sec_count )
      {
         /* calculate the card size for high density MMC card */
         /* device density is calculated from the EXT_CSD by */
         /* multiplying the value of the sector count by 512B/sector */
         /* for example, 2Tera bytes is (4294967296 x 512B) */
         sdcc_pdata.mem.card_size = 512 *
            (ext_csd.sec_count / SDCC_DEFAULT_BLK_LENGTH_SIZE);

         /* Per MMC v4.2 System Specification */
         /* In case of multi card system, a sector access mode card (>2GB) */
         /* is not necessarily recognized as a sector access mode card */
         /* during the initialization.  This needs to be reconfirmed by */
         /* reading the SEC_COUNT from the EXT_CSD register. */
         /* confirmed device is MMCHC */
         sdcc_pdata.card_type = SDCC_CARD_MMCHC;
         DPRINTF(("MMCHC card found\n"));
      }

      /* check the freq that the card can support */
      /* 1 means 26MHz and 3 means 52MHz */
      if ( ext_csd.card_type == 3 )
      {
         /* set clock freq to high speed */
         sdcc_pdata.mem.clk_freq = 1;

         if ( SDCC_CARD_MMCHC == sdcc_pdata.card_type )
         {
            /* confirmed device is MMCPlus as well as MMCHC */
            DPRINTF(("MMCPlus High Density card found\n"));
         }
         else
         {
            /* confirmed device is MMCPlus */
            DPRINTF(("MMCPlus card found\n"));
         }
      }

      /* re-program the clock according to the MMC card type */
      sdcc_config_clk(SDCC_DATA_TRANSFER_MODE, sdcc_pdata.card_type);

      /* configure MMC BUS_WIDTH to 4-bit mode */
      bus_width          = SDCC_MMC_BUSWIDTH_4BIT;
      sdcc_cmd.cmd       = SD_CMD6_SWITCH;
      sdcc_cmd.cmd_arg   = (SDCC_MMCPLUS_CHANGE_BUSWIDTH 
                           | ((uint32)(bus_width << 8)));
      sdcc_cmd.resp_type = SDCC_RESP_SHORT;
      sdcc_cmd.prog_scan = 0;

      rc = sdcc_command(&sdcc_cmd);
      if ( SDCC_NO_ERROR != rc )
      {
         return rc;
      }

      /* give 1ms for the device to programming the bus width */
      sdcc_mdelay(1);

      /* configure MMC bus width */
      (void) sdcc_config_mmc_bus_width(0 /*don't care*/, bus_width);
   }

   return rc;
} /* sdcc_config_mmc_modes_segment */

/******************************************************************************
* Name: sdcc_get_ext_csd
*
* Description:
*    This function gets the EXT_CSD register as a block of data from the
*    MMCPlus card.  It performs the adtc command with the read operation.
*
* Arguments:
*    buff         [OUT] : pointer to buffer for the incoming data
*
* Returns:
*    Returns error code
*
******************************************************************************/
SDCC_STATUS
sdcc_get_ext_csd
(
   byte *buff
)
{
   sdcc_cmd_type   sdcc_cmd;
   SDCC_STATUS     rc        = SDCC_ERR_READ_FIFO;

   /*----------------------------------------------------------------------*/

   sdcc_cmd.cmd       = SD_CMD8_SEND_EXT_CSD;
   sdcc_cmd.cmd_arg   = MCI_ARGU_NULL;
   sdcc_cmd.resp_type = SDCC_RESP_SHORT;
   sdcc_cmd.prog_scan = 0;

   rc = sdcc_do_transfer(&sdcc_cmd, SDCC_READ_TRANSFER_FLAG, buff, 1/*block*/);

   return rc;
} /* sdcc_get_ext_csd */

/******************************************************************************
* Name: sdcc_decode_ext_csd
*
* Description:
*    This function is to parse out the Extended CSD data.
*
* Arguments:
*    data          [IN] : pointer to buffer that contains ext_csd data
*    ext_csd      [OUT] : pointer to the structure to save the ext csd data to
*
* Returns:
*    TRUE if successful
*    FALSE if failed
*
******************************************************************************/
boolean
sdcc_decode_ext_csd
(
   const byte          *data,
   sdcc_ext_csd_type   *ext_csd
)
{
   uint8 value;

   /*----------------------------------------------------------------------*/

   if ( NULL == ext_csd )
   {
      return FALSE;
   }

   /* in data[189] */
   value = *(data + 189);
   /* cmd_set_rev: command set revision --> ext_csd[189] in bytes */
   ext_csd->cmd_set_rev = value;

   /* in data[191] */
   value = *(data + 191);
   /* cmd_set: command set --> ext_csd[191] in bytes */
   ext_csd->cmd_set = value;

   /* in data[194] */
   value = *(data + 194);
   /* csd_structure: CSD structure version --> ext_csd[194] in bytes */
   ext_csd->csd_structure = value;

   /* in data[196] */
   value = *(data + 196);
   /* card_type: card type --> ext_csd[196] in bytes */
   ext_csd->card_type = value;

   /* in data[200] */
   value = *(data + 200);
   /* pwr_cl_52_195: power class for 52MHz @ 1.95V --> ext_csd[200] in bytes */
   ext_csd->pwr_cl_52_195 = value;

   /* in data[201] */
   value = *(data + 201);
   /* pwr_cl_26_195: power class for 26MHz @ 1.95V --> ext_csd[201] in bytes */
   ext_csd->pwr_cl_26_195 = value;

   /* in data[202] */
   value = *(data + 202);
   /* pwr_cl_52_360: power class for 52MHz @ 3.6V --> ext_csd[202] in bytes */
   ext_csd->pwr_cl_52_360 = value;

   /* in data[203] */
   value = *(data + 203);
   /* pwr_cl_26_360: power class for 26MHz @ 3.6V --> ext_csd[203] in bytes */
   ext_csd->pwr_cl_26_360 = value;

   /* in data[215:212] */
   value = *(data + 212);
   /* sec_count: sector count --> ext_csd[212] in bytes */
   ext_csd->sec_count = (uint32)value;
   value = *(data + 213);
   /* sec_count: sector count --> ext_csd[213:212] in bytes */
   ext_csd->sec_count |= ((uint32)(value << 8));
   value = *(data + 214);
   /* sec_count: sector count --> ext_csd[214:212] in bytes */
   ext_csd->sec_count |= ((uint32)(value << 16));
   value = *(data + 215);
   /* sec_count: sector count --> ext_csd[215:212] in bytes */
   ext_csd->sec_count |= ((uint32)(value << 24));

   /* in data[504] */
   value = *(data + 504);
   /* s_cmd_set: supported command sets --> ext_csd[504] in bytes */
   ext_csd->s_cmd_set = value;

   return TRUE;
} /* sdcc_decode_ext_csd */
/*lint -restore */

