#include "A800_usb.h"
#include "AIT700_ebibus.h"
extern void sys_IF_ait_delay1ms(u_int time);
extern u_short A8L_SetUSBPllFreq(u_char X);
/**
 @fn		u_short	A800_SetUSBPllFreq(u_char on)
 @brief	
 @param	
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short	A800_SetUSBPllFreq(u_char on)
{
	u_short ret;
	if(A8_ON == on)
	{
		ret = A8L_SetUSBPllFreq(2);
	}
	else
	{
		ret = A8L_SetUSBPllFreq(0);
	}
	if(ret)
	{
		AIT_Message_Error("A800_SetUSBPllFreq Err: %d", ret);
		return ret;
	}
	
	return A8_NO_ERROR;
}

/**
 @fn		u_short	A800_SetUSBMode(u_short usbMode)
 @brief	Init USB Mode.
 @param	usbMode (1:MSDC, 4:VCOM).
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short	A800_SetUSBMode(u_short mode)
{
	u_int	UsbBaseAddr;
	u_short	i;
	u_short retVal=0;

	extern u_char  INQUIRY_TABLE_START[1][42];
	
	extern u_short A8L_SetPCCamCompressMemory(u_int compbuf_start, u_int compbuf_size);
	extern u_short A8L_SetPCCamLineBufMemory(u_int line_buf_start);
	
	switch(mode)
	{
		case	A8_USB_PCCAM:
			A8L_SetPCCamCompressMemory(0x0000, 0x13000);
			A8L_SetPCCamLineBufMemory(0x13000);
    		
    		UsbBaseAddr = 0;
			
	    	for(i=0;i<0x0a;i++)
    			SetA8MemB(UsbBaseAddr++,DEVICE_QUALIFIER_DESCRIPTOR_DATA1[i]);
    		for(i=0;i<0x12;i++)
    			SetA8MemB(UsbBaseAddr++,DEVICE_DESCRIPTOR_DATA1[i]);
	    	for(i=0;i<0x20;i++)
    			SetA8MemB(UsbBaseAddr++,CONFIG_DESCRIPTOR_DATA1[i]);
    		for(i=0;i<0x04;i++)
    			SetA8MemB(UsbBaseAddr++,LANGUAGE_ID_DATA[i]);
	    	for(i=0;i<0x1e;i++)
    			SetA8MemB(UsbBaseAddr++,MANUFACTURER_STRING_DATA[i]);
    		for(i=0;i<0x1e;i++)
    			SetA8MemB(UsbBaseAddr++,PRODUCT_STRING_DATA[i]);
	    	for(i=0;i<0x1a;i++)
    			SetA8MemB(UsbBaseAddr++,SERIALNUMBER_STRING_DATA[i]);
			break;

			
		case	A8_USB_MSDC:
			retVal = A8L_SetMSDCHandshakeMemory(0x08000);
			if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
			{
				//sys_IF_ait_set_status(AIT_STATUS_DUMMY);
				AIT_Message_P0("[USB] FW Not Ready is failed !!!\n");

				AIT_Message_P1("A8L_SetMSDCHandshakeMemory is failed  = %d!!!\n",retVal);
				
				return	A8_USB_ERROR;
			}

			retVal = A8L_SetMSDCRxMemory(0x0400, 0x2000);
			if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
			{
				//sys_IF_ait_set_status(AIT_STATUS_DUMMY);
				AIT_Message_P1("A8L_SetMSDCRxMemory is failed  = %d !!!\n",retVal);
				
				return	A8_USB_ERROR;
			}

			retVal = A8L_SetMSDCInterfaceMemory(0x2400, 0x2000);
			if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
			{
				//sys_IF_ait_set_status(AIT_STATUS_DUMMY);
				AIT_Message_P1("A8L_SetMSDCInterfaceMemory is failed   = %d!!!\n",retVal);
				
				return	A8_USB_ERROR;
			}
    		
    		UsbBaseAddr = 0;
			
	    	for(i=0;i<0x0a;i++)
    			SetA8MemB(UsbBaseAddr++,DEVICE_QUALIFIER_DESCRIPTOR_DATA2[i]);
    		for(i=0;i<0x12;i++)
    			SetA8MemB(UsbBaseAddr++,DEVICE_DESCRIPTOR_DATA2[i]);
	    	for(i=0;i<0x20;i++)
    			SetA8MemB(UsbBaseAddr++,CONFIG_DESCRIPTOR_DATA2[i]);
    		for(i=0;i<0x04;i++)
    			SetA8MemB(UsbBaseAddr++,LANGUAGE_ID_DATA[i]);
	    	for(i=0;i<0x1e;i++)
    			SetA8MemB(UsbBaseAddr++,MANUFACTURER_STRING_DATA[i]);
    		for(i=0;i<0x1e;i++)
    			SetA8MemB(UsbBaseAddr++,PRODUCT_STRING_DATA[i]);
	    	for(i=0;i<0x1a;i++)
    			SetA8MemB(UsbBaseAddr++,SERIALNUMBER_STRING_DATA[i]);

			UsbBaseAddr = 0x100;
	    	for(i=0;i<42;i++)
    			SetA8MemB(UsbBaseAddr++,INQUIRY_TABLE_START[0][i]);

			break;
		case	A8_USB_VCOM:
			A8L_SetCDCHandshakeMemory(0x8000);
			A8L_SetCDCTxMemory(0x400, 0x4000);
			A8L_SetCDCRxMemory(0x4400, 0x200);
    		
    		UsbBaseAddr = 0;
    		
	    	for(i=0;i<0x0a;i++)
    			SetA8MemB(UsbBaseAddr++,DEVICE_QUALIFIER_DESCRIPTOR_DATA5[i]);
    		for(i=0;i<0x12;i++)
    			SetA8MemB(UsbBaseAddr++,DEVICE_DESCRIPTOR_DATA5[i]);
	    	for(i=0;i<0x43;i++)
    			SetA8MemB(UsbBaseAddr++,CONFIG_DESCRIPTOR_DATA5[i]);
    		for(i=0;i<0x04;i++)
    			SetA8MemB(UsbBaseAddr++,LANGUAGE_ID_DATA[i]);
	    	for(i=0;i<0x1e;i++)
    			SetA8MemB(UsbBaseAddr++,MANUFACTURER_STRING_DATA[i]);
    		for(i=0;i<0x1e;i++)
    			SetA8MemB(UsbBaseAddr++,PRODUCT_STRING_DATA[i]);
	    	for(i=0;i<0x1a;i++)
    			SetA8MemB(UsbBaseAddr++,SERIALNUMBER_STRING_DATA[i]);
    		break;
		default:
			return A8_OUT_OF_RANGE_ERROR;
	}
	
	retVal = A8L_SetUSBBaseMemory(0x0);
	if(retVal != 0){
		AIT_Message_P1("A8L_SetUSBBaseMemory is failed = %d !!!\n",retVal);
		return retVal;
	}
	
	A8L_SetKernalUSBMode(mode);
	if(retVal != 0){
		AIT_Message_P1("A8L_SetKernalUSBMode is failed !!! = %d\n",retVal);
	}	
	return A8_NO_ERROR;
}

/**
 @fn		u_short	A800_ExitUSBMode(void)
 @brief	Exit USB Mode.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short	A800_ExitUSBMode(void)
{
	//SetA8RegB(0x4c41,GetA8RegB(0x4c41) | 0x80);
	//SetA8RegB(0x4c40,GetA8RegB(0x4c40) & ~0x80);

	//SetA8RegB(0x4c50,GetA8RegB(0x4c50) | 0x40);
	//SetA8RegB(0x4c51,GetA8RegB(0x4c51) & ~0x08);

	//SetA8RegB(0x4401, GetA8RegB(0x4401) | 0x01);
	//SetA8RegB(0x440B, GetA8RegB(0x440B) | 0x01);
	SetA8RegB(0x6904,GetA8RegB(0x6904)&~ 0x60);
	sys_IF_ait_delay1ms(3);

	SetA8RegB(0x440F, 0x00);
	sys_IF_ait_delay1ms(3);
	
	SetA8RegB(0x440F, 0x10);
	sys_IF_ait_delay1ms(3);;
	
	SetA8RegB(0x440F, 0x18);
	sys_IF_ait_delay1ms(3);

	SetA8RegB(0x4401, 0x01);
	SetA8RegB(0x440B, 0x01);
	sys_IF_ait_delay1ms(3);
	SetA8RegB(0x6904,GetA8RegB(0x6904) |0x60);
   
	//SetA8RegB(0x440B,0x01); //1205
	//SetA8RegB(0x4441,(GetA8RegB(0x4441) | 0x80)); //D+, D- pull down.
	//SetA8RegB(0x4440,(GetA8RegB(0x4440) & 0x7F)); //PHY
	A800_SetUSBPllFreq(A8_OFF);
	return A8_NO_ERROR;
}

/**
 @fn		u_short	A800_CheckUSBConnectState(void)
 @brief	.
 @return	u_short
 @note	.
 @bug	N/A.
*/
u_short A800_CheckUSBConnectState(void)
{
	u_int timeout;

	SetA8RegB(0x440B, GetA8RegB(0x440B) | 0x08);

	timeout = 0;
	while (((GetA8RegB(0x440A) & 0x08) != 0x08) && (timeout < 0x400))
	{
		sys_IF_ait_delay1ms(1);
		timeout++;
	}

	SetA8RegB(0x440B, GetA8RegB(0x440B) & 0xF7);
	
	if (timeout >= 0x400)
	{
		AIT_Message_P0("USB is disconnected\r\n");

		return A8_TIMEOUT_ERROR;
	}

	AIT_Message_P0("USB is connected\r\n");

	return A8_NO_ERROR;
}
