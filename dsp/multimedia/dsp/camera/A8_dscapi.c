/**
 * @file A8_dscapi.c
 * @brief Copyright (C) 2002-2008 Alpha Imaging Technology Corp.
 * @n AIT701/703 Series Host API.
 * @n The purpose of AIT701 Host API is for Host CPU to control and communicate AIT701
*	chip via direct/indirect parallel interface.
 * @note N/A
 * @bug N/A
*/
#include "AIT700_ebibus.h"
#include "A8_common.h"
#include "A8_win.h"
#include "A8_graphics.h"
#include "A8_panel.h"
#include "A8_dscapi.h"
#include "sys_IF_ait_api.h"
#include "AIT_interface.h"

#if	defined (AIT703_SEC) || defined (AIT702_SEC)
#define  MaxAITDecodeJpegWidth  1280
#else
#define  MaxAITDecodeJpegWidth  (480)
#endif

#define MAX_JPEG_SIZE				(0x100000)

u_int gA8JpegDecodeRowBufAddr = A8_JPEG_DECODE_ROW_BUF_ADDR;
u_int gA8JpegDecodeCompBufStartAddr = A8_JPEG_DECODE_COMP_BUF_START_ADDR;
u_int gA8JpegDecodeCompBufEndAddr = A8_JPEG_DECODE_COMP_BUF_END_ADDR;
u_int gA8JpegDecodeFifoBufSize = A8_JPEG_DECODE_FIFO_BUF_SIZE;

#ifndef EXIFSUPPORT
// These code depends on Customer!
u_char gMakeValue[14] =	{"AIT_701"};
u_char gModelValue[9]=	{"701/703"};	
u_char gDateTimeInfo[20] = {"2009.12.02 22:00:00"};
#endif

#ifdef EXIFSUPPORT
u_char	DateTimePtr[0x14] = {
	'2','0','0','6','.','1','1','.','0','8',' ','1','1',':','4','8',':','1','0', 0x00
};
extern u_short gA8PrevZoomFactor; //080430
u_char gCurrentCaptureResolution = 0;
s_short sEXIFEVBias = 0;
u_char EXIF_Header_Table[0x314] = {
/* (4,2,2) file table */
/* APP1 Exif */
	0xFF, 0xD8, 0xFF, 0xE1, 0x03, 0x10, 0x45, 0x78, 0x69, 0x66, 0x00, 0x00, 			
/*===================================================================================================	*/
/* offset = 0*/
/* TIFF Header	*/
	0x49, 0x49, 0x2A, 0x00, 0x08, 0x00, 0x00, 0x00,	
/*===================================================================================================*/
/* 0th IFD	*/
/* Number of Interoperability*/
	0x0B, 0x00,
	0x0E, 0x01, 0x02, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x92, 0x00, 0x00, 0x00,	
	0x0F, 0x01, 0x02, 0x00, 0x0E, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00,	
	0x10, 0x01, 0x02, 0x00, 0x0A, 0x00, 0x00, 0x00, 0xAE, 0x00, 0x00, 0x00,
	0x12, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,	
	0x1A, 0x01, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0xB8, 0x00, 0x00, 0x00,
	0x1B, 0x01, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00,	
	0x28, 0x01, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,	
	0x32, 0x01, 0x02, 0x00, 0x14, 0x00, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00,	
	0x13, 0x02, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,	
	0x98, 0x82, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0xDC, 0x00, 0x00, 0x00,	
	0x69, 0x87, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0xEC, 0x00, 0x00, 0x00,	
	0x00, 0x00, 0x00, 0x00,		/* Thumbnail offset */
/*===================================================================================================*/
/* offset = 0x92*/
	'E','x','i','f','_','J','P','E','G','_','4','2','2', 0x00,
	'A','I','T',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', 0x00,     
	'A','I','T','7','0','X',' ',' ',' ', 0x00,       
	0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x48, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	'2','0','0','4',':','0','4',':','2','0',' ','1','2',':','0','0',':','0','0', 0x00,
	'C','o','p','y','r','i','g','h','t',',','2','0','0','6', 0x00, 0x00,
/*===================================================================================================*/
/* Exif IFD*/
/* Exif IFD Number*/
	0x1B, 0x00,
	0x9A, 0x82, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x36, 0x02, 0x00, 0x00,	
	0x9D, 0x82, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3E, 0x02, 0x00, 0x00,	
	0x00, 0x90, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, '0','2','2','0',
	0x03, 0x90, 0x02, 0x00, 0x14, 0x00, 0x00, 0x00, 0x46, 0x02, 0x00, 0x00,	
	0x04, 0x90, 0x02, 0x00, 0x14, 0x00, 0x00, 0x00, 0x5A, 0x02, 0x00, 0x00,			
	0x01, 0x91, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x00,
	0x02, 0x91, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x6E, 0x02, 0x00, 0x00,
	0x01, 0x92, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x76, 0x02, 0x00, 0x00,	
	0x02, 0x92, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x7E, 0x02, 0x00, 0x00, 	
	0x03, 0x92, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x86, 0x02, 0x00, 0x00,	
	0x04, 0x92, 0x0A, 0x00, 0x01, 0x00, 0x00, 0x00, 0x8E, 0x02, 0x00, 0x00,	
	0x05, 0x92, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x96, 0x02, 0x00, 0x00,	
	0x06, 0x92, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x9E, 0x02, 0x00, 0x00,	
	0x07, 0x92, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,	
	0x22, 0x88, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,	
	0x09, 0x92, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	
	0x0A, 0x92, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0xA6, 0x02, 0x00, 0x00,	
	0x86, 0x92, 0x07, 0x00, 0x2E, 0x00, 0x00, 0x00, 0xAE, 0x02, 0x00, 0x00,	
	0x90, 0x92, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, '0','0','0', 0x00,	
	0x91, 0x92, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, '0','0','0', 0x00,		
	0x92, 0x92, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, '0','0','0', 0x00,		
	0x00, 0xA0, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00, '0','1','0','0',
	0x01, 0xA0, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x02, 0xA0, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00,
	0x03, 0xA0, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00,	
	0x04, 0xA4, 0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0xDC, 0x02, 0x00, 0x00,
	0x03, 0xA4, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,																							
/*===================================================================================================*/
/* VALUE Exif IFD value longer than 4 bytes 	*/
	0x01, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	'2','0','0','3',':','1','1',':','0','1',' ','1','2',':','0','0',':','0','0', 0x00,
	'2','0','0','3',':','1','1',':','0','1',' ','1','2',':','0','0',':','0','0', 0x00,
	0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,				
	0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
	0x0F, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00,	
	0x32, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
/* UserComment Value(46 bytes)	*/
	0x41, 0x53, 0x43, 0x49, 0x49, 0x00, 0x00, 0x00,				
	0x53, 0x65, 0x6C, 0x66, 0x54, 0x69, 0x6D, 0x65, 0x72, 0x20,		
	0x4F, 0x66, 0x66, 0x00, 0x00, 0x00,					
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* DigitalZoomRatio 2C4 */
	0x64, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 						
/*===================================================================================================*/	
};
#endif
/**
 @fn		s_short A8L_SetCamMode( u_short CamMode )
 @brief	Set AIT chip Operation mode. For different State Machine.
 @param	CamMode - 0:Preview, 1:Playback, 2 ... .
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A8L_SetCamMode( u_short CamMode )
{
	s_short retVal;
	retVal = SendA8Cmd( (CamMode << 8) + A8_HOST_CMD_SET_CAM_MODE );
	if (retVal != 0 )
	   return retVal;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
	else
		return A8_NO_ERROR; 
}

/**
 @fn		s_short A8L_TakeJpegPicture( s_short PictSize, s_short ZoomRate )
 @brief	Capture Command.
 @param	PictSize - 0:1.3M, 1:VGA, 2:QVGA, 3:QQVGA...
 @param	ZoomRate - From 0~10.
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A8L_TakeJpegPicture( s_short PictSize, s_short ZoomRate, u_char Re_Flag )
{
   SetA8RegB( 0x6242, 0x03 );		// Clear JPEG frame encode done status
   SetA8RegW( 0x6222, 0x0000 );	// Clear JPEG FIFO

//   if ( PictSize < 1 )
//      PictSize = 1;
#ifdef EXIFSUPPORT
	gCurrentCaptureResolution = PictSize;
#endif
   SendA8Cmd( (Re_Flag<<12) +(PictSize << 8) + A8_HOST_CMD_TAKE_JPEG );
   return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_TakeContinueJpegPicture( s_short PictSize, s_short ZoomRate )
 @brief	Capture Command. (Not Used!)
 @param	PictSize - 0:1.3M, 1:VGA, 2:QVGA, 3:QQVGA...
 @param	ZoomRate - From 0~10.(depends on customer)
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A8L_TakeContinueJpegPicture( s_short PictSize, s_short ZoomRate )
{
	SetA8RegB( 0x6242, 0x03 );		// Clear JPEG frame encode done status
	SetA8RegW( 0x6222, 0x0000 );	// Clear JPEG FIFO
	if ( ZoomRate < 0 )
		ZoomRate = 0;
	else if ( ZoomRate > 4 )
		ZoomRate = 4;
	if ( PictSize < 1 )
		PictSize = 1;
#ifdef EXIFSUPPORT
	gCurrentCaptureResolution = PictSize; 
#endif
	SendA8Cmd( ((ZoomRate<<12)+(PictSize << 8)) + A8_HOST_CMD_TAKE_CONTINUE_JPEG );
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_EncodeRGBToJpegByFW( u_short res )
 @brief	Translate RGB data to JPEG Picture. Mostly, this is used for Frame Sticker Capture.
 @param	res - Resolution depends on Customer`s requirement.
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A8L_EncodeRGBToJpegByFW( u_short res )
{
#ifdef EXIFSUPPORT 
	gCurrentCaptureResolution = res; 
#endif
	SetA8RegB( 0x6242, 0x03 );		// Clear JPEG frame encode done status
	SetA8RegW( 0x6222, 0x0000 );	// Clear JPEG FIFO
	
	SendA8Cmd( (res << 8) + A8_HOST_CMD_ENCODE_RGB_JPEG );
	
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_TakeStickJpegPicture( u_short PictSize, u_short ZoomRate )
 @brief	Capture Command.
 @param	PictSize - 0:1.3M, 1:VGA, 2:QVGA, 3:QQVGA...
 @param	ZoomRate - From 0~10.(depends on customer) 
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A8L_TakeStickJpegPicture( u_short PictSize, u_short ZoomRate )
{
	SetA8RegB( 0x6242, 0x01 );		// Clear JPEG frame encode done status
	SetA8RegW( 0x6222, 0x0000 );	// Clear JPEG FIFO

	if ( ZoomRate < 1 )
		ZoomRate = 1;
	else if ( ZoomRate > 15 )
		ZoomRate = 7;
		
	SendA8Cmd( (ZoomRate<<12) + (PictSize << 8) + A8_HOST_CMD_TAKE_STICK_JPEG );
	return A8_NO_ERROR;
}


/**
 @fn		s_int A8L_GetJpegPictureSize()
 @brief	Get the JPEG Picture size.
 @return	s_int
 @note	.
 @bug	N/A.
*/
s_int A8L_GetJpegPictureSize()
{
	s_int JpegSize;
	
	JpegSize = GetA8RegW( 0x621C );
	JpegSize = JpegSize + (GetA8RegB( 0x621E ) << 16);
	return JpegSize;
}


/**
 @fn		s_short A8L_IsJpegEncodeDone()
 @brief	Check if JPEG Encode Done.
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short A8L_IsJpegEncodeDone()
{
	u_char Status;
	
	Status = GetA8RegB( 0x6242 );
	if ( (Status & 0x01) != 0 )
		return 1;
	else
		return A8_NO_ERROR;
}


/*#pragma O0*/


/**
 @fn		s_int GetJpegPicture( u_short *JpegBufferAddr )
 @brief	Transfer the JPEG data to Host Buffer.
 @param	JpegBufferAddr - the host buffer address for JPEG.
 @return	s_int -the JPEG picture size got from FIFO.
 @note	.
 @bug	N/A.
*/
s_int GetJpegPicture( u_short *JpegBufferAddr )
{
	s_int JpegPictLength = 0;
	u_short *JpegBufferPtr;
	u_short Status, Length, i;
	s_int timeout = 0;

	JpegBufferPtr = JpegBufferAddr;	
	do
		{
		Status = GetA8RegW( 0x6222 );
		Length = Status & 0x1F;		
		if ( Length == 0 )
			{
			timeout++;
			if ( timeout > A8_GET_JPEG_TIMEOUT )
				{
				SetA8RegW(0x6222, 0x00);
				/*
				A8DM_AddTimeoutInfo( 0x0010000, "Get jpeg timeout", "GetJpegPicture()" );
				A8DM_SaveRegister( 0x6510, GetA8RegW( 0x6510 ) );
				A8DM_SaveRegister( 0x6222, GetA8RegW( 0x6222 ) );
				A8DM_SaveRegister( 0x6204, GetA8RegW( 0x6204 ) );
				*/
				return A8_TIMEOUT_ERROR;
				}
			}
		while ( Length > 0 )
			{
			if ( JpegBufferPtr < (JpegBufferAddr + MAX_JPEG_SIZE) )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<Length; i++ )
					{
					*(JpegBufferPtr++) = A8IndDataPW( i );
					}
				}
			else
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<Length; i++ )
					{
					*(JpegBufferPtr+1) = A8IndDataPW( i );
					}
				}
			JpegPictLength += Length;
			Status = GetA8RegW( 0x6222 );
			Length = Status & 0x1F;		
			} 
		}
	while ( (Status & 0x0100) != 0x0100 );

	Status = GetA8RegW( 0x6222 );
	Length = Status & 0x1F;		

	JpegPictLength += Length;
	while ( Length > 0 )
		{
		*(JpegBufferPtr++) = GetA8RegW( 0x6220 );
		Length--;
		} 

	SetA8RegW( 0x6222, 0x0000 );
	return (JpegPictLength << 1);
}


//20070621 JFIF_Start
u_short JFIF_APP0_HEADER[8] = {0x1000, 0x464A, 0x4649, 0x0100, 0x0102, 0x4800, 0x4800, 0x0000 };
//20070621 JFIF_END
/**
 @fn		s_short A8L_GetJpegPictureForVideo( u_short *JpegBufferAddr, s_int Limit, u_int *pJpegSize )
 @brief	Transfer the JPEG data to Host Buffer.
 @param	JpegBufferAddr - the host buffer address for JPEG.
 @param	Limit - the limitation size of transfer data(this depends on host buffer size).
 @return	s_int -the JPEG picture size got from FIFO.
 @note	Notice that if the jpeg size is larger than the size limit (Limit),
 @n		there are only the limit size of jpeg file will be retrieved. The
 @n		caller must check the size of the function returned to check
 @n		whether the size is over the limit or not.
 @bug	N/A.
*/
u_short A8L_GetJpegPictureForVideo( u_short *JpegBufferAddr, u_int Limit, u_int *pJpegSize )
{
	u_short *JpegBufferPtr;
	s_short JpegEncodeDone = 0;
	s_int timeout = 0;
	s_int CurrentJpegPageSize, CurrentJpegReadCount;
	s_int JpegSize, TempSize;
//	u_short Reg, Val;
	s_int PageLimit;
  	u_char	OverFlag = 0; // 0526


	JpegBufferPtr = JpegBufferAddr;

	*pJpegSize = 0;
	PageLimit = Limit >> 5;
	CurrentJpegReadCount = 0;
    
	while ((CurrentJpegPageSize = (GetA8RegW(0x6254)<<3))==0 && timeout < A8_JPEG_GET_PIC_TIME_OUT)
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	if ( timeout >= A8_JPEG_GET_PIC_TIME_OUT )
	{
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 2 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}
	
	timeout = 0;
	CurrentJpegPageSize = GetA8RegW(0x6254)<<3;
	do
	{
		if (GetA8RegB(0x6242) & 0x02) // 0526
		{
				AIT_Message_P0( "Encode OVERFLOW.... \n" ) ;
	      		OverFlag = 1;
	          	break;
		}
		if ( CurrentJpegReadCount < CurrentJpegPageSize )
		{
			timeout = 0;
			if (CurrentJpegPageSize > PageLimit)
			{
				JpegEncodeDone = 1;
				OverFlag = 2;
				break;
			}
			else
			{
#if 1
				u_int	tmp;
				tmp = CurrentJpegPageSize-CurrentJpegReadCount;
				A8L_Get16WDataFromFIFO(0x6220,JpegBufferPtr,tmp);
				JpegBufferPtr+=(tmp<<4);
#else
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for (i=CurrentJpegReadCount; i<CurrentJpegPageSize; i++)
				{
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
				}
#endif				
				CurrentJpegReadCount=CurrentJpegPageSize;		
			}
		}
		else
		{
			timeout ++ ;
			if ( (GetA8RegW(0x6204) & 0x0008) == 0 )
				JpegEncodeDone = 1;
		}
		CurrentJpegPageSize = (GetA8RegW(0x6254)<<3);
	}
	while (( JpegEncodeDone == 0 )&&(timeout < A8_JPEG_ENCODEDONE_TIME_OUT));
	if(timeout >= A8_JPEG_ENCODEDONE_TIME_OUT)
	{
		SetA8RegW( 0x6222, 0x0000 );
       AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 3 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}

	CurrentJpegReadCount = CurrentJpegReadCount << 4;		// count by u_short.
	timeout = 0;
	while((GetA8RegW(0x6204) & 0x08) && (timeout< A8_JPEG_GET_PIC_TIME_OUT));
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	JpegSize = (GetA8RegW( 0x6250 ) + ((s_int)GetA8RegW(0x6252)<<16) + 1)>>1;	// count by Word.
	if(timeout >= A8_JPEG_GET_PIC_TIME_OUT)
	{
		SetA8RegW( 0x6222, 0x0000 );
       AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 4 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}
  	if (OverFlag) // 0526
	{
		SetA8RegW( 0x6222, 0x0000 );    
		AIT_Message_P1( "OverFlow, filesize = %d \n", JpegSize<<1) ;
		return A8_CAM_FIFO_OVERFLOW_ERROR;
	}

	if (JpegSize < (Limit>>1) )
		TempSize = JpegSize;
	else
		TempSize = (Limit>>1);

	//open FIFO
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = 0x6220;
	A8IndCmdP = HII_RW_REG_W;
#if 1	
	while(CurrentJpegReadCount < TempSize){
		*(JpegBufferPtr++)  = A8IndDataPW(0);
		CurrentJpegReadCount++;		
		}
#else	
	for (i=CurrentJpegReadCount; i<TempSize; i++)			// count by Word.
	{
		//*(JpegBufferPtr++) = GetA8RegW(0x6220);
		*(JpegBufferPtr++)  = A8IndDataPW(0);
		CurrentJpegReadCount++;
	}
#endif
	*pJpegSize = JpegSize << 1;			// count by Byte !!

	return A8_NO_ERROR;
}

/**
 @fn		s_short A8L_GetJpegPictureWithLimit( u_short *JpegBufferAddr, s_int Limit, u_int *pJpegSize )
 @brief	Transfer the JPEG data to Host Buffer.
 @param	JpegBufferAddr - the host buffer address for JPEG.
 @param	Limit - the limitation size of transfer data(this depends on host buffer size).
 @return	s_int -the JPEG picture size got from FIFO.
 @note	Notice that if the jpeg size is larger than the size limit (Limit),
 @n		there are only the limit size of jpeg file will be retrieved. The
 @n		caller must check the size of the function returned to check
 @n		whether the size is over the limit or not.
 @bug	N/A.
*/
s_short A8L_GetJpegPictureWithLimit( u_short *JpegBufferAddr, s_int Limit, u_int *pJpegSize )
{
	u_short *JpegBufferPtr;
	s_short JpegEncodeDone = 0;
	s_int timeout = 0;
	s_int CurrentJpegPageSize, CurrentJpegReadCount, i;
	s_int JpegSize, TempSize;
//	u_short Reg, Val;
	s_int PageLimit;
  	u_char	OverFlag = 0; // 0526

#ifdef EXIFSUPPORT
	u_char *ExifPtr;
	ExifPtr = (u_char *)JpegBufferAddr;
#ifndef EXIFSUPPORT
	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();
#endif
	for(i=0;i<0x314;i++)
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegBufferPtr = JpegBufferAddr + 0x189;	
#else
	JpegBufferPtr = JpegBufferAddr;
#endif

	*pJpegSize = 0;
	PageLimit = Limit >> 5;
	CurrentJpegReadCount = 0;
    
	sys_IF_ait_delay1us(500);	// Now, BaseBand is fast, so, just wait few time for AIT processing command.
	timeout = 0;	
	while (((GetA8RegB(0x6222) & 0x80)==0) && timeout < A8_JPEG_GET_PIC_TIME_OUT) //0x1000000)
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	if(timeout >= A8_JPEG_GET_PIC_TIME_OUT )
	{
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 1 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}

	timeout = 0;
	while ((CurrentJpegPageSize = (GetA8RegW(0x6254)<<3))==0 && timeout < A8_JPEG_GET_PIC_TIME_OUT)
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	if ( timeout >= A8_JPEG_GET_PIC_TIME_OUT )
	{
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 2 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}
	
	timeout = 0;
	CurrentJpegPageSize = GetA8RegW(0x6254)<<3;
	do
	{
		if (GetA8RegB(0x6242) & 0x02) // 0526
		{
				AIT_Message_P0( "Encode OVERFLOW.... \n" ) ;
	      		OverFlag = 1;
	          	break;
		}
		if ( CurrentJpegReadCount < CurrentJpegPageSize )
		{
			timeout = 0;
			if (CurrentJpegPageSize > PageLimit)
			{
				AIT_Message_P0( "Encode OVER LIMIT.... \n" ) ;			
				JpegEncodeDone = 1;
				//return A8_CAM_CAPTURE_ERROR;
				OverFlag = 2;
				break;
			}
			else
			{

				u_int 	tmp;
				tmp = CurrentJpegPageSize-CurrentJpegReadCount;
				A8L_Get16WDataFromFIFO(0x6220,JpegBufferPtr,tmp);
				JpegBufferPtr+=(tmp<<4);
				CurrentJpegReadCount=CurrentJpegPageSize;		
			}
		}
		else
		{
			timeout ++ ;
			if ( (GetA8RegW(0x6204) & 0x0008) == 0 )
				JpegEncodeDone = 1;
		}
		CurrentJpegPageSize = (GetA8RegW(0x6254)<<3);
	}
	while (( JpegEncodeDone == 0 )&&(timeout < A8_JPEG_ENCODEDONE_TIME_OUT));
	if(timeout >= A8_JPEG_ENCODEDONE_TIME_OUT)
	{
		SetA8RegW( 0x6222, 0x0000 );
       AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 3 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}
	
	CurrentJpegReadCount = CurrentJpegReadCount << 4;		// count by u_short.
	timeout = 0;
	while((GetA8RegW(0x6204) & 0x08) && (timeout< A8_JPEG_GET_PIC_TIME_OUT));
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	JpegSize = (GetA8RegW( 0x6250 ) + ((s_int)GetA8RegW(0x6252)<<16) + 1)>>1;	// count by Word.
	if(timeout >= A8_JPEG_GET_PIC_TIME_OUT)
	{
		SetA8RegW( 0x6222, 0x0000 );
       AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 4 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}
  	if (OverFlag) // 0526
    {
		SetA8RegW( 0x6222, 0x0000 );    
     	AIT_Message_P1( "OverFlow, filesize = %d \n", JpegSize<<1) ;
		return A8_CAM_FIFO_OVERFLOW_ERROR;
	}

	if (JpegSize < (Limit>>1) )
		TempSize = JpegSize;
	else
		TempSize = (Limit>>1);

	A8L_Get16WDataFromFIFO(0x6220,JpegBufferPtr,(TempSize+31)/32);
	
	SetA8RegW( 0x6222, 0x0000 );

#ifdef EXIFSUPPORT
	JpegSize += 0x189;

	JpegBufferPtr = JpegBufferAddr + 0x189 + (0x298>>1);
#else
	JpegBufferPtr = JpegBufferAddr + (0x298>>1);
#endif	
	
	//20070621 JFIF_Start
	if (*(JpegBufferPtr) == 0xE0FF)
	{
		//AIT_Message_P0("update JFIF data!\n");
		JpegBufferPtr ++ ;
		for(i=0; i<8 ; i++)
		{
			*(JpegBufferPtr+i) = JFIF_APP0_HEADER[i];
		}
	}
	//20070621 JFIF_END
	
	*pJpegSize = JpegSize << 1;			// count by Byte !!
	
	return A8_NO_ERROR;
}



