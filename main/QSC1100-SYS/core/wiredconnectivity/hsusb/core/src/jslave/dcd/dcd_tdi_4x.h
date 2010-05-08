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
#ifndef _TDI_4X_DEVICE_H
#define _TDI_4X_DEVICE_H

#include <jos.h>
#include <jusb_core_dcd.h>
#include <lists.h>

#ifdef CONFIG_JTRANSCEIVER
#include <transceiver_device.h>
#endif

/* Transfer descriptor definitions */
#define TDI_4X_TRANSFER_DESCRIPTOR_SIZE        (7 * 4)
#define TDI_4X_TRANSFER_DESCRIPTOR_ALIGN 0x40

#define TD(soft_td) ((tdi4x_td_h)((soft_td)->virt))
#define TD_PHYS(soft_td) ((juint32_t)((soft_td)->phys))

struct tdi4x_soft_td
{
    void *virt;
    void *phys;
    void *cookie;
    juint32_t num_of_td;
    juint32_t td_size;

    TAILQ_ENTRY(tdi4x_soft_td) next;
};

#define PUTMACRO(a) a
#define GETOFS(ofs, mask)  ofs
#define GETMASK(ofs, mask)  mask

#define BFSET(field,val) ((juint32_t)(val) << PUTMACRO(GETOFS field))

#define BFGET(field,val) (((juint32_t)(val) >> PUTMACRO(GETOFS field)) & \
        PUTMACRO(GETMASK field))

#define BFMASK(field)    ((juint32_t)(GETMASK field) << PUTMACRO(GETOFS field))

#define BFRESET(field, prev_val, val) \
    (((prev_val) & ~BFMASK(field)) | BFSET(field, val))

typedef __PACKED_PRE__ struct
{
    /* HARDWARE */
    /* DWORD 0 */
    juint32_t next_td_ptr;
#define TDI_4X_TD_TERMINATE (0, 0x1)
    
    /* DWORD 1 */
    juint32_t td_header;
#define TDI_4X_TD_STATUS      (0,  0xff)
#define TDI_4X_TD_MULTO       (10, 0x3)
#define TDI_4X_TD_IOC         (15, 0x1)
#define TDI_4X_TD_TOTAL_BYTES (16, 0xffff)
    
    juint32_t buf_ptr[5];
} __PACKED_POST__ tdi4x_td_t;

typedef __PACKED_PRE__ struct 
{
    /* DWORD 0 */
    juint32_t qh_header;
#define TDI_4X_QH_IOS         (15, 0x1)
#define TDI_4X_QH_MAX_PKT_LEN (16, 0x7ff)
#define TDI_4X_QH_ZLT         (29, 0x1)
#define TDI_4X_QH_MULT        (30, 0x3)

    /* DWORD 1 */
    juint32_t cur_td_ptr;

    /* DWORD 2 - 8 */
    tdi4x_td_t cur_td;

    /* DWORD 9 */
    juint32_t reserved4;

#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
#define TDI_4X_QH_GET_SETUP_REQ(qh, req)                \
    req[0] = (juint8_t)((qh)->setup_buf_lo & 0x000000FF);\
    req[1] = (juint8_t)(((qh)->setup_buf_lo & 0x0000FF00) >> 8); \
    req[3] = (juint8_t)(((qh)->setup_buf_lo & 0x00FF0000) >> 16);\
    req[2] = (juint8_t)(((qh)->setup_buf_lo & 0xFF000000) >> 24);\
    req[5] = (juint8_t)((qh)->setup_buf_hi & 0x000000FF);        \
    req[4] = (juint8_t)(((qh)->setup_buf_hi & 0x0000FF00) >> 8); \
    req[7] = (juint8_t)(((qh)->setup_buf_hi & 0x00FF0000) >> 16);\
    req[6] = (juint8_t)(((qh)->setup_buf_hi & 0xFF000000) >> 24);        
    
#elif CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN
#define TDI_4X_QH_GET_SETUP_REQ(qh, req)                \
    req[0] = (juint8_t)((qh)->setup_buf_lo & 0x000000FF);\
    req[1] = (juint8_t)(((qh)->setup_buf_lo & 0x0000FF00) >> 8); \
    req[2] = (juint8_t)(((qh)->setup_buf_lo & 0x00FF0000) >> 16);\
    req[3] = (juint8_t)(((qh)->setup_buf_lo & 0xFF000000) >> 24);\
    req[4] = (juint8_t)((qh)->setup_buf_hi & 0x000000FF);        \
    req[5] = (juint8_t)(((qh)->setup_buf_hi & 0x0000FF00) >> 8); \
    req[6] = (juint8_t)(((qh)->setup_buf_hi & 0x00FF0000) >> 16);\
    req[7] = (juint8_t)(((qh)->setup_buf_hi & 0xFF000000) >> 24);        

#endif

    /* DWORD 10 */
    juint32_t setup_buf_lo;

    /* DWORD 11 */
    juint32_t setup_buf_hi;

} __PACKED_POST__ tdi4x_qh_t;

