/*=============================================================================

FILE: SwitchUSIM.c

SERVICES: Switch USIM card applets//开机选卡

=============================================================================*/


/*=============================================================================
                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who            what, where, why
----------   ---            ---------------------------------------------------
=============================================================================
2010 - 06 08    keagleyang     hx
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"         //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"  // Simulator Feature Definitions
#endif
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEHeap.h"
#include "AEEMenu.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
#include "appscommon_color.brh"
#include "AppComFunc.h"
#include "switchusim.brh"
#include "Appscommon.h"
#include "err.h"
/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/
#define  AEE_SWITCH_RES_FILE (AEE_RES_LANGDIR SWITCHUSIM_RES_FILE)
#define  MALLOC_AECHARS(nNumberChars)   MALLOC( nNumberChars * sizeof(AECHAR) )
#define  SIZEOF_AECHARS(nNumberChars)   ( nNumberChars * sizeof(AECHAR) )

#define  ADJUSTPENAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

/*===========================================================================

                    MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/
#define TEXTSIZE   256

typedef enum {
	USIM_ONE = 1,   //default图标
    USIM_TWO,    //focus图标
} ISFOCUS;


typedef struct _CSwitchUSIM {
    AEEApplet  a;
    AEERect    m_rc;          // Applet rectangle
    AEECLSID   m_cls;         // ClassID of the applet
    IStatic    *m_pIStatic;      // Static for displaying information
    IStatic    *m_pIStaticTwo;
	AECHAR     *m_szBuffer; 
	AEERect    m_rctext;          // Applet rectangle
	AECHAR     *m_szTWOBuffer;
    AEERect    m_rctwotext;       // 区域
    ISFOCUS    m_aState;          // select state
} CSwitchUSIM;

/*===========================================================================

                       STATIC/LOCAL FUNCTION DECLARATIONS

===========================================================================*/



int     SwitchUSIMMod_Load(IShell   *ps,
                            void     *pHelpers,
                            IModule **pMod);

static int     SwitchUSIMMod_CreateInstance(AEECLSID  ClsId,
                                      IShell   *pIShell,
                                      IModule  *po,
                                      void    **ppObj);


static boolean SwitchUSIM_InitAppData(CSwitchUSIM *pMe);
static void    SwitchUSIM_FreeAppData(CSwitchUSIM *pMe);

static boolean SwitchUSIM_HandleEvent(CSwitchUSIM *pme,
                                AEEEvent eCode,
                                uint16   wParam,
                                uint32   dwParam);
static void    SwitchUSIM_DrawScreen(CSwitchUSIM *pme);

/*===========================================================================

                    LOCAL/STATIC DATA

===========================================================================*/

