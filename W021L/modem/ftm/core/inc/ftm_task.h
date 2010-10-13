#ifndef FTM_TASK_H
#define FTM_TASK_H
/*===========================================================================

           F T M    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the FTM Services module.

REFERENCES
  None

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_task.h#1 $
  $DateTime: 2008/04/14 04:57:43 $ 
  $Author: ppottier $

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
06/16/05   dp      Add support for FTM EGPRS BER feature.
03/15/05   dp      Change FTM WCDMA/GSM BER signal definitions.
01/28/05   jfc     Added no_clear versions to ftm_sig_rex_wait(_cb)
11/12/04   ra      Changed msg mask from MSG_SSID_DFLT to MSG_SSID_FTM.
10/07/04   wd      Removed redundant functions ftm_gsm_wait and ftm_wait.
                   Added ftm_sig_rex_wait_cb function which can be passed
                   to drivers that will need to wait while executing on ftm 
                   task context.
01/31/03   xw      created
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "queue.h"
#include "prot.h"
#include "rfi.h"
#include "ftmicap.h"
#include "msg.h"
/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
#if defined( FTM_HAS_1X)
extern rf_chain_state_type rf_chain_status[];
#endif 

typedef enum
{
  FTM_MSG_LVL_FATAL,
  FTM_MSG_LVL_ERROR,
  FTM_MSG_LVL_HIGH,
  FTM_MSG_LVL_MED,
  FTM_MSG_LVL_LOW,
  FTM_MSG_LVL_MAX
} ftm_msg_lvl_type;


/* FTM signals */
#define FTM_CMD_Q_SIG          0x0001    /* A command is available in the queue. */
#define FTM_RPT_TIMER_SIG      0x0002    /* Used to signal a watchdog report.    */ 
#define FTM_HS_CMD_SIG         0x0004    /* This signal is used by the test      */
#define FTM_ACK_SIG            0x0008    /* State change ack signal              */
#define FTM_CMD_RSP_SIG        0x0010    /* Command response signal              */
#define FTM_CLK_WAIT_SIG       0X0020    /* Used generic for ftm_clk_rex_wait    */ 
#define FTM_SIG_WAIT_SIG       0X0040    /* Used generic for ftm_sig_rex_wait    */ 
#define FTM_SIG_WAIT_SIG2      0x00010000    /* Used generic for ftm_sig_rex_wait    */
#define FTM_ASYNC_PKT_SIG      0x0080    /* Used to process asyn packets         */
#define FTM_LEDS_SIG           0x0100    /* Used LEDs update                     */
#ifdef FTM_HAS_1X
#define FTM_ENABLE_PROT_SIG    0x0200    /* FTM enable protocol signal           */
#define FTM_MC_READY_SIG       0x0400    /* MC FTM ready siganl                  */
#endif
#define FTM_BT_SIG             0x0800    /* BT FTM siganl                        */
#define FTM_SEARCH_SIG         0x1000

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FTM_HAS_UMTS
#error code not present
#endif

#define FTM_NUM_CMDS           5

/* LEDS defines */
#ifdef FEATURE_FTM_LEDS
  #define FTM_LEDS_RATE         90 /* ms */
  #define FTM_LEDS_MASK         0xFFFF
  #define FTM_IDLE_LEDS_MASK    0x07E0
  #define FTM_IDLE_LEDS_SHIFT   5
  #define FTM_IDLE_LEDS_BITS    4
  #define FTM_WAIT_LEDS_MASK    0x001F
  #define FTM_WAIT_LEDS_SHIFT   0
  #define FTM_WAIT_LEDS_RATE    200 /* ms */
  #define FTM_CMD_LEDS_MASK     0xF800
  #define FTM_CMD_LEDS_SHIFT    11

  /* 16 LEDs layout                        */
  /* ------------------------------------- */
  /* | CMD COUNT |    IDLE   | Wait COUNT| */
  /* |  5 LEDS   |   6 LEDS  |   5 LEDS  | */
  /* ------------------------------------- */
#endif /* FEATURE_FTM_LEDS */

/* defines to use with ftm_sig_clk_rex_wait */
#define FTM_WAIT_FOREVER        0 
#define FTM_WAIT_TIMEOUT        FTM_CLK_WAIT_SIG
/*===========================================================================

                                  MACROS

===========================================================================*/
#define FTM_MSG_FATAL(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FTM, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define FTM_MSG_ERROR(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FTM, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define FTM_MSG_HIGH(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FTM, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define FTM_MSG_MED(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FTM, MSG_LEGACY_MED, x_fmt, a, b, c)

