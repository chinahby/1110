#ifndef FMRDS_GENERIC_H
#define FMRDS_GENERIC_H

/******************************************************************************
***
***   TITLE: fmrds_generic.h
***
***   Generic FM/RDS Function Header
***   ------------------------------
***
***
***   DESCRIPTION
***
***   Header for low level primitives controlling specific FM receiver device.
***   Functions are called as required to implement higher-level functionality.
***   There should be no device-specific references in this file.
***
***
***   Copyright (c) 2008 QUALCOMM Incorporated.
***   All Rights Reserved.
***   Qualcomm Confidential and Proprietary
***
***   Export of this technology or software is regulated by the U.S.
***   Government. Diversion contrary to U.S. law prohibited.
***
******************************************************************************/

/******************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
***  $Header: //source/qcom/qct/wconnect/fmradio/fmrds/main/latest/inc/fmrds_generic.h#11 $ $DateTime: 2008/11/26 09:33:21 $ $Author: abhis $
***
***  when       who         what, where, why
***  --------------------------------------------------------------------------
***  2008-09-29 rjm         Generic function list cleaned up.
***  2008-09-05 as          stereoProgram type added to DevLockSync struct
***  2008-07-25 rjm	        Additional structure values eg ifcnt added.
***  2008-07-17 as          Updated tsFmrdsDevLockSyncStruct
***  2008-07-15 rjm         rx volume added to settings structure
***  2008-07-01 rjm         provisional ftm functions included
***  2008-06-27 rjm         GetCurrentSettings() and settings structure updated
***  2008-06-05 rjm         initial release
*** 
******************************************************************************/


// generic device primitives perform following functions...
//
// device setup...
// power up/down                      FmrdsDev_SetPowerState( )
// get device id                      FmrdsDev_GetDeviceId( )
// set rx tx operating mode           FmrdsDev_SetRadioMode( )
// common initialisation              FmrdsDev_Init( )

// active settings...
// configure radio parameters         FmrdsDev_ConfigureRadio( )

// audio control...
// set audio level                    FmrdsDev_SetRxAudioLevel( )
// set mute mode                      FmrdsDev_SetMuteMode( )
// set stereo mode                    FmrdsDev_SetStereoMode( )

// tuning...
// tune to station                    FmrdsDev_SetStation( )
// get fm tune/lock status            FmrdsDev_GetTuneStatus( )
// get current platform settings      FmrdsDev_GetCurrentSettings( )

// seeking/scanning...
// seek next station                  FmrdsDev_StationSeek( )
// cancel station seek                FmrdsDev_CancelSeek( )

// rds data return...
// set up rds data capture            FmrdsDev_SetRdsMode( )
// set up rds group decoding          FmrdsDev_SetRdsRxProcessingOptions( )
// retrieve updated rds data          FmrdsDev_GetDeviceUpdate( )  

// interrupt operation...
// enable device interrupt            FmrdsDev_EnableIsr( )
// disable device interrupt           FmrdsDev_DisableIsr( )
// device interrupt handler           FmrdsDev_Isr( )

// device communications...
// device read data                   FmrdsDev_ReadData( )
// device write data                  FmrdsDev_WriteData( )

// future functions - tbc
//  get ps info                       FmrdsDev_GetPSInfo( )
//  get rt info                       FmrdsDev_GetRTInfo( )
//  get af info                       FmrdsDev_GetAFInfo( )
//  return rds group data             FmrdsDev_GetRdsGroup( )                    
//  set rds error level               FmrdsDev_SetRdsErrorLevel( )


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "custfmrds.h"          // include global fm rds switches

#include "target.h"
#include "comdef.h"


#ifdef FEATURE_FMRDS


// *** functional defines ***
#undef  FM_TX_SUPPORTED               // platform support for fm tx device present
 
#undef  FMRDS_STATION_SEEK_IN_HW      // station seek function performed by receiver
#define FMRDS_INCLUDE_FTM             // include basic factory test code


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

//
// where appropriate to generic drivers, local definitions are used.
// where there is commonality with the higher-level api, the appropriate
// high-level api definitions are typedef'd
//

