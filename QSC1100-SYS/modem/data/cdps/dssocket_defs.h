#ifndef _DSSOCKET_DEFS_H
#define _DSSOCKET_DEFS_H

/*===========================================================================

   D A T A   S E R V I C E S   S O C K E T   D E F S   H E A D E R   F I L E
                   
DESCRIPTION

 The Data Services Socket Definitions Header File. This file contains 
 declarations used internally by the DMSS code as well as by the socket
 applications.

  Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssocket_defs.h#2 $ 
  $Author: nsivakum $ $DateTime: 2007/12/10 23:14:44 $ 

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/25/03    ss     Created module.

===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_SOCKETS
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================
  Flags passed to dss_sendto() call for SDB. Non-SDB related flags are 
  defined in dssocket.h. Any time a SDB related flags is added here, OR it 
  with the existing flags in MSG_VALID_BITS macro definition in dssocki.c 
  so that the reserved bit mask can be properly built.
===========================================================================*/
/*---------------------------------------------------------------------------
  SDB flags will cause short data burst to be used if the mobile is not on
  traffic. Don't set any bit if one doesn't want SDB and traffic is to be 
  brought up. Set both bits if one doesn't care which channel is used for
  SDB transmission, else set the appropriate bit.
---------------------------------------------------------------------------*/
#define MSG_EXPEDITE      0x00000001   /* use ACH/REACH/TCH                */
#define MSG_FAST_EXPEDITE 0x00000002   /* use     REACH/TCH                */

/*---------------------------------------------------------------------------
  Enum to indicate the SDB ack status code to the application. The pkt mgr
  receives the status from CM and maps it to the this enum to be returned
  to the application. Changes to CM/Pkt mgr may necessitate modifying this
  enum definition.
---------------------------------------------------------------------------*/
typedef enum dss_sdb_ack_status_enum_type
{
  DSS_SDB_ACK_NONE = -1,                         /* completed successfully */
  DSS_SDB_ACK_OK = 0,                            /* completed successfully */
  DSS_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT = 1,      /* hold orig retry timeout */
  DSS_SDB_ACK_HOLD_ORIG = 2,   /* cannot process because hold orig is true */
  DSS_SDB_ACK_NO_SRV = 3,                                    /* no service */
  DSS_SDB_ACK_ABORT = 4,                                          /* abort */
  DSS_SDB_ACK_NOT_ALLOWED_IN_AMPS = 5,       /* cannot send in analog mode */
  DSS_SDB_ACK_NOT_ALLOWED_IN_HDR = 6,           /* cannot send in HDR call */
  DSS_SDB_ACK_L2_ACK_FAILURE = 7,              /* failure receiving L2 ack */
  DSS_SDB_ACK_OUT_OF_RESOURCES = 8,          /* e.g., out of memory buffer */
  DSS_SDB_ACK_ACCESS_TOO_LARGE = 9,   /* msg too large to be sent over acc */
  DSS_SDB_ACK_DTC_TOO_LARGE = 10,     /* msg too large to be sent over DTC */
  DSS_SDB_ACK_OTHER = 11,          /* any status response other than above */
  DSS_SDB_ACK_ACCT_BLOCK = 12,   /* Access blocked based on service option */
  DSS_SDB_ACK_FORCE_32_BIT = 0x7FFFFFFF     /* Force the enum to be 32-bit */
} dss_sdb_ack_status_enum_type;

/*---------------------------------------------------------------------------
  The structure containing information about the sdb ack status. This info 
  is returned to the socket application in the sdb ack callback if the sdb 
  ack callback socket option is set.
---------------------------------------------------------------------------*/
typedef struct dss_sdb_ack_status_info_type
{
  uint32                       overflow;
  dss_sdb_ack_status_enum_type status;
} dss_sdb_ack_status_info_type;


extern struct dss_socket_config_s
{
  /*-------------------------------------------------------------------------
    Flag to determine whether to do a graceful close or abort the
    connection when the associated interface is dormant.
  -------------------------------------------------------------------------*/
  boolean                   tcp_graceful_dormant_close;
}sock_config_cb;

/*===========================================================================

                          PUBLIC MACRO DECLARATIONS

===========================================================================*/

/*===========================================================================

MACRO DSS_HTONL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))


/*===========================================================================

MACRO DSS_HTONS()

DESCRIPTION
 Converts host-to-network short integer.  Handles potential byte order
 differences between different computer architectures and different network
 protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_htons(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
        (((uint16)(x) & 0xff00) >> 8)))


/*===========================================================================

MACRO DSS_NTOHL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))


/*===========================================================================

MACRO DSS_NTOHS()

DESCRIPTION
 Converts network-to-host short integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned short integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

===========================================================================*/
#define dss_ntohs(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
        (((uint16)(x) & 0xff00) >> 8)))


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
#endif  /* FEATURE_DS_SOCKETS */

#endif  /* _DSSOCKET_DEFS_H */
