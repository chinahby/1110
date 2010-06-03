/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  F T M   1 X   R F   N V   S E R V I C E S
                     
GENERAL DESCRIPTION
  Handles internal storage of CDMA 1x RF NV calibration parameters and
  automatic storage of those items to NV.
  
  
EXTERNALIZED FUNCTIONS
  ftm_nv_write()
  ftm_1x_write_band_nv()
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2004, 2005, 2006 by QUALCOMM, Incorporated. All Rights Reserved.
Copyright(c) 2007             by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/ftm/ftm_1x_nv.c#2 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/08   vm/vb   Added Support for Band Class Sub Block Implementation
11/28/07   ems     Featurized NV tables under the corresponding band class
03/12/07   bmg     RTR6500 updates
03/05/07   vm      Featurize the RF Library Calls until RF library
                   is merged for Common Code 
02/26/07   vm      Merge for Common archive
01/10/06   bmg     QSC60x5 updates
10/02/06   dyc     Updated for compilation of undef RF_HAS_POWER_MODE_LIB
09/27/06   vm      Added the support for DVGA Resolution change to 1/10th 
                   of dB for SC2X
09/26/06   hkk     Added support for BC15, with featurization
03/02/06   zg/sar  Added support for RFR6185.
11/16/05   rmd     Added support for BC11.
10/12/05   rmd     Added support for BC14.
07/08/05   bmg     Fixed a possible problem with DVGA (and LNA) NV offsets
                   running out of bits to accurately store the calibration
                   data.  Modified the base offset computation to pick the
                   midpoint between the current min and max value.
03/22/05   bmg     Implemented InteliCeiver calibration
12/03/04   bmg     Created

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE

#include "ftm_task.h"
#include "ftm_1x_nv.h"
#include "rf.h"
#include "task.h"
#include "rf_intelliceiver.h"


/* -------------------------------------------------------------------------

                            LOCAL DATA TYPES

---------------------------------------------------------------------------- */

typedef struct
{
  nv_items_enum_type RX_cal_chan_list[2];
  nv_items_enum_type RX_cal_chan_LRU_list[2];
  nv_items_enum_type DVGA[2];
  nv_items_enum_type DVGA_vs_freq[2];
  nv_items_enum_type LNA_1[2];
  nv_items_enum_type LNA_1_vs_freq[2];
  nv_items_enum_type LNA_2[2];
  nv_items_enum_type LNA_2_vs_freq[2];
  nv_items_enum_type LNA_3[2];
  nv_items_enum_type LNA_3_vs_freq[2];
  nv_items_enum_type LNA_4[2];
  nv_items_enum_type LNA_4_vs_freq[2];
  nv_items_enum_type IM2_I[2];
  nv_items_enum_type IM2_Q[2];
  nv_items_enum_type IM2_Gm[2];
#ifdef RF_HAS_POWER_MODE_LIB
  nv_items_enum_type IM2_lpm[2];
  nv_items_enum_type IntelliCeiver_Cal;
#endif /* RF_HAS_POWER_MODE_LIB */
} ftm_1x_nv_items_map;


/* -------------------------------------------------------------------------

                          LOCAL DATA

---------------------------------------------------------------------------- */

