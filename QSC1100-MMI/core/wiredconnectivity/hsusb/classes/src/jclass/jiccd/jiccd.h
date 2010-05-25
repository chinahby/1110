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

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.   http://www/jungo/com */
#ifndef _ICCD_H_
#define _ICCD_H_

#include <jos.h>
#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>
#include <usbdi_util.h>
#include <usb_quirks.h>
#include <uw_host_iccd.h>

/* Values of bRequest */
#define UWH_ICCD_HOST_TO_RDR_BREQ_ICC_POWER_ON 0x62
#define UWH_ICCD_HOST_TO_RDR_BREQ_ICC_POWER_OFF 0x63
#define UWH_ICCD_HOST_TO_RDR_BREQ_XFER_BLOCK 0x65
#define UWH_ICCD_RDD_TO_HOST_BREQ_DATA_BLOCK 0x6F
#define UWH_ICCD_RDR_TO_HOST_BREQ_GET_STATUS 0xA0

/* Values of bMessageType */
#define UWH_ICCD_HOST_TO_RDR_BTYPE_ICC_POWER_ON 0x62
#define UWH_ICCD_HOST_TO_RDR_BTYPE_ICC_POWER_OFF 0x63
#define UWH_ICCD_HOST_TO_RDR_BTYPE_XFER_BLOCK 0x6F
#define UWH_ICCD_RDD_TO_HOST_BTYPE_DATA_BLOCK 0x80
#define UWH_ICCD_RDR_TO_HOST_BTYPE_SLOT_STATUS 0x81

struct jiccd_softc;

typedef struct
{
    usbd_xfer_handle xfer;
    uwh_iccd_cb cb;
    void *arg;
    struct jiccd_softc *sc;
} iccd_xfer_t;

typedef struct jiccd_softc {
    uwh_iccd_app_h os_ctx;
    juint8_t protocol;

    usbd_interface_handle iface;
    usbd_device_handle sc_udev;
    jdevice_t dev;

    juint8_t bulkin;  /* bulk-in Endpoint Address */
    juint8_t bulkout; /* bulk-out Endpoint Address */
    juint8_t intrin;  /* intr-in Endp. (CBI) */
    usbd_pipe_handle bulkin_pipe;
    usbd_pipe_handle bulkout_pipe;
    usbd_pipe_handle intrin_pipe;

    usb_device_request_t request;
    juint32_t timeout;

    iccd_xfer_t *ctrl_xfer;
    jbool_t pending_ctrl_req;

    uwh_iccd_descriptor_t iccd_info;
#define ICCD_STATE_DETACHED 0
#define ICCD_STATE_ATTACHED 1
#define ICCD_STATE_SUSPEND 2
    juint8_t state;
} jiccd_softc_t;

jresult_t jiccd_suspend(jdevice_t self);
jresult_t jiccd_resume(jdevice_t self);
    
/* pack/unpack macros */
#define PACK_BYTE(buffer, pos, val) (buffer)[(pos)++] = (val)

#define PACK_WORD(buffer, pos, val) \
    do { \
        ua_htole16(&(buffer)[(pos)], (val)); \
        (pos) += 2; \
    } while (0)

#define PACK_DWORD(buffer, pos, val) \
    do { \
        ua_htole32(&(buffer)[(pos)], (val)); \
        (pos) += 4; \
    } while (0)

#define PACK_BYTE_ARRAY(buffer, pos, from, size) \
    do { \
        if ((size)) \
        { \
            j_memcpy((buffer) + (pos), (void *)(from), (size)); \
            (pos) += (size); \
        } \
    } while(0)

#define UNPACK_BYTE(buf, pos, to) (to) = (buf)[(pos)++];

#define UNPACK_WORD(buf, pos, to) \
    do { \
        (to) = ua_le16toh((buf) + (pos)); \
        pos += 2; \
    } while(0)

#define UNPACK_DWORD(buf, pos, to) \
    do { \
        (to) = ua_le32toh((buf) + (pos)); \
        pos += 4; \
    } while(0)

#define UNPACK_BYTE_ARRAY(buffer, pos, to, size) \
    do { \
        if ((size)) \
        { \
            j_memcpy((to), (void *)((buffer) + (pos)), (size)); \
            (pos) += (size); \
        } \
    } while(0)

#endif

