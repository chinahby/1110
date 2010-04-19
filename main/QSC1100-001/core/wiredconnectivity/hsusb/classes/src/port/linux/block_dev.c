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
#include <uw_host_mass.h>
#include <devfs_fs_kernel.h>

/* Max 2^4 partitions per disk */
#define MINOR_SHIFT     4       
#define DISK_SHIFT      (8 - MINOR_SHIFT)

/* Max 16 disks (More majors might be better) */
#define MAX_DISKS       (1 << DISK_SHIFT)
#define MAX_MINORS      (MAX_DISKS << MINOR_SHIFT)
#define MAX_PARTITIONS  (1 << MINOR_SHIFT)

/* array of "disk hosts numbers" - used for assigning id's without colision */
static char jms_hostv[MAX_DISKS];

/* Also DevFS entry /dev/jms */
#define DEVICE_NAME     "jms"   
#define MAJOR_NR        253

#define KERNEL_SECTOR_SIZE 512
#define MAX_SECTORS_PER_REQUEST 64

enum disk_states {
    STATE_UNUSED = 0,   /* Disk entry not in use */
    STATE_INUSE,        /* Disk attached and usable */
    STATE_DETACHED      /* Disk removed, waiting for locks to get free */
};

#include <blkpg.h>
#include <blkdev.h>
#include <hdreg.h>

int jos_block_attach_cb(void *dev, void **user_ctx);
void jos_block_detach_cb(void *dev, void *user_ctx);

/* Interface to disk layer */
struct disk_handle_cb disk_functions = {
    jos_block_attach_cb, 
    jos_block_detach_cb, 
};

struct jms_dev_t {
    jint8_t              disk_id;
    jint8_t              usage;          /* Number of Open()s on the device */
    jint8_t              xfers;          /* Number of pending usb xfers */
    spinlock_t          lock;

    void                *dev;
    struct media_info   di;

    enum                disk_states state;

    struct gendisk      *jgd;
};

static struct block_device_operations jms_fops;

static void jms_request(request_queue_t *q)
{
    struct jms_dev_t *jdev;
    struct request *rq;
    struct bio *bio;
    struct bio_vec *bvec;
    juint8_t dir, *buf, *seg_buf;
    juint32_t nbytes, start_sector, num_sectors, xfer_num_sectors = 0;
    int rc, i, continuity = 0;
    nbytes = 0;
    
    jsafe_enter();

    /* Go over all segments within all bios of each request.
     * For each segment check continuity.
     * break when current segment isn't 
     * continuous with previous ones,
     * or when data direction is changed 
     * or when finished all data
     * and send data to lower USB layer.
     * */
    if (!q || !q->queuedata)
    {
        DBG_E(DPORT_BLOCK, ("LB: Request - calling request with invalid "
            "queue %p\n",q));
        return;        
    }
    
    jdev = q->queuedata;
    
    while ((rq = elv_next_request(q)) != NULL)
    {
        if (!rq || !rq->rq_disk || !rq->rq_disk->private_data)
        {
            DBG_E(DPORT_BLOCK, ("LB: Request - calling request with invalid "
                "queue %p\n",q));
            goto Exit;
        }
        if (!blk_fs_request(rq))
        {
            DBG_E(DPORT_BLOCK, ("LB: Error processing request %p - not fs "
                "request\n",rq));
            end_request(rq, 0);
            continue;
        }
        
        jdev = rq->rq_disk->private_data;
        rq_for_each_bio(bio, rq)
        {
            bio_for_each_segment(bvec, bio, i)
            {
                seg_buf = bio_data(bio);
                if (!continuity)
                {
                    
                    /* will reach here if starting "fresh" */
                    continuity = 1;
                    buf = seg_buf;
                    start_sector = bio->bi_sector;
                    nbytes = 0;
                    dir = bio_data_dir(bio);
                }
                if (((seg_buf - buf) != nbytes) || (bio_data_dir(bio) != dir))
                {
                    /* continuity broke */
                    if (!xfer_num_sectors)
                    {
                        DBG_E(DPORT_BLOCK, ("LB: Zero length request [%p]",rq));
                        continuity = 0;
                        continue;
                    }
                    /* inside a request, continuity broke
                     * need to send the data */
                    goto end_of_request;
                }
                else
                {
                    /* still continuous */
                    nbytes += bio->bi_size;
                    num_sectors = bio->bi_size/KERNEL_SECTOR_SIZE;
                    xfer_num_sectors += num_sectors;
                }
                /* next segment */
            }
            /* next bio */
        }
end_of_request:
        rc = disk_sync_transfer(jdev->dev, buf, 0, start_sector, 
            xfer_num_sectors, (dir == READ) ? FROM_DEVICE : TO_DEVICE);

        if (!end_that_request_first(rq, (rc) ? 0 : 1, xfer_num_sectors)) 
        {
            jsafe_leave();
            {
                blkdev_dequeue_request(rq);
                end_that_request_last(rq);
            }
            jsafe_enter();
        }
        xfer_num_sectors = 0;
        nbytes = 0;
        continuity = 0;
    }

Exit:
    jsafe_leave();
}
        
