/*=============================================================================

FILE:  AEELogger.h

SERVICES:  
   AEE Logger General Interface

GENERAL DESCRIPTION:
   Definitions, typedefs, etc. for ILogger Interface

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#ifndef AEELOGGER_H
#define AEELOGGER_H

#include "AEE.h"
#include "AEELoggerTypes.h"

//=============================================================================
//  Logger Instances not declared in AEECLSIDs.h
//=============================================================================
#define AEECLSID_LOGGER_BTIL    	0x101e6b8

//=============================================================================
//  VARIABLE AND TYPE DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
//      Type Declarations
//-----------------------------------------------------------------------------
typedef  uint16                        AEELogParamType;
#define  AEE_LOG_PARAM_INSTANCE_ID     0x001 // InstanceID is a developer defined ID number 
                                             // that can be used as the developer wishes.  
                                             // Possible uses are to distinguish between 
                                             // different application thread, or different 
                                             // application run-time states. Default instance
                                             // ID is zero
#define  AEE_LOG_PARAM_FILTER_ONE      0x002 // Filter a single log bucket
#define  AEE_LOG_PARAM_FILTER_BLOCK    0x003 // sets filters for a block of 32 buckets
#define  AEE_LOG_PARAM_SENT            0x004 // Number of packets sent by an instance of ILogger
#define  AEE_LOG_PARAM_DROPPED         0x005 // Number of packets dropped by an instance of ILogger
                                             // Dropeed packets do not include packets that have
                                             // been filtered.
#define  AEE_LOG_PARAM_FILE_FLUSH      0x006 // Flushes the current log file and resets file pointer
                                             // to beginning of file.  May only be used 
                                             // with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_FILE_MAX_SIZE   0x007 // Sets the maximum size the log file may become
                                             // Defaults to 1/2 the free EFS space.
                                             // May only be used with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_FILE_FREE_SPACE 0x008 // Amount of free space left in the log file
                                             // before it reaches the maximum size set with
                                             // the AEE_LOG_PARAM_FILE_MAX_SIZE parameter
                                             // May only be used with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_FILE_NEW        0x009 // Closes the current log file and creates 
                                             // a new one using the specified name
                                             // May only be used with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_SIO_PORT_CONFIG 0x00A // Gets and sets the serial port configuration
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_SIO_OPEN_PORT   0x00B // Opens the serial port for logging, this may
                                             // or may not open the port immediatly, the serial 
                                             // port status is returned by ILOGGER_SetParam() and
                                             // if the port open is in progress the caller must
                                             // check the port status at a later time using
                                             // AEE_LOG_PARAM_SIO_PORT_STATUS or providing
                                             // a callback
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_SIO_CLOSE_PORT  0x00C // Closes the serial port used for logging, this may
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_SIO_PORT_STATUS 0x00D // Returns the status of the serial port
                                             // May only be used with AEECLSID_LOGGER_SERIAL 
#define  AEE_LOG_PARAM_SIO_SET_TRIG    0x00E // Sets serial input/output triggers
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_LAST            0x100 // This must be the last defined param type
                                             // OEM specific parameters start at this value, 
                                             // See the specific implementation header files 
                                             // for more information.

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------
#define LOG_TEXT(    inst, bkt, str ) ILOGGER_PutItem( inst, (AEELogBucketType)bkt, AEE_LOG_TYPE_TEXT, (uint16)( (char*)str == NULL ? 0 : ( STRLEN(str) + 1 ) ), (const uint8*)str )
#define LOG_TEXT_EX( inst, bkt, str ) ILOGGER_Printf( inst, ( inst, (AEELogBucketType)bkt, __FILE__, (uint16)__LINE__, str ) )

//=============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//=============================================================================
//-----------------------------------------------------------------------------
// Interface Definitions
//-----------------------------------------------------------------------------
typedef struct _ILogger    ILogger;

QINTERFACE(ILogger){

   DECLARE_IBASE(ILogger)

   int   (*SetParam)    ( ILogger *po, AEELogParamType pType, uint32 param, const void *pParam );
   int   (*GetParam)    ( ILogger *po, AEELogParamType pType, const void *pParam );
   int   (*PutRecord)   ( ILogger *po, AEELogBucketType bucket, AEELogRecord *pRcd );
   int   (*Printf)      ( ILogger *po, AEELogBucketType bucket, const char *pszFileName, uint16 nLineNum, const char *pszFormat, ... );
   int   (*PutMsg)      ( ILogger *po, AEELogBucketType bucket, const char *pszFileName, uint16 nLineNum, const char *pszMsg, uint8 nNumArgs, uint32 args[ MAX_LOG_TYPE_BIN_MSG_ARGS ] );
   int   (*PutItem)     ( ILogger *po, AEELogBucketType bucket, AEELogItemType type, uint16 nSize, const uint8 *pItem );

};

// Access Macros for ILogger Interface
#define ILOGGER_AddRef(p)                    GET_PVTBL(p,ILogger)->AddRef(p)                 
#define ILOGGER_Release(p)                   GET_PVTBL(p,ILogger)->Release(p) 
               
#define ILOGGER_SetParam(p,typ,prm,pprm)     GET_PVTBL(p,ILogger)->SetParam(p,typ,prm,pprm)
#define ILOGGER_GetParam(p,typ,pprm)         GET_PVTBL(p,ILogger)->GetParam(p,typ,pprm)
#if !defined( AEE_LOG_DISABLE )
#define ILOGGER_PutRecord(p,bkt,pRcd)        GET_PVTBL(p,ILogger)->PutRecord(p,bkt,pRcd)
#define ILOGGER_Printf(p,args)               GET_PVTBL(p,ILogger)->Printf args    // declared like IWEB_GetResponse()
#define ILOGGER_PutMsg(p,bkt,fn,ln,msg,na,a) GET_PVTBL(p,ILogger)->PutMsg(p,bkt,fn,ln,msg,na,a)
#define ILOGGER_PutItem(p,bkt,typ,itm,sz)    GET_PVTBL(p,ILogger)->PutItem(p,bkt,typ,itm,sz)
#else
#define ILOGGER_PutRecord(p,bkt,pRcd)        SUCCESS
#define ILOGGER_Printf(p,args)               SUCCESS
#define ILOGGER_PutMsg(p,bkt,fn,ln,msg,na,a) SUCCESS
#define ILOGGER_PutItem(p,bkt,typ,itm,sz)    SUCCESS
#endif

#endif   // AEELOGGER_H
/*=============================================================================
   DATA STRUCTURE DOCUMENTATION
===============================================================================
AEELogParamType

Description:
   Possible parameters available to configure the ILogger interface or to 
   get information concerning the current state of the ILogger interface.

   In addition to the list below, there may be OEM specific log parameters which 
   will numerically start after AEE_LOG_PARAM_LAST.  See the specific implementation 
   header files for more information.

Definition:

typedef  uint16                        AEELogParamType;
#define  AEE_LOG_PARAM_INSTANCE_ID     0x001 // InstanceID is a developer-defined ID number 
                                             // that can be used as the developer wishes.  
                                             // Possible uses are to distinguish between 
                                             // different application threads or different 
                                             // application runtime states. The default instance
                                             // ID is zero.
#define  AEE_LOG_PARAM_FILTER_ONE      0x002 // Filter a single log bucket
#define  AEE_LOG_PARAM_FILTER_BLOCK    0x003 // sets filters for a block of 32 buckets
#define  AEE_LOG_PARAM_SENT            0x004 // Number of packets sent by an instance of ILogger
#define  AEE_LOG_PARAM_DROPPED         0x005 // Number of packets dropped by an instance of ILogger
                                             // Dropeed packets do not include packets that have
                                             // been filtered.
#define  AEE_LOG_PARAM_FILE_FLUSH      0x006 // Flushes the current log file and resets file pointer
                                             // to beginning of file.  May only be used 
                                             // with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_FILE_MAX_SIZE   0x007 // Sets the maximum size the log file may become
                                             // Defaults to 1/2 the free EFS space.
                                             // May only be used with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_FILE_FREE_SPACE 0x008 // Amount of free space left in the log file
                                             // before it reaches the maximum size set with
                                             // the AEE_LOG_PARAM_FILE_MAX_SIZE parameter
                                             // May only be used with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_FILE_NEW        0x009 // Closes the current log file and creates 
                                             // a new one using the specified name
                                             // May only be used with AEECLSID_LOGGER_FILE
#define  AEE_LOG_PARAM_SIO_PORT_CONFIG 0x00A // Gets and sets the serial port configuration
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_SIO_OPEN_PORT   0x00B // Opens the serial port for logging, this may
                                             // or may not open the port immediatly, the serial 
                                             // port status is returned by ILOGGER_SetParam() and
                                             // if the port open is in progress the caller must
                                             // check the port status at a later time using
                                             // AEE_LOG_PARAM_SIO_PORT_STATUS or providing
                                             // a callback
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_SIO_CLOSE_PORT  0x00C // Closes the serial port used for logging, this may
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_SIO_PORT_STATUS 0x00D // Returns the status of the serial port
                                             // May only be used with AEECLSID_LOGGER_SERIAL 
#define  AEE_LOG_PARAM_SIO_SET_TRIG    0x00E // Sets serial input/output triggers
                                             // May only be used with AEECLSID_LOGGER_SERIAL
#define  AEE_LOG_PARAM_LAST            0x100 // This must be the last defined param type
                                             // OEM specific parameters start at this value, 
                                             // See the specific implementation header files 
                                             // for more information.
Comments:

  The following tables describes the parameter usage:
===pre>
   AEE_LOG_PARAM_INSTANCE_ID
      SetParam:  param  - New instance ID, 
                 pParam - None  
      GetParam:  pParam - Address of memory location to place current instance ID
   AEE_LOG_PARAM_FILTER_ONE
      SetParam:  param  - Log bucket to filter, must be greater then 0 and less then AEE_LOG_BUCKET_LAST
                 pParam - Boolean value indicating weather to turn bucket on or off, TRUE turns bucket on
                          All buckets are off by default
      GetParam:  pParam - None   
   AEE_LOG_PARAM_FILTER_BLOCK
      SetParam:  param  - Which block of filters to set where 0 is the first block and param 
                          must be less then AEE_LOG_NUM_BUCKET_BLOCKS
                 pParam - 32 bit value to set block to.  Each bit represents one bucket
      GetParam:  pParam - When calling which block to get where 0 is the first block, on return the requsted 32 bit block  
   AEE_LOG_PARAM_SENT
      SetParam:  param  - Number to preset sent count with, or zero to reset count, 
                 pParam - None  
      GetParam:  pParam - Address of memory location to place current number of packets sent
   AEE_LOG_PARAM_DROPPED
      SetParam:  param  - Number to preset dropped count with, or zero to reset count, 
                 pParam - None  
      GetParam:  pParam - Address of memory location to place current number of packets dropped
   AEE_LOG_PARAM_FILE_FLUSH
      SetParam:  param  - None 
                 pParam - None  
      GetParam:  Not supported
   AEE_LOG_PARAM_FILE_MAX_SIZE
      SetParam:  param  - New maximum log file size, 
                 pParam - none  
      GetParam:  pParam - address of memory location to place current maximum log file size
   AEE_LOG_PARAM_FILE_FREE_SPACE
      SetParam:  Not supported  
      GetParam:  pParam - address of memory location to place remaining free space in log file
   AEE_LOG_PARAM_FILE_NEW
      SetParam:  param  - File mode: _OFM_READWRITE or_OFM_APPEND, 
                 pParam - string containing name of new log file 
      GetParam:  Not supported
   AEE_LOG_PARAM_SIO_PORT_CONFIG
      SetParam:  param  - None
                 pParam - Pointer to user filled structure of type AEESIOConfig
      GetParam:  param  - None
                 pParam - Pointer to empty structure of type AEESIOConfig to be filled by ILogger
    AEE_LOG_PARAM_SIO_OPEN_PORT
      SetParam:  param  - Optional user callback that will be called when port is opened
                 pParam - Pointer to string that describes which port to open, see AEESio.h
      GetParam:  Not supported
    AEE_LOG_PARAM_SIO_CLOSE_PORT
      SetParam:  param  - None
                 pParam - None
      GetParam:  Not supported
    AEE_LOG_PARAM_SIO_PORT_STATUS
      SetParam:  Not supported
      GetParam:  pParam - address of memory location to place current port status
   AEE_LOG_PARAM_SIO_SET_TRIG
      SetParam:  param  - None
                 pParam - Pointer to user filled structure of type AEESIOTriggers
      GetParam:  Not supported

  *AEE_LOG_PARAM_FILE_NEW must be used to create new file but any other file management
  must be done by the developer using IFILEMGR and IFILE.

  *When using AEECLSID_LOGGER_SERIAL all serial port handling is done by ILogger
   Refer to the ISerialPort interface documentaion for more information about
   serial triggers (AEESIOTriggers) and serial port configuration (AEESIOConfig).
===/pre>
See Also:
   specific implementation header files for more information.

=============================================================================
AEELogPriorityType

Description:
    Defines list of log item priorities usable by the application

Definition:
    typedef uint16 AEELogPriorityType;
    #define AEE_LOG_LVL_LOW 0
    #define AEE_LOG_LVL_MED 1
    #define AEE_LOG_LVL_HIGH 2
    #define AEE_LOG_LVL_URGENT 3
    #define AEE_LOG_LVL_ERROR 4 

Comments:
    How these priorites are used is up to the application and the end receiver of
log items

See Also:
    None

===============================================================================
   MACROS DOCUMENTATION
===============================================================================

LOG_TEXT()

Description: 
   The macros, LOG_TEXT() and LOG_TEXT_EX() are provided to the developer to 
   quickly output text messages with or without file and line numbers.  
   LOG_TEXT() only ouputs a simple text message, whereas LOG_TEXT_EX(), in addition
   to the text message, will output the file and line number of where the call
   occurred.

Definition:

#define LOG_TEXT(    inst, bkt, str ) ILOGGER_PutItem( inst, bkt, AEE_LOG_TYPE_TEXT, (uint16)( STRLEN(str) + 1 ), (uint8*)str )

#define LOG_TEXT_EX( inst, bkt, str ) ILOGGER_Printf( inst, ( inst, bkt, __FILE__, (uint16)__LINE__, str ) )

Version:
   Introduced BREW Client 3.1

See Also:
None

===============================================================================
LOG_TEXT_EX()
See the description of LOG_TEXT()

===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

ILogger Interface

Description: 
  This module provides the general logging interface for the AEE layer.

  There are three primary LOGGER implementations; a single one is chosen by the
  application writer by creating an ILogger instance with one of the following
  class ID's:

   AEECLSID_LOGGER_FILE:          sends log items to a file.

   AEECLSID_LOGGER_SERIAL:        sends log items to the serial port.

   AEECLSID_LOGGER_WIN:           sends log items to the BREW Simulator output window.

   Both the file and serial logger implementations output data to their 
   respective output mediums in the following BREW packet format:
===pre>

   -----------------------------
   | BREW Header | Packet data |
   -----------------------------

===/pre>

  But, the serial logger wraps these BREW log packets in an HDLC frame as follows:

===pre>

  ------------------------------------------------------
  | BREW Header | Packet Data | 16b FCS | 8b HDLC flag |
  ------------------------------------------------------

===/pre>

   - The 16 bit FCS (Frame Check Sequence) is a 16 bit CRC as specified in RFC 1171
   - The 8 bit HDLC flag has the value 0x7E, if this flag is found in the BREW Header,
     Packet Data, or the 16b FCS it must be escaped with the 8 bit value 0x7D and xor'd
     with 0x20.  In addition, the escape value 0x7D must also be escaped in the same mannor
     if found.
     
       For example:
===pre>

         In the data -> In the ouput stream
            0x7E     -> 0x7D,0x5E
            0x7D     -> 0x7D,0x5D
===/pre>

     These values must then be unescaped by the receiving software.

   When logging to the serial port there may be a delay between when the ILogger
   interface indicates that the packet has been sent successfully and when the packet
   is completely sent out the serial port.  This is due to buffering delays introduced
   by the lower level serial interface.  For this reason the application developer should
   be aware that if the ILogger interface is released too soon after the last packet
   has been sent, the packet received over the serial port may be truncated.  To avoid this,
   a delay may be introduced between when the last packet is sent and when the 
   ILogger interface is released.

The Win implementation of the ILogger interface writes all outgoing logs to the BREW
   output window using the following format
===pre>

   bkt:XX typ:XX cID:XX iID:XX FILENAME LINENUMBER MESSAGE ARGS

   Where:
   bkt:            log bucket
   typ:            log type
   cID:            classId of currently running BREW application
   iID:            user defined instance ID
   FILENAME:       Optional file name where log was sent
   LINENUMBER:     Optional line number where log was sent
   MESSAGE:        User defined text message
   ARGS:           Optional arguments using ILOGGER_PutMsg()

===/pre>

   When compiling a release version of a BREW applet the constant
   AEE_LOG_DISABLE may be defined which using the preprocessor remove
   all ILogger interface logging functions except the instance creation
   process and getting and setting parameters.  This constant MUST be 
   defined before the BREW application includes AEELogger.h.

The following header file is required:~
AEELogger.h
=======================================================================

ILOGGER_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

=============================================================================

ILOGGER_Release()

Description:
    This function is inherited from IBASE_Release(). 

=======================================================================


ILOGGER_SetParam()

Description: 
   This function is called to configure the performance and behavior of
   the ILogger interface

   Supported Parameters:
      Depends on the current implementation's support, see 
      parameter documentation above.

Prototype:
   int ILOGGER_SetParam( ILogger *po, AEELogParamType pType, uint32 param, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   param:   New settings parameter
   pParam:  Pointer to new settings parameter

Return Value:
  SUCCESS:            Parameter handled successfully
  EUNSUPPORTED:       Parameter type or option not supported
  EFAILED:            General failure, option not handled successfully
  EBADPARM:           NULL parameter pointer

  -- The following log codes only apply to file logging

  EFILENOEXISTS:      Output log file is closed
  EFILEEXISTS:        Output log file is open

  -- The following log codes only apply to serial logging

  SUCCESS:            Parameter handled successfully or successfully opened the port
  EFAILED:            Parameter type or option not supported, or general failure
  AEESIO_EINPROGRESS: Open in progress use AEE_LOG_PARAM_SIO_GET_STATUS
                     to check port status later or wait for callback
  AEESIO_ENOSUCHPORT: The asked port name is not supported
  AEESIO_EINUSE:      Port is currently used by some other entity

  -- See OEM specific header files for further return values

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   AEELogParamType
   Documentation for AEELoggerFile and ILogger interfaces

===============================================================================

ILOGGER_GetParam()

Description: 
   This function is called to get the configuration of the ILogger interface.  

   Supported Parameters:
      Depends on the current implementations support, see 
      AEELoggerTypes.h for more information

Prototype:
   int ILOGGER_GetParam( ILogger *po, AEELogParamType pType, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   pParam:  Pointer to be filled with settings parameter

Return Value:
  SUCCESS:            Parameter handled successfully
  EBADPARM:           NULL parameter pointer
  EUNSUPPORTED:       Parameter type or option not supported
  EFAILED:            General failure, option not handled successfully

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   AEELogParamType

===============================================================================

ILOGGER_Printf()

Description: 
   This function is called to send a formatted ASCII text 
   message.

   Since this routine is a MACRO that allows variable arguments, it must
   be called as follows:
===pre>
   ILOGGER_Printf( pMe->m_pILogger, 
                   ( pMe->m_pILogger, 
                   USER_BUCKET1,
                   __FILE__, 
                   (uint16)__LINE__, 
                   "msg",
                   args ) );
===/pre>
  Notice that the second argument is actually multiple arguments in parentheses,
  and args can be multiple comma-separated values

Prototype:
   int ILOGGER_Printf( ILogger *pMe, 
                       AEELogBucketType bucket, 
                       const char *pszFileName, 
                       uint16 nLineNum, 
                       const char *pszFormat, 
                       ... );

Parameters:
   pMe:           Pointer to the ILogger object
   bucket:        Bucket to place item
   pszFileName:   Name of file calling this function
   nLineNum:      Line number in file where it was called
   pszFormat:     ASCII text string similar to a printf format string
   ...:           Format string arguments

Return Value:
  SUCCESS:         Log sent successfully
  EBADPARM:        Invalid pointer to pszFormat
  EUNSUPPORTED:    Log item filtered
  ENOMEMORY:       Unable to allocate required memory
  EFAILED:         Log not sent

  -- The following log codes only apply to file logging

  EFSFULL:         Not enough space in log file for this packet
  EFILENOEXISTS:   Output log file is closed   

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   AEELogPriorityType

===============================================================================

ILOGGER_PutMsg()

Description:
   This function is called to send a predefined binary message and
   allows fast logging due to the limited formatting required and the fixed size
   of the outgoing log message.

   The outgoing binary message's data of type structure AEELogBinMsgType, which
   is defined in AEELoggerTypes.h.

Prototype:
   int ILOGGER_PutMsg( ILogger *pMe, 
                       AEELogBucketType bucket, 
                       const char *pszFileName, 
                       uint16 nLineNum, 
                       const char *pszMsg, 
                       uint8  nNumArgs,
                       uint32 args[ MAX_LOG_TYPE_BIN_MSG_ARGS ] )

Parameters:
   pMe:           Pointer to the ILogger object
   bucket:        Bucket to place item
   pszFileName:   ASCII NULL terminated name of file calling this function
   nLineNum:      Line number in file where it was called
   pszMsg:        ASCII NULL terminated text message
   nNumArgs:      length of the args array
   args:          array containing uint32 arguments

Return Value:
  SUCCESS:         Log sent successfully
  EBADPARM:        Invalid pointer to pszMsg or nNumArgs too large
  EUNSUPPORTED :   Log item filtered
  ENOMEMORY:       Unable to allocate required memory
  EFAILED:         Log not sent

  -- The following log codes only apply to file logging

  EFSFULL:         Not enough space in log file for this packet
  EFILENOEXISTS:   Output log file is closed   

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:
   AEELogBinMsgType
   AEELogPriorityType

===============================================================================

ILOGGER_PutItem()

Description: 
   This function is called to send a prioritized user-defined binary 
   message.

   Here are the steps to define a user log item type:
   1. Choose a user item number and define a meaningful name to it, 
      ie:   #define MY_APPS_LOG_ITEM_TYPE AEE_LOG_TYPE_USER_1
   2. Define a structure that corresponds to you're new type, 
      ie:
===pre>
            typedef struct{
               uint8    foo1;
               uint32   foo2;
               uint8    fooString[ STRING_SIZE ];
            } myAppsItem;
===/pre>
   3. Enable the PC software that will be reading the logging output to 
      recognize the log item type AEE_LOG_TYPE_USER_1 ( which in this case 
      is MY_APPS_LOT_ITEM_TYPE )
   4. Call ILOGGER_PutItem() with MY_APPS_LOT_ITEM_TYPE, a pointer to an 
      instance of myAppsItem, and the size of myAppsItem.

Prototype:
   int ILOGGER_PutItem( ILogger *pMe, 
                        AEELogBucketType bucket, 
                        AEELogItemType type, 
                        uint16 nSize,
                        uint8 *pItem )

Parameters:
   pMe:     Pointer to the ILogger object
   bucket:  Bucket to place item
   type:    User defined item type
   nSize:   Size of type in bytes
   pItem:   Pointer to instance of type

Return Value:
  SUCCESS:         Log sent successfully
  EBADPARM:        Invalid pointer to pItem or size equal to zero
  EUNSUPPORTED:   Log item filtered
  ENOMEMORY:       Unable to allocate required memory
  EFAILED:         Log not sent

  -- The following log codes only apply to file logging

  EFSFULL:         Not enough space in log file for this packet
  EFILENOEXISTS:   Output log file is closed   

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also: 
   AEELogPriorityType
   AEELogItemType

===============================================================================

ILOGGER_PutRecord()

Description:
   This function should not be used by application developers and is contained 
   in the vtable so that it is passed down to lower layers where it is 
   required.

Prototype:
   None

Parameters:
   None

Return Value:
   None

Comments:  
   None

Side Effects: 
   None

Version:
   Introduced BREW Client 2.1

See Also:

=============================================================================*/

// end AEELogger.h
