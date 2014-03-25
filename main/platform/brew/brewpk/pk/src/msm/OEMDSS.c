/*=============================================================================

FILE: OEMDSS.c

SERVICES: OEM DSS backwards compatibility layer

GENERAL DESCRIPTION:
Definitions, typedefs, etc. for newer DSS functions that do not exist in older distributions
  
PUBLIC CLASSES AND STATIC FUNCTIONS:
   dss_getsockname
   dss_getpeername
   dss_get_iface_id
   dss_iface_ioctl
   dss_shutdown
   dss_listen
   dss_accept

INITIALIZATION AND SEQUENCING REQUIREMENTS:
   None

        Copyright © 2003-2008 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
=============================================================================*/

#include "OEMSock.h"
#include "OEMSock_priv.h"       // OEM_PROVIDES_*, etc.
#include "OEMTAPI.h"
#include "OEMConfig.h"
#include "AEEError.h"
#include "AEEStdLib.h"
#include "AEE_OEMNet.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEM.h"
#include "OEMHeap.h"

#if !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)
#include "dsiface.h"
#endif // !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)

#if (defined DSS_VERSION) && \
    ( ((DSS_VERSION >= 1005) && (DSS_VERSION <= 1099) && (defined FEATURE_DATA_PS_WLAN_QOS)) || \
       (DSS_VERSION >= 1204) )
// OEM Supports the QoS Flow option: WLAN User Priority
#define QOS_FLOW_OPT_WLAN_USER_PRIORITY_SUPPORTED
#endif

#if (defined DSS_VERSION) && \
    ( ((DSS_VERSION >= 1007) && (DSS_VERSION <= 1099) && (defined FEATURE_DATA_PS_WLAN_QOS)) || \
      (DSS_VERSION >= 1400) )   
// OEM Supports the WLAN Intervals QoS Flow options: 
// Min Service Interval,
// Max Service Interval,
// Inactivity  Interval
#define QOS_FLOW_OPT_WLAN_INTERVALS_SUPPORTED
// OEM Supports Nominal SDU Size option:
#define QOS_FLOW_OPT_NOMINAL_SDU_SIZE_SUPPORTED
#endif

#if (defined DSS_VERSION) && (DSS_VERSION >= 1500)
#define QOS_FLOW_OPT_CDMA_FLOW_PRIORITY_SUPPORTED
#endif

#if (defined DSS_VERSION) && (DSS_VERSION >= 1100)
#define QOS_AUX_FLOWS_SUPPORTED
#endif // (defined DSS_VERSION) && (DSS_VERSION >= 1100)

#if (defined DSS_VERSION) && ((DSS_VERSION >= 1700) || (defined OLD_DS_NET_POLICY_SUPPORTED))
#define CDMA_DATA_SESSION_PROFILE_ID_SUPPORTED
#endif // (defined DSS_VERSION) && ((DSS_VERSION >= 1700) || (defined OLD_DS_NET_POLICY_SUPPORTED))

//
// Conditional Debug Output
//

#define FARF_EVENT             1
#define FARF_TAPISTATE         1
#define FARF_CLOSE_OFFLINE_PPP 1

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p 

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

#if defined(TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE)
typedef struct
{
   OEMTAPI*                   piTAPI; /* monitors call status for dormancy, 
                                         dropped calls, etc. */
   int                        nTAPIRefCount; // Reference counter for piTAPI
   AEECallback                cbFreeTAPI; // exit handler

#ifdef CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE
   AEECallback                cbCloseOfflinePPP; // offline timer
#endif // CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE

   boolean                    bUp; // physical link is currently up
   dss_iface_ioctl_ev_cb_type cb;  // OEMNet interface events callback 

} DSSNet;
/*===========================================================================

             Globals

===========================================================================*/

static DSSNet gNet = {0};

#endif // defined(TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE)

#ifdef OEMDSS_NETLIB2_NOT_SUPPORTED
dss_net_cb_fcn  gpfnNetEventOccurred;    // network callback function 
dss_sock_cb_fcn gpfnSocketEventOccurred; // socket callback function 
#endif // OEMDSS_NETLIB2_NOT_SUPPORTED

/*===========================================================================

                      FUNCTION DEFINITIONS

 ===========================================================================*/

#if !defined(OEM_PROVIDES_SERVER_SOCKETS)

/*===========================================================================
FUNCTION DSS_LISTEN()
===========================================================================*/
sint15 dss_listen
(
  sint15 sockfd,                                      /* Socket descriptor */
  sint15 backlog,                      /* Number of connections to backlog */
  sint15 *dss_errno                               /* error condition value */
)
{
   *dss_errno = DS_EOPNOTSUPP;
   return DSS_ERROR;
}

/*===========================================================================
FUNCTION DSS_ACCEPT()
===========================================================================*/
sint15 dss_accept 
(
  sint15 sockfd,                                      /* Socket descriptor */
  struct sockaddr *remoteaddr,                       /* new remote address */
  uint16 *addrlen,                                   /* length of servaddr */
  sint15 *dss_errno                               /* error condition value */
)
{
   *dss_errno = DS_EOPNOTSUPP;
   return DSS_ERROR;
}

#endif // !defined(OEM_PROVIDES_SERVER_SOCKETS)

#if !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)

/*===========================================================================
FUNCTION DSS_GETSOCKNAME()
===========================================================================*/
sint15 dss_getsockname
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
)
{
   struct scb_s  *pScbType;
   struct sockaddr_in* psin = (struct sockaddr_in*)addr;

   if ((pScbType = dssocki_sockfdtoscb(sockfd)) == NULL) {
      return AEE_NET_EBADF;
   }

   if (*addrlen < sizeof(struct sockaddr_in)) {
      return AEE_NET_EFAULT;
   }
   *addrlen = sizeof(struct sockaddr_in);

   psin->sin_family = AF_INET;
   psin->sin_port = HTONS(pScbType->localname.port);
   psin->sin_addr.s_addr = (uint32)HTONL(pScbType->localname.address);

   return DSS_SUCCESS;   
}

/*===========================================================================
FUNCTION DSS_GETPEERNAME()
===========================================================================*/
sint15 dss_getpeername
(
  sint15           sockfd,                  /* socket descriptor           */
  struct sockaddr* addr,                    /* address of the socket       */
  uint16*          addrlen,                 /* address length              */
  sint15*          dss_errno                /* error number                */
)
{
   struct scb_s    *pScbType;
   struct sockaddr_in* psin = (struct sockaddr_in*)addr;
      
   if ((pScbType = dssocki_sockfdtoscb(sockfd)) == NULL) {
      return AEE_NET_EBADF;
   }
      
   if (*addrlen < sizeof(struct sockaddr_in)) {
      return AEE_NET_EFAULT;
   }
   *addrlen = sizeof(struct sockaddr_in);

   psin->sin_family = AF_INET;
   psin->sin_port = HTONS(pScbType->servname.port);

   if (0 == psin->sin_port) {
      return AEE_NET_ENOTCONN;
   }

   psin->sin_addr.s_addr = (uint32)HTONL(pScbType->servname.address);

   return DSS_SUCCESS;   
}

