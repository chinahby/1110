/*==============================================================================

  High Speed USB Module Remote APIs on Modem processor 
  
  GENERAL DESCRIPTION
    This module defines High Speed USB APIs that are implemented on the modem 
    processor is the server. 

  EXTERNALIZED FUNCTIONS
    hsu_api_init_phy
    hsu_api_vbus_powerup
    hsu_api_vbus_shutdown
    hsu_api_update_product_id
    hsu_api_update_serial_number
    hsu_api_update_is_serial_number_null
    hsu_api_init_chg
    hsu_api_chg_is_connected
    hsu_api_enable_pmic_otg_comparators
    hsu_api_disable_pmic_otg_comparators
    hsu_api_init_uicc
    hsu_api_start_uicc
    hsu_api_disconnect_pulldowns_uicc

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2007, 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_mdm_apis.c#63 $
  $DateTime: 2009/03/29 08:32:35 $ $Author: dsegal $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  02/08/07  ke  Initial version
  05/02/07  me  Add functions to update the product id and the serial number

==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_apis.h"
#include "comdef.h"
#include "hsu_common.h"
#include "clk.h"
#include "hsu_phy_utils.h"
#ifdef T_QSC6270
#include "hsu_config_selector.h"
#endif /* T_QSC6270 */
#if (defined(IMAGE_MODEM_PROC) && !defined(BUILD_HOSTDL)) || !defined(FEATURE_MULTIPROCESSOR)
#include "pm.h"
#include "msm.h"
#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else
  #include "bio.h"
#endif /* FEATURE_MULTIPROCESSOR */
#include "rex.h"

#ifdef FEATURE_L4
#error code not present
#endif /* FEATURE_L4 */  

#endif /* IMAGE_MODEM_PROC && !BUILD_HOSTDL */

#ifndef FEATURE_HS_USB_BASIC
  #include "hsu_modem_proc_services.h"
  #include "hsu_conf_sel_types.h"
  #ifndef FEATURE_HS_USB_CHG_REMOTE_API
  #include "hsu_chg.h"
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
  #ifdef FEATURE_HS_USB_LEGACY_DEVICE
    #include "clkrgm_msm.h"
    #include "pm.h"
  #endif /* FEATURE_HS_USB_LEGACY_DEVICE */
  #ifdef FEATURE_HS_USB_LEGACY_HOST
    #include "clkrgm_msm.h"
    #include "pmapp_otg_custom_hw.h"
    //include "pm66XXotg.h"
    #include "pm.h"
  #endif /*FEATURE_HS_USB_LEGACY_HOST*/
#elif !defined(BUILD_HOSTDL)
#include "boothw.h"
#endif

#if defined(T_MSM6246) || defined(T_QSC6270) || defined(T_MSM6290) || defined(T_QSC1100)
  #include "clkrgm_msm.h"
  #include "clkrgm_api.h"
  #include "hsu_common.h"
#endif /* T_MSM6246 || T_QSC6270  || T_MSM6290 || T_QSC1100 */

#ifdef FEATURE_HS_USB_CHARGING
  #include "hsu_chg_wallchg.h"
#endif /* FEATURE_HS_USB_CHARGING */

#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#else
#error   "HAL is required for HS-USB"
#endif /* FEATURE_HS_USB_HAL */

#if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))
#include "hsu_usb_uicc.h"
#endif /* FEATURE_HS_USB_UICC & !FEATURE_HS_USB_BASIC */

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
/* 
 * Define the PM_MPP to be used in order to power up / down the VBUS
 */
#ifndef FEATURE_HS_USB_BASIC
  #ifdef T_FFA
#error code not present
  #else
    #define HSU_MPP_FOR_VBUS  PM_MPP_16
  #endif /* FFA */
#endif /* FEATURE_HS_USB_BASIC */


/* Voltage setting for the USB VREG = 3.3 Volts. */
#define PM_VREG_USB_LEVEL         (3300)

/* UICC PMIC configuration settling time */
#define HSU_UICC_PMIC_SETTLING_TIME_MS 100

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
#ifndef FEATURE_HS_USB_BASIC
  static boolean vbus_is_on = FALSE;

#ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
  static boolean usb_reset_rework_irq_handle_enabled = FALSE;
  static boolean tested_usb_reset_rework_is_installed = FALSE;
  static boolean usb_reset_rework_is_installed = FALSE;
#endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */


#endif /* FEATURE_HS_USB_BASIC */

#ifdef FEATURE_HS_USB_CHARGING
#if (defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL))
  static boolean mdm_apis_is_wall_charger = FALSE;
#endif /* defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
#endif /* FEATURE_HS_USB_CHARGING */

#if !(defined FEATURE_DONT_USE_PMIC_OTG_COMPARATORS) && \
  defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_BASIC)
  static boolean mdm_apis_pmic_comparators_enabled = FALSE ;
#endif /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS &&... !FEATURE_HS_USB_BASIC*/
/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/
#ifndef BUILD_HOSTDL
#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
static boolean hsu_verify_ulpi_phy_access(uint8 instance, uint8 port);
#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
#endif /* !BUILD_HOSTDL */


#ifndef BUILD_HOSTDL
/*==============================================================================

FUNCTION          HSU_DELAY_MS

DESCRIPTION
  Helper function. Delays execution for the given time in milliseconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

==============================================================================*/
static void hsu_delay_ms(uint32 milliseconds)
{
#ifndef FEATURE_HS_USB_BASIC
  rex_sleep(milliseconds);
#else
	volatile uint32 counter;

	BOOTHW_KICK_WATCHDOG();

	for (counter = 0; counter < milliseconds; counter++)
    {
        clk_busy_wait(1000); 
        BOOTHW_KICK_WATCHDOG();
    }   
#endif /*FEATURE_HS_USB_BASIC*/
}
#endif /* BUILD_HOSTDL */

#if (!defined FEATURE_HS_USB_BASIC) && (defined FEATURE_HS_USB_LEGACY_HOST)

/*==============================================================================
FUNCTION          INIT_PHILIPS_PHY

DESCRIPTION
This function configures philips phy for using legasy host via usb2 port

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 
==============================================================================*/
static void init_philips_phy(void)
{
  /* enable PMIC VBUS boost: */
  (void) pm_otg_reset_mode(); //will not be needed for interchip
  /* Configures the 4 USB interface lines - Vbus, D+, D- and ID
  lines at the same time*/
  (void) pm_otg_config_interface(
    PM_OTG_VBUS_CONFIG_CMD__NONE,
    PM_OTG_DP_CONFIG_CMD__PULL_DOWN,
    PM_OTG_DM_CONFIG_CMD__PULL_DOWN,
    PM_OTG_ID_CONFIG_CMD__NONE ); //will probably not be needed for interchip
  (void) pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST); //will not be needed for interchip
  /* taken from usbhc_hw_init */
  /* Will the data on those lines be USB or UART */
#ifdef HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK
  /* MSM7500 */
  HWIO_USB_PIN_CONFIG_OUTM(HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK, 0);
#else /*!HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK*/ /* the else will probably be deleted */
  HWIO_USB_PIN_CONFIG_OUT(HWIO_USB_PIN_CONFIG_REG_OE_TP_OE_BMSK);
#endif /*HWIO_USB_PIN_CONFIG_REG_OE_INT_N_OE_BMSK*/
#ifdef HWIO_USB_PIN_SEL_OUT /* delete the ifdef - leave only the HWIO_USB_PIN_SEL_OUT(0); */
  HWIO_USB_PIN_SEL_OUT(0);            
#endif
}
#endif /* !FEATURE_HS_USB_BASIC && FEATURE_HS_USB_LEGACY_HOST */

/*===========================================================================
FUNCTION          HSU_WALL_CHG_DETECT_CB

DESCRIPTION
Called as a wall charger detection callback, with indication whether or not a USB wall charger is connected.

INPUT PARAMETERS
None

RETURN VALUE
None

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
#if (defined(FEATURE_HS_USB_CHARGING) && defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL))
static void hsu_wall_chg_detect_cb
(
  boolean is_wall_chg
)
{
  mdm_apis_is_wall_charger = is_wall_chg;
}
#endif /* defined(FEATURE_HS_USB_CHARGING) && defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */


#ifndef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
#if defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================

FUNCTION      HSU_API_LPM_EXIT_BY_DPC

DESCRIPTION
  An LPM exit callback to be called by the DPC task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_L4
#error code not present
#else
static boolean hsu_api_lpm_exit_by_dpc(unsigned long unused_param1,
                                       unsigned long unused_param2,
                                       unsigned long unused_param3)
#endif
{
  #ifdef FEATURE_L4
#error code not present
  #else
  HSU_USE_PARAM(unused_param1);
  HSU_USE_PARAM(unused_param2);
  HSU_USE_PARAM(unused_param3);
  #endif  /* FEATURE_L4 */

  hsu_api_exit_lpm();

  #ifndef FEATURE_L4
  return TRUE;
  #endif  /* FEATURE_L4 */

}  /* hsu_api_lpm_exit_by_dpc */
#endif  /* FEATURE_HS_USB_INTEGRATED_PHY && !FEATURE_HS_USB_BASIC */
#endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */


#ifndef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
#if defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================

FUNCTION      HSU_API_PMIC_OTG_ISR

