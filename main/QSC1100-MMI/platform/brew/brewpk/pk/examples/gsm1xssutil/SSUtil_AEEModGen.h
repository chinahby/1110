#ifndef __AEEMODGEN_H__
#define __AEEMODGEN_H__
/*===========================================================================

FILE:      SSUtil_AEEModGen.h // singleton-change
                 
 
SERVICES:  Header file for the sample AEEModule implementation

 
DESCRIPTION: This file contains definitions of data structures used in the sample
implementation of the IModule interface. 


PUBLIC CLASSES:  Not Applicable


INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable


   	   Copyright © 2000-2002 QUALCOMM Incorporated.
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


/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

// singleton-change - Type of the singleton member
#ifndef HACK_A_SHAQ
typedef struct _ISSUtilCls ISSUtilCls;
#define HACK_A_SHAQ
#endif

//Address of IMODULE_CreateInstance() function
typedef int (*PFNMODCREATEINST)(AEECLSID, IShell*, IModule*, void**);

//Address of Freedata function for the module
typedef void (*PFNFREEMODDATA)(IModule*);


//Structure that implements the IModule interface
typedef struct _AEEMod                                     
{
	DECLARE_VTBL(IModule)	// Virtual Table with pointers to IModule functions
	
	uint32	      m_nRefs;			// Reference count for this module
   IShell *       m_pIShell;		// Pointer to IShell

	//Address of CreateInstance function of the module. This is needed for
	// static modules
	PFNMODCREATEINST	pfnModCrInst; 

	//Address of the function to free the module data. This is needed for static
	// modules that define their own data.
	PFNFREEMODDATA		pfnModFreeData;

    // singleton-change - The singleton member
    ISSUtilCls* pISSUtilCls;

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


// Prototype for the standard Module Load function. When a module is loaded,
// this is the first function that is invoked.
#ifdef AEE_SIMULATOR   // Running app on Windows

	// Need to prefix this function with __declspec(dllexport) so that this function
	// is exported from the DLL. 
 __declspec(dllexport) int  AEEMod_Load(IShell *ps, void * ph, IModule ** pMod);

#else

#ifndef BREW_STATIC_APP
    int AEEMod_Load(IShell *pIShell, void *ph, IModule **ppMod);
#endif

#endif   //AEE_SIMULATOR

// Create instance function for dynamic modules
int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, void **ppobj);

//Module load function for static modules
int      SSUtil_AEEStaticMod_New(int16 nSize, IShell *pIShell, void * ph, IModule ** ppMod,PFNMODCREATEINST pfnMC,PFNFREEMODDATA pfnMF);


#ifdef __cplusplus
}
#endif

#endif  //__AEEMODGEN_H__


