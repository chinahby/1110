/*============================================================================
 FILE: fmrds_api.c

 FM-RDS API Level Code
 ---------------------
 
 DESCRIPTION

 Messages into FM-RDS task are processed here. Operation is based around
 an FM task that must be incorporated into the task list with an appropriate 
 priority.

 Higher-level code must first register with FM services as a client. 
 Subsequently, FM-related commands are passed from the Apps layer into an FM 
 command queue, for subsequent service by the FM task. Depending on function, 
 commands are actioned immediately and a response sent to the caller or, in the 
 case of actions involving some delay, pending command status is returned and 
 the command outcome returned some time later as an event.

 Event notification is asynchronous and is notified to the client by means of a 
 previously-registered callback function.

 This file contains support code for 3rd-party FM devices.


 Copyright (c) 2008 QUALCOMM Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
 ============================================================================*/

/*============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order. Please
 use ISO format for dates.

 $Header: //source/qcom/qct/wconnect/fmradio/fmrds/main/latest/src/fmrds_api.c#12 $$DateTime: 2008/11/26 09:33:21 $$Author: abhis $

 when		who	what, where, why
 ---------- ---   -----------------------------------------------------------
 2008-11-17 rjm   Simple periodic monitor code added.
 2008-10-31 rjm   Cleanup, teFmrdsCmdQStatusType removed.
                  No of AFs returned now range checked.
 2008-10-27 rjm   fmrdsQStatus in command queue now legacy field.
 2008-09-25 rjm   StereoMonoMode removed from paramsAvlble event. Support
                  for basic station seek operation added. Set Station now
                  returns tune status.
 2008-09-18 rjm   code clean-up.
 2008-09-17 as    support for raw RDS data buffering added
 2008-09-09 rjm   stereoProgram field added to station parameter event
 2008-09-05 as    stereoProgram field added to GetTuneStatus()
 2008-08-13 rjm	  Legacy service available command added. af list return added.
 2008-07-25 rjm	  Commands extended for basic RDS support. RDS sig handling
                  moved.
 2008-07-16 rjm	  Added reg event mask, updated crit sect macros. SetVolume()
                  updated. GetTuneStatus() command now supported.
 2008-07-10 rjm	  Command count 10, SendEvent error msg, InitClients() added.
 2008-07-09 as	  Command count temp reduced to 5, check for max no clients
                  on registration, trap event to zero clients.
 2008-06-27 rjm	  GetCurrentSettings() introduced for event returns.
                  All reference to legacy API and functions removed.
 2008-06-20 rjm	  Basic command handling updated for revised fm api.
 2008-06-09 rjm	  Initial revision for 3rd-party fm devices.
 2008-03-20 amar	Initial revision.

 ============================================================================*/


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "custfmrds.h"          /* Include global fm rds switches */
#include "comdef.h"             /* Definitions for byte, word, ... */
#include "target.h"             /* Target specific definitions */

#ifdef FEATURE_FM

#include "task.h"
#include "tmc.h"
//#include "bio.h"
#include "err.h"                /* Error logging macros */
#include "clkregim.h"
#include "dog.h"
//#include "fs.h"
#include "mc.h"
#include "queue.h"
#include "rex.h"
#include "clk.h"                /* Clock services header */
#include "clki.h"               /* Clock services internal header */
//#include "dsati.h"

// fmrds specific files
//#include "fmrds_device.h"     // fmrds device declarations
#include "fmrds_api.h"          // fmrds api efinitions
#include "fmrds_generic.h"      // generic definitions for specific fmrds devices


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// functional defines
#define FM_MONITOR_SUPPORTED     // regular monitor/update functions supported

// max number of fmrds clients
#define FMRDS_MAX_NO_CLIENTS  1   // single client in current implementation

// maximum number of buffers to be placed in the fmrdsfreeq
#define FMRDS_NUM_CMDS        10  // may change according to requirements 

#ifdef FM_MONITOR_SUPPORTED
// periodic monitor poll time (ms)
#define FMRDS_MONITOR_POLL_TIME   2000    // 2 second default
#endif


/*----------------------------------------------------------------------------
 * Storage Declarations
 * -------------------------------------------------------------------------*/

// watchdog report timer
static rex_timer_type  fmrdsReportTimer; 

// legacy timers
//static rex_timer_type  fmrdsWaitTimer;

// command queue
static q_type          fmrdsCmdQ;                         // api command q
static q_type          fmrdsCmdFreeQ;                     // free buffers
static tsFmrdsCmdMsgType fmrdsCmdPool[FMRDS_NUM_CMDS];    // buffer pool

// client registration: room for single client callback pointer at present
static tfFmrdsEvCbFnType *pfnFmrdsClientEvCb = NULL;                    // single client at present
static teFmrdsCbEvMaskType teFmrdsCbEvMask = FM_CB_EV_MASK_SEND_NONE;   // single client event mask
static uint8 ucFmrdsNoOfClients = 0;    // current client count

static uint16 usStoredStation = US_EUROPE_BAND_MIN;   // last tuned station, default 87.5mhz

#ifdef FM_MONITOR_SUPPORTED
static clk_cb_type    CbFmrdsClkStatusMonitor;
#endif 
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

static void FmrdsTaskInit(void);

static void FmrdsInitializeCmdBuffer(tsFmrdsCmdMsgType *fmrds_cmd_ptr);

static void FmrdsApiProcessCommandQueue(void);

static void FmrdsApiProcessCommand(tsFmrdsCmdMsgType *fmrds_cmd_ptr);

static teFmrdsCmdQStatusType FmrdsApiPutCommand(tsFmrdsCmdMsgType *fmrds_cmd_ptr);

static tsFmrdsCmdMsgType *FmrdsGetFreeCmd(void);

static void FmrdsInitClients(void);
static uint8 FmrdsAddClient(tfFmrdsEvCbFnType  *pfnFmrdsEvCb, teFmrdsCbEvMaskType fmCbEvMask);
static void FmrdsRemoveClient(uint8 ucClientId);
static uint8 FmrdsGetClientCount(void);
void FmrdsSendEventToClients(tsFmrdsEvMsgType *ptsEvPtr);

static boolean FmrdsCmdSupported(teFmrdsCmdType fmrdsCmdId);
static void FmrdsProcessRdsBuffFull(void);
static void FmrdsSaveStation(void);
static void FmrdsRestoreStation(void);
static void FmrdsSendTuneStatusEvent(void);

static void FmrdsStatusMonitor(void);
static void FmrdsStartStatusMonitor(void);
static void FmrdsStopStatusMonitor(void);

/*******************************************************************************
***
***     Public Functions
***
*******************************************************************************/

/*******************************************************************************
***                                                                         ****
***     FM Common API Function Calls                                        ****
***                                                                         ****
*******************************************************************************/

