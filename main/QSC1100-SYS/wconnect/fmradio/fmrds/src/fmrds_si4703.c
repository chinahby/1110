/******************************************************************************
***
***   TITLE: fmrds_si4703.c
***
***   Generic FM/RDS Device Driver Code.
***
***   Device = SiLabs 4703 FM Receiver
***   -------------------------------------------------------------------
***
***
***   DESCRIPTION
***
***   Low level primitives controlling specific FM receiver device. These are
***   defined in the generic layer of the FM/RDS support code and are called
***   as required to implement higher-level functionality.
***
***   Function prototypes present in fmrds_generic.h.
***   
***   Used to initialise FM device, set/get parameters and monitor device
***   status.
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
***  $Header: //source/qcom/qct/wconnect/fmradio/fmrds/main/latest/src/fmrds_si4703.c#9 $ $DateTime: 2008/11/26 09:33:21 $ $Author: abhis $
***
***  when       who         what, where, why
***  --------------------------------------------------------------------------
***  2008-11-07 as          klockwork array bounds check
***  2008-11-06 as          Added out of range parameter check for 
                            configureradio(), setrxaudiolevel(), setstereomode()
***  2008-09-17 as          Moved F3 Medium\Low messages to High till QXDM 
                            support for them becomes available
***  2008-09-13 as          Added sleep_time to allow clearing of STC status bit
                            after tune\seek bit reset.
***  2008-09-10 as          klocwork type mismatch errors fixes
***  2008-09-10 rjm         All freq values now symbolic for consistancy.
***  2008-09-02 rjm         Klocwork: structure inits added.
***  2008-09-29 rjm         Example GetDeviceID added. Placemarker functions
***                         removed.
***  2008-09-23 as          StationSeek() added
***  2008-09-17 as          support for raw RDS data buffering added
***  2008-09-15 as          Fix for ISRs in low power mode 
***  2008-09-05 as          stereoProgram field added to GetTuneStatus(),PS & RT
***                         cleared on retune to new channel,support for radiotext
***                         and msswitch flag
***  2008-08-14 rjm         Null af list return added.
***  2008-08-06 rjm         PMIC clock control voting unpatched.
***  2008-07-30 rjm         Some rds returns hard coded, will be decoded in
***                         subsequent releases.
***  2008-07-25 as          Basic rds support added. Some TuneStatus and
***                         Settings fields rationalised/hard-coded.
***  2008-07-15 rjm         rx vol param changed to enum
***  2008-07-09 as          Mods to freq/spacing calculation, i2c delay added
***                         on SetStation, minor mods to ftm routines.
***  2008-06-30 as          Initial si4703 functionality
***  2008-06-05 rjm         initial release
******************************************************************************/


//
// Driver support for Silicon Labs 4703 fm receiver. 
// Refer to device data sheet and SiLabs App Notes AN230, AN243, AN284.
// Reference code only - check any device respins by manufacturer.
//


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include <string.h>

#include "custfmrds.h"          // include global fm rds switches

#include "target.h"
#include "comdef.h"

#ifdef FEATURE_FMRDS

#include "fmrds_si4703.h"       // include si4703 definitions

#if (defined(T_QSC1100) || defined(T_QSC1110))
#include "pm.h"                 // include pmic defintions if needed
#endif


/*****************************************************************************
***
***     Public Device-Related Defines & Constants 
***
*****************************************************************************/

// ...


/*****************************************************************************
***
***     Local Device-Related Shadow registers\variables 
***
*****************************************************************************/

/* Store Current Mode of silab4703 fmrds device */
static tsSi4703ControlRegMapType        tsSi4703ControlRegMap;
static tsSi4703StatusRegMapType         tsSi4703StatusRegMap;
static tusFmrdsDevFreqType              tusSi4703CurrentFrequency;
static teFmrdsDevRdsStdType             teFmrdsDevRdsStd;
static tsFmrdsDevBandRangeType          tsFmrdsDevBandRange;
static teFmrdsDevRadioModeType          teFmrdsDevRadioMode;

// rds related storage 
static  uint16  usRdsPi = 0;

static char ucaRbdsPi[5] = { 0, 0, 0, 0, 0 };

static tsFmrdsDevPtyType   fmrdsPty;
 
static teFmrdsGroupType  fmrdsGroup = GROUP_TYPE_INVALID;

// ps and rt string buffers: include terminating nulls
static  char  ucaPs[MAX_PS_INFO_CHAR_LEN] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static  char ucaRadioText[MAX_RT_LEN] = {                                
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                 0 };
                                 
static uint8   ucPsCharNum = 0;    // index for referencing PS data contained in block_d 
static uint8   ucRtCharNum = 0;    // index for referencing RT data contained in block_c/d 

static boolean bMusicSpeechFlag = FALSE;
static boolean bRadioTextFlag = FALSE; 
static boolean bRawRdsFlag = FALSE;
static uint8  usRdsBufSz = 0;
static uint8  usRdsMaxBufSz = 0;
static uint8  usRdsBufIndex = 0;  
static uint8  usPostRdsBufIndex = 0;
static tsSi4703RdsDataRegMapType   tsSi4703RdsDataRegMap;
static tsFmrdsDevRawBlockData  tsRawRdsBuffer[MAX_DRVR_RDS_BUFF_SZ]; 

/*******************************************************************************
***
***     Local Device Support Functions
***
*******************************************************************************/

// ...


