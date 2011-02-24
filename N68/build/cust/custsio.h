#ifndef CUSTSIO_H
#define CUSTSIO_H
/*===========================================================================

DESCRIPTION
  Configuration for SIO

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/build/cust/custsio.h#11 $ $DateTime: 2009/04/02 17:24:01 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/07   mp      Removing L4/DM
05/17/07   gdp     Removed Redundant definition of FEATURE_USB_BREW_EXTENDED_DEVICE
05/16/07   gdp     Added Features for HS USB
04/05/07   mh      Add FEATURE_USB_DIAG_NMEA
02/06/07   mh      Remove FEATURE_USB_READ_XCVR_SELECT_SWITCH
02/24/06   ap      Add F_USB_HOST enhanced and mass storage.
09/24/03   ht      Removed FEATURE_USB_MICREL
09/23/03   ht      Update USB features for MSM6500 USB integration and OTG
07/28/02   jct     Added top level build time control of FEATURE_USB
05/01/02   jct     Created
===========================================================================*/

#ifdef T_FFA6550
#error code not present
#endif

/* set USB as default DIAG port */
#define RDM_USB_SER1_INIT_SRVC              RDM_DIAG_SRVC

/* Enable USB support
*/
#ifdef FEATURE_BUILD_USB
   #define FEATURE_USB
   #define FEATURE_USB_BREW_EXTENDED_DEVICE
   #ifdef FEATURE_BUILD_USB_MASS_STORAGE
       #define FEATURE_USB_PERIPHERAL_MASS_STORAGE
   #endif
   #define FEATURE_USB_DIAG_NMEA
#endif

/* UART power-down feature.
*/
#define FEATURE_UART_POWERDOWN
/* Enable UART clock to be TCXO not TCXO/4 */
#define FEATURE_UART_TCXO_CLK_FREQ
#define FEATURE_NO_SIO_USB_DEV_INIT

/* Enable the MDD/PDD UART driver architecture */
#define SIORS_USES_MDDPDD_DRIVER
#define SIORS_USES_EXTERNAL_MND_CNT
#define SIORS_USES_SC2X_CLKRGM
#define FEATURE_USE_UART1DM
#define FEATURE_FIRST_UART
#define SIORS_HAVE_TXRDY_INTR

#define SIORS_FIRST_UART_TYPE               HW_UARTDM_LT
#define SIORS_FIRST_UART_PD_WAKE_ON_RX      FALSE
#define SIORS_FIRST_UART_PD_CABLE_DETECT    FALSE
#define SIORS_FIRST_UART_UIM_PWR_VOTE       FALSE
#define SIORS_FIRST_UART_NEGATE_ARM_HALT    TRUE
#define SIORS_FIRST_UART_FIFO_SIZE          512

#define SIORS_SECOND_UART_TYPE              HW_NONE
#define SIORS_SECOND_UART_PD_WAKE_ON_RX     FALSE
#define SIORS_SECOND_UART_PD_CABLE_DETECT   FALSE
#define SIORS_SECOND_UART_UIM_PWR_VOTE      FALSE
#define SIORS_SECOND_UART_NEGATE_ARM_HALT   FALSE

#define SIORS_THIRD_UART_TYPE               HW_NONE
#define SIORS_THIRD_UART_PD_WAKE_ON_RX      FALSE
#define SIORS_THIRD_UART_PD_CABLE_DETECT    FALSE
#define SIORS_THIRD_UART_UIM_PWR_VOTE       FALSE
#define SIORS_THIRD_UART_NEGATE_ARM_HALT    FALSE

#define SIORS_TRAMP_VERSION                 TRAMP_VERSION_1100

#define SIO_REGISTER_DEVICE                 0

/* Enable new SIO code - required for MSM3100 s/w release
*/
#define FEATURE_NEW_SIO

#ifdef FEATURE_NEW_SIO

/*
** Inverts the polarity of DTR line.
** Fixes uart disconnect problem on cougar.
*/
#define FEATURE_SIO_DTR_ACTIVE_LOW

/* Enables second UART.  Must define
** FEATURE_NEW_SIO for this to be effective.
*/


/* Enables Serial Device Mapper.  Must define
** FEATURE_NEW_SIO for this to be effective.
*/
  #define FEATURE_SERIAL_DEVICE_MAPPER

