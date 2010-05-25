#ifndef __AUTOMATION_LOGGING_H__
#define __AUTOMATION_LOGGING_H__

/*=============================================================================
FILE: AutomationLogging.h

SERVICES: 
    Logging definitions for automatic testing.

GENERAL DESCRIPTION:
    Some logs emitted by the application can be designed for
    parsing by an automatic testing system. This file supports this
    development feature by providing common definitions for these logs.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEE.h"

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
#define AUTOMATION_LOG_HEADER "GSM1X:"

#define AUTOMATION_LOG(format,parameter1)                        \
         DBGPRINTF(AUTOMATION_LOG_HEADER format, parameter1);    \


#define AUTOMATION_LOG_WSTR(format,wStr)                         \
     {                                                           \
         char buffer[40];                                        \
         WSTRTOSTR(wStr, buffer, sizeof(buffer));                \
                                                                 \
         DBGPRINTF(AUTOMATION_LOG_HEADER format, buffer);        \
     }                                                           \


#endif // end __AUTOMATION_LOGGING_H__