DESCRIPTION
  Handles PMIC OTG interrupts during LPM.
  This is for catching USB cable connect and disconnect events during 
  low-power mode. Once this ISR is invoked, it exits the HS-USB module from
  low-power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_api_pmic_otg_isr(void* unused_param)
{
  boolean  result;


  HSU_USE_PARAM(unused_param);

  #ifdef FEATURE_L4
#error code not present
  #else
  result = rex_queue_dpc(hsu_api_lpm_exit_by_dpc, 0, 0, 0);
  #endif

  if (result == TRUE)
  {
    HSU_MSG_HIGH("hsu_api_pmic_otg_isr: Queued an LPM exit in the DPC task.",
                  0, 0, 0);
  }
  else
  {
    HSU_MSG_ERROR("hsu_api_pmic_otg_isr: Failed to queue an LPM exit in "
                  "the DPC task.",
                  0, 0, 0);
  }

}  /* hsu_api_pmic_otg_isr */
#endif  /* FEATURE_HS_USB_INTEGRATED_PHY && !FEATURE_HS_USB_BASIC */
#endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */


#ifndef BUILD_HOSTDL
#if defined(T_MDM8200) || defined(T_MSM7800) || defined(T_MSM8650B)
static void hsu_assert_phy_reset(void)
{
  MSM_SET( APPS_RESET, USB_PHY );
}
#elif defined(T_MSM7625)
static void hsu_assert_phy_reset(void)
{
  /* Set USB_PHY high.  MSM_CLEAR() is used because of HW bug in which the 
  reset signal of PHY is inverted, so clearing the reset bit actually asserts
  it. */
  MSM_CLEAR( APPS_RESET, USB_PHY );
}
#elif defined(T_MSM6246) || defined(T_MSM6290) || defined(T_QSC6270)
static void hsu_assert_phy_reset(void)
{
  clk_regime_msm_reset_assert(CLK_RGM_USB_PHY_RESET_M);
}
#endif /* T_MSM6246 || T_MSM6290 || T_QSC6270 */
#endif /* !BUILD_HOSTDL */

#ifndef BUILD_HOSTDL
#if defined(T_MDM8200) || defined(T_MSM7800) || defined(T_MSM8650B)
static void hsu_deassert_phy_reset(void)
{
  MSM_CLEAR( APPS_RESET, USB_PHY );
}
#elif defined(T_MSM7625)
static void hsu_deassert_phy_reset(void)
{
  /* Set USB_PHY low.  MSM_SET() is used because of HW bug in which the 
  reset signal of PHY is inverted, so setting the reset bit actually de-asserts
  it. */
  MSM_SET( APPS_RESET, USB_PHY );
}
#elif defined(T_MSM6246) || defined(T_MSM6290) || defined(T_QSC6270)
static void hsu_deassert_phy_reset(void)
{
  clk_regime_msm_reset_deassert(CLK_RGM_USB_PHY_RESET_M);
}
#endif /* T_MSM6246 || T_MSM6290 || T_QSC6270 */
#endif /* !BUILD_HOSTDL */


#ifndef BUILD_HOSTDL
#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
/*==============================================================================
FUNCTION HSU_RESET_ULPI_PHY

DESCRIPTION
  Resets ULPI PHY by asserting and de-asserting PHY reset pin.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

==============================================================================*/
static void hsu_reset_ulpi_phy(uint8 instance, uint8 port)
{
  HSU_USE_PARAM(instance);
  HSU_USE_PARAM(port);
  
  /* Assert PHY RESET */
  hsu_assert_phy_reset();

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  /* Deassert PHY RESET */
  hsu_deassert_phy_reset();

  /* Wait until PHY completely comes out of reset */
  hsu_delay_ms(1);
}
#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
#endif /* BUILD_HOSTDL */


#ifndef BUILD_HOSTDL
#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
/*==============================================================================
FUNCTION HSU_SOFT_RESET_LINK_CONTROLLER

DESCRIPTION
  Reset link controller via USBCMD LINK register as opposed to resetting
  link controller via APPS_RESET register or clk regime API.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

==============================================================================*/
static void hsu_soft_reset_link_controller(uint8 instance, uint8 port)
{
  uint32 loop_count = 0;
  uint32 max_loop_count = 10;

  HSU_USE_PARAM(instance);
  HSU_USE_PARAM(port);

  HAL_hsusb_Write32(0, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RST_BMSK);   

  /* Wait until reset is complete */
  while(0 != HAL_hsusb_GetBits32(0, HAL_HSUSB_USBCMD_ADDR, 
                                 HAL_HSUSB_USBCMD_RST_BMSK))
  {    
    HSU_ASSERT(loop_count < max_loop_count); 
    hsu_delay_ms(1);
    loop_count++;
  }  
}
#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
#endif /* BUILD_HOSTDL */


#ifndef BUILD_HOSTDL
#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
/*==============================================================================
FUNCTION hsu_recal_ulpi_phy

DESCRIPTION
  The integrated ULPI PHY requires re-calibration if 3.3V (or 2.6V for targets
  that have PLL tied to 2.6V) to PHY is turned on after the core voltage (1.2V 
  or 1.3V) to PHY is turned on.  

  Since we cannot control the order of enabling these power rails, it is safe 
  to re-calibrate the PHY for all targets that use integrated PHY.

  Reset of the PHY is not really required to enable the PHY PLL.
  As long as ULPI access is attempted after clearing suspendM bit, the PHY PLL
  will get turned on.  This initial ULPI read or write may fail.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If PHY re-calibration was successful.
  FALSE: If PHY re-calibration failed.

SIDE EFFECTS
  None. 

==============================================================================*/
static boolean hsu_recal_ulpi_phy(uint8 instance, uint8 port)
{
  uint8 ulpi_addr = 0;
  uint8 read_value = 0;
  boolean ret = TRUE;

  /* Check if PHY is alive before attempting the re-calibration sequence */
  ret = hsu_verify_ulpi_phy_access(instance, port);
  HSU_ASSERT(TRUE == ret);


  ulpi_addr = 0x6;

  /* clear suspendM by setting (bit 6) of ULPI PHY register 
     (func ctl clear) at addr 0x6 to stop ULPI PHY PLL */ 
  if (HAL_HSUSB_Success != HAL_hsusb_ReadUlpiPhy(instance, port, 
                                                 ulpi_addr, &read_value))
  {
    HSU_MSG_ERROR("hsu_recal_ulpi_phy: ULPI read failed", 0,0,0);
    return FALSE;
  }
  
  if (HAL_HSUSB_Success != HAL_hsusb_WriteUlpiPhy(instance, port, 
                                                  ulpi_addr, read_value | 0x40))
  {
    HSU_MSG_ERROR("hsu_recal_ulpi_phy: ULPI write failed", 0,0,0);
    return FALSE;
  }

  /* Reset ULPI PHY */
  hsu_reset_ulpi_phy(0, 0);

  return TRUE;
}
#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
#endif /* BUILD_HOSTDL */


#ifndef BUILD_HOSTDL
#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
/*==============================================================================
FUNCTION HSU_VERIFY_ULPI_PHY_ACCESS

DESCRIPTION 
  Verifies ULPI PHY register access by reading PHY debug register via 
  VIEWPORT.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: successfully read PHY debug register
  FALSE: PHY register read failed.

SIDE EFFECTS
  This function may reset ULPI PHY up to three times if ULPI register
  read fails. 
==============================================================================*/
static boolean hsu_verify_ulpi_phy_access(uint8 instance, uint8 port)
{
  uint32 loop_count = 0;
  uint32 max_loop_count = 3;
  uint8  ulpi_addr = 0x15;  /* Debug register: Vstatus[7:2], Linestate[1:0] */
  uint8  read_value = 0;
  HAL_HSUSB_ErrorType hal_err = HAL_HSUSB_Success;
  
  /* Try reading ULPI PHY debug register for calibration status */
  while (loop_count < max_loop_count)
  {
    hal_err = HAL_hsusb_ReadUlpiPhy(instance, port, ulpi_addr, &read_value);
    if(HAL_HSUSB_Success == hal_err)
    {
      break; /* ULPI read worked fine */
    }

    /* Reset ULPI PHY */
    hsu_reset_ulpi_phy(0, 0);
  
    loop_count++;

    HSU_MSG_ERROR("hsu_verify_ulpi_phy_access: ULPI PHY read failed = %d",
                  loop_count,0,0);
  }

  return (HAL_HSUSB_Success == hal_err);
}  /* hsu_verify_ulpi_phy_access */
#endif /* FEATURE_HS_USB_INTEGRATED_PHY */
#endif /* BUILD_HOSTDL */

#ifndef BUILD_HOSTDL
#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
#if defined(T_MDM8200) || defined(T_MSM7625) || defined(T_MDM8900)
/*==============================================================================

FUNCTION HSU_SET_ULPI_PHY_PREEMPHASIS         

DESCRIPTION
  Configures pre-emphasis parameters.  Some targets may require enabling the
  pre-emphasis and to set pre-emphasis depth to raise the voltage shown on
  eye-diagram. 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: Successfully updated pre-emphasis parameters.
  FALSE: Failed to update pre-emphasis parameters.

SIDE EFFECTS
  None. 

==============================================================================*/
static boolean hsu_set_ulpi_phy_preemphasis(uint8 instance, 
                                            uint8 port,
                                            uint8 enable,
                                            uint8 depth)
{
  uint8 read_value = 0;
  uint8 write_value = 0;
  uint8 phy_reg = 0x32;
  const uint8 enpre_mask = 0x20;
  const uint8 preemdepth_mask = 0x10;

  /* USB PHY Configuration 3/4 - ( Address = 0x32 )
       Bit 7:6 reserved
       Bit 5 enpre: Enables Pre-Emphasis.
       Bit 4 preemdepth: Pre-Emphasis Depth.
          ‘0’ = 10%
          ‘1’ = 20%
       Bit 3:0 hsdrvslope: HS Driver Slope Control.
  */
  
  if (HAL_HSUSB_Success != HAL_hsusb_ReadUlpiPhy(instance, port, phy_reg, 
                                                 &read_value))
  {
    return FALSE;
  }

  write_value = read_value;
  write_value |= (enable << 5) & enpre_mask;      /* update enable bit */
  write_value |= (depth  << 4) & preemdepth_mask; /* update depth bit  */
  
  if (HAL_HSUSB_Success != HAL_hsusb_WriteUlpiPhy(instance, port, phy_reg, 
                                                  write_value))
  {
    return FALSE;
  }

  return TRUE;
}
#endif  /* T_MDM8200 || T_MSM7625 || T_MDM8900 */
#endif  /* FEATURE_HS_USB_INTEGRATED_PHY */
#endif  /* BUILD_HOSTDL */


