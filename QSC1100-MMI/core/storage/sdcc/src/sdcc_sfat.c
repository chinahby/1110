/**********************************************************************
 * sdcc_sfat.c
 *
 * This file has all the glue functions for SFAT. Inherited mostly from
 * the old MMCC driver.
 *
 * Copyright (C) 2004-2006 Qualcomm, Inc.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History

$Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_sfat.c#3 $
$DateTime: 2008/11/12 10:46:30 $ $Author: vink $

when         who     what, where, why
--------     ---     -----------------------------------------------
2008-10-31   vin     Support for SDCC power collapse
2008-06-19   vin     Added critical section locking
2008-06-19   vin     Removed SDCC Mux features
04/11/07     sc      Ported in MMCPlus support
11/28/06     dng     Added the drive_format_information function back under
                     the feature flag for older M6550 build.
09/07/06     dng     Removed the drive_format_information function.
07/17/06     sc      Added SD2.0 feature
07/17/06     dng     First pass of code review clean up.
11/10/05     hwu     Fixed lint error complains.
07/24/05     hwu     Added check for card type in send for status.
                     Added sdcc_chk_for_sdio().  
11/09/04     hwu     Implemented oem_getsysdate().
06/27/04     hwu     Changed to use "round down" to look up the CHS table.
06/10/04     hwu     Initial Version.
===========================================================================*/
#include "sdcc_api.h"
#include "sdcc_priv.h"
#include "sdcc_util.h"
#include "clk.h"

#define SDCC_MSDOS_START_TIME      1980

/* This data structure is used to construct a table that specifies
 * the number of heads and sectors per track for various card sizes.
 */
typedef struct card_CHS_t {
    uint16 numberMegBytes;      /* card size in mega-bytes */
    uint8  numberHeads;         /* number of  Heads   */
    uint8  secPerTrack;         /* sectors per track  */
}card_CHS;

/*lint -save -e641 Suppress 'Converting enum to int' warning */

#ifdef FEATURE_SDCC_DRIVE_FORMAT_INFO_REQUIRE
/******************************************************************************
* Name: drive_format_information
*
* Description:
*    Provides all the driver format information.
*
* Arguments:
*       driveno  Drive Number
*       n_heads
*       sec_ptrack
*       n_cyls
*
* Returns:
*        None
*
******************************************************************************/
void
drive_format_information (int16 driveno, uint16 *n_heads, 
                               uint16 *sec_ptrack, uint16 *n_cyls)
{

  boolean found        = FALSE;
  uint32  total_mbytes  = 0;
  int  i;
  uint32  card_size;
  uint32  block_len;
  int     CHS_Table_size = 0;

  /*---------------------------------------------------------------------*/

    card_CHS CHS_Table[] = {
        {2,  2, 16},                         /*    2 MByte card */
        {16, 2, 32},                         /*   16 MByte card */
        {32, 4, 32},                         /*   32 MByte card */
        {128, 8, 32},                        /*  128 MByte card */
        {256, 16, 32},                       /*  256 MByte card */
        {504, 16, 63},                       /*  504 MByte card */
        {1008, 32, 63},                      /* 1008 MByte card */
        {2016, 64, 63},                      /* 2016 MByte card */
        {2048, 128, 63},                     /* 2048 MByte card */
        {4096, 128, 63 },                    /* 4096 MByte card */
        {0xFFFF, 0, 0}                       /* dummy entry     */
    };
    
    CHS_Table_size = sizeof(CHS_Table)/sizeof(card_CHS);

    /* Determine CHS by lookup table based on SD standard.
     * Number of bytes varies a bit by vendor, so we round down to
     * the lower entry in the table.
     */
    card_size = sdcc_ioctl(0, SDCC_CARD_SIZE);
    block_len = sdcc_ioctl(0, SDCC_BLOCK_LEN);
    
    total_mbytes   = (card_size * block_len) / 1024 / 1024; 
    for (i = 1; i < CHS_Table_size; i++)
    {
        if ((total_mbytes >= CHS_Table[i-1].numberMegBytes)  &&
            (total_mbytes < CHS_Table[i].numberMegBytes))
        {
            found = TRUE;
            break;
        }
    }

    if (found)
    {
        *n_heads    = CHS_Table[i].numberHeads;
        *sec_ptrack = CHS_Table[i].secPerTrack;
        
        /* totalLBA = heads * cylinders * sectors per track */
        *n_cyls = card_size /((*n_heads) * (*sec_ptrack));
    }

}/* sdcc_drive_format_information */


