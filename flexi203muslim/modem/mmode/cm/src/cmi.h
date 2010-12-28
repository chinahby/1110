#ifndef CMI_H
#define CMI_H
/*===========================================================================

              C A L L   M A N A G E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions used internally by the Call Manager.
  It provides internal interface to the CM.C file.
  It should NOT be included by any source file outside the Call Manager


Copyright (c) 1998 - 2009 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmi.h#7 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/09   pm      Added MMEXTN define to replace EXTERN define
04/09/09   aj      Added support for IRAT measurements 
                   and hold cmd reason for hold cmd Q
04/02/09   sv      Added event buffers for MBMS during power collapse
03/26/09   ks      Added cm_disable_call_type typedef for uint64 bit mask
02/25/09   sv      Correct Lint errors
12/28/08   rn      FDN checking option in call control
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
09/15/08   cl      Add initialization before subscription commands are sent.
                   This allows the service domain to be sent to UI correctly.
09/04/08   dm/sv   Update Ph cmd info to accept WPS start parameter
07/01/08   rm      Mainlining of MBMS changes
05/28/08   ks      Updating the associated primary profile number when 
                   secondary PDP is promoted to Primary
05/27/08   sv      Add new power collapse bitmask, to buffer any events
                   during Power Collapse State
05/06/08   ak      BCMCS 2.0 changes
04/29/08   ks      Reverting the changes of Updating the associated primary 
                   profile number when secondary PDP is promoted to Primary
03/31/08   ks      Updating the associated primary profile number 
                   when secondary PDP is promoted to Primary
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
01/03/08   ka      Adding handset based plus code dialing feature.
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/07/07   ra      Added support for IMS VCC in HOMER files 
11/26/07   sv      Added CM_ALPHA_CODING_SCHEME_NONE to initialize 
                   enc_alpha.coding_scheme
06/14/07   pk/cl   Added support for power suspend
04/15/07   rp      Removing FEATURE_MMGPS
04/03/07   sk      Added unused_client_ptr field to phone info object to use
                   along with CM_PH_CMD_CLIENT_RELEASE command.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/18/07   rp      Added support for WMS<->CM<->NAS interface.
11/17/06   jqi     Added support for HDR active protocol revision information.
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
11/07/06   sk      Memory reduction effort.
09/14/06   ka      Adding support for call control on secondary ps calls
08/28/06   ka      Adding pdp header comp and data comp enums.
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data 
                   structures, fields and functions
07/06/06   ka/ic   Featurization fixes for PS call control 
07/04/06   ka      Changes to support call control on PS calls.
06/14/06   pk      Added support for cmtask_orig_para_count_2 and related 
                   enums
06/05/06   ka/ic   Adding CM_ASCII_MAX_NUMBER_CHARS.
03/27/06   pk      Added support for cmtask_orig_para_change_act_priority
                   Added helper functions for the support
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/09/06   pk      Added support for POWER COLLAPSE
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/19/06   pk      Externed cm_dip_switch_set_ddtm_on and 
                   cm_dip_switch_set_ddtm_off
01/06/06   ic      Lint cleanup  
12/09/05   ic      Lint cleanup 
11/23/05   ka      Modification to allow hold/active for ip calls.
11/08/05   sk      Separated CM reports and commands.
10/23/05   jqi     Hold ps data call orig or data avail request if there is a 
                   pending get network list request.
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
                   Moved cm_ss_e_type from cmcall.h
09/28/05   pk      Added CM_ACT_TYPE_NON_STD_OTASP
09/12/05   pk      Added FEATURE_GPSONE to cm_pd_cmd_info_s_type added
                   cm_pd_session_dormant_e_type session_dormant to info type
08/31/05   pk      Added support for priority Queue for the following call 
                   types CM_CALL_TYPE_TEST, CM_CALL_TYPE_PD,
                   CM_CALL_TYPE_VT, CM_CALL_TYPE_VT_LOOPBACK, 
                   CM_CALL_TYPE_SUPS
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/15/05   pk      Added support for priority Queues for origination 
                   parameters
07/21/05   sk      Removed internal use of is_jcdma_emergency flag.
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/06/05   pk      Added cm_ss_cmd_info_s_type to hold SS command information
03/14/05   ka      IP rpt struct added to cm_cmd_u_type
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/03/04   ws      Add API for Remote WMS.
11/23/04   ic      Put user-to-user signaling code under FEATURE_UUS
11/19/04   ka      Adding support for user to user signaling
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/27/04   jqi     Added ps dormant optimization.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/20/04   sj      Added support for PRL Preference in sys_sel_pref().
08/17/04   ic      Undoing P4 change list #121638
08/12/04   ws      Remove cm20 support.
08/02/04   ic      Added Lint error supression to INRANGE()
07/28/04   ka      Added support for secondary PDP context
07/27/04   ws      Call Control support for SUPS.
07/26/04   ic      Modified INRANGE() macro to cast its parameters to int32
                   to get rid of 'pointless comparison of unsigned integer
                   with zero' RVCT 2.1 compiler warning
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Added support for FEATURE_ALS
07/05/04   dk      Added Iterator Type.
06/22/04   ic      Comments clean up
06/09/04   dk      Moved cm_cmd_dealloc to cmtask.h.
06/01/04   npr     Added IP Header and Data compression requirements to GW PS
                   call origination and information structures for GPRS SNDCP.
04/28/04   ka      Fixed Lint errors.
04/13/04   ic      Featurization changes
03/30/04   ws      Moved favored_mode_pref to mode independent structs.
03/24/04   ic      Fixed compiler warning
03/17/04   sj      Added new DDTM parameters in the command info struct.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ka      Added support for Multiple PDP and get call info api.
11/14/03   ws      Added support for is_jcdma_emergency
11/07/03   ws      Fixed GPS featurization compile errors
10/28/03   ws      Removed uim pref slot support.
10/20/03   ws      Dual Slot Support.
10/08/03   ws      Added ps data orig pref enums.
09/30/03   vt      Modified to support common cm.h across all targets.
09/18/03   ws      Added subscription source enum.
09/16/03   vt      Added preferred slot handling.
08/22/03   vt      Mainlined packet state handling.
08/13/03   prk     Added mimum_qos field to cm_call_cmd_info_gw_ps_s_type.
07/08/03   sbs/lt  Added support for Location Services related SS messages
05/22/03   sun     Made gw_cs and gw_ps a union in call_cmd_info_s_type
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/25/03   ws      JCDMA support for call hold
04/28/03   AT      Lint cleanup.
04/14/03   prk     Removed references to FEATURE_SUPS.
04/14/03   ws      Added BSID changes
03/11/03   AT      Added support for call control performed by clients.
02/28/03   AT      Added support for call control.
02/27/03   ws      Removed redundant end_reason and end_reason_included definitions
02/24/03   vt      Undid the changes for HDR.
02/18/03   vt      Fixed problems to build for HDR.
02/13/03   prk     Mainlined the FEATURE_CONC_SRVC_NEW_API feature.
02/06/03   RI      Added support for concurrent services.
                     - added end_params field in cm_call_cmd_mode_info_s_type.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for accessing user preferred PLMN list.
11/19/02   vt      Added changing RTRE configuration.
11/15/02   vt      Properly featurized the packet state.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Mainlined feature SD20.
10/22/02   ic      Added cm_m51_mode_init() prototype
09/20/02   az      Changed the type of variable l2_ack_wait_time from uint8 to dword
                   in cm_dbm_cmd_s_type
09/10/02   ws      Added initial cm20 backward compatibility.
08/30/02   vt      Added calling number to call cmd structure.
08/06/02   HQ      Added FEATURE_CDSMS.
07/22/02   PRK     Added network_list_type field to cm_ph_cmd_info_s_type.
04/02/02   hxw     Modified FEATURE_GPSONE_MSBASED.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for tiered services - user zone support.
05/07/02   PRK     Fixed indentation.
05/05/02   vt      Fixed compile problem with new mm cm.h
02/13/02   ic      Changed the type of call_id field from byte/uint8 etc
                   to cm_call_id_type as it should have been to begin with.
02/07/02   ic      Under FEATURE_IS2000_REL_A, removed cm_nc_is_flash_emergency_func
02/04/02   RI      Added a new field ie_ss_code_s_type in
                   cm_sups_cmd_info_s_type.
01/23/02   PRK     Updated CM_PH_CMD_SELECT_NETWORK command parameters.
                   Featurized future PLMN selection/reselection code with
                   FEATURE_NEW_PLMN.
                   Updated type definition for PLMN id.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
01/14/02   ic      Fixes related to running with FEATURE_IS2000_REL_A undefined
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/21/01   RI      Modified the function declaration for cm_find_call_id.
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Added support for PLMN selection/reselection (FEATURE_PLMN).
                     Moved srv_state, roam_status, rssi and rssi_delta from
                     phone group to serving system group (FEATURE_NEWSS).
10/17/01   AT      Added the signals for AOC timers and events.
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
09/14/01   HD      Added support for interface to TMC.
08/21/01   ic      Main-lined FEATURE_NEWCM
08/23/01   HD      NAS header file reorg changes.
04/27/01   HD      Added changes for WCDMA/GSM mode.
11/17/01   sh      Added 1xhdr data session handoff support
11/01/01   sh      Removed is_force_colloc from the call cmd info
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/03/01   VT      Added determining whether flash number is emergency or not.
08/21/01   ic      Main-lined FEATURE_NEWCM
03/01/01   sko     Added CM monitor data into cm_client_s_type for NEWCM.
02/28/01   HQ      Fixed compiler warnings.
02/23/01   HQ      Added CM_RUIM_STATUS_SIG.
02/07/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Added GPSONE_DBM support when NEWCM is defined.
01/23/01   ych     Merged JCDMA features.
12/15/00   HQ      Added privacy_pref field.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
07/27/00   PJ      Added support for PD commands. (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
07/18/00   JQ      Added support for end_reason
03/20/00   RY      Featurized all the cm_commit_reply_* functions with
                   FEATURE_OTASP
03/16/00   VT      Added BROADCAST SMS support
03/13/00   RY      Added OTAPA support
12/18/99   CS      Added cm_kick_dog function.
08/31/99   SH      Modified definitions to featurize UASMS
08/09/99   SH      Added SMS object
07/07/99   SH      Added CNAP feature
06/30/99   SH      Changed type of otasp_act_code to cm_activate_code_type
06/21/99   SH      added the cmd_err field for all cmd_info structures
05/31/99   SH      added CM 2.0 features
02/04/99   RC      Changes the definition of CM_OTASP_ACT_CODE_NONE and
                   CM_SRV_OPT_NONE to avoid ARM compiler confusion.
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/


#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */
#include "sys.h"       /* System/Global definition file */
#include "cm.h"        /* External interface to cm.c */

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#include "cmxll.h"     /* Interface to UI task */
#include "db.h"        /* Interface to Database services */
#include "mc.h"        /* Interface to MC Task */
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#include "cmipapp.h"   /* Interface to IP report structure */
#include "rex.h"


