/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_mm.h
 * [Description]          : Provide functionality that relevant to memory management.
 * [Author]               : zhangxf
 * [Date Of Creation]     : 2007-09-17
 * [Platform]             : Platform dependent
 * [Note]                 : None
 *
 * Modification History : 
 * ------------------------------------------------------------------------------
 * Date        Author     Modifications
 * ------------------------------------------------------------------------------
 * 2007-09-17  zhangxf    Created
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
 #ifndef _MMD_MM_H_
 #define _MMD_MM_H_
 
#include "mmp_rdk.h"
#include "mmd_data_type.h"
#define MEM_ALIGNMENT		4

#ifdef MMD_BASEBAND_32BIT


 #ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif

#ifndef MEM_ALIGN
#define MEM_ALIGN(addr) ((void *)(((MMD_U32)(addr) + MEM_ALIGNMENT - 1) & ~(MMD_U32)(MEM_ALIGNMENT-1)))
#endif


#else

 #ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif

#ifndef MEM_ALIGN
#define MEM_ALIGN(addr) ((void *)(((MMD_U16)(addr) + MEM_ALIGNMENT - 1) & ~(MMD_U16)(MEM_ALIGNMENT-1)))
#endif


#endif //MMD_BASEBAND_32BIT

void MMD_MM_Initialize(void);
void *MMD_MM_Malloc(MMD_U32 size);
void MMD_MM_Free(void * ptr);

#endif // _MMD_MM_H_