/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 B L U E T O O T H    S E R I A L    I N P U T

                        O U T P U T    M O D U L E

GENERAL DESCRIPTION
  This contains the code for the Bluetooth Serial Input Output (SIO) module.

              Copyright (c) 2000-2010 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btsio.c_v   1.20   25 Sep 2002 11:29:30   waynelee
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btsio.c#3 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btsio.c_v  $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2010-08-09   us  Sending a connection Fail event to the App when the steady 
                   state moniter timer expires. 
  2010-01-21  jtl  Allow apps to close SIO ports before they're fully opened
                   (aka canceling registration).
  2008-09-05   gs  Fixed race condition in NA enable/disable.  Enable/Disable
                   event now be sent only after sdp and rfcomm reg/re-deregistration.
  2008-09-01   gs  Added support for '+++' escape code detection under
                   FEATURE_BT_SIO_ESC_CODE_DETECTION.
                   Resolved a few issues in AT command processing.
  2008-07-23   gs  Fixed a few issues in BT SIO mode change function.  Added 
                   utility routines to unlink TX wm non-empty/gone-empty fn pts.
  2008-07-11   gs  Do proper cleanup if RFCOMM connection establishment fails
                   due to bad cmd-done event.
  2008-06-26   gs  Added support for sio ioctl command SIO_IOCTL_GET_CURRENT_RTS.

 *
 *    #61        15 May 2007            SSK
 * Fixed compiler warnings. 
 *
 *    #60        09 May 2007            SSK
 * Fixed compiler warnings.
 *
 *    #59        20 Feb 2007            DSN
 * Increasing the maximum number of events in BTSIO layer for SOC solutions.
 *
 *    #58        16 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *    #57        18 Dec 2006            JTL
 * Fix #54 to handle canceling connect when RC connect has not yet
 * been called.
 *
 *    #55-56     04 Dec 2006            JTL
 * Increase SS timer from 75 to 85 seconds. Timer expiration should be
 * revisited to properly clean up when it fires.
 *
 *    #54        03 Dec 2006            JTL
 * Make use of new RC Cancel Connect command for client streams.
 *
 *    #53        14 Nov 2006            GS
 * Check if there is enough room on BT SIO TX wm before enqueuing data.
 *
 *    #52        31 Aug 2006            JTL
 * Empty any pending data on the TX watermark when SIO gets connected.
 *
 *    #51        25 May 2006            JTL
 * Fix possible dereference of NULL ptr in bt_sio_cmd_open().
 *
 *    #50        5  Apr 2006            DP
 * Fix Unpairing UUID/SCNs for Servers upon connection failures.
 *
 *    #48-49     21 Mar 2006            JTL
 * Handle RC Unregistered events for servers. Delay sending SIO Close
 * status change until the event is received.
 *
 *    #47        9  Nov 2005            DP
 * Now use new SD service registration command soas to not
 * be connectable solely for the SD service registration.     
 * 
 *    #46        22 Oct 2005            JH
 * Replaced FEATURE_PCAA with FEATURE_PCAA_SIRIUS.
 *
 *    #45        28 Jul 2005            DP
 * Adjust system monitoring timeouts to expire in order from lowest
 * to highest layers.
 *
 *    #44        27 Jul 2005            SL
 * Made changes to support PCAA over SPP.
 *
 *    #43        12 Jul 2005            JH
 * Modified bt_sio_cmd_open() to return an error when app ID is unavailable.
 *
 *    #42        06 Jul 2005           JTL
 * Fixing possible overrun of bt_sio_table.
 *
 *    #41        7  Jun 2005            DP
 * Removed critical Lint error.
 *
 *    #40        31 May 2005            RY
 * Merged in Sirius changes.
 *
 *    #39        13 Apr 2005           JTL
 * Initializing the rex_crit_sect before using it.
 *
 *    #38        8  Apr 2005            DP
 * Make use of BT_BDA debug message macro (improving upon previous checkin's
 * use of similar debug message macros).
 *
 *    #37        7  Apr 2005            DP
 * Fixes to client side Mode 2 security.  Make use of BDA message macro.
 *
 *    #36        16 Mar 2005            DP
 * Fix Mode 2 security for client connections.  Increased maximum BT SIO
 * ports available.
 *
 *    #35        21 Jan 2005            PN
 * Added missing code to handle BT_CMD_SIO_BT_FLUSH_TX.  Realigned code to stay
 * within 80 characters/line.
 *
 *    #34        29 Dec 2004            JH
 * Replaced calls to bt_cmd_sd_register_service() with bt_cmd_sd_register_srv()
 * and bt_cmd_sd_unregister_service() with bt_cmd_sd_unregister_srv_by_scn().
 *
 *    #32-33     17 Dec 2004            JTL
 * Add more command queuing for the rest of the SIO driver. Now all non-read
 * IOCTLs and _transmit are queued.
 *
 *    #31        13 Dec 2004            PN
 * The command status is set for BT_CMD_SIO_IN_FLOW_ON and BT_CMD_SIO_IN_FLOW_OFF
 * when the commands are processed.
 *
 *    #30        07 Dec 2004            JTL
 * For flow on/off IOCTLs, queue the commands if tasks are locked.
 *
 *    #29        17 Nov 2004            PN
 * Cleans up properly when RC_CONNECTED received for a stream being closed.
 *
 *    #28        24 Sep 2004            DP
 * Reworked AMS compilation controls.
 *
 *    #27        3  Sep 2004            PN
 * Closing a connected client stream will cause SPP_DISCONNECTED state update.
 * Fixed problem in closing a client stream being opened.
 *
 *    #26        02 Sep 2004            JTL
 * Changed TASKLOCKs to rex_enter_crit_sect. Needed to protect the rest
 * of the accesses to the bt_sio_table similarly. Also, DSM callbacks now
 * generate events passed back to btsio itself, so that the handler can
 * operate outside of a tasklock.
 *
 *    #25        12 Aug 2004            PN
 * Fixed the misspelled "Dial-up".
 *
 *    #24        1  Jun 2004            DP
 * Made open errors clean up better.  Implemented Mode 2 security.
 *
 *    #23        17 May 2004            PN
 * Fixed problem with closing an enabled BT_NA port.
 *
 *    #22        11 May 2004            PN
 * Fixed SIO_IOCTL_FLUSH_TX command processing.
 *
 *    #21        9  Apr 2004            DP
 * Prevent same Stream ID on multiple BTSIO table entries.
 * Reduce background timer for cleanup of entries with open errors.
 *
 *    #20        30 Mar 2004            DP
 * Added EMS support.
 *
 *    #19        29 Jan 2004            DP
 * Improved NA connected/disconnected event sending.  Registering/
 * unregistering with RFCOMM is improved for NA.  Upon connection
 * failures server apps are now notified with cause and BD address
 * and then end up in OPEN state.
 *
 *    #18        14 Jan 2004            DP
 * SPP ports no longer affect discoverable mode automatically.
 * NA enable now registers with SD and sets general discoverable ON
 * using the enabling application's application ID.
 * NA disable now undoes the above.
 *
 *    #17        23 Dec 2003            PN
 * Put back the automatic service discoverable setting.
 *
 *    #16        19 Dec 2003            DP
 * No longer automatically set service discoverable for SPP ports.
 * Better handling of quick closing of ports.
 *
 *    #15        12 Dec 2003            DP
 * Allow same SCN for different BD addresses for client applications.
 * Add Steady State timer to ensure ports reach steady state or else
 * clean up table to prevent running out of entries.
 *
 *    #14        14 Nov 2003            DP
 * Do not output NA disconnected event due to initial NA enable.
 *
 *    #13        3  Oct 2003            DP
 * Updated watermark callback linking to prevent problems with application
 * mistakes.
 *
 *    #12        26 Sep 2003            DP
 * Made service name character string.  Updated get_status.
 *
 *    #11        22 Jul 2003            RY
 * Fixed corner case with sio_close when a RFCOMM Connected event is received.
 *
 *    Rev 1.20   25 Sep 2002 11:29:30   waynelee
 * Update watermarks when mode changes (for DUN).
 *
 *    Rev 1.19   11 Sep 2002 15:22:32   waynelee
 * Change to compile for non DATA_MM.
 *
 *    Rev 1.18   10 Sep 2002 17:39:30   waynelee
 * Added support for 3G DS.  Implemented flush_tx ioctl.  Save new
 * watermarks when mode changes.  Removed tcp_enqueue call; use rx
 * func ptr instead.
 *
 *    Rev 1.17   05 Aug 2002 18:00:10   ryuen
 * Merged changes from MSM_BT.01.00.11.01.02 (MSM5100 2.1 Patch 6)
 *
 *    Rev 1.16   07 Jun 2002 17:08:38   waynelee
 * Merged in changes necessary for Cheetah.
 *
 *    Rev 1.15.1.0   24 Jul 2002 12:14:30   propach
 * Improved SIO API for SPP.
 *
 *    Rev 1.15   03 Apr 2002 14:28:46   propach
 * Enhanced RFCOMM modem status changed events.
 *
 *    Rev 1.14   08 Feb 2002 17:49:30   ryuen
 * Added the support for FEATURE_BT_SIO_API_ENHANCEMENT
 *
 *    Rev 1.13   08 Feb 2002 10:52:12   propach
 * Data call drops now when RC is disconnected.
 *
 *    Rev 1.12   23 Oct 2001 16:52:14   propach
 * Added support for RFCOMM Credit Based Flow Control (CBFC).
 *
 *    Rev 1.11   10 Sep 2001 13:57:52   propach
 * Fixes to SPP update.
 *
 *    Rev 1.10   07 Sep 2001 16:12:28   propach
 * Optimized event handling.  Made compatible with new DSMs.
 * Improved support for SPP.
 *
 *    Rev 1.9   16 Mar 2001 17:30:34   propach
 * Made app ID get/free and RC registrations more consistent.
 *
 *    Rev 1.8   14 Mar 2001 17:14:32   sramacha
 * 1. Set phone in discoverable mode when NA is enabled.
 * 2. Updated dte_ready_on value in bt_sio_table so that DS ends a data call
 *    when the BT link goes down.
 *
 *    Rev 1.7   07 Mar 2001 16:45:56   propach
 * Update command status for BT NA commands.
 *
 *    Rev 1.6   02 Mar 2001 17:51:12   propach
 * Fixed NA connected event.
 *
 *    Rev 1.5   02 Mar 2001 01:21:04   ryuen
 * 1. Updated bt_sio_table_type to contain one appID for the user application
 *    and one appID for the port
 * 2. Removed bt_sio_app_id
 * 3. Renamed bt_sio_online and bt_sio_offline to bt_sio_qdsp_ready_init and
 *    bt_sio_qdsp_reset_shutdown respectively
 * 4. Removed to call to enable/disable the driver
 * 5. Removed the driver online/offline event processing
 * 6. Renamed bt_sio_initialize to bt_sio_powerup_init
 * 7. Upddated to use the new EC get_app_id_and_register API
 *
 *    Rev 1.4   23 Feb 2001 15:36:56   ryuen
 * Removed obsolete function bt_sio_cmd_initialize
 *
 *    Rev 1.3   16 Feb 2001 16:41:10   propach
 * Increased number of event buffers.
 *
 *    Rev 1.2   16 Feb 2001 16:11:58   propach
 * Completed EC routing of events by BT application ID.
 *
 *    Rev 1.1   07 Feb 2001 10:18:52   propach
 * Reworked internal BT application IDs and made event control
 * route based on these.
 *
 *    Rev 1.0   29 Jan 2001 17:10:34   propach
 * Initial revision.
 *
 *    Rev 1.21   29 Jan 2001 16:22:10   propach
 * Improved debug messages for IOCTL calls.
 *
 *    Rev 1.20   18 Jan 2001 11:36:04   waynelee
 * minor fix to make FEATURE_BT_TEST_MODE_ONLY build
 *
 *    Rev 1.19   21 Dec 2000 17:48:46   waynelee
 * Removed usage of bt_reocrd_fault().  Use BT_SET_PKT_TRACER().
 *
 *    Rev 1.18   08 Dec 2000 10:25:38   propach
 * Added BTSIO event queue.
 *
 *    Rev 1.17   01 Dec 2000 18:30:34   sramacha
 * Some minor changes in bt_sio_cmd_close and na_disconnected event.
 *
 *    Rev 1.16   01 Dec 2000 15:34:38   sramacha
 * Fixed updating bd_addr in bt_sio_table.
 *
 *    Rev 1.15   01 Dec 2000 11:09:58   sramacha
 * Changed BT port names to SIO_PORT_BT_NA and SIO_PORT_BT_SPP.
 *
 *    Rev 1.14   30 Nov 2000 17:39:16   sramacha
 * Removed BT_ENABLE and BT_DISABLE IOCTLs.  Added NA_CONNECTED and
 * NA_DISCONNECTED event generation.
 *
 *    Rev 1.13   29 Nov 2000 13:56:40   propach
 * Added NA enable/disable command support and removed FEATURE_BT_NA.
 *
 *    Rev 1.12   22 Nov 2000 16:35:22   propach
 * Clear RFCOMM PID after RC disconnect.
 *
 *    Rev 1.11   22 Nov 2000 10:39:42   propach
 * Redid entire module to generalize solution for SIO over BT.
 *
 *    Rev 1.10   30 Oct 2000 15:30:46   balas
 * Updated tx command. Added Enable DTR handling.
 *
 *    Rev 1.9   29 Sep 2000 17:36:26   waynelee
 * Fixed L2CAP so SDP works; fixed L2CAP for non-default MTU; added SDP entries;
 * fixed RD_BD_ADDR cmd_cmplt ev
 *
 *    Rev 1.0   29 Sep 2000 17:32:40   waynelee
 * Initial revision.
 *
 *    Rev 1.8   21 Sep 2000 15:22:14   ryuen
 * Moved the registration of the DRIVER_ONLINE and DRIVER_OFFLINE event from
 * bt_sio_cmd_open() to the processing of the SIO_IOCTL_BT_ENABLE command

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/00   sb      Changed transmit interface to issue RFCOMM commands using
           SIO watermarks. Added checks for a BT connection before
           issuing ioctl's. Changed bt_app_id to bt_na_app_id.
09/21/00   sb      Added flow control code
08/30/00   sb      Updates for Network Access & DS interfacing
08/10/00   la      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include <string.h>
#include "assert.h"
#include "dsm.h"
#include "err.h"
#include "msg.h"
#include "ps.h"
#include "rex.h"
#include "queue.h"
#include "task.h"

#include "bt.h"
#include "bti.h"
#include "btcmdi.h"
#include "btdebug.h"
#include "btmsg.h"
#include "btrc.h"
#include "btrcev.h"
#include "btrci.h"
#include "btrcmsgi.h"
#include "btrcrx.h"
#include "btrctx.h"
#include "btsd.h"
#include "btsio.h"
#include "btutils.h"


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#define BT_MSG_LAYER BT_MSG_NA

#define BT_SIO_SS_TIMEOUT_MS  85000
#define BT_SIO_SS_ERROR_TO_MS 5000

#define BT_NA_SERV_NAME "QC Dial-up Networking"
#define BT_NA_SERV_NAME_LEN (MIN( 21, BT_SD_MAX_SERVICE_NAME_LEN ))

#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
// Escape code ('+++') detection
#define BT_SIO_ESC_GUARD_TIME_MS 900
#define BT_SIO_ESC_SEQ_CHAR      '+'
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

/*-------------------------------------------------------------------------*/
/*  Information and queue for events destined for the BT SIO module.       */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_BT_SOC
  #define BT_SIO_NUM_EVENTS   100
#else
  #define BT_SIO_NUM_EVENTS   30
#endif /* FEATURE_BT_SOC */
LOCAL bt_event_q_info_type  bt_sio_ev_q_info;
LOCAL q_type                bt_sio_ev_q;

typedef enum
{
  NO_BYTES,
  GOT_A,
  GOT_AT
} bt_na_autodet_state;

typedef enum
{
  BT_RC_REG_UNREGISTERED,
  BT_RC_REG_REGISTERED,
  BT_RC_REG_REGISTERING,
  BT_RC_REG_UNREGISTERING
} bt_rc_reg_state_type; 

#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
typedef enum
{
  BT_SIO_ESC_SEQ_DISABLE,       // escape seq detection is disabled
  BT_SIO_ESC_SEQ_ENABLE,        // escape seq detection is enabled
  BT_SIO_ESC_SEQ_1ST_CHAR,      // 1st escape character rx'ed
  BT_SIO_ESC_SEQ_2ND_CHAR,      // 2nd escape character rx'ed
  BT_SIO_ESC_SEQ_3RD_CHAR       // 3rd escape character rx'ed
} bt_na_esc_enum_type;

typedef struct
{
  bt_na_esc_enum_type  esc_state;
  void                 (*esc_guardtime_ptr) ( unsigned long dummy );
  rex_timer_type       esc_timer;
} bt_sio_na_info_type;

LOCAL bt_sio_na_info_type  bt_sio_na_info;
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

typedef struct
{
  bt_app_id_type         bt_app_id_user;
  bt_app_id_type         bt_app_id_port;
  sio_port_id_type       sio_port_id;
  sio_stream_id_type     stream_id;
  bt_bd_addr_type        bd_addr;
  bt_spp_state_type      spp_state;
  bt_event_reason_type   spp_reason;
  rex_timer_cnt_type     ss_timer_ms_left;
  bt_rc_ev_config_type   rc_config;
  uint16                 rc_pid;
  boolean                client_app;
  boolean                sd_registered;
  bt_sd_uuid_type        service_uuid;
  uint16                 service_version;
  char                   service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN + 1 ];
  uint8                  rc_server_channel;
  bt_rc_reg_state_type   rc_registered;
  bt_sd_uuid_type        uuid_scn_paired_uuid;
  boolean                open;
  boolean                enabled;
  boolean                na_connected;
  bt_spp_ms_enum_type    local_modem_status;
  bt_spp_ms_enum_type    ms_changed_mask;
  bt_spp_ms_enum_type    far_modem_status;
  boolean                break_present;
  uint8                  break_length;
  uint8                  breaks;
  bt_spp_le_enum_type    cur_line_error;
  uint8                  overruns;
  uint8                  parity_errors;
  uint8                  framing_errors;
  sio_stream_mode_type   stream_mode;
  sio_flow_ctl_type      tx_flow_ctl;
  sio_flow_ctl_type      rx_flow_ctl;
  dsm_watermark_type*    tx_wm_ptr;
  dsm_watermark_type*    rx_wm_ptr;
  boolean                enable_auto_detect;
  bt_na_autodet_state    auto_detect_state;
  boolean                rx_flow_on;
  void                   (*orig_tx_non_empty_func_ptr)( void );
  void                   (*orig_tx_gone_empty_func_ptr)( void );
  void                   (*my_tx_non_empty_func_ptr)( void );
  void                   (*my_tx_gone_empty_func_ptr)( void );
  void                   (*dtr_cb_ptr)( void );
  void                   (*pkt_func_ptr)( void );
  void                   (*esc_func_ptr)( void );
  void                   (*close_func_ptr)( void );
  sio_rx_func_ptr_type   rx_func_ptr;
  void                   (*flush_tx_func_ptr)( void );
  void                   (*status_change_fptr)( bt_spp_status_type* );
  void                   (*config_change_fptr)( bt_spp_cfg_rpt_type* );
  void                   (*modem_status_fptr)( bt_spp_ms_type* );
  void                   (*line_error_fptr)( bt_spp_le_type* );
} bt_sio_table_type;


