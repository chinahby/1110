/*============================================================================

FILE:  OEMPosDet.c


==============================================================================

               Copyright © 1999-2005 QUALCOMM Incorporated
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#include "customer.h"

#ifdef FEATURE_CGPS          
#include "pdapi.h"
#else
#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
#include "pdsm.h"
#endif
#endif /* FEATURE_CGPS */

#if !defined(AEE_SIMULATOR)
#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#include "mccdma.h"
#endif
#endif // !defined(AEE_SIMULATOR)
#include "OEMFeatures.h"
#include "OEMHeap.h"
#include "AEEStdLib.h"
#include "AEE_OEMDispatch.h"
#include "OEMPosDet.h"
#include "OEMCriticalSection.h"

/*******************************************/
/* types and defines                       */
/*******************************************/
# ifdef AEE_SIMULATOR
#  ifndef FEATURE_PDSM_API_R2
#   define FEATURE_PDSM_API_R2
#   define FEATURE_GPSONE_PE
#  endif
# endif

# ifdef FEATURE_MMGPS
#  define FEATURE_PDSM_API_R4
#  ifndef FEATURE_GPSONE_PE
#   define FEATURE_GPSONE_PE
#  endif
#  ifndef FEATURE_GPSONE_STANDALONE
#   define FEATURE_GPSONE_STANDALONE
#  endif
# endif

#if !defined(PDSM_API_MAJ_V) || !defined(PDSM_API_MIN_V)
# define PDSM_API_MAJ_V 4
# define PDSM_API_MIN_V 99
#endif

#define MIN_PDSM_API_V(maj,min) (PDSM_API_MAJ_V > maj || (PDSM_API_MAJ_V == maj && PDSM_API_MIN_V >= min))
#ifdef FEATURE_CGPS          
#  ifndef FEATURE_GPSONE_PE
#   define FEATURE_GPSONE_PE
#  endif
#  ifndef FEATURE_GPSONE_STANDALONE
#   define FEATURE_GPSONE_STANDALONE
#  endif
#endif /* FEATURE_CGPS */

#define VALIDATE_HANDLE(p) do { if( p!= g_pOEMPosDet) return EBADPARM;  } while( 0 )

#define  GPS_DEF_NUM_FIXES    1
#define  GPS_DEF_FIX_INTERVAL 1
#define  GPS_DEF_QOS_VALUE    20
#define  GPS_LINGER_TIMEOUT   59
#define  GPS_REQUEST_TIMEOUT  800

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
# define MAX_CLIENTS 3
#else
# define MAX_CLIENTS 1
#endif

#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
struct pdsm_client{
    pdsm_client_type_e_type type;
    pdsm_client_id_type     id;
};
#endif // FEATURE_GPSONE || FEATURE_PDAPI

struct _OEMPosDet {
   void                    *pNotifyData;
   PFN_PD_NOFITY           pfnNotify;
   AEEGPSInfo              theResponse;
   uint16                  wRef;
#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
   pdsm_pd_event_type      pd_event;
   /* make room for three clients and a current active client */
   struct pdsm_client      m_PDSMclientID[MAX_CLIENTS];
   pdsm_client_id_type     m_clientInUse;

   pdsm_pd_event_type      sw_event;
#endif
   boolean                 bSwState;
};

/*******************************************/
/* local declarations                      */
/*******************************************/
#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
static void OEM_XlateOptions( const AEEGPSConfig *pConfig, pdsm_pd_option_s_type *pOpt );
static void OEM_PD_EventCB(
#ifdef FEATURE_PDSM_API_R2
   OEMPosDet *pMe,
#endif /* FEATURE_PDSM_API_R2 */
   pdsm_pd_event_type pd_event,
   const pdsm_pd_info_s_type *pd_info_ptr
   );
static void OEM_PD_CmdCB(
   OEMPosDet                 *pMe,
   pdsm_pd_cmd_e_type        pd_cmd,
   pdsm_pd_cmd_err_e_type    pd_cmd_err
   );

#if defined (FEATURE_GPSONE_PE) || defined (FEATURE_PDAPI)
#if !defined(FEATURE_PDSM_API_R4)
static void OEM_SW_CmdCB(
   OEMPosDet                 *pMe,
   pdsm_pd_cmd_e_type        pd_cmd,
   pdsm_pd_cmd_err_e_type    pd_cmd_err
   );
#endif // !defined (FEATURE_PDSM_API_R4)
static int OEM_SW_End( OEMPosDet *pMe );
static int OEM_SW_Start( OEMPosDet *pMe, AEECLSID cls );
#endif
#endif /* defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI) */

#if defined (AEE_SIMULATOR) || defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
static int32 OEM_PD_SignExtend_DWORD( dword dwVal, byte size );
#endif

#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
static void PrvEvCB( OEMPosDet *pMe, PosDetEventType theEvent );
#endif
/*******************************************/
/* global and/or constant data             */
/*******************************************/
static OEMCriticalSection  g_oem_pd_cs;
static OEMPosDet *g_pOEMPosDet;

/*******************************************/
/*     Function definitions                */
/*******************************************/
/*===========================================================================
FUNCTION: OEM_PD_EnterCS

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_PD_EnterCS( void )
{
   OEMCriticalSection_Enter( &g_oem_pd_cs );
}

/*===========================================================================
FUNCTION: OEM_PD_LeaveCS

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_PD_LeaveCS( void )
{
   OEMCriticalSection_Leave( &g_oem_pd_cs );
}

/*===========================================================================
FUNCTION: OEM_PD_GetBaseStationInfo

DESCRIPTION
   Get the information of the serving basestation.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS
  EFAILED
  EBADSTATE   When outside the coverage area.

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_GetBaseStationInfo( AEESectorInfo *psi, uint16 wSize )
{
#if defined(AEE_SIMULATOR)

   psi->wSysID     = 4;
   psi->wNetID     = 8;
   psi->wBaseID    = 7763;
   psi->wBaseClass = 0;
   psi->wBestPN    = 168;
   if (wSize >= sizeof(AEESectorInfo_20)) {
      psi->wPacketZoneID      = 1;
      psi->wMobileCountryCode = 209;
   }
   if (wSize >= sizeof(AEESectorInfo_21)) {
      ((AEESectorInfo_21 *)psi)->dwBaseLatitude     = OEM_PD_SignExtend_DWORD(473800, 22);
      ((AEESectorInfo_21 *)psi)->dwBaseLongitude    = OEM_PD_SignExtend_DWORD(6700916, 23);
   }
   return SUCCESS;
#else /* #if defined(AEE_SIMULATOR) */

#if  defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
   if (!cur_bs_ptr) {
      return EFAILED;
   }

# ifdef FEATURE_BREW_COMPLETE_SYSPARAMS
   psi->wSysID     = cur_bs_ptr->csp.sp.sid; // System Identification
   psi->wNetID     = cur_bs_ptr->csp.sp.nid; // Network Idntification
   psi->wBaseID    = cur_bs_ptr->csp.sp.base_id;    // Base station identification
   psi->wBaseClass = cur_bs_ptr->csp.sp.base_class;// Base station class
   psi->wBestPN    = cur_bs_ptr->csp.sp.pilot_pn; // Pilot PN sequence offset index

   if (wSize >= sizeof(AEESectorInfo_20)) {
      psi->wPacketZoneID      = cur_bs_ptr->csp.esp.packet_zone_id;
      psi->wMobileCountryCode = cur_bs_ptr->csp.esp.mcc;
   }
   if (wSize >= sizeof(AEESectorInfo_21)) {
      ((AEESectorInfo_21 *)psi)->dwBaseLatitude     = OEM_PD_SignExtend_DWORD( cur_bs_ptr->csp.sp.base_lat, 22 );
      ((AEESectorInfo_21 *)psi)->dwBaseLongitude    = OEM_PD_SignExtend_DWORD( cur_bs_ptr->csp.sp.base_long, 23 );
   }
