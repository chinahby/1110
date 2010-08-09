/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#define INCLUDE_MEM
#define INCLUDE_PCI
#include "jos_internal.h"

#ifndef CONFIG_MEMPOOL_ALIGN
# define CONFIG_MEMPOOL_ALIGN (sizeof(void *))
#endif

#ifdef CONFIG_MEMPOOL

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
# ifndef CONFIG_MEMPOOL
#  error "Cannot show mempool statistics when mempool is not configured !"
# endif
#endif

void *jmemp_alloc(juint32_t size, juint16_t flags);
void jmemp_free(void *item);
#endif

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
static jint32_t total_dma_peak_curr = 0;
static jint32_t blocks_used_curr = 0;
static jint32_t total_dma_free_items_curr = 0;
static jint32_t total_dma_free_items_max = 0;
static jint32_t total_dma_items = 0;
#endif

#if defined(JDEBUG)
#define MEM_LOG_MAGIC_START     0x2B1A11CC
#define MEM_LOG_MAGIC_END       0x3C2B22DD

typedef struct memory_info_t 
{
    jint32_t     magic_start;
    char        *file;
    jint32_t     line;
    unsigned long size;
    void        *org;
    void        *user_ptr;
    jint32_t     page_allocated;
    jint32_t     freed;
    jint32_t     id;
    struct memory_info_t *next, *prev;
    jint32_t     magic_end;
} memory_info_t;

static memory_info_t mem_log_head[] = {{
    MEM_LOG_MAGIC_START, "head", 0, 0, (void*)0xffff, NULL,
    0, 0, 0, NULL, NULL, ~MEM_LOG_MAGIC_START }};

static jint32_t mem_log_id = 0;
void mem_log_print(void)
{
    memory_info_t *curr;
    jint32_t total_allocs = 0;
    jint32_t total_mem = 0;
    char * filename = NULL;
    juint16_t str_len = 0;
    juint16_t i = 0;

    DBG_E(DJOS, ("\n"));

    DBG_E(DJOS, ("MEM: Starting memory log dump:\n"));
    for (curr = mem_log_head->next; curr; curr = curr->next)
    {
        total_mem += curr->size;
        total_allocs ++;

        /* calculate the string length */
        filename = curr->file;
        while( *filename++ ) ;
        str_len = (int)(filename - curr->file - 1);

        /* find the last "\" in the string, point to the first character after it */
        filename = curr->file;
        for(i = str_len-1 ; i > 0 ; --i)
        {
          if (filename[i] == '\\')
          {
            filename = &(filename[i+1]);
            break;
          }
        }

        DBG_E(DJOS, ("MEM: %ld: %c%c%c%c%c:%ld - Size %ld    -  %p\n", 
            curr->id,
            filename[0],filename[1],filename[2],filename[3],filename[4],
            curr->line, curr->size, curr->user_ptr));
    }
    DBG_E(DJOS, ("MEM: Total of %ld bad entries using %ld bytes of memory\n",
        total_allocs, total_mem));

    DBG_E(DJOS, ("MEM: Un-Freed DMA %ld entries using %ld bytes of dma\n",
        blocks_used_curr, total_dma_peak_curr));
}

void *jmalloc_d(juint32_t size, juint16_t flags, char *file, jint32_t line)
{
    juint32_t *p = NULL;
    memory_info_t *m;

#ifdef CONFIG_MEMPOOL
    m = jmemp_alloc(size + sizeof(memory_info_t) + sizeof(juint32_t) * 2 + 4, 
        flags | M_ZERO);
#else
    m = jos_malloc(size + sizeof(memory_info_t) + sizeof(juint32_t) * 2 + 4, 
        flags | M_ZERO);
#endif
    if (!m)
        return NULL;
    m->magic_start      = MEM_LOG_MAGIC_START;
    m->magic_end        = ~MEM_LOG_MAGIC_START;
    m->file             = file;
    m->line             = line;
    m->size             = size;
    m->org              = m;
    m->id               = mem_log_id ++;
    m->freed            = 0;
    m->prev             = mem_log_head;
    m->next             = mem_log_head->next;
    mem_log_head->next  = m;
    if (m->next)
        m->next->prev = m;
    
    p = (juint32_t *)(m + 1);
    m->user_ptr = (void *)p;

    p[(size / 4) + 1] = MEM_LOG_MAGIC_END;
    p[(size / 4) + 2] = ~MEM_LOG_MAGIC_END;

    return (void *)p;
}

