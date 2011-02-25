//============================================================================
/**
 * @file        qhsusb_dci_msm6295.c
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
// $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/dci/qhsusb_dci_m6295.c#10 $$DateTime: 2008/12/03 04:34:39 $$Author: amirs $
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2008-09-03   amirs    First Draft
// 
// ===========================================================================

// #define DEBUG_ON_HAWK 1

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "comdef.h"                 // common defines - basic types as uint32 etc
#include "msm.h"                    // HWIO registers and macros see msmhwioreg.h
                                    
#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API
    #ifdef DEBUG_ON_HAWK
#error code not present
    #endif // DEBUG_ON_HAWK

    #include "clk.h"
    #include "clkrgm_msm.h"           // clk_regime_type
    #include "oemsbl_msm.h"           // clk_busy_wait
    #include "msmhwioregp.h"            // HWIO Reg
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

#ifdef FEATURE_QHSUSB_PHY_HSIC
static uint32 dci_read_ulpi(uint32 core_id,ulpi_reg_t ulpi_reg);
static void dci_write_ulpi(uint32 core_id,ulpi_reg_t ulpi_reg,ulpi_mode_t mode,uint32 value);
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
 * @Note :  USB-Charging is not supported in SCMM PBL, only software download!.
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
void qhsusb_dci_connect(uint32 core_id)
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

#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API
//============================================================================
/**
 * @function  dci_clk_regime_msm_reset_assert
 *
 * @brief     Assert CLK reset
 *
 * @Note : Local copy of CLK API to avoid build & Link of CLK in PBL (Boot team).
 *
 * @param regime    
 *
 * @return none
 *
 */
//============================================================================
void dci_clk_regime_msm_reset_assert(clk_regime_type regime)
{
  switch(regime)
  {
    case CLK_RGM_USB_M:
      MSM_SET (MSM_CLK_RESETB, USB_1_SW_RESET);
      MSM_SET (MSM_CLK_RESETB, USB_2_SW_RESET);
      /* Bug in HW causes ULPI_PHY to be reset when we write a 0 to it. */
      MSM_CLEAR (MSM_CLK_RESETB, ULPI_PHY_SW_RESET);
      break;

    case CLK_RGM_USB_PHY_RESET_M:
      /* Bug in HW causes ULPI_PHY to be reset when we write a 0 to it. */
      MSM_CLEAR (MSM_CLK_RESETB, ULPI_PHY_SW_RESET);
      break;

    default:
      /* Reset not supported for this item. */
      break;
  }
}
//============================================================================

//============================================================================
/**
 * @function  dci_clk_regime_msm_reset_deassert
 *
 * @brief     De-Assert CLK reset
 *
 * @Note : Local copy of CLK API to avoid build & Link of CLK in PBL (Boot team).
 *
 * @param regime    
 *
 * @return none
 *
 */
//============================================================================
void dci_clk_regime_msm_reset_deassert(clk_regime_type regime)
{
  /* ------------------------------------------------------------------------
     Reset specified clock regime.
  ------------------------------------------------------------------------ */
  switch(regime)
  {
    case CLK_RGM_USB_M:
      /* Bug in HW causes ULPI_PHY to be reset when we write a 0 to it. */
      MSM_SET (MSM_CLK_RESETB, ULPI_PHY_SW_RESET);
      MSM_CLEAR (MSM_CLK_RESETB, USB_1_SW_RESET);
      MSM_CLEAR (MSM_CLK_RESETB, USB_2_SW_RESET);
      break;

    case CLK_RGM_USB_PHY_RESET_M:
      /* Bug in HW causes ULPI_PHY to be reset when we write a 0 to it. */
      MSM_SET (MSM_CLK_RESETB, ULPI_PHY_SW_RESET);
      break;

    default:
      /* Reset not supported for this item. */
      break;
  }
}
#endif  // BUILD_BOOT_CHAIN

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
 * @Note : For development , the code was tested on Hawk (before SCMM Tape-Out).
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
  dci_core_info[0].phy_type = DCI_PHY_TYPE_ULPI;  // @todo - move to this file dci_core_info[]
  #ifdef FEATURE_QHSUSB_PHY_HSIC
  dci_core_info[1].phy_type = DCI_PHY_TYPE_HSIC;
  #endif // FEATURE_QHSUSB_PHY_HSIC

  #ifdef FEATURE_QHSUSB_PHY_HSIC
  if (dci_core_info[core_id].phy_type == DCI_PHY_TYPE_ULPI) 
  #endif // FEATURE_QHSUSB_PHY_HSIC
  {
      #ifdef DEBUG_ON_HAWK
#error code not present
      #endif // DEBUG_ON_HAWK

      MSM_LOCK();
    
      /* Force the link to be in reset until the PHY reset is done */
      dci_clk_regime_msm_reset_assert(CLK_RGM_USB_M);

      MSM_UNLOCK();
    
      /* Wait for PLL lock duration of the integrated phy which is 300 us */
      dci_delay_ms(1);
    
      MSM_LOCK();
    
      dci_clk_regime_msm_reset_deassert(CLK_RGM_USB_M);
    
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
      dci_clk_regime_msm_reset_assert(CLK_RGM_USB_PHY_RESET_M);
    
      /* Wait for PLL lock duration of the integrated phy which is 300 us */
      dci_delay_ms(1);
    
      /* Deassert PHY RESET */
      dci_clk_regime_msm_reset_deassert(CLK_RGM_USB_PHY_RESET_M);
    
      /* **************************** */
      /*  Integrated PHY CAL END      */
      /* **************************** */
      // pm_ret_val = pm_otg_pwr_sel(PM_OTG_PWR_SEL__VBUS);
      // DCI_ASSERT(pm_ret_val);
  }

  //========================================================
  //        HSIC
  //========================================================
  #ifdef FEATURE_QHSUSB_PHY_HSIC
  if (dci_core_info[core_id].phy_type == DCI_PHY_TYPE_HSIC) 
  {
      // Start Calibration of HSIC IO pads
      dci_write_ulpi(uint32 core_id,0x30,ULPI_MODE_SET_BIT,0x2);

      // @todo - CALIBARTION SEQUENCE !

      dci_write_ulpi(uint32 core_id,0x30,ULPI_MODE_CLEAR_BIT,0x2);
      // Set to normal mode
  }
  #endif // FEATURE_QHSUSB_PHY_HSIC
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

