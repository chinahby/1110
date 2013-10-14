/*=============================================================================

FILE: AEEAlarm.h
 
SERVICES: The IAlarm object manages alarms for other applets.

DESCRIPTION:
   This object provides the same functionality as ISHELL_SetAlarm() and
   ISHELL_CancelAlarm().  It also all the active alarms to be suspended
   and then later resumed, specifically for LPM.

   Hopefully eventually the native ISHELL alarms will support this feature
   and this entire object can be removed.

   THIS OBJECT ASSUMES THAT THE PHONE HAS VALID TIME.  DO NOT INVOKE ANY
   METHODS IN THIS OBJECT IF THIS IS NOT THE CASE.  IT IS OK, HOWEVER, TO 
   CREATE THE OBJECT WHILE THE PHONE DOES NOT HAVE VALID TIME.


PUBLIC CLASSES AND STATIC FUNCTIONS:
   IAlarm

INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //tsg/UI/main/AEE/inc/AEEAlarm.h#3 $
$DateTime: 2002/08/15 15:08:09 $
  $Author: mvines $
  $Change: 54873 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
08/14/02   mjv     IAlarm is now simply a BREW class, instead of 
                   an applet.
07/19/02   lnr     Removed the DECLARE_IAPPLET definition to use the 
                   INHERIT_IApplet.
02/28/02   mjv     Added IModule * parameter to IAlarm_New()
12/12/01   mjv     Initial revision
 
=============================================================================*/

#ifndef __ALARM_H__
#define __ALARM_H__

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/
////
// EFS file that is used to store the alarm info
#define ALARM_EFS_FILE  "fs:/sys/alarms.db"



/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/


/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/
typedef struct _IAlarm IAlarm;

QINTERFACE(IAlarm)
{
   INHERIT_IBase(IAlarm);
   int (*SetAlarm)(IAlarm *p, AEECLSID cls, uint16 nUserCode, uint32 nMins);
   int (*CancelAlarm)(IAlarm *p, AEECLSID cls, uint16 nUserCode);
   void (*SuspendAlarms)(IAlarm *p);
   void (*ResumeAlarms)(IAlarm *p);
   void (*RemAllAlarm)(IAlarm *p);
};


/*===========================================================================

                    METHOD DECLARATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: AEEAlarm_New

DESCRIPTION: Creates a new IAlarm interface

PARAMETERS:
	*ps [in]: IShell pointer
   cls [in]: Class to create
*ppif [out]: IAlarm interface double pointer

RETURN VALUE:
int: SUCCESS if successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int AEEAlarm_New(IShell * piShell, AEECLSID cls, void **ppif);


/*=============================================================================
FUNCTION: IAlarm_AddRef

DESCRIPTION: Add a reference to this object

PARAMETERS:
	*p: IAlarm interface pointer

RETURN VALUE:
uint32: the new reference count

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IAlarm_AddRef(p)                GET_PVTBL(p,IAlarm)->AddRef(p)


/*=============================================================================
FUNCTION: IAlarm_Release

DESCRIPTION: Remove a reference from this object

PARAMETERS:
	*p: IAlarm interface pointer 

RETURN VALUE:
uint32: the new reference count

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IAlarm_Release(p)               GET_PVTBL(p,IAlarm)->Release(p)


/*=============================================================================
FUNCTION:  IAlarm_SetAlarm
 
DESCRIPTION: Allows the caller to set a long-term alarm.
 
PARAMETERS:
   *p: IAlarm interface pointer
   cls: Applet class to call when the alarm expires
   nUserCode: 16-bit code passed to the applet
   nMins: Number of minutes to set the alarm from the current time
 
RETURN VALUE:
   SUCCESS  if operation is successful

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IAlarm_SetAlarm(p,cls,u,m)      GET_PVTBL(p,IAlarm)->SetAlarm(p,cls,u,m)


/*=============================================================================
FUNCTION:  IAlarm_CancelAlarm
 
DESCRIPTION: Cancels an alarm previously set via IAlarm_SetAlarm()
 
PARAMETERS:
   *p: IAlarm interface pointer
   cls: Applet class of alarm to cancel
   nUserCode: 16-bit user code of alarm to cancel
 
RETURN VALUE:
   SUCCESS  if alarm is found and canceled
   EFAILED  if alarm can not be found or canceled

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
#define IAlarm_CancelAlarm(p,cls,u)     GET_PVTBL(p,IAlarm)->CancelAlarm(p,cls,u)


/*=============================================================================
FUNCTION:  IAlarm_SuspendAlarms
 
DESCRIPTION: Suspends all active alarms
 
PARAMETERS:
   *p: IAlarm interface pointer 
 
RETURN VALUE:
   None

COMMENTS: 
   This method may be called when are no active alarms.

SIDE EFFECTS:

SEE ALSO:
   IAlarm_ResumeAlarms

=============================================================================*/
#define IAlarm_SuspendAlarms(p)         GET_PVTBL(p,IAlarm)->SuspendAlarms(p)


/*=============================================================================
FUNCTION:  IAlarm_ResumeAlarms
 
DESCRIPTION:  Resumes the alarms previously suspended by 
              IAlarm_SuspendAlarms()
 
PARAMETERS:
   *p: IRefIAlarm interface pointer
 
RETURN VALUE:
   None

COMMENTS:
   This method may be called when are no alarms to resume.

SIDE EFFECTS:

SEE ALSO:
   IAlarm_SuspendAlarms

=============================================================================*/
#define IAlarm_ResumeAlarms(p)          GET_PVTBL(p,IAlarm)->ResumeAlarms(p)

#define IAlarm_RemAllAlarm(p)     GET_PVTBL(p,IAlarm)->RemAllAlarm(p)


#endif 
