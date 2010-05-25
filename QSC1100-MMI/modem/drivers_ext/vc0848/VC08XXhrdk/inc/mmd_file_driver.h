/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2008                                                     
 * ------------------------------------------------------------------------------
 * [File Name] 		: mmd_file_driver.h
 * [Description] 		: File mode header file
 * [Author] 			: chenjingen
 * [Date Of Creation] 	: 2008-04-15
 * [Platform] 			: Platform independent
 * [Note] 				: None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     			Modifications
 * ------------------------------------------------------------------------------
 * 2008-04-15 		chenjingen 			Modified for VC0848 SDK
 *******************************************************************************/
/*******************************************************************************
 * This source code has been made available to you by VIMICRO on an
 * AS-IS basis. Anyone receiving this source code is licensed under VIMICRO
 * copyrights to use it in any way he or she deems fit, including copying it,
 * modifying it, compiling it, and redistributing it either with or without
 * modifications. Any person who transfers this source code or any derivative
 * work must include the VIMICRO copyright notice and this paragraph in
 * the transferred software.
 *******************************************************************************/
 
#ifndef _MMD_FILE_DRIVER_H_
#define _MMD_FILE_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mmp_rdk.h"
#include "..\Drivers\fat\thin_usr.h"

#define MMD_FILE_COPY_BUFFER_SIZE 			(4*1024) 	/* bytes, the size of buffer for copy */
#define MMD_FILE_FASTCOPY_BUFFER_SIZE 		(40*1024)	/* bytes, the size of read or write in fast copy time isr */
#define MMD_FILE_FASTCOPY_TIMER_INTERVAL 	(40) 		/* ms, the interval of fast copy timer */

enum {
	MMD_FILE_NO_ERROR 				= 0, 	/* operation successful*/
	MMD_FILE_ERR_RESERVED 			= 1, 	/* reserved */
	MMD_FILE_ERR_NOTFORMATTED		= 2, 	/* the media is not formatted */
	MMD_FILE_ERR_INVALIDDIR			= 3,  	/* invalid directory path */
	MMD_FILE_ERR_INVALIDNAME		= 4, 	/* invalid name of file or directory */
	MMD_FILE_ERR_NOTFOUND			= 5, 	/* file or directory is not found */
	MMD_FILE_ERR_DUPLICATED			= 6, 	/* file or directory is already existed */
	MMD_FILE_ERR_NOMOREENTRY		= 7, 	/* no more space for new file entry */
	MMD_FILE_ERR_NOTOPEN			= 8, 	/* file is not opened */
	MMD_FILE_ERR_EOF				= 9, 	/* reachs the end of file */
	MMD_FILE_ERR_NULLPOINTER		= 10, 	/* cannot allocate memory or null-pointer variable */
	MMD_FILE_ERR_NOTUSEABLE			= 11, 	/* invalid parameter or option */
	MMD_FILE_ERR_LOCKED				= 12, 	/* file or directory is locked by current operation */
	MMD_FILE_ERR_ACCESSDENIED		= 13, 	/* media access denied */
	MMD_FILE_ERR_NOTEMPTY			= 14, 	/* directory is not empty */
	MMD_FILE_ERR_INITFUNC			= 15, 	/* media initializtion failed */
	MMD_FILE_ERR_CARDREMOVED		= 16, 	/* media is removed */
	MMD_FILE_ERR_ONDRIVE			= 17, 	/* media drive does not work */
	MMD_FILE_ERR_INVALIDSECTOR		= 18, 	/* invalid sector of current media */
	MMD_FILE_ERR_READ				= 19, 	/* read sector(s) failed */
	MMD_FILE_ERR_WRITE				= 20, 	/* write sector(s) failed */
	MMD_FILE_ERR_INVALIDMEDIA		= 21, 	/* invalid media format */
	MMD_FILE_ERR_BUSY				= 22, 	/* media is busy */
	MMD_FILE_ERR_WRITEPROTECT		= 23, 	/* media is write protected */
	MMD_FILE_ERR_INVFATTYPE			= 24, 	/* invalid FAT type */
	MMD_FILE_ERR_MEDIATOOSMALL		= 25, 	/* the size of media is too small to format */
	MMD_FILE_ERR_MEDIATOOLARGE		= 26, 	/* the size of media is too large to format */
	MMD_FILE_ERR_NOTSUPPSECTORSIZE	= 27,	/* unsupported sector size, only 512-byte sector is supported in this version */
	MMD_FILE_ERR_CANNOTOPEN 		= 28,	/* open file failed */
	MMD_FILE_ERR_DEVMALLOCFAIL 		= 29,	/* VC0848 SRAM malloc fail */
	MMD_FILE_ERR_NOMOREHANDLE 		= 30, 	/* no more vacant handle for opening file */
	MMD_FILE_ERR_GETSECTORCHAIN 	= 31, 	/* get sector chain fail */
	MMD_FILE_ERR_ALLOCSECTORCHAIN 	= 32, 	/* alloc sector chain fail */
	MMD_FILE_ERR_ACKTIMEOUT			= 33, 	/* wait command ack timeout */
	MMD_FILE_ERR_FASTCOPYBUSY		= 34, 	/* can't start a new fast copy for there is already one under processing */
	MMD_FILE_ERR_FASTCOPYNOTPAUSE	= 35,	/* can't resume a fast copy that is not paused */
	MMD_FILE_ERR_CORESDRW 			= 36, 	/* VC0848 Core SD read or write error */
	MMD_FILE_ERR_NOMOREDATA 		= 37, 	/* no more data for file reading */
	MMD_FILE_ERR_NOMORESPACE 		= 38	/* no more space for file writing */
};

