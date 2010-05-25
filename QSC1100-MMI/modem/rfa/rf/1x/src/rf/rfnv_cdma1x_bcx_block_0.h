#ifndef RFNV_CDMA1X_BCX_BLOCK_0_H
#define RFNV_CDMA1X_BCX_BLOCK_0_H


/*===========================================================================

                         R F  P C S  N V  M a n a g e r

DESCRIPTION
  This header file contains prototypes & definitions necessary to retrieve
  RF bcx_block specific Non-Volatile memory items, and store them in the local
  tables.

Copyright (c) 2008 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x_bcx_block_0.h#2 $ $DateTime: 2008/10/01 18:07:35 $ $Author: vboda $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
09/07/08   vb/vm   Initial Version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "nv.h"
#include "rfnv.h"
#include "rfnv_cdma1x.h"
#include "rficap.h"
#ifdef RF_HAS_RFCONFIG_RX_LNA
#include "rfnv_rfconfig_migration_tracker.h"
#endif /* RF_HAS_RFCONFIG_RX_LNA */

/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/
  
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     CDMA mode LNA on/off thresholds

     LNA rise/fall (dBm to MSM register & NV storage)

        [ (dBm Rx + 106) / 85.333 * 256 ] - 128

     LNA rise/fall (MSM register & NV storage to dBm)

        [ (value + 128) / 256 * 85.333 ] - 106
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* LNA range values
*/
#ifdef RF_HAS_POWER_MODE_LIB
    #define  BCX_BLOCK_LNA_RANGE_1_RISE_V     {{(int1)0xc3,(int1)0xc3,(int1)0xc3},{(int1)0xc3,(int1)0xc3,(int1)0xc3}}
    #define  BCX_BLOCK_LNA_RANGE_2_RISE_V     {{(int1)0xdc,(int1)0xdc,(int1)0xdc},{(int1)0xdc,(int1)0xdc,(int1)0xdc}}
    #define  BCX_BLOCK_LNA_RANGE_3_RISE_V     {{(int1)0x27,(int1)0x27,(int1)0x27},{(int1)0x27,(int1)0x27,(int1)0x27}}
    #define  BCX_BLOCK_LNA_RANGE_4_RISE_V     {{(int1)0x4a,(int1)0x4a,(int1)0x4a},{(int1)0x4a,(int1)0x4a,(int1)0x4a}}
    
    #define  BCX_BLOCK_LNA_RANGE_1_FALL_V     {{(int1)0xbe,(int1)0xbe,(int1)0xbe},{(int1)0xbe,(int1)0xbe,(int1)0xbe}}
    #define  BCX_BLOCK_LNA_RANGE_2_FALL_V     {{(int1)0xd7,(int1)0xd7,(int1)0xd7},{(int1)0xd7,(int1)0xd7,(int1)0xd7}}
    #define  BCX_BLOCK_LNA_RANGE_3_FALL_V     {{(int1)0x1d,(int1)0x1d,(int1)0x1d},{(int1)0x1d,(int1)0x1d,(int1)0x1d}}
    #define  BCX_BLOCK_LNA_RANGE_4_FALL_V     {{(int1)0x40,(int1)0x40,(int1)0x40},{(int1)0x40,(int1)0x40,(int1)0x40}}
    
#else
    #define  BCX_BLOCK_LNA_RANGE_1_RISE_V     {(int1)0xc3, (int1)0xc3}
    #define  BCX_BLOCK_LNA_RANGE_2_RISE_V     {(int1)0xdc, (int1)0xdc}
    #define  BCX_BLOCK_LNA_RANGE_3_RISE_V     {(int1)0x27, (int1)0x27}
    #define  BCX_BLOCK_LNA_RANGE_4_RISE_V     {(int1)0x4a, (int1)0x4a}
    
    #define  BCX_BLOCK_LNA_RANGE_1_FALL_V     {(int1)0xbe, (int1)0xbe}
    #define  BCX_BLOCK_LNA_RANGE_2_FALL_V     {(int1)0xd7, (int1)0xd7}
    #define  BCX_BLOCK_LNA_RANGE_3_FALL_V     {(int1)0x1d, (int1)0x1d}
    #define  BCX_BLOCK_LNA_RANGE_4_FALL_V     {(int1)0x40, (int1)0x40}
#endif

