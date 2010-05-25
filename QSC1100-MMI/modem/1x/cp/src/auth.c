/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


          M A I N    A U T H E N T I C A T I O N    T A S K

GENERAL DESCRIPTION
  This module performs all authentication functions required by IS-95A and is
  also the server for the R-UIM card ( Removable User Identity Module) when
  FEATURE_UIM_RUIM is defined. In the presence of FEATURE_UIM_RUIM, the Authentication
  is performed by the RUIM card and this task acts as the server for the RUIM.

EXTERNALIZED FUNCTIONS
  auth_validate_a_key
    Determines if a given A-key is valid.

  auth_private_lcm_available
    Tells whether or not a private long code mask is available.

  auth_get_count
    Returns the current value of COUNTsp.

  auth_calculate_digits
    Packs the origination or sms destination digits .

  auth_encryption_possible
    Tells if message encryption is possible at the current time.

  auth_encrypt_cdma
    Encrypts a CDMA message to be transmitted on the traffic channel.

  auth_decrypt_cdma
    Decrypts a CDMA message received on the traffic channel.

  auth_encrypt_analog
    Encrypts an analog message to be transmitted on the voice channel.

  auth_decrypt_analog
    Decrypts an analog message received on the voice channel.

  auth_cmd
    External interface to send a command to the authentication task.

  auth_task
    This is the entrance procedure for the Authentication/ RUIM  task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The AUTH_INIT_F command must be sent before successful authentication
  can be performed.

Copyright (c) 1995-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/auth.c_v   1.12   03 Sep 2002 22:10:42   azafer  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/auth.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/07   pg      Lint clean up.
05/14/07   pg      Use ESN ME for RUN CAVE and other authentication commands 
                   regardless of UIM ID usage indicator provided by card 
                   under FEATURE_UIM_JCDMA_RUIM_SUPPORT.
04/26/06   an      Fixed Lint Errors
01/19/06   fc      Merged the following :
           fh      Added AKA and MI support.
11/30/05   va      Added new function auth_send_update_a_key_cmd  which is 
                   for calling from SVCPRG module.
11/30/05   an      Mainlined FEATURE_IS2000
10/10/05   fc      Removed including of enc.h
05/03/05   fc      Mainlined FEATURE_IS2000.
12/13/04   az      Fixed a variable name
12/10/04   sb      Fixed lint issue.
12/05/04   az      Submission done for Wes Li. This is for CR 53350 where 
                   a word is written to a pointer pointed to a byte.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
08/17/04   yll     For burst type 0x3e and 0x3f of a data burst message, only
                   encrypt the CHARi after the first two bytes.
03/19/04   sb      Removed auth_switch_to_private_lcm.
03/09/04   ph      Change slot to AUTOMATIC in command to UIM.
06/20/03   bkm     Clear T64 timer when SSD Update is received to prevent
                   timer expiration from aborting a second SSD Update attempt
                   after an Idle Handoff.
01/28/03   ts      Make sure UIM can power off whether RTRE control is NV or
                   UIM.
09/03/02   ts      Added support for Run Time R-UIM Enable.
08/26/02   ck      Featurised the processing of CAI_EFWIM_MSG around the feature
                   FEATURE_IS2000_REL_A.
07/25/02   jqi     Updated with task operation interface.
04/12/02   lh      Added support for subaddress in Orig_C.
03/12/02   lh      Added CMEA support for release A messages.
08/09/01   sb      Merged following from common archive:
                   07/27/01   ts      Allow UIM to power off after initializing
                                      (reading call count).
05/25/01   ts      Removed RUIM server code from auth and added interface to
                   new UIM server.
04/12/01   fc      Renamed mcc_report_event_timer to mclog_report_event_timer.
03/12/01   fas     Backed out call to MSG_LOW.
03/09/01   fas     Added includes for comdef.h and customer.h.
                   Added call to MSG_LOW to inhibit compiler warning.
03/08/01   fc      Fixed timer ID of T64m and moved "#include mccdma,h"
                   oustide feature #ifdef to support events report.
03/06/01   fc      Added support of events report: Timer expired.
02/16/01   ck      Fixed the issue where 8 digits of CHV was incorrectly
                   formatted by the CHV formatting routine.
02/07/01   wli     Split RUIM_UPDATE_RECORD_ST to two states:
                   RUIM_UPDATE_LINEAR_FIXED_RECORD_ST and
                   RUIM_UPDATE_CYCLIC_RECORD_ST.
1/27/01    wli     Added new RUIM commands, INVALIDATE and REHABILITATE.
01/11/01   ts      Added a call to flush the nvruim cache when resetting the
                   RUIM.
12/18/00   ck      Fixed the problem where the bytes of the request offset
                   in the SSPR Config request were swapped causing the R-UIM
                   to reject the request with 'Data Size Mismatch'.
11/28/00   ck      Added support for IS-683A with the R-UIM.
                   Added support for STORE_ESN_ME and STATUS command.
                   Added the debug feature for Unblocking the CHV.
                   Added paths for new EFs in the table.
                   Fixed the issue with zeroing 3 extra bytes in the RUN_CAVE.
                   Added a different time-out for the Commit command.
                   Changed the RAND_SEED_F command to BS_CHAL_F.
                   Fixed the problem where an external command that got xlated
                   to an internal command caused a buffer leak.
11/14/00   ts      Added access to the MSISDN in the CDMA directory.  Added
                   support for a faster RUIM clock frequency.  Fixed the APDU
                   for the status command.
10/11/00   ts      Include the RUIM status with auth reports.
07/11/00   ms      Added functionality to return just the size of an EF in records,
07/16/00   ck      Added support to select the CDMA DF before sending any
                   authentication related commands to the card.
                   Fixed the bug where the disabled CHV was causing the phone
                   to go offline.
06/13/00   ck      Added a debug feature to not do PPS since the cards do not do
                   PPS and the ATR tells us that they do.
06/12/00   ck      Added functionality to parse the ATR to determine if PPS can
                   be sent.
                   Added the functionality of reading and writing record from
                   RUIM.
06/06/00   ck      Cleaned up the warning caused by a bad merge for the prev fix.
06/06/00   ck      Fixed the problem where NEW_CHV command followed by any
                   command before powering down the RUIM rsulted in the cmd
                   being ignored.
06/01/00   ck      Added debug feature for ESN in the command RUN CAVE to flip
                   the byte ordering and changed the polling of RUIM based on
                   the status of the RUIM.
05/30/00   ck      Changed instances of RUIM_UI_CHV_SIG to RUIM_CHV_SIG.
05/01/00   ck      Added support for RUIM software. AUTH code has been
                   featurised such that the basic task functionality is
                   obtained by defining either FEATURE_AUTH or FEATURE_RUIM.
                   Authentication functionality performed by CAVE is defined
                   under FEATURE_AUTH and is ifdefed out when FEATURE_RUIM is
                   defined.
                   Various RUIM functions have been added.
04/06/00   va      Merged the following from MSM3100_CP.03.00.17
           ry      Added OTAPA support
08/13/99   jq      Changed the return mechanism of auth_get_nv_item() so the
                   function also returns.
06/17/99   ck      Fixed the problem where the CMEA and VPM keys were computed
                   based on the old SSD during re-authentication.
05/06/99   kmp     Changed T_ACP and T_AUTH to FEATURE_ACP and FEATURE_AUTH
                   respectively.
03/18/99   ck      Merged in the changes for A-Key exchange and
                   Re-Authentication.
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
07/28/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
06/21/96   dna     Removed AUTH_BAD_SSD_S status.  Always consider SSD valid.
05/15/96   dna     CMEA key generation status now set ASAP.
11/17/95   dna     Report back when keys are generated.
11/16/95   jca     Removed INTLOCK/INTFREE pair in function auth_cmd().
10/05/95   dna     re-initialize after A_Key update
08/03/95   dna     Added support for message encryption
06/07/95   dna     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -e766*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#if defined (FEATURE_AUTH) || defined (FEATURE_RUIM)
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "msg.h"
#include "err.h"
#include "mc.h"
#include "mccdma.h"
#include "queue.h"
#include "cai.h"
#include "auth.h"
#ifdef FEATURE_AUTH
#include "authi.h"
#include "cave.h"
#endif /* FEATURE_AUTH */
#include "srchmc.h"
#include "bit.h"
#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */
#include "memory.h"

#ifdef FEATURE_UIM_RUIM

#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
#include "tmc.h"
#endif

#include "uim.h"
#include "nvruimi.h"
#ifdef FEATURE_OTASP
#include "otaspe.h"
#endif /* FEATURE_OTASP */
#endif /* FEATURE_UIM_RUIM */
#include "mclog.h"

/*lint -e818 */

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_AUTH

#define AUTH_THREE_BITS  3
#define AUTH_FOUR_BITS   4
#define AUTH_FIVE_BITS   5
#define AUTH_EIGHT_BITS  8
#define AUTH_TWO_BYTES   2

/* Suppress the following lint messages:
   e413: Likely use of null pointer in left argument to operator '->',
         which is heavily used in FPOS and FSIZ macros.
   e734:  Loss of precision (assignment).
*/
/*lint -e734 -e413 */

#define AUTH_RECORD_TYPE_SIZE FSIZ( cai_rec_hdr_type, record_type )
#define AUTH_RECORD_LEN_SIZE  FSIZ( cai_rec_hdr_type, record_len )

/* Macro to unpack the T field of an analog reverse voice channel msg */
#define AUTH_VC_MSG_T( ptr ) b_unpackb( ptr,  3,  1)

/* Macro to unpack the NAWC field of a called address reverse VC msg */
#define AUTH_VC_NAWC( ptr ) b_unpackb( ptr,  1,  2)

/* Macros to replace the characters into forward voice channel info ords */
#define AUTH_VC_MSG_CHAR1( value, ptr ) b_packb( value, ptr,  4,  8)
#define AUTH_VC_MSG_CHAR2( value, ptr ) b_packb( value, ptr, 12,  8)
#define AUTH_VC_MSG_CHAR3( value, ptr ) b_packb( value, ptr, 20,  8)

/* Buffer to hold data to be encrypted */
byte auth_temp_buf[CAI_REV_TC_MAX_MSG_BYTES];

#endif /* FEATURE_AUTH */

/* Buffers to be placed on the auth_free_q */
#define                      AUTH_NUM_CMD_BUFS 3

auth_cmd_type auth_cmd_pool[ AUTH_NUM_CMD_BUFS ];

/* Queue to hold commands for auth */
q_type          auth_cmd_q;

/* Queue to hold free buffers to be placed on the auth_cmd_q */
q_type auth_free_q;

/* Buffer to hold report to be returned to task issuing command */
auth_rpt_type auth_rpt_buf;

/* Timer definitions */
rex_timer_type auth_rpt_timer;  /* Watchdog timer */
#ifdef FEATURE_AUTH
rex_timer_type auth_ssd_timer;  /* SSD timeout timer */
#endif /* FEATURE_AUTH */

#ifdef FEATURE_AUTH
/* Boolean to indicate if we are in the middle of performing SSD update */
LOCAL boolean auth_ssd_update_in_progress = FALSE;

/* Indicates if CMEA key has been generated */
boolean auth_valid_CMEA_key = FALSE;

/* Indicates if Private Long Code Mask has been generated */
boolean auth_valid_PLCM = FALSE;

/* Indicates if auth_signature() has been told to save_registers */
boolean auth_saved_registers = FALSE;

/* Status of auth initialization */
auth_init_status_type auth_init_status = AUTH_UNINITIALIZED;

/* COUNTsp */
byte auth_count = 0;

/* NAM index used in last auth init, used for updating auth_count. */
byte auth_stored_NAM_index;

/* A_KEY_TEMP stores the newly generated A-Key  */
qword a_key_temp;

/* Indicates if a new A_KEY_TEMP has been generated */
boolean a_key_temp_valid = FALSE;

/* Indicates if new SSD has been generated */
boolean new_ssd_valid = FALSE;

/* Indicates that the new ssd has been used for generating the AUTHR in the
   Re-Authentication Response. This flag is primarily used to choose the
   correct SSD during key generation(CMEA and VPM). We cannot use new_ssd_valid
   as there is a chance that we might need to use the old SSD when new SSD is
   valid( If for some reason, encryption is turned on after SSD update and
   before Re-Authentication) */
LOCAL boolean used_new_ssd = FALSE;

#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
/* Response buffer */
uim_rpt_type auth_uim_rsp_buf;

/* Temporary COUNTsp */
word temp_auth_count = 0;

#ifdef FEATURE_AUTH
ulpn_type auth_plcm;
/* plcm returned by RUIM */

dword auth_randbs;
/* Randbs returned by RUIM */
#endif /* FEATURE_AUTH */

/* Constants for origination message translation */

#define AUTH_DIAL_0                0xA
#define AUTH_DIAL_STAR             0xB
#define AUTH_DIAL_PND              0xC
#define AUTH_DIAL_NULL             0x0

