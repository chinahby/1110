/*==============================================================================
File:
   OEMFMRDS.c

Services:
   FM Radio RDS related Services

General Description:
   Base level definitions, typedefs, etc. for AEEFMRDS

Public Classes And Static Functions:
   OEMFMRDS

Initialization and Sequencing Requirements:

Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

==============================================================================*/

/*=============================================================================

  $Header: //depot/asic/qsc1100/platform/brew/brewpk/pk/src/msm/OEMFMRDS.c#3 $
  $DateTime: 2008/11/25 04:06:35 $
  $Author: bhargavg $
  $Change: 792258 $
                      EDIT HISTORY FOR FILE

when         who     what, where, why
----------   ---     ----------------------------------------------------------

=============================================================================*/
#include "customer.h"
#include "AEEIFMRDS.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEFMRDS.bid"

#ifdef FEATURE_FM
#include "fmrds_api.h"
#include "AEE_OEM.h"

#include "OEMFMRDS_priv.h"
#include "OEMFMRadio_priv.h"

/*===========================================================================

                    PRIVATE DATA

===========================================================================*/



/*===========================================================================

                    DATA STRUCTURES

===========================================================================*/

typedef struct{

   uint32         m_nSeqNum;
   uint32         m_nBytesAllocated;
   AEEFMRDSGroup *m_pRDSData;
   
}OEMRDSData;

typedef struct {

  const AEEVTBL(IFMRDS) *pvt;

  //stores the number of references to this object
  uint32 m_nRefs;

  // IShell pointer.
  IShell *ps;

  FMRadio_Callback  m_cbFMRadio;

  //list of all CB's for Raw Grouped data
  AEECallback  *m_pcbGroupedData[FMRDS_LAST_GROUPID];

  //list of all CB's for Parsed data
  AEECallback  *m_pcbParsedData[FMRDS_LAST_GROUPID];

  //list of all CB's for Raw data
  AEECallback  *m_pcbRawData;

  //grouped data blocks are stored here
  OEMRDSData   m_RDSGroupedData[FMRDS_LAST_GROUPID];

  //unparsed raw data blocks get allocated into this
  OEMRDSData   m_RawDataBlocks;


}OEMFMRDS;


/*===========================================================================

                    LOCAL/STATIC Functions

===========================================================================*/

static uint32 OEMFMRDS_AddRef(IFMRDS *pIFMRDS);
static uint32 OEMFMRDS_Release(IFMRDS *pIFMRDS);
static int OEMFMRDS_QueryInterface(IFMRDS *pIFMRDS,AEECLSID id, void **ppo);

static int OEMFMRDS_GetInfoData(IFMRDS        *pIFMRDS,
                                AEEFMRDSInfo  nId,
                                int           *pnSequence,
                                void          *pBuf,
                                uint32        nBufSize,
                                uint32        *pnBufSizeReq);

static int OEMFMRDS_GetNextData(IFMRDS          *pIFMRDS,
                                int             *pnSequence,
                                AEEFMRDSGroup   *pFMRDSGroupsBuf,
                                int             nFMRDSGroupsBufSize,
                                int             *pnFMRDSBufSizeReq);

static int OEMFMRDS_GetParsedData(IFMRDS              *pIFMRDS,
                                  AEEFMRDSInfo        nId,
                                  AEEFMRDSParsedInfo  *pRDSInfo);

static void OEMFMRDS_InfoReadable(IFMRDS              *pIFMRDS,
                                  AEECallback         *pcb,
                                  AEEFMRDSInfo        nId,
                                  AEEFMRDSDataStream  nType);


//call back to clean up OEMFMRDS upon BREW exit.
static void OEMFMRDS_FreeData(OEMFMRDS *pOEMFMRDS);
static void OEM_FMRDS_CallBackCancel(AEECallback *pcb);
static void OEMFMRDS_Notify(int event,uint32 nGroupId);
static int OEMFMRDS_ProcessRDSData(fmrds_event_msg_type  fmrds_event,
                                   OEMFMRDS              *pMe);
static int OEMFMRDS_ParseGroupedData(OEMFMRDS *pMe);
#ifdef FMRDS_ON_ULC
int OEMFMRDS_GroupedRawData_Fill_ULC(IFMRDS *pIFMRDS,
                                     void   *pBuf,
                                     uint32 nBufSize,
                                     AEEFMRDSInfo nId);
#endif /* FMRDS_ON_ULC */
static void OEMFMRDS_FMRadioEventsCb(void *p, fmrds_event_msg_type *pFMRDS_Event);

static void OEMFMRDS_ClearRDSData(OEMFMRDS *pMe);
/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/
static const VTBL(IFMRDS) gIFMRDS =
{
  OEMFMRDS_AddRef,
  OEMFMRDS_Release,
  OEMFMRDS_QueryInterface,
  OEMFMRDS_GetInfoData,
  OEMFMRDS_GetNextData,
  OEMFMRDS_GetParsedData,
  OEMFMRDS_InfoReadable
};

/* whether to enable debug messages or not. */
static boolean fmrds_enable_dbgs = TRUE;

/* The FMRDS Object is singleton */
static OEMFMRDS sOEMFMRDS = {0,};
static OEMFMRDS *sOEMFMRDSPtr = NULL;
//static OEMFMRDS *FMEventstr = NULL;



/*=============================================================================
Function:
   OEMFMRDS_New

Description:
   To create new interface of IFMRDS.

Parameters:
  *ps     : pointer to IShell object
  cls     : class ID of IFMRDS
  **ppobj : returns the pointer to the newly created object.

RETURN VALUE:

 AEE_SUCCESS          : If it was able to initialise the new object.
 AEE_ECLASSNOTSUPPORT : If its not a valid class ID.
 AEE_ENOMEMORY        : If the Interface was unable to allocate memory.
 AEE_EFAILED          : Otherwise.

Comments:

Side Effects:

See Also:

=============================================================================*/
int OEMFMRDS_New(IShell *ps, AEECLSID cls, void **ppObj)
{
   int nResult = EFAILED;
   efmrds_event_type  aefmrds_event_type[1] = {FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA};

   *ppObj = NULL;

   if ( cls != AEECLSID_FMRDS ) {
      return ECLASSNOTSUPPORT;
   }

   do {

      /* check if we have already been intialised, if so just AddRef */
      if( NULL != sOEMFMRDSPtr){
         (void)IFMRDS_AddRef((IFMRDS *)sOEMFMRDSPtr);
         nResult = AEE_SUCCESS;
         break;
      }

      AEEFMRDS_MSG_LOW("Starting FMRDS....",0,0,0);

      sOEMFMRDSPtr = &sOEMFMRDS;

      sOEMFMRDSPtr->pvt = &gIFMRDS;

      sOEMFMRDSPtr->m_nRefs = 1;

      sOEMFMRDSPtr->ps = ps;

      sOEMFMRDSPtr->m_pcbRawData = NULL;

      // Registering with FMRadio.
       FMRadioCALLBACK_Init(&sOEMFMRDSPtr->m_cbFMRadio, (FMRadioEventNotify *)OEMFMRDS_FMRadioEventsCb, NULL);
       nResult = OEMFMRadio_Register_Events(&sOEMFMRDSPtr->m_cbFMRadio,
                                           aefmrds_event_type,
                                           sizeof(aefmrds_event_type)/sizeof(aefmrds_event_type[0]));
       BREAKIF(nResult);

      (void)ISHELL_AddRef(ps);

      nResult = AEE_SUCCESS;

   }while(FALSE);


   if( SUCCESS != nResult){
     DBGPRINTF_FATAL("Unable to start the FMRDS Service");

     OEMFMRDS_FreeData(sOEMFMRDSPtr);
   }
   else {
      DBGPRINTF_HIGH("Successfully started the FMRDS");

      *ppObj = (void *)sOEMFMRDSPtr;

   }

   return nResult;
}

