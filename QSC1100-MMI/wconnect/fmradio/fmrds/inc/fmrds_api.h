#ifndef _FMRDS_API_H
#define _FMRDS_API_H
/*============================================================================
 @file fmrds_api.h

 File lists public function calls and commands for FM-RDS support. Operation 
 is based around an FM task that must be incorporated into the task list with 
 an appropriate priority,

 Higher-level code must first register with FM services as a client. 
 Subsequently, FM-related commands are passed from the Apps layer into an FM 
 command queue, for subsequent service by the FM task. Depending on function, 
 commands are actioned immediately and a response sent to the caller or, in 
 the case of actions involving some delay, pending command status is returned 
 and the command outcome returned some time later as an event.

 Event notification is asynchronous and is notified to the client by means of 
 a previously-registered callback function.


 Copyright (c) 2008 QUALCOMM Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary
 ============================================================================*/

/*=============================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order. Please
 use ISO format for dates.

 $Header: //source/qcom/qct/wconnect/fmradio/fmrds/main/latest/inc/fmrds_api.h#13 $$DateTime: 2008/11/26 09:33:21 $$Author: abhis $

 when       who	  what where, why
 ---------- ---   -------------------------------------------------------------
 2008-11-17 rjm   Simple periodic monitor code added.
 2008-10-31 rjm   Cleanup, teFmrdsCmdQStatusType removed.
 2008-09-17  as   Move F3 messages from 'legacy' to 'FM Radio'
 2008-09-10 rjm   Band limit definitions now in api.h
 2008-09-25 rjm   StereoMonoMode removed from ParamsAvlble event. Band-wrap
                  parameter added to seek/search functions. Search Complete
                  event type added for clarity. References to fm recording
                  removed. Missing union declaration for FmrdsCmdMsgBodyType
                  restored.
 2008-09-18 rjm   code clean-up
 2008-09-17 as    support for raw RDS data buffering added
 2008-09-09 rjm   Edited for 3rd party FM rx support.
                  Support for rdsReturnRawBlocks introduced.
                  stereoProgram type added to DevLockSync struct.
                  Legacy code removed from header. Some function renaming
                  for consistency.
 2008-09-09 rjm   stereoProgram field added to station parameter event.
 2008-09-05 as    stereoProgram field added to GetTuneStatus()
 2008-08-13 rjm	  Legacy service available switch added.
 2008-07-30 rjm	  Minor updates re client events & PS, RT return strings.
 2008-07-25 rjm	  Minor updates.
 2008-07-16 rjm	  Added reg event mask, updated crit sect macros. SetVolume()
                  updated. GetTuneStatus() command now supported. General clean
                  up.
 2008-07-09 as	  Band ranges now uint16.
 2008-07-07 as	  Minor changes to device defines to fix compilation errors.
 2008-05-22 rjm	  Minor changes to naming.
 2008-03-20 amar  Initial revision.

=============================================================================*/

 /*----------------------------------------------------------------------------
  * Include Files
  * -------------------------------------------------------------------------*/

#include "custfmrds.h"           /* Include global Fmrds switches */
#include "queue.h"
#include "task.h"
#include "cmd.h"
#include "msg.h"
#include "msmhwioreg_port.h"     /* Added to track RDS ISR usage */

#ifdef  FEATURE_FM

// *** functional defines ***
#define FEATURE_FM_SERVICE_AVAILABLE  // support for legacy service available command

/* FMRDS specific files */
#include "fmrds_device.h"        /* FMRDS device specific definitions */

 /*----------------------------------------------------------------------------
  * Preprocessor Definitions and Constants
  * -------------------------------------------------------------------------*/
/* Signal definitions */

// fmrds task
#define FMRDS_CMD_Q_SIG         0x00000001    // command queue signal
#define FMRDS_EVENT_Q_SIG       0x00000002    // legacy
#define FMRDS_RPT_TIMER_SIG     0x00000004    // watchdog signal

// fmrds functionality
#define FMRDS_WAIT_SIG          0x00000008    // legacy - fmrds wait signal
#define FMRDS_MONITOR_SIG       0x00000010    // status monitor signal, if enabled
#define FMRDS_UPDATE_RDS_SIG    0x00000020    // rds data update signal

// test signals
#define FMRDS_UPDATE_EXT_INT    0x00000080
#define FMRDS_RX_DATA           0x00000080
#define FMRDS_SEEK_T_SIG        0x00000100
#define FMRDS_CANCEL_T_SIG      0x00000400
#define FMRDS_TUNE_T_SIG        0x00000800
#define FMRDS_SOC_SIG_1         0x00001000

#define FMRDS_TASK_OFFLINE_SIG  TASK_OFFLINE_SIG      
#define FMRDS_TASK_STOP_SIG     TASK_STOP_SIG         
#define FMRDS_TASK_START_SIG    TASK_START_SIG   

#define FMRDS_COMMS_RD_T_SIG    0x00000080
#define FMRDS_COMMS_WR_T_SIG    0x00020000
#define FMRDS_SOC_SIG_2         0x00040000
#define QFMRDS_TRANSFER_SIG     0x00040000

/* F3 debug message definitions */
#define FMRDS_MSG_FATAL(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FM_RADIO, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define FMRDS_MSG_ERROR(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FM_RADIO, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define FMRDS_MSG_HIGH(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FM_RADIO, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define FMRDS_MSG_MED(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FM_RADIO, MSG_LEGACY_MED, x_fmt, a, b, c)

#define FMRDS_MSG_LOW(x_fmt, a, b, c) \
    MSG_SPRINTF_3 (MSG_SSID_FM_RADIO, MSG_LEGACY_LOW, x_fmt, a, b, c)


/* Constant definitions for arrays */
#define MAX_PS_INFO_CHARS           12
#define MAX_PS_STR_LEN              (96+1)
#define MAX_PS_INFO_CHAR_LEN        (8+1)
#define MAX_AF_LIST_VAL             24
#define MAX_RT_LEN                  (64+1)
//#define MAX_NO_OF_STATIONS          48
#define MAX_TX_PS_LEN               (64+1)
#define MAX_TX_RT_LEN               (64+1)
#define MAX_RDS_GRPS                21
#define MAX_NO_OF_TX_BKS_IN_GRP     3
#define MAX_NO_OF_RX_BKS_IN_GRP     4

/* =Search RSSI Level */
#define SEARCH_RSSI_LEVEL_WEAK      0x00   /* TBD */
#define SEARCH_RSSI_LEVEL_MID       0x01   /* TBD */
#define SEARCH_RSSI_LEVEL_STRONG    0x02   /* TBD */


/*** FM-RDS task semaphore definitions ***/
extern rex_crit_sect_type fmrds_crit_sect;

#define FMRDS_INIT_CRIT_SECT()      rex_init_crit_sect (&fmrds_crit_sect)
#define FMRDS_ENTER_CRIT_SECT()     rex_enter_crit_sect (&fmrds_crit_sect)
#define FMRDS_LEAVE_CRIT_SECT()     rex_leave_crit_sect (&fmrds_crit_sect)


/*----------------------------------------------------------------------------
 * Externalized Enum Definitions
 * -------------------------------------------------------------------------*/

/* =US/Europe or Japan FM Bands */
typedef enum {
   US_EUROPE_BAND      = 0,   // 87.5-108MHz
   JAPAN_WIDE_BAND     = 1,   // 76-108MHz
   JAPAN_STANDARD_BAND = 2,   // 76-90MHz
   USER_DEFINED_BAND   = 3
} teFmrdsBandLimitsType;

/* =Band  Limits: frequencies are in 10kHz steps */
#define US_EUROPE_BAND_MIN      8750      // 87.5 MHz
#define US_EUROPE_BAND_MAX      10800     // 108  MHz
#define JAPAN_BAND_MIN          7600      // 76   MHz
#define JAPAN_WIDE_BAND_MAX     10800     // 108  MHz
#define JAPAN_STND_BAND_MAX     9000      // 90   MHz

/* =De-emphasis Level */
typedef enum {
   DE_EMP75 = 0,
   DE_EMP50 = 1
} teFmrdsDtcTimeType;

/* =CHSPACE */
typedef enum {
   CHSPACE_200_KHZ     = 0,
   CHSPACE_100_KHZ     = 1,    
   CHSPACE_50_KHZ      = 2,    
   CHSPACE_RESERVED    = 3    
} teFmrdsChanSpacingType;

/* =The RDS standard we are running in */
typedef enum {
   RDS_STD_RBDS          = 0,
   RDS_STD_RDS           = 1,    
   RDS_STD_NONE          = 2
} teFmrdsRdsStd;

/* =Mute both Channels Left and Right */
typedef enum {    
   NOMUTE     = 0,
   MUTELEFT   = 1,
   MUTERIGHT  = 2,
   MUTEBOTH   = 3  
} teFmrdsMuteLftRgtType;

