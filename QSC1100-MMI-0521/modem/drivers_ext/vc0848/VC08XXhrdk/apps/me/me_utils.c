/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_utils.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_utils.h"

#ifdef _WIN32
ME_PVOID me_malloc(ME_U32 size)
{
	void* memblock;
	
	memblock = (void*)malloc(size);
	ME_MALLOC_MSG(("address=0x%x,size=%d", (unsigned int)memblock,size));
	
	return memblock;
}
ME_PVOID	me_calloc(ME_U32 num, ME_U32 size)
{
	return (ME_PVOID)malloc(num*size);
}
// by dnn 2008/4/11
//ME_PVOID me_realloc(ME_PVOID pold, ME_U32 newsize)	/* add by yangl, 2007 11 21 */
//{
//	return realloc(pold, newsize);
//}
ME_VOID	me_free(ME_PVOID buf)
{
	if(buf)
	{
		ME_FREE_MSG(("address=0x%x", (unsigned int)buf));
		free((void *)buf );
	}

}
#else
ME_PVOID me_malloc(ME_U32 size)
{
	void* memblock;
	
	memblock = (void*)ME_MALLOC(size);
	ME_MALLOC_MSG(("address=0x%x,size=%d", (unsigned int)memblock,size));
	
	return memblock;
}
ME_PVOID	me_calloc(ME_U32 num, ME_U32 size)
{
	return (ME_PVOID)ME_MALLOC(num*size);
}
//by dnn 2008/4/11
//ME_PVOID me_realloc(ME_PVOID pold, ME_U32 newsize)	/* add by yangl, 2007 11 21 */
//{
//	return (ME_PVOID)SD_Realloc(pold, newsize);
//}
ME_VOID	me_free(ME_PVOID buf)
{
	if(buf)
	{
		ME_FREE_MSG(("address=0x%x", (unsigned int)buf));
		ME_FREE((void *)buf );
	}

}
#endif

ME_PS08	me_memcpy(ME_PS08 str_dst, ME_PCS08	str_src, ME_U32 len)
{
	return memcpy(str_dst, str_src, len);
}
ME_U32	me_strlen(ME_PU08 str)
{
	return (ME_U32)strlen((char*)str);
}
ME_VOID	me_memset(ME_PU08 buf, ME_U08 val, ME_U32 size)
{
	memset(buf, val, size);
}
ME_U32	me_abs(ME_U32 value)
{
	return abs(value);
}

ME_S32	me_cmp(ME_PU08 str_dst, ME_PU08 str_src, ME_U32 len)
{
	return memcmp(str_dst, str_src, len);
}

ME_PS08	me_move(ME_PS08 str_dst, ME_PCS08	str_src, ME_U32 len)
{
	return memmove(str_dst, str_src, len);
}

//by dnn 2008/4/11
#if ME_VERSION2_FOURHANDLE
#if 0
void* me_malloc_h2d(unsigned int size)
{
	void* memblock;
	memblock = (void*)malloc(size);
	return memblock;
}

void	me_free_h2d(void*  buf)
{
	if(buf)
	{
		free((unsigned int)buf );
	}
}
ME_U32 me_memcpy_h2d(ME_PS08  str_dst, ME_PCS08 str_src, ME_U32 len)
{
	 memcpy(str_dst, str_src, len);
	 return len;
}

ME_U32 me_memcpy_d2h(ME_PS08  str_dst, ME_PCS08 str_src, ME_U32 len)
{
	memcpy(str_dst, str_src, len);
	return len;
}

ME_U32 me_memset_h2d(ME_U32 addr, ME_U32 size){
	memset((char *)addr,0xff,size);
	return 0;
}
#else
void* me_malloc_h2d(unsigned int size)
{
	void* memblock;
	memblock = (void*)ME_MALLOC_H2D(size);
	return memblock;
}

void	me_free_h2d(void*  buf)
{
	if(buf)
	{
		ME_FREE_H2D((unsigned int)buf );
	}
}
//ME_PS08 
ME_U32 me_memcpy_h2d(ME_PS08  str_dst, ME_PCS08 str_src, ME_U32 len)
{
	//return memcpy(str_dst, str_src, len);
//	return Vm_WrSram((ME_U32)str_dst, (ME_PS08)str_src, len);
//	return V8_Mul16WrSram((ME_U32)str_dst, (ME_PS08)str_src, len);
	return ME_MEMCPY_H2D((ME_U32)str_dst, (ME_PU08)str_src, len);
}

//ME_PS08 
ME_U32 me_memcpy_d2h(ME_PS08  str_dst, ME_PCS08 str_src, ME_U32 len)
{
	//return memcpy(str_dst, str_src, len);
//	return Vm_RdSram((ME_U32)str_src, (ME_PS08)str_dst, len);
//	return V8_Mul16RdSram((ME_U32)str_src, (ME_PS08)str_dst, len);
	return ME_MEMCPY_D2H((ME_U32)str_src, (ME_PS08)str_dst, len);
}

ME_U32 me_memset_h2d(ME_U32 addr, ME_U32 size){
	return MMD_SYS_FillMem(addr,size);
}
#endif
#endif