/*=============================================================================
Function:
   OEMFMRDS_FMRadioEventsCb

Description:
   Event callback from FMRDS layer

Parameters:
   fmrds_event : event info structure

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=============================================================================*/
static void OEMFMRDS_FMRadioEventsCb(void *p, fmrds_event_msg_type *pFMRDS_Event)
{
   OEMFMRDS *pMe = sOEMFMRDSPtr;
   int nResult = SUCCESS;

   if( NULL == pMe){
      return;
   }

   DBGPRINTF_LOW("Recieved Event:%d",pFMRDS_Event->fmrdsEventName,0,0);

   switch(pFMRDS_Event->fmrdsEventName){

      case FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA:
         AEEFMRDS_MSG_LOW("Received event for availability of RDS/RBDS data",0,0,0);

         nResult = OEMFMRDS_ProcessRDSData(*pFMRDS_Event, pMe);
         break;
   }
}
/*=============================================================================
Function:
   OEMFMRDS_AddRef

Description:
   Increments the reference count of the IFMRDS object

Parameters:
   pIFMRDS  :  Pointer to the interface object

Return Value:
   Incremented reference count

Comments:

Side Effects:

See Also:

=============================================================================*/
static uint32 OEMFMRDS_AddRef(IFMRDS *pIFMRDS)
{
   OEMFMRDS *pMe = (OEMFMRDS *)(void *) pIFMRDS;

   if ( NULL != pMe ) {
     return ( ++pMe->m_nRefs );
   }

   return 0;
}

/*=============================================================================
Function:
   OEMFMRDS_Release

Description:
   Release the IFMRDS object once the reference count reaches zero

Parameters:
   pIFMRDS  :  Pointer to the interface object

Return Value:
   Returns the decremented reference count, if count reaches zero destroys the
   object

Comments:

Side Effects:

See Also:

=============================================================================*/
static uint32 OEMFMRDS_Release(IFMRDS *pIFMRDS)
{
  OEMFMRDS *pMe = (OEMFMRDS *)(void *)pIFMRDS;

  if ( NULL != pMe ){

     if( 0 != --pMe->m_nRefs) {
        return pMe->m_nRefs;
     }

     (void)ISHELL_Release(sOEMFMRDSPtr->ps);

     OEMFMRDS_FreeData(pMe);

  }

  return 0;
}


/*=============================================================================
Function:
   OEMFMRDS_FreeData

Description:
   Frees the FMRDS object internal parameters

Parameters:
   pOEMFMRDS : Pointer to the OEMFMRDS structure

Return Value:
   None

Comments:

Side Effects:

See Also:

=============================================================================*/
static void OEMFMRDS_FreeData(OEMFMRDS *pOEMFMRDS)
{
  PARAM_NOT_REF(pOEMFMRadio)

  FREEIF(pOEMFMRDS->m_RawDataBlocks.m_pRDSData);

  OEMFMRDS_ClearRDSData(pOEMFMRDS);

  FMRadio_CBCancel(&sOEMFMRDS.m_cbFMRadio);

  MEMSET(&sOEMFMRDS,0,sizeof(sOEMFMRDS));

 
  sOEMFMRDSPtr = NULL;

  return;

}


/*=============================================================================
Function:
   OEMFMRDS_QueryInterface

Description:
   This method asks an object for another API contract from the object in
   question.

Parameters:
   po  :   Pointer to the object
   iid :  Requested interface ID exposed by the object
   ppo :  Returned object.  Filled by this method

Return Value:
   AEE_SUCCESS - Interface found
   AEE_ENOMEMORY - Insufficient memory
   AEE_ECLASSNOTSUPPORT - Requested interface is unsupported
   Another AEEError.h, if appropriate

Comments:

Side Effects:

See Also:

=============================================================================*/
static int OEMFMRDS_QueryInterface(IFMRDS    *pIFMRDS,
                                   AEECLSID  iid,
                                   void      **ppo)
{
  OEMFMRDS   *pMe     = (OEMFMRDS *)(void *)pIFMRDS;
  int        nResult  = ECLASSNOTSUPPORT;

  if( NULL == pMe || NULL == ppo ) {
    return EFAILED;
  }

  *ppo = NULL;

  if( AEECLSID_FMRDS == iid) {

    *ppo = (void *)pIFMRDS;

    (void)IFMRDS_AddRef(pIFMRDS);

    nResult  = SUCCESS;
  }

  return nResult;
}
/*============================================================================
Function:
   OEMFMRDS_GetInfoData

Description:
   This function tries to retrieve the data requested for FMRDSInfo id when
   the app is notified through IFMRDS_InfoReadable CB.

   The apps can also directly poll to get the info data, in that case there
   may be cases in which there's no data available yet for that FMRDSInfo.
 
Parameters:

   pIFMRDS     :  Pointer to the IFMRDS Interface object.
   nId         :  The RDSSource data id for which data has to be retrieved.
   pnSequence  :  This will be incremented each time we receive new block of
                  data. Using this sequence number apps can come to know
                  whether they have retrieved this chunk of data already.
   pBuf        :  Contains the corresponding information.
   nBufSize    :  The size of the pBuf.
   pnBufSizeReq : The required size of the buffer. If the passed in buffer
                  pBuf of size nSize is not sufficient to hold the current
                  data then we fill in the pnBufSizeReq with size needed,
                  and zero out the pBuf.
Return Value:
   SUCCESS         : If successful.
   ENOTYPE         : If no data is available for that Id.
   EBUFFERTOOSMALL : If the passed in buffer is too small to store the data
                       and pnBufSizeReq if not NULL would say the size needed
 
Comments:
  None
 
See Also:
  None
===========================================================================*/
static int OEMFMRDS_GetInfoData(IFMRDS         *pIFMRDS,
                                AEEFMRDSInfo   nId,
                                int            *pnSequence,
                                void           *pBuf,
                                uint32         nBufSize,
                                uint32         *pnBufSizeReq)


