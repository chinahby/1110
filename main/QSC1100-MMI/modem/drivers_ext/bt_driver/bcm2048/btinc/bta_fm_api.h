/*****************************************************************************
**
**  Name:           bta_fm_api.h
**
**  Description:    This is the public interface file for the FM subsystem of 
**                  BTA, Broadcom's Bluetooth application layer for mobile phones.
**                  
**  Copyright (c) 2006, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_FM_API_H
#define BTA_FM_API_H

#include "bta_api.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/
/* Extra Debug Code */
#ifndef BTA_FM_DEBUG
#define BTA_FM_DEBUG           FALSE
#endif

#ifndef BTA_FM_AF_INCLUDED  
#define BTA_FM_AF_INCLUDED      TRUE
#endif

/* Scanning step option provided by software algorithm */
#ifndef BTA_FM_SCAN_STEP_ALGO
#define BTA_FM_SCAN_STEP_ALGO           TRUE
#endif

/* noise floor estimation */
#ifndef     BTA_FM_NFE_DEFAILT
#define     BTA_FM_NFE_DEFAILT      93      /* default Noise floor value */
#endif
#ifndef     BTA_FM_NFE_THRESH
#define     BTA_FM_NFE_THRESH       0x32    /* default NFE threshold 53 db */
#endif
#ifndef     BTA_FM_NFE_SNR_STEREO
#define     BTA_FM_NFE_SNR_STEREO   19      /* Stereo audio mode SNR level above NFL */
#endif
#ifndef     BTA_FM_NFE_SNR_MONO
#define     BTA_FM_NFE_SNR_MONO     10      /* Mono audio mode SNR level above NFL */
#endif

/* Max AF number */
#ifndef BTA_FM_AF_MAX_NUM
#define BTA_FM_AF_MAX_NUM   25
#endif

#define BTA_FM_MAX_PRESET_STA       62      /* same number defined as I2C_PRESET_STA_MAX */

enum
{
    BTA_FM_ENABLE_EVT,          /* BTA FM is enabled */
    BTA_FM_DISABLE_EVT,         /* BTA FM is disabled */
    BTA_FM_TUNE_EVT,            /* FM tuning is completed or failed */
    BTA_FM_SEARCH_EVT,          /* FM scanning, clear channel update event */
    BTA_FM_SEARCH_CMPL_EVT,     /* FM scanning is completed event */
    BTA_FM_AF_JMP_EVT,          /* AF jump event */
    BTA_FM_AUD_MODE_EVT,        /* Set audio mode completed  */
    BTA_FM_AUD_DATA_EVT,        /* audio quality live updating call back event */
    BTA_FM_AUD_PATH_EVT,        /* set audio path completed */
    BTA_FM_RDS_UPD_EVT,         /* read RDS data event */
    BTA_FM_RDS_MODE_EVT,        /* set RDS mode event */
    BTA_FM_SET_DEEMPH_EVT,      /* configure deempahsis parameter event */
    BTA_FM_MUTE_AUD_EVT,         /* audio mute/unmute event */
    BTA_FM_SCAN_STEP_EVT,       /* config scan step */
    BTA_FM_SET_REGION_EVT,       /* set band region */
    BTA_FM_NFL_EVT              /* noise floor estimation event */
};
typedef UINT8  tBTA_FM_EVT;

enum
{
    BTA_FM_OK,
    BTA_FM_SCAN_RSSI_LOW,
    BTA_FM_SCAN_FAIL,
    BTA_FM_SCAN_ABORT,
    BTA_FM_SCAN_NO_RES,
    BTA_FM_ERR,
    BTA_FM_UNSPT_ERR,
    BTA_FM_FLAG_TOUT_ERR,
    BTA_FM_FREQ_ERR,
    BTA_FM_VCMD_ERR
};
typedef UINT8   tBTA_FM_STATUS;

/* FM band region, bit 0, 1,2 of func_mask */
#ifndef BTA_MAX_REGION_SETTING
#define BTA_MAX_REGION_SETTING  3   /* max region code defined */
#endif

/* FM function mask */
#define     BTA_FM_REGION_NA    0x00        /* bit0/bit1/bit2: north america */
#define     BTA_FM_REGION_EUR   0x01        /* bit0/bit1/bit2: Europe           */
#define     BTA_FM_REGION_JP    0x02        /* bit0/bit1/bit2: Japan            */
#define     BTA_FM_RDS_BIT      1<<4        /* bit4: RDS functionality */
#define     BTA_FM_RBDS_BIT     1<<5        /* bit5: RBDS functionality, exclusive with RDS bit */
#define     BTA_FM_AF_BIT       1<<6        /* bit6: AF functionality */
typedef UINT8   tBTA_FM_FUNC_MASK;

