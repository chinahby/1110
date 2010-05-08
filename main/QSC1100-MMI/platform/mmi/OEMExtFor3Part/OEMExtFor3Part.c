
#include "aeeshell.h"
#include "aeestdlib.h"
#include "aeecomdef.h"
#include "aeemodgen.h"
#include "aeeappgen.h"


#include "AEEBacklight.h"
#include "AEEOEMExtFor3Part.h"

#include "OEMClassIDs.h"

#include "AEEAnnunciator.h"
#include "AEEAlert.h"
#include "oemcfgi.h"
#include "AEEConfig.h"

#include "coreapp_priv.h"
#include "wmsapp.h"
#include "CallApp.h"
#include "contapp.h"
#include "musicplayer.h"
#include "mediagallery.h"

#include "sndi.h"
#include "aeesound.h"


#ifdef FEATURE_SUPPORT_WAPMMS_APP
#include "OemMMSExt.c"
#endif


#define TIME_MS_VIBRATE                  2000


//////////////////////////////////////////////////////////////////////////

// OEMExtFor3Part sturct definition.
typedef struct OEMExtFor3Part
{
	AEEVTBL(IOEMExtFor3Part) *pvt;
	
	int m_nRefs;

	IShell *m_pIShell;
	IModule *m_pIModule;
}OEMExtFor3Part;

// New a OEMExtFor3Part class.
int OEMExtFor3Part_New(int16 nSize, IShell *pIShell, IModule* pIModule, void **ppo);

//////////////////////////////////////////////////////////////////////////

/*
	oem extension for three part class can either be statically built into BREW 
or dynamically linked during run-time.
	If AEE_STATIC is defined, then the class will be a static class.

NOTE: 
	Static apps can be built only by OEMs or Carriers. App developers can build dynamic apps only.
*/

#ifdef AEE_STATIC

// Class entry.
int OEMExtFor3Part_CreateInstance(AEECLSID ClsId, IShell * pIShell, IModule * po, void ** ppObj);

//static const AEEAppInfo  gaiOEMExtFor3PartExtension = {0, 0, 0, 0, 0, 0, 0, 0};
static const AEECLSID gaiOEMExtFor3PartClasses[] = {AEECLSID_OEMEXT_FOR3PART, AEECLSID_OEMMMSEXT, 0	};

// Module entry.
extern int OEMExtFor3Part_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,OEMExtFor3Part_CreateInstance,NULL));
}

// Returns static module information
PFNMODENTRY OEMExtFor3Part_GetModInfo(IShell * ps, AEECLSID ** ppClasses, AEEAppInfo ** pApps, 
											uint16 * pnApps, uint16 * pwMinPriv)
{
   *pApps = NULL;
   *pnApps = 0;
   *ppClasses = (AEECLSID*)gaiOEMExtFor3PartClasses;
   return((PFNMODENTRY)OEMExtFor3Part_Load);
}


#endif  //AEE_STATIC

/*===========================================================================

FUNCTION: AEEClsCreateInstance

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
extern int OEMExtFor3Part_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
extern int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
	*ppObj = NULL;

	switch(ClsId) {
	case AEECLSID_OEMEXT_FOR3PART:
		return OEMExtFor3Part_New(0, pIShell, po, ppObj);

#ifdef FEATURE_SUPPORT_WAPMMS_APP
       case AEECLSID_OEMMMSEXT:
            return IMMSExt_New((IMMSExt **)ppObj, pIShell, po);            
#endif

	default:
		break;
	}

   return(EFAILED);
}

//////////////////////////////////////////////////////////////////////////

/*===========================================================================================*/
uint32 IOEMExtFor3Part_AddRef(IOEMExtFor3Part *pi)
{
	OEMExtFor3Part *pme = (OEMExtFor3Part *)pi;
	
	return ++pme->m_nRefs;
}

/*===========================================================================================*/
uint32 IOEMExtFor3Part_Release(IOEMExtFor3Part *pi)
{
	OEMExtFor3Part *pme = (OEMExtFor3Part *)pi;	

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
void IOEMExtFor3Part_ShowUnReadPushIcon(IOEMExtFor3Part *pi, boolean bShowOrHide)
{
#ifdef AEE_STATIC
    OEMExtFor3Part *pme = (OEMExtFor3Part *)pi;
    IAnnunciator *pIAnnunciator;

    if(SUCCESS == ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_ANNUNCIATOR, (void **)&pIAnnunciator))
    {
        IANNUNCIATOR_SetField(pIAnnunciator, 
                                ANNUN_FIELD_SPORTS, 
                                bShowOrHide ? ANNUN_STATE_PUSH_ON : ANNUN_STATE_PUSH_OFF);
        IANNUNCIATOR_Release(pIAnnunciator); 
    }

#endif	
}

