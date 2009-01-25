
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    B L U E T O O T H   B T S   D R I V E R   M O D U L E

GENERAL DESCRIPTION
  This module contains the Bluetooth BTS SoC driver data and code.

EXTERNALIZED FUNCTIONS

  bt_qsoc_powerup()
    Performs power-on sequence for BTS.

  bt_qsoc_init()
    Starts BTS configuration.

  bt_qsoc_process_signals()
    Processes BTS specific signals.

  bt_qsoc_process_vs_events()
    Process BTS' vendor specific events.

  bt_qsoc_process_hci_reset()
    Process HCI_RESET event coming back from BTS.

  bt_qsoc_shutdown()
    Performs power-off sequence for BTS.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007-2011 by QUALCOMM Incorporated. All Rights Reserved.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order. Please
  use ISO format for dates.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/hci/src/btqsoc.c#9 $
  $DateTime: 2011/01/17 23:23:57 $
  $Author: roystonr $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-10-25   tw  Added support for 4025 B3
  2010-05-06   us  Disbale Scans when intiating Remote Name request or
                   Create Connection for some SOC versions.
  2009-12-14  dgh  Fixed minor string copy bug in bt_qsoc_detect_fw_version().
  2009-08-07  dgh  Added 4025 B2 support.
  2009-04-27   vk  Enabled pull down on VREG supplying power to BTS
  2009-03-04   jn  Enable clock sharing feature as well as voting for 
                   VREG_GP2 on QSC6240/70.
  2009-01-14   sp  Disable LPM mode for VREG_GP1, VREG_GP2 for QSC6270
  2008-12-22   rb  Fixes for clock being turned on during BT sleep on ULC
  2008-11-14   sa  Support for NV Automation.
  2008-11-05   pg  Generic handling for HCI Vendor Specific cmd group.
  2008-10-27   sa  Added functionality to read SOC hw version register
                   (Currently this is used to differentiate BT 4020 BD B1 from 
		    BT 4020 DB B0, the cmd BT_QSOC_VS_EDL_GETAPPVER_REQ 
		    response alone cannot distinguish BT 4020 BD B1 from B1).  
  2008-10-27   sa  Added support to read BT 2.1 support from a nv item in 
                   NV browser at run-time.
  2008-10-23   rb  Added support for reset line on keypads with 4025 on 6290
  2008-09-06   ap  Added support for setting BTS voltage level for Q6270 B1 MSM
  2008-08-20   rb  Added support for 4025 B1
  2008-08-20   jn  Enable clock sharing on QSC1110.
  2008-08-07   sp  T_QSC6270 feature changed instead of T_MSM6270
  2008-08-06   sp  Enabling pull down on VREG supplying power to BTS for QSC6240/70
  2008-07-31   sp  Set VREG_GP1 to 2.6V for QSC6240 and QSC6270
  2008-07-28   sa  Added support to read Clk type,Clk sharing,in-band sleep mode, 
                   soc logging enabled, BT Wlan co-ext, BTS power mgmt mode  
		           from NV browser at run-time.
  2008-07-28   sa  Added support for voting against shutting BT 32K sleep clk 
                   by calling pmic API (pm_vote_clk_32k).
  2008-07-28   ssk Merging 'Array Index out of bounds' KlocWork error
                   fixes from the 2007.11 Depot branch.
  2008-07-28   jn  Fix EOL issue.
  2008-07-15   jn  Enable pull down on VREG supplying power to BTS for 6290/46.
  2008-07-03   jn  Remove flow enable call before powering VREG for 6290/6246.
  2008-06-13   jn  Support QSOC_POWERDOWN feature for 6290/6246.
  2008-05-15   rb  Support for 4025 B0.
  2008-05-06   rb  Added support for 4025 A0
  2008-04-02   jn  Fix BT powerup/shutdown on QSC6270.
  2008-03-13   rb  Added support for 4021 B1 and 4020 BD B0
  2008-03-13   jn  Vote for VDD minimization of QSOC init fails. Add QSC6270 
                   featurization for powerup.

*
*   #43         12 Mar 2008          JN
*   Vote for VDD mimuimization if SoC initialization fails.
*
*   #42         27 Feb 2008          SSK
*   Optimise Inband SM to enter TX BREAK after waiting for TX path to go idle.
*
*   #41         31 Jan 2008          BH
*   Incorporated a shared, target independant NVM file.
*
*   #40         29 Jan 2008          RB
*   Disable logging for R4 devices on FFAs
*
*   #39         10 Jan 2008          SSK
*   Fix for "SoC fails to come up after it is powered OFF in Inband sleep enabled
*   builds" and incorporate code review comments from code collab Review #19060
*
*   #38         21 Dec 2007          BH
*   Corrected EXT_WAKE and HOST_WAKE signals for when QSoC power-down doesn't
*   actually power down SoC.
*
*   #37         19 Dec 2007          JN
*   Added support for FEATURE_BT_SOC_BITRATE_115200. Modified NVM tables, 
*   inband sleep feature and various bit rate features (460800, 115200)
*   are not mutually exclusive.
*
*   #36         14 Dec 2007          BH
*   Corrected R4 Lisbon NVM counter and removed previous checkin as it is no
*   longer necessary.
*
*   #35         06 Dec 2007          JN
*   Defined a macro to fix compile errors due difference in strct name on
*   some targets.
*
*   #34         29 Nov 2007          SSK
*   Implemented Inband Sleep mechanism under FEATURE_BT_QSOC_INBAND_SLEEP
*
*   #33         27 Nov 2007          BH 
*   Fixed an issue where the HOST_WAKE ISR could occur while we are handling it
*   and cause the sleep state machine to get out of sync.
*
*   #32         26 Nov 2007          BK
*   Only do powerup sequence once if FEATURE_BT_QSOC_POWERDOWN is not defined.
*
*   #31         21 Nov 2007          BH 
*   Updated BD NVMs and added the clock request tags for 19.2 builds.
*   -------- New Bump-Die NVM version ---------
*    19.2 Mhz Clock: 07.0A (11/09/2007)
*    32 Mhz Clock: 0D.09 (11/08/2007)
*
*   #30         15 Nov 2007          RB 
*   Control the 4020 power source based on target 
*
*   #29         06 Nov 2007          BH
*   Disabled Park Mode feature bit for R3, BD, 
*   and R4 (including the Lisbon version).
*
*   #28         01 Nov 2007          AG
*   Write HW version to enable Lisbon(BT2.1)
*
*   #27         01 Nov 2007          AG
*   Merged Lisbon(BT2.1) changes into the mainline
*
*   #26         19 Oct 2007          SSK
*   Redefining BT_QSOC_MAX_HC_RX_LEN to BT_SOC_MAX_RX_PKT_SIZE.
*
*   #25         16 Oct 2007          BH
*   Updated NVMs for R3, Bump-Die, and R4.  Each are now at the 
*   following versions:
*   ----------- R3 ------------
*    19.2 Mhz Clock: 07.0E (10/15/2007)
*    32 Mhz Clock: 0D.0F (10/15/2007)
*   -------- Bump-Die ---------
*    19.2 Mhz Clock: 07.06 (09/07/2007)
*    32 Mhz Clock: 0D.06 (09/07/2007)
*   ----------- R4 ------------
*    Automatic Switching Front End
*     - 19.2 Mhz Clock: 87.01 (08/31/2007)
*     - 32 Mhz Clock: 8D.01 (08/31/2007)
*    LDO Front End Regulator
*     - 19.2 Mhz Clock: 07.01 (08/31/2007)
*     - 32 Mhz Clock: 0D.01 (08/31/2007)
*
*   #24         26 Sep 2007          BH
*   For R4 and Bump-Die, fixed an issue in bt_qsoc_process_vs_cmd_cmplt_event 
*   where the retry signal was not getting cleared.
*
*   #23         12 Sep 2007          BK
*   For support of QSOC Driver State, add new FEATURE_BT_QSOC_POWERDOWN
*   which can disable actual powering off of the QSOC.
*
*   #22         10 Sep 2007          VK
*   Fixing Command Buffer leakage when sending HCI commands directly
*   to HCI driver.
*
*   #21         11 Sep 2007          BH
*   Unsupported versions of BTS no longer ERR_FATAL, but quietly
*   shutdown. Also added WLAN-coexistence NVMs for R3 BD and R4.
*   Modified R3 tag 41 for both 19.2 and 32 Mhz for improved performance.
*
*   #20         04 Sep 2007          BH
*   Added R3 Bumped-Die and R4 support.
*
*   #19         21 Aug 2007          BH
*   Adding NVM tag #36 to skip over the lowest power mode.
*
*   #18         17 Aug 2007          BH
*   Updated Patch #64 and Poke patch to fix an issue with RF test mode.
*
*   #17         03 Aug 2007          BH
*   Fixed incorrect packet length in R3 NVM tags 41 & 57.
*
*   #16         25 Jul 2007          BH
*   Updated R3 patches, R3 NVM tag #41, and added R3 poke patch.  
*   Also changed NXP PTA_Status pin from GPIO_1 to GPIO_2.
*
*   #15         29 Jun 2007          BH
*   Fixed WLAN Coex typo so the correct length will be used for
*   for bt_qsoc_nvm_53_nxp.
*
*   #14         06 Jun 2007          DSN
*   Support to get Encryption key Length from QSOC .
*
*   #13         29 May  2007         JN
*   Update 4020 R3 configuration to improve RF performance.
*
*   #12         25 May  2007         DSN
*  Updated BT-WLAN coexixtence related NVM tags.
*
*   #11         25 Apr  2007         BH
*   Following mods
*   - Added FEATURE_BT_SOC_BITRATE_460800 for low-speed UART targets.
*   - BT/WLAN coexistence related code is now under the new feature
*     FEATURE_BT_WLAN_COEXISTENCE.
*   - Added some additional sleep cleanup code for bt_qsoc_shutdown
*     and bt_qsoc_powerup.
*
*   #10        27 Apr  2007          JN
*   Added BTS4020 configuration (patches) to fix flush error (R2B/R2C) 
*   and pairing not allowed error code (R2C/R3).
*
*   #9         20 Apr  2007          DSN
*   Enable 3.2M UART baud for MSM7200A.
*   Change Tag 41 for better R2B RF calibration.
*
*   #8         13 Apr  2007          JN
*   Use bt_dc_sio_change_baud() to change baud rate. Modify BTS R2C and R3 
*   configs.
*
*   #7         04 Apr  2007          BH
*   Added QSoC sleep initialization, signal processing and sleep/wake
*   message functions.
*
*   #6         02 Apr  2007          AP
*   Make qsoc init state a global var. 
*
*   #5         29 Mar  2007          WFD
*   Use a lower baud rate (115K) for MSM7200.
*   This requires NVM #17 baud rate parameter to be different, and
*   for NVM #47 not be called when compiled 7200.
*
*   #4         16 Mar  2007          JN
*   Fix a bug fault-tolerance mechanism for HCI_RESET.
*
*   #3         15 Mar  2007          JN
*   Fix typo.
*  
*   #2         14 Mar  2007          JN
*   Following mods
*    - Added robustness for "no response to HCI_RESET" scenario.
*    - Changed buffer sizes to be macros from literals.
*    - Change post HCI_RESET send processing out of interrupt context
*      i.e. out of bt_qsoc_sio_flush_tx_hci_reset().
*
*   #1         02 Mar  2007          JN
*   Created new file for BTS 4020 (QSoC) support.

===========================================================================*/
/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/********************* PUBLIC INCLUDE FILES ********************************/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_QSOC


#include <stdio.h>
#include <string.h>

#include "dsm.h"
#include "rex.h"
#include "pm.h"
#include GPIO_H

#include "bt.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "bthc.h"
#include "bthcdrv.h"
#include "bti.h"
#include "btlogi.h"
#include "btmsg.h"
#include "btutils.h"
#include "btsocetc.h"
#include "btsoc.h"

#ifdef T_QSC6270
#include "hw_api.h"
#endif /* T_QSC6270 */

#ifdef FEATURE_BT_QSOC_SLEEP
#include "btqsocsleep.h"
#include "gpio_int.h"
#endif /* FEATURE_BT_QSOC_SLEEP */

/* pull in shared header file */
#include "btqsocnvm.h" 
#include "btqsocnvmplatform.h"     
         
/********************* PRIVATE INCLUDE FILES********************************/

#include "btqsoc.h"


/*===========================================================================
             DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.
===========================================================================*/

/*=========================================================================*/
/*                      MACROS FOR BTS Power Control                       */
/*=========================================================================*/

/* Defines to use the correct power source to 4020 during power-up and
 * shutdown based on the target.
 */ 

#if (defined(T_MSM6246)) || (defined(T_MSM6290))

#define BT_PM_PWR_SRC          PM_VREG_WLAN_ID
#define BT_PM_APP_VOTE         PM_VOTE_VREG_WLAN_APP__BT 

#elif defined(T_QSC6270)

extern pm_err_flag_type pm_dvt_out(byte reg, byte val);

static pm_vote_vreg_app_type   bt_vreg_vote_gp1;
static pm_vote_vreg_app_type   bt_vreg_vote_gp2;

#define BT_PM_PWR_SRC          PM_VREG_GP1_ID
#define BT_PM_GP1_VOTE         bt_vreg_vote_gp1
#define BT_PM_GP2_VOTE         bt_vreg_vote_gp2

#else

#define BT_PM_PWR_SRC          PM_VREG_BT_ID
#define BT_PM_APP_VOTE         PM_VOTE_VREG_BT_APP__BT

#endif /* (defined(T_MSM6246)) || (defined(T_MSM6290)) */