/*===========================================================================
FUNCTION DSS_GET_IFACE_ID()
===========================================================================*/
int dss_get_iface_id
(
  dss_id_info_type id,
  dss_iface_ioctl_id_type *iface_id 
)
{
   switch (id.type) {
      case DSS_APP_ID:
         iface_id->name = DSS_IFACE_CDMA_SN;
         iface_id->instance = id.info.app_id;
         return 0;

      case DSS_SOCK_FD:         // currently not used
      default:
         return -1;
   }
}

#endif // !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)

/*===========================================================================

 ===========================================================================*/

#ifdef CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE
static void OEMDSS_CloseOfflinePPP(void *unused)
{   

   FARF(CLOSE_OFFLINE_PPP,("close offline PPP"));
   OEMNet_CloseAllNets(); 
}
#endif // CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE

#if defined(TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE)

/*===========================================================================
  This is a static callback from OEMTAPI.c, invoked every time the CM
  task notifies TAPI about a state change (call, system, or phone)
 ===========================================================================*/
void OEMDSS_OnCallStatus(void)
{
   TAPIStatus ts;

   FARF(EVENT,("OCS netevent"));

   // Get the status according to TAPI
   if (gNet.piTAPI && SUCCESS == OEMTAPI_GetStatus(gNet.piTAPI,&ts)) {
      boolean bOffline = (PS_OFFLINE == ts.state) || ((PS_IDLE == ts.state) && !ts.bDigital);
      boolean bUp = (PS_CONVERSATION == ts.state) && ts.bData;

#if !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)
 
      // has the phys link state changed?
      if (bUp != gNet.bUp) {
         dss_iface_ioctl_ev_cb_type* pCb = &gNet.cb;

         // notify registered callback
         if (pCb->event_cb) {
            dss_iface_ioctl_event_info_union_type u;
            dss_iface_ioctl_event_enum_type event;

            event = bUp ? DSS_IFACE_IOCTL_PHYS_LINK_UP_EV : DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV;
            u.phys_link_state_info = bUp ? PHYS_LINK_UP : PHYS_LINK_DOWN; 
            pCb->event_cb(event, u, pCb->user_data_ptr, pCb->app_id,
                          DSS_IFACE_IOCTL_CDMA_SN, 0);
         }

         // remember new state
         gNet.bUp = bUp;
      }
#endif   //!defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)

      // 5100 (at least) has a bug where PPP will get stuck if digital
      // service is lost.  If the call state has gone offline, start a
      // timer to close PPP, unless the call state comes back online
      if (bOffline &&
          !CALLBACK_IsQueued(&gNet.cbCloseOfflinePPP)) {
         uint32 dwOfflinePPPTimeout;

         // Default to 60 seconds
         if (SUCCESS != OEM_GetConfig(CFGI_OFFLINE_PPP_TIMEOUT,
                                      (void *)&dwOfflinePPPTimeout, sizeof(dwOfflinePPPTimeout))) {
            dwOfflinePPPTimeout = 60000;
         }

         FARF(CLOSE_OFFLINE_PPP,("set offline PPP timer"));
         CALLBACK_Init(&gNet.cbCloseOfflinePPP, OEMDSS_CloseOfflinePPP, 0);
         AEE_SetSysTimerCallback(dwOfflinePPPTimeout, &gNet.cbCloseOfflinePPP);
      } else if (CALLBACK_IsQueued(&gNet.cbCloseOfflinePPP) &&
                 bUp) {
         FARF(CLOSE_OFFLINE_PPP,("cancel offline PPP timer"));
         CALLBACK_Cancel(&gNet.cbCloseOfflinePPP);
      }
   }
}

void OEMDSS_FreeTAPI(void *p)
{
   (void)p;

   if ((OEMTAPI *)0 != gNet.piTAPI) {
      // Update TAPI reference counter
      if (--gNet.nTAPIRefCount) {
         return;
      }

      CALLBACK_Cancel(&gNet.cbFreeTAPI);
      OEMTAPI_Destroy(gNet.piTAPI);
      gNet.piTAPI = 0;
   }
}

void OEMDSS_SetupTAPI(void)
{
   if (((OEMTAPI *)0 == gNet.piTAPI) &&
       (SUCCESS == OEMTAPI_New(&gNet.piTAPI))) {
      /* Register for notification on AEE_Exit()... */
      CALLBACK_Init(&gNet.cbFreeTAPI, OEMDSS_FreeTAPI, 0);
      AEE_RegisterSystemCallback(&gNet.cbFreeTAPI, AEE_SCB_AEE_EXIT, 0);
   }
   // Update TAPI reference counter 
   // (even if New operation fails - since a FreeTapi call is 
   //  expected to follow a SetupTAPI call)
   ++gNet.nTAPIRefCount;
}

#endif //defined(TRACK_TAPI_STATE_FOR_DATA_NETWORK_USE)

#if !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)

static sint15 DSIFaceToDSSIFaceIOCtl_Err(dsiface_error_type err)
{
/*===========================================================================
===========================================================================*/

   //Store mapping between dsiface Error Codes and dss_iface_ioctl Error codes.
   static const struct {
      dsiface_error_type from;  // dsiface Error Code
      sint15              to;   // dss_iface_ioctl Error Code
   } map[] =
   {
      {DSIFACE_BAD_OPT_NAME   , DS_EINVAL   },
      {DSIFACE_BAD_IFACE      , DS_EBADF    },
      {DSIFACE_BAD_PARAM      , DS_EFAULT   },
      {DSIFACE_BAD_LEN        , DS_EFAULT   },
      {DSIFACE_BAD_ARG_VAL    , DS_EFAULT   },
      {DSIFACE_SUCCESS        , DSS_SUCCESS }
   };

   int   i;
   sint15 DSS_Err = DSS_ERROR;

   for (i = 0; i < ARRAY_SIZE(map); i++) {
      if (err == map[i].from) {
         DSS_Err = map[i].to;
         break;
      }
   }

   return DSS_Err;
}

