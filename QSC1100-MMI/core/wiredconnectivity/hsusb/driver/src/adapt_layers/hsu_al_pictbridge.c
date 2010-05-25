
/*==============================================================================

                      High Speed USB PictBridge Adaptation Layer
  
GENERAL DESCRIPTION
Implementation of PictBridge Adaptation Layer.
This includes implementation of the API functions defined in pictbridge_api.h.
      
Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

  EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
    $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_pictbridge.c#2 $
    
    when      who     what, where, why
    --------  ---     ----------------------------------------------------------
    
==============================================================================*/


/*==============================================================================

  INCLUDE FILES FOR MODULE
  
==============================================================================*/

#include "customer.h" /* For FEATURE_HSU_TEST to work */
#include "hsu_common.h"
#include "err.h"


#include "dps_device_api.h"
#include "jusb_common.h"
#include "uw_device_sicd.h"

#include "pictbridge_api.h" /* Generic PictBridge API that wraps Jungo DPS API */
#include "hsu_conf_sel_i.h"  /* Dynamic composition switching */
#include "hsu_conf_sel_ui.h" /* Dynamic composition switching */
#include "hsu_al_sicd.h"

#include "rdevmap.h"

#include "AEEstd.h"


/*==============================================================================

                     Jungo specific private data

==============================================================================*/

#define DPS_CAP_MAX ((uint32)DPS_CAP_CROPPINGS + 1)

static struct
{
    dps_handle_t dpsh;
    dps_req_conf_t config_req;

    dps_job_config_t jconfig;
    ptp_storage_id_t store_id;

    pictbridge_notify_cb_t pictbridge_cb;
    void * client_h; /* PictBridge client handle. Used for callback */

    /* save the original composition to restore later */
    hsu_conf_sel_composition_id_enum old_hsu_comp_id; 

    /* printer capability list. Each entry represent a list of capablity in the
       order defined in dps_capability_t.  
       i.e. cap_list[DPS_CAP_QUALITIES] reprsents a list of qualities capability.
    */
    dps_cap_array_t cap_list[DPS_CAP_MAX];

    /* indicates dynamic comp switching is finished. */          
    boolean comp_change_done; 
} app_ctx;


/*==============================================================================

                   pictbridge private functions

==============================================================================*/

static void pictbridge_notify(pictbridge_notifiy_t type, void * data)
{
    if (NULL == app_ctx.pictbridge_cb)
        return;

    /* BREW OEM layer will have to translate Jungo DPS data into BREW DPS data */
    app_ctx.pictbridge_cb(app_ctx.client_h, type, data);
}


static void pictbridge_rdm_event_cb (rdm_assign_status_type status,
                                            rdm_service_enum_type service,
                                            rdm_device_enum_type device)
/*lint -esym(715,service)*/
/*lint -esym(715,device)*/
{
    pb_rdm_change_status_t pb_rdm_status; 

    switch(status) 
    {
    case RDM_DONE_S:
        pb_rdm_status = PB_RDM_DONE;
        break;    

    case RDM_NOT_ALLOWED_S:
        pb_rdm_status = PB_RDM_NOT_ALLOWED;
        break;
        
    case RDM_DEVMAP_BUSY_S:
        pb_rdm_status = PB_RDM_DEVMAP_BUSY;
        break;

    case RDM_APP_BUSY_S:
        pb_rdm_status = PB_RDM_APP_BUSY;
        break;

    default:
        pb_rdm_status = PB_RDM_NOT_ALLOWED;
        break;        
    }       


    pictbridge_notify(PICTBRIDGE_RDM_CHANGE_STATUS, &pb_rdm_status);
  
} /* pictbridge_rdm_event_cb */