#ifdef FEATURE_BT_QSOC_GPIO_WAKE
/*=========================================================================*/
/*                    MACROS FOR BTS Sleep GPIO Values                     */
/*=========================================================================*/
#define BT_QSOC_HOST_WAKE_ASSERTED          GPIO_LOW_VALUE
#define BT_QSOC_HOST_WAKE_DEASSERTED        GPIO_HIGH_VALUE
#define BT_QSOC_EXT_WAKE_ASSERTED           GPIO_LOW_VALUE
#define BT_QSOC_EXT_WAKE_DEASSERTED         GPIO_HIGH_VALUE

#define BT_QSOC_HOST_WAKE_ISR_ASSERTED      ACTIVE_LOW
#define BT_QSOC_HOST_WAKE_ISR_DEASSERTED    ACTIVE_HIGH

#endif /* FEATURE_BT_QSOC_GPIO_WAKE */

/*=========================================================================*/
/*                      MACROS FOR BTS NVM/VS Processing                   */
/*=========================================================================*/

#define BT_QSOC_MAX_RETRY_COUNT      3
#define BT_QSOC_MAX_PWRUP_RTRY_CNT   BT_QSOC_MAX_RETRY_COUNT

/* Signals that QSoC/BTS uses
 * NOTE: Signal BT_QSOC_HCI_RESET_FLUSH_SIG can be reused once the 
 * initialization is complete.
 */
#define BT_QSOC_RETRY_SIG            BT_SOC_SIG1 /* 0x00000004 */
#define BT_QSOC_HCI_RESET_FLUSH_SIG  BT_SOC_SIG2 /* 0x00000100 */


#define BT_QSOC_RESP_TIMEOUT         1000        /* 1000ms, 1 sec */

#define BT_QSOC_NVTBL_BDADDR_IDX     0x00  /* BD Addr entry index in nvtbl   */
#define BT_QSOC_NVTBL_TAG53_IDX      0x01  /* Tag53 index in nvtbl   */
#define BT_QSOC_NVTBL_TAG55_IDX      0x02  /* Tag55 index in nvtbl   */
#define BT_QSOC_NVTBL_BDADDR_OFF     0x03  /* BD Addr offset within cmd data */
#define BT_QSOC_MAX_NVM_CMD_SIZE     0x64  /* Maximum size config (NVM) cmd  */
#define BT_QSOC_EVT_SUB_POS          0x00  /* Position of the sub-event code */
#define BT_QSOC_MAX_VS_POKE8_SIZE    0xF0  /* Maximum length of poke8 payload */


/* Number of bytes to read the connection database address - 4 bytes for Address
   1 byte at the end to indicate that it is ACL connection database */
#define BT_QSOC_PEEK8_ADDR_LEN           4
#define BT_QSOC_CDB_ADDR_NUM_BYTES       5
#define BT_QSOC_ENC_KEY_LEN_NUM_BYTES    1

/* Convert connection handle map to connection database address */
#define BT_QSOC_CONN_HNDL_MAP_2_ADDR( map, chndl ) \
          (map + ((chndl & 0x0FFF) << 3))


#define BT_MSG_LAYER                 BT_MSG_GN   /* necessary for btmsg.h */

/*=========================================================================*/
/*                               TYPEDEFS                                  */
/*=========================================================================*/

/* Structure for peeking the connection database on QSOC */
typedef struct
{
  uint16   conn_hndl;    /* ACL connection handle */
  uint32   conn_db_addr; /* Address of Connection database */
  uint32   conn_db_ptr;  /* Pointer to the connection database */
  uint8    enc_key_len;  /* Encryption key length */
} bt_qsoc_conn_db_struct_type;

/* Structure for storing BT QSOC related information. */
typedef struct
{
  bt_qsoc_enum_type           bt_qsoc_type;           /* QSOC ROM version */
  bt_qsoc_conn_db_struct_type conn_db[BT_MAX_BT_DEV]; /* Connection database 
                                                         information */
} bt_qsoc_struct_type;


/*=========================================================================*/
/*                           DATA DECLARATIONS                             */
/*=========================================================================*/

/* Connection Handle map on the QSOC */
uint32 bt_qsoc_conn_hndl_map[BT_QSOC_MAX] = { 0x00000874, /* R2B */
                                              0x00000874, /* R2C */
                                              0x00000ba4,  /* R3 */
                                              0x00000ba4,  /* R3BD */
                                              0x00000ba4   /* R4 */
                                            };

/* Connection's encryption key offset on the QSOC */
uint32 bt_qsoc_conn_key_offset[BT_QSOC_MAX] = { 0x8a, /* R2B */
                                                0x8a, /* R2C */
                                                0x90,  /* R3 */
                                                0x90,  /* R3BD */
                                                0x90   /* R4 */
                                              };

/* Fixed configuration table for BTS' NVM. The elements of this table are 
 * pretty much hardcoded. They do not have any dependency on NVM or customer. 
 */

static bt_qsoc_runtime_params_struct_type bt_qsoc_vs_nvm_cmd_tbl;


static uint8                     bt_qsoc_fw_ver[0xFF];
static uint8                     bt_qsoc_conf_tbl_idx;
static uint8                     bt_qsoc_retry_count;     /* keeps track of 
                                                             # of reties */                                                          
static uint8                     bt_qsoc_pwrup_rtry_cnt;  /* # powerup tries*/
rex_timer_type                   bt_qsoc_retry_timer;


static uint8                     bt_qsoc_r3_poke_tbl_idx;   /* R3 Poke8 index */

/* Structure to hold any BT QSOC specific information */
static bt_qsoc_struct_type       bt_qsoc;

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
rex_timer_type                   bt_qsoc_inband_rx_flush_timer;
rex_timer_type                   bt_qsoc_inband_tx_flush_timer;

/* Flag that indicates 4020 Flow off\on state after Power Reset */
static boolean                   bt_qsoc_inband_soc_flowed_on1 = TRUE;  
/* Flag that indicates 4020 Flow off\on state after HCI Reset */
static boolean                   bt_qsoc_inband_soc_flowed_on2 = TRUE;

#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

/* Pointer to tag for dynamic parsing */
static bt_qsoc_cfg_cst_element_type *nvm_ptr_dyn = NULL;
uint8 hw_ver_reg_id[5];
const uint8 bt_qsoc_hw_ver_reg_4020_BD_B0 [] = { 0x05, 0x00, 0x00, 0x00 };
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
/*=========================================================================*/
/*                  Customer NVM table support                             */
/*=========================================================================*/
/* #34 Serial Number - Device serial number (This is an example)
*/
static bt_qsoc_cfg_cst_element_type bt_qsoc_cfg_tag_34_customer[] = \
{
  0x06, 0x01, 0x20, 0x03, 
  0x01, 0x02, 0x03
};
#define CFG_TAG_34_CUSTOMER (&bt_qsoc_cfg_tag_34_customer[0]) 

#define SIZEOF_BT_QSOC_CFG_TBL_CUSTOMER \
        ((sizeof(bt_qsoc_target_vs_nvm_cmd_custtbl))/(sizeof(bt_qsoc_cfg_cst_element_type*)))

/* Customer NVM table.  This is only an example, but can be rewritten to include
   specific NVMs needed by a customer.
*/
static bt_qsoc_cfg_cst_element_type* bt_qsoc_target_vs_nvm_cmd_custtbl[] = \
{
  CFG_TAG_34_CUSTOMER
};
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */

/*===========================================================================
                              Globals
===========================================================================*/
bt_qsoc_init_st_enum_type bt_qsoc_init_state;


/*===========================================================================
                       State Init/Cleanup Functions
===========================================================================*/

/*===========================================================================
                         FUNCTION DEFINITIONS
===========================================================================*/
void bt_qsoc_send_vs_cmds(void);
/*===========================================================================
                       Internal Functions
===========================================================================*/

#ifdef FEATURE_BT_QSOC_SLEEP
/*==========================================================================

  FUNCTION       BT_QSOC_SLEEP

  DESCRIPTION    This routine sends a sleep message to the SoC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_sleep
(
  void
)
{

#ifdef FEATURE_BT_QSOC_GPIO_WAKE
  gpio_out( EXT_WAKE, BT_QSOC_EXT_WAKE_DEASSERTED );
#elif defined( FEATURE_BT_QSOC_INBAND_SLEEP )
  /*  We do nothing here as TX BREAK is already set to allow  SOC sleep */ 
#endif /* FEATURE_BT_QSOC_GPIO_WAKE */

} /* bt_qsoc_sleep */


/*==========================================================================

  FUNCTION       BT_QSOC_WAKEUP

  DESCRIPTION    This routine sends a wake message to the SoC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_wakeup
(
  void
)
{

#if defined( FEATURE_BT_QSOC_GPIO_WAKE )
  gpio_out( EXT_WAKE, BT_QSOC_EXT_WAKE_ASSERTED );
#elif defined( FEATURE_BT_QSOC_INBAND_SLEEP )
  /*  We do nothing here as TX UNBREAK is already set to make  SOC  wakeup */ 
#endif /* FEATURE_BT_QSOC_GPIO_WAKE */

} /* bt_qsoc_wakeup */


#ifdef FEATURE_BT_QSOC_GPIO_WAKE
/*==========================================================================

  FUNCTION       BT_QSOC_HOST_WAKE_ISR

  DESCRIPTION    Callback function to handle changes in the HOST_WAKE signal.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_host_wake_isr
(
  void
)
{
  bt_qsoc_sleep_msm_state_type msm_state;
  GPIO_ValueType host_wake_level;

  host_wake_level = gpio_in( HOST_WAKE );

  if (host_wake_level == BT_QSOC_HOST_WAKE_ASSERTED)
  {    
    bt_qsoc_sleep_sm_eval_event( BT_QSOC_SLEEP_EVENT_HOST_WAKE_ENABLED,
                                 FALSE );

    msm_state = bt_qsoc_sleep_sm_get_msm_state();    
  }
  else
  {    
    bt_qsoc_sleep_sm_eval_event( BT_QSOC_SLEEP_EVENT_HOST_WAKE_DISABLED,
                                 FALSE );

    msm_state = bt_qsoc_sleep_sm_get_pending_msm_state();
  }

  /* If the MSM can sleep, signal BT Task to do so */
  if (msm_state == BT_QSOC_SLEEP_MSM_STATE_ASLEEP ||
      msm_state == BT_QSOC_SLEEP_MSM_STATE_SLEEP_WAIT)
  {
    (void) rex_set_sigs( &bt_tcb, BT_QSOC_SLEEP_SIG );
  }

    /* 
     The gpio_int_set_handler function MUST come at the end of 
     this function because this function unmasks this ISR and allows 
     for re-entry. 
     */
  if (host_wake_level == BT_QSOC_HOST_WAKE_ASSERTED)
  {
    /* Set interrupt for when HW is de-asserted */
    (void) gpio_int_set_handler( HOST_WAKE_INT, 
                         BT_QSOC_HOST_WAKE_ISR_DEASSERTED,
                         bt_qsoc_host_wake_isr ); 
  }
  else
  {
    /* Set interrupt for when HW is asserted */
    (void) gpio_int_set_handler( HOST_WAKE_INT,
                         BT_QSOC_HOST_WAKE_ISR_ASSERTED, 
                         bt_qsoc_host_wake_isr ); 
  }
  
} /* bt_qsoc_host_wake_isr */
#endif /* FEATURE_BT_QSOC_GPIO_WAKE */
#endif /* FEATURE_BT_QSOC_SLEEP */


