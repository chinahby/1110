#ifndef FMRDS_SI4703_H
#define FMRDS_SI4703_H

/******************************************************************************
***
***   TITLE: fmrds_si4703.h
***
***   Generic FM/RDS Device Driver Code. Device = SiLabs 4703 FM Receiver
***   -------------------------------------------------------------------
***
***
***   DESCRIPTION
***
***   Low level primitives controlling specific FM receiver device. These are
***   defined in the generic layer of the FM/RDS support code and are called
***   as required to implement higher-level functionality.
***   
***   Used to initialise FM device, set/get parameters and monitor device
***   status.
***
***   Code should be regarded as a reference only.
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
***  $Header: //source/qcom/qct/wconnect/fmradio/fmrds/main/latest/src/fmrds_si4703.h#8 $ $DateTime: 2008/11/26 09:33:21 $ $Author: abhis $
***
***  when       who         what, where, why
***  --------------------------------------------------------------------------
***  2008-09-10 rjm         band limit definitions now in api.h
***  2008-09-17 as          support for raw RDS data buffering added
***  2008-09-05 as          seekth enum updated 
***  2008-07-30 as          rds definitions added
***  2008-06-30 as          Added si4703 related registers
***  2008-06-05 rjm         initial release
******************************************************************************/


//
// Driver support for Silicon Labs 4703 receiver.
// Refer to device data sheet and SiLabs App Note AN230.
// Reference code only - check any device respins by manufacturer.
//

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "custfmrds.h"          // include global fm rds switches

#ifdef FEATURE_FMRDS

#include "fmrds_api.h"          // for typedefs
#include "fmrds_generic.h"      // include generic fm definitions
#include "i2c.h"                // i2c/2-wire defines
#include "gpio_int.h"           // GPIO interrupt defines



/*****************************************************************************
***
***     Public Device-Related Defines & Constants
***
*****************************************************************************/


/* Provides monitoring of Si4703 physical layer through QXDM */
//#undef QXDM_SI4703_TRACE
#define QXDM_SI4703_TRACE 

//
// *** si4703 control reg related defines ***
//

/* device addresses */
#define FMRDS_SI4703_2WIRE_ID  0x20    // device bus address

/* Default FM Radio Frequency */
#define SI4703_DEFAULT_CHAN_FREQ   US_EUROPE_BAND_MIN
#define SI4703_DEFAULT_CHAN_VAL    0       /* 87.5MHz */

/* Si4703 Power up time = 110ms */
#define SI4703_PWRUP_TIME 110 

/*Si4703 Seek/Tune time = 60ms */
#define SI4703_SEEKTUNE_TIME 60 

// RDS related defines - Start
// RBDS PI call letter conversion - Start
#define MIN_PI_STATION_START_K   0x1000 /* decimal value: 4096  */
#define MAX_PI_STATION_START_K   0x54A7 /* decimal value: 21671 */
#define MIN_PI_STATION_START_W   0x54A8 /* decimal value: 21672 */
#define MAX_PI_STATION_START_W   0x994F /* decimal value: 39247 */

#define MIN_PI_CODE_0__ 0xA100
#define MAX_PI_CODE_0__ 0xA9FF

#define MIN_PI_CODE__00 0xAFA1
#define MAX_PI_CODE__00 0xAFAF
// RBDS PI call letter conversion - End

#define MAX_PS_CHAR_NUM 3
#define MAX_RT_CHAR_NUM  15

// Raw Rds data buffer sizes
#define MIN_APP_RDS_BUFF_SZ    10
#define MAX_APP_RDS_BUFF_SZ    100  
#define MAX_DRVR_RDS_BUFF_SZ   (MAX_APP_RDS_BUFF_SZ * 2)  
#define RDS_BUFF_START         0

//Max seek polling count
#define MIN_SEEK_POLL_COUNT  1
#define MAX_SEEK_POLL_COUNT  100
// RDS related defines - End

/* Power Mode Setting*/
typedef enum
{
   SI4703_NORMAL_MODE    = 0x01,  /* disable=0, enable=1 */
   SI4703_LOW_POWER_MODE = 0x41,  /* disable=1, enable=1 */
   SI4703_AFTER_LOW_PWR  = 0x00   /* disable=0, enable=0 after low power mode is set */
}teSi4703PwrModeType;