/*===========================================================================

            FUNCTION PROTOTYPES

This section contains local function prototypes for the RUIM interface code.

===========================================================================*/
LOCAL word  auth_wait
(
  /* Mask of signals to wait for */
  word  sigs
);

LOCAL void auth_uim_response_callback ( uim_rpt_type *uim_rpt_buf_ptr );

#endif /*FEATURE_UIM_RUIM */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION AUTH_GET_NV_ITEM

DESCRIPTION
  This function gets a specific item from NV.

DEPENDENCIES
  None.

RETURN VALUE
  Status returned from the NV read request.  Status can be either:
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active
  Any other status causes ERR_FATAL and the function does not return.

SIDE EFFECTS
  While this function is running all other task activities are
  suspended except for watchdog kicking, and until the NV item is
  read in.  If any of read status other then the one described above
  is returned then an ERR_FATAL call is made.

===========================================================================*/

nv_stat_enum_type auth_get_nv_item
(
  nv_items_enum_type  item_code,       /* Item to get */
  nv_item_type        *item_ptr        /* Pointer where to put the item */
)
{
  static nv_cmd_type  nv_cmd_buf;
    /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare command buffer to NV. */

  nv_cmd_buf.cmd = NV_READ_F;
  nv_cmd_buf.tcb_ptr = &auth_tcb;
  nv_cmd_buf.sigs = AUTH_NV_CMD_SIG;
  nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return  */
  nv_cmd_buf.item = item_code;
  nv_cmd_buf.data_ptr = item_ptr;

  /* Clear signal, issue the command, and wait for the respone. */

  (void) rex_clr_sigs(&auth_tcb, AUTH_NV_CMD_SIG);
  nv_cmd(&nv_cmd_buf);

  /* -----------------------------
  ** Kick watchdog and reset timer
  ** ----------------------------- */
  dog_report(DOG_AUTH_RPT);
  (void) rex_set_timer( &auth_rpt_timer, DOG_AUTH_RPT_TIME );

  /* Wait for NV to finish processing NV_READ_F command */
  (void) rex_wait( AUTH_NV_CMD_SIG );

  /* Check and return status. */

  if ((nv_cmd_buf.status != NV_DONE_S) &&
      (nv_cmd_buf.status != NV_NOTACTIVE_S))
  {
    ERR_FATAL("Bad NV read status %d", nv_cmd_buf.status, 0, 0);
  }
  return (nv_cmd_buf.status);
} /* auth_get_nv_item */

/*===========================================================================

FUNCTION AUTH_PUT_NV_ITEM

DESCRIPTION
  This function puts a specific item to NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  While this function is running all other task activities are
  suspended except for watchdog kicking, and until the NV item is
  written.

===========================================================================*/

void auth_put_nv_item
(
  nv_items_enum_type  item_code,       /* Item to put */
  nv_item_type        *item_ptr        /* Pointer where to get the item */
)
{

  static nv_cmd_type  nv_cmd_buf;   /* Command buffer to NV */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Prepare command buffer to NV. */

  nv_cmd_buf.cmd = NV_WRITE_F;
  nv_cmd_buf.tcb_ptr = &auth_tcb;
  nv_cmd_buf.sigs = AUTH_NV_CMD_SIG;
  nv_cmd_buf.done_q_ptr = NULL;              /* No buffer to return */
  nv_cmd_buf.item = item_code;
  nv_cmd_buf.data_ptr = item_ptr;

  (void) rex_clr_sigs(&auth_tcb, AUTH_NV_CMD_SIG);
  nv_cmd(&nv_cmd_buf);

  /* -----------------------------
  ** Kick watchdog and reset timer
  ** ----------------------------- */
  dog_report(DOG_AUTH_RPT);
  (void) rex_set_timer( &auth_rpt_timer, DOG_AUTH_RPT_TIME );

  /* Wait for NV to finish processing NV_WRITE_F command */
  (void) rex_wait( AUTH_NV_CMD_SIG );

  if (nv_cmd_buf.status != NV_DONE_S)
  {
    ERR_FATAL("Failed to write item to NV %d", item_code, 0, 0);
  }
} /* auth_put_nv_item */

/*===========================================================================

FUNCTION AUTH_VAR_INIT

DESCRIPTION
  This procedure initializes the authentication task variables after a
  powerup, NAM change, or A_Key change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void auth_var_init
(
  byte nam
    /* The nam active during this initialization */
)
{
  nv_item_type nv_item;
    /* Item to hold values retrieved from NV */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize boolean indicators */
  auth_saved_registers = FALSE;
  auth_ssd_update_in_progress = FALSE;
  auth_valid_CMEA_key = FALSE;
  auth_valid_PLCM = FALSE;

  /* Store NAM index */
  auth_stored_NAM_index = nam;

  /* Get value of COUNTsp from NV */
  nv_item.count.nam = auth_stored_NAM_index;
  if (auth_get_nv_item( NV_COUNT_I, &nv_item ) == NV_NOTACTIVE_S)
  {
    /* Write a zero in for the COUNTsp if no count has ever been used */
    nv_item.count.nam = auth_stored_NAM_index;
    nv_item.count.count = 0;
    auth_put_nv_item( NV_COUNT_I, &nv_item );
    auth_count = 0;
  }
  else
  {
    auth_count = nv_item.count.count;
  }
} /* auth_var_init() */
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION AUTH_SEND_UIM_COMMAND

DESCRIPTION
  This function sends a UIM command top the UIM server.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function waits for the response before returning to the calling
  function.

===========================================================================*/
void auth_send_uim_command
(
  uim_cmd_type *uim_cmd_ptr
)
{
  /* Indicate command completion is to be signaled:
   *    Do not signal the Auth task upon receipt of command
   *    Use no signal for receipt of command
   *    No "done" queue
   *    Status call-back function
   *    Always Report status
   */

  uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
  uim_cmd_ptr->hdr.cmd_hdr.sigs = 0;
  uim_cmd_ptr->hdr.cmd_hdr.done_q_ptr = &uim_free_q;
  uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;
  uim_cmd_ptr->hdr.protocol = UIM_CDMA;
  uim_cmd_ptr->hdr.slot = UIM_SLOT_AUTOMATIC;
  uim_cmd_ptr->hdr.rpt_function = auth_uim_response_callback;

  /* Send the command to the UIM server:
   *    Clear the "command done signal"
   *    Send the command
   *    Wait for the command to be done
   */
  (void) rex_clr_sigs( &auth_tcb, AUTH_UIM_CMD_RSP_SIG );

  /* Send the command to the UIM server. */
  uim_cmd ( uim_cmd_ptr );
  /* Wait for the response.  Process the watchdog, stop, and offline
     signals. */
  (void) auth_wait( AUTH_UIM_CMD_RSP_SIG );

} /* auth_send_uim_command */

/*===========================================================================

FUNCTION AUTH_UIM_RESPONSE_CALLBACK

DESCRIPTION
  This procedure initiates the processing of the response that has been recd
  from the RUIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the RUIM_CMD_RSP_SIG.

===========================================================================*/

LOCAL void auth_uim_response_callback ( uim_rpt_type *uim_rpt_buf_ptr )
{
  /* Copy the report into the local buffer. */
  auth_uim_rsp_buf = *uim_rpt_buf_ptr;

  /* set the command response signal */
  MSG_LOW(" Recd Command Response Signal",0,0,0);
  (void) rex_set_sigs(&auth_tcb, AUTH_UIM_CMD_RSP_SIG);

} /*  auth_uim_response_callback */

#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION AUTH_PROCESS_COMMAND

DESCRIPTION
  This procedure processes a command sent to the Authentication task. If
  the FEATURE_UIM_RUIM is defined, it sets the RUIM state variable and does
  not actually perform the AUTH computations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the command is AUTH_GENERATE_KEY_F, lots of CPU time will be taken
  to do this huge calculation.
  If FEATURE_UIM_RUIM is defined, it sets the RUIM state variable amongst others.

===========================================================================*/

LOCAL void auth_process_command
(
  auth_cmd_type *cmd_ptr
    /* Pointer to received command */
)
{
#if defined (FEATURE_AUTH) && (!defined(FEATURE_UIM_RUIM) || \
    defined(FEATURE_UIM_RUN_TIME_ENABLE))
  static dword authbs;
    /* Value computed internally during SSD Update, compared to BS version */
  nv_item_type nv_item;
    /* Item to hold values retrieved from NV */
  boolean return_status;
    /* Status to be returned for Finish SSD Update or Update A Key */
  dword auth_sig;
    /* Auth signature value returned from CAVE */
#endif /* FEATURE_AUTH && (!FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE) */
#ifdef FEATURE_UIM_RUIM
  uim_cmd_type *uim_cmd_ptr;
    /* UIM command pointer points to UIM command buffer for sending commands
       to the UIM server. */
  uim_rpt_type *uim_rpt_ptr = &auth_uim_rsp_buf;
    /* Pointer to UIM report */
#endif /* FEATURE_UIM_RUIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command to local buffer */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    if ((auth_init_status == AUTH_UNINITIALIZED) &&
        ((cmd_ptr->hdr.command != AUTH_INIT_F) &&
         (cmd_ptr->hdr.command != AUTH_UPDATE_A_KEY_F) &&
         (cmd_ptr->hdr.command != AUTH_RESYNC_F) &&
         (cmd_ptr->hdr.command != AUTH_RESET_F)))
    {
      ERR( "AUTH not initialized", 0, 0, 0);
      cmd_ptr->hdr.status = AUTH_INVALID_S;
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
        (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                            cmd_ptr->hdr.cmd_hdr.sigs );
      }
      return;
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

  switch (cmd_ptr->hdr.command)
  {
#ifdef FEATURE_AUTH
    case AUTH_INIT_F:
      {
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                              cmd_ptr->hdr.cmd_hdr.sigs );
        }