/* Compare A8L_GetJpegPictureWithLimit(). */
/* Only different in "JpegEncodeDone = 1" check!! */
s_short A8L_GetJpegPictureWithLimit_RAW( u_short *JpegBufferAddr, s_int Limit, s_short resolution, u_int *pJpegSize )
{
	u_short *JpegBufferPtr;
	s_short JpegEncodeDone = 0;
	s_int timeout = 0;
	s_int CurrentJpegPageSize, CurrentJpegReadCount, i;
	s_int JpegSize, TempSize;
//	u_short Reg, Val;
	s_int PageLimit;
	s_int targetsize =0;

#if 0 // mark for Mosaic shot mode
#ifdef EXIFSUPPORT
	u_char *ExifPtr;
	ExifPtr = (u_char *)JpegBufferAddr;

	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();
	for(i=0;i<0x314;i++)
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegBufferPtr = JpegBufferAddr + 0x189;	
#else
	JpegBufferPtr = JpegBufferAddr;
#endif
#else
	JpegBufferPtr = JpegBufferAddr;
#endif
#if 0 // OPUS2G
	if (resolution==8)
		targetsize=176*144;
#else // 0507 MOSAIC EXIF test
	if (resolution==9)
		targetsize=128*96;
#endif    

	*pJpegSize = 0;
	PageLimit = Limit >> 5;
	CurrentJpegReadCount = 0;

	timeout = 0;	
	while (((GetA8RegB(0x6222) & 0x80)==0) && timeout < A8_JPEG_GET_PIC_TIME_OUT) 
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	if(timeout >= A8_JPEG_GET_PIC_TIME_OUT )
	{
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 1 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}

	timeout = 0;			
	while ((CurrentJpegPageSize = (GetA8RegW(0x6254)<<3))==0 && timeout < A8_JPEG_GET_PIC_TIME_OUT)
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	if ( timeout >= A8_JPEG_GET_PIC_TIME_OUT )
	{
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 2 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}

	timeout = 0;
	CurrentJpegPageSize = GetA8RegW(0x6254)<<3;
	do
	{
		if ( CurrentJpegReadCount < CurrentJpegPageSize )
		{
			timeout = 0;
			if (CurrentJpegPageSize > PageLimit)
			{
				JpegEncodeDone = 1;
			}
			else
			{				
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for (i=CurrentJpegReadCount; i<CurrentJpegPageSize; i++)
				{
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );
					*(JpegBufferPtr++) = A8IndDataPW( i );			
				}
				CurrentJpegReadCount=CurrentJpegPageSize;		
			}
		}
		else
		{
			timeout ++ ;
			//if ( (GetA8RegW(0x6204) & 0x0008) == 0 )
			if(((GetA8RegW( 0x6250 ) + ((s_int)GetA8RegW(0x6252)<<16) + 1)>>1)>=targetsize)
				JpegEncodeDone = 1;
		}
		CurrentJpegPageSize = (GetA8RegW(0x6254)<<3);
	}
	while (( JpegEncodeDone == 0 )&&(timeout < A8_JPEG_ENCODEDONE_TIME_OUT));
	if(timeout >= A8_JPEG_ENCODEDONE_TIME_OUT)
	{
		SetA8RegW( 0x6222, 0x0000 );
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 3 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}

	CurrentJpegReadCount = CurrentJpegReadCount << 4;		// count by u_short.
	timeout = 0;
	while((GetA8RegW(0x6204) & 0x08) && (timeout< A8_JPEG_GET_PIC_TIME_OUT));
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	}
	JpegSize = (GetA8RegW( 0x6250 ) + ((s_int)GetA8RegW(0x6252)<<16) + 1)>>1;	// count by Word.
	if(timeout >= A8_JPEG_GET_PIC_TIME_OUT)
	{
		SetA8RegW( 0x6222, 0x0000 );
		AIT_Message_P0( "A8L_GetJpegPictureWithLimit time out 4 \n" ) ;
		return A8_TIMEOUT_ERROR;
	}
	if (JpegSize < (Limit>>1) )
		TempSize = JpegSize;
	else
		TempSize = (Limit>>1);

	//open FIFO
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = 0x6220;
	A8IndCmdP = HII_RW_REG_W;
	for (i=CurrentJpegReadCount; i<TempSize; i++)			// count by Word.
	{
		//*(JpegBufferPtr++) = GetA8RegW(0x6220);
		*(JpegBufferPtr++)  = A8IndDataPW(0);
		CurrentJpegReadCount++;
	}

	SetA8RegW( 0x6222, 0x0000 );
#if 0 // 080508 mark for Mosaic shot
#ifdef EXIFSUPPORT
	JpegSize += 0x189;

	JpegBufferPtr = JpegBufferAddr + 0x189 + (0x298>>1);
#else
	JpegBufferPtr = JpegBufferAddr + (0x298>>1);
#endif	

#if 0 // 0507 open for MOSAIC EXIF test	// If RAW data case, there is no any header need to be update!!
	//20070621 JFIF_Start
	if (*(JpegBufferPtr) == 0xE0FF)
	{
		//AIT_Message_P0("update JFIF data!\n");
		JpegBufferPtr ++ ;
		for(i=0; i<8 ; i++)
		{
			*(JpegBufferPtr+i) = JFIF_APP0_HEADER[i];
		}
	}
	//20070621 JFIF_END
#endif
#endif
	
	*pJpegSize = JpegSize << 1;			// count by Byte !!
	
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_GetJpegPictureBufWithLimit( u_short *JpegBufferAddr, u_int A8JpegBufAddr, s_int Limit, u_int *pJpegSize  )
 @brief	Transfer the JPEG data to Host Buffer.
 @param	JpegBufferAddr - the host buffer address for JPEG Data.
 @param	A8JpegBufAddr - the memory address of JPEG Data in AIT chip.
 @param	Limit - the limitation size of transfer data( if the jpeg size large than the limit, only the limit size portion will be retrieved.)
 @return	s_int -the JPEG picture size got from FIFO.
 @note	Notice that if the jpeg size is larger than the size limit (Limit),
 @n		there are only the limit size of jpeg file will be retrieved. The
 @n		caller must check the size of the function returned to check
 @n		whether the size is over the limit or not.
 @bug	N/A.
*/
s_short A8L_GetJpegPictureBufWithLimit( u_short *JpegBufferAddr, u_int A8JpegBufAddr, s_int Limit, u_int *pJpegSize )
{
	u_short Status;
	s_int JpegSize;
	s_int Timeout;
	u_short *JpegBufferPtr;

#ifdef EXIFSUPPORT
	int i;
	u_char *ExifPtr;
	ExifPtr = (u_char *)JpegBufferAddr;
#ifndef EXIFSUPPORT
	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();
#endif
	for(i=0;i<0x314;i++)
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegBufferPtr = JpegBufferAddr + 0x189;	
#else
	JpegBufferPtr = JpegBufferAddr;
#endif

	*pJpegSize = 0;

	Timeout = 0;
	do
	{
		Status = GetA8RegW(0x654E);
		sys_IF_ait_delay1us(100);
		Timeout++;
		// 0x20, Encode Stop flag!
	}while ( ( (Status & 0x20) != 0 ) && (Timeout < 10000 ) );
	
	if ( Timeout >= 10000 )
	{
		AIT_Message_P0("Time count 2\n");
		return A8_TIMEOUT_ERROR;
	}
		
	JpegSize = A8L_GetJpegPictureSize();
	//AIT_Message_P1( "JPEG Size = %d\n", JpegSize ) ;

	if (JpegSize > Limit )
		JpegSize = Limit;
	          
	CopyMemWordA8ToHost(JpegBufferPtr, A8JpegBufAddr, JpegSize+1);

#ifdef EXIFSUPPORT
	JpegSize += 0x314;
#endif 

//	AIT_Message_P1( "JPEG Size result = %d\n", JpegSize ) ;
	
	*pJpegSize = JpegSize;

	return A8_NO_ERROR;
}

/**
 @fn		u_short A8L_MainLCDPreviewActive()
 @brief	Select Main LCD for preview operation.
 @return	u_short.
 @note	.
 @bug	N/A.
*/
u_short A8L_MainLCDPreviewActive(void)
{ 
	s_short retVal;
	
	retVal = SendA8Cmd( A8_HOST_CMD_SELECT_LCD_ACTIVE + A8_HOST_SCMD_MAINLCD_ACTIVE );
	if ( retVal != 0 )
	   return retVal;

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}


/**
 @fn		u_short A8L_SubLCDPreviewActive()
 @brief	Select Sub LCD for preview operation.
 @return	u_short.
 @note	.
 @bug	N/A.
*/
u_short A8L_SubLCDPreviewActive(void)
{ 
	s_short retVal;
	
	retVal = SendA8Cmd( A8_HOST_CMD_SELECT_LCD_ACTIVE + A8_HOST_SCMD_SUBLCD_ACTIVE );
	if ( retVal != 0 )
	   return retVal;

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}	
 	
	return A8_NO_ERROR;
}


/**
 @note	.
 @bug	N/A.
*/
#if (AIT_VIDEO_PHONE_SUPPORT)
extern s_short A8L_SetTVPreviewMode(unsigned short bEnable, u_int addr_pip0, u_int addr_pip1)
#else
extern s_short A8L_SetTVPreviewMode(unsigned short bEnable)
#endif
{   
	s_short retVal;

#if AIT_VIDEO_PHONE_SUPPORT
    SetA8RegW(0x6540, (u_short) ((addr_pip0 & 0xFF) + ((addr_pip0>>8) & 0xFF)));
    SetA8RegW(0x6542, (u_short) ((addr_pip0>>16) & 0xFF));
    SetA8RegW(0x6544, (u_short)((addr_pip1 & 0xFF) + ((addr_pip1>>8) & 0xFF)));
    SetA8RegW(0x6546, (u_short) ((addr_pip1>>16) & 0xFF));

	retVal = SendA8Cmd((bEnable << 8) + A8_HOST_CMD_SET_VIF_SRC);
#else
	retVal = SendA8Cmd(((bEnable&0x01) << 8) + A8_HOST_CMD_SET_VIF_SRC);
#endif	
    if ( retVal != A8_NO_ERROR)
   {
      AIT_Message_P0("A8L_SetLCDPreviewModeCMD timeout!!");
		return retVal;
   }


	return A8L_CheckReadyForA8Command();
}

/**
 @fn		s_short A8L_StartLCDPreview()
 @brief	Enable preview operation.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_StartLCDPreview()
{
	s_short retVal;

	retVal = SendA8Cmd( A8_HOST_CMD_SET_PREVIEW_MODE + A8_HOST_SCMD_START_PREVIEW );

	
	if ( retVal != 0 )
		return retVal;

	return A8L_CheckReadyForPreview(1);
}


/**
 @fn		s_short A8L_StopLCDPreview()
 @brief	Disable preview operation.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_StopLCDPreview(void)
{   
	s_short retVal;

	retVal = SendA8Cmd( A8_HOST_CMD_SET_PREVIEW_MODE + A8_HOST_SCMD_STOP_PREVIEW );
    sys_IF_ait_delay1ms(100); // 080516 reduce wait time (should be 0)
    if ( retVal != A8_NO_ERROR)
   {
      AIT_Message_P0("Stop Preview CMD timeout!!");
		return retVal;
   }
	return A8L_CheckReadyForPreview(0);
}


u_short gA8PrevScaleRatio = 1;
u_short gA8PrevZoomBase;
u_short gA8PrevZoomScaleStep = 1;		
u_short gA8PrevZoomOffsetH, gA8PrevZoomOffsetV;
u_short gA8CurrentZoomRate = 10;


/**
 @fn		s_short A8L_SetCdspPreviewResolutionL( s_short TgRawW, s_short TgRawH, s_short ScaleN, s_short ScaleM, u_short ZoomRate )
 @brief	Setup Liner Preview ZoomRate.
 @param	TgRawW - the Width of Raw resolution from Sensor.
 @param	TgRawH - the Height of Raw resolution from Sensor.
 @param	ScaleN - Scaler N.
 @param	ScaleM - Scaler M.
 @param	ZoomRate - ZoomRate for preview.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_SetCdspPreviewResolutionL( s_short TgRawW, s_short TgRawH, s_short ScaleN, s_short ScaleM, u_short ZoomRate )
{	
	if(gA8CurrentZoomRate== ZoomRate)
		return A8_NO_ERROR;
	//AIT_Message_P1("A8L_SetCdspPreviewResolutionL (%d) : \n", ZoomRate);

	if(gA8CurrentZoomRate < ZoomRate)
	{
	SetA8RegB( 0x4F34,(gA8PrevScaleRatio*ScaleN*ZoomRate)/16);
	SetA8RegB( 0x4F35,(gA8PrevScaleRatio*ScaleN*gA8CurrentZoomRate)/16);
	SetA8RegB(0x4F30,0x02);
	}
	else
	{
	SetA8RegB( 0x4F35,(gA8PrevScaleRatio*ScaleN*ZoomRate)/16);
	SetA8RegB( 0x4F34,(gA8PrevScaleRatio*ScaleN*gA8CurrentZoomRate)/16);
	SetA8RegB(0x4f30,0x06);
	}
	//AIT_Message_P5("offset(x,y)=(%d,%d); step=%d; (N_new,N_cur)=(%d,%d)", gA8PrevZoomOffsetH, gA8PrevZoomOffsetV, gA8PrevZoomScaleStep, 
						//((gA8PrevScaleRatio*ScaleN*ZoomRate)/10), ((gA8PrevScaleRatio*ScaleN*gA8CurrentZoomRate)/10));

	gA8CurrentZoomRate = ZoomRate;

    return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_SetCdspPreviewResolution( s_short TgRawW, s_short TgRawH, s_short PrevW, s_short PrevH, s_short ScaleN, s_short ScaleM, u_short ZoomRate )
 @brief	Setup Preview ZoomRate.
 @param	TgRawW - the Width of Raw resolution from Sensor.
 @param	TgRawH - the Height of Raw resolution from Sensor.
 @param	PrevW - the Width of preview resolution.
 @param	PrevH - the Height of preview resolution.
 @param	ScaleN - Scaler N.
 @param	ScaleM - Scaler M.
 @param	ZoomRate - ZoomRate for preview.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_SetCdspPreviewResolution( s_short TgRawW, s_short TgRawH, s_short PrevW, s_short PrevH, s_short ScaleN, s_short ScaleM, u_short ZoomRate )
{
	s_short SDWidth, SDHeight;
	s_short StartX, EndX, StartY, EndY;
	
	if ( ScaleM < 1 )
		ScaleM = 1;
	if ( ScaleN < 1 )
		ScaleN = 1;

		gA8CurrentZoomRate = ZoomRate;
		gA8PrevZoomBase = ScaleM * gA8PrevScaleRatio;

	if(!gA8PrevZoomBase)
	{
		AIT_Message_P0("gA8PrevZoomBase = 0, it will cause zero divid error!");
		return A8_CAM_DECODE_ERROR;

	}
	else
	{		
		gA8PrevZoomOffsetH = TgRawW / gA8PrevZoomBase/2;
		gA8PrevZoomOffsetV = TgRawH / gA8PrevZoomBase/2;
	}
	SetA8RegB( 0x4F14, 0x00);		       //20080417 AIT_Wind  Active the bypass scaler.
	SetA8RegB( 0x4F1D, 0x00);
	SetA8RegB( 0x4F10, 0x00 );
	SetA8RegB( 0x4F12, GetA8RegB(0x4F12)|0x01 );
	SetA8RegB( 0x4F15, (ScaleN * gA8PrevScaleRatio + ZoomRate) );
	SetA8RegB( 0x4F16, gA8PrevZoomBase );
	SetA8RegB( 0x4F18, 0x03 );
	SetA8RegW( 0x4F30, 0x00 );

	SetA8RegB( 0x4F1C, 0x31 );
	if ((ScaleN * gA8PrevScaleRatio + ZoomRate) <= gA8PrevZoomBase)
	{
		SDWidth = (s_short)( (((s_int)TgRawW * (ScaleN * gA8PrevScaleRatio + ZoomRate)) ) / gA8PrevZoomBase );
		SDHeight =(s_short)( (((s_int)TgRawH * (ScaleN * gA8PrevScaleRatio + ZoomRate)) ) / gA8PrevZoomBase );
	
		StartX = ((SDWidth - PrevW) / 2) + 1;
		EndX = StartX + PrevW - 1;
		StartY = ((SDHeight - PrevH) / 2) + 1;
		EndY = StartY + PrevH - 1;

		SetA8RegW( 0x4F00, 0x01 );
		SetA8RegW( 0x4F02, TgRawW );
		SetA8RegW( 0x4F04, 0x01 );
		SetA8RegW( 0x4F06, TgRawH );

		SetA8RegW( 0x4F08, StartX );
		SetA8RegW( 0x4F0A, EndX );
		SetA8RegW( 0x4F0C, StartY );
		SetA8RegW( 0x4F0E, EndY );
		AIT_Message_P0("A8L_SetCdspPreviewResolution : \n");
		AIT_Message_P6("Grab Back S(x,y)=(%d,%d); E(x,y)=(%d,%d), (N,M)=(%d,%d)\n", StartX, StartY, EndX, EndY, (ScaleN * gA8PrevScaleRatio + ZoomRate), gA8PrevZoomBase);
	}
	else
	{
		if(!gA8PrevScaleRatio)
		{
			AIT_Message_P0("gA8PrevZoomBase = 0, it will cause zero divid error!");
			return A8_CAM_DECODE_ERROR;
		}
		else
		{
		
			SDWidth = ( (s_int) (PrevW * gA8PrevZoomBase) / (ScaleN * gA8PrevScaleRatio + ZoomRate));
			SDHeight = ( (s_int) (PrevH * gA8PrevZoomBase) / (ScaleN * gA8PrevScaleRatio + ZoomRate));
		}
		StartX = ((TgRawW - SDWidth) /2);
		EndX = StartX + SDWidth + 2;
		StartY = ((TgRawH - SDHeight) /2);
		EndY = StartY + SDHeight + 2;
		
		SetA8RegW( 0x4F00, StartX );
		SetA8RegW( 0x4F02, EndX );
		SetA8RegW( 0x4F04, StartY );
		SetA8RegW( 0x4F06, EndY );

		SetA8RegW( 0x4F08, 0x01 );
		SetA8RegW( 0x4F0A, PrevW );
		SetA8RegW( 0x4F0C, 0x01 );
		SetA8RegW( 0x4F0E, PrevH );
		AIT_Message_P0("A8L_SetCdspPreviewResolution : \n");
		AIT_Message_P6("Grab Front S(x,y)=(%d,%d); E(x,y)=(%d,%d), (N,M)=(%d,%d)\n", StartX, StartY, EndX, EndY, (ScaleN * gA8PrevScaleRatio + ZoomRate), gA8PrevZoomBase);
	}
	return A8_NO_ERROR;

}


/**
 @fn		s_short A8L_ConfigPreviewZoomParam( u_short ScaleRatio, u_short ScaleStep )
 @brief	Setup Preview related parameters.
 @param	ScaleRatio - .
 @param	ScaleStep - .
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_ConfigPreviewZoomParam( u_short ScaleRatio, u_short ScaleStep )
{
	gA8PrevScaleRatio = ScaleRatio;
	gA8PrevZoomScaleStep = ScaleStep;
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_SetPreviewZoom( u_short TgtZoomRate )
 @brief	Setup Preview target ZoomRate.
 @param	TgtZoomRate - target ZoomRate.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_SetPreviewZoom( u_short TgtZoomRate )
{
	u_short Temp;
    u_int timeout = 0;
	
	if ( TgtZoomRate > 40 || TgtZoomRate < 10 )
        return A8_OUT_OF_RANGE_ERROR;
		
	do
		{
		Temp = GetA8RegB( 0x4F31 );
		timeout++;
		}
	while ( timeout < 0x10000 && Temp != 0 );
		
	SetA8RegB( 0x4F10, 0x00 );
	SetA8RegW( 0x4F30, 0x00 );
	SetA8RegB( 0x4F16, gA8PrevZoomBase );
	SetA8RegB( 0x4F32, gA8PrevZoomOffsetH );
	SetA8RegB( 0x4F33, gA8PrevZoomOffsetV );
	SetA8RegW( 0x4F34, 0x4001 );

	if ( TgtZoomRate > gA8CurrentZoomRate )
		{
		SetA8RegB( 0x4F30, 0x01 );
		Temp = (( TgtZoomRate - gA8CurrentZoomRate )>>3) * gA8PrevScaleRatio * gA8PrevZoomScaleStep;
		SetA8RegB( 0x4F31, Temp );
		}
	else if ( TgtZoomRate < gA8CurrentZoomRate )
		{
		SetA8RegB( 0x4F30, 0x05 );
		Temp = (( gA8CurrentZoomRate - TgtZoomRate )>>3) * gA8PrevScaleRatio * gA8PrevZoomScaleStep;
		SetA8RegB( 0x4F31, Temp );
		}
	else
		SetA8RegW( 0x4F30, 0x00 );
    
	gA8CurrentZoomRate = TgtZoomRate;
	
    return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_SetPreviewWindowAttribute( s_int Buffer0BaseAddr, s_int Buffer1BaseAddr, u_short width, u_short height )
 @brief	Setup PIP window buffer attributes.
 @param	Buffer0BaseAddr - PIP window buffer0 address.
 @param	Buffer1BaseAddr - PIP window buffer1 address.
 @param	width - PIP window Width.
 @param	height - PIP window Height.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_SetPreviewWindowAttribute( s_int Buffer0BaseAddr, s_int Buffer1BaseAddr, u_short width, u_short height )
{
	A8L_SetWindowAttribute( A8_PIP_WINDOW, width, height, A8_DEPTH_16B, Buffer0BaseAddr );
	SetA8RegW( 0x508C, (u_short)(Buffer1BaseAddr & 0x0FFFF) );
	SetA8RegW( 0x508E, (u_short)(Buffer1BaseAddr >> 16 ));
	
	SetA8RegW(0x6508, (u_short)(Buffer0BaseAddr & 0xFFFF));	// JPEG start address
	SetA8RegW(0x650A, (u_short)(Buffer0BaseAddr >> 16));	// JPEG start address
	SetA8RegW(0x650C, (u_short)(Buffer1BaseAddr & 0xFFFF));	// JPEG start address
	SetA8RegW(0x650E, (u_short)(Buffer1BaseAddr >> 16));

	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_SetPreviewWindowToDisplay( u_short startx, u_short starty, u_short displayWidth, u_short displayHeight, u_short displayOffsetx, u_short displayOffsety, u_short rotateType)
 @brief	Setup "position", "resolution" and "rotate type" on LCD for PIP window.
 @param	startx - X of start position of PIP Window.
 @param	starty - Y of start position of PIP Window.
 @param	displayWidth - Width of display resolution of PIP Window.
 @param	displayHeight - Height of display resolution of PIP Window.
 @param	displayOffsetx - X of start position on LCD.
 @param	displayOffsety - X of start position on LCD.
 @param	rotateType - RotateType of Window on LCD. 
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_SetPreviewWindowToDisplay( u_short startx, u_short starty, u_short displayWidth, u_short displayHeight, u_short displayOffsetx, 
			u_short displayOffsety, u_short rotateType)
{
	A8L_SetWindowToDisplay( A8_PIP_WINDOW, startx, starty, displayWidth, displayHeight, displayOffsetx, displayOffsety, rotateType );
	A8L_SetWindowActive( A8_PIP_WINDOW, 1 );
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_SetPreviewPath( s_short PrevPath )
 @brief	Preview Format (RGB565 or YUV422).
 @param	PrevPath - 0:RGB565 path,  1:Yuv422 path.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short A8L_SetPreviewPath( s_short PrevPath )
{
   
   if ( PrevPath == 0 )
      {
      SetA8RegB( 0x6510, GetA8RegB(0x6510) & 0xFC );
      SetA8RegB( 0x5042, (GetA8RegB(0x5042) & 0xFE ));
      }
   else if ( PrevPath == 1 )
      {
      SetA8RegB( 0x6510, (GetA8RegB(0x6510) & 0xFC) | 0x01 );
      SetA8RegB( 0x5042, (GetA8RegB(0x5042) | 0x01) );
      }
	return A8_NO_ERROR;
}

/**
 @fn		s_short RestorePreviewSetting()
 @brief	Preview Rotation.
 @return	s_short.
 @note	.
 @bug	N/A.
*/
s_short RestorePreviewSetting()
{
	SetA8RegW( 0x6026, 0x0000 );
	SetA8RegW( 0x6174, 0xFFE0 );
	SetA8RegW( 0x617A, 0x6C80 );
	SetA8RegW( 0x620A, 0x0001 );
	return A8_NO_ERROR;
}


//=========================================================================
u_short A8L_GetJfifTag( u_short *TagLength, u_char **ImgAddr, s_int Length )
{
	s_int i;
	u_short TagID;
	
	i = 0;
	while ( *(*ImgAddr+i) != 0xFF && i < Length )
		i++;
	
	if ( i >= Length )
		return 0x00;
		
	*ImgAddr = *ImgAddr + i;

	TagID = (**ImgAddr << 8) + *(*ImgAddr+1);
	if ( TagID >= 0xFFD0 && TagID <= 0xFFD9 )
		{
		*TagLength = 0;
		}
	else
		{
		*TagLength = (*(*ImgAddr+2) << 8) + *(*ImgAddr+3);
		}
	return TagID;
}


