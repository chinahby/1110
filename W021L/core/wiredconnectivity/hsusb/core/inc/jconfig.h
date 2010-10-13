#ifndef JCONFIG_H
#define JCONFIG_H

/* Features list header file. */
#include <customer.h>
/* #include <jtypes.h> */

/* Disable sync checks */
/* TODO: consider enabling checks (once the issue of giant mutex nesting
** is resolved).
** Note: Even if sync checks are enabled for the application processor build,
** they still must be disabled on RexNT, and also in ARMPRG and DLOAD.
**/
#define CONFIG_PORT_NO_GET_CONTEXT

/* Device config */
#define CONFIG_JSLAVE 1

#ifdef FEATURE_HS_USB_FS_ONLY
#define CONFIG_FULL_SPEED_ONLY
#endif /* FEATURE_HS_USB_FS_ONLY */

#ifndef FEATURE_HS_USB_BASIC
/* For supporting OTG Device Electrical Tests */
#ifndef CONFIG_FULL_SPEED_ONLY
#define CONFIG_JSLAVE_TEST_MODE 1 
#endif /* ! CONFIG_FULL_SPEED_ONLY */
#endif /* FEATURE_HS_USB_BASIC */

#if (!defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_LEGACY_HOST))
#define CONFIG_TD243HC_REV2 1
#else
#define CONFIG_TDI_4X 1
#endif
#define CONFIG_TDI_4X_STREAM_DISABLE 1

#define CONFIG_TDI_4X_ITC 0x08

#if (defined(FEATURE_HS_USB_LEGACY_DEVICE) && !defined(FEATURE_HS_USB_BASIC))
#define CONFIG_TD243FC_REV2 1
#define CONFIG_TD243FC_DATA_MEMORY_SIZE 4096
#define CONFIG_TD243FC_DATA_MEMORY_OFFSET 0
#endif

#if (defined FEATURE_HS_USB_LEGACY_HOST) && (!defined FEATURE_HS_USB_BASIC)
#define CONFIG_TD243HC_REV2 1 
#endif /*FEATURE_HS_USB_LEGACY_HOST*/

#ifdef FEATURE_HS_USB_TRANSCEIVER_DEVICE
#define CONFIG_JTRANSCEIVER 1
#endif /*FEATURE_HS_USB_TRANSCEIVER_DEVICE*/

/* Allow remote wakeup timeout in milliseconds (in case host did not issue resume) */
#define CONFIG_REMOTE_WAKEUP_TIMEOUT 2000

#ifndef FEATURE_HS_USB_BASIC

#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif  /* FEATURE_HS_USB_OTG */

#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE
#define CONFIG_FD_AUDIO
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */

#ifdef FEATURE_HS_USB_MS_FD
#define CONFIG_FD_MASS    1
#define CONFIG_SCSI_TRANSPARENT_COMMAND 1

#ifdef FEATURE_HS_USB_FAST_CSW
#define CONFIG_FAST_CSW 1
#endif

#endif /* FEATURE_HS_USB_MS_FD */

#define CONFIG_FD_CDC_ACM 1

#ifdef FEATURE_HS_USB_ECM
#define CONFIG_FD_CDC_ECM 1
#endif /* FEATURE_HS_USB_CDC_ECM */

#endif /* FEATURE_HS_USB_BASIC */

#define CONFIG_FD_CDC 1
#define CONFIG_FD_CDC_OBEX 1

#ifndef FEATURE_HS_USB_BASIC

#ifdef FEATURE_PICTBRIDGE_HS

/* Sicd Function Driver */
#define CONFIG_FD_SICD 1

/* PictBridge FD */
#define CONFIG_PROTO 1
#define CONFIG_PROTO_PTP 1
#define CONFIG_PROTO_DPS 1
#define CONFIG_FD_DPS_APP 1
#define CONFIG_PKG 1
#define CONFIG_PKG_XML 1
#undef CONFIG_PROTO_PTP_VS_EXT
#undef CONFIG_PROTO_PTP_MTP
#undef CONFIG_PROTO_PTP_MTP_ENHANCED
#undef CONFIG_FD_MTP_APP
#undef CONFIG_FD_SICD_APP

#endif  /* FEATURE_PICTBRIDGE_HS */

/* OTG configuration */
#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif  /* FEATURE_HS_USB_OTG */

#define PTP_TRANSPORT_LITTLE_ENDIAN 1

#endif /* FEATURE_HS_USB_BASIC */

/* Environment config */
#define CONFIG_BYTE_ORDER CPU_LITTLE_ENDIAN
#define CONFIG_ALIGN_CRITICAL 1

/* Tests config */
#ifdef FEATURE_HSU_TEST
#error code not present
#endif

/* Environment config */
#define CONFIG_MEMPOOL 1
#define CONFIG_MEMPOOL_DMABLE 1
#ifndef T_REXNT
#define CONFIG_ARM_COMPILER  1
#else
#error code not present
#endif

#ifndef FEATURE_HS_USB_BASIC

  #ifdef FEATURE_HS_USB_OTG
#error code not present
  #else
    #define CONFIG_TASK_SINGLE /* use a single task for the device stack */
  #endif  /* FEATURE_HS_USB_OTG */

#else /* FEATURE_HS_USB_BASIC */
  #define CONFIG_TASK_SINGLE /* use a single task for the device stack */

#endif /* FEATURE_HS_USB_BASIC */

/* To reduce code space usage, disable JDEBUG if memory optimization is on */
#ifdef FEATURE_HS_USB_MEM_OPTIMIZATION
#undef JDEBUG
#endif /* FEATURE_HS_USB_MEM_OPTIMIZATION */

#ifdef FEATURE_HS_USB_ENABLE_DATA_PATH_LOGGING
#define CONFIG_DATA_PATH_DEBUG
#endif /* FEATURE_HS_USB_ENABLE_DATA_PATH_LOGGING */

#endif /* JCONFIG_H */