ftm_1x_nv_items_map ftm_bc0_nv_items_map = {
  {NV_C0_BC0_RX_CAL_CHAN_I,              NV_C1_BC0_RX_CAL_CHAN_I},
  {NV_C0_BC0_RX_CAL_CHAN_LRU_I,          NV_C1_BC0_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC0_VGA_GAIN_OFFSET_I,          NV_C1_BC0_VGA_GAIN_OFFSET_I},
  {NV_C0_BC0_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC0_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC0_LNA_1_OFFSET_I,             NV_C1_BC0_LNA_1_OFFSET_I},
  {NV_C0_BC0_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC0_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC0_LNA_2_OFFSET_I,             NV_C1_BC0_LNA_2_OFFSET_I},
  {NV_C0_BC0_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC0_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC0_LNA_3_OFFSET_I,             NV_C1_BC0_LNA_3_OFFSET_I},
  {NV_C0_BC0_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC0_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC0_LNA_4_OFFSET_I,             NV_C1_BC0_LNA_4_OFFSET_I},
  {NV_C0_BC0_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC0_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC0_IM2_I_VALUE_I,              NV_C1_BC0_IM2_I_VALUE_I},
  {NV_C0_BC0_IM2_Q_VALUE_I,              NV_C1_BC0_IM2_Q_VALUE_I},
  {NV_C0_BC0_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC0_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC0_IM2_LPM_1_I,
   NV_C0_BC0_IM2_LPM_2_I},
  NV_C0_BC0_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc1_nv_items_map = {
  {NV_C0_BC1_RX_CAL_CHAN_I,              NV_C1_BC1_RX_CAL_CHAN_I},
  {NV_C0_BC1_RX_CAL_CHAN_LRU_I,          NV_C1_BC1_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC1_VGA_GAIN_OFFSET_I,          NV_C1_BC1_VGA_GAIN_OFFSET_I},
  {NV_C0_BC1_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC1_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC1_LNA_1_OFFSET_I,             NV_C1_BC1_LNA_1_OFFSET_I},
  {NV_C0_BC1_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC1_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC1_LNA_2_OFFSET_I,             NV_C1_BC1_LNA_2_OFFSET_I},
  {NV_C0_BC1_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC1_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC1_LNA_3_OFFSET_I,             NV_C1_BC1_LNA_3_OFFSET_I},
  {NV_C0_BC1_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC1_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC1_LNA_4_OFFSET_I,             NV_C1_BC1_LNA_4_OFFSET_I},
  {NV_C0_BC1_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC1_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC1_IM2_I_VALUE_I,              NV_C1_BC1_IM2_I_VALUE_I},
  {NV_C0_BC1_IM2_Q_VALUE_I,              NV_C1_BC1_IM2_Q_VALUE_I},
  {NV_C0_BC1_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC1_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC1_IM2_LPM_1_I,
   NV_C0_BC1_IM2_LPM_2_I},
  NV_C0_BC1_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc3_nv_items_map = {
  {NV_C0_BC3_RX_CAL_CHAN_I,              NV_C1_BC3_RX_CAL_CHAN_I},
  {NV_C0_BC3_RX_CAL_CHAN_LRU_I,          NV_C1_BC3_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC3_VGA_GAIN_OFFSET_I,          NV_C1_BC3_VGA_GAIN_OFFSET_I},
  {NV_C0_BC3_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC3_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC3_LNA_1_OFFSET_I,             NV_C1_BC3_LNA_1_OFFSET_I},
  {NV_C0_BC3_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC3_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC3_LNA_2_OFFSET_I,             NV_C1_BC3_LNA_2_OFFSET_I},
  {NV_C0_BC3_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC3_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC3_LNA_3_OFFSET_I,             NV_C1_BC3_LNA_3_OFFSET_I},
  {NV_C0_BC3_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC3_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC3_LNA_4_OFFSET_I,             NV_C1_BC3_LNA_4_OFFSET_I},
  {NV_C0_BC3_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC3_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC3_IM2_I_VALUE_I,              NV_C1_BC3_IM2_I_VALUE_I},
  {NV_C0_BC3_IM2_Q_VALUE_I,              NV_C1_BC3_IM2_Q_VALUE_I},
  {NV_C0_BC3_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC3_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC3_IM2_LPM_1_I,
   NV_C0_BC3_IM2_LPM_2_I},
  NV_C0_BC3_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc4_nv_items_map = {
  {NV_C0_BC4_RX_CAL_CHAN_I,              NV_C1_BC4_RX_CAL_CHAN_I},
  {NV_C0_BC4_RX_CAL_CHAN_LRU_I,          NV_C1_BC4_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC4_VGA_GAIN_OFFSET_I,          NV_C1_BC4_VGA_GAIN_OFFSET_I},
  {NV_C0_BC4_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC4_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC4_LNA_1_OFFSET_I,             NV_C1_BC4_LNA_1_OFFSET_I},
  {NV_C0_BC4_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC4_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC4_LNA_2_OFFSET_I,             NV_C1_BC4_LNA_2_OFFSET_I},
  {NV_C0_BC4_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC4_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC4_LNA_3_OFFSET_I,             NV_C1_BC4_LNA_3_OFFSET_I},
  {NV_C0_BC4_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC4_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC4_LNA_4_OFFSET_I,             NV_C1_BC4_LNA_4_OFFSET_I},
  {NV_C0_BC4_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC4_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC4_IM2_I_VALUE_I,              NV_C1_BC4_IM2_I_VALUE_I},
  {NV_C0_BC4_IM2_Q_VALUE_I,              NV_C1_BC4_IM2_Q_VALUE_I},
  {NV_C0_BC4_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC4_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC4_IM2_LPM_1_I,
   NV_C0_BC4_IM2_LPM_2_I},
  NV_C0_BC4_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc5_nv_items_map = {
  {NV_C0_BC5_RX_CAL_CHAN_I,              NV_C1_BC5_RX_CAL_CHAN_I},
  {NV_C0_BC5_RX_CAL_CHAN_LRU_I,          NV_C1_BC5_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC5_VGA_GAIN_OFFSET_I,          NV_C1_BC5_VGA_GAIN_OFFSET_I},
  {NV_C0_BC5_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC5_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC5_LNA_1_OFFSET_I,             NV_C1_BC5_LNA_1_OFFSET_I},
  {NV_C0_BC5_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC5_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC5_LNA_2_OFFSET_I,             NV_C1_BC5_LNA_2_OFFSET_I},
  {NV_C0_BC5_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC5_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC5_LNA_3_OFFSET_I,             NV_C1_BC5_LNA_3_OFFSET_I},
  {NV_C0_BC5_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC5_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC5_LNA_4_OFFSET_I,             NV_C1_BC5_LNA_4_OFFSET_I},
  {NV_C0_BC5_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC5_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC5_IM2_I_VALUE_I,              NV_C1_BC5_IM2_I_VALUE_I},
  {NV_C0_BC5_IM2_Q_VALUE_I,              NV_C1_BC5_IM2_Q_VALUE_I},
  {NV_C0_BC5_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC5_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC5_IM2_LPM_1_I,
   NV_C0_BC5_IM2_LPM_2_I},
  NV_C0_BC5_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc6_nv_items_map = {
  {NV_C0_BC6_RX_CAL_CHAN_I,              NV_C1_BC6_RX_CAL_CHAN_I},
  {NV_C0_BC6_RX_CAL_CHAN_LRU_I,          NV_C1_BC6_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC6_VGA_GAIN_OFFSET_I,          NV_C1_BC6_VGA_GAIN_OFFSET_I},
  {NV_C0_BC6_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC6_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC6_LNA_1_OFFSET_I,             NV_C1_BC6_LNA_1_OFFSET_I},
  {NV_C0_BC6_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC6_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC6_LNA_2_OFFSET_I,             NV_C1_BC6_LNA_2_OFFSET_I},
  {NV_C0_BC6_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC6_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC6_LNA_3_OFFSET_I,             NV_C1_BC6_LNA_3_OFFSET_I},
  {NV_C0_BC6_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC6_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC6_LNA_4_OFFSET_I,             NV_C1_BC6_LNA_4_OFFSET_I},
  {NV_C0_BC6_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC6_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC6_IM2_I_VALUE_I,              NV_C1_BC6_IM2_I_VALUE_I},
  {NV_C0_BC6_IM2_Q_VALUE_I,              NV_C1_BC6_IM2_Q_VALUE_I},
  {NV_C0_BC6_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC6_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC6_IM2_LPM_1_I,
   NV_C0_BC6_IM2_LPM_2_I},
  NV_C0_BC6_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc11_nv_items_map = {
  {NV_C0_BC11_RX_CAL_CHAN_I,              NV_C1_BC11_RX_CAL_CHAN_I},
  {NV_C0_BC11_RX_CAL_CHAN_LRU_I,          NV_C1_BC11_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC11_VGA_GAIN_OFFSET_I,          NV_C1_BC11_VGA_GAIN_OFFSET_I},
  {NV_C0_BC11_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC11_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC11_LNA_1_OFFSET_I,             NV_C1_BC11_LNA_1_OFFSET_I},
  {NV_C0_BC11_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC11_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC11_LNA_2_OFFSET_I,             NV_C1_BC11_LNA_2_OFFSET_I},
  {NV_C0_BC11_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC11_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC11_LNA_3_OFFSET_I,             NV_C1_BC11_LNA_3_OFFSET_I},
  {NV_C0_BC11_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC11_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC11_LNA_4_OFFSET_I,             NV_C1_BC11_LNA_4_OFFSET_I},
  {NV_C0_BC11_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC11_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC11_IM2_I_VALUE_I,              NV_C1_BC11_IM2_I_VALUE_I},
  {NV_C0_BC11_IM2_Q_VALUE_I,              NV_C1_BC11_IM2_Q_VALUE_I},
  {NV_C0_BC11_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC11_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC11_IM2_LPM_1_I,
   NV_C0_BC11_IM2_LPM_2_I},
   NV_C0_BC11_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

ftm_1x_nv_items_map ftm_bc14_nv_items_map = {
  {NV_C0_BC14_RX_CAL_CHAN_I,              NV_C1_BC14_RX_CAL_CHAN_I},
  {NV_C0_BC14_RX_CAL_CHAN_LRU_I,          NV_C1_BC14_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC14_VGA_GAIN_OFFSET_I,          NV_C1_BC14_VGA_GAIN_OFFSET_I},
  {NV_C0_BC14_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC14_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC14_LNA_1_OFFSET_I,             NV_C1_BC14_LNA_1_OFFSET_I},
  {NV_C0_BC14_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC14_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC14_LNA_2_OFFSET_I,             NV_C1_BC14_LNA_2_OFFSET_I},
  {NV_C0_BC14_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC14_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC14_LNA_3_OFFSET_I,             NV_C1_BC14_LNA_3_OFFSET_I},
  {NV_C0_BC14_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC14_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC14_LNA_4_OFFSET_I,             NV_C1_BC14_LNA_4_OFFSET_I},
  {NV_C0_BC14_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC14_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC14_IM2_I_VALUE_I,              NV_C1_BC14_IM2_I_VALUE_I},
  {NV_C0_BC14_IM2_Q_VALUE_I,              NV_C1_BC14_IM2_Q_VALUE_I},
  {NV_C0_BC14_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC14_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC14_IM2_LPM_1_I,
   NV_C0_BC14_IM2_LPM_2_I},
   NV_C0_BC14_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};

#ifdef FTM_HAS_SUPPORT_FOR_BC15
ftm_1x_nv_items_map ftm_bc15_nv_items_map = {
  {NV_C0_BC15_RX_CAL_CHAN_I,              NV_C1_BC15_RX_CAL_CHAN_I},
  {NV_C0_BC15_RX_CAL_CHAN_LRU_I,          NV_C1_BC15_RX_CAL_CHAN_LRU_I},

  {NV_C0_BC15_VGA_GAIN_OFFSET_I,          NV_C1_BC15_VGA_GAIN_OFFSET_I},
  {NV_C0_BC15_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BC15_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC15_LNA_1_OFFSET_I,             NV_C1_BC15_LNA_1_OFFSET_I},
  {NV_C0_BC15_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BC15_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BC15_LNA_2_OFFSET_I,             NV_C1_BC15_LNA_2_OFFSET_I},
  {NV_C0_BC15_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BC15_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC15_LNA_3_OFFSET_I,             NV_C1_BC15_LNA_3_OFFSET_I},
  {NV_C0_BC15_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BC15_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC15_LNA_4_OFFSET_I,             NV_C1_BC15_LNA_4_OFFSET_I},
  {NV_C0_BC15_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BC15_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BC15_IM2_I_VALUE_I,              NV_C1_BC15_IM2_I_VALUE_I},
  {NV_C0_BC15_IM2_Q_VALUE_I,              NV_C1_BC15_IM2_Q_VALUE_I},
  {NV_C0_BC15_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BC15_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BC15_IM2_LPM_1_I,
   NV_C0_BC15_IM2_LPM_2_I},
   NV_C0_BC15_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_BCX_BLOCK_0_NV
ftm_1x_nv_items_map ftm_bcx_block_0_nv_items_map = {
  {NV_C0_BCX_BLOCK_RX_CAL_CHAN_I,              NV_C1_BCX_BLOCK_RX_CAL_CHAN_I},
  {NV_C0_BCX_BLOCK_RX_CAL_CHAN_LRU_I,          NV_C1_BCX_BLOCK_RX_CAL_CHAN_LRU_I},

  {NV_C0_BCX_BLOCK_VGA_GAIN_OFFSET_I,          NV_C1_BCX_BLOCK_VGA_GAIN_OFFSET_I},
  {NV_C0_BCX_BLOCK_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BCX_BLOCK_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_LNA_1_OFFSET_I,             NV_C1_BCX_BLOCK_LNA_1_OFFSET_I},
  {NV_C0_BCX_BLOCK_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BCX_BLOCK_LNA_2_OFFSET_I,             NV_C1_BCX_BLOCK_LNA_2_OFFSET_I},
  {NV_C0_BCX_BLOCK_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_LNA_3_OFFSET_I,             NV_C1_BCX_BLOCK_LNA_3_OFFSET_I},
  {NV_C0_BCX_BLOCK_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_LNA_4_OFFSET_I,             NV_C1_BCX_BLOCK_LNA_4_OFFSET_I},
  {NV_C0_BCX_BLOCK_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_IM2_I_VALUE_I,              NV_C1_BCX_BLOCK_IM2_I_VALUE_I},
  {NV_C0_BCX_BLOCK_IM2_Q_VALUE_I,              NV_C1_BCX_BLOCK_IM2_Q_VALUE_I},
  {NV_C0_BCX_BLOCK_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BCX_BLOCK_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BCX_BLOCK_IM2_LPM_1_I,
   NV_C0_BCX_BLOCK_IM2_LPM_2_I},
   NV_C0_BCX_BLOCK_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};
#endif

#ifdef RF_HAS_BCX_BLOCK_1_NV
ftm_1x_nv_items_map ftm_bcx_block_1_nv_items_map = {
  {NV_C0_BCX_BLOCK_1_RX_CAL_CHAN_I,              NV_C1_BCX_BLOCK_1_RX_CAL_CHAN_I},
  {NV_C0_BCX_BLOCK_1_RX_CAL_CHAN_LRU_I,          NV_C1_BCX_BLOCK_1_RX_CAL_CHAN_LRU_I},

  {NV_C0_BCX_BLOCK_1_VGA_GAIN_OFFSET_I,          NV_C1_BCX_BLOCK_1_VGA_GAIN_OFFSET_I},
  {NV_C0_BCX_BLOCK_1_VGA_GAIN_OFFSET_VS_FREQ_I,  NV_C1_BCX_BLOCK_1_VGA_GAIN_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_1_LNA_1_OFFSET_I,             NV_C1_BCX_BLOCK_1_LNA_1_OFFSET_I},
  {NV_C0_BCX_BLOCK_1_LNA_1_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_1_LNA_1_OFFSET_VS_FREQ_I},

  {NV_C0_BCX_BLOCK_1_LNA_2_OFFSET_I,             NV_C1_BCX_BLOCK_1_LNA_2_OFFSET_I},
  {NV_C0_BCX_BLOCK_1_LNA_2_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_1_LNA_2_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_1_LNA_3_OFFSET_I,             NV_C1_BCX_BLOCK_1_LNA_3_OFFSET_I},
  {NV_C0_BCX_BLOCK_1_LNA_3_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_1_LNA_3_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_1_LNA_4_OFFSET_I,             NV_C1_BCX_BLOCK_1_LNA_4_OFFSET_I},
  {NV_C0_BCX_BLOCK_1_LNA_4_OFFSET_VS_FREQ_I,     NV_C1_BCX_BLOCK_1_LNA_4_OFFSET_VS_FREQ_I},
  
  {NV_C0_BCX_BLOCK_1_IM2_I_VALUE_I,              NV_C1_BCX_BLOCK_1_IM2_I_VALUE_I},
  {NV_C0_BCX_BLOCK_1_IM2_Q_VALUE_I,              NV_C1_BCX_BLOCK_1_IM2_Q_VALUE_I},
  {NV_C0_BCX_BLOCK_1_IM2_TRANSCONDUCTOR_VALUE_I, NV_C1_BCX_BLOCK_1_IM2_TRANSCONDUCTOR_VALUE_I},

#ifdef RF_HAS_POWER_MODE_LIB
  {NV_C0_BCX_BLOCK_1_IM2_LPM_1_I,
   NV_C0_BCX_BLOCK_1_IM2_LPM_2_I},
   NV_C0_BCX_BLOCK_1_INTELLICEIVER_CAL_I
#endif /* RF_HAS_POWER_MODE_LIB */
};
#endif

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/


/* -------------------------------------------------------------------------

                               EXTERNAL DATA

---------------------------------------------------------------------------- */

/* Provide special direct access to the RF NV tables */
#ifdef RF_HAS_BC0_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc0_tbl;
#endif
#ifdef RF_HAS_BC1_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc1_tbl;
#endif
#ifdef RF_HAS_BC3_NV_TABLE
#error code not present
#endif
#ifdef RF_HAS_BC4_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc4_tbl;
#endif
#ifdef RF_HAS_BC5_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc5_tbl;
#endif
#ifdef RF_HAS_BC6_NV_TABLE
#error code not present
#endif
#ifdef RF_HAS_BC11_NV_TABLE
#error code not present
#endif
#ifdef RF_HAS_BC14_NV_TABLE
extern rfnv_digital_items_type rfnv_cdma1x_bc14_tbl;
#endif

#ifdef FTM_HAS_SUPPORT_FOR_BC15
extern rfnv_digital_items_type rfnv_cdma1x_bc15_tbl;
#endif /*FTM_HAS_SUPPORT_FOR_BC15*/

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

#ifdef RF_HAS_BCX_BLOCK_0_NV
extern rfnv_digital_items_type rfnv_cdma1x_bcx_block_0_tbl;
#endif

#ifdef RF_HAS_BCX_BLOCK_1_NV
extern rfnv_digital_items_type rfnv_cdma1x_bcx_block_1_tbl;
#endif

#endif /*RF_HAS_SUB_BLOCK_SUPPORT*/

extern rf_path_0_config_type rf_path_0;  
extern rf_path_1_config_type rf_path_1;


/* -------------------------------------------------------------------------

                          LOCAL FUNCTION PROTOTYPES

---------------------------------------------------------------------------- */

#ifdef RF_HAS_POWER_MODE_LIB
static void ftm_1x_compute_rx_dvga_freq_nv(int2 rfnv_table[][16], const int channel_count, const int power_mode_count, int2 *base, int1 freq[][16]);
static void ftm_1x_compute_rx_lna_freq_nv(int2 rfnv_table[][16], const int channel_count, const int power_mode_count, int2 *base, int1 freq[][16]);
#else
static void ftm_1x_compute_rx_dvga_freq_nv(int2 rfnv_table[], const int channel_count, const int power_mode_count, int2 *base, int1 freq[]);
static void ftm_1x_compute_rx_lna_freq_nv(int2 rfnv_table[], const int channel_count, const int power_mode_count, int2 *base, int1 freq[]);
#endif /* RF_HAS_POWER_MODE_LIB */
static void ftm_1x_rx_nv_move_next_down( rfnv_digital_items_type* rfnv_ptr, rf_path_enum_type path, int i );
static void ftm_1x_rx_nv_move_prev_up( rfnv_digital_items_type* rfnv_ptr, rf_path_enum_type path, int i );
static void ftm_1x_rx_nv_create_blank( rfnv_digital_items_type* rfnv_ptr, rf_path_enum_type path, int i, word channel );

uint32 rf_interpolate_chan_offset ( rf_path_enum_type path, word chan );


/* -------------------------------------------------------------------------

                         LOCAL FUNCTION DEFINITIONS

---------------------------------------------------------------------------- */

/*===========================================================================

FUNCTION    LIMIT

DESCRIPTION
  Clamp the first argument to the range [min_val, max_val].
   
PARAMETERS
  x, minimum limit, maximum limit.
  
DEPENDENCIES
  None.

RETURN VALUE
  Clamped value.

SIDE EFFECTS
  None.

===========================================================================*/
static int limit( int x, int min_val, int max_val )
{
  if ( x  < min_val )
  {
    return min_val;
  }
  else if ( x > max_val )
  {
    return max_val;
  }
  else
  {
    return x;
  }
}


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
nv_stat_enum_type ftm_nv_write(nv_items_enum_type nv_item, void *data_ptr)
{
  nv_cmd_type ftm_nv_cmd;

  /* Prepare the NV write command buffer. */
  ftm_nv_cmd.item       = nv_item;            /* item to write */
  ftm_nv_cmd.cmd        = NV_WRITE_F;         /* NV operation */
  ftm_nv_cmd.data_ptr   = data_ptr;           /* buffer to store write data  */
  ftm_nv_cmd.tcb_ptr    = rex_self();         /* Notify this task when done */
  ftm_nv_cmd.sigs       = FS_OP_COMPLETE_SIG; /* Signal with this sig when done */
  ftm_nv_cmd.done_q_ptr = NULL;               /* Return cmd to NO Q when done */

  /* 
  * Write to NV by:
  *
  * 1. Clearing the NV signal.
  * 2. Queuing the write command to the NV task.
  * 3. Waiting for the NV task to finish servicing the write command. Ensure that
  *    watchdog is being hit during this synchronous operation. 
  */
  (void) rex_clr_sigs( rex_self(), ftm_nv_cmd.sigs );
  
  nv_cmd( &ftm_nv_cmd );

  /*
    If NV writes hang, the FTM task should indicate this with a task dog
    timeout.  Therefore, do not use ftm_sig_rex_wait, which will incorrectly
    send a report periodically
  */
  (void) rex_wait(FS_OP_COMPLETE_SIG);

  if (ftm_nv_cmd.status != NV_DONE_S)
  {
    FTM_MSG_ERROR("FTM error writing NV: item %d, status %d",
                  ftm_nv_cmd.item, ftm_nv_cmd.status, 0);
  }
  
  return ftm_nv_cmd.status;
} /* ftm_nv_write */


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
int ftm_1x_add_cal_chan(rf_path_enum_type path, word channel)
{
  int insert_index, delete_index;
  int i;
  rfnv_digital_items_type *rfnv_ptr;

  rfnv_ptr = rf_chain_status[path].rf_nv_dig_items_ptr;

  if (path > 1 || channel == 0)
  {
    ERR_FATAL("Bad parameters to ftm_1x_add_cal_chan (path = %d, channel = %d)", path, channel, 0);
  }

  /*
    Find the insertion index, the first channel bin in the list
    where the channel is larger (or equal in the event of a match)
    than the inserting channel
  */
  insert_index = 0;
  while (insert_index < NV_FREQ_TABLE_SIZ &&
         rfnv_ptr->rfnv_rx_cal_chan[path][insert_index] != 0 &&
         rfnv_ptr->rfnv_rx_cal_chan_lru[path][insert_index] != 0 &&
         rf_interpolate_chan_offset(path, channel) > rf_interpolate_chan_offset(path, rfnv_ptr->rfnv_rx_cal_chan[path][insert_index]))
  {
    insert_index++;
  }

  /*
    If the channel at the insertion index matches the requested channel
    there is nothing to do, return the index and do not modify NV structures.
    Special case where insert_index == NV_FREQ_TABLE_SIZ which means we
    searched the entire list and didn't find a larger channel.
  */
  if (insert_index < NV_FREQ_TABLE_SIZ &&
      channel == rfnv_ptr->rfnv_rx_cal_chan[path][insert_index] &&
      rfnv_ptr->rfnv_rx_cal_chan_lru[path][insert_index] != 0)
  {
    /*
      Update the LRU list if the current channel is not already at
      the tail of the list
    */
    if (rfnv_ptr->rfnv_rx_cal_chan_lru[path][insert_index] != 1)
    {
      for (i = 0; i < NV_FREQ_TABLE_SIZ; i++)
      {
        /* Stop if the list ends prematurely */
        if (rfnv_ptr->rfnv_rx_cal_chan_lru[path][i] == 0)
        {
          break;
        }
        
        /* Increment the LRU entries that are lower than the one that moves */
        if (rfnv_ptr->rfnv_rx_cal_chan_lru[path][i] < rfnv_ptr->rfnv_rx_cal_chan_lru[path][insert_index])
        {
          rfnv_ptr->rfnv_rx_cal_chan_lru[path][i]++;
        }
      }
      /* And move the current entry to the tail of the list */
      rfnv_ptr->rfnv_rx_cal_chan_lru[path][insert_index] = 1;
    }
    return insert_index;
  }

  /*
    Past here, a matching channel in the RX cal list was not found.
    A new entry will need to be inserted
  */

  /*
    Find the least recently used calibration channel, or an unused
    one if it exists
  */
  delete_index = 0;
  while (delete_index < NV_FREQ_TABLE_SIZ &&
         rfnv_ptr->rfnv_rx_cal_chan_lru[path][delete_index] != 16 &&
         rfnv_ptr->rfnv_rx_cal_chan_lru[path][delete_index] != 0)
  {
    delete_index++;
  }

  if (delete_index < insert_index)
  {
    /*
      The deletion occurs earler than the insertion:

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  | X3X |  4  |  5  |  6  |  7  |  8  |
      |     |     | XXX |     |     |     |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
       \   Stay  /  Del  \  Down   /^\    Stay       /
                                    | Insert    

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  |  4  |  5  | new |  6  |  7  |  8  |
      |     |     |     |     |     |     |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
                              ^
    */

    for (i = delete_index; i < insert_index - 1; i++)
    {
      ftm_1x_rx_nv_move_next_down(rfnv_ptr, path, i);
    }
    insert_index = insert_index - 1;
    ftm_1x_rx_nv_create_blank(rfnv_ptr, path, insert_index, channel);
  }
  else
  {
    /*
      The deletion occurs later than the insertion:

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  |  3  |  4  |  5  | X6X |  7  |  8  |
      |     |     |     |     |     | XXX |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
       \   Stay  /^\      Up       /  Del  \  Stay   /
                  | Insert

      +-----+-----+-----+-----+-----+-----+-----+-----+
      |  1  |  2  | new |  3  |  4  |  5  |  7  |  8  |
      |     |     |     |     |     |     |     |     |
      +-----+-----+-----+-----+-----+-----+-----+-----+
                  ^
    */

    for (i = delete_index; i > insert_index; i--)
    {
      ftm_1x_rx_nv_move_prev_up(rfnv_ptr, path, i);
    }
    ftm_1x_rx_nv_create_blank(rfnv_ptr, path, insert_index, channel);
  }

  return insert_index;
} /* ftm_1x_add_cal_chan */


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
nv_stat_enum_type ftm_1x_write_band_nv(unsigned int band)
{
  ftm_1x_nv_items_map *nv_item_ptr;
  rfnv_digital_items_type *rfnv_ptr;

  ftm_1x_nv_table_id_type ftm_1x_nv_table_id;
  
  int chain;
  
  int2 base_offset;
#ifdef RF_HAS_POWER_MODE_LIB
  int1 offset_vs_freq[NV_RF_POWER_MODE_SIZ][NV_FREQ_TABLE_SIZ];
  nv_rf_rx_im2_cal im2_cal_buffer;
#else
  int1 offset_vs_freq[NV_FREQ_TABLE_SIZ];
#endif /* RF_HAS_POWER_MODE_LIB */

  nv_stat_enum_type write_status;

  ftm_1x_nv_table_id = (ftm_1x_nv_table_id_type) band;

  switch ( ftm_1x_nv_table_id )
  {
  #ifdef RF_HAS_BC0_NV_TABLE
  case FTM_BC0_BAND_NV_TABLE:
    nv_item_ptr = &ftm_bc0_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bc0_tbl;
    break;
  #endif
  #ifdef RF_HAS_BC1_NV_TABLE  
  case FTM_BC1_BAND_NV_TABLE:
    nv_item_ptr = &ftm_bc1_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bc1_tbl;
    break;
  #endif
  #ifdef RF_HAS_BC3_NV_TABLE
#error code not present
  #endif
  #ifdef RF_HAS_BC4_NV_TABLE
  case FTM_BC4_BAND_NV_TABLE:
    nv_item_ptr = &ftm_bc4_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bc4_tbl;
    break;
  #endif
  #ifdef RF_HAS_BC5_NV_TABLE
  case FTM_BC5_BAND_NV_TABLE:
    nv_item_ptr = &ftm_bc5_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bc5_tbl;
    break;
  #endif
  #ifdef RF_HAS_BC6_NV_TABLE
#error code not present
  #endif
  #ifdef RF_HAS_BC11_NV_TABLE
#error code not present
  #endif
  #ifdef RF_HAS_BC14_NV_TABLE
  case FTM_BC14_BAND_NV_TABLE:
    nv_item_ptr = &ftm_bc14_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bc14_tbl;
    break;
  #endif

#ifdef FTM_HAS_SUPPORT_FOR_BC15
  case FTM_BC15_BAND_NV_TABLE:
    nv_item_ptr = &ftm_bc15_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bc15_tbl;
    break;
#endif /* FTM_HAS_SUPPORT_FOR_BC15 */

#ifdef RF_HAS_SUB_BLOCK_SUPPORT

  #ifdef RF_HAS_BCX_BLOCK_0_NV /* Value of 128 in Band settings for loading Block 0 NV */
  case FTM_BCX_BLOCK_0_NV_TABLE:
    nv_item_ptr = &ftm_bcx_block_0_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bcx_block_0_tbl;
    break;  
  #endif

  #ifdef RF_HAS_BCX_BLOCK_1_NV
  case FTM_BCX_BLOCK_1_NV_TABLE: /* Value of 129 in Band settings for loading Block 1 NV */
    nv_item_ptr = &ftm_bcx_block_1_nv_items_map;
    rfnv_ptr = &rfnv_cdma1x_bcx_block_1_tbl;
    break;  
  #endif

#endif/*RF_HAS_SUB_BLOCK_SUPPORT*/

  default:
    ERR_FATAL("Unsupported band in write band NV (%d)", band, 0, 0);
    /* This won't actually get returned if ERR_FATAL halts the mobile */
    return NV_FAIL_S;
  }

  for (chain = 0; chain < 2; chain++)
  {
    int channel_count;
    int power_mode_count;

    write_status = ftm_nv_write(nv_item_ptr->RX_cal_chan_list[chain] ,
                                rfnv_ptr->rfnv_rx_cal_chan[chain]);
    if (write_status != NV_DONE_S)
    {
      return write_status;
    }
    
    write_status = ftm_nv_write(nv_item_ptr->RX_cal_chan_LRU_list[chain] ,
                                &rfnv_ptr->rfnv_rx_cal_chan_lru[chain]);
    if (write_status != NV_DONE_S)
    {
      return write_status;
    }
    
    /* Compute the number of channels to consider when calculating base/offset */
    channel_count = 0;
    while (channel_count < NV_FREQ_TABLE_SIZ &&
           rfnv_ptr->rfnv_rx_cal_chan[chain][channel_count] != 0)
    {
      channel_count++;
    }

    power_mode_count = rf_intelliceiver_get_lowest_power_mode( chain ) + 1;

    if ( channel_count > 0 )
    {
      ftm_1x_compute_rx_dvga_freq_nv(rfnv_ptr->rfnv_vga_gain_offset_vs_freq[chain],
                                     channel_count, power_mode_count, &base_offset,
                                     offset_vs_freq);
      write_status = ftm_nv_write(nv_item_ptr->DVGA[chain],
                                  &base_offset);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->DVGA_vs_freq[chain],
                                  offset_vs_freq);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      ftm_1x_compute_rx_lna_freq_nv(rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[chain],
                                    channel_count, power_mode_count, &base_offset,
                                    offset_vs_freq);
      write_status = ftm_nv_write(nv_item_ptr->LNA_1[chain],
                                  &base_offset);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->LNA_1_vs_freq[chain],
                                  offset_vs_freq);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      ftm_1x_compute_rx_lna_freq_nv(rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[chain],
                                    channel_count, power_mode_count, &base_offset,
                                    offset_vs_freq);
      write_status = ftm_nv_write(nv_item_ptr->LNA_2[chain],
                                  &base_offset);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->LNA_2_vs_freq[chain],
                                  offset_vs_freq);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      ftm_1x_compute_rx_lna_freq_nv(rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[chain],
                                    channel_count, power_mode_count, &base_offset,
                                    offset_vs_freq);
      write_status = ftm_nv_write(nv_item_ptr->LNA_3[chain],
                                  &base_offset);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->LNA_3_vs_freq[chain],
                                  offset_vs_freq);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      ftm_1x_compute_rx_lna_freq_nv(rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[chain],
                                    channel_count, power_mode_count, &base_offset,
                                    offset_vs_freq);
      write_status = ftm_nv_write(nv_item_ptr->LNA_4[chain],
                                  &base_offset);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->LNA_4_vs_freq[chain],
                                  offset_vs_freq);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->IM2_I[chain],
                                  &rfnv_ptr->rfnv_im2_i_value[chain]);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->IM2_Q[chain],
                                  &rfnv_ptr->rfnv_im2_q_value[chain]);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }

      write_status = ftm_nv_write(nv_item_ptr->IM2_Gm[chain],
                                  &rfnv_ptr->rfnv_im2_transconductor_value[chain]);
      if (write_status != NV_DONE_S)
      {
        return write_status;
      }
    }
  }
#ifdef RF_HAS_POWER_MODE_LIB
  im2_cal_buffer.i = rfnv_ptr->rfnv_im2_i_value[0][RFLIB_LOW_POWER_MODE_1];
  im2_cal_buffer.q = rfnv_ptr->rfnv_im2_q_value[0][RFLIB_LOW_POWER_MODE_1];
  im2_cal_buffer.transconductor = rfnv_ptr->rfnv_im2_transconductor_value[0][RFLIB_LOW_POWER_MODE_1];
  
  write_status = ftm_nv_write(nv_item_ptr->IM2_lpm[0],
                              (void*)&im2_cal_buffer);
  if (write_status != NV_DONE_S)
  {
    return write_status;
  }

  im2_cal_buffer.i = rfnv_ptr->rfnv_im2_i_value[0][RFLIB_LOW_POWER_MODE_2];
  im2_cal_buffer.q = rfnv_ptr->rfnv_im2_q_value[0][RFLIB_LOW_POWER_MODE_2];
  im2_cal_buffer.transconductor = rfnv_ptr->rfnv_im2_transconductor_value[0][RFLIB_LOW_POWER_MODE_2];
  
  write_status = ftm_nv_write(nv_item_ptr->IM2_lpm[1],
                              (void*)&im2_cal_buffer);
  if (write_status != NV_DONE_S)
  {
    return write_status;
  }

  write_status = ftm_nv_write(nv_item_ptr->IntelliCeiver_Cal,
                              rfnv_ptr->rfnv_intelliceiver_cal[0]);
  if (write_status != NV_DONE_S)
  {
    return write_status;
  }
#endif /* RF_HAS_POWER_MODE_LIB */

  /*
    This will only be reached if all writes were successful.
    Return NV_DONE_S;
  */
  return NV_DONE_S;
} /* ftm_1x_write_band_nv */


