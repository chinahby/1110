/*
===========================================================================

FILE:         HAL_hsusb_targets.c

DESCRIPTION:  
This is the HAL implementation of the run-time initialization for all MSM targets.

===========================================================================

===========================================================================
Copyright © 2009 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "HAL_hsusb_impl_1r8.h"
#include "HAL_hsusb_impl_3r0.h"
#include "HAL_hsusb_impl_Legacy.h"

/* -----------------------------------------------------------------------
**                           EXTERNS
** ----------------------------------------------------------------------- */

/*
* HS-USB Cores' base addresses.
*/
extern uint32 usb_core_base[];

#ifndef hw_msm_id
extern uint32 hw_msm_id(void);
#endif /* !hw_msm_id */
/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/* ChipIdea core interface */
static HAL_HSUSB_CoreIfType HAL_HSUSB_CoreIfChipIdea;

/* Legacy USB core interface */
static HAL_HSUSB_CoreIfType HAL_HSUSB_CoreIfLegacy;

/* USB core 0 ports. */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore0[1];
/* USB core 1 ports. */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore1[1];
/* USB core 2 ports. */
static HAL_HSUSB_PortInfoType HAL_HSUSB_PortInfoCore2[1];

/*
* HS-USB HAL CoreInfo table.
*/
static HAL_HSUSB_CoreInfoType HAL_HSUSB_CoreInfo[3];

/*
* HS-USB HAL Cores object.
*/
HAL_HSUSB_CoresType HAL_HSUSB_CoresObj =
{
  &HAL_HSUSB_CoreInfo[0],
  0,
  FALSE
};

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/*
* HS-USB HAL Constructor.
*/
void HAL_hsusb_ConstructImplInstance(void)
{
  uint8 instance;

  switch (hw_msm_id())
  {
  case 1/*HW_MDM1000*/:
    HAL_hsusb_ConstructImpl1r8(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 1;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_EXTERNAL;
    break;
  case 39/*HW_MDM2000*/:
    HAL_hsusb_ConstructImpl2r8(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 1;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 32/*HW_QST1000*/:
  case 33/*HW_QST1005*/:
  case 34/*HW_QST1100*/:
  case 35/*HW_QST1105*/:
    HAL_hsusb_ConstructImpl2r10(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 1;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_SERIAL;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 2/*HW_ESM6235*/:
  case 3/*HW_QSC6240*/:
  case 4/*HW_MSM6245*/:
  case 8/*HW_MSM6246*/:
  case 9/*HW_QSC6270*/:
  case 11/*HW_MSM6290*/:
  case 52/*HW_ESM6246*/:
  case 53/*ESM6290*/:
    HAL_hsusb_ConstructImpl2r8(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 2;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_FS_INTERCHIP;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 12/*HW_MSM7200*/:
  case 13/*HW_MSM7201*/:
  case 14/*HW_ESM7205*/:
  case 15/*HW_ESM7206*/:
  case 16/*HW_MSM7200A*/:
  case 17/*HW_MSM7201A*/:
  case 18/*HW_ESM7205A*/:
  case 19/*HW_ESM7206A*/:
  case 22/*HW_MSM7500*/:
  case 23/*HW_MSM7500A*/:
  case 25/*HW_MSM7600*/:
  case 26/*HW_MSM7601*/:
    HAL_hsusb_ConstructImpl1r8(&HAL_HSUSB_CoreIfChipIdea);
    HAL_hsusb_ConstructImplLegacy(&HAL_HSUSB_CoreIfLegacy);
    HAL_HSUSB_CoresObj.MaxCoresQty = 2;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_EXTERNAL;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfLegacy;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_SERIAL;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 20/*HW_ESM7225*/:
  case 21/*HW_MSM7225*/:
  case 24/*HW_MSM7525*/:
  case 27/*HW_MSM7625*/:
  case 40/*HW_QST1500*/:
  case 41/*HW_QST1600*/:
  case 42/*HW_QST1700*/:
  case 43/*HW_MSM7227*/:
  case 44/*HW_MSM7627*/:
    HAL_hsusb_ConstructImpl2r8(&HAL_HSUSB_CoreIfChipIdea);
    HAL_hsusb_ConstructImplLegacy(&HAL_HSUSB_CoreIfLegacy);
    HAL_HSUSB_CoresObj.MaxCoresQty = 2;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfLegacy;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_SERIAL;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 28/*HW_MSM7800*/:
    HAL_hsusb_ConstructImpl2r9(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 3;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_FS_INTERCHIP;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 2 */
    instance = 2;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore2[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_SERIAL;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 29/*HW_MSM8200,HW_MSM8200*/:
  case 31/*HW_MDM8900*/:
    HAL_hsusb_ConstructImpl2r10(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 2;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_FS_INTERCHIP;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 30/*HW_QSD8650*/:
  case 36/*HW_QSD8250*/:
  case 37/*HW_QSD8550*/:
  case 38/*HW_QSD8850*/:
    HAL_hsusb_ConstructImpl2r10(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 3;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_FS_INTERCHIP;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 2 */
    instance = 2;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore2[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_SERIAL;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  case 45/*HW_QSC6165*/:
  case 46/*HW_QSC6175*/:
  case 47/*HW_QSC6185*/:
  case 48/*HW_QSC6195*/:
  case 49/*HW_QSC6285*/:
  case 50/*HW_QSC6295*/:
  case 51/*HW_QSC6695*/:
    HAL_hsusb_ConstructImpl3r0(&HAL_HSUSB_CoreIfChipIdea);
    HAL_HSUSB_CoresObj.MaxCoresQty = 3;
    /* USB Core 0 */
    instance = 0;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore0[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_ULPI;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 1 */
    instance = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore1[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_FS_INTERCHIP;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    /* USB Core 2 */
    instance = 2;
    HAL_HSUSB_CoresObj.CoreInfo[instance].If = &HAL_HSUSB_CoreIfChipIdea;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo = &HAL_HSUSB_PortInfoCore2[0];
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortsQty = 1;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyInterfaceType = HAL_HSUSB_PHY_IF_HS_INTERCHIP;
    HAL_HSUSB_CoresObj.CoreInfo[instance].PortInfo[0].PhyType.PhyConnectionType = HAL_HSUSB_PHY_INTEGRATED;
    break;
  default:
    return;
  }
  for (instance=0; instance<HAL_HSUSB_CoresObj.MaxCoresQty; instance++)
  {
    if (usb_core_base[instance] == 0)
    {
      HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_NotSupported;
    }
    else
    {
      HAL_HSUSB_CoresObj.CoreInfo[instance].BaseAddress = usb_core_base[instance];
      HAL_HSUSB_CoresObj.CoreInfo[instance].LastError = HAL_HSUSB_Success;
    }
  }
}