#ifdef FEATURE_UIM_RUIM
        /* Allow the RUIM to power off for AUTH. */
        (void) uim_power_control ( UIM_AUTH, FALSE );

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          auth_saved_registers = FALSE;
          auth_ssd_update_in_progress = FALSE;
          auth_valid_CMEA_key = FALSE;
          auth_valid_PLCM = FALSE;

          /* Store NAM index */
          auth_stored_NAM_index = cmd_ptr->init.nam;

          auth_init_status = AUTH_INIT_OK;

          /* Treat this command as a read from Call Count */
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
            uim_cmd_ptr->access_uim.num_bytes = 2;  /* One byte in size */
            uim_cmd_ptr->access_uim.offset = 1;     /* First byte */
            uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;
            uim_cmd_ptr->access_uim.data_ptr = (byte *)&(temp_auth_count);
            uim_cmd_ptr->access_uim.item = UIM_CDMA_CC;       /* EF is "Call Count" */
            uim_cmd_ptr->access_uim.access = UIM_READ_F;     /* "Read" command        */

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Determine if the command passed. */
            if (uim_rpt_ptr->rpt_status == UIM_PASS)
            {
              /* Copy the new call count to the call count buffer. */
              temp_auth_count = temp_auth_count >> RUIM_CC_SHIFT;
              auth_count = temp_auth_count & RUIM_CC_MASK;
            } /* end if - the command passed */
          }
          else /* No UIM command buffers on the free queue. */
          {
            ERR( "No free buffers on auth_free_q", 0, 0, 0 );
          } /* Get a UIM command buffer from the UIM free queue. */
          auth_init_status = AUTH_INIT_OK;
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          auth_var_init(cmd_ptr->init.nam);
          auth_init_status = CAVE_init( cmd_ptr->init.nam );
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
      } /* end case - AUTH_INIT_F */
      break;

    case AUTH_SSD_UPDATE_F:
      {
        /* Clear the T64 timer to handle the case where an SSD Update
        is in progress and an Idle HO occurs.  The new BS will initiate
        SSD Update and we don't want the old timer expiration to abort
        the new attempt */
        (void) rex_clr_timer( &auth_ssd_timer );
        (void) rex_clr_sigs( &auth_tcb, AUTH_SSD_TIMER_SIG );

        /* Inform the client that the command is being processed. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                              cmd_ptr->hdr.cmd_hdr.sigs );
        }
        /* Indicate the SSD update has started */
        auth_ssd_update_in_progress = TRUE;

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            /* Build a SSD update command. */
            uim_cmd_ptr->hdr.command = UIM_SSD_UPDATE_F;
            uim_cmd_ptr->ssd_update.randssd[0] = cmd_ptr->ssd.randssd[0];
            uim_cmd_ptr->ssd_update.randssd[1] = cmd_ptr->ssd.randssd[1];
            /* Include the process control bit if the card has
               generated new A-Key */
            if (a_key_temp_valid)
            {
                cmd_ptr->ssd.process_control |= RUIM_COMMIT_NEW_SSD;
            }
            /* Fill in the rest of the command parameters. */
            uim_cmd_ptr->ssd_update.process_control = cmd_ptr->ssd.process_control;

#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
            uim_cmd_ptr->ssd_update.esn = tmc_get_stored_esn_me();
#else
            uim_cmd_ptr->ssd_update.esn = cmd_ptr->ssd.esn;
#endif

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Do NOT check the response.  There is no use for a response
               for this command. */
          }
          else /* No UIM command buffers on the free queue. */
          {
            ERR( "No free buffers on auth_free_q", 0, 0, 0 );
          } /* Get a UIM command buffer from the UIM free queue. */
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          CAVE_generate_SSD( cmd_ptr->ssd.randssd );
          authbs = CAVE_auth_signature( cmd_ptr->ssd.randbs,
                     cmd_ptr->ssd.auth_data, TRUE, FALSE
#ifdef FEATURE_OTASP_OTAPA
                     , FALSE  // Not for SPASM
#endif /* FEATURE_OTASP_OTAPA */
                           );
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

        MSG_MED( "SSD update command recd", 0, 0, 0 );
      } /* end case - AUTH_SSD_UPDATE_F */
      break;

    case AUTH_FINISH_SSD_UPDATE_F:
      {
        /* Cancel the SSD timer now that the FINISH_SSD command has
           been received. */
        (void) rex_clr_sigs( &auth_tcb, AUTH_SSD_TIMER_SIG );
        (void) rex_clr_timer( &auth_ssd_timer );

        /* Determine if the SSD update has started. */
        if (!auth_ssd_update_in_progress)
        {
          MSG_MED( "No SSD Update in progress", 0, 0, 0 );
          cmd_ptr->hdr.status = AUTH_INVALID_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* We do not need to process this command and the buffer
             needs to be released */
        }
        else /* We can proceed and finish the SSD Update */
        {
          /* Inform the client the command was received. */
          cmd_ptr->hdr.status = AUTH_RECEIVED_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* Indicate the SSD is finished. */
          auth_ssd_update_in_progress = FALSE;
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
          {
            /* Retrieve a UIM command buffer from the queue. */
            if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
            {
              /* Build a confirm SSD command. */
              uim_cmd_ptr->confirm_ssd.hdr.command = UIM_CONFIRM_SSD_F;
              uim_cmd_ptr->confirm_ssd.authbs = cmd_ptr->f_ssd.authbs;

              /* Send the command and wait for the response. */
              auth_send_uim_command(uim_cmd_ptr);

              /* -------------------------------------
              ** Now, if required, report back whether
              ** or not SSD update was successful.
              ** ------------------------------------- */
              if (cmd_ptr->f_ssd.rpt_function != NULL)
              {
                /* Retrieve the UIM response from the UIM report buffer. */
                auth_rpt_buf.rpt_type = AUTH_FINISH_SSD_R;
                auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

                /* Copy the UIM response to the report buffer. */
                auth_rpt_buf.rpt.ssd_update_ok = uim_rpt_ptr->rpt.ssd_update_ok;

                /* Fill in necessary fields of command block */
                auth_rpt_buf.rpt_hdr.task_ptr = NULL;

                /* Report to requesting task */
                (*(cmd_ptr->f_ssd.rpt_function))( &auth_rpt_buf );
                MSG_MED( "Reporting SSD update complete", 0, 0, 0 );
              }
            }
            else /* No UIM command buffers on the free queue. */
            {
              ERR( "No free buffers on auth_free_q", 0, 0, 0 );
            } /* Get a UIM command buffer from the UIM free queue. */
            /* The SSD is no longer in progress.  Allow the UIM to be powered 
               down */
            (void) uim_power_control ( UIM_AUTH, FALSE );
          }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
          {
            MSG_MED( "Received authbs %ld", cmd_ptr->f_ssd.authbs, 0, 0 );

            /* If the base station value is correct, report SSD Update success */
            if (authbs == cmd_ptr->f_ssd.authbs)
            {
              return_status = TRUE;
            }
            else
            {
              return_status = FALSE;
            }

            /* -------------------------------------
            ** Now, if required, report back whether
            ** or not SSD update was successful.
            ** ------------------------------------- */
            if (cmd_ptr->f_ssd.rpt_function != NULL)
            {
              auth_rpt_buf.rpt_type = AUTH_FINISH_SSD_R;
              auth_rpt_buf.rpt.ssd_update_ok = return_status;

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Queue report for requesting task */
              (*(cmd_ptr->f_ssd.rpt_function))( &auth_rpt_buf );
              MSG_MED( "Reporting SSD update complete", 0, 0, 0 );
            }

            /* ----------------------------------------------------------------
            ** Tell CAVE to update the SSD (Write it to NV) after reporting
            ** the success.  There is a 750 ms timeout for analog voice channel
            ** operation, and we take too long otherwise.  this is OK since the
            ** NV is guaranteed to write properly or else cause a fatal error.
            ** The NV writes take almost two seconds.
            ** ---------------------------------------------------------------- */

            /* ------------------------------------------------------------------
            ** If a_key_temp_valid is true, we do not want to write SSD to NV.
            ** The new_ssd_valid flag is set to true to indicate that ssd has
            ** been generated from the new A-Key. Also if we got a commit request,
            ** by looking at this flag we can decide if we need to commit the ssd
            ** ---------------------------------------------------------------- */

            if (a_key_temp_valid) {
              /* change the flag to indicate that new A-Key has been used
              ** to generate the SSDs  */
              new_ssd_valid = TRUE;
            }
            else 
            {
              if (return_status == TRUE)
              {
                (void) CAVE_update_SSD();
              }
            }
          }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
        }
        /* Send a indicating what was done. */
        MSG_MED( "Received SSD update command ", 0, 0, 0 );
      } /* end case - AUTH_FINISH_SSD_UPDATE_F */
      break;

    case AUTH_SIGNATURE_F:
      {
        MSG_HIGH("Received AUTH_SIGNATURE_F command", 0, 0, 0);
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
           (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                             cmd_ptr->hdr.cmd_hdr.sigs );
        }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            uim_cmd_ptr->run_cave.hdr.command = UIM_RUN_CAVE_F;
            uim_cmd_ptr->run_cave.rand_type = cmd_ptr->sig.rand_type;
            uim_cmd_ptr->run_cave.rand_chal = cmd_ptr->sig.rand_chal;
            uim_cmd_ptr->run_cave.dig_len = cmd_ptr->sig.dig_len;
            memcpy ( uim_cmd_ptr->run_cave.digits, 
                     cmd_ptr->sig.digits, 
                     RUIM_RUN_CAVE_DIGITS);
            uim_cmd_ptr->run_cave.process_control = cmd_ptr->sig.process_control;
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
			uim_cmd_ptr->run_cave.esn[0] = tmc_get_stored_esn_me();
#else            
			uim_cmd_ptr->run_cave.esn[0] = cmd_ptr->sig.esn;
#endif
            uim_cmd_ptr->run_cave.esn[1] = 0; /* Zero out the MSbits of the ESN 
                                                 since the command takes 7 bytes 
                                                 and our ESN is only 4 bytes.*/

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Determine if the run CAVE command passed. */
            if (uim_rpt_ptr->rpt_status == UIM_PASS)
            {
              /* Determine whether the registers were saved based on the
                 command control. */
              if (cmd_ptr->sig.process_control & RUIM_SAVE_REGS_ON)
              {
                 auth_saved_registers = TRUE;
              }
              else
              {
                 auth_saved_registers = FALSE;
              }
            }
            else /* The run CAVE command failed. */
            {
              /* The registers were NOT saved. */
              auth_saved_registers = FALSE;
            }

            /* -------------------------------------
            ** Now, if required, send the signature 
            ** back 
            ** ------------------------------------- */
            if (cmd_ptr->sig.rpt_function != NULL)
            {
              /* Assign the report type */
              auth_rpt_buf.rpt_type = AUTH_SIGNATURE_R;

              /* Copy the UIM response to the report buffer. */
              auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;
              auth_rpt_buf.rpt.auth = uim_rpt_ptr->rpt.run_cave.auth;

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* Report to requesting task */
              (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
              MSG_MED( "Reporting auth Signature %ld", 
                                          auth_rpt_buf.rpt.auth, 0, 0 );
            } /* end if - the client supplied a reportback function. */
          }
          else /* No UIM command buffers on the free queue. */
          {
            ERR( "No free buffers on auth_free_q", 0, 0, 0 );
          } /* Get a UIM command buffer from the UIM free queue. */
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          /* indicate that the new ssd values need to be used if they
          ** have been generated  using the new a-key*/
          if (new_ssd_valid)
          {
             auth_sig = CAVE_auth_signature( cmd_ptr->sig.rand_chal,
                cmd_ptr->sig.auth_data, TRUE, cmd_ptr->sig.save_reg
#ifdef FEATURE_OTASP_OTAPA
            , FALSE  // Not for SPASM
#endif
            );

             /* set to TRUE since new ssd values have been used to compute
                AUTHR */
             used_new_ssd = TRUE;
          }
          else
          {
             auth_sig = CAVE_auth_signature( cmd_ptr->sig.rand_chal,
                cmd_ptr->sig.auth_data, FALSE, cmd_ptr->sig.save_reg
#ifdef FEATURE_OTASP_OTAPA
            , FALSE  // Not for SPASM
#endif
            );

            used_new_ssd = FALSE;
          }

          /* ------------------------------------------------------------
          ** If we saved the register values, set saved_registers to TRUE
          ** to indicate we are ready to generate CMEA key and VPM.
          ** ------------------------------------------------------------ */
          if (cmd_ptr->sig.save_reg)
          {
             auth_saved_registers = TRUE;
          }

          /* Report back auth signature, if requested */
          if (cmd_ptr->sig.rpt_function != NULL)
          {
            /* Retrieve the UIM response from the UIM report buffer. */
             auth_rpt_buf.rpt_type = AUTH_SIGNATURE_R;
             auth_rpt_buf.rpt.auth = auth_sig;

             /* Fill in necessary fields of command block */
             auth_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* --------------------------------
             ** Queue report for requesting task
             ** -------------------------------- */
             (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
             MSG_MED( "Reporting auth %ld", auth_rpt_buf.rpt.auth, 0, 0 );
          }
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

        MSG_MED( "Received Auth Signature command ", 0, 0, 0 );
      } /* end case - AUTH_SIGNATURE_F */
      break;

    case AUTH_GENERATE_KEY_F:
      {
        /* Determine if the auth registers were saved from the last
           RUN CAVE command. */
        if (!auth_saved_registers)
        {
          /* The registers were NOT saved.  Generate keys will fail.
             Inform the client the command failed. */
          ERR( "Need to call auth_signature first", 0, 0, 0);
          cmd_ptr->hdr.status = AUTH_INVALID_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }
          /* Indicate the keys are no longer valid. */
          auth_valid_CMEA_key = FALSE;
          auth_valid_PLCM = FALSE;
        }
        else
        {
          /* Inform the client that the command was received */
          cmd_ptr->hdr.status = AUTH_RECEIVED_S;
          if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
          {
            (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
          }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
          {
            /* Retrieve a UIM command buffer from the queue. */
            if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
            {
              uim_cmd_ptr->hdr.command = UIM_GENERATE_KEYS_F;
              uim_cmd_ptr->gen_keys.vpm_first_octet = cmd_ptr->key.vpm_first_octet;
              uim_cmd_ptr->gen_keys.vpm_last_octet = cmd_ptr->key.vpm_last_octet;

              /* Send the command and wait for the response. */
              auth_send_uim_command(uim_cmd_ptr);

              /* Determine if the run CAVE command passed. */
              if (uim_rpt_ptr->rpt_status == UIM_PASS)
              {
                /* With the new interface, we always compute the VPM */
                /* Indocate that the keys are now valid. */
                auth_valid_PLCM = TRUE;
                auth_valid_CMEA_key = TRUE;

                /* Update CAVE with CMEA and VPM keys */
                CAVE_update_keys(uim_rpt_ptr->rpt.generate_keys.data);
              }
              else /* The run CAVE command failed. */
              {
                /* The command failed, so the keys are NOT valid. */
                auth_valid_PLCM = FALSE;
                auth_valid_CMEA_key = FALSE;
              } /* end if - the command passed */

              /* -------------------------------------
              ** Now, if required, send the signature 
              ** back 
              ** ------------------------------------- */
              if (cmd_ptr->key.rpt_function != NULL)
              {
                /* Assign the report type */
                auth_rpt_buf.rpt_type = AUTH_GENERATE_KEY_R;

                /* Retrieve the UIM response from the UIM report buffer. */
                auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

                /* Fill in necessary fields of command block */
                auth_rpt_buf.rpt_hdr.task_ptr = NULL;

                /* Report to requesting task */
                (*(cmd_ptr->key.rpt_function))( &auth_rpt_buf );
                MSG_MED( "Reporting keys generated", 0, 0, 0 );
              } /* end if - the client supplied a reportback function. */
            }
            else /* No UIM command buffers on the free queue. */
            {
              ERR( "No free buffers on auth_free_q", 0, 0, 0 );
            } /* Get a UIM command buffer from the UIM free queue. */
          }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
          {
            /* Full key generation takes 100+ms of CPU time */
            /* It sets auth_valid_CMEA_key as soon as the key is valid */
            /* Variable used_new_ssd is sent as a parameter */
            if (used_new_ssd)
            {
              CAVE_generate_key( cmd_ptr->key.do_vpm, &auth_valid_CMEA_key, TRUE );
              MSG_HIGH( "Keys generated with new SSD", 0, 0, 0);
            }
            else
            {
              CAVE_generate_key( cmd_ptr->key.do_vpm, &auth_valid_CMEA_key, FALSE );
              MSG_HIGH( "Keys generated", 0, 0, 0);
            }

            /* If we were told to calculate the VPM, then our PLCM is valid */
            auth_valid_PLCM = cmd_ptr->key.do_vpm;

            /* Report back that keys are generated, if requested */
            if (cmd_ptr->key.rpt_function != NULL)
            {
              auth_rpt_buf.rpt_type = AUTH_GENERATE_KEY_R;

              /* Fill in necessary fields of command block */
              auth_rpt_buf.rpt_hdr.task_ptr = NULL;

              /* --------------------------------
              ** Queue report for requesting task
              ** -------------------------------- */
              (*(cmd_ptr->key.rpt_function))( &auth_rpt_buf );
              MSG_MED( "Reporting keys generated", 0, 0, 0 );
            }
          }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
        } /* end if - the auth registers were saved. */
      } /* end case - AUTH_GENERATE_KEY_F */
      break;

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    
    case AUTH_UPDATE_A_KEY_F:
      {

        cmd_ptr->hdr.status = AUTH_RECEIVED_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

        if (CAVE_update_A_key( cmd_ptr->a_key.a_key,
                              cmd_ptr->a_key.nam ))
        {
          /* Re-initialize everything following A_key change */
          auth_var_init( cmd_ptr->a_key.nam );
          auth_init_status = CAVE_init( cmd_ptr->a_key.nam );
          return_status = TRUE;
        }
        else
        {
          return_status = FALSE;
        }

        /* ------------------------------------------
        ** Now if a report was requested, report back
        ** whether or not A-KEY update was successful.
        ** ------------------------------------------ */
        if (cmd_ptr->a_key.rpt_function != NULL)
        {
          auth_rpt_buf.rpt_type = AUTH_A_KEY_R;
          auth_rpt_buf.rpt.a_key_update_ok = return_status;

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Queue report for requesting task */
          (*(cmd_ptr->a_key.rpt_function))( &auth_rpt_buf );
          MSG_MED( "Reporting A-key update complete", 0, 0, 0 );
        }
      } /* end case - AUTH_UPDATE_A_KEY_F */
      break;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    case AUTH_INC_COUNT_F:
      {
        /* Inform the client that the command was received */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        /* Determine if the R-UIM is available for this NAM */
        if (nv_rtre_use_ruim_for_nam(auth_stored_NAM_index))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
        {
          /* Treat this command as a write to Call Count */
          /* Use a temporary buffer to hold the call count. */
          temp_auth_count = (auth_count + 1) & RUIM_CC_MASK;
          temp_auth_count = temp_auth_count << RUIM_CC_SHIFT;
          /* Retrieve a UIM command buffer from the queue. */
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
            uim_cmd_ptr->access_uim.num_bytes = 2;  /* One byte in size */
            uim_cmd_ptr->access_uim.offset = 1;     /* Second byte */
            uim_cmd_ptr->access_uim.rec_mode = UIM_PREVIOUS;
            uim_cmd_ptr->access_uim.data_ptr = (byte *)&(temp_auth_count);
            uim_cmd_ptr->access_uim.item = UIM_CDMA_CC;       /* EF is "Call Count" */
            uim_cmd_ptr->access_uim.access = UIM_WRITE_F;     /* "Write" command        */

            /* Send the command and wait for the response. */
            auth_send_uim_command(uim_cmd_ptr);

            /* Determine if the run CAVE command passed. */
            if (uim_rpt_ptr->rpt_status == UIM_PASS)
            {
              /* Copy the new call count to the call count buffer. */
              temp_auth_count = temp_auth_count >> RUIM_CC_SHIFT;
              auth_count = temp_auth_count & RUIM_CC_MASK;
            } /* end if - the command passed */
          }
          else /* No UIM command buffers on the free queue. */
          {
            ERR( "No free buffers on auth_free_q", 0, 0, 0 );
          } /* Get a UIM command buffer from the UIM free queue. */
        }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
        else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
        {
          auth_count++;
          /* Put new COUNTsp in NV */
          nv_item.count.nam = auth_stored_NAM_index;
          nv_item.count.count = auth_count;
          auth_put_nv_item( NV_COUNT_I, &nv_item );
        }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
      } /* end case - AUTH_INC_COUNT_F */
      break;

    case AUTH_START_TIMER_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

        /* Set timer in case the Base Station leaves us hanging */
        MSG_MED( "Setting SSD timer", 0, 0, 0 );
        (void) rex_set_timer( &auth_ssd_timer, CAI_T64M );
      } /* end case - AUTH_START_TIMER_F */
      break;

    case AUTH_RESET_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }

        /* -------------------------------------------------------------
        ** Initialize boolean indicators.  This allows us to detect when
        ** operations are done out of order i.e. a key generation before
        ** keys are saved or an encryption request before keys are
        ** generated.
        ** ------------------------------------------------------------- */
        auth_saved_registers = FALSE;
        auth_valid_CMEA_key = FALSE;
        auth_valid_PLCM = FALSE;
        MSG_MED( "AUTH state reset", 0, 0, 0 );
      } /* end case - AUTH_RESET_F */
      break;

    case AUTH_RESYNC_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                cmd_ptr->hdr.cmd_hdr.sigs );
        }
        if (cmd_ptr->resync.rpt_function != NULL)
        {
          auth_rpt_buf.rpt_type = AUTH_RESYNC_R;

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* --------------------------------
          ** Queue report for requesting task
          ** -------------------------------- */
          (*(cmd_ptr->resync.rpt_function))( &auth_rpt_buf );
          MSG_MED( "AUTH synchronized", 0, 0, 0 );
        }
      } /* end case - AUTH_RESYNC_F */
      break;

