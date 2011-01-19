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
#ifndef _JOS_INTERNAL_H_
#define _JOS_INTERNAL_H_

#define JOS_IMPLEMENT
#include <jos.h>
#include <jutil.h>

struct driver_list_s;

struct jdevice_s {
    struct driver_list_s *jdrv;
    struct jdevice_s    *parent;
    void                *softc;
    jint_t               type;
    
    union {
        jbus_h          bus;
        void            *usbd_device;
    } bus_u;
    
    void            *ivars;
    
    juint16_t            unit;
    char                *nameunit;
    char                *desc;

    TAILQ_ENTRY(jdevice_s) next;
};

jresult_t jos_sync_init(void);
void jos_sync_uninit(void);

/* DMA block descriptor */
typedef struct dma_block_s {
    void        *paddr;
    void        *vaddr;
    juint32_t    size;
    juint16_t    align;
#define BLOCK_CACHABLE   0x01
#define BLOCK_PAGE_ALIGN 0x02
#define BLOCK_OWNER      0x04
#define BLOCK_DUMMY      0x08
    juint8_t     flags;
    void        *os_handle;
    LIST_ENTRY(dma_block_s) next;
} jdma_t;

jresult_t jos_mem_init(uw_args_t *args);
void jos_mem_uninit(void);
void jdma_free_all(void);

struct jtask_s {
    struct jtask_s      *next;
    jtime_t             timeout;
    jtask_type          type;
    jthread_func        cb;
    void                *arg;
#define JTASK_STOPPED           0
#define JTASK_RUNNING           1
#define JTASK_PENDING           2
#define JTASK_RUN_N_PEND        3
#define JTASK_STOPPING          4
    juint8_t             state;
    juint32_t            start_context;
#ifdef JDEBUG
    char                *file;
    int                 line;
#endif
};

#ifdef JDEBUG
void debug_init(void);
#endif

#endif

