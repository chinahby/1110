#ifndef __A1LITEMODGEN_H__
#define __A1LITEMODGEN_H__
/*===========================================================================

FILE:      A1LiteModGen.h
                 
 
SERVICES:  Header file for the AEEModule implementation for A1Lite

 
DESCRIPTION: This file contains definitions of data structures used in the sample
implementation of the IModule interface. 


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


   	   Copyright © 2000-2007 QUALCOMM Incorporated.
	                  All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/
#include "AEE.h"			// Standard AEE definitions
#include "AEEShell.h"	// AEE Shell Services
#include "AEEIEnv.h"


/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

//Address of IMODULE_CreateInstance() function
typedef int (*PFNMODCREATEINST)(AEECLSID, IShell*, IModule*, IEnv *, void**);

//Address of Freedata function for the module
typedef void (*PFNFREEMODDATA)(IModule*);


//Structure that implements the IModule interface
typedef struct _AEEMod                                     
{
	DECLARE_VTBL(IModule)	// Virtual Table with pointers to IModule functions
	
	uint32	      m_nRefs;			// Reference count for this module
   IShell *       m_pIShell;		// Pointer to IShell
   IEnv *         m_piEnv;       // Pointer to IEnv

	//Address of CreateInstance function of the module. This is needed for
	// static modules
	PFNMODCREATEINST	pfnModCrInst; 

	//Address of the function to free the module data. This is needed for static
	// modules that define their own data.
	PFNFREEMODDATA		pfnModFreeData;

} AEEMod;

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//===============================================================================
// FUNCTION DECLARATIONS 
//===============================================================================

#ifdef __cplusplus
extern "C" {
#endif

#ifndef  AEE_LOAD_DLL
#  if ((defined(AEE_SIMULATOR) || defined(WIN32)) && !defined(_AEEWIN)) || ( defined(_AEEWIN) && defined(_M_IX86))
#     define  AEE_LOAD_DLL
#  endif
#endif

// Prototype for the standard Module Load function. When a module is loaded,
// this is the first function that is invoked.
#ifdef AEE_LOAD_DLL

	// Need to prefix this function with __declspec(dllexport) so that this function
	// is exported from the DLL. 
 __declspec(dllexport) int  AEEMod_Load(IShell *ps, void * ph, IModule ** pMod);

#else

	int  AEEMod_Load(IShell *ps, void * ph, IModule ** pMod);

#endif   // AEE_LOAD_DLL

// Create instance function for dynamic modules
int A1LiteClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, IEnv *piEnv, void **ppobj);

//Module load function for static modules
int      AEEStaticMod_New(int16 nSize, IShell *pIShell, void * ph, IModule ** ppMod,PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF);


#ifdef __cplusplus
}
#endif

#endif  //__A1LITEMODGEN_H__


