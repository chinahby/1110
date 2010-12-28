#ifndef AEETELEPHONE_H
#define AEETELEPHONE_H
/*======================================================
FILE:  AEETelephone.h

SERVICES:  Telephone Interface to AEE

GENERAL DESCRIPTION:
   Definitions for interfaces 
      ITelephone, 
      ICallMgr, 
      ICall,
      ICallOrigOpts,
      IMultiPartyCall, 
      IPhoneCtl

Copyright (c) 2004-2008 QUALCOMM Incorporated.
               All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEE.h"

#include "AEEWebOpts.h"
#include "AEETelephoneTypes.h"

#define AEET_ERR_BASE                     0x04000000

#define AEET_ERR_BADDESC                  (0x0000 + AEET_ERR_BASE)
#define AEET_ERR_OP_NOT_ALLOWED           (0x0001 + AEET_ERR_BASE)
#define AEET_ERR_OUTOF_RESOURCES          (0x0002 + AEET_ERR_BASE)
#define AEET_ERR_PHONE_NOT_ONLINE         (0x0003 + AEET_ERR_BASE)
#define AEET_ERR_PHONE_NO_SUBSCRIPTION    (0x0004 + AEET_ERR_BASE)
#define AEET_ERR_SUPPS_STRING_NO_MATCH    (0x0005 + AEET_ERR_BASE)
#define AEET_ERR_SUPPS_STRING_BAD_PARAM   (0x0006 + AEET_ERR_BASE)
#define AEET_ERR_SUPPS_MSG_INVALID_TYPE   (0x0007 + AEET_ERR_BASE)
#define AEET_ERR_PHONE_NOT_ACTIVE         (0x0008 + AEET_ERR_BASE)
#define AEET_ERR_PHONE_IN_USE             (0x0009 + AEET_ERR_BASE)

/*************************************/
/* MOBILE PHONE MODULE STATE NOTIFY  */
/*************************************/

/* Event Masks for Notification */ 
#define AEET_NMASK_VOICE_CALL    0x0001   /* includes emergency calls */
#define AEET_NMASK_DATA_CALL     0x0002
#define AEET_NMASK_TEST_CALL     0x0004
#define AEET_NMASK_OTHER_CALL    0x0008
#define AEET_NMASK_PHONE         0x0010
#define AEET_NMASK_SS            0x0020
#define AEET_NMASK_INBAND        0x0040
#define AEET_NMASK_NEW_CALLDESC  0x0100
#define AEET_NMASK_NEW_NI_SUPP_TRANS 0x0200

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                                 ITelephone                                    */
/*********************************************************************************/
AEEINTERFACE( ITelephone ) {

   INHERIT_IQueryInterface( ITelephone );

   int (*GetPhoneInfo) ( ITelephone *pme,
                         AEETPhInfo *pPhInfo,
                         uint32 dwSize );
   int (*GetCallInfo) ( ITelephone *pme,
                        AEECallDesc cd,
                        AEETCallInfo *pCallInfo,
                        uint32 dwSize );
   int (*GetServingSystemInfo) ( ITelephone *pme,
                                 AEETSSInfo *pSSInfo,
                                 uint32 dwSize );
   
   int (*Get) ( ITelephone *pme, 
                uint32 dwCmd,                         /* [in] Command AEET_GET_CMD_* */
                void *pRequestParams,                 /* [in] Additional parameters for the query */
                uint32 dwParamsSize,
                AEECallback *pcb,                     /* [in|out] If SUCCESS Callback is invoked */
                void *pResponse,                      /* [out] Response buffer */
                uint32 dwSize );                      /* [in] Sizeof response buffer */
   
};

/* Get Cmds */
#define AEET_GET_CMD_ALL_CALLS            0x0001      /* Gives a list of calls in progress. */
#define AEET_GET_CMD_NETWORKS             0x0002
#define AEET_GET_CMD_MAX_CALL_DESCRIPTORS 0x0003      /* Gives maximum Call descriptor resources used
                                                      ** by the system */
#define AEET_GET_CMD_LINE_INFO            0x0004      /* Gives AEETLineInfo */

#define ITELEPHONE_AddRef( p )                        AEEGETPVTBL((p), ITelephone)->AddRef( p )
#define ITELEPHONE_Release( p )                       AEEGETPVTBL((p), ITelephone)->Release( p )
#define ITELEPHONE_QueryInterface( p, c, po )         AEEGETPVTBL((p), ITelephone)->QueryInterface( p, c, po )
#define ITELEPHONE_GetPhoneInfo( p, po, s )           AEEGETPVTBL((p), ITelephone)->GetPhoneInfo( p, po, s )
#define ITELEPHONE_GetCallInfo( p, c, po, s )         AEEGETPVTBL((p), ITelephone)->GetCallInfo( p, c, po, s )
#define ITELEPHONE_GetServingSystemInfo( p, po, s )   AEEGETPVTBL((p), ITelephone)->GetServingSystemInfo( p, po, s )

static __inline int ITELEPHONE_GetCalls( ITelephone *pme, AEETCalls *pout, uint32 dwSize )
{
   return AEEGETPVTBL(pme, ITelephone)->Get( pme, AEET_GET_CMD_ALL_CALLS, 
                                             NULL, 0, NULL, pout, dwSize );
}

static __inline int ITELEPHONE_GetNetworks( ITelephone *pme, AEETNetworkType type, AEECallback *pcb, void *pout, uint32 dwSize )
{
   return AEEGETPVTBL(pme, ITelephone)->Get( pme, AEET_GET_CMD_NETWORKS, 
                                             (void *)type, 0, pcb, pout, dwSize );
}

static __inline int ITELEPHONE_GetMaxCallDescriptors( ITelephone *pme, uint32 *pdwCount )
{
   return AEEGETPVTBL(pme, ITelephone)->Get( pme, AEET_GET_CMD_MAX_CALL_DESCRIPTORS, NULL, 
                                             0, NULL, pdwCount, sizeof(uint32) );
}

static __inline int ITELEPHONE_GetLineInfo( ITelephone *pme, AEETLineInfo *po, uint32 dwSize )
{
   return AEEGETPVTBL(pme, ITelephone)->Get( pme, AEET_GET_CMD_LINE_INFO, 
                                             NULL, 0, NULL, po, dwSize );
}

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                                ICallOrigOpts                                  */
/*********************************************************************************/
AEEINTERFACE(ICallOrigOpts)
{
   INHERIT_IWebOpts(ICallOrigOpts);
};

#define ICALLORIGOPTS_AddRef( p )                      AEEGETPVTBL(p, ICallOrigOpts)->AddRef( p )
#define ICALLORIGOPTS_Release( p )                     AEEGETPVTBL(p, ICallOrigOpts)->Release( p )
#define ICALLORIGOPTS_QueryInterface( p, cls, ppo )    AEEGETPVTBL(p, ICallOrigOpts)->QueryInterface( p, cls, ppo )

#define ICALLORIGOPTS_AddOpt( p, po )                  AEEGETPVTBL(p, ICallOrigOpts)->AddOpt( p, po )
#define ICALLORIGOPTS_RemoveOpt( p, n, dx )            AEEGETPVTBL(p, ICallOrigOpts)->RemoveOpt( p, n, dx )
#define ICALLORIGOPTS_GetOpt( p, n, dx, po )           AEEGETPVTBL(p, ICallOrigOpts)->GetOpt( p, n, dx, po )

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                                ICallMgr                                       */
/*********************************************************************************/
AEEINTERFACE(ICallMgr)
{
   INHERIT_IWebOpts(ICallMgr);

   int (*Originate) ( ICallMgr *pme, 
                      AEETCallType callType, 
                      const char *pszNumber, 
                      const AECHAR *puzAlpha,
                      ICall **ppic, 
                      CallListener *pl );

   int (*OriginateEx) ( ICallMgr *pme, AEETCallType callType, 
                        ICall **ppic, 
                        CallListener *pl, 
                        AEETOriginateOpt *apwOptsZ );

   int (*GetCall) ( ICallMgr *pme, AEECallDesc cd, ICall **ppCall );
   int (*GetActiveVoiceCall) ( ICallMgr *pme, ICall **ppCall );
   int (*EndAllCalls) ( ICallMgr *pme );
};

#define ICALLMGR_AddRef( p )                          AEEGETPVTBL(p, ICallMgr)->AddRef( p )
#define ICALLMGR_Release( p )                         AEEGETPVTBL(p, ICallMgr)->Release( p )
#define ICALLMGR_QueryInterface( p, cls, ppo )        AEEGETPVTBL(p, ICallMgr)->QueryInterface( p, cls, ppo )

#define ICALLMGR_AddOrigOpt( p, po )                  AEEGETPVTBL(p, ICallMgr)->AddOpt( p, po )
#define ICALLMGR_RemoveOrigOpt( p, n, idx )           AEEGETPVTBL(p, ICallMgr)->RemoveOpt( p, n, idx )
#define ICALLMGR_GetOrigOpt( p, n, idx, po )          AEEGETPVTBL(p, ICallMgr)->GetOpt( p, n, idx, po )
#define ICALLMGR_Originate( p, t, pn, pa, ppic, pl )  AEEGETPVTBL(p, ICallMgr)->Originate( p, t, pn, pa, ppic, pl )
#define ICALLMGR_OriginateVoice( p, pn, ppic, pl )    AEEGETPVTBL(p, ICallMgr)->Originate( p, AEET_CALL_TYPE_VOICE, pn, NULL, ppic, pl )
#define ICALLMGR_OriginateEx( p, t, ppic, pl, pOp )   AEEGETPVTBL(p, ICallMgr)->OriginateEx( p, t, ppic, pl, pOp )

#define ICALLMGR_GetCall( p, cd, ppo )                AEEGETPVTBL(p, ICallMgr)->GetCall( p, cd, ppo )
#define ICALLMGR_GetActiveVoiceCall( p, ppo )         AEEGETPVTBL(p, ICallMgr)->GetActiveVoiceCall( p, ppo )

#define ICALLMGR_EndAllCalls( p )                     AEEGETPVTBL(p, ICallMgr)->EndAllCalls( p )

/* Terminating option Id */
#define AEET_ORIGOPT_END                        (WEBOPT_END)

/* Originate Options */
#define AEET_ORIGOPT_SERVICE_TYPE               (WEBOPT_32BIT+0x0001)
  /* AEETServiceType: automatic, amps etc. */

#define AEET_ORIGOPT_SERVICE_OPTION             (WEBOPT_32BIT+0x0002)
  /* AEETServiceOption: specifies service option. */

#define AEET_ORIGOPT_OTASP_ACTIVATION_CODE      (WEBOPT_32BIT+0x0003)
  /* AEETOTASPActivationCode: OTASP activation code */