s_short A8L_GetJpegInfo( u_short *ImgAddr, s_int Length, s_short *Width, s_short *Height, s_short *Format, u_short *Quality )
{
	s_int  ImgLength;
	u_char *JpegBufferPtr;
	u_short TagID, TagLength;
	u_short retVal;
	u_short CType;
	
	JpegBufferPtr = (u_char *) ImgAddr;
	
	ImgLength = Length;
	retVal = 0;
	do
		{
		TagID = A8L_GetJfifTag( &TagLength, &JpegBufferPtr, ImgLength );
		ImgLength = ImgLength - TagLength - 2;

        if (TagID == 0xFFDB)
        {
            if (((*(JpegBufferPtr+5) << 8) + *(JpegBufferPtr+6)) == 0x303)
                *Quality = 0;
            else if (((*(JpegBufferPtr+5) << 8) + *(JpegBufferPtr+6)) == 0x505)
                *Quality = 1;
            else if (((*(JpegBufferPtr+5) << 8) + *(JpegBufferPtr+6)) == 0xD0B)
                *Quality = 2;
            else
                *Quality = 0;
	}
            
		if ( TagID > 0xFFC0 && TagID < 0xFFC4 )
			{
			retVal = 1;		// Not Baseline JPEG
			}
		else if ( TagID > 0xFFC4 && TagID < 0xFFCC )
			{
			retVal = 1;		// Not Baseline JPEG
			}
		else if ( TagID > 0xFFCC && TagID < 0xFFD0 )
			{
			retVal = 1;		// Not Baseline JPEG
			}
		else if ( TagID == 0xFFC0 )
			{
			retVal = 2;		// Baseline JPEG
			}
		else 	//	if ( TagID != 0xFFC0 )
			{
			JpegBufferPtr += (TagLength+2);
			}
		}
	while ( TagID != 0xFFC0 && ImgLength > 0 && retVal == 0 );

	//if ( ImgLength > 0 )
	if ( retVal == 2 )
	{	
		*Height = (*(JpegBufferPtr+5) << 8) + *(JpegBufferPtr+6);
		*Width =  (*(JpegBufferPtr+7) << 8) + *(JpegBufferPtr+8);
		if ( *(JpegBufferPtr+9) == 3 )// grayscale image sw codec 
		//if ( (*(JpegBufferPtr+9) == 1) || (*(JpegBufferPtr+9) == 3) ) //070403 for mono jpeg decode
			{
			CType = *(JpegBufferPtr+11);
			if ( CType == 0x22 )			// Yuv420
				*Format = A8_JPEG_FORMAT_YUV420;
			else if ( CType == 0x21 )		// Yuv422
				*Format = A8_JPEG_FORMAT_YUV422;
			else if ( CType == 0x11 )		// Yuv444
				*Format = A8_JPEG_FORMAT_YUV444;
			else if ( CType == 0x41 )		// Yuv411
				*Format = A8_JPEG_FORMAT_YUV411;
			}
		else
			*Format = A8_JPEG_FORMAT_NOT_3_COLOR;
                return A8_NO_ERROR;
	}
	else if ( retVal == 1 )
	{
		*Width = 0;
		*Height = 0;
		*Format = A8_JPEG_FORMAT_NOT_BASELINE;
		return 1;
	}
	else
	{	
		*Width = 0;
		*Height = 0;
		*Format = A8_JPEG_FORMAT_ERROR;
		return 2;
	}
		
}


/*****************************************************************************************/
u_char	gbExchangeStuffFound;
u_char	gbAddHeaderStuff; //AIT0801

u_char *gJpegDHTAddr;
u_char *gJpegDHTEndAddr;
s_short gJpegHuffTabCount;
s_short gJpegDHTCount;
u_short gJpegHuffBackup[512];
u_short gJpegHuffBackupTail;
u_short gJpegHuffBackupLength;
u_short gJpegHuffBackupMarkLengthPos;

u_char *gJpegLastDHTEndAddr;
s_short gJpegHuffBackupEnable;


s_short A8L_ScanJpegMarker( u_short *ImgAddr, s_int Length, s_short *Width, s_short *Height, s_short *Format )
{
	s_int i, ImgLength;
	u_char *JpegBufferPtr, *HuffPtr, *TagPtr;
	u_short TagID, TagLength;
	u_short retVal;
	u_short CType;
	u_short AccumTableLength, TableLength, TcIndex, IsOddAddr;
	
	JpegBufferPtr = (u_char *) ImgAddr;
	
	ImgLength = Length;
	retVal = 0;
	gJpegHuffTabCount = 0;
	gJpegDHTCount = 0;
	gJpegHuffBackupTail = 0;
	gJpegHuffBackupLength = 0;
	gJpegHuffBackupMarkLengthPos = 3;

	gbAddHeaderStuff=0;//AIT0801
	do
	{
		TagID = A8L_GetJfifTag( &TagLength, &JpegBufferPtr, ImgLength );
		ImgLength = ImgLength - TagLength - 2;
		if ( TagID > 0xFFC0 && TagID < 0xFFC4 )
		{
			retVal = 1;		// Not Baseline JPEG
			*Width = 0;
			*Height = 0;
			*Format = A8_JPEG_FORMAT_NOT_BASELINE;
			return 1;
		}
		else if ( TagID > 0xFFC4 && TagID < 0xFFCC )
		{
			retVal = 1;		// Not Baseline JPEG
			*Width = 0;
			*Height = 0;
			*Format = A8_JPEG_FORMAT_NOT_BASELINE;
			return 1;
		}
		else if ( TagID > 0xFFCC && TagID < 0xFFD0 )
		{
			retVal = 1;		// Not Baseline JPEG
			*Width = 0;
			*Height = 0;
			*Format = A8_JPEG_FORMAT_NOT_BASELINE;
			return 1;
		}
		else if ( TagID == 0xFFC0 )
		{
			retVal = 2;		// Baseline JPEG

			*Height = (*(JpegBufferPtr+5) << 8) + *(JpegBufferPtr+6);
			*Width =  (*(JpegBufferPtr+7) << 8) + *(JpegBufferPtr+8);
			if ( *(JpegBufferPtr+9) == 3 )
			{
				CType = *(JpegBufferPtr+11);
				if ( CType == 0x22 )
					*Format = A8_JPEG_FORMAT_YUV420;
				else if ( CType == 0x21 )
					*Format = A8_JPEG_FORMAT_YUV422;
				else if ( CType == 0x11 )
					*Format = A8_JPEG_FORMAT_YUV444;
				else if ( CType == 0x41 )
					*Format = A8_JPEG_FORMAT_YUV411;
			}
			else
				*Format = A8_JPEG_FORMAT_NOT_3_COLOR;
				
			*(JpegBufferPtr+10) = 0x01;
			*(JpegBufferPtr+13) = 0x02;
			*(JpegBufferPtr+16) = 0x03;
		}
		else if ( TagID == 0xFFC4 )	// DHT
		{
			gJpegLastDHTEndAddr = JpegBufferPtr + TagLength + 2;
			if ( gJpegDHTCount < 2 )
			{
				if ( gJpegHuffTabCount == 0 )
				{
					gJpegDHTAddr = JpegBufferPtr;
					gJpegHuffBackupLength = TagLength + 2;
					gJpegDHTEndAddr = JpegBufferPtr + TagLength + 2;
				}
				else if ( gJpegHuffTabCount < 2 )
				{
					gJpegHuffBackupMarkLengthPos += gJpegHuffBackupLength;
				gJpegHuffBackupLength += (TagLength+2);
					gJpegDHTEndAddr = JpegBufferPtr + TagLength + 2;
				}
			}
				
			AccumTableLength = 0;
			do
			{
				TableLength = 0;				
				for ( i=0; i<16; i++ )
					TableLength = TableLength + *(JpegBufferPtr+AccumTableLength+5+i);

				AccumTableLength += (TableLength + 1 + 16);
				gJpegHuffTabCount++;
			}
			while ( (AccumTableLength+2) < TagLength );
			gJpegDHTCount++;
		}
		else if ( TagID == 0xFFDA )
		{
			*(JpegBufferPtr+5) = 0x01;
			*(JpegBufferPtr+7) = 0x02;
			*(JpegBufferPtr+9) = 0x03;
			
			gJpegHuffBackupEnable = 0;
			if ( *(JpegBufferPtr+8) != 0x11 || *(JpegBufferPtr+10) != 0x11 )
			{
				gJpegHuffBackupEnable = 1;
			}
			
			//Start Modified by Hank @200090306
			if((*(JpegBufferPtr+14) == 0xFF) && (*(JpegBufferPtr+15) == 0x00)) 
			{
				*(JpegBufferPtr+15) = *(JpegBufferPtr+16);
				//*(JpegBufferPtr+16) = 0x00;
				gbExchangeStuffFound = 1;
				//AIT_Message_P0("[AIT810] bit-stream FF00 pattern found\r\n");
			}
			
			for(i = 0; i < 24;i++)
			{
				if(*(JpegBufferPtr + 14 + i) == 0xFF)
				{
	              if((((s_int) (JpegBufferPtr + 14 + i) - (s_int) ImgAddr) & 0x07) == (0x07)) 
					{
						gbAddHeaderStuff = 1;
					  	//AIT_Message_P0("[AIT810] bit-stream FF00 pattern found\r\n");
	              }
	           }
	       }
			//End Modified by Hank @200090306
			
		}
		
		JpegBufferPtr += (TagLength+2);
	}
	while ( TagID != 0xFFDA && ImgLength > 0 );

	if ( TagID == 0xFFDA )
	{
		if ( gJpegHuffTabCount < 3 || gJpegHuffBackupEnable )
		{
			TagPtr = gJpegDHTAddr;
			IsOddAddr = ((s_int )gJpegDHTAddr) & 0x01;
			if ( IsOddAddr )
			{
				*((u_char *)gJpegHuffBackup) = *(gJpegLastDHTEndAddr-1);
				gJpegHuffBackupTail++;
				gJpegHuffBackupMarkLengthPos++;
			}
				
			HuffPtr = ((u_char *)gJpegHuffBackup) + gJpegHuffBackupTail;
			for (i=0; i<gJpegHuffBackupLength; i++ )
			{
				*HuffPtr++ = *TagPtr++;
			}
			gJpegHuffBackupTail = gJpegHuffBackupTail+gJpegHuffBackupLength;			
			
			HuffPtr = (u_char *)gJpegHuffBackup;
			if ( IsOddAddr )
				HuffPtr++;
			
			AccumTableLength = 0;
			TcIndex = 4;
			do
			{
				TableLength = 0;
				for ( i=0; i<16; i++ )
					TableLength = TableLength + *(HuffPtr+TcIndex+1+i);

				if ( (*(HuffPtr+TcIndex) & 0x01 ) == 0x00 )
					*(HuffPtr + TcIndex) = *(HuffPtr+TcIndex) | 0x01;
				else
					*(HuffPtr + TcIndex) = *(HuffPtr+TcIndex) & 0x10;

				AccumTableLength += (TableLength + 1 + 16);
				TcIndex += AccumTableLength;
				if ( gJpegHuffTabCount == gJpegDHTCount )
					TcIndex += 4;
			}
			while ( TcIndex < gJpegHuffBackupLength );
			
			HuffPtr = ((u_char *)gJpegHuffBackup);
			if ( gJpegHuffBackupLength != gJpegHuffBackupTail )
			{
				if ( (gJpegHuffBackupLength & 0x01) != 0 )
				{
					*(HuffPtr + gJpegHuffBackupTail) = 0x00;
					*(HuffPtr + gJpegHuffBackupMarkLengthPos) = *(HuffPtr + gJpegHuffBackupMarkLengthPos) + 1;
					if ( *(HuffPtr + gJpegHuffBackupMarkLengthPos) == 0 )
						*(HuffPtr + gJpegHuffBackupMarkLengthPos-1) = *(HuffPtr + gJpegHuffBackupMarkLengthPos-1) + 1;
					gJpegHuffBackupTail++;
				}
				*(HuffPtr + gJpegHuffBackupTail) = 0x0FF;
				gJpegHuffBackupTail++;
			}
			else
			{
				if ( (gJpegHuffBackupLength & 0x01) != 0 )
				{
					*(HuffPtr + gJpegHuffBackupTail) = 0x00;
					*(HuffPtr + gJpegHuffBackupMarkLengthPos) = *(HuffPtr + gJpegHuffBackupMarkLengthPos) + 1;
					if ( *(HuffPtr + gJpegHuffBackupMarkLengthPos) == 0 )
						*(HuffPtr + gJpegHuffBackupMarkLengthPos-1) = *(HuffPtr + gJpegHuffBackupMarkLengthPos-1) + 1;
					gJpegHuffBackupTail++;
				}
			}
		}
        
        // 20080623, not support Huffman Table Count = 3 case !!
        if (gJpegHuffTabCount == 3)
        {
            *Width = 0;
				*Height = 0;
            *Format = A8_JPEG_FORMAT_ERROR;
            return 2;
        }
		return A8_NO_ERROR;
	}
	else
	{
		*Width = 0;
		*Height = 0;
		*Format = A8_JPEG_FORMAT_ERROR;
		return 2;
	}
}


//s_short A8L_DecodeJpegBase( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, s_short Mode, s_short ScaleN, s_short ScaleM, s_short x0, s_short y0, s_short x1, s_short y1 )

/**
 @fn		s_short A8L_DecodeJpegBase( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
 @brief	Decode a jpeg picture from FIFO to a buffer on A8 chip. The format of the
 @n		output image is determined by the parameter "Mode". This API is a basic
 @n		function which is called by the other JPEG decode API.
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	Length - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	A8S_Cameraparam - decompress width,height ScalerN,ScalerM related setting.
 @n		Mode - 1:output RGB565, 2:output Yuv422, 3:output RGB888
 @n		ScaleN : the scaling up factor for Scaler
 @n		ScaleM : the scaling down factor for Scaler
 @n		x0, y0 : the start position of the JPEG grabe range
 @n		x1, y1 : the end position of the JPEG grabe range
 @return	s_short - The JPEG format.
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegBase( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
{
	u_short Status, Temp;//,Quality;
	s_int ByteCount;
	s_int i, LengthForSend, timeout;
	/*s_short HScale;*/
	s_short Width, Height, Format;
	s_short Ratio;

	s_short HuffBufferIndex = 0;
	u_short *JpegDHTEvenAddr;
	u_int AddrTemp;

	//A8L_GetJpegInfo( JpegBufferPtr, Length, &Width, &Height, &Format ,&Quality);
	A8L_ScanJpegMarker( JpegBufferPtr, Length, &Width, &Height, &Format );
	if (Height%2 ==1)
		Height +=1;
	
	if ( Format != A8_JPEG_FORMAT_YUV411 && Format != A8_JPEG_FORMAT_YUV422 && Format != A8_JPEG_FORMAT_YUV420 && Format != A8_JPEG_FORMAT_YUV444 )
		return Format;
	switch (Format) 
	{
		case	A8_JPEG_FORMAT_YUV411:
			if (Width & 0x1F) 
				Width = ((Width >> 5) + 1) << 5;
			if (Height & 0x07)
				Height = ((Height >> 3) + 1) << 3;
			break;
		case	A8_JPEG_FORMAT_YUV420:
			if (Width & 0x0F) 
				Width = ((Width >> 4) + 1) << 4;
			if (Height & 0x0F)
				Height = ((Height >> 4) + 1) << 4;
			break;
		case	A8_JPEG_FORMAT_YUV422:
			if (Width & 0x0F) 
				Width = ((Width >> 4) + 1) << 4;
			if (Height & 0x07)
				Height = ((Height >> 3) + 1) << 3;
			break;
		case	A8_JPEG_FORMAT_YUV444:	
			if (Width & 0x07) 
				Width = ((Width >> 3) + 1) << 3;
			if (Height & 0x07)
				Height = ((Height >> 3) + 1) << 3;
			break;
	}
		
	SetA8RegB( 0x690A, 0x1E );	
	SetA8RegB( 0x690A, 0x00 );	
//Ming
{
	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	// Reset PIP and jpeg line buffer
	SetA8RegW( 0x6510, Temp );
}
	SetA8RegW( 0x6222, 0x0000 );
	
	if ( Width > A8_JPEG_DECODE_MAX_WIDTH )
	{
		AIT_Message_P0("A8_JPEG_DECODE_ERR_OVER_WIDTH\n");
		return A8_UNSUPPORT_ERROR;		// jpeg width over the limit
	}

	if ( A8S_Cameraparam.Mode > 3 || A8S_Cameraparam.Mode < 1 )
	{
		AIT_Message_P0("A8_JPEG_DECODE_ERR_UNKNOWN_FORMAT\n");
		return A8_OUT_OF_RANGE_ERROR;		// unknown output image format
	}
	
	SetA8RegW(0x6020, 0x0000);		/* Disable TG viewfinder mode and viewfinder data output */
//Ming	
{
	SetA8RegW(0x7110, 0x0000);
}	
	/* Reset PIP and jpeg line buffer */
	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	/* Reset PIP and jpeg line buffer */
	SetA8RegW( 0x6510, Temp );
	
	/* Set Scaler Output Address(PIP) */
	SetA8RegW(0x6508, (u_short)(DestAddr & 0xFFFF));	// Image Bank 0 start address. Dest buffer address
	SetA8RegW(0x650A, (u_short)(DestAddr >> 16));	// Image Bank 0 start address. Dest buffer address
	SetA8RegW(0x650C, (u_short)(DestAddr & 0xFFFF));	// Image Bank 1 start address. Dest buffer address
	SetA8RegW(0x650E, (u_short)(DestAddr >> 16));	// Image Bank 1 start address. Dest buffer address
	
	/* if ( Width > 1280 ) */
	if ( Width > MaxAITDecodeJpegWidth )
	{
		SetA8RegB(0x620A, 0x41);		/* JPEG 1/2 down sample with repeat ratio */
		SetA8RegB(0x4F1D, 0x01);		/* Scaler V 1/2 downsample enable */
		A8S_Cameraparam.ScaleN = A8S_Cameraparam.ScaleN<<1;
		Width= Width>>1;
	}
	else
	{
		SetA8RegB(0x620A, 0x01);		/* JPEG repeat ratio */
		SetA8RegB(0x4F1D, 0x00);
	}
	

	SetA8RegW( 0x4F00, 1 );				/* Scaler input x0 */
	SetA8RegW( 0x4F02, Width );			/* Scaler input x1 */
	SetA8RegW( 0x4F04, 1 );				/* Scaler input y0 */
	SetA8RegW( 0x4F06, Height );		/* Scaler input y1 */
	
	SetA8RegW( 0x4F08, A8S_Cameraparam.startx);			/* Scaler output H grabe range */
	SetA8RegW( 0x4F0A, A8S_Cameraparam.endx);
	SetA8RegW( 0x4F0C, A8S_Cameraparam.starty);			/* Scaler output V grabe range */
	SetA8RegW( 0x4F0E, A8S_Cameraparam.endy);
	
	Ratio = A8S_Cameraparam.ScaleM/A8S_Cameraparam.ScaleN;
	if ( Ratio >= 4 )
	{
		if ( (Width & 0x03) )
		{
			SetA8RegB( 0x4F12, 0x00 );			// Auto LPF
	        	SetA8RegB( 0x4F18, 0x01 ); 	// enable edge enhancement
		}
		else
		{
			SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
			SetA8RegB( 0x4F19, 0x06 );		// edge gain
			SetA8RegB( 0x4F1A, 0x03 );		// edge core
			SetA8RegB( 0x4F18, 0x00 ); 	// enable edge enhancement
		}
	}
	else if ( Ratio >= 2 )
	{
		if ( (Width & 0x01) )
		{
			SetA8RegB( 0x4F12, 0x00 );			// Auto LPF
			SetA8RegB( 0x4F18, 0x01 ); 	// enable edge enhancement
		}
		else
		{
			SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
			SetA8RegB( 0x4F19, 0x06 );		// edge gain
			SetA8RegB( 0x4F1A, 0x03 );		// edge core
			SetA8RegB( 0x4F18, 0x00 ); 	// enable edge enhancement
		}
	}
	else if ( Ratio >= 1 )
	{
		SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
		SetA8RegB( 0x4F19, 0x06 );		// edge gain
		SetA8RegB( 0x4F1A, 0x03 );		// edge core
		SetA8RegB( 0x4F18, 0x01 /*0x00*/ ); 	// bypass edge enhancement
	}
	else
	{
		SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
		SetA8RegB( 0x4F18, 0x01 ); 	// bypass edge enhancement
	}
	
	if( A8S_Cameraparam.ScaleN == A8S_Cameraparam.ScaleM)
		SetA8RegB( 0x4F14, 0x01 );	//Bypass scaling
	else
		SetA8RegB( 0x4F14, 0x00 );

	SetA8RegB( 0x4F15, A8S_Cameraparam.ScaleN );	//Scale N
	SetA8RegB( 0x4F16, A8S_Cameraparam.ScaleM);	//Scale M
	//SetA8RegB( 0x4F18, 0x03 );
	//SetA8RegB( 0x4F18, 0x02 ); //Dali-CR-151
	//SetA8RegB( 0x4F18, 0x01 ); 
	//SetA8RegB( 0x4F18, 0x00 ); 	// enable edge enhancement
	SetA8RegB( 0x4F1C, GetA8RegB(0x4F1C) | 0x40 );	// RGB order on output bus is BGR

	SetA8RegW( 0x6210, A8_JPEG_DECODE_ROW_BUF_ADDR & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_DECODE_ROW_BUF_ADDR >> 16 );
	SetA8RegW( 0x6214, A8_JPEG_DECODE_COMP_BUF_START_ADDR & 0xFFFF );
	SetA8RegW( 0x6216, A8_JPEG_DECODE_COMP_BUF_START_ADDR >> 16 );
	SetA8RegW( 0x6218, A8_JPEG_DECODE_COMP_BUF_END_ADDR & 0xFFFF );
	SetA8RegW( 0x621A, A8_JPEG_DECODE_COMP_BUF_END_ADDR >> 16 );

	SetA8RegW( 0x6224, A8_JPEG_DECODE_FIFO_BUF_SIZE & 0xFFFF );
	SetA8RegB( 0x6226, A8_JPEG_DECODE_FIFO_BUF_SIZE >> 16 );
	
	if ( AIT_STATUS_VDO_PLAY != sys_IF_ait_get_status() ){
		SetA8RegW( 0x6210, A8_JPEG_DECODE_ROW_BUF_ADDR_2 & 0xFFFF );
		SetA8RegW( 0x6212, A8_JPEG_DECODE_ROW_BUF_ADDR_2 >> 16 );
		SetA8RegW( 0x6214, A8_JPEG_DECODE_COMP_BUF_START_ADDR_2 & 0xFFFF );
		SetA8RegW( 0x6216, A8_JPEG_DECODE_COMP_BUF_START_ADDR_2 >> 16 );
		SetA8RegW( 0x6218, A8_JPEG_DECODE_COMP_BUF_END_ADDR_2 & 0xFFFF );
		SetA8RegW( 0x621A, A8_JPEG_DECODE_COMP_BUF_END_ADDR_2 >> 16 );
		SetA8RegW( 0x6224, A8_JPEG_DECODE_FIFO_BUF_SIZE_2 & 0xFFFF );
		SetA8RegB( 0x6226, A8_JPEG_DECODE_FIFO_BUF_SIZE_2 >> 16 );	
	}
	SetA8RegW( 0x6222, 0x0000);	//JPEG/VIDEO FIFO Control and Status Register

	Temp = GetA8RegB( 0x5040 );
	SetA8RegB( 0x5040, Temp & 0xFB);	// Disable ping-pong buffer
	if (A8S_Cameraparam.ScaleN > A8S_Cameraparam.ScaleM)
	{
		SetA8RegW(0x6B2E, 0x278);/*070226 */ //JPEG delay counter
	}
	else
	{
		SetA8RegW(0x6B2E, 0x80);		// Default value is 0x80. //JPEG delay counter
	}

	SetA8RegW(0x6510, 0x0004 + A8S_Cameraparam.Mode - 1 );//PIP Source Select

	ByteCount = 0;
	
	SetA8RegW(0x6242, 0x0F1F);	//Reset JPEG Interrupt to Host Status
	SetA8RegW(0x6222, 0x0080);		// Enable JPEG FIFO Path
	SetA8RegB(0x6204, 0x0006);		// Enable JPEG Decoder
	timeout = 0;

	AddrTemp = (u_int ) gJpegLastDHTEndAddr;
	AddrTemp = (AddrTemp & 0xFFFFFFFE);
	JpegDHTEvenAddr = (u_short *)AddrTemp;

	if ( gJpegHuffTabCount < 3 || gJpegHuffBackupEnable )
	{
		do
		{
			LengthForSend = Length + gJpegHuffBackupTail - ByteCount;
			if ( LengthForSend >32 )
				LengthForSend = 16;
			else
				LengthForSend = (LengthForSend+1) >> 1;
				
			Status = GetA8RegB(0x6222);
			if ( Status == 0xA0 )
			{
				//open FIFO

				OpenFIFO(0x6220);
				for (i=0; i<LengthForSend; i++ )
				{
					if ( JpegBufferPtr != JpegDHTEvenAddr )
						PutWordFIFO(*(JpegBufferPtr++));
					else
					{
						if ( HuffBufferIndex < (gJpegHuffBackupTail>>1) )
						{
PutWordFIFO(gJpegHuffBackup[HuffBufferIndex++]);

						}
						else
						{
							if ( gJpegHuffBackupTail != gJpegHuffBackupLength )
								JpegBufferPtr++;	// Skipping 1 word

								PutWordFIFO(*(JpegBufferPtr++));

						}
					}
				}
				ByteCount += (LengthForSend<<1);
			}
			else
			{
				timeout++;
				if (timeout >= (A8_DECODE_JPEG_TIMEOUT>>2) )
				{
					SetA8RegB(0x6223, 0x02);
					SetA8RegW(0x6222, 0x00);
					return A8_TIMEOUT_ERROR;
				}
			}
			Status = GetA8RegW(0x6204);
		}	while ( (ByteCount < (Length+gJpegHuffBackupTail)) && (( Status & 0x02 ) != 0) );
	}
	else
	{
		do
		{
			LengthForSend = Length - ByteCount;
			if ( LengthForSend >32 )
				LengthForSend = 16;
			else
				LengthForSend = (LengthForSend+1) >> 1;
			
			Status = GetA8RegB(0x6222);
			if ( Status == 0xA0 )
			{
				//open FIFO
				OpenFIFO(0x6220);




				ByteCount += (LengthForSend<<1);
				while(LengthForSend--){
							PutWordFIFO(*(JpegBufferPtr++));

				}

//}
			}
			else
			{
				timeout++;
				if (timeout >= (A8_DECODE_JPEG_TIMEOUT>>2) )
				{
					SetA8RegB(0x6223, 0x02);
					SetA8RegW(0x6222, 0x00);
					return A8_TIMEOUT_ERROR;
				}
			}
		Status = GetA8RegW(0x6204);
		}while ( (ByteCount < Length) && (( Status & 0x02 ) != 0) );
	}
	
	SetA8RegB(0x6223, 0x02);
	
	timeout = 0;
	do
	{
		Status = GetA8RegB(0x6223);
		if ( (Status & 0x01) != 0 )
			SetA8RegW(0x6222, 0x0000);
		Status = GetA8RegW(0x6204);
		timeout++;
	}while ((Status & 0x02) == 0x02 && timeout < (A8_DECODE_JPEG_TIMEOUT>>2));	
	
	SetA8RegW(0x6222, 0x0000);
	SetA8RegB( 0x4F18, 0x01 );
	SetA8RegB( 0x4F19, 0x08 );
	SetA8RegB( 0x4F1A, 0x04 );

	if ( timeout >= (A8_DECODE_JPEG_TIMEOUT>>2) )
	{
		AIT_Message_P0("A802:Decode JPEG Timeout as checking status\n");
		SetA8RegB( 0x690A, 0x0C );	
		SetA8RegB( 0x690A, 0x00 );	
		SetA8RegB( 0x4F14, 0x00 );    //+Bing add for M300 scaling issue 070620 
		return A8_TIMEOUT_ERROR;
	}
	else
	{
		/*AIT_Message_P0("A802: Decode Jpeg Picture OK");*/
		SetA8RegB( 0x4F14, 0x00 );    //+Bing add for M300 scaling issue 070620 
		return Format;
	}
}


