/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2005                                                     
 *--------------------------------------------------------------------------------------------------------------
 * [File Name]            : Debug.h
 * [Description]          : 定义整个工程的调试信息输出开关
 * [Author]               : Lizg	
 * [Date Of Creation]     : 2007-2-7 17:22
 * [Platform]             : 平台无关
 * [Note]                 : 
 *
 * Modification History : 
 *------------------------------------------------------------------------------
 * Date        Author     Modifications
 *------------------------------------------------------------------------------
 * 2007-03-14  Lizg       Created
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
#ifndef _VIM_DRV_DEBUG_H_
#define _VIM_DRV_DEBUG_H_
#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* 调试开关定义
* 
 *******************************************************************************/
#define DRV_ASSERT_ENABLE		0  // 1: Enable , 0: Disable

#if 0
#define DRV_MSG_IRQ_ON 
#define DRV_ERR_IRQ_ON 
#define DRV_MSG_DMA_ON 
#define DRV_ERR_DMA_ON 
#define DRV_DBG_BIU_ON
#define DRV_MSG_BIU_ON
#define DRV_ERR_BIU_ON
#define DRV_DBG_FIL_ON
#define DRV_MSG_FIL_ON
#define DRV_ERR_FIL_ON
#define DRV_DBG_USB_ON
#define DRV_MSG_USB_ON
#define DRV_ERR_USB_ON
/*******************************************************************************
* 每个模块使用单独的打印函数，调试信息，一般输出信息
* 和错误信息使用不同的函数，有独立的开关控制输出
 *******************************************************************************/
#ifdef DRV_DBG_BIU_ON
#define BIU_DBG(str)		VIM_UART_Printf str
#else
#define BIU_DBG(str)		
#endif
#ifdef DRV_MSG_BIU_ON
#define BIU_MSG(str)		VIM_UART_Printf str 
#else
#define BIU_MSG(str)		
#endif
#ifdef DRV_ERR_BIU_ON 
#define BIU_ERR(str)		VIM_UART_Printf str
#else
#define BIU_ERR(str)		
#endif
#ifdef DRV_DBG_FIL_ON
#define FIL_DBG(str)		VIM_UART_Printf str
#else
#define FIL_DBG(str)		
#endif
#ifdef DRV_MSG_FIL_ON
#define FIL_MSG(str)		VIM_UART_Printf str
#else
#define FIL_MSG(str)		
#endif
#ifdef DRV_ERR_FIL_ON 
#define FIL_ERR(str)		VIM_UART_Printf str
#else
#define FIL_ERR(str)		
#endif

#ifdef DRV_DBG_USB_ON
#define USB_DBG(str)		VIM_UART_Printf str
#else
#define USB_DBG(str)		
#endif
#ifdef DRV_MSG_USB_ON
#define USB_MSG(str)		VIM_UART_Printf str
#else
#define USB_MSG(str)		
#endif
#ifdef DRV_ERR_USB_ON 
#define USB_ERR(str)		VIM_UART_Printf str
#else
#define USB_ERR(str)		
#endif
#endif
#if 0//DRV_ASSERT_ENABLE

#define DRV_ASSERT(_X_)	{ if(!(_X_)) \
	{VIM_UART_Printf("\nAssert error at %s, line=%d \nfunc = %s\n",__FILE__, __LINE__,__func__);\
	while(1);}}
#define DRV_ASSERT_BREAK(_X_)	{ if(!(_X_)) \
	{VIM_UART_Printf("\nAssert error at %s, line=%d \nfunc = %s\n",__FILE__, __LINE__,__func__);\
	break;}}
#define DRV_ASSERT_RETURN(_X_)	{ if(!(_X_)) \
	{VIM_UART_Printf("\nAssert error at %s, line=%d \nfunc = %s\n",__FILE__, __LINE__,__func__);\
	return;}}
#define DRV_WARNING(_X_)	{ if(!(_X_)) \
	VIM_UART_Printf("\nWarning at %s, line=%d \nfunc = %s\n",__FILE__, __LINE__,__func__);}

#else

#define DRV_ASSERT(_X_) 
#define DRV_ASSERT_BREAK(_X_) 	
#define DRV_ASSERT_RETURN(_X_)
#define DRV_WARNING(_X_)

#endif

#ifdef __cplusplus
	}
#endif

#endif  //_VIM_DRV_DEBUG_H_