/*===========================================================================

FUNCTION      HSU_API_ENABLE_PMIC_OTG_COMPARATORS

DESCRIPTION
  Turn on the PMIC OTG comparators. This is for detecting USB cable 
  connection events during LPM. 

  The goal of this function is to replace the HS-USB PHY's comparators with 
  the PMIC OTG comparators in targets which use an integrated HS-USB PHY. 
  
  This is because the PMIC OTG comparators are more power saving than the PHY's
  comparators (the PHY's comparators consume ~80 uA, while the PMIC OTG 
  comparators consume ~1.6 uA).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE\FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_api_enable_pmic_otg_comparators(void)
{
  #if !(defined FEATURE_DONT_USE_PMIC_OTG_COMPARATORS) && \
  defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_BASIC)

  pm_err_flag_type  pm_result;

  if (mdm_apis_pmic_comparators_enabled)
  {
    HSU_MSG_HIGH("hsu_api_enable_pmic_otg_comparators: Already enabled",0,0,0);
    return TRUE;
  }

  mdm_apis_pmic_comparators_enabled = TRUE;
  do /* A do .. while(0) loop for handling errors. */
  {
    /* Register for handling B-Session Valid rising PMIC interrupts.
    ** This interrupt indicates that the HS-USB port was connected to 
    ** a USB host.
    */
    pm_result = 
      pm_set_irq_multi_handle(PM_USB_SN_VALID_RE_IRQ_HDL,
                              hsu_api_pmic_otg_isr,
                              NULL);

    if (pm_result != PM_ERR_FLAG__SUCCESS)
    {
      HSU_MSG_ERROR("hsu_api_enable_pmic_otg_comparators: Failed to register "
                    "for PMIC OTG B-Session Valid rising interrupts. "
                    "pm_result: %d",
                    pm_result, 0, 0);

      break;
    }

    /* On Raptor2, PMIC interrupts are not used when the cable is connected. */
    #ifndef T_MSM8650
    /* Register for handling B-Session Valid falling PMIC interrupts.
    ** This interrupt indicates that the HS-USB port was disconnected 
    ** from the USB host.
    */
    pm_result = 
      pm_set_irq_multi_handle(PM_USB_SN_VALID_FE_IRQ_HDL,
                              hsu_api_pmic_otg_isr,
                              NULL);

    if (pm_result != PM_ERR_FLAG__SUCCESS)
    {
      HSU_MSG_ERROR("hsu_api_enable_pmic_otg_comparators: Failed to register "
                    "for PMIC OTG B-Session Valid Falling interrupts. "
                    "pm_result: %d",
                    pm_result, 0, 0);

      break;
    }
    #endif  /* !T_MSM8650 */

    /* LCU HAN PMIC doesn't support host mode detection over the ID pin.*/
    #ifndef T_QSC6270

    /* On Raptor2, host mode is not available when PMIC comparators are used. */
    #ifndef T_MSM8650

    /* Register for handling ID Grounded PMIC interrupts.
    ** This interrupt indicates that a device has been connected to
    ** the HS-USB port.
    */
    pm_result = 
      pm_set_irq_multi_handle(PM_USB_ID_GND_RE_IRQ_HDL,
                              hsu_api_pmic_otg_isr,
                              NULL);

    if (pm_result != PM_ERR_FLAG__SUCCESS)
    {
      HSU_MSG_ERROR("hsu_api_enable_pmic_otg_comparators: Failed to register "
                    "for PMIC OTG ID Grounded interrupts. "
                    "pm_result: %d",
                    pm_result, 0, 0);

      break;
    }

    /* Register for handling ID Grounded falling edege PMIC interrupts.
    ** This interrupt indicates that a device has been disconnected from
    ** the HS-USB port.
    */
    pm_result = 
      pm_set_irq_multi_handle(PM_USB_ID_GND_FE_IRQ_HDL,
                              hsu_api_pmic_otg_isr,
                              NULL);

    if (pm_result != PM_ERR_FLAG__SUCCESS)
    {
      HSU_MSG_ERROR("hsu_api_enable_pmic_otg_comparators: Failed to register "
                    "for PMIC OTG ID Grounded interrupts. "
                    "pm_result: %d",
                    pm_result, 0, 0);

      break;
    }
    #endif  /* !T_MSM8650 */

    pm_result = 
      pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND);

    if (pm_result != PM_ERR_FLAG__SUCCESS)
    {
      HSU_MSG_ERROR("hsu_api_enable_pmic_otg_comparators: Failed to put "
                    "the PMIC OTG transceiver in suspend mode. "
                    "pm_result: %d",
                    pm_result, 0, 0);

      break;
    }
    #else /* T_QSC6270 */

    if (pm_usb_vbus_sess_comp_en_set(TRUE) != PM_ERR_FLAG__SUCCESS)
    {
      HSU_MSG_ERROR("hsu_api_enable_pmic_otg_comparators: "
        "Failed to enable B Session Valid comparator",
        0, 0, 0);
      HSU_ASSERT(FALSE);
    }

    #endif  /* !T_QSC6270 */  
    return TRUE;

  } while (0);
  
  /* Error handling and rollback section. */
  (void)pm_set_irq_multi_handle(PM_USB_SN_VALID_RE_IRQ_HDL, NULL, NULL);
  (void)pm_set_irq_multi_handle(PM_USB_SN_VALID_FE_IRQ_HDL, NULL, NULL);
  (void)pm_set_irq_multi_handle(PM_USB_ID_GND_RE_IRQ_HDL,   NULL, NULL);
  (void)pm_set_irq_multi_handle(PM_USB_ID_GND_FE_IRQ_HDL,   NULL, NULL);
  (void)pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SHUTDOWN);
  
  return FALSE;
  

  #else
  return TRUE;
  #endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS &..FEATURE_HS_USB_BASIC */
}  /* hsu_api_enable_pmic_otg_comparators */


/*===========================================================================

FUNCTION      HSU_API_DISABLE_PMIC_OTG_COMPARATORS

DESCRIPTION
  Turn off the PMIC OTG comparators. (See hsu_api_enable_pmic_otg_comparators()
  for more details on PMIC comparators)

  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE\FALSE.

SIDE EFFECTS
  None.
===========================================================================*/

boolean hsu_api_disable_pmic_otg_comparators(void)
{
  #if !(defined FEATURE_DONT_USE_PMIC_OTG_COMPARATORS) && \
  defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_BASIC)

  pm_err_flag_type  pm_result;

  if (!mdm_apis_pmic_comparators_enabled)
  {
    HSU_MSG_HIGH("hsu_api_disable_pmic_otg_comparators: Already disabled",0,0,0);
    return TRUE;
  }

  mdm_apis_pmic_comparators_enabled = FALSE;

  /* Un-register for handling B-Session Valid PMIC interrupts.
  ** This interrupt indicates that the HS-USB port was connected to 
  ** a USB host.
  */
  pm_result = 
    pm_set_irq_multi_handle(PM_USB_SN_VALID_RE_IRQ_HDL, NULL, NULL);

  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_disable_pmic_otg_comparators: Failed to un-register "
                  "PMIC OTG B-Session Valid rising interrupts. "
                  "pm_result: %d",
                  pm_result, 0, 0);

    return FALSE;
  }

  /* Un-register for handling B-Session Valid PMIC interrupts.
  ** This interrupt indicates that the HS-USB port was disconnected 
  ** from the USB host.
  */
  pm_result = 
    pm_set_irq_multi_handle(PM_USB_SN_VALID_FE_IRQ_HDL, NULL, NULL);

  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_disable_pmic_otg_comparators: Failed to un-register "
                  "from PMIC OTG B-Session Valid Falling interrupts. "
                  "pm_result: %d",
                  pm_result, 0, 0);

    return FALSE;
  }

  /* Register for handling ID Grounded PMIC interrupts.
  ** This interrupt indicates that a device has been connected to
  ** the HS-USB port.
  */
  pm_result = 
    pm_set_irq_multi_handle(PM_USB_ID_GND_RE_IRQ_HDL, NULL, NULL);

  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_disable_pmic_otg_comparators: Failed to register "
                  "from PMIC OTG ID Grounded interrupts. "
                  "pm_result: %d",
                  pm_result, 0, 0);

    return FALSE;
  }

  /* Register for handling ID Grounded falling edege PMIC interrupts.
  ** This interrupt indicates that a device has been disconnected from
  ** the HS-USB port.
  */
  pm_result = 
    pm_set_irq_multi_handle(PM_USB_ID_GND_FE_IRQ_HDL, NULL, NULL);

  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_disable_pmic_otg_comparators: Failed to register "
                  "from PMIC OTG ID Grounded interrupts. "
                  "pm_result: %d",
                  pm_result, 0, 0);

    return FALSE;
  }

  pm_result = pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SHUTDOWN);
  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_disable_pmic_otg_comparators: Failed shutdown "
                  "the PMIC OTG transceiver. "
                  "pm_result: %d",
                  pm_result, 0, 0);

    return FALSE;
  }
 
  #endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS &.. FEATURE_HS_USB_BASIC*/
  return TRUE;
}



/*==============================================================================

FUNCTION          HSU_API_INIT_PHY

DESCRIPTION
  TODO

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None. 

==============================================================================*/

