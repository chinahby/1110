/*=================================================================================
FILE:          qlocales_mod.c

SERVICES:      ILocale extension implememtation for Brazilian Portuguese

PUBLIC CLASSES:ILocale

DESCRIPTION:   This file describes an interface that provides language
               dependent string formatting for a specific language. The
               intent is that there be one of these objects for each
               language your device supports.  Addition implementations
               may be added later to support additional languages.

                Copyright © 1999-2007 QUALCOMM Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEEStdLib.h"
#include "AEEShell.h"           /* Shell interface definitions */

#include "AEECLSID_LOCALEAR.BID"
#include "AEECLSID_LOCALEBG.BID"
#include "AEECLSID_LOCALECA.BID"
#include "AEECLSID_LOCALECS.BID"
#include "AEECLSID_LOCALEDA.BID"
#include "AEECLSID_LOCALEDEDE.BID"
#include "AEECLSID_LOCALEEL.BID"
#include "AEECLSID_LOCALEENGB.BID"
#include "AEECLSID_LOCALEENUS.BID"
#include "AEECLSID_LOCALEES.BID"
#include "AEECLSID_LOCALEESMX.BID"
#include "AEECLSID_LOCALEFA.BID"
#include "AEECLSID_LOCALEFI.BID"
#include "AEECLSID_LOCALEFRFR.BID"
#include "AEECLSID_LOCALEHE.BID"
#include "AEECLSID_LOCALEHI.BID"
#include "AEECLSID_LOCALEHR.BID"
#include "AEECLSID_LOCALEHU.BID"
#include "AEECLSID_LOCALEID.BID"
#include "AEECLSID_LOCALEIT.BID"
#include "AEECLSID_LOCALELT.BID"
#include "AEECLSID_LOCALELV.BID"
#include "AEECLSID_LOCALEML.BID"
#include "AEECLSID_LOCALEMS.BID"
#include "AEECLSID_LOCALENL.BID"
#include "AEECLSID_LOCALENO.BID"
#include "AEECLSID_LOCALEPL.BID"
#include "AEECLSID_LOCALEPT.BID"
#include "AEECLSID_LOCALEPTBR.BID"
#include "AEECLSID_LOCALERO.BID"
#include "AEECLSID_LOCALERU.BID"
#include "AEECLSID_LOCALESK.BID"
#include "AEECLSID_LOCALESL.BID"
#include "AEECLSID_LOCALESR.BID"
#include "AEECLSID_LOCALESV.BID"
#include "AEECLSID_LOCALETH.BID"
#include "AEECLSID_LOCALETL.BID"
#include "AEECLSID_LOCALETR.BID"
#include "AEECLSID_LOCALEUK.BID"
#include "AEECLSID_LOCALEUR.BID"
#include "AEECLSID_LOCALEVI.BID"
#include "AEECLSID_LOCALEZHCN.BID"
#include "AEECLSID_LOCALEZHTW.BID"

extern int LocaleAR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleBG_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleCA_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleCS_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleDA_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleDEDE_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleEL_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleENGB_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleENUS_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleES_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleESMX_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleFA_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleFI_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleFRFR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleHE_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleHI_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleHR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleHU_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleID_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleIT_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleLT_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleLV_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleML_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleMS_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleNL_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleNO_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocalePL_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocalePT_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocalePTBR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleRO_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleRU_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleSK_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleSL_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleSR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleSV_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleTH_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleTL_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleTR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleUK_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleUR_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleVI_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleZHCN_CreateInstance(IModule *, IShell *, AEECLSID, void **);
extern int LocaleZHTW_CreateInstance(IModule *, IShell *, AEECLSID, void **);


/*===========================================================================
  || xModule 
  ===========================================================================*/
int xModule_CreateInstance(IModule *piModule, IShell *piShell, AEECLSID clsid, void **ppo)
{
	switch (clsid)
	{
		case AEECLSID_LOCALEAR:
			return LocaleAR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEBG:
         return LocaleBG_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALECA:
			return LocaleCA_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALECS:
			return LocaleCS_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEDA:
         return LocaleDA_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEDEDE:
			return LocaleDEDE_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEEL:
         return LocaleEL_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEENGB:
         return LocaleENGB_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEENUS:
         return LocaleENUS_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEES:
			return LocaleES_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEESMX:
			return LocaleESMX_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEFA:
         return LocaleFA_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEFI:
			return LocaleFI_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEFRFR:
			return LocaleFRFR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEHE:
         return LocaleHE_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEHI:
			return LocaleHI_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEHR:
			return LocaleHR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEHU:
         return LocaleHU_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEID:
			return LocaleID_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEIT:
         return LocaleIT_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALELT:
			return LocaleLT_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALELV:
			return LocaleLV_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEML:
         return LocaleML_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEMS:
			return LocaleMS_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALENL:
			return LocaleNL_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALENO:
         return LocaleNO_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEPL:
			return LocalePL_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEPT:
			return LocalePT_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEPTBR:
         return LocalePTBR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALERO:
			return LocaleRO_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALERU:
			return LocaleRU_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALESK:
         return LocaleSK_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALESL:
			return LocaleSL_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALESR:
			return LocaleSR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALESV:
         return LocaleSV_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALETH:
			return LocaleTH_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALETL:
			return LocaleTL_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALETR:
         return LocaleTR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEUK:
			return LocaleUK_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEUR:
			return LocaleUR_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      case AEECLSID_LOCALEVI:
         return LocaleVI_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEZHCN:
			return LocaleZHCN_CreateInstance(piModule, piShell, clsid, (void**)ppo);
		case AEECLSID_LOCALEZHTW:
			return LocaleZHTW_CreateInstance(piModule, piShell, clsid, (void**)ppo);
      default:
		   break;
   }
   return (ECLASSNOTSUPPORT);
}

/*=============================================================================
Function: QLocales_Load

Description:This is the module load function. It is invoked when the module is being
   loaded. 

Prototype:
  int QLocales_Load(IShell *piShell, void * pHelpers, IModule ** piModule)
   

Parameters:
  piShell:
  pHelpers:
  piModule:

Return Value:
   SUCCESS: If successful
   Error code: IF failed

Comments:  None

Side Effects: None
=============================================================================*/
#if defined(AEE_STATIC)
int QLocales_Load(IShell *piShell, void * pHelpers, IModule ** piModule)
{
 return(AEEStaticMod_New((int16)(sizeof(AEEMod)),
							piShell,
							pHelpers,
							piModule,
							(PFNMODCREATEINST)QLocales_CreateInstance,
							(PFNFREEMODDATA)NULL));
}
#endif // AEE_STATIC