#define  BCX_BLOCK_LNA_RANGE_1_OFFSET_V   {0x065, 0x065}
#define  BCX_BLOCK_LNA_RANGE_2_OFFSET_V   {0x0dc, 0x0dc}
#define  BCX_BLOCK_LNA_RANGE_3_OFFSET_V   {0x168, 0x168}
#define  BCX_BLOCK_LNA_RANGE_4_OFFSET_V   {0x208, 0x208}

/* Intermod Level values
*/
#define  BCX_BLOCK_IM_LEVEL1_V {(int1)0x7f,(int1)0x7f}
#define  BCX_BLOCK_IM_LEVEL2_V {(int1)0x7f,(int1)0x7f}
#define  BCX_BLOCK_IM_LEVEL3_V {(int1)0x22,(int1)0x22}
#define  BCX_BLOCK_IM_LEVEL4_V {(int1)0x45,(int1)0x45}

/* LNA non-bypass hold timer - multiples of 52us
*/
#define  BCX_BLOCK_NONBYPASS_TIMER_V  1

/* LNA bypass hold timer - multiples of 6.6ms
*/
#define  BCX_BLOCK_BYPASS_TIMER_V  1
 
/* CHIPX8 delay for SYNC80M, biased +25.
 This is the RF path delay from the antenna to the IQ
 digital MSM pins */
#define  BCX_BLOCK_ENC_BTF_DLY_V  3305

/* Volts*10 for upper and lower Vbatt calibration.
*/
#define  BCX_BLOCK_VBATT_CAL_VOLTS_V {32,42}

#define RF_BCX_BLOCK_TX_LIM_OFFSET   768

/* Fall setting for HDR IM */
#define RF_BCX_BLOCK_HDR_IM_FALL  ((int1)0xbf)

/* Rise settings for HDR IM */
#define RF_BCX_BLOCK_HDR_IM_RISE  ((int1)0xbf)

/* HDR im Level */
#define RF_BCX_BLOCK_HDR_IM_LEVEL ((int1)0xbf)
 
/* PA R1 Rise, 11 dBm */
#define RF_BCX_BLOCK_PA_R1_RISE 0xBD

/* PA R1 Fall, 16 dBm */
#define RF_BCX_BLOCK_PA_R1_FALL 0xB1

/* Tx limit versus temperature */
#define  RF_BCX_BLOCK_TX_LIM_VS_TEMP_V  {873,873,873,873,873,873,873,873}

/*
Relative Antenna Quality
*/
#define RF_BCX_BLOCK_ANT_QUALITY_TBL {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},}
    
/*===========================================================================

FUNCTION RFNV_CDMA1X_BCX_BLOCK_0_COPY_NV_ITEM              EXTERNALIZED FUNCTION

DESCRIPTION
  This function copies one RF data item, as specified by "item_code" into a
  local data structure.  This function must be called during initialization
  for each and every RF NV item.

DEPENDENCIES
  Note that there must NOT be a dependency on the read order of the NV
  calibration items.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_cdma1x_bcx_block_0_copy_nv_items
(
  nv_items_enum_type  item_code,    /* Item to put */
  nv_item_type        *item_ptr     /* Pointer where to get the item */
);

/*===========================================================================

FUNCTION RFNV_CDMA1X_BCX_BLOCK_0_LIST_SIZE                                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_bcx_block_0_list_size( void );

/*===========================================================================

FUNCTION RFNV_CDMA1X_BCX_BLOCK_0_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns a void pointer the bcx_block nv table.

DEPENDENCIES
  None

RETURN VALUE
  void pointer the bcx_block nv table.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_bcx_block_0_item_table(void);


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*===========================================================================
FUNCTION  RFNV_CDMA1X_BCX_BLOCK_GET_RX_RFCONFIG_MIGRATION_TRACKER  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns pointer to the RFNV->RFConfigDBase migration tracker
  object for the specified Rx path.

DEPENDENCIES
  None

RETURN VALUE
  RFNV->RFConfigDBase migration tracker pointer for a given Rx path

SIDE EFFECTS
  None
===========================================================================*/
extern const rfnv_rfconfig_migration_tracker_type* rfnv_cdma1x_bcx_block_0_get_rx_rfconfig_migration_tracker(
   rf_path_enum_type which_rx_chain);
#endif /* RF_HAS_RFCONFIG_RX_LNA */
#endif /* RFNV_CDMA1X_BCX_BLOCK_0_H */
