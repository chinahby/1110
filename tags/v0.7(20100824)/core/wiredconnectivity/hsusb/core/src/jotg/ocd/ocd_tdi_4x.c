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

/* *  Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com          */
#include <jos.h>
#include <otg_core_ocd.h>

#include "ocd_tdi_4x.h"

#include "hsu_mdm_apis.h"             /* HSU addition */
#include "hsu_os_abstract_bus_ext.h"  /* HSU addition */

/* write / read register */
#define TDI_4X_OTG_WRITE_REG(sc, offset, value) \
    j_bus_space_write_4((sc)->reg_res, offset, value)
#define TDI_4X_OTG_READ_REG(sc, offset) \
    j_bus_space_read_4((sc)->reg_res, offset)

#define TDI_4X_OTG_SET_TIMER_PARAM(sc, time, func) \
    do { \
        jspinlock_lock((sc)->timer_lock);   \
        (sc)->timer_time = (time);          \
        (sc)->timer_func = (func);          \
        if ((time)) \
        { \
            (sc)->intr_msk |= TDI_4X_OTG_SC_1MSIE; \
            TDI_4X_OTG_WRITE_REG((sc), TDI_4X_OTG_SC, \
                TDI_4X_OTG_READ_REG((sc), TDI_4X_OTG_SC) | \
                TDI_4X_OTG_SC_1MSIE); \
        } \
        else \
        { \
            (sc)->intr_msk &= ~TDI_4X_OTG_SC_1MSIE; \
            TDI_4X_OTG_WRITE_REG((sc), TDI_4X_OTG_SC, \
                TDI_4X_OTG_READ_REG((sc), TDI_4X_OTG_SC) & \
                ~TDI_4X_OTG_SC_1MSIE); \
        } \
        jspinlock_unlock((sc)->timer_lock); \
    } while(0)

#ifdef JDEBUG
#define TDI_4X_OTG_ASSERT_CTX(sc) \
    KASSERT(sc, ("%s: Invalid ocd context\n", fname));
#else
#define TDI_4X_OTG_ASSERT_CTX(sc)
#endif

#if CONFIG_BYTE_ORDER == CPU_BIG_ENDIAN
#  define MODE_ENDIAN TDI_4X_MODE_ES
#elif CONFIG_BYTE_ORDER == CPU_LITTLE_ENDIAN
#  define MODE_ENDIAN 0x0
#endif

static jresult_t ocd_enable_srp_detection(ocd_ctx_t ocd_ctx, jbool_t enable);
static jresult_t ocd_drive_vbus(ocd_ctx_t ocd_ctx, jbool_t drive);
static jresult_t ocd_charge_vbus(ocd_ctx_t ocd_ctx, jbool_t charge);
static jresult_t ocd_initiate_srp(ocd_ctx_t ocd_ctx);
static jresult_t ocd_start_timer(ocd_ctx_t ocd_ctx, juint32_t ms);
static jresult_t ocd_stop_timer(ocd_ctx_t ocd_ctx);
static jresult_t ocd_get_state(ocd_ctx_t ocd_ctx, otg_hw_smp_t *hw_smp);
static jresult_t ocd_set_local_dplus(ocd_ctx_t ocd_ctx, jbool_t enable);
static jresult_t ocd_set_local_dminus(ocd_ctx_t ocd_ctx, jbool_t enable);
static jresult_t ocd_invoke_ioctl(ocd_ctx_t ocd_ctx, drv_ioctl_t ioctl,
    void *args);
static void ocd_set_mode(ocd_ctx_t ocd_ctx, jbool_t device);
static void ocd_set_auto_reset(ocd_ctx_t ocd_ctx, jbool_t set);
static jbus_h ocd_get_bus(ocd_ctx_t ocd_ctx);
static jresult_t reset_controller(tdi4x_ocd_sc_t *sc, juint32_t new_mode); /* HSU Addition */

