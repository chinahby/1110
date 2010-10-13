/*
================================================================================

  FILE:  OEMOverlayHelper.c
  
  SERVICES: Provides the Overlay Helper interfaces

  GENERAL DESCRIPTION:

================================================================================
================================================================================
    
               Copyright ?1999-2005 QUALCOMM Incorporated 
                     All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

$Header: //source/qcom/qct/multimedia/display/ioverlay/main/latest/src/OEMOverlayHelper.c#4 $
$Author: jbrasen $
$DateTime: 2008/06/27 16:22:13 $

================================================================================
===============================================================================
*/

#include "OEMFeatures.h"
#include "OEMOverlayHelper.h"

#ifdef FEATURE_MMOVERLAY
#include "AEEMMOverlay.h"
#endif /* FEATURE_MMOVERLAY */

/*======================================================================= 
=======================================================================*/
static void OverlayHelper_DeleteListenerCB(void *pData)
{
   ModelListener *pMListener = (ModelListener *) pData;
   LISTENER_Cancel(pMListener);
   return;
}


/*======================================================================= 
=======================================================================*/
//Function called to remove a listener from the list
void OverlayHelper_CancelListener( ModelListener *pMListener )
{
   OverlayHelper_Listener * pListener = (OverlayHelper_Listener *)pMListener->pCancelData;

   CALLBACK_Cancel(&(pListener->cb));

   //Verify that it is me set as the cancel function.
   //Cross-task cancellations need this check.
   if(pMListener->pfnCancel != OverlayHelper_CancelListener)
   {
      return;
   }

   if(NULL == pListener)
   {
      return;
   }

   if( NULL != pMListener->pPrev ) {
      pMListener->pPrev->pNext = pMListener->pNext;
   }

   if( NULL != pMListener->pNext ) {
      pMListener->pNext->pPrev = pMListener->pPrev;
   }

   //Remove from the DisplayCfg list
   if( NULL != pListener->pPrev) {
      pListener->pPrev->pNext = pListener->pNext;
   }
   else
   {
      *(pListener->ppHead) = pListener->pNext;
   }

   if( NULL != pListener->pNext)
   {
      pListener->pNext->pPrev = pListener->pPrev;
   }

   pMListener->pCancelData = NULL;
   pMListener->pfnCancel   = NULL;

   FREE(pListener);
}

/*======================================================================= 
=======================================================================*/
//Function called to notify applications about the event
void OverlayHelper_NotifyListeners( OverlayHelper_Listener *pHead, ModelEvent *pEvent )
{
   OverlayHelper_Listener * pl = pHead;

   while (pl)
   { 
      ModelListener * pListener = pl->pListener;
      PACONTEXT pacLast = AEE_EnterAppContext(pl->pac);
      //Move to next node
      pl = pl->pNext;
      //Call the function
      if(NULL != pListener->pfnListener)
      {
         pListener->pfnListener(pListener->pListenerData, pEvent);
      }
      AEE_LeaveAppContext(pacLast);
   }
}

/*======================================================================= 
=======================================================================*/
int OverlayHelper_AddListener( OverlayHelper_Listener **ppHead, ModelListener *pListener )
{
   OverlayHelper_Listener * pHeadListener = NULL;
   OverlayHelper_Listener * pNewListener = MALLOCREC(OverlayHelper_Listener);

   if(NULL == pNewListener)
   {
      return ENOMEMORY;
   }

   CALLBACK_Init(&(pNewListener->cb), OverlayHelper_DeleteListenerCB, pListener);
   AEE_LinkSysObject(&(pNewListener->cb));

   // Make sure that its not there in any other list
   LISTENER_Cancel(pListener);

   pListener->pfnCancel   = OverlayHelper_CancelListener;
   pListener->pCancelData = (void *)pNewListener;
   pListener->pPrev       = NULL;

   //Get the head Listener
   pHeadListener = *ppHead;


   if( NULL != pHeadListener ) 
   {
      pHeadListener->pListener->pPrev = pListener;
      pListener->pNext = pHeadListener->pListener;
   }
   else
   {
      pListener->pNext = NULL;
   }

   //Add new DisplayCfg listener to the list
   pNewListener->pListener = pListener;

   pNewListener->pPrev = NULL;
   pNewListener->pNext = pHeadListener;

   //Save the app context
   pNewListener->pac = AEE_GetAppContext();

   if(NULL != pHeadListener)
   {
      pHeadListener->pPrev = pNewListener;
   }

   *ppHead = pNewListener;
   pNewListener->ppHead = ppHead;

   return SUCCESS;
}