#define AEET_ORIGOPT_CLIR_INFO                  (WEBOPT_32BIT+0x0004) 
  /* AEETCLIR: CLIR suppression / invocation etc. */

#define AEET_ORIGOPT_CALL_CONTROL_COMPLETE      (WEBOPT_32BIT+0x0005)
  /* Boolean: Denotes whether calling task has already applied call control */

#define AEET_ORIGOPT_CALL_TYPE_FALLBACK         (WEBOPT_32BIT+0x0006)
  /* AEETCallType: Fallback Bearer Information Element, negotiates SCUDIF call */

#define AEET_ORIGOPT_CALLED_NUMBER              (WEBOPT_SZ+0x0001)
  /* NULL terminated ASCIIZ string for Called party address */

#define AEET_ORIGOPT_CALLED_SUBADDR             (WEBOPT_SZ+0x0002)
  /* NULL terminated ASCIIZ string for Called party subaddress */

#define AEET_ORIGOPT_CALLING_NUMBER             (WEBOPT_SZ+0x0003)
  /* NULL terminated ASCIIZ string of calling number */

#define AEET_ORIGOPT_ALPHATAG                   (WEBOPT_WSZ+0x0001)
  /* A AECHAR type of wide string */

#define AEET_ORIGOPT_FWD_CUG_INFO               (WEBOPT_VARIABLE + (1 * WEBOPT_VARIABLE_INC) + (sizeof(AEETFwdCugInfo) & 0x00000FFF))
  /* AEETFwdCugInfo: Closed user Group information */

#define AEET_ORIGOPT_USER_DATA                  (WEBOPT_VARIABLE + (2 * WEBOPT_VARIABLE_INC) + (sizeof(AEETUserDataInfo) & 0x00000FFF))
  /* AEETUserDataInfo: User to User data information */

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                                   ICall                                       */
/*********************************************************************************/
AEEINTERFACE( ICall ) {
   INHERIT_IQueryInterface( ICall );
   
   int (*GetInfo) ( ICall *pme, AEETCallInfo *pInfo, uint32 dwSize );
   AEETCallType (*GetType) ( ICall *pme );
   AEETCallState (*GetState) ( ICall *pme );

   int (*Answer) ( ICall *pme );

   int (*End) ( ICall *pme );
   int (*Redirect) ( ICall *pme, const char *pszNumber );
   int (*Transfer) ( ICall *pme, ICall *pOtherParty );

   int (*Hold) ( ICall *pme );

   int (*GetCallDescriptor) ( ICall *pme, AEECallDesc *pcd );

   int (*BurstDTMF) ( ICall *pme, AEETDTMFOnLength on_length, AEETDTMFOffLength off_length, 
                      const char *pszDigits );
   int (*StartDTMF) ( ICall *pme, char bDigit );
   int (*StopDTMF) ( ICall *pme );
};

#define ICALL_AddRef( p )                    AEEGETPVTBL(p, ICall)->AddRef( p )
#define ICALL_Release( p )                   AEEGETPVTBL(p, ICall)->Release( p )
#define ICALL_QueryInterface( p, cls, ppo )  AEEGETPVTBL(p, ICall)->QueryInterface( p, cls, ppo )

#define ICALL_GetInfo( p, po, s )            AEEGETPVTBL(p, ICall)->GetInfo( p, po, s )
#define ICALL_GetType( p )                   AEEGETPVTBL(p, ICall)->GetType( p )
#define ICALL_GetState( p )                  AEEGETPVTBL(p, ICall)->GetState( p )

#define ICALL_Answer( p )                    AEEGETPVTBL(p, ICall)->Answer( p )
#define ICALL_End( p )                       AEEGETPVTBL(p, ICall)->End( p )
#define ICALL_Hold( p )                      AEEGETPVTBL(p, ICall)->Hold( p )
#define ICALL_Redirect( p, pn )              AEEGETPVTBL(p, ICall)->Redirect( p, pn )
#define ICALL_Transfer( p, pH )              AEEGETPVTBL(p, ICall)->Transfer( p, pH )
#define ICALL_SwitchTo( p )                  AEEGETPVTBL(p, ICall)->Answer( p )
#define ICALL_GetCallDescriptor( p, pcd )    AEEGETPVTBL(p, ICall)->GetCallDescriptor( p, pcd )

#define ICALL_BurstDTMF( p, on, off, pd )    AEEGETPVTBL(p, ICall)->BurstDTMF( p, on, off, pd )
#define ICALL_StartDTMF( p, bd )             AEEGETPVTBL(p, ICall)->StartDTMF( p, bd )
#define ICALL_StopDTMF( p )                  AEEGETPVTBL(p, ICall)->StopDTMF( p )

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                               IMultipartyCall                                 */
/*********************************************************************************/
AEEINTERFACE( IMultipartyCall ) {
   INHERIT_IQueryInterface( IMultipartyCall );

   int (*AddParty) ( IMultipartyCall *pme, ICall *pCall );
   int (*RemoveParty) ( IMultipartyCall *pme, ICall *pCall );
   int (*DisconnectParty) ( IMultipartyCall *pme, ICall *pCall );

   int (*GetNextCall) ( IMultipartyCall *pme, ICall *pCall, ICall **ppCall );
};

#define IMULTIPARTYCALL_AddRef( p )                    AEEGETPVTBL(p, IMultipartyCall)->AddRef( p )
#define IMULTIPARTYCALL_Release( p )                   AEEGETPVTBL(p, IMultipartyCall)->Release( p )
#define IMULTIPARTYCALL_QueryInterface( p, cls, ppo )  AEEGETPVTBL(p, IMultipartyCall)->QueryInterface( p, cls, ppo )

#define IMULTIPARTYCALL_AddParty( p, pc )              AEEGETPVTBL(p, IMultipartyCall)->AddParty( p, pc )
#define IMULTIPARTYCALL_RemoveParty( p, pc )           AEEGETPVTBL(p, IMultipartyCall)->RemoveParty( p, pc )
#define IMULTIPARTYCALL_DisconnectParty( p, pc )       AEEGETPVTBL(p, IMultipartyCall)->DisconnectParty( p, pc )
#define IMULTIPARTYCALL_GetNextCall( p, pc, ppc )      AEEGETPVTBL(p, IMultipartyCall)->GetNextCall( p, pc, ppc )

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                                 ISuppsTrans                                   */
/*********************************************************************************/
AEEINTERFACE( ISuppsTrans ) {

   INHERIT_IQueryInterface( ISuppsTrans );

   /* Parses the string for any errors */
   int (*ParseString)( ISuppsTrans *pme, const char *pszString, AEETSuppsMessage *pMsg, uint32 *pdwSize );

   /* Get the current state */
   AEETSuppsTransState (*GetState)( ISuppsTrans *pme );

   int (*LoadMessage)( ISuppsTrans *pme, const AEETSuppsMessage *pMsg, uint32 dwSize );

   void (*Run)( ISuppsTrans *pme, AEECallback *pcbWait );

   int (*GetResult)( ISuppsTrans *pme, const AEETSuppsStatus **ppResult );
};

#define ISUPPSTRANS_AddRef( p )                             AEEGETPVTBL(p, ISuppsTrans)->AddRef( p )
#define ISUPPSTRANS_Release( p )                            AEEGETPVTBL(p, ISuppsTrans)->Release( p )
#define ISUPPSTRANS_QueryInterface( p, cls, ppo )           AEEGETPVTBL(p, ISuppsTrans)->QueryInterface( p, cls, ppo )

#define ISUPPSTRANS_ParseString( p, psz, pM, ps )           AEEGETPVTBL(p, ISuppsTrans)->ParseString(p, psz, pM, ps )
#define ISUPPSTRANS_GetState( p )                           AEEGETPVTBL(p, ISuppsTrans)->GetState( p )
#define ISUPPSTRANS_LoadMessage( p, pM, s )                 AEEGETPVTBL(p, ISuppsTrans)->LoadMessage( p, pM, s )
#define ISUPPSTRANS_Run( p, pcb )                           AEEGETPVTBL(p, ISuppsTrans)->Run( p, pcb )
#define ISUPPSTRANS_GetResult( p, ppR )                     AEEGETPVTBL(p, ISuppsTrans)->GetResult( p, ppR )

/*********************************************************************************/
/*                   I N T E R F A C E    D E F I N I T I O N                    */
/*                                  IPhoneCtl                                    */
/*********************************************************************************/
AEEINTERFACE( IPhoneCtl ) {
   INHERIT_IQueryInterface( IPhoneCtl );
   int (*PhoneCtl_Command) ( IPhoneCtl *pme, uint32 dwCmd, AEECallback *pcb, 
                             void *pInBuffer, uint32 dwInBufSize, void *pOutBuffer, uint32 dwOutBufSize );

   int (*SuppsCallOp) ( IPhoneCtl *pme, const char *pszString );

   /* Helper routines */
   int (*IsSuppsCallOp) ( IPhoneCtl *pme, 
                             const char *pszString,    /* [in] A string to parse and identify whether 
                                                       ** it is a valid Supplementary service request */
                             uint32 *pnum );           /* [out] Returns the Call X if any on call-related
                                                       ** request. */

   int (*GetSuppsTrans) ( IPhoneCtl *pme, AEETSuppsTransId tid, ISuppsTrans **ppTrans );
};

/* IPhoneCtl Commands */
#define AEEPHCTL_CMD_SET_SYSTEM_PREFERENCE      0x0001
#define AEEPHCTL_CMD_SET_NAM_SEL                0x0002
#define AEEPHCTL_CMD_SET_OPERATING_MODE         0x0003
#define AEEPHCTL_CMD_SET_SUBSCRIPTION_STATUS    0x0004
#define AEEPHCTL_CMD_SET_RSSI_DELTA_THRESHOLD   0x0005
#define AEEPHCTL_CMD_SET_PREFERRED_NETWORKS     0x0006
#define AEEPHCTL_CMD_WAKEUP_FROM_STANDBY        0x0007
#define AEEPHCTL_CMD_SET_RTRE_CONFIG            0x0008
#define AEEPHCTL_CMD_SET_ANSWER_VOICE_PREF      0x0009
#define AEEPHCTL_CMD_AVOID_SERVING_SYSTEM       0x000A
#define AEEPHCTL_CMD_SET_VOICE_PRIVACY          0x000D
#define AEEPHCTL_CMD_RESET_ACM                  0x000F
#define AEEPHCTL_CMD_SET_ACMMAX                 0x0010
#define AEEPHCTL_CMD_SET_DDTMPREF               0x0011
#define AEEPHCTL_CMD_SELECT_LINE                0x0012
#define AEEPHCTL_CMD_ENABLE_LINESWITCH          0x0013
#define AEEPHCTL_CMD_REFRESH_NETWORKS_LIST      0x0014