/*===========================================================================
FUNCTION DSS_IFACE_IOCTL()
===========================================================================*/
int dss_iface_ioctl
( 
  dss_iface_ioctl_id_type  *id_ptr,
  dss_iface_ioctl_type     ioctl_name,
  void                     *argval_ptr,
  sint15                   *dss_errno
)
{
   switch (ioctl_name) {

      case DSS_IFACE_IOCTL_REG_EVENT_CB:
      case DSS_IFACE_IOCTL_DEREG_EVENT_CB:
      {
         dss_iface_ioctl_ev_cb_type* pCb = (dss_iface_ioctl_ev_cb_type*)argval_ptr;
         
         // there is no equivalent for this event
         if (pCb->event == DSS_IFACE_IOCTL_ADDR_CHANGED_EV) {
            return DSS_SUCCESS;
         } else if (pCb->event == DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV) { // implies UP_EV, as well
            if (DSS_IFACE_IOCTL_REG_EVENT_CB == ioctl_name) {
               gNet.cb = *pCb;
               OEMDSS_SetupTAPI();
            } else {
               memset(&gNet.cb, 0, sizeof(gNet.cb));
               OEMDSS_FreeTAPI(0);
            }
            return DSS_SUCCESS;
         } else {
            *dss_errno = DS_EOPNOTSUPP;
            return DSS_ERROR;
         }
      }

      case DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE:
      {
         TAPIStatus ts;
         phys_link_state_type* pState = (phys_link_state_type*)argval_ptr;
 
         if (!argval_ptr) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }
         
         if (((OEMTAPI *)0 != gNet.piTAPI) && 
             (SUCCESS == OEMTAPI_GetStatus(gNet.piTAPI,&ts))) {

            FARF(TAPISTATE,("TAPISTATE: %s",
                            PS_ORIG==ts.state?"orig":
                            PS_INCOMING==ts.state?"incoming":
                            PS_OFFLINE==ts.state?"offline":
                            PS_IDLE==ts.state?"idle":
                            PS_CONVERSATION==ts.state?"conversation":"what the??"));

            /* check phone state */
            switch (ts.state) {

               case PS_OFFLINE:
               case PS_IDLE:
                  *pState = PHYS_LINK_DOWN;
                  break;

               case PS_ORIG:
               case PS_INCOMING:
                  if (!ts.bData) {
                     *pState = PHYS_LINK_DOWN;
                  } else { 
                     *pState = PHYS_LINK_COMING_UP;
                  }
                  break;

               case PS_CONVERSATION:
                  if (!ts.bData) {
                     *pState = PHYS_LINK_DOWN;
                  } else { 
                     *pState = PHYS_LINK_UP;
                  }
                  break;

               default: 
                  *dss_errno = DS_EFAULT;
                  return DSS_ERROR;
            }

            return DSS_SUCCESS;
         } else {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }
      }

      case DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY:
      {
         dss_iface_ioctl_707_sdb_support_query_type* pQuery =
            (dss_iface_ioctl_707_sdb_support_query_type*)argval_ptr;

         pQuery->can_do_sdb = FALSE;
         return DSS_SUCCESS;
      }

      case DSS_IFACE_IOCTL_GET_IPV4_ADDR:
      {
         ip_addr_type* pIpAddr = (ip_addr_type*)argval_ptr;

         IP_ADDR_V4(*pIpAddr) = HTONL(dsiface_get_ip_addr(IFACE_UM_PPP_STACK));

         return DSS_SUCCESS;
      }

      case DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR:
      {
         ip_addr_type* pIpAddr = (ip_addr_type*)argval_ptr;
         INAddr ina = 0;

         ina = dsiface_get_dns_addr(IFACE_UM_PPP_STACK, IFACE_IPv4_DNS_PRIMARY);
         if ((0 == ina) || ((INAddr)-1 == ina)) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }
         
         IP_ADDR_V4(*pIpAddr) = HTONL(ina);
         return DSS_SUCCESS;
      }

      case DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR:
      {
         ip_addr_type* pIpAddr = (ip_addr_type*)argval_ptr;
         INAddr ina = 0;

         ina = dsiface_get_dns_addr(IFACE_UM_PPP_STACK, IFACE_IPv4_DNS_SECONDARY);
         if ((0 == ina) || ((INAddr)-1 == ina)) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         IP_ADDR_V4(*pIpAddr) = HTONL(ina);
         return DSS_SUCCESS;
      }

      case DSS_IFACE_IOCTL_707_GET_MDR:
      {
         dss_iface_ioctl_707_mdr_type* pMdrMode = (dss_iface_ioctl_707_mdr_type*)argval_ptr;
         int mdrMode;
         int nModeLen = sizeof(mdrMode);
         
         if (DSIFACE_SUCCESS != dsiface_get_opt(IFACE_UM_PPP_STACK,
                                                DSIFACE_OPT_MDR,
                                                &mdrMode,
                                                &nModeLen)) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         *pMdrMode = mdrMode;
         return DSS_SUCCESS;
      }

      case DSS_IFACE_IOCTL_707_SET_MDR:
      {
         dss_iface_ioctl_707_mdr_type* pMdrMode = (dss_iface_ioctl_707_mdr_type*)argval_ptr;
         int mdrMode = *pMdrMode;
         int nModeLen = sizeof(mdrMode);
         
         if (DSIFACE_SUCCESS != dsiface_set_opt(IFACE_UM_PPP_STACK,
                                                DSIFACE_OPT_MDR,
                                                &mdrMode,
                                                nModeLen)) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         return DSS_SUCCESS;
      }

      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK:
      {
         dss_iface_ioctl_707_rlp_opt_type* pOptval = (dss_iface_ioctl_707_rlp_opt_type*)argval_ptr;
         dsiface_error_type err;
         int noptlen = sizeof(*pOptval);

         err = dsiface_get_opt(IFACE_UM_PPP_STACK,
                               DSIFACE_OPT_RLP_ALL_CUR_NAK,
                               pOptval,
                               &noptlen);

         return DSIFaceToDSSIFaceIOCtl_Err(err);
      }

      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK:
      {
         dss_iface_ioctl_707_rlp_opt_type* pOptval = (dss_iface_ioctl_707_rlp_opt_type*)argval_ptr;
         dsiface_error_type err;
         int noptlen = sizeof(*pOptval);

         err = dsiface_set_opt(IFACE_UM_PPP_STACK,
                               DSIFACE_OPT_RLP_ALL_CUR_NAK,
                               pOptval,
                               noptlen);

         return DSIFaceToDSSIFaceIOCtl_Err(err);
      }

      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK:
      {
         dss_iface_ioctl_707_rlp_opt_type* pOptval = (dss_iface_ioctl_707_rlp_opt_type*)argval_ptr;
         dsiface_error_type err;
         int noptlen = sizeof(*pOptval);

         err = dsiface_get_opt(IFACE_UM_PPP_STACK,
                               DSIFACE_OPT_RLP_ALL_DEF_NAK,
                               pOptval,
                               &noptlen);

         return DSIFaceToDSSIFaceIOCtl_Err(err);
      }

      case DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK:
      {
         dss_iface_ioctl_707_rlp_opt_type* pOptval = (dss_iface_ioctl_707_rlp_opt_type*)argval_ptr;
         dsiface_error_type err;
         int noptlen = sizeof(*pOptval);

         err = dsiface_set_opt(IFACE_UM_PPP_STACK,
                               DSIFACE_OPT_RLP_ALL_DEF_NAK,
                               pOptval,
                               noptlen);

         return DSIFaceToDSSIFaceIOCtl_Err(err);
      }

      case DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK:
      {
         dss_iface_ioctl_707_rlp_opt_type* pOptval = (dss_iface_ioctl_707_rlp_opt_type*)argval_ptr;
         dsiface_error_type err;
         int noptlen = sizeof(*pOptval);

         err = dsiface_get_opt(IFACE_UM_PPP_STACK,
                               DSIFACE_OPT_RLP_ALL_NEG_NAK,
                               pOptval,
                               &noptlen);

         return DSIFaceToDSSIFaceIOCtl_Err(err);
      }

      case DSS_IFACE_IOCTL_707_SET_DORM_TIMER:
      {
         unsigned int* pTimeout = (unsigned int*)argval_ptr;
         dsiface_error_type err;

         int dwTimeout = *pTimeout;
         int noptlen = sizeof(dwTimeout);

         // check MDR mode. change dormancy timer only if MDR is set to SO33.
         // in other configurations (at least for async data), mobile originated
         // dormancy causes problems so leave dormancy timeout as is (=0 => no dormancy)
         int mdrMode;
         int nModeLen = sizeof(mdrMode);
         
         if (DSIFACE_SUCCESS != dsiface_get_opt(IFACE_UM_PPP_STACK,
                                                DSIFACE_OPT_MDR,
                                                &mdrMode,
                                                &nModeLen)) {
            *dss_errno = DS_EFAULT;
            return DSS_ERROR;
         }

         if (mdrMode != DS_MDR_MODE_SO33_PREF) {
            return DSS_SUCCESS;
         }
         
         err = dsiface_set_opt(IFACE_UM_PPP_STACK,
                               DSIFACE_OPT_DORM_TIMER,
                               &dwTimeout,
                               noptlen);

         return DSIFaceToDSSIFaceIOCtl_Err(err);
      }

      case DSS_IFACE_IOCTL_GET_ALL_IFACES:
      {
         dss_iface_ioctl_all_ifaces_type* pIFaceInfo = (dss_iface_ioctl_all_ifaces_type*)argval_ptr;

         pIFaceInfo->ifaces[0].name = DSS_IFACE_CDMA_SN;
         pIFaceInfo->ifaces[0].instance = 0;
         pIFaceInfo->ifaces[1].name = DSS_IFACE_LO;
         pIFaceInfo->ifaces[1].instance = 0;
         pIFaceInfo->number_of_ifaces = 2;
         return DSS_SUCCESS;
      }

      default:
         *dss_errno = DS_EINVAL;
         return DSS_ERROR;
   }
}