/*===========================================================================

FUNCTION FTM_1X_COMPUTE_RX_DVGA_FREQ_NV

DESCRIPTION
  Converts full representation RF NV DVGA calibration vs. frequency values
  into NV combatable base + offset vs. frequency data.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The integer pointed to by base is updated with the average of the min and
  max calibration values across the band and power modes.  The integer array
  pointed to by freq is updated with the corresponding calibration value
  minus the base.
  
===========================================================================*/
#ifdef RF_HAS_POWER_MODE_LIB
static void ftm_1x_compute_rx_dvga_freq_nv( int2 rfnv_table[][16],
                                            const int channel_count,
                                            const int power_mode_count,
                                            int2 *base,
                                            int1 freq[][16] )
#else
static void ftm_1x_compute_rx_dvga_freq_nv( int2 rfnv_table[],
                                            const int channel_count,
                                            const int power_mode_count,
                                            int2 *base,
                                            int1 freq[] )
#endif /* RF_HAS_POWER_MODE_LIB */
{
  int i;
  int2 min, max;

#ifdef RF_HAS_POWER_MODE_LIB
  int j;
#endif /* RF_HAS_POWER_MODE_LIB */
  
  int local_base;

  ASSERT(channel_count <= NV_FREQ_TABLE_SIZ);

  local_base = 0;

  min = 32767;
  max = -32768;
#ifdef RF_HAS_POWER_MODE_LIB
  for (j = 0; j < power_mode_count; j++)
  {
    for (i = 0; i < channel_count; i++)
    {
      if ( rfnv_table[j][i] < min )
      {
        min = rfnv_table[j][i];
      }
      if ( rfnv_table[j][i] > max )
      {
        max = rfnv_table[j][i];
      }
    }
  }
#else
  for (i = 0; i < channel_count; i++)
  {
    if ( rfnv_table[i] < min )
    {
      min = rfnv_table[i];
    }
    if ( rfnv_table[i] > max )
    {
      max = rfnv_table[i];
    }
  }
#endif /* RF_HAS_POWER_MODE_LIB */

  local_base = (min + max)/2;
  *base = local_base;

#ifdef RF_HAS_POWER_MODE_LIB
  for (j = 0; j < NV_RF_POWER_MODE_SIZ; j++)
  {
    for (i = 0; i < channel_count; i++)
    {
      /*
        Frequency NV is stored in 1/10th dB.
        Convert 1/120th dB resolution DVGA into 1/10th dB by dividing
        count by 12.  Pre-add 1/2 dB to the intermediate result to
        accomplish rounding.This is to be done only when the DVGA 
        Resolution is 1/120th of db.
      */
      if (rfnv_table[j][i] - local_base > 0)
      {
        #ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
        freq[j][i]
          = limit((rfnv_table[j][i] - local_base + 0.5), -128, 127);
        #else
#error code not present
        #endif
      }
      else
      {
        #ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
        freq[j][i]
          = limit((rfnv_table[j][i] - local_base - 0.5), -128, 127);
        #else
#error code not present
        #endif
      }
    }
    for (i = i; i < NV_FREQ_TABLE_SIZ; i++)
    {
      freq[j][i] = 0;
    }
  }
#else
  for (i = 0; i < channel_count; i++)
  {
    /*
      Frequency NV is stored in 1/10th dB.
      Convert 1/120th dB resolution DVGA into 1/10th dB by dividing
      count by 12.  Pre-add 1/2 to the intermediate result to
      accomplish rounding.This is to be done only when the DVGA 
        Resolution is 1/120th of db.
    */
    if (rfnv_table[i] - local_base > 0)
    {
        #ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
        freq[i]
          = limit((rfnv_table[i] - local_base + 0.5), -128, 127);
        #else
#error code not present
        #endif
    }
    else
    {
        #ifdef RF_HAS_DVGA_1OTH_OF_DB_RESOLUTION   
        freq[i]
          = limit((rfnv_table[i] - local_base - 0.5), -128, 127);
        #else
#error code not present
        #endif
    }
  }
  for (i = i; i < NV_FREQ_TABLE_SIZ; i++)
  {
    freq[i] = 0;
  }
#endif /* RF_HAS_POWER_MODE_LIB */

} /* ftm_compute_rx_dvga_freq_nv */


