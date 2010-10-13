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
#ifndef _JDEBUG_H_
#define _JDEBUG_H_

#ifdef JDEBUG 

#include <comdef.h>
#include <msg.h>

/* Define to allow the usage of Performance Logs */
#define DEBUG_PERF_LOG

/* --- Debug printing --- */
/* Define to add timing to regular DBG prints */
#define DEBUG_WITH_TIMERS

#ifdef DEBUG_WITH_TIMERS
long dbg_get_time_diff(void);
#endif

/* Default debug output function */
#define DBG_FUNC os_dprintf

#ifndef T_REXNT

/***************************/
/* Defined in debug_init.c */
/***************************/
uint32 jdebug_get_num_args(const char *fmt);

/*HSU addition*/
#define X_MSG_ERROR_VAR_ARGS(fmt, ...)                           \
do                                                             \
{                                                              \
  /*lint -e{506, 774}*/                                        \
  if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_ ## MSG_SSID_HSUSB))  \
  {                                                            \
    XX_MSG_CONST (MSG_SSID_HSUSB, MSG_LEGACY_ERROR, fmt);      \
                                                               \
    /*lint -e{571} */                                          \
    msg_send_var (&xx_msg_const,                               \
                  jdebug_get_num_args(fmt),                    \
                  ##__VA_ARGS__);                              \
  } /* if (MESSAGE_LEGACY_ERROR ...) */                        \
/*lint -e{717} */                                              \
} while (0)                                                    \

/*HSU addition*/
#define X_MSG_HIGH_VAR_ARGS(fmt, ...)                            \
do                                                             \
{                                                              \
  /*lint -e{506, 774}*/                                        \
  if (MSG_LEGACY_HIGH & (MSG_BUILD_MASK_ ## MSG_SSID_HSUSB))   \
  {                                                            \
    XX_MSG_CONST (MSG_SSID_HSUSB, MSG_LEGACY_HIGH, fmt);       \
                                                               \
    /*lint -e{571} */                                          \
    msg_send_var (&xx_msg_const,                               \
                  jdebug_get_num_args(fmt),                    \
                  ##__VA_ARGS__);                              \
  } /* if (MESSAGE_LEGACY_HIGH ...) */                         \
/*lint -e{717} */                                              \
} while (0)                                                    \

/*HSU addition*/
#define X_MSG_MED_VAR_ARGS(fmt, ...)                             \
do                                                             \
{                                                              \
  /*lint -e{506, 774}*/                                        \
  if (MSG_LEGACY_MED & (MSG_BUILD_MASK_ ## MSG_SSID_HSUSB))    \
  {                                                            \
    XX_MSG_CONST (MSG_SSID_HSUSB, MSG_LEGACY_MED, fmt);        \
                                                               \
    /*lint -e{571} */                                          \
    msg_send_var (&xx_msg_const,                               \
                  jdebug_get_num_args(fmt),                    \
                  ##__VA_ARGS__);                              \
  } /* if (MESSAGE_LEGACY_MED ...) */                          \
/*lint -e{717} */                                              \
} while (0)                                                    \

/*HSU addition*/
#define X_MSG_LOW_VAR_ARGS(fmt, ...)                             \
do                                                             \
{                                                              \
  /*lint -e{506, 774}*/                                        \
  if (MSG_LEGACY_LOW & (MSG_BUILD_MASK_ ## MSG_SSID_HSUSB))    \
  {                                                            \
    XX_MSG_CONST (MSG_SSID_HSUSB, MSG_LEGACY_LOW, fmt);        \
                                                               \
    /*lint -e{571} */                                          \
    msg_send_var (&xx_msg_const,                               \
                  jdebug_get_num_args(fmt),                    \
                  ##__VA_ARGS__);                              \
  } /* if (MESSAGE_LEGACY_LOW ...) */                          \
/*lint -e{717} */                                              \
} while (0)                                                    \

/*HSU addition*/
#define MSG_ERROR_VAR_ARGS(...) X_MSG_ERROR_VAR_ARGS(__VA_ARGS__, 0)
#define MSG_HIGH_VAR_ARGS(...) X_MSG_HIGH_VAR_ARGS(__VA_ARGS__, 0)
#define MSG_MED_VAR_ARGS(...) X_MSG_MED_VAR_ARGS(__VA_ARGS__, 0)
#define MSG_LOW_VAR_ARGS(...) X_MSG_LOW_VAR_ARGS(__VA_ARGS__, 0)

/* HSU modification for DBG_* to reduce CPU usage */
#define DBG_E(type, message) \
        do { \
            if ((DL_ERROR) <= debug_bank[(jint_t)(type)]) \
            { \
              MSG_ERROR_VAR_ARGS message; \
            } \
        } while (0)

#define DBG_W(type, message) \
        do { \
            if ((DL_WARNING) <= debug_bank[(jint_t)(type)]) \
            { \
              MSG_HIGH_VAR_ARGS  message; \
            } \
        } while (0)

#define DBG_I(type, message) \
        do { \
            if ((DL_INFO) <= debug_bank[(jint_t)(type)]) \
            { \
              MSG_MED_VAR_ARGS  message; \
            } \
        } while (0)

#define DBG_V(type, message) \
        do { \
            if ((DL_VERBOSE) <= debug_bank[(jint_t)(type)]) \
            { \
              MSG_LOW_VAR_ARGS  message; \
            } \
        } while (0)

#define DBG_X(type, message) \
        do { \
            if ((DL_EX_VERBOSE) <= debug_bank[(jint_t)(type)]) \
            { \
              MSG_LOW_VAR_ARGS  message; \
            } \
        } while (0)

#else /* T_REXNT */
#error code not present
#endif /* T_REXNT */

#define RETURN_WITH_ERROR(type, code, message) \
    do { \
        DBG_E((type), message); \
        return code; \
    } while (0)

#define DBG_RUN(type, level, code) \
        do { \
        valid_type(type); \
        if ((level) <= debug_bank[(jint_t)(type) - ((jint_t)(type) % 10)] && \
            (level) <= debug_bank[(jint_t)(type)]) \
            do { code ; } while (0); \
        } while (0)

#define DBG_IF(expr)    if (expr)

#define DBG_SET_LEVEL(type, level) \
        do { \
        valid_type(type); \
        debug_bank[(jint_t)(type)] = (level); \
        } while (0)

#define DBG_SET_GLOBAL_LEVEL(level) \
        do { \
                jint_t i; \
                for (i = 0; i < DLAST; i++) \
                    debug_bank[i] = (level); \
        } while(0)

#define DBG_GET_LEVEL(type) debug_bank[(jint_t)(type)]

enum DEBUG_LEVELS {
    DL_NONE,
    DL_ERROR,
    DL_WARNING,
    DL_INFO,
    DL_VERBOSE,
    DL_EX_VERBOSE,
    DL_ALL
};

enum DEBUG_MODULES {
    DPORT       =       0,
        DPORT_INIT,
        DPORT_SYNC,
        DPORT_BUS,
        DPORT_MEM,
        DPORT_PCI,
        DPORT_UTIL,
        DPORT_ENTRY,
        DPORT_IRQ,

    DPORT_APP      =    10,
        DPORT_BLOCK,
        DPORT_NET,
        DPORT_SERIAL,
        DPORT_PRINTER,
        DPORT_ECM,

    DUTIL       =       20,
        DUTIL_XFRAG,
        DUTIL_DEVMON,
        DUTIL_XML,
        DUTIL_APPMGR,

    DJOS        =       30,
        DJOS_DEVICE,
        DJOS_INIT,
        DJOS_MEM,
        DJOS_DRIVER,
        DJOS_SYNC,

#ifdef CONFIG_JSLAVE
    DSLAVE      =       40,
        DSLAVE_CORE,
        DSLAVE_DCD,
        DSLAVE_FD,
        DSLAVE_LOOP,
        DSLAVE_SERIAL,
#endif

#ifdef CONFIG_FD_MASS   
    DSLAVE_MS   =       50,
        DSLAVE_MS_USB,
        DSLAVE_MS_SCSI,
        DSLAVE_MS_API,
        DSLAVE_MS_TRANS,
#endif

#ifdef CONFIG_FD_CDC
    DSLAVE_CDC_ALL  =   60,
        DSLAVE_CDC,
        DSLAVE_CDC_ACM,
        DSLAVE_CDC_ECM,
        DSLAVE_CDC_OBEX,
#endif

#ifdef CONFIG_FD_SICD
    DSLAVE_SICD_ALL =   70,
        DSLAVE_SICD,
        DSLAVE_SICD_API,
        DSLAVE_MTP_API,
        DSLAVE_DPS_API,
#endif

#ifdef CONFIG_JHOST     
    DHCD        =       80,
        DHCD_INIT,
        DHCD_TRANSFER,
        DHCD_ABORT,
        DHCD_POWER,
        DHCD_INTR,
        DHCD_HUB,
        
    DHOST       =       90,
        DHOST_EXPLORE,
        DHOST_SUBR,
        DHOST_USBDI,
        DHOST_MEM,
        DHOST_USB,
        DHOST_TRANSFER,
        DHOST_ROOT,
        DHOST_POWER,

    DHUB        =       100,
        DHUB_EXPLORE,
        DHUB_ATTACH,
        DHUB_POWER,

    DCOMPOSITE  =       110,
        DCOMPOSITE_ATTACH,
        DCOMPOSITE_DETACH,
        DCOMPOSITE_POWER,
#endif

#ifdef CONFIG_JUMASS    
    DMASS       =       120,
        DMASS_UMASS,
        DMASS_SCSI,
        DMASS_DEVICE,
        DMASS_DISK,
        DMASS_ASYNC,
        DMASS_POWER,
#endif

#if defined(CONFIG_TEST) || defined(CONFIG_EM_FS_TEST) || \
    defined(CONFIG_USB_TESTING)
    DTESTS      =       130,
        DTEST_PARAM,
#endif

#ifdef CONFIG_EM_FS
    DFS         =       140,
#endif

#ifdef CONFIG_FD_TEST
    DSLAVE_TEST =       150,
#endif

#ifdef CONFIG_FD_HID
    DSLAVE_HID =        160,
        DSLAVE_HID_LIB,
        DSLAVE_HID_KBD,
#endif

#ifdef CONFIG_JHOST_HID
    DHOST_HID =         170,
        DHOST_HID_LIB,
        DHOST_HID_KBD,
        DHOST_HID_GENERAL,
        DHOST_HID_POWER,
#endif
#ifdef CONFIG_JHOST_SERIAL
    DHOST_SERIAL =      180,
#endif

#ifdef CONFIG_USB_TESTING 
    DJTEST =            190,
        DJTEST_USBD,    
        DJTEST_TESTS, 
        DJTEST_HARNESS, 
#endif

#ifdef CONFIG_FD_VIDEO
    DSLAVE_VIDEO =      200,
        DSLAVE_VIDEO_FD,
        DSLAVE_VIDEO_DESC,
        DSLAVE_VIDEO_API,
#endif

#ifdef CONFIG_FD_DFU
    DSLAVE_DFU =        210,
        DSLAVE_DFU_API,
        DSLAVE_DFU_FD,
#endif

#ifdef CONFIG_PROTO
    DPROTO =            220,
        DPROTO_PTP,
        DPROTO_MTP,
        DPROTO_DPS,
#endif

#ifdef CONFIG_JOTG
    DJOTG =             230,
        DJOTG_OCD,
        DJOTG_CORE,
        DJOTG_API,
#endif

    DVENDOR =           240,

#ifdef CONFIG_JHOST_PRINTER
    DJPRINTER =         250,
        DJPRINTER_CTRL,
        DJPRINTER_API,
#endif
        
#ifdef CONFIG_HID_TEST
    DHOST_HID_TEST =    260,
    DHID_MOUSE,
    DHID_KEYBOARD,
    DHID_JOYSTICK,

#endif
        
#ifdef CONFIG_JHOST_CDC
    DHOST_CDC =         270,
        DHOST_CDC_GENERAL,
        DHOST_CDC_UTILS,
        DHOST_CDC_ECM,
        DHOST_CDC_ACM,
        DHOST_CDC_EEM,
        DHOST_CDC_OBEX,
#endif

#ifdef CONFIG_JTRANSCEIVER
    DTRCV =             280,
        DTRCV_TCD,
        DTRCV_CORE,
#endif

#ifdef CONFIG_JHOST_ETEST
        DHOST_ETEST =   290,
#endif
#ifdef CONFIG_JHOST_ICCD
        DHOST_ICCD = 370,
        DHOST_ICCD_GENERAL,
        DHOST_ICCD_API,
        DHOST_ICCD_APP,
#endif

#ifdef CONFIG_JHOST_AUDIO
    DHOST_AUDIO_GLOBAL = 310,
        DHOST_AUDIO,
        DHOST_AUDIO_STREAM,
    DHOST_AUDIO_APP,
#endif

#ifdef CONFIG_FD_AUDIO
    DSLAVE_AUDIO_GLOBAL = 380,
        DSLAVE_AUDIO,
        DSLAVE_AUDIO_DESC,
        DSLAVE_AUDIO_CTRL,
        DSLAVE_AUDIO_STREAM,
        DSLAVE_AUDIO_APP,
#endif
    DLAST
};

void valid_type(enum DEBUG_MODULES type);

extern enum DEBUG_LEVELS debug_bank[DLAST];

#define DECLARE_FNAME(name) static const char *fname = (name)

/* HSU temp fix */
#define PRINT_DBG(type, level, message) do { } while (0)
/* HSU temp fix */

#else

#define DBG_E(type, message) do { } while (0)
#define DBG_W(type, message) do { } while (0)
#define DBG_I(type, message) do { } while (0)
#define DBG_V(type, message) do { } while (0)
#define DBG_X(type, message) do { } while (0)

#define RETURN_WITH_ERROR(type, code, message) do { } while (0)

#define DBG_RUN(type, level, code)      do { } while (0)
#define DBG_IF(expr)                    if (0)
#define DBG_SET_LEVEL(type, level)      do { } while (0)
#define DBG_SET_GLOBAL_LEVEL(level)     do { } while (0)
#define DBG_GET_LEVEL(type)             do { } while (0)
#define PRINT_DBG(type, level, message) do { } while (0)
#define DECLARE_FNAME(name)

#define KASSERT(expr, msg)                                      

#endif

#endif

