/*======================================================
FILE:  AEETSupps.c

SERVICES:  Implementation of ISuppsTrans

GENERAL DESCRIPTION:
   Implements ISuppsTrans

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEETelephone.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"

#include "OEMTSupps.h"

/*======================================================================= 

   Defines

=======================================================================*/
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

boolean AEESUPPS_IsDebugMsgOn(void);
#define FARF_SUPPS               AEESUPPS_IsDebugMsgOn()

#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }

struct ISuppsTrans {
   const ISuppsTransVtbl   *pvt;

   IShell                  *pIShell;
   uint32                  dwRefs;
   AEECallback             cbAppClose;
   ACONTEXT                *pac;

   OEMSuppsTrans           theTrans;
   OEMTSupps*              pOEMTSupps;

   AEETSuppsMessage        theMessage;

   /* Application callbacks to resume when halted. */
   AEECallback             *pcbList;

   /* Supps processor to resume when halted. */
   AEECallback             cbProc;
};

static uint32 ISuppsTrans_AddRef( ISuppsTrans *po );
static uint32 ISuppsTrans_Release( ISuppsTrans *po );
static int ISuppsTrans_QueryInterface( ISuppsTrans *po, AEECLSID clsid, void **ppo );
static int ISuppsTrans_ParseString( ISuppsTrans *po, const char *pszString, AEETSuppsMessage *pMsg, uint32 *pdwSize );
static AEETSuppsTransState ISuppsTrans_GetState( ISuppsTrans *po );
int ISuppsTrans_LoadMessage( ISuppsTrans *po, const AEETSuppsMessage *pMsg, uint32 dwSize );
void ISuppsTrans_Run( ISuppsTrans *po, AEECallback *pcbWait );
static int ISuppsTrans_GetResult( ISuppsTrans *po, const AEETSuppsStatus **ppResult );

/*======================================================================= 
=========================================================================
SUBSECTION  Supps private services.
=========================================================================
=======================================================================*/
int ISuppsTrans_New( IShell *pIShell, AEECLSID clsid, void **ppf );

/*======================================================================= 
=========================================================================
SUBSECTION  ISuppsTrans private services.
=========================================================================
=======================================================================*/

/*======================================================================= 
=======================================================================*/
static void ISuppsTrans_Cancel( AEECallback *pcb )
{
   ISuppsTrans *pme = pcb->pCancelData;
   AEECallback **ppcb = &pme->pcbList;

   while( *ppcb ) {     /* Find the callback in list */

      if( *ppcb == pcb ) {
         break;
      }
      *ppcb = (*ppcb)->pNext;
   }

   if( *ppcb ) {        /* Callback found ? Remove it from list */

      *ppcb = (*ppcb)->pNext;
   }

   if( !pme->pcbList ) {         /* List empty ? Stop processing the message */

      CALLBACK_Cancel( &pme->cbProc );
   }
   ISuppsTrans_Release( pme );
}

/*======================================================================= 
=======================================================================*/
static void ISuppsTrans_Proc(ISuppsTrans *pme)
{
   AEECallback *pcb;

   while( pme->pcbList ) {     /* Find the callback in list */

      pcb = pme->pcbList;
      pme->pcbList = pcb->pNext;
      AEE_ResumeCallback(pcb, pme->pac);
   }
}

static const AEETSuppsCodeType supported_codes[] = {
   AEETSUPPS_CODE_CLIP,
   AEETSUPPS_CODE_CLIR,
   AEETSUPPS_CODE_COLP,
   AEETSUPPS_CODE_COLR,
   AEETSUPPS_CODE_ALL_FORWARDING_SS,
   AEETSUPPS_CODE_CFU,
   AEETSUPPS_CODE_ALL_CONDFWD_SS,
   AEETSUPPS_CODE_CFB,
   AEETSUPPS_CODE_CFNRY,
   AEETSUPPS_CODE_CFNRC,
   AEETSUPPS_CODE_CW,
   AEETSUPPS_CODE_ALL_CALL_RSTRCT_SS,
   AEETSUPPS_CODE_BARRING_OUTGOING_CALLS,
   AEETSUPPS_CODE_BAOC,
   AEETSUPPS_CODE_BOIC,
   AEETSUPPS_CODE_BOIC_EXHC,
   AEETSUPPS_CODE_BARRING_OF_INCOMING_CALLS,
   AEETSUPPS_CODE_BAIC,
   AEETSUPPS_CODE_BICROAM
};

