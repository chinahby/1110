/*****************************************************************************
**
**  Name:          l2c_api.h
**
**  Description:   this file contains the L2CAP API definitions
**
**
**  Copyright (c) 1999-2004, WIDCOMM Inc., All Rights Reserved.
**  WIDCOMM Bluetooth Core. Proprietary and confidential.
******************************************************************************/
#ifndef L2C_API_H
#define L2C_API_H

#include "bt_target.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

/* Define the minimum offset that L2CAP needs in a buffer. This is made up of
** HCI type, len, handle, L2CAP len and CID
*/
#if (L2CAP_FCR_INCLUDED==TRUE)
#define L2CAP_MIN_OFFSET    11
#else
#define L2CAP_MIN_OFFSET    9
#endif

/* Minimum offset for broadcast needs another two bytes for the PSM */
#define L2CAP_BCST_MIN_OFFSET       11

/* ping result codes */
#define L2CAP_PING_RESULT_OK        0       /* Ping reply received OK     */
#define L2CAP_PING_RESULT_NO_LINK   1       /* Link could not be setup    */
#define L2CAP_PING_RESULT_NO_RESP   2       /* Remote L2CAP did not reply */

/* Bits for 1.2 features mask used in L2CA_SetFeatureMask and L2CA_ClearFeatureMask */
/* Octet 0 of supported feature mask returned in Info Response */
#define L2CAP_FCR_FEAT_RTRANS       (1 << 0)
#define L2CAP_FCR_FEAT_FC           (1 << 1)
#define L2CAP_FCR_FEAT_QOS          (1 << 2)

/* Mask for flow control and Retransmission features */
#define L2CAP_FCR_FEAT_MASK         (L2CAP_FCR_FEAT_RTRANS | L2CAP_FCR_FEAT_FC)

/* result code for L2CA_DataWrite() */
#define L2CAP_DW_FAILED        FALSE
#define L2CAP_DW_SUCCESS       TRUE
#define L2CAP_DW_CONGESTED     2

/* Values for priority parameter to L2CA_SetTxPriority */
#define L2CAP_PRIORITY_NORMAL       0
#define L2CAP_PRIORITY_HIGH         1

/* Data Packet Flags  (bits 0-14 are reserved) */
#define L2CAP_FLUSHABLE_MASK        0x8000
#define L2CAP_FLUSHABLE_PKT         0x8000
#define L2CAP_NONE_FLUSHABLE_PKT    0x0000

/*****************************************************************************
**  Type Definitions
*****************************************************************************/

typedef struct
{
#define L2CAP_FCR_BASIC_MODE  0x00
#define L2CAP_FCR_RTRANS_MODE 0x01
#define L2CAP_FCR_FC_MODE     0x02

    UINT8  mode;

    UINT8  tx_win_sz;
    UINT8  max_transmit;
    UINT16 rtrans_tout;
    UINT16 mon_tout;
    UINT16 mps;

    UINT8  tx_pool_id;
    UINT16 tx_buf_sz;
    UINT8  rx_pool_id;
} tL2CAP_FCR_OPTS;

/* Define a structure to hold the configuration parameters. Since the
** parameters are optional, for each parameter there is a boolean to
** use to signify its presence or absemce.
*/
typedef struct
{
    UINT16      result;                 /* Only used in confirm messages */
    BOOLEAN     mtu_present;
    UINT16      mtu;
    BOOLEAN     qos_present;
    FLOW_SPEC   qos;
    BOOLEAN     flush_to_present;
    UINT16      flush_to;
    BOOLEAN     fcr_present;
    tL2CAP_FCR_OPTS fcr;
    UINT16      cong_param; /* Congestion would start at % of this and ends at %of this */

    UINT16      flags;                  /* Internally used by L2CAP */
} tL2CAP_CFG_INFO;


/*********************************
**  Callback Functions Prototypes
**********************************/

/* Connection indication callback prototype. Parameters are
**              BD Address of remote
**              Local CID assigned to the connection
**              PSM that the remote wants to connect to
**              Identifier that the remote sent
*/
typedef void (tL2CA_CONNECT_IND_CB) (BD_ADDR, UINT16, UINT16, UINT8);


