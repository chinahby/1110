#ifndef AEERINGER_H
#define AEERINGER_H
/*======================================================
FILE:  AEERinger.h

SERVICES:  BREW Ringer Interface

GENERAL DESCRIPTION:
   Base level definitions, typedefs, etc. for AEEShell

        Copyright ?1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEFile.h"
#include "AEESoundPlayer.h"

typedef struct _IRingerMgr IRingerMgr;
typedef uint32             AEERingerID;
typedef uint32             AEERingerCatID;

#define AEE_RINGER_CATEGORY_ALL  ((AEERingerCatID)0)
#define AEE_RINGER_ID_NONE       ((AEERingerID)0xffffffff)
#ifdef CUST_EDITION	
#define OEM_RINGER_SUPPORT
#endif /*CUST_EDITION*/
#define MAX_RINGER_NAME          (32)

enum {
   ARE_NONE,
   ARE_PLAY,
   ARE_CREATE,
   ARE_WRITE
};
#ifdef WIN32
typedef unsigned AEERingerEvent;
#else
typedef int8 AEERingerEvent;
#endif

typedef void (*PFNRINGEREVENT)(void * pUser, AEERingerEvent evt, uint32 dwParm, int nErr);


typedef struct _AEERingerCat
{
   AEERingerCatID       id;                        // Category ID - See EnumRingerInit
   AEERingerID          idRinger;                  // Ringer ID for default ringer
   AECHAR               szName[MAX_RINGER_NAME];
} AEERingerCat;

typedef struct _AEERingerInfo
{
   AEERingerID          id;
   AEESoundPlayerFile   format;
   boolean              bReadOnly;
   char                 szFile[MAX_FILE_NAME];        // NULL if read-only
   AECHAR               szName[MAX_RINGER_NAME];
} AEERingerInfo;

QINTERFACE(IRingerMgr)
{
   DECLARE_IBASE(IRingerMgr)

   // Supported format functions...

   void           (*RegisterNotify)(IRingerMgr * po,PFNRINGEREVENT pfn, void * pUser);
   int            (*GetNumberFormats)(IRingerMgr * po);
   int            (*GetFormats)(IRingerMgr * po, AEESoundPlayerFile * pwFormat, int nCount);

   // Set ringer by category..

   int            (*SetRinger)(IRingerMgr * po, AEERingerCatID idCat, AEERingerID id);

   // Category enumeration...

   int            (*EnumCategoryInit)(IRingerMgr * po);
   boolean        (*EnumNextCategory)(IRingerMgr * po, AEERingerCat * pc);

   // Ringer enumeration

   int            (*EnumRingerInit)(IRingerMgr * po);
   boolean        (*EnumNextRinger)(IRingerMgr * po,AEERingerInfo * pi);

   AEERingerID    (*GetRingerID)(IRingerMgr * po, const char * pszFile);
   int            (*GetRingerInfo)(IRingerMgr * po, AEERingerID id, AEERingerInfo * pi);

   int            (*Create)(IRingerMgr * po,const AECHAR * pszName, AEESoundPlayerFile format, IAStream * pStream);
   int            (*Remove)(IRingerMgr * po, AEERingerID id);
   int            (*Play)(IRingerMgr * po,AEERingerID id,const char * pszFile, IAStream * pStream,uint32 dwPause);
   int            (*Stop)(IRingerMgr * po);
};

#define IRINGERMGR_AddRef(p)                       GET_PVTBL(p,IRingerMgr)->AddRef(p)
#define IRINGERMGR_Release(p)                      GET_PVTBL(p,IRingerMgr)->Release(p)

