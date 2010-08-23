/*=============================================================================
                    IMPORTANT NOTICE

FILE: OEMLoggerFile.c

SERVICES: 
   OEMLoggerFile OEM interfaces implementation

DESCRIPTION
   This file contains the implementation of the functions in OEMLoggerFile.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"
#ifdef FEATURE_LOGGER_FILE

#include "OEMLogger.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "AEEFile.h"

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
//            Defines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//      Type Declarations
//-----------------------------------------------------------------------------
typedef struct _IOEMLoggerFile    IOEMLoggerFile;
struct _IOEMLoggerFile{

   DECLARE_VTBL(ILogger)

   int32       m_nRefs;

   uint8       instanceID;
   uint32      bucketFilter[AEE_LOG_NUM_BUCKET_BLOCKS];
   uint32      numSent;
   uint32      numDropped;

   IFileMgr    *m_pIFileMgr;
   IFile       *m_pIFile;
   char        *pszFilePath;
   uint32      fileSizeCap;
   uint32      fileSizeCur;

};

//-----------------------------------------------------------------------------
//          Function Prototypes
//-----------------------------------------------------------------------------
// intrface functions
static uint32  OEMLoggerFile_AddRef(    ILogger *po );
static uint32  OEMLoggerFile_Release(   ILogger *po );
static int     OEMLoggerFile_SetParam(  ILogger *po, AEELogParamType pType, uint32 param, const void *pParam );
static int     OEMLoggerFile_GetParam(  ILogger *po, AEELogParamType pType, const void *pParam );
static int     OEMLoggerFile_PutRecord( ILogger *po, AEELogBucketType bucket, AEELogRecord *pRcd );

//-----------------------------------------------------------------------------
//          Static variables
//-----------------------------------------------------------------------------
static const VTBL(ILogger) gsOEMLoggerFileFunc = { OEMLoggerFile_AddRef,
                                                   OEMLoggerFile_Release,
                                                   OEMLoggerFile_SetParam,
                                                   OEMLoggerFile_GetParam,
                                                   OEMLoggerFile_PutRecord,
                                                   OEMLogger_Printf,
                                                   OEMLogger_PutMsg,
                                                   OEMLogger_PutItem
};
/*=============================================================================
                  FUNCTION DEFINITIONS
=============================================================================*/
/*=============================================================================
Function:  OEMLoggerFile_New()
=============================================================================*/
int OEMLoggerFile_New( IShell * pShell, AEECLSID cls, void **ppif ){

   IOEMLoggerFile  *pMe = NULL;
   uint32         status;
   uint32         i;

   if(!pShell){
      return(EBADPARM);
   }

   if((pMe = (IOEMLoggerFile *)AEE_NewClass((IBaseVtbl *)&gsOEMLoggerFileFunc, sizeof(IOEMLoggerFile))) == NULL){
      return(ENOMEMORY);
   }

   pMe->m_pIFileMgr  = NULL;
   pMe->m_pIFile     = NULL;

   if( (status = ISHELL_CreateInstance( pShell, AEECLSID_FILEMGR, (void**)&pMe->m_pIFileMgr ) ) != SUCCESS ){
      OEMLoggerFile_Release( (ILogger*)pMe );
      return( status );
   }
   
   // setup private data
   pMe->instanceID   = 0;   
   pMe->m_nRefs      = 1;
   pMe->numSent      = 0;
   pMe->numDropped   = 0;
   pMe->fileSizeCap  = IFILEMGR_GetFreeSpace( pMe->m_pIFileMgr, NULL ) / 2;
   pMe->fileSizeCur  = 0;
   *ppif = pMe;
   // clear bucket filters
   for( i= 0; i<AEE_LOG_NUM_BUCKET_BLOCKS; i++ ){
      pMe->bucketFilter[i] = 0;
   }

   //DBGPRINTF( "Free Flash: %d", pMe->fileSizeCap * 2 );

   return(SUCCESS);

}
/*=============================================================================
Function:  OEMLoggerFile_AddRef()
=============================================================================*/
static uint32 OEMLoggerFile_AddRef( ILogger *po ){

   IOEMLoggerFile *pMe = (IOEMLoggerFile*)po;

   return(++pMe->m_nRefs);
}
/*============================================================================ 
Function: OEMLoggerFile_Release()
=============================================================================*/
static uint32 OEMLoggerFile_Release( ILogger *po ){

   IOEMLoggerFile *pMe = (IOEMLoggerFile*)po;

   if( --pMe->m_nRefs > 0){
         return( pMe->m_nRefs );
   }else{
      pMe->m_nRefs = 0;
   }

   // free all resources
   if( pMe->m_pIFile ){
      IFILE_Release( pMe->m_pIFile );
   }
   if( pMe->m_pIFileMgr ){
      IFILEMGR_Release( pMe->m_pIFileMgr );
   }
   FREE(pMe);
   return(0);
}
/*============================================================================ 
Function: OEMLoggerFile_SetParam()
=============================================================================*/
int OEMLoggerFile_SetParam( ILogger *po, AEELogParamType pType, uint32 param, const void *pParam ){

   IOEMLoggerFile  *pMe = (IOEMLoggerFile*)po;
   uint32         status;

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

   case AEE_LOG_PARAM_FILE_FLUSH:
      if( !pMe->m_pIFile ){
         return(EFILENOEXISTS);
      }
      status = IFILE_Truncate( pMe->m_pIFile, 0 );
      if( status == SUCCESS){
         pMe->fileSizeCur = 0;
      }
      return(status);

   case AEE_LOG_PARAM_FILE_MAX_SIZE:
      pMe->fileSizeCap = param;
      break;

   case AEE_LOG_PARAM_FILE_NEW:

      if( param != _OFM_READWRITE && param != _OFM_APPEND ){
         return(EUNSUPPORTED);
      }

      // close the old file if one exists
      if( pMe->m_pIFile ){
         if( IFILE_Release( pMe->m_pIFile ) != 0 ){
            return(EFILEEXISTS);
         }
         pMe->fileSizeCur = 0;
      }

      // now open the new file
      pMe->m_pIFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr, (const char*)pParam, (AEEOpenFileMode)param );
      if( !pMe->m_pIFile ){
         // file not found to open, create a new one
         pMe->m_pIFile = IFILEMGR_OpenFile( pMe->m_pIFileMgr, (const char*)pParam, _OFM_CREATE );
         if( !pMe->m_pIFile ){
            return(EFILENOEXISTS);
         }
      }
      
      if( (AEEOpenFileMode)param == _OFM_READWRITE ){
         IFILE_Truncate( pMe->m_pIFile, 0 );
      }
      return(SUCCESS);

   default:
      return(EUNSUPPORTED);
   }
   return(SUCCESS);

}
/*============================================================================ 
Function: OEMLoggerFile_GetParam()
=============================================================================*/
int OEMLoggerFile_GetParam( ILogger *po, AEELogParamType pType, const void *pParam ){

   IOEMLoggerFile *pMe = (IOEMLoggerFile*)po;

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

   case AEE_LOG_PARAM_FILE_MAX_SIZE:
      *((uint32*)pParam) = pMe->fileSizeCap;
      break;

   case AEE_LOG_PARAM_FILE_FREE_SPACE:
      *((uint32*)pParam) = ( pMe->fileSizeCap > pMe->fileSizeCur ) ? pMe->fileSizeCap - pMe->fileSizeCur : 0;
      break;

   default:
      return(EUNSUPPORTED);
   }
   return(SUCCESS);

}
/*============================================================================ 
Function: OEMLoggerFile_PutRecord()
=============================================================================*/
int OEMLoggerFile_PutRecord( ILogger *po, 
                             AEELogBucketType bucket, 
                             AEELogRecord *pRcd ){

   IOEMLoggerFile     *pMe     = (IOEMLoggerFile*)po;
   uint32            status;

   if( !pRcd ){
      return(EBADPARM);  
   }

   if( !pMe->m_pIFile ){
      return(EFILENOEXISTS);
   }

   // assume we will drop this until we know for sure
   pMe->numDropped++;

   if( pMe->fileSizeCur + pRcd->hdr.verHdr.length >= pMe->fileSizeCap ){
      return(EFSFULL);
   }


   pMe->fileSizeCur += pRcd->hdr.verHdr.length;
   if( ( status = IFILE_Write( pMe->m_pIFile, (const void *)pRcd, sizeof(AEELogRcdHdrType) ) ) != sizeof(AEELogRcdHdrType) ){
      IFILE_Seek( pMe->m_pIFile, _SEEK_CURRENT, -1 * (int32)status );
      return(EFAILED);
   }


   if( ( status = IFILE_Write( pMe->m_pIFile, (const void *)pRcd->data, pRcd->hdr.verHdr.length - sizeof(AEELogRcdHdrType) ) ) != pRcd->hdr.verHdr.length - sizeof(AEELogRcdHdrType) ){
      IFILE_Seek( pMe->m_pIFile, _SEEK_CURRENT, -1 * (int32)(status + sizeof(AEELogRcdHdrType))  );
      return(EFAILED);
   }

   // we know we didn't drop it so correct our earlier assumption
   pMe->numDropped--;
   pMe->numSent++;
   return(SUCCESS);
}

#endif   //FEATURE_LOGGER_FILE

// end OEMLoggerFile.c