#define     BTA_FM_REGION_MASK  0x07             /* low 3 bits (bit0, 1, 2)of FUNC mask is region code */
typedef UINT8  tBTA_FM_REGION_CODE;        


#define     BTA_FM_DEEMPHA_50U      0       /* 6th bit in FM_AUDIO_CTRL0 set to 0, Europe default */
#define     BTA_FM_DEEMPHA_75U      1<<6    /* 6th bit in FM_AUDIO_CTRL0 set to 1, US  default */
typedef UINT8 tBTA_FM_DEEMPHA_TIME;

#define BTA_FM_GET_FREQ(x)  ((UINT16)((x * 10) - 64000))

enum    
{
    BTA_FM_SCH_RDS_NONE,
    BTA_FM_SCH_RDS_PTY, 
    BTA_FM_SCH_RDS_TP   
};
typedef UINT8 tBTA_FM_SCH_RDS_TYPE;

typedef struct
{
    tBTA_FM_SCH_RDS_TYPE    cond_type;
    UINT8                   cond_val;
} tBTA_FM_SCH_RDS_COND;

/* FM audio output mode */
enum
{
    BTA_FM_AUTO_MODE,       /* auto blend by default */
    BTA_FM_STEREO_MODE,     /* manual stereo switch */
    BTA_FM_MONO_MODE,       /* manual mono switch */
    BTA_FM_SWITCH_MODE      /* auto stereo, and switch activated */
};
typedef UINT8  tBTA_FM_AUDIO_MODE;

/* FM audio output quality */
#define BTA_FM_STEREO_ACTIVE    0x01     /* audio stereo detected */
#define BTA_FM_MONO_ACTIVE      0x02     /* audio mono */
#define BTA_FM_BLEND_ACTIVE     0x04     /* stereo blend active */

typedef UINT8  tBTA_FM_AUDIO_QUALITY;

/* FM audio routing configuration */
#define BTA_FM_SCO_ON       0x01        /* routing FM over SCO off, voice on SCO on */
#define BTA_FM_SCO_OFF      0x02        /* routing FM over SCO on, voice on SCO off */
#define BTA_FM_A2D_ON       0x10        /* FM analog output active */
#define BTA_FM_A2D_OFF      0x40        /* FM analog outout off */
#define BTA_FM_I2S_ON       0x20        /* FM digital (I2S) output on */
#define BTA_FM_I2S_OFF      0x80        /* FM digital output off */

typedef UINT8 tBTA_FM_AUDIO_PATH;

/* frequency scanning direction */
#define BTA_FM_SCAN_DOWN        0x00        /* scanning toward lower frequency */
#define BTA_FM_SCAN_UP          0x80        /* scanning toward higher frequency */
#define BTA_FM_SCAN_FULL        0x81        /* full band scan */
#define BTA_FM_FAST_SCAN        0x88        /* fast scan */
#define BTA_FM_SCAN_NONE        0xff

typedef UINT8 tBTA_FM_SCAN_MODE;

#define  BTA_FM_STEP_100KHZ     0x00
#define  BTA_FM_STEP_50KHZ      0x10
typedef UINT8   tBTA_FM_STEP_TYPE;

enum
{
    BTA_FM_AF_FM = 1,
    BTA_FM_AF_LF,
    BTA_FM_AF_MF
};
typedef UINT8 tBTA_FM_AF_TYPE;

/* AF structure */
typedef struct
{
    UINT16              num_af;     /* number of AF in list */
    tBTA_FM_AF_TYPE     af_type[BTA_FM_AF_MAX_NUM];
    UINT16              af_list[BTA_FM_AF_MAX_NUM];
}tBTA_FM_AF_LIST;

typedef struct
{
    UINT16              pi_code;          /* currently tuned frequency PI code */
    tBTA_FM_AF_LIST     af_list;          /* AF frequency list */
} tBTA_FM_AF_PARAM;

enum
{
    BTA_FM_NFL_LOW,
    BTA_FM_NFL_MED,
    BTA_FM_NFL_FINE
};
typedef UINT8 tBTA_FM_NFE_LEVL;