static ptp_obj_format_t dps_filetype_to_ptp_obj_format(dps_param_t file_type)
{
    ptp_obj_format_t ptp_obj_format = PTP_OBJ_FORMAT_UNDEFINED;

    switch(file_type)
    {
    /* DPS file_type */   /* ptp obj format */
    case DPS_FILE_TYPE_DEFAULT:
    case DPS_FILE_TYPE_EXIF_JPEG:
    case DPS_FILE_TYPE_EXIF:
    case DPS_FILE_TYPE_JPEG:
      ptp_obj_format = PTP_OBJ_FORMAT_IMG_JPEG;
      break;

    case DPS_FILE_TYPE_DPOF:
      ptp_obj_format = PTP_OBJ_FORMAT_DPOF;
      break;

    default:
      ptp_obj_format = PTP_OBJ_FORMAT_IMG_JPEG;     
    }  

    return ptp_obj_format;
}

/*==============================================================================

                      pictbridge API implementations

==============================================================================*/

boolean pictbridge_handle_mode_change(pb_rdm_change_status_t status)
{
    boolean ret = FALSE;
    
    switch(status)
    {
    case PB_RDM_DONE:
        /* Ignore subsequent RDM_DONE_S notifications after the initial one.
           Calling hsu_conf_sel_ui_rdm_done() multiple times will result in
           ERROR FATAL.
        */
        if (TRUE == app_ctx.comp_change_done)
        {
            ret = TRUE;
            break;
        }
          
        if(hsu_conf_sel_ui_rdm_done() == FALSE)
        {
            /* Error. */
            MSG_ERROR("hsu_conf_sel_ui_rdm_done failed", 0, 0, 0);

        }
        else
        {
            if(hsu_conf_sel_ui_comp_change_is_done() == TRUE)
            {
                /* Successful composition change */
                MSG_HIGH("composition change successful!!!", 0,0,0);
                ret = TRUE;
                app_ctx.comp_change_done = TRUE;
            }
        }              
        break;
        
    case PB_RDM_NOT_ALLOWED:       
    case PB_RDM_DEVMAP_BUSY:       
    case PB_RDM_APP_BUSY:
        /* USB mode change failed. */
        MSG_ERROR("RDM status error, w=%d", status, 0, 0);
        hsu_conf_sel_ui_rdm_event_received((uint16)status);
        break;
    }

    return ret;
}


/* Initiate spoof disconnect and re-enumrate the device as a Still Image Capture
Device (SICD).  The process could take several seconds. */
uint32 pictbridge_connect_printer()
{
    const hsu_conf_sel_comp_info_type * comp_info = hsu_conf_sel_get_cur_comp_details();

    if (NULL == comp_info)
    {
        return 0;
    }

    if (HSU_CONF_SEL_PICTBRIDGE_COMP == comp_info->hsu_comp_id)
    {
        return 1; /* Nothing to do here */
    }
    
    /* Check if we can switch the composition dynamically right now */
    if (!hsu_conf_sel_ui_dynamic_switch_is_allowed())
    {
        return 0; /* We cannot change the composition right now */
    }
    
    /* Save current composition.  Restore it when disconnecting the printer */
    app_ctx.old_hsu_comp_id = comp_info->hsu_comp_id;
    app_ctx.comp_change_done = FALSE;

    /* Dynamically swtich over to PictBridge composition */
    if ( !hsu_conf_sel_ui_switch_comp(HSU_CONF_SEL_PICTBRIDGE_COMP, pictbridge_rdm_event_cb) )
    {
        return 0;
    }

    return 1;
}


uint32 pictbridge_disconnect_printer()
{
    /* Check if we can switch the composition dynamically right now */
    if (!hsu_conf_sel_ui_dynamic_switch_is_allowed())
    {
        return 0; /* We cannot change the composition right now */
    }

    app_ctx.comp_change_done = FALSE;
    /* Dynamically swtich over to PictBridge composition */
    if ( !hsu_conf_sel_ui_switch_comp(app_ctx.old_hsu_comp_id, pictbridge_rdm_event_cb) )
    {
        return 0;
    }

    return 1;
}