void jfree_d(void *addr, char *file, jint32_t line)
{
    memory_info_t *m = addr;
    juint32_t *p = (juint32_t *)addr;
    m--;

    KASSERT(addr, ("NULL pointer passed to jos_free %s:%ld\n", file, line));
    
    KASSERT(
        (m->magic_start == MEM_LOG_MAGIC_START) &&
        (m->magic_end == ~MEM_LOG_MAGIC_START),
        ("Freeing someone else's memory %s:%ld\n", file, line));
    
    KASSERT(
        (p[(m->size / 4) + 1] == MEM_LOG_MAGIC_END) &&
        (p[(m->size / 4) + 2] == ~MEM_LOG_MAGIC_END),
        ("The memory was overflown/corrupted, allocated at %s:%ld, "
        "freed %s:%ld\n", m->file, m->line, file, line));

    KASSERT(!m->freed, ("Memory was already freed %s:%ld\n", file, line));

    m->freed ++;
    m->prev->next = m->next;
    if (m->next)
        m->next->prev = m->prev;

#ifdef CONFIG_MEMPOOL
    jmemp_free(m->org);
#else
    jos_free(m->org);
#endif
}

#else
void mem_log_print(void)
{
}

#endif

/* 
 * Statically allocated memory management 
 *
 * Automaticaly allocate fragments from a large buffer passed during
 * initialization of the stack. Used instead of having to call the OS's
 * os_alloc/jos_free/os_dma_alloc/etc. */

#ifdef CONFIG_MEMPOOL

typedef enum {
    POOL_VIRTUAL,
#ifdef CONFIG_POOL_DMA 
    POOL_DMA,
# ifdef CONFIG_POOL_DMA_CACHABLE
    POOL_DMA_CACHABLE,
# endif
#endif    
    POOL_LAST
} pool_types;
   
static juint32_t pool_offset[POOL_LAST];
static juint32_t pool_size[POOL_LAST];
static juint8_t *pool_vbuf[POOL_LAST];
static juint8_t *pool_pbuf[POOL_LAST];

static void pool_init(pool_types type, void *vstart, 
    void *pstart, juint32_t size)
{
    juint32_t offset;
  
    DBG_I(DJOS_MEM, ("MEM: New pool %d size %d from [%x:%x]\n",
        type, size, vstart, pstart));

    offset = (juint32_t)(vstart) % CONFIG_MEMPOOL_ALIGN;
    if (offset)
        offset = CONFIG_MEMPOOL_ALIGN - offset;

    pool_offset[type] = offset;
    pool_size[type] = size;
    pool_vbuf[type] = (juint8_t *)vstart;
    pool_pbuf[type] = (juint8_t *)pstart;
}

static jresult_t pool_alloc(juint32_t size, void **vaddr, void **paddr, 
    pool_types type)
{
    juint32_t alloc_size = size;

    DBG_I(DJOS_MEM, ("MEM: Pool alloc %d from pool %d\n", size, type));
    
    if (alloc_size % CONFIG_MEMPOOL_ALIGN)
    {
        alloc_size = (alloc_size + CONFIG_MEMPOOL_ALIGN) - 
            (alloc_size % CONFIG_MEMPOOL_ALIGN);
    }

    if (pool_offset[type] + alloc_size >= pool_size[type])
    {
        DBG_E(DJOS_MEM, ("JOS_MEM: Not enough memory on pool %d, "
            "need %ld have %ld\n", type, size,
            pool_size[type] - pool_offset[type]));
        return JENOMEM;
    }
    
    if (vaddr)
        *vaddr = (void *)&((pool_vbuf[type])[pool_offset[type]]);

    if (paddr)
        *paddr = (void *)&((pool_pbuf[type])[pool_offset[type]]);

    pool_offset[type] += alloc_size;

    return 0;
}

#endif

/* 
 * DMA Memory pool management 
 *
 * We need to allocate a lot of small (many 8 byte, some larger)
 * memory blocks that can be used for DMA.  Using the bus_dma
 * routines directly would incur large overheads in space and time. */

#define DMA_MEM_BLOCK   32

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
static juint32_t blocks_used_max = 0;
static juint32_t total_dma_peak_max = 0;

# define DBG_INC(x)             ((x)++)
# define DBG_DEC(x)             ((x)--)
#else
# define DBG_INC(x)
# define DBG_DEC(x)
#endif

