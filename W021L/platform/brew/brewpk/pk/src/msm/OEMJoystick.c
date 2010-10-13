/*===========================================================================

FILE: OEMJoystick.c

SERVICES: IJoystick interface implementation

DESCRIPTION
   This file implements BREW IJoystick. IJoystick controls the configuration 
   and reads from the device joystick. 
   
PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#if defined(FEATURE_BREW_JOYSTICK)     

#include "OEMCriticalSection.h"
#include "OEMFS.h"
#include "OEMJoystick.h"
#include "AEE_OEMFile.h"    
#include "AEEPosDet.h"
#include "AEEStdLib.h"
#include "joyst.h"
#include "joystc.h"

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/

/* Enumeration of available joystick hardware instances. */
enum {
  OEMJOYSTICK_1 = 0,  /* Corresponds to AEECLSID_IJOYSTICK1 */
  OEMJOYSTICK_2,      /* Corresponds to AEECLSID_IJOYSTICK2, unused for now */

  OEMJOYSTICK_MAX     /* for range checking */
};

/*===========================================================================

                      IJoystick Static Variables

===========================================================================*/    

/* Reference count per joystick hardware instance */
static unsigned int oem_joystick_refcnt[OEMJOYSTICK_MAX];

/*===============================================================================

                     IJoystick Function Definitions

===============================================================================*/
/*==================================================================

==================================================================*/
void OEMJoystick_Init(void)
{
   // Empty
}

/*==================================================================

==================================================================*/
int OEMJoystick_New(AEECLSID cls)
{
  switch ( cls )
  {
   case AEECLSID_IJOYSTICK1:
      /* Increment the reference count, and
       * start the joystick service if it's the first reference.
       */
      if ( ++oem_joystick_refcnt[OEMJOYSTICK_1] == 1 )
      {
        joyst_start();
      }
      return (SUCCESS);
   case AEECLSID_IJOYSTICK2:
   default:
      return (EUNSUPPORTED);
   }
}

/*==================================================================

==================================================================*/
void OEMJoystick_Delete(AEECLSID cls)
{
  switch ( cls )
  {
   case AEECLSID_IJOYSTICK1:
      /* Decrement the reference count, and
       * stop the joystick service if it's the last reference.
       */
      if ( (oem_joystick_refcnt[OEMJOYSTICK_1] > 0) && 
           (--oem_joystick_refcnt[OEMJOYSTICK_1] == 0 ) )
      {
        joyst_stop();
      }
	  break;
   case AEECLSID_IJOYSTICK2:
   default:
      return;
   }
}

/*==================================================================

==================================================================*/
int OEMJoystick_SetParm(AEECLSID cls, int16 nParmID, int32 p1, int32 p2)
{
   boolean            nRet; 

   switch (cls) {
   case AEECLSID_IJOYSTICK1:
      switch (nParmID) {
      case JOY_PARM_SAMPLEFREQ:
         nRet = joyst_config_sample_rate( (uint8)p1 );
         break;
      case JOY_PARM_INACTIVITY:
         nRet = joyst_config_inactivity( (uint16)p1 );
         break;
      case JOY_PARM_SF_IC:
         nRet = joyst_config_joystick( (uint8)p1, (uint16)p2 );
         break;
      default:
         return EUNSUPPORTED;
	  }
	  break;
   case AEECLSID_IJOYSTICK2:
   default:
      return EUNSUPPORTED;  
   }

   return nRet ? SUCCESS:EFAILED;
}

/*==================================================================

==================================================================*/
uint32 OEMJoystick_GetParm(AEECLSID cls, int16 nParmID, uint32 *nErr)
{
   if (!nErr)
      return 0;
      
   switch (cls) {
   case AEECLSID_IJOYSTICK1:
      switch (nParmID) {
      case JOY_PARM_MAXCOUNT:
         *nErr = SUCCESS;
         return JOY_MAX_INACTIVITY;
      case JOY_PARM_MAXFREQ:
         *nErr = SUCCESS;
         return JOY_MAX_SAMPLEFREQ;
      }
	  *nErr = EUNSUPPORTED;
	  return 0;
      break;
   case AEECLSID_IJOYSTICK2:
   default:
      *nErr = EUNSUPPORTED;
      return 0;
   }
}

/*==================================================================

==================================================================*/
uint32 OEMJoystick_Read(AEECLSID cls)
{
   switch (cls) {
   case AEECLSID_IJOYSTICK1:
      return joyst_get_xy();
      break;
   case AEECLSID_IJOYSTICK2:
   default:
      return 0;
   }
}

/*==================================================================  */

#endif // defined(FEATURE_BREW_JOYSTICK)


