/*=============================================================================
                    IMPORTANT NOTICE

FILE: OEMLoggerDMSS.h

DESCRIPTION
   This file contains the function declarations specific to OEMLoggerDMSS 
   implementations.  This file is for documentation purposes only.

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

//-----------------------------------------------------------------------------
//           Include Files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//          Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//          Type Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//          Function Prototypes
//-----------------------------------------------------------------------------

// end OEMLoggerDMSS.h

/*=============================================================================
   DATA STRUCTURE DOCUMENTATION
===============================================================================

oemLogType

Description:
   This structure type is defined in either log.h or 
   log_dmss.h depending on the target handset.  This member
   will contain the standard DMSS log header (which
   contains the log code, length, and a timestamp), a 
   standard brew header, and the log data send by
   the BREW application.                 

Definition:

typedef logBinType oemLogType;

Comments:
None

See Also:
None


=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
OEMLoggerDMSS Interface

  The OEMLoggerDMSS functions documented here are required to implement 
  the ILogger interface for MSM platforms (i.e. ILogger using 
  AEECLSID_LOGGER_DMSS and for FEATURE_LOGGER_DMSS).

  List of Header files to be included:
OEMLogger.h

=======================================================================

OEMLoggerDMSS_SetParam()

Description: 
   This function is called to set performance and behavior of
   the logging interface.  

   Supported Parameters:
      Depends on the current implementation's support, See AEELogger.h for 
      supported parameters

Prototype:
   int OEMLoggerDMSS_SetParam( ILogger *po, AEELogParamType pType, uint32 param, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   param:   New settings parameter
   pParam:  Pointer to new settings parameter

Return Value:
   EMEMPTR:       Invalid pParam pointer if pParam is required for this LogParamType
   SUCCESS:       LogParamType handled successfully
   EFAILED:       General failure
   EUNSUPPORTED:  The log parameter is not supported

Comments:  
   None

Side Effects: 
   None

See Also:
   AEELogParamType

===============================================================================

OEMLoggerDMSS_GetParam()

Description: 
   This function is called to get the configuration of the OEM logging interface.  

Prototype:
   int OEMLoggerDMSS_GetParam( ILogger *po, AEELogParamType pType, void* pParam )

Parameters:
   pMe:     Pointer to the ILogger object
   pType:   Parameter to modify
   pParam:  Pointer to settings parameter to fill

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

OEMLoggerDMSS_PutRecord()

Description: 
   This function is called to output a log record to the DMSS Diagnostic 
   task.  This function shall not prepend the BREW Log header.  the 
   prepending of the BREW header is performed in the reference functions 
   ILOGGER_PutItem(), ILOGGER_Printf(), or ILOGGER_PutMsg().  See the 
   interface description for details.

Prototype:
   int OEMLoggerDMSS_PutRecord( ILogger *po, 
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

=============================================================================*/
