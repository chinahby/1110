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

/* HSU addition - start */
#ifdef T_REXNT
#error code not present
#endif  /* T_REXNT */
/* HSU addition - end */

#include "jos_internal.h"

#ifdef JDEBUG

enum DEBUG_LEVELS debug_bank[DLAST];

void debug_init(void)
{
    DBG_SET_GLOBAL_LEVEL(DL_WARNING);
    {
        DBG_SET_LEVEL(DPORT,            DL_ALL);
        {
            DBG_SET_LEVEL(DPORT_INIT,           DL_WARNING);
            DBG_SET_LEVEL(DPORT_SYNC,           DL_WARNING);
            DBG_SET_LEVEL(DPORT_BUS,            DL_WARNING);
            DBG_SET_LEVEL(DPORT_MEM,            DL_WARNING);
            DBG_SET_LEVEL(DPORT_PCI,            DL_WARNING);
            DBG_SET_LEVEL(DPORT_UTIL,           DL_WARNING);
            DBG_SET_LEVEL(DPORT_ENTRY,          DL_WARNING);
            DBG_SET_LEVEL(DPORT_IRQ,            DL_WARNING);
        }
        
        DBG_SET_LEVEL(DPORT_APP,        DL_ALL);
        {
            DBG_SET_LEVEL(DPORT_BLOCK,          DL_WARNING);
            DBG_SET_LEVEL(DPORT_SERIAL,         DL_WARNING);
            DBG_SET_LEVEL(DPORT_NET,            DL_WARNING);
            DBG_SET_LEVEL(DPORT_PRINTER,        DL_WARNING);
			DBG_SET_LEVEL(DPORT_ECM,            DL_WARNING);
        }

        DBG_SET_LEVEL(DUTIL,            DL_ALL);
        {
            DBG_SET_LEVEL(DUTIL_XFRAG,          DL_WARNING);
            DBG_SET_LEVEL(DUTIL_DEVMON,         DL_WARNING);
            DBG_SET_LEVEL(DUTIL_XML,            DL_WARNING);
            DBG_SET_LEVEL(DUTIL_APPMGR,         DL_WARNING);
        }
        
        DBG_SET_LEVEL(DJOS,             DL_ALL);
        {
            DBG_SET_LEVEL(DJOS_DRIVER,          DL_WARNING);
            DBG_SET_LEVEL(DJOS_DEVICE,          DL_WARNING);
            DBG_SET_LEVEL(DJOS_INIT,            DL_WARNING);
            DBG_SET_LEVEL(DJOS_MEM,             DL_WARNING);
            DBG_SET_LEVEL(DJOS_SYNC,            DL_ERROR);
        }

#ifdef CONFIG_JSLAVE
        DBG_SET_LEVEL(DSLAVE,           DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_FD,            DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_DCD,           DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_CORE,          DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_LOOP,          DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_SERIAL,        DL_WARNING);
        }
#endif
#ifdef CONFIG_FD_MASS   
        DBG_SET_LEVEL(DSLAVE_MS,        DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_MS_USB,        DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_MS_SCSI,       DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_MS_API,        DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_MS_TRANS,      DL_WARNING);
        }
#endif
#ifdef CONFIG_FD_CDC
        DBG_SET_LEVEL(DSLAVE_CDC_ALL,  DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_CDC,           DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_CDC_ACM,       DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_CDC_ECM,       DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_CDC_OBEX,      DL_WARNING);
        }
#endif
#ifdef CONFIG_FD_SICD
        DBG_SET_LEVEL(DSLAVE_SICD_ALL,  DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_SICD,          DL_WARNING);
			      DBG_SET_LEVEL(DSLAVE_SICD_API,      DL_WARNING);
 			      DBG_SET_LEVEL(DSLAVE_MTP_API,       DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_DPS_API,       DL_WARNING);
        }
#endif
#ifdef CONFIG_FD_HID
        DBG_SET_LEVEL(DSLAVE_HID,       DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_HID_LIB,       DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_HID_KBD,       DL_WARNING);
        }
#endif
        
#ifdef CONFIG_FD_TEST
        DBG_SET_LEVEL(DSLAVE_TEST,      DL_INFO);
#endif

#ifdef CONFIG_FD_VIDEO
        DBG_SET_LEVEL(DSLAVE_VIDEO,     DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_VIDEO_FD,   DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_VIDEO_DESC, DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_VIDEO_API, DL_WARNING);
        }
#endif

#ifdef CONFIG_FD_DFU
        DBG_SET_LEVEL(DSLAVE_DFU,     DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_DFU_FD,   DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_DFU_API,  DL_WARNING);
        }
#endif

