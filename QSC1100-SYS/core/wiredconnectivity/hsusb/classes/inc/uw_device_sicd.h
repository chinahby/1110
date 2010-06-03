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

#ifndef _SICD_API_H_
#define _SICD_API_H_

#include <ptp_responder_api.h>

#ifdef CONFIG_PROTO_DPS
#include <dps_device_api.h>
#endif

#ifdef CONFIG_PROTO_PTP_MTP
#include <mtp_responder_api.h>
#endif

typedef struct {
    juint8_t interface_number;
} sicd_init_info_t;

jresult_t sicd_app_init(ptp_instanceh_t insth, sicd_init_info_t *info);

#endif /*_SICD_API_H_*/
