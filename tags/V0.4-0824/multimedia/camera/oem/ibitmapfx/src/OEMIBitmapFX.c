/*=============================================================================
FILE: OEMBitmapFX.c

SERVICES: This file implements the IBitmapFX interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEE.h"
#include "AEEStdLib.h"
#include "AEEShell.h"
#ifdef FEATURE_IIPL
#include "AEEIBitmapFX.h"
#include "IYCbCr.h"
#include "ipl.h"

/*===========================================================================*/
#ifdef _BUILD_AS_STATIC_EXTENSION_
// Define this if you are intending to build this as a static extension.
// Otherwise this implementation file can be statically built and linked
// in the target build.

#include "StdAfx.h"
#include "StaticExtensionInc.h"
#include "AEEModTable.h"

extern int OEMBitmapFX_New(IShell *ps, AEECLSID cls, void **ppo);

// Create an AEEStaticClass array that lists the information about the interface
const AEEStaticClass gOEMIBITMAPFXExtensionClasses[] =
{
   { AEECLSID_BitmapFX, ASCF_UPGRADE, 0, 0, OEMBitmapFX_New },
   { 0, 0, 0, 0, NULL},
};

/*===========================================================================

Function:  GetStaticClassEntries()

Description:
  This function is required to be exposed if this interface will be
  exposed as a static extension DLL. BREW Simulator loads all the DLLs
  that expose this function as a Static Extension DLL.

Prototype:
     const AEEStaticClass* GetStaticClassEntries(void)

Parameters: None

Return Value:
  return an array of AEEStaticClass

Comments:  None

Side Effects: None

==============================================================================*/
BREWSTATICEXTENSION_API const AEEStaticClass *GetStaticClassEntries(void)
{
  return gOEMIPLExtensionClasses;
}
#endif // _BUILD_AS_STATIC_EXTENSION_
/************************************************************************
                  Constant definitions
 ***********************************************************************/
#define MAX_HUE_FACTOR (360)        // Maximum hue factor supported
#define MIN_HUE_FACTOR (-360)       // Minimum hue factor supported
#define MAX_SAT_FACTOR (255)        // Maximum saturation factor supported
#define MIN_SAT_FACTOR (-255)       // Minimum saturation factor supported
#define MAX_POS_FACTOR (255)        // Maximum posterization factor supported
#define MIN_POS_FACTOR (0)          // Minimum posterizationfactor supported
#define MAX_INT_FACTOR (255)        // Maximum intensity factor supported
#define MIN_INT_FACTOR (-255)       // Minimum intensity factor supported
#define MAX_SOL_FACTOR (255)        // Maximum solarization factor supported
#define MIN_SOL_FACTOR (0)          // Minimum solarization factor supported

#define RELEASEIF(p)  OEMBitmapFX_ReleaseIF((IBase **)&(p))
#define OEMBitmapFX_FreeIPLTypeIF(p) OEMBitmapFX_FreeIplTypeIf((ipl_image_type **)&p)
#define OEMBitmapFX_FreeIPLDataIF(p) OEMBitmapFX_FreeIplDataIf((ipl_image_type **)&p)
/************************************************************************
                 Common IBitmapFX type definitions
 ***********************************************************************/
typedef struct OEMBitmapFX OEMBitmapFX;

AEEINTERFACE(OEMBitmapFX)
{
  INHERIT_IBitmapFX(OEMBitmapFX);
};

struct OEMBitmapFX
{
  const AEEVTBL(OEMBitmapFX) *m_pvt;        // Vector table of IBitmapFX methods
  IShell                      *m_pIShell;    // Pointer to the IShell object
  IModule                     *m_pIModule;   // Pointer to module
  uint32                       m_uRefs;      // Reference count
};
/************************************************************************
             Interface Function Prototypes
**************************************************************************/
static uint32 OEMBitmapFX_AddRef(OEMBitmapFX* pMe);
static uint32 OEMBitmapFX_Release(OEMBitmapFX* pMe);
static void OEMBitmapFX_ReleaseIF(IBase **ppif);
static int OEMBitmapFX_QueryInterface(OEMBitmapFX* pMe, AEECLSID clsid, void** ppNew);
static int  OEMBitmapFX_Effect(OEMBitmapFX* pMe, IBitmap* pinImg, IBitmap* poutImg,
                                int32 nEffect, int32 nEffectValue);
static int  OEMBitmapFX_Filter(OEMBitmapFX* pMe, IBitmap* pinImg, IBitmap* poutImg,
                                int32 nFilter, int32 nBlurWindow);
static int  OEMBitmapFX_MotionBlur(OEMBitmapFX* pMe, IBitmap* pinImg, IBitmap* poutImg,
                                    int32 nAngle, int32 nBlurWindow);
static int  OEMBitmapFX_RadialBlur(OEMBitmapFX* pMe, IBitmap* pinImg, IBitmap* poutImg,
                                    int32 nLevel);
static int  OEMBitmapFX_DrawLumaHistogram(OEMBitmapFX* pMe,IBitmap* inImage, IBitmap* outImage,
                                           AEERect *pRect);
/************************************************************************
                         Internal Functions
 ***********************************************************************/
static int OEMBitmapFX_NewEx(IShell* ps, IModule* pIModule, AEECLSID cls, void** ppo);
static int OEMBitmapFX_Grayscale(const ipl_image_type* iIpl, ipl_image_type* oIpl);
static int OEMBitmapFX_Hue(const ipl_image_type* iIpl, ipl_image_type* oIpl, int32 nHue);
static int OEMBitmapFX_Negative(const ipl_image_type* iIpl, ipl_image_type* oIpl);
static int OEMBitmapFX_Saturation(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                   int32 nSaturation);
static int OEMBitmapFX_Sepia(const ipl_image_type* iIpl, ipl_image_type* oIpl);
static int OEMBitmapFX_Posterize(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                  int32 nPosterize);
static int OEMBitmapFX_Solarize(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                 int32 nSolarize);
static int OEMBitmapFX_Intensity(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                  int32 nIntensity);
static int OEMBitmapFX_Flash(const ipl_image_type* iIpl, ipl_image_type* oIpl);
static int OEMBitmapFX_Whiteboard(ipl_image_type* iIpl, ipl_image_type* oIpl,
                                   int32 nThreshold);
/************************************************************************
                       Helper Functions
 ***********************************************************************/
static int OEMBitmapFX_GetIPLs(IBitmap* pinImg, IBitmap* poutImg, ipl_image_type** iIpl,
                                ipl_image_type** oIpl, boolean bCheckSize);
