/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 












This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.











************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

#include <uw_device_sicd.h>
#include "sicd_sample.h"

#define TEST_DPOF                       0
#define TEST_DPOF_RESTART               1
#define TEST_PRINT_MULTIPLE_IMAGES      0
#define TEST_NUMBER_OF_COPIES           1
#define TEST_DPS_QUALITY                DPS_QUALITY_DEFAULT
#define TEST_DPS_PAPER_SIZE             DPS_PAPER_SIZE_DEFAULT 
#define TEST_DPS_PAPER_TYPE             DPS_PAPER_TYPE_DEFAULT
#define TEST_DPS_FILE_TYPE              DPS_FILE_TYPE_DEFAULT
#define TEST_DPS_DATE_PRINT             DPS_DATE_PRINT_DEFAULT
#define TEST_DPS_FILENAME_PRINT         DPS_FILENAME_PRINT_DEFAULT
#define TEST_DPS_IMAGE_OPTIMIZE         DPS_IMAGE_OPTIMIZE_DEFAULT
#define TEST_DPS_LAYOUT                 DPS_LAYOUT_DEFAULT
#define TEST_DPS_FIXED_SIZE             DPS_FIXED_SIZE_DEFAULT
#define TEST_DPS_CROPPING               DPS_CROPPING_OFF
#define TEST_DPS_CROPPING_LEFT          200
#define TEST_DPS_CROPPING_TOP           70
#define TEST_DPS_CROPPING_WIDTH         160
#define TEST_DPS_CROPPING_HEIGHT        180


#define DATE "01/01/2007"

static juint8_t jpg_1[] =
{
#include "jungo.hex"
};

static juint8_t thumb_1[] =
{
#include "jungo_thumb.hex"
};

#if TEST_PRINT_MULTIPLE_IMAGES
#error code not present
#endif


static struct
{
    juint8_t *image;
    juint32_t image_size;
    juint8_t *thumb;
    juint32_t thumb_size;
    char *name;
    ptp_obj_handle_t id;

} sample_image[] =
{
    { jpg_1, sizeof(jpg_1), thumb_1, sizeof(thumb_1), "jungo1.jpg", 0 },
#if TEST_PRINT_MULTIPLE_IMAGES
#error code not present
#endif
    { NULL, 0, NULL, 0, NULL, 0 },
};

struct
{
    dps_handle_t dpsh;
    dps_req_conf_t config_req;
    dps_capability_t current_cap;
    dps_job_config_t jconfig;
    ptp_storage_id_t store_id;
    ptp_obj_handle_t dpof_file_id;
    jbool_t can_restart;
    dpof_params_t dpof_restart_info;
} app_ctx;

static jint_t handle_printer_connect(dps_appctx_t appctx)
{
    DECLARE_FNAME("handle_printer_connect");
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    return 1;           /* WE WANT PICTBRIDGE */
}

static void handle_printer_established(dps_appctx_t appctx)
{
    app_ctx.config_req.versions_count = 1;
    app_ctx.config_req.dps_version[0].major = 1;
    app_ctx.config_req.dps_version[0].minor = 0;
    j_strlcpy(app_ctx.config_req.vendor_name, "Acme Inc", 
        DPS_MAX_STRING_SIZE + 1);
    j_strlcpy(app_ctx.config_req.product_name, "RoadRunner",
        DPS_MAX_STRING_SIZE + 1);
    j_strlcpy(app_ctx.config_req.serial_no, "MIPMIP", DPS_MAX_STRING_SIZE + 1);
    app_ctx.config_req.vendor_specific_version.major = 1;
    app_ctx.config_req.vendor_specific_version.minor = 0;

    dps_configure_print_service(app_ctx.dpsh, &app_ctx.config_req);
}

