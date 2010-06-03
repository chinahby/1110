#ifndef HAL_HSUSB
#define HAL_HSUSB
/*
===========================================================================

FILE:         HAL_hsusb.h

DESCRIPTION:  
This is the HAL external interface for the HS-USB core.

===========================================================================

===========================================================================
Copyright © 2008 QUALCOMM Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/*
* HAL_HSUSB_Regs
*/
/* Identification Registers */
#define HAL_HSUSB_ID_ADDR                           (0x00000000)
#define HAL_HSUSB_ID_REVISON_BMSK                       0xff0000
#define HAL_HSUSB_ID_REVISON_SHFT                           0x10
#define HAL_HSUSB_ID_NID_BMSK                             0x3f00
#define HAL_HSUSB_ID_NID_SHFT                                0x8
#define HAL_HSUSB_ID_ID_BMSK                                0x3f
#define HAL_HSUSB_ID_ID_SHFT                                   0

#define HAL_HSUSB_HWGENERAL_ADDR                     (0x00000004)
#define HAL_HSUSB_HWGENERAL_SM_BMSK                        0x200
#define HAL_HSUSB_HWGENERAL_SM_SHFT                          0x9
#define HAL_HSUSB_HWGENERAL_PHYM_BMSK                      0x1c0
#define HAL_HSUSB_HWGENERAL_PHYM_SHFT                        0x6
#define HAL_HSUSB_HWGENERAL_PHYW_BMSK                       0x30
#define HAL_HSUSB_HWGENERAL_PHYW_SHFT                        0x4
#define HAL_HSUSB_HWGENERAL_BWT_BMSK                         0x8
#define HAL_HSUSB_HWGENERAL_BWT_SHFT                         0x3
#define HAL_HSUSB_HWGENERAL_CLCK_BMSK                        0x6
#define HAL_HSUSB_HWGENERAL_CLCK_SHFT                        0x1
#define HAL_HSUSB_HWGENERAL_RT_BMSK                          0x1
#define HAL_HSUSB_HWGENERAL_RT_SHFT                            0

#define HAL_HSUSB_HWHOST_ADDR                       (0x00000008)
#define HAL_HSUSB_HWHOST_TTPER_BMSK                   0xff000000
#define HAL_HSUSB_HWHOST_TTPER_SHFT                         0x18
#define HAL_HSUSB_HWHOST_TTASY_BMSK                     0xff0000
#define HAL_HSUSB_HWHOST_TTASY_SHFT                         0x10
#define HAL_HSUSB_HWHOST_NPORT_BMSK                          0xe
#define HAL_HSUSB_HWHOST_NPORT_SHFT                          0x1
#define HAL_HSUSB_HWHOST_HC_BMSK                             0x1
#define HAL_HSUSB_HWHOST_HC_SHFT                               0

#define HAL_HSUSB_HWDEVICE_ADDR                     (0x0000000c)
#define HAL_HSUSB_HWDEVICE_DEVEP_BMSK                       0x3e
#define HAL_HSUSB_HWDEVICE_DEVEP_SHFT                        0x1
#define HAL_HSUSB_HWDEVICE_DC_BMSK                           0x1
#define HAL_HSUSB_HWDEVICE_DC_SHFT                             0

#define HAL_HSUSB_HWTXBUF_ADDR                      (0x00000010)
#define HAL_HSUSB_HWTXBUF_TXLCR_BMSK                  0x80000000
#define HAL_HSUSB_HWTXBUF_TXLCR_SHFT                        0x1f
#define HAL_HSUSB_HWTXBUF_TXCHANADD_BMSK                0xff0000
#define HAL_HSUSB_HWTXBUF_TXCHANADD_SHFT                    0x10
#define HAL_HSUSB_HWTXBUF_TXADD_BMSK                      0xff00
#define HAL_HSUSB_HWTXBUF_TXADD_SHFT                         0x8
#define HAL_HSUSB_HWTXBUF_TCBURST_BMSK                      0xff
#define HAL_HSUSB_HWTXBUF_TCBURST_SHFT                         0

#define HAL_HSUSB_HWRXBUF_ADDR                      (0x00000014)
#define HAL_HSUSB_HWRXBUF_RX_ADD_BMSK                     0xff00
#define HAL_HSUSB_HWRXBUF_RX_ADD_SHFT                        0x8
#define HAL_HSUSB_HWRXBUF_RX_BURST_BMSK                     0xff
#define HAL_HSUSB_HWRXBUF_RX_BURST_SHFT                        0

#define HAL_HSUSB_GPTIMER0LD_ADDR                   (0x00000080)
#define HAL_HSUSB_GPTIMER0LD_GPTLD_BMSK                 0xffffff
#define HAL_HSUSB_GPTIMER0LD_GPTLD_SHFT                        0

#define HAL_HSUSB_GPTIMER0CTRL_ADDR                 (0x00000084)
#define HAL_HSUSB_GPTIMER0CTRL_GTPRUN_BMSK            0x80000000
#define HAL_HSUSB_GPTIMER0CTRL_GTPRUN_SHFT                  0x1f
#define HAL_HSUSB_GPTIMER0CTRL_GPTRST_BMSK            0x40000000
#define HAL_HSUSB_GPTIMER0CTRL_GPTRST_SHFT                  0x1e
#define HAL_HSUSB_GPTIMER0CTRL_GPTMODE_BMSK            0x1000000
#define HAL_HSUSB_GPTIMER0CTRL_GPTMODE_SHFT                 0x18
#define HAL_HSUSB_GPTIMER0CTRL_GPTCNT_BMSK              0xffffff
#define HAL_HSUSB_GPTIMER0CTRL_GPTCNT_SHFT                     0

#define HAL_HSUSB_GPTIMER1LD_ADDR                   (0x00000088)
#define HAL_HSUSB_GPTIMER1LD_GPTLD_BMSK                 0xffffff
#define HAL_HSUSB_GPTIMER1LD_GPTLD_SHFT                        0

#define HAL_HSUSB_GPTIMER1CTRL_ADDR                 (0x0000008c)
#define HAL_HSUSB_GPTIMER1CTRL_GTPRUN_BMSK            0x80000000
#define HAL_HSUSB_GPTIMER1CTRL_GTPRUN_SHFT                  0x1f
#define HAL_HSUSB_GPTIMER1CTRL_GPTRST_BMSK            0x40000000
#define HAL_HSUSB_GPTIMER1CTRL_GPTRST_SHFT                  0x1e
#define HAL_HSUSB_GPTIMER1CTRL_GPTMODE_BMSK            0x1000000
#define HAL_HSUSB_GPTIMER1CTRL_GPTMODE_SHFT                 0x18
#define HAL_HSUSB_GPTIMER1CTRL_GPTCNT_BMSK              0xffffff
#define HAL_HSUSB_GPTIMER1CTRL_GPTCNT_SHFT                     0

/* Capability Registers */
#define HAL_HSUSB_CAPLENGTH_ADDR                    (0x00000100)
#define HAL_HSUSB_CAPLENGTH_CAPLENGTH_BMSK                  0xff
#define HAL_HSUSB_CAPLENGTH_CAPLENGTH_SHFT                     0

#define HAL_HSUSB_HCIVERSION_ADDR                   (0x00000102)
#define HAL_HSUSB_HCIVERSION_HCIVERSION_BMSK              0xffff
#define HAL_HSUSB_HCIVERSION_HCIVERSION_SHFT                   0

#define HAL_HSUSB_HCSPARAMS_ADDR                    (0x00000104)
#define HAL_HSUSB_HCSPARAMS_N_TT_BMSK                  0xf000000
#define HAL_HSUSB_HCSPARAMS_N_TT_SHFT                       0x18
#define HAL_HSUSB_HCSPARAMS_N_PTT_BMSK                  0xf00000
#define HAL_HSUSB_HCSPARAMS_N_PTT_SHFT                      0x14
#define HAL_HSUSB_HCSPARAMS_PI_BMSK                      0x10000
#define HAL_HSUSB_HCSPARAMS_PI_SHFT                         0x10
#define HAL_HSUSB_HCSPARAMS_N_CC_BMSK                     0xf000
#define HAL_HSUSB_HCSPARAMS_N_CC_SHFT                        0xc
#define HAL_HSUSB_HCSPARAMS_N_PCC_BMSK                     0xf00
#define HAL_HSUSB_HCSPARAMS_N_PCC_SHFT                       0x8
#define HAL_HSUSB_HCSPARAMS_PPC_BMSK                        0x10
#define HAL_HSUSB_HCSPARAMS_PPC_SHFT                         0x4
#define HAL_HSUSB_HCSPARAMS_N_PORTS_BMSK                     0xf
#define HAL_HSUSB_HCSPARAMS_N_PORTS_SHFT                       0

