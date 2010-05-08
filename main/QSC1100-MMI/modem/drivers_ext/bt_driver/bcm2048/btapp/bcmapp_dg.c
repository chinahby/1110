/****************************************************************************
**                                                                           
**  Name:          btapp_dg.c                                           
**                 
**  Description:   Contains application code for  data gateway
**                 
**                                                                           
**  Copyright (c) 2002, Widcomm Inc., All Rights Reserved.              
**  Widcomm Bluetooth Core. Proprietary and confidential.                    
******************************************************************************/

#include "bt_target.h"

#if( defined BTA_DG_INCLUDED ) && (BTA_DG_INCLUDED == TRUE)

#include "gki.h"
#include "bta_api.h"
#include "bta_dg_api.h"
#include "btui.h"
#include "bta_dg_ci.h"
#include "btui_int.h"
#include <stdio.h>
#include <string.h>
#include "bd.h"

#include "btapp_dm.h"
#include "btapp_dg.h"

/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/

void btui_dg_cback(tBTA_DG_EVT event, tBTA_DG *p_data);

tBTUI_DG_CB btui_dg_cb;

/*******************************************************************************
**
** Function         btapp_dg_init
**
** Description      Initialises data gateway 
**                  
**
** Returns          void
**
*******************************************************************************/
void btapp_dg_init(void)
{

    tBTA_SERVICE_MASK   services;

    services = (btui_cfg.supported_services &
                (BTA_SPP_SERVICE_MASK | BTA_DUN_SERVICE_MASK | BTA_FAX_SERVICE_MASK));
    
    BTA_DgEnable(btui_dg_cback);

    /* start servers for enable services */
    /* the sample application uses the bta
    service id as the appl_id parameter for
    the API */

    if(services & BTA_SPP_SERVICE_MASK)
    {
        BTA_DgListen(BTA_SPP_SERVICE_ID, btui_cfg.sppdg_security,
                     btui_cfg.sppdg_service_name, BTUI_DG_ID_SPP);
    }
    if(services & BTA_DUN_SERVICE_MASK)
    {
        BTA_DgListen(BTA_DUN_SERVICE_ID, btui_cfg.dundg_security,
                     btui_cfg.dundg_service_name, BTUI_DG_ID_DUN);
    }
    if(services & BTA_FAX_SERVICE_MASK)
    {
        BTA_DgListen(BTA_FAX_SERVICE_ID, btui_cfg.faxdg_security,
                     btui_cfg.faxdg_service_name, BTUI_DG_ID_FAX);
    }

    /* perform platform specific
    initialization */
    btui_dg_platform_init();
}

/*******************************************************************************
**
** Function         btapp_dg_close_connection
**
** Description      
**                  
**
** Returns          void
**
*******************************************************************************/
void btapp_dg_close_connection(UINT8 conn_index)
{

    BTA_DgClose(btui_dg_cb.app_cb[conn_index].api_handle);

}
/*******************************************************************************
**
** Function         btapp_dg_connect
**
** Description      
**                  
**
** Returns          void
**
*******************************************************************************/
void btapp_dg_connect(void)
{
    char *p_name;

    if (btui_cfg.dg_client_peer_name[0] == '\0')
    {
        p_name = NULL;
    }
    else
    {
        p_name = btui_cfg.dg_client_peer_name;
    }

    BTA_DgOpen(btui_cb.p_selected_rem_device->bd_addr,
           btui_dg_cb.app_cb[BTUI_DG_ID_CLIENT].service_id,
           btui_cfg.dg_client_security,
           p_name, BTUI_DG_ID_CLIENT);
}


/*******************************************************************************
**
** Function         btapp_dg_set_device_authorized
**
** Description      Bond with the device
**                  
**
** Returns          void
*******************************************************************************/
void btapp_dg_set_device_authorized (tBTUI_REM_DEVICE * p_device_rec)
{


    /* update BTA with this information.If a device is set as trusted, BTA will
    not ask application for authorization, */

    p_device_rec->is_trusted = TRUE;
    p_device_rec->trusted_mask |= BTA_SPP_SERVICE_MASK |BTA_DUN_SERVICE_MASK | BTA_FAX_SERVICE_MASK | BTA_LAP_SERVICE_MASK;
    btui_store_device(p_device_rec);
    btapp_dm_sec_add_device(p_device_rec);



}
/*******************************************************************************
**
** Function         btui_dg_cback
**
** Description      Callback function from BTA data gateway
**                  
**
** Returns          void
**
*******************************************************************************/
void btui_dg_cback(tBTA_DG_EVT event, tBTA_DG *p_data)
{
    tBTUI_BTA_MSG * p_msg;

    APPL_TRACE_DEBUG1("btui_dg_cback event:%d", event);

    if (event == BTA_DG_LISTEN_EVT)
    {
        APPL_TRACE_DEBUG2(" DG listen handle:%d, app_id:%d", p_data->listen.handle, p_data->listen.app_id);

        /* save the handle for the services */
        btui_dg_cb.app_cb[p_data->listen.app_id].api_handle = p_data->listen.handle;
    }
    else if (event == BTA_DG_OPENING_EVT)
    {
        APPL_TRACE_DEBUG2(" DG opening handle:%d, app_id:%d", p_data->opening.handle, p_data->opening.app_id);

        /* save the handle */
        btui_dg_cb.app_cb[p_data->opening.app_id].api_handle = p_data->opening.handle;

        /* set is_open flag */
        btui_dg_cb.app_cb[p_data->opening.app_id].is_open = TRUE;
    }
    else if (event == BTA_DG_OPEN_EVT)
    {
        APPL_TRACE_DEBUG2(" DG open handle:%d, service:%d", p_data->open.handle, p_data->open.service);
        APPL_TRACE_DEBUG6(" bd addr %x:%x:%x:%x:%x:%x", p_data->open.bd_addr[0], p_data->open.bd_addr[1],
                          p_data->open.bd_addr[2], p_data->open.bd_addr[3], p_data->open.bd_addr[4], p_data->open.bd_addr[5]);

        /* set is_open flag */
        btui_dg_cb.app_cb[p_data->open.app_id].is_open = TRUE;

        /* sent a event to indicate an up connection */
        if ((p_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
	    {
		    p_msg->open.hdr.event = BTUI_MMI_CONN_UP;
            p_msg->open.status = 0;
            bdcpy(p_msg->open.bd_addr, p_data->open.bd_addr);
            p_msg->open.service = p_data->open.service;
            GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_msg);            
	    }

    }
    else if (event == BTA_DG_CLOSE_EVT)
    {
        APPL_TRACE_DEBUG1(" DG close handle:%d", p_data->close.handle);

        /* if this is a close event when the connection went down */
        if(btui_dg_cb.app_cb[p_data->close.app_id].is_open)
        {

            /* clear is_open flag */
            btui_dg_cb.app_cb[p_data->close.app_id].is_open = FALSE;

            /* send an event to indicate a down connection */
            if ((p_msg = (tBTUI_BTA_MSG *)GKI_getbuf(sizeof(tBTUI_BTA_MSG))) != NULL)
	        {
		        p_msg->close.hdr.event = BTUI_MMI_CONN_DOWN;
                p_msg->close.service = btui_dg_cb.app_cb[p_data->close.app_id].service_id;
                GKI_send_msg(BTE_APPL_TASK, TASK_MBOX_0, p_msg);
	        }

        }
    }
}

#endif