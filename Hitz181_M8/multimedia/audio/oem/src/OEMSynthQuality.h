#ifndef OEMSYNTHQUALITY_H
#define OEMSYNTHQUALITY_H

/*============================================================================
FILE: OEMSynthQuality.h

SERVICES:  BREW MIDI Synthesizer Quality APIs

DESCRIPTION:
   This file defines interface for BREW MIDI Synthesizer Quality

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
/*===========================================================================
 $Header: //source/qcom/qct/multimedia/audio/oem/BREW3.1.5/main/latest/src/OEMSynthQuality.h#1 $
============================================================================*/
//===================================================================
//      Include Files
//===================================================================
#include "AEE.h"


//*********************************************************************
//
// ISynthQuality interface definition
//
//*********************************************************************
//==========================================================================
//   Macro definitions
//==========================================================================
#define AEEIID_SYNTHQUALITY   0x01037784

//==========================================================================
//   Type definitions
//==========================================================================
//
// SynthQuality modes.
// HP mode: High Polyphony mode. The number of notes (voices) is high at normal quality.
// HQ mode: High Quality mode. Very high quality notes at the expense of number of notes.
//
#define AEE_SYNTH_MODE_HP_ALL        0
#define AEE_SYNTH_MODE_HP_HI         1
#define AEE_SYNTH_MODE_HP_HI_MED     2   
#define AEE_SYNTH_MODE_HP_MED        3
#define AEE_SYNTH_MODE_HQ_MED        4
#define AEE_SYNTH_MODE_HQ_HI_MED     5
#define AEE_SYNTH_MODE_HQ_HI         6
#define AEE_SYNTH_MODE_HQ_ALL        7

//===================================================================
//    Interface Definition
//===================================================================
#define INHERIT_ISynthQuality(iname)\
   INHERIT_IQueryInterface(iname);\
   \
   int (*SetSynthMode)(iname *po, int nSynthMode);\
   int (*GetSynthMode)(iname *po, int *pnSynthMode)


AEEINTERFACE_DEFINE(ISynthQuality);

//===============================================================================
//    API Definition
//===============================================================================
static __inline uint32 ISYNTHQUALITY_AddRef(ISynthQuality *me)
{
   return AEEGETPVTBL(me,ISynthQuality)->AddRef(me);
}

static __inline uint32 ISYNTHQUALITY_Release(ISynthQuality *me)
{
   return AEEGETPVTBL(me,ISynthQuality)->Release(me);
}

static __inline uint32 ISYNTHQUALITY_QueryInterface(ISynthQuality *me, AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,ISynthQuality)->QueryInterface(me, cls, ppo);
}

static __inline int ISYNTHQUALITY_SetSynthMode(ISynthQuality *me, int nSynthMode)
{
   return AEEGETPVTBL(me,ISynthQuality)->SetSynthMode(me, nSynthMode);
}

static __inline int ISYNTHQUALITY_GetSynthMode(ISynthQuality *me, int *pnSynthMode)
{
   return AEEGETPVTBL(me,ISynthQuality)->GetSynthMode(me, pnSynthMode);
}


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

===================================================================== 
  INTERFACES   DOCUMENTATION
===================================================================== 

ISynthQuality interface provides the ability to configure MIDI sythesizer quality.

Two synth modes are defined and these represent the quality of playback:
HP mode: High Polyphony mode. The number of notes (voices) is high at normal quality.
HQ mode: High Quality mode. Very high quality notes at the expense of number of notes.

ISynthQuality allows the synthesizer to be configured at several levels of HP/HQ notes.

ISYNTHQUALITY_SetSynthMode() API needs to be called before IMEDIA_Play() and before the
DLS files are loaded.

=============================================================================

ISYNTHQUALITY_SetSynthMode()

Description:
   This function sets the MIDI synthesizer mode.

Prototype:
   int ISYNTHQUALITY_SetSynthMode(ISynthQuality *me, int nSynthMode);

Parameters:
   me :  [in]:  Pointer to ISynthQuality Interface.

   nSynthMode:  [in]:  Synth mode specified by AEE_SYNTH_MODE_XXX. See comments.

Return Value:
   EBADPARM: Bad parm
   EITEMBUSY: Synthesizer is busy. Try later.
   EBADSTATE: Cannot set parm in the current state
   EUNSUPPORTED: Parm not supported by the class
   EFAILED: General failure

Comments:
   Currently, the following are the supported modes that specify 
   high polyphony(HP) / high quality(HQ) levels:
   AEE_SYNTH_MODE_HP_ALL: All HP-mode
   AEE_SYNTH_MODE_HP_HI     
   AEE_SYNTH_MODE_HP_HI_MED 
   AEE_SYNTH_MODE_HP_MED    
   AEE_SYNTH_MODE_HQ_MED    
   AEE_SYNTH_MODE_HQ_HI_MED 
   AEE_SYNTH_MODE_HQ_HI     
   AEE_SYNTH_MODE_HQ_ALL: All HQ-mode

Version:
   Introduced BREW SDK 3.1.5

See Also:
   ISYNTHQUALITY_GetSynthMode()

=============================================================================

ISYNTHQUALITY_GetSynthMode()

Description:
   This function gets the current mode of MIDI synthesizer.

Prototype:
   int ISYNTHQUALITY_GetSynthMode(ISynthQuality *me, int nSynthMode);

Parameters:
   me :  [in]:  Pointer to ISynthQuality Interface.

   pnSynthMode:  [out]:  Pointer to current Synth mode, AEE_SYNTH_MODE_XXX. 
                         See comments of ISYNTHQUALITY_SetSynthMode()

Return Value:
   EBADPARM: Bad parm
   EITEMBUSY: Synthesizer is busy. Try later.
   EBADSTATE: Cannot set parm in the current state
   EUNSUPPORTED: Parm not supported by the class
   EFAILED: General failure

Comments:
   None

Version:
   Introduced BREW SDK 3.1.5

See Also:
   ISYNTHQUALITY_SetSynthMode()


============================================================================= */

#endif // OEMSYNTHQUALITY_H