#define IRINGERMGR_RegisterNotify(p,pfn,pu)        GET_PVTBL(p,IRingerMgr)->RegisterNotify(p,pfn,pu)
#define IRINGERMGR_GetNumberFormats(p)             GET_PVTBL(p,IRingerMgr)->GetNumberFormats(p)
#define IRINGERMGR_GetFormats(p,pw,n)              GET_PVTBL(p,IRingerMgr)->GetFormats(p,pw,n)
#define IRINGERMGR_SetRinger(p,idc,id)             GET_PVTBL(p,IRingerMgr)->SetRinger(p,idc,id)
#define IRINGERMGR_EnumCategoryInit(p)             GET_PVTBL(p,IRingerMgr)->EnumCategoryInit(p)
#define IRINGERMGR_EnumNextCategory(p,pc)          GET_PVTBL(p,IRingerMgr)->EnumNextCategory(p,pc)
#define IRINGERMGR_EnumRingerInit(p)               GET_PVTBL(p,IRingerMgr)->EnumRingerInit(p)
#define IRINGERMGR_EnumNextRinger(p,pi)            GET_PVTBL(p,IRingerMgr)->EnumNextRinger(p,pi)
#define IRINGERMGR_GetRingerID(p,psz)              GET_PVTBL(p,IRingerMgr)->GetRingerID(p,psz)
#define IRINGERMGR_GetRingerInfo(p,id,pi)          GET_PVTBL(p,IRingerMgr)->GetRingerInfo(p,id,pi)
#define IRINGERMGR_Create(p,pn,f,ps)               GET_PVTBL(p,IRingerMgr)->Create(p,pn,f,ps)
#define IRINGERMGR_Remove(p,id)                    GET_PVTBL(p,IRingerMgr)->Remove(p,id)
#define IRINGERMGR_PlayEx(p,id,psz,ps,dw)          GET_PVTBL(p,IRingerMgr)->Play(p,id,psz,ps,dw)
#define IRINGERMGR_Play(p,id,dw)                   GET_PVTBL(p,IRingerMgr)->Play(p,id,NULL,NULL,dw)
#define IRINGERMGR_PlayFile(p,psz,dw)              GET_PVTBL(p,IRingerMgr)->Play(p,AEE_RINGER_ID_NONE,psz,NULL,dw)
#define IRINGERMGR_PlayStream(p,ps,dw)             GET_PVTBL(p,IRingerMgr)->Play(p,AEE_RINGER_ID_NONE,NULL,ps,dw)
#define IRINGERMGR_Stop(p)                         GET_PVTBL(p,IRingerMgr)->Stop(p)

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEERingerID

Description:
Ringer identifier

Definition:
None

Members:
None

Comments:
None

See Also:
None

=======================================================================
AEERingerCatID

Description:
Ringer category identifier

Definition:
None

Members:
None

Comments:
None

See Also:
None

=======================================================================
AEERingerCat

Description:
Structure containing information about a ringer category.

Definition:
typedef struct _AEERingerCat
{
   AEERingerCatID  id;                           // Category ID - See EnumRingerInit
   AEERingerID     idDefRinger;                  // Ringer ID for default ringer
   AECHAR          szName[MAX_RINGER_CATEGORY];
} AEERingerCat;

Members:

id:            ID for the category for use in later calls
idDefRinger:   ID of default ringer for category
szName:        Wide string name of category

Comments:
None

See Also:
   AEERingerID

=======================================================================
AEERingerInfo

Description:
Structure containing information about a ringer.

Definition:
typedef struct _AEERingerInfo
{
   AEERingerID          id;
   AEESoundPlayerFile   format;
   boolean              bReadOnly;
   char                 szFile[MAX_FILE_NAME];        // NULL if read-only
   AECHAR               szName[MAX_RINGER_NAME];
} AEERingerInfo;

Members:

id:            ID of ringer
format:        Sound format of ringer
bReadOnly:     If the ringer is readonly
szFile:        Full file path to ringer file.  0 string if read-only.
szName:        Name of the ringer.

Comments:
None

See Also:
   AEERingerID
   AEESoundPlayerFile

=============================================================================
AEERingerEvent

Description:
    AEERingerEvent specifies the different notifications sent by the IRingerMgr
Interface. These are sent using the notification function registered using IRINGERMGR_RegisterNotify().

Definition:
    typedef enum {
    ARE_NONE,
    ARE_PLAY,
    ARE_CREATE,
    ARE_WRITE 
    } AEERingerEvent;

Members:
    ARE_NONE :  No notification is sent.
    ARE_PLAY :  Sent when play is done or when IRINGERMGR_Stop() is called.
    ARE_CREATE :  Sent when creation of the ringer is done or when an error occurs.
    ARE_WRITE :  Sent any time the ringer file is written to.