#if defined(T_MDM8200) || defined(T_MDM8900)
/* Mustang or Skywalker */
void hsu_api_init_phy(void)
{
#ifndef BUILD_HOSTDL
  boolean ret = TRUE;

  /* HS-USB HAL must be initialized before using HAL_hsusb_ API */
  HAL_hsusb_Init();

  /* Force the link to be in reset until the PHY reset is done */
  MSM_SET( APPS_RESET, USBH );

  /* Assert reset signal for phy */
  hsu_assert_phy_reset();

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  /* De-assert reset signal to the phy while asserting reset to the link */
  hsu_deassert_phy_reset();

  MSM_CLEAR( APPS_RESET, USBH );

  /* Cause the STP signal to be de-asserted, so that ULPI mode would be possible.
      Due to a HW bug, setting the ULPI_MX_NOM_STOP_CTRL bit actually 
      de-asserts the STP signal. */
  MSM_SET( ULPI_TEST_CTL, ULPI_MX_NOM_STOP_CTRL ); 

  /* set core to ULPI mode */
  HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);

  /* Perform integrated PHY recalibration */
  ret = hsu_recal_ulpi_phy(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Verify ULPI PHY register access */
  ret = hsu_verify_ulpi_phy_access(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Enable Pre-Emphasis with 20% depth */
  ret = hsu_set_ulpi_phy_preemphasis(0, 0, 1, 1);
  HSU_ASSERT(TRUE == ret);

  /* Reset the core in order to reconfig the PHY to pull-up the ID pin again. 
     This is because the PHY's ID pin pull-up state after reset is inactive. */
  hsu_soft_reset_link_controller(0, 0);
 
#endif /* !BUILD_HOSTDL */

} /* hsu_api_init_phy */

/* Since T_MSM7600 is also defined for MSM7800 and MSM7625 we need to include the code
   for Serra and sRoc before the code for Roc.  
   Raptor2 (MSM8650B) is very close to Serra in terms of HS-USB. 
   Note that Raptor2 also defines T_MSM7600. */
#elif defined(T_MSM7800)  || defined(T_MSM8650B)
/* Serra (MDM7800) */
void hsu_api_init_phy(void)
{
#ifndef BUILD_HOSTDL

  boolean ret = FALSE;

#if defined(T_FFA)
#error code not present
#endif /* T_FFA */

  HAL_hsusb_Init();

  /* set core to ULPI mode */
  HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);

  /* Force the link to be in reset until the PHY reset is done */
  MSM_SET( APPS_RESET, USBH );

  /* Assert reset signal for phy */
  hsu_assert_phy_reset();

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  /* De-assert reset signal to the phy while asserting reset to the link */
  hsu_deassert_phy_reset();

  MSM_CLEAR( APPS_RESET, USBH );

  /* Deassert LINK_TEST_EN to use integrated PHY instead of external one */
  MSM_CLEAR( ULPI_MODE_CONFIG, LINK_TEST_EN ); 

  /* set core to ULPI mode - after reseting the USBH we may need to set the 
     mode again*/
  HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);

  /* Perform integrated PHY recalibration */
  ret = hsu_recal_ulpi_phy(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Verify ULPI PHY register access */
  ret = hsu_verify_ulpi_phy_access(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Reset the core in order to reconfig the PHY to pull-up the ID pin again. 
     This is because the PHY's ID pin pull-up state after reset is inactive. */
  hsu_soft_reset_link_controller(0, 0);

#endif /* BUILD_HOSTDL */

#if !defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_OTG)
#error code not present
#endif /* !defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_OTG) */

#ifdef T_MSM8650
#ifndef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
#ifndef FEATURE_HS_USB_BASIC

  if (hsu_api_use_pmic_otg_comparators()) 
  {
    boolean result;

    result = hsu_api_enable_pmic_otg_comparators();

    if (result != TRUE)
    {
      HSU_MSG_ERROR("hsu_api_init_phy: "
        "Failed to enable the PMIC OTG comparators.",
        0, 0, 0);

      HSU_ASSERT(result == TRUE);
    }
  }
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */
#endif  /* T_MSM8650 */

} /* hsu_api_init_phy */
#elif defined(T_MSM7625)
/* sRoc */
void hsu_api_init_phy(void)
{
#ifndef BUILD_HOSTDL
  boolean ret = FALSE;

#if defined(T_FFA)
#error code not present
#endif /* T_FFA */

  /* Force the link to be in reset until the PHY reset is done */
  MSM_SET( APPS_RESET, USBH );

  /* Assert reset signal for phy */
  hsu_assert_phy_reset();

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  /* De-assert reset signal to the phy while asserting reset to the link */
  hsu_deassert_phy_reset();

  MSM_CLEAR( APPS_RESET, USBH );

  /* Cause the STP signal to be de-asserted, so that ULPI mode would be possible.
  Due to a HW bug, setting the ULPI_MX_NOM_STOP_CTRL bit actually 
  de-asserts the STP signal. */
  MSM_SET( ULPI_TEST_CTL, ULPI_MX_NOM_STOP_CTRL ); 

  HAL_hsusb_Init();

  /* set core to ULPI mode - after reseting the USBH we may need to set the mode again*/
  HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);

  /* Perform integrated PHY recalibration */
  ret = hsu_recal_ulpi_phy(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Verify ULPI PHY register access */
  ret = hsu_verify_ulpi_phy_access(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Enable Pre-Emphasis with 20% depth */
  ret = hsu_set_ulpi_phy_preemphasis(0, 0, 1, 1);
  HSU_ASSERT(TRUE == ret);

  /* Reset the core in order to reconfig the PHY to pull-up the ID pin again. 
     This is because the PHY's ID pin pull-up state after reset is inactive. */
  hsu_soft_reset_link_controller(0, 0);

#endif /* !BUILD_HOSTDL */

#ifndef FEATURE_HS_USB_BASIC
  vbus_is_on = TRUE; /* fake this, so that we indeed order VBUS power down */
  hsu_api_vbus_shutdown(); 
#endif /* ! FEATURE_HS_USB_BASIC */

} /* hsu_api_init_phy */

#elif defined(T_MSM7600)
#error code not present
#elif defined(T_MSM6246) || defined(T_MSM6290)

   /* Koel and Hawk */
void hsu_api_init_phy(void)
{
#ifndef BUILD_HOSTDL
  pm_err_flag_type pm_ret_val;
  boolean ret = FALSE;

  /* Config the PHY 3.3V rail to source from VREG_5V.
  ** Without this change, there is no power source for
  ** the PHY 3.3V power input. This causes the pull-up
  ** on the ID pin to be lost. Then, the ID pin starts
  ** floating and generate spurious interupts.
  */
  pm_ret_val = pm_otg_pwr_sel(PM_OTG_PWR_SEL__BOOST);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  /* Request from PMIC 3.3 V for VREG_USB */
  pm_ret_val = pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  pm_ret_val = pm_vreg_set_level(PM_VREG_USB_ID, 3300);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);  

  MSM_LOCK();

  /* Force the link to be in reset until the PHY reset is done */
  clk_regime_msm_reset_assert(CLK_RGM_USB_OTG_1_M);
  clk_regime_msm_reset_assert(CLK_RGM_USB_PHY_RESET_M);

  MSM_UNLOCK();

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  MSM_LOCK();

  clk_regime_msm_reset_deassert(CLK_RGM_USB_PHY_RESET_M);
  clk_regime_msm_reset_deassert(CLK_RGM_USB_OTG_1_M);
  
  MSM_UNLOCK();


  HAL_hsusb_Init();

  /***********************************************************************/
  /* KOEL BRING-UP FIX #2: USB_1_SW_RESET reset all link register settings
                          to default.  Putting link to ULPI mode has to
                          be done after USB_1_SW_RESET reset.            */
  /* set core to ULPI mode */
  HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);
  /***********************************************************************/

  MSM_LOCK();

  /* De-assert ulpi_mx_nom_stop_ctrl signal */
  /* @TODO-integrated PHY - This code will not work for multi-core. 
  May need to check the controller index or do the actions in HSU adaptation layer if possible*/ 
  MSM_CLEAR( ULPI_MUX_CONFIG, STOP_CTRL );

  MSM_UNLOCK();

  /* Perform integrated PHY recalibration */
  ret = hsu_recal_ulpi_phy(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Verify ULPI PHY register access */
  ret = hsu_verify_ulpi_phy_access(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Reset the core in order to reconfig the PHY to pull-up the ID pin again. 
     This is because the PHY's ID pin pull-up state after reset is inactive. */
  hsu_soft_reset_link_controller(0, 0);
 
#endif /* BUILD_HOSTDL */

#ifndef FEATURE_HS_USB_BASIC
  vbus_is_on = TRUE; /* fake this, so that we indeed order VBUS power down */
  hsu_api_vbus_shutdown(); 

#ifndef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
#ifndef FEATURE_HS_USB_BASIC
  {
    boolean result;

    result = hsu_api_enable_pmic_otg_comparators();

    if (result != TRUE)
    {
      HSU_MSG_ERROR("hsu_api_init_phy: "
                    "Failed to enable the PMIC OTG comparators.",
                    0, 0, 0);

      HSU_ASSERT(result == TRUE);
    }
  }
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */

#elif !defined(BUILD_HOSTDL)
  {
    pm_err_flag_type pm_ret_val;
    pm_ret_val = pm_otg_pwr_sel(PM_OTG_PWR_SEL__VBUS);
    HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);
  }
#endif /* ! FEATURE_HS_USB_BASIC */
} /* hsu_api_init_phy */

#elif defined(T_QSC6270)

  /* LCU */
void hsu_api_init_phy(void)
{
#ifndef BUILD_HOSTDL
  pm_err_flag_type pm_ret_val;
  boolean ret = FALSE;

  /* Config the PHY 3.3V rail to source from VREG_5V.
  ** Without this change, there is no power source for
  ** the PHY 3.3V power input. This causes the pull-up
  ** on the ID pin to be lost. Then, the ID pin starts
  ** floating and generate spurious interrupts.
  */
  pm_ret_val = pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VREG5V);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  /* Request from PMIC 3.3 V for VREG_USB */
  pm_ret_val = pm_vreg_set_level(PM_VREG_USB3P3_ID, 3300);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);    

  pm_ret_val = pm_vreg_control(PM_ON_CMD, PM_VREG_USB3P3_M);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  /* Configure VREG_5V to be the USB 3.3V LDO's power source.  */
  pm_ret_val = pm_usb_vbus_control_set(PM_USB_VBUS_CONTROL_VREG5V);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  /* Turn on USB_LDO_EN for 3.3V via PMIC BUS_CTRL_1 register */
  pm_ret_val = pm_usb_vbus_ldo_en_set(TRUE);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  /* Turn off VBUS_CHG via PMIC BUS_CTRL_1 register */
  pm_ret_val = pm_usb_vbus_chg_set(FALSE);
  HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

  /* Force the link to be in reset until the PHY reset is done */
  clk_regime_msm_reset_assert(CLK_RGM_USB_M);

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  clk_regime_msm_reset_deassert(CLK_RGM_USB_M);

  HAL_hsusb_Init();

  /* set core to ULPI mode */
  HAL_hsusb_Write32(0, HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PTS_ULPI_BMSK);
  
  /* De-assert ulpi_mx_nom_stop_ctrl signal */
  /* @TODO-integrated PHY - This code will not work for multi-core. 
  May need to check the controller index or do the actions in HSU adaptation 
  layer if possible */

    MSM_CLEAR( USB_CHARGE_DET, USB_PHY_STOP_HIGH );

  /* Perform integrated PHY recalibration */
  ret = hsu_recal_ulpi_phy(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Verify ULPI PHY register access */
  ret = hsu_verify_ulpi_phy_access(0, 0);
  HSU_ASSERT(TRUE == ret);

  /* Reset the core in order to reconfig the PHY to pull-up the ID pin again. 
     This is because the PHY's ID pin pull-up state after reset is inactive. */
  hsu_soft_reset_link_controller(0, 0);

#endif /* BUILD_HOSTDL */

#ifndef FEATURE_HS_USB_BASIC
  vbus_is_on = TRUE; /* fake this, so that we indeed order VBUS power down */
  hsu_api_vbus_shutdown(); 
  if (pm_usb_vbus_comp_en_set(FALSE) != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_init_phy: "
      "Failed to disable VBUS Valid comparator",
      0, 0, 0);
    HSU_ASSERT(FALSE);
  }

  if (hsu_api_use_pmic_otg_comparators()) 
  {
    boolean result;

    result = hsu_api_enable_pmic_otg_comparators();

    if (result != TRUE)
    {
      HSU_MSG_ERROR("hsu_api_init_phy: "
        "Failed to enable the PMIC OTG comparators.",
        0, 0, 0);

      HSU_ASSERT(result == TRUE);
    }
   }