#define HAL_HSUSB_HCCPARAMS_ADDR                    (0x00000108)
#define HAL_HSUSB_HCCPARAMS_EECP_BMSK                     0xff00
#define HAL_HSUSB_HCCPARAMS_EECP_SHFT                        0x8
#define HAL_HSUSB_HCCPARAMS_IST_BMSK                        0xf0
#define HAL_HSUSB_HCCPARAMS_IST_SHFT                         0x4
#define HAL_HSUSB_HCCPARAMS_ASP_BMSK                         0x4
#define HAL_HSUSB_HCCPARAMS_ASP_SHFT                         0x2
#define HAL_HSUSB_HCCPARAMS_PFL_BMSK                         0x2
#define HAL_HSUSB_HCCPARAMS_PFL_SHFT                         0x1
#define HAL_HSUSB_HCCPARAMS_ADC_BMSK                         0x1
#define HAL_HSUSB_HCCPARAMS_ADC_SHFT                           0

#define HAL_HSUSB_DCIVERSION_ADDR                   (0x00000120)
#define HAL_HSUSB_DCIVERSION_DCIVERSION_BMSK              0xffff
#define HAL_HSUSB_DCIVERSION_DCIVERSION_SHFT                   0

#define HAL_HSUSB_DCCPARAMS_ADDR                    (0x00000124)
#define HAL_HSUSB_DCCPARAMS_HC_BMSK                        0x100
#define HAL_HSUSB_DCCPARAMS_HC_SHFT                          0x8
#define HAL_HSUSB_DCCPARAMS_DC_BMSK                         0x80
#define HAL_HSUSB_DCCPARAMS_DC_SHFT                          0x7
#define HAL_HSUSB_DCCPARAMS_DEN_BMSK                        0x1f
#define HAL_HSUSB_DCCPARAMS_DEN_SHFT                           0

/* Operational Registers */
#define HAL_HSUSB_USBCMD_ADDR                       (0x00000140)
#define HAL_HSUSB_USBCMD_RST_CTRL_BMSK                0x80000000
#define HAL_HSUSB_USBCMD_RST_CTRL_SHFT                      0x1f
#define HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK           0x40000000
#define HAL_HSUSB_USBCMD_ULPI_STP_CTRL_SHFT                 0x1e
#define HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK         0x20000000
#define HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_SHFT               0x1d
#define HAL_HSUSB_USBCMD_ITC_BMSK                       0xff0000
#define HAL_HSUSB_USBCMD_ITC_SHFT                           0x10
#define HAL_HSUSB_USBCMD_FS2_BMSK                         0x8000
#define HAL_HSUSB_USBCMD_FS2_SHFT                            0xf
#define HAL_HSUSB_USBCMD_SUTW_BMSK                        0x2000
#define HAL_HSUSB_USBCMD_SUTW_SHFT                           0xd
#define HAL_HSUSB_USBCMD_ATDTW_BMSK                       0x1000
#define HAL_HSUSB_USBCMD_ATDTW_SHFT                          0xc
#define HAL_HSUSB_USBCMD_ASPE_BMSK                         0x800
#define HAL_HSUSB_USBCMD_ASPE_SHFT                           0xb
#define HAL_HSUSB_USBCMD_ASP_BMSK                          0x300
#define HAL_HSUSB_USBCMD_ASP_SHFT                            0x8
#define HAL_HSUSB_USBCMD_LR_BMSK                            0x80
#define HAL_HSUSB_USBCMD_LR_SHFT                             0x7
#define HAL_HSUSB_USBCMD_IAA_BMSK                           0x40
#define HAL_HSUSB_USBCMD_IAA_SHFT                            0x6
#define HAL_HSUSB_USBCMD_ASE_BMSK                           0x20
#define HAL_HSUSB_USBCMD_ASE_SHFT                            0x5
#define HAL_HSUSB_USBCMD_PSE_BMSK                           0x10
#define HAL_HSUSB_USBCMD_PSE_SHFT                            0x4
#define HAL_HSUSB_USBCMD_FS1_BMSK                            0x8
#define HAL_HSUSB_USBCMD_FS1_SHFT                            0x3
#define HAL_HSUSB_USBCMD_FS0_BMSK                            0x4
#define HAL_HSUSB_USBCMD_FS0_SHFT                            0x2
#define HAL_HSUSB_USBCMD_RST_BMSK                            0x2
#define HAL_HSUSB_USBCMD_RST_SHFT                            0x1
#define HAL_HSUSB_USBCMD_RS_BMSK                             0x1
#define HAL_HSUSB_USBCMD_RS_SHFT                               0

#define HAL_HSUSB_USBSTS_ADDR                       (0x00000144)
#define HAL_HSUSB_USBSTS_ULPI_INTR_BMSK               0x80000000
#define HAL_HSUSB_USBSTS_ULPI_INTR_SHFT                     0x1f
#define HAL_HSUSB_USBSTS_TI1_BMSK                      0x2000000
#define HAL_HSUSB_USBSTS_TI1_SHFT                           0x19
#define HAL_HSUSB_USBSTS_TI0_BMSK                      0x1000000
#define HAL_HSUSB_USBSTS_TI0_SHFT                           0x18
#define HAL_HSUSB_USBSTS_UPI_BMSK                        0x80000
#define HAL_HSUSB_USBSTS_UPI_SHFT                           0x13
#define HAL_HSUSB_USBSTS_UAI_BMSK                        0x40000
#define HAL_HSUSB_USBSTS_UAI_SHFT                           0x12
#define HAL_HSUSB_USBSTS_NAKI_BMSK                       0x10000
#define HAL_HSUSB_USBSTS_NAKI_SHFT                          0x10
#define HAL_HSUSB_USBSTS_AS_BMSK                          0x8000
#define HAL_HSUSB_USBSTS_AS_SHFT                             0xf
#define HAL_HSUSB_USBSTS_PS_BMSK                          0x4000
#define HAL_HSUSB_USBSTS_PS_SHFT                             0xe
#define HAL_HSUSB_USBSTS_RCL_BMSK                         0x2000
#define HAL_HSUSB_USBSTS_RCL_SHFT                            0xd
#define HAL_HSUSB_USBSTS_HCH_BMSK                         0x1000
#define HAL_HSUSB_USBSTS_HCH_SHFT                            0xc
#define HAL_HSUSB_USBSTS_ULPII_BMSK                        0x400
#define HAL_HSUSB_USBSTS_ULPII_SHFT                          0xa
#define HAL_HSUSB_USBSTS_SLI_BMSK                          0x100
#define HAL_HSUSB_USBSTS_SLI_SHFT                            0x8
#define HAL_HSUSB_USBSTS_SRI_BMSK                           0x80
#define HAL_HSUSB_USBSTS_SRI_SHFT                            0x7
#define HAL_HSUSB_USBSTS_URI_BMSK                           0x40
#define HAL_HSUSB_USBSTS_URI_SHFT                            0x6
#define HAL_HSUSB_USBSTS_AAI_BMSK                           0x20
#define HAL_HSUSB_USBSTS_AAI_SHFT                            0x5
#define HAL_HSUSB_USBSTS_SEI_BMSK                           0x10
#define HAL_HSUSB_USBSTS_SEI_SHFT                            0x4
#define HAL_HSUSB_USBSTS_FRI_BMSK                            0x8
#define HAL_HSUSB_USBSTS_FRI_SHFT                            0x3
#define HAL_HSUSB_USBSTS_PCI_BMSK                            0x4
#define HAL_HSUSB_USBSTS_PCI_SHFT                            0x2
#define HAL_HSUSB_USBSTS_UEI_BMSK                            0x2
#define HAL_HSUSB_USBSTS_UEI_SHFT                            0x1
#define HAL_HSUSB_USBSTS_UI_BMSK                             0x1
#define HAL_HSUSB_USBSTS_UI_SHFT                               0

