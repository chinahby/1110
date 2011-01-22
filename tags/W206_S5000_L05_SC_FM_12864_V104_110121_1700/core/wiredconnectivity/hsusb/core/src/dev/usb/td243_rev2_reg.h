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
#ifndef _TD243HC_R2_REG_H_
#define _TD243HC_R2_REG_H_

/* Controller access macros */
#define PUTMACRO(a) a
#define GETOFS(ofs, mask)  ofs
#define GETMASK(ofs, mask)  mask

#define BFSET4(field,val) ((juint32_t)(val) << PUTMACRO(GETOFS field))

#define BFGET4(field,val) \
    (((juint32_t)(val) >> PUTMACRO(GETOFS field)) & PUTMACRO(GETMASK field))

#define BFMASK4(field)    ((juint32_t)(GETMASK field) << PUTMACRO(GETOFS field))

#define WRITE4(reg, val) j_bus_space_write_4(sc->mem_res, reg, val)
#define READ4(reg)       j_bus_space_read_4(sc->mem_res, reg)
#define CLEAR4(reg, val) WRITE4(reg, READ4(reg) & ~(val))
#define SET4(reg, val)   WRITE4(reg, READ4(reg) | (val))

/* List of HCD registers and HW structures */

/* TD243HC_R2 registers                         */
/* TD243HC_R2 register fields - (offset,mask)   */

/* OTG Module registers */
#define TD243HC_R2_HARDWARE_MODE_REG           0x0000L
#  define TD243HC_R2_HSTREV       (16, 0xff)
#  define TD243HC_R2_HOSTXCVR2    (8,  0x3)
#  define TD243HC_R2_OTGXCVR      (6,  0x3)
#  define TD243HC_R2_HOSTXCVR     (4,  0x3)
#    define TD243HC_R2_XCVR_DD           0x0
#    define TD243HC_R2_XCVR_DS           0x1
#    define TD243HC_R2_XCVR_SD           0x2
#    define TD243HC_R2_XCVR_SS           0x3
#  define TD243HC_R2_CRECFG       (0,  0x3)
#    define TD243HC_R2_HOST_ONLY         0x1

#define TD243HC_R2_OTG_INTR_STAT_REG           0x0004L
#define TD243HC_R2_OTG_INTR_EN_REG             0x0008L
#  define TD243HC_R2_HCINT        (1 << 0)

#define TD243HC_R2_OTG_CLK_CTRL_REG            0x000CL
#  define TD243HC_R2_HSTCLK       (1 << 1)

#define TD243HC_R2_OTG_RESET_CTRL_REG          0x0010L
#  define TD243HC_R2_RSTI2C       (1 << 15)
#  define TD243HC_R2_RSTCTRL      (1 << 5)
#  define TD243HC_R2_RSTRH        (1 << 2)
#  define TD243HC_R2_RSTHSIE      (1 << 1)
#  define TD243HC_R2_RSTHC        (1 << 0)

/* HOST Module registers */
#define TD243HC_R2_CONTROL_REG                 0x0080L 
#  define TD243HC_R2_HCRESET      ((juint32_t)1 << 31)
#  define TD243HC_R2_SHEDOVR      (16, 0x3)
#  define TD243HC_R2_RMTWUEN      (1 << 4)
#  define TD243HC_R2_HCUSBSTE     (2, 0x3)
#    define TD243HC_R2_USB_RESET    0x0
#    define TD243HC_R2_USB_RESUME   0x1
#    define TD243HC_R2_USB_OPER     0x2
#    define TD243HC_R2_USB_SUSPEND  0x3
#  define TD243HC_R2_CTLBLKSR     (0, 0x3)
#    define TD243HC_R2_1_1          0x0
#    define TD243HC_R2_2_1          0x1
#    define TD243HC_R2_3_1          0x2
#    define TD243HC_R2_4_1          0x3

#define TD243HC_R2_SYS_INTR_STAT_REG           0x0088L
#define TD243HC_R2_SYS_INTR_EN_REG             0x008CL
#  define TD243HC_R2_SYS_INTR_RHSC (1<<6)
#  define TD243HC_R2_SYS_INTR_FNO  (1<<5)
#  define TD243HC_R2_SYS_INTR_UE   (1<<4)
#  define TD243HC_R2_SYS_INTR_RD   (1<<3)
#  define TD243HC_R2_SYS_INTR_SF   (1<<2)
#  define TD243HC_R2_SYS_INTR_RDQ  (1<<1)
#  define TD243HC_R2_SYS_INTR_SO   (1<<0)

#define TD243HC_R2_XBUF_INT_STATUS_REG         0x0098L
#define TD243HC_R2_YBUF_INT_STATUS_REG         0x009CL
#define TD243HC_R2_XY_INT_ENA_REG              0x00A0L
#define TD243HC_R2_XFILLED_STATUS_REG          0x00A8L
#define TD243HC_R2_YFILLED_STATUS_REG          0x00ACL
#  define TD243HC_R2_XYBUF(n)        (1 << (n))

#define TD243HC_R2_ETD_ENA_REG                 0x00C0L
#define TD243HC_R2_ETD_ENA_CLR_REG             0x00C4L
#define TD243HC_R2_IMMEDIATE_INT_REG           0x00CCL
#define TD243HC_R2_ETD_DONE_STATUS_REG         0x00D0L
#define TD243HC_R2_ETD_DONE_ENA_REG            0x00D4L
#  define TD243HC_R2_ETD(n)          (1 << (n))

