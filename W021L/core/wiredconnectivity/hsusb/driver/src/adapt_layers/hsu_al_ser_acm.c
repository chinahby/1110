/*==============================================================================

USB Serial I/F ACM (Modem) Adaptation Layer for High Speed USB

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions' over ACM
adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_acm_populate_acm_specific_data
hsu_al_ser_acm_ioctl
hsu_al_ser_acm_open
hsu_al_ser_acm_close

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_base_init must be called for function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_ser_acm.c#27 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
07/30/06  esh     First revision of file
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ser_acm.h"
#include "hsu_common.h"
#include "hsu_al_ser_base_i.h"

#include "jtypes.h"
#include "jerrno.h"

#include "hsu_al_ser_i.h"
#include "err.h"

#include "port.h"

#include "hsu_al_task.h"

/* NOTE: defined(FEATURE_HSU_TEST) && !defined(T_REXNT) 
means the code is used for testing the adaptation layer above UART */
#if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
#endif /* FEATURE_HSU_TEST && !T_REXNT */

#include "hsu_al_ser.h"

#ifdef FEATURE_DATA_S2_REG
#include "dsat.h" /* for dsat_get_sreg_val */
#endif /* FEATURE_DATA_S2_REG */

#include "hsu_core_config.h"
/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define AL_SER_ACM_DTR_ASSERTED_MASK (0x1)
  /* See CDC Spec. Section 6.2.14 */
#define AL_SER_ACM_RTS_ASSERTED_MASK (0x2)
  /* See CDC Spec. Section 6.2.14*/
#define AL_SER_ACM_CD_ASSERTED_MASK (0x1)
  /* See CDC Spec. Section 6.2.5 */
#define AL_SER_ACM_DSR_ASSERTED_MASK (0x2)
  /* See CDC Spec. Section 6.2.5 */
#define AL_SER_ACM_RI_ASSERTED_MASK (0x8)
  /* See CDC Spec. Section 6.2.5 */


/* For Raw Data Mode: */
#ifdef FEATURE_HSU_TEST
#error code not present
#else /* !FEATURE_HSU_TEST */
#define AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS 900
  /* Was previously 1000 msec; now it seems to be 900. */
#endif /* FEATURE_HSU_TEST */

#ifndef FEATURE_DATA_S2_REG
#define AL_SER_ACM_RAWDATA_ESC_SEQ_CHAR '+'
#else /* FEATURE_DATA_S2_REG */
#define AL_SER_ACM_ESC_SEQ_CHAR_REG 2
#define AL_SER_ACM_RAWDATA_ESC_SEQ_CHAR dsat_get_sreg_val(AL_SER_ACM_ESC_SEQ_CHAR_REG)
#endif /* !FEATURE_DATA_S2_REG */
#define AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH 3

#define AL_SER_ACM_SIO_IOCTL_SET_BUFFER_SIZE_DEFAULT 0
#define AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE          512

#define HSU_AL_SER_ACM_SUBCLASS         0xFF
#define HSU_AL_SER_ACM_PROTOCOL         0xFF

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/
/* placeholders for the stream_mode_specific_data_ptr field in 
modem_specific_data_struct*/
static rawdata_mode_data_struct rawdata_data;
/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
/*
The *_ignore_* functions are needed only because cannot pass NULL to 
mandatory ACM requests' callbacks:
*/
static jresult_t al_ser_acm_ignore_encap_command
(
  void* command, 
  juint16_t size, 
  cdc_appctx_t ctx
);

static void al_ser_acm_ignore_encap_response
(
  cdc_iostatus_t status, 
  juint32_t bytes_sent, 
  cdc_appctx_t ctx
);

/* this function is used for a special case:
   When a SIO_IOCTL_CHANGE_MODE is received, 
   and the mode is changed from SIO_DS_AUTODETECT_MODE  to a different mode,
   we need to flush the tx queue.
   The application does not expect that - therefor it does not supply a CB.
   The ACM itself supplies this decadent CB instead. */
static void al_ser_acm_flush_tx_from_ioctl_cb
(
  hsu_al_ser_base_data_for_ser_if* data_for_acm_if_ptr
);


static uint32 al_ser_acm_check_and_set_packet_size
(
  uint32 packet_size,
  uint32 max_buff_size_for_read
);

