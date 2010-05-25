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
#ifndef _JUSB_COMMON_H_
#define _JUSB_COMMON_H_

#include <jos.h>
#include <jutil.h>
#include "jslave_init.h"

#if defined(CONFIG_JSLAVE_TEST_MODE) && defined(CONFIG_FULL_SPEED_ONLY)
#error "CONFIG_JSLAVE_TEST_MODE can not be set with CONFIG_FULL_SPEED_ONLY"
#endif

/* Types of supported Pipes (same as USB Chp 9) */
typedef enum  {
    PIPE_CONTROL = 0,
    PIPE_ISOC    = 1,
    PIPE_BULK    = 2,
    PIPE_INTR    = 3
} pipe_type_t;

/* Used to return the status of a pipe */
typedef enum {
    PIPE_UNKNOWN = 0,
    PIPE_ENABLED,       /* I/O can be preformed on the pipe */
    PIPE_DISABLED,
    PIPE_STALLLED,
    PIPE_ABORTING
} pipe_status_t;


#define DIRECTION_IN    0
#define DIRECTION_OUT   1

/* Supported speeds */
typedef enum {
    SPEED_UNKNOWN       = 0,
    SPEED_LOW           = 1,
    SPEED_FULL          = 2,
    SPEED_HIGH          = 3
} device_speed_t;

/* This array holds maximum pacet sizes for each type:
 *      Bulk / Intr / Isync / Control
 * and each speed:
 *      Low / Full / High 
 */
extern juint16_t MAX_PACKET_SIZES[4][3]; 

/* Standard callback context */
typedef void *context_t;

/* Standard buffer (with physical address for DMA) */
typedef struct {
    void        *vaddr;
    void        *dma_addr;
    juint32_t    buffer_size;
    void        *cookie; 
} buffer_t;

/* Possible Request statuses */
typedef enum {
    REQUEST_READY = 0,
    REQUEST_INPROGRESS = 1,
    REQUEST_CANCELLED = 2,
    REQUEST_PENDING = 3,
    REQUEST_ERROR = 4,
    REQUEST_COMPLETED = 5
} request_status_t;

struct request_t;

typedef void (*callback_t)(struct request_t *);

/* Used to hold request data for data transfer (FD <-> DCD)  */
typedef struct request_t  {
    juint_t      zero      : 1;  /* Force a "zero length packet" 
                                   if last fame = max packet size */
    juint_t      direction : 1;  /* For use by callbacks,
                                   set by Core 0/1 - In/Out*/
    juint_t      is_buffer_allocated : 1;  /* Should request_free free
                                             the buffer */
    juint_t      ep0_zlp_reply : 1; /* Should be set to 1 when a zero
                                      length data stage is required
                                      on ep0 */
    juint_t      padding : 4;      /* Reserved bits for future use and 
                                   alignment */

    /* Tag to verify the reply match the right control request */
    juint8_t ep0_req_tag;

    buffer_t    buffer;         /* The buffer to preform operation on */
    
    callback_t complete;        /* Completion callback */
    context_t   context;        /* For use by the complete callback */

    request_status_t status;    /* Request status */

    juint32_t    transfer_size;  /* Number of bytes to transfer */
    juint32_t    bytes_transferred; /* Number of bytes transferred */

    void        *caller_private; /* Opaque for use by request initiator */

    STAILQ_ENTRY(request_t) next_request; 
    LIST_ENTRY(request_t) next_request_in_list;
    juint32_t    timeout;
    juint32_t    completed_req;
    jbool_t      single_request;
    
    void        *dcd_private;   /* Opaque for use by the DCD */

    juint32_t    isoc_max_payload;

#ifdef CONFIG_JSLAVE_TEST_MODE
    juint16_t    test_mode;
#endif
} request_t;

typedef struct pipe_desc_t {
    /* Normal representation not in 2^(size) */
    juint16_t max_pkt_size_high;
    juint16_t max_pkt_size_full;
    pipe_type_t type;
    unsigned direction    : 1;
    unsigned sync_type    : 2;  /* For isoc pipes */
    unsigned usage_type   : 2;  /* For isoc pipes */
    juint8_t address;
    
    /* In high speed we can allow 1-3 transactions per microframe) 
     * Zero indexed, i.e.: 0 = 1 packet per microframe */
    unsigned transaction_per_microframe :2; 
    union {
        juint8_t nak_rate;       /* For bulk, max NAK rate per microframe */
        juint8_t poll_interval;  /* For isoc/interrupt, polling frequency */
    } rate_u; 

    juint32_t extra_descriptor_size;
    juint8_t *extra_descriptor;
    juint8_t poll_interval_high;
    
    void *fd_handle;
    void *dcd_handle;

    juint16_t max_pkt_size;      /* Packet size for current speed */
    pipe_status_t status;
    
    STAILQ_HEAD(pipe_req_queue, request_t) req_queue; /* Requests queue */
} pipe_desc_t; 

jresult_t core_init(uw_hw_core_param_t *hw_core_params, juint8_t num_of_cores);
void core_uninit(void);

juint8_t core_get_controller_idx(juint8_t controller_number);
jresult_t core_enable_by_idx(juint8_t core_index);
void core_disable_by_idx(juint8_t core_index);

jresult_t core_reconfig(juint8_t core_index, device_params_t *new_config,
    reconfig_complete_cb_t cb, void *arg);

juint8_t core_get_curr_configuration(juint8_t core_number);

/* HSU Addition */
void core_clear_reconfig_flag(juint8_t core_index);
/* HSU Addition End */

#endif