/*******************************************************************************
***
***     Public Generic Device Functions
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
teFmrdsDevResult FmrdsDev_SetPowerState(teFmrdsDevPowerState fmrdsPowerState)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   
   switch (fmrdsPowerState)
   {
      case FMRDSDEV_POWER_UP:
      {
         // power up fm/rds area
#if (defined(T_QSC1100) || defined(T_QSC1110))
         // qsc1100 platform
         // - enable platform power here if necessary
         pm_vote_clk_32k(ON, PM_32K_CLK_FM_APP);  // vote to enable 32KHz clock
#else
         // other platforms
         // - enable platform here
#endif
         result = FMRDSDEV_OK;
         break;
      }
   
      case FMRDSDEV_POWER_DOWN:
      {
         if((teFmrdsDevRdsStd == RDS_STD_RBDS) || (teFmrdsDevRdsStd == RDS_STD_RDS))
         {
            result = FmrdsDev_DisableIsr();
         }

         // power down the chip - go to low-power mode
         tsSi4703ControlRegMap.si4703PwrMode = SI4703_LOW_POWER_MODE;
         result = FmrdsDev_FillRegs(SIZE_OF_PWRUP_DATA_WRITE);

         // set pwrmode shadow register to correct value after low Pwr
         tsSi4703ControlRegMap.si4703PwrMode = SI4703_AFTER_LOW_PWR;

#if (defined(T_QSC1100) || defined(T_QSC1110))
         // qsc1100 platform
         pm_vote_clk_32k(OFF, PM_32K_CLK_FM_APP);  // vote to disable 32KHz clock
         // - disable platform power here if necessary
#else
         // other platforms
         // - disable platform power here
#endif
         break;
      }
   
      case FMRDSDEV_LOW_POWER:
      {
         // If GPIO2_line != GPIO2_LOW & low power mode is set, GPIO2_line state
         // changes & false interrupt is received. To prevent this GPIO2_line state
         // is changed from GPIO2_INT_ENABLE to GPIO2_LOW as advised 
         // by silab Engineers
         if((teFmrdsDevRdsStd == RDS_STD_RBDS) || (teFmrdsDevRdsStd == RDS_STD_RDS))
         {
            result = FmrdsDev_DisableIsr();
         }
         else
         {            
            result = FMRDSDEV_OK;
         }

         if(result == FMRDSDEV_OK)
         {
            // go to low-power mode
            tsSi4703ControlRegMap.si4703PwrMode = SI4703_LOW_POWER_MODE;
            result = FmrdsDev_FillRegs(SIZE_OF_PWRUP_DATA_WRITE);
         }
         
         // set pwrmode shadow register to correct value after low Pwr
         tsSi4703ControlRegMap.si4703PwrMode = SI4703_AFTER_LOW_PWR;
         
         break;
      }
   
      case FMRDSDEV_HIGH_POWER:
      {
         // go to high-power/normal mode
         tsSi4703ControlRegMap.si4703PwrMode = SI4703_NORMAL_MODE;
         result = FmrdsDev_FillRegs(SIZE_OF_PWRUP_DATA_WRITE);
         rex_sleep(SI4703_PWRUP_TIME);
         
         if((teFmrdsDevRdsStd == RDS_STD_RBDS) || (teFmrdsDevRdsStd == RDS_STD_RDS))
         {
            FmrdsDev_EnableIsr();
         }

         break;
      }
   
      default:
      {
        break;  // with error
      }
   }
   
   return(result);
}


/*******************************************************************************

FUNCTION      FmrdsDev_GetDeviceId(void)

DESCRIPTION   Returns ID info for FM device in use.
              
DEPENDENCIES  None

RETURN VALUE  Returns command status and device id info.

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevIDReturn FmrdsDev_GetDeviceId(void)
{
  // example implementation
  tsFmrdsDevIDReturn tsResult = {FMRDSDEV_ERROR, "XYZ FM Device   ", 0x01, 0x01};
  
  // read device info here...
  tsResult.teDevIDResult = FMRDSDEV_OK;   // ...and return success
  
  return(tsResult);
}


/*******************************************************************************

FUNCTION      FmrdsDev_SetRadioMode(teFmrdsDevRadioModeType fmrdsRadioMode)

DESCRIPTION   Set receive or transmit operating modes (or both) according to
              device capabilities. This function should configure hw areas as
              necessary.
              
DEPENDENCIES  Rx and/or Tx operation will be FM HW device-dependant

RETURN VALUE  Error returned if device does not support specified mode, else ok.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult  FmrdsDev_SetRadioMode(teFmrdsDevRadioModeType fmrdsRadioMode)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  
  // operation here needs to be further rationalised - rx/tx operation tbd
  switch (fmrdsRadioMode)
  {
    case RADIO_MODE_RX: 
    {
      // set rx only
      teFmrdsDevRadioMode = RADIO_MODE_RX;
      result = FMRDSDEV_OK;
      break;
    }
    
#ifdef FM_TX_SUPPORTED
    
    // tx operation supported
    case RADIO_MODE_TX :
    {
      // set tx only
      teFmrdsDevRadioMode = RADIO_MODE_TX;
      result = FMRDSDEV_OK;
      break;
    }
    
    case RADIO_MODE_TX_RX:
    {
      // set rx and tx
      teFmrdsDevRadioMode = RADIO_MODE_TX_RX;
      result = FMRDSDEV_OK;
      break;
    }
    
#else
    
    // no tx supported
    case RADIO_MODE_TX :
    case RADIO_MODE_TX_RX:
    {
      break;    // with error
    }
    
#endif  // FM_TX_SUPPORTED
    
    
    case RADIO_MODE_OFF:
    default:
    {
      break;  // with error
    }
  }
  
  return(result);
}


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
teFmrdsDevResult FmrdsDev_Init(void)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  
  //Initialize data Storage at driver level
  FmrdsDev_InitStorage();

  // base device initialisation here
  tsSi4703ControlRegMap.si4703PwrMode = SI4703_LOW_POWER_MODE;

  //Silab engineers asked not to write to TEST1 register
  //tsSi4703ControlRegMap.si4703XoscenMode = XOSCEN_DISABLE;
  //tsSi4703ControlRegMap.si4703AhizenMode = AHIZEN_DISABLE;
  
  tsSi4703ControlRegMap.si4703Gpio3Mode = GPIO3_HIGH_IMPEDANCE;
  tsSi4703ControlRegMap.si4703IntMode = GPIO2_LOW; 
  tsSi4703ControlRegMap.si4703Gpio1Mode = GPIO1_HIGH_IMPEDANCE;
  tsSi4703ControlRegMap.si4703RdsIntMode = RDS_DISABLE_INT;
  tsSi4703ControlRegMap.si4703StcIntMode = STC_DISABLE_INT; 
  tsSi4703ControlRegMap.si4703AgcDMode = AGC_ENABLE;
  
  tsSi4703ControlRegMap.si4703BlndAdj = BLNDADJ_LEV0;
  tsSi4703ControlRegMap.si4703DsMute = DSMUTE_ENABLE;
  tsSi4703ControlRegMap.si4703SMuteR = SMUTER_FASTEST;
  tsSi4703ControlRegMap.si4703SMuteA = SMUTEA_LEV0;
  tsSi4703ControlRegMap.si4703VolExt = VOLEXT_DISABLE;
  tsSi4703ControlRegMap.si4703SeekTh = (teSi4703SeekThType)SEEKTH_MID_RSSI;
  tsSi4703ControlRegMap.si4703SkSnr = SKSNR_DISABLE;
  tsSi4703ControlRegMap.si4703SkCnt = SKCNT_DISABLE;
  tsSi4703ControlRegMap.si4703RdsMode = RDS_STANDARD_MODE;
  
  tsSi4703ControlRegMap.si4703RdsPwrupMode = RDS_OFF;
  tsSi4703ControlRegMap.si4703DeEmphasis = DE_EMP75;
  tsSi4703ControlRegMap.si4703RadioBand = US_EUROPE_BAND;
  tsSi4703ControlRegMap.si4703ChSpacing = CHSPACE_200_KHZ; // us defaults
  
  tsSi4703ControlRegMap.si4703DMuteMode = MUTE_ENABLE;
  tsSi4703ControlRegMap.si4703StereoMonoMode = AUTO_MONO_STEREO_MODE ;
  tsSi4703ControlRegMap.si4703Volume = FM_VOLUME_LEVEL_5;
  
  tsSi4703ControlRegMap.si4703SeekDir = SEEK_DOWN;
  tsSi4703ControlRegMap.si4703SkMode = SKMODE_CONT_BLMT;
  tsSi4703ControlRegMap.si4703SeekPwrUpMode = SEEK_DISABLE;
  
  tsSi4703ControlRegMap.si4703TunePwrUpMode = TUNE_DISABLE;
  
  tusSi4703CurrentFrequency =SI4703_DEFAULT_CHAN_FREQ;
  tsSi4703ControlRegMap.si4703ChanVal = SI4703_DEFAULT_CHAN_VAL; //using a func
  
  result = FmrdsDev_FillRegs(SIZE_OF_FMRDS_DATA_WRITE);

  // set pwrmode shadow register to correct value after low Pwr
  tsSi4703ControlRegMap.si4703PwrMode = SI4703_AFTER_LOW_PWR;
  
  return(result);
}


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
teFmrdsDevResult FmrdsDev_ConfigureRadio(tsFmrdsDevRadioSettingsType fmrdsRadioSettings)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  boolean bRangeChkResult = TRUE;
  
  //check for out of range parameters
  if(
     ((fmrdsRadioSettings.radioBand < US_EUROPE_BAND) || (fmrdsRadioSettings.radioBand > USER_DEFINED_BAND))
     ||  
     ((fmrdsRadioSettings.deEmphasis < DE_EMP75) || (fmrdsRadioSettings.deEmphasis > DE_EMP50))
     ||
     ((fmrdsRadioSettings.chSpacing < CHSPACE_200_KHZ) || (fmrdsRadioSettings.chSpacing > CHSPACE_50_KHZ))
     || 
     ((fmrdsRadioSettings.rdsStd < RDS_STD_RBDS) || (fmrdsRadioSettings.rdsStd > RDS_STD_NONE))
  )
     bRangeChkResult = FALSE;
  
  if(fmrdsRadioSettings.radioBand == USER_DEFINED_BAND)
  {
     if((fmrdsRadioSettings.bandRange.lowerLimit < JAPAN_BAND_MIN)  
        ||(fmrdsRadioSettings.bandRange.upperLimit > JAPAN_WIDE_BAND_MAX)
        ||(fmrdsRadioSettings.bandRange.lowerLimit >= fmrdsRadioSettings.bandRange.upperLimit))
        bRangeChkResult = FALSE;
  }


  //If all the arguments are in range proceed...
  if(bRangeChkResult)
  {
     tsSi4703ControlRegMap.si4703RadioBand  = fmrdsRadioSettings.radioBand;
     tsSi4703ControlRegMap.si4703DeEmphasis = fmrdsRadioSettings.deEmphasis;
     tsSi4703ControlRegMap.si4703ChSpacing  = fmrdsRadioSettings.chSpacing;
     
     // no rdsStd or bandrange reg in si4703, use this info as required 
     teFmrdsDevRdsStd = fmrdsRadioSettings.rdsStd;
     tsFmrdsDevBandRange = fmrdsRadioSettings.bandRange;
     
          
     if (FmrdsDev_FillRegs(SIZE_OF_CONFIG_DATA_WRITE) == FMRDSDEV_OK)
        result = FmrdsDev_SetRdsMode();
  }

  return(result);
}

/*******************************************************************************

FUNCTION      FmrdsDev_SetRdsMode(void)

DESCRIPTION   Set up the RDS mode & related interrupt settings for its operation.
           
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetRdsMode(void)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  
  // check global RdsPwrupMode and enable/disable rds operation accordingly.
  // in future releases, required operating mode needs to be passed to this
  // function as parameter(s).
  // rdsPwrupMode written to device inside FmrdsDev_EnableIsr\DisableIsr()

  if((teFmrdsDevRdsStd == RDS_STD_RBDS) || (teFmrdsDevRdsStd == RDS_STD_RDS))
  {      
    tsSi4703ControlRegMap.si4703RdsPwrupMode = RDS_ON;
    result = FmrdsDev_EnableIsr(); 
  }
  else
  {
    tsSi4703ControlRegMap.si4703RdsPwrupMode = RDS_OFF;
    result = FmrdsDev_DisableIsr();
  }
  
  return(result);
}

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
                                                       fmrdsRxProcOpt)
{
   teFmrdsDevResult result = FMRDSDEV_OK;
   
   bRawRdsFlag = fmrdsRxProcOpt.rdsReturnRawBlocks;
   
#ifdef QXDM_SI4703_TRACE
   if(bRawRdsFlag == TRUE)
      FMRDS_MSG_HIGH("rdsReturnRawBlocks ON",0,0,0);
   else
      FMRDS_MSG_HIGH("rdsReturnRawBlocks OFF",0,0,0);
#endif


   if((fmrdsRxProcOpt.rdsBufSz >= MIN_APP_RDS_BUFF_SZ) && 
      (fmrdsRxProcOpt.rdsBufSz <= MAX_APP_RDS_BUFF_SZ))
   {
      usRdsBufSz = fmrdsRxProcOpt.rdsBufSz;
   }
   else
   {
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_ERROR("Incorrect RDS buffer size %d",fmrdsRxProcOpt.rdsBufSz,0,0);
#endif
      usRdsBufSz = MAX_APP_RDS_BUFF_SZ;
      result = FMRDSDEV_ERROR;
   }

   //We are maintaining 2 circular ping-pong buffers
   usRdsMaxBufSz = usRdsBufSz * 2;


   return(result);
}



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
                                                               fmrdsAudioLevel)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;

  if((fmrdsAudioLevel <= FM_VOLUME_LEVEL_0) && (fmrdsAudioLevel >= FM_VOLUME_LEVEL_15)) 
     return(result);
  
  // set audio level
  tsSi4703ControlRegMap.si4703Volume = fmrdsAudioLevel;
  
  result = FmrdsDev_FillRegs(SIZE_OF_VOLUME_DATA_WRITE);
  
  return(result);
}


/*******************************************************************************

FUNCTION      FmrdsDev_SetMuteMode(teFmrdsDevMuteType fmrdsMuteMode)

DESCRIPTION   Set audio muting for left and right channels
              
              Settings data can be straight pass-through from API layer above.

DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None    NOMUTE     = 0,
   MUTELEFT   = 1,
   MUTERIGHT  = 2,
   MUTEBOTH   = 3 

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetMuteMode(teFmrdsDevMuteType fmrdsMuteMode)
{ 
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  switch(fmrdsMuteMode)
  {
    case NOMUTE:
    {
      tsSi4703ControlRegMap.si4703DMuteMode = MUTE_DISABLE;
      result = FmrdsDev_FillRegs(SIZE_OF_MUTE_DATA_WRITE);
      break;
    }
    
    case MUTEBOTH:
    {
      tsSi4703ControlRegMap.si4703DMuteMode = MUTE_ENABLE;
      result = FmrdsDev_FillRegs(SIZE_OF_MUTE_DATA_WRITE);
      break;
    }
    
    case MUTELEFT:
    case MUTERIGHT:
    {
      // no separate mute on 4703 
      break;
    }
    
    default:
    {
      break;  // with error
    }
  }
  
  return(result);
}


/*******************************************************************************

FUNCTION      FmrdsDev_SetStereoMode(teFmrdsDevStereoType fmrdsStereoMode)

DESCRIPTION   Set stereo or mono audio operation.
              
              Settings data can be straight pass-through from API layer above.

DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetStereoMode(teFmrdsDevStereoType fmrdsStereoMode)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;

  if((fmrdsStereoMode == (teFmrdsDevStereoType) AUTO_MONO_STEREO_MODE) || 
     (fmrdsStereoMode == (teFmrdsDevStereoType) FORCE_MONO_ONLY_MODE))
  {
     tsSi4703ControlRegMap.si4703StereoMonoMode = fmrdsStereoMode;
     result = FmrdsDev_FillRegs(SIZE_OF_STEREO_DATA_WRITE);
  }
  
  return(result);
}


//
// basic operation
//

/*******************************************************************************

FUNCTION      FmrdsDev_SetStation(tusFmrdsDevFreqType fmrdsFmrdsFreq)

DESCRIPTION   Tune FM receiver to specified channel. Frequency is in 0.01Mhz
              steps, giving basic bands a valid range of 8750 - 10800 for
              US/Europe, 7600 - 9100 for Japan.

              Note: uses seek/tune time timing as specified in silabs AN230
              application note.
              
DEPENDENCIES  None

RETURN VALUE  Returns ok or error status.

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_SetStation(tusFmrdsDevFreqType fmrdsFmrdsFreq)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  tusFmrdsDevFreqType fmrdsChanVal;
  uint8 aucread_data_byte = 0;
  
  result = FmrdsDev_ChanValDecode(fmrdsFmrdsFreq, &fmrdsChanVal);
  
  if(result == FMRDSDEV_OK)
  {
#ifdef QXDM_SI4703_TRACE
    FMRDS_MSG_HIGH("FMRDS: Tunetochan value = %d for frequency- %d", 
      fmrdsChanVal,fmrdsFmrdsFreq,0);
#endif 
    //if tune bit is set due to prev. tune operation disable it
    tsSi4703ControlRegMap.si4703TunePwrUpMode = TUNE_DISABLE;
    result = FmrdsDev_FillRegs(SIZE_OF_SET_STATION_DATA_WRITE);
    if (result != FMRDSDEV_OK)
      return(result);
    
    //set channel value for desired frequency & enable tune bit
    tsSi4703ControlRegMap.si4703ChanVal = fmrdsChanVal;
    tsSi4703ControlRegMap.si4703TunePwrUpMode = TUNE_ENABLE;
    result = FmrdsDev_FillRegs(SIZE_OF_SET_STATION_DATA_WRITE);
    if(result == FMRDSDEV_OK)
    {
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("FMRDS: TUNE_ENABLE Reg set",0,0,0);
#endif
      rex_sleep(SI4703_SEEKTUNE_TIME);        //As recommended in AN230
      result = FmrdsDev_ReadData(FMRDS_SI4703_2WIRE_ID, 1, 
        &aucread_data_byte); 
      if(result == FMRDSDEV_OK)
      {
        tsSi4703StatusRegMap.si4703StcStatus = (teSi4703StcStatusType)(
          ((aucread_data_byte & 0x40)>>6)
          );
        if(tsSi4703StatusRegMap.si4703StcStatus == STC_COMPLETE)
        {
#ifdef QXDM_SI4703_TRACE
          FMRDS_MSG_HIGH("FMRDS: STC Status bit set",0,0,0);
#endif
          //added rex_sleep(1) bcoz FTM_FMRDS_SET_STATION was failing
          //at this point with comm error. 
          rex_sleep(1); 
          
          tsSi4703ControlRegMap.si4703TunePwrUpMode = TUNE_DISABLE;
          result = FmrdsDev_FillRegs(SIZE_OF_SET_STATION_DATA_WRITE); 
          rex_sleep(SI4703_SEEKTUNE_TIME); //time to allow STC status bit to clear        
#ifdef QXDM_SI4703_TRACE
          FMRDS_MSG_HIGH("FMRDS: Command issued to disable tune bit after STC",0,0,0);
#endif
        }            
      }
    }
    //Tuned to a new station;clear rds data buffers
    FmrdsDev_ClearRdsBuffers();
  }
  
  return(result);
}


/*******************************************************************************

FUNCTION      FmrdsDev_GetCurrentSettings(void)

DESCRIPTION   Return current FM device settings and status. This function should
              provide necessary info passed up within API events.

              For more instantaneous values, use GetTuneStatus( )
              
DEPENDENCIES  None

RETURN VALUE  tsFmrdsDevDeviceSettingsType

SIDE EFFECTS  None
  
*******************************************************************************/
tsFmrdsDevDeviceSettingsType FmrdsDev_GetCurrentSettings(void)
{
  tsFmrdsDevDeviceSettingsType result = \
  { US_EUROPE_BAND, DE_EMP75, CHSPACE_200_KHZ, RDS_STD_RBDS, US_EUROPE_BAND_MIN, US_EUROPE_BAND_MAX, \
    RADIO_MODE_RX, RADIO_CAP_RX, FM_RX_ON, RDS_ON, FMRDS_NORMAL_MODE, NOMUTE, \
    AUTO_MONO_STEREO_MODE, FM_VOLUME_LEVEL_0, US_EUROPE_BAND_MIN, \
    0, 0, FALSE, FALSE, FALSE, FALSE, FALSE };   // sane init values

  result.radioSettings.radioBand  = tsSi4703ControlRegMap.si4703RadioBand;
  result.radioSettings.deEmphasis = tsSi4703ControlRegMap.si4703DeEmphasis;
  result.radioSettings.chSpacing  = tsSi4703ControlRegMap.si4703ChSpacing;
  result.radioSettings.rdsStd  = teFmrdsDevRdsStd;
  result.radioSettings.bandRange = tsFmrdsDevBandRange;
  
  //All 3 related to rx tx mode 
  result.radioMode = teFmrdsDevRadioMode ;
  result.radioCap = RADIO_CAP_RX;       // note: hard-coded for this implementation
  result.fmPwrUpMode = FM_RX_ON;        // note: hard-coded for this implementation
  
  
  result.rdsPwrUpMode = tsSi4703ControlRegMap.si4703RdsPwrupMode;
  
  if(tsSi4703ControlRegMap.si4703PwrMode == SI4703_NORMAL_MODE)
    result.pwrMode = FMRDS_NORMAL_MODE;
  else
    result.pwrMode = FMRDS_LOW_POWER_MODE;
  
  
  if(tsSi4703ControlRegMap.si4703DMuteMode == MUTE_ENABLE)
    result.muteMode = MUTEBOTH; 
  else
    result.muteMode = NOMUTE;
  
  result.monoStereo = tsSi4703ControlRegMap.si4703StereoMonoMode;
  result.rxAudioLevel = tsSi4703ControlRegMap.si4703Volume;
  result.stationFreq = tusSi4703CurrentFrequency; 

  result.rawRdsProcOpts.rdsReturnRawBlocks = bRawRdsFlag;
  result.rawRdsProcOpts.rdsBufSz = usRdsBufSz;
  
  return(result);
}


