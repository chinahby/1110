/**
 * @file cam_IF_ait_API.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT70x Series Host API.
 * @n The purpose of AIT70x Host API is for Host CPU to control and communicate AIT701
 *      chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
 */
#include "a800_camera.h" 
#include "a800_sd.h"
#include "a8_sd.h"

#include "sys_IF_ait_api.h"
#include "sd_IF_ait_api.h"

/**
 @fn		A8_ERROR_MSG sd_IF_ait_open_sd(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG sd_IF_ait_open_sd(void)
{
	u_char retVal;
	sys_IF_ait_set_input_clock(A8_SD, A8_ON);

	retVal = A800_SetPllFreq(A8_SD, A8_ON);
	SetA8RegB(0x6960, (GetA8RegB(0x6960) | 0x70)); // 20080614, // 2T SRAM buffer memory Enable!
	if (retVal != A8_NO_ERROR)	{
		AIT_Message_P0("A800_SetPllFreq(A8_ON) is failed !!!\n");		
		return A8_SD_ERROR;
	}else{		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG sd_IF_ait_close_sd(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG sd_IF_ait_close_sd(void)
{
	u_char retVal;

	sys_IF_ait_set_input_clock(A8_SD, A8_ON);
	retVal = A800_SetPllFreq(A8_SD, A8_OFF);
	sys_IF_ait_set_input_clock(A8_SD, A8_OFF);

	if (retVal != A8_NO_ERROR)	{
		AIT_Message_P0("A800_SetPllFreq(A8_OFF) is failed !!!\n");		
		return A8_SD_ERROR;
	}else{		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG sd_IF_ait_init_sd(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG sd_IF_ait_init_sd(void)
{
	u_char retVal = A8_NO_ERROR;

	sys_IF_ait_set_input_clock(A8_SD, A8_ON);
	retVal = A800_CheckSDDevice();
	if (retVal){
		AIT_Message_P0("A800_CheckSDDevice is failed !!!\n");		
		return A8_SD_ERROR;
	}else{		
		AIT_Message_P0("A800_CheckSDDevice is sucess !!!\n");	
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG sd_IF_ait_write_sectors(u_int startsect, u_int offset, u_char* buf, u_int read_size)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG sd_IF_ait_write_sectors(u_int startsect, u_int offset, u_char* buf, u_int write_size)
{
	u_char retVal;

	sys_IF_ait_set_input_clock(A8_SD, A8_ON);
	retVal = A800_WriteSD(startsect, offset, buf, write_size);

	if(retVal){
		AIT_Message_P0("1st A800_WriteSD is failed !!!\n");		
		retVal = A800_CheckSDDevice();
		if(!retVal)
			retVal = A800_WriteSD(startsect, offset, buf, write_size);
	}

	if (retVal){
		AIT_Message_P0("2nd A800_WriteSD is failed !!!\n");		
		return A8_SD_ERROR;
	}else{
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG sd_IF_ait_read_sectors(u_int startsect, u_int offset, u_char* buf, u_int read_size)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
A8_ERROR_MSG sd_IF_ait_read_sectors(u_int startsect, u_int offset, u_char* buf, u_int read_size)
{
	u_char retVal;
	sys_IF_ait_set_input_clock(A8_SD, A8_ON);

	retVal = A800_ReadSD(startsect, offset, buf, read_size);

	if(retVal){
		AIT_Message_P0("1st A800_ReadSD is failed !!!\n");		
		retVal = A800_CheckSDDevice();
		if(!retVal)
			retVal = A800_ReadSD(startsect, offset, buf, read_size);
	}

	if (retVal){
		AIT_Message_P0("2nd A800_ReadSD is failed !!!\n");		
		return A8_SD_ERROR;
	}else{		
		return A8_NO_ERROR;
	}
}

/**
 @fn		A8_ERROR_MSG	sd_IF_ait_ioctl(void)
 @brief	
 @return	A8_ERROR_MSG
 @note  	TBD
 @bug	N/A.
*/
u_int sd_IF_ait_ioctl(void)
{
	u_char retVal;
	u_char isBypassMode = sys_IF_ait_get_bypass_status();
	
	sys_IF_ait_set_input_clock(A8_SD, A8_ON);

	if (isBypassMode == TRUE)
		sys_IF_ait_set_bypass_mode(A8_OFF);
	
	retVal = A800_GetSDSize();
	
	if (isBypassMode == TRUE)
		sys_IF_ait_set_bypass_mode(A8_ON);
	
	return retVal;
}

u_char* sd_IF_ait_get_CSD(void)
{
	return	(u_char*)A8L_GetSDCSDValue();	
}