Comments:
    None

See Also:
    PFNRINGEREVENT
    IRINGERMGR_RegisterNotify()

=============================================================================

PFNRINGEREVENT

Description:
    PFNRINGEREVENT specifies the type of notification function registered using
IRINGERMGR_RegisterNotify().

Definition:
    typedef void (* PFNRINGEREVENT)
    (
    void * pUser,
    AEERingerEvent evt,
    uint32 dwParm,
    int nErr
    ); 

Members:
    pUser  :  User data.
    evt  :  The event that specifies the reason for the notification.
    dwParm  :  Contains event-specific information.
    nErr  :  Error codes:
       AEE_SUCCESS, if successful.

       Error code, if otherwise

       Here are the different notifications sent and their event specific parameters:

       ARE_PLAY: Sent when play is done or when IRINGERMGR_stop is called. ~
       dwparam = 0.~
       nErr is EINCOMPLETEITEM if play was still going on, ~
       nErr is 0 if play is done

       ARE_CREATE: Sent when creation of the ringer is done or error. ~
       dwParam contains id of Ringer.~
       nErr: AEE_SUCCESS or EFAILED

       ARE_WRITE: Sent any time a write occurs to the Ringer File. ~
       dwParam: number of bytes written to file.~
       nErr is set to AEE_SUCCESS


Comments:
    None

See Also:
    AEERingerEvent
    IRINGERMGR_RegisterNotify()

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IRingerMgr

Description:
   IRingerMgr provides the BREW interface to OEM-supported ringer functions.
This interface is obtained by calling IRINGERMGR_CreateInstance with
AEECLSID_RINGER.  The class allows the caller to:

- Obtain the list of supported formats
- Obtain the list of ringer categories
- Obtain the list of ringers in a category
- Play a ringer
- Remove a ringer
- Set the ringer for a category (or default)

=======================================================================
Function: IRINGERMGR_AddRef()

Description:
   This function increments the reference count of the IRingerMgr Interface object,
   allowing the object to be shared by multiple callers. The object is freed when the
   reference count reaches 0 (zero).

Prototype:

   uint32 IRINGERMGR_AddRef(IRingerMgr * pIRingerMgr)

Parameters:
   pIRingerMgr: [in]. Pointer to the IRingerMgr Interface object.

Return Value:

   Incremented reference count for the object.

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_Release()

=======================================================================
Function: IRINGERMGR_Release()

Description:
   This function decrements the reference count of the IRingerMgr Interface object. The
   object is freed from memory and is no longer valid when the reference count reaches
   0 (zero).

Prototype:

   uint32 IRINGERMGR_Release(IRingerMgr * pIRingerMgr)

Parameters:

   pIRingerMgr: [in]. Pointer to the IRingerMgr Interface object.

Return Value:

   Decremented reference count for the object.

   0 (zero), If the object has been freed and is no longer valid.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_AddRef()
=======================================================================

Function: IRINGERMGR_GetNumberFormats()

Description:
   This method is called to retrieve the number of ringer formats supported on
the device.

Prototype:

   int   IRINGERMGR_GetNumberFormats(IRingerMgr * po);

Parameters:
   po: [in]. Pointer to the IRingerMgr object

Return Value:

  Number of ringer formats supported.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_GetFormats()

=======================================================================

Function: IRINGERMGR_RegisterNotify()

Description:
   Registers or de-registers a notification callback that is called
when playback or creation events are complete.

Prototype:

   void IRINGERMGR_RegisterNotify(IRingerMgr * po,PFNRINGEREVENT pfn, void * pUser);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pfn: [in]. Pointer to user callback (NULL to de-register)
   pUser: [in]. Pointer to user data for callback

Return Value:
   None

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

Function: IRINGERMGR_GetFormats()

Description:
   This method fills a list of the ringer formats supported.

Prototype:

   int   IRINGERMGR_GetFormats(IRingerMgr * po,AEESoundPlayerFile * pwFormats, int nCount);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pwFormats:  [out]. Pointer to a list of formats of size nCount * sizeof(AEESoundPlayerFile)
   nCount: [in]. Number of format entries to fill