/* Is an FM Service Available */
typedef enum {
   SERVICE_NOT_AVAILABLE  = 0,
   SERVICE_AVAILABLE      = 1
} teFmrdsServAvbleType;

/* =Stereo/Mono Mode */
// these values determine the commanded stereo/mono state of the fm receiver
typedef enum {
   AUTO_MONO_STEREO_MODE  = 0,
   FORCE_MONO_ONLY_MODE   = 1
} teFmrdsStereoMonoModeType;

/* =Stereo Indicator Status for audio received from FM device */
// these values reflect the stereo/mono state of receiver operation
// if set to AUTO_MONO_STEREO_MODE. this will be station program
// and receiver decoding-specific.
typedef enum
{
   FM_MONO_PROGRAM     = 0,
   FM_STEREO_PROGRAM   = 1
}teFmrdsStereoProgramType;

/* =Normal/Low Power Mode */
typedef enum
{
   FMRDS_NORMAL_MODE       = 0,
   FMRDS_LOW_POWER_MODE    = 1
} teFmrdsPwrModeType;

/* =FM TX Power level */
typedef enum
{
   FMTX_POWER_LEVEL_0 = 0,
   FMTX_POWER_LEVEL_1,
   FMTX_POWER_LEVEL_2,
   FMTX_POWER_LEVEL_3,
   FMTX_POWER_LEVEL_4,
   FMTX_POWER_LEVEL_5,
   FMTX_POWER_LEVEL_6,
   FMTX_POWER_LEVEL_MAX
} teFmrdsTxPwrLevType;

/* =FM volume level */
typedef enum
{
   FM_VOLUME_LEVEL_0 = 0,
   FM_VOLUME_LEVEL_1,
   FM_VOLUME_LEVEL_2,
   FM_VOLUME_LEVEL_3,
   FM_VOLUME_LEVEL_4,
   FM_VOLUME_LEVEL_5,
   FM_VOLUME_LEVEL_6,
   FM_VOLUME_LEVEL_7,
   FM_VOLUME_LEVEL_8,
   FM_VOLUME_LEVEL_9,
   FM_VOLUME_LEVEL_10,
   FM_VOLUME_LEVEL_11,
   FM_VOLUME_LEVEL_12,
   FM_VOLUME_LEVEL_13,
   FM_VOLUME_LEVEL_14,
   FM_VOLUME_LEVEL_15,
   FM_VOLUME_LEVEL_MAX
} teFmrdsVolumeLevType;

/* =FM volume mode */
typedef enum
{
   FM_VOLUME_MODE_RX = 0,
   FM_VOLUME_MODE_TX
} teFmrdsVolumeModeType;

/* =Set volume level options */
typedef struct _tsFmrdsVolLevOptStruct
{
   teFmrdsVolumeModeType   volMode;
   teFmrdsVolumeLevType    volLev;
} tsFmrdsVolLevOptsType;

/* =FM event registration */
typedef enum
{
   /* 0 -> All events */ 
   FM_CB_EV_MASK_SEND_NONE = 0,    // return no events to client
   /* 1 -> Registration/Initialization Events (enable/disable/configure, etc) */
   FM_CB_EV_MASK_INIT,
   /* 2 -> FM status events (Tune/rssi, etc) */
   FM_CB_EV_MASK_STATUS,
   /* 3 -> FM control events (volume/pwr, etc) */
   FM_CB_EV_MASK_CONTROL,
   /* 4 -> FM search events (scan/seek, etc) */
   FM_CB_EV_MASK_SEARCH,
   /* 5 -> FMRDS RDS status events (rds lock, tx settings etc) */
   FM_CB_EV_MASK_RDS_STATUS,
   /* 6 -> FMRDS Rx processing events (Rds rx data events) */
   FM_CB_EV_MASK_RX_DATA,
   /* 7 -> FMRDS Tx processing events (rds tx data events) */
   FM_CB_EV_MASK_TX_DATA,

   FM_CB_EV_MASK_SEND_ALL    // return all events to client
} teFmrdsCbEvMaskType;

/* =Returns Tx/Rx mode set */
typedef enum {  
   RADIO_MODE_OFF       = 0,
   /* FM Receiver: FM+RDS Radio */
   RADIO_MODE_RX        = 1,
   /* FM Transmitter: MP3 Player to Car Stereo */
   RADIO_MODE_TX        = 2, 
   /* FM Transmitter/receiver: MP3 Player to Car Stereo */
   RADIO_MODE_TX_RX     = 3 
} teFmrdsRadioModeType;

/* =Search Direction */
typedef enum {
   SEARCHDIR_UP         = 0,
   SEARCHDIR_DOWN       = 1
} teFmrdsSrchDirType;

/* =Search Band Wrap */
typedef enum {
   NO_BAND_WRAP         = 0,
   WRAP_AT_END_OF_BAND  = 1
} teFmrdsSrchWrapType;

/* = Scantime  */
typedef enum {
   DWELL_PERIOD_1S     = 1,
   DWELL_PERIOD_2S     = 2,
   DWELL_PERIOD_3S     = 3,
   DWELL_PERIOD_4S     = 4,
   DWELL_PERIOD_5S     = 5,
   DWELL_PERIOD_6S     = 6,
   DWELL_PERIOD_7S     = 7
} teFmrdsScanTimeType;

/* =Search modes */
typedef enum
{
   /* Searches for next valid station */
   SEARCHMODE_SEEK            = 0,  
   /* Searches for next valid station and stays for specified time before searching next */
   SEARCHMODE_SCAN            = 1,  

   /* Searches the band for strong stations  */
   STATIONLIST_STRONG         = 9,  
   /* Searches the band for weak stations  */
   STATIONLIST_WEAK           = 10,  
   /* Scans for strongest stations */
   STATIONLIST_STRONGEST      = 2,  
   /* Scans for weakest (lowest signal strength) stations */
   STATIONLIST_WEAKEST        = 3,  
   /* Similar to Seek PTY except dwells on valid station before searching next */
   STATIONLIST_PTY            = 8,  

   /* Similar to Seek PTY except dwells on valid station before searching next */
   SEARCHMODE_RDS_SCAN_PTY   = 5,  
   /* Searches for next valid station that is broadcasting specified program type  */
   SEARCHMODE_RDS_SEEK_PTY   = 4,  
   /* Searches for next valid station that is broadcasting specified program ID */
   SEARCHMODE_RDS_SEEK_PI    = 6,   
   /* Searches for next valid station for alternate frequency  */
   SEARCHMODE_RDS_SEEK_AF    = 7  

} teFmrdsSrchModeType;

/* = Register/Deregister Fmrds events */
typedef enum
{
   FMRDS_EV_REGISTER    = 0,
   FMRDS_EV_DEREGISTER  = 1
} teFmrdsRegType;

/* = Tx Rds group control */
typedef enum
{
   /* Pauses RDS group transmission. */
   FMRDS_TX_RDS_GROUP_PAUSE  = 0, 
   /* Stops RDS group transmission and clears all stored groups. */
   FMRDS_TX_RDS_GROUP_CLEAR  = 1,  
   /* Resumes a paused RDS group transmission. */
   FMRDS_TX_RDS_GROUP_RESUME = 2  
} teFmrdsTxRdsGrpCtrlType;

/* =The FM Mode we are running in */
typedef enum {
   FM_OFF          = 0,
   FM_RX_ON        = 1,    
   FM_TX_ON        = 2,
   FM_RESET        = 3    
} teFmrdsFmPwrUpMode;

/* =The RDS Mode we are running in */
typedef enum {
   RDS_OFF          = 0,
   RDS_ON           = 1    
} teFmrdsRdsPwrUpMode;

/* =Returns Tx/Rx capability */
typedef enum {  
   RADIO_CAP_NONE  = 0, 
   RADIO_CAP_TX    = 1, 
   RADIO_CAP_RX    = 2, 
   RADIO_CAP_TX_RX = 3 
} teFmrdsRadioCapType;

/* =Error code events */
typedef enum
{
   FMRDS_ERR_HW_FAILURE       = 0, 
   FMRDS_ERR_COMMS_FAILURE    = 1, 
   FMRDS_ERR_TIMEOUT          = 2,
   FMRDS_ERR_CMD_NOT_ALLOWED  = 3
} teFmrdsErrEvType;

/* =FM-RDS queue - command queue status */
typedef enum
{
   FMRDS_CMD_Q_PENDING = 0,       /* Pending execution */
   FMRDS_CMD_Q_SUCCESS,           /* Executed successfully - legacy */
   FMRDS_CMD_Q_FULL,              /* Input queue full */
   FMRDS_CMD_Q_UNRECOGNIZED_CMD   /* Unrecognized or invalid */
} teFmrdsCmdQStatusType;