# else /* FEATURE_BREW_COMPLETE_SYSPARAMS */
   psi->wSysID     = cur_bs_ptr->sp.sid; // System Identification
   psi->wNetID     = cur_bs_ptr->sp.nid; // Network Idntification
   psi->wBaseID    = cur_bs_ptr->sp.base_id;    // Base station identification
   psi->wBaseClass = cur_bs_ptr->sp.base_class;// Base station class
   psi->wBestPN    = cur_bs_ptr->sp.pilot_pn; // Pilot PN sequence offset index

   if (wSize >= sizeof(AEESectorInfo_20)) {
      psi->wPacketZoneID      = cur_bs_ptr->esp.packet_zone_id;
      psi->wMobileCountryCode = cur_bs_ptr->esp.mcc;
   }

   if (wSize >= sizeof(AEESectorInfo_21)) {
      ((AEESectorInfo_21 *)psi)->dwBaseLatitude     = OEM_PD_SignExtend_DWORD( cur_bs_ptr->sp.base_lat, 22 );
      ((AEESectorInfo_21 *)psi)->dwBaseLongitude    = OEM_PD_SignExtend_DWORD( cur_bs_ptr->sp.base_long, 23 );
   }
# endif /* FEATURE_BREW_COMPLETE_SYSPARAMS */
   return SUCCESS;
#else /*  defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900) */
   return EUNSUPPORTED;
#endif
#endif /* #if defined(AEE_SIMULATOR) */
}


/*===========================================================================
FUNCTION: OEM_PD_Open

DESCRIPTION
   Instantiantes a session with pdsm and initializes the members of
   OEMPosDet to handle queries and responses.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS, EFAILED, EUNSUPPORTED

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_Open( void *pUser, PFN_PD_NOFITY pfn, OEMPosDet **ppo )
{
#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)
   const pdsm_client_type_e_type types[] =
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
   { PDSM_CLIENT_TYPE_BREW, PDSM_CLIENT_TYPE_JAVA, PDSM_CLIENT_TYPE_BROWSER };
#else
   { PDSM_CLIENT_TYPE_UI };
#endif

   int nErr = SUCCESS;
   OEMPosDet   *pOEMPosDet = 0;
   pdsm_client_id_type pdsm_client_id = -1;
   int i;

   DBGPRINTF( "==> OEM_PD_Open" );

   if( g_pOEMPosDet ) {
      *ppo = g_pOEMPosDet;
      g_pOEMPosDet->wRef++;
      return SUCCESS;
   }

   if( (pOEMPosDet = (OEMPosDet *)sys_malloc( sizeof(OEMPosDet) )) == 0 ) {
      nErr = EFAILED;
      return nErr;
   }

   ZEROAT(pOEMPosDet);


   /* <porting> Fix the client_type passed to pdsm_client_init()
   ** Perhaps use something that is not shared with native UI.
   ** </porting> */

   /* initialize all three client types here. */
   for( i = 0; i < MAX_CLIENTS; i++ ) {
      pdsm_client_id =  pdsm_client_init(types[i]);

      if( pdsm_client_id < 0 ) {
         nErr = EFAILED;
      }
      else if( PDSM_CLIENT_OK != pdsm_client_act(pdsm_client_id) ){
         nErr = EFAILED;
      }
      else if( PDSM_CLIENT_OK != pdsm_client_pd_reg(pdsm_client_id,
#ifdef FEATURE_PDSM_API_R2
         pOEMPosDet,
#endif /* FEATURE_PDSM_API_R2 */
         (pdsm_pd_event_f_type *)OEM_PD_EventCB,
         PDSM_CLIENT_EVENT_REG,
         PDSM_PD_EVENT_MASK, NULL) ) {
         nErr = EFAILED;
      }

      if( nErr == SUCCESS ) {
         pOEMPosDet->m_PDSMclientID[i].id   = pdsm_client_id;
         pOEMPosDet->m_PDSMclientID[i].type = types[i];
      }
      else {
         *ppo = 0;
         if( pOEMPosDet ) {
            sys_free( pOEMPosDet );
            pdsm_client_deact( pdsm_client_id );
         }
         if( pdsm_client_id >= 0 ) {
            pdsm_client_release( pdsm_client_id );
         }
      }
   }

   if( nErr == SUCCESS ) {
      pOEMPosDet->pNotifyData = pUser;
      pOEMPosDet->pfnNotify   = pfn;
      pOEMPosDet->wRef        = 1;
      g_pOEMPosDet = *ppo     = pOEMPosDet;
   }

   return nErr;
#else /* !FEATURE_GPSONE && !FEATURE_PDAPI */
   return EUNSUPPORTED;
#endif /* FEATURE_GPSONE */
}

/*=======================================================================
Function: OEM_PD_PosDetRelease()

Description:
  When BREW applet releases its last reference to an instance of IPosDet.

Parameters:
   clsApplet: [in]. Class ID of applet. Used to support user plane
              LBS authorization.

Return Value:

   SUCCESS

Comments:
   None

Side Effects:
   None

See Also:
   None
=======================================================================*/
int OEM_PD_PosDetRelease( AEECLSID clsApplet )
{
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
   pdsm_client_type_e_type clientType;

   /* tell pdsm this instance of IPosDet is gone. What if an application
   ** uses multiple instances of IPosDet. */
   clientType = pdsm_lcs_get_client_type(clsApplet);
   pdsm_lcs_client_release(NULL, NULL, clsApplet, clientType);	
#endif

   return AEE_SUCCESS;
}

/*===========================================================================
FUNCTION: OEM_PD_Close

DESCRIPTION
   Releases the pdsm client instance and the memory holding the OEMPosDet.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_Close( OEMPosDet *pOEMPosDet )
{
#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)
   int i;

   DBGPRINTF( "==> OEM_PD_Close" );

   VALIDATE_HANDLE( pOEMPosDet );

   if( --pOEMPosDet->wRef == 0 ) {
      /* close all the clients */
      if(pOEMPosDet->m_PDSMclientID[0].id != 0 ) {
         for( i = 0; i < MAX_CLIENTS; i++ ) {
            pdsm_client_deact( pOEMPosDet->m_PDSMclientID[i].id );
            pdsm_client_release( pOEMPosDet->m_PDSMclientID[i].id );
            pOEMPosDet->m_PDSMclientID[i].id = 0;
         }
      }
      OEM_PD_EnterCS();
      g_pOEMPosDet = 0;
      OEM_PD_LeaveCS();
      sys_free( pOEMPosDet );
   }
   return SUCCESS;
#else /* !FEATURE_GPSONE */
   return EUNSUPPORTED;
#endif /* FEATURE_GPSONE */
}

#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
/* added this routine to get the client id for the given clsid */
/*===========================================================================
FUNCTION: get_client_by_clsid

  DESCRIPTION
  returns the client id to use based on the requestors class id.

    DEPENDENCIES
    none

      RETURN VALUE
      none

        SIDE EFFECTS
        none
===========================================================================*/
static pdsm_client_id_type get_client_by_clsid(AEECLSID clsid)
{
   pdsm_client_type_e_type cl_type;
   int i;
   pdsm_client_id_type ret_id = 0;

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
   cl_type = pdsm_lcs_get_client_type( clsid );
#else
   cl_type = PDSM_CLIENT_TYPE_UI;
#endif

   for( i=0; i<MAX_CLIENTS; i++ ){
      if( cl_type == g_pOEMPosDet->m_PDSMclientID[i].type ) {
         ret_id = g_pOEMPosDet->m_PDSMclientID[i].id;
         break;
      }
   }

   return ret_id;
}
#endif // FEATURE_GPSONE || FEATURE_PDAPI

