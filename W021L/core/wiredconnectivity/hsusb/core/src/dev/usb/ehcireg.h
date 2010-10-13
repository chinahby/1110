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
/*
 * Copyright (c) 2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Lennart Augustsson (lennart@augustsson.net) and by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEV_PCI_EHCIREG_H_
#define _DEV_PCI_EHCIREG_H_

/*** PCI config registers ***/

#define PCI_CBMEM               0x10    /* configuration base MEM */

#define PCI_INTERFACE_EHCI      0x20

#define PCI_USBREV              0x60    /* RO USB protocol revision */
#define  PCI_USBREV_MASK        0xff
#define  PCI_USBREV_PRE_1_0     0x00
#define  PCI_USBREV_1_0         0x10
#define  PCI_USBREV_1_1         0x11
#define  PCI_USBREV_2_0         0x20

#define PCI_EHCI_FLADJ          0x61    /*RW Frame len adj, SOF=59488+6*fladj */

#define PCI_EHCI_PORTWAKECAP    0x62    /* RW Port wake caps (opt)  */

/* EHCI Extended Capabilities */
#define EHCI_EC_LEGSUP          0x01

#define EHCI_EECP_NEXT(x)       (((x) >> 8) & 0xff)
#define EHCI_EECP_ID(x)         ((x) & 0xff)

/* Legacy support extended capability */
#define EHCI_LEGSUP_LEGSUP      0x01
#define  EHCI_LEGSUP_OSOWNED    0x01000000UL /* OS owned semaphore */
#define  EHCI_LEGSUP_BIOSOWNED  0x00010000UL /* BIOS owned semaphore */
#define EHCI_LEGSUP_USBLEGCTLSTS 0x04

/* Regs at EECP + offset */
#define PCI_EHCI_USBLEGSUP      0x00
#define  EHCI_LEG_HC_OS_OWNED           0x01000000UL
#define  EHCI_LEG_HC_BIOS_OWNED         0x00010000UL
#define PCI_EHCI_USBLEGCTLSTS   0x04

#define EHCI_CAP_GET_ID(cap) ((cap) & 0xff)
#define EHCI_CAP_GET_NEXT(cap) (((cap) >> 8) & 0xff)
#define EHCI_CAP_ID_LEGACY 1

/*** EHCI capability registers ***/

#define EHCI_CAPLENGTH          0x00    /*RO Capability register length field */
/* reserved                     0x01 */
#define EHCI_HCIVERSION         0x02    /* RO Interface version number */

#define EHCI_HCSPARAMS          0x04    /* RO Structural parameters */
#define  EHCI_HCS_DEBUGPORT(x)  (((x) >> 20) & 0xf)
#define  EHCI_HCS_P_INDICATOR(x) ((x) & 0x10000)
#define  EHCI_HCS_N_CC(x)       (((x) >> 12) & 0xf) /* # of companion ctlrs */
#define  EHCI_HCS_N_PCC(x)      (((x) >> 8) & 0xf) /* # of ports per comp. */
#define  EHCI_HCS_PPC(x)        ((x) & 0x10) /* port power control */
#define  EHCI_HCS_N_PORTS(x)    ((x) & 0xf) /* # of ports */

#define EHCI_HCCPARAMS          0x08    /* RO Capability parameters */
#define  EHCI_HCC_EECP(x)       (((x) >> 8) & 0xff) /* extended ports caps */
#define  EHCI_HCC_IST(x)        (((x) >> 4) & 0xf) /* isoc sched threshold */
#define  EHCI_HCC_ASPC(x)       ((x) & 0x4) /* async sched park cap */
#define  EHCI_HCC_PFLF(x)       ((x) & 0x2) /* prog frame list flag */
#define  EHCI_HCC_64BIT(x)      ((x) & 0x1) /* 64 bit address cap */

#define EHCI_HCSP_PORTROUTE     0x0c    /*RO Companion port route description */