#ifdef FEATURE_UIM_RUIM
    case AUTH_BS_CHAL_F:
      {
        /* We do not want the RUIM to power down as the SSD update is in
           progress and RANDBS is in temporary memory in the SIM */
        (void) uim_power_control ( UIM_AUTH, TRUE );
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
           (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                          cmd_ptr->hdr.cmd_hdr.sigs );
        }
        /* Retrieve a UIM command buffer from the queue. */
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->hdr.command = UIM_BS_CHAL_F;
          uim_cmd_ptr->bs_chal.randseed = cmd_ptr->chal.randseed;

          /* Send the command and wait for the response. */
          auth_send_uim_command(uim_cmd_ptr);

          /* Report back that Access was completed, if requested */
          if (cmd_ptr->chal.rpt_function != NULL)
          {
             auth_rpt_buf.rpt_type = AUTH_BS_CHAL_R;

             /* Copy the UIM response to the report buffer. */
             auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;
             auth_rpt_buf.rpt.randbs = uim_rpt_ptr->rpt.bs_chal.randbs;

             /* Fill in necessary fields of command block */
             auth_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to the Requesting task */
             (*(cmd_ptr->chal.rpt_function))( &auth_rpt_buf );
             MSG_MED( "Reporting RUIM Access", 0, 0, 0 );

          } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        }
        else /* No UIM command buffers on the free queue. */
        {
          ERR( "No free buffers on uim_free_q", 0, 0, 0 );
        } /* Get a UIM command buffer from the UIM free queue. */
        MSG_MED( "Received Base station Challenge command ", 0, 0, 0 );
      } /* end case - AUTH_BS_CHAL_F */
    break;
#endif /* FEATURE_UIM_RUIM */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    case AUTH_UPDATE_A_KEY_TEMP_F:

      cmd_ptr->hdr.status = AUTH_RECEIVED_S;
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                        cmd_ptr->hdr.cmd_hdr.sigs );
      }

      /* copy the contents of temp_a_key in the command to a_key_temp */
      qw_equ(a_key_temp, cmd_ptr->temp.a_key);

      a_key_temp_valid = TRUE;

      if (cmd_ptr->temp.rpt_function != NULL)
      {
        auth_rpt_buf.rpt_type = AUTH_UPDATE_A_KEY_TEMP_R;

        /* Fill in necessary fields of command block */
        auth_rpt_buf.rpt_hdr.task_ptr = NULL;

        /* --------------------------------
        ** Queue report for requesting task
        ** -------------------------------- */
        (*(cmd_ptr->temp.rpt_function))( &auth_rpt_buf );
        MSG_MED( "Updating A_KEY_TEMP", 0, 0, 0 );
      }
      break;

    case AUTH_COMMIT_KEYS_F:

      cmd_ptr->hdr.status = AUTH_RECEIVED_S;

      /* update the SSD */
      if (new_ssd_valid)
         (void) CAVE_update_SSD();

      return_status = CAVE_update_A_key_683( a_key_temp,
                              cmd_ptr->commit_keys.nam_index );

      /* We use the report in otasp to check the result of A-Key commit */
      if (cmd_ptr->commit_keys.rpt_function != NULL)
      {
          auth_rpt_buf.rpt_type = AUTH_COMMIT_KEYS_R;

          /* Fill in necessary fields of command block */
          auth_rpt_buf.rpt_hdr.task_ptr = NULL;

          auth_rpt_buf.rpt.commit_keys_ok = return_status;

          /* --------------------------------
          ** Queue report for requesting task
          ** -------------------------------- */
          (*(cmd_ptr->commit_keys.rpt_function))( &auth_rpt_buf );
          MSG_MED( "Reporting keys committed", 0, 0, 0 );
      }

      /* If specified in request then signal the requesting task. */
      /* signal MC that committing has been done */
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
         (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                               cmd_ptr->hdr.cmd_hdr.sigs );
      }

      break;

#ifdef FEATURE_OTASP_OTAPA
    case AUTH_SIGNATURE_SPASM_F:

      MSG_MED("Process AUTH_SIG_SPASM_F", 0,0,0);

      cmd_ptr->hdr.status = AUTH_RECEIVED_S;
      if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
      {
         (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                           cmd_ptr->hdr.cmd_hdr.sigs );
      }

      /* ------------------------------------------------------------
      ** We use the SSD_A from NV, so use_new_ssd is set to FALSE.
      ** Save_registers parameter is FALSE. See IS-683A 3.3.7.
      ** ------------------------------------------------------------ */
      auth_sig = CAVE_auth_signature( cmd_ptr->sig.rand_chal,
            cmd_ptr->sig.auth_data, FALSE, FALSE, TRUE );

      MSG_MED("rand %lx, auth_data %lx",
              cmd_ptr->sig.rand_chal, cmd_ptr->sig.auth_data, 0);

      MSG_MED("new_ssd FALSE, save_reg FALSE, val_spasm TRUE",0,0,0);

      /* Report back auth signature, if requested */
      if (cmd_ptr->sig.rpt_function != NULL)
      {
         auth_rpt_buf.rpt_type = AUTH_SIGNATURE_SPASM_R;
         auth_rpt_buf.rpt.auth = auth_sig;

         /* Fill in necessary fields of command block */
         auth_rpt_buf.rpt_hdr.task_ptr = NULL;

         /* --------------------------------
         ** Queue report for requesting task
         ** -------------------------------- */
         (*(cmd_ptr->sig.rpt_function))( &auth_rpt_buf );
         MSG_MED( "Reporting auth %ld", auth_rpt_buf.rpt.auth, 0, 0 );
      }

      break;
#endif /* FEATURE_OTASP_OTAPA */

