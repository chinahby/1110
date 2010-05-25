/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

#ifndef __DEV_PROPS_H__
#define __DEV_PROPS_H__

#ifdef CONFIG_FD_SICD_APP_DEVPROPS

#define NUM_DEV_PROPS 31

typedef struct
{
    ptp_prop_value_t def;
    ptp_prop_value_t curr;
} devprop_map_elem_t;

extern devprop_map_elem_t devprop_value_map[NUM_DEV_PROPS];
extern ptp_dev_prop_desc_t devprop_desc_map[NUM_DEV_PROPS];

void devprops_init(void);
void devprops_free(void);

#endif /* CONFIG_FD_SICD_APP_DEVPROPS */

#endif /*__DEV_PROPS_H__*/