{
   OEMFMRDS *pMe = (OEMFMRDS *)(void *)pIFMRDS;
   int nResult = AEE_SUCCESS;
   uint32 nReqSize = 0;

   //do decrement, as our array index is 0 based
   nId--;

   //we do <= comparison for last group id, if the user passes in 33, our
   //index may be out of bounds
   if( NULL == pMe || NULL == pnSequence || FMRDS_LAST_GROUPID <= nId || 
         NULL == pnBufSizeReq){
      return EBADPARM;
   }


   do {

      nReqSize = pMe->m_RDSGroupedData[nId].m_nBytesAllocated;

#ifdef FMRDS_ON_ULC
      if(  0x0 == gRawDataBuffer[0].nBlockB || 0 == nReqSize ){
#else
      if( NULL == pMe->m_RDSGroupedData[nId].m_pRDSData || 0 == nReqSize){
#endif /* FMRDS_ON_ULC */
         /* we dont have any data yet for this group id. */
         nResult = ENOTYPE;
         break;
      }

      /* See, if we have enough space and the buffer is valid */
      if(NULL == pBuf){
         *pnBufSizeReq = nReqSize;
         break;
      }

      *pnSequence = pMe->m_RDSGroupedData[nId].m_nSeqNum;

#ifdef FMRDS_ON_ULC
      if( SUCCESS != OEMFMRDS_GroupedRawData_Fill_ULC(pIFMRDS,
                                                      pBuf,
                                                      nBufSize,
                                                      nId) ){
         AEEFMRDS_MSG_FATAL("Unable to fill the buffer for ULC",0,0,0);
      }

#else 
      if(nBufSize < nReqSize){
         (void)MEMCPY(pBuf, pMe->m_RDSGroupedData[nId].m_pRDSData, nBufSize * sizeof(AEEFMRDSGroup));
      }
      else{
         (void)MEMCPY(pBuf,pMe->m_RDSGroupedData[nId].m_pRDSData, nReqSize * sizeof(AEEFMRDSGroup));
      }
#endif /* FMRDS_ON_ULC */

      *pnBufSizeReq = nReqSize;

   }while (0);


   return nResult;
}

/*============================================================================
Function:
   OEMFMRDS_GroupedRawData_Fill_ULC

Description:
   This function tries to retrieve the data requested for FMRDSInfo id when
   the app is notified through IFMRDS_InfoReadable CB.

   The apps can also directly poll to get the info data, in that case there
   may be cases in which there's no data available yet for that FMRDSInfo.
 
Parameters:

   pIFMRDS     :  Pointer to the IFMRDS Interface object.
   nId         :  The RDSSource data id for which data has to be retrieved.
   pBuf        :  Contains the corresponding information.
   nBufSize    :  The size of the pBuf.
   urn Value:
   SUCCESS         : If successful.
   ENOTYPE         : If no data is available for that Id.
   EBUFFERTOOSMALL : If the passed in buffer is too small to store the data
                       and pnBufSizeReq if not NULL would say the size needed
 
Comments:
   This function is used to fill out the buffer, if we have macro ULC
   enabled

   Nore passed in AEEFMRDSInfo id(nId) shouldn't be decremented its already
   decremented by the caller
  
 
See Also:
  None
===========================================================================*/
#ifdef FMRDS_ON_ULC
int OEMFMRDS_GroupedRawData_Fill_ULC(IFMRDS       *pIFMRDS,
                                     void         *pBuf,
                                     uint32       nBufSize,
                                     AEEFMRDSInfo nId)
{
   int nResult = AEE_SUCCESS;
   AEEFMRDSGroup *pNewGroup = NULL;
   OEMFMRDS *pMe = (OEMFMRDS *)(void *)pIFMRDS;
   int Ctr = 0;
   int nSize = 0;

   if ( NULL == pMe || NULL == pBuf ){
      return EBADPARM;
   }

   

   while( Ctr < OEMFMRDS_MAX_BUF_SIZE ){
      pNewGroup = gRawDataBuffer + Ctr;

      if( nId == OEMFMRDS_GROUPID(pNewGroup->nBlockB) ){
         MEMCPY((AEEFMRDSGroup *)pBuf + nSize,pNewGroup,sizeof(*pNewGroup));
         nSize++;
      }
      Ctr++;
   }

   return nResult;

}
#endif /* FMRDS_ON_ULC */

/*============================================================================
Function:
   OEMFMRDS_GetNextData

Description:
   This function is used to retrieve the raw data. The data would be
   returned in groups in AEEFMRDSGroup


Parameters:

   pIFMRDS              :  Pointer to the IFMRDS Interface object.
   pnSequence           :  This will be incremented each time we receive
                           new block of data. Using this sequence number
                           apps can come to know whether they have
                           retrieved this chunk of data already.
   pFMRDSGroupsBuf      :  Contains the raw data blocks
   nFMRDSGroupsBufSize  :  The size of the pFMRDSGroupsBuf 
   pnFMRDSBufSizeReq    :  The size that would be required to get the buffer
                           Will be a multiple of sizeof(AEEFMRDSGroup).If
                           buf isn't NULL, upon return would contain the
                           actual number of items filled

Return Value:
   SUCCESS         : If successful.
   ENOTYPE         : If no data is available for that type.
   EBUFFERTOOSMALL : If the provided buffer is too small to fill with data
   EBADPARM        : If either pnSequence or pnFMRDSBufSizeReq is NULL

Comments:
   None

See Also:
   None
===========================================================================*/
static int OEMFMRDS_GetNextData(IFMRDS          *pIFMRDS,
                                int             *pnSequence,
                                AEEFMRDSGroup   *pFMRDSGroupsBuf,
                                int             nFMRDSGroupsBufSize,
                                int             *pnFMRDSBufSizeReq)
{
   OEMFMRDS *pMe = (OEMFMRDS *)(void *)pIFMRDS;
   int nResult = AEE_SUCCESS;
   int nBytesToBeFilled = 0;

   if( NULL == pMe || NULL == pnSequence || NULL == pnFMRDSBufSizeReq){
      return EBADPARM;
   }

   do {

#ifdef FMRDS_ON_ULC
      if(  0x0 == gRawDataBuffer[0].nBlockB || 0 == pMe->m_RawDataBlocks.m_nBytesAllocated ){
         nResult = ENOTYPE;
         break;
      }
#else 
      if( NULL == pMe->m_RawDataBlocks.m_pRDSData || 0 == pMe->m_RawDataBlocks.m_nBytesAllocated){
         nResult = ENOTYPE;
         break;
      }
#endif /* FMRDS_ON_ULC */

      if(NULL == pFMRDSGroupsBuf){
         *pnFMRDSBufSizeReq = pMe->m_RawDataBlocks.m_nBytesAllocated;
         break;
      }

      // To support partial filling
      if(nFMRDSGroupsBufSize < (int)pMe->m_RawDataBlocks.m_nBytesAllocated){
         nBytesToBeFilled = nFMRDSGroupsBufSize * sizeof(AEEFMRDSGroup);
      }
      else{
         nBytesToBeFilled = (int)pMe->m_RawDataBlocks.m_nBytesAllocated * sizeof(AEEFMRDSGroup);
      }

      

#ifdef FMRDS_ON_ULC
      MEMCPY(pFMRDSGroupsBuf,
             gRawDataBuffer,
             nBytesToBeFilled);
#else 
      (void)MEMCPY(pFMRDSGroupsBuf,
                   pMe->m_RawDataBlocks.m_pRDSData,
                   nBytesToBeFilled);

#endif /* FMRDS_ON_ULC */

      *pnSequence = pMe->m_RawDataBlocks.m_nSeqNum;

      *pnFMRDSBufSizeReq = pMe->m_RawDataBlocks.m_nBytesAllocated;

   }while(0);


   return nResult;
}

/*=============================================================================
Function:
   OEMFMRDS_InfoReadable

Description:
   This function schedules a callback to be called when it has any RDS data
   indicated by nId ready for the app to consume.
   
Parameters:
   pIFMRDS   :  Pointer to the IFMRDS Interface object.
   pcb       :  Where to call when the IFMRDS may be readable.
   nId       :  The FMRDSInfo id in which the apps are interested.
   nType     :  Indicates in what type of data stream the user is interested
   
Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void OEMFMRDS_InfoReadable(IFMRDS              *pIFMRDS,
                                  AEECallback         *pcb,
                                  AEEFMRDSInfo        nId,
                                  AEEFMRDSDataStream  nType)
{
   OEMFMRDS *pMe = (OEMFMRDS *)(void *)pIFMRDS;
   int nResult = EFAILED;
   int nCBId = 0;


   if(NULL == pcb || NULL == pcb->pfnNotify || FMRDS_LAST_GROUPID <= nId ){
      return;
   }

   nId--; //our index is 0 based

   /* Cancel the CB if it was queued elsewhere */
   CALLBACK_Cancel(pcb);

   pcb->pfnCancel = NULL;
   pcb->pCancelData = NULL;
   
   if( AEEFMRDS_RAW_DATA == nType){
      nId = 0;
      pcb->pNext = pMe->m_pcbRawData;
      pMe->m_pcbRawData = pcb;
      nResult = AEE_SUCCESS;
   }
   else if( AEEFMRDS_PARSED_DATA == nType){
      
      pcb->pNext = pMe->m_pcbParsedData[nId];
      pMe->m_pcbParsedData[nId] = pcb;
      nResult = AEE_SUCCESS;
   }
   else if( AEEFMRDS_RAW_GROUPED_DATA == nType){

      pcb->pNext = pMe->m_pcbGroupedData[nId];
      pMe->m_pcbGroupedData[nId] = pcb;
      nResult = AEE_SUCCESS;
   }


#define OEMFMRDS_ENCODE_CB_ID(group,type,val) { val =0; val = (group) << 16; val |= type; }
   if( SUCCESS == nResult ){

      OEMFMRDS_ENCODE_CB_ID(nId,nType,nCBId);

      /* this would be our key while searching. */
      pcb->pReserved =  (void *) nCBId;  
      pcb->pmc =(void *) AEE_GetAppContext();

      pcb->pfnCancel = OEM_FMRDS_CallBackCancel;
      pcb->pCancelData = (void *)pMe;
   }

   return;

}
/*=============================================================================
IFMRDS_GetParsedData

Description:

Prototype:
   int OEMFMRDS_GetParsedData(IFMRDS              *pIFMRDS,
                              AEEFMRDSInfo        nId,
                              AEEFMRDSParsedInfo  *pRDSInfo);

Parameters:

   pIFMRDS :  Pointer to the IFMRDS Interface object.
   nId     :  The RDSSource data id for which parsed data has to be retrieved
   pRDSInfo:  Pointer to RDSInfo structure.

Return Value:
   SUCCESS  : If successful.
   ENOTYPE  : If no data is available for that Id.
   EBADPARM : If pRDSInfo is NULL

Comments:

   Though the passed in RDSInfo structure may contain all the members for
   all groups, but upon return the user should look only for data that is
   valid for that group.

   E.g., The structure may contain a ptr to RT buffer, if the user
   has queried for type OA elements then he is expected to look for only
   Program Service (PS) Name, in the structure apart from the common fields
   like PI code, PT code, and TP id code.

See Also:
   None

=============================================================================*/
static int OEMFMRDS_GetParsedData(IFMRDS              *pIFMRDS,
                                  AEEFMRDSInfo        nId,
                                  AEEFMRDSParsedInfo  *pRDSInfo)
{

   OEMFMRDS *pMe = (OEMFMRDS *)(void *)pIFMRDS;
   int nResult = AEE_SUCCESS;
   int nNumOfGroups = 0, nIndex = 0;
   int nHourCodeBlockC = 0, nAFCnt = 0;
   AEEFMRDSGroup  *pNewGroup = NULL;
   AEEFMRDSInfo   GroupId = 0;

   uint8  PS_DIAddr        = 0;     // Decoder identification address
   uint8  DISegment        = 0;
   uint8  TextSegmentAddr  = 0;
   uint8  PTYNAddress      = 0;
   uint8  PSSegmentAddr    = 0; 
   uint32 nMJDCodeBlockB   = 0;     //to store last 2 bits of block B
   boolean  bTextFlag      = FALSE; //used to refresh radio text
   boolean  bLF_MF_CODE    = FALSE; //used to differentiate VHF&LF/MF frequency
   boolean bAFCodeLenUpdate = FALSE;

#ifdef FMRDS_ON_ULC
   AEEFMRDSGroup GroupedDataBuffer[OEMFMRDS_MAX_BUF_SIZE];
#endif

   if(NULL == pIFMRDS || NULL == pRDSInfo || FMRDS_LAST_GROUPID <= nId){
      return EBADPARM;
   }

   nNumOfGroups = pMe->m_RDSGroupedData[nId - 1].m_nBytesAllocated;
   if(0 == nNumOfGroups){
      return EFAILED;
   }

#ifdef FMRDS_ON_ULC
   MEMSET((void *)GroupedDataBuffer, 0, OEMFMRDS_MAX_BUF_SIZE* sizeof(AEEFMRDSGroup));
   if( SUCCESS != OEMFMRDS_GroupedRawData_Fill_ULC(pIFMRDS,
                                                   (void *)GroupedDataBuffer,
                                                   OEMFMRDS_MAX_BUF_SIZE * sizeof(AEEFMRDSGroup),
                                                   nId) ){
      AEEFMRDS_MSG_FATAL("Unable to fill the buffer for ULC",0,0,0);
      return EFAILED;
   }
#endif

   
   for(nIndex = 0; nIndex < nNumOfGroups; nIndex++){

#ifdef FMRDS_ON_ULC
      pNewGroup = &GroupedDataBuffer[nIndex];

#else
	   pNewGroup = (pMe->m_RDSGroupedData[nId - 1].m_pRDSData + nIndex);
#endif

      
      /*==================== Parsing BlockA =============================*/
	   pRDSInfo->nPICode = pNewGroup->nBlockA;

	   GroupId = (AEEFMRDSInfo)FMRDSPARSER_GROUP_ID(pNewGroup->nBlockB);
	   if(nId != GroupId + 1){
	      nResult = EFAILED;
		   break;
	   }

	   //PTY & TP codes
      pRDSInfo->nPTYCode  = (uint8 )FMRDSPARSER_PTY_CODE(pNewGroup->nBlockB);
      pRDSInfo->bTPCode  = (boolean)FMRDSPARSER_TP_CODE(pNewGroup->nBlockB);


      
      /*==================== Parsing BlockB =============================*/
	   switch(nId){

	   case AEEFMRDS_INFO_0A:
      case AEEFMRDS_INFO_0B:
      case AEEFMRDS_INFO_15B:
         {

		     //MS and TA codes
           pRDSInfo->bMSCode = (boolean)FMRDSPARSER_MS_CODE(pNewGroup->nBlockB);
           pRDSInfo->bTACode = (boolean)FMRDSPARSER_TA_CODE(pNewGroup->nBlockB);

		     //DI code
           PS_DIAddr =(uint8)FMRDSPARSER_PS_DIADDR_CODE(pNewGroup->nBlockB);
           DISegment =(uint8)FMRDSPARSER_DI_SEGMENT(pNewGroup->nBlockB);

		   switch(PS_DIAddr){

		   case 0x0:{
			   pRDSInfo->nDICode =
				   (pRDSInfo->nDICode & 0x7) | (DISegment << 3);
					}
			   break;

		   case 0x1:{
			   pRDSInfo->nDICode =
				   (pRDSInfo->nDICode & 0xB) | (DISegment << 2);
					}
			   break;

		   case 0x2:{
			    pRDSInfo->nDICode =
					(pRDSInfo->nDICode & 0xD) | (DISegment << 1);
					}
			   break;

		   case 0x3:{
			    pRDSInfo->nDICode =
                     (pRDSInfo->nDICode & 0xE) | (DISegment );
					}
			   break;

		   default:
			   break;
		   }
							 }
		   break;


		   //Radio text message
	   case AEEFMRDS_INFO_2A:
      case AEEFMRDS_INFO_2B:
         {

   		   TextSegmentAddr = 
	   		   (uint8)FMRDSPARSER_TSADDR_CODE(pNewGroup->nBlockB);
		      bTextFlag = FMRDSPARSER_TEXTABFLAG_CODE(pNewGroup->nBlockB);
		   
		      if (bTextFlag != pRDSInfo->bABTextFalg) {
			      (void)MEMSET(pRDSInfo->aRTMessage,
				                0,
                            sizeof(pRDSInfo->aRTMessage)/sizeof(pRDSInfo->aRTMessage[0]));
   			   pRDSInfo->bABTextFalg = bTextFlag;
	   	   }
         }
		   break;

      case AEEFMRDS_INFO_3A:
         {
            pRDSInfo->sAICode.nAGTypeCode = 
               (int)FMRDSPARSER_AGT_CODE(pNewGroup->nBlockB);
         }
         break;

      case AEEFMRDS_INFO_4A:
         {
            nMJDCodeBlockB =
               (uint32)FMRDSPARSER_MJD_BLOCKB_CODE(pNewGroup->nBlockB);
         }
         break;

         //PTYName segment address
      case AEEFMRDS_INFO_10A:
         {
            PTYNAddress =  (uint8)FMRDSPARSER_PTYN_ADDRESS_CODE(pNewGroup->nBlockB);
         }
         break;

      case AEEFMRDS_INFO_5A:
      case AEEFMRDS_INFO_5B:
      case AEEFMRDS_INFO_6A:
      case AEEFMRDS_INFO_6B:
      case AEEFMRDS_INFO_7A:
      case AEEFMRDS_INFO_7B:
      case AEEFMRDS_INFO_8A:
      case AEEFMRDS_INFO_8B:
      case AEEFMRDS_INFO_9A:
      case AEEFMRDS_INFO_9B:
      case AEEFMRDS_INFO_10B:
      case AEEFMRDS_INFO_11A:
      case AEEFMRDS_INFO_11B:
      case AEEFMRDS_INFO_12A:
      case AEEFMRDS_INFO_12B:
      case AEEFMRDS_INFO_13A:
      case AEEFMRDS_INFO_13B:
      case AEEFMRDS_INFO_14A:
         {
            // do nothing
         }
         break;
                   
      //TA code
      case AEEFMRDS_INFO_14B:
         {
            pRDSInfo->bTACode = FMRDSPARSER_TA_CODE(pNewGroup->nBlockB);
         }
         break;

      case AEEFMRDS_INFO_15A:
         {
            pRDSInfo->bTACode = (boolean)FMRDSPARSER_TA_CODE(pNewGroup->nBlockB);
            PSSegmentAddr = (uint8)FMRDSPARSER_PSSEGMENTADR_CODE(pNewGroup->nBlockB);
         }
         break;
         

	   default:
		   break;
      }


	   /*==================== Parsing BlockC =================================*/
	   switch(nId){

         case AEEFMRDS_INFO_0A:
            {
               int AFValue = 0;
               int nNofAFs = 0;

               for (nNofAFs =1; nNofAFs <=2; nNofAFs++) {

                  if(TRUE == bAFCodeLenUpdate){
                     break;
                  }
                  
                  if(nNofAFs == 1){
                     //parse 1st 8 bits of block
                     AFValue = (int)FMRDSPARSER_AF_CODE1(pNewGroup->nBlockC);
                  }
                  else{
                     //parse last 8 bits of block
                     AFValue = (int)FMRDSPARSER_AF_CODE2(pNewGroup->nBlockC);
                  }

                  do{
                     if(0 == AFValue){
                        nResult = EBADPARM;
                        break;
                     }

                     if(AFValue <= 204){
                        
                        if ((NULL == pRDSInfo->sAFCodePairs.pAFCodePairs)&&
                                                                  (nAFCnt !=0)){
                         break;
                        }

                        //check it is VHF or (LF/MF frequency)
                        if(TRUE == bLF_MF_CODE){
                            
                           //assigning LF or MF frequencies
                           if(AFValue <= 15){

                              if(NULL != pRDSInfo->sAFCodePairs.pAFCodePairs &&
                                    nAFCnt < pRDSInfo->sAFCodePairs.nBufLen){
                                 *((pRDSInfo->sAFCodePairs.pAFCodePairs) + nAFCnt) 
                                    = LF_START_FREQ + (AFValue-1)* LF_FREQ_SPACE;
                                 nAFCnt = nAFCnt + 1;
                                 bLF_MF_CODE = FALSE;
                              }
                              else{
                                 nResult = EFAILED;
                                 break;
                              }

                              

                           }
                           else if ((AFValue >=16) & (AFValue <= 135)){

                               //MF frequencies starts from 16. so we need to 
                               //subtract 16 from AFValue to get proper frequ-
                               //ency seperation from MF start frequency.
                                if(NULL != pRDSInfo->sAFCodePairs.pAFCodePairs &&
                                    nAFCnt <  pRDSInfo->sAFCodePairs.nBufLen){
                                   *(pRDSInfo->sAFCodePairs.pAFCodePairs + nAFCnt) 
                                      = MF_START_FREQ + (AFValue-16)*MF_FREQ_SPACE;
                                  nAFCnt = nAFCnt + 1;
                                  bLF_MF_CODE = FALSE;
                                }
                                else{
                                   nResult = EFAILED;
                                   break;
                                }
                               

                           }
                           else{
                              nResult = EBADPARM;
                            }
                           
                        }
                        else{
                           
                           //assigning VHF frequencies
                            if(NULL != pRDSInfo->sAFCodePairs.pAFCodePairs &&
                                    nAFCnt <  pRDSInfo->sAFCodePairs.nBufLen){
                               *(pRDSInfo->sAFCodePairs.pAFCodePairs + nAFCnt) 
                                 = VHF_START_FREQ + (AFValue-1)*VHF_FREQ_SPACE;
                              nAFCnt = nAFCnt + 1;  
                            }
                            else{
                               nResult = EFAILED;
                               break;
                            }
                           
                        }

                        break;
                     }

                     if(((AFValue >= 205) && (AFValue <= 223))||((AFValue >= 251) && (AFValue <= 255))){
                        break;
                     }

                     if(224 == AFValue){

                        //NO AF exists; >> prasd << verify
                        // pRDSInfo->sAFCodePairs.nBufLen = 0;
                        break;
                     }

                     if((AFValue >= 225) && (AFValue <=249)){

                         if(NULL != pRDSInfo->sAFCodePairs.pAFCodePairs &&
                                    nAFCnt <  pRDSInfo->sAFCodePairs.nBufLen){
                            *(pRDSInfo->sAFCodePairs.pAFCodePairs + nAFCnt)= 
                              AFValue - 224;
                           nAFCnt = nAFCnt +1;
                         }
                         else{
                            nResult = EFAILED;
                         }
                        break;
                     }
                     
                     if(250 == AFValue){
                        bLF_MF_CODE = TRUE;
                        break;
                     }

                  }while(0);

                  if(EFAILED == nResult){
                     bAFCodeLenUpdate = TRUE;
                     break;
                  }
               }
            }
            break;

            // Extended Country code
         case AEEFMRDS_INFO_1A:
            {
               if(FMRDSPARSER_VARIANT_CODE(pNewGroup->nBlockB) == 0){
                  pRDSInfo->nECCode = (uint8)FMRDSPARSER_EC_CODE(pNewGroup->nBlockC);
               }
            }
            break;
         
         case AEEFMRDS_INFO_2A:
            {
               pRDSInfo->aRTMessage[TextSegmentAddr * 4] = 
			         (char)FMRDSPARSER_RT_CODE1(pNewGroup->nBlockC);
      		   pRDSInfo->aRTMessage[TextSegmentAddr * 4 + 1] = 
		      	   (char)FMRDSPARSER_RT_CODE2(pNewGroup->nBlockC);
            }
            break;


         case AEEFMRDS_INFO_3A:
            {
               pRDSInfo->sAICode.nMessage = (uint16)pNewGroup->nBlockC;
            }
            break;

         case AEEFMRDS_INFO_4A:
            {
               pRDSInfo->sClkTDCode.dwMJDCode = 
                  (uint32)FMRDSPARSER_MJD_CODE(pNewGroup->nBlockC, nMJDCodeBlockB);

               nHourCodeBlockC = (int)(pNewGroup->nBlockC & 0x1);
            }
            break;

            //Program type name
         case AEEFMRDS_INFO_10A:
            {
               pRDSInfo->aPTYName[PTYNAddress*4 ] =
                  (uint8) FMRDSPARSER_PTYN_CODE1(pNewGroup->nBlockC);

               pRDSInfo->aPTYName[PTYNAddress*4 + 1] =
                  (uint8) FMRDSPARSER_PTYN_CODE2(pNewGroup->nBlockC);
            }
            break;

            //Program service
         case AEEFMRDS_INFO_15A:
            {
               pRDSInfo->aPSName[PSSegmentAddr*4]  =
                  (uint8)FMRDSPARSER_PS_CODE1(pNewGroup->nBlockC);
               
               pRDSInfo->aPSName[PSSegmentAddr*4+1] = 
                  (uint8)FMRDSPARSER_PS_CODE2(pNewGroup->nBlockC);
            }
            break;

         case AEEFMRDS_INFO_0B:
         case AEEFMRDS_INFO_1B:
         case AEEFMRDS_INFO_2B:
         case AEEFMRDS_INFO_3B:
         case AEEFMRDS_INFO_4B:
         case AEEFMRDS_INFO_5B:
         case AEEFMRDS_INFO_6B:
         case AEEFMRDS_INFO_7B:
         case AEEFMRDS_INFO_8B:
         case AEEFMRDS_INFO_9B:
         case AEEFMRDS_INFO_10B:
         case AEEFMRDS_INFO_11B:
         case AEEFMRDS_INFO_12B:
         case AEEFMRDS_INFO_13B:
         case AEEFMRDS_INFO_15B:
            {
               if(0 != (pRDSInfo->nPICode ^ pNewGroup->nBlockC) ){
                  nResult = EFAILED;
               }
            }
            break;

         case AEEFMRDS_INFO_5A:
         case AEEFMRDS_INFO_6A:
         case AEEFMRDS_INFO_7A:
         case AEEFMRDS_INFO_8A:
         case AEEFMRDS_INFO_9A:
         case AEEFMRDS_INFO_11A:
         case AEEFMRDS_INFO_12A:
         case AEEFMRDS_INFO_13A:
         case AEEFMRDS_INFO_14A:
         case AEEFMRDS_INFO_14B:
            {
               //do nothing
            }

         default:
            break;
      }

      if(SUCCESS != nResult){
         break; 
      }


	   /*==================== Parsing BlockD =================================*/
	   switch(nId){

         //PS Code
         case AEEFMRDS_INFO_0A:
         case AEEFMRDS_INFO_0B:
            {
               pRDSInfo->aPSName[PS_DIAddr*2]		=
                  (uint8)FMRDSPARSER_PS_CODE1(pNewGroup->nBlockD);
               pRDSInfo->aPSName[PS_DIAddr*2+1]    = 
                  (uint8)FMRDSPARSER_PS_CODE2(pNewGroup->nBlockD);
            }
            break;

            // PIN code
         case AEEFMRDS_INFO_1A:
         case AEEFMRDS_INFO_1B:
            {
               pRDSInfo->sPinCode.nDayItemCode   = 
                  (uint16)FMRDSPARSER_PINDAY_CODE(pNewGroup->nBlockD);
               
               pRDSInfo->sPinCode.nHourItemCode   = 
                  (uint16)FMRDSPARSER_PINHOUR_CODE(pNewGroup->nBlockD);

               pRDSInfo->sPinCode.nMinuteItemCode =
                  (uint16)FMRDSPARSER_PINMINUTE_CODE(pNewGroup->nBlockD);
            }
            break;

		   // Radio text message
         case AEEFMRDS_INFO_2A:
            {
               pRDSInfo->aRTMessage[TextSegmentAddr*4 + 2]     =
                  (unsigned char)FMRDSPARSER_RT_CODE1(pNewGroup->nBlockD);
               pRDSInfo->aRTMessage[TextSegmentAddr*4 + 3]  =
                  (unsigned char)FMRDSPARSER_RT_CODE2(pNewGroup->nBlockD);
            }
            break;

         case AEEFMRDS_INFO_2B:
            {
               
		         pRDSInfo->aRTMessage[TextSegmentAddr * 2] =
			         (char)FMRDSPARSER_RT_CODE1(pNewGroup->nBlockD);
      		   pRDSInfo->aRTMessage[TextSegmentAddr * 2 + 1] =
		      	   (char)FMRDSPARSER_RT_CODE2(pNewGroup->nBlockD);
            }
            break;

            // Application identification 
         case AEEFMRDS_INFO_3A:
            {
               pRDSInfo->sAICode.nAID =(int)pNewGroup->nBlockD;
            }
            break;

            // clock time
         case AEEFMRDS_INFO_4A:
            {
               pRDSInfo->sClkTDCode.nHourCode = 
                  (uint16)FMRDSPARSER_HOUR_CODE(pNewGroup->nBlockD,nHourCodeBlockC);

               pRDSInfo->sClkTDCode.nMinuteCode = 
                  (uint16)FMRDSPARSER_MINUTE_CODE(pNewGroup->nBlockD);
               
               pRDSInfo->sClkTDCode.nLocalOffsetTimeCode = 
                  (int16)FMRDSPARSER_LOFFSET_CODE(pNewGroup->nBlockD);
            }
            break;

            //Program type name
         case AEEFMRDS_INFO_10A:
            {
               pRDSInfo->aPTYName[PTYNAddress*4 + 2] =
                  (uint8) FMRDSPARSER_PTYN_CODE1(pNewGroup->nBlockD);

               pRDSInfo->aPTYName[PTYNAddress*4 + 3] =
                  (uint8) FMRDSPARSER_PTYN_CODE2(pNewGroup->nBlockD);
            }
            break;

            //Program service
          case AEEFMRDS_INFO_15A:
            {
               pRDSInfo->aPSName[PSSegmentAddr*4 +2 ]		=
                  (uint8)FMRDSPARSER_PS_CODE1(pNewGroup->nBlockD);

               pRDSInfo->aPSName[PSSegmentAddr*4+ 3 ]    = 
                  (uint8)FMRDSPARSER_PS_CODE2(pNewGroup->nBlockD);
               
            }
            break;

            //DI code (fast basic tuning)
          case AEEFMRDS_INFO_15B:
            {

               PS_DIAddr =(uint8)FMRDSPARSER_PS_DIADDR_CODE(pNewGroup->nBlockD); 
               DISegment =(uint8)FMRDSPARSER_DI_SEGMENT(pNewGroup->nBlockD);

               switch(PS_DIAddr){
                  case 0x0:
                     pRDSInfo->nDICode =
                          (pRDSInfo->nDICode & 0x7) | (DISegment << 3);
                     break;

                  case 0x1:
                     pRDSInfo->nDICode =
                         (pRDSInfo->nDICode & 0xB) | (DISegment << 2);
                     break;

                  case 0x2:
                     pRDSInfo->nDICode =
                         (pRDSInfo->nDICode & 0xD) | (DISegment << 1);
                     break;

                  case 0x3:
                     pRDSInfo->nDICode =
                         (pRDSInfo->nDICode & 0xE) | (DISegment );
                     break;

                  default:
                     break;
               }
            }
            break;

          case AEEFMRDS_INFO_3B:
          case AEEFMRDS_INFO_4B:
          case AEEFMRDS_INFO_5A:
          case AEEFMRDS_INFO_5B:
          case AEEFMRDS_INFO_6A:
          case AEEFMRDS_INFO_6B:
          case AEEFMRDS_INFO_7A:
          case AEEFMRDS_INFO_7B:
          case AEEFMRDS_INFO_8A:
          case AEEFMRDS_INFO_8B:
          case AEEFMRDS_INFO_9A:
          case AEEFMRDS_INFO_9B:
          case AEEFMRDS_INFO_10B:
          case AEEFMRDS_INFO_11A:
          case AEEFMRDS_INFO_11B:
          case AEEFMRDS_INFO_12A:
          case AEEFMRDS_INFO_12B:
          case AEEFMRDS_INFO_13A:
          case AEEFMRDS_INFO_13B:
          case AEEFMRDS_INFO_14A:
            {
               // do nothing
            }
            break;
            

         default:
            break;
	   }
   }

   //UpDating AFCodePairs Required len
   if(AEEFMRDS_INFO_0A == nId && TRUE == bAFCodeLenUpdate && nIndex == nNumOfGroups - 1){
         pRDSInfo->sAFCodePairs.nBufLenReqd = nNumOfGroups * 2;
   }

   return nResult;
}

