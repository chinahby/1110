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
#include "jms_device.h"
#include "jms_scsi.h"
#include <uw_host_mass.h>
#include <jutil.h>

#define USBMSC_SUBCLASS_RBC         0x01
#define USBMSC_SUBCLASS_SFF8020i    0x02
#define USBMSC_SUBCLASS_QIC157      0x03
#define USBMSC_SUBCLASS_UFI         0x04
#define USBMSC_SUBCLASS_SFF8070i    0x05
#define USBMSC_SUBCLASS_SCSI        0x06

#define UNKNOWN_VENDOR  "unknown"
#define UNKNOWN_PRODUCT "unknown"

jresult_t get_medium_info(ms_device_t *device)
{
    jresult_t rc = 0;

    LOCK_CHECK;

    DBG_I(DMASS_DEVICE, ("DEVICE: Get medium info.\n"));
    if (device->devtype == SCSI_DEVICE_UNKNOWN || 
        device->medium_type == SCSI_MEDIUM_UNKNOWN)
    {
        rc = scsi_unit_attention(device);
        if (rc)
            goto Exit;
    }

    /* Try SENSE6 and if fails SENSE10 */
    if (device->devtype != SCSI_DEVICE_UNKNOWN &&
        device->medium_type == SCSI_MEDIUM_UNKNOWN)
    {
        rc = scsi_mode_sense_6(device);
        if (rc == JENXIO)
        {
            DBG_E(DMASS_DEVICE, ("DEVICE: Cannot communicate with device\n"));
            goto Exit;
        }

        if (rc)
        {
            DBG_I(DMASS_DEVICE, ("DEVICE: Mode Sense 6 didn't work, trying "
                "10.. \n"));
            rc = scsi_mode_sense_10(device);
            if (rc)
                goto Exit;
        }
    }

    /* Get media size/geometry */
    if (device->flags.medium_present)
    {
        rc = scsi_read_capacity(device);
        if (rc)
            goto Exit;

        /* Read from Partion table the 
         * cylinders , heads, and sectors information*/
        rc = scsi_read_partsize(device);
        if (rc)
            goto Exit;
    }

Exit:

    /* HSU temp fix - PRINT_DBG is no longer defined since it increases the size
    of the image dramatically */
    /* old code: PRINT_DBG(DMASS_DEVICE, rc ? DL_ERROR : DL_VERBOSE,
        ("DEVICE: Finished get media info %d\n", rc)); */
    if(rc)
    {
      DBG_E(DMASS_DEVICE, ("DEVICE: Finished get media info %d\n", rc));
    }
    else
    {
      DBG_V(DMASS_DEVICE, ("DEVICE: Finished get media info %d\n", rc));
    }
    /* HSU -  end of temp fix */
    return rc;
}

