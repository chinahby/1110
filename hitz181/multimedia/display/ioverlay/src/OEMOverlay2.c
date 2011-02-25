/*======================================================
FILE:  OEMOverlay2.c

SERVICES:  OEM functions for IOverlay interface in terms of IMMOverlay.

GENERAL DESCRIPTION:
   This provides a reference implementation for the IOverlay
   interface in terms of IMMOverlay.

        Copyright 2006, 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlay2.c#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $

=====================================================*/
#include "OEMFeatures.h"
#include "IOverlay2.h"
#include "AEE.h"
#include "AEEMMOverlay.h"
#include "AEEBase.h"
#include "OEMOverlayHelper.h"
#include "AEE_OEMComdef.h"
#ifdef RELEASEIF
   #undef RELEASEIF
#endif
#define RELEASEIF(p) \
   if (p != NULL) { IBASE_Release((IBase*)(p)); (p) = NULL; }


typedef struct IOverlay2
{
   const AEEVTBL(IOverlay2) *pvt;// Function Table
   IMMOverlay *pMMOverlay;
   IShell *pShell;
   int bCleanup;
   int dwRefs;
   int dwOwnerRefs;
} _IOverlay2;



void TranslateOverlayUnion(OverlayParamType paramID, OverlayTypeUnion2 *pP1_2, OverlayTypeUnion2 *pP2_2,uint32 *pMM1, uint32 * pMM2, int direction);
OverlayParamType TranslateOverlayParamType(OverlayParamType2 paramID2);
void TranslateOverlayCapabilitiesType(OverlayParamType2 paramID2, OverlayCapabilitiesType *pMMType, 
                                      OverlayCapabilitiesType2 *pType2, int nSize, int direction);

typedef struct
{
   OverlayParamType2 paramID;
   boolean bCanSet;
   OverlayUnionMemberType2 param1Type;
   OverlayUnionMemberType2 param2Type;
   uint8 privs;
} OverlayHelper_ParamInfo2;


/*==============================================================================
INTERFACE FUNCTIONS
==============================================================================*/
/*==============================================================================

IMMOVERLAY_AddRef()

Description:

This function increments the reference count of the IMMOverlay interface object,
allowing the interface to be shared by multiple callers.  The object is freed
when the reference count reaches 0 (zero).

Prototype:

uint32 IMMOVERLAY_AddRef(IMMOverlay *p);

Parameters:
===pre>
p:  Pointer to the IMMOverlay interface object
===/pre>

Return Value:
Returns the incremented reference count for the IMMOverlay interface object.

Comments:
None

Side Effects:
None

See Also:
IMMOVERLAY_Release()

==============================================================================*/
static uint32 Overlay2_AddRef(IOverlay2 *pMe)
{
   return ++pMe->dwRefs;
}

static void Overlay2_Delete(IOverlay2 *pMe)
{

   RELEASEIF(pMe->pMMOverlay);
   FREE(pMe);
}

/*==============================================================================

IMMOVERLAY_Release()

Description:

This function decrements the reference count of the IMMOverlay interface object. 
The object is freed from memory and is no longer valid once its reference count
reaches 0 (zero).

Prototype:

uint32 IMMOVERLAY_Release(IMMOverlay *p);

Parameters:
===pre>
p:  Pointer to the IMMOverlay interface object
===/pre>

Return Value:
Returns the decremented reference count for the IMMOverlay interface object.  If
the returned reference count is 0 (zero), the object has been freed and will no
longer be valid.

Comments:
None

Side Effects:
None

See Also:
IMMOVERLAY_AddRef()

==============================================================================*/
static uint32 Overlay2_Release(IOverlay2 *pMe)
{
   uint32 dwRefs;
   dwRefs = --pMe->dwRefs;
   if (dwRefs == 0)
   {
      Overlay2_Delete(pMe);
   }
   return(dwRefs);
}