static int jms_revalidate(struct gendisk *disk)
{
    struct jms_dev_t *jdev;
    
    if (!disk || !disk->private_data)
    {
        DBG_E(DPORT_BLOCK, ("LB: Revalidate - invalid disk\n"));
        return -JEINVAL;
    }
    jdev = disk->private_data;
    blk_queue_hardsect_size(disk->queue, 512);
    set_capacity(disk, jdev->di.sector_count);

    return 0;
}

static int jms_ioctl(struct inode *i, struct file *f, unsigned int cmd,
    unsigned long arg)
{
    struct hd_geometry geo;
    unsigned long size;
    int disk_id;
    int rc = 0;
    int tmp;
    struct jms_dev_t *jdev;
    struct gendisk *disk;
    int readahead = 4;
    struct backing_dev_info *bdi;
    
    if (!i || !i->i_bdev || !i->i_rdev || !i->i_bdev->bd_disk)
    {
        DBG_E(DPORT_BLOCK, ("LB: Invalid inode for ioctl cmd %p\n", i));
        return -JEINVAL;
    }

    disk = i->i_bdev->bd_disk;
    jdev = disk->private_data;
    if (jdev == NULL)
        return -JENXIO;

    disk_id = jdev->disk_id;

    switch(cmd)
    {
    case BLKSSZGET:
        size = jdev->di.sector_size;

        DBG_I(DPORT_BLOCK, ("LB: Block size is %ld\n", size));
        
        if (copy_to_user((long *) arg, &size, sizeof (size)))
        {
            DBG_E(DPORT_BLOCK, ("LB: Error passing block size to user\n"));
            rc = -EFAULT;
        }
        break;

    case BLKGETSIZE:
        size = jdev->di.sector_count;
        
        DBG_I(DPORT_BLOCK, ("LB: Disk size is %ld\n", size));

        if (copy_to_user((long *) arg, &size, sizeof (size)))
        {
            DBG_E(DPORT_BLOCK, ("LB: Error passing disk size to user\n"));
            rc = -EFAULT;
        }
        break;

    case HDIO_GETGEO:
        if (!arg)
        {
            rc =  -JEINVAL;
            break;
        }
        size = jdev->di.sector_count;
        geo.start = get_start_sect(i->i_bdev);

        geo.heads       = 64; 
        geo.sectors     = 32; 
        tmp = size / (geo.heads * geo.sectors);
        if (tmp > 0xffff)
        {
            geo.heads   = 0xff; 
            geo.sectors = 62; 
        }
        geo.cylinders = size / (geo.heads * geo.sectors);
        if (!geo.cylinders)
            geo.cylinders = 1;
        
        DBG_I(DPORT_BLOCK, ("LB: Geometry %xH %xC %xS Start %ld Size %ld\n",
            geo.heads, geo.cylinders, geo.sectors, geo.start,
            size));
        
        if (copy_to_user((void *) arg, &geo, sizeof(geo)))
        {
            DBG_E(DPORT_BLOCK, ("LB: Error passing geometry to user\n"));
            rc = -EFAULT;
        }
        break;
        
    case BLKRAGET:
        if (!arg)
        {
            rc = -JEINVAL;
            break;
        }
        
        DBG_I(DPORT_BLOCK, ("LB: Get read ahead [%d]\n", readahead));
        if (copy_to_user((long *) arg, &readahead, sizeof(int)))
        {
            DBG_E(DPORT_BLOCK, ("LB: Error passing read ahead to user\n"));
            rc = -EFAULT;
        }
        break;

    case BLKRASET:
        DBG_I(DPORT_BLOCK, ("LB: Trying to set read ahead\n"));
        
        if (get_user(tmp, (long *)arg))
        {
            rc = -JEINVAL;
            break;
        }

        if (tmp > 0xff)
        {
            rc = -JEINVAL;
            break;
        }

        bdi = blk_get_backing_dev_info(i->i_bdev);
        if (bdi == NULL)
        {
            rc = -JENOTTY;
            break;
        }
        bdi->ra_pages = (arg * 512) / PAGE_CACHE_SIZE;
        
        DBG_I(DPORT_BLOCK, ("LB: Read ahead is %ld now\n", bdi->ra_pages));
        
        break;
        
    case BLKRRPART:
        DBG_I(DPORT_BLOCK, ("LB: Validate partitions\n"));
        rc = jms_revalidate(disk);
        break;

    default:
        DBG_I(DPORT_BLOCK, ("LB: Default IOCTL returned %d\n", rc));
    }

    return rc; 
}