/**
 @fn		s_short A8L_DecodeJpegPicture( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
 @brief	Decode a jpeg picture to RGB565 image in a buffer on A8 chip.
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	Length - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	A8S_Cameraparam - decompress width,height ScalerN,ScalerM related setting.
 @n		ScaleN : the scaling up factor for Scaler
 @n		ScaleM : the scaling down factor for Scaler
 @n		x0, y0 : the start position of the JPEG grabe range
 @n		x1, y1 : the end position of the JPEG grabe range
 @return	s_short - The JPEG format.
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegPicture( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
{
	A8S_Cameraparam.Mode = 1;
	return A8L_DecodeJpegBase( JpegBufferPtr, Length, DestAddr, A8S_Cameraparam);
}


/**
 @fn		s_short A8L_DecodeJpegToYuv( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
 @brief	Decode a jpeg picture to Yuv422 image in a buffer on A8 chip.
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	Length - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	A8S_Cameraparam - decompress width,height ScalerN,ScalerM related setting.
 @n		ScaleN : the scaling up factor for Scaler
 @n		ScaleM : the scaling down factor for Scaler
 @n		x0, y0 : the start position of the JPEG grabe range
 @n		x1, y1 : the end position of the JPEG grabe range
 @return	s_short - The JPEG format.
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegToYuv( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
{
	A8S_Cameraparam.Mode = 2;
	return A8L_DecodeJpegBase( JpegBufferPtr, Length, DestAddr, A8S_Cameraparam);
}


/**
 @fn		s_short A8L_DecodeJpegToRGB888( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
 @brief	Decode a jpeg picture to RGB888 image in a buffer on A8 chip.
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	Length - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	A8S_Cameraparam - decompress width,height ScalerN,ScalerM related setting.
 @n		ScaleN : the scaling up factor for Scaler
 @n		ScaleM : the scaling down factor for Scaler
 @n		x0, y0 : the start position of the JPEG grabe range
 @n		x1, y1 : the end position of the JPEG grabe range
 @return	s_short - The JPEG format.
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegToRGB888( u_short *JpegBufferPtr, s_int Length, s_int DestAddr, A8S_CameraParam A8S_Cameraparam)
{
	//u_short width,height;
	//A8S_Cameraparam.JpgWidth = width;
	//A8S_Cameraparam.JpgHeight = height;
	A8S_Cameraparam.Mode = 3;
	return A8L_DecodeJpegBase( JpegBufferPtr, Length, DestAddr, A8S_Cameraparam);
}


/**
 @fn		s_short A8L_DecodeJpegBufToFIFOBase( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, u_short OutFormat, s_int YuvLength, s_short YuvWidth )
 @brief	
 @param	JpegBufferAddr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	OutFormat - .
 @param	YuvLength - .
 @param	YuvWidth - .
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegBufToFIFOBase( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, u_short OutFormat, s_int YuvLength, s_short YuvWidth )
{
	u_short Status, Temp;
	s_int JpegByteCount, YuvByteCount;
	s_int i, LengthForRecv;
	s_int JpegBufSize;


	SetA8RegW(0x6020, 0x0000);		// Disable TG viewfinder mode and viewfinder data output

	SetA8RegB( 0x690A, 0x08 );	
	SetA8RegB( 0x690A, 0x00 );	
	
	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	// Reset PIP and jpeg line buffer
	SetA8RegW( 0x6510, Temp );
	
	SetA8RegW( 0x6210, A8_JPEG_DECODE_ROW_BUF_ADDR & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_DECODE_ROW_BUF_ADDR >> 16 );
	SetA8RegW( 0x6214, (u_short)(JpegBufferAddr & 0xFFFF ));
	SetA8RegW( 0x6216, (u_short)(JpegBufferAddr >> 16) );
	JpegBufSize = (JpegLength + 7) & 0xFFFFF8;
	SetA8RegW( 0x6218, (u_short)((JpegBufferAddr + JpegBufSize) & 0xFFFF ));
	SetA8RegW( 0x621A, (u_short)((JpegBufferAddr + JpegBufSize) >> 16 ));
	
	SetA8RegW( 0x6224, (u_short)((JpegBufSize>>3) & 0xFFFF ));
	SetA8RegB( 0x6226, (u_char)((JpegBufSize>>3) >> 16 ));
	
	SetA8RegW(0x6222, 0x0000);

	SetA8RegW(0x6510, 0x0004);

	if  ( OutFormat == 1 )			// output RGB565
		SetA8RegW(0x6B0E, 0x0112);	// Enable Graphic FIFO ( RGB565 )
	else //if ( OutFormat == 2 )		// output Yuv422
		SetA8RegW(0x6B0E, 0x0102);	// Enable Graphic FIFO ( Yuv )

	SetA8RegB(0x6204, 0x0006);		// Enable JPEG Decoder
	
	JpegByteCount = 0;
	YuvByteCount = 0;
	do
		{
		
		LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		while ( LengthForRecv > 0 )
			{
			if ( LengthForRecv > 0 )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6B28;
				A8IndCmdP = HII_RW_REG_W;
				for (i=0; i<YuvWidth; i++ )
					*DestAddr++ = A8IndDataPW(i);
				YuvByteCount += YuvWidth;
				}
			
			LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
			}
		
		Status = GetA8RegW(0x6204);
		}
	while ( (YuvByteCount < YuvLength) && (( Status & 0x02 ) != 0) );
	
	do
		{
		LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		while ( LengthForRecv > 0 )
			{
			if ( LengthForRecv > 0 )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6B28;
				A8IndCmdP = HII_RW_REG_W;
				for (i=0; i<YuvWidth; i++ )
					*DestAddr++ = A8IndDataPW(i);
				YuvByteCount += YuvWidth;
				}
			
			LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
			}
			
		Status = GetA8RegW(0x6204);
		}
	while ((YuvByteCount < YuvLength) && (Status & 0x02) == 0x02 );	
	
	
	return (s_short)YuvByteCount;
}


/**
 @fn		s_short A8L_DecodeJpegBufToRGB( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_int YuvLength, s_short YuvWidth )
 @brief	
 @param	JpegBufferAddr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	YuvLength - .
 @param	YuvWidth - .
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegBufToRGB( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_int YuvLength, s_short YuvWidth )
{
	return A8L_DecodeJpegBufToFIFOBase( JpegBufferAddr, JpegLength, DestAddr, 1, YuvLength, YuvWidth );
}


/**
 @fn		s_short A8L_DecodeJpegBufToYuv( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_int YuvLength, s_short YuvWidth )
 @brief	
 @param	JpegBufferAddr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	YuvLength - .
 @param	YuvWidth - .
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegBufToYuv( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_int YuvLength, s_short YuvWidth )
{
	return A8L_DecodeJpegBufToFIFOBase( JpegBufferAddr, JpegLength, DestAddr, 2, YuvLength, YuvWidth );
}


/**
 @fn		s_short A8L_DecodeRotateJpegBufToFIFO( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_short YuvWidth, s_short YuvHeight )
 @brief	
 @param	JpegBufferAddr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	YuvWidth - .
 @param	YuvHeight - .
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeRotateJpegBufToFIFO( u_int JpegBufferAddr, s_int JpegLength, u_short *DestAddr, s_short YuvWidth, s_short YuvHeight )
{
	u_short Status, Temp;
	s_int JpegByteCount, YuvByteCount;
	s_int YuvLength;
	s_int i, LengthForRecv;
	s_int JpegBufSize;
//	u_short OutFormat;
	u_short *DestPtr;
	s_int DestOffset;

	YuvLength = YuvWidth * YuvHeight;
	SetA8RegW(0x6020, 0x0000);		// Disable TG viewfinder mode and viewfinder data output

	SetA8RegB( 0x690A, 0x08 );	
	SetA8RegB( 0x690A, 0x00 );	
	
	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	// Reset PIP and jpeg line buffer
	SetA8RegW( 0x6510, Temp );
	
	SetA8RegW( 0x6210,(A8_JPEG_DECODE_ROW_BUF_ADDR & 0xFFFF ));
	SetA8RegW( 0x6212,(A8_JPEG_DECODE_ROW_BUF_ADDR >> 16 ));
	SetA8RegW( 0x6214, (u_short)(JpegBufferAddr & 0xFFFF ));
	SetA8RegW( 0x6216, (u_short)(JpegBufferAddr >> 16 ));
	JpegBufSize = (JpegLength + 7) & 0xFFFFF8;
	SetA8RegW( 0x6218,(u_short)((JpegBufferAddr + JpegBufSize) & 0xFFFF ));
	SetA8RegW( 0x621A, (u_short)((JpegBufferAddr + JpegBufSize) >> 16 ));
	
	SetA8RegW( 0x6224, (u_short)(JpegBufSize>>3) & 0xFFFF );
	SetA8RegB( 0x6226, (u_char)((JpegBufSize>>3) >> 16 ));
	
	SetA8RegW(0x6222, 0x0000);

	SetA8RegW(0x6510, 0x0004);
/* dead code prevent Chuls 080513 */
//	OutFormat = 1;
//	if  ( OutFormat == 1 )			// output RGB565
		SetA8RegW(0x6B0E, 0x0112);	// Enable Graphic FIFO ( RGB565 )
//	else //if ( OutFormat == 2 )		// output Yuv422
//		SetA8RegW(0x6B0E, 0x0102);	// Enable Graphic FIFO ( Yuv )

	SetA8RegB(0x6204, 0x0006);		// Enable JPEG Decoder
	
	JpegByteCount = 0;
	YuvByteCount = 0;
	DestPtr = DestAddr + (YuvLength - YuvHeight);
	DestOffset = 0;
	do
		{
		
		LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		while ( LengthForRecv > 0 )
			{
			if ( LengthForRecv > 0 )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6B28;
				A8IndCmdP = HII_RW_REG_W;
				for (i=0; i<YuvWidth; i++ )
					{
					*(DestPtr-DestOffset) = A8IndDataPW(i);
					DestOffset += YuvHeight;
					}
				DestPtr++;
				DestOffset = 0;
				YuvByteCount += YuvWidth;
				}
			
			LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
			}
		
		Status = GetA8RegW(0x6204);
		}
	while ( (YuvByteCount < YuvLength) && (( Status & 0x02 ) != 0) );
	
	do
		{
		LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		while ( LengthForRecv > 0 )
			{
			if ( LengthForRecv > 0 )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6B28;
				A8IndCmdP = HII_RW_REG_W;
				for (i=0; i<YuvWidth; i++ )
					{
					*(DestPtr-DestOffset) = A8IndDataPW(i);
					DestOffset += YuvHeight;
					}
				DestPtr++;
				DestOffset = 0;
				YuvByteCount += YuvWidth;
				}
				
			LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
			}
			
		Status = GetA8RegW(0x6204);
		}
	while ((YuvByteCount < YuvLength) && (Status & 0x02) == 0x02 );	
	
	return (s_short)YuvByteCount;
}


/**
 @fn		s_short A8L_DecodeJpegViaFIFOBase( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr, u_short OutFormat )
 @brief	Decode JPEG(on Host) to RAW data(on Host).
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @param	OutFormat - .
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegViaFIFOBase( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr, u_short OutFormat )
{
	u_short Status, Temp,Quality;
	s_int JpegByteCount, RawByteCount;
	s_int i, LengthForSend, LengthForRecv;
	s_short Width, Height, Format;
	s_int RawLength, RawWidth ;
	s_short RetErr;

	RetErr = A8L_GetJpegInfo( JpegBufferPtr, JpegLength, &Width, &Height, &Format ,&Quality);
	if ( Format != A8_JPEG_FORMAT_YUV411 && Format != A8_JPEG_FORMAT_YUV422 && Format != A8_JPEG_FORMAT_YUV420 && Format != A8_JPEG_FORMAT_YUV444 )
		return A8_UNSUPPORT_ERROR;

	if(RetErr != 0)
		return A8_JPEG_FORMAT_NOT_BASELINE;
		
	RawLength = Width * Height * 2;
	RawWidth = Width;
		
	SetA8RegW(0x6020, 0x0000);		// Disable TG viewfinder mode and viewfinder data output
//Ming	
	SetA8RegW(0x7110, 0x0000);

	SetA8RegB( 0x690A, 0x08 );	
	SetA8RegB( 0x690A, 0x00 );	

	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	// Reset PIP and jpeg line buffer
	SetA8RegW( 0x6510, Temp );
	
	SetA8RegW( 0x6210, A8_JPEG_FIFO_DECODE_ROW_BUF_ADDR & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_FIFO_DECODE_ROW_BUF_ADDR >> 16 );
	SetA8RegW( 0x6214, A8_JPEG_FIFO_DECODE_COMP_BUF_START_ADDR & 0xFFFF );
	SetA8RegW( 0x6216, A8_JPEG_FIFO_DECODE_COMP_BUF_START_ADDR >> 16 );
	SetA8RegW( 0x6218, A8_JPEG_FIFO_DECODE_COMP_BUF_END_ADDR & 0xFFFF );
	SetA8RegW( 0x621A, A8_JPEG_FIFO_DECODE_COMP_BUF_END_ADDR >> 16 );

	SetA8RegW( 0x6224, A8_JPEG_FIFO_DECODE_FIFO_BUF_SIZE & 0xFFFF );
	SetA8RegB( 0x6226, A8_JPEG_FIFO_DECODE_FIFO_BUF_SIZE >> 16 );
        SetA8RegB(0x6902, GetA8RegB(0x6902)|0x40); //enable fast jpeg decode clock
        
	//if(Width>=800)
	//SetA8RegB(0x620A, GetA8RegB(0x620A)|0x80); //decode double buffer
	SetA8RegB(0x620A, 0x01); //disable decode double buffer
	
	SetA8RegW(0x6222, 0x0000);
	SetA8RegW(0x6510, 0x0004);

	if ( OutFormat == 1 )			// output RGB565
		SetA8RegW(0x6B0E, 0x0112);	// Enable Graphic FIFO ( RGB565 )
	else //if ( OutFormat == 2 )	// output Yuv422
		SetA8RegW(0x6B0E, 0x0102);	// Enable Graphic FIFO ( Yuv )

	SetA8RegW(0x6222, 0x0080);		// Enable JPEG FIFO Path
	SetA8RegB(0x6204, 0x0006);		// Enable JPEG Decoder
	
	JpegByteCount = 0;
	RawByteCount = 0;
	do	//Input jpeg source data from host
	{
		LengthForSend = JpegLength - JpegByteCount;
		if ( LengthForSend >32 )
			LengthForSend = 16;
		else
			LengthForSend = (LengthForSend+1) >> 1;
			
		Status = GetA8RegB(0x6222);
		if ( Status == 0xA0 )
		{
			A8IndCmdP = HII_SET_ADDR_L;
			A8IndDataPW(0) = 0x6220;
			A8IndCmdP = HII_RW_REG_W;
#if 0			
			for (i=0; i<LengthForSend; i++ )
				A8IndDataPW(i) = *(JpegBufferPtr++);
			JpegByteCount += (LengthForSend<<1);
#else
			JpegByteCount += (LengthForSend<<1);
			while(LengthForSend--){
				A8IndDataPW(LengthForSend) = *(JpegBufferPtr++);
			}
#endif
		}
		
		LengthForRecv = GetA8RegB(0x6B27) & 0x1F;	//FIFO un-read space
		while ( LengthForRecv > 0 )
		{
			if ( LengthForRecv > 0 )
			{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6B28;
				A8IndCmdP = HII_RW_REG_W;
#if 0				
				for (i=0; i<RawWidth; i++ )
					*DestAddr++ = A8IndDataPW(i);
				RawByteCount += (RawWidth<<1);
#else
				i=RawWidth;
				RawByteCount += (RawWidth<<1);
				while(i--){
					*DestAddr++ = A8IndDataPW(i);
				}
#endif
			}
			
			LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		}
		
		Status = GetA8RegW(0x6204);
	}while ( (JpegByteCount < JpegLength) && (( Status & 0x02 ) != 0) );
	
	SetA8RegB(0x6223, 0x02);
	
	do
	{
		LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		while ( LengthForRecv > 0 )
		{
			if ( LengthForRecv > 0 )
			{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6B28;
				A8IndCmdP = HII_RW_REG_W;
#if 0				
				for (i=0; i<RawWidth; i++ )
					*DestAddr++ = A8IndDataPW(i);
				RawByteCount += (RawWidth<<1);
#else
				i = RawWidth;
				RawByteCount += (RawWidth<<1);
				while(i--){
					*DestAddr++ = A8IndDataPW(i);
				}
#endif
			}
			
			LengthForRecv = GetA8RegB(0x6B27) & 0x1F;
		}
			
		Status = GetA8RegW(0x6204);
	}while ((RawByteCount < RawLength) && (Status & 0x02) == 0x02 );	
	
	SetA8RegW(0x6222, 0x0000);
	SetA8RegW(0x6B0E, 0x0000);
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_DecodeJpegToRGBViaFIFO( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr )
 @brief	
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegToRGBViaFIFO( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr )
{
	return A8L_DecodeJpegViaFIFOBase( JpegBufferPtr, JpegLength, DestAddr, 1 );
}


/**
 @fn		s_short A8L_DecodeJpegToYuvViaFIFO( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr )
 @brief	
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	JpegLength - the JPEG image length.
 @param	DestAddr - the address for the decompressed image on host.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegToYuvViaFIFO( u_short *JpegBufferPtr, s_int JpegLength, u_short *DestAddr )
{
	return A8L_DecodeJpegViaFIFOBase( JpegBufferPtr, JpegLength, DestAddr, 2 );
}


/**
 @fn		s_short A8L_DecodeJpegDirectShow( u_short *JpegBufferPtr, s_int Length )
 @brief	Decode JPEG data, and then, directly show to LCD.
 @param	JpegBufferPtr - the buffer address for JPEG image on host.
 @param	Length - the JPEG image length.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_DecodeJpegDirectShow( u_short *JpegBufferPtr, s_int Length )
{
	u_short Status, Temp,Quality;
	s_int ByteCount;
	s_int i, LengthForSend;
	s_short Width, Height, Format;
	u_short x0, y0, x1, y1;
	u_short ScaleN, ScaleM;
	WINDOW_INFO WinInfo;
	
	A8L_GetWindowAttribute( A8_PIP_WINDOW, &WinInfo );
	A8L_GetJpegInfo( JpegBufferPtr, Length, &Width, &Height, &Format ,&Quality);
	if ( Format != A8_JPEG_FORMAT_YUV411 && Format != A8_JPEG_FORMAT_YUV422 && Format != A8_JPEG_FORMAT_YUV420 && Format != A8_JPEG_FORMAT_YUV444 )
		return Format;
		
	if ( Width > A8_JPEG_DECODE_MAX_WIDTH )
		return A8_UNSUPPORT_ERROR;		// jpeg width over the limit
	
	ScaleM = 32;
	ScaleN = (( WinInfo.width << 5 ) + Width - 1 ) / Width;
	Temp = (( WinInfo.height << 5 ) + Height - 1 ) / Height;
	if ( ScaleN < Temp )
		ScaleN = Temp;
	if (ScaleN==0)
		ScaleN=1;

	if ( ScaleN > 127 )
		return A8_OUT_OF_RANGE_ERROR;		// jpeg picture size too small to fit the window
	else if ( ScaleN < 2 )
		return A8_OUT_OF_RANGE_ERROR;		// jpeg picture size too large to fit the window
		
	x0 = (((Width*ScaleN)>>5)-WinInfo.width)/2 + 1;
	y0 = (((Height*ScaleN)>>5)-WinInfo.height)/2 + 1;
	x1 = x0 + WinInfo.width -1;
	y1 = y0 + WinInfo.height -1;
		
	
	SetA8RegW(0x6020, 0x0000);		// Disable TG viewfinder mode and viewfinder data output
	
	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	// Reset PIP and jpeg line buffer
	SetA8RegW( 0x6510, Temp );
	
	SetA8RegB( 0x5049, 0x01 );
	Temp = GetA8RegB( 0x504B );
	SetA8RegB( 0x504B, Temp | 0x01 );	// Enable PIP buffer bypass mode
	Temp = GetA8RegB( 0x500A );
	SetA8RegB( 0x500A, Temp|0x02 );		// Update LCD a frame
	


	if ( Width > MaxAITDecodeJpegWidth )
		{
		SetA8RegB(0x620A, 0x41);		/* JPEG 1/2 down sample with repeat ratio */
		SetA8RegB(0x4F1D, 0x01);		/* Scaler V 1/2 downsample enable */
		ScaleN = ScaleN<<1;
		Width= Width>>1;
		}
	else
		{
		SetA8RegB(0x620A, 0x01);		// JPEG repeat ratio
		SetA8RegB(0x4F1D, 0x00);
		}

	SetA8RegW( 0x4F00, 1 );				// Scaler input x0
	SetA8RegW( 0x4F02, Width );			// Scaler input x1
	SetA8RegW( 0x4F04, 1 );				// Scaler input y0
	SetA8RegW( 0x4F06, Height );		// Scaler input y1
	
	SetA8RegW( 0x4F08, x0 );			// Scaler output H grabe range
	SetA8RegW( 0x4F0A, x1 );			//
	SetA8RegW( 0x4F0C, y0 );			// Scaler output V grabe range
	SetA8RegW( 0x4F0E, y1 );			//
	
	//SetA8RegB( 0x4F10, 0x00 );			// Clear LPF setting
	SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
	SetA8RegB( 0x4F14, 0x00 );
	SetA8RegB( 0x4F15, ScaleN );
	SetA8RegB( 0x4F16, ScaleM );
	SetA8RegB( 0x4F1C, 0x33 );			// RGB565 and Yuv422

	SetA8RegW( 0x6210, A8_JPEG_DECODE_ROW_BUF_ADDR & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_DECODE_ROW_BUF_ADDR >> 16 );
	SetA8RegW( 0x6214, A8_JPEG_DECODE_COMP_BUF_START_ADDR & 0xFFFF );
	SetA8RegW( 0x6216, A8_JPEG_DECODE_COMP_BUF_START_ADDR >> 16 );
	SetA8RegW( 0x6218, A8_JPEG_DECODE_COMP_BUF_END_ADDR & 0xFFFF );
	SetA8RegW( 0x621A, A8_JPEG_DECODE_COMP_BUF_END_ADDR >> 16 );

	SetA8RegW( 0x6224, A8_JPEG_DECODE_FIFO_BUF_SIZE & 0xFFFF );
	SetA8RegB( 0x6226, A8_JPEG_DECODE_FIFO_BUF_SIZE >> 16 );
	
	SetA8RegW( 0x6222, 0x0000);

	Temp = GetA8RegB( 0x5040 );
	SetA8RegB( 0x5040, (Temp & 0xFB) );		// Disable ping-pong buffer


	ByteCount = 0;

	SetA8RegW(0x6222, 0x0080);		// Enable JPEG FIFO Path
	SetA8RegB(0x6204, 0x0006);		// Enable JPEG Decoder
	
	do
		{
		LengthForSend = Length - ByteCount;
		if ( LengthForSend >32 )
			LengthForSend = 16;
		else
			LengthForSend = (LengthForSend+1) >> 1;
			
		Status = GetA8RegB(0x6222);
		if ( Status == 0xA0 )
			{
			for (i=0; i<LengthForSend; i++ )
				SetA8RegW(0x6220, *(JpegBufferPtr++));
			ByteCount += (LengthForSend<<1);
			}
		Status = GetA8RegW(0x6204);
		}
	while ( (ByteCount < Length) && (( Status & 0x02 ) != 0) );
	
	SetA8RegB(0x6223, 0x02);
	
	do
		{
		Status = GetA8RegW(0x6204);
		}
	while ((Status & 0x02) == 0x02 );	
	
	SetA8RegW(0x6222, 0x0000);
	
	return Format;
}