#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM
    case AUTH_ACCESS_RUIM_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                          cmd_ptr->hdr.cmd_hdr.sigs );
        }
        /* Retrieve a UIM command buffer from the queue. */
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
          uim_cmd_ptr->access_uim.item = cmd_ptr->access_ruim.item;
          uim_cmd_ptr->access_uim.access = cmd_ptr->access_ruim.access;
          uim_cmd_ptr->access_uim.rec_mode = UIM_ABSOLUTE;
          uim_cmd_ptr->access_uim.num_bytes = cmd_ptr->access_ruim.num_bytes;
          uim_cmd_ptr->access_uim.offset = cmd_ptr->access_ruim.offset;
          uim_cmd_ptr->access_uim.data_ptr = cmd_ptr->access_ruim.data_ptr;

          /* Send the command and wait for the response. */
          auth_send_uim_command(uim_cmd_ptr);

          /* Report back that Access was completed, if requested */
          if (cmd_ptr->access_ruim.rpt_function != NULL)
          {
             auth_rpt_buf.rpt_type = AUTH_ACCESS_RUIM_R;

             /* Copy the UIM response to the report buffer. */
             auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

             /* Fill in the number of bytes in the response that we got
                from RUIM into the NV command */
             cmd_ptr->access_ruim.num_bytes_rsp =
               uim_cmd_ptr->access_uim.num_bytes_rsp;

             /* The data is already copied to the client buffer as a result
                of copying the data_ptr from the client auth command to the
                UIM command above.  The UIM server copies the data to the
                client buffer. */

             /* Fill in necessary fields of command block */
             auth_rpt_buf.rpt_hdr.task_ptr = NULL;

             /* Report to the Requesting task */
             (*(cmd_ptr->access_ruim.rpt_function))( &auth_rpt_buf );
             MSG_MED( "Reporting RUIM Access", 0, 0, 0 );

          } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        }
        else /* No UIM command buffers on the free queue. */
        {
          ERR( "No free buffers on auth_free_q", 0, 0, 0 );
        } /* Get a UIM command buffer from the UIM free queue. */
        MSG_MED( "Received RUIM Access command ", 0, 0, 0 );
      } /* end case - AUTH_ACCESS_RUIM_F */
      break;

    case AUTH_RUIM_STATUS_F:
      {
        /* Inform the client that the command has been received. */
        cmd_ptr->hdr.status = AUTH_RECEIVED_S;
        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
           (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
              cmd_ptr->hdr.cmd_hdr.sigs );
        }
        /* Retrieve a UIM command buffer from the queue. */
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->status.hdr.command = UIM_STATUS_F;

          /* Send the command and wait for the response. */
          auth_send_uim_command(uim_cmd_ptr);

          /* Report back that Access was completed, if requested */
          if (cmd_ptr->status.rpt_function != NULL)
          {
            auth_rpt_buf.rpt_type = AUTH_RUIM_STATUS_R;

            /* Copy the UIM response to the report buffer. */
            auth_rpt_buf.rpt_status = uim_rpt_ptr->rpt_status;

            /* Fill in necessary fields of command block */
            auth_rpt_buf.rpt_hdr.task_ptr = NULL;

            /* Report to the Requesting task */
            (*(cmd_ptr->status.rpt_function))( &auth_rpt_buf );
            MSG_MED( "Reporting RUIM Access", 0, 0, 0 );

          } /* (cmd_ptr->access_ruim.rpt_function != NULL) */
        }
        else /* No UIM command buffers on the free queue. */
        {
          ERR( "No free buffers on auth_free_q", 0, 0, 0 );
        } /* Get a UIM command buffer from the UIM free queue. */
        MSG_MED( "Received RUIM STATUS command ", 0, 0, 0 );
      } /* end case - AUTH_RUIM_STATUS_F */
      break;

#endif /* FEATURE_UIM_RUIM */

    default:
      ERR_FATAL( "Unknown command to AUTH %d", cmd_ptr->hdr.command, 0, 0);
    break;

  } /* end switch (command_type) */

} /* auth_process_command() */

#if defined (FEATURE_AUTH) && (!defined(FEATURE_UIM_RUIM) || \
    defined (FEATURE_UIM_RUN_TIME_ENABLE))
/*===========================================================================

FUNCTION AUTH_VALIDATE_A_KEY

DESCRIPTION
  This procedure tests if the given A key is valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the A key is valid
  FALSE if the A key is invalid

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_validate_a_key
(
  byte a_key[AUTH_A_KEY_DIGITS]
    /* The A key to be validated */
)
{
  if (CAVE_validate_A_key( a_key ))
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
} /* auth_validate_a_key() */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION AUTH_PRIVATE_LCM_AVAILABLE

DESCRIPTION
  This procedure checks the private lcm to make sure it can be activated.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the new lcm is available, FALSE othewise.

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_private_lcm_available( void )
{
  boolean plcm_available;
    /* Flag if the private long code mask is available */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (auth_init_status == AUTH_UNINITIALIZED)
  {
    MSG_HIGH( "Need to initialize AUTH", 0, 0, 0 );
    plcm_available = FALSE;
  }
  else if (!auth_valid_PLCM)
  {
    ERR( "Need to generate plcm first", 0, 0, 0);
    plcm_available = FALSE;
  }
  else
  {
    plcm_available = TRUE;
  }
  MSG_MED("PLCM is ready? %d",plcm_available,0,0);

  return (plcm_available);
} /* auth_private_lcm_available() */

/*===========================================================================

FUNCTION AUTH_GET_PRIVATE_LCM

DESCRIPTION
  This procedure gets the private lcm.

DEPENDENCIES
  None

RETURN VALUE
  PLCM .

SIDE EFFECTS
  None

===========================================================================*/
void auth_get_private_lcm( qword qplcm )
{
  ulpn_type plcm;
    /* Private long code mask returned by CAVE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  plcm = CAVE_private_lcm();
  /* Now set the new long code mask */
  qw_set( qplcm, (dword) plcm.high, plcm.low);
} /* auth_get_private_lcm */

/*===========================================================================

FUNCTION AUTH_GET_COUNT

DESCRIPTION
  This procedure retrieves the current value of COUNTsp.

DEPENDENCIES
  None

RETURN VALUE
  Returns COUNTsp, or zero if no value is available.

SIDE EFFECTS
  None

===========================================================================*/
byte auth_get_count( void )
{
  return (auth_count);
} /* auth_get_count() */

#endif /* FEATURE_AUTH */

#ifdef FEATURE_UIM_RUIM

#if defined (FEATURE_AUTH) && defined (FEATURE_AUTH_DIGITS)
/*===========================================================================

FUNCTION AUTH_CALCULATE_DIGITS

DESCRIPTION
  This function packs the dialed digits that need to be sent to the RUIM. If
  the number of digits is less than 6, the leading digits are zero padded.
  A max of 6 digits are packed in a 3 byte array.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed digits

SIDE EFFECTS
  None

===========================================================================*/
void auth_calculate_digits
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode,
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
  byte output_digits[]
    /* output array */
)
{
  byte last_digit;
    /* Index of last dialed digit in cdma.dial.digits array */
  byte num_digits;
    /* The number of digits to be used in auth_data */
  byte i;
    /* Index into cdma.dial.digits array */
  byte converted_digit;
    /* Digit converted to 4 bit DTMF code */
  byte j;
    /* Index into the output array */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  last_digit = array_len - 1;

  /* compute how many digits we are to use */
  num_digits = (array_len < 6) ? array_len : 6;

 /* initialize the digit array */
  for (j = 0; j < RUIM_RUN_CAVE_DIGITS; j++ )
  {
     output_digits[j]  = 0;
  }
  j = 2;
  /* j is left to 2 as we start filling the array from the end */

  /* Then replace however much is necessary with supplied digits */
  for (i=0; i < num_digits; i++)
  {
    /* Get the next digit, which can be ascii or DTMF already */
    converted_digit = digits[last_digit - i];

    /* If digit is not DTMF, convert it to DTMF as per 6.3.12.1.6 */
    if (digit_mode)
    {
      /* Digits, star, and pound are encoded as real DTMF codes */
      if (converted_digit == '*')
      {
        converted_digit = AUTH_DIAL_STAR;
      }
      else if (converted_digit == '#')
      {
        converted_digit = AUTH_DIAL_PND;
      }
      else if (converted_digit == '0')
      {
        converted_digit = AUTH_DIAL_0;
      }
      else if (converted_digit >= '1' && converted_digit <= '9')
      {
        converted_digit -= '0';
      }
      else
      {
        /* All others are encoded as DTMF version of last decimal digit
         * of ASCII character code.  That is, the ASCII code modulo 10.
         */
        converted_digit %= 10;
        if (converted_digit == 0)
        {
          /* A zero is encoded as '1010' */
          converted_digit = AUTH_DIAL_0;
        }
      }
    }  /* digit mode */
    if ((i+1) % 2 == 0)
    {
       output_digits[j--] |= ((converted_digit & 0x0FL) << 4);
    }
    else
    {
       output_digits[j] = (converted_digit & 0x0FL);
    }
  }  /* for loop */
} /* auth_calculate_digits () */
#endif /* FEATURE_AUTH && FEATURE_AUTH_DIGITS */
#endif /*  FEATURE_UIM_RUIM */

