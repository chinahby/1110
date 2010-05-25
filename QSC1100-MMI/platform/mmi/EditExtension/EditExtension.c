

#include "EditExtension.h"


/*===========================================================================================*/
uint32 IEditExt_AddRef(IEditExt *pi)
{
	EditExt *pme = (EditExt *)pi;
	
	return ++pme->m_nRefs;
}

/*===========================================================================================*/
uint32 IEditExt_Release(IEditExt *pi)
{
	EditExt *pme = (EditExt *)pi;	
	
	pme->m_nRefs--;
	if(pme->m_nRefs == 0)	{
		ISHELL_Release(pme->m_pIShell);
		IMODULE_Release(pme->m_pIModule);

		FREE(pme);
		
		return 0;
	}
	
	return pme->m_nRefs;
}

/*===========================================================================================*/
int IEditExt_StartEditor(IEditExt *pi, EditExtStartParam *param)
{
	EditExt *pme = (EditExt *)pi;
	EditExtStartParam *my_param;
	char szCommand[16];
	
	if(!param || !param->inputBuffer)
		return EBADPARM;
	
	my_param = (EditExtStartParam *)MALLOC(sizeof(EditExtStartParam));
	if(NULL == my_param)
		return ENOMEMORY;

	MEMCPY(my_param, param, sizeof(EditExtStartParam));

	my_param->maxLength = (param->maxLength > 0) ? param->maxLength : EDITEXT_DEF_MAX_CHARS;
	
	my_param->preFilledString = NULL;
	if(param->preFilledString)
		my_param->preFilledString = WSTRDUP(param->preFilledString);	
	
	my_param->titleString = NULL;
	if(param->titleString)	
		my_param->titleString = WSTRDUP(param->titleString);
	
	SPRINTF(szCommand, "%p", my_param);
	return ISHELL_StartAppletArgs(pme->m_pIShell, AEECLSID_TEXT_APPLET, szCommand);
}


/*===========================================================================================*/
int EditExt_New(int16 nSize, IShell *pIShell, IModule* pIModule, void **ppo)
{
	EditExt *pme = NULL;
	AEEVTBL(IEditExt) *pvt;
	
	if(!ppo || !pIShell || !pIModule )
		return EFAILED;
	
	*ppo = NULL;
	
	// Allocate memory for the ExtensionCls object
	if( nSize < sizeof(EditExt) )
		nSize += sizeof(EditExt);
	
	// Allocate the module's struct and initialize it. Note that the 
	// modules and apps must not have any static data. 
	// Hence, we need to allocate the vtbl as well.
	if( (pme = (EditExt*)MALLOC(nSize + sizeof(AEEVTBL(IEditExt)))) == NULL )
		return ENOMEMORY; 
	MEMSET(pme, 0, sizeof(EditExt));
	
	//Initialize individual entries in the VTBL
	pvt = (AEEVTBL(IEditExt) *)((byte *)pme + nSize);
#define VTFUNC(name) pvt->name = IEditExt_##name;
	VTFUNC(AddRef);
	VTFUNC(Release);
	VTFUNC(StartEditor);
#undef VTFUNC
	// initialize the vtable
	AEEINITVTBL(pme, IEditExt, *pvt);
	
	// initialize the data members
	pme->m_nRefs = 1;
	pme->m_pIShell = pIShell;
	pme->m_pIModule = pIModule;
	
	// Add References and get IDisplay
	ISHELL_AddRef(pIShell);
	IMODULE_AddRef(pIModule);
	
	// Set the pointer in the parameter
	*ppo = (void*)pme;
	return AEE_SUCCESS;	
}