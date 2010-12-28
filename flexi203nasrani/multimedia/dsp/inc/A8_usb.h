#ifndef	A8_USB_H
#define	A8_USB_H
#include	"A8_common.h"

extern u_short A8L_SetKernalUSBMode(u_short usbMode);
extern u_short A8L_SetMSDCHandshakeMemory(u_int handShakeBufAddr);
extern u_short A8L_SetMSDCRxMemory(u_int rx_buf_start, u_int rx_buf_size);
extern u_short A8L_SetMSDCInterfaceMemory(u_int interface_buf_start, u_int interface_buf_size);
extern u_short A8L_SetUSBBaseMemory(u_int UsbAddrStart);
extern u_short A8L_SetCDCHandshakeMemory(u_int handShakeBufAddr);
extern u_short A8L_SetCDCTxMemory(u_int txBufAddr, u_int txBufAddrSize);
extern u_short A8L_SetCDCRxMemory(u_int rxBufAddr, u_int rxBufAddrSize);
#endif