/*===========================================================================

FILE: OEMFlip.c

SERVICES: IFlip

DESCRIPTION
   This file implements the IFlip interface.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "AEECLSID_FLIP.BID"
#include "AEEFlip.h"
#include "AEEStdLib.h"
#include "AEEShell.h"


#define INVALID_FLIP_ID -1
#ifdef AEE_SIMULATOR
#define FLIP_EXTERN extern
#else
#define FLIP_EXTERN
#endif

/*===========================================================================

                     MACRO DEFINITIONS

===========================================================================*/
#ifndef RELEASEIF
#define RELEASEIF(x) if (x) { IQI_Release((IQueryInterface *)(x)); (x) = 0; }
#endif

/*===========================================================================

                     STRUCTURE DEFINITIONS

===========================================================================*/
struct IFlip
{
   const AEEVTBL(IFlip) *m_pvt;
   IShell *          m_pIShell;
   uint32            m_dwRefs;
};

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/
//interface functions
static uint32 OEMFLIP_AddRef(IFlip *me);
static uint32 OEMFLIP_Release(IFlip *me);
static int OEMFLIP_QueryInterface(IFlip *me, AEECLSID class, void **ppo);
static int OEMFLIP_GetFlipInfoEx(IFlip *me, AEEFlipInfoEx *pFlipInfoEx);
static int OEMFLIP_GetFlipIDList(IFlip *me, AEEFlipInfo *pFlipInfoList, int16 *pnCount);
static int OEMFLIP_GetFlipPosition(IFlip *me, AEEFlipInfo *pFlipInfo, int nPositionDesired);
static int OEMFLIP_SetParm(IFlip *me, AEEFlipInfo *pFlipInfo, int16 nParmID, int32 p1, int32 p2);
static int OEMFLIP_GetParm(IFlip *me, AEEFlipInfo *pFlipInfo, int16 nParmID, int32 *pP1, int32 *pP2);
int OEMFLIP_New(IShell *piShell, AEECLSID cls, void **ppif);


//private functions
boolean Flip_CheckCountArg(IFlip *me, int16 *pnCount);

//device specific functions
FLIP_EXTERN boolean Flip_CheckFlipIDArg(int nFlipID);
FLIP_EXTERN int Flip_GetFlipInfoEx(IFlip *me, AEEFlipInfoEx *pFlipInfoEx);
FLIP_EXTERN int Flip_GetFlipIDList(IFlip *me, AEEFlipInfo *pFlipInfoList, int16 *pnCount);
FLIP_EXTERN int Flip_GetFlipMinPosition(IFlip *me, AEEFlipInfo *pFlipInfo);
FLIP_EXTERN int Flip_GetFlipMaxPosition(IFlip *me, AEEFlipInfo *pFlipInfo);
FLIP_EXTERN int Flip_GetFlipCurrentPosition(IFlip *me, AEEFlipInfo *pFlipInfo);
FLIP_EXTERN int Flip_IsParmSupported(IFlip *me, AEEFlipInfo *pFlipInfo, int32 nParmIDForSupport, boolean *pbIsSupport);
FLIP_EXTERN int Flip_GetDisplayList(IFlip *me, AEECLSID *pCLSIDList, int16 *pnCount);
FLIP_EXTERN int Flip_GetKeyList(IFlip *me, AVKType *pKeyList, int16 *pnCount);
FLIP_EXTERN boolean Flip_IsParmFlipSpecific(int16 nParmID);
FLIP_EXTERN int Flip_CheckFlipParm(AEEFlipInfo * pFlipInfo, int16 nParmID);

//functions required only for simulator  build
#ifdef AEE_SIMULATOR
extern int Flip_ReadDeviceDataEntries();
extern int Flip_GetNumFlipsFromDeviceDataEntry();
extern int Flip_ReleaseDeviceDataEntries();
#endif
/*===========================================================================

                           VTBL

===========================================================================*/
static const VTBL(IFlip) vtFlip = {
   OEMFLIP_AddRef,
      OEMFLIP_Release,
      OEMFLIP_QueryInterface,
      OEMFLIP_GetFlipInfoEx,
      OEMFLIP_GetFlipIDList,
      OEMFLIP_GetFlipPosition,
      OEMFLIP_SetParm,
      OEMFLIP_GetParm
};

/*===========================================================================

                           FUNCTION IMPLEMENTATION

===========================================================================*/
static uint32 OEMFLIP_AddRef(IFlip *me)
{
   me->m_dwRefs++;
   return me->m_dwRefs;
}

static uint32 OEMFLIP_Release(IFlip *me)
{
   me->m_dwRefs--;
   if (!me->m_dwRefs) {

#ifdef AEE_SIMULATOR
      Flip_ReleaseDeviceDataEntries();
#endif
      // Release interfaces
      ISHELL_Release(me->m_pIShell);
      FREE(me);
      return 0;
   }

   return me->m_dwRefs;
}