uint32 pictbridge_configure_print_service(
    const char * vendor_name,
    const char * product_name,
    const char * serial_number
)
{
    uint32 result = 0;
   
    app_ctx.config_req.versions_count = 1;
    app_ctx.config_req.dps_version[0].major = 1;
    app_ctx.config_req.dps_version[0].minor = 0;
    app_ctx.config_req.vendor_specific_version.major = 1;
    app_ctx.config_req.vendor_specific_version.minor = 0;

    if (NULL == vendor_name)
    {
        (void) std_strlcpy(app_ctx.config_req.vendor_name, "Qualcomm Inc.", DPS_MAX_STRING_SIZE+1);
    }
    else
    {
        (void) std_strlcpy(app_ctx.config_req.vendor_name, vendor_name, DPS_MAX_STRING_SIZE+1);
    }

    if (NULL == product_name)
    {
        (void) std_strlcpy(app_ctx.config_req.product_name, "PictBridge", DPS_MAX_STRING_SIZE+1);
    }
    else
    {
        (void) std_strlcpy(app_ctx.config_req.product_name, product_name, DPS_MAX_STRING_SIZE+1);
    }

    if (NULL == serial_number)
    {
        (void) std_strlcpy(app_ctx.config_req.serial_no, "0001", DPS_MAX_STRING_SIZE+1);
    }
    else
    {
        (void) std_strlcpy(app_ctx.config_req.serial_no, serial_number, DPS_MAX_STRING_SIZE+1);
    }
    
    if(0 == dps_configure_print_service(app_ctx.dpsh, &app_ctx.config_req))
    {
      result = 1;
    }
    
    return result;
}



uint32 pictbridge_register_callback(void * pUserData, pictbridge_notify_cb_t cb)
{
    app_ctx.client_h = pUserData;
    app_ctx.pictbridge_cb = cb;
    return 1;
}


/* dps_handle_t is the only thing we are adding when calling Jungo DPS API functions */
int pictbridge_start_job(hsu_dps_job_config_t *config, hsu_dps_job_info_t *info, int count)
{
    sicd_object_t *obj;
    jint_t i;
    jresult_t rc;
    
    /* Add all images in the job to sicd_store via sicd_store_add_object().
       We need to somehow add these images to the sicd store so that 
       handle_get_obj_info() and handle_get_obj() will know how to look up 
       these images as sicd objects.

       We need to pass in the full file path of each image. 
    */

    /* Cache the job_config data.  We may remove config parameter from this function API */
    (void) j_memcpy(&app_ctx.jconfig, config, sizeof(dps_job_config_t));

    for (i=0; i < count ; i++)
    {
        obj = (sicd_object_t *)jmalloc(sizeof(sicd_object_t), M_ZERO);
        if (!obj)
        {
            return JENOMEM; /* no memory */
        }
        
        info[i].file_id = obj->id = sicd_get_new_obj_handle();
        obj->info.storage_id = app_ctx.store_id;
        obj->info.format = dps_filetype_to_ptp_obj_format(config->file_type);
        obj->info.compressed_size = sicd_fs_file_size(info[i].file_path);


        if (PTP_OBJ_FORMAT_DPOF == obj->info.format)
        {
            obj->info.thumb_format = dps_filetype_to_ptp_obj_format(config->file_type);
            obj->info.thumb_compressed_size = 0;
            obj->info.filename = ptp_string_atow("/MISC/AUTPRINT.MRK");
        }
        else
        {
            /* TODO : get jpeg thumb size from the file if thumb image exist */
            obj->info.thumb_format = dps_filetype_to_ptp_obj_format(config->file_type);
            obj->info.thumb_compressed_size = sicd_fs_file_size(info[i].file_path); 
            obj->info.filename = ptp_string_atow(info[i].file_name);
        }

        obj->data = 0; /* image data should be copied directly from the file */       
        obj->sample_data = 0; /* thumbnail data should be copied directly from the file */
        (void) std_strlcpy(obj->file_path, info[i].file_path, DPS_FILEPATH_SIZE+1);
        rc = sicd_store_add_object(app_ctx.store_id, obj);
        if (rc)
        {
            sicd_free_object(NULL, obj);
            return EUNKNOWN; /* add obj failed */
        }
    }
    
    return dps_start_job(app_ctx.dpsh, (dps_job_config_t*)config, (dps_job_info_t*)info, count);
}

