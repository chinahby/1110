 

// AEE Services used by app
#include "AEEStdLib.h"        // AEE Stb Lib Services

#include "EditExtension.h"

//
// UMailCtl app can either be statically built into BREW or dynamically linked during run-time.
// If AEE_STATIC is defined, then UMailCtl app will be a static app.
// NOTE: Static apps can be built only by OEMs or Carriers. App developers can build dynamic apps only.

#ifdef AEE_STATIC

#include "aeemodgen.h"
#include "aeeappgen.h"

int EditExt_CreateInstance(AEECLSID ClsId, IShell * pIShell, IModule * po, void ** ppObj);

//===========================================================================
extern int EditExt_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,EditExt_CreateInstance,NULL));
}
#endif  //AEE_STATIC


/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
	that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time.

PROTOTYPE:
	int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the applet which is being loaded

	pIShell: [in]: Contains pointer to the IShell interface. 

	pIModule: pin]: Contains pointer to the IModule interface to the current module to which
	this app belongs

	ppObj: [out]: On return, *ppApplet must point to a valid IBase object. 
	If the classID	represnts an applet, then ppObj must point to a valid AEEApplet structure.Allocation
	of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the class/app creation was successful. 
  EFAILED: Error occurred while creating the class/app. In this case, the app/class will
  not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
extern int EditExt_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
extern int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
	*ppObj = NULL;

	switch(ClsId) {
	case AEECLSID_EDIT_EXTENSION:
		return EditExt_New(0, pIShell, po, ppObj);

	default:
		break;
	}

   return(EFAILED);
}


