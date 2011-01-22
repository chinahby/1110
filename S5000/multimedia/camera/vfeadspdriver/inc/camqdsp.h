#ifndef CAMQDSP_H
#define CAMQDSP_H

/*=============================================================================
FILE:       CAMQDSP.h

SERVICES:   Camera QDSP Interface

DESCRIPTION:
   This file provides the internal interfaces to the QDSP.

LIMITATIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright © 2004-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary

 $Header: //source/qcom/qct/multimedia/camera/6k/vfeadspdriver/main/latest/inc/camqdsp.h#5 $ $DateTime: 2009/02/25 02:35:41 $ $Author: c_caluru $

CAMIF Version Numbers:

ABSTRACT:
   
============================================================================*/
/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/09   chai    enabled MSG_INVALID_CONFIG_PARAMETER
02/12/09   cg      Changes for supporting SA VFE image to avoid AHB overflows
12/28/08   cg      Changes for AEC Multishot 1.0
11/25/08   hap     Changes made to resolve AHB overflow issues
09/03/07   Yogi    Replacing FEATURE_AAC_REC with FEATURE_CAMCORDER_AAC_REC in camcorder module 
05/02/07   ven     Changed the packed struct to regular struct since this sturct
                   is used only with in ARM s/w,
02/08/07   dle     Fixed lint errors by replacing enums with defines.
01/10/07   sigu    ARM side changes for integrating the incall downsizer for 6260 target.
09/14/06   jv      Redesign camera termination. New function CAMQDSP_Reset_Ack.
08/30/06   wyh     Unit test for VFE Reconfigure during video record
08/21/06   sigu    Added the required S/W changes to incorporate the new vfe 
                   message vfeMsgInvalidConfigparamreg.
08/07/06   sigu    Integrated ycbcr downsizer for 6260 target.
07/14/06   jdas    Added CAMQDSP_GetCurrentModule() API
04/02/06   sch     VFE20 Support
03/27/06   wyh     Add command to run output2enc_ack2criteria() in GRAPH task
02/16/06   dle     Linted.
01/04/06   ali     Added CAMQDSP_CMD_MAX to CAMQDSP_VFE_CommandType
12/09/05   why     Added CAMQDSP_GetModule() API
11/03/05   anb     Added support for CAMIF_DONE ISR.
10/14/05   dle     Renamed VFE1X to VFE1P.
08/22/05   dle     Corrected CAMQDSP_RolloffSlopeAndCorrectFactorTableType.
08/18/05   dle     Rolloff table is now 32 entries. Corrected some typo.
                   Added CAMQDSP_SelectModuleList() and modified
                   CAMQDSP_put_GammaTable() to have size parameter.
07/08/05   sch     Rolloff variables.
06/30/05   sch     MSMHW_VFE_10BIT_LUT -> MSMHW_VFE_10BIT_LUT
06/20/05   dle     Added MSMHW_VFE_10BIT_LUT.
06/01/05   jk      Added new entries to CAMQDSP_ImageType for concurrent DSP.
05/05/05   dl      Added BVCM support.
02/03/05   dw      Modified VideoEncFrameDoneBuf to include HW ME diagnostics
12/05/04   wyh     Added the SLEEP command and the SLEEP_DONE message.
12/02/04   ali     Support for Transcoder
11/30/04   mvd     Changes for new VFE/Camcorder image 0x1008
11/19/04   wyh     Make sure not to send backend select command if videoEncState
                   is not INIT.
10/04/04   mvd     Renamed to "camqdsp.h"
---------------------- OLD History of camqdsp_if.h ------------------------------
09/16/04   lrw     Add CAMQDSP_EPOCH_INTR  
08/27/04   sch     Support for CMD_END_OF_FRAME_ACK1, CMD_END_OF_FRAME_ACK2
08/24/04   dle     Added CAMQDSP_SERVICE_UNAVAILABLE.
                   Changed pMsgInfo param in CAMQDSP_MsgCBType() to pointer.
08/19/04   sj      Removed Audio support from this file(done in qdsp layer).
07/07/04   drl     Added ImageType argument to CAMQDSP_Initialize, renamed
                   CAMQDSP_Command to CAMQDSP_VFE_Command and added
                   CAMQDSP_VidEnc_Command.