static int jms_check_change(struct gendisk *disk)
{
    struct jms_dev_t *jdev;
    
    if (!disk || !disk->private_data)
    {
        DBG_E(DPORT_BLOCK, ("LB: Check_change - invalid disk\n"));
        return -JEINVAL;
    }
    jdev = disk->private_data;
    
    if (jdev->state != STATE_INUSE)
        return 1;       /* Non-zero value - Media changed*/

    return 0;
}

static int jms_open(struct inode *i, struct file *filp)
{
    int rc = 0;
    struct jms_dev_t *jdev;
    struct gendisk *disk;

    if (!i || !i->i_bdev || !i->i_bdev->bd_disk || 
        !i->i_bdev->bd_disk->private_data)
    {
        DBG_E(DPORT_BLOCK, ("LB: Invalid inode for open cmd - i=%p "
            "i_bdev=%p bd_disk=%p private_data=%p\n", i, i->i_bdev,
            i->i_bdev->bd_disk, i->i_bdev->bd_disk->private_data));
        return -JEINVAL;
    }

    disk = i->i_bdev->bd_disk;
    jdev = disk->private_data;

    if (jdev->state == STATE_DETACHED)
        return -JEINVAL;

    jsafe_enter();

    /* Even if this is not an active disk, we may inc it */
    jdev->usage++;
    
    if (jdev->state != STATE_INUSE)
    {
        rc = -JENOENT;
        goto Error;
    }
    
    /* check for read only... */
    if (!filp)
    {
        DBG_E(DPORT_BLOCK, ("LB: Jms_open accessing a NULL file pointer "
            "[filp= %p]\n", filp));
        rc = -JEINVAL;
        goto Error;
    }
    if ((jdev->di.write_protect) && (filp->f_mode & FMODE_WRITE) )
    {
        rc = -EROFS;
        goto Error;
    }
    
    jsafe_leave();
    return 0;
    
Error:
    jdev->usage--;
    jsafe_leave();
    return rc;
}

