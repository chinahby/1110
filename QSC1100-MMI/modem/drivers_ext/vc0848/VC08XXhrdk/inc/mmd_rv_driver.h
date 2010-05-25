/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2008                                                     
 * ------------------------------------------------------------------------------
 * [File Name] 		: mmd_rv_driver.h
 * [Description] 		: RV mode header file
 * [Author] 			: anton liu
 * [Date Of Creation] 	: 2008-09-01
 * [Platform] 			: Platform independent
 * [Note] 				: None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        			Author     			Modifications
 * ------------------------------------------------------------------------------
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
 
#ifndef _MMD_RV_DRIVER_H_
#define _MMD_RV_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mmp_rdk.h"
#include "mmd_ply_driver.h"

#define RV_FILE_FIFO_SIZE	0X80000		//512k
#define RV_BUF_NUM		4
#define RV_BUF_SIZE		0x20000	  	

#define RV_TIMER_INV	100
#define RV_TIMER_NUM	5
#define RV_TIMER_STOP	1
#define RV_TIMER_SET	2
#define RV_TIMER_START	3

#define RV_TIMER_DATA		0
#define RV_TIMER_DISP		1
#define RV_TIMER_DispFresh	2
#define RV_TIMER_def1		3
#define RV_TIMER_def2		4


#define CMD_TIME_OUT_LIMIT		20000//0x7fffffff		//1000
typedef enum _MMD_RV_ROTATEMODE{
	MMD_RV_ROTATE_0,
	MMD_RV_ROTATE_90,
	MMD_RV_ROTATE_180,
	MMD_RV_ROTATE_270,
	MMD_RV_MIRROR_0,
	MMD_RV_MIRROR_90,
	MMD_RV_MIRROR_180,
	MMD_RV_MIRROR_270,
	MMD_RV_ROTATEMODE_ERROR = 0x7fffffff
}MMD_RV_ROTATEMODE;

typedef enum _MMD_RV_TASK_OPER{
	// File system operation
	MMD_RV_FS_INIT=1,
	MMD_RV_FS_RELEASE,

	// File operation
	MMD_RV_FILE_READ,
	MMD_RV_FILE_CLOSE,

	
	MMD_RV_TASK_ERROR = 0x7fffffff
}MMD_RV_TASK_OPER;

typedef struct _MMD_RV_BUF{
	volatile MMD_U32		buffer_address;		/* buffer pointer */
   	volatile MMD_U32 		buffer_size;			/* buffer size*/
	volatile MMD_U32		data_length;			/* bytes in buffer */
      	volatile MMD_U32		cursor_read;		/* read cursor in buffer */
	volatile MMD_U32		cursor_write;		/* write cursor in buffer */
	volatile MMD_U32		is_over;				/* flag to identify buffer data finsh, 0: is not last block data, 1: is last data block */
}MMD_RV_BUF;

typedef struct _MMD_RV_BUF_INFO{
	MMD_RV_BUF			buffer[RV_BUF_NUM];		/* in our system user pingpang buffer of all */
	volatile MMD_U32		buffer_write;	/* current buffer index 0: buffer 0, 1: buffer 1 */
	volatile MMD_U32		buffer_read;		/* current buffer index 0: buffer 0, 1: buffer 1 */
}MMD_RV_BUF_INFO;
typedef void (*MMD_RV_TIMER)(void);
typedef struct _MMD_RV_TIM 
{
	MMD_RV_TIMER	pISR[RV_TIMER_NUM]; 
	MMD_U32		state[RV_TIMER_NUM];
	MMD_U32		count[RV_TIMER_NUM];
	MMD_U32		interval[RV_TIMER_NUM];
}MMD_RV_TIMER_STATE;
extern MMD_RV_BUF_INFO g_RV_Stream_Buf;

//RDK external functions
MMD_U32 MMD_RV_Init(void);
MMD_U32 MMD_RV_Quit(void);
MMD_U32 MMD_RV_File_Open(PMEDIA_FILE *plyfile, MMD_PLYFILE *pfile);
MMD_U32  MMD_RV_File_Pause(MMD_PLYFILE pfile);
MMD_U32  MMD_RV_File_Play(MMD_PLYFILE pfile, PLYCallback Callback);
MMD_U32  MMD_RV_File_Resume(MMD_PLYFILE pfile);
MMD_U32  MMD_RV_File_Stop(MMD_PLYFILE pfile);
MMD_U32  MMD_RV_File_Close(MMD_PLYFILE pfile);
MMD_U32 MMD_RV_File_GetPlayTime(MMD_PLYFILE pfile, MMD_U32 *uPlayTime);
MMD_U32 MMD_RV_File_GetProperty(MMD_PLYFILE pfile, FILE_PROPERTY *property);
MMD_U32 MMD_RV_File_GetTotalTime(MMD_PLYFILE pfile, MMD_U32 *uTotalTime);
MMD_U32 MMD_RV_File_Seek(MMD_PLYFILE pfile, MMD_U32 pos, MMD_U32 *accesspos);


//RDK internal functions
MMD_U32 Mode_RV_Lisr(MMD_U32 cmd);
MMD_U32 RV_Fifo_Init(MMD_U32 size, MMD_U32* addr);
MMD_U32 RV_Fifo_Release(MMD_U32 addr);
MMD_U32 RV_Fifo_Write_Notify(MMD_U32 addr, MMD_U32 len);
MMD_U32 RV_Fifo_Get_Empty(MMD_U32* addr, MMD_U32* len);
MMD_U32 RV_File_Open(MMD_U32 filelen, MMD_U32* handle,MMD_U32 *indexoffset);
MMD_U32 RV_File_Play(MMD_U32 handle);
MMD_U32 RV_File_Seek(MMD_U32 handle, MMD_U32 pos, MMD_U32* realpos);
MMD_U32 RV_File_Resume(MMD_U32 handle);
MMD_U32 RV_File_Pause(MMD_U32 handle);
MMD_U32 RV_File_Stop(MMD_U32 handle);
MMD_U32 RV_File_Close(MMD_U32 handle);
MMD_U32 RV_LCD_Set(MMD_U32 degree, MMD_U32 width, MMD_U32 height);
MMD_U32 RV_Mem_Poll_Create(void);
MMD_U32 RV_Mem_Poll_Delete(void);

#ifdef __cplusplus
}
#endif

#endif /*_MMD_FILE_DRIVER_H_*/
