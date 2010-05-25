/*======================================================
                    IMPORTANT NOTICE

               OEM/CARRIER DISTRIBUTION ONLY

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement,
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  AEEDownload2.h

SERVICES:  AEE IDownload2 Interface

GENERAL DESCRIPTION:

  This file is provided by QUALCOMM Incorporated to selected carrier partners ONLY.

  It is NOT to be distributed as part of any software development kit.

  It is NOT to be distributed outside of the context specifically noted in licensing
  terms with QUALCOMM.

  Reproduction and/or distribution of this file without the written consent of
  QUALCOMM, Incorporated. is prohibited.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#ifndef AEEDOWNLOAD2_H
#define AEEDOWNLOAD2_H

#include "AEEInterface.h"
#include "AEEFile.h"
#include "AEELicense.h"

typedef uint32 DLITEMID;
typedef uint32 DLPRICEID;

typedef struct
{
   uint32      pID;
   uint32      mID;
   uint32      iID;
} AEEDownloadIDs;

typedef struct
{
   AEELicenseInfo li;              // (GMI_LICENSE)
   uint64         qwSpaceTotal;    // (GMI_SIZE) - total space occupied by  
                                   //  this application 
   uint64         qwSpaceApp;      // (GMI_SIZE) - total space occupied by  
                                   //  application code/resources
   flg            bRemoved:1;      // (GMT_CHECK_REMOVED) - TRUE if the 
                                   //  executables have been removed
   flg            bLocked:1;       // TRUE if user has locked app from auto-    
                                   //  disable
   flg            bRemovable:1;    // (GMI_SIZE) - FALSE if the file(s) are 
                                   //  persistent
   flg            bUpgradable:1;   // (GMI_SIZE) - FALSE if the file(s) are 
                                   //  fixed
   uint32         dwLastUsedTime;  // Time (secs since 1980) since last used 
   uint32         dwInstalledTime; // Time (secs since 1980) of install
   int            nApps;           // (GMI_APPS)
   AEEAppInfo     apps[1];         // (GMI_APPS)
} DL2AppModInfo;

typedef struct _DL2ItemSize
{
   uint64         qwEstRAMRequired; // Estimate for how much ram the item requires
   uint64         qwRAMAvail;       // Largest block
   uint64         qwFSTotal;        // Total size of the app in question 
                                    //  after installation
   uint64         qwFSInstall;      // Total room needed to install the app 
   uint64         qwFSAvail;        // FS space avail for use
   uint64         qwFSSize;         // FS space total in file system
   int            nEstFilesRequired;// Estimate for the number of files req'd.
   int            nFilesAvail;      // FS files avail
} DL2ItemSize;

typedef struct _DL2DisableEntry
{
   DLITEMID       iID;             // Item id of the entry
   uint32         dwLastUsedTime;  // Time (secs since 1980) since last used
   uint64         qwSpace;         // Space that will be freed if app is 
                                   //  disabled
   boolean        bDisable;        // If this entry is set to be disabled by  
                                   //  a call to IDOWNLOAD2_AutoDisable
} DL2DisableEntry;

// These "Args" structs are used internally for IDOWNLOAD2 Apis that call IDOWNLOAD2_Command.
typedef struct _DL2GetModInfoExArgs
{
   uint32           dwFlags;
   DLITEMID         iID;
   DL2AppModInfo*   pami;

} DL2GetModInfoExArgs;

typedef struct _DL2GetSizeExArgs
{
   DLITEMID         iID;
   DL2ItemSize*     pis;

} DL2GetSizeExArgs;

typedef struct _DL2GetAutoDisableListArgs
{
   DLITEMID         idWant;
   uint64           qwExtra;
   int*             pnCount;
   int*             pnErr;
   DL2DisableEntry* padl;

} DL2GetAutoDisableListArgs;


//Action  types for Command API
#define DOWNLOAD2_COMMAND        0x0000
// 1-7 reserved for commands to be integrated from 4.x
#define CMD_GET_MODINFO          (DOWNLOAD2_COMMAND + 8)
#define CMD_GET_MODINFOEX        (DOWNLOAD2_COMMAND + 9)
#define CMD_GET_SIZEEX           (DOWNLOAD2_COMMAND + 10)
#define CMD_GET_AUTODISABLELIST  (DOWNLOAD2_COMMAND + 11)
#define CMD_AUTODISABLE          (DOWNLOAD2_COMMAND + 12)

#define DOWNLOAD2_TXN_DOWNLOAD   1
#define DOWNLOAD2_TXN_DELETE     2
//
// Generic IDownload callback structure.
//
typedef void (*PFNDLCOMMAND)(void * pv, int nErr, void * pUnused);
typedef void (*PFNITEMINFO)(void *pv, const char *pszLine, int nLen);
typedef int  (*PFNPKGFILEOPENER)(void *pv, AEEDownloadIDs *pids, 
                                 const char *pszFile, IFile **ppf);

//Synchronization Flags used in ServerSync and SetDownloadSyncFlags
#define DL_SS_NONE         0x00000000
#define DL_SS_RECALL       0x00000001
#define DL_SS_ACTIONLIST   0x00000002
#define DL_SS_WELCOME_MSG  0x00000004
#define DL_SS_FLUSH_ACKS   0x00000008  // This flags is set automatically if one of 
                                       // the DL_SS_RECALL or DL_SS_ACTIONLIST flags is chosen
#define DL_SS_ALL          0xFFFFFFFF

#define AEEIID_DOWNLOAD2    	0x01038478

#define INHERIT_IDownload2(iname)\
   INHERIT_IQueryInterface(iname); \
   int  (*Command)(iname *po, int nActionType, void *pv); \
   void (*SetDownloadSyncFlags)(iname *po, int nFlags); \
   void (*ServerSync)(iname *po, int nSyncFlags, PFNDLCOMMAND pfn, void *pcxt); \
   void (*SetItemListener)(iname* po, PFNITEMINFO pfn, void* pcxt);\
   void (*AcquireItem)(iname *po, DLITEMID iID, DLPRICEID dwPrice, \
                    PFNPKGFILEOPENER pfnOpener, void *pvOpener, \
                    PFNDLCOMMAND pfn, void * pcxt);\
   int  (*MakeTxn)(iname* po, int nAckType, DLITEMID iID, DLITEMID catID, \
                     DLPRICEID dwPrice, const char *pszSID, \
                     byte *pszTxn, int *pnTxn); \
   void (*SendTxn)(iname* po, byte *pszTxn, int nTxn, int nRepeatCount, \
                     PFNDLCOMMAND pfn, void * pcxt)

   
AEEINTERFACE_DEFINE(IDownload2);

static __inline uint32 IDOWNLOAD2_AddRef(IDownload2 *p) 
{
   return AEEGETPVTBL((p),IDownload2)->AddRef(p);
}

static __inline uint32 IDOWNLOAD2_Release(IDownload2 *p) 
{
   return AEEGETPVTBL((p),IDownload2)->Release(p);
}

static __inline int IDOWNLOAD2_QueryInterface(IDownload2 *p, AEECLSID id, void **ppo)
{
   return AEEGETPVTBL((p),IDownload2)->QueryInterface(p,id,ppo);
}

static __inline void IDOWNLOAD2_SetDownloadSyncFlags(IDownload2 *p, int nFlags) 
{
   AEEGETPVTBL((p),IDownload2)->SetDownloadSyncFlags(p, nFlags);
}


static __inline void IDOWNLOAD2_ServerSync(IDownload2 *p, int nSyncFlags, PFNDLCOMMAND pfn, void *pcxt) 
{
   AEEGETPVTBL((p),IDownload2)->ServerSync(p, nSyncFlags, pfn, pcxt);
}


static __inline void IDOWNLOAD2_SetItemListener(IDownload2 *p, PFNITEMINFO pfn, void *pcxt) 
{
   AEEGETPVTBL((p),IDownload2)->SetItemListener(p, pfn, pcxt);
}

static __inline void IDOWNLOAD2_AcquireItem(IDownload2 *p, DLITEMID iID, DLPRICEID dwPrice,
                                        PFNPKGFILEOPENER pfnOpener, void *pvOpener,
                                        PFNDLCOMMAND pfn, void *pcxt)
{
   AEEGETPVTBL((p),IDownload2)->AcquireItem(p, iID, dwPrice, pfnOpener, pvOpener, pfn, pcxt);
}

static __inline int IDOWNLOAD2_MakeTxn(IDownload2* po, int nAckType, DLITEMID iID, DLITEMID catID, 
                     DLPRICEID dwPrice, const char *pszSID, byte *pszTxn, int *pnTxn)
{
   return AEEGETPVTBL((po),IDownload2)->MakeTxn(po, nAckType, iID, catID, dwPrice, pszSID, pszTxn, pnTxn);
}

static __inline int IDOWNLOAD2_MakeDownloadTxn(IDownload2* po, DLITEMID iID, DLITEMID catID, 
                     DLPRICEID dwPrice, byte *pszTxn, int *pnTxn)
{
   return AEEGETPVTBL((po),IDownload2)->MakeTxn(po, DOWNLOAD2_TXN_DOWNLOAD, iID, catID, dwPrice, 0, pszTxn, pnTxn);
}

static __inline int IDOWNLOAD2_MakeDeleteTxn(IDownload2* po, DLITEMID iID, 
                     DLPRICEID dwPrice,  const char *pszSID, byte *pszTxn, int *pnTxn)
{
   return AEEGETPVTBL((po),IDownload2)->MakeTxn(po, DOWNLOAD2_TXN_DELETE, iID, 0, dwPrice, pszSID, pszTxn, pnTxn);
}


static __inline void IDOWNLOAD2_SendTxn(IDownload2* po, byte *pszTxn, int nTxnLen, 
                                        int nRepeatCount,
                                        PFNDLCOMMAND pfn, void * pcxt)
{
   AEEGETPVTBL((po),IDownload2)->SendTxn(po, pszTxn, nTxnLen, nRepeatCount, pfn, pcxt);
}

static __inline DL2AppModInfo* IDOWNLOAD2_GetModInfo(IDownload2 * po, DLITEMID iID)
{  
   DL2GetModInfoExArgs args;
   args.iID = iID;
   args.pami = NULL;

   AEEGETPVTBL((po),IDownload2)->Command(po, CMD_GET_MODINFO, (void*)&args);

   return args.pami;
}

static __inline DL2AppModInfo* IDOWNLOAD2_GetModInfoEx(IDownload2 * po, DLITEMID iID, uint32 dwFlags)
{
   DL2GetModInfoExArgs args;
   
   args.iID = iID;
   args.dwFlags = dwFlags;
   args.pami = NULL;

   AEEGETPVTBL((po),IDownload2)->Command(po, CMD_GET_MODINFOEX, (void*)&args);
   
   return args.pami;
}

static __inline uint64 IDOWNLOAD2_GetSize(IDownload2 *po, DLITEMID iID, uint64 * pqwTotal)
{
   DL2GetSizeExArgs args;
   DL2ItemSize is;
   
   args.iID = iID;
   args.pis = &is;
   AEEGETPVTBL((po),IDownload2)->Command(po, CMD_GET_SIZEEX, (void*)&args);
   if (pqwTotal) {
      *pqwTotal = is.qwFSTotal;
   }

   return is.qwFSInstall;
}

static __inline int IDOWNLOAD2_GetSizeEx(IDownload2 * po,DLITEMID iID, DL2ItemSize * psi)
{
   DL2GetSizeExArgs args;

   args.iID = iID;
   args.pis = psi;
   
   return AEEGETPVTBL((po),IDownload2)->Command(po, CMD_GET_SIZEEX, (void*)&args);
}

static __inline int IDOWNLOAD2_GetAvailable(IDownload2 * po, DL2ItemSize * psi)
{
   DL2GetSizeExArgs args;

   args.iID = 0;
   args.pis = psi;

   return AEEGETPVTBL((po),IDownload2)->Command(po, CMD_GET_SIZEEX, (void*)&args);
}

static __inline DL2DisableEntry* IDOWNLOAD2_GetAutoDisableList(IDownload2 * po, DLITEMID idWant, 
                                                                uint64 qwExtra, int * pnCount,int * pnErr)
{
   DL2GetAutoDisableListArgs args;

   args.idWant = idWant;
   args.qwExtra = qwExtra;
   args.pnCount = pnCount;
   args.pnErr = pnErr;
   args.padl = NULL;

   AEEGETPVTBL((po),IDownload2)->Command(po, CMD_GET_AUTODISABLELIST, (void*)&args);

   return args.padl;
}

static __inline int IDOWNLOAD2_AutoDisable(IDownload2 * po, DLITEMID iID)
{
   return AEEGETPVTBL((po),IDownload2)->Command(po, CMD_AUTODISABLE, (void*)&iID);
}
/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

PFNDLCOMMAND

Description:
    PFNDLCOMMAND specifies a function pointer used by various IDownload
    function to report the results of a particular operation.

Definition:
    typedef void (*PFNDLCOMMAND)(void * pv, int nErr, void * pvSpec);

Members:
    pv:     The argument given in addition to the calback
    nErr:   Error code (including SUCCESS)
    pvSpec: A data structure specific to the call (Could be NULL)

Comments:
    None

See Also:
    None

=======================================================================

PFNITEMINFO

Description:
    PFNITEMINFO specifies a function callback pointer. When supplied to
    IDownload, it is called back with each protocol line information.

Definition:
    typedef void (*PFNITEMINFO)(void *pv, const char *pszLine, int nLen);

Members:
    pv:        The argument given in addition to the calback
    pszLine:   A character string of the protocol data
    nLen:      Line Length

Comments:
    None

See Also:
    None

=======================================================================

PFNPKGFILEOPENER

Description:
    PFNPKGFILEOPENER specifies a function callback pointer. It is used
    by the caller to obtain a IFile pointer from the callee based on 
    the given file name.

Definition:
    typedef int  (*PFNPKGFILEOPENER)(void *pv, AEEDownloadIDs *pids, 
                                 const char *pszFile, IFile **ppf);


Members:
    pv:      The argument given in addition to the calback
    pids:    Ids that give context information about the file name
    pszFile: File name to be opened
    ppf:     Opened file pointer is returned here to the caller

Comments:
    None

See Also:
    None

=======================================================================

DL2AppModInfo

Description:
    DL2AppModInfo is the 64 bit version of AppModInfo returned by
    IDOWNLOAD2_GetModInfo and IDOWNLOAD2_GetModInfoEx.

Definition:

typedef struct
{
   AEELicenseInfo li;              // (GMI_LICENSE)
   uint64         qwSpaceTotal;    // (GMI_SIZE) - total space occupied by  
                                   //  this application 
   uint64         qwSpaceApp;      // (GMI_SIZE) - total space occupied by  
                                   //  application code/resources
   flg            bRemoved:1;      // (GMT_CHECK_REMOVED) - TRUE if the 
                                   //  executables have been removed
   flg            bLocked:1;       // TRUE if user has locked app from auto-    
                                   //  disable
   flg            bRemovable:1;    // (GMI_SIZE) - FALSE if the file(s) are 
                                   //  persistent
   flg            bUpgradable:1;   // (GMI_SIZE) - FALSE if the file(s) are 
                                   //  fixed
   uint32         dwLastUsedTime;  // Time (secs since 1980) since last used 
   uint32         dwInstalledTime; // Time (secs since 1980) of install
   int            nApps;           // (GMI_APPS)
   AEEAppInfo     apps[1];         // (GMI_APPS)
} DL2AppModInfo;


Members:
    See above

Comments:
    None

See Also:
    IDOWNLOAD2_GetModInfo and IDOWNLOAD2_GetModInfoEx

=======================================================================

DL2ItemSize

Description:
    DL2ItemSize is the 64 bit version of the DLItemSize struct which is passed
    in and filled by calls to IDOWNLOAD2_GetSizeEx and IDOWNLOAD2_GetAvailable.

Definition:
   
typedef struct _DL2ItemSize
{
   uint64         qwEstRAMRequired; // Estimate for how much ram the item requires
   uint64         qwRAMAvail;       // Largest block
   uint64         qwFSTotal;        // Total size of the app in question 
                                    //  after installation
   uint64         qwFSInstall;      // Total room needed to install the app 
   uint64         qwFSAvail;        // FS space avail for use
   uint64         qwFSSize;         // FS space total in file system
   int            nEstFilesRequired;// Estimate for the number of files req'd.
   int            nFilesAvail;      // FS files avail
} DL2ItemSize;

Members:
    See above

Comments:
    None

See Also:
    IDOWNLOAD2_GetSizeEx and IDOWNLOAD2_GetAvailable

=======================================================================

DL2DisableEntry

Description:
    DL2DisableEntry is the 64 bit version of DLDisableEntry returned by
    IDOWNLOAD2_GetAutoDisableList.

Definition:

typedef struct _DL2DisableEntry
{
   DLITEMID       iID;             // Item id of the entry
   uint32         dwLastUsedTime;  // Time (secs since 1980) since last used
   uint64         qwSpace;         // Space that will be freed if app is 
                                   //  disabled
   boolean        bDisable;        // If this entry is set to be disabled by  
                                   //  a call to IDOWNLOAD2_AutoDisable
} DL2DisableEntry;

Members:
    See above

Comments:
    None

See Also:
    IDOWNLOAD2_GetAutoDisableList and IDOWNLOAD2_AutoDisable

=======================================================================*/



