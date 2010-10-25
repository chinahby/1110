/*=============================================================================

FILE:  AEELoggerTypes.h

SERVICES:  
   ILOGGER data types to share between the BREW Core, Developer, and PC

DESCRIPTION:
   This file provides defintions for the logging data types that need to be 
   shared between the phone and log parser

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

#ifndef AEELOGGERTYPES_H
#define AEELOGGERTYPES_H

#include "AEEComdef.h"

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
//            Defines
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//      Type Declarations
//-----------------------------------------------------------------------------
#ifndef AEECLSID
#define AEECLSID  uint32
#endif  // AEECLSID

#define AEE_LOG_VERSION                      2     // BREW log version
#define AEE_LOG_NUM_BUCKET_BLOCKS            8     // Maximum number of filter blocks in filter mask
#define MAX_LOG_TYPE_BIN_MSG_ARGS            3     // Max number of args allowed for
                                                   // AEE_LOG_TYPE_BIN_MSG
#define MAX_LOG_TYPE_BIN_MSG_FILENAME_SIZE   20
#define MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE       80    // Max length of ASCII text message
                                                   // which is the combination of a null 
                                                   // terminated filename followed by a 
                                                   // null terminated message

// BREW logging buckets 
typedef  uint8                      AEELogBucketType; 
#define  AEE_LOG_BUCKET_FIRST       0 
#define  AEE_LOG_BUCKET_LAST        255

// BREW logging item type
typedef  uint16                     AEELogItemType;
// predefined log item types
#define  AEE_LOG_TYPE_TEXT          0x000    // ASCII text message
#define  AEE_LOG_TYPE_BIN_MSG       0x001    // AEELogTypeBinMsg
#define  AEE_LOG_TYPE_BIN_BLK       0x002    // binary data block
// user defined log item type
#define  AEE_LOG_TYPE_USER_BASE     0x100           // User must define own type structure
                                                    // for each user type they use
#if defined( AEE_SIMULATOR ) | defined( _WIN32 )
#define PACKED
#define GCCPACKED
#pragma pack(push,1)
#endif

// BREW version header 
typedef PACKED struct{ 
   uint8            version;        // Log record header version (AEE_LOG_VERSION) 
   uint16           length;         // Length of this entire BREW log packet              
} GCCPACKED AEELogVerHdrType;

// BREW log record header
typedef PACKED struct{
   AEELogVerHdrType  verHdr;        // Version and packet length information 
   uint32            upTime;        // number of milliseconds since the devices was powered on as returned 
                                    // by the BREW helper function GETUPTIMEMS(). 
   AEECLSID             classID;    // Class ID of the currently running application
   uint8             instanceID;    // InstanceID is a developer defined ID number that can be used 
                                    // as the developer wishes.  Possible uses are to distinguish
                                    // between different application thread, or different application 
                                    // run-time states.
   AEELogBucketType  bucket;        // Logging bin number  (word8) 
   AEELogItemType    type;          // Log type (word 16) 
} GCCPACKED AEELogRcdHdrType;

// BREW log record
typedef PACKED struct{
   AEELogRcdHdrType     hdr;        // Log header
   uint8               *data;       // Log type's data
} GCCPACKED AEELogRecord;

// BREW binary log type structure definition
typedef PACKED struct{
   uint8                header;        // b7,b6    - bits reserved
                                       // b5,b4    - number of args
                                       // b3 bit   - file name present
                                    // b2,b1,b0 - bucket
   uint32               line;          // line number in application code where
                                       // this log item was sent
   uint32               args[   MAX_LOG_TYPE_BIN_MSG_ARGS  ];
   char                 pszMsg[ MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE ];
                                       // pszMsg contains two consecutive NULL
                                       // terminated strings.  The first if the
                                       // file name where the log message was
                                       // sent, and the second is an ASCII text
                                       // message
} GCCPACKED AEELogBinMsgType;       

#if defined( AEE_SIMULATOR) | defined( _WIN32 )
#pragma pack()
#endif

#endif    // AEELOGGERTYPES_H

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================

AEELogBucketType

Description:
   Each log item  is placed in one of 255 log buckets.  By assigning a log bucket
   to each log item the post parser is able to filter based bucket.  Log items may
   be separated into subsystems, log types, and so forth.

Definition:
   typedef  uint8                      AEELogBucketType; 
   #define  AEE_LOG_BUCKET_FIRST       0 
   #define  AEE_LOG_BUCKET_LAST        255
                                 
Comments:
   How these buckets are used is up to the application developer

See Also:
   None

===============================================================================

AEELogItemType

Description:
   These are the different log types that distinguish the structure of the data
   contained in the log item.  AEE_LOG_TYPE_USER_BASE and above are available for application
   developers to define their own log data formats.  The application developer
   is responsible for eliminating any conflicts that may arise due to multiple
   applications using the same log type.

Definition:
    typedef uint16 AEELogItemType;
    #define AEE_LOG_TYPE_TEXT 0x000 
    #define AEE_LOG_TYPE_BIN_MSG 0x001 
    #define AEE_LOG_TYPE_BIN_BLK 0x002 
    #define AEE_LOG_TYPE_USER_BASE 0x100

Comments:
    Predefined log item types include:
    AEE_LOG_TYPE_TEXT  :  ASCII text message
    AEE_LOG_TYPE_BIN_MSG  :  AEELogTypeBinMsg
    AEE_LOG_TYPE_BIN_BLK  :  binary data block
    A user defined log item type
    AEE_LOG_TYPE_USER_BASE  :  The user must define his or her own type structure for each user
type he or she uses.

See Also:
    None

===============================================================================
AEELogRecord

Description:
    All log records are passed down to the OEM layer using this structure.

Definition:
    typedef PACKED struct{
    AEELogRcdHdrType hdr; // Log header
    uint8 *data; // Log type's data
    } AEELogRecord;

Comments:
    hdr :  AEELogRcdHdrType Log header
    data :  Pointer to uint8 Log data
    
See Also:
    AEELogRcdHdrType

===============================================================================
AEELogRcdHdrType

Description:
    Standard BREW log header version 2
    This header is appended to the beginning of each outgoing
    log packet.

Definition:
    typedef PACKED struct{
    AEELogVerHdrType verHdr; 
    uint32 upTime; 
    AEECLSID classID;
    uint8 instanceID; 
    AEELogBucketType bucket; 
    AEELogItemType type; 
    } AEELogRcdHdrType;

Comments
    verHdr :  Version and packet length information of the BREW log record header
    upTime :  Number of milliseconds since the devices was powered on as returned
by the BREW helper function GETUPTIMEMS().
    classID :  Class ID of the currently running application 
    instanceID :  Developer-defined uint8 ID number that can be used as the developer
wishes. Possible uses are to distinguish between different application threads, or
different application runtime states.
    bucket :  AEELogBucketType Logging bin number (word8)
    type :  AEELogItemType Log type (word 16)

See Also:
    AEELogBucketType
    AEELogBucketType

===============================================================================

AEELogVerHdrType

Description:
   Starting with header version 2 and above this version header will always
   come first in the BREW log packet (this does not include any transport medium
   headers such as the serial packet headers).  This version header defines the
   version of the following BREW header (AEELogRcdHdrType) and the size of this entire
   BREW packet (not including transport medium headers).

Definition:
   // BREW version header 
   typedef PACKED struct{
      uint8            version;        // Log record header version (AEE_LOG_VERSION) 
      uint16           length;         // Length of this entire BREW log packet              
   } AEELogVerHdrType;
See Also:
   None

===============================================================================

AEELogBinMsgType

Description:
   This represents the structure of the log data when uisng brew log type
   AEE_LOG_TYPE_BIN_MSG as defined above.  ILOGGER_PutMsg() fills
   this structure using its function arguments.

Definition:
    typedef PACKED struct{
    uint8 header; 
    uint32 line; 
    uint32 args[ MAX_LOG_TYPE_BIN_MSG_ARGS ];
    char pszMsg[ MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE ];
    } AEELogBinMsgType;

Comments:
    header :  b7,b6 - bits reserved
     b5,b4 - number of args
     b3 bit - filename present
     b2,b1,b0 - log bucket
    line :  Line number in application code where this log item was sent
    args :  The maximum size of this element is MAX_LOG_TYPE_BIN_MSG_TEXT_SIZE.

    pszMsg :  pszMsg contains two consecutive NULL-terminated strings. The first
if the filename where the log message was sent, and the second is an ASCII text
message. 

See Also:
    None

===============================================================================*/

// end AEELoggerTypes.h
