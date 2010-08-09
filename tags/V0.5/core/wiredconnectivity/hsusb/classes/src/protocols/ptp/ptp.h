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
#ifndef __PTP_H__
#define __PTP_H__

#include "ptp_responder_api.h"
#include "ptp_transport_api.h"
#include "ptp_hook.h"
#include "ptp_data_packing.h"

typedef void * ptp_vs_ext_ctx_t;
typedef void * ptp_vs_ext_cbs_t;

/* residue used for buffer manipulation where ptp vs extensions need to pack
 * data without the needs to keep for them selves the residues.
 * size is at most the length of ptp string for non array ptp type */
#define PTP_MAX_RESIDUE         PTP_STR_PACKED_SIZE(PTP_MAX_STRING_LEN)

/*
 *   PTP session
 */
typedef struct
{
    juint32_t buff_size;
    juint32_t data_len;
    juint32_t data_offset;
    juint8_t buff[PTP_MAX_BLOCK_SIZE];
    juint8_t residue_buf[PTP_MAX_RESIDUE];
} ptp_buffer_t;

typedef struct ptp_session
{
    /* The following fields MUST always be first and in the same order */
    ptp_appctx_t        appctx;         /* session specific app ctx */
    ptp_appctx_t        hookctx;        /* session specific hook ctx */
    
    ptp_block_t         operation;
    ptp_block_t         response;

    enum
    {
        PTP_STATE_COMMAND, PTP_STATE_READ_DATA,
        PTP_STATE_SEND_DATA, PTP_STATE_DATA_DONE, PTP_STATE_RESPONSE
    } state;

    juint32_t            transaction_cancelled : 1;  /* Initiator signalled */
                                                    /* a 'cancel event' */
    juint32_t            power_down : 1;
    juint32_t            device_reset : 1;
    juint32_t            flags : 27;          /* Unused flags */

    /* Transfer buffers */
    juint32_t            app_done : 1;        /* belongs to flags above */
    juint32_t            transport_done : 1;  /* belongs to flags above */
    ptp_buffer_t        data[2];
    jint_t               app_buf_idx;

    /* Active transfer tracking */
    juint32_t            end_offset;
    juint32_t            start_offset;
    juint32_t            curr_app_offset;
    juint32_t            curr_tr_offset;
    
    /* Send object data */
    juint32_t            obj_size;
    
    /* PTP session info */
    juint32_t            session_id;
    juint32_t            transaction_id;
    struct ptp_instance *inst;

    /* Vendor Specific extension session context */
    ptp_vs_ext_ctx_t    vs_ext_ctx;
#define PTP_VS_EXT_CTX(session) session->vs_ext_ctx

    struct ptp_session  *next;
} ptp_session_t;

/*
 * Vendor Specific extension definitions
 */
typedef jresult_t (*ptp_vs_dispatch_cb)(ptp_session_t *session);
typedef jresult_t (*ptp_vs_open_session_cb)(ptp_session_t *session);
typedef jresult_t (*ptp_vs_close_session_cb)(ptp_session_t *session);
typedef jresult_t (*ptp_vs_delete_cbs_cb)(ptp_vs_ext_cbs_t *cbs);

/* Structure that describes extension functionality
 * And is used to register an extension with PTP */
typedef struct
{
    juint32_t                    vendor_ext_id;
    juint16_t                    vendor_ext_ver;
    wchar_t                     *vendor_ext_desc;
    ptp_vs_dispatch_cb          dispatch_h;
    ptp_vs_open_session_cb      open_session_h;
    ptp_vs_close_session_cb     close_session_h;
    ptp_vs_delete_cbs_cb        del_cbs_h;
} ptp_vs_ext_params_t;

/*
 *   PTP instance
 */
typedef struct ptp_instance
{
    /* Active state */
    juint32_t            active_sessions;
    ptp_session_t       *sessions;
    ptp_session_t       null_session; /* This holds session info for handling
                                         out-of-session operations */
    /* Application completion handlers */
    ptp_responder_comp_cbs comp_cbs;

    /* Application args */
    ptp_responder_cbs   cbs;
    ptp_appctx_t        global_appctx;  /* app global ctx */

    /* Hook args */
    ptp_appctx_t        global_hookctx; /* hook global ctx */
    
    /* Vendor Specific args */
    ptp_vs_ext_cbs_t    vs_ext_cbs;
    ptp_vs_ext_params_t vs_ext_params;
#define PTP_VS_EXT_CBS(session) session->inst->vs_ext_cbs

    /* Transport args */
    ptp_transport_ops   ops;
    ptp_transport_ctx_t tr_ctx;
    juint32_t            transport_block_size;
    juint32_t            transport_hdr_size;
    juint32_t            max_sessions;
} ptp_instance_t;

/*
 *   Services provided by the PTP layer
 */

jresult_t ptp_register_vs_ext(ptp_instanceh_t insth, ptp_vs_ext_params_t *ext,
    ptp_vs_ext_cbs_t ext_cbs);

jresult_t send_response(ptp_session_t *session, juint32_t num_params,
    jbool_t in_data_phase);

jresult_t send_event(ptp_session_t *session, ptp_io_block_t *event,
    juint32_t num_params);

jresult_t start_send_data(ptp_session_t *session, juint32_t total_size,
        juint32_t block_size);

jresult_t start_read_data(ptp_session_t *session);

jresult_t schedule_advance(ptp_session_t *session);

jbool_t verify_command_supported(ptp_session_t *session,
    void *pfn, jbool_t in_data_phase);

#endif /*__PTP_H__*/