#define IPHONECTL_AddRef( p )                      AEEGETPVTBL(p, IPhoneCtl)->AddRef( p )
#define IPHONECTL_Release( p )                     AEEGETPVTBL(p, IPhoneCtl)->Release( p )
#define IPHONECTL_QueryInterface( p, c, po )       AEEGETPVTBL(p, IPhoneCtl)->QueryInterface( p, c, po )
#define IPHONECTL_IsSuppsCallOp( p, ps, pn )       AEEGETPVTBL(p, IPhoneCtl)->IsSuppsCallOp( p, ps, pn )
#define IPHONECTL_SuppsCallOp( p, ps )             AEEGETPVTBL(p, IPhoneCtl)->SuppsCallOp( p, ps )
#define IPHONECTL_GetSuppsTrans( p, t, ppT )       AEEGETPVTBL(p, IPhoneCtl)->GetSuppsTrans( p, t, ppT )

static __inline int IPHONECTL_SetSystemPreference( IPhoneCtl *pme, AEECallback *pcb, AEETSystemPreference *pPref, uint32 dwSize, int *pnStatus )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_SYSTEM_PREFERENCE, pcb, 
                                                         (void *)pPref, dwSize, 
                                                         (void *)pnStatus, sizeof(int) );
}

static __inline int IPHONECTL_SetNAMSel( IPhoneCtl *pme, AEETNAM nam )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_NAM_SEL, NULL, 
                                                         (void *)nam, 0,
                                                         (void *)0, 0 );
}

static __inline int IPHONECTL_SetOperatingMode( IPhoneCtl *pme, AEETOprtMode mode )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_OPERATING_MODE, NULL, 
                                                         (void *)mode, 0,
                                                         (void *)0, 0 );
}

static __inline int IPHONECTL_SetSubscriptionStatus( IPhoneCtl *pme, AEETSysMode mode, boolean status )
{
   struct {
      AEETSysMode sm;
      boolean     st;
   } local;

   local.sm = mode;
   local.st = status;
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_SUBSCRIPTION_STATUS, NULL, 
                                                         &local, sizeof( local ),
                                                         (void *)0, 0 );
}

static __inline int IPHONECTL_SetRSSIDeltaThreshold( IPhoneCtl *pme, byte thr_delta )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_RSSI_DELTA_THRESHOLD, NULL, 
                                                         (void *)(uint32)thr_delta, 0,
                                                         (void *)0, 0 );
}

static __inline int IPHONECTL_SetPrefNetworks( IPhoneCtl *pme, AEECallback *pcb, AEETUserPLMNList *pPref, uint32 dwSize, int *pnStatus )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_PREFERRED_NETWORKS, pcb, 
                                                         pPref, dwSize,
                                                         (void *)pnStatus, sizeof(int) );
}

static __inline int IPHONECTL_WakeupFromStandby( IPhoneCtl *pme )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_WAKEUP_FROM_STANDBY, NULL, 
                                                         NULL, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_SetRTREConfig( IPhoneCtl *pme, AEETRTREConfig config )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_RTRE_CONFIG, NULL, 
                                                         (void *)config, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_SetAnswerVoicePref( IPhoneCtl *pme, AEETAnsVoicePref pref, uint32 dwDuration )
{
   struct {
      AEETAnsVoicePref p;
      uint32 dwD;
   }local;

   local.p = pref;
   local.dwD = dwDuration;
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_ANSWER_VOICE_PREF, NULL, 
                                                         &local, sizeof(local),
                                                         NULL, 0 );
}

static __inline int IPHONECTL_AvoidServingSystem( IPhoneCtl *pme, AEETAvoidSSPref pref, uint32 dwDuration )
{
   struct {
      AEETAvoidSSPref p;
      uint32 dwD;
   }local;

   local.p = pref;
   local.dwD = dwDuration;
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_AVOID_SERVING_SYSTEM, NULL, 
                                                         &local, sizeof(local),
                                                         NULL, 0 );
}

static __inline int IPHONECTL_SetVoicePrivacy( IPhoneCtl *pme, AEETPrivacyPref pref )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_VOICE_PRIVACY, NULL, 
                                                         (void *)pref, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_ResetACM( IPhoneCtl *pme )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_RESET_ACM, NULL, 
                                                         NULL, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_SetACMMax( IPhoneCtl *pme, uint32 dwNewMax )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_ACMMAX, NULL, 
                                                         (void *)dwNewMax, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_SetDDTMPref( IPhoneCtl *pme, AEETDDTMPref pref )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SET_DDTMPREF, NULL, 
                                                         (void *)pref, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_SelectLine( IPhoneCtl *pme, uint32 lineid )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_SELECT_LINE, NULL, 
                                                         (void *)lineid, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_EnableLineSwitch( IPhoneCtl *pme, boolean bEnable )
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command( pme, AEEPHCTL_CMD_ENABLE_LINESWITCH, NULL,
                                                         (void *)(uint32)bEnable, 0,
                                                         NULL, 0 );
}