/*===========================================================================
=============================================================================
============================= MISCELLANEOUS BLOCK ===========================
=============================================================================
===========================================================================*/


/*===========================================================================
=============================================================================
========================  DEFINES ===========================================
=============================================================================
===========================================================================*/
#define CM_DEFAULT_VALUE   0

/*
** Put EXTERNAL in front of the definition of every function that is supposed
** to be externalized.
** This way each function in a .c file will end up having EXTERNAL or static
** in front of it.
** This makes it easy to verify whether we meant for this
** function to be externalized or whether we forgot to make it static
**
*/
#define EXTERNAL


/* Macro to correctly align function definitions (cosmetic).
*/
#define MMEXTN


#define CM_ACTIVATE_PDP_REQ  0x41
  /* SM message to indicate pdp activation request
  ** Message Types for sm to sm peer messages, 
  ** defined in Table 10.4a of TS 24.008.
  */


#define CM_SM_PD             0x0A
  /* Protocol descriminator to identify GPRS session management message.
  ** Table 11.2 of spec 24.007.
  */

#define CM_LLC_SAPI_3        3
  /* Section 10.5.6.9 of spec 24.008.  
  ** Identifies the service access point that is used for GPRS 
  ** data transfer at LLC layer.
  */

/* Type for init mask variable.
*/
typedef byte    cm_init_mask;
typedef word    cm_timer_event_type;
typedef uint32  cm_act_id_type;
typedef uint64  cm_disable_call_type;

/* CM uses temp ascii buffers to convert dialed digits from
** bcd. Extra byte required to store '\0'
*/
#define CM_ASCII_MAX_NUMBER_CHARS   (CM_MAX_NUMBER_CHARS + 1)

typedef int  cm_sm_prot_descr_type;
  /*   
  ** Describes protocol used for originating PDP context.
  ** Set to SM_PD by default.
  */

typedef byte cm_sm_trans_id_type;
  /*   
  ** Type for transaction id used for originating PDP context. 
  */

typedef  uint8     cm_sm_msg_type;
  /* Defines the type of sm layer message */

/*---------------------------------------------------------------------------
                        BASIC TYPES, MACROS & FUNCTIONS
---------------------------------------------------------------------------*/

/* Init mask to identify whether a specific object was ever initialized.
** During object initialization the object's init_mask parameter is set
** to a unique mask of alternating 1's and 0's (i.e. 1010...).
** Later the init_mask parameter can be checked against this value to
** confirm that the object was properly initialized.
*/

/* Init mask value.
*/
#define CM_INIT_MASK    0x66


/* Macro to mark an object as properly initialized.
*/
#define CM_INIT_MARK( mask )   ( (mask) = CM_INIT_MASK )


/* Macro to check whether an object was properly initialized.
*/
#define CM_INIT_CHECK( mask )  ( (mask) == CM_INIT_MASK )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of CM feature states.
*/
typedef enum cm_feature_e {

    CM_FEATURE_NONE=-1,      /* Don't care if feature is turned ON/OFF */

    CM_FEATURE_OFF = FALSE,  /* Feature is turned ON */
    CM_FEATURE_ON  = TRUE,   /* Feature us turned OFF */

    CM_FEATURE_MAX

} cm_feature_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration to keep track of preferences for packet switched
   data call originations.
*/
typedef enum cm_ps_data_orig_prefs_e {

  CM_PS_DATA_ORIG_PREF_NONE              = -1,

  CM_PS_DATA_ORIG_PREF_CDMA_ONLY         = 0,
    /* Orig ps data calls on cdma only */

  CM_PS_DATA_ORIG_PREF_CDMA_HDR          = 1,
    /* Orig ps data calls on cdma or hdr only */

  CM_PS_DATA_ORIG_PREF_CDMA_HDR_IF_CHINA = 2,
    /* Orig ps data calls on cdma or hdr if china */

  CM_PS_DATA_ORIG_PREF_GW_ONLY           = 3,
    /* Orig ps data calls on GSM/WCDMA only */

  CM_PS_DATA_ORIG_PREF_ANY               = 4,
    /* Orig ps data calls on any preferences */

  CM_PS_DATA_ORIG_PREF_WLAN_ONLY         = 5,
    /* Orig ps data calls on WLAN only */

  CM_PS_DATA_ORIG_PREF_WLAN_LESS_PREF    = 6,
    /* Orig ps data calls on any system except WLAN */

  CM_PS_DATA_ORIG_PREF_MAX

} cm_ps_data_orig_prefs_e_type;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of ps data state associated with data available request */
typedef enum cm_ps_data_stt_e {

  CM_PS_DATA_STT_NONE = -1,           /* For internal use */

  CM_PS_DATA_STT_PENDING,             /* Pending state */
  CM_PS_DATA_STT_SRV_ACQ,             /* Looking for full service */
  CM_PS_DATA_STT_RESUME,              /* Waiting for data resume */

  CM_PS_DATA_STT_MAX                  /* For internal use  */

} cm_ps_data_stt_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Enumeration to keep track of hold command reasons
*/
typedef enum cm_hold_cmd_reas_e {

  CM_HOLD_CMD_REAS_NONE = -1,

  CM_HOLD_CMD_REAS_GPS,
    /* Hold command till gps session end */

  CM_HOLD_CMD_REAS_MEAS,
    /* Hold command till measurement mode on response arrives from HDRCP */

  CM_HOLD_CMD_REAS_MAX

} cm_hold_cmd_reas_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether val is between min and max
** (not include min and max).
*/
#define BETWEEN( val, min, max ) \
/*lint -save -e641 -e568 */ ((val) > (min) && (val) < (max)) \
/*lint -restore */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to check whether val is in the range of min to max
** (including min and max).
*/
#define INRANGE( val, min, max )  \
/*lint -save -e641 -e568 -e685 */ (((int32)val) >= ((int32)min) && ((int32)val) <= ((int32)max)) \
/*lint -restore */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* Macro to check whether a character is an ASCII alpha.
*/
#define IS_ALPHA( c ) ( ((c) >= 'a' && (c) <= 'z') || \
                        ((c) >= 'A' && (c) <= 'Z') )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to safely decrement to 0.