#define BT_SIO_NULL_IDX  0xFF
#define BT_MAX_SIO_PORTS BT_SPP_MAX_PORTS

#if (BT_MAX_SIO_PORTS > BT_SIO_NULL_IDX)
#error "BT_MAX_SIO_PORTS incompatible with BT_SIO_NULL_IDX!!!"
#endif

LOCAL bt_sio_table_type  bt_sio_table[ BT_MAX_SIO_PORTS ];

LOCAL bt_app_id_type bt_sio_app_id;
LOCAL boolean        bt_sio_connectable;


/*  BT_TBD:  This should be put into an SIO/DS .h file.   */
/*           It is used in several DS and SIO .c files.   */
#define SIO_ASYNC_FLAG 0x7E  /* PPP Flag character value. */

/* Used to protect the bt_sio_table */
LOCAL rex_crit_sect_type bt_sio_crit_sect;

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION
  bt_sio_initialize_entry

DESCRIPTION
  Initializes a specified BT SIO table entry.

===========================================================================*/
LOCAL void bt_sio_initialize_entry
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_sio_table[ i ].bt_app_id_user              = BT_APP_ID_NULL;
  bt_sio_table[ i ].bt_app_id_port              = BT_APP_ID_NULL;
  bt_sio_table[ i ].sio_port_id                 = SIO_PORT_NULL;
  bt_sio_table[ i ].stream_id                   = SIO_NO_STREAM_ID;
  bt_sio_table[ i ].spp_state                   = BT_SPP_ST_CLOSED;
  bt_sio_table[ i ].spp_reason                  = BT_EVR_GN_SUCCESS;
  bt_sio_table[ i ].ss_timer_ms_left            = 0;
  bt_sio_table[ i ].rc_pid                      = BT_RC_NULL_PID;
  bt_sio_table[ i ].client_app                  = FALSE;
  bt_sio_table[ i ].sd_registered               = FALSE;
  bt_sio_table[ i ].service_uuid                = BT_SD_INVALID_UUID;
  bt_sio_table[ i ].service_version             = 0;
  bt_sio_table[ i ].service_name_str[ 0 ]       = 0x0;
  bt_sio_table[ i ].rc_server_channel           = BT_RC_SC_NULL;
  bt_sio_table[ i ].rc_registered               = BT_RC_REG_UNREGISTERED;
  bt_sio_table[ i ].uuid_scn_paired_uuid        = BT_SD_INVALID_UUID;
  bt_sio_table[ i ].open                        = FALSE;
  bt_sio_table[ i ].enabled                     = FALSE;
  bt_sio_table[ i ].na_connected                = FALSE;
  bt_sio_table[ i ].local_modem_status 
    = (bt_spp_ms_enum_type)BT_RC_MS_LOC_DEFAULT_V;
  bt_sio_table[ i ].ms_changed_mask             = BT_SPP_MS_NONE;
  bt_sio_table[ i ].far_modem_status            = BT_SPP_MS_NONE;
  bt_sio_table[ i ].break_present               = FALSE;
  bt_sio_table[ i ].break_length                = 0;
  bt_sio_table[ i ].breaks                      = 0;
  bt_sio_table[ i ].cur_line_error              = BT_SPP_LE_NONE;
  bt_sio_table[ i ].overruns                    = 0;
  bt_sio_table[ i ].parity_errors               = 0;
  bt_sio_table[ i ].framing_errors              = 0;
  bt_sio_table[ i ].stream_mode                 = SIO_DS_AUTODETECT_MODE;
  bt_sio_table[ i ].tx_flow_ctl                 = SIO_FCTL_BEST;
  bt_sio_table[ i ].rx_flow_ctl                 = SIO_FCTL_BEST;
  bt_sio_table[ i ].tx_wm_ptr                   = NULL;
  bt_sio_table[ i ].rx_wm_ptr                   = NULL;
  bt_sio_table[ i ].enable_auto_detect          = FALSE;
  bt_sio_table[ i ].auto_detect_state           = NO_BYTES;
  bt_sio_table[ i ].rx_flow_on                  = TRUE;
  bt_sio_table[ i ].orig_tx_non_empty_func_ptr  = NULL;
  bt_sio_table[ i ].orig_tx_gone_empty_func_ptr = NULL;
  bt_sio_table[ i ].dtr_cb_ptr                  = NULL;
  bt_sio_table[ i ].pkt_func_ptr                = NULL;
  bt_sio_table[ i ].esc_func_ptr                = NULL;
  bt_sio_table[ i ].close_func_ptr              = NULL;
  bt_sio_table[ i ].rx_func_ptr                 = NULL;
  bt_sio_table[ i ].flush_tx_func_ptr           = NULL;
  bt_sio_table[ i ].status_change_fptr          = NULL;
  bt_sio_table[ i ].config_change_fptr          = NULL;
  bt_sio_table[ i ].modem_status_fptr           = NULL;
  bt_sio_table[ i ].line_error_fptr             = NULL;

  bt_sio_table[ i ].rc_config.pn_negot_done  = FALSE;
  bt_sio_table[ i ].rc_config.rpn_negot_done = FALSE;
  bt_sio_table[ i ].rc_config.max_frame_size = BT_MTP_RC_INFO_LEN;
  bt_sio_table[ i ].rc_config.priority       = 1;
  bt_sio_table[ i ].rc_config.cb_flow_ctrl   = TRUE;
  bt_sio_table[ i ].rc_config.baudrate       = RCMCCM_RPN_BR_DEFAULT_V;
  bt_sio_table[ i ].rc_config.format         = RCMCCM_RPN_FMT_DEFAULT_V;
  bt_sio_table[ i ].rc_config.flow_ctrl      = RCMCCM_RPN_FC_DEFAULT_V;
  bt_sio_table[ i ].rc_config.xon_char       = RCMCCM_RPN_XON_DEFAULT_V;
  bt_sio_table[ i ].rc_config.xoff_char      = RCMCCM_RPN_XOFF_DEFAULT_V;

  memset( (byte*)(&bt_sio_table[ i ].bd_addr), 0,
          sizeof( bt_bd_addr_type ) );

}


/*===========================================================================

FUNCTION
  bt_sio_qdsp_ready_init

DESCRIPTION
  Initialize the SIO module when the Bluetooth QDSP image is loaded

===========================================================================*/
void bt_sio_qdsp_ready_init( void )
{
}


/*===========================================================================

FUNCTION
  bt_sio_qdsp_reset_shutdown

DESCRIPTION
  De-initialize the SIO module when the Bluetooth QDSP image is unloaded

===========================================================================*/
void bt_sio_qdsp_reset_shutdown( void )
{
}


/*===========================================================================

FUNCTION
  bt_sio_find_entry_app_id_port

DESCRIPTION
  Finds an entry in the bt_sio_table with a specified BT application
  ID port.

RETURN VALUE
  Index if found; otherwise BT_SIO_NULL_IDX.

===========================================================================*/
LOCAL uint8 bt_sio_find_entry_app_id_port
(
  bt_app_id_type  app_id
)
{

  uint8  i = 0;
  uint8  return_idx = BT_SIO_NULL_IDX;

  while ( (return_idx == BT_SIO_NULL_IDX) && (i < BT_MAX_SIO_PORTS) )
  {
    if ( bt_sio_table[ i ].bt_app_id_port == app_id )
    {
      return_idx = i;
    }
    i++;
  }

  return ( return_idx );

}


/*===========================================================================

FUNCTION
  bt_sio_find_entry_sio_port_id

DESCRIPTION
  Finds an entry in the bt_sio_table for a specified SIO port ID.

RETURN VALUE
  Index if found; otherwise BT_SIO_NULL_IDX.

===========================================================================*/
LOCAL uint8 bt_sio_find_entry_sio_port_id
(
  sio_port_id_type  sio_port_id
)
{

  uint8  i = 0;
  uint8  return_idx = BT_SIO_NULL_IDX;

  while ( (return_idx == BT_SIO_NULL_IDX) && (i < BT_MAX_SIO_PORTS) )
  {
    if ( bt_sio_table[ i ].sio_port_id == sio_port_id )
    {
      return_idx = i;
    }
    i++;
  }

  return ( return_idx );

}


/*===========================================================================

FUNCTION
  bt_sio_find_entry_rc_pid

DESCRIPTION
  Finds an entry in the bt_sio_table for a specified RFCOMM PID.

RETURN VALUE
  Index if found; otherwise BT_SIO_NULL_IDX.

===========================================================================*/
LOCAL uint8 bt_sio_find_entry_rc_pid
(
  uint16  rc_pid
)
{

  uint8  i = 0;
  uint8  return_idx = BT_SIO_NULL_IDX;

  while ( (return_idx == BT_SIO_NULL_IDX) && (i < BT_MAX_SIO_PORTS) )
  {
    if ( bt_sio_table[ i ].rc_pid == rc_pid )
    {
      return_idx = i;
    }
    i++;
  }

  return ( return_idx );

}


/*===========================================================================

FUNCTION
  bt_sio_find_entry_stream_id

DESCRIPTION
  Finds an entry in the bt_sio_table for a specified SIO stream ID.

RETURN VALUE
  Index if found; otherwise BT_SIO_NULL_IDX.

===========================================================================*/
LOCAL uint8 bt_sio_find_entry_stream_id
(
  sio_stream_id_type  stream_id
)
{

  uint8  i = 0;
  uint8  return_idx = BT_SIO_NULL_IDX;

  if ( stream_id != SIO_NO_STREAM_ID )
  {
    while ( (return_idx == BT_SIO_NULL_IDX) && (i < BT_MAX_SIO_PORTS) )
    {
      if ( bt_sio_table[ i ].stream_id == stream_id )
      {
        return_idx = i;
      }
      i++;
    }
  }

  return ( return_idx );

}


/*===========================================================================

FUNCTION
  bt_sio_find_entry_rc_server_channel

DESCRIPTION
  Finds an entry in the bt_sio_table for a specified application
  type (server/client) with a specified RFCOMM server channel number,
  and for client applications the BD address must also match.

RETURN VALUE
  Index if found; otherwise BT_SIO_NULL_IDX.

===========================================================================*/
LOCAL uint8 bt_sio_find_entry_rc_server_channel
(
  boolean           client_app,
  uint8             rc_server_channel,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  uint8  i = 0;
  uint8  return_idx = BT_SIO_NULL_IDX;

  while ( (return_idx == BT_SIO_NULL_IDX) && (i < BT_MAX_SIO_PORTS) )
  {
    if ( (bt_sio_table[ i ].client_app == client_app) &&
         (bt_sio_table[ i ].rc_server_channel == rc_server_channel) )
    {
      if ( (client_app == FALSE) ||
           (BT_BD_ADDRS_EQUAL( &bt_sio_table[ i ].bd_addr,
                               bd_addr_ptr)) )
      {
        return_idx = i;
      }
    }
    i++;
  }

  return ( return_idx );

}


/*===========================================================================

FUNCTION
  bt_sio_check_connectable_setting

DESCRIPTION
  This routine updates the connectable setting as necessary.

===========================================================================*/
LOCAL void bt_sio_check_connectable_setting
(
  void
)
{

  uint8    i = 0;
  boolean  cur_connectable = FALSE;

  while ( (cur_connectable == FALSE) && (i < BT_MAX_SIO_PORTS) )
  {
    if ( (bt_sio_table[ i ].bt_app_id_port != BT_APP_ID_NULL) && 
         (bt_sio_table[ i ].client_app == FALSE) &&
         (bt_sio_table[ i ].open != FALSE) &&
         (bt_sio_table[ i ].enabled != FALSE) &&
         (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPEN) )
    {
      cur_connectable = TRUE;
    }
    i++;
  }

  if ( bt_sio_connectable != cur_connectable )
  {
    bt_sio_connectable = cur_connectable;

    BT_MSG_HIGH( "BT SIO: Connectable %x", bt_sio_connectable, 0, 0 );
    BT_MSG_API( "BT SIO CMD TX: RM SetConn %x AID %x",
                bt_sio_connectable, bt_sio_app_id, 0 );
    bt_cmd_rm_set_connectable( bt_sio_app_id, bt_sio_connectable,
                               BT_RM_AVP_AUTOMATIC );
  }
}


/*===========================================================================

FUNCTION
  bt_sio_get_status

DESCRIPTION
  Gets the status for a specified port.

===========================================================================*/
LOCAL void bt_sio_get_status
(
  uint8                i,            /*  bt_sio_table index.  */
  bt_spp_status_type*  spp_stat_ptr
)
{

  spp_stat_ptr->stream_id         = bt_sio_table[ i ].stream_id;
  spp_stat_ptr->bd_addr           = bt_sio_table[ i ].bd_addr;
  spp_stat_ptr->client_app        = bt_sio_table[ i ].client_app;
  spp_stat_ptr->service_uuid      = bt_sio_table[ i ].service_uuid;
  spp_stat_ptr->service_version   = bt_sio_table[ i ].service_version;
  spp_stat_ptr->rc_server_channel = bt_sio_table[ i ].rc_server_channel;
  spp_stat_ptr->spp_state         = bt_sio_table[ i ].spp_state;
  spp_stat_ptr->spp_reason        = bt_sio_table[ i ].spp_reason;

}


/*===========================================================================

FUNCTION
  bt_sio_status_updated

DESCRIPTION
  Reports status updates for a specified port as necessary.

===========================================================================*/
LOCAL void bt_sio_status_updated
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_spp_status_type  spp_status;

  if ( (bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_SPP) &&
       (bt_sio_table[ i ].status_change_fptr != NULL) )
  {
    bt_sio_get_status( i, &spp_status );
    
    ( bt_sio_table[ i ].status_change_fptr )( &spp_status );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_set_spp_state

DESCRIPTION
  Sets the SPP state to a specified value for a specified port.

===========================================================================*/
LOCAL void bt_sio_set_spp_state
(
  uint8              i,         /*  bt_sio_table index.  */
  bt_spp_state_type  spp_state
)
{

  bt_ev_msg_type     ev_na;

  bt_sio_table[ i ].spp_state = spp_state;

  bt_sio_status_updated( i );

  if ( bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_NA )
  {
    if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_CONNECTED) &&
         (bt_sio_table[ i ].na_connected == FALSE) )
    {
      bt_sio_table[ i ].na_connected = TRUE;
      
      BT_MSG_API( "BT NA EV TX: NA Connected", 0, 0, 0 );
      ev_na.ev_hdr.ev_type   = BT_EV_NA_CONNECTED;
      ev_na.ev_hdr.bt_app_id = bt_sio_table[ i ].bt_app_id_user;
      ev_na.ev_msg.ev_na_conn.bd_addr = bt_sio_table[ i ].bd_addr;

      bt_ec_send_event( &ev_na );
    }
    else if ( ((bt_sio_table[ i ].spp_state == BT_SPP_ST_OPEN) ||
               (bt_sio_table[ i ].spp_state == BT_SPP_ST_CLOSED)) &&
              (bt_sio_table[ i ].na_connected != FALSE) )
    {
      bt_sio_table[ i ].na_connected = FALSE;

      BT_MSG_API( "BT NA EV TX: NA Disconnected", 0, 0, 0 );
      ev_na.ev_hdr.ev_type   = BT_EV_NA_DISCONNECTED;
      ev_na.ev_hdr.bt_app_id = bt_sio_table[ i ].bt_app_id_user;
      ev_na.ev_msg.ev_na_disc.bd_addr = bt_sio_table[ i ].bd_addr;
      ev_na.ev_msg.ev_na_disc.event_reason = bt_sio_table[ i ].spp_reason;

      bt_ec_send_event( &ev_na );
    }
  }

  bt_sio_check_connectable_setting();

}


