/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   F T M   M O D U L E
                     
GENERAL DESCRIPTION
  

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2002, 2003 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm.c#2 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/07   dyc     Added FTM_HAS_VREG_VOTE feature to support VREGs that need to
                   be managed at FTM level.
08/01/07   kg      featurization changes for EDGE only support
06/15/07   xw      Added ftm_enable_chip_reset() for 6260/6280.
06/02/07   vm      Added the support for Beta Scaling for SC2X
03/16/07   ka      Added call to reset RTR when exiting FTM.
05/18/06   tm      Initial release for dsp diagnostics driver - new FTM subsystem.
                   Event mechanism to conserve signal defs, and support for delayed
                   response.
05/10/06   ra      For 1x added logic for sleep voting
02/21/06   tws     Change WCDMA_1800 to BC3. Add support for BC4 and BC9.
01/18/06   sar     Replaced reference to RF_STATE_GPS with DB_RF_GPS in ftm_deactivate_protocol().. 
08/01/05   ra      For 1x: removed call to change rfm_initialized due to Park Mode changes
07/20/05   dp      Ensure that FTM WCDMA BER sends stop request to L1 while ftm_mode = FTM_MODE
                   so that L1 sends the response back to FTM, not RRC.
07/12/05   jac     Added line to set ftm_current_rf_mode = FTM_DB_RF_IDLE in
                   ftm_deactivate_protocol().
06/23/05   jac     Added variable for GSM 1X change. 
06/20/05   jac     Added code to send a deactivate command to the searcher when 
                   exiting FTM mode to fix phone crash when transitioning from FTM to
                   online after entering FTM GSM RF mode. 
03/18/05   ra      Removed FTM_HAS_UMTS wrapper from ftm_get_mode() to allow all system mode access.
                   Also, deleted ftm_set_mode api located in this file(there is a 1x specific ftm_set_mode which has different 
                   functionality and is not part of this change). It set the global variable ftm_mode however this is to 
                   be done in ftm_activate_protocol and ftm_deactivate_protocol. 
02/09/05   ra      For 1x targets, fixed mode switch ftm->online for all modes.
01/07/05   jb      Removed rfm_init_digital from ftm_activate_protocol to fix 2nd chain call
11/29/04   ra      Changed 1x section of ftm_activate_protocol
11/23/04   ra      For 1x targets, set rfm_initialized global variable to false in ftm_deactivate_protocol to that the rf_apinit for the 3 device desriptors gets 
                   executed in rfm_init 
11/18/04   dp      Added FTM BER cleanup function calls in ftm_deactivate_protocol
10/12/04   jac     Mainlined ASYNC packet code.
10/01/04   ra      Renamed FTM_GET_RF_MODE to FTM_GET_RF_DB_STATE.
10/01/04   ra      Renamed FTM_SET_RF_MODE to FTM_SET_RF_DB_STATE.
09/27/04   et      initialized the ftm_current_rf_mode to FTM_DB_RF_IDLE
                   unconditionally for all targets.
09/27/04   xw      Included task.h to fix saber P1 compilation error
09/03/04   dp      Added access functions for ftm mode variables
                   Added support for FTM GSM BER mode
08/24/04   ra      Merge 1X and UMTS
08/23/04   xw      Added FTM PMIC dispatch. 
08/20/04   xw      Removed ftm_wait LED for Saber.
10/02/03   ka      Added support for I2C.
10/02/03   ka      Added support for variable lenght response packet.
08/21/03   xw      Added check to abort FTM commands if we are not in FTM mode.
									 For FTM_HWTC builds, this check is ignored and FTM commands
									 are executed in non FTM modes.
05/23/03   xw      Added CPHY_STOP_WCDMA_MODE_REQ cmd in ftm_deactivate_protocol()
05/06/03   xw      Changed mmoc_rpt_ph_stat_chgd_cnf() to mmoc_rpt_prot_deactd_cnf()
                   in ftm_deactivate_protocol()
04/25/03   xw      Reduced FTM heap size from 2048*2 to 2048 
                   Modified ftm_wait() and added ftm_gsm_wait() for gl1_hw_init()
                   Modified ftm_clk_rex_wait()
                   Removed case FTM_1X_C in ftm_req()
                   Moved tmc_task_start() out of ftm_task_init() and placed it after 
                   ftm_task_init()  