/* Line coding (see CDC Spec. chapter 6.2.12 & 6.2.13) */
static jresult_t al_ser_acm_set_line_coding_cb
(
  acm_line_coding_t* coding, 
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_get_line_coding_cb
(
  acm_line_coding_t* coding,
  cdc_appctx_t ctx
);

/*
Calls modem_specific_data.app_dtr_changed_cb_func only if the dtr bit in
modem_specific_data.ctrl_line_state is different than ctrl_sig_bitmap, 
received in callback...
*/
/* Set Control Line State (See CDC Spec. chapter 6.2.14) */
static jresult_t al_ser_acm_set_control_line_state_cb
(
  juint16_t ctrl_sig_bitmap, 
  cdc_appctx_t ctx
);

/* Serial State (see CDC Spec. chapter 6.2.5) */
static void al_ser_acm_notify_serial_state_cb
(
  cdc_iostatus_t status,
  cdc_appctx_t ctx
);

static void al_ser_acm_rx_done
(
  cdc_iostatus_t status,
  void* buffer_ptr,
  juint32_t size,
  jbool_t transfer_complete,
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_enabled
(
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_disabled
(
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_resumed
(
  cdc_appctx_t ctx
);

static jresult_t al_ser_acm_uninit
(
 cdc_appctx_t ctx
 );


/* TODO: Move this to shim layer??? */
static jresult_t al_ser_acm_notify_serial_state
(
  cdc_handle_t acm_fd_handle,
  juint16_t serial_state_bitmap
);

static jresult_t al_ser_acm_notify_serial_state_if_enabled
(
  hsu_al_ser_acm_data* acm_data_ptr
);

static boolean al_ser_acm_update_serial_state_param
(
  hsu_al_ser_acm_data* acm_data_ptr,
  juint16_t param_mask,
  boolean param_is_asserted
);

static void al_ser_acm_command_simulate_rx_done
(
  void* ctx_for_al_task
);

/* Support for Rawdata mode (circuit switched )*/
static void al_ser_acm_detect_esc_seq
(
  rawdata_mode_data_struct* rawdata_data_ptr,
  const char* buffer_ptr,
  juint32_t buffer_size
);

static void al_ser_acm_init_rawdata_mode_data
(
  rawdata_mode_data_struct* rawdata_data_ptr
);

static void al_ser_acm_rawdata_esc_seq_isr
(
  unsigned long rawdata_data_ptr_as_timer_data
);

static void al_ser_acm_unarm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
);

static void al_ser_acm_arm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
);


#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

/*===========================================================================
FUNCTION hsu_al_ser_acm_populate_acm_specific_data()
===========================================================================*/
void hsu_al_ser_acm_populate_acm_specific_data
(
  hsu_al_ser_acm_data* acm_data_ptr,
  cdc_handle_t handle,
  acm_callbacks_t* cbs,
  acm_init_info_t* info,
  cdc_appctx_t* ctx
)
{
  HSU_ASSERT(acm_data_ptr != NULL);
  HSU_ASSERT(handle != NULL);
  HSU_ASSERT(cbs != NULL);
  HSU_ASSERT(info != NULL);
  HSU_ASSERT(ctx != NULL);

  /* Save context for later callbacks */
  *ctx = (cdc_appctx_t)(acm_data_ptr);

  /* First, explicitly mark the callbacks which aren't used and can be NULL */
  /* Comm. Feature not used */
  cbs->clear_comm_feature_h = NULL;
  cbs->get_comm_feature_complete_h = NULL;
  cbs->get_comm_feature_h = NULL;
  cbs->set_comm_feature_h = NULL;
  /* No need for Get Line Coding Completion */
  cbs->get_line_coding_complete_h = NULL;
  /* Send Break and Network Connection not used */
  cbs->notify_net_conn_complete_h = NULL;
  cbs->send_break_h = NULL;

  /* The remaining callbacks are used */
  cbs->basic.read_complete_h = al_ser_acm_rx_done;
  cbs->basic.write_complete_h = hsu_al_ser_base_i_tx_done;
  cbs->basic.uninit_h = al_ser_acm_uninit;

  cbs->basic.enabled_h = al_ser_acm_enabled;
  cbs->basic.disabled_h = al_ser_acm_disabled;

  /* This AL will not be using encap commands and responses */
  cbs->basic.encap_command_h = al_ser_acm_ignore_encap_command;
  cbs->basic.encap_response_complete_h = al_ser_acm_ignore_encap_response;
  
  cbs->set_line_coding_h = al_ser_acm_set_line_coding_cb;
  cbs->get_line_coding_h = al_ser_acm_get_line_coding_cb;

  cbs->set_control_line_state_h = al_ser_acm_set_control_line_state_cb;

  cbs->notify_serial_state_complete_h = al_ser_acm_notify_serial_state_cb;


  cbs->basic.resumed_h = al_ser_acm_resumed;

  /* Init info struct */
  info->basic.flags = ((juint32_t)CDC_INIT_BUFFER_DMABLE | 
                       (juint32_t)CDC_INIT_SINGLE_INTERFACE |
                       (juint32_t)CDC_INIT_VENDOR_SPECIFIC_CLASS |
                       (juint32_t)CDC_INIT_NO_CLASS_DESC);

  /* Enable support for ZLP */
  info->basic.zlp_terminator = TRUE;

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

  /* In order for the MDLM descriptors to appear, 
  CDC_INIT_NO_CLASS_DESC must not be used. */
  info->basic.flags ^= (juint32_t)CDC_INIT_NO_CLASS_DESC;

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

  info->basic.cci_subclass = HSU_AL_SER_ACM_SUBCLASS;
  info->basic.cci_protocol = HSU_AL_SER_ACM_PROTOCOL;

  /* Similar to FS: Allow line coding, and call mgmt by itself, over data plane */
  info->capabilities = 
    (juint32_t)ACM_CAP_LINE_CODING | \
      ((juint32_t)ACM_CAP_CALL_MGMT | (juint32_t)ACM_CAP_CALL_MGMT_OVER_DC);

  /* Communication interface string descriptor */
  info->basic.comm_string = NULL;
  /* data interface string descriptor */
  info->basic.data_string = NULL;

}

/*===========================================================================
FUNCTION hsu_al_ser_acm_init()
===========================================================================*/
void hsu_al_ser_acm_init
(
  hsu_al_ser_acm_data* acm_data_ptr          /* Pointer to ACM Data struct */
)
{
  HSU_ASSERT(acm_data_ptr != NULL);

  /* Call base class' method: */
  hsu_al_ser_base_init(&(acm_data_ptr->base_data));

  /* "zero" line coding (see uw_device_cdc_acm.h for more details)
  The smallest values are used here. Whenever the host sends the SET_LINE_CODING 
  request, the following fields will be updated accordingly and will remain
  updated even after receiving the disabled callback. */
  acm_data_ptr->modem_if_data.line_coding.char_format = (juint8_t)ACM_LC_1_STOP_BITS;
  acm_data_ptr->modem_if_data.line_coding.data_bits = (juint8_t)ACM_LC_DATA_5_BITS;
  acm_data_ptr->modem_if_data.line_coding.dte_rate = 0;
  acm_data_ptr->modem_if_data.line_coding.parity_type = (juint8_t)ACM_LC_PARITY_NONE;
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_ioctl()
===========================================================================*/
boolean hsu_al_ser_acm_ioctl
(
  hsu_al_ser_acm_data* acm_data_ptr,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  boolean update_ser_state_param_is_successful;

  sio_stream_mode_type prev_stream_mode = SIO_MAX_MODE;
  dsm_watermark_type* prev_rx_wmq_ptr = NULL;
  dsm_watermark_type* prev_tx_wmq_ptr = NULL;

  /* TODO: Might need a new "magic" number just for ACM */
  HSU_ASSERT(acm_data_ptr != NULL);

  HSU_ASSERT(ioctl_param_ptr != NULL);

  /* First try the "base class": */
  if (hsu_al_ser_base_ioctl((hsu_al_ser_base_data_for_ser_if*)acm_data_ptr, 
    ioctl_cmd, ioctl_param_ptr) == TRUE)
  {
    /* Handled in "base" */
    return TRUE;
  }

  /* Try the "acm class" ioctl handler: */

  /* Handles the following I/O CTLs: 
  SIO_IOCTL_ENABLE_DTR_EVENT
  SIO_IOCTL_DISABLE_DTR_EVENT
  SIO_IOCTL_DTE_READY_ASSERTED
  SIO_IOCTL_GET_CURRENT_RTS

  SIO_IOCTL_CHANGE_MODE
  SIO_IOCTL_RECORD_ESC_FUNC_PTR - For Raw Data mode only
  SIO_IOCTL_ENABLE_AUTODETECT
  SIO_IOCTL_RECORD_PKT_FUNC_PTR - Not used anymore...

  SIO_IOCTL_CD_ASSERT
  SIO_IOCTL_CD_DEASSERT
  SIO_IOCTL_RI_ASSERT
  SIO_IOCTL_RI_DEASSERT
  SIO_IOCTL_DSR_ASSERT
  SIO_IOCTL_DSR_DEASSERT

  SIO_IOCTL_INBOUND_FLOW_ENABLE
  SIO_IOCTL_INBOUND_FLOW_DISABLE

  SIO_IOCTL_SET_BUFFER_SIZE
  SIO_IOCTL_GET_BUFFER_SIZE
  */

  switch (ioctl_cmd)
  {
    /* Control Line State Commands */
    case SIO_IOCTL_ENABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_ENABLE_DTR_EVENT", 0, 0, 0);

      acm_data_ptr->modem_if_data.app_dtr_changed_cb_func = 
        (sio_vv_func_ptr_type)((*ioctl_param_ptr).enable_dte_ready_event);

      #if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
      #endif /* FEATURE_HSU_TEST && !T_REXNT */

      break;
    }
    case SIO_IOCTL_DISABLE_DTR_EVENT:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DISABLE_DTR_EVENT", 0, 0, 0);
      #if defined(FEATURE_HSU_TEST) && !defined(T_REXNT)
#error code not present
      #endif /* FEATURE_HSU_TEST && !T_REXNT */

      acm_data_ptr->modem_if_data.app_dtr_changed_cb_func = NULL;
      break;
    }
    case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DTE_READY_ASSERTED", 0, 0, 0);
      if (
        (acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK) 
          != 0)
      {
        *(ioctl_param_ptr->dte_ready_asserted) = TRUE;
      }
      else
      {
        *(ioctl_param_ptr->dte_ready_asserted) = FALSE;
      }
      break;
    }
    case SIO_IOCTL_GET_CURRENT_RTS:
    {
      HSU_MSG_LOW("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_GET_CURRENT_RTS", 0, 0, 0);
      if (
        (acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_RTS_ASSERTED_MASK) 
        != 0)
      {
        *(ioctl_param_ptr->rts_asserted) = TRUE;
      }
      else
      {
        *(ioctl_param_ptr->rts_asserted) = FALSE;
      }
      break;
    }

    /* Autodetect/Rawdata/Packet mode commands */
    case SIO_IOCTL_CHANGE_MODE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CHANGE_MODE", 0, 0, 0);

      /* Save the WMQs previously used */
      prev_stream_mode = acm_data_ptr->base_data.stream_mode;
      prev_rx_wmq_ptr = acm_data_ptr->base_data.rx_wmq_ptr;
      prev_tx_wmq_ptr = acm_data_ptr->base_data.tx_wmq_ptr;

      HSU_ASSERT((*ioctl_param_ptr).open_ptr != NULL);

      /* Update the new stream mode: */
      acm_data_ptr->base_data.stream_mode = 
        (*ioctl_param_ptr).open_ptr->stream_mode;

      /* When changing from autodetect to non autodetect, need to verify the 
      previous WMQs are now empty*/
      if (prev_stream_mode == SIO_DS_AUTODETECT_MODE &&
        acm_data_ptr->base_data.stream_mode != SIO_DS_AUTODETECT_MODE)
      {
        if (prev_rx_wmq_ptr != NULL)
        {
          dsm_empty_queue(prev_rx_wmq_ptr);
        }
        if ((prev_tx_wmq_ptr != NULL) && 
          ((acm_data_ptr->base_data.remaining_chain_to_tx_ptr != NULL) ||
            (dsm_is_wm_empty(prev_tx_wmq_ptr)== FALSE)) )
        {
          HSU_MSG_HIGH("al_ser_acm_ioctl got CHANGE_MODE=>flushing tx queue", 0, 0, 0);
          acm_data_ptr->modem_if_data.flush_tx_ended = FALSE;
          hsu_al_ser_base_flush_tx((hsu_al_ser_base_data_for_ser_if*)acm_data_ptr, al_ser_acm_flush_tx_from_ioctl_cb);  
          /* while flush is on going - sleep(1). flush has a timeout of 4sec (in case of failure)*/
          while (acm_data_ptr->modem_if_data.flush_tx_ended == FALSE)
          {
            rex_sleep(1);
          }
        }
      }

     /* Update the rest of the new parameters: */
      acm_data_ptr->base_data.ioctl_flow_ctl_mirror.rx_flow = 
        (*ioctl_param_ptr).open_ptr->rx_flow;
      acm_data_ptr->base_data.ioctl_flow_ctl_mirror.tx_flow =
        (*ioctl_param_ptr).open_ptr->tx_flow;

      /* TODO: Is there a use case in which there is a need to guard against 
      FD Rx and Tx done callbacks, since they use the following fields? 
      It seems theoretical, since:
      This mode change is done when the app received an AT command from host (Rx).
      Therefore the host cannot send anything (Rx) until receiving an answer from
      the app, which is only when the app sends something (Tx).
      However, the app cannot send anything (Tx) until we exit this function.
      */
      /* Note: In ds3gsiolib, when switching from autodetect to another mode,
      the autodetect WMQs are emptied without any INTLOCK/TASKLOCK...*/
      acm_data_ptr->base_data.app_rx_cb_func = 
        (*ioctl_param_ptr).open_ptr->rx_func_ptr;
      acm_data_ptr->base_data.rx_wmq_ptr =
        (*ioctl_param_ptr).open_ptr->rx_queue;

      acm_data_ptr->base_data.tx_wmq_ptr =
        (*ioctl_param_ptr).open_ptr->tx_queue;

      /* When moving from Rawdata to another mode, make sure the clk isr won't 
      be called */
      if (prev_stream_mode == SIO_DS_RAWDATA_MODE &&
        acm_data_ptr->base_data.stream_mode != SIO_DS_RAWDATA_MODE)
      {
        al_ser_acm_unarm_esc_seq_clk(&rawdata_data);
      }

      /* When moving to Rawdata, make sure the clk and states are initialized */
      if (acm_data_ptr->base_data.stream_mode == SIO_DS_RAWDATA_MODE)
      {
        al_ser_acm_init_rawdata_mode_data(&rawdata_data);

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */

        acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr = &rawdata_data;

        al_ser_acm_arm_esc_seq_clk(
          acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr);
      }

      break;
    }
    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_RECORD_ESC_FUNC_PTR", 
        0, 0, 0);
      if (acm_data_ptr->base_data.stream_mode != SIO_DS_RAWDATA_MODE)
      {
        HSU_MSG_ERROR("hsu_al_ser_acm_ioctl: Requested SIO_IOCTL_RECORD_ESC_FUNC_PTR\
                      (%u), but stream_mode(%u) != SIO_DS_RAWDATA_MODE (%u)", 
                      SIO_IOCTL_RECORD_ESC_FUNC_PTR, 
                      acm_data_ptr->base_data.stream_mode, SIO_DS_RAWDATA_MODE);
        return FALSE;
      }
      /* else (acm_data_ptr->base_data.stream_mode == SIO_DS_RAWDATA_MODE)*/
      rawdata_data.rawdata_mode_esc_sequence_cb_func = 
        (*ioctl_param_ptr).record_escape_func_ptr;
      break;
    }
    case SIO_IOCTL_ENABLE_AUTODETECT:
    {
      HSU_MSG_HIGH("*_acm_ioctl: cmd %u (ENABLE_AUTODETECT) nothing to do", 
        ioctl_cmd, 0, 0);
      break;
    }

    /* Serial state commands */
    case SIO_IOCTL_CD_ASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CD_ASSERT", 0, 0, 0);
      /* Check CD status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
          AL_SER_ACM_CD_ASSERTED_MASK, TRUE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_CD_DEASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_CD_DEASSERT", 0, 0, 0);
      /* Check CD status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_CD_ASSERTED_MASK, FALSE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_RI_ASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_RI_ASSERT", 0, 0, 0);
      /* Check RI status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_RI_ASSERTED_MASK, TRUE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_RI_DEASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_RI_DEASSERT", 0, 0, 0);
      /* Check RI status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_RI_ASSERTED_MASK, FALSE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_DSR_ASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DSR_ASSERT", 0, 0, 0);
      /* Check DSR status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_DSR_ASSERTED_MASK, TRUE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }
    case SIO_IOCTL_DSR_DEASSERT:
    {
      HSU_MSG_MED("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_DSR_DEASSERT", 0, 0, 0);
      /* Check DSR status, only update and send notification if changed */
      update_ser_state_param_is_successful = 
        al_ser_acm_update_serial_state_param(acm_data_ptr, 
        AL_SER_ACM_DSR_ASSERTED_MASK, FALSE);
      HSU_ASSERT(update_ser_state_param_is_successful);
      break;
    }

    /* In-bound flow commands */
    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_INBOUND_FLOW_ENABLE", 
        0, 0, 0);
      if (acm_data_ptr->modem_if_data.inbound_flow_enabled == FALSE)
      {
        acm_data_ptr->modem_if_data.inbound_flow_enabled = TRUE;

        if (acm_data_ptr->modem_if_data.enqueued_simulated_rx_done_when_inbound_enabled)
        {
          break;
        }

        if (acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled != NULL)
        {
          acm_data_ptr->modem_if_data.enqueued_simulated_rx_done_when_inbound_enabled = 
            TRUE;

          /* The AL task will take it from here (since inbound flow may be called 
          when INTLOCK/TASKLOCK, and DMOV API requires usage of crit section).
          This also is more appropriate, since AL task grabs the giant mutex,
          and calls Rx done callback.*/
          hsu_al_task_enqueue_cmd(al_ser_acm_command_simulate_rx_done, acm_data_ptr);
        }
      }
      break;
    }
    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: handling I/O Ctl: SIO_IOCTL_INBOUND_FLOW_DISABLE", 
        0, 0, 0);
      acm_data_ptr->modem_if_data.inbound_flow_enabled = FALSE;
      /* In the next Rx done callback, this will be taken into account */
      break;
    }
    case SIO_IOCTL_SET_BUFFER_SIZE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: SIO_IOCTL_SET_BUFFER_SIZE - Nothing to do", 
        0, 0, 0);
      acm_data_ptr->modem_if_data.sio_ioctl_set_buffer_size_mirror = 
        ioctl_param_ptr->buffer_size;
      break;
    }
    case SIO_IOCTL_GET_BUFFER_SIZE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: SIO_IOCTL_GET_BUFFER_SIZE - Nothing to do", 
        0, 0, 0);
      ioctl_param_ptr->buffer_size =
        acm_data_ptr->modem_if_data.sio_ioctl_set_buffer_size_mirror;
      break;
    }
    case SIO_IOCTL_SET_PACKET_SIZE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: handling SIO_IOCTL_SET_PACKET_SIZE. Setting to %d", 
        ioctl_param_ptr->packet_size, 0, 0);
      acm_data_ptr->base_data.buff_size_for_read = 
        al_ser_acm_check_and_set_packet_size(ioctl_param_ptr->packet_size,
        acm_data_ptr->base_data.max_buff_size_for_read);
      break;
    }
    case SIO_IOCTL_GET_PACKET_SIZE:
    {
      HSU_MSG_HIGH("*_acm_ioctl: handling SIO_IOCTL_GET_PACKET_SIZE", 
        0, 0, 0);
      ioctl_param_ptr->packet_size =
        acm_data_ptr->base_data.buff_size_for_read;
      break;
    }
    default:
    {
      HSU_MSG_ERROR("hsu_al_ser_acm_ioctl: cmd %u not supported", ioctl_cmd, 0, 0);
      return FALSE;
    }
  }

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_open()
===========================================================================*/
boolean hsu_al_ser_acm_open
(
  hsu_al_ser_acm_data* acm_data_ptr,
  const sio_open_type* open_ptr,
  dsm_mempool_id_enum_type pool_id,
  byte* memory_pool_for_rx_ptr,
  byte* memory_pool_for_tx_ptr,
  juint32_t buff_size_for_read,
  juint32_t max_buff_size_for_write,
  rex_timer_cnt_type tx_aggr_delay_in_msecs
)
{
  boolean open_result;
  /* TODO: A different ACM magic number to verify... */
  HSU_ASSERT(acm_data_ptr != NULL);

  HSU_MSG_HIGH("*_acm_open: entered", 0, 0, 0);

  /* Invoke base class' open: */
  open_result = hsu_al_ser_base_open(&(acm_data_ptr->base_data), open_ptr, pool_id, 
    memory_pool_for_rx_ptr, memory_pool_for_tx_ptr, buff_size_for_read, 
    max_buff_size_for_write,buff_size_for_read, tx_aggr_delay_in_msecs);
  if (open_result == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ser_acm_open: hsu_al_ser_base_open failed", 0, 0, 0);
    return FALSE;
  }

  /* Update mdm specific fields: */
  acm_data_ptr->modem_if_data.app_dtr_changed_cb_func = NULL;

  acm_data_ptr->base_data.stream_mode = open_ptr->stream_mode;
  if (acm_data_ptr->base_data.stream_mode == SIO_DS_RAWDATA_MODE)
  {
      acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr = &rawdata_data;
      
      al_ser_acm_init_rawdata_mode_data(&rawdata_data);

  }
  else
  {
    acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr = NULL;
  }

  acm_data_ptr->modem_if_data.inbound_flow_enabled = TRUE;
  acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled = NULL;
  acm_data_ptr->modem_if_data.saved_buffer_data_size = 0;
  acm_data_ptr->modem_if_data.enqueued_simulated_rx_done_when_inbound_enabled = FALSE;

  acm_data_ptr->modem_if_data.sio_ioctl_set_buffer_size_mirror = 
    AL_SER_ACM_SIO_IOCTL_SET_BUFFER_SIZE_DEFAULT;

  acm_data_ptr->modem_if_data.flush_tx_ended = FALSE; /* default init value*/

  HSU_MSG_HIGH("*_acm_open: exited", 0, 0, 0);

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_acm_close()
===========================================================================*/
void hsu_al_ser_acm_close
(
  hsu_al_ser_acm_data* acm_data_ptr,
  hsu_al_ser_base_op_complete_cb close_cb,
  boolean abort_io_operations_immediately
)
{
  /* TODO: A different ACM magic number to verify... */
  HSU_ASSERT(acm_data_ptr != NULL);

  HSU_MSG_HIGH("*_acm_close: entered", 0, 0, 0);

  /* In case flow ctl is still disabled, release the allocated buffer: */
  if (acm_data_ptr->modem_if_data.inbound_flow_enabled == FALSE)
  {
    if (acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled != NULL)
    {
      hsu_al_ser_i_free_buffer_from_read_write(
        acm_data_ptr->base_data.memory_pool_for_rx, 
          acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled);
      acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled = NULL;
      acm_data_ptr->modem_if_data.enqueued_simulated_rx_done_when_inbound_enabled = 
        FALSE;
    }
  }

  /* Invoke base class' close: */
  hsu_al_ser_base_close(&(acm_data_ptr->base_data), close_cb, 
    abort_io_operations_immediately);

  HSU_MSG_HIGH("*_acm_close: exit, abort_immediate=%u", 
    abort_io_operations_immediately, 0, 0);
}

/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================

FUNCTION al_ser_acm_check_and_set_packet_size

DESCRIPTION
Called whenever SIO_IOCTL_SET_PACKET_SIZE ioctl is issued. This function 
verifies that the requested packet is of the right size: 
between AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE and TYPICAL_BUFF_SIZE_FOR_MDM_READ
and that it's a multiplication of AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE.

DEPENDENCIES
None

RETURN VALUE
Corrected packet size

SIDE EFFECTS
None

===========================================================================*/
static uint32 al_ser_acm_check_and_set_packet_size
(
  uint32 packet_size,
  uint32 max_buff_size_for_read
)
{
  uint32 corrected_size = packet_size;

  if (packet_size < AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE)
  {
    corrected_size = AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE;
    return corrected_size;
  }
  if (packet_size > max_buff_size_for_read)
  {
    corrected_size = max_buff_size_for_read;
    return corrected_size;
  }

  if (packet_size % AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE > 0)
  {
    corrected_size = packet_size - 
      (packet_size % AL_SER_ACM_SIO_IOCTL_MIN_PACKET_SIZE);
    return corrected_size;
  }
  return corrected_size;
}

/*===========================================================================

FUNCTION al_ser_acm_ignore_encap_command

DESCRIPTION
Called whenever the ACM FD receives a SEND_ENCAPUSLATED_COMMAND from the host
(CDC Spec. chapter 6.2.1).
This request is not supported and is not expected to be received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_ignore_encap_command
(
  void* command,        /* Pointer to data representing the encap. command */
  juint16_t size,       /* Length, in bytes, of the received command       */
  cdc_appctx_t ctx      /* Context provided to the ACM FD, in acm_init     */
)
{
  HSU_USE_PARAM(command);
  HSU_USE_PARAM(size);
  HSU_USE_PARAM(ctx);
  HSU_MSG_ERROR(
      "*_ignore_encap_command(): encapsulated commands not supported in ACM",0,0,0);
  return JENOTSUP;
}

