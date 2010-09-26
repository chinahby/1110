/**
 * @file usb_IF_ait_API.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT70x Series Host API.
 * @n The purpose of AIT70x Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include	"usb_IF_ait_api.h"
#include	"cam_IF_ait_api.h"

#ifndef _FW_FROM_PC_

#include	"AIT701_USBFW.h"
#else
extern u_char AIT701_msdc_fw[0x4000];
extern u_char AIT701_pccam_fw[0x3000];
#endif

#include	"AIT_Interface.h"
#include	"a800_camera.h"
#include	"a800_usb.h"

/**
 @fn		A8_ERROR_MSG	usb_IF_ait_open_usb(A8_USB_MODE mode)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
static char gAitUsbPcCamEnable = 0;
static char gAitUsbPCCamFlag = 0;
A8_ERROR_MSG usb_IF_ait_open_usb(A8_USB_MODE mode)
{
	A8_ERROR_MSG retVal=A8_USB_ERROR;
	
	if((mode != A8_USB_PCCAM)  && (mode != A8_USB_MSDC) && (mode != A8_USB_VCOM))
	{
		AIT_Message_P1("Incorrect usb mode = %d",mode);
		return A8_INCORRECT_PARA_ERROR;
	}

	AIT701_DP_PULLUP_DISABLE();

	sys_IF_ait_set_input_clock(A8_USB, A8_ON);
	
	if(A800_IsResetStatus() || (sys_IF_ait_get_status() == AIT_STATUS_DUMMY))
	{
		SetA8RegB(0x6903, 0xFF);
		SetA8RegB(0x6904, 0x1C);
		sys_IF_ait_delay1ms(1);
		SetA8RegW(0x690A, 0xFFFF);
		SetA8RegW(0x690A, 0x0000);
		sys_IF_ait_delay1ms(1);
		SetA8RegB(0x6511, 0x04 );
		SetA8RegB(0x6511, 0x00);
		sys_IF_ait_delay1ms(1);
		
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
	}

#if 1//defined(AIT701U_ZINNIA)
	sys_IF_ait_set_status(AIT_STATUS_DUMMY);
#endif


	AIT_Message_P1("usb_IF_ait_open_usb, Status=%d! \n", sys_IF_ait_get_status());
	if((sys_IF_ait_get_status() == AIT_STATUS_DUMMY) || (sys_IF_ait_get_status() < AIT_STATUS_USB_CLOSED))
	{
		AIT_Message_P0("usb_IF_ait_open_usb need to download FW\n\r");
		
		sys_IF_ait_reset_chip();
		AIT701_DP_PULLUP_DISABLE();

		A800_SetPllFreq(A8_USB, A8_ON);
 
		if (mode == A8_USB_MSDC)
		{
	
				retVal = A800_DownloadFirmware((u_char *)AIT701_msdc_fw, sizeof(AIT701_msdc_fw));
		
		}	
		else if(mode ==A8_USB_PCCAM)
		{
			retVal = A800_DownloadFirmware((u_char *)AIT701_pccam_fw, sizeof(AIT701_pccam_fw));
			gAitUsbPCCamFlag = 1;
		}	
		else
			return A8_UNSUPPORT_ERROR;		

		if(retVal != A8_NO_ERROR)
		{
			AIT_Message_P0("A800_DownloadFirmware is failed, and re-download fw again !!!\n");
			
			sys_IF_ait_reset_chip();
			AIT701_DP_PULLUP_DISABLE();

			A800_SetPllFreq(A8_USB, A8_ON);

			if (mode == A8_USB_MSDC)
			{
		
					retVal = A800_DownloadFirmware((u_char *)AIT701_msdc_fw, sizeof(AIT701_msdc_fw));

				
			}	
			else if(mode ==A8_USB_PCCAM)
			{
				retVal = A800_DownloadFirmware((u_char *)AIT701_pccam_fw, sizeof(AIT701_pccam_fw));
				gAitUsbPCCamFlag = 1;
			}	
			else
				return A8_UNSUPPORT_ERROR;
			
			if (retVal != A8_NO_ERROR)	// AIT`s FW is not ready!!
			{
				sys_IF_ait_set_status(AIT_STATUS_DUMMY);
				AIT_Message_P0("A800_DownloadFirmware is failed !!!\n");
				
				return	A8_DOWNLOAD_FW_ERROR;
			}
		}
	}
	else
	{
		retVal = A800_SetPllFreq(A8_USB, A8_ON);
		if (retVal != A8_NO_ERROR)
		{
			sys_IF_ait_set_status(AIT_STATUS_DUMMY);
			AIT_Message_P0("A800_SetPllFreq(A8_ON) is failed !!!\n");
			
			return	A8_SYSTEM_ERROR;
		}
	}
	
	if (mode == A8_USB_PCCAM)
	{
		SetA8RegB(0x692A, GetA8RegB(0x692A)&(~0x20) ); //disable uart mode, set to gpio mode, gpio 6 control sub-sensor enable
		cam_IF_ait_VIF_Src_Selection(0);
		cam_IF_ait_open_camera();
		gAitUsbPcCamEnable = 1;
		
	}

	AIT_Message_P0("usb_IF_ait_set_usb_mode ...\n\r");
	
	retVal = usb_IF_ait_set_usb_mode(mode);
	
//	sys_IF_ait_set_bypass_mode(A8_ON);
	
	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("usb_IF_ait_set_usb_mode is failed !!!\n");
		
		return	A8_TIMEOUT_ERROR;
	}
	else
	{
		sys_IF_ait_set_status(AIT_STATUS_USB_READY);
		
		return A8_NO_ERROR;
	}
}
//add zfn 20090505
bool slt_is_ait_busy(void)
{
	if(sys_IF_ait_get_status()>=AIT_STATUS_USB_READY)
		return TRUE;
	else
		return FALSE;
}
/**
 @fn		A8_ERROR_MSG	usb_IF_ait_set_usb_mode(A8_USB_MODE mode)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG usb_IF_ait_set_usb_mode(A8_USB_MODE mode)
{
	u_char retVal=A8_NO_ERROR;
	
	sys_IF_ait_usb_power(A8_ON);
	
	A800_SetUSBPllFreq(A8_ON);
	
	retVal = A800_SetUSBMode(mode);
#if 0	//test by ming @20090411	
	retVal = A800_CheckUSBConnectState();
#endif
	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_usb_power(A8_OFF);
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("A800_CheckUSBConnectState is failed !!!\n");
		
		return	A8_TIMEOUT_ERROR;
	}
	else
	{
		if(mode == A8_USB_MSDC)
			sys_IF_ait_set_status(AIT_STATUS_USB_MSDC);
		else if(mode == A8_USB_PCCAM) 
			sys_IF_ait_set_status(AIT_STATUS_USB_PCCAM);
		else	
			sys_IF_ait_set_status(AIT_STATUS_USB_VCOM);
		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG	usb_IF_ait_close_usb(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG usb_IF_ait_close_usb(void)
{
	u_char retVal;

	sys_IF_ait_set_input_clock(A8_USB, A8_ON);

	//sys_IF_ait_set_bypass_mode(A8_OFF);
#if 0
	if(0 /*(gAitUsbPcCamEnable*/)
	{
		A800_SensorPowerOn(A8_OFF);
		gAitUsbPcCamEnable = 0;
	}	
