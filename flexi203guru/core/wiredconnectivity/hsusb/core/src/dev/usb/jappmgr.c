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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */
#include <jos.h>
#include <jusb.h>
#include <usbdi.h>
#include <uw_host_appmgr.h>
#include "jappmgr.h"

typedef struct {
    TAILQ_HEAD(app_list, app_entry) apps;
} appmgr_t;

typedef struct app_entry {
    TAILQ_ENTRY(app_entry) next;
    app_cbs_h cbs;
    uwh_app_probe_cb_t probe;
} app_entry_t;

/**
 * Function name:  appmgr_add
 * Description:    Adds application callbacks to application manager.
 *                 On first call creates application manager instance.
 * Parameters:
 *     @appmgr: (IN/OUT) Pointer to application manager handle.
 *     @cbs:    (IN) Pointer to application callbacks structure.
 *     @probe:  (IN) Application probe callback.
 *
 * Return value:   Application handle, or NULL if failed.
 * Scope:          Global
 **/
uwh_app_h appmgr_add(appmgr_h *appmgr, app_cbs_h cbs, uwh_app_probe_cb_t probe)
{
    app_entry_t *app = NULL;
    appmgr_t *mgr;
    DECLARE_FNAME("appmgr_add");

    KASSERT(appmgr, ("appmgr is NULL\n"));

    if (!*appmgr)
    {
        DBG_X(DUTIL_APPMGR, ("%s: first application\n", fname));

        mgr = (appmgr_t *)jmalloc(sizeof(appmgr_t), 0);
        if (!mgr)
            return NULL;

        TAILQ_INIT(&mgr->apps);
        *appmgr = mgr;
    }
    else
    {
        mgr = *appmgr;
    }

    app = (app_entry_t *)jmalloc(sizeof(app_entry_t), 0);
    if (!app)
        goto Error;

    app->cbs = cbs;
    app->probe = probe;

    TAILQ_INSERT_TAIL(&mgr->apps, app, next);

    DBG_I(DUTIL_APPMGR, ("%s: added application %p\n", fname, app));
    return app;
    
Error:
    DBG_I(DUTIL_APPMGR, ("%s: failed - NO MEMORY\n", fname));

    if (!TAILQ_FIRST(&mgr->apps))    
    {
        jfree(mgr);
        *appmgr = NULL;
    }
    return NULL;
}

/**
 * Function name:  appmgr_remove
 * Description:    Removes application callbacks from application manager.
 *                 For last application destroys application manager instance.
 * Parameters:
 *     @appmgr: (IN/OUT) Pointer to application manager handle.
 *     @app_h:  (IN)     Application handle.
 *
 * Return value:   None
 * Scope:          Global
 **/
void appmgr_remove(appmgr_h *appmgr, uwh_app_h app_h)
{
    appmgr_t *mgr;
    jbool_t removed = 0;
    app_entry_t *app = NULL;
    DECLARE_FNAME("appmgr_remove");

    KASSERT(appmgr, ("appmgr is NULL\n"));
    KASSERT(app_h, ("app_h is NULL\n"));
    
    mgr = (appmgr_t *)*appmgr;
    app = (app_entry_t *)app_h;
    DBG_I(DUTIL_APPMGR, ("%s: %p\n", fname, app_h));

    TAILQ_FIND_AND_REMOVE(&mgr->apps, app, app_entry_t *, next, removed);

    if (!removed)
    {
        DBG_E(DUTIL_APPMGR, ("%s: failed, can't find %p\n", fname, app_h));
        return;
    }

    jfree(app);
    
    if (!TAILQ_FIRST(&mgr->apps))    
    {
        DBG_X(DUTIL_APPMGR, ("%s: last application\n", fname));
        jfree(mgr);
        *appmgr = NULL;
    }
}

/**
 * Function name:  appmgr_probe
 * Description:    Calls probe methods of applications registered with class
 *                 driver and returns callbacks of application with highest 
 *                 matching value.
 * Parameters:
 *     @appmgr: (IN) Application manager handle.
 *     @dev:    (IN) Device handle.
 *
 * Return value:   Pointer to application callbacks or NULL if all applications 
 *                 returned UMATCH_NONE.
 * Scope:          Global
 **/
app_cbs_h appmgr_probe(appmgr_h appmgr, mdev_t dev)
{
    devmon_info_t desc;
    appmgr_t *mgr;
    app_entry_t *best_app = NULL, *app;
    juint_t best_match = UMATCH_NONE;
    DECLARE_FNAME("appmgr_probe");

    KASSERT(appmgr, ("appmgr is NULL\n"));
    
    mgr = (appmgr_t *)appmgr;

    devmon_get_info(dev, &desc);
    
    TAILQ_FOREACH(app, &mgr->apps, next)
    {
        juint_t cur_match;

        if (app->probe)
            cur_match = app->probe(&desc);
        else
            cur_match = UMATCH_GENERIC;

        DBG_X(DUTIL_APPMGR, ("%s: app %p match returned %d\n", fname, app, 
            cur_match));

        if (cur_match > best_match)
        {
            best_match = cur_match;
            best_app = app;
        }
    }

    if (best_app)
        return best_app->cbs;

    DBG_E(DUTIL_APPMGR, ("%s: failed, no matching application found\n", fname));
    return NULL;
}

