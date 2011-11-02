/*=============================================================================

FILE: OEMSock.c

SERVICES: OEM Functions for Network Services

GENERAL DESCRIPTION:
   A sample implementation of the OEMSock functions declared in
   OEMSock.h that are used in the implementation of AEENet.

        Copyright ?1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
=============================================================================*/
#include <stdlib.h>

/* necessary BREW includes */
#include "OEMSock_QoSSpec.h" // must be included before "OEMSock_priv.h"
#ifdef CUST_EDITION	  
#include "OEMSVC.h"
#endif /*CUST_EDITION*/
#include "OEMSock.h"
#include "OEMSock_priv.h"       // OEM_PROVIDES_*, etc.
#include "OEMNotify.h"
#include "OEMConfig.h"
#include "OEMHeap.h"
#include "OEMOS.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"
#include "AEE_OEMNet.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"
#include "AEEError.h"
#include "AEESockPort.h"   // AEE_IS_ADDR_*
#include "AEEBCMCSDB_FLOWIDLEN.h"   // AEEBCMCSDBRecord_*
#include "AEENetwork.h"   // AEE_NET_STATUS_*
#include "AEEMcastSession2.h"
#include "OEMCriticalSection.h"
#include "AEENetTypes2.h"

#if defined(FEATURE_DDTM_CNTL)
#ifdef SYS_DDTM_MAX_SO_LIST_SIZE
#include "cai.h"
#endif
#endif

// utility addressing macros
#define IPADDR_FROM_INADDR(ipa, ina) \
   (ipa).wFamily = AEE_AF_INET; \
   (ipa).addr.v4 = (ina);

//
// Conditional Debug Output
//

boolean OEM_NETEVENT_IsDebugMsgOn(void);
#define FARF_OEM_NETEVENT     OEM_NETEVENT_IsDebugMsgOn()

boolean OEM_NETSTATE_IsDebugMsgOn(void);
#define FARF_OEM_NETSTATE     OEM_NETSTATE_IsDebugMsgOn()

boolean OEMFAILOVER_3G_TO_2G_IsDebugMsgOn(void);
#define FARF_FAILOVER_3G_TO_2G     OEMFAILOVER_3G_TO_2G_IsDebugMsgOn()

boolean OEMUMTS_ERROR_IsDebugMsgOn(void);
#define FARF_UMTS_ERROR     OEMUMTS_ERROR_IsDebugMsgOn()


#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  1
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#define ASSERT_FARF(x) if (!(x)) { FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); }

#define AEE_SYS_RESUME_IF(x) if (x) { AEE_SYS_RESUME(x); }

#ifdef T_MSM6500
   #define OEMSOCK_MAX_BACKLOG_VALUE 3
#else
   #define OEMSOCK_MAX_BACKLOG_VALUE 1
#endif

// TBD: what is the optimal QoS queue event size ?
#define QOS_EVENT_QUEUE_SIZE 70

// TBD: what is the optimal Primary QoS queue event size ?
#define PRIMARY_QOS_MODIFY_EVENT_QUEUE_SIZE   6

// TBD: what is the optimal multicast queue event size ?
#define MCAST_EVENT_QUEUE_SIZE 160

// TBD: what is the optimal IPv6 Info list size ?
#define IPV6_PRIVACY_ADDR_EVENT_LIST_SIZE 70

// Size used to fetch all available prefixes from lower layers.
// This should correspond to the max possible number of prefixes.
// If this figure is modified in lower layer to be larger than
// the value specified here, this value sould be increased accordingly.
#define IPV6_PREFIXES_LIST_SIZE 10

// The default value for the BCMCS_DB flow_id length
#define BCMCS_DB_FLOW_ID_LEN_DEFAULT 4

// Hack for disabling multiple default iface lookups on UMTS targets.
// The hack is currently disabled and expected to be unnecessary
// with appropriate modifications in AMSS
/*
#if !defined(AEE_SIMULATOR)
#define OEM_DISABLE_MULTIPLE_DEFAULT_IFACE_LOOKUPS
#endif
*/

#define OEMNET_MAX_SIP_SERVER_NUM_ADDRESSES 20
#define OEMNET_MAX_SIP_SERVER_DOMAIN_NAMES  20

/* =======================================================================

   ======================================================================= */

typedef enum {
   NET_FAKE_NONE,
   NET_FAKE_COMING_UP,
   NET_FAKE_GOING_DOWN
} OEMNetTransitionStates;

typedef struct OEMNetPPPState
{
   // We fake the transition states since DMSS does not callback for
   // transition states (e.g. PHYS_LINK_COMING_UP)
   OEMNetTransitionStates nFakePhysLinkState;

#if defined(FAILOVER_3G_TO_2G)
   unsigned bTrying3G:1;  /* set when we call openppp and
                             QCMDR is set to SO33_PREF, cleared
                             when we re-set MDR mode, in netstatus,
                             after a 3g call fails */
   unsigned bRestore3G:1; /* set if we discover the normal mode
                             of operation is 3G */
#endif /* #if defined(FAILOVER_3G_TO_2G) */

} OEMNetPPPState;


#ifdef OEM_PROVIDES_QOS
/* =======================================================================
   OEMNetQoSEventData
   ======================================================================= */

typedef struct OEMNetQoSEventData OEMNetQoSEventData;

struct OEMNetQoSEventData {
   OEMNetQoSEventData  *pNext;

   dss_qos_handle_type                          handle;
   dss_iface_ioctl_event_enum_type              event;
   dss_iface_ioctl_extended_info_code_enum_type infoCode;
};

#endif // OEM_PROVIDES_QOS

#ifdef OEM_PROVIDES_PRIMARY_QOS
/* =======================================================================
   OEMNetPrimaryQoSModifyEventData
   ======================================================================= */

typedef struct OEMNetPrimaryQoSModifyEventData OEMNetPrimaryQoSModifyEventData;

struct OEMNetPrimaryQoSModifyEventData {
   OEMNetPrimaryQoSModifyEventData *pNext;
   void                            *handle; // record handle - passed to the OEM by the AEE
   dss_iface_ioctl_event_enum_type  event;
};

#endif // OEM_PROVIDES_PRIMARY_QOS

#ifdef OEM_PROVIDES_MCAST
/* =======================================================================
   OEMNetMcastEventData
   ======================================================================= */

typedef struct OEMNetMcastEventData OEMNetMcastEventData;

struct OEMNetMcastEventData {
   OEMNetMcastEventData  *pNext;

   dss_iface_ioctl_mcast_handle_type            handle;
   dss_iface_ioctl_event_enum_type              event;
   dss_iface_ioctl_mcast_info_code_enum_type    info_code;
};
#endif // OEM_PROVIDES_MCAST


/* =======================================================================
   OEMNetIPv6PrivAddrInfo
   ======================================================================= */

typedef struct OEMNetIPv6PrivAddrInfo OEMNetIPv6PrivAddrInfo;

struct OEMNetIPv6PrivAddrInfo {
   OEMNetIPv6PrivAddrInfo* pNext;

   void*                   pHandle; // record handle - passed to the OEM by the AEE
   AEEIPv6PrivAddrInfo     aiInfo;    // Address information
};

/* =======================================================================
   OEMNetOutageInfo
   ======================================================================= */

typedef struct OEMNetOutageInfo {
   AEECallback    cbUpdateOutageInfo; // Callback used to update uTimeModified
                                      // and copy the new outage information.
   uint32         uTimeModified;      // The last time this struct was modified.
                                      // The value is received from GETUPTIMEMS().
   AEEOutageInfo  aoiOutageInfo;      // The current outage information.
   AEEOutageInfo  aoiNewOutageInfo;   // Newly received outage information.
} OEMNetOutageInfo;

/* =======================================================================
   OEMNet
   ======================================================================= */

typedef struct OEMNet OEMNet;
typedef struct OEMSocket OEMSocket;

typedef struct
{
   // list of open nets
   OEMNet* pNets;

} OEMNetList;

struct OEMNet
{
   OEMNet*                 pNext;      // linked into OEMNetList

   sint15                  sAppID;     // AMSS application ID
   dss_iface_id_type       app_iface_id;  // iface id used by sAppID

   // list of open sockets
   OEMSocket*              pSockets;

   /* keeps track of funky states in dmss  */
   OEMNetPPPState          PPPState;

   /* Net State */
   NetState                ns;

   AEECallback*            pcbNetWaiter;
   AEECallback*            pcbSocketWaiter;
   AEECallback*            pcbMTPDWaiter;
   AEECallback*            pcbQoSWaiter;
   AEECallback*            pcbMcastWaiter;
   AEECallback*            pcbQoSAwareUnawareWaiter;
   AEECallback*            pcbBearerTechnologyChangeWaiter;
   AEECallback*            pcbIPv6PrivAddrWaiter;
   AEECallback*            pcbPrimaryQoSModifyResultWaiter;
   AEECallback*            pcbPrimaryQoSModifyWaiter;
   AEECallback*            pcbOutageWaiter;
   AEECallback*            pcbQoSProfilesChangedWaiter;
   AEECallback*            pcbIPv6PrefixUpdateWaiter;

   // socket event handler pfn for intercepting DS socket events,
   // typically used by QCT code.
   PFNNOTIFY               pfnSocketEventHandlerInterceptor;

   // Timers
   AEECallback             cbReEnableCallDialogs;
   AEECallback             cbRetryPPPOpen;

   dss_iface_ioctl_mt_handle_type mtpdpHandle;
   flg                     bMTpdpHandleValid;

   flg                     bCDMA_SN:1;
   flg                     bWatchForDormancy:1;
   flg                     bPPPOpenCloseInProgress:1;

#ifdef OEM_PROVIDES_QOS
   /* QoS event queue */
   OEMNetQoSEventData      qeBuffers[QOS_EVENT_QUEUE_SIZE];
   OEMNetQoSEventData     *pqeFreeBuffers;
   OEMNetQoSEventData     *pqeReadyBuffers;
#endif // OEM_PROVIDES_QOS

#ifdef OEM_PROVIDES_PRIMARY_QOS
   /* Primary QoS event queue */
   OEMNetPrimaryQoSModifyEventData      primQEBuffers[PRIMARY_QOS_MODIFY_EVENT_QUEUE_SIZE];
   OEMNetPrimaryQoSModifyEventData     *pPrimQEFreeBuffers;
   OEMNetPrimaryQoSModifyEventData     *pPrimQEReadyBuffers;
#endif // OEM_PROVIDES_PRIMARY_QOS

#ifdef OEM_PROVIDES_MCAST
   /* multicast event queue */
   OEMNetMcastEventData      eBuffers[MCAST_EVENT_QUEUE_SIZE];
   OEMNetMcastEventData     *peFreeBuffers;
   OEMNetMcastEventData     *peReadyBuffers;
#endif // OEM_PROVIDES_MCAST

   /* IPv6 privacy Addr information records list */
   OEMNetIPv6PrivAddrInfo      apiBuffers[IPV6_PRIVACY_ADDR_EVENT_LIST_SIZE];
   OEMNetIPv6PrivAddrInfo     *ppiFreeBuffers;
   OEMNetIPv6PrivAddrInfo     *ppiReadyBuffers;

   /* Outage event information */
   OEMNetOutageInfo        ooiOutageInfo;

#if defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)
   boolean bRegisteredToTAPI;
#endif // defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)
};

/* =======================================================================
   SDB Ack definitions
   ======================================================================= */

// struct holding the OEM's user data
typedef struct OEMSDBAckUserData {
   OEMSocket* pOEMSocket;     // OEMSocket object associated with the SDB ack.
                              // When callback is called, store the data within
                              // this OEMSocket instance.
   AEECallback* pAEECb;       // Callback provided by the AEE to be invoked
                              // when an event is received.
} OEMSDBAckUserData;

/* =======================================================================
   OEMSocket
   ======================================================================= */

struct OEMSocket
{
   OEMSocket        *pNext;     // linked into OEMNet
   OEMNet           *pNet;      // parent network

   int16             sockfd;    // DMSS socket descriptor
   uint32            nEventMask;// Socket event mask

   flg               bStream:1;
   flg               bLoopback:1; // don't fake waking for loopback
   flg               bListen:1; // socket in listen mode

#if defined(DSS_FAKE_CLOSE)
   flg               bFakeCloseEvent:1; // fake close timer fired

   // Timers
   AEECallback       cbFakeClose;
#endif // defined(DSS_FAKE_CLOSE)

   // are we shut down?
   int               nShutdown;

   AEESDBAckInfo asaInfo; // info associated with SDB ack.
   OEMSDBAckUserData OEMUserData;
   dss_so_sdb_ack_cb_type sdb_ack_cb_type;
};

/*===========================================================================

             Globals

===========================================================================*/

static OEMNetList gNetList;

#ifdef OEM_DISABLE_MULTIPLE_DEFAULT_IFACE_LOOKUPS
static boolean bDefaultIfaceFetched = FALSE;
static dss_iface_id_type fetched_iface_id;
#endif // OEM_DISABLE_MULTIPLE_DEFAULT_IFACE_LOOKUPS

/*===========================================================================

                      LOCAL FUNCTION DEFINITIONS

 ===========================================================================*/
static void    OEMNet_NonInterceptedSocketEventOccurred(void *pv);
static void    OEMNet_InterceptedSocketEventOccurred(void *pv);
static boolean OEMNet_IsSocketInterceptorInstalled(OEMNet* pNet);
static void    OEMNet_ReTryPPPOpen(void *po);
static void    OEMNet_MonitorPhysLink(OEMNet* pNet, boolean bRegister);
static void    OEMNet_UpdateOutageTime(void *pv);
static int16   DSSToAEE_BearerTechType(dss_iface_ioctl_bearer_tech_type* psDSSBearerTech,
                                       AEEBearerTechType* psAEEBearerTech);
static int16   DSSToAEE_BearerTechTypeOpts(dss_iface_ioctl_bearer_tech_type *psDSSBearerTech,
                                           IBearerTechnology                *pBearerTechOpts);

static void InsertPrivAddrInfoBufAtHead(OEMNetIPv6PrivAddrInfo **ppPaiListHead, OEMNetIPv6PrivAddrInfo *pPaiBuffer);
static OEMNetIPv6PrivAddrInfo* GetPrivAddrInfoBuff(OEMNetIPv6PrivAddrInfo **ppPaiListHead, const void* pHandle);
static OEMNetIPv6PrivAddrInfo* RemovePrivAddrInfoBuff(OEMNetIPv6PrivAddrInfo **ppPaiListHead, const void* pHandle);

#ifdef FEATURE_DATA_PS_IPV6
static void OEMNet_IPv6PrivAddrEvtCB(dss_iface_ioctl_event_enum_type        event,
                                     dss_iface_ioctl_event_info_union_type  event_info,
                                     void                                   *user_data,
                                     sint15                                 dss_nethandle,
                                     dss_iface_id_type                      iface_id);

static boolean ArePrefixsEqual(const unsigned char* pucAddr1, const unsigned char* pucAddr2, uint32 uLen);
static OEMNetIPv6PrivAddrInfo* RemovePrivAddrInfoBufFromHead(OEMNetIPv6PrivAddrInfo **ppPaiListHead);
static boolean DSv6AddrStateToAEEv6PrefixState(const dss_iface_ioctl_ipv6_addr_state_enum_type eDSV6AddrState,
                                               AEEIPv6PrefixState* pAEEPrefixState);
#endif

#ifdef OEM_PROVIDES_PRIMARY_QOS
static void OEMNet_PrimaryQoSModifyResultEventOccurred(OEMNet* pNet);
#endif // OEM_PROVIDES_PRIMARY_QOS

static int OEMSocket_DSSToAEESDBAckStatus(const dss_sdb_ack_status_enum_type dss_sdb_status,
                                          AEESDBAckStatus* pStatus);
// get the last SDB Ack information associated with this socket.
int16 OEMSocket_GetSDBAckInfo(OEMCONTEXT sockd, AEESDBAckInfo* pInfo);

// The following inline functions enable safely setting/getting the Network State.
// Thread safety is required since it is accessed from PS task and BREW task

// OEMSocket_SetEventMask - returns old Event Mask
static __inline NetState OEMNet_SetState(OEMNet* me, NetState ns)
{
   NetState oldNetStat;

   INTLOCK();
   oldNetStat = me->ns;
   me->ns     = ns;
   INTFREE();
   return oldNetStat;
}

static __inline NetState OEMNet_GetState(OEMNet* me)
{
   NetState ns;

   INTLOCK();
   ns = me->ns;
   INTFREE();
   return ns;
}

// The following inline functions enable safely setting/updating/getting the
// Socket event mask. Thread safety is required since it is accessed from
// PS task and BREW task

// OEMSocket_SetEventMask - returns old Event Mask
static __inline uint32 OEMSocket_SetEventMask(OEMSocket* me, uint32 uEventMask)
{
   uint32 uOldEventMask;
   INTLOCK();
   uOldEventMask  = me->nEventMask;
   me->nEventMask = uEventMask;
   INTFREE();
   return uOldEventMask;
}

// OEMSocket_UpdateEventMask - returns old Event Mask
static __inline uint32 OEMSocket_UpdateEventMask(OEMSocket* me, uint32 uEventMask)
{
   uint32 uOldEventMask;
   INTLOCK();
   uOldEventMask  = me->nEventMask;
   me->nEventMask |= uEventMask;
   INTFREE();
   return uOldEventMask;
}

static __inline uint32 OEMSocket_GetEventMask(OEMSocket* me)
{
   uint32 uEventMask;

   INTLOCK();
   uEventMask = me->nEventMask;
   INTFREE();
   return uEventMask;
}

/*===========================================================================

                      FUNCTION DEFINITIONS

 ===========================================================================*/


/*===========================================================================

                      OEM NET FUNCTION DEFINITIONS

 ===========================================================================*/


/*===========================================================================

FUNCTION OEMNet_New

DESCRIPTION
 Allocate a net related data structure and remember it

===========================================================================*/
static OEMNet* OEMNet_New(void)
{
   OEMNet* me;

   int nIndex;

   me = sys_malloc(sizeof(OEMNet));
   if ((OEMNet*)0 == me) {
      return (OEMNet*)0;
   }

   // insert at front of linked list
   me->pNext = gNetList.pNets;
   gNetList.pNets = me;

   me->sAppID = DSS_ERROR;
   me->app_iface_id = DSS_IFACE_INVALID_ID;
   me->ns = NET_INVALID_STATE;
   me->bMTpdpHandleValid = FALSE;

#ifdef OEM_PROVIDES_QOS
   // Initialize the QoS free buffers.
   {
      OEMNetQoSEventData **ppqe = &me->pqeFreeBuffers;
      int                  index;

      for( index = 0; index < QOS_EVENT_QUEUE_SIZE; ++index ) {
         *ppqe = &me->qeBuffers[index];
         ppqe = &((*ppqe)->pNext);
      }
      // Set end of QoS free buffer list
      *ppqe = NULL;
   }
#endif // OEM_PROVIDES_QOS

#ifdef OEM_PROVIDES_PRIMARY_QOS
   // Initialize the Primary QoS Modify free buffers
   {
      OEMNetPrimaryQoSModifyEventData **pPrimQE = &me->pPrimQEFreeBuffers;
      int index;

      for (index = 0; index < PRIMARY_QOS_MODIFY_EVENT_QUEUE_SIZE; ++index) {
         *pPrimQE = &me->primQEBuffers[index];
         pPrimQE = &((*pPrimQE)->pNext);
      }
      // Set end of Primary QoS free buffer list
      *pPrimQE = NULL;
   }
#endif // OEM_PROVIDES_PRIMARY_QOS

#ifdef OEM_PROVIDES_MCAST
   // Initialize the multicast free buffers.
   {
      OEMNetMcastEventData **ppMcastEventData = &me->peFreeBuffers;
      int                  index;

      // Initialize the callinfo buffers.
      for( index = 0; index < MCAST_EVENT_QUEUE_SIZE; ++index ) {
         *ppMcastEventData = &me->eBuffers[index];
         ppMcastEventData = &((*ppMcastEventData)->pNext);
      }
   }
#endif // OEM_PROVIDES_MCAST

   // Initialize the list of IPv6 Privacy Address free buffers.
   {
      OEMNetIPv6PrivAddrInfo **ppipe = &me->ppiFreeBuffers;

      // Initialize the IPv6PrivAddr Event buffers.
      for (nIndex = 0; nIndex < IPV6_PRIVACY_ADDR_EVENT_LIST_SIZE; ++nIndex) {
         *ppipe = &me->apiBuffers[nIndex];
         ppipe = &((*ppipe)->pNext);
      }

      // Set end of the list of private address info buffer
      *ppipe = NULL;
   }

   // Initialize the OEMNetOutageInfo struct - initially the event info is invalid
   me->ooiOutageInfo.aoiOutageInfo.nStatus = AEE_NET_OUTAGE_INVALID;

   return me;
}

/*===========================================================================

FUNCTION OEMNet_Delete

DESCRIPTION
 Forget and free a net related data structure

===========================================================================*/
static void OEMNet_Delete(OEMNet* me)
{
   OEMNet* ps;
   OEMNet* pPrev;

   // remove from linked list
   pPrev = (OEMNet*)0;
   for (ps = gNetList.pNets; ps; ps = ps->pNext) {

      if (me == ps) {
         if ((OEMNet*)0 == pPrev) {
            gNetList.pNets = ps->pNext;
         } else {
            pPrev->pNext = ps->pNext;
         }
         break;
      }

      pPrev = ps;
   }

   sys_free(ps);
}

/*===========================================================================

FUNCTION DSSToAEE_Err

DESCRIPTION
  Converts a DMSS error code to the corresponding AEE error code.

DEPENDENCIES
  none
===========================================================================*/
int16 DSSToAEE_Err( int16 nMSMErr)
{
/*===========================================================================
===========================================================================*/

   //Store mapping between MSM Error Codes and AEE Error codes.
   static const struct {
      int16          from; // MSM Error Code
      int16          to; // AEE Error Code
   } map[] =
   {
      {DSS_SUCCESS            , AEE_NET_SUCCESS              },
      {DS_EBADF               , AEE_NET_EBADF                },
      {DS_EFAULT              , AEE_NET_EFAULT               },
      {DS_EWOULDBLOCK         , AEE_NET_EWOULDBLOCK          },
      {DS_EAFNOSUPPORT        , AEE_NET_EAFNOSUPPORT         },
      {DS_EPROTOTYPE          , AEE_NET_EPROTOTYPE           },
      {DS_ESOCKNOSUPPORT      , AEE_NET_ESOCKNOSUPPORT       },
      {DS_EPROTONOSUPPORT     , AEE_NET_EPROTONOSUPPORT      },
      {DS_EMFILE              , AEE_NET_EMFILE               },
      {DS_EOPNOTSUPP          , AEE_NET_EOPNOTSUPP           },
      {DS_EADDRINUSE          , AEE_NET_EADDRINUSE           },
      {DS_EADDRREQ            , AEE_NET_EADDRREQ             },
      {DS_EINPROGRESS         , AEE_NET_EINPROGRESS          },
      {DS_EISCONN             , AEE_NET_EISCONN              },
      {DS_EIPADDRCHANGED      , AEE_NET_EIPADDRCHANGED       },
      {DS_ENOTCONN            , AEE_NET_ENOTCONN             },
      {DS_ECONNREFUSED        , AEE_NET_ECONNREFUSED         },
      {DS_ETIMEDOUT           , AEE_NET_ETIMEDOUT            },
      {DS_ECONNRESET          , AEE_NET_ECONNRESET           },
      {DS_ECONNABORTED        , AEE_NET_ECONNABORTED         },
      {DS_EPIPE               , AEE_NET_EPIPE                },
      {DS_ENETDOWN            , AEE_NET_ENETDOWN             },
      {DS_EMAPP               , AEE_NET_EMAPP                },
      {DS_EBADAPP             , AEE_NET_ENETDOWN             },
      {DS_SOCKEXIST           , AEE_NET_SOCKEXIST            },
      {DS_EINVAL              , AEE_NET_EINVAL               },
      {DS_ENOMEM              , AEE_NET_ENOMEM               },
      {DS_EMSGSIZE            , AEE_NET_EMSGSIZE             },
      {DS_EMSGTRUNC           , AEE_NET_EMSGSIZE             },
      {DS_ESHUTDOWN           , AEE_NET_ESHUTDOWN            },
      {DS_ENOPROTOOPT         , AEE_NET_ENOPROTOOPT          },
      {DS_ENETISCONN          , AEE_NET_ENETISCONN           },
      {DS_ENETINPROGRESS      , AEE_NET_ENETINPROGRESS       },
      {DS_ENETNONET           , AEE_NET_ENETNONET            },
      {DS_ENETCLOSEINPROGRESS , AEE_NET_ENETCLOSEINPROGRESS  },
      {DS_ENETEXIST           , AEE_NET_ENETEXIST            },
      {DS_EQOSUNAWARE         , AEE_NET_EQOSUNAWARE          }
   };

   int   i;
   int16 AEE_Err = AEE_NET_GENERAL_FAILURE;

   for (i = 0; i < ARRAY_SIZE(map); i++) {
      if (nMSMErr == map[i].from) {
         AEE_Err = map[i].to;
         break;
      }
   }

   return AEE_Err;
}

OEMNet* OEMNet_FromAppID(sint15 sAppID)
{
   OEMNet* pNet;

   for (pNet = gNetList.pNets; (OEMNet*)0 != pNet; pNet = pNet->pNext) {
      if (sAppID == pNet->sAppID) {
         break;
      }
   }

   return pNet;
}

// Get the OEMNet's iface id - does an iface lookup
// if the iface was not set during OEMNet creation!
static dss_iface_id_type OEMNet_GetIfaceId(OEMNet *pNet)
{
   dss_iface_id_type iface_id = pNet->app_iface_id;

   // no iface was selected - do a lookup now!
   if (DSS_IFACE_INVALID_ID == iface_id) {
      iface_id = oemdss_get_iface_id(pNet->sAppID);

      if (DSS_IFACE_INVALID_ID == iface_id) {
         FARF(ALWAYS, ("oemdss_get_iface_id failed %d", pNet->sAppID));
      }
   }

   return iface_id;
}

// Internal wrapper over oemdss_iface_ioctl
// for handling iface lookup if needed.
static int OEMNet_Iface_Ioctl
(
   OEMNet               *pNet,
   dss_iface_ioctl_type ioctl_name,
   void                 *argval_ptr,
   sint15               *dss_errno
 )
{
   dss_iface_id_type iface_id = OEMNet_GetIfaceId(pNet);

   if (DSS_IFACE_INVALID_ID == iface_id) {
      *dss_errno = DS_ENETNONET;
      return DSS_ERROR;
   }

   return oemdss_iface_ioctl(iface_id, ioctl_name, argval_ptr, dss_errno);
}

void OEMNet_ProcessSocketEvent(OEMNet* pNet)
{
   if (OEMNet_IsSocketInterceptorInstalled(pNet)) {
      OEMNet_InterceptedSocketEventOccurred(pNet);
   } else {
      OEMNet_NonInterceptedSocketEventOccurred(pNet);
   }
}

void OEMNet_ProcessNetEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET netevent"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbNetWaiter);
}

void OEMNet_ProcessMTPDEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET MT-PD event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbMTPDWaiter);
}

void OEMNet_ProcessQoSEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET QoS event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbQoSWaiter);
}

void OEMNet_ProcessMcastEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET multicast event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbMcastWaiter);
}

void OEMNet_ProcessQoSAwareUnawareEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET QoS Aware/Unaware event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbQoSAwareUnawareWaiter);
}

void OEMNet_ProcessBearerTechnologyChangeEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET Bearer Technology Change event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbBearerTechnologyChangeWaiter);
}

void OEMNet_ProcessIPv6PrefixUpdateEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET IPv6 Prefix Update event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbIPv6PrefixUpdateWaiter);
}

void OEMNet_ProcessIPv6PrivAddrEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("NET IPv6 Privacy Address event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbIPv6PrivAddrWaiter);
}

void OEMNet_ProcessPrimaryQoSModifyResultEvent(OEMNet* pNet)
{
   AEE_SYS_RESUME_IF(pNet->pcbPrimaryQoSModifyResultWaiter);
}

void OEMNet_ProcessPrimaryQoSModifyEvent(OEMNet* pNet)
{
   AEE_SYS_RESUME_IF(pNet->pcbPrimaryQoSModifyWaiter);
}

void OEMNet_ProcessOutageEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("Outage event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbOutageWaiter);
}

void OEMNet_ProcessQoSProfilesChangedEvent(OEMNet* pNet)
{
   // FARF(OEM_NETEVENT,("QoS Profiles Changed event"));
   // TODO: FARFing from callbacks is unsafe, find another solution
   AEE_SYS_RESUME_IF(pNet->pcbQoSProfilesChangedWaiter);
}

/* parameter shim for the openlib callbacks */
static void OEMNet_NetEventOccurred(sint15 appId,
                                    dss_iface_id_type iface_id,
                                    sint15 dss_errno,
                                    void* net_cb_user_data)
{
   OEMNet* pNet = OEMNet_FromAppID(appId);

   if ((OEMNet*)0 != pNet) {
      // Update network status
      switch (dss_errno) {
      case DS_ENETNONET:
         OEMNet_SetState(pNet, NET_PPP_CLOSED);
         break;

      case DS_ENETISCONN:
         OEMNet_SetState(pNet, NET_PPP_OPEN);
         break;

      case DS_ENETINPROGRESS:
         OEMNet_SetState(pNet, NET_PPP_OPENING);
         break;

      case DS_ENETCLOSEINPROGRESS:
         OEMNet_SetState(pNet, NET_PPP_CLOSING);
         break;

      default:
         OEMNet_SetState(pNet, NET_INVALID_STATE);
         break;
      }

      OEMNet_ProcessNetEvent(pNet);
   }
}

static void OEMNet_QoSAwareUnawareEventOccurred(OEMNet* pNet)
{
   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessQoSAwareUnawareEvent(pNet);
   }
}

static void OEMNet_BearerTechnologyChangeEventOccurred(OEMNet* pNet)
{
   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessBearerTechnologyChangeEvent(pNet);
   }
}

static void OEMNet_IPv6PrefixUpdateEventOccurred(OEMNet* pNet)
{
   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessIPv6PrefixUpdateEvent(pNet);
   }
}

static void OEMNet_OutageEventOccurred(OEMNet* pNet)
{
   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessOutageEvent(pNet);
   }
}

static void OEMNet_QoSProfilesChangedEventOccurred(OEMNet* pNet)
{
   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessQoSProfilesChangedEvent(pNet);
   }
}

//
// callback function for MT PDP events
//
static void OEMNet_MTPDEventOccurred
(
   dss_iface_ioctl_event_enum_type          event,
   dss_iface_ioctl_event_info_union_type    event_info,
   void                                     *user_data,
   sint15                                   app_id,
   dss_iface_id_type                        iface_id
)
{
   OEMNet* pNet = OEMNet_FromAppID(app_id);

   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessMTPDEvent(pNet);
   }
}

// handle a non intercepted socket event
static void OEMNet_NonInterceptedSocketEventOccurred(void *pv)
{
   OEMNet* pNet = (OEMNet*)pv;
   AEE_SYS_RESUME_IF(pNet->pcbSocketWaiter);
}

// handle an intercepted socket event
static void OEMNet_InterceptedSocketEventOccurred(void *pv)
{
   OEMNet* pNet = (OEMNet*)pv;
   pNet->pfnSocketEventHandlerInterceptor(pv);
}

// check if a socket interceptor has been installed
static boolean OEMNet_IsSocketInterceptorInstalled(OEMNet* pNet)
{
   return (pNet->pfnSocketEventHandlerInterceptor != NULL);
}

/* parameter shim for the openlib callbacks */
static void OEMNet_SocketEventOccurred(sint15 appId,
                                       sint15 sockfd,
                                       uint32 event_mask,
                                       void* sock_cb_user_data)
{
   OEMNet* pNet = OEMNet_FromAppID(appId);
   OEMSocket* ps;

   if ((OEMNet*)0 != pNet) {
      // Get socket instance
      for (ps = pNet->pSockets; (OEMSocket *)0 != ps; ps = ps->pNext) {
         if (sockfd == ps->sockfd) {
            break;
         }
      }
      if ((OEMSocket *)0 != ps) {
         // Update event mask
         OEMSocket_UpdateEventMask(ps, event_mask);
      }

      OEMNet_ProcessSocketEvent(pNet);
   }
}

/*===========================================================================
FUNCTION: OEMNet_CancelNetWaiter

DESCRIPTION
  Cancel the net waiter by removing itself from the net instance
===========================================================================*/
static void OEMNet_CancelNetWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbNetWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelSocketWaiter

DESCRIPTION
  Cancel the socket waiter by removing itself from the net instance
===========================================================================*/
static void OEMNet_CancelSocketWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbSocketWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelMTPDWaiter

DESCRIPTION
  Cancel the MT PD waiter by removing itself from the net instance
===========================================================================*/
static void OEMNet_CancelMTPDWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbMTPDWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelQoSWaiter

DESCRIPTION
  Cancel the QoS waiter by removing itself from the net instance
===========================================================================*/
static void OEMNet_CancelQoSWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbQoSWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelMcastWaiter

DESCRIPTION
  Cancel the multicast waiter by removing itself from the net instance
===========================================================================*/
static void OEMNet_CancelMcastWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbMcastWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelQoSAwareUnawareWaiter

DESCRIPTION
  Cancel the QoS Aware/Unaware waiter by removing itself from the net instance
===========================================================================*/
static void OEMNet_CancelQoSAwareUnawareWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbQoSAwareUnawareWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelBearerTechnologyChangeWaiter

DESCRIPTION
  Cancel the Bearer Technology Change waiter by removing it from the net instance
===========================================================================*/
static void OEMNet_CancelBearerTechnologyChangeWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbBearerTechnologyChangeWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelIPv6PrivAddrWaiter

DESCRIPTION
  Cancel the IPv6 Private Address waiter by removing it from the net instance
===========================================================================*/
static void OEMNet_CancelIPv6PrivAddrWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbIPv6PrivAddrWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelIPv6PrefixUpdateWaiter

DESCRIPTION
   Cancel the IPv6 Prefix Update waiter by removing it from the net instance
===========================================================================*/
static void OEMNet_CancelIPv6PrefixUpdateWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbIPv6PrefixUpdateWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelOutageWaiter

DESCRIPTION
  Cancel the Outage waiter by removing it from the net instance
===========================================================================*/
static void OEMNet_CancelOutageWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbOutageWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelQoSProfilesChangedWaiter

DESCRIPTION
  Cancel the QoS Profiles Changed waiter by removing it from the net instance
===========================================================================*/
static void OEMNet_CancelQoSProfilesChangedWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbQoSProfilesChangedWaiter = NULL;
}

// callback function for interface events
//
// NOTE: the fifth parameter (name) changed types from
// ps_iface_name_enum_type to dss_iface_ioctl_name_enum_type somewhere
// between 6100 releases 3055 and 4040.  Since this parameter is not
// used in the reference implementation, this difference can be simply
// cast away.
//
static void OEMNet_IfaceEventCallback
(
   dss_iface_ioctl_event_enum_type          event,
   dss_iface_ioctl_event_info_union_type    event_info,
   void                                     *user_data,
   sint15                                   app_id,
   dss_iface_id_type                        iface_id
)
{
   OEMNet* pNet = OEMNet_FromAppID(app_id);

   DBGPRINTF("***zzg OEMNet_IfaceEventCallback event=%d***", event);

   if ((OEMNet*)0 != pNet) {
   switch (event) {

      case DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV:
         // when this event is received, we can probably trust the chipset's state
         // FARF(OEM_NETSTATE,("FakePhyLinkState: NONE"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         pNet->PPPState.nFakePhysLinkState = NET_FAKE_NONE;
         // FARF(OEM_NETEVENT,("phys link down event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         break;

      case DSS_IFACE_IOCTL_PHYS_LINK_UP_EV:
         // when this event is received, we can probably trust the chipset's state
         // FARF(OEM_NETSTATE,("FakePhyLinkState: NONE"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         pNet->PPPState.nFakePhysLinkState = NET_FAKE_NONE;
         // FARF(OEM_NETEVENT,("phys link up event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         break;

      case DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV:
         // when this event is received, we can probably trust the chipset's state
         // FARF(OEM_NETSTATE,("FakePhyLinkState: NONE"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         pNet->PPPState.nFakePhysLinkState = NET_FAKE_NONE;
         // FARF(OEM_NETEVENT,("phys link coming up event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         break;

      case DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV:
         // when this event is received, we can probably trust the chipset's state
         // FARF(OEM_NETSTATE,("FakePhyLinkState: NONE"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         pNet->PPPState.nFakePhysLinkState = NET_FAKE_NONE;
         // FARF(OEM_NETEVENT,("phys link going down event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         break;

      case DSS_IFACE_IOCTL_GOING_DOWN_EV:
         // Network connection is in the process of closing.
         // In case the Network triggered the closing request, Network state
         // needs to be updated (unlike cases in which the App triggered the
         // requests and the state was updated in the request call)
         // FARF(OEM_NETEVENT,("Iface going down event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         INTLOCK();
         if (NET_PPP_CLOSED != pNet->ns) {
            pNet->ns = NET_PPP_CLOSING;
         }
         INTFREE();
         break;

      case DSS_IFACE_IOCTL_ADDR_CHANGED_EV:
         // FARF(OEM_NETEVENT,("addr changed event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         break;

      case DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV:
         // FARF(OEM_NETEVENT,("QoS Aware event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         // DS QoS Aware and DS QoS Unaware events are both passed to
         // the higher layers as the same event type.
         OEMNet_QoSAwareUnawareEventOccurred(pNet);
         return;

      case DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV:
         // FARF(OEM_NETEVENT,("QoS Unaware event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         // DS QoS Aware and DS QoS Unaware events are both passed to
         // the higher layers as the same event type.
         OEMNet_QoSAwareUnawareEventOccurred(pNet);
         return;


      case DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV:
         // FARF(OEM_NETEVENT,("Bearer Techonology Change event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         // pass the event to higher layers.
         OEMNet_BearerTechnologyChangeEventOccurred(pNet);
         return;
#ifdef OEM_PROVIDES_PRIMARY_QOS
      case DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV:
      {
         dss_iface_ioctl_primary_qos_modify_result_type* pPrimaryQoSModifyResult =
                  (dss_iface_ioctl_primary_qos_modify_result_type*)(&event_info);
         if(TRUE == pPrimaryQoSModifyResult->is_modify_succeeded) {
            // Pass event to AEE layer only if Primary QoS was actually modified
            OEMNet_PrimaryQoSModifyResultEventOccurred(pNet);
         }
      }
         return;
#endif // OEM_PROVIDES_PRIMARY_QOS

      case DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV:
      {
         dss_iface_ioctl_outage_notification_event_info_type* pOutageInfo =
            (dss_iface_ioctl_outage_notification_event_info_type*)(&event_info);

         if (!CALLBACK_IsQueued(&pNet->ooiOutageInfo.cbUpdateOutageInfo)) {
            AEE_SYS_RESUME(&pNet->ooiOutageInfo.cbUpdateOutageInfo);
         }

         INTLOCK();
         pNet->ooiOutageInfo.aoiNewOutageInfo.nStatus = AEE_NET_OUTAGE_VALID;
         // this casting is safe, since the time_to_outage is expected to be
         // at most a few hundred milliseconds
         // ASSERT_FARF(pOutageInfo->time_to_outage <= MAX_INT32);
         // TODO: FARFing from callbacks is unsafe, find another solution
         pNet->ooiOutageInfo.aoiNewOutageInfo.nTimeToOutage = (int) pOutageInfo->time_to_outage;
         pNet->ooiOutageInfo.aoiNewOutageInfo.uDuration = pOutageInfo->duration;
         INTFREE();

         // pass the event to higher layers.
         OEMNet_OutageEventOccurred(pNet);
         return;
      }

      case DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV:
         OEMNet_QoSProfilesChangedEventOccurred(pNet);
         return;

      case DSS_IFACE_IOCTL_PREFIX_UPDATE_EV:
         // FARF(OEM_NETEVENT,("IPv6 Prefix Update event"));
         // TODO: FARFing from callbacks is unsafe, find another solution
         // pass the event to higher layers.
         OEMNet_IPv6PrefixUpdateEventOccurred(pNet);
         break;

      default:
         return;
   }

      AEE_SYS_RESUME_IF(pNet->pcbNetWaiter);
   }
}

// (de)register event callback for an interface event
static int OEMNet_RegisterIfaceEventCallback(OEMNet* pNet, dss_iface_ioctl_event_enum_type event, boolean bRegister)
{
   dss_iface_ioctl_ev_cb_type ev_cb;
   dss_iface_id_type          iface_id;
   int ret;
   int16 ndssErr;

   // the following cast is needed as one of the enum types changed
   // names in middle of the 6100 series of releases...see note
   // above OEMNet_IfaceEventCallback() for more details.
   ev_cb.event_cb = (dss_iface_ioctl_event_cb)OEMNet_IfaceEventCallback;
   ev_cb.event = event;
   ev_cb.user_data_ptr = NULL;
   ev_cb.app_id = pNet->sAppID;

   iface_id = OEMNet_GetIfaceId(pNet);
   
   if (DSS_IFACE_INVALID_ID == iface_id) 
   {
      return AEE_NET_ENETNONET;
   }

	DBGPRINTF("***zzg OEMNet_RegisterIfaceEventCallback iface_id=%d***", iface_id);
	
   ret = oemdss_iface_ioctl(iface_id,
                            bRegister ? DSS_IFACE_IOCTL_REG_EVENT_CB : DSS_IFACE_IOCTL_DEREG_EVENT_CB,
                            &ev_cb,
                            &ndssErr);

   DBGPRINTF("***zzg OEMNet_RegisterIfaceEventCallback event=%d, ret=%d***", event, ret);
   
   if (DSS_SUCCESS != ret) 
   {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;
}

static void OEMNet_ReEnableCallDialogs(void *unused)
{
   (void)unused;
   OEM_Notify(OEMNTF_SHOW_CALL_DIALOGS, TRUE); // OK to show call dialogs again
}

/*===========================================================================

FUNCTION OEMNet_Open

DESCRIPTION

  Opens up the network.

  This function is called from the context of the socket client's task.

===========================================================================*/
OEMCONTEXT OEMNet_Open
(
   int          nNetwork,
   int16        nFamily,
   AEECallback *pcbNetWaiter,
   AEECallback *pcbSocketWaiter,
   AEECallback *pcbQoSWaiter,
   AEECallback *pcbMcastWaiter,
   AEECallback *pcbQoSAwareUnawareWaiter,
   AEECallback *pcbBearerTechnologyChangeWaiter,
   AEECallback *pcbIPv6PrivAddrWaiter,
   AEECallback *pcbPrimaryQoSModifyResultWaiter,
   AEECallback *pcbPrimaryQoSModifyWaiter,
   AEECallback *pcbOutageWaiter,
   AEECallback *pcbQoSProfilesChangedWaiter,
   AEECallback *pcbIPv6PrefixUpdateWaiter,
   int16       *err
)
{
   int16 ndssErr;
   OEMNet* pNet;

   DBGPRINTF("***zzg OEMNet_Open Start***");
   
#if defined(FEATURE_DATA_UMTS) || defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)
   boolean bSelectIface = TRUE;
#endif

   switch (nNetwork&0xffff) {
      case AEE_NETWORK_CDMA_SN:
      case AEE_NETWORK_WLAN:
      case AEE_NETWORK_SLIP:
      case AEE_NETWORK_MFLO_MCAST:
      case AEE_NETWORK_LOOPBACK:
      case AEE_NETWORK_IWLAN_3GPP(0):
      case AEE_NETWORK_IWLAN_3GPP2(0):
      case AEE_NETWORK_3GPP_ANY(0):
      case AEE_NETWORK_3GPP2_ANY(0):
      case AEE_NETWORK_UW_FMC:
          break;

#ifdef FEATURE_DTV_DVBH
      case AEE_NETWORK_DVBH_MCAST:
          break;
#endif //FEATURE_DTV_DVBH

#ifdef FEATURE_DATA_BCMCS
      case AEE_NETWORK_CDMA_BCMCS:
         break;
#endif // FEATURE_DATA_BCMCS

#if defined(FEATURE_DATA_UMTS)
      case AEE_NETWORK_UMTS(0):
      {
         boolean bValid;
         ds_umts_pdp_profile_status_etype status;

         status = ds_umts_get_pdp_profile_context_info_is_valid((byte)(nNetwork >> 16), &bValid);

         if ((DS_UMTS_PDP_SUCCESS != status) || !bValid) {
            *err = AEE_NET_ENETNONET;
            return (OEMCONTEXT)AEE_NET_ERROR;
         }

         break;
      }
#endif // defined(FEATURE_DATA_UMTS)

      default:
         *err = AEE_NET_ENETNONET;
         return (OEMCONTEXT)AEE_NET_ERROR;
   }

   pNet = OEMNet_New();
   if ((OEMNet*)0 == pNet) {
      *err = AEE_NET_EMFILE;
      return (OEMCONTEXT)AEE_NET_ERROR;
   }

   OEMNet_SetNetWaiter(pNet, pcbNetWaiter);
   OEMNet_SetSocketWaiter(pNet, pcbSocketWaiter);
   OEMNet_SetQoSWaiter(pNet, pcbQoSWaiter);
   OEMNet_SetMcastWaiter(pNet, pcbMcastWaiter);
   OEMNet_SetQoSAwareUnawareWaiter(pNet, pcbQoSAwareUnawareWaiter);
   OEMNet_SetMTPDWaiter(pNet, NULL);
   OEMNet_SetBearerTechnologyChangeWaiter(pNet, pcbBearerTechnologyChangeWaiter);
   OEMNet_SetIPv6PrivAddrWaiter(pNet, pcbIPv6PrivAddrWaiter);
   OEMNet_SetPrimaryQoSModifyResultWaiter(pNet, pcbPrimaryQoSModifyResultWaiter);
   OEMNet_SetPrimaryQoSModifyWaiter(pNet, pcbPrimaryQoSModifyWaiter);
   OEMNet_SetOutageWaiter(pNet, pcbOutageWaiter);
   OEMNet_SetQoSProfilesChangedWaiter(pNet, pcbQoSProfilesChangedWaiter);
   OEMNet_SetIPv6PrefixUpdateWaiter(pNet, pcbIPv6PrefixUpdateWaiter);

#if defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)
   pNet->bRegisteredToTAPI = FALSE;
#endif // defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)

	DBGPRINTF("***zzg OEMNet_Open 1 pNet->sAppID=%d***", pNet->sAppID);

   // Does the network library need to be opened?
   if (DSS_ERROR == pNet->sAppID) 
   {
      int ret;
      dss_net_policy_info_type policy_info;

      // Initialize to defaults
      dss_init_net_policy_info(&policy_info);

	  DBGPRINTF("***zzg OEMNet_Open nFamily=%d***", nFamily);

      switch (nFamily)
      {
         case AEE_AF_INET:
            policy_info.family = AF_INET;
            break;
         case AEE_AF_INET6:
            policy_info.family = AF_INET6;
            break;
         case AEE_AF_UNSPEC:
            policy_info.family = AF_UNSPEC;
            break;
         default:
            OEMNet_Close(pNet);
            *err = AEE_NET_EAFNOSUPPORT;
            return (OEMCONTEXT)AEE_NET_ERROR;
            break;
      }

      switch (nNetwork&0xffff) {
         case AEE_NETWORK_MFLO_MCAST:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_FLO;
            break;
         }
         case AEE_NETWORK_DVBH_MCAST:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_DVBH;
            break;
         }
         case AEE_NETWORK_WLAN:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_WLAN;
            break;
         }
         case AEE_NETWORK_SLIP:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_SLIP;
            break;
         }
         case AEE_NETWORK_UW_FMC:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_UW_FMC;
            break;
         }
         case AEE_NETWORK_CDMA(0):
         {
		 	DBGPRINTF("***zzg OEMNet_Open AEE_NETWORK_CDMA***");
			
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_CDMA_SN;

            // Allow certain ops for only for CDMA Service Network
            pNet->bCDMA_SN = TRUE;
            OEMDSS_SetCDMAProfileId(nNetwork,&policy_info);
            break;
         }
         case AEE_NETWORK_LOOPBACK:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_LO;
            break;
         }
         case AEE_NETWORK_IWLAN_3GPP2(0):
         {
		 	DBGPRINTF("***zzg OEMNet_Open AEE_NETWORK_IWLAN_3GPP2***");
			
            policy_info.iface.info.name = (dss_iface_name_enum_type)DSS_IFACE_IWLAN_3GPP2;
            // Allow certain ops for only for CDMA Service Network
            OEMDSS_SetCDMAProfileId(nNetwork,&policy_info);
            break;
         }

         case AEE_NETWORK_3GPP2_ANY(0):
         {
		 	DBGPRINTF("***zzg OEMNet_Open AEE_NETWORK_3GPP2_ANY***");
			
            policy_info.iface.info.name = (dss_iface_name_enum_type)DSS_IFACE_3GPP2_ANY;
            // Allow certain ops for only for CDMA Service Network
            OEMDSS_SetCDMAProfileId(nNetwork,&policy_info);
            break;
         }
#ifdef FEATURE_DATA_BCMCS
         case AEE_NETWORK_CDMA_BCMCS:
         {
            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_CDMA_BCAST;
            break;
         }
#endif // FEATURE_DATA_BCMCS
#if defined(FEATURE_DATA_UMTS)
         case AEE_NETWORK_UMTS(0):
         {
            bSelectIface = FALSE;

            policy_info.iface.kind = DSS_IFACE_NAME;
            policy_info.iface.info.name = DSS_IFACE_UMTS;
            policy_info.umts.pdp_profile_num = (nNetwork>>16)&0xffff;
            FARF(ALWAYS, ("OEMNet_Open: pdp_profile_num=%d",policy_info.umts.pdp_profile_num));
            break;
         }
         case AEE_NETWORK_IWLAN_3GPP(0):
         {
            policy_info.iface.info.name = (dss_iface_name_enum_type)DSS_IFACE_IWLAN_3GPP;
            policy_info.umts.pdp_profile_num = (nNetwork>>16)&0xffff;
            FARF(ALWAYS, ("OEMNet_Open: pdp_profile_num=%d",policy_info.umts.pdp_profile_num));
            break;
         }

         case AEE_NETWORK_3GPP_ANY(0):
         {
            policy_info.iface.info.name = (dss_iface_name_enum_type)DSS_IFACE_3GPP_ANY;
            policy_info.umts.pdp_profile_num = (nNetwork>>16)&0xffff;
            FARF(ALWAYS, ("OEMNet_Open: pdp_profile_num=%d",policy_info.umts.pdp_profile_num));
            break;
         }
#endif // defined(FEATURE_DATA_UMTS)
         default:
            // Should not get here. Already checked above.
            break;
      }

      pNet->sAppID = dss_open_netlib2((dss_net_cb_fcn)OEMNet_NetEventOccurred,
                                      NULL,
                                      OEMNet_SocketEventOccurred,
                                      NULL,
                                      &policy_info,
                                      &ndssErr);

	  DBGPRINTF("***zzg OEMNet_Open 2 pNet->sAppID=%d***", pNet->sAppID);
	  
      if (DSS_ERROR == pNet->sAppID) 
	  {
         OEMNet_Close(pNet);
         *err = DSSToAEE_Err(ndssErr);
         FARF(ALWAYS, ("OEMNet_Open: dss_open_netlib2 failed"));
         return (OEMCONTEXT)AEE_NET_ERROR;
      }

      OEMNet_SetState(pNet, NET_PPP_CLOSED);

#if defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

      if (bSelectIface) {
         // store the iface_id that matches the policy that was just set above.
         // later, all references to iface_id should use the stored value. this
         // guaranties that the same iface_id is used for this app.
         pNet->app_iface_id = oemdss_get_iface_id(pNet->sAppID);

		 DBGPRINTF("***zzg OEMNet_Open 1 pNet->app_iface_id=%d***", pNet->app_iface_id);
		 
         if (DSS_IFACE_INVALID_ID == pNet->app_iface_id) 
		 {
            OEMNet_Close(pNet);
            *err = AEE_NET_ENETNONET;
            FARF(ALWAYS, ("OEMNet_Open: oemdss_get_iface_id failed %d", pNet->sAppID));
            return (OEMCONTEXT)AEE_NET_ERROR;
         }

         // set the policy with this iface_id so that when dss_pppopen will be
         // called, the same iface_id will be chosen.
         policy_info.iface.kind = DSS_IFACE_ID;
         policy_info.iface.info.id = pNet->app_iface_id;
		 
         if (DSS_SUCCESS != dss_set_app_net_policy(pNet->sAppID, &policy_info, &ndssErr)) 
		 {
            OEMNet_Close(pNet);
            *err = DSSToAEE_Err(ndssErr);
            FARF(ALWAYS, ("OEMNet_Open: dss_set_app_net_policy failed %d", pNet->sAppID));
            return (OEMCONTEXT)AEE_NET_ERROR;
         }
      }

#else // #if !defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

      // fallback to the old way of finding the iface
      *err = OEMDSS_GetIfaceIdBeforePppopen(&pNet->app_iface_id);

	    DBGPRINTF("***zzg OEMNet_Open 2 pNet->app_iface_id=%d***", pNet->app_iface_id);
		
      if (DSS_IFACE_INVALID_ID == pNet->app_iface_id) {
         OEMNet_Close(pNet);
         FARF(ALWAYS, ("OEMNet_Open: failed to fetch iface id"));
         return (OEMCONTEXT)AEE_NET_ERROR;
      }

#endif // #if defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

      // register an event callback for ip address changes
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_ADDR_CHANGED_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_ADDR_CHANGED_EV) failed! %d", ret));
      }

      // register an event callback for Iface going down events. These events
      // are required for tracking cases in which network is in the state of
      // being closed, especially when the close request was triggered by the
      // network and therefore this is the only way to update the network state
      // field to "Closing".
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_GOING_DOWN_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_GOING_DOWN_EV) failed! %d", ret));
      }

      // register an event callback for QoS Aware events.
      // The registration should succeed for both - CDMA and UMTS networks.
      // Though for the second case, no events are expected to be generated.
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV) failed! %d", ret));
      }

      // register an event callback for QoS Unaware events
      // The registration should succeed for both - CDMA and UMTS networks.
      // Though for the second case, no events are expected to be generated.
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV) failed! %d", ret));
      }
#ifdef OEM_PROVIDES_PRIMARY_QOS
      // register an event callback for Primary QoS modify result events
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV) failed! %d", ret));
      }
#endif //OEM_PROVIDES_PRIMARY_QOS
      // register an event callback for Bearer Technology Change events
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV) failed! %d", ret));
      }

      // register an event callback for Outage events
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV) failed! %d", ret));
      }

      // register an event callback for QoS Profiles Changed events
      ret = OEMNet_RegisterIfaceEventCallback(pNet,
                                              DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV,
                                              TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS,
              ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV) failed! %d",
               ret));
      }

      // register an event callback for IPv6 Prefix Update events
      ret = OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PREFIX_UPDATE_EV, TRUE);
      if (DSS_SUCCESS != ret) {
         FARF(ALWAYS, ("OEMNet_RegisterIfaceEventCallback(DSS_IFACE_IOCTL_PREFIX_UPDATE_EV) failed! %d", ret));
      }

      CALLBACK_Init(&pNet->cbReEnableCallDialogs, OEMNet_ReEnableCallDialogs, 0);
      CALLBACK_Init(&pNet->cbRetryPPPOpen, OEMNet_ReTryPPPOpen, pNet);

      CALLBACK_Init(&pNet->ooiOutageInfo.cbUpdateOutageInfo, OEMNet_UpdateOutageTime, pNet);
   }

   return (OEMCONTEXT)pNet;
}

/*===========================================================================

FUNCTION OEMNet_Close

DESCRIPTION

  Closes the specified network library.  All sockets must have
  been closed for the network, prior to closing.

===========================================================================*/
int16 OEMNet_Close
(
   OEMCONTEXT netd
)
{
   OEMNet* pNet = (OEMNet*)netd;

   DBGPRINTF("***zzg OEMNet_Close pNet->sAppID=%d***", pNet->sAppID);

   if ((OEMNet*)0 != pNet) {
      int16 ndssErr;

      // remove the event handlers
      OEMNet_SetNetWaiter(pNet, NULL);
      OEMNet_SetSocketWaiter(pNet, NULL);
      OEMNet_DeRegisterFromMTPD(pNet);
      OEMNet_SetQoSWaiter(pNet, NULL);
      OEMNet_SetMcastWaiter(pNet, NULL);
      OEMNet_SetQoSAwareUnawareWaiter(pNet, NULL);
      OEMNet_SetBearerTechnologyChangeWaiter(pNet, NULL);
      OEMNet_SetIPv6PrivAddrWaiter(pNet, NULL);
      OEMNet_SetPrimaryQoSModifyResultWaiter(pNet, NULL);
      OEMNet_SetPrimaryQoSModifyWaiter(pNet, NULL);
      OEMNet_SetOutageWaiter(pNet, NULL);
      OEMNet_SetQoSProfilesChangedWaiter(pNet, NULL);

      // validate that the list or private addresses info records is empty
      if (pNet->ppiReadyBuffers != NULL) {
         ASSERT_FARF("OEMNet_Close: Private IPv6 Addresses info list is not empty");
      }
#ifdef OEM_PROVIDES_PRIMARY_QOS
      // Clean Primary QoS Buffer - this is done for debugging reasons only
      memset(pNet->primQEBuffers, 0, PRIMARY_QOS_MODIFY_EVENT_QUEUE_SIZE*sizeof(OEMNetPrimaryQoSModifyEventData));
#endif // OEM_PROVIDES_PRIMARY_QOS

      if (DSS_ERROR != pNet->sAppID) {

         CALLBACK_Cancel(&pNet->cbReEnableCallDialogs);
         CALLBACK_Cancel(&pNet->cbRetryPPPOpen);
         CALLBACK_Cancel(&pNet->ooiOutageInfo.cbUpdateOutageInfo);

         // deregister for ip address changes
         (void)OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_ADDR_CHANGED_EV, FALSE);
         // deregister for Iface down events
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_GOING_DOWN_EV, FALSE);
         // deregister for QoS Aware events
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_QOS_AWARE_SYSTEM_EV, FALSE);
         // deregister for QoS Unaware events
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_QOS_UNAWARE_SYSTEM_EV, FALSE);
         // deregister for Bearer Technology events
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_BEARER_TECH_CHANGED_EV, FALSE);
#ifdef OEM_PROVIDES_PRIMARY_QOS
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY_RESULT_EV, FALSE);
#endif // OEM_PROVIDES_PRIMARY_QOS
         // deregister for Outage events
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_OUTAGE_NOTIFICATION_EV, FALSE);
         // deregister for QoS Profiles Changed events
         OEMNet_RegisterIfaceEventCallback(pNet,
                                           DSS_IFACE_IOCTL_707_NETWORK_SUPPORTED_QOS_PROFILES_CHANGED_EV,
                                           FALSE);

         // deregister for IPv6 Prefix Update events
         OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PREFIX_UPDATE_EV, FALSE);

         pNet->pfnSocketEventHandlerInterceptor = NULL;

         if (DSS_SUCCESS != dss_close_netlib2(pNet->sAppID, &ndssErr)) {
            FARF(OEM_NETSTATE, ("dss_close_netlib2 failed. dssErr=%d", ndssErr));
         }

         // don't want these anymore
         FARF(OEM_NETSTATE,("Fake*State: NONE"));
         pNet->PPPState.nFakePhysLinkState = NET_FAKE_NONE;
         pNet->sAppID = DSS_ERROR;
      }

      OEMNet_Delete(pNet);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SetSocketWaiter

DESCRIPTION

  Registers a new socket waiter callback

===========================================================================*/
void OEMNet_SetSocketWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet* pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelSocketWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbSocketWaiter) {
      CALLBACK_Cancel(pNet->pcbSocketWaiter);
   }

   pNet->pcbSocketWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetNetWaiter

DESCRIPTION

  Registers a new network waiter callback

===========================================================================*/
void OEMNet_SetNetWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet* pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelNetWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbNetWaiter) {
      CALLBACK_Cancel(pNet->pcbNetWaiter);
   }

   pNet->pcbNetWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetMTPDWaiter

DESCRIPTION

  Registers a new MT packet data waiter callback

===========================================================================*/
void OEMNet_SetMTPDWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelMTPDWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbMTPDWaiter) {
      CALLBACK_Cancel(pNet->pcbMTPDWaiter);
   }

   pNet->pcbMTPDWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetQoSWaiter

DESCRIPTION

  Registers a new QoS event waiter callback

===========================================================================*/
void OEMNet_SetQoSWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelQoSWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbQoSWaiter) {
      CALLBACK_Cancel(pNet->pcbQoSWaiter);
   }

   pNet->pcbQoSWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetMcastWaiter

DESCRIPTION

  Registers a new multicast event waiter callback

===========================================================================*/
void OEMNet_SetMcastWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelMcastWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbMcastWaiter) {
      CALLBACK_Cancel(pNet->pcbMcastWaiter);
   }

   pNet->pcbMcastWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetQoSAwareUnawareWaiter

DESCRIPTION

  Registers a new QoS Aware/Unaware event waiter callback

===========================================================================*/
void OEMNet_SetQoSAwareUnawareWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelQoSAwareUnawareWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbQoSAwareUnawareWaiter) {
      CALLBACK_Cancel(pNet->pcbQoSAwareUnawareWaiter);
   }

   pNet->pcbQoSAwareUnawareWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetBearerTechnologyChangeWaiter

DESCRIPTION

  Registers a new Bearer Technology event waiter callback

===========================================================================*/
void OEMNet_SetBearerTechnologyChangeWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelBearerTechnologyChangeWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbBearerTechnologyChangeWaiter) {
      CALLBACK_Cancel(pNet->pcbBearerTechnologyChangeWaiter);
   }

   pNet->pcbBearerTechnologyChangeWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetIPv6PrivAddrWaiter

DESCRIPTION

  Registers a new IPv6 Privacy Address event waiter callback

===========================================================================*/
void OEMNet_SetIPv6PrivAddrWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelIPv6PrivAddrWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbIPv6PrivAddrWaiter) {
      CALLBACK_Cancel(pNet->pcbIPv6PrivAddrWaiter);
   }

   pNet->pcbIPv6PrivAddrWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetIPv6PrefixUpdateWaiter

DESCRIPTION

Registers a new IPv6 Prefix Update event waiter callback

===========================================================================*/
void OEMNet_SetIPv6PrefixUpdateWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelIPv6PrefixUpdateWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbIPv6PrefixUpdateWaiter) {
      CALLBACK_Cancel(pNet->pcbIPv6PrefixUpdateWaiter);
   }

   pNet->pcbIPv6PrefixUpdateWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetOutageWaiter

DESCRIPTION

  Registers a new Outage event waiter callback

===========================================================================*/
void OEMNet_SetOutageWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelOutageWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbOutageWaiter) {
      CALLBACK_Cancel(pNet->pcbOutageWaiter);
   }

   pNet->pcbOutageWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetQoSProfilesChangedWaiter

DESCRIPTION

  Registers a new QoS Profiles Changed event waiter callback

===========================================================================*/
void OEMNet_SetQoSProfilesChangedWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelQoSProfilesChangedWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbQoSProfilesChangedWaiter) {
      CALLBACK_Cancel(pNet->pcbQoSProfilesChangedWaiter);
   }

   pNet->pcbQoSProfilesChangedWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_RegisterToMTPD

DESCRIPTION

   Sets the MT packet data waiter and then registers to MT packet data events.

===========================================================================*/
int16 OEMNet_RegisterToMTPD(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;
   sint15  ndssErr;
   dss_iface_ioctl_mt_reg_cb_type mtpdp_reg_cb;
   int ret;

   OEMNet_SetMTPDWaiter(pNet, pcbWaiter);

   // if oemnet is already registered for MT PDP, then do nothing ...
   if (pNet->bMTpdpHandleValid) {
      return AEE_NET_SUCCESS;
   }

   // The following cast is needed as the dss_iface_ioctl_event_cb type
   // changed somewhere in AMSS releases.
   // In older AMSS releases, MT PDP is not supported and this event
   // callback will never be used.
   mtpdp_reg_cb.event_cb      = (dss_iface_ioctl_event_cb)OEMNet_MTPDEventOccurred;
   mtpdp_reg_cb.app_id        = pNet->sAppID;
   mtpdp_reg_cb.handle        = &pNet->mtpdpHandle;
   mtpdp_reg_cb.user_data_ptr = pNet;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MT_REG_CB,
                            &mtpdp_reg_cb,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   pNet->bMTpdpHandleValid = TRUE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_DeRegisterFromMTPD

DESCRIPTION

   Clears the MT packet data waiter and then de-registers from MT packet data
   events

===========================================================================*/
int16 OEMNet_DeRegisterFromMTPD(OEMCONTEXT netd)
{
   OEMNet *pNet = (OEMNet*)netd;
   sint15  ndssErr;
   dss_iface_ioctl_mt_dereg_cb_type mtpdp_dereg_cb;
   int ret;

   // remove waiter
   OEMNet_SetMTPDWaiter(pNet, NULL);

   // if oemnet is not registered for MT PDP, then do nothing ...
   if (!pNet->bMTpdpHandleValid) {
      return AEE_NET_SUCCESS;
   }

   mtpdp_dereg_cb.app_id = pNet->sAppID;
   mtpdp_dereg_cb.handle = pNet->mtpdpHandle;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MT_DEREG_CB,
                            &mtpdp_dereg_cb,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   pNet->bMTpdpHandleValid = FALSE;
   pNet->mtpdpHandle = 0; /* set to a known value */

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_InstallSocketEventHandler

DESCRIPTION
  Installs a socket event handler that will be called whenever a
  socket event occurs.  The previously installed event handler will be
  returned, and should be called if the event is not consumed.

  An installed socket handler cannot be deinstalled.

===========================================================================*/
PFNNOTIFY OEMNet_InstallSocketEventHandler
(
   OEMCONTEXT netd,
   PFNNOTIFY pfnSocketEventHandler
)
{
   OEMNet* pNet = (OEMNet*)netd;
   PFNNOTIFY prevInterceptor = pNet->pfnSocketEventHandlerInterceptor;

   pNet->pfnSocketEventHandlerInterceptor = pfnSocketEventHandler;

   if (prevInterceptor == NULL) {
      // if no prev Interceptor, then return a shim for the OEMNet's
      // socket handler callback
      prevInterceptor = &OEMNet_NonInterceptedSocketEventOccurred;
   }

   return prevInterceptor;
}


static void OEMNet_MonitorPhysLink(OEMNet* pNet, boolean bRegister)
{
   (void)OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PHYS_LINK_DOWN_EV, bRegister);
   (void)OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PHYS_LINK_UP_EV, bRegister);
   (void)OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PHYS_LINK_COMING_UP_EV, bRegister);
   (void)OEMNet_RegisterIfaceEventCallback(pNet, DSS_IFACE_IOCTL_PHYS_LINK_GOING_DOWN_EV, bRegister);
}

static void OEMNet_WatchForDormancy(OEMNet* pNet)
{
   pNet->bWatchForDormancy = TRUE;
   OEMNet_MonitorPhysLink(pNet, TRUE);
}

static void OEMNet_DoNotWatchForDormancy(OEMNet* pNet)
{
   pNet->bWatchForDormancy = FALSE;
   OEMNet_MonitorPhysLink(pNet, FALSE);
}

// Call dss_pppopen() for use by both OEMNet_PPPOpen() and OEMNet_ReconcileFailover()
static int16 OEMNet_PPPOpen2
(
   OEMNet* pNet
)
{
   int16 ndssErr;
   int16   nRet;

   if (DSS_SUCCESS != dss_pppopen(pNet->sAppID, &ndssErr)) {
      nRet = DSSToAEE_Err(ndssErr);
   } else {
      nRet = AEE_NET_SUCCESS;
   }

   DBGPRINTF("***zzg OEMNet_PPPOpen2  nRet=%x***", nRet);

   if (AEE_NET_ENETCLOSEINPROGRESS == nRet) {
      // retry ppp open attempt a little later
      AEE_SetSysTimerCallback(200, &pNet->cbRetryPPPOpen);

      // for now force network state to closing
      pNet->bPPPOpenCloseInProgress = TRUE;

      OEMNet_SetState(pNet, NET_PPP_CLOSING);
      // make sure AEE knows about the new Network state
      AEE_SYS_RESUME_IF(pNet->pcbNetWaiter);
   } else {
      // in case someone else tries a pppopen while a retry attempt is scheduled ...
      CALLBACK_Cancel(&pNet->cbRetryPPPOpen);
      pNet->bPPPOpenCloseInProgress = FALSE;
   }

   if ((AEE_NET_SUCCESS == nRet) ||
       (AEE_NET_EWOULDBLOCK == nRet)) {
      // Tell the system not to open any call related dialogs...
      CALLBACK_Cancel(&pNet->cbReEnableCallDialogs);
      OEM_Notify(OEMNTF_SHOW_CALL_DIALOGS, FALSE);

      if (AEE_NET_EWOULDBLOCK == nRet) {
         OEMNet_SetState(pNet, NET_PPP_OPENING);
         AEE_SYS_RESUME_IF(pNet->pcbNetWaiter);
      } else {
         OEMNet_SetState(pNet, NET_PPP_OPEN);
      }

#if defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)
      // each OEMNet instance should register as a TAPI user only once.
      // to assure the correctness of the TAPI reference count
      if (!pNet->bRegisteredToTAPI) {
         pNet->bRegisteredToTAPI = TRUE;
         OEMDSS_SetupTAPI();
      }
#endif // defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)

      OEMNet_WatchForDormancy(pNet);
   }

   return nRet;
}

/*===========================================================================

FUNCTION OEMNet_PPPOpen

DESCRIPTION
  Starts up the network subsystem (CDMA data service and PPP) over the Um
  interface for all sockets.

DEPENDENCIES
  dss_pppopen() cannot be called by the application if the network is in the
  process of closing. The network layer cannot queue the open request until
  the close is completely finished.  Therefore, the application should wait
  for the net_callback_fn() to be called (after dss_pppclose() has
  completed), before making a call to dss_pppopen().  Note that a valid
  application ID must be specified as a parameter, obtained by a successful
  return of dss_open_netlib2().

===========================================================================*/
int16 OEMNet_PPPOpen
(
   OEMCONTEXT netd
)
{
   OEMNet* pNet = (OEMNet*)netd;

   DBGPRINTF("***zzg OEMNet_PPPOpen  state=%x***", OEMNet_PPPState(pNet));

   // can't call dss_pppopen() while the network is still closing
   if (NET_PPP_CLOSING == OEMNet_PPPState(pNet)) {
      FARF(ALWAYS,("# PPPOpen: PPP still closing - Abort!"));
      return AEE_NET_GENERAL_FAILURE;
   }

#if defined(FAILOVER_3G_TO_2G)
   {
      boolean bAllow;

      // By default, the failover is disabled
      if (SUCCESS != OEM_GetConfig(CFGI_ALLOW_3G_2G_FAILOVER,
                                   (void *)&bAllow,sizeof(bAllow))) {
         bAllow = FALSE;
      }

      if (bAllow) {
         int ret;
         dss_iface_ioctl_707_mdr_type mdrMode;
         sint15 ndssErr;

         ret = OEMNet_Iface_Ioctl(pNet,
                                  DSS_IFACE_IOCTL_707_GET_MDR,
                                  &mdrMode,
                                  &ndssErr);
         if (DSS_SUCCESS == ret) {
            FARF(FAILOVER_3G_TO_2G,("MDR mode %d",mdrMode));

            if (DS_MDR_MODE_SO33_PREF == mdrMode) {
                  pNet->PPPState.bTrying3G = TRUE;
            }

         } else {
            FARF(FAILOVER_3G_TO_2G,("get MDR mode failed!"));
         }
      }
   }
#endif /* #if defined(FAILOVER_3G_TO_2G) */

   return OEMNet_PPPOpen2(pNet);
}

int16 OEMNet_SetPPPAuth(OEMCONTEXT netd,
                        const char *cpszAuth)
{
   return EUNSUPPORTED;
}

int16 OEMNet_GetPPPAuth(OEMCONTEXT netd,
                        char *pszAuth, int *pnLen)
{
   return EUNSUPPORTED;
}

// Data dedicated transmission mode for hybrid HDR (MSM5500/6500)
int16 OEMNet_SetDDTMPref(OEMCONTEXT netd,
                         boolean bOn)
{
#if defined(FEATURE_DDTM_CNTL)
#ifdef SYS_DDTM_MAX_SO_LIST_SIZE
   sys_srv_opt_type so_list[1] = { (sys_srv_opt_type)CAI_SO_NULL };
#endif
   cm_client_status_e_type nStatus;
   cm_client_id_type nClient;
   boolean bRet;

   nStatus = cm_client_init(CM_CLIENT_TYPE_FLOATING3, &nClient);
   if (CM_CLIENT_OK != nStatus) {
      return EFAILED;
   }

   // Don't worry about specifying a callback, because 1) this will make this
   // operation asynchronous, and 2)the cm code has no failure cases
   // specific to this command
   bRet = cm_ph_cmd_ddtm_pref(
			NULL,
			NULL,
			nClient,
			bOn ? CM_DDTM_PREF_ON : CM_DDTM_PREF_OFF
#ifdef SYS_DDTM_MAX_SO_LIST_SIZE
         ,SYS_DDTM_ACT_MASK_IGN_SO_PAGES,
	      CM_DDTM_SO_LIST_ACT_DELETE,
        	sizeof(so_list),
         so_list
#endif
			);
   if (FALSE == bRet) {
      (void)cm_client_release(nClient);
      return EFAILED;
   }

   (void)cm_client_release(nClient); // only fails in case of invalid client id
   return SUCCESS;
#else
   return EUNSUPPORTED;
#endif
}

/*===========================================================================

FUNCTION OEMNet_PPPClose

DESCRIPTION
  Initiates termination to bring down PPP and the traffic channel.  Upon
  successful close of the network subsystem, invokes the network callback
  function.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_PPPClose
(
   OEMCONTEXT netd
)
{
   OEMNet* pNet = (OEMNet*)netd;
   int16 ndssErr;

#if defined(FAILOVER_3G_TO_2G)
   pNet->PPPState.bTrying3G = FALSE;
#endif /* #if defined(FAILOVER_3G_TO_2G) */

	DBGPRINTF("***zzg OEMNet_PPPClose dss_netstatus=%x***", dss_netstatus(pNet->sAppID, &ndssErr));
	
   if ((DSS_SUCCESS != dss_pppclose(pNet->sAppID, &ndssErr)) &&
       (DS_ENETNONET != ndssErr)) {

      if (DS_EWOULDBLOCK == ndssErr) 
	  {
         OEMNet_SetState(pNet, NET_PPP_CLOSING);
         AEE_SYS_RESUME_IF(pNet->pcbNetWaiter);
      }

      return DSSToAEE_Err(ndssErr);
   }


#if defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)
       // each OEMNet instance should de-register from TAPI only once,
       // to assure the correctness of the TAPI reference count
       if (pNet->bRegisteredToTAPI) {
          pNet->bRegisteredToTAPI = FALSE;
          OEMDSS_FreeTAPI(0);
       }
#endif // defined(CLOSE_PPP_ON_OUT_OF_NETWORK_COVERAGE)

   OEMNet_DoNotWatchForDormancy(pNet);

   // Tell the system it can open any call related dialogs...
   AEE_SetSysTimerCallback(3000, &pNet->cbReEnableCallDialogs);

   return AEE_NET_SUCCESS;
}

/*===========================================================================
  called to reconcile NET_PPP_OPEN with physical link state
  ===========================================================================*/
static NetState OEMNet_ReconcilePhysLink(OEMNet* pNet, NetState ns)
{
   int ret;
   dss_iface_ioctl_phys_link_state_type phys_link_state;
   sint15 ndssErr;

   if (NET_PPP_OPEN == ns) {
      ret = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_GET_PHYS_LINK_STATE,
                               &phys_link_state,
                               &ndssErr);
      if (DSS_SUCCESS == ret) {
         // network is open, so figure out the exact network state based on physical link state
         switch (phys_link_state) {
            case PHYS_LINK_UP:
               if (NET_FAKE_GOING_DOWN == pNet->PPPState.nFakePhysLinkState) {
                  /* have called force dormancy, haven't been called back */
                  FARF(OEM_NETSTATE,("Faking sleeping"));
                  ns = NET_PPP_SLEEPING;
               }
               break;
            case PHYS_LINK_DOWN:
               if (NET_FAKE_COMING_UP == pNet->PPPState.nFakePhysLinkState) {
                  /* have called wakeup, haven't been called back */
                  FARF(OEM_NETSTATE,("Faking waking"));
                  ns = NET_PPP_WAKING;
               } else {
                  ns = NET_PPP_ASLEEP;
               }
               break;
            case PHYS_LINK_COMING_UP:
               ns = NET_PPP_WAKING;
               break;
            case PHYS_LINK_GOING_DOWN:
               ns = NET_PPP_SLEEPING;
               break;
         }
      } else {
         FARF(ALWAYS, ("DSS GET_PHYS_LINK_STATE failed: %d", ndssErr));
      }
   }
   return ns;
}

static NetState OEMNet_NetStatus(OEMNet* pNet)
{
   NetState ns;

   ns = OEMNet_GetState(pNet);

   FARF(OEM_NETSTATE,("dmss state %s",
               ns==NET_PPP_CLOSED?"closed":
               ns==NET_PPP_OPEN?"open":
               ns==NET_PPP_OPENING?"opening":
               ns==NET_PPP_CLOSING?"closing":"invalid"));


#if defined(FEATURE_CLOSE_TCP_SOCKETS_ON_NETWORK_CLOSE)
   /* Address out of coverage issue */
   if (NET_PPP_CLOSED == ns) {
      OEMSocket* ps;
      FARF(OEM_NETSTATE,("closing all dss_sockets, Data Network is closed"));
      for (ps = pNet->pSockets; ((OEMSocket*)0 != ps && ps->bStream);  ps = ps->pNext) {
         int16  ndssErrIgnored;
         dss_close(ps->sockfd, &ndssErrIgnored);
         ps->sockfd = DSS_ERROR;
      }
   }
#endif

   return ns;
}

/* ====================================================================
   fakes out state if we're doing app-level failover, returns TRUE
   if you should re-call netstatus, returns FALSE otherwise

   *pns might get changed
   ==================================================================== */
static NetState OEMNet_ReconcileFailover(OEMNet* pNet, NetState ns)
{
#if !defined(FAILOVER_3G_TO_2G)
   return ns;
#else /* #if !defined(FAILOVER_3G_TO_2G) */

   if (pNet->PPPState.bTrying3G) {

      /* closing, set to opening, wait until we're all the way closed... */
      if (ns == NET_PPP_CLOSING) {
         FARF(FAILOVER_3G_TO_2G,("3g try failing"));

         return NET_PPP_OPENING;
      }

      if (ns == NET_PPP_CLOSED) {
         int ret;
         dss_iface_ioctl_707_mdr_type mdrMode = DS_MDR_MODE_NO_MDR;
         sint15 ndssErr;

         FARF(FAILOVER_3G_TO_2G,("3g try failed"));
         /* closed, set to opening, retry open
            after setting MDR mode to 2G... */

         pNet->PPPState.bTrying3G = FALSE;

         ret = OEMNet_Iface_Ioctl(pNet,
                                  DSS_IFACE_IOCTL_707_SET_MDR,
                                  &mdrMode,
                                  &ndssErr);
         if (DSS_SUCCESS == ret) {
            /* retry, ignore return value.... */
            FARF(FAILOVER_3G_TO_2G,("retry 2g..."));

               pNet->PPPState.bRestore3G = TRUE;

            // Just do the bottom half of OEMNet_PPPOpen()
               OEMNet_PPPOpen2(pNet);

            /* re-derive netstate */
               return OEMNet_NetStatus(pNet);

         } else {
            FARF(FAILOVER_3G_TO_2G,("set MDR mode failed!"));
         }
      }
   }

   if (ns == NET_PPP_OPEN) {
      /* open, not trying anymore */
      pNet->PPPState.bTrying3G = FALSE;
   }

   if (pNet->PPPState.bRestore3G &&
       ((ns == NET_PPP_OPEN) || ns == NET_PPP_CLOSED)) {
      int ret;
      dss_iface_ioctl_707_mdr_type mdrMode = DS_MDR_MODE_SO33_PREF;
      sint15 ndssErr;

      /* We've finally got the 'net up, and need to restore the MDR mode.
         This API call doesn't affect the current data call */

      ret = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_707_SET_MDR,
                               &mdrMode,
                               &ndssErr);
      if (DSS_SUCCESS == ret) {
         FARF(FAILOVER_3G_TO_2G,("restored 3g..."));
         pNet->PPPState.bRestore3G = FALSE;
      } else {
         FARF(FAILOVER_3G_TO_2G,("set MDR mode failed!"));
      }
   }

   return ns;
#endif /* #if !defined(FAILOVER_3G_TO_2G) */
}

static void OEMNet_ReTryPPPOpen(void *po)
{
   OEMNet* pNet = (OEMNet*)po;

   (void)OEMNet_PPPOpen2(pNet);
}

static void OEMNet_UpdateOutageTime(void *pv)
{
   OEMNet* pNet = (OEMNet*)pv;

   INTLOCK();
   MEMCPY(&(pNet->ooiOutageInfo.aoiOutageInfo),
          &(pNet->ooiOutageInfo.aoiNewOutageInfo),
          sizeof(AEEOutageInfo));
   pNet->ooiOutageInfo.uTimeModified = GETUPTIMEMS();
   INTFREE();
}

/*===========================================================================

FUNCTION OEMNet_PPPState

DESCRIPTION
  Returns the state of the PPP connection.

DEPENDENCIES

===========================================================================*/
NetState OEMNet_PPPState
(
   OEMCONTEXT netd
)
{
   OEMNet* pNet = (OEMNet*)netd;
   NetState  ns;

   ns = OEMNet_NetStatus(pNet);

   // special case for ppp open waiting for network close
   if ((AEE_NET_STATUS_CLOSED == ns) && pNet->bPPPOpenCloseInProgress) {
      ns = AEE_NET_STATUS_CLOSING;
   }

   /* reconcile state with failover state */
   ns = OEMNet_ReconcileFailover(pNet, ns);

   /* see if open really means one of the dormancy states */
   ns = OEMNet_ReconcilePhysLink(pNet, ns);

   FARF(OEM_NETSTATE,("oemnet state %s",
                     ns==NET_PPP_CLOSED?"closed":
                     ns==NET_PPP_OPEN?"open":
                     ns==NET_PPP_OPENING?"opening":
                     ns==NET_PPP_SLEEPING?"sleeping":
                     ns==NET_PPP_WAKING?"waking":
                     ns==NET_PPP_ASLEEP?"asleep":
                     ns==NET_PPP_CLOSING?"closing":"invalid"));

   return  ns;
}

/*===========================================================================

FUNCTION OEMNet_MyIPAddr

DESCRIPTION

  Returns the IP address of the active session.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
int16 OEMNet_MyIPAddr
(
   OEMCONTEXT  netd,
   IPAddr     *addr
)
{
   OEMNet* pNet = (OEMNet*)netd;
   int ret;
   sint15 ndssErr;
   ip_addr_type ip_addr;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_GET_IP_ADDR,
                            &ip_addr,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   if (!IP_ADDR_IS_VALID(ip_addr)) {
      return AEE_NET_ENETNONET;
   }

   switch (ip_addr.type)
   {
      case IPV4_ADDR:
         addr->wFamily = AEE_AF_INET;

         if (NET_PPP_CLOSED == OEMNet_PPPState(netd)) {
            addr->addr.v4 = INADDR_ANY;
         } else {
            addr->addr.v4 = IP_ADDR_V4(ip_addr);
         }

         break;
#ifdef FEATURE_DATA_PS_IPV6
      case IPV6_ADDR:
         addr->wFamily = AEE_AF_INET6;

         if (NET_PPP_CLOSED == OEMNet_PPPState(netd)) {
            memset(addr->addr.v6.Byte, 0, sizeof(addr->addr.v6.Byte));
         } else {
            memcpy(addr->addr.v6.Byte, ip_addr.addr.v6, sizeof(addr->addr.v6.Byte));
         }
         break;
#endif
      default:
         return AEE_NET_EAFNOSUPPORT;
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetUrgent

DESCRIPTION

  Discovers status of urgent (short data burst) support

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
int16 OEMNet_GetUrgent
(
   OEMCONTEXT netd,
   AEEUDPUrgent *pUrg
)
{
#if defined(SDB_SUPPORTED)
   OEMNet* pNet = (OEMNet*)netd;
   int ret;
   sint15 ndssErr;
   dss_iface_ioctl_707_sdb_support_query_type query = {0,0};

   if (!pUrg) {
      return EBADPARM;
   }

   // needs to match SDB sendto flags
   query.flags = MSG_FAST_EXPEDITE;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_707_SDB_SUPPORT_QUERY,
                            &query,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   pUrg->bUrgentSupported = query.can_do_sdb;

// DBM limit 255 - IPv4 header size 20 - UDP header size 8
// Note that this protocol-defined limit is merely theoretical. The
// actual limit is imposed by the physical layer in terms of acceptable
// data rates and access probe duration, and by the signaling layer's
// need to add other headers to the data burst message.
#define MAX_UDP_VIA_SDB 222

   pUrg->nUrgentLimit = pUrg->bUrgentSupported ? MAX_UDP_VIA_SDB : 0;
   return AEE_NET_SUCCESS;
#endif // defined(SDB_SUPPORTED)
   return EUNSUPPORTED;
}

/*===========================================================================

FUNCTION OEMNet_GetNativeDescriptor

DESCRIPTION
  Returns the underlying network descriptor

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMNet_GetNativeDescriptor
(
   OEMCONTEXT  netd,
   uint32     *pdwDesc
)
{
   OEMNet* pNet = (OEMNet*)netd;

   if ((uint32*)0 == pdwDesc) {
      return AEE_NET_EINVAL;
   }

   *pdwDesc = pNet->sAppID;

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetDefaultNetwork

DESCRIPTION
   Returns the default network (AEE_NETWORK_*)

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMNet_GetDefaultNetwork
(
   int16   sFamily,
   int*    pnNetwork
)
{
#if !defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)
   // Error checking on sFamily
   switch(sFamily)
   {
      case AEE_AF_UNSPEC:
      case AEE_AF_LOCAL:
      case AEE_AF_INET:
      case AEE_AF_INET6:
	 break;

      default:
         return AEE_NET_EAFNOSUPPORT;
   }
   // If DSS_GET_IFACE_ID_BY_POLICY_VERS is not defined then fallback to the old
   // implemetnation with DSS_IFACE_IOCTL_GET_ALL_IFACES
   return OEMDSS_GetDefaultNetwork(pnNetwork);

#else //!defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)

   dss_net_policy_info_type policy_info;
   dss_iface_ioctl_iface_name_type iface_name_type;
   dss_iface_id_type iface_id;
   sint15 ndssErr;
   int ret;

   if ((int*)0 == pnNetwork) {
      return AEE_NET_EINVAL;
   }

   // Initialize to defaults
   dss_init_net_policy_info(&policy_info);

   policy_info.iface.kind = DSS_IFACE_NAME;
   policy_info.iface.info.name = DSS_IFACE_ANY_DEFAULT;

   // This is currently unsupported in AMSS for UMTS environment
   // since there is only one default iface which is IPv4.
   // (For CDMA environment it has no significance since the same
   // default is used for IPv4 and IPv6).

   // Check the address family
   switch (sFamily)
   {
      case AEE_AF_INET:
      case AEE_AF_INET6:
      case AEE_AF_UNSPEC:
         break;

      default:
         return AEE_NET_EAFNOSUPPORT;
   }

#ifdef OEM_DISABLE_MULTIPLE_DEFAULT_IFACE_LOOKUPS

   if (FALSE == bDefaultIfaceFetched) {
      iface_id = dss_get_iface_id_by_policy(policy_info, &ndssErr);
      if (DSS_IFACE_INVALID_ID == iface_id) {
         return DSSToAEE_Err(ndssErr);
      }
      bDefaultIfaceFetched = TRUE;
      fetched_iface_id = iface_id;
   }
   else {
      iface_id = fetched_iface_id;
   }

#else // #ifdef OEM_DISABLE_MULTIPLE_DEFAULT_IFACE_LOOKUPS

   iface_id = dss_get_iface_id_by_policy(policy_info, &ndssErr);
   if (DSS_IFACE_INVALID_ID == iface_id) {
      return DSSToAEE_Err(ndssErr);
   }

#endif // #else OEM_DISABLE_MULTIPLE_DEFAULT_IFACE_LOOKUPS
   ret = oemdss_iface_ioctl(iface_id, DSS_IFACE_IOCTL_GET_IFACE_NAME, &iface_name_type, &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   switch (iface_name_type) {

      case FLO_IFACE:
      {
         *pnNetwork = AEE_NETWORK_MFLO_MCAST;
         return AEE_NET_SUCCESS;
      }

      case DVBH_IFACE:
      {
         *pnNetwork = AEE_NETWORK_DVBH_MCAST;
         return AEE_NET_SUCCESS;
      }

      case WLAN_IFACE:
      {
         *pnNetwork = AEE_NETWORK_WLAN;
         return AEE_NET_SUCCESS;
      }

      case SLIP_IFACE:
      {
         *pnNetwork = AEE_NETWORK_SLIP;
         return AEE_NET_SUCCESS;
      }

      case UW_FMC_IFACE:
      {
         *pnNetwork = AEE_NETWORK_UW_FMC;
         return AEE_NET_SUCCESS;
      }

      case CDMA_SN_IFACE:
      {
         *pnNetwork = AEE_NETWORK_CDMA(0); // default profile
         return AEE_NET_SUCCESS;
      }

      case LO_IFACE:
      {
         *pnNetwork = AEE_NETWORK_LOOPBACK;
         return AEE_NET_SUCCESS;
      }

      case DSS_IFACE_3GPP2_ANY:
      {
         *pnNetwork = AEE_NETWORK_3GPP2_ANY(0);
         return AEE_NET_SUCCESS;
      }

      case DSS_IFACE_IWLAN_3GPP2:
      {
         *pnNetwork = AEE_NETWORK_IWLAN_3GPP2(0);
         return AEE_NET_SUCCESS;
      }
#if defined(FEATURE_DATA_UMTS)

      case DSS_IFACE_3GPP_ANY:
      {
         *pnNetwork = AEE_NETWORK_3GPP_ANY(0);
         return AEE_NET_SUCCESS;
      }

      case DSS_IFACE_IWLAN_3GPP:
      {
         *pnNetwork = AEE_NETWORK_IWLAN_3GPP(0);
         return AEE_NET_SUCCESS;
      }

      case UMTS_IFACE:
      {
         byte   profile_number = 1; // default profile
#ifdef FEATURE_DATA_PS_IPV6
         uint16 tmp_prof_num;
         ds_umts_pdp_profile_type          profile_info;
         ds_umts_pdp_profile_status_etype  status;
#endif // FEATURE_DATA_PS_IPV6

         if ( (AEE_AF_INET == sFamily) || (AEE_AF_UNSPEC == sFamily) ) {
            // Get profile number for IPv4 requested default network
            byte configured_default;
            ds_umts_pdp_profile_status_etype status;

            // See if there is a configured_default configured in the PDP database
            status = ds_umts_get_pdp_profile_num_for_sock_calls(&configured_default);
            if (DS_UMTS_PDP_SUCCESS == status) {
               profile_number = configured_default;
            }

            FARF(ALWAYS, ("OEMNet_GetDefaultNetwork: UMTS. IPv4. Found PDP#%u",profile_number));
            *pnNetwork = AEE_NETWORK_UMTS(profile_number);
            return AEE_NET_SUCCESS;
         }

#ifdef FEATURE_DATA_PS_IPV6
         // Get profile number for IPv6 requested default network
         // Note: this is a temporary implementation for finding an IPv6 PDP
         //       All available PDPs are queried. The first IPv6 PDP found is used.
         for (tmp_prof_num=1; tmp_prof_num <= DS_UMTS_MAX_PDP_PROFILE_NUM ; tmp_prof_num++ ) {

            status = ds_umts_get_pdp_profile_all_data(tmp_prof_num, &profile_info);
            if ( status != DS_UMTS_PDP_SUCCESS ) {
               FARF(UMTS_ERROR,("Failed to get pdp %u profile data. status=%u",tmp_prof_num, status));
               return AEE_NET_EFAULT;
            }

            if ( (TRUE == profile_info.context.valid_flg) &&
                 (DS_UMTS_PDP_IPV6 == profile_info.context.pdp_type) ) {
               // Found valid profile for IPv6.
               profile_number = (byte)tmp_prof_num;
               FARF(ALWAYS, ("GetDefaultNetwork: Using profile #%u for IPv6",profile_number));
               *pnNetwork = AEE_NETWORK_UMTS(profile_number);
               return AEE_NET_SUCCESS;
            }
         }

         FARF(UMTS_ERROR,("GetDefaultNetwork: Failed to find IPv6 PDP"));
         return AEE_NET_EAFNOSUPPORT;
#else  // FEATURE_DATA_PS_IPV6
         FARF(UMTS_ERROR,("GetDefaultNetwork: Failed to select PDP for default network"));
         return AEE_NET_EAFNOSUPPORT;
#endif // FEATURE_DATA_PS_IPV6

      }
#endif // defined(FEATURE_DATA_UMTS)

      default:
         return AEE_NET_EFAULT;
   }
#endif //!defined(DSS_GET_IFACE_ID_BY_POLICY_VERS)
}

/*===========================================================================

FUNCTION OEMNet_GetAppProfileId

DESCRIPTION
   Returns the profile id of the application.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMNet_GetAppProfileId
(
   uint32 uAppType,
   int16  *pnProfile
)
{
   int nProfile;

   nProfile = dss_get_app_profile_id(uAppType);
   if (nProfile < 0 || nProfile > MAX_INT16) {
      return AEE_NET_ERROR;
   }
   *pnProfile = (int16)nProfile;

   return AEE_NET_SUCCESS;
}

/*===========================================================================

Function: OEMNet_GetLastNetDownReason()

Description:
   This function returns the network down reason for the
   last network interface that the application was bound to.

Prototype:
   int16 OEMNet_GetLastNetDownReason
   (
      OEMCONTEXT        netd,
      AEENetDownReason* pnNetDownReason
   );

Parameters:
   netd: network descriptor
   pnNetDownReason: [out] - pointer to the last network down reason.
   On AEE_NET_SUCCESS will be filled with the reason value (pre-allocated by caller).

Return Value:
   AEE_NET_SUCCESS: Success. pnNetDownReason filled with last network down reason.
   AEE_NET_EFAULT: Lower layer returned an unexpected network down reason.

   Other AEE designated error codes might be returned.

Comments:
   None

Side Effects:
   None

See Also:
   AEENetDownReason
===========================================================================*/
int16 OEMNet_GetLastNetDownReason
(
   OEMCONTEXT        netd,
   AEENetDownReason* pnNetDownReason
)
{
   OEMNet* pNet = (OEMNet*)netd;
   dss_net_down_reason_type net_down_reason;
   sint15  ndssErr;
   sint15  ret;

   if ((int*)0 == pnNetDownReason){
      return AEE_NET_EINVAL;
   }

   ret = dss_last_netdownreason(pNet->sAppID, &net_down_reason, &ndssErr);

   if (DSS_SUCCESS != ret){
      return DSSToAEE_Err(ndssErr);
   }

   switch (net_down_reason){
      case DSS_NET_DOWN_REASON_NOT_SPECIFIED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_NOT_SPECIFIED;
         break;
      case DSS_NET_DOWN_REASON_UMTS_REATTACH_REQ:
         *pnNetDownReason = AEE_NET_DOWN_REASON_UMTS_REATTACH_REQ;
         break;
      case DSS_NET_DOWN_REASON_CLOSE_IN_PROGRESS:
         *pnNetDownReason = AEE_NET_DOWN_REASON_CLOSE_IN_PROGRESS;
         break;
      case DSS_NET_DOWN_REASON_NW_INITIATED_TERMINATION:
         *pnNetDownReason = AEE_NET_DOWN_REASON_NW_INITIATED_TERMINATION;
         break;
      case DSS_NET_DOWN_REASON_INSUFFICIENT_RESOURCES:
         *pnNetDownReason = AEE_NET_DOWN_REASON_INSUFFICIENT_RESOURCES;
         break;
      case DSS_NET_DOWN_REASON_UNKNOWN_APN:
         *pnNetDownReason = AEE_NET_DOWN_REASON_UNKNOWN_APN;
         break;
      case DSS_NET_DOWN_REASON_UNKNOWN_PDP:
         *pnNetDownReason = AEE_NET_DOWN_REASON_UNKNOWN_PDP;
         break;
      case DSS_NET_DOWN_REASON_AUTH_FAILED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_AUTH_FAILED;
         break;
      case DSS_NET_DOWN_REASON_GGSN_REJECT:
         *pnNetDownReason = AEE_NET_DOWN_REASON_GGSN_REJECT;
         break;
      case DSS_NET_DOWN_REASON_ACTIVATION_REJECT:
         *pnNetDownReason = AEE_NET_DOWN_REASON_ACTIVATION_REJECT;
         break;
      case DSS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_OPTION_NOT_SUPPORTED;
         break;
      case DSS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_OPTION_UNSUBSCRIBED;
         break;
      case DSS_NET_DOWN_REASON_OPTION_TEMP_OOO:
         *pnNetDownReason = AEE_NET_DOWN_REASON_OPTION_TEMP_OOO;
         break;
      case DSS_NET_DOWN_REASON_NSAPI_ALREADY_USED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_NSAPI_ALREADY_USED;
         break;
      case DSS_NET_DOWN_REASON_REGULAR_DEACTIVATION:
         *pnNetDownReason = AEE_NET_DOWN_REASON_REGULAR_DEACTIVATION;
         break;
      case DSS_NET_DOWN_REASON_QOS_NOT_ACCEPTED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_QOS_NOT_ACCEPTED;
         break;
      case DSS_NET_DOWN_REASON_NETWORK_FAILURE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_NETWORK_FAILURE;
         break;
      case DSS_NET_DOWN_REASON_TFT_SEMANTIC_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_TFT_SEMANTIC_ERROR;
         break;
      case DSS_NET_DOWN_REASON_TFT_SYNTAX_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_TFT_SYNTAX_ERROR;
         break;
      case DSS_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT:
         *pnNetDownReason = AEE_NET_DOWN_REASON_UNKNOWN_PDP_CONTEXT;
         break;
      case DSS_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_FILTER_SEMANTIC_ERROR;
         break;
      case DSS_NET_DOWN_REASON_FILTER_SYNTAX_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_FILTER_SYNTAX_ERROR;
         break;
      case DSS_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT:
         *pnNetDownReason = AEE_NET_DOWN_REASON_PDP_WITHOUT_ACTIVE_TFT;
         break;
      case DSS_NET_DOWN_REASON_INVALID_TRANSACTION_ID:
         *pnNetDownReason = AEE_NET_DOWN_REASON_INVALID_TRANSACTION_ID;
         break;
      case DSS_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC:
         *pnNetDownReason = AEE_NET_DOWN_REASON_MESSAGE_INCORRECT_SEMANTIC;
         break;
      case DSS_NET_DOWN_REASON_INVALID_MANDATORY_INFO:
         *pnNetDownReason = AEE_NET_DOWN_REASON_INVALID_MANDATORY_INFO;
         break;
      case DSS_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_MESSAGE_TYPE_UNSUPPORTED;
         break;
      case DSS_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_MSG_TYPE_NONCOMPATIBLE_STATE;
         break;
      case DSS_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT:
         *pnNetDownReason = AEE_NET_DOWN_REASON_UNKNOWN_INFO_ELEMENT;
         break;
      case DSS_NET_DOWN_REASON_CONDITIONAL_IE_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_CONDITIONAL_IE_ERROR;
         break;
      case DSS_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;
         break;
      case DSS_NET_DOWN_REASON_PROTOCOL_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_PROTOCOL_ERROR;
         break;
      case DSS_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_UNKNOWN_CAUSE_CODE;
         break;
      case DSS_NET_DOWN_REASON_INTERNAL_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_INTERNAL_ERROR;
         break;
      case DSS_NET_DOWN_REASON_INTERNAL_CALL_ENDED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_INTERNAL_CALL_ENDED;
         break;
      case DSS_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_INTERNAL_UNKNOWN_CAUSE_CODE;
         break;
      case DSS_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING:
         *pnNetDownReason = AEE_NET_DOWN_REASON_OPERATOR_DETERMINED_BARRING;
         break;
      case DSS_NET_DOWN_REASON_LLC_SNDCP_FAILURE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_LLC_SNDCP_FAILURE;
         break;
      case DSS_NET_DOWN_REASON_APN_TYPE_CONFLICT:
         *pnNetDownReason = AEE_NET_DOWN_REASON_APN_TYPE_CONFLICT;
         break;
      case DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNABLE_TO_PROCESS;
         break;
      case DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_UNSUPPORTED_VERS;
         break;
      case DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_INSUFFICIENT_CHALLANGES;
         break;
      case DSS_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_CLIENT_ERR_RAND_NOT_FRESH;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_AFTER_AUTH;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_GENERAL_FAILURE_BEFORE_AUTH;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_TEMP_DENIED_ACCESS;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NOT_SUBSCRIBED;
         break;
      case DSS_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE:
         *pnNetDownReason = AEE_NET_DOWN_REASON_IPSEC_GW_UNREACHABLE;
         break;
      case DSS_NET_DOWN_REASON_IPSEC_AUTH_FAILED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_IPSEC_AUTH_FAILED;
         break;
      case DSS_NET_DOWN_REASON_IPSEC_CERT_INVALID:
         *pnNetDownReason = AEE_NET_DOWN_REASON_IPSEC_CERT_INVALID;
         break;
      case DSS_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR:
         *pnNetDownReason = AEE_NET_DOWN_REASON_IPSEC_INTERNAL_ERROR;
         break;
      case DSS_NET_DOWN_REASON_EAP_SUCCESS: 
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_SUCCESS;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE: 
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_REALM_UNAVAILABLE;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE: 
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_USER_NAME_UNAVAILABLE;
         break;
      case DSS_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED:
         *pnNetDownReason = AEE_NET_DOWN_REASON_EAP_NOTIFICATION_CALL_BARRED;
         break;
      default:
         // Convert new network down reason to use value sent 
         // by lower layers + 2000
         // 2000 is in order to not conflict with existing values of 
         // BREW network down reasons
         *pnNetDownReason = net_down_reason + 2000;
   }
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetUMTSCount

DESCRIPTION
   Returns the number of PDP contexts.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMNet_GetUMTSCount
(
   uint16    *pwCount
)
{
#if defined(FEATURE_DATA_UMTS)

   if ((uint16*)0 == pwCount) {
      return AEE_NET_EINVAL;
   }

   *pwCount = DS_UMTS_MAX_PDP_PROFILE_NUM;

   return AEE_NET_SUCCESS;
#endif // defined(FEATURE_DATA_UMTS)
   return EUNSUPPORTED;
}

/*===========================================================================

FUNCTION OEMNet_GetUMTSInfo

DESCRIPTION
  Returns the requested PDP context information.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMNet_GetUMTSInfo
(
   uint16       wNumber,
   AEEUMTSInfo *pInfo
)
{
#if defined(FEATURE_DATA_UMTS)
   ds_umts_pdp_profile_status_etype status;
   byte profile_number;
   char profile_name[DS_UMTS_MAX_PROFILE_NAME_LEN+1];

   profile_number = (byte)wNumber;

   status = ds_umts_get_pdp_profile_context_info_is_valid(profile_number, &pInfo->bValid);
   if (DS_UMTS_PDP_SUCCESS != status) {
      return AEE_NET_EINVAL;
   }

   status = ds_umts_get_pdp_profile_name(profile_number, (byte *)profile_name, sizeof(profile_name));
   if (DS_UMTS_PDP_SUCCESS != status) {
      return AEE_NET_ERROR;
   }

   STRTOWSTR(profile_name, pInfo->szName, sizeof(pInfo->szName));
   pInfo->szName[ARRAY_SIZE(pInfo->szName)-1] = 0;

   return AEE_NET_SUCCESS;
#endif // defined(FEATURE_DATA_UMTS)
   return EUNSUPPORTED;
}

/*===========================================================================

FUNCTION OEMNet_GetHWAddr

DESCRIPTION

  Obtains the hardware address

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
int16 OEMNet_GetHWAddr
(
   OEMCONTEXT netd,
   byte       addr[],
   int*       pnSize
)
{
   OEMNet* pNet = (OEMNet*)netd;
   int ret;
   sint15 ndssErr;
   dss_iface_ioctl_hw_addr_type hw_addr;

   if (!addr || !pnSize || !*pnSize) {
      return EBADPARM;
   }

   hw_addr.hw_addr_len = *pnSize;
   hw_addr.hw_addr = addr;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_GET_HW_ADDR,
                            &hw_addr,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   *pnSize = hw_addr.hw_addr_len;
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SetDormancyTimeout

DESCRIPTION
  Set the IS-707 (cdma2000 1x) packet data dormancy timeout value
  (idle timeout after which mobile should enter dormancy)

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMNet_SetDormancyTimeout
(
   OEMCONTEXT netd,
   uint8      ucSecs
)
{
   OEMNet*           pNet = (OEMNet*)netd;
   uint32            dssSecs = ucSecs;
   int16             ndssErr;
   int               ret;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_707_SET_DORM_TIMER,
                            &dssSecs,
                            &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_NameServers

DESCRIPTION
   Gets the addresses of DNS servers.
   The addresses are retrieved according to the AF of the used network interface.

   For IPv4 interface, up to 2 addresses may be returned:
   2 from the AMSS with
   DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR,
   DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR
   or 1 from NV with CFGI_DNS_IP1.

   For IPv6 interface, up to 4 addresses may be returned:
   4 from the AMSS with
   DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR,
   DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR,
   DSS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR,
   DSS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR
   or 1 from NV with CFGI_IPV6_DNS_IP1.

   The value 0xffeeeeff for CFGI_DNS_IP2 is treated as a special internal
   testing value and means return CFGI_DNS_IP1 (or CFGI_IPV6_DNS_IP1) as the
   only DNS server address!

DEPENDENCIES
   None.

RETURN VALUE
   SUCCESS

SIDE EFFECTS
   None.

===========================================================================*/
int16 OEMNet_NameServers
(
   OEMCONTEXT netd,
   IPAddr *ainaAddrs,
   int    *pnNumAddrs
)
{
   OEMNet* pNet = (OEMNet*)netd;
   IPAddr aina[4]; /* max for IPv4 is 2 and max for IPv6 is 4 */
   int    i = 0;
   INAddr ina = 0;
   IPAddr ipa;

   if (OEMNet_MyIPAddr(netd, &ipa) != AEE_NET_SUCCESS) {
      *pnNumAddrs = 0;
      return SUCCESS;
   }

   /* special internal testing value, means use IP1 as the only one! */
   OEM_GetConfig(CFGI_DNS_IP2, (void*) &ina, sizeof(ina));
   if ((INAddr)0xffeeeeff == ina) {
      /* return IP1 unconditionally */
      switch (ipa.wFamily) {
         case AEE_AF_INET:
            ina = 0;
            OEM_GetConfig(CFGI_DNS_IP1, (void*) &ina, sizeof(ina));
            if ((0 != ina) && ((INAddr)-1 != ina)) {
               IPADDR_FROM_INADDR(aina[i], ina);
               ++i;
            }
            goto bail;
#ifdef FEATURE_DATA_PS_IPV6
         case AEE_AF_INET6:
         {
            INAddr6 ina6;
            memset(ina6.Byte, 0, sizeof(ina6.Byte));
            OEM_GetConfig(CFGI_IPV6_DNS_IP1, (void*) &ina6, sizeof(ina6));
            if (!IN6_IS_ADDR_UNSPECIFIED(ina6.DWord)) {
               aina[i].wFamily = AEE_AF_INET6;
               memcpy(aina[i].addr.v6.Byte, &ina6, sizeof(aina[i].addr.v6.Byte));
               ++i;
            }
            goto bail;
         }
#endif // FEATURE_DATA_PS_IPV6
         default:
            *pnNumAddrs = 0;
            return SUCCESS;
      }
   }

   // only if network is not closed
   if (NET_PPP_CLOSED != OEMNet_PPPState(netd)) {
      int ret;
      sint15 ndssErr;
      ip_addr_type ip_addr;

      dss_iface_id_type iface_id = OEMNet_GetIfaceId(pNet);

      if (DSS_IFACE_INVALID_ID != iface_id) {
         switch (ipa.wFamily) {
#ifdef FEATURE_DATA_PS_IPV6
            case AEE_AF_INET6:
               ret = oemdss_iface_ioctl(iface_id,
                                        DSS_IFACE_IOCTL_GET_IPV6_PRIM_DNS_ADDR,
                                        &ip_addr,
                                        &ndssErr);
               if ((DSS_SUCCESS == ret) && IP_ADDR_IS_VALID(ip_addr)) {
                  aina[i].wFamily = AEE_AF_INET6;
                  memcpy(aina[i].addr.v6.Byte, ip_addr.addr.v6, sizeof(aina[i].addr.v6.Byte));
                  ++i;
               }

               ret = oemdss_iface_ioctl(iface_id,
                                        DSS_IFACE_IOCTL_GET_IPV6_SECO_DNS_ADDR,
                                        &ip_addr,
                                        &ndssErr);
               if ((DSS_SUCCESS == ret) && IP_ADDR_IS_VALID(ip_addr)) {
                  aina[i].wFamily = AEE_AF_INET6;
                  memcpy(aina[i].addr.v6.Byte, ip_addr.addr.v6, sizeof(aina[i].addr.v6.Byte));
                  ++i;
               }
               // fallthrough
#endif // FEATURE_DATA_PS_IPV6

            case AEE_AF_INET:
               ret = oemdss_iface_ioctl(iface_id,
                                        DSS_IFACE_IOCTL_GET_IPV4_PRIM_DNS_ADDR,
                                        &ip_addr,
                                        &ndssErr);
               if ((DSS_SUCCESS == ret) && IP_ADDR_IS_VALID(ip_addr)) {
                  IPADDR_FROM_INADDR(aina[i], IP_ADDR_V4(ip_addr));
                  ++i;
               }

               ret = oemdss_iface_ioctl(iface_id,
                                        DSS_IFACE_IOCTL_GET_IPV4_SECO_DNS_ADDR,
                                        &ip_addr,
                                        &ndssErr);
               if ((DSS_SUCCESS == ret) && IP_ADDR_IS_VALID(ip_addr)) {
                  IPADDR_FROM_INADDR(aina[i], IP_ADDR_V4(ip_addr));
                  ++i;
               }
               break;

            default:
               *pnNumAddrs = 0;
               return SUCCESS;
         }
      }
   }

bail:
   if ((IPAddr *)0 != ainaAddrs) {
      i = MIN(*pnNumAddrs, i);
      MEMMOVE(ainaAddrs,aina,i*sizeof(IPAddr));
   }
   *pnNumAddrs = i;

   return SUCCESS;
}

#if defined(AEE_SIMULATOR)
/*===========================================================================

FUNCTION OEMNet_SimulateQoSAwareUnawareEvent

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating QoS Aware/Unaware events.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_QOS_AWARE_UNAWARE_EVENT
   to 0xdeadbeef.

===========================================================================*/
int16 OEMNet_SimulateQoSAwareUnawareEvent(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbeef, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_ResetSimulatedQoSSystemType

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function resets the simulated network QoS System Type (QoS Aware/
   QoS Unaware/none). The simulated QoS system type might change as a result
   of simulated QoS Aware/Unaware events (SimulateQoSAwareUnawareEvent()).
   Since the QoS system type effects QoS operations, it should be returned to
   its actual type (according to the current network type) before any QoS
   operations.


DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_RESET_SIMULATED_QOS_SYSTEM_TYPE
   to 0xdeadbef0.

===========================================================================*/
int16 OEMNet_ResetSimulatedQoSSystemType(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef0, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulateBearerTechnologyChangeEvent

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating Bearer Technology Change events.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_BEARER_TECH_CHANGE_EVENT
   to 0xdeadbef1.

===========================================================================*/
int16 OEMNet_SimulateBearerTechnologyChangeEvent(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef1, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulateOutageEvent

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating Outage events.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_OUTAGE_EVENT
   to 0xdeadbef5.

===========================================================================*/
int16 OEMNet_SimulateOutageEvent(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef5, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulateQoSProfilesChangedEvent

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating QoS Profiles Changed events.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_QOS_PROFILES_CHANGED_EVENT
   to 0xdeadbef6.

===========================================================================*/
int16 OEMNet_SimulateQoSProfilesChangedEvent(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef6, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulateIPv6PrefixUpdatedEvent

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating IPv6 PREFIX_UPDATED events
   on the prefix corresponding to the address the user supplies.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_IPV6_PREFIX_UPDATED_EVENT
   to 0xdeadbef4.

===========================================================================*/
int16 OEMNet_SimulateIPv6PrefixUpdatedEvent(OEMCONTEXT netd, INAddr6* pAddr)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef4, pAddr, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulateIPv6PrefixDeprecatedEvent

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating IPv6 PREFIX_DEPRECATED events
   on the prefix corresponding to the address the user supplies.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_IPV6_PREFIX_DEPRECATED_EVENT
   to 0xdeadbef7.

===========================================================================*/
int16 OEMNet_SimulateIPv6PrefixDeprecatedEvent(OEMCONTEXT netd, INAddr6* pAddr)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef7, pAddr, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulateAsyncPrivIPv6AddrGeneration

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating an asynchronous generation
   of a private IPv6 address.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_ASYNC_PRIV_IPV6_ADDR_GENERATION
   to 0xdeadbef2.

===========================================================================*/
int16 OEMNet_SimulateAsyncPrivIPv6AddrGeneration(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef2, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_SimulatePrivIPv6AddrFailureGeneration

DESCRIPTION
   Internal, un-documented function, which is used only for testing!
   This function is used for simulating an IPv6 private address generation
   which fails.

DEPENDENCIES
   sim_dssocket2.c defines SIMDSS_IFACE_IOCTL_SIMULATE_PRIV_IPV6_ADDR_FAILURE_GENERATION
   to 0xdeadbef3.

===========================================================================*/
int16 OEMNet_SimulatePrivIPv6AddrFailureGeneration(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   int nRet;
   int16 ndssErr;
   nRet = OEMNet_Iface_Ioctl(pNet, 0xdeadbef3, NULL, &ndssErr);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(ndssErr);
   }
   return SUCCESS;
}
#endif // defined(AEE_SIMULATOR)

#if defined(RLP3_CFG_SUPPORTED)
static AEERLP3Cfg *DSSIFaceIOCtlToAEE_RLP3Cfg
(
   const dss_iface_ioctl_707_rlp_opt_type *pfrom,
   AEERLP3Cfg *pto
)
{
   int i;

   for (i = 0; i < ARRAY_SIZE(pto->aucFwdNaksPerRound); i++) {
      pto->aucFwdNaksPerRound[i] = pfrom->naks_per_round_fwd[i];
      pto->aucRevNaksPerRound[i] = pfrom->naks_per_round_rev[i];
   }
   pto->ucFwdNakRounds = pfrom->nak_rounds_fwd;
   pto->ucRevNakRounds = pfrom->nak_rounds_rev;

   return pto;
}

static dss_iface_ioctl_707_rlp_opt_type *AEEToDSSIFaceIOCtl_RLP3Cfg
(
   const AEERLP3Cfg *pfrom,
   dss_iface_ioctl_707_rlp_opt_type *pto
)
{
   int i;

   pto->rscb_index = DSRLP_FIXED_SR_ID_INDEX;
   for (i = 0; i < ARRAY_SIZE(pto->naks_per_round_fwd); i++) {
      pto->naks_per_round_fwd[i] = pfrom->aucFwdNaksPerRound[i];
      pto->naks_per_round_rev[i] = pfrom->aucRevNaksPerRound[i];
   }
   pto->nak_rounds_fwd = pfrom->ucFwdNakRounds;
   pto->nak_rounds_rev = pfrom->ucRevNakRounds;

   return pto;
}

static dss_iface_ioctl_type AEEToDSSIFaceIOCtl_IOCtlName(int16 nOptName, boolean bGet)
{
   switch (nOptName) {
      case INET_OPT_DEF_RLP3:
         return bGet ? DSS_IFACE_IOCTL_707_GET_RLP_ALL_DEF_NAK : DSS_IFACE_IOCTL_707_SET_RLP_ALL_DEF_NAK;
      case INET_OPT_CUR_RLP3:
         return bGet ? DSS_IFACE_IOCTL_707_GET_RLP_ALL_CURR_NAK : DSS_IFACE_IOCTL_707_SET_RLP_ALL_CURR_NAK;
      case INET_OPT_NEG_RLP3:
         return bGet ? DSS_IFACE_IOCTL_707_GET_RLP_ALL_NEG_NAK : DSS_IFACE_IOCTL_MIN;
      default:
         return DSS_IFACE_IOCTL_MIN; /* bad val */
   }
}
#endif /* #if defined(RLP3_CFG_SUPPORTED) */

int16 OEMNet_GetRLP3Cfg
(
   OEMCONTEXT  netd,
   int16       nOptName,
   AEERLP3Cfg *prlp3
)
{
#if defined(RLP3_CFG_SUPPORTED)
   OEMNet* pNet = (OEMNet*)netd;
   int ret;
   dss_iface_ioctl_type ioctl_name;
   dss_iface_ioctl_707_rlp_opt_type optval;
   sint15 ndssErr;

   ZEROAT(&optval);
   optval.rscb_index = DSRLP_FIXED_SR_ID_INDEX;

   ioctl_name = AEEToDSSIFaceIOCtl_IOCtlName(nOptName, TRUE);
   if (DSS_IFACE_IOCTL_MIN == ioctl_name) {
      return AEE_NET_EINVAL;
   }

   ret = OEMNet_Iface_Ioctl(pNet,
                            ioctl_name,
                            &optval,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   DSSIFaceIOCtlToAEE_RLP3Cfg(&optval,prlp3);
   return DSS_SUCCESS;
#endif /* if defined(RLP3_CFG_SUPPORTED) */
   return EUNSUPPORTED;
}

int16 OEMNet_SetRLP3Cfg
(
   OEMCONTEXT        netd,
   int16             nOptName,
   const AEERLP3Cfg *prlp3
)
{
#if defined(RLP3_CFG_SUPPORTED)
   OEMNet* pNet = (OEMNet*)netd;
   int ret;
   dss_iface_ioctl_type ioctl_name;
   dss_iface_ioctl_707_rlp_opt_type optval;
   sint15 ndssErr;

   AEEToDSSIFaceIOCtl_RLP3Cfg(prlp3,&optval);

   ioctl_name = AEEToDSSIFaceIOCtl_IOCtlName(nOptName, FALSE);
   if (DSS_IFACE_IOCTL_MIN == ioctl_name) {
      return AEE_NET_EINVAL;
   }

   ret = OEMNet_Iface_Ioctl(pNet,
                            ioctl_name,
                            &optval,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return DSS_SUCCESS;
#endif /* if defined(RLP3_CFG_SUPPORTED) */
   return EUNSUPPORTED;
}


 /*===========================================================================

                      OEM SOCKET FUNCTION DEFINITIONS

 ===========================================================================*/


/*===========================================================================

FUNCTION OEMSocket_New

DESCRIPTION
 Allocate a socket related data structure and remember it

===========================================================================*/
static OEMSocket* OEMSocket_New(OEMNet* pNet)
{
   OEMSocket* me;

   me = sys_malloc(sizeof(OEMSocket));
   if ((OEMSocket*)0 == me) {
      return (OEMSocket*)0;
   }

   // insert at front of linked list
   me->pNext = pNet->pSockets;
   pNet->pSockets = me;

   me->pNet = pNet;
   me->nEventMask = 0;

   // init SDB Ack info
   me->asaInfo.bOverFlow   = FALSE;
   me->asaInfo.nStatus     = AEE_SDB_ACK_NONE;
   return me;
}

/*===========================================================================

FUNCTION OEMSocket_Delete

DESCRIPTION
 Forget and free a socket related data structure

===========================================================================*/
static void OEMSocket_Delete(OEMSocket* me)
{
   OEMNet* pNet = me->pNet;
   OEMSocket* ps;
   OEMSocket* pPrev;

   // remove from linked list
   pPrev = (OEMSocket*)0;
   for (ps = pNet->pSockets; ps; ps = ps->pNext) {

      if (me == ps) {
         if ((OEMSocket*)0 == pPrev) {
            pNet->pSockets = ps->pNext;
         } else {
            pPrev->pNext = ps->pNext;
         }
         break;
      }

      pPrev = ps;
   }

   sys_free(ps);
}

/*===========================================================================

Function:  AEE2DS_SockAddrStorage

Description:
   Convert a socket address from AEE format into DS format

Prototype:

Parameters:

Return Value:
   size of the resulting DS socket address or 0 on failure.

Comments:  None

Side Effects: None

===========================================================================*/
static uint16 AEE2DS_SockAddrStorage(AEESockAddrStorage* pAEE_SAS,
                                     struct sockaddr_storage* pDS_SAS)
{
   struct sockaddr_in* pSockaddr_in;
   uint16 addrlen;

   switch (pAEE_SAS->wFamily)
   {
      case AEE_AF_INET:
         pSockaddr_in = (struct sockaddr_in*)pDS_SAS;
         pSockaddr_in->sin_family = AF_INET;
         pSockaddr_in->sin_addr.s_addr = pAEE_SAS->inet.addr;
         pSockaddr_in->sin_port = pAEE_SAS->inet.port;
         addrlen = sizeof(struct sockaddr_in);
         break;
#ifdef FEATURE_DATA_PS_IPV6
      case AEE_AF_INET6:
      {
         struct sockaddr_in6* pSockaddr_in6 = (struct sockaddr_in6*)pDS_SAS;

         pSockaddr_in6->sin6_family = AF_INET6;
         memcpy(pSockaddr_in6->sin6_addr.s6_addr,
                pAEE_SAS->inet6.addr.Byte,
                sizeof(pSockaddr_in6->sin6_addr.s6_addr));
         pSockaddr_in6->sin6_port = pAEE_SAS->inet6.port;
         pSockaddr_in6->sin6_flowinfo = pAEE_SAS->inet6.dwFlowinfo;
         pSockaddr_in6->sin6_scope_id = pAEE_SAS->inet6.dwScopeId;
         addrlen = sizeof(struct sockaddr_in6);
         break;
      }
#endif // FEATURE_DATA_PS_IPV6
      default:
         ASSERT_FARF(!"AEE2DS_SockAddrStorage: bad addr family!");
         return 0;
   }

   return addrlen;
}

/*===========================================================================

Function:  DS2AEE_SockAddrStorage

Description:
   Convert a socket address from DS format into AEE format

Prototype:

Parameters:

Return Value:
   size of the resulting AEE socket address or 0 on failure.

Comments:  None

Side Effects: None

===========================================================================*/
static int DS2AEE_SockAddrStorage(struct sockaddr_storage* pDS_SAS,
                                  AEESockAddrStorage* pAEE_SAS, int nAEE_SASLen)
{
   struct sockaddr_in* pSockaddr_in;
   int addrlen;

#if !defined(AEE_SIMULATOR) && !defined(FEATURE_DATA_PS_IPV6) && defined(DSS_VERSION) && (DSS_VERSION >= 30)

   struct sockaddr_in6* pSockaddr_in6 = (struct sockaddr_in6*)pDS_SAS;

   // fix to avoid AMSS issue with zero address (AMSS always returns this as an IPv6 address)
   if ( (AF_INET6 == pDS_SAS->ss_family) &&
        (IN6_IS_ADDR_UNSPECIFIED(pSockaddr_in6->sin6_addr.s6_addr)) ) {
      pAEE_SAS->wFamily = AEE_AF_INET;
      pAEE_SAS->inet.addr = INADDR_ANY;
      pAEE_SAS->inet.port = pSockaddr_in6->sin6_port;
      return sizeof(AEESockAddrInet);
   }
#endif // !defined(AEE_SIMULATOR) && !defined(FEATURE_DATA_PS_IPV6) && defined(DSS_VERSION) && (DSS_VERSION >= 30)

   switch (pDS_SAS->ss_family)
   {
      case AF_INET:
         if (nAEE_SASLen < sizeof(AEESockAddrInet)) {
            return 0;
         }
         pSockaddr_in = (struct sockaddr_in*)pDS_SAS;
         pAEE_SAS->wFamily = AEE_AF_INET;
         pAEE_SAS->inet.addr = pSockaddr_in->sin_addr.s_addr;
         pAEE_SAS->inet.port = pSockaddr_in->sin_port;
         addrlen = sizeof(AEESockAddrInet);
         break;

#ifdef FEATURE_DATA_PS_IPV6
      case AF_INET6:
      {
         struct sockaddr_in6* pSockaddr_in6;
         if (nAEE_SASLen < sizeof(AEESockAddrInet6)) {
            return 0;
         }
         pSockaddr_in6 = (struct sockaddr_in6*)pDS_SAS;
         pAEE_SAS->wFamily = AEE_AF_INET6;
         memcpy(pAEE_SAS->inet6.addr.Byte, pSockaddr_in6->sin6_addr.s6_addr,
                sizeof(pSockaddr_in6->sin6_addr.s6_addr));
         pAEE_SAS->inet6.port = pSockaddr_in6->sin6_port;
         pAEE_SAS->inet6.dwFlowinfo = pSockaddr_in6->sin6_flowinfo;
         pAEE_SAS->inet6.dwScopeId = pSockaddr_in6->sin6_scope_id;
         addrlen = sizeof(AEESockAddrInet6);
         break;
      }
#endif // FEATURE_DATA_PS_IPV6

      default:
         ASSERT_FARF(!"DS2AEE_SockAddrStorage: bad addr family!");
         return 0;
   }

   return addrlen;
}

/*===========================================================================
  called by socket I/O methods
  ===========================================================================*/
static __inline void OEMSocket_FakeWaking(OEMSocket* me, int16 ndssErr)
{
   NetState ns;

   // If the I/O operation did not block, we are most likely already up
   if (DS_EWOULDBLOCK != ndssErr) {
      return;
   }

   // Are we waking, either real or fake?
   ns = OEMNet_PPPState(me->pNet);
   switch (ns) {
      case NET_PPP_WAKING:      // real waking
         break;

      case NET_PPP_ASLEEP:      // fake waking
         FARF(OEM_NETSTATE,("FAKE_PPP_WAKING"));
         if (TRUE != me->bLoopback) {
            me->pNet->PPPState.nFakePhysLinkState = NET_FAKE_COMING_UP;
         }
         break;

      default:                  // not waking
         return;
   }

   AEE_SYS_RESUME_IF(me->pNet->pcbNetWaiter);
}

#if defined(DSS_FAKE_CLOSE)
// The short term timer has expired, so fake
// a close event to get around the DMSS "not
// always sending close events" bug
static void OEMSocket_FakeClose(void *pv)
{
   OEMSocket* me = (OEMSocket*)pv;
   OEMNet* pNet = me->pNet;

   me->bFakeCloseEvent = TRUE;

   if (OEMNet_IsSocketInterceptorInstalled(pNet)) {
      OEMNet_InterceptedSocketEventOccurred(pNet);
   } else {
      OEMNet_NonInterceptedSocketEventOccurred(pNet);
   }
}
#endif // defined(DSS_FAKE_CLOSE)

/*===========================================================================

FUNCTION OEMSocket_Open

DESCRIPTION
 Create a socket and related data structures, and return a socket descriptor.

===========================================================================*/
OEMCONTEXT OEMSocket_Open
(
   OEMCONTEXT  netd,
   uint16      wFamily,
   AEESockType type,
   int         protocol,
   int16      *err
)
{
   OEMNet* pNet = (OEMNet*)netd;
   int16 sockfd;
   OEMSocket* me;
   int16 ndssErr;
   byte dmss_type;
   byte dmss_proto;
   byte dmss_af;

   switch (wFamily) {
      case AEE_AF_INET:
         dmss_af = AF_INET;
         break;
      case AEE_AF_INET6:
         dmss_af = AF_INET6;
         break;
      default:
         *err = AEE_NET_EAFNOSUPPORT;
         return (OEMCONTEXT)AEE_NET_ERROR;
   }

   switch (protocol) {
      case AEE_IPPROTO_TCP:
         dmss_proto = IPPROTO_TCP;
         break;

      case AEE_IPPROTO_UDP:
         dmss_proto = IPPROTO_UDP;
         break;

      default:
         *err = AEE_NET_EPROTONOSUPPORT;
         return (OEMCONTEXT)AEE_NET_ERROR;
   }

   switch (type) {
      case AEE_SOCKPORT_STREAM:
         dmss_type = SOCK_STREAM;
         break;

      case AEE_SOCKPORT_DGRAM:
         dmss_type = SOCK_DGRAM;
         break;

      default:
         *err = AEE_NET_ESOCKNOSUPPORT;
         return (OEMCONTEXT)AEE_NET_ERROR;
   }

   me = OEMSocket_New(pNet);
   if ((OEMSocket*)0 == me) {
      *err = AEE_NET_EMFILE;
      return (OEMCONTEXT)AEE_NET_ERROR;
   }

   sockfd = dss_socket(pNet->sAppID, dmss_af, dmss_type, dmss_proto, &ndssErr);
   if (DSS_ERROR == sockfd) {
      OEMSocket_Delete(me);
      *err = DSSToAEE_Err(ndssErr);
      return (OEMCONTEXT)AEE_NET_ERROR;
   } else {
      *err = AEE_NET_SUCCESS;
   }

   me->sockfd = sockfd;
   if (AEE_SOCKPORT_STREAM == type) {
      me->bStream = TRUE;
   }

#if defined(DSS_FAKE_CLOSE)
   CALLBACK_Init(&me->cbFakeClose, OEMSocket_FakeClose, me);
#endif // defined(DSS_FAKE_CLOSE)

   return (OEMCONTEXT)me;
}

/*===========================================================================g

FUNCTION OEMSocket_Connect

DESCRIPTION
  For TCP, attempts to establish the TCP connection.  Upon
  successful connection, calls the socket callback function asserting that
  the DS_WRITE_EVENT is TRUE.  The function must receive
  (as a parameter) a valid socket descriptor, implying a previous successful
  call to dss_socket().


===========================================================================*/
int16 OEMSocket_Connect
(
   OEMCONTEXT            sockd,        /* Socket descriptor */
   const void           *pAddr         /* destination address */
)
{
   OEMSocket*          me = (OEMSocket*)sockd;
   AEESockAddrStorage* pasasDest = (AEESockAddrStorage*)pAddr;
   int16               ndssErr;
   struct sockaddr_storage sasDest;   /* destination address */
   uint16               addrlen;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if (AEE_IS_ADDR_LOOPBACK(pasasDest)) {
#if defined(OEM_PROVIDES_LOOPBACK)
      me->bLoopback = TRUE;
#else
      return AEE_NET_EOPNOTSUPP;
#endif
   }

   addrlen = AEE2DS_SockAddrStorage(pasasDest, &sasDest);
   if (addrlen == 0) {
      return AEE_NET_ERROR;
   }

   if (DSS_SUCCESS !=
       dss_connect(me->sockfd, (struct sockaddr *) &sasDest,
                   addrlen, &ndssErr)) {
      OEMSocket_FakeWaking(me, ndssErr);
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_Bind

DESCRIPTION
  For all client sockets, attaches a local address and port value to the
  socket.  If the call is not explicitly issued, the socket will implicitly
  bind in calls to dss_connect() or dss_sendto().  Note that this function
  does not support binding a local IP address, but rather ONLY a local port
  number.  The local IP address is assigned automatically by the sockets
  library.  The function must receive (as a parameter) a valid socket
  descriptor, implying a previous successful call to dss_socket().

===========================================================================*/
int16 OEMSocket_Bind
(
   OEMCONTEXT            sockd,            /* socket descriptor */
   const void           *pAddr
)
{
   OEMSocket*          me = (OEMSocket*)sockd;
   AEESockAddrStorage* pasasLocal = (AEESockAddrStorage*)pAddr;
   int16               ndssErr;
   struct sockaddr_storage sasLocal;
   uint16               addrlen;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // These bugs are fixed in newer versions of DMSS
   // that support listen and accept...
#if !defined(OEM_PROVIDES_SERVER_SOCKETS)
   // we don't support bind with TCP sockets,
   // dsssocket crashes if we try to bind(), then close()
   if (me->bStream) {
      return AEE_NET_EOPNOTSUPP;
   }

   // wildcard port not supported
   if (0 == GET_PORT(pasasLocal)) {
      return AEE_NET_EOPNOTSUPP;
   }
#endif // !defined(OEM_PROVIDES_SERVER_SOCKETS)

   addrlen = AEE2DS_SockAddrStorage(pasasLocal, &sasLocal);
   if (addrlen == 0) {
      return AEE_NET_ERROR;
   }

   if (DSS_SUCCESS !=
       dss_bind( me->sockfd, (struct sockaddr *) &sasLocal,
                 addrlen, &ndssErr)) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_Close

DESCRIPTION
  Non-blocking close of a socket.  Performs all necessary clean-up of data
  structures and frees the socket for re-use.  For TCP initiates the active
  close for connection termination.  Once TCP has closed, the DS_CLOSE_EVENT
  will become TRUE, and the application can call dss_close() again to free
  the socket for re-use.  UDP sockets also need to call this to
  clean-up the socket and free it for re-use.

===========================================================================*/
int16 OEMSocket_Close
(
   OEMCONTEXT        sockd                /* socket descriptor */
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16  ndssErr;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if (DSS_SUCCESS != dss_close(me->sockfd, &ndssErr)) {
      int16 err = DSSToAEE_Err(ndssErr);

#if defined(DSS_FAKE_CLOSE)
      // 1/25/02 - DMSS Issue - Need to set a short-term timer here if
      //  the close did not complete.  If we dont we may/will have cases
      // where sockets sit out there "forever" without us being notified
      // that they moved to closed.
      if (AEE_NET_EWOULDBLOCK == err) {
         AEE_SetSysTimerCallback(500, &me->cbFakeClose);
      }
#endif // defined(DSS_FAKE_CLOSE)

      return err;
   }

#if defined(DSS_FAKE_CLOSE)
   // If bFakeCloseEvent is set, then the timer has fired but the fake close
   // event has not yet been generated, so just unset bFakeCloseEvent.
   // Else if the timer is registered but not yet fired, cancel the timer.
   if (me->bFakeCloseEvent) {
      me->bFakeCloseEvent = FALSE;
   } else if (CALLBACK_IsQueued(&me->cbFakeClose)) {
      CALLBACK_Cancel(&me->cbFakeClose);
   }
#endif // defined(DSS_FAKE_CLOSE)

   OEMSocket_Delete(me);

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_AsyncSelect

DESCRIPTION
  Enables the events to be notified about through the asynchronous
  notification mechanism.  Application specifies a bitmask of events that it
  is interested in, for which it will receive asynchronous notification via
  its application callback function.  This function also performs a real-time
  check to determine if any of the events have already occurred, and if so
  invokes the application callback.

===========================================================================*/
int16 OEMSocket_AsyncSelect
(
   OEMCONTEXT            sockd,              /* socket descriptor */
   int32                 interest_mask       /* bitmask of events to set */
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16      ndssErr;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // mutate READ into ACCEPT for listening sockets
   if (me->bListen &&
       interest_mask & DS_READ_EVENT) {
      interest_mask &= ~DS_READ_EVENT;
      interest_mask |= DS_ACCEPT_EVENT;
   }

   if (DSS_SUCCESS !=
       dss_async_select( me->sockfd, interest_mask, &ndssErr)) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_GetNextEvent

DESCRIPTION
  This function performs a real-time check to determine if any of the events
  of interest specified in the socket control block's event mask have
  occurred.  It also clears any bits in the event mask that have occurred.
  The application must re-enable these events through a subsequent call to
  dss_async_select().  The application passes in a pointer to a single
  socket descriptor to determine if any events have occurred for that socket.

 ===========================================================================*/
int32 OEMSocket_GetNextEvent
(
   OEMCONTEXT            *psockd,                /* socket descriptor */
   int16                 *err
)
{
   OEMSocket  *ps;
   int16       ndssErr = DSS_SUCCESS;
   int32       nRet = 0;

   ps = (OEMSocket *)*psockd;

   if ( ((OEMSocket *)AEE_NET_ERROR == ps) || (NULL == ps) ) {
      return AEE_NET_EBADF;
   }
#if defined(FEATURE_CLOSE_TCP_SOCKETS_ON_NETWORK_CLOSE)
   if (DSS_ERROR == ps->sockfd) {
      *err = DSSToAEE_Err(ndssErr);
      return DS_CLOSE_EVENT;
   }
#endif // defined(FEATURE_CLOSE_TCP_SOCKETS_ON_NETWORK_CLOSE)

   // Reset event mask and get the old mask
   nRet = OEMSocket_SetEventMask(ps, 0);

   // mutate ACCEPT into READ for listening sockets
   if (ps->bListen && nRet & DS_ACCEPT_EVENT) {
      nRet &= ~DS_ACCEPT_EVENT;
      nRet |= DS_READ_EVENT;
   }

#if defined(DSS_FAKE_CLOSE)
   // fake a close event?
   if (ps->bFakeCloseEvent) {
      ps->bFakeCloseEvent = FALSE;
      nRet |= DS_CLOSE_EVENT;
   }
#endif // defined(DSS_FAKE_CLOSE)

   *err = DSSToAEE_Err(ndssErr);
   return nRet;
}

/*===========================================================================

FUNCTION OEMSocket_Write

DESCRIPTION
  Sends specified number of bytes in the buffer over the TCP transport.

===========================================================================*/
int32 OEMSocket_Write
(
   OEMCONTEXT         sockd,      /* socket descriptor */
   const byte        *buffer,     /* user buffer from which to copy data */
   uint32             nbytes,     /* number of bytes to be written to socket */
   int16             *err         /* error condition value */
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16      Ret;
   int16      ndssErr = DSS_SUCCESS;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return AEE_NET_ERROR;
   }

   // are we shutdown?
   if ((AEE_SHUTDOWN_WR+1) & me->nShutdown) {
      *err = AEE_NET_ESHUTDOWN;
      return AEE_NET_ERROR;
   }

   Ret = dss_write(me->sockfd, (byte*)buffer, (uint16)MIN(MAX_UINT16,nbytes), &ndssErr);

   /* dss returns DSS_SUCCESS on EOF!! luckily, ndssErr is set to EEOF */
   if (Ret == 0 && nbytes != 0) {
      Ret = AEE_NET_ERROR;
   }

   OEMSocket_FakeWaking(me, ndssErr);
   *err = DSSToAEE_Err(ndssErr);
   return Ret;
}

/*===========================================================================

FUNCTION OEMSocket_Writev

DESCRIPTION
  Provides the gather write variant of the dss_write() call, which
  allows the application to write from non-contiguous buffers.    Sends
  specified number of bytes in the buffer over the TCP transport.

===========================================================================*/
int32 OEMSocket_Writev
(
   OEMCONTEXT            sockd,          /* socket descriptor */
   const SockIOBlock     iov[],          /* array of data buffers from which to copy data */
   uint16                iovcount,       /* number of array items */
   int16                 *err
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16      Ret;
   int16      ndssErr = DSS_SUCCESS;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return AEE_NET_ERROR;
   }

   // are we shutdown?
   if ((AEE_SHUTDOWN_WR+1) & me->nShutdown) {
      *err = AEE_NET_ESHUTDOWN;
      return AEE_NET_ERROR;
   }

   {
      uint16 i;

      for (i = 0; i < iovcount; i++) {
         if (iov[i].wLen != 0) {
            break;
         }
      }

      Ret = dss_writev(me->sockfd, (struct iovec*)iov, iovcount, &ndssErr);

      /* dss returns DSS_SUCCESS on EOF!! luckily, ndssErr is set to EEOF */
      if (Ret == 0 && i != iovcount) {
         Ret = AEE_NET_ERROR;
      }
   }

   OEMSocket_FakeWaking(me, ndssErr);
   *err = DSSToAEE_Err(ndssErr);
   return Ret;
}

/*===========================================================================

FUNCTION OEMSocket_Read

DESCRIPTION
  Reads specified number of bytes into buffer from the TCP transport.

===========================================================================*/
int32 OEMSocket_Read
(
   OEMCONTEXT            sockd,      /* socket descriptor */
   byte                 *buffer,     /* user buffer to which to copy data */
   uint32                nbytes,     /* number of bytes to be read from socket */
   int16                 *err
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16      ndssErr = DSS_SUCCESS;
   int32      Ret;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return AEE_NET_ERROR;
   }

   // are we shutdown?
   if ((AEE_SHUTDOWN_RD+1) & me->nShutdown) {
      *err = AEE_NET_ESHUTDOWN;
      return AEE_NET_ERROR;
   }

   Ret = dss_read(me->sockfd, buffer, (uint16)MIN(MAX_UINT16,nbytes), &ndssErr);

   *err = DSSToAEE_Err(ndssErr);
   return Ret;
}

/*===========================================================================

FUNCTION OEMSocket_Readv

DESCRIPTION
  Provides the scatter read variant of the dss_read() call, which
  allows the application to read into non-contiguous buffers.    Reads
  specified number of bytes into the buffer from the TCP transport.

===========================================================================*/
int32 OEMSocket_Readv
(
   OEMCONTEXT            sockd,     /* socket descriptor */
   SockIOBlock           iov[],     /* array of data buffers to copy data into */
   uint16                iovcount,  /* number of array items */
   int16                 *err
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int32      Ret;
   int16      ndssErr = DSS_SUCCESS;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return AEE_NET_ERROR;
   }

   // are we shutdown?
   if ((AEE_SHUTDOWN_RD+1) & me->nShutdown) {
      *err = AEE_NET_ESHUTDOWN;
      return AEE_NET_ERROR;
   }

   Ret = dss_readv(me->sockfd, (struct iovec*)iov, iovcount, &ndssErr);

   *err = DSSToAEE_Err(ndssErr);
   return Ret;
}

/*===========================================================================

FUNCTION OEMSocket_SendTo

DESCRIPTION
  Sends 'nbytes' bytes in the buffer over the UDP transport.
===========================================================================*/
int32 OEMSocket_SendTo
(
   OEMCONTEXT            sockd,     /* socket descriptor */
   const byte           *buffer,    /* user buffer from which to copy the data */
   uint32                nbytes,    /* number of bytes to be written */
   uint32                flags,     /* data transport options */
   const void           *pAddr,
   int16                 *err
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   AEESockAddrStorage* pasasToAddr = (AEESockAddrStorage*)pAddr;
   struct sockaddr_storage sasToAddr;  /* destination address */
   int32 Ret;
   int16 ndssErr = DSS_SUCCESS;
   uint32 ds_flags = 0;
   uint16 addrlen;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return AEE_NET_ERROR;
   }

   if (AEE_IS_ADDR_LOOPBACK(pasasToAddr)) {
#if defined(OEM_PROVIDES_LOOPBACK)
      me->bLoopback = TRUE;
#else
      *err = AEE_NET_EOPNOTSUPP;
      return AEE_NET_ERROR;
#endif
   }

   if (flags & ISOCKET_FLAG_SENDTO_URGENT) {
      // Request use of short data burst over enhanced access channel
      ds_flags |= MSG_FAST_EXPEDITE;
      flags &= ~ISOCKET_FLAG_SENDTO_URGENT;
   }

   // Should have translated all flags across so anything left is unsupported
   if (flags) {
      *err = AEE_NET_EOPNOTSUPP;
      return AEE_NET_ERROR;
   }

   addrlen = AEE2DS_SockAddrStorage(pasasToAddr, &sasToAddr);
   if (addrlen == 0) {
      *err = AEE_NET_EAFNOSUPPORT;
      return AEE_NET_ERROR;
   }

   Ret = dss_sendto(me->sockfd, (byte*)buffer, (uint16)MIN(MAX_UINT16,nbytes), ds_flags,
                    (struct sockaddr*)&sasToAddr, addrlen, &ndssErr);

   OEMSocket_FakeWaking(me, ndssErr);
   *err = DSSToAEE_Err(ndssErr);
   return Ret;
}

/*===========================================================================

FUNCTION OEMSocket_RecvFrom

DESCRIPTION
  Reads 'nbytes' bytes in the buffer from the UDP transport.  Fills in
  address structure with values from who sent the data.
===========================================================================*/
int32 OEMSocket_RecvFrom
(
   OEMCONTEXT        sockd,                         /* socket descriptor */
   byte             *buffer,  /* user buffer from which to copy the data */
   uint32            nbytes,            /* number of bytes to be written */
   uint32            flags,             /* unused */
   void             *pAddr,
   int              *pAddrLen,
   int16            *err
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   AEESockAddrStorage* pasasFrom = (AEESockAddrStorage*)pAddr;
   struct sockaddr_storage sasFrom;
   uint16 fromaddr_size = sizeof(sasFrom);
   int32 Ret;
   int16 ndssErr = DSS_SUCCESS;

   memset(&sasFrom, 0, sizeof(sasFrom));
   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return AEE_NET_ERROR;
   }

   Ret = dss_recvfrom(me->sockfd, buffer, (uint16)MIN(MAX_UINT16,nbytes), flags,
                      (struct sockaddr*)&sasFrom, &fromaddr_size, &ndssErr);

   *err = DSSToAEE_Err(ndssErr);
   if (Ret == DSS_ERROR) {
      return Ret;
   }

   *pAddrLen = DS2AEE_SockAddrStorage(&sasFrom, pasasFrom, MIN(*pAddrLen, fromaddr_size));
   if (*pAddrLen == 0) {
      *err = AEE_NET_EFAULT;
      return AEE_NET_ERROR;
   }

   return Ret;
}

/*===========================================================================

FUNCTION OEMSocket_GetSockName

DESCRIPTION
  Returns the local IP and port of a socket.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMSocket_GetSockName
(
   OEMCONTEXT            sockd,                       /* Socket descriptor */
   void                 *pAddr,
   int                  *pAddrLen
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   AEESockAddrStorage* pasasLocal = (AEESockAddrStorage*)pAddr;
   struct sockaddr_storage sasLocal;
   uint16 localaddr_size = sizeof(sasLocal);
   int16 ndssErr;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if (DSS_SUCCESS !=
       dss_getsockname(me->sockfd,(struct sockaddr *)&sasLocal,
                       &localaddr_size,&ndssErr)) {
      return DSSToAEE_Err(ndssErr);
   }

   *pAddrLen = DS2AEE_SockAddrStorage(&sasLocal, pasasLocal, MIN(*pAddrLen, localaddr_size));
   if (*pAddrLen == 0) {
      return AEE_NET_ERROR;
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_GetPeerName

DESCRIPTION
  Returns the IP and port of a connected peer.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMSocket_GetPeerName
(
   OEMCONTEXT            sockd,                       /* Socket descriptor */
   void                 *pAddr,
   int                  *pAddrLen
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   AEESockAddrStorage* pasasRemote = (AEESockAddrStorage*)pAddr;
   struct sockaddr_storage sasRemote;
   uint16 remoteaddr_size = sizeof(sasRemote);
   int16 ndssErr;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if (DSS_SUCCESS !=
       dss_getpeername(me->sockfd,(struct sockaddr *)&sasRemote,
                       &remoteaddr_size,&ndssErr)) {
      return DSSToAEE_Err(ndssErr);
   }

   *pAddrLen = DS2AEE_SockAddrStorage(&sasRemote, pasasRemote, MIN(*pAddrLen, remoteaddr_size));
   if (*pAddrLen == 0) {
      return AEE_NET_ERROR;
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_Listen

DESCRIPTION
  Places a socket in listening mode.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMSocket_Listen
(
   OEMCONTEXT            sockd,                       /* Socket descriptor */
   int16                 backlog
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16 ndssErr;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // CR20046: DMSS preallocates OEM sockets for the entire
   // backlog, so a backlog of 3 (for example) will consume 4 OEM sockets for
   // each listening socket, easily resulting in EMFILE for other
   // sockets/apps on targets with a small number of TCP sockets.
   // Silently cap the backlog to OEMSOCK_MAX_BACKLOG_VALUE
   // to minimize this problem.
   // Given the slower nature of wireless networks, a
   // large backlog is not really all that useful anyhow...

   if (OEMSOCK_MAX_BACKLOG_VALUE < backlog) {
      backlog = OEMSOCK_MAX_BACKLOG_VALUE;
   }

   if (DSS_SUCCESS !=
       dss_listen( me->sockfd, backlog, &ndssErr)) {
      return DSSToAEE_Err(ndssErr);
   }

   me->bListen = TRUE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_Accept

DESCRIPTION
  Returns a newly connected socket.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
OEMCONTEXT OEMSocket_Accept
(
   OEMCONTEXT            sockd,                /* Listen Socket descriptor */
   int16                *err
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16 sockfd;
   struct sockaddr_storage remoteaddr;
   uint16 addrlen = sizeof(remoteaddr);
   int16 ndssErr;
   OEMSocket* ps;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      *err = AEE_NET_EBADF;
      return (OEMCONTEXT)AEE_NET_ERROR;
   }

   ps = OEMSocket_New(me->pNet);
   if ((OEMSocket*)0 == ps) {
      *err = AEE_NET_EMFILE;
      return (OEMCONTEXT)AEE_NET_ERROR;
   }

   sockfd = dss_accept(me->sockfd, (struct sockaddr*)&remoteaddr, &addrlen, &ndssErr);
   if (DSS_ERROR == sockfd) {
      OEMSocket_Delete(ps);
      *err = DSSToAEE_Err(ndssErr);
      return (OEMCONTEXT)AEE_NET_ERROR;
   }

   ps->sockfd = sockfd;

   return ps;
}

/*===========================================================================

FUNCTION OEMSocket_Shutdown

DESCRIPTION
  Performs a half-close on a socket.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMSocket_Shutdown
(
   OEMCONTEXT            sockd,                       /* Socket descriptor */
   int32                 how
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   int16 ndssErr;
   uint16 ndssHow;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // map AEE_SHUT_* to DSS_SHUT_*
   switch (how) {
      case AEE_SHUTDOWN_RD:
         if (me->nShutdown & (AEE_SHUTDOWN_RD+1)) {
            return AEE_NET_ESHUTDOWN;
         }
         ndssHow = DSS_SHUT_RD;
         break;
      case AEE_SHUTDOWN_WR:
         if (me->nShutdown & (AEE_SHUTDOWN_WR+1)) {
            return AEE_NET_ESHUTDOWN;
         }
         ndssHow = DSS_SHUT_WR;
         break;
      case AEE_SHUTDOWN_RDWR:
         if (me->nShutdown & ((AEE_SHUTDOWN_RD+1)|(AEE_SHUTDOWN_WR+1))) {
            return AEE_NET_ESHUTDOWN;
         }
         ndssHow = DSS_SHUT_RDWR;
         break;
      default:
         return AEE_NET_EINVAL;
   }

   if (DSS_SUCCESS !=
       dss_shutdown( me->sockfd, (uint16)ndssHow, &ndssErr)) {
      return DSSToAEE_Err(ndssErr);
   }

   // OK, now we are shutdown
   me->nShutdown |= (how+1);

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMSocket_GetNativeDescriptor

DESCRIPTION
  Returns the underlying socket descriptor

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
int16 OEMSocket_GetNativeDescriptor(
  OEMCONTEXT            sockd,         /* socket descriptor */
  uint32               *pdwDesc
)
{
   OEMSocket* me = (OEMSocket*)sockd;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if ((uint32*)0 == pdwDesc) {
      return AEE_NET_EINVAL;
   }

   *pdwDesc = me->sockfd;

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION OEMSocket_GetSDBAckInfo

DESCRIPTION
  get the last SDB Ack information associated with this socket.

===========================================================================*/
int16 OEMSocket_GetSDBAckInfo(OEMCONTEXT sockd, AEESDBAckInfo* pInfo)
{
   OEMSocket* me = (OEMSocket*)sockd;

   if ((OEMSocket*)AEE_NET_ERROR == me ||
      (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
      }

   if (NULL == pInfo) {
      return EBADPARM;
   }

   *pInfo = me->asaInfo;

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetKeepAlive

DESCRIPTION
  Sets the underlying socket keepalive setting
===========================================================================*/
int16 OEMSocket_SetKeepAlive(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bKeepAlive
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bKeepAlive ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_KEEPALIVE,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetLinger

DESCRIPTION
  Sets the underlying socket linger setting
===========================================================================*/
int16 OEMSocket_SetLinger(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bOn,          // linger on or off
  uint32                dwLinger      // linger time, in seconds
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   dss_so_linger_type dss_so_linger;
   uint32 len = sizeof(dss_so_linger);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   dss_so_linger.l_onoff = (int)bOn;
   dss_so_linger.l_linger = 1000*dwLinger; // milliseconds

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_LINGER,
                        &dss_so_linger,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetRcvBuf

DESCRIPTION
  Sets the underlying socket receive buffer size
===========================================================================*/
int16 OEMSocket_SetRcvBuf(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                   nRcvBuf
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(nRcvBuf);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // check for valid range of receive buffer
   if ( nRcvBuf < DSS_MIN_RCVBUF)
   {
      DBGPRINTF("OEMSocket_SetRcvBuf change buffer size from %d to %d",
                nRcvBuf, DSS_MIN_RCVBUF);
      nRcvBuf = DSS_MIN_RCVBUF;
   }
   else if ( nRcvBuf > DSS_MAX_RCVBUF)
   {
      DBGPRINTF("OEMSocket_SetRcvBuf change buffer size from %d to %d",
                nRcvBuf, DSS_MAX_RCVBUF);

      nRcvBuf = DSS_MAX_RCVBUF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_RCVBUF,
                        &nRcvBuf,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetSndBuf

DESCRIPTION
  Sets the underlying socket send buffer size
===========================================================================*/
int16 OEMSocket_SetSndBuf(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                   nSndBuf
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(nSndBuf);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // check for valid range of send buffer
   if ( nSndBuf < DSS_MIN_SNDBUF)
   {
      DBGPRINTF("OEMSocket_SetSndBuf change buffer size from %d to %d",
                nSndBuf, DSS_MIN_SNDBUF);
      nSndBuf = DSS_MIN_SNDBUF;
   }
   else if ( nSndBuf > DSS_MAX_SNDBUF)
   {
      DBGPRINTF("OEMSocket_SetSndBuf change buffer size from %d to %d",
                nSndBuf, DSS_MAX_SNDBUF);

      nSndBuf = DSS_MAX_SNDBUF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_SNDBUF,
                        &nSndBuf,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetNoDelay

DESCRIPTION
  Sets the underlying socket Nagle delay setting
===========================================================================*/
int16 OEMSocket_SetNoDelay(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bNoDelay
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bNoDelay ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_NODELAY,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetDelayedAck

DESCRIPTION
  Sets the underlying socket Delayed Ack setting
===========================================================================*/
int16 OEMSocket_SetDelayedAck(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bDelayedAck
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bDelayedAck ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_DELAYED_ACK,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetSACK

DESCRIPTION
  Sets the underlying socket SACK setting
===========================================================================*/
int16 OEMSocket_SetSACK(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bSACK
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bSACK ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_SACK,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetTimeStamp

DESCRIPTION
  Sets the underlying socket TCP time stamp setting
===========================================================================*/
int16 OEMSocket_SetTimeStamp(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bTimeStamp
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bTimeStamp ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_TIME_STAMP,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetMembership

DESCRIPTION
  Sets the underlying socket membership setting
===========================================================================*/
int16 OEMSocket_SetMembership(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean               bAdd,          // Add or Drop membership
  IPAddr               *pMCast         // multicast group to join/leave
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   struct in_addr mcast_addr;
   uint32 len = sizeof(mcast_addr);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if (pMCast->wFamily != AEE_AF_INET) {
      return AEE_NET_EAFNOSUPPORT;
   }

   mcast_addr.s_addr = pMCast->addr.v4;

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_IP,
                        bAdd ? DSS_BCMCS_JOIN : DSS_BCMCS_LEAVE,
                        &mcast_addr,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetIPToS

DESCRIPTION
   Sets the socket's IP Type of Service.
===========================================================================*/
int16 OEMSocket_SetIPToS(OEMCONTEXT sockd, /* socket descriptor */
                         int nIPToS
                        )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(nIPToS);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_IP,
                        DSS_IP_TOS,
                        &nIPToS,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetIPv6TrafficClass

DESCRIPTION:
   Set the socket's Traffic Class for outgoing IPv6 datagrams.
===========================================================================*/
int16 OEMSocket_SetIPv6TrafficClass(OEMCONTEXT sockd, /* socket descriptor */
                                    int nIPv6TClass
                                   )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(nIPv6TClass);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_IPV6,
                        DSS_IPV6_TCLASS,
                        &nIPv6TClass,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetIPTTL

DESCRIPTION:
   Sets the socket's Time To Live for outgoing IP packets.
===========================================================================*/
int16 OEMSocket_SetIPTTL(OEMCONTEXT sockd, /* socket descriptor */
                         int nIPTTL
                        )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(nIPTTL);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_IP,
                        DSS_IP_TTL,
                        &nIPTTL,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetTCPMaxSegmentSize

DESCRIPTION:
   Sets the socket's maximum segment size for outgoing TCP packets.
===========================================================================*/
int16 OEMSocket_SetTCPMaxSegmentSize(OEMCONTEXT sockd, /* socket descriptor */
                                     int nMaxSeg
                                    )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(nMaxSeg);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_MAXSEG,
                        &nMaxSeg,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetReuseAddr

DESCRIPTION
   Allows sockets to reuse a pair of address and port number that is already bound by
   another socket.
===========================================================================*/
int16 OEMSocket_SetReuseAddr(OEMCONTEXT sockd, /* socket descriptor */
                             boolean bReuseAddr
                            )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bReuseAddr ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_REUSEADDR,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_SetDisableFlowFwding

DESCRIPTION
  Disables routing data on best effort flow when QoS is not available
===========================================================================*/
int16 OEMSocket_SetDisableFlowFwding(OEMCONTEXT sockd, /* socket descriptor */
                                     boolean bDisableFlowFwding
                                    )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag = bDisableFlowFwding ? 1 : 0;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_DISABLE_FLOW_FWDING,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_DSSToAEESDBAckStatus

DESCRIPTION
  Convert an SDB-Ack status from DSS terms to AEE terms.
===========================================================================*/
static int OEMSocket_DSSToAEESDBAckStatus(const dss_sdb_ack_status_enum_type dss_sdb_status,
                                          AEESDBAckStatus* pStatus)
{
   int nRet = AEE_NET_SUCCESS;

   switch (dss_sdb_status)
   {
      case DSS_SDB_ACK_NONE:
         *pStatus = AEE_SDB_ACK_NONE;
         break;
      case DSS_SDB_ACK_OK:
         *pStatus = AEE_SDB_ACK_OK;
         break;
      case DSS_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT:
         *pStatus = AEE_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT;
         break;
      case DSS_SDB_ACK_HOLD_ORIG:
         *pStatus = AEE_SDB_ACK_HOLD_ORIG;
         break;
      case DSS_SDB_ACK_NO_SRV:
         *pStatus = AEE_SDB_ACK_NO_SRV;
         break;
      case DSS_SDB_ACK_ABORT:
         *pStatus = AEE_SDB_ACK_ABORT;
         break;
      case DSS_SDB_ACK_NOT_ALLOWED_IN_AMPS:
         *pStatus = AEE_SDB_ACK_NOT_ALLOWED_IN_AMPS;
         break;
      case DSS_SDB_ACK_NOT_ALLOWED_IN_HDR:
         *pStatus = AEE_SDB_ACK_NOT_ALLOWED_IN_HDR;
         break;
      case DSS_SDB_ACK_L2_ACK_FAILURE:
         *pStatus = AEE_SDB_ACK_L2_ACK_FAILURE;
         break;
      case DSS_SDB_ACK_OUT_OF_RESOURCES:
         *pStatus = AEE_SDB_ACK_OUT_OF_RESOURCES;
         break;
      case DSS_SDB_ACK_ACCESS_TOO_LARGE:
         *pStatus = AEE_SDB_ACK_ACCESS_TOO_LARGE;
         break;
      case DSS_SDB_ACK_DTC_TOO_LARGE:
         *pStatus = AEE_SDB_ACK_DTC_TOO_LARGE;
         break;
      case DSS_SDB_ACK_OTHER:
         *pStatus = AEE_SDB_ACK_OTHER;
         break;
      case DSS_SDB_ACK_ACCT_BLOCK:
         *pStatus = AEE_SDB_ACK_ACCT_BLOCK;
         break;
      case DSS_SDB_ACK_L3_ACK_FAILURE:
         *pStatus = AEE_SDB_ACK_L3_ACK_FAILURE;
         break;

      default:
         nRet = AEE_NET_ERROR;
         //FARF(ALWAYS, ("Invalid DSS SDB Ack Status: %d", dss_sdb_status));
   }

   return nRet;
}

/*===========================================================================
FUNCTION: OEMSocket_SDBAckCB

DESCRIPTION
  Callback to be invoked when receiving an SDB Ack. The OEM registers this
  callback in OEMSocket_SetSDBAckCB().
===========================================================================*/
void OEMSocket_SDBAckCB(sint15 sockfd, dss_sdb_ack_status_info_type* sdb_ack_info, void* user_data)
{
   int nRet;
   OEMSDBAckUserData* pUserData = (OEMSDBAckUserData*)user_data;

   // store the event's info in the corresponding OEMSock instance
   pUserData->pOEMSocket->asaInfo.bOverFlow = ((sdb_ack_info->overflow != 0) ? TRUE : FALSE);
   nRet = OEMSocket_DSSToAEESDBAckStatus(sdb_ack_info->status,
                                         &(pUserData->pOEMSocket->asaInfo.nStatus));
   if (AEE_NET_SUCCESS != nRet) {
      return;
   }

   // invoke the AEE's callback
   AEE_SYS_RESUME_IF(pUserData->pAEECb);
}

/*===========================================================================
FUNCTION: OEMSocket_SetSDBAckCB

DESCRIPTION
  Sets a callback to be called when receiving an SDB Ack.
===========================================================================*/
int16 OEMSocket_SetSDBAckCB(OEMCONTEXT sockd, /* socket descriptor */
                            AEECallback* pCB
                           )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(dss_so_sdb_ack_cb_type);

   if ((OEMSocket*)AEE_NET_ERROR == me || (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   // initialize the struct sent to the DS with zeros
   memset(&me->sdb_ack_cb_type, 0, sizeof(me->sdb_ack_cb_type));

   // if the user wishes to register a callback,
   // init the data structures sent to lower layers.
   // if the user provided a NULL callback, we will pass NULL to the lower layers
   // (sdb_ack_cb_type.sdb_ack_cb will be 0).
   // This will cancel any previously registered callback.
   if (NULL != pCB->pfnNotify) {
      me->OEMUserData.pOEMSocket     = me;
      me->OEMUserData.pAEECb         = pCB;
      me->sdb_ack_cb_type.user_data  = (void*)&me->OEMUserData;
      me->sdb_ack_cb_type.sdb_ack_cb = (dss_so_sdb_ack_cb_fcn)OEMSocket_SDBAckCB;
   }

   ret = dss_setsockopt(me->sockfd,
                        DSS_SOCK,
                        DSS_SO_SDB_ACK_CB,
                        &me->sdb_ack_cb_type,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetKeepAlive

DESCRIPTION
  Returns the underlying socket keepalive setting
===========================================================================*/
int16 OEMSocket_GetKeepAlive(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbKeepAlive
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_KEEPALIVE,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbKeepAlive = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetLinger

DESCRIPTION
  Returns the underlying socket linger setting
===========================================================================*/
int16 OEMSocket_GetLinger(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbOn,          // linger on or off
  uint32               *pdwLinger      // linger time, in seconds
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   dss_so_linger_type dss_so_linger;
   uint32 len = sizeof(dss_so_linger);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_LINGER,
                        &dss_so_linger,
                        &len,
                        &dss_errno);

   *pbOn = (boolean)dss_so_linger.l_onoff;
   *pdwLinger = dss_so_linger.l_linger/1000; // seconds
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }


   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetRcvBuf

DESCRIPTION
  Returns the underlying socket receive buffer size
===========================================================================*/
int16 OEMSocket_GetRcvBuf(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                  *pnRcvBuf
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(*pnRcvBuf);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_RCVBUF,
                        pnRcvBuf,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetSndBuf

DESCRIPTION
  Returns the underlying socket send buffer size
===========================================================================*/
int16 OEMSocket_GetSndBuf(
  OEMCONTEXT            sockd,         /* socket descriptor */
  int                  *pnSndBuf
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(*pnSndBuf);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_SNDBUF,
                        pnSndBuf,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetNoDelay

DESCRIPTION
  Returns the underlying socket Nagle delay setting
===========================================================================*/
int16 OEMSocket_GetNoDelay(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbNoDelay
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_NODELAY,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbNoDelay = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetDelayedAck

DESCRIPTION
  Returns the underlying socket Delayed Ack setting
===========================================================================*/
int16 OEMSocket_GetDelayedAck(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbDelayedAck
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_DELAYED_ACK,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbDelayedAck = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetSACK

DESCRIPTION
  Returns the underlying socket SACK setting
===========================================================================*/
int16 OEMSocket_GetSACK(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbSACK
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_SACK,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbSACK = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetTimeStamp

DESCRIPTION
  Returns the underlying socket TCP time stamp setting
===========================================================================*/
int16 OEMSocket_GetTimeStamp(
  OEMCONTEXT            sockd,         /* socket descriptor */
  boolean              *pbTimeStamp
)
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_TIME_STAMP,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbTimeStamp = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetIPToS

DESCRIPTION:
   Gets the socket's Type of Service.
===========================================================================*/
int16 OEMSocket_GetIPToS(OEMCONTEXT sockd, /* socket descriptor */
                         int*       pnIPToS
                        )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(*pnIPToS);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_IP,
                        DSS_IP_TOS,
                        pnIPToS,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetIPv6TrafficClass

DESCRIPTION:
   Gets the socket's Traffic Class for outgoing IPv6 datagrams.
===========================================================================*/
int16 OEMSocket_GetIPv6TrafficClass(OEMCONTEXT sockd, /* socket descriptor */
                                    int*       pnIPv6TClass
                                   )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(*pnIPv6TClass);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
      }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_IPV6,
                        DSS_IPV6_TCLASS,
                        pnIPv6TClass,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetIPTTL

DESCRIPTION:
   Gets the socket's Time To Live for outgoing IP packets.
===========================================================================*/
int16 OEMSocket_GetIPTTL(OEMCONTEXT sockd, /* socket descriptor */
                         int*       pnIPTTL
                        )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(*pnIPTTL);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_IP,
                        DSS_IP_TTL,
                        pnIPTTL,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetTCPMaxSegmentSize

DESCRIPTION:
   Gets the socket's maximum segment size for outgoing TCP packets.
===========================================================================*/
int16 OEMSocket_GetTCPMaxSegmentSize(OEMCONTEXT sockd, /* socket descriptor */
                                     int*       pnMaxSeg
                                    )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   uint32 len = sizeof(*pnMaxSeg);

   if ((OEMSocket*)AEE_NET_ERROR == me ||
       (OEMSocket*)0 == me) {
         return AEE_NET_EBADF;
      }

   ret = dss_getsockopt(me->sockfd,
                        DSS_IPPROTO_TCP,
                        DSS_TCP_MAXSEG,
                        pnMaxSeg,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetReuseAddr

DESCRIPTION
   Returns the underlying socket reuseAddr setting.
===========================================================================*/
int16 OEMSocket_GetReuseAddr(OEMCONTEXT sockd, /* socket descriptor */
                             boolean*   pbReuseAddr
                            )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_REUSEADDR,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbReuseAddr = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetDisableFlowFwding

DESCRIPTION
  Returns the underlying socket disableFlowFwding setting
===========================================================================*/
int16 OEMSocket_GetDisableFlowFwding(OEMCONTEXT sockd, /* socket descriptor */
                                     boolean*   pbDisableFlowFwding
                                    )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   int nFlag;
   uint32 len = sizeof(nFlag);

   if ((OEMSocket*)AEE_NET_ERROR == me) {
      return AEE_NET_EBADF;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOL_SOCKET,
                        DSS_SO_DISABLE_FLOW_FWDING,
                        &nFlag,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   *pbDisableFlowFwding = nFlag ? TRUE : FALSE;
   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION: OEMSocket_GetSDBAckCB

DESCRIPTION
  Gets the data associated with the DSS_SO_SDB_ACK_CB socket option.
===========================================================================*/
int16 OEMSocket_GetSDBAckCB(OEMCONTEXT          sockd,
                            AEESDBAckCBType*    pAEESDBAckCBType
                           )
{
   OEMSocket* me = (OEMSocket*)sockd;
   sint15 ret;
   sint15 dss_errno;
   dss_so_sdb_ack_cb_type sdb_ack_cb_type;
   uint32 len = sizeof(dss_so_sdb_ack_cb_type);

   if ((OEMSocket*)AEE_NET_ERROR == me || (OEMSocket*)0 == me) {
      return AEE_NET_EBADF;
   }

   if (NULL == pAEESDBAckCBType) {
      return EBADPARM;
   }

   ret = dss_getsockopt(me->sockfd,
                        DSS_SOCK,
                        DSS_SO_SDB_ACK_CB,
                        &sdb_ack_cb_type,
                        &len,
                        &dss_errno);
   if (DSS_ERROR == ret) {
      return DSSToAEE_Err(dss_errno);
   }

   pAEESDBAckCBType->pfn       = (PFNSDBACKEVENT)(sdb_ack_cb_type.sdb_ack_cb);
   pAEESDBAckCBType->pUserData = sdb_ack_cb_type.user_data;

   return AEE_NET_SUCCESS;
}

/*===========================================================================

                      OEM NET QOS FUNCTION DEFINITIONS

 ===========================================================================*/

#ifdef OEM_PROVIDES_QOS

/*===========================================================================

FUNCTION TranslateAMSSFlowToOEM

DESCRIPTION
  Translate AMSS Flow spec into an OEM Flow spec

===========================================================================*/
uint16 TranslateAMSSFlowToOEM(ip_flow_type *pAMSSFlow, oem_ip_flow_type *pOemFlow)
{
    pOemFlow->field_mask = 0;
    pOemFlow->err_mask = pAMSSFlow->err_mask;

    if (pAMSSFlow->field_mask & IPFLOW_MASK_TRF_CLASS) {
       switch (pAMSSFlow->trf_class)
       {
       case IP_TRF_CLASS_CONVERSATIONAL:
          pOemFlow->trf_class = OEM_IP_TRF_CLASS_CONVERSATIONAL;
          break;

       case IP_TRF_CLASS_STREAMING:
          pOemFlow->trf_class = OEM_IP_TRF_CLASS_STREAMING;
          break;

       case IP_TRF_CLASS_INTERACTIVE:
          pOemFlow->trf_class = OEM_IP_TRF_CLASS_INTERACTIVE;
          break;

       case IP_TRF_CLASS_BACKGROUND:
          pOemFlow->trf_class = OEM_IP_TRF_CLASS_BACKGROUND;
          break;

       default:
          return AEE_NET_EBADOPTVAL;
       }

       pOemFlow->field_mask |= OEM_IPFLOW_MASK_TRF_CLASS;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_DATA_RATE) {

       switch (pAMSSFlow->data_rate.format_type)
       {
          case DATA_RATE_FORMAT_MIN_MAX_TYPE:
             pOemFlow->data_rate.format_type = OEM_DATA_RATE_FORMAT_MIN_MAX_TYPE;

             if (sizeof(pOemFlow->data_rate.format.min_max)
                 != sizeof(pAMSSFlow->data_rate.format.min_max)) {
                return AEE_NET_EBADOPTLEN;
             }

             memcpy(&pOemFlow->data_rate.format.min_max,
                    &pAMSSFlow->data_rate.format.min_max,
                    sizeof(pAMSSFlow->data_rate.format.min_max));
             break;

          case DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE:
             pOemFlow->data_rate.format_type = OEM_DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE;

             if (sizeof(pOemFlow->data_rate.format.token_bucket)
                 != sizeof(pAMSSFlow->data_rate.format.token_bucket)) {
                return AEE_NET_EBADOPTLEN;
             }

             memcpy(&pOemFlow->data_rate.format.token_bucket,
                    &pAMSSFlow->data_rate.format.token_bucket,
                    sizeof(pAMSSFlow->data_rate.format.token_bucket));
             break;

          default:
             return AEE_NET_EBADOPTVAL;
       }

       pOemFlow->field_mask |= OEM_IPFLOW_MASK_DATA_RATE;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_LATENCY) {

       pOemFlow->latency = pAMSSFlow->latency;
       pOemFlow->field_mask |= OEM_IPFLOW_MASK_LATENCY;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_LATENCY_VAR) {
       pOemFlow->latency_var = pAMSSFlow->latency_var;
       pOemFlow->field_mask |= OEM_IPFLOW_MASK_LATENCY_VAR;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_PKT_ERR_RATE) {

       if (sizeof(pOemFlow->pkt_err_rate)
           != sizeof(pAMSSFlow->pkt_err_rate)) {
          return AEE_NET_EBADOPTLEN;
       }

       memcpy(&pOemFlow->pkt_err_rate,
              &pAMSSFlow->pkt_err_rate,
              sizeof(pAMSSFlow->pkt_err_rate));

       pOemFlow->field_mask |= OEM_IPFLOW_MASK_PKT_ERR_RATE;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_MIN_POLICED_PKT_SIZE) {
       pOemFlow->min_policed_pkt_size = pAMSSFlow->min_policed_pkt_size;
       pOemFlow->field_mask |= OEM_IPFLOW_MASK_MIN_POLICED_PKT_SIZE;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE) {
       pOemFlow->max_allowed_pkt_size = pAMSSFlow->max_allowed_pkt_size;
       pOemFlow->field_mask |= OEM_IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_UMTS_RES_BER) {
       switch (pAMSSFlow->umts_params.res_ber)
       {
       case UMTS_RES_BIT_ERR_RATE1:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE1;
          break;

       case UMTS_RES_BIT_ERR_RATE2:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE2;
          break;

       case UMTS_RES_BIT_ERR_RATE3:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE3;
          break;

       case UMTS_RES_BIT_ERR_RATE4:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE4;
          break;

       case UMTS_RES_BIT_ERR_RATE5:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE5;
          break;

       case UMTS_RES_BIT_ERR_RATE6:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE6;
          break;

       case UMTS_RES_BIT_ERR_RATE7:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE7;
          break;

       case UMTS_RES_BIT_ERR_RATE8:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE8;
          break;

       case UMTS_RES_BIT_ERR_RATE9:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE9;
          break;

       case UMTS_RES_BIT_ERR_RATE10:
          pOemFlow->umts_params.res_ber = OEM_UMTS_RES_BIT_ERR_RATE10;
          break;

       default:
          return AEE_NET_EBADOPTVAL;
       }

       pOemFlow->field_mask |= OEM_IPFLOW_MASK_UMTS_RES_BER;
    }

   if (pAMSSFlow->field_mask & IPFLOW_MASK_UMTS_TRF_PRI) {
       switch (pAMSSFlow->umts_params.trf_pri)
       {
       case UMTS_TRF_HANDLING_PRI1:
          pOemFlow->umts_params.trf_pri = OEM_UMTS_TRF_HANDLING_PRI1;
          break;

       case UMTS_TRF_HANDLING_PRI2:
          pOemFlow->umts_params.trf_pri = OEM_UMTS_TRF_HANDLING_PRI2;
          break;

       case UMTS_TRF_HANDLING_PRI3:
          pOemFlow->umts_params.trf_pri = OEM_UMTS_TRF_HANDLING_PRI3;
          break;

       default:
          return AEE_NET_EBADOPTVAL;
       }

       pOemFlow->field_mask |= OEM_IPFLOW_MASK_UMTS_TRF_PRI;
    }

    if (pAMSSFlow->field_mask & IPFLOW_MASK_CDMA_PROFILE_ID) {
       pOemFlow->cdma_params.profile_id = pAMSSFlow->cdma_params.profile_id;
       pOemFlow->field_mask |= OEM_IPFLOW_MASK_CDMA_PROFILE_ID;
    }

    return OEMDSS_TranslateAMSSFlowOptToOEM(pAMSSFlow, pOemFlow);
}

/*===========================================================================

FUNCTION TranslateOEMFlowToAMSS

DESCRIPTION
  Translate OEM Flow spec into an AMSS Flow spec

===========================================================================*/
uint16 TranslateOEMFlowToAMSS(oem_ip_flow_type *pOemFlow, ip_flow_type *pAMSSFlow)
{
    pAMSSFlow->field_mask = IPFLOW_MASK_NONE;

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_TRF_CLASS) {
       switch (pOemFlow->trf_class)
       {
       case OEM_IP_TRF_CLASS_CONVERSATIONAL:
          pAMSSFlow->trf_class = IP_TRF_CLASS_CONVERSATIONAL;
          break;

       case OEM_IP_TRF_CLASS_STREAMING:
          pAMSSFlow->trf_class = IP_TRF_CLASS_STREAMING;
          break;

       case OEM_IP_TRF_CLASS_INTERACTIVE:
          pAMSSFlow->trf_class = IP_TRF_CLASS_INTERACTIVE;
          break;

       case OEM_IP_TRF_CLASS_BACKGROUND:
          pAMSSFlow->trf_class = IP_TRF_CLASS_BACKGROUND;
          break;

       default:
          pOemFlow->err_mask |= OEM_IPFLOW_MASK_TRF_CLASS;
          return AEE_NET_EBADOPTVAL;
       }

       pAMSSFlow->field_mask |= IPFLOW_MASK_TRF_CLASS;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_DATA_RATE) {

       switch (pOemFlow->data_rate.format_type)
       {
          case OEM_DATA_RATE_FORMAT_MIN_MAX_TYPE:
             pAMSSFlow->data_rate.format_type = DATA_RATE_FORMAT_MIN_MAX_TYPE;

             if (sizeof(pAMSSFlow->data_rate.format.min_max)
                 != sizeof(pOemFlow->data_rate.format.min_max)) {
                pOemFlow->err_mask |= OEM_IPFLOW_MASK_DATA_RATE;
                return AEE_NET_EBADOPTLEN;
             }

             memcpy(&pAMSSFlow->data_rate.format.min_max,
                    &pOemFlow->data_rate.format.min_max,
                    sizeof(pOemFlow->data_rate.format.min_max));
             break;

          case OEM_DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE:
             pAMSSFlow->data_rate.format_type = DATA_RATE_FORMAT_TOKEN_BUCKET_TYPE;

             if (sizeof(pAMSSFlow->data_rate.format.token_bucket)
                 != sizeof(pOemFlow->data_rate.format.token_bucket)) {
                pOemFlow->err_mask |= OEM_IPFLOW_MASK_DATA_RATE;
                return AEE_NET_EBADOPTLEN;
             }

             memcpy(&pAMSSFlow->data_rate.format.token_bucket,
                    &pOemFlow->data_rate.format.token_bucket,
                    sizeof(pOemFlow->data_rate.format.token_bucket));
             break;

          default:
             pOemFlow->err_mask |= OEM_IPFLOW_MASK_DATA_RATE;
             return AEE_NET_EBADOPTVAL;
       }

       pAMSSFlow->field_mask |= IPFLOW_MASK_DATA_RATE;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_LATENCY) {

       pAMSSFlow->latency = pOemFlow->latency;
       pAMSSFlow->field_mask |= IPFLOW_MASK_LATENCY;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_LATENCY_VAR) {
       pAMSSFlow->latency_var = pOemFlow->latency_var;
       pAMSSFlow->field_mask |= IPFLOW_MASK_LATENCY_VAR;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_PKT_ERR_RATE) {

       if (sizeof(pAMSSFlow->pkt_err_rate)
           != sizeof(pOemFlow->pkt_err_rate)) {
          pOemFlow->err_mask |= OEM_IPFLOW_MASK_PKT_ERR_RATE;
          return AEE_NET_EBADOPTLEN;
       }

       memcpy(&pAMSSFlow->pkt_err_rate,
              &pOemFlow->pkt_err_rate,
              sizeof(pOemFlow->pkt_err_rate));

       pAMSSFlow->field_mask |= IPFLOW_MASK_PKT_ERR_RATE;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_MIN_POLICED_PKT_SIZE) {
       pAMSSFlow->min_policed_pkt_size = pOemFlow->min_policed_pkt_size;
       pAMSSFlow->field_mask |= IPFLOW_MASK_MIN_POLICED_PKT_SIZE;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE) {
       pAMSSFlow->max_allowed_pkt_size = pOemFlow->max_allowed_pkt_size;
       pAMSSFlow->field_mask |= IPFLOW_MASK_MAX_ALLOWED_PKT_SIZE;
    }

    if (pOemFlow->field_mask & OEM_IPFLOW_MASK_UMTS_RES_BER) {
       switch (pOemFlow->umts_params.res_ber)
       {
       case OEM_UMTS_RES_BIT_ERR_RATE1:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE1;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE2:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE2;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE3:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE3;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE4:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE4;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE5:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE5;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE6:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE6;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE7:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE7;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE8:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE8;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE9:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE9;
          break;

       case OEM_UMTS_RES_BIT_ERR_RATE10:
          pAMSSFlow->umts_params.res_ber = UMTS_RES_BIT_ERR_RATE10;
          break;

       default:
          pOemFlow->err_mask |= OEM_IPFLOW_MASK_UMTS_RES_BER;
          return AEE_NET_EBADOPTVAL;
       }

       pAMSSFlow->field_mask |= IPFLOW_MASK_UMTS_RES_BER;
    }

   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_UMTS_TRF_PRI) {
       switch (pOemFlow->umts_params.trf_pri)
       {
       case OEM_UMTS_TRF_HANDLING_PRI1:
          pAMSSFlow->umts_params.trf_pri = UMTS_TRF_HANDLING_PRI1;
          break;

       case OEM_UMTS_TRF_HANDLING_PRI2:
          pAMSSFlow->umts_params.trf_pri = UMTS_TRF_HANDLING_PRI2;
          break;

       case OEM_UMTS_TRF_HANDLING_PRI3:
          pAMSSFlow->umts_params.trf_pri = UMTS_TRF_HANDLING_PRI3;
          break;

       default:
          pOemFlow->err_mask |= OEM_IPFLOW_MASK_UMTS_TRF_PRI;
          return AEE_NET_EBADOPTVAL;
       }

       pAMSSFlow->field_mask |= IPFLOW_MASK_UMTS_TRF_PRI;
    }

   if (pOemFlow->field_mask & OEM_IPFLOW_MASK_CDMA_PROFILE_ID) {
       // Validate CDMA Profile ID value size
       if ( ((sizeof(pAMSSFlow->cdma_params.profile_id)==2) && pOemFlow->cdma_params.profile_id>MAX_UINT16) ||
          ((sizeof(pAMSSFlow->cdma_params.profile_id)==1) && pOemFlow->cdma_params.profile_id>MAX_UINT8) ) {
            pOemFlow->err_mask |= OEM_IPFLOW_MASK_CDMA_PROFILE_ID;
            return AEE_NET_EBADOPTLEN;
       }
       pAMSSFlow->cdma_params.profile_id = pOemFlow->cdma_params.profile_id;
       pAMSSFlow->field_mask |= IPFLOW_MASK_CDMA_PROFILE_ID;
    }

    return OEMDSS_TranslateOEMFlowOptToAMSS(pOemFlow, pAMSSFlow);
}

/*===========================================================================

FUNCTION TranslateOEMFlowListToAMSSAuxFlowList

DESCRIPTION
  Translate OEM array of Flow specs into an AMSS array of Auxiliary Flow specs

===========================================================================*/
uint16 TranslateOEMFlowListToAMSSAuxFlowList(oem_ip_flow_type* pOEMFlowArr,
                                 uint8 uNumFlows,
                                 ip_flow_spec_type  *pAMSSFlowSpec)
{
   uint16 result;
   uint8 i;
   ip_flow_type* pFlow;
   if (uNumFlows > 0) {
      result = OEMDSS_MallocAuxFlowList(pAMSSFlowSpec, uNumFlows);
      if (AEE_SUCCESS != result) {
         return result;
      }
      // Translate each flow in the list
      for (i=0; i<uNumFlows; i++) {
         pFlow = OEMDSS_GetAuxFlow(pAMSSFlowSpec, i);
         result = TranslateOEMFlowToAMSS(&pOEMFlowArr[i], pFlow);
         if (AEE_NET_SUCCESS != result) {
            // It's the caller responsibility to free the allocated memory
            // for the AMSS Auxiliary Flow List by calling FreeAMSSSpecContents().
            return result;
         }
      }
   }
   return AEE_SUCCESS;
}

/*===========================================================================

FUNCTION TranslateOEMFilterToAMSS

DESCRIPTION
  Translate OEM Filter to AMSS Filter

===========================================================================*/
int16 TranslateOEMFilterToAMSS(oem_ip_filter_type *pOemFilter,
                               ip_filter_type     *pAMSSFilter)
{
   uint8 nOEMNextHeaderProt = OEM_NO_NEXT_HDR;
   uint8* pnAMSSNextHeaderProt = NULL;

   switch (pOemFilter->ip_vsn) {
      case OEM_IP_V4:
         pAMSSFilter->ip_vsn = IP_V4;
         break;

      case OEM_IP_V6:
         pAMSSFilter->ip_vsn = IP_V6;
         break;

      default:
         return AEE_NET_EBADOPTVAL;
   }

   if (OEM_IP_V4 == pOemFilter->ip_vsn) {
      if (pOemFilter->ip_hdr.v4.field_mask & OEM_IPFLTR_MASK_IP4_SRC_ADDR) {

         if (sizeof(pAMSSFilter->ip_hdr.v4.src)
            != sizeof(pOemFilter->ip_hdr.v4.src)) {
            pOemFilter->ip_hdr.v4.err_mask |= OEM_IPFLTR_MASK_IP4_SRC_ADDR;
            return AEE_NET_EBADOPTLEN;
         }

         memcpy(&pAMSSFilter->ip_hdr.v4.src,
               &pOemFilter->ip_hdr.v4.src,
               sizeof(pOemFilter->ip_hdr.v4.src));

         pAMSSFilter->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_SRC_ADDR;
      }

      if (pOemFilter->ip_hdr.v4.field_mask & OEM_IPFLTR_MASK_IP4_DST_ADDR) {

         if (sizeof(pAMSSFilter->ip_hdr.v4.dst)
            != sizeof(pOemFilter->ip_hdr.v4.dst)) {
            pOemFilter->ip_hdr.v4.err_mask |= OEM_IPFLTR_MASK_IP4_DST_ADDR;
            return AEE_NET_EBADOPTLEN;
         }

         memcpy(&pAMSSFilter->ip_hdr.v4.dst,
               &pOemFilter->ip_hdr.v4.dst,
               sizeof(pOemFilter->ip_hdr.v4.dst));

         pAMSSFilter->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_DST_ADDR;
      }

      if (pOemFilter->ip_hdr.v4.field_mask & OEM_IPFLTR_MASK_IP4_TOS) {

         if (sizeof(pAMSSFilter->ip_hdr.v4.tos)
            != sizeof(pOemFilter->ip_hdr.v4.tos)) {
            pOemFilter->ip_hdr.v4.err_mask |= OEM_IPFLTR_MASK_IP4_TOS;
            return AEE_NET_EBADOPTLEN;
         }

         memcpy(&pAMSSFilter->ip_hdr.v4.tos,
               &pOemFilter->ip_hdr.v4.tos,
               sizeof(pOemFilter->ip_hdr.v4.tos));

         pAMSSFilter->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_TOS;
      }

      if (pOemFilter->ip_hdr.v4.field_mask & OEM_IPFLTR_MASK_IP4_NEXT_HDR_PROT) {
         nOEMNextHeaderProt = pOemFilter->ip_hdr.v4.next_hdr_prot;
         pnAMSSNextHeaderProt = &(pAMSSFilter->ip_hdr.v4.next_hdr_prot);
      }

   } else if (OEM_IP_V6 == pOemFilter->ip_vsn) {
      if (pOemFilter->ip_hdr.v6.field_mask & OEM_IPFLTR_MASK_IP6_SRC_ADDR) {
         if (sizeof(pAMSSFilter->ip_hdr.v6.src.addr)
            != sizeof(pOemFilter->ip_hdr.v6.src.addr)) {

            pOemFilter->ip_hdr.v6.err_mask |= OEM_IPFLTR_MASK_IP6_SRC_ADDR;
            return AEE_NET_EBADOPTLEN;
         }

         memcpy(&pAMSSFilter->ip_hdr.v6.src.addr,
                &pOemFilter->ip_hdr.v6.src.addr,
                sizeof(pOemFilter->ip_hdr.v6.src.addr));
         pAMSSFilter->ip_hdr.v6.src.prefix_len = pOemFilter->ip_hdr.v6.src.prefix_len;
         pAMSSFilter->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_SRC_ADDR;
      }

      if (pOemFilter->ip_hdr.v6.field_mask & OEM_IPFLTR_MASK_IP6_DST_ADDR) {

         if (sizeof(pAMSSFilter->ip_hdr.v6.dst.addr)
            != sizeof(pOemFilter->ip_hdr.v6.dst.addr)) {
            pOemFilter->ip_hdr.v6.err_mask |= OEM_IPFLTR_MASK_IP6_DST_ADDR;
            return AEE_NET_EBADOPTLEN;
         }

         memcpy(&pAMSSFilter->ip_hdr.v6.dst.addr,
                &pOemFilter->ip_hdr.v6.dst.addr,
                sizeof(pOemFilter->ip_hdr.v6.dst.addr));
         pAMSSFilter->ip_hdr.v6.dst.prefix_len = pOemFilter->ip_hdr.v6.dst.prefix_len;
         pAMSSFilter->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_DST_ADDR;
      }

      if (pOemFilter->ip_hdr.v6.field_mask & OEM_IPFLTR_MASK_IP6_TRAFFIC_CLASS) {

         if (sizeof(pAMSSFilter->ip_hdr.v6.trf_cls)
            != sizeof(pOemFilter->ip_hdr.v6.trf_cls)) {
            pOemFilter->ip_hdr.v6.err_mask |= OEM_IPFLTR_MASK_IP6_TRAFFIC_CLASS;
            return AEE_NET_EBADOPTLEN;
         }

         memcpy(&pAMSSFilter->ip_hdr.v6.trf_cls,
                &pOemFilter->ip_hdr.v6.trf_cls,
                sizeof(pOemFilter->ip_hdr.v6.trf_cls));

         pAMSSFilter->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_TRAFFIC_CLASS;
      }

      if (pOemFilter->ip_hdr.v6.field_mask & OEM_IPFLTR_MASK_IP6_FLOW_LABEL) {

         if (sizeof(pAMSSFilter->ip_hdr.v6.flow_label)
            != sizeof(pOemFilter->ip_hdr.v6.flow_label)) {
            pOemFilter->ip_hdr.v6.err_mask |= OEM_IPFLTR_MASK_IP6_FLOW_LABEL;
            return AEE_NET_EBADOPTLEN;
         }

         pAMSSFilter->ip_hdr.v6.flow_label = pOemFilter->ip_hdr.v6.flow_label;
         pAMSSFilter->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_FLOW_LABEL;
      }

      if (pOemFilter->ip_hdr.v6.field_mask & OEM_IPFLTR_MASK_IP6_NEXT_HDR_PROT) {
         nOEMNextHeaderProt = pOemFilter->ip_hdr.v6.next_hdr_prot;
         pnAMSSNextHeaderProt = &(pAMSSFilter->ip_hdr.v6.next_hdr_prot);
      }
   }

   if (OEM_NO_NEXT_HDR != nOEMNextHeaderProt) {

      switch (nOEMNextHeaderProt) {

         case OEM_IPPROTO_TCP:
            *pnAMSSNextHeaderProt = IPPROTO_TCP;

            if (pOemFilter->next_prot_hdr.tcp.field_mask & OEM_IPFLTR_MASK_TCP_SRC_PORT) {
               if (sizeof(pAMSSFilter->next_prot_hdr.tcp.src)
                   != sizeof(pOemFilter->next_prot_hdr.tcp.src)) {
                  pOemFilter->next_prot_hdr.tcp.err_mask |= OEM_IPFLTR_MASK_TCP_SRC_PORT;
                  return AEE_NET_EBADOPTLEN;
               }

               memcpy(&pAMSSFilter->next_prot_hdr.tcp.src,
                      &pOemFilter->next_prot_hdr.tcp.src,
                      sizeof(pOemFilter->next_prot_hdr.tcp.src));

               pAMSSFilter->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_SRC_PORT;
            }

            if (pOemFilter->next_prot_hdr.tcp.field_mask & OEM_IPFLTR_MASK_TCP_DST_PORT) {
               if (sizeof(pAMSSFilter->next_prot_hdr.tcp.dst)
                   != sizeof(pOemFilter->next_prot_hdr.tcp.dst)) {
                  pOemFilter->next_prot_hdr.tcp.err_mask |= OEM_IPFLTR_MASK_TCP_DST_PORT;
                  return AEE_NET_EBADOPTLEN;
               }

               memcpy(&pAMSSFilter->next_prot_hdr.tcp.dst,
                      &pOemFilter->next_prot_hdr.tcp.dst,
                      sizeof(pOemFilter->next_prot_hdr.tcp.dst));

               pAMSSFilter->next_prot_hdr.tcp.field_mask |= IPFLTR_MASK_TCP_DST_PORT;
            }

            break;

         case OEM_IPPROTO_UDP:
            *pnAMSSNextHeaderProt = IPPROTO_UDP;
            if (pOemFilter->next_prot_hdr.udp.field_mask & OEM_IPFLTR_MASK_UDP_SRC_PORT) {
               if (sizeof(pAMSSFilter->next_prot_hdr.udp.src)
                   != sizeof(pOemFilter->next_prot_hdr.udp.src)) {
                  pOemFilter->next_prot_hdr.udp.err_mask |= OEM_IPFLTR_MASK_UDP_SRC_PORT;
                  return AEE_NET_EBADOPTLEN;
               }

               memcpy(&pAMSSFilter->next_prot_hdr.udp.src,
                      &pOemFilter->next_prot_hdr.udp.src,
                      sizeof(pOemFilter->next_prot_hdr.udp.src));

               pAMSSFilter->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_SRC_PORT;
            }

            if (pOemFilter->next_prot_hdr.udp.field_mask & OEM_IPFLTR_MASK_UDP_DST_PORT) {
               if (sizeof(pAMSSFilter->next_prot_hdr.udp.dst)
                   != sizeof(pOemFilter->next_prot_hdr.udp.dst)) {
                  pOemFilter->next_prot_hdr.udp.err_mask |= OEM_IPFLTR_MASK_UDP_DST_PORT;
                  return AEE_NET_EBADOPTLEN;
               }

               memcpy(&pAMSSFilter->next_prot_hdr.udp.dst,
                      &pOemFilter->next_prot_hdr.udp.dst,
                      sizeof(pOemFilter->next_prot_hdr.udp.dst));

               pAMSSFilter->next_prot_hdr.udp.field_mask |= IPFLTR_MASK_UDP_DST_PORT;
            }

            break;

         default:
            if (OEM_IP_V4 == pOemFilter->ip_vsn) {
               pOemFilter->ip_hdr.v4.err_mask |= OEM_IPFLTR_MASK_IP4_NEXT_HDR_PROT;
            } else { //OEM_IP_V6
               pOemFilter->ip_hdr.v6.err_mask |= OEM_IPFLTR_MASK_IP6_NEXT_HDR_PROT;
            }

            return AEE_NET_EBADOPTVAL;
      }

      if (OEM_IP_V4 == pOemFilter->ip_vsn) {
         pAMSSFilter->ip_hdr.v4.field_mask |= IPFLTR_MASK_IP4_NEXT_HDR_PROT;
      } else { //OEM_IP_V6
         pAMSSFilter->ip_hdr.v6.field_mask |= IPFLTR_MASK_IP6_NEXT_HDR_PROT;
      }
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION TranslateOEMFilterSpecToAMSS

DESCRIPTION
  Translate OEM Filter Spec to AMSS Filter Spec

===========================================================================*/
int16 TranslateOEMFilterSpecToAMSS(oem_ip_filter_spec_type *pOemFilterSpec,
                                   ip_filter_spec_type     *pAMSSFilterSpec)
{
   int16 result;
   uint8 index;

   pAMSSFilterSpec->list_ptr =
      sys_malloc(pOemFilterSpec->num_filters * sizeof(ip_filter_type));

   if (!pAMSSFilterSpec->list_ptr) {
      return AEE_NET_ENOMEM;
   }

   for (index = 0; index < pOemFilterSpec->num_filters; ++index) {
      result = TranslateOEMFilterToAMSS(&pOemFilterSpec->list_ptr[index],
                                        &pAMSSFilterSpec->list_ptr[index]);
      if (result != AEE_NET_SUCCESS) {
         sys_free(pAMSSFilterSpec->list_ptr);
         return result;
      }
   }

   pAMSSFilterSpec->num_filters = pOemFilterSpec->num_filters;
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION TranslateOEMQoSSpecToAMSS

DESCRIPTION
  Translate OEM QoS Spec to AMSS QoS Spec

===========================================================================*/
int16 TranslateOEMQoSSpecToAMSS(oem_qos_spec_type *pOemQoSSpec,
                                    qos_spec_type *pAMSSQoSSpec)
{
   int16 result;

   // Translate Rx QoS Filter:

#ifdef OEM_SUPPORTS_QOS_MODIFY
   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_RX_FLTR_MODIFY) {
      // Modify Rx Filter Request
      result = TranslateOEMFilterSpecToAMSS(&pOemQoSSpec->rx.fltr_template,
                                            &pAMSSQoSSpec->rx.fltr_template);
      if (result != AEE_NET_SUCCESS) {
         return result;
      }
      pAMSSQoSSpec->field_mask |= QOS_MODIFY_MASK_RX_FLTR_MODIFY;

   } else
#endif // OEM_SUPPORTS_QOS_MODIFY
   if ( (pOemQoSSpec->field_mask & OEM_QOS_MASK_RX_FLOW) &&
               (OEM_IPFLOW_MASK_NONE != pOemQoSSpec->rx.flow_template.req_flow.field_mask) &&
               (0 < pOemQoSSpec->rx.fltr_template.num_filters) ) {
      // Request Rx QoS Spec
      result = TranslateOEMFilterSpecToAMSS(&pOemQoSSpec->rx.fltr_template,
                                            &pAMSSQoSSpec->rx.fltr_template);
      if (result != AEE_NET_SUCCESS) {
         return result;
      }
   }

   // Translate Rx QoS Flow:

   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_RX_FLOW) {
      result = TranslateOEMFlowToAMSS(&pOemQoSSpec->rx.flow_template.req_flow,
                                      &pAMSSQoSSpec->rx.flow_template.req_flow);
      if (result != AEE_NET_SUCCESS) {
         return result;
      }
      pAMSSQoSSpec->field_mask |= QOS_MASK_RX_FLOW;
   }

   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_RX_MIN_FLOW) {
      result = TranslateOEMFlowToAMSS(&pOemQoSSpec->rx.flow_template.min_req_flow,
                                      &pAMSSQoSSpec->rx.flow_template.min_req_flow);
      if (result != AEE_NET_SUCCESS) {
         return result;
      }
      pAMSSQoSSpec->field_mask |= QOS_MASK_RX_MIN_FLOW;
   }

   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_RX_AUXILIARY_FLOWS) {
      result = TranslateOEMFlowListToAMSSAuxFlowList(pOemQoSSpec->rx.flow_template.aux_flow_list_ptr,
                              pOemQoSSpec->rx.flow_template.num_aux_flows,
                              &(pAMSSQoSSpec->rx.flow_template));
      if (result != AEE_NET_SUCCESS) {
         return result;
      }
      OEMDSS_SetRxAuxFlowMask(&(pAMSSQoSSpec->field_mask));
   }

   // Translate Tx QoS Filter:

#ifdef OEM_SUPPORTS_QOS_MODIFY
   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_TX_FLTR_MODIFY) {
      // Modify Tx Filter Request
      result = TranslateOEMFilterSpecToAMSS(&pOemQoSSpec->tx.fltr_template,
                                            &pAMSSQoSSpec->tx.fltr_template);
      if (result != AEE_NET_SUCCESS) {
         return result;
      }
      pAMSSQoSSpec->field_mask |= QOS_MODIFY_MASK_TX_FLTR_MODIFY;

   } else
#endif // OEM_SUPPORTS_QOS_MODIFY
   if ( (pOemQoSSpec->field_mask & OEM_QOS_MASK_TX_FLOW) &&
               (OEM_IPFLOW_MASK_NONE != pOemQoSSpec->tx.flow_template.req_flow.field_mask) &&
               (0 < pOemQoSSpec->tx.fltr_template.num_filters) ) {
      // Request Tx QoS Spec
      result = TranslateOEMFilterSpecToAMSS(&pOemQoSSpec->tx.fltr_template,
                                            &pAMSSQoSSpec->tx.fltr_template);
      if (result != AEE_NET_SUCCESS) {
         // It's the caller responsibility to free the allocated memory
         // for the AMSS Auxiliary Flow List by calling FreeAMSSSpecContents().
         return result;
      }
   }

   // Translate Tx QoS Flow:

   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_TX_FLOW) {
      result = TranslateOEMFlowToAMSS(&pOemQoSSpec->tx.flow_template.req_flow,
                                      &pAMSSQoSSpec->tx.flow_template.req_flow);
      if (result != AEE_NET_SUCCESS) {
         // It's the caller responsibility to free the allocated memory
         // for the AMSS Auxiliary Flow List by calling FreeAMSSSpecContents().
         return result;
      }
      pAMSSQoSSpec->field_mask |= QOS_MASK_TX_FLOW;
   }

   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_TX_MIN_FLOW) {
      result = TranslateOEMFlowToAMSS(&pOemQoSSpec->tx.flow_template.min_req_flow,
                                      &pAMSSQoSSpec->tx.flow_template.min_req_flow);
      if (result != AEE_NET_SUCCESS) {
         // It's the caller responsibility to free the allocated memory
         // for the AMSS Auxiliary Flow List by calling FreeAMSSSpecContents().
         return result;
      }
      pAMSSQoSSpec->field_mask |= QOS_MASK_TX_MIN_FLOW;
   }

   if (pOemQoSSpec->field_mask & OEM_QOS_MASK_TX_AUXILIARY_FLOWS) {
      result = TranslateOEMFlowListToAMSSAuxFlowList(pOemQoSSpec->tx.flow_template.aux_flow_list_ptr,
                              pOemQoSSpec->tx.flow_template.num_aux_flows,
                              &(pAMSSQoSSpec->tx.flow_template));
      if (result != AEE_NET_SUCCESS) {
         // It's the caller responsibility to free the allocated memory
         // for the AMSS Auxiliary Flow List by calling FreeAMSSSpecContents().
         return result;
      }
      OEMDSS_SetTxAuxFlowMask(&(pAMSSQoSSpec->field_mask));
   }

#if defined(AEE_SIMULATOR)
   // set last 4 bits according to the OEM QoS Spec
   pAMSSQoSSpec->field_mask = pAMSSQoSSpec->field_mask | (pOemQoSSpec->field_mask & 0xF00);
#endif // defined(AEE_SIMULATOR)

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION InitAMSSSpec

DESCRIPTION
  Initialize an AMSS QoS Spec to default/initial values

===========================================================================*/
void InitAMSSSpec(qos_spec_type *pAMSSQoSSpec)
{
   // init filter number to zero, for easy memory cleanup based on the filter
   // number, when translation fails
   pAMSSQoSSpec->rx.fltr_template.num_filters = 0;
   pAMSSQoSSpec->tx.fltr_template.num_filters = 0;

   // init auxiliary flows number to zero, for easy memory cleanup based on
   // the auxiliary flow number, when translation fails
   OEMDSS_SetNumAuxFlows(&(pAMSSQoSSpec->rx.flow_template), 0);
   OEMDSS_SetNumAuxFlows(&(pAMSSQoSSpec->tx.flow_template), 0);
}

/*===========================================================================

FUNCTION FreeAMSSSpecContents

DESCRIPTION
  free an AMSS QoS Spec's dynamically allocated buffers

===========================================================================*/
void FreeAMSSSpecContents(qos_spec_type *pAMSSQoSSpec)
{
   if (0 != pAMSSQoSSpec->rx.fltr_template.num_filters) {
      sys_free(pAMSSQoSSpec->rx.fltr_template.list_ptr);
   }

   if (0 != pAMSSQoSSpec->tx.fltr_template.num_filters) {
      sys_free(pAMSSQoSSpec->tx.fltr_template.list_ptr);
   }

   OEMDSS_FreeAuxFlowList(&(pAMSSQoSSpec->rx.flow_template));
   OEMDSS_FreeAuxFlowList(&(pAMSSQoSSpec->tx.flow_template));
}

/*===========================================================================

FUNCTION FreeAMSSSpecListContents

DESCRIPTION
  free AMSS QoS Spec list items, and the list itself which were allocated
  by TranslateOEMQoSSpecListToAMSS

===========================================================================*/
void FreeAMSSSpecListContents(qos_spec_type *pAMSSQoSSpecs, uint32 uNumQoSSpecs)
{
   uint32 i;

   for (i=0; i<uNumQoSSpecs; ++i) {
      FreeAMSSSpecContents(pAMSSQoSSpecs+i);
   }

   sys_free(pAMSSQoSSpecs);
}

/*===========================================================================

FUNCTION TranslateOEMQoSSpecListToAMSS

DESCRIPTION
  Translate OEM QoS Spec List to AMSS QoS Spec List.
  In case of SUCCESS return, the caller is responsible for freeing the
  list by calling FreeAMSSSpecListContents

===========================================================================*/
int16 TranslateOEMQoSSpecListToAMSS(oem_qos_spec_type   **ppOEMQoSSpecs,
                              qos_spec_type **ppAMSSQoSSpecs, uint32 uNumQoSSpecs) {
   qos_spec_type   *pAMSSQoSSpecs;
   uint32 i;
   int16 nRet;

   pAMSSQoSSpecs = sys_malloc(uNumQoSSpecs * sizeof(qos_spec_type));
   if (NULL == pAMSSQoSSpecs) {
      return AEE_NET_ENOMEM;
   }
   memset(pAMSSQoSSpecs, 0, uNumQoSSpecs * sizeof(qos_spec_type));

   for (i=0; i<uNumQoSSpecs; ++i) {
      InitAMSSSpec(pAMSSQoSSpecs+i);
      nRet = TranslateOEMQoSSpecToAMSS(ppOEMQoSSpecs[i], pAMSSQoSSpecs+i);
      if (AEE_NET_SUCCESS != nRet) {
         FreeAMSSSpecListContents(pAMSSQoSSpecs, i+1);
         return nRet;
      }
   }
   *ppAMSSQoSSpecs = pAMSSQoSSpecs;
   return SUCCESS;
}


/*===========================================================================

FUNCTION SetOEMQoSSpecErrorMaskFromAMSS

DESCRIPTION
  Set an OEM QoS Spec's error mask fields by coping them from an AMSS QoS Spec

===========================================================================*/
void SetOEMQoSSpecErrorMaskFromAMSS(oem_qos_spec_type *pOemQoSSpec,
                                    qos_spec_type     *pAMSSQoSSpec)
{
   int index;
   uint8 uNextHdrProt;

   // handle RX direction
   if (pAMSSQoSSpec->field_mask & QOS_MASK_RX_FLOW) {
      pOemQoSSpec->rx.flow_template.req_flow.err_mask =
         pAMSSQoSSpec->rx.flow_template.req_flow.err_mask;
   }

   if (OEMDSS_IsRxAuxFlowMask(pAMSSQoSSpec->field_mask)) {
      uint8 i, uSize;
      uSize = OEMDSS_GetNumAuxFlows(&(pAMSSQoSSpec->rx.flow_template));
      for (i=0; i<uSize; i++) {
         pOemQoSSpec->rx.flow_template.aux_flow_list_ptr[i].err_mask =
            OEMDSS_GetAuxFlowErrMask(&(pAMSSQoSSpec->rx.flow_template), i);
      }
   }

   if (pAMSSQoSSpec->field_mask & QOS_MASK_RX_MIN_FLOW) {
      pOemQoSSpec->rx.flow_template.min_req_flow.err_mask =
         pAMSSQoSSpec->rx.flow_template.min_req_flow.err_mask;
   }

   // handle RX filter list
   if ((pAMSSQoSSpec->field_mask & QOS_MASK_RX_FLOW) ||
       (pAMSSQoSSpec->field_mask & QOS_MASK_RX_MIN_FLOW) ||
       (OEMDSS_IsRxAuxFlowMask(pAMSSQoSSpec->field_mask)) ) {

      for (index = 0; index < pAMSSQoSSpec->rx.fltr_template.num_filters; ++index) {
         if (OEM_IP_V4 == pOemQoSSpec->rx.fltr_template.list_ptr[index].ip_vsn ) {
            pOemQoSSpec->rx.fltr_template.list_ptr[index].ip_hdr.v4.err_mask =
               pAMSSQoSSpec->rx.fltr_template.list_ptr[index].ip_hdr.v4.err_mask;

            uNextHdrProt = pAMSSQoSSpec->rx.fltr_template.list_ptr[index].ip_hdr.v4.next_hdr_prot;

         } else { // OEM_IP_V6
            pOemQoSSpec->rx.fltr_template.list_ptr[index].ip_hdr.v6.err_mask =
               pAMSSQoSSpec->rx.fltr_template.list_ptr[index].ip_hdr.v6.err_mask;

            uNextHdrProt = pAMSSQoSSpec->rx.fltr_template.list_ptr[index].ip_hdr.v6.next_hdr_prot;
         }

         if (IPPROTO_TCP == uNextHdrProt) {
            pOemQoSSpec->rx.fltr_template.list_ptr[index].next_prot_hdr.tcp.err_mask =
               pAMSSQoSSpec->rx.fltr_template.list_ptr[index].next_prot_hdr.tcp.err_mask;
         }

         if (IPPROTO_UDP == uNextHdrProt) {
            pOemQoSSpec->rx.fltr_template.list_ptr[index].next_prot_hdr.udp.err_mask =
               pAMSSQoSSpec->rx.fltr_template.list_ptr[index].next_prot_hdr.udp.err_mask;
         }
      }
   }

   // handle TX direction
   if (pAMSSQoSSpec->field_mask & QOS_MASK_TX_FLOW) {
      pOemQoSSpec->tx.flow_template.req_flow.err_mask =
         pAMSSQoSSpec->tx.flow_template.req_flow.err_mask;
   }

   if (OEMDSS_IsTxAuxFlowMask(pAMSSQoSSpec->field_mask)) {
      uint8 i, uSize;
      uSize = OEMDSS_GetNumAuxFlows(&(pAMSSQoSSpec->tx.flow_template));
      for (i=0; i<uSize; i++) {
         pOemQoSSpec->tx.flow_template.aux_flow_list_ptr[i].err_mask =
            OEMDSS_GetAuxFlowErrMask(&(pAMSSQoSSpec->tx.flow_template), i);
      }
   }

   if (pAMSSQoSSpec->field_mask & QOS_MASK_TX_MIN_FLOW) {
      pOemQoSSpec->tx.flow_template.min_req_flow.err_mask =
         pAMSSQoSSpec->tx.flow_template.min_req_flow.err_mask;
   }

   // handle TX filter list
   if ((pAMSSQoSSpec->field_mask & QOS_MASK_TX_FLOW) ||
       (pAMSSQoSSpec->field_mask & QOS_MASK_TX_MIN_FLOW) ||
       (OEMDSS_IsTxAuxFlowMask(pAMSSQoSSpec->field_mask)) ) {

      for (index = 0; index < pAMSSQoSSpec->tx.fltr_template.num_filters; ++index) {
         if (OEM_IP_V4 == pOemQoSSpec->tx.fltr_template.list_ptr[index].ip_vsn ) {
            pOemQoSSpec->tx.fltr_template.list_ptr[index].ip_hdr.v4.err_mask =
               pAMSSQoSSpec->tx.fltr_template.list_ptr[index].ip_hdr.v4.err_mask;

            uNextHdrProt = pAMSSQoSSpec->tx.fltr_template.list_ptr[index].ip_hdr.v4.next_hdr_prot;

         } else { // OEM_IP_V6
            pOemQoSSpec->tx.fltr_template.list_ptr[index].ip_hdr.v6.err_mask =
               pAMSSQoSSpec->tx.fltr_template.list_ptr[index].ip_hdr.v6.err_mask;

            uNextHdrProt = pAMSSQoSSpec->tx.fltr_template.list_ptr[index].ip_hdr.v6.next_hdr_prot;
         }

         if (IPPROTO_TCP == uNextHdrProt) {
            pOemQoSSpec->tx.fltr_template.list_ptr[index].next_prot_hdr.tcp.err_mask =
               pAMSSQoSSpec->tx.fltr_template.list_ptr[index].next_prot_hdr.tcp.err_mask;
         }

         if (IPPROTO_UDP == uNextHdrProt) {
            pOemQoSSpec->tx.fltr_template.list_ptr[index].next_prot_hdr.udp.err_mask =
               pAMSSQoSSpec->tx.fltr_template.list_ptr[index].next_prot_hdr.udp.err_mask;
         }
      }
   }
}

/*===========================================================================

FUNCTION SetOEMQoSSpecListErrorMasksFromAMSS

DESCRIPTION
  Set OEM QoS Spec's error mask fields for each item in the list,
  by coping the corresponding error from the AMSS QoS Spec list

===========================================================================*/
void SetOEMQoSSpecListErrorMasksFromAMSS(oem_qos_spec_type   **ppQoSSpecs,
                                             qos_spec_type     *pAMSSQoSSpec,
                                             uint32 uNumQoSSpecs)
{
   uint32 i;

   for (i=0; i<uNumQoSSpecs; ++i) {
      SetOEMQoSSpecErrorMaskFromAMSS(ppQoSSpecs[i], &pAMSSQoSSpec[i]);
   }
}

/*===========================================================================
 Insert a buffer into a QoS event list (at the head of the list)
===========================================================================*/
static void PutQoSEventBufAtHead( OEMNetQoSEventData **ppqeListHead, OEMNetQoSEventData *pqeBuffer )
{
   pqeBuffer->pNext = *ppqeListHead;
   *ppqeListHead = pqeBuffer;
}

/*===========================================================================
 Insert a buffer into a QoS event list (at the tail of the list)
===========================================================================*/
static void PutQoSEventBufAtTail( OEMNetQoSEventData **ppqeListHead, OEMNetQoSEventData *pqeBuffer )
{
   while ( *ppqeListHead ) {
      ppqeListHead = &((*ppqeListHead)->pNext);
   }
   *ppqeListHead = pqeBuffer;
}

/*===========================================================================
 Retrieve a buffer from a QoS event list (from the head of the list)
===========================================================================*/
static OEMNetQoSEventData* GetQoSEventBufFromHead( OEMNetQoSEventData **ppqeListHead )
{
   OEMNetQoSEventData *pqeResult = *ppqeListHead;

   if ( pqeResult ) {
      *ppqeListHead = pqeResult->pNext;
      pqeResult->pNext = NULL;
   }

   return pqeResult;
}

//
// callback function for QoS events
//
static void OEMNet_QoSEventOccurred
(
   dss_iface_ioctl_event_enum_type        event,
   dss_iface_ioctl_event_info_union_type  event_info,
   void                                  *user_data,
   sint15                                 app_id,
   dss_iface_id_type                      iface_id
)
{
   OEMNet *pNet = OEMNet_FromAppID(app_id); // is this safe ?

   if ((OEMNet*)0 != pNet) {

      OEMNetQoSEventData *pqeBuffer;

      INTLOCK();
      pqeBuffer = GetQoSEventBufFromHead(&pNet->pqeFreeBuffers);
      INTFREE();

      if ( pqeBuffer ) {

         pqeBuffer->handle = event_info.qos_info.handle;
         pqeBuffer->infoCode = event_info.qos_info.info_code;
         pqeBuffer->event = event;

         INTLOCK();
         PutQoSEventBufAtTail(&pNet->pqeReadyBuffers, pqeBuffer);
         INTFREE();
      }
      // else QoS free queue is empty!

      // TODO: in case of an empty QoS free queue,
      // need to print out a log indication.
      // Though, debug output is not safe in CBs.
      // Need to think of a different solution.

      OEMNet_ProcessQoSEvent(pNet);
   }
}

#endif // OEM_PROVIDES_QOS

/*===========================================================================

FUNCTION OEMNet_RequestQoS

DESCRIPTION
  Request a QoS.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_RequestQoS
(
   OEMCONTEXT           netd,
   oem_qos_spec_type   *pQoS,
   oem_qos_handle_type *pHandle
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet                           *pNet = (OEMNet*)netd;
   int16                             ret;
   sint15                            ndssErr;
   dss_iface_ioctl_qos_request_type *pqos_Request;
   int                               result;

   if ((NULL == pQoS) || (NULL == pHandle)) {
      return AEE_NET_EINVAL;
   }

   pqos_Request = sys_malloc(sizeof(dss_iface_ioctl_qos_request_type));
   if (!pqos_Request) {
      return AEE_NET_ENOMEM;
   }

   InitAMSSSpec(&pqos_Request->qos);

   ret = TranslateOEMQoSSpecToAMSS(pQoS, &pqos_Request->qos);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSSpec translation failed! n:%d", pNet));
      FreeAMSSSpecContents(&pqos_Request->qos);
      sys_free(pqos_Request);
      return ret;
   }

   // The following cast is needed as the dss_iface_ioctl_event_cb type
   // changed somewhere in AMSS releases.
   // In older AMSS releases, QoS is not supported and this callback
   // will never be used.
   pqos_Request->cback_fn  = (dss_iface_ioctl_event_cb)OEMNet_QoSEventOccurred;
   pqos_Request->user_data = pNet;
   pqos_Request->app_id    = pNet->sAppID;

   result = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_QOS_REQUEST,
                               pqos_Request,
                               &ndssErr);

   if (DSS_SUCCESS == result) {
      *pHandle = pqos_Request->handle;
      ret = AEE_NET_SUCCESS;
   } else {
      SetOEMQoSSpecErrorMaskFromAMSS(pQoS, &pqos_Request->qos);
      ret = DSSToAEE_Err(ndssErr);
   }

   FreeAMSSSpecContents(&pqos_Request->qos);
   sys_free(pqos_Request);

   return ret;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_ModifyQoS

DESCRIPTION
  Modify a QoS.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_ModifyQoS
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   oem_qos_spec_type   *pQoS
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet                           *pNet = (OEMNet*)netd;
   int16                             ret;
   sint15                            ndssErr;
   dss_iface_ioctl_qos_modify_type *pqos_Modify;
   int                               result;

   pqos_Modify = sys_malloc(sizeof(dss_iface_ioctl_qos_modify_type));
   if (!pqos_Modify) {
      return AEE_NET_ENOMEM;
   }

   InitAMSSSpec(&pqos_Modify->qos);

   ret = TranslateOEMQoSSpecToAMSS(pQoS, &pqos_Modify->qos);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSSpec translation failed! n:%d", pNet));
      FreeAMSSSpecContents(&pqos_Modify->qos);
      sys_free(pqos_Modify);
      return ret;
   }

   pqos_Modify->handle  = handle;

   result = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_QOS_MODIFY,
                               pqos_Modify,
                               &ndssErr);

   if (DSS_SUCCESS == result) {
      ret = AEE_NET_SUCCESS;
   } else {
      SetOEMQoSSpecErrorMaskFromAMSS(pQoS, &pqos_Modify->qos);
      ret = DSSToAEE_Err(ndssErr);
   }

   FreeAMSSSpecContents(&pqos_Modify->qos);
   sys_free(pqos_Modify);

   return ret;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_RequestQoSBundle

DESCRIPTION
  Request QoS Bundle.
  The caller is required to allocate the necessary memory to store the
  return QoS handles

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_RequestQoSBundle
(
   OEMCONTEXT           netd,
   oem_qos_spec_type   **ppQoSSpecs,
   oem_qos_handle_type *pHandles,
   uint32 uNumQoSSpecs,
   AEEQoSBundleRequestOpcode opcode
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet   *pNet = (OEMNet*)netd;
   int16    ret;
   sint15   ndssErr;
   qos_spec_type *pAMSSQoSSpecs;
   dss_iface_ioctl_qos_request_ex_type *pQoSRequestEx;
   int      result;


   if ((NULL == ppQoSSpecs) || (NULL == pHandles) ||
       (AEE_QOS_BUNDLE_REQUEST_OP != opcode &&
        AEE_QOS_BUNDLE_CONFIGURE_OP != opcode)) {
      return EBADPARM;
   }

   if (uNumQoSSpecs > MAX_UINT8) {
      // Validate number of requested QoS Specs
      return AEE_NET_EBADOPTLEN;
   }

   pQoSRequestEx = sys_malloc(sizeof(dss_iface_ioctl_qos_request_ex_type));
   if (!pQoSRequestEx) {
      return AEE_NET_ENOMEM;
   }

   ret = TranslateOEMQoSSpecListToAMSS(ppQoSSpecs, &pAMSSQoSSpecs,uNumQoSSpecs);
   if (AEE_NET_SUCCESS != ret) {
      sys_free(pQoSRequestEx);
      return ret;
   }

   pQoSRequestEx->app_id   = pNet->sAppID;
   pQoSRequestEx->num_qos_specs     = (uint8)uNumQoSSpecs;
   pQoSRequestEx->qos_control_flags = 0;
   pQoSRequestEx->qos_specs_ptr     = pAMSSQoSSpecs;
   pQoSRequestEx->cback_fn    = (dss_iface_ioctl_event_cb)OEMNet_QoSEventOccurred;
   pQoSRequestEx->user_data   = pNet;
   pQoSRequestEx->handles_ptr = (dss_qos_handle_type*)pHandles;
   if (AEE_QOS_BUNDLE_REQUEST_OP == opcode) {
      pQoSRequestEx->opcode = DSS_IFACE_IOCTL_QOS_REQUEST_OP;
   } else { // AEE_QOS_BUNDLE_CONFIGURE_OP
      pQoSRequestEx->opcode = DSS_IFACE_IOCTL_QOS_CONFIGURE_OP;
   }

   result = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_QOS_REQUEST_EX,
                               pQoSRequestEx,
                               &ndssErr);


   if (DSS_SUCCESS != result) {
      SetOEMQoSSpecListErrorMasksFromAMSS(ppQoSSpecs, pAMSSQoSSpecs, uNumQoSSpecs);
      ret = DSSToAEE_Err(ndssErr);
   }
   else {
      ret = AEE_NET_SUCCESS;
   }
   FreeAMSSSpecListContents(pAMSSQoSSpecs, uNumQoSSpecs);
   sys_free(pQoSRequestEx);

   return ret;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}
/*===========================================================================

FUNCTION OEMNet_ReleaseQoS

DESCRIPTION
  Release a granted QoS.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_ReleaseQoS
(
   OEMCONTEXT          netd,
   oem_qos_handle_type handle
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet                          *pNet = (OEMNet*)netd;
   sint15                           ndssErr;
   dss_iface_ioctl_qos_release_type qos_release;
   int ret;

   qos_release.handle = handle;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_QOS_RELEASE,
                            &qos_release,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_ReleaseQoSBundle

DESCRIPTION
  Release bundle of granted QoSs.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_ReleaseQoSBundle
(
   OEMCONTEXT          netd,
   oem_qos_handle_type* pHandles,
   uint32 uNumHandles
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet   *pNet = (OEMNet*)netd;
   sint15   ndssErr;
   dss_iface_ioctl_qos_release_ex_type qos_release_ex;
   int      ret;

   if (NULL == pHandles) {
      return AEE_NET_EINVAL;
   }

   if (uNumHandles > MAX_UINT8) {
      // Validate number of QoS Specs to release
      return AEE_NET_EBADOPTLEN;
   }

   qos_release_ex.handles_ptr = (dss_qos_handle_type*)pHandles;
   qos_release_ex.num_handles = (uint8)uNumHandles;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_QOS_RELEASE_EX,
                            &qos_release_ex,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_GetQoSFlowSpec

DESCRIPTION
  Retrieve the flow specification of a granted Quality of Service instance,
  identified by its handle value

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_GetQoSFlowSpec
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   oem_ip_flow_type    *pRxFlow,
   oem_ip_flow_type    *pTxFlow
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet                                *pNet = (OEMNet*)netd;
   int16                                  ret;
   sint15                                 ndssErr;
   dss_iface_ioctl_qos_get_flow_spec_type qos_getFlowSpec;
   int result;

   if ((NULL == pRxFlow) || (NULL == pTxFlow)) {
      return AEE_NET_EINVAL;
   }

   qos_getFlowSpec.handle = handle;

   result = OEMNet_Iface_Ioctl(pNet,
                               OEM_DSS_IFACE_IOCTL_QOS_GET_FLOW_SPEC,
                               &qos_getFlowSpec,
                               &ndssErr);
   if (DSS_SUCCESS != result) {
      return DSSToAEE_Err(ndssErr);
   }

   ret = TranslateAMSSFlowToOEM(&qos_getFlowSpec.rx_flow, pRxFlow);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSflow rx translation failed! n:%d h:%d", pNet, handle));
      return ret;
   }

   ret = TranslateAMSSFlowToOEM(&qos_getFlowSpec.tx_flow, pTxFlow);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSflow tx translation failed! n:%d h:%d", pNet, handle));
      return ret;
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_GetGrantedQoSFlowSpecDuringAnyState

DESCRIPTION
  Retrieve the flow specification of a granted Quality of Service instance,
  identified by its handle value. Unlike OEMNet_GetQoSFlowSpec(), this function
  will return the spec even if the QoS is currently not active, as long as
  the QoS is still available.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_GetGrantedQoSFlowSpecDuringAnyState
(
   OEMCONTEXT           netd,
   oem_qos_handle_type  handle,
   oem_ip_flow_type    *pRxFlow,
   oem_ip_flow_type    *pTxFlow
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNet   *pNet = (OEMNet*)netd;
   int16    ret;
   sint15   ndssErr;
   dss_iface_ioctl_qos_get_granted_flow_spec2_type qos_getGrantedFlowSpec2;
   int      result;

   if ((NULL == pRxFlow) || (NULL == pTxFlow)) {
      return AEE_NET_EINVAL;
   }

   qos_getGrantedFlowSpec2.handle = handle;

   result = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_QOS_GET_GRANTED_FLOW_SPEC2,
                               &qos_getGrantedFlowSpec2,
                               &ndssErr);
   if (DSS_SUCCESS != result) {
      return DSSToAEE_Err(ndssErr);
   }

   // translate the AMSS Rx and Tx flows to OEM flows
   ret = TranslateAMSSFlowToOEM(&qos_getGrantedFlowSpec2.rx_flow, pRxFlow);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSflow rx translation failed! n:%d h:%d", pNet, handle));
      return ret;
   }

   ret = TranslateAMSSFlowToOEM(&qos_getGrantedFlowSpec2.tx_flow, pTxFlow);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSflow tx translation failed! n:%d h:%d", pNet, handle));
      return ret;
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_GetQoSStatus

DESCRIPTION
  Returns the status of a QoS instance

DEPENDENCIES
  None.
===========================================================================*/
AEEQoSStatus OEMNet_GetQoSStatus
(
   OEMCONTEXT          netd,
   oem_qos_handle_type handle
)
{
#ifdef OEM_PROVIDES_QOS

   dss_iface_id_type  iface_id;
   AEEQoSStatus       ret = AEE_QOS_STATUS_NOT_AVAILABLE;

   if ((iface_id = dss_get_iface_id_by_qos_handle(handle)) != DSS_IFACE_INVALID_ID) {
      dss_iface_ioctl_qos_get_status_type qos_status_type;
      sint15 ndssErr;

      qos_status_type.handle = handle;
      ret = oemdss_qos_iface_ioctl(iface_id,
                                   DSS_IFACE_IOCTL_QOS_GET_STATUS,
                                   &qos_status_type,
                                   &ndssErr);
      if (DSS_SUCCESS == ret) {
         switch (qos_status_type.qos_status) {

            case QOS_STATE_INVALID:
               ret = AEE_QOS_STATUS_NOT_AVAILABLE;
               break;

            case QOS_UNAVAILABLE:
               ret = AEE_QOS_STATUS_NOT_AVAILABLE;
               break;

            case QOS_ACTIVATING:
               ret = AEE_QOS_STATUS_GOING_ACTIVE;
               break;

            case QOS_AVAILABLE:
               ret = AEE_QOS_STATUS_ACTIVE;
               break;

            case QOS_SUSPENDING:
               ret = AEE_QOS_STATUS_GOING_INACTIVE;
               break;

            case QOS_DEACTIVATED:
               ret = AEE_QOS_STATUS_INACTIVE;
               break;

            case QOS_RELEASING:
               ret = AEE_QOS_STATUS_NOT_AVAILABLE;
               break;

            default:
               ret = AEE_QOS_STATUS_NOT_AVAILABLE;
         }
      } else {
         ret = AEE_QOS_STATUS_NOT_AVAILABLE;
      }
   }

   return ret;

#else
   return AEE_QOS_STATUS_NOT_AVAILABLE;
#endif // OEM_PROVIDES_QOS
}
/*===========================================================================

FUNCTION OEMNet_QoSGoActive

DESCRIPTION
  Tries to make a QoS instance go active

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_QoSGoActive
(
   OEMCONTEXT          netd,
   oem_qos_handle_type handle
)
{
#ifdef OEM_PROVIDES_QOS

   dss_iface_id_type iface_id;
   int               ret;
   sint15            ndssErr;
   dss_iface_ioctl_qos_resume_type qos_resume;

   if ((iface_id = dss_get_iface_id_by_qos_handle(handle)) == DSS_IFACE_INVALID_ID) {
      return AEE_NET_ENETNONET;
   }

   qos_resume.handle = handle;
   ret = oemdss_qos_iface_ioctl(iface_id,
                                DSS_IFACE_IOCTL_QOS_RESUME,
                                &qos_resume,
                                &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_QoSDeactivate

DESCRIPTION
   Tries to suspend a QoS Session instance

DEPENDENCIES
   None.
===========================================================================*/
int16 OEMNet_QoSDeactivate
(
   OEMCONTEXT          netd,
   oem_qos_handle_type handle
)
{
#ifdef OEM_PROVIDES_QOS
   int      ret;
   sint15   ndssErr;
   dss_iface_ioctl_qos_suspend_type qos_suspend;
   dss_iface_id_type                iface_id;

   if (DSS_IFACE_INVALID_ID == (iface_id = dss_get_iface_id_by_qos_handle(handle))) {
      return AEE_NET_ENETNONET;
   }

   qos_suspend.handle = handle;
   ret = oemdss_qos_iface_ioctl(iface_id,
                                DSS_IFACE_IOCTL_QOS_SUSPEND,
                                &qos_suspend,
                                &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_QoSBundleGoActive

DESCRIPTION
  Tries to make a QoS instance go active

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_QoSBundleGoActive
(
   OEMCONTEXT          netd,
   oem_qos_handle_type* pHandles,
   uint32 uNumHandles
)
{
#ifdef OEM_PROVIDES_QOS
   int      ret;
   sint15   ndssErr;
   dss_iface_ioctl_qos_resume_ex_type qos_resume_ex;
   dss_iface_id_type                   iface_id;

   if (NULL == pHandles) {
      return EBADPARM;
   }

   if (uNumHandles > MAX_UINT8) {
      // Validate number of QoS Specs to activate
      return AEE_NET_EBADOPTLEN;
   }

   if (DSS_IFACE_INVALID_ID ==
       (iface_id = dss_get_iface_id_by_qos_handle(*(dss_qos_handle_type*)pHandles))) {
      return AEE_NET_ENETNONET;
   }

   qos_resume_ex.num_handles = (uint8)uNumHandles;
   qos_resume_ex.handles_ptr = (dss_qos_handle_type*)pHandles;
   ret = oemdss_qos_iface_ioctl(iface_id,
                                DSS_IFACE_IOCTL_QOS_RESUME_EX,
                                &qos_resume_ex,
                                &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_QoSBundleDeactivate

DESCRIPTION
   Tries to suspend multiple QoSs

DEPENDENCIES
   None.
===========================================================================*/
int16 OEMNet_QoSBundleDeactivate
(
   OEMCONTEXT          netd,
   oem_qos_handle_type* pHandles,
   uint32 uNumHandles
)
{
#ifdef OEM_PROVIDES_QOS
   int      ret;
   sint15   ndssErr;
   dss_iface_ioctl_qos_suspend_ex_type qos_suspend_ex;
   dss_iface_id_type                   iface_id;

   if (NULL == pHandles) {
      return EBADPARM;
   }

   if (uNumHandles > MAX_UINT8) {
      // Validate number of QoS Specs to deactivate
      return AEE_NET_EBADOPTLEN;
   }

   if (DSS_IFACE_INVALID_ID ==
       (iface_id = dss_get_iface_id_by_qos_handle(*(dss_qos_handle_type*)pHandles))) {
      return AEE_NET_ENETNONET;
   }

   qos_suspend_ex.num_handles = (uint8)uNumHandles;
   qos_suspend_ex.handles_ptr = (dss_qos_handle_type*)pHandles;
   ret = oemdss_qos_iface_ioctl(iface_id,
                                DSS_IFACE_IOCTL_QOS_SUSPEND_EX,
                                &qos_suspend_ex,
                                &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}


/*===========================================================================

FUNCTION OEMNet_SetQoSDormancyTimeout

DESCRIPTION
   Set a QoS inactivity timeout value (idle timeout after which the QoS
   should enter dormancy)

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_SetQoSDormancyTimeout
(
   OEMCONTEXT          netd,
   oem_qos_handle_type handle,
   uint8               ucSecs
)
{
#ifdef OEM_PROVIDES_QOS

   uint32             dssSecs = ucSecs;
   dss_iface_id_type  iface_id;
   int16              ndssErr;
   int                ret;

   if ((iface_id = dss_get_iface_id_by_qos_handle(handle)) == DSS_IFACE_INVALID_ID) {
      return AEE_NET_ENETNONET;
   }

   ret = oemdss_iface_ioctl(iface_id,
                            DSS_IFACE_IOCTL_707_SET_DORM_TIMER,
                            &dssSecs,
                            &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_GetQoSEventInfo

DESCRIPTION
  Retrieve QoS event information

DEPENDENCIES
  None.
===========================================================================*/
int32 OEMNet_GetQoSEventInfo(
   OEMCONTEXT           netd,
   oem_qos_handle_type *pHandle,
   AEEQoSEvent         *pEvent,
   AEEQoSInfoCode      *pInfoCode
)
{
#ifdef OEM_PROVIDES_QOS

   OEMNetQoSEventData *pqeBuffer;
   OEMNet             *pNet = (OEMNet*)netd;
   int32               ret;

   if ((NULL == pHandle) || (NULL == pEvent) || (NULL == pInfoCode)) {
      return AEE_NET_EINVAL;
   }

   INTLOCK();
   pqeBuffer = GetQoSEventBufFromHead(&pNet->pqeReadyBuffers);
   INTFREE();

   if (pqeBuffer) {

      ret = AEE_NET_SUCCESS;
      *pHandle = (oem_qos_handle_type)pqeBuffer->handle;

      switch (pqeBuffer->event)
      {
         case DSS_IFACE_IOCTL_QOS_AVAILABLE_EV:
            *pEvent = AEE_QOS_AVAILABLE_EV;
            break;

         case DSS_IFACE_IOCTL_QOS_AVAILABLE_MODIFIED_EV:
            *pEvent = AEE_QOS_AVAILABLE_MODIFIED_EV;
            break;

         case DSS_IFACE_IOCTL_QOS_AVAILABLE_DEACTIVATED_EV:
            *pEvent = AEE_QOS_AVAILABLE_DEACTIVATED_EV;
            break;

         case DSS_IFACE_IOCTL_QOS_UNAVAILABLE_EV:
            *pEvent = AEE_QOS_UNAVAILABLE_EV;
            break;

#ifdef OEM_SUPPORTS_QOS_MODIFY
         case DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV:
            *pEvent = AEE_QOS_MODIFY_ACCEPTED_EV;
            break;

         case DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV:
            *pEvent = AEE_QOS_MODIFY_REJECTED_EV;
            break;
#endif // OEM_SUPPORTS_QOS_MODIFY

         case DSS_IFACE_IOCTL_QOS_INFO_CODE_UPDATED_EV:
            *pEvent = AEE_QOS_INFO_CODE_UPDATED_EV;
            break;

         default:
            // Set the event type to "-1" to indicated to the caller that this
            // is an unsupported event
            *pEvent = -1;
            FARF(ALWAYS, ("unknown QoS event value: %d", pqeBuffer->event));
            break;
      }

      switch ((uint32)pqeBuffer->infoCode)
      {
         case DSS_IFACE_IOCTL_EIC_NOT_SPECIFIED:
            *pInfoCode = AEE_QOS_INFO_CODE_NOT_SPECIFIED;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_NOT_SUPPORTED:
            *pInfoCode = AEE_QOS_INFO_CODE_NOT_SUPPORTED;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_NOT_AVAILABLE:
            *pInfoCode = AEE_QOS_INFO_CODE_NOT_AVAILABLE;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_NOT_GUARANTEED:
            *pInfoCode = AEE_QOS_INFO_CODE_NOT_GUARANTEED;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_INVALID_PARAMS:
            *pInfoCode = AEE_QOS_INFO_CODE_INVALID_PARAMS;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_INTERNAL_ERROR:
            *pInfoCode = AEE_QOS_INFO_CODE_INTERNAL_ERROR;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_INSUFFICIENT_NET_RESOURCES:
            *pInfoCode = AEE_QOS_INFO_CODE_INSUFFICIENT_NET_RESOURCES;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_INSUFFICIENT_LOCAL_RESOURCES:
            *pInfoCode = AEE_QOS_INFO_CODE_INSUFFICIENT_LOCAL_RESOURCES;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_AWARE_SYSTEM:
            *pInfoCode = AEE_QOS_INFO_CODE_QOS_AWARE_SYSTEM;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_UNAWARE_SYSTEM:
            *pInfoCode = AEE_QOS_INFO_CODE_QOS_UNAWARE_SYSTEM;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_REJECTED_OPERATION:
            *pInfoCode = AEE_QOS_INFO_CODE_QOS_REJECTED_OPERATION;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_TIMED_OUT_OPERATION:
            *pInfoCode = AEE_QOS_INFO_CODE_QOS_TIMED_OUT_OPERATION;
            break;

         case DSS_IFACE_IOCTL_EIC_QOS_WILL_GRANT_WHEN_QOS_RESUMED:
            *pInfoCode = AEE_QOS_INFO_CODE_QOS_WILL_GRANT_WHEN_QOS_RESUMED;
            break;

         default:
            *pInfoCode = AEE_QOS_INFO_CODE_NOT_SPECIFIED;
            FARF(ALWAYS, ("unknown QoS info code value: %d", pqeBuffer->infoCode));
            break;
      }

      INTLOCK();
      PutQoSEventBufAtHead(&pNet->pqeFreeBuffers, pqeBuffer);
      INTFREE();
   } else {
      ret = ENOSUCH;
   }

   return ret;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_QOS
}

/*===========================================================================

FUNCTION OEMNet_CloseAllNets

DESCRIPTION
   Close all network connections

DEPENDENCIES
  None.
===========================================================================*/
void OEMNet_CloseAllNets(void)
{
   OEMNet* pNet;

   DBGPRINTF("***zzg OEMNet_CloseAllNets***");

   for (pNet = gNetList.pNets; pNet; pNet = pNet->pNext) 
   {
      OEMNet_PPPClose((OEMCONTEXT)pNet);
   }
}

/*===========================================================================

FUNCTION OEMNet_CloseSiblingNets

DESCRIPTION
   Close all network connections which uses the same iface_id as netd.

DEPENDENCIES
  None.
===========================================================================*/
void OEMNet_CloseSiblingNets(OEMCONTEXT netd)
{
   OEMNet* pNet = (OEMNet*)netd;
   OEMNet* pCurNet;

   DBGPRINTF("***zzg OEMNet_CloseSiblingNets***");

   for (pCurNet = gNetList.pNets; pCurNet; pCurNet = pCurNet->pNext) {
      if (pNet->app_iface_id == pCurNet->app_iface_id) {
         OEMNet_PPPClose((OEMCONTEXT)pCurNet);
      }
   }
}

/*===========================================================================

FUNCTION OEMNet_GoNull

DESCRIPTION
   Terminate PPP with a release reason

DEPENDENCIES
   None.
===========================================================================*/
int16 OEMNet_GoNull(OEMCONTEXT netd, int nReason)
{
   OEMNet* pNet = (OEMNet*)netd;
   dss_iface_ioctl_null_arg_type *pArg = (dss_iface_ioctl_null_arg_type*)&nReason;
   int nRet;
   sint15 dss_errno;

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GO_NULL,
                             pArg,
                             &dss_errno);
   if (DSS_ERROR == nRet) {
      return DSSToAEE_Err(dss_errno);
   }
   return AEE_NET_SUCCESS;
}


/*===========================================================================

                      OEM NET MULTICAST FUNCTION DEFINITIONS

 ===========================================================================*/

#ifdef OEM_PROVIDES_MCAST

/*===========================================================================
 Insert a buffer into a Mcast event list (at the head of the list)
===========================================================================*/
static void PutMcastEventBufAtHead(OEMNetMcastEventData **ppListHead,
                                   OEMNetMcastEventData *pBuffer)
{
   pBuffer->pNext = *ppListHead;
   *ppListHead = pBuffer;
}

/*===========================================================================
 Insert a buffer into a Mcast event list (at the tail of the list)
===========================================================================*/
static void PutMcastEventBufAtTail(OEMNetMcastEventData **ppListHead,
                                   OEMNetMcastEventData *pBuffer)
{
   while ( *ppListHead ) {
      ppListHead = &((*ppListHead)->pNext);
   }
   *ppListHead = pBuffer;
}

/*===========================================================================
 Retrieve a buffer from a Mcast event list (from the head of the list)
===========================================================================*/
static OEMNetMcastEventData* GetMcastEventBufFromHead(OEMNetMcastEventData **ppListHead )
{
   OEMNetMcastEventData *pResult = *ppListHead;

   if ( pResult ) {
      *ppListHead = pResult->pNext;
      pResult->pNext = NULL;
   }

   return pResult;
}

//
// callback function for Mcast events
//
static void OEMNet_McastEventOccurred
(
 dss_iface_ioctl_event_enum_type        event,
 dss_iface_ioctl_event_info_union_type  event_info,
 void                                  *user_data,
 sint15                                 app_id,
 dss_iface_id_type                      iface_id
 )
{
   OEMNet *pNet = OEMNet_FromAppID(app_id); // is this safe ?

   if ((OEMNet*)0 != pNet) {

      OEMNetMcastEventData *pBuffer;

      INTLOCK();
      pBuffer = GetMcastEventBufFromHead(&pNet->peFreeBuffers);
      INTFREE();

      if ( pBuffer ) {

         pBuffer->handle = event_info.mcast_info.handle;
         pBuffer->info_code = event_info.mcast_info.info_code;
         pBuffer->event = event;

         INTLOCK();
         PutMcastEventBufAtTail(&pNet->peReadyBuffers, pBuffer);
         INTFREE();
      } // else multicast event queue is empty!

      // TODO: in case of an empty multicast event queue,
      // should issue a printing.
      // However, as debug output is not safe in CBs DS we avoid it here.

      OEMNet_ProcessMcastEvent(pNet);
   }
}

#endif // OEM_PROVIDES_MCAST

/*===========================================================================

FUNCTION OEMNet_McastLeave

DESCRIPTION
  Leaves a multicast session - see OEMSock.h

===========================================================================*/
int16 OEMNet_McastLeave
(
   OEMCONTEXT            netd,
   oem_mcast_handle_type handle
)
{
#ifdef OEM_PROVIDES_MCAST

   OEMNet                          *pNet = (OEMNet*)netd;
   sint15                           ndssErr;
   int                              ret;
   dss_iface_ioctl_mcast_leave_type mcast_leave;

   mcast_leave.handle = handle;
   mcast_leave.app_id = pNet->sAppID;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MCAST_LEAVE,
                            &mcast_leave,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return EUNSUPPORTED;
#endif // OEM_PROVIDES_MCAST
}

/*===========================================================================

FUNCTION OEMNet_McastLeaveEx

DESCRIPTION
   Leaves a multicast bundle - see OEMSock.h

===========================================================================*/
int16 OEMNet_McastLeaveEx
(
   OEMCONTEXT             netd,
   oem_mcast_handle_type  aHandles[],
   int                    nNumOfRequests
)
{
#ifdef OEM_PROVIDES_MCAST

   OEMNet                             *pNet = (OEMNet*)netd;
   sint15                              ndssErr;
   int                                 ret;
   dss_iface_ioctl_mcast_leave_ex_type mcast_leave_ex;
   int                                 i;

   for (i = 0; i < nNumOfRequests; i++) {
      mcast_leave_ex.handle[i] = aHandles[i];
   }
   mcast_leave_ex.num_flows = nNumOfRequests;
   mcast_leave_ex.dss_nethandle = (int16)(pNet->sAppID);

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MCAST_LEAVE_EX,
                            &mcast_leave_ex,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return EUNSUPPORTED;
#endif // OEM_PROVIDES_MCAST
}

/*===========================================================================

FUNCTION OEMNet_GetMcastEventInfo

DESCRIPTION
  Retrieve multicast event information - see OEMSock.h

DEPENDENCIES
  None.
===========================================================================*/
int32 OEMNet_GetMcastEventInfo(
   OEMCONTEXT             netd,
   oem_mcast_handle_type *pHandle,
   AEEMcastEvent         *pEvent,
   AEEMcastInfoCode      *pInfoCode,
   boolean               *pbIsDeprecatedInfoCode
)
{
#ifdef OEM_PROVIDES_MCAST

   OEMNetMcastEventData *pBuffer;
   OEMNet             *pNet = (OEMNet*)netd;
   int32               ret;

   if ((NULL == pHandle) || (NULL == pEvent) || (NULL == pInfoCode)) {
      return AEE_NET_EINVAL;
   }

   INTLOCK();
   pBuffer = GetMcastEventBufFromHead(&pNet->peReadyBuffers);
   INTFREE();

   if (pBuffer) {

      ret = AEE_NET_SUCCESS;
      *pHandle = (oem_mcast_handle_type)pBuffer->handle;

      switch (pBuffer->event)
      {
         case DSS_IFACE_IOCTL_MCAST_REGISTER_SUCCESS_EV:
            *pEvent = AEE_MCAST_REGISTER_SUCCESS_EV;
            break;

         case DSS_IFACE_IOCTL_MCAST_REGISTER_FAILURE_EV:
            *pEvent = AEE_MCAST_REGISTER_FAILURE_EV;
            break;

         case DSS_IFACE_IOCTL_MCAST_DEREGISTERED_EV:
            *pEvent = AEE_MCAST_DEREGISTERED_EV;
            break;

         case DSS_IFACE_IOCTL_MCAST_STATUS_EV:
            *pEvent = AEE_MCAST_STATUS_EV;
            break;

         default:
            *pEvent = -1;
            ret = AEE_NET_ERROR; /* consider this an unusable event! */
            FARF(ALWAYS, ("unknown multicast event value: %d", pBuffer->event));
            break;
      }

      if ((uint32)pBuffer->info_code < DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_DEPRECATED_INFO_CODE) {
         *pbIsDeprecatedInfoCode = TRUE;
      }
      else {
         *pbIsDeprecatedInfoCode = FALSE;
      }

      switch ((uint32)pBuffer->info_code)
      {
         case DSS_IFACE_IOCTL_MCAST_IC_NOT_SPECIFIED:
            *pInfoCode = AEE_MCAST_INFO_CODE_NOT_SPECIFIED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_MONITORED:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_MONITORED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_UNABLE_TO_MONITOR:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_UNABLE_TO_MONITOR;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_ID_NOT_FOUND;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_NO_MAPPING:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_NO_MAPPING;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT;
            break;

         case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_CANCELLED:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_CANCELLED;
            break;

         case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_UNAVAILABLE:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_UNAVAILABLE;
            break;

         case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_TIMEOUT:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_TIMEOUT;
            break;

         case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_LOST:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_LOST;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_AVAILABLE;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_NOT_TRANSMITTED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_AN_REJECT_INVALID_AUTH_SIG;
            break;

         case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_REQUESTED:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_REQUESTED;
            break;

         case DSS_IFACE_IOCTL_BCMCS_FLOW_STATUS_SYS_UNAVAILABLE :
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_SYS_UNAVAILABLE;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_MAX_FLOWS_REACHED:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_MAX_FLOWS_REACHED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_CANCELLED:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_CANCELLED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_UNABLE_TO_MONITOR:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_UNABLE_TO_MONITOR;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_TIMEOUT:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_TIMEOUT;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_LOST:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_LOST;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_SYS_UNAVAILABLE:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_SYS_UNAVAILABLE;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_AVAILABLE:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_AN_REJECT_NOT_AVAILABLE;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_NOT_TRANSMITTED:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_AN_REJECT_NOT_TRANSMITTED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_AN_REJECT_INVALID_AUTH_SIG:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_AN_REJECT_INVALID_AUTH_SIG;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_UNAVAILABLE:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_UNAVAILABLE;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_NO_MAPPING:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_NO_MAPPING;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_ID_NOT_FOUND_FOR_GIVEN_MULTICAST_IP;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_FLOW_STATUS_REQUESTED:
            *pInfoCode = AEE_MCAST_BCMCS2_INFO_CODE_REQUESTED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS2p0_MAX_FLOWS_REACHED:
            *pInfoCode = AEE_MCAST_BCMCS2_MAX_FLOWS_REACHED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS:
            *pInfoCode = AEE_MCAST_BCMCS_JOIN_REQ_IN_PROGRESS;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_REQUEST_SENT:
            *pInfoCode = AEE_MCAST_BCMCS_FLOW_REQUEST_SENT;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_STATUS_MAX_MONITORED_FLOWS:
            *pInfoCode = AEE_MCAST_BCMCS_INFO_CODE_MAX_MONITORED_FLOWS;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_SUCCESS:
            *pInfoCode = AEE_MCAST_BCMCS_REGISTRATION_SUCCESS;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED:
            *pInfoCode = AEE_MCAST_BCMCS_REGISTRATION_NOT_ALLOWED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_REGISTRATION_FAILED:
            *pInfoCode = AEE_MCAST_BCMCS_REGISTRATION_FAILED;
            break;

         case DSS_IFACE_IOCTL_MCAST_BCMCS_FLOW_DEREGISTERED:
            *pInfoCode = AEE_MCAST_BCMCS_FLOW_DEREGISTERED;

         case DSS_IFACE_IOCTL_MCAST_FLO_IP_OR_PORT_NOT_SUPPORTED:
            *pInfoCode = AEE_MCAST_MFLO_INFO_CODE_IP_OR_PORT_NOT_SUPPORTED;
            break;

         case DSS_IFACE_IOCTL_MCAST_FLO_NO_AUTHORIZATION:
            *pInfoCode = AEE_MCAST_MFLO_INFO_CODE_NO_AUTHORIZATION;
            break;

         case DSS_IFACE_IOCTL_MCAST_FLO_NO_SYSTEM_COVERAGE:
            *pInfoCode = AEE_MCAST_MFLO_INFO_CODE_NO_SYSTEM_COVERAGE;
            break;

         case DSS_IFACE_IOCTL_MCAST_FLO_MAX_FLOW_REACHED:
            *pInfoCode = AEE_MCAST_MFLO_INFO_CODE_MAX_FLOW_REACHED;
            break;

         default:
            *pInfoCode = AEE_MCAST_INFO_CODE_NOT_SPECIFIED;
            FARF(ALWAYS, ("unknown multicast info code value: %d", pBuffer->info_code));
            break;
      }

      INTLOCK();
      PutMcastEventBufAtHead(&pNet->peFreeBuffers, pBuffer);
      INTFREE();
   } else {
      ret = AEE_NET_ERROR;
   }

   return ret;

#else
   return EUNSUPPORTED;
#endif // OEM_PROVIDES_MCAST
}

/*===========================================================================

FUNCTION OEMNet_McastJoin

DESCRIPTION
  Open a multicast session - see OEMSock.h

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_McastJoin(
      OEMCONTEXT             netd,
      AEESockAddrStorage    *psaGroupAddr,
      oem_mcast_handle_type *pHandle)
{
#ifdef OEM_PROVIDES_MCAST

   OEMNet                           *pNet = (OEMNet*)netd;
   int                               ret;
   sint15                            ndssErr;
   dss_iface_ioctl_mcast_join_type   mcast_join_req;

   if ((NULL == psaGroupAddr) || (NULL == pHandle)) {
      return EBADPARM;
   }

   switch(psaGroupAddr->wFamily) {
      case AEE_AF_INET:
         mcast_join_req.ip_addr.type = IPV4_ADDR;
         mcast_join_req.ip_addr.addr.v4 = psaGroupAddr->inet.addr;
         mcast_join_req.port = psaGroupAddr->inet.port;
   	   break;
      case AEE_AF_INET6:
         mcast_join_req.ip_addr.type = IPV6_ADDR;
         memcpy(&(mcast_join_req.ip_addr.addr.v6),
                &(psaGroupAddr->inet6.addr),
                sizeof(mcast_join_req.ip_addr.addr.v6));
         mcast_join_req.port = psaGroupAddr->inet6.port;
         break;
      default:
         return AEE_NET_EAFNOSUPPORT;
   }

   mcast_join_req.mcast_param_ptr = NULL;
   mcast_join_req.event_cb = OEMNet_McastEventOccurred;
   mcast_join_req.user_data_ptr = pNet;
   mcast_join_req.app_id = pNet->sAppID;

   // handle is passed as an "out" parameter. However, we also
   // use it to specify the requested result to the BREW simulator.
   mcast_join_req.handle = *pHandle;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MCAST_JOIN,
                            &mcast_join_req,
                            &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   *pHandle = mcast_join_req.handle;
   return SUCCESS;

#else
   return EUNSUPPORTED;
#endif // OEM_PROVIDES_MCAST
}

/*===========================================================================

FUNCTION OEMNet_McastJoinEx

DESCRIPTION
   Open a multicast bundle - see OEMSock.h

DEPENDENCIES
   None.
===========================================================================*/
int16 OEMNet_McastJoinEx(
      OEMCONTEXT            netd,
      AEESockAddrStorage*   apsaGroupAddresses[],
      oem_mcast_handle_type aHandles[],
      boolean               aSendReg[],
      int                   nNumOfRequests)
{
#ifdef OEM_PROVIDES_MCAST

   OEMNet                              *pNet = (OEMNet*)netd;
   int                                  ret;
   sint15                               ndssErr;
   dss_iface_ioctl_mcast_join_ex_type   mcast_join_ex_req;
   int                                  i;

   if (DSS_IFACE_MAX_MCAST_FLOWS_PER_IOCTL < nNumOfRequests) {
      return AEE_NET_EFAULT;
   }

   for (i = 0; i < nNumOfRequests; i++) {
      switch(apsaGroupAddresses[i]->wFamily) {
         case AEE_AF_INET:
            mcast_join_ex_req.ip_addr[i].type = IPV4_ADDR;
            mcast_join_ex_req.ip_addr[i].addr.v4 = apsaGroupAddresses[i]->inet.addr;
            mcast_join_ex_req.port[i] = apsaGroupAddresses[i]->inet.port;
            break;
         case AEE_AF_INET6:
            mcast_join_ex_req.ip_addr[i].type = IPV6_ADDR;
            memcpy(&(mcast_join_ex_req.ip_addr[i].addr.v6),
                   &(apsaGroupAddresses[i]->inet6.addr),
                   sizeof(mcast_join_ex_req.ip_addr[i].addr.v6));
            mcast_join_ex_req.port[i] = apsaGroupAddresses[i]->inet6.port;
            break;
         default:
            return AEE_NET_EAFNOSUPPORT;
      }
      mcast_join_ex_req.mcast_param_ptr[i] = NULL;
      if (aSendReg[i]) {
         mcast_join_ex_req.mcast_request_flags[i] = DSS_IFACE_IOCTL_MCAST_REG_SETUP_NOT_ALLOWED;
      }
      else {
         mcast_join_ex_req.mcast_request_flags[i] = DSS_IFACE_IOCTL_MCAST_REG_SETUP_ALLOWED;
      }

      // handle is passed as an "out" parameter. However, we also
      // use it to specify the requested result to the BREW simulator.
      mcast_join_ex_req.handle[i] = aHandles[i];
   }

   mcast_join_ex_req.num_flows = nNumOfRequests;
   mcast_join_ex_req.user_data_ptr = pNet;
   mcast_join_ex_req.dss_nethandle = (int16)(pNet->sAppID);
   mcast_join_ex_req.event_cb = OEMNet_McastEventOccurred;

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MCAST_JOIN_EX,
                            &mcast_join_ex_req,
                            &ndssErr);

   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   for (i = 0; i < nNumOfRequests; i++) {
      aHandles[i] = mcast_join_ex_req.handle[i];
   }

   return SUCCESS;

#else
   return EUNSUPPORTED;
#endif // OEM_PROVIDES_MCAST
}

/*===========================================================================

FUNCTION OEMNet_McastRegisterEx

DESCRIPTION
   Registers a multicast bundle - see OEMSock.h

===========================================================================*/
int16 OEMNet_McastRegisterEx
(
   OEMCONTEXT             netd,
   oem_mcast_handle_type  aHandles[],
   int                    nNumOfRequests
)
{
#ifdef OEM_PROVIDES_MCAST

   OEMNet                                *pNet = (OEMNet*)netd;
   sint15                                 ndssErr;
   int                                    ret;
   dss_iface_ioctl_mcast_register_ex_type mcast_register_ex;
   int                                    i;

   for (i = 0; i < nNumOfRequests; i++) {
      mcast_register_ex.handle[i] = aHandles[i];
   }
   mcast_register_ex.num_flows = nNumOfRequests;
   mcast_register_ex.dss_nethandle = (int16)(pNet->sAppID);

   ret = OEMNet_Iface_Ioctl(pNet,
                            DSS_IFACE_IOCTL_MCAST_REGISTER_EX,
                            &mcast_register_ex,
                            &ndssErr);
   if (DSS_SUCCESS != ret) {
      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else
   return EUNSUPPORTED;
#endif // OEM_PROVIDES_MCAST
}

/*===========================================================================

FUNCTION OEMNet_BcmcsDbUpdate

DESCRIPTION
  Update the BCMCS DB of flow mapping information - see IBCMCSDB_Update().

===========================================================================*/
int32 OEMNet_BcmcsDbUpdate
(
   IWebOpts* pBcmcsDbRecord,
   uint32 uUpdateMode
)
{
#ifdef OEM_PROVIDES_BCMCS_DB_UPDATE

   ps_iface_ioctl_bcmcs_db_update_type bcmcs_db_update;
   dss_net_policy_info_type policy_info;
   dss_iface_id_type iface_id;
   sint15 ndssErr;
   WebOpt web_opt;
   IPAddr* ip_addr;
   AEEBCMCSDBRecord_CompositeFlowId* composite_flow_id;
   int composite_id, simple_id;

   if (NULL == pBcmcsDbRecord) {
      return EBADPARM;
   }

   // Initialize to defaults
   dss_init_net_policy_info(&policy_info);

   policy_info.iface.kind = DSS_IFACE_NAME;
   policy_info.iface.info.name = DSS_IFACE_CDMA_BCAST;

   iface_id = dss_get_iface_id_by_policy(policy_info, &ndssErr);
   if (DSS_IFACE_INVALID_ID == iface_id) {
      return DSSToAEE_Err(ndssErr);
   }

   // IP Addr
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_IpAddr, 0, &web_opt)) {
      ip_addr = web_opt.pVal;
      switch(ip_addr->wFamily) {
         case AEE_AF_INET:
            bcmcs_db_update.multicast_ip.type = IPV4_ADDR;
            bcmcs_db_update.multicast_ip.addr.v4 = ip_addr->addr.v4;
   	      break;
         case AEE_AF_INET6:
            bcmcs_db_update.multicast_ip.type = IPV6_ADDR;
            memcpy(&(bcmcs_db_update.multicast_ip.addr.v6),
                     ip_addr->addr.v6.Byte, sizeof(ip_addr->addr.v6.Byte));
   	      break;
         default:
            return EBADITEM;
      }
   }
   else {
      return EBADITEM;
   }

   // Port
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_Port, 0, &web_opt)
       && ((uint32)(web_opt.pVal) <= MAX_UINT16)) {
      // casting (void*) to uint16 makes warning on ADS-1.2 compiler
      bcmcs_db_update.port = (uint16)(uint32)(web_opt.pVal);
   }
   else {
      return EBADITEM;
   }

   // Flow ID - either simple or composite
   simple_id = (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                              AEEBCMCSDBRECORDOPT_SimpleFlowId, 0, &web_opt));
   composite_id = (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                                 AEEBCMCSDBRECORDOPT_CompositeFlowId, 0, &web_opt));

   // program_id length in bits. 0 means program_id is undefined.
   bcmcs_db_update.program_id_len = 0;

   // flow_discrim length in bits. 0 means flow_discrim is undefined.
   bcmcs_db_update.flow_discrim_len = 0;

   if (simple_id && composite_id) {
      return EBADITEM; // simple_id conflicts with composite_id
   }
   else if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_SimpleFlowId, 0, &web_opt)) {
      bcmcs_db_update.flow_id = (uint32)(web_opt.pVal);
      bcmcs_db_update.flow_format = 0;
   }
   else if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                        AEEBCMCSDBRECORDOPT_CompositeFlowId, 0, &web_opt)) {
      composite_flow_id = (AEEBCMCSDBRecord_CompositeFlowId*)(web_opt.pVal);
      bcmcs_db_update.flow_format = 1;
      bcmcs_db_update.flow_id = composite_flow_id->sSimpleFlowId;
      bcmcs_db_update.program_id_len = composite_flow_id->sProgramId.len;
      bcmcs_db_update.program_id = composite_flow_id->sProgramId.val;
      bcmcs_db_update.flow_discrim_len = composite_flow_id->sFlowDiscrim.len;
      bcmcs_db_update.flow_discrim = composite_flow_id->sFlowDiscrim.val;
   }
   else {
      return EBADITEM; // neither simple_id nor composite_id
   }


   // HDR Zone
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_HdrZone, 0, &web_opt)) {

      hdraddr_type subnet = {0};

      // The value of all zeros is reserved for future use.
      if (0 == memcmp(subnet, web_opt.pVal, sizeof(subnet))) {
         return EBADITEM;
      }

      bcmcs_db_update.zone.type = DSBCMCS_ZONE_HDR;
      memcpy(bcmcs_db_update.zone.zone.subnet,
             web_opt.pVal,
             sizeof(bcmcs_db_update.zone.zone.subnet));
   }
   else {
      return EBADITEM;
   }

   // Framing
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_Framing, 0, &web_opt)) {
      switch((uint32)(web_opt.pVal)) {
         case AEEBCMCSDB_FRAMING_SEGMENT:
            bcmcs_db_update.framing = DSBCMCS_FRAMING_SEGMENT;
      	   break;
         case AEEBCMCSDB_FRAMING_HDLC:
            bcmcs_db_update.framing = DSBCMCS_FRAMING_HDLC;
      	   break;
         default:
            return EBADITEM;
      }
   }
   else {
      // 0xFF padding represents "undefined" value.
	   // TODO: replace by DSBCMCS_FRAMING_NOT_SET
      memset(&bcmcs_db_update.framing, 0xFF, sizeof(dsbcmcs_framing_enum_type));
   }

   // Protocol
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_Protocol, 0, &web_opt)) {
      switch((uint32)(web_opt.pVal)) {
         case AEEBCMCSDB_PROTOCOL_PPP:
            bcmcs_db_update.protocol = DSBCMCS_PROTOCOL_PPP;
      	   break;
         case AEEBCMCSDB_PROTOCOL_IPv4:
            bcmcs_db_update.protocol = DSBCMCS_PROTOCOL_IPv4;
      	   break;
         case AEEBCMCSDB_PROTOCOL_IPv6:
            bcmcs_db_update.protocol = DSBCMCS_PROTOCOL_IPv6;
      	   break;
         default:
            return EBADITEM;
      }
   }
   else {
      // 0xFF padding represents "undefined" value.
      // TODO: replace by DSBCMCS_PROTOCOL_NOT_SET
      memset(&bcmcs_db_update.protocol, 0xFF, sizeof(dsbcmcs_protocol_enum_type));
   }

   // CRC Length
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_CrcLen, 0, &web_opt)) {
      switch((uint32)(web_opt.pVal)) {
         case AEEBCMCSDB_CRC_NONE:
            bcmcs_db_update.crc_len = 0;
            break;
         case AEEBCMCSDB_CRC_2BYTES:
            bcmcs_db_update.crc_len = 2;
            break;
         default:
            return EBADITEM;
      }
   }
   else {
      bcmcs_db_update.crc_len = (uint8)(-1);
   }

#if (DSS_VER_FLOW_ID_LEN)
   // Flow ID Length
   if (SUCCESS == IWEBOPTS_GetOpt(pBcmcsDbRecord,
                     AEEBCMCSDBRECORDOPT_FlowIDLen, 0, &web_opt)) {
      uint32 nVal = (uint32)web_opt.pVal;

      if (MAX_UINT8 < nVal) {
         return EBADPARM;
      }
      bcmcs_db_update.flow_id_len = (uint8)nVal;
   }
   else {
      // Use default value for flow_id_len
      bcmcs_db_update.flow_id_len = (uint8)(BCMCS_DB_FLOW_ID_LEN_DEFAULT);
   }
#endif
   // Check whether to overwrite a matching record that already exists
   if (uUpdateMode & AEEBCMCSDB_FLAG_NO_OVERWRITE) {
      bcmcs_db_update.overwrite = FALSE;
   }
   else {
      bcmcs_db_update.overwrite = TRUE;
   }

   if (DSS_SUCCESS != oemdss_iface_ioctl(iface_id,
                                         DSS_IFACE_IOCTL_BCMCS_DB_UPDATE,
                                         &bcmcs_db_update,
                                         &ndssErr)) {
      // TODO: add handling of specific errors
      //   switch(ndssErr) {
      //      case E_DB_OVERFLOW: // TODO
      //   	     return AEE_NET_EBCMCSDB_FULL;
      //      case E_DB_NO_OVERWRITE:
      //   	     return AEE_NET_EBCMCSDB_EXIST;
      //      default:
      //	        return DSSToAEE_Err(ndssErr);
      //   }

      return DSSToAEE_Err(ndssErr);
   }

   return AEE_NET_SUCCESS;

#else

   return EUNSUPPORTED;

#endif // OEM_PROVIDES_BCMCS_DB_UPDATE
}

/*===========================================================================

FUNCTION OEMNet_IsQoSAware

DESCRIPTION
   In case of a CDMA system, this function queries if the mobile is
   currently on a system that support QoS (Aware System).

===========================================================================*/
int16 OEMNet_IsQoSAware(OEMCONTEXT netd, boolean *pbIsQoSAware)
{
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_ON_QOS_AWARE_SYSTEM,
                             (dss_iface_ioctl_on_qos_aware_system_type*) pbIsQoSAware,
                             &dss_errno);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(dss_errno);
   }
   return SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetSupportedQoSProfiles

DESCRIPTION
   This function returns the current QoS Profiles supported by the network.

===========================================================================*/
int16 OEMNet_GetSupportedQoSProfiles(OEMCONTEXT netd,
                                     uint16 awQoSProfiles[],
                                     uint8 uNumProfiles,
                                     uint8* puNumProfilesRequired)
{
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   uint8    uNumProfilesToCopy;
   dss_iface_ioctl_get_network_supported_qos_profiles_type suppQoSProfiles;
   uint32   uNumBytesToCopy;

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GET_NETWORK_SUPPORTED_QOS_PROFILES,
                             &suppQoSProfiles,
                             &dss_errno);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(dss_errno);
   }

   // If required the actual number of supported QoS Profile IDs,
   // update puNumProfilesRequired with the actual value.
   if (NULL != puNumProfilesRequired) {
      *puNumProfilesRequired = suppQoSProfiles.profile_count;
   }

   // If awQoSProfiles can hold all supported Profile IDs then copy all
   // of them (suppQoSProfiles.profile_count). If it is too small to hold all
   // of them copy only the maximum it can contain (uNumProfiles).
   uNumProfilesToCopy = (uNumProfiles > suppQoSProfiles.profile_count) ?
      suppQoSProfiles.profile_count : uNumProfiles;

   if (0 < uNumProfilesToCopy) {
      // protect against buffer overflow - don't copy data 
      // which is out of the bounds of suppQoSProfiles.profile_value[].
      uNumBytesToCopy = MIN(sizeof(suppQoSProfiles.profile_value), 
                            (uNumProfilesToCopy*sizeof(awQoSProfiles[0])));
      memcpy(awQoSProfiles, suppQoSProfiles.profile_value, uNumBytesToCopy);
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetSipServerAddr

DESCRIPTION
   This function returns the SIP (Session Initiation Protocol) server IP
   addresses for this network.

===========================================================================*/
int16 OEMNet_GetSipServerAddr(OEMCONTEXT netd,
                              IPAddr aSipServerAddresses[],
                              uint8  uNumAddresses,
                              uint8* puNumAddressesRequired)
{
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   uint8    uIndex = 0;
   uint8    uNumAddressesToCopy;
   dss_iface_ioctl_sip_serv_addr_info_type ssaSipServerAddresses;

   // Allocate space that should be enough for all SIP server addresses
   // We want to be sure we get all the addresses
   ssaSipServerAddresses.addr_array =
      sys_malloc(sizeof(ip_addr_type)*OEMNET_MAX_SIP_SERVER_NUM_ADDRESSES);
   if (NULL == ssaSipServerAddresses.addr_array) {
      return AEE_NET_ENOMEM;
   }
   // Ask for maximum number of addresses (not limited to user allocated buffer size!)
   ssaSipServerAddresses.count = OEMNET_MAX_SIP_SERVER_NUM_ADDRESSES;

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GET_SIP_SERV_ADDR,
                             &ssaSipServerAddresses,
                             &dss_errno);
   if (DSS_SUCCESS != nRet) {
      sys_free(ssaSipServerAddresses.addr_array);
      return DSSToAEE_Err(dss_errno);
   }

   // If the actual number of SIP server IP addresses was requested
   // update puNumAddressesRequired with the actual value.
   if (NULL != puNumAddressesRequired) {
      *puNumAddressesRequired = (uint8)ssaSipServerAddresses.count;
   }

   // If aSipServerAddresses can hold all SIP addresses then copy all
   // of them (ssaSipServerAddresses.count). If it is too small to hold all
   // of them copy only the maximum it can contain (uNumAddresses).
   uNumAddressesToCopy = (uint8)( (uNumAddresses >= ssaSipServerAddresses.count) ?
      ssaSipServerAddresses.count : uNumAddresses );

   // Copy provided addresses to the output buffer until all provided addresses
   // are copied or the output buffer is full.
   while (uIndex < uNumAddressesToCopy) {

      switch (ssaSipServerAddresses.addr_array[uIndex].type)
      {
         case IPV4_ADDR:
            aSipServerAddresses[uIndex].wFamily = AEE_AF_INET;
            aSipServerAddresses[uIndex].addr.v4 =
               IP_ADDR_V4(ssaSipServerAddresses.addr_array[uIndex]);
            break;

#ifdef FEATURE_DATA_PS_IPV6
         case IPV6_ADDR:
            aSipServerAddresses[uIndex].wFamily = AEE_AF_INET6;
            memcpy(aSipServerAddresses[uIndex].addr.v6.Byte,
                   ssaSipServerAddresses.addr_array[uIndex].addr.v6,
                   sizeof(aSipServerAddresses[uIndex].addr.v6.Byte));
            break;
#endif // FEATURE_DATA_PS_IPV6

         default:
            sys_free(ssaSipServerAddresses.addr_array);
            return AEE_NET_EAFNOSUPPORT;
      }
      uIndex++;
   }

   sys_free(ssaSipServerAddresses.addr_array);
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetSipServerDomainNames

DESCRIPTION
   This function returns the SIP (Session Initiation Protocol) server Domain
   Names for this network.

===========================================================================*/
int16 OEMNet_GetSipServerDomainNames(OEMCONTEXT netd,
                                     AEENetDomainName aSipServerDomainNames[],
                                     uint8  uNumNames,
                                     uint8* puNumNamesRequired)
{
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   uint8    uNumNamesToCopy;
   dss_iface_ioctl_sip_serv_domain_name_info_type ssdSipServerDomainNames;

   // Allocate space that should be enough for all SIP server domain names
   // We want to be sure we get all the names
   ssdSipServerDomainNames.name_array =
      sys_malloc(sizeof(dss_iface_ioctl_domain_name_type)*OEMNET_MAX_SIP_SERVER_DOMAIN_NAMES);
   if (NULL == ssdSipServerDomainNames.name_array) {
      return AEE_NET_ENOMEM;
   }
   // Ask for maximum number of domain names (not limited to user allocated buffer size!)
   ssdSipServerDomainNames.count = OEMNET_MAX_SIP_SERVER_DOMAIN_NAMES;

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GET_SIP_SERV_DOMAIN_NAMES,
                             &ssdSipServerDomainNames,
                             &dss_errno);
   if (DSS_SUCCESS != nRet) {
      sys_free(ssdSipServerDomainNames.name_array);
      return DSSToAEE_Err(dss_errno);
   }

   // If the actual number of SIP server Domain Names was requested
   // update puNumNamesRequired with the actual value.
   if (NULL != puNumNamesRequired) {
      *puNumNamesRequired = (uint8)ssdSipServerDomainNames.count;
   }

   // If aSipServerDomainNames can hold all SIP Domain names then copy all
   // of them (ssdSipServerDomainNames.count). If it is too small to hold all
   // of them copy only the maximum it can contain (uNumNames).
   uNumNamesToCopy = (uint8)( (uNumNames >= ssdSipServerDomainNames.count) ?
      ssdSipServerDomainNames.count : uNumNames );

   if (0 < uNumNamesToCopy) {
      memcpy(aSipServerDomainNames,
             ssdSipServerDomainNames.name_array,
             uNumNamesToCopy*sizeof(aSipServerDomainNames[0]));
   }

   sys_free(ssdSipServerDomainNames.name_array);
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GetHystActTimer

DESCRIPTION
   This function returns the Hysteresis Activation Timer for this network.

===========================================================================*/
int16 OEMNet_GetHystActTimer(OEMCONTEXT netd,
                             int *pnHystActTimer)
{
#if !defined(FEATURE_NO_HAT)
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   dss_iface_ioctl_707_hat_type *pTimer = (dss_iface_ioctl_707_hat_type*)pnHystActTimer;

   if (NULL == pTimer) {
      return AEE_NET_EINVAL;
   }

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_707_GET_HYSTERESIS_ACT_TIMER,
                             (void*)pTimer,
                             &dss_errno);

   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
#else
   return AEE_NET_EOPNOTSUPP;
#endif
}

/*===========================================================================

FUNCTION OEMNet_SetHystActTimer

DESCRIPTION
   This function sets the Hysteresis Activation Timer for this network.

===========================================================================*/
int16 OEMNet_SetHystActTimer(OEMCONTEXT netd,
                             int nHystActTimer)
{
#if !defined(FEATURE_NO_HAT)
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   dss_iface_ioctl_707_hat_type timer = (dss_iface_ioctl_707_hat_type)nHystActTimer;

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_707_SET_HYSTERESIS_ACT_TIMER,
                             (void*)(&timer),
                             &dss_errno);

   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(dss_errno);
   }

   return AEE_NET_SUCCESS;
#else
   return AEE_NET_EOPNOTSUPP;
#endif
}

/*===========================================================================

FUNCTION OEMNet_GetBearerTechnology

DESCRIPTION
   This function returns the bearer technology type for this network.

===========================================================================*/
int16 OEMNet_GetBearerTechnology(OEMCONTEXT netd,
                                 AEEBearerTechType *pAEEBearerTech)
{
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   dss_iface_ioctl_bearer_tech_type BearerTech;

   if (NULL == pAEEBearerTech) {
      return AEE_NET_EINVAL;
   }

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
                             &BearerTech,
                             &dss_errno);

   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(dss_errno);
   }

   // translate DS bearer technology info to AEE bearer technology info
   nRet = DSSToAEE_BearerTechType(&BearerTech, pAEEBearerTech);

   return nRet;
}

/*===========================================================================

FUNCTION OEMNet_GetBearerTechnologyOpts

DESCRIPTION
   This function returns the bearer technology options for this network.

===========================================================================*/
int16 OEMNet_GetBearerTechnologyOpts(OEMCONTEXT netd,
                                     IBearerTechnology *pBearerTechOpts)
{
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   dss_iface_ioctl_bearer_tech_type BearerTech;

   if (NULL == pBearerTechOpts) {
      return AEE_NET_EINVAL;
   }

   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GET_BEARER_TECHNOLOGY,
                             &BearerTech,
                             &dss_errno);
   if (DSS_SUCCESS != nRet) {
      return DSSToAEE_Err(dss_errno);
   }

   // translate DS bearer technology info to AEE bearer technology info
   nRet = DSSToAEE_BearerTechTypeOpts(&BearerTech, pBearerTechOpts);

   return nRet;
}

/*===========================================================================

Function: DSSToAEE_BearerTechType()

Description:
   This function converts a struct of type dss_iface_ioctl_bearer_tech_type
   to a struct of the type AEEBearerTechType

Prototype:
   static int16 DSSToAEE_BearerTechType(dss_iface_ioctl_bearer_tech_type* psDSSBearerTech,
                                        AEEBearerTechType*                psAEEBearerTech)
Parameters:
   psDSSBearerTech: [In] - points to a struct of type dss_iface_ioctl_bearer_tech_type
   psAEEBearerTech: [Out] - points to a struct of type AEEBearerTechType, to be
                            filled by this function

Return Value:
   AEE_NET_SUCCESS: Success. psAEEBearerTech holds the bearer technology info.
   AEE_NET_ERROR: Failure. The input data (in psDSSBearerTech) was invalid.

Comments:
   None

Side Effects:
   None

See Also:
   OEMNet_GetBearerTechnology
===========================================================================*/
static int16 DSSToAEE_BearerTechType(dss_iface_ioctl_bearer_tech_type* psDSSBearerTech,
                                     AEEBearerTechType*                psAEEBearerTech)
{
   int nRet = AEE_NET_SUCCESS;

   // set the AEE struct to zeros
   memset(psAEEBearerTech, 0, sizeof((*psAEEBearerTech)));

   // fill in the technology type info
   switch(psDSSBearerTech->current_network)
   {
      // current network is CDMA
      case PS_IFACE_NETWORK_CDMA:
         psAEEBearerTech->uBearerTech = AEE_BEARER_TECH_CDMA;
         if ((psDSSBearerTech->data.cdma_type.rat_mask) & PS_IFACE_CDMA_1X) {
            psAEEBearerTech->uBearerTechInfo.acbCDMABearerTechInfo.uCDMABearerTech |= AEE_CDMA_BEARER_TECH_1X;
         }
         if ((psDSSBearerTech->data.cdma_type.rat_mask) & PS_IFACE_CDMA_EVDO_REV0) {
            psAEEBearerTech->uBearerTechInfo.acbCDMABearerTechInfo.uCDMABearerTech |= AEE_CDMA_BEARER_TECH_EVDO_REV0;
         }
         if ((psDSSBearerTech->data.cdma_type.rat_mask) & PS_IFACE_CDMA_EVDO_REVA) {
            psAEEBearerTech->uBearerTechInfo.acbCDMABearerTechInfo.uCDMABearerTech |= AEE_CDMA_BEARER_TECH_EVDO_REVA;
         }
         break;

      // current network is UMTS
      case PS_IFACE_NETWORK_UMTS:
         psAEEBearerTech->uBearerTech = AEE_BEARER_TECH_UMTS;
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_WCDMA) {
            psAEEBearerTech->uBearerTechInfo.aubUMTSBearerTechInfo.uUMTSBearerTech |= AEE_UMTS_BEARER_TECH_WCDMA;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_GPRS) {
            psAEEBearerTech->uBearerTechInfo.aubUMTSBearerTechInfo.uUMTSBearerTech |= AEE_UMTS_BEARER_TECH_GPRS;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_HSDPA) {
            psAEEBearerTech->uBearerTechInfo.aubUMTSBearerTechInfo.uUMTSBearerTech |= AEE_UMTS_BEARER_TECH_HSDPA;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_HSUPA) {
            psAEEBearerTech->uBearerTechInfo.aubUMTSBearerTechInfo.uUMTSBearerTech |= AEE_UMTS_BEARER_TECH_HSUPA;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_EDGE) {
            psAEEBearerTech->uBearerTechInfo.aubUMTSBearerTechInfo.uUMTSBearerTech |= AEE_UMTS_BEARER_TECH_EDGE;
         }
         break;

      // current network is WLAN
      case PS_IFACE_NETWORK_WLAN:
         psAEEBearerTech->uBearerTech = AEE_BEARER_TECH_WLAN;
         break;

      default:
         nRet = AEE_NET_ERROR;
         break;
   }

   return nRet;
}

/*===========================================================================

Function: DSSToAEE_BearerTechTypeOpts()

Description:
   This function converts a struct of type dss_iface_ioctl_bearer_tech_type
   to an option list of the type IBearerTechnology.

Prototype:
   static int16 DSSToAEE_BearerTechTypeOpts(dss_iface_ioctl_bearer_tech_type *psDSSBearerTech,
                                            IBearerTechnology                *pBearerTechOpts)
Parameters:
   psDSSBearerTech: [In] - points to a struct of type dss_iface_ioctl_bearer_tech_type
   pBearerTechOpts: [Out] - points to an object of type IBearerTechnology, to be
                            filled by this function

Return Value:
   AEE_NET_SUCCESS: Success. pBearerTechOpts holds the bearer technology options.
   AEE_NET_ERROR: Failure. The input data (in psDSSBearerTech) was invalid.

Comments:
   This function assumes that the IBearerTechnology option list is empty.

Side Effects:
   None

See Also:
   OEMNet_GetBearerTechnologyOpts
===========================================================================*/
static int16 DSSToAEE_BearerTechTypeOpts(dss_iface_ioctl_bearer_tech_type *psDSSBearerTech,
                                         IBearerTechnology                *pBearerTechOpts)
{
   AEEBearerTechnologyOpt aOpts[4];
   int nRet = AEE_NET_SUCCESS;
   int i = 0;

   aOpts[0].nId = AEEBEARERTECHNOLOGYOPT_COPYOPTS;
   aOpts[0].pVal = (void*)TRUE;
   aOpts[1].nId = AEEBEARERTECHNOLOGYOPT_END;
   aOpts[1].pVal = NULL;
   if (SUCCESS != IBEARERTECHNOLOGY_AddOpt(pBearerTechOpts, aOpts)) {
      return EFAILED;
   }

   // fill in the technology type info
   switch(psDSSBearerTech->current_network)
   {
      // current network is CDMA
      case PS_IFACE_NETWORK_CDMA:
      {
         AEEBEARERTECHNOLOGY_Network                btNet = AEE_BEARER_TECH_CDMA;
         AEEBEARERTECHNOLOGY_CDMATypeMask           btCDMAType = 0;
#ifdef BEARER_TECH_CONTAINS_SO_MASK
         AEEBEARERTECHNOLOGY_CDMAServiceOptionsMask btCDMAOpts = 0;
#endif //BEARER_TECH_CONTAINS_SO_MASK
         aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_NETWORK;
         aOpts[i].pVal = (void*) btNet;
         i++;
         if ((psDSSBearerTech->data.cdma_type.rat_mask) & PS_IFACE_CDMA_1X) {
            btCDMAType |= AEEBEARERTECHNOLOGY_CDMA_1X;

#ifdef BEARER_TECH_CONTAINS_SO_MASK
            if ((psDSSBearerTech->data.cdma_type.so_mask) & PS_IFACE_CDMA_1X_IS95) {
               btCDMAOpts |= AEEBEARERTECHNOLOGY_CDMA_1X_IS95;
            }
            if ((psDSSBearerTech->data.cdma_type.so_mask) & PS_IFACE_CDMA_1X_IS2000) {
               btCDMAOpts |= AEEBEARERTECHNOLOGY_CDMA_1X_IS2000;
            }
            if ((psDSSBearerTech->data.cdma_type.so_mask) & PS_IFACE_CDMA_1X_IS2000_REL_A) {
               btCDMAOpts |= AEEBEARERTECHNOLOGY_CDMA_IS2000_REL_A;
            }
#endif //BEARER_TECH_CONTAINS_SO_MASK
         }
         if ((psDSSBearerTech->data.cdma_type.rat_mask) & PS_IFACE_CDMA_EVDO_REV0) {
            btCDMAType |= AEEBEARERTECHNOLOGY_CDMA_EVDO_REV0;
         }
         if ((psDSSBearerTech->data.cdma_type.rat_mask) & PS_IFACE_CDMA_EVDO_REVA) {
            btCDMAType |= AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA;

#ifdef BEARER_TECH_CONTAINS_SO_MASK
            if ((psDSSBearerTech->data.cdma_type.so_mask) & PS_IFACE_CDMA_EVDO_REVA_DPA) {
               btCDMAOpts |= AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_DPA;
            }
            if ((psDSSBearerTech->data.cdma_type.so_mask) & PS_IFACE_CDMA_EVDO_REVA_MFPA) {
               btCDMAOpts |= AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_MFPA;
            }
            if ((psDSSBearerTech->data.cdma_type.so_mask) & PS_IFACE_CDMA_EVDO_REVA_EMPA) {
               btCDMAOpts |= AEEBEARERTECHNOLOGY_CDMA_EVDO_REVA_EMPA;
            }
#endif //BEARER_TECH_CONTAINS_SO_MASK
         }
         aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_CDMA_TYPE;
         aOpts[i].pVal = (void*) btCDMAType;
         i++;
#ifdef BEARER_TECH_CONTAINS_SO_MASK
         aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_CDMA_SERVICE_OPTIONS;
         aOpts[i].pVal = (void*) btCDMAOpts;
         i++;
#endif //BEARER_TECH_CONTAINS_SO_MASK
         break;
      }

      // current network is UMTS
      case PS_IFACE_NETWORK_UMTS:
      {
         AEEBEARERTECHNOLOGY_Network       btNet = AEEBEARERTECHNOLOGY_UMTS;
         AEEBEARERTECHNOLOGY_UMTSTypeMask  btUMTSType = 0;
         aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_NETWORK;
         aOpts[i].pVal = (void*) btNet;
         i++;
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_WCDMA) {
            btUMTSType |= AEEBEARERTECHNOLOGY_UMTS_WCDMA;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_GPRS) {
            btUMTSType |= AEEBEARERTECHNOLOGY_UMTS_GPRS;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_HSDPA) {
            btUMTSType |= AEEBEARERTECHNOLOGY_UMTS_HSDPA;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_HSUPA) {
            btUMTSType|= AEEBEARERTECHNOLOGY_UMTS_HSUPA;
         }
         if ((psDSSBearerTech->data.umts_type.rat_mask) & PS_IFACE_UMTS_EDGE) {
            btUMTSType |= AEEBEARERTECHNOLOGY_UMTS_EDGE;
         }
         aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_UMTS_TYPE;
         aOpts[i].pVal = (void*) btUMTSType;
         i++;
         break;
      }

      // current network is WLAN
      case PS_IFACE_NETWORK_WLAN:
      {
         AEEBEARERTECHNOLOGY_Network btNet = AEEBEARERTECHNOLOGY_WLAN;
         aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_NETWORK;
         aOpts[i].pVal = (void*) btNet;
         i++;
         break;
      }

      default:
         nRet = AEE_NET_ERROR;
         break;
   }

   aOpts[i].nId = AEEBEARERTECHNOLOGYOPT_END;
   aOpts[i].pVal = NULL;
   if (SUCCESS != IBEARERTECHNOLOGY_AddOpt(pBearerTechOpts, aOpts)) {
      nRet = EFAILED;
   }

   return nRet;
}

/*===========================================================================
 Insert a buffer into the head of IPv6 Private Address info list
===========================================================================*/
static void InsertPrivAddrInfoBufAtHead(OEMNetIPv6PrivAddrInfo **ppPaiListHead, OEMNetIPv6PrivAddrInfo *pPaiBuffer)
{
   if (NULL == pPaiBuffer || NULL == ppPaiListHead) {
      FARF(ALWAYS, ("OEMNet: InsertPrivAddrInfoBufAtHead: one of the parameters is a NULL pointer"));
      return;
   }
   pPaiBuffer->pNext = *ppPaiListHead;
   *ppPaiListHead = pPaiBuffer;
}

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
  Remove a buffer from the head of an IPv6 Private Address info list
===========================================================================*/
static OEMNetIPv6PrivAddrInfo* RemovePrivAddrInfoBufFromHead(OEMNetIPv6PrivAddrInfo **ppPaiListHead)
{
   OEMNetIPv6PrivAddrInfo *ppaiResult = NULL;

   if (NULL == ppPaiListHead) {
      FARF(ALWAYS, ("OEMNet: RemovePrivAddrInfoBufFromHead: ppPaiListHead or *ppPaiListHead is a NULL pointer"));
      return NULL;
   }

   ppaiResult = *ppPaiListHead;
   if (ppaiResult) {
      *ppPaiListHead = ppaiResult->pNext;
      ppaiResult->pNext = NULL;
   }

   return ppaiResult;
}
#endif //#ifdef FEATURE_DATA_PS_IPV6

/*===========================================================================
 Get a pointer to the buffer matching to a certain handle
===========================================================================*/
static OEMNetIPv6PrivAddrInfo* GetPrivAddrInfoBuff(OEMNetIPv6PrivAddrInfo **ppPaiListHead, const void* pHandle)
{
   if (NULL == pHandle || NULL == ppPaiListHead) {
      FARF(ALWAYS, ("OEMNet: GetPrivAddrInfoBuff: one of the parameters is a NULL pointer"));
      return NULL;
   }

   while (*ppPaiListHead) {
      if(pHandle == (*ppPaiListHead)->pHandle) {
         break;
      }
      ppPaiListHead = &((*ppPaiListHead)->pNext);
   }

   return *ppPaiListHead;
}

/*===========================================================================
 Remove a buffer matching to a certain handle
===========================================================================*/
static OEMNetIPv6PrivAddrInfo* RemovePrivAddrInfoBuff(OEMNetIPv6PrivAddrInfo **ppPaiListHead, const void* pHandle)
{
   OEMNetIPv6PrivAddrInfo* pRet = NULL;

   // return the removed element, or null if element was not found
   if (NULL == pHandle) {
      return NULL;
   }

   while (*ppPaiListHead) {
      if( pHandle == (*ppPaiListHead)->pHandle) {

         pRet = *ppPaiListHead;
         *ppPaiListHead = (*ppPaiListHead)->pNext;
         break;
      }
      ppPaiListHead = &((*ppPaiListHead)->pNext);
   }

   return pRet;
}

/*===========================================================================

FUNCTION OEMNet_DeletePrivAddrInfo

DESCRIPTION
   This function deletes a private address information record from its list.

===========================================================================*/
extern int16 OEMNet_DeletePrivAddrInfo(OEMCONTEXT netd, void* pHandle)
{
   OEMNetIPv6PrivAddrInfo* paiBuffer = NULL;
   OEMNet*  pNet = (OEMNet*)netd;

   INTLOCK();
   paiBuffer = RemovePrivAddrInfoBuff(&pNet->ppiReadyBuffers, pHandle);
   INTFREE();

   if (NULL == paiBuffer) {
      return AEE_NET_ERROR;
   }

   // return the record to the free buffers pool
   INTLOCK();
   InsertPrivAddrInfoBufAtHead(&pNet->ppiFreeBuffers, paiBuffer);
   INTFREE();

   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION OEMNet_GenerateIPv6PrivateAddr

DESCRIPTION
   This function requests the AMSS to generate a new private IPv6 address.

===========================================================================*/
extern int16 OEMNet_GenerateIPv6PrivateAddr(OEMCONTEXT   netd,
                                            boolean      bIsUnique,
                                            void*        pHandle)
{
#ifdef FEATURE_DATA_PS_IPV6
   OEMNet*  pNet = (OEMNet*)netd;
   sint15   dss_errno;
   int      nRet;
   ip_addr_type IPAddr;
   dss_iface_ioctl_priv_ipv6_addr_type PrivAddrType;
   OEMNetIPv6PrivAddrInfo* paiBuffer = NULL;

   /*******************************************************/
   /* get the info buffer corresponding to this pUserData */
   /*******************************************************/

   // look for the buffer in the "ready" list.
   INTLOCK();
   paiBuffer = GetPrivAddrInfoBuff(&pNet->ppiReadyBuffers, pHandle);
   INTFREE();

   // if the buffer corresponding to this handle doesn't exist,
   // create a new info buffer and put it in the ready list
   if(NULL == paiBuffer) {
      INTLOCK();
      paiBuffer = RemovePrivAddrInfoBufFromHead(&pNet->ppiFreeBuffers);
      INTFREE();

      if(paiBuffer) {
         // update the handle field and insert the element into the ready buffers' list.
         paiBuffer->pHandle = pHandle;
         // init the address with zeros.
         memset(&(paiBuffer->aiInfo.ipaAddr.Byte), 0x0, sizeof(paiBuffer->aiInfo.ipaAddr.Byte));
         INTLOCK();
         InsertPrivAddrInfoBufAtHead(&pNet->ppiReadyBuffers, paiBuffer);
         INTFREE();
      } else {// no more free buffers
         FARF(ALWAYS, ("OEMNet_GenerateIPv6PrivateAddr: out of privat address info buffers"));
         return AEE_NET_ENOBUFS;
      }
   } else { // genearte() was called for this handle at least once
      if ( AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING == paiBuffer->aiInfo.uState) {
         // if the state is GEN_PENDING - return with EALREADY
         return EALREADY;
      }
   }

   /******************************/
   /* prepare data for the ioctl */
   /******************************/
   PrivAddrType.dss_nethandle          = pNet->sAppID;
   PrivAddrType.iid_params.is_unique   = bIsUnique;
   PrivAddrType.event_cb               = OEMNet_IPv6PrivAddrEvtCB;
   PrivAddrType.user_data_ptr          = pHandle;
   PrivAddrType.ip_addr                = &IPAddr;

   // call the AMSS's function
   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GENERATE_PRIV_IPV6_ADDR,
                             &PrivAddrType,
                             &dss_errno);


   // handle case of failure or asynchronous operation
   if (DSS_SUCCESS != nRet) {
      if(DS_EWOULDBLOCK  == dss_errno) {
         paiBuffer->aiInfo.uState = AEE_IPV6_PRIV_ADDR_STATE_GEN_PENDING;
      } else {
         paiBuffer->aiInfo.uState = AEE_IPV6_PRIV_ADDR_STATE_GEN_FAILED;
      }

      return DSSToAEE_Err(dss_errno);
   }

   /*******************************************************/
   /* handle a successful, synchronous address generation */
   /*******************************************************/

   // update the info buffer
   paiBuffer->aiInfo.uState      = AEE_IPV6_PRIV_ADDR_STATE_VALID;
   paiBuffer->aiInfo.bIsUnique   = bIsUnique;

   // Copy the address. Note that both the source buffer (of type ip_addr_type)
   // and destination buffer (of type INAddr6), hold the address in a network byte order.
   memcpy(paiBuffer->aiInfo.ipaAddr.Byte,
          PrivAddrType.ip_addr->addr.v6,
          sizeof(paiBuffer->aiInfo.ipaAddr.Byte));

   return AEE_NET_SUCCESS;
#else
   return EUNSUPPORTED;
#endif // ifdef FEATURE_DATA_PS_IPV6
}

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================

FUNCTION OEMNet_IPv6PrivAddrEvtCB

DESCRIPTION
   This function is being called by the AMSS when an IPv6 Privacy Event occurred.

===========================================================================*/
static void OEMNet_IPv6PrivAddrEvtCB(dss_iface_ioctl_event_enum_type        event,
                                     dss_iface_ioctl_event_info_union_type  event_info,
                                     void                                   *user_data,
                                     sint15                                 dss_nethandle,
                                     dss_iface_id_type                      iface_id)
{
   dss_iface_ioctl_priv_addr_info_type* pEvtPrivIPv6AddrInfo = (dss_iface_ioctl_priv_addr_info_type*)(&event_info);
   OEMNet *pNet = OEMNet_FromAppID(dss_nethandle);

   if ((OEMNet*)0 != pNet) {
      OEMNetIPv6PrivAddrInfo* paiBuffer;

      // get the corresponding info buffer
      INTLOCK();
      paiBuffer = GetPrivAddrInfoBuff(&pNet->ppiReadyBuffers, user_data);
      INTFREE();

      if ( paiBuffer ) {
         // In case of "deprecated or "deleted" event, verify it refers to the most recently generated address.
         // If the address associated with this event is not the most recent - don't handle this event.
         switch (event) {
            case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV:
            case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DELETED_EV:

               // Compare the address. Note that the compared structs
               // (of type ip_addr type and INAddr6) are both in a network byte order.
               if (0 != memcmp(paiBuffer->aiInfo.ipaAddr.Byte,
                               pEvtPrivIPv6AddrInfo->ip_addr.addr.v6,
                               sizeof(paiBuffer->aiInfo.ipaAddr.Byte))) {
                     // event isn't associated with current address
                     // TODO: FARFing from callbacks is unsafe, find another solution
                     return;
               }

               // update the state
               paiBuffer->aiInfo.uState = ((event == DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_DEPRECATED_EV) ?
                                            AEE_IPV6_PRIV_ADDR_STATE_DEPRECATED : AEE_IPV6_PRIV_ADDR_STATE_DELETED);
               break;

            case DSS_IFACE_IOCTL_IPV6_PRIV_ADDR_GENERATED_EV:
               if(pEvtPrivIPv6AddrInfo->ip_addr.type != IP_ADDR_INVALID) {
                  paiBuffer->aiInfo.uState            = AEE_IPV6_PRIV_ADDR_STATE_VALID;
                  paiBuffer->aiInfo.bIsUnique         = pEvtPrivIPv6AddrInfo->is_unique;

                  // Copy the address. Note that both the source buffer (of type ip_addr_type)
                  // and destination buffer (of type INAddr6), hold the address in a network byte order.
                  memcpy(paiBuffer->aiInfo.ipaAddr.Byte,
                         pEvtPrivIPv6AddrInfo->ip_addr.addr.v6,
                         sizeof(paiBuffer->aiInfo.ipaAddr.Byte));
               } else {
                  paiBuffer->aiInfo.uState = AEE_IPV6_PRIV_ADDR_STATE_GEN_FAILED;
               }
               break;

            default:
               // TODO: FARFing from callbacks is unsafe, find another solution
               return;
         }

      } else { // info buffer was not found
         // TODO: FARFing from callbacks is unsafe, find another solution
         return;
      }

      // dispatch the event to the upper layers
      OEMNet_ProcessIPv6PrivAddrEvent(pNet);
   }
}
#endif // ifdef FEATURE_DATA_PS_IPV6

/*===========================================================================

FUNCTION OEMNet_GetIPv6PrivAddrInfo

DESCRIPTION
  Retrieve IPv6 Privacy Address event information

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_GetIPv6PrivAddrInfo(OEMCONTEXT           netd,
                                 AEEIPv6PrivAddrInfo* pPrivAddrInfo,
                                 const void*          pHandle)
{
   OEMNet*                 pNet = (OEMNet*)netd;
   int16                   nRet;
   OEMNetIPv6PrivAddrInfo* paiBuffer;

   if ((NULL == pHandle) || (NULL == pPrivAddrInfo)) {
      return EBADPARM;
   }

   INTLOCK();
   paiBuffer = GetPrivAddrInfoBuff(&pNet->ppiReadyBuffers, pHandle);
   INTFREE();

   if (paiBuffer) {
      memcpy(pPrivAddrInfo, &(paiBuffer->aiInfo), sizeof(*pPrivAddrInfo));
      nRet = AEE_NET_SUCCESS;
   } else {
      nRet = AEE_NET_ERROR;
   }

   return nRet;
}

/*===========================================================================

FUNCTION OEMNet_GetOutageInfo

DESCRIPTION
  Retrieve Outage event information

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_GetOutageInfo(OEMCONTEXT       netd,
                           AEEOutageInfo    *pOutageInfo)
{
   OEMNet*  pNet = (OEMNet*)netd;
   uint32   uTimeModified = 0;

   if (NULL == pOutageInfo) {
      return EBADPARM;
   }

   INTLOCK();
   MEMCPY(pOutageInfo,
          &(pNet->ooiOutageInfo.aoiOutageInfo),
          sizeof(*pOutageInfo));
   uTimeModified = pNet->ooiOutageInfo.uTimeModified;
   INTFREE();

   // update the event info if the event is valid
   if (AEE_NET_OUTAGE_INVALID != pOutageInfo->nStatus) {
      uint32 uNow = GETUPTIMEMS();
      uint32 uTimePassed = 0;

      if (uNow < uTimeModified) {
         // the value returned by GETUPTIMEMS() is cyclic
         uTimePassed = uNow + (MAX_UINT32 - uTimeModified);
      } else {
         uTimePassed = uNow - uTimeModified;
      }
      pOutageInfo->nTimeToOutage -= uTimePassed;

      if (pOutageInfo->nTimeToOutage > 0) {
         pOutageInfo->nStatus = AEE_NET_OUTAGE_VALID;
      } else {
         // this casting is safe, since uDuration is expected to be
         // at most a few hundred milliseconds
         ASSERT_FARF(pOutageInfo->uDuration <= MAX_INT32);
         if (pOutageInfo->nTimeToOutage +
             (int) pOutageInfo->uDuration > 0) {
            pOutageInfo->nStatus = AEE_NET_OUTAGE_STARTED;
         } else {
            pOutageInfo->nStatus = AEE_NET_OUTAGE_EXPIRED;
         }
      }
   }

   return AEE_NET_SUCCESS;
}

/*===========================================================================
FUNCTION OEMNet_GetIPv6PrefixInfo

DESCRIPTION
  Retrieve the information regarding an IPv6 Prefix
  corresponding to a given address.

DEPENDENCIES
  None.
===========================================================================*/
int16 OEMNet_GetIPv6PrefixInfo(OEMCONTEXT          netd,
                               INAddr6*            pAddr,
                               AEEIPv6PrefixInfo*  pPrefixInfo
                              )
{
#ifdef FEATURE_DATA_PS_IPV6
   OEMNet*        pNet              = (OEMNet*)netd;
   int            nRet              = AEE_NET_SUCCESS;
   uint8          i, uNumPrefixes   = IPV6_PREFIXES_LIST_SIZE;
   sint15         dss_errno;
   dss_iface_ioctl_prefix_info_type prefix_info_list[IPV6_PREFIXES_LIST_SIZE];
   dss_iface_ioctl_get_all_v6_prefixes_type all_prefixes;

   if (NULL == pAddr || NULL == pPrefixInfo) {
      return EBADPARM;
   }

   // init the ioctl's parameter
   all_prefixes.prefix_info_ptr = prefix_info_list;
   all_prefixes.num_prefixes = uNumPrefixes;

   // call the AMSS's function
   nRet = OEMNet_Iface_Ioctl(pNet,
                             DSS_IFACE_IOCTL_GET_ALL_V6_PREFIXES,
                             &all_prefixes,
                             &dss_errno);

   if (nRet != DSS_SUCCESS) {
      return DSSToAEE_Err(dss_errno);
   }

   // search for a prefix matching the given address
   for (i = 0 ; i < all_prefixes.num_prefixes ; ++i) {
      if (ArePrefixsEqual(pAddr->Byte,
                          (prefix_info_list[i]).prefix.s6_addr,
                          (prefix_info_list[i]).prefix_len)) {

         // matching prefix was found, copy its info
         pPrefixInfo->uPrefixLen = (prefix_info_list[i]).prefix_len;
         if (DSv6AddrStateToAEEv6PrefixState((prefix_info_list[i]).prefix_state, &(pPrefixInfo->uState))) {
            return AEE_NET_SUCCESS;
         } else {
            return AEE_NET_ERROR;
         }
      }
   }

   // no matching prefix was found
   pPrefixInfo->uPrefixLen = 0;
   pPrefixInfo->uState = AEE_IPV6_PREFIX_NOT_EXIST;

   return AEE_NET_SUCCESS;
#else
   return EUNSUPPORTED;
#endif //FEATURE_DATA_PS_IPV6
}

#ifdef FEATURE_DATA_PS_IPV6
static boolean ArePrefixsEqual(const unsigned char* pucAddr1, const unsigned char* pucAddr2, uint32 uLen)
{
   unsigned int i;
   unsigned char ucMask;

   if (0 == uLen || 128 < uLen) {
      return FALSE;
   }

   // compare the bytes
   for (i = 0 ; i < (uLen / 8) ; i++) {
      if (pucAddr1[i] != pucAddr2[i]) {
         FARF(ALWAYS, ("Illegal prefix length received from lower layers: %d", uLen));
         return FALSE;
      }
   }

   // compare the remaining bits
   if (uLen % 8) {
      ucMask = ~((1 << (8 - (uLen % 8))) - 1);

      // since we compare each byte, we don't need to convert ucMask into network byte order.
      if ((pucAddr1[i] & ucMask) != (pucAddr2[i] & ucMask)) {
         return FALSE;
      }
   }

   return TRUE;
}

static boolean DSv6AddrStateToAEEv6PrefixState(const dss_iface_ioctl_ipv6_addr_state_enum_type eDSV6AddrState,
                                               AEEIPv6PrefixState* pAEEPrefixState)
{
   boolean bRet = TRUE;

   switch(eDSV6AddrState)
   {
      case IPV6_ADDR_STATE_TENTATIVE:
         *pAEEPrefixState = AEE_IPV6_PREFIX_TENTATIVE;
         break;

      case IPV6_ADDR_STATE_VALID:
         *pAEEPrefixState = AEE_IPV6_PREFIX_VALID;
         break;

      case IPV6_ADDR_STATE_DEPRECATED:
         *pAEEPrefixState = AEE_IPV6_PREFIX_DEPRECATED;
         break;

      default:
         FARF(ALWAYS, ("Illegal address state received from lower layers: %d", (int)eDSV6AddrState));
         bRet = FALSE;
   }

   return bRet;
}
#endif //FEATURE_DATA_PS_IPV6

/*===========================================================================

                      OEM NET PRIMARY QOS FUNCTION DEFINITIONS

 ===========================================================================*/

#ifdef OEM_PROVIDES_PRIMARY_QOS

/*===========================================================================

FUNCTION PutPrimaryQoSModifyEventBufAtHead

DESCRIPTION
   Insert a buffer into a Primary QoS Modify Event list (at the head of the list)

===========================================================================*/
static void PutPrimaryQoSModifyEventBufAtHead(OEMNetPrimaryQoSModifyEventData **ppPrimQEListHead,
                                              OEMNetPrimaryQoSModifyEventData *pPrimQEBuffer)
{
   if (NULL == pPrimQEBuffer || NULL == ppPrimQEListHead) {
      return;
   }

   INTLOCK();
   pPrimQEBuffer->pNext = *ppPrimQEListHead;
   *ppPrimQEListHead = pPrimQEBuffer;
   INTFREE();
}

/*===========================================================================

FUNCTION PutPrimaryQoSModifyEventBufAtTail

DESCRIPTION
   Insert a buffer into a Primary QoS Modify Event list (at the tail of the list)

===========================================================================*/
static void PutPrimaryQoSModifyEventBufAtTail(OEMNetPrimaryQoSModifyEventData **ppPrimQEListHead,
                                              OEMNetPrimaryQoSModifyEventData *pPrimQEBuffer)
{
   INTLOCK();
   while (NULL != *ppPrimQEListHead) {
      ppPrimQEListHead = &((*ppPrimQEListHead)->pNext);
   }
   *ppPrimQEListHead = pPrimQEBuffer;
   INTFREE();
}

/*===========================================================================

FUNCTION GetPrimaryQoSModifyEventBufFromHead

DESCRIPTION
   Retrieve a buffer from a Primary QoS Modify Event list
   (from the head of the list) - Removes it from the list

===========================================================================*/
static OEMNetPrimaryQoSModifyEventData* GetPrimaryQoSModifyEventBufFromHead(OEMNetPrimaryQoSModifyEventData **ppPrimQEListHead)
{
   OEMNetPrimaryQoSModifyEventData *pPrimQEResult;

   if (NULL == ppPrimQEListHead || NULL == *ppPrimQEListHead ) {
      return NULL;
   }

   INTLOCK();
   pPrimQEResult = *ppPrimQEListHead;
   if (pPrimQEResult) {
      *ppPrimQEListHead = pPrimQEResult->pNext;
      pPrimQEResult->pNext = NULL;
   }
   INTFREE();

   return pPrimQEResult;
}

/*===========================================================================

FUNCTION OEMNet_PrimaryQoSModifyEventOccurred

DESCRIPTION
  Callback function for Primary QoS Modify events

===========================================================================*/
static void OEMNet_PrimaryQoSModifyEventOccurred
(
   dss_iface_ioctl_event_enum_type        event,
   dss_iface_ioctl_event_info_union_type  event_info,
   void                                  *user_data,
   sint15                                 app_id,
   dss_iface_id_type                      iface_id
)
{
   OEMNet *pNet = OEMNet_FromAppID(app_id);

   if ((OEMNet*)0 != pNet) {
      OEMNetPrimaryQoSModifyEventData* pPrimQEBuffer;

      // Get a free buffer from list
      pPrimQEBuffer = GetPrimaryQoSModifyEventBufFromHead(&pNet->pPrimQEFreeBuffers);
      if(NULL == pPrimQEBuffer) {
         // buffer not found
         // TODO: FARFing from callbacks is unsafe, find another solution
         return;
      }

      // Update buffer info: handle field and event info
      pPrimQEBuffer->handle = user_data;
      pPrimQEBuffer->event = event;

      // Insert the element into the ready buffers' list.
      PutPrimaryQoSModifyEventBufAtTail(&pNet->pPrimQEReadyBuffers, pPrimQEBuffer);

      // Dispatch the event to the upper layers
      OEMNet_ProcessPrimaryQoSModifyEvent(pNet);
   }
}

/*===========================================================================

FUNCTION TranslateOEMPrimaryQoSSpecToAMSS

DESCRIPTION
  Translate OEM Primary QoS Spec to AMSS Primary QoS Spec.
  It's the caller responsibility to free the allocated memory for the AMSS
  Auxiliary Flow List by calling FreeAMSSSpecContents() (event if function
  returns an error)

===========================================================================*/
int16 TranslateOEMPrimaryQoSSpecToAMSS(oem_primary_qos_spec_type *pOEMPrimaryQoSSpec,
                                       primary_qos_spec_type *pAMSSPrimaryQoSSpec)
{
   int16 nRet;

   if (pOEMPrimaryQoSSpec->field_mask & OEM_QOS_MASK_RX_FLOW) {
      nRet = TranslateOEMFlowToAMSS(&pOEMPrimaryQoSSpec->rx_flow_template.req_flow,
                                      &pAMSSPrimaryQoSSpec->rx_flow_template.req_flow);
      if (nRet != AEE_NET_SUCCESS) {
         return nRet;
      }
      pAMSSPrimaryQoSSpec->field_mask |= QOS_MASK_RX_FLOW;
   }

   if (pOEMPrimaryQoSSpec->field_mask & OEM_QOS_MASK_RX_MIN_FLOW) {
      nRet = TranslateOEMFlowToAMSS(&pOEMPrimaryQoSSpec->rx_flow_template.min_req_flow,
                                      &pAMSSPrimaryQoSSpec->rx_flow_template.min_req_flow);
      if (nRet != AEE_NET_SUCCESS) {
         return nRet;
      }
      pAMSSPrimaryQoSSpec->field_mask |= QOS_MASK_RX_MIN_FLOW;
   }

   if (pOEMPrimaryQoSSpec->field_mask & OEM_QOS_MASK_RX_AUXILIARY_FLOWS) {
      nRet = TranslateOEMFlowListToAMSSAuxFlowList(pOEMPrimaryQoSSpec->rx_flow_template.aux_flow_list_ptr,
                              pOEMPrimaryQoSSpec->rx_flow_template.num_aux_flows,
                              &(pAMSSPrimaryQoSSpec->rx_flow_template));
      if (nRet != AEE_NET_SUCCESS) {
         return nRet;
      }
      OEMDSS_SetRxAuxFlowMask(&(pAMSSPrimaryQoSSpec->field_mask));
   }

   if (pOEMPrimaryQoSSpec->field_mask & OEM_QOS_MASK_TX_FLOW) {
      nRet = TranslateOEMFlowToAMSS(&pOEMPrimaryQoSSpec->tx_flow_template.req_flow,
                                      &pAMSSPrimaryQoSSpec->tx_flow_template.req_flow);
      if (nRet != AEE_NET_SUCCESS) {
         return nRet;
      }
      pAMSSPrimaryQoSSpec->field_mask |= QOS_MASK_TX_FLOW;
   }

   if (pOEMPrimaryQoSSpec->field_mask & OEM_QOS_MASK_TX_MIN_FLOW) {
      nRet = TranslateOEMFlowToAMSS(&pOEMPrimaryQoSSpec->tx_flow_template.min_req_flow,
                                      &pAMSSPrimaryQoSSpec->tx_flow_template.min_req_flow);
      if (nRet != AEE_NET_SUCCESS) {
         return nRet;
      }
      pAMSSPrimaryQoSSpec->field_mask |= QOS_MASK_TX_MIN_FLOW;
   }

   if (pOEMPrimaryQoSSpec->field_mask & OEM_QOS_MASK_TX_AUXILIARY_FLOWS) {
      nRet = TranslateOEMFlowListToAMSSAuxFlowList(pOEMPrimaryQoSSpec->tx_flow_template.aux_flow_list_ptr,
                              pOEMPrimaryQoSSpec->tx_flow_template.num_aux_flows,
                              &(pAMSSPrimaryQoSSpec->tx_flow_template));
      if (nRet != AEE_NET_SUCCESS) {
         return nRet;
      }
      OEMDSS_SetTxAuxFlowMask(&(pAMSSPrimaryQoSSpec->field_mask));
   }
#if defined(AEE_SIMULATOR)
   // set last 4 bits according to the OEM QoS Spec
   pAMSSPrimaryQoSSpec->field_mask = pAMSSPrimaryQoSSpec->field_mask | (pOEMPrimaryQoSSpec->field_mask & 0xF00);
#endif // defined(AEE_SIMULATOR)
   return AEE_NET_SUCCESS;
}

/*===========================================================================

FUNCTION FreeAMSSPrimarySpecContents

DESCRIPTION
  Free an AMSS Primary QoS Spec's dynamically allocated buffers

===========================================================================*/
void FreeAMSSPrimarySpecContents(primary_qos_spec_type  *pAMSSPrimaryQoSSpec)
{
   OEMDSS_FreeAuxFlowList(&(pAMSSPrimaryQoSSpec->rx_flow_template));
   OEMDSS_FreeAuxFlowList(&(pAMSSPrimaryQoSSpec->tx_flow_template));
}

/*===========================================================================

FUNCTION SetOEMPrimaryQoSSpecErrorMaskFromAMSS

DESCRIPTION
  Set an OEM Primary QoS error mask fields by coping them from an AMSS Primary QoS

===========================================================================*/
void SetOEMPrimaryQoSSpecErrorMaskFromAMSS(oem_primary_qos_spec_type *pOEMPrimaryQoSSpec,
                                           primary_qos_spec_type *pAMSSPrimaryQoSSpec)
{
   // handle RX direction
   if (pAMSSPrimaryQoSSpec->field_mask & QOS_MASK_RX_FLOW) {
      pOEMPrimaryQoSSpec->rx_flow_template.req_flow.err_mask =
         pAMSSPrimaryQoSSpec->rx_flow_template.req_flow.err_mask;
   }

   if (OEMDSS_IsRxAuxFlowMask(pAMSSPrimaryQoSSpec->field_mask)) {
      uint8 i, uSize;
      uSize = OEMDSS_GetNumAuxFlows(&(pAMSSPrimaryQoSSpec->rx_flow_template));
      for (i=0; i<uSize; i++) {
         pOEMPrimaryQoSSpec->rx_flow_template.aux_flow_list_ptr[i].err_mask =
            OEMDSS_GetAuxFlowErrMask(&(pAMSSPrimaryQoSSpec->rx_flow_template), i);
      }
   }

   if (pAMSSPrimaryQoSSpec->field_mask & QOS_MASK_RX_MIN_FLOW) {
      pOEMPrimaryQoSSpec->rx_flow_template.min_req_flow.err_mask =
         pAMSSPrimaryQoSSpec->rx_flow_template.min_req_flow.err_mask;
   }

   // handle TX direction
   if (pAMSSPrimaryQoSSpec->field_mask & QOS_MASK_TX_FLOW) {
      pOEMPrimaryQoSSpec->tx_flow_template.req_flow.err_mask =
         pAMSSPrimaryQoSSpec->tx_flow_template.req_flow.err_mask;
   }

   if (OEMDSS_IsTxAuxFlowMask(pAMSSPrimaryQoSSpec->field_mask)) {
      uint8 i, uSize;
      uSize = OEMDSS_GetNumAuxFlows(&(pAMSSPrimaryQoSSpec->tx_flow_template));
      for (i=0; i<uSize; i++) {
         pOEMPrimaryQoSSpec->tx_flow_template.aux_flow_list_ptr[i].err_mask =
            OEMDSS_GetAuxFlowErrMask(&(pAMSSPrimaryQoSSpec->tx_flow_template), i);
      }
   }

   if (pAMSSPrimaryQoSSpec->field_mask & QOS_MASK_TX_MIN_FLOW) {
      pOEMPrimaryQoSSpec->tx_flow_template.min_req_flow.err_mask =
         pAMSSPrimaryQoSSpec->tx_flow_template.min_req_flow.err_mask;
   }
}

#endif // OEM_PROVIDES_PRIMARY_QOS

/*===========================================================================

FUNCTION OEMNet_GetPrimaryQoSGrantedFlowSpec

DESCRIPTION
  Retrieve the flow specification of a granted Primary QoS.

===========================================================================*/
int16 OEMNet_GetPrimaryQoSGrantedFlowSpec
(
   OEMCONTEXT           netd,
   oem_ip_flow_type    *pRxFlow,
   oem_ip_flow_type    *pTxFlow
)
{
#ifdef OEM_PROVIDES_PRIMARY_QOS

   OEMNet                                *pNet = (OEMNet*)netd;
   int16                                  ret;
   sint15                                 ndssErr;
   dss_iface_ioctl_primary_qos_get_granted_flow_spec_type primaryQoS_getFlowSpec;
   int result;

   if ((NULL == pRxFlow) || (NULL == pTxFlow)) {
      return AEE_NET_EINVAL;
   }

   memset(&primaryQoS_getFlowSpec, 0, sizeof(primaryQoS_getFlowSpec));

   result = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_PRIMARY_QOS_GET_GRANTED_FLOW_SPEC,
                               &primaryQoS_getFlowSpec,
                               &ndssErr);
   if (DSS_SUCCESS != result) {
      return DSSToAEE_Err(ndssErr);
   }

   ret = TranslateAMSSFlowToOEM(&primaryQoS_getFlowSpec.rx_ip_flow, pRxFlow);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("Primary QoS Flow Rx translation failed! n:%d", pNet));
      return ret;
   }

   ret = TranslateAMSSFlowToOEM(&primaryQoS_getFlowSpec.tx_ip_flow, pTxFlow);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("Primary QoS Flow Tx translation failed! n:%d", pNet));
      return ret;
   }

   return AEE_NET_SUCCESS;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_PRIMARY_QOS
}

/*===========================================================================

FUNCTION OEMNet_ModifyPrimaryQoS

DESCRIPTION
  Modify a Primary QoS.

===========================================================================*/
int16 OEMNet_ModifyPrimaryQoS
(
   OEMCONTEXT                  netd,
   oem_primary_qos_spec_type   *pPrimaryQoS,
   void                        *handle
)
{
#ifdef OEM_PROVIDES_PRIMARY_QOS

   OEMNet *pNet = (OEMNet*)netd;
   int16  ret;
   sint15 ndssErr;
   dss_iface_ioctl_primary_qos_modify_type *pPrimQoSModify;
   int result;

   pPrimQoSModify = sys_malloc(sizeof(dss_iface_ioctl_primary_qos_modify_type));
   if (!pPrimQoSModify) {
      return AEE_NET_ENOMEM;
   }

   // init auxiliary flows number to zero, for easy memory cleanup based on
   // the auxiliary flow number, when translation fails
   OEMDSS_SetNumAuxFlows(&(pPrimQoSModify->primary_qos_spec.rx_flow_template), 0);
   OEMDSS_SetNumAuxFlows(&(pPrimQoSModify->primary_qos_spec.tx_flow_template), 0);

   ret = TranslateOEMPrimaryQoSSpecToAMSS(pPrimaryQoS, &pPrimQoSModify->primary_qos_spec);
   if (AEE_NET_SUCCESS != ret) {
      FARF(ALWAYS, ("QoSSpec translation failed! n:%d", pNet));
      FreeAMSSPrimarySpecContents(&pPrimQoSModify->primary_qos_spec);
      sys_free(pPrimQoSModify);
      return ret;
   }

  pPrimQoSModify->dss_nethandle = pNet->sAppID;
  pPrimQoSModify->cback_fn      = (dss_iface_ioctl_event_cb)OEMNet_PrimaryQoSModifyEventOccurred;
  pPrimQoSModify->user_data_ptr = handle;

   result = OEMNet_Iface_Ioctl(pNet,
                               DSS_IFACE_IOCTL_PRIMARY_QOS_MODIFY,
                               pPrimQoSModify,
                               &ndssErr);

   if (DSS_SUCCESS == result) {
      ret = AEE_NET_SUCCESS;
   } else {
      SetOEMPrimaryQoSSpecErrorMaskFromAMSS(pPrimaryQoS, &pPrimQoSModify->primary_qos_spec);
      ret = DSSToAEE_Err(ndssErr);
   }

   FreeAMSSPrimarySpecContents(&pPrimQoSModify->primary_qos_spec);
   sys_free(pPrimQoSModify);

   return ret;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_PRIMARY_QOS
}

/*===========================================================================

FUNCTION OEMNet_GetPrimaryQoSModifyEventInfo

DESCRIPTION
  Retrieve Primary QoS Modify event information

===========================================================================*/
int32 OEMNet_GetPrimaryQoSModifyEventInfo(OEMCONTEXT netd,
                                          void **pHandle,
                                          AEEPrimaryQoSEvent *pEvent)
{
#ifdef OEM_PROVIDES_PRIMARY_QOS

   OEMNetPrimaryQoSModifyEventData *pPrimQEBuffer;
   OEMNet *pNet = (OEMNet*)netd;
   int32  ret;

   if ((NULL == pHandle) || (NULL == pEvent)) {
      return AEE_NET_EINVAL;
   }

   pPrimQEBuffer = GetPrimaryQoSModifyEventBufFromHead(&pNet->pPrimQEReadyBuffers);

   if (pPrimQEBuffer) {

      ret = AEE_NET_SUCCESS;
      *pHandle = pPrimQEBuffer->handle;

      switch (pPrimQEBuffer->event)
      {
         case DSS_IFACE_IOCTL_QOS_MODIFY_ACCEPTED_EV:
            *pEvent = AEE_PRIMARY_QOS_MODIFY_ACCEPTED_EV;
            break;

         case DSS_IFACE_IOCTL_QOS_MODIFY_REJECTED_EV:
            *pEvent = AEE_PRIMARY_QOS_MODIFY_REJECTED_EV;
            break;

         default:
            // Set the event type to "-1" to indicated to the caller that this
            // is an unsupported event
            *pEvent = -1;
            FARF(ALWAYS, ("unknown Primary QoS Mosidfy event value: %d", pPrimQEBuffer->event));
            break;
      }

      PutPrimaryQoSModifyEventBufAtHead(&pNet->pPrimQEFreeBuffers, pPrimQEBuffer);
   } else {
      ret = ENOSUCH;
   }

   return ret;

#else
   return AEE_NET_EOPNOTSUPP;
#endif // OEM_PROVIDES_PRIMARY_QOS
}

/*===========================================================================
FUNCTION: OEMNet_CancelPrimaryQoSModifyResultWaiter

DESCRIPTION
  Cancel the Primary QoS Modify Result waiter by removing it from the net instance

===========================================================================*/
static void OEMNet_CancelPrimaryQoSModifyResultWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbPrimaryQoSModifyResultWaiter = NULL;
}

/*===========================================================================
FUNCTION: OEMNet_CancelPrimaryQoSModifyWaiter

DESCRIPTION
  Cancel the Primary QoS Modify waiter by removing itself from the net instance

===========================================================================*/
static void OEMNet_CancelPrimaryQoSModifyWaiter(AEECallback *pcb)
{
   OEMNet* pNet = (OEMNet*)pcb->pCancelData;

   // Mark as canceled
   pcb->pfnCancel = 0;

   pNet->pcbPrimaryQoSModifyWaiter = NULL;
}

/*===========================================================================

FUNCTION OEMNet_SetPrimaryQoSModifyResultWaiter

DESCRIPTION
  Registers a new Primary QoS Modify Result event waiter callback

===========================================================================*/
void OEMNet_SetPrimaryQoSModifyResultWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelPrimaryQoSModifyResultWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbPrimaryQoSModifyResultWaiter) {
      CALLBACK_Cancel(pNet->pcbPrimaryQoSModifyResultWaiter);
   }

   pNet->pcbPrimaryQoSModifyResultWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_SetPrimaryQoSModifyWaiter

DESCRIPTION
  Registers a new Primary QoS Modify event waiter callback

===========================================================================*/
void OEMNet_SetPrimaryQoSModifyWaiter(OEMCONTEXT netd, AEECallback *pcbWaiter)
{
   OEMNet *pNet = (OEMNet*)netd;

   if (NULL != pcbWaiter) {
      CALLBACK_Cancel(pcbWaiter);
      pcbWaiter->pfnCancel = OEMNet_CancelPrimaryQoSModifyWaiter;
      pcbWaiter->pCancelData = pNet;
   }

   if (NULL != pNet->pcbPrimaryQoSModifyWaiter) {
      CALLBACK_Cancel(pNet->pcbPrimaryQoSModifyWaiter);
   }

   pNet->pcbPrimaryQoSModifyWaiter = pcbWaiter;
}

/*===========================================================================

FUNCTION OEMNet_PrimaryQoSModifyResultEventOccurred

DESCRIPTION
  Callback function for QoS Modify Result events.

===========================================================================*/
#ifdef OEM_PROVIDES_PRIMARY_QOS
static void OEMNet_PrimaryQoSModifyResultEventOccurred(OEMNet* pNet)
{
   if ((OEMNet*)0 != pNet) {
      OEMNet_ProcessPrimaryQoSModifyResultEvent(pNet);
   }
}
#endif // OEM_PROVIDES_PRIMARY_QOS