/* =Fmrds command messages */
typedef enum{

   FMRDS_NULL_COMMAND = 0,       // null command id

   FMRDS_ENABLE_FM, 
   FMRDS_DISABLE_FM,
   FMRDS_CFG_RADIO,
   FMRDS_SET_MUTE_MODE,
   FMRDS_SET_STEREO_MODE, 
   FMRDS_SET_RADIO_STATION, 
   FMRDS_GET_TUNE_STATUS,
   FMRDS_GET_STATION_PARAMETERS,
   FMRDS_GET_RDS_LOCK_STATUS,
   FMRDS_GET_RDS_PS_INFO,
   FMRDS_GET_RDS_RT_INFO,
   FMRDS_GET_RDS_AF_INFO,
   FMRDS_GET_RSSI_INFO,
   FMRDS_SET_PWR_MODE,
   FMRDS_SET_TX_PWR_LEVEL,
   FMRDS_GET_TX_SETTINGS,
   FMRDS_SET_RADIO_MODE,
   FMRDS_SEARCH_STATIONS, 
   FMRDS_RDS_SEARCH_STATIONS,
   FMRDS_SEARCH_STATION_LIST,
   FMRDS_CANCEL_SEARCH,
   FMRDS_SET_RDS_RX_PROCESSING_OPTIONS,
   FMRDS_REG_RDS_RX_GRPS_PROCESSING, 
   FMRDS_REG_PI_MATCH_PROCESSING, 
   FMRDS_REG_BLOCKB_PROCESSING, 
   FMRDS_SET_VOLUME_LEVEL,
   FMRDS_TX_PS_INFO,
   FMRDS_STOP_TX_PS_INFO,
   FMRDS_TX_RT_INFO,
   FMRDS_STOP_TX_RT_INFO,
   FMRDS_TX_RDS_GROUPS,
   FMRDS_TX_CONT_RDS_GROUPS,
   FMRDS_TX_RDS_GROUP_CTRL,
   FMRDS_REG_RTPLUS_CLS_PROCESSING,
   FMRDS_TX_RTPLUS_INFO,
#ifdef FEATURE_FM_SERVICE_AVAILABLE
   FMRDS_SERVICE_AVAILABLE,
#endif  // FEATURE_FM_SERVICE_AVAILABLE
   FMRDS_MAX_CMD
} teFmrdsCmdType;

/* =Fmrds event messages */
typedef enum {
   FMRDS_EV_ENABLE_FM,
   FMRDS_EV_DISABLE_FM,
   FMRDS_EV_CFG_RADIO,
   FMRDS_EV_MUTE_MODE_SET,
   FMRDS_EV_STEREO_MODE_SET,
   FMRDS_EV_RADIO_STATION_SET,
   FMRDS_EV_RADIO_TUNE_STATUS,            
   FMRDS_EV_STATION_PARAMETERS,
   FMRDS_EV_RDS_LOCK_STATUS,
   FMRDS_EV_RDS_PS_INFO,
   FMRDS_EV_RDS_RT_INFO,
   FMRDS_EV_RDS_AF_INFO,
   FMRDS_EV_RSSI_INFO,
   FMRDS_EV_PWR_MODE_SET,
   FMRDS_EV_TX_PWR_LEVEL_SET,
   FMRDS_EV_RDS_TX_SETTINGS,
   FMRDS_EV_RADIO_MODE_SET,
   FMRDS_EV_SCAN_IN_PROGRESS,
   FMRDS_EV_SEARCH_COMPLETE,
   FMRDS_EV_SEARCH_LIST_COMPLETE,
   FMRDS_EV_SEARCH_CANCELED,
   FMRDS_EV_RDS_RX_OPTIONS_SET,
   FMRDS_EV_RDS_RX_GRPS_REG_DONE, 
   FMRDS_EV_RAW_RDS_RBDS_DATA,
   FMRDS_EV_RDS_RX_PI_MATCH_REG_DONE,
   FMRDS_EV_RDS_RX_PI_MATCH_AVAILABLE,
   FMRDS_EV_RDS_RX_BLOCKB_REG_DONE,
   FMRDS_EV_RDS_RX_BLOCKB_AVAILABLE,
   FMRDS_EV_TX_PS_INFO_COMPLETE,
   FMRDS_EV_TX_STOP_PS_INFO_COMPLETE,
   FMRDS_EV_TX_RT_INFO_COMPLETE,
   FMRDS_EV_TX_STOP_RT_INFO_COMPLETE,
   FMRDS_EV_TX_RDS_GROUPS_COMPLETE,
   FMRDS_EV_TX_CONT_RDS_GROUPS_COMPLETE,
   FMRDS_EV_TX_RDS_GROUP_CTRL_COMPLETE,
   FMRDS_EV_RX_RTPLUS_CLS_REG_DONE,
   FMRDS_EV_TX_RTPLUS_INFO_COMPLETE,
   FMRDS_EV_RX_VOLUME_LEVEL_SET,
   FMRDS_EV_TX_VOLUME_LEVEL_SET,
   FMRDS_EV_RAW_RDS_RBDS_BLOCK_DATA,    // return raw rds block data

#ifdef FEATURE_FM_SERVICE_AVAILABLE
   FMRDS_EV_SERVICE_AVAILABLE,
#endif  // FEATURE_FM_SERVICE_AVAILABLE

   FMRDS_EV_MAX_EVENT
} teFmrdsEventType;

/* =FMRDS event result */
typedef enum {    
   FMRDS_EV_SUCCESS  = 0,
   FMRDS_EV_FAILURE  = 1
} teFmrdsEvResultType;

/* =Sychronized? */
typedef enum {    
    NO_RDS_SYNC   = 0,
    RDS_SYNC      = 1
} teFmrdsSyncStateChangeType;

/* =Sychronized? */
typedef enum {    
    NO_SCAN_NEXT     = 0,
    SCAN_NEXT        = 1
} teFmrdsScanNextType;

/* =Block ID Truth Table */
/* =Enums below apply to *raw* rds operation only - some duplication with driver layer */
/* =Raw rds block ids */
typedef enum {
   FMRDS_NO_DECODE            = 0,
   FMRDS_BLOCK_A		          = 1,
   FMRDS_BLOCK_B		          = 2,
   FMRDS_BLOCK_C		          = 3,
   FMRDS_BLOCK_C_DASH	        = 4,
   FMRDS_BLOCK_D		          = 5,
   FMRDS_BLOCK_E              = 6,
   FMRDS_BLOCK_UNDEFINED      = 7,
   FMRDS_BLOCK_INVAL_E_RBDS   = 8,
   FMRDS_BLOCK_INVALID        = 9
} teFmrdsBlkIdTpye; 

/* =RDS error threshold settings */
/* =Values generic, likely will need to be mapped to device in use */
/* If no error in the block use RDS_ERRORS_NO_ERRORS_DETECTED */
typedef enum {
   RDS_ERRORS_UNCORRECTABLE         = 0,
   RDS_ERRORS_NO_ERRORS_CORRECTED   = 8,
   RDS_ERRORS_NO_ERRORS             = 15,
   RDS_ERRORS_MAX_1_BITS            = 9,
   RDS_ERRORS_MAX_2_BITS            = 10,
   RDS_ERRORS_MAX_3_BITS            = 11,
   RDS_ERRORS_MAX_4_BITS            = 12,
   RDS_ERRORS_MAX_5_BITS            = 13,
   RDS_ERRORS_UNDEFINED_ERROR       = 14,
   RDS_ERRORS_NO_ERRORS_DETECTED    = 15
} teFmrdsErrLevType; 

/* =Program types for RDS mode */
typedef enum {
   NONE = 0,
   NEWS,
   CURRENT_AFFAIRS,
   INFORMATION,
   SPORT,
   EDUCATION,
   DRAMA,
   CULTURE,
   SCIENCE,
   VARIED,
   POP_MUSIC,
   ROCK_MUSIC,
   MOR_MUSIC,
   LIGHT_CLASSICAL,
   SERIOUS_CLASSICAL,
   OTHER_MUSIC,
   WEATHER,
   FINANCE,
   CHILDRENS_PROGRAM,
   SOCIAL_AFFAIRS,
   RELIGION,
   PHONE_IN,
   TRAVEL,
   LEISURE,
   JAZZ_MUSIC,
   COUNTRY_MUSIC,
   NATIONAL_MUSIC,
   OLDIES_MUSIC,
   FOLK_MUSIC,
   DOCUMENTARY,
   ALARM_TEST,
   ALARM
} teFmrdsRdsPrgmType;