*/
#define CM_DEC( num )  ( (num) > 0 ? (--(num)) : (num) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to safely assign a value to a boolean variable.
*/
#ifndef BOOLEAN
#define BOOLEAN( val )  ( (boolean) ((val) ? TRUE : FALSE) )
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Return TRUE if p1 contains p2 otherwise return FALSE.
*/
#define CM_CONTAIN( p1, p2 )  \
        ( BOOLEAN( (byte)p1 & (byte)p2 ) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                          CIRCULAR QUEUE MACROS
---------------------------------------------------------------------------*/


/* Macros to operate on a circular queue that is implemented as a
** contiguous array of elements in memory.
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to define a circular queue of a specified type, name and size.
*/
#define CIRC_Q_DEF( type, name, size ) struct { unsigned int  head;      \
                                                unsigned int  tail;      \
                                                unsigned int  cnt;       \
                                                type arr[size];          \
                                              } name


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to initialize a circular queue.
*/
#define CIRC_Q_INIT( q )  q.tail = q.head = q.cnt = 0;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the size of a circular queue.
*/
#define CIRC_Q_SIZE( q ) ARR_SIZE( q.arr )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the count (i.e. the number of elements) of a circular
** queue.
*/
#define CIRC_Q_CNT( q ) ( q.cnt )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the next index of a circular queue relatively
** to a given index.
*/
#define CIRC_Q_NEXT( q_size, q_pos ) ((q_pos)+1 >= (q_size) ? 0: ((q_pos)+1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to increment an index (i.e. head or tail) of a circular queue.
*/
#define CIRC_Q_INC( q_size, q_pos )  ( q_pos = CIRC_Q_NEXT( q_size, q_pos ) )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to enqueue an item onto a circular queue.
** Return TRUE if item is successfully enqueued, else return FALSE.
*/
#define CIRC_Q_PUT( q, item )                                               \
 ( q.cnt >= CIRC_Q_SIZE(q) ? FALSE : ( q.arr[q.tail] = item,                \
                                       CIRC_Q_INC( CIRC_Q_SIZE(q), q.tail), \
                                       q.cnt++,                             \
                                       TRUE ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to dequeue an item from a circular queue.
** Return TRUE if item is successfully dequeued, else return FALSE.
*/
#define CIRC_Q_GET( q, item )                                               \
    ( q.cnt == 0 ? FALSE : ( item = q.arr[q.head],                          \
                             CIRC_Q_INC( CIRC_Q_SIZE(q), q.head ),          \
                             q.cnt--,                                       \
                             TRUE ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the item at the head of a circular
** queue. If queue is empty a NULL is returned.
*/
#define CIRC_Q_CHECK( q ) ( q.cnt == 0 ? NULL : &q.arr[q.head] )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to extract the low/high 32bits of a QWORD (64 bits).
*/
#define QWORD_LOW( qword )  ( (dword) ((uint64) qword & (uint64) 0xFFFFFFFF))
#define QWORD_HIGH( qword ) ( (dword) ((uint64) qword >> 32) )

/*---------------------------------------------------------------------------
                       PHONE-BOOK INTERFACE FUNCTIONS
---------------------------------------------------------------------------*/



/*===========================================================================

FUNCTION cm_nc_resolve_call_type

DESCRIPTION
  Resolve originating number type.

  This functions consults the number-classification to whether the originating number
  matches one of the special numbers that are stored in the phone-book
  (i.e. E911, OTASP, etc.). If a match is found, it modifies the originating
  call type accordingly.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating call type (pointed by activate_code_ptr) and or
  the activation code (pointed by activate_code_ptr).

===========================================================================*/
extern void cm_nc_resolve_call_type(

    const cm_num_s_type            *orig_num_ptr,
        /* pointer to originating number */

    cm_call_type_e_type      *call_type_ptr,
        /* pointer to associated call type */

    cm_activate_code_e_type    *activate_code_ptr
        /* pointer to associated OTASP activation code
        ** (for OTASP call types only) */
);




/*===========================================================================

FUNCTION cm_nc_is_call_allowed_func

DESCRIPTION
  Decide whether an originating number is allowed.

  This functions consults number-classification to whether the originating number
  is allowed by the origination restriction currently in effect.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of originating number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_nc_is_call_allowed_func(

    const cm_num_s_type            *orig_num_ptr
        /* pointer to originating number */
);



/*===========================================================================

FUNCTION cm_nc_call_control_processing

DESCRIPTION
  Perform call type resolution and consults number-classification to whether
  the originating number is allowed and do any required call control.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  FALSE if CM cmd buffer allocation fails, TRUE otherwise

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_nc_call_control_processing(

  cm_call_id_type                       call_id,
    /* call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /* call type */

  cm_num_s_type                         *orig_num_ptr,
    /* orginating number */

  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  cm_gw_ps_cc_params_s_type             *gw_ps_cc_params_ptr,

  const cm_cc_generic_params_s_type           *cc_generic_params_ptr
    /* Call-control generic parameters */

);


/*===========================================================================

FUNCTION cm_nc_call_control_preferences

DESCRIPTION
  Perform a resolution of which mode is preferred after call control.

DEPENDENCIES
  number-classification interface object must have already been initialized with
  cm_nc_init().

RETURN VALUE
  TRUE of originating number is allowed for origination, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cc_mode_pref_e_type cm_nc_call_control_preferences(

  cm_orig_call_control_s_type            *cdma_call_control_info_ptr,
    /* cdma information for the call */

  cm_orig_call_control_s_type            *gw_call_control_info_ptr,
    /* gw information for the call */

  cm_num_s_type                          *orig_num_ptr,
    /* Original number information before call control */

  cm_call_type_e_type                    call_type,
    /* The type of call returned by call control */

  sys_sys_mode_e_type                    current_mode
    /* mode, if any, that is acquired right now */
);


#ifdef FEATURE_JCDMA
/*===========================================================================

FUNCTION cm_nc_pre_pending_dial_func

DESCRIPTION
  Resolve originating dial numbers for pre-pending.

  This functions insert or delete the dialed number.
  (i.e. 184, 186, etc.)

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  Modifies the originating dial numbers.

===========================================================================*/
extern void cm_nc_pre_pending_dial_func(

    cm_num_s_type                  *orig_num_ptr,
        /* dialed number */

    const cm_call_type_e_type      *call_type_ptr
        /* Associated call type */

);
#endif /* FEATURE_JCDMA */



/*===========================================================================
=============================================================================
=========================== CLIENT INTERFACE BLOCK ==========================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                                  CM COMMAND
---------------------------------------------------------------------------*/


/* Enum of CM command types.

   Whenever you modify this enumeration,
   make sure that you modify cm_cmd_type_e_type_string_map
   in cmdbg.c accordingly.

*/
typedef enum cm_cmd_type_e {

    CM_CMD_TYPE_NONE = -1,

    CM_CMD_TYPE_CALL,         /* Client call command */
    CM_CMD_TYPE_PH,           /* Client phone command */
    CM_CMD_TYPE_SS,           /* Client serving system command */
    CM_CMD_TYPE_INBAND,       /* Client inband command */
    CM_CMD_TYPE_SMS,          /* Client SMS command */
    CM_CMD_TYPE_MC_RPT,       /* MC report */
    CM_CMD_TYPE_RPT,          /* Lower Layer report */

    CM_CMD_TYPE_SD_RPT,       /* SD report */

    #ifdef FEATURE_GPSONE
#error code not present
    #endif /* FEATURE_GPSONE */

    #ifdef FEATURE_DBM
    CM_CMD_TYPE_DBM,           /* Data Burst Message command */
    #endif /* FEATURE_DBM */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
    CM_CMD_TYPE_BCMCS,        /* Client BCMCS command */
    #endif /* FEATURE_BCMCS || MBMS */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif   /* FEATURE_WCDMA || FEATURE_GSM */

    CM_CMD_TYPE_IP_RPT,       /* Report queued from IP Apps */

    CM_CMD_TYPE_MBMS,         /* Client MBMS Command*/

    CM_CMD_TYPE_MAX

} cm_cmd_type_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


typedef enum cm_apps_pwr_status_e{

  CM_APPS_PWR_STATUS_NONE = -1,     /* Internal for CM */

  CM_APPS_PWR_STATUS_PWR_COLLAPSE,  /* The apps processor is power collapsed */

  CM_APPS_PWR_STATUS_PWR_RESTORE,   /* The apps processor is NOT power collapsed */

  CM_APPS_PWR_STATUS_PWR_SUSPEND,   /* The apps processor is in standby mode */

  CM_APPS_PWR_STATUS_MAX            /* Internal for CM */

}cm_apps_pwr_status_e_type;

/* Type for Dynamic Environment Manager object
*/
typedef struct cm_dem_s{

  cm_apps_pwr_status_e_type      apps_pwr_status;

}cm_dem_s_type;

/* Define values indicating empty (or inactive) command fields,
*/
#define CM_SRV_OPT_NONE              ( (word) (0xFFFF) )
#define CM_NUMBER_LEN_NONE           ( 0 )
#define CM_ALPHA_LEN_NONE            ( 0 )
#define CM_ALPHA_CODING_SCHEME_NONE  ( 0 )
#ifdef FEATURE_IS95B_EXT_DISP
/* display type for CDMA extended display info record */
#define CM_EXT_DISP_TYPE_NONE  ( (byte) 0xFF )
#endif /* FEATURE_IS95B_EXT_DISP */
#define CM_CLIENT_PTR_NONE           ( 0 )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for bit array of arbitrary size.
** Being used to indicate for which call and phone events
** a client is registered.
*/
typedef struct bit_arr_s {

    byte    *bit_buf;
        /* Pointer to bit buffer */

    int     bit_size;
        /* Bit size of the buffer pointed by bit_buf */

} bit_arr_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for CM client.
**
** Client structures keep a record of all the CM events for which their
** respective client is registered and thus call on their client callback
** functions as needed.
**
** Each CM client needs to allocate such a CM client structure in its own
** program space.
**
** Clients register and/or de-register for CM events dynamically via methods
** that operate on the CM client object that is allocated in their own
** program space.
**
** NOTE THAT NONE OF THE CM CLIENT STRUCTURE FIELDS SHOULD BE MODIFIED OR
** ACCESSED DIRECTLY BY THE ITS CLIENT. ANY INITIALIZATION OR MANIPULATION
** OF THE CM CLIENT STRUCTURE IS TO BE DONE VIA THE CM CLIENT METHODS
** SUPPLIED IN THIS MODULE.
*/
typedef struct cm_client_s {


    q_link_type               link;
        /* Link element to link clients into a client-list */
        /* this element has to be the first in this structure
           because the address of link is also used as
           the "self pointer" (to save memory space by eliminating
           self_ptr in q_link_type) to access other element in this
           data structure */

    byte                      init_mask;
        /* Indicate whether this object was properly initialized */

    boolean                   is_active;
        /* Indicate whether client object is active. Note that CM
        ** only notifies active clients of CM events */

    struct cm_client_s        *client;
        /* Client ID - each initialized client structure has
        ** a unique client ID */

    cm_processor_e_type       client_processor;
        /* The processor on which the client is executing
        */

    byte                      call_bit_buf[1 + (int)CM_CALL_EVENT_MAX/8];
    bit_arr_s_type            call_bit_arr;
        /* Bit array indicating to which CALL events this
        ** client is registered */

    byte                      ph_bit_buf[1 + (int)CM_PH_EVENT_MAX/8];
    bit_arr_s_type            ph_bit_arr;
        /* Bit array indicating to which PHONE events this
        ** client is registered */

    byte                      ss_bit_buf[1 + (int)CM_SS_EVENT_MAX/8];
    bit_arr_s_type            ss_bit_arr;
        /* Bit array indicating to which SERVING SYSTEM events this
        ** client is registered */

    byte                      inband_bit_buf[1 + (int)CM_INBAND_EVENT_MAX/8];
    bit_arr_s_type            inband_bit_arr;
        /* Bit array indicating to which inband events this
        ** client is registered */

    byte                      sms_bit_buf[1 + (int)CM_SMS_EVENT_MAX/8];
    bit_arr_s_type            sms_bit_arr;
       /* Bit array indicating to which SMS events this
       ** client is registered */

    #ifdef FEATURE_GPSONE
#error code not present
    #endif /* FEATURE_GPSONE */

    #ifdef FEATURE_DBM
    byte                      dbm_event_bit_buf[1 + (int)CM_DBM_EVENT_MAX/8];
    bit_arr_s_type            dbm_event_bit_arr;
        /* Bit array indicating to which DBM events this
        ** client is registered */

    byte                      dbm_class_bit_buf[1 + (int)CM_DBM_CLASS_MAX/8];
    bit_arr_s_type            dbm_class_bit_arr;
        /* Bit array indicating to which DBM class this
        ** client is registered */
    #endif /* FEATURE_DBM */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
    byte                      bcmcs_bit_buf[1 + (int)CM_BCMCS_EVENT_MAX/8];
    bit_arr_s_type            bcmcs_bit_arr;
        /* Bit array indicating to which BCMCS events this
        ** client is registered */
    #endif /* FEATURE_BCMCS || MBMS */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
    #endif /* FEATURE_APPS_POWER_COLLAPSE */

    cm_mm_call_event_f_type   *call_event_func;
        /* Function pointer to notify client of call events */

    cm_ph_event_f_type        *ph_event_func;
        /* Function pointer to notify client of phone events */

    cm_mm_ss_event_f_type     *ss_event_func;
        /* Function pointer to notify client of serving system events */

    cm_inband_event_f_type    *inband_event_func;
        /* Function pointer to notify client of inband events */

    cm_sms_event_f_type       *sms_event_func;
        /* Function pointer to notify client of SMS events */

    cm_call_info_list_f_type  *call_info_list_func;
        /* Function pointer to notify client of call information list */

    #ifdef FEATURE_GPSONE
#error code not present
    #endif /* FEATURE_GPSONE */

    #ifdef FEATURE_DBM
    cm_dbm_event_f_type       *dbm_event_func;
        /* Function pointer to notify client of DBM events */

    #endif /* FEATURE_DBM */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
    cm_bcmcs_event_f_type     *bcmcs_event_func;
        /* Function pointer to notify client of BCMCS events */

    #endif /* FEATURE_BCMCS || MBMS */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
    #endif /* FEATURE_APPS_POWER_COLLAPSE */

    cm_mm_call_cmd_err_f_type *call_cmd_err_func;
        /* Function to notify client of call command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients - e.g. data services failed to
        ** originate because phone is offline */

    cm_ph_cmd_err_f_type      *ph_cmd_err_func;
        /* Function to notify the client of phone command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_ss_cmd_err_f_type      *ss_cmd_err_func;
        /* Function to notify the client of serving command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_mm_call_cmd_err_f_type      *call_info_list_cmd_err_func;
        /* Function to notify the client of serving command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_mm_inband_cmd_err_f_type  *inband_cmd_err_func;
        /* Function to notify the client of inband command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    cm_sms_cmd_err_f_type     *sms_cmd_err_func;
        /* Function to notify the client of SMS command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    #ifdef FEATURE_GPSONE
#error code not present
    #endif /* FEATURE_GPSONE */

    #ifdef FEATURE_DBM
    cm_dbm_cmd_err_f_type     *dbm_cmd_err_func;
        /* Function to notify the client of PD command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */

    #endif /* FEATURE_DBM */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

    #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
    cm_bcmcs_cmd_err_f_type    *bcmcs_cmd_err_func;
        /* Function to notify the client of BCMCS command errors of OTHER
        ** clients.
        ** NOTE that typically only a UI-like client registers for such a
        ** service since it needs to display messages for command errors
        ** on behalf of other clients */
    #endif /* FEATURE_BCMCS || MBMS */

    #if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCDMA || FEATURE_GSM */

} cm_client_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold call commands information.
*/

/* lower layer task that provides the connection ID
*/
typedef enum cm_conn_id_src_e
{
  CONN_ID_UNKNOWN_SRC = -1,
  CONN_ID_MN,
  CONN_ID_SM
}cm_conn_id_src_e_type;

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */


#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
typedef struct cm_call_cmd_info_cdma_s
{

  word                       srv_opt;
    /* service option */

  cm_activate_code_e_type    otasp_act_code;
    /* OTASP call activation code */

  #ifdef FEATURE_IS2000

  boolean                    drs_bit;
    /* set the DRS bit or not */
  #endif /* FEATURE_IS2000 */

  cm_privacy_pref_e_type     privacy_pref;

  #ifdef FEATURE_HDR_HANDOFF
#error code not present
  #endif /* FEATURE_HDR_HANDOFF */

  #ifdef FEATURE_IS2000_REL_A
  boolean                    sr_id_included;
    /* sr_id indicator */

  byte                       sr_id;
    /* sr_id */

  boolean                    qos_parms_incl;
    /* Qos Parameters indicator */

  byte                       qos_parms_len;
    /* Length of QOS parameters */

  byte                       qos_parms[CM_ORIG_QOS_LEN_MAX];
    /* QOS parameters */

  #endif /* FEATURE_IS2000_REL_A */

  cm_call_sups_type_e_type   flash_type;
    /* type of flash */

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
#error code not present
  #endif

} cm_call_cmd_info_cdma_s_type;
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

typedef struct cm_call_cmd_info_ip_s
{
    boolean                   accept;
      /* Used for sending setup response for MT calls
      */
}cm_call_cmd_info_ip_s_type;



typedef struct cm_call_cmd_info_s
{
  /*
  ** Mode independent information.
  */
  cm_call_cmd_err_e_type         cmd_err;
      /* error in command request */

  cm_call_id_type                call_id;
      /* call ID of the call */

  cm_call_id_type                switch_out_call_id;
      /* call ID of the call */

  cm_call_type_e_type            call_type;
      /* Call type */

  cm_srv_type_e_type             srv_type;
      /* Service type - AMPS, CDMA, etc. */


  cm_num_s_type                  num;
      /* Number buffer:
      ** Originated calls - contains dialed address.
      ** Flash with info  - contains dialed address. */

  cm_num_s_type                  calling_num;
      /* Mobile number information */

  cm_alpha_s_type                alpha;
      /* Client specified alpha tag to be displayed along
      ** with dialed address of an originated call.
      ** NOTE! THIS IS NOT A PHONE BOOK NAME MATCH */

  uint8                          num_end_call_ids;
      /* Number of calls to be ended */

  cm_end_params_s_type           end_params[CM_CALL_ID_MAX];
      /* Call id's and end parameters of all calls to be ended */

  cm_call_end_e_type             end_status;
      /* Call end status (i.e. reason for ending a call in origination,
      ** incoming or conversation state) */

  cm_mode_pref_e_type        favored_mode_pref;
    /* Call mode preference */

  cm_hybr_pref_e_type       favored_hybr_pref;
    /* favored hybrid preference */

  /*
  ** Mode dependent information.
  */
  cm_call_mode_info_e_type       info_type;
    /* Current mode */

  //#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
  boolean   is_gsm_wcdma_cs_info_present;
    /* TRUE if GW cs info is present */

  boolean   is_gsm_wcdma_ps_info_present;
    /* TRUE if GW ps info is present */

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

  boolean                  call_control_processing_complete;
  cm_call_cmd_err_e_type   call_control_cmd_err;
    /* Variables to communicate results of call control. */

  //#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  boolean   is_cdma_info_present;
    /* TRUE if CDMA info is present */

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  cm_call_cmd_info_cdma_s_type   cdma_info;
    /* CDMA mode specific information */
  #endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  boolean                          is_ip_call_info_present;
    /* Not being set anywhere right now
    */

  cm_call_cmd_info_ip_s_type  ip_call;
    /* Structure carries specific parameters for ipcall cmd.
    */

  cm_call_sups_type_e_type            sups_cmd_type;

  cm_cc_generic_params_s_type         cc_generic_params;
    /* Call-control generic parameters */

} cm_call_cmd_info_s_type;


/* Type for call commands.
*/
typedef struct cm_call_cmd_s
{
  cm_call_cmd_cb_f_type          *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                           *data_block_ptr;
    /* pointer to a client provided callback data block */

  cm_client_s_type               *client_ptr;
    /* Client sending this command */

  cm_client_id_type              client_id;
    /* Client sending this command */

  cm_call_cmd_e_type             cmd;
    /* CM command */

  cm_call_cmd_info_s_type        info;
    /* CM command information */

} cm_call_cmd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold phone command information.
*/
typedef struct cm_ph_cmd_info_s {

    cm_ph_cmd_err_e_type              cmd_err;
        /* error in command request */

    /*
    ** cm_ph_cmd_oprt_mode parameters
    */
    sys_oprt_mode_e_type              oprt_mode;
      /* Indicate current operating mode */

    cm_answer_voice_e_type            answer_voice;
        /* indicate whether incoming calls answered as voice,
        ** modem, fax, etc */

    dword                             answer_duration;
        /* indicate seconds duration for temp. answer-voice settings,
        such as CM_ANSWER_VOICE_AS_FAX_ONCE or DB_VOICE_AS_MODEM_ONCE */

    /*
    ** cm_ph_cmd_sys_sel_pref parameters
    */
    cm_mode_pref_e_type               mode_pref;
      /* mode preference */

    cm_pref_term_e_type               pref_term;
      /* indicate the mode term preference */

    dword                             pref_duration;
      /* indicate seconds duration for temp. mode term */

    cm_gw_acq_order_pref_e_type       acq_order_pref;
      /* gw acquisition order preference */

    cm_band_pref_e_type               band_pref;
      /* band preference */

    cm_prl_pref_e_type                prl_pref;
      /* prl preference */

    cm_roam_pref_e_type               roam_pref;
      /* roam preference */

    cm_hybr_pref_e_type               hybr_pref;
      /* hybrid preference */

    cm_srv_domain_pref_e_type         srv_domain_pref;
      /* The requested/preferred service domain.               */

    cm_network_sel_mode_pref_e_type   network_sel_mode_pref;
      /* The network selection mode selected by the client.    */

    sys_plmn_id_s_type                plmn;
      /* The id of the system to be selected */

    /*
    ** cm_ph_cmd_nam_sel parameters
    */
    cm_nam_e_type                     nam_sel;
      /* indicate current NAM selection */

    /*
    ** cm_ph_cmd_signal_strength_delta parameters
    */
    uint8                             rssi_delta;
      /*indicate current RSSI Delta Threshold  */

    uint8                             ecio_delta;
      /*indicate current ECIO Delta Threshold  */

    uint8                             io_delta;
      /*indicate current ECIO Delta Threshold  */

    uint8                             sir_delta;
      /*indicate current SIR Delta Threshold  */

    uint8                             pathloss_delta;
      /*indicate current PATHLOSS Delta Threshold  */


    /*
    ** cm_ph_cmd_subscription_available parameters
    */
    cm_subscription_status_e_type     cdma_sub_avail_status;
      /* Subscription status of CDMA system */

    cm_subscription_status_e_type     gw_sub_avail_status;
      /* Subscription status of GSM/WCDMA system */

    cm_subscription_status_e_type     wlan_sub_avail_status;
    /* Subscription status of WLAN system */

    /*
    ** cm_ph_cmd_subscription_not_available parameters
    */
    cm_subscription_status_e_type     cdma_sub_not_avail_status;
      /* Subscription status of CDMA system */

    cm_subscription_status_e_type     gw_sub_not_avail_status;
      /* Subscription status of GSM/WCDMA system */

    cm_subscription_status_e_type     wlan_sub_not_avail_status;
    /* Subscription status of WLAN system */

    /*
    ** cm_ph_cmd_subscription_changed parameters
    */
    cm_subscription_status_e_type     cdma_sub_changed_status;
      /* Subscription status of CDMA system */

    cm_subscription_status_e_type     gw_sub_changed_status;
      /* Subscription status of GSM/WCDMA system */

    cm_subscription_status_e_type     wlan_sub_changed_status;
    /* Subscription status of WLAN system */

    #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
    #endif /* FEATURE_WCMDA, FEATURE_GSM */


    /*
    ** cm_ph_cmd_packet_state parameters
    **/
    cm_packet_state_e_type            packet_state;
      /* packet state */

    #ifdef FEATURE_JCDMA
    /*
    ** cm_ph_cmd_voc_activity parameters
    **/
    boolean                           voc_act_enable;
      /* Voice activity enable */

    word                              voc_act_voc_1;
      /* # of full rate */

    word                              voc_act_voc_2;
      /* # of 1/2 rate */
    word                              voc_act_voc_8;
      /* # of 1/8 rate */
    #endif /* FEATURE_JCDMA */

    cm_ss_avoid_sys_e_type      avoid_type;
      /* type of the system to be avoided */

    dword                       avoid_time;
      /* avoid time in seconds */

    cm_rtre_config_e_type       rtre_config;
      /* new RTRE configuration */

    cm_rtre_control_e_type      rtre_control;
      /* Current RTRE control - RUIM or NV */

    uint32                      new_acmmax;
    /* New value to be set to the ACMmax. */

    boolean                     uim_status;
      /* Indicates the success/failure of UIM command to set slot pref */

    #ifdef FEATURE_DDTM_CNTL
    cm_ddtm_pref_e_type         ddtm_pref;
      /* ddtm preference.
      */

    sys_ddtm_act_mask_e_type    ddtm_act_mask;
      /* DDTM actions mask.
      */

    cm_ddtm_so_list_act_e_type  ddtm_so_list_act;
      /* Specifies what to do ( add, replace, delete ) with the user
      ** specified SO list in the following parameters.
      */

    uint16                      ddtm_num_srv_opt;
      /* Number of SOs specified in list;
      */

    sys_srv_opt_type            ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
      /* List of SOs for which 1x pages have to ignored while in
      ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
      ** & ddtm_pref is ON, ignore all pages.
      */

    #endif

    cm_cphs_proc_e_type         cphs_allowed;
      /*
      ** FEATURE_ALS
      ** Whether Common PCN Handset Specifications procedures are allowed
      */

    cm_als_proc_e_type          als_allowed;
      /*
      ** FEATURE_ALS
      ** Whether ALS procedures are allowed.
      ** cphs_allowed must be CM_ALS_PROC_ALLOWED for
      ** als_allowed to be CM_ALS_PROC_ALLOWED
      */

    cm_als_line_switching_e_type line_switching;
      /*
      ** FEATURE_ALS
      ** Whether the user is allowed to switch between lines
      */

    cm_als_line_e_type          line;
      /*
      ** FEATURE_ALS
      ** Currently selected line (all MO voice calls will use it)
      */

    #if defined(FEATURE_UMTS_1X_HANDOVER_1XMSM) || \
        defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM)
#error code not present
    #endif

    cm_wakeup_e_type            wakeup_service;

    cm_wlan_pref_s_type         wlan_pref;

    cm_network_list_type_e_type network_list_type;
      /* The type of network list to be obtained. */

    boolean                     is_wlan_pref_included;

    cm_mode_pref_e_type         network_type;
      /* Network Type requested in cm_ph_cmd_get_networks() API */

    boolean                     is_command_on_hold;
      /* Is the command on Hold */

    dword                       hold_cmd_uptime;
      /* The uptime until which to hold the command  */

    cm_hold_cmd_reas_e_type     hold_cmd_reason;
      /* The reason for  which to hold the command  */

    cm_client_s_type            *unused_client_ptr;
      /**< The client object that should be freed when processing the
      ** CM_PH_CMD_CLIENT_RELASE command. */
      
    sys_wlan_wps_start_param_s_type wps_start_param;
    /* Info required to start WPS procedure */

    sys_meas_mode_e_type        meas_mode;
    /* Mode to enable/disable HDR access */

} cm_ph_cmd_info_s_type;



