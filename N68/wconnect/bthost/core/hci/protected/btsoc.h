#ifndef _BTSOC_H
#define _BTSOC_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_SOC

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          B L U E T O O T H    I N T E R N A L    H E A D E R

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/protected/btsoc.h#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $
*
*   #10       31 Jan  2008          BH
*   Added needed header files so this header file can be brought in from modules
*   other than btsoc.c.
*
*   #9        19 Dec  2007          JN
*   Added support for FEATURE_BT_SOC_BITRATE_115200.
*
*   #8        19 Oct  2007          SSK
*   Adding new constant BT_SOC_MAX_RX_PKT_SIZE.
*
*   #7        25 May  2007          DSN
*   Add support to read WLAN type from NV.
*
*   #6        25 Apr  2007           BH
*   Added FEATURE_BT_SOC_BITRATE_460800 for builds without high-speed UART.
*
*   #5        20 Apr  2007          DSN
*   Enable 3.2M UART baud for MSM7200A.
*
*   #4        05 Apr  2007           BH
*   Added stubs for bt_soc_sleep/wakeup.
*
*   #3        29 Mar  2007           WFD
*   For the MSM7200, the constants for baud rate are set to SIO_BITRATE_115200.
*
*   #2         14 Mar  2007          JN
*   Following mods:
*   - Removed "_stub" from function names.
*   - Added bt_soc_ifc_init to initialize bt_soc_ifc.
*   - Adde macros for sio baud rates.
*
*   #1         02 Mar  2007          JN
*   Created new file for BT SoC Abstraction Layer.

===========================================================================*/

/*===========================================================================
                         INCLUDE FILES FOR MODULE
===========================================================================*/
#include "rex.h"
#include "bt.h"
/*=========================================================================*/
/*                               CONSTANTS                                 */
/*=========================================================================*/
/* The MAX RX packet size for SOC based solutions which in turn determines both
   the SOC RX buffer size and the RX ACL DATA PKT Length */
#define BT_SOC_MAX_RX_PKT_SIZE                 0x0208  /* 520d */

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/

typedef enum
{
  BT_SOC_TYPE_NONE = 0,   /* None      */
  BT_SOC_TYPE_SOC1 = 1,   /* CSR's BC4 */
  BT_SOC_TYPE_QSOC = 2    /* QC SoC    */
} bt_soc_type_enum;

/* WLAN chip types */
typedef enum
{
  BT_SOC_WLAN_PHILIPS = 0,
  BT_SOC_WLAN_ATHEROS,
  /* .. Add here any more new WLAN Chip types.. */
  BT_SOC_WLAN_NONE
} bt_soc_wlan_enum_type;

typedef void (*bt_soc_powerup_func_ptr_type) (void);
typedef void (*bt_soc_init_func_ptr_type) (void);
typedef void (*bt_soc_sleep_func_ptr_type) (void);
typedef void (*bt_soc_wakeup_func_ptr_type) (void);
typedef void (*bt_soc_proc_soc_sigs_func_ptr_type) (rex_sigs_type  rex_signals);
typedef void (*bt_soc_proc_vs_event_func_ptr_type) (bt_ev_msg_type *ev_msg_ptr);
typedef void (*bt_soc_proc_hci_rst_func_ptr_type)(bt_ev_msg_type *ev_msg_ptr);
typedef void (*bt_soc_shutdown_func_ptr_type)(void);

typedef struct 
{
  bt_soc_powerup_func_ptr_type        bt_soc_powerup_func_ptr;
  bt_soc_init_func_ptr_type           bt_soc_init_func_ptr;
  bt_soc_sleep_func_ptr_type          bt_soc_sleep_func_ptr;
  bt_soc_wakeup_func_ptr_type         bt_soc_wakeup_func_ptr;
  bt_soc_proc_soc_sigs_func_ptr_type  bt_soc_proc_soc_sigs_func_ptr;
  bt_soc_proc_vs_event_func_ptr_type  bt_soc_proc_vs_event_func_ptr;
  bt_soc_proc_hci_rst_func_ptr_type   bt_soc_proc_hci_reset_func_ptr;
  bt_soc_shutdown_func_ptr_type       bt_soc_proc_shutdown_func_ptr;
} bt_soc_abs_struct_type;