/*==============================================================================

IMMOVERLAY_QueryInterface()

Description: 
This function asks an object for another API from the object in question.

Prototype:

int IMMOVERLAY_QueryInterface(IMMOverlay *p, AEECLSID c, void **d)

Parameters:
===pre>
p:  Pointer to the IMMOverlay interface object.
c:  Requested class ID exposed by the object
d:  Pointer to the returned IMMOverlayMgr interface object.
===/pre>

Return Value:
SUCCESS          - Interface found
ENOMEMORY        - Insufficient memory
ECLASSNOTSUPPORT - Requested interface is unsupported

Comments:  
The IMMOverlay pointer in 'p' is set to the new interface (with refcount positive)
, or NULL if the ClassID is not supported by the object.  'p' must _not_ be set
to NULL.

Side Effects: 
None

See Also:
IQI_QueryInterface()


==============================================================================*/
static int Overlay2_QueryInterface(IOverlay2 *pif, AEECLSID id, void **ppo)
{
   switch (id)
   {
      case AEECLSID_QUERYINTERFACE:
      case AEEIID_OVERLAY2:
         *ppo = (void*)pif;
         IQI_AddRef(*ppo);
         return SUCCESS;
      default:
         break;
   }

   /* Default case: We don't know what they mean by their classid */
   return ECLASSNOTSUPPORT;
} 


OverlayHelper_ParamInfo2 * Overlay2Helper_GetParamInfo(OverlayParamType2 paramID)
{
   static const OverlayHelper_ParamInfo2 gParamInfo[OV2_PARAM_MAX_VALUE] = 
   {
      {0                         , FALSE, OV2_UNION_TYPE_NOTYPE, OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_BITMAP            , TRUE , OV2_UNION_TYPE_PTR   , OV2_UNION_TYPE_NOTYPE},  
      {OV2_PARM_SRC_REGION        , TRUE , OV2_UNION_TYPE_PTR   , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_DEST_REGION       , TRUE , OV2_UNION_TYPE_PTR   , OV2_UNION_TYPE_UINT},
      {OV2_PARM_DISPLAY           , TRUE , OV2_UNION_TYPE_CLSID , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_LAYER             , TRUE , OV2_UNION_TYPE_LAYER , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_TRANSPARENCY      , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_RGB},
      {OV2_PARM_ALPHA_BLEND_PCT   , TRUE , OV2_UNION_TYPE_FLOAT , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_LETTER_BOX        , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_RGB},
      {OV2_PARM_ENABLE            , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_AUTO_DISABLE      , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_ROTATE            , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_REFLECT           , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_SCALE_X           , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_FLOAT},
      {OV2_PARM_SCALE_Y           , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_FLOAT},
      {OV2_PARM_OWNER             , FALSE, OV2_UNION_TYPE_CLSID , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_GROUPID           , TRUE , OV2_UNION_TYPE_CLSID , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_OVERLAYID         , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_NOTYPE},
      {OV2_PARM_DEST_REGION_SCREEN, FALSE, OV2_UNION_TYPE_PTR   , OV2_UNION_TYPE_UINT},
      {OV2_PARM_VSYNC             , TRUE , OV2_UNION_TYPE_UINT  , OV2_UNION_TYPE_NOTYPE},
   };

   return(OverlayHelper_ParamInfo2 *)&gParamInfo[paramID];
}



/*
================================================================================

FILE:  OEMOverlay.c

SERVICES: Provides overlay related services.


================================================================================

Copyright © 1999-2005 QUALCOMM Incorporated 
All Rights Reserved.
QUALCOMM Proprietary

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlay2.c#2 $

================================================================================
*/
/*lint -save -e611 -e740 -e545 -esym(751,_IOverlay)*/


//Function called to add a listener to the list
static int IOverlay2_AddListener( IOverlay2 *pMe, ModelListener *pListener )
{
   return EUNSUPPORTED;

}