typedef void (*F_CALLBACK)(MMD_U32 err_code);


typedef struct __F_COREMEM
{
	MMD_U32 addr; 	/* SRAM addr in core */
	MMD_U32 reg;  	/* Index of status registers in core */
	MMD_U32 bit; 	/* The bit of specified status register */
}F_COREMEM;

MMD_U32 MMD_Mode_File_Lisr(MMD_U32 cmd);
MMD_U32 MMD_Mode_File_Hisr(MMD_U32 cmd);
/*--------------- Volume APIs ---------------*/
MMD_U32 MMD_FILE_Mount(void);
MMD_U32 MMD_FILE_SetLabel(MMD_U08 *pbLabel);
MMD_U32 MMD_FILE_GetLabel(MMD_U08 *pbLabel);
MMD_U32 MMD_FILE_Format(MMD_U08 bFATType,MMD_U08 bHardFlag);
MMD_U32 MMD_FILE_GetSpace(F_SPACE *psSpace);
MMD_U32 MMD_FILE_GetClusterSize(MMD_U32 *pdSize);
/*--------------- Common APIs ---------------*/
MMD_U32 MMD_FILE_FindFirst(MMD_U08 *pbPath, F_FIND *psFind);
MMD_U32 MMD_FILE_FindNext(F_FIND *psFind);
MMD_U32 MMD_FILE_Rename(MMD_U08 *pbName,MMD_U08 *pbNewName);
MMD_U32 MMD_FILE_GetTimeDate(MMD_U08 *pbPath, MMD_DATE *psDate, MMD_TIME *psTime);
MMD_U32 MMD_FILE_SetTimeDate(MMD_U08 *pbPath, MMD_DATE *psDate, MMD_TIME *psTime);
MMD_U32 MMD_FILE_GetAttr(MMD_U08 *pbPath,MMD_U08 *pbAttr);
MMD_U32 MMD_FILE_SetAttr(MMD_U08 *pbPath,MMD_U08 bAttr);
/*--------------- Directory APIs ---------------*/
MMD_U32 MMD_FILE_MkDir(MMD_U08 *pbPath);
MMD_U32 MMD_FILE_RmDir(MMD_U08 *pbPath);
MMD_U32 MMD_FILE_ChDir(MMD_U08 *pbPath);
MMD_U32 MMD_FILE_GetCWD(MMD_U08 *pbPath);
/*--------------- File APIs ---------------*/
MMD_U32 MMD_FILE_Open(MMD_U08 *pbPath, MMD_U08 *pbMode, F_FILE **ppFile);
MMD_U32 MMD_FILE_Close(F_FILE *psFile);
MMD_U32 MMD_FILE_Read(F_FILE *psFile, MMD_U08 *pbData, MMD_U32 dLength,MMD_U32 *pdRdLen);
MMD_U32 MMD_FILE_Write(F_FILE *psFile, MMD_U08 *pbData, MMD_U32 dLength, MMD_U32 *pdWrLen);
MMD_U32 MMD_FILE_Seek(F_FILE *psFile, MMD_U32 dOffset, MMD_U08 bWhence);
MMD_U32 MMD_FILE_Tell(F_FILE *psFile, MMD_U32 *pdPos);
MMD_U32 MMD_FILE_SetEOF(F_FILE *psFile, MMD_U32 dSize);
MMD_U32 MMD_FILE_Delete(MMD_U08 *pbPath);
MMD_U32 MMD_FILE_Copy(MMD_U08 *pbSrcPath, MMD_U08 *pbDestPath);
MMD_U32 MMD_FILE_StartFastCopy(MMD_U08 *pbSrcPath, MMD_U08 *pbDestPath, F_CALLBACK pfCallback);
MMD_U32 MMD_FILE_AbortFastCopy(void);
MMD_U32 MMD_FILE_PauseFastCopy(void);
MMD_U32 MMD_FILE_ResumeFastCopy(void);
MMD_U08 MMD_FILE_GetFastCopyProgress(void);
MMD_U32 MMD_FILE_IsFileExisted(MMD_U08 *pbPath, MMD_U08 *pbFlag);
MMD_U32 MMD_FILE_InitCore(void);
MMD_U32 MMD_FILE_Read2Core(F_FILE *psFile, F_COREMEM sCoreMem, MMD_U32 dLength, MMD_U32 *pdRdLen);
MMD_U32 MMD_FILE_StartRead2Core(void);
MMD_U32 MMD_FILE_WriteFrCore(F_FILE *psFile, F_COREMEM sCoreMem, MMD_U32 dLength, MMD_U32 *pdWrLen);
MMD_U32 MMD_FILE_StartWriteFrCore(void);
MMD_U32 MMD_FILE_DupHandle(F_FILE *psFileSrc, F_FILE **ppFile);
MMD_U32 MMD_FILE_Combine(MMD_U08 *pbPath1, MMD_U08 *pbPath2);
/*--------------- SD Sector Read & Write APIs ---------------*/
MMD_U32 MMD_FILE_ReadSDSectors(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum);
MMD_U32 MMD_FILE_WriteSDSectors(MMD_U08 *pbDataBuf, MMD_U32 dSecAddr, MMD_U32 dNum);

#ifdef __cplusplus
}
#endif

#endif /*_MMD_FILE_DRIVER_H_*/
