/*======================================================
FILE:  OEMTAPI.c

SERVICES:  OEM TAPI Interface

GENERAL DESCRIPTION:
   OEM TAPI Interfaces

PUBLIC CLASSES AND STATIC FUNCTIONS:
   List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

   See Exported Routines

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#define OEMTAPI_IMPL

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <stdlib.h>
#include "customer.h"
#include "cm.h"
#include "cmph.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"
#include "OEMFeatures.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMNet.h"
#include "OEMTAPI.h"
#include "OEMConfig.h"
#include "OEMHeap.h"
#include "OEMSock.h"
#include "OEMDSS.h"
#include "msm/AEEPhone.h"
#include "msm/OEMPhone.h"
#include "OEMSMS.h"


#ifdef FEATURE_GSM1x
int OEMGSM1xMessage_SVC_RerouteTapi (const char * pszDst,
                                     const char * pszMsg,
                                     AEECLSID     clsDst,
                                     uint32     * pdwStatus,
                                     AEECallback* pcb);
#endif   // FEATURE_GSM1x



#if defined(FEATURE_BREW_TAPI)

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

struct _OEMTAPI  {
   uint32           dwRefs;
   char             szMobileID[MOBILE_ID_LEN + 1];   // Mobile ID (digits)

   // MakeVoiceCall
   AEECallback      *m_pcbDial;
   char *            m_pszDial;

   TAPIStatus        m_lastTAPIStatus;
   uint16            m_wLastOnCallStatusFlags;
   
   OEMPhone          *pOEMPhone;
   AEECallback       cbPhoneListener;

   AEECallback       *m_pcbPhoneStatus;
   AEECallback       *m_pcbCallEvent;
};

//
// Method declarations...
//

static boolean OEMTAPI_FillStatus(OEMTAPI * pme, TAPIStatus * ps) ;
static void OEMTAPI_ResumeVoiceCall(OEMTAPI * pme) ;

static boolean OEMTAPI_CompressNumber(char * pszSrc) ;
static void OEMTAPI_PhoneListener( OEMTAPI *pme );
static OEMTAPI *           gpTAPI = NULL;

/*===========================================================================
//
//
//   TAPI INTERFACE
//
//
===========================================================================*/
/*===========================================================================

===========================================================================*/
int OEMTAPI_New(OEMTAPI **ppif)
{
   int nErr = SUCCESS;

   if (gpTAPI) {

      ++gpTAPI->dwRefs;
      *ppif = gpTAPI;
      return SUCCESS;
   }

   gpTAPI = (OEMTAPI *)sys_malloc(sizeof(OEMTAPI));
   if( (OEMTAPI *)0 == gpTAPI ) {

      return ENOMEMORY;
   }

   ZEROAT( gpTAPI );

   gpTAPI->dwRefs = 1;

   if( SUCCESS == (nErr = OEMPhone_New(&gpTAPI->pOEMPhone)) ) {

      CALLBACK_Init( &gpTAPI->cbPhoneListener, OEMTAPI_PhoneListener, gpTAPI );
      OEMPhone_AddListener( gpTAPI->pOEMPhone, &gpTAPI->cbPhoneListener );
   }
   else {

      OEMTAPI_Destroy( gpTAPI );          /* This makes gpTAPI NULL */
   }

   *ppif = gpTAPI;

   return nErr;
}

/*===========================================================================

===========================================================================*/
int OEMTAPI_Destroy(OEMTAPI * pme)
{

   if( --pme->dwRefs ) {

      return SUCCESS;
   }
   
   if( pme->pOEMPhone ) {

      CALLBACK_Cancel( &pme->cbPhoneListener );
      OEMPhone_Release( pme->pOEMPhone );
   }

   if( pme == gpTAPI ) {

      gpTAPI = NULL;
   }
   sys_free(pme);

   return SUCCESS;
}