static int OEMBitmapFX_GetIPL(IBitmap *pinImg, ipl_image_type **iIpl);
static int OEMBitmapFX_DIBToIPL(const IDIB* pDIB, ipl_image_type** inIpl);
static int OEMBitmapFX_YCbCrToIPL(const IYCbCr* pYCbCr, ipl_image_type** inIpl);
static int OEMBitmapFX_CreateIPL(ipl_image_type** poutIpl, uint16 nwidth, uint16 nheight,
                                  ipl_col_for_type  colorFormat);
static void OEMBitmapFX_FreeIplTypeIf(ipl_image_type** inIpl);
static void OEMBitmapFX_FreeIplDataIf(ipl_image_type** inIpl);
static int OEMBitmapFX_GetYCbCrIPL(IBitmap* pinImg, ipl_image_type** iIpl);
static int OEMBitmapFX_GetErrCode(int nVal);
static int OEMBitmapFX_Convert2YCbCr(const ipl_image_type* iIpl,ipl_image_type** oIpl);
static int OEMBitmapFX_GetSupportedIPL(const ipl_image_type* iIpl, boolean bInput,
                                        boolean bOkLP, boolean bOkRGB565,
                                        ipl_image_type **oIpl);
static int OEMBitmapFX_CleanTempIPL(const ipl_image_type* iIpl, ipl_image_type** iTempIpl,
                                     ipl_image_type** oTempIpl, ipl_image_type** oIpl,
                                     int retval);

// gvtOEMBitmapFXFuncs is the virtual table for the IBitmapFX object
static const VTBL(OEMBitmapFX) gvtOEMBitmapFXFuncs =
{
  OEMBitmapFX_AddRef,
  OEMBitmapFX_Release,
  OEMBitmapFX_QueryInterface,
  OEMBitmapFX_Effect,
  OEMBitmapFX_Filter,
  OEMBitmapFX_MotionBlur,
  OEMBitmapFX_RadialBlur,
  OEMBitmapFX_DrawLumaHistogram
};
/*===========================================================================
FUNCTION: OEMBitmapFX_New
DESCRIPTION
  Creates an instance of the IBitmapFX class.  Only one instance of the IBitmapFX
  class can exist at any given time. So, if an instance already exists.
  then the create instance operation will simply return a pointer to the
  existing object.
DEPENDENCIES
  None
RETURN VALUE
  Returns AEE return values
  AEE_SUCCESS:  Registration was successful
  EBADPARM:     One of the arguments was invalid (e.g. NULL pointer)
  ENOMEMORY:    There was not enough memory to create the object
  EFAILED:      An unspecified error occurred; IBitmapFX object not created
SIDE EFFECTS
  None
===========================================================================*/

#if defined(AEE_STATIC)
  int OEMBitmapFX_New(IShell *pIShell, AEECLSID cls, void **ppif)
  {
    return OEMBitmapFX_NewEx(pIShell, NULL, cls, ppif);
  }
#else
int AEEClsCreateInstance(AEECLSID cls, 
                         IShell*   pIShell,
                         IModule*  pIModule,
                         void**    ppObj)
  {
    return OEMBitmapFX_NewEx(pIShell, pIModule, cls, ppObj);
  }
#endif


/*===========================================================================

FUNCTION: OEMBitmapFX_NewEx
DESCRIPTION
  Creates an instance of the IBitmapFX class.  Only one instance of the IBitmapFX
  class can exist at any given time. So, if an instance already exists.
  then the create instance operation will simply return a pointer to the
  existing object.
  The Ex version will keep a copy of IModule pointer.
DEPENDENCIES
  None
RETURN VALUE
  Returns AEE return values
  AEE_SUCCESS:  Registration was successful
  EBADPARM:     One of the arguments was invalid (e.g. NULL pointer)
  ENOMEMORY:    There was not enough memory to create the object
  EFAILED:      An unspecified error occurred; IBitmapFX object not created
SIDE EFFECTS
  None
===========================================================================*/
static int OEMBitmapFX_NewEx(IShell* pIShell, IModule* pIModule, AEECLSID cls, void** ppif)
{
  OEMBitmapFX* pMe  = 0;
  *ppif = NULL;

  pMe = (OEMBitmapFX *)MALLOC(sizeof(OEMBitmapFX));
  if (!pMe)
  {
    return ENOMEMORY;
  }

  pMe->m_pvt = &gvtOEMBitmapFXFuncs;
  /* Set ref count to one, since we just created it.*/
  pMe->m_uRefs = 1;
  // Set shell pointer
  pMe->m_pIShell = pIShell;
  (void)ISHELL_AddRef(pMe->m_pIShell);

  // Set the module pointer and addref it
  pMe->m_pIModule = pIModule;
  if (pIModule)
  {
    (void)IMODULE_AddRef(pIModule);
  }

  *ppif = pMe;

  return AEE_SUCCESS;
}
/*===========================================================================
FUNCTION: OEMBitmapFX_AddRef
DESCRIPTION
  Increments the reference count for the IBitmapFX object.
DEPENDENCIES
  None
RETURN VALUE
  Returns the value of the incremented reference count
SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMBitmapFX_AddRef(OEMBitmapFX *pMe)
{
  return ++(pMe->m_uRefs);
}
/*===========================================================================
FUNCTION: OEMBitmapFX_Release
DESCRIPTION
  Decrements the reference count for the IBitmapFX object.  When the
  reference count drops to zero, it deletes the object.
DEPENDENCIES
  None
RETURN VALUE
  Returns the decremented reference count for the object
SIDE EFFECTS
  None
===========================================================================*/
static uint32 OEMBitmapFX_Release(OEMBitmapFX* pMe)
{
  // If there are still references after decrementing, return ref count
  if (--(pMe->m_uRefs) > 0)
  {
    return (uint32)(pMe->m_uRefs);
  }

  (void)ISHELL_Release(pMe->m_pIShell);
  if (pMe->m_pIModule)
  {
    (void)IMODULE_Release(pMe->m_pIModule);
  }
  FREE(pMe);

  return 0;
}
/*===========================================================================
FUNCTION: OEMBitmapFX_QueryInterface
DESCRIPTION
  Allows applications to query if IBitmapFX supports other interfaces and then
  returns a pointer to that new interface supported via IBitmapFX.  Currently,
  we do not support any other interface.
DEPENDENCIES
  none
RETURN VALUE
  none
SIDE EFFECTS
===========================================================================*/
static int OEMBitmapFX_QueryInterface(OEMBitmapFX* pMe, AEECLSID id, void** ppNew)
{
  int ret_val = AEE_SUCCESS;

  switch (id){
  case AEEIID_IQI:
  case AEEIID_IBitmapFX:
    if (OEMBitmapFX_AddRef(pMe))
    {
      *ppNew = pMe;
    }
    break;
  default:
    *ppNew = NULL;
    ret_val = ECLASSNOTSUPPORT;
    break;
  }
  return ret_val;
}