static int jms_release(struct inode *i, struct file *filp)
{
    struct jms_dev_t *jdev;
    struct gendisk *disk;
    int rc = 0;
    
    jsafe_enter();
    
    if (!i || !i->i_bdev || !i->i_bdev->bd_disk || 
        !i->i_bdev->bd_disk->private_data)
    {
        DBG_E(DPORT_BLOCK, ("LB: Invalid inode for open cmd - i=%p "
            "i_bdev=%p bd_disk=%p private_data=%p\n", i, i->i_bdev, 
            i->i_bdev->bd_disk, i->i_bdev->bd_disk->private_data));
        rc = -JEINVAL;
        goto Exit;
    }
    disk = i->i_bdev->bd_disk;

    jdev = disk->private_data;

    if (jdev->state == STATE_DETACHED)
    {
        jdev->usage--;

        if (!jdev->usage)
            jdev->state = STATE_UNUSED;

        rc = -JENOENT;
        goto Exit;
    }

    if (jdev->state == STATE_UNUSED)
    {
        DBG_E(DPORT_BLOCK, ("LB: Someone thought they were accessing an unused "
            "disk !\n"));
        rc = -JENOENT;
        goto Exit;
    }

    if (!jdev->usage)
    {
        /* releasing a closed disk */
        DBG_E(DPORT_BLOCK, ("LB: Error releasing disk [%d] - disk is already "
            "closed\n", jdev->disk_id));
        rc = -JEINVAL;
        goto Exit;
    }
    jdev->usage--;
        
Exit:
    jsafe_leave();
    return rc;
}

static struct block_device_operations jms_fops = {
        owner:          THIS_MODULE,
        open:           jms_open,
        release:        jms_release,
        ioctl:          jms_ioctl,
        revalidate_disk:     jms_revalidate,
        media_changed: jms_check_change,
};

static int block_dev_id_get(void)
{
    int i;
    for (i = 0; i < MAX_DISKS; i++)
    {
        if (jms_hostv[i] == 0)
        {
            jms_hostv[i] = 1;
            return i;
        }
    }
    /* number of disks acceeds max */
    return -1;
}

static void block_dev_id_put(int id)
{
    if (id < 0 || id >= MAX_DISKS)
    {
        /* illegal disk_id */
        DBG_E(DPORT_BLOCK, ("LB: Could not free disk id [%d]\n", id));
        return;
    }
    
    if (jms_hostv[id] == 0)
    {
        /* freeing an already free id */
        DBG_E(DPORT_BLOCK, ("LB: Could not free disk id [%d], id not used...\n",
            id));
        return;
    }
    
    jms_hostv[id] = 0;
}