static ocd_ops_t ocd_ops = {
    /* ocd_enable_srp_detection */ ocd_enable_srp_detection,
    /* ocd_drive_vbus */           ocd_drive_vbus,
    /* ocd_charge_vbus */          ocd_charge_vbus,
    /* ocd_initiate_srp */         ocd_initiate_srp,
    /* ocd_start_timer */          ocd_start_timer,
    /* ocd_stop_timer */           ocd_stop_timer,
    /* ocd_get_state */            ocd_get_state,
    /* ocd_set_local_dplus */      ocd_set_local_dplus,
    /* ocd_set_local_dminus */     ocd_set_local_dminus,
    /* ocd_invoke_ioctl */         ocd_invoke_ioctl,
    /* ocd_set_mode */             ocd_set_mode,
    /* ocd_set_auto_reset */       ocd_set_auto_reset,
    /* ocd_get_bus */              ocd_get_bus
};

static jresult_t ocd_enable_srp_detection(ocd_ctx_t ocd_ctx, jbool_t enable)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t tmp_reg;
    DECLARE_FNAME("ocd_enable_srp_detection");

    TDI_4X_OTG_ASSERT_CTX(sc);

    tmp_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);

    if (enable)
    {
        tmp_reg |= TDI_4X_OTG_SC_DPIE;
        sc->intr_msk |= TDI_4X_OTG_SC_DPIE;
    }
    else
    {
        tmp_reg &= ~TDI_4X_OTG_SC_DPIE;
        sc->intr_msk &= ~TDI_4X_OTG_SC_DPIE;
    }

    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, tmp_reg);

    return 0;
}

static jresult_t ocd_drive_vbus(ocd_ctx_t ocd_ctx, jbool_t drive)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t port_reg;

    port_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_PORT_SC_REG);

    if (drive)
        port_reg |= TDI_4X_PORT_SC_PP;
    else
        port_reg &= ~TDI_4X_PORT_SC_PP;

    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_PORT_SC_REG, port_reg);

    return 0;
}

static jresult_t ocd_charge_vbus(ocd_ctx_t ocd_ctx, jbool_t charge)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t tmp_reg;
    DECLARE_FNAME("ocd_charge_vbus");

    TDI_4X_OTG_ASSERT_CTX(sc);

    tmp_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);

    if (charge)
    {
        tmp_reg &= ~TDI_4X_OTG_SC_VD;
        tmp_reg |= TDI_4X_OTG_SC_VC;
    }
    else
    {
        tmp_reg &= ~TDI_4X_OTG_SC_VC;
        tmp_reg |= TDI_4X_OTG_SC_VD;
    }

    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, tmp_reg);

    return 0;
}

static void ocd_end_timer(tdi4x_ocd_sc_t *sc)
{
    ocd_stop_timer(sc);

    if (sc->is_dying)
        return;

    jotg_timer_done(sc->otg_core_ctx);
}

static jresult_t ocd_start_timer(ocd_ctx_t ocd_ctx, juint32_t ms)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    DECLARE_FNAME("ocd_start_timer");

    TDI_4X_OTG_ASSERT_CTX(sc);

    /* Initialize timer parameters */
    TDI_4X_OTG_SET_TIMER_PARAM(sc, ms, ocd_end_timer);

    return 0;
}

static jresult_t ocd_stop_timer(ocd_ctx_t ocd_ctx)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    DECLARE_FNAME("ocd_stop_timer");

    TDI_4X_OTG_ASSERT_CTX(sc);
    
    /* Null timer parameters */
    TDI_4X_OTG_SET_TIMER_PARAM(sc, 0, NULL);

    return 0;
}