/*===========================================================================
FUNCTION DSS_SHUTDOWN()
===========================================================================*/
sint15 dss_shutdown
(
  sint15           sockfd,                  /* socket descriptor           */
  uint16           how,                     /* what action to perform      */
  sint15*          dss_errno                /* error number                */
)
{
   *dss_errno = DS_EOPNOTSUPP;
   return DSS_ERROR;
}

#endif // !defined(AEE_SIMULATOR) && !defined(T_MSM6100) && !defined(T_MSM6250)

#if ((!defined DSS_VERSION) || \
     (DSS_VERSION<902) || \
     ((DSS_VERSION>=1000)&&(DSS_VERSION<1100)))                                

/*===========================================================================
FUNCTION DSS_LAST_NETDOWNREASON()
===========================================================================*/
sint15 dss_last_netdownreason
(
   sint15                      appid,                /* Application id      */
   dss_net_down_reason_type  * reason,               /* network down reason */
   sint15                    * dss_errno             /* error number        */
)
{
   *reason = DSS_NET_DOWN_REASON_NOT_SPECIFIED;
   return DSS_SUCCESS;
}
#endif // ((!defined DSS_VERSION) || (DSS_VERSION<902) || ((DSS_VERSION>=1000)&&(DSS_VERSION<1100)))


/*===========================================================================
Functions used for QOS Flow options: 
   User Priority,
   Min Service Interval,
   Max Service Interval,
   Inactivity  Interval,
   Nominal SDU Size
   CDMA FLOW Priority
===========================================================================*/
uint16 OEMDSS_TranslateOEMFlowOptToAMSS(oem_ip_flow_type* pOemFlow, ip_flow_type* pAMSSFlow)
{
#ifdef QOS_FLOW_OPT_WLAN_USER_PRIORITY_SUPPORTED      
   // user_priority:
   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_WLAN_USER_PRI) {
      switch (pOemFlow->wlan_params.user_priority)
      {
      case OEM_WLAN_USER_PRI_BEST_EFFORT:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_BEST_EFFORT;
         break;
      case OEM_WLAN_USER_PRI_BACKGROUND:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_BACKGROUND;
         break;
      case OEM_WLAN_USER_PRI_RESERVED:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_RESERVED;
         break;
      case OEM_WLAN_USER_PRI_EXCELLENT_EFFORT:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_EXCELLENT_EFFORT;
         break;
      case OEM_WLAN_USER_PRI_CONTROLLED_LOAD:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_CONTROLLED_LOAD;
         break;
      case OEM_WLAN_USER_PRI_VIDEO:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_VIDEO;
         break;
      case OEM_WLAN_USER_PRI_VOICE:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_VOICE;
         break;
      case OEM_WLAN_USER_PRI_NETWORK_CONTROL:
         pAMSSFlow->wlan_params.user_priority = WLAN_USER_PRI_NETWORK_CONTROL;
         break;          
      default:
         return AEE_NET_EBADOPTVAL;
      }
      
      pAMSSFlow->field_mask |= IPFLOW_MASK_WLAN_USER_PRI;
   }
#endif // QOS_FLOW_OPT_WLAN_USER_PRIORITY_SUPPORTED

#ifdef QOS_FLOW_OPT_WLAN_INTERVALS_SUPPORTED
   // min_service_interval:
   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL) {
      pAMSSFlow->wlan_params.min_service_interval = pOemFlow->wlan_params.min_service_interval;
      pAMSSFlow->field_mask |= IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL;
   }
   // max_service_interval:
   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL) {
      pAMSSFlow->wlan_params.max_service_interval = pOemFlow->wlan_params.max_service_interval;
      pAMSSFlow->field_mask |= IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL;
   }
   // inactivity_interval:
   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL) {
      pAMSSFlow->wlan_params.inactivity_interval = pOemFlow->wlan_params.inactivity_interval;
      pAMSSFlow->field_mask |= IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL;
   }
#endif // QOS_FLOW_OPT_WLAN_INTERVALS_SUPPORTED
   
#ifdef QOS_FLOW_OPT_NOMINAL_SDU_SIZE_SUPPORTED
   // nominal_sdu_size:
   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_NOMINAL_SDU_SIZE) {
      pAMSSFlow->nominal_sdu_size.is_fixed = pOemFlow->nominal_sdu_size.is_fixed;
      pAMSSFlow->nominal_sdu_size.size     = pOemFlow->nominal_sdu_size.size;
      pAMSSFlow->field_mask |= IPFLOW_MASK_NOMINAL_SDU_SIZE;
   }
#endif // QOS_FLOW_OPT_NOMINAL_SDU_SIZE_SUPPORTED