#endif

/******************************************************************************
* Name: get_interface_error
*
* Description:
*    Returns the error code
*
* Arguments:
*       driveno  Drive Number
*
* Returns:
*       Error code
*
******************************************************************************/
int16
get_interface_error (int16 driveno)
{
    uint32 errno = 0;
    
    errno = sdcc_ioctl(driveno, SDCC_GET_ERROR);
        
    return(errno);

} /* get_interface_error */

/******************************************************************************
* Name: get_extended_error
*
* Description:
*    Returns the error code
*
* Arguments:
*       driveno  Drive Number
*
* Returns:
*       Error code
*
******************************************************************************/
int16
get_extended_error (int16 driveno)
{
    uint32 errno = 0;
    
    errno = sdcc_ioctl(driveno, SDCC_GET_ERROR);
    return errno;

} /* get_extended_error */

/******************************************************************************
* Name: system_controller_close
*
* Description:
*    Closes the sdcc controller
*
* Arguments:
*       driveno  Drive Number
*
* Returns:
*      
******************************************************************************/
void
system_controller_close (int16 driveno)
{
    (void) sdcc_close(0);
    return;

}/* system_controller_close */

/******************************************************************************
* Name: mmc_send_status
*
* Description:
*    Checks the status of the sd/mmc card.
*
* Arguments:
*       driveno  Drive Number
*
* Returns:
*      
******************************************************************************/
boolean
mmc_send_status (void)
{
   SDCC_STATUS        status = SDCC_ERR_UNKNOWN;
   SDCC_CARD_TYPE     card_type;
   boolean            ret;

   if (sdcc_pdata.slot_state_changed)
      return FALSE;

   sdcc_enter_crit_sect();
   do
   {
      card_type = (SDCC_CARD_TYPE)sdcc_ioctl(0, SDCC_GET_TYPE);

      if ((SDCC_CARD_MMC == card_type) ||
          (SDCC_CARD_MMCHC == card_type) ||
          (SDCC_CARD_SD == card_type) ||
          (SDCC_CARD_SDHC == card_type ))
      {
         status = (SDCC_STATUS)sdcc_ioctl(0, SDCC_CARD_STATE);
      }
      ret = status ? FALSE : TRUE;
   }
   while (0);
   sdcc_leave_crit_sect();
   return(ret);
}/* mmc_send_status */


/******************************************************************************
* Name: sdcc_chk_for_sdio
*
* Description:
*    Checks for the SDIO card.
*
* Arguments:
*       None
*
* Returns:
*    TRUE  : SDIO card present.
*    FALSE : No SDIO card. 
*      
******************************************************************************/
boolean
sdcc_chk_for_sdio (void)
{
    boolean            rc = FALSE;
    SDCC_CARD_TYPE     card_type;
    
    card_type = (SDCC_CARD_TYPE)sdcc_ioctl(0, SDCC_GET_TYPE);

    if(SDCC_CARD_SDIO == card_type)
        rc = TRUE;
    
    return(rc);
}


/******************************************************************************
* Name: platform_get_ticks
*
* Description:
*    Stub function
*
* Arguments:
*       None
*
* Returns: 0
*     
*      
******************************************************************************/
uint32
platform_get_ticks (void)
{
  return 0;

} /* platform_get_ticks */


/******************************************************************************
* Name: oem_getsysdate
*
* Description:
*    Gets the date and time.
*
* Arguments:
*       tdate
*       ttime
*
* Returns: 0
*     
*      
******************************************************************************/
void
oem_getsysdate (uint16 *tdate, uint16 *ttime)
{
  dword            seconds;
  clk_julian_type  julian;
  
  /* get the real time in Julian's time */
  seconds = clk_read_secs();
  clk_secs_to_julian(seconds, &julian);

  /* FAT16 Date/Time format:
     date: year[15:9],  month[8:5],   day[4:0]  --> 2Bytes
     time: hour[15:11], minute[10:5], secs[4:0] --> 2Bytes
  */  
  *ttime = (julian.hour << 11)  |
           (julian.minute << 5) |
           (julian.second >> 1);
  
  *tdate = ((julian.year - SDCC_MSDOS_START_TIME ) << 9)  |
            (julian.month << 5) |
            julian.day;

} /* oem_getsysdata */
/*lint -restore */
