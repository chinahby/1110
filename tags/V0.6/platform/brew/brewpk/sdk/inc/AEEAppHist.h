#ifndef AEEAPPHIST_H
#define AEEAPPHIST_H
/*======================================================
FILE:  AEEAppHist.h

SERVICES:  Application History Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEEAppHist

Copyright © 1999,2004-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEE.h"

#define APPHIST_MAX_DATASIZE     (1024)
#define APPHIST_MAX_DATAELEMENTS (5) 

#define INHERIT_IAppHistory(iname)\
   INHERIT_IQueryInterface(iname);\
   int  (*Forward)(iname * po);\
   int  (*Back)(iname * po);\
   int  (*Top)(iname *po);\
   int  (*Insert)(iname * po, AEECLSID cls,const char * pszArgs);\
   int  (*GetClass)(iname * po, AEECLSID *pcls);\
   int  (*GetArgs)(iname * po, char * pBuff, int * pnSize);\
   int  (*GetData)(iname * po, const char * pszName, void * pBuffer, int *pnSize);\
   int  (*SetData)(iname * po, const char * pszName, void * pBuffer, int nSize);\
   int  (*Current)(iname * po, int *pnIdx);\
   int  (*Remove)(iname * po);\
   int  (*Move)  (iname * po, int nIdx);\
   int  (*GetReason)(iname *po, uint32 *pdw);\
   int  (*SetReason)(iname *po, uint32 dw)

AEEINTERFACE_DEFINE(IAppHistory);

#define IAPPHISTORY_AddRef(p)              AEEGETPVTBL((p),IAppHistory)->AddRef(p)
#define IAPPHISTORY_Release(p)             AEEGETPVTBL((p),IAppHistory)->Release(p)
#define IAPPHISTORY_QueryInterface(p,i,o)  AEEGETPVTBL((p),IAppHistory)->QueryInterface((p),(i),(o))
#define IAPPHISTORY_Forward(p)             AEEGETPVTBL((p),IAppHistory)->Forward((p))
#define IAPPHISTORY_Back(p)                AEEGETPVTBL((p),IAppHistory)->Back((p))
#define IAPPHISTORY_Top(p)                 AEEGETPVTBL((p),IAppHistory)->Top((p))
#define IAPPHISTORY_Insert(p,cls,psz)      AEEGETPVTBL((p),IAppHistory)->Insert((p),(cls),(psz))
#define IAPPHISTORY_GetClass(p, pcls)      AEEGETPVTBL((p),IAppHistory)->GetClass((p),(pcls))
#define IAPPHISTORY_GetArgs(p,ps,pn)       AEEGETPVTBL((p),IAppHistory)->GetArgs((p),(ps),(pn))
#define IAPPHISTORY_GetResumeData(p,pb,pn) AEEGETPVTBL((p),IAppHistory)->GetData((p),NULL,(pb),(pn))
#define IAPPHISTORY_SetResumeData(p,pb,n)  AEEGETPVTBL((p),IAppHistory)->SetData((p),NULL,(pb),(n))
#define IAPPHISTORY_GetData(p,psz,pb,pn)   AEEGETPVTBL((p),IAppHistory)->GetData((p),(psz),(pb),(pn))
#define IAPPHISTORY_SetData(p,psz,pb,n)    AEEGETPVTBL((p),IAppHistory)->SetData((p),(psz),(pb),(n))
#define IAPPHISTORY_Current(p,pn)          AEEGETPVTBL((p),IAppHistory)->Current((p),(pn))
#define IAPPHISTORY_Remove(p)              AEEGETPVTBL((p),IAppHistory)->Remove((p))
#define IAPPHISTORY_Move(p,n)              AEEGETPVTBL((p),IAppHistory)->Move((p),(n))
#define IAPPHISTORY_GetReason(p,pdw)       AEEGETPVTBL((p),IAppHistory)->GetReason((p),(pdw))
#define IAPPHISTORY_SetReason(p,dw)        AEEGETPVTBL((p),IAppHistory)->SetReason((p),(dw))

static __inline int IAPPHISTORY_Bottom(IAppHistory *pme)
{
   int nErr = SUCCESS;
   
   while (SUCCESS == (nErr = IAPPHISTORY_Back(pme)));
   if (ENOSUCH == nErr) {
      return SUCCESS;
   }
   return nErr; 
}

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IAppHistory Interface

   The functions in the IAppHistory interface allow the caller to interact
   with the application history list.  This list consists of the visible
   and suspended applications.  The APIs allow the caller to:

   - Navigate through entries in the history list
   - Obtain the class ID of a history entry
   - Set or retrieve data stored by the system when the application is suspended
   - Close the application history entry 
   - Insert a new entry into the history

   This interface is obtained by calling ISHELL_CreateInstance.  A new 
   instance of IAppHistory is initialized pointing to the top-most history
   entry (that of the currently top-visible application), unless the 
   history list is empty.

   Rules on creating history entry for an app: 
   Every application that is either top-visible or is suspended has an associated history entry. When there is 
   an attempt to start an app and make it top-visible (i.e. when ISHELL_StartApplet() or one of the macros based on
   this function) is invoked, BREW will associate an apphistory entry with this app. If the app does not already have 
   a history entry with it (i.e if it is not already in the list of suspended apps), a new history entry will
   be created for this app. If this app is already in the history list, the following rules will be used to decide whether to
   create a new entry for this app OR to move the already existing history entry to the top of the list:
   a. If the app has set resume data previously (when it got suspended) by invoking IAPPHISTORY_SetResumeData(), then
   a new history entry will be created for the app when ISHELL_StartApplet() is invoked on it again
   b. If the app has not set resume data previously by calling IAPPHISTORY_SetResumeData(), then a new history entry will
   not be created for this app when it is started again. Instead, the already existing history entry will be moved
   to the top of the list (to make this app top-visible).

   Rules for removing a history entry:
   Other than using IAPPHISTORY APIs to move a history entry, when ISHELL_CloseApplet() is called, it will 
   remove the history entry (if one exists) for the caller.
   Calling IAPPLETCTL_Stop() will not remove a history entry unless it is the top-visible app.

==============================================================================
Function: IAPPHISTORY_AddRef()

Description:
	This function is inherited from IQI_AddRef().

See Also:
   IAPPHISTORY_Release()
==============================================================================
Function: IAPPHISTORY_Release()

Description:
	This function is inherited from IQI_Release().

See Also:
   IAPPHISTORY_AddRef()

==============================================================================
Function: IAPPHISTORY_QueryInterface()

Description:
	This function is inherited from IQI_QueryInterface().

=============================================================================

IAPPHISTORY_Forward()

Description:
    This function moves the IAppHistory to the next history entry in 
    the history list, i.e. next closest to top-visible.

Prototype:
    int IAPPHISTORY_Forward(IAppHistory * po)

Parameters:
    po  :  Pointer to the IAppHistory Interface object.

Return Value:
    SUCCESS - no errors, the IAppHistory points to the next entry

    ENOSUCH - No next history entry exists.

    EFAILED - this IAppHistory does not point to a history entry, perhaps
               the history list is empty
    
    EINVALIDITEM - Current item was removed by another user of IAppHistory.
    Use IAPPHISTORY_Top() to reset to the top most entry.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_Back()

=============================================================================

IAPPHISTORY_Back()

Description:
    This function moves the IAppHistory to the previous history entry in 
    the history list, i.e. next farthest from top-visible.

Prototype:
    int IAPPHISTORY_Back(IAppHistory * po)

Parameters:
    po  :  Pointer to the IAppHistory Interface object.

Return Value:
    SUCCESS - no errors, the IAppHistory points to the previous entry

    ENOSUCH - No previous history entry exists.

    EFAILED - this IAppHistory does not point to a history entry, perhaps
               the history list is empty
    
    EINVALIDITEM - Current item was removed by another user of IAppHistory.
    Use IAPPHISTORY_Top() to reset to the top most entry.

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_Forward()

=============================================================================

IAPPHISTORY_Top()

Description:
    This function resets the IAppHistory to point to the top-visible entry.

Prototype:
    int IAPPHISTORY_Top(IAppHistory * po)

Parameters:
    po  :  Pointer to the IAppHistory Interface object.

Return Value:
    SUCCESS - no errors, the IAppHistory points to the top entry

    EFAILED - No history entries exists.


Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_Forward(), IAPPHISTORY_Back(), IAPPHISTORY_Bottom()

=============================================================================

IAPPHISTORY_Bottom()

Description:
    This function resets the IAppHistory to point to the entry farthest from 
    the top-visible entry.

Prototype:
    int IAPPHISTORY_Bottom(IAppHistory * po)

Parameters:
    po  :  Pointer to the IAppHistory Interface object.

Return Value:
    SUCCESS - no errors, the IAppHistory points to the bottm entry

    EFAILED - No history entries exists.


Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_Forward(), IAPPHISTORY_Back(), IAPPHISTORY_Top()
=============================================================================

IAPPHISTORY_Insert()

Description:
    This function inserts a new history entry immediately "Back" of the 
     current entry. It will send notification about the change in AppHistory.

Prototype:
    int  IAPPHISTORY_Insert(IAppHistory * po, AEECLSID cls,const char * pszArgs)

Parameters:
    po  :  Pointer to the IAppHistory Interface object.
    cls :  AEECLSID of the requested application
    pszArgs: Pointer to startup arguments

Return Value:
    SUCCESS - No error

    EFAILED - Unable to insert new entry.  Current entry unlinked from history list.

    EPRIVLEVEL - if the caller has insufficient privilege

Comments: 
   To insert an entry, the caller must either be an instance of cls or
     have PL_SYSTEM.

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    ISHELL_StartApplet()

=============================================================================

IAPPHISTORY_GetClass()

Description:
    This function retrieves the AEECLSID of the app history entry the 
    IAppHistory is pointed to.

Prototype:
    int IAPPHISTORY_GetClass(IAppHistory * po, AEECLSID *pcls)

Parameters:
    po    :  Pointer to the IAppHistory Interface object.
    pcls  :  pointer to an AEECLSID to be filled with the current entry's
              class id

Return Value:
    SUCCESS - No errors

    EFAILED - if no history entries exist

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None
=============================================================================

IAPPHISTORY_GetArgs()

Description:
    This function retrieves the startup argument string for the history
    entry.

Prototype:
    int IAPPHISTORY_GetArgs(IAppHistory * po, char * pszDest, int * pnSize)

Parameters:
   po          :  Pointer to the IAppHistory Interface object.
   pszDest     :  Buffer to fill
   pnSize      :  on input, pointer to integer of size in bytes of the buffer, 
                  on output, size of pBuffer necessary to hold the arguments

Return Value:
   SUCCESS - No error

   EFAILED - if no history entries exist

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   None
=============================================================================

IAPPHISTORY_SetData()

Description:
    This function allows the caller to store history context sensitive data
associated with the input name.  The data can be retrieved using IAPPHISTORY_GetData.
It will notify registered listeners if associated data has been changed.
Note the following:

   - The maximum size of resume data plus the size of the name is APPHIST_MAX_DATASIZE
   - The maximum number of named data elements is APPHIST_MAX_DATAELEMENTS
   - Calling SetData with a zero data size will free data previously stored
   - Data will automatically be freed when the history entry is Removed()

Prototype:
    int  IAPPHISTORY_SetData(IAppHistory * po, const char * pszName, void * pBuffer, int nSize)

Parameters:
   po          :  Pointer to the IAppHistory Interface object.
   pszName     :  Name associated with the data.
   pBuffer     :  Buffer to copy.  ignored if nSize is 0
   nSize:      :  Size in bytes of the data.  Not to exceed APPHIST_MAX_DATASIZE, 0 if freeing previous data

Return Value:
    SUCCESS - No errors

    EBADPARM - nSize + name length > APPHIST_MAX_DATASIZE 

    ENOMEMORY - Insufficient memory or the new number of elements exceeds 
               APPHIST_MAX_DATAELEMENTS

    EPRIVLEVEL - if the caller has insufficient privilege (e.g. the entry does
                   not belong to the caller).

    EFAILED - if no history entries exist

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_GetResumeData(), IAPPHISTORY_GetData()

=============================================================================

IAPPHISTORY_Current()

Description:
    This function allows the caller to find out where in the history list
    this instance of IAppHistory points.

Prototype:
    int  IAPPHISTORY_Current(IAppHistory * po, int *pnIdx);

Parameters:
   po    :  Pointer to the IAppHistory Interface object.
   pnIdx :  [out] filled with the distance from the top-visible app, if
            the entry has been unlinked, pnIdx is set to -1

Return Value:
    SUCCESS - No errors

    EFAILED - if this instance does not point at a history entry
              or no history entries exist

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
   IAPPHISTORY_Top(), IAPPHISTORY_Bottom()

=============================================================================

IAPPHISTORY_GetData()

Description:
    This function allows the caller to retrieve history context sensitive data
associated with the specified input name stored in previous calls to IAPPHISTORY_SetData.
Calling this function with NULL for the pBuffer will fill *pnSize with the size of the buffer.

Prototype:
    int  IAPPHISTORY_GetData(IAppHistory * po,const char * pszName, void * pBuffer, int * pnSize)

Parameters:
   po          :  Pointer to the IAppHistory Interface object.
   pszName     :  Name associated with the data.
   pBuffer     :  Buffer to fill
   pnSize      :  on input, pointer to integer of size in bytes of the buffer, 
                  on output, size of pBuffer necessary to hold the resume data

Return Value:
   SUCCESS - No error

   EPRIVLEVEL - Caller does not have privileges for this operation

   EFAILED - if no history entries exist

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_SetResumeData(), IAPPHISTORY_SetData

=============================================================================

IAPPHISTORY_SetResumeData()

Description:
   This method is a macro call to IAPPHISTORY_SetData with no name (NULL)
   associated with the data. It will send notification if the data got changed.

Prototype:
    int  IAPPHISTORY_SetResumeData(IAppHistory * po, void * pBuffer, int nSize)

Parameters:
   po          :  Pointer to the IAppHistory Interface object.
   pBuffer     :  Buffer to copy.  NULL if freeing the previous data
   nSize:      :  Size in bytes of the data.  Not to exceed 4k

Return Value:
   See IAPPHISTORY_SetData

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_SetData


=============================================================================

IAPPHISTORY_GetResumeData()

Description:
   This method is a macro call to IAPPHISTORY_GetData with no name (NULL)
   associated with the data.

Prototype:
    int  IAPPHISTORY_GetResumeData(IAppHistory * po, void * pBuffer, int * pnSize)

Parameters:
   po          :  Pointer to the IAppHistory Interface object.
   pBuffer     :  Buffer to fill
   pnSize      :  on input, pointer to integer of size in bytes of the buffer, 
                  on output, size of pBuffer necessary to hold the resume data

Return Value:
   See IAPPHISTORY_GetData

Comments: 
   None

Side Effects: 
   None

Version:
   Introduced Brew Client 3.1 

See Also:
    IAPPHISTORY_GetData()

=============================================================================

IAPPHISTORY_Remove()

Description:
  This function removes the current entry from the app history list and notifies
  registered listeners.

Prototype:
    int  IAPPHISTORY_Remove(IAppHistory * pIAppHist);

Parameters:
   pIAppHist   :  Pointer to the IAppHist Interface object.

Return Value:
   SUCCESS - Item removed

   EPRIVLEVEL - Incorrect privilege level for this operation

   EFAILED - if this IAppHistory does not point to a history entry

   EALREADY - Item already being removed

Comments: 
   None

Side Effects: 
  Calling IAPPHISTORY_Remove() on the last entry associated with a loaded
  applet will close the underlying applet. In this case, if the applet that needs to be closed is currently running,
  it will be closed asycnrhonously i.e. calling IAPPHISTORY_Remove() will not result in synchronously sending EVT_APP_STOP
  to the app if the app is currently running. That will be done asynchronously.

  Calling IAPPHISTORY_Remove() on the top-visible applet will cause the app associated with the 
  next history entry to be resumed 
  

  IAPPHISTORY_Remove() will cause IAPPHISTORY_Back() and IAPPHISTORY_Forward() 
  to fail with EINVALIDITEM on this and any other instance of IAppHistory
  that points to the removed entry.  To restore an instance of IAppHistory
  to point into the history list, call IAPPHISTORY_Top().

Version:
   Introduced Brew Client 3.1 

See Also:
   IAPPHISTORY_Top()

=====================================================================
IAPPHISTORY_Move()

Description:
    This function allows the caller to move a history entry to a 
    new position. This function also allows moving an entry to top-visible 
    or away from top-visible. Before moving to top-visible, this function first checks
    whether the app is allowed to move to top-visible using ISHELL_CanStartApplet()
    If the current top-visible spot is impacted by the move operation, the move will
    be done asynchronously. if the current top-visible spot is not impacted by the
    move operation, the move is done synchronously. This function notifies registered
    listeners about the move.

Prototype:
    int  IAPPHISTORY_Move(IAppHistory * po, int nIdx);

Parameters:
   po    :  Pointer to the IAppHistory Interface object.
   nIdx :  [in]: Position where this entry needs to be moved to. It specifies the
   distance from the top-visible app. 
   0 indicates top-visible. -1 indicates move to end of 
   the list.


Return Value:
    SUCCESS : No errors

    EPRIVLEVEL : if the caller has insufficient privilege

    EFAILED : if the history entry has already been unlinked from the list

    ENOSUCH: Value specified by Index is beyond the size of the current AppHistory
    list. To move to end of list, use -1.

    EBADPARM: invalid nIdx i.e < -1
    If attempting to move to top-visible and if the app is not allowed to move
    to top-visible, error codes that are returned from ISHELL_StartApplet() will
    also be returned here.

Comments: 
   None

Side Effects: 
   

Version:
   Introduced Brew Client 3.1 

See Also:
   IAPPHISTORY_Insert(), 
   IAPPHISTORY_Remove(), 
   IAPPHISTORY_Current()

=============================================================================
IAPPHISTORY_GetReason()

Description:
    This function allows the caller to get the start-reason associated with
    this history entry. The start-reason can be set either by using IAppletCtl or
    by usibng IAPPHISTORY_SetReason()

Prototype:
    int IAPPHISTORY_GetReason(IAppHistory * po,uint32 *pdwReason)

Parameters:
   po    :  Pointer to the IAppHistory Interface object.
   pdwReason: uint32* Buffer to fill with reason code on return

Return Value:
    SUCCESS : No errors

    EPRIVLEVEL : if the caller has insufficient privilege

    EFAILED : if the history entry is invalid

    EBADPARM: dwReason is NULL

Comments: 
   None

Side Effects: 
   

Version:
   Introduced Brew Client 3.1 

See Also:
   IAPPLETCTL, 
   IAPPHISTORY_SetReason

=============================================================================
IAPPHISTORY_SetReason()

Description:
    This function allows the caller to set the start-reason associated with
    this history entry. It also notifies listeners about the change.

Prototype:
    int IAPPHISTORY_SetReason(IAppHistory * po,uint32 dwReason)

Parameters:
   po    :  Pointer to the IAppHistory Interface object.
   dwReason: reason code 

Return Value:
    SUCCESS : No errors

    EPRIVLEVEL : if the caller has insufficient privilege

    EFAILED : if the history entry is invalid

Comments:
   None

Side Effects: 
   

Version:
   Introduced Brew Client 3.1 

See Also:
   IAPPLETCTL, 
   IAPPHISTORY_GetReason

=============================================================================*/
#endif   // AEEAPPHIST_H
