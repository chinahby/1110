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
#ifndef _UW_HOST_MASS_H_
#define _UW_HOST_MASS_H_

#include "jtypes.h"

/* For use by upper layer */    
struct media_info {
    juint32_t    sector_count;
    juint32_t    sector_size;
    juint8_t     medium_type;
    juint8_t     write_protect;
    juint8_t     heads;
    juint8_t     sectors;
    juint32_t    cylinders;
};

struct device_info {
    juint8_t     subclass;
    juint8_t     devtype;
    juint8_t     lun;
    juint8_t     *vendor;  /* ASCI vendor ID */
    juint8_t     *product; /* ASCI product ID */
    juint16_t    release;
    juint8_t     removable;
};

#define TO_DEVICE       0
#define FROM_DEVICE     1

jresult_t disk_sync_transfer(void *dev, juint8_t *buffer, juint8_t flags,
    juint32_t start_sector, juint32_t num_sectors, jint_t direction);

jresult_t disk_get_media_info(void *dev, struct media_info *pinfo);

jresult_t disk_get_device_info(void *dev, struct device_info *pinfo);

void *disk_get_usbd_handle(void *dev);

jresult_t disk_suspend(void *dev);
jresult_t disk_resume(void *dev);

/* Implemented by upper layer */

/* NOTE: Before returning, detach must verify 
 *    that all transfers submitted have finished.
 * */ 
struct disk_handle_cb {
    /* Notify about media insertion and removal */
    jresult_t (*media_insert)(void *dev, void **context);
    void (*media_remove)(void *dev, void *context);

    /* Optional callbacks, will notify when a device is connected
     * before any media exploring is performed */
    jresult_t (*device_attach)(void *dev, void **context);
    void (*device_detach)(void *dev, void *context);
 };

/* OS/Filesystem must call this function on init, to let the
 * disk layer know who to report disk operations to. */
jresult_t register_disk_handler(struct disk_handle_cb *functions);
void unregister_disk_handler(void);

#ifdef CONFIG_JUMASS_ASYNC
/* Number of threads used for async operation */
#define JMS_ASYNC_THREADS 2

/* Callback provided to async transfer for completion notification */
typedef void (*disk_async_cb)(void *arg, jresult_t status);

/* Asynchronous disk transfer initiator */
jresult_t disk_async_transfer(void *dev, disk_async_cb callback, void *arg,
    juint8_t *buffer, juint8_t flags, 
    juint32_t start_sector, juint32_t num_sectors, juint_t direction);
#endif

#endif

