
#ifndef __TEXT_EDITOR_H__
#define __TEXT_EDITOR_H__

#include "aeeshell.h"
#include "aeestdlib.h"
#include "aeecomdef.h"
#include "aeeeditextension.h"

#ifndef AEE_SIMULATOR
#include "OEMClassIDs.h"
#else
#include "EditExtension.bid"
#include "..\editapplet\EditApplet.bid"
#endif

///////////////////////////////////////////////
#define EDITEXT_MAX_TITLE_LEN	31
#define EDITEXT_DEF_MAX_CHARS	2048


// EditExt sturct definition.
typedef struct EditExt
{
	AEEVTBL(IEditExt) *pvt;
	
	int m_nRefs;

	IShell *m_pIShell;
	IModule *m_pIModule;
}EditExt;


/*===========================================================================================*/
int EditExt_New(int16 nSize, IShell *pIShell, IModule* pIModule, void **ppo);


#endif	// __TEXT_EDITOR_H__