/*=============================================================================
Function:
   OEM_FMRDS_CallBackCancel

Description:
   Cancels the previous enqued call back

Parameters:
   pcb : Pointer to the call back that has to be cancelled

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:

=============================================================================*/
static void OEM_FMRDS_CallBackCancel(AEECallback *pcb)
{
   OEMFMRDS *pMe = NULL;
   AEEFMRDSInfo nGroupId = 0;
   AEECallback **lfp = NULL, *pcbTry;
   AEEFMRDSDataStream nType = 0;
   uint32 nCBId  = 0;

#define OEMFMRDS_DECODE_CB_ID(group,type,cbid) { group=type=0; group = (cbid) >> 16; type = cbid & 0xFFFF;}

   if( NULL == pcb || NULL == pcb->pCancelData){
      return;
   }

   pMe = (OEMFMRDS *)(void *)pcb->pCancelData;
   nCBId = (uint32)(*(&(pcb->pReserved)));

   OEMFMRDS_DECODE_CB_ID(nGroupId,nType,nCBId);

   if( FMRDS_LAST_GROUPID <= nGroupId||NULL == pMe||AEEFMRDS_RAW_DATA < nType){
      return;
   }

   do{

      pcb->pfnCancel = NULL;

      if( AEEFMRDS_RAW_DATA == nType){

         if(NULL != pMe->m_pcbRawData){
            lfp = &pMe->m_pcbRawData;
         }
      }
      else if( AEEFMRDS_RAW_GROUPED_DATA == nType){

         if(NULL != pMe->m_pcbGroupedData[nGroupId]){
            lfp = &pMe->m_pcbGroupedData[nGroupId];
         }
      }
      else if( AEEFMRDS_PARSED_DATA == nType){

         if(NULL != pMe->m_pcbParsedData[nGroupId]){
            lfp = &pMe->m_pcbParsedData[nGroupId];
         }
      }

      if(NULL == lfp){
         break;
      }

      for ( ; pcb != (pcbTry = *lfp); lfp = &pcbTry->pNext);

      *lfp = pcb->pNext;

   }while(0);

   return;
}

