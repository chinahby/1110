/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     ANALOG BASEBAND RECEIVER SERVICES

GENERAL DESCRIPTION
  This file contains support for the Baseband Receiver in the analog
  portion of the MSM.


EXTERNALIZED FUNCTIONS
  None.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/bbrx.c#1 $
  $DateTime: 2008/04/15 22:54:48 $
  $Author: gswiech $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   vm      Added more Support for supporting Single chain on QSC1100
03/12/08   vm      Added Support for supporting Single chain on QSC1100
02/07/06   bmg     Cleaned up customized SC2x BBRX module

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "bbrx.h"

#include "bbrx_qsc1100.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/



/*===========================================================================

FUNCTION BBRX_INIT                                      EXTERNALIZED FUNCTION

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bbrx_init( void )
{
   bbrx_qsc1100_init();
} /* end of bbrx_init */


/*===========================================================================

FUNCTION      BBRX_SET_MODE                             EXTERNALIZED FUNCTION

DESCRIPTION

  This function sets the registers to the appropriate value when 
  switching to different systems
  The arrays passed in to the macros are defined at the top of the file

PARAMETERS

  bbrx_mode_type is an enumerated value that denotes the desired 
  system to switch to.

DEPENDENCIES

  sbi_init() must have been called. (for sbi communication targets)
  adie_init() must have been called.


RETURN VALUE
  TRUE  - if valid mode selected
  FALSE - if unknown mode selected

SIDE EFFECTS
  none
===========================================================================*/
#ifdef RF_HAS_MULTI_RX_CHAIN
boolean bbrx_set_mode(bbrx_rf_path_type path, bbrx_mode_type mode)
#else
boolean bbrx_set_mode(bbrx_mode_type mode)
#endif
{  
   #ifdef RF_HAS_MULTI_RX_CHAIN 
   return bbrx_qsc1100_set_mode(path, mode);
   #else
   return bbrx_qsc1100_set_mode(BBRX_CHAIN_0, mode);
   #endif
} /* end of bbrx_set_mode */


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
void bbrx_set_adc_source( bbrx_rf_path_type path,
                          bbrx_adc_clock_source_type source )
{
   bbrx_qsc1100_set_adc_source(path, source);
}
