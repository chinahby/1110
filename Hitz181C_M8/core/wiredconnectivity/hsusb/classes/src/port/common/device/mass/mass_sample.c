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
#include <uw_device_mass.h>

/* SCSI-2 device types */
#define SCSI_DEVICE_DIRECT_ACCESS       0x00
#define SCSI_DEVICE_SEQUENTIAL_ACCESS   0x01
#define SCSI_DEVICE_PRINTER             0x02
#define SCSI_DEVICE_PROCESSOR           0x03
#define SCSI_DEVICE_WRITE_ONCE          0x04
#define SCSI_DEVICE_CDROM               0x05
#define SCSI_DEVICE_SCANNER             0x06
#define SCSI_DEVICE_OPTICAL             0x07
#define SCSI_DEVICE_MEDIUM_CHANGER      0x08
#define SCSI_DEVICE_COMM                0x09
#define SCSI_DEVICE_UNKNOWN             0x1F

/* Device specific properties */
#define RELEASE                 "1.00"

#define KILOBYTE                1024UL
#define MEGABYTE                (KILOBYTE * 1024)
#define MEMORY_BLOCK_SIZE       512

#ifdef CONFIG_RAMDISK_FILE

#define DEVICE_TYPE             SCSI_DEVICE_CDROM
#define MEDIA_REMOVABLE         1
#define MEDIA_READONLY          1
#define VENDOR_ID               "Jungo"
#define PRODUCT_ID              "CDROM"
#define DEVICE_COUNT            1
#define DEVICE_LUNS             1
extern juint32_t mem_buffer_size;
extern juint8_t *uwd_get_msd_ramdisk(juint8_t dev_index, juint8_t vlun);
#else
#define DEVICE_TYPE             SCSI_DEVICE_DIRECT_ACCESS
#define MEDIA_REMOVABLE         1
#define MEDIA_READONLY          0
#define VENDOR_ID               "Jungo"
#define PRODUCT_ID              "Mass Storage FD"
/* Sample memory buffer */
# define DEVICE_COUNT           CONFIG_FD_MASS

#ifdef CONFIG_DEVICE_LUNS
#define DEVICE_LUNS             CONFIG_DEVICE_LUNS
#else
#define DEVICE_LUNS             1 
#endif

#define DEVICE_CAPACITY         4 /* In MegaBytes */
#define MEMORY_BUFFER_SIZE      (MEGABYTE * DEVICE_CAPACITY)
static juint8_t mem_buffer[DEVICE_COUNT][DEVICE_LUNS][MEMORY_BUFFER_SIZE];
static juint32_t mem_buffer_size = MEMORY_BUFFER_SIZE;

juint8_t *uwd_get_msd_ramdisk(juint8_t dev_index, juint8_t vlun)
{
    return mem_buffer[dev_index][vlun];
}
#endif

static jbool_t dev_index_pool[DEVICE_COUNT];

/* Keep track of LUN status, used to simulate removable media */
typedef struct {
    jbool_t inserted;
    jbool_t prev;
    completion_callback cb;
    void *context;
#ifdef CONFIG_FD_MASS_TEST
    jtask_h task_h;
#endif
} lun_status_t;

static lun_status_t lun_status[DEVICE_COUNT][DEVICE_LUNS];

static jresult_t vallow_cache(msfd_appctx_t ctx, juint8_t vlun, juint8_t flag)
{
    return 0;
}

static jresult_t vflush(msfd_appctx_t ctx, juint8_t vlun)
{
    return 0;
}

static jbool_t vready(msfd_appctx_t ctx, juint8_t vlun)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);

    return lun_status[dev_index][vlun].inserted;
}

static jbool_t vchanged(msfd_appctx_t ctx, juint8_t vlun)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);

    if (lun_status[dev_index][vlun].inserted !=
        lun_status[dev_index][vlun].prev)
    {
        lun_status[dev_index][vlun].prev =
            lun_status[dev_index][vlun].inserted;

        DBG_I(DSLAVE_MS_API, ("APP: Changed %d : %d - %s.\n",
            dev_index, vlun, lun_status[dev_index][vlun].inserted ?
            "inserted" : "removed"));
        return 1;
    }
    return 0;
}

static jresult_t vmount(msfd_appctx_t ctx, juint8_t vlun)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);

    DBG_E(DSLAVE_MS_API, ("APP: Mount dev %d, LUN %d.\n", dev_index, vlun));
    if (lun_status[dev_index][vlun].inserted)
        return JEINVAL;

    lun_status[dev_index][vlun].inserted = 1;
    return 0;
}