02/20/03   xw      Added ftm_task support
08/25/02   sb      Initial revision


===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_task.h"
#include "assert.h"
#include "task.h"

#ifdef FTM_HAS_WCDMA 
#error code not present
#endif /*FTM_HAS_WCDMA*/

#ifdef FTM_HAS_GSM
#error code not present
#endif /*FTM_HAS_GSM*/

#ifdef FEATURE_FTM_HWTC
#error code not present
#endif


#ifdef FTM_HAS_1X
#include "rfm.h"
#include "sleep.h"
#include "rficap.h"
#endif

#if defined(FTM_HAS_GSM) && defined(FTM_HAS_1X)
#error code not present
#endif

#if defined(T_MSM6260) || defined(T_MSM6280)
#error code not present
#endif

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/
extern ftm_pkt_data_type ftm_pkt_data; /* holds information for current ftm cmd */

#if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
extern boolean rf_cdma_load_master_lin_tables;
#endif
/* To track the current RF mode */
extern ftm_rf_mode_type ftm_current_rf_mode;
#ifdef FTM_HAS_1X
extern rf_card_type rf_card;
extern sleep_okts_handle ftm_okts_handle;
#endif 
/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS
===========================================================================*/
#if defined( FTM_HAS_UMTS)
#error code not present
#else
  ftm_mode_type ftm_mode = DMSS_MODE;
#endif
  
  /* Set the default ftm_current_rf_mode to FTM idle */
  ftm_rf_mode_type ftm_current_rf_mode = FTM_DB_RF_IDLE;
  



/* Command queue for the test task.  All command to this task will be
** placed on this queue to await processing.
*/
q_type ftm_task_cmd_q;

/* Event queue for the task.  General commands (not 75 commands from Diag)
** will be placed on this queue to await processing.
*/
q_type ftm_task_event_q;

/*===========================================================================

FUNCTION   ftm_enable_chip_reset

DESCRIPTION
  It enables RF chip reset when the UE does a mode change from ftm to online. 

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ftm_enable_chip_reset(boolean enable)
{
#if defined(T_MSM6280) || defined(T_MSM6260) 
#error code not present
#endif
}

/*===========================================================================

FUNCTION FTM_CMD

DESCRIPTION  
   This function is used to send command to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_cmd
(
   ftm_msg_type *msg_ptr   
   /* pointer to a test command packet.  When the command has finished,
   ** the command structure is put on the "done queue". If there is no
   ** done queue", or this operation is not desirable, set the "done queue"
   ** pointer to NULL.  Also when the command has completed, the specified
   ** task is signaled with the sig parameter.  If no signaling is required,
   ** the task pointer parameter can be set to NULL.
   */
)
{
  (void) q_link(msg_ptr, &msg_ptr->hdr.link );      /* Initialize link field */

  q_put( &ftm_task_cmd_q, &msg_ptr->hdr.link );         /* Put on command queue */

  (void) rex_set_sigs( &ftm_tcb, FTM_CMD_Q_SIG ); /* Signal a queue event */

} /* End ftm_cmd */



/*===========================================================================

FUNCTION FTM_EVENT

DESCRIPTION  
   This function is used to send events to the ftm task.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_event
(
   ftm_event_type *evt_ptr   
   /* see ftm_cmd */
)
{
  (void) q_link(evt_ptr, &evt_ptr->hdr.link );    /* Initialize link field */

  q_put( &ftm_task_event_q, &evt_ptr->hdr.link ); /* Put on command queue  */

  (void) rex_set_sigs( &ftm_tcb, FTM_EVT_Q_SIG ); /* Signal a queue event  */

} /* End ftm_event */




/*===========================================================================

FUNCTION FTM_ACTIVATE_PROTOCOL

DESCRIPTION  
   The function is responsible for getting MB ready to FTM mode.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_activate_protocol ( prot_act_e_type act_reason )
{
act_reason = act_reason; /* Lint */
  ftm_mode = FTM_MODE;