#define HAL_HSUSB_USBINTR_ADDR                      (0x00000148)
#define HAL_HSUSB_USBINTR_ULPI_INTR_EN_BMSK           0x80000000
#define HAL_HSUSB_USBINTR_ULPI_INTR_EN_SHFT                 0x1f
#define HAL_HSUSB_USBINTR_TIE1_BMSK                    0x2000000
#define HAL_HSUSB_USBINTR_TIE1_SHFT                         0x19
#define HAL_HSUSB_USBINTR_TIE0_BMSK                    0x1000000
#define HAL_HSUSB_USBINTR_TIE0_SHFT                         0x18
#define HAL_HSUSB_USBINTR_UPIE_BMSK                      0x80000
#define HAL_HSUSB_USBINTR_UPIE_SHFT                         0x13
#define HAL_HSUSB_USBINTR_UAIE_BMSK                      0x40000
#define HAL_HSUSB_USBINTR_UAIE_SHFT                         0x12
#define HAL_HSUSB_USBINTR_NAKE_BMSK                      0x10000
#define HAL_HSUSB_USBINTR_NAKE_SHFT                         0x10
#define HAL_HSUSB_USBINTR_ULPIE_BMSK                       0x400
#define HAL_HSUSB_USBINTR_ULPIE_SHFT                         0xa
#define HAL_HSUSB_USBINTR_SLE_BMSK                         0x100
#define HAL_HSUSB_USBINTR_SLE_SHFT                           0x8
#define HAL_HSUSB_USBINTR_SRE_BMSK                          0x80
#define HAL_HSUSB_USBINTR_SRE_SHFT                           0x7
#define HAL_HSUSB_USBINTR_URE_BMSK                          0x40
#define HAL_HSUSB_USBINTR_URE_SHFT                           0x6
#define HAL_HSUSB_USBINTR_AAE_BMSK                          0x20
#define HAL_HSUSB_USBINTR_AAE_SHFT                           0x5
#define HAL_HSUSB_USBINTR_SEE_BMSK                          0x10
#define HAL_HSUSB_USBINTR_SEE_SHFT                           0x4
#define HAL_HSUSB_USBINTR_FRE_BMSK                           0x8
#define HAL_HSUSB_USBINTR_FRE_SHFT                           0x3
#define HAL_HSUSB_USBINTR_PCE_BMSK                           0x4
#define HAL_HSUSB_USBINTR_PCE_SHFT                           0x2
#define HAL_HSUSB_USBINTR_UEE_BMSK                           0x2
#define HAL_HSUSB_USBINTR_UEE_SHFT                           0x1
#define HAL_HSUSB_USBINTR_UE_BMSK                            0x1
#define HAL_HSUSB_USBINTR_UE_SHFT                              0

#define HAL_HSUSB_FRINDEX_ADDR                      (0x0000014c)
#define HAL_HSUSB_FRINDEX_FRINDEX_BMSK                    0x3fff
#define HAL_HSUSB_FRINDEX_FRINDEX_SHFT                         0

#define HAL_HSUSB_PERIODICLISTBASE_ADDR             (0x00000154)
#define HAL_HSUSB_PERIODICLISTBASE_PERBASE_BMSK       0xfffff000
#define HAL_HSUSB_PERIODICLISTBASE_PERBASE_SHFT              0xc

#define HAL_HSUSB_DEVICEADDR_ADDR                   (0x00000154)
#define HAL_HSUSB_DEVICEADDR_USBADR_BMSK              0xfe000000
#define HAL_HSUSB_DEVICEADDR_USBADR_SHFT                    0x19
#define HAL_HSUSB_DEVICEADDR_USBADRA_BMSK              0x1000000
#define HAL_HSUSB_DEVICEADDR_USBADRA_SHFT                   0x18

#define HAL_HSUSB_ASYNCLISTADDR_ADDR                (0x00000158)
#define HAL_HSUSB_ASYNCLISTADDR_ASYBASE_BMSK          0xffffffe0
#define HAL_HSUSB_ASYNCLISTADDR_ASYBASE_SHFT                 0x5

#define HAL_HSUSB_ENDPOINTLISTADDR_ADDR             (0x00000158)
#define HAL_HSUSB_ENDPOINTLISTADDR_EPBASE_BMSK        0xfffff800
#define HAL_HSUSB_ENDPOINTLISTADDR_EPBASE_SHFT               0xb

#define HAL_HSUSB_TTCTRL_ADDR                       (0x0000015c)
#define HAL_HSUSB_TTCTRL_TTHA_BMSK                    0x7f000000
#define HAL_HSUSB_TTCTRL_TTHA_SHFT                          0x18

#define HAL_HSUSB_BURSTSIZE_ADDR                    (0x00000160)
#define HAL_HSUSB_BURSTSIZE_TXPBURST_BMSK                 0xff00
#define HAL_HSUSB_BURSTSIZE_TXPBURST_SHFT                    0x8
#define HAL_HSUSB_BURSTSIZE_RXPBURST_BMSK                   0xff
#define HAL_HSUSB_BURSTSIZE_RXPBURST_SHFT                      0

#define HAL_HSUSB_TXFILLTUNING_ADDR                 (0x00000164)
#define HAL_HSUSB_TXFILLTUNING_TXFIFOTHRES_BMSK         0x3f0000
#define HAL_HSUSB_TXFILLTUNING_TXFIFOTHRES_SHFT             0x10
#define HAL_HSUSB_TXFILLTUNING_TXSCHHEALTH_BMSK           0x1f00
#define HAL_HSUSB_TXFILLTUNING_TXSCHHEALTH_SHFT              0x8
#define HAL_HSUSB_TXFILLTUNING_TXSCHOH_BMSK                 0xff
#define HAL_HSUSB_TXFILLTUNING_TXSCHOH_SHFT                    0

#define HAL_HSUSB_ULPI_VIEWPORT_ADDR                (0x00000170)
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIWU_BMSK           0x80000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIWU_SHFT                 0x1f
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_BMSK          0x40000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRUN_SHFT                0x1e
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRW_BMSK           0x20000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIRW_SHFT                 0x1d
#define HAL_HSUSB_ULPI_VIEWPORT_ULPISS_BMSK            0x8000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPISS_SHFT                 0x1b
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIPORT_BMSK          0x7000000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIPORT_SHFT               0x18
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_BMSK           0xff0000
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIADDR_SHFT               0x10
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_BMSK            0xff00
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATRD_SHFT               0x8
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATWR_BMSK              0xff
#define HAL_HSUSB_ULPI_VIEWPORT_ULPIDATWR_SHFT                 0

#define HAL_HSUSB_ENDPTNAK_ADDR                     (0x00000178)
#define HAL_HSUSB_ENDPTNAK_EPTN_BMSK                  0xffff0000
#define HAL_HSUSB_ENDPTNAK_EPTN_SHFT                        0x10
#define HAL_HSUSB_ENDPTNAK_EPRN_BMSK                      0xffff
#define HAL_HSUSB_ENDPTNAK_EPRN_SHFT                           0

#define HAL_HSUSB_ENDPTNAKEN_ADDR                   (0x0000017c)
#define HAL_HSUSB_ENDPTNAKEN_EPTNE_BMSK               0xffff0000
#define HAL_HSUSB_ENDPTNAKEN_EPTNE_SHFT                     0x10
#define HAL_HSUSB_ENDPTNAKEN_EPRNE_BMSK                   0xffff
#define HAL_HSUSB_ENDPTNAKEN_EPRNE_SHFT                        0

