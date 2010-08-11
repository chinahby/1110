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
#ifndef _DCD_TD243FC_H
#define _DCD_TD243FC_H

#include <jos.h>
#include <jusb_core_dcd.h>
#include <lists.h>

#define TD243FC_GEN_EP_NUM                       15
#define TD243FC_EP_NUM                           (TD243FC_GEN_EP_NUM + 1)

typedef struct
{
    pipe_desc_t *pipe;
    request_t *req;
    juint16_t buf_size;
    juint16_t global_max_packet;
    juint16_t xaddr;
    juint16_t yaddr;
    juint8_t  iface;
    juint8_t  alt;
    juint_t   last_filled         : 1;
    juint_t   zero_last_in_packet : 1;
} td243fc_rev2_ep_t;

typedef struct
{
    jdevice_t dev;
    void *core_ctx;
    
    jbus_resource_h mem_res;
    jbus_resource_h irq_res;
    void *interrupt_h;

    juint32_t sys_intr_mask;
    juint32_t ep_intr_mask;

    /* Two logical endpoints for each physical endpoint */
    td243fc_rev2_ep_t ep[TD243FC_EP_NUM*2];

    juint16_t free_mem_start;
    /* DMA buffer for transfers with no requests */
    buffer_t dma_buf;

    jbool_t is_suspend;
    juint16_t address;
    jbool_t pendig_address;
    jbool_t add_to_list;
    juint32_t ep_ready;
    juint32_t sys_intr; 
    juint32_t ep_intr;
    juint32_t fc_cmd;
    jspinlock_h sc_lock;

} td243fc_rev2_softc_t;

#define PUTMACRO(a) a
#define GETOFS(ofs, mask)  ofs
#define GETMASK(ofs, mask)  mask

#define BFSET4(field,val) ((juint32_t)(val) << PUTMACRO(GETOFS field))

#define BFGET4(field,val) (((juint32_t)(val) >> PUTMACRO(GETOFS field)) & \
        PUTMACRO(GETMASK field))

#define BFMASK4(field)    ((juint32_t)(GETMASK field) << PUTMACRO(GETOFS field))

#define WRITE4(reg, val) j_bus_space_write_4(sc->mem_res, reg, val)
#define READ4(reg)       j_bus_space_read_4(sc->mem_res, reg)
#define CLEAR4(reg, val) WRITE4(reg, READ4(reg) & ~(val))
#define SET4(reg, val)   WRITE4(reg, READ4(reg) | (val))

#define TD243FC_EP0_MAX_PACKET_SIZE            64
#define TD243FC_EP0_OUT_BUF_OFFSET             TD243FC_EP0_MAX_PACKET_SIZE
#define TD243FC_EP0_IN_BUF_OFFSET              TD243FC_EP0_MAX_PACKET_SIZE * 2

/* TD243FC registers                         */
/* TD243FC register fields - (offset,mask)   */

/* Chip level registers */

#define TD243FC_HARDWARE_MODE_REG                 0x000
#define TD243FC_FCREV      (24, 0xff) /* FunctionRevision */
#define TD243FC_HCREV      (16, 0xff) /* HostRevision */
#define TD243FC_OTGXCVR    (6, 0x3)   /* OtgTransceiverProperties */
#define TD243FC_HOSTXCVR   (4, 0x3)   /* HostTransceiverProperties */
#define TD243FC_SE_SE  0x3      /* Tx single ended, Rx single ended */
#define TD243FC_SE_DI  0x2      /* Tx single ended, Rx differential */
#define TD243FC_DI_SE  0x1      /* Tx differential, Rx single ended */
#define TD243FC_DI_DI  0x0      /* Tx differential, Rx differential */
#define TD243FC_TSTMDE     (1<<15)    /* TestMode */
#define TD243FC_BEMDE      (1<<2)     /* BigEndianMode */
#define TD243FC_HF         (0, 0x3)   /* HardwareConfiguration */
#define TD243FC_HW_OTG 0x0
#define TD243FC_HOST   0x1
#define TD243FC_DEVICE 0x2
#define TD243FC_SW_OTG 0x3

#define TD243FC_CHIP_INT_STATUS_REG               0x004
#define TD243FC_CHIP_INT_ENABLE_REG               0x008
#define TD243FC_INTHNP     (1<<2)  /* HnpInterrupt */
#define TD243FC_INTF       (1<<1)  /* FunctionControllerInterrupt */
#define TD243FC_INTH       (1<<0)  /* HostControllerInterrupt */

#define TD243FC_CLOCK_CONTROL_REG                 0x00C
#define TD243FC_CPCLK (13, 0x3)  /* ChargePumpClockControl */
#define TD243FC_FCCLK (1<<2)     /* FunctionControllerClockEnable */
#define TD243FC_HCCLK (1<<1)     /* HostControllerClockEnable */

#define TD243FC_RESET_CONTROL_REG                 0x010
#define TD243FC_RESET_CONTROL_LOGIC       (1<<5)
#define TD243FC_RESET_FUNCTION_CONTROLLER (1<<4)
#define TD243FC_RESET_FUNCTION_SIE        (1<<3)
#define TD243FC_HNP_CONTROL_STATUS_REG            0x01C

/* Device controller registers */
#define TD243FC_COMMAND_REG                       0x040
#define TD243FC_SOFTRESET  (1<<7)     /* SoftResetFunctionController */
#define TD243FC_BADISOAP   (1<<3)     /* AcceptBadIsoFrame */
#define TD243FC_SUSPDET    (1<<2)     /* SuspendDetected */
#define TD243FC_RSMINPROG  (1<<1)     /* ResumeInProgress */
#define TD243FC_RESETDET   (1<<0)     /* USBBusResetDetected */

