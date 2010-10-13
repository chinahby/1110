#ifndef SRCH_MDSP_H
#define SRCH_MDSP_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S E A R C H   T A S K  --  M D S P   S U B S Y S T E M

GENERAL DESCRIPTION
  This module contains functions pertaining to the MDSP used by the 
  demodulator and searcher. 

EXTERNALIZED FUNCTIONS/MACROS
  srch_mdsp_download
  srch_mdsp_demod_running
  srch_mdsp_flush_samp_ram
  srch_mdsp_sched_flush_samp_ram
  srch_mdsp_check_cmd_until_e16
  srch_mdsp_check_cmd_until_e32
  srch_mdsp_check_cmd_until_ne16
  srch_mdsp_check_cmd_until_ne32
  srch_mdsp_sleep
  srch_mdsp_wakeup
  srch_mdsp_awake
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2002 - 2007
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_mdsp.h#1 $

when       who      what, where, why 
--------   ---      ----------------------------------------------------------
03/05/07   aps      Added srch_mdsp_add_isr_cb().
03/05/07   nnv      Added function srch_mdsp_mnd_ctrl
01/23/07   rkc      Added srch_mdsp_enable().
10/31/06   tjc      Changed srch_mdsp_sleep/wakeup macros to functions and
                    added srch_mdsp_awake() check function.
07/13/05   grl      Added flushing the sample ram of the requested chain.
06/15/05   bt       Removed MDSP_CMD_FLUSH_SR.
04/20/05   sst      Removed srch_mdsp_isr
03/10/05   ddh      Added Featurized support for executing on 7500 RUMI station
02/07/05   sst      Created srch_mdsp_disable() to disable app and update vars
                    Extern'ed srch_mdsp_pre1x()
02/04/05   bt       Add scheduled sample ram flush.
08/09/04   sst      Extern'ed new generic mdsp isr func srch_mdsp_isr()
06/21/04   sst      Corrected naming of srch_mdsp_check_cmd_ functions
                    Updated srch_mdsp_check_cmd_ functions and their macros
05/04/04   sst      Created functions out of MDSP_CHECK_CMD_E/NE
02/19/04   sst      Lint corrections in MDSP_CHECK_CMD_E/NE
01/28/04   ejv      Mainline T_MSM6500.
12/15/03   sst      Added 2 macro's MDSP_CHECK_CMD_*() to check the MDSP cmd
                    semaphore, in which a timeout and checking of MDSP_dspHalt
                    reg is used to to report MDSP (fatal) errors
09/18/03   bt       Wait for the command semaphore after waking up the dsp srch
                    part. Also delete unused macros.
06/25/03   ejv      Update srch_mdsp_sleep() and srch_mdsp_wakeup().
06/05/03   ejv      Replace old function srch_mdsp_current_image() with
                    new function srch_mdsp_demod_running().
01/23/03   bt       Added srch_mdsp_sleep/wakeup macros.Cleaned up func names.
10/29/02   bt       add srch_mdsp_flush_samp_ram command
09/11/02   va       Added function to get the current mdsp image.
07/23/02   bt       implementation , first cut
===========================================================================*/ 


/*===========================================================================

                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "mdsp.h"
#include "timetick.h"
#include "sample_server.h"


/*===========================================================================
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------
      Constants & Macros
-------------------------------------------------------------------------*/

/* This macro will signal the MDSP to execute a command */
#define MDSP_SIGNAL() \
         mdsp_gen_interrupt( 1 << SRCH_CMD_INTERRUPT )

/* value the MDSP will return if the MDSP received a bad command */
#define MDSP_CMD_ERROR                 0xffff

/* time to wait for the MDSP command semaphore to clear (in ms) */
#ifdef T_RUMI
#error code not present
#else
#define MDSP_CMD_TIMEOUT_MS            5
#endif /* T_RUMI */


/* turn off the lint errors that come from this optimization */
/*lint -emacro(506,MDSP_CHECK_CMD_UNTIL_E)   Const val Boolean */
/*lint -emacro(774,MDSP_CHECK_CMD_UNTIL_E)   "if" always true */
/*lint -emacro(740,MDSP_CHECK_CMD_UNTIL_E)   Unusual pointer cast */
/*lint -emacro(506,MDSP_CHECK_CMD_UNTIL_NE)  Const val Boolean */
/*lint -emacro(774,MDSP_CHECK_CMD_UNTIL_NE)  "if" always true */
/*lint -emacro(740,MDSP_CHECK_CMD_UNTIL_NE)  Unusual pointer cast */