/* Type for phone commands.
*/
typedef struct cm_ph_cmd_s {

    cm_ph_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_client_s_type            *client_ptr;
        /* Client sending this command */

    cm_client_id_type           client_id;
        /* client sending this command */

    cm_ph_cmd_e_type            cmd;
        /* command */

    cm_ph_cmd_info_s_type       info;
        /* command information */

} cm_ph_cmd_s_type;


/* Type to hold serving system command information.
*/

typedef struct cm_ss_cmd_info_s 
{
  cm_ss_orig_thr_act_e_type         orig_thr_act;
        /* Origination throttle action */
   
  cm_ss_orig_thr_call_status_s_type call_info;
        /* Command information */

  sys_sys_mode_e_type               mode;
        /* The mode on which the call was connected */
   
  cm_country_code_type              selected_country;
        /* Stores country selected by the user
        */

  cm_sid_type                       sid;
       /* Stores sid selected by user for selected_country
       */

  cm_ss_meas_req_s_type             meas_req;
       /* Provides info about system for which measurement is 
       ** required
       */      

} cm_ss_cmd_info_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type for serving system commands.
*/
typedef struct cm_ss_cmd_s {

    cm_ss_cmd_cb_f_type         *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_client_s_type            *client_ptr;
        /* Client sending this command */

    cm_client_id_type           client_id;
        /* client sending this command */

    cm_ss_cmd_e_type            cmd;
        /* command */

    cm_ss_cmd_info_s_type       info;
        /* Command information */

} cm_ss_cmd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold inbnad commands information.
*/
typedef struct cm_inband_cmd_info_s {

  cm_inband_cmd_err_e_type    cmd_err;
         /* error in command request */
 cm_call_id_type              call_id;
     /* Call ID */
 uint8                        on_length;
     /* DTMF pulse width */
 uint8                        off_length;
     /* DTMF inter-digit interval */
 uint8                        cnt;
     /* digit count */
 uint8                        digits[CM_INBAND_MAX_REV_DIALED_DIGITS];
     /* DTMF digit buffer */
 uint16                       duration;
    /* Continuous DTMF duration; in milliseconds */

} cm_inband_cmd_info_s_type;



