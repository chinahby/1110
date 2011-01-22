#ifndef _PS_PPP_EVENTS_H
#define _PS_PPP_EVENTS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ P P P _ E V E N T . H

GENERAL DESCRIPTION
  This header file contains the definitions of the enums that are used to
  generate PPP events.



    Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_events.h_v   1.1   27 Feb 2003 15:43:00   jysong  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_events.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/02/03    mvl    Complete modularization of the authentication code which
                   entailed adding PPP_EV_INVALID_PROTO to event_proto type
03/17/03    js     Added the prefix PS_PPP_EVENT_ to ppp_event_state_type.
03/07/03    js     Added include files, and inline documentations etc. 
02/27/03    js     initial revision.
02/25/03    mvl    created file
===========================================================================*/
#include "comdef.h"


#include "event.h"
#include "event_defs.h"

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_PROTOCOL_TYPE

DESRIPTION
  enum that defines the various supported PPP protocols - the event mechanism
  will map these to the corresponding strings
---------------------------------------------------------------------------*/
typedef enum 
{
     PPP_EV_LCP,
     PPP_EV_PAP,
     PPP_EV_CHAP,
     PPP_EV_IPCP,
     PPP_EV_IPv6CP,
     PPP_EV_INVALID_PROTO = 255   /* this should NEVER be used as payload! */
} ppp_event_protocol_type;


/*---------------------------------------------------------------------------
TYPEDEF PPP_EVENT_STATE_TYPE

DESCRIPTION
  enum that defines the states of the protocol for event repporting.  
---------------------------------------------------------------------------*/
typedef enum
{
   PS_PPP_EVENT_UNKNOWN,
   PS_PPP_EVENT_STARTING, 
   PS_PPP_EVENT_OPEN,
   PS_PPP_EVENT_CLOSING, 
   PS_PPP_EVENT_CLOSED
} ppp_event_state_type;

/*--------------------------------------------------------------------------
TYPEDEF PPP_EVENT_PAYLOAD_TYPE

DESCRIPTION
    PPP Event Payload Type
--------------------------------------------------------------------------*/
typedef struct 
{
   int                      ppp_event_device;     /* PPP on dev0 or dev1  */
   ppp_event_protocol_type  ppp_event_protocol;   /* protocol type        */ 
   ppp_event_state_type     ppp_event_state;      /* Current state        */
   ppp_event_state_type     ppp_event_prev_state; /* previous state       */ 
} ppp_event_payload_type;

#endif /* _PS_PPP_EVENTS_H */