static int OEMFLIP_QueryInterface(IFlip *me, AEECLSID class, void **ppo)
{
   switch (class) {
      case AEECLSID_FLIP:
      case AEEIID_IFLIP:
         *ppo = me;
         OEMFLIP_AddRef((IFlip*)(*ppo));
         return SUCCESS;

      default:
         *ppo = 0;
         return ECLASSNOTSUPPORT;
   }
}

static int OEMFLIP_GetFlipInfoEx(IFlip *me, AEEFlipInfoEx *pFlipInfoEx)
{
   int nFlipID = INVALID_FLIP_ID;
   if (!me || !pFlipInfoEx)
   {
      return EBADPARM;
   }
   nFlipID = DEVNOTIFY_GET_ID(pFlipInfoEx->flipInfo);
   if (!Flip_CheckFlipIDArg(nFlipID))
   {
      return EUNSUPPORTED;
   }
   return Flip_GetFlipInfoEx(me, pFlipInfoEx);
}

static int OEMFLIP_GetFlipIDList(IFlip *me, AEEFlipInfo *pFlipInfoList, int16 *pnCount)
{
   if (!Flip_CheckCountArg(me,pnCount))
   { 
      return EBADPARM;
   }
   return Flip_GetFlipIDList(me, pFlipInfoList, pnCount);
}

static int OEMFLIP_GetFlipPosition(IFlip *me, AEEFlipInfo *pFlipInfo, int nPositionDesired)
{
   int nFlipID = INVALID_FLIP_ID;
   if ( (!me) || (!pFlipInfo))
   {
      return EBADPARM;
   }
   if ( (FLIP_POSITION_CURRENT != nPositionDesired) && (FLIP_POSITION_MIN != nPositionDesired) &&
        (FLIP_POSITION_MAX != nPositionDesired) )
   {
      return EBADPARM;
   }
   nFlipID = DEVNOTIFY_GET_ID(*pFlipInfo);
   if (!Flip_CheckFlipIDArg(nFlipID))
   {
      return EUNSUPPORTED;
   }
   switch (nPositionDesired)
   {
   case FLIP_POSITION_MIN:
      {
         return Flip_GetFlipMinPosition(me, pFlipInfo);
      }
   case FLIP_POSITION_MAX:
      {
         return Flip_GetFlipMaxPosition(me, pFlipInfo);
      }
   case FLIP_POSITION_CURRENT:
      {
         return Flip_GetFlipCurrentPosition(me, pFlipInfo);
      }
   default:
      {
         return EBADPARM;
      }
   } //switch (nPositionDesired)
   return EFAILED;
}

static int OEMFLIP_SetParm(IFlip * me, AEEFlipInfo * pFlipInfo, int16 nParmID, int32 p1, int32 p2)
{
   if (!me)
   {
      return EBADPARM;
   }
   //no SetParm support for any parameter, currently
   return EUNSUPPORTED;
}

static int OEMFLIP_GetParm(IFlip * me, AEEFlipInfo * pFlipInfo, int16 nParmID, int32 *pP1, int32 * pP2)
{
   if (!me)
   {
      return EBADPARM;
   }

   //pFlipInfo check 
   if (FLIP_PARM_IS_SUPPORT != nParmID) //pFlipInfo check is done later on for FLIP_PARM_IS_SUPPORT, in Flip_IsParmSupported()
   {
      int nRetVal = EFAILED;
      nRetVal = Flip_CheckFlipParm(pFlipInfo, nParmID);
      if (SUCCESS != nRetVal)    
      {
         return nRetVal;
      }
   }

   switch (nParmID)
   {
   case FLIP_PARM_IS_SUPPORT:
      {
         if (!pP2)
         {
            return EBADPARM;
         }
         return Flip_IsParmSupported(me, pFlipInfo, (int32) pP1, (boolean *)pP2);
      } //case FLIP_PARM_IS_SUPPORT
   case FLIP_PARM_DISPLAYLIST:
      {
         return Flip_GetDisplayList(me, (AEECLSID *) pP1, (int16 *)pP2);
      }
   case FLIP_PARM_KEYLIST:
      {
         return Flip_GetKeyList(me, (AVKType *) pP1, (int16 *)pP2);
      }
   default:
      {
         return EBADPARM;
      }
   }
   return EFAILED;
}

int OEMFLIP_New(IShell* piShell, AEECLSID cls, void **ppif)
{
   IFlip *me;

#ifdef AEE_SIMULATOR
   if (SUCCESS != Flip_ReadDeviceDataEntries())
   {
      return EFAILED;
   }
   if (0 == Flip_GetNumFlipsFromDeviceDataEntry())
   {
      return EUNSUPPORTED;
   }
#endif //AEE_SIMULATOR



   me = (IFlip *)MALLOC(sizeof(IFlip));
   if ((IFlip *) 0 == me)
   {
      return ENOMEMORY;
   }
   me->m_pvt = &vtFlip;
   me->m_dwRefs = 1;
   
   me->m_pIShell = piShell;
   ISHELL_AddRef(piShell);
  
   *ppif = (void *)me;

   return SUCCESS;
}
/*===========================================================================

                        PRIVATE FUNCTION IMPLEMENTATION

===========================================================================*/
boolean Flip_CheckCountArg(IFlip* me, int16 * pnCount)
{
   if ( (!me) || (!pnCount) )
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}


