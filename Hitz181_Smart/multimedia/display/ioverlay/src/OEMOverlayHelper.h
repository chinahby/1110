/*
================================================================================

  FILE:  OEMOverlayHelper.h
  
  SERVICES: Defines the Overlay Helper interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright © 1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlayHelper.h#2 $
$Author: jbrasen $
$DateTime: 2008/06/25 09:32:38 $
    
================================================================================
===============================================================================
*/
#ifndef __OEMOVERLAYHELPER_H__
#define __OEMOVERLAYHELPER_H__

#include "math.h"
#include "AEEModel.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#endif
#include "AEECriticalSection.h"

//Function compares to floats
//Returns -1 if a < b
//Returns  0 if a == b
//Returns  1 if a > b
#define FLOAT_EPSILON 0.00001
static __inline int  OverlayHelper_FloatCompare(float a, float b)
{
   if(fabsf(a-b) < FLOAT_EPSILON)
   {
      return 0;
   }
   else if(a < b)
   {
      return -1;
   }
   else
   {
      return 1;
   }
}

typedef struct OverlayHelper_Listener OverlayHelper_Listener;

struct OverlayHelper_Listener{
   ModelListener * pListener;
   PACONTEXT pac;
   AEECallback cb;
   OverlayHelper_Listener *pNext;
   OverlayHelper_Listener *pPrev;
   OverlayHelper_Listener **ppHead;
};

/*======================================================================= 
=======================================================================*/
//Easier to use interface for notifications
void OverlayHelper_Notify(IQueryInterface * pMe,
                                              uint32 evCode, 
                                              uint32 dwParam);

// calculate the intersection of two rectangles
boolean OverlayHelper_IntersectRect(AEERect *prcDest, 
                                                const AEERect *prcSrc1, 
                                                const AEERect *prcSrc2);

/*======================================================================= 
=======================================================================*/
//Function called to remove a listener from the list
void OverlayHelper_CancelListener( ModelListener *pMListener );

/*======================================================================= 
=======================================================================*/
//Function called to notify applications about the event
void OverlayHelper_NotifyListeners( OverlayHelper_Listener *pHead, ModelEvent *pEvent );

/*======================================================================= 
=======================================================================*/
int OverlayHelper_AddListener( OverlayHelper_Listener **pHead, ModelListener *pListener );

#ifdef FEATURE_MMOVERLAY
/*======================================================================= 
=======================================================================*/
int OverlayHelper_GetCapabilities(OverlayParamType paramID,
                                  int32 * pSize,
                                  OverlayCapabilitiesType * pBuff,
                                  IMMOverlay *pOverlay);

/*======================================================================= 
=======================================================================*/
int OverlayHelper_VerifyParameters(IMMOverlay *pOverlay, OverlayParamType paramID,
                                    uint32 p1, uint32 p2);


/*======================================================================= 
=======================================================================*/
int OverlayHelper_CalculateScaleFactor(IMMOverlay *pMe,
                                       const AEERect *prcSrc, 
                                       const AEERect *prcDest,
                                       boolean bIsRotated90, boolean bPreferY,
                                       uint32 *pdwScaleX, uint32 *pdwScaleY,
                                       uint32 dwScaleFlagsX, uint32 dwScaleFlagsY);

uint32 OverlayHelper_FindClosestScaleFactor(IMMOverlay *pMe, OverlayParamType paramID, 
                                   uint32 dwScale, boolean bPreferSmall);

int OverlayHelper_CalculateSrcDest(AEERect *prcSrc, AEERect *prcDest,
                                   const AEERect *prcClip,
                                   uint32 dwScaleX, uint32 dwScaleY, 
                                   boolean bIsRotated90,
                                   uint32 dwAlignment);
#endif

#endif //__OEMOVERLAYHELPER_H__
