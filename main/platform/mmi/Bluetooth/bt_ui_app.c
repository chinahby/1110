


#include "bt_ui_ht_common.h"
#include "bcmapp_dm.h"
#include "bcmapp_ag.h"
#if defined(AEE_STATIC)
#include "OEMClassIDs.h"
//#include "btapp.brh"
#else
#include "bt_ui_app.bid"
#endif
#ifndef AEE_SIMULATOR
#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
#include "MediaGalleryApp.h"
#endif
#endif

static bt_ui_start *ui_start = NULL;
//////////////////////////////////////////////////////////////////////////
#define UI_SPACE    1
#ifdef AEE_SIMULATOR
IShell * g_pIShell = NULL;
#endif
//////////////////////////////////////////////////////////////////////////
tBtuiApp *g_pBtuiApp = NULL;

#ifdef AEE_STATIC

//////////////////////////////////////////////////////////////////////////
int tBtuiApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
int tBtuiApp_Load(IShell *ps, void *pHelpers, IModule **pMod);
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
#endif
#ifdef AEE_SIMULATOR
extern BootEntry(void);
#endif

#if defined(AEE_STATIC)
extern BTA_HCILP_Enable(BOOLEAN on);
int tBtuiApp_Load(IShell *ps, void *pHelpers, IModule **pMod)
{
  return AEEStaticMod_New(sizeof(AEEMod),
                          ps,
                          pHelpers,
                          pMod,
                          (PFNMODCREATEINST) tBtuiApp_CreateInstance,
                          (PFNFREEMODDATA) NULL);
}
//#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
//static void bcmapp_dm_get_sd_card_state(tBtuiApp *pMe);
//#endif
#endif


/*===========================================================================

FUNCTION tBtuiApp_InitAppData

===========================================================================*/
static boolean tBtuiApp_InitAppData(IApplet* pi)
{
    tBtuiApp * pMe = (tBtuiApp*)pi;
    AEEDeviceInfo sDevInfo; 
#ifdef AEE_SIMULATOR
    int nAscent, nDescent, nNCSize, nLCSize;
#endif
    // Init the global var.
    g_pBtuiApp = pMe;

    // Set the short hand of IShell and IDisplay.
    pMe->m_pIShell = pMe->m_a.m_pIShell;
#ifdef AEE_SIMULATOR
    g_pIShell = pMe->m_pIShell;
#endif
    ISHELL_AddRef(pMe->m_pIShell);

    pMe->m_pIDisplay = pMe->m_a.m_pIDisplay;
    IDISPLAY_AddRef(pMe->m_pIDisplay);   

    // Get the device infomation and set some member about display information.
    sDevInfo.wStructSize = sizeof(sDevInfo);
    ISHELL_GetDeviceInfo(pMe->m_pIShell, &sDevInfo); 

    // Set the client rect.
    SETAEERECT(&pMe->m_rcClient, 0, 0, sDevInfo.cxScreen, sDevInfo.cyScreen);
#ifdef AEE_SIMULATOR
    pMe->m_cxScreen = sDevInfo.cxScreen;
    pMe->m_cyScreen = sDevInfo.cyScreen;

    IDISPLAY_GetFontMetrics(pMe->m_pIDisplay, AEE_FONT_NORMAL, &nAscent, &nDescent);
    nNCSize = nAscent + nDescent; 
    IDISPLAY_GetFontMetrics(pMe->m_pIDisplay, AEE_FONT_LARGE, &nAscent, &nDescent);
    nLCSize = nAscent + nDescent;

    pMe->m_cyLine = ((nNCSize + nLCSize)>>1) + UI_SPACE;
    pMe->m_cyTitle = pMe->m_cyLine + UI_SPACE;
    pMe->m_nButtonLine = pMe->m_cyScreen - pMe->m_cyTitle;
    pMe->m_cyClip = pMe->m_nButtonLine - pMe->m_cyTitle;

    // Set the message box client rect.
    nAscent = (pMe->m_cyTitle<<1) + 8;
    SETAEERECT(&pMe->m_rcMsgBox, 12, nAscent, pMe->m_cxScreen - 24, pMe->m_cyClip - nAscent);
#endif
    bt_ui_cb.m_bsend_opc_file = FALSE;
    // Create the file manager.
    if(ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_FILEMGR, (void **)&pMe->m_pIFileMgr))
    {
        return FALSE;
    }
    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pIShell,AEECLSID_STATIC, (void **)&pMe->m_pStatic))
    {
        return FALSE;
    }  
    