static void config_complete(dps_appctx_t appctx,
    dps_param_t result, dps_res_conf_t *printer_cfg)
{
    int i;
    dps_req_conf_t *cfg;
    DECLARE_FNAME("config_complete");
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    if (!result || !printer_cfg)
    {
        DBG_E(DSLAVE_DPS_API, ("%s: failed to parse printer configuration (%d)"
            "\n", fname, result));
        return;
    }

    DBG_I(DSLAVE_DPS_API, ("%s: result=%X\n", fname, result));
    DBG_I(DSLAVE_DPS_API, ("%s: service_available=%X\n", fname,
        printer_cfg->service_available));

    cfg = &printer_cfg->conf;

    DBG_I(DSLAVE_DPS_API, ("%s: version_count=%u\n", fname, cfg->versions_count));
    for(i=0; i<cfg->versions_count; i++)
    {
        DBG_I(DSLAVE_DPS_API, ("%s: version=%d.%d\n", fname,
            cfg->dps_version[i].major,
            cfg->dps_version[i].minor));
    }
    DBG_I(DSLAVE_DPS_API, ("%s: vendor_name=%s\n", fname, cfg->vendor_name));
    DBG_I(DSLAVE_DPS_API, ("%s: ven_spec_ver=%d.%d\n", fname,
        cfg->vendor_specific_version.major,
        cfg->vendor_specific_version.minor));
    DBG_I(DSLAVE_DPS_API, ("%s: product_name=%s\n", fname, cfg->product_name));
    DBG_I(DSLAVE_DPS_API, ("%s: serial_no=%s\n", fname, cfg->serial_no));
}

/*
 *   The code below (up to the execute_job() function) is used to parse a
 *   minimal DPOF file in order to test the feature. In practice, properly
 *   generated DPOF files should be used with dps_start_job()
 */

static const char *DPOF_HDR_TEMPLATE =
"[HDR]\r\n"
"GEN REV = 01.00\r\n"
"GEN CRT = \"JuMP camera\"\r\n"
"%s"
;

static const char *DPOF_JOB_TEMPLATE = 
"%s"
"[JOB]\r\n"
"PRT PID = %03u\r\n"
"PRT TYP = STD\r\n"
"PRT QTY = %03u\r\n"
"IMG FMT = EXIF2 -J\r\n"
"<IMG SRC = \"%s%s\">\r\n"
;

#define MAX_DPOF_FILE_SIZE (10*1024)
static const char *DCF_PREFIX = "../DCIM/";

static void to_upper(char *str)
{
    char diff = 'a' - 'A';
    
    while(*str)
    {
        if (*str >= 'a' && *str <= 'z')
            *str = *str - diff;
        str++;
    }
}

static void execute_dpof_job(void)
{
    char *dpof_file = NULL;
    char *dpof_jobs = NULL;
    sicd_object_t *obj = NULL;
    jint_t i;
    dps_job_config_t jconfig;
    dps_job_info_t jinfo;
    char *filename = NULL;
    DECLARE_FNAME("execute_dpof_job");

    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    dpof_file = (char*)jmalloc(MAX_DPOF_FILE_SIZE, M_ZERO);
    dpof_jobs = (char*)jmalloc(MAX_DPOF_FILE_SIZE, M_ZERO);
    if (!dpof_file || !dpof_jobs)
    {
        DBG_E(DSLAVE_DPS_API, ("%s: failed to allocate job info\n", fname));
        goto Error;
    }

    /* Generate JOB sections */
    *dpof_jobs = '\0';
    for (i=0; sample_image[i].image; i++)
    {
        if (!sicd_store_find_object(app_ctx.store_id, sample_image[i].id))
        {
            DBG_E(DSLAVE_DPS_API, ("%s: could not find picture object "
                "(%X) in store\n", filename, sample_image[i].id));
            goto Error;
        }

        filename = j_strdup(sample_image[i].name);
        if (!filename)
        {
            DBG_E(DSLAVE_DPS_API, ("%s: error allocating memory (strdup)."
                "skipping %s\n", fname, sample_image[i].name));
            continue;
        }

        to_upper(filename);
        
        /* TODO: In a real system, the file name given here should be
         * the image's path relative to the DPOF file */
        j_snprintf(dpof_jobs, MAX_DPOF_FILE_SIZE, DPOF_JOB_TEMPLATE,
            dpof_jobs, i+1, TEST_NUMBER_OF_COPIES, DCF_PREFIX,
            filename);

        jfree(filename);
    }

    dpof_jobs[MAX_DPOF_FILE_SIZE-1] = '\0';

    /* Generate header */
    j_snprintf(dpof_file, MAX_DPOF_FILE_SIZE, DPOF_HDR_TEMPLATE, dpof_jobs);
    dpof_file[MAX_DPOF_FILE_SIZE-1] = '\0';

    DBG_X(DSLAVE_DPS_API, ("%s: DPOF file: \n%s\n\n", fname, dpof_file));

    /*
     *   DPOF file generation ends here. Proper API sample code
     *   for DPOF printing begins here.
     */
    
    /* Add DPOF virtual file to store */
    obj = jmalloc(sizeof(sicd_object_t), M_ZERO);
    if (!obj)
    {
        DBG_E(DSLAVE_DPS_API, ("%s: failed to allocate sicd_object_t "
            "for sample image %u\n", fname, i+1));
        goto Error;
    }

    app_ctx.dpof_file_id = obj->id = sicd_get_new_obj_handle();
    obj->info.storage_id = app_ctx.store_id;
    obj->info.format = PTP_OBJ_FORMAT_DPOF;
    obj->info.compressed_size = j_strlen(dpof_file)+1;
    obj->info.filename = ptp_string_atow("/MISC/AUTPRINT.MRK");
    obj->data = (juint8_t*)dpof_file;

    if (sicd_store_add_object(app_ctx.store_id, obj))
    {
        DBG_E(DSLAVE_DPS_API, ("generate_dpof: failed on "
            "sicd_store_add_object (%u)\n"));
        goto Error;
    }

    /* Create JobInfo for DPOF file */
    j_memset(&jconfig, 0, sizeof(jconfig));
    jconfig.file_type = DPS_FILE_TYPE_DPOF;

    j_memset(&jinfo, 0, sizeof(jinfo));
    jinfo.file_id = obj->id;
    j_strlcpy(jinfo.file_name, "/MISC/AUTPRINT.MRK", DPS_FILENAME_SIZE + 1);

    /* If we are testing DPOF restart, add relevant parameters */
    if (TEST_DPOF_RESTART && app_ctx.can_restart)
    {
        j_memcpy(&jinfo.dpof_restart, &app_ctx.dpof_restart_info,
            sizeof(dpof_params_t));
    }

    /* Send Job */
    dps_start_job(app_ctx.dpsh, &jconfig, &jinfo, 1);
    goto Exit;

Error:
    if (obj)
        sicd_free_object(NULL, obj);
    else if (dpof_file)
        jfree(dpof_file);
Exit:
    if (dpof_jobs)
        jfree(dpof_jobs);
}