#elif !defined(BUILD_HOSTDL)
  /* TODO : configure VBUS power select */
#endif
} /* hsu_api_init_phy */

#elif defined(T_QSC1100) || defined(T_QSC1110)
void hsu_api_init_phy(void)
{
#ifndef BUILD_HOSTDL
  pm_err_flag_type ret = PM_ERR_FLAG__SUCCESS;

#ifndef FEATURE_HS_USB_BASIC
  HSU_USE_PARAM(vbus_is_on);
#endif /* !FEATURE_HS_USB_BASIC */

  /* turn on VREG_USB */
  ret = pm_vreg_set_level( PM_VREG_USB3P3_ID, PM_VREG_USB_LEVEL);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);
  
  ret = pm_vreg_control( PM_ON_CMD, PM_VREG_USB3P3_M);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);
  
  /* TODO : MSM_SET and MSM_CLEAR will be used after they are defined
       in msmhwio.h*/
  /* assert USB reset */
  HWIO_SW_RESET_REG_OUTM(HWIO_SW_RESET_REG_USB_RESET_BMSK, 
                         HWIO_SW_RESET_REG_USB_RESET_BMSK);

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  hsu_delay_ms(1);

  /* de-assert USB reset */
  HWIO_SW_RESET_REG_OUTM(HWIO_SW_RESET_REG_USB_RESET_BMSK,
                         0);

  #if defined(FEATURE_HS_USB_BASIC) || !defined(FEATURE_HS_USB_PMIC_PHY)
  /* activate the OTG xcvr */
  ret = pm_otg_config_power_management( PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* pull down D+/- low initially to start off from a USB disconnect state */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* reset OTG transceiver's mode */
  ret = pm_otg_reset_mode();
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* config OTG xcvr for USB B-dev mode */
  ret = pm_otg_config_usb_mode( PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* pull D+ high to indicate to PC that the phone is ready for USB traffic */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE,
    PM_OTG_DP_CONFIG_CMD__PULL_UP,
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);
  #endif /* FEATURE_HS_USB_BASIC || !FEATURE_HS_USB_PMIC_PHY */

#endif /* !BUILD_HOSTDL */

} /* hsu_api_init_phy */
#endif /* T_QSC1100 */

#ifdef FEATURE_HS_USB_PMIC_PHY
/*===========================================================================

FUNCTION      hsu_api_spoof_reconnect

DESCRIPTION   
    This function simulates usb cable detach and attach back. 

DEPENDENCIES
   This function is valid only when PMIC PHY works. ex) QSC1100.

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_spoof_reconnect(void)
{
#ifndef BUILD_HOSTDL
  pm_err_flag_type ret = PM_ERR_FLAG__SUCCESS;

  /* Pull down D+/D- to make sure fast transtion of line status */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DM_CONFIG_CMD__PULL_DOWN, 
    PM_OTG_DM_CONFIG_CMD__PULL_DOWN, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* Wait for host to detect the disconnect */
  hsu_delay_ms(3);


  /* Pull up D+ */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DM_CONFIG_CMD__PULL_UP, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);
#endif /* ! BUILD_HOSTDL */
}
#endif /* FEATURE_HS_USB_PMIC_PHY */

/*===========================================================================

FUNCTION      hsu_api_init_uicc

DESCRIPTION   
    Initialize the USB UICC core and GPIO's

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
#if (defined(T_MSM6246) || defined(T_MSM6290))
/* Koel & Hawk */
void hsu_api_init_uicc(void)
{
#if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))

  /* Turn on USB UICC core clocks. */
  clk_regime_enable (CLK_RGM_USB_OTG_2_M);
  clk_regime_enable (CLK_RGM_USB_MUSIM_M);

  /* Power off VCC */
  pm_vreg_control(PM_OFF_CMD, PM_VREG_RUIM_M);

  /* Enabling the two wire interface for USB UICC by setting bit 27 of    */
  /* USBCMD. We must set this bit after UICC core is initialized to avoid */
  /* false detection of port connect. In addition, Jungo code must not    */
  /* reset this bit when the stack is started from the same reason.       */
  HAL_hsusb_SetFs3Wire2WireSelect(1, TRUE);

  /* set core to Serial mode, and force full speed */
  HAL_hsusb_Write32(1, HAL_HSUSB_PORTSC_ADDR(0), 
    HAL_HSUSB_PORTSC_PTS_SERIAL_BMSK | HAL_HSUSB_PORTSC_PFSC_BMSK);

  /* Set the power UP pin */
#ifdef T_FFA
#error code not present
#else
  gpio_out(GPIO_OUTP_89, GPIO_HIGH_VALUE);
#endif /* T_FFA */

  /* Disabling UIM clock and UIM data function for GPIO88 and GPIO91 */
  gpio_tlmm_config(GPIO_OUTP_91);
  gpio_tlmm_config(GPIO_INP_88);  

  /* Enabling GPIO106 and GPIO107 (D+ & D- on USB UICC) with pull down enabled */
  gpio_tlmm_config(MUSIM_FS_SEO_PD);
  gpio_tlmm_config(MUSIM_FS_TX_EN_PD);

#endif /* FEATURE_HS_USB_UICC and !FEATURE_HS_USB_BASIC*/
} /* hsu_api_init_uicc */

#elif defined(T_QSC6270)
/* LCU */
void hsu_api_init_uicc(void)
{
#if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))

  /* Turn on USB UICC core clocks. */
  clk_regime_enable (CLK_RGM_MEGASIM_FS_M);
  clk_regime_enable (CLK_RGM_MEGASIM_HCLK_M);

  /* Power off VCC */
  pm_vreg_control(PM_OFF_CMD, PM_VREG_USIM_M);

  /* Enabling the two wire interface for USB UICC by setting bit 27 of    */
  /* USBCMD. We must set this bit after UICC core is initialized to avoid */
  /* false detection of port connect. In addition, Jungo code must not    */
  /* reset this bit when the stack is started from the same reason.       */
  HAL_hsusb_SetFs3Wire2WireSelect(1, TRUE);

  /* set core to Serial mode, and force full speed */
  HAL_hsusb_Write32(1, HAL_HSUSB_PORTSC_ADDR(0), 
    HAL_HSUSB_PORTSC_PTS_SERIAL_BMSK | HAL_HSUSB_PORTSC_PFSC_BMSK);

  /* Disabling UIM clock and UIM data */
  gpio_tlmm_config(GPIO_OUTP_46);
  gpio_tlmm_config(GPIO_INP_47);  

  /* Enabling D+ & D- on USB UICC with pull down enabled */
  gpio_tlmm_config(MUSIM_DM_PD);
  gpio_tlmm_config(MUSIM_DP_PD);

#endif /* FEATURE_HS_USB_UICC and !FEATURE_HS_USB_BASIC*/
} /* hsu_api_init_uicc */