/* Macro to check MDSP command semaphore at "addr"
   Loops util command register does not equal "comp"
   Calls one of two functions (compile time determined) depending on the
   data type pointed to */
#define MDSP_CHECK_CMD_UNTIL_E( addr, comp )                                \
{                                                                           \
  /*lint -save -e774,740  "if" always true,  Unusual pointer cast */        \
  if ( sizeof(*addr) == sizeof(uint16) )                                    \
  {                                                                         \
    srch_mdsp_check_cmd_until_e16((volatile uint16*)addr, (uint16)(comp));  \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    srch_mdsp_check_cmd_until_e32((volatile uint32*)addr, (uint32)(comp));  \
  }                                                                         \
  /*lint -restore */                                                        \
}

/* Macro to check MDSP command semaphore at "addr"
   Loops util command register equals "comp"
   Calls one of two functions (compile time determined) depending on the
   data type pointed to */
#define MDSP_CHECK_CMD_UNTIL_NE( addr, comp )                               \
{                                                                           \
  /*lint -save -e774,740  "if" always true,  Unusual pointer cast */        \
  if ( sizeof(*addr) == sizeof(uint16) )                                    \
  {                                                                         \
    srch_mdsp_check_cmd_until_ne16((volatile uint16*)addr, (uint16)(comp)); \
  }                                                                         \
  else                                                                      \
  {                                                                         \
    srch_mdsp_check_cmd_until_ne32((volatile uint32*)addr, (uint32)(comp)); \
  }                                                                         \
  /*lint -restore */                                                        \
}

/**************************************************************************
 *  DSP INTERFACE FOR SLEEP/WAKEUP COMMANDS                               *
 *************************************************************************/

/* Change DSP waitstates to Searcher 4 running at 40MHz */     
#define DSP_CMD_CHANGE_WS_40_MHZ( ) \
         mdsp_write(MDSP_xmemWaitSrch, 0x0505)

/* Change DSP waitstates to Searcher 4 running at 33MHz */     
#define DSP_CMD_CHANGE_WS_33_MHZ( ) \
         mdsp_write(MDSP_xmemWaitSrch, 0x0606)

/* Change DSP waitstates to Searcher 4 running at Cx8 (9.83 MHz) */     
#define DSP_CMD_CHANGE_WS_CX8( ) \
         mdsp_write(MDSP_xmemWaitSrch, 0x1111)

/* Command DSP to sleep immediately */
#define DSP_CMD_SLEEP_NOW( )
         
/* Inform DSP of wakeup */
#define DSP_CMD_WAKEUP( )
         
/* Retrieve RTC value when sleep was triggered */
#define DSP_GET_SLEEP_RC_HI( ) \
         mdsp_read_offset(SRCH_microSrchData,0);
         
#define DSP_GET_SLEEP_RC_LO( ) \
         mdsp_read_offset(SRCH_microSrchData,1);

/* MDSP_MOD_READY callback function type */
typedef void (*srch_mdsp_mod_ready_cb_t) (void);

/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/

/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_MDSP_SLEEP
DESCRIPTION    This function informs the MDSP subsystem that it can shut of 
               the DSP clock if nobody else needs it. 
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_sleep(void);

