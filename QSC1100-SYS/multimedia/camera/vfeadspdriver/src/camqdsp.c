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

#include "clk.h"
#include "msg.h"             /* Message logging/reporting services         */
#include "qdsp.h"
#include "graph.h"
#include <string.h>          /* memcpy */

#ifdef FEATURE_QVPHONE
#include "qvp_msg_venc.h"
#include "camera.h"
#endif

#ifdef FEATURE_MP4_TRANSCODER
#include "transcoder_task.h"
#endif

#include "video_enc_aux.h"    /* For DSP and HW ME profiling purposes */

#define CAMQDSP_BITS_PER_WORD          32
/*#define CAMQDSP_BITS_PER_HALFWORD      (CAMQDSP_BITS_PER_WORD/2)*/
#define CAMQDSP_BYTES_PER_WORD         (CAMQDSP_BITS_PER_WORD / 8)
/*#define CAMQDSP_BYTES_PER_HALFWORD     (CAMQDSP_BITS_PER_HALFWORD/8)*/

typedef enum
{
  QDSP_DISABLED  = 0,          /* disabled state */
  QDSP_ENABLING,               /* image is being downloaded */
  QDSP_ENABLED                 /* download complete, ready for command. */
} CAMQDSP_QDSPDrvStateEnumType;

CAMQDSP_QDSPDrvStateEnumType CamQDSPState = QDSP_DISABLED;
CAMQDSP_ImageType            currentCamqdspApplication;

#ifdef FEATURE_QVPHONE
//Video Enc Frame Done Register + Buffer (3)
uint16 QVP_VideoEncFrameDoneBuf[2][4];
#endif

/* Callback function pointer provided by ARM in init function.  Will
   be set to a non-null value once initialized. */
CAMQDSP_MsgCBType    CamQDSP_CB = NULL;
static boolean        waitForVfeFrameMessage = TRUE;
static uint16 vfeVersion = 0;

qdsp_module_type module_list[CAMQDSP_SizeOfImageType];

/* Set when received QDSP service to reset the image. In _terminate(),
 * if set, ack to QDSP service then clear. */
static boolean ack_reset = FALSE;

/* Saves the current QDSP services prioritized application to terminate
** the correct application while transitioning out of voice call.
*/
static qdsp_app_type current_qdsp_app = QDSP_APP_CAMERA;
#define MSG_CAMQDSPIO(x,a,b,c)
//MSG_ERROR(x,a,b,c)

static void CAMQDSP_Callback (qdsp_event_data_type *pEvent);

#ifdef FEATURE_MP4_TRANSCODER
void CAMQDSP_Offline_Encode_Callback (const qdsp_event_data_type *pEvent,
                                      qdsp_module_type          app);
#endif

/* remove this include when camera feature file is available */
#include "camsensor.h"

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
/* psuedo isr function defined in ycbcr_qdsp.c */
extern void ycbcr_downsizer_q2a_isr ( void );
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */

/* DMA related functions */

/*
CAMQDSP_OperationalStateType CAMQDSP_get_state (void)
{
  CAMQDSP_OperationalStateType result;

  result = (CAMQDSP_OperationalStateType) *QDSP_vfeState;

  return result;
}

int CAMQDSP_get_Output1FrameMessageDropCount (void)
{
  int result;

  result = *QDSP_vfeOutput1FrameMessageDropCount;

  return result;
}

int CAMQDSP_get_Output2FrameMessageDropCount (void)
{
  int result;

  result = *QDSP_vfeOutput2FrameMessageDropCount;

  return result;
}

int CAMQDSP_get_CamifErrorCount (void)
{
  int result;

  result = *QDSP_vfeCamifErrorCount;

  return result;
}
*/
void CAMQDSP_put_GammaTable (const CAMQDSP_GammaTableType * gammaTable, uint32 size)
{
  if (CamQDSPState == QDSP_ENABLED)
  {
    vfeVersion = qdsp_read (QDSP_vfeVersion);
    MSG_HIGH("VFE Version = 0x%x", vfeVersion, 0, 0 );
#ifdef CAMERA_GAMMA_TABLE_IN_MEMC
    qdsp_block_write32 ((uint32*)QDSP_vfeGammaTable, 0, (uint32*)gammaTable, \
                      (uint32)size/4, FALSE);
#else /* CAMERA_GAMMA_TABLE_IN_MEMC */
    qdsp_block_write ((uint16*)QDSP_vfeGammaTable, 0, (uint16*)gammaTable, \
                      (uint32)size/2, FALSE);
#endif /* CAMERA_GAMMA_TABLE_IN_MEMC */
  }
}

/*===========================================================================

Function:  CAMQDSP_PUT_ROLLOFFTABLE

Description:
  DMA's vignette correction tables.

Return Value:
  None

Comments:
  MSMHW_VFE_ROLLOFF must be defined. DSP image must be enabled before
  accessing DMA variables.

Side Effects:

==============================================================================*/
/*lint -save -e715 rolloff maybe used later*/
void CAMQDSP_put_RolloffTable(const CAMQDSP_RolloffType *rolloff)
{
  if (CamQDSPState == QDSP_ENABLED)
  {
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
    /* This count should be derived as following:
     * QDSP_clock_period * count > 2 * PCLK_period.
     * With QDSP clock at 73.3824, period is 13.63ns.
     * At 15fps with possible support of VGA preview size, PCLK is at 12MHz,
     * or period at 83.3ns. Therefore:
     * 13.63 * 20 > 83.3 * 2, or
     * 272.6 > 166.6.
     * To verify that this number is correct, select the largest image
     * resolution, and do previewcircle.cmm or snapshotcircle.cmm file as
     * described in vfe.c file. The circle should not move in preview. */

#ifdef ROLLOFF_RESET_WAIT_CYCLE
    qdsp_write(QDSP_lensRolloffResetWaitCycles, ROLLOFF_RESET_WAIT_CYCLE);
#else /* ROLLOFF_RESET_WAIT_CYCLE */
    qdsp_write(QDSP_lensRolloffResetWaitCycles, 20);
#endif /* ROLLOFF_RESET_WAIT_CYCLE */
    memcpy ((void *) QDSP_vfeRolloffRadiusSquareTable, rolloff->RSTable, sizeof (CAMQDSP_RolloffRadiusSquareTableType));
    memcpy ((void *) QDSP_vfeRolloffSlopeAndCorrectFactorTable_R, rolloff->SCFTable_R, sizeof (CAMQDSP_RolloffSlopeAndCorrectFactorTableType));
    memcpy ((void *) QDSP_vfeRolloffSlopeAndCorrectFactorTable_G, rolloff->SCFTable_G, sizeof (CAMQDSP_RolloffSlopeAndCorrectFactorTableType));
    memcpy ((void *) QDSP_vfeRolloffSlopeAndCorrectFactorTable_B, rolloff->SCFTable_B, sizeof (CAMQDSP_RolloffSlopeAndCorrectFactorTableType));
#endif /* QDSP_MODULE_VFE25_DEFINED */
  }
} /* CAMQDSP_put_RolloffTable */
/*lint -restore */

/* End of DMA related functions */

/*===========================================================================

Function: CAMQDSP_SELECTMODULELIST

Description: This function selects the module table based on VFE.

Return Value:
   None

==============================================================================*/
void CAMQDSP_SelectModuleList(CAMQDSP_VfeType vfeType)
{
  int i=0;
  switch (vfeType)
  {
#ifdef QDSP_MODULE_VFE05_DEFINED
    case CAMQDSP_VFE05:
#if defined QDSP_MODULE_VFE05_CAPTURE_DEFINED
      module_list[CAMQDSP_VFE_CAPTURE] = QDSP_MODULE_VFE05_CAPTURE;
#endif
#if defined QDSP_MODULE_SA_DS05_DEFINED
      module_list[CAMQDSP_YCbCr_downsizer] = QDSP_MODULE_SA_DS05; 
#endif /* QDSP_MODULE_SA_DS05_DEFINED */
#if defined QDSP_MODULE_DS05_DEFINED
      module_list[CAMQDSP_YCbCr_downsizer] = QDSP_MODULE_DS05;
#endif /* QDSP_MODULE_DS05_DEFINED */
      module_list[CAMQDSP_VFE05] = QDSP_MODULE_VFE05;
#ifdef FEATURE_VIDEO_ENCODE
      module_list[CAMQDSP_MPEG4_ENC] = QDSP_MODULE_MP4E;
#endif /* FEATURE_VIDEO_ENCODE */
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_MPEG4_AAC_ENC] = QDSP_MODULE_MP4E_AAC;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
      module_list[CAMQDSP_H264_ENC] = QDSP_MODULE_H264E;
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_H264_AAC_ENC] = QDSP_MODULE_H264E_AAC;
#endif  /* FEATURE_CAMCORDER_AAC_REC */
#endif
      break;