/*=============================================================================

FUNCTION		FmrdsApi_RegisterClient

DESCRIPTION     
 This is a direct command used to register for event notification. 
 The client/application layer can pass a pointer to callback function which 
 will be used for asynchronous event notification. A numeric client ID will be 
 returned, where a non-zero value indicates successful registration 
 (Client_id == 0 indicates a registration error). 

 The parameter fmrds_cb_ptr must be a non-NULL value.  If a NULL value is 
 passed to this function, the registration will fail and a zero client id will 
 be returned. Similarly, if the total number of clients supported by the 
 FM-stack is exceeded, registration will fail and a zero client id will be 
 returned.

 Registration also passes an event mask to enable selective notification of
 fm-related events - not currently implemented.

DEPENDENCIES    None.

PARAMETERS		
 o	pfnFmrdsEvCbFn  - callback function pointer
 o  fmCbEvMask      - event mask

 Callback function provided by the client/application layer, which will be used
 by the FM-Stack to deliver event notifications.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
uint8 FmrdsApi_RegisterClient (tfFmrdsEvCbFnType *pfnFmrdsEvCb,
                                                teFmrdsCbEvMaskType fmCbEvMask)
{
  uint8 ucFmrdsRegClientId = 0;   // set default fail status

  if ((pfnFmrdsEvCb != NULL) && (FmrdsGetClientCount( ) < FMRDS_MAX_NO_CLIENTS))
  {
    // function pointer ok
    ucFmrdsRegClientId = FmrdsAddClient(pfnFmrdsEvCb, fmCbEvMask);
  }
  else
  {
    // null client pointer or client count exceeded  
    FMRDS_MSG_ERROR("FMRDS: Client Registration Failed",0,0,0);
  }

  return(ucFmrdsRegClientId);
}


/*=============================================================================

FUNCTION		FmrdsApi_DeRegisterClient

DESCRIPTION     
 This is a direct command used to deregister the callback for event 
 notification. The client/application layer can pass its registered client id 
 which will be used to deregister asynchronous event notification for that 
 client. 

 The parameter fmrds_client_id should be the client id returned from 
 FmrdsApi_RegisterClient.  Passing an invalid client id will result in no 
 clients being de-registered. 

DEPENDENCIES    None.

PARAMETERS		
 o	ucFmrdsClientId

 Client ID returned from a successful call to fmrds_api_register_client. 
 The command is issued directly to the FM stack. This command does not result 
 in a command being placed on the command queue, or any direct communication 
 with the FM-RDS device.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
void FmrdsApi_DeRegisterClient(uint8 ucFmrdsClientId)
{
  // client registration here
  if ((ucFmrdsClientId != 0) && (ucFmrdsClientId <= FmrdsGetClientCount( )))
  {
    // client id ok
    FmrdsRemoveClient(ucFmrdsClientId);
  }
  else
  {
    FMRDS_MSG_ERROR("FMRDS: Invalid dereg client id %d",ucFmrdsClientId,0,0);
  }
  
  return;
}


/*=============================================================================

FUNCTION		FmrdsApi_EnableFm

DESCRIPTION     
 This is an indirect command used to power on the FM-RDS hardware, initialize 
 it with default settings, and place it into a low power mode ready for further 
 commands.

 Although the device will be powered on, the FM-RDS device will be placed in 
 the RADIO_MODE_OFF mode to ensure that the device is consuming the least 
 amount of current possible.  It is the client's responsibility to switch the 
 device to the desired operating mode via the fmrds_api_set_radio_mode function.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsRadioSettings
     o	efm_radio_band
         "	US_EUROPE_BAND
         "	JAPAN_BAND
     o	efm_deemphasis
         "	DE_EMP75
         "	DE_EMP50
     o	efm_chanspacing
         "	CHSP_200K
         "	CHSP_100K
         "	CHSP_50K
     o	efm_rds_mode
         "	RDS_OFF
         "	RDS_MODE
         "	RBDS_MODE

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_ENABLE_FM command in the FM-RDS command queue. The 
 queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 initialize the FM-RDS device with the default/sane settings. On completion, an 
 FMRDS_EV_ENABLE_FM event and associated event message is returned 
 asynchronously to the application layer. This event will return information 
 about the FM-RDS device. In this case, it is returned via a callback mechanism 
 to the FM-RDS BREW OEM layer. This API should only be called after successful 
 registration via fmrds_api_register_client.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_EnableFm (tsFmrdsRadioSettingsType 
                                                          fmrdsRadioSettings)
{
  teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
  tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
  
  
  // get free buffer, pass data into command queue
  if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
  {
    // package up into command message
    pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_ENABLE_FM;
    pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSettings = fmrdsRadioSettings;

    fmrdsQStatus = FmrdsApiPutCommand(pFmrdsCmd);
  }
  
  // return result to client
  return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_DisableFm

DESCRIPTION     
 This is an indirect command used to power-off and disable the FM-RDS hardware.
 While powered off, no communication to the FM-RDS device will be possible 
 until the client re-enables the device again via fmrds_api_enable_fm.

DEPENDENCIES    None.

PARAMETERS		
 o	None. 

 The command is issued indirectly to the FM-RDS device. Initially, command 
 data is packaged into an FMRDS_DISABLE_FM command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 power down the FM-RDS device and put the FM stack into a benign state. On 
 completion, an FMRDS_EV_DISABLE_FM event and associated event message is 
 returned asynchronously to the application layer. In this case, it is returned 
 via a callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_DisableFm (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_DISABLE_FM;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


#ifdef FEATURE_FM_SERVICE_AVAILABLE

/*=============================================================================

FUNCTION		FmrdsApi_FMServiceAvailable

DESCRIPTION     

  Legacy command to support corresponding BREW FM API command. Only used
  in FM receive modes; used to indicate whether there are any FM stations
  with signal levels above the passed value in the selected FM band. The
  entire set band is scanned for activity and the corresponding result returned.

  This command might more meaningfully be replaced with an equivalent
  SearchStations( ) command at a later date.

DEPENDENCIES None.

PARAMETERS		
 o	srchRssiLev eg:
     o	SEARCH_RSSI_LEVEL_WEAK
     o	SEARCH_RSSI_LEVEL_MID
     o	SEARCH_RSSI_LEVEL_STRONG

 The command is issued indirectly to the FM-RDS device. Initially, command data
 is packaged into an FMRDS_SERVICE_AVAILABLE command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 apply the mute settings to the FM-RDS device. On completion, a 
 FMRDS_EV_SERVICE_AVAILABLE event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_FMServiceAvailable (uint8 fmrdsServiceRssi)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SERVICE_AVAILABLE;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsServiceRssiLev = fmrdsServiceRssi;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}

#endif  // FEATURE_FM_SERVICE_AVAILABLE

/*=============================================================================

FUNCTION		FmrdsApi_ConfigureRadio

DESCRIPTION     
 This is an indirect command used to reconfigure various settings on the FM-RDS  
 device and FM-Stack. Included in the passed structure are settings which 
 typically differ from one region to another, and also control for RDS/RBDS.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsRadioSettings
     o	efm_radio_band
        "	US_EUROPE_BAND
        "	JAPAN_BAND
     o	efm_deemphasis
        "	DE_EMP75
        "	DE_EMP50
     o	efm_chanspacing
        "	CHSP_200K
        "	CHSP_100K
        "	CHSP_50K
     o	efm_rds_mode
        "	RDS_OFF
        "	RDS_MODE
        "	RBDS_MODE

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_CFG_RADIO command in the FM-RDS command queue. The 
 queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process.

 When ready, the fmrds_task will pull this message from its command queue and 
 reconfigure the FM-RDS device and stack with the new regional settings. On 
 completion, an FMRDS_EV_CFG_RADIO event and associated event message is 
 returned asynchronously to the application layer. This event will return 
 information about the FM-RDS device. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration via fmrds_api_register_client.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_ConfigureRadio (tsFmrdsRadioSettingsType 
                                                          fmrdsRadioSettings)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_CFG_RADIO;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSettings = fmrdsRadioSettings;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }
 
   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetMuteMode

DESCRIPTION     

 This is an indirect command used to mute or un-mute audio on the FM-RDS 
 device. This function can only be used in RADIO_MODE_RX, and RADIO_MODE_TX_RX 
 modes.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsMuteCtrl
     o	MUTE_NONE
     o	MUTE_LEFT
     o	MUTE_RIGHT
     o	MUTE_BOTH 

 The command is issued indirectly to the FM-RDS device. Initially, command data
 is packaged into an FMRDS_SET_MUTE_MODE command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 apply the mute settings to the FM-RDS device. On completion, a 
 FMRDS_EV_MUTE_MODE_SET event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetMuteMode (teFmrdsMuteLftRgtType fmrdsMuteCtrl)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer

   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_MUTE_MODE;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsMuteCtrl = fmrdsMuteCtrl;
      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetStereoMode

DESCRIPTION     

 This is an indirect command used to set the mono/AUTO_MONO_STEREO_MODE mode of the FM-RDS 
 device. This function can only be used in RADIO_MODE_RX, and RADIO_MODE_TX_RX 
 modes.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsStroMode
     o	AUTO_MONO_STEREO_MODE
     o	FORCE_MONO_ONLY_MODE

 The command is issued indirectly to the FM-RDS device. Initially, command 
 data is packaged into an FMRDS_SET_STEREO_MODE command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is
 to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 apply the mono/AUTO_MONO_STEREO_MODE settings to the FM-RDS device. On completion, a 
 FMRDS_EV_STEREO_MODE_SET event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetStereoMode (teFmrdsStereoMonoModeType fmrdsStroMode)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_STEREO_MODE;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsStereoMonoMode = fmrdsStroMode;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetStation

DESCRIPTION     

 This is an indirect command which tunes the FM-RDS device to a station on the 
 supplied frequency. Only valid frequencies within the band set by 
 FmrdsApi_EnableFm can be tuned by this function.  Attempting to tune to 
 frequencies outside of this band will result in an error.

DEPENDENCIES    None.

PARAMETERS		
 o	usFmrdsFreq
    o The desired frequency to be tuned. This frequency will be in 0.01MHz steps, 
      giving a valid range of 8750 - 10800 for US/Europe, and 7600 - 9100 for Japan 
      bands.  This frequency might be rounded to the closest frequency supported by 
      the FM-RDS device.

 This function can be used in RADIO_MODE_RX, RADIO_MODE_TX, and 
 RADIO_MODE_TX_RX modes.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SET_RADIO_STATION command in the FM-RDS command queue. The 
 queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 instruct the FM-RDS device to tune to the supplied frequency. On completion, 
 a FMRDS_EV_RADIO_STATION_SET event and associated event message is returned 
 asynchronously to the application layer. Additionally, one the specified 
 station is successfully tuned by the FM-RDS device, a 
 FMRDS_EV_RADIO_TUNE_STATUS event will be returned to the client/application 
 layer. In this case, it is returned via a callback mechanism to the FM-RDS 
 BREW OEM layer. This API should only be called after successful registration 
 and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetStation(uint16 usFmrdsFreq)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {  
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_RADIO_STATION;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsFreq = usFmrdsFreq;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);

}


/*=============================================================================

FUNCTION		FmrdsApi_GetTuneStatus

DESCRIPTION     

 This is an indirect command which returns the tune status of the currently 
 tuned frequency.  The client/application layer can use this function to 
 determine whether the FM-RDS device is no longer tuned to the desired 
 frequency.

DEPENDENCIES    None.

PARAMETERS		

 o	None.

 This function can be used in RADIO_MODE_RX, RADIO_MODE_TX, and 
 RADIO_MODE_TX_RX modes.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_TUNE_STATUS command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 fetch the tune status of the currently tuned frequency. On completion, a 
 FMRDS_EV_RADIO_TUNE_STATUS event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetTuneStatus (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	  
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_TUNE_STATUS;
      
      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_GetStationParameters

DESCRIPTION     

 This is an indirect command which returns various statistics related to a 
 tuned station. These statistics include, but are not limited to, the 
 following: currently tuned frequency (zero if not tuned), RDS/RBDS Sync 
 status, Mono/AUTO_MONO_STEREO_MODE indication, RSSI level, and Interference count.  This 
 function is only valid for RADIO_MODE_RX and RADIO_MODE_TX_RX.

DEPENDENCIES    None.

PARAMETERS		
 o	None

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_STATION_PARAMETERS command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is 
 to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 gather various statistics on the currently tuned station. On completion, a 
 FMRDS_EV_PARAMETERS_AVAILABLE event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetStationParameters (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
     // Package up into command message 
     pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_STATION_PARAMETERS;
     
     fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_GetRdsSyncStatus

DESCRIPTION     

 This is an indirect command which returns the RDS/RBDS synchronization status 
 of the currently tuned station. This function is only valid for RADIO_MODE_RX 
 and RADIO_MODE_TX_RX and while RDS/RBDS is not disabled (RDS_MODE/RBDS_MODE).

DEPENDENCIES    None.

PARAMETERS		
 o	None

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_RDS_LOCK_STATUS command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is 
 to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 fetch the RDS/RBDS synchronization status of the currently tuned station. On 
 completion, a FMRDS_EV_RDS_LOCK_STATUS event and associated event message is 
 returned asynchronously to the application layer. In this case, it is returned 
 via a callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetRdsSyncStatus (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_RDS_LOCK_STATUS;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_GetPSInfo

DESCRIPTION     

 This is an indirect command which returns the last known program service 
 information for the currently tuned station. To use this command, the 
 client/application layer must first register for program service info by 
 setting the RDSPSEN bit in the fmrds_api_reg_rds_rx_group_processing function.

 This function is only valid for RADIO_MODE_RX and RADIO_MODE_TX_RX.

DEPENDENCIES    None.

PARAMETERS		
 o	None

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_RDS_PS_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 fetch the last Program Service information received by the FM Stack. On 
 completion, a FMRDS_EV_RDS_PS_INFO event and associated event message is 
 returned asynchronously to the application layer. In this case, it is returned 
 via a callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetPSInfo (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_RDS_PS_INFO;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_GetRTInfo

DESCRIPTION     

 This is an indirect command which returns the last known Radio Text 
 information for the currently tuned station. To use this command, the 
 client/application layer must first register for Radio Text info by setting 
 the RDSRTEN bit in the fmrds_api_reg_rds_rx_group_processing function.

 This function is only valid for RADIO_MODE_RX and RADIO_MODE_TX_RX.

DEPENDENCIES    None.

PARAMETERS		
 o	None

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_RDS_RT_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 fetch the last Radio Text information received by the FM Stack. On completion, 
 a FMRDS_EV_RDS_RT_INFO event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetRTInfo (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {   
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_RDS_RT_INFO;
      
      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_GetAFInfo

DESCRIPTION     

 This is an indirect command which returns the last known Alternate Frequency 
 information for the currently tuned station. To use this command, the 
 client/application layer must first register for Alternate Frequency info by 
 setting the RDSAFEN bit in the fmrds_api_reg_rds_rx_group_processing function.

 This function is only valid for RADIO_MODE_RX and RADIO_MODE_TX_RX.
 
DEPENDENCIES    None.

PARAMETERS	
 o	None

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_RDS_AF_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 fetch the last Alternate Frequency information received by the FM Stack. On 
 completion, a FMRDS_EV_RDS_AF_INFO event and associated event message is 
 returned asynchronously to the application layer. In this case, it is returned 
 via a callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetAFInfo (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue 
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message 
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_RDS_AF_INFO;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process 
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_GetRssiInfo

DESCRIPTION     

 This is an indirect command which returns the RSSI information of the 
 currently tuned station. This function is only valid for RADIO_MODE_RX and 
 RADIO_MODE_TX_RX.

DEPENDENCIES    None.

PARAMETERS		
 o	None

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_RSSI_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process.
  
 When ready, the fmrds_task will pull this message from its command queue and 
 fetch the RSSI information of the currently tuned station. On completion, a 
 FMRDS_EV_RSSI_INFO event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetRssiInfo (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_RSSI_INFO;
  
      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetLowPowerMode

DESCRIPTION     

 This is an indirect command which can put the FM-RDS device and stack into 
 and out of low power mode.  When additional functionality is no longer needed,
 low power mode is designed to reduce FM functionality to its most minimal 
 setting.  By disabling this extra functionality, low power mode can result 
 in a smaller processing footprint which might lead to a power savings.  

 While in low power mode, minimal events will be sent to the client/application 
 layer, and all RDS data and processing will be disabled.  

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsPwrMode
    o FMRDS_NORMAL_MODE - Used to restore stack and FM device to normal operation.
    o FMRDS_LOW_POWER_MODE - used to place stack and FM device into power saving 
      mode.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SET_LOW_PWR_MODE command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 place the stack and device into the appropriate mode. On completion, a 
 FMRDS_EV_LOW_PWR_MODE_SET event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetLowPowerMode (teFmrdsPwrModeType fmrdsPwrMode)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_PWR_MODE;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsPwrMode = fmrdsPwrMode;
   
      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxSetPower

DESCRIPTION     
 This is an indirect command which sets the FM transmit power level. 

DEPENDENCIES    None.

PARAMETERS		
 o	txPwrLev - FM Transmit power level
     o	FMTX_POWER_LEVEL_0
     o	FMTX_POWER_LEVEL_1
     o	FMTX_POWER_LEVEL_2
     o	FMTX_POWER_LEVEL_3
     o	FMTX_POWER_LEVEL_4
     o	FMTX_POWER_LEVEL_5
     o	FMTX_POWER_LEVEL_6
     o	FMTX_POWER_LEVEL_MAX

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SET_TX_PWR_LEVEL command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 set the power level used for FM transmit. On completion, a 
 FMRDS_EV_TX_PWR_LEVEL_SET event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxSetPower (teFmrdsTxPwrLevType txPwrLev)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType  *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_TX_PWR_LEVEL;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxPwrLev = txPwrLev;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetVolume

DESCRIPTION     
 This is an indirect command which sets the Tx/Rx FM volume level. 

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsVolLevOpts - specifies required volume level and whether rx or tx
    volume level should be set.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SET_VOLUME_LEVEL command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process.
 
 On completion, either an FMRDS_EV_RX_VOLUME_LEVEL_SET or
 FMRDS_EV_TX_VOLUME_LEVEL_SET event will be generated.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetVolume (tsFmrdsVolLevOptsType fmrdsVolLevOpts)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_VOLUME_LEVEL;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsVolLevOpts = fmrdsVolLevOpts;  

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxGetSettings

DESCRIPTION     
 This is an indirect command which returns the FM transmit related settings. 

DEPENDENCIES    None.

PARAMETERS		
 o	None.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_GET_TX_SETTINGS command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 returns the settings used while in FM transmit mode. On completion, a 
 FMRDS_EV_RDS_TX_SETTINGS event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxGetSettings (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_GET_TX_SETTINGS;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetRadioMode

DESCRIPTION     

 This is an indirect command used to set the state of the FM-RDS device. It 
 sets up the operating the mode of the radio to Rx or Tx. 

 The parameter tx_rx_mode indicates the desired operating mode.  In TX mode, 
 user is only allowed scan for weakest stations, tune to a particular station, 
 and Tx API's (see section TBD); rest of the API's return no-op

DEPENDENCIES    None.

PARAMETERS	
	
 "	fmrdsTxRxMode
    o	RADIO_MODE_RX		' FM Receiver: FM+RDS Radio
    o	RADIO_MODE_TX 		' FM Transmitter: MP3 Player to Car Stereo
    o	RADIO_MODE_TX_RX	' Not used

 NOTE: Since scanning for weakest stations may require the use of the receiver, 
 the FM stack may need to put the FM-RDS device into Rx mode before issuing 
 this command. Upon completion of the search, the FM-RDS device will be 
 transitioned back to TX mode.  Since this is quiet inefficient, it is 
 advisable to only call Scan For Weakest while in RADIO_MODE_RX or 
 RADIO_MODE_TX_RX.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SET_RADIO_MODE command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetRadioMode (teFmrdsRadioModeType fmrdsTxRxMode)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_RADIO_MODE;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioMode = fmrdsTxRxMode;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SearchStations

DESCRIPTION     

 This is an indirect command used to seek or scan for stations which have a 
 relatively strong signal.

DEPENDENCIES    None.

PARAMETERS		
 "	search_mode - One of the following search modes:
    o	SEARCHMODE_SEEK ' Searches for next valid station
    o	SEARCHMODE_SCAN ' Searches for next valid station and dwells for a 
        specific period of time before searching to the next station
 "	search_up_down - Search direction (Up/Down)
    o	SEARCHDIR_UP ' increment by next channel spacing 
    o	SEARCHDIR_DOWN ' decrement by next channel spacing
 "	scan_time - Scan dwell time in seconds (only valid for	SEARCHMODE_SCAN)
 "	srchRssiLev - RSSI Level for the search operation (i.e. some predefined 
    values:SEARCH_RSSI_LEVEL_WEAK,SEARCH_RSSI_LEVEL_MID,SEARCH_RSSI_LEVEL_STRONG)

 This command is used to invoke a basic seek/scan of the FM radio band. The 
 most basic operation performed by this function is a SEARCHMODE_SEEK command. 
 The seek process is handled by incrementally setting the frequency in 
 pre-defined channel steps and measuring the resulting signal level. Once a 
 station with a reasonably strong signal level is found, and successfully 
 tuned, the seek operation will be completed and a FMRDS_EV_SEARCH_COMPLETE 
 event will be returned to the client.  If no stations are found to match the 
 search criteria, the frequency will be returned to the originally tuned 
 station.   

 Since a seek always results in a frequency being tuned, each seek operation 
 will also return a single FMRDS_EV_RADIO_TUNE_STATUS event to the 
 client/application layer.

 Much like SEARCHMODE_SEEK, a SEARCHMODE_SCAN command can be likened to many 
 back to back seeks with a dwell period after each successful seek.  Once 
 issued, a scan will either increment or decrement frequencies until a 
 relatively strong station is found.  Once this station is found, and is 
 successfully tuned, a FMRDS_EV_RADIO_TUNE_STATUS event will be returned to 
 the client and the station will remain tuned for the specific period of time 
 indicated by scan_time.  After that time expires, a FMRDS_EV_SCAN_IN_PROGRESS 
 event will be sent to the client and a new search will begin for the next 
 relatively strong station. After scanning the entire band, or after a cancel 
 search has been initiated by the client, a FMRDS_EV_SEARCH_COMPLETE event will 
 be sent to the client.

 Once issuing a search command, several commands from the client may be 
 disallowed until the search is completed or canceled. A current list of 
 disallowed commands can be found in section TBD.  

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SEARCH_STATIONS command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the search operation through the relevant generic driver layer command. 
 On completion of the search, FMRDS_EV_SEARCH_COMPLETE and 
 FMRDS_EV_RADIO_TUNE_STATUS events and their associated event messages will be 
 returned asynchronously to the application layer. For the seek operation it 
 returns the tuned station. In this case, it is returned via a callback 
 mechanism to the FM-RDS BREW OEM layer. This API should only be called after 
 successful registration, connection, and initialization is completed.

 The search can be canceled at any time by using API FmrdsApi_CancelSearch( ). 

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SearchStations (tsFmrdsRadioSrchModeType  
                                               fmrdsRadioSrchMode)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	   
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SEARCH_STATIONS;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSrchMode = fmrdsRadioSrchMode;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_RdsSearchStations

DESCRIPTION     

 This API is used to search stations using RDS:
   - Invokes seek based on program type
   - Invokes scan based on program type with specified dwell period 
   - Invokes seek based on program identification 
   - Invokes seek for alternate frequency

DEPENDENCIES    None.

PARAMETERS		
 "	rds_search_mode - One of the following RDS search modes:
    o	SEARCHMODE_RDS_SEEK_PTY ' Searches for next valid station that is 
        broadcasting the specified program type (PTY)
    o	SEARCHMODE_RDS_SCAN_PTY ' Similar to Seek PTY except dwells on valid 
        station before searching next
    o	SEARCHMODE_RDS_SEEK_PI ' Searches for next valid station that is 
        broadcasting the specified program ID
    o	SEARCHMODE_RDS_SEEK_AF ' Searches next valid station for alternate 
        frequency
 "	search_up_down - Search direction (Up/Down)
 "	scan_time - Scan dwell time in seconds (only valid for	SEARCHMODE_SCAN 
    and	SEARCHMODE_RDS_SCAN_PTY)
 "	search_pty - Program Type to search for (only valid for 
    SEARCHMODE_RDS_SEEK_PTY)
 "	search_pi - Program ID to search for (only valid for SEARCHMODE_RDS_SEEK_PI)
 "	srchRssiLev - RSSI Level for the search operation (i.e. some predefined 
    values:SEARCH_RSSI_LEVEL_WEAK,SEARCH_RSSI_LEVEL_MID,SEARCH_RSSI_LEVEL_STRONG)

 This function allows the client to issue seeks and scans similar to commands 
 found in fmrds_api_search_stations. However, each command has an additional 
 RDS component which must be satisfied before a station is successfully tuned. 
 Please see fmrds_api_search_stations for an understanding of how seeks and 
 scans work.

 Much like SEARCHMODE_SEEK in fmrds_api_search_stations, 
 SEARCHMODE_RDS_SEEK_PTY allows the client to seek to stations which are 
 broadcasting RDS data with a program type that matches search_pty. The 
 behavior and events generated for a SEARCHMODE_RDS_SEEK_PTY exactly match the 
 behavior of SEARCHMODE_SEEK, yet only stations broadcasting the specified RDS 
 Program Type (PTY) will be found.  If no matching stations can be found, the 
 original station will be restored.

 Just as SEARCHMODE_RDS_SEEK_PTY's functionality matches SEARCHMODE_SEEK, so 
 does SEARCHMODE_RDS_SCAN_PTY match SEARCHMODE_SCAN. The only difference 
 between the two is that only stations broadcasting a RDS Program Type (PTY) 
 matching search_pty are found and tuned when using SEARCHMODE_RDS_SCAN_PTY.  
 If no station is found to have the PTY as specified by search_pty, then the 
 original station will be restored.

 SEARCHMODE_RDS_SEEK_PI is used the same way as SEARCHMODE_RDS_SEEK_PTY, but 
 only stations matching the specified PI are found.  

 Lastly, SEARCHMODE_RDS_SEEK_AF's functionality differs slightly compared to 
 the other commands in this function.  This command only seeks to stations 
 which are known ahead of time to be alternate frequencies for the currently 
 tune station.  If no alternate frequencies are known, or if the alternate 
 frequencies have a weaker signal strength than the original frequency, the 
 original frequency will be restored.

 Once issuing a search command, several commands from the client may be 
 disallowed until the search is completed or canceled. A current list of 
 disallowed commands can be found in section TBD.  

 The command is issued indirectly to the FM-RDS device. Initially, command 
 data is packaged into an FMRDS_SEARCH_STATIONS command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This 
 is to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the scan operation through the relevant generic driver layer command. 
 On completion of the search, an FMRDS_EV_SEARCH_COMPLETE event and associated 
 event message is returned asynchronously to the application layer. For the 
 seek operation it returns the tuned station. In this case, it is returned via 
 a callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration, connection, and initialization is 
 completed.

 The search can be canceled at any time by using API FmrdsApi_CancelSearch( ). 

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RdsSearchStations ( tsFmrdsRdsRadioSrchModeType
                                                   fmrdsRdsSrchMode)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message   
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_RDS_SEARCH_STATIONS;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRdsRadioSrchMode = fmrdsRdsSrchMode;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SearchStationList

DESCRIPTION     

 This API is used to search station list; i.e. generates a list of frequencies, 
 RDS capability, etc for each valid station.
   - Invokes search for strong stations.
   - Invokes search for weak stations.
   - Invokes search for strongest stations.
   - Invokes search for weakest stations. 
   - Invokes search based on program type.

DEPENDENCIES    None.

PARAMETERS		

 The parameter fmrds_station_list_options indicates the search station list 
 criteria
 "	station_list_mode - Any of the following search modes
   o STATIONLIST_STRONG – Searches the band for strong stations (confined by
     Search_list_max)
   o STATIONLIST_WEAK – Searches the band for weak stations (confined by 
     Search_list_max)
   o STATIONLIST_STRONGEST – Searches entire band for strongest stations 
     (confined by Search_list_max)
   o STATIONLIST_WEAKEST – Searches entire band for weakest (lowest signal 
     strength) stations (confined by Search_list_max)
   o STATIONLIST_PTY – Searches the band and generates a station list based on 
     matching PTY (confined by Search_list_max)
 "	Search_list_max – The maximum number of stations to be returned (0 for 
    unlimited).
 "	search_up_down - Search direction (Up/Down)
 "	search_pty - Program Type to search for (only valid for STATIONLIST_PTY)
 "	srchRssiLev - RSSI Level for the search operation (i.e. some predefined 
    values:SEARCH_RSSI_LEVEL_WEAK,SEARCH_RSSI_LEVEL_MID,SEARCH_RSSI_LEVEL_STRONG)

 This command is used to invoke a search of the FM radio band. The range of 
 frequencies scanned also depends on the band set (see section TBD); otherwise, 
 it is default to the Europe/U.S settings of 87.5 MHz to 108 MHz. The driver 
 searches for all valid stations in the band and when complete, returns a 
 channel list. The scan process is handled by incrementally setting the station 
 in pre-defined channel steps and measuring the resulting level and signal 
 parameters. On completion or canceling of the search, the originally tuned 
 station will be restored and a FMRDS_EV_RADIO_TUNE_STATUS event will be 
 generated.

 Search for strongest or weakest station will generate number of stations as 
 supported by the specific target. 

 Once issuing a search command, several commands from the client may be 
 disallowed until the search is completed or canceled. A current list of 
 disallowed commands can be found in section TBD.  

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SEARCH_STATION_LIST command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is 
 to prevent unnecessary blocking from the calling process.

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the search operation through the relevant generic driver layer command. 
 On completion of the search, an FMRDS_EV_SEARCH_LIST_COMPLETE event and 
 associated event message is returned asynchronously to the application layer. 
 In this case, it is returned via a callback mechanism to the FM-RDS BREW OEM 
 layer. This API should only be called after successful registration, 
 connection, and initialization is completed.

 The return event will list all the frequencies with matching criteria and if 
 RDS is received on these frequencies.

 NOTE: When the search is started, it starts from the currently tuned frequency 
 and returns to the original frequency on completion.

 The Scan can be canceled at any time by using API fmrds_api_cancel_search (). 
 An event FMRDS_EV_SEARCH_CANCELED is called with the partial list of channels 
 for the search period as per the search criteria (if available). 

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SearchStationList (tsFmrdsRadioStationListOptsType 
                                                  fmrdsStationListOpts)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SEARCH_STATION_LIST;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsStationListOpts = fmrdsStationListOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_CancelSearch

DESCRIPTION     

 Invokes a cancel search or search station list command; i.e. used to cancel 
 scan/seeks of the FM radio band. In case search station list command is 
 canceled, a partial list of successfully tuned radio stations (if available) 
 is returned separately in the data associated with the callback EVENT.  Once 
 the search is canceled, the originally tuned station will be restored and a 
 FMRDS_EV_RADIO_TUNE_STATUS event will be returned to the client.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_CANCEL_SEARCH command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process.

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the cancel search operation through the relevant generic driver layer 
 command. On completion, an FMRDS_EV_SEARCH_CANCELED event and associated event 
 message is returned asynchronously to the application layer. In this case, it 
 is returned via a callback mechanism to the FM-RDS BREW OEM layer. This API 
 should only be called after successful registration, connection, and 
 initialization is completed.

DEPENDENCIES    None.

PARAMETERS		

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_CancelSearch (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer

   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_CANCEL_SEARCH;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_SetRdsRxProcessingOptions

DESCRIPTION     

 This function sets RDS Rx processing options.  Included in these options is 
 the ability for the client/application layer to select what "raw" RDS groups 
 should be passed to the client.
 
 Once a group is enabled, and when a buffer holding those groups reaches the
 threshold defined by en_rds_buffer_size, a FMRDS_EV_RAW_RDS_RBDS_DATA event
 will be sent to the client.  This event will contain all groups currently held
 in this buffer, and there will be no need for the client to request the groups
 later.

 Alternatively, if the flag rdsReturnRawBlocks is set, raw rds block data will
 be collected until the specified buffer is full. A FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA
 event will then be returned instead of FMRDS_EV_RAW_RDS_RBDS_DATA. This enables
 raw block/group data to be decoded externally, to support legacy functionality.
 Only error-free block/group data will be returned.

DEPENDENCIES    None.

PARAMETERS		
 The parameter fmrds_rx_processing_options indicates the available RDS RX 
 processing options accessible to the application.
 "	rds_grp_enable (group type enable 0A-15B)
    o	Each bit in the group type field represents a particular RDS group type. 
        If a bit is set ('1') then that particular RDS group type will be buffered 
        and passed to the client/application layer. By default, no RDS groups will 
        be passed to the client.

 "	en_rds_buffer_size (The number of groups to buffer before sending to the App) 
    o	XXXX - Currently this buffer size can not exceed 100 groups.

 "	en_rds_change_filter (Enable pass of changed RDS data)
    o	0 = bypass RDS change filter.
    o	1 = passes RDS group data only if it differs from previous groups of 
        the same group type.

 "	en_pass_uncorrectable_blocks (Enable pass of uncorrectable blocks)
    o	0 = filters out uncorrectable blocks from the Host.
    o	1 = passes uncorrectable blocks to the Host.

 "	en_rds_blockE_processing (Enable pass of block-E groups) 
    o	0 = filters out Block-E from the Host.
    o	1 = passes Block-E to the Host.	

 "	en_rds_auto_af (Enable automatic search for alternate frequencies)
    o	0 = do not automatically search for alternative frequency.
    o	1 = automatically search for alternative frequencies, if available, 
        when the signal qualities is degraded.

 "	rdsReturnRawBlocks (return raw rds block data - external decoding support)
    o	0 = do not return raw rds block data: decode groups as specified by
          rds_group_enable and settings above.
    o	1 = raw rds block data will be buffered and returned for subsequent
          external group decoding. decoding of groups as specified above will
          not be performed. only block data with no errors will be returned.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_SET_RDS_RX_PROCESSING_OPTIONS command in the FM-RDS 
 command queue. The queue will return a PENDING status immediately if the 
 message is passed successfully; otherwise, the respective error code is 
 returned. This is to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to set RDS Rx Configuration registers through the 
 relevant generic driver layer command. On completion, an 
 FMRDS_EV_RDS_RX_OPTIONS_SET event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration, connection, and initialization is 
 completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetRdsRxProcessingOptions (tsFmrdsRxProcOptType 
                                                          fmrdsRxProcOpt)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_SET_RDS_RX_PROCESSING_OPTIONS;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRxProcOpt = fmrdsRxProcOpt;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_RegRdsRxGroupProcessing

DESCRIPTION  
   
 Provides an option to register RDS Rx group processing.  This function allows 
 the client/application layer to decide what RDS processing should be done by 
 the FM-Stack/FM-RDS device.  Once this processing is complete, only the data 
 specific to the RDS service will be returned to the client.

 Requests the return of FM-RDS Rx group processing; As of now, we plan to 
 support program service name and/or radio text data and/or alternate frequency 
 list processing in the return event message; 

DEPENDENCIES    None.

PARAMETERS		
 "	ulFmrdsGrpsToProc
    o	Each bit in the services to process represents a particular service option; 
        if bit is set ('1') then that particular service processing is enabled.

   Bit	 Field Name	 Description
    0	 RDSRTEN	    RDS Process Radio Text:
                        0 = Do not process RDS Radio Text (RT).
                        1 = Process RDS Radio Text.
    1	 RDSPSEN	    RDS Process Program Service Name:
                        0 = Do not process RDS Program Service (PS) name.
                        1 = Process RDS Program Service name.
    2	 RDSAFEN	    RDS Process Alternative Frequency List:
                        0 = Do not process RDS Alternative Frequency (AF) list.
                        1 = Process RDS Alternative Frequency list.
    3…31 Not Supported	NA

 This function is used by the client/application layer to register and 
 deregister for extended FM-RDS Rx event processing. As of now, the FM-stack 
 supports program service, radio text, and alternate frequency list 
 processing. Only one client is supported. 

 The result of registered RDS processing will be returned to the 
 client/application layer in the form of multiple events.  Included in these 
 events will be the status information for program service name, radio text, 
 and alternate frequency list processing.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_REG_RDS_RX_GRPS_PROCESSINGcommand in the FM-RDS 
 command queue. The queue will return a PENDING status immediately if the 
 message is passed successfully; otherwise, the respective error code is 
 returned. This is to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to set RDS Rx Configuration registers through the 
 relevant generic driver layer command. On completion, an 
 FMRDS_EV_RDS_RX_GRPS_REG_DONE event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration, connection, and initialization is 
 completed.

 When Program Service (PS) information becomes available, a 
 FMRDS_EV_RDS_PS_INFO event and PS information will be sent to the client.  
 When Radio Text (RT) information becomes available, a FMRDS_GET_RDS_RT_INFO 
 event and RT data will be sent to the client. When Alternate Frequency (AF) 
 information becomes available, a FMRDS_EV_RDS_AF_INFO event and AF data 
 will be sent to the client.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RegRdsRxGroupProcessing (uint32 ulFmrdsGrpsToProc)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_REG_RDS_RX_GRPS_PROCESSING;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.ulFmrdsGrpsToProc = ulFmrdsGrpsToProc;
   
      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_RegRdsRxPiMatchProcessing

DESCRIPTION     

 Registers or deregisters for PI match events. When a PI match occurs the 
 corresponding event is generated.

DEPENDENCIES    None.

PARAMETERS	
 o fmrdsRdsPiMtchProcOpts	
    " tfmrds_reg_pi_match 
       o FMRDS_EV_REGISTER - Registers for PI match events.
       o FMRDS_EV_DEREGISTER - Deregisters for PI match events.
    " prog_id specifies the program id to match
       o Program Identification (PI) code for fast PI check; Ignored when 
         deregistering.

 This function is used by the client to register for FM-RDS radio event for PI 
 match processing with the FM-RDS device layers. Call this routine with 
 tfmrds_reg_pi_match set to FMRDS_EV_REGISTER to register, and with 
 FMRDS_EV_DEREGISTER to de-register. 

 The command is issued indirectly to the FM-RDS device. Initially, command 
 data is packaged into an FMRDS_REG_PI_MATCH_PROCESSING command in the FM-RDS 
 command queue. The queue will return a PENDING status immediately if the 
 message is passed successfully; otherwise, the respective error code is 
 returned. This is to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to set RDS Rx Configuration registers through the 
 relevant generic driver layer command. On completion, an 
 FMRDS_EV_PI_MATCH_REG_DONE event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration, connection, and initialization is 
 completed.

 An FMRDS_EV_RDS_RX_PI_MATCH_AVAILABLE event and associated event message is 
 generated asynchronously to the application layer whenever PI match processing 
 is available.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RegRdsRxPiMatchProcessing (tsFmrdsRdsRxPIMtchProcOptsType 
                                                          fmrdsRdsPIMtchProcOpts)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	  
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_REG_PI_MATCH_PROCESSING;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRdsPIMtchProcOpts = fmrdsRdsPIMtchProcOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_RegRdsRxBlockBMatchProcessing

DESCRIPTION     

 This function registers or deregisters Block B match events. Whenever an RDS 
 Block B match occurs, a corresponding event with RDS data will be returned to 
 the client/application layer.

DEPENDENCIES    None.

PARAMETERS		
 o fmrdsBlkBMtchProcOpts
    " fmrds_reg_rds_blockb_match 
       o FMRDS_EV_REGISTER - Registers for Block B match events.
       o FMRDS_EV_DEREGISTER - Deregisters for Block B match events.
    " match_condition - block B matching condition, 0 if not used
       o Block-B matching condition; Used in conjunction with Block-B mask bits
    " match_mask - block B matching mask, 0 if not used
       o Set bits for parts of Block-B for which match is not required

 This function is used by the client to register for FM-RDS radio event for 
 blockB match processing with the FM-RDS device layers. Call this routine 
 with fmrds_reg_rds_blockb_match set to FMRDS_EV_REGISTER to register, and with 
 FMRDS_EV_DEREGISTER to de-register. Only one client is supported.

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_REG_BLOCKB_PROCESSING command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is 
 to prevent unnecessary blocking from the calling process. 
 
 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to set RDS Rx Configuration registers through the 
 relevant generic driver layer command. On completion, an 
 FMRDS_EV_BLOCKB_REG_DONE event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration, connection, and initialization is 
 completed.

 An FMRDS_EV_RDS_RX_BLOCKB_AVAILABLE event and associated event message is 
 generated asynchronously to the application layer whenever blockB match 
 processing is available.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RegRdsRxBlockBMatchProcessing (tsFmrdsRdsRxBlkBMtchProcOptsType 
                                                              fmrdsBlkBMtchProcOpts)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	  
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_REG_BLOCKB_PROCESSING;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRdsBlkBMtchProcOpts = fmrdsBlkBMtchProcOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxSetStationPi

DESCRIPTION     

 This function will store the desired Program Identification for use in future 
 RDS transmissions.

DEPENDENCIES    None.

PARAMETERS		
 "	fmrdsTxPi 
    o	Program Identification (PI) code to be used in later RDS transmissions.

 This function is used by the client to store a unique Program Identification 
 code (PI) to be used in preceding RDS transmissions.  The process used to 
 transmit Program Service strings, Radio Text, and Raw RDS groups will use the 
 PI set by this function to generate RDS groups.  If the client does not supply 
 a PI through this function or another, a default PI will be generated and used 
 until it is changed by the client.  

 The command is issued directly to the FM stack. This command does not result 
 in a command being placed on the command queue, or any direct communication 
 with the FM-RDS device. 

 The Program Identification (PI) set by this function does not immediately 
 affect current one-shot and continuous RDS transmissions. If the client wishes 
 to update the PI for current RDS transmissions, they must be stopped and 
 restarted after the new PI is set.  

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
boolean FmrdsApi_TxSetStationPi (teFmrdsPrgmIdType fmrdsTxPi)
{
   return TRUE;
}


/*=============================================================================

FUNCTION		FmrdsApi_TxGetPSFeatures

DESCRIPTION     

 This function returns a structure detailing the features supported for Program 
 Service RDS transmit.  

DEPENDENCIES    None.

PARAMETERS		

 o	None.

 This function is used in conjunction with fmrds_api_tx_ps_station_info to 
 determine what capabilities are supported for Program Service transmission.  
 By using this function, the client will know the maximum number of characters 
 which can be passed into fmrds_api_tx_ps_station_info without the string being 
 truncated, the number of times each program service string can be repeated, 
 and the number of alternate frequencies which can be transmitted. 

 The command is issued directly to the FM stack. This command does not result 
 in a command being placed on the command queue, or any direct communication 
 with the FM-RDS device.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
uint8 FmrdsApi_TxGetPSFeatures (void)
{
   uint8 maxNoOfTxChars = 0;

   return (maxNoOfTxChars);  // temp return value
}


/*=============================================================================

FUNCTION		FmrdsApi_TxPSStationInfo

DESCRIPTION     

 This function will continuously transmit RDS Program Service information over 
 a tuned frequency.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsTxPSStationInfoOpts
    o A single null-terminated character string to be parsed and transmitted as 
      one or more 8-character strings.
    o fmrds_tx_ps_rpt_cnt
    o The number of times each 8-character string will be repeated before 
      transmitting the next string.
    o fmrds_tx_ps_pty
    o The Program Type code (PTY) which will be transmitted along with the 
      Program Service information.
    o fmrds_tx_af_list
    o A pointer to a structure holding the number of alternate frequencies to 
      transmit and that list of alternate frequencies.  If no alternate 
      frequencies should be transmitted, pass a NULL for this parameter.

 This function is used by the client to transmit RDS Program Service 
 information on an already tuned frequency.  The Program Service information 
 is made up of one or more 8-character strings, a Program Type (PTY), an 
 Alternate Frequency (AF) and also a Program Identification (PI).   This 
 information is eventually used to generate 0A and/or 0B RDS groups to be 
 transmitted on a tuned station.

 The one or more 8-character strings are created from the passed 
 fmrds_tx_ps_string string, which is parsed into a new string after every 
 8 characters.  If the final string consists of less than 8 characters, the 
 remaining characters will be filled with "blank" space characters.  Each 
 8-character string will be repeated zero or more times before the next 
 Program Service string is transmitted.  Once all Program Service strings have 
 been transmitted, the process will start over from the first string again.  

 The Program Service information will be continuously transmitted until the 
 client stops or starts another Program Service transmission, or if the FM-RDS 
 device is no longer in TX mode and tuned to a frequency.  Each call to 
 transmit Program Service information will replace all previously set Program 
 Service information.

 Since the PI is not set through this function, it is recommended that 
 fmrds_api_tx_set_station_pi is called before executing this function.  If the 
 PI is not set, a default PI will be used instead.  

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_PS_INFO command in the FM-RDS command queue. The 
 queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to send the Program Service information to the FM-RDS 
 device and start Program Service transmission through the relevant generic 
 driver layer command. On completion, an FMRDS_EV_TX_PS_INFO_COMPLETE event and 
 associated event message is returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxPSStationInfo (tsFmrdsTxPSStationInfoType
                                                      fmrdsTxPSStationInfoOpts)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_TX_PS_INFO;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxPSStationInfoOpts = fmrdsTxPSStationInfoOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_RxStopPSStationInfo

DESCRIPTION     

 This function will stop RDS Program Service info transmission.

DEPENDENCIES    None.

PARAMETERS		
 o	None.

 This function is used by the client to stop RDS Program Service information 
 transmission. 

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_STOP_PS_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to stop Program Service information transmission on the 
 FM-RDS device through the relevant generic driver layer command. On 
 completion, a FMRDS_EV_TX_STOP_PS_INFO_COMPLETE event and associated event 
 message is returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RxStopPSStationInfo (void)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_STOP_TX_PS_INFO;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxRTStationInfo

DESCRIPTION     

 This function will continuously transmit RDS Radio Text information over a 
 tuned frequency.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsTxRTStationInfoOpts
    o A single 65 character or less, null-terminated character string to be 
      transmitted as Radio Text.
    o fmrds_tx_rt_pty
    o The Program Type code (PTY) which will be transmitted along with the Radio 
      Text information

 This function is used by the client to transmit RDS Radio Text information on 
 an already tuned frequency.  The Radio Text information is made up of one 64 
 character or less string, a Program Type (PTY) and also a Program 
 Identification (PI).   This information is eventually used to generate 2A 
 and/or 2B RDS groups to be transmitted on a tuned station.

 The Radio Text information will be continuously transmitted until the client 
 stops or starts another Radio Text transmission, or if the FM-RDS device is no 
 longer in TX mode and tuned to a frequency.  Each call to transmit Radio Text 
 information will replace all previously set Radio Text information.

 Since the PI is not set through this function, it is recommended that 
 fmrds_api_tx_set_station_pi is called before executing this function. If the 
 PI is not set, a default PI will be used instead  

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_RT_INFO command in the FM-RDS command queue. The 
 queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to send the Radio Text information to the FM-RDS device 
 and start Radio Text transmission through the relevant generic driver layer 
 command. On completion, an FMRDS_EV_TX_RT_INFO_COMPLETE event and associated 
 event message is returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxRTStationInfo (tsFmrdsTxRTStationInfoType 
                                                      fmrdsTxRTStationInfoOpts)

{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_TX_RT_INFO;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxRTStationInfoOpts = fmrdsTxRTStationInfoOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxStopRTStationInfo

DESCRIPTION     

 This function stops RDS Radio Text info transmission.

DEPENDENCIES    None.

PARAMETERS
 o	None.

 This function is used by the client to stop RDS Radio Text information 
 transmission. 

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_STOP_RT_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to stop Radio Text information transmission on the FM-RDS 
 device through the relevant generic driver layer command. On completion, a 
 FMRDS_EV_TX_STOP_RT_INFO_COMPLETE event and associated event message is 
 returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxStopRTStationInfo (void)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_STOP_TX_RT_INFO;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxRdsGroups

DESCRIPTION     

 This function will transmit one or more RDS groups once over a tuned 
 frequency.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsTxOneShotRdsGrpsOpts
    o A pointer to a buffer of one or more RDS groups to transmit.
    o fmrds_tx_num_rds_groups_to_tx.
    o The number of RDS groups to be copied and transmitted from 
      fmrds_tx_rds_groups.
    o fmrds_tx_num_rds_groups_processed.
    o The number of RDS groups actually copied from fmrds_tx_rds_groups.

 This function is used by the client to transmit one or more RDS groups, only 
 once, on an already tuned frequency.  Since block A of each RDS group consists 
 only of a Program Identification code (PI) and a checkword, this block is 
 removed from the tfmrds_tx_rds_group_type type.   Each RDS group passed into 
 this function should consist only of blocks BCD, and block A will be 
 automatically generated from the PI set from fmrds_api_tx_set_station_pi. 
 If no PI has been set by the client, a default PI will be used to generate 
 block A for each group.  

 Each call to this function will store the passed RDS groups into one or more 
 local buffers before they are passed to the FM-RDS device. Each local buffer 
 can store one or more RDS groups before they are passed to the FM-RDS device. 
 Once the client begins RDS group transmission, the FM-stack will issue an 
 event to the client whenever there is at least one RDS group buffer empty. 
 If the client has more groups to transmit than there is room in the buffers, 
 the client can use fmrds_api_tx_get_rds_groups_buf to determine when there is 
 sufficient room for additional groups.  

 Once an RDS group buffer becomes empty, the stack will alert the client that 
 more groups can be added via a FMRDS_EV_TX_RDS_GROUPS_COMPLETE event.  Because 
 the stack will only issue this event when a buffer becomes empty, multiple 
 calls to this function might result in only one 
 FMRDS_EV_TX_RDS_GROUPS_COMPLETE event being generated.

 Since switching between transmitting "one-shot" and continuous RDS groups can 
 lead to unexpected behavior, transitioning between "one-shot" and continuous 
 RDS transmission, and vice versa, will lead to all un-transmitted groups 
 being cleared. 

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_RDS_GROUPS command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to send RDS groups to the FM-RDS device and start 
 one-shot RDS group transmission through the relevant generic driver layer 
 command. On completion, an FMRDS_EV_TX_RDS_GROUPS_COMPLETE event and 
 associated event message is returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxRdsGroups (tsFmrdsTxOneShotRdsGrpsType 
                                                      fmrdsTxOneShotRdsGrpsOpts)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_TX_RDS_GROUPS;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxOneShotRdsGrpsOpts = fmrdsTxOneShotRdsGrpsOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxContRdsGroups

DESCRIPTION     

 This function transmits one or more RDS groups continuously over a tuned 
 frequency.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsTxContRdsGrpsOpts
    o A pointer to a buffer of one or more RDS groups to transmit.
    o fmrds_tx_num_rds_groups_to_tx.
    o The number of RDS groups to be copied and transmitted from 
      fmrds_tx_rds_groups.
    o fmrds_tx_num_rds_groups_processed.
    o The number of RDS groups actually copied from fmrds_tx_rds_groups.

 This function is used by the client to transmit one or more RDS groups, 
 continuously, on an already tuned frequency.  Since block A of each RDS group 
 consists only of a Program Identification code (PI) and a checkword, this 
 block is removed from the tfmrds_tx_rds_group_type type.   Each RDS group 
 passed into this function should consist only of blocks BCD, and block A will 
 be automatically generated from the PI set from fmrds_api_tx_set_station_pi. 
 If no PI has been set by the client, a default PI will be used to generate 
 block A for each group.  

 Unlike "One-Shot" RDS group transmission, continuous group transmission will 
 repeatedly transmit all RDS groups stored in an RDS group buffer until stopped 
 by the client.  Additionally, continuous RDS group transmission must consist 
 of only a single static RDS group buffer.  This buffer can consist of one or 
 more RDS groups, but the RDS group buffer is cleared each time this function 
 is called. Each call of this function will store the passed RDS groups into a 
 local buffer before it is passed to the FM-RDS device. 

 Since continuous RDS group transmission works on a static RDS buffer, the 
 number of groups in the buffer will not change as groups are transmitted. 

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_CONT_RDS_GROUPS command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is 
 to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to send RDS groups to the FM-RDS device and start 
 continuous RDS group transmission through the relevant generic driver layer 
 command. On completion, an FMRDS_EV_TX_CONT_RDS_GROUPS_COMPLETE event and 
 associated event message is returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxContRdsGroups (tsFmrdsTxContRdsGrpsType 
                                                        fmrdsTxContRdsGrpsOpts)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_TX_CONT_RDS_GROUPS;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxContRdsGrpsOpts = fmrdsTxContRdsGrpsOpts;   

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxRdsGroupCtrl

DESCRIPTION     

 This function is used to pause or resume RDS group transmission, or stop and 
 clear all RDS groups.

DEPENDENCIES    None.

PARAMETERS		

 o	fmrdsTxRdsGrpCtrlOpt - used to pause, stop and clear, or resume RDS 
    group transmission.
     o FMRDS_TX_RDS_GROUP_PAUSE - pauses RDS group transmission.
     o FMRDS_TX_RDS_GROUP_CLEAR - stops RDS group transmission and clears all 
       stored groups.
     o FMRDS_TX_RDS_GROUP_RESUME - resumes a paused RDS group transmission.

 This function can be used to pause RDS group transmission, as well as resume 
 RDS group transmission. Additionally, this function can also be used to stop 
 RDS group transmission and clear previously stored RDS groups.
 
 If the client calls this function with fmrds_tx_rds_group_ctrl set to 
 FMRDS_TX_RDS_GROUP_PAUSE, this function will stop RDS group transmission 
 started by fmrds_api_tx_rds_groups and  fmrds_api_tx_cont_rds_groups. 
 Although RDS group transmission will be stopped, all stored RDS group data 
 will remain intact.  Once paused, RDS group transmission can be resumed by 
 calling fmrds_api_tx_rds_group_ctrl again with fmrds_tx_rds_group_ctrl set to 
 FMRDS_TX_RDS_GROUP_RESUME.  Resuming RDS transmission will restart RDS 
 transmission after the last transmitted group.

 If this function is called with fmrds_tx_rds_group_ctrl set to 
 FMRDS_TX_RDS_GROUP_CLEAR, all previously stored RDS groups will be cleared 
 and RDS group transmission will be stopped.  

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_RDS_GROUP_CTRL command in the FM-RDS command 
 queue. The queue will return a PENDING status immediately if the message is 
 passed successfully; otherwise, the respective error code is returned. This is 
 to prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to pause, resume or clear RDS group transmission through 
 the relevant generic driver layer command. On completion, an 
 FMRDS_EV_TX_RDS_GROUP_CTRL_COMPLETE event and associated event message is 
 returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxRdsGroupCtrl (teFmrdsTxRdsGrpCtrlType 
                                                          fmrdsTxRdsGrpCtrlOpt)	

{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_TX_RDS_GROUP_CTRL;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxRdsGrpCtrlOpt = fmrdsTxRdsGrpCtrlOpt;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxGetRdsGroupsBufSize

DESCRIPTION     

 This function will return the number of RDS groups currently available for 
 transmit.

DEPENDENCIES    None.

PARAMETERS		

 o	None.

 This function is used by the client to determine how many RDS groups can be 
 transmitted via fmrds_api_tx_rds_groups or fmrds_api_tx_cont_rds_groups.  By 
 using this function, the client will know the currently available RDS groups 
 which can be passed into fmrds_api_tx_rds_groups and 
 fmrds_api_tx_cont_rds_groups. 

 The command is issued directly to the FM stack. This command does not result 
 in a command being placed on the command queue, or any direct communication 
 with the FM-RDS device.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
uint8 FmrdsApi_TxGetRdsGroupsBufSize (void)	
{
   return (0);  // temp return value
}


/*=============================================================================

FUNCTION		FmrdsApi_RegRxRTPlusClassProc

DESCRIPTION     

 Provides an option to register RDS Rx RTPlus class processing; this function 
 allows the client/application layer to decide what Rx RTPlus class processing 
 should be done by the FM-Stack/FM-RDS device.  Once this processing is 
 complete, only the data specific to these classes will be returned to the 
 client.

DEPENDENCIES    None.

PARAMETERS		

 lsbClassMsk
   Each bit in the services to process represents a particular service option; 
   if bit is set (‘1’) then that particular service processing is enabled.

 msbClassMsk
   Each bit in the services to process represents a particular service option; 
   if bit is set (‘1’) then that particular service processing is enabled.

   RTPlus class bit mask table is as follows:

   Code	    RTplus classes
   0		ITEM.TITLE
   1		ITEM.ALBUM
   2		ITEM.TRACKNUMBER
   3		ITEM.ARTIST
   4		ITEM.COMPOSITION
   5		ITEM.MOVEMENT
   6		ITEM.CONDUCTOR
   7		ITEM.COMPOSER
   8		ITEM.BAND
   9		ITEM.COMMENT
   10		ITEM.GENRE
   11		INFO.NEWS
   12		INFO.NEWS.LOCAL
   13		INFO.STOCKMARKET
   14		INFO.SPORT
   15		INFO.LOTTERY
   16		INFO.HOROSCOPE
   17		INFO.DAILY_DIVERSION
   18		INFO.HEALTH
   19		INFO.EVENT
   20		INFO.SZENE
   21		INFO.CINEMA
   22		INFO.TV
   23		INFO.DATE_TIME
   24		INFO.WEATHER
   25		INFO.ALARM
   26		INFO.ADVERTISEMENT
   27		INFO.OTHER
   28		STATIONNAME.LONG
   29		PROGRAM.NOW
   30		PROGRAM.NEXT
   31		PROGRAM.PART
   32		PROGRAM.HOST
   33		PROGRAM.EDITORIAL_STAFF
   34		PROGRAM.RADIO
   35		PROGRAM.HOMEPAGE
   36		PHONE.HOTLINE
   37		PHONE.STUDIO
   38		PHONE.OTHER
   39		SMS.STUDIO
   40		SMS.OTHER
   41		EMAIL.HOTLINE
   42		EMAIL.STUDIO
   43		EMAIL.OTHER
   44		MMS.OTHER
   45		CHAT
   46		CHAT.CENTER
   47		VOTE.QUESTION
   48		VOTE.CENTER
   49
   50
   51
   52
   53
   54
   55
   56
   57
   58		PLACE
   59		APPOINTMENT
   60		HOTLINE
   61		IDENTIFIER
   62		PURCHASE
   63		GET_DATA

   This function is used by the client/application layer to register and 
   deregister for RTPlus class processing. The result of registered RDS 
   processing will be returned to the client/application layer in the form of 
   multiple events.  

   The command is issued indirectly to the FM-RDS device. Initially, command 
   data is packaged into an FMRDS_REG_RX_RTPLUS_CLS_PROCESSING command in the 
   FM-RDS command queue. The queue will return a PENDING status immediately if 
   the message is passed successfully; otherwise, the respective error code is 
   returned. This is to prevent unnecessary blocking from the calling process. 

   When ready, the fmrds_task will pull this message from its command queue and 
   invoke the operation to set 2A/B, 3A and all ODA “A” group processing 
   through the relevant generic driver layer command. On completion, an 
   FMRDS_EV_RX_RTPLUS_CLS_REG_DONE event and associated event message is 
   returned asynchronously to the application layer. In this case, it is 
   returned via a callback mechanism to the FM-RDS BREW OEM layer. This API 
   should only be called after successful registration, connection, and 
   initialization is completed.

   When RTPlus information becomes available, a FMRDS_EV_RTPLUSE_INFO event and 
   RTPlus class information will be sent to the client.  
 
RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RegRxRTPlusClassProc (tsFmrdsRTPlusClassProcOptType      
                                                        fmrdsRTPlusClassProcOpts)
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_REG_RTPLUS_CLS_PROCESSING;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsRTPlusClassProcOpts = fmrdsRTPlusClassProcOpts;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}


/*=============================================================================

FUNCTION		FmrdsApi_TxGetRdsGroupsBufSize

DESCRIPTION     

 This function will continuously transmit RDS RTPlus information over a tuned 
 frequency.

DEPENDENCIES    None.

PARAMETERS		
 fmrds_tx_rt_string
   A single 65 character or less, null-terminated character string to be 
   transmitted as Radio Text.

 fmrds_tx_rt_pty
   The Program Type code (PTY) which will be transmitted along with the Radio 
   Text information

 tsFmrdsRTPlusTagsType fmrdsRTplusTags[2]
   teFmrdsRTPlusClasstType rtPlusContType;
      • RT Content Type
   uint8 startMarker;
      • Start Marker pointing to the position (inside the RT message) of the 
      first character of that RTplus information element
   uint8 lenMarker;
      • Length Marker indicating the length of that RTplus information element

 teFmrdsAppGrpTypeCode appGrpTypeCode;
 boolean newItem;
 boolean clearCmd;

 This function is used by the client to transmit RTPlus radio text information 
 on an already tuned frequency.  The RTplus information is made up of one 64 
 character or less string, PTY, PI and RTPlus tags.   This information is 
 eventually used to generate 2A and/or 2B, 3A and ODA type A RDS groups to be 
 transmitted on a tuned station.

 The RTPlus information will be continuously transmitted until the client stops 
 or starts another RTPlus/RT transmission, or if the FM-RDS device is no longer 
 in TX mode and tuned to a frequency.  Each call to transmit RTPlus information 
 will replace all previously set Radio Text/ RTPlus information.

 Since the PI is not set through this function, it is recommended that 
 fmrds_api_tx_set_station_pi is called before executing this function. If the 
 PI is not set, a default PI will be used instead  

 The command is issued indirectly to the FM-RDS device. Initially, command data 
 is packaged into an FMRDS_TX_RTPLUS_INFO command in the FM-RDS command queue. 
 The queue will return a PENDING status immediately if the message is passed 
 successfully; otherwise, the respective error code is returned. This is to 
 prevent unnecessary blocking from the calling process. 

 When ready, the fmrds_task will pull this message from its command queue and 
 invoke the operation to send the RTPlus information to the FM-RDS device and 
 start RTPlus transmission through the relevant generic driver layer command. 
 On completion, an FMRDS_EV_TX_ RTPlUS_INFO_COMPLETE event and associated event 
 message is returned asynchronously to the application layer.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_TxRTPlusStationInfo (tsFmrdsTxRTPlusStationInfoType
                                                        fmrdsTxRTPlusStationInfo)	
{
   teFmrdsCmdQStatusType fmrdsQStatus = FMRDS_CMD_Q_UNRECOGNIZED_CMD;
   tsFmrdsCmdMsgType *pFmrdsCmd = NULL;    // command buffer pointer
 
   // Pass message into FM-RDS Command queue
   if ( (pFmrdsCmd = FmrdsGetFreeCmd()) != NULL )
   {	           
      // Package up into command message
      pFmrdsCmd->fmrdsCmdHdr.fmrdsCmdType = FMRDS_TX_RTPLUS_INFO;
      pFmrdsCmd->fmrdsCmdMsgBody.tuCmdParams.fmrdsTxRTPlusStationInfo = fmrdsTxRTPlusStationInfo;

      fmrdsQStatus   = FmrdsApiPutCommand(pFmrdsCmd);
   }

   // Return queue status immediately to calling process
   return(fmrdsQStatus);
}



/*******************************************************************************
***                                                                          ***
***                                                                          ***
***     FM RDS Task, Queues, Dispatching and Notification                    ***
***                                                                          ***
***                                                                          ***
*******************************************************************************/

