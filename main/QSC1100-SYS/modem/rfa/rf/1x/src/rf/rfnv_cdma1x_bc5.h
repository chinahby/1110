#ifndef RFNV_CDMA1X_BC5_H
#define RFNV_CDMA1X_BC5_H


/*===========================================================================

                         R F  P C S  N V  M a n a g e r

DESCRIPTION
  This header file contains prototypes & definitions necessary to retrieve
  RF BC5 specific Non-Volatile memory items, and store them in the local
  tables.

Copyright (c) 2002 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_cdma1x_bc5.h#1 $ $DateTime: 2008/04/22 11:08:31 $ $Author: vmahajan $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/07   Vish    Added rfnv_cdma1x_bc5_get_rx_rfconfig_migration_tracker()
                   API for aiding migrating over of LNA switch point rise,
                   fall and IM level NV items to RFConfigDBase.
06/08/05   sar     Fixed Lint warnings and errors.
11/15/04   sar     Deleted HDR OLTA constant. Will be set by rfnv_set_defaults(). 
11/11/04   sar     eliminating olta constants since these defaults are set by 
                   rfnv_set_defaults().
09/18/04   dbc     Updated TX LIM VS TEMP default value
09/16/04   dbc     Updated default values
09/14/04   sar     Updated default values.
09/03/04   sar     Eliminated OLTA default values differentiation based on RF_HAS_RFR6125.
07/02/04   sar     Updated the value of LNA_RANGE_3_RISE_V.
06/07/04   sar     Updated default value for ENC_BTF_DLY.
05/10/04   sar     Added default for for HDR IM LEVEL.
04/26/04   SAR     Added default values for HDR IM FALL and  HDR IM RISE.
04/01/04   SAR     Added support for Freq Comp Interpolation.
02/23/04   SAR     Initial Revision.

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
    #define  BC5_LNA_RANGE_1_RISE_V     {{(int1)0xc3,(int1)0xc3,(int1)0xc3},{(int1)0xc3,(int1)0xc3,(int1)0xc3}}
    #define  BC5_LNA_RANGE_2_RISE_V     {{(int1)0xdc,(int1)0xdc,(int1)0xdc},{(int1)0xdc,(int1)0xdc,(int1)0xdc}}
    #define  BC5_LNA_RANGE_3_RISE_V     {{(int1)0x27,(int1)0x27,(int1)0x27},{(int1)0x27,(int1)0x27,(int1)0x27}}
    #define  BC5_LNA_RANGE_4_RISE_V     {{(int1)0x4a,(int1)0x4a,(int1)0x4a},{(int1)0x4a,(int1)0x4a,(int1)0x4a}}
    
    #define  BC5_LNA_RANGE_1_FALL_V     {{(int1)0xbe,(int1)0xbe,(int1)0xbe},{(int1)0xbe,(int1)0xbe,(int1)0xbe}}
    #define  BC5_LNA_RANGE_2_FALL_V     {{(int1)0xd7,(int1)0xd7,(int1)0xd7},{(int1)0xd7,(int1)0xd7,(int1)0xd7}}
    #define  BC5_LNA_RANGE_3_FALL_V     {{(int1)0x1d,(int1)0x1d,(int1)0x1d},{(int1)0x1d,(int1)0x1d,(int1)0x1d}}
    #define  BC5_LNA_RANGE_4_FALL_V     {{(int1)0x40,(int1)0x40,(int1)0x40},{(int1)0x40,(int1)0x40,(int1)0x40}}
    
#else
    #define  BC5_LNA_RANGE_1_RISE_V     {(int1)0xc3, (int1)0xc3}
    #define  BC5_LNA_RANGE_2_RISE_V     {(int1)0xdc, (int1)0xdc}
    #define  BC5_LNA_RANGE_3_RISE_V     {(int1)0x27, (int1)0x27}
    #define  BC5_LNA_RANGE_4_RISE_V     {(int1)0x4a, (int1)0x4a}
    
    #define  BC5_LNA_RANGE_1_FALL_V     {(int1)0xbe, (int1)0xbe}
    #define  BC5_LNA_RANGE_2_FALL_V     {(int1)0xd7, (int1)0xd7}
    #define  BC5_LNA_RANGE_3_FALL_V     {(int1)0x1d, (int1)0x1d}
    #define  BC5_LNA_RANGE_4_FALL_V     {(int1)0x40, (int1)0x40}
#endif

#define  BC5_LNA_RANGE_1_OFFSET_V   {0x065, 0x065}
#define  BC5_LNA_RANGE_2_OFFSET_V   {0x0dc, 0x0dc}
#define  BC5_LNA_RANGE_3_OFFSET_V   {0x168, 0x168}
#define  BC5_LNA_RANGE_4_OFFSET_V   {0x208, 0x208}

/* Intermod Level values
*/
#define  BC5_IM_LEVEL1_V {(int1)0x7f,(int1)0x7f}
#define  BC5_IM_LEVEL2_V {(int1)0x7f,(int1)0x7f}
#define  BC5_IM_LEVEL3_V {(int1)0x22,(int1)0x22}
#define  BC5_IM_LEVEL4_V {(int1)0x45,(int1)0x45}