06/23/04   mvd     Added VB_GainType for legacy camif code.  
                   Redefined CAMQDSP_GammaTableType and changed 
                   CAMQDSP_put_GammaTable() input arg to be a pointer to 
                   gamma array.
03/25/04   drl     Creation

===========================================================================*/

/*===========================================================================*/
/*                                INCLUDE FILES                              */
/*===========================================================================*/

#include <comdef.h>
#include <qdspext.h>

#ifndef CAMERA_USES_SOFTDSP
/*===========================================================================*/
/*                                CONSTANTS                                  */
/*===========================================================================*/

#define ROLLOFF_RADIUS_SQUARE_TABLE_ENTRIES  32
#define ROLLOFF_MESH_HORIZONTAL_GRIDS        16
#define ROLLOFF_MESH_VERTICAL_GRIDS          12
#define ROLLOFF_MESH_INIT_TABLE_ENTRIES      (ROLLOFF_MESH_VERTICAL_GRIDS + 1)
#define ROLLOFF_MESH_DELTA_TABLE_ENTRIES     ((ROLLOFF_MESH_VERTICAL_GRIDS + 1) * \
                                              (ROLLOFF_MESH_HORIZONTAL_GRIDS))
#define ROLLOFF_MESH_CF_TABLE_ENTRIES        ((ROLLOFF_MESH_VERTICAL_GRIDS + 1) * \
                                              (ROLLOFF_MESH_HORIZONTAL_GRIDS + 1))


/* Unit test to artificially generate vfe reconfigures every 2 seconds
*/
//#define CAMCORDER_VFE_RECONFIG_TEST


/*===========================================================================*/
/*                               DATA TYPES                                  */
/*===========================================================================*/

typedef enum CAMQDSP_VfeType
{
  CAMQDSP_VFE05,
  CAMQDSP_VFE10,
  CAMQDSP_VFE1P,
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  CAMQDSP_YCbCr_DOWNSIZE,
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */
  CAMQDSP_VFE20,
  CAMQDSP_VFE25
} CAMQDSP_VfeType;

typedef enum CAMQDSP_ReturnCodeType
{
   CAMQDSP_SUCCESS,              /* cmd sucessful */

   /* Download done return codes */
   CAMQDSP_QDSP_DOWNLD_FAIL,     /* qdsp video codec download failed */
   CAMQDSP_INVALID_STATE,
   CAMQDSP_QDSP_BUSY,

   /* cmd pointers errors */
   CAMQDSP_INVALID_BUF,

   /* QDSP command error return codes */
   CAMQDSP_INVALID_CMD,          /* invalid QDSP command specified */
   CAMQDSP_INVALID_PARM,         /* invalid QDSP command parameter */
   CAMQDSP_MAX_RETURN_CODES
} CAMQDSP_ReturnCodeType;

typedef enum CAMQDSP_ImageType
{
  CAMQDSP_VFE,
  CAMQDSP_MPEG4_ENC,
#ifdef FEATURE_CAMCORDER_AAC_REC
  CAMQDSP_MPEG4_AAC_ENC,
#endif /* FEATURE_CAMCORDER_AAC_REC */
#ifdef FEATURE_H264_ENCODER
  CAMQDSP_H264_ENC,
#ifdef FEATURE_CAMCORDER_AAC_REC
  CAMQDSP_H264_AAC_ENC,
#endif /* FEATURE_CAMCORDER_AAC_REC */
#endif /* FEATURE_H264_ENCODER */
  CAMQDSP_BVCM,                  /* Bayer Vignette Correction Module */
#if defined QDSP_MODULE_DS05_DEFINED || \
  defined QDSP_MODULE_SA_DS05_DEFINED
  CAMQDSP_YCbCr_downsizer,                  /* ycbcr downsizer */
#endif /* QDSP_MODULE_DS05_DEFINED || QDSP_MODULE_SA_DS05_DEFINED */
#ifdef QDSP_MODULE_VFE05_CAPTURE_DEFINED
  CAMQDSP_VFE_CAPTURE,   /* Used in Snapshot */
#endif
#ifdef QDSP_MODULE_VFE25_SA_DEFINED
#error code not present
#endif

  CAMQDSP_SizeOfImageType        /* Used for size only */
} CAMQDSP_ImageType;