/*======================================================================= 

Function: OEMTAPI_GetStatus()

Description: 
   This method is obtains the current status of the telephony device.  This
includes service status and call status.  

Prototype:

   // C Users
   int   OEMTAPI_GetStatus(OEMTAPI * po, TAPIStatus * ps);

Parameters:
   po: Pointer to the OEMTAPI object
   ps: Pointer to status information to be filled.

Return Value:

  SUCCESS - Status information is valid
  EBADPARM -  Bad parameter

Comments:  
   None

Side Effects: 
   None

See Also: 
   None
=======================================================================*/
int OEMTAPI_GetStatus(OEMTAPI * pme, TAPIStatus * pStatus)
{
   int nSize = sizeof(pme->szMobileID);

   MEMCPY( pStatus, &pme->m_lastTAPIStatus, sizeof(TAPIStatus) );

   if( SUCCESS != OEM_GetDeviceInfoEx( AEE_DEVICEITEM_MOBILE_ID, (void *)pme->szMobileID, &nSize ) ) {
      AEEMobileInfo mi;

      MEMSET(&mi,0,sizeof(AEEMobileInfo));

      if( SUCCESS == OEM_GetConfig( CFGI_MOBILEINFO, &mi,sizeof(AEEMobileInfo)) ) {
         OEMTAPI_CompressNumber(mi.szMobileID);
         STRLCPY(pme->szMobileID,mi.szMobileID,nSize);
      }
   }

   STRCPY(pStatus->szMobileID, pme->szMobileID);
   return SUCCESS;
}

/*======================================================================= 
Function: OEMTAPI_GetCallerID()

Description: 
   This method is used to retrieve the ID (digits) of an incoming or
outgoing voice call.  

Prototype:

   // C Users
   boolean OEMTAPI_GetCallerID(OEMTAPI * po,AECHAR * pDest, int nSize);

Parameters:
   po: Pointer to the OEMTAPI object
   pDest: Destination pointer
   nSize: Size in bytes of the destination buffer

Return Value:

  TRUE - Call in progress and buffer filled.
  FALSE - No call in progress or invalid buffer

Comments:  
   None

Side Effects: 
   None

See Also: 
   None
=======================================================================*/
boolean OEMTAPI_GetCallerID(OEMTAPI * pme, AECHAR * psz, int nSize)
{
   const CM_CALL_INFO_S_TYPE *      pci;
   char *                     pszNum;

   if( AEE_CHECK_PTR("GetCallerID",psz,nSize,TRUE) && 
       OEMPhone_IsVoiceCall(pme->pOEMPhone)) {
      pci      = OEMPhone_GetCallInfo( pme->pOEMPhone );
      pszNum   = (char *)pci->num.buf;
      *psz     = 0;
      if(NULL != pci){
         if(pci->num.len){
            pszNum[pci->num.len] = 0;
            STRTOWSTR(pszNum,psz,nSize);
            return TRUE;
         }
      }
   }
   return FALSE;
}

/*===========================================================================

===========================================================================*/
int OEMTAPI_OnCallStatus(OEMTAPI * pme, AEECallback *pcb) 
{
   pme->m_pcbCallEvent = pcb;
   return SUCCESS;
}

/*===========================================================================

===========================================================================*/
boolean OEMTAPI_IsVoiceCall(OEMTAPI * pme)
{
   return OEMPhone_IsVoiceCall(pme->pOEMPhone);
}

/*===========================================================================

===========================================================================*/
boolean OEMTAPI_IsDataSupported(OEMTAPI * pme)
{
   return OEMPhone_IsDataSupported(pme->pOEMPhone);
}

/*===========================================================================
Cancels the MVC requests that are not dispatched to CM.
===========================================================================*/
static void OEMTAPI_CancelVoiceCall( AEECallback *pcb )
{
   char *   psz = (char *)pcb->pCancelData;

  //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pcb->pfnCancel != OEMTAPI_CancelVoiceCall)
      return;
   
   /* Releasing the dial string will do the job. */
   if( psz == gpTAPI->m_pszDial ) {
      gpTAPI->m_pszDial = NULL;
      sys_free( psz );
   }
}