/* EHCI operational registers.  Offset given by EHCI_CAPLENGTH register */
#define EHCI_USBCMD             0x00    /* RO, RW, WO Command register */
#define  EHCI_CMD_ITC_M         0x00ff0000UL /* RW interrupt threshold ctrl */
#define   EHCI_CMD_ITC_1        0x00010000UL
#define   EHCI_CMD_ITC_2        0x00020000UL
#define   EHCI_CMD_ITC_4        0x00040000UL
#define   EHCI_CMD_ITC_8        0x00080000UL
#define   EHCI_CMD_ITC_16       0x00100000UL
#define   EHCI_CMD_ITC_32       0x00200000UL
#define   EHCI_CMD_ITC_64       0x00400000UL
#define  EHCI_CMD_ASPME         0x00000800UL /* RW/RO async park enable */
#define  EHCI_CMD_ASPMC         0x00000300UL /* RW/RO async park count */
#define  EHCI_CMD_LHCR          0x00000080UL /* RW light host ctrl reset */
#define  EHCI_CMD_IAAD          0x00000040UL /* RW intr on async adv door bell */
#define  EHCI_CMD_ASE           0x00000020UL /* RW async sched enable */
#define  EHCI_CMD_PSE           0x00000010UL /* RW periodic sched enable */
#define  EHCI_CMD_FLS_M         0x0000000cUL /* RW/RO frame list size */
#define  EHCI_CMD_FLS(x)        (((x) >> 2) & 3) /* RW/RO frame list size */
#define  EHCI_CMD_HCRESET       0x00000002UL /* RW reset */
#define  EHCI_CMD_RS            0x00000001UL /* RW run/stop */

#define EHCI_USBSTS             0x04    /* RO, RW, RWC Status register */
#define  EHCI_STS_ASS           0x00008000UL /* RO async sched status */
#define  EHCI_STS_PSS           0x00004000UL /* RO periodic sched status */
#define  EHCI_STS_REC           0x00002000UL /* RO reclamation */
#define  EHCI_STS_HCH           0x00001000UL /* RO host controller halted */
#define  EHCI_STS_IAA           0x00000020UL /* RWC interrupt on async adv */
#define  EHCI_STS_HSE           0x00000010UL /* RWC host system error */
#define  EHCI_STS_FLR           0x00000008UL /* RWC frame list rollover */
#define  EHCI_STS_PCD           0x00000004UL /* RWC port change detect */
#define  EHCI_STS_ERRINT        0x00000002UL /* RWC error interrupt */
#define  EHCI_STS_INT           0x00000001UL /* RWC interrupt */
#define  EHCI_STS_INTRS(x)      ((x) & 0x3f)

#define EHCI_NORMAL_INTRS (EHCI_STS_IAA | EHCI_STS_HSE | EHCI_STS_PCD | EHCI_STS_ERRINT | EHCI_STS_INT | EHCI_STS_FLR)

#define EHCI_USBINTR            0x08    /* RW Interrupt register */
#define EHCI_INTR_IAAE          0x00000020UL /* interrupt on async advance ena */
#define EHCI_INTR_HSEE          0x00000010UL /* host system error ena */
#define EHCI_INTR_FLRE          0x00000008UL /* frame list rollover ena */
#define EHCI_INTR_PCIE          0x00000004UL /* port change ena */
#define EHCI_INTR_UEIE          0x00000002UL /* USB error intr ena */
#define EHCI_INTR_UIE           0x00000001UL /* USB intr ena */

#define EHCI_FRINDEX            0x0c    /* RW Frame Index register */

#define EHCI_CTRLDSSEGMENT      0x10    /* RW Control Data Structure Segment */

#define EHCI_PERIODICLISTBASE   0x14    /* RW Periodic List Base */
#define EHCI_ASYNCLISTADDR      0x18    /* RW Async List Base */

#define EHCI_CONFIGFLAG         0x40    /* RW Configure Flag register */
#define  EHCI_CONF_CF           0x00000001UL /* RW configure flag */