#else /* Empty implementation for all targets without UICC support */
void hsu_api_init_uicc(void)
{
} /* hsu_api_init_uicc */
#endif /* !(T_MSM6246 || T_MSM6290 || T_QSC6270) */

/*===========================================================================

FUNCTION      hsu_api_start_uicc

DESCRIPTION   
    Start the USB UICC core and GPIO's

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_start_uicc(void)
{
#if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))

/* Request from PMIC the required voltage for UICC VCC */
#if (defined(T_MSM6246) || defined(T_MSM6290))
  /* Use VREG_UIM regulator */
  pm_vreg_set_level(PM_VREG_RUIM_ID, hsu_uicc_get_voltage_class());
  pm_vreg_control(PM_ON_CMD, PM_VREG_RUIM_M);
#elif defined(T_QSC6270)
  /* Use VREG_USIM regulator */
  /* The problem the MSM has with using the RUIM regulator for its pads is that 
     with no, or an unstable supply voltage, the output driver cannot guarantee 
     a low during power-up. The MSM cannot run with the power at 0V, but the 
     USIM must see 0V before it boots up. The USIM signals need to remain low 
     through system power-on and off. The proposed solution decouples the pad 
     and USIM power without adding an extra LDO. */
  pm_usim_aux_enable(PM_ON_CMD);
  pm_vreg_set_level(PM_VREG_USIM_ID, hsu_uicc_get_voltage_class());
  clk_busy_wait(HSU_UICC_PMIC_SETTLING_TIME_MS);
  pm_vreg_control(PM_ON_CMD, PM_VREG_USIM_M);
  clk_busy_wait(HSU_UICC_PMIC_SETTLING_TIME_MS);
  pm_usim_aux_vreg_set(PM_USIM_AUX_CTRL_VREG_RUIM);        
#endif /* defined(T_MSM6246 || T_MSM6290 || T_QSC6270) */

#endif /* FEATURE_HS_USB_UICC and !FEATURE_HS_USB_BASIC*/
} /* hsu_api_start_uicc */

/*===========================================================================

FUNCTION      hsu_api_disconnect_pulldowns_uicc

DESCRIPTION   
    Disconnect the pull downs on the D+ and D- of the USB UICC GPIOs

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_disconnect_pulldowns_uicc(void)
{
#if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))

/* Enabling D+ & D- on USB UICC with no pull down */
#if (defined(T_MSM6246) || defined(T_MSM6290))
  /* Use GPIO106 and GPIO107 (FS SE0 and FS Tx En) with no pull downs */
  gpio_tlmm_config(MUSIM_FS_SE0);
  gpio_tlmm_config(MUSIM_FS_TX_EN);
#elif defined(T_QSC6270)
  /* Use GPIO72 and GPIO73 configured with no pull downs */
  gpio_tlmm_config(MUSIM_DM);
  gpio_tlmm_config(MUSIM_DP);
#endif /* defined(T_MSM6246 || T_MSM6290 || T_QSC6270) */
#endif /* FEATURE_HS_USB_UICC and !FEATURE_HS_USB_BASIC*/
} /* hsu_api_disconnect_pulldowns_uicc */


/*===========================================================================

FUNCTION      hsu_api_polldown_uicc

DESCRIPTION   
    Pull down the D+ and D- of the USB UICC GPIOs

DEPENDENCIES
    FEATURE_HS_USB_UICC must be defined. 

RETURN VALUE
    None.

SIDE EFFECTS
    None.
===========================================================================*/
void hsu_api_polldown_uicc(void)
{
#if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC))
  hsu_api_disconnect_pulldowns_uicc();
#endif /* FEATURE_HS_USB_UICC and !FEATURE_HS_USB_BASIC*/
} /* hsu_api_polldown_uicc */

/*===========================================================================

FUNCTION      HSU_API_VBUS_POWERUP

DESCRIPTION
  Turn on the 5V power supply on the VBUS pin (when the HS-USB stack 
  enters host mode).

DEPENDENCIES
  FEATURE_HS_USB must be defined. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#if defined(T_MDM8200) || defined(T_MDM8900)
  /* Mustang or Skywalker */