/*==============================================================================

IOVERLAY_GetCapabilities()

Description: 
This function retrieves the list of possible values for a parameter to 
IOVERLAY_SetParam. These values represent what the given overlay supports at the
current moment. The values that are supported can change at any time.

Prototype:

int IOVERLAY_GetCapabilities(IOverlay2 *p, OverlayParamType paramID,
                      uint32 * pSize, OverlayCapabilitiesType * pBuff);

Parameters:
===pre>
p        :  Pointer to the IOverlay2 interface object.
paramID  :  Parameter we are interested in.
pSize    :  Pointer to a variable the will return the size of the buffer needed.
pBuff    :  Pointer to a buffer to fill the possible values in. If this is NULL
         pSize will be set, but the buffer will not be used.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IOverlay2 supported capability parameters.

Side Effects: 
None

See Also:
IOVERLAYMGR_GetCapabilities()
IOVERLAY_SetParam()
IOVERLAY_GetParam()

==============================================================================*/
/*lint -save -esym(715, pMe)*/
static int Overlay2_GetCapabilities(IOverlay2 *pMe, OverlayParamType2 paramID2,
                                    uint32 * pSize,
                                    OverlayCapabilitiesType2 * pBuff2)
{
   int nResult=0, nErr=0;
   OverlayParamType paramID;
   OverlayCapabilitiesType* pcapType=NULL;
   int32 nSize= 0;
   paramID = TranslateOverlayParamType(paramID2);

   nErr = OverlayHelper_GetCapabilities(paramID, &nSize, NULL, NULL);
   if (nErr == SUCCESS)
   {
      pcapType = (OverlayCapabilitiesType *) MALLOC(nSize);
      if (pcapType == NULL)
      {
         nErr = ENOMEMORY;
      }
      nResult = IMMOVERLAY_GetCapabilities(pMe->pMMOverlay,  paramID,  &nSize, pcapType);
      TranslateOverlayCapabilitiesType(paramID2, pcapType, pBuff2,nSize, 0);

      //adjust size here
   }
   return nResult;
}
/*lint -restore*/


/*==============================================================================

IOVERLAY_SetParam()

Description: 
This function sets specified the IOverlay2 Interface object parameters. 

Prototype:

int IOVERLAY_SetParam(IOverlay2 *p, OverlayParamType paramID,
                 OverlayTypeUnion p1, OverlayTypeUnion p2);

Parameters:
===pre>
p        :  Pointer to the IOverlay2 interface object.
paramID  :  Parameter we are interested in.
p1       :  Depends on paramID.
p2       :  Depends on paramID.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IOverlay2 supported settable parameters.

Side Effects: 
If the values that are specified are valid, but can not be set at the current
time, the closest value that is possible will be used and the listeners
specified in the IModel interface of IOverlay2 will be called.

See Also:
IOVERLAY_GetCapabilities()
IOVERLAY_GetParam()

==============================================================================*/
static int Overlay2_SetParam(IOverlay2 *pMe, OverlayParamType2 paramID2,
                             OverlayTypeUnion2 p1_2, OverlayTypeUnion2 p2_2,
                             uint8 nSetType)
{
   uint32 p1, p2;
   OverlayParamType paramID;
   paramID = TranslateOverlayParamType(paramID2);
   TranslateOverlayUnion(paramID2, &p1_2,&p2_2,&p1,&p2, 0);
   if (paramID == 0xFFFFFFFF || p1 ==0xFFFFFFFF || p2 ==0xFFFFFFFF)
      return EFAILED;
   return IMMOVERLAY_SetParam(pMe->pMMOverlay,  paramID, p1,  p2);
}


/*==============================================================================

IOVERLAY_GetParam()

Description: 
This function gets specified the IOverlay2 Interface object parameters. 

Prototype:

int IOVERLAY_GetParam(IOverlay2 *p, OverlayParamType paramID,
                 OverlayTypeUnion * pP1, OverlayTypeUnion * pP2);

Parameters:
===pre>
p        :  Pointer to the IOverlay2 interface object.
paramID  :  Parameter we are interested in.
pP1      :  Depends on paramID.
pP2      :  Depends on paramID.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
The table in "comments section of OverlayParamType" documentation gives the list
of IOverlay2 supported gettable parameters.

Side Effects: 
None

See Also:
IOVERLAY_GetCapabilities()
IOVERLAY_SetParam()

==============================================================================*/
static int Overlay2_GetParam(IOverlay2 *pMe, OverlayParamType2 paramID2,
                             OverlayTypeUnion2 * pP1_2, OverlayTypeUnion2 * pP2_2)
{
   uint32 p1, p2;
   int nRetval;
   OverlayParamType paramID;
   paramID = TranslateOverlayParamType(paramID2);
   nRetval = IMMOVERLAY_GetParam(pMe->pMMOverlay,  paramID, &p1, &p2);
   TranslateOverlayUnion(paramID2, pP1_2, pP1_2, &p1,&p2, 1);
   return nRetval;
}