#define EHCI_PORTSC(n)          (0x40+4*(n)) /* RO, RW, RWC Port Status reg */
#ifdef CONFIG_TDI_4X
#define  TDI_4X_PFSC            0x01000000 /* RW Port Force Full Speed 
                                              Connect */
#define  TDI_4X_PSPD            0x0C000000UL /* RO, Port speed*/
#define  TDI_4X_IS_HISPD(x)     (((x) & TDI_4X_PSPD) == 0x08000000UL)
#define  TDI_4X_IS_LOWSPD(x)    (((x) & TDI_4X_PSPD) == 0x04000000UL)
#define  TDI_4X_IS_FULLSPD(x)   (((x) & TDI_4X_PSPD) == 0x0UL)


#endif
#define  EHCI_PS_WKOC_E         0x00400000UL /* RW wake on over current ena */
#define  EHCI_PS_WKDSCNNT_E     0x00200000UL /* RW wake on disconnect ena */
#define  EHCI_PS_WKCNNT_E       0x00100000UL /* RW wake on connect ena */
#define  EHCI_PS_PTC            0x000f0000UL /* RW port test control */
#define  EHCI_PS_TEST_J_STATE   0x00010000UL /* RW J-State test mode */
#define  EHCI_PS_TEST_K_STATE   0x00020000UL /* RW K_State test mode */
#define  EHCI_PS_TEST_SE0_NAK   0x00030000UL /* RW SE0_NAK test mode */
#define  EHCI_PS_TEST_PACKET    0x00040000UL /* RW Packet test mode */
#define  EHCI_PS_TEST_FORCE_ENABLE_HS 0x00050000UL /* RW Force Enable test 
                                                      mode */
#define  EHCI_PS_PIC            0x0000c000UL /* RW port indicator control */
#define  EHCI_PS_PO             0x00002000UL /* RW port owner */
#define  EHCI_PS_PP             0x00001000UL /* RW,RO port power */
#define  EHCI_PS_LS             0x00000c00UL /* RO line status */
#define  EHCI_PS_IS_LOWSPEED(x) (((x) & EHCI_PS_LS) == 0x00000400UL)
#define  EHCI_PS_PR             0x00000100UL /* RW port reset */
#define  EHCI_PS_SUSP           0x00000080UL /* RW suspend */
#define  EHCI_PS_FPR            0x00000040UL /* RW force port resume */
#define  EHCI_PS_OCC            0x00000020UL /* RWC over current change */
#define  EHCI_PS_OCA            0x00000010UL /* RO over current active */
#define  EHCI_PS_PEC            0x00000008UL /* RWC port enable change */
#define  EHCI_PS_PE             0x00000004UL /* RW port enable */
#define  EHCI_PS_CSC            0x00000002UL /* RWC connect status change */
#define  EHCI_PS_CS             0x00000001UL /* RO connect status */
#define  EHCI_PS_CLEAR          (EHCI_PS_OCC|EHCI_PS_PEC|EHCI_PS_CSC)
#ifdef CONFIG_TDI_4X
#define TDI_4X_USBMODE              0x68
#define TDI_4X_USBMODE_DEVICE       0x2
#define TDI_4X_USBMODE_HOST         0x3
#define TDI_4X_USBMODE_BIGENDIAN    0x4
#define TDI_4X_USBMODE_SDIS_ACTIVE  0x10

#define TDI_4X_TTCTRL               0x1C
#define TDI_4X_BURST_SIZE           0x20
#define TDI_4X_BURST_SIZE_QUIRK     0x00000404UL
#endif

#define EHCI_PORT_RESET_COMPLETE 2 /* ms */

#define EHCI_FLALIGN_ALIGN      0x1000UL
#define EHCI_MAX_PORTS          16 /* only 4 bits available in EHCI_HCS_N_PORTS */

/* No data structure may cross a page boundary. */
#define EHCI_PAGE_SIZE 0x1000UL
#define EHCI_PAGE(x) ((x) &~ 0xfff)
#define EHCI_PAGE_OFFSET(x) ((x) & 0xfff)
#define EHCI_PAGE_MASK(x) ((x) & 0xfff)