/*=============================================================================
Function:
   OEMFMRDS_Notify

Description:
   Notifier for apps

Parameters:
   event : 
   nParm :

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=============================================================================*/
static void OEMFMRDS_Notify(int event,uint32 nParm)
{

   int Ctr = FMRDS_LAST_GROUPID-1;

   OEMFMRDS *pMe = (OEMFMRDS *)sOEMFMRDSPtr;
   AEECallback *pcb;

   switch(event){

      case FMRDS_EVT_NOTIFY_RDSDATA:

         switch(nParm){

            case AEEFMRDS_RAW_DATA:

               while( NULL != (pcb = pMe->m_pcbRawData) ){
                  pMe->m_pcbRawData = pcb->pNext;
                  pcb->pfnCancel = NULL;
                  AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
               }

               (void)OEMFMRDS_ParseGroupedData(pMe);

               break;

            case AEEFMRDS_RAW_GROUPED_DATA:
               
               for(Ctr = 0; Ctr < FMRDS_LAST_GROUPID; Ctr++){
#ifdef FMRDS_ON_ULC
                  if( FALSE != (nGroupsRcvd & (1 << Ctr)) ){
#else 
                  if ( NULL != pMe->m_RDSGroupedData[Ctr].m_pRDSData ){
#endif /* FMRDS_ON_ULC */
                     
                     while ( NULL != (pcb = pMe->m_pcbGroupedData[Ctr]) ) {
                        pMe->m_pcbGroupedData[Ctr] = pcb->pNext;
                        pcb->pfnCancel = 0;
                        AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
                     }
                  }
               }
               break;

            case AEEFMRDS_PARSED_DATA:

               //the groups that we are going to parse are the groups that
               //we have recieved.
               for(Ctr = 0; Ctr < FMRDS_LAST_GROUPID; Ctr++){
#ifdef FMRDS_ON_ULC
                  if( FALSE != (nGroupsRcvd & (1 << Ctr)) ){
#else 
                  if ( NULL != pMe->m_RDSGroupedData[Ctr].m_pRDSData ){
#endif /* FMRDS_ON_ULC */
                     while ( NULL != (pcb = pMe->m_pcbParsedData[Ctr]) ) {
                        pMe->m_pcbParsedData[Ctr] = pcb->pNext;
                        pcb->pfnCancel = 0;
                        AEE_ResumeCallback(pcb,(ACONTEXT *)pcb->pmc);
                     }
                  }
               }
               break;

            default:
               break;
         }

      default:
         break;
   }

   return;
}

/*=============================================================================
Function:
   OEMFMRDS_ProcessRDSData

Description:  
    Function to process the raw RDS data and form the individual blocks into
    groups so that they can be passed on to apps.

Parameters:
    fmrds_evnt    [in]: Event posted by FM-RDS layer.

Return Value: 
    AEE return value.

Comments:
    The current Philips ASIC chip gives only each block of a group separately.
    But the apps would except the entire group of data when they register for
    a group. So it's our responsibility to concatenate these different blocks
    into a group and notify the app about the recieved group. 

    When we receive data we may get some data corrupt, need to take care of 
    those blocks and form a complete group out of good blocks only by skipping
    all blocks which aren't in order.
  

Side Effects:

See Also:
=============================================================================*/
static int OEMFMRDS_ProcessRDSData(fmrds_event_msg_type  fmrds_event,
                                   OEMFMRDS              *pMe)
{
   int nResult = AEE_SUCCESS; 
   int Ctr = 0;
   int nCnt = 0; 
   tsFmrdsRawRdsBlockEvType rds_raw_data = {0,};
#ifdef FMRDS_ON_ULC
   uint32 nGroupedDataSeqNumUpdate = 0;
#endif /* FMRDS_ON_ULC */

   rds_raw_data = fmrds_event.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRawRdsBlockEv;

   // get the number of total groups. 
   nCnt = (int)rds_raw_data.usRawBlockBuffSize;

#ifdef FMRDS_ON_ULC
   MEMSET(gRawDataBuffer, 0, OEMFMRDS_MAX_BUF_SIZE * sizeof(AEEFMRDSGroup));
   pMe->m_RawDataBlocks.m_nBytesAllocated = 0;
   nGroupsRcvd = 0;

   //check if we have received more blocks than we can store
   if( OEMFMRDS_MAX_BUF_SIZE < (int)nCnt   ){
      DBGPRINTF_FATAL("Received more buffer, than we can store, skipping some");
      nCnt = OEMFMRDS_MAX_BUF_SIZE;;
   }
#else

   FREEIF(pMe->m_RawDataBlocks.m_pRDSData);
   pMe->m_RawDataBlocks.m_nBytesAllocated = 0;


   // nCnt holds the number of groups
   pMe->m_RawDataBlocks.m_pRDSData = MALLOC(OEMFMRDS_BLOCK_SIZE_IN_BYTES * nCnt * 4);
   if( NULL == pMe->m_RawDataBlocks.m_pRDSData ){
      return ENOMEMORY;
   }
#endif /* FMRDS_ON_ULC */

   for(Ctr = 0; Ctr < nCnt; Ctr++){

#ifdef FMRDS_ON_ULC
      int nGroupId = 0;
      nGroupId = OEMFMRDS_GROUPID(rds_raw_data.psRawBlockBuffPtr[Ctr].usBlockB);
         MEMCPY(gRawDataBuffer + pMe->m_RawDataBlocks.m_nBytesAllocated,
             (void *)(rds_raw_data.psRawBlockBuffPtr+Ctr),
             OEMFMRDS_BLOCK_SIZE_IN_BYTES * 4);

         //note the group id
         nGroupsRcvd |= 1 << nGroupId;
         pMe->m_RDSGroupedData[nGroupId].m_nBytesAllocated++;
         
         if(FALSE == (nGroupedDataSeqNumUpdate & (1 << nGroupId)) ){
            nGroupedDataSeqNumUpdate |= 1<< nGroupId;
            pMe->m_RDSGroupedData[nGroupId].m_nSeqNum++;
         }
#else
         MEMCPY(pMe->m_RawDataBlocks.m_pRDSData + pMe->m_RawDataBlocks.m_nBytesAllocated,
              (void *)(rds_raw_data.psRawBlockBuffPtr+Ctr),
             OEMFMRDS_BLOCK_SIZE_IN_BYTES * 4);
#endif /* FMRDS_ON_ULC */

         pMe->m_RawDataBlocks.m_nBytesAllocated++;
      }

   if( 0 != pMe->m_RawDataBlocks.m_nBytesAllocated ){
      pMe->m_RawDataBlocks.m_nSeqNum++;
   }

   OEMFMRDS_Notify(FMRDS_EVT_NOTIFY_RDSDATA, AEEFMRDS_RAW_DATA);

   return nResult;
}

/*=============================================================================
Function:
   OEMFMRDS_ParseGroupedData

Description:
   Parses for grouped data

Parameters:
   pMe : Pointer to the OEMFMRDS structure

Return Value:
   AEE return value

Comments:

Side Effects:

See Also:

=============================================================================*/
int OEMFMRDS_ParseGroupedData(OEMFMRDS *pMe)
{
   int nResult = AEE_SUCCESS;

#ifndef FMRDS_ON_ULC
   AEEFMRDSGroup *pNewGroup = NULL;
   AEEFMRDSGroup *pNewBlock = NULL;
   byte *pMarkerBlock = NULL;
   int nGroupId = 0;
   boolean bUpdateSeqNum = FALSE;

   if ( NULL == pMe ){
      return EBADPARM;
   }

   if( NULL == pMe->m_RawDataBlocks.m_pRDSData ){
      return nResult;
   }

   pMarkerBlock = (byte *)pMe->m_RawDataBlocks.m_pRDSData + 
      (pMe->m_RawDataBlocks.m_nBytesAllocated * sizeof(AEEFMRDSGroup));

   pNewBlock = pMe->m_RawDataBlocks.m_pRDSData;

    // Clear previously grouped RDS data.
   OEMFMRDS_ClearRDSData(pMe);

   while( (int)pNewBlock < (int)pMarkerBlock ){
            
      nGroupId = OEMFMRDS_GROUPID(pNewBlock->nBlockB);

      if ( NULL == pMe->m_RDSGroupedData[nGroupId].m_pRDSData){
         bUpdateSeqNum = TRUE;
      }

      pNewGroup = REALLOC(pMe->m_RDSGroupedData[nGroupId].m_pRDSData,
                          (sizeof(*pNewGroup) * pMe->m_RDSGroupedData[nGroupId].m_nBytesAllocated) +sizeof(*pNewGroup));
      if( NULL == pNewGroup ){
         nResult = ENOMEMORY;
         break;
      }

      MEMCPY(pNewGroup + pMe->m_RDSGroupedData[nGroupId].m_nBytesAllocated,
             pNewBlock,
             sizeof(*pNewGroup));

      pMe->m_RDSGroupedData[nGroupId].m_nBytesAllocated++;

      if( FALSE != bUpdateSeqNum ){
         pMe->m_RDSGroupedData[nGroupId].m_nSeqNum++;
         bUpdateSeqNum = FALSE;
      }

      pNewBlock++;

       pMe->m_RDSGroupedData[nGroupId].m_pRDSData = pNewGroup;
   }
#endif /* !defined FMRDS_ON_ULC */

   if( AEE_SUCCESS == nResult ){
      OEMFMRDS_Notify(FMRDS_EVT_NOTIFY_RDSDATA ,AEEFMRDS_RAW_GROUPED_DATA);
      OEMFMRDS_Notify(FMRDS_EVT_NOTIFY_RDSDATA ,AEEFMRDS_PARSED_DATA);
   }

   return nResult;
}
/*=============================================================================
FUNCTION: OEMFMRDS_ClearRDSData

DESCRIPTION:  
   Function to clear all the RDS data stored.

PARAMETERS:
   pMe   [in]: THE OEMFMRDS ptr

RETURN VALUE: 
   None.

COMMENTS:
   
SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
static void OEMFMRDS_ClearRDSData(OEMFMRDS *pMe)
{
   int Ctr  =0;

   if(NULL == pMe){
      return;
   }
   for( Ctr = 0; Ctr < FMRDS_LAST_GROUPID; Ctr++){
      FREEIF(pMe->m_RDSGroupedData[Ctr].m_pRDSData)
         pMe->m_RDSGroupedData[Ctr].m_nBytesAllocated = 0;
   }
}
#endif /* FEATURE_FM */

