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

#include <jos.h>
#include <uw_otg_app.h>

#define TEST_HNP 0
#define TEST_WHITE_LIST 0

#define DROP_VBUS_TIMEOUT 60000

#ifdef TEST_WHITE_LIST
#define TEST_MULTIPLE_WHITELIST_RESTRICTIONS 0
/* The following defines relate to the first element of the white list */
#define TEST_WL_VENDOR_ID 0x045E
#define TEST_WL_PRODUCT_ID 0
#define TEST_WL_MIN_BCD_DEVICE 0
#define TEST_WL_MAX_BCD_DEVICE 0
#define TEST_WL_USB_CLASS 0
#define TEST_WL_USB_SUB_CLASS 0
#define TEST_WL_USB_PROTOCOL 0
#endif

typedef struct {
    otg_ctx_t otg_ctx;
    jbool_t dont_init_srp;
} otg_app_t;

/* Declarations of OTG Application Callbacks */
static void notify_peripheral_state(otg_appctx_t appctx, 
    otg_peripheral_state_t state);
static void notify_hnp_state(otg_appctx_t appctx, otg_hnp_state_t state);
static void notify_srp_state(otg_appctx_t appctx, jbool_t is_enabled);
static void notify_error(otg_appctx_t appctx, otg_error_t err);
static void notify_device_mode(otg_appctx_t appctx, otg_device_mode_t mode);
static jresult_t otg_app_uninit(otg_appctx_t appctx);
static void notify_device_connect_status(otg_appctx_t appctx, jbool_t status);
static jbool_t respond_srp(otg_appctx_t appctx);

jresult_t otg_app_init(otg_ctx_t core_ctx, otg_app_callbacks_t *otg_app_cbs, 
    otg_app_info_t *otg_app_info, otg_appctx_t *appctx)
{
    otg_app_t *otg_app;
#if TEST_WHITE_LIST
    
#if TEST_MULTIPLE_WHITELIST_RESTRICTIONS
#define WL_RESTRICTIONS_NUMBER 2
#else 
#define WL_RESTRICTIONS_NUMBER 1
#endif
   
    peripheral_matcher_t whitelist[WL_RESTRICTIONS_NUMBER];
    jresult_t rc;   
#endif 

    /* Initialize the OTG Application's context */
    otg_app = (otg_app_t *)jmalloc(sizeof(otg_app_t), M_ZERO);
    if (!otg_app)
    return JENOMEM;

    /* Store the OTG Core context */
    otg_app->otg_ctx = core_ctx;

    /* Assign the OUT parameters */
    /* Initialize the callbacks struct */
    otg_app_cbs->notify_peripheral_state = notify_peripheral_state;
    otg_app_cbs->notify_hnp_state = notify_hnp_state;
    otg_app_cbs->notify_srp_state = notify_srp_state;
    otg_app_cbs->notify_error = notify_error;
    otg_app_cbs->notify_device_mode = notify_device_mode;
    otg_app_cbs->notify_device_connect_status = notify_device_connect_status;
    otg_app_cbs->respond_srp = respond_srp;
    otg_app_cbs->otg_app_uninit = otg_app_uninit;
    /* Initialize the Application info struct */   
    otg_app_info->drop_vbus_timeout = DROP_VBUS_TIMEOUT;
    /* Initialize the Application context */
    *appctx = otg_app; 

#if TEST_WHITE_LIST
    DBG_E(DJOTG_API, ("TEST_WHITE_LIST: Set White List\n"));
    whitelist[0].flags = 
    ((TEST_WL_VENDOR_ID)      ? PERIPH_MATCH_BY_VENDOR_ID      : 0) |
    ((TEST_WL_PRODUCT_ID)     ? PERIPH_MATCH_BY_PRODUCT_ID     : 0) |
    ((TEST_WL_MIN_BCD_DEVICE) ? PERIPH_MATCH_BY_MIN_BCD_DEVICE : 0) |
    ((TEST_WL_MAX_BCD_DEVICE) ? PERIPH_MATCH_BY_MAX_BCD_DEVICE : 0) |
    ((TEST_WL_USB_CLASS)      ? PERIPH_MATCH_BY_USB_CLASS      : 0) |
    ((TEST_WL_USB_SUB_CLASS)  ? PERIPH_MATCH_BY_USB_SUB_CLASS  : 0) |
    ((TEST_WL_USB_PROTOCOL)   ? PERIPH_MATCH_BY_USB_PROTOCOL   : 0) ;

    whitelist[0].vendor_id = TEST_WL_VENDOR_ID;
    whitelist[0].product_id = TEST_WL_PRODUCT_ID;
    whitelist[0].min_bcd_device = TEST_WL_MIN_BCD_DEVICE;
    whitelist[0].max_bcd_device = TEST_WL_MAX_BCD_DEVICE;
    whitelist[0].usb_class = TEST_WL_USB_CLASS;
    whitelist[0].usb_sub_class = TEST_WL_USB_SUB_CLASS;
    whitelist[0].usb_protocol = TEST_WL_USB_PROTOCOL;

#if TEST_MULTIPLE_WHITELIST_RESTRICTIONS
    /* Change these variables to set the desired restriction */
    whitelist[1].flags = PERIPH_MATCH_BY_PRODUCT_ID;
    whitelist[1].product_id = 0x2202;
#endif

    rc = otg_set_whitelist(core_ctx, WL_RESTRICTIONS_NUMBER, whitelist);
    if (rc)
    {
        DBG_E(DJOTG_API, ("TEST_WHITE_LIST: Set of White List FAILED! "
                                "rc = %d\n", rc));
        return rc;
    }
#endif  
    return 0;
}

