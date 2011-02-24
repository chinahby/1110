#ifndef MVS_H
#define MVS_H

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This header file defines function interfaces and corresponding data 
structures for invoking multi-mode voice services.

REFERENCES
  None.

Copyright (c) 2002 - 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/audio/6k/common/mvs/main/latest/src/mvs.h#1 $ $DateTime: 2008/05/19 03:35:58 $ $Author: vparasar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/26/07   kan     Added AMR-WB changes 
07/05/05    sm     Added G723.1 support.
04/28/05   ymc     Added MVS_CLIENT_VOIP.
           ymc     Added support for MVS_MODE_4GV_NB and MVS_MODE_4GV_WB.
02/20/05    sm     Added support for MVS_MODE_G711.
           ymc     Updated Copyright.
01/28/05   ymc     Added support for MVS_MODE_LINEAR_PCM. Removed MSMAUD deps
11/17/04   ymc     Removed support for MSMAUD_VOC_WVRC and MSMAUD_VOC_SMV.
10/13/04   ymc     Added interface for packet exchange in different context.
           ymc     Moved mvs_pkt_status_type to mvssup.h.
           ymc     Added mvs_mode_supported.
08/18/04   ymc     Added support for MVS_CMD_STANDBY command.
01/27/04   ymc     Updated Copyright.
           ymc     Added MVS_CLIENT_QVP_TEST.
06/30/03   ymc     Added MVS_CLIENT_QVP.
04/17/03    sm     Refeaturized MVS code.
12/12/02   ymc     Added GSM half rate support.
10/17/02   ymc     Added MVS_CLIENT_TEST.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_MVS

#include "mvssup.h"

/* <EJECT> */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Definitions of MVS client type */
typedef enum {
  MVS_CLIENT_CDMA,     
  MVS_CLIENT_WCDMA,   
  MVS_CLIENT_GSM,      
  MVS_CLIENT_VOIP,
  MVS_CLIENT_QCHAT,  
  MVS_CLIENT_VMEMO,   
  MVS_CLIENT_QVP,
  MVS_CLIENT_QVP_TEST,
  MVS_CLIENT_TEST,
  MVS_CLIENT_MAX,
  MVS_CLIENT_NONE = MVS_CLIENT_MAX
} mvs_client_type;




/* Definitions of context in which MVS packet exchange takes place */
typedef enum {
  MVS_PKT_CONTEXT_NONE,   /* no packet exchange requested */
  MVS_PKT_CONTEXT_ISR,    /* packets are exchanged in ISR context */
  MVS_PKT_CONTEXT_VOC,    /* packets are exchanged in VOC task context */
  MVS_PKT_CONTEXT_MAX
} mvs_pkt_context_type;

/* Definitions of MVS loopback type */
typedef enum {
  MVS_LB_NONE,

  MVS_LB_UL2DL_VOC,
  MVS_LB_UL2DL_DSM,
  MVS_LB_UL2DL_PCM,

  MVS_LB_DL2UL_VOC,
  MVS_LB_DL2UL_PCM,

  MVS_LB_MAX
} mvs_loopback_type;


/* ==========================================================================
** Callback data structure definitions
** ==========================================================================
*/

/* Event type definitions */
typedef enum {
  MVS_EV_COMMAND,   /* command status */
  MVS_EV_MODE       /* mode status    */
} mvs_event_enum_type;

typedef struct {
  mvs_event_enum_type event;
  mvs_client_type client;
} mvs_ev_header_type;

/* Command event definitions */
typedef enum {
  MVS_CMD_FAILURE,
  MVS_CMD_BUSY,
  MVS_CMD_SUCCESS
} mvs_cmd_status_type;

typedef struct {
  mvs_ev_header_type  hdr;
  mvs_cmd_status_type cmd_status;
} mvs_ev_command_type;

/* Mode event definitions */
typedef enum {
  MVS_MODE_NOT_AVAIL,
  MVS_MODE_INIT,
  MVS_MODE_READY
} mvs_mode_status_type;

typedef struct {
  mvs_ev_header_type   hdr;
  mvs_mode_status_type mode_status;
  mvs_mode_type        mode;
} mvs_ev_mode_type;

/* Generic event data type definitions */
typedef union {
  mvs_ev_header_type  hdr;
  mvs_ev_command_type cmd;
  mvs_ev_mode_type    mode;
} mvs_event_type;


/* ==========================================================================
** Callback function type definitions
** ==========================================================================
*/

/* Event callback function type */
typedef void (*mvs_event_cb_type)(mvs_event_type *event);

/* Uplink packet transfer callback function type */
typedef void (*mvs_packet_ul_fn_type)(uint8               *vocoder_packet,
                                      mvs_frame_info_type *frame_info,
                                      uint16               packet_length,
                                      mvs_pkt_status_type *status);

/* Downlink packet transfer callback function type */
typedef void (*mvs_packet_dl_fn_type)(uint8                *vocoder_packet,
                                      mvs_frame_info_type  *frame_info,
                                      mvs_pkt_status_type  *status);

/* Packet listening callback function type */
typedef void (*mvs_listen_fn_type)(uint8               *vocoder_packet,
                                   mvs_frame_info_type *frame_info);

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_mode_supported

DESCRIPTION
  This function determines if a requested MVS mode is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : supported.   
  FALSE : not.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mvs_mode_supported(
  mvs_mode_type mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_set_loopback

DESCRIPTION
  This function will set up the MVS loopback mode:

  MVS_LB_NONE: no loopback

  MVS_LB_UL2DL_VOC: route uplink voice packets to downlink; bypass DSM queues
  MVS_LB_UL2DL_DSM: route uplink voice packets to downlink through DSM queues
  MVS_LB_UL2DL_PCM: route uplink PCM samples into downlink

  MVS_LB_DL2UL_VOC: route downlink voice packets back to uplink
  MVS_LB_DL2UL_PCM: route downlink PCM samples into uplink

  NOTE: if DSM queues are not utilized, MVS_LB_UL2DL_DSM functions the same as
        MVS_LB_UL2DL_VOC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_set_loopback(mvs_loopback_type lb_mode);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_acquire

DESCRIPTION
  This function allows a client to acquire MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_acquire(
  mvs_client_type client, 
  mvs_event_cb_type cb_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_enable

DESCRIPTION
  This function enables an MVS client to use the vocoder to process audio 
packets of a given mode.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_enable(
  mvs_client_type client, 
  mvs_mode_type mode, 
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_standby

DESCRIPTION
  This function will cause vocoder to download the requested image but stay
in IDLE mode until mvs_enable is invoked.

DEPENDENCIES
  mvs_acquire() has to be called first.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_standby(
  mvs_client_type client,
  mvs_mode_type mode
);


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_listen

DESCRIPTION
  This function enables an MVS client to listen to the packets sent to and from
the vocoder.  It is typically used for in-call recording.

DEPENDENCIES
  A client that has called mvs_acquire() cannot also call mvs_listen().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_listen(
  mvs_client_type client,
  mvs_event_cb_type cb_func,
  mvs_listen_fn_type ul_func,
  mvs_listen_fn_type dl_func
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_release

DESCRIPTION
  This function disables an MVS client, shutting down the vocoder.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvs_release(mvs_client_type client);

/* <EJECT> */

#endif  /* FEATURE_MVS */
#endif  /* MVS_H */