/*======================================================================= 

Function: OEMTAPI_MakeVoiceCall()

Description: 
   This method is called to place a voice call.  The number dialed is 
specified in the digits string.  Note the following when using this method:

- No call is placed if the input string is empty or NULL.  Only the following
digits are allowed (0-9, #, *).  All other digits are ignored.

- If a voice call is in progress EALREADY is returned.

- If a data call is in progress and no network activity is in-progress (TCP),
the data call is ended and the call is placed.

- This function enforces the privacy policies established by the carrier.  This
may include intermediate prompts to the user.


Prototype:

   // C Users
   int   OEMTAPI_MakeVoiceCall(OEMTAPI * po, const char * pszNumber);

Parameters:
   po: Pointer to the OEMTAPI object
   pszNumber: Pointer to number to dial

Return Value:

  SUCCESS - Function is in progress
  EBADPARM -  Invalid number
  EALEADY - Voice call already in progress

Comments:  
   None

Side Effects: 
   None

See Also: 
   None
=======================================================================*/
int OEMTAPI_MakeVoiceCall(OEMTAPI * pme, const char * pszNumber, AEECallback *pcbDial )
{
   char  *                  psz;
   const CM_CALL_INFO_S_TYPE    * pci;
   cm_call_state_e_type     st;
   boolean                  bDataCall = FALSE;


   // See if there is another call in progress
   //
   // - Data Calls - Drop the connection then proceed...
   // - Voice Calls - Return an error...
   //

   if((pci = OEMPhone_GetCallInfo(pme->pOEMPhone)) != NULL){
      st = pci->call_state;

      if(st == CM_CALL_STATE_CONV || st == CM_CALL_STATE_INCOM || st == CM_CALL_STATE_ORIG){
         if(pci->call_type == CM_CALL_TYPE_VOICE){
            FARF(TAPI,("MVC - FAILED (in progress)"));
            return EALREADY;
         }

         if(pci->call_type == CM_CALL_TYPE_DATA) {
            bDataCall = TRUE;
         }
      }
   }

   // Allocate the dial string...

   psz = sys_strdup(pszNumber);

   if(!OEMTAPI_CompressNumber(psz)){
      FARF(TAPI,("MVC - FAILED (no digits)"));
      sys_free(psz);
      return EBADPARM;
   }

   pme->m_pszDial = psz;
   pme->m_pcbDial = pcbDial;

   pcbDial->pfnCancel = OEMTAPI_CancelVoiceCall;
   pcbDial->pCancelData = psz;

   if(bDataCall){
      /*  <porting> Release the traffic channel. </porting> */
      OEMPhone_EndCall( pme->pOEMPhone );
      /* Defer the voice call. OEMTAPI_ResumeVoiceCall is invoked when 
      ** phone returns to IDLE state. */
   }
   else {
      OEMTAPI_ResumeVoiceCall(pme);
   }
   return 0;
}

/*===========================================================================

===========================================================================*/
static void OEMTAPI_ResumeVoiceCall(OEMTAPI * pme)
{
   const char *   psz = (const char *)pme->m_pszDial;
   int            i;

   if(psz){
      i = STRLEN(psz);

      (void)OEMPhone_MakeCall(pme->pOEMPhone, CM_CALL_TYPE_VOICE, CM_SRV_TYPE_AUTOMATIC,0,(const byte *)psz,(byte)i,CM_DIGIT_MODE_4BIT_DTMF, (const byte *)"",0, CM_OTASP_ACT_CODE_NONE);

      AEE_SYS_RESUME( pme->m_pcbDial );
   }
   else {
      FARF(TAPI,("MVC - no number"));
   }
}

/*===========================================================================

===========================================================================*/
int OEMTAPI_OnPhoneStatus( OEMTAPI *pme, AEECallback *pcb )
{

   pme->m_pcbPhoneStatus = pcb;

   return SUCCESS;
}

/*===========================================================================

===========================================================================*/
uint16 OEMTAPI_GetCallStatus( OEMTAPI *pme )
{
   return pme->m_wLastOnCallStatusFlags;
}