/*===========================================================================

FUNCTION FTM_1X_COMPUTE_RX_LNA_FREQ_NV

DESCRIPTION
  Converts full representation RF NV LNA calibration vs. frequency values
  into NV combatable base + offset vs. frequency data.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The integer pointed to by base is updated with the average of the high
  power calibration values.  The integer array pointed to by freq is
  updated with the corresponding calibration value minus the base.
  
===========================================================================*/
#ifdef RF_HAS_POWER_MODE_LIB
static void ftm_1x_compute_rx_lna_freq_nv( int2 rfnv_table[][16],
                                           const int channel_count,
                                           const int power_mode_count,
                                           int2 *base,
                                           int1 freq[][16] )
#else
static void ftm_1x_compute_rx_lna_freq_nv( int2 rfnv_table[],
                                           const int channel_count,
                                           const int power_mode_count,
                                           int2 *base,
                                           int1 freq[] )
#endif /* RF_HAS_POWER_MODE_LIB */
{
  int i;
  int2 min, max;

#ifdef RF_HAS_POWER_MODE_LIB
  int j;
#endif /* RF_HAS_POWER_MODE_LIB */
  
  int local_base;

  ASSERT(channel_count <= NV_FREQ_TABLE_SIZ);

  local_base = 0;

  min = 32767;
  max = -32768;
#ifdef RF_HAS_POWER_MODE_LIB
  for (j = 0; j < power_mode_count; j++)
  {
    for (i = 0; i < channel_count; i++)
    {
      if ( rfnv_table[j][i] < min )
      {
        min = rfnv_table[j][i];
      }
      if ( rfnv_table[j][i] > max )
      {
        max = rfnv_table[j][i];
      }
    }
  }
#else
  for (i = 0; i < channel_count; i++)
  {
    if ( rfnv_table[i] < min )
    {
      min = rfnv_table[i];
    }
    if ( rfnv_table[i] > max )
    {
      max = rfnv_table[i];
    }
  }
#endif /* RF_HAS_POWER_MODE_LIB */

  local_base = (min + max)/2;
  *base = local_base;

#ifdef RF_HAS_POWER_MODE_LIB
  for (j = 0; j < NV_RF_POWER_MODE_SIZ; j++)
  {
    for (i = 0; i < channel_count; i++)
    {
      freq[j][i] = limit(rfnv_table[j][i] - local_base, -128, 127);
    }
    for (i = i; i < NV_FREQ_TABLE_SIZ; i++)
    {
      freq[j][i] = 0;
    }
  }
#else
  for (i = 0; i < channel_count; i++)
  {
    freq[i] = limit(rfnv_table[i] - local_base, -128, 127);
  }
  for (i = i; i < NV_FREQ_TABLE_SIZ; i++)
  {
    freq[i] = 0;
  }
#endif /* RF_HAS_POWER_MODE_LIB */

} /* ftm_compute_rx_lna_freq_nv */