// platform-related definitions
#if (defined(T_QSC1100) || defined(T_QSC1110))

#define FMRDS_INT_GPIO    GPIO_INT_31

#endif  // T_QSC1100...


// device primitive result codes
typedef enum {
  FMRDSDEV_OK,
  FMRDSDEV_HW_ERROR,      // hw-related
  FMRDSDEV_COMMS_ERROR,   // device comms error
  FMRDSDEV_ERROR          // generic failure/unsupported feature
} teFmrdsDevResult;

// device power-switching states
typedef enum {
  FMRDSDEV_POWER_UP,      // power fm area up
  FMRDSDEV_POWER_DOWN,    // power fm area down
  FMRDSDEV_LOW_POWER,     // go to low power state
  FMRDSDEV_HIGH_POWER     // go to normal power state
} teFmrdsDevPowerState;

// radio rx/tx operating modes
/*typedef enum {  
   FM_RX_ONLY, 
   FM_TX_ONLY, 
   FM_RX_TX            // possible, but unlikely
} teFmrdsDevOperatingMode; */ 
typedef teFmrdsRadioModeType teFmrdsDevRadioModeType; 

// fm lock/rds sync state
typedef enum {  
   FMRDS_UNLOCKED,    // station not locked 
   FMRDS_LOCKED,      // station locked
   FMRDS_NORDSSYNC,   // no rds sync
   FMRDS_RDSSYNC      // rds synch
} teFmrdsDevLockSyncState;

// device update return codes
typedef enum {  
   FMRDS_UPDATE_DONE,             // done, no action 
   FMRDS_UPDATE_BLOCK_BUFF_FULL,  // raw rds buffer full
   FMRDS_UPDATE_PS_AVAILABLE,     // ps data available
   FMRDS_UPDATE_RT_AVAILABLE,     // rt data available
   FMRDS_UPDATE_AF_AVAILABLE      // af data available
} teFmrdsDevUpdateStatus;


#define FMRDS_DEVID_STR (16+1)    // textual device id string

// fm device id info
typedef struct _tsFmrdsDevIDStruct {
  uint8 pucDeviceIDString[FMRDS_DEVID_STR];   // descriptive string
  uint8 ucDevHWRev;                           // numeric hw revision
  uint8 ucDevFWRev;                           // numeric fw revision
} tsFmrdsDevIDStruct;

// fm get device id return structure
typedef struct _tsFmrdsDevIDReturnStruct {
  teFmrdsDevResult    teDevIDResult;          // result code
  tsFmrdsDevIDStruct  tsDevIDInfo;            // dev id data if ok
} tsFmrdsDevIDReturn;

// radio settings
typedef uint16 tusFmrdsDevFreqType;      // station (channel) frequency
typedef uint8 tucFmrdsDevRSSIValType;    // current station rssi value
typedef tsFmrdsRadioSettingsType  tsFmrdsDevRadioSettingsType; // as for fm api
typedef teFmrdsErrLevType         teFmrdsDevErrLevType;        // as for fm api
typedef teFmrdsFmPwrUpMode        teFmrdsDevFmPwrUpMode;
typedef teFmrdsPwrModeType        teFmrdsDevPwrModeType;
typedef teFmrdsMuteLftRgtType     teFmrdsDevMuteLftRgtType;
typedef teFmrdsRadioCapType       teFmrdsDevRadioCapType;
typedef tsFmrdsRxProcOptType      tsFmrdsDevRxProcOptType;
typedef tsRawBlockData            tsFmrdsDevRawBlockData;
typedef tsFmrdsRawRdsBlockEvType  tsFmrdsDevRawRdsBlock;

// station seek settings
typedef tsFmrdsRadioSrchModeType  tsFmrdsDevRadioSrchModeType;  // as for fm api


// audio settings
typedef teFmrdsVolumeLevType teFmrdsDevAudioLevelType;  // audio level (rx and tx)
typedef teFmrdsMuteLftRgtType     teFmrdsDevMuteType;   // as for api
typedef teFmrdsStereoMonoModeType teFmrdsDevStereoType; // as for api & si4703 shadow reg map
typedef teFmrdsSyncStateChangeType teFmrdsDevSyncStateChangeType;