/*===========================================================================

===========================================================================*/
boolean OEMTAPI_PhoneInEmergencyMode(const cm_ph_info_s_type *ph_ptr)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if we are in call back mode.
  */
  if ((ph_ptr->mode_pref == CM_MODE_PREF_EMERGENCY 
        && ph_ptr->pref_term == CM_PREF_TERM_PWR_CYCLE) 
       || ph_ptr->network_sel_mode_pref 
            == CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV){ /* In emergency mode.*/
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

===========================================================================*/
static boolean OEMTAPI_FillStatus(OEMTAPI * pme, TAPIStatus * ps)
{
   const CM_CALL_INFO_S_TYPE *pci = OEMPhone_GetCallInfo(pme->pOEMPhone);
   const CM_SS_INFO_S_TYPE   *psi = OEMPhone_GetSystemInfo(pme->pOEMPhone);
   const cm_ph_info_s_type   *ppi = OEMPhone_GetPhoneInfo(pme->pOEMPhone);
   
   MEMSET(ps, 0, sizeof(TAPIStatus));

   if (!ppi) {
      return FALSE;
   }

   // See if we are in E911 callback mode...
   ps->bE911CallbackMode = OEMTAPI_PhoneInEmergencyMode(ppi);

   // Set the phone state...
   ps->state = PS_OFFLINE;

#ifdef FEATURE_MULTIMODE_ARCH
   if( ppi->oprt_mode == SYS_OPRT_MODE_LPM ) {
      ps->bAirplaneMode = TRUE;
   }
#else
   if( ppi->oprt_mode == CM_OPRT_MODE_LPM ) {
      ps->bAirplaneMode = TRUE;
   }
#endif

   if ( OEMPhone_IsOffline(pme->pOEMPhone) ){
      return TRUE;  /* we're done */
   }

   // default the phone state to idle (not NO_SRV)
   ps->state = PS_IDLE;
#if defined(FEATURE_MULTIMODE_ARCH) && !defined(FEATURE_CM20_INTERFACE)
   if ((cm_mm_ss_info_s_type *)0 != psi) {
      // Indicate whether we have digital coverage...
      ps->bDigital =
         cm_is_sys_mode_digital(psi->sys_mode) &&
         (psi->srv_status == SYS_SRV_STATUS_SRV);
         
#ifdef CM_API_WLAN  
      ps->bDigital = ps->bDigital || (SYS_SRV_STATUS_SRV == psi->wlan_srv_status);
#endif
      // Indicate whether we are roaming...
      ps->bRoaming = (SYS_ROAM_STATUS_OFF != psi->roam_status);
      
      ps->bHDRMode = ( (psi->sys_mode == SYS_SYS_MODE_HDR && sys_srv_status_is_srv(psi->srv_status)) 
#ifdef FEATURE_HDR_HYBRID
                       || (psi->hdr_hybrid && sys_srv_status_is_srv(psi->hdr_srv_status)) 
#endif /* FEATURE_HDR_HYBRID */
                       );

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)      
      ps->bDormancy = (psi->srv_domain == SYS_SRV_DOMAIN_PS_ONLY 
                       || psi->srv_domain == SYS_SRV_DOMAIN_CS_PS);      
#endif
   }
   if ((cm_mm_call_info_s_type *)0 != pci) {
         
      // Indicate whether it was a call end event...
      ps->bCallEnded = (CM_CALL_END_FADE <= pci->end_status);

      // Indicate whether we're in a data call
      ps->bData      = ((CM_CALL_TYPE_CS_DATA == pci->call_type) || (CM_CALL_TYPE_PS_DATA == pci->call_type));

#ifdef FEATURE_IS2000
      /* used to signal dormancy capable network */
      // AMSS - 9/4/2002 modified pci to reflect the location of srv_opt
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      FARF(TAPI,("SO %d, dorm == %s\n",pci->mode_info.info.cdma_call.srv_opt,
                  (CAI_SO_PPP_PKT_DATA_3G == pci->mode_info.info.cdma_call.srv_opt)?"TRUE":"FALSE"));
      if( ps->bDormancy == FALSE ) {
         ps->bDormancy = (CAI_SO_PPP_PKT_DATA_3G == pci->mode_info.info.cdma_call.srv_opt);
      }
#endif /* if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
#endif /* ifdef FEATURE_IS2000 */

#else
   // Indicate whether we have digital coverage...
   ps->bDigital = ( CM_SRV_STATE_ANALOG < ppi->srv_state 
                    && ppi->srv_state < CM_SRV_STATE_MAX );
   
   // Indicate whether we are roaming...
   ps->bRoaming = (CM_ROAM_STATUS_OFF != ppi->roam_status);
   
   if ((cm_call_info_s_type *)0 != pci) {
         
      // Indicate whether it was a call end event...
      ps->bCallEnded = (CM_CALL_END_FADE <= pci->end_status);

      // Indicate whether we're in a data call
      ps->bData      = (CM_CALL_TYPE_DATA == pci->call_type);

#ifdef FEATURE_IS2000
      /* used to signal dormancy capable network */
      FARF(TAPI,("SO %d, dorm == %s\n",pci->srv_opt,
                  (CAI_SO_PPP_PKT_DATA_3G == pci->srv_opt)?"TRUE":"FALSE"));
      ps->bDormancy = (CAI_SO_PPP_PKT_DATA_3G == pci->srv_opt);
#endif /* ifdef FEATURE_IS2000 */
#endif /*FEATURE_MULTIMODE_ARCH*/

      switch (pci->call_state) {
      case CM_CALL_STATE_IDLE:
         ps->state = PS_IDLE;
         break;

      case CM_CALL_STATE_ORIG:
         ps->state = PS_ORIG;
         break;

      case CM_CALL_STATE_INCOM:
         ps->state = PS_INCOMING;
         break;

      case CM_CALL_STATE_CONV:
         ps->state = PS_CONVERSATION;
         break;
      }
   }

#ifdef FEATURE_MULTIMODE_ARCH
   if ((cm_mm_ss_info_s_type *)0 != psi) {
#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
      ps->bRegistered = psi->mode_info.cdma_info.is_registered;
#endif /* if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */
   }
#else
   if ((cm_ss_info_s_type *)0 != psi) {
      ps->bRegistered = psi->is_registered;
   }
#endif /*FEATURE_MULTIMODE_ARCH*/

   return TRUE;
}

/*===========================================================================

===========================================================================*/
static void OEMTAPI_PhoneListener( OEMTAPI *pme )
{
   uint16   wFlags = OCS_OFFLINE;
   const CM_CALL_INFO_S_TYPE *pci = OEMPhone_GetCallInfo( pme->pOEMPhone );

   if( pme->m_pcbPhoneStatus ) {
      TAPIStatus ts;

      // Fill the status buffer and see if it changed.  If not, there is no need
      // to notify the user/client...

      if (OEMTAPI_FillStatus(pme,&ts) && MEMCMP(&ts,&pme->m_lastTAPIStatus,sizeof(TAPIStatus))) {
         
         MEMCPY( &pme->m_lastTAPIStatus, &ts, sizeof(TAPIStatus) );
         AEE_SYS_RESUME( pme->m_pcbPhoneStatus );
      }
   }

   if( !OEMPhone_IsOffline( pme->pOEMPhone ) && pci ) {

      switch( pci->call_state ) {
      case CM_CALL_STATE_IDLE:
         wFlags = OCS_IDLE;
         break;
         
      case CM_CALL_STATE_ORIG:
         wFlags = OCS_ORIG;
         break;
         
      case CM_CALL_STATE_INCOM:
         wFlags = OCS_INCOMING;
         break;
         
      case CM_CALL_STATE_CONV:
         wFlags = OCS_CONVERSATION;
         break;
      }
   }

   if (wFlags != pme->m_wLastOnCallStatusFlags) {
      
      pme->m_wLastOnCallStatusFlags = wFlags;

      if( pme->m_pcbCallEvent ) AEE_SYS_RESUME( pme->m_pcbCallEvent );

      if( (wFlags & OCS_IDLE) && pme->m_pszDial ) {
         /* If this hits idle state and there is a dial request pending
         ** resume it. This occurs when a voice call is requested while 
         ** data call exists. Voice call attempts to preempt the data call.
         */
         OEMTAPI_ResumeVoiceCall( pme );
      }
   }
#if defined(TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE)
   OEMDSS_OnCallStatus();
#endif // defined(TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE)
}

/*===========================================================================

===========================================================================*/
static boolean OEMTAPI_CompressNumber(char * pszSrc)
{
   char *   pszDest;
   char     ch;
   int      i;

   // Compress out any garbage...

   for(i = 0, pszDest = pszSrc; (ch = *pszSrc) != 0; pszSrc++){
      if((ch == '*' || ch == '#') || (ch >= '0' && ch <= '9')){
         *(pszDest++) = ch;
         i++;
      }
   }
   *pszDest = 0;

   return (boolean)(i > 0);
}

#endif /* if defined(FEATURE_BREW_TAPI) */