// extract coordinates of an AEERect
#define RCRIGHT(prc)          (((prc)->x) + ((prc)->dx) - 1)
#define RCBOTTOM(prc)         (((prc)->y) + ((prc)->dy) - 1)
#define RCCOORD(prc,l,t,r,b)  (l)=(prc)->x,(t)=(prc)->y, \
                              (r)=RCRIGHT(prc),(b)=RCBOTTOM(prc)

// calculate the intersection of two rectangles
boolean OverlayHelper_IntersectRect(AEERect *prcDest, 
                                                const AEERect *prcSrc1, 
                                                const AEERect *prcSrc2)
{
   int aleft, atop, aright, abottom;   // coordinates of prcSrc1
   int bleft, btop, bright, bbottom;   // coordinates of prcSrc2

   // get the left, top, right, bottom coordinates of each source rect
   RCCOORD(prcSrc1, aleft, atop, aright, abottom);
   RCCOORD(prcSrc2, bleft, btop, bright, bbottom);

   // early rejection test
   if (aleft > bright || atop > bbottom || bleft > aright || btop > abottom) {
      if (prcDest) {
         SETAEERECT(prcDest, 0, 0, 0, 0);
      }
      return FALSE;   // empty intersection
   }

   if (prcDest) {
      int right = MIN(aright, bright);
      int bottom = MIN(abottom, bbottom);

      prcDest->x = (int16) MAX(aleft, bleft);
      prcDest->y = (int16) MAX(atop, btop);
      prcDest->dx = (int16) (right - prcDest->x) + 1;
      prcDest->dy = (int16) (bottom - prcDest->y) + 1;
   }

   return TRUE;
}

/*======================================================================= 
=======================================================================*/
//Easier to use interface for notifications
void OverlayHelper_Notify(IQueryInterface * pMe,
                                              uint32 evCode, 
                                              uint32 dwParam)
{
   ModelEvent MEvent;

   MEvent.evCode = evCode;
   MEvent.dwParam = dwParam;

   if( SUCCESS != IQI_QueryInterface( (IQueryInterface *) pMe, AEEIID_MODEL,
                                            (void **)&(MEvent.pModel) ) ) {
      return;
   }

   IMODEL_Notify(MEvent.pModel, &MEvent);

   (void) IMODEL_Release(MEvent.pModel);
}

#ifdef FEATURE_MMOVERLAY
//The device capabilities
/*lint -e{818, 415} pOverlay can not be const, buffers are correctly sized */
int OverlayHelper_GetCapabilities(OverlayParamType paramID,
                                  int32 * pSize,
                                  OverlayCapabilitiesType * pBuff,
                                  IMMOverlay *pOverlay)
{
   (void)pOverlay;
   if(pSize == NULL)
      return EBADPARM;

   switch(paramID)
   {
   case OV_PARM_DISPLAY:
      {
#ifdef FEATURE_MDP2
#error code not present
#else
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 4; //dwValues
#endif
         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
#ifdef FEATURE_MDP2
#error code not present
#else
               pBuff->paramInfo[nIndex].dwValues = 2;
#endif
               pBuff->paramInfo[nIndex].OverlayUnionMember =
                  OV_UNION_TYPE_CLSID;
               pBuff->paramInfo[nIndex].pdwParams = ((uint32 *)
                  &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] = AEECLSID_DISPLAY1;
               pBuff->paramInfo[nIndex].pdwParams[1] = AEECLSID_DISPLAY2; 
#ifdef FEATURE_MDP2
#error code not present
#endif
               return SUCCESS;
            }
         }
      }

   case OV_PARM_LAYER:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
#ifdef FEATURE_MDP2
#error code not present
#else
            +sizeof(uint32) * 3; //dwValues
#endif
         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
#ifdef FEATURE_MDP2
#error code not present
#else
               pBuff->paramInfo[nIndex].dwValues = 3;
#endif
               pBuff->paramInfo[nIndex].OverlayUnionMember =
                  OV_UNION_TYPE_LAYER;
               pBuff->paramInfo[nIndex].pdwParams = 
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
#ifdef FEATURE_MDP2
#error code not present
#else
               pBuff->paramInfo[nIndex].pdwParams[0] = 0x00;
               pBuff->paramInfo[nIndex].pdwParams[1] = 0x7F;
               pBuff->paramInfo[nIndex].pdwParams[2] = 0xFF;