/*===========================================================================
FUNCTION OEMBitmapFX_ReleaseIF

  DESCRIPTION
    This function releases the pointer and set it to NULL if it is not NULL.

  PARAMETERS:
    ppif - Pointer to (IBase *), replaced by any BREW interface pointer

  RETURN VALUE:
   None
===========================================================================*/
static void OEMBitmapFX_ReleaseIF(IBase **ppif)
{
  if (ppif && *ppif)
  {
    (void)IBASE_Release(*ppif);
  }
  if (ppif)
    *ppif = NULL;
}

/*=============================================================================
                           SPECIAL EFFECTS
=============================================================================*/

/*=============================================================================
OEMBitmapFX_Filter()
Description:
    This function filters the input image. There are four types of filters that can be applied to
    an input image.
    1) BLUR : It'll result in the blurring of the input image
    2) SHARP: It'll result in the sharpening of the input image
    3) GAUSSIAN : It'll add a gaussian blur to the input image
    4) UNIFORM : It'll add a uniform blur to the input image
    The blurring is done in incrementally. The blur window specifies the rectangular area
    which is blurred at a time. The blur window can range from
    [ 3 .. 0.5 * min (input height, input width)]
    For BLUR and SHARP, the blur Window size is restricted to 3.
Return Value:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EBADPARM, if the filter type is not one of the permitted values or the blur Window
              falls outside the range
See Also:
=============================================================================*/
static int OEMBitmapFX_Filter (OEMBitmapFX* pIIpl, IBitmap* inImage, IBitmap* outImage,
                                int32 nfiltertype, int32 nBlurWindow)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* iIpl = NULL;
  ipl_image_type* oIpl = NULL;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;
  int32 maxDimension = 0;

  if (!pIIpl || !inImage || !outImage)
  {
    return EBADPARM;
  }
  do
  {
    retVal = OEMBitmapFX_GetIPLs(inImage, outImage, &iIpl, &oIpl, TRUE);
    if (retVal)
    {
      break;
    }
    if(iIpl->dx > iIpl->dy)
    {
      if(((int32)iIpl->dx/2)%2)
      {
        maxDimension = (int32)iIpl->dx/2;
      }
      else
      {
        maxDimension = (int32)iIpl->dx/2 + 1;
      }
    }
    else
    {
      if(((int32)iIpl->dy/2)%2)
      {
        maxDimension = (int32)iIpl->dy/2;
      }
      else
      {
        maxDimension = (int32)iIpl->dy/2 + 1;
      }
    }

    //Do conversion to fit ipl input color format requirement
    if ((nfiltertype == AEEBITMAPFX_BLUR_GAUSSIAN) || (nfiltertype == AEEBITMAPFX_BLUR_UNIFORM))
    {
      //Input and output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP
      retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, TRUE, &tempiIpl);
      retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);
    }
    else if (nfiltertype == AEEBITMAPFX_BLUR)
    {
      //Input and output could be RGB565 or YCbCr 4:2:2.
      retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, TRUE, &tempiIpl);
      retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);
    }
    else if (nfiltertype == AEEBITMAPFX_SHARP)
    {
      //Input images must be in YCbCr 4:2:2.
      retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, FALSE, &tempiIpl);
      //Output can be RGB565 or YCbCr 4:2:2
      retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);
    }
    if (retVal)
    {
      break;
    }

    switch(nfiltertype)
    {
    case AEEBITMAPFX_BLUR:
      retVal = (int)ipl_filter_image(tempiIpl, tempoIpl, IPL_BLUR, 3);
      retVal = OEMBitmapFX_GetErrCode(retVal);
      break;
    case AEEBITMAPFX_BLUR_GAUSSIAN:
      if ((nBlurWindow%2) && (nBlurWindow < maxDimension))
      {
        retVal = (int)ipl_filter_image(tempiIpl, tempoIpl, IPL_BLUR_GAUSSIAN, nBlurWindow);
        retVal = OEMBitmapFX_GetErrCode(retVal);
      }
      else
      {
        retVal = EBADPARM;
      }
      break;
    case AEEBITMAPFX_BLUR_UNIFORM:
      if ((nBlurWindow%2) && (nBlurWindow < maxDimension))
      {
        retVal = (int)ipl_filter_image(tempiIpl, tempoIpl, IPL_BLUR_UNIFORM, nBlurWindow);
        retVal = OEMBitmapFX_GetErrCode(retVal);
      }
      else
      {
        retVal = EBADPARM;
      }
      break;
    case AEEBITMAPFX_SHARP:
      retVal = (int)ipl_filter_image(tempiIpl, tempoIpl, IPL_SHARP, 3);
      retVal = OEMBitmapFX_GetErrCode(retVal);
      break;
    default:
      break;
    }
  }while(0);

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);
  OEMBitmapFX_FreeIPLTypeIF(iIpl);
  OEMBitmapFX_FreeIPLTypeIF(oIpl);
  
  return retVal;
}