static void test_media_status(void *arg)
{
    ms_device_t *device = (ms_device_t *)arg;
    jint_t need_attach, need_detach;
    jresult_t rc = 0;

    DBG_V(DMASS_DEVICE, ("DEVICE: Test media status %s \n", device->vendor));

    device->flags.media_polling_sched = 0;

    /* Are we detaching ? */
    if (device->flags.pending_removal)
        return;

    device->flags.media_polling_running = 1;

    rc = scsi_test_unit_ready(device);

    if (rc && rc != JENOMEDIUM)
    {
        DBG_W(DMASS_DEVICE, ("DEVICE: Poll method reports device error %s "
            "[%d]\n", device->vendor, rc));

        if (rc == JENXIO)
        {
            DBG_E(DMASS_DEVICE, ("DEVICE: Cannot communicate with device\n"));
            device->flags.media_polling_running = 0;
            return;
        }

        /* TODO: We might want to try some device recovery here */

        goto PollAgain;
    }

    if (!device->flags.medium_changed)
        goto PollAgain;

    DBG_V(DMASS_DEVICE, ("DEVICE: Medium in device %s changed\n",
        device->vendor));

    need_attach = 0;
    need_detach = 0;

    /* Media was replaced */
    if (device->flags.medium_present &&
        device->flags.media_notify_done)
    {
        DBG_I(DMASS_DEVICE, ("DEVICE: Medium in drive changed\n"));
        need_detach = 1;
        need_attach = 1;
    }
    /* New media inserted */
    else if (device->flags.medium_present)
    {
        DBG_I(DMASS_DEVICE, ("DEVICE: Medium inserted\n"));
        need_attach = 1;

    }
    /* Media inserted and removed before we had time to cope */
    else if (!device->flags.medium_present &&
        !device->flags.media_notify_done)
    {
        DBG_I(DMASS_DEVICE, ("DEVICE: Medium removed, previous state "
            "unknown\n"));
    }
    /* Media removed */
    else
    {
        DBG_I(DMASS_DEVICE, ("DEVICE: Medium removed\n"));
        need_detach = 1;
    }

    device->flags.medium_changed = FALSE;

    if (need_detach && device->detach)
        device->detach(device);

    if (need_attach && device->attach)
    {
        rc = get_medium_info(device);
        if (rc)
            goto PollAgain;

        rc = device->attach(device);
        if (rc)
        {
            DBG_E(DMASS_DEVICE, ("DEVICE: Error while attaching %d\n", rc));
            goto PollAgain;
        }
    }

PollAgain:

    DBG_V(DMASS_DEVICE, ("DEVICE: Continuing to poll...\n"));

    device->flags.media_polling_running = 0;
    /* Check if detach happened while we were checking for media presense */
    if (device->flags.pending_removal)
        return;

    /* Continue polling for changes */
    device_start_polling(device, 0);
}