/* Type for inbnad commands.
*/
typedef struct cm_inband_cmd_s {

    cm_inband_cmd_cb_f_type     *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                        *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_client_s_type            *client_ptr;
        /* Client sending this command */

    cm_client_id_type           client_id;
        /* Client sending this command */

    cm_inband_cmd_e_type        cmd;
        /* CM command */

    cm_inband_cmd_info_s_type   info;
        /* CM command information */

} cm_inband_cmd_s_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type to hold SMS commands information.
*/
typedef struct cm_sms_cmd_info_s {

 cm_sms_cmd_err_e_type     cmd_err;
     /* error in command request */

 cm_sms_awi_ack_s_type     awi_ack;
     /* field for awi ack */

 cm_sms_msg_type           *data;
     /* MO SMS data */

 boolean                   l2_ack_requested;
    /* l2 ack required or not - used by MO SMS*/

  #ifdef FEATURE_BROADCAST_SMS
 boolean                   bc_enable;
  #endif /* FEATURE_BROADCAST_SMS */

 cm_sms_mo_status_e_type   mo_status;
     /* Status for MO SMS */

 cm_mode_pref_e_type       mode_pref;
     /* mode preference over which SMS is sent */

 #ifdef FEATURE_IP_CALL
 cmipapp_id_type           app_id;
    /* App ID */
 #endif

 sys_sys_mode_e_type       sys_mode;
    /* The sys mode over which the SMS is sent or received. */

} cm_sms_cmd_info_s_type;



/* Type for SMS commands.
*/
typedef struct cm_sms_cmd_s {

    cm_sms_cmd_cb_f_type    *cmd_cb_func;
        /* pointer to a client provided callback function */

    void                    *data_block_ptr;
        /* pointer to a client provided callback data block */

    cm_client_s_type        *client_ptr;
        /* Client sending this command */

    cm_client_id_type       client_id;
        /* Client sending this command */

    cm_sms_cmd_e_type       cmd;
        /* CM command */

    cm_sms_cmd_info_s_type  info;
        /* CM command information */

} cm_sms_cmd_s_type;


#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif  /* FEATURE_WCDMA || FEATURE_GSM */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_DBM

/* Type for Data Burst Message commands
*/