#ifdef FEATURE_AUTH
/*===========================================================================

FUNCTION AUTH_ENCRYPT_INFO_RECS

DESCRIPTION
  This procedure encrypts type specific fields in reverse traffic channel
  message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_info_recs
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word rec_start_pos,
    /* Start position of the info record after num_rec field */
  byte num_recs
    /* Number of info records to be processed */
)
{

  byte msg_buf[CAI_REV_TC_MAX_MSG_BYTES];
    /* Buffer to hold copy of message body */
  word crypt_pos;
    /* Position of end of message to encrypt */
  word msg_pos;
    /* Pointer into packed external message */
  word num_bytes;
    /* Number of bytes in flash with info message */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i, j;
    /* Loop counters */
  boolean bytes_added = FALSE;
    /* Flag if bytes have been added to the message */
  byte *buf_index = buf_ptr;
    /* Index into packed external message body, used to copy it */
  byte rec_type;
    /* The type of an information record in this message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Pay the price to copy the entire packed message body */
  num_bytes = (*len_ptr + AUTH_EIGHT_BITS - 1) / AUTH_EIGHT_BITS;
  for (i = 0; i < num_bytes; i++)
  {
    msg_buf[i] = *buf_index;
    buf_index++;
  }

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  crypt_pos = msg_pos = rec_start_pos;

  for (i = 0; i < num_recs; i++)
  {
    if (bytes_added)
    {
      /* We need to copy the record type to the encrypted message */
      rec_type = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, crypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    crypt_pos += AUTH_RECORD_TYPE_SIZE;

    num_data_bytes = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_added)
    {
      /* We need to copy the record length to the encrypted message */
      b_packb( num_data_bytes, buf_ptr, crypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    crypt_pos += AUTH_RECORD_LEN_SIZE;

    /* Unpack the type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( msg_buf, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    if (num_data_bytes == 1)
    {
      /* Add an extra zero byte */
      bytes_added = TRUE;
      (*len_ptr) += AUTH_EIGHT_BITS;
      num_data_bytes = 2;
      auth_temp_buf[1] = 0;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < num_data_bytes; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, crypt_pos, AUTH_EIGHT_BITS );
      crypt_pos += AUTH_EIGHT_BITS;
    }
  }
} /* auth_encrypt_info_recs() */ /*lint !e715 */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_FLASH

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel Flash with
  Information message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_flash
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{

  byte msg_buf[CAI_REV_TC_MAX_MSG_BYTES];
    /* Buffer to hold copy of message body */
  word crypt_pos;
    /* Position of end of message to encrypt */
  word msg_pos;
    /* Pointer into packed external message */
  word num_bytes;
    /* Number of bytes in flash with info message */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i, j;
    /* Loop counters */
  boolean bytes_added = FALSE;
    /* Flag if bytes have been added to the message */
  byte *buf_index = buf_ptr;
    /* Index into packed external message body, used to copy it */
  byte rec_type;
    /* The type of an information record in this message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Pay the price to copy the entire packed message body */
  num_bytes = (*len_ptr + AUTH_EIGHT_BITS - 1) / AUTH_EIGHT_BITS;
  for (i = 0; i < num_bytes; i++)
  {
    msg_buf[i] = *buf_index;
    buf_index++;
  }

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = crypt_pos = FPOS( cai_flash_fix_type, hdr.encryption ) +
                        FSIZ( cai_flash_fix_type, hdr.encryption );

  for (i = 0; i < int_ptr->flash.num_recs; i++)
  {
    if (bytes_added)
    {
      /* We need to copy the record type to the encrypted message */
      rec_type = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_TYPE_SIZE);  
      b_packb( rec_type, buf_ptr, crypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    crypt_pos += AUTH_RECORD_TYPE_SIZE;

    num_data_bytes = b_unpackb( msg_buf, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_added)
    {
      /* We need to copy the record length to the encrypted message */
      b_packb( num_data_bytes, buf_ptr, crypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    crypt_pos += AUTH_RECORD_LEN_SIZE;

    /* Unpack the type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( msg_buf, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    if (num_data_bytes == 1)
    {
      /* Add an extra zero byte */
      bytes_added = TRUE;
      (*len_ptr) += AUTH_EIGHT_BITS;
      num_data_bytes = 2;
      auth_temp_buf[1] = 0;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < num_data_bytes; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, crypt_pos, AUTH_EIGHT_BITS );
      crypt_pos += AUTH_EIGHT_BITS;
    }
  }
} /* auth_encrypt_flash() */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_ORIG_C

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel origination
  continuation message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Origination Continuation Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_orig_c
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{
  word end_pos;
    /* Position of end of message to encrypt */
  word data_pos;
    /* Position of data in packed external message */
  boolean eight_bit_digits;
    /* Whether or not the digits are eight bits (as opposed to four) */
  byte num_digits;
    /* Number of digits in origination continuation */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i;
    /* Loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy some values to make the code more readable */
  eight_bit_digits = (int_ptr->orig_c.digit_mode == 1);
  num_digits = int_ptr->orig_c.num_fields;
  end_pos = *len_ptr;

  /* --------------------------------------------------------------------
  ** Determine how many bytes of data we have to encrypt, and add bits
  ** to ensure we have an integral number of bytes greater than one.
  ** -------------------------------------------------------------------- */
  num_data_bytes = (eight_bit_digits) ? num_digits : ((num_digits + 1) / 2);

  if ((!eight_bit_digits) && ((num_digits % 2) != 0))
  {
    /* ------------------------------------------------
    ** We have an odd number of 4 bit digits. Fill with
    ** four zero bits to allign to a byte boundry.
    ** ------------------------------------------------ */
    b_packb( 0, buf_ptr, end_pos, AUTH_FOUR_BITS );
    end_pos += AUTH_FOUR_BITS;
  }

  if (((eight_bit_digits) && (num_digits == 1)) ||
      ((!eight_bit_digits) && ((num_digits == 1) || (num_digits == 2))))
  {
    /* We have only one byte of data. Fill with eight more zero bits. */
    b_packb( 0, buf_ptr, end_pos, AUTH_EIGHT_BITS );
    end_pos += AUTH_EIGHT_BITS;
    num_data_bytes++;
  }

  /* Update total length to reflect if it has been increased */
  *len_ptr = end_pos;

  /* ------------------------------------------
  ** Extract data to be encrypted, which is now
  ** an integral number of bytes greater than 1
  ** ------------------------------------------ */
  data_pos = FPOS( cai_orig_c_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = FPOS( cai_orig_c_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

} /* auth_encrypt_orig_c() */

/*===========================================================================

FUNCTION AUTH_CRYPT_BURST_DTMF

DESCRIPTION
  This procedure encrypts/decrypts CDMA Burst DTMF message.  I copy the
  packed digit data into a temporary buffer, encrypt/decrypt it, and
  then put it back.

DEPENDENCIES
  The Burst DTMF Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of the message may increase if encrypting or decrease if
  decrypting.

===========================================================================*/
void auth_crypt_burst_dtmf
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  boolean encrypting
    /* Flag if we are encrypting or decrypting tye message */
)
{
  word end_pos;
    /* Position of end of message to encrypt */
  word data_pos;
    /* Position of data in packed external message */
  byte num_dgts;
    /* Number of digits in origination continuation */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i;
    /* Loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy some values to make the code more readable */
  num_dgts = b_unpackb( buf_ptr, FPOS( cai_sbdtmf_fix_type, num_digits ),
                                 FSIZ( cai_sbdtmf_fix_type, num_digits ));
  end_pos = *len_ptr;

  /* --------------------------------------------------------------------
  ** Determine how many bytes of data we have to encrypt/decrypt, and add
  ** to ensure we have an integral number of bytes greater than one.
  ** -------------------------------------------------------------------- */
  num_data_bytes = (num_dgts + 1) / 2;

  if (encrypting)
  {
    if ((num_dgts % 2) != 0)
    {
      /* ------------------------------------------------
      ** We have an odd number of 4 bit digits. Fill with
      ** four zero bits to allign to a byte boundry.
      ** ------------------------------------------------ */
      b_packb( 0, buf_ptr, end_pos, AUTH_FOUR_BITS );
      end_pos += AUTH_FOUR_BITS;
    }

    if ((num_dgts == 1) || (num_dgts == 2))
    {
      /* We have only one byte of data. Fill with eight more zero bits. */
      b_packb( 0, buf_ptr, end_pos, AUTH_EIGHT_BITS );
      end_pos += AUTH_EIGHT_BITS;
      num_data_bytes++;
    }

    /* Update total length to reflect if it has been increased */
    *len_ptr = end_pos;
  }
  else /* decrypting */
  {
    if (num_data_bytes == 1)
    {
      /* -------------------------------------------
      ** If there is only one bytes worth of digits,
      ** there are two bytes of data to decrypt.
      ** ------------------------------------------- */
      num_data_bytes = AUTH_TWO_BYTES;
      /* Adjust length to reflect we will be removing a byte. */
      *len_ptr -= AUTH_EIGHT_BITS;
    }

    if ((num_dgts % 2) != 0)
    {
      /* ----------------------------------------------
      ** We have an odd number of 4 bit digits.  Adjust
      ** length to reflect we will be removing 4 bits.
      ** ---------------------------------------------- */
      *len_ptr -= AUTH_FOUR_BITS;
    }
  }

  /* -------------------------------------------------
  ** Extract data to be encrypted/decrypted.  The data
  ** is now an integral number of bytes greater than 1
  ** ------------------------------------------------- */
  data_pos = FPOS( cai_sbdtmf_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = FPOS( cai_sbdtmf_type, var );

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }
} /* auth_crypt_burst_dtmf() */

/*===========================================================================

FUNCTION AUTH_CRYPT_DTMF_ORD

DESCRIPTION
  This procedure encrypts/decrypts a CDMA Continuous DTMF Tone Order.
  I copy the data to be encrypted/decrypted into a temporary buffer,
  encrypt/decrypt it, and then put it back.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_crypt_dtmf_ord
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  boolean encrypting
    /* Flag if we are encrypting or decrypting */
)
{
  word data_pos;
    /* Position of data in packed external message */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------
  ** Extract data to be encrypted.
  ** ----------------------------- */
  if (encrypting)
  {
    data_pos = FPOS( cai_rtc_ord_type, ordq.add_record_len );
  }
  else /* decrypting */
  {
    data_pos = FPOS( cai_ftc_ord_type, ordq.add_record_len );
  }

  /* First byte is 3 bit ADD_RECORD_LEN field and 5 bits of ORDQ */
  auth_temp_buf[0] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
  data_pos += AUTH_EIGHT_BITS;

  if (encrypting)
  {
    /* ----------------------------------------------------------------
    ** Second byte is last 3 bits of ORDQ.  Shift these so they are the
    ** most significant bits of the byte, and fill the rest with zeros.
    ** ---------------------------------------------------------------- */
    auth_temp_buf[1] = (byte) (b_unpackb( buf_ptr, data_pos,
                                     AUTH_THREE_BITS ) << 5);

    /* ---------------------------------------------------------------------
    ** If we are encrypting, 5 'reserved' bits of the message are encrypted.
    ** Since the reserved bits are added after the encryption, increasing
    ** the message length by 5 bits effectively decreases the number of
    ** reserved bits (added later in order to allign the message to a byte
    ** boundary) by 5.
    ** --------------------------------------------------------------------- */
    *len_ptr += AUTH_FIVE_BITS;
  }
  else /* decrypting */
  {
    /* ---------------------------------------------------------------------
    ** If we are decrypting, 5 'reserved' bits of the message are decrypted.
    ** Since these bits were never really part of the message, decreasing
    ** the length by 5 bits effectively removes them.
    ** --------------------------------------------------------------------- */
    auth_temp_buf[1] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    *len_ptr -= AUTH_FIVE_BITS;
  }

  CAVE_encrypt( auth_temp_buf, AUTH_TWO_BYTES );

  if (encrypting)
  {
    data_pos = FPOS( cai_rtc_ord_type, ordq.add_record_len );
  }
  else /* decrypting */
  {
    data_pos = FPOS( cai_ftc_ord_type, ordq.add_record_len );
  }
  b_packb( auth_temp_buf[0], buf_ptr, data_pos, AUTH_EIGHT_BITS );
  data_pos += AUTH_EIGHT_BITS;
  b_packb( auth_temp_buf[1], buf_ptr, data_pos, AUTH_EIGHT_BITS );

} /* auth_crypt_dtmf_ord() */

/*===========================================================================

FUNCTION AUTH_CRYPT_DATA_BURST

DESCRIPTION
  This procedure encrypts/decrypts CDMA reverse traffic channel Data Burst
  Message.  I copy the packed chari data into a temporary buffer, encrypt/
  decrypt it, and then put it back.

DEPENDENCIES
  The Data Burst must have at least two chari fields.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_crypt_data_burst
(
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  boolean encrypting
    /* Flag if we are encrypting or decrypting */
)
{
  word data_pos;
    /* Position of data in packed external message */
  byte num_fields;
    /* Number of bytes in the un-encrypted data burst message */
  byte num_data_bytes;
    /* Number of bytes of data to be encrypted/decrypted */
  word i;
    /* Loop counter */

  byte burst_type;
    /* Burst type of the Data Burst Message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  burst_type = b_unpackb( buf_ptr, FPOS( cai_burst_tc_fix_type, burst_type ),
                                   FSIZ( cai_burst_tc_fix_type, burst_type ));

  /* Copy some values to make the code more readable */
  num_fields = b_unpackb( buf_ptr, FPOS( cai_burst_tc_fix_type, num_fields ),
                                   FSIZ( cai_burst_tc_fix_type, num_fields ));

  /* ------------------------------------------
  ** Extract data to be encrypted, which is an
  ** integral number of bytes greater than 1.
  ** ------------------------------------------ */
  data_pos = FPOS( cai_burst_tc_fix_type, num_fields ) +
             FSIZ( cai_burst_tc_fix_type, num_fields );

  /* If BURST_TYPE is equal to '111110' or '111111', */
  /* all CHARi fields after the first two shall be   */
  /* encrypted; otherwise, all CHARi fields shall be */
  /* encrypted.                                      */
  if ((burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
      ||
      (burst_type == CAI_EXTENDED_BURST_TYPE)
     )
  {
    if (num_fields <= 2)
    {
      MSG_ERROR(" <= 2 bytes of payload for burst %d", burst_type, 0, 0);
      return;
    }
    else
    {
      MSG_HIGH("skip two bytes for burst %d", burst_type, 0, 0);
      num_fields -= 2;
      data_pos += ( 2 * AUTH_EIGHT_BITS ) ;
    }
  }
  
  /* These are the same unless we are dealing with a one-byte data burst */
  num_data_bytes = num_fields;

  if (!encrypting) /* decrypting */
  {
    if (num_fields == 1)
    {
      /* --------------------------------------------
      ** If there is only one byte in the data burst,
      ** there are two bytes of data to decrypt.
      ** -------------------------------------------- */
      num_data_bytes = AUTH_TWO_BYTES;
    }
  }

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  if (encrypting)
  {
    if (num_fields == 1)
    {
      /* We have only one byte of data.  Add another zero byte. */
      auth_temp_buf[1] = 0;
      num_data_bytes = AUTH_TWO_BYTES;
    }
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = FPOS( cai_burst_tc_fix_type, num_fields ) +
             FSIZ( cai_burst_tc_fix_type, num_fields );

  if ((burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
      ||
      (burst_type == CAI_EXTENDED_BURST_TYPE)
     )
  {
    data_pos += ( 2 * AUTH_EIGHT_BITS ) ;
  }
  
  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }
} /* auth_crypt_data_burst() */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION AUTH_ENCRYPT_EFWIM

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel Extended Flash with
  Information message.  To accomplish this I first make a temporary
  copy of the packed message body.  Then I unpack each set of type
  specific fields from the copy, encrypt them, and copy them back
  into the original message to be transmitted.  If a record_len is
  only one byte, another byte is added.  Once the message to be
  transmitted is lengthened it is necessary to copy all remaining
  fields (still encrypting the type specific fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_efwim
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{

  word msg_pos;
    /* Pointer into packed external message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = FPOS( cai_efwim_type, con_ref_incl ) +
              FSIZ(cai_efwim_type, con_ref_incl );

  if (int_ptr->efwim.con_ref_incl)
  {
    msg_pos = FPOS(cai_efwim_type, num_rec) + FSIZ(cai_efwim_type, num_rec);
  }
  else
  {
    msg_pos += FSIZ(cai_efwim_type, num_rec);
  }

  auth_encrypt_info_recs(len_ptr, buf_ptr, int_ptr, msg_pos,
                         int_ptr->efwim.num_rec);

} /* auth_encrypt_efwim() */

/*===========================================================================

FUNCTION AUTH_ENCRYPT_EOM_DIGITS

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel enhanced origination
  message.  I copy the packed digit data into a temporary
  buffer, encrypt it, and then put it back.

DEPENDENCIES
  The Enhanced Origination Message must have at least one digit in it.

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_encrypt_eom_digits
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  caii_tx_msg_type *int_ptr,
    /* Pointer to internal message to be transmitted */
  word digit_start_pos
    /* Start location of digits */
)
{
  word end_pos;
    /* Position of end of message to encrypt */
  word data_pos;
    /* Position of data in packed external message */
  boolean eight_bit_digits;
    /* Whether or not the digits are eight bits (as opposed to four) */
  byte num_digits;
    /* Number of digits in origination continuation */
  byte num_data_bytes;
    /* Number of data bytes to be encrypted */
  word i;
    /* Loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy some values to make the code more readable */
  eight_bit_digits = (int_ptr->eom.digit_mode == 1);
  num_digits = int_ptr->eom.num_fields;
  end_pos = *len_ptr;

  /* --------------------------------------------------------------------
  ** Determine how many bytes of data we have to encrypt, and add bits
  ** to ensure we have an integral number of bytes greater than one.
  ** -------------------------------------------------------------------- */
  num_data_bytes = (eight_bit_digits) ? num_digits : ((num_digits + 1) / 2);

  if ((!eight_bit_digits) && ((num_digits % 2) != 0))
  {
    /* ------------------------------------------------
    ** We have an odd number of 4 bit digits. Fill with
    ** four zero bits to allign to a byte boundry.
    ** ------------------------------------------------ */
    b_packb( 0, buf_ptr, end_pos, AUTH_FOUR_BITS );
    end_pos += AUTH_FOUR_BITS;
  }

  if (((eight_bit_digits) && (num_digits == 1)) ||
      ((!eight_bit_digits) && ((num_digits == 1) || (num_digits == 2))))
  {
    /* We have only one byte of data. Fill with eight more zero bits. */
    b_packb( 0, buf_ptr, end_pos, AUTH_EIGHT_BITS );
    end_pos += AUTH_EIGHT_BITS;
    num_data_bytes++;
  }

  /* Update total length to reflect if it has been increased */
  *len_ptr = end_pos;

  /* ------------------------------------------
  ** Extract data to be encrypted, which is now
  ** an integral number of bytes greater than 1
  ** ------------------------------------------ */
  data_pos = digit_start_pos;

  for (i=0; i < num_data_bytes; i++)
  {
    auth_temp_buf[i] = b_unpackb( buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

  CAVE_encrypt( auth_temp_buf, num_data_bytes );

  /* Now write encrypted data back over original data */
  data_pos = digit_start_pos;

  for (i=0; i < num_data_bytes; i++)
  {
    b_packb( auth_temp_buf[i], buf_ptr, data_pos, AUTH_EIGHT_BITS );
    data_pos += AUTH_EIGHT_BITS;
  }

} /* auth_encrypt_eom_digits() */

/*===========================================================================

FUNCTION AUTH_DECRYPT_EXTENDED_INFO

DESCRIPTION
  This procedure decrypts CDMA forward traffic channel Extended Flash with
  Information and Extended Alert with information messages.  To accomplish
  this I unpack each set of type specific fields, encrypt them,
  and copy them back into the original received packed message.
  If a record_len is only one byte, the extra byte added for
  encryption is removed and the overall length is decreased.
  Once the received packed message is shortened it is necessary
  to copy all remaining fields (still decrypting the type specific
  fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  TURE if successful. Otherwise return FALSE.

SIDE EFFECTS
  The length of Traffic Channel message may be increased.

===========================================================================*/
boolean auth_decrypt_extended_info
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of packed external-format message */
  word start_pos,
    /* Start point of Information Record */
  byte num_recs
    /* Numer of records in this message */
)
{

  word msg_pos;
    /* Current position in encrypted external message */
  byte num_data_bytes;
    /* Number of data bytes to be decrypted */
  word i, j;
    /* Loop counter */
  boolean bytes_removed = FALSE;
    /* Flag if bytes have been removed from the message */
  byte rec_type;
    /* The type of an information record in this message */
  byte rec_len;
    /* The original length of an information record before encryption */
  word total_length = *len_ptr;
    /* The overall length of the encrypted message body */
  boolean status = TRUE;
    /* return status */
  word decrypt_pos;
    /* Pointer to decrypt position */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** decrypt it, remove a byte if necessary, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = decrypt_pos = start_pos;

  for (i=0; i<num_recs; i++)
  {
    if (bytes_removed)
    {
      /* We need to slide the record type field up due to removed bytes */
      rec_type = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, decrypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    decrypt_pos += AUTH_RECORD_TYPE_SIZE;

    rec_len = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_removed)
    {
      /* We need to slide the record length field up due to removed bytes */
      b_packb( rec_len, buf_ptr, decrypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    decrypt_pos += AUTH_RECORD_LEN_SIZE;

    if (rec_len == 1)
    {
      /* Add an extra zero byte */
      bytes_removed = TRUE;
      (*len_ptr) -= AUTH_EIGHT_BITS;
      num_data_bytes = 2;
    }
    else
    {
      num_data_bytes = rec_len;
    }

    if ((num_data_bytes * AUTH_EIGHT_BITS + msg_pos) > total_length)
    {
      /* --------------------------------------------------------
      ** The info record length would cause us to read beyond the
      ** end of the received message buffer.
      ** -------------------------------------------------------- */
      ERR("Message Length invalid", 0, 0, 0);
      status = FALSE;
      break;
    }

    /* Unpack the (encrypted) type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( buf_ptr, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < rec_len; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, decrypt_pos, AUTH_EIGHT_BITS );
      decrypt_pos += AUTH_EIGHT_BITS;
    }
  }

  return status;

} /* auth_decrypt_extended_info() */

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION AUTH_DECRYPT_INFO

DESCRIPTION
  This procedure decrypts CDMA reverse traffic channel Flash with
  Information and alert with information messages.  To accomplish
  this I unpack each set of type specific fields, encrypt them,
  and copy them back into the original received packed message.
  If a record_len is only one byte, the extra byte added for
  encryption is removed and the overall length is decreased.
  Once the received packed message is shortened it is necessary
  to copy all remaining fields (still decrypting the type specific
  fields, of course).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The length of Traffic Channel message to transmit may be increased.

===========================================================================*/
void auth_decrypt_info
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr
    /* Pointer to body of packed external-format message */
)
{

  word msg_pos;
    /* Current position in encrypted external message */
  word decrypt_pos;
    /* Current position in decrypted message (decrypt_pos <= msg_pos) */
  byte num_data_bytes;
    /* Number of data bytes to be decrypted */
  word j;
    /* Loop counter */
  boolean bytes_removed = FALSE;
    /* Flag if bytes have been removed from the message */
  byte rec_type;
    /* The type of an information record in this message */
  byte rec_len;
    /* The original length of an information record before encryption */
  word total_length = *len_ptr;
    /* The overall length of the encrypted message body */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------------
  ** Now step through each info record, extract the type specific fields,
  ** add a byte if necessary, encrypt it, and place it back in the message.
  ** ---------------------------------------------------------------------- */
  msg_pos = decrypt_pos = FPOS( cai_gen_tc_type, hdr.encryption ) +
                        FSIZ( cai_gen_tc_type, hdr.encryption );

  while ((total_length - msg_pos) > sizeof( cai_rec_hdr_type )) /*lint !e574 */
  {
    if (bytes_removed)
    {
      /* We need to slide the record type field up due to removed bytes */
      rec_type = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_TYPE_SIZE);
      b_packb( rec_type, buf_ptr, decrypt_pos, AUTH_RECORD_TYPE_SIZE);
    }
    msg_pos += AUTH_RECORD_TYPE_SIZE;
    decrypt_pos += AUTH_RECORD_TYPE_SIZE;

    rec_len = b_unpackb( buf_ptr, msg_pos, AUTH_RECORD_LEN_SIZE);
    if (bytes_removed)
    {
      /* We need to slide the record length field up due to removed bytes */
      b_packb( rec_len, buf_ptr, decrypt_pos, AUTH_RECORD_LEN_SIZE);
    }
    msg_pos += AUTH_RECORD_LEN_SIZE;
    decrypt_pos += AUTH_RECORD_LEN_SIZE;

    if (rec_len == 1)
    {
      /* Add an extra zero byte */
      bytes_removed = TRUE;
      (*len_ptr) -= AUTH_EIGHT_BITS;
      num_data_bytes = 2;
    }
    else
    {
      num_data_bytes = rec_len;
    }

    if ((num_data_bytes * AUTH_EIGHT_BITS + msg_pos) > total_length)
    {
      /* --------------------------------------------------------
      ** The info record length would cause us to read beyond the
      ** end of the received message buffer.
      ** -------------------------------------------------------- */
      ERR("Message Length invalid", 0, 0, 0);
      break;
    }

    /* Unpack the (encrypted) type specific fields */
    for (j = 0; j < num_data_bytes; j++)
    {
      auth_temp_buf[j] = b_unpackb( buf_ptr, msg_pos, AUTH_EIGHT_BITS);
      msg_pos += AUTH_EIGHT_BITS;
    }

    CAVE_encrypt( auth_temp_buf, num_data_bytes );

    for (j = 0; j < rec_len; j++)
    {
      b_packb( auth_temp_buf[j], buf_ptr, decrypt_pos, AUTH_EIGHT_BITS );
      decrypt_pos += AUTH_EIGHT_BITS;
    }
  }
} /* auth_decrypt_info() */

/*===========================================================================

FUNCTION AUTH_ENCRYPTION_POSSIBLE

DESCRIPTION
  This procedure determines if message encryption is possible.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if auth could successfully encrypt messages.
  FALSE if auth is not initialized properly to do encryption.

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_encryption_possible( void )
{
  boolean possible;
    /* Flag if encryption is possible */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (auth_init_status == AUTH_UNINITIALIZED)
  {
    MSG_HIGH( "Need to initialize AUTH", 0, 0, 0 );
    possible = FALSE;
  }
  else if (!auth_valid_CMEA_key)
  {
    ERR( "Need to generate keys first", 0, 0, 0 );
    possible = FALSE;
  }
  else
  {
    possible = TRUE;
  }
  return (possible);
}

/*===========================================================================

FUNCTION AUTH_ENCRYPT_CDMA

DESCRIPTION
  This procedure encrypts CDMA reverse traffic channel messages.
  If any fields require encryption they are encrypted.

DEPENDENCIES
  The function auth_encryption_possible needs to be called first to ensure
  message encryption is possible.

RETURN VALUE
  None

SIDE EFFECTS
  The num_digits, num_fields, record_len, etc. fields may not accurately
  represent the size of the field(s) if bits are added for encryption.

===========================================================================*/
void auth_encrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr,
    /* Pointer to body of already packed traffic channel message */
  caii_tx_msg_type *int_ptr
    /* Pointer to internal message to be transmitted */
)
{

  switch (int_ptr->gen.msg_type)
  {
    case CAI_REV_FLASH_MSG:
      if (int_ptr->flash.num_recs !=0)
      {
        auth_encrypt_flash( len_ptr, buf_ptr, int_ptr );
      }
      break;

    case CAI_SEND_DTMF_MSG:
      if (int_ptr->sbdtmf.num_digits != 0)
      {
        /* There is some data to encrypt */
        auth_crypt_burst_dtmf( len_ptr, buf_ptr, TRUE );
      }
      break;

    case CAI_ORIG_C_MSG:
      if (int_ptr->orig_c.num_fields != 0)
      {
        /* There is some data to encrypt */
        auth_encrypt_orig_c( len_ptr, buf_ptr, int_ptr );
      }
      break;

    case CAI_TC_REV_ORD_MSG:
      if (int_ptr->tc_ord.gen.order == CAI_DTMF_ORD)
      {
        auth_crypt_dtmf_ord( len_ptr, buf_ptr, TRUE );
      }
      break;

    case CAI_TC_REV_BURST_MSG:
      if (int_ptr->tc_burst.num_fields != 0)
      {
        auth_crypt_data_burst( buf_ptr, TRUE );
      }
      break;

#ifdef FEATURE_IS2000_REL_A
    case CAI_EFWIM_MSG:
      if (int_ptr->efwim.num_rec != 0)
      {
        auth_encrypt_efwim( len_ptr, buf_ptr, int_ptr );
      }
      break;
#endif

    default:
      /* The message doesn't require any encryption */
      break;
  } /* end switch */
} /* auth_encrypt_cdma() */

/*===========================================================================

FUNCTION AUTH_DECRYPT_CDMA

DESCRIPTION
  This procedure decrypts CDMA reverse traffic channel messages.
  If any fields require decryption they are decrypted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if there are no problems encountered.
  FALSE if fields of the given message that should be decrypted are not.

SIDE EFFECTS
  None

===========================================================================*/
boolean auth_decrypt_cdma
(
  word *len_ptr,
    /* Length of message in bits */
  byte *buf_ptr
    /* Pointer to received message to be translated */
)
{
  boolean success;
    /* Flag if decryption is successful */
#ifdef FEATURE_IS2000_REL_A
  boolean con_ref_incl;
    /* Flag to remember whetehr con_ref is included */
  byte num_recs;
    /* Number of info records in the message */
  word msg_pos;
    /* Keeps track of the current location in the packed msg */
#endif /* FEATURE_IS2000_REL_A */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (auth_init_status == AUTH_UNINITIALIZED)
  {
    ERR( "Need to initialize AUTH", 0, 0, 0 );
    success = FALSE;
  }
  else
  {
    switch ( b_unpackb( buf_ptr, FPOS( cai_gen_type, msg_type ),
                                 FSIZ( cai_gen_type, msg_type )))
    {
      case CAI_FWD_FLASH_MSG:
      case CAI_ALERT_MSG:
        if (!auth_valid_CMEA_key)
        {
          ERR( "Need to generate keys first", 0, 0, 0 );
          success = FALSE;
        }
        else
        {
          /* Make sure we have at least one information record */
          if ((sizeof( cai_gen_tc_type ) +
              sizeof( cai_rec_hdr_type )) < *len_ptr)
          {
            auth_decrypt_info( len_ptr, buf_ptr );
          }
          success = TRUE;
        }
        break;

      case CAI_SEND_BURST_MSG:
        if (!auth_valid_CMEA_key)
        {
          ERR( "Need to generate keys first", 0, 0, 0 );
          success = FALSE;
        }
        else
        {
          if ((b_unpackb( buf_ptr, FPOS( cai_sbdtmf_fix_type, num_digits ),
                          FSIZ( cai_sbdtmf_fix_type, num_digits ))) != 0)
          {
            /* There is some data to decrypt */
            auth_crypt_burst_dtmf( len_ptr, buf_ptr, FALSE );
          }
          success = TRUE;
        }
        break;

      case CAI_TC_FWD_ORD_MSG:
        if ((b_unpackb( buf_ptr, FPOS( cai_ftc_ord_type, gen.order ),
                    FSIZ( cai_ftc_ord_type, gen.order )))  == CAI_DTMF_ORD)
        {
          if (!auth_valid_CMEA_key)
          {
             ERR( "Need to generate keys first", 0, 0, 0 );
             success = FALSE;
          }
          else
          {
            auth_crypt_dtmf_ord( len_ptr, buf_ptr, FALSE );
            success = TRUE;
          }
        }
        else
        {
          /* There is no encryption necessary for any other orders */
          success = TRUE;
        }
        break;

      case CAI_TC_FWD_BURST_MSG:
        if (!auth_valid_CMEA_key)
        {
          ERR( "Need to generate keys first", 0, 0, 0 );
          success = FALSE;
        }
        else
        {
          if ((b_unpackb( buf_ptr, FPOS( cai_burst_tc_fix_type, num_fields ),
              FSIZ( cai_burst_tc_fix_type, num_fields )))  != 0)
          {
            auth_crypt_data_burst( buf_ptr, FALSE );
          }
          success = TRUE;
        }
        break;

#ifdef FEATURE_IS2000_REL_A
      case CAI_EXT_ALERT_W_INFO_MSG:
      case CAI_EXT_FLASH_W_INFO_MSG:
        if (!auth_valid_CMEA_key)
        {
          ERR( "Need to generate keys first", 0, 0, 0 );
          success = FALSE;
        }
        else
        {
          /* If this function is called, that means encryption
          ** is set to '01'. There should be no extended
          ** encryption fields. L3 fields starts right after
          ** encryption.
          */
          msg_pos = FPOS( cai_gen_tc_type, hdr.encryption)
                     + FSIZ( cai_gen_tc_type, hdr.encryption);

          con_ref_incl = b_unpackb( buf_ptr, msg_pos,
                          FSIZ(cai_extended_alert_w_info_type, con_ref_incl));
          msg_pos += FSIZ(cai_extended_alert_w_info_type, con_ref_incl);

          if (con_ref_incl)
          {
            msg_pos += FSIZ(cai_extended_alert_w_info_type, con_ref);
          }

          num_recs =
            b_unpackb( buf_ptr, msg_pos,
                       FSIZ( cai_extended_alert_w_info_type, num_recs ));
          msg_pos += FSIZ(cai_extended_alert_w_info_type, num_recs);


          /* Make sure we have at least one information record */
          if (num_recs > 0)
          {
            success = auth_decrypt_extended_info( len_ptr, buf_ptr,
                                                  msg_pos, num_recs );
          }
          else
          {
            success = TRUE;
          }
        }
        break;

#endif /* FEATURE_IS2000_REL_A */

      default:
        /* The message doesn't require any decryption */
        success = TRUE;
        break;
    }
  }

  return (success);
} /* auth_decrypt_cdma() */

#ifdef FEATURE_ACP
#error code not present
#endif /* FEATURE_ACP */

#endif /* FEATURE_AUTH */

/*===========================================================================

FUNCTION AUTH_INIT

DESCRIPTION
  This procedure initializes the queues and timers for Authentication Task.
  It should be called only once, at powerup time. It also initializes RUIM
  related variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void auth_init( void )
{
  byte i; /* Index into free buffer pool */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------
  ** Initialize timers
  ** ----------------- */
  rex_def_timer( &auth_rpt_timer, &auth_tcb, AUTH_RPT_TIMER_SIG );
#ifdef FEATURE_AUTH
  rex_def_timer( &auth_ssd_timer, &auth_tcb, AUTH_SSD_TIMER_SIG );
#endif /* FEATURE_AUTH */

  /* -----------------
  ** Initialize queues
  ** ----------------- */

  (void) q_init( &auth_cmd_q );
  (void) q_init( &auth_free_q );

  /* Fill auth_free_q */
  for (i = 0; i < AUTH_NUM_CMD_BUFS; i++)
  {
    auth_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &auth_free_q;
    q_put( &auth_free_q, q_link( &auth_cmd_pool[i],
           &auth_cmd_pool[i].hdr.cmd_hdr.link ) );
  }

#ifdef FEATURE_AUTH
  /* -----------------
  ** Initialize flags
  ** ----------------- */

  new_ssd_valid = FALSE;
  used_new_ssd = FALSE;
  a_key_temp_valid = FALSE;
#endif /* FEATURE_AUTH */

} /* auth_init() */

#ifdef FEATURE_UIM_RUIM
/* <EJECT> */
/*===========================================================================

FUNCTION AUTH_WAIT

DESCRIPTION
      This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting. It also acknowledges the TASK_STOP_SIG and
      TASK_OFFLINE_SIG when they are set.

DEPENDENCIES
      Relies on AUTH_RPT_TIMER_SIG being raised often enough to
      avoid watchdog alarms, so should not be called until auth_init has
      run.

RETURN VALUE
      Returns the value returned by rex_wait.

SIDE EFFECTS
      The watchdog gets kicked periodically.

===========================================================================*/

LOCAL word  auth_wait
(
  /* Mask of signals to wait for */
  word  sigs
)
{
  /* Return value from REX: bitmap of signals found raised */
  word  rex_signals_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. Also acknowledge the TASK_OFFLINE_
     SIG or TASK_STOP_SIG if they are set*/

  do
  {
    /* Wait for the caller's conditions OR our kick timer */
    rex_signals_mask = rex_wait ( sigs | AUTH_RPT_TIMER_SIG | TASK_STOP_SIG |
                                TASK_OFFLINE_SIG );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &auth_tcb, TASK_STOP_SIG );
      /* Process task stop procedure.*/
      task_stop();
    }
    else if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &auth_tcb, TASK_OFFLINE_SIG );
      /* Process task offline procedure. */
      task_offline();
    }
    else if ((rex_signals_mask & AUTH_RPT_TIMER_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_AUTH_RPT);
      (void) rex_set_timer( &auth_rpt_timer, DOG_AUTH_RPT_TIME );
    }
  } while ( (rex_signals_mask & sigs ) == 0x0000 );
  /* Keep looping as long as user's conditions are un-met */

  /* Return the rex_wait return value with user condition(s) set */
  return ( rex_signals_mask );

} /* auth_wait */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION AUTH_TASK