ms_device_t *device_init(void *transport, void *usbd_dev, juint8_t lun, 
    juint8_t subclass, void *devmgr)
{
    ms_device_t *device = NULL;
    jresult_t rc = 0;

    DBG_I(DMASS_DEVICE, ("DEVICE: Init device. Lun %d Subclass %d\n", lun, 
        subclass));

    switch (subclass)
    {
    case USBMSC_SUBCLASS_RBC:
    case USBMSC_SUBCLASS_SFF8020i:
    case USBMSC_SUBCLASS_QIC157:
    case USBMSC_SUBCLASS_UFI:
    case USBMSC_SUBCLASS_SFF8070i:
    case USBMSC_SUBCLASS_SCSI:
        break;
    default:
        DBG_E(DMASS_DEVICE, ("DEVICE: Unsupported sub class :%x\n", subclass));
        rc = JEINVAL;
        goto Error;
    }

    if (!transport)
    {
        DBG_E(DMASS_DEVICE, ("Device: No transport !\n"));
        rc = JEINVAL;
        goto Error;
    }

    device = jmalloc(sizeof(ms_device_t), 0);
    if (!device)
    {
        DBG_E(DMASS_DEVICE, ("Device: Cannot alloc memory for device.\n"));
        rc = JENOMEM;
        goto Error;
    }

    /* Prevent device removal */
    device->remove_lock = 1;

    device->usbd_dev    = usbd_dev;
    device->transport   = transport;
    device->subclass    = subclass;
    device->lun         = lun; 
    device->medium_type = SCSI_MEDIUM_UNKNOWN;
    device->devtype     = SCSI_DEVICE_UNKNOWN;
    device->devmgr      = devmgr;

    j_strlcpy((void *)device->vendor, UNKNOWN_VENDOR, VENDOR_ID_LEN);
    j_strlcpy((void *)device->product, UNKNOWN_PRODUCT, PRODUCT_ID_LEN);

    device->release     = 0x0;

    device->flags.pending_removal       = FALSE;
    device->flags.suspended             = FALSE;
    device->flags.medium_present        = TRUE; 
    device->flags.medium_changed        = TRUE;
    device->flags.media_notify_done     = FALSE;
    device->flags.media_polling_sched   = FALSE;
    device->flags.media_polling_running = FALSE;
    device->flags.media_inserted        = FALSE;

    /* Give the device a little time to settle down */
    jdelay_ms(500);

    rc = scsi_inquiry(device);
    if (rc)
        goto Error;

    if (device->flags.removable)
    {
        device->flags.medium_present    = FALSE; 
        rc = jtask_init(&device->media_poll_task, TASK_CLASS_DRIVER);
        if (rc)
            goto Error;
    }

    switch (device->devtype)
    {
    case SCSI_DEVICE_DIRECT_ACCESS:
    case SCSI_DEVICE_CDROM:
        device->attach = disk_attach;
        device->detach = disk_detach;
        break;
    case SCSI_DEVICE_SEQUENTIAL_ACCESS:
    case SCSI_DEVICE_PRINTER:
    case SCSI_DEVICE_PROCESSOR:
    case SCSI_DEVICE_WRITE_ONCE:
    case SCSI_DEVICE_SCANNER:
    case SCSI_DEVICE_OPTICAL:
    case SCSI_DEVICE_MEDIUM_CHANGER:
    case SCSI_DEVICE_COMM:
    case SCSI_DEVICE_UNKNOWN:
    default:
        DBG_E(DMASS_DEVICE, ("DEVICE: Unknown device type %x\n", 
            device->devtype));
        rc = JEINVAL;
        goto Error;
    }

    /* Notify upper layer about the new device */
    if (upper_layer && upper_layer->device_attach)
    {
        rc = upper_layer->device_attach(device, &device->upper_context);
        if (rc)
        {
            DBG_E(DMASS_DEVICE, ("DEVICE: Failed attach to upper layer\n"));
            goto Error;
        }
    }

#ifdef CONFIG_TEST_DICT
    jtest_dict_add(TEST_DEV_MASS, device);
#endif

    /* Check if there is media present */
    if (device->flags.removable)
    {
        device_start_polling(device, 1);
    }
    else
    {
        rc = get_medium_info(device);
        if (rc)
            goto Error;

        rc = device->attach(device);
        if (rc)
            goto Error;
    }

    /* Allow device removal */
    device->remove_lock--;

    DBG_I(DMASS_DEVICE, ("DEVICE: Finished device init [%p] which is %s\n", 
        device, device->flags.removable ? "removable" : "non removable"));

    return device;

Error: 
#ifdef CONFIG_TEST_DICT
    jtest_dict_del(TEST_DEV_MASS, device);
#endif

    if (device)
        jfree(device);

    DBG_E(DMASS_DEVICE, ("DEVICE: Finished device init %d\n", rc));
    return NULL;
}

/* Delete device, stop activity and report to upper layer */
void device_uninit(ms_device_t *device)
{
    DBG_I(DMASS_DEVICE, ("DEVICE: Un-Init device. %s\n", device->vendor));

    if (!device)
    {
        DBG_E(DMASS_DEVICE, ("DEVICE: Invalid device passed to uninit\n"));
        return;
    }

    device->flags.pending_removal = TRUE;

    /* Stop the media polling task */
    device_stop_polling(device);

    if (device->flags.media_notify_done)
        device->detach(device);

    while (device->remove_lock)
    {
        DBG_I(DMASS_DEVICE, ("DEVICE: [%s] Waiting for device remove_lock "
            "to get free.. [%d]\n", device->vendor, device->remove_lock));
        jdelay_ms(500);
    }

#ifdef CONFIG_TEST_DICT
    jtest_dict_del(TEST_DEV_MASS, device);
#endif
    
    /* Notify upper layer that the device was detached */
    if (upper_layer && upper_layer->device_detach)
        upper_layer->device_detach(device, device->upper_context);

    if (device->flags.removable)
        jtask_uninit(device->media_poll_task);

    jfree(device);
}

