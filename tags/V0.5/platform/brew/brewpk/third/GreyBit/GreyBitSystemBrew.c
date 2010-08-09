

//#include "windows.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "AEEShell.h"
#include "GreyBitSystem.h"
#include "GreyBitType.h"
#include "AEEStdLib.h"
#include "AEEFile.h"
#include "AEE_OEM.h"
#include "OEMHeap.h"
#include "AEE_OEMComdef.h"

//-----------------------------------------------------------------------------+
void *GreyBit_Malloc_Sys(GB_INT32 size)
{
    void *p = sys_malloc((dword)size);
    return p;
}
void *GreyBit_Realloc_Sys(void *p, GB_INT32 newsize)
{
    return sys_realloc(p,(uint32)newsize);
}
extern void  GreyBit_Free_Sys(void *p)
{
    sys_free(p);
}

//-----------------------------------------------------------------------------
typedef struct _GB_SysFileRec{
    IFile *fp;
    IFileMgr	*pIFileMgr;
}GB_SysFileRec,*GB_SysFile;

GB_IOHandler GreyBit_Open_Sys(const GB_CHAR *p, GB_BOOL bcreate)
{
    ACONTEXT *pAConText = AEE_EnterAppContext(NULL);
    int result;
    GB_SysFile handle = (GB_SysFile)GreyBit_Malloc_Sys(sizeof(GB_SysFileRec));
    if(handle == NULL)
    {
        goto CLEAN_UP;
    }
    
    result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR, (void**)&(handle->pIFileMgr));
    
	if(SUCCESS != result|| handle->pIFileMgr == NULL)
	{
        GreyBit_Free_Sys(handle);
		handle = 0;
		goto CLEAN_UP;
	}
    if(bcreate)
    {
        handle->fp = IFILEMGR_OpenFile(handle->pIFileMgr, (char*)p, _OFM_READWRITE);
    }
    else
    {
        handle->fp = IFILEMGR_OpenFile(handle->pIFileMgr, (char*)p, _OFM_READ);
    }
    
	if (handle->fp == NULL)
	{
		GreyBit_Free_Sys(handle);
		handle = 0;
		goto CLEAN_UP;
	}
    
CLEAN_UP:
    AEE_LeaveAppContext(pAConText);
    return handle;
} 

GB_INT32 GreyBit_Read_Sys(GB_IOHandler f, GB_BYTE *p, GB_INT32 size)
{
    ACONTEXT *pAConText = AEE_EnterAppContext(NULL);
    GB_INT32 result = 0;
    GB_SysFile handle = (GB_SysFile)f;
    result = (GB_INT32)IFILE_Read(handle->fp, (void*)p, (uint32)size);
    AEE_LeaveAppContext(pAConText);
    return result;
}

GB_INT32 GreyBit_Write_Sys(GB_IOHandler f, GB_BYTE *p, GB_INT32 size)
{
    ACONTEXT *pAConText = AEE_EnterAppContext(NULL);
    GB_INT32 result = 0;
    GB_SysFile handle = (GB_SysFile)f;
    result = (GB_INT32)IFILE_Write( handle->fp, (PACKED const void *)p, (uint32)size );
    AEE_LeaveAppContext(pAConText);
    return result;
}

GB_INT32 GreyBit_Seek_Sys(GB_IOHandler f, GB_INT32 pos)
{
    ACONTEXT *pAConText = AEE_EnterAppContext(NULL);
    GB_INT32 result = 0;
    GB_SysFile handle = (GB_SysFile)f;
    result = (GB_INT32)IFILE_Seek(handle->fp, _SEEK_START,(uint32)pos);
    AEE_LeaveAppContext(pAConText);
    return result;
}

GB_INT32 GreyBit_GetSize_Sys(GB_IOHandler f)
{
    ACONTEXT *pAConText = AEE_EnterAppContext(NULL);
    GB_SysFile handle = (GB_SysFile)f;
    GB_INT32 length;
    FileInfo   fi;
    IFILE_GetInfo(handle->fp, &fi);
    length = (GB_INT32)fi.dwSize;
    AEE_LeaveAppContext(pAConText);
    return length;
}

void GreyBit_Close_Sys(GB_IOHandler f)
{
    ACONTEXT *pAConText = AEE_EnterAppContext(NULL);
    GB_SysFile handle = (GB_SysFile)f;
   	IFILE_Release(handle->fp);
	IFILEMGR_Release(handle->pIFileMgr);
    GreyBit_Free_Sys(handle);
    AEE_LeaveAppContext(pAConText);
}

//-----------------------------------------------------------------------------
int GreyBit_Memcmp_Sys(const void *b1, const void *b2, GB_UINT32 n)
{
    return MEMCMP(b1,b2,(size_t)n);
}

void *GreyBit_Memcpy_Sys(void *d, const void *s, GB_UINT32 n)
{
    return MEMCPY(d,s,(size_t)n);
}

void *GreyBit_Memset_Sys(void *s, int i, GB_UINT32 n)
{
    return MEMSET(s,i,(size_t)n);
}

char *GreyBit_Strcat_Sys(char *d, const char *s)
{
    return STRCAT(d,s);
}

int GreyBit_Strcmp_Sys(const char *s1, const char * s2)
{
    return STRCMP(s1,s2);
}

char *GreyBit_Strcpy_Sys(char *dest, const char *src)
{
    return STRCPY(dest,src);
}

int GreyBit_Strlen_Sys(const char *s)
{
    return STRLEN(s);
}

int GreyBit_Strncmp_Sys(const char *s1, const char *s2, GB_UINT32 n)
{
    return STRNCMP(s1,s2,(size_t)n);
}

char *GreyBit_Strncpy_Sys(char *d, const char *s, GB_INT32 n)
{
    return STRNCPY(d,s,(size_t)n);
}

char *GreyBit_Strchr_Sys(const char *s, char c)
{
    return STRCHR(s,(int)c);
}

char *GreyBit_Strrchr_Sys(const char *s, char c)
{
    return STRRCHR(s,(int)c);
}

char *GreyBit_Strstr_Sys(const char *s1, const char *s2)
{
    return STRSTR(s1, s2);
}

GB_INT32 GreyBit_Atol_Sys(const char *s)
{
    return ATOI(s);
}

GB_INT32 GreyBit_Labs_Sys(GB_INT32 i)
{
    return (i>0?i:-i);
}