int pictbridge_abort_job(uint32 abort_style)
{
    return dps_abort_job(app_ctx.dpsh, abort_style);
}

int pictbridge_continue_job()
{
    return dps_continue_job(app_ctx.dpsh);
}


int pictbridge_get_capability(hsu_dps_capability_t cap_type, dps_param_t paper_size)
{
    if (cap_type > HSU_DPS_CAP_CROPPINGS)
        return NULL;

    return dps_get_capability(app_ctx.dpsh, (dps_capability_t)cap_type, paper_size);
}

int pictbridge_get_job_status()
{
    return dps_get_job_status(app_ctx.dpsh);
}

int pictbridge_get_device_status()
{
    return dps_get_device_status(app_ctx.dpsh);
}

/*==============================================================================

                Internal functions -- JUNGO call-back handlers

==============================================================================*/


/* pictbridge_handle_printer_connect */
static jint_t pictbridge_handle_printer_connect(dps_appctx_t appctx)
{
    HSU_USE_PARAM(appctx);
    
    /* Notify the client printer connect */
    /* PBPrint has issues handling printer disconnect event.  
       Since we are not sending printer disconnect, we should
       not send printer connect event either to keep the symmetry.
    */
    /* pictbridge_notify(PICTBRIDGE_PRINTER_CONNECT, NULL); */
   
    /* Tell DPS layer that we want to connect to the printer by returning 1 */
    return 1;           /* WE WANT PICTBRIDGE */
}

/* pictbridge_handle_printer_established */
static void pictbridge_handle_printer_established(dps_appctx_t appctx)
{
    int32 status = TRUE;

    HSU_USE_PARAM(appctx);

    HSU_MSG_HIGH("hsu_al_pictbridge: DPS Discovery done!", 0, 0, 0);
    
    /* DPS Discovery is finished successfully.
       notify the clients of the DPS discovery */
    pictbridge_notify(PICTBRIDGE_DPS_DISCOVERY, &status);
}

/* pictbridge_config_complete */
static void pictbridge_config_complete(dps_appctx_t appctx,
                            dps_param_t result, 
                            dps_res_conf_t *printer_cfg)
{
    HSU_USE_PARAM(appctx);
    HSU_USE_PARAM(result);

    /* Received DPS ConfigurePrintService info from the printer. */

    /* If there are problems with received DPS configurePrintService 
       response, printer_cfg may be NULL. */
    if (NULL == printer_cfg)
      return;

    /* Most important information from printer_cfg is service_available field */
    pictbridge_notify(PICTBRIDGE_PRINTER_CFG, printer_cfg);
}


/* pictbridge_get_cap_complete */
static void pictbridge_get_cap_complete(dps_appctx_t appctx, 
                             dps_param_t result,
                             dps_cap_array_t *cap_list)
{
    uint32 cap_type = (uint32) cap_list->cap_type;
    HSU_USE_PARAM(appctx);
    HSU_USE_PARAM(result);
    HSU_PARAM_COULD_BE_CONST(cap_list);
    
    /* Update the current DPS app's printer capability cache */
    if (NULL == cap_list)
    {
        return;
    }

    (void) j_memcpy(&app_ctx.cap_list[cap_type], cap_list, sizeof(dps_cap_array_t));

    /* Notify the client app of the printer capability just received */
    pictbridge_notify(PICTBRIDGE_GET_CAPABILITY, &app_ctx.cap_list[cap_type]);
}


/* pictbridge_handle_printer_disconnect */
static void pictbridge_handle_printer_disconnect(dps_appctx_t appctx)
{
    /* Notify the client printer disconnect */
    HSU_USE_PARAM(appctx);

    /* If we send printer disconnect notification, PBPrint app 
       will release IPictBridge and destroy the object.
       We should not send disconnect notification until PBPrint
       fixes this issue.
    */
   
    /* pictbridge_notify(PICTBRIDGE_PRINTER_DISCONNECT, NULL); */
}