//fm stereo indicator
typedef teFmrdsStereoProgramType teFmrdsDevStStateType;

// fm lock/sync status
typedef struct _tsFmrdsDevLockSyncStruct {
  tusFmrdsDevFreqType       tusDevFreq;             // station (channel) frequency
  teFmrdsDevLockSyncState   teDevLockState;         // FM lock status
  teFmrdsDevLockSyncState   teDevRdsSyncState;      // RDS Sync status
  tucFmrdsDevRSSIValType    tucDevRSSIVal;          // current rssi value
  uint8                     ucDevIfcnt;             // if count (if supported)
  boolean                   rdsCapability;          // rds/no rds
  teFmrdsDevStStateType     teDevStState;           // Stereo/Mono mode status
} tsFmrdsDevLockSyncStruct;

typedef struct _tsFmrdsDevLockSyncReturnStruct {
  teFmrdsDevResult          teDevLockSyncResult;    // result code
  tsFmrdsDevLockSyncStruct  tsDevLockSyncStatus;    // tune/lock status
} tsFmrdsDevLockSyncReturn;

// seek operation return structures
// basic station tune info...
typedef struct _tsFmrdsDevTuneStatus {
  uint16                    stationFreq;    // tuned station
  uint8                     rssi;           // rssi
  teFmrdsDevStStateType     stereoProgram;  // stereo/mono operation
} tsFmrdsDevTuneStatus;
  
// return structure
typedef struct _tsFmrdsDevSeekReturn {
  teFmrdsDevResult          teDevSeekResult;    // seek success/fail
  tsFmrdsDevTuneStatus      tusDevTuneStatus;   // if success, tuned station info
  boolean                   bandLimitReached;   // seek operation reached end of band
}tsFmrdsDevSeekReturn;


// used in fmrds_si4703 shadow register map
typedef teFmrdsRdsPwrUpMode     teFmrdsDevRdsPwrUpMode;
typedef teFmrdsDtcTimeType      teFmrdsDevDtcTimeType;
typedef teFmrdsBandLimitsType   teFmrdsDevBandLimitsType;
typedef teFmrdsChanSpacingType  teFmrdsDevChanSpacingType;

typedef tsFmrdsBandRangeType tsFmrdsDevBandRangeType;
typedef teFmrdsRdsStd   teFmrdsDevRdsStdType;

// current platform fm device settings
typedef struct _tsDevDeviceSettingsStruct
{
  tsFmrdsDevRadioSettingsType    radioSettings;  // band, deemph, chan spacing,
                                                 // rds standard, band range 
  teFmrdsDevRadioModeType        radioMode;      // off, rx, tx, rx + tx
  teFmrdsDevRadioCapType         radioCap;       // none, rx, tx, rx + tx
  teFmrdsDevFmPwrUpMode          fmPwrUpMode;    // rx on, tx on, reset
  teFmrdsDevRdsPwrUpMode         rdsPwrUpMode;   // rds on or off
  teFmrdsDevPwrModeType          pwrMode;        // normal or low power
  teFmrdsDevMuteLftRgtType       muteMode;       // muted/unmuted   
  teFmrdsDevStereoType           monoStereo;     // stereo/mono  
  teFmrdsDevAudioLevelType       rxAudioLevel;   // rx audio level
  tusFmrdsDevFreqType            stationFreq;    // channel freq
  tsFmrdsDevRxProcOptType        rawRdsProcOpts; // rx raw rds settings
} tsFmrdsDevDeviceSettingsType;


//  *** RDS\RBDS related type definitions

// content enumeration
typedef teFmrdsGroupType      teFmrdsDevGroupType;
typedef teFmrdsRdsPrgmType    teFmrdsDevRdsPrgmType;
typedef teFmrdsRbdsPrgmType   teFmrdsDevRbdsPrgmType;

//RDS block types
typedef enum 
{
  BLOCK_A = 0,
  BLOCK_B,
  BLOCK_C,
  BLOCK_D
}teFmrdsDevBlockIdType;