#endif /* QDSP_MODULE_VFE05_DEFINED */

#ifdef QDSP_MODULE_VFE_DEFINED
    case CAMQDSP_VFE10:
      module_list[CAMQDSP_VFE] = QDSP_MODULE_VFE;
#ifdef FEATURE_MP4E_MULTI_VFE
      module_list[CAMQDSP_MPEG4_ENC] = QDSP_MODULE_MP4E_VFE;
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_MPEG4_AAC_ENC] = QDSP_MODULE_MP4E_AAC_VFE;
#endif  /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
      module_list[CAMQDSP_H264_ENC] = QDSP_MODULE_H264E_VFE;
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_H264_AAC_ENC] = QDSP_MODULE_H264E_AAC_VFE;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /* FEATURE_H264_ENCODER */
#else /* FEATURE_MP4E_MULTI_VFE */
#ifdef FEATURE_VIDEO_ENCODE
      module_list[CAMQDSP_MPEG4_ENC] = QDSP_MODULE_MP4E;
#endif /* FEATURE_VIDEO_ENCODE */
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_MPEG4_AAC_ENC] = QDSP_MODULE_MP4E_AAC;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
      module_list[CAMQDSP_H264_ENC] = QDSP_MODULE_H264E;
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_H264_AAC_ENC] = QDSP_MODULE_H264E_AAC;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /* FEATURE_H264_ENCODER */
#endif /* FEATURE_MP4E_MULTI_VFE */
      break;
#endif /* QDSP_MODULE_VFE_DEFINED */

#ifdef QDSP_MODULE_VFE1P_DEFINED
    case CAMQDSP_VFE1P:
      module_list[CAMQDSP_VFE] = QDSP_MODULE_VFE1P;
      module_list[CAMQDSP_MPEG4_ENC] = QDSP_MODULE_MP4E_VFE1P;
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_MPEG4_AAC_ENC] = QDSP_MODULE_MP4E_AAC_VFE1P;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
      module_list[CAMQDSP_H264_ENC] = QDSP_MODULE_H264E_VFE1P;
#ifdef FEATURE_CAMCORDER_AAC_REC
      module_list[CAMQDSP_H264_AAC_ENC] = QDSP_MODULE_H264E_AAC_VFE1P;
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif
      break;
#endif /* QDSP_MODULE_VFE1P_DEFINED */

#ifdef QDSP_MODULE_VFE20_DEFINED
    case CAMQDSP_VFE20:
      module_list[CAMQDSP_VFE] = QDSP_MODULE_VFE20;
#ifdef QDSP_MODULE_MP4E_VFE20_DEFINED
      module_list[CAMQDSP_MPEG4_ENC] = QDSP_MODULE_MP4E_VFE20;
#endif /* QDSP_MODULE_MP4E_VFE20_DEFINED */
#ifdef FEATURE_CAMCORDER_AAC_REC
#ifdef QDSP_MODULE_MP4E_AAC_VFE20_DEFINED
      module_list[CAMQDSP_MPEG4_AAC_ENC] = QDSP_MODULE_MP4E_AAC_VFE20;
#endif /* QDSP_MODULE_MP4E_AAC_VFE20_DEFINED */
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
#ifdef QDSP_MODULE_H264E_VFE20_DEFINED
      module_list[CAMQDSP_H264_ENC] = QDSP_MODULE_H264E_VFE20;
#endif /* QDSP_MODULE_H264E_VFE20_DEFINED */
#ifdef FEATURE_CAMCORDER_AAC_REC
#ifdef QDSP_MODULE_H264E_AAC_VFE20_DEFINED
      module_list[CAMQDSP_H264_AAC_ENC] = QDSP_MODULE_H264E_AAC_VFE20;
#endif /* QDSP_MODULE_H264E_AAC_VFE20 */
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /* FEATURE_H264_ENCODER */
      break;
#endif /* QDSP_MODULE_VFE20_DEFINED */

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_DEFINED */

    default:
      for(i=0;i<CAMQDSP_SizeOfImageType;++i)
       module_list[i]=QDSP_MODULE_MAX;
      break;

  }
}

/*===========================================================================

FUNCTION CAMQDSP_GetModule

DESCRIPTION
  This function returns the module selected by CAMQDSP_SelectModuleList for
  a particular application.

RETURN VALUE
  qdsp module id

SIDE EFFECTS
  None.
===========================================================================*/
qdsp_module_type CAMQDSP_GetModule(CAMQDSP_ImageType application)
{
    return module_list[application];
}

/*===========================================================================

FUNCTION CAMQDSP_GetCurrentModule

DESCRIPTION
  This function returns the current module of the running application. 

RETURN VALUE
  qdsp module id

SIDE EFFECTS
  None.
===========================================================================*/
qdsp_module_type CAMQDSP_GetCurrentModule(void)
{
    return module_list[currentCamqdspApplication];
}

/*===========================================================================

Function: CAMQDSP_INITIALIZE

Description: This function initializes the camera APP. QDSP codec.
             This is an asynchronous function. Caller of this routine
              will receive a return status.
             Upon successful downloading of the QDSP code, a callback
              will be invoked to notify the caller. This may take few hundred
              miliseconds.

Return Value:
   CAMQDSP_SUCCESS              cmd sucessful
   CAMQDSP_INVALID_CB           init cmd contains a NULL cb pointer
   CAMQDSP_INVALID_STATE        init cmd issued during invalid state
   CAMQDSP_QDSP_DWNLD_FAIL      qdsp video codec download failed

Comments: This function must be prior to calling any other function or issuing
          any qdsp commands.

Side Effects: Resets the QDSP and loads the camera app. codec image.

==============================================================================*/
CAMQDSP_ReturnCodeType CAMQDSP_Initialize
(
  CAMQDSP_MsgCBType     QDSPServiceCb,  /* qdsp service call back function */
  CAMQDSP_ImageType     application
)
{
  qdsp_isr_func_type dsp_isr_table[QDSP_ISR_MAX] = {NULL, NULL};

  if (ack_reset)
  {
    /* We should not get here.  This code is here to handle an unexpected error
    ** condition in case camera did not exit properly.  This code makes sure the
    ** QDSP task starts with a clean slate.  current_qdsp_app contains the previous
    ** camera application, it will be updated later on in this function to reflect
    ** the current call state.
    */
    ack_reset = FALSE;
    camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
    qdsp_disable_event_rsp(current_qdsp_app);
    MSG_ERROR("Unexpected ack_reset, previous app =%d", current_qdsp_app,0,0);
  }

  if (QDSPServiceCb == NULL)
  {
     MSG_HIGH("CAMQDSP_Initialize: Null call back pointer", 0, 0, 0 );
     return CAMQDSP_INVALID_PARM;
  }

  if (QDSP_MODULE_MAX == module_list[application])
  {
     MSG_HIGH("CAMQDSP_Initialize: Invalid Module", 0, 0, 0 );
     return CAMQDSP_INVALID_PARM;
  }

  CamQDSP_CB         = QDSPServiceCb;
  currentCamqdspApplication = application;

  /* QDSP image is being downloaded */
  CamQDSPState = QDSP_ENABLING;
  dsp_isr_table [QDSP_ISR_INT0] = CAMQDSP_isr;

#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  if (currentCamqdspApplication == CAMQDSP_YCbCr_downsizer)
  {
    current_qdsp_app = QDSP_APP_CAMERA;
    dsp_isr_table [QDSP_ISR_INT0] = ycbcr_downsizer_q2a_isr;
    qdsp_register_app (QDSP_APP_CAMERA, dsp_isr_table, CAMQDSP_Callback,
                      (int)QDSP_EV_COMMAND | (int)QDSP_EV_MODULE);
    qdsp_enable( QDSP_APP_CAMERA, module_list [application]);
  }
  else
#endif /*QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED*/
  if (camera_phone_state == IN_VOICE_CALL)
  {
#if defined QDSP_MODULE_VFE05_CAPTURE_DEFINED
    if(application == CAMQDSP_VFE_CAPTURE)
    {
       current_qdsp_app = QDSP_APP_CAMERA;
       qdsp_register_app (QDSP_APP_CAMERA, dsp_isr_table, CAMQDSP_Callback,
                          (int)QDSP_EV_COMMAND | (int)QDSP_EV_MODULE);
   
       qdsp_enable( QDSP_APP_CAMERA, module_list [application]);
    
    }
    else
    {
#endif /* QDSP_MODULE_VFE05_CAPTURE_DEFINED*/
#ifdef QDSP_MODULE_VFE25_SA_DEFINED
#error code not present
#endif /* QDSP_MODULE_VFE25_SA_DEFINED */
    current_qdsp_app = QDSP_APP_CAMERA_PROXY;
    qdsp_register_app (QDSP_APP_CAMERA_PROXY, dsp_isr_table, CAMQDSP_Callback,
                       (int)QDSP_EV_COMMAND | (int)QDSP_EV_MODULE);

    qdsp_enable( QDSP_APP_CAMERA_PROXY, module_list [application]);
#if defined QDSP_MODULE_VFE05_CAPTURE_DEFINED || defined QDSP_MODULE_VFE25_SA_DEFINED
    }
#endif /* QDSP_MODULE_VFE25_DEFINED */
   
  }
  else 
  {
    current_qdsp_app = QDSP_APP_CAMERA;
    qdsp_register_app (QDSP_APP_CAMERA, dsp_isr_table, CAMQDSP_Callback,
                       (int)QDSP_EV_COMMAND | (int)QDSP_EV_MODULE);

    qdsp_enable( QDSP_APP_CAMERA, module_list [application]);
  }


  return CAMQDSP_SUCCESS;

} /* CAMQDSP_Initialize */