/* 
    pictbridge_job_stat_complete() gets called when the phone receives
    job status response after calling dps_get_job_status().
*/
static void pictbridge_job_stat_complete(dps_appctx_t appctx, 
                              dps_param_t result,
                              dps_job_status_t *status)
{
    HSU_USE_PARAM(appctx);
    HSU_PARAM_COULD_BE_CONST(result);

    if (DPS_RESULT_OK == result && status)
    {
        pictbridge_notify(PICTBRIDGE_GET_JOB_STUTUS, status);
    }
}

/*  
    pictbridge_dev_stat_complete() gets called when the phone receives 
    device status response after calling dps_get_device_status().
*/
static void pictbridge_dev_stat_complete(dps_appctx_t appctx, 
                              dps_param_t result,
                              dps_device_status_t *status)
{
    HSU_USE_PARAM(appctx);
    HSU_PARAM_COULD_BE_CONST(result);

    if (DPS_RESULT_OK == result && status)
    {
        pictbridge_notify(PICTBRIDGE_GET_DEVICE_STATUS, status);
    }
}


/* pictbridge_handle_notify_job_status */
static dps_param_t pictbridge_handle_notify_job_status(dps_appctx_t appctx,
                                            dps_job_status_t *status)
{   
    HSU_USE_PARAM(appctx);
    
    if (NULL == status)
    {
        return DPS_RESULT_UNKNOWN_OP;
    }
    
    pictbridge_notify(PICTBRIDGE_JOB_STUTUS, status);
    return DPS_RESULT_OK;
}


/* pictbridge_handle_notify_device_status */
static dps_param_t pictbridge_handle_notify_device_status(dps_appctx_t appctx,
                                               dps_device_status_t *status)
{
    HSU_USE_PARAM(appctx);
    
    /* DPS_NotifyDeviceStatus event has been received.  One or more of printer 
       status params must have changed. 
    */

    /* MUST check for null for status */
    if ( NULL == status )
    {
        return DPS_RESULT_UNKNOWN_OP;
    }
        
    /* When a print job is finished, remove all objects added for the job.
       If a fatal error occurred, remove all objects from the default store.
       If the print status is IDLE, we should remove images stored for the job.
    */
    if (status->job_end_reason > DPS_DEV_STAT_JOB_NOT_ENDED  ||
        status->error_status == DPS_DEV_STAT_ERR_FATAL ||
        status->print_service_status == DPS_DEV_STAT_IDLE)
    {
        sicd_store_t* store = sicd_store_get_first();
        if (store)
        {
            (void) sicd_store_remove_all_objects(store->id);
        }
    }
   
    pictbridge_notify(PICTBRIDGE_DEVICE_STATUS, status);

    return DPS_RESULT_OK;
}