/*=============================================================================
OEMBitmapFX_MotionBlur()
Description:
  This function produces motion blur in any arbitrary direction. The size
  of the blurring window must be an odd, positive integer in the range
  3 <= size <= 1/2 * min(w, h), where w and h are the width and height of
  the input image. The direction of motion is specified by a positive integer
  angle measured in degrees from the horizontal.
Return Value:
   Returns AEEError Codes
   AEE_SUCCESS: operation was successful
   EBADPARM: if the nBlurWindow is not a positive odd integer.
See Also:
=============================================================================*/
static int OEMBitmapFX_MotionBlur(OEMBitmapFX* pIIpl, IBitmap* inImage, IBitmap* outImage,
                                   int32 nBlurWindow, int32 nAngle)
{
  AEEBitmapInfo info;
  int retVal = AEE_SUCCESS;
  ipl_image_type* iIpl = NULL;
  ipl_image_type* oIpl = NULL;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;
  int32 maxDimension = 0;

  if (!pIIpl || !inImage || !outImage )
  {
    return EBADPARM;
  }

  if (IBITMAP_GetInfo(inImage, &info, sizeof(AEEBitmapInfo)) != SUCCESS)
  {
    return EUNSUPPORTED;
  }

  if(info.cx > info.cy)
  {
    if((info.cx/2)%2)
    {
      maxDimension = info.cx/2;
    }
    else
    {
      maxDimension = info.cx/2 + 1;
    }
  }
  else
  {
    if((info.cy/2)%2)
    {
      maxDimension = info.cy/2;
    }
    else
    {
      maxDimension = info.cy/2 + 1;
    }
  }

  if((nBlurWindow%2) && (nBlurWindow < maxDimension))
  {
    retVal = OEMBitmapFX_GetIPLs(inImage, outImage, &iIpl, &oIpl, TRUE);
    if (retVal == AEE_SUCCESS)
    {
      //Input and output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP
      retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, TRUE, &tempiIpl);
      retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);
        
      if (retVal == AEE_SUCCESS)
      {
        retVal = (int)ipl_motion_blur(tempiIpl, tempoIpl, nBlurWindow, (uint16)(nAngle%360));
        retVal = OEMBitmapFX_GetErrCode(retVal);
      }
      retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);
    }
  }
  else
  {
    retVal = EBADPARM;
  }
  OEMBitmapFX_FreeIPLTypeIF(iIpl);
  OEMBitmapFX_FreeIPLTypeIF(oIpl);

  return retVal;
}
/*=============================================================================
OEMBitmapFX_RadialBlur()
Description:
  This function performs radial blur. The blurring level can be set
  to an integer between 0 and 100, inclusive. No blurring occurs
  when the blurring nlevel is 0.
Return Value:
   Returns AEEError Codes
   AEE_SUCCESS: operation was successful
   EBADPARM: if the blurring level is outside the permitted range
See Also:
=============================================================================*/
static int OEMBitmapFX_RadialBlur(OEMBitmapFX* pIIpl, IBitmap* inImage, IBitmap* outImage,
                                   int32 nlevel)
{ 
  int retVal = AEE_SUCCESS;
  ipl_image_type* iIpl = NULL;
  ipl_image_type* oIpl = NULL;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;


  if (!pIIpl || !inImage || !outImage )
  {
    return EBADPARM;
  }

  if (nlevel > 100)
  {
    return EBADPARM;
  }

  retVal = OEMBitmapFX_GetIPLs(inImage, outImage, &iIpl, &oIpl, TRUE);
  if (retVal == AEE_SUCCESS)
  {
    //Input and output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP
    retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, TRUE, &tempiIpl);
    retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);
    if (retVal == AEE_SUCCESS)
    {
      retVal = (int)ipl_radial_blur(tempiIpl, tempoIpl, 0, 0, (uint8)nlevel);
      retVal = OEMBitmapFX_GetErrCode(retVal);
    }
    retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);
  }

  OEMBitmapFX_FreeIPLTypeIF(iIpl);
  OEMBitmapFX_FreeIPLTypeIF(oIpl);

  return retVal;
}

/*=============================================================================
OEMBitmapFX_Effect()
Description:
  This function adds special effects to the input image. The type of effect to be
  applied is determined by the effect type passed in to the function.
  The different effect types are indicated by OEMBitmapFXEffectType
Return Value:
   Returns AEEError Codes
   AEE_SUCCESS: operation was successful
   EBADPARM: if the effectType is not one of the permitted transformations or if
             the effectValue is not one of the permitted values (dependent on effectType)
See Also:
=============================================================================*/
static int OEMBitmapFX_Effect(OEMBitmapFX* pIIpl, IBitmap* inImage, IBitmap* outImage,
                               int32 nEffectType, int32 nEffectValue)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* iIpl = NULL;
  ipl_image_type* oIpl = NULL;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!pIIpl || !inImage || !outImage )
  {
    return EBADPARM;
  }


  do
  {
    retVal = OEMBitmapFX_GetIPLs(inImage, outImage, &iIpl, &oIpl, TRUE);
    if (retVal)
    {
      break;
    }

    if ((nEffectType == AEEBITMAPFX_FOG) || (nEffectType == AEEBITMAPFX_NEON))
    {
      //Input and output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP
      retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, TRUE, &tempiIpl);
      retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);
    }
    if (retVal)
    {
      break;
    }

    switch(nEffectType){
    case AEEBITMAPFX_GRAYSCALE :
      retVal = OEMBitmapFX_Grayscale(iIpl, oIpl);
      break;
    case AEEBITMAPFX_HUE :
      retVal = OEMBitmapFX_Hue(iIpl, oIpl, nEffectValue);
      break;
    case AEEBITMAPFX_NEGATIVE:
      retVal = OEMBitmapFX_Negative(iIpl, oIpl);
      break;
    case AEEBITMAPFX_SATURATE:
      retVal = OEMBitmapFX_Saturation(iIpl, oIpl, nEffectValue);
      break;
    case AEEBITMAPFX_SEPIA :
      retVal = OEMBitmapFX_Sepia(iIpl, oIpl);
      break;
    case AEEBITMAPFX_POSTERIZE:
      retVal = OEMBitmapFX_Posterize(iIpl, oIpl, nEffectValue);
      break;
    case AEEBITMAPFX_SOLARIZE:
      retVal = OEMBitmapFX_Solarize(iIpl, oIpl, nEffectValue);
      break;
    case AEEBITMAPFX_FLASH :
      retVal = OEMBitmapFX_Flash(iIpl, oIpl);
      break;
    case AEEBITMAPFX_INTENSITY:
      retVal = OEMBitmapFX_Intensity(iIpl, oIpl, nEffectValue);
      break;
    case AEEBITMAPFX_FOG:
      if ((nEffectValue < 0) || (nEffectValue > 1))
      {
        retVal = EBADPARM;
      }
      else
      {
        retVal = (int)ipl_fog(tempiIpl, tempoIpl, (uint8)nEffectValue);
        retVal = OEMBitmapFX_GetErrCode(retVal);
      }
      retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);
      break;
    case AEEBITMAPFX_NEON:
      retVal = (int)ipl_neon(tempiIpl, tempoIpl);
      retVal = OEMBitmapFX_GetErrCode(retVal);
      retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);
      break;
    case AEEBITMAPFX_WHITEBOARD:
      retVal = OEMBitmapFX_Whiteboard(iIpl, oIpl, nEffectValue);
      break;
    default:
      retVal = EBADPARM;
    }
  }while(0);

  OEMBitmapFX_FreeIPLTypeIF(iIpl);
  OEMBitmapFX_FreeIPLTypeIF(oIpl);

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Grayscale
  DESCRIPTION:
    This function will convert the color of input image to gray scale
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Grayscale(const ipl_image_type* iIpl, ipl_image_type* oIpl)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  //Input could be YCbCr 4:2:2 or YCbCr 4:2:0 LP.
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, FALSE, &tempiIpl);
  //Output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP. 
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_image_grayscale(tempiIpl, tempoIpl);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}

