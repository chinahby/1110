#ifndef CMIPAPP_H
#define CMIPAPP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

               C A L L   M A N A G E R   I P   A P P L I C A T I O N

                        H E A D E R   F I L E

DESCRIPTION

   This header describes functions that interface an application (VoIP) to
   Call Manager. Call Manager can interact with the application to place
   calls on whatever technology application may support. This way current
   Call Manager interfaces to its clients need not change and at the same
   time clients get to make use of new technology.

  Copyright (c) 2004 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1_QSC1110_3.X/inc/cmipapp.h#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/09   mp      Added support to remove the last party added for VOIP calls
11/13/08   rn/cl   Corrected metacomment for below fix
10/30/08   jd      Added changes for IMS registration
10/27/08   cl      Added support for KTFT Voip Call Waiting Feature
05/23/08   aj      Adding metacomments for IMS support. 
                   Comment out cmipapp_get_ph_identity
03/29/08   rm      Adding UMTS interface support for cmipapp
12/07/07   ra      Added support for IMS VCC in HOMER files 
06/21/07   jqi     Fixed meta comemnts for cmipapp_ind_u_type. 
06/18/07   ic      Added meta comments for cmipapp_sms_cmd_mo_sms_f_type
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
01/12/07   ka      Adding metacomments.
01/09/07   ka      Changing 0xFFFFFFFF to 0x7FFFFFFF to satisfy C++ compilers
12/25/06   ka      Including customer.h
08/16/06   ic      Fixed compiler warnings (comment inside comment)
07/31/06   ka      Adding Explicit call transfer and Multiparty support for
                   VoIP.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
05/07/06   ka      Adding callback functions for activating/deactivating 
                   answer hold
02/17/06   ka      Adding support for videoshare
12/11/05   ic      Lint cleanup
11/23/05   ka      Modification to allow hold/active/dtmf for ip calls.
10/08/05   ic      Added Header: field 
09/26/05   ka      Adding structure cmipapp_ind_mt_ring_cnf_s.
04/20/05   ka      Informing IP apps when there is a service change
04/06/05   ka      Removed mo ring back structure, adding sys_mode to call
                   orig and removed headers to find_active_app() and
                   get_cb_ptrs ()
03/14/05   ka      Cleaning hold, retrieve and other structures
08/27/04   dk/ka   Initial release.
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "customer.h"  /* Customer configuration file */

#if defined(FEATURE_IP_CALL)

#include "cm.h"

#include "cmll.h"   /* Includes cm_hdr_type, cm_name_e_type */