#define HAL_HSUSB_PORTSC_ADDR(n)          (0x00000184 + 4 * (n))
#define HAL_HSUSB_PORTSC_PTS_BMSK                     0xc0000000
#define HAL_HSUSB_PORTSC_PTS_SERIAL_BMSK              0xc0000000
#define HAL_HSUSB_PORTSC_PTS_ULPI_BMSK                0x80000000
#define HAL_HSUSB_PORTSC_PTS_SHFT                           0x1e
#define HAL_HSUSB_PORTSC_STS_BMSK                     0x20000000
#define HAL_HSUSB_PORTSC_STS_SHFT                           0x1d
#define HAL_HSUSB_PORTSC_PTW_BMSK                     0x10000000
#define HAL_HSUSB_PORTSC_PTW_SHFT                           0x1c
#define HAL_HSUSB_PORTSC_PSPD_BMSK                     0xc000000
#define HAL_HSUSB_PORTSC_PSPD_SHFT                          0x1a
#define HAL_HSUSB_PORTSC_PFSC_BMSK                     0x1000000
#define HAL_HSUSB_PORTSC_PFSC_SHFT                          0x18
#define HAL_HSUSB_PORTSC_PHCD_BMSK                      0x800000
#define HAL_HSUSB_PORTSC_PHCD_SHFT                          0x17
#define HAL_HSUSB_PORTSC_WKOC_BMSK                      0x400000
#define HAL_HSUSB_PORTSC_WKOC_SHFT                          0x16
#define HAL_HSUSB_PORTSC_WKDS_BMSK                      0x200000
#define HAL_HSUSB_PORTSC_WKDS_SHFT                          0x15
#define HAL_HSUSB_PORTSC_WKCN_BMSK                      0x100000
#define HAL_HSUSB_PORTSC_WKCN_SHFT                          0x14
#define HAL_HSUSB_PORTSC_PTC_BMSK                        0xf0000
#define HAL_HSUSB_PORTSC_PTC_SHFT                           0x10
#define HAL_HSUSB_PORTSC_PIC_BMSK                         0xc000
#define HAL_HSUSB_PORTSC_PIC_SHFT                            0xe
#define HAL_HSUSB_PORTSC_PO_BMSK                          0x2000
#define HAL_HSUSB_PORTSC_PO_SHFT                             0xd
#define HAL_HSUSB_PORTSC_PP_BMSK                          0x1000
#define HAL_HSUSB_PORTSC_PP_SHFT                             0xc
#define HAL_HSUSB_PORTSC_LS_BMSK                           0xc00
#define HAL_HSUSB_PORTSC_LS_SHFT                             0xa
#define HAL_HSUSB_PORTSC_HSP_BMSK                          0x200
#define HAL_HSUSB_PORTSC_HSP_SHFT                            0x9
#define HAL_HSUSB_PORTSC_PR_BMSK                           0x100
#define HAL_HSUSB_PORTSC_PR_SHFT                             0x8
#define HAL_HSUSB_PORTSC_SUSP_BMSK                          0x80
#define HAL_HSUSB_PORTSC_SUSP_SHFT                           0x7
#define HAL_HSUSB_PORTSC_FPR_BMSK                           0x40
#define HAL_HSUSB_PORTSC_FPR_SHFT                            0x6
#define HAL_HSUSB_PORTSC_OCC_BMSK                           0x20
#define HAL_HSUSB_PORTSC_OCC_SHFT                            0x5
#define HAL_HSUSB_PORTSC_OCA_BMSK                           0x10
#define HAL_HSUSB_PORTSC_OCA_SHFT                            0x4
#define HAL_HSUSB_PORTSC_PEC_BMSK                            0x8
#define HAL_HSUSB_PORTSC_PEC_SHFT                            0x3
#define HAL_HSUSB_PORTSC_PE_BMSK                             0x4
#define HAL_HSUSB_PORTSC_PE_SHFT                             0x2
#define HAL_HSUSB_PORTSC_CSC_BMSK                            0x2
#define HAL_HSUSB_PORTSC_CSC_SHFT                            0x1
#define HAL_HSUSB_PORTSC_CCS_BMSK                            0x1
#define HAL_HSUSB_PORTSC_CCS_SHFT                              0

#define HAL_HSUSB_OTGSC_ADDR                        (0x000001a4)
#define HAL_HSUSB_OTGSC_DPIE_BMSK                     0x40000000
#define HAL_HSUSB_OTGSC_DPIE_SHFT                           0x1e
#define HAL_HSUSB_OTGSC_1MSE_BMSK                     0x20000000
#define HAL_HSUSB_OTGSC_1MSE_SHFT                           0x1d
#define HAL_HSUSB_OTGSC_BSEIE_BMSK                    0x10000000
#define HAL_HSUSB_OTGSC_BSEIE_SHFT                          0x1c
#define HAL_HSUSB_OTGSC_BSVIE_BMSK                     0x8000000
#define HAL_HSUSB_OTGSC_BSVIE_SHFT                          0x1b
#define HAL_HSUSB_OTGSC_ASVIE_BMSK                     0x4000000
#define HAL_HSUSB_OTGSC_ASVIE_SHFT                          0x1a
#define HAL_HSUSB_OTGSC_AVVIE_BMSK                     0x2000000
#define HAL_HSUSB_OTGSC_AVVIE_SHFT                          0x19
#define HAL_HSUSB_OTGSC_IDIE_BMSK                      0x1000000
#define HAL_HSUSB_OTGSC_IDIE_SHFT                           0x18
#define HAL_HSUSB_OTGSC_DPIS_BMSK                       0x400000
#define HAL_HSUSB_OTGSC_DPIS_SHFT                           0x16
#define HAL_HSUSB_OTGSC_1MSS_BMSK                       0x200000
#define HAL_HSUSB_OTGSC_1MSS_SHFT                           0x15
#define HAL_HSUSB_OTGSC_BSEIS_BMSK                      0x100000
#define HAL_HSUSB_OTGSC_BSEIS_SHFT                          0x14
#define HAL_HSUSB_OTGSC_BSVIS_BMSK                       0x80000
#define HAL_HSUSB_OTGSC_BSVIS_SHFT                          0x13
#define HAL_HSUSB_OTGSC_ASVIS_BMSK                       0x40000
#define HAL_HSUSB_OTGSC_ASVIS_SHFT                          0x12
#define HAL_HSUSB_OTGSC_AVVIS_BMSK                       0x20000
#define HAL_HSUSB_OTGSC_AVVIS_SHFT                          0x11
#define HAL_HSUSB_OTGSC_IDIS_BMSK                        0x10000
#define HAL_HSUSB_OTGSC_IDIS_SHFT                           0x10
#define HAL_HSUSB_OTGSC_DPS_BMSK                          0x4000
#define HAL_HSUSB_OTGSC_DPS_SHFT                             0xe
#define HAL_HSUSB_OTGSC_1MST_BMSK                         0x2000
#define HAL_HSUSB_OTGSC_1MST_SHFT                            0xd
#define HAL_HSUSB_OTGSC_BSE_BMSK                          0x1000
#define HAL_HSUSB_OTGSC_BSE_SHFT                             0xc
#define HAL_HSUSB_OTGSC_BSV_BMSK                           0x800
#define HAL_HSUSB_OTGSC_BSV_SHFT                             0xb
#define HAL_HSUSB_OTGSC_ASV_BMSK                           0x400
#define HAL_HSUSB_OTGSC_ASV_SHFT                             0xa
#define HAL_HSUSB_OTGSC_AVV_BMSK                           0x200
#define HAL_HSUSB_OTGSC_AVV_SHFT                             0x9
#define HAL_HSUSB_OTGSC_ID_BMSK                            0x100
#define HAL_HSUSB_OTGSC_ID_SHFT                              0x8
#define HAL_HSUSB_OTGSC_HABA_BMSK                           0x80
#define HAL_HSUSB_OTGSC_HABA_SHFT                            0x7
#define HAL_HSUSB_OTGSC_HADP_BMSK                           0x40
#define HAL_HSUSB_OTGSC_HADP_SHFT                            0x6
#define HAL_HSUSB_OTGSC_IDPU_BMSK                           0x20
#define HAL_HSUSB_OTGSC_IDPU_SHFT                            0x5
#define HAL_HSUSB_OTGSC_DP_BMSK                             0x10
#define HAL_HSUSB_OTGSC_DP_SHFT                              0x4
#define HAL_HSUSB_OTGSC_OT_BMSK                              0x8
#define HAL_HSUSB_OTGSC_OT_SHFT                              0x3
#define HAL_HSUSB_OTGSC_HAAR_BMSK                            0x4
#define HAL_HSUSB_OTGSC_HAAR_SHFT                            0x2
#define HAL_HSUSB_OTGSC_VC_BMSK                              0x2
#define HAL_HSUSB_OTGSC_VC_SHFT                              0x1
#define HAL_HSUSB_OTGSC_VD_BMSK                              0x1
#define HAL_HSUSB_OTGSC_VD_SHFT                                0