#ifndef AEE_SIMULATOR
    if(SUCCESS != ISHELL_CreateInstance(pMe->m_pIShell, AEECLSID_ANNUNCIATOR, (void **) &pMe->m_pIAnn))
    {
        return FALSE;
    }
#endif

    // Create the window stack.
    pMe->m_p_win_stack = HtWinStack_New();
    if(NULL == pMe->m_p_win_stack)
    {
        return FALSE;
    }
    // Init the btui message dispatch callback.
    CALLBACK_Init(&pMe->m_cb_dispatch_msg, bt_ui_dispatch, (void *)pMe);

    //HT_MessageBoxTimeout(L"Starting", MAX_BTA_START_UP_TIME);

#ifdef AEE_SIMULATOR
    BootEntry();
#endif
    ui_start = (bt_ui_start*)BTUI_Malloc(sizeof(bt_ui_start));
    if(NULL == ui_start)
    {
        return FALSE;
    }
    //bcmapp_dm_get_sd_card_state(pMe);
    return TRUE;
}

/*===================================================================================================

FUNCTION tBtuiApp_FreeAppData

===================================================================================================*/
static void tBtuiApp_FreeAppData(IApplet* pi)
{
   tBtuiApp * pMe = (tBtuiApp*)pi;
    
    if(pMe->m_p_win_stack)  
    {
        HtWinStack_Delete(pMe->m_p_win_stack);
        pMe->m_p_win_stack = NULL;
    }

    {
        uint16 i = BTUI_MAX_MMI_MSG;
        while(i--)
            BTUI_FreeIf(pMe->m_p_msg_array[i]);
    }
    
    BTUI_FreeIf(ui_start);
#ifndef AEE_SIMULATOR
    if (pMe->m_pIAnn) 
    {
        IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_OFF);
        IANNUNCIATOR_Release(pMe->m_pIAnn);
        pMe->m_pIAnn= NULL;
    }
#endif

    BTUI_ReleaseObjIf(pMe->m_pIShell);
    BTUI_ReleaseObjIf(pMe->m_pIDisplay);
    BTUI_ReleaseObjIf(pMe->m_pIFileMgr);
    BTUI_ReleaseObjIf(pMe->m_pStatic);
    g_pBtuiApp = NULL;
}

/*******************************************************************************
FUNCTION         
    tBtuiApp_Exit

Description      
    exit bcm application.

PROTOTYPE:
    void tBtuiApp_Exit(void *po)
                 
Returns          
    void
*******************************************************************************/
void tBtuiApp_Exit(tBtuiApp *pMe)
{
    ISHELL_CloseApplet(pMe->m_pIShell, FALSE);
}


