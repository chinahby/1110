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
#ifndef __JOS_H__
#define __JOS_H__

#include <jconfig.h>

#ifndef ERRORNO_INCLUDED
#include <jerrno.h>
#endif

#include "jdebug.h"
#include "jtypes.h"
#ifdef CONFIG_JHOST
    #include "dev_monitor.h"
#endif
#include "lists.h"
#include "compiler.h"
#include "os_port.h"
#include "jutil.h"
#include "controllers.h"

#ifdef CONFIG_TEST_DICT
# include <test_dict.h>
#endif

#ifdef CONFIG_BUS_PCI
# include "pci_defs.h"
#endif

/* Device & Driver registration methods */
typedef struct jdevice_s *jdevice_t;

typedef enum {
    DRV_IOCTL_NONE,
    
    /* Host Power Management related IOCTLs */
    DRV_IOCTL_PWR_CHILD_SUSPEND,
    DRV_IOCTL_PWR_CHILD_RESUME,
    DRV_IOCTL_PWR_HUB_REMOTE_WAKEUP,
    
    /* OTG related IOCTLS */
    /* OTG Controller IOCTLs */
    DRV_IOCTL_DEV_SUSPEND,
    DRV_IOCTL_DEV_RESUME,
    DRV_IOCTL_DEV_RESET,
    DRV_IOCTL_DEV_HNP_ENBL,
    DRV_IOCTL_DEV_HNP_ALT_SUPPORTED,
    DRV_IOCTL_HOST_SUSPEND,
    DRV_IOCTL_HOST_RESUME,
    DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED,
    DRV_IOCTL_HOST_REMOTE_CONNECTED,
    DRV_IOCTL_HOST_REMOTE_DISCONNECTED,
    DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED,
    DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED,
    /* Device Controller IOCTLs */
    DRV_IOCTL_DEV_ENABLE,
    DRV_IOCTL_DEV_DISABLE,
    /* Host Controller IOCTLs */
#define HOST_PORT_ENABLE_NO_RESET 0
#define HOST_PORT_ENABLE_RESET    1
    DRV_IOCTL_HOST_PORT_ENABLE,
    DRV_IOCTL_HOST_PORT_DISABLE,
    DRV_IOCTL_HOST_PORT_SUSPEND,
    DRV_IOCTL_HOST_PORT_RESUME,
    DRV_IOCTL_HOST_ENABLE_HNP,

    /* Transceiver Related IOCTLs*/
    /* OTG Controller IOCTLs */
    DRV_IOCTL_OTG_INTR,
    /* Device Controller IOCTLs */
    DRV_IOCTL_DEVICE_INTR,
    /* Host Controller IOCTLs */
    DRV_IOCTL_HOST_PORT_CHANGE,
    /* Transceiver controller IOCTLs */
    DRV_IOCTL_TCD_GET_OPS,
    DRV_IOCTL_TCD_SET_CTX,
    DRV_IOCTL_HOST_SET_PARAMS,
    DRV_IOCTL_DEV_SET_PARAMS,

    /* Electrical Host Controller Tests IOCTLs*/
    DRV_IOCTL_SET_ETEST_MODE,

    DRV_IOCTL_LAST
} drv_ioctl_t;

typedef jresult_t (*driver_init_t)(void); 
typedef void     (*driver_uninit_t)(void);
typedef jresult_t (*driver_func_t)(jdevice_t dev);
typedef jresult_t (*driver_ioctl_t)(jdevice_t dev, drv_ioctl_t ioctl, void *arg);

typedef struct {
    driver_func_t       match;
    driver_func_t       attach;
    driver_func_t       detach;
    driver_func_t       suspend;
    driver_func_t       resume;
    driver_ioctl_t      ioctl;
    driver_init_t       uninit;
} driver_methods_t;

/* Auto-init for non usb drivers */
typedef struct {
    driver_init_t init;
    driver_uninit_t uninit;
    jbool_t init_done;
} load_arr_t;

/* PCI Methods */
#ifdef CONFIG_BUS_PCI
#define BUS_SPACE_BARRIER_READ  0x01    /* force read barrier */
#define BUS_SPACE_BARRIER_WRITE 0x02    /* force write barrier */

jint_t jpci_get_powerstate(jdevice_t dev);
jresult_t jpci_set_powerstate(jdevice_t dev, jint_t state);
jint32_t j_pci_get_devid(os_bus_h bus);
juint8_t j_pci_get_class(os_bus_h bus);
juint8_t j_pci_get_subclass(os_bus_h bus);
juint8_t j_pci_get_progif(os_bus_h bus);
juint16_t j_pci_get_vendor(os_bus_h bus);
void j_bus_space_barrier(jint_t flags);
#endif

