#ifndef __SDCC_SD_UTIL_H
#define __SDCC_SD_UTIL_H
/******************************************************************************
 * File: sdcc_sd_util.h
 *
 * Services: 
 *    This file provides SDCC util definitions for SD device.
 *
 * Description:
 *    This file contains the SD specific utility function prototypes.
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

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_sd_util.h#1 $
  $DateTime: 2008/05/13 20:54:50 $ $Author: shaehnic $

when         who     what, where, why
--------     ---     ----------------------------------------------------------
01/22/08     sc      Initial version

=============================================================================*/

#include "comdef.h"
#include "sdcc_errno.h"

/* Refer to SD2.0 Part 1 Physical Layer Specification, Section 5.6 for */
/* DAT SD bus widths supported */
#define SDCC_SD_WIDE_BUS_WIDTH   5 /* 0101b: bit 0 (DAT0), bit 2 (DAT0-3) */

typedef enum
{
   SDCC_SD_BUS_WIDTH_1BIT,
   SDCC_SD_BUS_WIDTH_4BIT
} sdcc_sd_bus_width_type;

/* Refer to SD2.0 Part 1 Physical Layer Specification, Section 5.6 for */
/* the SD CARD Configuration register (SCR) */
typedef struct _sdcc_scr
{
   uint8  scr_structure;         /* SCR structure */
   uint8  sd_spec;               /* SD memory card - spec. version */
   uint8  data_stat_after_erase; /* data status after erases */
   uint8  sd_security;           /* SD security support */
   uint8  sd_bus_widths;         /* DAT bus widths supports */
} sdcc_scr_type;


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
SDCC_STATUS sdcc_get_scr( sdcc_scr_type *scr );

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
boolean sdcc_decode_scr( const byte      *data,
                         sdcc_scr_type   *scr );

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
SDCC_STATUS sdcc_set_sd_bus_width( sdcc_sd_bus_width_type bus_width );

#endif /* __SDCC_SD_UTIL_H */