/* Crystal oscillator Mode Setting */
typedef enum
{
   XOSCEN_DISABLE = 0,
   XOSCEN_ENABLE  = 1 
}teSi4703XoscenModeType;

/* Audio High-Z setting */
typedef enum
{
   AHIZEN_DISABLE = 0,
   AHIZEN_ENABLE  = 1 
}teSi4703AhizenModeType;

/* GPIO3 Mode setting */
typedef enum
{
   GPIO3_HIGH_IMPEDANCE = 0, 
   GPIO3_MS_IND         = 1, /* Mono/Stereo indicator */
   GPIO3_LOW            = 2, 
   GPIO3_HIGH           = 3  
}teSi4703Gpio3ModeType;

/* Interrupt Mode */
typedef enum
{
   GPIO2_HIGH_IMPEDANCE = 0, 
   GPIO2_INT_ENABLE     = 1, /* GPIO2 generates STC\RDS interrupt */
   GPIO2_LOW            = 2, 
   GPIO2_HIGH           = 3  
}teSi4703IntModeType;

/* GPIO1 Mode setting */
typedef enum
{
   GPIO1_HIGH_IMPEDANCE = 0, 
   GPIO1_RES            = 1, /* Reserved for future use */
   GPIO1_LOW            = 2, 
   GPIO1_HIGH           = 3  
}teSi4703Gpio1ModeType;

/* RDS Interrupt Mode */
typedef enum
{
   RDS_DISABLE_INT = 0,
   RDS_ENABLE_INT  = 1
}teSi4703RdsIntModeType;

/* STC Interrupt Mode */
typedef enum
{
   STC_DISABLE_INT = 0,
   STC_ENABLE_INT  = 1
}teSi4703StcIntModeType;

/* AGC Disable */
typedef enum
{
   AGC_ENABLE    = 0,
   AGC_DISABLE   = 1
}teSi4703AgcDModeType;


/* Stereo/Mono Blend Level adjustment */
typedef enum
{
   BLNDADJ_LEV0 = 0, 
   BLNDADJ_LEV1 = 1,
   BLNDADJ_LEV2 = 2,
   BLNDADJ_LEV3 = 3
}teSi4703BlndAdjType;

/* Disable Soft Mute setting */
typedef enum
{
   DSMUTE_ENABLE  = 0,
   DSMUTE_DISABLE = 1
}teSi4703DsMuteType;

/* Soft Mute Attack\Recover Rate setting */
typedef enum
{
   SMUTER_FASTEST = 0,
   SMUTER_FAST    = 1,
   SMUTER_SLOW    = 2,
   SMUTER_SLOWEST = 3
}teSi4703SMuteRType;

/* Soft Mute Attenuation setting */
typedef enum
{
   SMUTEA_LEV0 = 0,
   SMUTEA_LEV1 = 1,
   SMUTEA_LEV2 = 2,
   SMUTEA_LEV3 = 3
}teSi4703SMuteAType;

/* Extended volume Range Mode*/
typedef enum
{
   VOLEXT_DISABLE = 0,
   VOLEXT_ENABLE  = 1
}teSi4703VolExtType;

/* RSSI Seek Threshold */
/*typedef enum
{
   SEEKTH_MIN_RSSI = 0x0,
   SEEKTH_MID_RSSI = 0xC,
   //other values can be added as needed
   SEEKTH_MAX_RSSI = 0x7F
}teSi4703SeekThType;*/

#define SEEKTH_MID_RSSI 0xC
typedef uint8 teSi4703SeekThType;

/* Seek SNR Threshold */
typedef enum
{
   SKSNR_DISABLE = 0x0, 
   SKSNR_MIN     = 0x1,
   SKSNR_VAL     = 0x5, //from AN284
   SKSNR_MAX     = 0xF
}teSi4703SkSnrType;

/* Seek FM Impulse Detection Threshold */
typedef enum
{
   SKCNT_DISABLE = 0x0, 
   SKCNT_MIN     = 0x1,
   SKCNT_VAL     = 0x8, //from AN284
   SKCNT_MAX     = 0xF
}teSi4703SkCntType;