#ifdef QOS_FLOW_OPT_CDMA_FLOW_PRIORITY_SUPPORTED
   // flow_priority:
   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_CDMA_FLOW_PRIORITY) {
      pAMSSFlow->cdma_params.flow_priority = pOemFlow->cdma_params.flow_priority;
      pAMSSFlow->field_mask |= IPFLOW_MASK_CDMA_FLOW_PRIORITY;
   }
#endif // QOS_FLOW_OPT_CDMA_FLOW_PRIORITY_SUPPORTED

   return AEE_NET_SUCCESS;
}

uint16 OEMDSS_TranslateAMSSFlowOptToOEM(ip_flow_type* pAMSSFlow, oem_ip_flow_type* pOemFlow)
{
#ifdef QOS_FLOW_OPT_WLAN_USER_PRIORITY_SUPPORTED
   // user_priority
   if (pAMSSFlow->field_mask & IPFLOW_MASK_WLAN_USER_PRI) {
      switch (pAMSSFlow->wlan_params.user_priority)
      {
      case WLAN_USER_PRI_BEST_EFFORT:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_BEST_EFFORT;
         break;
      case WLAN_USER_PRI_BACKGROUND:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_BACKGROUND;
         break;
      case WLAN_USER_PRI_RESERVED:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_RESERVED;
         break;
      case WLAN_USER_PRI_EXCELLENT_EFFORT:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_EXCELLENT_EFFORT;
         break;
      case WLAN_USER_PRI_CONTROLLED_LOAD:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_CONTROLLED_LOAD;
         break;
      case WLAN_USER_PRI_VIDEO:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_VIDEO;
         break;
      case WLAN_USER_PRI_VOICE:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_VOICE;
         break;
      case WLAN_USER_PRI_NETWORK_CONTROL:
         pOemFlow->wlan_params.user_priority = OEM_WLAN_USER_PRI_NETWORK_CONTROL;
         break;          
      default:
         return AEE_NET_EBADOPTVAL;
      }
      
      pOemFlow->field_mask |= OEM_IPFLOW_MASK_WLAN_USER_PRI;
    }
#endif // QOS_FLOW_OPT_WLAN_USER_PRIORITY_SUPPORTED

#ifdef QOS_FLOW_OPT_WLAN_INTERVALS_SUPPORTED
   // min_service_interval:
   if (pAMSSFlow->field_mask & IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL) {
      pOemFlow->wlan_params.min_service_interval =  pAMSSFlow->wlan_params.min_service_interval;
      pOemFlow->field_mask |= OEM_IPFLOW_MASK_WLAN_MIN_SERVICE_INTERVAL;
   }
   // max_service_interval:
   if (pAMSSFlow->field_mask & IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL) {
      pOemFlow->wlan_params.max_service_interval =  pAMSSFlow->wlan_params.max_service_interval;
      pOemFlow->field_mask |= OEM_IPFLOW_MASK_WLAN_MAX_SERVICE_INTERVAL;
   }
   // inactivity_interval:
   if (pAMSSFlow->field_mask & IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL) {
      pOemFlow->wlan_params.inactivity_interval =  pAMSSFlow->wlan_params.inactivity_interval;
      pOemFlow->field_mask |= OEM_IPFLOW_MASK_WLAN_INACTIVITY_INTERVAL;
   }
#endif // QOS_FLOW_OPT_WLAN_INTERVALS_SUPPORTED
   
#ifdef QOS_FLOW_OPT_NOMINAL_SDU_SIZE_SUPPORTED
   // nominal_sdu_size:
   if (pAMSSFlow->field_mask & IPFLOW_MASK_NOMINAL_SDU_SIZE) {
      pOemFlow->nominal_sdu_size.is_fixed =  pAMSSFlow->nominal_sdu_size.is_fixed;
      pOemFlow->nominal_sdu_size.size     =  pAMSSFlow->nominal_sdu_size.size;
      pOemFlow->field_mask |= OEM_IPFLOW_MASK_NOMINAL_SDU_SIZE;
   }
#endif // QOS_FLOW_OPT_NOMINAL_SDU_SIZE_SUPPORTED

#ifdef QOS_FLOW_OPT_CDMA_FLOW_PRIORITY_SUPPORTED
   // flow_priority:
   if (pAMSSFlow->field_mask & IPFLOW_MASK_CDMA_FLOW_PRIORITY) {
      pOemFlow->cdma_params.flow_priority = pAMSSFlow->cdma_params.flow_priority;
      pOemFlow->field_mask |= OEM_IPFLOW_MASK_CDMA_FLOW_PRIORITY;
   }
#endif // QOS_FLOW_OPT_CDMA_FLOW_PRIORITY_SUPPORTED

   return AEE_NET_SUCCESS;
}


#if ((!defined DSS_VERSION) || (DSS_VERSION<1000))
int oemdss_qos_iface_ioctl
( 
   dss_iface_id_type        iface_id,
   dss_iface_ioctl_type     ioctl_name,
   void                     *argval_ptr,
   sint15                   *dss_errno
)
{
   int ret;

   if (DSS_IFACE_IOCTL_QOS_GET_STATUS == ioctl_name) {
      dss_iface_ioctl_phys_link_state_type phys_link_state;
      dss_iface_ioctl_qos_get_status_type* pqos_status_type = (dss_iface_ioctl_qos_get_status_type*)argval_ptr; 

      ret = oemdss_iface_ioctl(iface_id,
                               DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE,
                               &phys_link_state,
                               dss_errno);
      if (DSS_SUCCESS == ret) {
         switch (phys_link_state) {
            
            case PHYS_LINK_UP:
               pqos_status_type->qos_status = QOS_AVAILABLE;
               break;

            case PHYS_LINK_DOWN:
               pqos_status_type->qos_status = QOS_UNAVAILABLE;
               break;

            case PHYS_LINK_COMING_UP:
               pqos_status_type->qos_status = QOS_ACTIVATING;
               break;

            case PHYS_LINK_GOING_DOWN:
               pqos_status_type->qos_status = QOS_SUSPENDING;
               break;

            default:
               pqos_status_type->qos_status = QOS_UNAVAILABLE;
         }
      } else {
         pqos_status_type->qos_status = QOS_UNAVAILABLE;
      }
      
      return ret;
   }
   
   if (DSS_IFACE_IOCTL_QOS_RESUME == ioctl_name) {
      return oemdss_iface_ioctl(iface_id,
                                DSS_IFACE_IOCTL_GO_ACTIVE,
                                NULL,
                                dss_errno);
   }

   if (DSS_IFACE_IOCTL_QOS_SUSPEND == ioctl_name) {
      *dss_errno = DS_EOPNOTSUPP;
   }

   // should not get here
   return DSS_ERROR;
}
#endif //((!defined DSS_VERSION) || (DSS_VERSION<1000))

void OEMDSS_SetRxAuxFlowMask(qos_spec_field_mask_type* pFieldMask) {
#ifdef QOS_AUX_FLOWS_SUPPORTED
   *pFieldMask |= QOS_MASK_RX_AUXILIARY_FLOWS;
#endif // QOS_AUX_FLOWS_SUPPORTED
}