/*===========================================================================

FUNCTION al_ser_acm_ignore_encap_response

DESCRIPTION
Called whenever the ACM FD receives a GET_ENCAPUSLATED_RESPONSE from the host
(CDC Spec. chapter 6.2.2).
This request is not supported and is not expected to be received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_ignore_encap_response
(
  cdc_iostatus_t status, /* indication from host if the response was 
                         received */
  juint32_t bytes_sent,  /* number of bytes actually sent */
  cdc_appctx_t ctx       /* Context provided to the ACM FD, in acm_init    */
)
{
  HSU_USE_PARAM(status);
  HSU_USE_PARAM(bytes_sent);
  HSU_USE_PARAM(ctx);

  HSU_ASSERT((uint32)"encapsulated commands not supported in ACM" == NULL);
}

/*===========================================================================
FUNCTION al_ser_acm_flush_tx_from_ioctl_cb

DESCRIPTION
this function is used for a special case: 
when a SIO_IOCTL_CHANGE_MODE cmd is received, 
and the mode is changed from SIO_DS_AUTODETECT_MODE  to a different mode,
we need to flush the tx queue.
The application does not expect that - therefor it does not supply a CB.
The ACM itself supplies this decadent CB instead. 

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None
===========================================================================*/
static void al_ser_acm_flush_tx_from_ioctl_cb
(
 hsu_al_ser_base_data_for_ser_if* data_for_acm_if_ptr
)
{
  ((hsu_al_ser_acm_data*)data_for_acm_if_ptr)->modem_if_data.flush_tx_ended = TRUE;
  HSU_MSG_LOW("*_flush_tx_from_ioctl_cb: ended",0,0,0);
}
/*===========================================================================

FUNCTION al_ser_acm_set_line_coding_cb

DESCRIPTION
Called whenever the ACM FD receives a SET_LINE_CODING from the host
(CDC Spec. chapter 6.2.12). It will simply save the received data, so that
when a GET_LINE_CODING occurs, it will return the data it has just stored.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_set_line_coding_cb
(
  acm_line_coding_t* coding, /* Pointer to struct holding the line coding 
                             values, as was given by the host */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  /* TODO: Give a different "MAGIC NUM" for ACM and for OBEX ?*/
  /* Theoretically, ctx here may be hsu_al_ser_base_data_for_ser_if* and 
  not hsu_al_ser_acm_data* !!!*/
  /* Perhaps add an ACM extra data magic number...*/
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;

  HSU_PARAM_COULD_BE_CONST(coding);

  HSU_ASSERT(coding != NULL);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  acm_data_ptr->modem_if_data.line_coding = *coding;

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_get_line_coding_cb

