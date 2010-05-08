#ifndef RFNV_CDMA1X_BC10_H
#define RFNV_CDMA1X_BC10_H


/*===========================================================================

                         R F  B C 1 0   N V  M a n a g e r

DESCRIPTION
  This header file contains prototypes & definitions necessary to retrieve
  RF BC10 specific Non-Volatile memory items, and store them in the local
  tables.

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x_bc10.h#1 $ $DateTime: 2008/04/22 11:08:31 $ $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/08   aro	   Initial Version from SC2X branch

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
    #define  BC10_LNA_RANGE_1_RISE_V     {{-128,-128,-128},{-128,-128,-128}}
    #define  BC10_LNA_RANGE_2_RISE_V     {{-11,-11,-11},{-11,-11,-11}}
    #define  BC10_LNA_RANGE_3_RISE_V     {{52,52,52},{52,52,52}}
    #define  BC10_LNA_RANGE_4_RISE_V     {{0x4F,0x4F,0x4F},{0x4F,0x4F,0x4F}}
    
    #define  BC10_LNA_RANGE_1_FALL_V     {{-128,-128,-128},{-128,-128,-128}}
    #define  BC10_LNA_RANGE_2_FALL_V     {{-21,-21,-21},{-21,-21,-21}}
    #define  BC10_LNA_RANGE_3_FALL_V     {{42,42,42},{42,42,42}}
    #define  BC10_LNA_RANGE_4_FALL_V     {{69,69,69},{69,69,69}}
    
#else
    #define  BC10_LNA_RANGE_1_RISE_V    { -128, -128} 
    #define  BC10_LNA_RANGE_2_RISE_V    {  -11,  -11}  
    #define  BC10_LNA_RANGE_3_RISE_V    {   52,   52}  
    #define  BC10_LNA_RANGE_4_RISE_V    { 0x4F, 0x4F}

    #define  BC10_LNA_RANGE_1_FALL_V    { -128, -128}
    #define  BC10_LNA_RANGE_2_FALL_V    {  -21,  -21}
    #define  BC10_LNA_RANGE_3_FALL_V    {   42,   42}
    #define  BC10_LNA_RANGE_4_FALL_V    {   69,   69}
#endif    

#define  BC10_LNA_RANGE_1_OFFSET_V  {    0,    0}
#define  BC10_LNA_RANGE_2_OFFSET_V  { 0xFA, 0xFA}
#define  BC10_LNA_RANGE_3_OFFSET_V  {0x168,0x168}
#define  BC10_LNA_RANGE_4_OFFSET_V  {0x208,0x208}

/* Intermod Level values
*/
#define  BC10_IM_LEVEL1_V {(int1) 0x80,(int1) 0x80 }
#define  BC10_IM_LEVEL2_V {(int1) 0xF0,(int1) 0xF0 }
#define  BC10_IM_LEVEL3_V {(int1) 0x2F,(int1) 0x2F }
#define  BC10_IM_LEVEL4_V {(int1) 0x4A,(int1) 0x4A }

/* LNA non-bypass hold timer - multiples of 52us
*/
#define  BC10_NONBYPASS_TIMER_V  0

/* LNA bypass hold timer - multiples of 6.6ms
*/
#define  BC10_BYPASS_TIMER_V  0

 
/* CHIPX8 delay for SYNC80M, biased +25.
 This is the RF path delay from the antenna to the IQ
 digital MSM pins */
#define  BC10_ENC_BTF_DLY_V  3301

// Volts*10 for upper and lower Vbatt calibration.
#define  BC10_VBATT_CAL_VOLTS_V {31,43}

#define RF_BC10_TX_LIM_OFFSET   768

/* Fall setting for HDR IM */
#define RF_BC10_HDR_IM_FALL  ((int1)0xbf)

/* Rise settings for HDR IM */
#define RF_BC10_HDR_IM_RISE  ((int1)0xbf)

/* HDR im Level */
#define RF_BC10_HDR_IM_LEVEL ((int1)0xbf) 
 
/* Tx limit versus temperature */
#define  RF_BC10_TX_LIM_VS_TEMP_V  {873,873,873,873,873,873,873,873}

/*
Relative Antenna Quality
*/
#define RF_BC10_ANT_QUALITY_TBL {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},}
    
/*===========================================================================

FUNCTION RFNV_CDMA1X_BC10_COPY_NV_ITEM              EXTERNALIZED FUNCTION

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
void rfnv_cdma1x_bc10_copy_nv_items
(
  nv_items_enum_type  item_code,    /* Item to put */
  nv_item_type        *item_ptr     /* Pointer where to get the item */
);

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC10_LIST_SIZE                                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF BC1 NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_bc10_list_size( void );

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC10_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns a void pointer the bc1 nv table.

DEPENDENCIES
  None

RETURN VALUE
  void pointer the pcs nv table.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_bc10_item_table(void);


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*===========================================================================
FUNCTION  RFNV_CDMA1X_BC10_GET_RX_RFCONFIG_MIGRATION_TRACKER  EXTERNALIZED FUNCTION

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
extern const rfnv_rfconfig_migration_tracker_type* rfnv_cdma1x_bc10_get_rx_rfconfig_migration_tracker(
   rf_path_enum_type which_rx_chain);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFNV_CDMA1X_BC10_H */