/*******************************************************************************

FUNCTION      FmrdsDev_GetTuneStatus(void)

DESCRIPTION   Function returns the more instantaneous fm rds values: frequency,
              lock and rssi status of currently tuned channel.
              
DEPENDENCIES  None

RETURN VALUE  tsFmrdsDevLockSyncReturn

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevLockSyncReturn FmrdsDev_GetTuneStatus(void)
{
  tsFmrdsDevLockSyncReturn result = {FMRDSDEV_ERROR, US_EUROPE_BAND_MIN, FMRDS_UNLOCKED,
                                        FMRDS_NORDSSYNC, 0, 0, FALSE, FM_MONO_PROGRAM};
  uint8 aucread_data_byte[2];

  result.teDevLockSyncResult = FmrdsDev_ReadData(FMRDS_SI4703_2WIRE_ID, 
                         SIZE_OF_GET_TUNE_STATUS, aucread_data_byte);

  //if read fails
  if(result.teDevLockSyncResult != FMRDSDEV_OK)
     return(result);

  tsSi4703StatusRegMap.si4703AfcRlStatus = 
            (teSi4703AfcRlStatusType)((aucread_data_byte[0] & 0x10) >> 4);
  tsSi4703StatusRegMap.si4703RdsSStatus  = 
            (teSi4703RdsSStatusType) ((aucread_data_byte[0] & 0x08) >> 3);
  tsSi4703StatusRegMap.rssiStatus = (uint8) (aucread_data_byte[1] & 0xFF);
  tsSi4703StatusRegMap.si4703StStatus = (teFmrdsDevStStateType)(
                                        aucread_data_byte[0] & 0x01);



  // update frequency...
  result.tsDevLockSyncStatus.tusDevFreq = tusSi4703CurrentFrequency; 
  
  // update station locked...
  if((tsSi4703StatusRegMap.si4703AfcRlStatus == AFC_NOT_RAILED)) 
  {
     // locked to valid frequency
     result.tsDevLockSyncStatus.teDevLockState = FMRDS_LOCKED; 
     
#ifdef QXDM_SI4703_TRACE
     FMRDS_MSG_HIGH("FMRDS: PLL Locked",0,0,0);
#endif
  }
  else
  {
#ifdef QXDM_SI4703_TRACE
        FMRDS_MSG_HIGH("FMRDS: PLL not Locked",0,0,0);
#endif
     result.tsDevLockSyncStatus.teDevLockState = FMRDS_UNLOCKED;
  }

  // update rds synch status...
  if(tsSi4703ControlRegMap.si4703RdsMode == RDS_VERBOSE_MODE)
  {
     // verbose mode, check for sync
     if(tsSi4703StatusRegMap.si4703RdsSStatus == RDS_SYNCED)
     {
#ifdef QXDM_SI4703_TRACE
        FMRDS_MSG_HIGH("FMRDS: RDS_SYNCHED",0,0,0);
#endif
        result.tsDevLockSyncStatus.teDevRdsSyncState = FMRDS_RDSSYNC;
     }
     else
     {
#ifdef QXDM_SI4703_TRACE
        FMRDS_MSG_HIGH("FMRDS: RDS_NOT_SYNCHED",0,0,0);
#endif
        result.tsDevLockSyncStatus.teDevRdsSyncState = FMRDS_NORDSSYNC;
     }
  }
  else
  {
     // non-verbose mode: every block passed is checked, implies synched.
     // note: hard-code synch'd for this implementation
     result.tsDevLockSyncStatus.teDevRdsSyncState = FMRDS_RDSSYNC;
  } 

  // update other fields...
  result.tsDevLockSyncStatus.tucDevRSSIVal = tsSi4703StatusRegMap.rssiStatus;
  result.tsDevLockSyncStatus.teDevStState = (teFmrdsDevStStateType)tsSi4703StatusRegMap.
                                            si4703StStatus;
#ifdef QXDM_SI4703_TRACE
  if(result.tsDevLockSyncStatus.teDevStState == FM_STEREO_PROGRAM)
        FMRDS_MSG_HIGH("FMRDS: Stereo Audio played now",0,0,0);
  else 
        FMRDS_MSG_HIGH("FMRDS: Mono Audio played now",0,0,0);
#endif
  result.tsDevLockSyncStatus.ucDevIfcnt = 0;        // note: hard-coded in this implementation
  result.tsDevLockSyncStatus.rdsCapability = TRUE;  // note: hard-coded in this implementation
  
  return(result);
}



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
tsFmrdsDevSeekReturn FmrdsDev_StationSeek(tsFmrdsDevRadioSrchModeType fmrdsSeekParams)
{
  tsFmrdsDevSeekReturn result = { FMRDSDEV_OK, \
                                    US_EUROPE_BAND_MIN, 0, FM_STEREO_PROGRAM }; // sane init values

  uint8 aucread_data_byte[4], ucTempPollCount;
  tusFmrdsDevFreqType tempChanVal, currentFreqVal;
  
  //check station search mode
  if(fmrdsSeekParams.srchMode != SEARCHMODE_SEEK)
  {
    result.teDevSeekResult = FMRDSDEV_ERROR;
  }
  
  //set station search direction
  if (fmrdsSeekParams.srchDir == SEARCHDIR_DOWN)
  {
    tsSi4703ControlRegMap.si4703SeekDir = SEEK_DOWN;
  }
  else if (fmrdsSeekParams.srchDir == SEARCHDIR_UP)
  {
    tsSi4703ControlRegMap.si4703SeekDir = SEEK_UP;
  }
  else
  {
    result.teDevSeekResult = FMRDSDEV_ERROR;
  }
  
  // set RSSI level
  tsSi4703ControlRegMap.si4703SeekTh = (teSi4703SeekThType)fmrdsSeekParams.srchRssiLev;
  
  // Set search mode
  if(fmrdsSeekParams.srchWrap == WRAP_AT_END_OF_BAND)
  {
    tsSi4703ControlRegMap.si4703SkMode = SKMODE_CONT_BLMT; //default
  }
  else if(fmrdsSeekParams.srchWrap == NO_BAND_WRAP)
  {
    tsSi4703ControlRegMap.si4703SkMode = SKMODE_STOP_BLMT;
  }
  else
  {
    result.teDevSeekResult = FMRDSDEV_ERROR;
  }
  
  //return error argument values do not satisfy requirement
  if(result.teDevSeekResult != FMRDSDEV_OK)
    return(result);
  
  tsSi4703ControlRegMap.si4703SkSnr = SKSNR_VAL; 
  tsSi4703ControlRegMap.si4703SkCnt = SKCNT_VAL; 
  
  tsSi4703ControlRegMap.si4703SeekPwrUpMode = SEEK_ENABLE;
  result.teDevSeekResult = FmrdsDev_FillRegs(SIZE_OF_SRCH_SEEK_DATA_WRITE);
  
  //return error if write to device fails
  if(result.teDevSeekResult != FMRDSDEV_OK)
    return(result);
  
#ifdef QXDM_SI4703_TRACE
  FMRDS_MSG_HIGH("SEEK ENABLE START",0,0,0);
#endif
  
  
  tsSi4703StatusRegMap.si4703StcStatus = STC_NOT_COMPLETE;
  ucTempPollCount = MIN_SEEK_POLL_COUNT; 
  
  while(tsSi4703StatusRegMap.si4703StcStatus != STC_COMPLETE)
  {
    rex_sleep(SI4703_SEEKTUNE_TIME);//As recommended in AN230
    
    result.teDevSeekResult = FmrdsDev_ReadData(FMRDS_SI4703_2WIRE_ID, 4, aucread_data_byte);
    
    //return error if read data fails
    if(result.teDevSeekResult != FMRDSDEV_OK)
      return(result);
    
    // read registers
    tsSi4703StatusRegMap.si4703StcStatus = (teSi4703StcStatusType)(
      ((aucread_data_byte[0] & 0x40)>>6)
      );
    
    //Max limit on polling so that it does not get stuck here 
    //if I2C comm fails
    if(ucTempPollCount == MAX_SEEK_POLL_COUNT)
    {
      tsSi4703StatusRegMap.si4703StcStatus = STC_NOT_COMPLETE;
      break;
    }
    
    ucTempPollCount++;
  }
  
  // read SF/BL register
  tsSi4703StatusRegMap.si4703SfStatus =(teSi4703SfStatusType)(
    ((aucread_data_byte[0] & 0x20)>> 5)
    );
  
  //Check if seek failed or bandlimit reached
  if((tsSi4703StatusRegMap.si4703StcStatus == STC_COMPLETE) &&
    (tsSi4703StatusRegMap.si4703SfStatus == SEEK_SUCCESS)
    )
  {
    //service available
    result.bandLimitReached = FALSE;
#ifdef QXDM_SI4703_TRACE
    FMRDS_MSG_HIGH("SEEK SUCCESSFUL",0,0,0);
#endif      
    //Found a new station;clear rds data buffers
    FmrdsDev_ClearRdsBuffers();
  }
  else
  {
    //service not available
    result.bandLimitReached = TRUE;
#ifdef QXDM_SI4703_TRACE
    FMRDS_MSG_HIGH("SEEK NOT SUCCESSFUL",0,0,0);
#endif
  }
  
  // Decode frequency from read chanval
  tsSi4703StatusRegMap.currentChanVal = (tusFmrdsDevFreqType)(
    (aucread_data_byte[3] & 0xFF)
    );
  tempChanVal = tsSi4703StatusRegMap.currentChanVal;
  result.teDevSeekResult = FmrdsDev_FreqDecode(tempChanVal, &currentFreqVal);
#ifdef QXDM_SI4703_TRACE
  FMRDS_MSG_HIGH("CHANVAL-%d, FREQUENCY-%d",tempChanVal,currentFreqVal,0);
#endif       
  
  //Read other status fields
  tsSi4703StatusRegMap.si4703StStatus = (teFmrdsDevStStateType)(
    aucread_data_byte[0] & 0x01);
  tsSi4703StatusRegMap.rssiStatus = (uint8)(aucread_data_byte[1] & 0xFF);
  
#ifdef QXDM_SI4703_TRACE
  FMRDS_MSG_HIGH("STEREO-%d RSSI-%d",tsSi4703StatusRegMap.si4703StStatus,
    tsSi4703StatusRegMap.rssiStatus,0);
#endif       
  
  //Seek operation is complete so disable seek bit
  rex_sleep(1);
  tsSi4703ControlRegMap.si4703SeekPwrUpMode = SEEK_DISABLE;
  result.teDevSeekResult = FmrdsDev_FillRegs(SIZE_OF_ONLY_SEEK_DATA_WRITE);
  rex_sleep(SI4703_SEEKTUNE_TIME); //time to allow STC status bit to clear 
#ifdef QXDM_SI4703_TRACE
  FMRDS_MSG_HIGH("SEEK COMPLETE",0,0,0);
#endif
  
  //Fill in the return result
  result.tusDevTuneStatus.stationFreq = currentFreqVal; 
  result.tusDevTuneStatus.rssi = tsSi4703StatusRegMap.rssiStatus;
  result.tusDevTuneStatus.stereoProgram = tsSi4703StatusRegMap.si4703StStatus;
  
  return(result);
  
}


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
teFmrdsDevResult FmrdsDev_CancelSeek(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   
   return(result);
}


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
teFmrdsDevUpdateStatus FmrdsDev_GetDeviceUpdate(void)
{
  uint8 aucread_data_byte[12];
  teFmrdsDevUpdateStatus updateStatus = FMRDS_UPDATE_DONE;  // default return
  
  // disable Fm Rds Isr
  FmrdsDev_DisableIsr();
  
  // read data from device
  if( (FmrdsDev_ReadData(FMRDS_SI4703_2WIRE_ID,SIZE_OF_RDS_DATA_STATUS, 
    aucread_data_byte)) == FMRDSDEV_OK)
  {
    tsSi4703RdsDataRegMap.usBlockA = (((aucread_data_byte[4] & 0xFF) << 8) |
      (aucread_data_byte[5] & 0xFF) 
      );
    tsSi4703RdsDataRegMap.usBlockB = (((aucread_data_byte[6] & 0xFF) << 8) |
      (aucread_data_byte[7] & 0xFF) 
      );
    tsSi4703RdsDataRegMap.usBlockC = (((aucread_data_byte[8] & 0xFF) << 8) |
      (aucread_data_byte[9] & 0xFF) 
      );
    tsSi4703RdsDataRegMap.usBlockD = (((aucread_data_byte[10] & 0xFF) << 8) |
      (aucread_data_byte[11] & 0xFF) 
      );

    if(bRawRdsFlag == TRUE)
    {
       //Copy Received data
       updateStatus = FmrdsDev_CopyRdsData();
    }
    else
    {
       // decode Received data
       FmrdsDev_DecodeRdsData();
    }
  }
  
  // enable Isr
  FmrdsDev_EnableIsr();
  
  return(updateStatus);
}


//
// interrupt handling
//

/*******************************************************************************

FUNCTION      teFmrdsDevResult FmrdsDev_EnableIsr(void)

DESCRIPTION   Enable device interrupt(s).
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_EnableIsr(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;

#ifdef QXDM_SI4703_TRACE
   FMRDS_MSG_HIGH("FMRDS: Enabled FMRDS ISR",0,0,0);
#endif
   
   // enable platform interrupts
#if (defined(T_QSC1100) || defined(T_QSC1110))

   (void) gpio_int_set_detect(FMRDS_INT_GPIO, DETECT_EDGE);
   (void) gpio_int_set_handler(FMRDS_INT_GPIO, ACTIVE_LOW, FmrdsDev_Isr);

#endif  // T_QSC1100
   
   
   tsSi4703ControlRegMap.si4703RdsIntMode = RDS_ENABLE_INT;
   tsSi4703ControlRegMap.si4703IntMode = GPIO2_INT_ENABLE;

   result = FmrdsDev_FillRegs(SIZE_OF_GPIO2_INT_DATA_WRITE);
   
   return(result);
}


/*******************************************************************************

FUNCTION      teFmrdsDevResult FmrdsDev_DisableIsr(void)

DESCRIPTION   Disable device interrupt(s).
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevResult FmrdsDev_DisableIsr(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;

#ifdef QXDM_SI4703_TRACE
   FMRDS_MSG_HIGH("FMRDS: Disabled FMRDS ISR",0,0,0);
#endif
   
   // disable platform interrupts
#if (defined(T_QSC1100) || defined(T_QSC1110))

   (void) gpio_int_set_handler(FMRDS_INT_GPIO, ACTIVE_LOW, NULL);

#endif  // T_QSC110
   
   tsSi4703ControlRegMap.si4703RdsIntMode = RDS_DISABLE_INT;
   tsSi4703ControlRegMap.si4703IntMode = GPIO2_LOW;

   result = FmrdsDev_FillRegs(SIZE_OF_GPIO2_INT_DATA_WRITE );
   
   return(result);
}


/*******************************************************************************

FUNCTION      void FmrdsDev_Isr(void)

DESCRIPTION   Device interrupt handler. Primarily to be for retrieval of
              RDS block or group data.
                            
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_Isr(void)
{
   // Disable other interrupts 
   dword dwResult = rex_int_lock();

   // I2C access not available from ISR, so RDSIEN read\write not possible
   // Only deregister FMRDS int handler 
   // enable platform interrupts
#if (defined(T_QSC1100) || defined(T_QSC1110))

   (void) gpio_int_set_handler(FMRDS_INT_GPIO, ACTIVE_LOW, NULL);

#endif  // T_QSC1100
   
   if(rex_is_in_irq_mode())
   {
     // signal rds data present to fmrds task
     (void) rex_set_sigs( &fmrds_tcb, FMRDS_UPDATE_RDS_SIG);
#ifdef QXDM_SI4703_TRACE
     FMRDS_MSG_HIGH("FMRDS: FmrdsDev_Isr: Set signal to fmrds_task", 0, 0, 0);
#endif
   }

   /* Resume Interrupts */
   if(!dwResult) (void) rex_int_free();

   return;
}


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
                                         uint8 ucNoOfBytes, uint8 *pucByteBuff)
{
   teFmrdsDevResult result = FMRDSDEV_COMMS_ERROR;
   static i2c_rw_cmd_type fmrds_2wire_command;
   i2c_rw_cmd_type    *pfmrds_2wire_command = NULL;
   
   fmrds_2wire_command.bus_id = I2C_BUS_HW_CTRL;
   fmrds_2wire_command.options = I2C_DFLT_ADDR_DEV;
   fmrds_2wire_command.slave_addr = ucSlaveAddress;
   fmrds_2wire_command.buf_ptr = pucByteBuff;
   fmrds_2wire_command.len = ucNoOfBytes;
   pfmrds_2wire_command = &fmrds_2wire_command;
   
   if (i2c_read(pfmrds_2wire_command) == I2C_SUCCESS)
   {
      result = FMRDSDEV_OK;
   }
   else
   {  //Retry if i2c_read fails the 1st time
      if (i2c_read(pfmrds_2wire_command) == I2C_SUCCESS)
      {
         result = FMRDSDEV_OK;
      }
      else
      {
         FMRDS_MSG_ERROR("FMRDS: FmrdsDev_ReadData Failed",0,0,0);
      }
   }  
   
   return(result);
}


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
                                          uint8 ucNoOfBytes, uint8 *pucByteBuff)
{
   teFmrdsDevResult result = FMRDSDEV_COMMS_ERROR;
   static i2c_rw_cmd_type fmrds_2wire_command;
   i2c_rw_cmd_type    *pfmrds_2wire_command = NULL;
   
   fmrds_2wire_command.bus_id = I2C_BUS_HW_CTRL;
   fmrds_2wire_command.options = I2C_DFLT_ADDR_DEV;
   fmrds_2wire_command.slave_addr = ucSlaveAddress;
   fmrds_2wire_command.buf_ptr = pucByteBuff;
   fmrds_2wire_command.len = ucNoOfBytes;
   pfmrds_2wire_command = &fmrds_2wire_command;
   
   if (i2c_write(pfmrds_2wire_command) == I2C_SUCCESS)
   {
      result = FMRDSDEV_OK;
   }
   else
   { //Retry if i2c_write fails the 1st time
      if (i2c_write(pfmrds_2wire_command) == I2C_SUCCESS)
      {
         result = FMRDSDEV_OK;
      }
      else
      {
         FMRDS_MSG_ERROR("FMRDS: FmrdsDev_WriteData Failed",0,0,0);
      }
   }
   
   return(result);
}