DESCRIPTION
Called whenever the ACM FD receives a GET_LINE_CODING from the host
(CDC Spec. chapter 6.2.13). It will simply provide the ACM FD the line coding 
data it had previously stored.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_get_line_coding_cb
(
  acm_line_coding_t* coding, /* Pointer to struct holding the line coding 
                             values, as will be given to the host */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;

  HSU_ASSERT(coding != NULL);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* TODO: Make sure acm_data_ptr->modem_if_data.line_coding is packed !!!*/
  *coding = acm_data_ptr->modem_if_data.line_coding;

  return HSU_JSUCCESS;  
}

/*===========================================================================

FUNCTION al_ser_acm_send_recent_serial_state

DESCRIPTION
Sends the last serial state notification if it is different from the last one the host received. 
Called from resumed and when we get DTE ready from host to resolve cases where the 
messages was not sent to the host (we got suspended or the application
on the host was not open yet).


DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS for succees

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_send_recent_serial_state(hsu_al_ser_acm_data* acm_data_ptr)
{
  jresult_t notify_serial_state_result;

  if ((acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host != 0) &&
    (acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host != 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host))
  {
    notify_serial_state_result = 
      al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);

    if (notify_serial_state_result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("_send_recent_serial_state: Failed to send serial state =%d, notify_serial_state_result", acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host, notify_serial_state_result, 0);
    }
    return notify_serial_state_result;
  }
  return HSU_JSUCCESS;
}
/*===========================================================================

FUNCTION al_ser_acm_set_control_line_state_cb

DESCRIPTION
Called whenever the ACM FD receives a SET_CONTROL_LINE_STATE from the host
(CDC Spec. chapter 6.2.14). It will simply check if the DTR (Data Terminal Ready)
is different than what was previously saved. If so, will update the new DTR and 
invoke the modem's DTR flag changed callback.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_set_control_line_state_cb
(
  juint16_t ctrl_sig_bitmap, /* Bitmap representing the control line's 
                             state */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;
  juint16_t orig_ctrl_line_state;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Save original ctrl_line_state and update new ctrl line state prior
  to invoking client callback (since if client DTR callback immediately checks 
  new ctrl line state, it will already be updated)*/
  orig_ctrl_line_state = acm_data_ptr->modem_if_data.ctrl_line_state;
  acm_data_ptr->modem_if_data.ctrl_line_state = ctrl_sig_bitmap;

  /* In case we get DTE ready we need to resend the serial state notification, 
  for cases where we tried to send it but the application on the host side
  was not ready yet */
  if ((acm_data_ptr->modem_if_data.ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK) 
    != 0)
  {
    (void)al_ser_acm_send_recent_serial_state(acm_data_ptr);
  }

  /* Check if the DTR bit is different between the given ctrl_sig_bitmap and
  the current ctrl_line_state: */
  if ((orig_ctrl_line_state & AL_SER_ACM_DTR_ASSERTED_MASK) !=
    (ctrl_sig_bitmap & AL_SER_ACM_DTR_ASSERTED_MASK))
  {
    if (acm_data_ptr->modem_if_data.app_dtr_changed_cb_func != NULL)
    {
      acm_data_ptr->modem_if_data.app_dtr_changed_cb_func();
    }
  }

  return HSU_JSUCCESS;
}