void OEMDSS_SetTxAuxFlowMask(qos_spec_field_mask_type* pFieldMask) {
#ifdef QOS_AUX_FLOWS_SUPPORTED
   *pFieldMask |= QOS_MASK_TX_AUXILIARY_FLOWS;
#endif // QOS_AUX_FLOWS_SUPPORTED
}

boolean OEMDSS_IsRxAuxFlowMask(qos_spec_field_mask_type field_mask) {
#ifdef QOS_AUX_FLOWS_SUPPORTED
   if (field_mask & QOS_MASK_RX_AUXILIARY_FLOWS) {
      return TRUE;
   }
#endif // QOS_AUX_FLOWS_SUPPORTED
   return FALSE;
}

boolean OEMDSS_IsTxAuxFlowMask(qos_spec_field_mask_type field_mask) {
#ifdef QOS_AUX_FLOWS_SUPPORTED
   if (field_mask & QOS_MASK_TX_AUXILIARY_FLOWS) {
      return TRUE;
   }
#endif // QOS_AUX_FLOWS_SUPPORTED
   return FALSE;
}

void OEMDSS_SetNumAuxFlows(ip_flow_spec_type* pFlowSpec, uint8 uNumAuxFlows) {
#ifdef QOS_AUX_FLOWS_SUPPORTED
   pFlowSpec->num_aux_flows = uNumAuxFlows;
#endif // QOS_AUX_FLOWS_SUPPORTED
}

uint8 OEMDSS_GetNumAuxFlows(ip_flow_spec_type* pFlowSpec) {
   uint8 uRet = 0;
#ifdef QOS_AUX_FLOWS_SUPPORTED
   uRet = pFlowSpec->num_aux_flows;
#endif // QOS_AUX_FLOWS_SUPPORTED
   return uRet;
}

ip_flow_type* OEMDSS_GetAuxFlow(ip_flow_spec_type* pFlowSpec, uint8 uIndex) {   
   ip_flow_type * pFlow = NULL;
#ifdef QOS_AUX_FLOWS_SUPPORTED
   if (pFlowSpec->num_aux_flows > uIndex) {
      pFlow = &(pFlowSpec->aux_flow_list_ptr[uIndex]);
   }
#endif // QOS_AUX_FLOWS_SUPPORTED
   return pFlow;
}

uint16 OEMDSS_MallocAuxFlowList(ip_flow_spec_type* pFlowSpec, uint8 uNumFlows) {   
   uint16 result = AEE_NET_SUCCESS;
#ifdef QOS_AUX_FLOWS_SUPPORTED
   pFlowSpec->num_aux_flows = uNumFlows;
   if (0 < uNumFlows){
      pFlowSpec->aux_flow_list_ptr = sys_malloc(uNumFlows * sizeof(ip_flow_type));
      if (!pFlowSpec->aux_flow_list_ptr) {
         pFlowSpec->aux_flow_list_ptr = 0;
         result = AEE_NET_ENOMEM;         
      }   
   }
#endif // QOS_AUX_FLOWS_SUPPORTED
   return result;
}

void OEMDSS_FreeAuxFlowList(ip_flow_spec_type* pFlowSpec) {   
#ifdef QOS_AUX_FLOWS_SUPPORTED
   if (pFlowSpec->num_aux_flows > 0) {
      sys_free(pFlowSpec->aux_flow_list_ptr);
      pFlowSpec->num_aux_flows = 0;
   }
#endif // QOS_AUX_FLOWS_SUPPORTED
}

ipflow_field_mask_type OEMDSS_GetAuxFlowErrMask(ip_flow_spec_type* pFlowSpec, uint8 uIndex) {   
   ipflow_field_mask_type errorMask = 0;
#ifdef QOS_AUX_FLOWS_SUPPORTED
   if (pFlowSpec->num_aux_flows > uIndex) {
      errorMask = pFlowSpec->aux_flow_list_ptr[uIndex].err_mask;
   }
#endif // QOS_AUX_FLOWS_SUPPORTED
   return errorMask;
}

static uint32 g_uAppType = 0;
void OEMDSS_SetAppType(uint32 uAppType)
{
    g_uAppType = uAppType;
}

#include "OEMSVC.h"
#include "AEERUIM.h"
#include "ds707_data_session_profile.h"
extern char charsvc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
void OEMDSS_SetPPPAccount(int nProfile, uint32 uAppType)
{
    char  user_id_info[PPP_MAX_USER_ID_LEN];
    char  passwd_info[PPP_MAX_PASSWD_LEN];
    nv_item_type nvi;
    boolean bUsedUserID = FALSE;

    OEM_GetConfig(CFGI_BREWSET_USENAME, user_id_info, MAS_BREWSETINT_STRING);
    OEM_GetConfig(CFGI_BREWSET_PASSWORD, passwd_info, MAS_BREWSETINT_STRING);
    
    switch(uAppType){
    case DA_WAP_TYPE:
        if(STRISTR (charsvc_p_name,"mts"))
    	{
            STRCPY(user_id_info,"wap@wap.mtsindia.in");
            STRCPY(passwd_info,"wap");
    	}
    	else if(STRISTR (charsvc_p_name,"tata"))
    	{
            STRCPY(user_id_info,"wapuser");
            STRCPY(passwd_info,"wapuser");
    	}
    	else if(STRISTR (charsvc_p_name,"reliance"))
    	{
            bUsedUserID = TRUE; // RelianceµÄ¿¨²»±ê×¼£¬ÐèÒªÊ¹ÓÃ±¾µØÓÃ»§Ãû
            STRCPY(user_id_info,"SpiceD88@wap.relianceinfo.com");
            STRCPY(passwd_info,"K39MspDeci");
    	}
    	else if(STRISTR (charsvc_p_name,"vmi"))
    	{
            STRCPY(user_id_info,"wap@ttsl.vmi.com");
            STRCPY(passwd_info,"wap");
    	}
        break;
    case DA_BREW_TYPE:
        if(STRISTR (charsvc_p_name,"mts"))
    	{
            STRCPY(user_id_info,"wap@wap.mtsindia.in");
            STRCPY(passwd_info,"wap");
    	}
    	else if(STRISTR (charsvc_p_name,"tata"))
    	{
            STRCPY(user_id_info,"brewuser");
            STRCPY(passwd_info,"brewuser");
    	}
    	else if(STRISTR (charsvc_p_name,"reliance"))
    	{
            bUsedUserID = TRUE;// RelianceµÄ¿¨²»±ê×¼£¬ÐèÒªÊ¹ÓÃ±¾µØÓÃ»§Ãû
            STRCPY(user_id_info,"SpiceD88@wap.relianceinfo.com");
            STRCPY(passwd_info,"K39MspDeci");
    	}
    	else if(STRISTR (charsvc_p_name,"vmi"))
    	{
            STRCPY(user_id_info,"wap@ttsl.vmi.com");
            STRCPY(passwd_info,"wap");
    	}
        break;
    default:
        DBGPRINTF("OEMDSS_SetPPPAccount unsupport type %d",uAppType);
        return;
    }
	
	DBGPRINTF("OEMDSS_SetPPPAccount %s %s %d %d",user_id_info,passwd_info,nProfile,bUsedUserID);

    if(nProfile != DATA_SESSION_PROFILE_ID_INVALID)
    {
        if(bUsedUserID)
        {
        	(void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)user_id_info);
            nvi.pap_user_id.user_id_len = STRLEN((char *)user_id_info);
            if(OEMNV_Put(NV_PPP_USER_ID_I, &nvi) != NV_DONE_S)
            {
                DBGPRINTF(";put NV_PPP_USER_ID_I failed");
            }

            (void)STRCPY((char *)nvi.pap_password.password, (char *)passwd_info);
            nvi.pap_password.password_len = STRLEN((char *)passwd_info);
            if(OEMNV_Put(NV_PPP_PASSWORD_I, &nvi) != NV_DONE_S)
            {
                DBGPRINTF(";put NV_PPP_PASSWORD_I failed");
            }
        }
        else
        {
            // ÉèÖÃ±¾µØÓÃ»§Îª¿Õ£¬µ×²ãÔÚ¶ÁÈ¡µÄÊ±ºòÓÅÏÈÊ¹ÓÃNV_PPP_USER_ID_I,Ã»ÓÐÄÚÈÝ²ÅÊ¹ÓÃNV_DS_SIP_NAI_INFO_I
            nvi.pap_user_id.user_id[0]  = 0;
            nvi.pap_user_id.user_id_len = 0;
            if(OEMNV_Put(NV_PPP_USER_ID_I, &nvi) != NV_DONE_S)
            {
                DBGPRINTF(";put NV_PPP_USER_ID_I failed");
            }
        }
    }
    else
    {
        (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)user_id_info);
        nvi.pap_user_id.user_id_len = STRLEN((char *)user_id_info);
        if(OEMNV_Put(NV_PPP_USER_ID_I, &nvi) != NV_DONE_S)
        {
            DBGPRINTF(";put NV_PPP_USER_ID_I failed");
        }
        
        (void)STRCPY((char *)nvi.pap_password.password, (char *)passwd_info);
        nvi.pap_password.password_len = STRLEN((char *)passwd_info);
        if(OEMNV_Put(NV_PPP_PASSWORD_I, &nvi) != NV_DONE_S)
        {
            DBGPRINTF(";put NV_PPP_PASSWORD_I failed");
        }
    }
}

