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
#ifndef _SICD_H_
#define _SICD_H_

#include <ptp_transport_api.h>

enum sicd_container_type
{
    SICD_CONT_UNDEF = 0,
    SICD_CONT_OP,
    SICD_CONT_DATA,
    SICD_CONT_RESP,
    SICD_CONT_EVT
};

/* Bulk only protocol containers */
typedef __PACKED_PRE__ struct sicd_container_common
{
    juint32_t length;
    juint16_t type;      /* enum sicd_container_type */
    juint16_t code;
    juint32_t transaction_id;
} __PACKED_POST__ sicd_container_common_t;

typedef __PACKED_PRE__ struct sicd_container
{
    sicd_container_common_t common;
    juint32_t parameter[PTP_BLOCK_NUM_PARAMS];
} __PACKED_POST__ sicd_container_t;

/* ep0 message structures */

__PACKED_PRE__ struct ep0_canceltrans_data     /* OUT */
{
    juint16_t code;
    juint32_t transaction_id;
} __PACKED_POST__;

__PACKED_PRE__ struct ep0_exevent_data         /* IN */
{
    juint16_t code;
    juint32_t transaction_id;
    juint16_t num_params;
    /*struct
      {
      juint16_t size;
      juint8_t  value[1];
      } param[1];*/
} __PACKED_POST__;

__PACKED_PRE__ struct ep0_devstat_data         /* IN */
{
    juint16_t length;
    juint16_t code;
    juint8_t  ep_num[2];
} __PACKED_POST__;

#endif /*_SICD_H_*/
