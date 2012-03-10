/*======================================================
FILE:  AEE_OEM.h

SERVICES:  AEE OEM Interface header

GENERAL DESCRIPTION:

This file contains function prototypes for AEE routines that
may be required by the OEM.

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(AEE_OEM_H)
#define AEE_OEM_H  1

#include "AEE_OEMComdef.h"
#include "AEE_OEMDispatch.h"
#include "AEESysClock.h"
#include "AEEShell.h"

#if defined(__cplusplus)
extern "C" {
#endif

// AEE Helper Routines

extern AEE_EXPORTS IShell    *AEE_GetShell(void);

//
// These routines are provided for the OEM to use the UI controls mechanism...
//

extern AEE_EXPORTS int                 AEE_CreateControl(OEMCONTEXT pDispContext, AEECLSID iid, void ** po);
extern AEE_EXPORTS boolean             AEE_IsInitialized(void);
extern AEE_EXPORTS void                AEE_RegisterForValidTime(PFNNOTIFY pfn, void * pData, boolean bActive);
extern AEE_EXPORTS void                AEE_RegisterForDataService(PFNNOTIFY pfn, void * pData, boolean bActive);
extern AEE_EXPORTS int                 AEE_CreateInstance(AEECLSID cls, void ** ppobj);
extern AEE_EXPORTS int                 AEE_CreateInstanceSys(AEECLSID cls, void ** ppobj);
extern AEE_EXPORTS ACONTEXT           *AEE_EnterAppContext(ACONTEXT *pac);
extern AEE_EXPORTS void                AEE_LeaveAppContext(ACONTEXT *pacRestore);
extern AEE_EXPORTS ACONTEXT           *AEE_GetAppContext(void);
extern AEE_EXPORTS ACONTEXT           *AEE_GetLastAppContext(void);
extern AEE_EXPORTS IDialog *           AEE_GetAppContextDlg(PACONTEXT pac);
extern AEE_EXPORTS void                AEE_SetAppContextDlg(PACONTEXT pac, IDialog * pd);
extern AEE_EXPORTS MCONTEXT           *AEE_GetAppModuleContext(PACONTEXT pac);
extern AEE_EXPORTS AEECLSID            AEE_GetAppContextCls(PACONTEXT pac)   ;
extern AEE_EXPORTS IBase *             AEE_NewClass(VTBL(IBase) * pvt, int32 nSize);
extern AEE_EXPORTS IBase *             AEE_NewClassEx(VTBL(IBase) * pvt, int32 nSize, boolean bSystem);
extern AEE_EXPORTS int                 AEE_GetResData(const char * pszResFile, uint16 nResID, ResType nType, void ** pData, uint32 * pdwFilled);
extern AEE_EXPORTS uint16              AEE_GetAppCloseKey(void);
extern AEE_EXPORTS IApplet *           AEE_GetAppContextApplet(PACONTEXT pac);
extern AEE_EXPORTS const char *        AEE_GetModuleContextMIF(PMCONTEXT pac);
extern AEE_EXPORTS ACONTEXT           *AEE_GetStackedAppContext(void) ;
extern AEE_EXPORTS const char *        AEE_GetSID(int * pnSize);
extern AEE_EXPORTS void                AEE_EnableDisplay(AEECLSID cls, boolean bEnabled);
extern AEE_EXPORTS void                AEE_GetDeviceInfo(AEEDeviceInfo *pi);
extern AEE_EXPORTS boolean             AEE_IsBREWDirectedSMS(char *pMsg);
extern AEE_EXPORTS void                AEE_InitAlarms(void * pUnused);
extern AEE_EXPORTS byte               *AEE_GetDecodedTextString( byte * pText, uint32 nText, int se, int * ae, uint32 * pnBytes);
extern AEE_EXPORTS boolean             AEE_IsDebugBREWSMS(void);

#if defined(__cplusplus)
}
#endif

/*=======================================================================
  AEE  DOCUMENTATION
============================================================================
AEE_OEM Interface
This interface contains functions for AEE routines to use the UI controls mechanism.


The function perform the following tasks:
-allows the OEM layer to create user interface controls for use by the existing user interface (UI), if one is provided.

-resolves the input path and returns the fully qualified path based upon the inputs.

-allows the OEM layer to take advantage of the BREW notification capability to monitor the system for valid time or data service.

-set permissions

-restore context

-detect test devices

-determine if Brew is initialized

-enable or disable application access to a display.
============================================================================

Function:  AEE_GetShell()

Description:
   This function returns the IShell interface pointer to the active AEE
shell.  This pointer can then be used by the OEM layer to invoke any other ISHELL
interface function.

Prototype:

   IShell * AEE_GetShell(void)

Parameters:
   None

Return Value:
   Pointer to the shell interface

Comments:
   None

Side Effects:
   None

See Also:
   None


=======================================================================
Function:  AEE_ResolveSystemPath()

Description:
   This function resolves the input path and returns the fully qualified
path based upon the inputs.  Note the following:

- The returned memory is allocated on behalf of the system and should be
freed using sys_free.

- The input SFP_XXX value mask will be checked against shared rights
on the file.

Prototype:

   int AEE_ResolveSystemPath(const char * pszSub,uint32 dwSharedRights,char ** ppszDest, uint16 * pwDirType);

Parameters:
   pszSub: input path substring
   ppszDest: pointer to the pointer to store the allocated result
   dwSharedRights: SFP_XXX flags above
   pwDirType: Flag that notes whether the path is in the AEE shared directory

Return Value:
   Non-Zero - error
   Zero - success

Comments:
   None

Side Effects:
   None

See Also:
   None

============================================================================

Function:  AEE_CreateControl()

Description:
   This function allows the OEM layer to create user interface controls
   for use by the existing User Interface (UI), if one is provided.

Prototype:
   int  AEE_CreateControl(OEMCONTEXT pDispContext, AEECLSID iid, void ** po);

Parameters:

   pDispContext:   pointer to OEMCONTEXT for the display
   iid: class of the control
   po: destination pointer to be filled

Return Value:
   0 (zero): if successful
   See ISHELL_CreateInstance() in the BREW API Reference.

Comments:
   None

Side Effects:
   None

See Also:
   ISHELL_CreateInstance()

=======================================================================
Function:  AEE_RegisterForValidTime()

Description:
   This function allows the OEM layer to take advantage of the BREW notification
capability to monitor the system for valid time.

Prototype:
   void AEE_RegisterForValidTime(PFNNOTIFY pfn, void * pData, boolean bActive);

Parameters:
   pfn:     function to call when the valid time is determined
   pData:   callback argument for the valid time
   bActive: Callback is to be called only when the time is valid and the first BREW
   application has started.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function:  AEE_RegisterForDataService()

Description:
   This function allows the OEM layer to take advantage of the BREW notification
capability to monitor the system for data service.

Prototype:
   void AEE_RegisterForDataService(PFNNOTIFY pfn, void * pData, boolean bActive);

Parameters:
   pfn:     function to call when the data service is determined
   pData:   callback argument for the data service
   bActive: Callback is to be called only when there is data service and the first BREW
   application has started

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function:  AEE_EnterAppContext()

Description:
   This function is used to set the application context, asserting
    permissions and marking the application context as running.

Prototype:
   ACONTEXT *AEE_EnterAppContext(ACONTEXT * pc);

Parameters:
   pc: pointer to the new application context

Return Value:
   The previous application context to later be passed to
      AEE_LeaveAppContext()

Comments:
   You must use AEE_LeaveAppContext() to undo and restore the
    previous ACONTEXT.

Side Effects:
   None

See Also:
   AEE_GetAppContext(), AEE_LeaveAppContext()

=======================================================================
Function:  AEE_LeaveAppContext()

Description:
   This function is used to de-assert an application context and
    to restore the *previously* asserted ACONTEXT.

Prototype:
   void AEE_LeaveAppContext(ACONTEXT *pacRestore)

Parameters:
   pacRestore: the application context to return to

Return Value:
   None

Comments:
   pacRestore should be the return value of a matching call to
    AEE_EnterAppContext().

Side Effects:
   None

See Also:
   AEE_GetAppContext(), AEE_EnterAppContext()

=======================================================================
Function:  AEE_GetAppContext()

Description:
   This function is used to get the current application context.

Prototype:
   void * AEE_GetAppContext(void);

Parameters:
   None

Return Value:
   Pointer to the current application context

Comments:
   None

Side Effects:
   None

See Also:
   AEE_GetLastAppContext(),AEE_EnterAppContext(), AEE_LeaveAppContext()

=======================================================================
Function:  AEE_GetLastAppContext()

Description:
   This function is used to get the most recently running, non-system app context (if any).
   For ex: While App A is executing,  if it switches temporariy to system context (for
   privilege purposes), this API can be used to determine the class ID of the app (A) that is
   running while A has switched to system context. 
   
   When A has switched to system context, calling the function AEE_GetAppContext()
   will not yield the app context of A since that function returns the current app-context. This
   function AEE_GetLastAppContext() has to be used for that purpose.
   
   After obtaining the AppContext, the function AEE_GetAppContextCls() can then be used to 
   obtain the actual ClassID of the app is running. If that returns AEECLSID_SHELL, it indicates 
   that the system context is currently running and there is no non-system app 
   executing currently.

Prototype:
   void * AEE_GetLastAppContext(void);

Parameters:
   None

Return Value:
   Pointer to the current application context. 

Comments:
   None

Side Effects:
   None

See Also:
   AEE_GetAppContext(),AEE_GetAppContextCls

=======================================================================
Function: AEE_IsInitialized()

Description:
   This function checks whether BREW has been initialized or not.

Prototype:
   boolean AEE_IsInitialized(void);

Parameters:
   None

Return Value:
   TRUE if initialized, FALSE otherwise

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_EnableDisplay()

Description:
   This function is called by the OEM to enable or disable application access
   to a display.  This needs to be called before the OEM layer draws to a
   display to prevent anything else from writing to the display at the
   same time.  It also needs to be called again, when the OEM no longer needs
   the display, to return control to BREW.

Prototype:
   void AEE_EnableDisplay(AEECLSID cls, boolean bEnabled);

Parameters:
   cls:           Class ID for the display's device bitmap.  This is one of
                  AEECLSID_DEVBITMAP1 through AEECLSID_DEVBITMAP4.
   bEnabled:      TRUE to allow BREW to draw to the display,
                  FALSE to prevent BREW from drawing to the display.

Return Value:
   None

Comments:
   This function currently has no effect on the primary display.
   AEE_Suspend() and AEE_Resume() should be used instead.

Side Effects:
   None

See Also:
   AEE_Suspend(), AEE_Resume()

=======================================================================
Function: AEE_CreateInstance()

Description:
   This function creates a class of the given class ID.

Prototype:
   int AEE_CreateInstance(AEECLSID cls, void ** ppobj);

Parameters:
   cls:           Class ID to be created.
   ppobj:         Object to be filled on success.

Return Value:
   See ISHELL_CreateInstance()

Comments:
   This function should not be called when AEE is not initialised.

Side Effects:
   None

See Also:
   ISHELL_CreateInstance()

=======================================================================
Function: AEE_CreateInstanceSys()

Description:
   This function creates a class of the given class ID and is created
   in the system context using the system privileges and system tagged
   memory nodes.

Prototype:
   int AEE_CreateInstanceSys(AEECLSID cls, void ** ppobj);

Parameters:
   cls:           Class ID to be created.
   ppobj:         Object to be filled on success.

Return Value:
   See ISHELL_CreateInstance()

Comments:
   This function should not be called when AEE is not initialised.

Side Effects:
   None

See Also:
   ISHELL_CreateInstance()

=======================================================================
Function: AEE_GetAppContextDlg()

Description:
   This function gets the IDialog pointer of the active dialog for the 
   app context provided.

Prototype:
   IDialog * AEE_GetAppContextDlg(PACONTEXT pac);

Parameters:
   pac:           Application context.

Return Value:
   IDialog Pointer   - If there is active dialog for this application
   NULL              - No active dialog for this application

Comments:
   None

Side Effects:
   None

See Also:
   ISHELL_GetActiveDialog()

=======================================================================
Function: AEE_SetAppContextDlg()

Description:
   This function sets the IDialog pointer of the active dialog for the 
   app context provided.

Prototype:
   void AEE_SetAppContextDlg(PACONTEXT pac, IDialog * pd);

Parameters:
   pac:           Application context.
   pd:            IDialog to set to applet

Return Value:
   None

Comments:
   None

Side Effects:
   Danger, Danger! This will replace the dialog stack in the application.
   You will probably wish to store the current active dialog to replace later.

See Also:
   AEE_GetAppContextDlg(), ISHELL_GetActiveDialog()

=======================================================================
Function: AEE_GetAppModuleContext()

Description:
   This function retrieves the MCONTEXT for the app context provided. This
   is the module context that owns the applet context provided. It can be 
   used for AEE_ functions that need an PMCONTEXT.

Prototype:
   MCONTEXT * AEE_GetAppModuleContext(PACONTEXT pac);

Parameters:
   pac:           Application context.

Return Value:
   PMCONTEXT that can be used with other AEE_ functions.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_GetAppContextCls()

Description:
   This function retrieves the AEECLSID for the app context supplied.

Prototype:
   AEECLSID AEE_GetAppContextCls(PACONTEXT pac);

Parameters:
   pac:           Application context.

Return Value:
   AEECLSID of the app context supplied.

Comments:
   AEECLSID_SHELL will be returned for the system context.

Side Effects:
   None

See Also:
   AEE_GetLastAppContext()

=======================================================================
Function: AEE_NewClass()

Description:
   This function helps set up a new class and allocate space for it.

Prototype:
   IBase * AEE_NewClass(VTBL(IBase) * pvt, int32 nSize);

Parameters:
   pvt:           Pointer to the VTable of the new class.
   nSize:         Size to allocate for the class.

Return Value:
   non-0:         Allocated class of nSize with VTable pointer set up.
   0:             Class creating failed.

Comments:
   This will create a class with memory tagged with whichever the current
   operating mode is set at currently. If you wish to specify the memory
   to be tagged, you should use AEE_NewClassEx().

Side Effects:
   None

See Also:
   AEE_NewClassEx()

=======================================================================
Function: AEE_NewClassEx()

Description:
   This function helps set up a new class and allocate space for it as well as
   specify which context the memory is tagged with.

Prototype:
   IBase * AEE_NewClassEx(VTBL(IBase) * pvt, int32 nSize, boolean bSystem);

Parameters:
   pvt:           Pointer to the VTable of the new class.
   nSize:         Size to allocate for the class.
   bSystem:       Context to tag the memory; TRUE: System, FALSE: Executing Module

Return Value:
   non-0:         Allocated class of nSize with VTable pointer set up.
   0:             Class creating failed.

Comments:
   None

Side Effects:
   None

See Also:
   AEE_NewClass()

=======================================================================
Function: AEE_GetResData()

Description:
   This function obtains resource data from the supplied file.

Prototype:
   int AEE_GetResData(const char * pszResFile, uint16 nResID, ResType nType, 
                        void ** pData, uint32 * pdwFilled);

Parameters:
   pszResFile:    Pointer to the Resource File Name
   nResID:        ID of resource to obtain
   nType:         The type of resource:~
~
            RESTYPE_IMAGE, ~
            RESTYPE_DIALOG,~
            RESTYPE_CONTROL, ~
            RESTYPE_ LISTITEM, (currently not supported)~
            RESTYPE_BINARY, ~
*
    If nType is RESTYPE_IMAGE, the first byte indicates the offset where the
    actual image data begins, and the second byte is 0 (zero). Starting from
    the third byte, the string indicates the MIME type followed by the actual
    image data.
   pData:         Data to be filled on successful retrieval
   pdwFilled:     Size of the data filled in pData on successful retrieval

Return Value:
   AEE_SUCCESS:         Successful retrieval of resource data
   FILE_ERROR:          General File Error
   EBADFILENAME:        Bad file name supplied
   EFILENOEXISTS:       File not found
   EBADSEEKPOS:         Invalid seek operation
   ENOMEMORY:           Not enough memory to complete the operation
   EBADPARM:            Invalid parameter
   EFAILED:             Failed operation
   EPRIVLEVEL:          Invalid privilege level for the operation
   ERESOURCENOTFOUND:   Resource not found


Comments:
   None

Side Effects:
   None

See Also:
   ISHELL_LoadResData()

=======================================================================
Function: AEE_GetAppCloseKey()

Description:
   This function gets the AVKType value for the App Close operation..

Prototype:
   uint16 AEE_GetAppCloseKey(void)

Parameters:
   None

Return Value:
   Key code that BREW uses to close an applet.

Comments:
   None

Side Effects:
   None

See Also:
   OEM_GetConfig() - CFGI_CLOSE_KEYS

=======================================================================
Function: AEE_GetAppContextApplet()

Description:
   This function gets the IApplet interface for the supplied app context.

Prototype:
   IApplet * AEE_GetAppContextApplet(PACONTEXT pac);

Parameters:
   pac:           Application context.

Return Value:
   IApplet Interface Pointer for the app context.

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_GetModuleContextMIF()

Description:
   This function gets the name of the MIF for the supplied MCONTEXT.

Prototype:
   const char * AEE_GetModuleContextMIF(PMCONTEXT pac);

Parameters:
   pac:           Module context.

Return Value:
   String containing the name of the MIF file for the Module Context supplied.
   If the system context is supplied, the string will be "sys.mif"

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_GetStackedAppContext()

Description:
   This function gets the App Context stack.

Prototype:
   ACONTEXT * AEE_GetStackedAppContext(void) ;

Parameters:
   None

Return Value:
   App Context at the head of the list.

Comments:
   No assumptions about what pac is on the head of the list should be made.

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_GetSID()

Description:
   This function gets the Subscriber ID.

Prototype:
   const char * AEE_GetSID(int * pnSize);

Parameters:
   pnSize:        [OUT] Filled SID size

Return Value:
   Non-0:         String Filled with the Subscriber ID
   NULL:          Failed to get SID

Comments:
   This memory is maintained by BREW and must not be FREE().

Side Effects:
   None

See Also:
   ISHELL_GetDeviceInfoEx() - AEE_DEVICEITEM_SID

=======================================================================
Function: AEE_GetDeviceInfo()

Description:
   This function gets the device info.

Prototype:
   void AEE_GetDeviceInfo(AEEDeviceInfo *pi);

Parameters:
   pi:            [OUT] Filled Device Info

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   ISHELL_GetDeviceInfo(), OEM_GetDeviceInfo()

=======================================================================
Function: AEE_IsBREWDirectedSMS()

Description:
   This function checks a text string to see if it is a BREW directed SMS
   message.

Prototype:
   boolean AEE_IsBREWDirectedSMS(char *pMsg);

Parameters:
   pMsg:          SMS Message Text

Return Value:
   TRUE:          BREW directed SMS text string
   FALSE:         Not a BREW directed SMS text string

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_GetDecodedTextString()

Description:
   This function gets the decoded text string given a format.

Prototype:
   byte * AEE_GetDecodedTextString( byte * pText, uint32 nText, int se, int * ae, uint32 * pnBytes);

Parameters:
   pText:         Message to decode
   nText:         Size of message in bytes in pText
   se:            AEESMSEncType of pText
   pae:           [OUT] Optional AEE_ENC_XXX type for the message
   pnBytes:       [OUT] Optional size of returned buffer in bytes

Return Value:
   Decoded string. Caller is responsible to free this memory with FREE().

Comments:
   None

Side Effects:
   None

See Also:
   None

=======================================================================
Function: AEE_IsDebugBREWSMS()

Description:
   This function checks whether the device is in SMS debug mode.

Prototype:
   boolean AEE_IsDebugBREWSMS(void);

Parameters:
   None

Return Value:
   TRUE:          In debug SMS mode
   FALSE:         Not in debug SMS mode

Comments:
   You can toggle this by using the debug key sequence and it is option 11.

Side Effects:
   None

See Also:
   OEM_GetConfig() - CFGI_DEBUG_KEY

==================================================================*/

#endif // AEE_OEM_H

