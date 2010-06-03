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
#ifndef _JMS_DEVICE_H_
#define _JMS_DEVICE_H_

#include <jos.h>
#include "jms_scsi.h"
#include <uw_host_mass.h>

#define MEDIA_POLL_INTERVAL     2000 /* 2 seconds */

/* Create a new device (disk/cd/etc) and report to upper layer */
ms_device_t *device_init(void *transport, void *usbd_dev, juint8_t lun, 
    juint8_t subclass, void *devmgr);

/* Delete device, stop activity and report to upper layer */
void device_uninit(ms_device_t *device);

/* Device power magement */
jresult_t device_suspend(ms_device_t *device);
jresult_t device_resume(ms_device_t *device);

/* Media polling management */
void device_start_polling(ms_device_t *device, jbool_t immediately);
void device_stop_polling(ms_device_t *device);

/* List of supported device types */
jresult_t disk_attach(ms_device_t *device);
void disk_detach(ms_device_t *device);

/* Pointer to upper layer above mass storage */
extern struct disk_handle_cb *upper_layer;

#endif

