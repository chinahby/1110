#ifndef FTM_1X_NV_H
#define FTM_1X_NV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F T M   1 X   R F   N V   S E R V I C E S
                     
GENERAL DESCRIPTION
  Handles internal storage of CDMA 1x RF NV calibration parameters and
  automatic storage of those items to NV.
  
  
EXTERNALIZED FUNCTIONS
  ftm_nv_write()


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2004 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_nv.h#2 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/08   vb/vm   Added Support for Band Class Sub Block Implementation
12/03/04   bmg     Created

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "nv.h"
#include "rflib.h"

typedef enum {
  FTM_BC0_BAND_NV_TABLE = 0,                  /*    US Cellular (800MHz )*/
  FTM_BC1_BAND_NV_TABLE = 1,                  /*    US PCS (1900MHz)     */
  FTM_BC2_BAND_NV_TABLE = 2,                  /*    JTACS                */
  FTM_BC3_BAND_NV_TABLE = 3,                  /*    JCDMA                */
  FTM_BC4_BAND_NV_TABLE = 4,                  /*    Korean PCS           */
  FTM_BC5_BAND_NV_TABLE = 5,                  /*    NMT (450MHz)         */
  FTM_BC6_BAND_NV_TABLE = 6,                  /*    IMT (2.1GHz)         */
  FTM_BC7_BAND_NV_TABLE = 7,
  FTM_BC8_BAND_NV_TABLE = 8,
  FTM_BC9_BAND_NV_TABLE = 9,
  FTM_BC10_BAND_NV_TABLE = 10,
  FTM_BC11_BAND_NV_TABLE = 11,
  FTM_BC12_BAND_NV_TABLE = 12,
  FTM_BC13_BAND_NV_TABLE = 13,
  FTM_BC14_BAND_NV_TABLE = 14,
  FTM_BC15_BAND_NV_TABLE = 15,

  FTM_MAX_BAND_NV_TABLE
  
#ifdef RF_HAS_SUB_BLOCK_SUPPORT
  ,FTM_BCX_BLOCK_0_NV_TABLE = 128
  ,FTM_BCX_BLOCK_1_NV_TABLE = 129
#endif
  
  
} ftm_1x_nv_table_id_type;


/*===========================================================================

FUNCTION FTM_NV_WRITE

DESCRIPTION
  Write a single NV item in the FTM task context
  
DEPENDENCIES
  None

RETURN VALUE
  nv_stat_enum_type - nv write status is returned.

SIDE EFFECTS
  The specified NV item is updated if sucessful.
  
===========================================================================*/
nv_stat_enum_type ftm_nv_write(nv_items_enum_type nv_item, void *data_ptr);


/*===========================================================================

FUNCTION FTM_1X_ADD_CAL_CHAN

DESCRIPTION
  Add a calibration channel to the internal RF NV data structures.
  If the requested channel is already in the calibration channel list, the
  correct index is returned.  If it is not present, a new entry is created
  and the location of the new entry is returned.
  
DEPENDENCIES
  None

RETURN VALUE
  Index into the rfnv_digital_items_type.rfnv_rx_cal_chan array for the
  requested channel.

SIDE EFFECTS
  RF calibration items may be re-arranged or lost, depending on the
  requested channel.
  
===========================================================================*/
int ftm_1x_add_cal_chan(rf_path_enum_type path, word channel);


/*===========================================================================

FUNCTION FTM_1X_WRITE_BAND_NV

DESCRIPTION
  Write all RF calibration items for a particular band to NV.
  
DEPENDENCIES
  None

RETURN VALUE
  nv_stat_enum_type - nv write status is returned.
  If all items are written sucessfully, NV_DONE_S is returned, otherwise
  the return code of the first write error is returned.

SIDE EFFECTS
  RF calibration NV items are modified.
  
===========================================================================*/
nv_stat_enum_type ftm_1x_write_band_nv(unsigned int band);


#endif /* FTM_1X_NV_H */