/* RDS Mode setting */
typedef enum
{
   RDS_STANDARD_MODE = 0,
   RDS_VERBOSE_MODE  = 1
}teSi4703RdsModeType;

/* Mute Mode setting */
typedef enum
{
   MUTE_ENABLE   = 0,    /* Default value */
   MUTE_DISABLE  = 1    
}teSi4703DMuteModeType;

/* Seek direction setting */
typedef enum
{
   SEEK_DOWN = 0,
   SEEK_UP   = 1
}teSi4703SeekDirType; 

/* Seek mode settings */
typedef enum
{
   SKMODE_CONT_BLMT = 0, /* Wrap at bandlimits & cont. seeking */
   SKMODE_STOP_BLMT = 1  /* Stop seeking at bandlimits */
}teSi4703SkModeType; 

/* Seek Power up Mode */
typedef enum
{
   SEEK_DISABLE = 0,
   SEEK_ENABLE   = 1
}teSi4703SeekPwrUpModeType; 

/* Tune Power up Mode */
typedef enum
{
   TUNE_DISABLE = 0,
   TUNE_ENABLE   = 1
}teSi4703TunePwrUpModeType; 

//
// *** si4703 status reg related defines ***
//
/* RDS Ready Status */
typedef enum
{
   RDS_NO_GROUP_READY   = 0,
   RDS_NEW_GROUP_READY  = 1
}teSi4703RdsRStatusType;

/* Seek/Tune Complete Status */
typedef enum
{ 
   STC_NOT_COMPLETE   = 0,
   STC_COMPLETE       = 1
}teSi4703StcStatusType;

/* Seek Status */
typedef enum
{ 
   SEEK_SUCCESS    = 0,
   SEEK_FAIL       = 1
}teSi4703SfStatusType;

/* AFC Rail Status */
typedef enum
{
   AFC_NOT_RAILED   = 0,
   AFC_RAILED       = 1
}teSi4703AfcRlStatusType;

/* RDS Synchronized */
typedef enum
{
   RDS_NOT_SYNCED   = 0,
   RDS_SYNCED       = 1
}teSi4703RdsSStatusType;

/* RDS Block Errors - available only in verbose mode */
typedef enum
{
   ERRORS_NONE             = 0,
   ERRORS_1_2              = 1,
   ERRORS_3_5              = 2,
   ERRORS_UNCORRECTABLE    = 3
}teSi4703RdsEStatusType;


// *** Shadow functional register map for si4703 chip  ***
typedef struct _tsSi4703ControlRegMapType
{
   /* Si4703 Hardware Control */
   teSi4703PwrModeType        si4703PwrMode;
   teSi4703XoscenModeType     si4703XoscenMode; // XOSCEN_default_value
   teSi4703AhizenModeType     si4703AhizenMode; // AHIZEN
   teSi4703Gpio3ModeType      si4703Gpio3Mode;  // define GPIO3_DEFAULT_VAL
   teSi4703IntModeType        si4703IntMode;
   teSi4703Gpio1ModeType      si4703Gpio1Mode;  // define GPIO1_DEFAULT_VAL
   teSi4703RdsIntModeType     si4703RdsIntMode;
   teSi4703StcIntModeType     si4703StcIntMode;
   teSi4703AgcDModeType       si4703AgcDMode;
   
   /* Si4703 General Config */
   teSi4703BlndAdjType        si4703BlndAdj;  // define BLNDADJ_DEFAULT_VAL
   teSi4703DsMuteType         si4703DsMute;   // define DSMUTE_DEFAULT_VAL
   teSi4703SMuteRType         si4703SMuteR;   // define SMUTER_DEFAULT_VAL
   teSi4703SMuteAType         si4703SMuteA;   // define SMUTEA_DEFAULT_VAL
   teSi4703VolExtType         si4703VolExt;   // VOLEXT function tbd
   teSi4703SeekThType         si4703SeekTh;   // define SEEKTH_DEFAULT_VAL
   teSi4703SkSnrType          si4703SkSnr;    // define SKSNR_DEFAULT_VAL
   teSi4703SkCntType          si4703SkCnt;    // define SKCNT_DEFAULT_VAL
   // cannot find RDS_PRF in reg map
   teSi4703RdsModeType    si4703RdsMode;
   
   /* si4703 Regional Config */
   teFmrdsDevRdsPwrUpMode     si4703RdsPwrupMode;  // typedef from api.h
   teFmrdsDevDtcTimeType      si4703DeEmphasis;    // typedef from api.h
   teFmrdsDevBandLimitsType   si4703RadioBand;     // typedef from api.h
   teFmrdsDevChanSpacingType  si4703ChSpacing;     // typedef from api.h
   
   /* si4703 End-user Adjustable */
   teSi4703DMuteModeType      si4703DMuteMode;
   teFmrdsDevStereoType       si4703StereoMonoMode; // typedef from api.h
   teFmrdsVolumeLevType       si4703Volume;         // local volume level
   
   /* si4703 Seek Config */
   teSi4703SeekDirType        si4703SeekDir;
   teSi4703SkModeType         si4703SkMode; // Define SKMODE_DEFAULT_VAL
   teSi4703SeekPwrUpModeType  si4703SeekPwrUpMode;
   
   /*si4703 Tune config */
   teSi4703TunePwrUpModeType  si4703TunePwrUpMode;
   tusFmrdsDevFreqType        si4703ChanVal;
}tsSi4703ControlRegMapType;

