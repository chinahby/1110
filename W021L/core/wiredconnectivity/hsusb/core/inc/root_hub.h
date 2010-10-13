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
#ifndef _ROOT_HUB_H_
#define _ROOT_HUB_H_

#include <jusb.h>
#include <usbdi.h>
#include <usbdivar.h>

/* Root hub intr endpoint number */
#define HCD_INTR_ENDPT  1

/* Root hub descriptor */
typedef struct root_hub_desc_s {
    juint_t pwr_on_2_pwr_good            : 8;
    juint_t noport                       : 8;
    juint_t speed                        : 3;
    juint_t self_powered                 : 1;
    juint_t port_indication              : 1;
    juint_t remote_wakeup                : 1;
} root_hub_desc_t;

/* Host Controller Driver (HCD) root hub callback methods */
struct root_hub_methods {
    /* Handle a port change request from the hub driver */
    usbd_status (*port_do_change)(void *context, juint8_t index, jint_t value,
        jbool_t set);

    /* Get port status */
    juint16_t (*port_get_status)(void *context, juint8_t index);

    /* Get port change status */
    juint16_t (*port_get_change)(void *context, juint8_t index);

    /* Enable/Disable port change interrupt */
    jint_t (*port_enable_intr)(void *context, jbool_t enable);

#ifdef CONFIG_PORT_INTERCHIP
    jbool_t (*is_port_interchip)(void *context, juint8_t index);
#endif
};

typedef struct root_hub_softc {
    juint16_t conf;              /* Selected configuration */
    juint8_t addr;               /* Device address */
    usbd_xfer_handle xfer;      /* xfer of hub interrupt pipe */
    jtask_h intr_enable;        /* Callout for port change interrupt enable */
    struct root_hub_methods *methods;
    root_hub_desc_t *desc;
    void *context;              /* Controller context */
} root_hub_softc_t;

/* Notify root hub of a port change */
void root_hub_port_change(root_hub_softc_t *sc);

/* Open a pipe to the root hub */
usbd_status root_hub_open_pipe(usbd_pipe_handle pipe);

/* Get root hub device address */
juint8_t root_hub_get_address(root_hub_softc_t *sc);

void root_hub_uninit(root_hub_softc_t *sc);
void root_hub_init(root_hub_softc_t *sc, struct root_hub_methods *methods,
    root_hub_desc_t *desc, void *context);

#endif

