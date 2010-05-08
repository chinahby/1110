#ifndef __AEEAPPGEN_H__
#define __AEEAPPGEN_H__
/*===========================================================================

FILE:      AEEAppGen.h
                 
 
SERVICES:  Header file for the sample AEEApplet implementation

 
DESCRIPTION: This file contains definitions of data structures used in the sample
implementation of the IApplet interface. 


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
#include "AEEShell.h"	// AEE Shell services
#include "AEEModGen.h"  // AEEModGen.h - REQUIRED!

// Function to free app data. This function will be provided by the
// app so that it can be invoked when the reference count of the app
// reaches zero. It gives the app an opportunity to free all the allocated
// data.
typedef void (*PFNFREEAPPDATA)(IApplet*);

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
//Structure to hold Applet infprmation
typedef struct _AEEApplet AEEApplet;

struct _AEEApplet
{
   // 
   // NOTE:  These 3 fields must be declared in this order!
   //
   DECLARE_VTBL(IApplet)  
   AEECLSID       clsID;
   
   uint32         m_nRefs;			// Applet reference counter
   IShell    *    m_pIShell;	   // pointer to IShell
   IModule   *    m_pIModule;		// pointer to IModule
	IDisplay  *    m_pIDisplay;	// pointer to IDisplay

	//Pointer to Handle Event Function
   AEEHANDLER     pAppHandleEvent;

	// Pointer to FreeAppData function. This will be invoked when the
   // reference count of the App goes to zero. This function is supplied
   // by the app developer.
   // NOTE: Apps should NOT directly call their FreeAppData function.
   PFNFREEAPPDATA pFreeAppData;
};

//-------------------------------------------------------------------
// Global Constant Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS 
//===============================================================================

//This function is invoked from within the applications AEEClsCreateInstance() to
// create a new instance of the AEEApplet structure for this applet.

#ifdef __cplusplus
extern "C" {
#endif

// Function that shall be invoked by the App to create an instance of the
// Applet.
boolean AEEApplet_New(  int16 nSize,
							   AEECLSID clsID,
							   IShell * pIShell, 
							   IModule * pIModule, 
							   IApplet **ppobj, 
							   AEEHANDLER pAppHandleEvent,
							   PFNFREEAPPDATA pFreeAppData
							);

#ifdef __cplusplus
}
#endif

#endif  //__AEEAPPGEN_H__


