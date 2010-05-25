/*======================================================================
*                                                                       
*                Copyright (C) 2007 Vimicro CO.,LTD     		 
* File Name: 	media_utils.c
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2007/04/02
* Last modify : Amanda Deng 2007-xx-xx     
*
* version 0.1: 
*	have 12 interface API for user .
*	void	 media_engine_initialize(void);
*	void  media_engine_release(void);
*	
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/
#include "media_utils.h"		
#include	"mmd_user_depend.h"
#include "msg.h"
#ifdef _WIN32
void* mp_malloc(unsigned int size)
{
	void* memblock;
	memblock = (void*)malloc(size);
	mp_malloc_print(("address=0x%x,size=%d", (unsigned int)memblock,size));
	return memblock;
}

void* mp_ncmalloc(unsigned int size)
{
	void* memblock;
	memblock = (void*)malloc(size);
	mp_malloc_print(("address=0x%x,size=%d", (unsigned int)memblock,size));
	return memblock;
}

void* mp_calloc(unsigned int num, unsigned int size)
{
	void* memblock;
	memblock = (void*)malloc(num*size);
	mp_malloc_print(("address=0x%x,size=%d", (unsigned int)memblock,num*size));
	return memblock;

}

void	mp_free(void*  buf)
{
	if(buf)
	{
		mp_free_print(("address=0x%x", (unsigned int)buf));
		free((void *)buf );
	}
}

void	mp_ncfree(void*  buf)
{
	if(buf)
	{
		mp_free_print(("address=0x%x", (unsigned int)buf));
		free((void *)buf );
	}
}

char* mp_memcpy(char*  str_dst, const char* str_src, unsigned int len)
{
	return memcpy(str_dst, str_src, len);
}


void mp_memset(unsigned char* buf, unsigned char val, unsigned int size)
{
	memset(buf, val, size);
}

unsigned int mp_strlen(MP_PU08 str)
{
	return (unsigned int)strlen((char*)str);
}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
int filesys_fileread(void* file, char* buffer, int count)
{
#if MEDIA_VIM_MEDIA_PLAYER_FILE_DEBUG
	int result;
	result = fread((void*)buffer, sizeof(char), (long)count, (FILE *)file);
	mp_print(("filesys_fileread() return = %d ", result));
	return result;
#else
	return fread((void*)buffer, sizeof(char), (long)count, (FILE *)file);
#endif
}

int filesys_filewrite(void* file, char* buffer, int count)
{
#if MEDIA_VIM_MEDIA_PLAYER_FILE_DEBUG
	int result;
	result = fwrite((const void*)buffer, sizeof(char), (long)count, (FILE*)file);
	mp_print(("filesys_filewrite() return = %d ", result));
	return result;
#else
	return fwrite((const void*)buffer, sizeof(char), (long)count, (FILE*)file);
#endif
}

int filesys_fileseek(void* file, int offset, int mode)
{
#if MEDIA_VIM_MEDIA_PLAYER_FILE_DEBUG
	int result;
	result = fseek((FILE*)file, offset, mode);
	mp_print(("filesys_fileseek() return = %d ", result));
	return result;
#else
	return fseek((FILE*)file, offset, mode);
#endif
}

int filesys_filetell(void* file)
{
#if MEDIA_VIM_MEDIA_PLAYER_FILE_DEBUG
	int result;
	result = ftell((FILE*)file);
	mp_print(("filesys_filetell() return = %d ", result));
	return result;
#else
	return ftell((FILE*)file);
#endif
}

void*filesys_fileopen(void *filename, void *mode)
{
	FILE *file = fopen((const char *)filename, (const char *)mode);
	return (void*)file;
}

int filesys_fileclose(void* file)
{
	return fclose((FILE*)file);
}

int filesys_filedelete(void *filename)
{
	return 0;//f_delete(filename);
}
#else
/*extern void* 	SD_Malloc(MP_U32 size);
extern void 	SD_Free(void* pBuf);*/