typedef struct
{
  uint8 burst_type;
    /* Burst type whether a PD or SDB e.g. CAI_POSITION_DET etc. */

  cm_dbm_cmd_cb_f_type    *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                    *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_client_s_type           *client_ptr;
      /* Client sending this command */

  cm_client_id_type       client_id;
      /* Client sending this command */

  cm_dbm_cmd_e_type        cmd;
      /* CM command */

  cm_dbm_cmd_info_s_type   info;
      /* The data itself */

  cm_dbm_cmd_err_e_type cmd_err;

  dword l2_ack_wait_time;
      /* Max num of seconds to wait for receiving L2 ack */

} cm_dbm_cmd_s_type;
#endif /* FEATURE_DBM */

#if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
/* Type to hold BCMCS command information.
*/
typedef struct {

  cm_bcmcs_cmd_err_e_type         cmd_err;
      /* error in command request */

  uint8                           num_of_flows;
      /* number of entries in flow_addr_array[] */

  sys_bcmcs_flow_addr_s_type      flow_addr_array[ SYS_BCMCS_MAX_FLOWS ];
      /* array of flow addresses */

  sys_bcmcs_reg_setup_e_type      flow_reg_setup_array[ SYS_BCMCS_MAX_FLOWS ];

} cm_bcmcs_cmd_info_flow_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_bcmcs_cmd_info_flow_s_type.flow_addr_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_cmd_info_flow_s_type.num_of_flows */
/*~ FIELD cm_bcmcs_cmd_info_flow_s_type.flow_reg_setup_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_cmd_info_flow_s_type.num_of_flows */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef struct {

  uint8                           num_of_flows;
      /* number of entries in flow_addr_array[] */

  sys_bcmcs_flow_addr_s_type      flow_addr_array[ SYS_BCMCS_MAX_FLOWS ];
      /* array of flow addresses */

} cm_bcmcs_cmd_info_reg_s_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FIELD cm_bcmcs_cmd_info_reg_s_type.flow_addr_array VARRAY SYS_BCMCS_MAX_FLOWS
    LENGTH cm_bcmcs_cmd_info_reg_s_type.num_of_flows */
#endif /* FEATURE_HTORPC_METACOMMENTS */

typedef struct {

  sys_bcmcs_bom_caching_e_type    enable_strict_caching;
      /* enable / disable strict BOM caching */

  uint8                           stale_BOM_factor;
      /* multiplier used to compute the staleness of BOM entry based 
         on the BOP.Only valid if strict caching is enabled. */
}cm_bcmcs_cmd_info_bom_cache_s_type;

typedef union
{
  cm_bcmcs_cmd_info_reg_s_type  reg_req;

  cm_bcmcs_cmd_info_flow_s_type flow_req;

  cm_bcmcs_cmd_info_bom_cache_s_type bom_cache_setup;
}cm_bcmcs_cmd_info_u_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE CM_BCMCS_CMD_FLOW_REQUEST_F  cm_bcmcs_cmd_info_u_type.flow_req */
/*~ CASE CM_BCMCS_CMD_REG_REQUEST_F   cm_bcmcs_cmd_info_u_type.reg_req */
/*~ CASE CM_BCMCS_CMD_REG_HANDOFF_F   cm_bcmcs_cmd_info_u_type.reg_req */
/*~ CASE CM_BCMCS_CMD_BOM_CACHING_SETUP_F cm_bcmcs_cmd_info_bom_cache_s_type */
#endif
/* Type for BCMCS commands.
*/
typedef struct
{

  cm_bcmcs_cmd_cb_f_type    *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                   *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_client_s_type       *client_ptr;
      /* Client sending this command */

  cm_client_id_type       client_id;
      /* Client sending this command */

  cm_bcmcs_cmd_e_type        cmd;
      /* CM command */

  cm_bcmcs_cmd_info_u_type   info;
      /* The data itself */

  cm_bcmcs_cmd_err_e_type    cmd_err;


} cm_bcmcs_cmd_s_type;
#endif /* FEATURE_BCMCS || MBMS */
/* Type to hold MBMS command information.
*/

typedef struct {

  cm_mbms_cmd_err_e_type                          cmd_err;
  sys_gw_mbms_req_e_type                          cmd_type;
    /* Command Type */                  

  union {
    sys_gw_mbms_activate_req_s_type               activate_req;
    sys_gw_mbms_deactivate_req_s_type             deactivate_req;
    sys_gw_mbms_session_complete_ind_s_type       session_complete_ind;
    sys_gw_mbms_abort_req_s_type                  abort_req;
    sys_gw_mbms_suspend_req_s_type                suspend_req;
    sys_gw_mbms_resume_req_s_type                 resume_req;
    sys_gw_mbms_avail_srv_list_req_s_type         avail_srv_list_req;
    sys_gw_mbms_capability_req_s_type             capability_req;
    sys_gw_mbms_set_attr_req_s_type               set_attr;                    
    
  }cmd;   

} cm_mbms_cmd_info_s_type;

/* Type for MBMS commands.
*/
typedef struct
{

  cm_mbms_cmd_cb_f_type    *cmd_cb_func;
    /* pointer to a client provided callback function */

  void                   *data_block_ptr;
      /* pointer to a client provided callback data block */

  cm_client_s_type       *client_ptr;
      /* Client sending this command */

  cm_client_id_type       client_id;
      /* Client sending this command */

  cm_mbms_cmd_e_type        cmd;
      /* CM command */

  cm_mbms_cmd_info_s_type   info;
      /* The data itself */

  cm_mbms_cmd_err_e_type    cmd_err;


} cm_mbms_cmd_s_type;

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/* Type for MC reports */
typedef struct cm_mc_rpt_s {

    cm_mc_rpt_type          *cm_mc_rpt_ptr;
        /* pointer to MC report */

} cm_mc_rpt_s_type;

#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Union of all CM command types
*/
typedef union cm_cmd_u {

  cm_call_cmd_s_type              call;
  cm_ph_cmd_s_type                ph;
  cm_ss_cmd_s_type                ss;
  cm_inband_cmd_s_type            inband;

  cm_sms_cmd_s_type               sms;

  #ifdef FEATURE_GPSONE
#error code not present
  #endif /* FEATURE_GPSONE */

  #ifdef FEATURE_DBM
  cm_dbm_cmd_s_type               dbm_msg;
  #endif

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif

  #if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
  cm_bcmcs_cmd_s_type             bcmcs;
  #endif /* FEATURE_BCMCS || MBMS */

  #if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
  #endif  /* FEATURE_WCDMA || FEATURE_GSM */

} cm_cmd_u_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type to hold all CM commands.
*/
typedef struct cm_cmd_s {

  q_link_type           link;
      /* Q linking element

         This element has to be the first
         element in this data structure
         because of removal of self_ptr
         in q_link_type
       */


  cm_cmd_type_e_type    cmd_type;
      /* Type of CM command */

  cm_cmd_u_type         cmd;
      /* CM command information */

  boolean               is_reused;
      /* Command buffers that will be requeued can use this flag to prevent
      ** the buffer from being freed when the command is processed for the
      ** first time. This flag should be check by all the callers of
      ** cm_cmd_dealloc().
      */

  uint32                alloc_time;
      /* Uptime when this buffer was allocated */

  #ifdef CM_DEBUG
#error code not present
  #endif

  uint32                line;
      /* Line number in the file above */

  char                  task[10];
      /* Name of task context which requested this buffer */

  byte                  passed_for_cc;
      /* Status of call control processing. */

} cm_cmd_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of system selections.
*/
typedef enum {

  CM_SS_NONE = -1,  /* FOR INTERNAL USE OF CM ONLY! */

  CM_SS_MAIN = SD_SS_MAIN,         /* Main system selection */
  CM_SS_HYBR_1  = SD_SS_HYBR_1,     /* HDR system selection (applicable only when doing
                                    * CDMA/HDR hybrid operation) */
  CM_SS_WLAN = SD_SS_HYBR_WLAN,    /* WLAN system selection (applicable only when doing
                                    * WLAN hybrid operation) */
  CM_SS_MAX  = SD_SS_MAX           /* Notify 1X, HDR and WLAN*/

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */

} cm_ss_e_type;


#define CM_SS_HDR CM_SS_HYBR_1

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef enum cm_cmp_criteria_e {

  CM_CMP_CRITERIA_NONE = -1,

  CM_CMP_CRITERIA_LESS_THAN,

  CM_CMP_CRITERIA_EQUAL_TO,

  CM_CMP_CRITERIA_NOT_EQUAL_TO,

  CM_CMP_CRITERIA_LESS_THAN_EQUAL_TO,

  CM_CMP_CRITERIA_GREATER_THAN,

  CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO,

  CM_CMP_CRITERIA_MAX

  #ifdef FEATURE_RPC
#error code not present
  #endif /* FEATURE_RPC */
} cm_cmp_criteria_e_type;


#define CM_ACT_ID_NONE ((cm_act_id_type) (-1))
/* Enum of CM activity
*/

typedef enum cm_act_type_e {

  CM_ACT_TYPE_NONE = -1,

  CM_ACT_TYPE_EMERG_CALL,    /* Emergency Call           */
 
  CM_ACT_TYPE_EMERG_CB_MODE, /* Emergency Call back mode */

  CM_ACT_TYPE_TEST_CALL,     /* Test call                */

  CM_ACT_TYPE_VOICE_CALL,    /* Voice Call               */

  CM_ACT_TYPE_VT_LOOPBACK,   /* VT loopback calls        */

  CM_ACT_TYPE_VT_CALL,       /* Video Telephony call     */

  CM_ACT_TYPE_PD_CALL,       /* PD call                  */
  
  CM_ACT_TYPE_SMS_CALL,      /* SMS Call                 */
  
  CM_ACT_TYPE_DATA_CALL,     /* Data Call                */

  CM_ACT_TYPE_OTAPA_CALL,    /* OTAPA Call               */

  CM_ACT_TYPE_STD_OTASP,     /* STD OTASP Call           */

  CM_ACT_TYPE_NON_STD_OTASP, /* NON_STD OTASP Call       */

  CM_ACT_TYPE_DBM,           /* Data Burst               */
  
  CM_ACT_TYPE_GPS,           /* GPS activity             */

  CM_ACT_TYPE_DEFAULT_CALL,  /* The type for the default call */

  CM_ACT_TYPE_PH_OBJ,        /* The default activity for the phone object */

  CM_ACT_TYPE_MAX            /* For internal CM use      */

}cm_act_type_e_type;


