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

#ifndef _PTP_TRANSPORT_API_H_
#define _PTP_TRANSPORT_API_H_

#include "ptp_responder_api.h"

/* The device info dataset is the largest structure in use and can be at most
 * 3165 bytes long (packed) */
#define PTP_MAX_BLOCK_SIZE      4096

typedef void * ptp_ctx_t;
typedef void * ptp_transport_ctx_t;

/* ====== IO (transport layer) data types ====== */

#define PTP_BLOCK_NUM_PARAMS    5

typedef struct
{
    juint16_t code;
    juint32_t session_id;
    juint32_t transaction_id;
    juint32_t parameter[PTP_BLOCK_NUM_PARAMS];
} ptp_block_t;

typedef __PACKED_PRE__ struct
{
    juint16_t code;
    juint32_t session_id;
    juint32_t transaction_id;
    juint32_t parameter[PTP_BLOCK_NUM_PARAMS];
} __PACKED_POST__ ptp_io_block_t;

/* PTP callbacks */
jresult_t ptp_io_read_operation_complete(ptp_instanceh_t inst,
    ptp_io_block_t *operation);

jresult_t ptp_io_read_data_complete(ptp_ctx_t ctx, juint32_t size,
    juint32_t total, juint32_t data_offset, jbool_t last);

jresult_t ptp_io_send_data_complete(ptp_ctx_t ctx, juint32_t size);

jresult_t ptp_io_send_response_complete(ptp_ctx_t ctx);

jresult_t ptp_io_send_event_complete(ptp_ctx_t ctx);

jresult_t ptp_io_initiator_event(ptp_instanceh_t insth,
    ptp_io_block_t *event);

/* Transport functions */
typedef jresult_t (*ptp_io_read_operation_fn)(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ctx, juint32_t session_id);

typedef jresult_t (*ptp_io_read_data_fn)(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ctx, void *data, juint32_t size);

typedef jresult_t (*ptp_io_send_data_fn)(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ctx, void *data, juint32_t size, juint32_t total, jbool_t last);

typedef jresult_t (*ptp_io_send_response_fn)(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ctx, ptp_io_block_t *response, juint32_t num_params,
    jbool_t data_phase_present);

typedef jresult_t (*ptp_io_send_event_fn)(ptp_transport_ctx_t tr_ctx,
    ptp_ctx_t ctx, ptp_io_block_t *event, juint32_t num_params);

/* ====== PTP initialization ====== */

typedef struct
{
    /* Transport functions (called by PTP) */
    ptp_io_read_operation_fn    read_operation_h;
    ptp_io_read_data_fn         read_data_h;
    ptp_io_send_data_fn         send_data_h;
    ptp_io_send_response_fn     send_response_h;
    ptp_io_send_event_fn        send_event_h;
} ptp_transport_ops;

typedef struct
{
    juint32_t transport_datahdr_size;    /* facilitates encapsulation */
    juint32_t transport_block_size;      /* 0 = stream transport */
    juint32_t max_sessions;
} ptp_init_args;

jresult_t ptp_create_instance(ptp_instanceh_t *insth, ptp_transport_ops *ops,
    ptp_transport_ctx_t tr_ctx, ptp_init_args *args);
jresult_t ptp_delete_instance(ptp_instanceh_t insth, jbool_t graceful);

jresult_t ptp_start_responder(ptp_instanceh_t insth);
jresult_t ptp_stop_responder(ptp_instanceh_t insth, jbool_t graceful);

#endif /*_PTP_TRANSPORT_API_H_*/