/* These are the Microprocessor commands.  These are issued from the CPU to the
   VDSP to perform a VDSP operation */

typedef enum CAMQDSP_VFE_CommandType
{
  CAMQDSP_CMD_CONFIG_VIDEO        =   0x700,
  CAMQDSP_CMD_CONFIG_RAW_SNAPSHOT =   0x701,
  CAMQDSP_CMD_CONFIG_SNAPSHOT     =   0x702,
  CAMQDSP_CMD_CONFIG_OFFLINE      =   0x703,
  CAMQDSP_CMD_CAPTURE             =   0x704,
  CAMQDSP_CMD_IDLE                =   0x705,
  CAMQDSP_CMD_UPDATE              =   0x706,
  CAMQDSP_CMD_END_OF_FRAME_ACK1   =   0x707,
  CAMQDSP_CMD_END_OF_FRAME_ACK2   =   0x708,
  CAMQDSP_CMD_AF_STATS_START      =   0x709,
  CAMQDSP_CMD_AF_STATS_STOP       =   0x70a,
  CAMQDSP_CMD_AE_AWB_STATS_REQ    =   0x70b,
  CAMQDSP_CMD_HIST_STATS_REQ      =   0x70c,
  CAMQDSP_CMD_MAX                 =   0xfff,
} CAMQDSP_VFE_CommandType;

typedef enum CAMQDSP_VidEnc_CommandType
{
  CAMQDSP_VIDEO_ENCODE_SELECTION      = 0x500,
  CAMQDSP_VIDEO_ENCODE_CONFIGURATION  = 0x501,
  CAMQDSP_VIDEO_ENCODE_START          = 0x502,
  CAMQDSP_VIDEO_ENCODE_IDLE           = 0x503,
  CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE = 0x504,
  CAMQDSP_VIDEO_ENCODE_SLEEP          = 0x505
} CAMQDSP_VidEnc_CommandType;

/* These are QDSP videoEncStates */
typedef enum CAMQDSP_VidEnc_StateType{
  CAMQDSP_VIDEO_ENC_STATE_VIDEO_INIT   = 0xFFFF,
  CAMQDSP_VIDEO_ENC_STATE_VIDEO_IDLE   = 0x1,
  CAMQDSP_VIDEO_ENC_STATE_VIDEO_CAPTURE= 0x2
} CAMQDSP_VideoEncStateType;

/* These are the QDSP response messages.  These are issued from the QDSP to the 
   microprocessor. */

typedef enum CAMQDSP_ResponseMsgType
{
  CAMQDSP_DOWNLOAD_DONE,
  CAMQDSP_MSG_CONFIG_COMPLETE,
  CAMQDSP_MSG_OUTPUT1_END_OF_FRAME,
  CAMQDSP_MSG_OUTPUT2_END_OF_FRAME,
  CAMQDSP_MSG_IDLE_COMPLETE,
  CAMQDSP_MSG_ILLEGAL_COMMAND,
  CAMQDSP_MSG_AHB_OVERFLOW_ERROR,
  CAMQDSP_MSG_CAMIF_ERROR,
  CAMQDSP_MSG_MICRO_DMA_OVERFLOW_ERROR,
  CAMQDSP_MSG_UPDATE_COMPLETE,
  CAMQDSP_MSG_AF_STATS_COMPLETE,
  CAMQDSP_MSG_AE_AWB_STATS_COMPLETE,
  CAMQDSP_MSG_HIST_STATS_COMPLETE,
  CAMQDSP_VIDEO_FRAME_DONE,
  CAMQDSP_VIDEO_ENCODE_SELECTION_DONE,
  CAMQDSP_VIDEO_ENCODE_CONFIG_DONE,
  CAMQDSP_VIDEO_ENCODE_IDLE_DONE,
  CAMQDSP_VIDEO_ENCODE_FEATURE_UPDATE_DONE,
  CAMQDSP_VIDEO_ENCODE_SLEEP_DONE,
  CAMQDSP_EPOCH_INTR,
  CAMQDSP_SERVICE_UNAVAILABLE,
  CAMQDSP_MSG_CAMIF_DONE,
#ifdef FEATURE_CAMERA_AEC_DURING_MULTISHOT
#error code not present
#endif /* FEATURE_CAMERA_AEC_DURING_MULTISHOT */
#if defined QDSP_MODULE_VFE05_DEFINED || defined QDSP_MODULE_VFE25_DEFINED
  CAMQDSP_MSG_INVALID_CONFIG_PARAMETER,
#endif /*QDSP_MODULE_VFE05_DEFINED*/
  CAMQDSP_OUTPUT2ACK2_CRITERIA,
#ifdef CAMCORDER_VFE_RECONFIG_TEST
  CAMQDSP_VFE_RECONFIGURE_UNIT_TEST,
#endif
} CAMQDSP_ResponseMsgType;