#define HAL_HSUSB_USBMODE_ADDR                      (0x000001a8)
#define HAL_HSUSB_USBMODE_VBPS_BMSK                         0x20
#define HAL_HSUSB_USBMODE_VBPS_SHFT                          0x5
#define HAL_HSUSB_USBMODE_SDIS_BMSK                         0x10
#define HAL_HSUSB_USBMODE_SDIS_SHFT                          0x4
#define HAL_HSUSB_USBMODE_SLOM_BMSK                          0x8
#define HAL_HSUSB_USBMODE_SLOM_SHFT                          0x3
#define HAL_HSUSB_USBMODE_ES_BMSK                            0x4
#define HAL_HSUSB_USBMODE_ES_SHFT                            0x2
#define HAL_HSUSB_USBMODE_CM_BMSK                            0x3
#define HAL_HSUSB_USBMODE_CM_HOST_BMSK                       0x3
#define HAL_HSUSB_USBMODE_CM_DEVICE_BMSK                     0x2
#define HAL_HSUSB_USBMODE_CM_IDLE_BMSK                       0x0
#define HAL_HSUSB_USBMODE_CM_SHFT                              0

#define HAL_HSUSB_ENPDTSETUPSTAT_ADDR               (0x000001ac)
#define HAL_HSUSB_ENPDTSETUPSTAT_ENDPTSETUPSTAT_BMSK      0xffff
#define HAL_HSUSB_ENPDTSETUPSTAT_ENDPTSETUPSTAT_SHFT           0

#define HAL_HSUSB_ENDPTPRIME_ADDR                   (0x000001b0)
#define HAL_HSUSB_ENDPTPRIME_PETB_BMSK                0xffff0000
#define HAL_HSUSB_ENDPTPRIME_PETB_SHFT                      0x10
#define HAL_HSUSB_ENDPTPRIME_PERB_BMSK                    0xffff
#define HAL_HSUSB_ENDPTPRIME_PERB_SHFT                         0

#define HAL_HSUSB_ENDPTFLUSH_ADDR                   (0x000001b4)
#define HAL_HSUSB_ENDPTFLUSH_FETB_BMSK                0xffff0000
#define HAL_HSUSB_ENDPTFLUSH_FETB_SHFT                      0x10
#define HAL_HSUSB_ENDPTFLUSH_FERB_BMSK                    0xffff
#define HAL_HSUSB_ENDPTFLUSH_FERB_SHFT                         0

#define HAL_HSUSB_ENDPTSTAT_ADDR                    (0x000001b8)
#define HAL_HSUSB_ENDPTSTAT_ETBR_BMSK                 0xffff0000
#define HAL_HSUSB_ENDPTSTAT_ETBR_SHFT                       0x10
#define HAL_HSUSB_ENDPTSTAT_ERBR_BMSK                     0xffff
#define HAL_HSUSB_ENDPTSTAT_ERBR_SHFT                          0

#define HAL_HSUSB_ENDPTCOMPLETE_ADDR                (0x000001bc)
#define HAL_HSUSB_ENDPTCOMPLETE_ETCE_BMSK             0xffff0000
#define HAL_HSUSB_ENDPTCOMPLETE_ETCE_SHFT                   0x10
#define HAL_HSUSB_ENDPTCOMPLETE_ERCE_BMSK                 0xffff
#define HAL_HSUSB_ENDPTCOMPLETE_ERCE_SHFT                      0

#define HAL_HSUSB_ENDPTCTRL0_ADDR                   (0x000001c0)
#define HAL_HSUSB_ENDPTCTRL0_TXE_BMSK                   0x800000
#define HAL_HSUSB_ENDPTCTRL0_TXE_SHFT                       0x17
#define HAL_HSUSB_ENDPTCTRL0_TXT_BMSK                    0xc0000
#define HAL_HSUSB_ENDPTCTRL0_TXT_SHFT                       0x12
#define HAL_HSUSB_ENDPTCTRL0_TXS_BMSK                    0x10000
#define HAL_HSUSB_ENDPTCTRL0_TXS_SHFT                       0x10
#define HAL_HSUSB_ENDPTCTRL0_RXE_BMSK                       0x80
#define HAL_HSUSB_ENDPTCTRL0_RXE_SHFT                        0x7
#define HAL_HSUSB_ENDPTCTRL0_RXT_BMSK                        0xc
#define HAL_HSUSB_ENDPTCTRL0_RXT_SHFT                        0x2
#define HAL_HSUSB_ENDPTCTRL0_RXS_BMSK                        0x1
#define HAL_HSUSB_ENDPTCTRL0_RXS_SHFT                          0

#define HAL_HSUSB_ENDPTCTRLn_ADDR(n)       (0x000001c0 + 4 * (n))
#define HAL_HSUSB_ENDPTCTRLn_TXE_BMSK                   0x800000
#define HAL_HSUSB_ENDPTCTRLn_TXE_SHFT                       0x17
#define HAL_HSUSB_ENDPTCTRLn_TXR_BMSK                   0x400000
#define HAL_HSUSB_ENDPTCTRLn_TXR_SHFT                       0x16
#define HAL_HSUSB_ENDPTCTRLn_TXI_BMSK                   0x200000
#define HAL_HSUSB_ENDPTCTRLn_TXI_SHFT                       0x15
#define HAL_HSUSB_ENDPTCTRLn_TXT_BMSK                    0xc0000
#define HAL_HSUSB_ENDPTCTRLn_TXT_SHFT                       0x12
#define HAL_HSUSB_ENDPTCTRLn_TXD_BMSK                    0x20000
#define HAL_HSUSB_ENDPTCTRLn_TXD_SHFT                       0x11
#define HAL_HSUSB_ENDPTCTRLn_TXS_BMSK                    0x10000
#define HAL_HSUSB_ENDPTCTRLn_TXS_SHFT                       0x10
#define HAL_HSUSB_ENDPTCTRLn_RXE_BMSK                       0x80
#define HAL_HSUSB_ENDPTCTRLn_RXE_SHFT                        0x7
#define HAL_HSUSB_ENDPTCTRLn_RXR_BMSK                       0x40
#define HAL_HSUSB_ENDPTCTRLn_RXR_SHFT                        0x6
#define HAL_HSUSB_ENDPTCTRLn_RXI_BMSK                       0x20
#define HAL_HSUSB_ENDPTCTRLn_RXI_SHFT                        0x5
#define HAL_HSUSB_ENDPTCTRLn_RXT_BMSK                        0xc
#define HAL_HSUSB_ENDPTCTRLn_RXT_SHFT                        0x2
#define HAL_HSUSB_ENDPTCTRLn_RXD_BMSK                        0x2
#define HAL_HSUSB_ENDPTCTRLn_RXD_SHFT                        0x1
#define HAL_HSUSB_ENDPTCTRLn_RXS_BMSK                        0x1
#define HAL_HSUSB_ENDPTCTRLn_RXS_SHFT                          0

/*
* HAL_HSUSB_ErrorType
*
* Enumeration of possible error types for a HS-USB core.
*
* HAL_HSUSB_Success:        There are no errors.
* HAL_HSUSB_ NotSupported:  The function is not supported by 
*                           the HS-USB core.
* HAL_HSUSB_WrongPort:      The port is out of scope reported by 
#                           the HAL_hsusb_SupportedPortsQty().
* HSUSB_WrongPhyInterface:  The PHY Interface is not supported by 
*                           the HS-USB core.
* HSUSB_TimeoutExpired:     The maximal timeout has been expired.
*/
typedef enum
{
  HAL_HSUSB_Success,
  HAL_HSUSB_NotSupported,
  HAL_HSUSB_WrongPort,
  HSUSB_WrongPhyInterface,
  HSUSB_TimeoutExpired
} HAL_HSUSB_ErrorType;