/*===========================================================================

FUNCTION al_ser_acm_notify_serial_state_cb

DESCRIPTION
Called whenever the ACM FD notifies completion of a previous call to 
acm_notify_serial_state.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_notify_serial_state_cb
(
  cdc_iostatus_t status,                               /* Operation status */
  cdc_appctx_t ctx           /* Context provided to the ACM FD, in acm_init*/
)
{
  jresult_t notify_serial_state_result;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  HSU_ASSERT(status != CDC_TRANSFER_ERROR);

  if (status == CDC_TRANSFER_CANCELLED)
  {
    HSU_MSG_HIGH("*_notify_serial_state_cb: canceled", 0, 0 ,0);

    /* We can get the CDC_TRANSFER_CANCELLED in case the application on the host was not ready, 
       or if we requested to send the serial state and there is a suspend and in case of 
       cable disconnect (disable). 
       Therefore, we keep acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host
       and we will send it when we get DTE ready, when we resume and in enable.*/
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host = 0;
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host = 0;
    return;
  }
  /* else: status == CDC_TRANSFER_OK */

  /* Host acks it received last_serial_state_bitmap_sent_to_host, update accordingly: */
  acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host = 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host;

  HSU_MSG_MED("*_notify_serial_state_cb: host acked updated ser. state: %u", 
    acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host, 0, 0);

  /* Check if there is a need to send a more updated serial state to host: */
  if (acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host
    != acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host)
  {
    /* Since this callback is called from DSR context, it means
    that the FD is still enabled, therefore it is guaranteed that a call will be
    made to acm_notify_serial_state. The reason 
    al_ser_acm_notify_serial_state_if_enabled is used here is to avoid code
    duplication: */
    HSU_ASSERT(acm_data_ptr->base_data.instance_enabled_by_cdc_fd == TRUE);

    notify_serial_state_result = 
      al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);

    if (notify_serial_state_result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("_notify_serial_state_cb: Failed to send serial state =%d, notify_serial_state_result", acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host, notify_serial_state_result, 0);
    }
    else
    {
      HSU_MSG_HIGH("*_notify_serial_state_cb: sent a more updated ser. state", 0, 0, 0);
    }
  }
}