void OEMDSS_SetCDMAProfileId(int nNetwork, dss_net_policy_info_type* policy_info) 
{
#ifdef CDMA_DATA_SESSION_PROFILE_ID_SUPPORTED
    int cdma_profile_id = (nNetwork>>16)&0xffff;

    //BREW Downloaded apps will get profile 0 here. But this global flag would have been set in BAM.
    if((cdma_profile_id == 0) && (g_uAppType != 0))
    {
        policy_info->cdma.data_session_profile_id = dss_get_app_profile_id(g_uAppType);
        if(policy_info->cdma.data_session_profile_id>=DATA_SESSION_MIN_PROFILE)
        {
            OEMDSS_SetPPPAccount(policy_info->cdma.data_session_profile_id-DATA_SESSION_MIN_PROFILE,g_uAppType);
        }
        else
        {
            OEMDSS_SetPPPAccount(DATA_SESSION_PROFILE_ID_INVALID,g_uAppType);
        }
        
        if(policy_info->cdma.data_session_profile_id < 0)
        {
            policy_info->cdma.data_session_profile_id = 0;
        }
    }
    else
    {
        policy_info->cdma.data_session_profile_id = cdma_profile_id;    
    }
    DBGPRINTF("OEMDSS_SetCDMAProfileId %d %d %d",g_uAppType,cdma_profile_id,policy_info->cdma.data_session_profile_id);
#endif // CDMA_DATA_SESSION_PROFILE_ID_SUPPORTED
}

int oemdss_iface_ioctl(dss_iface_id_type iface_id,
                       dss_iface_ioctl_type ioctl_name,
                       void *argval_ptr,
                       sint15 *dss_errno)
{
#if (!defined DSS_VERSION || DSS_VERSION < 1100) 
   if (DSS_IFACE_IOCTL_GET_IFACE_NAME == ioctl_name) {
      dss_iface_ioctl_iface_name_type* iface_name_type = 
         (dss_iface_ioctl_iface_name_type*)argval_ptr;
      dss_iface_name_enum_type iface_name_enum_type;
      
      iface_name_enum_type = ((iface_id >> 16) & 0xffff);
      switch (iface_name_enum_type) {
      case DSS_IFACE_CDMA_SN:
         *iface_name_type = CDMA_SN_IFACE;
         return DSS_SUCCESS;
      case DSS_IFACE_CDMA_AN:
         *iface_name_type = CDMA_AN_IFACE;
         return DSS_SUCCESS;
#ifdef FEATURE_DATA_BCMCS
      case DSS_IFACE_CDMA_BCAST:
         *iface_name_type = CDMA_BCAST_IFACE;
         return DSS_SUCCESS;
#endif // FEATURE_DATA_BCMCS
      case DSS_IFACE_FLO:
         *iface_name_type = FLO_IFACE;
         return DSS_SUCCESS;
      case DSS_IFACE_UMTS:
         *iface_name_type = UMTS_IFACE;
         return DSS_SUCCESS;
      case DSS_IFACE_SIO:
         *iface_name_type = SIO_IFACE;
         return DSS_SUCCESS;
      case DSS_IFACE_LO:
         *iface_name_type = LO_IFACE;
         return DSS_SUCCESS;
      case DSS_IFACE_WLAN:
         *iface_name_type = WLAN_IFACE;
         return DSS_SUCCESS;
      default:
         *dss_errno = DS_EFAULT;
         return DSS_ERROR;
      }
   }
#endif // (!defined DSS_VERSION || DSS_VERSION < 1100) 
   
#if defined(OEMDSS_NO_IFACE_GET_ID3_DEFINITION)

   if (DSS_IFACE_IOCTL_REG_EVENT_CB == ioctl_name ||
       DSS_IFACE_IOCTL_DEREG_EVENT_CB == ioctl_name) {
      dss_iface_ioctl_ev_cb_type* pIFaceInfo = (dss_iface_ioctl_ev_cb_type*)argval_ptr;

      if (pIFaceInfo && (pIFaceInfo->event >= DSS_IFACE_IOCTL_EVENT_MAX)){
          *dss_errno = DS_EINVAL;
          return DSS_ERROR;
      }
   }

   if (DSS_IFACE_IOCTL_GET_ALL_IFACES == ioctl_name) {
      dss_iface_ioctl_all_ifaces_type iface_info;
      int ret;
      
      ret = dss_iface_ioctl(0,
                            ioctl_name,
                            &iface_info,
                            dss_errno);
      if (DSS_SUCCESS == ret) {
         oemdss_iface_ioctl_all_ifaces_type* pIFaceInfo = (oemdss_iface_ioctl_all_ifaces_type*)argval_ptr;
         int i;
         
         pIFaceInfo->number_of_ifaces = iface_info.number_of_ifaces;
         for (i = 0; i < iface_info.number_of_ifaces; ++i) {
            pIFaceInfo->ifaces[i] = DSS_IFACE_GET_ID3(iface_info.ifaces[i].name, iface_info.ifaces[i].instance, 0);
         }
      }
      
      return ret;
   } else {
      static dss_iface_ioctl_id_type iface_ioctl_id;
      
      iface_ioctl_id.name = (iface_id >> 16) & 0x0000FFFF;
      iface_ioctl_id.instance = (iface_id >> 8) & 0x000000FF;
      
      return dss_iface_ioctl(&iface_ioctl_id,
                             ioctl_name,
                             argval_ptr,
                             dss_errno);
   }
#else  // !defined(OEMDSS_NO_IFACE_GET_ID3_DEFINITION)
   return dss_iface_ioctl(iface_id, ioctl_name, argval_ptr, dss_errno);
#endif // defined(OEMDSS_NO_IFACE_GET_ID3_DEFINITION)
}