static jresult_t vunmount(msfd_appctx_t ctx, juint8_t vlun)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);

    DBG_E(DSLAVE_MS_API, ("APP: Un-mount dev %d, LUN %d.\n", dev_index, vlun));
    if (!lun_status[dev_index][vlun].inserted)
        return JEINVAL;

    lun_status[dev_index][vlun].inserted = 0;
    return 0;
}

static jresult_t vopen(msfd_appctx_t ctx, juint8_t vlun, juint8_t *removable, 
    completion_callback callback, void *arg)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);

#ifdef CONFIG_FD_MASS_TEST
    lun_status[dev_index][vlun].inserted = CONFIG_FD_MASS_TEST;
#else
    lun_status[dev_index][vlun].inserted = 1;
#endif
    lun_status[dev_index][vlun].prev     = lun_status[dev_index][vlun].inserted;
    lun_status[dev_index][vlun].cb       = callback;
    lun_status[dev_index][vlun].context  = arg;
    
    *removable  = MEDIA_REMOVABLE;

    return 0;
}

static jresult_t vget_media_details(msfd_appctx_t ctx, juint8_t vlun,
    juint32_t *block_size, juint32_t *num_blocks, juint32_t *read_only)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);
    
    if (!lun_status[dev_index][vlun].inserted)
        return JEINVAL;

    *block_size = MEMORY_BLOCK_SIZE;
    *num_blocks = mem_buffer_size / MEMORY_BLOCK_SIZE;
    *read_only = MEDIA_READONLY;

    return 0;
}

static jresult_t vclose(msfd_appctx_t ctx, juint8_t vlun)
{
    return 0;
}

static jresult_t vread(msfd_appctx_t ctx, juint8_t vlun, juint8_t *buffer,
    juint32_t amount, juint32_t offset)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);
    juint8_t *mem_buffer = uwd_get_msd_ramdisk(dev_index, vlun);

    DBG_I(DSLAVE_MS_API, ("APP: ### Read %u starting from %u, dev %d, "
        "LUN %d.\n", amount, offset, dev_index, vlun));

    if ((offset + amount) * MEMORY_BLOCK_SIZE > mem_buffer_size)
    {
        DBG_E(DSLAVE_MS_API, ("APP: Bad read parameters.\n"));
        return JEINVAL;
    }
    j_memcpy(buffer, &mem_buffer[offset * MEMORY_BLOCK_SIZE],
        amount * MEMORY_BLOCK_SIZE);

    if (lun_status[dev_index][vlun].cb)
    {
        lun_status[dev_index][vlun].cb(lun_status[dev_index][vlun].context, 
            TRUE);
    }
  
    return 0;
}

static jresult_t vwrite(msfd_appctx_t ctx, juint8_t vlun, juint8_t *buffer,
    juint32_t amount, juint32_t offset)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);
    juint8_t *mem_buffer = uwd_get_msd_ramdisk(dev_index, vlun);

    DBG_I(DSLAVE_MS_API, ("APP: ### Write %u starting from %u, dev %d, "
        "LUN %d.\n", amount, offset, dev_index, vlun));

    if ((offset + amount) * MEMORY_BLOCK_SIZE > mem_buffer_size)
    {
        DBG_E(DSLAVE_MS_API, ("APP: Bad write parameters.\n"));
        return JEINVAL;
    }
    j_memcpy(&mem_buffer[offset * MEMORY_BLOCK_SIZE], buffer, 
        amount * MEMORY_BLOCK_SIZE);

    if (lun_status[dev_index][vlun].cb)
    {
        lun_status[dev_index][vlun].cb(lun_status[dev_index][vlun].context, 
            TRUE);
    }
    return 0;
}

static jint32_t vverify(msfd_appctx_t ctx, juint8_t vlun, juint32_t amount, 
    juint32_t offset)
{
#ifdef JDEBUG
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);
#endif

    DBG_I(DSLAVE_MS_API, ("APP: ### Verify %u starting from %u, dev %d, "
        "LUN %d\n", amount, offset, dev_index, vlun));

    if ((offset + amount) * MEMORY_BLOCK_SIZE > mem_buffer_size)
    {
        DBG_E(DSLAVE_MS_API, ("APP: Bad read parameters\n"));
        return JEINVAL;
    }
  
    return amount;
}