// RDS Programme Type 
typedef struct _tsFmrdsPtyStruct
{
  union
  {
    teFmrdsDevRdsPrgmType    rdsPty;
    teFmrdsDevRbdsPrgmType   rbdsPty;
  }tuFmrdsPtyUnion;
}tsFmrdsPtyType; 

// RDS Programme Identification
typedef struct _tsFmrdsPiStruct
{
  union
  {
    uint16   usRdsPi;       // 16 bit unique program id 
    char     *pucRbdsPi;    // call letter for rbds pi code
  }tuFmrdsPiUnion;
}tsFmrdsPiType; 

// return structure for commonly decoded RDS\RBDS fields
// extend as required for basic rds info...
typedef struct _tsFmrdsRdsFields 
{
  
  tsFmrdsPiType   fmrdsPi;                  // program identifier: number or short string (rbds)
  tsFmrdsPtyType  fmrdsPty;                 // program type fields: differ in rds and rbds 
  char            *pucPs;                   // null-terminated 8 character programme name 
  char            *pucRadioText;            // null-terminated 64 char radio text string
  
  uint8           ucPsLength;               // ps string length
  uint8           ucRtLength;               // rt string length
  uint8           psNum;                    // ps repeat       
  boolean         textABFlag;               // text a/b flag
  
  boolean         msSwitchCodeFlag;         // music/speech switch flag
  boolean         decIdCtrlCodeFlag;        // decoder id control code
  
  boolean         taPrgmCodeFlag;           // traffic program flag
  boolean         taAnnouncementCodeFlag;   // traffic announcement flag
  
  uint8           noOfAFInList;                 // af list length
  uint16          AFFreqList[MAX_AF_LIST_VAL];  // af list

}tsFmrdsRdsFields;

typedef tsFmrdsRdsFields      tsFmrdsDevRdsFields;   // common current rds data
typedef tsFmrdsPtyType        tsFmrdsDevPtyType;     // program type


/*******************************************************************************
***
***     Public Generic Device Function Prototypes
***
*******************************************************************************/

//
// power and configuration
//

