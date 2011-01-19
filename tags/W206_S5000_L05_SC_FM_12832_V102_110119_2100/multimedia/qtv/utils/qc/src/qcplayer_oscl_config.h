/*==========================================================================
                      qcplayer_oscl_config.h
   Description:

    This is the top-level configuration file that gets included
    in all the Qualcomm player code, including oscl zrex,
    oscl psos, and QCPlayer.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved;
and Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */


#ifndef _QCPLAYER_OSCL_CONFIG_H_
#define _QCPLAYER_OSCL_CONFIG_H_

//#if defined(PV_OS_ZREX)
#if 1

///////////////////////////
//
// QC Player Build configuration
//
///////////////////////////

//Release vs engineering build.
#ifndef _DEBUG
#define QC_RELEASE_BUILD
#endif

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"


///////////////////////////
//
// Player configuration parameters
//
///////////////////////////




#endif //PV_OS...
#endif //_QCPLAYER_OSCL_CONFIG_H_