/*===========================================================================

                        DEVICE SPECIFIC FUNCTION IMPLEMENTATION

===========================================================================*/
#ifndef AEE_SIMULATOR
boolean Flip_CheckFlipIDArg(int nFlipID)
{
   //check for the flip ID being valid, here
   //return TRUE if the ID is valid, FALSE otherwise
   return TRUE;
}

int Flip_GetFlipInfoEx(IFlip *me, AEEFlipInfoEx *pFlipInfoEx)
{
   //TODO: fill in pFlipInfoEx here
   return EFAILED;

}

int Flip_GetFlipIDList(IFlip *me, AEEFlipInfo *pFlipInfoList, int16 *pnCount)
{
   //TODO: set pnCount to number of flips on device.
   //if pFlipInfoList if not NULL and if pnCount < number of flips on device, return EBUFFERTOOSMALL
   //otherwise, populate pFlipInfoList and return SUCCESS.
   return EFAILED;
}


int Flip_GetFlipMinPosition(IFlip *me, AEEFlipInfo *pFlipInfo)
{
   //TODO: populate pFlipInfo with min position of flip
   return EFAILED;
}

int Flip_GetFlipMaxPosition(IFlip *me, AEEFlipInfo *pFlipInfo)
{
   //TODO: populate pFlipInfo with max position of flip
   return EFAILED;

}
int Flip_GetFlipCurrentPosition(IFlip *me, AEEFlipInfo *pFlipInfo)
{
   //TODO: populate pFlipInfo with current position of flip
   return EFAILED;
}

int Flip_IsParmSupported(IFlip * me, AEEFlipInfo * pFlipInfo, int32 nParmIDForSupport, boolean * pbIsSupport)
{
   int nRetVal = EFAILED;
   nRetVal = Flip_CheckFlipParm(pFlipInfo, nParmIDForSupport);
   if (SUCCESS != nRetVal)
   {
      return nRetVal;
   }
   *pbIsSupport = FALSE;

   switch (nParmIDForSupport)
   {
   case FLIP_PARM_IS_SUPPORT:
      {
         //FLIP_PARM_IS_SUPPORT is supported
         *pbIsSupport = TRUE;
         return SUCCESS;
      }
   case FLIP_PARM_KEYLIST:
      {
         //TODO: set *pbIsSupport to TRUE if FLIP_PARM_KEYLIST is supported
         return SUCCESS;
      }
   case FLIP_PARM_DISPLAYLIST:
      {
         //TODO: set *pbIsSupport to TRUE if FLIP_PARM_DISPLAYLIST is supported
         return SUCCESS;
      }
   default:
      {
         return EBADPARM;
      }
   } //switch (nParmIDForSupport)
}

int Flip_GetDisplayList(IFlip *me,AEECLSID *pCLSIDList, int16 *pnCount)
{
   if (!Flip_CheckCountArg(me, pnCount))
   {
      return EBADPARM;
   }
   //TODO: set *pnCount to number of displays accessible
   //if pCLSIDList is not NULL, and if *pnCount < number of displays accessible, return EBUFFERTOOSMALL
   //otherwise, populate pCLSIDList and return SUCCESS
   return EFAILED;
}

int Flip_GetKeyList(IFlip * me, AVKType * pKeyList, int16 *pnCount)
{
   if (!Flip_CheckCountArg(me, pnCount))
   {
      return EBADPARM;
   }

   //TODO: set *pnCount to number of keys accessible
   //if pKeyList is not NULL, and if *pnCount < number of keys accessible, return EBUFFERTOOSMALL
   //otherwise, populate pKeyList and return SUCCESS
   return EFAILED;

}

boolean Flip_IsParmFlipSpecific(int16 nParmID)
{
   if ( (FLIP_PARM_IS_SUPPORT) || (FLIP_PARM_KEYLIST == nParmID ) || (FLIP_PARM_DISPLAYLIST == nParmID) )
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}

int Flip_CheckFlipParm(AEEFlipInfo * pFlipInfo, int16 nParmID)
{
   int nFlipID = INVALID_FLIP_ID;

   if (TRUE == Flip_IsParmFlipSpecific(nParmID))//parm is flip specific
   {
      if (NULL == pFlipInfo) //flip ID should be present for flip-specific parms
      {
         return EBADPARM;
      }
      else //flip ID is present, check it for validity
      {
         nFlipID = DEVNOTIFY_GET_ID(*pFlipInfo);
         if (!Flip_CheckFlipIDArg(nFlipID))
         {
            return EUNSUPPORTED;
         }
      }
   }
   else //parm is not flip specific, flip ID should be NULL
   {
      if (NULL != pFlipInfo)
      {
         return EBADPARM;
      }
   }
   return SUCCESS;
}
#endif

