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
#ifndef _DEV_MONITOR_H_
#define _DEV_MONITOR_H_

typedef void *mdev_t;

typedef struct {
    jint_t dev_class;
    jint_t subclass;
    jint_t protocol;
    jint_t vendor;
    jint_t product;
    jint_t version;
    char *name;
    char *desc;
    juint8_t depth;
} devmon_info_t;

typedef enum {
    DM_NEW_DEVICE,
    DM_NEW_DEVICE_READY,
    DM_NEW_DEVICE_NO_DRIVER,
    DM_NEW_DEVICE_ATTACHING,
    DM_NEW_DEVICE_ATTACH_DENIED,

    DM_DEVICE_DETACHING,
    DM_DEVICE_DETACHED,
    DM_DEVICE_DISCONNECTED,
    DM_DEVICE_SUSPENDED,
    DM_DEVICE_RESUMED,
    DM_DEVICE_CONFIG_SETTING,
    DM_DEVICE_CONFIG_SET,

    DM_UCOMP_ATTACH_SUBDEV,
    DM_UCOMP_DETACHING_SUBDEV,
    DM_UCOMP_DETACHED_SUBDEV,

    DM_ERROR_CANNOT_OPEN_EP0,
    DM_ERROR_CANNOT_ALLOC_ADDR,
    DM_ERROR_CANNOT_SET_ADDR,
    DM_ERROR_CANNOT_GET_FIRST_DESC,
    DM_ERROR_BAD_DEV_DESC,
    DM_ERROR_CANNOT_GET_CONFIG,
    DM_ERROR_CANNOT_ATTACH,
    DM_ERROR_CANNOT_SET_CONFIG,
    DM_ERROR_RESET_FAILED,
    DM_ERROR_OVERCURRENT,
    DM_ERROR_POWER_NEGOTIATION_FAILED,

    DM_ERROR_HUB_TOO_DEEP,
    DM_ERROR_HUB_GET_DESC,
    DM_ERROR_HUB_BUSPWR_ATTACHED_TO_BUSPWR,
    DM_ERROR_HUB_NO_INTERFACE,
    DM_ERROR_HUB_NO_ENDPOINT,
    DM_ERROR_HUB_BAD_ENDPOINT,
    DM_ERROR_HUB_CANNOT_OPEN_PIPE,

    /* HSU temp fix */ 
    DM_DEVICE_PARTIAL_SUPPORTED, 
    DM_DEVICE_SUPPORTED, 
    DM_DEVICE_NOT_SUPPORTED, 
    /* HSU temp fix */ 

    DM_LAST
} devmon_msg_t;

typedef struct devmon_methods {
    /* Implemented by user */
    void (*detached)(mdev_t dev, mdev_t parent);
    void (*attached)(mdev_t dev, mdev_t parent);
    jresult_t (*allow_attach)(mdev_t dev);
    jresult_t (*devmon_notify_h)(devmon_msg_t msg, mdev_t dev, juint32_t param);
} devmon_methods_t;

/* Implemented by HOST stack */
jresult_t devmon_notify(devmon_msg_t msg, mdev_t dev, juint32_t param);
jresult_t devmon_register(devmon_methods_t *methods);
void devmon_unregister(void);
jresult_t devmon_force_detach(mdev_t dev);
void devmon_get_info(mdev_t dev, devmon_info_t *info);

/*  Internal host stack functions */
void devmon_handle_detached(mdev_t dev, mdev_t parent);
void devmon_handle_attached(mdev_t dev, mdev_t parent);
jresult_t devmon_allow_attach(mdev_t dev);
    
#endif