/*******************************************************************************

FUNCTION     FmrdsDev_FillRegs(uint8 ucNoOfBytes)

DESCRIPTION  TBC
             For 4703 device, uses 2-wire/I2C communications.

DEPENDENCIES None

RETURN VALUE Returns ok or error status. 

SIDE EFFECTS None 

*******************************************************************************/
teFmrdsDevResult FmrdsDev_FillRegs(uint8 ucNoOfBytes)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   static uint8 aucwrite_data_byte[SIZE_OF_FMRDS_DATA_WRITE];
   
   aucwrite_data_byte[0] = (
             (((uint8)(tsSi4703ControlRegMap.si4703DsMute)&(0x01))<<7) |
             (((uint8)(tsSi4703ControlRegMap.si4703DMuteMode)&(0x01))<<6) | 
             (((uint8)(tsSi4703ControlRegMap.si4703StereoMonoMode)&(0x01))<<5) |
             (((uint8)(tsSi4703ControlRegMap.si4703RdsMode)&(0x01))<<3) | 
             (((uint8)(tsSi4703ControlRegMap.si4703SkMode)&(0x01))<<2) | 
             (((uint8)(tsSi4703ControlRegMap.si4703SeekDir)&(0x01))<<1) |
             ((uint8)(tsSi4703ControlRegMap.si4703SeekPwrUpMode)&(0x01))
                           );
   
   aucwrite_data_byte[1] = (
             ((uint8)(tsSi4703ControlRegMap.si4703PwrMode)&(0xFF))
                           );
   
   aucwrite_data_byte[2] = (
             (((uint8)(tsSi4703ControlRegMap.si4703TunePwrUpMode)&(0x01))<<7) |
             (((uint8)(tsSi4703ControlRegMap.si4703ChanVal)&(0x300))>>7) 
                           );
   
   aucwrite_data_byte[3] = (
             ((uint8)(tsSi4703ControlRegMap.si4703ChanVal)&(0x0FF)) 
                           );
   
   aucwrite_data_byte[4] = (
             (((uint8)(tsSi4703ControlRegMap.si4703RdsIntMode)&(0x01))<<7) |
             (((uint8)(tsSi4703ControlRegMap.si4703StcIntMode)&(0x01))<<6) |
             (((uint8)(tsSi4703ControlRegMap.si4703RdsPwrupMode)&(0x01))<<4) |
             (((uint8)(tsSi4703ControlRegMap.si4703DeEmphasis)&(0x01))<<3) | 
             (((uint8)(tsSi4703ControlRegMap.si4703AgcDMode)&(0x01))<<2) 
                           );
   
   aucwrite_data_byte[5] = (
             (((uint8)(tsSi4703ControlRegMap.si4703BlndAdj)&(0x03))<<6) |
             (((uint8)(tsSi4703ControlRegMap.si4703Gpio3Mode)&(0x03))<<4) |
             (((uint8)(tsSi4703ControlRegMap.si4703IntMode)&(0x03))<<2) |
             ((uint8)(tsSi4703ControlRegMap.si4703Gpio1Mode)&(0x03))           
                           );
   
   aucwrite_data_byte[6] = (
             ((uint8)(tsSi4703ControlRegMap.si4703SeekTh)&(0x0FF)) 
                           );
   
   aucwrite_data_byte[7] = (
             (((uint8)(tsSi4703ControlRegMap.si4703RadioBand)&(0x03))<<6) |
             (((uint8)(tsSi4703ControlRegMap.si4703ChSpacing)&(0x03))<<4) |
             ((uint8)(tsSi4703ControlRegMap.si4703Volume)&(0x0F))            
                           );
   
   aucwrite_data_byte[8] = (
             (((uint8)(tsSi4703ControlRegMap.si4703SMuteR)&(0x03))<<6) |
             (((uint8)(tsSi4703ControlRegMap.si4703SMuteA)&(0x03))<<4) |
             ((uint8)(tsSi4703ControlRegMap.si4703VolExt)&(0x01))            
                           );
   
   aucwrite_data_byte[9] = (
             (((uint8)(tsSi4703ControlRegMap.si4703SkSnr)&(0x0F))<<4) |
             ((uint8)(tsSi4703ControlRegMap.si4703SkCnt)&(0x0F))
                           );
   
   // Silab engineers asked not to write to TEST1 register
   /*aucwrite_data_byte[10] = (
             (((uint8)(tsSi4703ControlRegMap.si4703XoscenMode)&(0x01))<<7) |
             (((uint8)(tsSi4703ControlRegMap.si4703AhizenMode)&(0x01))<<6)
                            );*/
   
   result = FmrdsDev_WriteData(FMRDS_SI4703_2WIRE_ID,
                                          ucNoOfBytes, &aucwrite_data_byte[0]);
   
   return(result); 
}


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
                                     tusFmrdsDevFreqType *outputFmrdsChanVal)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   float tempFmrdsChanVal;
   uint8 ucTempChanSpace = 200;
   
   if(tsSi4703ControlRegMap.si4703RadioBand == US_EUROPE_BAND)
   {
      if((inputFmrdsFreq < US_EUROPE_BAND_MIN) || 
         (inputFmrdsFreq > US_EUROPE_BAND_MAX))
      {
         FMRDS_MSG_ERROR("FMRDS: Invalid frequency %d", inputFmrdsFreq, 0, 0);
         return(result);
      }
      else
      {
         /* Freq(MHz)  = Spacing(KHz) x Channel + 8750(MHz) */
         tempFmrdsChanVal = (float)((inputFmrdsFreq - US_EUROPE_BAND_MIN) * 10); 
      }
   }
   else if((tsSi4703ControlRegMap.si4703RadioBand == JAPAN_WIDE_BAND) || 
      (tsSi4703ControlRegMap.si4703RadioBand == JAPAN_STANDARD_BAND))
   {
      if(
         ((tsSi4703ControlRegMap.si4703RadioBand == JAPAN_WIDE_BAND) && 
          (inputFmrdsFreq > JAPAN_WIDE_BAND_MAX)) ||
         ((tsSi4703ControlRegMap.si4703RadioBand == JAPAN_STANDARD_BAND) && 
          (inputFmrdsFreq > JAPAN_STND_BAND_MAX)) ||
         (inputFmrdsFreq<JAPAN_BAND_MIN)
         )
      {
         FMRDS_MSG_ERROR("FMRDS: Invalid frequency %d", inputFmrdsFreq, 0, 0);
         return(result);
      }
      else
      {
         /* Freq(MHz)  = Spacing(KHz) x Channel + 7600(MHz) */
         tempFmrdsChanVal = (float)((inputFmrdsFreq - JAPAN_BAND_MIN) * 10);
      }
   }
   else
   {
      if((inputFmrdsFreq < tsFmrdsDevBandRange.lowerLimit) || 
         (inputFmrdsFreq > tsFmrdsDevBandRange.upperLimit))
      {
         FMRDS_MSG_ERROR("FMRDS: Invalid frequency %d", inputFmrdsFreq, 0, 0);
         return(result);
      }
      else
      {
          tempFmrdsChanVal = (float)((inputFmrdsFreq - 
                             tsFmrdsDevBandRange.lowerLimit) * 10);
      }

   }

   switch(tsSi4703ControlRegMap.si4703ChSpacing)
   {
      case CHSPACE_200_KHZ:
      {
         ucTempChanSpace = 200;
         break;
      }

      case CHSPACE_100_KHZ:
      {
          ucTempChanSpace = 100;
          break;
      }

      case CHSPACE_50_KHZ:
      {
          ucTempChanSpace = 50;
          break;
      }
      
      default:
      {
          // use default init chan spacing 
          ucTempChanSpace = 200;
          break;
      }
   }

   *outputFmrdsChanVal = (uint16) (tempFmrdsChanVal / ucTempChanSpace);

   tusSi4703CurrentFrequency = inputFmrdsFreq;
   result = FMRDSDEV_OK;

   return(result);
}

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
                                     tusFmrdsDevFreqType *outputFmrdsFreqVal)
{
  teFmrdsDevResult result = FMRDSDEV_ERROR;
  uint8 ucTempChanSpace = 200; 
  uint32 minFreq, tempFreq;
  
  switch(tsSi4703ControlRegMap.si4703ChSpacing)
  {
  case CHSPACE_200_KHZ:
    {
      ucTempChanSpace = 200;
      break;
    }
    
  case CHSPACE_100_KHZ:
    {
      ucTempChanSpace = 100;
      break;
    }
    
  case CHSPACE_50_KHZ:
    {
      ucTempChanSpace = 50;
      break;
    }
    
  default:
    {
      // use default init chan spacing 
      ucTempChanSpace = 200;
      break;
    }
  }
  
  
  if(tsSi4703ControlRegMap.si4703RadioBand == US_EUROPE_BAND)
  {
    minFreq = US_EUROPE_BAND_MIN ;
  }
  else if((tsSi4703ControlRegMap.si4703RadioBand == JAPAN_WIDE_BAND) || 
    (tsSi4703ControlRegMap.si4703RadioBand == JAPAN_STANDARD_BAND))
  {
    minFreq = JAPAN_BAND_MIN;
  }
  else
  {
    minFreq = tsFmrdsDevBandRange.lowerLimit;
  }
  
  minFreq = (minFreq * 10);
  
  tempFreq = (ucTempChanSpace * inputFmrdsChanVal); 
  
  tempFreq = (tempFreq + minFreq);
  
  tusSi4703CurrentFrequency =  (tusFmrdsDevFreqType)(tempFreq/10);
  
  *outputFmrdsFreqVal = tusSi4703CurrentFrequency;
  
  result = FMRDSDEV_OK;
  return(result);
}


