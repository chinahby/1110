/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   SoC   A B S T R A C T I O N   L A Y E R

GENERAL DESCRIPTION
  This module contains Bluetooth SoC abstraction layer interfaces.

EXTERNALIZED FUNCTIONS

  bt_soc_ifc_init()
    Initializes the abstraction interface.

  bt_soc_powerup()
    Stub function for power-up sequence in HCI mode.

  bt_qsoc_init()
    Stub function for initialization sequence in HCI mode.

  bt_soc_proc_signals()
    Stub function for processing signals in HCI mode.

  bt_soc_proc_vs_event()
    Stub function for processing vendor specific events in HCI mode.

  bt_soc_proc_hci_reset()
    Stub function for processing HCI_RESET event in HCI mode.

  bt_soc_shutdown()
    Stub function for power-off sequence in HCI mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/src/btsoc.c#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $
*
*   #7         19 Oct  2007          SSK
*   Add API to return total buffer length available for TX in the SOC.
*
*   #6         20 Aug 2007           PG
*   Seperated HCI generic vendor specific event handling from Qualcomm 
*   proprietary implementation used with embedded controller.
*
*   #5         25 May  2007          DSN
*   Add support to read WLAN type from NV.
*
*   #4         13 Apr  2007          JN
*   Initialize shutdown function ptr.
*
*   #3         04 Apr  2007          BH
*   Now setting the QSoC sleep/wakeup function pointers. And added
*   stub functions for bt_soc_sleep/wakeup.
*
*   #2         14 Mar  2007          JN
*   Following mods:
*   - Removed "_stub" from function names.
*   - Added bt_soc_ifc_init to initialize bt_soc_ifc.
*   - Feature baded header file inclusion for SoCs.
*
*   #1         02 Mar  2007          JN
*   Created new file for BT SoC Abstraction Layer.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*==========================================================================
                     PUBLIC INCLUDE FILES
===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_SOC

#include <string.h>
#ifdef FEATURE_BT_WLAN_COEXISTENCE
#include "nv.h"
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
#include "rex.h"
#include "pm.h"
#include GPIO_H

#include "bt.h"
#include "btdebug.h"
#include "bthcdrv.h"
#include "btmsg.h"
#include "btsocetc.h"
#include "btsoc.h"

#ifdef FEATURE_BT_SOC1
#error code not present
#elif (defined FEATURE_BT_QSOC)
#include "btqsoc.h"
#endif /* FEATURE_BT_SOC1 */

/*===========================================================================
                        MACROS
===========================================================================*/
#define BT_MSG_LAYER  BT_MSG_GN   /* necessary for btmsg.h */

/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
===========================================================================*/

bt_soc_abs_struct_type bt_soc_ifc;
bt_soc_type_enum       bt_soc_type;

#ifdef FEATURE_BT_WLAN_COEXISTENCE
bt_soc_wlan_enum_type  bt_soc_wlan_type;
#endif /* FEATURE_BT_WLAN_COEXISTENCE */

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================

  FUNCTION       BT_SOC_IFC_INIT

  DESCRIPTION    Initializes the abstraction interface for SoCs. Bases on 
                 the SoC type, it will assign appropriate and available
                 functions to the function pointers in bt_soc_ifc.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_ifc_init
