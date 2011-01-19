//============================================================================
/**
 * @file        qhsusb_dci_msm6290.c
 * @author      amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) PHY specific handling.
 *
 * @details      There might be different PHY types for different targets.
 *               When using 3-wired Full-Speed PHY the USB Core (Link) cannot conntrol the D+/D-.
 * 
 * @note 
 * 
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *
 *              Copyright (c) 2008 QUALCOMM Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
 */
//============================================================================

// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/dci/qhsusb_dci_m6290.c#5 $$DateTime: 2008/12/14 01:45:57 $$Author: amirs $
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2008-08-20   amirs    First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "comdef.h"                 // common defines - basic types as uint32 etc
#include "msm.h"                    // HWIO registers and macros see msmhwioreg.h
                                    
#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API
    #include "pm.h"                 // pm_otg_config_interface()
    #include "clk.h"
    #include "clkrgm_msm.h"
    #include "oemsbl_msm.h"           // clk_busy_wait
    #include "cache_mmu.h"             // MMU routines
#endif // BUILD_BOOT_CHAIN
                                    
#include "qhsusb_dci_api.h"         // DCI API Prototypes
#include "qhsusb_dci_private.h"     // dQH_t
#include "qhsusb_dci_hw_registers.h"    // For Multi-Core

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
// None

//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------
core_info_t dci_core_info[DCI_MAX_CORES];
uint32 dci_core_base_address[DCI_MAX_CORES] = {HWIO_USB_OTG_HS_ID_ADDR}; // By default HS-USB Core

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------
static boolean dci_is_connected = FALSE;
static boolean dci_is_attached = FALSE;    // debug
//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------
#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API
static void dci_delay_ms(uint32 msec);
#endif
//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------


//============================================================================
/**
 * @function  qhsusb_dci_is_attached
 *
 * @brief Report if a USB cable is attached to the device.
 *
 * @Note :  Although attached, the host communicates with the device only after 
 *          the device is *connected* to the bus (D+ pull-up).
 *
 * @Note : In Device Mode:
 *      1=Attached. 0=Not Attached. Default=0. 
 *      A one indicates that the device successfully attached
 *      and is operating in either high speed or full speed as indicated by the High Speed Port bit in
 *      this register. 
 *      A zero indicates that the device did not attach successfully or was forcibly
 *      disconnected by the software writing a zero to the Run bit in the USBCMD register. 
 *      It does not state the device being disconnected or suspended.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx , CCS bit.
 *
 */
//============================================================================
boolean qhsusb_dci_is_attached(uint32 core_id)
{
    uint32 attached = DCI_HWIO_INM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_CCS_BMSK);
    attached = VAL_TO_BOOL(attached);

    dci_is_attached = (boolean)attached;

   return((boolean)attached);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_wall_charger
 *
 * @brief check if Wall-Charger detected.
 *
 * @Note :  USB-Charging is not supported. only software download!.
 *
 * @Note :  USB-Wall-Charger can be detected by the PHY. See ULPI-PHY spec.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "USB2ULPItpl_45nm-b - USB2.0 OTG ULPI":
 *      Table 5-10 Charger Detection Interface
 *
 */