//
// rds block/group processing routines
// at present, these all operate on the statically-defined buffers above
//

/*******************************************************************************

FUNCTION      FmrdsDev_DecodeRdsData(void)

DESCRIPTION   Function decodes the RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_DecodeRdsData(void)
{ 
   FmrdsDev_ProcessBlockA(); 
   FmrdsDev_ProcessBlockB();
   FmrdsDev_ProcessBlockC();
   FmrdsDev_ProcessBlockD();

   return;
}


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockA(void)

DESCRIPTION   Function decodes BLOCK-A RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockA(void)
{
  uint16  usTempBlockData;
  uint16 usStage1PiCode, usTempPiCode = 0, x, y, z;
  
  usTempBlockData = tsSi4703RdsDataRegMap.usBlockA;
  
  // Get PI 
  usRdsPi  = usTempBlockData;
  
#ifdef QXDM_SI4703_TRACE
  FMRDS_MSG_HIGH("FMRDS: RDS PI = 0x%x",usRdsPi,0,0);
#endif
  
  if(teFmrdsDevRdsStd == RDS_STD_RBDS)
  {
    // In case rds_programme_id does not fall in the range of PI_CODE__00 
    // or PI_CODE_0__ usStage1PiCode will be equal to rds_programme_id 
    usStage1PiCode = usTempBlockData;
    
    //RBDS CALL LETTER decoding algorithm 
    
    // 0xAFP1P2 --> 0xP1P200 
    if((usTempBlockData >= MIN_PI_CODE__00) && 
      (usTempBlockData <= MAX_PI_CODE__00))
    {
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("FMRDS: Entered MIN_PI_CODE__00 code area", 0, 0, 0);
#endif
      usStage1PiCode = (usTempBlockData << 8);
    }
    
    // AP1P3P4 ---> P10P3P4 
    if((usTempBlockData >= MIN_PI_CODE_0__) && 
      (usTempBlockData <= MAX_PI_CODE_0__))
    {
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("FMRDS: Entered MIN_PI_CODE_0__ code area", 0, 0, 0);
#endif
      usStage1PiCode = (((usTempBlockData & 0x0F00)<<4)|
        (usTempBlockData & 0x00FF));                
    }
    
    // Check if station begins with K  or W
    if((usStage1PiCode >= MIN_PI_STATION_START_K) && 
      (usStage1PiCode <= MAX_PI_STATION_START_W))
    { 
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("FMRDS: Entered MIN_PI_STATION_K or W code area", 0, 0, 0);
#endif         
      if((usStage1PiCode >= MIN_PI_STATION_START_K) && 
        (usStage1PiCode <= MAX_PI_STATION_START_K))
      {
#ifdef QXDM_SI4703_TRACE
        FMRDS_MSG_HIGH("FMRDS: Entered MIN_PI_STATION_K code area", 0, 0, 0);
#endif
        ucaRbdsPi[3] = (char)'K';
        usTempPiCode = usStage1PiCode - MIN_PI_STATION_START_K;
      }
      else if ((usStage1PiCode >= MIN_PI_STATION_START_W) && 
        (usStage1PiCode <= MAX_PI_STATION_START_W))
      {
#ifdef QXDM_SI4703_TRACE
        FMRDS_MSG_HIGH("FMRDS: Entered MIN_PI_STATION_W code area", 0, 0, 0);
#endif
        ucaRbdsPi[3] = (char)'W';
        usTempPiCode = usStage1PiCode - MIN_PI_STATION_START_W;
      }  
      
      // (676 * x) + (26 * y) + z = usTempPiCode;
      x = (usTempPiCode/676) ; 
      y = ((usTempPiCode - (676 * x))/26);
      z = (usTempPiCode - ((676 * x) + (26 * y))); 
      
      ucaRbdsPi[2] = (char) ( x + 65);
      ucaRbdsPi[1] = (char) ( y + 65);
      ucaRbdsPi[0] = (char) ( z + 65); 
    }
    
#if 0
    // As the rbds_3cl_pi_table will consume lot of memory, not supporting 3 
    // letter only call-letter
    else if((rds_programme_id >= MIN_PI_CODE_3CL) && (rds_programme_id <= MAX_PI_CODE_3CL))
    {
      
      usTempPiCode = rds_programme_id - MIN_PI_CODE_3CL;
      ucaRbdsPi[3] = rbds_3cl_pi_table[usTempPiCode][2];
      ucaRbdsPi[2] = rbds_3cl_pi_table[usTempPiCode][1];
      ucaRbdsPi[1] = rbds_3cl_pi_table[usTempPiCode][0];
      ucaRbdsPi[0] = 0;
    }
#endif  // 0
    
    else
    { 
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("FMRDS: Entered MIN_PI_OTHERS code area", 0, 0, 0);
#endif         
      /* This includes: 3-letter-only-call-letter, canada-pi-code, 
      mexico-pi-code, nationally-linked-radio-station, other-pi-codes
      -which-are-not-covered-by-above-K\W-decoding */
      /* This function currently displays the pi_number for 
      3-letter-only-call-letter as the array to store 3-letter-call-letter 
      wud take 97-100 bytes of storage space. If required in future 
      extending support for 3 call letter wud be easy only trade-off being 
      memory space */
      
      // convert uint16 to char                
      usTempPiCode = (usStage1PiCode & 0xF000) >> 12;
      if(usTempPiCode <= 0x9)
        ucaRbdsPi[3] = (char) (usTempPiCode + 48);
      else if((usTempPiCode >= 0xA) && (usTempPiCode <= 0xF))
        ucaRbdsPi[3] = (char) ((usTempPiCode - 10) + 65);
      
      usTempPiCode = (usStage1PiCode & 0x0F00) >> 8;
      if(usTempPiCode <= 0x9)
        ucaRbdsPi[2] = (char) (usTempPiCode + 48);
      else if((usTempPiCode >= 0xA) && (usTempPiCode <= 0xF))
        ucaRbdsPi[2] = (char) ((usTempPiCode - 10) + 65);
      
      usTempPiCode = (usStage1PiCode & 0x00F0) >> 4;
      if(usTempPiCode <= 0x9)
        ucaRbdsPi[1] = (char) (usTempPiCode + 48);
      else if((usTempPiCode >= 0xA) && (usTempPiCode <= 0xF))
        ucaRbdsPi[1] = (char) ((usTempPiCode - 10) + 65);
      
      usTempPiCode = (usStage1PiCode & 0x000F);
      if(usTempPiCode <= 0x9)
        ucaRbdsPi[0] = (char) (usTempPiCode + 48);
      else if((usTempPiCode >= 0xA) && (usTempPiCode <= 0xF))
        ucaRbdsPi[0] = (char) ((usTempPiCode - 10) + 65);
      
    }
    
