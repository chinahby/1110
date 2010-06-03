/*=============================================================================
                    IMPORTANT NOTICE

FILE: OEMLogger.h

DESCRIPTION
   This file contains the function declarations common to all OEMLogger
   implementations

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

//-----------------------------------------------------------------------------
//           Include Files
//-----------------------------------------------------------------------------

#include "AEELogger.h"

//-----------------------------------------------------------------------------
//          Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//          Type Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//          Function Prototypes
//-----------------------------------------------------------------------------
int  OEMLogger_Printf(  ILogger *po, AEELogBucketType bucket, const char *pszFileName, uint16 nLineNum, const char *pszFormat, ... );
int  OEMLogger_PutMsg(  ILogger *po, AEELogBucketType bucket, const char *pszFileName, uint16 nLineNum, const char *pszMsg, uint8 nNumArgs, uint32 args[ MAX_LOG_TYPE_BIN_MSG_ARGS ] );
int  OEMLogger_PutItem( ILogger *po, AEELogBucketType bucket, AEELogItemType type, uint16 nSize, const uint8 *pItem );

// end OEMLogger.h

/*=============================================================================
   DATA STRUCTURE DOCUMENTATION
===============================================================================


===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================

OEMLogger Interface


Description:
  This module provides the general logging interface for the OEM layer.

  There are three primary OEMLogger implementations, a single one is chosen by the
  application writer by creating an OEMLogger instance with one of the following
  class ID's:
~

   AEECLSID_LOGGER_FILE          sends log items to a file~
   AEECLSID_LOGGER_SERIAL        sends log items to the serial port~
   AEECLSID_LOGGER_WIN           sends log items to the Simulator output window~
*

  Note: Serial Logger is defined but no reference implementation exists at this
  time.

   Both the file and serial logger implementations output data to their
   respective output mediums in the following BREW packet format:
===pre>
   -----------------------------
   | BREW Header | Packet data |
   -----------------------------

  But, the serial logger wraps these BREW log packets in an HDLC frame as follows:
  ------------------------------------------------------
  | BREW Header | Packet Data | 16b FCS | 8b HDLC flag |
  ------------------------------------------------------
===/pre>
   - The 16 bit FCS (Frame Check Sequence) is a 16 bit CRC as specified in RFC 1171
   - The 8 bit HDLC flag has the value 0x7E, if this flag is found in the BREW Header,
     Packet Data, or the 16b FCS it must be escaped with the 8 bit value 0x7D and xor'd
     with 0x20.  In addition, the escape value 0x7D must also be escaped in the same manner
     if found.

       For example:
~

         In the data -> In the output stream~
            0x7E     -> 0x7D,0x5E~
            0x7D     -> 0x7D,0x5D~
*
     These values must then be unescaped by the receiving software.

   When logging to the serial port there may be a delay between when the OEMLogger
   interface indicates that the packet has been sent successfully and when the packet
   is completely sent out the serial port.  This is due to buffering delays introduced
   by the lower level serial interface.  For this reason the application developer should
   be aware that if the OEMLogger interface is released too soon after the last packet
   has been sent, the packet received over the serial port may be truncated.  To avoid this,
   a delay may be introduced between when the last packet is sent and when the
   OEMLogger interface is released.

The Win implementation of the OEMLogger interface writes all outgoing logs to the BREW
   output window using the following format:
===pre>

   bkt:XX typ:XX cID:XX iID:XX FILENAME LINENUMBER MESSAGE ARGS
===/pre>
~
   Where:~
   bkt:            log bucket~
   typ:            log type~
   cID:            classID of currently running BREW application~
   iID:            user defined instance ID~
   FILENAME:       Optional file name where log was sent~
   LINENUMBER:     Optional line number where log was sent~
   MESSAGE:        User defined text message~
   ARGS:           Optional arguments using OEMLogger_PutMsg()~
*

   When compiling a release version of a BREW applet the constant
   AEE_LOG_DISABLE may be defined which using the preprocessor remove
   all OEMLogger interface logging functions except the instance creation
   process and getting and setting parameters.  This constant MUST be
   defined before the BREW application includes AEELogger.h.

===============================================================================


OEMLogger_Printf()

Description:
   This function is called to send a formatted ASCII text
   message.

   Since this routine is a MACRO that allows variable arguments it must
   be called as follows:

   OEMLogger_Printf( pMe->m_pILogger,
                   ( pMe->m_pILogger,
                   USER_BUCKET1,
                   __FILE__,
                   (uint16)__LINE__,
                   "msg",
                   args ) );

  Notice that the second argument is actually multiple arguments in parentheses,
  and args can be multiple comma separated values

Prototype:
   int OEMLogger_Printf( ILogger *pMe,
                       AEELogBucketType bucket,
                       const char *pszFileName,
                       uint16 nLineNum,
                       const char *pszFormat,
                       ... );

Parameters:
   pMe:           Pointer to the OEMLogger object
   bucket:        Bucket to place item
   pszFileName:   Name of file calling this function
   nLineNum:      Line number in file where it was called
   pszFormat:     ASCII text string similar to a printf format string
   ...:           Format string arguments

Return Value:
  SUCCESS         Log send successfully
  EBADPARM        Invalid pointer to pszFormat
  EUNSUPPORTED    Log item filtered
  ENOMEMORY       Unable to allocated required memory
  EFAILED         Log not sent
  -- The following error codes only apply to file logging
  EFSFULL         Not enough space in log file for this packet
  EFILENOEXISTS   Output log file is closed

Comments:
   This function is already implemented in the reference implementation and
   should not be modified by the OEM.

Side Effects:
   None

See Also:
   AEELogPriorityType

===============================================================================


OEMLogger_PutMsg()

Description:
   This function is called to send a predefined binary message and
   allows fast logging due to the limited formatting required and the fixed size
   of the outgoing log message.

   The outgoing binary message's data of type structure AEELogBinMsgType, which
   is defined in AEELoggerTypes.h.

Prototype:
   int OEMLogger_PutMsg( ILogger *pMe,
                       AEELogBucketType bucket,
                       const char *pszFileName,
                       uint16 nLineNum,
                       const char *pszMsg,
                       uint8  nNumArgs,
                       uint32 args[ MAX_LOG_TYPE_BIN_MSG_ARGS ] )

Parameters:
   pMe:           Pointer to the OEMLogger object
   bucket:        Bucket to place item
   pszFileName:   ASCII NULL terminated name of file calling this function
   nLineNum:      Line number in file where it was called
   pszMsg:        ASCII NULL terminated text message
   nNumArgs       length of the args array
   args:          array containing uint32 arguments

Return Value:
  SUCCESS         Log send successfully
  EBADPARM        Invalid pointer to pszMsg or nNumArgs too large
  EUNSUPPORTED    Log item filtered
  ENOMEMORY       Unable to allocated required memory
  EFAILED         Log not sent
  -- The following error codes only apply to file logging
  EFSFULL         Not enough space in log file for this packet
  EFILENOEXISTS   Output log file is closed

Comments:
   This function is already implemented in the reference implementation and
   should not be modified by the OEM.

Side Effects:
   None

See Also:
   AEELogBinMsgType
   AEELogPriorityType

===============================================================================

OEMLogger_PutItem()

Description:
   This function is called to send a user defined binary
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
   4. Call OEMLogger_PutItem() with MY_APPS_LOT_ITEM_TYPE, a pointer to an
      instance of myAppsItem, and the size of myAppsItem.

Prototype:
   int OEMLogger_PutItem( ILogger *pMe,
                        AEELogBucketType bucket,
                        AEELogItemType type,
                        uint16 nSize,
                        uint8 *pItem )

Parameters:
   pMe:     Pointer to the OEMLogger object
   bucket:  Bucket to place item
   type:    User defined item type
   nSize:   Size of type in bytes
   pItem:   Pointer to instance of type

Return Value:
  SUCCESS         Log send successfully
  EBADPARM        Invalid pointer to pItem or size equal to zero
  EUNSUPPORTED    Log item filtered
  ENOMEMORY       Unable to allocated required memory
  EFAILED         Log not sent

The following error codes only apply to file logging~
~
  EFSFULL         Not enough space in log file for this packet~
  EFILENOEXISTS   Output log file is closed   ~
*
Comments:
   This function is already implemented in the reference implementation and
   should not be modified by the OEM.

Side Effects:
   None

See Also:
   AEELogPriorityType
   AEELogItemType

=======================================================================

OEMLoggerFile_SetParam()

Description:
   This function is called to set performance and behavior of
   the logging interface.

   Supported Parameters:
      Depends on the current implementation's support, See AEELogger.h for
      supported parameters

Prototype:
   int OEMLoggerFile_SetParam( ILogger *po, AEELogParamType pType, uint32 param, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   param:   New settings parameter
   pParam:  Pointer to new settings parameter

Return Value:
   EMEMPTR:       Invalid pParam pointer if pParam is required for this LogParamType
   SUCCESS:       LogParamType handled successfully
   EUNSUPPORTED:  The log parameter is not supported

Comments:
   None

Side Effects:
   None

See Also:
   AEELogParamType

===============================================================================

OEMLoggerFile_GetParam()

Description:
   This function is called to get the configuration of the ILogger interface

  Supported Parameters:
   See AEELogger.h for supported parameters

Prototype:
   int OEMLoggerFile_GetParam( ILogger *po, AEELogParamType pType, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   pParam:  Pointer to be filled with settings parameter

Return Value:
   EMEMPTR:       Invalid pParam pointer if pParam is required for this LogParamType
   SUCCESS:       LogParamType handled successfully
   EUNSUPPORTED:  The log parameter is not supported

Comments:
   None

Side Effects:
   None

See Also:
   AEELogParamType

=======================================================================

OEMLoggerFile_PutRecord()

Description:
   This function is called to output a log record to a file.  This function
   shall not prepend the BREW Log header.  the prepending of the BREW header
   is performed in the reference functions ILOGGER_PutItem(), ILOGGER_Printf(),
   or ILOGGER_PutMsg().

Prototype:
   int OEMLoggerFile_PutRecord( ILogger *po,
                                AEELogBucketType bucket,
                                AEELogRecord *pRcd ){

Parameters:
   pMe:     Pointer to the ILOGGER object
   bucket:  Bucket to place item
   pItem:   Pointer to data with BREW header at beginning to write to log

Return Value:
  SUCCESS         Item data written successfully
  EBADPARM        Invalid pointer to pItem
  EFAILED         General failure
  EFSFULL         Not enough space in log file for this packet
  EFILENOEXISTS   Output log file is closed

Comments:
   None

Side Effects:
   None

See Also:
   AEELogBucketType
   AEELogRecord

=======================================================================

OEMLoggerWin_SetParam()

Description:
   This function is called to set performance and behavior of
   the logging interface.

   Supported Parameters:
      Depends on the current implementation's support, See AEELogger.h for
      supported parameters

Prototype:
   int OEMLoggerWin_SetParam( ILogger *po, AEELogParamType pType, uint32 param, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   param:   New settings parameter
   pParam:  Pointer to new settings parameter

Return Value:
   EMEMPTR:       Invalid pParam pointer if pParam is required for this LogParamType
   SUCCESS:       LogParamType handled successfully
   EUNSUPPORTED:  The log parameter is not supported

Comments:
   None

Side Effects:
   None

See Also:
   AEELogParamType

===============================================================================

OEMLoggerWin_GetParam()

Description:
   This function is called to get the configuration of the ILogger interface

  Supported Parameters:
   See AEELogger.h for supported parameters

Prototype:
   int OEMLoggerWin_GetParam( ILogger *po, AEELogParamType pType, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   pParam:  Pointer to be filled with settings parameter

Return Value:
   EMEMPTR:       Invalid pParam pointer if pParam is required for this LogParamType
   SUCCESS:       LogParamType handled successfully
   EUNSUPPORTED:  The log parameter is not supported

Comments:
   None

Side Effects:
   None

See Also:
   AEELogParamType

=======================================================================

OEMLoggerWin_PutRecord()

Description:
   This function is called to output a log record to the BREW output
   window.  This function shall not prepend the BREW Log header.  the
   prepending of the BREW header is performed in the reference functions
   ILOGGER_PutItem(), ILOGGER_Printf(), or ILOGGER_PutMsg().  See the
   interface description for detail of the string format used when
   writing to the BREW output window.

Prototype:
   int OEMLoggerWin_PutRecord( ILogger *po,
                               AEELogBucketType bucket,
                               AEELogRecord *pRcd ){

Parameters:
   pMe:     Pointer to the ILOGGER object
   bucket:  Bucket to place item
   pItem:   Pointer to data with BREW header at beginning to write to log

Return Value:
  SUCCESS         Item data written successfully
  EBADPARM        Invalid pointer to pItem
  EFAILED         General failure

Comments:
   None

Side Effects:
   None

See Also:
   AEELogBucketType
   AEELogRecord
==============================================================
AEELogger

See the ILOGGER Interface in the BREW API Reference.

======================================================================*/