extern void* 	SDNC_Malloc(MP_U32 size);
extern void 	SDNC_Free(void* pBuf);

void* mp_malloc(unsigned int size)
{
	void* memblock;
	memblock = (void*)MMD_Malloc(size);
	mp_malloc_print(("address=0x%x,size=%d", (unsigned int)memblock,size));
	return memblock;
}

void* mp_calloc(unsigned int num, unsigned int size)
{
	void* memblock;
	memblock = (void*)MMD_Malloc(num*size);
	mp_malloc_print(("address=0x%x,size=%d", (unsigned int)memblock,num*size));
	return memblock;

}

void	mp_free(void*  buf)
{
	if(buf)
	{
		mp_free_print(("address=0x%x", (unsigned int)buf));
		MMD_Free((void *)buf );
	}
}

char* mp_memcpy(char*  str_dst, const char* str_src, unsigned int len)
{
	return memcpy(str_dst, str_src, len);
}


void mp_memset(unsigned char* buf, unsigned char val, unsigned int size)
{
	memset(buf, val, size);
}

unsigned int mp_strlen(MP_PU08 str)
{
	return (unsigned int)strlen((char*)str);
}

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
				file system function define for media use Amanda Deng
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
int filesys_fileread(void* file, char* buffer, int count)
{
	MP_U32 ret=0;
	MP_U32 readlen=0;
	
	ret = MMD_FILE_Read((F_FILE *)file,(MP_U08*)buffer,(MP_U32)count,&readlen);
	if(ret) {
		mp_error(("MMD_FILE_Read=0x%08x",ret));
	}
	return (MP_S32)readlen;
}

int filesys_filewrite(void* file, char* buffer, int count)
{
	//return 0;
	MP_U32 ret=0;
	MP_U32 writelen=0;
	
	ret = MMD_FILE_Write((F_FILE *)file,(MP_U08*)buffer,(MP_U32)count,&writelen);
	if(ret) {
		mp_error(("MMD_FILE_Read=0x%08x",ret));
	}
	return (MP_S32)writelen;
}

int filesys_fileseek(void* file, int offset, int mode)
{
	MP_U32 ret=0;
	ret = MMD_FILE_Seek((F_FILE *)file,offset,(MP_U08)mode);
	if(ret) {
		mp_error(("MMD_FILE_Seek=0x%08x",ret));
	}
	return (MP_S32)ret;
}

int filesys_filetell(void* file)
{
	MP_U32 pos=0;
	MMD_FILE_Tell((F_FILE *)file,&pos);
	return (MP_S32)pos;
}

int filesys_fileeof(void* file)
{
	return 0;
}

void*filesys_fileopen(void *filename, void *mode)
{
	MP_U32 ret=0;
	F_FILE * pfile=NULL;
	ret = MMD_FILE_Open((MP_U08 *)filename, (MP_U08*)mode,&pfile);
	if(ret) {
		MSG_FATAL("ret =%d",ret, 0, 0);
		mp_error(("MMD_FILE_Open=0x%08x",ret));
	}
	MSG_FATAL("%d", pfile,0, 0);
	return (void *)pfile;
}

int filesys_fileclose(void* file)
{	
	MP_U32 ret=0;
	ret = MMD_FILE_Close((F_FILE *)file);
	if(ret) {
		mp_error(("MMD_FILE_Close=0x%08x",ret));
	}
	return (int)ret;
}

int filesys_filedelete(void *filename)
{
	MP_U32 ret=0;
	ret = MMD_FILE_Delete((MP_U08 *)filename);
	if(ret) {
		mp_error(("MMD_FILE_Close=0x%08x",ret));
	}
	return (int)ret;
	//return 0;
}
#endif