/*===========================================================================

FUNCTION FTM_1X_RX_NV_MOVE_NEXT_DOWN

DESCRIPTION
  Copies RF RX NV from channel index i+1 into index i.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  RF RX NV is modified, and may be inconsistent until the caller has
  finished the entire move operation.
  
===========================================================================*/
static void ftm_1x_rx_nv_move_next_down( rfnv_digital_items_type* rfnv_ptr, rf_path_enum_type path, int i )
{
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode;
#endif /* RF_HAS_POWER_MODE_LIB */
  
  ASSERT(i >= 0 && i < NV_FREQ_TABLE_SIZ-1);

  rfnv_ptr->rfnv_rx_cal_chan[path][i] = rfnv_ptr->rfnv_rx_cal_chan[path][i+1];
  rfnv_ptr->rfnv_rx_cal_chan_lru[path][i] = rfnv_ptr->rfnv_rx_cal_chan_lru[path][i+1];
#ifdef RF_HAS_POWER_MODE_LIB
  for ( power_mode = RFLIB_NORMAL_POWER_MODE;
        power_mode < RFLIB_MAX_POWER_MODE;
        power_mode++ )
  {
    rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][i+1];
    rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][i+1];
    rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][i+1];
    rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][i+1];
    rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][power_mode][i+1];
  }