static jresult_t ocd_get_state(ocd_ctx_t ocd_ctx, otg_hw_smp_t *hw_smp)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t tmp_reg;
    DECLARE_FNAME("ocd_get_state");

    TDI_4X_OTG_ASSERT_CTX(sc);

    tmp_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);

    hw_smp->id             = (tmp_reg & TDI_4X_OTG_SC_ID)? 1 : 0;
    hw_smp->a_sess_vld     = (tmp_reg & TDI_4X_OTG_SC_ASV)? 1 : 0;
    /* Use A_VBUS_VLD for SRP detection due to HW issues */
    /*hw_smp->a_srp_det      = (tmp_reg & TDI_4X_OTG_SC_DPS)? 1 : 0;*/
    hw_smp->a_srp_det      = (tmp_reg & TDI_4X_OTG_SC_ASV)? 1 : 0;
    hw_smp->a_vbus_vld     = (tmp_reg & TDI_4X_OTG_SC_AVV)? 1 : 0;
    hw_smp->b_se0_srp      = sc->is_se0_srp;
    hw_smp->b_sess_end     = (tmp_reg & TDI_4X_OTG_SC_BSE)? 1 : 0;
    hw_smp->b_sess_vld     = (tmp_reg & TDI_4X_OTG_SC_BSV)? 1 : 0;

    tmp_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_PORT_SC_REG);

    hw_smp->connect        = (tmp_reg & TDI_4X_PORT_SC_CCS)? 1 : 0;
    hw_smp->resume         = (tmp_reg & TDI_4X_PORT_SC_FPR)? 1 : 0;

    DBG_I(DJOTG_OCD, ("%s Dumping HW State: id = 0x%x, a_sess_vld = 0x%x, "
        "a_srp_det = 0x%x, a_vbus_vld = 0x%x, b_se0_srp = 0x%x, "
        "b_sess_end = 0x%x, b_sess_vld = 0x%x, connect = 0x%x, resume = 0x%x\n",
        fname, hw_smp->id, hw_smp->a_sess_vld, hw_smp->a_srp_det,
        hw_smp->a_vbus_vld, hw_smp->b_se0_srp, hw_smp->b_sess_end, 
        hw_smp->b_sess_vld, hw_smp->connect, hw_smp->resume));

    return 0;
}

static jresult_t ocd_set_local_dplus(ocd_ctx_t ocd_ctx, jbool_t enable)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t tmp_reg;
    DECLARE_FNAME("ocd_set_local_dplus");

    TDI_4X_OTG_ASSERT_CTX(sc);

    tmp_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);

    if (enable)
        tmp_reg |= TDI_4X_OTG_SC_DP;
    else
        tmp_reg &= ~TDI_4X_OTG_SC_DP;

    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, tmp_reg);

    return 0;
}

static jresult_t ocd_set_local_dminus(ocd_ctx_t ocd_ctx, jbool_t enable)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t tmp_reg;
    DECLARE_FNAME("ocd_set_local_dminus");

    TDI_4X_OTG_ASSERT_CTX(sc);

    tmp_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);

    if (enable)
        tmp_reg |= TDI_4X_OTG_SC_OT;
    else
        tmp_reg &= ~TDI_4X_OTG_SC_OT;

    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, tmp_reg);

    return 0;
}

static jresult_t ocd_invoke_ioctl(ocd_ctx_t ocd_ctx, drv_ioctl_t ioctl,
    void *args)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    jdevice_t dev;
    jresult_t rc = JEINVAL;
    DECLARE_FNAME("ocd_invoke_ioctl");

    TDI_4X_OTG_ASSERT_CTX(sc);

    /* TODO check if there's anything that needs to be done on the OTG
     * controller when these IOCTLs are invoked */
    switch(ioctl)
    {
        case DRV_IOCTL_DEV_ENABLE:
        case DRV_IOCTL_DEV_DISABLE:
        case DRV_IOCTL_DEV_SET_PARAMS:
            dev = sc->dcd_dev;
            break;
        case DRV_IOCTL_HOST_PORT_ENABLE:
        case DRV_IOCTL_HOST_PORT_DISABLE:
        case DRV_IOCTL_HOST_PORT_SUSPEND:
        case DRV_IOCTL_HOST_PORT_RESUME:
        case DRV_IOCTL_HOST_ENABLE_HNP:
        case DRV_IOCTL_HOST_SET_PARAMS:
            dev = sc->hcd_dev;
            break;
        default:
            DBG_E(DJOTG_OCD, ("%s: Trying to invoke an unknown ioctl\n", 
                fname));
            goto Exit;
    }

    if ((ioctl == DRV_IOCTL_DEV_ENABLE) || 
        (ioctl == DRV_IOCTL_HOST_PORT_ENABLE))
    {
        TDI_4X_OTG_WRITE_REG(sc, TDI_4X_USBINTR_REG, TDI_4X_OTG_READ_REG(sc,
            TDI_4X_USBINTR_REG) & ~TDI_4X_USBINTR_PCD_ENB);
        sc->ack_pcd = 0;
    }

    rc = j_device_ioctl(dev, ioctl, args);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Failure in running IOCTL #%d - %d\n", fname,
            ioctl, rc));
    }
    
    if ((ioctl == DRV_IOCTL_DEV_DISABLE) || 
        (ioctl == DRV_IOCTL_HOST_PORT_DISABLE))
    {    
        sc->ack_pcd = 1;
        TDI_4X_OTG_WRITE_REG(sc, TDI_4X_USBINTR_REG, TDI_4X_USBINTR_PCD_ENB); 
    }

