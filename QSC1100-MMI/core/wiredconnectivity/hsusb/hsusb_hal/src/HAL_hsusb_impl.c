/*
===========================================================================

FILE:         HAL_hsusb_impl.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB core.

===========================================================================

===========================================================================
Copyright © 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl.h"

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Constructor.
*/
extern void HAL_hsusb_ConstructImplInstance(void);

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_Init
**
**  Description:
**    This function initializes the HS-USB core Hardware prior 
**    to the first usage after power-up or a call to HAL_hsusb_Reset().
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Init(void)
{
  if (!HAL_HSUSB_CoresObj.Initialized)
  {
    HAL_hsusb_ConstructImplInstance();
    HAL_HSUSB_CoresObj.Initialized = TRUE;
  }
}

/* ===========================================================================
**
**  HAL_hsusb_Reset
**
**  Description:
**    This function is intended to return the HS-USB hardware to its 
**    power-on status.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Reset(void)
{
  uint8 instance;

  for (instance=0; instance<HAL_hsusb_SupportedCoresQty() ; instance++)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].If->Reset(instance);
  }
  HAL_HSUSB_CoresObj.Initialized = FALSE;
}


/* ===========================================================================
**
**  HAL_hsusb_SupportedCoresQty
**
**  Description:
**    This function reports the HS-USB cores’ amount.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**     The number of the HS-USB cores attached to the target.
**
** ======================================================================== */
uint8 HAL_hsusb_SupportedCoresQty(void)
{
  return HAL_HSUSB_CoresObj.MaxCoresQty;
}

/* ===========================================================================
**
**  HAL_hsusb_HWversion
**
**  Description:
**    This function reports the HW version for a given HS-USB core.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The HW version for a given HS-USB core.
**
** ======================================================================== */
uint16 HAL_hsusb_HWversion(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->Version;
}

/* ===========================================================================
**
**  HAL_hsusb_SupportedPortsQty
**
**  Description:
**    This function reports the HS-USB core ports' amount.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The number of the ports attached to a HS-USB core.
**
** ======================================================================== */
uint8 HAL_hsusb_SupportedPortsQty(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty;
}

/* ===========================================================================
**
**  HAL_hsusb_SupportedPhyType
**
**  Description:
**    This function reports which PHY is supported by a HS-USB core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**
**  Return:
**    The PHY supported by a HS-USB core port.
**
** ======================================================================== */
HAL_HSUSB_PhyType* HAL_hsusb_SupportedPhyType(uint8 instance, uint8 port)
{
  if (port >= HAL_hsusb_SupportedPortsQty(instance))
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_WrongPort;
    return NULL;
  }

  return &HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[port].PhyType;
}

/* ===========================================================================
**
**  HAL_hsusb_GetLastError
**
**  Description:
**    This function gets the last error code of the HS-USB core recorded
**    from the HS-USB HAL failure. It is intended for debug purposes.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The last error code of the HS-USB core recorded from the HS-USB HAL failure.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_GetLastError(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].LastError;
}

/* ===========================================================================
**
**  HAL_hsusb_Read8
**
**  Description:
**    This function reads 8 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint8 HAL_hsusb_Read8(uint8 instance, uint32 offset)
{
  return inp(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset);
}

/* ===========================================================================
**
**  HAL_hsusb_Read16
**
**  Description:
**    This function reads 16 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint16 HAL_hsusb_Read16(uint8 instance, uint32 offset)
{
  return inpw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset);
}

/* ===========================================================================
**
**  HAL_hsusb_Read32
**
**  Description:
**    This function reads 32 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ======================================================================== */
uint32 HAL_hsusb_Read32(uint8 instance, uint32 offset)
{
  return inpdw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset);
}

/* ===========================================================================
**
**  HAL_hsusb_Write8
**
**  Description:
**    This function writes 8 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Write8(uint8 instance, uint32 offset, uint8 data)
{
  outp(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset, data);
}

/* ===========================================================================
**
**  HAL_hsusb_Write16
**
**  Description:
**    This function writes 16 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Write16(uint8 instance, uint32 offset, uint16 data)
{
  outpw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset, data);
}

/* ===========================================================================
**
**  HAL_hsusb_Write32
**
**  Description:
**    This function writes 32 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_Write32(uint8 instance, uint32 offset, uint32 data)
{
  outpdw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset, data);
}

/* ===========================================================================
**
**  HAL_hsusb_GetBits32
**
**  Description:
**    This function gets bits from the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be get.
**
**  Return:
**    The read data masked with <bits>.
**
** ======================================================================== */
uint32 HAL_hsusb_GetBits32(uint8 instance, uint32 offset, uint32 bits)
{
  return (inpdw(HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset) & bits);
}