/* Connection confirmation callback prototype. Parameters are
**              Local CID
**              Result - 0 = connected, non-zero means failure reason
*/
typedef void (tL2CA_CONNECT_CFM_CB) (UINT16, UINT16);


/* Connection pending callback prototype. Parameters are
**              Local CID
*/
typedef void (tL2CA_CONNECT_PND_CB) (UINT16);


/* Configuration indication callback prototype. Parameters are
**              Local CID assigned to the connection
**              Pointer to configuration info
*/
typedef void (tL2CA_CONFIG_IND_CB) (UINT16, tL2CAP_CFG_INFO *);


/* Configuration confirm callback prototype. Parameters are
**              Local CID assigned to the connection
**              Pointer to configuration info
*/
typedef void (tL2CA_CONFIG_CFM_CB) (UINT16, tL2CAP_CFG_INFO *);


/* Disconnect indication callback prototype. Parameters are
**              Local CID
**              Boolean whether upper layer should ack this
*/
typedef void (tL2CA_DISCONNECT_IND_CB) (UINT16, BOOLEAN);


/* Disconnect confirm callback prototype. Parameters are
**              Local CID
**              Result
*/
typedef void (tL2CA_DISCONNECT_CFM_CB) (UINT16, UINT16);


/* QOS Violation indication callback prototype. Parameters are
**              BD Address of violating device
*/
typedef void (tL2CA_QOS_VIOLATION_IND_CB) (BD_ADDR);


/* Data received indication callback prototype. Parameters are
**              Local CID
**              Address of buffer queue
*/
typedef void (tL2CA_DATA_IND_CB) (UINT16, BT_HDR *);


/* Echo response callback prototype. Note that this is not included in the
** registration information, but is passed to L2CAP as part of the API to
** actually send an echo request. Parameters are
**              Result
*/
typedef void (tL2CA_ECHO_RSP_CB) (UINT16);


/* Congestion status callback protype. This callback is optional. If
** an application tries to send data when the transmit queue is full,
** the data will anyways be dropped. The parameter is:
**              Local CID
**              TRUE if congested, FALSE if uncongested
*/
typedef void (tL2CA_CONGESTION_STATUS_CB) (UINT16, BOOLEAN);

/* Define the structure that applications use to register with
** L2CAP. This structure includes callback functions. All functions
** MUST be provided, with the exception of the "connect pending"
** callback and "congestion status" callback.
*/
typedef struct
{
    tL2CA_CONNECT_IND_CB        *pL2CA_ConnectInd_Cb;
    tL2CA_CONNECT_CFM_CB        *pL2CA_ConnectCfm_Cb;
    tL2CA_CONNECT_PND_CB        *pL2CA_ConnectPnd_Cb;
    tL2CA_CONFIG_IND_CB         *pL2CA_ConfigInd_Cb;
    tL2CA_CONFIG_CFM_CB         *pL2CA_ConfigCfm_Cb;
    tL2CA_DISCONNECT_IND_CB     *pL2CA_DisconnectInd_Cb;
    tL2CA_DISCONNECT_CFM_CB     *pL2CA_DisconnectCfm_Cb;
    tL2CA_QOS_VIOLATION_IND_CB  *pL2CA_QoSViolationInd_Cb;
    tL2CA_DATA_IND_CB           *pL2CA_DataInd_Cb;
    tL2CA_CONGESTION_STATUS_CB  *pL2CA_CongestionStatus_Cb;

} tL2CAP_APPL_INFO;


/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         L2CA_Register
**
** Description      Other layers call this function to register for L2CAP
**                  services.
**
** Returns          TRUE of registered OK, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_Register (UINT16 psm, tL2CAP_APPL_INFO *p_cb_info);

/*******************************************************************************
**
** Function         L2CA_Deregister
**
** Description      Other layers call this function to deregister for L2CAP
**                  services.
**
** Returns          void
**
*******************************************************************************/
L2C_API extern void L2CA_Deregister (UINT16 psm);

