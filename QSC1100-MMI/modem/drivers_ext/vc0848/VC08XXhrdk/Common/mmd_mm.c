/*******************************************************************************
 *  All rights reserved, Copyright (C) vimicro LIMITED 2007                                                     
 * --------------------------------------------------------------------------------------------------------------
 * [File Name]            : mmd_mm.c
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
 
/*
 * INCLUDE FILE                                              
 */
#include "mmd_mm.h"

#if 1
//MMD_U08 g_rdk_heap_pool[MMD_RDK_HEAP_SIZE];

//static inline void MMD_MM_Internal_Alloc(MMD_U32 size, struct heap *heap)
//{

//}

// allocate size bytes and returns a pointer to the allocated memory
//the memory is not cleared

#endif


#ifdef MMD_MEM_DEBUG_INFO
#define PRINTF(WriteVal)     MMD_PRINTF(WriteVal)
#define PRINTFN(WriteVal)   MMD_PRINTFN(WriteVal)
#define PRINTFE(WriteVal)   MMD_PRINTFE(WriteVal)
#define PRINTFS(WriteVal)   MMD_PRINTFS(WriteVal)
#else
    #define PRINTF(WriteVal)
    #define PRINTFN(WriteVal) 
    #define PRINTFE(WriteVal)
    #define PRINTFS(WriteVal)
#endif

#define  u32_t MMD_U32
#define  u8_t  MMD_U08
#define  u16_t MMD_U16
#define  u64_t MMD_U64
#define  s16_t MMD_S16

typedef u32_t mem_ptr_t;

typedef u32_t mem_size_t;

struct mem {
  mem_size_t next, prev;
#if MEM_ALIGNMENT == 1
  u8_t used;
#elif MEM_ALIGNMENT == 2
  u16_t used;
#elif MEM_ALIGNMENT == 4
  u32_t used;
#elif MEM_ALIGNMENT == 8
  u64_t used;
#else
#error "unhandled MEM_ALIGNMENT size"
#endif /* MEM_ALIGNMENT */
}; 

#define MEM_SIZE                        MMD_RDK_HEAP_SIZE

static struct mem *ram_end;
static u8_t ram[MEM_SIZE + sizeof(struct mem) + MEM_ALIGNMENT];

#define MIN_SIZE 12
#if 0 /* this one does not align correctly for some, resulting in crashes */
#define SIZEOF_STRUCT_MEM (unsigned int)MEM_ALIGN_SIZE(sizeof(struct mem))
#else
#define SIZEOF_STRUCT_MEM (sizeof(struct mem) + \
                          (((sizeof(struct mem) % MEM_ALIGNMENT) == 0)? 0 : \
                          (4 - (sizeof(struct mem) % MEM_ALIGNMENT))))
#endif
static struct mem *lfree; 

static void plug_holes(struct mem *mem)
{
  struct mem *nmem;
  struct mem *pmem;

  if((u8_t *)mem >= ram)
      PRINTF(("plug_holes: mem >= ram"));
  if((u8_t *)mem < (u8_t *)ram_end)
      PRINTF(("plug_holes: mem < ram_end"));
  if(mem->used == 0)    
      PRINTF(("plug_holes: mem->used == 0"));
  
  /* plug hole forward */
  if(mem->next <= MEM_SIZE)
      PRINTF(("plug_holes: mem->next <= MEM_SIZE"));
  
  nmem = (struct mem *)&ram[mem->next];
  if (mem != nmem && nmem->used == 0 && (u8_t *)nmem != (u8_t *)ram_end) {
    if (lfree == nmem) {
      lfree = mem;
    }
    mem->next = nmem->next;
    ((struct mem *)&ram[nmem->next])->prev = (u8_t *)mem - ram;
  }

  /* plug hole backward */
  pmem = (struct mem *)&ram[mem->prev];
  if (pmem != mem && pmem->used == 0) {
    if (lfree == mem) {
      lfree = pmem;
    }
    pmem->next = mem->next;
    ((struct mem *)&ram[mem->next])->prev = (u8_t *)pmem - ram;
  }

}

