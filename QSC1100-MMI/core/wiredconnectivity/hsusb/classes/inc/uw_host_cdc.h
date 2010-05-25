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

/* CDC Class Driver General API */

#ifndef _UW_HOST_CDC_H_
#define _UW_HOST_CDC_H_

#include <usbdi_status.h>
#include <jtypes.h>

/* General CDC Context Types */
typedef void *uwh_cdc_dev_h;
typedef void *uwh_cdc_app_h;

/* General CDC Application Callback Types */
/**
 * Function type:  cdc_general_command_cb_t
 * Description:    General CDC request completion application callback type.
 * Parameters:
 *     @app_ctx: (IN) Application context. 
 *     @app_priv: (IN) Private application data.
 *     @status:  (IN) Request status.
 *
 * Return value:   None
 * Scope:          Global
 **/
typedef void (*cdc_general_command_cb_t)(uwh_cdc_app_h app_ctx, 
    void *app_priv, jresult_t status);

/**
 * Function type:  cdc_general_data_cb_t
 * Description:    General CDC data transfer completion application callback
 *                 type.
 * Parameters:
 *     @app_ctx: (IN) Application context. 
 *     @app_priv: (IN) Private application data.
 *     @buffer:  (IN) Handler to a buffer containing the transfer data.
 *     @act_len: (IN) The actual amount of bytes transferred.
 *     @status:  (IN) Transfer status.
 *
 * Return value:   None
 * Scope:          Global
 **/
typedef void (*cdc_general_data_cb_t)(uwh_cdc_app_h app_ctx, void *app_priv, 
    void *buffer, juint32_t act_len, jresult_t status);

/* Communication Class Interface Requests */
jresult_t cdc_encapsulated_command_send(uwh_cdc_dev_h dev_ctx, void *cmd, 
    juint16_t size, cdc_general_command_cb_t cb, uwh_cdc_app_h app_ctx,
    void *app_priv);

jresult_t cdc_encapsulated_response_get(uwh_cdc_dev_h dev_ctx, 
    void *buffer, juint16_t size, cdc_general_data_cb_t cb, 
    uwh_cdc_app_h app_ctx, void *app_priv);

/* Data Class Interface Transfers */
jresult_t cdc_data_send(uwh_cdc_dev_h dev_ctx, void *buffer, 
    juint32_t size, cdc_general_data_cb_t cb, uwh_cdc_app_h app_ctx, 
    void *app_priv);
jresult_t cdc_data_get(uwh_cdc_dev_h dev_ctx, void *buffer, juint32_t size,
    cdc_general_data_cb_t cb, uwh_cdc_app_h app_ctx, void *app_priv);

#ifdef CONFIG_JHOST_CDC_UTILS
jresult_t cdc_data_send_safe(uwh_cdc_dev_h dev_ctx, void *buffer,
    juint32_t size, cdc_general_data_cb_t cb,
    uwh_cdc_app_h app_ctx, void *app_priv);
jresult_t cdc_data_get_safe(uwh_cdc_dev_h dev_ctx, void *buffer,
    juint32_t size, cdc_general_data_cb_t cb, uwh_cdc_app_h app_ctx, 
    void *app_priv);
#endif

#endif

