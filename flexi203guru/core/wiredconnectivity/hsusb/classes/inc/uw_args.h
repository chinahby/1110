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

#ifndef _UW_ARGS_H_
#define _UW_ARGS_H_

typedef enum
{
    DEVICE_TYPE_NONE,
    DEVICE_TYPE_MSD,
    DEVICE_TYPE_HID,
    DEVICE_TYPE_SERIAL,
    DEVICE_TYPE_LOOPBACK,
    DEVICE_TYPE_TEST,
    DEVICE_TYPE_CDC_ACM,
    DEVICE_TYPE_CDC_ECM,
    DEVICE_TYPE_CDC_OBEX,
    DEVICE_TYPE_VIDEO,
    DEVICE_TYPE_DFU,
    DEVICE_TYPE_SICD,
    DEVICE_TYPE_AUDIO,
    DEVICE_TYPE_VENDOR
} fd_type_t;

typedef enum
{   /* Initialize USBWare host stack */
    UW_INIT_HOST = 1,                  

    /* Initialize USBWare device stack */
    UW_INIT_DEVICE,

    /* Initialize USBWare both host and device stacks */
    UW_INIT_HOST_DEVICE,

    /* Initialize USBWare OTG stack (includes also host and device stacks */
    UW_INIT_OTG
}
uw_init_opt_t;

typedef jresult_t (*fd_init_func_t)(void *ctx);
typedef struct 
{
    fd_type_t type;             /* Function driver type */
    juint8_t count;              /* Number of instances of 'type' */
    fd_init_func_t init_func;   /* Optional parameter for vendor specific
                                   function drivers - requires type to be 
                                   FD_TYPE_VENDOR */
} fd_instance_t;

typedef jbool_t (*self_powered_func_t)(void);
typedef struct {
    self_powered_func_t self_powered_func;
    juint_t self_powered : 1;
#ifdef CONFIG_JOTG
    juint_t srp_support: 1;
    juint_t hnp_support: 1;
#endif
    
    juint16_t vendor;
    juint16_t product;
    juint16_t release;

    juint8_t dev_class;
    juint8_t dev_sub_class;
    juint8_t dev_protocol;

    char *product_name;
    char *product_manufacturer;
    char *product_serial;
    
    /* HSU temp fix */
    juint16_t bcdUSB;
    /* HSU temp fix */

    /*HSU Addition*/
    char *configuration;
    /*End of HSU addition*/
} dev_info_t;

typedef struct
{
    juint8_t num_of_fd_instances;
    fd_instance_t *fd_instances;
    char *config_name;
    juint_t self_powered : 1;
    juint_t remote_wakeup: 1;
    juint8_t max_power; /* In 2mAmp units */
} config_param_t;

typedef jresult_t (*vendor_cmd_in_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size);
typedef jresult_t (*vendor_cmd_out_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint16_t wLength);
typedef jresult_t (*vendor_cmd_out_data_stage_cb_t)(juint8_t *data, 
    juint16_t size);

/*HSU addition*/
typedef jresult_t (*reserved_cmd_in_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size);
typedef jresult_t (*reserved_cmd_out_cb_t)(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint16_t wLength);
typedef jresult_t (*reserved_cmd_out_data_stage_cb_t)(juint8_t *data, 
    juint16_t size);
/*End of HSU addition*/

typedef char* (*vendor_get_string_cb_t)(juint8_t index);

/* HSU addition - standard_cmd_cb_t */
typedef void (*standard_cmd_cb_t)(juint8_t bmRequestType,
    juint8_t bRequest, juint16_t wValue, juint16_t wIndex, juint16_t wLength);

typedef struct
{
    jbool_t auto_enable;
    jbool_t full_speed_only; /* HSU addition: removed featurization */
    dev_info_t *dev_info;
    juint8_t num_of_configs;
    config_param_t *config_param;
    vendor_get_string_cb_t vendor_get_string_cb;
    vendor_cmd_in_cb_t vendor_cmd_in_cb;
    vendor_cmd_out_cb_t vendor_cmd_out_cb;
    vendor_cmd_out_data_stage_cb_t vendor_cmd_out_data_stage_cb;
    /*HSU addition*/    
    reserved_cmd_in_cb_t reserved_cmd_in_cb;
    reserved_cmd_out_cb_t reserved_cmd_out_cb;
    reserved_cmd_out_data_stage_cb_t reserved_cmd_out_data_stage_cb;
    /*End of HSU addition*/
    standard_cmd_cb_t standard_cmd_cb; /* HSU addition - standard_cmd_cb_t */
    jbool_t static_interface_number;
} device_params_t;

typedef struct
{
    juint8_t core_number;     
    uw_init_opt_t init_opt;
    
    /* Relevant only for device stack */
    device_params_t *device_param;
} uw_hw_core_param_t;

typedef struct 
{
    /* For static memory management */
#ifdef CONFIG_MEMPOOL
    void *mem_buf;
    juint32_t mem_buf_size;
#endif

#ifdef CONFIG_POOL_DMA
    void *dma_vbuf;
    void *dma_pbuf;
    juint32_t dma_buf_size;
# ifdef CONFIG_POOL_DMA_CACHABLE
    void *dma_cachable_vbuf;
    void *dma_cachable_pbuf;
    juint32_t dma_cachable_size;
# endif
#endif

    juint8_t num_of_cores;
    uw_hw_core_param_t *hw_core_params;
} uw_args_t;

uw_args_t *get_uw_args(void);      
#endif