/*
* HAL_HSUSB_PhyInterfaceType
*
* Enumeration of possible PHY interface types for a HS-USB core.
*
* HAL_HSUSB_PHY_IF_ULPI:          The ULPI connection.
* HAL_HSUSB_PHY_IF_SERIAL:        The Serial connection.
* HAL_HSUSB_PHY_IF_FS_INTERCHIP:  The Full Speed Inter-chip connection (no PHY).
* HAL_HSUSB_PHY_IF_HS_INTERCHIP:  The High Speed Inter-chip connection (no PHY).
*/
typedef enum
{
  HAL_HSUSB_PHY_IF_ULPI,
  HAL_HSUSB_PHY_IF_SERIAL,
  HAL_HSUSB_PHY_IF_FS_INTERCHIP,
  HAL_HSUSB_PHY_IF_HS_INTERCHIP
} HAL_HSUSB_PhyInterfaceType;

/*
* HAL_HSUSB_PhyConnectionType
*
* Enumeration of possible PHY connection types for a HS-USB core.
*
* HAL_HSUSB_PHY_INTEGRATED:  The PHY is integrated.
* HAL_HSUSB_PHY_EXTERNAL:  The PHY is external.
*/
typedef enum
{
  HAL_HSUSB_PHY_INTEGRATED,
  HAL_HSUSB_PHY_EXTERNAL
} HAL_HSUSB_PhyConnectionType;

/*
* HAL_HSUSB_PhyType
*/
typedef struct
{
  HAL_HSUSB_PhyInterfaceType  PhyInterfaceType;
  HAL_HSUSB_PhyConnectionType PhyConnectionType;
} HAL_HSUSB_PhyType;

/*
* HAL_HSUSB_AhbBurstMode
*
* Enumeration of modes of the AMBA AHB master interface m_hburst signal.
*
*/
typedef enum
{
  HAL_HSUSB_AHB_BURST_INCR          = 0x0,
  HAL_HSUSB_AHB_BURST_INCR4_SINGLE  = 0x1,
  HAL_HSUSB_AHB_BURST_INCR8_SINGLE  = 0x2,
  HAL_HSUSB_AHB_BURST_INCR16_SINGLE = 0x3,
  HAL_HSUSB_AHB_BURST_INCR4_MULTI   = 0x5,
  HAL_HSUSB_AHB_BURST_INCR8_MULTI   = 0x6,
  HAL_HSUSB_AHB_BURST_INCR16_MULTI  = 0x7
} HAL_HSUSB_AhbBurstMode;

/*
* HAL_HSUSB_HprotMode
*
* Enumeration of the HPROT signal modes.
*
* HAL_HSUSB_HPROT_NONPOSTED:  The HPROT signal out of the XTOR is “0001”, 
*                             and all transactions are non-posted.
* HAL_HSUSB_HPROT_POSTED:     The HPROT signal out of the XTOR is “0101”, 
*                             and all transactions are posted.
* HAL_HSUSB_HPROT_ALT:        The HPROT signal out of the XTOR alters 
*                             between the above states according to 
*                             the context of the access. Control structures 
*                             are non-posted while data is posted.
*/
typedef enum
{
  HAL_HSUSB_HPROT_NONPOSTED = 0x0,
  HAL_HSUSB_HPROT_POSTED    = 0x1,
  HAL_HSUSB_HPROT_ALT       = 0x2
} HAL_HSUSB_HprotMode;

/*
* HAL_HSUSB_TestBusSelectorType
*
* Enumeration of possible PHY types for a HS-USB core.
*
* HAL_HSUSB_TEST_BUS_KSM:          Key state machine
* HAL_HSUSB_TEST_BUS_DMA_ENG_0:    dma_eng_0
* HAL_HSUSB_TEST_BUS_DMA_ENG_1:    dma_eng_1
* HAL_HSUSB_TEST_BUS_DMA_ENG_2:    dma_eng_2
* HAL_HSUSB_TEST_BUS_PROT_ENG_0:   prot_eng_0
* HAL_HSUSB_TEST_BUS_PROT_ENG_1:   prot_eng_1
* HAL_HSUSB_TEST_BUS_PROT_ENG_2:   prot_eng_2
* HAL_HSUSB_TEST_BUS_PORT_CTRL_0:  port_ctrl_0
* HAL_HSUSB_TEST_BUS_PORT_CTRL_1:  port_ctrl_1
* HAL_HSUSB_TEST_BUS_TX_BUFFER:    tx_buffer
* HAL_HSUSB_TEST_BUS_RX_BUFFER:    tx_buffer
* HAL_HSUSB_TEST_BUS_UTG:          utg
*/
typedef enum
{
  HAL_HSUSB_TEST_BUS_KSM         = 0x0,
  HAL_HSUSB_TEST_BUS_DMA_ENG_0   = 0x1,
  HAL_HSUSB_TEST_BUS_DMA_ENG_1   = 0x2,
  HAL_HSUSB_TEST_BUS_DMA_ENG_2   = 0x3,
  HAL_HSUSB_TEST_BUS_PROT_ENG_0  = 0x4,
  HAL_HSUSB_TEST_BUS_PROT_ENG_1  = 0x5,
  HAL_HSUSB_TEST_BUS_PROT_ENG_2  = 0x6,
  HAL_HSUSB_TEST_BUS_PORT_CTRL_0 = 0x7,
  HAL_HSUSB_TEST_BUS_PORT_CTRL_1 = 0x8,
  HAL_HSUSB_TEST_BUS_TX_BUFFER   = 0x9,
  HAL_HSUSB_TEST_BUS_RX_BUFFER   = 0xA,
  HAL_HSUSB_TEST_BUS_UTG         = 0xB
} HAL_HSUSB_TestBusSelectorType;

/* -----------------------------------------------------------------------
**                           INTERFACES
** ----------------------------------------------------------------------- */

