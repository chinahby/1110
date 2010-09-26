#include "A8_usb.h"
#include "AIT700_EBIbus.h"
#include "a8_common.h"
#include "A8_dscapi.h"

u_int	glMSDCHandshakeMemory,glMSDCStatusMemory;
u_int	glMSDCRxBufAddr,glMSDCRxBufSize;
u_int	glMSDCInterfaceBufAddr,glMSDCInterfaceBufSize;
u_int	glCDCHandshakeMemory,glCDCStatusMemory;
//u_int	glCDCRxBufAddr,glCDCRxBufSize;
u_int	glCDCTxMemory,glCDCTxMemorySize;
u_int	glCDCRxMemory,glCDCRxMemorySize;
u_int	glCDCInterfaceBufAddr,glCDCInterfaceBufSize;

//======================================================================//
// function : A800_SetUSBPllFreq                                    //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short	A8L_SetUSBPllFreq(u_char X)
{
	u_short Val=0, Tmp=0,retVal = 0;
	volatile s_int timeout, Maxtimeout=1000;

	switch(X)
	{
		case 0:	//Bypass PLL
			Val = GetA8RegB( 0x6905 );
			Val = Val & 0xFD;
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6905, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6905 );
				
				timeout++;
			}while ((Tmp & 0x02) != 0x00 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
			{
				AIT_Message_Error("A8L_SetUSBPllFreq Error:%d", __LINE__);
				return A8_USB_DLL_ERROR;
			}
			// Power Down PLL
			Val = GetA8RegB( 0x6913 );
			Val = Val & 0xFE;
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6913, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6913 );

				timeout++;
			}while ((Tmp & 0x01) != 0x00 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
			{
				AIT_Message_Error("A8L_SetUSBPllFreq Error:%d", __LINE__);
				return A8_USB_DLL_ERROR;
			}
			
			SetA8RegB(0x6904, GetA8RegB(0x6904) | 0x60);
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			SetA8RegB(0x6904, GetA8RegB(0x6904) & (~0x60));	//Disable PHY clk
			sys_IF_ait_delay1ms(1);

			//power down and reset UPLL
			Val = GetA8RegB( 0x6913 );
			Val = Val | 0x03;	

			timeout = 0;
			do
			{
				SetA8RegB( 0x6913, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6913 );

				timeout++;
			}while ( (Tmp & 0x01) != 0x01 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
			{
				AIT_Message_Error("A8L_SetUSBPllFreq Error:%d", __LINE__);
				return A8_USB_DLL_ERROR;
			}

			sys_IF_ait_delay1ms(2);

#ifdef EXTCLK_26M
			SetA8RegB(0x6971,0x07);	//M=7

			SetA8RegB(0x6972,0x4e);	//0xec42/d'262144 = 0.23076629638671875
			SetA8RegB(0x6973,0xec);
			SetA8RegB(0x6974,0x00);  //26*(7.230766+2) = 240
#elif defined(EXTCLK_19M2)
			SetA8RegB(0x6971,0x10);
			SetA8RegB(0x6972,0xBC);
			SetA8RegB(0x6973,0x86);
			SetA8RegB(0x6974,0x02);		// 18+

//60*4 = 240
//240/19.2 = 12.5
//
			SetA8RegB(0x6971,0xA);
			SetA8RegB(0x6972,0x00);
			SetA8RegB(0x6973,0x00);
			SetA8RegB(0x6974,0x02);		// 18+
#endif

			SetA8RegB(0x6913, GetA8RegB(0x6913) & (~0x04UL));		//Setting M/N to UPLL
			SetA8RegB(0x6913, GetA8RegB(0x6913) | 0x04);
			SetA8RegB(0x6913, GetA8RegB(0x6913) & (~0x04UL));	
			
			SetA8RegB(0x6978,0x00);	//UPLL clock output 1 as USB PHY src clock

			// Select PLL path
			Val = GetA8RegB( 0x6905 );
			Val = Val | 0x02;	//UPLL Clock enable
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6905, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6905 );
				
				timeout++;
			}while ( (Tmp & 0x02) != 0x2 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
			{
				AIT_Message_Error("A8L_SetUSBPllFreq Error:%d", __LINE__);
				return A8_USB_DLL_ERROR;
			}
			break;
		default:
			break;
	}

	sys_IF_ait_delay1ms(1);
	
	if ( retVal )
		return A8_SYSTEM_ERROR;
	else
		return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetKernalUSBMode                                    //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetKernalUSBMode(u_short usbMode)
{
	u_short	retVal;

    retVal = SendA8Cmd((usbMode << 8) + A8_HOST_CMD_SET_USB_CHANGE_MODE);
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetPCCamCompressMemory                              //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetPCCamCompressMemory(u_int compbuf_start, u_int compbuf_size)
{
	u_short	retVal;
	
    SetA8RegW(0x6540, (u_short) compbuf_start);
    SetA8RegW(0x6542, (u_short) (compbuf_start >> 16));
    SetA8RegW(0x6544, (u_short) compbuf_size);
    SetA8RegW(0x6546, (u_short) (compbuf_size >> 16));

    retVal = SendA8Cmd(PCCAM_COMPRESS_BUF | A8_HOST_CMD_PCCAM_MEMORY);                 
    if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetPCCamLineBufMemory                               //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetPCCamLineBufMemory(u_int line_buf_start)
{
	u_short	retVal;
	
    SetA8RegW(0x6540, (u_short) line_buf_start);
    SetA8RegW(0x6542, (u_short) (line_buf_start >> 16));

   retVal = SendA8Cmd(PCCAM_LINE_BUF | A8_HOST_CMD_PCCAM_MEMORY);                 
    if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetMSDCHandshakeMemory                              //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetMSDCHandshakeMemory(u_int handShakeBufAddr)
{
	u_short	retVal;
	
    glMSDCHandshakeMemory = handShakeBufAddr;
    glMSDCStatusMemory = glMSDCHandshakeMemory + 12;

    SetA8MemW(glMSDCStatusMemory, 0);
    SetA8MemW(glMSDCStatusMemory + 2, 0);

    // init in buffer addr in 81x
    SetA8RegW(0x6540, (u_short) handShakeBufAddr);
    SetA8RegW(0x6542, (u_short) (handShakeBufAddr >> 16));

    retVal = SendA8Cmd(HANDSHAKE_BUFFER_ADDR | A8_HOST_CMD_MSDC_MEMORY);               
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetMSDCRxMemory                      //
// parameters: none                                      //
// return : 0: success others:error code                       //
// description : Start video capture for the previously set          //
//             configuration                             //
//======================================================================//
u_short A8L_SetMSDCRxMemory(u_int rx_buf_start, u_int rx_buf_size)
{
	u_short	retVal;
	
    glMSDCRxBufAddr = rx_buf_start;
    glMSDCRxBufSize = rx_buf_size;

    SetA8RegW(0x6540, (u_short) rx_buf_start);
    SetA8RegW(0x6542, (u_short) (rx_buf_start >> 16));
    SetA8RegW(0x6544, (u_short) rx_buf_size);
    SetA8RegW(0x6546, (u_short) (rx_buf_size >> 16));

    retVal = SendA8Cmd(RX_BUF | A8_HOST_CMD_MSDC_MEMORY);               
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetMSDCInterfaceMemory                              //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetMSDCInterfaceMemory(u_int interface_buf_start, u_int interface_buf_size)
{
	u_short	retVal;

    glMSDCInterfaceBufAddr = interface_buf_start;
    glMSDCInterfaceBufSize = interface_buf_size;

    SetA8RegW(0x6540, (u_short) interface_buf_start);
    SetA8RegW(0x6542, (u_short) (interface_buf_start >> 16));
    SetA8RegW(0x6544, (u_short) interface_buf_size);
    SetA8RegW(0x6546, (u_short) (interface_buf_size >> 16));

    retVal = SendA8Cmd(INTERFACE_BUF | A8_HOST_CMD_MSDC_MEMORY);               
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}

u_short A8L_SetUSBBaseMemory(u_int UsbAddrStart)
{
	u_short	retVal;

    // init in buffer addr in 81x
    SetA8RegW(0x6540, (u_short) UsbAddrStart);
    SetA8RegW(0x6542, (u_short) (UsbAddrStart >> 16));
    retVal = SendA8Cmd(A8_HOST_CMD_USBBASE_MEMORY);
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetCDCHandshakeMemory                            //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetCDCHandshakeMemory(u_int handShakeBufAddr)
{
	u_short	retVal;

    glCDCHandshakeMemory = handShakeBufAddr;
    glCDCStatusMemory = glCDCHandshakeMemory + 4;

    SetA8MemW(glCDCStatusMemory, 0);
    SetA8MemW(glCDCStatusMemory + 2, 0);

    // init in buffer addr in 81x
    SetA8RegW(0x6540, (u_short) handShakeBufAddr);
    SetA8RegW(0x6542, (u_short) (handShakeBufAddr >> 16));

    retVal = SendA8Cmd(HANDSHAKE_BUFFER_ADDR | A8_HOST_CMD_CDC_MEMORY);
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetCDCTxMemory                               //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetCDCTxMemory(u_int txBufAddr, u_int txBufAddrSize)
{
	u_short	retVal;
    glCDCTxMemory = txBufAddr;
    glCDCTxMemorySize = txBufAddrSize;

    // init tx buffer addr
    SetA8RegW(0x6540, (u_short) txBufAddr);
    SetA8RegW(0x6542, (u_short) (txBufAddr >> 16));

    retVal = SendA8Cmd(CDC_TX_BUFFER_ADDR | A8_HOST_CMD_CDC_MEMORY);               
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}
//======================================================================//
// function : A810H_SetCDCRxMemory                              //
// parameters:  none                                                    //
// return : 0: success others:error code                                //
// description : Start video capture for the previously set             //
//                  configuration                                       //
//======================================================================//
u_short A8L_SetCDCRxMemory(u_int rxBufAddr, u_int rxBufAddrSize)
{
	u_short	retVal;

    glCDCRxMemory = rxBufAddr;
    glCDCRxMemorySize = rxBufAddrSize;

    // init rx buffer addr 
    SetA8RegW(0x6540, (u_short) rxBufAddr);
    SetA8RegW(0x6542, (u_short) (rxBufAddr >> 16));

    retVal = SendA8Cmd(CDC_RX_BUFFER_ADDR | A8_HOST_CMD_CDC_MEMORY);                  
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}
