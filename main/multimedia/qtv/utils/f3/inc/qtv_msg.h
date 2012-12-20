#ifndef QTV_MSG_H
#define QTV_MSG_H
/* =======================================================================
                               qtv_msg.h
DESCRIPTION
  All the declarations and definitions necessary to support the reporting 
  of messages for errors and debugging.  This includes support for the 
  extended capabilities as well as the legacy messaging scheme.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/f3/main/latest/inc/qtv_msg.h#3 $
$DateTime: 2009/10/27 04:20:28 $
$Change: 1065213 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "msg.h"
#ifdef FEATURE_WINCE
#error code not present
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef FEATURE_WINCE
#error code not present
#endif // #ifdef FEATURE_WINCE
/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#ifdef FEATURE_QTV_MSG_3_0  

 /* Note: In 3.0, the first parameter to QTV_MSG() is no longer a mask!
  * It must be a single SSID.
  */

 /* QTV Message Sub-Group Ids */

#define QTVDIAG_GENERAL           (MSG_SSID_APPS_QTV_GENERAL)
#define QTVDIAG_DEBUG             (MSG_SSID_APPS_QTV_DEBUG)
#define QTVDIAG_STATISTICS        (MSG_SSID_APPS_QTV_STATISTICS)
#define QTVDIAG_UI_TASK           (MSG_SSID_APPS_QTV_UI_TASK)
#define QTVDIAG_MP4_PLAYER        (MSG_SSID_APPS_QTV_MP4_PLAYER)
#define QTVDIAG_AUDIO_TASK        (MSG_SSID_APPS_QTV_AUDIO_TASK)
#define QTVDIAG_VIDEO_TASK        (MSG_SSID_APPS_QTV_VIDEO_TASK)
#define QTVDIAG_STREAMING         (MSG_SSID_APPS_QTV_STREAMING)
#define QTVDIAG_MPEG4_TASK        (MSG_SSID_APPS_QTV_MPEG4_TASK)
#define QTVDIAG_FILE_OPS          (MSG_SSID_APPS_QTV_FILE_OPS)
#define QTVDIAG_RTP               (MSG_SSID_APPS_QTV_RTP)
#define QTVDIAG_RTCP              (MSG_SSID_APPS_QTV_RTCP)
#define QTVDIAG_RTSP              (MSG_SSID_APPS_QTV_RTSP)
#define QTVDIAG_SDP_PARSE         (MSG_SSID_APPS_QTV_SDP_PARSE)
#define QTVDIAG_ATOM_PARSE        (MSG_SSID_APPS_QTV_ATOM_PARSE)
#define QTVDIAG_TEXT_TASK         (MSG_SSID_APPS_QTV_TEXT_TASK)
#define QTVDIAG_DEC_DSP_IF        (MSG_SSID_APPS_QTV_DEC_DSP_IF)
#define QTVDIAG_STREAM_RECORDING  (MSG_SSID_APPS_QTV_STREAM_RECORDING)
#define QTVDIAG_CONFIGURATION     (MSG_SSID_APPS_QTV_CONFIGURATION)
#define QTVDIAG_BCAST_FLO         (MSG_SSID_APPS_QTV_BCAST_FLO)
#define QTVDIAG_GENERIC_BCAST     (MSG_SSID_APPS_QTV_STREAMING)

/* QTV Message Priorities */
#define QTVDIAG_PRIO_LOW      (MSG_MASK_0)
#define QTVDIAG_PRIO_MED      (MSG_MASK_1)
#define QTVDIAG_PRIO_HIGH     (MSG_MASK_2)
#define QTVDIAG_PRIO_ERROR    (MSG_MASK_3)
#define QTVDIAG_PRIO_FATAL    (MSG_MASK_4)
#define QTVDIAG_PRIO_DEBUG    (MSG_MASK_5)

/* Message macros */

/* For compatibility -- use medium priority by default. */
#define QTV_MSG(xx_ss_mask, xx_fmt) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send (&xx_msg_const); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