(
  void
)
{
  /* Assign interface fucntion pointers appropriately based on SoC */        
  switch( bt_soc_type )
  {
#ifdef FEATURE_BT_QSOC
    case BT_SOC_TYPE_QSOC:
    {
      bt_soc_ifc.bt_soc_powerup_func_ptr = bt_qsoc_powerup;
      bt_soc_ifc.bt_soc_init_func_ptr = bt_qsoc_init;
      bt_soc_ifc.bt_soc_proc_soc_sigs_func_ptr = bt_qsoc_process_signals;
      bt_soc_ifc.bt_soc_proc_vs_event_func_ptr = bt_qsoc_process_vs_event;
      bt_soc_ifc.bt_soc_proc_hci_reset_func_ptr = bt_qsoc_process_hci_reset;
      bt_soc_ifc.bt_soc_proc_shutdown_func_ptr = bt_qsoc_shutdown;
#ifdef FEATURE_BT_QSOC_SLEEP
      bt_soc_ifc.bt_soc_sleep_func_ptr = bt_qsoc_sleep;
      bt_soc_ifc.bt_soc_wakeup_func_ptr = bt_qsoc_wakeup;
#endif /* FEATURE_BT_QSOC_SLEEP */

      break;
    }
#endif

#ifdef FEATURE_BT_SOC1
#error code not present
#endif

    case BT_SOC_TYPE_NONE: /* HCI Mode */
    default:
    {
      bt_soc_ifc.bt_soc_powerup_func_ptr = bt_soc_powerup;
      bt_soc_ifc.bt_soc_init_func_ptr = bt_soc_init;
      bt_soc_ifc.bt_soc_proc_soc_sigs_func_ptr = bt_soc_proc_signals;
      bt_soc_ifc.bt_soc_proc_vs_event_func_ptr = bt_soc_proc_vs_event;
      bt_soc_ifc.bt_soc_proc_hci_reset_func_ptr = bt_soc_proc_hci_reset;
      bt_soc_ifc.bt_soc_proc_shutdown_func_ptr = bt_soc_shutdown;
#ifdef FEATURE_BT_QSOC_SLEEP
      bt_soc_ifc.bt_soc_sleep_func_ptr = bt_soc_sleep;
      bt_soc_ifc.bt_soc_wakeup_func_ptr = bt_soc_wakeup;
#endif /* FEATURE_BT_QSOC_SLEEP */

      break;
    }
  } /* switch */
#ifdef FEATURE_BT_WLAN_COEXISTENCE
  bt_soc_wlan_type = BT_SOC_WLAN_NONE;
#endif /* FEATURE_BT_WLAN_COEXISTENCE */
} /* bt_soc_ifc_init */


/*==========================================================================

  FUNCTION       BT_SOC_POWERUP

  DESCRIPTION    Stub power-up function, used for HCI mode. Even though 
                 the function is a power-up function, it shuts-off power to 
                 SoC since none is present.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_powerup
(
  void
)
{
  /* Enable power */ 
#ifdef T_QSC60X5
  gpio_out(BT_PWR_EN, GPIO_HIGH_VALUE);
  BT_MSG_DEBUG( "BT SOC: Calling gpio_out - to turn on",0,0,0 );

  /* wait for vreg to turn ON */
  rex_sleep( 10 );
#else
#error code not present
#endif /* T_QSC60X5 */
}


/*==========================================================================

  FUNCTION       BT_SOC_INIT

  DESCRIPTION    Stub init function. Calls bt_soc_etc_init and outputs a log
                 message. Nothing needs to be done. Present more for 
                 compatibility reasons.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_init
(
  void
)
{
  BT_MSG_DEBUG( "BT SOC init stub:", 0, 0, 0 );

  bt_soc_etc_init();
}


/*==========================================================================

  FUNCTION       BT_SOC_PROC_SIGNALS

  DESCRIPTION    Stub signal processing function. Clears all the signals and
                 outputs a log message. Nothing needs to be done. Present 
                 more for compatibility reasons.

  DEPENDENCIES   None.

  PARAMETERS     rex_signals - mask of all the signals currently asserted.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_proc_signals
(
  rex_sigs_type  rex_signals
)
{
  BT_MSG_DEBUG( "BT SOC proc signals stub: Clearing all signals: %x", 
                rex_signals, 0, 0 );

  /* Clear all signals */
  rex_clr_sigs( &bt_tcb, BT_SOC_SIG_ALL );
}


/*============================================================================

  FUNCTION       BT_SOC_PROC_VS_EVENT

  DESCRIPTION    Stub VS events processing function just outputs a log 
                 message. Nothing needs to be done. Present more for 
                 compatibility reasons.

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr - vendor specific event data

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_proc_vs_event
(
  bt_ev_msg_type *ev_msg_ptr
)
{
  BT_MSG_DEBUG( "BT SOC Proc VS Event: Received VS Event", 0, 0, 0 );
}


/*============================================================================

  FUNCTION       BT_SOC_PROC_HCI_RESET

  DESCRIPTION    Stub HCI_RESET event processing function just outputs a log 
                 message. Nothing needs to be done. Present more for 
                 compatibility reasons.

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr - HCI_RESET event data

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_proc_hci_reset
(
  bt_ev_msg_type *ev_msg_ptr
)
{
  BT_MSG_DEBUG( "BT SOC: Received HCI RESET event in HCI mode", 0, 0, 0 );
}


/*============================================================================

  FUNCTION       BT_SOC_PROC_SHUTDOWN

  DESCRIPTION    Stub shutdown function turns the power off and outputs a log message.
                 Present more for compatibility reasons.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_shutdown
(
  void
)
{
  /* Diable power */ 
