/*===========================================================================
                    IMPORTANT NOTICE

FILE: OEMLogger.c

SERVICES: 
   OEMLogger interface implementation

DESCRIPTION
   This file contains the function definitions that are common
   to all OEMLogger implmentations


        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "OEMFeatures.h"
#ifdef FEATURE_LOGGER

#include "OEMLogger.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
//          Defines
//-----------------------------------------------------------------------------
// binary message header binary masks
#define MASK_BUCKET        0x07
#define MASK_FILE_EXIST    0x08
#define MASK_NUM_ARGS      0x30
#define MASK_RESERVED      0xC0
//-----------------------------------------------------------------------------
//          Type Declarations
//-----------------------------------------------------------------------------
typedef struct _OEMLogger    OEMLogger;
struct _OEMLogger{

   DECLARE_VTBL(ILogger)

   int32       m_nRefs;

   uint8       instanceID;
   uint32      bucketFilter[AEE_LOG_NUM_BUCKET_BLOCKS];

};
/*=============================================================================
                  FUNCTION DEFINITIONS
=============================================================================*/
/*=============================================================================
Function: OEMLogger_Printf
=============================================================================*/
int OEMLogger_Printf( ILogger *po, 
                      AEELogBucketType bucket, 
                      const char *pszFileName, 
                      uint16 nLineNum, 
                      const char *pszFormat, 
                      ... ){

   OEMLogger         *pMe        = (OEMLogger *)po;
   char              *pszFileNameLocal;
   AEELogRecord      logRecord;
   AEELogRcdHdrType  *pHdr;
   char              *buffer1    = NULL;
   char              *buffer2    = NULL;
   uint16            dataSize;
   va_list           vl;
   uint32            status;

   if( !( pMe->bucketFilter[ bucket / (uint32)32 ] & (uint32)( (uint32)1 << (bucket % (uint32)32) ) ) ){
      return(EUNSUPPORTED);
   }

   if( pszFormat == NULL ){
     return(EBADPARM);
   }

   if( pszFileName != NULL ){
   // remove file path from string
   if( STRRCHR( pszFileName, '\\' ) ){
      pszFileNameLocal = STRRCHR( pszFileName, '\\' )+1;
   }else{
      pszFileNameLocal = (char*)pszFileName;

      }
   }else{
      pszFileNameLocal = (char*)&pszFileName;
   }

   dataSize = STRLEN("%s %d %s") - 6 + STRLEN(pszFileNameLocal) + 10 + STRLEN(pszFormat);
   buffer1  = (char*)MALLOC( dataSize );
   if( buffer1 == NULL ){
      return(ENOMEMORY);
   }
   SPRINTF( buffer1, "%s %d %s", pszFileNameLocal, nLineNum, pszFormat );

   va_start(vl,pszFormat);
   dataSize = 1 + (uint16)AEE_vxprintf( NULL, 0, buffer1, vl );
   buffer2  = (char*)MALLOC( dataSize );
   if( buffer2 == NULL ){
      return(ENOMEMORY);
   }
   va_start(vl,pszFormat);
   dataSize = (uint16)AEE_vxprintf( buffer2, dataSize, buffer1, vl );
   /* vxprintf doesn't null-terminate on overflow */
   FREE(buffer1);
   va_end(vl);

   pHdr                 = (AEELogRcdHdrType*)&logRecord.hdr;
   pHdr->verHdr.version = AEE_LOG_VERSION;
   pHdr->verHdr.length  = dataSize + sizeof( AEELogRcdHdrType);
   pHdr->upTime         = GETUPTIMEMS();
   pHdr->classID        = AEE_GetAppContextCls(AEE_GetAppContext());
   pHdr->instanceID     = pMe->instanceID;
   pHdr->bucket         = bucket;
   pHdr->type           = (uint16)AEE_LOG_TYPE_TEXT;
   logRecord.data       = (uint8*)buffer2;

   status = ILOGGER_PutRecord( (ILogger*)pMe, bucket, &logRecord );

   FREE(buffer2);

   return(status);
}
/*=============================================================================
Function: OEMLogger_PutMsg()
=============================================================================*/
int OEMLogger_PutMsg( ILogger *po, 
                      AEELogBucketType bucket, 
                      const char *pszFileName, 
                      uint16 nLineNum, 
                      const char *pszMsg, 
                      uint8 nNumArgs, 
                      uint32 args[ MAX_LOG_TYPE_BIN_MSG_ARGS ] ){

   OEMLogger         *pMe        = (OEMLogger *)po;
   char              *pszFileNameLocal;
   AEELogRecord      logRecord;
   AEELogBinMsgType  binMsg;
   AEELogRcdHdrType  *pHdr;
   uint32            fnSize      = 0;
   uint32            strSize     = 0;

   if( !( pMe->bucketFilter[ bucket / (uint32)32 ] & (uint32)( (uint32)1 << (bucket % (uint32)32) ) ) ){
      return(EUNSUPPORTED);
   }

   if( pszMsg == NULL || nNumArgs > MAX_LOG_TYPE_BIN_MSG_ARGS ){
     return(EBADPARM);
   }

   binMsg.header  = ( (uint8)bucket & MASK_BUCKET );
   binMsg.line    = nLineNum;

   if( pszFileName != NULL ){

      binMsg.header |= MASK_FILE_EXIST;

      // remove file path from string
      if( STRRCHR( pszFileName, '\\' ) ){
         pszFileNameLocal = STRRCHR( pszFileName, '\\' )+1;
      }else{
         pszFileNameLocal = (char*)pszFileName;
      }

      fnSize = STRLEN(pszFileNameLocal)+1;
      if( fnSize > MAX_LOG_TYPE_BIN_MSG_FILENAME_SIZE ){
         fnSize = MAX_LOG_TYPE_BIN_MSG_FILENAME_SIZE-1;
         MEMCPY( (uint8*)binMsg.pszMsg, pszFileNameLocal, fnSize );
         *( binMsg.pszMsg + MAX_LOG_TYPE_BIN_MSG_FILENAME_SIZE-1 ) = NULL;
      }else{
         MEMCPY( (uint8*)binMsg.pszMsg, pszFileNameLocal, fnSize+1 );
      }
   }

   if( pszMsg != NULL ){
      strSize = STRLEN(pszMsg)+1;
      if( strSize >= MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE - fnSize ){
         MEMCPY( (uint8*)binMsg.pszMsg + fnSize, pszMsg, MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE - fnSize - 1 );
         *( binMsg.pszMsg + MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE-1 ) = NULL;
      }else{
         MEMCPY( (uint8*)binMsg.pszMsg + fnSize, pszMsg, strSize );
      }
   }

   if( nNumArgs ){
      if( nNumArgs > MAX_LOG_TYPE_BIN_MSG_ARGS){
         nNumArgs = 3;
      }
      binMsg.header |= ( nNumArgs << 4 );
      MEMCPY( (uint8*)binMsg.args, args, nNumArgs * sizeof(uint32) );
   }

   pHdr                 = (AEELogRcdHdrType*)&logRecord.hdr;
   pHdr->verHdr.version = AEE_LOG_VERSION;
   pHdr->verHdr.length  = sizeof(AEELogBinMsgType) + sizeof( AEELogRcdHdrType);
   pHdr->upTime         = GETUPTIMEMS();
   pHdr->classID        = AEE_GetAppContextCls(AEE_GetAppContext());
   pHdr->instanceID     = pMe->instanceID;
   pHdr->bucket         = bucket;
   pHdr->type           = (uint16)AEE_LOG_TYPE_BIN_MSG;
   logRecord.data       = (uint8*)&binMsg;

   return( ILOGGER_PutRecord( (ILogger*)pMe, bucket, &logRecord ) );
}
/*=============================================================================
Function: OEMLogger_PutItem()
=============================================================================*/
int OEMLogger_PutItem( ILogger *po, 
                       AEELogBucketType bucket, 
                       AEELogItemType type,
                       uint16 nSize,
                       const uint8 *pItem ){

   OEMLogger         *pMe  = (OEMLogger *)po;
   AEELogRecord      logRecord;
   AEELogRcdHdrType  *pHdr;

   if( !( pMe->bucketFilter[ bucket / (uint32)32 ] & (uint32)( (uint32)1 << (bucket % (uint32)32) ) ) ){
      return(EUNSUPPORTED);
   }

   if( pItem == NULL || nSize == 0 ){
     return(EBADPARM);
   }

   pHdr                          = (AEELogRcdHdrType*)&logRecord.hdr;
   pHdr->verHdr.version          = AEE_LOG_VERSION;
   pHdr->verHdr.length           = nSize + sizeof( AEELogRcdHdrType);
   pHdr->upTime                  = GETUPTIMEMS();
   pHdr->classID                 = AEE_GetAppContextCls(AEE_GetAppContext());
   pHdr->instanceID              = pMe->instanceID;
   pHdr->bucket                  = bucket;
   pHdr->type                    = type;
   logRecord.data                = (uint8*) pItem;   

   return( ILOGGER_PutRecord( (ILogger*)pMe, bucket, &logRecord ) );
}

#endif // FEATURE_LOGGER

// end OEMLogger.c