#endif
               return SUCCESS;              
            }
         }
      }

   case OV_PARM_TRANSPARENCY:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(OverlayCapabilitiesSubType) //One Extra Parameter
            +sizeof(uint32) * (2+2); //dwValues


         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 2;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams = 
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] = 1;
               pBuff->paramInfo[nIndex].pdwParams[1] = 0;

               nIndex++;
               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_CONTINUOUS;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_RGB;
               pBuff->paramInfo[nIndex].pdwParams =
                  pBuff->paramInfo[nIndex-1].pdwParams +
                  pBuff->paramInfo[nIndex-1].dwValues;
               pBuff->paramInfo[nIndex].pdwParams[0] = RGB_BLACK;
               pBuff->paramInfo[nIndex].pdwParams[1] = RGB_WHITE;

               return SUCCESS;              
            }
         }
      }

   case OV_PARM_ALPHA_BLEND_PCT:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 2; //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_CONTINUOUS;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember =OV_UNION_TYPE_FIXEDPOINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] = 256;
               pBuff->paramInfo[nIndex].pdwParams[1] = 0;

               return SUCCESS;
            }
         }
      }
   
   case OV_PARM_ROTATE:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 4; //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 4;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] =   0;
               pBuff->paramInfo[nIndex].pdwParams[1] =  90;
               pBuff->paramInfo[nIndex].pdwParams[2] = 180;
               pBuff->paramInfo[nIndex].pdwParams[3] = 270;

               return SUCCESS;
            }
         }
      }
   case OV_PARM_REFLECT:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 4; //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 4;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] =
                  0;
               pBuff->paramInfo[nIndex].pdwParams[1] =
                  OV_FLAG_REFLECT_X_AXIS;
               pBuff->paramInfo[nIndex].pdwParams[2] =
                  OV_FLAG_REFLECT_Y_AXIS;
               pBuff->paramInfo[nIndex].pdwParams[3] =
                  OV_FLAG_REFLECT_X_AXIS | OV_FLAG_REFLECT_Y_AXIS;

               return SUCCESS;
            }
         }
      }

   case OV_PARM_SCALE_X:
   case OV_PARM_SCALE_Y:
      {
         int32 nSize;

         nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(OverlayCapabilitiesSubType) //One Extra Parameter
            +sizeof(uint32) * (8) //dwValues
            +sizeof(uint32) * (2); //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 2;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 8;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));

               pBuff->paramInfo[nIndex].pdwParams[0] =
                  OV_FLAG_SCALE_KEEP_ASPECT;
               pBuff->paramInfo[nIndex].pdwParams[1] =
                  OV_FLAG_SCALE_KEEP_ASPECT|OV_FLAG_SCALE_TO_FIT;
               pBuff->paramInfo[nIndex].pdwParams[2] =
                  OV_FLAG_SCALE_KEEP_ASPECT|OV_FLAG_SCALE_NO_CROP;
               pBuff->paramInfo[nIndex].pdwParams[3] =
                  OV_FLAG_SCALE_KEEP_ASPECT|OV_FLAG_SCALE_TO_FIT|OV_FLAG_SCALE_NO_CROP;
               pBuff->paramInfo[nIndex].pdwParams[4] =
                  0;
               pBuff->paramInfo[nIndex].pdwParams[5] =
                  OV_FLAG_SCALE_TO_FIT;
               pBuff->paramInfo[nIndex].pdwParams[6] =
                  OV_FLAG_SCALE_NO_CROP;
               pBuff->paramInfo[nIndex].pdwParams[7] =
                  OV_FLAG_SCALE_TO_FIT|OV_FLAG_SCALE_NO_CROP;

               nIndex++;
               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_CONTINUOUS;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember =
                  OV_UNION_TYPE_FIXEDPOINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  pBuff->paramInfo[nIndex-1].pdwParams +
                  pBuff->paramInfo[nIndex-1].dwValues;
               pBuff->paramInfo[nIndex].pdwParams[0] = 64;
               pBuff->paramInfo[nIndex].pdwParams[1] = 1024;

               return SUCCESS;
            }
         }
      }
   case OV_PARM_VSYNC:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 2; //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] = 0;
               pBuff->paramInfo[nIndex].pdwParams[1] = 1;

               return SUCCESS;
            }
         }
      }

   case OV_PARM_MINIMIZE_CPU:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 2; //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] = 0;
               pBuff->paramInfo[nIndex].pdwParams[1] = 1;

               return SUCCESS;
            }
         }
      }

      case OV_PARM_SYSTEM_OVERLAY:
      {
         int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
            +sizeof(uint32) * 2; //dwValues

         if(pBuff == NULL)
         {
            *pSize = nSize;
            return SUCCESS;
         }
         else
         {
            if(*pSize != nSize)
            {
               return EBADPARM;
            }
            else
            {
               int nIndex = 0;
               pBuff->wParams = 1;

               pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
               pBuff->paramInfo[nIndex].dwValues = 2;
               pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
               pBuff->paramInfo[nIndex].pdwParams =
                  ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
               pBuff->paramInfo[nIndex].pdwParams[0] = 0;
               pBuff->paramInfo[nIndex].pdwParams[1] = 1;

               return SUCCESS;
            }
         }
      }

   case OV_PARM_NO_AUTOCLEAR:
   {
      int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
         +sizeof(uint32) * 2; //dwValues

      if(pBuff == NULL)
      {
         *pSize = nSize;
         return SUCCESS;
      }
      else
      {
         if(*pSize != nSize)
         {
            return EBADPARM;
         }
         else
         {
            int nIndex = 0;
            pBuff->wParams = 1;

            pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
            pBuff->paramInfo[nIndex].dwValues = 2;
            pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
            pBuff->paramInfo[nIndex].pdwParams =
               ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
            pBuff->paramInfo[nIndex].pdwParams[0] = 0;
            pBuff->paramInfo[nIndex].pdwParams[1] = 1;

            return SUCCESS;
         }
      }
   }

   case OV_PARM_EDGE_SCALING:
   case OV_PARM_ALLOW_NEGATIVE_REGION:
   {
      int32 nSize = sizeof(OverlayCapabilitiesType) // One Parameter
         +sizeof(uint32) * 2; //dwValues

      if(pBuff == NULL)
      {
         *pSize = nSize;
         return SUCCESS;
      }
      else
      {
         if(*pSize != nSize)
         {
            return EBADPARM;
         }
         else
         {
            int nIndex = 0;
            pBuff->wParams = 1;

            pBuff->paramInfo[nIndex].wFlags = OVERLAY_PARAM_DISCRETE;
            pBuff->paramInfo[nIndex].dwValues = 2;
            pBuff->paramInfo[nIndex].OverlayUnionMember = OV_UNION_TYPE_UINT;
            pBuff->paramInfo[nIndex].pdwParams =
               ((uint32 *) &(pBuff->paramInfo[pBuff->wParams]));
            pBuff->paramInfo[nIndex].pdwParams[0] = 0;
            pBuff->paramInfo[nIndex].pdwParams[1] = 1;

            return SUCCESS;
         }
      }
   }

   default:
      return EUNSUPPORTED;
   }
}

