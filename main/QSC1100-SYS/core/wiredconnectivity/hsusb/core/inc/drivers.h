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
#ifndef _DRIVERS_H_
#define _DRIVERS_H_

#include <jos.h>

/* Host Controller drivers */
jresult_t ehci_pci_init(void);
jresult_t ehci_local_init(void);
jresult_t ohci_pci_init(void);
jresult_t ohci_local_init(void);
jresult_t uhci_pci_init(void);
jresult_t ahci_local_init(void);
jresult_t isp176x_init(void);
jresult_t isp116x_pci_init(void);
jresult_t isp116x_local_init(void);
jresult_t isp1362_init(void);
jresult_t td243hc_init(void);
jresult_t td243hc_rev2_init(void);

/* Device Controller drivers */
jresult_t netchip_init(void);
jresult_t atmel_init(void);
jresult_t tdi_4x_init(void);
jresult_t isp_1582_init(void);
jresult_t omapv1030_init(void);
jresult_t td243fc_init(void);
jresult_t td243fc_rev2_init(void);
jresult_t dcd_name_init(void);

/* OTG Controllers drivers */
jresult_t tdi_4x_otg_init(void);

/* Transciever Controller Drivers */
jresult_t pmic_init(void);

/* Usb drivers */
jresult_t uhub_init(void);
jresult_t ucompdev_init(void);
jresult_t ugen_init(void);
jresult_t umass_init(void);
jresult_t jkbd_init(void);
jresult_t jhid_general_init(void);
jresult_t jcdc_ecm_init(void);
jresult_t jserial_init(void);
jresult_t jloopback_init(void);
jresult_t jprinter_init(void);
jresult_t jiccd_init(void);
jresult_t jcdc_eem_init(void);
jresult_t jaudio_init(void);

/* Embedded HS host electrical test driver */
jresult_t jetest_init(void);

jresult_t utest_init(void);

driver_init_t driver_load_list[] = {
#ifndef CONFIG_TEST_HCI
#ifdef CONFIG_OHCI
# ifdef CONFIG_PCI
    ohci_pci_init,
# else
    ohci_local_init,
# endif
#endif
    
#ifdef CONFIG_UHCI
    uhci_pci_init,
#endif

#ifdef CONFIG_EHCI
# ifdef CONFIG_PCI
    ehci_pci_init,
# else
    ehci_local_init,
# endif
#endif

#ifdef CONFIG_AHCI
    ahci_local_init,
#endif

#ifdef CONFIG_ISP176x
    isp176x_init,
#endif

#ifdef CONFIG_ISP116X
# ifdef CONFIG_PCI
    isp116x_pci_init,
# else
    isp116x_local_init,
# endif
#endif

#ifdef CONFIG_ISP1362
    isp1362_init,
#endif

#ifdef CONFIG_TD243HC
    td243hc_init,
#endif
    
#ifdef CONFIG_TD243HC_REV2
    td243hc_rev2_init,
#endif
#endif
    
#ifdef CONFIG_JSLAVE

    #ifdef CONFIG_NETCHIP
        netchip_init,
    #endif

    #ifdef CONFIG_AT91RM9200
        atmel_init,
    #endif

    #ifdef CONFIG_TDI_4X
        tdi_4x_init,
    #endif
    #ifdef CONFIG_ISP_1582
        isp_1582_init,
    #endif
    #ifdef CONFIG_OMAPV1030
        omapv1030_init,
    #endif
    #ifdef CONFIG_TD243FC
        td243fc_init,
    #endif
    #ifdef CONFIG_TD243FC_REV2
        td243fc_rev2_init,
    #endif
    #ifdef CONFIG_DCD_TEMPLATE
        dcd_name_init,
    #endif

#endif

#ifdef CONFIG_JOTG
    #ifdef CONFIG_TDI_4X
        tdi_4x_otg_init,
    #endif
#endif
    
#if defined CONFIG_JHOST
    uhub_init,
    ucompdev_init,
#endif
    
#if defined CONFIG_UGEN
    ugen_init,
#endif

#if defined CONFIG_JUMASS
    umass_init,
#endif

#if defined CONFIG_JHOST_HID_KBD
    jkbd_init,
#endif

#if defined CONFIG_JHOST_HID_GENERAL
    jhid_general_init,
#endif
    
#if defined CONFIG_JHOST_CDC_ECM
    jcdc_ecm_init,
#endif
    
#if defined CONFIG_JHOST_SERIAL
    jserial_init,
#endif

#if defined CONFIG_JHOST_LOOPBACK
    jloopback_init,
#endif
#if defined CONFIG_TEST_CORE
    utest_init,
#endif

#if defined CONFIG_EM_FS
    jfs_initialize,
#endif

#if defined CONFIG_JHOST_PRINTER
    jprinter_init,
#endif

#if defined CONFIG_JTRANSCEIVER

    #ifdef CONFIG_PMIC
        pmic_init,
    #endif

#endif

#if defined CONFIG_JHOST_ETEST
    jetest_init,
#endif

#if defined CONFIG_JHOST_ICCD
    jiccd_init,
#endif

#if defined CONFIG_JHOST_CDC_EEM
    jcdc_eem_init,
#endif

#if defined CONFIG_JHOST_AUDIO
    jaudio_init,
#endif
    
    NULL
};

/* Misc (non class/controller) drivers */
#ifdef CONFIG_PKG_XML
# include <xml.h>
#endif

#ifdef CONFIG_TEST
# include <test.h>
#endif

#ifdef CONFIG_JHOST_ICCD_APP
jresult_t iccd_app_init(void);
void iccd_app_uninit(void);
#endif
#ifdef CONFIG_JHOST_CDC_EEM_APP
jresult_t cdc_eem_init(void);
void cdc_eem_uninit(void);
#endif

jresult_t mass_empty_init(void);
void mass_empty_uninit(void);

int mass_test_init(void);
void mass_test_uninit(void);

jresult_t audio_sample_init(void);
void audio_sample_uninit(void);

jresult_t audio_loopback_init(void);
void audio_loopback_uninit(void);

load_arr_t non_usb_driver_list[] = {
#ifdef CONFIG_PKG_XML
    { xml_init, xml_uninit },
#endif

#ifdef CONFIG_MASS_EMPTY_APP    
    { mass_empty_init, mass_empty_uninit },
#endif
    
#ifdef CONFIG_MASS_TEST_APP    
    { mass_test_init, mass_test_uninit },
#endif

#ifdef CONFIG_TEST
    { test_init, test_uninit },
#endif

#ifdef CONFIG_JHOST_ICCD_APP
    { iccd_app_init, iccd_app_uninit },
#endif
#ifdef CONFIG_JHOST_CDC_EEM_APP
    { cdc_eem_init, cdc_eem_uninit },
#endif

#ifdef CONFIG_JHOST_AUDIO_APP
    { audio_sample_init, audio_sample_uninit },
#endif

#ifdef CONFIG_JHOST_AUDIO_LOOPBACK
    { audio_loopback_init, audio_loopback_uninit },
#endif

    { NULL }
};
    
#endif