/*******************************************************************************
**
** Function         L2CA_ConnectReq
**
** Description      Higher layers call this function to create an L2CAP connection.
**                  Note that the connection is not established at this time, but
**                  connection establishment gets started. The callback function
**                  will be invoked when connection establishes or fails.
**
** Returns          the CID of the connection, or 0 if it failed to start
**
*******************************************************************************/
L2C_API extern UINT16 L2CA_ConnectReq (UINT16 psm, BD_ADDR p_bd_addr);

/*******************************************************************************
**
** Function         L2CA_ConnectRsp
**
** Description      Higher layers call this function to accept an incoming
**                  L2CAP connection, for which they had gotten an connect
**                  indication callback.
**
** Returns          TRUE for success, FALSE for failure
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_ConnectRsp (BD_ADDR p_bd_addr, UINT8 id, UINT16 lcid,
                                        UINT16 result, UINT16 status);

/*******************************************************************************
**
** Function         L2CA_ConfigReq
**
** Description      Higher layers call this function to send configuration.
**
** Returns          TRUE if configuration sent, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_ConfigReq (UINT16 cid, tL2CAP_CFG_INFO *p_cfg);

/*******************************************************************************
**
** Function         L2CA_ConfigRsp
**
** Description      Higher layers call this function to send a configuration
**                  response.
**
** Returns          TRUE if configuration response sent, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_ConfigRsp (UINT16 cid, tL2CAP_CFG_INFO *p_cfg);

/*******************************************************************************
**
** Function         L2CA_DisconnectReq
**
** Description      Higher layers call this function to disconnect a channel.
**
** Returns          TRUE if disconnect sent, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_DisconnectReq (UINT16 cid);

/*******************************************************************************
**
** Function         L2CA_DisconnectRsp
**
** Description      Higher layers call this function to acknowledge the 
**                  disconnection of a channel.
**
** Returns          void
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_DisconnectRsp (UINT16 cid);

/*******************************************************************************
**
** Function         L2CA_DataWrite
**
** Description      Higher layers call this function to write data.
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
L2C_API extern UINT8 L2CA_DataWrite (UINT16 cid, BT_HDR *p_data);

/*******************************************************************************
**
** Function         L2CA_Ping
**
** Description      Higher layers call this function to send an echo request.
**
** Returns          TRUE if echo request sent, else FALSE.
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_Ping (BD_ADDR p_bd_addr, tL2CA_ECHO_RSP_CB *p_cb);

/*******************************************************************************
**
** Function         L2CA_SetIdleTimeout
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection, or for all future connections. The "idle timeout"
**                  is the amount of time that a connection can remain up with
**                  no L2CAP channels on it. A timeout of zero means that the
**                  connection will be torn down immediately when the last channel
**                  is removed. A timeout of 0xFFFF means no timeout. Values are
**                  in seconds.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SetIdleTimeout (UINT16 cid, UINT16 timeout,
                                            BOOLEAN is_global);

/*******************************************************************************
**
** Function         L2CA_SetIdleTimeoutByBdAddr
**
** Description      Higher layers call this function to set the idle timeout for
**                  a connection. The "idle timeout" is the amount of time that 
**                  a connection can remain up with no L2CAP channels on it. 
**                  A timeout of zero means that the connection will be torn 
**                  down immediately when the last channel is removed. 
**                  A timeout of 0xFFFF means no timeout. Values are in seconds. 
**                  A bd_addr is the remote BD address. If bd_addr = BT_BD_ANY, 
**                  then the idle timeouts for all active l2cap links will be 
**                  changed.
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This timeout applies to all logical channels active on the 
**                  ACL link.
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SetIdleTimeoutByBdAddr(BD_ADDR bd_addr, UINT16 timeout);

/*******************************************************************************
**
** Function         L2CA_SetTraceLevel
**
** Description      This function sets the trace level for L2CAP. If called with
**                  a value of 0xFF, it simply reads the current trace level.
**
** Returns          the new (current) trace level
**
*******************************************************************************/
L2C_API extern UINT8 L2CA_SetTraceLevel (UINT8 trace_level);

