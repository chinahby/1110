/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_common_debug.h
 * [Description]          : Debug macro definition
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-03-13 16:01
 * [Platform]             : 
 * [Note]                 : 
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-03-13  zhangxf    Created
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
#ifndef _MMD_COMMON_DEBUG_H_
#define _MMD_COMMON_DEBUG_H_


#define MMD_PRINTFE(string)	{						\
		MMDPrintf ("[ERROR] %s(%d)\n",__FILE__, __LINE__);	\
		MMDPrintf ##string;							\
	}
#define MMD_PRINTFW(string)	{							\
		MMDPrintf ("[WARNING] %s(%d)\n",__FILE__, __LINE__);	\
		MMDPrintf ##string;								\
	}
#define MMD_PRINTFN(string)	{						\
		MMDPrintf ("[NOTE]");	\
		MMDPrintf ##string;							\
	}
#define MMD_PRINTFS(string)	{							\
		MMDPrintf ("[SUCCESS] %s(%d)\n",__FILE__, __LINE__); 	\
		MMDPrintf ##string;								\
	}
#define MMD_PRINTF(string)	{	\
		MMDPrintf ##string;		\
	}
#define MMD_ASSERT(x) {	\
	if(!(x)) {MMD_PRINTF(("ASSERT %s(%d)\n",__FILE__,__LINE__));		\
	while(1);}																\
	}
#define MMD_TRACE(_X_)	{ if(!(_X_)) \
	MMDPrintf("\nTrace at %s, line=%d \n",__FILE__, __LINE__);}



#endif /*_MMD_COMMON_DEBUG_H_*/

