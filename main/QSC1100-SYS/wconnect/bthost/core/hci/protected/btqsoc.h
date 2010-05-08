#ifndef _BTQSOC_H
#define _BTQSOC_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_QSOC

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
/* Debug features */
/* #define BT_QSOC_INBAND_TRACE_ENABLE */
/* #define BT_HC_DRV_INBAND_TRACE_ENABLE */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       B L U E T O O T H    B T S    D R I V E R    H E A D E R    F I L E

GENERAL DESCRIPTION
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/protected/btqsoc.h#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $
*   #12        14 Nov 2008          SA
*   Support for NV automation 
*   #11        28 Feb 2008          SSK
*   Optimise Inband SM to enter TX BREAK after waiting for TX path to go idle.
* 
*   #10        10 Jan 2008          SSK
*   Introduce new QSOC driver state:BT_QSOC_SHUT_DOWN to fix "SoC fails to come
*   up after it is powered OFF in Inband sleep enabled  builds" issue.
*
*   #9         29 Nov 2007          SSK
*   Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
*
*   #8         19 Oct 2007          SSK
*   Adding function bt_qsoc_tot_hc_tx_len() protototype.
*
*   #7         04 Sept 2007         BH
*   Added R3 Bumped-Die and R4 support.
*
*   #6         24 Jul 2007          BH
*   Added support for addtional R3 patches.
*
*   #5         06 Jun 2007          DSN
*   Add support to get Enc key Length from QSOC.
*
*   #4         04 Apr  2007          BH
*   Updated comments for QSoC shutdown/powerup.
*
*   #3         04 Apr  2007          BH
*   Added QSoC sleep support.
*
*   #2         02 Apr  2007          AP
*   Change qsoc init to a global var. Added marco.
*
*   #1         02 Mar  2007          JN
*   Created new file for BTS 4020 (QSoC) support.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/


/*=========================================================================*/
/*                               CONSTANTS                                 */
/*=========================================================================*/

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/

/* BTS initialization states */
typedef enum
{
  BT_QSOC_INIT_STARTED = 0,
  BT_QSOC_DETECT_FW,
  BT_QSOC_POKE8_TBL_INIT,
  BT_QSOC_FIXED_TBL_INIT,
  BT_QSOC_DYN_TBL_INIT,
  BT_QSOC_NV_TBL_INIT,
  BT_QSOC_CUST_TBL_INIT,
  BT_QSOC_HCI_RESET,
  BT_QSOC_INIT_COMPLETED,
  BT_QSOC_SHUT_DOWN
} bt_qsoc_init_st_enum_type;

/*=========================================================================*/
/*                           DATA DECLARATIONS                             */
/*=========================================================================*/
extern bt_qsoc_init_st_enum_type bt_qsoc_init_state;

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
extern rex_timer_type            bt_qsoc_inband_rx_flush_timer;
extern rex_timer_type            bt_qsoc_inband_tx_flush_timer;
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

/*=========================================================================*/
/*                                MACROS                                   */
/*=========================================================================*/
#define BT_QSOC_IS_READY() \
  (( bt_qsoc_init_state == BT_QSOC_INIT_COMPLETED ) ? TRUE:FALSE)

/*=========================================================================*/
/*                           FUNCTION INTERFACES                           */
/*=========================================================================*/
/*==========================================================================

  FUNCTION       BT_QSOC_POWERUP

  DESCRIPTION    This function performs the BTS 4020 power-up sequence.  

  DEPENDENCIES   The PMIC interface is not available on 60x5 series, 
                 which uses GPIO interface for power supply.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   Since bt_qsoc_shutdown can put the SIO driver to
                 sleep, a matching bt_qsoc_powerup must be able to
                 wake it.  This function will awaken the SIO driver
                 if it is sleeping.

==========================================================================*/
extern void bt_qsoc_powerup
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_INIT

  DESCRIPTION    This function kick-starts BTS initialization by issuing 
                 VS cmd to determine firmware version. All the VS cmds to 
                 set BTS' NVM are sent following each other. The initialization 
                 ends by sending HCI_RESET command.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_init
(
  void
);


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_SIGNALS

  DESCRIPTION    This function processes the SoC specific signals which are 
                 defined for BTS. Signals that are not expected are still 
                 cleared but a log message is generated.

  DEPENDENCIES   None.

  PARAMETERS     rex_signals - mask of all the signals currently asserted.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_process_signals
(
  rex_sigs_type  rex_signals
);


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_VS_EVENT

  DESCRIPTION    This function handles vendor specific events sent from BTS.
                 At present it only handles the embedded dowloader events 
                 (received in response to firmware version cmd) and NVM 
                 events (received in response to NVM configuration cmds).

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr - vendor specific event data.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_process_vs_event
(
  bt_ev_msg_type *ev_msg_ptr
);


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_VS_CMD_CMPLT_EVENT

  DESCRIPTION    This function handles command complete events resulting from
                 vendor specific commands sent to the BTS.

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr - vendor specific event data.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_process_vs_cmd_cmplt_event
(
  bt_ev_msg_type *ev_msg_ptr
);


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_HCI_RESET

  DESCRIPTION    Handles any BTS specific HCI_RESET processing. BTS requires
                 an HCI_RESET cmd to apply BTS configuration parameters 
                 (sent down at initialization).

  DEPENDENCIES   It is important to note that sequence of steps to ensure 
                 proper effect of changing the baud-rate and apply 
                 configurations (just sent) on BTS4020. To ensure HCI_RESET
                 response comes back only after the MSM's UART1DM has been 
                 properly set to new baud-rate, the BTS must be flowed-off 
                 even before   the issue of HCI_RESET command.

  PARAMETERS     ev_msg_ptr - HCI_RESET event data.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_process_hci_reset
(
  bt_ev_msg_type *ev_msg_ptr
);


/*==========================================================================

  FUNCTION       BT_QSOC_SHUTDOWN

  DESCRIPTION    This routine powers down BTS. It also performs any other 
                 clean up required.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   SIO driver will be put to sleep.

==========================================================================*/
extern void bt_qsoc_shutdown
(
  void
);

/*==========================================================================

  FUNCTION       BT_QSOC_GET_ENC_KEY_LEN

  DESCRIPTION    This function sends a VS command to peek a certain number of 
                 bytes from a particular memory location of BTS Controller.

  DEPENDENCIES   None.

  PARAMETERS     peek_addr - Address to read the bytes from
                 num_bytes  - Number of bytes to read 
                 
  RETURN VALUE   TRUE (success) or FALSE (failure).

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_get_enc_key_len
(  
  uint16 conn_hndl
);


/*==========================================================================

  FUNCTION       BT_QSOC_TOT_HC_TX_LEN

  DESCRIPTION    This routine returns the total buffer length available for
                 Tx in the BTS SOC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern uint16 bt_qsoc_tot_hc_tx_len
(
  void
);


#ifdef FEATURE_BT_QSOC_SLEEP
/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP

  DESCRIPTION    This routine sends a sleep message to the SoC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_sleep
(
    void
);

/*==========================================================================

  FUNCTION       BT_QSOC_WAKEUP

  DESCRIPTION    This routine sends a wake message to the SoC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_qsoc_wakeup
(
    void
);
#endif /* FEATURE_BT_QSOC_SLEEP */

#endif /* FEATURE_BT_QSOC */
#endif /* FEATURE_BT */
#endif /*_BTQSOC_H*/
