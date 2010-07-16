/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  Camera QDSP Interface

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/vfeadspdriver/main/latest/src/camqdsp.c#7 $ $Author: c_caluru $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/09   chai    MSG_INVALID_CONFIG_PARAMETER
02/12/08   cg      Added Support for SA VFE image to avoid AHB overflows for 5 MP sensor.
12/24/08   cg      Changes for AEC Multishot Feature
11/25/08   hap     Changes made to resolve AHB overflow issues
11/17/08   sm      changes to ensure that camera does not work when h/w is not detected
07/04/08   ps      Removed RVCT compiler warnings
10/15/07   sanal   Changes to remove linker errors when 
                   FEATURE_VIDEO_ENCODE_DEBUG is disabled 
09/12/07   Yogi    Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
04/27/07   ven     Featurized Camcoder functionality under FEATURE_VIDEO_ENCODE
04/05/07   ven     Added support for both Stand alone downsizer and In call downsizer in single build.
04/04/07   dle     Back out 2/7/07. _APP_CAMERA is enum, not define.
03/14/07   klee    Removed unnecessary conditional statement which is always TRUE
02/07/07   dle     Need to use _APP_IGG if _APP_CAMERA is not defined.
02/05/07   dhaval  Replaced QDSP_APP_CAMERA with QDSP_APP_IGG in 
                   CAMQDSP_Offline_Encode_Initialize for transcoder. 
01/10/07   sigu    ARM side changes for integrating the incall downsizer for 6260 target.
01/08/07    hz     Replaced QDSP_APP_IGG with QDSP_APP_CAMERA.
12/12/06   dle     Need to lock interrupt when updating command buffer and
                   command reg. Also, if not in IRQ, need to double check if
                   command reg is NULL. If not, wait for up to 1000us.
12/05/06   dle     If in IRQ use clk_busy_wait(500), else camera_timed_wait(5).
11/20/06   jv      Update value of current_qdsp_app before register and enable.
11/14/06   ven     Wrapped the necessary code under the feature 
                   definion FEATURE_VIDEO_ENCODE_DEBUG
10/09/06   nhk     Fix in QVP_FRAME_DONE ISR for access conflict of QVP variable
09/14/06   jv      Redesign camera termination. New function CAMQDSP_Reset_Ack.
09/04/06   dhaval  Added transcoder definition using transcoder_task.h file
08/21/06   sigu    Added the required S/W changes to incorporate the new vfe 
                   message vfeMsgInvalidConfigparamreg.
08/08/06   sigu    Corrected a problem with YCbCr downsizer when using incall.
08/07/06   sigu    Integrated ycbcr downsizer for 6260 target
07/14/06   jdas    Added CAMQDSP_GetCurrentModule() API
06/16/06   Shahed  Renamed the module names and fixed the feature names for
                   VFE20+VIDEO ENCODER related module names.
05/26/06   dle     Removed CAMQDSP_LastCommand, lint error and not needed.
05/19/06   jv      Added #ifdef statements in CAMQDSP_SelectModuleList to
                   compile for MSM6800B
05/19/06   dle     Using qdsp_block_writexx for gamma table. Need to know
                   select the right function.
04/20/06   sch     Remove CAMERA_VFE_HW_RESET for easier integration of
                   VFE H/W roll-off fix accross MSM6xxx targets
04/02/06   sch     VFE20 Support.
03/27/06   wyh     Add 5ms sleep when BE is busy in CAMQDSP_VidEnc_Command()
02/17/06   dle     When reset is requested, make qdsp_disable_event_rsp()
                   is called one way or the other.
02/16/06   dle     Linted.
02/09/06   wyh     Removed checking videoEncState at CAMQDSP_VFE_Command()
02/02/06   dle     Replaced LOCAL with static. Added vfeVersion.
02/01/06   dle     Added vfeVersion, retrieve each time put_GammaTable.
12/16/05   dle     Added camera_log() for command to DSP and illegal command
                   response from DSP.
12/09/05   why     Added CAMQDSP_GetModule() API
12/03/05   wyh     Xcoder will only use module MP4E
11/09/05   dle     Use ROLLOFF_RESET_WAIT_CYCLE defined in custcamera.h file
                   so that it can be customized for different MSM.
