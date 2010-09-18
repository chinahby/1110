#include "target.h"
#ifdef T_QSC60X5

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     ANALOG BASEBAND (Sigma Delta) RECEIVER SERVICES

GENERAL DESCRIPTION
  This file contains support for the Baseband Receiver in the analog
  portion of the MSM.  The majority of the programming of the Sigma Delta
  configuration is done within the RaMSIS library.


EXTERNALIZED FUNCTIONS
  None.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2006 - 2007 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/bbrx_1x.c#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/08   vm      Added Support for supporting Single chain on QSC1100
03/21/07   dyc     Fixed compiler warning.
11/29/06   dyc     Stub out routines for bringup.
11/26/06   dyc     Created.  Template from Saturn based code.

===========================================================================*/
#include "target.h"
#include "customer.h"

#include "comdef.h"
#include "adie.h"

#include "bbrx.h"
#include "err.h"
#include "hw.h"

#include "bbrx_1x.h"
#include "adie_sbi.h"

#include CLKRGM_H


/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

/*--------------------------------------------------------------
            Baseband Receiver Register Definitions
----------------------------------------------------------------*/

/* RaMSIS SSBI Registers */
#define PRX_ADC_CLK_R          0x25
#define DRX_ADC_CLK_R          0x9F

#define PRX_UHF_LO_V           0x00
#define DRX_UHF_LO_V           0x01



/*===========================================================================

FUNCTION BBRX_1x_INIT                                   EXTERNALIZED FUNCTION

DESCRIPTION
  Initialization of the BBRX component in RaMSIS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bbrx_1x_init( void )
{

  return;
  /* initialize any BBRX clocking here */
  
  #ifdef RF_HAS_MULTI_RX_CHAIN 
  /* initialize the set of registers that only need to be initialized once */ 
  bbrx_set_mode(BBRX_CHAIN_0,BBRX_INIT_MODE);

  /* initialize all the BBRX in cellular mode to start, because we currently
     don't know what mode we're in, so just assume CELL */  
  bbrx_set_mode(BBRX_CHAIN_0,BBRX_CELL_MODE);
  #else

  /* initialize the set of registers that only need to be initialized once */ 
  bbrx_set_mode(BBRX_INIT_MODE);

  /* initialize all the BBRX in cellular mode to start, because we currently
     don't know what mode we're in, so just assume CELL */  
  bbrx_set_mode(BBRX_CELL_MODE);
  #endif

} /* end of bbrx_init */



/*===========================================================================

FUNCTION      BBRX_1x_SET_MODE                          EXTERNALIZED FUNCTION

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
boolean bbrx_1x_set_mode (bbrx_rf_path_type path, bbrx_mode_type mode)
{
  uint8 data;  /* sbi register value */

#ifdef T_QSC60X5
  /* BBRX manaaged in RaMSIS RF driver */
  return TRUE;
#endif /* T_QDC60X5 */

  switch (mode) 
  {
    case BBRX_INIT_MODE :
      break;

    case BBRX_CELL_MODE :
    case BBRX_PCS_MODE :

      /* Determine path to switch clock sampling */
      switch (path)
      { 
      case BBRX_CHAIN_0:  /* Chain0 always configured to use ch0 VCO */

          /* Read the data and add bit fields 3:2 for clk src */
          (void)adie_bbrx_read_byte(PRX_ADC_CLK_R, &data);
          data &= (0xFC | (PRX_UHF_LO_V << 2));

          /* Configure PRX ADC register */
          (void)adie_bbrx_write_byte(PRX_ADC_CLK_R, 0xFF, data);

          break;

      case BBRX_CHAIN_1:
           /* Read the data and add bit fields 3:2 for clk src */
          (void)adie_bbrx_read_byte(DRX_ADC_CLK_R, &data);
          data &= (0xFC | (DRX_UHF_LO_V << 2));

          /* Configure PRX ADC register */
          (void)adie_bbrx_write_byte(DRX_ADC_CLK_R, 0xFF, data);

          break;

      default:
          break;

      }

      break;

#ifdef RF_HAS_GPS
#error code not present
#endif  //RF_HAS_GPS

    default :
      ERR("UNKNOWN MODE SELECTION",0,0,0);
      return( FALSE );
  }

  return ( TRUE);

} /* end of bbrx_6500_set_mode */

#endif /* T_QSC60X5 */
