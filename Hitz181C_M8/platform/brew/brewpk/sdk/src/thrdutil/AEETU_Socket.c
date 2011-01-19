/*
  ============================================================================

FILE:  AEETU_Socket.c

SERVICES:  
        blocking socket send call

GENERAL DESCRIPTION:
        AEEThread/IThread in BREW provides only the most elementary 
        thread functionality, and leaves implementation of blocking calls
        to an external library (this one).

        These functions can be implemented by anyone, since they use 
        the IThread object in a public way (no private hooks).  Hopefully
        these implementations save some time...

REVISION HISTORY: 
        Wed Oct 03 14:47:41 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2001 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/

#include "AEEThreadUtil.h"
#include "AEECallbackUtil.h"

ISocket *IThread_Socket(IThread *me, INetMgr *pmgr, NetSocket stype)
{
   ISocket *piSock = INETMGR_OpenSocket(pmgr, stype);

#if 0

   // We could "realize" the socket now so that callers would see errors at
   // the appropriate time (i.e. EMFILE would be returned by socket(), instead
   // of later by connect()).  This might help portability from UNIX/Win32,
   // but then again, most UNIX/Win32 code probably isn't expecting to ever
   // run out of sockets anyway.
   int nRv;

   if ((ISocket *)0 != piSock) {
      while ((nRv = ISOCKET_IOCtl(piSock, ISOCKET_IOCTL_REALIZE, 0)) == 
             AEE_NET_WOULDBLOCK) {
         AEECallback_Writeable(pcb, piSock);
         ITHREAD_Suspend(me);
      }
      
      if (SUCCESS != nRv) {
         ISOCKET_Release(piSock);
         piSock = 0;
      }
   }
#endif
   if ((ISocket *)0 != piSock) {
      ITHREAD_HoldRsc(me, (IBase*) piSock);
   }
   
   return piSock;
}