11/03/05   anb     Added support for CAMIF_DONE ISR.
10/14/05   dle     Renamed VFE1X to VFE1P.
10/12/05   jch     Lint Error removal.
10/11/05   wyh     Reverse the order of writing DSP command buffer & command
                   in CAMQDSP_VidEnc_Command() to avoid a race condition.
10/03/05   cla     Modified video recording profiling code to support debugging
                   using QXDM.
09/07/05   vlc     In CAMQDSP_Initialize(), send response to QDSP task using
                   the saved application rather than relying on current state.
09/02/05   anb     Added support for in call voice hand off.
08/25/05   dle     When put rolloff table, also program
                   QDSP_lensRolloffResetWaitCycles.
08/16/05  dw       Updated video recording profiling and event logging system
08/18/05   dle     module_list is now run time generated by new function
                   CAMQDSP_SelectModuleList().
                   CAMQDSP_put_GammaTable() now needs size parameter.
                   Modified to use new rolloff variables.
06/23/05   dle     Obsolete FEATURE_QDSP_USES_MODULES.
                   Enable camera_log.
                   In CAMQDSP_VFE_Command(), if not ready, wait for 5ms and
                   retry.
07/28/05   sch     Added CAMQDSP_put_RolloffTable.
07/11/05   dl      Added FEATURE_CAMERA_BVCM feature flags
06/24/05   anb     Fixed QDSP crash by replying to the request to release the
                   image.
06/22/05   dle     Commented out CAMQDSP_put_* functions. Not used, and also
                   blindly accessing DSP memory.
                   Modified CAMQDSP_put_GammaTable() to only copy to DSP when
                   CamQDSPState is QDSP_ENABLED.
06/01/05   jk      Added new entries to CAMQDSP_ImageType for concurrent DSP.
05/25/05   mvd     Camera will now request the "VFE" module in preparation
                   for In-Call support.
05/05/05   dl      Added BVCM support.
02/22/05   wh      Timing info in MSG_CAMQDSPIO()
02/18/05   sch     Merged Jun Zhang's changes for VFE and Decoder Race
                   condition for QVideoPhone application. Move VFE command
                   register writing after command buffer writing.
02/15/05   dw      Added logic analyzer hooks with timelog under VIDEO_TIMETEST
02/03/05   dw      Added code for camcorder time analysis under VIDEO_TIMETEST
01/25/05   qcam/tv Independent Qcamcorder/Qtv transcoding.
01/12/05   jz      Added QVideophone Support for 6550
12/05/04   wyh     Handle the videoEncSleepDoneReg DSP Message
12/02/04   ali     Support for Transcoder
12/01/04   wyh     Do not reset DSP when camcorder backend starts.
11/30/04   mvd     Changes for new VFE/Camcorder image 0x1008.  Adds error
                   msgs and splits stat reporting.
11/29/04   wyh     Added MSG_CAMQDSPIO()
11/19/04   wyh     Use qdsp_reset_and_enable() instead of qdsp_enable() to
                   put DSP in init state at each start of record.
11/19/04   wyh     Make sure not to send backend select command if
                   videoEncState is not INIT.
11/11/04   drl     Always load camcorder image so that we can get DTMF tones
                   and key press tones.
10/04/04   mvd     Renamed to "camqdsp.c"
---------------------- OLD History of camqdsp_if.c --------------------------
09/16/04   lrw     Add interrupt entry for Epoch
09/07/04   drl     Fixed incorrect QDSP_vfe_MsgFatalErrorReg reference.
08/24/04   dle     Redo the state machine and handles QDSP reset.
8/19/04    sj      Removed Audio from this file (done in qdsp layer).
                   Renamed dsp image from qcamcorder to mp4e.
                   Removed tabs from file.
7/12/04    drl     Renamed CAMQDSP_Command to CAMQDSP_VFE_Command and added
                   CAMQDSP_VideEnc_Command.
6/23/04    mvd     Changed CAMQDSP_put_GammaTable() input arg to be a pointer
                   to gamma array.
3/26/04    drl     Creation
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                       INCLUDE FILES FOR MODULE


===========================================================================*/
#include "camqdsp.h"
#include "camerai.h"

#ifdef CAMERA_USES_SOFTDSP

#endif // CAMERA_USES_SOFTDSP