//Verifies the parameters are valid
//Returns SUCCESS on successful operation
//Return EBADPARM or EUNSUPPORTED on failure
int OverlayHelper_VerifyParameters(IMMOverlay *pOverlay, OverlayParamType paramID,
                                    uint32 p1, uint32 p2)
{
   int nErr = SUCCESS;
   boolean bMatched[2] = {FALSE, FALSE};

   //Verify Non NULL Pointer parameters
   if((paramID == OV_PARM_BITMAP) ||
      (paramID == OV_PARM_SRC_REGION) ||
      (paramID == OV_PARM_DEST_REGION))
   {
      if(p1 == NULL)
      {
         nErr = EBADPARM;
      }
   }

   //Check all parameters against Capabilities that support it
   if(!((paramID == OV_PARM_BITMAP) ||
      (paramID == OV_PARM_SRC_REGION) ||
      (paramID == OV_PARM_DEST_REGION) ||
      (paramID == OV_PARM_ENABLE) ||
      (paramID == OV_PARM_ENABLE) ||
      (paramID == OV_PARM_AUTO_DISABLE)))
   {
      OverlayCapabilitiesType * pOverlayCapabilities;
      int32 nSize;

      //This function will have to be modified if more that two parameters are
      //supported by any paramID

      //Check against OverlayMgr Capabilities

      //Get the Size
      nErr = OverlayHelper_GetCapabilities(paramID, &nSize, NULL, NULL);
      if(nErr == SUCCESS)
      {
         pOverlayCapabilities = 
            (OverlayCapabilitiesType *) MALLOC((uint32)nSize);
         if(pOverlayCapabilities == NULL)
         {
            nErr = ENOMEMORY;
         }
         else
         {
            nErr =  OverlayHelper_GetCapabilities(paramID, &nSize,
               pOverlayCapabilities, NULL);
            if(nErr == SUCCESS)
            {
               if(pOverlayCapabilities->wParams == 0)
               {
                  bMatched[0] = TRUE;
               }
               if(pOverlayCapabilities->wParams == 1)
               {
                  bMatched[1] = TRUE;
               }
               if(pOverlayCapabilities->wParams > 2)
               {
                  nErr = EFAILED;
               }
               else
               {
                  uint32 parameterIndex;
                  uint32 valueIndex;
                  uint32 tempParam;
                  for(parameterIndex = 0; parameterIndex <
                     pOverlayCapabilities->wParams;
                     parameterIndex++)
                  {
                     uint32 lowerbound,upperbound, current;
                     lowerbound = 0;

                     if(parameterIndex == 0)
                     {
                        tempParam = p1;
                     }
                     else
                     {
                        tempParam = p2;
                     }
                     
                     for(valueIndex = 0; valueIndex <
                         pOverlayCapabilities->paramInfo[parameterIndex].dwValues;
                         valueIndex++)
                     {
                        if((pOverlayCapabilities->paramInfo[parameterIndex].\
                            wFlags 
                            & OVERLAY_PARAM_DISCRETE) != 0)
                        {
                           if(tempParam == 
                              pOverlayCapabilities->paramInfo[parameterIndex]\
                              .pdwParams[valueIndex])
                           {
                              bMatched[parameterIndex] = TRUE;
                              break;
                           }
                        }
                        else
                        {
                           current = 
                             pOverlayCapabilities->paramInfo[parameterIndex]\
                             .pdwParams[valueIndex];

                           if((valueIndex % 2) == 0)
                           {
                                //Only have one bound
                                lowerbound = current;
                           }
                           else
                           {
                              if(lowerbound > current)
                              {
                                 upperbound = lowerbound;
                                 lowerbound = current;
                              }
                              else
                              {
                                 upperbound = current;
                              }

                              if((tempParam >=
                                 lowerbound) && 
                                 (tempParam <=
                                 upperbound))
                              {
                                 bMatched[parameterIndex] = TRUE;
                                 break;
                              }
                           }
                        }
                     }
                  }
               }
               FREE(pOverlayCapabilities);
               pOverlayCapabilities = NULL;

            }
         }


         //Check against Overlay Capabilities
         if(nErr == SUCCESS && 
            ((bMatched[0] == FALSE) || (bMatched[1] == FALSE)))
         {
            //Get the Size
            nErr = IMMOVERLAY_GetCapabilities(pOverlay, paramID, &nSize, NULL);
            if(nErr == SUCCESS)
            {
               pOverlayCapabilities = 
                  (OverlayCapabilitiesType *) MALLOC((uint32)nSize);
               if(pOverlayCapabilities == NULL)
               {
                  nErr = ENOMEMORY;
               }
               else
               {
                  nErr = IMMOVERLAY_GetCapabilities(pOverlay, paramID, &nSize, 
                                                  pOverlayCapabilities);
                  if(nErr == SUCCESS)
                  {
                     if(pOverlayCapabilities->wParams == 0)
                     {
                        bMatched[0] = TRUE;
                     }
                     if(pOverlayCapabilities->wParams == 1)
                     {
                        bMatched[1] = TRUE;
                     }
                     if(pOverlayCapabilities->wParams > 2)
                     {
                        nErr = EFAILED;
                     }
                     else
                     {
                        uint32 parameterIndex;
                        uint32 valueIndex;
                        uint32 tempParam;
                        for(parameterIndex = 0; parameterIndex <
                           pOverlayCapabilities->wParams;
                           parameterIndex++)
                        {
                           uint32 lowerbound,upperbound, current;
                           lowerbound = 0;

                           if(parameterIndex == 0)
                           {
                              tempParam = p1;
                           }
                           else
                           {
                              tempParam = p2;
                           }
                           
                           for(valueIndex = 0; valueIndex <
                              pOverlayCapabilities->paramInfo[parameterIndex].dwValues;
                              valueIndex++)
                           {
                              if((pOverlayCapabilities->paramInfo[parameterIndex].\
                                 wFlags 
                                 & OVERLAY_PARAM_DISCRETE) != 0)
                              {
                                 if(tempParam == 
                                    pOverlayCapabilities->paramInfo[parameterIndex]\
                                    .pdwParams[valueIndex])
                                 {
                                    bMatched[parameterIndex] = TRUE;
                                    break;
                                 }
                              }
                              else
                              {
                                 current = 
                                 pOverlayCapabilities->paramInfo[parameterIndex]\
                                 .pdwParams[valueIndex];

                                 if((valueIndex % 2) == 0)
                                 {
                                    //Only have one bound
                                    lowerbound = current;
                                 }
                                 else
                                 {
                                    if(lowerbound > current)
                                    {
                                       upperbound = lowerbound;
                                       lowerbound = current;
                                    }
                                    else
                                    {
                                       upperbound = current;
                                    }

                                    if((tempParam >=
                                       lowerbound) && 
                                       (tempParam <=
                                       upperbound))
                                    {
                                       bMatched[parameterIndex] = TRUE;
                                       break;
                                    }
                                 }
                              }
                           }
                        }
                     }
                     FREE(pOverlayCapabilities);
                     pOverlayCapabilities = NULL;

                  }
               }
            }

            if(nErr == SUCCESS &&
               ((bMatched[0] == FALSE) || (bMatched[1] == FALSE)))
            {
               nErr = EBADPARM;
            }
         }
      }
   }
   return nErr;

}