Exit:
    return rc;
}

static void ocd_finalize_srp(tdi4x_ocd_sc_t *sc)
{
    DBG_X(DJOTG_OCD, ("ocd_finalize_srp: Finilizing SRP Protocol\n"));
    
    /* Disable Data Pulsing */
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC,
        TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC) & ~TDI_4X_OTG_SC_DP);

    ocd_stop_timer(sc);
    
    jotg_srp_done(sc->otg_core_ctx);
}

static jresult_t ocd_initiate_srp(ocd_ctx_t ocd_ctx)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;

    DBG_V(DJOTG_OCD, ("ocd_initiate_srp: Starting SRP Protocol\n"));
   
    if (sc->is_dying)
        goto Exit;

    /* Initilize the SRP parameters */
    TDI_4X_OTG_SET_TIMER_PARAM(sc, 7, ocd_finalize_srp);

    /* Write register's new value -  Enable Data Pulsing */
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, 
        TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC) | TDI_4X_OTG_SC_DP);

Exit:
    return 0;
}

static void ocd_set_mode(ocd_ctx_t ocd_ctx, jbool_t device)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t otg_sc;

    /* HSU Addition BEGIN */    
    juint32_t loop_counter = 0;
    juint32_t loop_max = 10;

    /* Reset the controller */
    while (reset_controller(sc, device ? TDI_4X_MODE_CM_DEVICE : TDI_4X_MODE_CM_HOST))
    {
      HSU_ASSERT(loop_counter < loop_max);
      hsu_api_init_phy();
      loop_counter++;
    }
    /* HSU Addition END */
   
    sc->ack_pcd = 1;
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_USBINTR_REG, TDI_4X_USBINTR_PCD_ENB); 

    /* Set/Clear the OTG Termination bit according to mode */
    otg_sc = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);
    if (device)
        otg_sc |= TDI_4X_OTG_SC_OT;
    else
        otg_sc &= ~TDI_4X_OTG_SC_OT;
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, otg_sc);
}

static void ocd_set_auto_reset(ocd_ctx_t ocd_ctx, jbool_t set)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)ocd_ctx;
    juint32_t otg_sc;

    /* Set/Clear the Auto Reset bit */
    otg_sc = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);
    if (set)
        otg_sc |= TDI_4X_OTG_SC_HAAR;
    else
        otg_sc &= ~TDI_4X_OTG_SC_HAAR;

    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, otg_sc);
}

static jbus_h ocd_get_bus(ocd_ctx_t ocd_ctx)
{
    return j_device_get_bus(((tdi4x_ocd_sc_t *)ocd_ctx)->ocd_dev);
}

static void invoke_se0_srp(tdi4x_ocd_sc_t *sc)
{
    DBG_X(DJOTG_OCD, ("Checking SE0 state\n"));

    ocd_stop_timer(sc);
    sc->waiting_for_se0_srp = 0;

    if (TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC) & TDI_4X_OTG_SC_BSE)    
        sc->is_se0_srp = 1;
    else
        sc->is_se0_srp = 0;
}

static void wait_for_se0_srp(tdi4x_ocd_sc_t *sc)
{
    DBG_X(DJOTG_OCD, ("Starting wait for SE0\n"));

    sc->waiting_for_se0_srp = 1;

    TDI_4X_OTG_SET_TIMER_PARAM(sc, 3, invoke_se0_srp);
}