/* Register a Controller with parameters sent as ivars */
jresult_t add_generic_controller_ex(os_bus_h bus, jint_t id, void **handle,
    void *args);
jresult_t del_generic_controller_ex(jdevice_t dev);

/* Device manipulation methods */
void *j_device_get_softc(jdevice_t dev);
void *j_get_usbd_device(jdevice_t dev);
char *j_device_get_desc(jdevice_t dev);
jbus_h j_device_get_bus(jdevice_t dev);

jdevice_t jdevice_add(jdevice_t parent);
void jdevice_delete(jdevice_t device);

jbool_t j_device_is_attached(jdevice_t dev);
jresult_t j_device_detach(jdevice_t dev);
    
jdevice_t j_device_get_parent(jdevice_t dev);
jint16_t j_device_get_unit(jdevice_t dev);
char *j_device_get_nameunit(jdevice_t dev);
void j_device_set_nameunit(jdevice_t dev, char *name, int unit);
jbool_t j_device_attached(jdevice_t dev);

typedef struct kobj_class       driver_t;

void jinterrupt_teardown(jbus_resource_h res, void *handle);
jresult_t jinterrupt_setup(jbus_resource_h res, 
    interrupt_handler isr_func, void *isr_arg,
    jthread_func dsr_func, void *dsr_arg, void **handle);

/* Task manipulation methods */
typedef enum {
    /* Soft interrupts */
    TASK_CONTROLLER =   THREAD_PRIORITY_CONTROLLER,

#ifndef CONFIG_TASK_SINGLE
    /* Core specific, non-blocking tasks */
    TASK_CORE =         THREAD_PRIORITY_CORE,

    /* Generic task for use by class & function drivers */
    TASK_CLASS_DRIVER = THREAD_PRIORITY_DRIVER,
    
#ifdef JDEBUG
    /* Used by the monitor task, to catch deadlocks. 
     * Must be one before last */
    TASK_DEBUG =        THREAD_PRIORITY_OTHER, 
#endif
#endif
    /* Must be last ! */
    TASK_TYPE_COUNT 
} jtask_type;

#ifndef SINGLE_THREADED
typedef struct jtask_s *jtask_h;

jresult_t jtask_init(jtask_h *task, jtask_type type);
void jtask_uninit(jtask_h task);
void jtask_stop(jtask_h task);
int jtask_pending(jtask_h task);
# ifndef JDEBUG
jresult_t jtask_schedule(jtask_h task, juint32_t timeout, 
    jthread_func cb, void *arg);
# else
#  define jtask_schedule(a,b,c,d)   \
    jtask_schedule_d(a,b,c,d, __FILE__, __LINE__)
jresult_t jtask_schedule_d(jtask_h task, juint32_t timeout, 
    jthread_func cb, void *arg, char *file, jint_t line);
# endif
#endif

#ifndef SINGLE_THREADED
# define CORE_SAFE_ENTER        jcore_safe_enter()
# define CORE_SAFE_LEAVE        jcore_safe_leave()
void jcore_safe_enter(void);
void jcore_safe_leave(void);
#else
# define CORE_SAFE_ENTER        do { } while (0)
# define CORE_SAFE_LEAVE        do { } while (0)
#endif

#ifndef PAGE_SHIFT
#define PAGE_SHIFT      12              /* LOG2(PAGE_SIZE) */
#endif
#ifndef LINUX24
#ifndef PAGE_SIZE
#define PAGE_SIZE       (1<<PAGE_SHIFT) /* bytes/page */
#endif
#endif

void j_device_set_desc(jdevice_t dev, const char* desc);
void j_device_set_ivars(jdevice_t dev, void * ivars);
jresult_t j_device_probe_and_attach(jdevice_t dev, void *usbd_device);
jresult_t j_device_suspend(jdevice_t dev);
jresult_t j_device_resume(jdevice_t dev);
jresult_t j_device_ioctl(jdevice_t dev, drv_ioctl_t ioctl, void *arg);

typedef void bus_driver_added_t(jdevice_t _dev, driver_t *_driver);
typedef void bus_child_detached_t(jdevice_t _dev, jdevice_t _child);
typedef jresult_t bus_child_location_str_t(jdevice_t _dev, jdevice_t _child,
    char *buf, juint32_t buflen);
typedef jresult_t bus_child_pnpinfo_str_t(jdevice_t _dev, jdevice_t _child,
    char *buf, juint32_t buflen);

void *j_device_get_ivars(jdevice_t dev);

/*
 *   JOS String Manipulation functions
 */
char *j_strdup(const char *str);
char *j_numtostr(char *outstr, juint32_t base, juint32_t min_digits,
    juint32_t max_digits, juint32_t num);