/*=====================================================================
FUNCTION OEMBitmapFX_Hue
  DESCRIPTION:
    This function is used to do hue scaling.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
    hue factor: Hue scale factor with range -360 to 360
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Hue(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                            int32 nHue)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  if ((nHue < MIN_HUE_FACTOR) || (nHue > MAX_HUE_FACTOR))
  {
    return EUNSUPPORTED;
  }

  //Input and output could be RGB565 or YCbCr 4:2:2
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, TRUE, &tempiIpl);
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);
  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_hue(tempiIpl, tempoIpl, nHue);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }
    
  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}

/*=====================================================================
FUNCTION OEMBitmapFX_Negative
  DESCRIPTION:
    This function will negate the input image.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Negative(const ipl_image_type* iIpl, ipl_image_type* oIpl)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  //Input must be YCbCr 4:2:2.
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, FALSE, &tempiIpl);
  //Output could be RGB565 or YCbCr 4:2:2. 
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_image_negative(tempiIpl, tempoIpl);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Saturation
  DESCRIPTION:
    This function is used to do saturation scaling.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
    nSaturation: Saturation scale factor with range -255 to 255
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Saturation(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                   int32 nSaturation)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  if ((nSaturation < MIN_SAT_FACTOR) || (nSaturation > MAX_SAT_FACTOR))
  {
    return EUNSUPPORTED;
  }

  //Input and output must be YCbCr 4:2:2
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, FALSE, &tempiIpl);
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, FALSE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_sat(tempiIpl, tempoIpl, nSaturation);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Sepia
  DESCRIPTION:
    This function will convert the input image to sepia.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Sepia(const ipl_image_type* iIpl, ipl_image_type* oIpl)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  //Input must be YCbCr 4:2:2.
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, FALSE, &tempiIpl);
  //Output could be RGB565 or YCbCr 4:2:2. 
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_image_sepia(tempiIpl, tempoIpl);
    if (retVal)
    {
      retVal = OEMBitmapFX_GetErrCode(retVal);
    }
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Posterize
  DESCRIPTION:
    This function will posterize the input image.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
    nPosterize: Number of steps to quantize the input. [0..255]
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Posterize(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                  int32 nPosterize)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  if ((nPosterize < MIN_POS_FACTOR) || (nPosterize > MAX_POS_FACTOR))
  {
    return EUNSUPPORTED;
  }

  //Input could be YCbCr 4:2:2 or YCbCr 4:2:0 LP.
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, FALSE, &tempiIpl);
  //Output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP. 
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_image_posterize(tempiIpl, tempoIpl, (uint8)nPosterize);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Solarize
  DESCRIPTION:
    This function will solarise the input image.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
    nSolarize: Number of steps to quantize the input. [1..255]
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Solarize(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                 int32 nSolarize)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  if ((nSolarize < MIN_SOL_FACTOR) || (nSolarize > MAX_SOL_FACTOR))
  {
    return EUNSUPPORTED;
  }

  //Input and output could be RGB565, YCbCr 4:2:2 or YCbCr 4:2:0 LP
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, TRUE, TRUE, &tempiIpl);
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, TRUE, TRUE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_image_solarize(tempiIpl, tempoIpl, (uint8)nSolarize);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}

/*=====================================================================
FUNCTION OEMBitmapFX_Intensity
  DESCRIPTION:
    This function will do intensity scaling.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
    nIntensity: Intensity scale factor with range -255 to 255
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Intensity(const ipl_image_type* iIpl, ipl_image_type* oIpl,
                                  int32 nIntensity)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  if ((nIntensity < MIN_INT_FACTOR) || (nIntensity > MAX_INT_FACTOR))
  {
    return EUNSUPPORTED;
  }

  //Input and output could be RGB565 or YCbCr 4:2:2
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, TRUE, &tempiIpl);
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);
  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_int(tempiIpl, tempoIpl, nIntensity);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }
    
  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}

/*=====================================================================
FUNCTION OEMBitmapFX_Flash
  DESCRIPTION:
    This function will do intensity equalization and color conversion if required.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Flash(const ipl_image_type* iIpl, ipl_image_type* oIpl)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  //Input must be YCbCr 4:2:2.
  retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, FALSE, &tempiIpl);
  //Output could be RGB565 or YCbCr 4:2:2. 
  retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);

  if (retVal == AEE_SUCCESS)
  {
    retVal = (int)ipl_smart_flash(tempiIpl, tempoIpl);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Whiteboard
  DESCRIPTION:
    This function takes an image taken of a white board, or chalk board, and
    cleans it up for easy viewing and printing.
  PARAMETERS:
    iIpl: Pointer to input Image
    oIpl: Pointer to output image
    threshold: granulatiry when removing non-white board items
               0: high, 1: regular, 2: coarse, 3: do not remove non-white board items.
  LIMITATION:
    ipl API has more options for Whiteboard effect. Here uses default values
    to simplify the user interface.
    blackmode: 0 auto, 1 whiteboard, 2 blackboard, Default: 0
    th_noise: 0 - 255 Default: 10
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Whiteboard(ipl_image_type* iIpl, ipl_image_type* oIpl,
                                   int32 threshold)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type* tempiIpl = NULL;
  ipl_image_type* tempoIpl = NULL;

  if (!iIpl || !oIpl)
  {
    return EBADPARM;
  }

  do
  {
    //Input and output must be YCbCr 4:2:2
    retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, FALSE, &tempiIpl);
    retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, FALSE, &tempoIpl);
    if (retVal)
    {
      break;
    }

    retVal = (int)ipl_efx_Whiteboard(tempiIpl, tempoIpl, 0, 10, threshold, 0);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }while(0);

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);

  return retVal;
}
/*=============================================================================
OEMBitmapFX_DrawLumaHistogram()

Description:

  This function composes a 2D histogram chart over an image.

Return Value:
   Returns AEEError Codes

   AEE_SUCCESS: operation was successful
   EBADPARM: if input params are invalid

See Also:
=============================================================================*/
static int OEMBitmapFX_DrawLumaHistogram(OEMBitmapFX *pIIpl, IBitmap *inImage, IBitmap *outImage,
                                          AEERect *pRect)
{
  int retVal = AEE_SUCCESS;
  ipl_image_type *iIpl = NULL;
  ipl_image_type *oIpl = NULL;
  ipl_image_type *tempiIpl = NULL;
  ipl_image_type *tempoIpl = NULL;
  ipl_histogram_type hist;
  ipl_rect_type *pRectIpl = NULL;
  
  if (!pIIpl || !inImage || !outImage)
  {
    return EBADPARM;
  }

  do
  {
    retVal = OEMBitmapFX_GetIPLs(inImage, outImage, &iIpl, &oIpl, TRUE);
    if (retVal)
    {
      break;
    }
    //Input and output could be RGB565 or YCbCr 4:2:2
    retVal = OEMBitmapFX_GetSupportedIPL(iIpl, TRUE, FALSE, TRUE, &tempiIpl);
    retVal += OEMBitmapFX_GetSupportedIPL(oIpl, FALSE, FALSE, TRUE, &tempoIpl);
    if (retVal)
    {
      break;
    }

    hist.size = 256;
    hist.channel = IPL_CHANNEL_LUMA;
    retVal = (int)ipl_util_CalcHistograms(tempiIpl, NULL, &hist, IPL_QUALITY_HIGH);
    retVal = OEMBitmapFX_GetErrCode(retVal);
    if(retVal)
    {
      break;
    }
    if (pRect)
    {
      pRectIpl = (ipl_rect_type *)MALLOC(sizeof(ipl_rect_type));
      if(!pRectIpl)
      {
        retVal = ENOMEMORY;
        break;
      }
      else
      {
        pRectIpl->dx = (uint16)pRect->dx;
        pRectIpl->dy = (uint16)pRect->dy;
        pRectIpl->x  = (uint16)pRect->x;
        pRectIpl->y  = (uint16)pRect->y;
      }
    }

    //pRectIpl could be NULL pointer. In this case, ipl function will assign
    //a location and size for the histogram.
    retVal = (int)ipl_comp_DrawHistogram(tempiIpl, tempoIpl, &hist, pRectIpl, 255, 128, 0);
    retVal = OEMBitmapFX_GetErrCode(retVal);
  }while(0);

  retVal = OEMBitmapFX_CleanTempIPL(iIpl, &tempiIpl, &tempoIpl, &oIpl, retVal);
  OEMBitmapFX_FreeIPLTypeIF(iIpl);
  OEMBitmapFX_FreeIPLTypeIF(oIpl);

  FREEIF(pRectIpl);
  pRectIpl = NULL;

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_DIBToIPL
  DESCRIPTION:
    This function converts input DIB to output IPL image format.
  PARAMETERS:
    pDIB: Pointer to input Image
    inIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    ENOMEMORY, if no memory to perform the task.
======================================================================*/
static int OEMBitmapFX_DIBToIPL(const IDIB* pDIB, ipl_image_type** inIpl)
{
  ipl_image_type* tmpIpl = NULL;

  if(!pDIB || !inIpl)
  {
    return EBADPARM;
  }
  *inIpl = NULL;
  //Simply check if pDIB is valid
  if (((pDIB->nColorScheme == IDIB_COLORSCHEME_565) && (pDIB->nDepth != 16)) ||
     ((pDIB->nColorScheme == IDIB_COLORSCHEME_888) && (pDIB->nDepth != 24)) ||
     ((pDIB->nColorScheme == IDIB_COLORSCHEME_888) && (pDIB->nDepth != 32)) ||
     ((pDIB->nColorScheme == IDIB_COLORSCHEME_666) && (pDIB->nDepth != 18)))
    return EBADPARM;

  tmpIpl = (ipl_image_type *)MALLOC(sizeof(ipl_image_type));

  if(!tmpIpl)
    return ENOMEMORY;

  tmpIpl->dy = pDIB->cy;
  tmpIpl->imgPtr = pDIB->pBmp;
  switch (pDIB->nColorScheme){
  case IDIB_COLORSCHEME_565:
    tmpIpl->dx = (pDIB->nPitch > 1) ? (int32)(int)((pDIB->nPitch * 8) / pDIB->nDepth) :
                 (int32)pDIB->cx;
    tmpIpl->cFormat = IPL_RGB565;
    break;
  case IDIB_COLORSCHEME_888:
    tmpIpl->dx = (pDIB->nPitch > 2) ? (int32)(int)((pDIB->nPitch * 8) / pDIB->nDepth) :
                 (int32)pDIB->cx;
    tmpIpl->cFormat = IPL_RGB888;
    break;
  case IDIB_COLORSCHEME_666:
    tmpIpl->dx = (pDIB->nPitch > 1) ? (int32)(int)((pDIB->nPitch * 8) / pDIB->nDepth) :
                 (int32)pDIB->cx;
    tmpIpl->cFormat = IPL_RGB666;
    break;
   
  default:
    FREE (tmpIpl);
    tmpIpl = NULL;
    return EUNSUPPORTED;
  }


  *inIpl = tmpIpl;
  return AEE_SUCCESS;
}

/*=====================================================================
FUNCTION OEMBitmapFX_YCbCrToIPL
  DESCRIPTION:
    This function converts input YCbCr to output IPL image format.
  PARAMETERS:
    pYCbCr: Pointer to input Image
    inIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    ENOMEMORY, if no memory to perform the task.
======================================================================*/
static int  OEMBitmapFX_YCbCrToIPL(const IYCbCr* pYCbCr, ipl_image_type** inIpl)
{
  ipl_image_type* tmpIpl = NULL;

  if(!inIpl || !pYCbCr)
  {
    return EBADPARM;
  }

  *inIpl = NULL;

  tmpIpl = (ipl_image_type *)MALLOC(sizeof(ipl_image_type));

  if(!tmpIpl)
  {
    return ENOMEMORY;
  }

  tmpIpl->dx = pYCbCr->cx;
  tmpIpl->dy = pYCbCr->cy;
  tmpIpl->imgPtr = pYCbCr->pLuma;
  tmpIpl->clrPtr = pYCbCr->pChroma;
  switch (pYCbCr->uScheme){
  case IYCBCR_COLORSCHEME_420LP:
    tmpIpl->cFormat = IPL_YCbCr420_LINE_PK;
    break;
  case IYCBCR_COLORSCHEME_422LP:
    tmpIpl->cFormat = IPL_YCbCr422_LINE_PK;
    break;
  case IYCRCB_COLORSCHEME_420LP:
    tmpIpl->cFormat = IPL_YCrCb420_LINE_PK;
    break;
  case IYCRCB_COLORSCHEME_422LP:
    tmpIpl->cFormat = IPL_YCrCb422_LINE_PK;
    break;

  default:
    FREE (tmpIpl);
    tmpIpl = NULL;
    return EUNSUPPORTED;
  }

  *inIpl = tmpIpl;
  return AEE_SUCCESS;
}

/*=====================================================================
FUNCTION OEMBitmapFX_CreateIPL
  DESCRIPTION:
    This function is used to create temporary IPL.
  PARAMETERS:
    outIpl: Pointer to IPL
    width: Width of input frame.
    height: Height of  input frame
    colorFormat: Type of Color format
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    ENOMEMORY, if no memory to perform the task.
======================================================================*/
static int OEMBitmapFX_CreateIPL(ipl_image_type** poutIpl, uint16 nwidth, uint16 nheight,
                                  ipl_col_for_type colorFormat)
{
  ipl_image_type* tmpIpl = NULL;

  if (!poutIpl)
  {
    return EBADPARM;
  }

  *poutIpl = NULL;

  tmpIpl = (ipl_image_type *)MALLOC(sizeof(ipl_image_type));
  if(!tmpIpl)
  {
    return ENOMEMORY;
  }

  switch (colorFormat){
  case IPL_RGB565:
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*2);
    break;
  case IPL_RGB666: 
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*3);
    break;
  case IPL_RGB888:
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*3);
    break;
  case IPL_YCbCr:
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*2);
    break;
  case IPL_YCbCr420_LINE_PK:
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*3/2);
    tmpIpl->clrPtr = tmpIpl->imgPtr + (nwidth*nheight);
    break;
  case IPL_YCbCr422_LINE_PK: 
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*2);
    tmpIpl->clrPtr = tmpIpl->imgPtr + (nwidth*nheight);
    break;
  case IPL_YCrCb420_LINE_PK:
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*3/2);
    tmpIpl->clrPtr = tmpIpl->imgPtr + (nwidth*nheight);
    break;
  case IPL_YCrCb422_LINE_PK:
    tmpIpl->imgPtr = (unsigned char *)MALLOC(nwidth*nheight*2);
    tmpIpl->clrPtr = tmpIpl->imgPtr + (nwidth*nheight);
    break;
  default:
    FREE (tmpIpl);
    tmpIpl = NULL;
    return EBADPARM;
  }

  if(!tmpIpl->imgPtr)
  {
    FREE(tmpIpl);
    tmpIpl = NULL;
    return ENOMEMORY;
  }
  else
  {
    tmpIpl->dx = nwidth;
    tmpIpl->dy = nheight;
    tmpIpl->cFormat = colorFormat;
  }

  *poutIpl = tmpIpl;
  return AEE_SUCCESS;
}