typedef juint32_t ehci_link_t;
#define EHCI_LINK_TERMINATE     0x00000001UL
#define EHCI_LINK_TYPE(x)       ((x) & 0x00000006UL)
#define  EHCI_LINK_ITD          0x0
#define  EHCI_LINK_QH           0x2
#define  EHCI_LINK_SITD         0x4
#define  EHCI_LINK_FSTN         0x6
#define EHCI_LINK_ADDR(x)       ((x) &~ 0x1fUL)

typedef juint32_t ehci_physaddr_t;

#define BSET(x, shift, mask) ((juint32_t)(x) << (shift))
#define BGET(x, shift, mask) (((juint32_t)(x) >> (shift)) & (mask))
#define BMASK(x, shift, mask) ((juint32_t)(mask) << (shift))

/* Isochronous Transfer Descriptor */
#define EHCI_ITD_NTRANS     8
#define EHCI_ITD_NBUFFERS   7
typedef struct {
    ehci_link_t itd_next;
    juint32_t    itd_trans[EHCI_ITD_NTRANS];
#define EHCI_ITD_STATUS(gs, x) gs(x, 28, 0xf)
#define EHCI_ITD_ACTIVE  0x08
#define EHCI_ITD_BUFERR  0x04
#define EHCI_ITD_BABBLE  0x02
#define EHCI_ITD_XACTERR 0x01
#define EHCI_ITD_LEN(gs, x) gs(x, 16, 0xfff)
#define EHCI_ITD_IOC(gs, x) gs(x, 15, 0x1)
#define EHCI_ITD_PG(gs, x)  gs(x, 12, 0x7)
#define EHCI_ITD_OFS(gs, x) gs(x,  0, 0xfff)
    juint32_t    itd_buffer[EHCI_ITD_NBUFFERS];
#define EHCI_ITD_BUFP(gs, x)     gs(x, 12, 0xfffffUL)
#define EHCI_ITD_BUFP_0   0 
#define EHCI_ITD_EPT(gs, x)      gs(x,  8, 0xf)
#define EHCI_ITD_DEVADDR(gs, x)  gs(x,  0, 0x7f)
#define EHCI_ITD_BUFP_1   1 
#define EHCI_ITD_DIR(gs, x)      gs(x,  11, 0x1)
#define EHCI_ITD_OUT  0
#define EHCI_ITD_IN   1
#define EHCI_ITD_MAXPKT(gs, x)   gs(x,  0, 0x7ff)
#define EHCI_ITD_BUFP_2   2 
#define EHCI_ITD_MULT(gs, x)     gs(x,  0, 0x3)
    juint32_t    itd_buffer_hi[EHCI_ITD_NBUFFERS];
} ehci_itd_t;
#ifdef CONFIG_CX2417X
#define EHCI_ITD_ALIGN 64
#else
#define EHCI_ITD_ALIGN 32
#endif
/* Split Transaction Isochronous Transfer Descriptor */
#define EHCI_SITD_NBUFFERS 2
typedef struct {
    ehci_link_t sitd_next;
    juint32_t      sitd_addr;
#define EHCI_SITD_DIR(gs, x)       gs(x, 31, 0x01)
#define EHCI_SITD_OUT  0
#define EHCI_SITD_IN   1
#define EHCI_SITD_PORTNUM(gs, x)   gs(x, 24, 0x3f)
#define EHCI_SITD_HUBADDR(gs, x)   gs(x, 16, 0x3f)
#define EHCI_SITD_EPT(gs, x)       gs(x,  8, 0x0f)
#define EHCI_SITD_DEVADDR(gs, x)   gs(x,  0, 0x3f)
    juint32_t      sitd_mask;
#define EHCI_SITD_CMASK(gs, x)     gs(x, 8, 0xff)
#define EHCI_SITD_SMASK(gs, x)     gs(x, 0, 0xff)
    juint32_t      sitd_status;
#define EHCI_SITD_IOC(gs, x)       gs(x, 31, 0x1)
#define EHCI_SITD_PG(gs, x)        gs(x, 30, 0x1)
#define EHCI_SITD_LEN(gs, x)       gs(x, 16, 0x3ff)
#define EHCI_SITD_CPROGMASK(gs, x) gs(x, 8,  0xff)
#define EHCI_SITD_STATUS(gs, x)    gs(x, 0,  0xff)
#define EHCI_SITD_ACTIVE   0x80
#define EHCI_SITD_ERR      0x40
#define EHCI_SITD_BUFERR   0x20
#define EHCI_SITD_BABBLE   0x10
#define EHCI_SITD_XACTERR  0x08
#define EHCI_SITD_MISSEDUF 0x04
#define EHCI_SITD_COMPLETE 0x02
#define EHCI_SITD_START    0x00
    juint32_t      sitd_buffer[EHCI_SITD_NBUFFERS];
#define EHCI_SITD_BUFP(gs, x)      gs(x, 12, 0xfffffUL)
#define EHCI_SITD_BUFP_0 0
#define EHCI_SITD_OFS(gs, x)       gs(x, 0, 0xfff)
#define EHCI_SITD_BUFP_1 1
#define EHCI_SITD_TPOS(gs, x)      gs(x, 3, 0x3)
#define EHCI_SITD_TCOUNT(gs, x)    gs(x, 0, 0x7)
    juint32_t      sitd_back;
    juint32_t      sitd_buffer_hi[EHCI_SITD_NBUFFERS];
} ehci_sitd_t;
#define EHCI_SITD_ALIGN 32

