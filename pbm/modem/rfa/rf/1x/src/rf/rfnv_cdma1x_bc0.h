#ifndef RFNV_CDMA1X_BC0_H
#define RFNV_CDMA1X_BC0_H

/*===========================================================================

                         R F  C D M A  N V   M a n a g e r

DESCRIPTION
  This header file contains prototypes & definitions necessary to retrieve
  RF BC0 specific Non-Volatile memory items, and store them in the local
  tables.

Copyright (c) 2002 by QUALCOMM, Inc.  All Rights Reserved.
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x_bc0.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/20/07   Vish    Added rfnv_cdma1x_bc0_get_rx_rfconfig_migration_tracker()
                   API for aiding migrating over of LNA switch point rise,
                   fall and IM level NV items to RFConfigDBase.
01/25/07   jfc     Include high level rficap.h instead of rficap_mm_cdma1x.h
10/31/06   ycl     Include the correct rficap file.
06/08/05   sar     Fixed Lint warnings and errors.
06/03/05   dyc     Added RF_BC0_HDR_G0_IM_* default values.
05/12/05   sar     Added support for NV_BC0_ANT_QUALITY_I.
11/15/04   sar     Deleted HDR OLTA constant. Will be set by rfnv_set_defaults(). 
11/11/04   sar     eliminating olta constants since these defaults are set by 
                   rfnv_set_defaults().
11/01/04   sar     Changed Tx Lim Vs. Temp default values.
09/30/04   sar     Removed function rfnv_cdma1x_bc0_set_cell_only_defaults() and 
                   eliminated RF Card specific defaults.
09/14/04   sar     Added default for Tx limit versus temperature.
09/02/04   sar     Added function prototype for rfnv_cdma1x_bc0_set_cell_only_defaults()
                   and default values for RFR6125.
07/05/04   dyc     Enable 2nd chain OLTA for all sw points.
06/07/04   sar     Updated default value for ENC_BTF_DLY.
05/10/04   sar     Added default value for HDR IM Level.
04/26/04   SAR     Added default values for HDR IM FALL and  HDR IM RISE.
03/19/04   SAR     Added definition RF_BC0_TX_LIM_OFFSET.
01/28/04   SAR     Updated default values for lna rise/fall and im level 1/4
                   due to structure split into chain specific items.
12/18/03   SAR     Added constant definition to used in the initialization
                   of rfnv_cdma1x_bc0_tbl.
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
    #define  BC0_LNA_RANGE_1_RISE_V     {{-128,-128,-128},{-128,-128,-128}}
    #define  BC0_LNA_RANGE_2_RISE_V     {{-58,-58,-58},{-58,-58,-58}}
    #define  BC0_LNA_RANGE_3_RISE_V     {{29,29,29},{29,29,29}}
    #define  BC0_LNA_RANGE_4_RISE_V     {{75,75,75},{75,75,75}}
    
    #define  BC0_LNA_RANGE_1_FALL_V     {{-128,-128,-128},{-128,-128,-128}}
    #define  BC0_LNA_RANGE_2_FALL_V     {{-61,-61,-61},{-61,-61,-61}}
    #define  BC0_LNA_RANGE_3_FALL_V     {{19,19,19},{19,19,19}}
    #define  BC0_LNA_RANGE_4_FALL_V     {{65,65,65},{65,65,65}}
#else
    #define  BC0_LNA_RANGE_1_RISE_V     {-128,-128}
    #define  BC0_LNA_RANGE_2_RISE_V     { -58, -58}
    #define  BC0_LNA_RANGE_3_RISE_V     {  29,  29}
    #define  BC0_LNA_RANGE_4_RISE_V     {  75,  75}
    
    #define  BC0_LNA_RANGE_1_FALL_V     {-128,-128}
    #define  BC0_LNA_RANGE_2_FALL_V     { -61, -61}
    #define  BC0_LNA_RANGE_3_FALL_V     {  19,  19}
    #define  BC0_LNA_RANGE_4_FALL_V     {  65,  65}
#endif    
    
    #define  BC0_LNA_RANGE_1_OFFSET_V   {0,0}
    #define  BC0_LNA_RANGE_2_OFFSET_V   {0xB4,0xB4}
    #define  BC0_LNA_RANGE_3_OFFSET_V   {0x168,0x168}
    #define  BC0_LNA_RANGE_4_OFFSET_V   {0x208,0x208}
    
    /* Intermod Level values
    */
    #define  BC0_IM_LEVEL1_V {(int1) 0x80,(int1) 0x80}
    #define  BC0_IM_LEVEL2_V {(int1) 0x7F,(int1) 0x7F}
    #define  BC0_IM_LEVEL3_V {(int1) 0x18,(int1) 0x18}
    #define  BC0_IM_LEVEL4_V {(int1) 0x46,(int1) 0x46}

