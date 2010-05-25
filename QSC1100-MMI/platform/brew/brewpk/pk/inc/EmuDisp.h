#ifndef  EMUDISP_H
#define  EMUDISP_H
/*=================================================================================
FILE:          EmuDisp.h

SERVICES:      IEmuDisp interface

DESCRIPTION:   Interface to simulator displays

PUBLIC CLASSES:

INITIALIAZTION AND SEQUENCEING REQUIREMENTS:

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEBitmap.h"

/*---------------------------------------------------------------------------------
      Type Declarations
---------------------------------------------------------------------------------*/

typedef struct IEmuDisp       IEmuDisp;
typedef struct IEmuDisp2      IEmuDisp2;


/*---------------------------------------------------------------------------------
      Class IDs
---------------------------------------------------------------------------------*/

#define AEECLSID_EMUDISP1              0x01013247
#define AEECLSID_EMUDISP2              0x01013248
#define AEECLSID_EMUDISP3              0x01013249
#define AEECLSID_EMUDISP4              0x0101324a


//*********************************************************************************
//
// IEmuDisp Interface
//
//*********************************************************************************

#define AEEIID_EMUDISP                 0x01013246

#define INHERIT_IEmuDisp(iname)                                                  \
   INHERIT_IQueryInterface(iname);                                               \
   int  (*GetInfo)                (iname *po, AEEBitmapInfo *pInfo, int nSize);  \
   IBitmap *(*GetSpeedEmuBitmap)  (iname *po, IBitmap *pbm);                     \
   int  (*Update)                 (iname *po, IBitmap *pbmSrc, AEERect *prc)


AEEINTERFACE(IEmuDisp)
{
   INHERIT_IEmuDisp(IEmuDisp);
};


// Access macros for IEmuDisp
#define  IEMUDISP_AddRef(p)               AEEGETPVTBL(p,IEmuDisp)->AddRef(p)
#define  IEMUDISP_Release(p)              AEEGETPVTBL(p,IEmuDisp)->Release(p)
#define  IEMUDISP_QueryInterface(p,i,p2)  AEEGETPVTBL(p,IEmuDisp)->QueryInterface(p,i,p2)
#define  IEMUDISP_GetInfo(p,i,n)          AEEGETPVTBL(p,IEmuDisp)->GetInfo(p,i,n)
#define  IEMUDISP_GetSpeedEmuBitmap(p,b)  AEEGETPVTBL(p,IEmuDisp)->GetSpeedEmuBitmap(p,b)
#define  IEMUDISP_Update(p,b,r)           AEEGETPVTBL(p,IEmuDisp)->Update(p,b,r)


//*********************************************************************************
//
// IEmuDisp2 Interface
//
//*********************************************************************************

#define AEEIID_EMUDISP2                0x0102f84f

#define INHERIT_IEmuDisp2(iname)                                                 \
   INHERIT_IEmuDisp(iname);                                                      \
   int    (*GetDepthList)         (iname *po, uint32 *pu, int *pnSize)


AEEINTERFACE(IEmuDisp2)
{
   INHERIT_IEmuDisp2(IEmuDisp2);
};


// Access macros for IEmuDisp2
#define  IEMUDISP2_AddRef(p)              AEEGETPVTBL(p,IEmuDisp2)->AddRef(p)
#define  IEMUDISP2_Release(p)             AEEGETPVTBL(p,IEmuDisp2)->Release(p)
#define  IEMUDISP2_QueryInterface(p,i,p2) AEEGETPVTBL(p,IEmuDisp2)->QueryInterface(p,i,p2)
#define  IEMUDISP2_GetInfo(p,i,n)         AEEGETPVTBL(p,IEmuDisp2)->GetInfo(p,i,n)
#define  IEMUDISP2_GetSpeedEmuBitmap(p,b) AEEGETPVTBL(p,IEmuDisp2)->GetSpeedEmuBitmap(p,b)
#define  IEMUDISP2_Update(p,b,r)          AEEGETPVTBL(p,IEmuDisp2)->Update(p,b,r)
#define  IEMUDISP2_AddRef(p)              AEEGETPVTBL(p,IEmuDisp2)->AddRef(p)
#define  IEMUDISP2_Release(p)             AEEGETPVTBL(p,IEmuDisp2)->Release(p)
#define  IEMUDISP2_QueryInterface(p,i,p2) AEEGETPVTBL(p,IEmuDisp2)->QueryInterface(p,i,p2)
#define  IEMUDISP2_GetInfo(p,i,n)         AEEGETPVTBL(p,IEmuDisp2)->GetInfo(p,i,n)
#define  IEMUDISP2_GetSpeedEmuBitmap(p,b) AEEGETPVTBL(p,IEmuDisp2)->GetSpeedEmuBitmap(p,b)
#define  IEMUDISP2_Update(p,b,r)          AEEGETPVTBL(p,IEmuDisp2)->Update(p,b,r)
#define  IEMUDISP2_GetDepthList(p,l,s)    AEEGETPVTBL(p,IEmuDisp2)->GetDepthList(p,l,s)

#endif /* EMUDISP_H */