/*===========================================================================

FUNCTION
  bt_sio_config_updated

DESCRIPTION
  Reports configuration updates for a specified port as necessary.

===========================================================================*/
LOCAL void bt_sio_config_updated
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_spp_cfg_rpt_type  spp_cfg_rpt;

  if ( (bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_SPP) &&
       (bt_sio_table[ i ].config_change_fptr != NULL) )
  {
    spp_cfg_rpt.stream_id = bt_sio_table[ i ].stream_id;

    spp_cfg_rpt.spp_config.max_frame_size =
      bt_sio_table[ i ].rc_config.max_frame_size;
    spp_cfg_rpt.spp_config.priority =
      bt_sio_table[ i ].rc_config.priority;
    spp_cfg_rpt.spp_config.cb_flow_ctrl =
      bt_sio_table[ i ].rc_config.cb_flow_ctrl;
    spp_cfg_rpt.spp_config.baudrate =
      bt_sio_table[ i ].rc_config.baudrate;
    spp_cfg_rpt.spp_config.format =
      bt_sio_table[ i ].rc_config.format;
    spp_cfg_rpt.spp_config.flow_ctrl =
      bt_sio_table[ i ].rc_config.flow_ctrl;
    spp_cfg_rpt.spp_config.xon_char =
      bt_sio_table[ i ].rc_config.xon_char;
    spp_cfg_rpt.spp_config.xoff_char =
      bt_sio_table[ i ].rc_config.xoff_char;

    ( bt_sio_table[ i ].config_change_fptr )( &spp_cfg_rpt );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_modem_status_updated

DESCRIPTION
  Reports modem status updates for a specified port as necessary.

===========================================================================*/
LOCAL void bt_sio_modem_status_updated
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_spp_ms_type  spp_ms;

  if ( (bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_SPP) &&
       (bt_sio_table[ i ].modem_status_fptr != NULL) )
  {
    spp_ms.stream_id       = bt_sio_table[ i ].stream_id;
    spp_ms.ms_changed_mask = bt_sio_table[ i ].ms_changed_mask;
    spp_ms.modem_status    = bt_sio_table[ i ].far_modem_status;
    spp_ms.break_present   = bt_sio_table[ i ].break_present;
    spp_ms.break_length    = bt_sio_table[ i ].break_length;
    spp_ms.breaks          = bt_sio_table[ i ].breaks;

    ( bt_sio_table[ i ].modem_status_fptr )( &spp_ms );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_line_errors_updated

DESCRIPTION
  Reports line error updates for a specified port as necessary.

===========================================================================*/
LOCAL void bt_sio_line_errors_updated
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_spp_le_type  spp_le;

  if ( (bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_SPP) &&
       (bt_sio_table[ i ].line_error_fptr != NULL) )
  {
    spp_le.stream_id      = bt_sio_table[ i ].stream_id;
    spp_le.cur_line_error = bt_sio_table[ i ].cur_line_error;
    spp_le.overruns       = bt_sio_table[ i ].overruns;
    spp_le.parity_errors  = bt_sio_table[ i ].parity_errors;
    spp_le.framing_errors = bt_sio_table[ i ].framing_errors;

    ( bt_sio_table[ i ].line_error_fptr )( &spp_le );
  }

}

#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
/*===========================================================================

FUNCTION
 bt_sio_esc_guard_time_cb

DESCRIPTION

===========================================================================*/
LOCAL void bt_sio_esc_guard_time_cb
(
 unsigned long dummy
)
{
  void (*func_ptr)(void) = NULL;
  uint8 i;
 
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( ( i = bt_sio_find_entry_sio_port_id(SIO_PORT_BT_NA) ) != BT_SIO_NULL_IDX ) 
  {
    switch ( bt_sio_na_info.esc_state ) 
    {
      case BT_SIO_ESC_SEQ_3RD_CHAR:
      {
        // The final guardtime occurred after receiving the third
        // escape sequence character.  This is a valid escape sequence.
        // Call the escape sequence callback function.
        func_ptr                 = bt_sio_table[ i ].esc_func_ptr;
        bt_sio_na_info.esc_state = BT_SIO_ESC_SEQ_DISABLE;

        // Start timer to wait for another guardtime.
        rex_set_timer( &bt_sio_na_info.esc_timer,
                        BT_SIO_ESC_GUARD_TIME_MS );
        break;
      }
    case BT_SIO_ESC_SEQ_DISABLE:
    case BT_SIO_ESC_SEQ_ENABLE:
    case BT_SIO_ESC_SEQ_1ST_CHAR:
    case BT_SIO_ESC_SEQ_2ND_CHAR:
    default:
      bt_sio_na_info.esc_state = BT_SIO_ESC_SEQ_ENABLE;
      break;
    }
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );
 
  if ( func_ptr != NULL ) 
  {
    (*func_ptr)();
  }
}
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

/*===========================================================================

FUNCTION
  bt_sio_process_rc_rx_data_na

DESCRIPTION
  Processes data received by RFCOMM specifically meant for the
  Network Access application.

===========================================================================*/
LOCAL void bt_sio_process_rc_rx_data_na
(
  uint8            sio_idx,           /*  bt_sio_table index.  */
  dsm_item_type**  dsm_ptr_ptr
)
{

  byte                rx_char;
  int                 i;
  uint16              pkt_length;
  uint16              total_bytes;
  dsm_item_type*      cur_item_ptr;
  dsm_item_type*      next_item_ptr;
  word                cur_used;
  uint16              cur_idx;
  boolean             item_has_valid_data = TRUE;
  bt_sio_table_type*  sio_ptr = &bt_sio_table[ sio_idx ];

  if ( sio_ptr->rc_pid != BT_RC_NULL_PID )
  {
    switch ( sio_ptr->stream_mode )
    {
      case SIO_DS_AUTODETECT_MODE:
        {
          /*  Process each character in the buffer until an  */
          /*  AT command or a PPP packet is detected.        */
          cur_item_ptr = *dsm_ptr_ptr;
          pkt_length   = dsm_length_packet( cur_item_ptr );
          total_bytes  = 0;
          cur_used     = cur_item_ptr->used;
          cur_idx      = 0;

          for ( i = 0; i < pkt_length; i++ )
          {
            if ( cur_used == 0 )
            {
              cur_item_ptr = cur_item_ptr->pkt_ptr;
              cur_used     = cur_item_ptr->used;
              cur_idx      = 0;
            }
            rx_char = *((byte*)(cur_item_ptr->data_ptr + cur_idx));
            cur_used--;
            cur_idx++;
            total_bytes++;

            switch ( sio_ptr->auto_detect_state )
            {
              case NO_BYTES:
                {
                  if ( (rx_char == SIO_ASYNC_FLAG) && (pkt_length >= 2) )
                  {
                    i = pkt_length - 1;
                    if ( sio_ptr->pkt_func_ptr != NULL )
                    {
                      (sio_ptr->pkt_func_ptr)();
                    }
                    else
                    {
                      BT_ERR( "BT SIO: No Pkt Fcn Ptr!!! %x", sio_idx, 0, 0 );
                    }
                  }
                  else if ( (rx_char == 'A') || (rx_char == 'a') )
                  {
                    sio_ptr->auto_detect_state = GOT_A;
                  }
                  else
                  {
                    item_has_valid_data = FALSE;
                    i = pkt_length;
                  }
                  break;
                }

              case GOT_A:
                {
                  if( (rx_char == '/') ||
                      (rx_char == 'T') || (rx_char == 't') )
                  {
                    sio_ptr->auto_detect_state = GOT_AT;
                  }
                  else if ( (rx_char != 'A') && (rx_char != 'a') )
                  {
                    // throw away analyzed data and restart reading command
                    dsm_pullup(dsm_ptr_ptr, NULL, total_bytes);
                    cur_item_ptr = *dsm_ptr_ptr;
                    pkt_length   = dsm_length_packet( cur_item_ptr );
                    total_bytes  = 0;
                    cur_used     = cur_item_ptr->used;
                    cur_idx      = 0;
                    i            = -1;

                    item_has_valid_data = FALSE;
                    sio_ptr->auto_detect_state = NO_BYTES;
                  }
                  break;
                }

              case GOT_AT:
                {
                  i = pkt_length;
                  break;
                }
              default:
                {
                  item_has_valid_data = FALSE;
                  BT_ERR( "BT SIO: NA bad AD state %x Idx %x",
                          sio_ptr->auto_detect_state, sio_idx, 0 );
                  break;
                }
            }
          }
          break;
        }

      case SIO_DS_RAWDATA_MODE:
      case SIO_DS_PKT_MODE:
#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
        {
          uint8               esc_code[3];
          bt_na_esc_enum_type esc_stat = bt_sio_na_info.esc_state;

          switch ( esc_stat ) 
          {
            case BT_SIO_ESC_SEQ_ENABLE:
              total_bytes = 3;
              break;
            case BT_SIO_ESC_SEQ_1ST_CHAR:
              total_bytes = 2;
              break;
            case BT_SIO_ESC_SEQ_2ND_CHAR:
              total_bytes = 1;
              break;
            case BT_SIO_ESC_SEQ_DISABLE:
            case BT_SIO_ESC_SEQ_3RD_CHAR:
            default:
              total_bytes = 0;
              break;
          }

          // Check if there are not too many characters for an escape sequence
          if ( dsm_length_packet (*dsm_ptr_ptr) <= total_bytes ) 
          {
            total_bytes = dsm_extract( *dsm_ptr_ptr, 0, &esc_code[0], total_bytes );

            for ( pkt_length = 0; 
                  ( pkt_length < total_bytes ) && ( esc_code[pkt_length] == BT_SIO_ESC_SEQ_CHAR );
                  pkt_length++);

            if (pkt_length == total_bytes) 
            {
              esc_stat += total_bytes;
            }
            else 
            {
              // Not an escape sequence. Reset the gaurdtime
              esc_stat = BT_SIO_ESC_SEQ_DISABLE;
            }
          }
          else 
          {
            // Reset the guardtime
            esc_stat = BT_SIO_ESC_SEQ_DISABLE;
          }

          bt_sio_na_info.esc_state = esc_stat;

          // Start timer to wait for another guardtime
          rex_set_timer( &bt_sio_na_info.esc_timer,
                       BT_SIO_ESC_GUARD_TIME_MS );
          break;
        }
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

      case SIO_DM_MODE:
      case SIO_GENERIC_MODE:
        {
          break;
        }
      default:
        {
          item_has_valid_data = FALSE;
          BT_ERR( "BT SIO: NA bad Stream mode %x Idx %x",
                  sio_ptr->stream_mode, sio_idx, 0 );
          break;
        }
    }
  }
  else
  {
    item_has_valid_data = FALSE;
  }

  if ( item_has_valid_data != FALSE )
  {
    if ( (sio_ptr->rx_func_ptr != NULL) || (sio_ptr->rx_wm_ptr != NULL) )
    {
      cur_item_ptr = *dsm_ptr_ptr;
      while ( cur_item_ptr != NULL )
      {
        next_item_ptr = cur_item_ptr->pkt_ptr;
        cur_item_ptr->pkt_ptr = NULL;
        BT_SET_PKT_TRACER( cur_item_ptr, DSM_BT_RX_RC_NA );
        if ( sio_ptr->rx_func_ptr != NULL )
        {
#ifdef FEATURE_DATA_MM
          cur_item_ptr->kind = DSM_PS_RAW_APP_PAYLOAD;
#else
          cur_item_ptr->kind = PS_RAW_APP_PAYLOAD;
#endif /* FEATURE_DATA_MM */
  
          (sio_ptr->rx_func_ptr)( &cur_item_ptr );
        }
        else
        {
          dsm_enqueue( sio_ptr->rx_wm_ptr, &cur_item_ptr );
        }
        cur_item_ptr = next_item_ptr;
      }
      *dsm_ptr_ptr = NULL;
    }
    else
    {
      /*  Nowhere to deliver received data.  */
      BT_ERR( "BT SIO: Bad RX ptrs SID %x PID %x",
              sio_ptr->stream_id, sio_ptr->rc_pid, 0 );
      dsm_free_packet( dsm_ptr_ptr );
    }
  }
  else
  {
    dsm_free_packet( dsm_ptr_ptr );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_process_rc_rx_data

DESCRIPTION
  Processes data received by RFCOMM on a specified PID as necessary
  when it is intended for either the Network Access port or an SPP port.

RETURN VALUE
  dsm_ptr is set to NULL if the data was consumed by BTSIO.

===========================================================================*/
void bt_sio_process_rc_rx_data
(
  uint16           pid,
  dsm_item_type**  dsm_ptr_ptr
)
{

  uint16          i;
  dsm_item_type*  cur_dsm_ptr;
  dsm_item_type*  next_dsm_ptr;

  if ( ( i = bt_sio_find_entry_rc_pid( pid ) ) != BT_SIO_NULL_IDX )
  {
    if ( bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_NA )
    {
      bt_sio_process_rc_rx_data_na( i, dsm_ptr_ptr );
    }
    else if ( (bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_SPP) &&
              (bt_sio_table[ i ].rx_func_ptr != NULL) )
    {
      cur_dsm_ptr = *dsm_ptr_ptr;
      while ( cur_dsm_ptr != NULL )
      {
        next_dsm_ptr = cur_dsm_ptr->pkt_ptr;
        cur_dsm_ptr->pkt_ptr = NULL;

        ( bt_sio_table[ i ].rx_func_ptr )( &cur_dsm_ptr );

        cur_dsm_ptr = next_dsm_ptr;
      }
      *dsm_ptr_ptr = NULL;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_sio_store_bt_event

DESCRIPTION
  Stores a specified BT event on the BT SIO BT event queue.

===========================================================================*/
void bt_sio_store_bt_event
(
  bt_ev_msg_type*  bt_ev_ptr
)
{

  bt_store_bt_event( bt_ev_ptr, &bt_sio_ev_q_info );

}


/*===========================================================================

FUNCTION
  bt_sio_write_local_modem_status

DESCRIPTION
  Updates the local_modem_status field of a specified BT SIO table
  entry.

===========================================================================*/
LOCAL void bt_sio_write_local_modem_status
(
  uint8  i,              /*  bt_sio_table index.  */
  uint8  ms_valid_mask,
  uint8  modem_status
)
{

  bt_sio_table[ i ].local_modem_status &= ~ms_valid_mask;
  bt_sio_table[ i ].local_modem_status |=
    ( ms_valid_mask & modem_status );

  if ( bt_sio_table[ i ].rc_pid != BT_RC_NULL_PID )
  {
    bt_cmd_rc_write_modem_status(
      bt_sio_table[ i ].bt_app_id_port,
      bt_sio_table[ i ].rc_pid,
      BT_RC_MS_SIGS_M,
      bt_sio_table[ i ].local_modem_status,
      FALSE,  /*  No break signal.  */
      0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_write_far_modem_status

DESCRIPTION
  Updates as specified the far_modem_status field of a specified BT SIO
  table entry.  If DTR is changed, the application is notified.

===========================================================================*/
LOCAL void bt_sio_write_far_modem_status
(
  uint8                i,  /*  bt_sio_table index.  */
  bt_spp_ms_enum_type  modem_status
)
{

  boolean  dtr_changed = FALSE;

  if ( BT_RC_MS_RTC_ASSERTED_B &
       (bt_sio_table[ i ].far_modem_status ^ modem_status) )
  {
    dtr_changed = TRUE;
  }

  bt_sio_table[ i ].far_modem_status = modem_status;

  if ( (dtr_changed != FALSE) &&
       (bt_sio_table[ i ].dtr_cb_ptr != NULL) )
  {
    (*(bt_sio_table[ i ].dtr_cb_ptr))();
  }

  bt_sio_modem_status_updated( i );

}


/*===========================================================================

FUNCTION
  bt_sio_rc_port_initialize

DESCRIPTION
  Initializes an RFCOMM port appropriately for the current state
  of SIO_IOCTLs associated with that port.

===========================================================================*/
void bt_sio_rc_port_initialize
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_spp_ms_enum_type  far_modem_status;
  uint8                ignored;

  if( ( bt_sio_table[ i ].tx_wm_ptr != NULL )          &&
      ( bt_sio_table[ i ].tx_wm_ptr->current_cnt != 0 ) )
  {
    BT_ERR("BT SIO: Emptying non-empty WM. AID %x SID %x PID %x",
           bt_sio_table[ i ].bt_app_id_port,
           bt_sio_table[ i ].stream_id,
           bt_sio_table[ i ].rc_pid );
    dsm_empty_queue( bt_sio_table[ i ].tx_wm_ptr );
  }

  bt_cmd_rc_set_port_rx_fc(
    bt_sio_table[ i ].bt_app_id_port,
    bt_sio_table[ i ].rc_pid,
    bt_sio_table[ i ].rx_flow_on );

  bt_cmd_rc_write_modem_status(
    bt_sio_table[ i ].bt_app_id_port,
    bt_sio_table[ i ].rc_pid,
    BT_RC_MS_SIGS_M,
    bt_sio_table[ i ].local_modem_status,
    FALSE,  /*  No break signal.  */
    0 );

  if ( bt_cmd_rc_read_modem_status(
         bt_sio_table[ i ].bt_app_id_port,
         bt_sio_table[ i ].rc_pid,
         (uint8*)&far_modem_status,
         &ignored,  /*  Unused break information.  */
         &ignored ) == BT_CS_GN_SUCCESS )
  {
    bt_sio_write_far_modem_status( i, far_modem_status );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_update_sdp_registration

DESCRIPTION
  Updates the SDP registration for a specified BT SIO table entry.

===========================================================================*/
LOCAL void bt_sio_update_sdp_registration
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_sd_service_discoverable_mode_type  sd_mode;

  if ( bt_sio_table[ i ].service_uuid != BT_SD_INVALID_UUID )
  {
    if ( bt_sio_table[ i ].enabled != FALSE )
    {
      if ( bt_sio_table[ i ].sd_registered == FALSE )
      {
        bt_sio_table[ i ].sd_registered = TRUE;

        BT_MSG_API( "BT SIO CMD TX: SD Reg UUID %x SCN %x AID %x",
                    bt_sio_table[ i ].service_uuid,
                    bt_sio_table[ i ].rc_server_channel,
                    bt_sio_table[ i ].bt_app_id_port );
        bt_cmd_sd_register_serv_ext(
          bt_sio_table[ i ].bt_app_id_port,
          bt_sio_table[ i ].service_uuid,
          bt_sio_table[ i ].service_version,
          bt_sio_table[ i ].rc_server_channel,
          BT_L2_PSM_INVALID,
          FALSE,  /*  Do not page scan for this SD registration.  */
          bt_sio_table[ i ].service_name_str );
      }
    }
    else
    {
      if ( bt_sio_table[ i ].sd_registered != FALSE )
      {
        bt_sio_table[ i ].sd_registered = FALSE;

        BT_MSG_API( "BT SIO CMD TX: SD Unreg UUID %x AID %x",
                    bt_sio_table[ i ].service_uuid,
                    bt_sio_table[ i ].bt_app_id_port, 0 );
        bt_cmd_sd_unregister_srv_by_scn(
          bt_sio_table[ i ].bt_app_id_port,
          bt_sio_table[ i ].service_uuid,
          bt_sio_table[ i ].rc_server_channel );
      }
    }

    if ( (bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_NA) &&
         (bt_sio_table[ i ].bt_app_id_user != BT_APP_ID_NULL) )
    {
      if ( bt_sio_table[ i ].enabled != FALSE )
      {
        sd_mode = BT_SD_SERVICE_DISCOVERABLE_MODE_GENERAL;
      }
      else
      {
        sd_mode = BT_SD_SERVICE_DISCOVERABLE_MODE_NONE;
      }

      BT_MSG_API( "BT SIO CMD TX: SD Set DM %x AID %x",
                  sd_mode, bt_sio_table[ i ].bt_app_id_user, 0 );
      bt_cmd_sd_set_service_discoverable_mode(
        bt_sio_table[ i ].bt_app_id_user, sd_mode );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_sio_rc_is_disconnected

DESCRIPTION
  Updates a specified BT SIO table entry after RFCOMM has
  been disconnected for that port.

===========================================================================*/
LOCAL void bt_sio_rc_is_disconnected
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  void (*temp_close_func_ptr)( void );
  bt_cmd_status_type  status;
  bt_bd_addr_type*    bd_addr_ptr = NULL;

#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
  if ( bt_sio_table[i].sio_port_id == SIO_PORT_BT_NA )
  {
    if ( ( bt_sio_table[i].stream_mode == SIO_DS_RAWDATA_MODE ) ||
         ( bt_sio_table[i].stream_mode == SIO_DS_PKT_MODE ) )
    {
      rex_clr_timer ( &bt_sio_na_info.esc_timer );
      bt_sio_na_info.esc_state = BT_SIO_ESC_SEQ_ENABLE;
    }
  }
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

  if ( (bt_sio_table[ i ].client_app == FALSE) &&
       (bt_sio_table[ i ].enabled == FALSE) )
  {
    /*  Server going off-line.  Unregister  */
    /*  with SDP & RFCOMM as necessary.     */

    bt_sio_update_sdp_registration( i );

    if ( bt_sio_table[ i ].rc_registered == BT_RC_REG_REGISTERED ||
         bt_sio_table[ i ].rc_registered == BT_RC_REG_REGISTERING )
    {
      bt_sio_table[ i ].rc_registered = BT_RC_REG_UNREGISTERING;

      BT_MSG_API( "BT SIO CMD TX: RC Unreg SCN %x AID %x",
                  bt_sio_table[ i ].rc_server_channel,
                  bt_sio_table[ i ].bt_app_id_port, 0 );
      bt_cmd_rc_unregister( bt_sio_table[ i ].bt_app_id_port,
                            bt_sio_table[ i ].rc_server_channel );
    }
  }

  /*  Have RM unpair UUID/SCN as necessary.  */
  if ( (bt_sio_table[ i ].uuid_scn_paired_uuid != BT_SD_INVALID_UUID) &&
       ((bt_sio_table[ i ].client_app != FALSE) ||
        (bt_sio_table[ i ].open == FALSE) ||
        (bt_sio_table[ i ].enabled == FALSE)) )

  {
    if ( bt_sio_table[ i ].client_app != FALSE )
    {
      bd_addr_ptr = &bt_sio_table[ i ].bd_addr;
    }

    BT_MSG_API( "BT SIO CMD TX: RM Unpaired UUID %x C/SCN %04x AID %x",
                bt_sio_table[ i ].service_uuid,
                (uint16)((bt_sio_table[ i ].client_app << 8) |
                            bt_sio_table[ i ].rc_server_channel),
                bt_sio_table[ i ].bt_app_id_port );
    BT_BDA( MSG_API, "BT SIO CMD TX: RM Unpaired", bd_addr_ptr );
    if ( (status =
            bt_cmd_rm_uuid_scn_unpaired(
              bt_sio_table[ i ].uuid_scn_paired_uuid,
              bt_sio_table[ i ].rc_server_channel,
              bd_addr_ptr )) != BT_CS_GN_SUCCESS )
    {
      BT_ERR( "BT SIO: Bad RM Unpaired St %x UUID %x C/SCN %04x",
              status, bt_sio_table[ i ].uuid_scn_paired_uuid,
              (uint16)((bt_sio_table[ i ].client_app << 8) |
                         bt_sio_table[ i ].rc_server_channel) );
      BT_BDA( ERR, "BT SIO: Bad RM Unpaired", bd_addr_ptr );
    }

    bt_sio_table[ i ].uuid_scn_paired_uuid = BT_SD_INVALID_UUID;
  }

  /*  Update SPP state as necessary.  */
  if ( bt_sio_table[ i ].open == FALSE )
  {
    if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_DISCONNECTING) &&
         (bt_sio_table[ i ].client_app != FALSE) )
    {
      bt_sio_set_spp_state( i, BT_SPP_ST_DISCONNECTED );
    }
    if ( bt_sio_table[ i ].rc_registered == BT_RC_REG_UNREGISTERED )
    {
      bt_sio_set_spp_state( i, BT_SPP_ST_CLOSED );
    }
  }
  else
  {
    if ( bt_sio_table[ i ].client_app == FALSE )
    {
      bt_sio_set_spp_state( i, BT_SPP_ST_OPEN );
    }
    else
    {
      bt_sio_set_spp_state( i, BT_SPP_ST_DISCONNECTED );
    }
  }

  if ( (bt_sio_table[ i ].open == FALSE) &&
       (bt_sio_table[ i ].enabled == FALSE) &&
       (bt_sio_table[ i ].spp_state == BT_SPP_ST_CLOSED) )
  {
    temp_close_func_ptr = bt_sio_table[ i ].close_func_ptr;

    BT_MSG_API( "BT SIO CMD TX: EC Free AppID %x",
                bt_sio_table[ i ].bt_app_id_port, 0, 0 );

    bt_cmd_ec_free_application_id(
      bt_sio_table[ i ].bt_app_id_port );

    bt_sio_initialize_entry( i );

    if ( temp_close_func_ptr != NULL )
    {
      (temp_close_func_ptr());
    }
  }

}


/*===========================================================================

FUNCTION
  bt_sio_disconnect_rc

DESCRIPTION
  Disconnects RFCOMM for a specified BT SIO table entry as necessary.

===========================================================================*/
LOCAL void bt_sio_disconnect_rc
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_CONNECTED) &&
       (bt_sio_table[ i ].rc_pid != BT_RC_NULL_PID) )
  {
    BT_MSG_API( "BT SIO CMD TX: RC Disc AID %x PID %x",
                bt_sio_table[ i ].bt_app_id_port,
                bt_sio_table[ i ].rc_pid, 0 );

    bt_cmd_rc_disconnect(
      bt_sio_table[ i ].bt_app_id_port,
      bt_sio_table[ i ].rc_pid );

    bt_sio_set_spp_state( i, BT_SPP_ST_DISCONNECTING );
  }
  else if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) &&
            (bt_sio_table[ i ].client_app != FALSE ) )
  {
    bt_sio_set_spp_state( i, BT_SPP_ST_DISCONNECTING );
    if ( bt_sio_table[ i ].rc_server_channel != BT_SPP_SCN_UNSPECIFIED )
    {
      BT_MSG_API( "BT SIO CMD TX: RC Cancel Connect AID %x SCN %x",
                  bt_sio_table[ i ].bt_app_id_port,
                  bt_sio_table[ i ].rc_server_channel, 0 );
      BT_BDA( MSG_API, "BT SIO CMD TX: RC Cancel Connect",
              &bt_sio_table[ i ].bd_addr );
      bt_cmd_rc_cancel_connect( bt_sio_table[ i ].bt_app_id_port,
                                bt_sio_table[ i ].rc_server_channel,
                                &bt_sio_table[ i ].bd_addr );
    }
  }
  else if( (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) &&
            (bt_sio_table[ i ].client_app == FALSE ) )
  {
      BT_MSG_DEBUG( "BT SIO: Cancel SRV register AID %x SCN %x",
                    bt_sio_table[ i ].bt_app_id_port,
                    bt_sio_table[ i ].rc_server_channel, 0 );
      bt_sio_rc_is_disconnected( i );
  }
  else if( bt_sio_table[ i ].spp_state != BT_SPP_ST_DISCONNECTING )
  {
    bt_sio_rc_is_disconnected( i );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_deassert_far_rtc

DESCRIPTION
  Deasserts far RTC for a specified SIO table entry.

===========================================================================*/
LOCAL void bt_sio_deassert_far_rtc
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_spp_ms_enum_type  far_modem_status;

  far_modem_status =  bt_sio_table[ i ].far_modem_status;
  far_modem_status &= ~BT_RC_MS_RTC_ASSERTED_B;

  bt_sio_write_far_modem_status( i, far_modem_status );

}

/*===========================================================================

FUNCTION
  bt_sio_ev_send_na_enabled

DESCRIPTION
  Sends a NA Enabled event.

===========================================================================*/
LOCAL void bt_sio_ev_send_na_enabled
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_ev_msg_type  ev_msg;

  BT_MSG_API( "BT NA EV TX: NA Enabled AID %x",
              bt_sio_table[ i ].bt_app_id_user, 0, 0 );
  ev_msg.ev_hdr.ev_type   = BT_EV_NA_ENABLED;
  ev_msg.ev_hdr.bt_app_id = bt_sio_table[ i ].bt_app_id_user;
  bt_ec_send_event( &ev_msg );

}

/*===========================================================================

FUNCTION
  bt_sio_ev_send_na_disabled

DESCRIPTION
  Sends a NA Disabled event.

===========================================================================*/
LOCAL void bt_sio_ev_send_na_disabled
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_ev_msg_type  ev_msg;

  BT_MSG_API( "BT NA EV TX: NA Disabled AID %x",
              bt_sio_table[ i ].bt_app_id_user, 0, 0 );
  ev_msg.ev_hdr.ev_type   = BT_EV_NA_DISABLED;
  ev_msg.ev_hdr.bt_app_id = bt_sio_table[ i ].bt_app_id_user;
  bt_ec_send_event( &ev_msg );

}


/*===========================================================================

FUNCTION
  bt_sio_disable_na

DESCRIPTION
  Disables NA and sends event as necessary.

===========================================================================*/
LOCAL void bt_sio_disable_na
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  if ( bt_sio_table[ i ].enabled != FALSE )
  {
    bt_sio_table[ i ].enabled = FALSE;

    bt_sio_disconnect_rc( i );
  }

}

/*===========================================================================

FUNCTION
  bt_na_process_command

DESCRIPTION
  Processes all commands intended for NA.

===========================================================================*/
void bt_na_process_command
(
  bt_cmd_msg_type*  na_cmd_ptr
)
{

  uint8           i;

  na_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_sio_port_id(
              SIO_PORT_BT_NA )) != BT_SIO_NULL_IDX )
  {
    switch ( na_cmd_ptr->cmd_hdr.cmd_type )
    {
      case BT_CMD_NA_ENABLE:
        {
          BT_MSG_API( "BT NA CMD RX: Enable AID %x",
                      na_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

          if ( bt_sio_table[ i ].enabled == FALSE )
          {
            bt_sio_table[ i ].enabled = TRUE;

            bt_sio_table[ i ].bt_app_id_user =
              na_cmd_ptr->cmd_hdr.bt_app_id;

            bt_sio_update_sdp_registration( i );

            bt_sio_set_spp_state( i, BT_SPP_ST_OPENING );

            /* Enabled event will be sent to the app after registering with RFCOMM */
          }
          else
          {
            na_cmd_ptr->cmd_hdr.cmd_status = BT_CS_NA_ALREADY_ENABLED;
          }
          break;
        }

      case BT_CMD_NA_DISABLE:
        {
          BT_MSG_API( "BT NA CMD RX: Disable AID %x",
                      na_cmd_ptr->cmd_hdr.bt_app_id, 0, 0 );

          if ( bt_sio_table[ i ].bt_app_id_user ==
                 na_cmd_ptr->cmd_hdr.bt_app_id )
          {
            if ( bt_sio_table[ i ].enabled == FALSE )
            {
              BT_MSG_DEBUG( "BT NA: Already Disabled %x I %x AID %x",
                            bt_sio_table[ i ].enabled, i,
                            bt_sio_table[ i ].bt_app_id_user );
            }

            /* Disabled event will be sent to the app after RFCOMM de-reg */
            bt_sio_disable_na( i );
          }
          else
          {
            na_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_BAD_APP_ID;

            BT_MSG_DEBUG( "BT NA: Bad AID %x Exp %x",
                          na_cmd_ptr->cmd_hdr.bt_app_id,
                          bt_sio_table[ i ].bt_app_id_user, 0 );
          }
          break;
        }

      default:
        {
          na_cmd_ptr->cmd_hdr.cmd_status = BT_CS_NA_UNRECOGNIZED_CMD;
          BT_ERR( "BT NA: Bad RX CMD Type %x AID %x",
                  na_cmd_ptr->cmd_hdr.cmd_type,
                  na_cmd_ptr->cmd_hdr.bt_app_id, 0 );
          break;
        }
    }
  }
  else
  {
    na_cmd_ptr->cmd_hdr.cmd_status = BT_CS_NA_NO_SIO_PORT;

    BT_ERR( "BT NA: No SIO Port %x %x", SIO_PORT_BT_NA, i, 0 );
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );

}


/*===========================================================================

FUNCTION
  bt_sio_open_error

DESCRIPTION
  This function deals with an error while opening a port.

===========================================================================*/
LOCAL void bt_sio_open_error
(
  uint8                 i,
  bt_event_reason_type  reason,
  boolean               force_rc_is_discd

)
{

  MSG_FATAL( "BT SIO: Open Error I %x R %x", i, reason, 0 );

  bt_sio_table[ i ].spp_reason = reason;

  bt_sio_set_spp_state( i, BT_SPP_ST_OPEN_ERROR );

  bt_sio_table[ i ].spp_reason = BT_EVR_GN_SUCCESS;

  if ( force_rc_is_discd != FALSE )
  {
    bt_sio_rc_is_disconnected( i );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_rm_pair_uuid_scn_bda

DESCRIPTION
  Informs the RM that the specified UUID / Server Channel Number (SCN) /
  should be paired for the specified BD address.

===========================================================================*/
LOCAL bt_cmd_status_type bt_sio_rm_pair_uuid_scn_bda
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_cmd_status_type  status;
  bt_bd_addr_type*    bd_addr_ptr = NULL;
          

  if ( bt_sio_table[ i ].client_app != FALSE )
  {
    bd_addr_ptr = &bt_sio_table[ i ].bd_addr;
  }

  BT_MSG_API( "BT SIO CMD TX: RM Paired UUID %x C/SCN %04x AID %x",
              bt_sio_table[ i ].service_uuid,
              (uint16)((bt_sio_table[ i ].client_app << 8) |
                          bt_sio_table[ i ].rc_server_channel),
              bt_sio_table[ i ].bt_app_id_port );
  BT_BDA( MSG_API, "BT SIO CMD TX: RM Paired", bd_addr_ptr );
  if ( (status =
          bt_cmd_rm_uuid_scn_paired(
            bt_sio_table[ i ].service_uuid,
            bt_sio_table[ i ].rc_server_channel,
            bd_addr_ptr )) == BT_CS_GN_SUCCESS )
  {
    bt_sio_table[ i ].uuid_scn_paired_uuid =
      bt_sio_table[ i ].service_uuid;
  }
  else
  {
    BT_ERR( "BT SIO: Bad RM Paired St %x UUID %x C/SCN %04x",
            status, bt_sio_table[ i ].service_uuid,
            (uint16)((bt_sio_table[ i ].client_app << 8) |
                       bt_sio_table[ i ].rc_server_channel) );
    BT_BDA( ERR, "BT SIO: Bad RM Paired", bd_addr_ptr );
  }

  return( status );

}


/*===========================================================================

FUNCTION
  bt_sio_sdp_registration_done

DESCRIPTION
  Handles post SDP Registration processing.

===========================================================================*/
LOCAL void bt_sio_sdp_registration_done
(
  bt_app_id_type      bt_app_id,
  bt_cmd_status_type  cmd_status
)
{

  uint8               i;

  if ( (i = bt_sio_find_entry_app_id_port( bt_app_id )) !=
              BT_SIO_NULL_IDX )
  {
    if ( (bt_sio_table[ i ].client_app == FALSE) &&
         (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) )
    {
      if ( cmd_status == BT_CS_GN_SUCCESS )
      {
        if ( bt_sio_table[ i ].service_uuid != BT_SD_INVALID_UUID )
        {
          bt_sio_rm_pair_uuid_scn_bda( i );
        }

        bt_sio_table[ i ].rc_registered = BT_RC_REG_REGISTERING;

        BT_MSG_API( "BT SIO CMD TX: RC Reg SCN %x AID %x",
                    bt_sio_table[ i ].rc_server_channel,
                    bt_sio_table[ i ].bt_app_id_port, 0 );
        bt_cmd_rc_register( bt_sio_table[ i ].bt_app_id_port,
                            bt_sio_table[ i ].rc_server_channel,
                            bt_sio_table[ i ].tx_wm_ptr,
                            bt_sio_table[ i ].rx_wm_ptr );
      }
      else
      {
        bt_sio_open_error( i, BT_EVR_SPP_SDP_REGISTRATION_FAILED, TRUE );
      }
    }
    else
    {
      BT_ERR( "BT SIO: Unexp SDP RegD St %x I %x",
              bt_sio_table[ i ].spp_state, i, 0 );
    }
  }
  else
  {
    BT_ERR( "BT SIO: Bad SDP RegD AID %x",
            bt_app_id, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_rc_registration_done

DESCRIPTION
  Handles a Command Done event for an RFCOMM Register command.

===========================================================================*/
LOCAL void bt_sio_rc_registration_done
(
  bt_ev_msg_type*  cd_rcr_ptr
)
{

  uint8  i;

  if ( (i = bt_sio_find_entry_app_id_port(
              cd_rcr_ptr->ev_hdr.bt_app_id )) != BT_SIO_NULL_IDX )
  {
    if ( (bt_sio_table[ i ].client_app == FALSE) &&
         (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) )
    {
      if ( cd_rcr_ptr->ev_msg.ev_gn_cmd_done.cmd_status ==
             BT_CS_GN_SUCCESS )
      {
        if ( (bt_sio_table[ i ].rc_registered == BT_RC_REG_REGISTERING) ||
             (bt_sio_table[ i ].rc_registered != BT_RC_REG_UNREGISTERING) )
        {
          if ( bt_sio_table[ i ].rc_registered == BT_RC_REG_REGISTERING )
          {
            bt_sio_table[ i ].rc_registered = BT_RC_REG_REGISTERED;
          }
          
          if ( bt_sio_table[ i ].open != FALSE )
          {
            bt_sio_set_spp_state( i, BT_SPP_ST_OPEN );
          }

          if ( bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_NA ) 
          {
            bt_sio_ev_send_na_enabled( i );
          }
        }
        else
        {
          BT_ERR( "BTSIO: RC Reg CmdDn Bad RC reg St %x idx %x",
                  bt_sio_table[ i ].rc_registered, i, 0);
          bt_sio_open_error( i,
                             BT_EVR_SPP_RFCOMM_REGISTRATION_FAILED,
                             FALSE );
        }
      }
      else
      {
        bt_sio_open_error( i, BT_EVR_SPP_RFCOMM_REGISTRATION_FAILED, TRUE );
      }
    }
    else
    {
      BT_ERR( "BT SIO: Unexp RC RegD St %x I %x",
              bt_sio_table[ i ].spp_state, i, 0 );
    }
  }
  else
  {
    BT_ERR( "BT SIO: Bad RC RegD AID %x",
            cd_rcr_ptr->ev_hdr.bt_app_id, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_open_spp

DESCRIPTION
  This function handles the specifics of opening an SPP port.

===========================================================================*/
LOCAL void bt_sio_open_spp
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_sio_set_spp_state( i, BT_SPP_ST_OPENING );

  if ( bt_sio_table[ i ].client_app == FALSE )
  {
    if ( bt_sio_table[ i ].service_uuid != BT_SD_INVALID_UUID )
    {
      bt_sio_update_sdp_registration( i );
    }
    else
    {
      bt_sio_sdp_registration_done( bt_sio_table[ i ].bt_app_id_port,
                                    BT_CS_GN_SUCCESS );
    }
  }
  else  /*  Client application.  */
  {
    if ( bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED )
    {
      if ( bt_sio_table[ i ].service_uuid != BT_SD_INVALID_UUID )
      {
        BT_MSG_API( "BT SIO CMD TX: SD Get SCN UUID %x AID %x",
                    bt_sio_table[ i ].service_uuid,
                    bt_sio_table[ i ].bt_app_id_port, 0 );
        BT_BDA( MSG_API, "BT SIO CMD TX: SD Get SCN",
                &bt_sio_table[ i ].bd_addr );
        bt_cmd_sd_get_server_channel_number(
          bt_sio_table[ i ].bt_app_id_port,
          &bt_sio_table[ i ].bd_addr,
          bt_sio_table[ i ].service_uuid );
      }
      else
      {
        bt_sio_open_error( i, BT_EVR_SPP_BAD_OPEN_PARAMS, FALSE );

        BT_ERR( "BT SIO: No UUID or SCN I %x SID %x",
                i, bt_sio_table[ i ].stream_id, 0 );
      }
    }
    else
    {
      if ( bt_sio_table[ i ].service_uuid != BT_SD_INVALID_UUID )
      {
        bt_sio_rm_pair_uuid_scn_bda( i );
      }

      BT_MSG_API( "BT SIO CMD TX: RC ConnCC SCN %x MFS %x AID %x",
                  bt_sio_table[ i ].rc_server_channel,
                  bt_sio_table[ i ].rc_config.max_frame_size,
                  bt_sio_table[ i ].bt_app_id_port );
      BT_BDA( MSG_API, "BT SIO CMD TX: RC ConnCC",
              &bt_sio_table[ i ].bd_addr );
      bt_cmd_rc_connect_custom_cfg(
        bt_sio_table[ i ].bt_app_id_port,
        bt_sio_table[ i ].rc_server_channel,
        &bt_sio_table[ i ].bd_addr,
        bt_sio_table[ i ].tx_wm_ptr,
        bt_sio_table[ i ].rx_wm_ptr,
        bt_sio_table[ i ].rc_config.pn_negot_done,
        bt_sio_table[ i ].rc_config.rpn_negot_done,
        bt_sio_table[ i ].rc_config.max_frame_size,
        bt_sio_table[ i ].rc_config.priority,
        bt_sio_table[ i ].rc_config.cb_flow_ctrl,
        bt_sio_table[ i ].rc_config.baudrate,
        bt_sio_table[ i ].rc_config.format,
        bt_sio_table[ i ].rc_config.flow_ctrl,
        bt_sio_table[ i ].rc_config.xon_char,
        bt_sio_table[ i ].rc_config.xoff_char );
    }
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_sd_scn_response

DESCRIPTION
  Handles SDP Server Channel Number Response events for the BT SIO
  module.

===========================================================================*/
LOCAL void bt_sio_ev_sd_scn_response
(
  bt_ev_msg_type*  sd_scn_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: SD SCN Resp UUID %x SCN %x",
              sd_scn_ptr->ev_msg.ev_sd_scn_resp.service_class,
              sd_scn_ptr->ev_msg.ev_sd_scn_resp.scn, 0 );
  BT_BDA( MSG_API, "BT SIO EV RX: SD SCN Resp",
          &sd_scn_ptr->ev_msg.ev_sd_scn_resp.bd_addr );

  if ( (i = bt_sio_find_entry_app_id_port(
              sd_scn_ptr->ev_hdr.bt_app_id )) != BT_SIO_NULL_IDX )
  {
    if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) &&
         (bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
    {
      if ( (BT_BD_ADDRS_EQUAL(
              &bt_sio_table[ i ].bd_addr,
              &sd_scn_ptr->ev_msg.ev_sd_scn_resp.bd_addr)) &&
           (bt_sio_table[ i ].service_uuid ==
              sd_scn_ptr->ev_msg.ev_sd_scn_resp.service_class) )
      {
        if ( bt_sio_table[ i ].open != FALSE )
        {
          bt_sio_table[ i ].rc_server_channel =
            sd_scn_ptr->ev_msg.ev_sd_scn_resp.scn;

          bt_sio_rm_pair_uuid_scn_bda( i );
          
          bt_sio_table[ i ].service_uuid = BT_SD_INVALID_UUID;

          bt_sio_open_spp( i );
        }
        else
        {
          /*  Close was called before it was opened.  Manually  */
          /*  set state to Close without sending status update. */
          /*  Application will get status update when state     */
          /*  gets to Close at end of closing process.          */

          bt_sio_table[ i ].spp_state = BT_SPP_ST_CLOSED;

          bt_sio_disconnect_rc( i );
        }
      }
      else
      {
        BT_ERR( "BT SIO: Bad SD SCN Resp I %x UUID %x %x",
                i, bt_sio_table[ i ].service_uuid,
                sd_scn_ptr->ev_msg.ev_sd_scn_resp.service_class );
      }
    }
    else if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_DISCONNECTING) &&
              (bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
    {
      bt_sio_rc_is_disconnected( i );
    }
    else
    {
      BT_ERR( "BT SIO: Unexp SD SCN Resp St %x SCN %x I %x",
              bt_sio_table[ i ].spp_state,
              bt_sio_table[ i ].rc_server_channel, i );
    }
  }
  else
  {
    BT_ERR( "BT SIO: SD SCN Bad AID %x SCN %x UUID %x",
            sd_scn_ptr->ev_hdr.bt_app_id,
            sd_scn_ptr->ev_msg.ev_sd_scn_resp.scn,
            sd_scn_ptr->ev_msg.ev_sd_scn_resp.service_class );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_sd_timeout_response

DESCRIPTION
  Handles SDP Timeout events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_sd_timeout_response
(
  bt_ev_msg_type*  sd_to_ptr
)
{

  uint8  i;

  BT_BDA( MSG_API, "BT SIO EV RX: SD TO Resp",
          &sd_to_ptr->ev_msg.ev_sd_timeout_resp.bd_addr );

  if ( (i = bt_sio_find_entry_app_id_port(
              sd_to_ptr->ev_hdr.bt_app_id )) != BT_SIO_NULL_IDX )
  {
    if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) &&
         (bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
    {
      if ( BT_BD_ADDRS_EQUAL(
             &bt_sio_table[ i ].bd_addr,
             &sd_to_ptr->ev_msg.ev_sd_timeout_resp.bd_addr ) )
      {
        bt_sio_open_error( i, BT_EVR_SPP_SDP_TIMEOUT, TRUE );
      }
      else
      {
        BT_ERR( "BT SIO: Bad SD TO Resp I %x AID %x",
                i, bt_sio_table[ i ].bt_app_id_port, 0 );
        BT_BDA( ERR, "BT SIO: Bad SD TO Resp Evt",
                &sd_to_ptr->ev_msg.ev_sd_timeout_resp.bd_addr );
        BT_BDA( ERR, "BT SIO: Bad SD TO Resp Tbl",
                &bt_sio_table[ i ].bd_addr );
      }
    }
    else if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_DISCONNECTING) &&
              (bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
    {
      bt_sio_rc_is_disconnected( i );
    }
    else
    {
      BT_ERR( "BT SIO: Unexp SD TO Resp St %x SCN %x I %x",
              bt_sio_table[ i ].spp_state,
              bt_sio_table[ i ].rc_server_channel, i );
    }
  }
  else
  {
    BT_ERR( "BT SIO: Bad SD TO Resp AID %x",
            sd_to_ptr->ev_hdr.bt_app_id, 0, 0 );
    BT_BDA( ERR, "BT SIO: Bad SD TO Resp",
            &sd_to_ptr->ev_msg.ev_sd_timeout_resp.bd_addr );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_sd_error_response

DESCRIPTION
  Handles SDP Error Response events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_sd_error_response
(
  bt_ev_msg_type*  sd_err_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: SD Err Resp EC %x AID %x",
              sd_err_ptr->ev_msg.ev_sd_error_resp.error_code,
              sd_err_ptr->ev_hdr.bt_app_id, 0 );
  BT_BDA( MSG_API, "BT SIO EV RX: SD Err Resp",
          &sd_err_ptr->ev_msg.ev_sd_error_resp.bd_addr );

  if ( (i = bt_sio_find_entry_app_id_port(
              sd_err_ptr->ev_hdr.bt_app_id )) != BT_SIO_NULL_IDX )
  {
    if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_OPENING) &&
         (bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
    {
      if ( BT_BD_ADDRS_EQUAL(
             &bt_sio_table[ i ].bd_addr,
             &sd_err_ptr->ev_msg.ev_sd_error_resp.bd_addr ) )
      {
        bt_sio_open_error(
          i,
          (bt_event_reason_type)
           (sd_err_ptr->ev_msg.ev_sd_error_resp.error_code),
          TRUE );
      }
      else
      {
        BT_ERR( "BT SIO: Bad SD Err Resp I %x AID %x",
                i, bt_sio_table[ i ].bt_app_id_port, 0 );
        BT_BDA( ERR, "BT SIO: Bad SD Err Resp Evt",
                &sd_err_ptr->ev_msg.ev_sd_error_resp.bd_addr );
        BT_BDA( ERR, "BT SIO: Bad SD Err Resp Tbl",
                &bt_sio_table[ i ].bd_addr );
      }
    }
    else if ( (bt_sio_table[ i ].spp_state == BT_SPP_ST_DISCONNECTING) &&
              (bt_sio_table[ i ].rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
    {
      bt_sio_rc_is_disconnected( i );
    }
    else
    {
      BT_ERR( "BT SIO: Unexp SD Err Resp St %x SCN %x I %x",
              bt_sio_table[ i ].spp_state,
              bt_sio_table[ i ].rc_server_channel, i );
    }
  }
  else
  {
    BT_ERR( "BT SIO: SD Err Resp Bad AID %x EC %x",
            sd_err_ptr->ev_hdr.bt_app_id,
            sd_err_ptr->ev_msg.ev_sd_error_resp.error_code, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_rc_connected

DESCRIPTION
  Handles RC Connected events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_connected
(
  bt_ev_msg_type*  rc_conn_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: RC Conn AID %x PID %x",
              rc_conn_ptr->ev_hdr.bt_app_id,
              rc_conn_ptr->ev_msg.ev_rc_conn.pid, 0 );

  if ( (i = bt_sio_find_entry_app_id_port(
              rc_conn_ptr->ev_hdr.bt_app_id )) != BT_SIO_NULL_IDX )
  {
    bt_sio_table[ i ].rc_pid    = rc_conn_ptr->ev_msg.ev_rc_conn.pid;
    bt_sio_table[ i ].bd_addr   = rc_conn_ptr->ev_msg.ev_rc_conn.bd_addr;
    bt_sio_table[ i ].rc_config = rc_conn_ptr->ev_msg.ev_rc_conn.config;

    if ( bt_sio_table[ i ].open != FALSE )
    {
      bt_sio_rc_port_initialize( i );
      bt_sio_set_spp_state( i, BT_SPP_ST_CONNECTED );
      bt_sio_config_updated( i );
    }
    else if ( bt_sio_table[ i ].client_app != FALSE )
    {
      /*  Close was called before it was connected.  Manually    */
      /*  set state to Connected without sending status update.  */
      /*  Application will get status update when state          */
      /*  gets to Close at end of closing process.               */
      /*  For servers, RC has been unregistered by now, so it    */
      /*  is unnecessary to send the disconnect command.         */

      bt_sio_table[ i ].spp_state = BT_SPP_ST_CONNECTED;
      bt_sio_disconnect_rc( i );
    }
  }
  else
  {
    BT_ERR( "BT SIO: RC Conn Bad AID %x PID %x",
            rc_conn_ptr->ev_hdr.bt_app_id,
            rc_conn_ptr->ev_msg.ev_rc_conn.pid, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_rc_ms_changed

DESCRIPTION
  Handles RC Modem Status Changed events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_ms_changed
(
  bt_ev_msg_type*  rc_ms_chgd_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: RC MS Chgd %02x %02x PID %x",
              rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.ms_changed_mask,
              rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.modem_status,
              rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.pid );

  if ( (i = bt_sio_find_entry_rc_pid(
              rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.pid )) !=
       BT_SIO_NULL_IDX )
  {
    bt_sio_table[ i ].ms_changed_mask =
      rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.ms_changed_mask;
    bt_sio_table[ i ].break_present =
      rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.break_present;
    bt_sio_table[ i ].break_length =
      rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.break_length;
    bt_sio_table[ i ].breaks =
      rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.breaks;

    bt_sio_write_far_modem_status(
      i, rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.modem_status );
  }
  else
  {
    BT_ERR( "BT SIO: RC MS Chgd Bad PID %x AID %x",
            rc_ms_chgd_ptr->ev_msg.ev_rc_ms_chgd.pid,
            rc_ms_chgd_ptr->ev_hdr.bt_app_id, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_rc_reconfigured

DESCRIPTION
  Handles RC Reconfigured events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_reconfigured
(
  bt_ev_msg_type*  rc_rcfgd_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: RC Recfg MFS %x CBFC %x PID %x",
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.
                config.max_frame_size,
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.
                config.cb_flow_ctrl,
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.pid );

  BT_MSG_API( "BT SIO EV RX: RC Recfg BR %x Fmt %x FC %x",
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.
                config.baudrate,
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.
                config.format,
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.
                config.flow_ctrl );

  if ( (i = bt_sio_find_entry_rc_pid(
              rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.pid )) !=
       BT_SIO_NULL_IDX )
  {
    bt_sio_table[ i ].rc_config =
      rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.config;

    bt_sio_config_updated( i );
  }
  else
  {
    BT_ERR( "BT SIO: RC Recfg Bad PID %x AID %x",
            rc_rcfgd_ptr->ev_msg.ev_rc_reconfig.pid,
            rc_rcfgd_ptr->ev_hdr.bt_app_id, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_ev_rc_line_error

DESCRIPTION
  Handles RC Line Error events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_line_error
(
  bt_ev_msg_type*  rc_le_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: RC LE %x PID %x",
              rc_le_ptr->ev_msg.ev_rc_line_error.cur_line_error,
              rc_le_ptr->ev_msg.ev_rc_line_error.pid, 0 );

  if ( (i = bt_sio_find_entry_rc_pid(
              rc_le_ptr->ev_msg.ev_rc_line_error.pid )) !=
       BT_SIO_NULL_IDX )
  {
    bt_sio_table[ i ].cur_line_error =
      ( bt_spp_le_enum_type )rc_le_ptr->ev_msg.ev_rc_line_error.cur_line_error;
    bt_sio_table[ i ].overruns =
      rc_le_ptr->ev_msg.ev_rc_line_error.overruns;
    bt_sio_table[ i ].parity_errors =
      rc_le_ptr->ev_msg.ev_rc_line_error.parity_errors;
    bt_sio_table[ i ].framing_errors =
      rc_le_ptr->ev_msg.ev_rc_line_error.framing_errors;

    bt_sio_line_errors_updated( i );
  }
  else
  {
    BT_ERR( "BT SIO: RC LE Bad PID %x AID %x",
            rc_le_ptr->ev_msg.ev_rc_reconfig.pid,
            rc_le_ptr->ev_hdr.bt_app_id, 0 );
  }

}

/*===========================================================================

FUNCTION
  bt_sio_process_connection_failed

DESCRIPTION
  Handles RC Connection Failed events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_process_connection_failed
(
  bt_app_id_type        app_id,
  bt_event_reason_type  reason,
  bt_bd_addr_type*      bd_addr_ptr
)
{
  uint8  i;

  if ( (i = bt_sio_find_entry_app_id_port( app_id )) != BT_SIO_NULL_IDX )
  {
    bt_sio_table[ i ].rc_pid = BT_RC_NULL_PID;

    bt_sio_deassert_far_rtc( i );

    if ( bt_sio_table[ i ].client_app != FALSE )
    {
      bt_sio_open_error( i, reason, FALSE );
    }
    else
    {
      BT_MSG_DEBUG( "BT SIO: Server ConnF I %x R %x",
                    i, reason, 0 );

      /*  For servers a status update is generated indicating  */
      /*  an open error with the BD address and reason.        */

      if ( bd_addr_ptr ) 
      {
        bt_sio_table[ i ].bd_addr = *bd_addr_ptr;
      }

      bt_sio_open_error( i, reason, FALSE );

      memset( (byte*)(&bt_sio_table[ i ].bd_addr), 0,
              sizeof( bt_bd_addr_type ) );
    }

    bt_sio_rc_is_disconnected( i );
  }
  else
  {
    BT_ERR( "BT SIO: Bad RC ConnF AID %x R %x", app_id, reason, 0 );
    BT_BDA( ERR, "BT SIO: RC ConnF", bd_addr_ptr );
  }
}


/*===========================================================================

FUNCTION
  bt_sio_ev_rc_connection_failed

DESCRIPTION
  Handles RC Connection Failed events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_connection_failed
(
  bt_ev_msg_type*  rc_cf_ptr
)
{

  BT_MSG_API( "BT SIO EV RX: RC ConnF R %x AID %x",
              rc_cf_ptr->ev_msg.ev_rc_conn_failed.reason,
              rc_cf_ptr->ev_hdr.bt_app_id, 0 );

  bt_sio_process_connection_failed ( 
    rc_cf_ptr->ev_hdr.bt_app_id,
    rc_cf_ptr->ev_msg.ev_rc_conn_failed.reason,
    &rc_cf_ptr->ev_msg.ev_rc_conn_failed.bd_addr );

}


/*===========================================================================

FUNCTION
  bt_sio_ev_rc_disconnected

DESCRIPTION
  Handles RC Disconnected events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_disconnected
(
  bt_ev_msg_type*  rc_disc_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: RC Disc R %x PID %x AID %x",
              rc_disc_ptr->ev_msg.ev_rc_disc.reason,
              rc_disc_ptr->ev_msg.ev_rc_disc.pid,
              rc_disc_ptr->ev_hdr.bt_app_id );

  if ( (i = bt_sio_find_entry_rc_pid(
              rc_disc_ptr->ev_msg.ev_rc_disc.pid )) != BT_SIO_NULL_IDX )
  {
    bt_sio_table[ i ].rc_pid     = BT_RC_NULL_PID;
    bt_sio_table[ i ].spp_reason = rc_disc_ptr->ev_msg.ev_rc_disc.reason;

    bt_sio_deassert_far_rtc( i );

    bt_sio_rc_is_disconnected( i );
  }
  else
  {
    BT_ERR( "BT SIO: RC Disc Bad PID %x AID %x",
            rc_disc_ptr->ev_msg.ev_rc_disc.pid,
            rc_disc_ptr->ev_hdr.bt_app_id, 0 );
  }

}

/*===========================================================================

FUNCTION
  bt_sio_ev_rc_unregistered

DESCRIPTION
  Handles RC Unregistered events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_rc_unregistered
(
  bt_ev_msg_type*  rc_unreg_ptr
)
{

  uint8  i;

  BT_MSG_API( "BT SIO EV RX: RC Unreg SCN %x AID %x",
              rc_unreg_ptr->ev_msg.ev_rc_unregistered.server_channel,
              rc_unreg_ptr->ev_hdr.bt_app_id, 0 );

  if ( (i = bt_sio_find_entry_app_id_port(
              rc_unreg_ptr->ev_hdr.bt_app_id )) != BT_SIO_NULL_IDX )
  {
    if( bt_sio_table[ i ].rc_server_channel !=
        rc_unreg_ptr->ev_msg.ev_rc_unregistered.server_channel )
    {
      BT_ERR("BT SIO: RC Unreg Bad SCN %x expecting %x; unreg anyway",
             rc_unreg_ptr->ev_msg.ev_rc_unregistered.server_channel,
             bt_sio_table[ i ].rc_server_channel, 0);
    }
      
    bt_sio_table[ i ].rc_registered = BT_RC_REG_UNREGISTERED;
    bt_sio_rc_is_disconnected( i );

    if ( bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_NA ) 
    {
      /* send disabled event */
      bt_sio_ev_send_na_disabled( i );
    }
  }
  else
  {
    BT_ERR( "BT SIO: RC Unreg Bad AID %x",
            rc_unreg_ptr->ev_hdr.bt_app_id, 0, 0 );
  }
}


/*===========================================================================

FUNCTION
  bt_sio_ev_spp_tx_gne

DESCRIPTION
  Handles SPP TX Gone Non-Empty events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_spp_tx_gne
( 
  bt_ev_msg_type * bt_ev 
)
{
  uint8 i = 
    bt_sio_find_entry_stream_id( bt_ev->ev_msg.ev_spp_tx_gne.stream_id );

  if( i == BT_SIO_NULL_IDX )
  {
    return;
  }

  bt_cmd_rc_tx_data( bt_sio_table[ i ].bt_app_id_port,
                     bt_sio_table[ i ].rc_pid );
  
  /*  Call application's callback as necessary.  */
  if ( bt_sio_table[ i ].orig_tx_non_empty_func_ptr != NULL )
  {
    (bt_sio_table[ i ].orig_tx_non_empty_func_ptr)();
  }
}     


/*===========================================================================

FUNCTION
  bt_sio_ev_spp_tx_empty

DESCRIPTION
  Handles SPP TX Gone Empty events for the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_ev_spp_tx_empty
( 
  bt_ev_msg_type * bt_ev 
)
{
  uint8 i = 
    bt_sio_find_entry_stream_id( bt_ev->ev_msg.ev_spp_tx_gne.stream_id );

  if( i == BT_SIO_NULL_IDX )
  {
    return;
  }

  if ( bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr ==
         bt_sio_table[ i ].my_tx_gone_empty_func_ptr )
  {
    INTLOCK();
    bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr =
      bt_sio_table[ i ].orig_tx_gone_empty_func_ptr;
    INTFREE();
    bt_sio_table[ i ].orig_tx_gone_empty_func_ptr = NULL;
  }

  if ( bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr != NULL )
  {
    (bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr)();
  }

  if ( bt_sio_table[ i ].flush_tx_func_ptr != NULL )
  {
    (bt_sio_table[ i ].flush_tx_func_ptr)();
    bt_sio_table[ i ].flush_tx_func_ptr = NULL;
  }

}


/*===========================================================================

FUNCTION
  bt_sio_background_tick

DESCRIPTION
  Performs background monitoring for the BT SIO module.

===========================================================================*/
void bt_sio_background_tick
(
  void
)
{
  
  uint8  i;

  for ( i = 0; i < BT_MAX_SIO_PORTS; i++ )
  {
    if ( bt_sio_table[ i ].spp_state != BT_SPP_ST_CLOSED )
    {
      /*  Entry in use.  Monitor if it is not in steady state.  */
      /*  Steady state for client ports is connected.           */
      /*  Steady state for server ports are open or connected.  */

      if ( ((bt_sio_table[ i ].client_app != FALSE) &&
            (bt_sio_table[ i ].spp_state != BT_SPP_ST_CONNECTED)) ||
           ((bt_sio_table[ i ].client_app == FALSE) &&
            (bt_sio_table[ i ].spp_state != BT_SPP_ST_OPEN) &&
            (bt_sio_table[ i ].spp_state != BT_SPP_ST_CONNECTED)) )
      {
        if ( bt_sio_table[ i ].ss_timer_ms_left != 0 )
        {
          if ( bt_sio_table[ i ].ss_timer_ms_left <= BT_BACKGROUND_TIMER_MS )
          {
            /*  Steady state monitor has expired.  */

            BT_MSG_HIGH( "BT SIO: SST Exp St %x SID %x PID %x",
                         bt_sio_table[ i ].spp_state,
                         bt_sio_table[ i ].stream_id,
                         bt_sio_table[ i ].rc_pid );
						 
		   if ( bt_sio_table[ i ].client_app != FALSE )
		   {
		     bt_sio_open_error(i,
                               (bt_event_reason_type)
                               (BT_SD_EV_ERROR_CONNECTION_FAILED),
                               TRUE
                              );
		   }

            bt_sio_initialize_entry( i );
          }
          else
          {
            bt_sio_table[ i ].ss_timer_ms_left -= BT_BACKGROUND_TIMER_MS;
          }
        }
        else
        {
          if ( bt_sio_table[ i ].spp_state == BT_SPP_ST_OPEN_ERROR )
          {
            /*  Port had error while opening.  Monitor  */
            /*  that it is closed in a timely fashion.  */

            bt_sio_table[ i ].ss_timer_ms_left = BT_SIO_SS_ERROR_TO_MS;
          }
          else
          {
            /*  Port state is in transition.           */
            /*  Monitor that it reaches steady state.  */

            bt_sio_table[ i ].ss_timer_ms_left = BT_SIO_SS_TIMEOUT_MS;
          }
        }
      }
      else
      {
        /*  Steady state reached - disable timer.  */
        bt_sio_table[ i ].ss_timer_ms_left = 0;
      }
    }
  }

}


/*===========================================================================

FUNCTIONS
  bt_sio_tx_wm_non_empty_cb*

DESCRIPTION
  The following functions are the TX "Gone Non-Empty" call back functions
  for each of the ports supported by the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_tx_wm_non_empty_cb
(
  uint8  i  /*  bt_sio_table index.  */
)
{
  bt_ev_msg_type bt_ev;

  bt_ev.ev_hdr.ev_type = BT_EV_SPP_TX_GNE;
  bt_ev.ev_hdr.bt_app_id = bt_sio_app_id;
  bt_ev.ev_msg.ev_spp_flush_tx.stream_id = bt_sio_table[ i ].stream_id;
  bt_sio_store_bt_event( &bt_ev );
}

/*  There must be one bt_sio_tx_wm_non_empty_cb_*  */
/*  per BT_MAX_SIO_PORTS below and they must be    */
/*  used in bt_sio_powerup_init().                 */

LOCAL void bt_sio_tx_wm_non_empty_cb_0( void )
{
  bt_sio_tx_wm_non_empty_cb( 0 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_1( void )
{
  bt_sio_tx_wm_non_empty_cb( 1 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_2( void )
{
  bt_sio_tx_wm_non_empty_cb( 2 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_3( void )
{
  bt_sio_tx_wm_non_empty_cb( 3 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_4( void )
{
  bt_sio_tx_wm_non_empty_cb( 4 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_5( void )
{
  bt_sio_tx_wm_non_empty_cb( 5 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_6( void )
{
  bt_sio_tx_wm_non_empty_cb( 6 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_7( void )
{
  bt_sio_tx_wm_non_empty_cb( 7 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_8( void )
{
  bt_sio_tx_wm_non_empty_cb( 8 );
}

LOCAL void bt_sio_tx_wm_non_empty_cb_9( void )
{
  bt_sio_tx_wm_non_empty_cb( 9 );
}

#if ( BT_MAX_SIO_PORTS > 10 )
#error "Need more TX NE cbs for BT_MAX_SIO_PORTS > 10"
#endif


/*===========================================================================

FUNCTIONS
  bt_sio_tx_wm_gone_empty_cb*

DESCRIPTION
  The following functions are the TX "Gone Empty" call back functions
  for each of the ports supported by the BT SIO module.

===========================================================================*/
LOCAL void bt_sio_tx_wm_gone_empty_cb
(
  uint8  i  /*  bt_sio_table index.  */
)
{

  bt_ev_msg_type bt_ev;

  bt_ev.ev_hdr.ev_type = BT_EV_SPP_TX_EMPTY;
  bt_ev.ev_hdr.bt_app_id = bt_sio_app_id;
  bt_ev.ev_msg.ev_spp_flush_tx.stream_id = bt_sio_table[ i ].stream_id;
  bt_sio_store_bt_event( &bt_ev );

}

/*  There must be one bt_sio_tx_wm_gone_empty_cb_*  */
/*  per BT_MAX_SIO_PORTS below and they must be     */
/*  used in bt_sio_powerup_init().                  */

LOCAL void bt_sio_tx_wm_gone_empty_cb_0( void )
{
  bt_sio_tx_wm_gone_empty_cb( 0 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_1( void )
{
  bt_sio_tx_wm_gone_empty_cb( 1 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_2( void )
{
  bt_sio_tx_wm_gone_empty_cb( 2 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_3( void )
{
  bt_sio_tx_wm_gone_empty_cb( 3 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_4( void )
{
  bt_sio_tx_wm_gone_empty_cb( 4 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_5( void )
{
  bt_sio_tx_wm_gone_empty_cb( 5 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_6( void )
{
  bt_sio_tx_wm_gone_empty_cb( 6 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_7( void )
{
  bt_sio_tx_wm_gone_empty_cb( 7 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_8( void )
{
  bt_sio_tx_wm_gone_empty_cb( 8 );
}

LOCAL void bt_sio_tx_wm_gone_empty_cb_9( void )
{
  bt_sio_tx_wm_gone_empty_cb( 9 );
}

#if ( BT_MAX_SIO_PORTS > 10 )
#error "Need more TX GE cbs for BT_MAX_SIO_PORTS > 10"
#endif


/*===========================================================================

FUNCTION
  bt_sio_get_max_event_bytes

DESCRIPTION
  Returns the number of bytes in the largest event expected to be
  processed by the BT SIO module.

===========================================================================*/
LOCAL uint16 bt_sio_get_max_event_bytes
(
  void
)
{

  uint16  max_eb;

  max_eb = sizeof( bt_ev_gn_cmd_done_type );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_server_channel_number_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_timeout_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_sd_error_resp_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_connected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_connection_failed_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_ms_changed_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_reconfigured_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_line_error_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_disconnected_type ) );
  max_eb = MAX( max_eb, sizeof( bt_ev_rc_unregistered_type ) );

  max_eb += sizeof( bt_ev_hdr_type );

  return( max_eb );

}


/*===========================================================================

FUNCTION
  bt_sio_process_event

DESCRIPTION
  Handles all BT driver events coming to the BT SIO module.

===========================================================================*/
void bt_sio_process_event
(
  bt_ev_msg_type*  ev_msg_ptr
)
{

  rex_enter_crit_sect( &bt_sio_crit_sect );

  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      {
        if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type ==
               BT_CMD_SD_REGISTER_SERV_EXT )
        {
          BT_MSG_API( "BT SIO EV RX: SDP Reg Done CS %x AID %x",
                      ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
          bt_sio_sdp_registration_done(
            ev_msg_ptr->ev_hdr.bt_app_id,
            ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status );
        }
        else if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type ==
                    BT_CMD_RC_REGISTER )
        {
          BT_MSG_API( "BT SIO EV RX: RC Reg Done CS %x AID %x",
                      ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                      ev_msg_ptr->ev_hdr.bt_app_id, 0 );
          bt_sio_rc_registration_done( ev_msg_ptr );
        }
        else if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
                    BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT SIO: Bad CMD Done Stat %x Typ %x AID %x",
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                  ev_msg_ptr->ev_hdr.bt_app_id );

          if ( (ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type == 
                   BT_CMD_RC_CONN_CUST_CFG) ||
                (ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type == 
                   BT_CMD_RC_CONNECT) )
          {
            bt_sio_process_connection_failed ( 
              ev_msg_ptr->ev_hdr.bt_app_id,
              BT_EVR_SPP_BAD_OPEN_PARAMS,
              NULL );
          }
        }
        break;
      }

    case BT_EV_SD_SERVER_CHANNEL_NUMBER_RESP:
      {
        bt_sio_ev_sd_scn_response( ev_msg_ptr );
        break;
      }

    case BT_EV_SD_TIMEOUT_RESP:
      {
        bt_sio_ev_sd_timeout_response( ev_msg_ptr );
        break;
      }

    case BT_EV_SD_ERROR_RESP:
      {
        bt_sio_ev_sd_error_response( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_CONNECTED:
      {
        bt_sio_ev_rc_connected( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_CONNECTION_FAILED:
      {
        bt_sio_ev_rc_connection_failed( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_MS_CHANGED:
      {
        bt_sio_ev_rc_ms_changed( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_RECONFIGURED:
      {
        bt_sio_ev_rc_reconfigured( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_LINE_ERROR:
      {
        bt_sio_ev_rc_line_error( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_DISCONNECTED:
      {
        bt_sio_ev_rc_disconnected( ev_msg_ptr );
        break;
      }
    case BT_EV_RC_UNREGISTERED:
      {
        bt_sio_ev_rc_unregistered( ev_msg_ptr );
        break;
      }

    case BT_EV_SPP_TX_GNE:
      {
        bt_sio_ev_spp_tx_gne( ev_msg_ptr );
        break;
      }

    case BT_EV_SPP_TX_EMPTY:
      {
        bt_sio_ev_spp_tx_empty( ev_msg_ptr );
        break;
      }

    case BT_EV_RC_PING_RESULT:
    default:
      {
        BT_ERR( "BT SIO: Bad RX EV Type %x AID %x",
                ev_msg_ptr->ev_hdr.ev_type,
                ev_msg_ptr->ev_hdr.bt_app_id, 0 );
        break;
      }
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );

}


/*===========================================================================

FUNCTION
  bt_sio_process_ev_queue

DESCRIPTION
  Dequeues events on the BT SIO event queue and processes them.

===========================================================================*/
void bt_sio_process_ev_queue
(
  void
)
{

  bt_ev_msg_type*  bt_ev_ptr;
  int q_count = q_cnt( &bt_sio_ev_q );

  BT_VSTAT_MAX( q_count, SIO_MAX_EV_Q_DEPTH );
  
  BT_MSG_API_PYLD( "BTSIO ev Q - cur_cnt = %x, max_cnt = %x",
                   q_count,
                   bt_vital_stats[BT_VSAI_SIO_MAX_EV_Q_DEPTH], 0);

  while ( (bt_ev_ptr = (bt_ev_msg_type *)q_get( &bt_sio_ev_q )) != NULL )
  {
    bt_sio_process_event( bt_ev_ptr );
    q_put( &bt_event_free_q, &bt_ev_ptr->ev_hdr.link );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_powerup_init

DESCRIPTION
  Initializes the SIO module upon powerup.

===========================================================================*/
void bt_sio_powerup_init
(
  void
)
{

  uint8  i;

  for ( i = 0; i < BT_MAX_SIO_PORTS; i++ )
  {
    bt_sio_initialize_entry( i );
  }

  /*  Must be one bt_sio_tx_wm_non_empty_cb_* per  */
  /*  BT_MAX_SIO_PORTS referenced below.           */

  bt_sio_table[ 0 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_0;
  bt_sio_table[ 1 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_1;
  bt_sio_table[ 2 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_2;
  bt_sio_table[ 3 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_3;
  bt_sio_table[ 4 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_4;
  bt_sio_table[ 5 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_5;
  bt_sio_table[ 6 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_6;
  bt_sio_table[ 7 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_7;
  bt_sio_table[ 8 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_8;
  bt_sio_table[ 9 ].my_tx_non_empty_func_ptr = bt_sio_tx_wm_non_empty_cb_9;

  /*  Must be one bt_sio_tx_wm_gone_empty_cb_* per  */
  /*  BT_MAX_SIO_PORTS referenced below.            */

  bt_sio_table[ 0 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_0;
  bt_sio_table[ 1 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_1;
  bt_sio_table[ 2 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_2;
  bt_sio_table[ 3 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_3;
  bt_sio_table[ 4 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_4;
  bt_sio_table[ 5 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_5;
  bt_sio_table[ 6 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_6;
  bt_sio_table[ 7 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_7;
  bt_sio_table[ 8 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_8;
  bt_sio_table[ 9 ].my_tx_gone_empty_func_ptr = bt_sio_tx_wm_gone_empty_cb_9;

  (void) q_init( &bt_sio_ev_q );
  bt_sio_ev_q_info.event_q_ptr      = &bt_sio_ev_q;
  bt_sio_ev_q_info.max_q_depth      = BT_SIO_NUM_EVENTS;
  bt_sio_ev_q_info.event_q_bit_mask = BT_EVQ_SIO_B;
  bt_sio_ev_q_info.max_event_bytes  = bt_sio_get_max_event_bytes();

  bt_sio_app_id = bt_cmd_ec_get_app_id_and_register(
                    bt_sio_store_bt_event );
  ASSERT_DEBUG( bt_sio_app_id != BT_APP_ID_NULL );
  BT_MSG_API( "BT SIO TRX: EC Get AID %x", bt_sio_app_id, 0, 0 );
  BT_MSG_HIGH( "BT SIO AID is %x", bt_sio_app_id, 0, 0 );
  
  bt_sio_connectable = FALSE;

#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
  bt_sio_na_info.esc_guardtime_ptr = bt_sio_esc_guard_time_cb;
  rex_def_timer_ex( &bt_sio_na_info.esc_timer,
                    bt_sio_na_info.esc_guardtime_ptr,
                    BT_SIO_ESC_GUARD_TIME_MS );
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

  rex_init_crit_sect( &bt_sio_crit_sect );

}


/*===========================================================================

FUNCTION
  bt_sio_spp_scn_valid

DESCRIPTION
  This function verifies that the specified RFCOMM server channel number
  for an SPP port is valid.  If necessary, it automatically generates
  the number for server applications.

RETURN VALUE
  BT_EVR_GN_SUCCESS if SCN is valid;  otherwise an appropriate error value.

===========================================================================*/
LOCAL bt_event_reason_type bt_sio_spp_scn_valid
(
  uint8             i,                 /*  bt_sio_table index.  */
  uint8             rc_server_channel,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_event_reason_type  reason = BT_EVR_GN_SUCCESS;
  uint8                 j = BT_RC_SCN_SPP_MIN_AUTO_V;

  if ( (bt_sio_table[ i ].client_app == FALSE) &&
       (rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
  {
    /*  Automatically generate the RFCOMM server     */
    /*  channel number for this server application.  */

    while ( (rc_server_channel == BT_SPP_SCN_UNSPECIFIED) &&
            (j <= BT_SPP_SCN_MAX_AUTO_V) )
    {
      if ( bt_sio_find_entry_rc_server_channel( FALSE, j, NULL ) !=
              BT_SIO_NULL_IDX )
      {
        j++;
      }
      else
      {
        rc_server_channel = j;
      }
    }
    if ( rc_server_channel == BT_SPP_SCN_UNSPECIFIED )
    {
      reason = BT_EVR_SPP_OUT_OF_SERVER_CHANNELS;
    }
  }
  else if ( (rc_server_channel >= RC_SERV_CHAN_MIN_V) &&
            (rc_server_channel <= RC_SERV_CHAN_MAX_V) )
  {
    if ( bt_sio_find_entry_rc_server_channel(
           bt_sio_table[ i ].client_app,
           rc_server_channel, bd_addr_ptr ) != BT_SIO_NULL_IDX )
    {
      reason = BT_EVR_SPP_SERVER_CHANNEL_IN_USE;
    }
  }
  else
  {
    reason = BT_EVR_SPP_BAD_SERVER_CHANNEL;
  }

  if ( reason == BT_EVR_GN_SUCCESS )
  {
    bt_sio_table[ i ].rc_server_channel = rc_server_channel;
  }

  return ( reason );

}


/*===========================================================================

FUNCTION
  bt_sio_spp_open_params_valid

DESCRIPTION
  This function verifies that the parameters are valid to open a
  specified SPP port.

RETURN VALUE
  BT_EVR_GN_SUCCESS if parameters are valid;  otherwise an appropriate
    error value.

===========================================================================*/
LOCAL bt_event_reason_type bt_sio_spp_open_params_valid
(
  uint8             i,                 /*  bt_sio_table index.  */
  uint8             rc_server_channel,
  bt_bd_addr_type*  bd_addr_ptr
)
{

  bt_event_reason_type  reason;

  if ( (bt_sio_table[ i ].client_app != FALSE) &&
       (bt_sio_table[ i ].service_uuid != BT_SD_INVALID_UUID) &&
       (rc_server_channel == BT_SPP_SCN_UNSPECIFIED) )
  {
    /*  Client application connecting via UUID.  */
    reason = BT_EVR_GN_SUCCESS;

    bt_sio_table[ i ].rc_server_channel = rc_server_channel;
  }
  else
  {
    reason = bt_sio_spp_scn_valid( i, rc_server_channel, bd_addr_ptr );
  }

  return ( reason );

}

/*===========================================================================

FUNCTION
  bt_sio_link_in_tx_non_empty_fptr

DESCRIPTION
  This function links in the BT SIO "Gone Non-Empty" function into
  the transmit watermark of specified stream as necessary.

===========================================================================*/
LOCAL void bt_sio_link_in_tx_non_empty_fptr
(
  uint8  i
)
{

  void  (*ne_fptr)( void );

  ne_fptr = bt_sio_table[ i ].tx_wm_ptr->non_empty_func_ptr;

  if ( (ne_fptr != bt_sio_table[ i ].my_tx_non_empty_func_ptr) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_0) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_1) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_2) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_3) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_4) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_5) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_6) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_7) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_8) &&
       (ne_fptr != bt_sio_tx_wm_non_empty_cb_9)
       )
  {
    INTLOCK();
    bt_sio_table[ i ].orig_tx_non_empty_func_ptr = ne_fptr;
    bt_sio_table[ i ].tx_wm_ptr->non_empty_func_ptr =
      bt_sio_table[ i ].my_tx_non_empty_func_ptr;
    INTFREE();
  }

}


/*===========================================================================

FUNCTION
  bt_sio_link_in_tx_gone_empty_fptr

DESCRIPTION
  This function links in the BT SIO "Gone Empty" function into
  the transmit watermark of specified stream as necessary.

===========================================================================*/
LOCAL void bt_sio_link_in_tx_gone_empty_fptr
(
  uint8  i,
  void   (*flush_fptr)( void )

)
{

  void  (*ge_fptr)( void );

  ge_fptr = bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr;

  if ( (ge_fptr != bt_sio_table[ i ].my_tx_gone_empty_func_ptr) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_0) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_1) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_2) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_3) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_4) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_5) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_6) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_7) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_8) &&
       (ge_fptr != bt_sio_tx_wm_gone_empty_cb_9)
    )
  {
    INTLOCK();
    bt_sio_table[ i ].flush_tx_func_ptr = flush_fptr;
    bt_sio_table[ i ].orig_tx_gone_empty_func_ptr = ge_fptr;
    bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr =
      bt_sio_table[ i ].my_tx_gone_empty_func_ptr;
    INTFREE();
  }

}


/*===========================================================================

FUNCTION
  bt_sio_unlink_tx_non_empty_fptr

DESCRIPTION
  This function sets tx non-empty func ptr to it's original.

===========================================================================*/
LOCAL void bt_sio_unlink_tx_non_empty_fptr
(
  uint8  i
)
{
  void  (*ne_fptr)( void );

  ne_fptr = bt_sio_table[ i ].tx_wm_ptr->non_empty_func_ptr;

  if ( (ne_fptr == bt_sio_table[ i ].my_tx_non_empty_func_ptr) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_0) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_1) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_2) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_3) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_4) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_5) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_6) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_7) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_8) ||
       (ne_fptr == bt_sio_tx_wm_non_empty_cb_9)
       )
  {
    INTLOCK();
    bt_sio_table[ i ].tx_wm_ptr->non_empty_func_ptr =
      bt_sio_table[ i ].orig_tx_non_empty_func_ptr;
    bt_sio_table[ i ].orig_tx_non_empty_func_ptr = NULL;
    INTFREE();
  }
}

/*===========================================================================

FUNCTION
  bt_sio_unlink_tx_gne_empty_fptr

DESCRIPTION
  This function sets tx gne-empty func ptr to it's original.

===========================================================================*/
LOCAL void bt_sio_unlink_tx_gne_empty_fptr
(
  uint8  i
)
{
  void  (*ge_fptr)( void );

  ge_fptr = bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr;

  if ( (ge_fptr == bt_sio_table[ i ].my_tx_gone_empty_func_ptr) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_0) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_1) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_2) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_3) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_4) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_5) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_6) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_7) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_8) ||
       (ge_fptr == bt_sio_tx_wm_gone_empty_cb_9)
    )
  {
    INTLOCK();
    bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr =
      bt_sio_table[ i ].orig_tx_gone_empty_func_ptr;
    bt_sio_table[ i ].orig_tx_gone_empty_func_ptr = NULL;
    INTFREE();
  }  
}

/*===========================================================================

FUNCTION
  bt_sio_cmd_open

DESCRIPTION
  This function opens a stream across an interface over BT.

RETURN VALUE
  SIO_DONE_S:     Successfully opened stream
  SIO_BADP_S:     Bad parameters passed.
  SIO_UNAVAIL_S:  All available streams have been allocated.

===========================================================================*/
sio_status_type bt_sio_cmd_open
(
  sio_open_type*  open_ptr
)
{

  uint8                 i;
  bt_app_id_type        app_id;
  sio_status_type       status = SIO_DONE_S;
  bt_event_reason_type  reason;

  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( ( open_ptr != NULL )                                       &&
       ( bt_sio_find_entry_stream_id( open_ptr->stream_id ) ==
         BT_SIO_NULL_IDX )                                        &&
       ( (i = bt_sio_find_entry_app_id_port( BT_APP_ID_NULL )) !=
         BT_SIO_NULL_IDX ) )
  {
    if ( (open_ptr->stream_id != SIO_NO_STREAM_ID) &&
         (open_ptr->rx_queue != NULL) && (open_ptr->tx_queue != NULL) )
    {
      /* Check the bt_open_ptr for NULL before using it as parameter.  This will be 
         NULL when the port is NA. */
      if ( open_ptr->port_id == SIO_PORT_BT_NA || open_ptr->bt_open_ptr == NULL)
      {
        BT_MSG_API( "BT SIO CMD RX: Open SPID %x ",
                    open_ptr->port_id,
                    0, 0 );
      }
      else
      {
        BT_MSG_API( "BT SIO CMD RX: Open SPID %x C %x",
                    open_ptr->port_id,
                    open_ptr->bt_open_ptr->client_app, 0 );
      }

      app_id = bt_cmd_ec_get_app_id_and_register(
                 bt_sio_store_bt_event );
      
      if ( app_id != BT_APP_ID_NULL )
      {
        BT_MSG_API( "BT SIO TRX: EC Get AID %x Port %x",
                    app_id, open_ptr->port_id, 0 );
        BT_MSG_HIGH( "BT SIO port %x App ID is %x",
                     open_ptr->port_id, app_id, 0 );
  
        bt_sio_table[ i ].open           = TRUE;
        bt_sio_table[ i ].bt_app_id_port = app_id;
  
        bt_sio_table[ i ].stream_id      = open_ptr->stream_id;
        bt_sio_table[ i ].sio_port_id    = open_ptr->port_id;
        bt_sio_table[ i ].rc_pid         = BT_RC_NULL_PID;
  
        bt_sio_table[ i ].tx_flow_ctl    = open_ptr->tx_flow;
        bt_sio_table[ i ].rx_flow_ctl    = open_ptr->rx_flow;
        bt_sio_table[ i ].tx_wm_ptr      = open_ptr->tx_queue;
        bt_sio_table[ i ].rx_wm_ptr      = open_ptr->rx_queue;
        bt_sio_table[ i ].rx_func_ptr    = open_ptr->rx_func_ptr;
  
        bt_sio_link_in_tx_non_empty_fptr( i );
  
        if ( open_ptr->port_id == SIO_PORT_BT_NA )
        {
          bt_sio_table[ i ].client_app        = FALSE;
          bt_sio_table[ i ].service_uuid =
            BT_SD_SERVICE_CLASS_DIALUP_NETWORKING;
          bt_sio_table[ i ].service_version   = 0x0100;
          bt_sio_table[ i ].rc_server_channel = BT_RC_SCN_NA;
  
          memcpy( bt_sio_table[ i ].service_name_str,
                  BT_NA_SERV_NAME, BT_NA_SERV_NAME_LEN );
  
          bt_sio_table[ i ].
            service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN ] = 0;
  
        }
        else  /*  SIO_PORT_BT_SPP  */
        {
          bt_sio_table[ i ].enabled = TRUE;
  
          bt_sio_table[ i ].client_app =
            open_ptr->bt_open_ptr->client_app;
          bt_sio_table[ i ].service_uuid =
            open_ptr->bt_open_ptr->service_uuid;
          bt_sio_table[ i ].service_version =
            open_ptr->bt_open_ptr->service_version;
  
          if ( open_ptr->bt_open_ptr->max_frame_size !=
                 BT_SPP_MFS_AUTOMATIC )
          {
            bt_sio_table[ i ].rc_config.max_frame_size =
              open_ptr->bt_open_ptr->max_frame_size;
          }
  
          bt_sio_table[ i ].status_change_fptr =
            open_ptr->bt_open_ptr->status_change_fptr;
          bt_sio_table[ i ].config_change_fptr =
            open_ptr->bt_open_ptr->config_change_fptr;
          bt_sio_table[ i ].modem_status_fptr =
            open_ptr->bt_open_ptr->modem_status_fptr;
          bt_sio_table[ i ].line_error_fptr =
            open_ptr->bt_open_ptr->line_error_fptr;
  
          if ( open_ptr->bt_open_ptr->service_name_str_ptr != NULL )
          {
            memcpy( bt_sio_table[ i ].service_name_str,
                    open_ptr->bt_open_ptr->service_name_str_ptr,
                    BT_SD_MAX_SERVICE_NAME_LEN );
  
            bt_sio_table[ i ].
              service_name_str[ BT_SD_MAX_SERVICE_NAME_LEN ] = 0;
          }
  
          reason = bt_sio_spp_open_params_valid(
                     i, open_ptr->bt_open_ptr->rc_server_channel,
                     open_ptr->bt_open_ptr->bd_addr_ptr );
  
          if ( bt_sio_table[ i ].client_app != FALSE )
          {
            bt_sio_table[ i ].bd_addr =
              *open_ptr->bt_open_ptr->bd_addr_ptr;
          }
  
          if ( reason == BT_EVR_GN_SUCCESS )
          {
            bt_sio_open_spp( i );
          }
          else
          {
            bt_sio_open_error( i, reason, FALSE );
          }
        }
      }
      else
      {
        BT_ERR( "BT SIO: app id unavailable", 0, 0, 0 );
        status = SIO_UNAVAIL_S;
      }
    }
    else
    {
      status = SIO_BADP_S;
    }
  }
  else
  {
    status = SIO_UNAVAIL_S;
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );

  if ( status != SIO_DONE_S )
  {
    uint32           q_ptrs  = 0;
    sio_port_id_type port_id = SIO_PORT_NULL;
    if ( open_ptr != NULL )
    {
      q_ptrs  = (uint32)(open_ptr->rx_queue) | (uint32)(open_ptr->tx_queue);
      port_id = open_ptr->port_id;
    }
    BT_ERR( "BT SIO: Bad Cmd Open %x %x SPID %x", open_ptr, q_ptrs, port_id );
  }

  return ( status );

}


/*===========================================================================

FUNCTION
  bt_sio_cmd_transmit

DESCRIPTION
  This function will transmit data on a specified stream over BT.

===========================================================================*/
void bt_sio_cmd_transmit
(
  sio_stream_id_type  stream_id,
  dsm_item_type*      dsm_ptr
)
{

  uint8  i;

  if( TASKS_ARE_LOCKED() )
  {
    /* Rather than do the processing here, queue a command to ourselves to
     * handle the transmit later */
    BT_MSG_API( "BT SIO TX: queuing msg sid %x", 
                stream_id, 0, 0 );
    bt_cmd_sio_transmit( bt_sio_app_id, stream_id, dsm_ptr );

    return;
  }

  if ( dsm_ptr != NULL )
  {
    rex_enter_crit_sect( &bt_sio_crit_sect );

    if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
                BT_SIO_NULL_IDX )
    {
      if ( bt_sio_table[ i ].rc_pid != BT_RC_NULL_PID )
      {
        bt_sio_link_in_tx_non_empty_fptr( i );

        BT_MSG_API_PYLD( "BT SIO CMD TX: RC TXD SID %x PID %x",
                         stream_id, bt_sio_table[ i ].rc_pid, 0 );

        // Make sure there is enough room to fit the data on the wm
        if((bt_sio_table[ i ].tx_wm_ptr->current_cnt + 
            dsm_length_packet(dsm_ptr)) <= 
            bt_sio_table[ i ].tx_wm_ptr->dont_exceed_cnt)
        {
          dsm_enqueue( bt_sio_table[ i ].tx_wm_ptr, &dsm_ptr );
        }
        else
        {
          uint32 il = dsm_length_packet(dsm_ptr);
          BT_ERR( "BT SIO: Enq in WM failed! CC %d IL %d DEC %d", 
                  bt_sio_table[ i ].tx_wm_ptr->current_cnt, il, 
                  bt_sio_table[ i ].tx_wm_ptr->dont_exceed_cnt );
          dsm_free_packet( &dsm_ptr );
        }
      }
      else
      {
        BT_ERR( "BT SIO: Cmd TX Bad PID %x SID %x Idx %x",
                bt_sio_table[ i ].rc_pid, stream_id, i );
        dsm_free_packet( &dsm_ptr );
      }
    }
    else
    {
      BT_ERR( "BT SIO: Cmd TX Bad SID %x", stream_id, 0, 0 );
      dsm_free_packet( &dsm_ptr );
    }

    rex_leave_crit_sect( &bt_sio_crit_sect );
  }
  else
  {
    BT_ERR( "BT SIO: Cmd TX Bad DSM Ptr %x", dsm_ptr, 0, 0 );
  }

}


/*===========================================================================

FUNCTION
  bt_sio_cmd_ioctl

DESCRIPTION
  Allows control of an open stream over BT.

===========================================================================*/
void bt_sio_cmd_ioctl
(
  sio_stream_id_type     stream_id,
  sio_ioctl_cmd_type     cmd,
  sio_ioctl_param_type*  param_ptr
)
{

  uint8  i;

  /* Rather than do the processing here, queue a command to ourselves to
   * handle the IOCTL later */
  BT_MSG_API( "BT SIO IOCTL: queuing msg sid %x cmd %x", 
              stream_id, 
              cmd, 0 );

  switch( cmd )
  {
    case SIO_IOCTL_INBOUND_FLOW_ENABLE:
      bt_cmd_sio_in_flow_on( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_INBOUND_FLOW_DISABLE:
      bt_cmd_sio_in_flow_off( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_CHANGE_MODE:
      bt_cmd_sio_change_mode( bt_sio_app_id, stream_id,
                              param_ptr->open_ptr->stream_mode,
                              param_ptr->open_ptr->tx_queue,
                              param_ptr->open_ptr->rx_queue,
                              param_ptr->open_ptr->rx_func_ptr);
      break;
    case SIO_IOCTL_ENABLE_AUTODETECT:
      bt_cmd_sio_enable_autodetect( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_SET_FLOW_CTL:
      bt_cmd_sio_set_flow_ctl( bt_sio_app_id, stream_id,
                               param_ptr->flow_ctl.tx_flow,
                               param_ptr->flow_ctl.rx_flow );
      break;
    case SIO_IOCTL_ENABLE_DTR_EVENT:
      bt_cmd_sio_enable_dtr_event( bt_sio_app_id, stream_id, 
                                   param_ptr->enable_dte_ready_event );
      break;
    case SIO_IOCTL_DISABLE_DTR_EVENT:
      bt_cmd_sio_disable_dtr_event( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_CD_ASSERT:
      bt_cmd_sio_cd_assert( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_CD_DEASSERT:
      bt_cmd_sio_cd_deassert( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_RI_ASSERT:
      bt_cmd_sio_ri_assert( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_RI_DEASSERT:
      bt_cmd_sio_ri_deassert( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_DSR_ASSERT:
      bt_cmd_sio_dsr_assert( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_DSR_DEASSERT:
      bt_cmd_sio_dsr_deassert( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_RECORD_PKT_FUNC_PTR:
      bt_cmd_sio_record_pkt_func_ptr( bt_sio_app_id, stream_id,
                                      param_ptr->record_pkt_func_ptr );
      break;
    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:
      bt_cmd_sio_record_esc_func_ptr( bt_sio_app_id, stream_id,
                                      param_ptr->record_escape_func_ptr );
      break;
    case SIO_IOCTL_BT_CONFIGURE:
      bt_cmd_sio_bt_configure( bt_sio_app_id, stream_id,
                               param_ptr->bt_spp_config_ptr->baudrate,
                               param_ptr->bt_spp_config_ptr->format,
                               param_ptr->bt_spp_config_ptr->flow_ctrl,
                               param_ptr->bt_spp_config_ptr->xon_char,
                               param_ptr->bt_spp_config_ptr->xoff_char );
      break;
    case SIO_IOCTL_BT_DISCONNECT:
      bt_cmd_sio_bt_disconnect( bt_sio_app_id, stream_id );
      break;
    case SIO_IOCTL_CHANGE_BAUD_RATE:
      {
        BT_MSG_API( "BT SIO CMD RX: Set BR %x SID %x",
                    stream_id, 0, 0 );
        /* BT_TBD */
        break;
      }
    case SIO_IOCTL_CHANGE_BAUD_NOW:
      {
        BT_MSG_API( "BT SIO CMD RX: Set BR now %x SID %x",
                    stream_id, 0, 0 );
        /* BT_TBD */
        break;
      }
    case SIO_IOCTL_FLUSH_TX:
      bt_cmd_sio_bt_flush_tx( bt_sio_app_id, stream_id,
                              param_ptr->record_flush_func_ptr );
      break;


      /* For the rest, we have to get data from the table here. This is
       * not exactly safe, but necessary */
    case SIO_IOCTL_GET_FLOW_CTL:
      if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
           BT_SIO_NULL_IDX )
      {
        param_ptr->flow_ctl.tx_flow = bt_sio_table[ i ].tx_flow_ctl;
        param_ptr->flow_ctl.rx_flow = bt_sio_table[ i ].rx_flow_ctl;
      }
      break;
    case SIO_IOCTL_DTE_READY_ASSERTED:
      /* Bummer, we have to get data from the table here. This is 
       * not exactly safe, but necessary */
      if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
           BT_SIO_NULL_IDX )
      {
        BT_MSG_API( "BT SIO CMD RX: Get DTR %x SID %x",
                    *param_ptr->dte_ready_asserted,
                    stream_id, 0 );
        if ( BT_RC_MS_RTC_ASSERTED_B &
             bt_sio_table[ i ].far_modem_status )
        {
          *param_ptr->dte_ready_asserted = TRUE;
        }
        else
        {
          *param_ptr->dte_ready_asserted = FALSE;
        }
      }
      break;
    case SIO_IOCTL_GET_CURRENT_RTS:
      if ( (i = bt_sio_find_entry_stream_id( stream_id )) != BT_SIO_NULL_IDX )
      {
        if ( BT_RC_MS_RTR_ASSERTED_B & bt_sio_table[ i ].far_modem_status )
        {
          *param_ptr->rts_asserted = TRUE;
        }
        else
        {
          *param_ptr->rts_asserted = FALSE;
        }
        BT_MSG_API( "BT SIO CMD RX: Get RTS %x SID %x",
                    *param_ptr->rts_asserted,
                    stream_id, 0 );
      }
      break;

    case SIO_IOCTL_BT_GET_STATUS:
      /* Bummer, we have to get data from the table here. This is 
       * not exactly safe, but necessary */
      if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
           BT_SIO_NULL_IDX )
      {
        BT_MSG_API( "BT SIO CMD RX: Get Status SID %x",
                    stream_id, 0, 0 );
        bt_sio_get_status( i, param_ptr->bt_spp_status_ptr );
      }
      break;
    case SIO_IOCTL_BEGIN_FAX_IMAGE:
    default:
      BT_ERR("BT SIO CMD IOCTL: Bad sid %x cmd %x while TASKLOCKed", 
             stream_id, cmd, 0);
  }
    
  return;
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_close

DESCRIPTION
  This procedure closes an SIO stream over BT.

===========================================================================*/
void bt_sio_cmd_close
(
  sio_stream_id_type  stream_id,
  void                (*close_func_ptr) (void)
)
{

  uint8  i;

  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
              BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Close SID %x PID %x ST %x",
                stream_id, bt_sio_table[ i ].rc_pid,
                bt_sio_table[ i ].spp_state );

    bt_sio_table[ i ].open           = FALSE;
    bt_sio_table[ i ].close_func_ptr = close_func_ptr;

    if ( bt_sio_table[ i ].tx_wm_ptr->non_empty_func_ptr ==
           bt_sio_table[ i ].my_tx_non_empty_func_ptr )
    {
      INTLOCK();
      bt_sio_table[ i ].tx_wm_ptr->non_empty_func_ptr =
        bt_sio_table[ i ].orig_tx_non_empty_func_ptr;
      INTFREE();
    }
    bt_sio_table[ i ].orig_tx_non_empty_func_ptr = NULL;

    if ( bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr ==
           bt_sio_table[ i ].my_tx_gone_empty_func_ptr )
    {
      INTLOCK();
      bt_sio_table[ i ].tx_wm_ptr->gone_empty_func_ptr =
        bt_sio_table[ i ].orig_tx_gone_empty_func_ptr;
      INTFREE();
    }
    bt_sio_table[ i ].orig_tx_gone_empty_func_ptr = NULL;

    if ( bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_SPP )
    {
      bt_sio_table[ i ].enabled = FALSE;

      bt_sio_disconnect_rc( i );
    }
    else if ( bt_sio_table[ i ].sio_port_id == SIO_PORT_BT_NA )
    {
      if ( bt_sio_table[ i ].enabled != FALSE )
      {
        BT_MSG_DEBUG( "BT SIO: Closed enabled NA StID %x En %x I %x",
                      stream_id, bt_sio_table[ i ].enabled, i );

        bt_sio_ev_send_na_disabled( i );
        
        bt_sio_disable_na( i );
      }
      else
      {
        bt_sio_disconnect_rc( i );
      }
    }
    else
    {
      BT_ERR( "BT SIO: Cmd Close Bad PID %x I %x SID %x",
              bt_sio_table[ i ].sio_port_id, i, stream_id );
    }
  }
  else
  {
    BT_ERR( "BT SIO: Cmd Close Bad SID %x", stream_id, 0, 0 );
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );

}


/*===========================================================================

FUNCTION
  bt_sio_cmd_in_flow

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_in_flow
(
  sio_stream_id_type  stream_id,
  sio_ioctl_cmd_type  cmd
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
              BT_SIO_NULL_IDX )
  {
    switch ( cmd )
    {
      case SIO_IOCTL_INBOUND_FLOW_ENABLE:
        {
          BT_MSG_API( "BT SIO CMD RX: In Flow ON SID %x",
                      stream_id, 0, 0 );
          bt_sio_table[ i ].rx_flow_on = TRUE;
          if ( bt_sio_table[ i ].rc_pid != BT_RC_NULL_PID )
          {
            bt_cmd_rc_set_port_rx_fc( bt_sio_table[ i ].bt_app_id_port,
                                      bt_sio_table[ i ].rc_pid,
                                      bt_sio_table[ i ].rx_flow_on );
          }
        }
        break;
      case SIO_IOCTL_INBOUND_FLOW_DISABLE:
        {
          BT_MSG_API( "BT SIO CMD RX: In Flow OFF SID %x",
                      stream_id, 0, 0 );
          bt_sio_table[ i ].rx_flow_on = FALSE;
          if ( bt_sio_table[ i ].rc_pid != BT_RC_NULL_PID )
          {
            bt_cmd_rc_set_port_rx_fc(
              bt_sio_table[ i ].bt_app_id_port,
              bt_sio_table[ i ].rc_pid,
              bt_sio_table[ i ].rx_flow_on );
          }
          break;
        }
    }
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_change_mode

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_change_mode
(
  sio_stream_id_type  stream_id,
  sio_stream_mode_type   stream_mode,
  dsm_watermark_type    *tx_queue,
  dsm_watermark_type    *rx_queue,
  sio_rx_func_ptr_type   rx_func_ptr
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Set Mode %x SID %x",
                stream_mode, stream_id, 0 );

#ifdef FEATURE_BT_SIO_ESC_CODE_DETECTION
    if ( ( bt_sio_table[i].stream_mode == SIO_DS_RAWDATA_MODE )  || 
         ( bt_sio_table[i].stream_mode == SIO_DS_PKT_MODE ) )
    {
      rex_clr_timer ( &bt_sio_na_info.esc_timer );
      bt_sio_na_info.esc_state = BT_SIO_ESC_SEQ_ENABLE;
    }
#endif /* FEATURE_BT_SIO_ESC_CODE_DETECTION */

    /* Save new TX watermark */
    if ( bt_sio_table[ i ].tx_wm_ptr != tx_queue )
    {
      if ( bt_sio_table[ i ].tx_wm_ptr != NULL )
      {
        if ( bt_sio_table[ i ].tx_wm_ptr->current_cnt > 0 )
        {
          BT_ERR( "BT SIO: Emptying non-empty WM %x",
                  bt_sio_table[ i ].tx_wm_ptr->current_cnt, 0, 0 );
        }
        dsm_empty_queue( bt_sio_table[ i ].tx_wm_ptr );
      }

      INTLOCK();

      /* Restore orig tx wm callbacks */
      bt_sio_unlink_tx_non_empty_fptr ( i );
      bt_sio_unlink_tx_gne_empty_fptr ( i );

      bt_sio_table[ i ].tx_wm_ptr = tx_queue;
            
      bt_sio_link_in_tx_non_empty_fptr( i );

      INTFREE();
    }

    /* Save new RX watermark */
    if ( bt_sio_table[ i ].rx_wm_ptr != rx_queue )
    {
      if (bt_sio_table[ i ].rx_wm_ptr != NULL)
      {
        if ( bt_sio_table[ i ].rx_wm_ptr->current_cnt > 0 )
        {
          BT_ERR( "Erasing non-empty watermark", 0, 0, 0 );
        }
        dsm_empty_queue( bt_sio_table[ i ].rx_wm_ptr );
      }

      INTLOCK();
      bt_sio_table[ i ].rx_wm_ptr = rx_queue;
      INTFREE();
    }

    /* Save RX func ptr and new stream mode. */
    INTLOCK();
    bt_sio_table[ i ].rx_func_ptr = rx_func_ptr;
    INTFREE();

    bt_sio_table[ i ].stream_mode = stream_mode;
    bt_sio_table[ i ].enable_auto_detect = FALSE;
    bt_sio_table[ i ].auto_detect_state  = NO_BYTES;
    bt_sio_table[ i ].pkt_func_ptr       = NULL;
    bt_sio_table[ i ].esc_func_ptr       = NULL;

    if ( bt_sio_table[ i ].rc_pid != BT_RC_NULL_PID )
    {
      bt_rc_update_wm_ptrs( bt_sio_table[ i ].rc_pid,
                            bt_sio_table[ i ].tx_wm_ptr,
                            bt_sio_table[ i ].rx_wm_ptr );
    }
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_enable_autodetect

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_enable_autodetect
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: AutoD ON SID %x",
                stream_id, 0, 0 );
    
    bt_sio_table[ i ].enable_auto_detect = TRUE;
    bt_sio_table[ i ].stream_mode = SIO_DS_AUTODETECT_MODE;
    bt_sio_table[ i ].auto_detect_state = NO_BYTES;
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_set_flow_ctl

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_set_flow_ctl
(
  sio_stream_id_type  stream_id,
  sio_flow_ctl_type   tx_flow,
  sio_flow_ctl_type   rx_flow
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Set FC SID %x TX %x RX %x",
                stream_id,
                tx_flow,
                rx_flow );
    bt_sio_table[ i ].tx_flow_ctl = tx_flow;
    bt_sio_table[ i ].rx_flow_ctl = rx_flow;
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_enable_dtr_event

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_enable_dtr_event
(
  sio_stream_id_type  stream_id,
  sio_vv_func_ptr_type enable_dte_ready_event
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: DTR CB ON SID %x",
                stream_id, 0, 0 );
    bt_sio_table[ i ].dtr_cb_ptr = enable_dte_ready_event;
    if ( bt_sio_table[ i ].dtr_cb_ptr == NULL )
    {
      BT_ERR( "BT SIO: DTR ON with NULL CB SID %x",
              stream_id, 0, 0 );
    }
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_disable_dtr_event

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_disable_dtr_event
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: DTR CB OFF SID %x",
                stream_id, 0, 0 );
    bt_sio_table[ i ].dtr_cb_ptr = NULL;
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_cd_assert

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_cd_assert
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: CD ON SID %x",
                stream_id, 0, 0 );
    bt_sio_write_local_modem_status( i,
                                     BT_RC_MS_DV_ASSERTED_B, 
                                     BT_RC_MS_DV_ASSERTED_B );
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_cd_deassert

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_cd_deassert
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: CD OFF SID %x",
                stream_id, 0, 0 );
    bt_sio_write_local_modem_status( i,
                                     BT_RC_MS_DV_ASSERTED_B, 
                                     0 );
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_ri_assert

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_ri_assert
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: RI ON SID %x",
                stream_id, 0, 0 );
    bt_sio_write_local_modem_status( i,
                                     BT_RC_MS_IC_ASSERTED_B, 
                                     BT_RC_MS_IC_ASSERTED_B );
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_ri_deassert

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_ri_deassert
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: RI OFF SID %x",
                stream_id, 0, 0 );
    bt_sio_write_local_modem_status( i,
                                     BT_RC_MS_IC_ASSERTED_B, 
                                     0 );
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_dsr_assert

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_dsr_assert
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: DSR ON SID %x",
                stream_id, 0, 0 );
    bt_sio_write_local_modem_status( i,
                                     BT_RC_MS_RTC_ASSERTED_B, 
                                     BT_RC_MS_RTC_ASSERTED_B );
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_dsr_deassert

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_dsr_deassert
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: DSR OFF SID %x",
                stream_id, 0, 0 );
    bt_sio_write_local_modem_status( i,
                                     BT_RC_MS_RTC_ASSERTED_B, 
                                     0 );
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_record_pkt_func_ptr

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_record_pkt_func_ptr
(
  sio_stream_id_type   stream_id,
  sio_vv_func_ptr_type func_ptr
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Set Pkt ptr SID %x",
                stream_id, 0, 0 );
    bt_sio_table[ i ].pkt_func_ptr = func_ptr;
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_record_esc_func_ptr

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_record_esc_func_ptr
(
  sio_stream_id_type  stream_id,
  sio_vv_func_ptr_type func_ptr
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Set Esc ptr SID %x",
                stream_id, 0, 0 );
    bt_sio_table[ i ].esc_func_ptr = func_ptr;
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_bt_configure

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_bt_configure
(
  sio_stream_id_type  stream_id,
  uint8               baudrate,
  uint8               format,
  uint8               flow_ctrl,
  uint8               xon_char,
  uint8               xoff_char
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Cfg SID %x",
                stream_id, 0, 0 );
    
    bt_cmd_rc_write_sp_config( bt_sio_table[ i ].bt_app_id_port,
                               bt_sio_table[ i ].rc_pid,
                               baudrate,
                               format,
                               flow_ctrl,
                               xon_char,
                               xoff_char );
  }

  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_bt_disconnect

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_bt_disconnect
(
  sio_stream_id_type  stream_id
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: DISC SID %x",
                stream_id, 0, 0 );
    
    bt_sio_disconnect_rc( i );
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_cmd_bt_flush_tx

DESCRIPTION

===========================================================================*/
void bt_sio_cmd_bt_flush_tx
(
 sio_stream_id_type   stream_id,
 sio_vv_func_ptr_type func_ptr
)
{
  uint8 i;
  rex_enter_crit_sect( &bt_sio_crit_sect );

  if ( (i = bt_sio_find_entry_stream_id( stream_id )) !=
       BT_SIO_NULL_IDX )
  {
    BT_MSG_API( "BT SIO CMD RX: Set Flu ptr SID %x",
                stream_id, 0, 0 );
    if ( (bt_sio_table[ i ].spp_state != BT_SPP_ST_CONNECTED) ||
         (bt_sio_table[ i ].tx_wm_ptr->current_cnt == 0) )
    {
      if ( func_ptr != NULL )
      {
        (func_ptr)();
      }
    }
    else /* connected and non-empty */
    {
      bt_sio_link_in_tx_gone_empty_fptr( i, func_ptr );
    }
  }
  rex_leave_crit_sect( &bt_sio_crit_sect );
}


/*===========================================================================

FUNCTION
  bt_sio_process_command

DESCRIPTION
  Processes an SIO command.

===========================================================================*/
void bt_sio_process_command
(
  bt_cmd_msg_type*  sio_cmd_ptr
)
{
  sio_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  switch ( sio_cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_CMD_SIO_TRANSMIT:
      bt_sio_cmd_transmit( 
        sio_cmd_ptr->cmd_msg.cmd_sio_transmit.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_transmit.dsm_ptr );
      break;

    case BT_CMD_SIO_IN_FLOW_ON:
    case BT_CMD_SIO_IN_FLOW_OFF:
      // don't need to init param here
      bt_sio_cmd_in_flow( 
        sio_cmd_ptr->cmd_msg.cmd_sio_in_flow.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_in_flow.cmd );
      break;

    case BT_CMD_SIO_CHANGE_MODE:
      bt_sio_cmd_change_mode( 
        sio_cmd_ptr->cmd_msg.cmd_sio_change_mode.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_change_mode.stream_mode,
        sio_cmd_ptr->cmd_msg.cmd_sio_change_mode.tx_queue,
        sio_cmd_ptr->cmd_msg.cmd_sio_change_mode.rx_queue,
        sio_cmd_ptr->cmd_msg.cmd_sio_change_mode.rx_func_ptr );
      break;
    case BT_CMD_SIO_ENABLE_AUTODETECT:
      bt_sio_cmd_enable_autodetect( 
        sio_cmd_ptr->cmd_msg.cmd_sio_enable_autodetect.stream_id );
      break;
    case BT_CMD_SIO_SET_FLOW_CTL:
      bt_sio_cmd_set_flow_ctl( 
        sio_cmd_ptr->cmd_msg.cmd_sio_set_flow_ctl.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_set_flow_ctl.tx_flow,
        sio_cmd_ptr->cmd_msg.cmd_sio_set_flow_ctl.rx_flow );
      break;
    case BT_CMD_SIO_ENABLE_DTR_EVENT:
      bt_sio_cmd_enable_dtr_event( 
        sio_cmd_ptr->cmd_msg.cmd_sio_enable_dtr_event.stream_id, 
        sio_cmd_ptr->cmd_msg.cmd_sio_enable_dtr_event.
          enable_dte_ready_event );
      break;
    case BT_CMD_SIO_DISABLE_DTR_EVENT:
      bt_sio_cmd_disable_dtr_event( 
        sio_cmd_ptr->cmd_msg.cmd_sio_disable_dtr_event.stream_id );
      break;
    case BT_CMD_SIO_CD_ASSERT:
      bt_sio_cmd_cd_assert( 
        sio_cmd_ptr->cmd_msg.cmd_sio_cd_assert.stream_id );
      break;
    case BT_CMD_SIO_CD_DEASSERT:
      bt_sio_cmd_cd_deassert( 
        sio_cmd_ptr->cmd_msg.cmd_sio_cd_deassert.stream_id );
      break;
    case BT_CMD_SIO_RI_ASSERT:
      bt_sio_cmd_ri_assert( sio_cmd_ptr->cmd_msg.cmd_sio_ri_assert.
                              stream_id );
      break;
    case BT_CMD_SIO_RI_DEASSERT:
      bt_sio_cmd_ri_deassert( 
        sio_cmd_ptr->cmd_msg.cmd_sio_ri_deassert.stream_id );
      break;
    case BT_CMD_SIO_DSR_ASSERT:
      bt_sio_cmd_dsr_assert( 
        sio_cmd_ptr->cmd_msg.cmd_sio_dsr_assert.stream_id );
      break;
    case BT_CMD_SIO_DSR_DEASSERT:
      bt_sio_cmd_dsr_deassert( 
        sio_cmd_ptr->cmd_msg.cmd_sio_dsr_deassert.stream_id );
      break;
    case BT_CMD_SIO_RECORD_PKT_FUNC_PTR:
      bt_sio_cmd_record_pkt_func_ptr( 
        sio_cmd_ptr->cmd_msg.cmd_sio_record_pkt_func_ptr.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_record_pkt_func_ptr.func_ptr );
      break;
    case BT_CMD_SIO_RECORD_ESC_FUNC_PTR:
      bt_sio_cmd_record_esc_func_ptr( 
        sio_cmd_ptr->cmd_msg.cmd_sio_record_esc_func_ptr.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_record_esc_func_ptr.func_ptr );
      break;
    case BT_CMD_SIO_BT_CONFIGURE:
      bt_sio_cmd_bt_configure( 
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_configure.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_configure.baudrate,
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_configure.format,
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_configure.flow_ctrl,
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_configure.xon_char,
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_configure.xoff_char );
      break;
    case BT_CMD_SIO_BT_DISCONNECT:
      bt_sio_cmd_bt_disconnect( 
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_disconnect.stream_id );
      break;
    case BT_CMD_SIO_BT_FLUSH_TX:
      bt_sio_cmd_bt_flush_tx( 
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_flush_tx.stream_id,
        sio_cmd_ptr->cmd_msg.cmd_sio_bt_flush_tx.func_ptr );
      break;
    default:
      BT_MSG_API("BT SIO CMD RX: Unknown cmd",0,0,0);
      sio_cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}

#if defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS)
#error code not present
#endif /* defined(FEATURE_MT_OBEX) && defined(FEATURE_PCAA_SIRIUS) */

#endif /* FEATURE_BT */