#ifdef FEATURE_MP4_TRANSCODER

/*===========================================================================

Function: CAMQDSP_OFFLINE_ENCODE_INITIALIZE

Description: This function initializes the camera APP. QDSP codec.
             This is an asynchronous function. Caller of this routine
              will receive a return status.
             Upon successful downloading of the QDSP code, a callback
              will be invoked to notify the caller. This may take few hundred
              miliseconds.

Return Value:
   CAMQDSP_SUCCESS              cmd sucessful
   CAMQDSP_INVALID_CB           init cmd contains a NULL cb pointer
   CAMQDSP_INVALID_STATE        init cmd issued during invalid state
   CAMQDSP_QDSP_DWNLD_FAIL      qdsp video codec download failed

Comments: This function must be prior to calling any other function or issuing
          any qdsp commands.

Side Effects: Resets the QDSP and loads the camera app. codec image.

==============================================================================*/
CAMQDSP_ReturnCodeType CAMQDSP_Offline_Encode_Initialize
(
  CAMQDSP_MsgCBType     QDSPServiceCb,  /* qdsp service call back function */
  CAMQDSP_ImageType     application
)
{
  qdsp_isr_func_type dsp_isr_table[QDSP_ISR_MAX] = {NULL, NULL};
  if (ack_reset)
  {
    ack_reset = FALSE;
    camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
    qdsp_disable_event_rsp(current_qdsp_app);
    MSG_ERROR("Unexpected ack_reset, previous app =%d", current_qdsp_app,0,0);
  }

  if (QDSPServiceCb == NULL)
  {
     MSG_HIGH("CAMQDSP_Initialize: Null call back pointer", 0, 0, 0 );
     return CAMQDSP_INVALID_PARM;
  }

  CamQDSP_CB         = QDSPServiceCb;
  currentCamqdspApplication = application;
  //setNotCoded = 1
  /* QDSP image is being downloaded */
  CamQDSPState = QDSP_ENABLING;
  //dsp_isr_table [QDSP_ISR_INT0] = CAMQDSP_Offline_isr;
  dsp_isr_table[QDSP_ISR_INT0] = xcoder_qdsp_isr0;

  current_qdsp_app = QDSP_APP_IGG;
  qdsp_register_app (QDSP_APP_IGG, dsp_isr_table,
                     //CAMQDSP_Offline_Encode_Callback,
                     transcoder_qdsp_callback,
                     (int)QDSP_EV_COMMAND | (int)QDSP_EV_MODULE);
  //qdsp_enable( QDSP_APP_CAMERA, module_list [application]);

  return CAMQDSP_SUCCESS;

} /* CAMQDSP_Offline_Encode_Initialize */
#endif

/*===========================================================================

Function: CAMQDSP_REGISTER

Description: This function registers callback to CAMQDSP_DRV in the case
             there is no need to install DSP image.

Return Value:
   CAMQDSP_SUCCESS              cmd sucessful
   CAMQDSP_INVALID_CB           init cmd contains a NULL cb pointer

Comments: This function must be prior to calling any other function or issuing
          any qdsp commands.

Side Effects: None

==============================================================================*/
CAMQDSP_ReturnCodeType   CAMQDSP_Register
(
  CAMQDSP_MsgCBType      QDSPServiceCb  /* qdsp service call back function */
)
{
  if (QDSPServiceCb != NULL)
  {
    CamQDSP_CB   = QDSPServiceCb;
    CamQDSPState = QDSP_ENABLED;
    return CAMQDSP_SUCCESS;
  }
  else
  {
    return CAMQDSP_INVALID_PARM;
  }
} /* CAMQDSP_register */

/*===========================================================================

Function: CAMQDSP_DEREGISTER

Description: This function deregisters callback to CAMQDSP_DRV in the case
             there is no need to uninstall DSP image.

Return Value:
   None

Comments:

Side Effects: None

==============================================================================*/
void CAMQDSP_Deregister(void)
{
  CamQDSP_CB   = NULL;
  CamQDSPState = QDSP_DISABLED;
}

/*===========================================================================
Function: CAMQDSP_TERMINATE

Description: This function terminates the CAMQDSP codec.

Return Value: none

Comments: This function terminates all activities of camera App. on the QDSP,
         inclduing DME transfers.

Side Effects: None

==============================================================================*/
void CAMQDSP_Terminate( void )
{
  CamQDSPState = QDSP_DISABLED;
  if (ack_reset)
  {
    ack_reset = FALSE;
    camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
    qdsp_disable_event_rsp(current_qdsp_app);
    MSG_HIGH("Disable event response for application %d", current_qdsp_app,0,0);
  }
  qdsp_disable (current_qdsp_app);
} /* camqdsp_terminate */


CAMQDSP_ReturnCodeType CAMQDSP_VFE_Command
(
   CAMQDSP_VFE_CommandType  command,
   void                    *commandBuffer,
   int                      commandBufferSize
)
{
  uint32  command_register_value, current_command;
  uint32 *commandStream = (uint32 *) commandBuffer;
  boolean irq_mode;
  uint32  save, i;

  /* First, make sure that the unit has been initialized.  We do this
     by checking the callback pointer to make sure that it's non-null. */
  if ((CamQDSP_CB == NULL) || (CamQDSPState != QDSP_ENABLED))
  {
    return CAMQDSP_INVALID_STATE;
  }


  /* "Since there is no command queue within the QDSP, the microprocessor
     must first ensure that vfeCommandReg is cleared before overwriting it."
     The caller is responsible for throttling commands, so if there QDSP is
     not ready, this is an error. */
  waitForVfeFrameMessage = TRUE;
  command_register_value = qdsp_read (QDSP_vfeCommandReg);

  irq_mode = rex_is_in_irq_mode();
  if (command_register_value != 0)
  {
    /* Retry once after 5ms or upto 1ms if in IRQ mode */
    if(irq_mode)
    {
      /* Wait for upto 1000us */
      for (i=0; i<10; i++)
      {
        clk_busy_wait(100);
        command_register_value = qdsp_read (QDSP_vfeCommandReg);
        if (command_register_value == 0)
        {
          break;
        }
      }
    }
    else
    {
      camera_timed_wait(5);
    }
    command_register_value = qdsp_read (QDSP_vfeCommandReg);
    if (command_register_value != 0)
    {
      MSG_ERROR("Command register was %x not 0 comm:%x", command_register_value, command, 0);
      return CAMQDSP_QDSP_BUSY;
    }
  }

  /* "... the microprocessor writes the desired command header and command body into
     vfeCommandReg and vfeCommandBuf, respectively." */

  camera_log(LOG_DSPC, 0, 0, (int)command, __LINE__);
  current_command = (((int)command & 0x0000FFFF) << 16) |
                            (commandBufferSize & 0x0000FFFF);


  /* Lock interrupt to prevent overwriting of command buffer and register */
  INTLOCK_SAV(save);
  if (irq_mode == FALSE)
  {
    /* If we are not in IRQ mode, double check that no new command in IRQ.
     * If found, wait for upto 1000us */
    for (i=0; i<10; i++)
    {
      command_register_value = qdsp_read (QDSP_vfeCommandReg);
      if (command_register_value == 0)
      {
        break;
      }
      clk_busy_wait(100);
    }
    if (command_register_value != 0)
    {
      MSG_ERROR("Command register was %x, will overwrite with:%x", command_register_value, command, 0);
    }
  }
  if (commandBufferSize > 0)
  {
     qdsp_block_write32 (QDSP_vfeCommandBuf, 0, commandStream, \
                        (uint32)commandBufferSize, FALSE);
  }

  qdsp_write         (QDSP_vfeCommandReg, (uint32) current_command);
  INTFREE_SAV(save);

  /* "The microprocessor issues a microprocessor command interrupt to the QDSP */
  qdsp_up_to_dsp_irq ();


  return CAMQDSP_SUCCESS;
}

