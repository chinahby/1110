#ifndef AEEBCMCSDB_FLOWIDLEN
#define AEEBCMCSDB_FLOWIDLEN
/*======================================================
FILE:  AEEBCMCSDB_FLOWIDLEN.h

SERVICES:  An extension to the IBCMCSDB options

DESCRIPTION:   An extension to the BREW BCMCS DB Interface 
               which adds DB Record options.
=====================================================*/

// This header is a beta and is subject to future changes
#include "AEEBCMCSDB.h"

// We need a unique offset for WEBOPT_32BIT
#define AEEBCMCSDBRECORDOPT_FlowIDLen  (WEBOPT_32BIT + (0x0106da05 & 0x0000ffff))

typedef uint8 AEEBCMCSDBRecord_FlowIDLen;

/*
===============================================================================
MACROS DOCUMENTATION
===============================================================================
BCMCS DB Record options

Description:
   These IWebOpts options specify fields in the BCMCS DB Record.
   ** This is BETA, temporary documentation for the feature in this file **

Definition:

#define AEEBCMCSDBRECORDOPT_FlowIDLen  (WEBOPT_32BIT + (0x0106da05 & 0x0000ffff))

Members:

   AEEBCMCSDBRECORDOPT_FlowIDLen:
   Flow ID Length. pVal contains AEEBCMCSDBRecord_FlowIDLen. 

Version:
   Introduced BREW Client 3.1.5 SP02

See Also:
   WebOpt

===============================================================================
*/

#endif //#ifndef AEEBCMCSDB_FLOWIDLEN