/*===========================================================================
FUNCTION: OEM_PD_InitiateNewSession

DESCRIPTION
   Sends Position requests to PDSM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_InitiateNewSession( OEMPosDet *pMe, const AEEGPSConfig *pConfig, AEEGPSReq req, AEEGPSAccuracy accuracy, AEECLSID cls )
{
#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)
   int                     nErr = SUCCESS;
   pdsm_pd_qos_type        qos;
   pdsm_pd_svc_type        svcType = 0;
   pdsm_pd_option_s_type   pdsm_opt;

#ifdef FEATURE_PDSM_API_R2
   const int kAccXlate[AEEGPS_ACCURACY_HIGHEST] =  {1000, 500, 250, 100, 50, 25};
#endif /* FEATURE_PDSM_API_R2 */

   DBGPRINTF( "==> OEM_PD_InitiateNewSession" );

   VALIDATE_HANDLE( pMe );

   memset(&qos, 0, sizeof(qos));
   memset(&pdsm_opt, 0, sizeof(pdsm_opt));

   //Now look at the GPS configuration to pick up parameters for the get_position
   //Remember we are overloading status to keep the request
#ifdef FEATURE_PDSM_API_R2
   if( pConfig->mode == AEEGPS_MODE_TRACK_LOCAL
       || pConfig->mode == AEEGPS_MODE_ONE_SHOT
       || pConfig->mode == AEEGPS_MODE_TRACK_STANDALONE
       || pConfig->mode == AEEGPS_MODE_TRACK_OPTIMAL ) {
      /* For single shot and MS based mode */
      qos.accuracy_threshold = kAccXlate[accuracy-1];
   }
   else {
      qos.accuracy_threshold = kAccXlate[AEEGPS_ACCURACY_LOWEST-1];
   }
#ifdef FEATURE_CGPS          
   qos.gps_session_timeout  = pConfig->qos;
#else 
   qos.performance  = pConfig->qos;
#endif /* FEATURE_CGPS */
#else
   qos.call_quality = pConfig->qos;
   qos.performance  = pConfig->qos;
#endif /* FEATURE_PDSM_API_R2 */

   OEM_XlateOptions( pConfig, &pdsm_opt );

   /* Translate the request into OEM service */
   if(req & AEEGPS_GETINFO_LOCATION)
      svcType |= PDSM_PD_SVC_TYPE_POSITION;

   if(req & AEEGPS_GETINFO_VELOCITY)
      svcType |= PDSM_PD_SVC_TYPE_VELOCITY;

   if(req & AEEGPS_GETINFO_ALTITUDE)
      svcType |= PDSM_PD_SVC_TYPE_HEIGHT;

   if( svcType == 0 ) {
#ifdef FEATURE_GPSONE_PE
      if( pdsm_opt.session == PDSM_PD_SESS_TYPE_DATA_DLOAD ) {

         return OEM_SW_Start(pMe, cls);
      }
#endif
      nErr = EBADPARM;
   }

   OEM_PD_EnterCS();
   memset( &(pMe->theResponse), 0, sizeof(pMe->theResponse) );
   pMe->theResponse.accuracy = accuracy;
   OEM_PD_LeaveCS();

   /* get the client id for the given clsid */
   pMe->m_clientInUse = get_client_by_clsid(cls );
   DBGPRINTF( "==> ClientInUse %x",pMe->m_clientInUse );

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
    pdsm_opt.class_id = cls;
#endif
#ifdef FEATURE_CGPS          

   if( SUCCESS == nErr && FALSE == pdsm_get_position( (pdsm_pd_cmd_cb_f_type *)OEM_PD_CmdCB,
                                   (void *) pMe,  &pdsm_opt, &qos, 
                                   pMe->m_clientInUse ) ) {
      nErr = EFAILED;
   }
#else
   if( SUCCESS == nErr && FALSE == pdsm_get_position( (pdsm_pd_cmd_cb_f_type *)OEM_PD_CmdCB,
                                   (void *) pMe, svcType, &pdsm_opt, &qos,
                                   pMe->m_clientInUse ) ) {
      nErr = EFAILED;
   }
#endif /* FEATURE_CGPS */

   return nErr;
#else /* !FEATURE_GPSONE */
   return EUNSUPPORTED;
#endif /* FEATURE_GPSONE */
}

/*===========================================================================
FUNCTION: OEM_PD_EndSession

DESCRIPTION
   Ends Position session in progress.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_EndSession( OEMPosDet *pMe, boolean bStayWarm )
{
#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)
   pdsm_pd_event_type    theEvent;

   DBGPRINTF( "==> OEM_PD_EndSession" );
   VALIDATE_HANDLE( pMe );

   OEM_PD_EnterCS();
   theEvent = pMe->pd_event;
   OEM_PD_LeaveCS();
   if( bStayWarm ) {
#ifdef FEATURE_GPSONE_PE
      OEM_SW_End( pMe );
#endif
   }
   else if( /* PD session has been started but not done. */
       ((theEvent & (PDSM_PD_EVENT_BEGIN|PDSM_PD_EVENT_DONE)) == PDSM_PD_EVENT_BEGIN) ) {
      pdsm_end_session( (pdsm_pd_cmd_cb_f_type *)OEM_PD_CmdCB,
#ifdef FEATURE_PDSM_API_R2
                        PDSM_PD_END_SESSION_FIX_REQ,
#endif
                        pMe, pMe->m_clientInUse );
   }
   return SUCCESS;
#else /* !FEATURE_GPSONE */
   return EUNSUPPORTED;
#endif /* FEATURE_GPSONE */
}

