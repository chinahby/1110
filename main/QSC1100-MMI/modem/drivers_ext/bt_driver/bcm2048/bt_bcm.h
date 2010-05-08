#ifndef __BT_BCM_H_
#define __BT_BCM_H_
/*===========================================================================
        
                    BT Driver for BCM Serial Chipset

DESCRIPTION
  All the declarations and definitions necessary to support interaction
  with the FM Radio functions for the MSM60x0.

REFERENCES

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "gpio_60x0.h"
#include "bio.h"
#include "clk.h"
#include "msg.h"
#include "gpio_int.h"
#include "sleep.h"

/*===========================================================================

FUNCTION hs_init_bt_host_ctrl

DESCRIPTION
  This function init the GPIO pads which use to control BT device
  
PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_init_bt_host_ctrl(void);

/*===========================================================================

FUNCTION hs_open_bt_device

DESCRIPTION
  This function control the GPIO to open/close power supply to BT device
  
PARAMETERS
  boolean bIsOpen [IN]: TRUE = Open, FALSE = CLOSE.
  
DEPENDENCIES
  hs_init_bt_host_ctrl must be called before.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_open_bt_device(boolean bIsOpen);

/*===========================================================================

FUNCTION hs_bt_wakeup

DESCRIPTION
  This function control the GPIO to open/close power supply to BT device
  
PARAMETERS
  boolean bIsWakeup [IN]: TRUE = Wake Up Device, FALSE = Make Device Sleep.
  
DEPENDENCIES
  hs_init_bt_host_ctrl must be called before.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void hs_bt_wakeup(boolean bIsWakeup);

/*===========================================================================

FUNCTION btem_host_wakeup_int_enable

DESCRIPTION
  This function control the GPIO to open/close power supply to BT device
  
PARAMETERS
  boolean enable [IN]: TRUE = Enable, FALSE = Disable.
  
DEPENDENCIES
  hs_init_bt_host_ctrl must be called before.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void btem_host_wakeup_int_enable(boolean enable);

/*===========================================================================

FUNCTION btui_get_bt_addr

DESCRIPTION
  This function get the BT address from Host EFS
  
PARAMETERS
  byte* my_bd_addr [IO]: ptr to address buffer
  int   bd_size    [IN]: buffer size 
  
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean btui_get_bt_addr(byte *my_bd_addr,int bd_size);

#endif //__BT_BCM_H_