typedef uint32 *CAMQDSP_AddressType;

typedef enum CAMQDSP_ReadyType
{
  CAMQDSP_NOT_READY,
  CAMQDSP_READY
} CAMQDSP_ReadyType;

/* Used for frameCorruption field */
#define CAMQDSP_NOT_CORRUPTED 0 
#define CAMQDSP_CORRUPTED     1

/* Used for gain config */
typedef enum
{
  GAIN_DISABLED,
  GAIN_FIXED,
  GAIN_AUTO
} VB_GainType;

typedef enum CAMQDSP_ValidType
{
  CAMQDSP_NOT_VALID,
  CAMQDSP_VALID
} CAMQDSP_ValidType;

typedef __packed struct CAMQDSP_EndOfFrameMessageType
{
  unsigned int           frameCorruption         :  1;
  unsigned int           reserved                : 31;

  CAMQDSP_AddressType    output_Y_Address;
  CAMQDSP_AddressType    output_CbCr_Address;
  
#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else

  uint16                 averageBlackFromActive  : 10;
  unsigned int           reserved1               : 22;
#endif /* QDSP_MODULE_VFE25_DEFINED */
  uint16                 greenDefectPixelCount   : 8;
  uint16                 redBlueDefectPixelCount : 8;
  unsigned int           reserved2               : 16;
  uint16                 frame_seq_num           : 16;
  uint16                                         : 16;
} CAMQDSP_EndOfFrameMessageType;

typedef __packed struct CAMQDSP_IllegalCommandMessageType
{
  uint32            vfeState             : 16;
  unsigned int      cmdCode              : 16;

} CAMQDSP_IllegalCommandMessageType;

typedef __packed struct CAMQDSP_AFStatsMessageType
{
  uint32            focusValue             : 27;
  unsigned int      reserved1              :  5;

  uint16            numFocus               : 9;
  unsigned int      reserved2              :23;
  uint16            frame_seq_num          : 16;
  uint16                                   : 16;
} CAMQDSP_AFStatsMessageType;

typedef __packed struct CAMQDSP_AE_AWBStatsMessageType
{
  uint32 pixelsUsedForExposure : 32;
  uint16                 frame_seq_num     : 16;
  uint16                                   : 16;
} CAMQDSP_AE_AWBStatsMessageType;

#ifdef QDSP_MODULE_VFE20_DEFINED
typedef __packed struct CAMQDSP_HistStatsMessageType
{
  uint16                 frame_seq_num     : 16;
  uint16                                   : 16;
} CAMQDSP_HistStatsMessageType;
#endif /* QDSP_MODULE_VFE20_DEFINED */

typedef enum CAMQDSP_VideoFrameType
{
  CAMQDSP_INTRA_FRAME,
  CAMQDSP_INTER_FRAME
} CAMQDSP_VideoFrameType;

typedef enum CAMQDSP_VideoPacketFitsType
{
  CAMQDSP_PACKET_FITS,
  CAMQDSP_PACKET_DOES_NOT_FIT
} CAMQDSP_VideoPacketFitsType;

typedef enum CAMQDSP_VideoTimingType
{
  CAMQDSP_ONTIME,
  CAMQDSP_LATE
} CAMQDSP_VideoTimingType;