#ifdef CONFIG_MEMPOOL

/* 
 * Implementation of Memory Pool allocation
 * ----------------------------------------
 * The code below allocates a single large block of memory from the
 * porting layer during the stack initialization and uses block lists to 
 * support basic malloc/free 
 * 
 * */

/* Set the distribution of item sizes */
juint32_t alloc_list_size[] =    {16, 32, 64, 128, 256};
#define ALLOC_LIST_GET(i)       (alloc_list_size[i])

#define ALLOCATION_LISTS        (sizeof(alloc_list_size) / sizeof(juint32_t))

/* Holds the head of all allocation lists - last one for un-even sized blocks */
void *alloc_list[ALLOCATION_LISTS + 1];

#define ITEM_NEXT(x)            (*((void **)(x)))
#define ITEM_SIZE(x)            (((juint32_t *)(x))[-1])

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
# define ITEM_SIZE_USED(x)      (((juint32_t *)(x))[-2])
static juint32_t alloc_list_watch[ALLOCATION_LISTS + 1][2];
static juint32_t total_peak_curr = 0;
static juint32_t total_peak_max = 0;
static juint32_t total_allocated = 0;

static void dump_mem_status(void);
#endif

static void *memp_item_alloc(juint32_t size);
static void *memp_item_get(void **head, juint32_t size);
static void *memp_item_get_ex(void **head, juint32_t size);

void mempool_init(void)
{
    jint_t i;

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    for (i = 0; i < ALLOCATION_LISTS + 1; i++)
    {
        alloc_list_watch[i][0] = 0;
        alloc_list_watch[i][1] = 0;
    }
#endif
    
    /* Zero the lists */
    for (i = 0; i < ALLOCATION_LISTS + 1; i++)
        alloc_list[i] = NULL;

#ifdef JDEBUG
    /* In debug mode we have an overhead per malloc for the memory debug module,
     * so increase the alloc lists accordingly */
# define DEBUG_ALLOC_OVERHEAD \
    (sizeof(memory_info_t) + sizeof(juint32_t) * 2 + 4)

    for (i = 0; i < ALLOCATION_LISTS; i++)
        alloc_list_size[i] += DEBUG_ALLOC_OVERHEAD;
#endif
}

void mempool_uninit(void)
{
#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    dump_mem_status();
#endif
}

void *jmemp_alloc(juint32_t size, juint16_t flags)
{
    jint_t i;
    void *item;

    /* Find smallest pool to alloc from */
    for (i = 0; i < ALLOCATION_LISTS && size > ALLOC_LIST_GET(i); i++)
        ;

    /* Get an item off the list */
    if (i < ALLOCATION_LISTS)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("JOS_MEM: Allocating item from const list %d\n", i));
#endif /*  CONFIG_DATA_PATH_DEBUG */

        item = memp_item_get(&(alloc_list[i]), ALLOC_LIST_GET(i));
    }
    /* Use the un-even sized pool */
    else
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("JOS_MEM: Allocating from variable sized list\n"));
#endif /*  CONFIG_DATA_PATH_DEBUG */
        item = memp_item_get_ex(&alloc_list[ALLOCATION_LISTS], size);
    }

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("JOS_MEM: Allocated %p size %ld pool %d\n", 
        item, size, i));
#endif /*  CONFIG_DATA_PATH_DEBUG */

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    if (item)
    {
        alloc_list_watch[i][0]++;
        alloc_list_watch[i][1] = MAX(alloc_list_watch[i][0],
            alloc_list_watch[i][1]);

        ITEM_SIZE_USED(item) = size;
        total_peak_curr += size;
        total_peak_max = MAX(total_peak_max, total_peak_curr);
    }
#endif
    if (item && (flags & M_ZERO))
        j_memset(item, 0, size);
    
    return item;
}

void jmemp_free(void *item)
{
    juint32_t size;
    jint_t i;

    size = ITEM_SIZE(item);
    
    /* Find the pool we alloced from */
    for (i = 0; i < ALLOCATION_LISTS && size > ALLOC_LIST_GET(i); i++)
        ;

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("JOS_MEM: Freeing item %p size %ld pool %d\n",
        item, size, i));
#endif /* CONFIG_DATA_PATH_DEBUG */

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    alloc_list_watch[i][0]--;
    total_peak_curr -= ITEM_SIZE_USED(item);