/*==============================================================================

IOVERLAY_Update()

Description: 
This function updates the IOverlay2 on the screen. 

Prototype:

int IOVERLAY_Update(IOverlay2 *p, AEERect rect);

Parameters:
===pre>
p        :  Pointer to the IOverlay2 interface object.
rect     :  Rectangle that describes the dirty rectangle.
If the dx/dy values are 0 the entire Overlay will be redrawn.
These coordinates are relative to the Bitmap that is passed in.
===/pre>

Return Value:
SUCCESS          - Operation successful
EBADPARM         - One or more of the parameters passed in is not valid
EFAILED          - Generic failure

Comments:  
None.

Side Effects: 
None.

See Also:
None.

==============================================================================*/
static int Overlay2_Update(IOverlay2 *pMe, AEERect dirtyRect)
{
   return IMMOVERLAY_Update( pMe->pMMOverlay, &dirtyRect);
}

/*
================================================================================

IOVERLAY_UpdateAsync()

Description: 
   This function updates the IOverlay2 asynchronously on the screen. 

Prototype:

   uint32 IOVERLAY_Update(IOverlay2 *p, AEERect rect, uint32 dwUser);

Parameters:
===pre>
   p        :  Pointer to the IOverlay2 interface object.
   rect     :  Rectangle that describes the dirty rectangle.
            If the dx/dy values are 0 the entire Overlay will be redrawn.
            These coordinates are relative to the Bitmap that is passed in.
   dwUser   :  A user specified value. This will only be passed to the listener
            event for this update.
===/pre>

Return Value:
   A unique ID assigned for this update. If this value is 0xFFFFFFFF an error
   occured and the update will not happen.

Comments:  
   None.

Side Effects: 
   This call will result cause the listeners specified in the 
   IModel interface of IOverlay2 will be called with the event 
   EVT_MDL_OV_ASYNC_UPDATE_COMPLETE.

See Also:
   IOverlay2_AddListener

================================================================================
*/

static uint32 Overlay2_UpdateAsync(IOverlay2 *pMe, AEERect dirtyRect, uint32 dwUser)
{
   return IMMOVERLAY_UpdateAsync(pMe->pMMOverlay, &dirtyRect, dwUser);
}

/*==============================================================================

IOVERLAY_Clear()

Description: 
This function clears the IOverlay2 from the screen. 

Prototype:

int IOVERLAY_Clear(IOverlay2 *p);

Parameters:
===pre>
p        :  Pointer to the IOverlay2 interface object.
===/pre>

Return Value:
SUCCESS          - Operation successful
EFAILED          - Generic failure

Comments:  
None.

Side Effects: 
None.

See Also:
None.

==============================================================================*/
static int Overlay2_Clear(IOverlay2 *pMe)
{
   return IMMOVERLAY_Clear(pMe->pMMOverlay);
}

