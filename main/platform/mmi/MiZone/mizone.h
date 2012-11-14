#ifndef __IMiZone_H__
#define __IMiZone_H__
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

typedef struct _IMiZone IMiZone;
//-----------------------------------------------------------------------------
//  Interfaces
//-----------------------------------------------------------------------------
QINTERFACE(IMiZone)
{
   INHERIT_IApplet(IMiZone);
};

#define IMIZONE_AddRef(p)                GET_PVTBL(p,IMiZone)->AddRef(p)
#define IMIZONE_Release(p)               GET_PVTBL(p,IMiZone)->Release(p)
#define IMIZONE_HandleEvent(p,ec,wp,dw)  GET_PVTBL(p,IMiZone)->HandleEvent(p,ec,wp,dw)


#endif