/*******************************************************************************
FUNCTION         
    tBtuiApp_Startup

Description      
    Initializes bcm application and waits for device to come up

PROTOTYPE:
    void tBtuiApp_Startup(void *po)
                 
Returns          
    void
*******************************************************************************/
static void tBtuiApp_Startup(tBtuiApp *pMe)
{
   BCM_FUN_STAR("tBtuiApp_Startup",0,0,0);
#ifdef FEATURE_BLUETOOTH_APP
   if(!BTA_DmIsDeviceUp())
    {
        if(pMe->m_dwStartup >= MAX_BTA_START_UP_TIME)
        {
            //HT_MessageBox(bt_ui_get_brx_string(IDS_FAIL));
            HT_MessageBoxTimeout_ex(IDS_FAIL,BTUI_TIMEOUT_MSG_PERIOD);
            ISHELL_SetTimer(pMe->m_pIShell, 3000, (PFNNOTIFY)tBtuiApp_Exit, (void *)pMe);       
        }
        else
        {   
            pMe->m_dwStartup += 200;
            ISHELL_SetTimer(pMe->m_pIShell, 200, (PFNNOTIFY)tBtuiApp_Startup, (void *)pMe);                 
        }   
        return;
    }
    bcmapp_dm_set_sleep_state(TRUE);
#endif//FEATURE_BLUETOOTH_APP

#ifdef FEATURE_CS09_BLUETOOTH_APP
//begin,liuhaijun,2009/01/07
#if 0
   if(!BTA_DmIsDeviceUp())
    {
        if(pMe->m_dwStartup >= MAX_BTA_START_UP_TIME)
        {
            HT_MessageBox(bt_ui_get_brx_string(IDS_FAIL));//L"Fail to start bluetooth device."
            ISHELL_SetTimer(pMe->m_pIShell, 3000, (PFNNOTIFY)tBtuiApp_Exit, (void *)pMe);       
        }
        else
        {   
            pMe->m_dwStartup += 200;
            ISHELL_SetTimer(pMe->m_pIShell, 200, (PFNNOTIFY)tBtuiApp_Startup, (void *)pMe);                 
        }   

        return;
    }
   
    bcmapp_dm_set_sleep_state(TRUE);

#endif
//end,liuhaijun,2009/01/07
#endif //FEATURE_CS09_BLUETOOTH


    bt_ui_cfg_init();
//#ifndef AEE_SIMULATOR
//#if((( defined BTA_FS_INCLUDED ) && (BTA_FS_INCLUDED == TRUE)) ||(( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)))
//    bcmapp_dm_get_sd_card_state(pMe);
//#endif
//#endif
    /* read all parmeters stored in nvram */
    bt_ui_init_device_db();

    bt_ui_menu_idle();

    if(bt_ui_device_db.bcm_enabled)
    {
        bcmapp_dm_init();
    }
}

/*===========================================================================

FUNCTION tBtuiApp_OnStart

===========================================================================*/
static boolean tBtuiApp_OnStart(tBtuiApp *pMe)
{
    if(pMe->m_bBackground == FALSE)
    {
        tBtuiApp_Startup(pMe);
    }
    else
    {
        bt_ui_menu_idle();
    }
    return TRUE; 
}
static void bt_ui_init_in_app_start(tBtuiApp *pMe)
{
    pMe->m_bQuicktest = FALSE;
    pMe->m_bsuspend = FALSE;
    bt_ui_cb.m_bsend_opc_file = FALSE;
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    MEMSET(bt_ui_cb.opp_send_file_addr,0,sizeof(bt_ui_cb.opp_send_file_addr));
    bt_ui_cb.opp_send_file_p = NULL;
    bt_ui_cb.recv_vcard_not_ack = FALSE;
#endif
    bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
    bt_ui_cb.b_opp_show_proc = FALSE;
#ifdef FEATURE_KEYGUARD
    bt_ui_cb.b_auto_keyguard = FALSE;
#endif
}

static void bt_ui_init_in_app_stop(tBtuiApp *pMe)
{
    pMe->m_bsuspend = FALSE;
    bt_ui_cb.m_bsend_opc_file = FALSE;
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
    MEMSET(bt_ui_cb.opp_send_file_addr,0,sizeof(bt_ui_cb.opp_send_file_addr));
    bt_ui_cb.opp_send_file_p = NULL;
    bt_ui_cb.recv_vcard_not_ack = FALSE;
#endif
    bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
    //bt_ui_cb.b_opp_show_proc = FALSE;
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
    bt_ui_cb.b_stop_op_file = TRUE;
#endif
    IANNUNCIATOR_SetField (bt_ui_get_IAnn(), ANNUN_FIELD_BLUETOOTH, ANNUN_STATE_BT_TRANS_OFF);
    if(bt_ui_device_db.bcm_enabled)
    {
        // If the bluetooth is enable, we switch the applet to background
        pMe->m_bBackground = TRUE;
        pMe->m_bsuspend = TRUE;
    }
}
/*===========================================================================

FUNCTION tBtuiApp_HandleEvent

DESCRIPTION
    This is the EventHandler for this app. All events to this app are handled in this
    function. All APPs must supply an Event Handler.

PROTOTYPE:
    boolean tBtuiApp_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
    pi: Pointer to the AEEApplet structure. This structure contains information 
      specific to this applet. It was initialized during the 
      AEEClsCreateInstance().
    ecode: Specifies the Event sent to this applet
   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none
===========================================================================*/