/*===========================================================================

FUNCTION al_ser_acm_rx_done

DESCRIPTION
Rx Callback whenever the ACM FD wishes to notify a buffer has been received
over the data class I/F. See also hsu_al_ser_base_i_rx_done.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_rx_done
(
  cdc_iostatus_t status,                               /* Operation status */
  void* buffer_ptr,                                  /* Ptr to buffer used */
  juint32_t size,                                /* buffer size (in bytes) */
  jbool_t transfer_complete,                  /* is this transfer complete */
  cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;

  rawdata_mode_data_struct* rawdata_data_ptr = NULL;

  HSU_ASSERT(buffer_ptr != NULL);

  HSU_MSG_LOW("al_ser_acm_rx_done: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* Moved this line from al_ser_acm_command_simulate_rx_done.
  From this point, it is safe to enqueue another simulate_rx_done command. 
  Since any change in inbound flow will result in either saving the buffer now,
  or on the next call to acm_rx_done. */
  acm_data_ptr->modem_if_data.enqueued_simulated_rx_done_when_inbound_enabled = 
    FALSE;

  /* If received a zero length packet - size == 0 - (e.g. transfer was exactly a 
  multiple of max_packet_size AND it filled the entire buffer), 
  simply cause a new cdc_read. 
  */
  if ((status == CDC_TRANSFER_OK) && (size != 0))
  {
    /* It no longer matters if transfer was completed or not, as the next cdc_read
    will pick up what was left. */

    /* Check if in-bound flow is disabled: */
    /* This may occur due to one of the following:
    1. Allocation of DSM items exceeding limits. The only place this can occur is 
    via call to hsu_al_dsm_utils_generate_dsm_chain, which is following this check.
    However, if this is checked prior to hsu_al_dsm_utils_generate_dsm_chain, 
    it will guarantee no further DSM items will be allocated.

    2. via "external event" from layers above this AL (app context). It is assumed 
    that this case does not need to be handled immediately so it is ok to wait 
    until this point (for example: it is possible one of the layers issues such 
    an event, but before the IOCTL is issued, is then interrupted by DMOV callback 
    which forwards the data to the app, even though it requested not to receive 
    anymore).
    */
    if (acm_data_ptr->modem_if_data.inbound_flow_enabled == FALSE)
    {
      HSU_ASSERT(
        acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled == NULL);
      /* Save buffer, when enabled will request to scatter it, followed by cdc_read: */
      acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled = 
        buffer_ptr;
      acm_data_ptr->modem_if_data.saved_buffer_data_size = size;

      return ;
    }
    /* else acm_data_ptr->modem_if_data.inbound_flow_enabled == TRUE */

    /* Before calling the "base class" Rx done, need to process the given data.
    For now, this only needs to be done for rawdata mode */
    if (acm_data_ptr->base_data.stream_mode == SIO_DS_RAWDATA_MODE)
    {
      rawdata_data_ptr = 
        (rawdata_mode_data_struct*)(acm_data_ptr->modem_if_data.stream_mode_specific_data_ptr);

      al_ser_acm_detect_esc_seq(rawdata_data_ptr, (char*)buffer_ptr, size);
    }
  }

  /* If transfer is not valid, or received zero length packet - allow base class 
  to handle it.
  If data is legal and in-bound flow is enabled, proceed as normal, by letting 
  base class handle it: */
  hsu_al_ser_base_i_rx_done(status, buffer_ptr, size, transfer_complete, ctx);
}

/*===========================================================================

FUNCTION al_ser_acm_enabled

DESCRIPTION
Callback called whenever the ACM FD wishes to notify the instance is enabled.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Success. Otherwise, see hsu_al_ser_base_i_enabled.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_enabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  jresult_t base_class_call_result, notify_serial_state_result;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;

  HSU_MSG_LOW("al_ser_acm_enabled: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

#ifndef T_REXNT
  /* Set the FD handle to the current configuration's FD */
  /* Assume ACM is supported for primary core only. Otherwise, core_index 
     should be added to ACM data structure */
  hsu_al_ser_handle_set_configuration(HSU_CORE_IDX_PRIMARY);
#endif /* T_REXNT */

  /* Call base class: */
  base_class_call_result = hsu_al_ser_base_i_enabled(ctx,
                           acm_data_ptr->modem_if_data.inbound_flow_enabled);

  if (base_class_call_result != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR(
      "al_ser_acm_enabled: hsu_al_ser_base_i_enabled failed with result: %d", 
        base_class_call_result, 0, 0);
    return base_class_call_result;
  }

  /* send an updated serial_state: 
  */

  /* Since this callback means the FD is enabled, therefore it is guaranteed 
  that a call will be made to acm_notify_serial_state. The reason 
  al_ser_acm_notify_serial_state_if_enabled is used here is to avoid code
  duplication: */
  notify_serial_state_result = 
    al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);

  HSU_ASSERT(notify_serial_state_result == HSU_JSUCCESS);

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_disabled

DESCRIPTION
Callback called whenever the ACM FD wishes to notify the instance is disabled.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_disabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  jresult_t base_i_disabled_result;
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;

  HSU_MSG_LOW("al_ser_acm_disabled: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* TODO: Verify this is in fact an ACM data ptr and not just a "base class" */

  /* Call base class: shouldn't fail */
  base_i_disabled_result = hsu_al_ser_base_i_disabled(ctx);
  HSU_ASSERT(base_i_disabled_result == HSU_JSUCCESS);

  /* Call DTE Ready Callback, but do so via set ctrl line state, to simulate
  as if a terminal was connected and it issued a normal disconnection. */
  (void)al_ser_acm_set_control_line_state_cb(0, ctx);

  return HSU_JSUCCESS;
}

/*===========================================================================

FUNCTION al_ser_acm_resumed

DESCRIPTION
Callback called whenever the ACM FD wishes to notify resumed occurred.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_resumed
(
  cdc_appctx_t ctx                           /* Context passed in callback */  
)
{
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr = 
    (hsu_al_ser_base_data_for_ser_if*)ctx;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ser_if_data_ptr;

  HSU_MSG_LOW("al_ser_acm_resumed: Checking if ctx (0x%x) valid...", ctx, 0, 0);
  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(ser_if_data_ptr) == TRUE);

  /* In case we are resumed we need to resend the serial state notification, 
  for cases where we requested to send it but then got suspended */  
  (void)al_ser_acm_send_recent_serial_state(acm_data_ptr);

  if ((acm_data_ptr->modem_if_data.inbound_flow_enabled == FALSE) ||
    (acm_data_ptr->modem_if_data.enqueued_simulated_rx_done_when_inbound_enabled) ||
      (acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled != NULL))
  {
    /* Either inbound flow disabled, 
    or it is in the process of simulating an Rx done callback (right after 
    inbound flow is enabled and there is a pending buffer). In both cases, 
    do not issue a cdc_read. */
    return HSU_JSUCCESS;  
  }
  /* else: inbound_flow_enabled == TRUE && 
  enqueued_simulated_rx_done_when_inbound_enabled == FALSE &&
  saved_buffer_when_inbound_flow_disabled == NULL
  This means that both flow is enabled as well as no pending saved buffer for when
  inbound flow is re-enabled.
  */
  return hsu_al_ser_base_i_resumed(ctx);
}

/*===========================================================================

FUNCTION AL_SER_ACM_UNINIT

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Success. Otherwise, see hsu_al_ser_base_i_uninit.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_uninit
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 )
{
  hsu_al_ser_handle_acm_uninit();
  return hsu_al_ser_base_i_uninit(ctx);
}

/*===========================================================================

FUNCTION al_ser_acm_notify_serial_state

DESCRIPTION
Serves as a wrapper function for acm_notify_serial_state, by grabbing the
"Giant Mutex", invoking acm_notify_serial_state then releasing the "Giant Mutex".

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Operation Successful. Otherwise - Failure. 
See acm_notify_serial_state for more details.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_notify_serial_state
(
  cdc_handle_t acm_fd_handle,
  juint16_t serial_state_bitmap
)
{
  jresult_t notify_serial_state_result;

  /* Grab giant mutex */
  jsafe_enter();

  notify_serial_state_result = acm_notify_serial_state(acm_fd_handle, 
    serial_state_bitmap);

  /* Release giant mutex */
  jsafe_leave();

  return notify_serial_state_result;
}