/* =Program types for RBDS mode */
typedef enum {
   _NONE = 0,
   _NEWS,
   _INFORMATION,
   _SPORTS,
   _TALK,
   _ROCK,
   _CLASSIC_ROCK,
   _ADULT_HITS,
   _SOFT_ROCK,
   _TOP40,
   _COUNTRY,
   _OLDIES,
   _SOFT,
   _NOSTALGIA,
   _JAZZ,
   _CLASSICAL,
   _RHYTHM_AND_BLUES,
   _SOFT_RHYTHM_AND_BLUES,
   _LANGUAGE,
   _RELIGIOUS_MUSIC,
   _RELIGIOUS_TALK,
   _PERSONALITY,
   _PUBLIC,
   _COLLEGE,
   _UNASSIGNED1,
   _UNASSIGNED2,
   _UNASSIGNED3,
   _UNASSIGNED4,
   _UNASSIGNED5,
   _WEATHER,
   _EMERGENCY_TEST,
   _EMERGENCY
} teFmrdsRbdsPrgmType; 

typedef enum 
{
    GROUP_TYPE_INVALID = -1,
    GROUP_0A = 0,
    GROUP_0B,
    GROUP_1A,
    GROUP_1B,
    GROUP_2A,
    GROUP_2B,
    GROUP_3A,
    GROUP_3B,
    GROUP_4A,
    GROUP_4B,
    GROUP_5A,
    GROUP_5B,
    GROUP_6A,
    GROUP_6B,
    GROUP_7A,
    GROUP_7B,
    GROUP_8A,
    GROUP_8B,
    GROUP_9A,
    GROUP_9B,
    GROUP_10A,
    GROUP_10B,
    GROUP_11A,
    GROUP_11B,
    GROUP_12A,
    GROUP_12B,
    GROUP_13A,
    GROUP_13B,
    GROUP_14A,
    GROUP_14B,
    GROUP_15A,
    GROUP_15B
} teFmrdsGroupType; 


/* =RTPlus classes */
typedef enum {
   ITEM_TITLE	=0,
   ITEM_ALBUM,
   ITEM_TRACKNUMBER,
   ITEM_ARTIST,
   ITEM_COMPOSITION,
   ITEM_MOVEMENT,
   ITEM_CONDUCTOR,
   ITEM_COMPOSER,
   ITEM_BAND,
   ITEM_COMMENT,
   ITEM_GENRE,
   INFO_NEWS,
   INFO_NEWS_LOCAL,
   INFO_STOCKMARKET,
   INFO_SPORT,
   INFO_LOTTERY,
   INFO_HOROSCOPE,
   INFO_DAILY_DIVERSION,
   INFO_HEALTH,
   INFO_EVENT,
   INFO_SZENE,
   INFO_CINEMA,
   INFO_TV,
   INFO_DATE_TIME,
   INFO_WEATHER,
   INFO_ALARM,
   INFO_ADVERTISEMENT,
   INFO_OTHER,
   STATIONNAME_LONG,
   PROGRAM_NOW,
   PROGRAM_NEXT,
   PROGRAM_PART,
   PROGRAM_HOST,
   PROGRAM_EDITORIAL_STAFF,
   PROGRAM_RADIO,
   PROGRAM_HOMEPAGE,
   PHONE_HOTLINE,
   PHONE_STUDIO,
   PHONE_OTHER,
   SMS_STUDIO,
   SMS_OTHER,
   EMAIL_HOTLINE,
   EMAIL_STUDIO,
   EMAIL_OTHER,
   MMS_OTHER,
   CHAT,
   CHAT_CENTER,
   VOTE_QUESTION,
   VOTE_CENTER,
   OPEN_CLASS_1,
   OPEN_CLASS_2,
   OPEN_CLASS_3,
   OPEN_CLASS_4,
   OPEN_CLASS_5,
   OPEN_CLASS_6,
   OPEN_CLASS_7,
   OPEN_CLASS_8,
   OPEN_CLASS_9,
   PLACE,
   APPOINTMENT,
   HOTLINE,
   IDENTIFIER,
   PURCHASE,
   GET_DATA
} teFmrdsRTPlusClassType;

/* =ODA xA used for RTPlus */
typedef enum {
   ODA_GROUP_5A = 0,
   ODA_GROUP_6A,
   ODA_GROUP_7A,
   ODA_GROUP_8A,
   ODA_GROUP_9A,
   ODA_GROUP_11A,
   ODA_GROUP_12A,
   ODA_GROUP_13A
} teFmrdsRdsGrpOdaxAType;

typedef uint16 teFmrdsPrgmIdType;


/*----------------------------------------------------------------------------
 * Externalized Structure Definitions
 * -------------------------------------------------------------------------*/

#define US_GENERIC_NORBDS            {US_EUROPE_BAND,    DE_EMP75, CHSPACE_200_KHZ, RDS_STD_NONE, {0, 0}}
#define US_GENERIC_RBDS              {US_EUROPE_BAND,    DE_EMP75, CHSPACE_200_KHZ, RDS_STD_RBDS, {0, 0}}
#define EUROPE_GENERIC_NORDS         {US_EUROPE_BAND,    DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE, {0, 0}}
#define EUROPE_GENERIC_RDS           {US_EUROPE_BAND,    DE_EMP50, CHSPACE_100_KHZ, RDS_STD_RDS, {0, 0}}
#define JAPAN_GENERIC_NORDS          {JAPAN_STANDARD_BAND, DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE, {0, 0}}
#define JAPAN_WIDE_GENERIC_NORDS     {JAPAN_WIDE_BAND,     DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE, {0, 0}}
#define INDIA_GENERIC_NORDS          {USER_DEFINED_BAND, DE_EMP50, CHSPACE_100_KHZ, RDS_STD_NONE, {9100, 10640}}
#define INDIA_GENERIC_RDS            {USER_DEFINED_BAND, DE_EMP50, CHSPACE_100_KHZ, RDS_STD_RDS, {9100, 10640}}

/* =FMRDS Band Range */
typedef PACKED struct _tsFmrdsBandRangeStruct
{
   uint16 lowerLimit;
   uint16 upperLimit;
} tsFmrdsBandRangeType;

/* =Fmrds Radio settings */
typedef PACKED struct _tsFmrdsRadioSettingsStruct
{
   teFmrdsBandLimitsType   radioBand;
   teFmrdsDtcTimeType      deEmphasis;
   teFmrdsChanSpacingType  chSpacing;
   teFmrdsRdsStd           rdsStd;
   tsFmrdsBandRangeType    bandRange;
} tsFmrdsRadioSettingsType;

/* =Select on the fly between RDS & RBDS mode */
typedef PACKED struct _tsFmrdsPrgmStruct
{
   PACKED union {
      teFmrdsRdsPrgmType    rdsPrgmType;
      teFmrdsRbdsPrgmType   rbdsPrgmType;
   } rdsOrRbdsMode;
} tsFmrdsPrgmType;

/* =Basic radio search options */
typedef PACKED struct _tsFmrdsRadioSrchModeStruct
{
   teFmrdsSrchModeType     srchMode;
   teFmrdsScanTimeType     scanTime;
   teFmrdsSrchDirType      srchDir;
   teFmrdsSrchWrapType     srchWrap;
   uint8                   srchRssiLev;
} tsFmrdsRadioSrchModeType;

/* =RDS radio search options */
typedef PACKED struct _tsFmrdsRdsRadioSrchModeStruct
{
   teFmrdsSrchModeType     rdsSrchMode;
   teFmrdsSrchDirType      srchDir;
   teFmrdsScanTimeType     scanTime;
   tsFmrdsPrgmType         srchPty;
   teFmrdsPrgmIdType       srchPI;
   uint8                   srchRssiLev;
} tsFmrdsRdsRadioSrchModeType;

/* =Radio station list options */
typedef PACKED struct _tsFmrdsRadioStationListOptsStruct
{
   teFmrdsSrchModeType     stationListMode;
   teFmrdsSrchDirType      srchDir; 
   uint8                   srchListMax;
   tsFmrdsPrgmType         srchPty;
   uint8                   srchRssiLev;
} tsFmrdsRadioStationListOptsType;

/* =PI match processing options */
typedef PACKED struct _tsFmrdsRdsRxPIMtchProcOptsStruct
{
   teFmrdsRegType    regPIMtchOpt;
   teFmrdsPrgmIdType prgmId;
} tsFmrdsRdsRxPIMtchProcOptsType;

/* =BlockB match processing options */
typedef PACKED struct _tsFmrdsRdsRxBlkBMtchProcOptsStruct
{
   teFmrdsRegType regRdsBlkBMtchOpt;
   uint16         usMtchCond;
   uint16         usMtchMsk;
} tsFmrdsRdsRxBlkBMtchProcOptsType;