#ifdef QXDM_SI4703_TRACE      
    FMRDS_MSG_HIGH("ucaRbdsPi[3] = %c", ucaRbdsPi[3], 0, 0);
    FMRDS_MSG_HIGH("ucaRbdsPi[2] = %c", ucaRbdsPi[2], 0, 0);
    FMRDS_MSG_HIGH("ucaRbdsPi[1] = %c", ucaRbdsPi[1], 0, 0);
    FMRDS_MSG_HIGH("ucaRbdsPi[0] = %c", ucaRbdsPi[0], 0, 0);
#endif   
   } 
   
}


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockB(void)

DESCRIPTION   Function decodes BLOCK-B RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockB(void)
{ 
  uint16  usTempBlockData;
  static  uint8    ucPty;
  boolean bTempRadioTextFlag;
  uint8 ucTempCount = 0;
  
  usTempBlockData = tsSi4703RdsDataRegMap.usBlockB;
  
  // Get PTY based on RDS\RBDS mode
  ucPty = ( ((usTempBlockData)&(0x03E0))>>5 );
  
  if (teFmrdsDevRdsStd == RDS_STD_RBDS)
  {
    fmrdsPty.tuFmrdsPtyUnion.rbdsPty = (teFmrdsDevRbdsPrgmType)(ucPty);
#ifdef QXDM_SI4703_TRACE 
    FMRDS_MSG_HIGH("FMRDS: rbdsPty = %d",fmrdsPty.tuFmrdsPtyUnion.rbdsPty,0,0);
#endif
  }
  else
  {
    fmrdsPty.tuFmrdsPtyUnion.rdsPty = (teFmrdsDevRdsPrgmType)(ucPty);
#ifdef QXDM_SI4703_TRACE 
    FMRDS_MSG_HIGH("FMRDS: rdsPty = %d",fmrdsPty.tuFmrdsPtyUnion.rdsPty,0,0);
#endif 
    
  }
  
  //Group type used in decoding data contained in block C & D
  fmrdsGroup = (teFmrdsGroupType)( ((usTempBlockData)&(0xF800))>>11 );
  
  // Get PS_char_number (GROUP 0A/0B) or RT_char number(GROUP 2A/2B) 
  // Group 0A or Group 0B contains Programme Service Name (PS) information
  if ( (fmrdsGroup==GROUP_0A) || (fmrdsGroup==GROUP_0B) )
  {
    // Get PS character number 
    ucPsCharNum = ((usTempBlockData)&(0x0003));
    
    // Get music\speech switch flag
    bMusicSpeechFlag = ((usTempBlockData & (0x0008)) >> 3); 
  }
  else if ( (fmrdsGroup == GROUP_2A) || (fmrdsGroup == GROUP_2B) )
  {
    // Get RT character number 
    ucRtCharNum = ((usTempBlockData)&(0x000F)); 
    
    bTempRadioTextFlag = (((usTempBlockData)&(0x0010))>>4);
    if(bTempRadioTextFlag != bRadioTextFlag)
    {
       //radio text flag changed from A -> B or viceversa
#ifdef QXDM_SI4703_TRACE 
       FMRDS_MSG_HIGH("Radiotextflag changed from %d to %d",bRadioTextFlag,
                                                     bTempRadioTextFlag,0);
#endif
       
       //Clear Radiotext buffer
       for(ucTempCount = 0;ucTempCount<MAX_RT_LEN; ucTempCount++)
          ucaRadioText[ucTempCount] = NULL;

       //store the new value of radio text flag
       bRadioTextFlag = bTempRadioTextFlag;

    }

  }   
}


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockC(void)

