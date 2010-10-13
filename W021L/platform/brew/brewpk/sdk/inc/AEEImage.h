#ifndef AEEIMAGE_H
#define AEEIMAGE_H
/*======================================================
FILE:  AEEImage.h

SERVICES:  Image Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEEShell

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/
#include "../../inc/AEEIImage.h"
#include "AEE.h"

#define AEEIID_IMAGE AEEIID_IImage
#define IIMAGE_Draw IImage_Draw
#define IIMAGE_DrawFrame IImage_DrawFrame
#define IIMAGE_DrawOffscreen IImage_DrawOffscreen
#define IIMAGE_ExtFunc IImage_ExtFunc
#define IIMAGE_GetInfo IImage_GetInfo
#define IIMAGE_HandleEvent IImage_HandleEvent
#define IIMAGE_SetAnimationRate IImage_SetAnimationRate
#define IIMAGE_SetDisplay IImage_SetDisplay
#define IIMAGE_SetDrawSize IImage_SetDrawSize
#define IIMAGE_SetFrameCount IImage_SetFrameCount
#define IIMAGE_SetFrameSize IImage_SetFrameSize
#define IIMAGE_SetOffset IImage_SetOffset
#define IIMAGE_Start IImage_Start
#define IIMAGE_Stop IImage_Stop
#define IIMAGE_AddRef IImage_AddRef
#define IIMAGE_Notify IImage_Notify
#define IIMAGE_Release IImage_Release
#define IIMAGE_SetParm IImage_SetParm
#define IIMAGE_SetStream IImage_SetStream

#define IVIEWER_AddRef(p)                     IImage_AddRef(p)
#define IVIEWER_Release(p)                    IImage_Release(p)
#define IVIEWER_Draw(p,x,y)                   IImage_Draw(p,x,y)
#define IVIEWER_DrawFrame(p,n,x,y)            IImage_DrawFrame(p,n,x,y)
#define IVIEWER_GetInfo(p,pi)                 IImage_GetInfo(p, pi)
#define IVIEWER_SetParm(p,n,p1,p2)            IImage_SetParm(p,n,p1,p2)
#define IVIEWER_Start(p,x,y)                  IImage_Start(p,x,y)
#define IVIEWER_Stop(p)                       IImage_Stop(p)

#define IVIEWER_SetStream(p,ps)               IImage_SetStream(p,ps)
#define IVIEWER_HandleEvent(p,e,w,d)          IImage_HandleEvent(p,e,w,d)
#define IVIEWER_Notify(p,pfn,pu)              IImage_Notify(p,pfn,pu)


#define IVIEWER_SetDrawSize(p,cx,cy)         IImage_SetParm(p,IPARM_SIZE,(int)cx,(int)cy)
#define IVIEWER_SetOffset(p,x,y)             IImage_SetParm(p,IPARM_OFFSET,(int)x,(int)y)
#define IVIEWER_SetFrameSize(p,cx)           IImage_SetParm(p,IPARM_CXFRAME,(int)cx,0)
#define IVIEWER_SetFrameCount(p,n)           IImage_SetParm(p,IPARM_NFRAMES,(int)n,0)
#define IVIEWER_SetAnimationRate(p,r)        IImage_SetParm(p,IPARM_RATE,(int)r,0)
#define IVIEWER_SetDisplay(p,pd)             IImage_SetParm(p,IPARM_DISPLAY,(int)(pd),0)
#define IVIEWER_DrawOffscreen(p,b)           IImage_SetParm(p,IPARM_OFFSCREEN,(int)b,0)


struct IImage {
   IImageVtbl* pvt;
};


/*=============================================================================
   INTERFACE DOCUMENTATION
===============================================================================
IViewer Interface

The IViewer Interface is identical to the IImage Interface.

The Descriptions of the IImage functions also describe the IViewer functions.

The Prototypes are the same with the exception that each place where there is IIMAGE,
it is replaced by IVIEWER.

The Parameters are the same as IImage’s parameters. The parameter pIImage is replaced
with pIViewer, which is the IViewer Interface object.

The Return Values are the same return values as shown in the IImage Interface.

The Comments applicable to the IImage functions are also applicable to the IViewer
functions.

The Side Effects applicable to the IImage functions are also applicable to the IViewer
functions.

The See Also relationship between functions in the IImage functions are also applicable
between the IViewer functions.


List of Header files to be included


=================================================================
IVIEWER_AddRef()

Description:
This function is inherited from IBASE_AddRef()
=================================================================	
IVIEWER_Release() 	

Description:
This function is inherited from IBASE_Release() 
=================================================================	
IVIEWER_Draw()	

Description:
This function is identical to IIMAGE_Draw()
=================================================================	
IVIEWER_DrawFrame()	

Description:
This function is identical to IIMAGE_DrawFrame()
=================================================================	
IVIEWER_GetInfo()	

Description:
This function is identical to IIMAGE_GetInfo()
=================================================================	
IVIEWER_SetParm()	

Description:
This function is identical to IIMAGE_SetParm()
=================================================================	
IVIEWER_Start()	

Description:
This function is identical to IIMAGE_Start()
=================================================================	
IVIEWER_Stop() 	

Description:
This function is identical to IIMAGE_Stop() 
=================================================================	
IVIEWER_SetStream()	

Description:
This function is identical to IIMAGE_SetStream()
=================================================================	
IVIEWER_HandleEvent() 	

Description:
This function is identical to IIMAGE_HandleEvent()
=================================================================	
IVIEWER_Notify()	

Description:
This function is identical to IIMAGE_Notify()
=================================================================	
IVIEWER_SetDrawSize()	

Description:
This function is identical to IIMAGE_SetDrawSize()
=================================================================	
IVIEWER_SetOffset()	

Description:
This function is identical to IIMAGE_SetOffset()
=================================================================	
IVIEWER_SetFrameSize()	

Description:
This function is identical to IIMAGE_SetFrameSize()
=================================================================	
IVIEWER_SetFrameCount()	

Description:
This function is identical to IIMAGE_SetFrameCount()
=================================================================	
IVIEWER_SetAnimationRate()	

Description:
This function is identical to IIMAGE_SetAnimationRate()
=================================================================	
IVIEWER_SetDisplay()	

Description:
This function is identical to IIMAGE_SetDisplay()
=================================================================	
IVIEWER_DrawOffscreen()	

Description:
This function is deprecated in BREW Client 1.1. .
===========================================================================*/

#endif //AEEIMAGE_H