void MMD_MM_Initialize(void)
{
  struct mem *mem;

  memset(ram, 0, MEM_SIZE);
  mem = (struct mem *)ram;
  mem->next = MEM_SIZE;
  mem->prev = 0;
  mem->used = 0;
  ram_end = (struct mem *)&ram[MEM_SIZE];
  ram_end->used = 1;
  ram_end->next = MEM_SIZE;
  ram_end->prev = MEM_SIZE;

  //mem_sem = sys_sem_new(1);

  lfree = (struct mem *)ram;

}
void mmd_mem_free(void *rmem)
{
  struct mem *mem;

  if (rmem == NULL) {
    PRINTF(("mem_free(p == NULL) was called.\n"));
    return;
  }
  if((u8_t *)rmem >= (u8_t *)ram && (u8_t *)rmem < (u8_t *)ram_end)
    PRINTF(("mem_free: legal memory"));
  
  if ((u8_t *)rmem < (u8_t *)ram || (u8_t *)rmem >= (u8_t *)ram_end) {
    PRINTF(("mem_free: illegal memory\n"));

    return;
  }
  mem = (struct mem *)((u8_t *)rmem - SIZEOF_STRUCT_MEM);

  if(mem->used)
    PRINTF(("mem_free: mem->used"));
  
  mem->used = 0;

  if (mem < lfree) {
    lfree = mem;
  }
  
  plug_holes(mem);

}

void * mmd_mem_malloc(mem_size_t size)
{
  mem_size_t ptr, ptr2;
  struct mem *mem, *mem2;

  if (size == 0) {
    return NULL;
  }
  PRINTFN(("mem_malloc size[%d]!\n",size));
  /* Expand the size of the allocated memory region so that we can
     adjust for alignment. */
  if ((size % MEM_ALIGNMENT) != 0) {
    size += MEM_ALIGNMENT - ((size + SIZEOF_STRUCT_MEM) % MEM_ALIGNMENT);
  }
  
  if (size > MEM_SIZE) {
    return NULL;
  }

  for (ptr = (u8_t *)lfree - ram; ptr < MEM_SIZE; ptr = ((struct mem *)&ram[ptr])->next) {
    mem = (struct mem *)&ram[ptr];
    if (!mem->used &&
       mem->next - (ptr + SIZEOF_STRUCT_MEM) >= size + SIZEOF_STRUCT_MEM) {
      ptr2 = ptr + SIZEOF_STRUCT_MEM + size;
      mem2 = (struct mem *)&ram[ptr2];

      mem2->prev = ptr;      
      mem2->next = mem->next;
      mem->next = ptr2;      
      if (mem2->next != MEM_SIZE) {
        ((struct mem *)&ram[mem2->next])->prev = ptr2;
      }
      
      mem2->used = 0;      
      mem->used = 1;


      if (mem == lfree) 
      {
           /* Find next free block after mem */
           while (lfree->used && lfree != ram_end) 
	    {
               lfree = (struct mem *)&ram[lfree->next];
           }
           PRINTFN(("mem_malloc: !lfree->used", !lfree->used));
      }

      PRINTFN(("mem_malloc: allocated memory not above ram_end.",
       (mem_ptr_t)mem + SIZEOF_STRUCT_MEM + size <= (mem_ptr_t)ram_end));
      PRINTFN(("mem_malloc: allocated memory properly aligned.",
       (unsigned long)((u8_t *)mem + SIZEOF_STRUCT_MEM) % MEM_ALIGNMENT == 0));
      return (u8_t *)mem + SIZEOF_STRUCT_MEM;
    }    
  }
  PRINTFE(("!!!!!!!!!!\n\nmem_malloc: could not allocate %d bytes\n\n\n", (s16_t)size));

  return NULL;
}

void *MMD_MM_Malloc(MMD_U32 size)
{

	void * ptr;
	//MMD_USER_OS_GetResource(MMD_OS_MALLOC);

	ptr = mmd_mem_malloc(size);

	//MMD_USER_OS_RelResource(MMD_OS_MALLOC);

	return ptr;
}


void MMD_MM_Free(void * ptr)
{
	mmd_mem_free(ptr);
}
