/*==============================================================================
 
  GENERAL DESCRIPTION
    
  EXTERNALIZED FUNCTIONS
    hsu_init_legacy_phy
    hsu_init_fs_usb

  INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_config_selector_init() should be called before any other function in 
    this file is called.

  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_legacy_phy.c#3 $
  $DateTime: 2009/02/22 08:52:33 $ $Author: ablay $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  02/08/07  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "comdef.h"

#ifdef FEATURE_HS_USB_LEGACY_HOST

#if defined(T_SURF) && defined(T_MSM7500)
#error code not present
#endif /* defined(T_SURF) && defined(T_MSM7500) */

#endif /* FEATURE_HS_USB_LEGACY_HOST */

#include "msmhwio.h"
#include "msmhwioreg.h" 
#include "hsu_common.h"
#include "clkrgm_msm.h"
#include "pm.h"

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else /* !FEATURE_MULTIPROCESSOR */
#include "bio.h"
#endif /* FEATURE_MULTIPROCESSOR */

#include "err.h"

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef FEATURE_HS_USB_LEGACY_HOST
#if defined(T_SURF) && defined(T_MSM7500)
#error code not present
#endif /* defined(T_SURF) && defined(T_MSM7500) */

#endif /* FEATURE_HS_USB_LEGACY_HOST */

/* Voltage setting for the USB VREG = 3.3 Volts. */
#define PM_VREG_USB_LEVEL         (3300)

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
#ifdef FEATURE_HS_USB_LEGACY_HOST

#if defined(T_SURF) && defined(T_MSM7500)
#error code not present
#endif /* T_SURF && T_MSM7500*/

#endif /* FEATURE_HS_USB_LEGACY_HOST */
/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

#ifdef FEATURE_HS_USB_LEGACY_HOST

/*===========================================================================

FUNCTION      USB_PHILIPS_XCVR_I2C_WRITE_BYTE

DESCRIPTION
Write a byte to Philips Xcvr I2C

DEPENDENCIES
None

RETURN VALUE
TRUE if successful, FALSE otherwise

SIDE EFFECTS
None

===========================================================================*/
static boolean usb_philips_xcvr_i2c_write_byte(
  i2c_dev_addr_type xcvr_i2c_addr, 
  byte reg, 
  byte data)
{
  int i;
  static uint8 my_data;
  my_data = data;
  usb_philips_xcvr_i2c_command.bus_id = I2C_BUS_HW_CTRL;
  usb_philips_xcvr_i2c_command.options = (i2c_options_type)(I2C_REG_DEV | I2C_START_BEFORE_READ);
  usb_philips_xcvr_i2c_command.slave_addr = xcvr_i2c_addr; 
  usb_philips_xcvr_i2c_command.addr.reg = reg;
  usb_philips_xcvr_i2c_command.buf_ptr = &my_data;
  usb_philips_xcvr_i2c_command.len = 1;
  /* Try 10 times */
  for (i = 0; i < 10 ; i++)
  {
    if (i2c_write(&usb_philips_xcvr_i2c_command) == I2C_SUCCESS)
    {
      return TRUE;
    }
  }
  return FALSE;
}
/*===========================================================================
FUNCTION  HSU_INIT_LEGACY_PHY    

DESCRIPTION
This function inits Philips phy 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_init_legacy_phy(void)
{
  /*taken from usb_philips_xcvr_host_init */
#if defined(T_SURF) && defined(T_MSM7500)
#error code not present
#endif /* defined(T_SURF) && defined(T_MSM7500) */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_MODE_CTL_REG1_CLR, PH_MODE_CTL_REG1_BMASK );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_MODE_CTL_REG1_SET, (OE_INT_ENA|SPEED_REG|DAT_SE0) );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_MODE_CTL_REG2_CLR, GLOBAL_PWR_DOWN );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, PH_OTG_CTL_REG1_CLR, 0xFF );
  /* For a host device, both D+ and D- must be pulled down, and VBUS on */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_OTG_CTL_REG1_SET, VBUS_DRV | 
    DP_PULLDOWN | DM_PULLDOWN );
  /* Enable the external charge pump */
#ifndef T_FFA
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, PH_MODE_CTL_REG2_SET, PSW_OE );
#endif
  /* Enable VBUS LOW interrupt */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_INT_ENA_LOW_REG_SET, VBUS_VLD_INT );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_INT_LATCH_REG_CLR, VBUS_VLD_INT );
  /* Enable D+/D- interrupt */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, 
    PH_INT_ENA_HIGH_REG_SET, DP_HI_INT | DM_HI_INT );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, PH_INT_LATCH_REG_CLR, 
    DP_HI_INT | DM_HI_INT );
  /* Now turn off power, it will come back automatically with D+/D- activity. */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR2_I2C_ADDR, PH_MODE_CTL_REG2_SET, 
    GLOBAL_PWR_DOWN );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_MODE_CTL_REG1_CLR, 
    PH_MODE_CTL_REG1_BMASK );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_MODE_CTL_REG1_SET, 
    (OE_INT_ENA|SPEED_REG) );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_MODE_CTL_REG2_CLR, 
    GLOBAL_PWR_DOWN );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_OTG_CTL_REG1_CLR, 0xFF );
  // For a host device, both D+ and D- must be pulled down, and VBUS on
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_OTG_CTL_REG1_SET, 
    VBUS_DRV | DP_PULLDOWN | DM_PULLDOWN );
  /* Enable the external charge pump */