DESCRIPTION   Function decodes BLOCK-C RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockC(void)
{
   if(fmrdsGroup == GROUP_2A)
   {
      FmrdsDev_GetRt((teFmrdsDevBlockIdType)BLOCK_C); 
   }
}


/*******************************************************************************

FUNCTION      FmrdsDev_ProcessBlockD(void)

DESCRIPTION   Function decodes BLOCK-D RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ProcessBlockD(void)
{
   if ( (fmrdsGroup == GROUP_0A) || (fmrdsGroup == GROUP_0B) )
   {
      FmrdsDev_GetPs(); 
   }
   else if( (fmrdsGroup == GROUP_2A) || (fmrdsGroup == GROUP_2B) )
   {
      FmrdsDev_GetRt((teFmrdsDevBlockIdType)BLOCK_D); 
   }
}


/*******************************************************************************

FUNCTION      void FmrdsDev_GetPs(void)

DESCRIPTION   Function decodes "Programme station" RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_GetPs(void)
{
   uint16  usTempBlockData;
   uint8   ucPsLowByte, ucPsUpByte;
   uint8   ucTempCount;
   
   usTempBlockData = tsSi4703RdsDataRegMap.usBlockD;
   
   if(ucPsCharNum <= MAX_PS_CHAR_NUM)
   {
      ucPsLowByte = ((usTempBlockData>>8)&(0x00FF));
      ucPsUpByte  = ((usTempBlockData)&(0x00FF));
      
      ucTempCount = ucPsCharNum * 2;
      
      ucaPs[ucTempCount] = (char)ucPsLowByte;
      ucaPs[ucTempCount + 1] = (char)ucPsUpByte;

#ifdef QXDM_SI4703_TRACE     
      FMRDS_MSG_HIGH("ucaPs[%d] = %c",ucTempCount,ucaPs[ucTempCount], 0);
      FMRDS_MSG_HIGH("ucaPs[%d] = %c",ucTempCount+1,ucaPs[ucTempCount + 1],0);
#endif

   }
}


/*******************************************************************************

FUNCTION      void FmrdsDev_GetRt(teFmrdsDevBlockIdType fmrdsBlockId)

DESCRIPTION   Function decodes "Radio Text" RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_GetRt(teFmrdsDevBlockIdType fmrdsBlockId)
{
   uint16  usTempBlockData;
   uint8   ucTempCount = 0, ucIndex=0;
   uint8   ucRtLowByte, ucRtUpByte;
   
   if(fmrdsBlockId == BLOCK_C)
   {
      usTempBlockData = tsSi4703RdsDataRegMap.usBlockC;
   }
   else
   {
      usTempBlockData = tsSi4703RdsDataRegMap.usBlockD;
   } 
   
   if(ucRtCharNum <= MAX_RT_CHAR_NUM)
   {
      ucRtLowByte = ((usTempBlockData>>8)&(0x00FF));  
      ucRtUpByte = ((usTempBlockData)&(0x00FF));
      
      if( fmrdsBlockId == BLOCK_C)
      {                     
         ucTempCount = (ucRtCharNum * 4);
      }
      else if(fmrdsBlockId == BLOCK_D)
      {
         if(fmrdsGroup == GROUP_2A)
            ucTempCount = ((ucRtCharNum * 4) + 2);
         else if (fmrdsGroup == GROUP_2B)
            ucTempCount = (ucRtCharNum * 2);
      }
      else
      {
         FMRDS_MSG_ERROR("FMRDS: Neither BLOCK_C nor BLOCK_D ", 0, 0, 0 );
         ucTempCount = 0;
      }
      
      ucaRadioText[ucTempCount] =  (char)ucRtLowByte;
      ucaRadioText[ucTempCount + 1] = (char)ucRtUpByte; 
  
#ifdef QXDM_SI4703_TRACE     
      FMRDS_MSG_HIGH("ucaRadioText[%d] = %c",ucTempCount, 
                                             ucaRadioText[ucTempCount], 0);
      FMRDS_MSG_HIGH("ucaRadioText[%d] = %c",ucTempCount+1, 
                                             ucaRadioText[ucTempCount + 1], 0);
#endif

      //Clear radiotext buffer on reception of 0x0d
      if(ucaRadioText[ucTempCount] == 0x0D)
      {
         for(ucIndex = ucTempCount; ucIndex<MAX_RT_LEN; ucIndex++)
            ucaRadioText[ucIndex] = NULL;
      }
      else if (ucaRadioText[ucTempCount + 1] == 0x0D)
      {
         for(ucIndex = ucTempCount+1; ucIndex<MAX_RT_LEN; ucIndex++)
            ucaRadioText[ucIndex] = NULL;
      }
   } 
}

/*******************************************************************************

FUNCTION      void FmrdsDev_InitStorage(void)

DESCRIPTION   Function to initialize all data storage at driver level.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_InitStorage(void)
{
   bRawRdsFlag = FALSE;
   usRdsBufSz = 0;
   usRdsMaxBufSz = 0;
   usPostRdsBufIndex = 0;

   FmrdsDev_ClearRdsBuffers();
}


/*******************************************************************************

FUNCTION      void FmrdsDev_ClearRdsBuffers(void)

DESCRIPTION   Function to clear the RDS data storage at driver level.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
void FmrdsDev_ClearRdsBuffers(void)
{
   uint8 ucTempCount = 0;

   fmrdsGroup = GROUP_TYPE_INVALID;
   ucPsCharNum = 0;    
   ucRtCharNum = 0; 
   usRdsBufIndex = 0;

   usRdsPi = 0;
   
   for(ucTempCount = 0;ucTempCount < 5;ucTempCount++) 
      ucaRbdsPi[ucTempCount] = NULL;
   
   fmrdsPty.tuFmrdsPtyUnion.rbdsPty = _NONE;
   fmrdsPty.tuFmrdsPtyUnion.rdsPty = NONE;
   
   for(ucTempCount = 0;ucTempCount<MAX_PS_INFO_CHAR_LEN; ucTempCount++)
      ucaPs[ucTempCount] = NULL;
   
   for(ucTempCount = 0;ucTempCount<MAX_RT_LEN; ucTempCount++)
      ucaRadioText[ucTempCount] = NULL;

   bMusicSpeechFlag = 0;
   bRadioTextFlag = 0;
}

/*******************************************************************************

FUNCTION      tsFmrdsDevRdsFields FmrdsDev_GetRdsData(void)

DESCRIPTION   Function to read the available decoded RDS data.
              
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevRdsFields FmrdsDev_GetRdsData(void)
{
   tsFmrdsDevRdsFields tsFmrdsCurrentRdsFields;

   tsFmrdsCurrentRdsFields.fmrdsPi.tuFmrdsPiUnion.usRdsPi = usRdsPi;
   // line below for decoded rbds pi - not currently used
   //   tsFmrdsCurrentRdsFields.fmrdsPi.tuFmrdsPiUnion.pucRbdsPi = &ucaRbdsPi[0];
   tsFmrdsCurrentRdsFields.fmrdsPty = fmrdsPty;
   tsFmrdsCurrentRdsFields.pucPs = &ucaPs[0];
   tsFmrdsCurrentRdsFields.pucRadioText = &ucaRadioText[0];

   // calculate ps and rt string lengths - these are currently maximal lengths, revise
   // in future releases after ps/rt strategy clarified
   if ((tsFmrdsCurrentRdsFields.ucPsLength = strlen(ucaPs)) > (MAX_PS_INFO_CHAR_LEN - 1))
     tsFmrdsCurrentRdsFields.ucPsLength = MAX_PS_INFO_CHAR_LEN - 1;

   if ((tsFmrdsCurrentRdsFields.ucRtLength = strlen(ucaRadioText)) > (MAX_RT_LEN - 1))
     tsFmrdsCurrentRdsFields.ucRtLength = MAX_RT_LEN - 1;
   ucaPs[MAX_PS_INFO_CHAR_LEN - 1] = (char) '\0';  // explicitly rewrite ps string null
   ucaRadioText[MAX_RT_LEN - 1] = (char) '\0';     // explicitly rewrite rt string null

   tsFmrdsCurrentRdsFields.msSwitchCodeFlag = bMusicSpeechFlag; 
   tsFmrdsCurrentRdsFields.textABFlag = bRadioTextFlag;    ///change in rt


   // following hard-coded in existing implementation
   tsFmrdsCurrentRdsFields.psNum = 0;                   // ps repeat - tbc
    
   tsFmrdsCurrentRdsFields.decIdCtrlCodeFlag = 0;       // decoder id tbd

   tsFmrdsCurrentRdsFields.taPrgmCodeFlag = 0;          //
   tsFmrdsCurrentRdsFields.taAnnouncementCodeFlag = 0;  // return no traffic info

   // af retrieval currently not supported
   tsFmrdsCurrentRdsFields.noOfAFInList = 0;            // return no afs found

   return(tsFmrdsCurrentRdsFields);
} 


/*******************************************************************************

FUNCTION      FmrdsDev_CopyRdsData(void)

DESCRIPTION   Function decodes the RDS data.
              
DEPENDENCIES  None

RETURN VALUE  Returns status of updated data

SIDE EFFECTS  None

*******************************************************************************/
teFmrdsDevUpdateStatus FmrdsDev_CopyRdsData(void)
{
   teFmrdsDevUpdateStatus updateStatus = FMRDS_UPDATE_DONE;  // default return

   //check array bounds
   if((usRdsBufIndex > usRdsMaxBufSz)||(usRdsBufIndex >= MAX_DRVR_RDS_BUFF_SZ))
   {
      FMRDS_MSG_ERROR("RDS Buffer Index exceeds max buffer size",0,0,0);
      return(updateStatus);
   }

   tsRawRdsBuffer[usRdsBufIndex] = tsSi4703RdsDataRegMap;

#ifdef QXDM_SI4703_TRACE     
   FMRDS_MSG_HIGH("tsRawRdsBuffer[%d].usBlockA = 0x%x",
                       usRdsBufIndex,tsRawRdsBuffer[usRdsBufIndex].usBlockA, 0);
   FMRDS_MSG_HIGH("tsRawRdsBuffer[%d].usBlockB = 0x%x",
                       usRdsBufIndex,tsRawRdsBuffer[usRdsBufIndex].usBlockB, 0);
   FMRDS_MSG_HIGH("tsRawRdsBuffer[%d].usBlockC = 0x%x",
                       usRdsBufIndex,tsRawRdsBuffer[usRdsBufIndex].usBlockC, 0);
   FMRDS_MSG_HIGH("tsRawRdsBuffer[%d].usBlockD = 0x%x",
                       usRdsBufIndex,tsRawRdsBuffer[usRdsBufIndex].usBlockD, 0);
#endif

   usRdsBufIndex++;

   // circular buffer so wrap around the usRdsBufIndex to start of buffer
  usRdsBufIndex = usRdsBufIndex % usRdsMaxBufSz;
  
  if(usRdsBufIndex == usRdsBufSz)
   {
     // First half of buffer is full 
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("First buffer full",0,0,0);
#endif
      usPostRdsBufIndex = RDS_BUFF_START;
      updateStatus = FMRDS_UPDATE_BLOCK_BUFF_FULL; 
   }
   else if (usRdsBufIndex == RDS_BUFF_START)
   {
      // Second half of buffer is full 
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("Second buffer full",0,0,0);
#endif
     usPostRdsBufIndex = usRdsBufSz;
     updateStatus = FMRDS_UPDATE_BLOCK_BUFF_FULL;
   }

   return(updateStatus);
}