#ifdef CONFIG_FD_AUDIO
        DBG_SET_LEVEL(DSLAVE_AUDIO_GLOBAL, DL_ALL);
        {
            DBG_SET_LEVEL(DSLAVE_AUDIO,        DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_AUDIO_DESC,   DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_AUDIO_CTRL,   DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_AUDIO_STREAM, DL_WARNING);
            DBG_SET_LEVEL(DSLAVE_AUDIO_APP,    DL_WARNING);
        }
#endif

#ifdef CONFIG_JHOST     
        DBG_SET_LEVEL(DHOST,            DL_ALL);
        {
            DBG_SET_LEVEL(DHOST_EXPLORE,        DL_WARNING);
            DBG_SET_LEVEL(DHOST_SUBR,           DL_WARNING);
            DBG_SET_LEVEL(DHOST_USBDI,          DL_WARNING);
            DBG_SET_LEVEL(DHOST_MEM,            DL_WARNING);
            DBG_SET_LEVEL(DHOST_USB,            DL_WARNING);
            DBG_SET_LEVEL(DHOST_TRANSFER,       DL_WARNING);
            DBG_SET_LEVEL(DHOST_ROOT,           DL_WARNING);
            DBG_SET_LEVEL(DHOST_POWER,          DL_WARNING);
        }

        DBG_SET_LEVEL(DHCD,             DL_ALL);
        {
            DBG_SET_LEVEL(DHCD_INIT,            DL_WARNING);
            DBG_SET_LEVEL(DHCD_ABORT,           DL_WARNING);
            DBG_SET_LEVEL(DHCD_POWER,           DL_WARNING);
            DBG_SET_LEVEL(DHCD_INTR,            DL_WARNING);
            DBG_SET_LEVEL(DHCD_TRANSFER,        DL_WARNING);
            DBG_SET_LEVEL(DHCD_HUB,             DL_ALL);
        }

        DBG_SET_LEVEL(DHUB,             DL_ALL);
        {
            DBG_SET_LEVEL(DHUB_ATTACH,          DL_WARNING);
            DBG_SET_LEVEL(DHUB_EXPLORE,         DL_ALL);
            DBG_SET_LEVEL(DHUB_POWER,           DL_WARNING);
        }

        DBG_SET_LEVEL(DCOMPOSITE,       DL_ALL);
        {
            DBG_SET_LEVEL(DCOMPOSITE_ATTACH,    DL_WARNING);
            DBG_SET_LEVEL(DCOMPOSITE_DETACH,    DL_WARNING);
            DBG_SET_LEVEL(DCOMPOSITE_POWER,     DL_WARNING);
        }
#endif
#ifdef CONFIG_JUMASS    
        {
        DBG_SET_LEVEL(DMASS,            DL_ALL);
            DBG_SET_LEVEL(DMASS_SCSI,           DL_WARNING);
            DBG_SET_LEVEL(DMASS_DEVICE,         DL_WARNING);
            DBG_SET_LEVEL(DMASS_UMASS,          DL_WARNING);
            DBG_SET_LEVEL(DMASS_DISK,           DL_WARNING);
            DBG_SET_LEVEL(DMASS_ASYNC,          DL_WARNING);
            DBG_SET_LEVEL(DMASS_POWER,          DL_WARNING);
        }
#endif
        
#ifdef CONFIG_HID_TEST
        {
            DBG_SET_LEVEL(DHOST_HID_TEST,   DL_ALL);
            DBG_SET_LEVEL(DHID_MOUSE,           DL_ERROR);
            DBG_SET_LEVEL(DHID_KEYBOARD,        DL_ERROR);
            DBG_SET_LEVEL(DHID_JOYSTICK,        DL_ERROR);
        }
#endif

#if defined(CONFIG_TEST) || defined(CONFIG_EM_FS_TEST) || \
    defined(CONFIG_USB_TESTING)
        DBG_SET_LEVEL(DTESTS,           DL_WARNING);
            DBG_SET_LEVEL(DTEST_PARAM,          DL_WARNING);
#endif
#ifdef CONFIG_EM_FS
        DBG_SET_LEVEL(DFS,              DL_ALL);
#endif
#ifdef CONFIG_JHOST_HID 
        DBG_SET_LEVEL(DHOST_HID,                DL_ALL);
        {
            DBG_SET_LEVEL(DHOST_HID_LIB,        DL_WARNING);
            DBG_SET_LEVEL(DHOST_HID_KBD,        DL_WARNING);
            DBG_SET_LEVEL(DHOST_HID_GENERAL,    DL_WARNING);
            DBG_SET_LEVEL(DHOST_HID_POWER,      DL_WARNING);
        }
