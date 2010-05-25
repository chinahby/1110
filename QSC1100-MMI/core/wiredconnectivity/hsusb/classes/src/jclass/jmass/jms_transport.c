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
#include <jutil.h>
#include "umass.h"
#include "jms_transport.h"

#define MAGIC_STATUS 0xBABA
struct transfer_ctx
{
    juint16_t    status;
    juint32_t    data_len;
    juint32_t    actual_data_len;
    juint32_t    residue;
    jmsleep_h   sleep_handle;
};

void jms_transport_complete(void *context, juint32_t residue, juint16_t status)
{
    struct transfer_ctx *tc = (struct transfer_ctx *) context;

    tc->status = (juint16_t)status;

    /* This should never happen, status is 0/1/2/3 as defined in umass.h */
    if (status == MAGIC_STATUS)
        j_panic("TRANS: Status is equal to Magic !\n");

    tc->actual_data_len = tc->data_len - residue;

    tc->residue = residue;

    jwakeup(tc->sleep_handle);
}

jresult_t jms_transfer(void *dev_h, juint8_t lun, 
    juint8_t *cmd, jint_t cmdlen, jint_t is_read, juint32_t timeout, 
    void *data, juint32_t data_len, juint8_t flags, juint32_t *actual_data_len)
{
    struct transfer_ctx tc;
    jresult_t rc;

    tc.status   = MAGIC_STATUS;
    tc.data_len = data_len;
    tc.sleep_handle = NULL;
    rc = jmsleep_init(&tc.sleep_handle);
    if (rc)
    {
        rc = JENOMEM;
        goto Exit;
    }

    rc = umass_transfer(dev_h, lun, cmd, cmdlen, is_read,
        timeout, data, data_len, flags, actual_data_len, (void *)&tc);
    if (rc == STATUS_CMD_UNKNOWN)
    {
        rc = JEINVAL;
        goto Exit;
    }
    else if (rc)
    {
        rc = JENXIO;
        goto Exit;
    }

    /* Sleep until transport complete callback */
    if (tc.status == MAGIC_STATUS)
        jmsleep(tc.sleep_handle, 0);

    *actual_data_len = tc.actual_data_len;

    switch (tc.status)
    {
    case STATUS_CMD_OK:
        rc = 0;
        break;

    case STATUS_CMD_UNKNOWN:
        rc = JEAGAIN;
        break;

    case STATUS_CMD_FAILED:
        rc = JEIO;
        break;

    case STATUS_WIRE_FAILED:
        rc = JENXIO;
        break;

    case MAGIC_STATUS:
        /* This means transport callback never got called */
        j_panic("TRANS: No callback on command completion !\n");
        rc = JENXIO;
        break;

    default:
        rc = JEINVAL;
    }

    if (tc.residue && !rc)
    {
        DBG_W(DMASS_SCSI, ("TRANS: Command completed with residue %ld "
            "[%ld:%ld]\n", tc.residue, tc.actual_data_len, tc.data_len));
    }

Exit:
    if (tc.sleep_handle)
        jmsleep_uninit(tc.sleep_handle);
    return rc;
}

jresult_t jms_transport_attach(void *sc, void *usbd_dev,juint8_t maxlun, 
    juint8_t subclass, void *user_ctx)
{
    return jms_devmgr_attach(sc, usbd_dev, maxlun, subclass, user_ctx);
}

void jms_transport_detach(void *user_ctx)
{
    jms_devmgr_detach(user_ctx);
}

jresult_t jms_transport_suspend(void *user_ctx)
{
    return jms_devmgr_suspend(user_ctx); 
}

void jms_transport_resume(void *user_ctx)
{
    jms_devmgr_resume(user_ctx);
}

jresult_t jms_transport_suspend_self(void *dev_h)
{
    return umass_suspend_self(dev_h);
}

jresult_t jms_transport_resume_self(void *dev_h)
{
    return umass_resume_self(dev_h);
}