void hsu_api_vbus_powerup(void)
{
  /* not yet implemented */
}
#elif defined(T_MSM8650B) /* Raptor2*/
void hsu_api_vbus_powerup(void)
{
#ifndef FEATURE_HS_USB_BASIC
  pm_err_flag_type pm_result;
  if(vbus_is_on == FALSE)
  {
    /* Vote to turn on VREG_5V in the PMIC */
    pm_vote_vreg_switch(PM_ON_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);

#ifdef T_FFA
#error code not present
#else  /* T_SURF */
    pm_result = pm_mpp_config_digital_output(HSU_MPP_FOR_VBUS,
      PM_MPP__DLOGIC__LVL_VDD, 
      PM_MPP__DLOGIC_OUT__CTRL_HIGH);
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
#endif  /* T_FFA */

    vbus_is_on = TRUE;
  }
#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_powerup */

#elif defined(T_MSM7600) || defined(T_MSM7625)/* Roc or sRoc*/
void hsu_api_vbus_powerup(void)
{
#ifndef FEATURE_HS_USB_BASIC
#ifndef T_GOBI2000
  pm_err_flag_type pm_result;
#endif /*T_GOBI2000*/
  if(vbus_is_on == FALSE)
  {
#ifndef T_GOBI2000
    pm_result = pm_mpp_config_digital_output(HSU_MPP_FOR_VBUS,
      PM_MPP__DLOGIC__LVL_VDD, 
      PM_MPP__DLOGIC_OUT__CTRL_HIGH);
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);

    /* Vote to turn on VREG_5V in the PMIC */
    pm_vote_vreg_switch(PM_ON_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);
#endif /*FEATURE_HS_USB_OTG*/
    vbus_is_on = TRUE;
  }
#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_powerup */
#elif defined(T_MSM6246) || defined(T_MSM6290) /* Koel && Hawk */

void hsu_api_vbus_powerup(void)
{
#ifndef FEATURE_HS_USB_BASIC

  pm_err_flag_type pm_result;

  if(vbus_is_on == FALSE)
  {
    /* Vote to turn on VREG_5V in the PMIC */
    pm_vote_vreg_switch(PM_ON_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);

    pm_result = pm_vreg_control ( PM_ON_CMD, PM_VREG_USB_M );
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
    
    pm_result = pm_otg_config_usb_mode (PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);

    pm_result = pm_otg_config_power_management ( PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE );
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
    
    pm_result = pm_otg_config_interface
                 (
                   PM_OTG_VBUS_CONFIG_CMD__DRV,
                   PM_OTG_DP_CONFIG_CMD__NONE,
                   PM_OTG_DM_CONFIG_CMD__NONE,
                   PM_OTG_ID_CONFIG_CMD__NONE
                 );
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
 
    vbus_is_on = TRUE;
  }

#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_powerup */

#elif defined(T_QSC6270)

void hsu_api_vbus_powerup(void)
{
#ifndef FEATURE_HS_USB_BASIC

  pm_err_flag_type pm_ret_val;

  if(vbus_is_on == FALSE)
  {  
    /* Vote to turn on VREG_5V in the PMIC */
    pm_vote_vreg_switch(PM_ON_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);

    pm_ret_val = pm_usb_vbus_drv_set(TRUE);
    HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

    vbus_is_on = TRUE;
  }

#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_powerup */

#elif defined(T_QSC1100)
void hsu_api_vbus_powerup(void)
{
  /* TODO-ULC: Implement */

} /* hsu_api_vbus_powerup */
#endif /* T_QSC1100 */

/*===========================================================================

FUNCTION      HSU_API_VBUS_SHUTDOWN

DESCRIPTION
  Turn off the 5V power supply on the VBUS pin (when the HS-USB stack 
  exits host mode).

DEPENDENCIES
  FEATURE_HS_USB must be defined. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#if defined(T_MDM8200) || defined(T_MDM8900)
  /* Mustang or Skywalker */
void hsu_api_vbus_shutdown(void)
{
  /* Not yet implemented */
}
#elif defined(T_MSM8650B) /* Raptor2*/
void hsu_api_vbus_shutdown(void)
{
#ifndef FEATURE_HS_USB_BASIC
  pm_err_flag_type pm_result;
  if(vbus_is_on == TRUE)
  {
#ifdef T_FFA
#error code not present
#else  /* T_SURF */
    pm_result = pm_mpp_config_digital_output(HSU_MPP_FOR_VBUS,
      PM_MPP__DLOGIC__LVL_VDD, 
      PM_MPP__DLOGIC_OUT__CTRL_LOW);
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
#endif  /* T_FFA */


    /* Vote to turn off VREG_5V only if it is not needed for the LDO */
    pm_vote_vreg_switch(PM_OFF_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);

    vbus_is_on = FALSE;
  }
#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_shutdown */

#elif defined(T_MSM7600)|| defined (T_MSM7625) /* Roc or sRoc*/
void hsu_api_vbus_shutdown(void)
{
#ifndef FEATURE_HS_USB_BASIC
  pm_err_flag_type pm_result;
  if(vbus_is_on == TRUE)
  {
#ifndef T_GOBI2000
    #ifndef FEATURE_HS_USB_PHY_DEPENDS_ON_VREG_5V
    /* Vote to turn off VREG_5V only if it is not needed for the LDO */
    pm_vote_vreg_switch(PM_OFF_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);
    #endif

    pm_result = pm_mpp_config_digital_output(HSU_MPP_FOR_VBUS,
      PM_MPP__DLOGIC__LVL_VDD, 
      PM_MPP__DLOGIC_OUT__CTRL_LOW);
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
#endif /* T_GOBI2000 */
    vbus_is_on = FALSE;
  }
#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_shutdown */

#elif defined(T_MSM6246) || defined(T_MSM6290) /* Koel && Hawk */

void hsu_api_vbus_shutdown(void)
{
#ifndef FEATURE_HS_USB_BASIC

  pm_err_flag_type pm_result;

  if(vbus_is_on == TRUE)
  {
    pm_result = pm_otg_config_interface
                (
                   PM_OTG_VBUS_CONFIG_CMD__DISABLE_DRV,
                   PM_OTG_DP_CONFIG_CMD__NONE,
                   PM_OTG_DM_CONFIG_CMD__NONE,
                   PM_OTG_ID_CONFIG_CMD__NONE
                );
    HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);

    #ifndef FEATURE_HS_USB_PHY_DEPENDS_ON_VREG_5V
    /* Vote to turn off VREG_5V only if it is not needed for the LDO */
    pm_vote_vreg_switch(PM_OFF_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);
    #endif

    vbus_is_on = FALSE;
  }
#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_shutdown */

#elif defined(T_QSC6270)

void hsu_api_vbus_shutdown(void)
{
#ifndef FEATURE_HS_USB_BASIC

  pm_err_flag_type pm_ret_val;

  if(vbus_is_on == TRUE)
  {
    pm_ret_val = pm_usb_vbus_drv_set(FALSE);
    HSU_ASSERT(pm_ret_val == PM_ERR_FLAG__SUCCESS);

    #ifndef FEATURE_HS_USB_PHY_DEPENDS_ON_VREG_5V
    /* Vote to turn off VREG_5V only if it is not needed for the LDO */
    pm_vote_vreg_switch(PM_OFF_CMD,
      PM_VREG_BOOST_ID,
      PM_VOTE_VREG_BOOST_APP__OTG);
    #endif

    vbus_is_on = FALSE;
  }
#endif /* FEATURE_HS_USB_BASIC */
} /* hsu_api_vbus_shutdown */

#elif defined(T_QSC1100)
void hsu_api_vbus_shutdown(void)
{
  /* TODO-ULC: Implement */

} /* hsu_api_vbus_shutdown */
#endif /* T_QSC1100 */


/*===========================================================================
FUNCTION HSU_API_TURN_OFF_VREG_USB_LDO

DESCRIPTION
  This function turns off the 3.3v power rail which goes from the PMIC
  to the HS-USB PHY.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hsu_api_turn_off_vreg_usb_ldo(void)

#if ( !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC) ) && \
                                                                        \
      ( defined(T_MSM6246) || defined(T_MSM6290) ||                     \
        defined(T_QSC6270) || defined(T_MSM8650) )  &&                  \
                                                                        \
      !defined(FEATURE_DONT_USE_PMIC_OTG_COMPARATORS)

{
  pm_err_flag_type  pm_ret_val;


  #ifndef T_QSC6270
  /* Turn off the HS-USB PHY 3.3V analog power rail. */
  pm_ret_val = pm_vreg_control(PM_OFF_CMD, PM_VREG_USB_M);
  #else
  /* disable USB LDO */
  pm_ret_val = pm_usb_vbus_ldo_en_set(FALSE);
  #endif /* !T_QSC6270 */

  if (pm_ret_val != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_turn_off_vreg_usb_ldo: "
                  "Failed to turn off the HS-USB PHY 3.3V power rail.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }
}  /* hsu_api_turn_off_vreg_usb_ldo */

#else

{

  /* An empty implementation defined when 
  ** PMIC comparators are not used.
  */

}  /* hsu_api_turn_off_vreg_usb_ldo */

#endif  /* ( !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC))    &&
             (T_MSM6246 || T_QSC6270 || T_MSM6290 || T_MSM8650) &&
             !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS )              */


/*===========================================================================
FUNCTION HSU_API_TURN_ON_VREG_USB_LDO

DESCRIPTION
  This function turns on the 3.3v power rail which goes from the PMIC
  to the HS-USB PHY.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void hsu_api_turn_on_vreg_usb_ldo(void)

#if ( !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC) ) && \
                                                                        \
      ( defined(T_MSM6246) || defined(T_MSM6290) ||                     \
        defined(T_QSC6270) || defined(T_MSM8650) )  &&                  \
                                                                        \
      ! defined(FEATURE_DONT_USE_PMIC_OTG_COMPARATORS)

{
  pm_err_flag_type  pm_ret_val;


  #ifndef T_QSC6270
  /* Turn on the HS-USB PHY 3.3V power rail. */
  pm_ret_val = pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
  #else
  /* enable USB LDO */
  pm_ret_val = pm_usb_vbus_ldo_en_set(TRUE);
  #endif /* !T_QSC6270 */

  if (pm_ret_val != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("hsu_api_turn_on_vreg_usb_ldo: "
                  "Failed to turn on the HS-USB PHY 3.3V power rail.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }
}  /* hsu_api_turn_on_vreg_usb_ldo */

#else

{

  /* An empty implementation defined when 
  ** PMIC comparators are not used.
  */

}  /* hsu_api_turn_on_vreg_usb_ldo */

#endif  /* ( !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC))    &&
             (T_MSM6246 || T_QSC6270 || T_MSM6290 || T_MSM8650) &&
             !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS )              */


#ifndef FEATURE_HS_USB_BASIC

/*===========================================================================

FUNCTION      HSU_API_UPDATE_PRODUCT_ID

DESCRIPTION
Updates the ARM9 product id variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_product_id(unsigned short product_id)
{
  hsu_modem_proc_services_set_product_id(product_id);
}

/*===========================================================================

FUNCTION      HSU_API_UPDATE_SERIAL_NUMBER

DESCRIPTION
Updates the ARM9 serial number variable.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_serial_number(const char* serial_number)
{
  hsu_modem_proc_services_set_serial_number(serial_number);
}

/*===========================================================================

FUNCTION      HSU_API_UPDATE_IS_SERIAL_NUMBER_NULL

DESCRIPTION
Updates the ARM9 whether the serial number is null.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_update_is_serial_number_null(boolean is_serial_number_null)
{
  hsu_modem_proc_services_set_is_serial_number_null(is_serial_number_null);
}



#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_NOT_AVAIL

DESCRIPTION
Notifies the charger module that it can no longer draw current.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_not_avail(void)
{
#ifdef FEATURE_HS_USB_CHARGING
  #ifdef FEATURE_HS_USB_CHG_REMOTE_API
  #else
  hsu_chg_invoke_not_avail_cb();
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
}
#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_IS_AVAIL

DESCRIPTION
Notifies the charger module that it can start drawing current.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_is_avail(uint32 amount_current_to_draw)
{
#ifndef FEATURE_HS_USB_CHARGING
  HSU_USE_PARAM(amount_current_to_draw);
#else /* FEATURE_HS_USB_CHARGING */
  #ifdef FEATURE_HS_USB_CHG_REMOTE_API
	  HSU_USE_PARAM(amount_current_to_draw);
  #else
  hsu_chg_invoke_avail_cb(amount_current_to_draw);
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* !FEATURE_HS_USB_CHARGING */
}
#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_DISCONNECTED

DESCRIPTION
Notifies the charger module that it is disconnected from host PC.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_disconnected(void)
{
#ifdef FEATURE_HS_USB_CHARGING
  #ifdef FEATURE_HS_USB_CHG_REMOTE_API
  #else
  hsu_chg_invoke_disconnected_cb();
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
}
#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION      HSU_API_NOTIF_CHARGER_CONNECTED

DESCRIPTION
Notifies the charger module that it is connected to host PC.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_notif_charger_connected(boolean is_wall_charger)
{
#ifndef FEATURE_HS_USB_CHARGING
  HSU_USE_PARAM(is_wall_charger);
#else /* FEATURE_HS_USB_CHARGING */
  #ifdef FEATURE_HS_USB_CHG_REMOTE_API
	  HSU_USE_PARAM(is_wall_charger);
  #else
  hsu_chg_invoke_connected_cb(is_wall_charger);
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* !FEATURE_HS_USB_CHARGING */
}
#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION      HSU_API_INIT_CHG

DESCRIPTION
Initializes the hsu_chg module

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_api_init_chg(void)
{
#ifdef FEATURE_HS_USB_CHARGING
  #ifdef FEATURE_HS_USB_CHG_REMOTE_API
  #else
  hsu_chg_init();
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
}
#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================

FUNCTION      HSU_API_CHG_IS_CONNECTED

DESCRIPTION
Returns whether charger module is connected.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Charger connected. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_api_chg_is_connected(void)
{
#ifndef FEATURE_HS_USB_CHARGING
  return FALSE;
#else /* FEATURE_HS_USB_CHARGING */
  #ifdef FEATURE_HS_USB_CHG_REMOTE_API
	  return FALSE;
  #else
  return hsu_chg_is_connected();
  #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* !FEATURE_HS_USB_CHARGING */
}
#endif /* !FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_API_USB_RESET_REWORK_INSTALLED

DESCRIPTION
Checks whether or not USB reset rework is installed.
The rework is for the purpose of properly exiting TCXO shutdown upon receiving 
USB Reset.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Rework is installed. FALSE - Otherwise.

SIDE EFFECTS
None. 

==============================================================================*/
boolean hsu_api_usb_reset_rework_installed
(
  void
)
{
#ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND

  const uint8 MPP15_REWORK_TEST_SEQ_LENGTH = 5;
  
  uint8 seq_index = 0;
  pm_mpp_dlogic_out_ctrl_type output_level = PM_MPP__DLOGIC_OUT__CTRL_LOW;
  GPIO_ValueType return_val_from_gpio_111;
  pm_err_flag_type pm_result;

  if (!tested_usb_reset_rework_is_installed)
  {
    usb_reset_rework_is_installed = TRUE;

    /* Configure GPIO111 as default input */
    gpio_tlmm_config(GPIO_IN_111);
    /* Delay to ensure GPIO is configured to default input, prior to reading 
    sequence. */
    hsu_delay_ms(1);

    /* Perform a sequence of 01010 and see if this is received by GPIO 111*/
    for (seq_index = 0 ; seq_index < MPP15_REWORK_TEST_SEQ_LENGTH ; ++seq_index)
    {
      pm_result = pm_mpp_config_digital_output(PM_MPP_15, 
        PM_MPP__DLOGIC__LVL_VDD, output_level);   
      HSU_ASSUME(pm_result == PM_ERR_FLAG__SUCCESS);
      /* Delay to make sure gpio_in will read the updated value, if rework is 
      installed. */
      hsu_delay_ms(1);

      return_val_from_gpio_111 = gpio_in(GPIO_IN_111);

      if (output_level != (pm_mpp_dlogic_out_ctrl_type)return_val_from_gpio_111)
      {
        usb_reset_rework_is_installed = FALSE;
        break;
      }

      if (output_level == PM_MPP__DLOGIC_OUT__CTRL_LOW)
      {
        output_level = PM_MPP__DLOGIC_OUT__CTRL_HIGH;
      }
      else
      {
        output_level = PM_MPP__DLOGIC_OUT__CTRL_LOW;
      }
    }

    /* Reconfigure GPIO111 back to ULPI data0 */
    gpio_tlmm_config(USBH_DATA_0);
    /* Delay to ensure GPIO is configured back to ULPI_DATA0 */
    hsu_delay_ms(1);
  }

  tested_usb_reset_rework_is_installed = TRUE;
  return usb_reset_rework_is_installed;
#else /*   FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */
  return FALSE;
#endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */
}

/*===========================================================================

FUNCTION hsu_mdm_usb_reset_rework_change_state_isr 

DESCRIPTION
Callback function executed by PMIC interrupt when registered.

The purpose of this function is to have the PMIC power-up / wakeup the MSM
due to level change on MPP15 - see hsu_api_enable_PMIC_ulpidata0_irq_handler() 
for more info.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
#ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
static void hsu_mdm_usb_reset_rework_change_state_isr(void)
{
  (void) pm_clear_irq(PM_MPP15_CHGED_ST_IRQ_HDL);
}
#endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

/*===========================================================================

FUNCTION su_api_enable_pmic_ulpidata0_irq_handler 

DESCRIPTION
Configure PMIC MPPxx to interrupt when ULPI Data(0)[D+] changes since 
this line is not monitored by the MSM during TCXO shutdown (sleep).
This work-around is required to support USB Reset [D+ D- both LOW] 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Registers an ISR to be invoked upon change in input value to MPP15
===========================================================================*/
void hsu_api_enable_pmic_ulpidata0_irq_handler(void)
{
#ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND

  pm_err_flag_type pm_result;

  HSU_ASSUME(tested_usb_reset_rework_is_installed);
  if (!usb_reset_rework_is_installed)
  {
    HSU_MSG_ERROR("*_enable_pmic_ulpidata0_*: invoked despite rework not installed.", 
      0, 0, 0);
    return;
  }

  /* In case this gets called repeatedly, do not register mpp irq_handle */
  if (usb_reset_rework_irq_handle_enabled) 
  {
    return;
  }

  pm_result = pm_clear_irq(PM_MPP15_CHGED_ST_IRQ_HDL);
  HSU_ASSUME(pm_result == PM_ERR_FLAG__SUCCESS);
  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("*_enable_pmic_ulpidata0_*: pm_clear_irq failed (%d)", 
      pm_result, 0, 0);
    return;
  }

  /* Config MPP15 to accept ULPI_DATA0 as input */
  pm_result = pm_mpp_config_digital_input(PM_MPP_15,
    PM_MPP__DLOGIC__LVL_MSME, 
    PM_MPP__DLOGIC_IN__DBUS_NONE);
  HSU_ASSUME(pm_result == PM_ERR_FLAG__SUCCESS);
  if (pm_result != PM_ERR_FLAG__SUCCESS)
  {
    HSU_MSG_ERROR("*_enable_pmic_ulpidata0_*: pm_mpp_config_digital_input failed (%d)", 
      pm_result, 0, 0);
    return;
  }

  usb_reset_rework_irq_handle_enabled = TRUE;

  /* Register MPP15 ISR */
  pm_result = pm_set_irq_handle(PM_MPP15_CHGED_ST_IRQ_HDL, 
    hsu_mdm_usb_reset_rework_change_state_isr);
  HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);

#endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */
}

/*===========================================================================

FUNCTION hsu_api_disable_pmic_ulpidata0_irq_handler 

DESCRIPTION
Disable PMIC MPPxx interrupt.
This is called when USB is no longer in LPM, hence ULPI Data(0)[D+] 
does not need to be monitored by the PMIC.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Unregisters the ISR from the corresponding enable function
===========================================================================*/
void hsu_api_disable_pmic_ulpidata0_irq_handler(void)
{
#ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND

  pm_err_flag_type pm_result;

  HSU_ASSUME(tested_usb_reset_rework_is_installed);
  if (!usb_reset_rework_is_installed)
  {
    HSU_MSG_ERROR("*_disable_pmic_ulpidata0_*: invoked despite rework not installed.", 
      0, 0, 0);
    return;
  }

  /* In case this gets called repeatedly during resume */
  if (!usb_reset_rework_irq_handle_enabled) 
  {
    return;
  }

  usb_reset_rework_irq_handle_enabled = FALSE;

  /* Unregister MPP15 ISR */
  pm_result = pm_set_irq_handle(PM_MPP15_CHGED_ST_IRQ_HDL, NULL);
  HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);

  /* Divert MPP 15 back to output */
  pm_result = pm_mpp_config_digital_output(PM_MPP_15, 
    PM_MPP__DLOGIC__LVL_MSME, PM_MPP__DLOGIC_OUT__CTRL_LOW);
  HSU_ASSERT(pm_result == PM_ERR_FLAG__SUCCESS);
#endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */
}
#endif /* FEATURE_HS_USB_BASIC */