#endif
#ifdef CONFIG_JHOST_CDC 
        DBG_SET_LEVEL(DHOST_CDC,                DL_ALL);
        {
            DBG_SET_LEVEL(DHOST_CDC_GENERAL,    DL_ERROR);
            DBG_SET_LEVEL(DHOST_CDC_UTILS,      DL_ERROR);
            DBG_SET_LEVEL(DHOST_CDC_ECM,        DL_ERROR);
            DBG_SET_LEVEL(DHOST_CDC_EEM,        DL_ERROR);
            DBG_SET_LEVEL(DHOST_CDC_ACM,        DL_ERROR);
            DBG_SET_LEVEL(DHOST_CDC_OBEX,       DL_ERROR);
        }
#endif
#ifdef CONFIG_JHOST_SERIAL
        DBG_SET_LEVEL(DHOST_SERIAL, DL_ALL);
#endif
#ifdef CONFIG_USB_TESTING
        DBG_SET_LEVEL(DJTEST, DL_WARNING);
        {
            DBG_SET_LEVEL(DJTEST_USBD, DL_WARNING);
            DBG_SET_LEVEL(DJTEST_TESTS, DL_WARNING);
            DBG_SET_LEVEL(DJTEST_HARNESS, DL_WARNING);
        }
#endif

#ifdef CONFIG_PROTO
        DBG_SET_LEVEL(DPROTO, DL_ALL);
        {
            DBG_SET_LEVEL(DPROTO_PTP, DL_WARNING);
            DBG_SET_LEVEL(DPROTO_MTP, DL_WARNING);
      			DBG_SET_LEVEL(DPROTO_DPS, DL_WARNING);
        }
#endif

#ifdef CONFIG_JOTG
        DBG_SET_LEVEL(DJOTG, DL_ALL);
        {
            DBG_SET_LEVEL(DJOTG_OCD,  DL_WARNING);
            DBG_SET_LEVEL(DJOTG_CORE, DL_WARNING);
            DBG_SET_LEVEL(DJOTG_API,  DL_WARNING);
        }
#endif

        DBG_SET_LEVEL(DVENDOR, DL_ALL);

#ifdef CONFIG_JHOST_PRINTER
        DBG_SET_LEVEL(DJPRINTER, DL_ALL);
        {
            DBG_SET_LEVEL(DJPRINTER_CTRL,       DL_ERROR);
            DBG_SET_LEVEL(DJPRINTER_API,        DL_ERROR);
        }
#endif

#ifdef CONFIG_JTRANSCEIVER
        DBG_SET_LEVEL(DTRCV, DL_ALL);
        {
            DBG_SET_LEVEL(DTRCV_CORE, DL_ERROR);
            DBG_SET_LEVEL(DTRCV_TCD,  DL_ERROR);
        }
#endif

#ifdef CONFIG_JHOST_ETEST
        DBG_SET_LEVEL(DHOST_ETEST, DL_ALL);
#endif

#ifdef CONFIG_JHOST_ICCD
        DBG_SET_LEVEL(DHOST_ICCD,                DL_ALL);
        {
            DBG_SET_LEVEL(DHOST_ICCD_GENERAL, DL_WARNING);
            DBG_SET_LEVEL(DHOST_ICCD_API,     DL_WARNING);
            DBG_SET_LEVEL(DHOST_ICCD_APP,     DL_WARNING);
        }
#endif

#ifdef CONFIG_JHOST_AUDIO
        DBG_SET_LEVEL(DHOST_AUDIO_GLOBAL, DL_ALL);
        {
            DBG_SET_LEVEL(DHOST_AUDIO,        DL_WARNING);
            DBG_SET_LEVEL(DHOST_AUDIO_STREAM, DL_WARNING);
            DBG_SET_LEVEL(DHOST_AUDIO_APP,    DL_WARNING);
        }
#endif

    }
}

void valid_type(enum DEBUG_MODULES type)
{
    UNUSED_VAR(type);
}

#endif /* JDEBUG */


#ifdef T_REXNT
#error code not present
#else  /* T_REXNT */

/* ==========================================================================
FUNCTION JDEBUG_GET_NUM_ARGS

DESCRIPTION
  Gets a printf style formatted string and returns the number of arguments
  inside it.

DEPENDENCIES
  None.

RETURN VALUE
  Return the number of arguments in the formatted string.

SIDE EFFECTS
  None.
============================================================================= */
juint32_t jdebug_get_num_args(const char *fmt)
{
  juint32_t       num_args;
  const char*  prev_pos = NULL;
  const char*  curr_pos = NULL;


  for (num_args = 0, curr_pos = fmt;
       *curr_pos != '\0';
       prev_pos = curr_pos, curr_pos++)
  {
    if (prev_pos != NULL)
    {
      if (*curr_pos != '%' && *prev_pos == '%')
      {
        num_args++;
      }
    }
  }

  return num_args;
}  /* jdebug_get_num_args */

#endif  /* T_REXNT */
