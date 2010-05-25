#ifndef RFNV_CDMA1X_BC1_H
#define RFNV_CDMA1X_BC1_H


/*===========================================================================

                         R F  P C S  N V  M a n a g e r

DESCRIPTION
  This header file contains prototypes & definitions necessary to retrieve
  RF BC1 specific Non-Volatile memory items, and store them in the local
  tables.

Copyright (c) 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x_bc1.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/05/07   Vish    Added rfnv_cdma1x_bc1_get_rx_rfconfig_migration_tracker()
                   API for aiding migrating over of LNA switch point rise,
                   fall and IM level NV items to RFConfigDBase.
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
11/15/04   sar     Deleted HDR OLTA constant. Will be set by rfnv_set_defaults(). 
11/11/04   sar     eliminating olta constants since these defaults are set by 
                   rfnv_set_defaults().
11/01/04   sar     Changed Tx Lim Vs. Temp default values.
09/30/04   sar     Removed function rfnv_cdma1x_bc1_set_kpcs_defaults().
09/16/04   jac     Added routine rfnv_cdma1x_bc1_set_kpcs_defaults() to change
                   the statically initialized structure for non-FEBC builds 
                   for KPCS band.
09/14/04   sar     Added default for Tx limit versus temperature.
09/03/04   sar     Eliminated OLTA default values differentiation based on RF_HAS_RFR6125.
07/02/04   sar     Updated the value of LNA_RANGE_3_RISE_V.
06/07/04   sar     Updated default value for ENC_BTF_DLY.
05/10/04   sar     Added default for for HDR IM LEVEL.
04/26/04   SAR     Added default values for HDR IM FALL and  HDR IM RISE.
03/19/04   SAR     Added definition RF_BC1_TX_LIM_OFFSET.
01/28/04   SAR     Updated default values for lna rise/fall and im level 1/4
                   due to structure split into chain specific items.
12/18/03   SAR     Added constant definition to used in the initialization
                   of rfnv_cdma1x_bc1_tbl.
07/29/03   SAR     Initial Revision.

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
    #define  BC1_LNA_RANGE_1_RISE_V     {{-128,-128,-128},{-128,-128,-128}}
    #define  BC1_LNA_RANGE_2_RISE_V     {{-11,-11,-11},{-11,-11,-11}}
    #define  BC1_LNA_RANGE_3_RISE_V     {{52,52,52},{52,52,52}}
    #define  BC1_LNA_RANGE_4_RISE_V     {{0x4F,0x4F,0x4F},{0x4F,0x4F,0x4F}}
    
    #define  BC1_LNA_RANGE_1_FALL_V     {{-128,-128,-128},{-128,-128,-128}}
    #define  BC1_LNA_RANGE_2_FALL_V     {{-21,-21,-21},{-21,-21,-21}}
    #define  BC1_LNA_RANGE_3_FALL_V     {{42,42,42},{42,42,42}}
    #define  BC1_LNA_RANGE_4_FALL_V     {{69,69,69},{69,69,69}}
    
#else
    #define  BC1_LNA_RANGE_1_RISE_V    { -128, -128} 
    #define  BC1_LNA_RANGE_2_RISE_V    {  -11,  -11}  
    #define  BC1_LNA_RANGE_3_RISE_V    {   52,   52}  
    #define  BC1_LNA_RANGE_4_RISE_V    { 0x4F, 0x4F}

    #define  BC1_LNA_RANGE_1_FALL_V    { -128, -128}
    #define  BC1_LNA_RANGE_2_FALL_V    {  -21,  -21}
    #define  BC1_LNA_RANGE_3_FALL_V    {   42,   42}
    #define  BC1_LNA_RANGE_4_FALL_V    {   69,   69}
#endif    

#define  BC1_LNA_RANGE_1_OFFSET_V  {    0,    0}
#define  BC1_LNA_RANGE_2_OFFSET_V  { 0xFA, 0xFA}
#define  BC1_LNA_RANGE_3_OFFSET_V  {0x168,0x168}
#define  BC1_LNA_RANGE_4_OFFSET_V  {0x208,0x208}

/* Intermod Level values
*/
#define  BC1_IM_LEVEL1_V {(int1) 0x80,(int1) 0x80 }
#define  BC1_IM_LEVEL2_V {(int1) 0xF0,(int1) 0xF0 }
#define  BC1_IM_LEVEL3_V {(int1) 0x2F,(int1) 0x2F }
#define  BC1_IM_LEVEL4_V {(int1) 0x4A,(int1) 0x4A }

/* LNA non-bypass hold timer - multiples of 52us
*/
#define  BC1_NONBYPASS_TIMER_V  0

/* LNA bypass hold timer - multiples of 6.6ms
*/
#define  BC1_BYPASS_TIMER_V  0

 
/* CHIPX8 delay for SYNC80M, biased +25.
 This is the RF path delay from the antenna to the IQ
 digital MSM pins */
#define  BC1_ENC_BTF_DLY_V  3301

// Volts*10 for upper and lower Vbatt calibration.
#define  BC1_VBATT_CAL_VOLTS_V {31,43}

#define RF_BC1_TX_LIM_OFFSET   768

/* Fall setting for HDR IM */
#define RF_BC1_HDR_IM_FALL  ((int1)0xbf)

/* Rise settings for HDR IM */
#define RF_BC1_HDR_IM_RISE  ((int1)0xbf)

/* HDR im Level */
#define RF_BC1_HDR_IM_LEVEL ((int1)0xbf) 
 
/* Tx limit versus temperature */
#define  RF_BC1_TX_LIM_VS_TEMP_V  {873,873,873,873,873,873,873,873}

/*
Relative Antenna Quality
*/
#define RF_BC1_ANT_QUALITY_TBL {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},}
    
/*===========================================================================

FUNCTION RFNV_CDMA1X_BC1_COPY_NV_ITEM              EXTERNALIZED FUNCTION

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
void rfnv_cdma1x_bc1_copy_nv_items
(
  nv_items_enum_type  item_code,    /* Item to put */
  nv_item_type        *item_ptr     /* Pointer where to get the item */
);

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC1_LIST_SIZE                                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF BC1 NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_bc1_list_size( void );

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC1_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns a void pointer the bc1 nv table.

DEPENDENCIES
  None

RETURN VALUE
  void pointer the pcs nv table.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_bc1_item_table(void);


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*===========================================================================
FUNCTION  RFNV_CDMA1X_BC1_GET_RX_RFCONFIG_MIGRATION_TRACKER  EXTERNALIZED FUNCTION

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
extern const rfnv_rfconfig_migration_tracker_type* rfnv_cdma1x_bc1_get_rx_rfconfig_migration_tracker(
   rf_path_enum_type which_rx_chain);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFNV_CDMA1X_BC1_H */