void *disk_get_usbd_handle(void *dev)
{
    ms_device_t *device = (ms_device_t *)dev;

    if (!dev)
    {
        DBG_E(DMASS_DEVICE, ("disk_get_usbd_handle: invalid device handle\n"));
        return NULL;
    }        
    return device->usbd_dev;
}

jresult_t disk_get_device_info(void *dev, struct device_info *info)
{
    ms_device_t *device = dev;

    if (!device)
        return JEINVAL;

    DBG_I(DMASS_DISK, ("DISK: Disk get device info:\n   Vendor          : %s\n"
        "   Product         : %s\n   Release         : %x\n"
        "   Sub class       : %ld\n   Device type     : %ld\n"
        "   Device LUN      : %ld\n   Removable       : %s\n", device->vendor, 
        device->product, device->release, device->subclass, 
        device->devtype, device->lun, device->flags.removable ? "Yes" : "No")); 

    info->subclass      = device->subclass;
    info->devtype       = device->devtype;
    info->lun           = device->lun;
    info->vendor        = device->vendor;
    info->product       = device->product;
    info->release       = device->release;
    info->removable     = (juint8_t)device->flags.removable;

    return 0;
}

jresult_t device_suspend(ms_device_t *device)
{
    DBG_I(DMASS_POWER, ("DEVICE: Suspend request %s\n", device->vendor));

    if (!device)
    {
        DBG_I(DMASS_POWER, ("DEVICE: Invalid device passed to suspend\n"));
        return JEINVAL;
    }

    /* Stop the media polling task */
    device_stop_polling(device);

    /* Make sure all transfers are stopped */
    while (device->remove_lock)
    {
        DBG_W(DMASS_POWER, ("DEVICE: Waiting for %d transfers to stop "
            "to enter suspend state\n", device->remove_lock));
        jdelay_ms(50);
    }

    device->flags.suspended = TRUE;

#ifdef CONFIG_TEST_DICT
    jtest_dict_state_set(TEST_DEV_MASS, device, TEST_DEV_STATE_SUSPENDED);
    if (device->flags.media_notify_done)
        jtest_dict_state_set(TEST_DEV_MEDIA, device, TEST_DEV_STATE_SUSPENDED);
#endif
    
    return 0;
}

jresult_t device_resume(ms_device_t *device)
{
    DBG_I(DMASS_POWER, ("DEVICE: Resume request %s\n", device->vendor));

    if (!device)
    {
        DBG_E(DMASS_POWER, ("DEVICE: Invalid device passed to resume\n"));
        return JEINVAL;
    }

    if (!(device->flags.suspended))
    {
        DBG_E(DMASS_POWER, ("DEVICE: Resuming a non suspended device %d\n",
            device->vendor));
        return JEINVAL;
    }

    /* Restart media polling */
    device_start_polling(device, 1);

    device->flags.suspended = FALSE;

#ifdef CONFIG_TEST_DICT
    jtest_dict_state_set(TEST_DEV_MASS, device, TEST_DEV_STATE_NORMAL);
    if (device->flags.media_notify_done)
        jtest_dict_state_set(TEST_DEV_MEDIA, device, TEST_DEV_STATE_NORMAL);
#endif

    return 0;
}

void device_start_polling(ms_device_t *device, jbool_t immediately)
{
    KASSERT(device, ("DEVICE: No device given to device_start_polling\n"));
    
    if (!device->flags.removable)
        return;
   
    device->flags.media_polling_sched = 1;

    jtask_schedule(device->media_poll_task, 
        immediately ? 0 : MEDIA_POLL_INTERVAL, test_media_status, device);
}

void device_stop_polling(ms_device_t *device)
{
    KASSERT(device, ("DEVICE: No device given to device_stop_polling\n"));
    
    if (!device->flags.media_polling_sched || 
        device->flags.media_polling_running)
    {
        return;
    }
    
    if (!device->flags.removable)
        return;
    
    device->flags.media_polling_sched = 0;
    
    jtask_stop(device->media_poll_task);
}