typedef tdi4x_qh_t *tdi4x_qh_h;
typedef tdi4x_td_t *tdi4x_td_h;

/* we are using the same qTD methond of EHCI controller */

#define USB_DIR_IN                       0x80    /* to host */
#define TDI_4X_MAX_PACKET_SIZE_EP0       64

/* Queue head definitions */
#define TDI_4X_DC_QUEUE_HEAD_SIZE 64 /* 48 but should be aligned to 64 */
#define TDI_4X_DC_QUEUE_HEAD_ACT_SIZE 48
#define TDI_4X_DC_QUEUE_HEAD_ALIGN 0x800

#define TDI_4X_TD_MAX_BYTES (0x1000 * 4) 

/* Status bits */
#define TDI_4X_TD_ACTIVE 0x80
#define TDI_4X_TD_HALTED 0x40
#define TDI_4X_TD_DATA_ERROR 0x20
#define TDI_4X_TD_TRAN_ERROR 0x08

typedef struct {
    juint8_t soft_addr; /* End Point number (hardware wise) */
    juint8_t hw_addr; /* End Point number (usb) */
    jbool_t is_in; /* Direction */
#define TYPE_CTRL 0x1
#define TYPE_ISOC 0x2
#define TYPE_BULK 0x4
#define TYPE_INTR 0x8
#define TYPE_ALL (TYPE_CTRL | TYPE_ISOC | TYPE_INTR | TYPE_BULK)
    unsigned type_map           : 4; /* Control/Isync/Bulk/Interrupt */
    unsigned inuse              : 1; /* Set if already enabled */
    unsigned ping_pong_attr     : 1; /* supports ping pong (double buffering) */
    unsigned last_buf           : 1; /* last buffer we handle (BANK 0 or 1) */

    juint16_t packet_size;       /* Actual size, not 2^size */
    juint8_t claimed_config;     /* Which config claimed this ep */
    juint8_t claimed_iface;      /* Which iface claimed this ep */
    juint8_t claimed_alt;        /* Which alt-iface of the FD claimed the ep */

    pipe_desc_t *pipe;

    jbool_t ep_stalled;

    request_t *cur_req;

    tdi4x_qh_h qhead;
    jtask_h timeout_task;       /* task for handling transfer timeouts */
    jbool_t abort_timeout_task;
    struct tdi4x_soft_dc_t *sc;
    jbool_t handle_comp_from_intr;

    TAILQ_HEAD(dcd_tdi_4x_td_head, tdi4x_soft_td) td_head;
    STAILQ_HEAD(inreq_queue, request_t) in_req_queue; /* Requests queue */
} ep_desc_t;

typedef struct tdi4x_soft_dc_t 
{
    jdevice_t dev;

    device_speed_t dev_speed;

#define TDI_4X_DC_NO_ADDR 0
    juint16_t address;
    
#define TDI_4X_DC_NOT_RDY           0x0  /* no set address took place */
#define TDI_4X_DC_RST_DONE          0x1  /* bus reset done */
#define TDI_4X_DC_ADDR_SET          0x2  /* set address register */
#define TDI_4X_DC_CONFIG_SET        0x3  /* configuration was set */
    juint8_t state;

    juint8_t max_available_eps;
    ep_desc_t *ep_desc_table;

    juint8_t *qhead_list;
    void *qhead_phys;
    void *qhead_list_cookie;

    /* interrupt enable software shadow */
    volatile juint32_t intr_msk;

#define TDI_4X_DC_INTR_BUS_RESET       0x01
#define TDI_4X_DC_INTR_PORT_ATTACHED   0x02
#define TDI_4X_DC_INTR_SUSPEND         0x04
#define TDI_4X_DC_INTR_BSESS_END       0x08
#define TDI_4X_DC_INTR_BSESS_VALID     0x10
    volatile juint8_t intr_to_handle;
    volatile juint32_t intr_ep_setup;
    volatile juint32_t intr_ep_data;
    jspinlock_h lock;

    juint8_t setup_packet[8];

    jbool_t is_suspended;
    jbool_t is_connected;

    /* resources */
    jbus_resource_h reg_res;
    jbus_resource_h irq_res;
    void *interrupt_h;

    void *core_ctx;
#ifdef CONFIG_JOTG
    /* Device Context for OTG Communication */ 
    jdevice_t otg_dev;
#endif
#ifdef CONFIG_JTRANSCEIVER
    trcv_ctx_h trcv;
#endif
    juint32_t itc_num;
} tdi4x_soft_dc_t;

