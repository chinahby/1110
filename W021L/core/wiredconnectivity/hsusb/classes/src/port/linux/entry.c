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
#include <version.h>

/* Global sync mutex */
struct semaphore GlobalMtx;

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
    #include <test_ioctl.h>
    static int do_usbware_ioctl(struct inode *inode, struct file *filp, 
        unsigned int cmd, unsigned long arg);
    static int linux_open(struct inode *inode, struct file *filp);
    static int linux_close(struct inode *inode, struct file *filp);

    struct file_operations usbware_fops = {
        owner: THIS_MODULE,
        ioctl: do_usbware_ioctl,
        open: linux_open,
        release: linux_close,
    };

    static int usbware_linux_major = 0;
    
    /* Forward declaration - defined in main test directory */
    jresult_t test_run(juint32_t id, char *params);
#endif

int init_module(void)
{
    int rc = 0;

    init_MUTEX(&GlobalMtx);

    jsafe_enter();

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
    usbware_linux_major = register_chrdev(0, UW_DEV_NAME, &usbware_fops);
#endif

#if defined CONFIG_JOS_BLOCK
    rc = jos_block_init();
    if (rc)
    {
        jsafe_leave();
        return rc;
    }
#endif

#if defined CONFIG_JSERIAL
    rc = serial_dev_init();
    if (rc)
    {
        jsafe_leave();
        return rc;
    }
#endif

#if defined CONFIG_JHOST_CDC_ECM
    rc = cdc_ecm_init();
    if (rc)
    {
        jsafe_leave();
        return rc;
    }
#endif
    rc = jstart_stack(get_uw_args());
    if (rc)
    {
        jsafe_leave();
        return rc;
    }

#ifdef CONFIG_PORT_NET_DEV
    rc = net_dev_init();
#endif

    jsafe_leave();
    return rc;
}

void cleanup_module(void)
{
    jsafe_enter();
#ifdef CONFIG_PORT_NET_DEV
    net_dev_uninit();
#endif
    jstop_stack();

#if defined CONFIG_JOS_BLOCK
    jos_block_uninit();
#endif

#if defined CONFIG_JSERIAL
    serial_dev_uninit();
#endif

#if defined CONFIG_JHOST_CDC_ECM
    cdc_ecm_uninit();
#endif

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
    unregister_chrdev(usbware_linux_major, UW_DEV_NAME);
#endif

    jsafe_leave();
}

#if defined(CONFIG_TEST) && !defined(CONFIG_TEST_AUTORUN)
int linux_open(struct inode *inode, struct file *filp) 
{
    return 0;
}

int linux_close(struct inode *inode, struct file *filp) 
{
    return 0;
}

int do_usbware_ioctl(struct inode *inode, struct file *filp, unsigned int cmd,
    unsigned long arg) 
{
    int rc = 1;

    switch (cmd)
    {
    case IOCTL_UW_RUN_SINGLE_TEST:
        {
            test_data_t *data = (test_data_t*)arg;

            printk("do_usbware_ioctl: recieved IOCTL_UW_RUN_SINGLE_TEST, tid =" 
                " %d\n", data->test_id);
            data->rc = test_run(data->test_id, data->test_params);
            rc = data->rc;
            break;
        }
    default:
        printk("do_usbware_ioctl: Invalid IOCTL code %d\n", cmd);
    }
    return rc;
}
#endif

jresult_t os_port_init(void)
{
   return 0;
}

void os_port_uninit(void)
{
}