/* =TX PS transmit options */
typedef PACKED struct _tsFmrdsTxPSStationInfoStruct
{
   char              txPSStr[MAX_TX_PS_LEN];
   uint8             txPSRptCnt;
   tsFmrdsPrgmType   txPSPty; 
   uint8             noOfTxAFs;
   uint16            txAFList[MAX_AF_LIST_VAL];   /* Array of AF's */
} tsFmrdsTxPSStationInfoType;

/* =TX RT transmit options */
typedef struct _FmrdsTxRTStationInfoStruct
{
   char              txRTStr[MAX_TX_RT_LEN];
   tsFmrdsPrgmType   txRTPty; 
} tsFmrdsTxRTStationInfoType;

/* =TX RT transmit options */
typedef struct _FmrdsTxRTPlusTagStruct
{
   teFmrdsRTPlusClassType   txRTPlusCont;
   uint8                   txStartMarker;
   uint8                   txLenMarker;
} tsFmrdsTxRTPlusTagType;

/* =TX RT transmit options */
typedef struct _FmrdsTxRTPlusStationInfoStruct
{
   char                    txRTPlusStr[MAX_TX_RT_LEN];
   tsFmrdsPrgmType         txRTPlusPty; 
   tsFmrdsTxRTPlusTagType  txTag[2];
   teFmrdsRdsGrpOdaxAType  appGrpTypeCode;
   boolean                 newItem;
   boolean                 clearCmd;
} tsFmrdsTxRTPlusStationInfoType;

/* =TX RDS group transmit options */
typedef struct _tsFmrdsTxRdsGrpStruct {
   /* Block ID of RDS2/RDS3/RDS4 Block */
   /* No need for App to transmit group A */
   teFmrdsBlkIdTpye      rdsBlkId[MAX_NO_OF_TX_BKS_IN_GRP];        
   uint16               rdsWord[MAX_NO_OF_TX_BKS_IN_GRP];
} tsFmrdsTxRdsGrpType;

/* =Single data group (Used in RAW data evnet to APP) */ 
typedef struct _tsFmrdsGrpDataStruct {
	teFmrdsErrLevType   rdsBlkErrStat[MAX_NO_OF_RX_BKS_IN_GRP]; 
    teFmrdsBlkIdTpye     rdsBlkId[MAX_NO_OF_RX_BKS_IN_GRP];        
    uint16              rdsWord[MAX_NO_OF_RX_BKS_IN_GRP];
} tsFmrdsGrpDataType;

/* =Raw data block buffer definition, as passed in event message */ 
typedef struct _tsFmrdsRawRdsDataStruct {
    uint16              noOfRdsGrps;     
    tsFmrdsGrpDataType  rdsGrps[MAX_RDS_GRPS];      
} tsFmrdsRawRdsDataType;

/* =TX "One shot" transmit groups options */
typedef struct _tsFmrdsTxOneShotRdsGrpsStruct
{
   tsFmrdsTxRdsGrpType* psTxRdsGrps;
   uint8                txNoOfRdsGrpsToTx; 
   uint8*               txNoOfRdsGrpsProcessed;
} tsFmrdsTxOneShotRdsGrpsType;

/* =TX "Continuous" transmit groups options */
typedef struct _tsFmrdsTxContRdsGrpsStruct
{
   tsFmrdsTxRdsGrpType* psTxRdsGrps;
   uint8                txNoOfRdsGrpsToTx; 
   uint8*               txNoOfRdsGrpsProcessed;
} tsFmrdsTxContRdsGrpsType;

/* =Rx processing options */
typedef PACKED struct _tsFmrdsRxProcOptStruct
{
  uint32  enRdsGrps;
  uint8   rdsBufSz;
  boolean enRdsChangeFiltr; 
  boolean enPSUncrtbleBlks;
  boolean enRdsBlkEProc; 
  boolean enRdsAutoAF;
  boolean rdsReturnRawBlocks;
} tsFmrdsRxProcOptType;

/* =RT Plus class processing options */
typedef struct _tsFmrdsRTPlusClassProcOptStruct
{
   uint32 lsbClassMsk;
   uint32 msbClassMsk;
} tsFmrdsRTPlusClassProcOptType;

/* =FMRDS message header */
typedef struct _FmrdsCmdHdrStruct
{
   q_link_type              fmrdsLink;                  
   teFmrdsCmdType           fmrdsCmdType;          
//   teFmrdsCmdQStatusType    fmrdsCmdQStatus;  // legacy: no longer used       
} tsFmrdsCmdHdrType;

/* =FMRDS command message body - parameters passed */
typedef struct  _tsFmrdsCmdMsgBodyStruct
{
  union
  {
    /* To Enable FM and configure FM radio */
    tsFmrdsRadioSettingsType           fmrdsRadioSettings; 
    /* Mute control */
    teFmrdsMuteLftRgtType              fmrdsMuteCtrl;   
    /* Mono/Stereo mode */
    teFmrdsStereoMonoModeType          fmrdsStereoMonoMode;  
    /* Channel frequency */                                            
    uint16                             fmrdsFreq;      
    /* Set operating power mode */
    teFmrdsPwrModeType                 fmrdsPwrMode;
    /* Set transmit power level */
    teFmrdsTxPwrLevType                fmrdsTxPwrLev;
    /* Set radio mode */
    teFmrdsRadioModeType               fmrdsRadioMode;
    /* Set basic search mode */
    teFmrdsSrchModeType                fmrdsSrchMode;
    /* Radio search mode */
    tsFmrdsRadioSrchModeType           fmrdsRadioSrchMode;
    /* Rds radio search mode */
    tsFmrdsRdsRadioSrchModeType        fmrdsRdsRadioSrchMode;
    /* Radio station list options */
    tsFmrdsRadioStationListOptsType    fmrdsStationListOpts;
    /* Rds Rx processing options */
    tsFmrdsRxProcOptType               fmrdsRxProcOpt;
    /* Rds Rx group processing */
    uint32                             ulFmrdsGrpsToProc;
    /* Register for PI match option */
    tsFmrdsRdsRxPIMtchProcOptsType     fmrdsRdsPIMtchProcOpts;
    /* Register for BlockB match option */
    tsFmrdsRdsRxBlkBMtchProcOptsType   fmrdsRdsBlkBMtchProcOpts;
    /* Set Tx Pi type */
    teFmrdsPrgmIdType                  fmrdsTxPI;
    /* Transmit program service information */
    tsFmrdsTxPSStationInfoType         fmrdsTxPSStationInfoOpts;
    /* Transmit radio text information */
    tsFmrdsTxRTStationInfoType         fmrdsTxRTStationInfoOpts;
    /* Transmit "One Shot" groups */
    tsFmrdsTxOneShotRdsGrpsType        fmrdsTxOneShotRdsGrpsOpts;
    /* Transmit "Continuous" groups */
    tsFmrdsTxContRdsGrpsType           fmrdsTxContRdsGrpsOpts;
    /* Transmit Rds group control */
    teFmrdsTxRdsGrpCtrlType            fmrdsTxRdsGrpCtrlOpt;
    /* RTPlus class processing options */
    tsFmrdsRTPlusClassProcOptType      fmrdsRTPlusClassProcOpts;
    /* RT Plus station info */
    tsFmrdsTxRTPlusStationInfoType     fmrdsTxRTPlusStationInfo;
    /* Volume level options */
    tsFmrdsVolLevOptsType              fmrdsVolLevOpts;
#ifdef FEATURE_FM_SERVICE_AVAILABLE
    // service available rssi threshold
    uint8                              fmrdsServiceRssiLev;
#endif  // FEATURE_FM_SERVICE_AVAILABLE
  } tuCmdParams;
} tsFmrdsCmdMsgBodyType;

/* =FMRDS Command Message structure */
typedef struct _tsFmrdsCmdMsgType
{
   tsFmrdsCmdHdrType        fmrdsCmdHdr;       /* Header */
   tsFmrdsCmdMsgBodyType    fmrdsCmdMsgBody;   /* Message body */
} tsFmrdsCmdMsgType;

/* Event Results */
/* =Enable FM Results */
typedef struct _tsFmrdsEnableFmEvStruct
{
   /* Radio settings */
   tsFmrdsRadioSettingsType   radioSettings; 
   /* Set radio mode */
   teFmrdsRadioModeType       radioMode;
   /* FM capability */
   teFmrdsRadioCapType        radioCap;
   /* FM Pwr Mode */
   teFmrdsFmPwrUpMode         fmPwrUpMode;
   /* RDS Pwr Mode */
   teFmrdsRdsPwrUpMode        rdsPwrUpMode;
} tsFmrdsEnableFmEvType;