#endif

    ITEM_NEXT(item) = alloc_list[i];
    alloc_list[i] = item;
}

static void *memp_item_alloc(juint32_t size)
{
    juint32_t *item;
    juint32_t alloc_size = size + sizeof(juint32_t);
    void *ptr;

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    alloc_size += sizeof(juint32_t);
#endif 

    DBG_X(DJOS_MEM, ("JOS_MEM:  Allocating a new item size %ld\n" , size));
#ifdef CONFIG_MEMPOOL
    if (pool_alloc(alloc_size, &ptr, NULL, POOL_VIRTUAL))
    {
        DBG_E(DJOS_MEM, ("JOS_MEM: Not enough memory in pool\n"));
        return NULL;
    }
#else
    ptr = jos_malloc(alloc_size, 0);
    if (!ptr)
        return NULL;
#endif
    
    item = (juint32_t *)ptr;

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    /* Save space to put the "used size" of the item */
    item = &(item[1]);
    total_allocated += alloc_size;
#endif

    /* Save size of the item at the start */
    item[0] = size;

    DBG_X(DJOS_MEM, ("JOS_MEM:  Allocated %p (orig %p) - %ld\n",
        &item[1], item, item[0]));

    /* Advance actual pointer to follow the size */
    return ((void *) &(item[1]));
}

static void *memp_item_get(void **head, juint32_t size)
{
    void *item;
   
    /* If the list is empty alloc new item */
    if (!*head)
    {
        item = memp_item_alloc(size);
        if (!item)
            return NULL;
    }
    /* Get one off the list */
    else
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("JOS_MEM: Getting item of list %p\n", *head));
#endif /* CONFIG_DATA_PATH_DEBUG */
        item = *head;
        *head = ITEM_NEXT(item);
    }

    return item;
}

static void *memp_item_get_ex(void **head, juint32_t size)
{
    void *item = NULL;
    void **curr = head;
    
#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("JOS_MEM: Item_get_ex %p size %ld\n", *head, size));
#endif /* CONFIG_DATA_PATH_DEBUG */

    while (*curr)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_V(DJOS_MEM, ("JOS_MEM: Checking %p size %ld\n",
            *curr, ITEM_SIZE(*curr)));
#endif /* CONFIG_DATA_PATH_DEBUG */

        if (ITEM_SIZE(*curr) >= size)
        {
            item = *curr;
            *curr = ITEM_NEXT(item);
            break;
        }

        /* Get the next item from the start of the previous mem block */
        curr = &(ITEM_NEXT(*curr));
    }

    if (!item)
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_V(DJOS_MEM, ("JOS_MEM: Nothing fitting found, making new one\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
        item = memp_item_alloc(size);
        if (!item)
            return NULL;
    }
    else
    {
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_I(DJOS_MEM, ("JOS_MEM: Reusing big block %p sized %ld\n", item, 
            ITEM_SIZE(item)));
#endif /* CONFIG_DATA_PATH_DEBUG */
    }

    return item;
}

#ifdef CONFIG_POOL_DMA
static jresult_t pool_dma_alloc(juint32_t size, void **vaddr, void **paddr,
    juint16_t flags, jbool_t *cachable)
{
#ifdef CONFIG_POOL_DMA_CACHABLE
    jresult_t rc;
#endif

    DBG_V(DJOS_MEM, ("JOS_MEM: Allocating a new dma buffer size %ld\n" , size));

#ifdef CONFIG_POOL_DMA_CACHABLE
    /* For cachable buffers check if there is room in the cachable pool */
    if (flags & M_CACHABLE)
    {
        rc = pool_alloc(size, vaddr, paddr, POOL_DMA_CACHABLE);
        if (!rc)
        {
            (*cachable) = TRUE;
            return 0;
        }
    }
#endif

    *cachable = FALSE;
    return pool_alloc(size, vaddr, paddr, POOL_DMA);
}
#endif

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
static void dump_mem_status(void)
{
    jint_t i;

    jprintf("\n");

    jprintf("Memory utilization:\n");

    jprintf("\n");

    jprintf("RAM buffer usage: %ld\n", total_allocated);
    
#ifdef JDEBUG
    DBG_E(DJOS_MEM, ("(with %ld bytes per malloc debug overhead)\n",
        DEBUG_ALLOC_OVERHEAD));
#endif

    jprintf("\n");
    
    jprintf("Actual Max use:\n");

    jprintf("RAM: %ld DMA: %ld Total: %ld\n",
        total_peak_max, total_dma_peak_max,
        total_peak_max + total_dma_peak_max);

    jprintf("\n");

    jprintf("Dumping RAM allocation statistics:\n");

    for (i = 0; i < ALLOCATION_LISTS + 1; i++)
    {
        jprintf("  %d: Size %4d in use %3d max used %3d\n",
            i, (i < ALLOCATION_LISTS ? ALLOC_LIST_GET(i) : 0),
            alloc_list_watch[i][0], alloc_list_watch[i][1]);
    }

    jprintf("\n");

    jprintf("Total dma entries %d, max on free list: %d \n",
        total_dma_items, total_dma_free_items_max);
    
    jprintf("\n");
}
#endif