typedef enum CAMQDSP_VideoDataCorruptType
{
  CAMQDSP_OKAY,
  CAMQDSP_CORRUPT
} CAMQDSP_VideoDataCorruptType;

typedef enum CAMQDSP_Video_ARM_RequestType
{
  CAMQDSP_NO_REQUEST,
  CAMQDSP_NOT_CODED
} CAMQDSP_Video_ARM_RequestType;

typedef enum CAMQDSP_VideoPingPongBuffer
{
  CAMQDSP_BUFFER1,
  CAMQDSP_BUFFER2
} CAMQDSP_VideoPingPongBuffer;

typedef struct CAMQDSP_GammaTableType
{
  uint8 arr[1024];
} CAMQDSP_GammaTableType;

typedef __packed struct CAMQDSP_VideoFrameDoneType
{
  //CAMQDSP_VideoFrameType frameType : 16;
  uint16 frameType;

  CAMQDSP_Video_ARM_RequestType armRequest  :  1;
  CAMQDSP_VideoDataCorruptType  dataCorrupt :  1;
  CAMQDSP_VideoTimingType       armTiming   :  1;
  CAMQDSP_VideoTimingType       dspTiming   :  1;
  CAMQDSP_VideoPacketFitsType   packetFits  :  1;
  unsigned int                  reserved    : 11;

  uint16                        framePacketNum;
  uint16                        mbAvgFetchCycles;
  uint16                        mbAvgSearchCycles;
  uint16                        mbAvgNumMbSearch;
  uint16                        mbAvgNumBlkSearch;
} CAMQDSP_VideoFrameDoneType;

typedef __packed struct CAMQDSP_VideoFrameDoneWrapperType
{
  CAMQDSP_VideoPingPongBuffer pingPongBuffer : 16;
  CAMQDSP_VideoFrameDoneType  videoFrameDone;
} CAMQDSP_VideoFrameDoneWrapperType;

typedef struct CAMQDSP_ResponseType
{
  union
  {
    uint16                            DSPStateReg;
    CAMQDSP_EndOfFrameMessageType     endOfFrame;
    CAMQDSP_AFStatsMessageType        AFStats;
    CAMQDSP_AE_AWBStatsMessageType    AE_AWBStats;
#ifdef QDSP_MODULE_VFE20_DEFINED
    CAMQDSP_HistStatsMessageType      HistStats;
#endif /* QDSP_MODULE_VFE20_DEFINED */
    CAMQDSP_VideoFrameDoneWrapperType videoFrameDone;
    uint16                            mybuf[8];
    CAMQDSP_IllegalCommandMessageType illegalCmd;
  } responsePayload;
  /* Moved to the bottom of this structure since qdsp_memcpy32
   * does not support 16 bit aligned address. It supports only
   * 32 bit and 8 bit aligned address. */
  CAMQDSP_ResponseMsgType responseMsg;

} CAMQDSP_ResponseType;

/*
** Message processing callback function structure
*/
typedef void (* CAMQDSP_MsgCBType)
(
  CAMQDSP_ResponseType  *pMsgInfo
);

typedef enum CAMQDSP_OperationalStateType
{
  CAMQDSP_NOT_READY_STATE,
  CAMQDSP_INIT_STATE,
  CAMQDSP_IDLE_STATE,
  CAMQDSP_CAPTURE_STATE,
  CAMQDSP_TRANSITIONING_STATE,
  CAMQDSP_NumberOf_OperationalStateType   /* Used for count purposes only */
} CAMQDSP_OperationalStateType;

/* Table with the radius square values marking the boundaries of the 32 
 * segments of the vignette correction curve. 
 * Bits 23:0 of each entry contain the radius square in Q0 unsigned format 
 * and bits 31:24 are set to zero.  
 * The entries are arranged so that the radius values monotonically 
 * increase starting from 0.  
 */
typedef struct 
{
  uint32 radius_square : 24; /* Q0 unsigned */
  uint32 reserved      : 8;  /* Reserved, set to 0 */
} CAMQDSP_RolloffRadiusSquareTableType [ROLLOFF_RADIUS_SQUARE_TABLE_ENTRIES];