#if !defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

extern int16 DSSToAEE_Err(int16 nMSMErr);

/*===========================================================================
   Create fallback inplementation for OEMNet_GetDefaultNetwork() when
   DSS_GET_IFACE_ID_BY_POILICY is not defined
===========================================================================*/

/*===========================================================================
FUNCTION OEMDSS_GetDefaultNetwork()
===========================================================================*/
int16 OEMDSS_GetDefaultNetwork
(
   int *pnNetwork
)
{
   oemdss_iface_ioctl_all_ifaces_type iface_info;
   sint15 ndssErr;
   int ret;
   uint16 i;

   if ((int*)0 == pnNetwork) {
      return AEE_NET_EINVAL;
   }

   ret = oemdss_iface_ioctl(0,
                            DSS_IFACE_IOCTL_GET_ALL_IFACES,
                            &iface_info,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   // Look for an ENABLED interface that we know about
   for (i = 0; i < iface_info.number_of_ifaces; ++i) {
      switch ((iface_info.ifaces[i]>>16)&0xffff) {
         case DSS_IFACE_CDMA_SN:
         {
            *pnNetwork = AEE_NETWORK_CDMA_SN;
            return AEE_NET_SUCCESS;
         }
#if defined(FEATURE_DATA_UMTS)
         case DSS_IFACE_UMTS:
         {
            byte profile_number = 1; // default default
            byte configured_default;
            ds_umts_pdp_profile_status_etype status;

            // See if there is a configured_default configured in the PDP database
            status = ds_umts_get_pdp_profile_num_for_sock_calls(&configured_default);
            if (DS_UMTS_PDP_SUCCESS == status) {
               profile_number = configured_default;
            }
            *pnNetwork = AEE_NETWORK_UMTS(profile_number);
            return AEE_NET_SUCCESS;
         }
#endif // defined(FEATURE_DATA_UMTS)
         default:
            continue;
      }
   }

   return AEE_NET_ENETNONET;
}

/*===========================================================================
FUNCTION OEMDSS_GetIfaceIdBeforePppopen()
===========================================================================*/
int16 OEMDSS_GetIfaceIdBeforePppopen
(
   dss_iface_id_type* iface_id
)
{
   oemdss_iface_ioctl_all_ifaces_type iface_info;
   sint15 ndssErr;
   int ret;
   uint16 i;

   ret = oemdss_iface_ioctl(0,
                            DSS_IFACE_IOCTL_GET_ALL_IFACES,
                            &iface_info,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      *iface_id = DSS_IFACE_INVALID_ID;
      return DSSToAEE_Err(ndssErr);
   }

   // Look for an ENABLED interface that we know about
   for (i = 0; i < iface_info.number_of_ifaces; ++i) {
      switch ((iface_info.ifaces[i]>>16)&0xffff) {
         case DSS_IFACE_CDMA_SN:
         {
            *iface_id = iface_info.ifaces[i];
            return AEE_NET_SUCCESS;
         }
#if defined(FEATURE_DATA_UMTS)
         case DSS_IFACE_UMTS:
         {
            *iface_id = iface_info.ifaces[i];
            return AEE_NET_SUCCESS;
         }
#endif // defined(FEATURE_DATA_UMTS)
         default:
            continue;
      }
   }

   *iface_id = DSS_IFACE_INVALID_ID;
   return AEE_NET_ENETNONET;
}

#endif //!defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

#ifdef OEMDSS_NETLIB2_NOT_SUPPORTED
// The following implementation is for builds which do not support 
// dss_open_netlib2() (e.g. SC1x) and therefor the implementation is 
// done via dss_open_netlib()

// Callback parameter for dss_open_netlib  
static void OEMDSS_SocketEventOccurred(void *pv)
{
   sint15*  psAppID = (sint15*)pv;
   sint15   sockfd = 0;
   sint31   event_mask;
   sint15   dss_errno;
   
   event_mask = dss_getnextevent(*psAppID, &sockfd, &dss_errno);
   if (DSS_ERROR != event_mask) {
      gpfnSocketEventOccurred(*psAppID, sockfd, event_mask, 0);
   }
}

// Callback parameter for dss_open_netlib 
static void OEMDSS_NetEventOccurred(void *pv) 
{
   sint15* psAppID = (sint15*)pv;
   int16    nstat;

   dss_netstatus(*psAppID,&nstat);
   gpfnNetEventOccurred(*psAppID, 0, nstat, 0);
}

sint15 dss_open_netlib2
(
   dss_net_cb_fcn net_cb,                // network callback function 
   void *  net_cb_user_data,             // User data for network call back  
   dss_sock_cb_fcn sock_cb,              // socket callback function 
   void * sock_cb_user_data,             // User data for socket call back  
   dss_net_policy_info_type * policy_info_ptr,     // Network policy info 
   sint15 *dss_errno                               // error condition value 
) 
{
   sint15 sAppID;
      
   // Save original callbacks
   gpfnNetEventOccurred    = net_cb;
   gpfnSocketEventOccurred = sock_cb;

   sAppID = dss_open_netlib(OEMDSS_NetEventOccurred,
                            OEMDSS_SocketEventOccurred,
                            dss_errno);
   
   DBGPRINTF("***zzg dss_open_netlib2 sAppID=%d***", sAppID);
   
   if (DSS_ERROR != sAppID) 
   {
      if (DSS_SUCCESS != dss_set_app_net_policy(sAppID, policy_info_ptr, dss_errno)) 
	  {
         sint15 ndssErr;
         dss_close_netlib(sAppID, &ndssErr);

		 DBGPRINTF("***zzg dss_set_app_net_policy Failed!***");
         return DSS_ERROR;
      }
   }

   return sAppID;
}
#endif // OEMDSS_NETLIB2_NOT_SUPPORTED

