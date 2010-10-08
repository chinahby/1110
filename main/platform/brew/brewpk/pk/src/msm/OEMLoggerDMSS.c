/*=============================================================================
                    IMPORTANT NOTICE

               DMSS LICENSEE DISTRIBUTION ONLY
                  OEM DISTRIBUTION ONLY

FILE: OEMLoggerDMSS.c

SERVICES: 
   OEMLogger DMSS specific interface implementation

DESCRIPTION
   This file contains the DMSS implementation of OEMLogger

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#if defined(FEATURE_DIAG_V7_LOG_IFACE)
#error Diag version 7 and below not supported (must support log_commit() )
#endif

#include "OEMFeatures.h"
#ifdef FEATURE_LOGGER_DMSS

#ifdef AEE_SIMULATOR
#define TG  T_G
#define TARG_H "target.h"
#endif

#include <stdlib.h>
#include "log.h"
#include "AEE.h"
#include "AEELoggerTypes.h"
#include "OEMLogger.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
//      Defines
//-----------------------------------------------------------------------------
#define MAX_BUCKET   ((LOG_BREW_LAST_C - LOG_BREW_RESERVED_CODES_BASE_C) - 1 )

//-----------------------------------------------------------------------------
//      Type Declarations
//-----------------------------------------------------------------------------

typedef struct {
   log_hdr_type      hdr;
   AEELogRcdHdrType  logRcdHdr;
   // data immediately follows header here
} oemLogType;

// OEM logger interface private data
typedef struct _IOEMLoggerDMSS IOEMLoggerDMSS;
struct _IOEMLoggerDMSS{

   DECLARE_VTBL(ILogger)

   uint32      nRefs;

   uint8       instanceID;
   uint32      bucketFilter[AEE_LOG_NUM_BUCKET_BLOCKS];
   uint32      numSent;
   uint32      numDropped;

};

//-----------------------------------------------------------------------------
//            Function Prototypes
//-----------------------------------------------------------------------------

// interface functions
static uint32  OEMLoggerDMSS_AddRef(    ILogger *po );
static uint32  OEMLoggerDMSS_Release(   ILogger *po );
static int     OEMLoggerDMSS_SetParam(  ILogger *po, AEELogParamType pType, uint32 param, const void *pParam );
static int     OEMLoggerDMSS_GetParam(  ILogger *po, AEELogParamType pType, const void *pParam );
static int     OEMLoggerDMSS_PutRecord( ILogger *po, AEELogBucketType bucket, AEELogRecord *pRcd );

// disable "Pointer parameter 'x' could be declared as pointing to const"
// for those functions which are called via the public vtbl.
//lint -efunc(818, OEMLoggerDMSS_New,OEMLoggerDMSS_PutRecord)
//-----------------------------------------------------------------------------
//          Static variable Definitions
//-----------------------------------------------------------------------------

static const VTBL(ILogger) gsOEMLoggerDMSSFunc = { OEMLoggerDMSS_AddRef,
                                                   OEMLoggerDMSS_Release,
                                                   OEMLoggerDMSS_SetParam,
                                                   OEMLoggerDMSS_GetParam,
                                                   OEMLoggerDMSS_PutRecord,
                                                   OEMLogger_Printf,
                                                   OEMLogger_PutMsg,
                                                   OEMLogger_PutItem
};
/*=============================================================================
                  FUNCTION DEFINITIONS
=============================================================================*/
/*=============================================================================
Function:  OEMLoggerDMSS_New()
=============================================================================*/
int OEMLoggerDMSS_New( IShell *pShell, AEECLSID cls, void **ppif ){

   IOEMLoggerDMSS *pMe = NULL;
   uint32         i;

   (void)cls; /* unused parameter*/

   if(!pShell){
      return(EBADPARM);
   }

   pMe = (IOEMLoggerDMSS*)(void*)AEE_OEM_NEWCLASS((IBaseVtbl *)(void*)&gsOEMLoggerDMSSFunc, sizeof(IOEMLoggerDMSS));

   if(NULL == pMe){
      return(ENOMEMORY);
   }

   // setup private data
   pMe->instanceID   = 0;
   pMe->nRefs        = 1;
   pMe->numSent      = 0;
   pMe->numDropped   = 0;
   *ppif = pMe;
   // clear bucket filters
   for( i= 0; i<AEE_LOG_NUM_BUCKET_BLOCKS; i++ ){
      pMe->bucketFilter[i] = 0;
   }

   return(SUCCESS);
}
/*=============================================================================
Function:  OEMLoggerDMSS_AddRef()
=============================================================================*/
static uint32 OEMLoggerDMSS_AddRef( ILogger *po ){

   IOEMLoggerDMSS *pMe = (IOEMLoggerDMSS*)po;

   return( ++pMe->nRefs );
}
/*=============================================================================
Function: OEMLoggerDMSS_Release()
=============================================================================*/
static uint32 OEMLoggerDMSS_Release( ILogger *po ){

   IOEMLoggerDMSS *pMe = (IOEMLoggerDMSS*)po;

   if( --pMe->nRefs > 0){
         return( pMe->nRefs );
   }else{
      pMe->nRefs = 0;
   }

   FREE(pMe);

   //DBGPRINTF( "freeing object" );
   return(0);
}
/*=============================================================================
Function: OEMLoggerDMSS_SetParam()
=============================================================================*/
static int OEMLoggerDMSS_SetParam( ILogger *po, AEELogParamType pType, uint32 param, const void *pParam ){

   IOEMLoggerDMSS *pMe = (IOEMLoggerDMSS*)po;

   switch(pType){

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
/*=============================================================================
Function: OEMLoggerDMSS_GetParam()
=============================================================================*/
static int OEMLoggerDMSS_GetParam( ILogger *po, AEELogParamType pType, const void *pParam ){

   IOEMLoggerDMSS *pMe = (IOEMLoggerDMSS*)po;

   if( pParam == NULL ){
      return(EBADPARM);
   }   

   switch(pType){

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

/*=============================================================================
Function: OEMLoggerDMSS_Commit()
=============================================================================*/
int OEMLoggerDMSS_Commit( uint32 hdrCode, AEELogRcdHdrType const *pHeader, uint8 const *pData, unsigned int length ){

  oemLogType   *pLog     = NULL;
  uint8        *pLogData = NULL;
  uint32       logSize;

   // check to see if this log code is enabled
   if( !log_status( (word)hdrCode ) ){
      return(EUNSUPPORTED);
   }

   // allocate memory for this log item
   logSize = sizeof(oemLogType) + length;
   if( ( pLog = (oemLogType*)log_alloc_ex( (word)hdrCode, logSize ) ) == NULL ){
      return(ENOMEMORY);
   }

   pLogData = ((uint8*)pLog) + sizeof(oemLogType);

   // fill memory
   (void) MEMMOVE( (uint8*)&pLog->logRcdHdr, (uint8*)pHeader, sizeof(AEELogRcdHdrType) );
   (void) MEMMOVE( pLogData, pData, length );

   // send packet
   log_commit( pLog );

   return(SUCCESS);
}

/*=============================================================================
Function: OEMLoggerDMSS_PutRecord()
=============================================================================*/
static int OEMLoggerDMSS_PutRecord( ILogger *po, 
                                    AEELogBucketType bucket, 
                                    AEELogRecord *pRcd ){

   IOEMLoggerDMSS *pMe = (IOEMLoggerDMSS*)po;
   uint32         hdrCode;
   int            nErr;
 
   if( pRcd == NULL ){
      return(EBADPARM);  
   }

   if( pRcd->data == NULL ){
      return(EBADPARM);
   }

   // assume we will drop until we have actually sent log
   pMe->numDropped++;

   // first 31 buckets are assigned to a corresponding BREW log code
   // buckets 32-255 are all assigned to the last BREW log code
   if( bucket < MAX_BUCKET ){
      hdrCode = LOG_BREW_RESERVED_CODES_BASE_C + bucket;
   }else{
      hdrCode = LOG_BREW_LAST_C;
   }

   // allocate memory with diag and send packet
   nErr = OEMLoggerDMSS_Commit( hdrCode, &pRcd->hdr, pRcd->data, pRcd->hdr.verHdr.length - sizeof(AEELogRcdHdrType) );
   if( nErr != SUCCESS ){
      return(nErr);
   }
   
   pMe->numDropped--;
   pMe->numSent++;

   return(SUCCESS);
}

#endif   //FEATURE_LOGGER_DMSS

// end OEMLoggerDMSS.c