int OverlayHelper_CalculateScaleFactor(IMMOverlay *pMe,
                                       const AEERect *prcSrc, 
                                       const AEERect *prcDest,
                                       boolean bIsRotated90, boolean bPreferY,
                                       uint32 *pdwScaleX, uint32 *pdwScaleY,
                                       uint32 dwScaleFlagsX, uint32 dwScaleFlagsY)
{
   uint32 dwScaleX, dwScaleY;
   AEERect srcRegion = *prcSrc;

   if((srcRegion.dx == 0) || (srcRegion.dy == 0))
   {
      return EBADPARM;
   }

   //Rotate the source region if the rotate angle is 90 or 270.
   if(bIsRotated90)
   {
      int16 temp = srcRegion.dy;
      srcRegion.dy = srcRegion.dx;
      srcRegion.dx = temp;
      bPreferY = !bPreferY;
   }

   if(!bPreferY)
   {
      uint32 dwMaxScaleX = ((uint32)prcDest->dx * 256)/(uint16)srcRegion.dx;
      uint32 dwMaxScaleY = ((uint32)prcDest->dy * 256)/(uint16)srcRegion.dy;

      if(dwScaleFlagsX & OV_FLAG_SCALE_TO_FIT)
      {
         *pdwScaleX = dwMaxScaleX;
      }

      if(dwScaleFlagsY & OV_FLAG_SCALE_TO_FIT)
      {
         *pdwScaleY = dwMaxScaleY;
      }

      if(dwScaleFlagsX & OV_FLAG_SCALE_KEEP_ASPECT)
      {
         dwScaleFlagsY = dwScaleFlagsX;

         if(dwScaleFlagsX & OV_FLAG_SCALE_NO_CROP)
         {
            if(dwMaxScaleX < dwMaxScaleY)
            {
               *pdwScaleY = *pdwScaleX;
            }
            else
            {
               *pdwScaleX = *pdwScaleY;
            }
         }
         else
         {
            *pdwScaleY = *pdwScaleX;
         }
      }
      else
      {
         if(dwScaleFlagsX & OV_FLAG_SCALE_NO_CROP)
         {
            if(*pdwScaleX > dwMaxScaleX)
            {
               *pdwScaleX = dwMaxScaleX;
            }
         }
      }
   }
   else
   {
      uint32 dwMaxScaleX = ((uint32)prcDest->dx * 256)/(uint16)srcRegion.dx;
      uint32 dwMaxScaleY = ((uint32)prcDest->dy * 256)/(uint16)srcRegion.dy;

      if(dwScaleFlagsY & OV_FLAG_SCALE_TO_FIT)
      {
         *pdwScaleY = dwMaxScaleY;
      }

      if(dwScaleFlagsX & OV_FLAG_SCALE_TO_FIT)
      {
         *pdwScaleX = dwMaxScaleX;
      }

      if(dwScaleFlagsY & OV_FLAG_SCALE_KEEP_ASPECT)
      {
         dwScaleFlagsX = dwScaleFlagsY;

         if(dwScaleFlagsY & OV_FLAG_SCALE_NO_CROP)
         {
            if(dwMaxScaleX < dwMaxScaleY)
            {
               *pdwScaleY = *pdwScaleX;
            }
            else
            {
               *pdwScaleX = *pdwScaleY;
            }
         }
         else
         {
            *pdwScaleX = *pdwScaleY;
         }
      }
      else
      {
         if(dwScaleFlagsY & OV_FLAG_SCALE_NO_CROP)
         {
            if(*pdwScaleY > dwMaxScaleY)
            {
               *pdwScaleY = dwMaxScaleY;
            }
         }
      }
   }

   //Need to find closest valid scale factor
   if(!bIsRotated90)
   {
      dwScaleX = OverlayHelper_FindClosestScaleFactor(pMe, OV_PARM_SCALE_X, *pdwScaleX, TRUE);      
      dwScaleY = OverlayHelper_FindClosestScaleFactor(pMe, OV_PARM_SCALE_Y, *pdwScaleY, TRUE);      
   }
   else
   {
      dwScaleY = OverlayHelper_FindClosestScaleFactor(pMe, OV_PARM_SCALE_X, *pdwScaleX, TRUE);      
      dwScaleX = OverlayHelper_FindClosestScaleFactor(pMe, OV_PARM_SCALE_Y, *pdwScaleY, TRUE);      
   }

   *pdwScaleX = dwScaleX;
   *pdwScaleY = dwScaleY;

   if(((dwScaleFlagsX & OV_FLAG_SCALE_KEEP_ASPECT) ||
      (dwScaleFlagsY & OV_FLAG_SCALE_KEEP_ASPECT)) &&
      (*pdwScaleX != *pdwScaleY))
   {
      DBGPRINTF("Maintain Aspect Ratio was requested, however this is not "
         "Possible given the supported scaling factors, using closest "
         "availible");
   }
   return SUCCESS;
}