#endif

#ifdef JDEBUG
# define CORRUPT_CHECK_MAGIC    0xAABB00CC
#endif

static LIST_HEAD(dma_frag_s_s, dma_block_s) dma_freelist =
    LIST_HEAD_INITIALIZER(dma_freelist);

void jdma_dummy_set(juint32_t size, void *vaddr, void *paddr, jdma_handle p)
{
    jdma_t *dma = (jdma_t *)p;

    if (!dma)
    {
        DBG_E(DJOS_MEM, ("MEM: Setting dummy for an invalid buffer !\n"));
        return;
    }

    if (!(dma->flags & BLOCK_DUMMY))
    {
        DBG_E(DJOS_MEM, ("MEM: Trying to set non-dummy block with values\n"));
        return;
    }
    
    dma->paddr = paddr;
    dma->vaddr = vaddr;
    dma->size = size;
}

void *jdma_phys_addr(jdma_handle p, juint32_t offset)
{
    return (void *)(((char *)((jdma_t *)p)->paddr) + offset);
}

void *jdma_virt_addr(jdma_handle p, juint32_t offset)
{
    return (void *)(((char *)((jdma_t *)p)->vaddr) + offset);
}

static void jdma_block_free(jdma_t *dma)
{
#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_free_items_curr++;
    total_dma_free_items_max = 
        MAX(total_dma_free_items_max, total_dma_free_items_curr);
#endif

#ifdef JDEBUG
    ((juint32_t *)jdma_virt_addr(dma, 0))[0] = CORRUPT_CHECK_MAGIC;
#endif
    
    LIST_INSERT_HEAD(&dma_freelist, dma, next);
}

static jresult_t dma_block_add(juint32_t size, jbool_t cachable, 
    juint8_t *vaddr, juint8_t *paddr, void *handle)
{
    juint32_t addr = (juint32_t)paddr;
    jdma_t *dma;
    jint_t i;

    KASSERT(!((juint32_t)vaddr % CONFIG_MEMPOOL_ALIGN),
        ("MEM: New buffer is incorrectly aligned\n"));

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_items ++;
#endif

    dma = jmalloc(sizeof(struct dma_block_s), M_ZERO);
    if (!dma)
        return JENOMEM;

    DBG_V(DJOS_MEM, ("MEM: dma_block_add(size %d [%p:%p] as %p)\n",
        size, vaddr, paddr, dma));

    dma->vaddr = (void *)vaddr;
    dma->paddr = (void *)paddr;
    dma->size = size;

    if (cachable)
        dma->flags |= BLOCK_CACHABLE;
    
    /* Calculate buffer alignment */
    for (i = 1; !(addr & i) && (i <= 4096); i *= 2)
            ;
    dma->align = i;
    
    /* Check if does not cross page boundry */
    if (((addr / PAGE_SIZE) == (addr + size) / PAGE_SIZE))
        dma->flags |= BLOCK_PAGE_ALIGN;

    /* Save handle if this struct owns the real buffer */
    if (handle)
    {
        dma->os_handle = handle;
        dma->flags |= BLOCK_OWNER;
    }

    /* Put at head of list */
    jdma_block_free(dma);

    return 0;
}