OverlayParamType TranslateOverlayParamType(OverlayParamType2 paramID2)
{
   OverlayParamType newval = 0xFFFFFFFF; 

   switch (paramID2)
   {
      case OV2_PARM_BITMAP: // 1
         newval =  OV_PARM_BITMAP;//          0x0103ab72
         break;

      case OV2_PARM_SRC_REGION: // 2 
         newval = OV_PARM_SRC_REGION ;//     0x0103ab73
         break;

      case OV2_PARM_DEST_REGION : // 3
         newval = OV_PARM_DEST_REGION ;//    0x0103ab74
         break;

      case OV2_PARM_DISPLAY : // 4
         newval =  OV_PARM_DISPLAY;//         0x0103ab75
         break;

      case OV2_PARM_LAYER: // 5
         newval = OV_PARM_LAYER ; //          0x0103ab76
         break;

      case OV2_PARM_TRANSPARENCY: // 6
         newval =  OV_PARM_TRANSPARENCY ; //    0x0103ab77
         break;

      case OV2_PARM_ALPHA_BLEND_PCT: // 7
         newval = OV_PARM_ALPHA_BLEND_PCT ; //0x0103ab78
         break;

      case OV2_PARM_LETTER_BOX: // 8 ??????????????
         newval = 0xFFFFFFFF;
         break;

      case OV2_PARM_ENABLE: // 9
         newval = OV_PARM_ENABLE  ; //        0x0103ab7a
         break;


      case OV2_PARM_AUTO_DISABLE: // 10
         newval =  OV_PARM_AUTO_DISABLE ; //   0x0103ab7b
         break;

      case OV2_PARM_ROTATE : //11
         newval = OV_PARM_ROTATE  ; //        0x0103ab7c
         break;

      case OV2_PARM_REFLECT: //12
         newval = OV_PARM_REFLECT   ; //      0x0103ab7d
         break;

      case OV2_PARM_SCALE_X : // 13
         newval = OV_PARM_SCALE_X ; //        0x0103ab7e
         break;

      case OV2_PARM_SCALE_Y : //14
         newval =  OV_PARM_SCALE_Y  ; //       0x0103ab7f
         break;

         ////////////////////////////////////////////
      case OV2_PARM_OWNER  : //         15
         newval=0xFFFFFFFF;
         break;

      case OV2_PARM_GROUPID: //       16
         newval=0xFFFFFFFF;
         break;

      case OV2_PARM_OVERLAYID: //       17
         newval=0xFFFFFFFF;
         break;

      case OV2_PARM_DEST_REGION_SCREEN: // 18
         newval = OV_PARM_DEST_REGION_SCREEN ; //0x0103ab80
         break;

      case OV2_PARM_VSYNC : //          19
         newval = OV_PARM_VSYNC   ; //        0x0103ab81
         break;

      case OV2_PARM_BACK_BITMAP: //     20
         newval=0xFFFFFFFF;
         break;

      case OV2_PARM_NO_AUTOCLEAR: //    21
         newval=OV_PARM_NO_AUTOCLEAR;
         break;

      case OV2_PARAM_MAX_VALUE: //      22
         newval=0xFFFFFFFF;
         break;
   }
   return newval;

}
void TranslateOverlayUnion(OverlayParamType paramID, OverlayTypeUnion2 *pP1_2, OverlayTypeUnion2 *pP2_2,uint32 *pMM1, uint32 * pMM2, int direction)
{


   OverlayHelper_ParamInfo2 *pInfo=NULL;
   pInfo = Overlay2Helper_GetParamInfo(paramID);

   switch (pInfo->param1Type)
   {
      case OV2_UNION_TYPE_UINT:
         if (!direction)
            *pMM1 = pP1_2->uint_data;
         else
            pP1_2->uint_data = *pMM1;
         break;

      case OV2_UNION_TYPE_FLOAT:
         if (!direction)
            *pMM1 = pP1_2->float_data*256.0;
         else
            pP1_2->float_data = *pMM1/256.0;

         break;
      case OV2_UNION_TYPE_PTR:
         if (!direction)
            *pMM1 = (uint32 )pP1_2->ptr_data;
         else
            pP1_2->ptr_data = (void*)*pMM1;

         break;
      case OV2_UNION_TYPE_LAYER:
         if (!direction)
            *pMM1 = pP1_2->layer_data;
         else
            pP1_2->layer_data = *pMM1;

         break;
      case OV2_UNION_TYPE_RGB:
         if (!direction)
            *pMM1 = pP1_2->rgb_data;
         else
            pP1_2->rgb_data = *pMM1;

         break;
      case OV2_UNION_TYPE_CLSID:
         if (!direction)
            *pMM1 = pP1_2->clsid_data;
         else
            pP1_2->clsid_data = *pMM1;

         break;
      default:
         break;
   }

   switch (pInfo->param2Type)
   {
      case OV2_UNION_TYPE_UINT:
         if (!direction)
            *pMM2 = pP2_2->uint_data;
         else
            pP2_2->uint_data = *pMM2;
         break;

      case OV2_UNION_TYPE_FLOAT:
         if (!direction)
            *pMM2 = pP2_2->float_data*256.0;
         else
            pP2_2->float_data = *pMM2/256.0;

         break;
      case OV2_UNION_TYPE_PTR:
         if (!direction)
            *pMM2 = (uint32)pP2_2->ptr_data;
         else
            pP2_2->ptr_data = (void*)*pMM2;

         break;
      case OV2_UNION_TYPE_LAYER:
         if (!direction)
            *pMM2 = pP2_2->layer_data;
         else
            pP2_2->layer_data = *pMM2;

         break;
      case OV2_UNION_TYPE_RGB:
         if (!direction)
            *pMM2 = pP2_2->rgb_data;
         else
            pP2_2->rgb_data = *pMM2;

         break;
      case OV2_UNION_TYPE_CLSID:
         if (!direction)
            *pMM2 = pP2_2->clsid_data;
         else
            pP2_2->clsid_data = *pMM2;

         break;
      default:
         break;
   }

}

void TranslateOverlayCapabilitiesType(OverlayParamType2 paramID2, OverlayCapabilitiesType *pMMType, 
                                      OverlayCapabilitiesType2 *pType2, int nSize, int direction)
{
   MEMCPY(pType2, pMMType, nSize);
}
/**********************/
IMMOverlay * Overlay_Create(IShell * pShell);

