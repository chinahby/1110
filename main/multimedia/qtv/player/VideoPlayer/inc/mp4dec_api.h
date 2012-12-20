#ifndef __MP4DEC_API_H__
#define __MP4DEC_API_H__
/* =======================================================================
                              mp4dec_api.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/player/videoplayer/main/latest/inc/mp4dec_api.h#2 $
$DateTime: 2009/03/18 21:44:35 $
$Change: 866593 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "MP4_Types.h"
#ifdef PLATFORM_LTK
#error code not present
#endif // PLATFORM_LTK
class Mpeg4Player;
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef struct tagvideoDecControls
{
  uint32 audioFormat;
} VideoDecControls;


typedef enum
{
  MP4_I_FRAME, MP4_P_FRAME, MP4_B_FRAME, MP4_BAD_FRAME
} MP4FrameType;

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/* ======================================================================
FUNCTION 
  QtvResetDSP

DESCRIPTION
  This function disables and then resets the DSP. 

DEPENDENCIES
    None
  
RETURN VALUE
   Void
   
SIDEEFFECTS
      None.
========================================================================== */
void QtvResetDSP(void);

/* ======================================================================
FUNCTION 
  PVDisplayVideoFrame

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
boolean PVDisplayVideoFrame(void *pRGBFrame, void *pYUVFrame, 
                            uint16 SrcHeight, uint16 SrcWidth, 
                            uint32 Timestamp, uint32 NumConcealedMB,
                            uint8 fVOP, void *pPostFilterMbInfo, unsigned long NumIntraMbs,
                            unsigned short Rounded_width, unsigned short Rounded_height, 
                            Mpeg4Player *pMpeg4Player);

/* ======================================================================
FUNCTION 
  PVPeekFrameType

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
MP4FrameType PVPeekFrameType(uint8 *buff);

/* ======================================================================
FUNCTION 
  IsIntraFrame

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
boolean IsIntraFrame(VideoDecControls *decCtrl);

/* ======================================================================
FUNCTION 
  PVGetPrevRGBFrame

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
boolean PVGetPrevRGBFrame(unsigned short *pCurRGBBuf, unsigned short **ppRGBBuf);

/* ======================================================================
FUNCTION 
  PVGetPrevYUVFrame

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
boolean PVGetPrevYUVFrame(unsigned short *pCurYUVBuf, unsigned short **ppYUVBuf);

#ifdef FEATURE_MP4_FRAME_TRANSFORMATIONS
/* ======================================================================
FUNCTION 
  PVRotateVideo

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
bool PVRotateVideo(MP4TransformType VideoRotation);

/* ======================================================================
FUNCTION 
  PVScaleVideo

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
bool PVScaleVideo(MP4TransformType VideoScaling);

#endif /* FEATURE_MP4_FRAME_TRANSFORMATIONS */

#endif // __MP4DEC_API_H__

