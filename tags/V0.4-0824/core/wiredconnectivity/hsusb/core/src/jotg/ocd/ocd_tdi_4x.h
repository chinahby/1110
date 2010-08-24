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
#ifndef _OCD_TDI_4X_H_
#define _OCD_TDI_4X_H_

typedef struct tdi4x_ocd_sc_t {
    /* resources */
    jbus_resource_h reg_res;
    jbus_resource_h irq_res;

    /* OTG Core Context */
    otg_core_ctx_t otg_core_ctx;

    juint32_t intr_msk;

    jbool_t ack_pcd;
    jbool_t is_dying;

    /* Interrupt setup handle */
    void *intr_hndl;

    /* Internal Timer parameters */
    jspinlock_h timer_lock;
    juint32_t timer_time;
    void (*timer_func)(struct tdi4x_ocd_sc_t *sc);

    /* b_se0_srp detection patameters */
    jbool_t waiting_for_se0_srp;
    jbool_t is_se0_srp;

    /* Handles for the DCD and HCD */
    jdevice_t dcd_dev;
    jdevice_t hcd_dev;
    jdevice_t ocd_dev;

    jbool_t full_speed_only;
} tdi4x_ocd_sc_t;

/* TDI_4X USB device registers */
#define TDI_4X_OTG_ID_REG       0x000

#define TDI_4X_OTG_SC_ENB2STS(x) (((x) >> 8) & 0x7F0000)
#define TDI_4X_OTG_SC           0x1A4
/* OTG_SC Interrupt Enable bits - Read/Write */
#define TDI_4X_OTG_SC_ENB_LOC           24
#define TDI_4X_OTG_SC_ENB_DEF           0x1F000000 /* All interrupts that are 
                                                      enabled by default */
#define TDI_4X_OTG_SC_DPIE              0x40000000 /* Data pulse IE */
#define TDI_4X_OTG_SC_1MSIE             0x20000000 /* 1 ms timer IE */
#define TDI_4X_OTG_SC_BSEIE             0x10000000 /* B-Session End IE */
#define TDI_4X_OTG_SC_BSVIE             0x08000000 /* B-Session Valid IE */
#define TDI_4X_OTG_SC_ASVIE             0x04000000 /* A-Session Valid IE */
#define TDI_4X_OTG_SC_AVVIE             0x02000000 /* A VBus Valid IE */
#define TDI_4X_OTG_SC_IDIE              0x01000000 /* USB ID IE */

/* OTG_SC Interrupt Status bits - Read/Write to clear */
#define TDI_4X_OTG_SC_IS_LOC            16
#define TDI_4X_OTG_SC_DPIS              0x00400000 /* Data pulse IS */
#define TDI_4X_OTG_SC_1MSIS             0x00200000 /* 1 ms timer IS */
#define TDI_4X_OTG_SC_BSEIS             0x00100000 /* B-Session End IS */
#define TDI_4X_OTG_SC_BSVIS             0x00080000 /* B-Session Valid IS */
#define TDI_4X_OTG_SC_ASVIS             0x00040000 /* A-Session Valid IS */
#define TDI_4X_OTG_SC_AVVIS             0x00020000 /* A VBus Valid IS */
#define TDI_4X_OTG_SC_IDIS              0x00010000 /* USB ID IS */

/* OTG_SC Interrupt Status Input bits - Read only */
#define TDI_4X_OTG_SC_DPS               0x00004000 /* Data pulse Status */
#define TDI_4X_OTG_SC_1MSS              0x00002000 /* 1 ms timer Status */
#define TDI_4X_OTG_SC_BSE               0x00001000 /* B-Session End Status */
#define TDI_4X_OTG_SC_BSV               0x00000800 /* B-Session Valid Status */
#define TDI_4X_OTG_SC_ASV               0x00000400 /* A-Session Valid Status */
#define TDI_4X_OTG_SC_AVV               0x00000200 /* A VBus Valid Status */
#define TDI_4X_OTG_SC_ID                0x00000100 /* USB ID Status */

/* OTG_SC Hardware Assist bits - Read/Write */
#define TDI_4X_OTG_SC_HABA              0x00000080 /* HW Assist B-Disconnect to
                                                      A-Connect */