static boolean tBtuiApp_HandleEvent(IApplet * pi, AEEEvent eCode, 
                                     uint16 wParam, uint32 dwParam)
{  
    tBtuiApp * pMe = (tBtuiApp*)pi;
    if(eCode != EVT_APP_NO_SLEEP)
    {
        BCM_FUN_STAR("%x %x %x tBtuiApp_HandleEvent",eCode,wParam,dwParam);
    }
    switch (eCode) 
    {
    case EVT_APP_START:
    {
        AEEAppStart *as;
        as = (AEEAppStart*)dwParam;
        bt_ui_init_in_app_start(pMe) ;
        
        if(as->pszArgs)
        {
            BCM_FUN_STAR("pszArgs %d %d",as->pszArgs[0],pMe->m_bQuicktest,0);/*Q = 81,S =83,F = 70 A = 65*/
            if( (as->pszArgs[0] == 'Q')||(as->pszArgs[0] == 'q') )/*quick test*/
            {
                pMe->m_bQuicktest = TRUE;
                bt_ui_device_db.bcm_enabled = TRUE;
            }
            else if( (as->pszArgs[0] == 'S')||(as->pszArgs[0] == 's') )/*bluetooth in suspend ,restart it*/
            {
                if((ui_start != NULL) && (ui_start->fun != NULL))
                {
#ifdef FEATURE_KEYGUARD
                    if(OEMKeyguard_IsEnabled())
                    {
                        OEMKeyguard_SetState(FALSE);
                        bt_ui_cb.b_auto_keyguard = TRUE;
                    }
#endif
                    bt_ui_cb.after_resume_menu = bt_ui_close_applet;
                    if(BTUI_MMI_OPS_PROC == ui_start->msg.hdr.event ||BTUI_MMI_OPC_PROC == ui_start->msg.hdr.event ||BTUI_MMI_FTS_PROGRESS == ui_start->msg.hdr.event )
                    {
                        bt_ui_enable_backlight();
                        bt_ui_cb.b_opp_show_proc = TRUE;
                    }
                    ui_start->fun(&ui_start->msg);
                }
                return TRUE;  
            }
#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
            else if( (as->pszArgs[0] == 'F')||(as->pszArgs[0] == 'f') )/*user send file or vcard*/
            {
                //if(bt_ui_cb.num_devices == 0)/*no paired device, search it*/
                //else/*show bonded device*/
                MEMSET(bt_ui_cb.opp_send_file_addr , 0 ,sizeof(bt_ui_cb.opp_send_file_addr));
                STRNCPY(bt_ui_cb.opp_send_file_addr,as->pszArgs+1,AEE_MAX_FILE_NAME);
                bt_ui_cb.m_bsend_opc_file = TRUE;
                bt_ui_cb.opp_send_file_p = NULL;
            }
            else if( (as->pszArgs[0] == 'A')||(as->pszArgs[0] == 'a') )/*user send many file or vcard*/
            {
                char temp[16] = {0};
                STRLCPY(temp,as->pszArgs +1,sizeof(temp));
                bt_ui_cb.opp_send_file_p = (void*)ATOI(temp);
                bt_ui_cb.m_bsend_opc_file = TRUE;
            }
#endif
        }
        if(tBtuiApp_OnStart(pMe))
        {
            return TRUE;  
        }
        return FALSE;
    }
    case EVT_APP_STOP:
#ifdef FEATURE_KEYGUARD
        if(bt_ui_cb.b_auto_keyguard)
        {
            bt_ui_cb.b_auto_keyguard = FALSE;
            OEMKeyguard_SetState(TRUE);
        }
#endif
        bt_ui_init_in_app_stop(pMe);
        if(bt_ui_device_db.bcm_enabled)
        {
            boolean *pb = (boolean *)dwParam;
            if(pb) // brew magic var
            {
                // Set the app to background app
                *pb = FALSE;
            }

            if(bt_ui_cb.ui_state == UI_SEARCH_NEW_DEVICE)
            {
                bcmapp_dm_cancel_search();
                bt_ui_cb.ui_state = 0;
                bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
                bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
                bt_ui_cb.current_menu_handle = BT_UI_NULL_HANDLE;
            }

            // Close all the window
            //if(pMe->m_p_win_stack)
            //{
            //    HtWinStack_Clear(pMe->m_p_win_stack);
            //}
            //return FALSE;
        }

        if(pMe->m_p_win_stack)
        {
            HtWinStack_Clear(pMe->m_p_win_stack);
        }

        return TRUE;

    case EVT_APP_RESUME:
        //if(pMe->m_bBackground )
        //{
        //    pMe->m_bBackground = FALSE;
            //if(pMe->m_bQuicktest == FALSE)
            //{
                //bt_ui_menu_idle();  // create the dm idle menu.
            //}
            //else
            //{
            //    HT_UpdateDisplay();
            //}
        //}
        pMe->m_bBackground = FALSE;
        bt_ui_cb.b_opp_show_proc = TRUE;
        //HT_UpdateDisplay();
        HtWinStack_Draw(bt_ui_get_win_stack());
        pMe->m_bsuspend = FALSE;
        //bt_ui_set_draw_hold_down(FALSE);
        HtWinStack_SetDrawHoldDown(bt_ui_get_win_stack(), FALSE);
        return TRUE;

    case EVT_APP_SUSPEND:
        {
#ifdef FEATURE_BLUETOOTH_NOT_SUPPORT_OP_BACKGROUND
            //db_items_value_type db;
            //db_get(DB_IN_USE, &db);
            //if(db.in_use)
            //{
            //    bt_ui_cb.b_stop_op_file = TRUE;
            //}
#endif
            // Don't draw the ui if suspended.
            pMe->m_bBackground = TRUE;
            pMe->m_bsuspend = TRUE;
            bt_ui_cb.b_opp_show_proc = FALSE;//if suspend,we don,t show proc windows
            //bt_ui_set_draw_hold_down(TRUE);
            HtWinStack_SetDrawHoldDown(bt_ui_get_win_stack(), TRUE);
            HtWinStack_Enable(bt_ui_get_win_stack(), FALSE);
        }   
        return TRUE;

    case EVT_BLUETOOTH_EVENT:
        {
            // Dispatch the MMI message from bcm task callback.
            bt_ui_handle((tBTUI_BTA_MSG *)dwParam, (BTUI_MSG_TYPE)wParam);
        }   
        return TRUE;

    case EVT_KEY:
#ifdef AEE_SIMULATOR    
        if(wParam == AVK_CLR)
            wParam = AVK_SOFT2;
#endif  
   default:
        if(pMe->m_p_win_stack && 
            HtWinStack_HandleEvent(pMe->m_p_win_stack, eCode, wParam, dwParam))
        {   
            return TRUE;
        }
        else if(AVK_CLR == wParam)
        {
            return TRUE;	// The applet will be closed if we do not return TRUE when the KEY value is AVK_CLR.
        }

        break;

    case EVT_KEY_PRESS:
        if(wParam == AVK_END)
        {
            bt_ui_cb.b_opp_show_proc = FALSE;//only stop show proc ,if we press end key,not set it in app stop!!!!!!
        }
        break;
   }

   return FALSE;
}