/*******************************************************************************
***
***     Public Functions
***
*******************************************************************************/

/*******************************************************************************

FUNCTION      FmrdsTaskInit( )

DESCRIPTION   Initialization routine the queues and timers for FM Radio + RDS
              Task. Should be called only once, at powerup time. 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
LOCAL void FmrdsTaskInit( void )
{
  uint8 i=0;
  
  // create timers
  rex_def_timer( &fmrdsReportTimer, &fmrds_tcb, FMRDS_RPT_TIMER_SIG );

#if 0
  // fmrds wait timer declaration
  rex_def_timer( &fmrdsWaitTimer, &fmrds_tcb, FMRDS_WAIT_SIG ); // legacy
#endif
  
  // initialize queues
  (void) q_init( &fmrdsCmdQ );       /* initialise fm command queue */
  (void) q_init( &fmrdsCmdFreeQ );   /* free queue */
  
  // fill fmrds_cmd_free_q with command buffers
  for ( i = 0; i < FMRDS_NUM_CMDS; i++ )    
  {
    FmrdsInitializeCmdBuffer( &fmrdsCmdPool[i] );
    q_put( &fmrdsCmdFreeQ, q_link( &fmrdsCmdPool[i],
      &fmrdsCmdPool[ i ].fmrdsCmdHdr.fmrdsLink ) );
  }
  
}   // end of FmrdsTaskInit( )