/**
 @fn		s_short A8L_PlaybackJpegPicture( u_short *ImgAddr, s_int Length, s_short WinID )
 @brief	Decode JPEG data to AIT specific window buffer.
 @param	ImgAddr - the buffer address for JPEG image on host.
 @param	Length - the JPEG image length.
 @param	WinID - the specific window of AIT.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_PlaybackJpegPicture( u_short *ImgAddr, s_int Length, s_short WinID )
{
	s_short PictWidth, PictHeight, Format;
	u_short ScaleM, ScaleN, Temp, sx, sy;
	WINDOW_INFO WinInfo;
	u_short width=0, height=0,Quality; /* prevent Chuls 080513 initial struct */
	A8S_CameraParam A8S_Cameraparam;
	A8L_GetWindowAttribute( WinID, &WinInfo );
	if ( WinInfo.colordepth != A8_DEPTH_16B )
		return A8_INCORRECT_PARA_ERROR;
	
	A8L_GetJpegInfo( ImgAddr, Length, &PictWidth, &PictHeight, &Format ,&Quality);
	
		{
		ScaleM = 32;
		if ( WinInfo.width < PictWidth && WinInfo.height < PictHeight )
			{
			ScaleN = (( WinInfo.width << 5 ) + PictWidth - 1 ) / PictWidth;
			Temp = (( WinInfo.height << 5 ) + PictHeight - 1 ) / PictHeight;
			}
		else
			{
			ScaleN = (( WinInfo.width << 5 ) + PictWidth - 2 ) / (PictWidth-1);
			Temp = (( WinInfo.height << 5 ) + PictHeight - 2 ) / (PictHeight-1);
			}
		if ( ScaleN < Temp )
			ScaleN = Temp;
		if (ScaleN==0)
			ScaleN=1;
		sx = (((PictWidth*ScaleN)>>5)-WinInfo.width)/2;
		sy = (((PictHeight*ScaleN)>>5)-WinInfo.height)/2;
		}
	if ( ScaleN > 127 )
		return A8_OUT_OF_RANGE_ERROR;		// jpeg picture size too small to fit the window
	else if ( ScaleN < 2 )
		return A8_OUT_OF_RANGE_ERROR;		// jpeg picture size too large to fit the window
	
	A8S_Cameraparam.JpgWidth = width;
	A8S_Cameraparam.JpgHeight = height;
	A8S_Cameraparam.ScaleN = ScaleN;
	A8S_Cameraparam.ScaleM = ScaleM;
	A8S_Cameraparam.startx = sx+1;
	A8S_Cameraparam.starty = sy+1;
	A8S_Cameraparam.endx = sx+WinInfo.width;
	A8S_Cameraparam.endy = sy+WinInfo.height;
	A8L_DecodeJpegPicture(ImgAddr, Length, WinInfo.baseaddr, A8S_Cameraparam);
	return A8_NO_ERROR;
}

/**
 @fn		s_short A8L_SetJpegQuality( s_short JpegQuality )
 @brief	Setup JPEG Quality Level.
 @param	JpegQuality - the JPEG Quality Level.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_SetJpegQuality( s_short JpegQuality )
{
	if ( JpegQuality >= 0 && JpegQuality <= 7 )
	{
		SendA8Cmd( (JpegQuality << 8) + A8_HOST_CMD_SET_JPEG_QUALITY );

		if(A8L_CheckReadyForA8Command())
		{
			return A8_TIMEOUT_ERROR;
		}
		return A8_NO_ERROR;
	}
	else
		return A8_OUT_OF_RANGE_ERROR;		
}


/**
 @fn		s_short A8L_DownloadA8Firmware( u_char *FWBuffer, s_short Length )
 @brief	Download AIT Fireware into AIT program RAM.
 @param	FWBuffer -the address of firmware buffer.
 @param	Length -the length of the firmware, for 701/703 must be 16K Bytes.
 @return	s_short -0:Download successfully, 1:Download failed. .
 @note	.
 @bug	N/A.
*/
s_short A8L_DownloadA8Firmware( u_char *FWBuffer, s_short Length )
{
	s_short i;
	
	SetA8RegB( 0x6914, 0x01 );
	SetA8RegB( 0x6901, 0x46 );
	SetA8RegW( 0x6540, 0x00 );

	for ( i=0; i<Length; i++ )
	{
		SetA8RegB( i, *(FWBuffer+i) );
	}
	
	SetA8RegB( 0x6914, 0x00 );
	SetA8RegB( 0x6901, 0x40 );
	sys_IF_ait_delay1ms(1);

  	SetA8RegB(0x6960, (GetA8RegB(0x6960) | 0x70)); // 20080614, buffer memory enable!
    sys_IF_ait_delay1ms(1);

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}

	return A8_NO_ERROR;
}

/**
 @fn		s_short A8L_InitialLCD()
 @brief	Init LCD by FW.
 @return	s_short  .
 @note	( LCD should be init by Host, this function For Test only!! ).
 @bug	N/A.
*/
s_short A8L_InitialLCD()
{
	SendA8Cmd( A8_HOST_CMD_INIT_LCD );
	return A8_NO_ERROR;
}

/**
 @fn		u_int A8L_ConvertImageFIFOToBuf(u_short *IMGPtr, u_short ScalN, u_short ScalM, u_short SrcW, u_short SrcH, u_char SrcFormat, 
 @n										u_short TarW, u_short TarH, u_char TarFormat, u_int BufADDR)
 @brief	Transfer a RGB565/YUV422 image to AIT memory with size scaling. Notice that the RAW (source)
 @n		image is in a host side buffer pointed by IMGPtr. 
 @n		The RAW image dimension is specified by SrcW and SrcH and will be UpScale/DownScale. The target memory in AIT 
 @n		is RGB565/YUV422 is 16 bits per pixel. 
 @param	IMGPtr -RGB565/YUV422 data buffer pointer on host.
 @param	ScalN : Scaler N Ratio.
 @n		ScalM : Scaler M Ratio.
 @n		SrcW : Source RAW Image Width.
 @n		SrcH : Source RAW Image Height.
 @n		SrcFormat : Source RAW Image Format.
 @n		TarW : Target RAW Image Width.
 @n		TarH : Target RAW Image Height.
 @n		TarFormat : Target RAW Image Format.
 @param	BufADDR : Memory Address in AIT chip.
 @return	s_short - 0 : Success, 1 : Fail.
 @bug	N/A.
*/
s_short A8L_ConvertImageFIFOToBuf(u_short *IMGPtr, u_short ScalN, u_short ScalM, 
										u_short SrcW, u_short SrcH, u_char SrcFormat, 
										u_short TarW, u_short TarH, u_char TarFormat, u_int BufADDR)
{
	s_int SrcLength;
	u_short SrcRowChunk, GphFIFOCount;
	s_short RowCount, PixCount;
	u_short i, timeout;
	u_short *ImgPixPtr;
	u_short ScalWidth, ScalHeight, GrabX, GrabY;

	SetA8RegB( 0x690A, 0x1E );	
	SetA8RegB( 0x690A, 0x00 );	
	SetA8RegB( 0x6B24, 0x02);
	SetA8RegB( 0x6B24, 0x00);

	SetA8RegW( 0x6510, 0x0400 );		
	SetA8RegW( 0x6510, 0x0000 );		
	SetA8RegW( 0x6510, 0x0004 );

	SetA8RegW( 0x6B2E, 0x100);	
	
	SetA8RegW( 0x4F00, 0x0001 );
	SetA8RegW( 0x4F02, SrcW );
	SetA8RegW( 0x4F04, 0x0001 );
	SetA8RegW( 0x4F06, SrcH );

	if (ScalN > ScalM)
	{
		ScalWidth = (SrcW-1) * ScalN / ScalM ; 
		ScalHeight = (SrcH-1) * ScalN / ScalM ; 
		GrabX = (ScalWidth - TarW)/2 + 1;
		GrabY = (ScalHeight- TarH)/2 + 1;

		SetA8RegW( 0x4F08, GrabX );
		SetA8RegW( 0x4F0A, GrabX + (TarW-1));
		SetA8RegW( 0x4F0C, GrabY );
		SetA8RegW( 0x4F0E, GrabY + (TarH-1));
	}
	else
	{
		ScalWidth = SrcW * ScalN / ScalM ; 
		ScalHeight = SrcH * ScalN / ScalM ; 
		GrabX = (ScalWidth - TarW)/2 + 1;
		GrabY = (ScalHeight- TarH)/2 + 1;
    
		// Note !! (ScalWidth, ScalHeight) must be same as (TarW, TarH)
		SetA8RegW( 0x4F08, GrabX );
		SetA8RegW( 0x4F0A, GrabX + (TarW-1));
		SetA8RegW( 0x4F0C, GrabY );
		SetA8RegW( 0x4F0E, GrabY + (TarH-1));
	}
	
	SetA8RegB( 0x4F14, 0x00 );
	SetA8RegB( 0x4F15, ScalN); 
	SetA8RegB( 0x4F16, ScalM);
	SetA8RegB( 0x4F12, 0x00 );
	SetA8RegB( 0x4F18, 0x01 ); //disable edge enchancement
	SetA8RegB( 0x4F1D, 0x00 );
	
	//SetA8RegW( 0x6510, 0x0400 );		
	//SetA8RegW( 0x6510, 0x0000 );		
	//SetA8RegW( 0x6510, 0x0004 );

	if ( TarFormat == 1)			// RGB format
		SetA8RegB( 0x6510, 0x04);
	else if ( TarFormat == 2 )		// YUV format
		SetA8RegB( 0x6510, 0x04 + 1);

	SetA8RegW( 0x6500, BufADDR & 0xFFFF);
	SetA8RegW( 0x6502, BufADDR >>16);

	// !!! Note, Take case this address for Graphic-Engineer one RAW line buffer !!!
	if (TarH > 96) // 0418 OSD garbage line issue in VPS
	{
		SetA8RegW( 0x6B50, 0x4000 ); // 0414 open for 701, src_width*2, 	0xa000
		SetA8RegW( 0x6B52, 0x0001 ); // 0414 open for 701
	}
	else
	{
		SetA8RegW( 0x6B50, 0x4000 ); // 0414 open for 701, src_width*2,	0x7000
		SetA8RegW( 0x6B52, 0x0001 ); // 0414 open for 701
	}
		
//	SetA8RegW( 0x6B50, 0x7000 );		// 0x27000 is for AIT704, 160KB.
//	SetA8RegW( 0x6B52, 0x0002 );
	
	if ( SrcFormat == 1 )					/* Source image format RGB565 */
		SetA8RegW( 0x6B0E, 0x0102 );		/* Graphic FIFO path enable */
	else if (SrcFormat == 2 )				/* Source image format YUV422 */
		SetA8RegW( 0x6B0E, 0x0103 );		/* Graphic FIFO path enable */
		
	SetA8RegW( 0x6B06, SrcW);
	SetA8RegW( 0x6B0C, SrcH);

	SetA8RegB( 0x6B24, 0x04 );

	ImgPixPtr = IMGPtr;
	
	if ( SrcH >= 8 )
		SrcRowChunk = SrcW << 3;
	else
		SrcRowChunk = SrcW*SrcH;
		
	SrcLength = SrcW * SrcH;
	RowCount = SrcH;
	while ( RowCount > 0 )
	{
		PixCount = SrcRowChunk;
		do
		{
			GphFIFOCount = GetA8RegB( 0x6B26 ) & 0x01F;
			if ( GphFIFOCount > 0 )
			{
				if ( PixCount > GphFIFOCount )
				{
					A8IndCmdP = HII_SET_ADDR_L;
					A8IndDataPW(0) = 0x6B28;
					A8IndCmdP = HII_RW_REG_W;
					for ( i=0; i<GphFIFOCount; i++ )
						A8IndDataPW(i) = *ImgPixPtr++;
					PixCount -= GphFIFOCount;
				}
				else
				{
					A8IndCmdP = HII_SET_ADDR_L;
					A8IndDataPW(0) = 0x6B28;
					A8IndCmdP = HII_RW_REG_W;
					for ( i=0; i<PixCount; i++ )
						A8IndDataPW(i) = *ImgPixPtr++;
					PixCount = 0;
				}
			}
		}
		while ( PixCount > 0 );
		
		RowCount = RowCount - 8;
		
		if ( RowCount < 8 )
			SrcRowChunk = SrcW * RowCount;
	}


	timeout = 0;
	while ((GetA8RegB(GRH_ENABLE_CTL) & GRH_SCALE_ENABLE) && timeout < 0x1000)
	{
		for (i=0; i<100; i++);
		timeout ++;
	}

	SetA8RegW( 0x6222, 0x0000 );
	SetA8RegW( 0x6510, 0x0000 );
	SetA8RegW( 0x6B0E, 0x0000 );
	
	if (timeout >= 0x1000)
		return A8_TIMEOUT_ERROR;
	else
		return A8_NO_ERROR;
}


/**
 @fn		u_int A8L_TranslateImageToJpegBase( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo )
 @brief	Encode a RGB565/YUV422 image to a jpeg picture with size scaling. Notice that the RAW (source)
 @n		image is in a host side buffer pointed by RgbPtr. The encoded jpeg picture will be 
 @n		stored in a host side buffer pointed by JpegBufferPtr after the function is done.
 @n		The RAW image dimension is specified by SrcWidth and SrcHeight. The max of SrcWidth is
 @n		640.The jpeg image size is defined by JpegWidth and JpegHeight. The RAW image width must
 @n		be a multiply of 4.	The jpeg width must be a multiply of 16. The size scaling ratio is 
 @n		controlled by UpScale and DownScale. Their range is from 1 to 127. The scale ratio is equal
 @n		to UpScale/DownScale. The RGB565/YUV422 is 16 bits per pixel. 
 @param	RgbPtr -RGB565/YUV422 data buffer pointer on host.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @n		SrcWidth : RGB565 image width. the width max is 640.
 @n		SrcHeight : RGB565 image height.
 @n		JpegWidth : the jpeg image width, the source image width must be the same and 1 multiply of 16
 @n		JpegHeight : the jpeg image height, the source image height must be the same
 @n		UpScale : the factor for scaling up.
 @n		DownScale : the factor for scaling down. The scale ratio is UpScale/DownScale.
 @param	JpegBufferAddr -JPEG data buffer pointer on host.
 @return	u_int - file size.
 @note	Jpeg picture dimension limitation
 @n		JpegWidth <= SrcWidth * UpScale/DownScale
 @n		JpegHeight <= SrcHeight * UpScale/DownScale.
 @bug	N/A.
*/
// @param	makerName - the array of maker name (this is for EXIF).
// @param	modelName - the array of model name (this is for EXIF).
// @param	DateTimeInfo - the array of Data Time Info (this is for EXIF).
//u_int A8L_TranslateImageToJpegBase( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo )
u_int A8L_TranslateImageToJpegBase( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
{
	s_int i, JpegPictLength;//, Status, Length,filesize;
	s_int PixCount, RowCount;
	s_int SrcLength, SrcRowChunk;
//	s_short JpegWidth, JpegHeight;
	u_short *ImgPixPtr, *JpegPtr;
	u_short JpegStatus, JpegFIFOCount, GphFIFOCount;//,JpegEncodeDone=0;
	
#ifdef EXIFSUPPORT 
	u_char *ExifPtr; //exif
	ExifPtr = (u_char *)JpegBufferAddr;
#ifndef EXIFSUPPORT 
	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();
#endif
	for(i=0;i<0x314;i++)  
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegPtr = JpegBufferAddr + 0x189; 
#else	
	JpegPtr = JpegBufferAddr; 
#endif	

	SetA8RegB( 0x690A, 0x1E );	
	SetA8RegB( 0x690A, 0x00 );	
	//software reset
	SetA8RegB( 0x6B24, 0x02);
	SetA8RegB( 0x6B24, 0x00);
	
	SetA8RegW(0x6B2E, 0x80);	// 070516, A127 updated
	SetA8RegB( 0x6242, 0x03 );	
	SetA8RegW( 0x6222, 0x0000 );
	// Set Jpeg Encode
	SetA8RegW( 0x6200, A8S_Cameraparam.JpgWidth );
	SetA8RegW( 0x6202, A8S_Cameraparam.JpgHeight );
	
	SetA8RegW( 0x4F00, 0x0001 );
	SetA8RegW( 0x4F02, A8S_Cameraparam.SrcWidth );
	SetA8RegW( 0x4F04, 0x0001 );
	SetA8RegW( 0x4F06, A8S_Cameraparam.SrcHeight );
	SetA8RegW( 0x4F08, 0x0001 );
	SetA8RegW( 0x4F0A, A8S_Cameraparam.JpgWidth);
	SetA8RegW( 0x4F0C, 0x0001 );
	SetA8RegW( 0x4F0E, A8S_Cameraparam.JpgHeight);
	
	if(A8S_Cameraparam.ScaleM == A8S_Cameraparam.ScaleN) // 0425 wrong value fix
		SetA8RegB( 0x4F14, 0x01 );
	else
		SetA8RegB( 0x4F14, 0x00 );
	SetA8RegB( 0x4F15, A8S_Cameraparam.ScaleN); 
	SetA8RegB( 0x4F16, A8S_Cameraparam.ScaleM);
	SetA8RegB( 0x4F12, 0x01 );
	SetA8RegB( 0x4F1D, 0x00 );
	
	SetA8RegW( 0x6510, 0x0400 );		
	SetA8RegW( 0x6510, 0x0000 );		
#if 0	
	SetA8RegW( 0x6210, A8_JPEG_ENCODE_ROW_BUF_ADDR & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_ENCODE_ROW_BUF_ADDR >> 16 );
	SetA8RegW( 0x6214, A8_JPEG_ENCODE_COMP_BUF_START_ADDR & 0xFFFF );
	SetA8RegW( 0x6216, A8_JPEG_ENCODE_COMP_BUF_START_ADDR >> 16 );
	SetA8RegW( 0x6218, A8_JPEG_ENCODE_COMP_BUF_END_ADDR & 0xFFFF );
	SetA8RegW( 0x621A, A8_JPEG_ENCODE_COMP_BUF_END_ADDR >> 16 );
#else
	SetA8RegW( 0x6210, 0xF000 & 0xFFFF ); //0xEE00 ?
	SetA8RegW( 0x6212, 0xF000 >> 16 );
	SetA8RegW( 0x6214, 0x0500 & 0xFFFF ); //0xA000 ?
	SetA8RegW( 0x6216, 0x0500 >> 16 );
//SetA8RegW( 0x6218, 0xEFFF & 0xFFFF );
//SetA8RegW( 0x621A, 0xEFFF >> 16 );
	SetA8RegW( 0x6218, 0xEFF0 & 0xFFFF ); //0422 for TechnoB ImageEdit //0xEDF0
	SetA8RegW( 0x621A, 0xEFF0 >> 16 );
#endif
	SetA8RegW( 0x6222, 0x0080 );
	// 0428 SetA8RegB( 0x6206, 0x0018 );        /*add by harvey*/
	SetA8RegB( 0x6206, 0x0010 );   // 0428 for test ImageEdit quality issue
	
	SetA8RegB( 0x6204, 0x0028 );		/* Jpeg encode enable */
	#if 0
	SetA8RegW( 0x6B50, 0x9000 );
	SetA8RegW( 0x6B52, 0x0000 );
        #else
//	SetA8RegW( 0x6B50, 0xA980 ); //0x1A980 (1600x2) for 704
//	SetA8RegW( 0x6B52, 0x0001 );
 	SetA8RegW( 0x6B50, 0x0000 ); //0422 (640x2), for TechnoB ImageEdit
	SetA8RegW( 0x6B52, 0x0000 );

        #endif
	
	if ( A8S_Cameraparam.Mode == 1 )					/* Source image format RGB565 */
		SetA8RegW( 0x6B0E, 0x0102 );		/* Graphic FIFO path enable */
	else if (A8S_Cameraparam.Mode == 2 )				/* Source image format YUV422 */
		SetA8RegW( 0x6B0E, 0x0103 );		/* Graphic FIFO path enable */
		
	SetA8RegW( 0x6B06, A8S_Cameraparam.SrcWidth);
	SetA8RegW( 0x6B0C, A8S_Cameraparam.SrcHeight);
	SetA8RegW( 0x6B24, 0x0004 );

	ImgPixPtr = RgbPtr;
	JpegPictLength = 0;
	
	if ( A8S_Cameraparam.SrcHeight >= 8 )
		SrcRowChunk = A8S_Cameraparam.SrcWidth << 3;
	else
		SrcRowChunk = A8S_Cameraparam.SrcWidth*A8S_Cameraparam.SrcHeight;
		
	SrcLength = A8S_Cameraparam.SrcWidth*A8S_Cameraparam.SrcHeight;
	RowCount = A8S_Cameraparam.SrcHeight;
	while ( RowCount > 0 )
	{
		PixCount = SrcRowChunk;
		do
		{
			GphFIFOCount = GetA8RegB( 0x6B26 ) & 0x01F;
			if ( GphFIFOCount > 0 )
			{
				if ( PixCount > GphFIFOCount )
				{
					A8IndCmdP = HII_SET_ADDR_L;
					A8IndDataPW(0) = 0x6B28;
					A8IndCmdP = HII_RW_REG_W;
					for ( i=0; i<GphFIFOCount; i++ )
						A8IndDataPW(i) = *ImgPixPtr++;
					PixCount -= GphFIFOCount;
				}
				else
				{
					A8IndCmdP = HII_SET_ADDR_L;
					A8IndDataPW(0) = 0x6B28;
					A8IndCmdP = HII_RW_REG_W;
					for ( i=0; i<PixCount; i++ )
						A8IndDataPW(i) = *ImgPixPtr++;
					PixCount = 0;
				}
			}
		}
		while ( PixCount > 0 );

		RowCount = RowCount - 8;
		if ( RowCount < 8 )
			SrcRowChunk = A8S_Cameraparam.SrcWidth * RowCount;
		
		JpegStatus = GetA8RegW( 0x6222 );
		JpegFIFOCount = JpegStatus & 0x01F;
		while ( JpegFIFOCount > 0 )
		{
			if (GetA8RegB(0x6242) & 0x02) 
			{
				AIT_Message_P0("1JPEG Encode FIFO OVERFLOW .... \n");
                		return A8_NO_ERROR; //0425
			}
			A8IndCmdP = HII_SET_ADDR_L;
			A8IndDataPW(0) = 0x6220;
			A8IndCmdP = HII_RW_REG_W;
			for ( i=0; i<JpegFIFOCount; i++ )
			{
				*(JpegPtr++) = A8IndDataPW( i );
			}
			JpegPictLength += JpegFIFOCount;
			JpegStatus = GetA8RegW( 0x6222 );
			JpegFIFOCount = JpegStatus & 0x01F;
		}
	}
		
	do
		{
		JpegStatus = GetA8RegW( 0x6222 );
		JpegFIFOCount = JpegStatus & 0x1F;		
		while ( JpegFIFOCount > 0 )
			{
			if (GetA8RegB(0x6242) & 0x02) 
			{
				AIT_Message_P0("2JPEG Encode FIFO OVERFLOW .... \n");
                		return A8_NO_ERROR; //0425
			}
			if ( JpegPtr < (u_short *) (JpegBufferAddr + MAX_JPEG_SIZE) )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<JpegFIFOCount; i++ )
					{
					*(JpegPtr++) = A8IndDataPW( i );
					}
				}
			else
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<JpegFIFOCount; i++ )
					{
					*(JpegPtr+1) = A8IndDataPW( i );
					}
				}
			JpegPictLength += JpegFIFOCount;
			JpegStatus = GetA8RegW( 0x6222 );
			JpegFIFOCount = JpegStatus & 0x1F;		
			} 
		}
	while ( (JpegStatus & 0x0100) != 0x0100 );
	SetA8RegW( 0x6222, 0x0000 );
	SetA8RegW( 0x6510, 0x0000 );
	SetA8RegW( 0x6B0E, 0x0000 );
	