/* 
* C++ wrapper
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Mandatory API */
void    HAL_hsusb_Init(void);
void    HAL_hsusb_Reset(void);
/* Special API */
uint8   HAL_hsusb_SupportedCoresQty(void);
uint16  HAL_hsusb_HWversion(uint8 instance);
uint8   HAL_hsusb_SupportedPortsQty(uint8 instance);
HAL_HSUSB_PhyType* HAL_hsusb_SupportedPhyType(uint8 instance, uint8 port);
HAL_HSUSB_ErrorType HAL_hsusb_GetLastError(uint8 instance);
HAL_HSUSB_ErrorType HAL_hsusb_ReadUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8* data);
HAL_HSUSB_ErrorType HAL_hsusb_WriteUlpiPhy(uint8 instance, uint8 port, uint8 reg, uint8 data);
uint8   HAL_hsusb_Read8(uint8 instance, uint32 offset);
uint16  HAL_hsusb_Read16(uint8 instance, uint32 offset);
uint32  HAL_hsusb_Read32(uint8 instance, uint32 offset);
void    HAL_hsusb_Write8(uint8 instance, uint32 offset, uint8 data);
void    HAL_hsusb_Write16(uint8 instance, uint32 offset, uint16 data);
void    HAL_hsusb_Write32(uint8 instance, uint32 offset, uint32 data);
uint32  HAL_hsusb_GetBits32(uint8 instance, uint32 offset, uint32 bits);
void    HAL_hsusb_SetBits32(uint8 instance, uint32 offset, uint32 bits);
void    HAL_hsusb_ClearBits32(uint8 instance, uint32 offset, uint32 bits);
void    HAL_hsusb_ClearAndSetBits32(uint8 instance, uint32 offset, uint32 clearBits, uint32 setBits);
HAL_HSUSB_AhbBurstMode HAL_hsusb_GetAhbBurstMode(uint8 instance);
void    HAL_hsusb_SetAhbBurstMode(uint8 instance, HAL_HSUSB_AhbBurstMode eMode);
boolean HAL_hsusb_GetSe0GlitchFixCtrl(uint8 instance);
void    HAL_hsusb_SetSe0GlitchFixCtrl(uint8 instance, boolean state);
boolean HAL_hsusb_GetFs3Wire2WireSelect(uint8 instance);
void    HAL_hsusb_SetFs3Wire2WireSelect(uint8 instance, boolean state);
boolean HAL_hsusb_GetXtorBypass(uint8 instance);
void    HAL_hsusb_SetXtorBypass(uint8 instance, boolean state);
boolean HAL_hsusb_GetPostedWrites(uint8 instance);
void    HAL_hsusb_SetPostedWrites(uint8 instance, boolean state);
boolean HAL_hsusb_GetGrantStolen(uint8 instance);
void    HAL_hsusb_SetGrantStolen(uint8 instance, boolean state);
HAL_HSUSB_HprotMode HAL_hsusb_GetHprotMode(uint8 instance);
void    HAL_hsusb_SetHprotMode(uint8 instance, HAL_HSUSB_HprotMode eMode);
boolean HAL_hsusb_GetIncrOverride(uint8 instance);
void    HAL_hsusb_SetIncrOverride(uint8 instance, boolean state);
boolean HAL_hsusb_GetAsyncBridgesBypass(uint8 instance);
void    HAL_hsusb_SetAsyncBridgesBypass(uint8 instance, boolean state);
HAL_HSUSB_TestBusSelectorType HAL_hsusb_GetTestBusSelector(uint8 instance);
void    HAL_hsusb_SetTestBusSelector(uint8 instance, HAL_HSUSB_TestBusSelectorType eMode);
boolean HAL_hsusb_GetTxBufCollisionMode(uint8 instance);
void    HAL_hsusb_SetTxBufCollisionMode(uint8 instance, boolean state);
boolean HAL_hsusb_GetStreamRxBypassMode(uint8 instance);
void    HAL_hsusb_SetStreamRxBypassMode(uint8 instance, boolean state);
boolean HAL_hsusb_GetStreamTxAutoMode(uint8 instance);
void    HAL_hsusb_SetStreamTxAutoMode(uint8 instance, boolean state);
boolean HAL_hsusb_GetHostSimTimersStdMode(uint8 instance);
void    HAL_hsusb_SetHostSimTimersStdMode(uint8 instance, boolean state);
boolean HAL_hsusb_GetHostSimTimersSuspMode(uint8 instance);
void    HAL_hsusb_SetHostSimTimersSuspMode(uint8 instance, boolean state);

