/*======================================================
                    IMPORTANT NOTICE

               OEM/CARRIER DISTRIBUTION ONLY

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement,
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  AEEDLSigCheck.h

SERVICES:  AEE IDLSigCheck Interface

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
#ifndef AEEDLSigCheck_H
#define AEEDLSigCheck_H

#include "AEEInterface.h"
#include "AEEFile.h"

//
// Generic IDownload callback structure.
//
typedef int  (*PFNPKGFILEREADER)(void *pv, const char *pszFile, IFile **ppf);

#define AEEIID_DLSIGCHECK    	0x0103847b

#define INHERIT_IDLSigCheck(iname)\
   INHERIT_IQueryInterface(iname); \
   int  (*CheckSig)(iname* po, const char *pszSigFile, \
                     PFNPKGFILEREADER pfnFileReader, void *pvOpener)

   
AEEINTERFACE_DEFINE(IDLSigCheck);

static __inline uint32 IDLSIGCHECK_AddRef(IDLSigCheck *p) 
{
   return AEEGETPVTBL((p),IDLSigCheck)->AddRef(p);
}

static __inline uint32 IDLSIGCHECK_Release(IDLSigCheck *p) 
{
   return AEEGETPVTBL((p),IDLSigCheck)->Release(p);
}

static __inline int IDLSIGCHECK_QueryInterface(IDLSigCheck *p, AEECLSID id, void **ppo)
{
   return AEEGETPVTBL((p),IDLSigCheck)->QueryInterface(p,id,ppo);
}


static __inline int IDLSIGCHECK_CheckSig(IDLSigCheck *p, const char *pszSig, 
                                        PFNPKGFILEREADER pfnFileReader, void *pv)
{
   return AEEGETPVTBL((p),IDLSigCheck)->CheckSig(p, pszSig, pfnFileReader, pv);
}


/*
=======================================================================
  DATA STRUCTURES DOCUMENTATION
=======================================================================

PFNPKGFILEREADER

Description:
    PFNPKGFILEREADER specifies a function callback pointer. It is used
    by the caller to obtain a IFile pointer from the callee based on 
    the given file name.

Definition:
    typedef int  (*PFNPKGFILEREADER)(void *pv, const char *pszFile, 
                                               IFile **ppf);


Members:
    pv:      The argument given in addition to the calback
    pszFile: File name to be opened
    ppf:     Opened file pointer is returned here to the caller

Comments:
    None

See Also:
    None

=======================================================================*/


/*=======================================================================
  INTERFACE DOCUMENTATION
=======================================================================

Interface Name: IDLSigCheck

Description:
   This interface provides access to first version of BREW Signature 
   check mechanism as needed to verify files downloaded

=======================================================================
Function: IDLSigCheck_CheckSig()


Description:
    This function verfies the signature of the files based upon a .sig 
    file and listed files as specified in the sig file.

Prototype:
   int IDLSigCheck_CheckSig(IDLSigCheck *po, const char *pszSig, 
                           PFNPKGFILEREADER pfnFileReader, void *pv)


Parameters:
    po [in]: Pointer to the IDLSigCheck interface object.
    pszSig [in]: Name of the file to open. 
                 Must supply the name in terms of 'fs:' scheme
    pfnFileReader [in]: To be called to open a file for reading
                    The file name supplied will be prefixed with the folder
                    name where the sig file resides.
    pvOpener [in]: pfnFileReader context to be supplied back as first argument
    
Return Value:
    None

Comments:
    None

Side Effects:
    None
=======================================================================
*/



#endif