/*===========================================================================

FUNCTION: AEEClsCreateInstance

DESCRIPTION
    This function is invoked while the app is being loaded. All Modules must provide this 
    function. Ensure to retain the same name and parameters for this function.
    In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
    that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers 
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
    clsID: [in]: Specifies the ClassID of the applet which is being loaded

    pIShell: [in]: Contains pointer to the IShell interface. 

    pIModule: pin]: Contains pointer to the IModule interface to the current module to which
    this app belongs

    ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
    of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
#if defined(AEE_STATIC)
int tBtuiApp_CreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#else
int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)
#endif
{
   *ppObj = NULL;
        
   // We want to load this App. So, invoke AEEApplet_New().To it, pass the
   // address of the app-specific handle event function.
   if(ClsId == AEECLSID_BTUIAPP)
   {
       if(AEEApplet_New(sizeof(tBtuiApp), ClsId, pIShell,po,(IApplet**)ppObj,
         (AEEHANDLER)tBtuiApp_HandleEvent,(PFNFREEAPPDATA)tBtuiApp_FreeAppData)
         == TRUE)
      {
            if (tBtuiApp_InitAppData((IApplet*)*ppObj) == TRUE)
            {
                return(AEE_SUCCESS);
            }
            else
            {
                //Release the applet. This will FREE the memory allocated for the applet
                IAPPLET_Release((IApplet*)*ppObj);
                *ppObj = NULL;
                return EFAILED;
            }            
      }
   }

   return (EFAILED);
}


/*===========================================================================

FUNCTION tBtuiApp_GetVersion

===========================================================================*/
extern void tBtuiApp_GetVersion(uint8* pszVersion, uint16 szLen)
{
    if(pszVersion)
        bt_ui_str_cpyn((char *)pszVersion, szLen, "V1.2008.11.03");
}

