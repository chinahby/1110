/*
===========================================================================

FILE:         HAL_hsusb_impl_ChipIdea.c

DESCRIPTION:  
This is the HAL implementation for the HS-USB ChipIdea core.

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

#include "HAL_hsusb_impl_ChipIdea.h"
#include "clk.h"                  /* clk_busy_wait etc functions */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

#define ULPI_NUM_OF_ATTEMPTS_TO_VERIFY_OPERATION 10000

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**
**  HAL_hsusb_ResetChipIdea
**
**  Description:
**    This function is intended to return the HS-USB ChipIdea core
**    to its  power-on status.
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
**    None.
**
** ======================================================================== */
void HAL_hsusb_ResetChipIdea(uint8 instance)
{
  HAL_hsusb_Write32(instance, HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RST_BMSK);
}

/* ===========================================================================
**
**  HAL_hsusb_ReadUlpiPhyChipIdea
**
**  Description:
**    This function reads from a ULPI PHY register of the HS-USB ChipIdea core port.
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
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhyChipIdea(uint8 instance, uint8 port, uint8 reg, uint8* data)
{
  uint32 ulpi_viewport_res;
  uint32 loop_idx;

  if (port >= HAL_hsusb_SupportedPortsQty(instance))
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_WrongPort;
    return HAL_HSUSB_WrongPort;
  }

  if (HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[port].PhyType.PhyInterfaceType != HAL_HSUSB_PHY_IF_ULPI)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HSUSB_WrongPhyInterface;
    return HSUSB_WrongPhyInterface;
  }

  HAL_hsusb_Write32(instance, HAL_HSUSB_ULPI_VIEWPORT_ADDR, 
    HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK
    | (port << HAL_HSUSB_ULPI_VIEWPORT_ULPIPORT_SHFT)
    | (reg << HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_SHFT)
    );
  /* Wait until the ULPI running bit is cleared */
  /* This is instead of a potential inf. loop;
  */
  for (loop_idx = 0 ; loop_idx < ULPI_NUM_OF_ATTEMPTS_TO_VERIFY_OPERATION ; ++loop_idx)
  {
    ulpi_viewport_res = HAL_hsusb_Read32(instance, HAL_HSUSB_ULPI_VIEWPORT_ADDR);
    if ((ulpi_viewport_res & HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK) == 0)
    {
      ulpi_viewport_res = HAL_hsusb_Read32(instance, HAL_HSUSB_ULPI_VIEWPORT_ADDR);
      if (data)
      {
        *data = 
          (uint8)((ulpi_viewport_res & HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_BMSK) 
          >> HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_SHFT);
      }
      return HAL_HSUSB_Success;
    }
    /* wait for 1 uS */
    clk_busy_wait(1);
  }
  /* Even after polling ULPI_NUM_OF_ATTEMPTS_TO_VERIFY_OPERATION times, 
  ULPIRUN bit still not cleared. */
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HSUSB_TimeoutExpired;
  return HSUSB_TimeoutExpired;
}

/* ===========================================================================
**
**  HAL_hsusb_WriteUlpiPhyChipIdea
**
**  Description:
**    This function writes to a ULPI PHY register of the HS-USB ChipIdea core port.
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
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhyChipIdea(uint8 instance, uint8 port, uint8 reg, uint8 data)
{
  uint32 ulpi_viewport_res;
  uint32 loop_idx;

  if (port >= HAL_hsusb_SupportedPortsQty(instance))
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_WrongPort;
    return HAL_HSUSB_WrongPort;
  }

  if (HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[port].PhyType.PhyInterfaceType != HAL_HSUSB_PHY_IF_ULPI)
  {
    HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HSUSB_WrongPhyInterface;
    return HSUSB_WrongPhyInterface;
  }

  HAL_hsusb_Write32(instance, HAL_HSUSB_ULPI_VIEWPORT_ADDR, 
    HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK
    | HAL_HSUSB_ULPI_VIEWPORT_ULPIRW_BMSK
    | (port << HAL_HSUSB_ULPI_VIEWPORT_ULPIPORT_SHFT)
    | (reg << HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_SHFT) 
    | (data << HAL_HSUSB_ULPI_VIEWPORT_ULPIDATWR_SHFT)
    );
  /* Wait until the ULPI running bit is cleared */
  /* This is instead of a potential inf. loop;
  */
  for (loop_idx = 0 ; loop_idx < ULPI_NUM_OF_ATTEMPTS_TO_VERIFY_OPERATION ; ++loop_idx)
  {
    ulpi_viewport_res = HAL_hsusb_Read32(instance, HAL_HSUSB_ULPI_VIEWPORT_ADDR);
    if ((ulpi_viewport_res & HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK) == 0)
    {
      return HAL_HSUSB_Success;
    }
    /* wait for 1 uS */
    clk_busy_wait(1);
  }
  /* Even after polling ULPI_NUM_OF_ATTEMPTS_TO_VERIFY_OPERATION times, 
  ULPIRUN bit still not cleared. */
  HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HSUSB_TimeoutExpired;
  return HSUSB_TimeoutExpired;
}

/* ===========================================================================
**
**  HAL_hsusb_ConstructImplChipIdea
**
**  Description:
**    This function initializes the HS-USB implementation object for 
**    the  HS-USB ChipIdea core implementation.
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    this     - Points to the HS-USB core interface object.
**
**  Return:
**    None.
**
** ======================================================================== */
void HAL_hsusb_ConstructImplChipIdea(struct HAL_HSUSB_CoreIfType* this)
{
  HAL_hsusb_ConstructImplDefault(this);
  this->Reset = HAL_hsusb_ResetChipIdea;
  this->ReadUlpiPhy = HAL_hsusb_ReadUlpiPhyChipIdea;
  this->WriteUlpiPhy = HAL_hsusb_WriteUlpiPhyChipIdea;
}