#if ME_VERSION2_FOURHANDLE
int filesys_get_sector_offset(void* file, unsigned int * sectorsize)
{	
	MP_U32 pos=0;
	MP_U32 offset=0;
	MMD_FILE_Tell((F_FILE *)file,&pos);
	*sectorsize = 512;
	offset = pos%512;
	return (int)offset;
}

int filesys_block_fileread(void* file, char* buffer, int count, unsigned int reg, unsigned int bit)
{
	ME_U32 ret=0;
	ME_U32 readlen=0;
	//ME_U32 regdata=0;
	F_COREMEM corebuf;
	corebuf.addr = (ME_U32)buffer;
	corebuf.reg = (ME_U32)reg;
	corebuf.bit = (ME_U32)bit;
	/* clear specified bit in register */
//	HAL_READ_UINT32(reg, regdata);
//	if((regdata&(1<<bit))) {	//not fit for audio buffer status
//		ME_DEBUG(("buffer now is already used to read, it cannot be readed more!",ret));
//		return 0;
//	}
//	regdata = regdata | (1<<bit);
//	HAL_WRITE_UINT32(reg, regdata);//clear to 0, sign reading
					
	ret = MMD_FILE_Read2Core((F_FILE *)file,corebuf,(ME_U32)count,&readlen);
	if(ret) {
		ME_ERROR(("MMD_FILE_Read2Core=0x%08x",ret));
	}
	return (int)readlen;
}

int filesys_block_filewrite(void* file, char* buffer, int count,unsigned int reg, unsigned int bit)
{
	ME_U32 ret=0;
	ME_U32 writelen=0;
	//ME_U32 regdata=0;
	F_COREMEM corebuf;
	
	corebuf.addr = (ME_U32)buffer;
	corebuf.reg = (ME_U32)reg;
	corebuf.bit = (ME_U32)bit;
	
	/* clear specified bit in register */
	//MMD_BIU_Set_Device_Status(reg, 0);				
	ret = MMD_FILE_WriteFrCore((F_FILE *)file,corebuf,(ME_U32)count,&writelen);
	if(ret) 
	{
		ME_ERROR(("MMD_FILE_Write2Core=0x%08x",ret));
	}
	/*MMD_BIU_Get_Device_Status(reg, &RegBit);
	ret = 200;
	while(!(RegBit & 1<<bit) &&ret)
	{
		mp_printf("RegBit(0x%x) \n",RegBit);
		MMD_BIU_Get_Device_Status(reg, &RegBit);
		MMD_USER_WaitMSEC(10);
		ret--;
	}
	MMD_FILE_StartWriteFrCore();*/
	return (int)writelen;

}

void * filesys_duplicate_handle(void *file)
{
	ME_U32 ret=0;	
	F_FILE * newfile=NULL;
	ret = MMD_FILE_DupHandle((F_FILE *)file,&newfile);
	if(ret) {
		ME_ERROR(("MMD_FILE_DupHandle=0x%08x",ret));
	}
	return (void *)newfile;
}
#endif

#if 0
int memory_fileread(void* file, char* buffer, int count)
{
	MEMORY_FILE* 	pFile =(MEMORY_FILE* )file;
	unsigned int 		cnt = 0;
	
	if(pFile->FileReadCur>=pFile->FileTotalLength)
		return 0;

	cnt = count>(pFile->FileTotalLength-pFile->FileReadCur)?(pFile->FileTotalLength-pFile->FileReadCur):count;
	mp_memcpy((char*)buffer, (const char*)pFile->FileAddress+pFile->FileReadCur,cnt);
	pFile->FileReadCur+=cnt;
	return cnt;
}

int memory_filewrite(void* file, char* buffer, int count)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;

	mp_memcpy((char*)pFile->FileAddress+pFile->FileWriteCur,(const char*)buffer, count);
	pFile->FileWriteCur+=count;
	pFile->FileTotalLength+=count;
	return count;
}