/*=============================================================================
FUNCTION: SwitchUSIMMod_Load

DESCRIPTION:

PARAMETERS:
   *ps:
   *pHelpers:
   **pMod:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int SwitchUSIMMod_Load(IShell   *ps,
                 void     *pHelpers,
                 IModule **pMod)
{
    return AEEStaticMod_New(sizeof(AEEMod),
                            ps,
                            pHelpers,
                            pMod,
                            (PFNMODCREATEINST)SwitchUSIMMod_CreateInstance,
                            (PFNFREEMODDATA)NULL);
}


/*=============================================================================
FUNCTION: SwitchUSIMMod_CreateInstance

DESCRIPTION:

PARAMETERS:
   ClsId:
   *pIShell:
   *po:
   **ppObj:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

static int SwitchUSIMMod_CreateInstance(AEECLSID  ClsId,
                                  IShell   *pIShell,
                                  IModule  *po,
                                  void    **ppObj)

{
    *ppObj = NULL;

    if (!AEEApplet_New(sizeof(CSwitchUSIM),
                       ClsId,
                       pIShell,
                       po,
                       (IApplet**)ppObj,
                       (AEEHANDLER) SwitchUSIM_HandleEvent,
                       (PFNFREEAPPDATA) SwitchUSIM_FreeAppData)) {
       return ENOMEMORY;
    }

    ((CSwitchUSIM *)*ppObj)->m_cls = ClsId;

	if(AEE_SUCCESS != SwitchUSIM_InitAppData((CSwitchUSIM *)*ppObj))
	{
		SwitchUSIM_FreeAppData((CSwitchUSIM *)*ppObj);
		return AEE_EFAILED;
	}
    return AEE_SUCCESS;
}



static void SwitchUSIM_CloseApplet( void *pMe)
{
    CSwitchUSIM *pme = (CSwitchUSIM *)pMe;
    (void) ISHELL_CloseApplet(pme->a.m_pIShell , FALSE);
}

static boolean SwitchUSIM_HandleEvent(CSwitchUSIM *pme, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    if (pme == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_APP_START:
            SwitchUSIM_DrawScreen(pme);
            return TRUE;

        case EVT_APP_STOP:
            ERR("EVT_APP_STOP::::::::::::::::",0,0,0);
            SwitchUSIM_FreeAppData(pme);
            return TRUE;

        case EVT_APP_SUSPEND:
            return TRUE;

        case EVT_APP_RESUME:
            SwitchUSIM_DrawScreen(pme);
            return TRUE;
            
        case EVT_KEY:
            switch(wParam){
            case AVK_INFO:
            case AVK_SELECT:
                ISHELL_PostEvent(pme->a.m_pIShell, AEECLSID_CORE_APP, EVT_USER,0,0);
                SwitchUSIM_CloseApplet(pme);
                break;
            case AVK_UP:
            case AVK_DOWN:
                if(pme->m_aState == USIM_ONE)
                {
                    pme->m_aState = USIM_TWO;
                }
                else if(pme->m_aState == USIM_TWO)
                {
                    pme->m_aState = USIM_ONE;
                }
                ERR("SwitchUSIM_HandleEvent EVT_KEY:::%d",pme->m_aState,0,0);
                SwitchUSIM_DrawScreen(pme);
                break;
            default:
                break;
            }
            break;
			return TRUE;
    } 
    return FALSE;
}

/*=============================================================================
FUNCTION: SwitchUSIM_DrawScreen

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void SwitchUSIM_DrawScreen(CSwitchUSIM *pMe)
{
    if(pMe->m_aState == USIM_ONE)
    {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_SWITCH_RES_FILE,
                                  IDS_SIMONE_FOCUS,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256)); 
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_SWITCH_RES_FILE,
                                  IDS_SIMTWO_DEFAULT,
                                  pMe->m_szTWOBuffer,
                                  SIZEOF_AECHARS(256)); 
    }
    else if(pMe->m_aState == USIM_TWO)
    {
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_SWITCH_RES_FILE,
                                  IDS_SIMONE_DEFAULT,
                                  pMe->m_szBuffer,
                                  SIZEOF_AECHARS(256)); 
        (void) ISHELL_LoadResString(pMe->a.m_pIShell,
                                  AEE_SWITCH_RES_FILE,
                                  IDS_SIMTWO_FOCUS,
                                  pMe->m_szTWOBuffer,
                                  SIZEOF_AECHARS(256)); 
    }
    
	IDISPLAY_FillRect( pMe->a.m_pIDisplay, &pMe->m_rc, RGB_BLACK);

	ISTATIC_SetRect(pMe->m_pIStatic, &pMe->m_rctext);
	ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTEXT|ST_NOSCROLL);
	(void) ISTATIC_SetText(pMe->m_pIStatic,
                          NULL,
                          pMe->m_szBuffer,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);
   (void) ISTATIC_Redraw(pMe->m_pIStatic);

    ISTATIC_SetRect(pMe->m_pIStaticTwo, &pMe->m_rctwotext);
	ISTATIC_SetProperties(pMe->m_pIStaticTwo, ST_CENTERTEXT|ST_NOSCROLL);
	(void) ISTATIC_SetText(pMe->m_pIStaticTwo,
                          NULL,
                          pMe->m_szTWOBuffer,
                          AEE_FONT_NORMAL,
                          AEE_FONT_NORMAL);
   (void) ISTATIC_Redraw(pMe->m_pIStaticTwo);
   
   IDISPLAY_Update( pMe->a.m_pIDisplay);
}

static boolean SwitchUSIM_InitAppData(CSwitchUSIM *pMe)
{
   IShell        *pShell = pMe->a.m_pIShell;
   AEEDeviceInfo  di;
   
   // Get screen pixel count.
   ISHELL_GetDeviceInfo(pShell, &di);
   
   pMe->m_szBuffer = NULL;
   pMe->m_szTWOBuffer = NULL;
   pMe->m_rctext.x = 10;
   pMe->m_rctext.y = 31;
   pMe->m_rctext.dx = di.cxScreen-10*2;
   pMe->m_rctext.dy = 20;

   pMe->m_rctwotext.x = 10;
   pMe->m_rctwotext.y = 77;
   pMe->m_rctwotext.dx = di.cxScreen-10*2;
   pMe->m_rctwotext.dy = 20;
   // Stash the screen rect
   SETAEERECT(&pMe->m_rc, 0, 0, di.cxScreen, di.cyScreen);
   pMe->m_aState = USIM_ONE;

   pMe->m_pIStatic	= NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStatic)) {
      return AEE_EFAILED;
   }

   pMe->m_pIStaticTwo	= NULL;
   if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->a.m_pIShell,
                                            AEECLSID_STATIC,
                                            (void **)&pMe->m_pIStaticTwo)) {
      return AEE_EFAILED;
   }

   pMe->m_szBuffer = MALLOC_AECHARS(TEXTSIZE);
   if (pMe->m_szBuffer == NULL) 
   {
      return AEE_EFAILED;
   }
   pMe->m_szTWOBuffer = MALLOC_AECHARS(TEXTSIZE);
   if(pMe->m_szTWOBuffer == NULL)
   {
      return AEE_EFAILED;   
   }
   return AEE_SUCCESS;
}
static void SwitchUSIM_FreeAppData(CSwitchUSIM *pMe)
{

   if (pMe->m_pIStatic)
   {
      ISTATIC_Release(pMe->m_pIStatic);
      pMe->m_pIStatic = NULL;
   }
   if (pMe->m_pIStaticTwo)
   {
      ISTATIC_Release(pMe->m_pIStaticTwo);
      pMe->m_pIStaticTwo = NULL;
   }

   if (pMe->m_szBuffer) 
   {
      FREE(pMe->m_szBuffer);
      pMe->m_szBuffer = NULL;
   }
   if (pMe->m_szTWOBuffer) 
   {
      FREE(pMe->m_szTWOBuffer);
      pMe->m_szTWOBuffer = NULL;
   }
}