#define QTV_MSG_PRIO(xx_ss_mask, xx_prio, xx_fmt) \
  do { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send (&xx_msg_const); \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameter.
---------------------------------------------------------------------------*/
#define QTV_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define QTV_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define QTV_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters. In this case the function
  called needs to have more than 4 parameters so it is going to be a slow 
  function call.  So for this case the  msg_send_var() uses var arg list 
  supported by the compiler.
---------------------------------------------------------------------------*/
#define QTV_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define QTV_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var(&xx_msg_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var(&xx_msg_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */


/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define QTV_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO6(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */


/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define QTV_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0)                                                             \
               /* lint +e717 */

#define QTV_MSG_PRIO7(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0)                                                             \
               /* lint +e717 */


/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define QTV_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO8(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */


/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define QTV_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_PRIO9(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define QTV_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_SPRINTF_PRIO_1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define QTV_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_SPRINTF_PRIO_2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define QTV_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_SPRINTF_PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define QTV_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  do { \
      XX_MSG_CONST ( xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_SPRINTF_PRIO_4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  do { \
      XX_MSG_CONST ( xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
  } while (0) \
               /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define QTV_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
  } while (0) \
               /* lint +e717 */

#define QTV_MSG_SPRINTF_PRIO_5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
  } while (0) \
               /* lint +e717 */


/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define QTV_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST_FMT_VAR (xx_ss_mask, QTVDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
  } while (0)

#define QTV_MSG_SPRINTF_FMT_VAR__PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST_FMT_VAR (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
  } while (0)

#else

/* Macros for Qtv Diagnostic messages 2.0*/
#ifndef FEATURE_WINCE
#define QTV_MSG(xx_ss_mask, xx_fmt) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send (&xx_msg_const); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */               

#define QTV_MSG_PRIO(xx_ss_mask, xx_prio, xx_fmt) QTV_MSG(xx_ss_mask, xx_fmt) 

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameter.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_PRIO1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) QTV_MSG1(xx_ss_mask, xx_fmt, xx_arg1) 

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_PRIO2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) QTV_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_PRIO3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  QTV_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters. In this case the function
  called needs to have more than 4 parameters so it is going to be a slow 
  function call.  So for this case the  msg_send_var() uses var arg list 
  supported by the compiler.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_PRIO4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  QTV_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var(&xx_msg_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */

#define QTV_MSG_PRIO5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  QTV_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_PRIO6(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  QTV_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)                                                             \
               /* lint +e717 */

#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_PRIO7(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  QTV_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* FEATURE_WINCE */

#define QTV_MSG_PRIO8(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  QTV_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* FEATURE_WINCE */

#define QTV_MSG_PRIO9(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  QTV_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_SPRINTF_PRIO_1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
   QTV_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_SPRINTF_PRIO_2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
   QTV_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* FEATURE_WINCE */
#define QTV_MSG_SPRINTF_PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
   QTV_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_SPRINTF_PRIO_4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
   QTV_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */
#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_SPRINTF_PRIO_5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
   QTV_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#ifndef FEATURE_WINCE
#define QTV_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_MSG_SSID_APPS_QTV)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST_FMT_VAR (MSG_SSID_APPS_QTV, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)

#else
#error code not present
#endif /* NOT FEATURE_WINCE */
#define QTV_MSG_SPRINTF_FMT_VAR_PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) QTV_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/* QTV Message Priorities (not used) */

#define QTVDIAG_PRIO_DEBUG    (0)
#define QTVDIAG_PRIO_LOW      (0)
#define QTVDIAG_PRIO_MED      (0)
#define QTVDIAG_PRIO_HIGH     (0)
#define QTVDIAG_PRIO_ERROR    (0)
#define QTVDIAG_PRIO_FATAL    (0)

 /* QTV Message Sub-Group Ids */

#define QTVDIAG_GENERAL           (MSG_MASK_5)
#define QTVDIAG_DEBUG             (MSG_MASK_6)
#define QTVDIAG_STATISTICS        (MSG_MASK_7)
#define QTVDIAG_UI_TASK           (MSG_MASK_8)
#define QTVDIAG_MP4_PLAYER        (MSG_MASK_9)
#define QTVDIAG_AUDIO_TASK        (MSG_MASK_10)
#define QTVDIAG_VIDEO_TASK        (MSG_MASK_11)
#define QTVDIAG_STREAMING         (MSG_MASK_12)
#define QTVDIAG_MPEG4_TASK        (MSG_MASK_13)
#define QTVDIAG_FILE_OPS          (MSG_MASK_14)
#define QTVDIAG_RTP               (MSG_MASK_15)
#define QTVDIAG_RTCP              (MSG_MASK_16)
#define QTVDIAG_RTSP              (MSG_MASK_17)
#define QTVDIAG_SDP_PARSE         (MSG_MASK_18)
#define QTVDIAG_ATOM_PARSE        (MSG_MASK_19)
#define QTVDIAG_TEXT_TASK         (MSG_MASK_20)
#define QTVDIAG_DEC_DSP_IF        (MSG_MASK_21)
#define QTVDIAG_BCAST_FLO         (MSG_MASK_22)
#ifdef FEATURE_QTV_GENERIC_BCAST
#define QTVDIAG_GENERIC_BCAST     (MSG_MASK_12)
#endif
#define QTVDIAG_STREAM_RECORDING  (MSG_MASK_12)
#define QTVDIAG_CONFIGURATION     (MSG_MASK_5)

#endif /* FEATURE_QTV_MSG_3_0 */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/*Diag has fixed its packet string arg size as 100, so when writing to daig
we need to limit our argument as well to 100 so that it won't crash*/
#define QTVDIAG_STR_ARG_SIZE 100

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

#endif  /* QTV_MSG_H */