static jresult_t dma_block_alloc_os(juint32_t size, void **vaddr, void **paddr,
    juint16_t flags, jbool_t *cachable, void **handle)
{
    jresult_t rc = 0;

#ifdef CONFIG_MEMPOOL_DMABLE
    *vaddr = jmalloc(size, 0);
    if (!*vaddr)
        return JENOMEM;
    
    *handle = *paddr = *vaddr;
    *cachable = FALSE;

#elif defined(CONFIG_POOL_DMA)
     /* Alloc memory block */
    rc = pool_dma_alloc(size, vaddr, paddr, flags, cachable);
    if (rc)
        return rc;

    *handle = NULL;
#else
    /* TODO: Pass the M_CACHABLE to the OS */
    rc = os_dma_alloc(size, vaddr, paddr, flags, handle);
    if (rc)
        return rc;

    *cachable = FALSE;
#endif

    return rc;
}

/**
 * Function name:  dma_block_recover
 * Description:    Try to recover memory region around a new block
 * Parameters:
 *     @size:      Size of available space for recovery
 *     @cachable:  Cachability of memory region
 *     @vaddr:     Virtual address of start of free region
 *     @paddr:     Physical address of start of free region
 *
 * Return value:   None
 * Scope:          Local
 **/
static void dma_block_recover(juint32_t size, jbool_t cachable, 
    juint8_t *vaddr, juint8_t *paddr)
{
    juint32_t phys_addr = (juint32_t)paddr;
    juint32_t offset = 0;

    /* Is the size enough to fit in the minimal block ? */
    if (size < DMA_MEM_BLOCK)
        return;

    /* Do we need to re-align for platform requirements ? */
    if (phys_addr % CONFIG_MEMPOOL_ALIGN)
        offset = CONFIG_MEMPOOL_ALIGN - (phys_addr % CONFIG_MEMPOOL_ALIGN);

    /* Did the re-alignment larger then the size we have ? */
    if (size <= offset)
        return;

    /* Do we still fit the minimal block (after re-align) */
    if ((size - offset) < DMA_MEM_BLOCK)
        return;

    /* Add new block */
    dma_block_add(size - offset, cachable, 
        (void *)((juint32_t)vaddr + offset), 
        (void *)((juint32_t)paddr + offset), NULL);
}

static jresult_t dma_block_alloc(juint32_t size, juint16_t align, juint16_t flags)
{
    jresult_t rc;
    jbool_t cachable = 0;
    juint32_t needed_offset = 0;
    juint32_t alloc_size = size;
    juint8_t *vaddr, *paddr;
    void *vtemp, *ptemp = NULL;
    void *handle = NULL;

    KASSERT(alloc_size, ("MEM: Cannot use dma_block_alloc for zero "
        "sized blocks\n"));

    /* Make sure the user doesn't want something crazy */
    if ((flags & M_PAGE_ALIGN) && (size > PAGE_SIZE))
    {
        DBG_E(DJOS_MEM, ("MEM: Requested buffer not to fall on page boundry "
            "while buffer is larger then a page\n"));
        return JEINVAL;
    }
    
    /* Make sure we allocate aleast DMA_MEM_BLOCK size */
    alloc_size = MAX(alloc_size, DMA_MEM_BLOCK);

    /* Extra space for alignment */
    alloc_size += align - 1;

    /* Extra space for page boundry restriction */
    if (flags & M_PAGE_ALIGN)
        alloc_size += alloc_size;

    DBG_V(DJOS_MEM, ("MEM: dma_block_alloc(size %d real %d)\n",
        alloc_size, size));

    /* Alloc memory block */
    rc = dma_block_alloc_os(alloc_size, &vtemp, &ptemp, 
        flags, &cachable, &handle);
    if (rc)
        return rc;

    vaddr = (juint8_t *)vtemp;
    paddr = (juint8_t *)ptemp;

    /* See if page bounds are crossed */
    if (flags & M_PAGE_ALIGN)
    {
        if ((((juint32_t)paddr % PAGE_SIZE) + (size + align - 1)) > PAGE_SIZE)
            needed_offset = PAGE_SIZE - ((juint32_t)paddr % PAGE_SIZE);
    }

    /* See if we need to realign the resulting buffer */
    if (((juint32_t)paddr + needed_offset) % align)
    {
        needed_offset = needed_offset + align - 
            (((juint32_t)paddr + needed_offset) % align);
    }

    /* Recover the wasted space around the allocated buffer */
    dma_block_recover(needed_offset, cachable, vaddr, paddr);
    dma_block_recover(alloc_size - size - needed_offset, cachable, 
        vaddr + needed_offset + size, 
        paddr + needed_offset + size);

    /* Add the required block */
    return dma_block_add(size, cachable, vaddr + needed_offset, 
        paddr + needed_offset, handle);
}