/* Table contains the correction factor offsets and slope values 
 * corresponding to the 32 radius intervals.  Bits 31:19 contain the slope 
 * in Q10 signed format. Bits 18:0 contain the correction factor offset in 
 * Q10 unsigned format
 */
typedef struct 
{
  int32 slope    : 19; /* Q30 signed */
  uint32 c_factor: 13; /* Q0 unsigned */
} CAMQDSP_RolloffSlopeAndCorrectFactorTableType [ROLLOFF_RADIUS_SQUARE_TABLE_ENTRIES];

/* Mesh rolloff table is in MEMC and it is packed in the following format:
 *           MSW                   LSW
 *       InitTable_Gr[0]       InitTable_R[0]
 *       InitTable_Gb[0]       InitTable_B[0]
 *       ...
 *       InitTable_Gr[12]      InitTable_R[12]
 *       InitTable_Gb[12]      InitTable_B[12]
 *       DeltaTable_Gr[0,0]    DeltaTable_R[0,0]
 *       DeltaTable_Gb[0,0]    DeltaTable_B[0,0]
 *       DeltaTable_Gr[0,1]    DeltaTable_R[0,1]
 *       DeltaTable_Gb[0,1]    DeltaTable_B[0,1]
 *       ...
 *       DeltaTable_Gr[12,15]  DeltaTable_R[12,15]
 *       DeltaTable_Gb[12,15]  DeltaTable_B[12,15]
 */
typedef __packed struct
{
  uint16 init_RB : 16; /* Q13 unsinged, LSW */
  uint16 init_G  : 16; /* Q13 unsinged, MSW */
} CAMQDSP_RolloffInitTableType[ROLLOFF_MESH_INIT_TABLE_ENTRIES * 2];

typedef __packed struct
{
  int16 delta_RB : 16; /* Q20 signed, LSW */
  int16 delta_G  : 16; /* Q20 signed, MSW */
} CAMQDSP_RolloffDeltaTableType[ROLLOFF_MESH_DELTA_TABLE_ENTRIES * 2];

#if defined QDSP_MODULE_VFE25_DEFINED
#error code not present
#else
typedef struct CAMQDSP_Rolloff
{
  CAMQDSP_RolloffRadiusSquareTableType           RSTable;
  CAMQDSP_RolloffSlopeAndCorrectFactorTableType  SCFTable_R;
  CAMQDSP_RolloffSlopeAndCorrectFactorTableType  SCFTable_G;
  CAMQDSP_RolloffSlopeAndCorrectFactorTableType  SCFTable_B;

} CAMQDSP_RolloffType;
#endif /* QDSP_MODULE_VFE25_DEFINED */

/*===========================================================================*/
/*                               FUNCTION DECLARATIONS                       */
/*===========================================================================*/

/*===========================================================================

Function: CAMQDSP_get_state

Description: Synchronous call which provides the operational state of the 
             VFE (Video Front End).

Return Value: The VFE Operation State

Side Effects: None

=========================================================================== */
CAMQDSP_OperationalStateType CAMQDSP_get_state (void);

int CAMQDSP_get_Output1FrameMessageDropCount (void);

int CAMQDSP_get_Output2FrameMessageDropCount (void);

int CAMQDSP_get_CamifErrorCount (void);

void CAMQDSP_put_GammaTable (const CAMQDSP_GammaTableType * gammaTable, uint32 size);

void CAMQDSP_put_RolloffTable(const CAMQDSP_RolloffType *rolloff);


/*===========================================================================

Function: CAMQDSP_SELECTMODULELIST

Description: This function selects the module table based on VFE.

Return Value:
   None

==============================================================================*/
void CAMQDSP_SelectModuleList(CAMQDSP_VfeType vfeType);

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
qdsp_module_type CAMQDSP_GetModule(CAMQDSP_ImageType application);

