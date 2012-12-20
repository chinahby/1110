#ifndef VDEC_LOG_H
#define VDEC_LOG_H
/* =======================================================================
                               qtv_log.h
DESCRIPTION
   definitions for Qtv Log Codes
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

$Header: //source/qcom/qct/multimedia/qtv/decoder/core/rel/2.0/inc/vdecoder_log.h#2 $
$DateTime: 2009/07/03 06:36:35 $
$Change: 958369 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/08/09    vs      Remove the dependencies of other modules.
--------   ---      ---------------------------------------------------------
========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "msg.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */

#define LOG_VDEC_ARM_VIDEO_DECODE_STATS                     (0x1BF + LOG_1X_BASE_C)
#define LOG_VDEC_VIDEO_FRAME_DECODE_INFO_C                  (0x1C3 + LOG_1X_BASE_C)
#define LOG_VDEC_DSP_SLICE_BUFFER_C                         LOG_QTV_DSP_SLICE_BUFFER_C
#define LOG_VDEC_VIDEO_BITSTREAM                            LOG_QTV_VIDEO_BITSTREAM
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

#ifndef PLATFORM_LTK

/* QTV Peak and average statistic */
LOG_RECORD_DEFINE(LOG_VDEC_ARM_VIDEO_DECODE_STATS)
  /* Average ARM decode time */
  uint32 average_dec_time_ms; 
  /* Average ARM decode time */
  uint32 peak_dec_time_ms;
  /* Type of video played*/
  uint8 video_type_played;
LOG_RECORD_END
typedef LOG_VDEC_ARM_VIDEO_DECODE_STATS_type log_vdec_arm_decode_stats_type;


/* Video Frame Stats */
LOG_RECORD_DEFINE(LOG_VDEC_VIDEO_FRAME_DECODE_INFO_C)
  /* Frame Type*/
  uint8 FrameType;
  /* SuperFrame ID */
  uint32 SuperFrameID;
  /* Layer decoded 0-Base,1-Enhanced,2-both*/
  uint8 Layer_Decoded;
  /* Intra MB's in base layer of the frame */
  uint16 NumIntraMBinBase; 
  /* Intra MB's in Enh layer of the frame */
  uint16 NumIntraMBinEnh; 
  /* Inter MB's in base layer of the frame */
  uint16 NumInterMBinBase; 
  /* Inter MB's in Enh layer of the frame */
  uint16 NumInterMBinEnh; 
  /* MacroBlocks in Error in base layer */
  uint16 NumErrorMBinBase; 
  /* MacroBlocks in Error in Enh layer */
  uint16 NumErrorMBinEnh; 
  /* Frame PTS */
  uint32 VideoFramePTS;
  /* IsInterpolated*/
  uint8 isInterpolated;
LOG_RECORD_END
typedef LOG_VDEC_VIDEO_FRAME_DECODE_INFO_C_type  log_vdec_VideoFrame_DecodeStats_type;


#define LOG_DSP_MAX_SLICE_BUFFER_SIZE 512
LOG_RECORD_DEFINE(LOG_VDEC_DSP_SLICE_BUFFER_C)
  uint16 ChunkNumber;
  uint16 DataSize;
  uint8  SliceBuffer[LOG_DSP_MAX_SLICE_BUFFER_SIZE];
LOG_RECORD_END
typedef LOG_VDEC_DSP_SLICE_BUFFER_C_type log_vdec_dsp_slice_buffer_type;

/* For video bitstream logging */
#define LOG_BITSTREAM_MAX_SIZE 512
LOG_RECORD_DEFINE(LOG_VDEC_VIDEO_BITSTREAM)
  uint16 ChunkNum;
  uint32 NBytes;
  uint8  Data[LOG_BITSTREAM_MAX_SIZE];
LOG_RECORD_END
typedef LOG_VDEC_VIDEO_BITSTREAM_type log_vdec_video_bitstream_type;
#endif /* ! PLATFORM_LTK */

/*------------------------------------------------------------------------------
                                         Message delecration                                                                      
------------------------------------------------------------------------------*/
#ifdef FEATURE_QTV_MSG_3_0

 /* Note: In 3.0, the first parameter to QTV_MSG() is no longer a mask!
  * It must be a single SSID.
  */

 /* VDEC Message Sub-Group Ids */

#define VDECDIAG_GENERAL           (MSG_SSID_APPS_QTV_GENERAL)
#define VDECDIAG_DEBUG             (MSG_SSID_APPS_QTV_DEBUG)
#define VDECDIAG_STATISTICS        (MSG_SSID_APPS_QTV_STATISTICS)
#define VDECDIAG_VIDEO_TASK        (MSG_SSID_APPS_QTV_VIDEO_TASK)
#define VDECDIAG_MPEG4_TASK        (MSG_SSID_APPS_QTV_MPEG4_TASK)
#define VDECDIAG_DEC_DSP_IF        (MSG_SSID_APPS_QTV_DEC_DSP_IF)
#define VDECDIAG_BCAST_FLO         (MSG_SSID_APPS_QTV_BCAST_FLO)

/* VDEC Message Priorities */
#define VDECDIAG_PRIO_LOW      (MSG_MASK_0)
#define VDECDIAG_PRIO_MED      (MSG_MASK_1)
#define VDECDIAG_PRIO_HIGH     (MSG_MASK_2)
#define VDECDIAG_PRIO_ERROR    (MSG_MASK_3)
#define VDECDIAG_PRIO_FATAL    (MSG_MASK_4)
#define VDECDIAG_PRIO_DEBUG    (MSG_MASK_5)