/* Queue Element Transfer Descriptor */
#define EHCI_QTD_NBUFFERS 5
typedef struct {
    ehci_link_t qtd_next;
    ehci_link_t qtd_altnext;
    juint32_t    qtd_status;
#define EHCI_QTD_GET_STATUS(x)  (((x) >>  0) & 0xff)
#define EHCI_QTD_SET_STATUS(x)  ((x) <<  0)
#define  EHCI_QTD_ACTIVE        0x80UL
#define  EHCI_QTD_HALTED        0x40UL
#define  EHCI_QTD_BUFERR        0x20UL
#define  EHCI_QTD_BABBLE        0x10UL
#define  EHCI_QTD_XACTERR       0x08UL
#define  EHCI_QTD_MISSEDMICRO   0x04UL
#define  EHCI_QTD_SPLITXSTATE   0x02UL
#define  EHCI_QTD_PINGSTATE     0x01UL
#define  EHCI_QTD_STATERRS      0x3cUL
#define EHCI_QTD_GET_PID(x)     (((x) >>  8) & 0x3)
#define EHCI_QTD_SET_PID(x)     ((juint32_t)(x) <<  8)
#define  EHCI_QTD_PID_OUT       0x0UL
#define  EHCI_QTD_PID_IN        0x1UL
#define  EHCI_QTD_PID_SETUP     0x2UL
#define EHCI_QTD_GET_CERR(x)    (((x) >> 10) &  0x3)
#define EHCI_QTD_SET_CERR(x)    ((juint32_t)(x) << 10)
#define EHCI_QTD_GET_C_PAGE(x)  (((x) >> 12) &  0x7)
#define EHCI_QTD_SET_C_PAGE(x)  ((juint32_t)(x) << 12)
#define EHCI_QTD_GET_IOC(x)     (((x) >> 15) &  0x1)
#define EHCI_QTD_IOC            0x00008000UL
#define EHCI_QTD_GET_BYTES(x)   (((x) >> 16) &  0x7fff)
#define EHCI_QTD_SET_BYTES(x)   ((juint32_t)(x) << 16)
#define EHCI_QTD_GET_TOGGLE(x)  (((x) >> 31) &  0x1)
#define EHCI_QTD_SET_TOGGLE(x)  ((juint32_t)(x) << 31)
#define EHCI_QTD_TOGGLE_MASK    0x80000000UL
        ehci_physaddr_t qtd_buffer[EHCI_QTD_NBUFFERS];
        ehci_physaddr_t qtd_buffer_hi[EHCI_QTD_NBUFFERS];
} ehci_qtd_t;
#define EHCI_QTD_ALIGN 32