/*******************************************************************************

FUNCTION      FmrdsDev_SetPowerState(teFmrdsDevPowerState fmrdsPowerState)

DESCRIPTION   Function switches the power state of the FM receiver device.
              Used to power the FM receiver on and off and subsequently switch
              between normal operation and (possibly several) power-saving modes.
              Function should perform any i/o or clock configuration necessary
              to bring device into operation.
              
              After power up, device should be in sane operating mode, awaiting
              software initialisation.

              Areas of this function will be target-dependant.

DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetPowerState(teFmrdsDevPowerState fmrdsPowerState);


/*******************************************************************************

FUNCTION      FmrdsDev_GetDeviceId(void)

DESCRIPTION   Returns ID info for FM device in use.
              
DEPENDENCIES  None

RETURN VALUE  Returns command status and device id info.

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevIDReturn FmrdsDev_GetDeviceId(void);


/*******************************************************************************

FUNCTION      FmrdsDev_SetRadioMode(teFmrdsDevRadioModeType fmrdsRadioMode)

DESCRIPTION   Set receive or transmit operating modes (or both) according to
              device capabilities. This function should configure hw areas as
              necessary.
              
DEPENDENCIES  Rx and/or Tx operation will be FM HW device-dependant

RETURN VALUE  Error returned if device does not support specified mode, else ok.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult  FmrdsDev_SetRadioMode(teFmrdsDevRadioModeType fmrdsRadioMode);


/*******************************************************************************

FUNCTION      FmrdsDev_Init(void)

DESCRIPTION   Function performs all necessary initialisation after power has
              been applied. Sets device to a sane state for subsequent
              configuration of operating parameters.

              Function should check any version ids and set sane default
              parameters as necessary. Device should be left benign in low-power
              mode with audio path muted pending further functional configuration.
              
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_Init(void);


//
// functional setup
//

/*******************************************************************************

FUNCTION      FmrdsDev_ConfigureRadio(tsFmrdsDevRadioSettingsType)

DESCRIPTION   Sets basic radio-related operating parameters for FM and RDS
              reception:
                - radio band
                - de-emphasis
                - channel spacing
                - rds standard
                - band range

              Accepts device level settings: these may well be a straight
              pass-through from API layer above.
              
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_ConfigureRadio(tsFmrdsDevRadioSettingsType
                                                            fmrdsRadioSettings);

/*******************************************************************************

FUNCTION      FmrdsDev_SetRdsMode(void)

DESCRIPTION   Set up the RDS mode & related interrupt settings for its operation.
           
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetRdsMode(void);

/*******************************************************************************

FUNCTION      FmrdsDev_SetRdsRxProcessingOptions(tsFmrdsDevRxProcOptType
                                                       fmrdsRxProcOpt)

DESCRIPTION   Sets raw Rds-related operating parameters:
                - Group type selection
                - rds buffer size
                - rds change filter
                - enable pass of uncorrectable blocks
                - auto search for alternative frequency
                - rdsReturnRawBlocks 

              Accepts device level settings: these may well be a straight
              pass-through from API layer above.

              * Only "rdsReturnRawBlocks" and "Rds buffer" size arguments 
                supported, function can be expanded to supported others.

                            
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetRdsRxProcessingOptions(tsFmrdsDevRxProcOptType
                                                       fmrdsRxProcOpt);

/*******************************************************************************

FUNCTION      FmrdsDev_SetRxAudioLevel(teFmrdsDevAudioLevelType fmrdsAudioLevel)

DESCRIPTION   Set receive-side audio level locally. Audio range will be device
              dependant.
              
              Level value of 0 = no audio output. Over-range levels
              truncated to highest level supported by device.
              
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetRxAudioLevel(teFmrdsDevAudioLevelType
                                                               fmrdsAudioLevel);


/*******************************************************************************

FUNCTION      FmrdsDev_SetMuteMode(teFmrdsDevMuteType fmrdsMuteMode)

DESCRIPTION   Set audio muting for left and right channels
              
              Settings data can be straight pass-through from API layer above.

DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetMuteMode(teFmrdsDevMuteType fmrdsMuteMode);


/*******************************************************************************

FUNCTION      FmrdsDev_SetStereoMode(teFmrdsDevStereoType fmrdsStereoMode)

DESCRIPTION   Set stereo or mono audio operation.
              
              Settings data can be straight pass-through from API layer above.

DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetStereoMode(teFmrdsDevStereoType fmrdsStereoMode);


//
// basic operation
//

/*******************************************************************************

FUNCTION      FmrdsDev_SetStation(tusFmrdsDevFreqType fmrdsFmrdsFreq)

DESCRIPTION   Tune FM receiver to specified channel. Frequency is in 0.01Mhz
              steps, giving basic bands a valid range of 8750 - 10800 for
              US/Europe, 7600 - 9100 for Japan.
              
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetStation(tusFmrdsDevFreqType fmrdsFmrdsFreq);


/*******************************************************************************

FUNCTION      FmrdsDev_GetTuneStatus(void)

DESCRIPTION   Get frequency, lock and rssi status of currently tuned channel.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevLockSyncReturn FmrdsDev_GetTuneStatus(void);


/*******************************************************************************

FUNCTION      FmrdsDev_GetCurrentSettings(void)

DESCRIPTION   Return current FM device settings and status. This function should
              provide necessary info passed up within API events.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevDeviceSettingsType FmrdsDev_GetCurrentSettings(void);


//
// channel seek/scan
//

/*******************************************************************************

FUNCTION      FmrdsDev_StationSeek(tsFmrdsDevRadioSrchModeType fmrdsSeekParams)

DESCRIPTION   Performs seek operation for next available fm station in band
              previously set band, using selected channel spacing. Seek
              performed according to parameters passed.

              Seek operation is dependant on receiver hardware functionality.

              Note: there will be no return from this function until a station
              is found or the band limits have been reached. This suits
              receiver devices that have stated, pre-determined seek/tune times.
              At no point within this function should the code cause other
              tasks to be blocked.
              
DEPENDENCIES  None

RETURN VALUE  Returns scan success and new station tune info if station found,
              else fail if no station found. Receiver left in benign state.
              There is no implied re-tune to station previously in use before
              seek operation initiated.

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevSeekReturn FmrdsDev_StationSeek(tsFmrdsDevRadioSrchModeType fmrdsSeekParams);


/*******************************************************************************

FUNCTION      FmrdsDev_CancelSeek(void)

DESCRIPTION   Cancel any previously initialted station scan, leaving receiver in
              a benign state.

              Given short hardware seek durations, this function may have limited
              use.
              
DEPENDENCIES  None

RETURN VALUE  Success or failure of cancel operation.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_CancelSeek(void);


//
// rds data return
//

/*******************************************************************************

FUNCTION      FmrdsDev_GetDeviceUpdate(void)

DESCRIPTION   Retrieve new data from fm device: likely to be new rds data.
              Need for update previously signalled via device isr.
              
DEPENDENCIES  None

RETURN VALUE  Returns status of updated data

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevUpdateStatus FmrdsDev_GetDeviceUpdate(void);


//
// interrupt handling
//

/*******************************************************************************

FUNCTION      FmrdsDev_EnableIsr(void)

DESCRIPTION   Enable device interrupt(s).
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_EnableIsr(void);


/*******************************************************************************

FUNCTION      FmrdsDev_DisableIsr(void)

DESCRIPTION   Disable device interrupt(s).
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_DisableIsr(void);


/*******************************************************************************

FUNCTION      FmrdsDev_Isr(void)

DESCRIPTION   Device interrupt handler. Primarily to be for retrieval of
              RDS block or group data.
                            
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_Isr(void);


//
// device communication
//

/*******************************************************************************

FUNCTION     FmrdsDev_ReadData(uint8 ucSlaveAddress, uint8 ucNoOfBytes,
                                                            uint8 *pucByteBuff)

DESCRIPTION  Reads data from fm device registers. Using slave address passed,
             reads specified number of bytes and stores them @ buffer pointer
             passed.

             For 4703 device, uses 2-wire/I2C communications.

DEPENDENCIES None

RETURN VALUE Returns ok or error status. 

SIDE EFFECTS None 

*******************************************************************************/
teFmrdsDevResult FmrdsDev_ReadData(uint8 ucSlaveAddress,
                                         uint8 ucNoOfBytes, uint8 *pucByteBuff);