int jos_block_attach_cb(void *dev, void **user_ctx)
{
    struct jms_dev_t *jdev = NULL;
    int rc = 0;
    request_queue_t *q = NULL;
    struct gendisk *disk = NULL;

    /* allocating & initializing new disk */
    try_module_get(THIS_MODULE);
    
    jdev = jmalloc(sizeof(struct jms_dev_t),M_ZERO);
    if (!jdev)
    {
        rc = -JENOMEM;
        goto Error;
    }
    
    spin_lock_init(&jdev->lock);

    jdev->disk_id = block_dev_id_get();
    if (jdev->disk_id == -1)
    {
        /* Trying to attach too many devices */ 
        DBG_E(DPORT_BLOCK, ("LB: Could not allocate disk id. "
            "Max disks attached [%d].\n", MAX_DISKS));
        rc = -JENOENT;
        goto Error;
    }

    jdev->usage = 0;
    jdev->xfers = 0;
    jdev->dev = dev;
    jdev->state = STATE_INUSE;

    disk = alloc_disk(MAX_PARTITIONS + 1);
    if (!disk)
    {
        rc = -JENOENT;
        goto Error;
    }

    snprintf(disk->disk_name, 32, DEVICE_NAME "%c", jdev->disk_id + 'a');
    snprintf(disk->devfs_name, 64, DEVICE_NAME "/%c", jdev->disk_id + 'a');
    disk->major = MAJOR_NR;
    disk->first_minor = jdev->disk_id * MAX_MINORS;
    disk->fops = &jms_fops;
    disk->private_data = jdev;
    q = blk_init_queue(jms_request, &jdev->lock);
    if (q == NULL)
    {
        rc = -JENOMEM;
        goto Error;
    }

    disk->queue = q;
    blk_queue_bounce_limit(q, BLK_BOUNCE_HIGH);
    blk_queue_max_hw_segments(q, 9);    
    blk_queue_max_phys_segments(q, 9);
    blk_queue_segment_boundary(q, 0xffffffff);
    blk_queue_max_sectors(q, MAX_SECTORS_PER_REQUEST);
    blk_queue_hardsect_size(q, jdev->di.sector_size);
    blk_queue_ordered(q, 1);

    q->queuedata = jdev;
    jdev->jgd = disk;

    /* create device's directory */
    devfs_mk_dir(disk->devfs_name);

    if (disk_get_media_info(dev, &jdev->di))
    {
        DBG_E(DPORT_BLOCK, ("LB: failed getting device info\n"));
        rc = -JENXIO;
        goto Error;
    }

    set_capacity(disk, jdev->di.sector_count);
    
    DBG_V(DPORT_BLOCK, ("LB: Registering disk\n"));
    *user_ctx = jdev;
    jsafe_leave();
    {
        add_disk(disk);
    }
    jsafe_enter();

    DBG_I(DPORT_BLOCK, ("LB: Attach_cb - disk added %s [num = %d]\n", 
        disk->disk_name, jdev->disk_id));


    return 0;
    
Error:
    if (q)
    {
        blk_cleanup_queue(q);
        disk->queue = NULL;
    }

    if (disk)
    {
        put_disk(disk);
        del_gendisk(disk);
    }

    if (jdev && (jdev->disk_id != -1))
        block_dev_id_put(jdev->disk_id);

    if (jdev)
        jfree(jdev);
    
    module_put(THIS_MODULE);
    return rc;
}

void jos_block_detach_cb(void *dev, void *user_ctx)
{
    struct gendisk *disk;
    struct jms_dev_t *jdev;
    request_queue_t *q;
    jdev = (struct jms_dev_t*)user_ctx;
    disk = jdev->jgd;
    q = disk->queue;
    
    /* Mark device as detached (so no more requests come) */
    jdev->state = STATE_DETACHED;

    jdev->dev = NULL;

    /* Wait until all pending transfers end */
    while (jdev->xfers)
    {
        DBG_E(DPORT_BLOCK, ("LB: Waiting for pending transfer requests to "
            "return %d\n", jdev->xfers));
        os_delay_ms(100);
    }

    /* Still pending OS requests. Mark device as unused 
     * (so no more requests come) */
    if (jdev->usage)
    {
        jdev->state = STATE_UNUSED;
        DBG_E(DPORT_BLOCK, ("LB: Detaching disk - cannot free disk id %d , "
            "[device usage = %d]\n", jdev->disk_id, jdev->usage));
    }
    else
    {
        block_dev_id_put(jdev->disk_id);
    }

    /* cleanup ops */
    jsafe_leave();
    {
        blk_cleanup_queue(q);
        jdev->jgd->queue = NULL;
        del_gendisk(jdev->jgd);
        put_disk(disk);
    }
    jsafe_enter();

    jdev->jgd = NULL;
    module_put(THIS_MODULE);
    jfree(jdev);
}

int jos_block_init(void)
{
    int i;
    int err;

    err = register_blkdev(MAJOR_NR, DEVICE_NAME);
    if (err < 0)
    {
        DBG_E(DPORT_BLOCK, ("LB: Could not register block device [%d]\n", err));
        return err;
    }
    register_disk_handler(&disk_functions);

    /* initializing id's array */
    for (i = 0; i < MAX_DISKS; i++)
        jms_hostv[i] = 0;
    
    return 0;
}

void jos_block_uninit(void)
{
    unregister_disk_handler();
    devfs_remove(DEVICE_NAME);
    unregister_blkdev(MAJOR_NR, DEVICE_NAME);
}