/* LNA non-bypass hold timer - multiples of 52us
*/
#define  BC0_NONBYPASS_TIMER_V  1

/* LNA bypass hold timer - multiples of 6.6ms
*/
#define  BC0_BYPASS_TIMER_V  1

/* CHIPX8 delay for SYNC80M, biased +25.
 This is the RF path delay from the antenna to the IQ
 digital MSM pins */
#define  BC0_ENC_BTF_DLY_V  3301

/* Volts*10 for upper and lower Vbatt calibration.
*/
#define  BC0_VBATT_CAL_VOLTS_V {32,42}

#define RF_BC0_TX_LIM_OFFSET  768   /* rfnv_cdma_tx_lim_vs_temp[] adder */

/* Fall setting for HDR IM (rfr6000) */
#define RF_BC0_HDR_IM_FALL_V  ((int1)0xbf)

/* Rise settings for HDR IM (rfr6000) */
#define RF_BC0_HDR_IM_RISE_V  ((int1)0xbf)

/* HDR setting for HDR IM Level (rfr6000) */
#define RF_BC0_HDR_IM_LEVEL_V ((int1)0xbf)

/* Fall setting for HDR G0 IM (rfr6500) */
#define RF_BC0_HDR_G0_IM_FALL_V  ((int1)0xba)

/* Rise settings for HDR G0 IM (rfr6500) */
#define RF_BC0_HDR_G0_IM_RISE_V  ((int1)0xbd)

/* HDR setting for HDR G0 IM Level (rfr6500) */
#define RF_BC0_HDR_G0_IM_LEVEL_V ((int1)0x7f)

/* Tx limit versus temperature */
#define  RF_BC0_TX_LIM_VS_TEMP_V  {843,843,843,843,843,843,843,843}

/*
Relative Antenna Quality
*/
#define RF_BC0_ANT_QUALITY_TBL {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},}
    
/*===========================================================================

FUNCTION RFNV_CDMA1X_BC0_COPY_NV_ITEM              EXTERNALIZED FUNCTION

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
void rfnv_cdma1x_bc0_copy_nv_items
(
  nv_items_enum_type  item_code,    /* Item to put */
  nv_item_type        *item_ptr     /* Pointer where to get the item */
);


/*===========================================================================

FUNCTION RFNV_CDMA1X_BC0_LIST_SIZE                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF BC0 NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the RF BC0 NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_bc0_list_size( void );

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC0_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns a void pointer to the BC0 nv table.

DEPENDENCIES
  None

RETURN VALUE
  Void pointer to the BC0 nv table.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_bc0_item_table(void);


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*===========================================================================
FUNCTION  RFNV_CDMA1X_BC0_GET_RX_RFCONFIG_MIGRATION_TRACKER  EXTERNALIZED FUNCTION

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
extern const rfnv_rfconfig_migration_tracker_type* rfnv_cdma1x_bc0_get_rx_rfconfig_migration_tracker(
   rf_path_enum_type which_rx_chain);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFNV_CDMA1X_BC0_H */