#ifdef EXIFSUPPORT 
	return (JpegPictLength << 1) + 0x312;
#else	
	return JpegPictLength << 1;
#endif
	//return A8_NO_ERROR;
}


s_short A8L_DecodeJpegBaseHWScale( u_short *JpegBufferPtr, u_short *pRgbBuf, s_int Length, s_int DestAddr ,u_char mode, u_short SrcX, u_short SrcY, u_short SrcWidth, u_short SrcHeight, u_short DstWidth, u_short DstHeight, u_char MyFave_mode)
{
	u_short Status, Temp;
	s_int ByteCount;
	s_int i, LengthForSend, timeout;
	s_short Width, Height, Format, LineOffset;
	s_short AdjustedWidth, AdjustedHeight, AdjustedSrcWidth, AdjustedSrcHeight;
	s_short frontGrab_X0, frontGrab_X1, frontGrab_Y0, frontGrab_Y1;
	s_short Ratio;
	u_int BufAddr;
	A8S_CameraParam A8S_Cameraparam;
	s_short HuffBufferIndex = 0;
	u_short *JpegDHTEvenAddr;
	u_int AddrTemp;
	u_short ScaleMBase, TempScaleX,TempScaleY;//, ScaleRatioN, ScaleRatioM, TempSrcX, TempSrcY;
	u_short x0, y0, x1, y1;

	A8S_Cameraparam.Mode = mode;

	SetA8RegB(0x6902, GetA8RegB(0x6902)|0x40); //enable fast jpeg decode clock

	//A8L_GetJpegInfo( JpegBufferPtr, Length, &Width, &Height, &Format, &Quality );
	A8L_ScanJpegMarker( JpegBufferPtr, Length, &Width, &Height, &Format );

	//Check parameters
	if((SrcX+SrcWidth)>Width || (SrcY+SrcHeight)>Height )
		return A8_OUT_OF_RANGE_ERROR;  //  1;  20080624 update !!
	
	if ( Format != A8_JPEG_FORMAT_YUV411 && Format != A8_JPEG_FORMAT_YUV422 && Format != A8_JPEG_FORMAT_YUV420 && Format != A8_JPEG_FORMAT_YUV444 )
		return A8_UNSUPPORT_ERROR;
	
	AdjustedWidth = Width;
	AdjustedHeight = Height;
	AdjustedSrcWidth = SrcWidth;
	AdjustedSrcHeight = SrcHeight;
	
	switch (Format) 
	{
		case	A8_JPEG_FORMAT_YUV411:
			if (Width & 0x1F)
			{
				AdjustedWidth = ((Width >> 5) + 1) << 5;
			}
			if (Height & 0x07)
			{
				AdjustedHeight = ((Height >> 3) + 1) << 3;
			}

			if (SrcWidth & 0x1F)
			{
				AdjustedSrcWidth = ((SrcWidth >> 5) + 1) << 5;
			}
			if (SrcHeight & 0x07)
			{
				AdjustedSrcHeight = ((SrcHeight >> 3) + 1) << 3;
			}
			break;
		case	A8_JPEG_FORMAT_YUV420:
			if (Width & 0x0F) 
			{
				AdjustedWidth = ((Width >> 4) + 1) << 4;
			}
			if (Height & 0x0F)
			{
				AdjustedHeight = ((Height >> 4) + 1) << 4;
			}

			if (SrcWidth & 0x0F) 
			{
				AdjustedSrcWidth = ((SrcWidth >> 4) + 1) << 4;
			}
			if (SrcHeight & 0x0F)
			{
				AdjustedSrcHeight = ((SrcHeight >> 4) + 1) << 4;
			}
			break;
		case	A8_JPEG_FORMAT_YUV422:
			if (Width & 0x0F) 
			{
				AdjustedWidth = ((Width >> 4) + 1) << 4;
			}
			if (Height & 0x07)
			{
				AdjustedHeight = ((Height >> 3) + 1) << 3;
			}

			if (SrcWidth & 0x0F) 
			{
				AdjustedSrcWidth = ((SrcWidth >> 4) + 1) << 4;
			}
			if (SrcHeight & 0x07)
			{
				AdjustedSrcHeight = ((SrcHeight >> 3) + 1) << 3;
			}
			break;
		case	A8_JPEG_FORMAT_YUV444:	
			if (Width & 0x07) 
			{
				AdjustedWidth = ((Width >> 3) + 1) << 3;
			}
			if (Height & 0x07)
			{
				AdjustedHeight = ((Height >> 3) + 1) << 3;
			}

			if (SrcWidth & 0x07) 
			{
				AdjustedSrcWidth = ((SrcWidth >> 3) + 1) << 3;
			}
			if (SrcHeight & 0x07)
			{
				AdjustedSrcHeight = ((SrcHeight >> 3) + 1) << 3;
			}
			break;
	}
		
	SetA8RegB( 0x690A, 0x1E );	
	SetA8RegB( 0x690A, 0x00 );	
	SetA8RegW( 0x6222, 0x0000 );
	sys_IF_ait_delay1us(300);
	if ( Width > A8_JPEG_DECODE_MAX_WIDTH )
	{
		AIT_Message_P0("A8_JPEG_DECODE_ERR_OVER_WIDTH\n");
		return A8_JPEG_DECODE_ERR_OVER_WIDTH;
	}

	if ( A8S_Cameraparam.Mode > 3 || A8S_Cameraparam.Mode < 1 )
	{
		AIT_Message_P0("A8_JPEG_DECODE_ERR_UNKNOWN_FORMAT\n");
		return A8_JPEG_DECODE_ERR_UNKNOWN_FORMAT;
	}
	
	SetA8RegW(0x6020, 0x0000);		/* Disable TG viewfinder mode and viewfinder data output */

	/* Reset PIP and jpeg line buffer */
	Temp = GetA8RegW( 0x6510 );
	SetA8RegW( 0x6510, Temp | 0x0400 );	/* Reset PIP and jpeg line buffer */
	SetA8RegW( 0x6510, Temp );
	
	/* Set Scaler Output Address(PIP) */
	SetA8RegW(0x6508, (u_short)(DestAddr & 0xFFFF));	// Image Bank 0 start address. Dest buffer address
	SetA8RegW(0x650A, (u_short)(DestAddr >> 16));	// Image Bank 0 start address. Dest buffer address
	SetA8RegW(0x650C, (u_short)(DestAddr & 0xFFFF));	// Image Bank 1 start address. Dest buffer address
	SetA8RegW(0x650E, (u_short)(DestAddr >> 16));	// Image Bank 1 start address. Dest buffer address
	
	//---Count  parameters here---
	if ( Width > JpegDecodeWidthToDownSample )
	{
		//parameters for down sample
		SrcX = SrcX>>1;
		SrcY = SrcY>>1;
		SrcWidth = SrcWidth>>1;
		SrcHeight = SrcHeight>>1;

		AdjustedSrcWidth = AdjustedSrcWidth>>1;
		AdjustedSrcHeight = AdjustedSrcHeight>>1;
	}

		if ((DstWidth <= SrcWidth) && (DstHeight <= SrcHeight)) // Scale down
		{
			ScaleMBase = 60;	// this value need to sync with Preview.
			TempScaleX = DstWidth * ScaleMBase / SrcWidth;			
			TempScaleY = DstHeight * ScaleMBase / SrcHeight;

			if( (SrcWidth * TempScaleX / ScaleMBase - DstWidth ) < 0 )
				TempScaleX+=1;
			if( (SrcHeight* TempScaleY / ScaleMBase - DstHeight) < 0 )
				TempScaleY+=1;
			if (TempScaleX < TempScaleY)
				TempScaleX = TempScaleY;
	
			x0 = (SrcWidth * TempScaleX / ScaleMBase - DstWidth)/2;
           if (x0== 1) // 0529 sticker shot right side cut issue
           	x0+=1;
           else
           {
				if ((x0 %2) ==0)
				  x0+=1;
           }
			x1 = x0 + DstWidth - 1;
			y0 = (SrcHeight * TempScaleX / ScaleMBase - DstHeight)/2 + 1;
			y1 = y0 + DstHeight - 1;
		}
		else		// Scale up
		{
			ScaleMBase = 20;	
			TempScaleX = DstWidth * ScaleMBase / (SrcWidth-1);
			TempScaleY = DstHeight * ScaleMBase / (SrcHeight-1);
		
			if( ((SrcWidth-1) * TempScaleX / ScaleMBase - DstWidth) < 0 )
				TempScaleX+=1;
			if( ((SrcHeight-1) * TempScaleY / ScaleMBase - DstHeight) < 0 )
				TempScaleY+=1;
			if (TempScaleX < TempScaleY)
				TempScaleX = TempScaleY;
		
			if (TempScaleX > 100)
			{
				TempScaleX = 100 ;
				DstWidth = (SrcWidth-1) * TempScaleX / ScaleMBase;
				DstHeight = (SrcHeight-1) * TempScaleY / ScaleMBase ;
			}
	
			x0 = ((SrcWidth-1) * TempScaleX / ScaleMBase - DstWidth)/2;
			if ((x0 %2) ==0)
			  x0+=1;
			x1 = x0 + DstWidth - 1;
			y0 = ((SrcHeight-1) * TempScaleX / ScaleMBase - DstHeight)/2 + 1;
			y1 = y0 + DstHeight - 1;
		}
	
		A8S_Cameraparam.ScaleN=TempScaleX;
		A8S_Cameraparam.ScaleM=ScaleMBase;
		A8S_Cameraparam.startx=x0;
		A8S_Cameraparam.endx=x1;
		A8S_Cameraparam.starty=y0;
		A8S_Cameraparam.endy=y1;
	
	//-----------------------------
	
	if ( Width > JpegDecodeWidthToDownSample )
	{
		SetA8RegB(0x620A, 0x41);		/* JPEG 1/2 down sample with repeat ratio */
		SetA8RegB(0x4F1D, 0x01);		/* Scaler V 1/2 downsample enable */
		
    	AdjustedWidth = AdjustedWidth>>1; //080612
    	AdjustedHeight = AdjustedHeight>>1; //080612
	}
	else
	{
		SetA8RegB(0x620A, 0x01);		/* JPEG repeat ratio */
		SetA8RegB(0x4F1D, 0x00);
	}

//0612        if(WidthToDoubleBuffer>=800)
//            SetA8RegB(0x620A, GetA8RegB(0x620A)|0x80); //decode double buffer

	frontGrab_X0 = SrcX + 1;
	frontGrab_X1 = SrcX + AdjustedSrcWidth;
	if (frontGrab_X1 > AdjustedWidth)
	{
		frontGrab_X0 = AdjustedWidth-AdjustedSrcWidth + 1;
		frontGrab_X1 = AdjustedWidth;
	}
	
	frontGrab_Y0 = SrcY + 1;
	frontGrab_Y1 = SrcY + AdjustedSrcHeight;
	if (frontGrab_Y1 > AdjustedHeight)
	{
		frontGrab_Y0 = AdjustedHeight-AdjustedSrcHeight + 1;
		frontGrab_Y1 = AdjustedHeight;
	}

	SetA8RegW( 0x4F00, frontGrab_X0);				/* H start,Scaler input x0 */ 
	SetA8RegW( 0x4F02, frontGrab_X1); 		/* H End, Scaler input x1 */
	SetA8RegW( 0x4F04, frontGrab_Y0);				/* V start,Scaler input y0 */
	SetA8RegW( 0x4F06, frontGrab_Y1);		/* V end, Scaler input y1 */

	SetA8RegW( 0x4F08, A8S_Cameraparam.startx);			/* Scaler output H grabe range */
	SetA8RegW( 0x4F0A, A8S_Cameraparam.endx);
	SetA8RegW( 0x4F0C, A8S_Cameraparam.starty);			/* Scaler output V grabe range */
	SetA8RegW( 0x4F0E, A8S_Cameraparam.endy);
	
	Ratio = A8S_Cameraparam.ScaleM/A8S_Cameraparam.ScaleN;
	if ( Ratio >= 2 )
	{
		SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
		SetA8RegB( 0x4F19, 0x06 );		// edge gain
		SetA8RegB( 0x4F1A, 0x03 );		// edge core
		SetA8RegB( 0x4F18, 0x00 ); 	// enable edge enhancement
	}
	else
	{
		SetA8RegB( 0x4F12, 0x01 );			// Auto LPF
		SetA8RegB( 0x4F18, 0x01 ); 	// bypass edge enhancement
	}
	
	if( A8S_Cameraparam.ScaleN == A8S_Cameraparam.ScaleM)
		SetA8RegB( 0x4F14, 0x01 );
	else
		SetA8RegB( 0x4F14, 0x00 );

	SetA8RegB( 0x4F15, A8S_Cameraparam.ScaleN );
	SetA8RegB( 0x4F16, A8S_Cameraparam.ScaleM);
	//SetA8RegB( 0x4F18, 0x03 );
	//SetA8RegB( 0x4F18, 0x02 ); //Dali-CR-151
	//SetA8RegB( 0x4F18, 0x01 ); 
	//SetA8RegB( 0x4F18, 0x00 ); 	// enable edge enhancement
	SetA8RegB( 0x4F1C, GetA8RegB(0x4F1C) | 0x40 );

	SetA8RegW( 0x6210, A8_JPEG_DECODE_ROW_BUF_ADDR_1 & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_DECODE_ROW_BUF_ADDR_1 >> 16 );
    if (MyFave_mode)
    {
     	SetA8RegW( 0x6214, (160*160*2) & 0xFFFF );
     	SetA8RegW( 0x6216, (160*160*2) >> 16 );
    }
    else
    {
		SetA8RegW( 0x6214, A8_JPEG_DECODE_COMP_BUF_START_ADDR_1 & 0xFFFF );
		SetA8RegW( 0x6216, A8_JPEG_DECODE_COMP_BUF_START_ADDR_1 >> 16 );
     }
	SetA8RegW( 0x6218, A8_JPEG_DECODE_COMP_BUF_END_ADDR_1 & 0xFFFF );
	SetA8RegW( 0x621A, A8_JPEG_DECODE_COMP_BUF_END_ADDR_1 >> 16 );

	SetA8RegW( 0x6224, A8_JPEG_DECODE_FIFO_BUF_SIZE_1 & 0xFFFF );
	SetA8RegB( 0x6226, A8_JPEG_DECODE_FIFO_BUF_SIZE_1 >> 16 );
	
	SetA8RegW( 0x6222, 0x0000);

	Temp = GetA8RegB( 0x5040 );
	SetA8RegB( 0x5040, Temp & 0xFB);	// Disable ping-pong buffer
	if (A8S_Cameraparam.ScaleN > A8S_Cameraparam.ScaleM)
	{
		SetA8RegW(0x6B2E, 0x278);/*070226 */
	}
	else
	{
		SetA8RegW(0x6B2E, 0x80);		// Default value is 0x80.
	}

	SetA8RegW(0x6510, 0x0004 + A8S_Cameraparam.Mode - 1 );

	ByteCount = 0;
	
	SetA8RegW(0x6242, 0x0F1F);
	SetA8RegW(0x6222, 0x0080);		// Enable JPEG FIFO Path
	SetA8RegB(0x6204, 0x0006);		// Enable JPEG Decoder

	timeout = 0;

	sys_IF_ait_delay1ms(8); // eric 20081215 need check the time. 65535/1800 = 36  36*2 = 72  __> 80


	AddrTemp = (u_int ) gJpegLastDHTEndAddr;
	AddrTemp = (AddrTemp & 0xFFFFFFFE);
	JpegDHTEvenAddr = (u_short *)AddrTemp;

	if ( gJpegHuffTabCount < 3 || gJpegHuffBackupEnable )
	{
		if(gbAddHeaderStuff)
		{
			SetA8RegW(0x6220, 0xD8FF);
		}
		
		do
		{
			LengthForSend = Length + gJpegHuffBackupTail - ByteCount;
			if ( LengthForSend >32 )
				LengthForSend = 16;
			else
				LengthForSend = (LengthForSend+1) >> 1;
				
			Status = GetA8RegB(0x6222);
			if ( Status == 0xA0 )
			{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for (i=0; i<LengthForSend; i++ )
				{
					if ( JpegBufferPtr != JpegDHTEvenAddr )
						A8IndDataPW(0) =  *(JpegBufferPtr++);
					else
					{
						if ( HuffBufferIndex < (gJpegHuffBackupTail>>1) )
						{
							A8IndDataPW(0) =  gJpegHuffBackup[HuffBufferIndex];
							HuffBufferIndex++;
						}
						else
						{
							if ( gJpegHuffBackupTail != gJpegHuffBackupLength )
								JpegBufferPtr++;	// Skipping 1 word
								
							A8IndDataPW(0) = *(JpegBufferPtr++);
						}
					}
				}
				ByteCount += (LengthForSend<<1);
                              //AIT0801
				timeout = 0;
			}
			else
			{
				timeout++;
				if (timeout >= (A8_DECODE_JPEG_TIMEOUT>>2) )
				{
					SetA8RegB(0x6223, 0x02);
					SetA8RegW(0x6222, 0x00);
					return A8_JPEG_DECODE_ERR_TIMEOUT;
				}
			}
			Status = GetA8RegW(0x6204);
			sys_IF_ait_delay1us(2);
		}
		while ( (ByteCount < (Length+gJpegHuffBackupTail)) && (( Status & 0x02 ) != 0) );

	}
	else
	{
		do
		{
			LengthForSend = Length - ByteCount;
			if ( LengthForSend >32 )
				LengthForSend = 16;
			else
				LengthForSend = (LengthForSend+1) >> 1;
				
			Status = GetA8RegB(0x6222);
			if ( Status == 0xA0 )
			{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for (i=0; i<LengthForSend; i++ )
				{
						A8IndDataPW(0) = *(JpegBufferPtr++);
				}
				ByteCount += (LengthForSend<<1);
			}
			else
			{
				timeout++;
				if (timeout >= (A8_DECODE_JPEG_TIMEOUT>>2) )
				{
					SetA8RegB(0x6223, 0x02);
					SetA8RegW(0x6222, 0x00);
					return A8_JPEG_DECODE_ERR_TIMEOUT;
				}
			}
			Status = GetA8RegW(0x6204);
		}
		while ( (ByteCount < Length) && (( Status & 0x02 ) != 0) );
	}
	
	SetA8RegB(0x6223, 0x02);
	
	timeout = 0;
	do
	{
		Status = GetA8RegB(0x6223);
		if ( (Status & 0x01) != 0 )
			SetA8RegW(0x6222, 0x0000);
		Status = GetA8RegW(0x6204);
		timeout++;
	}
	while ((Status & 0x02) == 0x02 && timeout < (A8_DECODE_JPEG_TIMEOUT>>2));	
	
	SetA8RegW(0x6222, 0x0000);
	SetA8RegB( 0x4F18, 0x01 );
	SetA8RegB( 0x4F19, 0x08 );
	SetA8RegB( 0x4F1A, 0x04 );

	if ( timeout >= (A8_DECODE_JPEG_TIMEOUT>>2) )
	{
		AIT_Message_P0("A802:Decode JPEG Timeout as checking status\n");
		SetA8RegB( 0x690A, 0x0C );	
		SetA8RegB( 0x690A, 0x00 );	
		SetA8RegB( 0x4F14, 0x00 );    //+Bing add for M300 scaling issue 070620 
		return A8_JPEG_DECODE_ERR_TIMEOUT;
	}
	else
	{
		/*AIT_Message_P0("A802: Decode Jpeg Picture OK");*/
         SetA8RegB( 0x4F14, 0x00 );    //+Bing add for M300 scaling issue 070620 
		//return Format;
	}

	//copy data to host
	#if 1 //save data to Host
	BufAddr = 0L;
//	if(pRgbBuf != NULL)
//	{
			//CopyMemWordA8ToHost(rgbBuf, 0L, tempWidth*tempHeight*2);	
			if(DstWidth % 4)
				LineOffset = ((DstWidth + (4- DstWidth %4)) << 1);
			else
				LineOffset = (DstWidth<< 1);

			if ( (DstWidth% 4) != 1 ) 
			{
				if ((DstWidth% 4) == 0)
				{	// 20070621, update. Memory copy is more stable!!
					CopyMemWordA8ToHost(pRgbBuf, 0L, DstWidth*DstHeight*2);    
		}
				else
				{
					A8L_SetBufferAttribute1(6, DstWidth, DstHeight, LineOffset, A8_DEPTH_16B, 0L);
					A8L_CopyImageBuftoFIFO(pRgbBuf, DstWidth, DstHeight, 6, 0, 0);
				}
			}
			else
			{
				//BufAddr = DecodeDataBuf;
				for ( i=0; i<DstHeight; i++ ) 
				{
					CopyMemWordA8ToHost(pRgbBuf, BufAddr, DstWidth <<1);	/* length in byte*/
					pRgbBuf += DstWidth;
					BufAddr += (DstWidth << 1);
					if (i & 0x01)
						BufAddr += 4;
				}

			}
//	}

	#endif
	
	return A8_NO_ERROR;
}