#define TD243FC_DEVICEADDRESS_REG                 0x044
#define TD243FC_DEVADDR    (0, 0x7f)  /* DeviceAddress */

#define TD243FC_SYSTEM_INT_STATUS_REG             0x048
#define TD243FC_SYSTEM_INT_ENABLE_REG             0x04C
#define TD243FC_SOF        (1<<4)  /* SofReceivedInterrupt */
#define TD243FC_DONEREG    (1<<3)  /* DoneRegisterInterrupt */
#define TD243FC_SUSP       (1<<2)  /* SuspendDetectedInterrupt */
#define TD243FC_RESUME     (1<<1)  /* ResumeFinishedInterrupr */
#define TD243FC_BUSRST     (1<<0)  /* BusResetDetectedInterrupt */

/* Even bits - OUT endpoints, Odd bits - IN endpoints */
#define TD243FC_EP_BIT(n, is_in) (1<<((n) * 2 + (is_in)))
#define TD243FC_EP_BIT_INDEX(n) (1<<(n))

#define TD243FC_XBUF_INT_STATUS_REG               0x050

#define TD243FC_YBUF_INT_STATUS_REG               0x054

#define TD243FC_XYBUF_INT_ENABLE_REG              0x058

#define TD243FC_XFILLED_STATUS_REG                0x05C

#define TD243FC_YFILLED_STATUS_REG                0x060

#define TD243FC_EP_ENABLE_REG                     0x064

#define TD243FC_EP_READY_REG                      0x068

#define TD243FC_EP_IMMEDIATE_INTR                 0x06C

#define TD243FC_EP_DONE_STATUS_REG                0x070
#define TD243FC_EP_DONE_ENABLE_REG                0x074

#define TD243FC_EP_TOGGLE_REG                     0x078

#define TD243FC_FRAME_NUMBER_REG                  0x07C
#define TD243FC_FRAME_NUMBER    (0, 0xfff)

/* Due to regiser contention in the TD243 CORE, this register is multiplexed
 * with the FRAME NUMBER register */
#define TD243FC_EP_READY_CLEAR_REG                0x07C

#define TD243FC_EP_CONFIG_BASE_ADDR               0x400
#define TD243FC_EPN_CONFIG_CONTROL_REG(n, is_in) \
    TD243FC_EP_CONFIG_BASE_ADDR + (((n)*2 + (is_in)) * 0x10)
#define TD243FC_STL        (((juint32_t)1)<<31)  /* EndpointStall */
#define TD243FC_SETUP_RECV (1<<30)              /* SetupRecieved */
#define TD243FC_OVERRUN    (1<<29)              /* DataOverrunDetected */
#define TD243FC_MAXPKTSIZ  (16, 0x3ff)          /* MaximumPacketSize */
#define TD243FC_FORMAT     (14, 0x3)            /* EndpointType */
#define TD243FC_CTRL_EP       0x00
#define TD243FC_ISOC_EP       0x01
#define TD243FC_BULK_EP       0x02
#define TD243FC_INTR_EP       0x03

#define TD243FC_EPN_XY_BUFFER_ADDRESS_REG(n, is_in) \
    (TD243FC_EPN_CONFIG_CONTROL_REG(n, is_in)) + 0x4
#define TD243FC_YBSA       (16, 0xffff) /* YBufferStartAddress */
#define TD243FC_XBSA       (0,  0xffff) /* XBufferStartAddress */

#define TD243FC_EPN_PACKET_CONTROL_REG(n, is_in) \
    (TD243FC_EPN_CONFIG_CONTROL_REG(n, is_in)) + 0xC
/* For Control/Bulk/Interrupt */
#define TD243FC_BUFFERSIZE (21, 0x7ff)  /* Size of X/Y Buffer 1-2K bytes */
#define TD243FC_TTLBTECNT  (0, 0x1ffff) /* TotalByteCountToTransfer 1-2Mbytes */
/* For Isochronous */
#define TD243FC_ISO1LEN1   (16, 0x3ff)  /* PacketLength1 */
#define TD243FC_ISO1LEN0   (0,  0x3ff)  /* PacketLength0 */

/* Beginning of 4KB data memory for endpoint buffers */
#define TD243FC_DATA_MEM_BASE                     0x1000

/* DMA registers */
#define TD243FC_DMA_BASE                          0x0800

#define TD243FC_DMA_REV_REG                       TD243FC_DMA_BASE + 0x000
#define TD243FC_DMA_REV    (0, 0xff)

#define TD243FC_DMA_INTR_STATUS_REG               TD243FC_DMA_BASE + 0x004
#define TD243FC_DMA_INTR_ENABLE_REG               TD243FC_DMA_BASE + 0x008
#define TD243FC_DMA_EPERR  (1<<1)       /* EndpointErrorInterrupr */
#define TD243FC_DMA_ETDERR (1<<0)       /* ETDErrorInterrupr */

#define TD243FC_DMA_EP_ERROR_STATUS_REG           TD243FC_DMA_BASE + 0x010

#define TD243FC_DMA_EP_ENABLE_REG                 TD243FC_DMA_BASE + 0x024

#define TD243FC_DMA_EP_XTRIGGER_REG               TD243FC_DMA_BASE + 0x02C

#define TD243FC_DMA_EP_XYTRIGGER_REG              TD243FC_DMA_BASE + 0x034

#define TD243FC_DMA_BURST_ENABLE                  TD243FC_DMA_BASE + 0x03C

#define TD243FC_DMA_EP_CHANNEL_CLEAR_REG          TD243FC_DMA_BASE + 0x048

#define TD243FC_DMA_EP_SYSTEM_MEM_ADDR_REG(n, is_in) \
    TD243FC_DMA_BASE + 0x180 + 4 * (2*(n) + (is_in))

#endif