static void enable_hnp(otg_app_t *otg_app)
{
    jresult_t rc;
    DECLARE_FNAME("enable_hnp");

    rc = otg_enable_hnp(otg_app->otg_ctx);
    if (rc)
    {
        if (rc == JENOTSUP)
        {
            DBG_W(DJOTG_API, ("%s: Connected device is not HNP capable\n", 
                fname));
        }
        else
        {
            DBG_E(DJOTG_API, ("%s: Failure in enabling HNP on an HNP capable "
                "device - %d\n", fname, rc));
        }
    }
}

static void notify_peripheral_state(otg_appctx_t appctx, 
    otg_peripheral_state_t state)
{
    otg_app_t *otg_app = (otg_app_t *)appctx;
    DECLARE_FNAME("notify_peripheral_state");

    DBG_V(DJOTG_API, ("%s: Connected device's peripheral state is %s\n", fname, 
        state));

    if (state != OTG_PERIPH_SUPPORTED)
    {
        DBG_X(DJOTG_API, ("%s: Connected device is not fully supported, "
            "trying to enable HNP\n", fname));
        enable_hnp(otg_app);
    }
#if TEST_HNP
    else
    {
        DBG_E(DJOTG_API, ("TEST_HNP: Change OTG devices roles\n"));
        enable_hnp(otg_app);        
    }
#endif
}

static void notify_hnp_state(otg_appctx_t appctx, otg_hnp_state_t state)
{
    DECLARE_FNAME("notify_hnp_state");

    DBG_E(DJOTG_API, ("%s: Not yet implemented\n", fname));
}

static void notify_srp_state(otg_appctx_t appctx, jbool_t is_enabled)
{
    otg_app_t *otg_app = (otg_app_t *)appctx;

    DBG_V(DJOTG_API, ("SRP state is %d, will %sstart SRP\n", is_enabled,
        ((!is_enabled || otg_app->dont_init_srp)?"not ":"")));

    if (!otg_app->dont_init_srp && is_enabled)
    {
        otg_app->dont_init_srp = 1;
        otg_start_srp(otg_app->otg_ctx);
    }
}

static void notify_error(otg_appctx_t appctx, otg_error_t err)
{
    otg_app_t *otg_app = (otg_app_t *)appctx;
    DECLARE_FNAME("notify_error");

    DBG_V(DJOTG_API, ("%s: Handling OTG Error - 0x%x\n", fname, err));

    switch (err)
    {
    case OTG_ERROR_SRP_FAILED:
    otg_app->dont_init_srp = 1;
    otg_start_srp(otg_app->otg_ctx);
    break;
    case OTG_ERROR_VBUS:
    otg_vbus_shutdown(otg_app->otg_ctx);
    break;
    default:
    break;
    }
}

static void notify_device_mode(otg_appctx_t appctx, otg_device_mode_t mode)
{
    otg_app_t *otg_app = (otg_app_t *)appctx;
    DECLARE_FNAME("notify_device_mode");

    DBG_V(DJOTG_API, ("%s: Setting Device State - 0x%x\n", fname, mode));

    otg_app->dont_init_srp = (mode == OTG_DEVICE_MODE_PERIPHERAL)?1:0;
}

static void notify_device_connect_status(otg_appctx_t appctx, jbool_t status)
{
    otg_app_t *otg_app = (otg_app_t *)appctx;
    DECLARE_FNAME("notify_device_connect_status");

    if (status == DEVICE_STATUS_CONNECTED)
    {
        jresult_t rc = otg_vbus_powerup(otg_app->otg_ctx);
        if (rc)
            DBG_E(DJOTG_API, ("%s: Failed to powerup VBus - %d\n", fname, rc));
    }
}

static jbool_t respond_srp(otg_appctx_t appctx)
{
    /* Always enable connection when SRP is detected */
    return 1;
}

static jresult_t otg_app_uninit(otg_appctx_t appctx)
{
    otg_app_t *otg_app = (otg_app_t *)appctx;

    if (!otg_app)
        goto Exit;

    jfree(otg_app);

Exit:
    return 0;
}