/*===========================================================================================*/
void IOEMExtFor3Part_PlayPushRing(IOEMExtFor3Part *pi)
{
	OEMExtFor3Part *pme = (OEMExtFor3Part *)pi;
	int nErr;
	IConfig *pIConfig = NULL;
	IALERT *pIAlert = NULL;
	IBacklight *pIBacklight = NULL;

	byte btActiveProfile;
       ringID  SmsRingerID[PROFILENUMBER]={0};

	nErr = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_BACKLIGHT, (void **)&pIBacklight);
	if(SUCCESS != nErr)	{
		DBGPRINTF("Fail to create IBacklight\n");
		return;
	}

    IBACKLIGHT_Enable(pIBacklight);
    IBACKLIGHT_Release(pIBacklight);

#ifdef FEATURE_APP_MUSICPLAYER
    if(!IsMp3PlayerStatusNone())
    {
		ISound *pISound;
		nErr = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_SOUND, (void**)&pISound);
		if(SUCCESS == nErr)	
		{
			ISOUND_Vibrate(pISound, TIME_MS_VIBRATE);
			ISOUND_Release(pISound);
		}
        return;
    }
#endif

	nErr = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_CONFIG, (void **)&pIConfig);
	if(SUCCESS != nErr)	{
		DBGPRINTF("Fail to create IConfig\n");
		goto LFail;
	}

	nErr = ISHELL_CreateInstance(pme->m_pIShell, AEECLSID_ALERT, (void **)&pIAlert);
	if(SUCCESS != nErr)	{
		DBGPRINTF("Fail to create IAlert\n");
		goto LFail;
	}

	ICONFIG_GetItem(pIConfig, CFGI_PROFILE_CUR_NUMBER, &btActiveProfile, sizeof(btActiveProfile));

	ICONFIG_GetItem(pIConfig, CFGI_PROFILE_SMS_RINGER_ID, (void *)SmsRingerID, sizeof(SmsRingerID));	

	if (btActiveProfile < PROFILENUMBER)
    {
        if(SmsRingerID[btActiveProfile].ringType == OEMNV_MP3_RINGER)
        {
            if (SUCCESS != IALERT_StartMp3Alert(pIAlert, SmsRingerID[btActiveProfile].szMusicname, ALERT_SMS_SND))
                IALERT_StartSMSAlert(pIAlert, SmsRingerID[btActiveProfile].midID);
        }
        else
        {
            IALERT_StartSMSAlert(pIAlert, SmsRingerID[btActiveProfile].midID);
        }
    }

LFail:
	if(pIConfig)
		ICONFIG_Release(pIConfig);	

	if(pIAlert)
		IALERT_Release(pIAlert);
}

/*===========================================================================================*/
boolean IOEMExtFor3Part_IsInIdle(IOEMExtFor3Part *pi)
{
    OEMExtFor3Part *pme = (OEMExtFor3Part *)pi;
#ifdef AEE_STATIC
    return (CoreApp_IsIdle() ? 0 : 1);
#else
    return (AEECLSID_COREAPP==ISHELL_ActiveApplet(pme->pIShell) ? 0 : 1);
#endif
}

/*===========================================================================================*/
int OEMExtFor3Part_New(int16 nSize, IShell *pIShell, IModule* pIModule, void **ppo)
{
	OEMExtFor3Part *pme = NULL;
	AEEVTBL(IOEMExtFor3Part) *pvt;
		
	if(!ppo || !pIShell || !pIModule )
		return EFAILED;

	*ppo = NULL;

	// Allocate memory for the ExtensionCls object
	if( nSize < sizeof(OEMExtFor3Part) )
		nSize += sizeof(OEMExtFor3Part);
	
	// Allocate the module's struct and initialize it. Note that the 
	// modules and apps must not have any static data. 
	// Hence, we need to allocate the vtbl as well.
	if( (pme = (OEMExtFor3Part*)MALLOC(nSize + sizeof(AEEVTBL(IOEMExtFor3Part)))) == NULL )
		return ENOMEMORY; 
	MEMSET(pme, 0, sizeof(OEMExtFor3Part));
		
	//Initialize individual entries in the VTBL
	pvt = (AEEVTBL(IOEMExtFor3Part) *)((byte *)pme + nSize);
#define VTFUNC(name) pvt->name = IOEMExtFor3Part_##name;
	VTFUNC(AddRef);
	VTFUNC(Release);
	VTFUNC(ShowUnReadPushIcon);
	VTFUNC(PlayPushRing);
	VTFUNC(IsInIdle);
#undef VTFUNC
	// initialize the vtable
	AEEINITVTBL(pme, IOEMExtFor3Part, *pvt);

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