/* =Disable FM Results */
typedef struct _tsFmrdsDisableFmEvStruct
{
   /* Radio settings */
   tsFmrdsRadioSettingsType   radioSettings; 
   /* Set radio mode */
   teFmrdsRadioModeType       radioMode;
   /* FM Pwr Mode */
   teFmrdsFmPwrUpMode         fmPwrUpMode;
   /* RDS Pwr Mode */
   teFmrdsRdsPwrUpMode        rdsPwrUpMode;
} tsFmrdsDisableFmEvType;

/* =Configure FM Radio Results */
typedef struct _tsFmrdsConfigRadioEvStruct
{
   /* Radio settings in response to configure radio */
   tsFmrdsRadioSettingsType   radioSettings;
} tsFmrdsConfigRadioEvType;

/* =Mute mode event */
typedef struct _tsFmrdsMuteModeEvStruct 
{
   /* This is the current mute status(audio channel control) */
   teFmrdsMuteLftRgtType      muteMode;     
} tsFmrdsMuteModeEvType;

/* Rx Volume Level event */
typedef struct _tsFmrdsVolLevEvStruct 
{
teFmrdsVolumeLevType      fmrdsVolLev;
} tsFmrdsVolLevEvType;

/* =Mono stereo mode event */
typedef struct _tsFmrdsStereoMonoModeEvStruct 
{
   /* Mono/Stereo mode */
   teFmrdsStereoMonoModeType  monoStereo;   
} tsFmrdsStereoMonoModeEvType;

/* =Radio station event */
typedef struct _tsFmrdsRadioStationSetEvStruct 
{         
   /* Station radio LOCKED to */
   /* Channel frequency - response to radio station set */                                            
   uint16                  stationFreq;   
} tsFmrdsRadioStationSetEvType;

/* =Radio tune status event */
typedef struct _tsFmrdsRadioTuneStatusEvStruct 
{         
  /* Station radio LOCKED to */
  uint16                     stationFreq;
  /* Averaged received signal strength measured at the antenna.*/
  uint8                      rssi;   
  // stereo/mono indicator for audio received from fmrds device.
  // use this for the fm application's stereo indicator.
  teFmrdsStereoProgramType   stereoProgram;   
} tsFmrdsRadioTuneStatusEvType;

/* =Service parameteres available event */
typedef struct _tsFmrdsParamsAvbleEvStruct {
  /* Radio Frequency in MHz with 0.01MHz step size */
  uint16                        stationFreq;     
  teFmrdsSyncStateChangeType    rdsSyncStatus;
  /* This is the current mute status(audio channel control) */
  teFmrdsMuteLftRgtType         muteMode;     
  /* Averaged received signal strength measured at the antenna.*/
  uint8                         rssi; 
  /* Measured by the Interference Detector as the frequency separation from the main signal.*/
  uint8                         ifcnt;  
  // stereo/mono indicator for audio received from fmrds device.
  // use this for the fm application's stereo indicator.
  teFmrdsStereoProgramType      stereoProgram;   
} tsFmrdsParamsAvbleEvType;

/* =RDS lock status event */
typedef struct _FmrdsRdsLockStatusEvStruct {         
    /* Station radio LOCKED to */
    uint16                    stationFreq;
    /* Averaged received signal strength measured at the antenna.*/
    uint8                     rssi;  
    /* RDS capabality of the station locked */
    boolean                   rdsCapability;
} tsFmrdsRdsLockStatusEvType;

/* =PS summary event */
typedef struct _tsFmrdsPSSummaryEvStruct {
    /* Program service name new/repeat flags */
    uint8                     psNum;       
    /* 16 bit unique program id */
    teFmrdsPrgmIdType         piPrgmId;        
    /* Program type fields: differ in rds and rbds */
    tsFmrdsPrgmType           ptyPrgmType;     
    /* Traffic program code */
    boolean                   taPrgmCodeFlag;     
    /* Traffic announcement code */
    boolean                   taAnnouncementCodeFlag;
    /* Music speech switch code */
    boolean                   msSwitchCodeFlag;     
    /* Decoder Identification control code */
    boolean                   decIdCtrlCodeFlag;
    /* Program service name characters */
    char                      psText[MAX_PS_INFO_CHARS][MAX_PS_INFO_CHAR_LEN];  
    /* Complete PS string */
    char                      *pCmptPSText;   // nb passed as string
} tsFmrdsPSSummaryEvType;

/* =RT summary event */
typedef struct _tsFmrdsRTSummaryEvStruct {
    /* Length of the Radio text string */
    uint8                     length;         
    /* 16 bit unique program id */ 
    teFmrdsPrgmIdType         piPrgmId;    
    /* Program type fields: differ in rds and rbds */
    tsFmrdsPrgmType           ptyPrgmType;     
    /* Traffic program code */
    boolean                   taPrgmCodeFlag; 
    /* Text A/B flag */
    boolean                   textAbFlag;      
    /* Null-terminated 64 char radio text string */
    char                      *pRadioText;   // nb passed as string       
} tsFmrdsRTSummaryEvType;

/* =AF summary event */
typedef struct _tsFmrdsRdsAFSummaryEvStruct {
    /* Radio Frequency in MHz to 0.01MHz */
    uint16                    radioFreq;	    
    /* Number of valid AF in the list */
    uint8                     noOfAFInList;   
    /* 16 bit unique program id */
    uint16                    piPrgmId;         
    /* Array of AF's */
    uint16                    AFFreqList[MAX_AF_LIST_VAL];
} tsFmrdsRdsAFSummaryEvType;

/* =RSSI Info summary event */
typedef struct _tsFmrdsRssiInfoEvStruct {
    /* RSSI – Received Signal Strength Indication Description */
    /* Averaged received signal strength measured at the antenna.*/
    uint8 rssi; 
    /* IFCNT – Interference Count Description */
    /* Measured by the Interference Detector as the frequency separation from the main signal.*/
    uint8 ifcnt;  
} tsFmrdsRssiInfoEvType;

/* =Power mode event */
typedef struct _tsFmrdsPwrModeEvStruct {
   /* Operating power mode set */
   teFmrdsPwrModeType              pwrMode;
} tsFmrdsPwrModeEvType;

/* =Tx Power level mode event */
typedef struct _tsFmrdsTxPwrLevModeEvStruct {
   /* Transmit power level set*/
   teFmrdsTxPwrLevType           txPwrLev;
} tsFmrdsTxPwrLevModeEvType;

/* =Radio mode event */
typedef struct _tsFmrdsRadioModeEvStruct {
   /* Set radio mode */
   teFmrdsRadioModeType            radioMode;
} tsFmrdsRadioModeEvType;

/* =Tx settings event */
typedef struct _tsFmrdsTxSettingsEvStruct {
   /* Tx settings */
   /*
   -------------------------------------------------------------------------
   BIT      0  1  2
   TX SRVC  PS RT AF
   -------------------------------------------------------------------------
   */
   uint32                        txSettings;
} tsFmrdsTxSettingsEvType;

/* =Scan next event */
typedef struct _tsFmrdsScanNextEvStruct {
   /* Scan next event */
   teFmrdsScanNextType           scanNext;
} tsFmrdsScanNextEvType;

/* =Search complete event */
typedef struct _tsFmrdsSrchCompleteEvStruct
{
   teFmrdsServAvbleType   serviceAvble;  // indicates fm activity in band found
} tsFmrdsSrchCompleteEvType;

/* =Station list event */
typedef struct _tsFmrdsSrchStationListEvStruct
{          
   /* Number of stations tuned to in band above SSL */
   uint32                        noOfStationsFound; 
   /* Pointer to list of stations radio LOCKED to */
   uint16                        *pStationList;            
   /* RDS capability for each station */
   boolean                       *pRdsCapability;
} tsFmrdsSrchStationListEvType;

/* =Rx processing options event */
typedef struct _tsFmrdsRxProcOptEvStruct {
   /* Rds Rx processing options */
   tsFmrdsRxProcOptType                rxProcOpt;
} tsFmrdsRxProcOptEvType;


/* =RAW RDS/RBDS group events */
// events return either decoded group data or raw rds blocks
// holds block data decoded into previously-specified groups
typedef struct _tsFmrdsRawRdsGrpsEvStruct {
      /* Rds Rx group processing */
      tsFmrdsRawRdsDataType            rawRdsGrps;
} tsFmrdsRawRdsGrpsEvType;

// event to pass completely raw, error-checked block data
// for external decoding. there is no element of group
// decoding in this data - this must be performed externally.
typedef struct _tsRawBlockData {
  uint16  usBlockA;
  uint16  usBlockB;
  uint16  usBlockC;
  uint16  usBlockD;
} tsRawBlockData;   // element holding four blocks = one rds group

typedef struct _tsFmrdsRawRdsBlockEvStruct {
  uint16          usRawBlockBuffSize;   // number of (blocks * 4 = undecoded group) in buffer       
  tsRawBlockData  *psRawBlockBuffPtr;   // pointer to block * 4 buffer
} tsFmrdsRawRdsBlockEvType;


