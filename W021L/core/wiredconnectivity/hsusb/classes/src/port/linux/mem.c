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
#include "includes.h"

void *jos_malloc(juint32_t size, juint16_t flags)
{
    void *p = vmalloc(size);

    OS_ASSERT(p, ("failed allocating %d(0x%x) bytes", size, size));
    if (p && (flags & M_ZERO))
        memset(p, 0, size);
    return p;
}

void jos_free(void *addr)
{
    OS_ASSERT(addr, ("tring to free NULL pointer"));

    vfree(addr);
}

typedef struct bus_dma_tag {
    juint32_t size;
    void *cpu_addr;
    void *dma_handle;
} bus_dma_tag;

typedef struct bus_dma_tag *bus_dma_tag_t;

jresult_t os_dma_alloc(juint32_t size, void **vaddr, void **paddr, juint16_t flags,
    void **cookie)
{
    bus_dma_tag_t dmat = jos_malloc(sizeof(bus_dma_tag), flags);
    if (!dmat)
        return JENOMEM;

#ifdef CONFIG_BUS_PCI
    *vaddr = pci_alloc_consistent(NULL, size, (dma_addr_t *)paddr);
#else
    *vaddr = dma_alloc_coherent(NULL, size, (dma_addr_t *)paddr, GFP_ATOMIC);
#endif
    if (!*vaddr)
    {
        jos_free(dmat);
        return JENOMEM;
    }

    dmat->cpu_addr = *vaddr;
    dmat->dma_handle = *paddr;
    dmat->size = size;
    *cookie = (void *)dmat;
    OS_ASSERT(!((juint32_t)*paddr & (PAGE_SIZE -1)),
        ("dma_alloc returned unaligned address\n"));
    return 0;
}

void os_dma_free(void *cookie)
{
    bus_dma_tag_t dmat = (bus_dma_tag_t)cookie;
#ifdef CONFIG_BUS_PCI
    pci_free_consistent(NULL, dmat->size, dmat->cpu_addr,
        (dma_addr_t)dmat->dma_handle);
#else
    dma_free_coherent(NULL, dmat->size, dmat->cpu_addr,
        (dma_addr_t)dmat->dma_handle);
#endif

    jos_free(dmat);
}

/* Flush the cache to the RAM */
void os_cache_flush(void *addr, juint32_t size)
{
}

/* Mark the cache as invalidate */
void os_cache_invalidate(void *addr, juint32_t size)
{
}

