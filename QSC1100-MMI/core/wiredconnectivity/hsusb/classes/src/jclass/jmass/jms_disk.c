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
#include <jos.h>
#include <jutil.h>
#include "jms_scsi.h"
#include <uw_host_mass.h>
#include "jms_device.h"
#include "jms_devmgr.h"

struct disk_handle_cb *upper_layer = NULL;

/* Register upper layer handler functions */
jresult_t register_disk_handler(struct disk_handle_cb *functions)
{
    LOCK_CHECK;

    if (!functions)
        return JEINVAL;

    if (upper_layer)
    {
        DBG_E(DMASS_DISK, ("DISK: Disk handler registration after " 
            "one was already registered \n"));
        return JEINVAL;
    }

    DBG_I(DMASS_DISK, ("DISK: Registering upper layer handler %p\n", 
        functions));

    upper_layer = functions;
    return 0;
}

void unregister_disk_handler(void)
{
    LOCK_CHECK;

    upper_layer = NULL;
}

jresult_t disk_attach(ms_device_t *device)
{
    jresult_t rc = 0;

    DBG_I(DMASS_DISK, ("DISK: Attach to upper %p\n", device));

    if (!upper_layer)
    {
        DBG_E(DMASS_DISK, ("DISK: No upper layer registered with disk "
            "driver\n"));
        return JEINVAL;
    }

    device->flags.media_inserted = TRUE;

    /* Attach to upper layer */
    rc = upper_layer->media_insert((void *)device, &device->upper_context);
    if (rc)
    {
        DBG_E(DMASS_DISK, ("DISK: Error attaching to upper layer %d.\n", rc));
        return rc;
    }
    device->flags.media_notify_done = TRUE;

#ifdef CONFIG_TEST_DICT
    jtest_dict_add(TEST_DEV_MEDIA, device);
#endif

    return 0;
}

void disk_detach(ms_device_t *device)
{
    DBG_I(DMASS_DISK, ("DISK: Detach from upper %p\n", device));

    if (!upper_layer)
    {
        DBG_E(DMASS_DISK, ("DISK: No upper layer registered with disk "
            "driver\n"));
        return;
    }

    if (!device->flags.media_notify_done)
    {
        DBG_E(DMASS_DISK, ("DISK: Trying to notify of media removal while "
            "no notification about media insertion was made !\n"));
        return;
    }

    device->flags.media_notify_done = FALSE;

#ifdef CONFIG_TEST_DICT
    jtest_dict_del(TEST_DEV_MEDIA, device);
#endif
    
    /* Detach from upper layer */
    upper_layer->media_remove((void *)device, device->upper_context);

    device->flags.media_inserted = FALSE;
}

jresult_t disk_get_media_info(void *dev, struct media_info *info)
{
    ms_device_t *device = dev;

    LOCK_CHECK;

    if (!device)
        return JEINVAL;

    if (!device->flags.media_inserted)
        return JENODEV;

    DBG_I(DMASS_DISK, ("DISK: Disk get media info:\n   Device name     : %s\n"
        "   Device type     : %d\n   Sector size     : %ld Bytes\n"
        "   Sector count    : %ld\n   Disk size       : %ldMB\n"
        "   Medium type     : %d\n   Write protected : %d\n", device->product,
        device->devtype, device->sector_size, device->sector_count,
        ((device->sector_count / 1024) * device->sector_size) / 1024,
        device->medium_type, device->flags.write_protected));

    info->sector_count  = device->sector_count;
    info->sector_size   = device->sector_size;
    info->medium_type   = device->medium_type;
    info->write_protect = (juint8_t)device->flags.write_protected;
    info->heads         = device->heads;
    info->sectors       = device->sectors;
    info->cylinders     = device->cylinders;

    return 0;
}

jresult_t disk_sync_transfer(void *dev, juint8_t *buffer, juint8_t flags, 
    juint32_t start_sector, juint32_t num_sectors, jint_t direction)
{
    ms_device_t *device = (ms_device_t *)dev;
    juint32_t    transfer_size;
    juint32_t    transferred;
    jresult_t    rc;

    LOCK_CHECK;

    DBG_V(DMASS_DISK, ("DISK: Transfer request !\n"));

    if (!device)
        return JEINVAL;

    if (!device->flags.media_inserted)
        return JENODEV;

    if (device->flags.pending_removal || device->flags.suspended)
        return JEPERM;

    device_stop_polling(device);

    /* Prevent device removal */
    device->remove_lock++;

    transferred = transfer_size = num_sectors * device->sector_size;

    rc = scsi_read_write(device, start_sector, num_sectors,
        buffer, flags, &transferred, direction);

    if (transfer_size != transferred)
    {
        DBG_W(DMASS_DISK, ("DISK: Short transfer, error reported to upper "
            "layer\n"));
        rc = JEIO;
    }

    /* Restart media polling, if there was an error, poll immediately */
    device_start_polling(device, rc ? 1 : 0);

    /* Allow device removal */
    device->remove_lock--;

    return rc;
}

jresult_t disk_suspend(void *dev) 
{
    ms_device_t *device = (ms_device_t *)dev;
    jresult_t rc;

    LOCK_CHECK;

    if (!dev)
        return JEINVAL;

    if (device->flags.suspended)
        return JEINVAL;

    DBG_I(DMASS_POWER, ("DISK: Selective suspend on %s\n", device->product));

    CORE_SAFE_ENTER;
    rc = jms_devmgr_suspend_device(device);
    CORE_SAFE_LEAVE;

    return rc;
}

jresult_t disk_resume(void *dev) 
{
    ms_device_t *device = (ms_device_t *)dev;
    jresult_t rc;

    LOCK_CHECK;

    if (!dev)
        return JEINVAL;

    if (!device->flags.suspended)
        return JEINVAL;

    DBG_I(DMASS_POWER, ("DISK: Selective resume on %s\n", device->product));

    CORE_SAFE_ENTER;
    rc = jms_devmgr_resume_device(device);
    CORE_SAFE_LEAVE;

    return rc;
}

