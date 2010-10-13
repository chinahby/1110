#ifndef BBRX_QSC1100_H
#define BBRX_QSC1100_H

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

  $Header: //source/qcom/qct/modem/rfa/device/common_rf/main/qsc1100/protected/bbrx_qsc1100.h#1 $
  $DateTime: 2008/04/16 15:00:30 $
  $Author: gswiech $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/08   vm      Included rficap.h file
12/10/07   ems     Ported from SC2X 3044

===========================================================================*/

#include "bbrx.h"
#include "rficap.h"

/*===========================================================================

FUNCTION BBRX_QSC1100_INIT                                 EXTERNALIZED FUNCTION

DESCRIPTION
  Initialization of the BBRX component in Napolean.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bbrx_qsc1100_init( void );


/*===========================================================================

FUNCTION      BBRX_QSC1100_SET_MODE                        EXTERNALIZED FUNCTION

DESCRIPTION

  This function will set the appropriate ADC Sigma Delta basebase sampling clock.

PARAMETERS

  adie_system_mode is an enumerated value that denotes the desired 
  system to switch to.

DEPENDENCIES
  sbi_init() must have been called. (for sbi communication targets)

RETURN VALUE
  TRUE  - if valid mode selected
  FALSE - if unknown mode selected

SIDE EFFECTS
  none

===========================================================================*/
boolean bbrx_qsc1100_set_mode (bbrx_rf_path_type path, bbrx_mode_type mode);


/*===========================================================================

FUNCTION BBRX_SET_ADC_SOURCE                            EXTERNALIZED FUNCTION

DESCRIPTION
  Configures the BBRX ADC clock source

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ADC sample rate may change, depending on clock source

===========================================================================*/
void bbrx_qsc1100_set_adc_source( bbrx_rf_path_type,
                               bbrx_adc_clock_source_type source );

#endif /* BBRX_QSC1100_H */
