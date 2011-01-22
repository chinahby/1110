#ifndef	A800_USB_H
#define	A800_USB_H
#include "A8_usb.h"
#include "A8_usbdscr.h"

extern u_short	A800_SetUSBMode(u_short usbMode);
extern u_short	A800_ExitUSBMode(void);
extern u_short	A800_SetUSBPllFreq(u_char on);
extern u_short A800_CheckUSBConnectState(void);
#endif