/*===========================================================================
FUNCTION CAMQDSP_GetCurrentModule

DESCRIPTION
  This function returns the current module of the running application. 

RETURN VALUE
  qdsp module id

SIDE EFFECTS
  None.
===========================================================================*/
qdsp_module_type CAMQDSP_GetCurrentModule(void);

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
CAMQDSP_ReturnCodeType   CAMQDSP_Initialize
(
  CAMQDSP_MsgCBType     QDSPServiceCb, /* qdsp service call back function */
  CAMQDSP_ImageType     application
);

#ifdef FEATURE_MP4_TRANSCODER
/*===========================================================================

Function: CAMQDSP_OFFLINE_ENCODE_INITIALIZE

Description: This function initializes the DSP for transcoder. QDSP codec.
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
CAMQDSP_ReturnCodeType   CAMQDSP_Offline_Encode_Initialize
(
  CAMQDSP_MsgCBType     QDSPServiceCb, /* qdsp service call back function */
  CAMQDSP_ImageType     application
);
#endif

/*===========================================================================

Function: CAMQDSP_REGISTER

Description: This function registers a callback to CAMQDSP_DRV in the case
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
);

/*===========================================================================

Function: CAMQDSP_DEREGISTER

Description: This function deregisters callback to CAMQDSP_DRV in the case
             there is no need to uninstall DSP image.

Return Value:
   None

Comments:

Side Effects: None

==============================================================================*/
void CAMQDSP_Deregister(void);

/*===========================================================================

Function: CAMQDSP_TERMINATE

Description: This function terminates the camera app. QDSP codec.

Return Value:

Comments:

Side Effects: Calls qdsp_disable() to release the QDSP.

==============================================================================*/
void CAMQDSP_Terminate(void);


/*===========================================================================

Function: CAMQDSP_VFE_COMMAND

Description:  This function is used to command the QDSP software.
              It is only valid after the QDSP download is successful.
              This is a synchronous operation.

Return Value:
   CAMQDSP_SUCCESS      - cmd sucessful
   CAMQDSP_INVALID_CMD  - invalid QDSP command specified
   CAMQDSP_INVALID_PARM - invalid QDSP command parameter

Comments: See CAMQDSP_MicroprocessorCommandType for command options

Side Effects:

============================================================================*/
CAMQDSP_ReturnCodeType   CAMQDSP_VFE_Command
(
   CAMQDSP_VFE_CommandType   command,        /* type of command */
   void                     *commandBuffer,  /* NULL if buffer is empty */
   int                       commandBufferSize
);

/*===========================================================================

Function: CAMQDSP_VidEnc_COMMAND

Description:  This function is used to command the QDSP backend software.
              It is only valid after the QDSP download is successful.
              This is a synchronous operation.

Return Value:
   CAMQDSP_SUCCESS      - cmd sucessful
   CAMQDSP_INVALID_CMD  - invalid QDSP command specified
   CAMQDSP_INVALID_PARM - invalid QDSP command parameter

Comments: See CAMQDSP_MicroprocessorCommandType for command options

Side Effects:

============================================================================*/
CAMQDSP_ReturnCodeType   CAMQDSP_VidEnc_Command
(
   CAMQDSP_VidEnc_CommandType  command,        /* type of command */
   void                       *commandBuffer,  /* NULL if buffer is empty */
   int                         commandBufferSize
);

/*===========================================================================

Function: CAMQDSP_ISR

Description: QDSP (aDSP) to Arm Interrupt Service Routine for  camera
             VFE (video front end).

Return Value: NONE

Side Effects: None

==============================================================================*/
void CAMQDSP_isr( void );

/*===========================================================================

Function: CAMQDSP_Offline_ISR

Description: QDSP (aDSP) to Arm Interrupt Service Routine for  camera
             VFE (video front end).

Return Value: NONE

Side Effects: None

==============================================================================*/
void CAMQDSP_Offline_isr( void );

/*===========================================================================

Function: CAMQDSP_FRAME_CLK_CB

Description: When expecting frame and no frame had come for certain amount of
             time (timeout), check it just in case interrupt was missing.

Return Value: NONE

Side Effects: None

==============================================================================*/
void camqdsp_frame_clk_cb(int4 ms);


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
void CAMQDSP_Reset_Ack(void);
#endif //#ifndef CAMERA_USES_SOFTDSP
#endif

