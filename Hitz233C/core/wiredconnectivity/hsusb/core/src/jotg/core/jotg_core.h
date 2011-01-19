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
#ifndef __OTG_CORE_H__
#define __OTG_CORE_H__

#include <otg_core_ocd.h>
#include <uw_otg_app.h>

typedef enum {
    OTG_TIMER_TYPE_NONE = 0,
    OTG_TIMER_TYPE_A_WAIT_VRISE,
    OTG_TIMER_TYPE_A_WAIT_BCON,
    OTG_TIMER_TYPE_A_AIDL_BDIS,
    OTG_TIMER_TYPE_B_ASE0_BRST,
    OTG_TIMER_TYPE_B_DATA_PLS,
    OTG_TIMER_TYPE_B_VBUS_PLS,
    OTG_TIMER_TYPE_B_BUS_REQ,
    OTG_TIMER_TYPE_A_SUSPEND_DET,
    OTG_TIMER_TYPE_A_BCON_DEBOUNCE,
    OTG_TIMER_TYPE_B_AIDL_BDIS,
    OTG_TIMER_TYPE_A_WAIT_VFALL,
    /* Add last for count */
    OTG_TIMER_TYPE_LAST
} otg_timer_type_t;

#define OTG_TIMER_TIME_NONE            0
#define OTG_TIMER_TIME_A_WAIT_VRISE    200
#define OTG_TIMER_TIME_A_WAIT_BCON     1200
#define OTG_TIMER_TIME_A_AIDL_BDIS     225
#define OTG_TIMER_TIME_B_ASE0_BRST     4
#define OTG_TIMER_TIME_B_DATA_PLS      8
#define OTG_TIMER_TIME_B_VBUS_PLS      15
#define OTG_TIMER_TIME_B_BUS_REQ       32000
#define OTG_TIMER_TIME_A_SUSPEND_DET   70
#define OTG_TIMER_TIME_A_BCON_DEBOUNCE 120
#define OTG_TIMER_TIME_B_AIDL_BDIS     7
#define OTG_TIMER_TIME_A_WAIT_VFALL    1000

typedef enum {
    SMP_NOTIF_NONE,

    SMP_NOTIF_HW,
    SMP_NOTIF_DEV_SUSPEND,
    SMP_NOTIF_DEV_RESUME,
    SMP_NOTIF_HOST_SUSPEND,
    SMP_NOTIF_HOST_RESUME,
    SMP_NOTIF_HOST_CONNECT,
    SMP_NOTIF_HOST_DISCONNECT,
    SMP_NOTIF_START_SRP,
    SMP_NOTIF_VBUS_SHUTDOWN,
    SMP_NOTIF_VBUS_POWERUP,
    SMP_NOTIF_START_HNP
} smp_notif_t;

typedef struct otg_smp_notif_t
{
    smp_notif_t notif;
    TAILQ_ENTRY(otg_smp_notif_t) next_notif;
} otg_smp_notif_t;

typedef struct
{
    /* SM parameters maintained by the OCD */
    otg_hw_smp_t hw_smp; /* State machine hardware parameters */

    juint8_t    state;               /* The state machine's current state */ 
    juint8_t    err_code;            /* Last error code */

    /* for A-device */
    juint_t     b_hw_conn        :1; /* 1 = B-device was connected */
    juint_t     b_sw_conn        :1; /* 1 = B-device was connected */
    juint_t     b_bus_suspend    :1; /* 1 = B-device suspended the bus */
    juint_t     b_bus_resume     :1; /* 1 = B-device resumed the bus */

    juint_t     b_hnp_support    :1; /* 1 = B-device supports HNP */
    juint_t     b_srp_support    :1; /* 1 = B-device supports SRP */
    juint_t     a_set_b_hnp_en   :1; /* 1 = A-device enabled HNP in the 
                                           B-device */

    juint_t     a_suspend_req    :1; /* 1 = A-device requests bus suspend */
    juint_t     a_clr_err        :1; /* 1 = Clear A-device's last a_vbus_err 
                                           state */
    juint_t     a_bus_drop       :1; /* 1 = Device wishes to drop the bus */

    juint_t     a_pend_hnp_en    :1; /* 1 = Session started by B-Device's SRP */

    /* for B-device */
    juint_t     a_conn           :1; /* 1 = A-device was connected */
    juint_t     a_bus_suspend    :1; /* 1 = A-device suspended the bus */
    juint_t     a_bus_resume     :1; /* 1 = A-device resumed the bus */

    juint_t     b_srp_done       :1; /* 1 = B-device completed initiation of
                                           an SRP session */

    juint_t     b_hnp_en         :1; /* 1 = HNP is enabled on the B-device */
    juint_t     a_hnp_support    :1; /* 1 = A-device supports HNP */
    juint_t     a_alt_hnp_support:1; /* 1 = The A-device port to which the
                                           B-device is connected is not 
                                           HNP-capable, but the A-device has an 
                                           alternate port that is HNP-capable */
    juint_t     b_host_pending   :1; /* 1 = B-device is awaiting a response
                                           from the host */

    /* for both A-device and B-device */
    juint_t     bus_req          :1; /* 1 = Device requests the bus */

    juint_t     b_srp_pending    :1; /* 1 = B-device initiated an SRP session  
                                           and is awaiting a response from the 
                                           host */

    juint_t     timeout          :1; /* 1 = OTG controller timer timed-out */
    juint_t     timer_running    :1; /* 1 = OTG controller timer is currently 
                                           running */

    otg_timer_type_t timer_type;    /* The type of the OTG controller timer that
                                       is currently running */
} otg_smp_t;

typedef struct jotg_core_t {
    /* State machine parameters */
    otg_smp_t state_machine;   

    /* Next state handler */
    jbool_t (*next_state_handler)(struct jotg_core_t *jotg_core);

    /* OCD connection vars */ 
    ocd_ops_t *ocd_ops;
    ocd_ctx_t ocd_ctx;

    /* OTG Application vars */
    otg_app_callbacks_t app_cbs;
    otg_app_info_t app_info;
    otg_appctx_t app_ctx;

    /* OTG Host White List parameters */
    juint32_t white_list_size;
    peripheral_matcher_t *white_list;
    
    /* Array for holding timer times */
    juint16_t otg_timer_times[OTG_TIMER_TYPE_LAST];

    /* Mutex to enable only single access to the state machine */
    jmutex_h state_machine_mtx;

    TAILQ_HEAD(otg_smp_notif_queue_t, otg_smp_notif_t) notif_queue;

    /* Flag for IDLE state */
    jbool_t is_idle;

    jbool_t full_speed_only;
} jotg_core_t;

#endif