/* ===========================================================================
**
**  HAL_hsusb_SetBits32
**
**  Description:
**    This function sets bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be set.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetBits32(uint8 instance, uint32 offset, uint32 bits)
{
  uint32 address;
  uint32 data;

  address = HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset;
  HWIO_INTLOCK();
  data = inpdw(address);
  outpdw(address, data | bits);
  HWIO_INTFREE();
}

/* ===========================================================================
**
**  HAL_hsusb_ClearBits32
**
**  Description:
**    This function clears bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be cleared.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ClearBits32(uint8 instance, uint32 offset, uint32 bits)
{
  uint32 address;
  uint32 data;

  address = HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset;
  HWIO_INTLOCK();
  data = inpdw(address);
  outpdw(address, data & ~bits);
  HWIO_INTFREE();
}

/* ===========================================================================
**
**  HAL_hsusb_ClearAndSetBits32
**
**  Description:
**    This function clears and then sets bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance  - Determines which HS-USB core to use. 
**              This value should be one less than reported by 
**              the HAL_hsusb_SupportedCoresQty().
**    offset    - Determines the register offset. 
**              This value must be aligned at the 32 bit boundary and 
**              in the address ranges defined by the HW specification.
**    clearBits - Determines the bits to be cleared.
**    setBits   - Determines the bits to be set.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ClearAndSetBits32(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits)
{
  uint32 address;
  uint32 data;

  address = HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress+offset;
  HWIO_INTLOCK();
  data = inpdw(address);
  outpdw(address, (data & ~clearBits) | setBits);
  HWIO_INTFREE();
}

/* ===========================================================================
**
**  HAL_hsusb_ReadUlpiPhy
**
**  Description:
**    This function reads from a ULPI PHY register of the HS-USB core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Returns the read data.
**
**  Return:
**    HAL_HSUSB_Success           - the returned <data> value is valid.
**    HAL_HSUSB_WrongPort         – the <port> is out of scope reported 
**                                  by the HAL_hsusb_SupportedPortsQty().
**    HAL_HSUSB_WrongPhyInterface – the Phy Interface is not ULPI.
**    HAL_HSUSB_TimeoutExpired    – the maximal timeout has been expired.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8* data)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->ReadUlpiPhy(instance, port, reg, data);
}

/* ===========================================================================
**
**  HAL_hsusb_WriteUlpiPhy
**
**  Description:
**    This function writes to a ULPI PHY register of the HS-USB core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Determines the write data.
**
**  Return:
**    HAL_HSUSB_Success           - the requested <data> is set.
**    HAL_HSUSB_WrongPort         – the <port> is out of scope reported 
**                                  by the HAL_hsusb_SupportedPortsQty().
**    HAL_HSUSB_WrongPhyInterface – the Phy Interface is not ULPI.
**    HAL_HSUSB_TimeoutExpired    – the maximal timeout has been expired.
**
** ======================================================================== */
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8 data)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->WriteUlpiPhy(instance, port, reg, data);
}

/* ===========================================================================
**
**  HAL_hsusb_GetAhbBurstMode
**
**  Description:
**    This function reports the mode of the AMBA AHB master interface 
**    m_hburst signal.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The mode of the AMBA AHB master interface m_hburst signal.
**
** ======================================================================== */
HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetAhbBurstMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAhbBurstMode
**
**  Description:
**    This function sets the mode of the AMBA AHB master interface 
**    m_hburst signal.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines mode of the AMBA AHB master interface 
**             m_hburst signal.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetAhbBurstMode(uint8 instance, HAL_HSUSB_AhbBurstMode eMode)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetAhbBurstMode(instance, eMode);
}

