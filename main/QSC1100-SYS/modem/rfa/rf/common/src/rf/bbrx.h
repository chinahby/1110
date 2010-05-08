#ifndef BBRX_H
#define BBRX_H
/*===========================================================================


                    ANALOG BASEBAND RECEIVER HEADER FILE

DESCRIPTION
  This file contains declarations and definitions to support interaction
  with the analog die of a stacked digital/analog die MSM.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/bbrx.h#1 $
  $DateTime: 2008/04/15 22:54:48 $
  $Author: gswiech $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   vm      Added Support for supporting Single chain on QSC1100
02/07/06   bmg     Cleaned up customized SC2x BBRX module

===========================================================================*/

/* -------------------------------------------------------------------------

                            EXPORTED DATA TYPES

---------------------------------------------------------------------------- */

/* BBRX mode type */
#include "rficap.h"

typedef enum
{
  BBRX_INIT_MODE,
  BBRX_AMPS_MODE,
  BBRX_CELL_MODE,
  BBRX_PCS_MODE,
  BBRX_GPS_MODE,
  BBRX_WCDMA_MODE,
  BBRX_GSM_MODE,
  BBRX_INIT_GSM_MODE,
  BBRX_GSM_CH1_MODE,
  BBRX_WCDMA_RXDIV_MODE,
  BBRX_POWER_DOWN_2ND_CH_MODE
} bbrx_mode_type;

/* rf path for bbrx mode configuration */

typedef enum{
  BBRX_CHAIN_0,
  BBRX_CHAIN_1,
  BBRX_CHAIN_BOTH
} bbrx_rf_path_type;

typedef enum {
   BBRX_PRX_VCO,
   BBRX_DRX_VCO,
   BBRX_EXTERNAL_CLOCK
} bbrx_adc_clock_source_type;


#ifdef __cplusplus
extern "C"
{
#endif
/*===========================================================================

FUNCTION BBRX_INIT                                      

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bbrx_init( void );

/*===========================================================================

FUNCTION      BBRX_SET_MODE                             

DESCRIPTION

  This function sets the registers to the appropriate value when 
  switching to different systems

PARAMETERS

  adie_system_mode is an enumerated value that denotes the desired 
  system to switch to.

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef RF_HAS_MULTI_RX_CHAIN 
boolean bbrx_set_mode(bbrx_rf_path_type path, bbrx_mode_type mode );
#else
boolean bbrx_set_mode(bbrx_mode_type mode );
#endif



/*===========================================================================

FUNCTION BBRX_SELECT_ADC_SOURCE                         EXTERNALIZED FUNCTION

DESCRIPTION
  Configures the BBRX ADC clock source

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ADC sample rate may change, depending on clock source

===========================================================================*/
void bbrx_set_adc_source( bbrx_rf_path_type,
                          bbrx_adc_clock_source_type source );

#ifdef __cplusplus
}
#endif
#endif /* BBRX_H */