jdma_t *find_existing(juint32_t size, juint16_t align, juint16_t flags)
{
    jdma_t *dma, *found = NULL;

    /* First check the free list. */
    for (dma = LIST_FIRST(&dma_freelist); 
        dma && size; 
        dma = LIST_NEXT(dma, next)) 
    {
#ifdef JDEBUG
        if (((juint32_t *)jdma_virt_addr(dma, 0))[0] != CORRUPT_CHECK_MAGIC)
        {
            DBG_E(DJOS_MEM, ("MEM: DMA block %p or freelist became corrupted\n",
                dma));
            return NULL;
        }
#endif
        /* Make sure its has the same cachablitity */
        if (((flags & M_CACHABLE) && !(dma->flags & BLOCK_CACHABLE)) || 
            (!(flags & M_CACHABLE) && (dma->flags & BLOCK_CACHABLE))) 
        {
            continue;
        }

        /* Make sure buffer is page aligned, if required */
        if ((flags & M_PAGE_ALIGN) && !(dma->flags & BLOCK_PAGE_ALIGN))
            continue;

        /* Make sure the alignment requirment is met */
        if (dma->align < align)
            continue;

        /* We need a block about the same size */
        if (dma->size < size || dma->size >= size * 2) 
            continue;

        LIST_REMOVE(dma, next);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        total_dma_free_items_curr --;
#endif

        found = dma;
#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM,("MEM: Found block %p size %ld\n", dma, dma->size));
#endif /* CONFIG_DATA_PATH_DEBUG */

        break;
    }

    return found;
}

jresult_t jdma_alloc(juint32_t size, juint16_t align, void **vaddr, void **paddr, 
    juint16_t flags, jdma_handle *handle)
{
    jresult_t err;
    jdma_t *dma = NULL; 
#ifdef JDEBUG
    juint32_t calc;
#endif

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("MEM: j_dma_alloc(%d align %d flags %x)\n",
        size, align, flags));
#endif /* CONFIG_DATA_PATH_DEBUG */
    /* Handle empty dummy blocks */
    if (size == 0)
    {
        dma = jmalloc(sizeof(struct dma_block_s), M_ZERO);
        if (!dma)
            return JENOMEM;

#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("MEM: Alloc dummy %p\n", dma));
#endif /* CONFIG_DATA_PATH_DEBUG */

        dma->flags |= BLOCK_DUMMY;

        *handle = (jdma_handle)dma;

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        blocks_used_curr ++;
#endif
        return 0;
    }

    /* Make sure we allocate aleast DMA_MEM_BLOCK size */
    size = MAX(size, DMA_MEM_BLOCK);  

    /* Make alignment reasonable */
    align = MAX(align, CONFIG_MEMPOOL_ALIGN);

    /* Try to find an existing buffer on the free list */
    dma = find_existing(size, align, flags);
      
    /* If we didn't find a cachable buffer, get a normal one */
    if ((!dma) && (flags & M_CACHABLE))
        dma = find_existing(size, align, (flags & ~M_CACHABLE));

    /* If we didn't find a suitable buffer, allocate a new one */
    if (!dma)
    {
        /* Alloc a new block if nothing matching was found */
        err = dma_block_alloc(size, align, flags);
        /* HSU addition: Memory fragmentation in the DMA pool  */
        if (err)
        {
            jdma_free_all();
            err = dma_block_alloc(size, align, flags);
        }
        /* End HSU addition */
        if (err)
            return (err);

        /* Get the first block 
         * (dma_alloc_block will put the needed block at head) */
        dma = LIST_FIRST(&dma_freelist);
        LIST_REMOVE(dma, next);

#ifdef JDEBUG
        if (((juint32_t *)jdma_virt_addr(dma, 0))[0] != CORRUPT_CHECK_MAGIC)
        {
            DBG_E(DJOS_MEM, ("MEM: New item on freelist is corrupted\n")); 
            return EUNKNOWN;
        }
#endif

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        total_dma_free_items_curr --;
#endif
    }
    
    if (handle)
        *handle = (jdma_handle)dma;

    if (vaddr)
        *vaddr = jdma_virt_addr(dma, 0);

    if (paddr)
        *paddr = jdma_phys_addr(dma, 0);

    if (flags & M_ZERO)
        j_memset(jdma_virt_addr(dma, 0), 0, size);

