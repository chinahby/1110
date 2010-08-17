#ifndef GREYBITSYSTEM_H_
#define GREYBITSYSTEM_H_
#include "GreyBitType.h"

#ifdef __cplusplus
extern "C"{
#endif

// Memory Management
typedef void *(*GB_MALLOC)(GB_INT32 size);
typedef void *(*GB_REALLOC)(void *p, GB_INT32 newsize);
typedef void  (*GB_FREE)(void *p);

typedef struct _GB_MemoryRec{
    GB_MALLOC  malloc;
    GB_REALLOC realloc;
    GB_FREE    free;
}GB_MemoryRec,*GB_Memory;

extern GB_Memory    GreyBit_Memory_New(void);
extern void *       GreyBit_Malloc(GB_Memory mem, GB_INT32 size);
extern void *       GreyBit_Realloc(GB_Memory mem, void *p, GB_INT32 newsize);
extern void         GreyBit_Free(GB_Memory mem, void *p);
extern void         GreyBit_Memory_Done(GB_Memory mem);  

// Stream IO
typedef void *GB_IOHandler;

typedef GB_INT32    (*GB_READ)(GB_IOHandler f, GB_BYTE *p, GB_INT32 size);
typedef GB_INT32    (*GB_WRITE)(GB_IOHandler f, GB_BYTE *p, GB_INT32 size);
typedef GB_INT32    (*GB_SEEK)(GB_IOHandler f, GB_INT32 pos);
typedef void        (*GB_CLOSE)(GB_IOHandler f);

typedef struct _GB_StreamRec{
    GB_IOHandler handler;
    GB_READ      read;
    GB_WRITE     write;
    GB_SEEK      seek;
    GB_CLOSE     close;
    GB_INT32     size;
    char        *pfilename;
}GB_StreamRec,*GB_Stream;

extern GB_Stream    GreyBit_Stream_New(const char* filepathname, char bcreate);
extern GB_Stream    GreyBit_Stream_New_Memory(const void *pBuf, GB_INT32 nBufSize);
extern GB_INT32     GreyBit_Stream_Read(GB_Stream stream, GB_BYTE *p, GB_INT32 size);
extern GB_INT32     GreyBit_Stream_Write(GB_Stream stream, GB_BYTE *p, GB_INT32 size);
extern GB_INT32     GreyBit_Stream_Seek(GB_Stream stream, GB_INT32 pos);
extern void         GreyBit_Stream_Done(GB_Stream stream);

// StdLib
#define GB_MEMCMP(b1,b2,n)  GreyBit_Memcmp_Sys(b1,b2,n)
#define GB_MEMCPY(d,s,n)    GreyBit_Memcpy_Sys(d,s,n)
#define GB_MEMSET(d,i,n)    GreyBit_Memset_Sys(d,i,n)
#define GB_STRCAT(d,s)      GreyBit_Strcat_Sys(d,s)
#define GB_STRCMP(s1,s2)    GreyBit_Strcmp_Sys(s1,s2)
#define GB_STRCPY(d,s)      GreyBit_Strcpy_Sys(d,s)
#define GB_STRLEN(s)        GreyBit_Strlen_Sys(s)
#define GB_STRNCMP(s1,s2,n) GreyBit_Strncmp_Sys(s1,s2,n)
#define GB_STRNCPY(s1,s2,n) GreyBit_Strncpy_Sys(s1,s2,n)
#define GB_STRCHR(s,c)      GreyBit_Strchr_Sys(s,c)
#define GB_STRRCHR(s,c)     GreyBit_Strrchr_Sys(s,c)
#define GB_STRSTR(s1,s2)    GreyBit_Strstr_Sys(s1,s2)
#define GB_ATOL(s)          GreyBit_Atol_Sys(s)
#define GB_LABS(i)          GreyBit_Labs_Sys(i)

extern int      GreyBit_Memcmp_Sys(const void *b1, const void *b2, GB_UINT32 n);
extern void *   GreyBit_Memcpy_Sys(void *d, const void *s, GB_UINT32 n);
extern void *   GreyBit_Memset_Sys(void *s, int i, GB_UINT32 n);
extern char *   GreyBit_Strcat_Sys(char *d, const char *s);
extern int      GreyBit_Strcmp_Sys(const char *s1, const char * s2);
extern char *   GreyBit_Strcpy_Sys(char *d,const char *s);
extern int      GreyBit_Strlen_Sys(const char *s);
extern int      GreyBit_Strncmp_Sys(const char *s1, const char *s2, GB_UINT32 n);
extern char *   GreyBit_Strncpy_Sys(char *d, const char *s, GB_INT32 n);
extern char *   GreyBit_Strchr_Sys(const char *s, char c);
extern char *   GreyBit_Strrchr_Sys(const char *s, char c);
extern char *   GreyBit_Strstr_Sys(const char *s1, const char *s2);
extern GB_INT32 GreyBit_Atol_Sys(const char *s);
extern GB_INT32 GreyBit_Labs_Sys(GB_INT32 i);
#ifdef __cplusplus
}
#endif 
#endif //GREYBITSYSTEM_H_