#define FTM_MSG_LOW(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FTM, MSG_LEGACY_LOW, x_fmt, a, b, c)


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
LOCAL void ftm_req (void);

/*===========================================================================

FUNCTION FTM_PROCESS_EVENTS

DESCRIPTION
   This function handles delayed rsp requests from Diag + events from other 
   susbsystems. Dispatches the appropriate functions and or macros.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

LOCAL void ftm_process_events (void);


/*===========================================================================

FUNCTION FTM_DIAG_EVT_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_evt_processor(void* pdata);

/*===========================================================================

FUNCTION FTM_DIAG_MSG_CONFIG_CMD_PROCESSOR

DESCRIPTION
   This function handles 125 requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_msg_config_cmd_processor(void* pdata);

/*===========================================================================

FUNCTION FTM_DIAG_DELAYED_RESPONSE_CMD_PROCESSOR

DESCRIPTION
   This function handles delayed rsp requests from Diag 

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/

void ftm_diag_delayed_rsp_cmd_processor(void* pdata);

/*===========================================================================

FUNCTION FTM_TASK

===========================================================================*/

void  ftm_task ( dword );
/* ==================================================================
FUNCTION FTM_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *ftm_malloc (unsigned long num_bytes);


/* ==================================================================
FUNCTION FTM_FREE
DESCRIPTION
   Frees a chunk of memory allocated using ftm_malloc.
===================================================================== */
void ftm_free (void *mem_ptr);

/* ==================================================================
FUNCTION FTM_ACTIVATE_PROTOCOL
DESCRIPTION
   The function is responsible for getting MB ready to FTM mode.
===================================================================== */
void ftm_activate_protocol ( prot_act_e_type );


/* ==================================================================
FUNCTION FTM_ACTIVATE_PROTOCOL
DESCRIPTION
   The function is responsible for release any resource used by FTM mode.  
 (for example, deregister isr handler from gstmr_handlers[],etc)
===================================================================== */
void ftm_deactivate_protocol  (prot_deact_e_type , prot_trans_type );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer.
===========================================================================*/
rex_sigs_type ftm_sig_rex_wait ( rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT_NO_CLEAR
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. At end of function, signals are not cleared.
===========================================================================*/
rex_sigs_type ftm_sig_rex_wait_no_clear (rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT_CB
DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait where a
  [ void func (uint32) ] function signature is needed.
===========================================================================*/
void ftm_sig_rex_wait_cb ( rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_SIG_REX_WAIT__NO_CLEAR_CB
DESCRIPTION
  This function is a stub to be able to call ftm_sig_rex_wait_no_clear where 
  a [ void func (uint32) ] function signature is needed.
===========================================================================*/
void ftm_sig_rex_wait_no_clear_cb (rex_sigs_type mask);

/*===========================================================================
FUNCTION FTM_SIG_REX_SET
DESCRIPTION
  This procedure sets a signal for ftm_task to handle.
===========================================================================*/
void ftm_sig_rex_set ( rex_sigs_type mask );

/*===========================================================================
FUNCTION FTM_CLK_REX_WAIT
DESCRIPTION
   This function does a wait in ms, but does not hug the cpu. instead it lets
   the OS (REX) run and other tasks run.            
===========================================================================*/
void ftm_clk_rex_wait (uint32 msec);

/*===========================================================================
FUNCTION FTM_SIG_CLK_REX_WAIT
DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal and reset
  the mc_rpt_timer. The clk addition means that this function is capable
  of waiting for a signal with a time out, instead of waiting forever.
===========================================================================*/
rex_sigs_type ftm_sig_clk_rex_wait
(
  rex_sigs_type   mask,        /* Mask of signals to wait for */
  uint32          msec         /* Timeout value in msecs */
);

/*===========================================================================
FUNCTION FTM_LEDS
DESCRIPTION  
  This procedure writes to the LEDs port shifting and masking the data.
===========================================================================*/
#ifdef FEATURE_FTM_LEDS    
void ftm_leds (word mask, word data, byte left_shift);
#else
__inline void ftm_leds (word mask, word data, byte left_shift) { }
#endif /* FEATURE_FTM_LEDS */

/*===========================================================================

FUNCTION       FTM_PRINTF
DESCRIPTION
  This function is a printf clone, it will work just as printf does, but 
  will output to string to TBD. 
===========================================================================*/
void ftm_printf(ftm_msg_lvl_type msg_lvl_enum, const char *fmt, ... );


#endif /* #ifndef FTM_TASK_H */