/* =PI match processing set event */
typedef struct _tsFmrdsRdsRxPIMtchProcOptsEvStruct {
      /* Register for PI match option */
      tsFmrdsRdsRxPIMtchProcOptsType   rdsPIMtchProcOpts;
} tsFmrdsRdsRxPIMtchProcOptsEvType;

/* =PI match groups processing event */
typedef struct _tsFmrdsPIMtchGrpEvStruct {
      /* PI match available */
      tsFmrdsGrpDataType               piMtchGrp;
} tsFmrdsPIMtchGrpEvType;

/* =BlockB match processing set event */
typedef struct _tsFmrdsRdsRxBlkBMtchProcOptsEvStruct {
      /* Register for BlockB match option */
      tsFmrdsRdsRxBlkBMtchProcOptsType  rdsBlkBMtchProcOpts;
} tsFmrdsRdsRxBlkBMtchProcOptsEvType;

/* =BlockB match groups processing event */
typedef struct _tsFmrdsBlkBMtchGrpEvStruct {
      /* BlockB match available */
      tsFmrdsGrpDataType               bkBMtchGrp;
} tsFmrdsBlkBMtchGrpEvType;

/* =Tx RDS "one-shot" groups complete event */
typedef struct _tsFmrdsTxRdsGrpsCmptEvStruct
{
   uint8  noOfRdsGrpsTx; 
   uint8  noOfRdsGrpsFree; 
} tsFmrdsTxRdsGrpsCmptEvType;

/* =Tx RDS "continuous" groups complete event */
typedef struct _tsFmrdsTxContRdsGrpsCmptEvStruct
{
   uint8  noOfRdsGrpsTx; 
} tsFmrdsTxContRdsGrpsCmptEvType;

/* =Tx RDS group control event */
typedef struct _tsFmrdsTxRdsGrpCtrlOptEvStruct {
      /* Transmit Rds group control */
      teFmrdsTxRdsGrpCtrlType          txRdsGrpCtrlOptEv;
} tsFmrdsTxRdsGrpCtrlOptEvType;

/* =Error event */
typedef struct _tsFmrdsErrEvStruct
{
   /* Error event */
   teFmrdsErrEvType           errCode;
} tsFmrdsErrEvType;

/* =Set volume level event */
typedef struct _tsFmrdsVolLevSetEvStruct
{
   teFmrdsVolumeLevType           volLev;
} tsFmrdsVolLevSetEvType;

/* =RTPlus class registration done event */
typedef struct _tsFmrdsRTPlusClassRegEvStruct
{
   uint32   lsbClassMsk;
   uint32   msbClassMsk;
} tsFmrdsRTPlusClassRegEvType;

/* =RTPlus Info event */
typedef struct _tsFmrdsRTPlusInfoEvStruct
{
   boolean                 newItem;
   boolean                 clearCmd;
   /* Null-terminated max 64 char rtplus text string */
   char                    xmlRTPlusTag1[MAX_RT_LEN];  
   /* Null-terminated max 64 char rtplus text string */
   char                    xmlRTPlusTag2[MAX_RT_LEN];  
} tsFmrdsRTPlusInfoEvType;

#ifdef FEATURE_FM_SERVICE_AVAILABLE
// service available event type
typedef struct _tsFmrdsServiceAvailableEvStruct
{
   teFmrdsServAvbleType  serviceAvailable;
} tsFmrdsServiceAvailableEvType;
#endif  // FEATURE_FM_SERVICE_AVAILABLE


/******************* FM RDS Specific Type *********/
/* Structure to define frequently used Parameters */
/**************************************************/
/* =FMRDS Event body */
typedef struct  _tsFmrdsEvMsgBodyType
{
   union {
      /* Enable FM radio */
      tsFmrdsEnableFmEvType               fmrdsEnableFmEv;
      /* Disable FM radio */
      tsFmrdsDisableFmEvType              fmrdsDisableFmEv;
      /* Radio settings in response to configure radio */
      tsFmrdsConfigRadioEvType            fmrdsConfigRadioEv; 
      /* Mute control */
      tsFmrdsMuteModeEvType               fmrdsMuteCtrlEv; 
      /* Volume Level */
      tsFmrdsVolLevEvType                 fmrdsVolLevEv;
      /* Mono/Stereo mode */
      tsFmrdsStereoMonoModeEvType         fmrdsStereoMonoModeEv;
      /* Channel frequency - response to radio station set */    
      tsFmrdsRadioStationSetEvType        fmrdsStationSetEv;
      /* Radio tune status */
      tsFmrdsRadioTuneStatusEvType        fmrdsTuneStatusEv;
      /* Service/Parameter Available */
      tsFmrdsParamsAvbleEvType            fmrdsParamAvbleEv;
      /* Radio rds lock status */
      tsFmrdsRdsLockStatusEvType          fmrdsRdsLockStatusEv;
      /* PS list summary */
      tsFmrdsPSSummaryEvType              fmrdsPSSummaryEv;
      /* RT list summary */
      tsFmrdsRTSummaryEvType              fmrdsRTSummaryEv;
      /* AF list summary */
      tsFmrdsRdsAFSummaryEvType           fmrdsAFSummaryEv;
      /* Rssi Info */
      tsFmrdsRssiInfoEvType               fmrdsRssiInfoEv;
      /* Operating power mode set */
      tsFmrdsPwrModeEvType                fmrdsPwrModeEv;
      /* Transmit power level set*/
      tsFmrdsTxPwrLevModeEvType           fmrdsTxPwrLevEv;
      /* Set radio mode */
      tsFmrdsRadioModeEvType              fmrdsRadioModeEv;
      /* Tx settings */
      tsFmrdsTxSettingsEvType             fmrdsTxSettingsEv;
      /* Scan next event */
      tsFmrdsScanNextEvType               fmrdsScanNextEv;
      /* Search complete */
      tsFmrdsSrchCompleteEvType           fmrdsSrchComplete;
      /* Search station list summary */
      tsFmrdsSrchStationListEvType        fmrdsSrchStationsList;
      /* Search canceled has no body */
      /* Rds Rx processing options */
      tsFmrdsRxProcOptEvType              fmrdsRxProcOptEv;
      /* Raw RDS Blocks buffer */
      tsFmrdsRawRdsBlockEvType            fmrdsRawRdsBlockEv;
      /* Rds Rx group processing */
      tsFmrdsRawRdsGrpsEvType             fmrdsRawRdsGrpsEv;
      /* Register for PI match option */
      tsFmrdsRdsRxPIMtchProcOptsEvType    fmrdsRdsPIMtchProcOptsEv;
      /* PI match available */
      tsFmrdsPIMtchGrpEvType              fmrdsPIMtchGrpEv;
      /* Register for BlockB match option */
      tsFmrdsRdsRxBlkBMtchProcOptsEvType  fmrdsRdsBlkBMtchProcOptsEv;
      /* BlockB match available */
      tsFmrdsBlkBMtchGrpEvType            fmrdsBlkBMtchGrpEv;
      /* PS Info complete has no body */
      /* Stop PS Info complete has no body */
      /* RT Info complete has no body */
      /* Stop RT Info complete has no body */
      /* Tx Rds group complete */
      tsFmrdsTxRdsGrpsCmptEvType          fmrdsTxRdsGrpsCmptEv;
      /* Tx Rds cont group complete */
      tsFmrdsTxContRdsGrpsCmptEvType      fmrdsTxRdsContGrpsCmptEv;
      /* Transmit Rds group control */
      tsFmrdsTxRdsGrpCtrlOptEvType        fmrdsTxRdsGrpCtrlOptEv;
      /* Error event */
      tsFmrdsErrEvType                    fmrdsErrCodeEv;

#ifdef FEATURE_FM_SERVICE_AVAILABLE
      // service available event
      tsFmrdsServiceAvailableEvType       fmrdsFmServiceAvailableEv;
#endif  // FEATURE_FM_SERVICE_AVAILABLE
   } tufmrdsStatusParameters;
} tsFmrdsEvMsgBodyType;

/* =FMRDS event message type structure */
typedef struct _tsFmrdsEvMsgStruct
{
   teFmrdsEvResultType   fmrdsEvResult;        /* Event success/failure */
   teFmrdsEventType      fmrdsEventName;       /* Event Id */
   tsFmrdsEvMsgBodyType  fmrdsEventMsgBody;    /* Event Content */
} tsFmrdsEvMsgType;

/* =Function pointer type used for registering of fmrds events */
typedef void (tfFmrdsEvCbFnType)(tsFmrdsEvMsgType fmrdsEvMsg);

/* =================================================================================== */

 /*----------------------------------------------------------------------------
  * Function Declarations and Documentation
  * -------------------------------------------------------------------------*/