/*==========================================================================

  FUNCTION       BT_QSOC_DETECT_FW_VERSION

  DESCRIPTION    Detect BTS firmware version based on the response to fw
                 version command. Goes on to set config table pointer 
                 and size of the table appropriately.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   TRUE (NVM init success) or FALSE (NVM init failure).

  SIDE EFFECTS   Set pointer for fixed table. Pointer is NULL if no matching
                 BTS detected.

==========================================================================*/
static boolean bt_qsoc_detect_fw_version
(
  void
)
{
  char bts_ver[16];
  bt_bd_addr_type bd_addr;
  bt_soc_refclock_type bt_refclock_type;
  bt_soc_clock_sharing_type bt_clock_sharing_type;
  uint8 bt_soc_logging;
  uint8 bt_2_1_lisbon_disabled;

  boolean init_successful = FALSE;

  if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_R2B_STR_ID) != NULL)
  {
    BT_STRCPY(bts_ver, "R2B", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_R2B;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_R2C_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "R2C", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_R2C;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_R3_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "R3", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_R3;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_R3BD_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "R3BD", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_R3BD;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4020BD_B0_or_B1_STR_ID) != NULL) 
  {
    if(memcmp((const void *)hw_ver_reg_id,(const void *)bt_qsoc_hw_ver_reg_4020_BD_B0, 0x04) == 0)
    {	    
      BT_STRCPY(bts_ver, "4020BD B0", sizeof(bts_ver));
      bt_qsoc.bt_qsoc_type = BT_QSOC_4020BD_B0;
    }
    else
    {
      BT_STRCPY(bts_ver, "4020BD B1", sizeof(bts_ver));
      bt_qsoc.bt_qsoc_type = BT_QSOC_4020BD_B1;
    }	    
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4021_B1_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "4021 B1", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_4021_B1;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_R4_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "R4", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_R4;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4025_A0_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "4025 A0", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_4025_A0;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4025_B0_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "4025 B0", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_4025_B0;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4025_B1_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "4025 B1", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_4025_B1;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4025_B2_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "4025 B2", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_4025_B2;
  }
  else if (strstr( (const char *)bt_qsoc_fw_ver, BT_QSOC_4025_B3_STR_ID) != NULL) 
  {
    BT_STRCPY(bts_ver, "4025 B3", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_4025_B3;
  }
  else
  {
    BT_STRCPY(bts_ver, "UNKN", sizeof(bts_ver));
    bt_qsoc.bt_qsoc_type = BT_QSOC_UNKNOWN;
  }

  MSG_SPRINTF_3(MSG_SSID_DFLT, MSG_LEGACY_LOW, "BT QSOC: BTS version %s, idx %x", \
  bts_ver, bt_qsoc.bt_qsoc_type, 0);
  bt_cmd_dc_set_soc_version(&bt_qsoc.bt_qsoc_type);

  /* NVM initalization below will need the BD Address, so read it from NV */
  bt_cmd_dc_get_bd_addr( &bd_addr ); 

  /* Set BD address in NVM configuration table with the value from MSM NV */
  memcpy( (uint8 *)(&(bt_qsoc_vs_nvm_cmd_tbl.bd_address[0])), 
          (uint8 *)bd_addr.bd_addr_bytes, 
          BT_QSOC_MAX_BD_ADDRESS_SIZE );

  bt_cmd_dc_get_refclock_type(&bt_refclock_type);
  bt_cmd_dc_get_clock_sharing_type(&bt_clock_sharing_type);
  bt_cmd_dc_get_soc_logging(&bt_soc_logging);
  bt_cmd_dc_get_bt_2_1_lisbon_status(&bt_2_1_lisbon_disabled);

  bt_qsoc_vs_nvm_cmd_tbl.refclock_type = bt_refclock_type;
  bt_qsoc_vs_nvm_cmd_tbl.clock_sharing = bt_clock_sharing_type;
  bt_qsoc_vs_nvm_cmd_tbl.soc_logging   = bt_soc_logging;
  bt_qsoc_vs_nvm_cmd_tbl.bt_2_1_lisbon_disabled =
	                                 bt_2_1_lisbon_disabled;

#ifdef FEATURE_BT_QSOC_CLASS2
  bt_qsoc_vs_nvm_cmd_tbl.bt_qsoc_device_class = BT_QSOC_DEV_CLASS2;
#else 
  bt_qsoc_vs_nvm_cmd_tbl.bt_qsoc_device_class = BT_QSOC_DEV_CLASS1;
#endif /* FEATURE_BT_QSOC_CLASS2 */

  BT_MSG_HIGH( "BT QSOC START SENDING NVMs", 0, 0, 0 );

  /* if(!bt_qsoc_nvm_vs_find_file(bt_qsoc.bt_qsoc_type, &bt_qsoc_vs_nvm_cmd_tbl)) */
  if(!bt_qsoc_read_nvm_tags_from_header(bt_qsoc.bt_qsoc_type,bt_refclock_type))
  {
      // The released nvm file is not present, use unknown soc config
      BT_MSG_HIGH( "bt_hci_qcomm_init  - NVM File not found. Shutting BT off ", 0, 0, 0 );
      return init_successful;
  }
  else
  {
    bt_qsoc_nvm_vs_init_released_nvm(bt_qsoc.bt_qsoc_type, &bt_qsoc_vs_nvm_cmd_tbl); 
  }
  BT_MSG_HIGH( "BT QSOC SoC detected", bt_qsoc.bt_qsoc_type , 0, 0 );

  /* Configure NVM values based on which SoC is connected */
  init_successful = bt_qsoc_nvm_vs_update_nvm_entries( bt_qsoc.bt_qsoc_type,
                                                        &bt_qsoc_vs_nvm_cmd_tbl );

  return init_successful;
}


/*==========================================================================

  FUNCTION       BT_QSOC_HC_DRV_SEND_CMD

  DESCRIPTION    This function sends a VS HCI command to BTS.

  DEPENDENCIES   None.

  PARAMETERS     opcode   - HCI VS opcode ( 16 bit OGF+OCF )
                 cmd_buf  - command data
                 cmd_size - data length

  RETURN VALUE   TRUE (success) or FALSE (failure).

  SIDE EFFECTS   None.

==========================================================================*/
static boolean bt_qsoc_hc_drv_send_cmd
( 
  uint16       opcode, 
  uint8        *cmd_buf, 
  uint8        cmd_size 
)
{
  bt_cmd_msg_type  *bt_cmd_ptr;
  dsm_item_type*   dsm_temp_ptr;
  boolean          got_dsm;
  boolean          success = TRUE;
  
  if( ( bt_cmd_ptr = bt_get_free_cmd() ) != NULL )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );

    bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_VS_ANY;
    bt_cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;

    dsm_temp_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, cmd_size );

    if (dsm_temp_ptr)
    {
      /* Copy the variable portion into the  dsm item. */
      got_dsm = bt_dsm_pushdown_tail( &dsm_temp_ptr,
                                      (void *)cmd_buf,
                                      cmd_size,
                                      DSM_DS_SMALL_ITEM_POOL
                                    );
      /* If pushdown was successful continue. */
      ASSERT( got_dsm == TRUE );

      bt_cmd_ptr->cmd_msg.cmd_hc_vs_any.dsm_ptr    = dsm_temp_ptr;
      bt_cmd_ptr->cmd_msg.cmd_hc_vs_any.opcode     = opcode;

      /* send the message directly bypassing the queues*/
      bt_hc_drv_send_cmd( bt_cmd_ptr );
      (void)rex_set_timer( &bt_qsoc_retry_timer, BT_QSOC_RESP_TIMEOUT );
    }
    else
    {
      BT_ERR( "BT QSOC: bt_qsoc_hc_drv_send_cmd failed: No free DSM memory",
               0, 0, 0 );
      success = FALSE;
    }

    /* Free Command Buffer as it will not be freed through normal path*/
    if ( bt_cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER )
    {
      BT_MSG_HIGH( "BT QSOC: bt_qsoc_hc_drv_send_cmd failed: SIO water mark Full",
                 0, 0, 0 );
      success = FALSE;
    }

    bt_initialize_cmd_buffer( bt_cmd_ptr );
    bt_free_cmd_buffer( bt_cmd_ptr );

  }
  else
  {
    BT_ERR( "BT QSOC: bt_qsoc_hc_drv_send_cmd failed: No free CMD buffer",
             0, 0, 0 );
    success = FALSE;
  }
  return success;
} /* bt_qsoc_hc_drv_send_cmd */


/*==========================================================================

  FUNCTION       BT_QSOC_SEND_VS_PEEK8_CMD

  DESCRIPTION    This function sends a VS command to peek a certain number of 
                 bytes from a particular memory location of BTS Controller.

  DEPENDENCIES   None.

  PARAMETERS     peek_addr - Address to read the bytes from
                 num_bytes  - Number of bytes to read 
                 
  RETURN VALUE   TRUE (success) or FALSE (failure).

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_send_vs_peek8_cmd
(  
  uint32 peek_addr,
  uint8  num_bytes 
)
{
  uint8 cmd_buf[ sizeof(peek_addr) + 2 ];

  cmd_buf[0] = BT_QSOC_VS_EDL_UPLOAD_REQ;
  memcpy( &(cmd_buf[1]), &peek_addr, sizeof( peek_addr ));
  cmd_buf[sizeof(peek_addr) + 1] = num_bytes;

  bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_EDL_GROUP, cmd_buf, 
                           sizeof( cmd_buf ));

} /* bt_qsoc_send_vs_peek8_cmd */


/*==========================================================================

  FUNCTION       BT_QSOC_SEND_VS_READ_EKL_CMD

  DESCRIPTION    This function sends a VS command to read the encryption key
                 length from the remote device.

  DEPENDENCIES   None.

  PARAMETERS     conn_hndl - Connection handle of the remote device.
                 
  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_send_vs_read_ekl_cmd
(  
  uint16 conn_hndl
)
{
  /* 
    FF FF 03 06 CC CC
    opcode [2], param len [1], subcommand [1], connection handle [2]
  */

  uint8 cmd_buf[ sizeof(conn_hndl) + 1 ];

  cmd_buf[0] = BT_QSOC_VS_CMD_READ_EKL_REQ;
  memcpy( &(cmd_buf[1]), &conn_hndl, sizeof( conn_hndl ));

  bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_CMD_GROUP, cmd_buf, 
                           sizeof( cmd_buf ));

} /* bt_qsoc_send_vs_read_ekl_cmd */


/*==========================================================================

  FUNCTION       BT_QSOC_GET_ENC_KEY_LEN

  DESCRIPTION    This function sends VS commands to read the encryption key 
                 length of the given connection handle by peeking memory of 
                 BTS Controller.
                 This is done using the following steps
                 1. Peek connection handle map to get connection database 
                    pointer
                 2. Peek connection database pointer offset by key offset to 
                    get Encryption Key Length

  DEPENDENCIES   Connection Handle must be for an esisting connection.

  PARAMETERS     conn_hndl - ACL Connection Handle
                 
  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_get_enc_key_len
(  
  uint16 conn_hndl
)
{
  /* R3 lacks the VS Read Encryption key length command, 
                                              so a VS peek is needed */
  if (bt_qsoc.bt_qsoc_type == BT_QSOC_R3)
  {
    uint8 conn_idx = 0;
    uint32 conn_db_addr = BT_QSOC_CONN_HNDL_MAP_2_ADDR(
                            bt_qsoc_conn_hndl_map[bt_qsoc.bt_qsoc_type], 
                            conn_hndl );

    /* If an entry exists for a connection handle find it */
    for( conn_idx = 0; conn_idx < BT_MAX_BT_DEV; conn_idx++)
    {
      if( bt_qsoc.conn_db[conn_idx].conn_hndl == conn_hndl )
      {
        bt_qsoc.conn_db[conn_idx].conn_db_addr = conn_db_addr;
        bt_qsoc.conn_db[conn_idx].conn_hndl = conn_hndl;
        break;
      } /* if */
    } /* for conn_idx */

    /* If no entry exists for a connection handle add a new one */
    if( conn_idx == BT_MAX_BT_DEV )
    {
      for( conn_idx = 0; conn_idx < BT_MAX_BT_DEV; conn_idx++)
      {
        if( bt_qsoc.conn_db[conn_idx].conn_hndl == BT_HC_NULL_CONN_HANDLE )
        {
          bt_qsoc.conn_db[conn_idx].conn_db_addr = conn_db_addr;
          bt_qsoc.conn_db[conn_idx].conn_hndl = conn_hndl;
          break;
        } /* if */
      } /* for conn_idx */

      if( conn_idx == BT_MAX_BT_DEV)
      {
       BT_MSG_HIGH( "No free connection database entries", 0,0,0);
      }
    } /* conn_idx == BT_MAX_BT_DEV */

    /* Send a vendor-specific peek command to read the connection DB address */
    bt_qsoc_send_vs_peek8_cmd( conn_db_addr, BT_QSOC_CDB_ADDR_NUM_BYTES );
  }
  else
  {
    /* For anything newer than an R3, 
          send the new vender specific Read Encryption key-length cmd */
    bt_qsoc_send_vs_read_ekl_cmd( conn_hndl );
  }

} /* bt_qsoc_send_vs_peek8_cmd */


/*==========================================================================

  FUNCTION       BT_QSOC_TOT_HC_TX_LEN

  DESCRIPTION    This routine returns the total buffer length available for
                 Tx in the BTS SOC.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
uint16 bt_qsoc_tot_hc_tx_len
(
  void
)
{
  return ( BT_QSOC_MAX_HC_NUM_TX_BUF * BT_QSOC_MAX_HC_TX_LEN );
}


/*==========================================================================

  FUNCTION       BT_QSOC_HCI_RESET_SEND

  DESCRIPTION    Send  HCI RESET command directly bypassing BT Command queue

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_hci_reset_send
(
  void
)
{
  bt_cmd_msg_type*   bt_cmd_ptr;

  if ( (bt_cmd_ptr = bt_get_free_cmd()) != NULL )
  {
    bt_initialize_cmd_buffer( bt_cmd_ptr );

    BT_MSG_HIGH( "BT SOC: Sending HCI RESET ", 0, 0, 0 );
    bt_cmd_ptr->cmd_hdr.cmd_type    = BT_CMD_HC_RESET;
    bt_cmd_ptr->cmd_hdr.bt_app_id   = bt_dc_last_hci_ev_reg_app_id;
    
    /* We need to send an HCI command but the HCI stack is not still allowed
     * to send HCI commands when the SOC driver is being still initalized.
     * So we send  a  HCI command to host controller through sio port
     * without going through the  regular BT command queue.
     */
    bt_hc_drv_send_cmd( bt_cmd_ptr );

    /* Free Command Buffer as it will not be freed through normal path*/
    if ( bt_cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_RETRY_CMD_LATER )
    {
      BT_MSG_HIGH( "BT QSOC: bt_qsoc_hci_reset_send failed: SIO water mark Full",
                 0, 0, 0 );
    }

    bt_initialize_cmd_buffer( bt_cmd_ptr );
    bt_free_cmd_buffer( bt_cmd_ptr );
  }
  else
  {
    BT_ERR ( "BT QSOC: HCI_RESET send failed", 0, 0, 0 );
  }
}/* bt_qsoc_hci_reset_send */


/*==========================================================================

  FUNCTION       BT_QSOC_SIO_FLUSH_TX_HCI_RESET

  DESCRIPTION    Callback for HCI_RESET flush. Indicates that HCI_RESET 
                 command has been sent out. At this point UART1's baud 
                 rate is changed to 3.2 Mbps as well as BTS is flowed-on 
                 again.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_sio_flush_tx_hci_reset
(
  void
)
{
  rex_set_sigs( &bt_tcb, BT_QSOC_HCI_RESET_FLUSH_SIG);
} /* bt_qsoc_sio_flush_tx_hci_reset */