Return Value:

  SUCCESS:  Buffer filled with ringer format entries
  EFAILED:  Invalid size return buffer

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEESoundPlayerFile
   IRINGERMGR_GetNumberFormats()

=======================================================================

Function: IRINGERMGR_SetRinger()

Description:
   This method is allows the caller to specify the ringer to be played from
the selected category.  AEE_RINGER_CATEGORY_ALL can be passed to specify that the
ringer specified is the default ringer.

Prototype:

   int   IRINGERMGR_SetRinger(IRingerMgr * po,AEERingerCatID idCat, AEERingerID id);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   idCat: [in]. Category for the ringer (AEE_RINGER_CATEGORY_ALL for default)
   id: [in]. ID of the ringer

Return Value:

  SUCCESS: Ringer set
  EFAILED: Rinter not set

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_EnumNextCategory()

=======================================================================

Function: IRINGERMGR_EnumCategoryInit()

Description:
   This method initializes the enumeration context category enumeration.

Prototype:

   int   IRINGERMGR_EnumCategoryInit(IRingerMgr * po);

Parameters:
   po: [in]. Pointer to the IRingerMgr object

Return Value:
   SUCCESS: Enumeration initialized.

   Other implementation specific error codes.

Comments:
   There is one interator for this function. Always call this before
   calling IRINGERMGR_EnumNextCategory()

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_EnumNextCategory()

=======================================================================

Function: IRINGERMGR_EnumNextCategory()

Description:
   This method enumerates the next ringer category.

Prototype:

   boolean  IRINGERMGR_EnumNextCategory(IRingerMgr * po,AEERingerCatInfo * pi);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pi: [out]. Pointer to ringer category information to fill

Return Value:
  TRUE: SUCCESS
  FALSE: FAILED or no more categories to enumerate.

Comments:
   There is one iterator for this function. Always call
   IRINGERMGR_EnumCategoryInit() before calling this.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEERingerCatInfo
   IRINGERMGR_EnumCategoryInit()

=======================================================================

Function: IRINGERMGR_EnumRingerInit()

Description:
   This method is called to initialize enumeration of the list of ringers.

Prototype:

   int   IRINGERMGR_EnumRingerInit(IRingerMgr * po);

Parameters:
   po: [in]. Pointer to the IRingerMgr object

Return Value:
   SUCCESS: Enumeration initialized
   EFAILED: Invalid category

   Other implementation specific error codes.

Comments:
   There is one interator for this function. Always call this before
   calling IRINGERMGR_EnumNextRinger()

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_EnumNextRinger()

=======================================================================

Function: IRINGERMGR_EnumNextRinger()

Description:
   This method is called to enumerate the next ringer.

Prototype:

   boolean  IRINGERMGR_EnumNextRinger(IRingerMgr * po, AEERingerInfo * pi);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pi: [out]. Pointer to ringer information to fill

Return Value:
  TRUE: SUCCESS
  FALSE: FAILED or no more ringers to enumerate.

Comments:
   There is one iterator for this function. Always call
   IRINGERMGR_EnumRingerInit() before calling this.

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEERingerInfo
   IRINGERMGR_EnumRingerInit()

=======================================================================

Function: IRINGERMGR_GetRingerID()

Description:
   This method returns the ringer ID for a ringer given the file name.

Prototype:

   AEERingerID IRINGERMGR_GetRingerID(IRingerMgr * po,const char * pszFile);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pszFile: [in]. File name of ringer

Return Value:
   AEERingerID: of the ringer specified.
   AEE_RINGER_NONE: if failed

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

Function: IRINGERMGR_GetRingerInfo()

Description:
   This method is retrieves information about the specified ringer.

Prototype:

   int   IRINGERMGR_GetRingerInfo(IRingerMgr * po, AEERingerID id, AEERingerInfo * pi);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   id: [in]. Ringer ID
   pi: [out]. Pointer to ringer info structure to fill

Return Value:
  SUCCESS:  Ringer info valid
  EFAILED:  Invalid ringer ID, or pi is NULL.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   AEERingerInfo

=======================================================================

