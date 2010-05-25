#ifndef AEEAPPLETCTL_H
#define AEEAPPLETCTL_H

/*============================================================================
FILE: AEEAppletCtl.h

SERVICES:  BREW Applet Control Services

DESCRIPTION:
   This file defines IAPPLETCTL interface that allows BREW applications to 
   query and control running applets.  Some operations require PL_SYSTEM
   privilege.
   

PUBLIC CLASSES:  IAPPLETCTL

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"
#include "AEEShell.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/



/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef struct
{
   uint16         wFlags;        // SSA flags
   const char *   pszStart;      // text startup string (passed to applet)
   uint32         dwReason;      // start reason code.  See RESCTL_REASON_*
} AppStartInfo;

#define APPLETCTL_OP_START       1  // start an applet
#define APPLETCTL_OP_CAN_START   2  // check if an applet can be started
#define APPLETCTL_OP_STOP        3  // stop a currently running applet


//Parameter definitions to be passed to IAPPLETCTL_GetRunningInfo()
typedef int AEEAppItem;

#define AEE_APPITEM_STATE           1   // current state of applet
#define AEE_APPITEM_START_TIME      2   // start time relative to GETUPTIMEMS()
#define AEE_APPITEM_MODULE_ID       3   // CLSID of module
#define AEE_APPITEM_HEAP_USAGE      4   // number of byte of heap used (by all applets associated w/ module)
#define AEE_APPITEM_MAX_HEAP_USAGE  5   // maximum number of bytes of heap used by applets associated w/ module.
#define AEE_APPITEM_APPINFO         6   // This returns the same information provided by ISHELL_EnumNextApplet()
#define AEE_APPITEM_CUR_RUNNING_APP 7   // Returns ClassID of the currently running applet

// The following states are returned by AEE_APPLETITEM_STATE
#define APPSTATE_STOPPED         0     // Applet is stopped
#define APPSTATE_TOP_VISIBLE     1     // Applet is topmost visible application (foreground)
#define APPSTATE_SUSPENDED       2     // Applet is suspended
#define APPSTATE_BACKGROUND      3     // Applet is running in the background
#define APPSTATE_STARTING        4     // Applet is starting
#define APPSTATE_CLOSING         5     // Applet is closing


//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//=============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//=============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

//*****************************************************************************
//
// IAPPLETCTL Interface
//
//*****************************************************************************

#define INHERIT_IAppletCtl(iname)\
   INHERIT_IQueryInterface(iname);\
   int      (*GetRunningList)(iname *po, void *pBuff, int *pnSize);\
   int      (*GetRunningInfo)(iname *po, AEECLSID cls, AEEAppItem nItem, void *pBuff, int *pnSize);\
   int      (*Control)(iname *po, AEECLSID cls, int op, void *pBuff, int *pnSize)

AEEINTERFACE_DEFINE(IAppletCtl);

#define IAPPLETCTL_AddRef(p)                    AEEGETPVTBL(p, IAppletCtl)->AddRef(p)
#define IAPPLETCTL_Release(p)                   AEEGETPVTBL(p, IAppletCtl)->Release(p)
#define IAPPLETCTL_QueryInterface(p,i,p2)       AEEGETPVTBL(p, IAppletCtl)->QueryInterface((p),(i),(p2))

#define IAPPLETCTL_GetRunningList(p, pb, pn)     AEEGETPVTBL(p, IAppletCtl)->GetRunningList((p),(pb),(pn))
#define IAPPLETCTL_GetRunningInfo(p,c,i,pb,pn)   AEEGETPVTBL(p, IAppletCtl)->GetRunningInfo((p),(c),(i),(pb),(pn))
#define IAPPLETCTL_Start(p,c,pb,pn)              AEEGETPVTBL(p, IAppletCtl)->Control((p),(c),APPLETCTL_OP_START,(pb),(pn))
#define IAPPLETCTL_CanStart(p,c,pb,pn)           AEEGETPVTBL(p, IAppletCtl)->Control((p),(c),APPLETCTL_OP_CAN_START,(pb),(pn))
#define IAPPLETCTL_Stop(p,c)                    AEEGETPVTBL(p, IAppletCtl)->Control((p),(c),APPLETCTL_OP_STOP,NULL,NULL)
#define IAPPLETCTL_Control(p,c,o,pb,pn)          AEEGETPVTBL(p, IAppletCtl)->Control((p),(c),(o),(pb),(pn))



//pseudo-methods
static __inline int IAPPLETCTL_BrowseFile(IAppletCtl *p, const char *pszFile, uint32 dwReason)
{
   AppStartInfo as;
   int nSize = sizeof(as);

   as.wFlags = SSA_NORMAL;
   as.pszStart = pszFile;
   as.dwReason = dwReason;

   return(IAPPLETCTL_Start(p,0,&as,&nSize));
}

#define IAPPLETCTL_BrowseURL   IAPPLETCTL_BrowseFile

static __inline AEECLSID IAPPLETCTL_RunningApplet(IAppletCtl *p)
{
   AEECLSID cls = 0;
   int nSize = sizeof(cls);

   IAPPLETCTL_GetRunningInfo(p,0,AEE_APPITEM_CUR_RUNNING_APP,&cls,&nSize);

   return(cls);
}


/*=======================================================================
  DATA STRUCTURE DOCUMENTATION
=========================================================================
AppStartInfo
 
Description:
   This structure contains the information to start a new applet.
   
Definition:
typedef struct
{
   uint16      wFlags;
   const char * pszStart;
   uint32      dwReason;
} AppStartInfo;

Members:
   wFlags:  Behavior flags (SSA_XXXX). Please refer to ISHELL_StartAppletEx() for details
   on the flags that can be used   
    pszStart :  ASCIIZ string of arguments to the application.
   dwReason; start reason.  Can be a RESCTL_REASON_XXX or group/privilege id.
         The start reason may be used to determine the priority between
         applications. Please refer to Resource Control reasons in the IResourceCtl Interface 

=======================================================================

AEEAppItem

Description:
This specifies the ID of the item whose information is needed. This is used
in the function IAPPLETCTL_GetRunningInfo()


Definition:
typedef int AEEAppItem

Members:

The following Items are supported:
===H2>
AEE_APPITEM_STATE
===/H2>
~
   Description:
   This returns a uint32 which contains the current running state of the applet.
   When this ID is passed to IAPPLETCTL_GetRunningInfo(), the following details apply:
===pre>
   int   IAPPLETCTL_GetRunningInfo(IAppletCtl *po, AEECLSID cls, AEEAppletItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_APPITEM_STATE
      pBuff: Buffer capable of holding a uint32.  Stats is one of the following values:
===pre>	  
         APPSTATE_STOPPED         0     // Applet is stopped
         APPSTATE_TOP_VISIBLE     1     // Applet is topmost visible application (foreground)
         APPSTATE_SUSPENDED       2     // Applet is suspended
         APPSTATE_BACKGROUND      3     // Applet is running in the background
         APPSTATE_STARTING        4     // Applet is starting
         APPSTATE_CLOSING         5     // Applet is closing
===/pre>
*
===H2>
AEE_APPITEM_START_TIME
===/H2>
~
   Description:
   This returns a uint32 which contains the start time of the application relative
   to GETUPTIMEMS().
   When this ID is passed to IAPPLETCTL_GetRunningInfo(), the following details apply:
===pre>
   int   IAPPLETCTL_GetRunningInfo(IAppletCtl *po, AEECLSID cls, AEEAppletItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_APPITEM_START_TIME
      pBuff: Buffer capable of holding a uint32
*
===H2>
AEE_APPITEM_MODULE_ID
===/H2>
~
   Description:
   This returns the 32-bit unique identifier associated with the owning module  All applets contained
   in owning module return the same id.
   When this ID is passed to IAPPLETCTL_GetRunningInfo(), the following details apply:
===pre>
   int   IAPPLETCTL_GetRunningInfo(IAppletCtl *po, AEECLSID cls, AEEAppletItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_APPITEM_MODULE_ID
      pBuff: Buffer capable of holding a AEECLSID
*
===H2>
AEE_APPITEM_HEAP_USAGE
===/H2>
~
   Description:
   This returns a 32 bit value specifying the amount of ram allocated to the module associated with
   the application.
   When this ID is passed to IAPPLETCTL_GetRunningInfo(), the following details apply:
===pre>
   int   IAPPLETCTL_GetRunningInfo(IAppletCtl *po, AEECLSID cls, AEEAppletItem nItem, void *pBuff, int *pnSize);
===/pre>

   Parameters:
      nItem: Specifies AEE_APPITEM_HEAP_USAGE
      pBuff: Buffer capable of holding a uint32
*
   Return Value:
      EUNSUPPORTED : Applet doesn't support specified item
      EBADPARM : pnSize == NULL
      EBADCLASS : Class is not an applet or applet not running.
      SUCCESS : Otherwise

Comments:
None

See Also:
IAppletCtl_GetRunningInfo

=========================================================================
  INTERFACE DOCUMENTATION
=========================================================================

IAPPLETCTL Interface

Description:
   The IAPPLETCTL interface is used to control the applications.  This
   includes getting the list of running applications, getting specific information
   on an application, starting an application and stopping an application.

=============================================================================
Function: IAPPLETCTL_AddRef()

Description:
	This function is inherited from IQI_AddRef().

==============================================================================
Function: IAPPLETCTL_Release()

Description:
	This function is inherited from IQI_Release().

==============================================================================
Function: IAPPLETCTL_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().

=======================================================================
IAPPLETCTL_GetRunningList()

Description:
   This method is used to get a list of IDs for the currently running applets.

Prototype:
   int IAPPLETCTL_GetRunningList(IAppletCtl * pIAppletCtl, void * pBuff, int * pnSize)

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    pBuff : On return, contains a list of currently running application ids.
               May be NULL, if querying for size.
    pnSize : [in/out] On input, when pBuff is non-NULL, pnSize specifies the
             size of pBuff in bytes.   On output pnSize is be set to the 
             number of bytes required to hold the currently running applet 
             ids. Note that the number of applets is dynamic and can change 
             between calls.

Return Value:
   SUCCESS: if successful
   EBADPARM: if bad parameters are passed in (pnSize is NULL)
   EBUFFERTOOSMALL: If buffer is not large enough for entire list.  The buffer is filled
                  with as many ids as possible.
                  
Comments:
      None.

Version:
    Introduced BREW Client 3.1

See Also:
    IAPPLETCTL_GetRunningInfo()

=======================================================================
IAPPLETCTL_GetRunningInfo()

Description:
   This method is used to get specific information about one of the currently running applications.
   This function takes an ID that specifies what information is needed. The buffer contains the
   corresponding information on return.

Prototype:
    int IAPPLETCTL_GetRunningInfo
    (
    IAppletCtl *pIAppletCtl,
    AEECLSID cls
    AEEAppItem nItem,
    void *pBuff,
    int *pnSize
    );

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    cls   : class id of a currently running applet.
    nItem : Specifies the Item whose info is needed. Please see documentation of
            AEEAppItem for the supported items.
    pBuff : Contains the corresponding information on return, may be NULL, if querying
            for size of the item.
    pnSize : [in/out] On input, when pBuff is non-NULL, pnSize specifies the
             size of pBuff in bytes.   On output, pnSize will be set to the
             number of bytes required to hold the DeviceInfoEx() item data.

Return Value:
    SUCCESS: if successful
    EBADCLASS: If specified cls is not a currently running applet
    EBADPARM: if bad parameters are passed in (pnSize is NULL)
    EUNSUPPORTED: if this item is not supported

    Other errors from AEEError.h are possible, for example 
	
	ENOMEMORY or EFAILED
     when errors gathering the information occur.

Comments:
    This function may partially fill pBuff in the case where
     pnSize is not large enough to hold all item data.  In this case, pnSize
     will be set to the size required.

Version:
    Introduced BREW Client 3.1

See Also:
    AEEAppItem
    IAPPLETCTL_GetRunningList()
    ISHELL_EnumNextApplet()

=======================================================================
IAPPLETCTL_Start()

Description:
   This method is used to start the specified applet class.

   If the applet being started is the screen saver app (SSA_SSAVER flag is set) the 
   top visible app will be sent EVT_APP_NO_CLOSE(EVT_BUSY) event. If the SSA_SSAVER flag
   of the applet being started is not set the top visible app will NOT be sent 
   EVT_APP_NO_CLOSE(EVT_BUSY).
   
Prototype:
    int IAPPLETCTL_Start
    (
    IAppletCtl *pIAppletCtl,
    AEECLSID cls,
    void *pBuff,
    int *pnSize
    );

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    cls   : class id of the applet to start.
    pBuff : Valid Pointer to a structure of type AppStartInfo.
    pnSize : pnSize specifies the size of pBuff in bytes.

Return Value:
    SUCCESS: if successful
    EBADPARM: if bad parameters are passed in (i.e. pnSize is NULL)
    EBADCLASS: if Shell class supplied invalid
    EEXPIRED: if Applet has expired
    EPRIVLEVEL: caller doesn't have privileges to perform this operation
    EFILENOEXISTS: if The applet has been disabled
    EUNABLETOLOAD: if OEM or current app won't allow loading this applet
    EUNSUPPORTED: if this operation is not supported
    ENOTALLOWED: OEM code won't allow application to start.
    EITEMBUSY: If current top visible application will not release control

    Other errors from AEEError.h are possible, for example 
	
	ENOMEMORY or EFAILED when errors gathering the information occur.

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_StartApplet
   
=======================================================================
IAPPLETCTL_CanStart()

Description:
   This method is used to determine if the specified applet can start, given
   the start information.

   If the applet being started is the screen saver app (SSA_SSAVER flag is set) the 
   top visible app will be sent EVT_APP_NO_CLOSE(EVT_BUSY) event. If the SSA_SSAVER flag
   of the applet being started is not set the top visible app will NOT be sent 
   EVT_APP_NO_CLOSE(EVT_BUSY).
   
Prototype:
    int IAPPLETCTL_CanStart
    (
    IAppletCtl *pIAppletCtl,
    AEECLSID cls,
    void *pBuff,
    int *pnSize
    );

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    cls   : class id of an applet..
    pBuff : Valid Pointer to a structure of type AppStartInfo.
    pnSize : pnSize specifies the size of pBuff in bytes.

Return Value:
    SUCCESS: if successful
    EBADPARM: if bad parameters are passed in (pnSize is NULL or AppStartInfo.dwReason is invalid)
    ECLASSNOTSUPPORT: if Class specified is not supported
    EEXPIRED: if Applet has expired
    EFILENOEXISTS: if The applet has been disabled
    EUNSUPPORTED: if this operation is not supported
    EUNABLETOLOAD: if OEM or current app won't allow loading this applet
    EITEMBUSY: current application won't allow app to start.

    Other errors from AEEError.h are possible, for example 
	
	ENOMEMORY or EFAILED when errors gathering the information occur.

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_StartApplet
   
=======================================================================
IAPPLETCTL_Stop()

Description:
   This method is used to stop a currently running applet.  It requries PL_SYSTEM unless
   the cls == caller's class. The app will receive an EVT_APP_STOP regardless of the number of entries this app 
   has on the history list. None of the history entries for the app will be removed. The only exception to this rule
   is if the app is currently the top-visible entry. In that case, the top-visible history entry will be removed.
   If the app being closed is currently top-visible, the next app in the history list will be resumed/started.


Prototype:
    int IAPPLETCTL_Stop
    (
    IAppletCtl *pIAppletCtl,
    AEECLSID cls
    );

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    cls   : class id of a currently running applet.

Return Value:
    SUCCESS: if successful
    EBADCLASS: If specified cls is not a currently running applet
    EPRIVLEVEL: caller doesn't have privileges to perform this operation

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_StartApplet
   ISHELL_StopApplet
   
=======================================================================
IAPPLETCTL_Control()

Description:
   This method is used to control applets.

Prototype:
    int IAPPLETCTL_Control
    (
    IAppletCtl *pIAppletCtl,
    AEECLSID cls,
    uint32 op,
    void *pBuff,
    int *pnSize
    );

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    cls   : class id of an applet.
    op : Specifies the operation to perform on the specified applet class.
    pBuff : Points to the corresponding information on return. May be NULL, if querying
            for size of the item.
    pnSize : pnSize specifies the size of pBuff in bytes.

Return Value:
    SUCCESS: if successful
    EBADCLASS: If specified cls is not a currently running or loaded applet
    EBADPARM: if bad parameters are passed in (pnSize is NULL)
    EPRIVLEVEL: caller doesn't have privileges to perform this operation
    EUNSUPPORTED: if this operation is not supported
    EEXPIRED: if Applet has expired
    EFILENOEXISTS: if The applet has been disabled
    EUNABLETOLOAD: if OEM or current app won't allow loading this applet
    EUNSUPPORTED: if this operation is not supported
    ENOTALLOWED: OEM code won't allow application to start.
    EITEMBUSY: If current top visible application will not release control

    Other errors from AEEError.h are possible, for example 
	
	ENOMEMORY or EFAILED when errors gathering the information occur.

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_StartApplet()
   IAPPLETCTL_Start()
   IAPPLETCTL_Stop()
   
============================================================================= 
IAPPLETCTL_BrowseFile()

Description:
   This method can be used to to start the applet associated with the
   file type specified (by extension).  The applet will be started and will
   receive the EVT_BROWSE_FILE event.  Startup behavior is identical to the
   IAPPLETCTL_Start() function.
   
   Example: If there is an app in the system that is registered for handling .txt files, 
   then the following can be used to launch the app that handles .txt files:
   IAPPLETCTL_BrowseFile(po, "foo.txt", 9000); 
 

Prototype:
int IAPPLETCTL_BrowseFile(IAppletCtl *p, const char *pszFile, uint32 dwReason)

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    pszFile: File name
    dwReason: start reason for the app

Return Value:
    SUCCESS: if successful
    EEXPIRED: if Applet has expired
    EPRIVLEVEL: caller doesn't have privileges to perform this operation
    EFILENOEXISTS: if The applet has been disabled
    EUNABLETOLOAD: if OEM or current app won't allow loading this applet
    EUNSUPPORTED: if this operation is not supported
    ENOTALLOWED: OEM code won't allow application to start.
    EITEMBUSY: If current top visible application will not release control

    Other errors from AEEError.h are possible, for example ENOMEMORY or EFAILED
     when errors gathering the information occur.

    EBADCLASS: NULL or empty pszFile
    EBADPARM: pszfile does not contain a file extension
    ENOMEMORY: System out of memory
    ESCHEMENOTSUPPORTED: no handler for this file extension

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_StartApplet()
   ISHELL_BrowseFile()
   ISHELL_BrowseURL()
   IAPPLETCTL_BrowseURL()
   IAPPLETCTL_Start()
   IAPPLETCTL_Stop()
   
============================================================================= 
IAPPLETCTL_BrowseURL()

Description:
   This method can be used to start the applet associated with the
   URL scheme specified.  The applet will be started and will receive the
   EVT_BROWSE_URL event.  Startup behavior is identical to the
   IAPPLETCTL_Start() function.

  
   Example: If there is an app in the system that is registered for handling myscheme: URLs, 
   then the following can be used to launch the app that handles myscheme: URLs:
   IAPPLETCTL_BrowseURL(po, "myscheme:foo", 9000); 
 

Prototype:
int IAPPLETCTL_BrowseURL(IAppletCtl *p, const char *pszURL, uint32 dwReason)

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object
    pszURL: Pointer to the URL to be used for starting the app
    dwReason: start reason for the app

Return Value:
    SUCCESS: if successful
    EEXPIRED: if Applet has expired
    EPRIVLEVEL: caller doesn't have privileges to perform this operation
    EFILENOEXISTS: if The applet has been disabled
    EUNABLETOLOAD: if OEM or current app won't allow loading this applet
    EUNSUPPORTED: if this operation is not supported
    ENOTALLOWED: OEM code won't allow application to start.
    EITEMBUSY: If current top visible application will not release control

    Other errors from AEEError.h are possible, for example ENOMEMORY or EFAILED
     when errors gathering the information occur.

    EBADCLASS: NULL or empty pszURL
    ENOMEMORY: System out of memory
    ESCHEMENOTSUPPORTED: no handler for this URL scheme

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_StartApplet()
   ISHELL_BrowseFile()
   ISHELL_BrowseURL()
   IAPPLETCTL_BrowseFile()
   IAPPLETCTL_Start()
   IAPPLETCTL_Stop()
   
============================================================================= 
IAPPLETCTL_RunningApplet()

Description:
   This method returns the classID of the currently running applet. This may or
   may not be the same as the top-visible applet. For ex: if this API is invoked
   when a bkgnd application is currently running, this API returns the classID
   of the bkgnd app and not of the top-visible app. 
   The funcion ISHELL_ActiveApplet() can be used to get the ClassID of the
   current top-visible applet
  

Prototype:
AEECLSID IAPPLETCTL_RunningApplet(IAppletCtl *p)

Parameters:
    pIAppletCtl : Pointer to the IAppletCtl Interface object

Return Value:
    classID of the currently running applet. If it returns AEECLSID_SHELL, it indicates
    that the system-context is currently active.

Comments:
   None.
               
Version:
    Introduced BREW Client 3.1

See Also:
   ISHELL_ActiveApplet()
   
============================================================================= */
#endif // AEEAPPLETCTL_H