#define TDI_4X_OTG_SC_HADP              0x00000040 /* HW Assist Data-Pulse */
#define TDI_4X_OTG_SC_IDPU         0x00000020 /* ID Pullup 
                                                      (1 = on default) */
#define TDI_4X_OTG_SC_DP           0x00000010 /* Data Pulsing during 
                                                      SRP */
#define TDI_4X_OTG_SC_OT           0x00000008 /* OTG Termination */
#define TDI_4X_OTG_SC_HAAR              0x00000004 /* HW Assist Auto-Reset */
#define TDI_4X_OTG_SC_VC           0x00000002 /* VBUS Charge */
#define TDI_4X_OTG_SC_VD           0x00000001 /* VBUS Discharge */

/* General Purpose Timer Registers */
/* Interrupt Enable Register and Bits */
#define TDI_4X_USBINTR_REG              0x148
#define TDI_4X_USBINTR_TI0_ENB          0x01000000 
#define TDI_4X_USBINTR_TI1_ENB          0x02000000 
#define TDI_4X_USBINTR_OTG_ENB          0x03000000 
#define TDI_4X_USBINTR_PCD_ENB          0x00000004
/* General Purpose Timer Time Load Register */
#define TDI_4X_GPTIMERLD_REG(num)      (0x80 + (num) * 8)

/* General Purpose Timer Control Register */
#define TDI_4X_GPTIMERCTRL_REG(num)    (0x84 + (num) * 8)
#define TDI_4X_GPTIMERCTRL_GTPRUN      0x80000000
#define TDI_4X_GPTIMERCTRL_GTPRST      0x40000000
#define TDI_4X_GPTIMERCTRL_GTPMODE     0x01000000
#define TDI_4X_GPTIMERCTRL_GTPCNT      0x00FFFFFF

#define TDI_4X_USBSTS_REG               0x144
#define TDI_4X_USBSTS_TI0               0x01000000
#define TDI_4X_USBSTS_TI1               0x02000000
#define TDI_4X_USBSTS_PCI               0x4
#define TDI_4X_USBSTS_SLI               0x100

/* USB Mode register */
#define TDI_4X_MODE_REG                 0x1A8
#define TDI_4X_MODE_VBPS                0x20
#define TDI_4X_MODE_SDIS                0x10
#define TDI_4X_MODE_SLOM                0x08
#define TDI_4X_MODE_ES                  0x04
#define TDI_4X_MODE_CM                  0x03
/* Possible values for CM */
#define TDI_4X_MODE_CM_IDLE             0x0
#define TDI_4X_MODE_CM_HOST             0x3
#define TDI_4X_MODE_CM_DEVICE           0x2

/* USB Command Register */
#define TDI_4X_USB_CMD_REG              0x140
#define TDI_4X_USB_CMD_RST              0x2

/* HSU addition */
#define TDI_4X_ULPI_VIEWPORT_REG        0x170
#define TDI_4X_ULPI_VIEWPORT_ULPIRUN_BMSK           0x40000000
#define TDI_4X_ULPI_VIEWPORT_ULPIRW_BMSK            0x20000000
#define TDI_4X_ULPI_VIEWPORT_ULPIADDR_SHFT          0x10
#define TDI_4X_ULPI_VIEWPORT_ULPIDATWR_SHFT         0

/* USB Port SC 0 */
#define TDI_4X_PORT_SC_REG              0x184
#define TDI_4X_PORT_SC_PFSC             0x01000000
#define TDI_4X_PORT_SC_PE               0x4
#define TDI_4X_PORT_SC_PR               0x100
#define TDI_4X_PORT_SC_PP               0x1000
#define TDI_4X_PORT_SC_CSC              0x2
#define TDI_4X_PORT_SC_CCS              0x1
#define TDI_4X_PORT_SC_FPR              0x40
#define TDI_4X_PORT_SC_SUSP             0x80
#define TDI_4X_PORT_SC_NOTIFY           0x42 /* TDI_4X_PORT_SC_CSC |
                                                TDI_4X_PORT_SC_FPR */
/* HSU Addition */
#define TDI_4X_PORT_SC_LS               0xc00
#define TDI_4X_PORT_SC_LS_SE1           0xc00

#endif