/* TDI_4X USB device registers */

#define TDI_4X_ID_REG                   0x000
#define TDI_4X_HW_GENERAL_REG           0x004

#define TDI_4X_HW_DEVICE_REG            0x00C
#define TDI_4X_HW_DEVICE_DEV_EP(x)      (((x) >> 1) & 0x1F)

#define TDI_4X_HW_TX_BUF_REG            0x010
#define TDI_4X_HW_RX_BUF_REG            0x014
#define TDI_4X_CAP_LEN_REG              0x100
#define TDI_4X_DCI_VER_REG              0x120
#define TDI_4X_DCC_PARAM_REG            0x124

#define TDI_4X_USB_CMD_REG              0x140
#define TDI_4X_USB_CMD_RS               0x1 /* Run bit*/ 
#define TDI_4X_USB_CMD_RST              0x2 /* Reset */ 
#define TDI_4X_USB_CMD_SUTW             0x2000 /* setup trip wire */
#define TDI_4X_USB_CMD_ATDTW            0x4000
#define TDI_4X_USB_CMD_ITC_SHFT         16
#define TDI_4X_USB_CMD_ITC_MSK          (0xff << TDI_4X_USB_CMD_ITC_SHFT)
#define TDI_4X_USB_CMD_ITC_IMMDIATE     0x00
#define TDI_4X_USB_CMD_ITC_01           0x01
#define TDI_4X_USB_CMD_ITC_02           0x02
#define TDI_4X_USB_CMD_ITC_04           0x04
#define TDI_4X_USB_CMD_ITC_08           0x08
#define TDI_4X_USB_CMD_ITC_16           0x10
#define TDI_4X_USB_CMD_ITC_32           0x20
#define TDI_4X_USB_CMD_ITC_64           0x40

#define TDI_4X_INTR_STS_REG             0x144
#define TDI_4X_INTR_STS_UE              0x0001   /* USB interrupt       */
#define TDI_4X_INTR_STS_UEI             0x0002   /* Error interrupt     */
#define TDI_4X_INTR_STS_PCI             0x0004   /* Port change         */

/* HSU addition - Catching core system errors (Start). */
#define TDI_4X_INTR_STS_SEI             0x0010   /* System Error         */
/* HSU addition - Catching core system errors (End).   */

#define TDI_4X_INTR_STS_URE             0x0040   /* Reset Recieved      */
#define TDI_4X_INTR_STS_SOF             0x0080   /* SOF recieved        */
#define TDI_4X_INTR_STS_SLI             0x0100   /* Suspend DC          */

#define TDI_4X_INTR_ENB_REG             0x148
#define TDI_4X_INTR_UE                  0x0001  /* USB interrupt        */
#define TDI_4X_INTR_UEE                 0x0002  /* USB error interrupt  */
#define TDI_4X_INTR_PCI                 0x0004  /* Port Change          */

/* HSU addition - Catching core system errors (Start). */
#define TDI_4X_INTR_SEE                 0x0010  /* System Error         */
/* HSU addition - Catching core system errors (End).   */

#define TDI_4X_INTR_URE                 0x0040  /* Reset Recieved       */
#define TDI_4X_INTR_SRE                 0x0080  /* SOF                  */
#define TDI_4X_INTR_SLE                 0x0100  /* DC Suspend           */

/* HSU modification - enabled the system error interrupt, too. (Start) */
#define TDI_4X_INTR_START                                 \
    (TDI_4X_INTR_PCI | TDI_4X_INTR_URE |                  \
     TDI_4X_INTR_UE  | TDI_4X_INTR_SLE | TDI_4X_INTR_SEE)
/* HSU modification - enabled the system error interrupt, too. (End)   */

#define TDI_4X_FRAME_INDEX_REG          0x14C

#define TDI_4X_DEVICE_ADDR_REG          0x154
#define TDI_4X_DEVICE_ADDR_SET(addr)    ((addr) << 25)
#define TDI_4X_DEVICE_ADDR_SET_ACC      (1 << 24)

#define TDI_4X_EP_LST_ADDR_REG          0x158
#define TDI_4X_BURST_SIZE_REG           0x160

/* HSU addition */
#define TDI_4X_ULPI_VIEWPORT_REG        0x170

#define TDI_4X_PORTSC_REG     0x184
#define TDI_4X_PORTSC_CCS     0x0001
#define TDI_4X_PORTSC_FPR     0x0040
#define TDI_4X_PORTSC_SUSP    0x0080
#define TDI_4X_PORTSC_PR      0x0100
#define TDI_4X_PORTSC_PHCD    0x0800
#define TDI_4X_PORTSC_PFSC    0x01000000
#define TDI_4X_PORTSC_PSPD_MASK    0xC000000
#define TDI_4X_PORT_SPEED(portstat) \
    (((portstat) & TDI_4X_PORTSC_PSPD_MASK) >> 26)
