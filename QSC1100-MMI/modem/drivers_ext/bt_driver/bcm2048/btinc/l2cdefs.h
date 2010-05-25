/****************************************************************************/
/*                                                                          */
/*  Name:       l2cdefs.h                                                   */
/*                                                                          */
/*  Function:   this file contains L2CAP protocol definitions               */
/*                                                                          */
/*                                                                          */
/*  Date        Modification                                                */
/*  ------------------------                                                */
/*  11/24/99    Ash  Create                                                 */
/*                                                                          */
/*  Copyright (c) 1999-2004, WIDCOMM Inc., All Rights Reserved.             */
/*  WIDCOMM Bluetooth Core. Proprietary and confidential.                   */
/*                                                                          */
/****************************************************************************/

#ifndef L2CDEFS_H
#define L2CDEFS_H


/* L2CAP command codes
*/
#define L2CAP_CMD_REJECT                    0x01
#define L2CAP_CMD_CONN_REQ                  0x02
#define L2CAP_CMD_CONN_RSP                  0x03
#define L2CAP_CMD_CONFIG_REQ                0x04
#define L2CAP_CMD_CONFIG_RSP                0x05
#define L2CAP_CMD_DISC_REQ                  0x06
#define L2CAP_CMD_DISC_RSP                  0x07
#define L2CAP_CMD_ECHO_REQ                  0x08
#define L2CAP_CMD_ECHO_RSP                  0x09
#define L2CAP_CMD_INFO_REQ                  0x0A
#define L2CAP_CMD_INFO_RSP                  0x0B


/* Define some packet and header lengths
*/
#define L2CAP_PKT_OVERHEAD      4           /* Length and CID                       */
#define L2CAP_CMD_OVERHEAD      4           /* Cmd code, Id and length              */
#define L2CAP_CMD_REJECT_LEN    2           /* Reason (data is optional)            */
#define L2CAP_CONN_REQ_LEN      4           /* PSM and source CID                   */
#define L2CAP_CONN_RSP_LEN      8           /* Dest CID, source CID, reason, status */
#define L2CAP_CONFIG_REQ_LEN    4           /* Dest CID, flags (data is optional)   */
#define L2CAP_CONFIG_RSP_LEN    6           /* Dest CID, flags, result,data optional*/
#define L2CAP_DISC_REQ_LEN      4           /* Dest CID, source CID                 */
#define L2CAP_DISC_RSP_LEN      4           /* Dest CID, source CID                 */
#define L2CAP_ECHO_REQ_LEN      0           /* Data is optional                     */
#define L2CAP_ECHO_RSP_LEN      0           /* Data is optional                     */
#define L2CAP_INFO_REQ_LEN      2           /* Info type                            */
#define L2CAP_INFO_RSP_LEN      4           /* Info type, result (data is optional) */
#define L2CAP_BCST_OVERHEAD     2           /* Additional broadcast packet overhead */


/* Define the packet boundary flags
*/
#if (L2CAP_NON_FLUSHABLE_PB_INCLUDED == TRUE)
#define L2CAP_PKT_START_FLUSHABLE       2
#define L2CAP_PKT_START_NON_FLUSHABLE   0
#endif
#define L2CAP_PKT_START                 2
#define L2CAP_PKT_CONTINUE              1
#define L2CAP_MASK_FLAG            0x0FFF
#define L2CAP_PKT_TYPE_SHIFT            12
#define L2CAP_PKT_TYPE_MASK             3


/* Define the L2CAP connection result codes
*/
#define L2CAP_CONN_OK                0
#define L2CAP_CONN_PENDING           1
#define L2CAP_CONN_NO_PSM            2
#define L2CAP_CONN_SECURITY_BLOCK    3
#define L2CAP_CONN_NO_RESOURCES      4
#define L2CAP_CONN_TIMEOUT           0xEEEE

#define L2CAP_CONN_NO_LINK           255        /* Add a couple of our own for internal use */



/* Define the L2CAP command reject reason codes
*/
#define L2CAP_CMD_REJ_NOT_UNDERSTOOD    0
#define L2CAP_CMD_REJ_MTU_EXCEEDED      1
#define L2CAP_CMD_REJ_INVALID_CID       2


/* L2CAP Predefined CIDs
*/
#define L2CAP_SIGNALLING_CID            1
#define L2CAP_CONNECTIONLESS_CID        2
#define L2CAP_BASE_APPL_CID             0x0040


/* Define the L2CAP configuration result codes
*/
#define L2CAP_CFG_OK                    0
#define L2CAP_CFG_UNACCEPTABLE_PARAMS   1
#define L2CAP_CFG_FAILED_NO_REASON      2
#define L2CAP_CFG_UNKNOWN_OPTIONS       3


/* Define the L2CAP configuration option types
*/
#define L2CAP_CFG_TYPE_MTU              0x01
#define L2CAP_CFG_TYPE_FLUSH_TOUT       0x02
#define L2CAP_CFG_TYPE_QOS              0x03
#define L2CAP_CFG_TYPE_FCR              0x04

#define L2CAP_CFG_MTU_OPTION_LEN        2           /* MTU option length    */
#define L2CAP_CFG_FLUSH_OPTION_LEN      2           /* Flush option len     */
#define L2CAP_CFG_QOS_OPTION_LEN        22          /* QOS option length    */
#define L2CAP_CFG_FCR_OPTION_LEN        9           /* FCR option length    */
#define L2CAP_CFG_OPTION_OVERHEAD       2           /* Type and length      */


/* Default values for configuration
*/
#define L2CAP_NO_AUTOMATIC_FLUSH        0xFFFF
#define L2CAP_NO_RETRANSMISSION         0x0001

#define L2CAP_DEFAULT_MTU               672
#define L2CAP_DEFAULT_FLUSH_TO          L2CAP_NO_AUTOMATIC_FLUSH
#define L2CAP_DEFAULT_SERV_TYPE         1
#define L2CAP_DEFAULT_TOKEN_RATE        0
#define L2CAP_DEFAULT_BUCKET_SIZE       0
#define L2CAP_DEFAULT_PEAK_BANDWIDTH    0
#define L2CAP_DEFAULT_LATENCY           0xFFFFFFFF
#define L2CAP_DEFAULT_DELAY             0xFFFFFFFF


/* Define the L2CAP disconnect result codes
*/
#define L2CAP_DISC_OK                   0
#define L2CAP_DISC_TIMEOUT              0xEEEE


/* Define the info-type fields of information request & response 
*/
#define L2CAP_CONNLESS_MTU_INFO_TYPE      0x0001
#define L2CAP_EXTENDED_FEATURES_INFO_TYPE 0x0002 /* Used in Information Req/Response */

#define L2CAP_FCR_FC_SUPPORT     0x1
#define L2CAP_FCR_RTRANS_SUPPORT 0x2
#endif