#include "cmutil.h" /* cmipapp_cause_e_type */

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Maximum length of identity buffer
*/
#define CMIPAPP_ID_BUFFER                        64

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* List of values for call progress indication
** 1xx messages need not be reported to user
** 3xx message support in form of CMIPAPP_PROG_REDIRECT is sufficient
** 4xx messages lead to call getting ended
*/
typedef enum
{
  CMIPAPP_PROG_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_PROG_SESSION,
    /* Message sent between trying and alerting during MO call.
    ** SIP is not giving us this detail currently, working with
    ** them to push this detail up.
    */

  CMIPAPP_PROG_FORWARD,
    /* User or network decided to forward the call to a diff number
    */

  CMIPAPP_PROG_REDIRECT,
    /* Indicates MO call is getting redirected
    */

  CMIPAPP_PROG_MO_RINGBACK,
    /* Indicates Remote user is being alerted
    */

  CMIPAPP_PROG_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_prog_e_type;

/* List of causes for call end from lower layers.
** error_code makes sense when cause is not NORMAL
**
**
** !!!! NEED TO START USING cm_ipapp_cause_e_type from cmutil.h and
** remove this enum declaration. !!!!
*/
typedef enum
{

  CMIPAPP_END_CAUSE_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_END_CAUSE_NORMAL = 0,
    /* Indicates user requested call end.
    **
    ** Cause also returned when ipapp completes
    ** operation on call hold/retrieve     
    ** successfully.
    */

  CMIPAPP_END_CAUSE_GENERAL_FAILURE = 1,
    /* Used by CM and app to map end causes
    ** not listed in this enumeration.
    */

  /* Call end cause due to redirectionx */
  CMIPAPP_END_CAUSE_MULTIPLE_CHOICES  = 300,
    /* Request resolved in several choices, each with
    ** its own specific location. User can select from
    ** these to reidrect request
    */

  CMIPAPP_END_CAUSE_MOVED_PERMANENTLY = 301,
   /* User is no longer at the requested address and
   ** client should retry at new address given
   */


  CMIPAPP_END_CAUSE_MOVED_TEMPORARILY = 302,
    /* Requesting client should retry request at
    ** new address given. Expires header field gives
    ** the expiration time
    */


  CMIPAPP_END_CAUSE_USE_PROXY = 305,
    /* Requested resource needs to be accessed
    ** through a proxy given by the contact field
    */


  CMIPAPP_END_CAUSE_ALTERNATE_SERVICE = 380,
    /* Call was not successful, but alternate services
    ** are possible
    */


  /* Call end due to Request Failure 4xx */

  CMIPAPP_END_CAUSE_BAD_REQUEST  = 400,
    /* Malformed syntax in request
    */

  CMIPAPP_END_CAUSE_UNAUTHORIZED = 401,
    /* Request requires user
    ** authentication
    */

  CMIPAPP_END_CAUSE_PAYMENT_REQUIRED = 402,
    /* Future use
    */

  CMIPAPP_END_CAUSE_FORBIDDEN        = 403,
    /* Server refusing to fulfill request
    ** Request should not be repeated,
    */

  CMIPAPP_END_CAUSE_NOT_FOUND        = 404,
    /* User does not exist at domain
    ** specified or Domain in requested
    ** URI does not match domains handled by
    ** recipient of the request
    */

  CMIPAPP_END_CAUSE_METHOD_NOT_ALLOWED = 405,
    /* Method requested in address line
    ** not allowed for address identified
    ** by request-URI
    */

  CMIPAPP_END_CAUSE_NOT_ACCEPTABLE     = 406,
    /* Resource identified by request can
    ** only generate resp with content
    ** not acceptable
    */

  CMIPAPP_END_CAUSE_PROXY_AUTHENTICATION_REQUIRED = 407,
    /* Client must first authenticate with proxy
    */

  CMIPAPP_END_CAUSE_REQUEST_TIMEOUT = 408,
    /* Server could not respond within
    ** suitable amount of time
    */

  CMIPAPP_END_CAUSE_GONE = 410,
    /* Requested resource no longer available
    ** at server and no forwarding address
    */

  CMIPAPP_END_CAUSE_REQUEST_ENTITY_TOO_LARGE = 413,
    /* Request entity body is larger than what
    ** server is willing to process
    */

  CMIPAPP_END_CAUSE_REQUEST_URI_TOO_LARGE    = 414,
    /* Server refusing to service since request-URI
    ** is longer than server willing to interpret.
    */

  CMIPAPP_END_CAUSE_UNSUPPORTED_MEDIA_TYPE   = 415,
    /* Message body of request not in format
    ** supported by server for requested method
    */

  CMIPAPP_END_CAUSE_UNSUPPORTED_URI_SCHEME   = 416,
    /* Unsupported URI scheme
    */

  CMIPAPP_END_CAUSE_BAD_EXTENSION            = 420,
    /* Server did not understand protocol exten
    ** specified in proxy-required or require
    ** header field.
    */

  CMIPAPP_END_CAUSE_EXTENSION_REQUIRED       = 421,
    /* Extension to process a request is not
    ** listed in supported header field in
    ** request
    */

  CMIPAPP_END_CAUSE_INTERVAL_TOO_BRIEF       = 422,
    /* Expiration time of resource refreshed by
    ** request is too short
    */

  CMIPAPP_END_CAUSE_TEMPORARILY_UNAVAILABLE  = 480,
    /* End system contacted but callee is currently
    ** unavailable
    */

  CMIPAPP_END_CAUSE_CALL_OR_TRANS_DOES_NOT_EXIST = 481,
    /* Request received by UAS does not match
    ** any existing dialog or transaction
    */

  CMIPAPP_END_CAUSE_LOOP_DETECTED              = 482,
    /* Server detected a loop
    */

  CMIPAPP_END_CAUSE_TOO_MANY_HOPS              = 483,
    /* Request received has Max-Forwards header
    ** field at 0
    */

  CMIPAPP_END_CAUSE_ADDRESS_INCOMPLETE         = 484,
    /* Request had incomplete URI
    */

  CMIPAPP_END_CAUSE_AMBIGUOUS                  = 485,
    /* Request URI was ambiguous
    */

  CMIPAPP_END_CAUSE_BUSY_HERE                  = 486,
    /* Callee is not willing/unable to take
    ** additional calls at this end system
    */


  CMIPAPP_END_CAUSE_REQUEST_TERMINATED         = 487,
    /* Request terminated by a BYE/Cancel
    */

  CMIPAPP_END_CAUSE_NOT_ACCEPTABLE_HERE        = 488,
    /* Resource requested by request-URI not
    ** acceptable
    */

  CMIPAPP_END_CAUSE_REQUEST_PENDING            = 491,
    /* Request was received by UAS that had
    ** a pending request within same dialog
    */

  CMIPAPP_END_CAUSE_UNDECIPHERABLE             = 493,
    /* Request has an encrypted MIME body for which
    ** the recipient does not possess appropriate
    ** decryption key
    */


  /* Call end due to Server Failure 5xx */
  CMIPAPP_END_CAUSE_SERVER_INTERNAL_ERROR      = 500,
    /* Server hit an unexpected condition that
    ** prevented it from fulfilling the request
    */

  CMIPAPP_END_CAUSE_NOT_IMPLEMENTED            = 501,
    /* Server does not support functionality to
    ** fulfill request
    */

  CMIPAPP_END_CAUSE_BAD_GATEWAY                = 502,
    /* Server received invalid response from
    ** downstream gateway
    */

  CMIPAPP_END_CAUSE_SERVER_UNAVAILABLE         = 503,
    /* Server is temporarily unable to process the
    ** request due to overloading or maintanence
    */

  CMIPAPP_END_CAUSE_SERVER_TIME_OUT            = 504,
    /* Server did not receive timely response from
    ** external server it accessed
    */

  CMIPAPP_END_CAUSE_VERSION_NOT_SUPPORTED      = 505,
    /* Server does not support SIP protocol version
    ** used in the request
    */

  CMIPAPP_END_CAUSE_MESSAGE_TOO_LARGE          = 513,
    /* Server unable to process request since message
    ** length exceeded capabilities
    */

  /* Call end due to Global Failure 6xx */
  CMIPAPP_END_CAUSE_BUSY_EVERYWHERE            = 600,
    /* Callee's end system was contacted but callee
    ** is busy and does not wish to take call
    */

  CMIPAPP_END_CAUSE_DECLINE                    = 603,
    /* Callee's end system was contacted but user
    ** explicitly does not wish to or cannot
    ** participate.
    */

  CMIPAPP_END_CAUSE_DOES_NOT_EXIST_ANYWHERE    = 604,
    /* Server has information that user indicated in
    ** request-URI does not exist anywhere
    */

  CMIPAPP_END_CAUSE_SESS_DESCR_NOT_ACCEPTABLE  = 605,
    /* User's agent contacted but some aspects of
    ** session description not acceptable
    */

  CMIPAPP_END_CAUSE_MAX = 0x7FFFFFFF
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_end_cause_e_type;

/* List of reasons given by user for rejecting an incoming call
*/
typedef enum
{

  CMIPAPP_INCOM_REJ_CAUSE_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_INCOM_REJ_CAUSE_BUSY,
    /* Indicates user requested call end
    */

  CMIPAPP_INCOM_REJ_CAUSE_ANOTHER_CALL,
    /* Indicates user was in another call
    */

  CMIPAPP_INCOM_REJ_CAUSE_ONLY_VOICE,
    /* Accept voice and reject video
    */

  CMIPAPP_INCOM_REJ_CAUSE_ONLY_VIDEO,
    /* Accept video and reject voice
    */

  CMIPAPP_INCOM_REJ_CAUSE_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_incom_rej_cause_e_type;


/* Bit Masks of system modes app can support.
** Combination of system modes can be created
** by using the macro ADD_SYSMODE
*/
typedef enum
{

  CMIPAPP_SYS_MODE_NONE        = SYS_SYS_MODE_MASK_NONE,
                                /* Acquire no system */

  CMIPAPP_SYS_MODE_NO_SRV      = SYS_SYS_MODE_MASK_NO_SRV,
                                /* No service */

  CMIPAPP_SYS_MODE_CDMA        = SYS_SYS_MODE_MASK_CDMA,
                                /* CDMA mode systems only */

  CMIPAPP_SYS_MODE_HDR         = SYS_SYS_MODE_MASK_HDR,
                                /* Acquire HDR mode systems only */

  CMIPAPP_SYS_MODE_GSM         = SYS_SYS_MODE_MASK_GSM,
                                /* Acquire GSM systems only */

  CMIPAPP_SYS_MODE_WCDMA       = SYS_SYS_MODE_MASK_WCDMA,
                                /* Acquire WCDMA systems only */

  CMIPAPP_SYS_MODE_WLAN        = SYS_SYS_MODE_MASK_WLAN,
                                /* Acquire WLAN systems only */

  CMIPAPP_SYS_MODE_MAX         = SYS_SYS_MODE_MASK_MAX

} cmipapp_sys_mode_e_type;



/* Return status for app registration
*/
typedef enum
{
  CMIPAPP_STATUS_NONE        = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_STATUS_OK,
    /* request was completed successfully
    */

  CMIPAPP_STATUS_CB_REG_FAILED,
    /* Call to register app call backs failed
    */

  CMIPAPP_STATUS_ID_DEALLOC_FAILED,
    /* Call to deallocate app id failed
    */

  CMIPAPP_STATUS_GET_PH_IDENTITY_FAILED,
    /* call to get phone identity failed
    */

  CMIPAPP_STATUS_CALL_TYPE_REG_FAILED,
    /* Request to register app call type failed
    */

  CMIPAPP_STATUS_INFORM_FAILED,
    /* call to inform app status failed
    */

  CMIPAPP_STATUS_APPID_PTR_ERR,
    /* call to get app id failed due to null
    ** parameter
    */

  CMIPAPP_STATUS_APPID_EXHAUST,
    /* No more free app ids available
    */

  CMIPAPP_STATUS_DUP_APP_EXISTS,
    /* Duplicate app exists
    */

  CMIPAPP_STATUS_GET_CB_PTRS_FAILED,
    /* Action to get Apps function ptr Failed
    */

  CMIPAPP_STATUS_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_status_e_type;


/* App active status
*/
typedef enum
{
  CMIPAPP_RUN_STATUS_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */


  CMIPAPP_RUN_STATUS_INACTIVE,
    /* App is inactive. App can be
    ** regist/deregist but cannot make
    ** a call.
    */

  CMIPAPP_RUN_STATUS_ACTIVE,
    /* App is active. App is registered
    ** and can make a call.
    */  

  CMIPAPP_RUN_STATUS_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_run_status_e_type;


/* From and To technology in hand over operation
*/
typedef enum
{
  CMIPAPP_HO_TYPE_NONE = -1,
    /* FOR INTERNAL USE OF CM ONLY!
    */

  CMIPAPP_HO_TYPE_DO_1X,
    /* HDR to CDMA handoff
    */

  CMIPAPP_HO_TYPE_UMTS_1X,
    /* UMTS to CDMA handoff
    */

  CMIPAPP_HO_TYPE_CS_WLAN,
    /* CS to WLAN handoff
    */

  CMIPAPP_HO_TYPE_MAX
    /* FOR INTERNAL USE OF CM ONLY!
    */

} cmipapp_ho_type_e_type;

/* Each app talks to CM with a pre registered id */
typedef uint8 cmipapp_id_type;

/* call type mask used by cmipapp. Maintained for backward compatibility.
** cm_call_type_mask_type got added later.
*/
typedef cm_call_type_mask_type cmipapp_call_type_e_type;


typedef uint32 cmipapp_trans_id_type;
/* Type to define trans id used
** between CM and ipapps
*/

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/* Utility to create combination of app capabilities. Maintaining it for
** backward compatibility with old clients. New approach is to directly
** use ADD_CALL_TYPE_MASK from cm.h
*/
#define ADD_CALLTYPE(X,Y)  (cmipapp_call_type_e_type) ADD_CALL_TYPE_MASK(X,Y)

/* Utility to create combination of system modes app can work with */
#define ADD_SYSMODE (X,Y)   (cmipapp_sys_mode_e_type) (((int)X) | ((int)Y))

/* Invalid app id
*/
#define CMIPAPP_ID_INVALID ((cmipapp_id_type) 0xFF)


/* cmipapp call types that are already being used by clients. These were 
** introduced before ip app's could include cm.h. If cm.h is included by 
** app's then use CM_CALL_TYPE_X_MASK instead.
*/

#define CMIPAPP_CALL_TYPE_NONE       0
    /* FOR INTERNAL USE OF CM ONLY!
    ** Since it is assigned to bit mask -1 is not used
    */

#define CMIPAPP_CALL_TYPE_VOICE      CM_CALL_TYPE_VOICE_MASK
    /* Voice call
    */

#define CMIPAPP_CALL_TYPE_VT         CM_CALL_TYPE_VT_MASK
    /* VT call
    */

#define CMIPAPP_CALL_TYPE_VS         CM_CALL_TYPE_VS_MASK
    /* VS call
    */

#define CMIPAPP_CALL_TYPE_SMS        CM_CALL_TYPE_SMS_MASK
    /* SMS
    */

#define CMIPAPP_CALL_TYPE_VOICE_VT   ADD_CALL_TYPE_MASK(CMIPAPP_CALL_TYPE_VOICE,CMIPAPP_CALL_TYPE_VT)
    /* Both voice and VT
    */

#define CMIPAPP_CALL_TYPE_VOICE_VS   ADD_CALL_TYPE_MASK(CMIPAPP_CALL_TYPE_VOICE,CMIPAPP_CALL_TYPE_VS)
    /* Both voice and VS
    */ 


    /* The CMIPAPP_CALL_TYPE_REG is not associated any with CM_CALL_TYPE_XXX. 
    */
#define CMIPAPP_CALL_TYPE_REG        SYS_BM_64BIT(CM_CALL_TYPE_MAX + 1)
    /* Registration for CMIPAPP Events
    */

#define CMIPAPP_CALL_TYPE_MAX        SYS_BM_64BIT(CM_CALL_TYPE_MAX + 2)
    /* FOR INTERNAL USE OF CM ONLY!
    */


#define CMIPAPP_TRANS_ID_NONE        (cmipapp_trans_id_type) (-1)
/* CM-APP Trans id initializer */

/**--------------------------------------------------------------------------
** Datatypes - Report structures
** --------------------------------------------------------------------------
*/



/* Structure holding International Mobile Subscriber Identity
*/
typedef struct
{

   byte                         buffer[CMIPAPP_ID_BUFFER];
     /* Carries the imsi
     */

   int                          length;
     /* length gives the size of buffer in bytes
     */

} cmipapp_identity_s_type;



/* Message is sent to indicate called party is being alerted
** for mobile originated call.
** Generic message to report provisional 1xx and redirection 3xx
** messages.
*/
typedef struct cmipapp_ind_mo_progr_info_s
{

  cm_call_id_type                id;
    /* call id of MO call
    */

  cmipapp_prog_e_type           progr;
    /* Indicates call progress
    */

  cm_num_s_type                  num;
    /* Gives remote number that is being
    ** alerted
    */

} cmipapp_ind_mo_progr_info_s_type;

/* Message is sent to indicate set up of an incoming call
** INVITE message consists of only the calling party identity
** in SIP format "sip:4455666@sip.internet.com"
*/
typedef struct cmipapp_ind_mt_invite_s
{

  cm_call_id_type           id;
    /* call id of incoming call
    */

  cmipapp_id_type           app_id;
    /* App that is placing the call
    */

  cm_num_s_type             num;
    /* Phone number of calling party
    */

  cmipapp_call_type_e_type  call_type;
    /* Call type of incoming call
    */

  cmipapp_sys_mode_e_type   sys_mode;
    /* Sytem mode of incoming call
    */

} cmipapp_ind_mt_invite_s_type;

/* UI responds to MT invite with setup response.
** Set up response sends 180 Ringing to remote user.
** Remote user sends an ack for 180 ringing.
**
** UI is informed of this ack with mt_ring_cnf.
*/
typedef struct cmipapp_ind_mt_ring_cnf_s
{

  cm_call_id_type           id;
    /* call id of incoming call
    */

  cmipapp_sys_mode_e_type   sys_mode;
    /* Sytem mode of incoming call
    */

} cmipapp_ind_mt_ring_cnf_s_type;



/* Message is sent for both MO and MT after call gets connected
*/
typedef struct cmipapp_ind_connected_s
{

  cm_call_id_type          id;
    /* call id of connected call
    */

  cm_num_s_type            num;
    /* Remote number call is connected to
    */

  cmipapp_sys_mode_e_type  sys_mode;
    /* Sytem mode of incoming call
    */

} cmipapp_ind_connected_s_type;


/* Message sent to indicate call end
** error_code is valid when end_cause is not NORMAL
*/
typedef struct cmipapp_ind_call_end_s
{

  cm_call_id_type              id;
    /* call id of ended call
    */

  cmipapp_end_cause_e_type    end_cause;
    /* reason for call getting ended
    */

  cm_ipapp_warn_info_s_type   warn_info;
    /* Warning info for ending the call
    */

  char                        reason[CM_IPAPP_TEXT_SIZE];
    /* Null terminated reason string 
    ** RFC 3326
    ** Does not mention any specific reason string lengths
    */

} cmipapp_ind_call_end_s_type;

/* Message indicating call being put on hold
*/
typedef struct cmipapp_ind_hold_s
{

  cm_call_id_type              id;
    /* call id of ended call
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of hold operation
    */

} cmipapp_ind_hold_s_type;

/* Message indicating call being retrieved
*/
typedef struct cmipapp_ind_retrieve_s
{

  cm_call_id_type              id;
    /* call id of ended call
    */

  cm_ipapp_cause_e_type        cause;
    /* sip cause reflecting result of retrieve operation
    */

} cmipapp_ind_retrieve_s_type;

/*------------------------------------------------------------------------*/

/* Message indicating videoshare availability
*/
typedef struct cmipapp_ind_vs_status
{

  cm_call_id_type              id;
    /* call id of voice call on which
    ** videoshare is possible
    */

  boolean                      is_vs_poss;
    /* TRUE   - videoshare is possible on voice call
    ** FALSE  - videoshare is not possible on voice call
    */

} cmipapp_ind_vs_status_s_type;

/*------------------------------------------------------------------------*/

/* Message indicating success/failure on the call related
** sups command. Call dependant supp service works on more
** than one call. Conf is not sent with respect to a particular
** call id.
*/
typedef struct cmipapp_ind_sups_conf
{  

  
  cm_call_sups_type_e_type     sups_type;
    /* Gives the supp service req that 
    ** is generating this conf.
    */

  boolean                      call_success;
    /* TRUE   - Sups request on call id is successfull.
    ** FALSE  - Sups request on call id failed.
    */

} cmipapp_ind_sups_conf_s_type;

/*------------------------------------------------------------------------*/

/* Message sent to indicate that current call has been transferred
** to another party
*/
typedef struct cmipapp_ind_call_transfer
{

  cm_call_id_type              id;
    /* call that has been transferred to a different party.
    */  

} cmipapp_ind_call_transfer_s_type;


/*------------------------------------------------------------------------*/

/* Message sent to update remote number that current call 
** is connected to.
*/
typedef struct cmipapp_ind_connected_num
{

  cm_call_id_type              id;
    /* call that needs to update number of remote party.
    */  

  cm_num_s_type                num;
    /* Remote number call is connected to
    */

} cmipapp_ind_connected_num_s_type;


/*------------------------------------------------------------------------*/

/* Message sent to indicate the MO SMS status
*/
typedef struct cmipapp_ind_mo_sms_status
{
  cm_sms_mo_status_e_type      status;
    /* The MO SMS status 
    */
} cmipapp_ind_mo_sms_status_s_type;

/*------------------------------------------------------------------------*/

/* Message sent for the contents of Incoming SMS.
*/
typedef cm_mt_sms_msg_type cmipapp_ind_mt_sms_s_type;

/* Indication sent for the waiting VOIP call */
typedef struct cmipapp_ind_wait_voip_call
{
  /* Call id */
  cm_call_id_type                 call_id;

  /* Calling number
  */
  cm_num_s_type                   num;

} cmipapp_ind_wait_voip_call_type;

/*------------------------------------------------------------------------*/

/* Union of all lower layer report types.
*/
typedef union {

    /* Message is sent to indicate set up of an incoming call
    */
    cmipapp_ind_mt_invite_s_type               mt_invite;

    /* Message is sent after remote user acks 180 ringing.
    ** local user can be alerted (ring) at this point.
    */
    cmipapp_ind_mt_ring_cnf_s_type             mt_ring_cnf;

    /* Message indicates progress of mobile originated call
    */
    cmipapp_ind_mo_progr_info_s_type           mo_progr_info;

    /* Message is sent after call gets connected
    */
    cmipapp_ind_connected_s_type               connected;

    /* Message sent to indicate call end
    */
    cmipapp_ind_call_end_s_type                call_end;

    /* Message sent to indicate call being put on hold
    */
    cmipapp_ind_hold_s_type                    hold;

    /* Message sent to indicate call being made active
    */
    cmipapp_ind_retrieve_s_type                retrieve;    

    /* Message sent to indicate videoshare availability for
    ** a voice call
    */
    cmipapp_ind_vs_status_s_type                 vs_status;

    /* Message sent to indicate success/failure of sups request       
    */
    cmipapp_ind_sups_conf_s_type                sups_conf ;

    /* Message sent to indicate that current call has been transferred
    ** to another party
    */
    cmipapp_ind_call_transfer_s_type            call_transfer;     
    
    /* Message sent to update remote number current call 
    ** is connected to.
    */
    cmipapp_ind_connected_num_s_type            connected_num;     

    /* Message sent to indicate the SMS status
    */
    cmipapp_ind_mo_sms_status_s_type            mo_sms_status;

    /* Message sent for Incoming SMS status
    */
    cmipapp_ind_mt_sms_s_type                   mt_sms;

    /* Call Waiting Report */
    cmipapp_ind_wait_voip_call_type             call_waiting;

} cmipapp_ind_u_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CASE CM_IP_CALL_IND_MT_INVITE      cmipapp_ind_u_type.mt_invite     */
/*~ CASE CM_IP_CALL_IND_MT_RING_CNF    cmipapp_ind_u_type.mt_ring_cnf   */
/*~ CASE CM_IP_CALL_IND_MO_PROGR_INFO  cmipapp_ind_u_type.mo_progr_info */
/*~ CASE CM_IP_CALL_IND_CONNECTED      cmipapp_ind_u_type.connected     */
/*~ CASE CM_IP_CALL_IND_CALL_END       cmipapp_ind_u_type.call_end      */
/*~ CASE CM_IP_CALL_IND_CALL_HOLD      cmipapp_ind_u_type.hold          */
/*~ CASE CM_IP_CALL_IND_CALL_RETRIEVE  cmipapp_ind_u_type.retrieve      */
/*~ CASE CM_IP_CALL_IND_VS_STATUS      cmipapp_ind_u_type.vs_status     */
/*~ CASE CM_IP_CALL_IND_SUPS_CONF      cmipapp_ind_u_type.sups_conf     */
/*~ CASE CM_IP_CALL_IND_TRANSFER       cmipapp_ind_u_type.call_transfer */
/*~ CASE CM_IP_CALL_IND_CONNECTED_NUM  cmipapp_ind_u_type.connected_num */
/*~ CASE CM_IP_SMS_IND_MO_SMS_STATUS   cmipapp_ind_u_type.mo_sms_status */
/*~ CASE CM_IP_SMS_IND_MT_SMS          cmipapp_ind_u_type.mt_sms        */
/*~ CASE CM_IP_CALL_IND_CALL_WAIT      cmipapp_ind_u_type.call_waiting  */
#endif

/* The header is always present and
** it specifies the command type and attributes.  If the command has
** arguments they follow in the union.
*/
typedef struct {

  /* Header for reports to CM
  */
  cm_hdr_type      hdr;

  /* Union of all possible indications from lower layers
  */
  cmipapp_ind_u_type ip_ind;


  /* Token created and used by registered apps.
  ** App gives CM this value whenever cmipapp_rpt_ind () is called.
  ** CM passes the most recent value given by app while calling
  ** app registered call back functions
  */
  uint32             data_block;


} cmipapp_rpt_s_type;


/* Used to inform apps of current service availability in CM
*/
typedef struct {

  sys_srv_status_e_type  evdo_srv_status;
  sys_srv_status_e_type  wlan_srv_status;
  sys_srv_status_e_type  umts_srv_status;

} cmipapp_srv_status_s_type;

/**--------------------------------------------------------------------------
** Datatypes - Call back Functions
** --------------------------------------------------------------------------
*/


/*  Call back function called by CM to send
**  call origination request.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean   (cmipapp_cmd_orig_f_type) (

  const cm_num_s_type      called_num,
    /* Number dialed - phone that is being called
    */

  const cm_call_id_type    call_id,
    /* call ID of the call
    */

  cmipapp_call_type_e_type call_type,
    /* call type for the call
    */

  cmipapp_sys_mode_e_type  sys_mode,
    /* Sys mode call needs to be originated on.
    ** Possible use when multiple systems WLAN, EVDO
    ** are available
    */

  uint32                   data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_orig_f_type
    ONERROR return FALSE */
#endif

/*  Call back function called by CM to answer
**  an incoming call.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_answer_f_type) (

  const cm_call_id_type            call_id,
    /* call id of incoming call
    */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Valid when accept is set to FALSE
    */

  uint32                           data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_answer_f_type
    ONERROR return FALSE */
#endif


/*  Call back function called by CM to indicate
**  decision of CM client to accept or reject call.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_invite_res_f_type) (

  cm_call_id_type                  call_id,
    /* call id of the call
    */

  boolean                          accept,
    /* Flag indicating whether the call is accepted or not
    */

  cmipapp_incom_rej_cause_e_type   answer_cause,
    /* Valid when accept is set to FALSE
    */

  uint32                           data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_invite_res_f_type
    ONERROR return FALSE */
#endif

/* Call back function called by CM to request
** ending a call
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_end_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_end_f_type
    ONERROR return FALSE */
#endif

/*  Call back function called by CM to start dtmf.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_inband_cmd_start_cont_dtmf_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint8           digit,
    /* DTMF digit
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_inband_cmd_start_cont_dtmf_f_type
    ONERROR return FALSE */
#endif

/*  Call back function called by CM to stop dtmf.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_inband_cmd_stop_cont_dtmf_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  uint16          duration,
    /* DTMF duration
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_inband_cmd_stop_cont_dtmf_f_type
    ONERROR return FALSE */
#endif

/* Call back function for CM to put call on hold
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_hold_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_hold_f_type
    ONERROR return FALSE */
#endif

/* Call back function for CM to put call active
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_active_f_type) (

  const cm_call_id_type call_id,
    /* call id
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_active_f_type
    ONERROR return FALSE */
#endif

/* Call back function for CM to modify call type
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_modify_f_type) (

  const cm_call_id_type           call_id,
    /* call id
    */

  cmipapp_call_type_e_type        cap,
    /* Call type to modify the call into
    */

  uint32                          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_modify_f_type
    ONERROR return FALSE */
#endif

/*  Call back function called by CM to inform client app
**  of change in serving system parameters.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_srv_sys_info_f_type) (


  cmipapp_srv_status_s_type       *srv_status_info
    /* Serving system information
    */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_srv_sys_info_f_type
    ONERROR return FALSE */
/*~ PARAM srv_status_info POINTER */
#endif

/*  Function called into ip app for activating answer hold.
*/
typedef boolean (cmipapp_cmd_act_answer_hold_f_type) (


  const cm_call_id_type           call_id,
    /* call id
    */  

  uint32                          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_act_answer_hold_f_type
    ONERROR return FALSE */
#endif

/*  Function called into ip app for deactivating answer hold.
*/
typedef boolean (cmipapp_cmd_deact_answer_hold_f_type) (


  const cm_call_id_type           call_id,
    /* call id
    */  

  uint32                          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_deact_answer_hold_f_type
    ONERROR return FALSE */
#endif


/*  Function called into ip app for deactivating answer hold.
*/
typedef boolean (cmipapp_cmd_answer_call_waiting_f_type) (


  const cm_call_id_type           call_id,
    /* call id
    */  

  uint32                          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_answer_call_waiting_f_type
    ONERROR return FALSE */
#endif

/*  Call back function called by CM to indicate flash event.
**  TRUE is returned on successfull operation, FALSE otherwise
*/
typedef boolean (cmipapp_cmd_simple_flash_f_type) (

  const cm_call_id_type call_id,
    /* call id of the call
    */

  const cm_mm_call_info_s_type     *call_info_ptr,
    /* call info pointer
    */

  uint32          data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_simple_flash_f_type
    ONERROR return FALSE */
#endif

/*  ECT supplementary service enables the served mobile subscriber 
**  (subscriber A) who has two calls , each of which can be an incoming or 
**  outgoing call, to connect the other parties in the two calls and release 
**  the served mobile subscriber's own connection
**
**  Example - call1 is active, call2 is on hold, Sending
**  ect connects remote parties of call1 and call2. Phone initiating
**  ect will finally receive call end on both the calls
**  TRUE is returned on successfull operation, FALSE otherwise.
**
**  Transfer acts over multiple calls and since data_block is call specific,
**  no data_block is passed.
*/
typedef boolean (cmipapp_cmd_ect_f_type) (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_ect_f_type
    ONERROR return FALSE */
#endif

/*  Call back function into IP app to put calls in MPTY.
**  Example - call1 is active, call2 is on hold, Sending
**  MPTY puts call1 and call2 in Conference. One call id is retained 
**  and the other is cleared.
**  TRUE is returned on successfull operation, FALSE otherwise.
**
**  MPTY acts over multiple calls and since data_block is call specific,
**  no data_block is passed.
*/
typedef boolean (cmipapp_cmd_mpty_f_type) (void);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_cmd_mpty_f_type
    ONERROR return FALSE */
#endif

/*  Call back function called by CM to inform client app
**  about being handed over from technology to another
*/
typedef boolean (cmipapp_cmd_notify_ho_f_type) (

 cmipapp_ho_type_e_type  ho_type,
    /* Indicates from and to handover technology
    */

  cm_call_id_type         call_id,
    /* Call for which handover is being
    ** notified
    */

  cm_call_id_type         new_call_id,
    /* New call id assigned to the call 
    ** after handoff
    */
  
  uint32                  data_block
    /* Token created and used by registered apps.
    ** App gives CM this value whenever cmipapp_rpt_ind () is called.
    ** CM passes the most recent value given by app while calling
    ** app registered call back functions
    */

);

/* Function called into ipapp 
** to report current operating 
** mode
*/
typedef boolean (cmipapp_inform_oprt_mode_f_type) (

  sys_oprt_mode_e_type    oprt_mode,
    /* Current operating mode
    */  

  cmipapp_trans_id_type   trans_id
    /* Transaction id used between
    ** CM and App
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_inform_oprt_mode_f_type
    ONERROR return FALSE */
#endif

/*  Call back function into IP app to send MO SMS over SIP
**  TRUE is returned on successfull operation, FALSE otherwise.
*/
typedef boolean (cmipapp_sms_cmd_mo_sms_f_type) (

  cm_sms_msg_type                 *data,
   /* The pointer to MO SMS message.
   */

  sys_sys_mode_e_type             sys_mode
  /* The sys mode over which the mo SMS is sent.
  */

);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ CALLBACK cmipapp_sms_cmd_mo_sms_f_type
    ONERROR return FALSE */
/*~ PARAM data POINTER */
#endif

/*-------------------------------------------------------------------------*/

/* Structure defining function pointers that
** are registered by Apps. CM calls these
** functions for call related services
*/
typedef struct {

  cmipapp_cmd_orig_f_type      *cmipapp_cmd_orig_func_ptr;
    /* callback to send call origination request
    */

  cmipapp_cmd_answer_f_type    *cmipapp_cmd_answer_func_ptr;
    /* callback for user to answer incoming call
    */

  cmipapp_cmd_invite_res_f_type       *cmipapp_cmd_invite_res_func_ptr;
    /* callback for client to answer incoming call
    */

  cmipapp_cmd_end_f_type       *cmipapp_cmd_end_func_ptr;
    /* callback to request ending a call
    */

  cmipapp_inband_cmd_start_cont_dtmf_f_type
    /* callback to start DTMF
    */
                               *cmipapp_inband_cmd_start_cont_dtmf_func_ptr;

  cmipapp_inband_cmd_stop_cont_dtmf_f_type
    /* callback to stop DTMF
    */
                               *cmipapp_inband_cmd_stop_cont_dtmf_func_ptr;

  cmipapp_cmd_hold_f_type      *cmipapp_cmd_hold_func_ptr;
    /* callback to put call on hold
    */

  cmipapp_cmd_active_f_type    *cmipapp_cmd_active_func_ptr;
    /* callback to make call active
    */

  cmipapp_cmd_modify_f_type    *cmipapp_cmd_modify_func_ptr;
    /* callback for CM to modify call type
    */

  cmipapp_cmd_srv_sys_info_f_type *cmipapp_cmd_serv_sys_info_ptr;
    /* callback to inform change in serving system parameters.
    */  

  cmipapp_cmd_act_answer_hold_f_type 
                               *cmipapp_cmd_act_answer_hold_func_ptr;
    /*  Function called into ip app for activating answer hold.
    */

  cmipapp_cmd_deact_answer_hold_f_type 
                               *cmipapp_cmd_deact_answer_hold_func_ptr;
    /*  Function called into ip app for de-activating answer hold.
    */

  cmipapp_cmd_answer_call_waiting_f_type
                                *cmipapp_cmd_answer_call_waiting_func_ptr; 
    /*  Function called into ip app for answer the call waiting;
    */

  cmipapp_cmd_simple_flash_f_type
                                *cmipapp_cmd_simple_flash_func_ptr; 
    /*  Function called into ip app for answer the call waiting or
     *  to remove last party added to the conference call
     */

  cmipapp_cmd_ect_f_type       *cmipapp_cmd_ect_func_ptr;
    /* Call back function called by CM of mobile initiating
    ** ECT
    */

  cmipapp_cmd_mpty_f_type      *cmipapp_cmd_mpty_func_ptr;
    /* Callback to put all the current calls in multiparty. */      

  cmipapp_cmd_notify_ho_f_type *cmipapp_cmd_notify_ho_func_ptr;
    /* Callback to inform ipapp of a handover from one technology
    ** to another
    */

  cmipapp_inform_oprt_mode_f_type *cmipapp_inform_oprt_mode;
    /* Callback to inform ipapp of operating mode */      

  cmipapp_sms_cmd_mo_sms_f_type 
                               *cmipapp_sms_cmd_mo_sms_func_ptr;
    /* Call back to send MO SMS over SIP */

} cmipapp_cb_s_type;


/**--------------------------------------------------------------------------
** Datatypes -   Others
** --------------------------------------------------------------------------
*/

typedef struct {

  cm_ipapp_cause_e_type           ipapp_cause;
    /* CM_IPAPP_CAUSE_NORMAL - When registration is successfull.
    ** Actual cause code when registration fails.
    */

  cm_ipapp_warn_info_s_type       warn_info;
    /* Warning information given by ipapp
    */

} cmipapp_cap_reg_reason_s_type; 

/**--------------------------------------------------------------------------
** Functions - Called by Apps
** ------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION cmipapp_cb_init

DESCRIPTION
  Used by application to initialize cmipapp_cb_s_type structure with
  NULL values for function pointers


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmipapp_cb_init (

    cmipapp_cb_s_type                *app_cb_ptr
    /* Structure pointing to app call back functions
    */

);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_cb_init */
/*~ PARAM OUT app_cb_ptr POINTER */
#endif

/*===========================================================================

FUNCTION cmipapp_reg

DESCRIPTION
  Used by application to register callback functions with CM

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/

extern cmipapp_status_e_type cmipapp_reg (

    const cmipapp_cb_s_type               *app_cb_ptr,
    /* Structure pointing to app call back functions
    */

    cmipapp_id_type                        app_id
    /* App id assigned by CM
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg */
/*~ PARAM app_cb_ptr POINTER */
#endif


/*===========================================================================

FUNCTION cmipapp_get_ph_identity

DESCRIPTION
  Application calls this function in CM with a buffer to hold phone identity.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().
  This API has not been implemented yet. Hence the declaration is commented out.
  It has been retained in order to preserve the design effort.

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
/* extern cmipapp_status_e_type   cmipapp_get_ph_identity (

**    cmipapp_identity_s_type        *identity_ptr
**    // identity of phone
**    

**);
*/


/*===========================================================================

FUNCTION cmipapp_rpt_ind

DESCRIPTION
  Allows Voip Apps to queue up messages to the CM. Grab a buffer from the
  cmipapp_rpt_free_q, fill it in, and call cmipapp_rpt_ind with it.
  All this will do is queue it and set a signal for the CM task.  When the
  command is done, the buffer will be queued as directed in the command
  header, and a task you specify may be signaled.


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmipapp_rpt_ind (

  cm_name_type        ip_rpt_name,
    /* Name of report being queued to CM
    */

  cmipapp_ind_u_type     ip_ind,
    /* Union of all rpt types getting queued to CM
    */

  uint32           data_block
    /* Token created and used by CM.
    ** data_block is passed by CM, when calling app registered
    ** callbacks. Client needs to pass the most recent value
    ** given by CM, all the time when cmipapp_rpt_ind () is called.
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_rpt_ind */
/*~ PARAM ip_ind DISC ip_rpt_name */
#endif


/*===========================================================================

FUNCTION cmipapp_cap_reg

DESCRIPTION
  Allows clients to register call handling capabilities

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_cap_reg (

  cmipapp_id_type             app_id,
    /* App ID
    */

  cmipapp_call_type_e_type    call_type,
    /* Bit Masks of call type it supports
    */

  cmipapp_sys_mode_e_type     sys_mode
    /* System modes app can support for calls
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_cap_reg */
#endif


/*===========================================================================

FUNCTION cmipapp_cap_reg_with_reason

DESCRIPTION
  Allows clients to register call handling capabilities

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_cap_reg_with_reason (

  cmipapp_id_type                 app_id,
    /* App ID
    */

  cmipapp_call_type_e_type        call_type,
    /* Bit Masks of call type it supports
    */

  cmipapp_sys_mode_e_type         sys_mode,
    /* System modes app can support for calls
    */

  cmipapp_cap_reg_reason_s_type   reason
    /* reason carries success or details of failure  
    ** with respect to app registeration with network.
    */  
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_cap_reg_with_reason */
#endif

/*===========================================================================

FUNCTION cmipapp_inform_status

DESCRIPTION
  Client informs CM if it is active or inactive

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_inform_status (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cmipapp_run_status_e_type      app_run_status
   /* CMIPAPP_STATUS_ACTIVE  - App is active
   ** CMIPAPP_STATUS_ACTIVE  - App is inactive
   */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_inform_status */
#endif

/*===========================================================================

FUNCTION cmipapp_inform_status_with_callid

DESCRIPTION
  Client informs CM if it is active or inactive

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_status_e_type cmipapp_inform_status_with_callid (

  cmipapp_id_type                app_id,
   /* App ID
   */

  cmipapp_run_status_e_type      app_run_status,
    /* CMIPAPP_STATUS_ACTIVE  - App is active
    ** CMIPAPP_STATUS_ACTIVE  - App is inactive
    */

  cm_call_id_type                call_id 
    /* voice_call_id on which videoshare call can be made
    ** is supplied here.
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_inform_status_with_callid */
#endif

/*===========================================================================

FUNCTION cmipapp_id_allocate

DESCRIPTION
  Returns a free index from cmipapp_arr[]

DEPENDENCIES
  cmipapp_init

RETURN VALUE

  CMIPAPP_STATUS_APPID_PTR_ERR - Supplied app id ptr is NULL

  CMIPAPP_STATUS_APPID_EXHAUST - No free app id available

  CMIPAPP_STATUS_OK            - App id successfully allocated

SIDE EFFECTS

  Modifies global cmipapp_arr[].
===========================================================================*/
extern cmipapp_status_e_type cmipapp_id_allocate (

    cmipapp_call_type_e_type     call_type,
      /* App type
      */

    cmipapp_sys_mode_e_type      sys_mode,
      /* System modes app can support for calls
      */

    cmipapp_id_type             *app_id_ptr
      /* App ID pointer
      */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_id_allocate */
/*~ PARAM INOUT app_id_ptr POINTER */
#endif


/*===========================================================================

FUNCTION cmipapp_id_deallocate

DESCRIPTION
  Application calls to deallocate app id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_id_deallocate (

  cmipapp_id_type                         app_id
    /* App id assigned by CM
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_id_deallocate */
#endif

/****************************************************************************
** RPC tool has limitations in remoting function pointers declared in a 
** structure. Defining individual apis for each of the function pointers
** in cmipapp_cb_s_type to overcome the limitation.
****************************************************************************/


/*===========================================================================

FUNCTION cmipapp_reg_orig_func

DESCRIPTION
  Registers function for the app to originate a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_orig_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_orig_f_type                 *cmipapp_cmd_orig_func_ptr
    /* Ptr to call orig function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_orig_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_answer_func

DESCRIPTION
  Registers function for the app to answer a MT call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_answer_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_answer_f_type              *cmipapp_cmd_answer_func_ptr
    /* Ptr to MT call answer function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_answer_func */
#endif


/*===========================================================================

FUNCTION cmipapp_reg_answer_call_waiting_func

DESCRIPTION
  Registers function for the app to answer an call waiting MT call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_answer_call_waiting_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_answer_call_waiting_f_type *answer_call_waiting_func_ptr
    /* Ptr to MT call answer function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_answer_call_waiting_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_simple_flash_func

DESCRIPTION
  Registers function for the app to answer an call waiting MT call or to
  remove the last party added to the conference call

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Registration Successful

SIDE EFFECTS
  None

===========================================================================*/
cmipapp_status_e_type cmipapp_reg_simple_flash_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_simple_flash_f_type *simple_flash_func_ptr
    /* Ptr to the simple flash request function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_simple_flash_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_invite_res_func

DESCRIPTION
  Registers function for the app to respond to a invite message.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_invite_res_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_invite_res_f_type          *cmipapp_cmd_invite_res_func_ptr
    /* Ptr to invite response function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_invite_res_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_end_func

DESCRIPTION
  Registers function for the app to end a call.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_end_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_end_f_type                 *cmipapp_cmd_end_func_ptr
    /* Ptr to call end function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_end_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_inband_start_cont_dtmf_func

DESCRIPTION
  Registers function for the app to start continuous inband dtmf.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_inband_start_cont_dtmf_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_inband_cmd_start_cont_dtmf_f_type

                              *cmipapp_inband_cmd_start_cont_dtmf_func_ptr
    /* Ptr to start inband dtmf function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_inband_start_cont_dtmf_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_inband_stop_cont_dtmf_func

DESCRIPTION
  Registers function for the app to stop inband continuous dtmf.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_inband_stop_cont_dtmf_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_inband_cmd_stop_cont_dtmf_f_type

                              *cmipapp_inband_cmd_stop_cont_dtmf_func_ptr
    /* Ptr to stop inband dtmf function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_inband_stop_cont_dtmf_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_hold_func

DESCRIPTION
  Registers function for the app to hold a call

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_hold_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_hold_f_type                *cmipapp_cmd_hold_func_ptr
    /* Ptr to call hold function 
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_hold_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_active_func

DESCRIPTION
  Registers function for the app to make the call active.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_active_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_active_f_type              *cmipapp_cmd_active_func_ptr
    /* Ptr to call active function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_active_func */
#endif


/*===========================================================================

FUNCTION cmipapp_reg_modify_func

DESCRIPTION
  Registers function for the app to modify the call type (ex:- voice to VT).

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_modify_func (

  cmipapp_id_type                         app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_modify_f_type              *cmipapp_cmd_modify_func_ptr
    /* Ptr to call modify function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_modify_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_act_answer_hold_func

DESCRIPTION
  Registers function for the app to activate answer hold.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_act_answer_hold_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_act_answer_hold_f_type 
                                      *cmipapp_cmd_act_answer_hold_func_ptr
    /* Ptr to activate answer hold function
    */
);


#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_act_answer_hold_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_deact_answer_hold_func

DESCRIPTION
  Registers function for the app to de-activate answer hold.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_deact_answer_hold_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_deact_answer_hold_f_type 
                                      *cmipapp_cmd_deact_answer_hold_func_ptr
    /* Ptr to de-activate answer hold function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_deact_answer_hold_func */
#endif


/*===========================================================================

FUNCTION cmipapp_reg_serv_sys_info_func

DESCRIPTION
  Registers function for the app to be informed of current service.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_serv_sys_info_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_srv_sys_info_f_type 
                                      *cmipapp_cmd_srv_sys_info_func_ptr
    /* Ptr to function that will inform app of service
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_serv_sys_info_func */
#endif


/*===========================================================================

FUNCTION cmipapp_reg_ect_func

DESCRIPTION
  Registers function for the app to provide Call transfer funcitonality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_ect_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_ect_f_type 
                                      *cmipapp_cmd_ect_func_ptr
    /* Ptr to call transfer function.
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_ect_func */
#endif

/*===========================================================================

FUNCTION cmipapp_reg_mpty_func

DESCRIPTION
  Registers function for the app to provide Multiparty functionality.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_mpty_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_cmd_mpty_f_type 
                                      *cmipapp_cmd_mpty_func_ptr
    /* Ptr to de-activate answer hold function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_mpty_func */
#endif


/*===========================================================================

FUNCTION cmipapp_reg_mo_sms_func

DESCRIPTION
  Registers function for the app to be informed of MO SMS message.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_mo_sms_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_sms_cmd_mo_sms_f_type        *cmipapp_sms_cmd_mo_sms_func_ptr
    /* Ptr to send MO SMS message function
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_mo_sms_func */
#endif



/**--------------------------------------------------------------------------
** Functions - Called by CM
** ------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cmipapp_call_id_allocate

DESCRIPTION
  Returns a new CM call id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CM_CALL_ID_INVALID - invalid call id
  valid call id

SIDE EFFECTS
  None
===========================================================================*/
extern cm_call_id_type cmipapp_call_id_allocate (

  void
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_call_id_allocate */
#endif


/*===========================================================================

FUNCTION cmipapp_call_id_deallocate

DESCRIPTION
  Deallocates a CM call id

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmipapp_call_id_deallocate (

   cm_call_id_type call_id
     /* Call id */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_call_id_deallocate */
#endif


/*===========================================================================

FUNCTION cmipapp_reg_ph_oprt_mode_func

DESCRIPTION
  Registers function for the app that gets called 
  to be informed about Phone's operating mode.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  CMIPAPP_STATUS_OK - Success, Other enumerations - Specific failure reasons          

SIDE EFFECTS
  None
===========================================================================*/
extern cmipapp_status_e_type cmipapp_reg_oprt_mode_func (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_inform_oprt_mode_f_type 
                                      *cmipapp_inform_oprt_mode_func_ptr
    /* Ptr to record oprt mode function
    */
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_reg_oprt_mode_func */
#endif


/*===========================================================================

FUNCTION cmipapp_get_cb_ptrs

DESCRIPTION
  Returns callback functions registered by an app

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  NULL - if operation to get call back ptrs FAILS
  valid ptr to call back ptrs

SIDE EFFECTS
  None
===========================================================================*/

extern cmipapp_cb_s_type *cmipapp_get_cb_ptrs (

  cmipapp_id_type             app_id
    /* App id
    */
);


/*===========================================================================

FUNCTION cmipapp_oprt_mode_cnf

DESCRIPTION
  Api called by app to confirm processing
  of operating mode request.

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmipapp_oprt_mode_cnf (

  cmipapp_id_type                      app_id,
    /* App id assigned by CM
    */

  cmipapp_trans_id_type                trans_id
    /* Transaction id
    */
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION cmipapp_oprt_mode_cnf */
#endif



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* defined(FEATURE_IP_CALL) */

#endif /* CMIPAPP_H */