/* Given a filepath of an image, this function returns the associated file ID. 
*/
static dps_param_t pictbridge_get_file_id
(
    dps_appctx_t appctx,
    ptp_obj_handle_t autprint_mrk_file_id, 
    char * dcf_file_path, 
    ptp_obj_handle_t *file_id
)
{
    dps_param_t result = DPS_RESULT_OK;
    uint32 file_size = 0;
    sicd_object_t *obj = NULL;  
    jresult_t rc;
    char * file_name = NULL;

    HSU_USE_PARAM(appctx);
    HSU_USE_PARAM(autprint_mrk_file_id);
    HSU_PARAM_COULD_BE_CONST(dcf_file_path);

    if (NULL == dcf_file_path)
    {
        return DPS_PARAM_MAKE(DPS_RESULT_UNEXECUTED_OP, DPS_RESULT_MIN_INVALID_VALUE);
    }
     
    /* convert DCF path to AMSS EFS path */
    while (('.' == *dcf_file_path) || ('/' ==*dcf_file_path))
    {
        ++dcf_file_path;
    }
    
    /* Check the received filepath */  
    file_size = sicd_fs_file_size(dcf_file_path);
    if (0 == file_size)
    {
        return DPS_PARAM_MAKE(DPS_RESULT_UNEXECUTED_OP, DPS_RESULT_MIN_INVALID_VALUE);
    }

    /* Create a new sicd object */
    obj = (sicd_object_t*)jmalloc(sizeof(sicd_object_t), M_ZERO);
    if (NULL == obj)
    {
        return DPS_RESULT_UNEXECUTED_OP; /* no memory */
    }
    
    /* Get a new object handle */
    obj->id = sicd_get_new_obj_handle();

    /* Fill the new object with file info */   
    obj->info.storage_id = app_ctx.store_id;
    obj->info.format = PTP_OBJ_FORMAT_IMG_JPEG;
    obj->info.compressed_size = file_size;

    /* TODO : get jpeg thumb size from the file if thumb image exist */
    obj->info.thumb_format = PTP_OBJ_FORMAT_IMG_JPEG;
    obj->info.thumb_compressed_size = file_size; 

    file_name = strrchr(dcf_file_path, (int)'/');
    if ( file_name )
    {
        file_name++; /* skip '/'  */
        obj->info.filename = ptp_string_atow(file_name);
    }
    obj->data = 0; /* image data should be copied directly from the file */       
    obj->sample_data = 0; /* thumbnail data should be copied directly from the file */
    (void) std_strlcpy(obj->file_path, dcf_file_path, DPS_FILEPATH_SIZE+1);

    /* Add the sicd object to the storage */
    rc = sicd_store_add_object(app_ctx.store_id, obj);
    if (rc)
    {
        sicd_free_object(NULL, obj);
        return DPS_RESULT_UNEXECUTED_OP; /* add obj failed */
    }
    
    /* Return the object handle (ID) */
    *file_id = obj->id;

    return result;
}


static void dps_init_storage(void)
{
    sicd_store_t *store;

    store = sicd_store_get_first();
    if (!store)
    {
        HSU_MSG_ERROR("dps_init_storage: could not find store"
          "to place sample JPG in",0,0,0);
        return;
    }

    app_ctx.store_id = store->id;
}


jresult_t dps_app_init(ptp_instanceh_t insth)
{
    dps_callbacks_t dps_cbs;
    jresult_t rc = 0;
      
    (void) j_memset(&app_ctx.jconfig, 0, sizeof(dps_job_config_t));

    dps_init_storage();

    dps_cbs.printer_connect_h = pictbridge_handle_printer_connect;
    dps_cbs.printer_estab_h = pictbridge_handle_printer_established;
    dps_cbs.printer_disconnect_h = pictbridge_handle_printer_disconnect;

    /* Print client action callbacks */
    dps_cbs.config_print_service_complete = pictbridge_config_complete;
    dps_cbs.start_job_complete = NULL;
    dps_cbs.abort_job_complete = NULL;
    dps_cbs.continue_job_complete = NULL;
    dps_cbs.get_capability_complete = pictbridge_get_cap_complete;
    dps_cbs.get_job_status_complete = pictbridge_job_stat_complete;
    dps_cbs.get_device_status_complete = pictbridge_dev_stat_complete;

    /* Print client event callbacks */
    dps_cbs.notify_job_status_h = pictbridge_handle_notify_job_status;
    dps_cbs.notify_device_status_h = pictbridge_handle_notify_device_status;

    /* DPOF handler */
    dps_cbs.get_file_id_h = pictbridge_get_file_id;


    /* The storage server operations were removed from DPS app.  There are now
       implemented as ptp object services in sicd app.  */


    /* Initialize the DPS layer */
    rc = dps_init(insth, &dps_cbs, (dps_appctx_t)&app_ctx, &app_ctx.dpsh);
    if (rc)
    {
        HSU_MSG_ERROR("dps_app_init: failed to register DPS (%d)", rc, 0,0);
    }

    return rc;
}


