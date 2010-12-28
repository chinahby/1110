//============================================================================
/**
 * @file        qhsusb_dci_msm11xx.c
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
// $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/dci/qhsusb_dci_q1100.c#8 $$DateTime: 2009/06/04 09:19:24 $$Author: vkondrat $
// 
// when          who     what, where, why
// ----------   -----    ----------------------------------------------------------
// 2009-05-29   dk       Change for QSC1100.
// 2008-08-20   amirs    First Draft
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "comdef.h"                 // common defines - basic types as uint32 etc
#include "msm.h"                    // HWIO registers and macros see msmhwioreg.h
                                    // 
#ifdef BUILD_BOOT_CHAIN    // ARMPROG doesn't support PMIC API
    #include "pm.h"                 // pm_otg_config_interface()
#endif // BUILD_BOOT_CHAIN
                                    
#include "qhsusb_dci_api.h"         // DCI API Prototypes
#include "qhsusb_dci_private.h"     // dQH_t
#include "qhsusb_dci_hw_registers.h"    // For Multi-Core
#include "armmmu.h"

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
    #if (defined(BUILD_BOOT_CHAIN))
    dci_is_attached = pm_sbl_usb_sess_is_valid();   
    #else
    dci_is_attached = TRUE;
    #endif

   return(dci_is_attached);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_wall_charger
 *
 * @brief check if Wall-Charger detected.
 *
 * @Note :  Wall Charger is detected by PMIC.
 *          It is done BEFORE the USB stack is called. 
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
boolean qhsusb_dci_is_wall_charger(uint32 core_id)
{
    // Stub function
    //lint -esym(715,core_id)
    // @note The USB-Stack is called after Wall-Charger connected
    return(FALSE);
}

//============================================================================
/**
 * @function  qhsusb_dci_is_connected
 *
 * @brief Report if the device is connected to the bus.
 *
 * @note  The D+ line is controlled via the PMIC.
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
    // Stub function
    //lint -esym(715,core_id)
    return(dci_is_connected);
}

//============================================================================
/**
 * @function  qhsusb_dci_connect
 *
 * @brief Connect the device to the USB bus.
 *
 * @note  The D+ line is controlled via the PMIC.
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

    #ifdef BUILD_BOOT_CHAIN
    {
        uint32 ret=0;
    
        /* Pull up D+ */
        ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
          PM_OTG_DP_CONFIG_CMD__DISABLE_BOTH, 
          PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
          PM_OTG_ID_CONFIG_CMD__NONE);

        DCI_ASSERT(ret);

    
        ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
          PM_OTG_DP_CONFIG_CMD__PULL_UP, 
          PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
          PM_OTG_ID_CONFIG_CMD__NONE);

        DCI_ASSERT(ret);

    }
    #endif // BUILD_BOOT_CHAIN

//    HWIO_OUTM(USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_RUN_FVAL); 
}

//============================================================================
/**
 * @function  qhsusb_dci_disconnect
 *
 * @brief Disconnect the device from the USB bus.
 *
 * @note  The D+ line is controlled via the PMIC.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 */
//============================================================================
void    qhsusb_dci_disconnect(uint32 core_id)
{

    dci_is_connected = FALSE;

    #ifdef BUILD_BOOT_CHAIN
    {
        uint32 ret=0;
        /* Pull down D+/D- to make sure fast transtion of line status */
        ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
          PM_OTG_DP_CONFIG_CMD__DISABLE_BOTH, 
          PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
          PM_OTG_ID_CONFIG_CMD__NONE);

        DCI_ASSERT(ret);
    
        ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
          PM_OTG_DP_CONFIG_CMD__PULL_DOWN, 
          PM_OTG_DM_CONFIG_CMD__PULL_DOWN, 
          PM_OTG_ID_CONFIG_CMD__NONE);

        DCI_ASSERT(ret);
    }
    #endif // BUILD_BOOT_CHAIN

//    HWIO_OUTM(USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_STOP_FVAL); 
}

//============================================================================
/**
 * @function  dci_init_phy
 *
 * @brief Initialize the PHY.
 *
 * @Note :  The func pm_init_otg_usb is called before the USB Stack and doing most of the preperations.
 *          Q1110 is using Full-Speed Externnal PHY connected via 3-wire.
 *
 * @param core_id    
 *
 * @return none
 *
 * @see pm_init_otg_usb
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx , PHY type selection.
 *      Chapter 3.6.12 ULPI VIEWPORT , control the PHY from the USB Core.
 *
 */
//============================================================================
void dci_init_phy(uint32 core_id)
{
    // Select serial-PHY for ULC ==> STS=Serial-Transceiver-Select
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_STS_BMSK,USB_OTG_HS_PORTSC_STS_BMSK); 
}

//============================================================================
/**
 * @function  dci_dcache_inval
 *
 * @brief Invalidate Data-Cache
 *
 * @note: STUB for Q1110
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
    mmu_dcache_inval_region(addr,length);
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
 * @note: STUB for Q1110
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
    mmu_dcache_flush_region(addr,length);  
    #else
    //lint -esym(715,addr)
    //lint -esym(818,addr)
    //lint -esym(715,length)
    #endif
}
