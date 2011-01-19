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
#ifndef _JUSB_CORE_H_
#define _JUSB_CORE_H_

#include "jusb_common.h"
#include "jusb_core_dcd.h"
#include "jusb_core_fd.h"
#include "jslave_init.h"

/* Temp fix for B47218 */
#include "hsu_dev_app.h"
/* Temp fix END */

#define MAX_DESCRIPTOR_SIZE 256

typedef enum {
    FD_ENABLED,
    FD_DISABLED
} fd_state;

/* Function device specific data */
typedef struct fd {
    struct fd   *next;          /* Pointer to next FD */
    fd_state    state;          /* FD state */
    fd_ops_t    *fd_ops;        /* FD callbacks */
    fd_desc_t   *fd_desc;       /* List of FD requested pipes */
    context_t   context;        /* Context for use of FD's functions */
} fd_t;

/* Interface data */
typedef struct  {
    /* Interface EP0 callback */
    jresult_t    (*handle_usb_cmd)(void *context, void *msg);
    juint8_t     number;         /* Interface number */
    juint8_t     fd_if_num;      /* Index of iface in FD's struct */
    fd_t        *fd;            /* Back reference to function device */
    interface_desc_t *if_arr;   /* Back reference to interface descriptor */
    alt_interface_desc_t *if_curr; /* Back ref to FDs requested interface */
} interface_t;

typedef enum {
    CORE_NOT_INITIALIZED,       /* Need Init() to start */
    CORE_INITIALIZED,           /* Waiting for DCD */
    CORE_DCD_CONFIGURED,        /* Waiting for FD */
    CORE_FD_CONFIGURED,         /* Waiting for register_finish */
    CORE_CONFIGURED,            /* Waiting for Enable() */
    CORE_ENABLED,               /* Core in normal operation */
    CORE_SUSPENDED              /* Power mode suspended */
} core_states_t;

typedef struct {
    reconfig_complete_cb_t cb;
    void                   *arg;
    device_params_t        device_param;
    core_states_t          state_before_reconfig;
} reconfig_arg_t;

typedef enum {
    USB_CONFIGURED      = 0,    /* SET_CONFIG processed - EP0 answers */
    USB_ADDRESS         = 1     /* Address set          - EP0 answers */
} usb_states_t;
/* Configuration functionality data */
typedef struct {
    fd_t        *fd_list;       /* List of function drivers */
    interface_t *interfaces;
    juint8_t     interface_count;
    juint8_t     i_config;
} config_t;

/* Core functionality data */
typedef struct core_t core_t;
struct core_t {
    juint8_t core_index;
    usb_states_t usb_state;     /* config/active/etc */
    core_states_t core_state;   /* not a USB state, its a Module state */
    juint8_t     curr_config;    /* Current selected USB configuration */
    device_speed_t curr_speed;  /* Current device speed (High/Full/Low) */
    juint8_t     max_speed;      /* Max supported speed */

    jbool_t      iad_defined;    /* Flag that indicates a device using IADs */
    
    jdevice_t   dcd_dev;        /* DCD device handle */
    dcd_ops_t   *dcd_ops;       /* DCD callbacks */

    config_t    *configs;       /* List of configurations */
    juint8_t     config_count;

    request_t   *request;
    juint8_t     ep0_req_tag;    /* Tag of the latest EP0 request. */
    
    #define MAX_STRING_DESCRIPTORS 32
    char *string_descriptors[MAX_STRING_DESCRIPTORS];
    juint8_t msos_vendor_code;
    jbool_t msos_vendor_code_set;

    /* String indexes */
    juint8_t i_product;
    juint8_t i_manufacturer;
    juint8_t i_serial_number;

    jbool_t remote_wakeup_enabled;
    jbool_t remote_wakeup_scheduled;
#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    jtask_h remote_wakeup_task;
#endif
#ifdef CONFIG_REMOTE_WAKEUP_TEST
    jtask_h remote_wakeup_test_task;
#endif

    /* Reconfiguration parameters */
    jtask_h reconfig_task;
    reconfig_arg_t reconfig_args;
    jbool_t reconfig_inprogress;

    /* Running device parameters */
    device_params_t device_param;

    juint8_t core_number;

/* Temp fix for B47218 */

    /* Application vars */
    app_callbacks_t app_cbs;
    appctx_t app_ctx;

/* Temp fix END */

    /* HSU Addition */
/* Override for handling standard requests */
    jresult_t (*ep0_handler)(core_t *core, void *buffer,juint8_t ep0_req_tag);
};
    /* HSU Addition */
/* Temp Fix for B47218 */

jresult_t dev_app_init(appctx_t ctx, app_callbacks_t *app_cbs);

/* Temp fix END */

/* Handle a generic USB command on EP0 */
jresult_t chp9_handle_usb_command(core_t *core, void *buffer, 
    juint8_t ep0_req_tag);

/* Set configuration */
jresult_t chp9_handle_set_configuration(core_t *core, juint8_t config);


/* Update descriptors before enabling */
void chp9_set_string_descriptors(core_t *core);

/* HSU addition */
/* Clear string descriptors array & indexes */
void chp9_clear_string_descriptors(core_t *core);
/* End of HSU addition */

/* HSU addition */
jresult_t core_set_ep0_handler(juint8_t core_index, jresult_t (*ep0_handler)(
                               core_t *core, void *buffer, juint8_t ep0_req_tag));
/* HSU addition */
#endif