int memory_fileseek(void* file, int offset, int mode)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;

	switch(mode)
	{
	case SEEK_SET:
		pFile->FileReadCur = offset;
		break;
	case SEEK_CUR:
		pFile->FileReadCur += offset;
		break;
	case SEEK_END:
		pFile->FileReadCur = pFile->FileTotalLength+offset;
		break;
	default:
		break;
	}
	if(pFile->FileReadCur > pFile->FileTotalLength)
		return pFile->FileReadCur;
	else
		return 0;
}

int memory_filetell(void* file)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	return pFile->FileReadCur;
}

int memory_fileeof(void* file)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	if(pFile->FileReadCur >= pFile->FileTotalLength)
		return pFile->FileReadCur;
	else
		return 0;
}

void*memory_fileopen(void *address, void *size)
{
	MEMORY_FILE* file=NULL;
	int *filelength = (int *)size;

	file=(MEMORY_FILE*)mp_malloc(sizeof(MEMORY_FILE));
	if(!file)
	{
		ME_ERROR(("MP_ERR_MALLOC_MEMORY_ERROR"));
		return NULL;
	}
	file->FileAddress 		= (char*)address;
	file->FileTotalLength 	= (int)*filelength;
	file->FileReadCur 		= file->FileWriteCur = 0;
	return (void*)file;
}

int memory_fileclose(void* file)
{
	mp_free((void*)file);
	return 0;
}

int memory_filedelete(void *address)
{
	return 0;
}

//by dnn 2008/4/11
#if ME_VERSION2_FOURHANDLE
int memory_block_fileread(void* file, char* buffer, int count, unsigned int reg, unsigned int bit)
{
	MEMORY_FILE* 	pFile =(MEMORY_FILE* )file;
	unsigned int 		cnt = 0;
	unsigned int  realstart=0;
	unsigned int  offset=0;
	unsigned int  reallen=0;
	unsigned int  result=0;
	
	if(pFile->FileReadCur>=pFile->FileTotalLength)
		return 0;
	realstart = (unsigned int)(pFile->FileAddress+pFile->FileReadCur);
	offset = realstart%4;
	if(offset!=0) {
		if(offset>pFile->FileReadCur) {
			return 0;
		} else {
			realstart -= offset; // 4x
		}
	}
	reallen = count+offset;
	if(reallen%4!=0) {
		cnt = 4-(reallen%4);
	}else{
		cnt = 0;
	}
	if(count+cnt<=(pFile->FileTotalLength-pFile->FileReadCur)) {
		reallen += cnt;
		result = ME_MEMCPY_H2D((unsigned int)buffer, (unsigned char*)(realstart),reallen);
		if(result<=offset){
			return 0;
		} else if(result>offset && result<offset+count) {
			pFile->FileReadCur += result-offset;
			return result-offset;
		} else if(result>=offset+count) {
			pFile->FileReadCur +=count;
			return count;
		}
	} else {
		reallen = offset+pFile->FileTotalLength-pFile->FileReadCur;
		result = ME_MEMCPY_H2D((unsigned int)buffer, (unsigned char*)(realstart),reallen);
		if(result<=offset){
			return 0;
		} else if(result>offset && result<offset+count) {
			pFile->FileReadCur += result-offset;
			return result-offset;
		} else if(result>=offset+count) {
			pFile->FileReadCur +=count;
			return count;
		}
	}	
}

int memory_block_filewrite(void* file, char* buffer, int count, unsigned int reg, unsigned int bit)
{
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;

//	mp_memcpy_d2h((char*)pFile->FileAddress+pFile->FileWriteCur,(const char*)buffer, count);
//	Vm_RdSram((unsigned int)buffer, (char*)(pFile->FileAddress+pFile->FileWriteCur), count);
	mp_memcpy_d2h(buffer, (char*)(pFile->FileAddress+pFile->FileWriteCur), count);
	pFile->FileWriteCur+=count;
	pFile->FileTotalLength+=count;
	return count;
}