#else
  rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][i+1];
  rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][i+1];
  rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][i+1];
  rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][i+1];
  rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][i+1];
#endif /* RF_HAS_POWER_MODE_LIB */
} /* ftm_1x_rx_nv_move_next_down */


/*===========================================================================

FUNCTION FTM_1X_RX_NV_MOVE_PREV_UP

DESCRIPTION
  Copies RF RX NV from channel index i-1 into index i.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  RF RX NV is modified, and may be inconsistent until the caller has
  finished the entire move operation.
  
===========================================================================*/
static void ftm_1x_rx_nv_move_prev_up( rfnv_digital_items_type* rfnv_ptr, rf_path_enum_type path, int i )
{
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode;
#endif /* RF_HAS_POWER_MODE_LIB */
  
  ASSERT(i >= 1 && i < NV_FREQ_TABLE_SIZ);

  rfnv_ptr->rfnv_rx_cal_chan[path][i] = rfnv_ptr->rfnv_rx_cal_chan[path][i-1];
  rfnv_ptr->rfnv_rx_cal_chan_lru[path][i] = rfnv_ptr->rfnv_rx_cal_chan_lru[path][i-1];
#ifdef RF_HAS_POWER_MODE_LIB
  for ( power_mode = RFLIB_NORMAL_POWER_MODE;
        power_mode < RFLIB_MAX_POWER_MODE;
        power_mode++ )
  {
    rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][i-1];
    rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][i-1];
    rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][i-1];
    rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][i-1];
    rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][power_mode][i]
      = rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][power_mode][i-1];
  }