//============================================================================
boolean qhsusb_dci_is_wall_charger(uint32 core_id)
{
    return(FALSE);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_connected
 *
 * @brief Report if the device is connected to the bus.
 *
 * @Note : When using ULPI-PHY the D+ is controlled via the Run/Stop bit in USBCMD register.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
boolean qhsusb_dci_is_connected(uint32 core_id)
{
    return(dci_is_connected);
}

//============================================================================
/**
 * @function  qhsusb_dci_connect
 *
 * @brief Connect the device to the USB bus.
 *
 * @Note : When using ULPI-PHY the D+ is controlled via the Run/Stop bit in USBCMD register.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
void    qhsusb_dci_connect(uint32 core_id)
{

    dci_is_connected = TRUE; 

    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_RUN_FVAL); 
}

//============================================================================
/**
 * @function  qhsusb_dci_disconnect
 *
 * @brief Disconnect the device from the USB bus.
 *
 * @Note : When using ULPI-PHY the D+ is controlled via the Run/Stop bit in USBCMD register.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
void    qhsusb_dci_disconnect(uint32 core_id)
{

    dci_is_connected = FALSE;

    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_STOP_FVAL); 
}

//============================================================================
/**
 * @function  dci_init_phy
 *
 * @brief Initialize the PHY.
 *
 * @Note : Power and clock should be provided by the Boot Team or by the Hardware default.
 *
 * @Note : The PHY D+/D- IO pad strength needs to be calibrated.
 *
 * @Note : Internal Power (battery) should be used until calibartion is completed.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx , PHY type selection.
 *      Chapter 3.6.12 ULPI VIEWPORT , control the PHY from the USB Core.
 *
 */
//============================================================================

   /* Koel and Hawk */
void dci_init_phy(uint32 core_id)
{
#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API

  pm_err_flag_type pm_ret_val;

  /* Request from PMIC 3.3 V for VREG_USB */
  pm_ret_val = pm_vreg_control(PM_ON_CMD, PM_VREG_USB_M);
  DCI_ASSERT(pm_ret_val);

  pm_ret_val = pm_vreg_set_level(PM_VREG_USB_ID, 3300);
  DCI_ASSERT(pm_ret_val);  

  MSM_LOCK();

  /* Force the link to be in reset until the PHY reset is done */
  clk_regime_msm_reset_assert(CLK_RGM_USB_M);

  MSM_UNLOCK();

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  dci_delay_ms(1);

  MSM_LOCK();

  clk_regime_msm_reset_deassert(CLK_RGM_USB_M);

  /***********************************************************************/
  /* KOEL BRING-UP FIX #2: USB_1_SW_RESET reset all link register settings
                          to default.  Putting link to ULPI mode has to
                          be done after USB_1_SW_RESET reset.            */
  /* set core to ULPI mode */
  DCI_HWIO_OUT(core_id,USB_OTG_HS_PORTSC, 0x80000000);
  /***********************************************************************/

  /* De-assert ulpi_mx_nom_stop_ctrl signal */
  /* @TODO-integrated PHY - This code will not work for multi-core. 
  May need to check the controller index or do the actions in HSU adaptation layer if possible*/ 
  MSM_CLEAR( ULPI_MUX_CONFIG, STOP_CTRL );

  MSM_UNLOCK();

  /* *************************** */
  /* Integrated PHY CAL BEGIN    */
  /* *************************** */

  /* Set suspendM (bit 6)=0. PHY reg: 0x5 (func ctl set) 0x6 (func ctl clear) */
  DCI_HWIO_OUT(core_id,USB_OTG_HS_ULPI_VIEWPORT, 0x60060040);

  /* Wait for 1 ms */
  dci_delay_ms(1);

  /* Assert PHY RESET */
  clk_regime_msm_reset_assert(CLK_RGM_USB_PHY_RESET_M);

  /* Wait for PLL lock duration of the integrated phy which is 300 us */
  dci_delay_ms(1);

  /* Deassert PHY RESET */
  clk_regime_msm_reset_deassert(CLK_RGM_USB_PHY_RESET_M);

  /* **************************** */
  /*  Integrated PHY CAL END      */
  /* **************************** */
    pm_ret_val = pm_otg_pwr_sel(PM_OTG_PWR_SEL__VBUS);
    DCI_ASSERT(pm_ret_val);
#endif // defined(BUILD_BOOT_CHAIN)

} // dci_init_phy

#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API
//============================================================================
/**
 * @function  dci_delay_ms
 *
 * @brief Perform delay in mili seconds.
 *
 * @Note : The USB Timers can not be used before the core is initialized.
 *
 * @param milliseconds
 *
 * @return none
 *
 */
//============================================================================
static void dci_delay_ms(uint32 milliseconds)
{
    int counter;

    for (counter = 0; counter < milliseconds; counter++)
    {
        clk_busy_wait(1000); 
    }   
}
#endif // defined(BUILD_BOOT_CHAIN)

//============================================================================
/**
 * @function  dci_dcache_inval
 *
 * @brief Invalidate Data-Cache
 *
 * @param address    
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void dci_dcache_inval(uint32 *addr, uint32 length)
{
    #if defined(BUILD_BOOT_CHAIN)
    dcache_inval_region(addr,length);
    #else
    //lint -esym(715,addr)
    //lint -esym(818,addr)
    //lint -esym(715,length)
    #endif
}

//============================================================================
/**
 * @function  dci_dcache_flush
 *
 * @brief Flush Data-Cache
 *
 * @param address    
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void dci_dcache_flush(uint32 *addr, uint32 length)
{
    #if defined(BUILD_BOOT_CHAIN)
    dcache_flush_region(addr,length);  
    #else
    //lint -esym(715,addr)
    //lint -esym(818,addr)
    //lint -esym(715,length)
    #endif
}