/*******************************************************************************
**
** Function     L2CA_SetDesireRole
**
** Description  This function sets the desire role for L2CAP.
**              If the new role is a valid value (HCI_ROLE_MASTER or HCI_ROLE_SLAVE),
**              the desire role is set to the new value. Otherwise, it is not changed.
**
** Returns      the new (current) role
**
*******************************************************************************/
L2C_API extern UINT8 L2CA_SetDesireRole (UINT8 new_role);

/*******************************************************************************
**
** Function         L2CA_SetAclPriority
**
** Description      Sets the transmission priority for an ACL channel.
**                  (For initial implementation only two values are valid.
**                  L2CAP_PRIORITY_NORMAL and L2CAP_PRIORITY_HIGH).
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SetAclPriority (BD_ADDR bd_addr, UINT8 priority);

/*******************************************************************************
**
** Function         L2CA_FlowControl
**
** Description      Higher layers call this function to flow control a channel.
**
** Returns          TRUE if valid channel, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_FlowControl (UINT16 cid, BOOLEAN on_off);

/*******************************************************************************
**
** Function         L2CA_SetFeatureMask
**
** Description      Higher layers call this function to set flow control features.
**
** Returns          none
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SetFeatureMask (UINT16 psm, UINT32 ext_fea_mask);

/*******************************************************************************
**
** Function         L2CA_ClearFeatureMask
**
** Description      Higher layers call this function to clear flow control features.
**
** Returns          none
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_ClearFeatureMask (UINT16 psm, UINT32 ext_fea_mask);

/*******************************************************************************
**
** Function         L2CA_SendTestSFrame
**
** Description      Higher layers call this function to send a test S-frame.
**
** Returns          TRUE if valid Channel, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SendTestSFrame (UINT16 cid, BOOLEAN rr_or_rej,
                                            UINT8 back_track);

/*******************************************************************************
**
** Function         L2CA_SetTxPriority
**
** Description      Sets the transmission priority for a channel. (FCR Mode)
**
** Returns          TRUE if a valid channel, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SetTxPriority (UINT16 cid, UINT8 priority);

/*********************************
**  Callback Functions Prototypes
**  Compression Callback for free compression memory pool
**********************************/
typedef void (tL2CA_COMP_FREEMEM_CB) (UINT8 *p_mem_pool);

/*********************************
**  Callback Functions Prototypes
**  Server Side Compression Callback during compression negotiation
**********************************/
/* Server Side Compression Callback prototype. Parameters are
** Inputs:
**              peer_addr: peer BD addr
**              client_pe_type: compression PE type of the client
**              client_mem_level:  compression memory level of the client
**              client_wbits: compression window bits of the client
**              direction: direction of the compress(0 for output, 
**                                 1 for input, 2 for bidiection)
** Outputs:
**              negotiated_mem_level: negotiated memory level sent back to client
**              negotiated_wbits: negotiated window bits sent back to client
**              p_mem_pool: server provide compression memory pool
**              mem_pool_size: server provide mem_pool_size
**  Return:
**              TRUE for accept compression, FALSE for reject
*/
typedef BOOLEAN (tL2CA_REGISTERCOMPRESSION_CBACK) (BD_ADDR peer_addr,
                                                   UINT8 client_pe_type, UINT8 client_mem_level,  UINT8 client_wbits, 
                                                   UINT8 direction, UINT8 *negotiated_mem_level, UINT8 *negotiated_wbits,
                                                   UINT8 **p_mem_pool, UINT32 *mem_pool_size);


/*******************************************************************************
**
** Function         L2CA_RegisterCompression
**
** Description      Register compression callback for the server for compression negotiation.
** Input Param      p_cb - callback for server compression negotiation
**                  p_free_mem_cb - callback for free memory
**
** Returns      
**                  
*******************************************************************************/
L2C_API void L2CA_RegisterCompression(tL2CA_REGISTERCOMPRESSION_CBACK *p_cb, 
                                      tL2CA_COMP_FREEMEM_CB *p_free_mem_cb);

/*********************************
**  Callback Functions Prototypes
**  Client Side Compression Callback during compression negotiation to notify the application of the compression
**  negotiation result with the server
**********************************/
/*  Client Side Compression Callback prototype. Parameters are
** Inputs:
**              UINT8 status: TRUE for success, FALSE for fail
** 
**  Return:
**              
*/
typedef void (tL2CA_SETCOMPRESSION_CBACK) (UINT8 status);