#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)
/*===========================================================================
FUNCTION: OEM_XlateEndReason

DESCRIPTION
   Translates the end reasons to AEEGPS_ERR_*.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
void OEM_XlateEndReason( pdsm_pd_event_type pd_event, const pdsm_pd_info_s_type *pd_info_ptr,
                    AEEGPSInfo *pR )
{
   if( pd_event & PDSM_PD_EVENT_END ) {

      if( pd_info_ptr->pd_info.end_status == PDSM_PD_END_OFFLINE
          || pd_info_ptr->pd_info.end_status == PDSM_PD_END_NO_SRV
          || pd_info_ptr->pd_info.end_status == PDSM_PD_END_NO_CON
          || pd_info_ptr->pd_info.end_status == PDSM_PD_END_CDMA_LOCK
          || pd_info_ptr->pd_info.end_status == PDSM_PD_END_NO_DATA ) {

         pR->status = AEEGPS_ERR_SRV_UNREACHABLE;
      }
      else if( pd_info_ptr->pd_info.end_status == PDSM_PD_END_CON_FAIL
               || pd_info_ptr->pd_info.end_status == PDSM_PD_END_UI_END
               || pd_info_ptr->pd_info.end_status == PDSM_PD_END_TC_EXIT ) {

         pR->status = AEEGPS_ERR_LINK_FAILED;
      }
      else if( pd_info_ptr->pd_info.end_status == PDSM_PD_END_GPS_LOCK
               || pd_info_ptr->pd_info.end_status == PDSM_PD_END_PDE_REJECT
#ifdef FEATURE_PDSM_API_R3
               || pd_info_ptr->pd_info.end_status == PDSM_PD_END_SERVER_ERROR
#endif
               ) {

         pR->status = AEEGPS_ERR_REJECTED;
      }
      else if( pd_info_ptr->pd_info.end_status == PDSM_PD_END_ERR_STATE
               || pd_info_ptr->pd_info.end_status == PDSM_PD_END_SESS_BUSY ) {

         pR->status = AEEGPS_ERR_OUTOF_RESOURCES;
      }
#ifdef FEATURE_PDSM_API_R2
      else if( pd_info_ptr->pd_info.end_status == PDSM_PD_END_STALE_BS_INFO ) {
         pR->status = AEEGPS_ERR_STALE_BS_INFO;
      }
#endif
   }
}

/*===========================================================================
FUNCTION: OEM_PD_EventCB

DESCRIPTION
   This is the function called back by the pdsm task when a position event
   occurs.
   If we get a position, velocity, or altitude event, we copy it into a structure
   protected by a mutex, and resume so that subsequent processing can happen in
   the BREW context.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void  OEM_PD_EventCB(
#ifdef FEATURE_PDSM_API_R2
   OEMPosDet *pMe,
#endif /* FEATURE_PDSM_API_R2 */
   pdsm_pd_event_type pd_event,
   const pdsm_pd_info_s_type *pd_info_ptr
   )
{
   uint16            wPosDetEvent = POSDET_E_NONE;
   boolean           bSignal = FALSE;
#ifndef FEATURE_PDSM_API_R2
   OEMPosDet *pMe = g_pOEMPosDet;
#endif
   OEM_PD_EnterCS();

   /* sanity checking for the events received */
   if( g_pOEMPosDet == pMe && pd_info_ptr->client_id == pMe->m_clientInUse ) {

      DBGPRINTF( "==> OEM_PD_EventCB: 0x%x", pd_event );

#ifdef FEATURE_GPSONE_PE
#ifdef FEATURE_PDSM_API_R2
      /* If there was an UPDATE_FAILURE already recorded, then abort the session. */
      if( (pMe->pd_event & pd_event) & PDSM_PD_EVENT_UPDATE_FAILURE ) {
         wPosDetEvent |= POSDET_E_OEM_SESSION_ABORT;
         bSignal = TRUE;                        /* Trigger AEEPosDet */
      }
#endif
#endif

      /* Collect the position determination events */
      if( pd_event == PDSM_PD_EVENT_BEGIN ) {
         /* reset the prior data */
         pMe->pd_event = 0;
         pMe->theResponse.status = pMe->theResponse.fValid = 0;
     }
      pMe->pd_event |= pd_event;

#ifdef FEATURE_GPSONE_PE
#ifdef FEATURE_PDSM_API_R2
      /* Collect the stay warm related events */
      if( pd_event & (PDSM_PD_EVENT_DLOAD_BEGIN|PDSM_PD_EVENT_DLOAD|PDSM_PD_EVENT_DLOAD_DONE|PDSM_PD_DLOAD_EVENT_END) ) {
         if( pd_event == PDSM_PD_EVENT_DLOAD_BEGIN ) {
            /* reset the prior data */
            pMe->sw_event = 0;
         }
         pMe->sw_event |= pd_event;
      }
#endif
#endif

      if(pd_event & PDSM_PD_EVENT_POSITION) {
#if defined (FEATURE_MMGPS) || defined (FEATURE_CGPS)
         AEEGPSMethod XlateMode2Method[PDSM_PD_POSITION_MODE_MAX];

         memset(&XlateMode2Method, 0, sizeof(XlateMode2Method));

         XlateMode2Method[PDSM_PD_POSITION_MODE_MSBASED] = AEEGPS_METHOD_LOCAL | AEEGPS_METHOD_ASSISTED;
         XlateMode2Method[PDSM_PD_POSITION_MODE_MSASSISTED] = AEEGPS_METHOD_NETWORK | AEEGPS_METHOD_ASSISTED;
         XlateMode2Method[PDSM_PD_POSITION_MODE_STANDALONE] = AEEGPS_METHOD_LOCAL | AEEGPS_METHOD_STANDALONE;
#else // !FEATURE_MMGPS || FEATURE_CGPS
#ifdef FEATURE_GPSONE_LOCATION_METHOD
         AEEGPSMethod XlateMode2Method[PDSM_PD_POSITION_MODE_MAX];

         memset(&XlateMode2Method, 0, sizeof(XlateMode2Method));

         XlateMode2Method[PDSM_PD_POSITION_MODE_MSBASED] = AEEGPS_METHOD_LOCAL | AEEGPS_METHOD_ASSISTED;
         XlateMode2Method[PDSM_PD_POSITION_MODE_MSASSISTED] = AEEGPS_METHOD_NETWORK | AEEGPS_METHOD_ASSISTED;
         XlateMode2Method[PDSM_PD_POSITION_MODE_STANDALONE] = AEEGPS_METHOD_LOCAL | AEEGPS_METHOD_STANDALONE;
         XlateMode2Method[PDSM_PD_POSITION_MODE_SMARTMODE] = AEEGPS_METHOD_LOCAL;
#endif // FEATURE_GPSONE_LOCATION_METHOD
#endif // FEATURE_MMGPS

         pMe->theResponse.dwTimeStamp    = pd_info_ptr->pd_info.pd_data.time_stamp;
         pMe->theResponse.dwLat          = pd_info_ptr->pd_info.pd_data.lat;
         pMe->theResponse.dwLon          = pd_info_ptr->pd_info.pd_data.lon;
         pMe->theResponse.bHorUnc        = pd_info_ptr->pd_info.pd_data.loc_uncertainty_a;
         pMe->theResponse.bHorUncAngle   = pd_info_ptr->pd_info.pd_data.loc_uncertainty_ang;
         pMe->theResponse.bHorUncPerp    = pd_info_ptr->pd_info.pd_data.loc_uncertainty_p;
         pMe->theResponse.status        |= AEEGPS_GETINFO_LOCATION;

#if defined (FEATURE_MMGPS) || defined (FEATURE_CGPS)
         pMe->theResponse.LocProbability = pd_info_ptr->pd_info.pd_data.loc_uncertainty_conf;

         if (pd_info_ptr->pd_info.pd_data.opt_field_mask & PDSM_PD_UTCOFFSET_VALID) {
            pMe->theResponse.GPS_UTCOffset = pd_info_ptr->pd_info.pd_data.gpsUtcOffset;
            pMe->theResponse.fValid |= AEEGPS_VALID_UTCOFFSET;
         }
         else {
            pMe->theResponse.fValid &= (~AEEGPS_VALID_UTCOFFSET);
         }

         pMe->theResponse.method |= XlateMode2Method[pd_info_ptr->pd_info.pd_data.position_mode];

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_POSITION_SOURCE_GPS) {
            pMe->theResponse.method |= AEEGPS_METHOD_SATELLITE;
         }

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_POSITION_SOURCE_CELLID) {
            pMe->theResponse.method |= AEEGPS_METHOD_CELLID;
         }

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_POSITION_SOURCE_AFLT) {
            pMe->theResponse.method |= AEEGPS_METHOD_AFLT;
         }

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_LOCATION_SOURCE_HYBRID ) {
            pMe->theResponse.method |= (AEEGPS_METHOD_SATELLITE | AEEGPS_METHOD_AFLT);
         }
#else // 1x target
         /* <porting> For devices where the position determination engine does
         not provide the location probability information directly, the OEM can
         configure the value returned for the location probability.

         The location probability is determined by the position determination
         engine on the device.  For 1x MSM chipsets, the location probability is
         typically 39% or 68%.  The correct value to use must be verified based on
         the position determination engine in that device. </porting>
         */

         pMe->theResponse.LocProbability = 68;
