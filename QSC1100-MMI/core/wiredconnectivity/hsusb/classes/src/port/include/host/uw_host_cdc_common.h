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
/* Common CDC Class Driver data & structures */

#ifndef _UW_HOST_CDC_COMMON
#define _UW_HOST_CDC_COMMON

/* General CDC Definitions */
typedef void *cdc_ctx_t;
typedef void *os_cdc_ctx_t;

/* Data Transfer flags */
typedef juint8_t cdc_xfer_flag_t;
#define CDC_DMA_BUFFER  0x01

#endif  /* _UW_HOST_CDC_COMMON */
