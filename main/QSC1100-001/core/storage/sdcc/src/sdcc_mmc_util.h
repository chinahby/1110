#ifndef __SDCC_MMC_UTIL_H
#define __SDCC_MMC_UTIL_H
/**********************************************************************
 * File: sdcc_mmc_util.h
 *
 * Services: 
 *    This file provides SDCC util definitions for MMC device.
 *
 * Description:
 *    This file contains the MMC specific utility function prototypes.
 *
 *
 *    Copyright (C) 2007 QUALCOMM Incorporated.
 *              All Rights Reserved.
 *           QUALCOMM Proprietary/GTDR
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_mmc_util.h#2 $
  $DateTime: 2008/06/18 10:12:18 $ $Author: shermenc $

when         who     what, where, why
----------   ---     -----------------------------------------------
2008-06-11   sc      Ported card detection changes
04/12/07     sc      Initial version

==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#include "customer.h"
#include "comdef.h"


/*************************************************************/
/*               DEFINITIONS AND CONSTANTS                   */
/*************************************************************/
/* Per section 9.3 in MMC v4.2 Specification, the card shall complete */
/* initialization within 1 second from the first CMD1 to card ready. */
#define SDCC_MMC_DETECT_MAX_BUSY_MS           1000

/* time interval (in ms) between the CMD1 retry */
#define SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL   50

/* SDCC_MMC_DETECT_MAX_RETRIES is derived from the initialization within */
/* 1 second from the first CMD1 to card ready. */
/* That is 1000 milli-second = 20 x 50 milli-seond where 50 milli-second is */
/* wait time between each CMD1 retry. */
/* Refer to section 9.3 in MMC v4.2 Specification */
#define SDCC_MMC_DETECT_MAX_RETRIES \
   (SDCC_MMC_DETECT_MAX_BUSY_MS / SDCC_MMC_DETECT_BUSY_RETRY_INTERVAL)

/* voltage 2.7-3.6 and high capacity support */
/* Refer to section 5.1 in MMC v4.2 Specification */
#define SDCC_MMC_HCS_NEGOTIATE_OCR          0x40FF8000


/*************************************************************/
/*                        ENUMERATIONS                       */
/*************************************************************/
/* MMC bus width */
typedef enum sdcc_mmc_buswidth
{
   SDCC_MMC_BUSWIDTH_1BIT,
   SDCC_MMC_BUSWIDTH_4BIT,
#ifdef T_MSM7600
#error code not present
#endif
} SDCC_MMC_BUSWIDTH;


/******************************************************************/
/*                     TYPES DECLARATIONS                         */
/******************************************************************/
/* The Extended CSD register is according MMC spec. v4.2. */
typedef struct sdcc_ext_csd {
   uint8  s_cmd_set;            /* supported command sets */
   uint32 sec_count;            /* sector count */
   uint8  min_perf_w_8_52;      /* min write performance for 8bit @52MHz */
   uint8  min_perf_r_8_52;      /* min read performance for 8bit @52MHz */
   uint8  min_perf_w_8_26_4_52; /* min write performance for 8bit @26MHz */
                                /* or 4bit @52MHz */
   uint8  min_perf_r_8_26_4_52; /* min read performance for 8bit @26MHz */
                                /* or 4bit @52MHz */
   uint8  min_perf_w_4_26;      /* min write performance for 4bit @26MHz */
   uint8  min_perf_r_4_26;      /* min read performance for 4bit @26MHz */
   uint8  pwr_cl_26_360;        /* power class for 26MHz @3.6V */
   uint8  pwr_cl_52_360;        /* power class for 52MHz @3.6V */
   uint8  pwr_cl_26_195;        /* power class for 26MHz @1.95V */
   uint8  pwr_cl_52_195;        /* power class for 52MHz @1.95V */
   uint8  card_type;            /* card type: 1 means 26MHz, 3 means 52MHz */
   uint8  csd_structure;        /* CSD structure version */
   uint8  ext_csd_rev;          /* extended CSD revision */
   uint8  cmd_set;              /* command set */
   uint8  cmd_set_rev;          /* command set revision */
   uint8  power_class;          /* power class */
   uint8  hs_timing;            /* high speed interface timing */
   uint8  bus_width;            /* bus width mode */
   uint8  erased_mem_cont;      /* erased memory content */
} sdcc_ext_csd_type;


/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/
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
boolean sdcc_config_mmc_bus_width( int16               driveno,
                                   SDCC_MMC_BUSWIDTH   bus_width );

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
SDCC_STATUS sdcc_config_mmc_modes_segment( void );

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
SDCC_STATUS sdcc_get_ext_csd( byte *buff );

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
boolean sdcc_decode_ext_csd( const byte          *data,
                             sdcc_ext_csd_type   *ext_csd );

#endif /* __SDCC_MMC_UTIL_H */