#ifdef FEATURE_GPSONE_LOCATION_METHOD
         pMe->theResponse.method |= XlateMode2Method[pd_info_ptr->pd_info.pd_data.position_mode];

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_LOCATION_GPS) {
            pMe->theResponse.method |= AEEGPS_METHOD_SATELLITE;
         }

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_LOCATION_CELL_ID) {
            pMe->theResponse.method |= AEEGPS_METHOD_CELLID;
         }

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_LOCATION_AFLT) {
            pMe->theResponse.method |= AEEGPS_METHOD_AFLT;
         }

         if ( pd_info_ptr->pd_info.pd_data.positioning_source & PDSM_PD_LOCATION_HYBRID ) {
            pMe->theResponse.method |= (AEEGPS_METHOD_SATELLITE | AEEGPS_METHOD_AFLT);
         }
#endif // FEATURE_GPSONE_LCOATION_METHOD
#endif // FEATURE_MMGPS

#if MIN_PDSM_API_V(5,0)
         if (pd_info_ptr->pd_info.pd_data.opt_field_mask & PDSM_PD_MSEC_VALID) {
            pMe->theResponse.dwTimeMS = pd_info_ptr->pd_info.pd_data.time_stamp_msec;
            pMe->theResponse.fValid |= AEEGPS_VALID_TIMEMS;
         }
         else {
            pMe->theResponse.fValid &= (~AEEGPS_VALID_TIMEMS);
         }
#endif
      }

      if((pd_event &  PDSM_PD_EVENT_VELOCITY) &&
         (pd_info_ptr->pd_info.pd_data.opt_field_mask & PDSM_PD_VELOCITY_VALID)){
         pMe->theResponse.dwTimeStamp    = pd_info_ptr->pd_info.pd_data.time_stamp;
         pMe->theResponse.wVelocityHor   = pd_info_ptr->pd_info.pd_data.velocity_hor;
         pMe->theResponse.bVelocityVer   = pd_info_ptr->pd_info.pd_data.velocity_ver;
         pMe->theResponse.wHeading       = pd_info_ptr->pd_info.pd_data.heading;
         pMe->theResponse.fValid         = pMe->theResponse.fValid
            | AEEGPS_VALID_HVEL
            | AEEGPS_VALID_VVEL
            | AEEGPS_VALID_HEAD;
         pMe->theResponse.status        |= AEEGPS_GETINFO_VELOCITY;
      }

      if((pd_event & PDSM_PD_EVENT_HEIGHT) &&
         (pd_info_ptr->pd_info.pd_data.opt_field_mask & PDSM_PD_ALTITUDE_VALID)){
         pMe->theResponse.dwTimeStamp    = pd_info_ptr->pd_info.pd_data.time_stamp;
         pMe->theResponse.wAltitude      = pd_info_ptr->pd_info.pd_data.altitude;
         pMe->theResponse.wVerUnc        = pd_info_ptr->pd_info.pd_data.loc_uncertainty_v;
         pMe->theResponse.fValid        |= AEEGPS_VALID_ALT;
         pMe->theResponse.status        |= AEEGPS_GETINFO_ALTITUDE;
      }

      if(pd_event & PDSM_PD_EVENT_END) {

         OEM_XlateEndReason( pd_event, pd_info_ptr, &pMe->theResponse );
      }

      /* On those events that require actions trigger the Brew handler */
      if( (pd_event & (PDSM_PD_EVENT_DONE
# ifdef FEATURE_PDSM_DONE_EVENT_HACK1
                       /* In this report the session done on any one of these
                       ** events.
                       */
                       |PDSM_PD_EVENT_POSITION
                       |PDSM_PD_EVENT_HEIGHT
                       |PDSM_PD_EVENT_VELOCITY
                       |PDSM_PD_EVENT_END
# endif /* FEATURE_PDSM_DONE_EVENT_HACK1 */
                       )) != 0 ) {
         wPosDetEvent |= POSDET_E_OEM_SESSION_DONE;
         bSignal = TRUE;                        /* Trigger AEEPosDet */
      }
   }

   OEM_PD_LeaveCS();
   if( bSignal ) {

      PrvEvCB( pMe, wPosDetEvent );
   }
}

/*===========================================================================
FUNCTION: OEM_PD_CmdCB

DESCRIPTION
   This is the callback called by pdsm for pdsm_get_position, pdsm_end_session
   functions to report if the command was successfully queued. All we care
   about here is if pdsm_get_position came back with any other error other
   than success, session in progress. or busy. If it does, we cannot service
   our pending callbacks, so dispatch them all with error status.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_PD_CmdCB(
   OEMPosDet                 *pMe,
   pdsm_pd_cmd_e_type        pd_cmd,
   pdsm_pd_cmd_err_e_type    pd_cmd_err
   )
{
   DBGPRINTF( "==> OEM_PD_CmdCB" );

   OEM_PD_EnterCS();
   if( g_pOEMPosDet == pMe ) {
      if( pd_cmd ==  PDSM_PD_CMD_GET_POS ) {
         switch(pd_cmd_err) {
         case PDSM_PD_CMD_ERR_NOERR:
            /* Do nothing */
            break;
         case PDSM_PD_CMD_ERR_BUSY_S:
         case PDSM_PD_CMD_ERR_SESS_ACT_S:
            DBGPRINTF( "==> OEM_PD_CmdCB: Busy|Sess_act" );
            PrvEvCB( pMe, POSDET_E_OEM_SESSION_BUSY );
            break;
         default:
            /* Report Error. */
            PrvEvCB( pMe, POSDET_E_OEM_SESSION_ERROR );
            break;
         }
      }
      else if( pd_cmd == PDSM_PD_CMD_END_SESSION ) {
         if( pd_cmd_err == PDSM_PD_CMD_ERR_NO_SESS_S ) {

            PrvEvCB( pMe, POSDET_E_OEM_SESSION_DONE );
         }
      }
   }
   OEM_PD_LeaveCS();
}
#endif /* FEATURE_GPSONE  || FEATURE_PDAPI */

/*===========================================================================
FUNCTION: OEM_PD_Validate_GetGPSInfo

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS
  EBADPARM
  EUNSUPPORTED

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_Validate_GetGPSInfo( AEEGPSReq req, AEEGPSAccuracy accuracy )
{
   int nErr = SUCCESS;

   /*
   ** Check against different features of DMSS.
   */
   if( accuracy < AEEGPS_ACCURACY_LOWEST
       || accuracy > AEEGPS_ACCURACY_HIGHEST ) {
      nErr = EBADPARM;
   }
#if !defined(FEATURE_GPSONE) && !defined(FEATURE_PDAPI)
   else if( req & (AEEGPS_GETINFO_LOCATION | AEEGPS_GETINFO_VELOCITY
                   | AEEGPS_GETINFO_ALTITUDE) ) {
      nErr = EUNSUPPORTED;
   }
#endif /* !FEATURE_GPSONE && !FEATURE_PDAPI */

   return nErr;
}