/*******************************************************************************

FUNCTION     FmrdsDev_WriteData(uint8 ucSlaveAddress, uint8 ucNoOfBytes,
                                                            uint8 *pucByteBuff)

DESCRIPTION  Writes data to fm device registers. Using slave address passed,
             writes specified number of bytes from buffer pointed to.

             For 4703 device, uses 2-wire/I2C communications.

DEPENDENCIES None

RETURN VALUE Returns ok or error status. 

SIDE EFFECTS None 

*******************************************************************************/
teFmrdsDevResult FmrdsDev_WriteData(uint8 ucSlaveAddress,
                                          uint8 ucNoOfBytes, uint8 *pucByteBuff);


/*******************************************************************************

FUNCTION     FmrdsDev_FillRegs(uint8 ucNoOfBytes)

DESCRIPTION  TBC
             For 4703 device, uses 2-wire/I2C communications.

DEPENDENCIES None

RETURN VALUE Returns ok or error status. 

SIDE EFFECTS None 

*******************************************************************************/
teFmrdsDevResult FmrdsDev_FillRegs(uint8 ucNoOfBytes);


/*******************************************************************************

FUNCTION      FmrdsDev_ChanValDecode(tusFmrdsDevFreqType inputFmrdsFreq, 
                                     tusFmrdsDevFreqType *outputFmrdsChanVal)

DESCRIPTION   Decodes the channel value from requested channel frequency.
              Freq(MHz) = Spacing(KHz) x Channel + Bottom of Band(MHz)
              
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None
*******************************************************************************/
teFmrdsDevResult FmrdsDev_ChanValDecode(tusFmrdsDevFreqType inputFmrdsFreq, 
                                     tusFmrdsDevFreqType *outputFmrdsChanVal);