#ifdef FTM_HAS_UMTS 
#error code not present
#endif 

#ifdef  FTM_HAS_1X 

#if defined (RF_HAS_MDSP_TX_AGC) || defined(RF_HAS_PDM_VS_POWER_TX_CAL)
rf_cdma_load_master_lin_tables = TRUE;
#endif

  rfm_enable_rx( RFCOM_TRANSCEIVER_0, NULL, NULL );

#if defined (FTM_HAS_VREG_VOTE)
  //pm_vote_vreg_switch( PM_ON_CMD, PM_VREG_RFRX2_ID,  PM_VOTE_VREG_RFRX2_APP__FTM );  //dyc
#endif

  dog_autokick(FALSE);
  sleep_negate_okts(ftm_okts_handle );
  ftm_sig_rex_set( FTM_ENABLE_PROT_SIG ); 
#endif 
  /* send sig to FTM task so it will wait for MC to be ready before issuing 
     any ftm commands 
  */

  FTM_MSG_MED("Activate FTM Protocol...",0,0,0);

}

/*===========================================================================

FUNCTION FTM_DEACTIVATE_PROTOCOL

DESCRIPTION  
    The function is responsible for release any resource used by FTM mode.  
 (for example, deregister isr handler from gstmr_handlers[],etc)

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_deactivate_protocol  (prot_deact_e_type deact_reason, prot_trans_type trans_id )
{
#if defined( FTM_HAS_GSM ) && defined( FTM_HAS_1X )
#error code not present
#endif

#ifdef FTM_HAS_1X
  rf_path_enum_type path = rfcom_to_path[ RFCOM_TRANSCEIVER_0 ];

  rf_chain_state_type *rf_chain_ptr = &rf_chain_status[ path ];
#endif 
#if defined( FTM_HAS_WCDMA )  
#error code not present
#endif 

#ifdef FTM_HAS_GSM
#error code not present
#endif


#ifdef FTM_HAS_1X

  /* Reset ftm_current_rf_mode */
  ftm_current_rf_mode = FTM_DB_RF_IDLE;

  ftm_mode = DMSS_MODE;
  #ifdef RF_HAS_BYPASS_RF_STATE_GPS_CHECK
  if( rf_chain_ptr->rf_mode == DB_RF_GPS)
  #else
  if( rf_chain_ptr->rf_state == RF_STATE_GPS)
  #endif
  {
    rf_digital_band_select(RFCOM_TRANSCEIVER_0,rf_card.rfc_default_band);
  }
  #ifdef FEATURE_FTM_HWTC
#error code not present
  #endif 
  sleep_assert_okts(ftm_okts_handle );

#if defined (FTM_HAS_VREG_VOTE)
  //pm_vote_vreg_switch( PM_OFF_CMD, PM_VREG_RFRX2_ID,  PM_VOTE_VREG_RFRX2_APP__FTM );  //dyc
#endif /* FTM_HAS_VREG_VOTE */

#endif 

#ifdef FTM_HAS_UMTS
#error code not present
#endif /* FTM_HAS_UMTS */
}

/*===========================================================================

FUNCTION FTM_GET_MODE

DESCRIPTION
		 

DEPENDENCIES
	 None.

RETURN VALUE
	 None.

SIDE EFFECTS
	 None.

===========================================================================*/
ftm_mode_type  ftm_get_mode( void)
{
    return ftm_mode;
}

/*===========================================================================

FUNCTION FTM_GET_RF_DB_STATE

DESCRIPTION
		 

DEPENDENCIES
	 None.

RETURN VALUE
	 None.

SIDE EFFECTS
	 None.

===========================================================================*/
ftm_rf_mode_type  ftm_get_rf_db_state( void )
{
    return ftm_current_rf_mode;
}


/*===========================================================================

FUNCTION FTM_SET_RF_DB_STATE

DESCRIPTION
		 

DEPENDENCIES
	 None.

RETURN VALUE
	 None.

SIDE EFFECTS
	 None.

===========================================================================*/
void ftm_set_rf_db_state( ftm_rf_mode_type  mode)
{
    ftm_current_rf_mode = mode;
}

#endif /* FEATURE_FACTORY_TESTMODE */