/*===========================================================================
FUNCTION: OEM_PD_GetDefaultConfig

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS
  EBADPARM
  EUNSUPPORTED

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_GetDefaultConfig( AEEGPSConfig *pConfig, uint32 *pdwTimeout, uint32 *pdwLingerTimeout )
{
   if( pConfig ) {
      pConfig->mode           = AEEGPS_MODE_ONE_SHOT;
      pConfig->nFixes         = GPS_DEF_NUM_FIXES;
      pConfig->nInterval      = GPS_DEF_FIX_INTERVAL;
      pConfig->optim          = AEEGPS_OPT_DEFAULT;
      pConfig->qos            = GPS_DEF_QOS_VALUE;
      pConfig->server.svrType = AEEGPS_SERVER_DEFAULT;
   }

   if( pdwTimeout ) {
      *pdwTimeout = GPS_REQUEST_TIMEOUT;
   }

   if( pdwLingerTimeout ) {
      *pdwLingerTimeout = GPS_LINGER_TIMEOUT;
   }

   return SUCCESS;
}

/*===========================================================================
FUNCTION: OEM_PD_Validate_SetGPSConfig

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS
  EBADPARM
  EUNSUPPORTED

SIDE EFFECTS
  none
===========================================================================*/
static int OEM_PD_Validate_SetGPSConfig_Int( const AEEGPSConfig *pConfig )
{
#if defined( FEATURE_GPSONE_VX_LCS_AGENT) && !defined (FEATURE_CGPS)
   lcs_agent_vx_config_s_type vx_config;
   boolean trustness_setting = FALSE;

   /* Since there is no easy way to figure out the exact client type,
      we querry all client types which can possibly call this funtion.
      Reject tracking session only when the phone is set to V1 and all
      client types are set to non-trusted.
   */
   pdsm_lcs_querry_vx_config(PDSM_CLIENT_TYPE_BREW, &vx_config);
   trustness_setting |= vx_config.trustedApp;

   pdsm_lcs_querry_vx_config(PDSM_CLIENT_TYPE_BROWSER, &vx_config);
   trustness_setting |= vx_config.trustedApp;

   pdsm_lcs_querry_vx_config(PDSM_CLIENT_TYPE_JAVA, &vx_config);
   trustness_setting |= vx_config.trustedApp;
#endif

   /*
   ** Check against different features of DMSS.
   */
#if defined(FEATURE_GPSONE) || defined(FEATURE_PDAPI)
   if( pConfig->mode != AEEGPS_MODE_ONE_SHOT
       && pConfig->mode != AEEGPS_MODE_DLOAD_FIRST
       && pConfig->mode != AEEGPS_MODE_TRACK_LOCAL
       && pConfig->mode != AEEGPS_MODE_TRACK_NETWORK
       && pConfig->mode != AEEGPS_MODE_TRACK_OPTIMAL
       && pConfig->mode != AEEGPS_MODE_TRACK_STANDALONE ){
      return EBADPARM;
   }
# ifndef FEATURE_GPSONE_PE
   if( pConfig->mode == AEEGPS_MODE_DLOAD_FIRST ) {
      return EUNSUPPORTED;
   }

   if( pConfig->mode == AEEGPS_MODE_TRACK_LOCAL ) {
      return EUNSUPPORTED;
   }
# endif/*FEATURE_GPSONE_PE*/

# ifndef FEATURE_GPSONE_STANDALONE
   if( pConfig->mode == AEEGPS_MODE_TRACK_STANDALONE ) {
      return EUNSUPPORTED;
   }
# endif /* FEATURE_GPSONE_STANDALONE */

#if defined( FEATURE_GPSONE_VX_LCS_AGENT) && !defined (FEATURE_CGPS)
   if((trustness_setting == FALSE) && (vx_config.vxVersion == PDSM_LCS_AGENT_VERSION_V1_ONLY))
   {
     if ((pConfig->nFixes > 1) || (pConfig->mode != AEEGPS_MODE_ONE_SHOT))
     {
       return EUNSUPPORTED;
     }
   }
#endif

   if( pConfig->server.svrType > AEEGPS_SERVER_DBURST ) {
      return EBADPARM;
   }

   if( pConfig->server.svrType == AEEGPS_SERVER_DBURST ) {
      return EUNSUPPORTED;
   }

   if( pConfig->optim > AEEGPS_OPT_PAYLOAD ) {
      return EBADPARM;
   }

#else  /* !FEATURE_GPSONE && !FEATURE_PDAPI */
   return EUNSUPPORTED;
#endif /* FEATURE_GPSONE && !FEATURE_PDAPI */

   return SUCCESS;
}

int OEM_PD_Validate_SetGPSConfig( const AEEGPSConfig *pConfig )
{
   int nErr = OEM_PD_Validate_SetGPSConfig_Int(pConfig);

#ifdef AEE_SIMULATOR
   if( nErr == EUNSUPPORTED ) {
      /* Support for all the modes on simulator */
      nErr = SUCCESS;
   }
#endif

#ifdef _DEBUG_LOW
   if( pConfig->server.svrType != AEEGPS_SERVER_DEFAULT ) {
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: Non Default Server" );
   }
   switch( pConfig->mode ) {
   case AEEGPS_MODE_ONE_SHOT:
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: AEEGPS_MODE_ONE_SHOT" );
      break;
   case AEEGPS_MODE_DLOAD_FIRST:
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: AEEGPS_MODE_DLOAD_FIRST" );
      break;
   case AEEGPS_MODE_TRACK_LOCAL:
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: AEEGPS_MODE_TRACK_LOCAL" );
      break;
   case AEEGPS_MODE_TRACK_NETWORK:
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: AEEGPS_MODE_TRACK_NETWORK" );
      break;
   case AEEGPS_MODE_TRACK_OPTIMAL:
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: AEEGPS_MODE_TRACK_OPTIMAL" );
      break;
   case AEEGPS_MODE_TRACK_STANDALONE:
      DBGPRINTF( "==> OEM_Validate_SetGPSConfig: AEEGPS_MODE_TRACK_STANDALONE" );
      break;
   }
#endif /* _DEBUG_LOW */
   return nErr;
}


/*===========================================================================
FUNCTION: OEM_PD_GetPositionInfo

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int OEM_PD_GetPositionInfo( OEMPosDet *pMe, AEEGPSInfo *pInfo )
{
   VALIDATE_HANDLE( pMe );
   OEM_PD_EnterCS();
   MEMCPY( pInfo, &pMe->theResponse, sizeof( AEEGPSInfo ) );
   OEM_PD_LeaveCS();
   return SUCCESS;
}

/* TRUE if device shares the GPS receiver with voice communication */
boolean OEM_PD_SharedVoiceReceiver( void )
{
   /*
   <porting> Update this depending on the GPS receiver setup in relation
   to the voice communication. If the device has a shared reciever, and
   tune-aways in voice communication during a GPS position request are 
   not acceptable, return TRUE. </porting>
   */

   return FALSE;
}

#if defined (AEE_SIMULATOR) || defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
/*===========================================================================
FUNCTION: OEM_PD_SignExtend_DWORD

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int32 OEM_PD_SignExtend_DWORD( dword dwVal, byte size )
{
   if( dwVal & (1 << --size) ) {
      while( ++size < (sizeof(dword) << 3) ) {
         dwVal |= (1 << size);
      }
   }
   return (int32)dwVal;
}
#endif /* defined (AEE_SIMULATOR) || defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900) */

#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI) 
#if !defined(FEATURE_CGPS)
/*===========================================================================
FUNCTION: OEM_XlateOptions

DESCRIPTION
   Translates the AEEPosDet options to pdsm options.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_XlateOptions( const AEEGPSConfig *pConfig, pdsm_pd_option_s_type *pOpt )
{

   ZEROAT(pOpt);

   switch(pConfig->mode) {
#ifdef FEATURE_GPSONE_PE
   case AEEGPS_MODE_DLOAD_FIRST:
      pOpt->session                                 = PDSM_PD_SESS_TYPE_DATA_DLOAD;
      pOpt->session_info.operation_mode             = PDSM_SESSION_OPERATION_PDE_CALC_ONLY;
      pOpt->session_info.data_download.dload_option = PDSM_DATA_DLOAD_SINGLE;
      pOpt->session_info.data_download.duration     = (pConfig->nFixes * pConfig->nInterval)/60;

      break;
   case AEEGPS_MODE_TRACK_LOCAL:
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session                                 = PDSM_PD_SESS_TYPE_TRACK_IND;
#else
      pOpt->session                                 = PDSM_PD_SESS_TYPE_NEW;
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
      pOpt->session_info.operation_mode             = PDSM_SESSION_OPERATION_MSBASED;
      pOpt->session_info.data_download.dload_option = PDSM_DATA_DLOAD_SINGLE;
      pOpt->session_info.data_download.duration     = (pConfig->nFixes * pConfig->nInterval)/60;

      break;
   case AEEGPS_MODE_TRACK_STANDALONE:
      pOpt->session                                 = PDSM_PD_SESS_TYPE_NEW;
      pOpt->session_info.operation_mode             = PDSM_SESSION_OPERATION_STANDALONE_ONLY;

      break;

#endif /* FEATURE_GPSONE_PE */
   case AEEGPS_MODE_TRACK_NETWORK:
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session                                 =  PDSM_PD_SESS_TYPE_TRACK_IND;
#else
      pOpt->session =  PDSM_PD_SESS_TYPE_NEW;
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
#ifdef FEATURE_PDSM_API_R2
      pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_PDE_CALC_ONLY;