int memory_get_sector_offset(void* file, unsigned int * sectorsize)
{	
	MEMORY_FILE* pFile =(MEMORY_FILE*)file;
	*sectorsize = 4;
	return pFile->FileReadCur%4;
}

void * memory_duplicate_handle(void *file){
	MEMORY_FILE* pFile2 = me_malloc(sizeof(MEMORY_FILE));
	if(pFile2==NULL){
		return NULL;
	}
	mp_memcpy((char *)pFile2,(char *)file,sizeof(MEMORY_FILE));
	return (void *)pFile2;
}
#endif
#endif // if 0
// by dnn 2008/4/11
#if !(ME_VERSION2_FOURHANDLE)
fs_callbacks	 g_filesys_file	= {filesys_filedelete, filesys_fileopen, filesys_fileclose,filesys_fileread, filesys_filewrite,filesys_fileseek,filesys_filetell,filesys_fileeof};
fs_callbacks	 g_memory_file	= {memory_filedelete, memory_fileopen, memory_fileclose,memory_fileread, memory_filewrite,memory_fileseek,memory_filetell,memory_fileeof};
#else
fs_callbacks	 g_filesys_file	= {filesys_filedelete, filesys_fileopen, filesys_fileclose,filesys_fileread, filesys_filewrite,filesys_fileseek,filesys_filetell,filesys_fileeof,filesys_block_fileread,filesys_block_filewrite,filesys_get_sector_offset,filesys_duplicate_handle};
fs_callbacks	 g_memory_file	= {memory_filedelete, memory_fileopen, memory_fileclose,memory_fileread, memory_filewrite,memory_fileseek,memory_filetell,memory_fileeof,NULL,NULL,NULL,NULL};
/*fs_callbacks	 g_memory_baseband_file	= {memory_filedelete, memory_fileopen, memory_fileclose,memory_fileread, memory_filewrite,memory_fileseek,memory_filetell,memory_fileeof,
										memory_block_fileread,memory_block_filewrite,memory_get_sector_offset,memory_duplicate_handle};*/
fs_callbacks	 g_TCL_RAM_file	= {TCL_RAM_filedelete, TCL_RAM_fileopen, TCL_RAM_fileclose,TCL_RAM_fileread, TCL_RAM_filewrite,TCL_RAM_fileseek,TCL_RAM_filetell,TCL_RAM_fileeof,
										NULL,NULL,NULL,NULL};