static void execute_job(void)
{
    static int once = 0;
    static jbool_t test_dpof = TEST_DPOF;
    dps_job_info_t *jinfo;
    sicd_object_t *obj;
    juint_t i;

    if (once)
    {
        once = 1;
        return;
    }

    if (test_dpof)
    {
        execute_dpof_job();
        return;
    }
    
    for (i=0; sample_image[i].image; i++)
        ;
    
    jinfo = (dps_job_info_t*)jmalloc(i*sizeof(dps_job_info_t), M_ZERO);
    if (!jinfo)
    {
        DBG_E(DSLAVE_DPS_API, ("execute_job: failed to allocate job info "
            "array\n"));
        return;
    }

    for (i=0; sample_image[i].image; i++)
    {
        obj = sicd_store_find_object(app_ctx.store_id, sample_image[i].id);
        if (!obj)
        {
            DBG_E(DSLAVE_DPS_API, ("execute_job: could not find picture"
                "object (%X) in store\n", sample_image[i].id));
            return;
        }

        jinfo[i].file_id = obj->id;

        if (TEST_DPS_CROPPING != DPS_CROPPING_OFF)
        {
            jinfo[i].cropping_area.left = TEST_DPS_CROPPING_LEFT;
            jinfo[i].cropping_area.top = TEST_DPS_CROPPING_TOP;
            jinfo[i].cropping_area.width = TEST_DPS_CROPPING_WIDTH;
            jinfo[i].cropping_area.height = TEST_DPS_CROPPING_HEIGHT;
        }

        j_strlcpy(jinfo[i].file_name, sample_image[i].name,
            DPS_FILENAME_SIZE + 1);
        j_strlcpy(jinfo[i].date, DATE, DPS_DATE_SIZE + 1);
        jinfo[i].copies = TEST_NUMBER_OF_COPIES;
    }

    dps_start_job(app_ctx.dpsh, &app_ctx.jconfig, jinfo, i);
    jfree(jinfo);
}