/*==========================================================================

  FUNCTION       BT_QSOC_PROC_HCI_RESET_FLUSH_SIG

  DESCRIPTION    Does processing for hci reset flush signal. The signal 
                 indicates that HCI_RESET command has been sent out. At 
                 this point UART1's baud rate is changed to 
                 BT_SOC_SIO_BITRATE as well as BTS is flowed-on again.

  DEPENDENCIES   None.

  PARAMETERS     rex_signals - signal mask with 
                 BT_QSOC_HCI_RESET_FLUSH_SIG
                 asserted.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_proc_hci_reset_flush_sig
(
  void
)
{
  /* clear the signal */
  rex_clr_sigs( &bt_tcb, BT_QSOC_HCI_RESET_FLUSH_SIG);

  bt_qsoc_init_state = BT_QSOC_HCI_RESET;

  /* Change UART1DM baud rate to 3.2 Mbps */
  BT_MSG_HIGH( "BT QSOC: Reopen SIO at higher baudrate of %d Mbs ", 
               BT_SOC_SIO_BITRATE, 0, 0 );

  bt_dc_sio_change_baud ( BT_SOC_SIO_BITRATE, SIO_IOCTL_CHANGE_BAUD_NOW );

  /* Re-enable the flow control */
  bt_soc_enable_flow();

  /* start timer */
  (void)rex_set_timer( &bt_qsoc_retry_timer, BT_QSOC_RESP_TIMEOUT );
}/* bt_qsoc_proc_hci_reset_flush_sig */


/*==========================================================================

  FUNCTION       BT_QSOC_CHANGE_BAUD_AND_HCI_RESET

  DESCRIPTION    This function is called to send HCI_RESET command and change
                 the baud rate for UART1DM.

  DEPENDENCIES   It is important to note that sequence of steps to ensure 
                 proper effect of changing the baud-rate and apply 
                 configurations (just sent) on BTS4020. To ensure HCI_RESET
                 response comes back only after the MSM's UART1DM has been
                 properly set to new baud-rate, the BTS must be flowed-off
                 even before the issue of HCI_RESET command.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_change_baud_and_hci_reset
(
  void
)
{
  sio_ioctl_param_type  ioctl_param;

  /* Flow-off the BTS */
  bt_soc_disable_flow();

  /* Issue HCI reset */
  bt_qsoc_hci_reset_send();
  
  /* Flush to make sure HCI_RESET has been sent out */

  /* Set the flush callback function */
  ioctl_param.record_flush_func_ptr = bt_qsoc_sio_flush_tx_hci_reset;

  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_FLUSH_TX,
                   &ioctl_param );
} /* bt_qsoc_change_baud_and_hci_reset */


/*==========================================================================

  FUNCTION       BT_QSOC_RESTART

  DESCRIPTION    This function restarts the BTS. It

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_restart
(
  void
)
{
  /* Shutdown SoC for now. Change the baud rate of UART1 and 
   * start the power-up sequence again.
   */
  BT_MSG_HIGH( "BT QSoC: No response to HCI_REST cmd. Restarting.", 0, 0, 0 );

  bt_qsoc_shutdown();

  /* Change baud rate back to BT_SOC_INIT_BITRATE */
  BT_MSG_HIGH( "BT QSOC: Reopen SIO at baudrate of %d Mbs ", 
               BT_SOC_SIO_INIT_BITRATE, 0, 0 );

  bt_dc_sio_change_baud ( BT_SOC_SIO_INIT_BITRATE, SIO_IOCTL_CHANGE_BAUD_NOW );
  
  bt_qsoc_powerup();
  bt_qsoc_init();
} /* bt_qsoc_restart */


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_RETRY_SIG

  DESCRIPTION    This function does the processing of retry signal 
                 (BT_QSOC_RETRY_SIG). If a response is not received after 
                 maximum number of tries, the BTS is powered-down.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_process_retry_sig
(
  void
)
{

  const uint8 hw_ver_cmd [] = { 0x0D, 0x34, 0x00, 0x00, 0x8C, 0x04 };

  /* Clear signal */
  rex_clr_sigs( &bt_tcb, BT_QSOC_RETRY_SIG );
  
  /* HCI_RESET no response case is different than other cases. This is due
   * to the fact that we have already changed UART1's baud rate in 
   * anticipation. Since the state of BTS is not deterministic, we must 
   * start all over again 
   */
  if (bt_qsoc_init_state == BT_QSOC_HCI_RESET)
  {
    /* Exhausted max attempts? */
    if (bt_qsoc_pwrup_rtry_cnt < BT_QSOC_MAX_PWRUP_RTRY_CNT) 
    {
      bt_qsoc_pwrup_rtry_cnt++;
      bt_qsoc_restart();
    }
    else
    {
      BT_MSG_HIGH( 
          "BT QSoC Shutting Down: Exhausted powerup retry count", 0, 0, 0 );
      bt_qsoc_shutdown();

      /* Vote for VDD minimization, to negate not allowed vote at startup */
      bt_set_vdd_min_allowed();
    }
  }
  else
  {
    /* Re-send if max attempts not reached */
    if (bt_qsoc_retry_count < BT_QSOC_MAX_RETRY_COUNT)
    {
      /* Set the command depending on the state */
      switch(bt_qsoc_init_state)
      {
        case BT_QSOC_INIT_STARTED:
	/* Did not receive response to HW version id request cmd */	
        bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_EDL_GROUP, 
                           (uint8 *)hw_ver_cmd, 
                           sizeof(hw_ver_cmd) ); 	
        break;

        case BT_QSOC_DETECT_FW:
        {
          /* Did not receive response to firmware ver cmd */
          uint8  fw_ver_cmd = BT_QSOC_VS_EDL_GETAPPVER_REQ;
   
          bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_EDL_GROUP, 
                                   &fw_ver_cmd, 
                                   sizeof(fw_ver_cmd) );
        }
        break;

        case BT_QSOC_POKE8_TBL_INIT:
        {
          if (bt_qsoc.bt_qsoc_type == BT_QSOC_R3) 
          {
            /* Resend the last command */
            bt_qsoc_hc_drv_send_cmd(
                    BT_HCI_QSOC_VS_EDL_GROUP, 
                    (uint8 *)bt_qsoc_vs_poke8_tbl_r3[bt_qsoc_r3_poke_tbl_idx - 1].vs_poke8_data,
                    bt_qsoc_vs_poke8_tbl_r3[bt_qsoc_r3_poke_tbl_idx - 1].vs_poke8_data_len);
          }
        }
        break;

        case BT_QSOC_FIXED_TBL_INIT:
        {
          /* Pointer into the fixed NVM table */
          bt_qsoc_cfg_cst_element_type *nvm_ptr = NULL;

          /* Ensure init completed successfully */
          ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr != NULL );

          /* Grab a pointer to the last sent NVM packet in our fixed table */
          nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr[(bt_qsoc_conf_tbl_idx - 1)];

          /* This command should have already been sent to the SoC, so ensure 
             that it isn't a placeholder in our table (packet == NULL).
          */
          ASSERT( nvm_ptr != NULL );

          /* Resend last command to SoC */
          bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                   (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                   (uint8) (nvm_ptr[0]));   /* cmd length */
        }
        break;
   
        case BT_QSOC_DYN_TBL_INIT:
        {
          /* This command should have already been sent to the SoC, so ensure 
             that it isn't a NULL.
          */
          ASSERT (nvm_ptr_dyn != NULL)
          
          /* Send down nvm parsed from released nvm file. */
          bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                   (uint8 *) (&nvm_ptr_dyn[1]),  /* cmd data ptr */
                                   (uint8) (nvm_ptr_dyn[0]) );   /* cmd length */
        }
        break;

        case BT_QSOC_NV_TBL_INIT:
        {
          /* Pointer into the runtime NVM table */
          bt_qsoc_cfg_cst_element_type *nvm_ptr = NULL;

          /* Ensure init completed successfully */
          ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr != NULL );

          /* Grab a pointer to the last sent NVM packet in our runtime table */
          nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[(bt_qsoc_conf_tbl_idx - 1)];

          /* This command should have already been sent to the SoC, so ensure 
             that it isn't a placeholder in our table (packet == NULL).
          */
          ASSERT( nvm_ptr != NULL );

          /* Resend last command to SoC */
          bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                   (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                   (uint8) (nvm_ptr[0]) );   /* cmd length */
        }
        break;

        case BT_QSOC_CUST_TBL_INIT:
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
        {
          /* Pointer into the customer NVM table */
          bt_qsoc_cfg_cst_element_type *nvm_ptr = NULL;

          /* Grab a pointer to the last sent NVM packet in our customer table */
          nvm_ptr = bt_qsoc_target_vs_nvm_cmd_custtbl[(bt_qsoc_conf_tbl_idx - 1)];

          /* This command should have already been sent to the SoC, so ensure 
             that it isn't a placeholder in our table (packet == NULL).
          */
          ASSERT( nvm_ptr != NULL );

          /* Resend last command to SoC */
          bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                   (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                   (uint8) (nvm_ptr[0]) );   /* cmd length */
        }
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */
        break;
      
        case BT_QSOC_HCI_RESET:
          /* Should never reach here */   
        case BT_QSOC_INIT_COMPLETED:
          break;
      }
   
      bt_qsoc_retry_count++;
    }
    else
    {
      bt_qsoc_shutdown();
      BT_ERR( "BT QSOC: Shutting down - No response from SoC", 0, 0, 0 );

      /* Vote for VDD minimization, to negate not allowed vote at startup */
      bt_set_vdd_min_allowed();
    }
  } /* else */
} /* bt_qsoc_process_retry_sig */


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_VS_PEEK8_EV

  DESCRIPTION    Processes Vendor specific events from BTS 4020 which are  
                 responses tovendor specfic peek8 commands . 

  DEPENDENCIES   None.

  PARAMETERS     peek8_ev_buf - Buffer with vendor-specific BT event payload.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_process_vs_peek8_ev
(
  uint8         *peek8_ev_buf
)
{
  uint8         conn_idx;
  uint32        peek_addr = 0;

  memcpy( &peek_addr, peek8_ev_buf, sizeof(peek_addr));

  /* Take action based on the number of bytes read for vendor-specific peek. */

  switch( peek8_ev_buf[BT_QSOC_PEEK8_ADDR_LEN] )
  {
    case BT_QSOC_CDB_ADDR_NUM_BYTES: /* Connection database address */
    {
      if( peek_addr ==0 )
      {
        BT_ERR( "ConnDbptr NULL %x", peek_addr, 0, 0 ); 
      }
      else
      {
        for( conn_idx = 0; conn_idx < BT_MAX_BT_DEV; conn_idx++ )
        {
          if( bt_qsoc.conn_db[conn_idx].conn_db_addr == peek_addr )
          {
            if( peek8_ev_buf[ BT_QSOC_PEEK8_ADDR_LEN + 
                BT_QSOC_CDB_ADDR_NUM_BYTES ] != 0x01)
            {
              /* The 5th byte should be one indicating ACL handle */
              BT_ERR( "Val at pk8_loc %x is not ACL ConnDbptr %x, ACL_ind %x ", 
                      peek_addr, bt_qsoc.conn_db[conn_idx].conn_db_ptr, 
                      peek8_ev_buf[BT_QSOC_PEEK8_ADDR_LEN + 
                      BT_QSOC_CDB_ADDR_NUM_BYTES]
                    );
            }
            memcpy( &bt_qsoc.conn_db[conn_idx].conn_db_ptr, 
                    &peek8_ev_buf[BT_QSOC_CDB_ADDR_NUM_BYTES], 
                    sizeof(bt_qsoc.conn_db[conn_idx].conn_db_ptr));

            BT_MSG_HIGH( "BT QSOC: Conn Db ptr val at pk8_loc %x is %x", 
                         peek_addr, bt_qsoc.conn_db[conn_idx].conn_db_ptr, 0 );
            break;
          } /* if conn_db_addr = peek_addr */
        } /* for conn_idx */

        /* Check for array indices being out of bounds */
        if( (conn_idx < BT_MAX_BT_DEV) && (bt_qsoc.bt_qsoc_type < BT_QSOC_MAX) )
        {
          /* Peek the value at (conn_db_ptr + conn_db_offset) for getting the 
             current connection encryption key length */
          bt_qsoc_send_vs_peek8_cmd( bt_qsoc.conn_db[conn_idx].conn_db_ptr + 
                                     bt_qsoc_conn_key_offset[bt_qsoc.bt_qsoc_type],
                                     BT_QSOC_ENC_KEY_LEN_NUM_BYTES );
        }
        else
        {
          BT_ERR( "Connection index %x >= max %x, BT QSoc Type :%d", 
                   conn_idx, BT_MAX_BT_DEV, bt_qsoc.bt_qsoc_type ); 
        } /* conn_idx >= BT_MAX_BT_DEV */

      }
      
      break;
     
    }

    case BT_QSOC_ENC_KEY_LEN_NUM_BYTES: /* Encryption key length */
    {
      for( conn_idx = 0; conn_idx < BT_MAX_BT_DEV; conn_idx++ )
      {
        if( ( bt_qsoc.bt_qsoc_type < BT_QSOC_MAX ) && 
            ( ( bt_qsoc.conn_db[conn_idx].conn_db_ptr +  
               bt_qsoc_conn_key_offset[bt_qsoc.bt_qsoc_type]) == peek_addr ) )
        {
          /* Get the value at (conn_db_ptr + conn_db_offset) for the connection 
             encryption key length */
          bt_qsoc.conn_db[conn_idx].enc_key_len = 
            peek8_ev_buf[BT_QSOC_PEEK8_ADDR_LEN + 
            BT_QSOC_ENC_KEY_LEN_NUM_BYTES];
          break;
        }
      } /* for conn_idx */

      if( conn_idx < BT_MAX_BT_DEV )
      {
        /* Send an internal vendor-specific event to update btrm about the 
           encryption key length */
        bt_soc_etc_send_cur_conn_enc_ev( bt_qsoc.conn_db[conn_idx].enc_key_len,
                                         bt_qsoc.conn_db[conn_idx].conn_hndl );

        /* Enc. key read has been completed - reset the entry */
        bt_qsoc.conn_db[conn_idx].conn_hndl = BT_HC_NULL_CONN_HANDLE;
      }
      else
      {
        BT_ERR( "Connection index %x >= max %x", conn_idx, BT_MAX_BT_DEV, 0 ); 
      } /* conn_idx >= BT_MAX_BT_DEV */
      break;
    }
    default:
      break;

  } /* switch */

} /* bt_qsoc_process_vs_peek8_ev */


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_EDL_EVENTS

  DESCRIPTION    Processes embedded dowloader (EDL) events from BTS. Firmware
                 version response is one such event and is the only one 
                 handled right now. It needs to be extended as required.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_process_edl_events