/*******************************************************************************
**
** Function         L2CA_SetCompression
**
** Description      Client turn on/off compression 
** Input Param      local_cid: local l2cap channel id 
**                  pe_type: compression pe type for client
**                  mem_level: compression memory level for client
**                  wbits: compression window bits for client
**                  direction: direction of the compress(0 for output, 1 for input, 2 for bidiection)
**                  p_cb: callback for notify client application of the negotiation result
**                  enable: TRUE for turn on compression, FALSE for turn off compression
**                  p_mem_pool: pointer to the memory pool used for compress for client
**                  mem_pool_size: size of the memeory pool
**                  p_free_mem_cb - callback for free memory
**
** Returns      
**                  
*******************************************************************************/
L2C_API void L2CA_SetCompression(UINT16 local_cid, UINT8 pe_type, UINT8 mem_level, UINT8 wbits, 
                                 UINT8 direction, tL2CA_SETCOMPRESSION_CBACK *p_cb, BOOLEAN enable,
                                 UINT8 *p_mem_pool, UINT32 mem_pool_size, tL2CA_COMP_FREEMEM_CB *p_free_mem_cb);

/*******************************************************************************
**
** Function         L2CA_GetCompressionFixMemoryPoolSize
**
** Description      Get the compression fix memory pool required size based on window bits and memeory level 
** Input Param      mem_level: compression memory level for client
**                  wbits: compression window bits for client
**                  
**
** Returns          Required fix memeory pool size
**                  
*******************************************************************************/
L2C_API UINT32 L2CA_GetCompressionFixMemoryPoolSize(UINT8 mem_level, UINT8 wbits);


/*******************************************************************************
**
** Function         L2CA_Flush
**
** Description      Higher layers call this function to flush data.
**
** Returns          TRUE if data accepted, else FALSE
**
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_Flush (UINT16 cid);

/*******************************************************************************
**
** Function     L2CA_GetNumQueuedBufs
**
** Description  This function returns the number of buffers queued for that CID
**
** Returns      Number of queued for that CID
**
*******************************************************************************/
L2C_API extern UINT16 L2CA_GetNumQueuedBufs (UINT16 lcid);

/*******************************************************************************
**
** Function         L2CA_SetFlushTimeout
**
** Description      This function set the automatic flush time out in Baseband 
**                  for ACL-U packets.
**                  BdAddr : the remote BD address of ACL link. If it is BT_DB_ANY
**                           then the flush time out will be applied to all ACL link.
**                  FlushTimeout: flush time out in ms
**                           0x0000 : No automatic flush  
**                           L2CAP_NO_RETRANSMISSION : No retransmission  
**                           0x0002 - 0xFFFE : flush time out, if (flush_tout*8)+3/5) 
**                                    <= HCI_MAX_AUTO_FLUSH_TOUT (in 625us slot).
**                                    Otherwise, return FALSE.
**                           L2CAP_NO_AUTOMATIC_FLUSH : No automatic flush  
**
** Returns          TRUE if command succeeded, FALSE if failed
**
** NOTE             This flush timeout applies to all logical channels active on the 
**                  ACL link.
*******************************************************************************/
L2C_API extern BOOLEAN L2CA_SetFlushTimeout (BD_ADDR bd_addr, UINT16 flush_tout);

/*******************************************************************************
**
** Function         L2CA_DataWriteEx
**
** Description      Higher layers call this function to write data and they set
**                  this data flushable or not.
**                  flags : L2CAP_FLUSHABLE_PKT or L2CAP_NONE_FLUSHABLE_PKT
**                          bits 0 - 14 are reserved
**
** Returns          L2CAP_DW_SUCCESS, if data accepted, else FALSE
**                  L2CAP_DW_CONGESTED, if data accepted and the channel is congested
**                  L2CAP_DW_FAILED, if error
**
*******************************************************************************/
L2C_API extern UINT8 L2CA_DataWriteEx (UINT16 cid, BT_HDR *p_data, UINT16 flags);

#ifdef __cplusplus
}
#endif

#endif  /* L2C_API_H */