#ifdef FEATURE_VIDEO_ENCODE


/* OK to call from INTERRUPT TASK
**
** The start BE command is called from INT task because BE timing is
** very critical -- If BE time is delayed by as little as 5 msec we may
** miss our frame rate and start dropping frames
*/
CAMQDSP_ReturnCodeType CAMQDSP_VidEnc_Command
(
   CAMQDSP_VidEnc_CommandType  command,
   void                       *commandBuffer,
   int                         commandBufferSize
)
{
  /*
  return CAMQDSP_Command (QDSP_videoEncCommandReg,
                          (uint32) command,
                          commandBuffer,
                          commandBufferSize,
                          CAMQDSP_16_BIT);
  */

  uint16  command_register_value;
  uint16 *commandStream = (uint16 *) commandBuffer;
  boolean irq_mode;
  uint32  save, i;

  /* First, make sure that the unit has been initialized.  We do this
     by checking the callback pointer to make sure that it's non-null. */
  if ((CamQDSP_CB == NULL) || (CamQDSPState != QDSP_ENABLED))
  {
    return CAMQDSP_INVALID_STATE;
  }

  MSG_CAMQDSPIO("BE Command %x (vfestate=%x bestate=%x)",
            command,qdsp_read(QDSP_vfeState),qdsp_read(QDSP_videoEncState));
  MSG_CAMQDSPIO(" t=%d",mp4e_get_time_ms(),0,0);

  /* "Since there is no command queue within the QDSP, the microprocessor
     must first ensure that vfeCommandReg is cleared before overwriting it."
     The caller is responsible for throttling commands, so if there QDSP is
     not ready, this is an error. */
  waitForVfeFrameMessage = TRUE;
  command_register_value = qdsp_read (QDSP_videoEncCommandReg);

  irq_mode = rex_is_in_irq_mode();
  if (command_register_value != 0)
  {
    if(irq_mode)
    {
      /* Wait for upto 1000us */
      for (i=0; i<10; i++)
      {
        clk_busy_wait(100);
        command_register_value = qdsp_read (QDSP_vfeCommandReg);
        if (command_register_value == 0)
        {
          break;
        }
      }
    }
    else
    {
      camera_timed_wait(5);
    }
    command_register_value = qdsp_read(QDSP_videoEncCommandReg);
    if(command_register_value != 0 )
    {
      MSG_ERROR("Command register was %x not 0", command_register_value, 0, 0);
      return CAMQDSP_QDSP_BUSY;
    }
  }

  /* To avoid a race condition, we must first write the command buffer before
  ** writing the command itself
  */

  INTLOCK_SAV(save);
  if (irq_mode == FALSE)
  {
    /* If we are not in IRQ mode, double check that no new command in IRQ.
     * If found, wait for upto 1000us */
    for (i=0; i<10; i++)
    {
      command_register_value = qdsp_read (QDSP_vfeCommandReg);
      if (command_register_value == 0)
      {
        break;
      }
      clk_busy_wait(100);
    }
    if (command_register_value != 0)
    {
      MSG_ERROR("Command register was %x, will overwrite with:%x", command_register_value, command, 0);
    }
  }
  if (commandBufferSize > 0)
  {
     qdsp_block_write (QDSP_videoEncCommandBuf, 0, commandStream, \
                      (uint32)commandBufferSize, FALSE);
  }
  qdsp_write         (QDSP_videoEncCommandReg, (uint16) command);
  INTFREE_SAV(save);

  /* "The microprocessor issues a microprocessor command interrupt to the QDSP */
  qdsp_up_to_dsp_irq ();

  return CAMQDSP_SUCCESS;

}

#endif /* FEATURE_VIDEO_ENCODE */


/*===========================================================================

FUNCTION CAMQDSP_isr

DESCRIPTION
  This function determines the type of interrupt from the QDSP and it will
  report appropriate action to the Graphic thread to process the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void CAMQDSP_isr( void)
{
  uint32                     TempReg;
#ifdef FEATURE_VIDEO_ENCODE
  uint16                     TempShortReg;
#endif /* FEATURE_VIDEO_ENCODE */
  CAMQDSP_ResponseType       DSPMessage;

  if (CamQDSPState != QDSP_ENABLED) return;

  /* We need to call this function to wake up the 5-ms tick to update
   * clk_uptime_val. This value is used by BREW to calculate the amount
   * of time spent in or out of BREW. If this value is not updated correctly,
   * BREW might think it spend more time than allowed. */
#ifdef FEATURE_REX_PREEMPT_INTS
  clk_unexpected_enable();