/*******************************************************************************

FUNCTION      fmrds_task

DESCRIPTION	  Task provides main processing loop for all FM commands and
              subsequent actions.

DEPENDENCIES  FEATURE_FM

RETURN VALUE  Does not return.

SIDE EFFECTS  None

*******************************************************************************/
void fmrds_task( dword dummy )
{
  rex_sigs_type rex_signals_mask;       // mask of signals returned by rex
  
  // *** first, initialise and ensure task is started ***
  FmrdsTaskInit();   // initialise queues and timers

  // note: may need to force power down of fm receiver here in case it's still
  // running from a previous reset
  
  // wait for task-start signal from main control task
  tmc_task_start();
  rex_clr_sigs( rex_self(), FMRDS_TASK_START_SIG );   // clear start signal
  
  // start timer that kicks the dog periodically
  (void) rex_set_timer( &fmrdsReportTimer, FMRDS_DOG_RPT_TIME );

  // other functional initialisation
  FmrdsInitClients( );   // initialise fm client handling


  // *** main fm control task loop - no return from this ***
  for ( ; ; )
  {
    
    // suspended waiting for signals we might be interested in...
    rex_signals_mask = rex_wait( FMRDS_RPT_TIMER_SIG       // watchdog report timer
                                 | FMRDS_UPDATE_RDS_SIG    // rds update from isr
                                 | FMRDS_CMD_Q_SIG         // queued fm rds command
                                 | FMRDS_TASK_STOP_SIG     // task stop signal
                                 | FMRDS_TASK_OFFLINE_SIG  // task offline signal
#ifdef FM_MONITOR_SUPPORTED
                                 | FMRDS_MONITOR_SIG       // periodic monitoring timeout
#endif
                                 );               
          
    // *** rex signal received ***
    // check for watchdog expiry
    if ((rex_signals_mask & FMRDS_RPT_TIMER_SIG) != 0)
    {
      dog_report(DOG_FMRDS_RPT);    // report that we're still alive
      (void) rex_set_timer( &fmrdsReportTimer, FMRDS_DOG_RPT_TIME );   // reload timer
    }
    
    // handle "stop" and "offline" signals if received
    if ((rex_signals_mask & FMRDS_TASK_STOP_SIG) != 0)
    {
      (void) rex_clr_sigs( &fmrds_tcb, FMRDS_TASK_STOP_SIG );
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
    }

    if ((rex_signals_mask & FMRDS_TASK_OFFLINE_SIG) != 0)
    {
      (void) rex_clr_sigs( &fmrds_tcb, FMRDS_TASK_OFFLINE_SIG );
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
    }
    

    // check for rds update signal passed from isr and process accordingly:
    // rds group/block data and other info waiting
    if ( ( rex_signals_mask & FMRDS_UPDATE_RDS_SIG ) !=0 )
    {
      // nb: sig needs to be cleared before device interrupt re-enabled
      (void) rex_clr_sigs(&fmrds_tcb, FMRDS_UPDATE_RDS_SIG );  // clear update signal

      // update & handle device rds info: most unsolicited rds events generated here
      switch (FmrdsDev_GetDeviceUpdate())  // refresh data from fm device
      {   

        // check for raw rds buffer full
        case FMRDS_UPDATE_BLOCK_BUFF_FULL:
        {
          // raw block data buffer full
          // generate raw data event here
          FmrdsProcessRdsBuffFull();
          break;
        }

        // check for new ps/rt/af data
        case FMRDS_UPDATE_PS_AVAILABLE:
        case FMRDS_UPDATE_RT_AVAILABLE:
        case FMRDS_UPDATE_AF_AVAILABLE:
          break;

        // no action required
        case FMRDS_UPDATE_DONE:
        default:
          break;
      }
    }


#ifdef FM_MONITOR_SUPPORTED
    // check for any periodic changes in receiver status
    if ( ( rex_signals_mask & FMRDS_MONITOR_SIG ) !=0 )
    {
      (void) rex_clr_sigs( &fmrds_tcb, FMRDS_MONITOR_SIG );   // clear signal

      // handle any periodic updates required
      FmrdsSendTuneStatusEvent();   // send current rssi, etc
      
      // add any further updates req'd here
      // ...
    }
#endif  // FM_MONITOR_SUPPORTED

    
    // finally, check for new commands in command queue...
    if ((rex_signals_mask & FMRDS_CMD_Q_SIG) != 0)
    {
      // clear the 'queued commands' signal
      rex_clr_sigs( &fmrds_tcb, FMRDS_CMD_Q_SIG );

      // *** command in queue ***
      FmrdsApiProcessCommandQueue();  // handle as appropriate
    }
       
  }

}     // end of fmrds task