#define TD243HC_R2_FRAME_NUMBER_REG            0x00E0L
#  define TD243HC_R2_FRMNUMB            (0, 0xffff)

#define TD243HC_R2_LOW_SPEED_THRESHOLD_REG     0x00E4L
#  define TD243HC_R2_LSTHRESH           (0, 0x7ff)

#define TD243HC_R2_ROOTHUB_DESC_A_REG          0x00E8L
#  define TD243HC_R2_PWRTOGOOD          (24, 0xff)
#  define TD243HC_R2_NOOVRCURP          (1 << 12)
#  define TD243HC_R2_OVRCURPM           (1 << 11)
#  define TD243HC_R2_DEVTYPE            (1 << 10)
#  define TD243HC_R2_PWRSWTMD           (1 << 9)
#  define TD243HC_R2_NOPWRSWT           (1 << 8)
#  define TD243HC_R2_NDNSTMPRT          (0, 0xff)

#define TD243HC_R2_ROOTHUB_DESC_B_REG          0x00ECL
#  define TD243HC_R2_PRTPWRCM           (16, 0xff)
#  define TD243HC_R2_DEVREMOVE          (0,  0xff)

#define TD243HC_R2_ROOTHUB_STATUS_REG          0x00F0L
#  define TD243HC_R2_CLRMTWUE           ((juint32_t)1 << 31)
#  define TD243HC_R2_OVRCURCHG          (1 << 17)
#  define TD243HC_R2_DEVCONWUE          (1 << 15)
#  define TD243HC_R2_OVRCURI            (1 << 1)
#  define TD243HC_R2_LOCPWRS            (1 << 0)

#define TD243HC_R2_PORT_1_STATUS_REG           0x00F4L
#define TD243HC_R2_PORT_2_STATUS_REG           0x00F8L
#define TD243HC_R2_PORT_3_STATUS_REG           0x00FCL
#define TD243HC_R2_PORT_STATUS_REG(n) \
    TD243HC_R2_PORT_1_STATUS_REG + ((n - 1) * 4)
#  define TD243HC_R2_PORTSC_PRSC        (1 << 20)
#  define TD243HC_R2_PORTSC_OCIC        (1 << 19)
#  define TD243HC_R2_PORTSC_PSSC        (1 << 18)
#  define TD243HC_R2_PORTSC_PESC        (1 << 17)
#  define TD243HC_R2_PORTSC_CSC         (1 << 16)
#  define TD243HC_R2_PORTSC_LSDA        (1 << 9)
#  define TD243HC_R2_PORTSC_PPS         (1 << 8)
#  define TD243HC_R2_PORTSC_PRS         (1 << 4)
#  define TD243HC_R2_PORTSC_POCI        (1 << 3)
#  define TD243HC_R2_PORTSC_PSS         (1 << 2)
#  define TD243HC_R2_PORTSC_PES         (1 << 1)
#  define TD243HC_R2_PORTSC_CCS         (1 << 0)
#define TD243HC_R2_MAX_PORTS           0x3

#define TD243HC_R2_ETD_MEM_MAP                 0x0200L 
#  define TD243HC_R2_ETD_ADDR(n) \
    TD243HC_R2_ETD_MEM_MAP + ((n) * 4 * (sizeof(juint32_t)))

#define TD243HC_R2_DMA_REV_REG                 0x0800L
#  define TD243HC_R2_DMA_REVCODE        (0, 0xff)
        
#define TD243HC_R2_DMA_INTR_STATUS_REG         0x0804L
#define TD243HC_R2_DMA_INTR_ENABLE_REG         0x0808L
#  define TD243HC_R2_DMA_ETDERR         (1 << 0)
        
#define TD243HC_R2_ETD_DMA_ERR_STATUS_REG      0x080CL
#define TD243HC_R2_ETD_DMA_ENABLE_REG          0x0820L
#define TD243HC_R2_ETD_DMA_ENA_X_TRIG_REQ_REG  0x0828L
#define TD243HC_R2_ETD_DMA_ENA_XY_TRIG_REQ_REG 0x0830L
#define TD243HC_R2_ETD_DMA_BURST_ENA_REG       0x0838L
#define TD243HC_R2_ETD_DMA_CHAN_CLR_REG        0x0848L
#  define TD243HC_R2_ETD_DMA(n)          (1 << (n))
        
    
#define TD243HC_R2_ETD_0_SYS_MEM_START_ADDR_REG 0x0900L
#  define TD243HC_R2_ETD_SYS_MEM_START_ADDR_REG(n) \
    TD243HC_R2_ETD_0_SYS_MEM_START_ADDR_REG + ((n) * 4)
#define TD243HC_R2_ETDSMSA              (0, 0xffffffff)
    
#define TD243HC_R2_ETD_0_DMA_BUF_XFER_PTR_REG  0x0A00L
#  define TD243HC_R2_ETD_DMA_BUF_XFER_PTR_REG(n) \
    ETD_0_DMA_BUF_XFER_PTR_REG + ((n) * 4)

#define TD243HC_R2_DATA_MEMORY                 0x1000L
#endif