static void vcancel(msfd_appctx_t ctx, juint8_t vlun)
{
    DBG_E(DSLAVE_MS_API, ("APP: Cancelling request.\n"));
}

static jresult_t vlun_count(msfd_appctx_t ctx, juint8_t *count)
{
    *count = DEVICE_LUNS;
    return 0;
}

static void vget_desc(msfd_appctx_t ctx, juint8_t vlun, juint32_t *dev_type, 
    char **vendor, char **product, char **revision)
{
    *dev_type   = DEVICE_TYPE;
    *vendor     = VENDOR_ID;
    *product    = PRODUCT_ID;
    *revision   = RELEASE;
}

#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
static jresult_t vtransparent(msfd_appctx_t ctx, juint8_t vlun, void *cmd)
{
    return 0;
}
#endif

#ifdef CONFIG_FD_MASS_TEST
static void media_change_event(void *arg)
{    
    static jint_t i = 0;
    jtask_h task_h = (jtask_h)arg;
    
    KASSERT(task_h != NULL,("media_change_event: No jtask_h \n"));        

    lun_status[0][0].inserted = !lun_status[0][0].inserted;

    DBG_I(DSLAVE_MS_API, ("TEST: media_change_event - media %s.\n",
        lun_status[0][0].inserted ? "inserted" : "removed"));

    if (i < 3)
    {
        jtask_schedule(task_h, 20000, media_change_event, task_h);      
        i++;
    }
}
#endif

static jresult_t msfd_app_uninit(msfd_appctx_t ctx)
{
    juint8_t dev_index = (juint8_t)((juint32_t)ctx);
    
    dev_index_pool[dev_index] = 0;

#ifdef CONFIG_FD_MASS_TEST
    jtask_uninit(lun_status[dev_index][0].task_h);
#endif
    return 0;
}

jresult_t msfd_app_init(msfd_ops_t *ops, msfd_appctx_t *ctx, 
    msfd_init_info_t *info)
{
    juint8_t dev_index;

#ifdef CONFIG_FD_MASS_TEST
    jresult_t rc;
#endif

#ifdef CONFIG_TEST_STATIC_INTERFACE  
    /* TODO: If the application wants to set the interface number it should be
     * done here */
    info->interface_number = 5;
#endif

#ifdef CONFIG_FD_MASS_SCSI_BUFF_SIZE
    info->scsi_buffer_size = CONFIG_FD_MASS_SCSI_BUFF_SIZE;
#endif

    /* Find next availble DISK */
    for (dev_index = 0; dev_index < DEVICE_COUNT; dev_index++)
    {
        if (!dev_index_pool[dev_index])
            break;
    }

    if (dev_index == DEVICE_COUNT)
    {
        DBG_E(DSLAVE_MS_API, ("APP: There is no available DISK.\n"));
        return JEINVAL;
    }
    
    dev_index_pool[dev_index] = 1;
    *((juint32_t*)ctx) = dev_index;

    /* The following APIs are required */
    ops->vlun_count          = vlun_count;
    ops->vget_desc           = vget_desc;
    ops->vready              = vready;
    ops->vchanged            = vchanged;
    ops->vmount              = vmount;
    ops->vunmount            = vunmount;
    ops->vget_media_details  = vget_media_details;
    ops->vopen               = vopen;
    ops->vclose              = vclose;
    ops->vread               = vread;
    ops->vwrite              = vwrite;
    ops->vverify             = vverify;
    ops->vcancel             = vcancel;
    ops->vflush              = vflush;
    ops->vallow_cache        = vallow_cache;
    /* The following APIs are optional */
#ifdef CONFIG_SCSI_TRANSPARENT_COMMAND
    ops->vtransparent        = vtransparent;
#endif
    ops->msfd_app_uninit     = msfd_app_uninit;

#ifdef CONFIG_FD_MASS_TEST
    rc = jtask_init(&lun_status[dev_index][0].task_h, TASK_CLASS_DRIVER);
    if (!rc)
    {
        jtask_schedule(lun_status[dev_index][0].task_h, 5000, 
            media_change_event, lun_status[dev_index][0].task_h);
    }
    else
    {
        DBG_E(DSLAVE_MS_API, ("os_timer_init: Timer initialization failed\n"));
        return rc;
    }

#endif

    return 0;
}

