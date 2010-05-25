#ifndef PS_PPPAUTH_H
#define PS_PPPAUTH_H
/*===========================================================================

                          P S _ P P P _ A U T H . H

DESCRIPTION
  Header file for PPP authentication

  Copyright (c) 1999-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_auth.h_v   1.3   21 Feb 2003 20:42:56   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_auth.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
21/01/04    aku    Moved some field length defines from .c to .h
09/12/03    mvl    Added some JCDMA timing changes.
04/08/03    mvl    Moved PPP_CHAP_MD5 to ps_ppp_defs.h
04/06/03    mvl    Addition of capabilities to support server style
                   authentication for both PAP and CHAP. 
04/02/03    mvl    Complete modularization of the authentication code.
02/21/03    jd     moved & renamed ppp_auth_get_from_nv() from this file to
                   to is707_get_ppp_auth_info_from_nv() in ps707_dssnet.h
02/06/03    jd     Renamed ppp_auth_get_info() to ppp_auth_get_from_nv()
01/31/03    jd     Added ppp_auth_get_info() prototype
08/02/02    mvl    moved authentication declarations here.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Message Id's for PAP and CHAP packets
---------------------------------------------------------------------------*/
#define PPP_PAP_REQ         0x01      /* Authenticate Request              */
#define PPP_PAP_ACK         0x02      /* Authenticate Acknowledge          */
#define PPP_PAP_NAK         0x03      /* Authenticate Nak                  */
#define PPP_CHAP_CHALLENGE  0x01      /* Code for CHAP Challenge           */
#define PPP_CHAP_RESPONSE   0x02      /* Code for CHAP Response            */
#define PPP_CHAP_SUCCESS    0x03      /* Code for CHAP Success             */
#define PPP_CHAP_FAILURE    0x04      /* Code for CHAP Failure             */

/*---------------------------------------------------------------------------
  Length of the various fields in Auth packets
---------------------------------------------------------------------------*/
#define PPP_AUTH_CODE_SIZE     1
#define PPP_AUTH_ID_SIZE       1
#define PPP_AUTH_LENGTH_SIZE   2
#define PPP_CHAP_VALUE_SIZE    1
#define PPP_PAP_UID_SIZE       1
#define PPP_PAP_PWD_SIZE       1

/*---------------------------------------------------------------------------
  The time-out value for the CHAP. This will be given a value of 3 seconds.
  The value should be given in milliseconds.
---------------------------------------------------------------------------*/
#define PPP_AUTH_TIMER_VAL 3000

/*---------------------------------------------------------------------------
  Constant to load the initial value of the Retry counter. If mobile does
  not receive a reply from the PDSN, then it will retry this number of times
  before giving up
---------------------------------------------------------------------------*/
#define PPP_AUTH_RETRY_COUNTER_VAL 5

#ifdef FEATURE_JCDMA_1X
#undef PPP_AUTH_RETRY_COUNTER_VAL

#define PPP_AUTH_RETRY_COUNTER_VAL 9
#endif /* FEATURE_JCDMA_1X */

/*---------------------------------------------------------------------------
  Size of the MD5 output. This is 16 bytes.
---------------------------------------------------------------------------*/
#define PPP_CHAP_MD5_SIZE           16

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                        PUBLIC FUNCTION DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PPP_AUTH_START()

DESCRIPTION
  This function checks for authentication and kicks it off if it was
  negotiated, otherwise it starts IPCP.

PARAMETERS
  device: the device on which auth is being started.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_auth_start
(
  ppp_dev_enum_type device
);

/*===========================================================================
FUNCTION PPP_AUTH_COMPLETE()

DESCRIPTION
  This function completes the PPP authentication phase and starts the PPP
  network phase.  This includes cleaning up of any data that was stored.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for completed authentication
  protocol:   protocol which completed (for protocol specific cleanup).  

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Cleans the buffer for the incoming packet and returns buffer to free q.
  Changes ppp_cb_ptr->flags to show that authentication is complete.
===========================================================================*/
void ppp_auth_complete
(
  ppp_type *ppp_cb_ptr,                     /* ptr to PPP control block    */
  uint16    protocol                        /* the completed auth protocol */
);

/*===========================================================================
FUNCTION PPP_AUTH_TIMEOUT()

DESCRIPTION
  This function will be called by other functions on time out.

PARAMETERS
  ppp_cb_ptr: ppp control block pointer for timeout.

RETURN VALUE
  TRUE:  Informs the calling function to retry
  FALSE: Informs calling function, retries are over and to
         terminate the call.

DEPENDENCIES
  Same as dependencies of the functions ppp_chap_timeout

SIDE EFFECTS
  If number of the retries are non-zero then an authentication packet is
  transmitted.
===========================================================================*/
boolean ppp_auth_timeout
(
  ppp_type *ppp_cb_ptr                         /* ptr to PPP control block */
);

/*===========================================================================
FUNCTION CHAP_PROC()

DESCRIPTION
  This function does the protocol processing for CHAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the CHAP pkt

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void chap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
);

/*===========================================================================
FUNCTION PAP_PROC()

DESCRIPTION
  This function does the protocol processing for PAP.

PARAMETERS
  device:       the PPP interface the packet arrived on
  item_ref_ptr: reference to pointer to dsm chain containing the PAP pkt.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pap_proc
(
  ppp_dev_enum_type   device,
  dsm_item_type     **item_ref_ptr
);

#endif /* PS_PPPAUTH_H */