/*=========================================================================*/
/*                           DATA DECLARATIONS                             */
/*=========================================================================*/

/* BT SoC Abstraction infterface */
extern bt_soc_abs_struct_type bt_soc_ifc;
extern bt_soc_type_enum       bt_soc_type;

/*=========================================================================*/
/*                                MACROS                                   */
/*=========================================================================*/
/* SIO Bitrates - both values below need to be changed for a change in 
 * the  bitrate 
 * NOTE: BT_SOC1_BR_PSKEY_VAL defined for SOC1 need to be changed for any 
 * change in bitrate */

#ifdef FEATURE_BT_SOC_BITRATE_460800
/* high baud rates are not supported, so use 460.8K */
#define BT_SOC_SIO_BITRATE                          SIO_BITRATE_460800
#define BT_SOC_SIO_INIT_BITRATE                     SIO_BITRATE_115200
#elif defined (FEATURE_BT_SOC_BITRATE_115200)
/* use 115.2K */
#define BT_SOC_SIO_BITRATE                          SIO_BITRATE_115200
#define BT_SOC_SIO_INIT_BITRATE                     SIO_BITRATE_115200
#else /* FEATURE_BT_SOC_BITRATE_115200 */
#define BT_SOC_SIO_BITRATE                          SIO_BITRATE_3200000
#define BT_SOC_SIO_INIT_BITRATE                     SIO_BITRATE_115200
#endif /* FEATURE_BT_SOC_BITRATE_460800 */

/*=========================================================================*/
/*                           FUNCTION INTERFACES                           */
/*=========================================================================*/

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
);


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
extern void bt_soc_powerup
(
  void
);


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
extern void bt_soc_init
(
  void
);


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
extern void bt_soc_proc_signals
(
  rex_sigs_type  rex_signals
);


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
extern void bt_soc_proc_vs_event
(
  bt_ev_msg_type *ev_msg_ptr
);


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
extern void bt_soc_proc_hci_reset
(
  bt_ev_msg_type *ev_msg_ptr
);


/*============================================================================

  FUNCTION       BT_SOC_PROC_SHUTDOWN

  DESCRIPTION    Stub shutdown function turns the power off and outputs a log message.
                 Present more for compatibility reasons.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_soc_shutdown
(
  void
);


/*==========================================================================

  FUNCTION       BT_SOC_SLEEP

  DESCRIPTION    Stub sleep message function. Outputs a message indicating
                 an SoC sleep message was requested.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_soc_sleep
(
  void
);


/*==========================================================================

  FUNCTION       BT_SOC_WAKEUP

  DESCRIPTION    Stub wakeup message function. Outputs a message indicating
                 an SoC wakeup message was requested.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern void bt_soc_wakeup
(
  void
);


/*==========================================================================

  FUNCTION       BT_SOC_TOT_HC_TX_LEN

  DESCRIPTION    This routine returns the total buffer length available for
                 Tx in the SOC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
extern uint16 bt_soc_tot_hc_tx_len
(
  void
);

#ifdef FEATURE_BT_WLAN_COEXISTENCE
/*==========================================================================

  FUNCTION      BT_SOC_READ_NV_WLAN_TYPE

  DESCRIPTION   Reads the OEM WLAN CHIP identifier from non volatile memory

  DEPENDENCIES  None.

  PARAMETERS    None

  RETURN VALUE  None

  SIDE EFFECTS  None.
==========================================================================*/
extern void bt_soc_read_nv_wlan_type
(
  void
);

/*==========================================================================

  FUNCTION      BT_SOC_GET_WLAN_TYPE

  DESCRIPTION   This function returns the OEM WLAN CHIP identifier used.

  DEPENDENCIES  None.

  PARAMETERS    None

  RETURN VALUE  WLAN chip type used

  SIDE EFFECTS  None.
==========================================================================*/
extern bt_soc_wlan_enum_type bt_soc_get_wlan_type
(
  void
);

#endif /* FEATURE_BT_WLAN_COEXISTENCE */
#endif /* FEATURE_BT_SOC */
#endif /* FEATURE_BT */
#endif /* _BTSOC_H*/
