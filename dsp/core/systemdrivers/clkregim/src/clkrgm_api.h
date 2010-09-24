#ifndef CLKRGM_API_H
#define CLKRGM_API_H
/*===========================================================================

   C L O C K   R E G I M E    H E A D E R    F I L E

DESCRIPTION
  This header file contains common clock regime external definitions 
  services.

  Copyright (c) 2004 by QUALCOMM, Incorporated. All Rights Reserved.
=========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/clkregim/clkrgm_api.h#1 $    $DateTime: 2007/11/12 18:05:22 $ $Author: bcalder $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/05   eav     Updated clk_regime_set_mdsp_clk_frequency() api.
03/22/05   eav     Replaced clk_regime_sel_clk_inv. Req'd for camera.
03/10/05   eav     Culled misc_reset, Uncommented clk_regime_reset()
3/08/05    eav     Changed decl for camera clock switch function
3/07/05    eav     Removed mdsp and gsm_core checking functions.  Added is_on().  
                   Removed extra comments.
2/28/05    eav     Added api to check if mdsp and gsm_core are on
2/25/05    eav     Removed get_rxmod_src_select_value and get_nand wait states.
                   Both are unused.
12/9/04    eav     Added API for switching mclock back and forth from 9.6MHz
09/07/04   abp     Initial release for common external interfaces.

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.     */
#include "msm.h"


/*===========================================================================

                DEFINITIONS AND DECLARATIONS

===========================================================================*/

/* USB CHG code can set the clocks into a charging mode */
typedef enum
{
  CLKRGM_USB_CHG_MODE,
  CLKRGM_USB_NORMAL_MODE
} clkrgm_usb_chg_mode_type;


/*===========================================================================

  FUNCTION CLK_REGIME_INIT_FOR_DLOAD_THRU_USB

  DESCRIPTION
    This is the MSM clock regime initialization function prior
    to invoking dload.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/
void clk_regime_init_for_dload_thru_usb( void );


/*===========================================================================

  FUNCTION CLK_REGIME_INIT_FOR_DLOAD_THRU_UART

  DESCRIPTION
    This is the MSM clock regime initialization function prior
    to invoking dload.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/

void clk_regime_init_for_dload_thru_uart( void );

/*===========================================================================

  FUNCTION CLK_REGIME_INIT_FOR_DLOAD

  DESCRIPTION
    This is the MSM clock regime initialization function prior
    to invoking dload.

  PARAMETERS
    None.

  DEPENDENCIES
    TCXO and SLEEP xtal clock sources need to be stable.

  RETURN VALUE
    None.

  SIDE EFFECTS
    The PLL and clock regime hardware is initialized.

===========================================================================*/

void clk_regime_init_for_dload( void );

/*===========================================================================
FUNCTION CLK_REGIME_GET_MAX_HCLK_FREQ_KHZ

DESCRIPTION
  This function returns the frequency in KHz of the maximum HCLK frequency

DEPENDENCIES
  None.

RETURN VALUE
  The frequency in KHz of the maximum HCLK frequency

SIDE EFFECTS
  None.
===========================================================================*/
uint32 clk_regime_get_max_hclk_freq_khz( void );

/*===========================================================================

FUNCTION CLK_REGIME_GET_USB_XTAL_WARMUP_TIME_USECS

DESCRIPTION
  Customizable function that allows user to configure warmup time on based on 
  USB xtal specs.

DEPENDENCIES
  None

RETURN VALUE
  USB xtal warmup time in microseconds.

SIDE EFFECTS
  None

===========================================================================*/
uint32 clk_regime_get_usb_xtal_warmup_time_usecs(void); 


/*===========================================================================

FUNCTION  CLKRGM_SET_USB_CHG_MODE

DESCRIPTION
  This function initializes the MSM into a low power state to improve the USB
  charging capabilities at boot up. It puts the MSM into low power mode, and
  takes it out of low power mode by restoring the registers that it touches.
  When MSM is in low power mode, CPU clock rate is switched down, and turns off
  the DCPLL, in order to save power. 

DEPENDENCIES
  NONE
  
RETURN VALUE
  TRUE if successful in setting the mode, FALSE otherwise

SIDE EFFECTS
  Clock speed, clock regimes may be changed.

===========================================================================*/
boolean clkrgm_set_usb_chg_mode(clkrgm_usb_chg_mode_type mode);

/*===========================================================================

FUNCTION  CLKRGM_GET_USB_CHG_MODE

DESCRIPTION
  This function returns whether USB CHG code has put the MSM into
  CLKRGM_USB_CHG_MODE, or if it is in CLKRGM_USB_NORMAL_MODE

DEPENDENCIES
  NONE
  
RETURN VALUE
  Returns the state of the MSM from USB CHG perspective   

SIDE EFFECTS
  None.

===========================================================================*/
clkrgm_usb_chg_mode_type clkrgm_get_usb_chg_mode(void);



#endif /*  CLKRGM_API_H */


