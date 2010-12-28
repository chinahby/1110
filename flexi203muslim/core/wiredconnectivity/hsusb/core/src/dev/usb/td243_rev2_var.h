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

#ifndef __TD243HC_R2_R2_VAR_H__
#define __TD243HC_R2_R2_VAR_H__

#include <jos.h>

typedef struct td243hc_rev2_etd_s {
    /* Transfer Descriptors fields */
    juint32_t dw0;
#define TD243HC_R2_ETD_ADDRESS     (0, 0x7f)
#define TD243HC_R2_ETD_ENDPT       (7, 0xf)
#define TD243HC_R2_ETD_DIRECT      (11, 0x3)
# define TD243HC_R2_ETD_DIRECT_OUT  0x1
# define TD243HC_R2_ETD_DIRECT_IN   0x2
#define TD243HC_R2_ETD_SPEED       (13, 0x1)
# define TD243HC_R2_ETD_SPEED_FULL  0x0
# define TD243HC_R2_ETD_SPEED_LOW   0x1
#define TD243HC_R2_ETD_FORMAT      (14, 0x3)
# define TD243HC_R2_ETD_FORMAT_CTRL 0x0
# define TD243HC_R2_ETD_FORMAT_ISOC 0x1
# define TD243HC_R2_ETD_FORMAT_BULK 0x2
# define TD243HC_R2_ETD_FORMAT_INTR 0x3
#define TD243HC_R2_ETD_MAXPKTSIZ   (16, 0x3ff)
#define TD243HC_R2_ETD_HALTED      (1<<27)
#define TD243HC_R2_ETD_TOGCRY      (1<<28)
#define TD243HC_R2_ETD_SNDNAK      (1<<30)

    juint32_t dw1;
#define TD243HC_R2_DW1_XBUFSRTAD   (0, 0xffff)
#define TD243HC_R2_DW1_YBUFSRTAD   (16, 0xffff)

    juint32_t dw2;
#define TD243HC_R2_DW2_RTRYDELAY   (0, 0x1f) /* Used by Bulk / Control */
#define TD243HC_R2_DW2_POLINTERV   (0, 0xff) /* Used by Interrupt */
#define TD243HC_R2_DW2_RELPOLPOS   (8, 0xff)
#define TD243HC_R2_DW2_DIRPID      (16, 0x3)
# define TD243HC_R2_DW2_DIRPID_SETUP 0x0
# define TD243HC_R2_DW2_DIRPID_OUT   0x1
# define TD243HC_R2_DW2_DIRPID_IN    0x2
#define TD243HC_R2_DW2_BUFROUND    (1<<18) /* When set - short packet is OK */
#define TD243HC_R2_DW2_DELAYINT    (19, 0x7)
# define TD243HC_R2_DW2_DELAYINT_NOINT 0x7 /* Generate no interrupt on completion */
#define TD243HC_R2_DW2_DATATOGL    (22, 0x3)
#define TD243HC_R2_DW2_ERRORCNT    (24, 0xf)
#define TD243HC_R2_DW2_COMPCODE    (28, 0xf)

    juint32_t dw3;
#define TD243HC_R2_DW3_TOTBYECNT    (0, 0x1fffff)
#define TD243HC_R2_DW3_BUFSIZE      (21, 0x7ff)
} td243hc_rev2_etd_t;

typedef struct td243hc_rev2_xfer {
    struct usbd_xfer xfer;
} td243hc_rev2_xfer_t;

/* Transfer error codes */
#define TD243HC_R2_STATUS_NOERROR                  0x0
#define TD243HC_R2_STATUS_CRC_ERROR                0x1
#define TD243HC_R2_STATUS_BITSTUFFING_ERROR        0x2
#define TD243HC_R2_STATUS_TOGGLE_ERROR             0x3
#define TD243HC_R2_STATUS_STALL                    0x4
#define TD243HC_R2_STATUS_DEV_NOT_RESPOND          0x5
#define TD243HC_R2_STATUS_PID_ERROR                0x6
#define TD243HC_R2_STATUS_DATA_OVERRUN             0x8
#define TD243HC_R2_STATUS_DATA_UNDERRUN            0x9
#define TD243HC_R2_STATUS_BUFFER_OVERRUN           0xc
#define TD243HC_R2_STATUS_BUFFER_UNDERRUN          0xd
#define TD243HC_R2_STATUS_BUFFER_SCHED_OVERRUN     0xe

#define TD243HC_R2_NUM_ETDS 32
#define TD243HC_R2_XY_SIZE 64
typedef struct {
    td243hc_rev2_xfer_t *xfer;
    juint16_t xbuf_offset;
    juint16_t ybuf_offset;
    
    struct xfrag *list_head;
    struct xfrag *current_active;
} td243hc_rev2_etd_table_t;

typedef struct td243hc_rev2_pipe {
    struct usbd_pipe pipe;
    juint32_t epd;

    juint8_t nexttoggle;
} td243hc_rev2_pipe_t;

typedef struct td243hc_rev2_softc {
    struct usbd_bus sc_bus;             /* base device */

    juint16_t sc_flags;
#define TD243HC_R2_SCFLG_DONEINIT         0x0001 /* td243hc_init() has been called */

    jbus_resource_h mem_res;
    jbus_resource_h irq_res;
    
    void *interrupt_h;

    jbool_t sc_dying;
    jbool_t sc_suspend;

    /* Extra HCD specific data */
    juint32_t eintrs;         /* Enabled interrupts */
    juint32_t intrs;          /* Interrupts to handle */
    juint32_t done_queue;     /* etd done queue to handle */
    jspinlock_h     sc_lock;

    td243hc_rev2_etd_table_t etd_table[TD243HC_R2_NUM_ETDS];
    juint32_t etd_to_proceed;
    jbool_t add_to_list;
} td243hc_rev2_softc_t;

/* Common Functions */
jresult_t td243hc_rev2_intr(void *v);
void td243hc_rev2_softintr(void *v);
jresult_t td243hc_rev2_hw_init(td243hc_rev2_softc_t *sc);
void td243hc_rev2_hw_uninit(struct td243hc_rev2_softc *sc, jint_t flags);
jresult_t td243hc_rev2_hw_ioctl(td243hc_rev2_softc_t *sc, drv_ioctl_t ioctl, void *arg);

#endif
