/*
  ========================================================================

  FILE:       SourceAStream.h

  SERVICES:   

  GENERAL DESCRIPTION:

  ========================================================================
  ========================================================================
    
               Copyright © 1999-2007 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
    
  ========================================================================
  ========================================================================
*/
#ifndef __SOURCEASTREAM_H__
#define __SOURCEASTREAM_H__

#include "AEE.h"
#include "AEESource.h"

typedef void (*PFNREADSTATUS) (void * pv, int32 rvRead);

/*
|| Create an IAStream from an ISource 
*/
IAStream *SourceAStream_Wrap(ISource *pis, PFNREADSTATUS pfn, void *pv, IShell *piShell);


#endif //__SOURCEASTREAM_H__