#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_I(DJOS_MEM, ("MEM: jdma_alloc_done(%p:%p)\n",
        jdma_virt_addr(dma, 0),
        jdma_phys_addr(dma, 0)));
#endif /* CONFIG_DATA_PATH_DEBUG */

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_peak_curr += dma->size;
    blocks_used_curr ++;
#endif

#ifdef CONFIG_MEMPOOL_USAGE_SHOW
    total_dma_peak_max = MAX(total_dma_peak_max, total_dma_peak_curr);
    blocks_used_max = MAX(blocks_used_max, blocks_used_curr);
#endif

#ifdef JDEBUG
    /* Check the alignment is as requested */
    calc = (juint32_t)jdma_phys_addr(dma, 0);
    if (calc % align)
    {
        DBG_E(DJOS_MEM, ("MEM: Allocated buffer does not fit requirment\n"));
        return JEINVAL;
    }

    if (flags & M_PAGE_ALIGN)
    {
        if ((calc / PAGE_SIZE) != ((calc + size) / PAGE_SIZE))
        {
            DBG_E(DJOS_MEM, ("MEM: Allocated buffer does not fit "
                "requirment buff %x size %d\n", calc, size));
            return JEINVAL;
        }
    }
#endif

    return 0;
}

void jdma_free(jdma_handle p)
{
    jdma_t *dma = (jdma_t *)p;
    
#ifdef CONFIG_DATA_PATH_DEBUG
    DBG_V(DJOS_MEM, ("MEM: jdma_free(%p - [%p:%p] size %d)\n",
        dma, dma->vaddr, dma->paddr, dma->size));
#endif /* CONFIG_DATA_PATH_DEBUG */

    if (dma->flags & BLOCK_DUMMY)
    {
        if (dma->size)
            DBG_E(DJOS_MEM, ("MEM: Dummy was not reset before being freed\n"));

#ifdef CONFIG_DATA_PATH_DEBUG
        DBG_X(DJOS_MEM, ("MEM: Freeing dummy !\n"));
#endif /* CONFIG_DATA_PATH_DEBUG */
        jfree(dma);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
        blocks_used_curr --;
#endif
        return;
    }

    jdma_block_free(dma);

#if defined(JDEBUG) || defined(CONFIG_MEMPOOL_USAGE_SHOW)
    total_dma_peak_curr -= dma->size;
    blocks_used_curr --;
#endif
}

void jdma_free_all(void)
{
    jdma_t *dma;

    /* Free the blocks */
    for (dma = LIST_FIRST(&dma_freelist);
        dma;
        dma = LIST_FIRST(&dma_freelist))
    {
        LIST_REMOVE(dma, next);

        if (!(dma->flags & BLOCK_OWNER))
        {
            jfree(dma);
            continue;
        }

        DBG_I(DJOS_MEM, ("MEM: Freeing block %p handle %p alignment %d\n", 
            dma, dma->os_handle, dma->align));

#ifdef CONFIG_MEMPOOL_DMABLE
        jfree(dma->os_handle);
#elif defined(CONFIG_POOL_DMA)
        /* We give the whole pool back on un-init */
#else
        os_dma_free(dma->os_handle);
#endif

        jfree(dma);
    }
}

jresult_t jos_mem_init(uw_args_t *args)
{
    jresult_t rc = 0;
   
    KASSERT(sizeof(juint32_t) <= DMA_MEM_BLOCK,
        ("MEM: Minimal dma alloc buffer is too small (%d) should"
        "be atleast %d\n", DMA_MEM_BLOCK, sizeof(juint32_t)));

#ifdef CONFIG_MEMPOOL
    mempool_init();
#endif

#ifdef CONFIG_MEMPOOL
    pool_init(POOL_VIRTUAL, args->mem_buf, args->mem_buf, args->mem_buf_size);
#endif

#ifdef CONFIG_POOL_DMA
    pool_init(POOL_DMA, args->dma_vbuf, args->dma_pbuf, args->dma_buf_size);
    
# ifdef CONFIG_POOL_DMA_CACHABLE
    pool_init(POOL_DMA_CACHABLE, args->dma_cachable_vbuf, 
        args->dma_cachable_pbuf, args->dma_cachable_size);
# endif
#endif

    return rc;
}

void jos_mem_uninit()
{
#ifdef CONFIG_MEMPOOL
    mempool_uninit();
#endif
}

