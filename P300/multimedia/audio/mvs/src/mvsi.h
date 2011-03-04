#ifndef MVSI_H
#define MVSI_H

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                      I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions internal to MVS.

REFERENCES
  None.

Copyright (c) 2002 - 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvsi.h#1 $ $DateTime: 2008/05/19 03:35:58 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
xx/xx/xx    sm     Added SILENCE generation.
04/28/05   ymc     Added MVSF_WCDMA_AMR_CRC_FRAME_WINDOW support.
02/21/05   ymc     Added MVS_STATE_STANDBY.
           ymc     Updated Copyright.
01/28/05   ymc     Added support for MVS_MODE_LINEAR_PCM.
10/13/04   ymc     Added MVS_CMD_TX and MVS_CMD_RX.
           ymc     Updated copyright.
           ymc     Added context information in MVS enable command structure.
08/18/04   ymc     Added support for MVS_CMD_STANDBY command.
04/24/03   ymc     Added MVS_STATE_CHANGING.
           ymc     Updated copyright.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "queue.h"
#include "mvs.h"
#include "msmaud.h"

/* <EJECT> */
#ifdef FEATURE_MVS

/*===========================================================================

              I N T E R N A L  F E A T U R E   D E F I N I T I O N S

===========================================================================*/
/* This feature will measure WCDMA AMR CRC error rate and if the rate is too
   high for a moment, the first good AMR frame will be treated as untrustible
   bad frame.  This is for filtering subtle noise in weak radio link condition
   due to insensible packets which are able to bypass CRC check in L1.
*/
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
#define MVSF_WCDMA_AMR_CRC_FRAME_WINDOW
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* This signal is set when an item is put on the mvs_cmd_q.
*/
#define VOC_MVS_CMD_Q_SIG               0x0100

/* MVS Command Queue Available Packets.
*/
#define MVS_NUM_PACKETS 6                /* # of Free Packets Allocated     */

typedef enum {
  MVS_CMD_ACQUIRE,
  MVS_CMD_ENABLE,
  MVS_CMD_STANDBY,
  MVS_CMD_LISTEN,
  MVS_CMD_RELEASE,
  MVS_CMD_MODE,
  MVS_CMD_UPDATE,
  MVS_CMD_TX,
  MVS_CMD_RX,
  MVS_CMD_INVALID
} mvs_cmd_type;


/* Header for MVS commands
*/
typedef struct
{
  q_link_type         link;            /* Command queue link               */
  mvs_cmd_type        cmd;             /* MVS command to perform           */

} mvs_hdr_type;

/* Base packet type for MVS commands that use standard CB function and have
** no parameters.
*/
typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
} mvs_base_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
} mvs_acquire_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
  mvs_mode_type          mode;
  mvs_packet_ul_fn_type  ul_func;
  mvs_packet_dl_fn_type  dl_func;
  mvs_pkt_context_type   context;
} mvs_enable_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
  mvs_mode_type          mode;
} mvs_standby_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
  mvs_listen_fn_type     ul_func;
  mvs_listen_fn_type     dl_func;
} mvs_listen_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
} mvs_release_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_mode_type          mode;
} mvs_mode_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
  mvs_frame_q_type       *ul_qptr;     /* uplink voice frame buffer */
  mvs_packet_ul_fn_type  ul_func;      /* uplink callback function */
} mvs_tx_packet_type;

typedef struct
{
  mvs_hdr_type           hdr;          /* Command Header            */
  mvs_event_cb_type      cb_func;      /* Callback function pointer */
  
  mvs_client_type        client;
  mvs_frame_q_type       *dl_qptr;     /* downlink voice frame buffer */
  mvs_packet_dl_fn_type  dl_func;      /* downlink callback function */
} mvs_rx_packet_type;

typedef union {
  mvs_hdr_type                 hdr;
  mvs_base_packet_type         base;             /* Base MVS packet type */
  mvs_acquire_packet_type      acquire;
  mvs_enable_packet_type       enable;
  mvs_standby_packet_type      standby;
  mvs_listen_packet_type       listen;
  mvs_release_packet_type      release;
  mvs_mode_packet_type         mode;
  mvs_tx_packet_type           tx;
  mvs_rx_packet_type           rx;
} mvs_packet_type;


typedef enum {
  MVS_STATE_DISABLE,
  MVS_STATE_STANDBY,
  MVS_STATE_STANDBY_SILENCE,  /* Standby, but silent */
  MVS_STATE_CHANGING,
  MVS_STATE_ENABLE,
  MVS_STATE_SILENCE,          /* Enabled, but silent */
  MVS_STATE_LISTEN            /* unused for now */
} mvs_state_enum_type;

typedef struct {
  mvs_state_enum_type    state;
  mvs_event_cb_type      event_cb;

  mvs_mode_type          desired_config_mode;
  mvs_packet_ul_fn_type  desired_ul_cb;
  mvs_packet_dl_fn_type  desired_dl_cb;
  mvs_pkt_context_type   desired_context;

  mvs_mode_type          config_mode;
  mvs_packet_ul_fn_type  ul_cb;
  mvs_packet_dl_fn_type  dl_cb;
  mvs_pkt_context_type   context;

#ifdef FEATURE_VOC_PCM_INTERFACE
  mvs_state_enum_type    pcm_state;

  mvs_packet_ul_fn_type  desired_pcm_ul_cb;
  mvs_packet_dl_fn_type  desired_pcm_dl_cb;
  mvs_pkt_context_type   desired_pcm_context;

  mvs_packet_ul_fn_type  pcm_ul_cb;
  mvs_packet_dl_fn_type  pcm_dl_cb;
  mvs_pkt_context_type   pcm_context;
#endif  /* FEATURE_VOC_PCM_INTERFACE */

  mvs_event_cb_type      listen_event_cb;
  mvs_listen_fn_type     listen_ul_cb;
  mvs_listen_fn_type     listen_dl_cb;
} mvs_client_state_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_init

DESCRIPTION
  This procedure initializes the MVS command queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_cmd_q
  mvs_free_q
  mvs_free_packet

===========================================================================*/
extern void mvs_init ( void );

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_cmd

DESCRIPTION
  Queue a command for processing by the MVS Services.

DEPENDENCIES
  mvs_init() needs to have initialized the queues.

RETURN VALUE
  None

SIDE EFFECTS
  mvs_cmd_q
  vs_tcb

===========================================================================*/
extern void mvs_cmd(mvs_packet_type *mvs_cmd_ptr);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_cmd_get_pkt

DESCRIPTION
  Get a MVS Command Packet.  Use by callers of mvs_cmd().

DEPENDENCIES
  FEATURE_VS_TASK is defined.
  mvs_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available mvs_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  mvs_free_q (and the packet fetched off mvs_free_q)

===========================================================================*/
extern mvs_packet_type *mvs_cmd_get_pkt (void);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_process_cmd

DESCRIPTION
  This procedure processes requests received by mvs_cmd().  See mvsi.h for
  a description of the actual MVS packets.

DEPENDENCIES
  mvs_init() must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  mvs_cmd_q

===========================================================================*/
extern void mvs_process_cmd (void);

#ifdef FEATURE_MVS_GEN_SILENCE
/*===========================================================================

FUNCTION
  mvs_reset_silence_frame

DESCRIPTION
  Will reset the current silence frame to a default value for the current mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_reset_silence_frame(mvs_mode_type mode);

#endif /* FEATURE_MVS_GEN_SILENCE */

#endif  /* FEATURE_MVS */
#endif  /* MVSI_H */