#ifdef FM_MONITOR_SUPPORTED

/************************************************************************************

FUNCTION      FmrdsStatusMonitor( )

DESCRIPTION	  This routine is called periodically on a 2 second basis to monitor
              the conditions (RSSI etc.) of FM-RDS device.
              
DEPENDENCIES  Will only be called when the callback timer has been registered
              using FmrdsStartStatusMonitor. FmrdsStartStatusMonitor must
              have been previously called.

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsStatusMonitor(void)
{
   /* Set signal into fmrds_task */
   (void)rex_set_sigs( &fmrds_tcb, FMRDS_MONITOR_SIG);
}

/************************************************************************************

FUNCTION      FmrdsStartStatusMonitor( )

DESCRIPTION	  This routine registers interrupt based timer callbacks.
              Registers FmrdsStatusMonitor.
              
DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsStartStatusMonitor(void)
{
  // Register this routine with the Clock services, so that it is called at
  // regular intervals to poll the status 
  clk_dereg( &CbFmrdsClkStatusMonitor);
  clk_def( &CbFmrdsClkStatusMonitor);      
  clk_reg( &CbFmrdsClkStatusMonitor,
      FmrdsStatusMonitor, FMRDS_MONITOR_POLL_TIME, FMRDS_MONITOR_POLL_TIME, TRUE );
}

/************************************************************************************

FUNCTION      FmrdsStopStatusMonitor( )

DESCRIPTION	  This routine terminates 2 second monitoring routine FmrdsStatusMonitor
              
DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsStopStatusMonitor(void)
{
   clk_dereg( &CbFmrdsClkStatusMonitor);
}

#endif //#ifdef FM_MONITOR_SUPPORTED

/************************************************************************************

FUNCTION      FmrdsProcessRdsBuffFull( )

DESCRIPTION	  Gets RDS buffer related information from lower layers and posts 
              FMRDS_EV_RAW_RDS_RBDS_DATA to registered client
              
DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsProcessRdsBuffFull(void)
{
   tsFmrdsEvMsgType   fmrdsEvBuff;

   FMRDS_MSG_HIGH("Post event FMRDS_EV_RAW_RDS_RBDS_DATA",0,0,0); 

   // construct event
   fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA;
   fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;
   fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRawRdsBlockEv = 
                                                           FmrdsDev_GetRdsBuffInfo();

   // send constructed event to client(s)
   FmrdsSendEventToClients(&fmrdsEvBuff);  
}


/************************************************************************************

FUNCTION      FmrdsSaveStation(void)

DESCRIPTION	  Common function to save currently tuned channel for later restoration
              if station seek/search aborted.

              Station freq saved in usStoredStation.
              
DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsSaveStation(void)
{
  tsFmrdsDevLockSyncReturn fmrdsTuneStatus;     // current tune/lock status

  fmrdsTuneStatus = FmrdsDev_GetTuneStatus();   // retrieve instantaneous tune info
  usStoredStation = fmrdsTuneStatus.tsDevLockSyncStatus.tusDevFreq;   // save freq
}


/************************************************************************************

FUNCTION      FmrdsRestoreStation(void)

DESCRIPTION	  Common function to restore previously saved fm station. Used after
              seek/search cancellation or other search error.
              
              Station freq retrieved from usStoredStation.

DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsRestoreStation(void)
{
  FmrdsDev_SetStation(usStoredStation);   // retune to stored station
}


/************************************************************************************

FUNCTION      FmrdsSendTuneStatusEvent(tsFmrdsEvMsgType fmrdsEvBuff)

DESCRIPTION	  Common function to retrieve info on currently tuned channel and
              return corresponding Tune Status event to client.
              
DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None.

************************************************************************************/
LOCAL void FmrdsSendTuneStatusEvent(void)
{
  tsFmrdsDevLockSyncReturn      fmrdsTuneStatus;        // current tune/lock status
  tsFmrdsEvMsgType              fmrdsEvBuff;

  fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RADIO_TUNE_STATUS;
  fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE; 
  
  fmrdsTuneStatus = FmrdsDev_GetTuneStatus();             // retrieve instantaneous tune info
  
  if(fmrdsTuneStatus.teDevLockSyncResult == FMRDSDEV_OK)
  {
    // construct and pass up event response: FMRDS_EV_RADIO_TUNE_STATUS
    fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // nb: FMRDS_EV_SUCCESS implies station locked
    
    // fill in message body settings from generic driver
    fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsTuneStatusEv.stationFreq = \
                                                      fmrdsTuneStatus.tsDevLockSyncStatus.tusDevFreq;   
    fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsTuneStatusEv.rssi = \
                                                      fmrdsTuneStatus.tsDevLockSyncStatus.tucDevRSSIVal; 
    fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsTuneStatusEv.stereoProgram = \
                                                      fmrdsTuneStatus.tsDevLockSyncStatus.teDevStState;
  }
  
  FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
  
  return;
}


