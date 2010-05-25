/*======================================================

                    IMPORTANT NOTICE

The contents of this file are provided under legal agreement
between QUALCOMM Incorporated and selected licensed carrier
and handset OEM partners. Under the terms of that agreement, 
this file MUST NOT be distributed in full or in part to anyone
not specifically authorized by QUALCOMM Incorporated.

FILE:  ViewApp.c

SERVICES:  Default/Fallback Content Viewer app

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
	List the class(es) and static functions included in this file

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2000-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEShell.h"

#include "AEEImage.h"
#include "AEEImageCtl.h"
#include "AEEAppGen.h"
#include "AEEModGen.h"
#include "AEEStdLib.h"
#include "AEEMenu.h"
#include "AEEControls_res.h"
#include "AEEFile.h"

#define IDC_DONE  (100)

typedef struct _ViewApp
{
   AEEApplet         a;
   IImageCtl   *     m_pImage;
   IMenuCtl    *     m_pMenu;
} ViewApp;

static int        ViewApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
static boolean    ViewApp_HandleEvent(ViewApp * pme, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void       ViewApp_Free(ViewApp * pme);
static boolean    ViewApp_Show(ViewApp * pme, AEEAppStart * ps);
static AECHAR *   ViewApp_GetTitle(ViewApp * pme, const char * pszFile);

//===========================================================================
//
// Called by BREW to initialize the static module.
//
//===========================================================================
int ViewApp_Load(IShell *ps, void * pHelpers, IModule ** pMod)
{
   return(AEEStaticMod_New((int16)(sizeof(AEEMod)),ps,pHelpers,pMod,(PFNMODCREATEINST)ViewApp_CreateInstance,(PFNFREEMODDATA)NULL));
}

//===========================================================================
//
// Called by AEEModGen when the applet has been requested by BREW
//
//===========================================================================
static int ViewApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
{
   ViewApp *      pme = NULL;

   *ppObj = NULL;

   if(!AEEApplet_New(sizeof(ViewApp), ClsId, pIShell,po,(IApplet**)ppObj,(AEEHANDLER)ViewApp_HandleEvent,(PFNFREEAPPDATA)ViewApp_Free))
      return(ENOMEMORY);

   pme = (ViewApp *) (*ppObj);

   if(pme)
      ISHELL_CreateInstance(pIShell, AEECLSID_SOFTKEYCTL, (void **)(&pme->m_pMenu));

   return(AEE_SUCCESS);
}

/*===========================================================================


===========================================================================*/
static void ViewApp_Free(ViewApp * pme)
{
   if(pme->m_pMenu){
      IMENUCTL_Release(pme->m_pMenu);
      pme->m_pMenu = NULL;
   }

   if(pme->m_pImage){
      IIMAGECTL_Release(pme->m_pImage);
      pme->m_pImage = NULL;
   }
}

   
/*===========================================================================


===========================================================================*/
static boolean ViewApp_HandleEvent(ViewApp * pme, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
   switch(evt){
      case EVT_COMMAND:
         if(wParam == IDC_DONE)
            ISHELL_CloseApplet(pme->a.m_pIShell,FALSE);
         return(TRUE);

      case EVT_APP_START:
         {
         AEEAppStart *  ps = (AEEAppStart *)dwParam;

         if(!ps || !ps->pszArgs)
            return(FALSE);

         return(ViewApp_Show(pme,ps));
         }
      default:
         if(pme->m_pMenu && IMENUCTL_HandleEvent(pme->m_pMenu,evt,wParam,dwParam))
            return(TRUE);

         if(pme->m_pImage)
            return(IIMAGECTL_HandleEvent(pme->m_pImage,evt,wParam,dwParam));
   }
            
   return(FALSE);
}

/*===========================================================================


===========================================================================*/
static AECHAR * ViewApp_GetTitle(ViewApp * pme, const char * pszFile)
{
   AEEFileInfoEx  fi;
   IFileMgr *     pfm;
   AECHAR *       psz,*pszDot;
   char *         pszName;
   

   ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_FILEMGR, (void **)(&pfm));
   if(!pfm)
      return(NULL);

   psz = (AECHAR *)MALLOC(32 * sizeof(AECHAR));
   if(psz){
      MEMSET(&fi,0,sizeof(fi));
      fi.nStructSize = sizeof(fi);
      fi.pszDescription = psz;
      fi.nDescriptionSize = 32 * sizeof(AECHAR);
      IFILEMGR_GetInfoEx(pfm,pszFile,&fi);
      if(!*psz){
         pszName = STRRCHR(pszFile,DIRECTORY_CHAR);
         if(pszName){
            STRTOWSTR(pszName + 1,psz,32 * sizeof(AECHAR));
            pszDot = WSTRRCHR(psz,(AECHAR)'.');
            if(pszDot)
               *pszDot = 0;
         }
         else{
            FREE(psz);
            psz = NULL;
         }
      }
   }

   IFILEMGR_Release(pfm);
   return(psz);
}   

/*===========================================================================


===========================================================================*/
static boolean ViewApp_Show(ViewApp * pme, AEEAppStart * ps)
{
   IImage *       pi;
   boolean        bRet = FALSE;
   AECHAR *       pszName;
   int            cyFont;
   AEERect        rc;


   // Load the image.

   pi = ISHELL_LoadImage(pme->a.m_pIShell, ps->pszArgs);
   if(pi){

   // Create the ImageCtl container...

      ISHELL_CreateInstance(pme->a.m_pIShell, AEECLSID_IMAGECTL,(void **)(&pme->m_pImage));
      if(pme->m_pImage){
         pszName = ViewApp_GetTitle(pme,ps->pszArgs);
         if(pszName){
            IDISPLAY_DrawText(pme->a.m_pIDisplay, AEE_FONT_BOLD, pszName, -1,0,0,NULL,IDF_ALIGN_CENTER);
            FREE(pszName);
            cyFont = IDISPLAY_GetFontMetrics(pme->a.m_pIDisplay,AEE_FONT_BOLD,NULL,NULL);
            ps->rc.y += cyFont;
            ps->rc.dy -= cyFont;
         }

   // Set the sofkey menu...

         if(pme->m_pMenu){
            IMENUCTL_AddItem(pme->m_pMenu,AEECONTROLS_RES_FILE,AEE_IDS_HV_DONE,IDC_DONE,NULL,0);
            IMENUCTL_SetActive(pme->m_pMenu,TRUE);
            IMENUCTL_GetRect(pme->m_pMenu,&rc);
            ps->rc.dy -= rc.dy;
         }

         IIMAGECTL_SetImage(pme->m_pImage,pi);
         IIMAGECTL_SetProperties(pme->m_pImage,CP_BORDER);
         IIMAGECTL_SetRect(pme->m_pImage,&ps->rc);
         IIMAGECTL_SetActive(pme->m_pImage,TRUE);
         bRet = TRUE;
      }
      IIMAGE_Release(pi);
   }
   return(bRet);
}

