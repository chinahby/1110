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

#ifndef _UW_DEVICE_CDC_OBEX_H_
#define _UW_DEVICE_CDC_OBEX_H_

#include <uw_device_cdc.h>

/* HSU addition */
typedef jresult_t (*obex_request_handler_cb_t)(cdc_appctx_t ctx, juint8_t bRequest, 
                             juint16_t wValue, juint16_t wIndex, juint16_t wLength);

/* End of HSU addition*/

typedef struct
{
    cdc_init_info_t basic;
} obex_init_info_t;

typedef struct
{
    cdc_callbacks_t basic;
    /* HSU addition */
    obex_request_handler_cb_t request_handler_cb; 
    /* End of HSU addition*/
} obex_callbacks_t;

jresult_t obex_init(cdc_handle_t handle, obex_callbacks_t *cbs,
    obex_init_info_t *info, cdc_appctx_t *ctx);
    
#endif 

