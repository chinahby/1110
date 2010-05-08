/*=============================================================================
                    IMPORTANT NOTICE

FILE: OEMLoggerBTIL.c

SERVICES: 
   OEMLoggerBTIL OEM interfaces implementation

DESCRIPTION
   This file contains the implementation of the functions in OEMLoggerBTIL.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_LOGGER_BTIL
#include "OEMLogger.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEFIFO.h"
#include "OEMLoggerBTILMsgs.h"

#ifndef MAX_INT32
#define MAX_INT32 0x7fffffff
#endif

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
//            Defines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//      Type Declarations
//-----------------------------------------------------------------------------
typedef struct _IOEMLoggerBTIL    IOEMLoggerBTIL;
struct _IOEMLoggerBTIL{

   DECLARE_VTBL(ILogger)

   int32       m_nRefs;

   uint8       instanceID;
   uint32      bucketFilter[AEE_LOG_NUM_BUCKET_BLOCKS];
   uint32      numSent;
   uint32      numDropped;

   IFIFO      *pFIFO;
};

//-----------------------------------------------------------------------------
//          Function Prototypes
//-----------------------------------------------------------------------------
// intrface functions
static uint32  OEMLoggerBTIL_AddRef(    ILogger *po );
static uint32  OEMLoggerBTIL_Release(   ILogger *po );
static int     OEMLoggerBTIL_SetParam(  ILogger *po, AEELogParamType pType, uint32 param, const void *pParam );
static int     OEMLoggerBTIL_GetParam(  ILogger *po, AEELogParamType pType, const void *pParam );
static int     OEMLoggerBTIL_PutRecord( ILogger *po, AEELogBucketType bucket, AEELogRecord *pRcd );

static uint32  CalcFIFOFreeSpace( IOEMLoggerBTIL *pMe );

//-----------------------------------------------------------------------------
//          Static variables
//-----------------------------------------------------------------------------
static const VTBL(ILogger) gsOEMLoggerBTILFunc = { OEMLoggerBTIL_AddRef,
                                                   OEMLoggerBTIL_Release,
                                                   OEMLoggerBTIL_SetParam,
                                                   OEMLoggerBTIL_GetParam,
                                                   OEMLoggerBTIL_PutRecord,
                                                   OEMLogger_Printf,
                                                   OEMLogger_PutMsg,
                                                   OEMLogger_PutItem
};
/*=============================================================================
                  FUNCTION DEFINITIONS
=============================================================================*/
/*=============================================================================
Function:  OEMLoggerBTIL_New()
=============================================================================*/
int OEMLoggerBTIL_New( IShell * pShell, AEECLSID cls, void **ppif )
{

   IOEMLoggerBTIL  *pMe = NULL;
   int            nErr;
   uint32         i;

   if(!pShell)
   {
      return(EBADPARM);
   }

   // Get our structure
   if((pMe = (IOEMLoggerBTIL*)AEE_NewClass((IBaseVtbl*)&gsOEMLoggerBTILFunc,
                                           sizeof(IOEMLoggerBTIL))) == NULL)
   {
      return(ENOMEMORY);
   }

   // setup private data
   pMe->instanceID   = 0;   
   pMe->m_nRefs      = 1;
   pMe->numSent      = 0;
   pMe->numDropped   = 0;

   // Open the FIFO to the BTIL process
   nErr = ISHELL_CreateInstance(pShell, AEECLSID_FIFO, (void**)&pMe->pFIFO);
   if (nErr != SUCCESS || pMe->pFIFO == NULL)
   {
      FREE(pMe);
      return (EFAILED);
   }
   nErr = IFIFO_OpenEx(pMe->pFIFO, BTIL_LOGGER_FIFO_NAME, AEEFIFO_MODE_W);
   if (nErr != SUCCESS)
   {
      IFIFO_Release(pMe->pFIFO);
      FREE(pMe);
      return (EFAILED);
   }
   IFIFO_SetBufSize(pMe->pFIFO, BTIL_LOGGER_FIFO_SIZE);

   *ppif = pMe;
   // clear bucket filters
   for( i= 0; i<AEE_LOG_NUM_BUCKET_BLOCKS; i++ ){
      pMe->bucketFilter[i] = 0;
   }

   return(SUCCESS);

}
/*=============================================================================
Function:  OEMLoggerBTIL_AddRef()
=============================================================================*/
static uint32 OEMLoggerBTIL_AddRef( ILogger *po ){

   IOEMLoggerBTIL *pMe = (IOEMLoggerBTIL*)po;

   return(++pMe->m_nRefs);
}
/*============================================================================ 
Function: OEMLoggerBTIL_Release()
=============================================================================*/
static uint32 OEMLoggerBTIL_Release( ILogger *po ){

   IOEMLoggerBTIL *pMe = (IOEMLoggerBTIL*)po;

   if( --pMe->m_nRefs > 0){
         return( pMe->m_nRefs );
   }else{
      pMe->m_nRefs = 0;
   }

   // free all resources
   if( pMe->pFIFO ){
      IFIFO_Release(pMe->pFIFO);
   }
   FREE(pMe);
   return(0);
}
/*============================================================================ 
Function: OEMLoggerBTIL_SetParam()
=============================================================================*/
int OEMLoggerBTIL_SetParam( ILogger *po, AEELogParamType pType, uint32 param, const void *pParam ){

   IOEMLoggerBTIL  *pMe = (IOEMLoggerBTIL*)po;

   switch( pType ){

   case AEE_LOG_PARAM_INSTANCE_ID:
      pMe->instanceID = (uint8)param;
      break;

   case AEE_LOG_PARAM_FILTER_ONE:
      if( param <= AEE_LOG_BUCKET_LAST && (uint32)pParam <= 1 ){
         // determine bit position and set log mask
         pMe->bucketFilter[ param / (uint32)32 ] &= ~(uint32)( 1 << (param % (uint32)32) );
         pMe->bucketFilter[ param / (uint32)32 ] |= (uint32)( (uint32)pParam << (param % (uint32)32) );
      }else{
         return(EBADPARM);
      }
      break;

   case AEE_LOG_PARAM_FILTER_BLOCK:
      if( param < AEE_LOG_NUM_BUCKET_BLOCKS ){
         // set log mask
         pMe->bucketFilter[ (uint32)param ] = (uint32)pParam;
      }else{
         return(EBADPARM);
      }
      break;
      
   case AEE_LOG_PARAM_SENT:
      pMe->numSent = param;
      break;

   case AEE_LOG_PARAM_DROPPED:
      pMe->numDropped = param;
      break;

   default:
      return(EUNSUPPORTED);
   }
   return(SUCCESS);

}
/*============================================================================ 
Function: OEMLoggerBTIL_GetParam()
=============================================================================*/
int OEMLoggerBTIL_GetParam( ILogger *po, AEELogParamType pType, const void *pParam ){

   IOEMLoggerBTIL *pMe = (IOEMLoggerBTIL*)po;

   if( pParam == NULL ){
      return(EBADPARM);
   }   
   
   switch( pType ){

   case AEE_LOG_PARAM_INSTANCE_ID:
      *((uint8*)pParam) = pMe->instanceID;
      break;
   case AEE_LOG_PARAM_FILTER_BLOCK:
      if( *((uint32*)pParam) < AEE_LOG_NUM_BUCKET_BLOCKS ){
         // set log mask
         *((uint32*)pParam)         = pMe->bucketFilter[ *((uint32*)pParam) ];
      }else{
         return(EBADPARM);
      }
      break;      
      
   case AEE_LOG_PARAM_SENT:
      *((uint32*)pParam) = pMe->numSent;
      break;

   case AEE_LOG_PARAM_DROPPED:
      *((uint32*)pParam) = pMe->numDropped;
      break;

   default:
      return(EUNSUPPORTED);
   }
   return(SUCCESS);

}
/*============================================================================ 
Function: OEMLoggerBTIL_PutRecord()
=============================================================================*/
int OEMLoggerBTIL_PutRecord( ILogger *po, 
                             AEELogBucketType bucket, 
                             AEELogRecord *pRcd ){

   IOEMLoggerBTIL     *pMe     = (IOEMLoggerBTIL*)po;
   uint32            status;
   uint32            dwPayload;

   if( !pRcd ){
      return(EBADPARM);  
   }

   if( !pMe->pFIFO ){
      return(EFILENOEXISTS);
   }

   // assume we will drop this until we know for sure
   pMe->numDropped++;

   // Write the message if we have enough space
   status = CalcFIFOFreeSpace(pMe);
   if (status >= pRcd->hdr.verHdr.length)
   {
      status = IFIFO_Write(pMe->pFIFO, (char*)&pRcd->hdr, sizeof(AEELogRcdHdrType));
      if (status != sizeof(AEELogRcdHdrType))
      {
         // Clear away the FIFO
         IFIFO_Flush(pMe->pFIFO, MAX_INT32);
         return EFAILED;
      }
      dwPayload = pRcd->hdr.verHdr.length - sizeof(AEELogRcdHdrType);
      status = IFIFO_Write(pMe->pFIFO, (char*)pRcd->data, dwPayload);
      if (status != dwPayload)
      {
         // Clear away the FIFO
         IFIFO_Flush(pMe->pFIFO, MAX_INT32);
         return EFAILED;
      }
      // we know we didn't drop it so correct our earlier assumption
      pMe->numDropped--;
      pMe->numSent++;
   }

   return(SUCCESS);
}

static uint32 CalcFIFOFreeSpace( IOEMLoggerBTIL *pMe )
{
   int nErr;
   uint32 dwBufSize;
   uint32 dwBufUsed;

   nErr = IFIFO_GetBufSize(pMe->pFIFO, &dwBufSize);
   if (nErr == SUCCESS)
   {
      nErr = IFIFO_GetBufUsed(pMe->pFIFO, &dwBufUsed);
   }

   if (nErr != SUCCESS || dwBufUsed > dwBufSize)
      return 0;
   return dwBufSize - dwBufUsed;
}

#endif  // FEATURE_LOGGER_BTIL

// end OEMLoggerBTIL.c