/* Message macros */

/* For compatibility -- use medium priority by default. */
#define VDEC_MSG(xx_ss_mask, xx_fmt) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send (&xx_msg_const); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
  /* lint +e717 */

#define VDEC_MSG_PRIO(xx_ss_mask, xx_prio, xx_fmt) \
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
#define VDEC_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
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
#define VDEC_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
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
#define VDEC_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
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
#define VDEC_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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
#define VDEC_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var(&xx_msg_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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
#define VDEC_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO6(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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
#define VDEC_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0)                                                             \
               /* lint +e717 */

#define VDEC_MSG_PRIO7(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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
#define VDEC_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_PRIO8(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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
#define VDEC_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
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

#define VDEC_MSG_PRIO9(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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
#define VDEC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_SPRINTF_PRIO_1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
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
#define VDEC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_SPRINTF_PRIO_2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
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
#define VDEC_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
  /*lint -e717 */ \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_SPRINTF_PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
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
#define VDEC_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  do { \
      XX_MSG_CONST ( xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_SPRINTF_PRIO_4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
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
#define VDEC_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  do { \
      XX_MSG_CONST (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
  } while (0) \
               /* lint +e717 */

#define VDEC_MSG_SPRINTF_PRIO_5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
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
#define VDEC_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST_FMT_VAR (xx_ss_mask, VDECDIAG_PRIO_MED, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
  } while (0)

#define VDEC_MSG_SPRINTF_FMT_VAR__PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
      XX_MSG_CONST_FMT_VAR (xx_ss_mask, xx_prio, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
  } while (0)

#else

/* Macros for VDEC Diagnostic messages 2.0*/

#define VDEC_MSG(xx_ss_mask, xx_fmt) \
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

#define VDEC_MSG_PRIO(xx_ss_mask, xx_prio, xx_fmt) VDEC_MSG(xx_ss_mask, xx_fmt) 

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameter.
---------------------------------------------------------------------------*/
#define VDEC_MSG1(xx_ss_mask, xx_fmt, xx_arg1) \
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

#define VDEC_MSG_PRIO1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) VDEC_MSG1(xx_ss_mask, xx_fmt, xx_arg1) 

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define VDEC_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
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
#define VDEC_MSG_PRIO2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) VDEC_MSG2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define VDEC_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
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

#define VDEC_MSG_PRIO3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  VDEC_MSG3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters. In this case the function
  called needs to have more than 4 parameters so it is going to be a slow 
  function call.  So for this case the  msg_send_var() uses var arg list 
  supported by the compiler.
---------------------------------------------------------------------------*/
#define VDEC_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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

#define VDEC_MSG_PRIO4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  VDEC_MSG4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define VDEC_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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

#define VDEC_MSG_PRIO5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  VDEC_MSG5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define VDEC_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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

#define VDEC_MSG_PRIO6(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  VDEC_MSG6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define VDEC_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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

#define VDEC_MSG_PRIO7(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
  VDEC_MSG7(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define VDEC_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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

#define VDEC_MSG_PRIO8(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
  VDEC_MSG8(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) 

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define VDEC_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
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

#define VDEC_MSG_PRIO9(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) \
  VDEC_MSG9(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define VDEC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
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

#define VDEC_MSG_SPRINTF_PRIO_1(xx_ss_mask, xx_prio, xx_fmt, xx_arg1) \
   VDEC_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define VDEC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
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
#define VDEC_MSG_SPRINTF_PRIO_2(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2) \
   VDEC_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define VDEC_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
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

#define VDEC_MSG_SPRINTF_PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
   VDEC_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define VDEC_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
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

#define VDEC_MSG_SPRINTF_PRIO_4(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
   VDEC_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define VDEC_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
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

#define VDEC_MSG_SPRINTF_PRIO_5(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
   VDEC_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) 

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define VDEC_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
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

#define VDEC_MSG_SPRINTF_FMT_VAR_PRIO_3(xx_ss_mask, xx_prio, xx_fmt, xx_arg1, xx_arg2, xx_arg3) VDEC_MSG_SPRINTF_FMT_VAR_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

/* VDEC Message Priorities (not used) */

#define VDECDIAG_PRIO_DEBUG    (0)
#define VDECDIAG_PRIO_LOW      (0)
#define VDECDIAG_PRIO_MED      (0)
#define VDECDIAG_PRIO_HIGH     (0)
#define VDECDIAG_PRIO_ERROR    (0)
#define VDECDIAG_PRIO_FATAL    (0)

 /* VDEC Message Sub-Group Ids */

#define VDECDIAG_GENERAL           (MSG_MASK_5)
#define VDECDIAG_DEBUG             (MSG_MASK_6)
#define VDECDIAG_STATISTICS        (MSG_MASK_7)
#define VDECDIAG_VIDEO_TASK        (MSG_MASK_11)
#define VDECDIAG_MPEG4_TASK        (MSG_MASK_13)
#define VDECDIAG_DEC_DSP_IF        (MSG_MASK_21)
#define VDECDIAG_BCAST_FLO         (MSG_MASK_22)

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
#define VDECDIAG_STR_ARG_SIZE 100

/* =======================================================================
**                        Function Declarations
** ======================================================================= */


#endif  /* VDEC_LOG_H */