/*===========================================================================

FUNCTION       SRCH_MDSP_WAKEUP
DESCRIPTION    This function wakes up mdsp from sleep. 
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_wakeup(void);

/*===========================================================================

FUNCTION       SRCH_MDSP_AWAKE
DESCRIPTION    Checks if demod is running on the MDSP and MDSP is awake.
DEPENDENCIES   None

RETURN VALUE   TRUE if demod is running and MDSP is awake; FALSE otherwise
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_mdsp_awake(void);

/*===========================================================================

FUNCTION       SRCH_MDSP_PRE1X
DESCRIPTION    This function should be called before enabling the
               1X App, to make sure clocks, DSP variable, etc. are
               in the correct state
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_pre1x( void );

/*===========================================================================

FUNCTION       SRCH_MDSP_DOWNLOAD
DESCRIPTION    This function downloads the MDSP
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_download(void);
       
/*===========================================================================

FUNCTION       SRCH_MDSP_ENABLE
DESCRIPTION    This function enables the searcher MDSP App and registers a
               callback function to be called when event_handler is called
               after MDSP_MOD_READY is received.
DEPENDENCIES   MDSP task must be running

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_enable( srch_mdsp_mod_ready_cb_t mod_ready_cb );

/*===========================================================================

FUNCTION       SRCH_MDSP_DISABLE
DESCRIPTION    This function disables the searcher MDSP App
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_disable( void );

/*===========================================================================

FUNCTION       SRCH_MDSP_DEMOD_RUNNING
DESCRIPTION    Checks if the demod is running on the DSP
DEPENDENCIES   None

RETURN VALUE   TRUE if demod is running; FALSE otherwise
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_mdsp_demod_running(void);

/*===========================================================================

FUNCTION       SRCH_MDSP_FLUSH_SAMP_RAM
DESCRIPTION    This function commands the MDSP to flush the sample ram. It 
               will cause subsequent searcher 4 searches to wait until enough
               samples have been collected. 
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_flush_samp_ram
(
  sample_server_rx_chain_type samp_serv_chain    /* sample server chain */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_SCHED_FLUSH_SAMP_RAM
DESCRIPTION    This function commands the MDSP to flush the sample ram at a 
               specific point. It will cause subsequent searcher 4 searches 
               to wait until enough samples have been collected. 
DEPENDENCIES   MDSP task must be running.

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_sched_flush_samp_ram
(
  sample_server_rx_chain_type samp_serv_chain,   /* sample server chain */
  uint16                      position           /* position in chips */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_CHECK_CMD_UNTIL_E16
DESCRIPTION    This function checks a specified register, and loops until
               that register is equal to "comp"
DEPENDENCIES   MDSP task must be running. Expecting a read of type uint16

RETURN VALUE   None
SIDE EFFECTS   This could possibily FATAL error the phone

===========================================================================*/
extern void srch_mdsp_check_cmd_until_e16
(
  volatile uint16 *addr,                         /* address of register */
  uint16           comp                          /* comparator */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_CHECK_CMD_UNTIL_E32
DESCRIPTION    This function checks a specified register, and loops until
               that register is equal to "comp"
DEPENDENCIES   MDSP task must be running. Expecting a read of type uint32

RETURN VALUE   None
SIDE EFFECTS   This could possibily FATAL error the phone

===========================================================================*/
extern void srch_mdsp_check_cmd_until_e32
(
  volatile uint32 *addr,                         /* address of register */
  uint32           comp                          /* comparator */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_CHECK_CMD_UNTIL_NE16
DESCRIPTION    This function checks a specified register, and loops until
               that register is not equal to "comp"
DEPENDENCIES   MDSP task must be running. Expecting a read of type uint16

RETURN VALUE   None
SIDE EFFECTS   This could possibily FATAL error the phone

===========================================================================*/
extern void srch_mdsp_check_cmd_until_ne16
(
  volatile uint16 *addr,                         /* address of register */
  uint16           comp                          /* comparator */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_CHECK_CMD_UNTIL_NE32
DESCRIPTION    This function checks a specified register, and loops until
               that register is not equal to "comp"
DEPENDENCIES   MDSP task must be running. Expecting a read of type uint32

RETURN VALUE   None
SIDE EFFECTS   This could possibily FATAL error the phone

===========================================================================*/
extern void srch_mdsp_check_cmd_until_ne32
(
  volatile uint32 *addr,                         /* address of register */
  uint32           comp                          /* comparator */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_MND_CTRL
DESCRIPTION    This function enables or disables the 1x control over MND
               counter hardware
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   None

===========================================================================*/
extern void srch_mdsp_mnd_ctrl
(
  boolean enable                              /* enable or disable control */
);

/*===========================================================================

FUNCTION       SRCH_MDSP_ADD_ISR_CB
DESCRIPTION    This function is an API to add a callback to the
               SRCH MDSP ISR.
               
DEPENDENCIES   None

RETURN VALUE   None
SIDE EFFECTS   This could possibily FATAL error the phone. Callbacks once
               added cannot be removed. 

===========================================================================*/
void srch_mdsp_add_isr_cb
(
 void ( *rf_cb )( void )
);

#endif /* SRCH_MDSP_H */