/* channel tunning/scanning call back data */
typedef struct
{
    tBTA_FM_STATUS      status;         /* operation status */
    UINT8               rssi;
    UINT16              freq;           /* tuned frequency */
}tBTA_FM_CHNL_DATA;


/* set FM audio mode callback data */
typedef struct
{
    tBTA_FM_STATUS      status;     /* operation status */
    tBTA_FM_AUDIO_MODE  audio_mode; /* audio mode */
}tBTA_FM_MODE_INFO;

/* audio quality live updating call back data */
typedef struct
{
    tBTA_FM_STATUS          status;     /* operation status */
    UINT8                   rssi;       /* rssi strength    */
    tBTA_FM_AUDIO_QUALITY   audio_mode; /* audio mode       */
}tBTA_FM_AUD_DATA;


typedef struct
{
    tBTA_FM_STATUS          status;     /* operation status         */
    tBTA_FM_DEEMPHA_TIME    time_const; /* deemphasis parameter     */
}tBTA_FM_DEEMPH_DATA;

/* set FM audio mode callback data */
typedef struct
{
    tBTA_FM_STATUS      status;     /* operation status */
    BOOLEAN             rds_on;
    BOOLEAN             af_on;  /* audio mode */
}tBTA_FM_RDS_MODE_INFO;

typedef struct
{
    UINT8               rssi;
    UINT16              freq;
}tBTA_FM_SCAN_DAT;

typedef struct
{
    tBTA_FM_STATUS      status;
    BOOLEAN             is_mute;
}tBTA_FM_MUTE_STAT;

typedef struct 
{
    tBTA_FM_STATUS          status;
    tBTA_FM_REGION_CODE     region;
}tBTA_FM_REGION_INFO;

/* Union of all FM callback structures */
typedef union
{
    tBTA_FM_STATUS          status;             /* BTA_FM_DISABLE_EVT/
                                                   BTA_FM_ENABLE_EVT/
                                                   BTA_FM_AUD_PATH_EVT/
                                                   BTA_FM_RDS_MODE_EVT call back data      
												   BTA_FM_AUD_PATH_EVTcall back data*/      
    tBTA_FM_CHNL_DATA       chnl_info;          /* BTA_FM_TUNE_EVT/BTA_FM_SEARCH_EVT */
    tBTA_FM_MODE_INFO       mode_info;          /* BTA_FM_AUD_MODE_EVT */
    tBTA_FM_AUD_DATA        audio_data;         /* BTA_FM_AUD_DATA_EVT call back data */
    tBTA_FM_DEEMPH_DATA     deemphasis;         /* BTA_FM_SET_DEEMPH_EVT call back data */
    tBTA_FM_RDS_MODE_INFO   rds_mode;           /* BTA_FM_RDS_MODE_EVT call back data */
    tBTA_FM_SCAN_DAT        scan_data;          /* BTA_FM_SEARCH_EVT call back data */
    tBTA_FM_MUTE_STAT       mute_stat;          /* BTA_FM_MUTE_AUD_EVT call back data */
    tBTA_FM_STEP_TYPE       scan_step;          /* BTA_FM_SCAN_STEP_EVT callback data */
    tBTA_FM_REGION_INFO     region_info;        /* BTA_FM_SET_REGION_EVT callback data */

    UINT8                   nfloor;
} tBTA_FM;

/* run-time configuration struct */
typedef struct
{
    UINT16                  low_bound;      /* lowest frequency boundary */
    UINT16                  high_bound;     /* highest frequency boundary */
    tBTA_FM_DEEMPHA_TIME    deemphasis;     /* FM de-emphasis time constant */
    UINT8                   scan_step;      /* scanning step */
} tBTA_FM_CFG_ENTY;

typedef struct
{
    tBTA_FM_CFG_ENTY        reg_cfg_tbl[BTA_MAX_REGION_SETTING];  
                                            /* region related setting table */
    INT32                   aud_timer_value;/* audio quality live updating timer 
                                               value, must be a non-0 positive value */
    INT32                   rds_timer_value;/* RDS data update timer */
    INT32                   enable_delay_val;/* FM enable delay time value, platform dependant */    
    UINT8                   rds_read_size;  /* how many RDS tuples per read */
    UINT8                   max_station;    /* maximum number of FM stations for a full band scan
                                                can not exceed BTA_FM_MAX_PRESET_STA */    
} tBTA_FM_CFG;

/* BTA FM callback function */
typedef void (tBTA_FM_CBACK)(tBTA_FM_EVT event, tBTA_FM *p_data);