/**
 @fn		u_int A8L_TranslateRGBToJpegPicture( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
 @brief	Encode a RGB565 image to a jpeg picture with size scaling. Notice that the RGB565 (source)
 @n		image is in a host side buffer pointed by RgbPtr. The encoded jpeg picture will be 
 @n		stored in a host side buffer pointed by JpegBufferPtr after the function is done.
 @n		The RGB565 image dimension is specified by SrcWidth and SrcHeight. The max of SrcWidth is
 @n		640.The jpeg image size is defined by JpegWidth and JpegHeight. The RGB565 image width must
 @n		be a multiply of 4.	The jpeg width must be a multiply of 16. The size scaling ratio is 
 @n		controlled by UpScale and DownScale. Their range is from 1 to 127. The scale ratio is equal
 @n		to UpScale/DownScale. The RGB565 is 16 bits per pixel. R sits at the highest 5 bits, and G 
 @n		sits at the middle 6 bits then B sits at the lowest 5 bits.
 @param	RgbPtr -the buffer pointer in host side for RGB565 image.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @n		SrcWidth : RGB565 image width. the width max is 640.
 @n		SrcHeight : RGB565 image height.
 @n		JpegWidth : the jpeg image width, the source image width must be the same and 1 multiply of 16
 @n		JpegHeight : the jpeg image height, the source image height must be the same
 @n		UpScale : the factor for scaling up.
 @n		DownScale : the factor for scaling down. The scale ratio is UpScale/DownScale.
 @param	JpegBufferAddr -the buffer pointer in host side for storing a encoded jpeg picture.
 @return	u_int - file size.
 @note	Jpeg picture dimension limitation
 @n		JpegWidth <= SrcWidth * UpScale/DownScale
 @n		JpegHeight <= SrcHeight * UpScale/DownScale.
 @bug	N/A.
*/
u_int A8L_TranslateRGBToJpegPicture( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
{
	A8S_Cameraparam.Mode = 1;
	return A8L_TranslateImageToJpegBase( RgbPtr, A8S_Cameraparam ,JpegBufferAddr);
}


/**
 @fn		u_int A8L_TranslateYuvToJpegPicture( u_short *YuvPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
 @brief	Encode a Yuv422 image to a jpeg picture with size scaling.  
 @n		The Yuv422 image is 16 bits per pixel. The even pixel of the image 
 @n		contains component Cb (low byte) and Y (high byte). The odd pixel contains Cr (low byte) 
 @n		and Y (high byte).
 @param	YuvPtr -the buffer pointer in host side for Yuv422 image.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @n		SrcWidth : RAW image width. the width max is 640.
 @n		SrcHeight : RAW image height.
 @n		JpegWidth : the jpeg image width, the source image width must be the same and 1 multiply of 16
 @n		JpegHeight : the jpeg image height, the source image height must be the same
 @n		UpScale : the factor for scaling up.
 @n		DownScale : the factor for scaling down. The scale ratio is UpScale/DownScale.
 @param	JpegBufferAddr -the buffer pointer in host side for storing a encoded jpeg picture.
 @return	u_int - file size.
 @note	Jpeg picture dimension limitation
 @n		JpegWidth <= SrcWidth * UpScale/DownScale
 @n		JpegHeight <= SrcHeight * UpScale/DownScale.
 @bug	N/A.
*/
u_int A8L_TranslateYuvToJpegPicture( u_short *YuvPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
{
	A8S_Cameraparam.Mode = 2;
	return A8L_TranslateImageToJpegBase( YuvPtr, A8S_Cameraparam, JpegBufferAddr );
}


/**
 @fn		u_int A8L_TranslateRGBToJpegPicture( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
 @brief	Encode a RGB565 image to a jpeg picture with size scaling.
 @param	RgbPtr -the buffer pointer in host side for RGB565 image.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @param	JpegBufferAddr -the buffer pointer in host side for storing a encoded jpeg picture.
 @return	u_int - file size.
 @note	.
 @bug	N/A.
*/
u_int A8L_EncodeRGBToJpegPicture( u_short *RgbPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr )
{
	A8S_Cameraparam.Mode = 1;
        return A8_NO_ERROR;
//	return A8L_TranslateImageToJpegBase( RgbPtr, A8S_Cameraparam, JpegBufferPtr );
}


/**
 @fn		u_int A8L_TranslateYuvToJpegPicture( u_short *YuvPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
 @brief	Encode a Yuv422 image to a jpeg picture with size scaling. 
 @param	YuvPtr -the buffer pointer in host side for Yuv422 image.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @param	JpegBufferAddr -the buffer pointer in host side for storing a encoded jpeg picture.
 @return	u_int - file size.
 @note	.
 @bug	N/A.
*/
u_int A8L_EncodeYuvToJpegPicture( u_short *YuvPtr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr )
{
	A8S_Cameraparam.Mode = 2;
        return A8_NO_ERROR;
//	return A8L_TranslateImageToJpegBase( YuvPtr, A8S_Cameraparam, JpegBufferPtr );
}


/**
 @fn		u_int A8L_TranslateImageBufToJpegBase( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo )
 @brief	Encode a RGB565/YUV422 image to a jpeg picture with size scaling. Notice that the RAW (source)
 @n		image is in AIT memory buffer pointed by RgbAddr. The encoded jpeg picture will be 
 @n		stored in a host side buffer pointed by JpegBufferPtr after the function is done.
 @n		The RAW image dimension is specified by SrcWidth and SrcHeight. The max of SrcWidth is
 @n		640.The jpeg image size is defined by JpegWidth and JpegHeight. The RAW image width must
 @n		be a multiply of 4.	The jpeg width must be a multiply of 16. The size scaling ratio is 
 @n		controlled by UpScale and DownScale. Their range is from 1 to 127. The scale ratio is equal
 @n		to UpScale/DownScale. The RGB565/YUV422 is 16 bits per pixel. 
 @param	RgbAddr -RGB565/YUV422 data buffer pointer on AIT memory buffer.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @n		SrcWidth : RGB565 image width. the width max is 640.
 @n		SrcHeight : RGB565 image height.
 @n		JpegWidth : the jpeg image width, the source image width must be the same and 1 multiply of 16
 @n		JpegHeight : the jpeg image height, the source image height must be the same
 @n		UpScale : the factor for scaling up.
 @n		DownScale : the factor for scaling down. The scale ratio is UpScale/DownScale.
 @param	JpegBufferAddr -JPEG data buffer pointer on host.
 @return	u_int - file size.
 @note	Jpeg picture dimension limitation
 @n		JpegWidth <= SrcWidth * UpScale/DownScale
 @n		JpegHeight <= SrcHeight * UpScale/DownScale.
 @bug	N/A.
*/

// @param	makerName - the array of maker name (this is for EXIF).
// @param	modelName - the array of model name (this is for EXIF).
// @param	DateTimeInfo - the array of Data Time Info (this is for EXIF).
// u_int A8L_TranslateImageBufToJpegBase( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo )

u_int A8L_TranslateImageBufToJpegBase( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr)
{
	s_int i, JpegPictLength;
	u_short  *JpegPtr;
	u_short JpegStatus, JpegFIFOCount;
#if 1
#ifdef EXIFSUPPORT 
	u_char *ExifPtr; //exif
	ExifPtr = (u_char *)JpegBufferAddr;
#ifndef EXIFSUPPORT 
	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();
#endif
	for(i=0;i<0x314;i++)  
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegPtr = JpegBufferAddr + 0x189; 
#else	
	JpegPtr = JpegBufferAddr; 
#endif	
#else
#ifdef EXIFSUPPORT
	u_char *ExifPtr; //exif

	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();

	for(i=0;i<0x314;i++)
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegPtr = JpegBufferAddr + 0x189;	
#else
	JpegPtr = JpegBufferAddr;
#endif
#endif	
	SetA8RegB( 0x690A, 0x1E );
	SetA8RegB( 0x690A, 0x00 );	
	SetA8RegW( 0x6B2E, 0x0140 ); 
	SetA8RegB( 0x6242, 0x03 );
	SetA8RegW( 0x6222, 0x0000 );		// Clear JPEG FIFO  
	
	// Set Jpeg Encode
	SetA8RegW( 0x6200, A8S_Cameraparam.JpgWidth );
	SetA8RegW( 0x6202, A8S_Cameraparam.JpgHeight); 
	
	SetA8RegW( 0x4F00, 0x0001 );
	SetA8RegW( 0x4F02, A8S_Cameraparam.SrcWidth);
	SetA8RegW( 0x4F04, 0x0001 );
	SetA8RegW( 0x4F06, A8S_Cameraparam.SrcHeight);
	SetA8RegW( 0x4F08, 0x0001 );
	SetA8RegW( 0x4F0A, A8S_Cameraparam.JpgWidth);
	SetA8RegW( 0x4F0C, 0x0001 );
	SetA8RegW( 0x4F0E, A8S_Cameraparam.JpgHeight);
	
	SetA8RegB( 0x4F14, 0x00 );
	SetA8RegB( 0x4F15, A8S_Cameraparam.ScaleN );
	SetA8RegB( 0x4F16, A8S_Cameraparam.ScaleM);
	SetA8RegB( 0x4F12, 0x01 );
	SetA8RegB( 0x4F1D, 0x00 );
	
	SetA8RegW( 0x6510, 0x0400 );		
	SetA8RegW( 0x6510, 0x0000 );		
	
	SetA8RegW( 0x6210, A8_JPEG_ENCODE_ROW_BUF_ADDR & 0xFFFF );
	SetA8RegW( 0x6212, A8_JPEG_ENCODE_ROW_BUF_ADDR >> 16 );
	SetA8RegW( 0x6214, A8_JPEG_ENCODE_COMP_BUF_START_ADDR & 0xFFFF );
	SetA8RegW( 0x6216, A8_JPEG_ENCODE_COMP_BUF_START_ADDR >> 16 );
	SetA8RegW( 0x6218, A8_JPEG_ENCODE_COMP_BUF_END_ADDR & 0xFFFF );
	SetA8RegW( 0x621A, A8_JPEG_ENCODE_COMP_BUF_END_ADDR >> 16 );
	
	SetA8RegW( 0x6222, 0x0080 );
	SetA8RegB( 0x6206, 0x0018 );
	SetA8RegB( 0x6204, 0x0028 );		// Jpeg encode enable
	
	
	SetA8RegW( 0x6B50, (u_short)(RgbAddr & 0xFFFF) );
	SetA8RegW( 0x6B52, (u_short)(RgbAddr >> 16) );
	SetA8RegW( 0x6B54, (A8S_Cameraparam.SrcWidth<<1) );
	SetA8RegW( 0x6B56, 0x00 );
	
	if ( A8S_Cameraparam.Mode == 1 )					/* Source image format RGB565 */
	{
	    SetA8RegB( 0x6B3C, 0x00);
		SetA8RegW( 0x6B0E, 0x0100 );		// 
    }		
	else if ( A8S_Cameraparam.Mode == 2 )				/* Source image format YUV422 */
	{
	    SetA8RegB( 0x6B3C, 0x01);
		SetA8RegW( 0x6B0E, 0x0101 );		// 
	}	
	SetA8RegW( 0x6B06, A8S_Cameraparam.SrcWidth );
	SetA8RegW( 0x6B0C, A8S_Cameraparam.SrcHeight);
	SetA8RegW( 0x6B24, 0x0004 ); 

/*	JpegPtr = JpegBufferAddr; */
	JpegPictLength = 0;
	
	do
	{
		JpegStatus = GetA8RegW( 0x6222 );
		JpegFIFOCount = JpegStatus & 0x1F;		
		while ( JpegFIFOCount > 0 )
		{
			if (GetA8RegB(0x6242) & 0x02) 
			{
				AIT_Message_P0("JPEG Encode FIFO OVERFLOW .... \n");
			}
			if ( JpegPtr < (u_short *) (JpegBufferAddr + MAX_JPEG_SIZE) )
			{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<JpegFIFOCount; i++ )
				{
					*(JpegPtr++) = A8IndDataPW( i );
				}
			}
			else
			{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<JpegFIFOCount; i++ )
				{
					*(JpegPtr+1) = A8IndDataPW( i );
				}
			}
			JpegPictLength += JpegFIFOCount;
			JpegStatus = GetA8RegW( 0x6222 );
			JpegFIFOCount = JpegStatus & 0x1F;		
		} 
	}
	while ( (JpegStatus & 0x0100) != 0x0100 );
	
	JpegStatus = GetA8RegW( 0x6222 );
	JpegFIFOCount = JpegStatus & 0x1F;		
	
	while ( JpegFIFOCount > 0 )
	{
		*(JpegPtr++) = GetA8RegW( 0x6220 );
		JpegFIFOCount--;
	}
	
	SetA8RegW( 0x6222, 0x0000 );
	SetA8RegW( 0x6510, 0x0000 );
	SetA8RegW( 0x6B0E, 0x0000 );

#ifdef EXIFSUPPORT 
	return (JpegPictLength << 1) + 0x312;
#else	
	return JpegPictLength << 1;
#endif
	//return A8_NO_ERROR;
}


/**
 @fn		u_int A8L_TranslateRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam , u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo )
 @brief	Encode a RGB565image to a jpeg picture with size scaling. Notice that the RAW (source)
 @n		image is in AIT memory buffer pointed by RgbAddr. 
 @param	RgbAddr -RGB565 data buffer pointer on AIT memory buffer.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @param	JpegBufferAddr -JPEG data buffer pointer on host.
 @return	u_int - file size.
 @note	.
 @bug	N/A.
*/
u_int A8L_TranslateRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam , u_short *JpegBufferAddr )
{
	A8S_Cameraparam.Mode = 1;
	//return A8L_TranslateImageBufToJpegBase( RgbAddr, A8S_Cameraparam, JpegBufferAddr, makerName, modelName, DateTimeInfo );
	return A8L_TranslateImageBufToJpegBase( RgbAddr, A8S_Cameraparam, JpegBufferAddr);
}


/**
 @fn		u_int A8L_TranslateYuvBufToJpegPicture( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
 @brief	Encode a YUV422 image to a jpeg picture with size scaling. Notice that the RAW (source)
 @n		image is in AIT memory buffer pointed by RgbAddr. 
 @param	YuvAddr -YUV422 data buffer pointer on AIT memory buffer.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @param	JpegBufferAddr -JPEG data buffer pointer on host.
 @return	u_int - file size.
 @note	.
 @bug	N/A.
*/
u_int A8L_TranslateYuvBufToJpegPicture( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
{
	A8S_Cameraparam.Mode = 2;
       return A8L_TranslateImageBufToJpegBase( YuvAddr, A8S_Cameraparam, JpegBufferAddr);
}


/**
 @fn		u_int A8L_EncodeRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr )
 @brief	Encode a RGB565image to a jpeg picture with size scaling. Notice that the RAW (source)
 @n		image is in AIT memory buffer pointed by RgbAddr. 
 @param	RgbAddr -RGB565 data buffer pointer on AIT memory buffer.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @param	JpegBufferPtr -JPEG data buffer pointer on host.
 @return	u_int - file size.
 @note	.
 @bug	N/A.
*/
u_int A8L_EncodeRGBBufToJpegPicture( u_int RgbAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr )
{
	A8S_Cameraparam.Mode = 1;
	return A8_NO_ERROR;
//	return A8L_TranslateImageBufToJpegBase( RgbAddr, A8S_Cameraparam, JpegBufferPtr );
}


/**
 @fn		u_int A8L_TranslateYuvBufToJpegPicture( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferAddr )
 @brief	Encode a YUV422 image to a jpeg picture with size scaling. Notice that the RAW (source)
 @n		image is in AIT memory buffer pointed by RgbAddr. 
 @param	YuvAddr -YUV422 data buffer pointer on AIT memory buffer.
 @param	A8S_Cameraparam -the related width/height of RAW data and JPEG data; the related setting for Scaler engine.
 @param	JpegBufferPtr -JPEG data buffer pointer on host.
 @return	u_int - file size.
 @note	.
 @bug	N/A.
*/
u_int A8L_EncodeYuvBufToJpegPicture( u_int YuvAddr, A8S_CameraParam A8S_Cameraparam, u_short *JpegBufferPtr )
{
	A8S_Cameraparam.Mode = 2;
        return A8_NO_ERROR;
//	return A8L_TranslateImageBufToJpegBase( YuvAddr, A8S_Cameraparam, JpegBufferPtr );
}


/**
 @fn		s_short A8L_SetPllFreq( s_short X )
 @brief	The function needs to be modified to support the 6.5MHz, 13MHz or 26MHz of external clock.
 @param	X -Multiple of PLL.
 @n		0 ... Disable PLL for by pass mode.
 @n		1 ... PLL is ?X for 13/26MHz external clock.
 @n		2 ... PLL is ?X for 13/26MHz external clock.
 @n		3 ... PLL is ?X for 13/26MHz external clock.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_SetPllFreq( s_short X )
{
	u_short Val, Tmp, retVal = 0;
	volatile s_int timeout, Maxtimeout=1000;

#if defined(AIT701U)
	SetA8RegB(0x6910,0x00);	//PMCLK no divid
#endif

	switch(X)
	{
		case 0:	//Bypass PLL
			Val = GetA8RegB( 0x6902 );
			Val = Val & 0xFD;
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6902, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6902 );
				
				timeout++;
			}while ((Tmp & 0x02) != 0x00 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
				retVal |= 0x01;

			// Power Down PLL
			Val = GetA8RegB( 0x6911 );
			Val = Val & 0xFE;
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6911, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6911 );
				
				timeout++;
			}while ((Tmp & 0x01) != 0x00 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
				retVal |= 0x02;
			
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:	
			// Power On PLL
			Val = GetA8RegB( 0x6911 );
			
#if defined(AIT701U)
			Val = Val | 0x03;		//DPLL Reset & Power on
#else
			Val = Val | 0x01;
#endif

			timeout = 0;
			do
			{
				SetA8RegB( 0x6911, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6911 );
				
				timeout++;
			}while ( (Tmp & 0x01) != 0x01 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
			{
				retVal |= 0x01;

			}

			sys_IF_ait_delay1ms(2);// delay 1.4ms

#if defined(AIT701U)
			// Set Frequency Multiplier
			Val = 4*X-2;	//PLL input integer = 4*2-2 = 6

			timeout = 0;
			do
			{
				SetA8RegB( 0x6981, Val );					//M = 6	,   
				sys_IF_ait_delay1us(5);
				Tmp = GetA8RegB( 0x6981 );
				
				timeout++;
			}while ( Tmp != Val && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
			{

				retVal |= 0x02;
			}
			// Update DPLL M/N
			SetA8RegB(0x6911, GetA8RegB(0x6911) & (~0x04UL));		//Setting to PLL
			SetA8RegB(0x6911, GetA8RegB(0x6911) | 0x04);     			//Setting to PLL
			SetA8RegB(0x6911, GetA8RegB(0x6911) & (~0x04UL));		//Setting to PLL
			
			SetA8RegB(0x6988,0x00);
#else
			// Set Frequency Multiplier
			Val = GetA8RegB( 0x6910 );
			Val = (Val & 0xF8)+(X-1);
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6910, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6910 );
				
				timeout++;
			}while ( (Tmp & 0x07) != (X-1) && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
				retVal |= 0x04;
#endif

			// Select PLL path
			Val = GetA8RegB( 0x6902 );
			Val = Val | 0x02;			// PLL path:System clock is from Embedded PLL clock out
			
			timeout = 0;
			do
			{
				SetA8RegB( 0x6902, Val );
				sys_IF_ait_delay1us(1);
				Tmp = GetA8RegB( 0x6902 );
				
				timeout++;
			}while ( (Tmp & 0x02) != 0x2 && timeout < Maxtimeout);

			if (timeout >= Maxtimeout)
				retVal |= 0x08;
			
			break;
		default:
			break;
	}
#if 1	//added by ming @20090422
	sys_IF_ait_delay1ms(15);		// Test on MT6223D platform, we MUST use 3ms or more!!!
#else
	sys_IF_ait_delay1ms(1);
#endif
	
	if ( retVal )
	{
		AIT_Message_Error("A8L_SetPllFreq: DLL Config Error: %d",retVal);
		
		return A8_SYSTEM_ERROR;
	}
	else
		return A8_NO_ERROR;
}

/**
 @fn		s_short A8L_UpdateLCD()
 @brief	Update LCD once.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_CheckLcdBusy(void)
{
   u_int timeout;
   u_short Status;

      timeout = 0;
      do
      {
         Status = GetA8RegB( 0x500A );
         timeout++;
      }
      while ( (Status & 0x02) != 0 && timeout<0x1000000);

  return A8_NO_ERROR;
}
s_short A8L_UpdateLCD()
{
   u_int timeout;
   u_short Status;
//   u_short Reg, Val, i;

   Status = GetA8RegW( 0x5000 );
   if ( (Status & 0x04) != 0 )
      return 1;

   Status = GetA8RegW( 0x500A );

   if ( (Status & 0x01) == 0 )
   {
#if 1
      timeout = 0;
      do
      {
         Status = GetA8RegB( 0x500A );
         timeout++;
      }
#if 0	//Timeout too long.
      while ( (Status & 0x02) != 0 && timeout<0x1000000);
      if ( timeout >= 0x1000000 )
#else
      while ( (Status & 0x02) != 0 && timeout<0x1000);
      if ( timeout >= 0x1000 )
#endif
      {
		return A8_TIMEOUT_ERROR;
      }
	  
      Status = Status | 0x02;
      SetA8RegW( 0x500A, Status );


#else   
      Status = Status | 0x02;
      SetA8RegW( 0x500A, Status );

      timeout = 0;
      do
      {
         Status = GetA8RegB( 0x500A );
         timeout++;
      }
      while ( (Status & 0x02) != 0 && timeout<0x100000);
      if ( timeout >= 0x100000 )
      {
		return A8_TIMEOUT_ERROR;
      }
#endif	  
   }
   else
	return 1;

   return A8_NO_ERROR;
}


u_short gA8IsLCDRefreshPaused = 0;
/**
 @fn		s_short A8L_PauseLCDRefresh(void)
 @brief	Pause LCD refresh.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_PauseLCDRefresh(void)
{
	u_short Status;
	s_int timeout = 0;
	
	if ( gA8IsLCDRefreshPaused != 0 )
		return 1;
	Status = GetA8RegB( 0x500A );
	if ( (Status & 0x01) == 0 )
		return 1;
		
	while ( (GetA8RegB(0x5008) & 0x01) == 0x01 && timeout < 0x50000 )
		timeout++;
	
	SetA8RegB( 0x500A, GetA8RegB(0x500A) & 0xFE );
	gA8IsLCDRefreshPaused = 1;
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_ResumeLCDRefresh(void)
 @brief	Resume LCD refresh.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_ResumeLCDRefresh(void)
{
	u_short Status;
	s_int timeout = 0;
	
	if ( gA8IsLCDRefreshPaused != 1 )
		return 1;
	Status = GetA8RegB( 0x500A );
	if ( (Status & 0x01) == 0x01 )
		return 1;
		
	SetA8RegB( 0x6041, 0x01 );
	while ( (GetA8RegB(0x6041) & 0x01) == 0 && timeout<0x10000)
		timeout++;
		
	SetA8RegB( 0x500A, GetA8RegB(0x500A) | 0x01 );
	gA8IsLCDRefreshPaused = 0;
	return A8_NO_ERROR;
}


u_short gA8IsLCDPreviewPaused = 0;
/**
 @fn		s_short A8L_PauseLCDPreview(void)
 @brief	Pause LCD Preview.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_PauseLCDPreview(void)
{
	u_short Status;
	s_int timeout = 0;
	
	if ( gA8IsLCDPreviewPaused != 0 )
		return 1;
	Status = GetA8RegB( 0x6510 );
	if ( (Status & 0x04) == 0 )
		return 1;
		
	SetA8RegB(0x6041,GetA8RegB(0x6041)|0x01);
	SetA8RegB(0x6510, GetA8RegB(0x6510) & 0xFB);
	while ( (GetA8RegB(0x6041) & 0x01) == 0x00 && timeout < A8_SWITCH_PREVIEW_TIME_OUT )
	{
		sys_IF_ait_delay1us(100);
		timeout++;
	}
	
	gA8IsLCDPreviewPaused = 1;
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_ResumeLCDPreview(void)
 @brief	Resume LCD Preview.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_ResumeLCDPreview(void)
{
	u_short Status;
	
	if ( gA8IsLCDPreviewPaused != 1 )
		return 1;
	Status = GetA8RegB( 0x6510 );
	if ( (Status & 0x04) == 0x04 )
		return 1;
		
	SetA8RegB( 0x6510, GetA8RegB(0x6510) | 0x04 );
	gA8IsLCDPreviewPaused = 0;
	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_CheckLCDPreview(void)
 @brief	Check LCD refresh.
 @return	s_short .
 @note	.
 @bug	N/A.
*/
s_short A8L_CheckLCDPreview(void)
{
	if ( (GetA8RegB(0x500A) & 0x01) == 1 && (GetA8RegB(0x6510) & 0x04) == 4 )
		return 1;
	else
		return A8_NO_ERROR;
}


/**
 @fn		s_short SetLCDRefreshType( s_short Type )
 @brief	Set LCD refresh type.
 @param	Type : 0:by frame ( Preview ), 1:by HW Timer, 2:by SW command
 @return	s_short
 @note	.
 @bug	N/A.
*/
s_short SetLCDRefreshType( s_short Type )
{
	u_short Status;

	if ( Type == 0 )
		{
		Status = GetA8RegW( 0x5000 );
		Status = Status & 0xFB;
		SetA8RegW( 0x5000, Status );

		Status = 0x01;					// For JemiTek LCD Panel
		SetA8RegW( 0x500A, Status );
		}
	else if ( Type == 1 )
		{
		Status = GetA8RegW( 0x500A );
		Status = Status & 0xFE;
		SetA8RegW( 0x500A, Status );
		
		Status = GetA8RegW( 0x5000 );
		Status = Status | 0x04;
		SetA8RegW( 0x5000, Status );
		}
	else if ( Type == 2 )
		{
		Status = GetA8RegW( 0x500A );
		Status = Status & 0xFE;
		SetA8RegW( 0x500A, Status );

		Status = GetA8RegW( 0x5000 );
		Status = Status & 0xFB;
		SetA8RegW( 0x5000, Status );
		}
		
		return A8_NO_ERROR;
}

/**
 @fn		s_short A8L_CheckReadyForA8ByPass()
 @brief	Check if AIT ready for HW Bypass.
 @return	s_short - 0:Not ready, 1:Ready
 @note	.
 @bug	N/A.
*/
s_short A8L_CheckReadyForA8ByPass()
{
	SendA8Cmd( A8_HOST_CMD_BY_PASS_REQ );

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}

	return A8_NO_ERROR;
}



/**
 @fn		s_short A8L_CheckReadyForA8Resume()
 @brief	Check if AIT ready for SW Resume.
 @return	s_short - 0:Not ready, 1:Ready
 @note	.
 @bug	N/A.
*/
s_short A8L_CheckReadyForA8Resume()
{
	s_short retVal;
	
	retVal = SendA8Cmd( A8_HOST_CMD_RESUME_REQ );
	if ( retVal != 0 )
	   return A8_SYSTEM_ERROR;

	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}

	return A8_NO_ERROR;
}

