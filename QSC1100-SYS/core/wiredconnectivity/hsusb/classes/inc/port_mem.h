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
#ifndef _PORT_MEM_H_
#define _PORT_MEM_H_

#define M_ZERO          0x0001          /* BZero the allocation */
#define M_CACHABLE      0x0002          /* Request cachable buffer */
#define M_PAGE_ALIGN    0x0004          /* Must not cross page boundaries */

void *jos_malloc(juint32_t size, juint16_t flags);
void jos_free(void *addr);

jresult_t os_dma_alloc(juint32_t size, void **vaddr, void **paddr, juint16_t flags,
    void **cookie);
void os_dma_free(void *cookie);

void os_cache_flush(void *addr, juint32_t size);
void os_cache_invalidate(void *addr, juint32_t size);
#endif