#define TDI_4X_PORT_SPEED_FULL          0x0
#define TDI_4X_PORT_SPEED_LOW           0x1
#define TDI_4X_PORT_SPEED_HIGH          0x2
#define TDI_4X_PORTSC_PTS_MASK   0xC0000000
#define TDI_4X_PORTSC_PTS_UTMI   0x00000000
#define TDI_4X_PORTSC_PTS_ULPI   0x80000000
#define TDI_4X_PORTSC_PTS_SERIAL 0xC0000000

#define TDI_4X_MODE_REG                 0x1A8
#define TDI_4X_MODE_DEVICE              0x02
#define TDI_4X_MODE_BIG_ENDIAN          0x04
#define TDI_4X_MODE_SLOM                0x08
#define TDI_4X_MODE_SDIS                0x10

#define TDI_4X_EP_SETUP_STAT_REG        0x1AC
#define TDI_4X_EP_SETUP_ACK(ep_num)     (1 << (ep_num))

#define TDI_4X_EP_PRIM_REG              0x1B0
#define TDI_4X_EP_PRIM_RX(ep)           (1 << (ep))       
#define TDI_4X_EP_PRIM_TX(ep)           (1 << ((ep) + 0x10))

#define TDI_4X_EP_FLUSH_REG             0x1B4
#define TDI_4X_FLUSH_RX_EP(ep)          (1 << (ep))          /* reset ep */
#define TDI_4X_FLUSH_TX_EP(ep)          (1 << ((ep) + 0x10)) /* reset ep */

#define TDI_4X_EP_STAT_REG              0x1B8
#define TDI_4X_STAT_RX_EP(ep)           (1 << (ep))
#define TDI_4X_STAT_TX_EP(ep)           (1 << ((ep) + 0x10))

#define TDI_4X_EP_COMP_REG              0x1BC
#define TDI_4X_COMP_RX_EP(ep)           (1 << (ep))          /* reset ep */
#define TDI_4X_COMP_TX_EP(ep)           (1 << ((ep) + 0x10)) /* reset ep */

#define TDI_4X_EP_CTRL_REG(ep_num)      (0x1C0 + ((ep_num) * 4))
#define TDI_4X_EP_CTRL_RXS              0x01            /* RX Stall           */
#define TDI_4X_EP_CTRL_RXD              0x02            /* Always zero        */
#define TDI_4X_EP_CTRL_RXT_CTRL         0x00
#define TDI_4X_EP_CTRL_RXT_ISOC         0x04
#define TDI_4X_EP_CTRL_RXT_BULK         0x08
#define TDI_4X_EP_CTRL_RXT_INTR         0x0C
#define TDI_4X_EP_CTRL_RXI              0x20            /* Always zero        */
#define TDI_4X_EP_CTRL_RXR              0x40            /* Data toggle reset  */
#define TDI_4X_EP_CTRL_RXE              0x80            /* RX Endpont enabled */
#define TDI_4X_EP_CTRL_TXS              0x010000        /* TX Stall           */
#define TDI_4X_EP_CTRL_TXD              0x020000        /* Always zero        */
#define TDI_4X_EP_CTRL_TXT_CTRL         0x000000
#define TDI_4X_EP_CTRL_TXT_ISOC         0x040000
#define TDI_4X_EP_CTRL_TXT_BULK         0x080000
#define TDI_4X_EP_CTRL_TXT_INTR         0x0C0000
#define TDI_4X_EP_CTRL_TXI              0x200000        /* Always zero        */
#define TDI_4X_EP_CTRL_TXR              0x400000        /* Data toggle reset  */
#define TDI_4X_EP_CTRL_TXE              0x800000        /* TX Endpont enabled */
#define TDI_4X_EP_CTRL_DISABLE_TX       0xFFFF
#define TDI_4X_EP_CTRL_DISABLE_RX       0xFFFF0000

#define TDI_4X_OTGSC                    0x1A4
/* OTG_SC Interrupt Enable bits - Read/Write */
#define TDI_4X_OTGSC_BSEIE              0x10000000 /* B-Session End IE   */
#define TDI_4X_OTGSC_BSVIE              0x08000000 /* B-Session Valid IE */

/* OTG_SC Interrupt Status bits - Read/Write to clear */
#define TDI_4X_OTGSC_BSEIS              0x00100000 /* B-Session End IS   */
#define TDI_4X_OTGSC_BSVIS              0x00080000 /* B-Session Valid IS */

/* OTG_SC status bits - Read only */
#define TDI_4X_OTGSC_BSV                0x00000800 /* B-Session Valid    */

#endif