void get_cap_complete(dps_appctx_t appctx, dps_param_t result,
    dps_cap_array_t *cap_list)
{
    static char dbg_buf[1000];
    jint_t i, j;
    dps_param_t conf_setting = 0;
    static struct
    {
        char *cap_str;
        dps_param_t *conf_param;
        dps_param_t test_value;
        dps_param_t default_value;
    } cap_map[] = {
        { "print qualities", &app_ctx.jconfig.quality,
            TEST_DPS_QUALITY, DPS_QUALITY_DEFAULT },
        { "paper sizes", &app_ctx.jconfig.paper_size,
            TEST_DPS_PAPER_SIZE, DPS_PAPER_SIZE_DEFAULT },
        { "paper types", &app_ctx.jconfig.paper_type,
            TEST_DPS_PAPER_TYPE, DPS_PAPER_TYPE_DEFAULT },
        { "file types", &app_ctx.jconfig.file_type,
            TEST_DPS_FILE_TYPE, DPS_FILE_TYPE_DEFAULT },
        { "date prints", &app_ctx.jconfig.date_print,
            TEST_DPS_DATE_PRINT, DPS_DATE_PRINT_DEFAULT },
        { "filename prints",
            &app_ctx.jconfig.filename_print, TEST_DPS_FILENAME_PRINT,
            DPS_FILENAME_PRINT_DEFAULT },
        { "image optimizations",
            &app_ctx.jconfig.image_optimize, TEST_DPS_IMAGE_OPTIMIZE,
            DPS_IMAGE_OPTIMIZE_DEFAULT },
        { "layouts", &app_ctx.jconfig.layout,
            TEST_DPS_LAYOUT, DPS_LAYOUT_DEFAULT },
        { "fixed paper sizes",
            &app_ctx.jconfig.fixed_size, TEST_DPS_FIXED_SIZE,
            DPS_FIXED_SIZE_DEFAULT },
        { "croppings", &app_ctx.jconfig.cropping,
            TEST_DPS_CROPPING, DPS_CROPPING_DEFAULT },
    };
    
    DECLARE_FNAME("get_cap_complete");
    
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    if (result!=DPS_RESULT_OK || !cap_list || !cap_list->count ||
        app_ctx.current_cap > DPS_CAP_CROPPINGS)
    {
        goto NextCapability;
    }

    /* Print out supported parameters */
    *dbg_buf = 0;
    i = app_ctx.current_cap;
    for (j=0; j<cap_list->count; j++)
    {
        j_snprintf(dbg_buf, sizeof(dbg_buf)-1, "%s 0x%X", dbg_buf,
            cap_list->capability[j]);

        if (cap_map[i].test_value == cap_list->capability[j])
            conf_setting = cap_map[i].test_value;
    }

    DBG_I(DSLAVE_DPS_API, ("%s: Supported %s: %s\n", fname,
        cap_map[app_ctx.current_cap].cap_str, dbg_buf));

    /* Set print config parameter */
    if (conf_setting)
    {
        *(cap_map[i].conf_param) = conf_setting;
        DBG_I(DSLAVE_DPS_API, ("%s:\t0x%X selected\n", fname, conf_setting));
    }
    else
    {
        *(cap_map[i].conf_param) = cap_map[i].default_value;
        DBG_W(DSLAVE_DPS_API, ("%s:\t0x%X is not supported by the printer."
            " using default (0x%X)\n", fname,
            cap_map[i].test_value, cap_map[i].default_value));
    }

NextCapability:

    if (app_ctx.current_cap >= DPS_CAP_CROPPINGS)
    {
        /* Done with capabilities - send a print job */
        execute_job();
    }
    else
    {
        /* Get more capabilities from printer */
        app_ctx.current_cap++;
        dps_get_capability(app_ctx.dpsh, app_ctx.current_cap,
            app_ctx.jconfig.paper_size);
    }
}

static void handle_printer_disconnect(dps_appctx_t appctx)
{
    app_ctx.current_cap = DPS_CAP_QUALITIES;
}