/*=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IDownload2

Description:
   This interface provides access to BREW application and file download
   mechanisms.  It is for use ONLY by QUALCOMM, Handset Manufacturer and
   selected partners.

   IDownload2 and IDownload are two interfaces for the same base class.
   If using APIs from both interfaces callers should first create an instance
   of IDownload2, and then use IDownload2_QueryInterface to get an instance 
   of the IDownload interface.  When finished both pointers should be 
   released.

=======================================================================
IDOWNLOAD2_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the 
   definition of the specified class ID. This can be used to query for 
   extended functionality, like future versions or proprietary extensions. 

   Upon a successful query, the interace is returned AddRef'ed. The caller 
   is responsible for calling Release() at some point in the future. One 
   exception is when the pointer returned is not an interface pointer. In 
   that case, the memory will share the lifetime of the object being queried, 
   and the returned pointer will not be used to free or release the object. 

Prototype:
   int IDOWNLOAD2_QueryInterface(IDownload2 *po, AEECLSID id, void **p)
 

Parameters:

   po [in]: Pointer to IDownload2 interface. 
   id [in]: A globally unique id to identify the entity (interface or data) that we are trying to query. 
      ids supported:
         AEEIID_DOWNLOAD
         AEEIID_DOWNLOAD2
         AEEIID_DLSIGCHECK
   p [out]: Pointer to the data or interface that we want to retrieve. If the interface is not available, this is set to NULL. 

Return Value:

   Return SUCCESS on success, 
   ECLASSNOTSUPPORT if class ID not supported 

Comments:

   IDownload2 and IDownload are two interfaces for the same base class.
   If using APIs from both interfaces callers should first create an instance
   of IDownload2, and then call IDOWNLOAD2_QueryInterface to get an instance 
   of the IDownload interface.  When finished both pointers should be 
   released.

Side Effects:
   
   None.
=======================================================================
Function: IDOWNLOAD2_Command()

Description:
    This function is a generic way to make calls into IDownload.  
    It can be used to extend the IDownload2 functionality without 
    modifying the vtable. For most purposes, there will be a wrapper
    function around this call.

Prototype:
   int IDOWNLOAD2_Command(IDownload2 *pme, int nActionType, void *pv1, void *pv2);

Parameters:
    po [in]: Pointer to the IDownload interface object.
    nActionType [in]: id of the type of command.
    pv1 [in/out]: User-defined 32 bit value
    pv2 [in/out]: User-defined 32 bit value

Return Value:
    EUNSUPPORTED: If no such command type is implemented
    others: specific to the command type

Comments:
    None

Side Effects:
    None

=======================================================================
Function: IDOWNLOAD2_SetDownloadSyncFlags()

Description:
    Use this function to set flags that determine which steps of the
    server synchronization will get executed. Usually when certain IDownload
    function are executed, they go through the synchronization work and this
    function sets flags that control the scope of the sync work wanted if any.

    Following are the function that go through sync states:

    IDOWNLOAD_GetItemInfo
    IDOWNLOAD_Acquire
    IDOWNLOAD_Enum
    IDOWNLOAD_Search
    IDOWNLOAD_GetItemList
    IDOWNLOAD_Credit
    IDOWNLOAD_CheckUpgrade
    IDOWNLOAD_GetCategoryList
    IDOWNLOAD_CheckUpgrades
    IDOWNLOAD_DeleteEx

    Sync states:
    DL_SS_RECALL      - Perform recalls
    DL_SS_ACTIONLIST  - Perform action lists
    DL_SS_WELCOME_MSG - Message received
    DL_SS_FLUSH_ACKS  - Flush ACKs; 
                        This flags is set automatically if one of 
                        the DL_SS_RECALL or DL_SS_ACTIONLIST flags is chosen
    DL_SS_ALL         - All of above 

Prototype:
   void IDOWNLOAD2_SetDownloadSyncFlags(IDownload2 *po, int nFlags)

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    nFlags [in]: synchronization flags
    
Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD2_ServerSync()

=======================================================================
Function: IDOWNLOAD2_ServerSync()

Description:
    Use this function to synchonize with server.  ServerSync flags 
    determine the kind of actions executed by this command.  Synchrnization
    action are as follows: doing recalls, perform action lists, 
    retrieving welcome message and flushing ACKs.

Prototype:
   void IDOWNLOAD2_ServerSync(IDownload2 *po, int nSyncFlags, PFNDLCOMMAND pfn, void *pcxt)
   

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    nSyncFlags [in]: synchronization flags
    pfn [in]: function to call upon synchronization or on error
    pcxt [in]: Callback context to be supplied back as first argument

Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD2_SetDownloadSyncFlags()

=======================================================================
Function: IDOWNLOAD2_SetItemListener()

Description:
    IDownload2 objects communicates with the ADS server using a line based
    protocol.  When requesting information, each line comes down in the 
    format of a "tag: value" and gets parsed and information accumulated.  
    As IDownload2 may not understand certain tags, it is best that these 
    lines are parsed by the client which initiated the server connection.

    SetItemListenr is the function used to install such a callback which 
    is called each time a line is obtained from the server. This callback 
    then can utilize the line information appropriately. 

    Unregister a previously set callback by calling SetItemListener again 
    with NULL values for pfn and pcxt.

Prototype:
   void IDOWNLOAD2_SetItemListener(IDownload2 *p, PFNITEMINFO pfn, void *pcxt)

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    pfn [in]: Callback to be called with line information
    pcxt [in]: Callback context to be supplied back as first argument

Return Value:
    None

Comments:

Side Effects:
    None

========================================================================
Function: IDOWNLOAD2_AcquireItem()

Description:
    This function is used to download an item (a list of files) to the
    client specified location. The client chooses the location of the 
    file, opens the IFile object and returns it to IDownload2 as part of 
    the pfnOpener callback. At the end of the download or on error, the
    pfnResp callback is called with appropriate callback.

    For this function to work properly, IDownload must know about the 
    item information.  This means IDOWNLOAD_GetItemInfo must be called
    before calling AcquireItem with the same item id.

Prototype:
   void IDOWNLOAD2_AcquireItem(IDownload2 *po, DLITEMID iID, DLPRICEID dwPrice,
                           PFNPKGFILEOPENER pfnOpener, void *pvOpener,
                           PFNDLCOMMAND pfnResp, void *pcxt)

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    iID [in]: Item ID to be downloaded
    dwPrice [in]: Price handle to use
    pfnOpener [in]: To be called when opening a new file
    pvOpener [in]: pfnOpener context to be supplied back as first argument
    pfnResp [in]: Callback to be called with status information
    pcxt [in]: Callback context to be supplied back as first argument

Return Value:
    None

Comments:

Side Effects:
    None

========================================================================
Function: IDOWNLOAD2_MakeTxn()

Description:
    This function amalgamates the transaction data into the given buffer.
    This Txn buffer can then be used to send the transaction to the server.

Prototype:
   int IDOWNLOAD2_MakeTxn(IDownload2 *po, int nAckType, 
                           DLITEMID iID, DLITEMID catID,
                           DLPRICEID dwPrice, 
                            const char *pszSID, 
                            byte *pszTxn, int *pnTxnLen)
   

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    nAckType [in]: Type of transaction 
                  (DOWNLOAD2_TXN_DOWNLOAD or DOWNLOAD2_TXN_DELETE)
    iID [in]: Item ID that needs to be registered
    catID [in]: Specifies the category in which the item ID is found.
                Mostly for tracking purposes; use 0 if not known
    dwPrice [in]: Price Handle value
    pszSID [in]: SID value at the time of download
                 Pass 0 if the SID is same as the current SID
    pszTxn [out]: buffer to write the txn data
    pnTxnLen [in/out]: On input, contains size of the pszTxn buffer
                       On output, updated with size of the txn buffer
                       If input size is not sufficient, call the function
                       again with the needed size buffer as given in pnTxnLen

Return Value:
    SUCCESS if txn is updated
    ENEEDMORE if the supplied buffer in not sufficent for storing the Txn
    ENOMEMORY if insufficient memory
    EBADPARM if nAckType is unsupported or unknown

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD2_MakeDownloadTxn()
    IDOWNLOAD2_MakeDeleteTxn()
    IDOWNLOAD2_SendTxn()
=======================================================================
Function: IDOWNLOAD2_MakeDeleteTxn()

Description:
    This function amalgamates the transaction data for a delete
    into the given buffer. This Txn buffer can then be used to send 
    the transaction to the server.
Prototype:
   void IDOWNLOAD2_MakeDeleteTxn(IDownload2 *po, DLITEMID iID, 
                                 DLPRICEID dwPrice,  const char *pszSID, 
                                 byte *pszTxn, int *pnTxn)
   

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    iID [in]: Item ID that needs to be registered
    dwPrice [in]: Price Handle value
    pszSID [in]: SID value at the time of download
                 Pass 0 if the SID is same as the current SID
    pszTxn [out]: buffer to write the txn data
    pnTxnLen [in/out]: On input, contains size of the pszTxn buffer
                       On output, updated with size of the txn buffer
                       If input size is not sufficient, call the function
                       again with the needed size buffer as given in pnTxnLen


Return Value:
    SUCCESS if txn is updated
    ENEEDMORE if the supplied buffer in not sufficent for storing the Txn
    ENOMEMORY if insufficient memory

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD2_MakeDownloadTxn()
    IDOWNLOAD2_SendTxn()
=======================================================================
Function: IDOWNLOAD2_MakeDownloadTxn()

Description:
    This function amalgamates the transaction data for a download
    into the given buffer. This Txn buffer can then be used to send 
    the transaction to the server.

Prototype:
   int IDOWNLOAD2_MakeDownloadTxn(IDownload2 *po, DLITEMID iID, DLITEMID catID,
                           DLPRICEID dwPrice, byte * pszTxn, int * pnTxnLen)
   

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    iID [in]: Item ID that needs to be registered
    catID [in]: Specifies the category in which the item ID is found.
                Mostly for tracking purposes; use 0 if not known
    dwPrice [in]: Price Handle value
    pszTxn [out]: buffer to write the txn data
    pnTxnLen [in/out]: On input, contains size of the pszTxn buffer
                       On output, updated with size of the txn buffer
                       If input size is not sufficient, call the function
                       again with the needed size buffer as given in pnTxnLen


Return Value:
    SUCCESS if txn is updated
    ENEEDMORE if the supplied buffer in not sufficent for storing the Txn
    ENOMEMORY if insufficient memory

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD2_MakeDownloadTxn()
    IDOWNLOAD2_SendTxn()

=======================================================================
Function: IDOWNLOAD2_SendTxn()

Description:
    This function registers a download/delete transaction with the ADS
    server. For some reason if the Txn has to be retried, specify the
    repeat count as appropriate.

Prototype:
   void IDOWNLOAD2_SendTxn(IDownload2 *po, 
                           byte *pszTxn, int nTxnLen, int nRepeatCount,
                           PFNDLCOMMAND pfn, void * pcxt)
   

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    pszTxn [in]: Txn buffer created using IDOWNLOAD2_MakeTxn function
    nTxnLen [in]: Length of the pszTxn buffer
    nRepeatCount [in]: Use 0 for first time.  Increment the count each time
                       the function is retried with the same Txn buffer
    pfn [in]: function to call upon registration or on error
    pcxt [in]: Callback context to be supplied back as first argument

Return Value:
    None

Comments:
    None

Side Effects:
    None

See Also:
    IDOWNLOAD2_MakeTxn()
    IDOWNLOAD2_MakeDownloadTxn()
    IDOWNLOAD2_MakeDeleteTxn()
=======================================================================
Function: IDOWNLOAD2_GetModInfo()

Description:
    This function allocates and returns a structure containing information
    regarding applications associated with a particular DLITEMID.

    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.

Prototype:
   DL2AppModInfo * IDOWNLOAD2_GetModInfo(IDownload2 *po, DLITEMID appID);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    appID [in]: App ID (returned from IDOWNLOAD_GetAppIDList())

Return Value:
    SUCCESS: Returns a pointer to the DL2AppModInfo.
    FAILURE: NULL

Comments:
    The pointer returned is valid until a subsequent call is made to
    IDOWNLOAD2_GetModInfo() or IDOWNLOAD2_getModInfoEx, or the interface 
    is released.

Side Effects:
    None

========================================================================
Function: IDOWNLOAD2_GetModInfoEx()

Description:
    This function allocates and returns a structure containing information
    regarding applications associated with a particular DLITEMID.  The 
    information returned is controlled by the flags specified.  The following
    flags are supported:


   GMI_LICENSE       - Return the license information
   GMI_CHECK_REMOVED - Determines if module executable has been removed
   GMI_SIZE          - Return the file size information
   GMI_APPS          - Return the applications for this module
   GMI_ALL           - Populate all fields
   
    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.

Prototype:
   DL2AppModInfo * IDOWNLOAD2_GetModInfoEx(IDownload2 *po, DLITEMID appID,uint32 dwFlags);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    appID [in]: App ID (returned from IDOWNLOAD_GetAppIDList())
    dwFlags [in]: Flags (see above)

Return Value:
    SUCCESS: Returns a pointer to the DL2AppModInfo.
    FAILURE: NULL

Comments:
    The pointer returned is valid until a subsequent call is made to
    IDOWNLOAD2_GetModInfo() or IDOWNLOAD2_getModInfoEx, or the interface 
    is released.

Side Effects:
    None

========================================================================
Function: IDOWNLOAD2_GetSize()

Description:
    This function returns the total size required to download any missing
    portions of the specified item.  If non-NULL, the pqwTotal is filled
    with the total size of all files for the item.
   
    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.

Prototype:

   uint64 IDOWNLOAD2_GetSize(IDownload2 *po, DLITEMID iID, uint64 * pqwTotal);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    iID [in]: Item ID
    pqwTotal [out]: Total bytes of all files (resident and non-resident)

Return Value:
    Size of non-resident file(s) to be downloaded.

Comments:
    None

Side Effects:
    None

========================================================================
Function: IDOWNLOAD_GetSizeEx()

Description:
    This method calculates the size required to store an item of 1-N packages.
    It returns an error if the size required is unavailable on the device.
   
    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.

Prototype:
   int IDOWNLOAD2_GetSizeEx(IDownload2 * po,DLITEMID iID, DL2ItemSize * psi);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    iID [in]: Item ID to check
    psi [out]: Size information

Return Value:
    SUCCESS - If the size is obtained
    EFSFULL - Insufficient FS storage space (qwFSAvail < qwFSRequired)
    ENOMEMORY - Insufficient contiguous RAM for the item (dwRAM < dwEstRAMRequired)
    EOUTOFNODES - Insufficient file handles available (nFilesAvail < nEstFileRequired)
    EBADPARM - If psi is NULL or the package is invalid

Comments:
    None

Side Effects:

========================================================================
Function: IDOWNLOAD2_GetAvailable()

Description:
    This method is called to populate available file and RAM space for
    potential downloads.

    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.
  
Prototype:
   int IDOWNLOAD2_GetAvailable(IDownload2 * po,DL2ItemSize * psi);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    psi [out]: Size information

Return Value:
    EFSFULL - Insufficient FS storage space (dwFSAvail < dwFSRequired)
    ENOMEMORY - Insufficient contiguous RAM for the item (dwRAM < dwEstRAMRequired)
    EOUTOFNODES - Insufficient file handles available (nFilesAvail < nEstFileRequired)
    EBADPARM - If psi is NULL or the package is invalid

Comments:
    None

Side Effects:
========================================================================
Function: IDOWNLOAD2_GetAutoDisableList()

Description:
    This function retrieves the list of entries that can be auto-disabled.  The
    idWant/dwExtra parameters can be specified in order to mark those items
    that would be candidates to disable based upon size and date/time last used.

    The list is returned sorted in least-recently used order.

    By using DLITEMID, this call is equivalent to the IDOWNLOAD_AutoDisable function
    without the function automatically disabling the items.

    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.

Prototype:
   DL2DisableEntry * IDOWNLOAD2_GetAutoDisableList(IDownload2 * po,DLITEMID idWant, uint64 qwExtra, int * pnCount,int * pnErr);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    idWant[in]: ID of item that may be downloaded. 0 if all entries
    qwExtra[in] : Extra number of bytes desired.
    pnCount[out]: Number of entries in the list
    pnErr[out] : Pointer to error

Return Value:
   Returns the list that can be auto-disabled
   NULL - Indicates an error
   *pnErr:
     SUCCESS - idWant + dwExtra size is available in file system
     EFSFULL - idWant + dwExtra cannot be satisified by disable

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD_Lock(), IDOWNLOAD2_AutoDisable()

========================================================================
Function: IDOWNLOAD2_AutoDisable()

Description:
    This function is used to auto-disable applications. Applications are
    "auto-disabled" in the following order...
     1)  List is scanned and apps are marked for "auto-disable" in least-recently-used
         order until enough space is recovered.
     2)  This sub-list is scanned backward and apps are "unmarked" if the
         necessary space can be achieved without them.

     This covers the following example:

     Space Required: 33K

     App A    10K
     App B    11K
     App C    23K

     After Step 1, all three apps are marked for disable.
     After Step 2, only Apps A and Apps C are marked.  App B is no longer marked because
     it can be left enabled and the space can still be recovered.

    This is the uint64 equivalent to the IDOWNLOAD function of the same name 
    for file systems > 4GB support. The uint32 IDOWNLOAD functions, structures 
    and function signatures are kept the same to maintain backward 
    compatibility.  Going forward, callers should always use the IDOWNLOAD2 
    uint64 versions of the functions.

Prototype:
   int IDOWNLOAD2_AutoDisable(IDownload2 * po,DLITEMID iID);

Parameters:
    po [in]: Pointer to the IDownload2 interface object.
    id [in]: Item ID for the operation.

Return Value:
    SUCCESS - If applications are successfully auto-disabled
    EFSFULL - Insufficient FS storage space (dwFSAvail < dwFSRequired)
    ENOMEMORY - Insufficient contiguous RAM for the item (dwRAM < dwEstRAMRequired)

Comments:
   None

Side Effects:
   None

See Also:
   IDOWNLOAD2_GetAutoDisableList()
   IDOWNLOAD_Lock()

=======================================================================
*/



#endif
