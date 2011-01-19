#ifndef _BTHCDRV_H
#define _BTHCDRV_H

#ifdef FEATURE_BT_SOC

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            B L U E T O O T H    H O S T   C O N T R O L L E R  
              I N T E R F A C E   D R I V E R   H E A D E R

DESCRIPTION
  This contains the highest level header for the for the HCI 
  (Host-Controller Interface) driver for the Bluetooth host.


Copyright (c) 2006-2007 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/protected/bthcdrv.h#1 $
 $DateTime: 2009/01/07 18:14:54 $$Author: deepikas $
*
*    #15      05 Dec 2007            RB
* Remove function prototype that is local to bthcdrv.c
*
*    #14      29 Nov 2007            SSK
* Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
*
*    #13       01 Nov 2007            AG
* Merged Lisbon(BT2.1) changes into the mainline
*
*    #12      19 Oct 2007            DSN/SSK
* Add support for Controller to Host flow control.
*
*    #11      27 Jun 2007            DM
* Changed featurization for AV-Sync in SOC based targets
*
*    #10      13 Apr 2007            JN
* Remove externalization for bt_hc_drv_powerup_init() function.
*
*    #9       01 Mar 2007            DSN
* Use HCI Max ACL data length from HCI Read Buffer Size command response.
*
*    #8       20 Jan 2007           DSN
* Extern bt_hc_drv_process_bytes_from_sio() function
*
*    #7       09 Feb 2007            DSN
* Fix for compile warnings.
*
*    #6       08 Jan 2007            ACG
* Following features were added for the SOC solution:
* - vote for SIO sleep/wakeup based on BT activity
* - enable/disable driver
* - radio on/off
* 
*   #5        22 Nov 2006            DM
* Added Flush mechanism for AV-Sync support on SoC.
*
*   #4        16 Nov 2006            DM
* Added Tx Complete Mechanism for AV-Sync Support on SoC.
*
*   #4        06 Sep 2006            DSN
* Removed the export of bt_hc_drv_send_acl_data.
*
*   #3        20 Oct 2006            SSK
* External SOC suppport.
*
*   #2        01 May 2006            DSN
* Added  Controller flow control for a single connection.
*
*   #1        27 Mar 2006            DSN
* Created file.

==========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "bt.h"
#include "btcmdi.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*               Extern Function Declarations for BT module                */
/*                                                                         */
/*-------------------------------------------------------------------------*/


/*==========================================================================
                          
FUNCTION       
  BT_HC_DRV_SET_HCI_BUF_SIZES

DESCRIPTION    
  Updates the parameters returned from Read Buffer Size command to the 
  HCI driver structures.

DEPENDENCIES   
  None.

INPUTS         
  acl_data_pkt_len       - Max length of ACL data packet in bytes
  sco_data_pkt_len       - Max length of SCO over HCI packet in bytes
  tot_num_acl_data_pkts  - Total number of ACL data packets
  tot_num_sco_data_pkts  - Total number of SCO data packets

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_set_hci_buf_sizes
(
  /* Max length of ACL data packet in bytes */
  uint16 acl_data_pkt_len,

  /* Max length of SCO over HCI packet in bytes */
  uint8  sco_data_pkt_len,

  /* Total number of HCI ACL Data Packets */
  uint16 tot_num_acl_data_pkts,

  /* Total number of HCI SCO Data Packets */
  uint16 tot_num_sco_data_pkts
);


/*==========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_CMD

DESCRIPTION    
  Processes HCI commands and ACL data to be sent to Host Controller.
  Adds HCI header and sends HCI Command and ACL  packets to host controller. 
  Note SCO over HCI is not handled now.

DEPENDENCIES   
  SIO port to communicate with Host controller must be open.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/

extern void bt_hc_drv_process_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
);


/*==========================================================================

FUNCTION
  BT_HC_DRV_FCTL_PROCESS_CMD

DESCRIPTION    
  Processes HCI Flow control commands that take appropriate flow control
  action based on DSM Item pools levels.
  
DEPENDENCIES   
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
extern void bt_hc_drv_fctl_process_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
);


/*==========================================================================

FUNCTION
  BT_HC_DRV_SEND_CMD

DESCRIPTION
  Send the HCI comamnd to host controller bypassing the BT Queue.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
extern void  bt_hc_drv_send_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
);

/*==========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_CMD_PREAMBLE

DESCRIPTION    
  Before messages are sent to the host controller, make sure
  SOC is up. If it is not, call bt_dc_enable_driver to bring
  it up first.

DEPENDENCIES   
  None.

INPUTS
  None.  

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
void bt_hc_drv_process_cmd_preamble(void);

/*==========================================================================

FUNCTION
  BT_HC_DRV_CMD

DESCRIPTION
  Send the HCI comamnd to host controller bypassing the BT Queue.

DEPENDENCIES
  None.

INPUTS
  Pointer to the BT command structure.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
extern void bt_hc_drv_cmd
(
  bt_cmd_msg_type*  bt_cmd_ptr
);

/*===========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_HCI_RX

DESCRIPTION    
  Processes bytes received from Host controller SOC to Host via SIO.

DEPENDENCIES   
  None.

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

===========================================================================*/
extern void bt_hc_drv_process_hci_rx( void );