static void job_stat_complete(dps_appctx_t appctx, dps_param_t result,
    dps_job_status_t *status)
{
    DECLARE_FNAME("job_stat_complete");
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    if (!status)
    {
        DBG_E(DSLAVE_DPS_API, ("%s: failed to parse job status (%d)\n",
            fname, result));
        return;
    }

    if (result)
    {
        DBG_I(DSLAVE_DPS_API, ("%s: result: %X\n", fname, result));
    }

    DBG_I(DSLAVE_DPS_API, ("%s: DPOF: %s\n", fname,
        status->job_is_dpof?"yes":"no"));

    DBG_I(DSLAVE_DPS_API, ("%s: Progress: %u/%u\n", fname,
        status->pages_done, status->total_pages));

    if (status->job_is_dpof)
    {
        j_memcpy(&app_ctx.dpof_restart_info, &status->dpof_params,
            sizeof(dpof_params_t));
        app_ctx.can_restart = 1;
        DBG_I(DSLAVE_DPS_API, ("%s: print PID: %u\n", fname,
            status->dpof_params.prt_pid));
        DBG_I(DSLAVE_DPS_API, ("%s: file: %s\n", fname,
            status->dpof_params.file_path));
        DBG_I(DSLAVE_DPS_API, ("%s: page number: %u\n", fname,
            status->dpof_params.copy_id));
    }
    else
    {        
#ifdef JDEBUG
        DBG_I(DSLAVE_DPS_API, ("%s: Images printed: %u\n", fname,
            status->images_printed));
#endif
    }
}

static void dev_stat_complete(dps_appctx_t appctx, dps_param_t result,
    dps_device_status_t *status)
{
    DECLARE_FNAME("dev_stat_complete");
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    if (!status)
    {
        DBG_E(DSLAVE_DPS_API, ("%s: failed to parse device status (%d)\n",
            fname, result));
        return;
    }

    if (result)
    {
        DBG_I(DSLAVE_DPS_API, ("%s: result: %X\n", fname, result));
    }

    if (app_ctx.dpof_file_id &&
        status->job_end_reason != DPS_DEV_STAT_JOB_NOT_ENDED)
    {
        sicd_store_remove_object(app_ctx.store_id, app_ctx.dpof_file_id);
        app_ctx.dpof_file_id = 0;
        j_memset(&app_ctx.dpof_restart_info, 0, sizeof(dpof_params_t));
        app_ctx.can_restart = 0;
    }

    DBG_I(DSLAVE_DPS_API, ("%s: print service status: %X\n", fname,
        status->print_service_status));
    DBG_I(DSLAVE_DPS_API, ("%s: job end reason: %X\n", fname,
        status->job_end_reason));
    DBG_I(DSLAVE_DPS_API, ("%s: error status: %X\n", fname,
        status->error_status));
    DBG_I(DSLAVE_DPS_API, ("%s: error reason: %X\n", fname,
        status->error_reason));
    DBG_I(DSLAVE_DPS_API, ("%s: disconnect enabled: %X\n", fname,
        status->disconnect_enable));
    DBG_I(DSLAVE_DPS_API, ("%s: capability changed: %X\n", fname,
        status->capability_changed));
    DBG_I(DSLAVE_DPS_API, ("%s: new job OK: %X\n", fname,
        status->new_job_ok));
}

static dps_param_t handle_notify_job_status(dps_appctx_t appctx,
    dps_job_status_t *status)
{
    DECLARE_FNAME("handle_notify_job_status");
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    job_stat_complete(appctx, 0, status);
    return DPS_RESULT_OK;
}

static dps_param_t handle_notify_device_status(dps_appctx_t appctx,
    dps_device_status_t *status)
{
    DECLARE_FNAME("handle_notify_device_status");
    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    dev_stat_complete(appctx, 0, status);

    if (app_ctx.current_cap < DPS_CAP_CROPPINGS)
        dps_get_capability(app_ctx.dpsh, DPS_CAP_QUALITIES,
            DPS_PAPER_SIZE_DEFAULT);

    return DPS_RESULT_OK;
}

static dps_param_t handle_get_file_id(dps_appctx_t appctx,
    ptp_obj_handle_t autprint_mrk_file_id, char *dcf_file_path,
    ptp_obj_handle_t *id)
{
    jint_t i;
    char *filename = NULL;
    dps_param_t rc;
    DECLARE_FNAME("handle_get_file_id");

    DBG_X(DSLAVE_DPS_API, ("%s: entered\n", fname));

    *id = 0;
    rc = DPS_PARAM_MAKE(DPS_RESULT_UNSUPPORTED_OP,
            DPS_RESULT_MIN_INVALID_VALUE);
    
    for (i=0; sample_image[i].image; i++)
    {
        if (!sample_image[i].name)
            continue;

        filename = j_strdup(sample_image[i].name);
        if (!filename)
        {
            DBG_E(DSLAVE_DPS_API, ("%s: error allocating memory (strdup)\n",
                fname));
            rc = DPS_RESULT_UNEXECUTED_OP;
            goto Exit;
        }

        to_upper(filename);

        /* TODO: The code that should be implemented here is not a
         * straightforward string comparison.
         * The dcf_file_path is the image's path relative
         * to the DPOF file. In a real system e.g, the image'ss full path
         * would be reconstructed in order to do a full path search
         * for the file */
        if (!j_strcmp(dcf_file_path+sizeof(DCF_PREFIX)-1, filename))
        {
            jfree(filename);
            break;
        }

        jfree(filename);
    }

    if (sample_image[i].image)
    {
        *id = sample_image[0].id;
        rc = DPS_RESULT_OK;
    }

    DBG_X(DSLAVE_DPS_API, ("%s: autprint_id: 0x%X filename: %s"
        " img_id: 0x%X\n", fname, autprint_mrk_file_id, dcf_file_path, *id));

Exit:
    return rc;
}