Function: IRINGERMGR_Create()

Description:
   This method is called to create a new ringer.

Prototype:

   int   IRINGERMGR_Create(IRingerMgr * po,const AECHAR * pszName,AEESoundPlayerFile format, IAStream * ps);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pszName: [in]. Name of the ringer
   format: [in]. Ringer format
   ps: [in]. Stream to ringer data

Return Value:
  SUCCESS: Ringer is being created
  EFAILED: Invalid ringer

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

Function: IRINGERMGR_Remove()

Description:
   This method is called to remove the specified ringer.

Prototype:

   int   IRINGERMGR_Remove(IRingerMgr * po,AEERingerID id);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   id: [in]. Ringer id

Return Value:
  SUCCESS: Ringer removed
  EFAILED: Invalid ringer
  Other implementation specific error codes.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None

=======================================================================

Function: IRINGERMGR_PlayEx()

Description:
   This method plays the specified ringer using the following (in order)
as the source:

- An installed ringer ID
- An input file name
- An input IAStream object

This allows the caller to test ringers prior to placing them in the ringer
directory.

Prototype:

   int   IRINGERMGR_PlayEx(IRingerMgr * po,AEERingerID id,const char * pszFile, IAStream * pStream, uint32 dwPause);

Parameters:
   po: Pointer to the IRingerMgr object
   id: [in]. Ringer ID to play
   pszFile: [in]. Input file name (if id == AEE_RINGER_ID_NONE)
   pStream: [in]. Input stream (if id == AEE_RINGER_ID_NONE and pszFile == NULL)
   dwPause: [in]. Time to pause between re-plays (0 if single play)

Return Value:
   SUCCESS:  Ringer will begin playing
   EFAILED:  Invalid ringer
   Other implementation specific error codes.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_Stop()

=======================================================================

Function: IRINGERMGR_Play()

Description:
   This method plays an installed ringer.

Prototype:

   int   IRINGERMGR_Play(IRingerMgr * po,AEERingerID id, uint32 dwPause);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   id: [in]. Ringer ID to play
   dwPause: [in]. Time to pause between re-plays (0 if single play)

Return Value:
   SUCCESS:  Ringer will begin playing
   EFAILED:  Invalid ringer
   Other implementation specific error codes.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_Stop()
=======================================================================

Function: IRINGERMGR_PlayFile()

Description:
   This method plays a ringer given an input file name. This allows the caller
to test ringers prior to placing them in the ringer directory.

Prototype:

   int   IRINGERMGR_PlayFile(IRingerMgr * po,const char * pszFile,uint32 dwPause);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pszFile: [in]. Input file name (if id == AEE_RINGER_ID_NONE)
   dwPause: [in]. Time to pause between re-plays (0 if single play)

Return Value:
   SUCCESS:  Ringer will begin playing
   EFAILED:  Invalid ringer
   Other implementation specific error codes.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_Stop()
=======================================================================

Function: IRINGERMGR_PlayStream()

Description:
   This method plays the specified ringer using an input IAStream object.
This allows the caller to test a ringer that may be located in a file (IFile), in
memory (IMemAStream) or via the network (ISocket).

Prototype:

   int   IRINGERMGR_PlayEx(IRingerMgr * po,IAStream * pStream, uint32 dwPause);

Parameters:
   po: [in]. Pointer to the IRingerMgr object
   pStream: [in]. Input stream (if id == AEE_RINGER_ID_NONE and pszFile == NULL)
   dwPause: [in]. Time to pause between re-plays (0 if single play)

Return Value:
   SUCCESS:  Ringer will begin playing
   EFAILED:  Invalid ringer
   Other implementation specific error codes.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_Stop()
=======================================================================

Function: IRINGERMGR_Stop()

Description:
   This method is called to terminate playback of a ringer.

Prototype:

   int   IRINGERMGR_Stop(IRingerMgr * po);

Parameters:
   po: [in]. Pointer to the IRingerMgr object

Return Value:
  SUCCESS:  Ringer is stopping
  EFAILED:  No ringer is playing

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   IRINGERMGR_Play()

=======================================================================*/

#endif      // AEERINGER_H