#ifdef T_QSC60X5
  gpio_out(BT_PWR_EN, GPIO_LOW_VALUE);
  BT_MSG_DEBUG( "BT SOC: Calling gpio_out - to turn off",0,0,0 );

  /* wait for vreg to turn OFF */
  rex_sleep( 10 );
#else
#error code not present
#endif /* T_QSC60X5 */
}


/*==========================================================================

  FUNCTION       BT_SOC_SLEEP

  DESCRIPTION    Stub sleep message function. Outputs a message indicating
                 an SoC sleep message was requested.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_sleep
(
  void
)
{
  BT_MSG_DEBUG( "BT SOC sleep stub: SoC sleep message requested", 0, 0, 0 );  
}


/*==========================================================================

  FUNCTION       BT_SOC_WAKEUP

  DESCRIPTION    Stub wakeup message function. Outputs a message indicating
                 an SoC wakeup message was requested.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_soc_wakeup
(
  void
)
{
  BT_MSG_DEBUG( "BT SOC wakeup stub: SoC wakeup message requested", 0, 0, 0 ); 
}


/*==========================================================================

  FUNCTION       BT_SOC_TOT_HC_TX_LEN

  DESCRIPTION    This routine returns the total buffer length available for
                 Tx in the SOC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
uint16 bt_soc_tot_hc_tx_len
(
  void
)
{    
#ifdef FEATURE_BT_QSOC
  return bt_qsoc_tot_hc_tx_len();
#elif (defined FEATURE_BT_SOC1)
#error code not present
#else
  return 0;
#endif /* FEATURE_BT_QSOC */  
}

#ifdef FEATURE_BT_WLAN_COEXISTENCE
/*==========================================================================

  FUNCTION      BT_SOC_READ_NV_WLAN_TYPE

  DESCRIPTION   Reads the OEM WLAN CHIP identifier from non volatile memory

  DEPENDENCIES  None.

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None.
==========================================================================*/
void bt_soc_read_nv_wlan_type
(
  void
)
{

  nv_cmd_type   rd_wlan_oem_nv_cmd;
  nv_item_type  nv_item;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read WLAN Chip id from NV */
  rd_wlan_oem_nv_cmd.item       = NV_WLAN_OEM_INDEX_I;
  rd_wlan_oem_nv_cmd.cmd        = NV_READ_F;
  rd_wlan_oem_nv_cmd.data_ptr   = &nv_item;
  rd_wlan_oem_nv_cmd.tcb_ptr    = &bt_tcb;
  rd_wlan_oem_nv_cmd.sigs       = BT_WAIT_SIG;
  rd_wlan_oem_nv_cmd.done_q_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );
  nv_cmd( &rd_wlan_oem_nv_cmd );
  (void) rex_wait( BT_WAIT_SIG );
  (void) rex_clr_sigs( rex_self(), BT_WAIT_SIG );

  if ( rd_wlan_oem_nv_cmd.status == NV_DONE_S )
  {
    bt_soc_wlan_type = (bt_soc_wlan_enum_type)nv_item.wlan_oem_index;

    BT_MSG_HIGH( "BT SOC: Read successfuly OEM WLAN Type, value: %x",
                 bt_soc_wlan_type, 0, 0 );
  }
  else if ( rd_wlan_oem_nv_cmd.status == NV_NOTACTIVE_S )
  {
    BT_ERR( "BT SOC: OEM WLAN TYPE NV inactive, Turn off WLAN-BT coex",
            0, 0, 0 );

    /* TBD */
    /* Turn off BT-WLAN coexistence */
  }
  else
  {
    BT_ERR( "BT SOC: Unable to read OEM WLAN TYPE from NV error code :%x",
            rd_wlan_oem_nv_cmd.status, 0, 0 );
  }

} /* bt_soc_read_nv_wlan_type */


/*==========================================================================

  FUNCTION      BT_SOC_GET_WLAN_TYPE

  DESCRIPTION   This function returns the OEM WLAN CHIP identifier used.

  DEPENDENCIES  None.

  PARAMETERS    None

  RETURN VALUE  WLAN chip type used

  SIDE EFFECTS  None.
==========================================================================*/
bt_soc_wlan_enum_type bt_soc_get_wlan_type
(
  void
)
{
  return( bt_soc_wlan_type );

} /* bt_soc_get_wlan_type */

#endif /* FEATURE_BT_WLAN_COEXISTENCE */


#endif /* FEATURE_BT_SOC */
#endif /* FEATURE_BT */