/*======================================================================= 
=======================================================================*/
static __inline int Supps_ValidateRequest(const AEETSuppsRequest *pReq, 
                                          uint32 dwSize )
{
   if (AEETSUPPS_REQ_UNBLOCK_PIN2 < pReq->operation) {
      return AEET_ERR_SUPPS_MSG_INVALID_TYPE;
   }

   switch( pReq->operation ) {
   case AEETSUPPS_REQ_CHANGE_PIN:
   case AEETSUPPS_REQ_UNBLOCK_PIN:
      if( STRNCMP( pReq->req.passwd.szNewPasswd, 
                   pReq->req.passwd.szNewPasswdRepeated, AEETSUPPS_MAX_PASSWD_LEN ) ){
         return AEET_ERR_SUPPS_MSG_INVALID_TYPE;
      }
      break;
   }

   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
static __inline int Supps_ValidateResponse(const AEETSuppsResponse *pRes, 
                                           uint32 dwSize)
{
   if( pRes->response_type > AEETSUPPS_RES_USS_NONE ) {
      return AEET_ERR_SUPPS_MSG_INVALID_TYPE;
   }

   return SUCCESS;
}

/*======================================================================= 
=======================================================================*/
static __inline int Supps_ValidateMessage(const AEETSuppsMessage *pMsg, 
                                          uint32 dwSize)
{
   int nErr = SUCCESS;

   switch( pMsg->type ){
   case AEETSUPPS_MSG_REQUEST :
      nErr = Supps_ValidateRequest(&pMsg->data.request, dwSize);
      break;
   case AEETSUPPS_MSG_RESPONSE :
      nErr = Supps_ValidateResponse(&pMsg->data.response, dwSize);
      break;
   default:
      nErr = AEET_ERR_SUPPS_MSG_INVALID_TYPE;
   }

   return nErr;
}

/*======================================================================= 
=======================================================================*/
static void ISuppsTrans_UnloadMessage( ISuppsTrans *pme )
{
   if( pme->theMessage.type == AEETSUPPS_MSG_REQUEST ) {
      if( AEETSUPPS_REQ_REGISTER == pme->theMessage.data.request.operation ) {
         FREEIF( pme->theMessage.data.request.req.reg.data.data );
      }
      else if ( AEETSUPPS_REQ_USS == pme->theMessage.data.request.operation ) {
         FREEIF( pme->theMessage.data.request.req.ussd.data );
      }
   }
   else if( pme->theMessage.type == AEETSUPPS_MSG_RESPONSE ) {
      FREEIF( pme->theMessage.data.response.response_data.data );
   }
}

/*======================================================================= 
=======================================================================*/
static void ISuppsTrans_Dtor( ISuppsTrans *pme )
{
   CALLBACK_Cancel( &pme->cbAppClose );
   CALLBACK_Cancel(&pme->cbProc);
   if (0 != pme->pOEMTSupps) {
      OEMTSupps_Delete(pme->pOEMTSupps);
      pme->pOEMTSupps = 0;
   }
   RELEASEIF( pme->pIShell );
}

/*======================================================================= 
=======================================================================*/
static void ISuppsTrans_Delete( ISuppsTrans *pme )
{
   ISuppsTrans_Dtor(pme);
   ISuppsTrans_UnloadMessage(pme);
   FREE(pme);
}

/*======================================================================= 
=======================================================================*/
int ISuppsTrans_New( IShell *pIShell, AEETSuppsTransId tid, void **ppf )
{
   static const ISuppsTransVtbl      vt_SuppsTrans   = {
      ISuppsTrans_AddRef,
      ISuppsTrans_Release,
      ISuppsTrans_QueryInterface,
      ISuppsTrans_ParseString,
      ISuppsTrans_GetState,
      ISuppsTrans_LoadMessage,
      ISuppsTrans_Run,
      ISuppsTrans_GetResult
   };

   ISuppsTrans *pme;
   int nErr;

   if( (void **)0 == ppf ) {
      return EBADPARM;
   }

   *ppf = (void *)0;

   pme = MALLOCREC(ISuppsTrans);
   if( 0 == pme ) {
      return ENOMEMORY;
   }

   nErr = OEMTSupps_New(tid, &pme->theTrans, &pme->pOEMTSupps);
   if (AEE_SUCCESS != nErr) {
      goto bail;
   }

      pme->pvt = &vt_SuppsTrans;

      pme->pIShell = pIShell;
      ISHELL_AddRef( pIShell );

      CALLBACK_Init( &pme->cbAppClose, ISuppsTrans_Dtor, pme );
      AEE_LinkSysObject( &pme->cbAppClose );

      pme->pac = AEE_GetAppContext();

      ISuppsTrans_AddRef( pme );

      *ppf = (void *)pme;

   return AEE_SUCCESS;

 bail:
   FREE(pme);
   *ppf = NULL;

   return nErr;
}

/*======================================================================= 
=======================================================================*/
static AEETSuppsData AEETSuppsData_Dup( const AEETSuppsData *pD )
{
   AEETSuppsData ret = { 0, (char *)0 };

   if( pD->length ) {
      ret.data = MALLOC(pD->length);
      if( ret.data ) {
         MEMCPY( ret.data, pD->data, pD->length );
         ret.length = pD->length;
      }
   }

   return ret;
}

/*======================================================================= 

ISuppsTrans_AddRef()

Description: 
   This function is inherited from IQI_AddRef().

Version:
   None

See Also:
   BREW SDK(TM) AEE Hierarchy Chart.

=======================================================================*/
static uint32 ISuppsTrans_AddRef( ISuppsTrans *pme )
{
   return ++pme->dwRefs;
}

/*======================================================================= 

ISuppsTrans_Release()

Description: 
   This function is inherited from IQI_Release().

Version:
   None

See Also:
   BREW SDK(TM) AEE Hierarchy Chart.

=======================================================================*/
static uint32 ISuppsTrans_Release( ISuppsTrans *pme )
{
   uint32   dwRefs = --pme->dwRefs;

   if( 0 == dwRefs ) {
      ISuppsTrans_Delete( pme );
   }

   return dwRefs;
}

/*======================================================================= 

ISuppsTrans_QueryInterface()

Description: 
   This function is inherited from IQI_QueryInterface().

Version:
   None

See Also:
   BREW SDK(TM) AEE Hierarchy Chart.

=======================================================================*/
static int ISuppsTrans_QueryInterface( ISuppsTrans *pme, AEECLSID clsid, void **ppo )
{

   if( (void **)0 == ppo ) {

      return EBADPARM;
   }

   if( !AEE_CHECK_PTR("ISuppsTrans_QueryInterface", ppo, sizeof(void*), TRUE) ) {
      
      return EMEMPTR;
   }

   if( clsid == AEECLSID_QUERYINTERFACE ) {

      *ppo = (void *)pme;
      ISuppsTrans_AddRef( *ppo );
      return SUCCESS;
   }
      
   return ECLASSNOTSUPPORT;
}

/*======================================================================= 

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
   EBADPARM : pszString or pdwSize is NULL or length of pszString is 0.
   EBUFFERTOOSMALL : buffer at pMsg is insufficient. *pdwSize will contain the needed size.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

=======================================================================*/
static int ISuppsTrans_ParseString( ISuppsTrans *pme, const char *pszString, AEETSuppsMessage *pMsg, 
                                    uint32 *pdwSize )
{
   AEETSuppsMessage  *pm;
   uint32            dwSize;
   int               nErr, nLen;

   if( NULL == pszString || NULL == pdwSize || 0 == (nLen = STRLEN(pszString)) ) {
      return EBADPARM;
   }

   if( pMsg && *pdwSize < sizeof(AEETSuppsMessage) ) {
      *pdwSize = sizeof(AEETSuppsMessage);
      return EBUFFERTOOSMALL;
   }

   if( nLen > AEET_MAX_DIGITS_LENGTH ) {
      return EINVALIDFORMAT;
   }

   if( !pMsg ) {
      pm = MALLOCREC( AEETSuppsMessage );
      if(!pm) return ENOMEMORY;
      dwSize = sizeof( AEETSuppsMessage );
   }
   else {
      pm = pMsg;
      dwSize = *pdwSize;
   }

   nErr = OEMSupps_ParseString(pszString, pm, &dwSize);

   if( !pMsg ) {                                          /* If we allocate here - cleanup */
      nErr = EBUFFERTOOSMALL;
      FREE(pm);                                  
   }

   *pdwSize = dwSize;

   return nErr;
}

/*======================================================================= 

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

=======================================================================*/
static AEETSuppsTransState ISuppsTrans_GetState( ISuppsTrans *pme )
{

   return pme->theTrans.m_transState;
}

/*======================================================================= 

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
   EBADSTATE : Object state does not allow loading a message.
   EUNABLETOLOAD : Failed to load the message as it is not the right type.
   EPRIVLEVEL : application does not have sufficient privileges to load this message.

Comments: 
   To get device IMEI application must have AEECLSID_GSMCREDENTIALS.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None

=======================================================================*/
int ISuppsTrans_LoadMessage( ISuppsTrans *pme, const AEETSuppsMessage *pMsg, uint32 dwSize )
{
   int nErr = SUCCESS;

   if( !pMsg || dwSize < sizeof(AEETSuppsMessage) ) {
      return EBADPARM;
   }

   if( AEETSUPPS_STATE_RUNNING == pme->theTrans.m_transState ) {
      return EBADSTATE;
   }

   nErr = Supps_ValidateMessage(pMsg, dwSize );
   if( SUCCESS != nErr ) {
      return nErr;
   }

   if( AEETSUPPS_STATE_DONE == pme->theTrans.m_transState ) {
      ISuppsTrans_UnloadMessage( pme );         /* Delete any earlier message */
      pme->theTrans.m_transState = AEETSUPPS_STATE_NONE;
   }

   if( AEETSUPPS_STATE_NONE == pme->theTrans.m_transState ) {

      if( pMsg->type != AEETSUPPS_MSG_REQUEST ) {
         return EUNABLETOLOAD;
      }

      MEMCPY( &pme->theMessage, pMsg, sizeof(AEETSuppsMessage) );
      if( AEETSUPPS_REQ_REGISTER == pme->theMessage.data.request.operation ) {
         pme->theMessage.data.request.req.reg.data 
            = AEETSuppsData_Dup(&pMsg->data.request.req.reg.data);
      }
      else if( AEETSUPPS_REQ_USS == pme->theMessage.data.request.operation ) {
         pme->theMessage.data.request.req.ussd 
            = AEETSuppsData_Dup( &pMsg->data.request.req.ussd );
      }
   }
   else if( AEETSUPPS_STATE_NW_INDICATION == pme->theTrans.m_transState ) {

      if( pMsg->type != AEETSUPPS_MSG_RESPONSE ) {
         return EUNABLETOLOAD;
      }

      MEMCPY( &pme->theMessage, pMsg, sizeof(AEETSuppsMessage) );
      pme->theMessage.data.response.response_data 
         = AEETSuppsData_Dup(&pMsg->data.response.response_data);
   }
   else {
      nErr = EBADSTATE;
   }

   return nErr;
}

/*======================================================================= 

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

=======================================================================*/
void ISuppsTrans_Run(ISuppsTrans *pme, AEECallback *pcbWait)
{

   if( AEETSUPPS_STATE_DONE == pme->theTrans.m_transState ) {
      AEE_ResumeCallback( pcbWait, pme->pac );
      return;
   }

   if ((AEECallback *)0 != pcbWait) {

      /* Install the canceller and take ownership of the callback */
      pcbWait->pfnCancel = ISuppsTrans_Cancel;
      pcbWait->pCancelData = pme;
      ISuppsTrans_AddRef( pme );

      /* Add it to the list */
      pcbWait->pNext = pme->pcbList;
      pme->pcbList = pcbWait;
   }

   if( AEETSUPPS_STATE_NONE == pme->theTrans.m_transState
       || AEETSUPPS_STATE_NW_INDICATION == pme->theTrans.m_transState ) {

      CALLBACK_Init(&pme->cbProc, ISuppsTrans_Proc, pme);

      if (SUCCESS != OEMTSupps_SendMessage(pme->pOEMTSupps,
                                           &pme->theMessage, 
                                           &pme->cbProc)){

         AEE_ResumeCallback(&pme->cbProc, pme->pac);
      }
   }
}

/*======================================================================= 

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

=======================================================================*/
static int ISuppsTrans_GetResult( ISuppsTrans *pme, const AEETSuppsStatus **ppResult )
{

   if (AEETSUPPS_STATE_NONE == pme->theTrans.m_transState
      || AEETSUPPS_STATE_RUNNING == pme->theTrans.m_transState) {
      return EBADSTATE;
   }
   *ppResult = &pme->theTrans.m_result;
   return SUCCESS;
}

static boolean gbDebugMsg = FALSE;

boolean AEESUPPS_IsDebugMsgOn()
{
   return gbDebugMsg;
}

void AEESUPPS_EnableDebugMsg(boolean bON)
{
   gbDebugMsg = bON;
}