/*===========================================================================

FUNCTION
  bt_hc_drv_background_tick

DESCRIPTION
  Performs background monitoring for HCI.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void bt_hc_drv_background_tick( void );

/*===========================================================================

FUNCTION
  bt_hc_drv_soc_init

DESCRIPTION
  Initializes the SOC Driver.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void bt_hc_drv_soc_init( void );

/*===========================================================================

FUNCTION
  BT_HC_DRV_SOC_SHUTDOWN

DESCRIPTION
  Shuts down the SOC driver.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void bt_hc_drv_soc_shutdown( void );

/*===========================================================================

FUNCTION
 bt_hc_drv_sio_transmit

DESCRIPTION
  Processes bytes received from Host controller SOC to Host via SIO.

DEPENDENCIES
  None.

INPUTS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void bt_hc_drv_sio_transmit
(
  void* buf,
  uint32 data_len
);

/*===========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_BYTES_FROM_SIO

DESCRIPTION    
  Process bytes coming from Host Controller SOC to Host via SIO.

DEPENDENCIES   
  None.

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

===========================================================================*/
extern void bt_hc_drv_process_bytes_from_sio( void );


#ifdef FEATURE_BT_AVSYNC_SOC
/*==========================================================================

FUNCTION       
  BT_HC_DRV_ENQUE_ACL_DATA

DESCRIPTION    
  Enques ACL data to be transmitted to Controller. This only enques the 
  packet header information so as to match a Num Complete and send 
  filer only necessary num completes to the Host Stack.

DEPENDENCIES   
  None.

INPUTS
  BT Cmd message with ACL Data.
  Index to ACL Connection Handle in HC Driver

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_enque_acl_data
( 
  bt_cmd_msg_type* bt_cmd_ptr,
  uint8            c_hndl_idx
);

/*==========================================================================

FUNCTION       
  BT_HC_DRV_UPDATE_TX_STATUS

DESCRIPTION    
  Updates the Tx Status for the Header Element to True when this ACL data is 
  sent to the SoC.

DEPENDENCIES   
  None.

INPUTS
  Index to ACL Connection Handle in HC Driver

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_update_tx_status( uint8 c_hndl_idx );


/*==========================================================================
                          
FUNCTION       
  BT_HC_DRV_HC2H_FLOW_SUPPORT_STATUS

DESCRIPTION    
  This functions updates whether the Controller supports the Host Controller 
  to Host flow control.

DEPENDENCIES   
  None.

INPUTS         
  HC2H flow control supported status

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/

extern void bt_hc_drv_hc2h_flow_support_status
( 
  /* HC2H flow control supported status */
  uint8  status
);


/*==========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_AVSYNC_NUM_CMPLT

DESCRIPTION    
  Processes the Num Completes received from the SoC, De-ques the completed ACL
  Packet headers and generates a Tx-Cfm Event to RM for ACL packets that require
  an acknowledgement to be generated.

DEPENDENCIES   
  None.

INPUTS
  Index to ACL Connection Handle in HC Driver
  The ACL Connection Handle for this ACL Connection
  The number of completed packets in this ACL Link.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_process_avsync_num_cmplt ( 
  uint8           c_hndl_idx, 
  uint16          conn_hndl,
  uint16          num_cmplt_pkts
);

/*==========================================================================

FUNCTION       
  BT_HC_DRV_EMPTY_AVSYNC_HEADER_Q

DESCRIPTION    
  Empty the Queued Packet headers and generates a Tx-Cfm Event to RM 
  for ACL packets that require an acknowledgement to be generated.

DEPENDENCIES   
  None.

INPUTS
  Index to ACL Connection Handle in HC Driver
  The ACL Connection Handle for this ACL Connection

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_empty_avsync_header_q
(
  uint8     c_hndl_idx, 
  uint16    conn_hndl
);

/*==========================================================================

FUNCTION       
  BT_HC_DRV_L2FLUSH_TIMER_TICK

DESCRIPTION    
  Tick timer toutine to monitor ACL packets that has stayed in the HC beyond 
  a pre-configured timeout period (Flush timeout).
  Only if all the packets are flushable (Media) then a HCI Flush command 
  is generated to the Controller.

DEPENDENCIES   
  None.

INPUTS
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_l2flush_timer_tick
(
  void
);

#endif /* FEATURE_BT_AVSYNC_SOC */

/*==========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_CMD_FROM_WM

DESCRIPTION    
  This funtion processes HCI cmds stored on to the WM when the HCI
  driver is ready to send out anothher HCI cmd.

DEPENDENCIES   
  None.

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_process_cmd_from_wm
(
 void
);


/*==========================================================================

FUNCTION       
  BT_HC_DRV_PROCESS_ACL_FROM_WM

DESCRIPTION    
  This function processes ACL data stored on to the WM when the HCI
  driver is ready to send out more ACL data.

DEPENDENCIES   
  None.

INPUTS         
  None.

RETURN VALUE   
  None.

SIDE EFFECTS   
  None.

==========================================================================*/
extern void bt_hc_drv_process_acl_from_wm
(
 void
);

#endif /* FEATURE_BT_SOC */

#endif /* _BTHCDRV_H */