(
  bt_ev_msg_type *ev_msg_ptr
)
{
  dsm_item_type *evt_dsm_ptr;
  uint8         *evt_buf; 
  const uint8 hw_ver_reg [] = { 0x34, 0x00, 0x00, 0x8C, 0x04 };

  evt_dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_vs_var_hdr.evt_data;
  evt_buf = (uint8*)(evt_dsm_ptr->data_ptr);

  /* Do processing based on exact event withing DNLD event family */
  switch(evt_buf[BT_QSOC_EVT_SUB_POS])
  {
    case BT_QSOC_VS_EDL_STATUS_RESP:
      if (bt_qsoc_init_state == BT_QSOC_POKE8_TBL_INIT)
      {
        if (bt_qsoc.bt_qsoc_type == BT_QSOC_R3) 
        {
          if (bt_qsoc_r3_poke_tbl_idx < BT_QSOC_R3_POKETBL_COUNT)
          {
            /* Currently doing fixed poke8 table processing. Send the cmd with next 
             * poke8 patch present in the table 
             */
            bt_qsoc_hc_drv_send_cmd(
                    BT_HCI_QSOC_VS_EDL_GROUP, 
                    (uint8 *)bt_qsoc_vs_poke8_tbl_r3[bt_qsoc_r3_poke_tbl_idx].vs_poke8_data,
                    bt_qsoc_vs_poke8_tbl_r3[bt_qsoc_r3_poke_tbl_idx].vs_poke8_data_len);
            bt_qsoc_r3_poke_tbl_idx++;
          }
          else
          {
            /* Pointer into the fixed NVM table */
            bt_qsoc_cfg_cst_element_type *nvm_ptr = NULL;

            /* Initialize configuration table index */
            bt_qsoc_conf_tbl_idx = 0;

            /* 
              Currently, it is assumed that we have at least one fixed table 
              NVM to send (Encryption enable).
            */
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_size > 0 );
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr != NULL );

            /* Grab a pointer to the NVM packet in our table */
            nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr[bt_qsoc_conf_tbl_idx];

            /* Placeholders are not valid for the fixed table */
            ASSERT( nvm_ptr != NULL );

            /* Kick-start BTS configuration one at a time from fixed table */
            bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                     (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                     (uint8) (nvm_ptr[0]) );   /* cmd length */
            bt_qsoc_conf_tbl_idx++; 

            /* Change state to NVM init state */
            bt_qsoc_init_state = BT_QSOC_FIXED_TBL_INIT;
          }
        }
      }
      break;

    case BT_QSOC_VS_EDL_UPLOAD_RESP:
      
      /* Peek to see if we got hw version register response */
      if(strstr( (const char *)evt_buf, (const char *)hw_ver_reg) != NULL)
      {
        memcpy(hw_ver_reg_id, &evt_buf[ BT_QSOC_EVT_SUB_POS + 6], 0x4);
        bt_qsoc_send_vs_cmds();
      }
      else 
      {  
        bt_qsoc_process_vs_peek8_ev( &evt_buf[ BT_QSOC_EVT_SUB_POS + 1] );
      }
      break;

    case BT_QSOC_VS_EDL_APPVER_RESP:
      memcpy( bt_qsoc_fw_ver, &(evt_buf[2]), ((uint8)evt_buf[1]) );

      /* Use appropriate NVM initialization table based on fw version */
      if (bt_qsoc_detect_fw_version() != TRUE)
      {
        BT_ERR( "BT QSOC: unsupported BTS version, shutting down SoC", 0, 0, 0 );
        bt_qsoc_shutdown();

        /* Vote for VDD minimization, to negate not allowed vote at startup */
        bt_set_vdd_min_allowed();

        break;
      }

      /* The processing here depends on BTS INIT state */
      switch(bt_qsoc_init_state)
      {
        case BT_QSOC_INIT_STARTED:
          break;

        case BT_QSOC_DETECT_FW:
          /* Initialize R3 Poke8 table index */
          bt_qsoc_r3_poke_tbl_idx = 0;

          /* If there are addtional R3 patches, send them down first */
          if (bt_qsoc.bt_qsoc_type == BT_QSOC_R3 &&
              BT_QSOC_R3_POKETBL_COUNT > 0) 
          {
            /* Send R3 poke patches first, then kick-start NVM afterward */
            bt_qsoc_hc_drv_send_cmd(
                    BT_HCI_QSOC_VS_EDL_GROUP, 
                    (uint8 *)bt_qsoc_vs_poke8_tbl_r3[bt_qsoc_r3_poke_tbl_idx].vs_poke8_data,
                    bt_qsoc_vs_poke8_tbl_r3[bt_qsoc_r3_poke_tbl_idx].vs_poke8_data_len);
            bt_qsoc_r3_poke_tbl_idx++;

            /* Change state to poke patch state */
            bt_qsoc_init_state = BT_QSOC_POKE8_TBL_INIT;
          }
          else
          {
            /* Pointer into the fixed NVM table */
            bt_qsoc_cfg_cst_element_type *nvm_ptr = NULL;

            /* Initialize configuration table index */
            bt_qsoc_conf_tbl_idx = 0;

            /* 
              Currently, it is assumed that we have at least one fixed table 
              NVM to send (Encryption enable).
            */
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_size > 0 );
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr != NULL );

            /* Grab a pointer to the NVM packet in our table */
            nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr[bt_qsoc_conf_tbl_idx];

            /* Placeholders are not valid for the fixed table */
            ASSERT( nvm_ptr != NULL );

            /* Kick-start BTS configuration one at a time from fixed table */
            bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                     (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                     (uint8) (nvm_ptr[0]) );   /* cmd length */
            bt_qsoc_conf_tbl_idx++; 

            /* Change state to NVM init state */
            bt_qsoc_init_state = BT_QSOC_FIXED_TBL_INIT;
          }
          break;

        case BT_QSOC_POKE8_TBL_INIT:
        case BT_QSOC_FIXED_TBL_INIT:
        case BT_QSOC_DYN_TBL_INIT:
        case BT_QSOC_NV_TBL_INIT:
        case BT_QSOC_CUST_TBL_INIT:
        case BT_QSOC_HCI_RESET:
        case BT_QSOC_INIT_COMPLETED:
          break;
      }
      break;

      case BT_QSOC_VS_EDL_DLVER_RESP:
        break;

      case BT_QSOC_VS_EDL_NVMADD_RESP:
        break;

      default:
        break;
  }
  dsm_free_packet( &(evt_dsm_ptr) );
} /* bt_qsoc_process_edl_events */


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_NVM_EVENTS

  DESCRIPTION    Processes NVM events from BTS 4020. These events are 
                 generated in response to setting NVM configuration 
                 parameters or peeking at NVM parameters. Currently only 
                 NVM set events are handled.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