/*******************************************************************************

FUNCTION      FmrdsDev_FreqDecode(tusFmrdsDevFreqType inputFmrdsChanVal, 
                                  tusFmrdsDevFreqType *outputFmrdsFreqVal)

DESCRIPTION   Decodes the frequency value from channel value read from silab regs.
              Freq(MHz) = Spacing(KHz) x Channel + Bottom of Band(MHz)
              Freq x 100KHz = Spacing(KHz) x Channel + Bottom of Band x 100(KHz)
                  
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None
*******************************************************************************/
teFmrdsDevResult FmrdsDev_FreqDecode(tusFmrdsDevFreqType inputFmrdsChanVal, 
                                     tusFmrdsDevFreqType *outputFmrdsFreqVal);

/*******************************************************************************

FUNCTION      FmrdsDev_DecodeRdsData(void)

DESCRIPTION   Function decodes the RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_DecodeRdsData(void);


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockA(void)

DESCRIPTION   Function decodes BLOCK-A RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockA(void);


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockB(void)

DESCRIPTION   Function decodes BLOCK-B RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockB(void);


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockC(void)

DESCRIPTION   Function decodes BLOCK-C RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockC(void);


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockD(void)

DESCRIPTION   Function decodes BLOCK-D RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockD(void);


/*******************************************************************************

FUNCTION      void FmrdsDev_GetPs(void)

DESCRIPTION   Function decodes "Programme station" RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_GetPs(void);


/*******************************************************************************

FUNCTION      void FmrdsDev_GetRt(teFmrdsDevBlockIdType fmrdsBlockId)

DESCRIPTION   Function decodes "Radio Text" RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_GetRt(teFmrdsDevBlockIdType fmrdsBlockId);

/*******************************************************************************

FUNCTION      void FmrdsDev_InitStorage(void)

DESCRIPTION   Function to clear all data storage at driver level.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_InitStorage(void);

/*******************************************************************************

FUNCTION      void FmrdsDev_ClearRdsBuffers(void)

DESCRIPTION   Function to clear the RDS data storage at driver level.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ClearRdsBuffers(void);

/*******************************************************************************

FUNCTION      tsFmrdsDevRdsFields FmrdsDev_GetRdsData(void)

DESCRIPTION   Function to read the available decoded RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevRdsFields FmrdsDev_GetRdsData(void);

/*******************************************************************************

FUNCTION      FmrdsDev_CopyRdsData(void)

DESCRIPTION   Function decodes the RDS data.
              
DEPENDENCIES  None

RETURN VALUE  Returns status of updated data

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevUpdateStatus FmrdsDev_CopyRdsData(void);

/*******************************************************************************

FUNCTION      FmrdsDev_GetRdsBuffInfo(void)

DESCRIPTION   Function to read raw rds data buffer information.
              
DEPENDENCIES  None

RETURN VALUE  Returns RDS buffer information.

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevRawRdsBlock FmrdsDev_GetRdsBuffInfo(void);

#ifdef FMRDS_INCLUDE_FTM

/*******************************************************************************
***
***     Low Level Factory Test Mode Routines
***
*******************************************************************************/
//
// used for basic functional software test: need not be included in
// basic build. These may change if fm rds is formalised.
//

// *** read device registers ***
teFmrdsDevResult FtmFmrdsDev_ReadRegs(void);

// *** initialise device ***
teFmrdsDevResult FtmFmrdsDev_Init(void);

// *** set normal power mode ***
teFmrdsDevResult FtmFmrdsDev_SetNormalPower(void);

// *** configure radio settings ***
teFmrdsDevResult FtmFmrdsDev_ConfigureRadio(void);

// *** set station ***
teFmrdsDevResult FtmFmrdsDev_SetStation(void);

#endif  // FMRDS_INCLUDE_FTM

#endif  /* FEATURE_FMRDS */

#endif  /* FMRDS_GENERIC_H  */


// *** end of file ***