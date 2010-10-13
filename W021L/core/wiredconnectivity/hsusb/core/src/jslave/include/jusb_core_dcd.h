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
#ifndef _JUST_CORE_DCD_H_
#define _JUST_CORE_DCD_H_

#include "jusb_common.h"
#define PIPE_TYPE_MASK(pipe_type) (0x01 << pipe_type)

#define SUPPORTED_PIPE_TYPE(type_map, pipe_type) \
        (type_map & (0x01 << pipe_type))
    
#define SUPPORTED_PIPE_DIR(direction_map, pipe_dir) \
        (direction_map & (0x01 << pipe_dir))

typedef struct {
    /* DCD functions for use by Core 
     *  dcd_enable              - Enable the DCD
     *  dcd_disable             - Disable the DCD
     *  
     *  wake_the_host           - Software HOST wakeup 
     *  set_self_power          - Change the device's self power setting 
     *
     *  dcd_is_otg              - Return true if device is part of an OTG device
     *  dcd_invoke_otg_ioctl    - Invoke an IOCTL to the OTG Stack
     *  
     *  vbus_connect            - Connect to power
     *  vbus_draw_amount        - Set device power usage
     *  vbus_disconnect         - Disconnect from power
     *  
     *  dcd_connect             - Software controlled connect to HOST
     *  dcd_disconnect          - Software controlled disconnect from HOST 
     *
     *  dcd_enable_ep           - Enable an endpoint
     *  dcd_disable_ep          - Disable an endpoint
     *  dcd_abort_ep            - Remove all pending I/O from an EP
     *  dcd_stall_ep            - Set a stall condition on an EP
     *
     *  dcd_send_io_request     - Send a new I/O request 
     *
     *  dcd_alloc_pipe          - Allocate a pipe for FDs use 
     *  dcd_free_pipe           - Free an allocated pipe
     *  dcd_get_ep_status       - Return endpoint status
     *  
     *  dcd_set_test_mode       - Configure the DCD into electrical test mode
     *  dcd_set_address_state   - Move device to addressed state
     *  dcd_set_config_state    - Move device to configured state 
     *  dcd_handle_feature      - Handle Set/ Clear feature request.
     *  dcd_get_max_packet0     - Return the packet size of EP 0
     */
    jresult_t (*dcd_enable)(jdevice_t dev, jbool_t is_full_speed);
    void (*dcd_disable)(jdevice_t dev);

    jresult_t (*wake_the_host)(jdevice_t dev);
    jresult_t (*set_self_power)(jdevice_t dev, juint8_t flag); /* (1/0-ON/OFF) */

#ifdef CONFIG_JOTG
    jbool_t   (*dcd_is_otg)(jdevice_t dev);
    jresult_t (*dcd_invoke_otg_ioctl)(jdevice_t dev, drv_ioctl_t ioctl, 
        void *args);
#endif

    jresult_t (*vbus_connect)(jdevice_t dev);
    jresult_t (*vbus_draw_amount)(jdevice_t dev, juint32_t power);
    jresult_t (*vbus_disconnect)(jdevice_t dev);

    jresult_t (*dcd_connect)(jdevice_t dev);    
    jresult_t (*dcd_disconnect)(jdevice_t dev);

    jresult_t (*dcd_enable_ep)(jdevice_t dev, pipe_desc_t *pipe);
    jresult_t (*dcd_disable_ep)(jdevice_t dev, pipe_desc_t *pipe);
    jresult_t (*dcd_abort_ep)(jdevice_t dev, pipe_desc_t *pipe);
    jresult_t (*dcd_stall_ep)(jdevice_t dev, pipe_desc_t *pipe, 
        jbool_t flag); /* Flag: 0/1-Clear/Set */

    jresult_t (*dcd_send_io_request)(jdevice_t dev, pipe_desc_t *pipe, 
        request_t *request); 

    jresult_t (*dcd_alloc_pipe)(jdevice_t dev, pipe_desc_t *pipe, 
        juint8_t config_index, juint8_t iface_index, juint8_t alt_index);
    jresult_t (*dcd_free_pipe)(jdevice_t dev, pipe_desc_t *pipe);

    jresult_t (*dcd_get_ep_status)(jdevice_t dev, juint8_t *status, 
        juint8_t ep_num);

    jresult_t (*dcd_set_address_state)(jdevice_t dev, juint16_t address);
    jresult_t (*dcd_handle_feature)(jdevice_t dev, juint16_t feature, 
        juint16_t index, juint8_t set);
    jresult_t (*dcd_set_config_state)(jdevice_t dev, juint8_t config);
    juint8_t (*dcd_get_max_packet0)(jdevice_t dev);
    void (*dcd_handle_tx_completion)(jdevice_t dev, pipe_desc_t *pipe);
    void (*dcd_set_test_mode)(jdevice_t dev, juint16_t test_mode);
} dcd_ops_t;

/* Core functions for use by DCD 
 *  core_handle_ep0  - Handle all messages comming to EP0 
 *  core_disconnect  - Handle disconnect from HOST
 *  core_register_dcd - Register the DCD with the Core
 *  core_unregister_dcd - Un-Register the DCD with the Core
 */
jresult_t core_handle_ep0(void *core_ctx, juint8_t *buffer, device_speed_t speed);
void core_disconnect(void *core_ctx);
jresult_t core_suspend(void *core_ctx);
jresult_t core_resume(void *core_ctx);
jresult_t core_register_dcd(jdevice_t dev, dcd_ops_t *ops, void **core_ctx);
void core_unregister_dcd(void *core_ctx);

jresult_t core_enable_by_ctx(void *core_ctx);
void core_disable_by_ctx(void *core_ctx);

#endif