/*===========================================================================

FUNCTION al_ser_acm_notify_serial_state_if_enabled

DESCRIPTION
Sends serial state notification to host, but only if the ACM FD is in an enabled 
state (thus avoiding an error of JENXIO).

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Operation Successful. Otherwise - Failure. 
See al_ser_acm_notify_serial_state for more details.

SIDE EFFECTS
None

===========================================================================*/
static jresult_t al_ser_acm_notify_serial_state_if_enabled
(
  hsu_al_ser_acm_data* acm_data_ptr    /* Pointer to ACM data struct */
)
{
  jresult_t notify_serial_state_result;

  HSU_ASSERT(hsu_al_ser_base_i_ctx_is_valid(
    (hsu_al_ser_base_data_for_ser_if*)acm_data_ptr) == TRUE);

  /* Grab giant mutex */
  jsafe_enter();

  /* Here it is safe to check whether instance enabled or not */
  if (acm_data_ptr->base_data.instance_enabled_by_cdc_fd == FALSE)
  {
    /* Release giant mutex */
    jsafe_leave();

    /* If instance is disabled, there is no point in sending notif */
    HSU_MSG_HIGH("al_ser_acm_notify_serial_state_if_enabled: instance 0x%x disabled,\
                 host will not receive notification. Returning success", 
                 acm_data_ptr->base_data.instance_of_fd_used, 0, 0);
    return HSU_JSUCCESS;
  }

  HSU_MSG_MED("*_notify_serial_state_if_enabled: sending bitmap: %u", 
    acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host, 0, 0);

  notify_serial_state_result = al_ser_acm_notify_serial_state(
    acm_data_ptr->base_data.instance_of_fd_used, 
    acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host);

  /* Update the serial state which has just been sent, as the last one ever 
  sent to host */
  acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host = 
    acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host;

  /* Release giant mutex */
  jsafe_leave();

  return notify_serial_state_result;
}

/*===========================================================================

FUNCTION al_ser_acm_update_serial_state_param

DESCRIPTION
Sends serial state notification to host.

DEPENDENCIES
param_mask must contain only one bit turned on ('1').

RETURN VALUE
TRUE - Operation Successful. Otherwise - Failure. 
See al_ser_acm_notify_serial_state_if_enabled for more details.

SIDE EFFECTS
None

===========================================================================*/
static boolean al_ser_acm_update_serial_state_param
(
  hsu_al_ser_acm_data* acm_data_ptr,         /* Pointer to ACM data struct */
  juint16_t param_mask,                      /* Param mask which indicates
                                             the parameter (CD, RI, or DSR)
                                             to update */
  boolean param_is_asserted                  /* Whether to assert (1) the param 
                                             or deassert (0)*/
)
{
  boolean need_to_notify_serial_state = FALSE;
  jresult_t notify_serial_state_result;
  /* TODO: Might need a new "magic" number just for ACM */
  HSU_ASSERT(acm_data_ptr != NULL);

  /* The giant mutex is used here to prevent the following two scenarios: 
  Scenario I:
  1. The FD is in disabled (i.e. upon startup or upon cable disconnection)
  2. The app issues an IOCTL which causes invoking al_ser_acm_update_serial_state_param, 
  as a result:
  (serial_state_bitmap_to_send_to_host = new value,
  last_serial_state_bitmap_sent_to_host == last_serial_state_bitmap_rxed_by_host 
  != new value), but notification is not sent (since FD disabled)
  3. enabled callback invoked, but since serial_state_bitmap_to_send_to_host != 
  last_serial_state_bitmap_rxed_by_host a notification is sent via call to
  al_ser_acm_notify_serial_state. When this call returns a context switch occurs.
  This happens prior to updating last_serial_state_bitmap_sent_to_host to be
  serial_state_bitmap_to_send_to_host.
  4. The app now issues yet another IOCTL, but since 
  last_serial_state_bitmap_sent_to_host is not updated, the following if statement 
  is not taken (but should have been). 
  5. The app is blocked by giant mutex, but when enabled cb is done, it resumes 
  execution, but since the if was not taken, it wrongly calls 
  al_ser_acm_notify_serial_state_if_enabled,
  causing an assert (since the FD is busy handling the previous request and has not yet 
  invoked its completion callback).


  Scenario II:
  1. The FD is in disabled (i.e. upon startup or upon cable disconnection)
  2. The app issues an IOCTL which causes invoking al_ser_acm_update_serial_state_param, 
  as a result:
  (serial_state_bitmap_to_send_to_host = new value,
  last_serial_state_bitmap_sent_to_host == last_serial_state_bitmap_rxed_by_host 
  != new value), but before it calls al_ser_acm_notify_serial_state_if_enabled
  a context switch occurs, and the FD's enabled callback is invoked.
  3. Since last_serial_state_bitmap_rxed_by_host != serial_state_bitmap_to_send_to_host
  al_ser_acm_notify_serial_state_if_enabled is called.
  4. The app resumes execution, which means it calls 
  al_ser_acm_notify_serial_state_if_enabled as well, causing an assertion for the 
  same reason.
  */

  /* Grab giant mutex */
  jsafe_enter();


  /* Assuming param_mask has only one bit as '1' (the rest are '0')*/

  if (param_is_asserted == TRUE)
  {
    if (
      (acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host & param_mask)
      == 0)
    {
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host |= param_mask;
      need_to_notify_serial_state = TRUE;
    }

  }
  else /* param_is_asserted == FALSE */
  {
    if (
      (acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host & param_mask) 
      != 0)
    {
      acm_data_ptr->modem_if_data.serial_state_bitmap_to_send_to_host &= (~param_mask);
      need_to_notify_serial_state = TRUE;
    }
  }

  if (need_to_notify_serial_state == FALSE)
  {
    /* Release giant mutex */
    jsafe_leave();

    HSU_MSG_MED("*_update_serial_state_param: Nothing to update", 0, 0, 0);
    return TRUE;
  }

  /* Check if there was a previous call to acm_notify_serial_state, which was 
  not answered by a corresponding callback. If so, do not call acm_notify_serial_state */
  if (acm_data_ptr->modem_if_data.last_serial_state_bitmap_rxed_by_host
    != acm_data_ptr->modem_if_data.last_serial_state_bitmap_sent_to_host)
  {
    /* Release giant mutex */
    jsafe_leave();

    HSU_MSG_MED("*_update_serial_state_param: a prev. notif ser. state not acked yet"
      , 0, 0, 0);
    HSU_MSG_MED("*_update_serial_state_param: update will be made once notif cb invoked"
      , 0, 0, 0);
    return TRUE;
  }

  notify_serial_state_result = 
    al_ser_acm_notify_serial_state_if_enabled(acm_data_ptr);
  if (notify_serial_state_result != HSU_JSUCCESS)
  {
    /* Release giant mutex */
    jsafe_leave();

    /* ESUSPENDED might happen here, in case of the following scenario: 
    1. Device with Mdm function was enumerated, thus the ACM FD is enabled, but the 
    USB is now disconnected. Only upon reconnecting will we get a disabled followed 
    by enabled callback.
    2. Mdm app sends a SIO_IOCTL_CD_DEASSERT, receiving priority over HSU (so 
    stack DPC which wants to call disable and then re-enable did not get the chance).
    3. We call update serial state, since the last ever txed serial_state == last ever 
    acked by host so we proceed to call acm_notify_serial_state which returns ESUSPENDED
    */
    if (notify_serial_state_result == ESUSPENDED)
    {
      /* Similar to handling cdc_write/cdc_read, nothing is done. Upon receiving the
      enabled callback, the updated serial state will be sent */
      return TRUE;
    }
    /* else: notify_serial_state_result != ESUSPENDED */

    ERR("al_ser_acm_update_serial_state_param: al_ser_acm_notify_serial_state_if_enabled\
        returned failure %u", notify_serial_state_result, 0, 0);
    HSU_ASSERT((uint32)"fail to notify serial state!" == NULL);
  }

  /* Release giant mutex */
  jsafe_leave();

  return TRUE;
}