//============================================================================
/**
 * @function  dci_dcache_inval
 *
 * @brief Invalidate Data-Cache
 *
 * @note: Data-Cache is disabled in SCMM ROM code (PBL) but used in Hawk.
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

    #if ( defined(BUILD_BOOT_CHAIN) && defined(DEBUG_ON_HAWK) )
#error code not present
    #else
    //lint -esym(715,addr)
    //lint -esym(818,addr)
    //lint -esym(715,length)
    #endif // BUILD_BOOT_CHAIN
}

//============================================================================
/**
 * @function  dci_dcache_flush
 *
 * @brief Flush Data-Cache
 *
 * @note: Data-Cache is disabled in SCMM ROM code (PBL) but used in Hawk.
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
    #if ( defined(BUILD_BOOT_CHAIN) && defined(DEBUG_ON_HAWK) )
#error code not present
    #else
    //lint -esym(715,addr)
    //lint -esym(818,addr)
    //lint -esym(715,length)
    #endif // BUILD_BOOT_CHAIN
}

//============================================================================

#ifdef FEATURE_QHSUSB_PHY_HSIC
//============================================================================
/**
 * @function  dci_send_ulpi_command
 *
 * @brief Send a command to the ULPI-PHY via the ULPI-Viewport Core register.
 *
 * @note: for HSIC
 *
 * @param core_id    
 * @param bWrite    - Read/Write
 * @param ulpi_reg  - PHY Register Address
 * @param mode      - SET bit /CLEAR bit / direct
 * @param value     - value to write
 *
 * @return none
 *
 */
//============================================================================
static uint32  dci_send_ulpi_command(uint32 core_id,boolean bWrite,ulpi_reg_t ulpi_reg,ulpi_mode_t mode,uint32 value)
{
    uint32 tmp=0;

    // Verify ULPI is ready
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    tmp = DCI_HWIO_INM(core_id,USB_OTG_HS_ULPI_VIEWPORT,USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK); 
    if( tmp == 0) 
    {
        DCI_ASSERT(0x170);
    }
    #endif // QHSUSB_DEBUG_DCI_ENABLE
           
    // This is a Command
    tmp = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;

    if(bWrite) 
    {
        // This is a Write command
        tmp |= USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK;
        // Data to write
        tmp |= (value & USB_OTG_HS_ULPI_VIEWPORT_ULPIDATWR_BMSK);
    }

    // ULPI Reg to access
    tmp |= (((uint32)ulpi_reg+(uint32)mode) << USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT);

    // Send the Command  
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ULPI_VIEWPORT,tmp); 

    // Wait until completed
    do
    {
        tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ULPI_VIEWPORT); 
    }
    while((tmp & USB_OTG_HS_ULPI_VIEWPORT_ULPISS_BMSK) == 0);

    return(tmp);
}

//============================================================================
/**
 * @function  dci_read_ulpi
 *
 * @brief Send a READ command to the ULPI-PHY via the ULPI-Viewport Core register.
 *
 * @note: for HSIC
 *
 * @param core_id    
 * @param ulpi_reg  - PHY Register Address
 *
 * @return value
 *
 */
//============================================================================
static uint32  dci_read_ulpi(uint32 core_id,ulpi_reg_t ulpi_reg)
{
    uint32 tmp;

    tmp = dci_send_ulpi_command(core_id,FALSE,ulpi_reg,ULPI_MODE_DIRECT,0);

    return(tmp);
}

//============================================================================
/**
 * @function  dci_write_ulpi
 *
 * @brief Send a WRITE command to the ULPI-PHY via the ULPI-Viewport Core register.
 *
 * @note: for HSIC
 *
 * @param core_id    
 * @param ulpi_reg  - PHY Register Address
 * @param mode      - SET bit /CLEAR bit / direct
 * @param value     - value to write
 *
 * @return none
 *
 */
//============================================================================
static void  dci_write_ulpi(uint32 core_id,ulpi_reg_t ulpi_reg,ulpi_mode_t mode,uint32 value)
{
    dci_send_ulpi_command(core_id,TRUE,ulpi_reg,mode,value);
}
#endif // FEATURE_QHSUSB_PHY_HSIC