static void bt_qsoc_process_nvm_events
(
  bt_ev_msg_type *ev_msg_ptr
)
{
  /* Pointer into the Fixed/Runtime NVM table */
  bt_qsoc_cfg_cst_element_type *nvm_ptr = NULL;

  uint8 bt_qsoc_nvm_error = 0;

  dsm_item_type *evt_dsm_ptr;
  uint8         *evt_buf; 

  evt_dsm_ptr = ev_msg_ptr->ev_msg.ev_hc_vs_var_hdr.evt_data;
  evt_buf = (uint8*)(evt_dsm_ptr->data_ptr);

  switch(evt_buf[BT_QSOC_EVT_SUB_POS])
  {
    case BT_QSOC_VS_NVM_ACCESS_GET:
      break;

    case BT_QSOC_VS_NVM_ACCESS_SET:
        
      bt_qsoc_retry_count = 0;

      /* Handle events based on the current state */
      switch(bt_qsoc_init_state)
      {
        case BT_QSOC_INIT_STARTED:
        case BT_QSOC_DETECT_FW:
        case BT_QSOC_POKE8_TBL_INIT:
          break;

        case BT_QSOC_FIXED_TBL_INIT:
        {
          if (bt_qsoc_conf_tbl_idx < bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_size)
          {
            /* Ensure init completed successfully */
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr != NULL );

            /* Grab a pointer to the NVM packet in our fixed table */
            nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_fixed_table_ptr[bt_qsoc_conf_tbl_idx];

            /* There should be no placeholders in the fixed table */
            ASSERT( nvm_ptr != NULL );

            /* Currently doing fixed table processing. Send the cmd with next 
             * NVM config parameter present in the table 
             */
            bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                     (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                     (uint8) (nvm_ptr[0]) );   /* cmd length */
            bt_qsoc_conf_tbl_idx++;
          }
          else if(bt_qsoc_nvm_vs_get_next_released_nvm((uint8 **)&nvm_ptr_dyn, &bt_qsoc_nvm_error))
          { /* Send down nvm parsed from released nvm file. */
            bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                       (uint8 *) (&nvm_ptr_dyn[1]),  /* cmd data ptr */
                                       (uint8) (nvm_ptr_dyn[0]) );   /* cmd length */
            bt_qsoc_init_state = BT_QSOC_DYN_TBL_INIT;
          }
          else if(bt_qsoc_nvm_error)
          {
            bt_qsoc_nvm_vs_close_released_nvm();
            bt_qsoc_shutdown();
            BT_ERR( "BT QSOC: Shutting down - No response from SoC", 0, 0, 0 );

            /* Vote for VDD minimization, to negate not allowed vote at startup */
            bt_set_vdd_min_allowed();
            return;
          }
          else
          {
            /* Fixed table configuration complete, start NV 
             * table configuration 
             */
            bt_qsoc_nvm_vs_close_released_nvm();
            bt_qsoc_conf_tbl_idx = 0;

            /* 
              Currently, it is assumed that we have at least one runtime table 
              NVM to send (System Clock).
            */
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_size > 0 );
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr != NULL );

            /* Grab a pointer to the NVM packet in our runtime table */
            nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[bt_qsoc_conf_tbl_idx];
            bt_qsoc_conf_tbl_idx++;

            /* Skip over any placeholders (pkt == NULL) in our runtime table */
            while (nvm_ptr == NULL && 
                   (bt_qsoc_conf_tbl_idx < bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_size))
            {
              /* Grab a pointer to the next NVM packet in our runtime table */
              nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[bt_qsoc_conf_tbl_idx];
              bt_qsoc_conf_tbl_idx++;
            }

            /* Do we have a non-placeholder NVMs to send down? */
            if (nvm_ptr != NULL)
            {
              bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                       (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                       (uint8) (nvm_ptr[0]) );   /* cmd length */
              bt_qsoc_init_state = BT_QSOC_NV_TBL_INIT;
            }
            else
            {
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
              /* 
                There are no more runtime NVMs to send down, see if there are 
                any customer NVMs to send or continue with an HCI reset.
              */
              if (SIZEOF_BT_QSOC_CFG_TBL_CUSTOMER > 0) 
              {              
                /* NV table configuration complete, start cust
                 * table configuration 
                 */
                bt_qsoc_conf_tbl_idx = 0;

                /* Grab a pointer to the next NVM packet in our customer table */
                nvm_ptr = bt_qsoc_target_vs_nvm_cmd_custtbl[bt_qsoc_conf_tbl_idx];

                bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                         (uint8 *) (&nvm_ptr[1]),  
                                         (uint8) (nvm_ptr[0]) );                   
                bt_qsoc_conf_tbl_idx++;

                bt_qsoc_init_state = BT_QSOC_CUST_TBL_INIT;
              }
              else /* Issue HCI_RESET command */
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */
              {
                /* NV table configuration complete, and no entries in cust
                 * table, send HCI_RESET cmd 
                 */
                bt_qsoc_change_baud_and_hci_reset();
                bt_qsoc_init_state = BT_QSOC_HCI_RESET;
              }
            }
          }
        }
        break;

        case BT_QSOC_DYN_TBL_INIT:
        {
          if(bt_qsoc_nvm_vs_get_next_released_nvm((uint8 **)&nvm_ptr_dyn, &bt_qsoc_nvm_error))
          { /* Send down nvm parsed from released nvm file. */
            bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                       (uint8 *) (&nvm_ptr_dyn[1]),  /* cmd data ptr */
                                       (uint8) (nvm_ptr_dyn[0]) );   /* cmd length */
            
          }
          else if(bt_qsoc_nvm_error)
          {
            bt_qsoc_nvm_vs_close_released_nvm();
            bt_qsoc_shutdown();
            BT_ERR( "BT QSOC: Shutting down - No response from SoC", 0, 0, 0 );

            /* Vote for VDD minimization, to negate not allowed vote at startup */
            bt_set_vdd_min_allowed();
            return;
          }
          else
          {
            /* Dynamic configuration complete, close nvm file, start NV 
             * table configuration 
             */
            bt_qsoc_nvm_vs_close_released_nvm();
            bt_qsoc_conf_tbl_idx = 0;

            /* 
              Currently, it is assumed that we have at least one runtime table 
              NVM to send (System Clock).
            */
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_size > 0 );
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr != NULL );

            /* Grab a pointer to the NVM packet in our runtime table */
            nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[bt_qsoc_conf_tbl_idx];
            bt_qsoc_conf_tbl_idx++;

            /* Skip over any placeholders (pkt == NULL) in our runtime table */
            while (nvm_ptr == NULL && 
                   (bt_qsoc_conf_tbl_idx < bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_size))
            {
              /* Grab a pointer to the next NVM packet in our runtime table */
              nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[bt_qsoc_conf_tbl_idx];
              bt_qsoc_conf_tbl_idx++;
            }

            /* Do we have a non-placeholder NVMs to send down? */
            if (nvm_ptr != NULL)
            {
              bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                       (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                       (uint8) (nvm_ptr[0]) );   /* cmd length */
              bt_qsoc_init_state = BT_QSOC_NV_TBL_INIT;
            }
            else
            {
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
              /* 
                There are no more runtime NVMs to send down, see if there are 
                any customer NVMs to send or continue with an HCI reset.
              */
              if (SIZEOF_BT_QSOC_CFG_TBL_CUSTOMER > 0) 
              {              
                /* NV table configuration complete, start cust
                 * table configuration 
                 */
                bt_qsoc_conf_tbl_idx = 0;

                /* Grab a pointer to the next NVM packet in our customer table */
                nvm_ptr = bt_qsoc_target_vs_nvm_cmd_custtbl[bt_qsoc_conf_tbl_idx];

                bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                         (uint8 *) (&nvm_ptr[1]),  
                                         (uint8) (nvm_ptr[0]) );                   
                bt_qsoc_conf_tbl_idx++;

                bt_qsoc_init_state = BT_QSOC_CUST_TBL_INIT;
              }
              else /* Issue HCI_RESET command */
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */
              {
                /* NV table configuration complete, and no entries in cust
                 * table, send HCI_RESET cmd 
                 */
                bt_qsoc_change_baud_and_hci_reset();
                bt_qsoc_init_state = BT_QSOC_HCI_RESET;
              }
            }
          }
        }
        break;

        case BT_QSOC_NV_TBL_INIT:
          if (bt_qsoc_conf_tbl_idx < bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_size)
          {
            /* Currently doing NV table processing. Send the cmd with next 
             * NVM config parameter present in the table 
             */
            ASSERT( bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr != NULL );
            /* Grab a pointer to the NVM packet in our runtime table */
            nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[bt_qsoc_conf_tbl_idx];
            bt_qsoc_conf_tbl_idx++;

            /* Skip over any placeholders in our runtime table */
            while (nvm_ptr == NULL && 
                   (bt_qsoc_conf_tbl_idx < bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_size))
            {
              /* Grab a pointer to the next NVM packet in our runtime table */
              nvm_ptr = bt_qsoc_vs_nvm_cmd_tbl.nvm_runtime_table_ptr[bt_qsoc_conf_tbl_idx];
              bt_qsoc_conf_tbl_idx++;
            }

            /* Do we have a non-placeholder NVMs to send down? */
            if (nvm_ptr != NULL)
            {
              bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                       (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                       (uint8) (nvm_ptr[0]) );   /* cmd length */
            }
            else
            {
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
              if (SIZEOF_BT_QSOC_CFG_TBL_CUSTOMER > 0) 
              {              
                /* NV table configuration complete, start customer
                 * table configuration 
                 */
                bt_qsoc_conf_tbl_idx = 0;

                /* Grab a pointer to the NVM packet in our customer table */
                nvm_ptr = bt_qsoc_target_vs_nvm_cmd_custtbl[bt_qsoc_conf_tbl_idx];

                bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                         (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                         (uint8) (nvm_ptr[0]) );   /* cmd length */
                bt_qsoc_conf_tbl_idx++;

                bt_qsoc_init_state = BT_QSOC_CUST_TBL_INIT;
              }
              else /* Issue HCI_RESET command */
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */
              {
                /* NV table configuration complete, and no entries in cust
                 * table, send HCI_RESET cmd 
                 */
                bt_qsoc_change_baud_and_hci_reset();
                bt_qsoc_init_state = BT_QSOC_HCI_RESET;
              }
            }
          }
          else
          {
            BT_MSG_HIGH( "BT QSOC DONE SENDING NVMs", 0, 0, 0 );
            
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
            if (SIZEOF_BT_QSOC_CFG_TBL_CUSTOMER > 0) 
            {              
              /* NV table configuration complete, start customer
               * table configuration 
               */
              bt_qsoc_conf_tbl_idx = 0;

              /* Grab a pointer to the NVM packet in our customer table */
              nvm_ptr = bt_qsoc_target_vs_nvm_cmd_custtbl[bt_qsoc_conf_tbl_idx];
             
              bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                       (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                       (uint8) (nvm_ptr[0]) );   /* cmd length */
              bt_qsoc_conf_tbl_idx++;
             
              bt_qsoc_init_state = BT_QSOC_CUST_TBL_INIT;
            }
            else /* Issue HCI_RESET command */
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */
            {
              /* NV table configuration complete, and no entries in cust
               * table, send HCI_RESET cmd 
               */
              bt_qsoc_change_baud_and_hci_reset();
              bt_qsoc_init_state = BT_QSOC_HCI_RESET;
            }
          }
          break;

        case BT_QSOC_CUST_TBL_INIT:
#ifdef FEATURE_BT_QSOC_CUST_NVM_TBL
          if (bt_qsoc_conf_tbl_idx < SIZEOF_BT_QSOC_CFG_TBL_CUSTOMER)
          {
            /* Grab a pointer to the next NVM packet in our customer table */
            nvm_ptr = bt_qsoc_target_vs_nvm_cmd_custtbl[bt_qsoc_conf_tbl_idx];

            bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_NVM_GROUP, 
                                     (uint8 *) (&nvm_ptr[1]),  /* cmd data ptr */
                                     (uint8) (nvm_ptr[0]) );   /* cmd length */
            bt_qsoc_conf_tbl_idx++;
          }
          else
          {
            /* Cust table configuration complete, send HCI_RESET cmd */
            bt_qsoc_change_baud_and_hci_reset(); 
            bt_qsoc_init_state = BT_QSOC_HCI_RESET;
          }
#endif /* FEATURE_BT_QSOC_CUST_NVM_TBL */
          break;

        case BT_QSOC_HCI_RESET:
          break;

        case BT_QSOC_INIT_COMPLETED:
          break;
      }
      break;

    default:
      break;
  }
  dsm_free_packet( &(evt_dsm_ptr) );
} /* bt_qsoc_process_nvm_events */


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
                 if it is sleeping and enable inbound flow.

==========================================================================*/
void bt_qsoc_powerup
(
  void
)
{
#ifdef T_QSC6270
  bt_soc_clock_sharing_type bt_clock_sharing;
  boolean gp1_result = FALSE;
  boolean gp2_result = FALSE;
#endif /* T_QSC6270 */

/* If POWERDOWN is not being done, then only do powerup the first time. */
#ifndef FEATURE_BT_QSOC_POWERDOWN
  if (bt_dc.driver_state == BT_DS_SOC_IDLE) 
  {
#endif /* FEATURE_BT_QSOC_POWERDOWN */

#ifdef FEATURE_BT_VOTING_32K_SLEEP_CLOCK
    pm_vote_clk_32k(PM_ON_CMD, PM_32K_CLK_BT_APP);	
#endif /* FEATURE_BT_VOTING_32K_SLEEP_CLOCK */

    /* Temporarily using target define, this will eventually need to be 
       a separate feature
     */
#if (defined(T_QSC1110) || defined(T_QSC1100)) // Gemsea Modify

    /* Force enable the clock (XO_GP1) from software. This must be done 
       when BTS is still in reset. After the HCI reset clock enable 
       signal from BTS will be functional so XO_GP1 will be switched to 
       automatic mode
     */ 
    pm_tcxo2_cmd(PM_TCXO_XO_GP2_OUT_ENABLE);
    pm_tcxo2_cmd(PM_TCXO_MANUAL_MODE_CMD); 

#endif /* T_QSC1110 */ 
  
#ifdef T_QSC6270
    bt_cmd_dc_get_clock_sharing_type(&bt_clock_sharing);

    if ( bt_clock_sharing == BT_SOC_CLOCK_SHARING_DISABLED )
    {
      /* Disable LPM mode (follow TCXO mode) for GP1(VDD_BT) and GP2(VDD_PA)
         If external BT reference clock is used, there is no CLK_REQ 
         to PMIC, so PMIC can't go back to normal mode when BT wakes up. 
         Note: Each LDO will consume extra current up to 40uA if LPM 
         mode is disabled.
      */
      pm_lp_mode_control(PM_OFF_CMD, PM_VREG_LP_GP1_ID);
      pm_lp_mode_control(PM_OFF_CMD, PM_VREG_LP_GP2_ID);
    }

    /* Get application vote id for VREG_GP1 and VREG_GP2 */
    gp1_result = pm_vote_vreg_request_vote(PM_VREG_GP1_ID, &BT_PM_GP1_VOTE);
    gp2_result = pm_vote_vreg_request_vote(PM_VREG_GP2_ID, &BT_PM_GP2_VOTE);

    if ( gp1_result == FALSE || gp2_result == FALSE )
    {
      BT_ERR("BT QSOC: PMIC vote request failed (%d, %d)", 
             gp1_result, gp2_result, 0);
    }

    /* Enabled pull down on VREG supplying power to BTS */
    pm_vreg_pull_down_switch(PM_ON_CMD, PM_VREG_PDOWN_GP1_ID);

    if (hw_msm_version() >= HW_VERSION(1,3)) /* > 3,0*/
    { 
      /* B1, C0, C1 MSM */
      /* Since BTS is used in bypass mode, set voltage level to 1.8V */
      pm_vreg_set_level( BT_PM_PWR_SRC, 1800 );
    }
    else
    {
      /* B0, A1 MSM */
      /* Since BTS is used in LDO mode, set voltage level to 2.6V */
      pm_vreg_set_level( BT_PM_PWR_SRC, 2600 ); 
    }

    /* Turn on VREG_GP1(VDD_BT) & VREG_GP2(VDD_PA) with turn off vote */
    pm_vote_vreg_switch(PM_ON_CMD, PM_VREG_GP1_ID, BT_PM_GP1_VOTE);
    pm_vote_vreg_switch(PM_ON_CMD, PM_VREG_GP2_ID, BT_PM_GP2_VOTE);

    if (bt_clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED )
    {
      /* Set TCXO Manual mode to route XO_GP1_OUT clock to BTS402x */
      pm_tcxo2_cmd(PM_TCXO_MANUAL_MODE_CMD); 
      pm_tcxo2_cmd(PM_TCXO_BUFFER_ON_CMD); 
      pm_tcxo2_cmd(PM_TCXO_EMULATE_TCXO_EN_ACTIVE_CMD);
 
      /* GP1 drive strength : Higher strength, more current consumption
           - 0x82: Lowest,
           - 0x80: 2nd Lowest - BT clock sharing default setting,
           - 0x88: 2nd Highest - PMIC default setting,
           - 0x8A: Highest 
       */
      pm_dvt_out(0x7A, 0x80);
    }
#endif /* T_QSC6270 */

#if (defined (T_QSC60X5) || defined (T_QSC6270) || defined (T_QSC1110) || defined(T_QSC1100))// Gemsea Modify
    /* NOTE: QSC60X5 is also defined for QSC1110 target */

    /* 60X5 uses the GPIO interface to control SoC power */
    gpio_tlmm_config(BT_PWR_EN);// Gemsea Modify
    gpio_out( BT_PWR_EN, GPIO_HIGH_VALUE );
    BT_MSG_HIGH( "BT QSOC Powerup: Calling gpio_out - to turn on", 0, 0, 0 );

    /* wait for vreg to turn ON */
    rex_sleep( 10 );

    gpio_out( BT_PWR_EN, GPIO_LOW_VALUE );
    BT_MSG_HIGH( "BT QSOC Powerup: Calling gpio_out - to turn off", 0, 0, 0 );

    /* wait for vreg to turn OFF */
    rex_sleep( 10 );

    gpio_out( BT_PWR_EN, GPIO_HIGH_VALUE );
    BT_MSG_HIGH( "BT QSOC Powerup: Calling gpio_out - to turn on", 0, 0, 0 );

    /* wait for vreg to turn ON*/
    rex_sleep( 10 );

#elif (defined(T_MSM6246)) || (defined(T_MSM6290)) 

  /* Enabled pull down on VREG supplying power to BTS */
  pm_vreg_pull_down_switch(PM_ON_CMD, PM_VREG_PDOWN_WLAN_ID);

  /* For MSMs with PMIC interface */
  pm_vreg_set_level( BT_PM_PWR_SRC, 2600 );
  (void)pm_vreg_control(PM_ON_CMD, 
                        (pm_vreg_masked_type)(1ULL << (uint32)PM_VREG_WLAN_ID));

  rex_sleep( 10 );

  /* Put the output lines (RTS and Tx) in proper state so there is no leakage 
     and BTS can be powered-off. (RTS is active low, Tx is active high)
   */
  bt_soc_enable_flow();

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_TX_START_BREAK,
                   NULL ); 
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */

/* GPIO 66 is connected on certain 4025 -based keypads to the reset on the BT 
 * chip. So, to bring the chip out of reset this line needs to be driven high.
 * Also, before the chip is powered off this line needs to be low.
 */

  gpio_out( GPIO_OUTP_66, GPIO_LOW_VALUE );
  (void)pm_vreg_control(PM_OFF_CMD, 
                        (pm_vreg_masked_type)(1ULL << (uint32)PM_VREG_WLAN_ID));

  rex_sleep( 10 );

  (void)pm_vreg_control(PM_ON_CMD, 
                        (pm_vreg_masked_type)(1ULL << (uint32)PM_VREG_WLAN_ID));

  rex_sleep( 10 );

  gpio_out( GPIO_OUTP_66, GPIO_HIGH_VALUE );
  rex_sleep( 10 );

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_TX_END_BREAK,
                   NULL ); 
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP */


#elif (defined(T_MSM7600))

  /* For MSMs with PMIC interface */
  /* Enabled pull down on VREG supplying power to BTS */
  pm_vreg_pull_down_switch(PM_ON_CMD, PM_VREG_PDOWN_BT_ID);

  pm_vreg_set_level( BT_PM_PWR_SRC, 2600 );

  /* Power-on BTS */
  pm_vote_vreg_switch( PM_ON_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Powerup: Calling PMIC - to turn on", 0, 0, 0 );

  /* wait for VREG to turn ON */
  rex_sleep( 10 );

  /* Power-off BTS */
  pm_vote_vreg_switch( PM_OFF_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Powerup: Calling PMIC - to turn off", 0, 0, 0 );

  /* wait for VREG to turn OFF */
  rex_sleep( 10 );

  /* Power-on BTS */
  pm_vote_vreg_switch( PM_ON_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Powerup: Calling PMIC - to turn on", 0, 0, 0 );

  /* wait for vreg to turn ON */
  rex_sleep( 10 );

#else

  /* For MSMs with PMIC interface */

  /* Power-on BTS */
  pm_vote_vreg_switch( PM_ON_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Powerup: Calling PMIC - to turn on", 0, 0, 0 );

  /* wait for VREG to turn ON */
  rex_sleep( 10 );

  /* Power-off BTS */
  pm_vote_vreg_switch( PM_OFF_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Powerup: Calling PMIC - to turn off", 0, 0, 0 );

  /* wait for VREG to turn OFF */
  rex_sleep( 10 );

  /* Power-on BTS */
  pm_vote_vreg_switch( PM_ON_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Powerup: Calling PMIC - to turn on", 0, 0, 0 );

  /* wait for vreg to turn ON */
  rex_sleep( 10 );

#endif /* T_QSC60X5 || T_QSC6270 || T_QSC1110 */

#ifndef FEATURE_BT_QSOC_POWERDOWN
  } /* bt_dc.driver_state == BT_DS_SOC_IDLE */
#endif /* FEATURE_BT_QSOC_POWERDOWN */

  /* 
    Since the SoC can be shutdown and powered-up multiple times, and each
    shutdown could result in the SIO driver being put to sleep. Ensure that
    there is a matching SIO awake up on power-up.
  */
  if (bt_dc.sio_state == BT_SIO_SLEEPING)
  {
    bt_dc_update_sio_state( BT_SIO_AWAKE, TRUE );
  }

  bt_qsoc_init_state = BT_QSOC_INIT_STARTED;

  /* Ensure the MSM is flowed on */
#ifdef BT_QSOC_INBAND_TRACE_ENABLE      
   BT_MSG_HIGH(" BT DC: Enabling Inbound Flow to SoC", 0, 0, 0);  
#endif /* BT_QSOC_INBAND_TRACE_ENABLE */              
  bt_soc_enable_flow();

#ifdef FEATURE_BT_QSOC_SLEEP

  /* Ensure sleep variables are at reset values */
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  bt_qsoc_inband_soc_flowed_on1 = TRUE;
  bt_qsoc_inband_soc_flowed_on2 = TRUE;
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

  bt_qsoc_sleep_sm_reset();

#ifdef FEATURE_BT_QSOC_GPIO_WAKE
  /* Configure HOST_WAKE ISR */
  (void) gpio_int_set_detect(HOST_WAKE_INT, DETECT_LEVEL);

  (void) gpio_int_set_handler(HOST_WAKE_INT, ACTIVE_LOW, bt_qsoc_host_wake_isr); 

  /* Keep the SoC awake during initialization */
  gpio_out(EXT_WAKE, BT_QSOC_EXT_WAKE_ASSERTED);
#endif /* FEATURE_BT_QSOC_GPIO_WAKE */
#endif /* FEATURE_BT_QSOC_SLEEP */

} /* bt_qsoc_powerup */

/*==========================================================================

  FUNCTION       bt_qsoc_send_vs_hw_version_reg

  DESCRIPTION    This function issues a VS cmd to determine hw version 
                 register value. It is used to determine SOC type.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/

void bt_qsoc_send_vs_hw_version_reg
(
  void
)
{
   const uint8 hw_ver_cmd [] = { 0x0D, 0x34, 0x00, 0x00, 0x8C, 0x04 };
   
   bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_EDL_GROUP, 
                           (uint8 *)hw_ver_cmd, 
                           sizeof(hw_ver_cmd) );
   BT_MSG_HIGH( "BT QSOC init: Sending get HW version register cmd", 0, 0, 0 );

   /* Initialize the retry count */
   bt_qsoc_retry_count = 0;
   bt_qsoc_pwrup_rtry_cnt = 0;
} /* bt_qsoc_send_vs_hw_version_reg */

/*==========================================================================

  FUNCTION       bt_qsoc_send_vs_cmds

  DESCRIPTION    This function kick-starts BTS initialization by issuing 
                 VS cmd to determine firmware version. All the VS cmds to 
                 set BTS' NVM are sent following each other. The initialization 
                 ends by sending HCI_RESET command.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_send_vs_cmds
(
  void
)
{
  uint8           fw_ver_cmd = BT_QSOC_VS_EDL_GETAPPVER_REQ;
  uint8           conn_idx = 0;

  /* Initialize the retry count */
  bt_qsoc_retry_count = 0;
  bt_qsoc_pwrup_rtry_cnt = 0;
  
  /* Send firmware version request command */
  bt_qsoc_hc_drv_send_cmd( BT_HCI_QSOC_VS_EDL_GROUP, 
                           &fw_ver_cmd, 
                           sizeof(fw_ver_cmd) );
  BT_MSG_HIGH( "BT QSOC init: Sending get Firmware version cmd", 0, 0, 0 );

  /* Change the BTS Init state to Detect Firmware */
  bt_qsoc_init_state   = BT_QSOC_DETECT_FW;
  bt_qsoc.bt_qsoc_type = BT_QSOC_NONE;

  for( conn_idx = 0; conn_idx < BT_MAX_BT_DEV; conn_idx++)
  {
    bt_qsoc.conn_db[conn_idx].conn_hndl = BT_HC_NULL_CONN_HANDLE;
  } /* for conn_idx */

} /* bt_qsoc_send_vs_cmds */


/*==========================================================================

  FUNCTION       BT_QSOC_INIT

  DESCRIPTION    This function wait for flow off before calling VS commands
  
  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_init
(
  void
)
{
  /* Initialize the Retry timer */
  rex_def_timer( &bt_qsoc_retry_timer, &bt_tcb, BT_QSOC_RETRY_SIG );    
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  /* Ensure that the TX line is high now, inband sleep is not in effect yet */
  bt_dc_sio_ioctl( bt_dc.sio_stream_id_ext_soc,
                   SIO_IOCTL_TX_END_BREAK,
                   NULL );

  /* Wait for SOC to  flow off on powerup and flow on again */
  bt_qsoc_inband_soc_flowed_on1 = FALSE;
  (void)rex_set_timer( &bt_qsoc_retry_timer, BT_QSOC_INBAND_CTS_DELAY);
#else
  bt_qsoc_send_vs_hw_version_reg();
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
  

} /* bt_qsoc_init */


#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
/*==========================================================================

  FUNCTION       BT_QSOC_INBAND_ETC_INIT

  DESCRIPTION    This routine continues SOC initialization with Inband Sleep
                 signalling enabled.                 

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   

==========================================================================*/
void bt_qsoc_inband_etc_init
(
  void
)
{
  bt_qsoc_init_state = BT_QSOC_INIT_COMPLETED;
  bt_qsoc_pwrup_rtry_cnt = 0;
  
#ifdef BT_QSOC_INBAND_TRACE_ENABLE
  BT_MSG_DEBUG( "BT QSOC Inband Etc Init: state1: %d state2: %d msm_state %d", 
                 bt_qsoc_sleep_inband_get_rx_state(),
                 bt_qsoc_sleep_inband_get_tx_state(),
                 bt_qsoc_sleep_sm_get_msm_state() );  
#endif /* BT_QSOC_INBAND_TRACE_ENABLE */
  
  /* This is the point at which we can trigger of the INBAND
      sleep state machine */
  bt_qsoc_sleep_inband_init();
}
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_RETRY_SIG

  DESCRIPTION    This function processes the SoC specific signals which are 
                 defined for BTS. Signals that are not expected are still 
                 cleared but a log message is generated.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_process_retry_signals
( 
  void
)
{
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
    if ( bt_qsoc_inband_soc_flowed_on1 == FALSE )
    {         
     /* The Flow-On Timer has expired indicating that  4020 is Flow On state 
        after Power Reset */        
      bt_qsoc_inband_soc_flowed_on1 = TRUE;
      rex_clr_timer( &bt_qsoc_retry_timer );
      rex_clr_sigs( &bt_tcb, BT_QSOC_RETRY_SIG );
      
      #ifdef BT_QSOC_INBAND_TRACE_ENABLE      
      BT_MSG_HIGH( "BT QSOC INBAND: Soc Flowed On-1 state1: %d state2: %d ", 
                     bt_qsoc_sleep_inband_get_rx_state(),
                     bt_qsoc_sleep_inband_get_tx_state(),
                     0 );      
      #endif /* BT_QSOC_INBAND_TRACE_ENABLE */
      
      bt_qsoc_send_vs_hw_version_reg();
    }
    else if ( bt_qsoc_inband_soc_flowed_on2 == FALSE )
    {
     /* The Flow-On Timer has expired indicating that  4020 is Flow On state 
        after HCI_RST */                
      bt_qsoc_inband_soc_flowed_on2 = TRUE;      
      rex_clr_timer( &bt_qsoc_retry_timer );
      rex_clr_sigs( &bt_tcb, BT_QSOC_RETRY_SIG );
      
      #ifdef BT_QSOC_INBAND_TRACE_ENABLE      
      BT_MSG_HIGH( "BT QSOC INBAND: Soc Flowed On-2 state1: %d state2: %d ",
                     bt_qsoc_sleep_inband_get_rx_state(),
                     bt_qsoc_sleep_inband_get_tx_state(),
                     0 );      
      #endif /* BT_QSOC_INBAND_TRACE_ENABLE */      
      
      bt_qsoc_inband_etc_init();
    }
    else if( BT_QSOC_IS_READY() == TRUE ) 
    {
      /* BT_QSOC_INBAND_TX_ACK_REQD_SIG is same as BT_QSOC_RETRY_SIG */
      rex_clr_timer( &bt_qsoc_retry_timer );
      (void)  rex_clr_sigs( &bt_tcb, BT_QSOC_RETRY_SIG );      
      /*
        Inband RX SM is indicating to TX SM to do UNBREAK ACK on TX line whene
        it is ready to do so. No need for TX SM to do it if it is already 
        transmitting data.Signal usage eliminates messy code & need for MUTEXs.
      */
      bt_qsoc_sleep_inband_proc_tx_ack_reqd_sig();
    }
    else
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/      
    {
      bt_qsoc_process_retry_sig();
    }
} /* bt_qsoc_process_retry_signals */


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
void bt_qsoc_process_signals
( 
  rex_sigs_type  rex_signals
)
{
  if (rex_signals & BT_QSOC_RETRY_SIG)
  {
    /* 
       BT_QSOC_RETRY_SIG is reused after SOC Initilization. So  it should be
       handled accordingly depending on the SOC Driver state.
     */ 
    bt_qsoc_process_retry_signals();
  }
  else if (rex_signals & BT_QSOC_HCI_RESET_FLUSH_SIG) 
  {
    #ifdef FEATURE_BT_QSOC_INBAND_SLEEP
    /* BT_QSOC_HCI_RESET_FLUSH_SIG =~ BT_QSOC_INBAND_RX_FLUSH_SIG =~BT_SOC_SIG2*/
      if( BT_QSOC_IS_READY() == TRUE )      
      {
      bt_qsoc_sleep_inband_proc_rx_flush_sig();
      }
      else
    #endif /* FEATURE_BT_QSOC_INBAND_SLEEP */     
      {
        bt_qsoc_proc_hci_reset_flush_sig();
      }
  }
#ifdef FEATURE_BT_QSOC_SLEEP
  else if (rex_signals & BT_QSOC_SLEEP_SIG) 
  {
    (void) rex_clr_sigs (&bt_tcb, BT_QSOC_SLEEP_SIG);
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP    
    /* The MSM TX path is now idle. MSM can vote for Inband sleep now. */    
    bt_qsoc_sleep_inband_proc_tx_flush_sig();
#else
    /* 
      A sleep related event occured in an interrupt context, but the MSM didn't
      change state. Re-evaluate all events so far and move to new state (if
      necessary). 
    */
    bt_qsoc_sleep_sm_update_msm_state();
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/    
  }
#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  else if (rex_signals & BT_QSOC_INBAND_BREAK_RX_SIG)
  {
      (void) rex_clr_sigs( &bt_tcb, BT_QSOC_INBAND_BREAK_RX_SIG );
    /* RX path is clear and RX BREAK has been recived.*/
      bt_qsoc_sleep_inband_proc_rx_brk_evt();      
  }
#else
  else if (rex_signals & BT_QSOC_SLEEP_T_SIG)
  {
      (void) rex_clr_sigs( &bt_tcb, BT_QSOC_SLEEP_T_SIG );

    /* The sleep timer expired, so the RX path should now be clear. */
      bt_qsoc_sleep_sm_eval_event(BT_QSOC_SLEEP_EVENT_SLEEP_TIMER, TRUE);
  }
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/
  
#endif /* FEATURE_BT_QSOC_SLEEP */
  else
  {
    BT_ERR( "BT QSOC: Signal %x not expected", rex_signals, 0 ,0 );

    /* Clear signal, even though the signal was not expected */
    rex_clr_sigs( &bt_tcb, (BT_SOC_SIG3 | BT_SOC_SIG4) );
  }
} /* bt_qsoc_process_signals */


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
void bt_qsoc_process_vs_event
( 
  bt_ev_msg_type *ev_msg_ptr
)
{
  BT_MSG_HIGH( "BT QSOC EV RX: HC VS Type %x AID %x",
                ev_msg_ptr->ev_msg.ev_hc_vs_var_hdr.sub_event,
                ev_msg_ptr->ev_hdr.bt_app_id, 0 );

  /* Clear the retry timer */
  rex_clr_timer( &bt_qsoc_retry_timer );

  switch(ev_msg_ptr->ev_msg.ev_hc_vs_var_hdr.sub_event)
  {
    case BT_QSOC_EDL_CMD_CODE:
    {
      bt_qsoc_process_edl_events( ev_msg_ptr );
      break;
    }
    
    case BT_QSOC_NVM_ACCESS_CODE:
    {
      bt_qsoc_process_nvm_events( ev_msg_ptr );
      break;
    }

    default:
      break;
  }
} /* bt_qsoc_process_vs_event */


/*==========================================================================

  FUNCTION       BT_QSOC_PROCESS_VS_CMD_CMPLT_EVENT

  DESCRIPTION    This function handles command complete events resulting from
                 vendor specific commands sent to the BTS.

  DEPENDENCIES   None.

  PARAMETERS     ev_msg_ptr - vendor specific event data.

  RETURN VALUE   None.

  SIDE EFFECTS   None.

==========================================================================*/
void bt_qsoc_process_vs_cmd_cmplt_event
( 
  bt_ev_msg_type *ev_msg_ptr
)
{
  /* Clear the retry timer */
  rex_clr_timer( &bt_qsoc_retry_timer );

  /* Do processing based on exact event withing VS CMD event family */
  switch(ev_msg_ptr->ev_msg.ev_hc_vs_cmd_cmplt_hdr.sub_event)
  {
    case BT_QSOC_VS_CMD_READ_EKL_RESP:
      bt_soc_etc_send_cur_conn_enc_ev( ev_msg_ptr->ev_msg.ev_hc_vs_cmd_cmplt_hdr.params.ekl.ekl,
                                       ev_msg_ptr->ev_msg.ev_hc_vs_cmd_cmplt_hdr.params.ekl.conn_hndl);
      break;

    case BT_QSOC_VS_CMD_PAGE_CANCEL_RESP:
    case BT_QSOC_VS_CMD_READ_RSSI_LINEAR_RESP:      
    case BT_QSOC_VS_CMD_WRITE_RSSI_THRESHOLD_RESP:  
    case BT_QSOC_VS_CMD_READ_RSSI_THRESHOLD_RESP:   
    case BT_QSOC_VS_CMD_ABSENCE_NOTIFY_RESP:      
    case BT_QSOC_VS_CMD_MAX_ABSENCES_RESP:          
      break;

    default:
      break;
  }

} /* bt_qsoc_process_vs_cmd_cmplt_event */


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
void bt_qsoc_process_hci_reset
(
  bt_ev_msg_type *ev_msg_ptr
)
{
#ifdef T_QSC6270
  bt_soc_clock_sharing_type bt_clock_sharing;
#endif /* T_QSC6270 */

  switch(bt_qsoc_init_state)
  {
    case BT_QSOC_INIT_STARTED:
    case BT_QSOC_DETECT_FW:
    case BT_QSOC_POKE8_TBL_INIT:
    case BT_QSOC_FIXED_TBL_INIT:
    case BT_QSOC_DYN_TBL_INIT:
    case BT_QSOC_NV_TBL_INIT:
    case BT_QSOC_CUST_TBL_INIT:
      break;
  
    case BT_QSOC_HCI_RESET:
      /* Clear the timer, update state and call soc_etc_init for any 
       * non-SoC specific initialization 
       */
      rex_clr_timer( &bt_qsoc_retry_timer );
      rex_clr_sigs( &bt_tcb, BT_QSOC_RETRY_SIG );

      BT_MSG_HIGH( "BT QSOC: Rx HCI_RESET event. BTS init comlete", 0, 0, 0 );

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  /* Wait for SOC to  flow off on powerup and flow on again */
  bt_qsoc_inband_soc_flowed_on2 = FALSE;
  (void)rex_set_timer( &bt_qsoc_retry_timer, BT_QSOC_INBAND_CTS_DELAY);
#else
      bt_qsoc_init_state = BT_QSOC_INIT_COMPLETED;
      bt_qsoc_pwrup_rtry_cnt = 0;
      bt_soc_etc_init();

#ifdef FEATURE_BT_QSOC_SLEEP
      bt_qsoc_sleep_sm_init();
#endif /* FEATURE_BT_QSOC_SLEEP */
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

#if (defined(T_QSC1110) || defined(T_QSC1100))// Gemsea Modify

 /* Switch the clock control to BTS HW signalling. Disable manual override
  * after setting it to automatic mode.
  */
      pm_tcxo2_cmd(PM_TCXO_AUTOMATIC_MODE_CMD); 
      pm_tcxo2_cmd(PM_TCXO_XO_GP2_OUT_DISABLE);
#endif /* T_QSC1110 */

#ifdef T_QSC6270
      bt_cmd_dc_get_clock_sharing_type(&bt_clock_sharing);

      if( bt_clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED )
      {
      	/* Set TCXO to Automatic mode, BT SoC will control CLK_REQ  */
      	pm_tcxo2_cmd(PM_TCXO_AUTOMATIC_MODE_CMD); 
      	pm_tcxo2_cmd(PM_TCXO_BUFFER_OFF_CMD); 
      	pm_tcxo2_cmd(PM_TCXO_EMULATE_TCXO_EN_INACTIVE_CMD);
      }
#endif /* T_QSC6270 */

      break;

    case BT_QSOC_INIT_COMPLETED:
      break;

  }
} /* bt_qsoc_process_hci_reset */


/*==========================================================================

  FUNCTION       BT_QSOC_SHUTDOWN

  DESCRIPTION    This routine powers down BTS. It also performs any other 
                 clean up required.

  DEPENDENCIES   None.

  PARAMETERS     None.

  RETURN VALUE   None.

  SIDE EFFECTS   SIO driver will be put to sleep.

==========================================================================*/
void bt_qsoc_shutdown
(
  void
)
{
#ifdef T_QSC6270
  bt_soc_clock_sharing_type bt_clock_sharing;
#endif /* T_QSC6270 */

  /* 
    After SOC is shutdown, we need to change the QSOC INIT state. Also Inband
    sleep checks this state to see if it can restart Inband signalling .
   */
  bt_qsoc_init_state = BT_QSOC_SHUT_DOWN;

#ifdef FEATURE_BT_QSOC_POWERDOWN

#if (defined(T_QSC1110) || defined(T_QSC1100))// Gemsea Modify
  /* Change XO_GP1 to automatic (HW signalling based) mode and disable
     the buffer
   */
  pm_tcxo2_cmd(PM_TCXO_AUTOMATIC_MODE_CMD); 
  pm_tcxo2_cmd(PM_TCXO_XO_GP2_OUT_DISABLE);
#endif /* T_QSC1110 */

#ifdef T_QSC6270
  bt_cmd_dc_get_clock_sharing_type(&bt_clock_sharing);

  if( bt_clock_sharing == BT_SOC_CLOCK_SHARING_ENABLED )
  {
    /* Set back to Manual mode and disable TCXO */
    pm_tcxo2_cmd(PM_TCXO_MANUAL_MODE_CMD);
    pm_tcxo2_cmd(PM_TCXO_BUFFER_OFF_CMD); 
    pm_tcxo2_cmd(PM_TCXO_EMULATE_TCXO_EN_INACTIVE_CMD);
  }
  else
  {
    /* Enable LPM mode for GP1(VDD_BT) & GP2(VDD_PA) when BT is off */
    pm_lp_mode_control(PM_ON_CMD, PM_VREG_LP_GP1_ID);
    pm_lp_mode_control(PM_ON_CMD, PM_VREG_LP_GP2_ID);
  }

  /* Turn off VREG_GP1(VDD_BT) & VREG_GP2(VDD_PA) with sleep vote. 
      If there is no vote against sleep, it will turn off the LDO completely 
   */
  pm_vote_vreg_switch(PM_OFF_CMD, PM_VREG_GP1_ID, BT_PM_GP1_VOTE);
  pm_vote_vreg_switch(PM_OFF_CMD, PM_VREG_GP2_ID, BT_PM_GP2_VOTE);

#endif /* T_QSC6270 */

#if (defined (T_QSC60X5) || defined (T_QSC6270) || defined (T_QSC1110) || defined(T_QSC1100))// Gemsea Modify

  gpio_out( BT_PWR_EN, GPIO_LOW_VALUE );
  BT_MSG_HIGH( "BT QSOC Shutdown: Calling gpio_out - to turn off", 0, 0, 0 );

#elif ( (defined(T_MSM6246)) || (defined(T_MSM6290)) && \
        (defined(FEATURE_BT_QSOC_INBAND_SLEEP)) )

  gpio_out( GPIO_OUTP_66, GPIO_LOW_VALUE );

  (void)pm_vreg_control(PM_OFF_CMD,
                        (pm_vreg_masked_type)(1ULL << (uint32)PM_VREG_WLAN_ID));

  BT_MSG_HIGH( "BT QSOC Shutdow: Calling PMIC - to turn off", 0, 0, 0 );
#else

  /* Power-off BTS */
  pm_vote_vreg_switch( PM_OFF_CMD,
                       BT_PM_PWR_SRC,
                       BT_PM_APP_VOTE );
  BT_MSG_HIGH( "BT QSOC Shutdow: Calling PMIC - to turn off", 0, 0, 0 );

#endif /* T_QSC60X5 || T_QSC6270 || T_QSC1110 */
#endif /* FEATURE_BT_QSOC_POWERDOWN */

  rex_clr_timer( &bt_qsoc_retry_timer );
  rex_clr_sigs( &bt_tcb, BT_QSOC_RETRY_SIG );

#ifdef FEATURE_BT_QSOC_SLEEP
  /* Clear all timers/signals and reset sleep states */

#ifdef FEATURE_BT_QSOC_INBAND_SLEEP
  bt_qsoc_inband_soc_flowed_on1 = TRUE;
  bt_qsoc_inband_soc_flowed_on2 = TRUE;
#endif /* FEATURE_BT_QSOC_INBAND_SLEEP*/

  bt_qsoc_sleep_sm_reset();

#ifdef FEATURE_BT_QSOC_GPIO_WAKE
  /* Deassert EXT_WAKE signal in case we are not really powering off SoC */
  gpio_out(EXT_WAKE, BT_QSOC_EXT_WAKE_DEASSERTED);

  /* 
    We consider the BTS to be off at this point, so we disarm the HOST_WAKE ISR 
  */
  (void) gpio_int_set_handler(HOST_WAKE_INT, BT_QSOC_HOST_WAKE_ISR_ASSERTED, NULL); 
#endif /* FEATURE_BT_QSOC_GPIO_WAKE */

#endif /* FEATURE_BT_QSOC_SLEEP */

  /* 
    bt_qsoc_shutdown will also disable the SIO driver allowing BT to
    vote for sleep.
  */
  
#ifdef BT_QSOC_INBAND_TRACE_ENABLE
  BT_MSG_DEBUG( "BT QSOC Shutdown Disable Flow: state1: %d state2: %d msm_state %d", 
                 bt_qsoc_sleep_inband_get_rx_state(),
                 bt_qsoc_sleep_inband_get_tx_state(),
                 bt_qsoc_sleep_sm_get_msm_state() );  
#endif /* BT_QSOC_INBAND_TRACE_ENABLE */ 

#ifndef FEATURE_BT_QSOC_POWERDOWN
  bt_soc_disable_flow();
#endif /* FEATURE_BT_QSOC_POWERDOWN */

  bt_dc_update_sio_state( BT_SIO_SLEEPING, TRUE );

#ifdef FEATURE_BT_VOTING_32K_SLEEP_CLOCK
  pm_vote_clk_32k(PM_OFF_CMD, PM_32K_CLK_BT_APP);
#endif /* FEATURE_BT_VOTING_32K_SLEEP_CLOCK */

} /* bt_qsoc_shutdown */

#endif /* FEATURE_BT_QSOC */
#endif /* FEATURE_BT  */