static jint_t ocd_intr(void *arg)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)arg;
    juint32_t otgsc_reg; 
    juint32_t intr_reg;
    jint_t rc = ISR_DONT_CALL_SOFT_INTR;
    DECLARE_FNAME("ocd_intr");
    
    TDI_4X_OTG_ASSERT_CTX(sc);

    otgsc_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC); 

    /* Check for OTG related interrupts */
    intr_reg = otgsc_reg & TDI_4X_OTG_SC_ENB2STS(sc->intr_msk);
    if (intr_reg)
    {
        DBG_X(DJOTG_OCD, ("%s: Got an interrupt - 0x%x\n", fname, intr_reg));
        DBG_X(DJOTG_OCD, ("%s: Status is - 0x%x\n", fname, 
            (otgsc_reg >> 8) & 0x7F));
      

        /* If b_sess_end is false, if waiting for se0_srp then stop waiting,
         * either way, assign 0 to is_se0_srp */
        if (!(otgsc_reg & TDI_4X_OTG_SC_BSE))
        {
            if (sc->waiting_for_se0_srp)
                invoke_se0_srp(sc);
            sc->is_se0_srp = 0;
        }

        if (!sc->is_se0_srp && !sc->waiting_for_se0_srp &&
            (otgsc_reg & TDI_4X_OTG_SC_ID) && (otgsc_reg & TDI_4X_OTG_SC_BSE))
        {
            wait_for_se0_srp(sc);
        }

        if (intr_reg & TDI_4X_OTG_SC_1MSIS)
        {
            sc->timer_time--;
            /* Attention - If timer time reaches 0 after decrement 
             * timer_func is not supposed to be NULL. 
             * Should consider the use of ASSERT */
            if (!sc->timer_time && sc->timer_func)
            {
                sc->timer_func(sc);
                rc = ISR_CALL_SOFT_INTR;
            }
        }        

        TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC,
            TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC) | intr_reg); 
      
        rc = ((rc == ISR_CALL_SOFT_INTR) || (intr_reg & ~TDI_4X_OTG_SC_1MSIS))?
            ISR_CALL_SOFT_INTR : ISR_DONT_CALL_SOFT_INTR;
    }

    /* If port change was detected */
    intr_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_USBSTS_REG);
    if (intr_reg & TDI_4X_USBSTS_PCI)
    {
        /* Call DSR on Connect, Disconnect, Resume */
        intr_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_PORT_SC_REG);
        if (intr_reg & TDI_4X_PORT_SC_NOTIFY)
            rc = ISR_CALL_SOFT_INTR;
        if (sc->ack_pcd)        
            TDI_4X_OTG_WRITE_REG(sc, TDI_4X_USBSTS_REG, TDI_4X_USBSTS_PCI);
    }

    return rc;
}

static void ocd_dsr(void *arg)
{
    tdi4x_ocd_sc_t *sc = (tdi4x_ocd_sc_t *)arg;
    
    if (sc->is_dying)
        return;

    jotg_handle_intr(sc->otg_core_ctx);
}