IOverlay2 * Overlay2_Create(IShell * pShell)
{
   IOverlay2 *pNew;

   static const VTBL(IOverlay2) Overlay2Funcs = 
   {
      Overlay2_AddRef,
      Overlay2_Release,
      Overlay2_QueryInterface,
      Overlay2_GetCapabilities,
      Overlay2_SetParam,
      Overlay2_GetParam,
      Overlay2_Update,
      Overlay2_Clear,
      IOverlay2_AddListener,
      Overlay2_UpdateAsync
   };

   pNew = MALLOCREC(IOverlay2);

   if (!pNew)
   {
      DBGPRINTF_FATAL("Error MALLOC FAILED");
      return NULL;
   }

   //Clear the record
   MEMSET(pNew, 0, sizeof(IOverlay2));

   pNew->pvt = &Overlay2Funcs;


   pNew->dwRefs = 1;


   pNew->pMMOverlay = Overlay_Create(pShell);

   pNew->bCleanup = TRUE;
   return pNew;
}



/*
   MMOverlay
typedef struct
{
   uint16 wFlags;
   uint32 dwValues;
   OverlayMemberType OverlayUnionMember;//uint8
   "uint32 * pdwParams;"
} OverlayCapabilitiesSubType;

typedef struct
{
   uint16 wParams;
   OverlayCapabilitiesSubType paramInfo[1];
} OverlayCapabilitiesType;

typedef struct
{
   uint16 wFlags;
   uint32 dwValues;
   OverlayUnionMemberType2 OverlayUnionMember2;//uint8
   "OverlayTypeUnion2 * pdwParams2;"
} OverlayCapabilitiesSubType2;

typedef struct
{
   uint16 wParams;
   OverlayCapabilitiesSubType2 paramInfo2[1];
} OverlayCapabilitiesType2;

   
   TranslateSetType()
   {
   
   //This value will be used
    case OV2_SETTYPE_HARD: //  0x0000
    
   //This value will be used unless we have an OV2_SETTYPE_HARD
   
    case OV2_SETTYPE_SOFT: //  0x0001
   
    
   //This causes the state of if an OV2_SETTYPE_HARD to be reset
   
   
    case OV2_SETTYPE_RESET: // 0x0002
   
    
   //This is used to prod the interface, will bypass these checks 
   
   
    case OV2_SETTYPE_SILENT: // 0x0004
   }
   
   
   TranslateEVT_MDL()
   {
   
    
   //Model events and data types
    case EVT_MDL_OV2_PARAMETER_CHANGED: //  EVT_MDL_DEV_START + 0x1000
   newval =  EVT_MDL_OV_PARAMETER_CHANGED; //          (EVT_MDL_DEV_START + 0x1000)
   
    
    case EVT_MDL_OV2_PARAMETER_CHANGED_INDIRECT: //EVT_MDL_DEV_START + 0x1001
   
    
    case EVT_MDL_OV2_PARAMETER_CHANGED _SUGGEST:  //EVT_MDL_DEV_START + 0x1002
   
    
    case EVT_MDL_OV2_CHANGES_COMMITTED: //EVT_MDL_DEV_START + 0x1003
   newval = EVT_MDL_OV_CHANGES_COMMITTED ; //         (EVT_MDL_DEV_START + 0x1003)
   
    
    case EVT_MDL_OV2_ASYNC_UPDATE_COMPLETE: // EVT_MDL_DEV_START + 0x1004
   
    newval = EVT_MDL_OV_ASYNC_UPDATE_COMPLETE ; //      (EVT_MDL_DEV_START + 0x1004)
   }
   
      }
   
                                    
   
   Translate OverlayCapabilitiesType2
   
      int        (*SetParam)        (iname *p, OverlayParamType2 paramID2, \
                                    OverlayTypeUnion2 p12, \
                                    OverlayTypeUnion2 p22, \
                                    uint8 nSetType); \
   
   
   
   
   
   uint32 TranslateValues (uint32 overlay2_value)
   
   //needs to be divided into subsections
   {
   
   uint32 newval;
   
   switch(overlay2_value)
   {
   //#define AEEIID_OVERLAYMGR        0x0102f8f5
   //#define AEEIID_OVERLAY    	      0x0102edf1
   //#define AEEIID_OVERLAY_DIRECT    0x0102edf2
   //#define NOT_MATCH_ENABLED  0x0001  //Do not match Enabled Overlays
   //#define NOT_MATCH_DISABLED 0x0002  //Do not match Disabled Overlays
   
   
   
   
   TranslateOVLayer()
   {
   //Overlay Layer Type
     case OV2_LAYER_BASE_LAYER://0x00
          newval =OV_LAYER_BASE_LAYER;// 0x00
          
     case OV2_LAYER_TOP_LAYER://0xff
          newval = OV_LAYER_TOP_LAYER;// 0xFF
          break;
   }
   
   TranslateOVUnion(OverlayTypeUnion *pOT, OverlayTypeUnion *pOT2)
   {
   
      pOT->
      case OV2_UNION_TYPE_UINT:// 0
      newval = OV_UNION_TYPE_UINT;
      break;
      
      case OV2_UNION_TYPE_FLOAT:// 1 
      newval = OV_UNION_TYPE_FIXEDPOINT;
      break;
      
      case OV2_UNION_TYPE_PTR://  2
      newval = OV_UNION_TYPE_PTR;
      break;
      
      case OV2_UNION_TYPE_LAYER: //3// 3
      newval = OV_UNION_TYPE_LAYER;
      break;
      
      case OV2_UNION_TYPE_RGB: //4//4
      newval = OV_UNION_TYPE_RGB
      break;
      
      case OV2_UNION_TYPE_CLSID: // 5
      newval = OV_UNION_TYPE_CLSID
      break;
      
      case OV2_UNION_TYPE_NOTYPE: //  6
      newval = OV_UNION_TYPE_NOTYPE;
      break;
    }
   
   
    TranslateOVParamAndFlag()
    {  
      
   //#define OVERLAY_PARAM_CONTINUOUS 0x0000
   //#define OVERLAY_PARAM_DISCRETE   0x0001
   //OverlayParamType definition
   
   
   //OverlayParamType Definition
    case OV2_PARM_BITMAP: // 1
    newval =  OV_PARM_BITMAP;//          0x0103ab72
    break;
    
   
    case OV2_PARM_SRC_REGION: // 2 
   newval = OV_PARM_SRC_REGION ;//     0x0103ab73
   break;
    
   
    case OV2_PARM_DEST_REGION : // 3
   newval = OV_PARM_DEST_REGION ;//    0x0103ab74
   break;
    }
   
    
    /////////////////////////////////////////
    case OV2_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT: // 0x00000000
       newval = OV_FLAG_DEST_REGION_ALIGN_HORIZ_LEFT;//   0x00000000
       break;
   
    case OV2_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT: // 0x00000001
       newval =  OV_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT; //  0x00000001
       break;
   
    case OV2_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER:  //0x00000002
      newval =  OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER; // 0x00000002
      break;
   
   
    ////////////////////////////////////////////////
    case OV2_FLAG_DEST_REGION_ALIGN_VERT_UPPER: //0x00000000
       newval =  OV_FLAG_DEST_REGION_ALIGN_VERT_UPPER; //   0x00000000
       break;
   
   case OV2_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM: // 0x00010000
       newval = OV_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM //  0x00010000
       break;
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
   
   case OV2_FLAG_DEST_REGION_ALIGN_VERT_CENTER : //0x00020000
      newval =  OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER  0x00020000
   
   ///////////////////////////////////
    case OV2_PARM_DISPLAY : // 4
    newval =  OV_PARM_DISPLAY;//         0x0103ab75
   
    case OV2_PARM_LAYER: // 5
    newval = OV_PARM_LAYER ; //          0x0103ab76
   
    case OV2_PARM_TRANSPARENCY: // 6
    newval =  OV_PARM_TRANSPARENCY ; //    0x0103ab77
   
    case OV2_PARM_ALPHA_BLEND_PCT: // 7
    newval = OV_PARM_ALPHA_BLEND_PCT ; //0x0103ab78
   
    case OV2_PARM_LETTER_BOX: // 8 ??????????????
    
    
    case OV2_PARM_ENABLE: // 9
   newval = OV_PARM_ENABLE  ; //        0x0103ab7a
   
    
    
    case OV2_PARM_AUTO_DISABLE: // 10
    newval =  OV_PARM_AUTO_DISABLE ; //   0x0103ab7b
   
    case OV2_PARM_ROTATE : //11
    newval = OV_PARM_ROTATE  ; //        0x0103ab7c
   
    case OV2_PARM_REFLECT: //12
    newval = OV_PARM_REFLECT   ; //      0x0103ab7d
   
    case OV2_FLAG_REFLECT_X_AXIS: //0x00000001
       newval = OV_FLAG_REFLECT_X_AXIS; //    0x00000001
   
    case OV2_FLAG_REFLECT_Y_AXIS : // 0x00010000
       newval = OV_FLAG_REFLECT_Y_AXIS ; //   0x00010000
   
    case OV2_PARM_SCALE_X : // 13
    newval = OV_PARM_SCALE_X ; //        0x0103ab7e
   
    case OV2_PARM_SCALE_Y : //14
    newval =  OV_PARM_SCALE_Y  ; //       0x0103ab7f
   
    case OV2_FLAG_SCALE_TO_FIT: // 
       newval = OV_FLAG_SCALE_TO_FIT ; //     0x00000001
   
    case OV2_FLAG_SCALE_KEEP_ASPECT: //0x00000002
         newval=  OV_FLAG_SCALE_KEEP_ASPECT ; // 0x00000002
   
    case OV2_FLAG_SCALE_NO_CROP :    //0x00000004
         newval = OV_FLAG_SCALE_NO_CROP; //     0x00000004
   
   ////////////////////////////////////////////
    case OV2_PARM_OWNER  : //         15
    
    case OV2_PARM_GROUPID: //       16
    
    case OV2_PARM_OVERLAYID: //       17
    
    case OV2_PARM_DEST_REGION_SCREEN: // 18
      newval = OV_PARM_DEST_REGION_SCREEN ; //0x0103ab80
   
    
    case OV2_PARM_VSYNC : //          19
      newval = OV_PARM_VSYNC   ; //        0x0103ab81
   
    case OV2_PARM_BACK_BITMAP: //     20
    
    case OV2_PARM_NO_AUTOCLEAR: //    21
    
    case OV2_PARAM_MAX_VALUE: //      22
    }
   
   
   TranslateSetType()
   {
   
   //This value will be used
    case OV2_SETTYPE_HARD: //  0x0000
    
   //This value will be used unless we have an OV2_SETTYPE_HARD
   
    case OV2_SETTYPE_SOFT: //  0x0001
   
    
   //This causes the state of if an OV2_SETTYPE_HARD to be reset
   
   
    case OV2_SETTYPE_RESET: // 0x0002
   
    
   //This is used to prod the interface, will bypass these checks 
   
   
    case OV2_SETTYPE_SILENT: // 0x0004
   }
   
   
   TranslateEVT_MDL()
   {
   
    
   //Model events and data types
    case EVT_MDL_OV2_PARAMETER_CHANGED: //  EVT_MDL_DEV_START + 0x1000
   newval =  EVT_MDL_OV_PARAMETER_CHANGED; //          (EVT_MDL_DEV_START + 0x1000)
   
    
    case EVT_MDL_OV2_PARAMETER_CHANGED_INDIRECT: //EVT_MDL_DEV_START + 0x1001
   
    
    case EVT_MDL_OV2_PARAMETER_CHANGED _SUGGEST:  //EVT_MDL_DEV_START + 0x1002
   
    
    case EVT_MDL_OV2_CHANGES_COMMITTED: //EVT_MDL_DEV_START + 0x1003
   newval = EVT_MDL_OV_CHANGES_COMMITTED ; //         (EVT_MDL_DEV_START + 0x1003)
   
    
    case EVT_MDL_OV2_ASYNC_UPDATE_COMPLETE: // EVT_MDL_DEV_START + 0x1004
   
    newval = EVT_MDL_OV_ASYNC_UPDATE_COMPLETE ; //      (EVT_MDL_DEV_START + 0x1004)
   }
   
   
   
   }
   
   
   //#define NOT_MATCH_ENABLED  0x0001  //Do not match Enabled Overlays
   //#define NOT_MATCH_DISABLED 0x0002  //Do not match Disabled Overlays
   
   
   //#define OVERLAY_PARAM_CONTINUOUS 0x0000
   //#define OVERLAY_PARAM_DISCRETE   0x0001
   
   //OverlayParamType definition
   //#define OV_PARM_MINIMIZE_CPU    0x0103ab82
   //#define OV_PARM_SYSTEM_OVERLAY  0x010415f8
   //#define OV_PARM_NO_AUTOCLEAR    0x010419c0
   
   */


/*lint -restore*/
