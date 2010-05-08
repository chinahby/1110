#ifndef __IOVEC_H__
#define __IOVEC_H__

//#define UXIOVEC_DEBUG_PRINT

#define DEFAULT_IOVEC_TABLE_SIZE 8

#define IOVEC_PAGE_SIZE 4096
	
typedef unsigned char uint8;
typedef unsigned long uint32;

typedef enum UxBufPool_
{
  BUF_POOL_LOCAL,             
  BUF_POOL_SHMEM,
  UxBufPool_SIZER = 0x0FFFFFFF  

} UxBufPool;

typedef struct {
    void   *pvBase;             // Starting address
    uint32  dwLen;              // Length in bytes
}UxIOVEC;

typedef struct tUxIOVEC_T {
  UxIOVEC *iov;                  // pointer to iovec table
  uint32 size;                   // num of iovecs, size of iovec table
  uint32 max_size;               // max num of iovecs that can be allocated
  uint32 tot_byte_cnt;           // total byte count
  uint32 offset;                 // starting data byte
  uint32 m_refcnt;               // reference count
  struct tUxIOVEC_T *m_refptr;           // pointer to the original IOVEC table referenced
  UxBufPool pooltype;
}UxIOVEC_T;


uint32 uxiovect_size(UxIOVEC_T *iovt);
uint32 uxiovect_length(UxIOVEC_T *iovt);

int uxiovect_extract(UxIOVEC_T *iovt, uint32 offset, uint8 *dest, uint32 num, uint32 *bytes_read);
int uxiovect_write(UxIOVEC_T *iovt, uint32 offset, uint8* src, uint32 num, uint32 *bytes_written);

int uxiovect_assign(UxIOVEC_T *iovt, uint8 *src, uint32 num);
int uxiovect_assign_iovect(UxIOVEC_T *iovt, UxIOVEC_T *src);

int uxiovect_append(UxIOVEC_T* iovt, uint8* src, uint32 num);
int uxiovect_append_iovect(UxIOVEC_T* dest, UxIOVEC_T *src);
int uxiovect_append_copy(UxIOVEC_T* iovt, uint8* src, uint32 num);

int uxiovect_addspace(UxIOVEC_T* iovt, uint8* src, uint32 num);

int uxiovect_prepend(UxIOVEC_T* iovt, uint8* src, uint32 num);
int uxiovect_prepend_iovect(UxIOVEC_T* dest, UxIOVEC_T *src);

int uxiovect_trunc(UxIOVEC_T *iovt, uint32 offset);
int uxiovect_trim(UxIOVEC_T* iovt, uint32 offset, uint32 num);

int uxiovect_duplicate(UxIOVEC_T *dest, UxIOVEC_T *src, uint32 offset, uint32 len, bool deep);

int uxiovect_insert_index(UxIOVEC_T *iovt, uint32 index, uint8* src, uint32 len);
int uxiovect_insert(UxIOVEC_T *iovt, uint32 offset, uint8* src, uint32 len);
int uxiovect_remove_index(UxIOVEC_T *iovt, uint32 index);
int uxiovect_removeall(UxIOVEC_T *iovt);

UxIOVEC *uxiovect_getsegment(UxIOVEC_T *iovt, uint32 offset);

UxIOVEC_T *uxiovect_init(UxBufPool pool);
void uxiovect_delete(UxIOVEC_T *iovt);
void uxiovect_entry(UxIOVEC_T *iovt);
int uxiovect_alloc(UxIOVEC_T *iovt, uint32 offset, uint32 size);
int uxiovect_growtable(UxIOVEC_T *iovt);
int uxiovect_freeall(UxIOVEC_T *iovt);
int uxiovect_reset(UxIOVEC_T *iovt);

#ifdef UXIOVEC_DEBUG_PRINT
void uxiovect_print(UxIOVEC_T *iovt);
#endif

#endif // __IOVEC_H__
