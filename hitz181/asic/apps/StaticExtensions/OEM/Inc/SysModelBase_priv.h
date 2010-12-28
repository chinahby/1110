#ifndef SYSMODELBASE_PRIV_H
#define SYSMODELBASE_PRIV_H
/*=============================================================================

FILE: SysModelBase_priv.h

SERVICES:  SysModelBase protected/private interface 

GENERAL DESCRIPTION:
   The SysModelBase class implements an IModel interface that may be shared
   by multiple BREW applets. 


PUBLIC CLASSES AND STATIC FUNCTIONS:
   IModel

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/oem/inc/SysModelBase_priv.h#3 $
$DateTime: 2008/03/11 13:46:26 $
  $Author: satishk $
  $Change: 623281 $
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/06/05   mjv     Correctly handle the canceling of any listener during a
                   notify cycle 
06/01/04   mjv     Ported to latest Forms/Widget library
04/30/04   mjv     SysModelBase now manually deallocates its system objects
                   when the associated applet stops listening to the Model,
                   instead of waiting for BREW to automatically deallocate
                   the object when the applet exits.  This prevents BREW 
                   from complaining about a "System Object Leak" whenever
                   the associated applet exits.
03/23/04   mjv     Initial revision

=============================================================================*/


/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEE.h"
#include "AEEModel.h"
#include "AEE_OEMComdef.h"


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/
typedef struct SysModelBase SysModelBase;

typedef void (*PFNSYSMODELBASE_DTOR)(SysModelBase *pMe);

typedef struct SysModelAppInfo SysModelAppInfo;

struct SysModelAppInfo {
   ACONTEXT        *appCtx;  // App context

   AEECallback      cbSysObject;  // Applet cleanup callback
   SysModelBase    *ownerSysModel;  // The SysModelBase object that owns us

   ModelListener   *pListeners;  // List of Listeners for this applet

   SysModelAppInfo *pNext;
};


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

typedef struct SysModelBase {
   AEEVTBL(IModel)      *pvt;
   uint32                nRefs;
   PFNSYSMODELBASE_DTOR  pDtor;

   SysModelAppInfo      *appList; // List of apps that are listening to us

   ModelListener        *pNextNotifyListener;
   SysModelAppInfo      *pNextNotifyApp;
} SysModelBase; 


////
// Safe typecasts
static __inline SysModelBase *IMODEL_TO_SYSMODELBASE(IModel *p)
{
   return (SysModelBase *) p;
}

static __inline IModel *SYSMODELBASE_TO_IMODEL(SysModelBase *pMe)
{
   return (IModel *) (void *) pMe;
}


////
// Field access macros
static __inline uint32 SYSMODELBASE_NREFS(SysModelBase *pMe)
{
   return pMe->nRefs;
}

////
// IModel Interface Methods
uint32 SysModelBase_AddRef(IModel *p);
uint32 SysModelBase_Release(IModel *p);
int SysModelBase_QueryInterface(IModel *p, AEECLSID cls, void **ppif);
int SysModelBase_AddListener(IModel *p, ModelListener *pListener);
void SysModelBase_Notify(IModel *p, ModelEvent *pe);



////
// Protected Methods
void SysModelBase_Ctor(SysModelBase          *pMe,
                       AEEVTBL(IModel)       *pvt,
                       PFNSYSMODELBASE_DTOR   pDtor);
void SysModelBase_Dtor(SysModelBase *pMe);

////
// Private Methods
void SysModelBase_CancelListener(ModelListener *pListener);
SysModelAppInfo *SysModelBase_GetAppInfo(SysModelBase *pMe);
void SysModelBase_FreeAppInfo(SysModelAppInfo *thisInfo);

#endif /* SYSMODELBASE_PRIV_H */