/* Enum of activity priorities
*/

typedef enum cm_act_priority_e {

  CM_ACT_PRIORITY_NONE = -1, /* NULL priority, Internal USE      */

  CM_ACT_PRIORITY_0    = 0,  /* Priority for Emergency calls     */

  CM_ACT_PRIORITY_10   = 10, /* Priority for the call back mode  */

  CM_ACT_PRIORITY_20   = 20, /* Priority for Test calls          */

  CM_ACT_PRIORITY_30   = 30, /* Priority for Voice calls         */

  CM_ACT_PRIORITY_40   = 40, /* Priority for VT loopback calls   */

  CM_ACT_PRIORITY_50   = 50, /* Priority for VT calls            */

  CM_ACT_PRIORITY_60   = 60, /* Priority for PD calls            */

  CM_ACT_PRIORITY_70   = 70, /* Priority for SMS calls           */

  CM_ACT_PRIORITY_80   = 80, /* Priority for Data calls          */

  CM_ACT_PRIORITY_90   = 90, /* Priority for OTAPA calls         */

  CM_ACT_PRIORITY_100  = 100, /* Priority for OTASP calls         */

  CM_ACT_PRIORITY_110  = 110, /* Priority for Data Burst Messages */

  CM_ACT_PRIORITY_120  = 120, /* Priority for GPS activity        */

  CM_ACT_PRIORITY_200  = 200, /* Default priority for all other calls */

  CM_ACT_PRIORITY_END  = 210,/* The priority when the call ends and it is 
                             ** still in the queue, this should be 
                             ** numerically higher (conceptually the lowest)
                             ** of all priorities                */
  CM_ACT_PRIORITY_PH   = 230,/* The priority for the PH entry, this is the 
                             ** lowest of all the activities     */
  CM_ACT_PRIORITY_BELOW_PH,  /* The priority after the phone object */

  CM_ACT_PRIORITY_MAX        /* For internal CM use              */

}cm_act_priority_e_type;

/* Enum of activity update reason
*/

typedef enum cm_act_update_reas_e {

  CM_ACT_UPDATE_REAS_NONE = -1, /* No Reason given, Internal use */

  CM_ACT_UPDATE_REAS_ACT_START, /* Activity started              */

  CM_ACT_UPDATE_REAS_ACT_CONT,  /* Continue with the activity    */

  CM_ACT_UPDATE_REAS_ACT_END,   /* Activity ended                */

  CM_ACT_UPDATE_REAS_USER,      /* User is forcing the pref      */

  CM_ACT_UPDATE_REAS_ACT_PWR_SAVE, /* Shut down the stack, used for handoffs */

  CM_ACT_UPDATE_REAS_MAX        /* For internal CM use           */

} cm_act_update_reas_e_type;
    

/* Structure to hold the origination parameters
*/

typedef struct cm_act_orig_s{

  cm_act_priority_e_type           act_priority;
      /* The priority of the activity                                   */

  cm_act_id_type                   act_id;
      /* Unique ID of the activity                                      */

  cm_act_update_reas_e_type        act_update_reas;
      /* The reason for this update                                     */

  sd_ss_orig_mode_e_type           orig_mode;
      /* Last origination mode sent to SD's SS_MAIN and SS_HDR.
      ** Needed for unforcing the origination mode for SS_MAIN and SS_HDR.
      */

  cm_pref_term_e_type              orig_pref_term;
      /* Indicate the current origination mode term                     */

  dword                            orig_mode_uptime;
      /* Indicate uptime for CM_MODE_TERM_CM_1_CALL_PLUS orig_mode_term */

  cm_mode_pref_e_type              orig_mode_pref;
      /* Indicate the mode preference used during call origination      */

  cm_band_pref_e_type              orig_band_pref;
      /* Indicate the band preference used during call origination      */

  cm_prl_pref_e_type               orig_prl_pref;
      /* Indicate the PRL preference used during call origination       */

  cm_roam_pref_e_type              orig_roam_pref;
      /* Indicate the roam preference used during call origination      */

  cm_hybr_pref_e_type              orig_hybr_pref;
      /* Indicate the hybr preference used during call origination      */

  sd_ss_hybr_pref_e_type           orig_int_hybr_pref;
      /* Indicate the mode preference used during call origination      */

  const cm_wlan_pref_s_type        *orig_wlan_pref;
      /* Indicate the WLAN preference used during call origination      */

  cm_srv_domain_pref_e_type        orig_srv_domain_pref;
      /* Indicate the service domain preference used during call 
      ** origination                                                    */

} cm_act_orig_s_type;


/* Type to hold all orig param.
*/
typedef struct cm_orig_q_s {

    q_link_type           link;
        /* Q linking element

           This element has to be the first
           element in this data structure
           because of removal of self_ptr
           in q_link_type
         */


    cm_act_type_e_type    act_type;
        /* Type of CM activity */

    cm_act_orig_s_type    orig;
        /* origination parameters */

} cm_orig_q_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return the type of a CM command.
*/
#define CMD_TYPE( cmd_ptr )             (cmd_ptr->cmd_type )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the call command part of a CM command.
*/
#define CALL_CMD_PTR( cmd_ptr )         ( &cmd_ptr->cmd.call )

#if (defined FEATURE_BCMCS || defined FEATURE_MODEM_MBMS)
/* Macro to return a pointer to the BCMCS command part of a CM command.
*/
#define BCMCS_CMD_PTR( cmd_ptr )        ( &cmd_ptr->cmd.bcmcs )
#endif /* FEATURE_BCMCS || MBMS */

/* Macro to return a pointer to the MBMS command part of a CM command.
*/
#define MBMS_CMD_PTR( cmd_ptr )        ( &cmd_ptr->cmd.mbms )

/* Macro to return a pointer to the phone command part of a CM command.
*/
#define PH_CMD_PTR( cmd_ptr )           ( &cmd_ptr->cmd.ph )


/* Macro to return a pointer to the serving system command part of a CM command.
*/
#define SS_CMD_PTR( cmd_ptr )           ( &cmd_ptr->cmd.ss )


/* Macro to return a pointer to the inband command part of a CM command.
*/
#define INBAND_CMD_PTR( cmd_ptr )       ( &cmd_ptr->cmd.inband )


/* Macro to return a pointer to the SMS command part of a CM command.
*/
#define SMS_CMD_PTR( cmd_ptr )          ( &cmd_ptr->cmd.sms )

#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */

#ifdef FEATURE_DBM
/* Macro to return a pointer to the DBM command part of a CM command.
*/
#define DBM_CMD_PTR( cmd_ptr )          ( &cmd_ptr->cmd.dbm_msg )
#endif /* FEATURE_DBM */

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM))
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the MC report
*/
#define MC_RPT_PTR( cmd_ptr )  ( &cmd_ptr->cmd.mc_rpt )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to return a pointer to the information part of a CM command.
*/
#define CMD_INFO_PTR( xx_cmd_ptr )          ( &xx_cmd_ptr->info )
#define CALL_CMD_INFO_PTR( xx_cmd_ptr )     ( &xx_cmd_ptr->info )

#if (defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
#error code not present
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
#define CALL_CMD_MODE_INFO_PTR( xx_cmd_ptr )    ( &xx_cmd_ptr->info.cdma_info )
#endif /* (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

#define IPCALL_CMD_INFO_PTR( xx_cmd_ptr ) ( &xx_cmd_ptr->info.ip_call )

#define CM_HOLD_CMD_TIME  2
/* Time to hold the command on the command queue
** default it to 2 seconds
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/*===========================================================================

FUNCTION cm_ph_cmd_client_act

DESCRIPTION
  Inform CM that a client was activated so that CM can inform the client
  when the phone information become available.

  This is mostly important during powerup, where a client needs to get
  the phone information in order to make powerup decisions.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
void cm_ph_cmd_client_act(

        cm_ph_cmd_cb_f_type        cmd_cb_func,
            /* client callback function */

        void                       *data_block_ptr,
            /* pointer to client callback data block */

        cm_client_id_type          client_id
            /* Requesting client */

);
/*---------------------------------------------------------------------------
                                 REPLY OBJECT
---------------------------------------------------------------------------*/


/* Type for CM reply.
** Handles the details that are associated with setting up a CM command
** to wait for a reply from MC.
*/
typedef struct cm_reply_s {

    cm_cmd_s_type              *cmd_ptr;
        /* Pointer to a CM command waiting for a reply from MC */

    cm_name_type               cm_mc_rpt;
        /* The mc reply  for which we are waiting */
    dword                      uptime;
        /* An uptime upper bound for how long we should wait
        ** for the reply to come */

} cm_reply_s_type;



/* Type to hold data burst messaging interface related information
*/
typedef struct cm_dbm_s {

    cm_pd_resp_is_within_t2m_f_type       *cm_pd_resp_is_within_t2m_func;
    /* pointer to a PD function checking if response is right away */

    /* Add SMS call back function here */
    /* pointer to a SMS function checking if response is right away */

} cm_dbm_s_type;

/*
** Type to hold the Iterator information.
*/
typedef struct cm_iterator_s {
  unsigned int next;
}cm_iterator_type;



/*===========================================================================

FUNCTION cmdbm_ptr

DESCRIPTION
  Return a pointer to the one and only dbm object.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to dbm object

SIDE EFFECTS
  none

===========================================================================*/
cm_dbm_s_type  *cmdbm_ptr( void );