void bcmapp_dm_set_sleep_state(boolean state)
{
    static boolean only_one = FALSE;
    if(only_one == FALSE )
    {
        if(TRUE == BTA_DmIsDeviceUp())
        {
            only_one = TRUE;
#ifdef AEE_STATIC
            BTA_HCILP_Enable(state);
#endif
        }
    }
}

void bt_ui_process_suspend_event(tBTUI_BTA_MSG* p_msg,bt_ui_start_fun * fun)
{
    
    boolean is_ok = TRUE;
    if(NULL == ui_start)
    {
        return;
    }

    MEMSET(&ui_start->msg,0,sizeof(tBTUI_BTA_MSG));
    ui_start->msg.pin_req.hdr.event = p_msg->hdr.event;
    ui_start->fun = fun;
    switch(p_msg->hdr.event)
    {
        case BTUI_MMI_PIN_REQ:
        {
            //bdcpy(ui_start->msg.pin_req.bd_addr, p_msg->pin_req.bd_addr);
            MEMCPY ((void *)&ui_start->msg.pin_req.bd_addr, (const void *)p_msg->pin_req.bd_addr, BD_ADDR_LEN);
            STRNCPY(ui_start->msg.pin_req.dev_name, (char*)p_msg->pin_req.dev_name, BTUI_DEV_NAME_LENGTH);
            ui_start->msg.pin_req.dev_name[BTUI_DEV_NAME_LENGTH] = 0;
            ui_start->msg.pin_req.dev_class[0] = p_msg->pin_req.dev_class[0];
            ui_start->msg.pin_req.dev_class[1] = p_msg->pin_req.dev_class[1];
            ui_start->msg.pin_req.dev_class[2] = p_msg->pin_req.dev_class[2];
            break;
        }

        case BTUI_MMI_AUTH_REQ:
            //bdcpy(ui_start->msg.auth_req.bd_addr, p_msg->auth_req.bd_addr);
            MEMCPY ((void *)&ui_start->msg.auth_req.bd_addr, (const void *)p_msg->auth_req.bd_addr, BD_ADDR_LEN);
            STRNCPY(ui_start->msg.auth_req.dev_name, (char*)p_msg->auth_req.dev_name, BTUI_DEV_NAME_LENGTH);
            ui_start->msg.auth_req.service = p_msg->auth_req.service;
            break;

#if( defined BTA_OP_INCLUDED ) && (BTA_OP_INCLUDED == TRUE)
        case BTUI_MMI_OPS_OBJECT:
            ui_start->msg.ops_object.format = p_msg->ops_object.format;
            STRNCPY(ui_start->msg.ops_object.name, p_msg->ops_object.name, 100/*move*/);
            break;
#ifdef BTUI_OPS_NEED_RECEIVE_ACK
        case BTUI_MMI_OPS_ACCESS:
            /*in ui_ops,have save msg to static memery*/
            break;
#endif
#endif //BTA_OP_INCLUDED
        case BTUI_MMI_OPS_PROC:
        case BTUI_MMI_OPC_PROC:
        case BTUI_MMI_FTS_PROGRESS:
            ui_start->msg.opp_proc.val.bytes_transferred = p_msg->opp_proc.val.bytes_transferred;
            ui_start->msg.opp_proc.val.obj_size= p_msg->opp_proc.val.obj_size;
            break;

        default:
            is_ok= FALSE;
            break;
    }
    if(is_ok)
    {
        ISHELL_StartAppletArgs(bt_ui_get_shell(), AEECLSID_BTUIAPP, "S");
    }
}
/////////////////////////////////File End/////////////////////////////////////////