/**
 @fn		s_short A8L_CheckReadyForA8Command()
 @brief	Check if AIT ready for Command.
 @return	s_short - 0:Ready, 1:Not ready
 @note	.
 @bug	N/A.
*/
s_short A8L_CheckReadyForA8Command(void)
{
	s_int timeout = 0;
	
	while(((GetA8DSCStatus() & A8_DSC_READY_FOR_CMD) == 0) && (timeout < A8_CMD_READY_TIME_OUT) )
	{
		sys_IF_ait_delay1us(100);	// Sleep delay about 0.1ms or 1ms. Depends on BB System delay function.
		timeout++;
	}

	MSG_FATAL("A8L_CheckReadyForA8Command timeout = %d",timeout,0,0);
	if ( timeout >= A8_CMD_READY_TIME_OUT )
	{
		AIT_Message_Error("A8L_CheckReadyForA8Command Error!",0);
		return A8_TIMEOUT_ERROR;
	}
	else
		return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_CheckReadyForPreview(u_char cStart)
 @brief	Check if AIT ready for preview start/stop.
 @param	cStart - 1: check start preview.  0: check stop preview.
 @return	s_short - 0:Ready, 1:Not ready
 @note	.
 @bug	N/A.
*/
s_short A8L_CheckReadyForPreview(u_char cStart)
{
	u_int timeout = 0;

	if(0==cStart)	//Wait until stop preview
	{
		while(((GetA8RegW(A8_DSC_STATUS_RG) & A8_DSC_PREVIEW_START_ST) != 0x00) && (timeout < A8_SWITCH_PREVIEW_TIME_OUT) )
		{
			sys_IF_ait_delay1ms(2);
			timeout++;
		}
		if (timeout >= A8_SWITCH_PREVIEW_TIME_OUT) 
		{
			AIT_Message_P0("Stop Preview Check timeout!! /n");
			return  A8_CAM_PREV_STOP_ERROR;
		}
	}
	else 	//Wait until start preview
	{
		sys_IF_ait_delay1ms(4); // 0507 for check preview start return value
		while(((GetA8RegW( A8_DSC_STATUS_RG ) & A8_DSC_PREVIEW_START_ST) == 0x00) && timeout < A8_SWITCH_PREVIEW_TIME_OUT )
		{
           sys_IF_ait_delay1ms(2);
			timeout++;
		}
		if (timeout >= A8_SWITCH_PREVIEW_TIME_OUT) 
		{
			AIT_Message_P0("Start Preview Check timeout!! /n");
			return A8_CAM_PREV_START_ERROR;
		}	
	}

	return A8_NO_ERROR;
}


/**
 @fn		s_short A8L_SetZoomRate( u_short ZoomRate )
 @brief	Setup ZoomRate.
 @param	ZoomRate - .
 @return	s_short
 @note	(For Test!).
 @bug	N/A.
*/
s_short A8L_SetZoomRate( u_short ZoomRate )
{
	u_short Cmd;
	s_short retVal;
	
	Cmd = ( ZoomRate << 8 ) + A8_HOST_CMD_SET_ZOOM_RATE;
	retVal = SendA8Cmd( Cmd );
	
	if (retVal != 0 )
	   return retVal;
	
	if(A8L_CheckReadyForA8Command())
	{
		return A8_TIMEOUT_ERROR;
	}
	else
	return A8_NO_ERROR;
}


/**
 @fn		u_short A8L_GetA8FirmwareVersion()
 @brief	Get Firmware Version.
 @return	s_short - 0xFFFF:Time out failure , Other:Firmware Version
 @note	.
 @bug	N/A.
*/
u_short A8L_GetA8FirmwareVersion()
{
	u_short Ver;
	
	SetA8RegW( 0x654C, 0x0000 );

	if(A8L_CheckReadyForA8Command())
	{
		return 0xFFFF;
	}	

	SendA8Cmd( A8_HOST_CMD_GET_FW_VER );
	
	if(A8L_CheckReadyForA8Command())
	{
		return 0xFFFF;
	}	

	Ver = GetA8RegW( 0x654C );
	return Ver;
}


/**
 @fn		u_short A8L_GetA8ChipVersion()
 @brief	Get Chip Version.
 @return	s_short - 0xFFFF:Time out failure , Other:Chip Version
 @note	(For Test!).
 @bug	N/A.
*/
u_short A8L_GetA8ChipVersion()
{
	u_short Ver;
	
	SetA8RegW( 0x654C, 0x0000 );
	
	if(A8L_CheckReadyForA8Command())
	{
		return 0xFFFF;
	}		

	SendA8Cmd( A8_HOST_CMD_GET_CHIP_VER );

	
	if(A8L_CheckReadyForA8Command())
	{
		return 0xFFFF;
	}

	Ver = GetA8RegW( 0x654C );
	return Ver;
}

//=========================================================================
// A8L_SaveScalerSetting
// Arguments :
//		None
// Return Value :
//		0x00 ... Success, 0x01 ... timeout failure
//=========================================================================
s_short A8L_SaveScalerSetting( A8ScaleDT *Scaler )
{
	Scaler->frontx0 = GetA8RegW( 0x4F00 );
	Scaler->frontx1 = GetA8RegW( 0x4F02 );
	Scaler->fronty0 = GetA8RegW( 0x4F04 );
	Scaler->fronty1 = GetA8RegW( 0x4F06 );
	
	Scaler->backx0 = GetA8RegW( 0x4F08 );
	Scaler->backx1 = GetA8RegW( 0x4F0A );
	Scaler->backy0 = GetA8RegW( 0x4F0C );
	Scaler->backy1 = GetA8RegW( 0x4F0E );
	
	Scaler->control = GetA8RegB( 0x4F14 );
	Scaler->m = GetA8RegB( 0x4F16 );
	Scaler->n = GetA8RegB( 0x4F15 );
	return A8_NO_ERROR;
}

//=========================================================================
// A8L_RestoreScalerSetting
// Arguments :
//		None
// Return Value :
//		0x00 ... Success, 0x01 ... timeout failure
//=========================================================================
s_short A8L_RestoreScalerSetting( A8ScaleDT *Scaler )
{
	SetA8RegW( 0x4F00, Scaler->frontx0 );
	SetA8RegW( 0x4F02, Scaler->frontx1 );
	SetA8RegW( 0x4F04, Scaler->fronty0 );
	SetA8RegW( 0x4F06, Scaler->fronty1 );
	
	SetA8RegW( 0x4F08, Scaler->backx0 );
	SetA8RegW( 0x4F0A, Scaler->backx1 );
	SetA8RegW( 0x4F0C, Scaler->backy0 );
	SetA8RegW( 0x4F0E, Scaler->backy1 );
	
	SetA8RegB( 0x4F14, Scaler->control );
	SetA8RegB( 0x4F16, Scaler->m );
	SetA8RegB( 0x4F15, Scaler->n );

	return A8_NO_ERROR;
}



//=========================================================================
// A8L_SetZoomStep
// Arguments :
//		None
// Return Value :
//		0x00 ... Success, 0x01 ... timeout failure
//=========================================================================
s_short A8L_SetZoomStep( u_short Step, s_short Offset, u_short FPS )
{
	u_short Temp;
	
	Temp = GetA8RegB( 0x4F30 );
	Temp = ( Temp & 0x0C ) + ((FPS-1) << 4);
	SetA8RegB( 0x4F30, Temp );
	
	SetA8RegB( 0x4F31, Step );
	SetA8RegB( 0x4F32, Offset );
	SetA8RegB( 0x4F33, Offset );
	
	return A8_NO_ERROR;
}

#ifdef EXIFSUPPORT
s_int GetJpegPictureWithEXIF( u_short *JpegBufferAddr, u_char *makerName, u_char *modelName, u_char *DateTimeInfo)
{
	s_int JpegPictLength = 0;
	u_short *JpegBufferPtr;
	u_short Status, Length, i;
	u_char *ExifPtr;
	s_int timeout = 0;
	ExifPtr = (u_char *)JpegBufferAddr;
#ifndef EXIFSUPPORT
	SetEXIFMakeModelInfo(gMakeValue, gModelValue);
	SetEXIFDateTimeInfo(gDateTimeInfo);
	SetEXIFZoomInfo();
	SetEXIFImageResolutionInfo();
	SetEXIFExposureInfo();
	SetEXIFExposureBiasInfo();
#endif    
//	AIT_Message_P0("GetJpegPictureWithEXIF : 1 Step \n");

	for(i=0;i<0x314;i++)
	{
		*(ExifPtr++) = (u_char)(EXIF_Header_Table[i]);
	}

	JpegPictLength += 0x189;
	JpegBufferPtr = JpegBufferAddr + 0x189;	

	do
		{
		Status = GetA8RegW( 0x6222 );
		Length = Status & 0x1F;		
		if ( Length == 0 )
			{
			timeout++;
			if ( timeout > A8_GET_JPEG_TIMEOUT )
				{
				SetA8RegW(0x6222, 0x00);
				/*
				A8DM_AddTimeoutInfo( 0x0010000, "Get jpeg timeout", "GetJpegPicture()" );
				A8DM_SaveRegister( 0x6510, GetA8RegW( 0x6510 ) );
				A8DM_SaveRegister( 0x6222, GetA8RegW( 0x6222 ) );
				A8DM_SaveRegister( 0x6204, GetA8RegW( 0x6204 ) );
				*/
				AIT_Message_P0("GetJpegPictureWithEXIF : timeout \n");
				return A8_NO_ERROR;
				}
			}
		while ( Length > 0 )
			{
			if ( JpegBufferPtr < (JpegBufferAddr + MAX_JPEG_SIZE) )
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<Length; i++ )
					{
					*(JpegBufferPtr++) = A8IndDataPW( i );
					}
				}
			else
				{
				A8IndCmdP = HII_SET_ADDR_L;
				A8IndDataPW(0) = 0x6220;
				A8IndCmdP = HII_RW_REG_W;
				for ( i=0; i<Length; i++ )
					{
					*(JpegBufferPtr+1) = A8IndDataPW( i );
					}
				}
			JpegPictLength += Length;
			Status = GetA8RegW( 0x6222 );
			Length = Status & 0x1F;		
			} 
		}
	while ( (Status & 0x0100) != 0x0100 );

	Status = GetA8RegW( 0x6222 );
	Length = Status & 0x1F;		

	JpegPictLength += Length;
	while ( Length > 0 )
		{
		*(JpegBufferPtr++) = GetA8RegW( 0x6220 );
		Length--;
		} 

	SetA8RegW( 0x6222, 0x0000 );
	return (JpegPictLength << 1);
}

void SetEXIFMakeModelInfo(u_char *makerName, u_char *modelName)
{
	u_char i;
	
	for (i=0;i<13;i++)
		EXIF_Header_Table[172+i] = makerName[i];/* ex. "AIT Co"*/
	for (i=0;i<9;i++)
		EXIF_Header_Table[186+i] = modelName[i];/* ex. "701/703"*/
}

void SetEXIFZoomInfo(void)
{
	u_short ZoomPara;
#if 0 	
	u_short SD_Mode, NValue;
	SD_Mode = GetA8RegB(0x4f10);
	if (SD_Mode&0x01)
		NValue = GetA8RegB(0x4f15)/4;
	else if (SD_Mode&0x02)
		NValue = GetA8RegB(0x4f15)/2;
	else
		NValue = GetA8RegB(0x4f15);
	
	//ZoomPara = NValue*100/(A800Data.SelectPrev->UpScale*gA8PrevScaleRatio); 
	ZoomPara = NValue*100/(gA8CurrentZoomRate*gA8PrevScaleRatio);
#else // 0425
	ZoomPara = 100 + gA8PrevZoomFactor * 100/24;
#endif
	EXIF_Header_Table[0x2E8] = ZoomPara & 0xFF;
	EXIF_Header_Table[0x2E9] = (ZoomPara & 0xFF00) >> 8;
}
void SetEXIFImageResolutionInfo(void)
{
	u_short width, height;
	switch(gCurrentCaptureResolution)
	{
		case 0:
		width = 640;
		height = 480;
		break;
		case 1:
		width = 320;
		height = 240;
		break;
		case 2:
			width = 128;
			height = 160;
		break;
		case 3:
			width = 128;
			height = 96;
			break;
		case 4: // for CROP shot
			width = 64;
			height = 64;
			break;
            	case 5: // for Frame Shot
                    width = 128;
                    height = 96;
                    break;
              case 9: //for MOSAIC Shot
                	width = 128;
                    height = 96;
		break;
		default:
			width = 640;
			height = 480;
		break;
	}

	EXIF_Header_Table[0x216] = width & 0xFF;
	EXIF_Header_Table[0x217] = (width & 0xFF00) >> 8;
	EXIF_Header_Table[0x222] = height & 0xFF;
	EXIF_Header_Table[0x223] = (height & 0xFF00) >> 8;
}
void SetEXIFDateTimeInfo(u_char *DateTimeInfo)
{
	s_int i;
	s_int currentPos;
	
	for (i=0;i<0x13;i++) {
		DateTimePtr[i] = DateTimeInfo[i];		/* BB Date and Time : ex:"2006.11.17 13:13:13"*/
	}
	currentPos = 0xD4;
	for(i = 0 ; i < 0x14; i ++)
	{
		EXIF_Header_Table[currentPos+i] = DateTimePtr[i];
	}
	currentPos = 0x252;
	for(i = 0 ; i < 0x14; i ++)
	{
		EXIF_Header_Table[currentPos+i] = DateTimePtr[i]; 
	}
	currentPos = 0x266;
	for(i = 0 ; i < 0x14; i ++)
	{
		EXIF_Header_Table[currentPos+i] = DateTimePtr[i]; 
	}
}

s_int ExpoTimeTab[17] = {30000, 15000, 8000, 4000, 2000, 1000, 500, 250, 125, 67, 33, 17, 8, 4, 2, 1, 0}; 
s_int ExpoInfoTab[17] = {30, 15, 8, 4, 2, 1, 2, 4, 8, 15, 30, 60, 125, 250, 500, 1000, 2000}; 

void SetEXIFExposureInfo(void)
{
	s_int ShutterValue=1;
	s_int ExpoN, ExpoM;

#if 0
	u_char EXPOLOW, EXPOMID, EXPOHI;
	s_int expoCount;
	s_int ExposureVal; /* Unit is msec.*/
	s_int i;

	A800_I2CWrite(0x03, 0x03); 
	EXPOLOW = A800_I2CRead(0x32);
	EXPOMID = A800_I2CRead(0x31);
	EXPOHI = A800_I2CRead(0x30);

	expoCount = (EXPOHI << 16)|(EXPOMID << 8)|(EXPOLOW);
	ExposureVal = (s_int)(expoCount	/ 26 * 8 / 1000);

	for (i = 0 ; i < 16 ; i ++)
	{
		if (ExposureVal == ExpoTimeTab[i])
		{
			ShutterValue = i - 5;
			break;
		}
		else if ((ExposureVal < ExpoTimeTab[i]) && (ExposureVal >= ExpoTimeTab[i+1])) 
		{
			ShutterValue = i - 4;
			break;
		}
	}
	if (ShutterValue <= 0)
	{
		ExpoN = ExpoInfoTab[ShutterValue+5];
		ExpoM = 1;
	}
	else
	{
		ExpoN = 1;
		ExpoM = ExpoInfoTab[ShutterValue+5];
	}	
#else // 0425  set just anyvalue for temp test  
	ExpoN = 1;
	ExpoM = 8;
	ShutterValue = 7;
#endif	
	EXIF_Header_Table[0x242] = ExpoN & 0xFF;
	EXIF_Header_Table[0x243] = (ExpoN & 0xFF00) >> 8;
	EXIF_Header_Table[0x244] = (ExpoN & 0xFF0000) >> 16;
	EXIF_Header_Table[0x245] = (ExpoN & 0xFF000000) >> 24;
	EXIF_Header_Table[0x246] = ExpoM & 0xFF;
	EXIF_Header_Table[0x247] = (ExpoM & 0xFF00) >> 8;
	EXIF_Header_Table[0x248] = (ExpoM & 0xFF0000) >> 16;
	EXIF_Header_Table[0x249] = (ExpoM & 0xFF000000) >> 24;
	
	EXIF_Header_Table[0x282] = ShutterValue & 0xFF;
	EXIF_Header_Table[0x283] = (ShutterValue & 0xFF00) >> 8;
	EXIF_Header_Table[0x284] = (ShutterValue & 0xFF0000) >> 16;
	EXIF_Header_Table[0x285] = (ShutterValue & 0xFF000000) >> 24;
}

void SetEXIFExposureBiasInfo(void)
{
	EXIF_Header_Table[0x29A] = sEXIFEVBias & 0xFF;
	EXIF_Header_Table[0x29B] = (sEXIFEVBias & 0xFF00) >> 8;
	EXIF_Header_Table[0x29C] = (sEXIFEVBias & 0xFF0000) >> 16;
	EXIF_Header_Table[0x29D] = (sEXIFEVBias & 0xFF000000) >> 24;
}
/* move to dcam1*/
/* Ychuls 080903 exifheader for BTprint */
void Remove_EXIFHeader_BTprint(u_short *JpegBufferAddr, u_int *lv_CaptureSize)
{
        memcpy((u_char *)JpegBufferAddr+0x59E, (u_char *)JpegBufferAddr+0x5BC, (*lv_CaptureSize)-0x5BB);
        lv_CaptureSize-= 0x1E;
}
#endif

#if 1
u_short gA8AlphaRate[8] = { 8, 6, 5, 4, 3, 2, 1, 0 };


s_short A8L_MergeImageWithOSD( u_short *FrameImg, u_short *OSDImg, u_short FrameKeyColor, u_int BufAddr, s_int Length )
{
   s_short OsdARate, FrameARate;
   register u_short TempO, TempF;
   u_short TempR, TempG, TempB;
   s_int i;
   
	A8IndCmdP = HII_SET_ADDR_H;
	A8IndDataPW(0) = (u_short)(BufAddr>>16);
	A8IndCmdP = HII_SET_ADDR_L;
	A8IndDataPW(0) = (u_short)(BufAddr & 0xFFFF);
	A8IndCmdP = HII_RW_MEM_W_INC;
   
   for ( i=0; i<Length; i++ )
   {

      if ( (*OSDImg & 0xE000) == 0xE000 )
      {
         if ( *FrameImg == FrameKeyColor )
         {
            TempF = 0xE000;
         }
         else
         {
             TempF = *FrameImg;
             TempF = ((TempF & 0x001F)>>1) + ((TempF & 0x07c0)>>2) + ((TempF & 0xF000)>>3) + 0x0000;
         }
      }
      else
      {
         if ( *FrameImg == FrameKeyColor )
         {
            TempF = *OSDImg;
         }
         else
         {
            OsdARate = gA8AlphaRate[ *OSDImg >> 13 ];
            FrameARate = 8 - OsdARate;
            TempO = *OSDImg;
            TempF = *FrameImg;
            TempR = (u_short)((((u_int)(TempO & 0x000F)) * OsdARate + ((TempF & 0x001F)>>1) * FrameARate) >> 3);
            if ( TempR > 0x1F )
               TempR = 0x1F;
            TempG = (u_short)((((u_int)(TempO & 0x01F0)) * OsdARate + ((TempF & 0x07c0)>>2) * FrameARate) >> 3);
            TempG = TempG & 0xFFF0;
            if ( TempG > 0x01F0 )
               TempG = 0x01F0;
            TempB = (u_short)((((u_int)(TempO & 0x1E00)) * OsdARate + ((TempF & 0xF000)>>3) * FrameARate) >> 3);
            TempB = TempB & 0xFE00;
            if ( TempB > 0x1E00 )
               TempB = 0x1E00;
            TempF = 0x0000 + TempR + TempG + TempB;
         }
      }

      A8IndDataPW(0) = TempF;
      OSDImg++;
      FrameImg++;
   }
   return A8_NO_ERROR;
}

#endif
u_short gA8AlphaRate1[8] = { 8, 6, 5, 4, 3, 2, 1, 0 };

s_short A8L_UpdateIconOSD( u_short *Obuf, u_int BufAddr, u_short IconW, u_short IconH, u_short IconX, u_short IconY)
{
     u_short *pOsdBuf;
     u_short k,j;

	u_char TempData;

    pOsdBuf = Obuf;

   
    for(k=IconY; k<(IconY+IconH); k++)
    {
        for(j=IconX; j<(IconX+IconW); j++)
        {
            pOsdBuf = Obuf+(176*k +j); 
            if((*pOsdBuf & 0xE000) == 0xE000) // TRANSPARENCY 100
            {
                TempData = 0x00;
            }
            else
            {
                TempData =  (u_char)((*pOsdBuf & 0x1C00)>>5) | (u_char)((*pOsdBuf & 0x01C0) >>4) |(u_char)((*pOsdBuf & 0x000C) >>2);
            }

            SetA8MemB(BufAddr, TempData);
            BufAddr++;
        }


    }   
 	return A8_NO_ERROR;
}    

u_short A8L_SetVideoResolution(u_short width, u_short height, u_short colorFormat, u_short fileFormat)
{
	switch(fileFormat)
	{
	case FILE_FORMAT_AVI:
		if (colorFormat == FORMAT_YUV_422)
		{
			SetA8RegW(0x6200, width);
			SetA8RegW(0x6202, height);
		}
		else
		{
			return A8_OUT_OF_RANGE_ERROR;
		}
		break;
	default:
		return A8_OUT_OF_RANGE_ERROR;
	}
	return A8_NO_ERROR;
}	

u_short A8L_SetVideoCaptureMemConfig(u_int encodeAddrStart, u_int encodeAddrEnd, u_int lineBufAddr)
{	
	u_int	gsLineBufAddr;
	u_int	gsEncodeBufStartAddr;
	u_int	gsEncodeBufEndAddr;
	
	gsLineBufAddr = lineBufAddr;
	gsEncodeBufStartAddr = (encodeAddrStart+0x7) & 0xfffffff8;
	gsEncodeBufEndAddr = ((encodeAddrEnd+0x1) & 0xfffffff8)-1;

	SetA8RegW(0x6210, gsLineBufAddr & 0xFFFF);					//JPEG block buffer start address
	SetA8RegW(0x6212, gsLineBufAddr >> 16);						//JPEG block buffer start address
	SetA8RegW(0x6214, gsEncodeBufStartAddr & 0xFFFF);	//JPEG compressed buffer start address
	SetA8RegW(0x6216, gsEncodeBufStartAddr >>16);			//JPEG compressed buffer start address
	SetA8RegW(0x6218, gsEncodeBufEndAddr & 0xFFFF);		//JPEG compressed buffer end address	10K
	SetA8RegW(0x621A, gsEncodeBufEndAddr >> 16);			//JPEG compressed buffer end address	10K
	
	AIT_Message_P1("gsEncodeBufStartAddr=0x%x \r\n",gsEncodeBufStartAddr);
	AIT_Message_P1("gsEncodeBufEndAddr=0x%x \r\n",gsEncodeBufEndAddr);
	
	return A8_NO_ERROR;
}	

u_short	A8L_StartMJPEGCapture(void)
{
	// Reduce frame rate
	SetA8RegB(0x6204, 0x00);	// Disable H.263

	//SetA8RegB(0x6511, GetA8RegB(0x6511) & ~(0x04));	
	//waitCount(1000);
	SetA8RegB(0x6511, GetA8RegB(0x6511) | (0x04));	
	//waitCount(1000);
	SetA8RegB(0x6511, GetA8RegB(0x6511) & ~(0x04));	
	SetA8RegW(0x690A, 0x0008);	// JPEG engine reset
	sys_IF_ait_delay1us(20); //need delay 20us, else it will be abnormal on sector r/w.
	SetA8RegW(0x690A, 0x0000);
	
	
	//SetA8RegW(0x6914, 0x000A);	// JPEG engine reset
	//waitCount(1000);
	//SetA8RegW(0x6914, 0x0000);
	
	SetA8RegB(0x6242, 0x03);

//	SendA8Cmd(A8_HOST_CMD_START_MJPEG_REC);
	//SetA8RegB(0x6511, 0x01);
	//SetA8RegB(0x6204, 0x40);	//JPEG control //AVI hardware marker enable
	//SetA8RegB(0x6510, 0x3C);	//JPEG control //AVI enable, JPEG hardware marker enable
	SetA8RegB(0x6222, 0x80);		// Enable JPEG FIFO for retrieving video data (AIT802)
	//SetA8RegB(0x6204, 0x70);	//JPEG control //AVI enable, JPEG hardware marker enable
	
	//Setting JPEG enable bit
	//Ring buffer back to the top of the buffer
	//SetA8RegB(0x6242, 0x01);

#if defined(AIT701_DEMO_BOARD)
	//SetA8RegB(0x6204, 0x28);//SetA8RegB(0x6204, 0x68);
#else
	//SetA8RegB(0x6204, 0x28);
#endif

	return A8_NO_ERROR;
}

u_char A8L_TransferMJPEGFrameToHost(u_char *frameBuf, u_int *frameSize)
{
	u_int transfercount = 0;
	u_int jpgSize;

	*frameSize = 0;
#if 0	//removed by ming @20090422	

	SetA8RegB(0x6242, 0x01); // clear JPEG frame encode done INT_SR
	SetA8RegB(0x6204, 0x28); // Marker auto gen, JPEG encode EN

    // Polling for JPEG frame encode done, or timeout
	while((!(GetA8RegB(0x6242) & 0x01)) && (timeout < 1000))
	{
		sys_IF_ait_delay1us(500);
		timeout++;
	};
	if (timeout >= 1000)
	{
		return 1;
	}
#endif
	transfercount = GetA8RegW(0x621C); //JPEG/Video compress frame size
	jpgSize = transfercount;
	if (transfercount)
	{
		if((transfercount % 8) != 0)
			transfercount += (u_char) (8 - (transfercount % 8));
		
		*frameSize = transfercount;

		CopyMemByteA8ToHost((u_char *)frameBuf, A8_AVI_ENCODE_COMP_BUF_START_ADDR, transfercount);
	}

	{

		if(frameBuf[1]!=0xD8||frameBuf[jpgSize-1]!=0xD9)
			AIT_Message_P2("JPEG: %x,%x\r\n", frameBuf[1],frameBuf[jpgSize-1]);
	}



	
#if 0	//added by ming @20090422	
	SetA8RegB(0x6204, 0x28);
#endif
	return A8_NO_ERROR;
}