/*===========================================================================
=============================================================================
============================= MC INTERFACE BLOCK ============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cm_hold_cmd_free_q_init

DESCRIPTION
  Initialize the hold command free queue.

  This function must be called before the hold command free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_free_q_init( void );


/*===========================================================================

FUNCTION cm_hold_cmd_q_init

DESCRIPTION
  Initialize the hold command queue.

  This function must be called before the hold command free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_q_init( void );




/*===========================================================================

FUNCTION cm_hold_cmd_q_get

DESCRIPTION
  Dequeue a command from the head of the CM command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cmd_s_type *cm_hold_cmd_q_get( void );




/*===========================================================================

FUNCTION cm_hold_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the CM hold command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_q_put(

    cm_cmd_s_type *cmd_ptr
        /* pointer to a command buffer to be enqueued */
);



/*===========================================================================

FUNCTION cm_hold_cmd_q_check

DESCRIPTION
  Check if the passed in command matches with the items in hold command q.
  It only checks the cmd type and cmd. It does not check for the contents
  of command, for example, the dialed digits for origination cmd.

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd are same.
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_hold_cmd_q_check(

  const cm_cmd_s_type *cmd_ptr

);

/*===========================================================================

FUNCTION cm_hold_cmd_q_add

DESCRIPTION
  Adds a command to a hold queue
  
DEPENDENCIES
  None

RETURN VALUE
None


SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_q_add
(
   const cm_cmd_s_type *cmd_ptr
);


/*===========================================================================

FUNCTION cm_hold_cmd_handle

DESCRIPTION
  Process clients' call commands

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_hold_cmd_handle(

  boolean chk_uptime,

  cm_hold_cmd_reas_e_type hold_reason

);


/*===========================================================================

FUNCTION cm_hold_cmd_q_exists_cmd

DESCRIPTION
  Check if the command type and the command are already in the hold q.
  If the NULL is passed as the command, then just the type is checked
  
DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd exists
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cm_hold_cmd_q_exists_cmd(

  cm_cmd_type_e_type cmd_type,
    /* The type of the command
    */

  const void *cmd
    /* The command 
    */
);

/*===========================================================================
FUNCTION CM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void cm_kick_dog(void);



/*===========================================================================

FUNCTION cm_cmd_alloc_ph_init

DESCRIPTION
  Allocate a CM command buffer and initialize its phone command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_s_type *cm_cmd_alloc_ph_init( void );



/*===========================================================================

FUNCTION cm_cmd_ph_init

DESCRIPTION
  Initialize the command buffer with its phone command fields

DEPENDENCIES
  The command buffer has been allocated.

RETURN VALUE
  .

SIDE EFFECTS
  none

===========================================================================*/
void cm_cmd_ph_init( cm_cmd_s_type * cmd_ptr);



/*===========================================================================

FUNCTION cm_cmd_alloc_ss_init

DESCRIPTION
  Allocate a CM command buffer and initialize its serving system command fields

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If allocation failed, a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_cmd_s_type *cm_cmd_alloc_ss_init( void );



/*===========================================================================

FUNCTION cm_nc_init

DESCRIPTION
  Initialize the CM number-classficationinterface object.

  This function must be called before the CM number-classfication interface object
  is being used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_nc_init( void );

/*===========================================================================
FUNCTION cm_m51_mode_init

DESCRIPTION
  Initialize M51 mode settings object.

  Use default handlers provided by CM for mode changes handlers.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cm_m51_mode_init( void );


/*===========================================================================

FUNCTION cm_si_init

DESCRIPTION
  Initialize the CM system indicators object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_init( void );

#if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))


/*===========================================================================

FUNCTION cm_num_assert

DESCRIPTION
  Check a series of assertions over a CM number to validate its state

DEPENDENCIES
  CM number must have already been initialized with
  cm_num_init()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_num_assert(

    const cm_num_s_type    *num_ptr
        /* Pointer to a CM number to be verified */
);
#endif // #if( defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))

/*===========================================================================
FUNCTION cm_remove_mode_pref_components

DESCRIPTION

  Remove given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will remove all single mode pref components from
  CM_MODE_PREF_AUTOMATIC and end up with CM_MODE_PREF_NONE.

  cm_mode_pref = cmcall_remove_mode_pref_components(
                                           CM_MODE_PREF_AUTOMATIC,
                                           6,// because 6 components follow
                                           SD_SS_MODE_PREF_WCDMA,
                                           SD_SS_MODE_PREF_GSM,
                                           SD_SS_MODE_PREF_GPS,
                                           SD_SS_MODE_PREF_HDR,
                                           SD_SS_MODE_PREF_CDMA,
                                           SD_SS_MODE_PREF_AMPS );

DEPENDENCIES
  sd_misc_remove_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cm_remove_mode_pref_components
(
  cm_mode_pref_e_type cm_mode_pref,
  int number_of_components_to_remove,
  ...
);

/*===========================================================================
FUNCTION cm_add_mode_pref_components

DESCRIPTION

  Add given SD mode preference component(s) from given CM mode preference.

  Example:

  The below will add a given mode pref component into existing mode pref.

DEPENDENCIES
  sd_misc_add_mode_pref()

RETURN VALUE
  Return new preference or CM_MODE_PREF_MAX if any of the mapping
  functions failed.

SIDE EFFECTS
  None

===========================================================================*/
cm_mode_pref_e_type cm_add_mode_pref_components
(
  cm_mode_pref_e_type cm_mode_pref,
  int number_of_components_to_add,
  ...
);

/*===========================================================================

FUNCTION cmtask_orig_para_insert

DESCRIPTION
  Queues the origination parameters in the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmtask_orig_para_insert( 
  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_orig_q_s_type *ph_orig_para_ptr
    /* The buffer to add to the queue
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_delete

DESCRIPTION
  Deletes the ph_orig_ptr from the origination queue
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmtask_orig_para_delete(

  cm_ss_e_type               ss,
     /* The ss, used to select a queue
     */

   cm_orig_q_s_type          *ph_orig_ptr,
     /* The buffer to delete from the queue
     */

   boolean                    free_buf
     /* Return the buffer to the free Q
     */

);


/*===========================================================================

FUNCTION cmtask_change_act_priority

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmtask_orig_para_change_act_priority( 

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type    act_type,
    /* Type of CM activity 
    */

  cm_act_priority_e_type  curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type  new_priority,
    /* The new priority of the activity
    */        

  boolean  global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */

);


/*===========================================================================

FUNCTION cmtask_orig_para_change_act_priority

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean cmtask_orig_para_change_act_priority_3( 

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_id_type             act_id,
    /* Type of CM activity 
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority
    /* The new priority of the activity
    */        
);

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id

DESCRIPTION
  Searches the orig queue for the act_id
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found 
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_orig_q_s_type* cmtask_orig_para_search_act_id(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_id_type act_id
    /* The buffer to add to the queue
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type

DESCRIPTION
  Searches the orig queue for the act_type
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found 
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_orig_q_s_type* cmtask_orig_para_search_act_type(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_type_e_type act_type
    /* The buffer to add to the queue
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_act_priority

DESCRIPTION
  Searches the orig queue for the act_type
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found 
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_orig_q_s_type* cmtask_orig_para_search_act_type_act_priority(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type act_type,
    /* The act_type to search in the queue
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_get_top

DESCRIPTION
  Returns the pointer to the top element of the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the top element if present
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_orig_q_s_type* cmtask_orig_para_get_top( 
  cm_ss_e_type ss
    /* The ss whose top element is required
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_get_first

DESCRIPTION
  Return a pointer to first orig_para in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with 
  cm_ph_orig_xxx_q_init().

RETURN VALUE
  Return a pointer to first buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_orig_q_s_type* cmtask_orig_para_get_first( 

  cm_ss_e_type ss
    /* The ss for which the first element is needed
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_get_next

DESCRIPTION
  Return a pointer to next orig_param in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with 
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  Return a pointer to next buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_orig_q_s_type* cmtask_orig_para_get_next( 

  cm_orig_q_s_type* q_ptr,
    /* Pointer to the current q element
    */

  cm_ss_e_type ss 
    /* The ss for which the next element is needed
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_alloc

DESCRIPTION
  Allocate a CM command buffer from the CM free hold command queue.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_orig_q_s_type *cmtask_orig_para_alloc( cm_ss_e_type ss );


/*===========================================================================

FUNCTION cmtask_orig_para_delloc

DESCRIPTION
  Dellocate a CM orig buffer.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmtask_orig_para_delloc ( 
  cm_ss_e_type ss, 
    /* The queue to deque from
    */
  cm_orig_q_s_type *q_ptr 
    /* The pointer to the buffer
    */
);


/*===========================================================================

FUNCTION cmtask_orig_para_reset

DESCRIPTION
  Reset the origination parameters queue

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with 
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

extern void cmtask_orig_para_reset(
  cm_ss_e_type ss
    /* The ss for which the Q is to be reset 
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_count

DESCRIPTION
  Returns the number of elements in the queue
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  
  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

extern int cmtask_orig_para_count(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
);

/*===========================================================================

FUNCTION cmtask_orig_para_count_2

DESCRIPTION
  Returns the number of elements in the queue
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  
  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

extern int cmtask_orig_para_count_2(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_act_priority_e_type act_priority,
    /* The priority of the activity
    */

  cm_cmp_criteria_e_type cmp_criteria
    /* Comparision criteria
    */
);


/*===========================================================================

FUNCTION cmtask_compute_srv_domain_pref

DESCRIPTION
  Gets the union of all the orig_srv_domain_pref 
  
DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The union of all the orig_srv_domain_pref 
  
SIDE EFFECTS
  None.

===========================================================================*/

extern cm_srv_domain_pref_e_type cmtask_compute_srv_domain_pref(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
);




/*===========================================================================

FUNCTION cmtask_orig_para_init_2

DESCRIPTION
  Initialize the buffer to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmtask_orig_para_init_2(
   cm_act_orig_s_type *orig_ptr
    /* The buffer to initialize
    */
);


/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_on

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is set

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cm_dip_switch_set_ddtm_on(void);


/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_off

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is set

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cm_dip_switch_set_ddtm_off(void);

#endif /* CMI_H */