#endif /* FEATURE_PDSM_API_R2 */
      break;
   case AEEGPS_MODE_TRACK_OPTIMAL:
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session                                 = PDSM_PD_SESS_TYPE_TRACK_IND;
#else
      pOpt->session =  PDSM_PD_SESS_TYPE_NEW;
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
#ifdef FEATURE_PDSM_API_R2
      if( pConfig->optim == AEEGPS_OPT_SPEED ) {
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
      }
      else if( pConfig->optim == AEEGPS_OPT_NONE ) {
#ifdef FEATURE_MMGPS
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_BESTPOSITIONMODE;
#else
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_PDE_CALC_ONLY;
#endif
      }
      else if( pConfig->optim == AEEGPS_OPT_PAYLOAD ) {
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_DATA;
      }
      else {
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;
      }
#endif /* FEATURE_PDSM_API_R2 */

      break;
   case AEEGPS_MODE_ONE_SHOT:

   default:
      pOpt->session =  PDSM_PD_SESS_TYPE_NEW;
#ifdef FEATURE_PDSM_API_R2
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_PDE_CALC_ONLY;
#else
      if( pConfig->optim == AEEGPS_OPT_SPEED ) {
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
      }
      else if( pConfig->optim == AEEGPS_OPT_NONE ) {
#ifdef FEATURE_MMGPS
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_BESTPOSITIONMODE;
#else
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_PDE_CALC_ONLY;
#endif
      }
      else if( pConfig->optim == AEEGPS_OPT_PAYLOAD ) {
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_DATA;
      }
      else {
         pOpt->session_info.operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;
      }
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
#endif /* FEATURE_PDSM_API_R2 */

      break;
   }

#ifdef FEATURE_PDSM_API_R2
   pOpt->fix_rate.num_fixes                      = (pConfig->nFixes == 0) ? 0xFFFFFFFF : pConfig->nFixes;
   pOpt->fix_rate.time_between_fixes             = pConfig->nInterval;

   pOpt->privacy                                 = PDSM_PD_PRIVACY_LEVEL_0; /* Will result in
   ** using default privacy from NV */
#endif /* FEATURE_PDSM_API_R2 */

   switch(pConfig->server.svrType) {
   case AEEGPS_SERVER_IP:
      pOpt->lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
      pOpt->lsinfo.server_adrs   = AEE_ntohl( pConfig->server.svr.ipsvr.addr ); /* Converted to
      ** host byte order for pdsm */
      if( pConfig->server.svr.ipsvr.port == 0 ) {
         pOpt->lsinfo.server_port = 4911;  /* in host byte order */
      }
      else {
         /* Convert to host byte order for pdsm */
         pOpt->lsinfo.server_port   = AEE_ntohs( pConfig->server.svr.ipsvr.port );
      }
      break;

   case AEEGPS_SERVER_DBURST:
      pOpt->lsinfo.server_option = PDSM_SERVER_OPTION_USE_DBM;
      break;

   default:
      pOpt->lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
      break;
   }
}

#else
/*===========================================================================
FUNCTION: OEM_XlateOptions

DESCRIPTION
   Translates the AEEPosDet options to pdsm options.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_XlateOptions( const AEEGPSConfig *pConfig, pdsm_pd_option_s_type *pOpt )
{

   ZEROAT(pOpt);

   switch(pConfig->mode) {
   case AEEGPS_MODE_DLOAD_FIRST:
      pOpt->session                                 = PDSM_PD_SESS_TYPE_DATA_DLOAD;
      pOpt->operation_mode                          = PDSM_SESSION_OPERATION_MSASSISTED;
      break;
   case AEEGPS_MODE_TRACK_LOCAL:
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session                                 = PDSM_PD_SESS_TYPE_TRACK_IND;
#else
      pOpt->session                                 = PDSM_PD_SESS_TYPE_NEW;
#endif
      pOpt->operation_mode                          = PDSM_SESSION_OPERATION_MSBASED;
      break;
   case AEEGPS_MODE_TRACK_STANDALONE:
      pOpt->session                                 = PDSM_PD_SESS_TYPE_NEW;
      pOpt->operation_mode                          = PDSM_SESSION_OPERATION_STANDALONE_ONLY;
      break;

   case AEEGPS_MODE_TRACK_NETWORK:
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session                                 = PDSM_PD_SESS_TYPE_TRACK_IND;
#else
      pOpt->session =  PDSM_PD_SESS_TYPE_NEW;
#endif
      pOpt->operation_mode                          = PDSM_SESSION_OPERATION_MSASSISTED;
      break;
   case AEEGPS_MODE_TRACK_OPTIMAL:
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      pOpt->session                                 = PDSM_PD_SESS_TYPE_TRACK_IND;
#else
      pOpt->session =  PDSM_PD_SESS_TYPE_NEW;
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */

      if( pConfig->optim == AEEGPS_OPT_SPEED ) {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
      }
      else if( pConfig->optim == AEEGPS_OPT_NONE ) {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_MSBASED;
      }
      else if( pConfig->optim == AEEGPS_OPT_PAYLOAD ) {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_DATA;
      }
      else {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;
      }
      break;
   case AEEGPS_MODE_ONE_SHOT:
      
   default:
      pOpt->session =  PDSM_PD_SESS_TYPE_NEW;
#ifdef FEATURE_GPSONE_VX_LCS_AGENT
      /* for VX, set the operation mode to MS Assisted */
      pOpt->operation_mode = PDSM_SESSION_OPERATION_MSASSISTED;
#else
      if( pConfig->optim == AEEGPS_OPT_SPEED ) {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_SPEED;
      }
      else if( pConfig->optim == AEEGPS_OPT_NONE ) {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_MSBASED;
      }
      else if( pConfig->optim == AEEGPS_OPT_PAYLOAD ) {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_DATA;
      }
      else {
         pOpt->operation_mode = PDSM_SESSION_OPERATION_OPTIMAL_ACCURACY;
      }
#endif /* FEATURE_GPSONE_VX_LCS_AGENT */
      break;
   }

   pOpt->fix_rate.num_fixes                      = (pConfig->nFixes == 0) ? 0xFFFFFFFF : pConfig->nFixes;
   pOpt->fix_rate.time_between_fixes             = pConfig->nInterval;
   

   switch(pConfig->server.svrType) {
   case AEEGPS_SERVER_IP:
      pOpt->lsinfo.server_option = PDSM_SERVER_OPTION_USE_LOCAL;
      pOpt->lsinfo.server_adrs.server_addr_type = PDSM_SERVER_TYPE_IPV4_ADDRESS;
      pOpt->lsinfo.server_adrs.server_adrs.server_adrs_v4.server_adrs   = AEE_ntohl( pConfig->server.svr.ipsvr.addr ); /* Converted to
      ** host byte order for pdsm */
      if( pConfig->server.svr.ipsvr.port == 0 ) {
         pOpt->lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id = 4911;  /* in host byte order */
      }
      else {
         /* Convert to host byte order for pdsm */
         pOpt->lsinfo.server_adrs.server_adrs.server_adrs_v4.port_id   = AEE_ntohs( pConfig->server.svr.ipsvr.port ); 
      }
      break;
      
   default:
      pOpt->lsinfo.server_option = PDSM_SERVER_OPTION_USE_DEFAULT;
      break;
   }
}
#endif /* FEATURE_CGPS */
#endif /* defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)  */