#ifdef RV_ENABLE
fs_callbacks	 g_filesys_baseband_file	= {NULL, MMD_USER_FS_File_Open, MMD_USER_FS_File_Close,MMD_USER_FS_File_Read, NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
#endif
#endif

/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
							function define for other(2007.8.7 )
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
void mp_writetofifo(MEDIA_FIFO*  dst, const char* src, unsigned int len)
{
	unsigned int address;
	unsigned int fifospace, cnt, writelen = len;
	
	if(dst->pread<=dst->pwrite)
	{
		fifospace = dst->size - dst->pwrite + dst->pread;
		if(fifospace<len)
		{
			writelen = fifospace;
			mp_warning(("video duration fifo overflow !!!"));
		}
		address = (unsigned int)dst->address + dst->pwrite;
		cnt = dst->size - dst->pwrite;
		if(cnt>=writelen)
		{
			mp_memcpy((char*)address, src,  writelen);
		}
		else
		{
			mp_memcpy((char*)address, src,  cnt);
			address = (unsigned int)src + cnt;
			mp_memcpy((char*)dst->address, src,  writelen-cnt);
			
		}
	}
	else
	{
		fifospace = dst->pread-dst->pwrite;
		address = (unsigned int)dst->address + dst->pwrite;
		if(fifospace<len)
		{
			writelen = fifospace;
			mp_warning(("video duration fifo overflow !!!"));
		}
		mp_memcpy((char*)address, src,  writelen);
	}

}

int property_type_is(char *property_type, char *type)
{
	MP_U32 i, typelength, result = MP_TRUE;
	
	typelength = mp_strlen((MP_PU08)type);
	if(typelength>8)
		typelength=8;

	for(i=0; i<typelength; i++)
		if(property_type[i] != type[i])
		{
			result = MP_FALSE;
			break;
		}
	return result;
}

MP_U32 media_get_systime_ticks_10ms(void)
{
	return MMD_SYS_GetTimer2Value();
	//return NU_Retrieve_Clock();	//10ms
}

/////////////////////////////////////////////////////////////
/*	       	memory interfaces added by dnn 2008/4/11		     */
/////////////////////////////////////////////////////////////
#if ME_VERSION2_FOURHANDLE
#ifdef _WIN32
void* mp_malloc_h2d(unsigned int size)
{
	void* memblock;
	memblock = (void*)malloc(size);
	mp_malloc_print(("address=0x%x,size=%d", (unsigned int)memblock,size));
	return memblock;
}

void	mp_free_h2d(void*  buf)
{
	if(buf)
	{
		mp_free_print(("address=0x%x", (unsigned int)buf));
		free((void *)buf );
	}
}

MP_U32 mp_memcpy_h2d(MP_PS08 str_dst, MP_PCS08 str_src, MP_U32 len)
{
	//return memcpy(str_dst, str_src, len);
	if(((MP_U32)str_dst%4) || ((MP_U32)str_src%4) || (len%4))
		return 0;
	memcpy(str_dst, str_src, len);
	return len;
}

MP_U32 mp_memcpy_d2h(MP_PS08 str_dst, MP_PCS08 str_src, MP_U32 len)
{
	//return memcpy(str_dst, str_src, len);
	if(((MP_U32)str_dst%4) || ((MP_U32)str_src%4) || (len%4))
		return 0;
	memcpy(str_dst, str_src, len);
	return len;
}

MP_U32 mp_memset_h2d(char* buf, char val, unsigned int size)
{
	memset(buf, val, size);
	return size;
}
#else
void* mp_malloc_h2d(MP_U32 size)
{
	void* memblock;
	memblock = (void*)MMD_SYS_Malloc(size);
	mp_malloc_h2d_print(("address=0x%x,size=%d", (unsigned int)memblock,size));
	return memblock;
}

void	mp_free_h2d(void*  buf)
{
	if(buf)
	{
		mp_free_h2d_print(("address=0x%x", (unsigned int)buf));
		MMD_SYS_Free((MP_U32)buf );
	}
}

MP_U32 mp_memcpy_h2d(MP_PS08 str_dst, MP_PCS08 str_src, MP_U32 len)
{
	//return memcpy(str_dst, str_src, len);
	if(((MP_U32)str_dst%4) || ((MP_U32)str_src%4) || (len%4))
		return 0;
	return VIM_HAPI_WriteVideoRawData((MP_U32)str_dst, (MP_PU08)str_src, len);
}

MP_U32 mp_memcpy_d2h(MP_PS08 str_dst, MP_PCS08 str_src, MP_U32 len)
{
	//return memcpy(str_dst, str_src, len);
	if(((MP_U32)str_dst%4) || ((MP_U32)str_src%4) || (len%4))
		return 0;
	return VIM_HAPI_ReadVideoRawData((MP_U32)str_src, (MP_PU08)str_dst, len);
	//return Vm_RdSram((MP_U32)str_src, (MP_PS08)str_dst, len);
}

MP_U32 mp_memset_h2d(char* buf, char val, unsigned int size)
{
	//memset(buf, val, size);
	MP_U32 result=0;
	void * tmpbuf = (void *)MMD_Malloc(size);
	if(tmpbuf) {
		memset((char *)tmpbuf, val, size);
		result = mp_memcpy_h2d(buf,tmpbuf,size);
		MMD_Free(tmpbuf);
		tmpbuf = NULL;
		return result;
	} else {
		return 0;
	}
}
#endif
#endif