#endif /* FEATURE_REX_PREEMPT_INTS */

  /* Check the error status from the DSP */
  TempReg = (*QDSP_vfeMsgAHBOverflowReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AHB_OVERFLOW_ERROR;

    *QDSP_vfeMsgAHBOverflowReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects AHB overflow error %d", TempReg, 0, 0);
  }

  TempReg = (*QDSP_vfeMsgCamifErrorReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CAMIF_ERROR;

    *QDSP_vfeMsgCamifErrorReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects CAMIF error %d", TempReg, 0, 0);
  }

  TempReg = (*QDSP_vfeMsgMicroDMAOverflowReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR;

    *QDSP_vfeMsgMicroDMAOverflowReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects Micro DMA overflow error %d", TempReg, 0, 0);
  }

    /* Check for Epoch interrupt message */
  TempReg = (*QDSP_vfeMsgEpochReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    /* Active Epoch Interrupt */
    DSPMessage.responseMsg = CAMQDSP_EPOCH_INTR;

    /* Clear the Interrupt */
    *QDSP_vfeMsgEpochReg  = 0;

    if (CamQDSP_CB)
    {
      CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_EPOCH_INTR %d",mp4e_get_time_ms(),0,0);
  }


  /* Check the illegal command status from the DSP */
  TempReg = (*QDSP_vfeMsgIllegalCommandReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    word command;
    DSPMessage.responseMsg = CAMQDSP_MSG_ILLEGAL_COMMAND;
/*lint -save -e740, casting of address to an uint32* is needed in this case*/
    qdsp_block_read32 (QDSP_vfeMsgIllegalCommandBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.illegalCmd,
                       sizeof (CAMQDSP_IllegalCommandMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    command = DSPMessage.responsePayload.illegalCmd.cmdCode;
    camera_log(LOG_DSPILL, 0, 0, command, __LINE__);

    *QDSP_vfeMsgIllegalCommandReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects illegal cmd %d %d",
        DSPMessage.responsePayload.illegalCmd.vfeState,
        DSPMessage.responsePayload.illegalCmd.cmdCode, 0);
  }

  TempReg = (*QDSP_vfeMsgConfigCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CONFIG_COMPLETE;

    *QDSP_vfeMsgConfigCompleteReg = 0;

#ifdef FEATURE_VIDEO_ENCODE
    DSPMessage.responsePayload.DSPStateReg = qdsp_read(QDSP_videoEncState);
#endif /* FEATURE_VIDEO_ENCODE */

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_CONFIG_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgIdleCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_IDLE_COMPLETE;

    *QDSP_vfeMsgIdleCompleteReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_IDLE_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgUpdateCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_UPDATE_COMPLETE;

    *QDSP_vfeMsgUpdateCompleteReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_UPDATE_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = qdsp_read (QDSP_vfeMsgOutput1EndOfFrameReg);

  if ((TempReg >> 16) != 0)
  {
    waitForVfeFrameMessage = FALSE;
    DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT1_END_OF_FRAME;

    qdsp_block_read32 (QDSP_vfeMsgOutput1EndOfFrameBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.endOfFrame,
                       (TempReg & (uint32) 0x0000FFFF),
                       FALSE);

    qdsp_write (QDSP_vfeMsgOutput1EndOfFrameReg, 0);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_OUTPUT1_END_OF_FRAME %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = qdsp_read (QDSP_vfeMsgOutput2EndOfFrameReg);
  if ((TempReg >> 16) != 0)
  {
    waitForVfeFrameMessage = FALSE;
    DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT2_END_OF_FRAME;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
    MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME,NULL,0);
#endif
    qdsp_block_read32 (QDSP_vfeMsgOutput2EndOfFrameBuf, 0, \
                      (uint32 *) &DSPMessage.responsePayload.endOfFrame, \
                      (TempReg & (uint32) 0x0000FFFF),
                      FALSE);

    qdsp_write (QDSP_vfeMsgOutput2EndOfFrameReg, 0);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_OUTPUT2_END_OF_FRAME %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = qdsp_read (QDSP_vfeMsgAFStatsReg);
  if ((TempReg >> 16) != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AF_STATS_COMPLETE;

    qdsp_block_read32 (QDSP_vfeMsgAFStatsBuf, 0, \
                       (uint32 *) &DSPMessage.responsePayload.AFStats, \
                       (TempReg & (uint32) 0x0000FFFF), \
                       FALSE);

    qdsp_write (QDSP_vfeMsgAFStatsReg, 0);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_AF_STATS_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = qdsp_read (QDSP_vfeMsgAEAWBStatsReg);
  if ((TempReg >> 16) != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AE_AWB_STATS_COMPLETE;

    qdsp_block_read32 (QDSP_vfeMsgAEAWBStatsBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.AE_AWBStats,
                       (TempReg & (uint32) 0x0000FFFF),
                       FALSE);

    qdsp_write (QDSP_vfeMsgAEAWBStatsReg, 0);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_AE_AWB_STATS_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = qdsp_read (QDSP_vfeMsgHistStatsReg);
  if ((TempReg >> 16) != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_HIST_STATS_COMPLETE;

#ifdef QDSP_MODULE_VFE20_DEFINED
    qdsp_block_read32 (QDSP_vfeMsgHistStatsBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.HistStats,
                       (TempReg & (uint32) 0x0000FFFF),
                       FALSE);
#endif /* QDSP_MODULE_VFE20_DEFINED */
    qdsp_write (QDSP_vfeMsgHistStatsReg, 0);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_HIST_STATS_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgCamifDoneReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CAMIF_DONE;

    *QDSP_vfeMsgCamifDoneReg = 0;

    if (CamQDSP_CB)
    {
      CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_CAMIF_DONE %d",mp4e_get_time_ms(),0,0);
 if(waitForVfeFrameMessage)
 {
    TempReg = qdsp_read (QDSP_vfeMsgOutput1EndOfFrameReg);
    if ((TempReg >> 16) != 0)
    {
      waitForVfeFrameMessage = FALSE;
      DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT1_END_OF_FRAME;

      qdsp_block_read32 (QDSP_vfeMsgOutput1EndOfFrameBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.endOfFrame,
                       (TempReg & (uint32) 0x0000FFFF),
                       FALSE);

      qdsp_write (QDSP_vfeMsgOutput1EndOfFrameReg, 0);

      if (CamQDSP_CB)
      {
        CamQDSP_CB(&DSPMessage);
      }
        MSG_CAMQDSPIO("CAMQDSP_MSG_OUTPUT1_END_OF_FRAME %d",mp4e_get_time_ms(),0,0);
    }

    TempReg = qdsp_read (QDSP_vfeMsgOutput2EndOfFrameReg);
    if ((TempReg >> 16) != 0)
    {
      waitForVfeFrameMessage = FALSE;
      DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT2_END_OF_FRAME;
     #ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_OUTPUT2_END_OF_FRAME,NULL,0);
     #endif
     qdsp_block_read32 (QDSP_vfeMsgOutput2EndOfFrameBuf, 0, \
                      (uint32 *) &DSPMessage.responsePayload.endOfFrame, \
                      (TempReg & (uint32) 0x0000FFFF),
                      FALSE);

     qdsp_write (QDSP_vfeMsgOutput2EndOfFrameReg, 0);

     if (CamQDSP_CB)
     {
        CamQDSP_CB(&DSPMessage);
     }
     MSG_CAMQDSPIO("CAMQDSP_MSG_OUTPUT2_END_OF_FRAME %d",mp4e_get_time_ms(),0,0);
    }
  }
  }

#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */

#if defined  QDSP_MODULE_VFE05_DEFINED || defined QDSP_MODULE_VFE25_DEFINED
  TempReg = (*QDSP_vfeMsgInvalidConfigParamReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_INVALID_CONFIG_PARAMETER;

    *QDSP_vfeMsgInvalidConfigParamReg = 0;

    if (CamQDSP_CB) 
    {
      CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_INVALID_CONFIG_PARAMETER_DONE %d",mp4e_get_time_ms(),0,0);
  }
#endif /*QDSP_MODULE_VFE05_DEFINED*/

  /* Only check Video Encode registers if we're running the Video Image */
  if (currentCamqdspApplication == CAMQDSP_MPEG4_ENC
#ifdef FEATURE_CAMCORDER_AAC_REC
      || currentCamqdspApplication == CAMQDSP_MPEG4_AAC_ENC
#endif  /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
      || currentCamqdspApplication == CAMQDSP_H264_ENC
#ifdef FEATURE_CAMCORDER_AAC_REC
      || currentCamqdspApplication == CAMQDSP_H264_AAC_ENC
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif
      )
  {

#ifdef FEATURE_VIDEO_ENCODE
    TempShortReg = *QDSP_videoEncFrameDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_FRAME_DONE;

      if (TempShortReg == 0x1)
      {
        DSPMessage.responsePayload.videoFrameDone.pingPongBuffer = CAMQDSP_BUFFER1;
        DSPMessage.responsePayload.mybuf[0] = 1;
      }
      else if (TempShortReg == 0x02)
      {
        DSPMessage.responsePayload.videoFrameDone.pingPongBuffer = CAMQDSP_BUFFER2;
        DSPMessage.responsePayload.mybuf[0] = 2;
      }
      else {
        /* Unexpected value in QDSP_videoFrameDoneReg */
        MSG_ERROR ("CAMQDSP: Unexpected val from videoEncFrameDoneReg%d", (int) TempShortReg, 0, 0);
        return;
      }
      qdsp_block_read (QDSP_videoEncFrameDoneBuf, 0,
                       (uint16 *) &DSPMessage.responsePayload.mybuf[1],
                       7,
                       FALSE);

      *QDSP_videoEncFrameDoneReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_VIDEO_FRAME_DONE,
                                NULL, MP4E_TIMELOG_FRAME_DSP);
#endif
      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_FRAME_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncSelectDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_SELECTION_DONE;

      *QDSP_videoEncSelectDoneReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SELECTION_DONE,NULL,0);
#endif
      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_SELECTION_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncSleepDoneReg;
    if (TempShortReg != 0 )
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_SLEEP_DONE;
      DSPMessage.responsePayload.DSPStateReg = qdsp_read(QDSP_videoEncState);

      *QDSP_videoEncSleepDoneReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_SLEEP_DONE,NULL,0);
#endif
      if ( CamQDSP_CB )
      {
        CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_SLEEP_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncConfigDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_CONFIG_DONE;

      *QDSP_videoEncConfigDoneReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_CONFIG_DONE,NULL,0);
#endif
      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_CONFIG_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncIdleDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_IDLE_DONE;

      *QDSP_videoEncIdleDoneReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_IDLE_DONE,NULL,0);
#endif
      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_IDLE_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncUpdateDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE_DONE;

      *QDSP_videoEncUpdateDoneReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
      MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_VIDEO_ENCODE_FEATURE_UPDATE_DONE,NULL,0);
#endif
      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE_DONE %d",mp4e_get_time_ms(),0,0);
    }

#endif /* FEATURE_VIDEO_ENCODE */

  }
} /* camqdsp_isr */

#ifdef FEATURE_QVPHONE
/*===========================================================================

FUNCTION CAMQDSP_qvp_isr

DESCRIPTION
  This function determines the type of interrupt from the QDSP and it will
  report appropriate action to the Graphic thread to process the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void CAMQDSP_qvp_isr( void)
{
  uint32                     TempReg;
  uint16                     TempShortReg;
  CAMQDSP_ResponseType       DSPMessage;

  if (CamQDSPState != QDSP_ENABLED) return;

  /* We need to call this function to wake up the 5-ms tick to update
   * clk_uptime_val. This value is used by BREW to calculate the amount
   * of time spent in or out of BREW. If this value is not updated correctly,
   * BREW might think it spend more time than allowed. */
#ifdef FEATURE_REX_PREEMPT_INTS
  clk_unexpected_enable();
#endif /* FEATURE_REX_PREEMPT_INTS */

  /* Check the error status from the DSP */
  TempReg = (*QDSP_vfeMsgAHBOverflowReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AHB_OVERFLOW_ERROR;

    *QDSP_vfeMsgAHBOverflowReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_ERROR ("CAMQDSP: VFE detects AHB overflow error %d", TempReg, 0, 0);
  }

  TempReg = (*QDSP_vfeMsgCamifErrorReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CAMIF_ERROR;

    *QDSP_vfeMsgCamifErrorReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_ERROR ("CAMQDSP: VFE detects CAMIF error %d", TempReg, 0, 0);
  }

  TempReg = (*QDSP_vfeMsgMicroDMAOverflowReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR;

    *QDSP_vfeMsgMicroDMAOverflowReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_ERROR ("CAMQDSP: VFE detects Micro DMA overflow error %d", TempReg, 0, 0);
  }

    /* Check for Epoch interrupt message */
  TempReg = (*QDSP_vfeMsgEpochReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    /* Active Epoch Interrupt */
    DSPMessage.responseMsg = CAMQDSP_EPOCH_INTR;

    /* Clear the Interrupt */
    *QDSP_vfeMsgEpochReg  = 0;

    if (CamQDSP_CB)
    {
      CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_LOW("CAMQDSP_EPOCH_INTR",0,0,0);
  }


  /* Check the illegal command status from the DSP */
  TempReg = (*QDSP_vfeMsgIllegalCommandReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_ILLEGAL_COMMAND;

    qdsp_block_read32 (QDSP_vfeMsgIllegalCommandBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.illegalCmd,
                       sizeof (CAMQDSP_IllegalCommandMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    *QDSP_vfeMsgIllegalCommandReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_ERROR ("CAMQDSP: VFE detects illegal cmd %d %d",
        DSPMessage.responsePayload.illegalCmd.vfeState,
        DSPMessage.responsePayload.illegalCmd.cmdCode, 0);
  }

  TempReg = (*QDSP_vfeMsgConfigCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CONFIG_COMPLETE;

    *QDSP_vfeMsgConfigCompleteReg = 0;

    DSPMessage.responsePayload.DSPStateReg = qdsp_read(QDSP_videoEncState);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_LOW("CAMQDSP_MSG_CONFIG_COMPLETE",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgIdleCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_IDLE_COMPLETE;

    *QDSP_vfeMsgIdleCompleteReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_LOW("CAMQDSP_MSG_IDLE_COMPLETE",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgUpdateCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_UPDATE_COMPLETE;

    *QDSP_vfeMsgUpdateCompleteReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_LOW("CAMQDSP_MSG_UPDATE_COMPLETE",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgOutput1EndOfFrameReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    waitForVfeFrameMessage = FALSE;
    DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT1_END_OF_FRAME;

    qdsp_block_read32 (QDSP_vfeMsgOutput1EndOfFrameBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.endOfFrame,
                       sizeof (CAMQDSP_EndOfFrameMessageType) / CAMQDSP_BYTES_PER_WORD, FALSE);

    *QDSP_vfeMsgOutput1EndOfFrameReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_LOW("CAMQDSP_MSG_OUTPUT1_END_OF_FRAME",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgOutput2EndOfFrameReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    waitForVfeFrameMessage = FALSE;
    DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT2_END_OF_FRAME;

    qdsp_block_read32 (QDSP_vfeMsgOutput2EndOfFrameBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.endOfFrame,
                       sizeof (CAMQDSP_EndOfFrameMessageType) / CAMQDSP_BYTES_PER_WORD, FALSE);

    *QDSP_vfeMsgOutput2EndOfFrameReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    QVP_MSG_VENC_LOW("CAMQDSP_MSG_OUTPUT2_END_OF_FRAME",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgAFStatsReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AF_STATS_COMPLETE;

    qdsp_block_read32 (QDSP_vfeMsgAFStatsBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.AFStats,
                       sizeof (CAMQDSP_AFStatsMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    *QDSP_vfeMsgAFStatsReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    QVP_MSG_VENC_LOW("CAMQDSP_MSG_AF_STATS_COMPLETE",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgAEAWBStatsReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AE_AWB_STATS_COMPLETE;

    qdsp_block_read32 (QDSP_vfeMsgAEAWBStatsBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.AE_AWBStats,
                       sizeof (CAMQDSP_AE_AWBStatsMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    *QDSP_vfeMsgAEAWBStatsReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    QVP_MSG_VENC_LOW("CAMQDSP_MSG_AE_AWB_STATS_COMPLETE",0,0,0);
  }

  TempReg = (*QDSP_vfeMsgHistStatsReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_HIST_STATS_COMPLETE;

    *QDSP_vfeMsgHistStatsReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    QVP_MSG_VENC_LOW("CAMQDSP_MSG_HIST_STATS_COMPLETE",0,0,0);
  }

  TempShortReg = *QDSP_videoEncFrameDoneReg;

  if (TempShortReg != 0 && TempShortReg != 0xFFFF)
  {
    int Indx;
    if (TempShortReg != 0x1 && TempShortReg != 0x02)
    {
      /* Unexpected value in QDSP_videoFrameDoneReg */
      QVP_MSG_VENC_ERROR ("CAMQDSP: Unexpected val from videoEncFrameDoneReg%d",
                          (int) TempShortReg, 0, 0);
      return;
    }

    Indx = TempShortReg - 1;

    QVP_VideoEncFrameDoneBuf[Indx][0] = TempShortReg;

    QVP_MSG_VENC_LOW ("CAMQDSP: CAMQDSP_VIDEO_FRAME_DONE, buffer %d", TempShortReg, 0, 0);
    qdsp_block_read (QDSP_videoEncFrameDoneBuf, 0,
                     (uint16 *) &QVP_VideoEncFrameDoneBuf[Indx][1], 3,FALSE);

    //JUNZ: Need to clear on QVP Encoder
    *QDSP_videoEncFrameDoneReg = 0xFFFF;

    //JUNZ: Need to make sure encode finish this pointer in time
    graph_video_enc_frame_done((byte*)(&(QVP_VideoEncFrameDoneBuf[Indx][0])));

  }


  TempShortReg = *QDSP_videoEncUpdateDoneReg;
  //Ack Video Enc Update, Video Enc Update Normally as Frame Rate
  if (TempShortReg != 0)
  {
    *QDSP_videoEncUpdateDoneReg = 0;

    QVP_MSG_VENC_LOW ("CAMQDSP: CAMQDSP_VIDEO_ENC_UPDATE_DONE", 0, 0, 0);
  }
} /* camqdsp_qvp_isr */
#endif


#ifdef FEATURE_VIDEO_ENCODE

/*===========================================================================

FUNCTION CAMQDSP_Offline_isr

DESCRIPTION
  This function determines the type of interrupt from the QDSP and it will
  report appropriate action to the Graphic thread to process the request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void CAMQDSP_Offline_isr( void)
{
  uint32                     TempReg;
  uint16                     TempShortReg;
  CAMQDSP_ResponseType       DSPMessage;

  if (CamQDSPState != QDSP_ENABLED) return;

  /* We need to call this function to wake up the 5-ms tick to update
   * clk_uptime_val. This value is used by BREW to calculate the amount
   * of time spent in or out of BREW. If this value is not updated correctly,
   * BREW might think it spend more time than allowed. */
#ifdef FEATURE_REX_PREEMPT_INTS
  clk_unexpected_enable();
#endif /* FEATURE_REX_PREEMPT_INTS */

  /* Check the error status from the DSP */
  TempReg = (*QDSP_vfeMsgAHBOverflowReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AHB_OVERFLOW_ERROR;

    *QDSP_vfeMsgAHBOverflowReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects AHB overflow error %d", TempReg, 0, 0);
  }

  TempReg = (*QDSP_vfeMsgCamifErrorReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CAMIF_ERROR;

    *QDSP_vfeMsgCamifErrorReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects CAMIF error %d", TempReg, 0, 0);
  }

  TempReg = (*QDSP_vfeMsgMicroDMAOverflowReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR;

    *QDSP_vfeMsgMicroDMAOverflowReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects Micro DMA overflow error %d", TempReg, 0, 0);
  }

    /* Check for Epoch interrupt message */
  TempReg = (*QDSP_vfeMsgEpochReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    /* Active Epoch Interrupt */
    DSPMessage.responseMsg = CAMQDSP_EPOCH_INTR;

    /* Clear the Interrupt */
    *QDSP_vfeMsgEpochReg  = 0;

    if (CamQDSP_CB)
    {
      CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_EPOCH_INTR %d",mp4e_get_time_ms(),0,0);
  }


  /* Check the illegal command status from the DSP */
  TempReg = (*QDSP_vfeMsgIllegalCommandReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_ILLEGAL_COMMAND;

    qdsp_block_read32 (QDSP_vfeMsgIllegalCommandBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.illegalCmd,
                       sizeof (CAMQDSP_IllegalCommandMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    *QDSP_vfeMsgIllegalCommandReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }

    MSG_ERROR ("CAMQDSP: VFE detects illegal cmd %d %d",
        DSPMessage.responsePayload.illegalCmd.vfeState,
        DSPMessage.responsePayload.illegalCmd.cmdCode, 0);
  }

  TempReg = (*QDSP_vfeMsgConfigCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_CONFIG_COMPLETE;

    *QDSP_vfeMsgConfigCompleteReg = 0;

    DSPMessage.responsePayload.DSPStateReg = qdsp_read(QDSP_videoEncState);

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_CONFIG_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgIdleCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_IDLE_COMPLETE;

    *QDSP_vfeMsgIdleCompleteReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_IDLE_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgUpdateCompleteReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_UPDATE_COMPLETE;

    *QDSP_vfeMsgUpdateCompleteReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_UPDATE_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgOutput1EndOfFrameReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    waitForVfeFrameMessage = FALSE;
    DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT1_END_OF_FRAME;

    qdsp_block_read32 (QDSP_vfeMsgOutput1EndOfFrameBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.endOfFrame,
                       sizeof (CAMQDSP_EndOfFrameMessageType) / CAMQDSP_BYTES_PER_WORD, FALSE);

    *QDSP_vfeMsgOutput1EndOfFrameReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_OUTPUT1_END_OF_FRAME %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgOutput2EndOfFrameReg) & (uint32)0xFFFF0000;
  TempReg >>= 16;
  if (TempReg != 0)
  {
    waitForVfeFrameMessage = FALSE;
    DSPMessage.responseMsg = CAMQDSP_MSG_OUTPUT2_END_OF_FRAME;

    qdsp_block_read32 (QDSP_vfeMsgOutput2EndOfFrameBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.endOfFrame,
                       sizeof (CAMQDSP_EndOfFrameMessageType) / CAMQDSP_BYTES_PER_WORD, FALSE);

    *QDSP_vfeMsgOutput2EndOfFrameReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_OUTPUT2_END_OF_FRAME %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgAFStatsReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AF_STATS_COMPLETE;

    qdsp_block_read32 (QDSP_vfeMsgAFStatsBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.AFStats,
                       sizeof (CAMQDSP_AFStatsMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    *QDSP_vfeMsgAFStatsReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_AF_STATS_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  TempReg = (*QDSP_vfeMsgAEAWBStatsReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_AE_AWB_STATS_COMPLETE;

    qdsp_block_read32 (QDSP_vfeMsgAEAWBStatsBuf, 0,
                       (uint32 *) &DSPMessage.responsePayload.AE_AWBStats,
                       sizeof (CAMQDSP_AE_AWBStatsMessageType) / CAMQDSP_BYTES_PER_WORD,
                       FALSE);

    *QDSP_vfeMsgAEAWBStatsReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_AE_AWB_STATS_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }
/*lint -restore*/
  TempReg = (*QDSP_vfeMsgHistStatsReg) & (uint32)0xFFFF0000;
  TempReg >>= 16 ;
  if (TempReg != 0)
  {
    DSPMessage.responseMsg = CAMQDSP_MSG_HIST_STATS_COMPLETE;

    *QDSP_vfeMsgHistStatsReg = 0;

    if (CamQDSP_CB)
    {
       CamQDSP_CB(&DSPMessage);
    }
    MSG_CAMQDSPIO("CAMQDSP_MSG_HIST_STATS_COMPLETE %d",mp4e_get_time_ms(),0,0);
  }

  /* Only check Video Encode registers if we're running the Video Image */
  if (currentCamqdspApplication == CAMQDSP_MPEG4_ENC
#ifdef FEATURE_CAMCORDER_AAC_REC
      || currentCamqdspApplication == CAMQDSP_MPEG4_AAC_ENC
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
      || currentCamqdspApplication == CAMQDSP_H264_ENC
#ifdef FEATURE_CAMCORDER_AAC_REC
      || currentCamqdspApplication == CAMQDSP_H264_AAC_ENC
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif
      )
  {
    TempShortReg = *QDSP_videoEncFrameDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_FRAME_DONE;

      if (TempShortReg == 0x1)
      {
        DSPMessage.responsePayload.videoFrameDone.pingPongBuffer = CAMQDSP_BUFFER1;
        DSPMessage.responsePayload.mybuf[0] = 1;
      }
      else if (TempShortReg == 0x02)
      {
        DSPMessage.responsePayload.videoFrameDone.pingPongBuffer = CAMQDSP_BUFFER2;
        DSPMessage.responsePayload.mybuf[0] = 2;
      }
      else {
        /* Unexpected value in QDSP_videoFrameDoneReg */
        MSG_ERROR ("CAMQDSP: Unexpected val from videoEncFrameDoneReg%d", (int) TempShortReg, 0, 0);
        return;
      }
      qdsp_block_read (QDSP_videoEncFrameDoneBuf, 0,
                       (uint16 *) &DSPMessage.responsePayload.mybuf[1],
                       3,
                       FALSE);

      *QDSP_videoEncFrameDoneReg = 0;

      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_FRAME_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncSelectDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_SELECTION_DONE;

      *QDSP_videoEncSelectDoneReg = 0;

      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_SELECTION_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncConfigDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_CONFIG_DONE;

      *QDSP_videoEncConfigDoneReg = 0;

      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_CONFIG_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncIdleDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_IDLE_DONE;

      *QDSP_videoEncIdleDoneReg = 0;

      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_IDLE_DONE %d",mp4e_get_time_ms(),0,0);
    }

    TempShortReg = *QDSP_videoEncUpdateDoneReg;
    if (TempShortReg != 0)
    {
      DSPMessage.responseMsg = CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE_DONE;

      *QDSP_videoEncUpdateDoneReg = 0;

      if (CamQDSP_CB)
      {
         CamQDSP_CB(&DSPMessage);
      }
      MSG_CAMQDSPIO("CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE_DONE %d",mp4e_get_time_ms(),0,0);
    }

  }

} /* CAMQDSP_Offline_isr */

#endif /* FEATURE_VIDEO_ENCODE */

/*===========================================================================

FUNCTION CAMQDSP_CALLBACK

DESCRIPTION
  This function will be called after the qdsp_enable function has
  completed.  This function is registered as a call back using the
  qdsp_register function.

DEPENDENCIES
  Must be registered with qdsp_register

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -save -e818 could be const * */
static void CAMQDSP_Callback
(
 qdsp_event_data_type *pEvent
)
{
  CAMQDSP_ResponseType DSPMessage;

  if (pEvent == NULL)
  {
   MSG_HIGH ("Call back with NULL event pointer", 0, 0, 0);
   return;
  }

  camera_log(LOG_QCB, 0, 0, 0, __LINE__);
  switch (pEvent->event)
  {
     case QDSP_EV_COMMAND:
         MSG_MED("Received QDSP command event (CAMERA)",0,0,0);

         break;

     case QDSP_EV_MODULE:
        if (pEvent->mod.mod_status == QDSP_MOD_READY)
        {
           if (pEvent->mod.module == module_list[currentCamqdspApplication])
           {
              camera_log(LOG_QCB, 0, 0, (int)IMG_ME, __LINE__);
              if (CamQDSPState == QDSP_ENABLING)
              {
                 camera_log(LOG_QCB, 0, 0, (int)IMG_MEOK, __LINE__);
                 CamQDSPState = QDSP_ENABLED;

                 /* Download of image is complete */
                 DSPMessage.responseMsg                 = CAMQDSP_DOWNLOAD_DONE;
                 DSPMessage.responsePayload.DSPStateReg = 0;
#ifdef FEATURE_VIDEO_ENCODE_DEBUG
                 MP4E_DEBUG_RECORD_TIMELOG(MP4E_TIMELOG_DSP2ARM_DOWNLOAD_DONE,NULL,0);
#endif
                 /* Call to notify download complete with qdspState  */
                 if (CamQDSP_CB)
                 {
                   CamQDSP_CB(&DSPMessage);
                 }

                 MSG_HIGH("QDSP image READY for app %d", current_qdsp_app, 0, 0 );
              }
           }
        }
        else
        /* QDSP service thread want to reset QDSP image */
        if (pEvent->mod.module == module_list[currentCamqdspApplication])
        {
           camera_log(LOG_QCB, 0, 0, (int)RST_ME, __LINE__);
           /* QDSP is requested to reset */
           if (CamQDSPState == QDSP_ENABLED)
           {
              camera_log(LOG_QCB, 0, 0, (int)RST_MEOK, __LINE__);

              /* QDSP image is not available */
              DSPMessage.responseMsg = CAMQDSP_SERVICE_UNAVAILABLE;

              ack_reset = TRUE;
              MSG_HIGH("Disable event for module %d", pEvent->mod.module,0,0);

              /* Call to notify App. QDSP functionality is not available */
              if (CamQDSP_CB) CamQDSP_CB(&DSPMessage);
           }
           else
           {
              camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
              qdsp_disable_event_rsp(current_qdsp_app);
              MSG_HIGH("Disable event response for application %d",
                  current_qdsp_app,0,0);
           }
        }
        else
        {
          MSG_HIGH("qdsp_disable request currentAPP:%d",current_qdsp_app,0,0);
          /* BVCM was added to use CAMQDSP so we need to handle it also NOW */
          if (currentCamqdspApplication == CAMQDSP_BVCM)
          {
            camera_log(LOG_QCB, 0, 0, (int)RST_ME, __LINE__);
            /* QDSP is requested to reset */
            if (CamQDSPState == QDSP_ENABLED)
            {
              camera_log(LOG_QCB, 0, 0, (int)RST_MEOK, __LINE__);

              /* QDSP image is not available */
              DSPMessage.responseMsg = CAMQDSP_SERVICE_UNAVAILABLE;
              ack_reset = TRUE;
              MSG_HIGH("Disable event for module %d", pEvent->mod.module,0,0);

              /* Call to notify App. QDSP functionality is not available */
              if (CamQDSP_CB) CamQDSP_CB(&DSPMessage);
            }
            else
            {
              camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
              qdsp_disable_event_rsp(current_qdsp_app);
              MSG_HIGH("Disable event response for application %d",
                  current_qdsp_app,0,0);
            }
          }
        }
        break;

     default:
        MSG_HIGH("CAMQDSP: Illegal QDSP Event type %d", pEvent->event, 0, 0);

  } /* switch case */
} /* CAMQDSP_Callback */
/*lint -restore */


#ifdef FEATURE_MP4_TRANSCODER




/*===========================================================================

FUNCTION CAMQDSP_Offline_Encode_CALLBACK

DESCRIPTION
  This function will be called after the qdsp_enable function has
  completed.  This function is registered as a call back using the
  qdsp_register function.

DEPENDENCIES
  Must be registered with qdsp_register

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/
void CAMQDSP_Offline_Encode_Callback
(
 const qdsp_event_data_type *pEvent,
 qdsp_module_type          app  
)
{
  CAMQDSP_ResponseType DSPMessage;

  if (pEvent == NULL)
  {
   MSG_HIGH ("CAMQDSP: call back with NULL event pointer", 0, 0, 0);
   return;
  }

  switch (pEvent->event)
  {
     case QDSP_EV_COMMAND:
         MSG_MED("Received QDSP command event (CAMERA)",0,0,0);

         break;

     case QDSP_EV_MODULE:
        if (pEvent->mod.mod_status == QDSP_MOD_READY)
        {
          /* xcoder will use only QDSP_MODULE_MP4E because VFE doesn't matter.
          ** so don't use module_list[currentCamqdspApplication]
          */
           //lint -e58  
     	   if (pEvent->mod.module == app )
           {
              if (CamQDSPState == QDSP_ENABLING)
              {
                 camera_log(LOG_QCB, 0, 0, (int)IMG_MEOK, __LINE__);
                 CamQDSPState = QDSP_ENABLED;

                 /* Download of image is complete */
                 DSPMessage.responseMsg                 = CAMQDSP_DOWNLOAD_DONE;
                 DSPMessage.responsePayload.DSPStateReg = 0;

                 /* Call to notify download complete with qdspState  */
                 graph_queue_offline_encode_qdsp_msg((int)DSPMessage.responseMsg,0,0);

                 MSG_HIGH("QDSP image READY for app %d", current_qdsp_app, 0, 0 );
              }
           }
           //lint +e58 
        }
        else
        /* QDSP service thread want to reset QDSP image */
        if (pEvent->mod.module == module_list[currentCamqdspApplication])
        {
           /* QDSP is requested to reset */
           if (CamQDSPState == QDSP_ENABLED)
           {
              CamQDSPState = QDSP_DISABLED;

              /* QDSP image is not available */
              DSPMessage.responseMsg = CAMQDSP_SERVICE_UNAVAILABLE;

              camera_log(LOG_QCB, 0, 0, (int)RST_MEOK, __LINE__);
              ack_reset = TRUE;
              MSG_HIGH("Disable event for module %d", pEvent->mod.module,0,0);

              /* Call to notify App. QDSP functionality is not available */
              if (CamQDSP_CB) CamQDSP_CB(&DSPMessage);
           }
           camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
           qdsp_disable_event_rsp(QDSP_APP_CAMERA);
           MSG_HIGH("Disable event response for application %d",
               current_qdsp_app,0,0);
        }
        else
        {
          MSG_FATAL("Unexpected qdsp_disable request",0,0,0);
        }
        break;

     default:
        MSG_HIGH("CAMQDSP: Illegal QDSP Event type %d", pEvent->event, 0, 0);

  } /* switch case */
} /* CAMQDSP_Offline_Encode_Callback */

#endif

/*===========================================================================

Function: CAMQDSP_FRAME_CLK_CB

Description: When expecting frame and no frame had come for certain amount of
             time (timeout), check it just in case interrupt was missing.

Return Value: NONE

Side Effects: None

==============================================================================*/
/*lint -save -e715, reserve ms for use in the future*/
void camqdsp_frame_clk_cb(int4 ms)
{
  if (waitForVfeFrameMessage)
  {
    CAMQDSP_isr ();
  }
}
/*lint -restore*/

/*===========================================================================
 *
 * Function: CAMQDSP_RESET_ACK
 *
 * Description: This function to indicates to QDSP Services that camera is 
 *              ready to be temporarily disabled. 
 *
 * Comments:    This function  allows qdsp to terminates all activities of 
 *              camera App. on the QDSP, inclduing DME transfers. When 
 *              available Camera QDSP code will be re-dowloaded.
 *
 * Return Value: NONE
 *
 * Side Effects: 
 *
 ==============================================================================*/
void CAMQDSP_Reset_Ack(void)
{
  if (ack_reset)
  {
    CamQDSPState = QDSP_ENABLING;
    ack_reset = FALSE;
    camera_log(LOG_QCB, 0, 0, (uint32)RST_ACK, __LINE__);
    qdsp_disable_event_rsp(current_qdsp_app);
    MSG_HIGH("Disable event response for application %d", current_qdsp_app,0,0);
  }
  else
  {
    MSG_HIGH("Unexpected qdsp_disable_event_rsp request",0,0,0);
  }
}