static void dps_init_storage(void)
{
    sicd_store_t *store;
    sicd_object_t *obj;
    jint_t i;
    jresult_t rc;

    store = sicd_store_get_first();
    if (!store)
    {
        DBG_W(DSLAVE_DPS_API, ("dps_init_storage: could not find store "
            "to place sample JPG in\n"));
        return;
    }

    app_ctx.store_id = store->id;

    /* Add our sample jpg objects for pictbridge sample tests */
    for (i=0; sample_image[i].image; i++)
    {
        obj = jmalloc(sizeof(sicd_object_t), M_ZERO);
        if (!obj)
        {
            DBG_E(DSLAVE_DPS_API, ("dps_init_storage: failed to allocate "
                " sicd_object_t for sample image %u\n", i+1));
            return;
        }

        sample_image[i].id = obj->id = sicd_get_new_obj_handle();
        obj->info.storage_id = store->id;
        obj->info.format = PTP_OBJ_FORMAT_IMG_JPEG;
        obj->info.compressed_size = sample_image[i].image_size;
        obj->info.thumb_format = PTP_OBJ_FORMAT_IMG_JPEG;
        obj->info.thumb_compressed_size = sample_image[i].thumb_size;
        obj->info.filename = ptp_string_atow(sample_image[i].name);
        obj->data = (juint8_t*)jmalloc(obj->info.compressed_size, 0);
        if (obj->data)
        {
            j_memcpy(obj->data, sample_image[i].image,
                obj->info.compressed_size);
        }
        
        obj->sample_data = (juint8_t*)jmalloc(obj->info.thumb_compressed_size,
            0);
        if (obj->sample_data)
        {
            j_memcpy(obj->sample_data, sample_image[i].thumb,
                obj->info.thumb_compressed_size);
        }
        
        rc = sicd_store_add_object(store->id, obj);
        if (rc)
        {
            DBG_E(DSLAVE_DPS_API, ("dps_init_storage: failed on "
                "sicd_store_add_object (%u)\n", rc));
            sicd_free_object(NULL, obj);
        }
    }
}

jresult_t dps_app_init(ptp_instanceh_t insth)
{
    dps_callbacks_t dps_cbs;
    jresult_t rc = 0;

    j_memset(&app_ctx.jconfig, 0, sizeof(dps_job_config_t));

    dps_init_storage();

    dps_cbs.printer_connect_h = handle_printer_connect;
    dps_cbs.printer_estab_h = handle_printer_established;
    dps_cbs.printer_disconnect_h = handle_printer_disconnect;

    /* Print client action callbacks */
    dps_cbs.config_print_service_complete = config_complete;
    dps_cbs.start_job_complete = NULL;
    dps_cbs.abort_job_complete = NULL;
    dps_cbs.continue_job_complete = NULL;
    dps_cbs.get_capability_complete = get_cap_complete;
    dps_cbs.get_job_status_complete = job_stat_complete;
    dps_cbs.get_device_status_complete = dev_stat_complete;

    /* Print client event callbacks */
    dps_cbs.notify_job_status_h = handle_notify_job_status;
    dps_cbs.notify_device_status_h = handle_notify_device_status;

    dps_cbs.get_file_id_h = handle_get_file_id;

    /* Initialize the DPS layer */
    rc = dps_init(insth, &dps_cbs, (dps_appctx_t)&app_ctx, &app_ctx.dpsh);
    if (rc)
    {
        DBG_E(DSLAVE_SICD, ("dps_app_init: failed to register DPS (%d)\n",
            rc));
    }

    return rc;
}