#ifndef T_FFA
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_MODE_CTL_REG2_SET, PSW_OE );
#endif
  /* Enable VBUS LOW interrupt */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_INT_ENA_LOW_REG_SET, 
    VBUS_VLD_INT );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_INT_LATCH_REG_CLR, 
    VBUS_VLD_INT );
  /* Enable D+/D- interrupt */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_INT_ENA_HIGH_REG_SET, 
    DP_HI_INT | DM_HI_INT );
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_INT_LATCH_REG_CLR, 
    DP_HI_INT | DM_HI_INT );
  /* Now turn off power, it will come back automatically with D+/D- activity. */
  usb_philips_xcvr_i2c_write_byte(PHILIPS_OTG_XCVR3_I2C_ADDR, PH_MODE_CTL_REG2_SET, 
    GLOBAL_PWR_DOWN );
}/* hsu_init_legacy_phy */

#endif /* FEATURE_HS_USB_LEGACY_HOST */

#if (defined(FEATURE_HS_USB_LEGACY_DEVICE) && !defined(FEATURE_HS_USB_BASIC))

/* Taken from oemsbl_hw_init.c, pm_init_otg_usb() */
/*===========================================================================

FUNCTION hsu_pm_init_fs_usb

DESCRIPTION
Perform PMIC-related initialization needed for using the full-speed USB core.

DEPENDENCIES
PMIC must be initialized.

RETURN VALUE
NONE

SIDE EFFECTS
None.

===========================================================================*/
static pm_err_flag_type hsu_pm_init_fs_usb( void )
{
  pm_err_flag_type ret = PM_ERR_FLAG__SUCCESS;

  /* derive power for OTG xcvr from USB_VBUS provided by the host */
  ret = pm_otg_pwr_sel( PM_OTG_PWR_SEL__VBUS);
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_otg_pwr_sel failed with %d ", ret,0,0);
    return ret;
  }

  /* Turn on VREG_USB: */
  /* Set the VREG voltage level */
  ret = pm_vreg_set_level( PM_VREG_USB_ID, PM_VREG_USB_LEVEL);
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_vreg_set_level failed with %d ", ret,0,0);
    return ret;
  }

  /* Turn on the VREG */
  ret = pm_vreg_control( PM_ON_CMD, PM_VREG_USB_M);
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_vreg_control failed with %d ", ret,0,0);
    return ret;
  }

  /* activate the OTG xcvr */
  ret = pm_otg_config_power_management( PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_otg_config_power_management failed with %d ", ret,0,0);
    return ret;
  }

  /* pull down D+/- low initially to start off from a USB disconnect state */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DP_CONFIG_CMD__PULL_DOWN, 
    PM_OTG_DM_CONFIG_CMD__PULL_DOWN, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_otg_config_interface failed with %d ", ret,0,0);
    return ret;
  }

  /* reset OTG transceiver's mode */
  ret = pm_otg_reset_mode();
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_otg_reset_mode failed with %d ", ret,0,0);
    return ret;
  }

  /* config OTG xcvr for USB B-dev mode */
  ret = pm_otg_config_usb_mode( PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);
  if (ret != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR(
      "hsu_pm_init_fs_usb(): pm_otg_config_usb_mode failed with %d ", ret,0,0);
    return ret;
  }

  /* pull D+ high to indicate to PC that the phone is ready for USB traffic */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE,
    PM_OTG_DP_CONFIG_CMD__PULL_UP,
    PM_OTG_DM_CONFIG_CMD__NONE, 
    PM_OTG_ID_CONFIG_CMD__NONE);

  return ret;
}

/*===========================================================================

FUNCTION hsu_init_fs_usb

DESCRIPTION
Perform initialization needed for using the full-speed USB core.

DEPENDENCIES
None.

RETURN VALUE
NONE

SIDE EFFECTS
None.

===========================================================================*/
void hsu_init_fs_usb( void )
{
  pm_err_flag_type pm_result;

  { /* Initialize clocks. This code was taken from VI. */
    uint32 m=1; 
    uint32 n=8;
    uint32 d=4; //768/2=384, 384/8=48MHz

    HWIO_OUT(USB_MD_REG, ((0xFFFF&(m))<<16)|(0xFFFF&(~(2*d))) /*clk_regime_md_reg_val(m, d)*/ ); 
    HWIO_OUT(USB_NS_REG, ((0xFF&(~(n-m)))<<16) | 0x1<<12 | 0x1<<11 | 0x1<<9 | 0x1<<8 | 
      0x0<<7 | 0x2<<5 | 0x1<<3 | 0x1 ); 
  }

#ifdef T_MSM7625
  /* In sROC and Karura, PMIC PHY is connected to MSM via GPIOs. */
  gpio_tlmm_config(USB1_DAT_VP);
  gpio_tlmm_config(USB1_SE0_VM);
  gpio_tlmm_config(USB1_OE_INT_N);
#endif /* T_MSM7625 */

  pm_result = hsu_pm_init_fs_usb();
  HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);

  clk_regime_usb_xtal_on();

  HWIO_USB_PIN_CONFIG_OUTM(HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK, 0);

  HWIO_USB_PIN_SEL_OUT(0);            
}

#endif /* FEATURE_HS_USB_LEGACY_DEVICE && !FEATURE_HS_USB_BASIC */