uint32 OverlayHelper_FindClosestScaleFactor(IMMOverlay *pMe, OverlayParamType paramID, 
                                            uint32 dwScale, boolean bPreferSmall)
{
   OverlayCapabilitiesType * pOverlayCaps;
   int32 nSize;
   uint32 dwNewScale = dwScale;
   OverlayCapabilitiesSubType paramInfo[2];

   
   if (SUCCESS == IMMOVERLAY_GetCapabilities(pMe, paramID, &nSize, NULL))
   {
      pOverlayCaps = (OverlayCapabilitiesType *) MALLOC((uint32)nSize);
      if(pOverlayCaps != NULL)
      {
         if(SUCCESS == IMMOVERLAY_GetCapabilities(pMe, paramID, &nSize, pOverlayCaps))
         {
            if(pOverlayCaps->wParams == 2)
            {
               (void)MEMCPY(paramInfo, pOverlayCaps->paramInfo, 
                            sizeof(paramInfo));
               dwNewScale = paramInfo[1].pdwParams[0];
               if(paramInfo[1].wFlags & OVERLAY_PARAM_DISCRETE)
               {
                  uint32 idx;
                  for(idx = 0; idx < paramInfo[1].dwValues; idx++)
                  {
                     uint32 dwTest  = paramInfo[1].pdwParams[idx];

                     if(dwScale == dwTest)
                     {
                        //Valid number
                        dwNewScale = dwTest;
                        break;
                     }
                     else
                     {
                        if((dwNewScale < dwScale) && (dwTest > dwScale))
                        {
                           if(!bPreferSmall)
                           {
                              dwNewScale = dwTest;
                           }
                        }
                        else if((dwNewScale > dwScale) && (dwTest < dwScale))
                        {
                           if(bPreferSmall)
                           {
                              dwNewScale = dwTest;
                           }
                        }
                        else if(dwTest < dwScale)
                        {
                           if(dwTest > dwNewScale)
                           {
                              dwNewScale = dwTest;
                           }
                        }
                        else
                        {
                           if(dwTest < dwNewScale)
                           {
                              dwNewScale = dwTest;
                           }
                        }
                     }
                  }
               }
               else
               {
                  uint32 dwPairs = paramInfo[1].dwValues / 2;
                  uint32 idx;

                  for(idx = 0; idx < dwPairs; idx++)
                  {
                     uint32 dwFirst  = paramInfo[1].pdwParams[idx * 2];
                     uint32 dwSecond = paramInfo[1].pdwParams[idx * 2 + 1];

                     if(dwFirst > dwSecond)
                     {
                        uint32 dwTemp = dwFirst;
                        dwFirst = dwSecond;
                        dwSecond = dwTemp;
                     }

                     if(dwScale < dwFirst)
                     {
                        if(dwScale > dwNewScale)
                        {
                           if(!bPreferSmall)
                           {
                              dwNewScale = dwFirst;
                           }
                        }
                        else
                        {
                           //Both numbers are larger that the scale factor
                           //Choose the smaller.
                           if(dwFirst < dwNewScale)
                           {
                              dwNewScale = dwFirst;
                           }
                        }
                     }
                     else if(dwScale < dwSecond)
                     {
                        //Valid number
                        dwNewScale = dwScale;
                        break;
                     }
                     else
                     {
                        if(dwScale < dwNewScale)
                        {
                           if(bPreferSmall)
                           {
                              dwNewScale = dwSecond;
                           }
                        }
                        else
                        {
                           //Both numbers are smaller that the scale factor
                           //Choose the larger.
                           if(dwSecond > dwNewScale)
                           {
                              dwNewScale = dwSecond;
                           }
                        }
                     }
                  }
               }
            }
         }
         FREE(pOverlayCaps);
      }
   }

   return dwNewScale;
}