/*===========================================================================

FUNCTION al_ser_acm_command_simulate_rx_done

DESCRIPTION
A command for the AL task, to simulate an Rx done callback for an ACM instance.
This is done in case the application did the following:
1. disabled inbound flow (saving the buffer which was received afterwards),
2. enabled inbound flow and now invokes the AL task to simulate an Rx done, with
the saved buffer.
See hsu_al_ser_acm_ioctl / SIO_IOCTL_INBOUND_FLOW_ENABLE for more information.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_command_simulate_rx_done
(
  void* ctx_for_al_task
)
{
  void* buffer_ptr = NULL;
  juint32_t size = 0;
  hsu_al_ser_acm_data* acm_data_ptr = 
    (hsu_al_ser_acm_data*)ctx_for_al_task;

  /* TODO: Might need a new "magic" number just for ACM */
  HSU_ASSERT(acm_data_ptr != NULL);

  buffer_ptr = 
    acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled;
  size = acm_data_ptr->modem_if_data.saved_buffer_data_size;

  HSU_ASSERT(size != 0);
  acm_data_ptr->modem_if_data.saved_buffer_when_inbound_flow_disabled = NULL;

  HSU_MSG_MED("*_command_simulate_rx_done: going to call Rx done cb. buff=0x%x, size=%u", 
    buffer_ptr, size, 0);

  hsu_al_ser_base_i_rx_done(CDC_TRANSFER_OK, buffer_ptr, size, TRUE, 
    (cdc_appctx_t)acm_data_ptr);  
}

/*===========================================================================

FUNCTION al_ser_acm_detect_esc_seq

DESCRIPTION
Detects if so far, a partial or full esc. sequence in raw data mode has been 
detected.

A legal esc. sequence should be:

At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character, e.g. '+',
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_detect_esc_seq
(
  rawdata_mode_data_struct* rawdata_data_ptr,
  const char* buffer_ptr,
  juint32_t buffer_size
)
{
  juint32_t esc_seq_detection_itor;

  HSU_ASSERT(rawdata_data_ptr != NULL);
  HSU_ASSERT(buffer_ptr != NULL);
  HSU_ASSERT(buffer_size != 0);

  /* Make sure clk won't interrupt in the middle */
  al_ser_acm_unarm_esc_seq_clk(rawdata_data_ptr);

  if (rawdata_data_ptr->should_wait_for_clk == TRUE)
  {
    /* Received data while waiting for inactivity, reset the clock */
    al_ser_acm_arm_esc_seq_clk(rawdata_data_ptr);
    return;
  }
  /* else: rawdata_data_ptr->should_wait_for_clk == FALSE */

  /* Verify that received buffer contains a partial (or full) legal esc. seq.: */
  if (rawdata_data_ptr->num_of_esc_seq_characters_seen + buffer_size <= 
    AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH)
  {
    for (esc_seq_detection_itor = 0 ;  esc_seq_detection_itor < buffer_size ; 
      ++esc_seq_detection_itor)
    {
      if (buffer_ptr[esc_seq_detection_itor] != AL_SER_ACM_RAWDATA_ESC_SEQ_CHAR)
      {
        /* Character not valid for esc sequence, need to reset */
        rawdata_data_ptr->should_wait_for_clk = TRUE;
        rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;
        break;
      }
    }
    if (esc_seq_detection_itor == buffer_size)
    {
      /* A partial or full ESC Seq detected, update the additional num of 
      characters seen: */
      /* To overcome VC++ compiler warning C4244*/
      /* It is okay to cast buffer_size to uint8, as the above "if" already verified
      that the sum of rawdata_data_ptr->num_of_esc_seq_characters_seen and buffer_size
      is not greater than AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH */
      rawdata_data_ptr->num_of_esc_seq_characters_seen = 
        rawdata_data_ptr->num_of_esc_seq_characters_seen + (uint8)buffer_size;
    }
  }
  else /* rawdata_data_ptr->num_of_esc_seq_characters_seen + buffer_size >
    AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH */
  {
    /* Too many characters received, need to reset */
    rawdata_data_ptr->should_wait_for_clk = TRUE;
    rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;
  }

  /* rearm clock */
  al_ser_acm_arm_esc_seq_clk(rawdata_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_acm_init_rawdata_mode_data

DESCRIPTION
Initializes the data structs used for raw data mode.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_init_rawdata_mode_data
(
  rawdata_mode_data_struct* rawdata_data_ptr
)
{
  HSU_ASSERT(rawdata_data_ptr != NULL);

  rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;
  rawdata_data_ptr->should_wait_for_clk = TRUE;
  rawdata_data_ptr->rawdata_mode_esc_sequence_cb_func = NULL;

  rex_def_timer_ex(&(rawdata_data.rawdata_mode_esc_seq_timer_cb_item), 
    al_ser_acm_rawdata_esc_seq_isr, (unsigned long)rawdata_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_acm_rawdata_esc_seq_isr

DESCRIPTION
ISR invoked when rex timer used for esc. sequence detection has expired.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_acm_rawdata_esc_seq_isr
(
  unsigned long rawdata_data_ptr_as_timer_data
)
{
  rawdata_mode_data_struct* rawdata_data_ptr = NULL;

  HSU_ASSERT((void*)(rawdata_data_ptr_as_timer_data) != NULL);

  rawdata_data_ptr = 
    (rawdata_mode_data_struct*)rawdata_data_ptr_as_timer_data;

  if (rawdata_data_ptr->should_wait_for_clk == TRUE)
  {
    /* The period of inactivity prior to esc seq has passed, and no data has been 
    received (otherwise rx callback would reset clock) so it is okay to proceed */
    rawdata_data_ptr->should_wait_for_clk = FALSE;

    return;
  }
  /* else: rawdata_data_ptr->should_wait_for_clk == FALSE
  This means whatever has accumulated so far has to be a full esc. seq.,
  anything else resets back to inactivity period. */

  /* Check if a full and exact esc seq. exists */
  if (rawdata_data_ptr->num_of_esc_seq_characters_seen == 
    AL_SER_ACM_RAWDATA_ESC_SEQ_LENGTH)
  {
    /* Esc. Seq. detected, invoke callback */
    if (rawdata_data_ptr->rawdata_mode_esc_sequence_cb_func != NULL)
    {
      rawdata_data_ptr->rawdata_mode_esc_sequence_cb_func();
      /* TODO: Re-arm the clk? This is done also in other SIO modules. */
    }
  }

  /* Restore state (now waiting for inactivity timeout) */
  rawdata_data_ptr->should_wait_for_clk = TRUE;
  rawdata_data_ptr->num_of_esc_seq_characters_seen = 0;

  /* Rearm the clk. If mode has changed from raw_data, then it already has cancelled
  this clk. */
  al_ser_acm_arm_esc_seq_clk(rawdata_data_ptr);
}

/*===========================================================================

FUNCTION al_ser_acm_unarm_esc_seq_clk

DESCRIPTION
Cancels the timer used for esc. seq. detection in rawdata mode.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Cancels the rex timer

===========================================================================*/
static void al_ser_acm_unarm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
)
{
  HSU_ASSERT(rawdata_data_ptr != NULL);

  (void)rex_clr_timer(&(rawdata_data_ptr->rawdata_mode_esc_seq_timer_cb_item));
    /* This will cause the registered timer to seize its operation*/
}

/*===========================================================================

FUNCTION al_ser_acm_arm_esc_seq_clk

DESCRIPTION
Arms a timer to expire when AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS
msecs have passed. This is used in the detecting a legal esc. sequence when
in rawdata mode.

A legal esc. sequence should be:

At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character, e.g. '+',
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
No more than AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence,
Escape Character,
At least AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs of silence

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
Arms a rex timer for AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS msecs.

===========================================================================*/
static void al_ser_acm_arm_esc_seq_clk
(
  rawdata_mode_data_struct* rawdata_data_ptr
)
{
  HSU_ASSERT(rawdata_data_ptr != NULL);

  (void)rex_set_timer(&(rawdata_data_ptr->rawdata_mode_esc_seq_timer_cb_item), 
    AL_SER_ACM_RAWDATA_ESC_SEQ_IDLE_PERIOD_IN_MSECS);
}

#ifdef FEATURE_HSU_TEST
#error code not present
#endif /* FEATURE_HSU_TEST */