/* HSU Addition: changed return type from void to jresult_t */
static jresult_t reset_controller(tdi4x_ocd_sc_t *sc, juint32_t new_mode)
{
    /* Reset the controller */
    TDI_4X_OTG_WRITE_REG((sc), TDI_4X_USB_CMD_REG, TDI_4X_USB_CMD_RST); 
    /* Wait until reset is done */
    while (TDI_4X_OTG_READ_REG((sc), TDI_4X_USB_CMD_REG) & 
        TDI_4X_USB_CMD_RST)
        ; 
    /* Set the controller's new mode */
/* 
 * HSU addition - added writing of  (TDI_4X_MODE_SDIS | TDI_4X_MODE_SLOM) as well 
 * when writing to TDI_4X_MODE_REG 
 */
    TDI_4X_OTG_WRITE_REG((sc), TDI_4X_MODE_REG, (new_mode | MODE_ENDIAN | TDI_4X_MODE_SDIS | TDI_4X_MODE_SLOM));

    if (sc->full_speed_only)
    {
        DBG_X(DJOTG_OCD, ("reset_controller: forcing full speed\n"));
        TDI_4X_OTG_WRITE_REG(sc, TDI_4X_PORT_SC_REG,
            TDI_4X_OTG_READ_REG(sc, TDI_4X_PORT_SC_REG) | TDI_4X_PORT_SC_PFSC);
    }

  /* HSU addition BEGIN */
#if defined(FEATURE_HS_USB_INTEGRATED_PHY) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)
  {
    juint8_t  read_value = 0;
    const juint8_t  ulpi_addr = 0x31;  /* USB PHY Config 2/4 Register Address */

    /* 
       Set USB PHY Config 2/4 (addr: 0x31), hsdrvamplitude (bit 3:2) to 
       '11' (binary) in order to boost amplitude to 400mV for integrated PHY
    */
    if ( ! hsu_os_bus_read_from_phy(ulpi_addr, &read_value) )
    {
      return JEIO; /* ULPI PHY read failed */
    }

    if ( ! hsu_os_bus_write_to_phy(ulpi_addr, read_value | 0x0C))
    {
      return JEIO; /* ULPI PHY write failed */
    }
}
#endif /* FEATURE_HS_USB_INTEGRATED_PHY && !FEATURE_HS_USB_NO_HS_OTG_CORE */

  return 0;  /* all is well */

  /* HSU addition END */
}

static void ocd_enable(tdi4x_ocd_sc_t *sc)
{
    juint32_t otgsc_reg;

    /* HSU Addition BEGIN */    
    juint32_t loop_counter = 0;
    juint32_t loop_max = 10;

    DBG_V(DJOTG_OCD, ("OCD: ocd_enable\n"));

    /* Reset the controller before attaching other drivers */
    while (reset_controller(sc, TDI_4X_MODE_CM_DEVICE))
    {
      HSU_ASSERT(loop_counter < loop_max);
      hsu_api_init_phy();
      loop_counter++;
    }
    /* HSU Addition END */

    /* Clear the current interrupt bits */
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, 0x7F0000);
    /* Initialize the interrupt enable bits */
    sc->intr_msk = TDI_4X_OTG_SC_ENB_DEF;
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC, TDI_4X_OTG_SC_ENB_DEF);
    /* Set ID Pullup to true */
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_OTG_SC,
        TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC) | TDI_4X_OTG_SC_IDPU);
    
    /* If needed -> wait for se0 */
    otgsc_reg = TDI_4X_OTG_READ_REG(sc, TDI_4X_OTG_SC);  
    if ((otgsc_reg & TDI_4X_OTG_SC_ID) && (otgsc_reg & TDI_4X_OTG_SC_BSE))
        wait_for_se0_srp(sc);        

    sc->ack_pcd = 1;
    TDI_4X_OTG_WRITE_REG(sc, TDI_4X_USBINTR_REG, TDI_4X_USBINTR_PCD_ENB); 
}

static jresult_t tdi_4x_otg_probe(jdevice_t dev)
{
    return 0;
}

static jresult_t tdi_4x_otg_detach(jdevice_t dev)
{
    tdi4x_ocd_sc_t *sc;
    jbus_h tdi4x_bus = j_device_get_bus(dev);

    if (!dev)
        goto Exit;

    sc = (tdi4x_ocd_sc_t *)j_device_get_softc(dev);
    if (!sc)
        goto Exit;

    if (sc->otg_core_ctx)
        unregister_ocd(sc->otg_core_ctx);
    sc->is_dying = 1;

    /* HSU addition 
       Moving this code forward 
    jinterrupt_teardown(sc->irq_res, sc->intr_hndl);
    End of HSU addition */

    sc->ocd_dev = NULL;

    if (sc->dcd_dev)
    {
        del_generic_controller_ex(sc->dcd_dev);
        sc->dcd_dev = NULL;
    }

    if (sc->hcd_dev)
    {
        del_generic_controller_ex(sc->hcd_dev);
        sc->hcd_dev = NULL;
    }
    
    /* HSU addition */
    jinterrupt_teardown(sc->irq_res, sc->intr_hndl);
    /* End of HSU addition */
    
    if (sc->timer_lock)
        jspinlock_uninit(sc->timer_lock);

    if (sc->reg_res)
        jbus_release_resource(tdi4x_bus, sc->reg_res);
    if (sc->irq_res)
        jbus_release_resource(tdi4x_bus, sc->irq_res);
    
Exit:
    return 0;
}