/************************************************************************************

FUNCTION      FmrdsApiProcessCommandQueue

DESCRIPTION   De-queue commands from the FM RDS command queue and process them  
              
DEPENDENCIES  FEATURE_FM

RETURN VALUE  None

SIDE EFFECTS  None

************************************************************************************/
LOCAL void FmrdsApiProcessCommandQueue(void)
{
  tsFmrdsCmdMsgType *cmd_ptr;
  
  while ( (cmd_ptr = (tsFmrdsCmdMsgType*)q_get( &fmrdsCmdQ )) != NULL)
  {
    FmrdsApiProcessCommand(cmd_ptr);

    /* Give back the buffer to the free queue */
    FmrdsInitializeCmdBuffer( cmd_ptr );
    q_put( &fmrdsCmdFreeQ, &cmd_ptr->fmrdsCmdHdr.fmrdsLink );
  }
}										


/************************************************************************************

FUNCTION      FmrdsApiProcessCommand

DESCRIPTION   Process commands from command queue.
              
DEPENDENCIES  FEATURE_FM

RETURN VALUE  None

SIDE EFFECTS  None

************************************************************************************/
LOCAL void FmrdsApiProcessCommand(tsFmrdsCmdMsgType *fmrds_cmd_ptr)
{
  
  // temp storage while processing commands
  teFmrdsDevResult              fmrdsResultCode;        // return code from device-level operation
  tsFmrdsEvMsgType              fmrdsEvBuff;            // temp buffer for event generation
  tsFmrdsDevDeviceSettingsType  fmrdsSettings;          // current radio settings
  tsFmrdsDevLockSyncReturn      fmrdsTuneStatus;        // current tune/lock status
  tsFmrdsDevRdsFields           fmrdsCurrentRdsData;    // latest rds data for currently tuned station
  tsFmrdsDevSeekReturn          fmrdsSeekReturn;        // return status from seek operation

#ifdef FM_MONITOR_SUPPORTED
  static boolean                bTuneStatus = FALSE;
#endif

#ifdef FM_MONITOR_SUPPORTED
  //Stop Status monitor
  FmrdsStopStatusMonitor();
#endif 

  // should only reach this point with a valid command id
  switch(fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsCmdType)
  {
   
    // *** setup/init commands ***
    case FMRDS_ENABLE_FM:
    { 
      // power fm receiver area up, initialise, configure radio, leave in low power
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_ENABLE_FM;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
      fmrdsResultCode = FMRDSDEV_ERROR;

      if (FmrdsDev_SetPowerState(FMRDSDEV_POWER_UP) == FMRDSDEV_OK)  // hw on
      {
         if (FmrdsDev_SetRadioMode(RADIO_MODE_RX) == FMRDSDEV_OK)    // default to rx at present 
         { 
            if (FmrdsDev_Init() == FMRDSDEV_OK) // set sane defaults & set device in low power mode
            { 
               // pass initial radio settings
               fmrdsResultCode = FmrdsDev_ConfigureRadio(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSettings);
            }
         }
      }
       
      
      if (fmrdsResultCode == FMRDSDEV_OK)
      {
         // construct and pass up event response: FMRDS_EV_ENABLE_FM
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsEnableFmEv.radioSettings = fmrdsSettings.radioSettings;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsEnableFmEv.radioMode = fmrdsSettings.radioMode;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsEnableFmEv.radioCap = fmrdsSettings.radioCap;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsEnableFmEv.fmPwrUpMode = fmrdsSettings.fmPwrUpMode;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsEnableFmEv.rdsPwrUpMode = fmrdsSettings.rdsPwrUpMode;
      }

      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client

      // done
      break;
    }
    
    case FMRDS_DISABLE_FM:
    {
      // power fm receiver area down
      // shut off periodic monitor here?
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_DISABLE_FM;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
      
      fmrdsResultCode = FmrdsDev_SetPowerState(FMRDSDEV_POWER_DOWN);

      if(fmrdsResultCode == FMRDSDEV_OK)  // hw off
      {
         // construct and pass up event response: FMRDS_EV_DISABLE_FM
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsDisableFmEv.radioSettings = fmrdsSettings.radioSettings;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsDisableFmEv.radioMode = fmrdsSettings.radioMode;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsDisableFmEv.fmPwrUpMode = fmrdsSettings.fmPwrUpMode;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsDisableFmEv.rdsPwrUpMode = fmrdsSettings.rdsPwrUpMode;
      }

      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
#ifdef FM_MONITOR_SUPPORTED  

      //turn off monitoring
      if(fmrdsResultCode == FMRDSDEV_OK)
         bTuneStatus = FALSE;
#endif

      // done
      break;
    }
    
    case FMRDS_SET_PWR_MODE:
    { 
      // set low/normal power operation
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_PWR_MODE_SET;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      if (fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsPwrMode == FMRDS_LOW_POWER_MODE)
      {
        fmrdsResultCode = FmrdsDev_SetPowerState(FMRDSDEV_LOW_POWER);
      }
      else if (fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsPwrMode == FMRDS_NORMAL_MODE)
      {
        fmrdsResultCode = FmrdsDev_SetPowerState(FMRDSDEV_HIGH_POWER);
      }
      else
      {
        // flag parameter error, do nothing
        FMRDS_MSG_ERROR("FMRDS: Wrong Power Mode Value %d",
                          fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsPwrMode, 0, 0);
        break;
      }
      
      if(fmrdsResultCode == FMRDSDEV_OK)
      {
         // construct and pass up event response: FMRDS_EV_PWR_MODE_SET
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPwrModeEv.pwrMode = fmrdsSettings.pwrMode;
      }
 
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }
    
    case FMRDS_SET_RADIO_MODE:
    {   
      // set rx or tx operation
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RADIO_MODE_SET;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsResultCode = FmrdsDev_SetRadioMode(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioMode);

      if(fmrdsResultCode == FMRDSDEV_OK)
      {      
         // construct and pass up event response: FMRDS_EV_RADIO_MODE_SET
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRadioModeEv.radioMode = fmrdsSettings.radioMode;
      }

      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }
    
    case FMRDS_CFG_RADIO:
    {
      
      // set rx side band, channel spacing, de-emph, rds mode
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_CFG_RADIO;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
      
      fmrdsResultCode = FmrdsDev_ConfigureRadio(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSettings);

      if(fmrdsResultCode == FMRDSDEV_OK)
      {
         // construct and pass up event response: FMRDS_EV_CFG_RADIO
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsConfigRadioEv.radioSettings = fmrdsSettings.radioSettings;
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }
    
    
    // *** rx commands ***
    case FMRDS_SET_MUTE_MODE:
    {      
      // set rx audio mute mode
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_MUTE_MODE_SET;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsResultCode = FmrdsDev_SetMuteMode(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsMuteCtrl);

      if(fmrdsResultCode == FMRDSDEV_OK)
      {
         // construct and pass up event response: FMRDS_EV_MUTE_MODE_SET
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsMuteCtrlEv.muteMode = fmrdsSettings.muteMode;
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }
    
    case FMRDS_SET_STEREO_MODE:
    {      
      // set rx audio stereo/mono mode
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_STEREO_MODE_SET;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsResultCode = FmrdsDev_SetStereoMode(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsStereoMonoMode);

      if(fmrdsResultCode == FMRDSDEV_OK)
      {
         // construct and pass up event response: FMRDS_EV_STEREO_MODE_SET
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsStereoMonoModeEv.monoStereo = fmrdsSettings.monoStereo;
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }
    
    case FMRDS_SET_VOLUME_LEVEL:
    {      
      // set rx or tx audio volume levels
      if (fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsVolLevOpts.volMode == FM_VOLUME_MODE_RX)
      {
         fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RX_VOLUME_LEVEL_SET;
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
         
         // rx volume setting
         fmrdsResultCode = FmrdsDev_SetRxAudioLevel(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsVolLevOpts.volLev);
         if(fmrdsResultCode == FMRDSDEV_OK)
         {
            fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // return rx event success
            fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
            // load event with level here
            fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsVolLevEv.fmrdsVolLev = fmrdsSettings.rxAudioLevel;
         }

      }
      else
      {
        // tx side unsupported at present
        fmrdsEvBuff.fmrdsEventName = FMRDS_EV_TX_VOLUME_LEVEL_SET;
        fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;           // return tx event fail
      }
   
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)

      // done
      break;
    }
          
    case FMRDS_SET_RADIO_STATION:
    {      
      // tune to rx channel
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RADIO_STATION_SET;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsResultCode = FmrdsDev_SetStation(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsFreq);  // initiate tuning

      if(fmrdsResultCode == FMRDSDEV_OK)
      {
         // assume device layer has tuned to channel & locked (FMRDS_EV_SUCCESS):
         // construct and pass up event response: FMRDS_EV_RADIO_STATION_SET 
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsStationSetEv.stationFreq = fmrdsSettings.stationFreq;
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);        // send station set event to client(s)...

      FmrdsSendTuneStatusEvent( );                  // ...and corresponding tune status

#ifdef FM_MONITOR_SUPPORTED  
      //successfully tuned to a station
      if(fmrdsResultCode == FMRDSDEV_OK)
         bTuneStatus = TRUE;
      else
         bTuneStatus = FALSE;
#endif

      // done
      break;
    }
    
    case FMRDS_GET_TUNE_STATUS:
    {      
      // set rx freq, rssi status
      FmrdsSendTuneStatusEvent( );   // construct and send tune status event for current channel
      
      // done
      break;
    }

    case FMRDS_GET_RSSI_INFO:
    {      
      // get current rssi level for tuned station
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RSSI_INFO;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsTuneStatus = FmrdsDev_GetTuneStatus();     // retrieve instantaneous tune info

      if(fmrdsTuneStatus.teDevLockSyncResult == FMRDSDEV_OK)
      {
         // construct and pass up event response: FMRDS_EV_RSSI_INFO
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in message body settings from generic driver
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRssiInfoEv.rssi = fmrdsTuneStatus.tsDevLockSyncStatus.tucDevRSSIVal; 
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRssiInfoEv.ifcnt = fmrdsTuneStatus.tsDevLockSyncStatus.ucDevIfcnt;
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }

    case FMRDS_GET_STATION_PARAMETERS:
    {      
      // return general tuned station info
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_STATION_PARAMETERS;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsTuneStatus = FmrdsDev_GetTuneStatus();     // retrieve instantaneous tune info

      if(fmrdsTuneStatus.teDevLockSyncResult == FMRDSDEV_OK)
      {
         // tune status retrieved ok: construct and pass up event response: FMRDS_EV_STATION_PARAMETERS
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
         // fill in tune info
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.stationFreq = fmrdsTuneStatus.tsDevLockSyncStatus.tusDevFreq;   
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.rssi = fmrdsTuneStatus.tsDevLockSyncStatus.tucDevRSSIVal; 
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.ifcnt = fmrdsTuneStatus.tsDevLockSyncStatus.ucDevIfcnt;
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.stereoProgram = fmrdsTuneStatus.tsDevLockSyncStatus.teDevStState;

         // translate sync status from device level: not optimal
         if (fmrdsTuneStatus.tsDevLockSyncStatus.teDevRdsSyncState == FMRDS_RDSSYNC)
            fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.rdsSyncStatus = RDS_SYNC;
         else
            fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.rdsSyncStatus = NO_RDS_SYNC;

         // fill in general settings
         fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve more general settings
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsParamAvbleEv.muteMode = fmrdsSettings.muteMode; 
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }

    case FMRDS_GET_RDS_LOCK_STATUS:
    {      
      // return current rds lock status, fail if rds disabled
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RDS_LOCK_STATUS;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsSettings = FmrdsDev_GetCurrentSettings( );   // retrieve current settings

      if(fmrdsSettings.rdsPwrUpMode == RDS_ON)
      {
        // rds reception enabled
        fmrdsTuneStatus = FmrdsDev_GetTuneStatus();       // retrieve instantaneous tune info

        if(fmrdsTuneStatus.teDevLockSyncResult == FMRDSDEV_OK)
        {
          // construct and pass up event response: FMRDS_EV_RDS_LOCK_STATUS
          if (fmrdsTuneStatus.tsDevLockSyncStatus.teDevRdsSyncState == FMRDS_RDSSYNC)
          {
              // rds on and synch'd
              fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
         
              // fill in message body settings from generic driver
              fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRdsLockStatusEv.stationFreq =
                                                                            fmrdsTuneStatus.tsDevLockSyncStatus.tusDevFreq;   
              fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRdsLockStatusEv.rssi =
                                                                            fmrdsTuneStatus.tsDevLockSyncStatus.tucDevRSSIVal; 
              fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRdsLockStatusEv.rdsCapability =
                                                                            fmrdsTuneStatus.tsDevLockSyncStatus.rdsCapability;
          }
        }
      }
      
      // return pass or fail event...
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }

    case FMRDS_GET_RDS_PS_INFO:
    {      
      // get latest ps info for tuned station, fail if rds disabled
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RDS_PS_INFO;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsSettings = FmrdsDev_GetCurrentSettings( );   // retrieve current settings

      if(fmrdsSettings.rdsPwrUpMode == RDS_ON)
      {
        // rds reception enabled: construct and pass up event response: FMRDS_EV_RDS_PS_INFO
        fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;   // flag success

        fmrdsCurrentRdsData = FmrdsDev_GetRdsData();    // get snapshot of current rds info

        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.psNum = fmrdsCurrentRdsData.psNum;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.piPrgmId =
                                                                            fmrdsCurrentRdsData.fmrdsPi.tuFmrdsPiUnion.usRdsPi;

        // pty: check rds standard so correct enumerations used
        if (fmrdsSettings.radioSettings.rdsStd == RDS_STD_RBDS)
        {
          fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.ptyPrgmType.rdsOrRbdsMode.rbdsPrgmType =
                                                                            fmrdsCurrentRdsData.fmrdsPty.tuFmrdsPtyUnion.rbdsPty;
        }
        else
        {
          fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.ptyPrgmType.rdsOrRbdsMode.rdsPrgmType =
                                                                            fmrdsCurrentRdsData.fmrdsPty.tuFmrdsPtyUnion.rdsPty;
        }

        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.taPrgmCodeFlag = fmrdsCurrentRdsData.taPrgmCodeFlag;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.taAnnouncementCodeFlag = fmrdsCurrentRdsData.taAnnouncementCodeFlag;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.msSwitchCodeFlag = fmrdsCurrentRdsData.msSwitchCodeFlag;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.decIdCtrlCodeFlag = fmrdsCurrentRdsData.decIdCtrlCodeFlag;

        // ps service name characters not currently populated

        // point to null-terminated ps string
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsPSSummaryEv.pCmptPSText = fmrdsCurrentRdsData.pucPs;
      }
      
      // return pass or fail event...
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }

    case FMRDS_GET_RDS_RT_INFO:
    {      
      // get latest rt info for tuned station, fail if rds disabled
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RDS_RT_INFO;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsSettings = FmrdsDev_GetCurrentSettings( );   // retrieve current settings

      if(fmrdsSettings.rdsPwrUpMode == RDS_ON)
      {
        // rds reception enabled: construct and pass up event response: FMRDS_EV_RDS_RT_INFO
        fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;   // flag success

        fmrdsCurrentRdsData = FmrdsDev_GetRdsData();    // get snapshot of current rds info

        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.length = fmrdsCurrentRdsData.ucRtLength;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.piPrgmId =
                                                                            fmrdsCurrentRdsData.fmrdsPi.tuFmrdsPiUnion.usRdsPi;

        // pty: check rds standard so correct enumerations used
        if (fmrdsSettings.radioSettings.rdsStd == RDS_STD_RBDS)
        {
          fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.ptyPrgmType.rdsOrRbdsMode.rbdsPrgmType =
                                                                            fmrdsCurrentRdsData.fmrdsPty.tuFmrdsPtyUnion.rbdsPty;
        }
        else
        {
          fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.ptyPrgmType.rdsOrRbdsMode.rdsPrgmType =
                                                                            fmrdsCurrentRdsData.fmrdsPty.tuFmrdsPtyUnion.rdsPty;
        }

        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.taPrgmCodeFlag = fmrdsCurrentRdsData.taPrgmCodeFlag;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.textAbFlag = fmrdsCurrentRdsData.textABFlag;
 
        // point to null-terminated rt string      
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRTSummaryEv.pRadioText = fmrdsCurrentRdsData.pucRadioText;
      }
      
      // return pass or fail event...
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }

    case FMRDS_GET_RDS_AF_INFO:
    {
      // get latest af info for tuned station, fail if rds disabled
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RDS_AF_INFO;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      fmrdsSettings = FmrdsDev_GetCurrentSettings( );   // retrieve current settings
      fmrdsTuneStatus = FmrdsDev_GetTuneStatus();       // retrieve instantaneous tune info

      if((fmrdsSettings.rdsPwrUpMode == RDS_ON) && (fmrdsTuneStatus.teDevLockSyncResult == FMRDSDEV_OK))
      {
        // rds reception enabled: construct and pass up event response: FMRDS_EV_RDS_AF_INFO
        fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;   // flag success
        
        // copy tuned freq
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsAFSummaryEv.radioFreq = \
                                                          fmrdsTuneStatus.tsDevLockSyncStatus.tusDevFreq;   
            
        fmrdsCurrentRdsData = FmrdsDev_GetRdsData();    // get snapshot of current rds info
        
        // copy pi
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsAFSummaryEv.piPrgmId = \
                                                          fmrdsCurrentRdsData.fmrdsPi.tuFmrdsPiUnion.usRdsPi;
        // copy no of afs returned
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsAFSummaryEv.noOfAFInList = \
                                                          fmrdsCurrentRdsData.noOfAFInList;

        // copy af list itself if populated
        if (fmrdsCurrentRdsData.noOfAFInList > MAX_AF_LIST_VAL)
        {
          // klocwork: returned list value overrange, so fail
          fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
        }
        else
        {
          // copy any afs to returned event
          while (fmrdsCurrentRdsData.noOfAFInList > 0)
          {
            fmrdsCurrentRdsData.noOfAFInList--;
            fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsAFSummaryEv.AFFreqList[fmrdsCurrentRdsData.noOfAFInList] = \
              fmrdsCurrentRdsData.AFFreqList[fmrdsCurrentRdsData.noOfAFInList];
          }
        }
      }
      
      // return pass or fail event...
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
       
      // done
      break;
    }
    
#ifdef FEATURE_FM_SERVICE_AVAILABLE
    case FMRDS_SERVICE_AVAILABLE:
    {      
      // check for fm service available
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_SERVICE_AVAILABLE;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;

      // example only: *** service availability check goes here ***
      // pass fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsServiceRssiLev to appropriate
      // non-blocking function

      // construct event response: FMRDS_EV_SERVICE_AVAILABLE
      // example only: *** return service available ***
      {
         fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;      
         fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters. \
                fmrdsFmServiceAvailableEv.serviceAvailable = SERVICE_AVAILABLE;
      }
      
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
      
      // done
      break;
    }
#endif  // FEATURE_FM_SERVICE_AVAILABLE
 
    case FMRDS_SET_RDS_RX_PROCESSING_OPTIONS:
      {
        
        // set rx side raw rds mode settings
        fmrdsEvBuff.fmrdsEventName = FMRDS_EV_RDS_RX_OPTIONS_SET;
        fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
        
        fmrdsResultCode = FmrdsDev_SetRdsRxProcessingOptions(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsRxProcOpt); 
        
        // fill in message body settings from generic driver
        fmrdsSettings = FmrdsDev_GetCurrentSettings( );     // retrieve any updated settings
        
        
        if((fmrdsResultCode == FMRDSDEV_OK)&& (fmrdsSettings.rawRdsProcOpts.rdsReturnRawBlocks == TRUE))
        {
          // construct and pass up event response: FMRDS_EV_RDS_RX_OPTIONS_SET
          fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;       // flag success
        }
        
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.rdsReturnRawBlocks = fmrdsSettings.rawRdsProcOpts.rdsReturnRawBlocks;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.rdsBufSz = fmrdsSettings.rawRdsProcOpts.rdsBufSz;     
        
        // Currently not used - Hardcoded values
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.enRdsGrps = (uint32)0; 
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.enRdsChangeFiltr = FALSE; 
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.enPSUncrtbleBlks = FALSE;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.enRdsBlkEProc = FALSE;
        fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsRxProcOptEv.
          rxProcOpt.enRdsAutoAF = FALSE;
        
        
        FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)
        
        // done
        break;
      }


    case FMRDS_SEARCH_STATIONS:
    {
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_SEARCH_COMPLETE;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
      fmrdsSeekReturn.teDevSeekResult = FMRDSDEV_ERROR;

      if (fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSrchMode.srchMode == SEARCHMODE_SEEK)
      {
        // seek next station
        FmrdsSaveStation( );  // save current station, initiate seek
        fmrdsSeekReturn = FmrdsDev_StationSeek(fmrds_cmd_ptr->fmrdsCmdMsgBody.tuCmdParams.fmrdsRadioSrchMode);

        if (fmrdsSeekReturn.teDevSeekResult == FMRDSDEV_OK)
        {
          // no errors on receiver seek
          fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;

          if (fmrdsSeekReturn.bandLimitReached == FALSE)
            fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsSrchComplete.serviceAvble = SERVICE_AVAILABLE;
          else
            fmrdsEvBuff.fmrdsEventMsgBody.tufmrdsStatusParameters.fmrdsSrchComplete.serviceAvble = SERVICE_NOT_AVAILABLE;

          FmrdsSendEventToClients(&fmrdsEvBuff);    // send search complete event

          if (fmrdsSeekReturn.bandLimitReached == TRUE)
          {
            // no new station found
            FmrdsRestoreStation( );       // retune to previous station
          }

          FmrdsSendTuneStatusEvent( );    // report new or previous station with tune status event
        }
        else
        {
          // receiver failure
          FmrdsSendEventToClients(&fmrdsEvBuff);  // send search_complete with error, done
        }    
      }
      else
      {
        // parameter not supported
        FmrdsSendEventToClients(&fmrdsEvBuff);  // send search_complete with error, done
      }

#ifdef FM_MONITOR_SUPPORTED  
      //successfully tuned to a station
      if(fmrdsSeekReturn.teDevSeekResult == FMRDSDEV_OK)
         bTuneStatus = TRUE;
      else
         bTuneStatus = FALSE;
#endif

      // done
      break;
    }
    

    case FMRDS_CANCEL_SEARCH:
    {
      // cancel any scan operation in progress (limited use for seek)
      fmrdsEvBuff.fmrdsEventName = FMRDS_EV_SEARCH_CANCELED;
      fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_FAILURE;
        
      fmrdsResultCode = FmrdsDev_CancelSeek( ); // cancel seek/scan

      if (fmrdsResultCode == FMRDSDEV_OK)
      {
        // seek/scan cancel ok
        fmrdsEvBuff.fmrdsEvResult = FMRDS_EV_SUCCESS;
      }
      FmrdsSendEventToClients(&fmrdsEvBuff);  // send constructed event to client(s)

      FmrdsRestoreStation( );         // restore previously tuned station
      FmrdsSendTuneStatusEvent( );    // send associated tune status event
             
      // done
      break;
    }
      
#if 0   // placemarkers for future commands
    case FMRDS_REG_RDS_RX_GRPS_PROCESSING:
    {
      ;
    }
    break;
#endif // #if 0 placemarkers
    
    
    // *** command ids that should not occur (unsupported commands trapped previously) ***
    case FMRDS_NULL_COMMAND:      // null command handled elsewhere
    
    default:
    {
      // trap any invalid command ids, warn & do nothing
      FMRDS_MSG_ERROR("FMRDS: Invalid command id: %x", fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsCmdType, 0, 0);
      break;
    }

  }   // end switch
 
#ifdef FM_MONITOR_SUPPORTED  
  //if tuned to a station, start status monitor
  if(bTuneStatus)
     FmrdsStartStatusMonitor();
#endif

}   // end of FmrdsApiProcessCommand



/************************************************************************************
***
***     command handling support functions
***
************************************************************************************/

/************************************************************************************

FUNCTION      FmrdsInitClients( )

DESCRIPTION	  Explicitly initialises client registration.
              
DEPENDENCIES  None.

RETURN VALUE  None.
              
SIDE EFFECTS  None.

************************************************************************************/
void FmrdsInitClients(void)
{
  // temp: support single client only
  // tbc - protect with crit_sect if multiple clients
  pfnFmrdsClientEvCb = NULL;                  // null out cb pointers
  teFmrdsCbEvMask = FM_CB_EV_MASK_SEND_NONE;  // return no events
  ucFmrdsNoOfClients = 0;                     // no clients
  // tbc - protect with crit_sect if multiple clients
  return;
}


/************************************************************************************

FUNCTION      FmrdsAddClient( )

DESCRIPTION	  Adds a client for FM/RDS services, stores any client-related data
              including callback pointer.
              
DEPENDENCIES  None

RETURN VALUE  Returns new client id, else zero on error.
              
SIDE EFFECTS  None.

************************************************************************************/
uint8 FmrdsAddClient(tfFmrdsEvCbFnType  *pfnFmrdsEvCb, teFmrdsCbEvMaskType fmCbEvMask)
{
  uint8 ucClientCnt;

  // temp: support single client only
  // tbc - protect with crit_sect if multiple clients
  pfnFmrdsClientEvCb = pfnFmrdsEvCb;
  teFmrdsCbEvMask = fmCbEvMask;
  ucClientCnt = ++ucFmrdsNoOfClients;
  // tbc - protect with crit_sect if multiple clients
  return(ucClientCnt);
}


/************************************************************************************

FUNCTION      FmrdsRemoveClient( )

DESCRIPTION	  Remove specified client id and related data from the list.
              
DEPENDENCIES  None

RETURN VALUE  Returns removed client id, else zero.
              
SIDE EFFECTS  None.

************************************************************************************/
void FmrdsRemoveClient(uint8 ucClientId)
{
  // temp: support single client only
  // tbc - protect with crit_sect if multiple clients
  ucClientId = ucClientId;  // temp

  pfnFmrdsClientEvCb = NULL;
  teFmrdsCbEvMask = FM_CB_EV_MASK_SEND_NONE;
  ucFmrdsNoOfClients--;
  // tbc - protect with crit_sect if multiple clients

  return;
}


/************************************************************************************

FUNCTION      FmrdsGetClientCount( )

DESCRIPTION	  Return number of currently-registered clients.
              
DEPENDENCIES  None

RETURN VALUE  Returns client count, 0 = no clients registered.
              
SIDE EFFECTS  None.

************************************************************************************/
uint8 FmrdsGetClientCount(void)
{
  // temp: support single client only
  // tbc - protect with crit_sect if multiple clients
  return(ucFmrdsNoOfClients);
  // tbc - protect with crit_sect if multiple clients
}


/************************************************************************************

FUNCTION      FmrdsSendEventToClients( )

DESCRIPTION	  Send event pointed to all FM/RDS clients using previously-registered
              callbacks.
              
DEPENDENCIES  None

RETURN VALUE  None.
              
SIDE EFFECTS  None.

************************************************************************************/
void FmrdsSendEventToClients(tsFmrdsEvMsgType *ptsEvPtr)
{
  // *** current support single client only ***
  if((FmrdsGetClientCount()) && (pfnFmrdsClientEvCb != NULL))
  {
    // > 0 clients, cb ptr not null,
    // traverse registered clients and return event using registered event masks
    if (teFmrdsCbEvMask == FM_CB_EV_MASK_SEND_ALL)
    {
      // tbc - protect with crit_sect if multiple clients
      (pfnFmrdsClientEvCb) (*ptsEvPtr);   // pass event structure by callback   
      // tbc - protect with crit_sect if multiple clients

      FMRDS_MSG_HIGH("FMRDS: Event Returned = %x",ptsEvPtr->fmrdsEventName,0,0);
    }
    else
    {
      FMRDS_MSG_HIGH("FMRDS: Return all events not set",0,0,0);
    }
  }
  else
  {
    // error - should not occur
    FMRDS_MSG_ERROR("FMRDS: Send event error",0,0,0);
  }

  return;  
}


/************************************************************************************

FUNCTION      FmrdsCmdSupported( )

DESCRIPTION	  Check whether command id passed is currently supported in this
              implementation. Provides a centralised means of trapping unsupported
              commands, so that unrecognised status can be immediately returned.
              
DEPENDENCIES  None

RETURN VALUE  Returns non-zero if command supported, else zero.
              
SIDE EFFECTS  None.

************************************************************************************/
boolean FmrdsCmdSupported(teFmrdsCmdType fmrdsCmdId)
{
  boolean bFmrdsIsSupported = FALSE;

  switch(fmrdsCmdId)
  {
    // list of currently supported queued commands
    // *** rx commands: supported for 3rd-party device ***
    case FMRDS_ENABLE_FM:
    case FMRDS_DISABLE_FM:
    case FMRDS_SET_PWR_MODE:
    case FMRDS_SET_RADIO_MODE:
    case FMRDS_CFG_RADIO:
    case FMRDS_SET_MUTE_MODE:
    case FMRDS_SET_STEREO_MODE:
    case FMRDS_SET_RADIO_STATION:
    case FMRDS_SET_VOLUME_LEVEL:
    case FMRDS_GET_TUNE_STATUS:
    case FMRDS_GET_RSSI_INFO:
    case FMRDS_GET_STATION_PARAMETERS:
    case FMRDS_GET_RDS_LOCK_STATUS:
    case FMRDS_GET_RDS_PS_INFO:
    case FMRDS_GET_RDS_RT_INFO:
    case FMRDS_GET_RDS_AF_INFO:
#ifdef FEATURE_FM_SERVICE_AVAILABLE
    case FMRDS_SERVICE_AVAILABLE:
#endif  // FEATURE_FM_SERVICE_AVAILABLE
    case FMRDS_SET_RDS_RX_PROCESSING_OPTIONS:
    case FMRDS_SEARCH_STATIONS:
    case FMRDS_CANCEL_SEARCH:

    // *** rx commands: not supported in current release ***  
    //case FMRDS_REG_RDS_RX_GRPS_PROCESSING: 
    // *** rx commands: may not be supported for 3rd-party device ***  
    //case FMRDS_RDS_SEARCH_STATIONS:
    //case FMRDS_SEARCH_STATION_LIST:
    //case FMRDS_REG_PI_MATCH_PROCESSING: 
    //case FMRDS_REG_BLOCKB_PROCESSING:
    // *** tx commands: supported for 3rd-party device ***
    //case FMRDS_SET_TX_PWR_LEVEL:
    //case FMRDS_GET_TX_SETTINGS:
    //case FMRDS_TX_PS_INFO:
    //case FMRDS_STOP_TX_PS_INFO:
    //case FMRDS_TX_RT_INFO:
    //case FMRDS_STOP_TX_RT_INFO:
    // *** tx commands: may not be supported for 3rd-party device ***  
    //case FMRDS_TX_RDS_GROUPS:
    //case FMRDS_TX_CONT_RDS_GROUPS:
    //case FMRDS_TX_RDS_GROUP_CTRL:
    //case FMRDS_REG_RTPLUS_CLS_PROCESSING:
    //case FMRDS_TX_RTPLUS_INFO:
    {
      bFmrdsIsSupported = TRUE;
      break;
    }

    // else return unsupported
    default:
      break;
  }

  return(bFmrdsIsSupported); 
}


/************************************************************************************

FUNCTION      FmrdsApiPutCommand

DESCRIPTION	  This function takes a filled out tsFmrdsCmdMsgType and places
              it on the FM-RDS command queue.
              
DEPENDENCIES  None.

RETURN VALUE  teFmrdsCmdQStatusType
              
SIDE EFFECTS  Sets the FMRDS_CMD_Q_SIG.

************************************************************************************/
teFmrdsCmdQStatusType FmrdsApiPutCommand(tsFmrdsCmdMsgType *fmrds_cmd_ptr)
{
  boolean                   free_cmd = TRUE;    // default: release buffer
  int                       fmrds_cmd_q_depth;
  teFmrdsCmdType            fmrdsCmdId;         // current command id
  teFmrdsCmdQStatusType     cmd_status;         // command return status
  

  cmd_status = FMRDS_CMD_Q_UNRECOGNIZED_CMD;  // default return status

  if (FmrdsGetClientCount() > 0 )
  {
    // registered with at least one client
    fmrdsCmdId = fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsCmdType; // get command id

    if((fmrdsCmdId != FMRDS_NULL_COMMAND) && (fmrdsCmdId < FMRDS_MAX_CMD) &&
                                                    (FmrdsCmdSupported(fmrdsCmdId)))
    { 
      // valid range, command supported
      if ( (fmrds_cmd_q_depth = q_cnt( &fmrdsCmdQ )) < FMRDS_NUM_CMDS )
      {
        // room in command queue, so add command and set status
        cmd_status = FMRDS_CMD_Q_PENDING;      
        q_put( &fmrdsCmdQ, &fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsLink );
        (void) rex_set_sigs( &fmrds_tcb, FMRDS_CMD_Q_SIG );
        free_cmd = FALSE;   // keep buffer
      }
      else
      {
        // command queue full
        cmd_status = FMRDS_CMD_Q_FULL;
        FMRDS_MSG_ERROR( "FMRDS: Cmd Q Full - Depth %x", fmrds_cmd_q_depth, 0, 0 );
      }
    }
    else
    {
      // command id unrecognised or currently unsupported
      FMRDS_MSG_ERROR("FMRDS: Unrecognised command: %d",
                                  fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsCmdType, 0, 0);
    }
    
    // if error, return buffer to free queue
    if ( free_cmd != FALSE )
    {
      FmrdsInitializeCmdBuffer( fmrds_cmd_ptr );
      q_put( &fmrdsCmdFreeQ, &fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsLink );
    }  
  }
    
  return (cmd_status);

}   /* end of FmrdsApiPutCommand */


/************************************************************************************

FUNCTION      FmrdsInitializeCmdBuffer

DESCRIPTION	  Initialises buffer space for a command before use application.

              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

************************************************************************************/
LOCAL void FmrdsInitializeCmdBuffer(tsFmrdsCmdMsgType *fmrds_cmd_ptr)
{
  fmrds_cmd_ptr->fmrdsCmdHdr.fmrdsCmdType = FMRDS_NULL_COMMAND;
}


/************************************************************************************

FUNCTION      FmrdsGetFreeCmd

DESCRIPTION	  Requests command buffer space, if available.

DEPENDENCIES  None

RETURN VALUE  Pointer to free FM-RDS command buffer if available; otherwise NULL.

SIDE EFFECTS  Pointer to the command buffer or NULL if there are none free.

************************************************************************************/
static tsFmrdsCmdMsgType*  FmrdsGetFreeCmd(void)
{
  tsFmrdsCmdMsgType*  free_fmrds_cmd_ptr = NULL;
  
  if ( q_cnt( &fmrdsCmdFreeQ ) > 0 )
  {
    free_fmrds_cmd_ptr = (tsFmrdsCmdMsgType*) q_get( &fmrdsCmdFreeQ );
  }
  else
  {
    FMRDS_MSG_ERROR("FMRDS: Out of command buffers", 0, 0, 0);
  }
  
  return(free_fmrds_cmd_ptr);
}


#endif /* FEATURE_FM */