// *** Shadow status register map for si4703 chip  ***
typedef struct _tsSi4703StatusRegMapType
{
   teSi4703RdsRStatusType     si4703RdsRStatus;  // RDS Group Ready
   teSi4703StcStatusType      si4703StcStatus;   // Seek\Tune Complete
   teSi4703SfStatusType       si4703SfStatus;    // Seek\Band Fail
   teSi4703AfcRlStatusType    si4703AfcRlStatus; // AFC Rail-Invalid channel identifier
   uint8                      rssiStatus;        // Received Signal Strength Indicator
   tusFmrdsDevFreqType        currentChanVal;    // Channel Value corresponding  
                                                 // to requested frequency
   teFmrdsDevStStateType      si4703StStatus;    // Stereo Indicator
   teSi4703RdsSStatusType     si4703RdsSStatus;  // RDS Synchronized       
   // Add rds regs as required
}tsSi4703StatusRegMapType;

/*typedef struct _tsSi4703RdsDataRegMapType
{

   teSi4703RdsEStatusType BlockAErrors;
   teSi4703RdsEStatusType BlockBErrors;
   teSi4703RdsEStatusType BlockCErrors;
   teSi4703RdsEStatusType BlockDErrors;
   uint16 BlockAData;
   uint16 BlockBData;
   uint16 BlockCData;
   uint16 BlockDData;

}tsSi4703RdsDataRegMapType;*/

// We are using FM Si4703's STANDARD RDS mode so errors information
// is not relevant.  
typedef tsFmrdsDevRawBlockData tsSi4703RdsDataRegMapType;



// *** internal register addresses ***

// *** various read/write defines ***

// Silab engineers asked not to write to TEST1 register
#define SIZE_OF_FMRDS_DATA_WRITE          10 // 10 bytes
 
#define SIZE_OF_PWRUP_DATA_WRITE          2
#define SIZE_OF_CONFIG_DATA_WRITE         8
#define SIZE_OF_VOLUME_DATA_WRITE         8
#define SIZE_OF_SET_STATION_DATA_WRITE    4
#define SIZE_OF_MUTE_DATA_WRITE           1
#define SIZE_OF_STEREO_DATA_WRITE         1
#define SIZE_OF_GPIO2_INT_DATA_WRITE      6 
#define SIZE_OF_SRCH_SEEK_DATA_WRITE      10
#define SIZE_OF_ONLY_SEEK_DATA_WRITE      1


#define SIZE_OF_FMRDS_DATA_READ           32 // can be reduced to 27/16 bytes after test 
#define SIZE_OF_GET_TUNE_STATUS           2
#define SIZE_OF_RDS_DATA_STATUS           12




/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

// 4703 id string

// ...


#endif  // FEATURE_FMRDS

#endif  // FMRDS_SI4703_H


// *** end of file ***
