/*
  ============================================================================

   FILE:  GIFViewerExtension.h
   
   SERVICES:  
           GIF Viewer extension types and definitions
   
   GENERAL DESCRIPTION:
   
   REVISION HISTORY: 
           Fri Nov 2, 2001: Created

  ============================================================================
  ============================================================================

               Copyright © 1999-2007 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

  ============================================================================
  ============================================================================
*/
#ifndef __GIFVIEWEREXTENSION_H__
#define __GIFVIEWEREXTENSION_H__


// For definition of IPARM_COMPATFLAGS
#include "IPARM_COMPATFLAGS.bid"


// compatibility flags
#define GIFCF_USEBGCOLOR   0x01  // unlike web browser mode, actually use the BG color

#ifndef IPARM_EXTFUNC
#define IPARM_EXTFUNC   13

// structure for IPARM_EXTFUNC
typedef struct {
   AEECLSID id;
   void    *pBuf;
   int      nBufSize;
} AEEImageExtFunc;

static __inline int IIMAGE_ExtFunc(IImage *me, AEECLSID id, void *pBuf, int nBufSize)
{
   AEEImageExtFunc parm;
   int nReturn = EUNSUPPORTED;

   parm.id = id;
   parm.pBuf = pBuf;
   parm.nBufSize = nBufSize;

   IIMAGE_SetParm(me, IPARM_EXTFUNC, (int)&parm, (int)&nReturn);
   return nReturn;
}

#endif   // IPARM_EXTFUNC


/*
=============================================================================

Extended functionality exposed through IPARM_EXTFUNC

============================================================================

IPARM_COMPATFLAGS

Description:
   This IPARM is used to set various behavior-compatibility flags for the GIF
   viewer. Currently the flag(s) are:

   GIFCF_USEBGCOLOR        Set the behavior to be as described in the GIF spec
                           as far as how the background color is used. The default
                           behavior is consistent with how web browsers display 
                           animated GIFs when transparent frames are specified. 
                           That is, ignore the background color specified in the 
                           logical screen descriptor when disposing a frame with 
                           the disposal method 'Dispose to background color'.  
                           When this flag is set, we dispose the frame by 
                           filling with the background color. 

   Example usage:

      AEEImageExtFunc parm;
      uint32 dwFlags = GIFCF_USEBGCOLOR;

      parm.id       = IPARM_COMPATFLAGS;
      parm.pBuf     = &dwFlags;
      parm.nBufSize = sizeof(dwFlags);
   
      IIMAGE_SetParm(me, IPARM_EXTFUNC, (int)&parm, (int)&nReturn);

   nReturn contains a standard BREW error code.      
   SUCCESS means IPARM_COMPATFLAGS is supported..

   
=============================================================================

*/


#endif   //__GIFVIEWEREXTENSION_H__