DESCRIPTION
  This procedure is the entrance procedure for the Authentication task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

void auth_task
(
  dword dummy
    /* Parameter required for REX.  Tell lint to ignore it. */
    /*lint -esym(715,dummy) */
)
{

  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */
  auth_cmd_type *cmd_ptr;          /* Pointer to received command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------
  ** Perform initialization
  ** ---------------------- */
  auth_init();
  cmd_ptr = NULL;  /* Initialize the pointer to NULL pointer */

  /* ------------------------------------------------
  ** Process task start procedure.
  ** ------------------------------------------------ */
  task_start(
    AUTH_RPT_TIMER_SIG,             /* report timer signal for task */
    DOG_AUTH_RPT,                   /* watchdog report */
    &auth_rpt_timer                 /* pointer to report timer */
  );

  /* ------------------------------------------------------------------
  ** Initially kick watchdog and set timer for watchdog report interval
  ** ------------------------------------------------------------------ */
  dog_report(DOG_AUTH_RPT);
  (void) rex_set_timer( &auth_rpt_timer, DOG_AUTH_RPT_TIME );

  for (;;)
  {
    /* Never exit this loop... */

    /* If there is already a command on our command queue,  don't wait. */
    if (q_cnt( &auth_cmd_q ) != 0)
    {
      rex_signals_mask = AUTH_CMD_Q_SIG;
    }
    else
    {
      rex_signals_mask =
        rex_wait( AUTH_RPT_TIMER_SIG | AUTH_CMD_Q_SIG | AUTH_SSD_TIMER_SIG |
                  TASK_OFFLINE_SIG | TASK_STOP_SIG );
    }

    if ((rex_signals_mask & AUTH_RPT_TIMER_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_AUTH_RPT);
      (void) rex_set_timer( &auth_rpt_timer, DOG_AUTH_RPT_TIME );
    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, and process task stop procedure.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &auth_tcb, TASK_STOP_SIG );
      /* Process task stop procedure. */
      task_stop();
    }

    /* -----------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, and process task offline procedure.
    ** ----------------------------------------------------- */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &auth_tcb, TASK_OFFLINE_SIG );
      /* Process task offline procedure. */
      task_offline();
    }