/***********************************************************************************
***
***     Public Functions
***
***********************************************************************************/

/***********************************************************************************
***                                                                             ****
***     FM RDS Queued Commands                                                  ****
***                                                                             ****
************************************************************************************/

/*
**  The function calls listed below result in a command data being entered in the FM
**  task's command queue and queue status being returned. The command will subsequently
**  be retrieved from the queue and executed. In most cases, an associated
**  asynchronous event will be generated.
**
**  note: event generation may be modified in future releases to reduce the return
**  of unnecessary command confirmation data.
**
*/

/***********************************************************************************
***                                                                             ****
***     FM RDS Task                                                             ****
***                                                                             ****
***********************************************************************************/

/***********************************************************************************

FUNCTION      fmrds_task( )

DESCRIPTION	  Main processing loop for all FM-RDS Radio messages.

DEPENDENCIES  FEATURE_FM defined, fmrds_tcb definition necessary

RETURN VALUE  *** Does not return ***

SIDE EFFECTS  None

************************************************************************************/

void fmrds_task(dword dummy);



/***********************************************************************************
***                                                                             ****
***     Other Non-Apps Level Public Functions                                   ****
***                                                                             ****
************************************************************************************/

 /*----------------------------------------------------------------------------
  * Function Declarations and Documentation
  * -------------------------------------------------------------------------*/

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
                                                teFmrdsCbEvMaskType fmCbEvMask);

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
 o	fmrds_client_id

 Client ID returned from a successful call to fmrds_api_register_client. 
 The command is issued directly to the FM stack. This command does not result 
 in a command being placed on the command queue, or any direct communication 
 with the FM-RDS device.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
void FmrdsApi_DeRegisterClient(uint8 ucFmrdsClientId);

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
    o efm_radio_band
         "	US_EUROPE_BAND
         "	JAPAN_BAND
    o efm_deemphasis
         "	DE_EMP75
         "	DE_EMP50
    o efm_chanspacing
         "	CHSP_200K
         "	CHSP_100K
         "	CHSP_50K
    o efm_rds_mode
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
                                         fmrdsRadioSettings);

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
teFmrdsCmdQStatusType FmrdsApi_DisableFm (void);

/*=============================================================================

FUNCTION		FmrdsApi_ConfigureRadio

DESCRIPTION     
 This is an indirect command used to reconfigure various settings on the FM-RDS 
 device and FM-Stack. Included in the passed structure are settings which 
 typically differ from one region to another, and also control for RDS/RBDS.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsRadioSettings
    o efm_radio_band
       " US_EUROPE_BAND
       " JAPAN_BAND
    o efm_deemphasis
       " DE_EMP75
       " DE_EMP50
    o efm_chanspacing
       " CHSP_200K
       " CHSP_100K
       " CHSP_50K
    o efm_rds_mode
       " RDS_OFF
       " RDS_MODE
       " RBDS_MODE

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
                                               fmrdsRadioSettings);

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
teFmrdsCmdQStatusType FmrdsApi_SetMuteMode (teFmrdsMuteLftRgtType fmrdsMuteCtrl);

/*=============================================================================

FUNCTION		FmrdsApi_SetStereoMode

DESCRIPTION     

 This is an indirect command used to set the mono/stereo mode of the FM-RDS 
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
 apply the mono/stereo settings to the FM-RDS device. On completion, a 
 FMRDS_EV_STEREO_MODE_SET event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.


RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SetStereoMode (teFmrdsStereoMonoModeType fmrdsStroMode);

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
 is packaged into an FMRDS_SET_STATION command in the FM-RDS command queue. The 
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
teFmrdsCmdQStatusType FmrdsApi_SetStation(uint16 usFmrdsFreq);

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
teFmrdsCmdQStatusType FmrdsApi_GetTuneStatus (void);

/*=============================================================================

FUNCTION		FmrdsApi_GetStationParameters

DESCRIPTION     

 This is an indirect command which returns various statistics related to a 
 tuned station. These statistics include, but are not limited to, the 
 following: currently tuned frequency (zero if not tuned), RDS/RBDS Sync 
 status, Mono/Stereo indication, RSSI level, and Interference count.  This 
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
 FMRDS_EV_STATION_PARAMETERS event and associated event message is returned 
 asynchronously to the application layer. In this case, it is returned via a 
 callback mechanism to the FM-RDS BREW OEM layer. This API should only be 
 called after successful registration and initialization is completed.

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_GetStationParameters (void);

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
teFmrdsCmdQStatusType FmrdsApi_GetRdsSyncStatus(void);

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
teFmrdsCmdQStatusType FmrdsApi_GetPSInfo (void);

/*=============================================================================

FUNCTION		FmrdsApi_GetRtInfo

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
teFmrdsCmdQStatusType FmrdsApi_GetRTInfo (void);

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
teFmrdsCmdQStatusType FmrdsApi_GetAFInfo (void);

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
teFmrdsCmdQStatusType FmrdsApi_GetRssiInfo (void);

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
teFmrdsCmdQStatusType FmrdsApi_SetLowPowerMode (teFmrdsPwrModeType fmrdsPwrMode);

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
teFmrdsCmdQStatusType FmrdsApi_TxSetPower (teFmrdsTxPwrLevType txPwrLev);

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
teFmrdsCmdQStatusType FmrdsApi_SetVolume (tsFmrdsVolLevOptsType fmrdsVolLevOpts);

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
teFmrdsCmdQStatusType FmrdsApi_TxGetSettings (void);

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
teFmrdsCmdQStatusType FmrdsApi_SetRadioMode (teFmrdsRadioModeType fmrdsTxRxMode);

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
       teFmrdsSrchWrapType     srchWrap;
    o	BAND_WRAP ' wrap at end of band if no stations found
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
 
 If the band-wrap parameter is set, the seek or search operation will wrap to
 the top/bottom of the selected band if no stations are found when the
 end-of-band limit is reached. In this case, the seek/search operation will
 terminate when the originally tuned station is reached.

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

 The search can be canceled at any time by using API fmrds_api_cancel_search (). 

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_SearchStations (tsFmrdsRadioSrchModeType  
                                               fmrdsRadioSrchMode);

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

 The search can be canceled at any time by using API fmrds_api_cancel_search 

RETURN VALUE      

SIDE EFFECTS      

===============================================================================*/
teFmrdsCmdQStatusType FmrdsApi_RdsSearchStations (tsFmrdsRdsRadioSrchModeType
                                                  fmrdsRdsSrchMode);

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
                                                  fmrdsStationListOpts);

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
teFmrdsCmdQStatusType FmrdsApi_CancelSearch (void);

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
 "en_rds_buffer_size" error-free groups will be returned.

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
                                                          fmrdsRxProcOpt);

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
 is packaged into an FMRDS_REG_RDS_RX_GRPS_PROCESSING command in the FM-RDS 
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
teFmrdsCmdQStatusType FmrdsApi_RegRdsRxGroupProcessing (uint32 ulFmrdsGrpsToProc);

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
                                                          fmrdsRdsPiMtchProcOpts);


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
                                                              fmrdsBlkBMtchProcOpts);

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
boolean FmrdsApi_TxSetStationPi (teFmrdsPrgmIdType fmrdsTxPi);

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
uint8 FmrdsApi_TxGetPSFeatures (void);

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
                                                fmrdsTxPSStationInfoOpts);

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
teFmrdsCmdQStatusType FmrdsApi_RxStopPSStationInfo (void);

/*=============================================================================

FUNCTION		FmrdsApi_TxRtStationInfo

DESCRIPTION     

 This function will continuously transmit RDS Radio Text information over a 
 tuned frequency.

DEPENDENCIES    None.

PARAMETERS		
 o	fmrdsTxRtStationInfoOpts
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
                                                fmrdsTxRtStationInfoOpts);

/*=============================================================================

FUNCTION		FmrdsApi_TxStopRtStationInfo

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
teFmrdsCmdQStatusType FmrdsApi_TxStopRTStationInfo (void);

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
                                            fmrdsTxOneShotRdsGrpsOpts);

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
                                                fmrdsTxContRdsGrpsOpts);

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
                                               fmrdsTxRdsGrpCtrlOpt);

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
uint8 FmrdsApi_TxGetRdsGroupsBufSize (void);

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
                                                   fmrdsRTPlusClassProcOpts);

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
   teFmrdsRTPlusClassType rtPlusContType;
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
                                                    fmrdsTxRTPlusStationInfo);

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
teFmrdsCmdQStatusType FmrdsApi_FMServiceAvailable (uint8 fmrdsServiceRssi);

/*----------------------------------------------------------------------------
 * End
 * -------------------------------------------------------------------------*/

#endif  // FEATURE_FM

#endif /* #ifndef FMRDS_API_H */