#ifdef __cplusplus
}
#endif

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================
**
**  HAL_hsusb_Init
**
**  Description:
**    This function initializes the HS-USB core Hardware prior 
**    to the first usage after power-up or a call to HAL_hsusb_Reset().
**    
**  Dependencies:
**    None.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_Reset
**
**  Description:
**    This function is intended to return the HS-USB hardware to its 
**    power-on status.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_SupportedCoresQty
**
**  Description:
**    This function reports the HS-USB cores’ amount.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    None.
**
**  Return:
**     The number of the HS-USB cores attached to the target.
**
** ========================================================================
**
**  HAL_hsusb_HWversion
**
**  Description:
**    This function reports the HW version for a given HS-USB core.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The HW version for a given HS-USB core.
**
** ========================================================================
**
**  HAL_hsusb_SupportedPortsQty
**
**  Description:
**    This function reports the HS-USB core ports' amount.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The number of the ports attached to a HS-USB core.
**
** ========================================================================
**
**  HAL_hsusb_SupportedPhyType
**
**  Description:
**    This function reports which PHY is supported by a HS-USB core port.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**
**  Return:
**    The PHY supported by a HS-USB core port.
**
** ========================================================================
**
**  HAL_hsusb_GetLastError
**
**  Description:
**    This function gets the last error code of the HS-USB core recorded
**    from the HS-USB HAL failure. It is intended for debug purposes.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The last error code of the HS-USB core recorded from the HS-USB HAL failure.
**
** ========================================================================
**
**  HAL_hsusb_ReadUlpiPhy
**
**  Description:
**    This function reads from a ULPI PHY register of the HS-USB core port.
**    This function is valid for HS-USB HW version number 1 and above.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Returns the read data.
**
**  Return:
**    HAL_HSUSB_Success           - the returned <data> value is valid.
**    HAL_HSUSB_WrongPort         – the <port> is out of scope reported 
**                                  by the HAL_hsusb_SupportedPortsQty().
**    HAL_HSUSB_WrongPhyInterface – the Phy Interface is not ULPI.
**    HAL_HSUSB_TimeoutExpired    – the maximal timeout has been expired.
**
** ========================================================================
**
**  HAL_hsusb_WriteUlpiPhy
**
**  Description:
**    This function writes to a ULPI PHY register of the HS-USB core port.
**    This function is valid for HS-USB HW version number 1 and above.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    port     - Determines which HS-USB core port to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedPortsQty().
**    reg      - Determines the register address. This value must be 
**             in the address ranges defined by the HW specification. 
**    data     - Determines the write data.
**
**  Return:
**    HAL_HSUSB_Success           - the requested <data> is set.
**    HAL_HSUSB_WrongPort         – the <port> is out of scope reported 
**                                  by the HAL_hsusb_SupportedPortsQty().
**    HAL_HSUSB_WrongPhyInterface – the Phy Interface is not ULPI.
**    HAL_HSUSB_TimeoutExpired    – the maximal timeout has been expired.
**
** ========================================================================
**
**  HAL_hsusb_Read8
**
**  Description:
**    This function reads 8 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ========================================================================
**
**  HAL_hsusb_Read16
**
**  Description:
**    This function reads 16 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ========================================================================
**
**  HAL_hsusb_Read32
**
**  Description:
**    This function reads 32 bits from the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**
**  Return:
**    The read data.
**
** ========================================================================
**
**  HAL_hsusb_Write8
**
**  Description:
**    This function writes 8 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_Write16
**
**  Description:
**    This function writes 16 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_Write32
**
**  Description:
**    This function writes 32 bits to the HS-USB core register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    data     - Determines the write data.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetBits32
**
**  Description:
**    This function gets bits f the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be get.
**
**  Return:
**    The read data masked with <bits>.
**
** ========================================================================
**
**  HAL_hsusb_SetBits32
**
**  Description:
**    This function sets bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be set.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_ClearBits32
**
**  Description:
**    This function clears bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    offset   - Determines the register offset. 
**             This value must be aligned at the 32 bit boundary and 
**             in the address ranges defined by the HW specification.
**    bits     - Determines the bits to be cleared.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_ClearAndSetBits32
**
**  Description:
**    This function clears and then sets bits in the HS-USB core 32-bit register.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**
**  Parameters:
**    instance  - Determines which HS-USB core to use. 
**              This value should be one less than reported by 
**              the HAL_hsusb_SupportedCoresQty().
**    offset    - Determines the register offset. 
**              This value must be aligned at the 32 bit boundary and 
**              in the address ranges defined by the HW specification.
**    clearBits - Determines the bits to be cleared.
**    setBits   - Determines the bits to be set.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetAhbBurstMode
**
**  Description:
**    This function reports the mode of the AMBA AHB master interface 
**    m_hburst signal.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**     The mode of the AMBA AHB master interface m_hburst signal.
**
** ========================================================================
**
**  HAL_hsusb_SetAhbBurstMode
**
**  Description:
**    This function sets the mode of the AMBA AHB master interface 
**    m_hburst signal.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines mode of the AMBA AHB master interface 
**             m_hburst signal.
**
**  Return:
**    None.
**
** ========================================================================
**  HAL_hsusb_GetSe0GlitchFixCtrl
**
**  Description:
**    This function reports whether the SE0 glitch fix mechanism is activated.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The SE0 glitch fix mechanism is activated.
**    FALSE   - The SE0 glitch fix mechanism is not activated.
**
** ========================================================================
**
**  HAL_hsusb_SetSe0GlitchFixCtrl
**
**  Description:
**    This function activates/disactivate the SE0 glitch fix mechanism.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Activate the SE0 glitch fix mechanism
**             FALSE - Deactivate the SE0 glitch fix mechanism
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetFs3Wire2WireSelect
**
**  Description:
**    This function reports the two wire interface on the fs_dat and 
**    fs_se0 pins mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The two wire interface on the fs_dat and 
**              fs_se0 pins is enabled.
**    FALSE   - The two wire interface on the fs_dat and 
**              fs_se0 pins is disabled.
**
** ========================================================================
**
**  HAL_hsusb_SetFs3Wire2WireSelect
**
**  Description:
**    This function enables/disables the two wire interface on the fs_dat and 
**    fs_se0 pins.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the requested action.
**             Values are:
**             TRUE  - Enable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**             FALSE - Disable the two wire interface on the fs_dat and 
**                     fs_se0 pins
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetXtorBypass
**
**  Description:
**    This function reports whether the AHB Transactor is bypassed.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The AHB Transactor is bypassed.
**    FALSE   - The AHB Transactor is not bypassed.
**
** ========================================================================
**
**  HAL_hsusb_SetXtorBypass
**
**  Description:
**    This function sets the AHB Transactor bypass mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the AHB Transactor bypass mode.
**             Values are:
**             TRUE  - Bypass the AHB Transactor
**             FALSE - Do not bypass the AHB Transactor
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetPostedWrites
**
**  Description:
**    This function reports the USB Core AHB posted data writes mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The AHB master interface enables posted data writes
**    FALSE - The AHB master interface disables posted data writes
**
** ========================================================================
**
**  HAL_hsusb_SetPostedWrites
**
**  Description:
**    This function sets the USB Core AHB posted data writes mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 only.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core AHB posted data writes mode.
**             Values are:
**             TRUE  - The AHB master interface enables posted data writes
**             FALSE - The AHB master interface disables posted data writes
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetGrantStolen
**
**  Description:
**    This function reports whether the arbiter removed the hgrant signal 
**    prior to completing a transaction.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The arbiter removed the hgrant signal 
**              prior to completing a transaction.
**    FALSE   - The arbiter doesn’t removed the hgrant signal 
**              prior to completing a transaction.
**
** ========================================================================
**
**  HAL_hsusb_SetGrantStolen
**
**  Description:
**    This function sets the arbiter removing the hgrant signal 
**    prior to completing a transaction.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the arbiter removing the hgrant signal 
**             prior to completing a transaction mode.
**             Values are:
**             TRUE  - Enable the arbiter removed the hgrant signal 
**                   prior to completing a transaction
**             FALSE - Disable the arbiter doesn’t removed the hgrant signal 
**                   prior to completing a transaction
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetHprotMode
**
**  Description:
**    This function reports the HPROT signal mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The HPROT signal mode.
**
** ========================================================================
**
**  HAL_hsusb_SetHprotMode
**
**  Description:
**    This function sets the HPROT signal mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines the HPROT signal mode.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetIncrOverride
**
**  Description:
**    This function reports the USB Core INCR bursts transformation.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE  - The all INCR bursts from the USB Core will be 
**            internally transformed into SINGLE transfers
**    FALSE - If the USB Core issues an INCR burst, it will 
**            propagate to the external master AHB port
**
** ========================================================================
**
**  HAL_hsusb_SetIncrOverride
**
**  Description:
**    This function sets the USB Core INCR bursts transformation.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the USB Core INCR bursts transformation.
**             Values are:
**             TRUE  - The all INCR bursts from the USB Core will be 
**                   internally transformed into SINGLE transfers
**             FALSE - If the USB Core issues an INCR burst, it will 
**                   propagate to the external master AHB port
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetAsyncBridgesBypass
**
**  Description:
**    This function reports the asynchronous bridge bypass mode on 
**    the master AHB interface.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The asynchronous bridge on the master AHB interface is bypassed.
**    FALSE   - The asynchronous bridge on the master AHB interface issued.
**
** ========================================================================
**
**  HAL_hsusb_SetAsyncBridgesBypass
**
**  Description:
**    This function sets the asynchronous bridge bypass mode on 
**    the master AHB interface.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the asynchronous bridge bypass mode on 
**             the master AHB interface.
**             Values are:
**             TRUE  - The asynchronous bridge on the master AHB interface 
**                   is bypassed
**             FALSE - The asynchronous bridge on the master AHB interface 
**                   issued
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetTestBusSelector
**
**  Description:
**    This function reports which group of the internal signals is 
**    routed to the test bus.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    The group of the internal signals that is routed to the test bus.
**
** ========================================================================
**
**  HAL_hsusb_SetTestBusSelector
**
**  Description:
**    This function sets which group of the internal signals is 
**    routed to the test bus.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    eMode    - Determines which group of the internal signals is routed 
**             to the test bus.
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetTxBufCollisionMode
**
**  Description:
**    This function reports the TX buffer controller mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX buffer controller direct mode.
**    FALSE   - The TX buffer controller revert mode.
**
** ========================================================================
**
**  HAL_hsusb_SetTxBufCollisionMode
**
**  Description:
**    This function sets the TX buffer controller mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 4 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX buffer controller mode.
**             Values are:
**             TRUE  - The TX buffer controller direct mode
**             FALSE - The TX buffer controller revert mode
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetStreamRxBypassMode
**
**  Description:
**    This function reports the RX streaming bypass mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The RX streaming bypass is enabled.
**    FALSE   - The RX streaming bypass is disabled.
**
** ========================================================================
**
**  HAL_hsusb_SetStreamRxBypassMode
**
**  Description:
**    This function sets the RX streaming bypass mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the RX streaming bypass mode.
**             Values are:
**             TRUE  - Enable the RX streaming bypassing
**             FALSE - Disable the RX streaming bypassing
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetStreamTxAutoMode
**
**  Description:
**    This function reports the TX streaming automatic determination mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The TX streaming automatic determination is enabled.
**    FALSE   - The TX streaming automatic determination s is disabled
**
** ========================================================================
**
**  HAL_hsusb_SetStreamTxAutoMode
**
**  Description:
**    This function sets the TX streaming automatic determination mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 5 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the TX streaming automatic determination mode.
**             Values are:
**             TRUE  - Enable TX streaming automatic determination
**             FALSE - Disable TX streaming automatic determination
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetHostSimTimersStdMode
**
**  Description:
**    This function reports the timers used for the USB reset on the ULPI mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB reset on the ULPI are enabled.
**    FALSE   - The timers used for the USB reset on the ULPI are disabled.
**
** ========================================================================
**
**  HAL_hsusb_SetHostSimTimersStdMode
**
**  Description:
**    This function sets the timers used for the USB reset on the ULPI mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 1 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB reset on the ULPI mode.
**             Values are:
**             TRUE  - Enable the timers used for the USB reset on the ULPI
**             FALSE - Disable the timers used for the USB reset on the ULPI
**
**  Return:
**    None.
**
** ========================================================================
**
**  HAL_hsusb_GetHostSimTimersSuspMode
**
**  Description:
**    This function reports the timers used for the USB suspend process mode.
**      
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**
**  Return:
**    TRUE    - The timers used for the USB suspend process short 
**              for faster simulation and ATE time.
**    FALSE   - The timers used for the USB suspend process are 
**              according to the USB specification.
**
** ========================================================================
**
**  HAL_hsusb_SetHostSimTimersSuspMode
**
**  Description:
**    This function sets the timers used for the USB suspend process mode.
**    
**  Dependencies:
**    HAL_hsusb_Init() must have been called prior to this function.
**    This function is valid for HS-USB HW version number 2 and above.
**
**  Parameters:
**    instance - Determines which HS-USB core to use. 
**             This value should be one less than reported by 
**             the HAL_hsusb_SupportedCoresQty().
**    state    - Determines the timers used for the USB suspend process mode.
**             Values are:
**             TRUE  - The timers used for the USB suspend process short 
**                     for faster simulation and ATE time
**             FALSE - The timers used for the USB suspend process are 
**                     according to the USB specification
**
**  Return:
**    None.
**
** ========================================================================
*/

#endif /* HAL_HSUSB */