#ifdef FEATURE_SERIAL_DEVICE_MAPPER
  /* Runtime Device Mapper services.  These are mutually exclusive
  ** with FEATURE_SDEVMAP_UI_MENU & FEATURE_SDEVMAP_MENU_ITEM_NV.
  */
  #define FEATURE_RUNTIME_DEVMAP

  /* Enables UI selection of port mapping. The port map is stored in
  ** NV and retrieved - requires power cycle for changes to take effect.
  ** These should be defined or undefined together.
  */
  #ifndef FEATURE_RUNTIME_DEVMAP
    #define FEATURE_SDEVMAP_UI_MENU
    #define FEATURE_SDEVMAP_MENU_ITEM_NV
  #endif

  #ifdef FEATURE_DS
    /* Enables UI selection of DS baud rate and the DS baud to be stored in
    ** and retrieved from NV.  These should be defined or undefined together.
    */
    #define  FEATURE_DS_UI_BAUD
    #define  FEATURE_DS_DEFAULT_BITRATE_NV
  #endif /* FEATURE_DS */

   /* Enables use of DSM Items for DIAG
   */
   #define FEATURE_DSM_DIAG_ITEMS

   /* Enables UI selection of DIAG baud rate and the DIAG baud to be stored in
   ** and retrieved from NV.  These should be defined or undefined together.
   */
   #define FEATURE_DIAG_UI_BAUD
   #define FEATURE_DIAG_DEFAULT_BITRATE_NV

#endif  /* FEATURE_SERIAL_DEVICE_MAPPER */

#ifdef FEATURE_USB

  /* Enables USB CDC ACM Port.  Must define
  ** FEATURE_NEW_SIO for this to be effective.
  */
  #define FEATURE_USB_CDC_ACM

  /* Enable diag over USB
  */
  #define FEATURE_USB_DIAG

  #ifdef FEATURE_USB_OTG

    #define FEATURE_USB_OTG_XCVR

  #else

    #define FEATURE_USB_FIVE_WIRE_XCVR

  #endif /* FEATURE_USB_OTG */


  #ifdef FEATURE_RUNTIME_DEVMAP
    /* Map DS to USB as the default configuration
    */
    #define FEATURE_RDEVMAP_DS_DEFAULT_TO_USB

    /* When defined, allows DS to be mappable to UART2.  However, since
    ** UART2 does not have a full set of RS232 signals, not all DS services
    ** are possible on UART2.
    */
    #undef FEATURE_DS_MAPPABLE_TO_UART2

  #endif

#endif  /* FEATURE_USB */

#endif  /* FEATURE_NEW_SIO */


/* Enables a nested GPIO interrupt to be used for DTR.  This is required for
   MSM5105 since all GPIO interrupts are nested
*/
#define FEATURE_SIO_DTR_HIGH_GPIO

#define RDM_UART1_INIT_SRVC   RDM_BT_SOC_SRVC

/* Enable USB support
*/
#ifdef FEATURE_BUILD_USB
   #define FEATURE_USB
#endif

#ifdef FEATURE_BUILD_USB_HOST
   #define FEATURE_USB_HOST
#endif

#ifdef FEATURE_BUILD_USB_HOST_ENHANCED
   #define FEATURE_USB_HOST_ENHANCED
#endif

#ifdef FEATURE_BUILD_USB_HOST_HID
   #define FEATURE_USB_HOST_HID
#endif

#ifdef FEATURE_BUILD_USB_HOST_MASS_STORAGE
   #define FEATURE_USB_HOST_MASS_STORAGE
#endif

#ifdef FEATURE_BUILD_USB_HOST_ACQUIRE_VBUS
   #define FEATURE_USB_HOST_ACQUIRE_VBUS
#endif

#ifdef FEATURE_BUILD_USB_HOST_TEST_ENHANCED
   #define FEATURE_USB_HOST_TEST_ENHANCED
#endif

#ifdef FEATURE_BUILD_USB_HOST_HID_TEST_ENHANCED
   #define FEATURE_USB_HOST_HID_TEST_ENHANCED
#endif

#ifdef FEATURE_BUILD_USB_HOST_MASS_STORAGE_TEST_ENHANCED
   #define FEATURE_USB_HOST_MASS_STORAGE_TEST_ENHANCED
#endif

/* High Speed USB Feature Definition */
#ifdef FEATURE_SC2X_HAS_HS_USB
#define FEATURE_EXT_USB_P1
#define FEATURE_EPSON_EBI2_MUXED_MODE
#endif

#endif /* CUSTSIO_H */