/*******************************************************************************

FUNCTION      FmrdsDev_GetRdsBuffInfo(void)

DESCRIPTION   Function to read raw rds data buffer information.
              
DEPENDENCIES  None

RETURN VALUE  Returns RDS buffer information.

SIDE EFFECTS  None

*******************************************************************************/
tsFmrdsDevRawRdsBlock FmrdsDev_GetRdsBuffInfo(void)
{
   tsFmrdsDevRawRdsBlock tsRdsBuffInfo;

   //usPostRdsBufIndex
   if((usPostRdsBufIndex > usRdsBufSz) || (usPostRdsBufIndex >= MAX_DRVR_RDS_BUFF_SZ))
   {
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_ERROR("usPostRdsBufIndex exceeds the defined limit ",0,0,0);
#endif
      tsRdsBuffInfo.usRawBlockBuffSize = 0; 
      tsRdsBuffInfo.psRawBlockBuffPtr = NULL;
   }
   else
   {
#ifdef QXDM_SI4703_TRACE
      FMRDS_MSG_HIGH("Read RDS buffer information usRdsBufSz =%d,usPostRdsBufIndex= %d ",usRdsBufSz,usPostRdsBufIndex,0);
#endif
      
      tsRdsBuffInfo.usRawBlockBuffSize = usRdsBufSz; 
      tsRdsBuffInfo.psRawBlockBuffPtr = &tsRawRdsBuffer[usPostRdsBufIndex];
   }

   return(tsRdsBuffInfo);
}


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
teFmrdsDevResult FtmFmrdsDev_ReadRegs(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   static uint8 aucread_data_byte[SIZE_OF_FMRDS_DATA_READ];
   
   result = FmrdsDev_ReadData(FMRDS_SI4703_2WIRE_ID, SIZE_OF_FMRDS_DATA_READ, 
      &aucread_data_byte[0]);
   
   if(result == FMRDSDEV_OK)
   {
      /*FMRDS_MSG_HIGH("[0]=0x%x,[1]=0x%x",aucread_data_byte[0], aucread_data_byte[1], 0);
      FMRDS_MSG_HIGH("[2]=0x%x,[3]=0x%x",aucread_data_byte[2], aucread_data_byte[3], 0);
      FMRDS_MSG_HIGH("[4]=0x%x,[5]=0x%x",aucread_data_byte[4], aucread_data_byte[5], 0);
      FMRDS_MSG_HIGH("[6]=0x%x,[7]=0x%x",aucread_data_byte[6], aucread_data_byte[7], 0);
      FMRDS_MSG_HIGH("[8]=0x%x,[9]=0x%x",aucread_data_byte[8], aucread_data_byte[9], 0);
      FMRDS_MSG_HIGH("[10]=0x%x,[11]=0x%x",aucread_data_byte[10], aucread_data_byte[11], 0);
      FMRDS_MSG_HIGH("[12]=0x%x,[13]=0x%x",aucread_data_byte[12], aucread_data_byte[13], 0);
      FMRDS_MSG_HIGH("[14]=0x%x,[15]=0x%x",aucread_data_byte[14], aucread_data_byte[15], 0);
      FMRDS_MSG_HIGH("[16]=0x%x,[17]=0x%x",aucread_data_byte[16], aucread_data_byte[17], 0);
      FMRDS_MSG_HIGH("[18]=0x%x,[19]=0x%x",aucread_data_byte[18], aucread_data_byte[19], 0);
      FMRDS_MSG_HIGH("[20]=0x%x,[21]=0x%x",aucread_data_byte[20], aucread_data_byte[21], 0);
      FMRDS_MSG_HIGH("[22]=0x%x,[23]=0x%x",aucread_data_byte[22], aucread_data_byte[23], 0);
      FMRDS_MSG_HIGH("[24]=0x%x,[25]=0x%x",aucread_data_byte[24], aucread_data_byte[25], 0);
      FMRDS_MSG_HIGH("[26]=0x%x ---------",aucread_data_byte[26], 0, 0);*/

      FMRDS_MSG_HIGH("[0]=0x%x%x",aucread_data_byte[12], aucread_data_byte[13], 0);
      FMRDS_MSG_HIGH("[1]=0x%x%x",aucread_data_byte[14], aucread_data_byte[15], 0);
      FMRDS_MSG_HIGH("[2]=0x%x%x",aucread_data_byte[16], aucread_data_byte[17], 0);
      FMRDS_MSG_HIGH("[3]=0x%x%x",aucread_data_byte[18], aucread_data_byte[19], 0);
      FMRDS_MSG_HIGH("[4]=0x%x%x",aucread_data_byte[20], aucread_data_byte[21], 0);
      FMRDS_MSG_HIGH("[5]=0x%x%x",aucread_data_byte[22], aucread_data_byte[23], 0);
      FMRDS_MSG_HIGH("[6]=0x%x%x",aucread_data_byte[24], aucread_data_byte[25], 0);
      FMRDS_MSG_HIGH("[7]=0x%x%x",aucread_data_byte[26], aucread_data_byte[27], 0);
      FMRDS_MSG_HIGH("[8]=0x%x%x",aucread_data_byte[28], aucread_data_byte[29], 0);
      FMRDS_MSG_HIGH("[9]=0x%x%x",aucread_data_byte[30], aucread_data_byte[31], 0);
      FMRDS_MSG_HIGH("[A]=0x%x%x",aucread_data_byte[0], aucread_data_byte[1], 0);
      FMRDS_MSG_HIGH("[B]=0x%x%x",aucread_data_byte[2], aucread_data_byte[3], 0);
      FMRDS_MSG_HIGH("[C]=0x%x%x",aucread_data_byte[4], aucread_data_byte[5], 0);
      FMRDS_MSG_HIGH("[D]=0x%x%x",aucread_data_byte[6], aucread_data_byte[7], 0);
      FMRDS_MSG_HIGH("[E]=0x%x%x",aucread_data_byte[8], aucread_data_byte[9], 0);
      FMRDS_MSG_HIGH("[F]=0x%x%x",aucread_data_byte[10], aucread_data_byte[11], 0);

   }
   else
   {
      FMRDS_MSG_HIGH("Communication with Si4703 failed",0,0,0);    
   }

   return(result);
}


// *** initialise device ***
teFmrdsDevResult FtmFmrdsDev_Init(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   
   result = FmrdsDev_Init();
   
   if(result == FMRDSDEV_OK)
   {
      FMRDS_MSG_HIGH("FmrdsDev_Init() SUCCESS",0,0,0); 
      FtmFmrdsDev_ReadRegs();
   }
   else
   {
      FMRDS_MSG_HIGH("FmrdsDev_Init() FAILURE",0,0,0);    
   }
   
   return(result);  
}

// *** configure radio settings ***
teFmrdsDevResult FtmFmrdsDev_ConfigureRadio(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   
   tsFmrdsDevRadioSettingsType fmrdsRadioSettings;
   fmrdsRadioSettings.radioBand  = US_EUROPE_BAND;
   fmrdsRadioSettings.deEmphasis = DE_EMP50;
   fmrdsRadioSettings.chSpacing  = CHSPACE_100_KHZ;    
   
   result = FmrdsDev_ConfigureRadio(fmrdsRadioSettings);
   if(result == FMRDSDEV_OK)
   {
      FMRDS_MSG_HIGH("FtmFmrdsDev_ConfigureRadio() SUCCESS",0,0,0); 
   }
   else
   {
      FMRDS_MSG_HIGH("FtmFmrdsDev_ConfigureRadio() FAILURE",0,0,0);
   }
   
   return(result);  
}

// *** set normal power mode ***
teFmrdsDevResult FtmFmrdsDev_SetNormalPower(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   teFmrdsDevPowerState fmrdsPowerState = FMRDSDEV_HIGH_POWER;
   
   result = FmrdsDev_SetPowerState(fmrdsPowerState);
   
   if(result == FMRDSDEV_OK)
   {
      FMRDS_MSG_HIGH("FtmFmrdsDev_SetNormalPower() SUCCESS",0,0,0); 
   }
   else
   {
      FMRDS_MSG_HIGH("FtmFmrdsDev_SetNormalPower() FAILURE",0,0,0);    
   }
   
   return(result);  
}

// *** set station ***
teFmrdsDevResult FtmFmrdsDev_SetStation(void)
{
   teFmrdsDevResult result = FMRDSDEV_ERROR;
   tusFmrdsDevFreqType fmrdsFmrdsFreq = 9000;
   
   result = FmrdsDev_SetStation(fmrdsFmrdsFreq);
   if(result == FMRDSDEV_OK)
   {
      FMRDS_MSG_HIGH("FtmFmrdsDev_SetStation() SUCCESS",0,0,0); 
   }
   else
   {
      FMRDS_MSG_HIGH("FtmFmrdsDev_SetStation() FAILURE",0,0,0);    
   }
   
   return(result);  
}

#endif  // FMRDS_INCLUDE_FTM




#endif  // FEATURE_FMRDS

// *** end of file ***