/*=====================================================================
FUNCTION OEMBitmapFX_FreeIPLTypeIF
  DESCRIPTION:
    This function is used to free IPL type
  PARAMETERS:
    inIpl: Pointer to (ipl_image_type *)
  RETURN VALUE:
    No return value
======================================================================*/
static void OEMBitmapFX_FreeIplTypeIf(ipl_image_type** inIpl)
{
  if(inIpl)
  {
    FREEIF(*inIpl);
    *inIpl = NULL;
  }
}
/*=====================================================================
FUNCTION OEMBitmapFX_FreeIplDataIf
  DESCRIPTION:
    This function is used to free IPL data
  PARAMETERS:
    inIpl: Pointer to (ipl_image_type *)
  RETURN VALUE:
    No return value
  ======================================================================*/
static void OEMBitmapFX_FreeIplDataIf(ipl_image_type** inIpl)
{
  if (inIpl && *inIpl)
  {
    FREEIF((*inIpl)->imgPtr);
    (*inIpl)->imgPtr = NULL;
    FREE(*inIpl);
    *inIpl = NULL;
  }
}
/*=====================================================================
FUNCTION OEMBitmapFX_GetIPL
  DESCRIPTION:
    This is a sharing function between different features.
  PARAMETERS:
    pinImg: Pointer to input image
    iIpl: Pointer to IPL type image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
========================================================================*/
static int OEMBitmapFX_GetIPL(IBitmap* pinImg, ipl_image_type** iIpl)
{
  int retval = AEE_SUCCESS;
  IDIB *iDIB = NULL;

  if (!pinImg || !iIpl)
  {
    return EBADPARM;
  }

  //Retrieve the input DIB
  if(IBITMAP_QueryInterface(pinImg, AEECLSID_DIB, (void **)&iDIB) != AEE_SUCCESS)
    return EUNSUPPORTED;

  //Convert the Input DIB to an Ipl Image type
  retval = OEMBitmapFX_DIBToIPL(iDIB, iIpl);

  //Cleanup
  RELEASEIF(iDIB);

  return retval;
}
/*=====================================================================
FUNCTION OEMBitmapFX_GetIPLs
  DESCRIPTION:
    This is a sharing function between different features.
  PARAMETERS:
    pinImg: Pointer to input image
    poutImg: Pointer to output image
    iIpl: Pointer to input IPL type image
    oIpl: Pointer to output IPL type image
    bCheckDIB: Boolean variable
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
  ======================================================================*/