static jresult_t tdi_4x_otg_attach(jdevice_t dev)
{
    tdi4x_ocd_sc_t *sc = j_device_get_softc(dev);
    jbus_h tdi4x_bus = j_device_get_bus(dev);
    jresult_t rc = 0;
    DECLARE_FNAME("tdi_4x_otg_attach");
    
    sc->ocd_dev = dev;

    /* getting registers base address */
    sc->reg_res = j_bus_alloc_resource(tdi4x_bus, SYS_RES_MEMORY,0);
    sc->irq_res = j_bus_alloc_resource(tdi4x_bus, SYS_RES_IRQ, 0);
    if (!sc->reg_res || !sc->irq_res)
    {
        DBG_E(DJOTG_OCD, ("%s: Error allocating bus resource for OCD\n", 
            fname));
        rc = JENOMEM;
        goto Error;
    }

    rc = jspinlock_init(&sc->timer_lock);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Error in spinlock initialization - %d\n", fname,
            rc));
        goto Error;
    }

    rc = jinterrupt_setup(sc->irq_res, ocd_intr, sc, ocd_dsr, sc, 
        &sc->intr_hndl);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Error in interrupt setup - %d\n", fname, rc));
        goto Error;
    }

    rc = register_ocd(sc, &ocd_ops, &sc->otg_core_ctx, &sc->full_speed_only);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Error in registering OCD - %d\n", fname, rc));
        goto Error;
    }

    /* HSU addition: Memory fragmentation in the DMA pool  */
    /* The original controllers addition order is the same as deletion
     * order in the tdi_4x_otg_detach. Since the "controller_type_tdi_4x" 
     * DMA blocks are less than "controller_type_ehci_local" DMA blocks 
     * the subsequent invocation of tdi_4x_otg_detach and tdi_4x_otg_attach
     * causes a memory fragmentation in the DMA pool and enlarges an amount
     * of allocated but not used memory. 
     * The controllers addition order change prevents this situation. */
    rc = add_generic_controller_ex(tdi4x_bus, controller_type_ehci_local, 
        (void **)&sc->hcd_dev, dev);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Error in adding HCD Controller\n", fname));
        goto Error;
    }
    /* End HSU addition */

    rc = add_generic_controller_ex(tdi4x_bus, controller_type_tdi_4x, 
        (void **)&sc->dcd_dev, dev);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Error in adding DCD Controller\n", fname));
        goto Error;
    }
    
    /* HSU deletion: moved up
    rc = add_generic_controller_ex(tdi4x_bus, controller_type_ehci_local, 
        (void **)&sc->hcd_dev, dev);
    if (rc)
    {
        DBG_E(DJOTG_OCD, ("%s: Error in adding HCD Controller\n", fname));
        goto Error;
    }
    End HSU deletion */

    ocd_enable(sc);    

    jotg_handle_intr(sc->otg_core_ctx);

    return 0;

Error:
    tdi_4x_otg_detach(dev);
    return rc;
}

static jresult_t tdi_4x_otg_suspend(jdevice_t dev)
{
    tdi4x_ocd_sc_t *sc = j_device_get_softc(dev);

    return j_device_suspend(sc->hcd_dev);
}

static jresult_t tdi_4x_otg_resume(jdevice_t dev)
{
    tdi4x_ocd_sc_t *sc = j_device_get_softc(dev);

    return j_device_resume(sc->hcd_dev);
}

static jresult_t tdi_4x_otg_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg)
{
    tdi4x_ocd_sc_t *sc;
    jresult_t rc = 0;

    if (!self)
        goto Exit;
    
    sc = (tdi4x_ocd_sc_t *)j_device_get_softc(self);
    if (!sc || sc->is_dying)
        goto Exit;

    rc = jotg_ioctl_invoked(sc->otg_core_ctx, ioctl, arg);

Exit:
    return rc;
}

DRIVER_CONTROLLER_DECLARE(tdi_4x_otg, sizeof(tdi4x_ocd_sc_t))