/* ===========================================================================
**
**  HAL_hsusb_GetSe0GlitchFixCtrl
**
**  Description:
**    This function reports whether the SE0 glitch fix mechanism is activated.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SE0 glitch fix mechanism is activated.
**    FALSE   - The SE0 glitch fix mechanism is not activated.
**
** ======================================================================== */
boolean HAL_hsusb_GetSe0GlitchFixCtrl(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetSe0GlitchFixCtrl(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetSe0GlitchFixCtrl
**
**  Description:
**    This function activates/disactivate the SE0 glitch fix mechanism.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Activate the SE0 glitch fix mechanism
**             FALSE - Deactivate the SE0 glitch fix mechanism
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetSe0GlitchFixCtrl(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetSe0GlitchFixCtrl(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetFs3Wire2WireSelect
**
**  Description:
**    This function reports the two wire interface on the fs_dat and 
**    fs_se0 pins mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 3 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The two wire interface on the fs_dat and 
**              fs_se0 pins is enabled.
**    FALSE   - The two wire interface on the fs_dat and 
**              fs_se0 pins is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetFs3Wire2WireSelect(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetFs3Wire2WireSelect(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetFs3Wire2WireSelect
**
**  Description:
**    This function enables/disables the two wire interface on the fs_dat and 
**    fs_se0 pins.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 3 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Enable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**             FALSE - Disable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetFs3Wire2WireSelect(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetFs3Wire2WireSelect(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetXtorBypass
**
**  Description:
**    This function reports whether the AHB Transactor is bypassed.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The AHB Transactor is bypassed.
**    FALSE   - The AHB Transactor is not bypassed.
**
** ======================================================================== */
boolean HAL_hsusb_GetXtorBypass(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetXtorBypass(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetXtorBypass
**
**  Description:
**    This function sets the AHB Transactor bypass mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the AHB Transactor bypass mode.
**             Values are:
**             TRUE  - Bypass the AHB Transactor
**             FALSE - Do not bypass the AHB Transactor
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetXtorBypass(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetXtorBypass(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetPostedWrites
**
**  Description:
**    This function reports the USB Core AHB posted data writes mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The AHB master interface enables posted data writes
**    FALSE - The AHB master interface disables posted data writes
**
** ======================================================================== */
boolean HAL_hsusb_GetPostedWrites(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetPostedWrites(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetPostedWrites
**
**  Description:
**    This function sets the USB Core AHB posted data writes mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core AHB posted data writes mode.
**             Values are:
**             TRUE  - The AHB master interface enables posted data writes
**             FALSE - The AHB master interface disables posted data writes
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetPostedWrites(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetPostedWrites(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetGrantStolen
**
**  Description:
**    This function reports whether the arbiter removed the hgrant signal 
**    prior to completing a transaction.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The arbiter removed the hgrant signal 
**              prior to completing a transaction.
**    FALSE   - The arbiter doesn’t removed the hgrant signal 
**              prior to completing a transaction.
**
** ======================================================================== */
boolean HAL_hsusb_GetGrantStolen(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetGrantStolen(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetGrantStolen
**
**  Description:
**    This function sets the arbiter removing the hgrant signal 
**    prior to completing a transaction.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the arbiter removing the hgrant signal 
**             prior to completing a transaction mode.
**             Values are:
**             TRUE  - Enable the arbiter removed the hgrant signal 
**                   prior to completing a transaction
**             FALSE - Disable the arbiter doesn’t removed the hgrant signal 
**                   prior to completing a transaction
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetGrantStolen(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetGrantStolen(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHprotMode
**
**  Description:
**    This function reports the HPROT signal mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The HPROT signal mode.
**
** ======================================================================== */
HAL_HSUSB_HprotMode HAL_hsusb_GetHprotMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHprotMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHprotMode
**
**  Description:
**    This function sets the HPROT signal mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines the HPROT signal mode.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetHprotMode(uint8 instance, HAL_HSUSB_HprotMode eMode)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHprotMode(instance, eMode);
}

/* ===========================================================================
**
**  HAL_hsusb_GetIncrOverride
**
**  Description:
**    This function reports the USB Core INCR bursts transformation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The all INCR bursts from the USB Core will be 
**            internally transformed into SINGLE transfers
**    FALSE - If the USB Core issues an INCR burst, it will 
**            propagate to the external master AHB port
**
** ======================================================================== */
boolean HAL_hsusb_GetIncrOverride(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetIncrOverride(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetIncrOverride
**
**  Description:
**    This function sets the USB Core INCR bursts transformation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core INCR bursts transformation.
**             Values are:
**             TRUE  - The all INCR bursts from the USB Core will be 
**                   internally transformed into SINGLE transfers
**             FALSE - If the USB Core issues an INCR burst, it will 
**                   propagate to the external master AHB port
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetIncrOverride(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetIncrOverride(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetAsyncBridgesBypass
**
**  Description:
**    This function reports the asynchronous bridge bypass mode on 
**    the master AHB interface.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The asynchronous bridge on the master AHB interface is bypassed.
**    FALSE   - The asynchronous bridge on the master AHB interface issued.
**
** ======================================================================== */
boolean HAL_hsusb_GetAsyncBridgesBypass(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetAsyncBridgesBypass(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetAsyncBridgesBypass
**
**  Description:
**    This function sets the asynchronous bridge bypass mode on 
**    the master AHB interface.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the asynchronous bridge bypass mode on 
**             the master AHB interface.
**             Values are:
**             TRUE  - The asynchronous bridge on the master AHB interface 
**                   is bypassed
**             FALSE - The asynchronous bridge on the master AHB interface 
**                   issued
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetAsyncBridgesBypass(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetAsyncBridgesBypass(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetTestBusSelector
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The group of the internal signals that is routed to the test bus.
**
** ======================================================================== */
HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelector(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetTestBusSelector(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetTestBusSelector
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines which group of the internal signals is routed 
**             to the test bus.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetTestBusSelector(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetTestBusSelector(instance, eMode);
}

/* ===========================================================================
**
**  HAL_hsusb_GetTxBufCollisionMode
**
**  Description:
**    This function reports the TX buffer controller mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX buffer controller direct mode.
**    FALSE   - The TX buffer controller revert mode.
**
** ======================================================================== */
boolean HAL_hsusb_GetTxBufCollisionMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetTxBufCollisionMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetTxBufCollisionMode
**
**  Description:
**    This function sets the TX buffer controller mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX buffer controller mode.
**             Values are:
**             TRUE  - The TX buffer controller direct mode
**             FALSE - The TX buffer controller revert mode
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetTxBufCollisionMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetTxBufCollisionMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamRxBypassMode
**
**  Description:
**    This function reports the RX streaming bypass mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The RX streaming bypass is enabled.
**    FALSE   - The RX streaming bypass is disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetStreamRxBypassMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetStreamRxBypassMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamRxBypassMode
**
**  Description:
**    This function sets the RX streaming bypass mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the RX streaming bypass mode.
**             Values are:
**             TRUE  - Enable the RX streaming bypassing
**             FALSE - Disable the RX streaming bypassing
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetStreamRxBypassMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetStreamRxBypassMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetStreamTxAutoMode
**
**  Description:
**    This function reports the TX streaming automatic determination mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX streaming automatic determination is enabled.
**    FALSE   - The TX streaming automatic determination s is disabled
**
** ======================================================================== */
boolean HAL_hsusb_GetStreamTxAutoMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetStreamTxAutoMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetStreamTxAutoMode
**
**  Description:
**    This function sets the TX streaming automatic determination mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX streaming automatic determination mode.
**             Values are:
**             TRUE  - Enable TX streaming automatic determination
**             FALSE - Disable TX streaming automatic determination
**
**  Return:
**    TRUE    - the requested <state> is set.
**    FALSE   - the requested <state> is not set.
**
** ======================================================================== */
void HAL_hsusb_SetStreamTxAutoMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetStreamTxAutoMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersStdMode
**
**  Description:
**    This function reports the timers used for the USB reset on the ULPI mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB reset on the ULPI are enabled.
**    FALSE   - The timers used for the USB reset on the ULPI are disabled.
**
** ======================================================================== */
boolean HAL_hsusb_GetHostSimTimersStdMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHostSimTimersStdMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersStdMode
**
**  Description:
**    This function sets the timers used for the USB reset on the ULPI mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB reset on the ULPI mode.
**             Values are:
**             TRUE  - Enable the timers used for the USB reset on the ULPI
**             FALSE - Disable the timers used for the USB reset on the ULPI
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetHostSimTimersStdMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHostSimTimersStdMode(instance, state);
}

/* ===========================================================================
**
**  HAL_hsusb_GetHostSimTimersSuspMode
**
**  Description:
**    This function reports the timers used for the USB suspend process mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB suspend process short 
**              for faster simulation and ATE time.
**    FALSE   - The timers used for the USB suspend process are 
**              according to the USB specification.
**
** ======================================================================== */
boolean HAL_hsusb_GetHostSimTimersSuspMode(uint8 instance)
{
  return HAL_HSUSB_CoresObj.CoreInfo[instance].If->GetHostSimTimersSuspMode(instance);
}

/* ===========================================================================
**
**  HAL_hsusb_SetHostSimTimersSuspMode
**
**  Description:
**    This function sets the timers used for the USB suspend process mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB suspend process mode.
**             Values are:
**             TRUE  - The timers used for the USB suspend process short 
**                     for faster simulation and ATE time
**             FALSE - The timers used for the USB suspend process are 
**                     according to the USB specification
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_SetHostSimTimersSuspMode(uint8 instance, boolean state)
{
  HAL_HSUSB_CoresObj.CoreInfo[instance].If->SetHostSimTimersSuspMode(instance, state);
}