#if defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI)
/*===========================================================================
FUNCTION: PrvEvCB

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void PrvEvCB( OEMPosDet *pMe, PosDetEventType theEvent )
{
   pMe->pfnNotify( pMe->pNotifyData, theEvent );
}
#endif /* defined (FEATURE_GPSONE) || defined (FEATURE_PDAPI) */

/*===========================================================================
FUNCTION: OEM_OR_GetOrientation

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
int OEM_OR_GetOrientation( AEEOrientationReq req, void *pInfo, AEECallback *pcb )
{
#ifdef AEE_SIMULATOR
   if( req == AEEORIENTATION_REQ_AZIMUTH ) {
      AEEOrientationAzimuthInfo *pOr = (AEEOrientationAzimuthInfo *)pInfo;

      if( pOr->wSize >= sizeof(AEEOrientationAzimuthInfo) ) {

         pOr->wAzimuth = (120 << 6) | (12 & 0x003F);
         pOr->dwTimeStamp = 0;
         AEE_SYS_RESUME( pcb );
         return SUCCESS;
      }

      return EBADPARM;
   }
#endif
   return EUNSUPPORTED;
}

#if defined (FEATURE_GPSONE_PE) || defined (FEATURE_PDAPI)
/*===========================================================================
FUNCTION: OEM_SW_Start

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int OEM_SW_Start( OEMPosDet *pMe, AEECLSID cls )
{
#ifdef FEATURE_PDSM_API_R4
   uint32        dwVal = 0xFFFFFFFF;
   pMe->bSwState = pdsm_set_parameters( NULL, NULL, PDSM_PA_KEEP_WARM, (pdsm_pa_set_type *)&dwVal, pMe->m_clientInUse );

   return SUCCESS;
#else
   int nErr = SUCCESS;
   DBGPRINTF( "==> OEM_SW_Start" );
   VALIDATE_HANDLE( pMe );

   if( !pMe->bSwState ) {
      pdsm_pd_qos_type        qos;
      pdsm_pd_svc_type        svcType = 0;
      pdsm_pd_option_s_type   pdsm_opt;

      memset(&qos, 0, sizeof(qos));
      qos.accuracy_threshold                             = 1000;
#ifdef  FEATURE_CGPS
      qos.gps_session_timeout                            = 20;
#else
      qos.performance                                    = 20;
#endif /*FEATURE_CGPS */
      svcType                                            = PDSM_PD_SVC_TYPE_POSITION;

      memset(&pdsm_opt, 0, sizeof(pdsm_opt));
      pdsm_opt.session                                   = PDSM_PD_SESS_TYPE_DATA_DLOAD;
#ifdef  FEATURE_CGPS
      pdsm_opt.operation_mode                            = PDSM_SESSION_OPERATION_MSASSISTED;
#else
      pdsm_opt.session_info.operation_mode               = PDSM_SESSION_OPERATION_PDE_CALC_ONLY;
      pdsm_opt.session_info.data_download.dload_option   = PDSM_DATA_DLOAD_PERIODIC;
      pdsm_opt.session_info.data_download.duration       = 0xFFFF;
#endif /*FEATURE_CGPS */
      
      pdsm_opt.fix_rate.num_fixes                        = 0xFFFFFFFF;
      pdsm_opt.fix_rate.time_between_fixes               = 0;
#ifndef  FEATURE_CGPS
      pdsm_opt.privacy                                   = PDSM_PD_PRIVACY_LEVEL_0;
#endif /*FEATURE_CGPS*/
      pdsm_opt.lsinfo.server_option                      = PDSM_SERVER_OPTION_USE_DEFAULT;

      /* get the client id for the given clsid */
      pMe->m_clientInUse = get_client_by_clsid(cls );
      DBGPRINTF( "==> ClassId %x",cls );
      DBGPRINTF( "==> ClientInUse %x",pMe->m_clientInUse );

#ifdef FEATURE_GPSONE_VX_LCS_AGENT
    pdsm_opt.class_id = cls;
#endif
#ifdef FEATURE_CGPS
      if( TRUE == pdsm_get_position( (pdsm_pd_cmd_cb_f_type *)OEM_SW_CmdCB,
                                     (void *) pMe, &pdsm_opt, &qos, 
                                     pMe->m_clientInUse  ) ) {
#else                               
      if( TRUE == pdsm_get_position( (pdsm_pd_cmd_cb_f_type *)OEM_SW_CmdCB,
                                     (void *) pMe, svcType, &pdsm_opt, &qos, 
                                     pMe->m_clientInUse  ) ) {
#endif /*FEATURE_CGPS*/
         
         pMe->bSwState = TRUE;                        /* Start the stay warm state */
      }
      else {

         nErr = EFAILED;
      }
   }

   if( pMe->bSwState ) {

      PrvEvCB( pMe, POSDET_E_OEM_SESSION_DOWNLOAD_DONE );
   }

   return nErr;
#endif
}

/*===========================================================================
FUNCTION: OEM_SW_End

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static int OEM_SW_End( OEMPosDet *pMe )
{
#ifdef FEATURE_PDSM_API_R4
   uint32        dwVal = 0x0;
#endif
   int nErr = SUCCESS;

   DBGPRINTF( "==> OEM_SW_End" );
   VALIDATE_HANDLE( pMe );

#ifdef FEATURE_PDSM_API_R4
   if( FALSE == pdsm_set_parameters( NULL, NULL, PDSM_PA_KEEP_WARM,
                                     (pdsm_pa_set_type *)&dwVal, pMe->m_clientInUse  ) ) {
      nErr = EFAILED;
   }
#else
#ifdef FEATURE_PDSM_API_R2
   if( FALSE == pdsm_end_session( (pdsm_pd_cmd_cb_f_type *)OEM_SW_CmdCB,
                                  PDSM_PD_END_SESSION_DLOAD_REQ,
                                  pMe,
                                  pMe->m_clientInUse   ) ) {
      nErr = EFAILED;
   }
#endif
#endif

   if( SUCCESS == nErr ) {
      pMe->bSwState = FALSE;
   }
   return nErr;
}

#if defined (FEATURE_GPSONE_PE) || defined (FEATURE_PDAPI)
#if !defined(FEATURE_PDSM_API_R4)
/*===========================================================================
FUNCTION: OEM_SW_CmdCB

DESCRIPTION
   This is the callback called by pdsm for stay warm requests.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none
===========================================================================*/
static void OEM_SW_CmdCB(
   OEMPosDet                 *pMe,
   pdsm_pd_cmd_e_type        pd_cmd,
   pdsm_pd_cmd_err_e_type    pd_cmd_err
   )
{
   DBGPRINTF( "==> OEM_SW_CmdCB" );

   OEM_PD_EnterCS();
   if( g_pOEMPosDet == pMe ) {
      if( pd_cmd ==  PDSM_PD_CMD_GET_POS ) {
      }
      else if( pd_cmd == PDSM_PD_CMD_END_SESSION ) {
      }
   }
   OEM_PD_LeaveCS();
}
#endif // !defined(FEATURE_PDSM_API_R4)
#endif // defined (FEATURE_GPSONE_PE) || defined (FEATURE_PDAPI)

#endif