/***************************************************************************/
/* code for the "dead battery" charging that called form the OEMSBL        */
/***************************************************************************/
/*===========================================================================
FUNCTION hsu_dload_set_usb_imax
DESCRIPTION
This function set the power limit register to the maximum current when
called from the bootloader 
INPUT PARAMETERS
NONE
RETURN VALUE
NONE
DEPENDENCIES
NONE
SIDE EFFECTS
NONE
===========================================================================*/
void hsu_dload_set_usb_imax(void)
{
#if (defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL))
  pm_chg_usb_charger_transistor_imaxsel(PM_CHG_UCHG_Q_IMAX_150mV_div_R);
#endif /* defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
}
/*===========================================================================
FUNCTION hsu_dload_enable_disable_core
DESCRIPTION
This function int the HSU PHY when called from the bootloader 
INPUT PARAMETERS
NONE
RETURN VALUE
NONE
DEPENDENCIES
NONE
SIDE EFFECTS
NONE
===========================================================================*/
void hsu_dload_enable_disable_core(boolean enable)
{
#if (defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL))
  if (enable)
  {
    /* Init the PHY */
    hsu_api_init_phy();

    /* asserting reset */
    HAL_hsusb_Write32(0, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RST_BMSK);
    /* waiting for reset to complete */
    while (HAL_hsusb_GetBits32(0, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RST_BMSK) != 0 );
    /* Device controller */
    HAL_hsusb_Write32(0, HAL_HSUSB_USBMODE_ADDR, HAL_HSUSB_USBMODE_CM_DEVICE_BMSK);
    /* Writing 1 to Run/Stop bit will cause to a atach from the host */
    HAL_hsusb_SetBits32(0, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK); 
  }
  else
  {
    (void)hsu_phy_utils_spoof_disconnect();
  }
#else /* defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
  HSU_USE_PARAM(enable);
#endif /* defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
}
/*===========================================================================
FUNCTION hsu_dload_is_wall_charger
DESCRIPTION
This function determines whether a wall charger is connected or not 
when called from the bootloader.
INPUT PARAMETERS
NONE
RETURN VALUE
1) Parameter type: boolean
-- TRUE: Its a wall charger 
-- FALSE: Its not
DEPENDENCIES
NONE
SIDE EFFECTS
NONE
===========================================================================*/
boolean hsu_dload_is_wall_charger(void)
{
#if (defined(FEATURE_HS_USB_CHARGING) && defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL))
    hsu_chg_wallchg_detect_wall_charger(hsu_wall_chg_detect_cb);
    return mdm_apis_is_wall_charger;
#else /* defined(FEATURE_HS_USB_CHARGING) && defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
  return FALSE;
#endif /* defined(FEATURE_HS_USB_CHARGING) && defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
}
/*===========================================================================
FUNCTION hsu_dload_sess_is_valid
DESCRIPTION
This function determines whether a HS USB session is valid or not when called
from the bootloader 
INPUT PARAMETERS
NONE
RETURN VALUE
Return Value Type: boolean 
-- TRUE: HS-USB session is valid 
-- FALSE: HS-USB session is invalid
DEPENDENCIES
pm_sbl_init
SIDE EFFECTS
NONE
===========================================================================*/
boolean hsu_dload_sess_is_valid(void)
{
#if (defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL))
  uint32 bsv_val = 0;
                            /* OTGSC */
  bsv_val = HAL_hsusb_GetBits32(0, HAL_HSUSB_OTGSC_ADDR, HAL_HSUSB_OTGSC_BSV_BMSK);
  return (bsv_val == HAL_HSUSB_OTGSC_BSV_BMSK);
#else /* defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
  return FALSE;
#endif /* defined(FEATURE_HS_USB_BASIC) && !defined(BUILD_HOSTDL) */
}