#ifdef FEATURE_AUTH
    /* ---------------------
    ** The SSD Timer expired
    ** --------------------- */
    if ((rex_signals_mask & AUTH_SSD_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( &auth_tcb, AUTH_SSD_TIMER_SIG );
      if (auth_ssd_update_in_progress)
      {
        /* ------------------------------------------
        ** The base station took too long to complete
        ** the SSD update so abort it.
        ** ------------------------------------------ */
        MSG_MED("SSD timer expired - Aborting", 0, 0, 0);
#ifdef FEATURE_UIM_RUIM
        (void) uim_power_control ( UIM_AUTH, FALSE );
#endif /* FEATURE_UIM_RUIM */
        auth_ssd_update_in_progress = FALSE;

       /* Event report: Timer T64m expired */
       mclog_report_event_timer(CAI_TIMER_T64M);

      }
    }
#endif /* FEATURE_AUTH */

    /*-----------------------------
    ** We have a command to process
    ** ---------------------------- */
    if ((rex_signals_mask & AUTH_CMD_Q_SIG) != 0)
    {
      (void) rex_clr_sigs( &auth_tcb, AUTH_CMD_Q_SIG );
      if ((cmd_ptr = (auth_cmd_type *)q_get( &auth_cmd_q )) != NULL)
      {
        auth_process_command( cmd_ptr );

        /* return buffer to free queue */
        if( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
        {
          /* place command on requested queue */
           q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                &cmd_ptr->hdr.cmd_hdr.link );
        }
        else
        {
          ERR("done_q_ptr was NULL!", 0, 0, 0);
        }
      }  /* (cmd_ptr != NULL) */
    } /* end if ((rex_signals_mask & AUTH_CMD_Q_SIG) != 0) */
  } /* end for (;;) */
} /* end auth_task */

/*===========================================================================

FUNCTION AUTH_SEND_UPDATE_A_KEY_CMD

DESCRIPTION
  Gets an available auth_cmd_type from the auth_free_q and constructs
  an update A_KEY cmd to AUTH task. This is used by the SVCPRG module.
  This function is remoted for use by the SVCPRG module in some targets.

DEPENDENCIES
  None.
  
RETURN VALUE
  True if auth cmd was successfully queued, False otherwise
  
SIDE EFFECTS
  Queue signal is set for the AUTH task
===========================================================================*/

boolean auth_send_update_a_key_cmd 
(
  byte * a_key,
  int num_a_key_digits,
  int nam
)
{
  auth_cmd_type *auth_ptr;

  MCC_CHK_NULL_PTR (a_key);

  if( ( auth_ptr = q_get(&auth_free_q)) == NULL )
  {
    /* could not get a buffer */
   MSG_ERROR( " couldn't get buffer for update AKEY cmd", 0, 0, 0 );
   return FALSE;
  }
  if (num_a_key_digits > AUTH_A_KEY_DIGITS)
  {
    /* Too many digits in AKEY */
    MSG_ERROR( "Too many digits in AKEY %d", num_a_key_digits, 0, 0 );
    return FALSE;
  }
  /* tell auth to update AKEY  */
  auth_ptr->hdr.command = AUTH_UPDATE_A_KEY_F;
  auth_ptr->hdr.cmd_hdr.task_ptr = NULL;
  memcpy( auth_ptr->a_key.a_key, a_key , num_a_key_digits ); /*lint !e732 */
  auth_ptr->a_key.nam = nam;
  auth_ptr->a_key.rpt_function = NULL;
  auth_cmd(auth_ptr );
  return TRUE;

}

/*===========================================================================

FUNCTION AUTH_CMD

DESCRIPTION
  The auth_cmd pointed to by the parameter passed in is queued up for AUTH.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void auth_cmd (
  auth_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
    /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  q_put( &auth_cmd_q, &cmd_ptr->hdr.cmd_hdr.link ); /* and queue it */

  (void)rex_set_sigs( &auth_tcb, AUTH_CMD_Q_SIG );  /* signal the Auth task */
}
#endif /* FEATURE_AUTH || FEATURE_UIM_RUIM */
/*lint +e818 */