/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function         BTA_FmEnable
**
** Description      Enable FM fuctionality.
**                  
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmEnable(tBTA_FM_FUNC_MASK func_mask,
                                 tBTA_FM_CBACK *p_cback, UINT8 app_id);

/*******************************************************************************
**
** Function         BTA_FmDisable
**
** Description      Disable FM functionality.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmDisable(void);

/*******************************************************************************
**
** Function         BTA_FmReadRDS
**
** Description      Read RDS information. For every BTA_FmReadRDS API call, 
**                  will receive one or more application BTA_FM_RDS_UPD_EVT event 
**                  call back. For each call back event, 50 or less tuples of RDS 
**                  data will be returned.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmReadRDS(void);

/*******************************************************************************
**
** Function         BTA_FmSearchFreq
**
** Description      Scan FM toward higher/lower frequency for next clear channel.
**                  If no clear channel is found, BTA_FM_SEARCH_EVT event with 
**                  BTA_FM_SCAN_FAIL status will be returned.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmSearchFreq(tBTA_FM_SCAN_MODE scn_mode, UINT8 rssi_thresh, 
                                    tBTA_FM_SCH_RDS_COND *p_rds_cond);

/*******************************************************************************
**
** Function         BTA_FmTuneFreq
**
** Description      Tune to a designated frequency, BTA_FM_TUNE_EVT will be returned 
**                  when tuning is finished.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmTuneFreq (UINT16 freq);

/*******************************************************************************
**
** Function         BTA_FmSearchAbort
**
** Description      Abort a frequency scanning operation. BTA_FM_SEARCH_EVT will
**                  be sent to application when operation is aborted, and the
**                  stopping frequency is reported with the event.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmSearchAbort (void);

/*******************************************************************************
**
** Function         BTA_FmSetAudioMode
**
** Description      Configure FM audio mode to be mono/stereo/blend. When operation 
**                  finishes, event BTA_FM_AUD_MODE_EVT will be returned.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmSetAudioMode ( tBTA_FM_AUDIO_MODE mode);

/*******************************************************************************
**
** Function         BTA_FmSetRDSMode
**
** Description      Turn on/off RDS feature and AF algorithm. 
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmSetRDSMode (BOOLEAN rds_on, BOOLEAN af_on, tBTA_FM_AF_PARAM *p_af_struct);


/*******************************************************************************
**
** Function         BTA_FmConfigAudioPath
**
** Description      Set FM audio path. When configuring FM audio over SCO, an
**                  existing SCO connection is requested.
**                  
** Parameters       audio_path: BTA_FM_SCO_OFF: disable FM over SCO.
**                              BTA_FM_SCO_ON: enable FM audio route over SCO via 
**                                              PCM FIFO.
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmConfigAudioPath(tBTA_FM_AUDIO_PATH audio_path);

/*******************************************************************************
**
** Function         BTA_FmReadAudioQuality
**
** Description      Turn on/off live audio data updating.
**                  
** Parameters       turn_on: TRUE: updating audio quality data(rssi, mode).
**                           FALSE: stop updaing audio quality data.
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmReadAudioQuality(BOOLEAN turn_on);


/*******************************************************************************
**
** Function         BTA_FmConfigDeemphasis
**
** Description      Config deemphasis parameter.
**                  
** Parameters       
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmConfigDeemphasis(tBTA_FM_DEEMPHA_TIME time_const);

/*******************************************************************************
**
** Function         BTA_FmMute
**
** Description      Mute/Unmute FM audio
**                  
** Parameters       TRUE:  mute audio
**                  FALSE: unmute audio
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmMute(BOOLEAN mute);

/*******************************************************************************
**
** Function         BTA_FmSetScanStep
**
** Description      Configure FM Scanning Step.
**                  
** Parameters       step: 50KHz/ 100KHz step
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmSetScanStep(tBTA_FM_STEP_TYPE step);
 /*******************************************************************************
**
** Function         BTA_FmEstNoiseFloor
**
** Description      Estimate noise floor.
**                  
** Parameters       
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmEstNoiseFloor(tBTA_FM_NFE_LEVL level);

/*******************************************************************************
**
** Function         BTA_FmSetRegion
**
** Description      Set a region for band selection. 
**                  
** Parameters       none
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void BTA_FmSetRegion (tBTA_FM_REGION_CODE region);
 
#ifdef __cplusplus
}
#endif

#endif /* BTA_FM_API_H */

