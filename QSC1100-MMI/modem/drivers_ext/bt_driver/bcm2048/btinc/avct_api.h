/*****************************************************************************
**
**  Name:           avct_api.h
**
**  Description:    This interface file contains the interface to the Audio
**                  Video Control Transport Protocol (AVCTP).
**
**  Copyright (c) 2003-2004, WIDCOMM Inc., All Rights Reserved.
**  WIDCOMM Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef AVCT_API_H
#define AVCT_API_H

#include "bt_types.h"
#include "bt_target.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

/* API function return value result codes. */
#define AVCT_SUCCESS                0       /* Function successful */
#define AVCT_NO_RESOURCES           1       /* Not enough resources */
#define AVCT_BAD_HANDLE             2       /* Bad handle */
#define AVCT_PID_IN_USE             3       /* PID already in use */
#define AVCT_NOT_OPEN               4       /* Connection not open */

/* PSM for AVCT. */
#define AVCT_PSM                    0x0017

/* Message offset.  The number of bytes needed by the protocol stack for the
** protocol headers of an AVCTP message packet.
*/
#define AVCT_MSG_OFFSET             15

/* Connection role. */
#define AVCT_INT                    0       /* Initiator connection */
#define AVCT_ACP                    1       /* Acceptor connection */

/* Command/Response indicator. */
#define AVCT_CMD                    0       /* Command message */
#define AVCT_RSP                    2       /* Response message */
#define AVCT_REJ                    3       /* Message rejected */

/* Control callback events. */
#define AVCT_CONNECT_CFM_EVT        0       /* Connection confirm */
#define AVCT_CONNECT_IND_EVT        1       /* Connection indication */
#define AVCT_DISCONNECT_CFM_EVT     2       /* Disconnect confirm */
#define AVCT_DISCONNECT_IND_EVT     3       /* Disconnect indication */
#define AVCT_CONG_IND_EVT           4       /* Congestion indication */
#define AVCT_UNCONG_IND_EVT         5       /* Uncongestion indication */


/* General purpose failure result code for callback events. */
#define AVCT_RESULT_FAIL            5

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

/* Control callback function. */
typedef void (tAVCT_CTRL_CBACK)(UINT8 handle, UINT8 event, UINT16 result,
                                BD_ADDR peer_addr);

/* Message callback function */
typedef void (tAVCT_MSG_CBACK)(UINT8 handle, UINT8 label, UINT8 cr,
                               BT_HDR *p_pkt);

/* Structure used by AVCT_CreateConn. */
typedef struct {
    tAVCT_CTRL_CBACK    *p_ctrl_cback;      /* Control callback */
    tAVCT_MSG_CBACK     *p_msg_cback;       /* Message callback */
    UINT16              pid;                /* Profile ID */
    UINT8               role;               /* Initiator/acceptor role */
} tAVCT_CC;

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         AVCT_Register
**
** Description      This is the system level registration function for the
**                  AVCTP protocol.  This function initializes AVCTP and
**                  prepares the protocol stack for its use.  This function
**                  must be called once by the system or platform using AVCTP
**                  before the other functions of the API an be used.
**                  
**
** Returns          void
**
*******************************************************************************/
AVCT_API extern void AVCT_Register(UINT16 mtu, UINT8 sec_mask);

/*******************************************************************************
**
** Function         AVCT_Deregister
**
** Description      This function is called to deregister use AVCTP protocol.
**                  It is called when AVCTP is no longer being used by any
**                  application in the system.  Before this function can be
**                  called, all connections must be removed with
**                  AVCT_RemoveConn().
**                  
**
** Returns          void
**
*******************************************************************************/
AVCT_API extern void AVCT_Deregister(void);

/*******************************************************************************
**
** Function         AVCT_CreateConn
**
** Description      Create an AVCTP connection.  There are two types of
**                  connections, initiator and acceptor, as determined by
**                  the p_cc->role parameter.  When this function is called to
**                  create an initiator connection, an AVCTP connection to
**                  the peer device is initiated if one does not already exist.
**                  If an acceptor connection is created, the connection waits 
**                  passively for an incoming AVCTP connection from a peer device.
**                  
**
** Returns          AVCT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
AVCT_API extern UINT16 AVCT_CreateConn(UINT8 *p_handle, tAVCT_CC *p_cc,
                                       BD_ADDR peer_addr);

/*******************************************************************************
**
** Function         AVCT_RemoveConn
**
** Description      Remove an AVCTP connection.  This function is called when
**                  the application is no longer using a connection.  If this
**                  is the last connection to a peer the L2CAP channel for AVCTP
**                  will be closed.
**                  
**
** Returns          AVCT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
AVCT_API extern UINT16 AVCT_RemoveConn(UINT8 handle);

/*******************************************************************************
**
** Function         AVCT_MsgReq
**
** Description      Send an AVCTP message to a peer device.  In calling
**                  AVCT_MsgReq(), the application should keep track of the
**                  congestion state of AVCTP as communicated with events
**                  AVCT_CONG_IND_EVT and AVCT_UNCONG_IND_EVT.   If the
**                  application calls AVCT_MsgReq() when AVCTP is congested
**                  the message may be discarded.  The application may make its
**                  first call to AVCT_MsgReq() after it receives an
**                  AVCT_CONNECT_CFM_EVT or AVCT_CONNECT_IND_EVT.
**                  
**
** Returns          AVCT_SUCCESS if successful, otherwise error.
**
*******************************************************************************/
AVCT_API extern UINT16 AVCT_MsgReq(UINT8 handle, UINT8 label, UINT8 cr, BT_HDR *p_msg);

#ifdef __cplusplus
}
#endif


#endif /* AVCT_API_H */