/* LNA non-bypass hold timer - multiples of 52us
*/
#define  BC5_NONBYPASS_TIMER_V  1

/* LNA bypass hold timer - multiples of 6.6ms
*/
#define  BC5_BYPASS_TIMER_V  1
 
/* CHIPX8 delay for SYNC80M, biased +25.
 This is the RF path delay from the antenna to the IQ
 digital MSM pins */
#define  BC5_ENC_BTF_DLY_V  3305

/* Volts*10 for upper and lower Vbatt calibration.
*/
#define  BC5_VBATT_CAL_VOLTS_V {32,42}

#define RF_BC5_TX_LIM_OFFSET   768

/* Fall setting for HDR IM */
#define RF_BC5_HDR_IM_FALL  ((int1)0xbf)

/* Rise settings for HDR IM */
#define RF_BC5_HDR_IM_RISE  ((int1)0xbf)

/* HDR im Level */
#define RF_BC5_HDR_IM_LEVEL ((int1)0xbf)
 
/* PA R1 Rise, 11 dBm */
#define RF_BC5_PA_R1_RISE 0xBD

/* PA R1 Fall, 16 dBm */
#define RF_BC5_PA_R1_FALL 0xB1

/* Tx limit versus temperature */
#define  RF_BC5_TX_LIM_VS_TEMP_V  {873,873,873,873,873,873,873,873}

/*
Relative Antenna Quality
*/
#define RF_BC5_ANT_QUALITY_TBL {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, \
                                {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},}
    
/*===========================================================================

FUNCTION RFNV_CDMA1X_BC5_COPY_NV_ITEM              EXTERNALIZED FUNCTION

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
void rfnv_cdma1x_bc5_copy_nv_items
(
  nv_items_enum_type  item_code,    /* Item to put */
  nv_item_type        *item_ptr     /* Pointer where to get the item */
);

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC5_LIST_SIZE                                 EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns the number of items in the RF NV list.

DEPENDENCIES
  None

RETURN VALUE
  Number of item in the NV list.

SIDE EFFECTS
  None

===========================================================================*/
word rfnv_cdma1x_bc5_list_size( void );

/*===========================================================================

FUNCTION RFNV_CDMA1X_BC5_ITEM_TABLE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This function returns a void pointer the bc5 nv table.

DEPENDENCIES
  None

RETURN VALUE
  void pointer the bc5 nv table.

SIDE EFFECTS
  None

===========================================================================*/
void *rfnv_cdma1x_bc5_item_table(void);


#ifdef RF_HAS_RFCONFIG_RX_LNA
/*===========================================================================
FUNCTION  RFNV_CDMA1X_BC5_GET_RX_RFCONFIG_MIGRATION_TRACKER  EXTERNALIZED FUNCTION

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
extern const rfnv_rfconfig_migration_tracker_type* rfnv_cdma1x_bc5_get_rx_rfconfig_migration_tracker(
   rf_path_enum_type which_rx_chain);
#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif /* RFNV_CDMA1X_BC5_H */