static int OEMBitmapFX_GetIPLs(IBitmap* pinImg, IBitmap* poutImg, ipl_image_type** iIpl,
                                ipl_image_type** oIpl, boolean bCheckSize)
{
  int retval = AEE_SUCCESS;

  if (!pinImg || !poutImg || !iIpl || !oIpl)
  {
    return EBADPARM;
  }

  retval = OEMBitmapFX_GetIPL(pinImg, iIpl);
    if (retval)
    {
      retval = OEMBitmapFX_GetYCbCrIPL(pinImg, iIpl);
      if (retval)
      {
        return retval;
      }
    }

  retval = OEMBitmapFX_GetIPL(poutImg, oIpl);
    if (retval)
    {
      retval = OEMBitmapFX_GetYCbCrIPL(poutImg, oIpl);
      if (retval)
      {
        return retval;
      }
    }

  if(bCheckSize)
  {
    //Check to see that the input and output images are of the same size
    if(((*iIpl)->dx != (*oIpl)->dx) || ((*iIpl)->dy != (*oIpl)->dy))
    {
      retval = EUNSUPPORTED;
    }
  }
  return retval;
}
/*=====================================================================
FUNCTION OEMBitmapFX_GetYCbCrIPL
  DESCRIPTION:
    This is a sharing function and Helper function to convert YCbCr to IPL
  PARAMETERS:
    pinImg: Pointer to input image
    iIpl: Pointer to IPL type image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
  ======================================================================*/