int OverlayHelper_CalculateSrcDest(AEERect *prcSrc, AEERect *prcDest, 
                                   const AEERect *prcClip,
                                   uint32 dwScaleX, uint32 dwScaleY, 
                                   boolean bIsRotated90,
                                   uint32 dwAlignment)
{
   boolean bClipped = FALSE;

   //Adjust for scaling
   prcDest->dx = (int16)(((int32)prcSrc->dx * (int32)dwScaleX)/256);
   prcDest->dy = (int16)(((int32)prcSrc->dy * (int32)dwScaleY)/256);

   //Calculate for rotate
   //Are we sideways?
   if(bIsRotated90)
   {
      AEERect Temprc = *prcDest;
      //Yes
      prcDest->dx = Temprc.dy;
      prcDest->dy = Temprc.dx;
   }

   //Adjust for alignment
   if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT)
   {
      prcDest->x = prcClip->x + (prcClip->dx - prcDest->dx);
   }
   else if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER)
   {
      prcDest->x = prcClip->x + (prcClip->dx - prcDest->dx)/2;
   }
   else
   {
      prcDest->x = prcClip->x;
   }


   if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM)
   {
      prcDest->y = prcClip->y + (prcClip->dy - prcDest->dy);
   }
   else if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER)
   {
      prcDest->y = prcClip->y + (prcClip->dy - prcDest->dy)/2;
   }
   else
   {
      prcDest->y = prcClip->y;
   }


   //Calculate Clipping Region

   //Check X Clipping
   if(prcDest->x < prcClip->x)
   {
      prcDest->x = prcClip->x;
      bClipped = TRUE;
   }

   if(prcClip->dx != 0)
   {
      if((prcDest->x + prcDest->dx) >
         (prcClip->x + prcClip->dx))
      {
         prcDest->dx = (prcClip->x + prcClip->dx) - prcDest->x;
         bClipped = TRUE;
      }
   }

   //Check Y Clipping
   if(prcDest->y < prcClip->y)
   {
      prcDest->y = prcClip->y;
      bClipped = TRUE;
   }

   if(prcClip->dy != 0)
   {
      if((prcDest->y + prcDest->dy) >
         (prcClip->y + prcClip->dy))
      {
         prcDest->dy = (prcClip->y + prcClip->dy) - prcDest->y;

         bClipped = TRUE;

      }
   }

   if(bClipped)
   {
      //Recalculate the source region
      AEERect SavedRectangle;
      SavedRectangle.dx = prcSrc->dx;
      SavedRectangle.dy = prcSrc->dy;

      //Unrotate
      if(bIsRotated90)
      {
         //Rotated
         prcSrc->dx = prcDest->dy;
         prcSrc->dy = prcDest->dx;
      }
      else
      {
         //Not rotated
         prcSrc->dx = prcDest->dx;
         prcSrc->dy = prcDest->dy;
      }

      prcSrc->dx = (int16)(((int32)prcSrc->dx * 256)/(int32)dwScaleX);
      prcSrc->dy = (int16)(((int32)prcSrc->dy * 256)/(int32)dwScaleY);

      //Reposition the Source Region
      if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_HORIZ_RIGHT)
      {

         prcSrc->x += SavedRectangle.dx - prcSrc->dx;
      }
      else if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_HORIZ_CENTER)
      {
         prcSrc->x += (SavedRectangle.dx - prcSrc->dx)/2;
      }


      if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_VERT_BOTTOM)
      {
         prcSrc->y += SavedRectangle.dy - prcSrc->dy;
      }
      else if(dwAlignment & OV_FLAG_DEST_REGION_ALIGN_VERT_CENTER)
      {
         prcSrc->y += (SavedRectangle.dy - prcSrc->dy)/2;
      }
   }
   return SUCCESS;
}
#endif /* FEATURE_MMOVERLAY */

