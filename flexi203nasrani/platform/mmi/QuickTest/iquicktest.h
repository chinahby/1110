#ifndef __IQuickTest_H__
#define __IQuickTest_H__
/*=============================================================================

FILE: IQuickTest.h

SERVICES: Provides the quick test functionality for the china phone

GENERAL DESCRIPTION:
        Quicktest Applet

STATIC FUNCTIONS:
        Not applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:
        Not applicable

(c) COPYRIGHT 2001 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/


//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------
// #include "AEE.h"

//-----------------------------------------------------------------------------
//  Defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//  Types
//-----------------------------------------------------------------------------

typedef struct _IQuickTest IQuickTest;
//-----------------------------------------------------------------------------
//  Interfaces
//-----------------------------------------------------------------------------
QINTERFACE(IQuickTest)
{
   INHERIT_IApplet(IQuickTest);
};

#define IQUICKTEST_AddRef(p)                GET_PVTBL(p,IQuickTest)->AddRef(p)
#define IQUICKTEST_Release(p)               GET_PVTBL(p,IQuickTest)->Release(p)
#define IQUICKTEST_HandleEvent(p,ec,wp,dw)  GET_PVTBL(p,IQuickTest)->HandleEvent(p,ec,wp,dw)


#endif