static int OEMBitmapFX_GetYCbCrIPL(IBitmap* pinImg, ipl_image_type** iIpl)
{
  int retval = AEE_SUCCESS;
  IYCbCr* iYCbCr= NULL;

  if (!pinImg || !iIpl)
  {
    return EBADPARM;
  }

  //Retrieve the input IYCbCr
  if(IBITMAP_QueryInterface(pinImg, AEEIID_YCBCR, (void **)&iYCbCr) != AEE_SUCCESS)
  {
    return EUNSUPPORTED;
  }

  //Convert the Input IYCbCr to an Ipl Image type
  retval = OEMBitmapFX_YCbCrToIPL(iYCbCr, iIpl);

  //Cleanup
  RELEASEIF(iYCbCr);

  return retval;
}

static int OEMBitmapFX_GetErrCode(int nVal)
{
  int retVal = AEE_SUCCESS;

  switch(nVal){
  case IPL_SUCCESS:
    retVal = AEE_SUCCESS;
    break;
  case IPL_FAILURE:
    retVal = EFAILED;
    break;
  case IPL_NO_MEMORY:
    retVal = ENOMEMORY;
    break;
  case IPL_UNSUPPORTED:
    retVal = EUNSUPPORTED;
    break;
  default:
    break;
  }
  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_Convert2YCbCr
  DESCRIPTION:
    This function will Check the color of input image and convert it to YCbCr.
  PARAMETERS:
    iIpl: Pointer to input Image wiil be IPL_RGB565,IPL_RGB888,IPL_RGB666,YCbCr, YCbCr422_LINE_PK,YCbCr422_LINE_PK
    outIpl: Pointer to output image where we are going to stored the new image data. YCbCr
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/
static int OEMBitmapFX_Convert2YCbCr(const ipl_image_type* iIpl, ipl_image_type** outIpl)
{
  int retval = AEE_SUCCESS;
  ipl_image_type* tempIpl = NULL;

  if (!iIpl || !outIpl){
    return EBADPARM;
  }

  switch (iIpl->cFormat){
  case IPL_YCbCr:
    *outIpl = (ipl_image_type *)iIpl;
    return retval;
  case IPL_RGB565:
  case IPL_RGB888:
  case IPL_RGB666:
  case IPL_YCbCr420_LINE_PK:
  case IPL_YCbCr422_LINE_PK:
  case IPL_YCrCb420_LINE_PK:
  case IPL_YCrCb422_LINE_PK: 
    retval = OEMBitmapFX_CreateIPL(&tempIpl, (uint16)iIpl->dx, (uint16)iIpl->dy, IPL_YCbCr);
    if (retval)
      return retval;

    retval = (int)ipl_convert_image((ipl_image_type *)iIpl, tempIpl);
    retval = OEMBitmapFX_GetErrCode(retval);
    break;
  default:
    retval = EUNSUPPORTED;
  }

  if (retval == AEE_SUCCESS){
    *outIpl = tempIpl;
  }

  return retval;
}
/*===================================================================== 
FUNCTION OEMBitmapFX_GetSupportedIPL
  DESCRIPTION:
    This function will Check the color of input image decide if need to convert it to YCbCr.
  PARAMETERS:
    iIpl: Pointer to input Image
    bInput: Flag to tell if the iIpl is for function input or output
    nType: The operating type of ipl function
    outIpl: Pointer to output image
  RETURN VALUE:
    Returns AEEError codes
    AEE_SUCCESS, if the function completed successfully
    EUNSUPPORTED,.if the API is not supported.
    EFAILED, if failure in performing the feature.
======================================================================*/ 
static int OEMBitmapFX_GetSupportedIPL(const ipl_image_type* iIpl, boolean bInput,
                                        boolean bOkLP, boolean bOkRGB565,
                                        ipl_image_type** oIpl)
{
  int retVal = AEE_SUCCESS;
  boolean bConvert = FALSE;
  ipl_image_type* tempIpl = (ipl_image_type *)iIpl;

  if (!iIpl || !oIpl){
    return EBADPARM;
  }

  switch (iIpl->cFormat){
  case IPL_YCbCr:
    break;
  case IPL_RGB565:
    if (!bOkRGB565)
      bConvert = TRUE;
    break;
  case IPL_YCbCr420_LINE_PK:
  case IPL_YCrCb420_LINE_PK:
    if (!bOkLP)
      bConvert = TRUE;
    break;
  default:
    bConvert = TRUE;
    break;
  }

  if (bConvert)
  {
    if (bInput)
    {
      retVal = OEMBitmapFX_Convert2YCbCr(iIpl, &tempIpl);
    }
    else
    {
      retVal = OEMBitmapFX_CreateIPL(&tempIpl, (uint16)iIpl->dx, (uint16)iIpl->dy, IPL_YCbCr);
    }
  }

  if (retVal == AEE_SUCCESS)
  {
    *oIpl = tempIpl;
  }

  return retVal;
}
/*=====================================================================
FUNCTION OEMBitmapFX_CleanTempIPL
  DESCRIPTION: This function clean up the tempiIpl and tempoIpl buffers if available.
               It also converts the tempoIpl back to oIpl format.   
  PARAMETERS:
    iIpl - Pointer to input Image
    *tempiIpl - Pointer to temp input Image
    *tempoIpl - Pointer to temp output Image
    *oIpl - Pointer to output Image
    retval - error code to tell if do the output conversion
  RETURN VALUE:
    Returns AEEError codes after conversion or the input error code
======================================================================*/
static int OEMBitmapFX_CleanTempIPL(const ipl_image_type* iIpl, ipl_image_type** tempiIpl,
                                     ipl_image_type** tempoIpl, ipl_image_type** oIpl, 
                                     int retval)
{
  int retVal = retval;

  if (iIpl && tempiIpl && *tempiIpl)
  {
    if (iIpl->cFormat != (*tempiIpl)->cFormat)
    {
      OEMBitmapFX_FreeIPLDataIF(*tempiIpl);
    }
  }

  if (oIpl && tempoIpl && *oIpl && *tempoIpl)
  {
    if ((*oIpl)->cFormat != (*tempoIpl)->cFormat)
    {
      if (retval == AEE_SUCCESS)
      {
        retVal = (int)ipl_convert_image(*tempoIpl, *oIpl);
        retVal = OEMBitmapFX_GetErrCode(retVal);
      }
      OEMBitmapFX_FreeIPLDataIF(*tempoIpl);
    }
  }
  return retVal;
}
#endif //#ifdef FEATURE_IIPL