#else
  rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][i-1];
  rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][i-1];
  rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][i-1];
  rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][i-1];
  rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][i] 
   = rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][i-1];
#endif /* RF_HAS_POWER_MODE_LIB */
} /* ftm_1x_rx_nv_move_prev_up */


/*===========================================================================

FUNCTION FTM_1X_RX_NV_CREATE_BLANK

DESCRIPTION
  Initializes a frequency calibration item at the specified index.
  Increments all LRU items and sets the specified entry to the tail.
  
DEPENDENCIES
  Any data that must be saved in the current entry must have already
  been saved.

RETURN VALUE
  None

SIDE EFFECTS
  RX cal channel LRU order changes.
  
===========================================================================*/
static void ftm_1x_rx_nv_create_blank( rfnv_digital_items_type* rfnv_ptr, rf_path_enum_type path, int i, word channel )
{
  int j;
#ifdef RF_HAS_POWER_MODE_LIB
  rflib_power_mode_type power_mode;
#endif /* RF_HAS_POWER_MODE_LIB */
  
  ASSERT(i >= 0 && i < NV_FREQ_TABLE_SIZ);

  /*
    Increment all LRU tracking entries, the new item will be going
    at the end of the list.
  */
  for (j = 0; j < NV_FREQ_TABLE_SIZ; j++)
  {
    /* Stop if the list ends prematurely */
    if (rfnv_ptr->rfnv_rx_cal_chan_lru[path][j] == 0)
    {
      break;
    }
    rfnv_ptr->rfnv_rx_cal_chan_lru[path][j]++;
  }

  rfnv_ptr->rfnv_rx_cal_chan[path][i] = channel;
  rfnv_ptr->rfnv_rx_cal_chan_lru[path][i] = 1;
#ifdef RF_HAS_POWER_MODE_LIB
  for ( power_mode = RFLIB_NORMAL_POWER_MODE;
        power_mode < RFLIB_MAX_POWER_MODE;
        power_mode++ )
  {
    rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][power_mode][i] = 0;
    rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][power_mode][i] = 0;
    rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][power_mode][i] = 0;
    rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][power_mode][i] = 0;
    rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][power_mode][i] = 0;
  }
#else
  rfnv_ptr->rfnv_cdma_lna_1_offset_vs_freq[path][i] = 0;
  rfnv_ptr->rfnv_cdma_lna_2_offset_vs_freq[path][i] = 0;
  rfnv_ptr->rfnv_cdma_lna_3_offset_vs_freq[path][i] = 0;
  rfnv_ptr->rfnv_cdma_lna_4_offset_vs_freq[path][i] = 0;
  rfnv_ptr->rfnv_vga_gain_offset_vs_freq[path][i] = 0;
#endif /* RF_HAS_POWER_MODE_LIB */

} /* ftm_1x_rx_nv_create_blank */

#endif /* FEATURE_FACTORY_TESTMODE */