/* Queue Head */
typedef struct {
    ehci_link_t qh_link;
    juint32_t    qh_endp;
#define EHCI_QH_GET_ADDR(x)     (((x) >>  0) & 0x7f) /* endpoint addr */
#define EHCI_QH_SET_ADDR(x)     (x)
#define EHCI_QH_ADDRMASK        0x0000007fUL
#define EHCI_QH_GET_INACT(x)    (((x) >>  7) & 0x01) /* inactivate on next */
#define EHCI_QH_INACT           0x00000080UL
#define EHCI_QH_GET_ENDPT(x)    (((x) >>  8) & 0x0f) /* endpoint no */
#define EHCI_QH_SET_ENDPT(x)    ((juint32_t)(x) <<  8)
#define EHCI_QH_GET_EPS(x)      (((x) >> 12) & 0x03) /* endpoint speed */
#define EHCI_QH_SET_EPS(x)      ((juint32_t)(x) << 12)
#define  EHCI_QH_SPEED_FULL     0x0
#define  EHCI_QH_SPEED_LOW      0x1
#define  EHCI_QH_SPEED_HIGH     0x2
#define EHCI_QH_GET_DTC(x)      (((x) >> 14) & 0x01) /* data toggle control */
#define EHCI_QH_DTC             0x00004000UL
#define EHCI_QH_GET_HRECL(x)    (((x) >> 15) & 0x01) /* head of reclamation */
#define EHCI_QH_HRECL           0x00008000UL
#define EHCI_QH_GET_MPL(x)      (((x) >> 16) & 0x7ff) /* max packet len */
#define EHCI_QH_SET_MPL(x)      ((juint32_t)(x) << 16)
#define EHCI_QH_MPLMASK         0x07ff0000UL
#define EHCI_QH_GET_CTL(x)      (((x) >> 27) & 0x01) /* control endpoint */
#define EHCI_QH_CTL             0x08000000UL
#define EHCI_QH_GET_NRL(x)      (((x) >> 28) & 0x0f) /* NAK reload */
#define EHCI_QH_SET_NRL(x)      ((juint32_t)(x) << 28)
        juint32_t        qh_endphub;
#define EHCI_QH_GET_SMASK(x)    (((x) >>  0) & 0xff) /* intr sched mask */
#define EHCI_QH_SET_SMASK(x)    ((juint32_t)(x) <<  0)
#define EHCI_QH_GET_CMASK(x)    (((x) >>  8) & 0xff) /* split completion mask */
#define EHCI_QH_SET_CMASK(x)    ((juint32_t)(x) <<  8)
#define EHCI_QH_GET_HUBA(x)     (((x) >> 16) & 0x7f) /* hub address */
#define EHCI_QH_SET_HUBA(x)     ((juint32_t)(x) << 16)
#define EHCI_QH_GET_PORT(x)     (((x) >> 23) & 0x7f) /* hub port */
#define EHCI_QH_SET_PORT(x)     ((juint32_t)(x) << 23)
#define EHCI_QH_GET_MULT(x)     (((x) >> 30) & 0x03) /* pipe multiplier */
#define EHCI_QH_SET_MULT(x)     ((juint32_t)(x) << 30)
        ehci_link_t     qh_curqtd;
        ehci_qtd_t      qh_qtd;
} ehci_qh_t;
#ifdef CONFIG_CX2417X
#define EHCI_QH_ALIGN 64
#else
#define EHCI_QH_ALIGN 32
#endif

/* Periodic Frame Span Traversal Node */
typedef struct {
    ehci_link_t fstn_link;
    ehci_link_t fstn_back;
} ehci_fstn_t;
#define EHCI_FSTN_ALIGN 32

#define EHCI_OTG_PORT_NUM 1

#endif /* _DEV_PCI_EHCIREG_H_ */