static __inline int IPHONECTL_RefreshAvailableNetworks(IPhoneCtl *pme,  AEECallback *pcb, 
                                                       int *pnStatus)
{
   return AEEGETPVTBL(pme, IPhoneCtl)->PhoneCtl_Command(pme,
                                                        AEEPHCTL_CMD_REFRESH_NETWORKS_LIST,
                                                        pcb,
                                                        NULL, 0,
                                                        (void*)pnStatus, sizeof(int));
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEET_NMASK_VOICE_CALL

Description:
   This notification mask is to be used to register for any state changes in voice calls
of the device. Whenever there is a status change applications receive EVT_NOTIFY event. The
dwParam for this event is of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_VOICE_CALL    0x0001

Supported Values:

Comments:
   This notification is also sent for any emergency calls, OTASP (std and non-std) calls.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_DATA_CALL

Description:
   This notification mask is to be used to register for any state changes in data calls
of the device. Whenever there is a status change, applications receive EVT_NOTIFY event. The
dwParam for this event is of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_DATA_CALL    0x0002

Supported Values:

Comments:
   This notification is sent for both circuit-switch and packet-switch type of data calls.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_TEST_CALL

Description:
   This notification mask is to be used to register for any state changes in Test calls
of the device. Whenever there is a status change applications receive EVT_NOTIFY event. The
dwParam for this event is of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_TEST_CALL    0x0004

Supported Values:

Comments:
   Examples of test calls include : Markov, Loopback, etc.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_OTHER_CALL

Description:
   This notification mask is to be used to register for any state changes in the calls
of the device that are not covered by the other masks (AEET_NMASK_VOICE_CALL, AEET_NMASK_DATA_CALL,
AEET_NMASK_TEST_CALL). Whenever there is a status change applications receive EVT_NOTIFY event. 
The dwParam for this event is of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_OTHER_CALL    0x0008

Supported Values:

Comments:
   None.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_PHONE

Description:
   This notification mask is to be used to register for any state changes in the phone.
Whenever there is a status change applications receive EVT_NOTIFY event. The dwParam for 
this event is of type AEENotify. The pData member of which is of type AEETNotifyInfo. 

Definition:

#define AEET_NMASK_PHONE    0x0010

Supported Values:

Comments:
   The event member of AEETNotifyInfo indicates the type of phone Event. The member
event_data.ph of AEETNotifyInfo contains the corresonding data.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_SS

Description:
   This notification mask is to be used to register for any changes in the serving system
information. Applications are notified with EVT_NOTIFY event. The dwParam for this event is 
of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_SS    0x0020

Supported Values:

Comments:
   The event member of AEETNotifyInfo indicates the type of data that changed. The member
event_data.ss of AEETNotifyInfo contains the corresonding data.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_INBAND

Description:
   This notification mask is to be used to register for notifications on any DTMF message 
recieved in a call. Applications are notified with EVT_NOTIFY event. The dwParam for this
event is of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_INBAND    0x0040

Supported Values:

Comments:
   The event member of AEETNotifyInfo indicates the type of DTMF data received. The member
event_data.inb of AEETNotifyInfo contains the corresonding data.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER

======================================================================= 

AEET_NMASK_NEW_CALLDESC

Description:
   This notification mask is to be used to register for notifications on any new call descriptor.
This happens as a result of a Call origination or an incoming call. Whenever this occurs applications 
receive EVT_NOTIFY event. The dwParam for this event is of type AEENotify. The pData member of which 
is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_NEW_CALLDESC  0x0100

Supported Values:
   None.

Comments:
   None.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER
   
=======================================================================

AEET_NMASK_NEW_NI_SUPP_TRANS

Description:
   This notification mask is to be used to register for notifications on any network initiated 
supplementary service transaction. Whenever this occurs applications receive EVT_NOTIFY event. 
The dwParam for this event is of type AEENotify. The pData member of which is of type AEETNotifyInfo.

Definition:

#define AEET_NMASK_NEW_NI_SUPP_TRANS  0x0200

Supported Values:
   None.

Comments:
   None.

See Also:
   AEETNotifyInfo, AEECLSID_PHONENOTIFIER
   
=======================================================================

AEET_ORIGOPT_

Description:
   The prefix AEET_ORIGOPT_ is used to specify various call origination options.

Definition:
   #define AEET_ORIGOPT_END                        (WEBOPT_END)

   #define AEET_ORIGOPT_SERVICE_TYPE               (WEBOPT_32BIT+0x0001)

   #define AEET_ORIGOPT_SERVICE_OPTION             (WEBOPT_32BIT+0x0002)

   #define AEET_ORIGOPT_OTASP_ACTIVATION_CODE      (WEBOPT_32BIT+0x0003)

   #define AEET_ORIGOPT_CLIR_INFO                  (WEBOPT_32BIT+0x0004) 

   #define AEET_ORIGOPT_CALL_CONTROL_COMPLETE      (WEBOPT_32BIT+0x0005)

   #define AEET_ORIGOPT_CALL_TYPE_FALLBACK         (WEBOPT_32BIT+0x0006)

   #define AEET_ORIGOPT_CALLED_NUMBER              (WEBOPT_SZ+0x0001)

   #define AEET_ORIGOPT_CALLED_SUBADDR             (WEBOPT_SZ+0x0002)

   #define AEET_ORIGOPT_CALLING_NUMBER             (WEBOPT_SZ+0x0003)

   #define AEET_ORIGOPT_ALPHATAG                   (WEBOPT_WSZ+0x0001)

   #define AEET_ORIGOPT_FWD_CUG_INFO               (WEBOPT_VARIABLE + (1 * WEBOPT_VARIABLE_INC) + (sizeof(AEETFwdCugInfo) & 0x00000FFF))

   #define AEET_ORIGOPT_USER_DATA                  (WEBOPT_VARIABLE + (2 * WEBOPT_VARIABLE_INC) + (sizeof(AEETUserDataInfo) & 0x00000FFF))

Members:

   AEET_ORIGOPT_END : Terminating option Id

   AEET_ORIGOPT_SERVICE_TYPE : automatic, amps etc (see AEETServiceType).

   AEET_ORIGOPT_SERVICE_OPTION : specifies service option (see AEETServiceOption).

   AEET_ORIGOPT_OTASP_ACTIVATION_CODE : OTASP activation code (see AEETOTASPActivationCode).

   AEET_ORIGOPT_CLIR_INFO : CLIR suppression / invocation etc. (see AEETCLIR) .

   AEET_ORIGOPT_CALL_CONTROL_COMPLETE  : Denotes whether calling task has 
                                         already applied call control, boolean.

   AEET_ORIGOPT_CALL_TYPE_FALLBACK : Fallback Bearer Information Element, 
                                     negotiates SCUDIF call (see AEETCallType).

   AEET_ORIGOPT_CALLED_NUMBER : Called party address (NULL terminated ASCIIZ string).

   AEET_ORIGOPT_CALLED_SUBADDR : Called party subaddress (NULL terminated ASCIIZ string).

   AEET_ORIGOPT_CALLING_NUMBER :  calling number (NULL terminated ASCIIZ string).

   AEET_ORIGOPT_ALPHATAG : A AECHAR type of wide string. 

   AEET_ORIGOPT_FWD_CUG_INFO : Closed user Group information (see AEETFwdCugInfo).
   
   AEET_ORIGOPT_USER_DATA : User to User data information.
   
Comments:
   None.

See Also:
   ICALLMGR_AddOrigOpt()
   ICALLMGR_RemoveOrigOpt()
   ICALLMGR_GetOrigOpt()
   
======================================================================= 
======================================================================= 
INTERFACES   DOCUMENTATION
======================================================================= 
PhoneNotifier Interface

Description:

   This interface provides asynchronous notifications. Applications registered
with one or more notification mask AEET_NMASK_* will be notified with EVT_NOTIFY.

List of Header files to be included:

   AEETelephone.h

======================================================================= 

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
ITelephone Interface

Description:

   This interface provides services to read phones state and information. This
interface does not require any special privileges.

   This interface also supports IModel. To obtain IModel use ITELEPHONE_QueryInterface()
with AEEIID_MODEL. A listener added to this model will be notified of all the calls,
phone, serving system, inband messages and events.

List of Header files to be included:

   AEETelephone.h

======================================================================= 

ITELEPHONE_AddRef()

Description: 
   This function is inherited from IBASE_AddRef().

See Also:
   ITELEPHONE_Release()

======================================================================= 

ITELEPHONE_Release()

Description: 
   This function is inherited from IBASE_Release().

See Also:
   ITELEPHONE_AddRef()
======================================================================= 

ITELEPHONE_QueryInterface()

Description: 
   This function can be used to
      -    Get a pointer to an interface or data based on the input class ID
      -    Query an extended version of the ITelephone-derived class
      -    Support version compatibility

Prototype:
   int ITELEPHONE_QueryInterface( ITelephone *pme, AEECLSID clsid, void **ppo );
   
Parameters:
   pme : [in] :  Pointer to ITelephone Interface.
   clsReq : [in] :  A globally unique id to identify the entity (interface or
data) that we are trying to query.
   ppo : [out] :  Pointer to the interface or data that we want to retrieve. If
the value passed back is NULL, the interface or data that we query are not available.

Return Value:
   SUCCESS: on success,    
   EBADPARM : if ppo is NULL.
   ECLASSNOTSUPPORT : if the clsid is not supported by this interface.

Comments:  
   None.

Side Effects: 
   If an interface is retrieved, then this function increments its reference count.

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ITELEPHONE_GetPhoneInfo()

Description: 
   Gets the current phone information to the given buffer.

Prototype:
   int ITELEPHONE_GetPhoneInfo( ITelephone *pme, AEETPhInfo *pPhInfo, uint32 dwSize );
   
Parameters:
   pme : [in] : The interface pointer.
   pPhInfo : [out] : Pointer to AEETPhInfo buffer.
   dwSize : [in] : sizeof buffer in bytes available at pPhInfo.

Return Value:
   SUCCESS : the function succeeded in writing Phone Info to the given memory.
   EBADPARM : Either pPhInfo is NULL or dwSize is insufficient size.

Comments: 
   None 

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   AEETPhInfo

======================================================================= 

ITELEPHONE_GetCallInfo()

Description: 
   Retrieves the requested call's (identified by call descriptor cd) information.

Prototype:
   int ITELEPHONE_GetCallInfo( ITelephone *pme, AEECallDesc cd, AEETCallInfo *pCallInfo, uint32 dwSize );

Parameters:
   pme : [in] : The interface pointer.
   cd : [in] : Call descriptor.
   AEETCallInfo : [out] : Pointer to AEETCallInfo.
   dwSize : [in] : Size in bytes of memory available at pCallInfo.
   

Return Value:
   SUCCESS : the function succeeded in writing Call Information to the given memory location.
   EBADPARM : Either pCallInfo is NULL or dwSize is insufficient size.
   AEET_ERR_BADDESC : given descriptor is invalid descriptor or state.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   AEETCallInfo

======================================================================= 

ITELEPHONE_GetCalls()

Description: 
   This function writes to the given memory location the list of current active Call Descriptors in the system.

Prototype:
   int ITELEPHONE_GetCalls( ITelephone *pme, AEETCalls *po, uint32 dwSize );

Parameters:
   pme : [in] : The interface pointer.
   po : [out] : Buffer to write the active calls data.
   dwSize : [in] : amount of memory in bytes available at address po.

Return Value:
   SUCCESS : the function succeeded in writing data describing active calls to the given location in memory.
   EBADPARM : Either po is NULL or dwSize is less than 4 bytes.

Comments:
   On SUCCESS, the dwCount member of AEETCalls will indicate the number of active calls in the
system. The number of descriptors written to the given memory location is limited to available space.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ITELEPHONE_GetServingSystemInfo()

Description: 
   Retrieve the serving system information to given buffer.

Prototype:
   int ITELEPHONE_GetServingSystemInfo( ITelephone *pme, AEETSSInfo *pSSInfo, uint32 dwSize );

Parameters:
   pme : [in] : The interface pointer.
   pSSInfo : [out] : Buffer to write the Serving System information.
   dwSize : [in] : size in bytes of memory available at pSSInfo.

Return Value:
   SUCCESS : the function succeeded.
   EBADPARM : Either the pSSInfo is NULL or dwSize is insufficient.
   
Comments: 
   EVDO Service is available if 
      (1) sys_mode is AEET_SYS_MODE_HDR and srv_status is AEET_SRV_STATUS_SRV or
      (2) hdr_hybrid is TRUE and hdr_srv_status is AEET_SRV_STATUS_SRV.  


Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None
======================================================================= 

ITELEPHONE_GetNetworks()

Description: 
   Get a list of networks depending on the type specified.

Prototype:
   int ITELEPHONE_GetNetworks( ITelephone *pme, AEETNetworkType t, AEECallback *pcb, void *pOut, uint32 dwSize );

Parameters:
   pme : [in] : The interface pointer.
   t : [in] : Type of networks.
   pcb : [in] : Callback to invoke when the operation is complete.
   pOut : [out] : buffer which will contain network information. 
   dwSize : [in] : size of memory in bytes at address pOut.

Return Value:
   SUCCESS : The request is being processed and response will be available in asynchronous fashion to pcb.
   EBADPARM : one or more of the parameters has invalid data or NULL pointer.
   EUNSUPPORTED : Not supported.
   
Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ITELEPHONE_GetMaxCallDescriptors()

Description: 
   Gets the maximum call descriptors available to the system. Indicates the maximum 
number of simultaneous calls the system is capable of holding.

Prototype:
   int ITELEPHONE_GetMaxCallDescriptors( ITelephone *pme, uint32 *pdwCount );

Parameters:
   pme : [in] : The interface pointer.
   pdwCount : [out] : the maximum number of call descriptors system support.

Return Value:
   SUCCESS : The request is complete.
   EBADPARM : pdwCount is NULL.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ITELEPHONE_GetLineInfo()

Description: 
   Gets the line support information of the phone.

Prototype:
   int ITELEPHONE_GetLineInfo( ITelephone *pme, AEETLineInfo *po, uint32 dwSize );

Parameters:
   pme : [in] : The interface pointer.
   po : [out] : Buffer where the line information is returned.
   dwSize : [in] : size of the buffer.

Return Value:
   SUCCESS : The request is complete.
   EBADPARM : po is NULL.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1.2 

See Also:
   None

======================================================================= 

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
ICallMgr Interface

Description:

   This interface provides servics that affect telephone calls in the system. 
These methods alter the state and resources of phone. These methods if used 
incorrectly may render the device unusable. Access to this interface go through 
a privilege check. Applications using this interface must explicitly specify 
the use of this object in the access control list.

List of Header files to be included:

   AEETelephone.h

======================================================================= 

ICALLMGR_AddRef()

Description: 
   This function is inherited from IBASE_AddRef().

See Also:
   ICALLMGR_Release()

======================================================================= 

ICALLMGR_Release()

Description: 
   This function is inherited from IBASE_Release().

See Also:
   ICALLMGR_AddRef()
   
======================================================================= 

ICALLMGR_QueryInterface()

Description: 
   This function is inherited from IQI_QueryInterface().

See Also:
   None

======================================================================= 

ICALLMGR_Originate()

Description: 
   Originates a telephone call of given type to the given destination address. 
Optionally adds a listener on the call so application is notified of state changes 
on the telephone call.

Prototype:
   int ICALLMGR_Originate( ICallMgr *pme, 
                           AEETCallType ct, 
                           const char *pszNumber, 
                           const AECHAR *puzAlpha, 
                           ICall **ppCall,
                           CallListener *pl ); 

Parameters:
   pme : [in] : the Interface pointer.
   ct : [in] : type of call to originate.
   pszNumber : [in] : number to process.
   puzAlpha : [in] : Alpha numeric text for MMI. Can be NULL.
   ppCall : [out] : Creates the ICall object on SUCCESS.
   pl : [in] : A listener to be notified of call states. Can be NULL.

Return Value:
   SUCCESS : The call origination is in progress. Applications must register
             a model listener to keep track of call states to determine if
             call origination was successful.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels
               to invoke this function.

   ENOMEMORY: Out of application heap.

   AEET_ERR_OUTOF_RESOURCES : device is out of resources to originate a call.

Comments:
   An error AEET_ERR_OUTOF_RESOURCES is a temporary condition that is dependent on
the state of device and network's capability. For example : If the device is already in a 
voice call, an originate request for call type AEET_CALL_TYPE_CS_DATA, if the device cannot
handle, will likely result in AEET_ERR_OUTOF_RESOURCES.
   
Side Effects:
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

ICALLMGR_OriginateVoice()

Description: 
   Originates a voice call to given destination address.

Prototype:
   int ICALLMGR_OriginateVoice( ICallMgr *pme, 
                                const char *pszNumber, 
                                ICall **ppCall, 
                                CallListener *pl );

Parameters:
   pme : [in] : Pointer to interface.
   pszNumber : [in] : number to dial.
   ppCall : [out] : Returns a ICall Object on Success.
   pl : [in] : A listener to be notified of call states.

Return Value:
   SUCCESS : the call origination is in progress. Applications must register
             a model listener to keep track of call states to determine if
             call origination was successful.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels
               to invoke this function.

   ENOMEMORY: Out of application heap.

   AEET_ERR_OUTOF_RESOURCES : device is out of resources to originate a call.

Comments:
   None.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   ICALLMGR_Originate().

======================================================================= 

ICALLMGR_OriginateEx()

Description: 
   An advanced method to originate a call. It accepts a vector of origination
options. Any of the options if present override the options of ICallMgr.

Prototype:
   int ICALLMGR_OriginateEx( ICallMgr *pme, 
                                AEETCallType callType, 
                                ICall **ppic, 
                                CallListener *pl, 
                                AEETOriginateOpt *apwOptsZ );

Parameters:
   pme : [in] : Pointer to interface.
   callType : [in] : type of the telephone call.
   ppic : [out] : Returns a ICall Object on Success.
   pl : [in] : A listener to be notified of call states.
   apwOptsZ : [in] : A vector of origination options.

Return Value:
   SUCCESS : the call origination is in progress. Applications must register
             a model listener to keep track of call states to determine if
             call origination was successful.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels
               to invoke this function.

   ENOMEMORY: Out of application heap.

   AEET_ERR_OUTOF_RESOURCES : device is out of resources to originate a call.

Comments:
   None.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

ICALLMGR_AddOrigOpt()

Description: 
   Load the vector of origination options. The identifier in the {nId,pVal} 
pair of each AEETOrigianteOpt must be given according to the AEET_ORIGOPT_* 
values. Each of option content at pVal is copied to internal buffers.

Prototype:
   int ICALLMGR_AddOrigOpt( ICallMgr *po, AEETOriginateOpt *apwoz );

Parameters:
   po : the interface pointer.
   apwoz : array of call origination options terminated by AEET_ORIGOPT_END.

Return Value:
   SUCCESS: if the options could be added successfully
   ENOMEMORY: if enough memory couldn't be allocated

Comments: 
   When multiple values are present on an option, ICALLMGR uses the last added
option. i.e, at index 0 : ICALLMGR_GetOrigOpt( p, id, 0, pval );

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   AEETOriginateOpt, IWEBOPTS_AddOpt() 

======================================================================= 

ICALLMGR_RemoveOrigOpt()

Description: 
   Removes the option identified by nOptId at the index. The option identifier must be
one of AEET_ORIGOPT_* values. 

Prototype:
   int ICALLMGR_RemoveOrigOpt( ICallMgr *po, int32 nOptId, int32 nIndex );

Parameters:
   po : the interface pointer.
   nOptId : the option identifier.
   nIndex : which (if there are multiple AEETOrigOpt of the same id) to 
                  remove nIndex counts from most recently AddOpt()ed option

Return Value:
   SUCCESS : the option at the index cleared.
   EFAILED : specified option is not found.

Comments: 
   To remove all values of an option :
   
      while( SUCCESS == ICALLMGR_RemoveOrigOpt( po, id, 0 ) ) {}

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   IWEBOPTS_RemoveOpt().

======================================================================= 

ICALLMGR_GetOrigOpt()

Description: 
   Retrieves the origination option identified by nOptId at the index nIdx. The option 
identifier must be one of AEET_ORIGOPT_* values. When succeeded, depending on the type 
of data retrieved in pwo->pVal will be referring to internal data buffer. Application 
must make a copy depending on the scope of use.

Prototype:
   int ICALLMGR_GetOrigOpt( ICallMgr *po, int32 nOptId, int32 nIdx, AEETOriginateOpt *pwo );

Parameters:
   po : the interface pointer.
   nOptId : Origination option to retrieve.
   nIdx:  which (if there are multiple WebOpts of the same id) to get
                 nIndex counts from most recently AddOpt()ed option, it is
                 zero-based
   pwo : buffer to place the option information.

Return Value:
   SUCCESS : When the option is found.
   EFAILED : the option is cleared or not initialized.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   IWEBOPTS_GetOpt().

======================================================================= 

ICALLMGR_GetCall()

Description: 
   Returns a reference to ICall given a call descriptor.

Prototype:
   int ICALLMGR_GetCall( ICallMgr *pme, AEECallDesc cd, ICall **ppCall );

Parameters:
   pme : [in] : Pointer to interface.
   cd : [in] : Call Descriptor.
   ppCall : [out] : returns the instance of ICall.

Return Value:
   SUCCESS : An ICall instance is succesfully placed in *ppCall.

   AEET_ERR_BADDESC : Given call descriptor is bad.

   ENOMEMORY : out of space on heap.

Comments:
   Application must release the reference when done using the ICall instance.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None
======================================================================= 

ICALLMGR_GetActiveVoiceCall()

Description: 
   Returns a reference to active voice ICall.

Prototype:
   int ICALLMGR_GetActiveVoiceCall( ICallMgr *pme, ICall **ppCall )

Parameters:
   pme : [in] : Pointer to interface.
   ppCall : [out] : returns the instance of ICall.

Return Value:
   SUCCESS : An ICall instance is succesfully placed in *ppCall.
   ENOSUCH : no active voice call.
   ENOMEMORY : out of space on heap.
   EBADPARM : Invalid Parameter.

Comments:
   Application must release the reference when done using the ICall instance.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1

See Also:
   ICALLMGR_GetCall().

=======================================================================

ICALLMGR_EndAllCalls()

Description: 
   Terminates all calls that are in connected state. This includes the calls on hold.

Prototype:
   int ICALLMGR_EndAllCalls( ICallMgr *pme );

Parameters:
   pme : [in] : Pointer to interface.

Return Value:
   SUCCESS : the request was accepted.

Comments:  
   None.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
ICallOrigOpts Interface

Description:

   This interface provides a way of building origination options. These
options can be passed to ICALLMGR_OriginateEx().

List of Header files to be included:

   AEETelephone.h

======================================================================= 

ICALLORIGOPTS_AddRef()

Description: 
   This function is inherited from IWEBOPTS_AddRef().

See Also:
   ICALLORIGOPTS_Release()

======================================================================= 

ICALLORIGOPTS_Release()

Description: 
   This function is inherited from IWEBOPTS_Release().

See Also:
   ICALLORIGOPTS_AddRef()

======================================================================= 

ICALLORIGOPTS_QueryInterface()

Description: 
   This function is inherited from IWEBOPTS_QueryInterface().

See Also:
   None

======================================================================= 

ICALLORIGOPTS_AddOpt()

Description: 
   This function is inherited from IWEBOPTS_AddOpt().

See Also:
   None

======================================================================= 

ICALLORIGOPTS_RemoveOpt()

Description: 
   This function is inherited from IWEBOPTS_RemoveOpt().

See Also:
   None

======================================================================= 

ICALLORIGOPTS_GetOpt()

Description: 
   This function is inherited from IWEBOPTS_GetOpt().

See Also:
   None

======================================================================= 

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
ICall Interface

Description:

   This interface provides call related services. These methods alter the
state and resources of phone. These methods if used incorrectly may render
the device unusable. Access to this interface go through a privilege check.
Applications using this interface must explicitly specify the use of this
object in the access control list.

   This interface also implements IModel. An instance of IModel can be 
obtained with Query for AEEIID_MODEL.

List of Header files to be included:

   AEETelephone.h

======================================================================= 

ICALL_AddRef()

Description: 
   This function is inherited from IBASE_AddRef().

See Also:
   ICALL_Release()

======================================================================= 

ICALL_Release()

Description: 
   This function is inherited from IBASE_Release().

See Also:
ICALL_AddRef()

======================================================================= 

ICALL_QueryInterface()

Description: 
   This function is inherited from IQI_QueryInterface().

See Also:
   None

======================================================================= 

ICALL_GetInfo()

Description: 
   Gets the information regarding the call.

Prototype:
   int ICALL_GetInfo( ICall *pme, AEETCallInfo *pInfo, uint32 dwSize );

Parameters:
   pme : [in] : the interface pointer.
   pInfo : [out] : address to which AEETCallInfo data will be written.
   dwSize : [in] : size of memory in bytes at pInfo.

Return Value:
   SUCCESS : call information is written successfully.

   EBADPARM : either pInfo is NULL or dwSize is insufficient

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function

Comments:  
   None.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

ICALL_GetType()

Description: 
   Gets the call type.

Prototype:
   AEETCallType ICALL_GetType( ICall *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   AEETCallType.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_GetState()

Description: 
   Gets the call state.

Prototype:
   AEETCallState ICALL_GetState( ICall *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   AEETCallState.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_Answer()

Description: 
   Answers the call if it is in incoming(or waiting) state. Activates the call if it is
in hold state.

Prototype:
   int ICALL_Answer( ICall *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   SUCCESS : when the answer request is accepted. Applications must register
             a model listener to keep track of call states to determine if
             answer request was satisfied.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function.

   EALREADY: An answer request was already in progress or the call is in
             conversation state.

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_End()

Description: 
   Performs one of the following operations depending on the state of Call :
      - Terminates the call in conversation state. 
      - Cancels the call if it is in origination. 
      - Rejects if it is incoming.
      
Prototype:
   int ICALL_End( ICall *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   SUCCESS : if the request is accepted. Applications must register
             a model listener to keep track of call states to determine if
             end request was satisfied.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function

   EALREADY: An end request was already in progress or the call is in
             idle state.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_Redirect()

Description: 
      Redirects an incoming call to the given address.

Prototype:
   int ICALL_Redirect( ICall *pme, const char *pszNumber );

Parameters:
   pme : [in] : the interface pointer.
   pszNumber : [in] : the null terminated digit buffer containing the address.

Return Value:
   SUCCESS : if the request is accepted. Applications must register
             a model listener to keep track of call states to determine if
             redirect request was satisfied.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function

   EALREADY: if the redirect request is already in progress or the call is 
               already in IDLE state.  

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_Transfer()

Description: 
      Transfers this call to the given call destination.

Prototype:
   int ICALL_Transfer( ICall *pme, ICall *pOtherCall );

Parameters:
   pme : [in] : the interface pointer.
   pOtherCall : [in] : the party to transfer.

Return Value:
   SUCCESS : if the request is accepted. Applications must register
             a model listener to keep track of call states to determine if
             transfer request was satisfied.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function

   EALREADY: if the forward request is already in progress or the call is 
               already in IDLE state.  

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_Hold()

Description: 
   Puts the call on hold. There can be atmost one call onhold on the device.

Prototype:
   int ICALL_Hold( ICall *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   SUCCESS : if the request is accepted. Applications must register
             a model listener to keep track of call states to determine if
             hold request was satisfied.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function

   EALREADY: if the hold request is already in progress or the call is 
               already in ONHOLD state.

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments:  
   None.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_SwitchTo()

Description: 
   Activates this call if on hold or if this is active call and it supports
switching (such as sending inband flash message).

Prototype:
   int ICALL_SwitchTo( ICall *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   SUCCESS : if the request is accepted. Applications must register
             a model listener to keep track of call states to determine if
             the request was satisfied.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function

   EALREADY: if the forward request is already in progress or the call is 
               already in IDLE state.  

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_GetCallDescriptor()

Description: 
   Returns the call descriptor associated with the object.

Prototype:
   int ICALL_GetCallDescriptor( ICall *pme, AEECallDesc *pcd );

Parameters:
   pme : [in] : the interface pointer.
   pcd : [out] : call descriptor associated with the object.

Return Value:
   SUCCESS : call descriptor is written to pcd.

   EBADPARM : pcd is NULL.

   AEET_ERR_BADDESC : The descriptor associated with ICall object is invalid.

Comments:
   When a call returns to IDLE state, the associated descriptor becomes invalid.
However the information obtained with ICALL_GetInfo() is valid.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_BurstDTMF()

Description: 
   Sends a sequence of DTMFs.

Prototype:
   int ICALL_BurstDTMF( ICall *pme, AEETDTMFOnLength on_length, AEETDTMFOffLength off_length, const char *pszDigits );

Parameters:
   pme : [in] : pointer to interface.
   on_length : [in] : duration of DTMF tone on each digit.
   off_length  : [in] : pause interval between digits.
   pszDigits : [in] : digit buffer.

Return Value:
   SUCCESS : when the request is being processed.

   AEET_ERR_OP_NOT_ALLOWED : Current device state does not allow this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_StartDTMF()

Description:
   Starts playing a DTMF tone to all active calls until a ICALL_StopDTMF() is invoked.

Prototype:
   int ICALL_StartDTMF( ICall *pme, char bDigit );

Parameters:
   pme    : [in] : Pointer to interface.
   bDigit : buffer digit.
   
Return Value:
    SUCCESS: if successful
    
    EBADPARM: if invalid character is passed other then 0-9, * or #
    
    EBADSTATE: if previous DTMF tone playing is in progress 
    
    Appropriate AEEError code: otherwise

Comments:
   If a prior DTMF is already in play it will be stopped.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ICALL_StopDTMF()

Description: 
   Stops if any DTMF is being played on active call. This affects the DTMFs
played with ICALL_StartDTMF() or ICALL_BurstDTMF().

Prototype:
   int ICALL_StopDTMF( ICall *pme );

Parameters:
   pme : [in] : Pointer to interface.

Return Value:
   SUCCESS : The stop request is being processed.

Comments:  
   None.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   ICALL_StartDTMF()
   ICALL_BurstDTMF()

======================================================================= 

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
IMultipartyCall Interface

Description:

   This interface supplements the services provided by ICall and offers making a 
multiparty call. This must be created as QueryInterface on the ICall object referred
herein as primary ICall object. The designated primary ICall object plays the master 
for all the calls added. The Calls added into a multiparty call are referred as secondary 
calls for clarity.

List of Header files to be included:

   AEETelephone.h

======================================================================= 

IMULTIPARTYCALL_AddRef()

Description: 
   This function is inherited from IBASE_AddRef().

See Also:
   IMULTIPARTYCALL_Release()

======================================================================= 

IMULTIPARTYCALL_Release()

Description: 
   This function is inherited from IBASE_Release().

See Also:
   IMULTIPARTYCALL_AddRef()

======================================================================= 

IMULTIPARTYCALL_QueryInterface()

Description: 
   This function is inherited from IQI_QueryInterface().

See Also:
   None

======================================================================= 

IMULTIPARTYCALL_AddParty()

Description: 
   Adds a connected call into a multiparty call. If the primary call is a secondary
call with some other multiparty call, it will be removed. If the secondary call pCall
is a primary of another multiparty call it will add all its secondaries.

Prototype:
   int IMULTIPARTYCALL_AddParty( IMultipartyCall *pme, ICall *pCall );

Parameters:
   pme : [in] : Pointer to interface.
   pCall : [in] : an instance of ICall.

Return Value:
   SUCCESS : successfully added.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function.

   EALREADY: if the call is already in the multiparty.

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments:  
   In IS95 or IS2000 networks, multiparty call is not valid. These operations in such
device mode result in AEET_ERR_OP_NOT_ALLOWED.
   Any call added into a multiparty call takes the state of primary ICall object.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

IMULTIPARTYCALL_RemoveParty()

Description:
   Removes a secondary call from the multiparty call. The call removed goes to ONHOLD state.

Prototype:
   int IMULTIPARTYCALL_RemoveParty( IMultipartyCall *pme, ICall *pCall );

Parameters:
   pme : [in] : Pointer to interface.
   pCall : [in] : an instance of ICall.

Return Value:
   SUCCESS : successfully removed.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function.

   ENOSUCH: if no such call is in the multiparty.

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments:
   Removing a primary call is not allowed.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

IMULTIPARTYCALL_DisconnectParty()

Description: 
   Not only Removes a secondary call from the multiparty call but also disconnects it.

Prototype:
   int IMULTIPARTYCALL_DisconnectParty( IMultipartyCall *pme, ICall *pCall );

Parameters:
   pme : [in] : Pointer to interface.
   pCall : [in] : an instance of ICall.

Return Value:
   SUCCESS : successfully removed and disconnection in progress.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function.

   ENOSUCH: if no such call is in the multiparty.

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

Comments:  
   None.

Side Effects:
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

IMULTIPARTYCALL_GetNextCall()

Description: 
   Gets reference to the ICall object next to the given pCall. Enables iterating through
the calls in multiparty.

Prototype:
   int IMULTIPARTYCALL_GetNextCall( IMultipartyCall *pme, ICall *pCall, ICall **ppCall );

Parameters:
   pme : [in] : Pointer to interface.
   pCall : [in] : an instance of ICall.
   ppCall : [in] : next instance of ICall.

Return Value:
   SUCCESS : successfully retreived next object in Multipart Call.

   EPRIVLEVEL: if the caller does not have sufficient privilege levels 
               to invoke this function.

   AEET_ERR_OP_NOT_ALLOWED: Current device state or operating mode does not allow 
               this operation.

   EBADPARM : If ppCall is Null.

   ENOSUCH : if pCall is not in the multiparty.

Comments:  
   If the pCall is NULL gets the first secondary call. On return SUCCESS if *ppCall is NULL, no
more calls in the Multiparty.

Side Effects: 
   None.

Version:
   Introduced Brew Client 3.1 

See Also:
   None.

======================================================================= 

=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
IPhoneCtl Interface

Description:

   This interface provides methods that alter the phone behavior. These
methods if not used correctly may render the device unusable. Each method in
this interface check for the applications access privileges.

List of Header files to be included:

   AEETelephone.h
   
======================================================================= 

IPHONECTL_AddRef()

Description: 
   This function is inherited from IBASE_AddRef().

See Also:
   IPHONECTL_Release()

======================================================================= 

IPHONECTL_Release()

Description: 
   This function is inherited from IBASE_Release().

See Also:
   IPHONECTL_AddRef()

======================================================================= 

IPHONECTL_QueryInterface()

Description: 
   This function is inherited from IQI_QueryInterface().

See Also:
   None

======================================================================= 

IPHONECTL_IsSuppsCallOp()

Description: 
   Verifies the given string is good call related supplementary service request.

Prototype:
   int IPHONECTL_IsSuppsCallOp( IPhoneCtl *pme, const char *pszString, uint32 *pnum );

Parameters:
   pme : [in] : Pointer to interface
   pszString : [in] : Request string
   pnum : [out] : returns the Call X if any in the request.

Return Value:
   SUCCESS : the string is a valid request.
   EBADPARM : pszString is NULL.
   EINVALIDFORMAT : string is completely unusable.
   EINCOMPLETEITEM : string is a partial request.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SuppsCallOp()

Description: 
   This function processes call related supplementary service request. 
   A request once accepted will affect the states of one or more calls 
   in progress. The end result of this processing can be monitored using 
   a model listener on the respective calls.

   e.g : 
      A call is initiated using ICALLMGR_Originate(), then using 
      IPhoneCtl_SuppsCallOp it is put on hold with pszString "2". 
      This function returns SUCCESS, indicating that the request went 
      through. But when there is a Lower Layer error, the call is not put 
      on hold. This is indicated to the application through 
      AEET_EVENT_CALL_OPS_ERROR. But for this and other lower layer error, 
      the application needs to digest AEETCallEvent on the calls.

   Refer to 3GPP TS 02.30 V7.1.1, section 4.5.5 for acceptable call related 
   supplementary service commands.


Prototype:
   int IPHONECTL_SuppsCallOp( IPhoneCtl *pme, const char *pszString );

Parameters:
   pme : [in] : Pointer to interface
   pszString : [in] : Request string

Return Value:
   SUCCESS : the string is a valid request and operation is in progress.
   EBADPARM : pszString is NULL.
   EINVALIDFORMAT : string is completely unusable.
   EINCOMPLETEITEM : string is a partial request.
   AEET_ERR_BADDESC : call descriptor in the request is bad.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_GetSuppsTrans()

Description: 
   Creates an instance of new supplementary services transaction.

Prototype:
   int IPHONECTL_GetSuppsTrans( IPhoneCtl *pme, AEETSuppsTransId tid, ISuppsTrans **ppTrans );

Parameters:
   pme : [in] : Pointer to interface
   tid : [in] : transaction id. If this is a new mobile initiated transaction
            use AEET_SUPPSVC_TRANSID_NEW. For network initiated transactions this
            value is given in notification.
   ppTrans : [out] : on success an instance of ISuppsTrans is returned.

Return Value:
   SUCCESS : An instance is created.
   EBADPARM : ppTrans is NULL.
   ENOMEMORY : out of memory

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetSystemPreference()

Description: 
  Changes the phone's system selection preference (mode, band, and roam preference). The status 
pointer pnStatus and pcb should be valid till the callback is invoked. If asynchrounous result 
in not required, both pnStatus and pcb can be NULL.


Prototype:
   int IPHONECTL_SetSystemPreference( IPhoneCtl *pme, AEECallback *pcb, 
                                      AEETSystemPreference *pPref, uint32 dwSize,
                                      int *pnStatus );

Parameters:
   pme : [in] : Pointer to interface.
   pcb : [in|out] : Callback for asynchronous completion status.
   pPref : [in] : Preferences data.
   dwSize : [in] : size of data.
   pnStatus : [out] : status of the request. check this value after pcb is resumed.

Return Value:
   SUCCESS : request was started successfully.
   EFAILED : failed to accept the request.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetNAMSel()

Description: 
   Changes NAM selection preference.

Prototype:
   int IPHONECTL_SetNAMSel( IPhoneCtl *pme, AEETNAM nam );

Parameters:
   pme : [in] : pointer to interface
   nam : [in] : identifies the nam to activate.

Return Value:
   SUCCESS : selection has been accepted.
   AEET_ERR_OP_NOT_ALLOWED : the system mode does not allow this operation.
   AEET_ERR_OUTOF_RESOURCES : Given nam exceeds the maximum device is capable.
   AEET_ERR_PHONE_NOT_ACTIVE : Phone is likely powered off or shutting down.
   AEET_ERR_PHONE_IN_USE : Phone is in use (traffic state) on the current NAM.
   
Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetOperatingMode()

Description: 
   Changes the devices' operating mode.

Prototype:
   int IPHONECTL_SetOperatingMode( IPhoneCtl *pme, AEETOprtMode mode );

Parameters:
   pme : [in] : the interface pointer
   mode : [in] : switch to this operating mode

Return Value:
   SUCCESS : request accepted.

Comments: 
   This table illustrates acceptable operating mode transitions:
===pre>
   
        Current Operating Mode          New Operating Mode
---------------------------------------------------------------------
        AEET_OPRT_MODE_PWROFF         AEET_OPRT_MODE_OFFLINE_CDMA   
        AEET_OPRT_MODE_PWROFF         AEET_OPRT_MODE_ONLINE         
        AEET_OPRT_MODE_PWROFF         AEET_OPRT_MODE_OFFLINE_AMPS   
        AEET_OPRT_MODE_PWROFF         AEET_OPRT_MODE_OFFLINE        
        AEET_OPRT_MODE_PWROFF         AEET_OPRT_MODE_LPM            

        AEET_OPRT_MODE_OFFLINE_CDMA   AEET_OPRT_MODE_PWROFF         
        AEET_OPRT_MODE_OFFLINE_CDMA   AEET_OPRT_MODE_RESET          
        AEET_OPRT_MODE_OFFLINE_CDMA   AEET_OPRT_MODE_OFFLINE_AMPS   
        AEET_OPRT_MODE_OFFLINE_CDMA   AEET_OPRT_MODE_OFFLINE        

        AEET_OPRT_MODE_OFFLINE_AMPS   AEET_OPRT_MODE_PWROFF         
        AEET_OPRT_MODE_OFFLINE_AMPS   AEET_OPRT_MODE_RESET          
        AEET_OPRT_MODE_OFFLINE_AMPS   AEET_OPRT_MODE_OFFLINE_CDMA   
        AEET_OPRT_MODE_OFFLINE_AMPS   AEET_OPRT_MODE_OFFLINE        

        AEET_OPRT_MODE_OFFLINE        AEET_OPRT_MODE_PWROFF         
        AEET_OPRT_MODE_OFFLINE        AEET_OPRT_MODE_RESET          
        AEET_OPRT_MODE_OFFLINE        AEET_OPRT_MODE_OFFLINE_CDMA   
        AEET_OPRT_MODE_OFFLINE        AEET_OPRT_MODE_OFFLINE_AMPS   

        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_OFFLINE_CDMA   
        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_OFFLINE_AMPS   
        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_OFFLINE        
        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_LPM            
        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_PWROFF         
        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_NET_TEST_GW    
        AEET_OPRT_MODE_ONLINE         AEET_OPRT_MODE_FTM            

        AEET_OPRT_MODE_NET_TEST_GW    AEET_OPRT_MODE_PWROFF         
        AEET_OPRT_MODE_NET_TEST_GW    AEET_OPRT_MODE_ONLINE         

        AEET_OPRT_MODE_FTM            AEET_OPRT_MODE_ONLINE         
        AEET_OPRT_MODE_FTM            AEET_OPRT_MODE_PWROFF         
        AEET_OPRT_MODE_FTM            AEET_OPRT_MODE_OFFLINE_CDMA   
        AEET_OPRT_MODE_FTM            AEET_OPRT_MODE_LPM            

        AEET_OPRT_MODE_LPM            AEET_OPRT_MODE_OFFLINE_CDMA   
        AEET_OPRT_MODE_LPM            AEET_OPRT_MODE_OFFLINE_AMPS   
        AEET_OPRT_MODE_LPM            AEET_OPRT_MODE_OFFLINE        
        AEET_OPRT_MODE_LPM            AEET_OPRT_MODE_ONLINE         
        AEET_OPRT_MODE_LPM            AEET_OPRT_MODE_PWROFF   
===/pre>

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetSubscriptionStatus()

Description: 
   Requests phone to process change in subscription status. Device capable of
supporting the mode may activate the corresponding protocol stack.

Prototype:
   int IPHONECTL_SetSubscriptionStatus( IPhoneCtl *pme, AEETSysMode mode, boolean status );

Parameters:
   pme : [in] : the interface pointer.
   mode : [in] : system mode
   status : [in] : TRUE to activate the mode.

Return Value:
   SUCCESS : subscription status was accepted.
   EUNSUPPORTED : device does not support the mode.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetRSSIDeltaThreshold()

Description: 
   Changes RSSI-reporting delta threshold. The default delta threshold is 5 dBm.

Prototype:
   int IPHONECTL_SetRSSIDeltaThreshold( IPhoneCtl *pme, byte thr_delta );

Parameters:
   pme : [in] : pointer to interface.
   thr_delta : [in] : delta threshold in dBm for RSSI reporting.

Return Value:
   SUCCESS : Setting is accepted.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetPrefNetworks()

Description: 
   Saves the preferred networks to the SIM/USIM. Gives an asynchronous response when done.

Prototype:
   int IPHONECTL_SetPrefNetworks( IPhoneCtl *pme, AEECallback *pcb, AEETUserPLMNList *pPref, uint32 dwSize, int *pnStatus );

Parameters:
   pme : [in] : Pointer to interface.
   pcb : [in|out] : Callback to inform when processing is complete.
   pPref : [in] : Buffer containing networks.
   dwSize : [in] : sizeof(AEETUserPLMNList)
   pnStatus : [out] : status of processing. Check the value after pcb is resumed.

Return Value:
   SUCCESS : request is accepted.
   AEET_ERR_OP_NOT_ALLOWED : if the current system mode does not allow this operation.
   EUNSUPPORTED : if device does not support this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_WakeupFromStandby()

Description: 
  Informs the lower layers to wake up from deep sleep (standby mode).

Prototype:
   int IPHONECTL_WakeupFromStandby( IPhoneCtl *pme );

Parameters:
   pme : [in] : Pointer to interface

Return Value:
   SUCCESS : request is accepted.
   AEET_ERR_PHONE_NOT_ONLINE : if phone is not online.
   EALREADY : if phone is not in deep sleep.
   AEET_ERR_OP_NOT_ALLOWED : if this request is not valid in the current device mode.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetRTREConfig()

Description: 
   Changes the RTRE(Runtime RUIM Enable) configuration to the specified value.

Prototype:
   int IPHONECTL_SetRTREConfig( IPhoneCtl *pme, AEETRTREConfig config );

Parameters:
   pme : [in] : pointer to interface.
   config : [in] : requested configuration.

Return Value:
   SUCCESS : Request is accepted.
   EUNSUPPORTED : Device does not support the given configuration.
   AEET_ERR_OP_NOT_ALLOWED : Operation is not allowed in the current system mode.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetAnswerVoicePref()

Description: 
   Typically phone answers incoming calls as voice call. This allows changing the answer preference setting.

Prototype:
   int IPHONECTL_SetAnswerVoicePref( IPhoneCtl *pme, AEETAnsVoicePref pref, uint32 dwDuration );

Parameters:
   pme : [in] : interface pointer
   pref : [in] : indicate whether the phone should answer incoming calls as
          voice, fax, or data calls.
   dwDuration : [in] : span of this preference in seconds.

Return Value:
   SUCCESS : request is accepted.
   EBADPARM : Invalid Parameter
   EUNSUPPORTED : device does not support this function.
   AEET_ERR_OP_NOT_ALLOWED : current system mode does not allow this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_AvoidServingSystem()

Description: 
   Avoid the current serving system for a specified duration under given conditions.

Prototype:
   int IPHONECTL_AvoidServingSystem( IPhoneCtl *pme, AEETAvoidSSPref pref, uint32 dwDuration );

Parameters:
   pme : [in] : pointer to interface
   pref : [in] : conditions.
   dwDuration : [in] : span for the preference in seconds.

Return Value:
   SUCCESS : request accepted.
   EUNSUPPORTED : device does not support this operation.
   AEET_ERR_OP_NOT_ALLOWED : current system mode does not allow this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetVoicePrivacy()

Description: 
   Changes the privacy preference on voice calls.

Prototype:
   int IPHONECTL_SetVoicePrivacy( IPhoneCtl *pme, AEETPrivacyPref pref );

Parameters:
   pme : [in] : interface pointer
   pref : [in] : new privacy preference.

Return Value:
   SUCCESS : request accepted
   EUNSUPPORTED : device does not support this function.
   AEET_ERR_OP_NOT_ALLOWED : current system mode does not allow this operation.

Comments:  
   If the device is not in voice call, this will be enforced on the next voice call.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None:

======================================================================= 

IPHONECTL_ResetACM()

Description: 
   Reset the Accumulate Call Meter to 0 in a system supporting Advice of Charge.

Prototype:
   int IPHONECTL_ResetACM( IPhoneCtl *pme );

Parameters:
   pme : [in] : Pointer to interface

Return Value:
   SUCCESS : request accepted.
   AEET_ERR_OP_NOT_ALLOWED : current system mode does not allow this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None
======================================================================= 

IPHONECTL_SetACMMax()

Description: 
   Set the maximum value for Accumulate call meter.

Prototype:
   int IPHONECTL_SetACMMax( IPhoneCtl *pme, uint32 dwNewMax );

Parameters:
   pme : [in] : interface pointer
   dwNewMax : [in] : new value for Maximum allowed Accumulated Call Meter

Return Value:
   SUCCESS : request accepted.
   AEET_ERR_OP_NOT_ALLOWED : current system mode does not allow this operation.   

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

IPHONECTL_SetDDTMPref()

Description: 
  Change the phone's data dedicated transmission mode (DDTM) preference to a 
specified selection, such as ON or OFF etc.

Prototype:
   int IPHONECTL_SetDDTMPref( IPhoneCtl *pme, AEETDDTMPref pref );

Parameters:
   pme : [in] : interface pointer
   pref : [in] : dedicated data transmission mode

Return Value:
   SUCCESS : request accepted.
   EUNSUPPORTED : device does not support this functionality

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

=======================================================================

IPHONECTL_SelectLine()

Description: 
  Select a line if the device supports alternate lines.

Prototype:
   int IPHONECTL_SelectLine( IPhoneCtl *pme, uint32 lineid );

Parameters:
   pme : [in] : interface pointer
   lineid : [in] : line to use.

Return Value:
   SUCCESS : request accepted.
   EUNSUPPORTED : device does not support this functionality
   AEET_ERR_OP_NOT_ALLOWED : line switching is disabled.
   AEET_ERR_BADDESC : line id is out of supported range.

Comments: 
   The change is effective after the phone info is updated with new line identifier.
   Once effective outgoing calls will use the selected line.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1.2

See Also:
   None

=======================================================================

IPHONECTL_EnableLineSwitch()

Description: 
  Enable or disable line switching. When disabled alternate lines if available cannot be selected.

Prototype:
   int IPHONECTL_EnableLineSwitch( IPhoneCtl *pme, boolean bEnable );

Parameters:
   pme : [in] : interface pointer
   bEnable : [in] : TRUE enables, FALSE disables line switching.

Return Value:
   SUCCESS : request accepted.
   EUNSUPPORTED : device does not support this functionality
   AEET_ERR_OP_NOT_ALLOWED : Curret device operation mode does not allow this operation.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1.2

See Also:
   None

=======================================================================

IPHONECTL_RefreshAvailableNetworks()

Description: 
  Perform a scan for available networks.

Prototype:
   int IPHONECTL_RefreshAvailableNetworks(IPhoneCtl *pme,  AEECallback *pcb, int *pnStatus)

Parameters:
   pme : [in] : interface pointer
   pcb : [in|out] : a AEECallback data type to asynchronously inform the status
   pnStatus : [out] : output the status of scan operation. 0 indicates scan is successfully
         complete. Otherwise an error of AEEError.h or AEET_ERR_* error code.

Return Value:
   SUCCESS : request accepted.
   EUNSUPPORTED : device does not support this functionality
   AEET_ERR_OP_NOT_ALLOWED : Curret device operation mode does not allow this operation.

Comments: 
   When asynchronous operation is complete successfully (*pnStatus == 0) the member
   AEETPhInfo::available_networks represents the new list. 

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1.5

See Also:
   AEET_EVENT_PH_AVIALBLE_NETWORKS, ITELEPHONE_GetPhoneInfo()

=======================================================================
=======================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 
ISuppsTrans Interface

Description:

   This interface provides services to send non-call related supplementary
services. A new instance can be obtained from IPHONECTL_GetSuppsTrans(). 

A transaction Id AEET_SUPPSVC_TRANSID_NEW given to IPHONECTL_GetSuppsTrans()
will create a new object for use of mobile initiated transactions. 

Network initiated transactions can be known using any of these two methods :
1. Specify interest for AEET_NMASK_NEW_NI_SUPP_TRANS to AEECLSID_PHONENOTIFIER
object.
2. Add listener to AEECLSID_TELEPHONE using IModel interface.

In both methods application will be given a network initiated transaction Id.
This transaction Id can be used with IPHONECTL_GetSuppsTrans() to create the
SuppsTrans Object. Life of the transaction Id is limited to the event 
notification call (for both EVT_NOTIFY and Listener callback).


Sending a mobile initiated request comprises of following steps :

 1. Create a SuppsTrans object.

   nErr = IPHONECTL_GetSuppsTrans(pme->pPhoneCtl, AEET_SUPPSVC_TRANSID_NEW, 
                                  &piSupps);

 2. Use it to parse a string in to a Message buffer of type AEETSuppsMessage. Optionally 
 you could use the ISUPPSTRANS_ParseString() to determine how large a buffer is needed.

   {
      AEETSuppsMessage* pMsg = 0;
      uint32 uSize = 0;

      nErr = ISUPPSTRANS_ParseString(piSupps, "*#21*11#", pMsg, &uSize);
      if (EBUFFERTOOSMALL != nErr) {
         goto bail;
      }

      // Allocate message buffer
      nErr = ERR_MALLOC(uSize, &pMsg);
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }

      // parse and intialize the buffer
      nErr = ISUPPSTRANS_ParseString(piSupps, psz, pMsg, &uSize);
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }
   }

 3. Load the message to transaction object.
   nErr = ISUPPSTRANS_LoadMessage(piSupps, pMsg, uSize);

 Load message copies the message in to the object. You can release the memory 
 associated with pMsg after the ISUPPSTRANS_LoadMessage().

 4. Run the transaction with a callback notification when done.

      // Prepare the asynchronous callback
      CALLBACK_Cancel(&me->cbOneShot);
      CALLBACK_Init(&me->cbOneShot, (PFNNOTIFY)SendCB, me);

      // Run the transaction
      ISUPPSTRANS_Run(piSupps, &me->cbOneShot);

Finding the current state of transaction object :

 ISUPPSTRANS_GetState() returns AEETSuppsTransState. Object is in AEETSUPPS_STATE_NONE 
 until a first ISUPPSTRANS_Run() is invoked. After ISUPPSTRANS_Run() is invoked object 
 enters AEETSUPPS_STATE_RUNNING state and when done will be in either 
 AEETSUPPS_STATE_NW_INDICATION or AEETSUPPS_STATE_DONE. Result of the transaction can 
 be retrieved using ISUPPSTRANS_GetResult(). The result is not valid when object is in 
 states AEETSUPPS_STATE_NONE or AEETSUPPS_STATE_RUNNING.

 ISUPPSTRANS_Run() can be issued any time, even when the object is in the 
 AEET_SUPPS_STATE_RUNNING. Callbacks will fire when transaction is halted.

Responding to a Network initiated transaction comprises of following steps 

 1. Retrieve the transaction Id in asynchronous notifications data AEETSuppsEventData, 
 and create the SuppsTrans object with that transaction Id.

   nErr = IPHONECTL_GetSuppsTrans(pme->pPhoneCtl, tid, &piSupps);

 2. Retrieve the network indication.
   {
      AEETSuppsMessage theMsg;
      int nErr;

      // compose a network indication request.
      theMsg.type = AEETSUPPS_MSG_REQUEST;
      theMsg.data.request.operation = AEETSUPPS_REQ_NETWORK_IND;

      nErr = ISUPPSTRANS_LoadMessage(piSupps, &theMsg, sizeof(theMsg));
      if (AEE_SUCCESS != nErr) {
         goto bail;
      }

      CALLBACK_Init(&me->cbNi, GetPasswdIndCb, me);
      ISUPPSTRANS_Run(piSupps, &me->cbNi);
   }

 3. After the network indication is retrieved compose and send the response.

   static void GetPasswdIndCb(void* po)
   {
      AEETSuppsMessage theMsg;
      ...

      theMsg.type = AEETSUPPS_MSG_RESPONSE;
      theMsg.data.response.response_type = AEETSUPPS_RES_PASSWD;

      // szPasswd is string buffer containing password.
      theMsg.data.response.response_data.length = STRLEN(szPasswd);
      theMsg.data.response.response_data.data   = szPasswd;      
      ...

      // Load the response message to object
      nErr = ISUPPSTRANS_LoadMessage(piSupps, &theMsg, sizeof(theMsg));

      // prepare a callback
      ...

      // run the transaction.
      ISUPPSTRANS_Run( ... );
   }

List of Header files to be included:

   AEETelephone.h
   
======================================================================= 

ISUPPSTRANS_AddRef()

Description: 
   This function is inherited from IBASE_AddRef().

See Also:
   ISUPPSTRANS_Release()

======================================================================= 

ISUPPSTRANS_Release()

Description: 
   This function is inherited from IBASE_Release().

See Also:
   ISUPPSTRANS_AddRef()

======================================================================= 

ISUPPSTRANS_QueryInterface()

Description: 
   This function is inherited from IQI_QueryInterface().

See Also:
   None

======================================================================= 

ISUPPSTRANS_ParseString()

Description: 
   A helper routine to verify the validity of a string request. 

Prototype:
   int ISUPPSTRANS_ParseString( ISuppsTrans *pme, const char *pszString, AEETSuppsMessage *pMsg, uint32* pdwSize );

Parameters:

   pme : [in] : the interface pointer.
   pszString : [in] : null terminated string.
   pMsg : [out] : A buffer to place the parsed message request. Can be NULL.
   pdwSize : [in|out] : sizeof buffer at pMsg, on return memory needed if pMsg is NULL. Can be NULL.

Return Value:
   SUCCESS : request is parsed successfully for errors.
   EBADPARM : pszString is NULL.
   EBUFFERTOOSMALL : buffer at pMsg is insufficient.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ISUPPSTRANS_GetState()

Description: 
   Obtain the state of transaction.

Prototype:
   AEETSuppsTransState ISUPPSTRANS_GetState( ISuppsTrans *pme );

Parameters:
   pme : [in] : the interface pointer.

Return Value:
   AEETSuppsTransState

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ISUPPSTRANS_LoadMessage()

Description: 
   Load the given message for the transaction.

Prototype:
   int (*ISUPPSTRANS_LoadMessage)( ISuppsTrans *pme, const AEETSuppsMessage *pMsg, uint32 dwSize );

Parameters:
   pme : [in] : the interface pointer.
   pMsg : [in] : message to send.
   dwSize : [in] : size of message buffer.

Return Value:
   SUCCESS : request is accepted for processing.
   EBADPARM : pMsg is NULL or not a recognized size.
   EPRIVLEVEL : application does not have sufficient privileges to load this message.

Comments: 
   To get device IMEI application must have AEECLSID_GSMCREDENTIALS.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ISUPPSTRANS_Run()

Description: 
   Run the transaction to process the loaded message.

Prototype:
   void ISUPPSTRANS_Run( ISuppsTrans *pme, AEECallback *pcbWait );

Parameters:
   pme : [in] : the interface pointer.
   pcbWait : [in|out] : Callback to resume when transaction halts. Can be NULL.

Return Value:
   None.
   
Comments: 
   None.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 

ISUPPSTRANS_GetResult()

Description: 
   Get the result of processed transaction. On success this provides the internal
buffers with the result. These buffers are invalidated on the next run or when interface is
released.

Prototype:
   int ISUPPSTRANS_GetResult( ISuppsTrans *pme, const AEETSuppsStatus **ppResult );

Parameters:
   pme : [in] : the interface pointer.
   ppResult : [in|out] : address of internal buffers is returned.

Return Value:
   SUCCESS : when the result buffer is available and address written to *ppResult.
   EBADPARM : when ppResult is NULL.
   EBADSTATE : when transaction is running and no result available.
   
Comments: 
   None.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

======================================================================= 
*/
#endif /* AEETELEPHONE_H */