#endif		
	SetA8RegB(0x6902, (GetA8RegB(0x6902) & (~0x80)));
	
	retVal = A800_ExitUSBMode();

	A800_PowerSavingOn();
	AIT701_DP_PULLUP_STANDBY();
	A800_SetPllFreq(A8_ALL, A8_OFF);
	
	//sys_IF_ait_set_bypass_mode(A8_ON);

	sys_IF_ait_set_input_clock(A8_ALL, A8_OFF);

	sys_IF_ait_usb_power(A8_OFF);

	if (retVal != A8_NO_ERROR)
	{
		sys_IF_ait_set_status(AIT_STATUS_DUMMY);
		AIT_Message_P0("A800_ExitUSBMode is failed !!!\n");
		
		return	A8_SYSTEM_ERROR;
	}
	else
	{
		sys_IF_ait_set_status(AIT_STATUS_USB_CLOSED);
		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG	usb_IF_ait_check_connect_state(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG usb_IF_ait_check_connect_state(void)
{
	u_int retVal;

	sys_IF_ait_set_input_clock(A8_USB, A8_ON);

	//sys_IF_ait_set_bypass_mode(A8_OFF);

	retVal = A800_CheckUSBConnectState();
	
	//sys_IF_ait_set_bypass_mode(A8_ON);

	return retVal;
}