#define NUMERIC_VALUE_INVALID   0xFFFFFFFF
juint32_t j_strtonum(char *instr, juint32_t base, char **last_ptr);
char *j_strstr(char *instr, char *substr);
jbool_t j_is_space(char c);

#define j_bzero(buf, len) j_memset(buf, 0, len)

#define timevalclear(tvp)               ((tvp)->tv_sec = (tvp)->tv_usec = 0)
#define timevalcmp(tvp, uvp, cmp)                                       \
        (((tvp)->tv_sec == (uvp)->tv_sec) ?                             \
            ((tvp)->tv_usec cmp (uvp)->tv_usec) :                       \
            ((tvp)->tv_sec cmp (uvp)->tv_sec))
void j_delay(jint32_t us);

#define SYSCTL_INT(parent, nbr, name, access, ptr, val, descr) 
#define TUNABLE_INT(a, b)

#if (!defined(LINUX24) && !defined(METAG_ALL_VALUES))
typedef jint32_t off_t;  /* file offset */
typedef void *dev_t;
#endif

#ifdef JDEBUG
void j_mark_time_v(char *file, jint32_t line);
#define j_mark_time() j_mark_time_v(__FILE__, __LINE__)
extern jint32_t j_start_perf;
void j_dump_time(void);
#endif

#ifdef CONFIG_EM_FS
jresult_t jfs_initialize(void);
jresult_t jfs_uninitialize(void);
#endif 

#define DRIVER_CONTROLLER       0x1
#define DRIVER_USB              0x2

jresult_t jdriver_register(jint_t type, driver_methods_t *methods,
    char *name, jint_t id, juint16_t size);

/* Common way to declare a driver */
#define DRIVER_COMMON_DECLARE(type, name, id, size, init_func, uninit_func) \
        \
        static driver_methods_t __CONCAT(name, _methods) = {    \
                __CONCAT(name, _probe),         \
                __CONCAT(name, _attach),        \
                __CONCAT(name, _detach),        \
                __CONCAT(name, _suspend),       \
                __CONCAT(name, _resume),        \
                __CONCAT(name, _ioctl),         \
                (uninit_func)                   \
        }; \
        \
        jresult_t __CONCAT(name, _init)(void)    \
        {               \
                int rc; \
                \
                if ((init_func) != NULL) \
                { \
                    rc = (init_func)(); \
                    if (rc) \
                        return rc; \
                } \
                \
                rc = jdriver_register(type, &__CONCAT(name, _methods), \
                        #name, id, size); \
                if (rc && (uninit_func) != NULL) \
                    (uninit_func)(); \
                \
                return rc; \
        }

/* Common way to declare a usb driver */
#define DRIVER_USB_DECLARE(name) \
        DRIVER_COMMON_DECLARE(DRIVER_USB, name, 0, \
            sizeof(struct __CONCAT(name, _softc)), \
            (driver_init_t)NULL, (driver_init_t)NULL)

/* Declare usb driver with custom init/uninit function */
#define DRIVER_USB_DECLARE_CUSTOM_INIT(name, init_func, uninit_func) \
        DRIVER_COMMON_DECLARE(DRIVER_USB, name, 0, \
            sizeof(struct __CONCAT(name, _softc)), \
            init_func, uninit_func)

/* Common way to declare a controller driver */
#define DRIVER_CONTROLLER_DECLARE(name, size) \
        DRIVER_COMMON_DECLARE(DRIVER_CONTROLLER, name, \
            __CONCAT(controller_type_, name), size, \
            (driver_init_t)NULL, (driver_init_t)NULL)

/* Declare controller driver with custom init/uninit function */
#define DRIVER_CONTROLLER_DECLARE_CUSTOM_INIT(name, size, init_func, uninit_func) \
        DRIVER_COMMON_DECLARE(DRIVER_CONTROLLER, name, \
            __CONCAT(controller_type_, name), size, \
            init_func, uninit_func)

/* DMA handler functions */
#define DMAADDR(dma, offset) \
        (juint32_t)jdma_phys_addr(*(dma), offset)

#define KERNADDR(dma, offset) \
        jdma_virt_addr(*(dma), offset)

typedef void *jdma_handle;

/* Get Physical/Virtual address of a block */
void *jdma_phys_addr(jdma_handle p, juint32_t offset);
void *jdma_virt_addr(jdma_handle p, juint32_t offset);

/* Set block to some dummy values 
 * (Make sure to reset back to (0, NULL, NULL) before freeing) */
void jdma_dummy_set(juint32_t size, void *vaddr, void *paddr, jdma_handle p);

/* Allocate new DMA block */
jresult_t jdma_alloc(juint32_t size, juint16_t align, 
    void **vaddr, void **paddr, juint16_t flags, jdma_handle *handle);

/* Free DMA block */
void jdma_free(jdma_handle handle);

#endif

