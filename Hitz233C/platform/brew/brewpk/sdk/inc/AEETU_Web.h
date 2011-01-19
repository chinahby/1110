#ifndef AEETU_WEB_H
#define AEETU_WEB_H /* #ifndef AEETU_WEB_H */
/*============================================================================

FILE:  AEETU_Web.h

SERVICES:  
        thread util header for IWeb interactions

GENERAL DESCRIPTION:
        AEEThread/IThread in BREW provides only the most elementary 
        thread functionality, and leaves implementation of blocking calls
        to an external library (this one).

        These functions can be implemented by anyone, since they use 
        the IThread object in a public way (no private hooks).  Hopefully
        these implementations save some time...


============================================================================

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#include "AEEThread.h"
#include "AEEWeb.h"

#ifdef __cplusplus
extern "C" {
#endif /* #ifdef __cplusplus */

void IThread_GetWebResponseV(IThread *piThread, IWeb *piWeb,
                             IWebResp **ppiwresp, const char *cpszUrl, 
                             WebOpt *awozList);

void IThread_GetWebResponse(IThread *piThread, IWeb *piWeb, 
                            IWebResp **ppiwresp, const char *cpszUrl, ... );


#ifdef __cplusplus
}
#endif /* #ifdef __cplusplus */


#endif /* #ifndef AEETU_WEB_H */
