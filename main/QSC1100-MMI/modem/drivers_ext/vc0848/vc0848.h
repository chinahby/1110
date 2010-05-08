#ifndef VC0848_H
#define VC0848_H

#include "comdef.h"
#include "customer.h"
//#include "mmd_ply_driver.h"
/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/

/*  Signals declarations. How many signals used in VC0848 Func              */

/* ==========================================================================
INIT_DEVICE
CLOSE_DEVICE
BAPASS_MODE
SLEEP_MODE
NORMAL_MODE

SETPARM_MODE
GETPARM_MODE

PREVIEW_MODE
CAPTURE_MODE
PLAY_MODE
STOP_MODE
PAUSE_MODE
RESUME_MODE

AUDIO_REC
VEDIO_REC
FM_PA

1. INIT_MODE_SIG: init vc0848, and switch it to bypass mode for lcd display, it's only set when vc task run first.

2. MODE SWITCH_SIG: in different scene, it needs to switch mode for vc0848 or native lcd display
BYPASS_MODE: native lcd update 
NORMAL_MODE: vc0848 working mode
SLEEP_MODE: power saving mode

3. USER_TIMER_SIG: start user timer for vc0848 for play and record func etc.

4. STOP_SIG: stop camera or media func, close the power needn't but still in normal mode

5. CMD_Q_SIG: in normal mode, get command from top layer to control vc0848, it includes:
    SENSOR_POWER_ON
    SENSOR_POWER_OFF

    SPK_SEL_ENABLE
    SPK_SEL_DISABLE

    AUDIO_PA_POWER_ON
    AUDIO_PA_POWER_OFF

    FM_PA_ON
    FM_PA_OFF

    VDD2V9_ON 
    VDD_2V9_OFF

    VDD_CORE_1V2_ON
    VDD_CORE_1V2_OFF
    
    PREVIEW_ON
    PREVIEW_OFF

    CAPTURE_ON

    PLAY_ON
    PAUSE_ON
    RESUME_ON
    STOP_ON

    FS_POS_NATIVE
    FS_POS_SD

    SET_PARM{...}
    GET_PARM{...}

    ...

============================================================================= */


#define  VC_RPT_TIMER_SIG      0x0001      /* Watchdog report signal */
#define  VC_INIT_SIG           0x0002      /* Write timer signal */
#define  VC_CMD_Q_SIG          0x0004      /* Command queue signal */
#define  VC_STOP_SIG           0x0008      /* error log buffer siganl */
#define  VC_USER_TIMER_SIG     0x0010      /* User Timer for VC0848 */

#define  VC_UI_SIG             0x0020      /* UI call VC task Sig*/

#define  VC_TIMER_STOP_SIG     0x0040       /* timer stop sig */
#define  VC_TIMER_START_SIG    0x0080       /* timer start sig */

#define  VC_PMU_SWITCH_BYPASS_SIG    0x0100  /* PMU switch start sig */

#define  VC_RSVD_0200_SIG     0x0200
#define  VC_RSVD_0400_SIG     0x0400
#define  VC_RSVD_0800_SIG     0x0800

#define  EFS_FILE_NAME_LEN    128
//#define  TASK_STOP_SIG           0x4000
//#define  TASK_OFFLINE_SIG       0x2000

// keep the same size with VC0848, F_SECOTR_SIZE was defined as 512 bytes in fat_thin.h
#define SDFS_SECTOR_SIZE     512

/* define macro for oemmedia, this macro should keep same value with MMD_D2H_PLY_FILE_END*/
#define MMD_PLY_END          0x40E0 

/*=========================================================================*/
/* Enumerators of items to access.                                         */
/*=========================================================================*/
/* define play status for PLAY_CMD */
typedef enum{
  VC_NONE_M,
  VC_PLAY_M,
  VC_PAUSE_M,
  VC_RESUME_M,
  VC_STOP_M,
  VC_SEEK_M
} vc_play_mode;  

typedef enum{
  VC_BACKLIGHT_ALLOW,
  VC_BACKLIGHT_FORBID
} vc_backlight_state;

typedef enum{
  VC_NORMAL_M,
  VC_BYPASS_M,
  VC_CODEC_M,
  VC_SLEEP_M
} vc_work_mode;

typedef enum{
  VC_FSPOS_NATIVE,
  VC_FSPOS_SD
} vc_fspos_type;

typedef enum{
  VC_OP_OFF,
  VC_OP_ON
} vc_operate_state;


typedef enum{
  VC_DEVICE_CLOSE,
  VC_DEVICE_OPEN,
  VC_DEVICE_BUSY
} vc_device_state;

typedef enum{
  VC_SENSOR_FIND_ERR,
  VC_SENSOR_FIND_OK   
} vc_sensor_state;

typedef enum{
  VC_SD_FIND_ERR,
  VC_SD_FIND_OK
} vc_sd_state;

typedef enum{
  VC_RECORD_AUDIO,
  VC_RECORD_VEDIO
} vc_record_type;

typedef enum{
  VC_DEV_ASYNC_NONE          = 0x0000, 
  VC_DEV_CAMERA_ASYNC_EN     = 0x0001,
  VC_DEV_PLY_MP3_ASYNC_EN    = 0x0010,
  VC_DEV_PLY_AMR_ASYNC_EN    = 0x0100, 
  VC_DEV_CAMERA_ASYNC_DIS    = 0xFFFE,
  VC_DEV_PLY_MP3_ASYNC_DIS   = 0xFFEF,
  VC_DEV_PLY_AMR_ASYNC_DIS   = 0xFEFF 
} vc_dev_async_mod;

typedef enum{
  VC_DEV_NONE        = 0x0000, 
  VC_DEV_PLY_MP3     = 0x0001, // mp3
  VC_DEV_FM_PA       = 0x0002, // includes fm pa and fm record audio
  VC_DEV_CAMERA      = 0x0004, // includes record video, capture
  VC_DEV_PLY_AMR     = 0x0008, // mp4, 3gp video play
  VC_DEV_PLY_AMRAUDIO     = 0x0010,  // AMR      add by jian.wang 09.3.11
  VC_DEV_PLY_AAC     = 0x0020,  // AAC      add by jian.wang 09.3.20
  VC_DEV_UDISK       = 0x0040,
  VC_DEV_JPEG        = 0x0080,
  VC_DEV_SD          = 0x0100
} vc_dev_type;

typedef enum _VIM_FILE_TYPE{
  VIM_FILE_TYPE_JPG,
  VIM_FILE_TYPE_AMR,
  VIM_FILE_TYPE_3GP,
  VIM_FILE_TYPE_ERROR = 0XFF
}VIM_FILE_TYPE;

typedef enum {
  VC_CAM_SIZE_1600_1200,
  VC_CAM_SIZE_1280_960,     
  VC_CAM_SIZE_640_480,
  VC_CAM_SIZE_320_240,
  VC_CAM_SIZE_176_220 
} vc_cam_size;

typedef enum {
  VC_JPEG_ENTRY_CAM,
  VC_JPEG_ENTRY_FS,  
} vc_jpeg_entry;

typedef enum {
  VC_IN_CORE_RAM,
  VC_IN_CORE_FILE,
  VC_IN_BASEBAND_RAM,
  VC_IN_BASEBAND_FILE
} vc_file_scr_type;

typedef enum {
  VC_TIMER_RUN     = 0x00,
  VC_TIMER_STOP    = 0x01,
  VC_TIMER_RELEASE = 0x02,
} vc_timer_state;

typedef enum {
  VC_F_SEEK_SET=0, /*Beginning of file*/
  VC_F_SEEK_CUR=1, /*Current position of file pointer*/
  VC_F_SEEK_END=2  /*End of file*/
} vc_seek_type;

typedef enum {
  VC_AUDIO_CHN_SPEAKER,
  VC_AUDIO_CHN_EARPHONE,
  VC_AUDIO_CHN_PCM
} vc_audio_channel;

/* define the parm struct member */
typedef struct{
  int32 p1;
  int32 p2;
  vc_file_scr_type file_scr;
  char  szStr[EFS_FILE_NAME_LEN];
} vc_func_parm;

typedef struct 
{
   int32    cx;
   int32    cy;
} vc_frame_parm;

typedef struct {
   vc_play_mode     play_mode;
   int              fileLen;
   vc_cam_size      cam_size;
   vc_jpeg_entry    entry;
   vc_file_scr_type file_scr;
   char             szFileName[EFS_FILE_NAME_LEN];
   uint8 * pFileaddr;   
} vc_play_info;

typedef struct {
	uint32 total;
	uint32 free;
	uint32 used;
	uint32 bad;
} vc_fs_space; /* the unit of space is F_SECTOR_SIZE-byte */

typedef struct {
   uint32      handle; // in and out put parm for oemfs
   uint32      dwCreationDate; // out parm, create date in second
   uint32      dwReadWrite; // in and out parm, bytes in read or write 
   uint32      dwSize; // out parm, file size for get info
   vc_fs_space space; // out parm, struct for read sd state
   char        szName[EFS_FILE_NAME_LEN]; // out parm, 
   char        attrib; // out parm, 
   char        nErr; // output parm returned for oemfs
   void *      buffer; // in and out parm, buffer for read or write
} efs2_info;

typedef struct {
   efs2_info efs2; // output parm, 
   char nMode[3]; // mode for file open
   char bDirs; // input parm, dir or filename
   char szDir[EFS_FILE_NAME_LEN];
   char szNewDir[EFS_FILE_NAME_LEN]; // the name to be renamed
   vc_seek_type nSeekMode;
   uint32 nOffset;
} vc_fs_info;

/* define items type */
typedef enum{
  VC_ITM_NONE_I,             /* item none */
  VC_ITM_DEV_OPEN_I,         /* device vc0848 open func, it includes: camera open, media open, fmpa and bt_mp3 */
  VC_ITM_DEV_STOP_I,         /* device VC0848 stop func */
  VC_ITM_MODE_I,             /*  device working mode switch among bypass, normal and sleep func*/
  VC_ITM_UDISK_I,            /* UDisk Func */
  VC_ITM_VDD_2V9_I,          /*  VDD2V9 LDO control func */
  VC_ITM_PREVIEW_I,          /* camera preview mode control func */
  VC_ITM_REC_SNAPSHOT_I,     /* camera capture operation func */
  VC_ITM_REC_AUDIO_I,        /* record audio in amr format */
  VC_ITM_REC_VIDEO_I,        /* record video file in 3gp format */
  VC_ITM_JPG_DISPCLR_I,       /*CHANGE display mode*/
  VC_ITM_JPG_DECODE_I,       /* jpeg decodec */
  VC_ITM_JPG_STOP_DECODE_I,  /* jpeg decodec stop*/
  VC_ITM_PLAY_MODE_I,        /* camera or media play status control among paly, stop, pause and resume func */
  VC_ITM_FS_POS_I,           /* fs position control on native efs or sd func */
  VC_ITM_DEV_STAT_I,         /* current vc0848 is running on which device */
  VC_ITM_SENSOR_STAT_I,      /*  get sensor status if it exist or not, can not use method CONTROL_I */
  VC_ITM_SD_STAT_I,          /*  get sd card status if it exist or not, can not use method CONTROL_I */
  VC_ITM_BACKLIGHT_STAT_I,   /* get backlight state to allow or forbid backlight */
  VC_ITM_SET_CAP_SIZE_I,     /* set camera capture size func */
  VC_ITM_SET_MEDIA_VOL_I,    /* set volume for media playing */
  VC_ITM_SET_FMPA_VOL_I,     /* set volume for fm pa */
  VC_ITM_SET_EFFECT_I,       /* set effect for camera */
  VC_ITM_SET_BRIGHTNESS_I,   /* set brightness for capture*/
  VC_ITM_SET_ENVIRONMENT_I,  /* set environment for capture*/
  VC_ITM_SET_BANDING_I,      /* set banding for camera */
  VC_ITM_SET_AUDIO_CHANNEL_I,/* set audio channel among speaker, earphone and pcm output */ 
  VC_ITM_GET_PARM_I,         /* get camera parm func */
  VC_ITM_GET_PLAY_TIME_I,    /* get current time played, just after play_paused call */
  VC_ITM_GET_PLAY_TOTALTIME_I, /* get the media total time for play */
  VC_ITM_FS_ENUM_INIT_I,    /* sdfs enum init */
  VC_ITM_FS_ENUM_NEXT_I,    /* sdfs enum next */
  VC_ITM_FS_MKDIR_I,    /* create dir in sd */
  VC_ITM_FS_RMDIR_I,    /* delete dir in sd */
  VC_ITM_FS_CREATE_FILE_I,   /* create file in sd */
  VC_ITM_FS_DELETE_FILE_I,   /* delere file in sd */
  VC_ITM_FS_GET_INFO_I,      /* get file info */
  VC_ITM_FS_SEEK_I,          /* seek file */
  VC_ITM_FS_TRUNCATE_I,      /* truncate file */
  VC_ITM_FS_OPEN_I,          /* open file */
  VC_ITM_FS_CLOSE_I,         /* close file */
  VC_ITM_FS_READ_I,          /* read file */
  VC_ITM_FS_WRITE_I,         /* write file */
  VC_ITM_FS_TEST_I,          /* test file or dir exist or not */
  VC_ITM_FS_SPACE_I,         /* get fs space info */
  VC_ITM_FS_RENAME_I,        /* rename file */
  VC_ITM_FS_GET_ATTR_I,      /* get attribute */
  VC_ITM_FS_TELL_I,          /* tell file position */
  VC_ITM_FS_GET_OPENFILE_SIZE_I,  /* get open file size */
  VC_ITM_FS_GET_DATETIME_I,  /* get creation date and time for specified file */
  VC_ITM_FS_SEEK_APPEND_I,   /* offset is great than filesize, add zero and seek to the specified position */
  VC_ITM_DISP_UPDATE_I,      /* lcd update in normal state */
  VC_ITM_MAX_I
} vc_items_type;


/*=========================================================================*/
/*  Returned status codes for requested operation.                         */
/*=========================================================================*/
typedef enum {
  VC_STAT_DONE,            /* Request completed okay */
  VC_STAT_BUSY,             /* task is busy, query state later */
  VC_STAT_INUSE,         /* device was in use */
  VC_STAT_BADCMD,        /* Unrecognizable command field */
  VC_STAT_BADPARM,       /* Bad parameter in command block */
  VC_STAT_BADSTATE,      /* bad Work mode state */
  VC_STAT_NO_SENSOR,     /* no sensor currently */
  VC_STAT_NO_SD,         /* no sd currently */
  VC_STAT_FAIL,              /* Command failed */
  VC_STAT_MAX
} vc_status_type;


/*  !!define function macro, careful!!  */
typedef enum {
  VC_FUNC_CONTROL,     /* device VC0848 control func, can not control DEV_STAT, SD_STAT andSENSOR_STAT */
  VC_FUNC_SET_PARM,    /* set camera parm func, can not operate on DEV_STAT, SD_STAT and SENSOR_STAT */
  VC_FUNC_GET_PARM,    /* get camera parm func, can only get DEV_STAT, SD_STAT and SENSOR_STAT, get other parm will return EBADPARM */
  VC_FUNC_CAMERA_ON,   /* set camera record/ captur or preview func on */
  VC_FUNC_PLAY_ON,     /* play media file */
  VC_FUNC_FS_ON,       /* access SD fs system */
  VC_FUNC_FMPA,        /* fm pa and fm recorder  */
  VC_FUNC_DISP_ON,     /* lcd update func */
  VC_FUNC_MAX
} vc_func_type;

typedef enum {
  VC_PREV_NONE, 
  VC_PREV_SNAPSHOT,
  VC_PREV_MOVIE,
  VC_PREV_MAX
} vc_preview_type;

typedef enum{
  VC_PREV_NOFRAME,    /*0 preview with no frame*/
  VC_PREV_WITHFRAME,  /*1 preview with frame*/
  VC_PREV_FRAMEMAX
}vc_preview_frametype;

typedef enum {
  VC_DEV_PRI_LEV_0, // lowest priority level
  VC_DEV_PRI_LEV_1,
  VC_DEV_PRI_LEV_2,
  VC_DEV_PRI_LEV_3, 
  VC_DEV_PRI_LEV_4,
  VC_DEV_PRI_LEV_5, // highest priority level
  VC_DEV_PRI_LEV_MAX  
} vc_dev_pri_level;

/* Note: keep vc_fs_err_status definition same with fat_thin.h */
typedef enum {
  VC_FS_NO_ERROR 				= 0, 	/* operation successful*/
  VC_FS_ERR_RESERVED 			= 1, 	/* reserved */
  VC_FS_ERR_NOTFORMATTED		= 2, 	/* the media is not formatted */
  VC_FS_ERR_INVALIDDIR			= 3,  	/* invalid directory path */
  VC_FS_ERR_INVALIDNAME		    = 4, 	/* invalid name of file or directory */
  VC_FS_ERR_NOTFOUND			= 5, 	/* file or directory is not found */
  VC_FS_ERR_DUPLICATED			= 6, 	/* file or directory is already existed */
  VC_FS_ERR_NOMOREENTRY		    = 7, 	/* no more space for new file entry */
  VC_FS_ERR_NOTOPEN			    = 8, 	/* file is not opened */
  VC_FS_ERR_EOF				    = 9, 	/* reachs the end of file */
  VC_FS_ERR_NULLPOINTER		    = 10, 	/* cannot allocate memory or null-pointer variable */
  VC_FS_ERR_NOTUSEABLE			= 11, 	/* invalid parameter or option */
  VC_FS_ERR_LOCKED				= 12, 	/* file or directory is locked by current operation */
  VC_FS_ERR_ACCESSDENIED		= 13, 	/* media access denied */
  VC_FS_ERR_NOTEMPTY			= 14, 	/* directory is not empty */
  VC_FS_ERR_INITFUNC			= 15, 	/* media initializtion failed */
  VC_FS_ERR_CARDREMOVED		    = 16, 	/* media is removed */
  VC_FS_ERR_ONDRIVE			    = 17, 	/* media drive does not work */
  VC_FS_ERR_INVALIDSECTOR		= 18, 	/* invalid sector of current media */
  VC_FS_ERR_READ				= 19, 	/* read sector(s) failed */
  VC_FS_ERR_WRITE				= 20, 	/* write sector(s) failed */
  VC_FS_ERR_INVALIDMEDIA		= 21, 	/* invalid media format */
  VC_FS_ERR_BUSY				= 22, 	/* media is busy */
  VC_FS_ERR_WRITEPROTECT		= 23, 	/* media is write protected */
  VC_FS_ERR_INVFATTYPE			= 24, 	/* invalid FAT type */
  VC_FS_ERR_MEDIATOOSMALL		= 25, 	/* the size of media is too small to format */
  VC_FS_ERR_MEDIATOOLARGE		= 26, 	/* the size of media is too large to format */
  VC_FS_ERR_NOTSUPPSECTORSIZE	= 27,	/* unsupported sector size, only 512-byte sector is supported in this version */
  VC_FS_ERR_CANNOTOPEN          = 28,   /* open file failed */
  VC_FS_ERR_DEVMALLOCFAIL       = 29,   /* VC0848 SRAM malloc fail */
  VC_FS_ERR_NOMOREHANDLE        = 30,   /* no more vacant handle for opening file */
  VC_FS_ERR_GETSECTORCHAIN      = 31,   /* get sector chain fail */
  VC_FS_ERR_ALLOCSECTORCHAIN    = 32,   /* alloc sector chain fail */
  VC_FS_ERR_ACKTIMEOUT          = 33,   /* wait command ack timeout */
  VC_FS_ERR_FASTCOPYBUSY        = 34,   /* can't start a new fast copy for there is already one under processing */
  VC_FS_ERR_FASTCOPYNOTPAUSE    = 35,   /* can't resume a fast copy that is not paused */
  VC_FS_ERR_CORESDRW            = 36,   /* VC0848 Core SD read or write error */
  VC_FS_ERR_NOMOREDATA          = 37,   /* no more data for file reading */
  VC_FS_ERR_NOMORESPACE         = 38    /* no more space for file writing */
} vc_fs_err_status;

typedef enum {
  VC_JPEG_DECODE_INIT,
  VC_JPEG_DECODE_DOING,
  VC_JPEG_DECODE_DONE
} vc_jpeg_decode;

typedef enum {
   EFS2_FA_NORMAL     = 0,
   EFS2_FA_HIDDEN     = 0x0001,
   EFS2_FA_DIR        = 0x0002,
   EFS2_FA_READONLY   = 0x0004,
   EFS2_FA_SYSTEM     = 0x0008,
   EFS2_FA_CONST      = 0x0010,
   EFS2_FA_FIXED      = 0x0020,
   EFS2_FA_NODIR      = 0x0040,
   EFS2_FA_NOFILE     = 0x0080
} vc_efs2_attr;

typedef struct {
  unsigned int     display_width;    
  unsigned int     display_height;         
  unsigned int     display_xpos;        
  unsigned int     display_ypos;
  unsigned int     display_effect;
} preview_outinfo;
   

typedef struct {
  preview_outinfo      prev_info;
  vc_frame_parm        frame;
  vc_preview_type      prev_type;
  vc_preview_frametype prev_frametype;
  int16                quality;
} vc_prev_parm_type;

typedef struct {
  vc_dev_pri_level dev_pri;
  vc_dev_type      curr_dev;
  vc_items_type    sub_item;
} vc_dev_info;

typedef struct {
  const void *buf_ptr;
  signed short src_width;
  signed short src_starting_row;
  signed short src_starting_column;
  signed short num_of_rows;
  signed short num_of_columns;
  signed short dst_starting_row;
  signed short dst_starting_column;
} vc_disp_type;

/*=========================================================================*/
/* VC Items Union Type.                                                   */
/*=========================================================================*/
typedef /*PACKED*/ union {
  vc_operate_state    clk;               /*  */
  vc_operate_state    sensor_pwr; /* sensor power, two status: on or off */
  vc_operate_state    spk_sel;       /* vc0848's speaker or native qc speaker selection, two status: TRUE-VC0848 spk, FALSE-QC spk */
  vc_operate_state    audio_pa;    /* QC audi_pa, two status: on or off */
  vc_operate_state    vdd_2v9;      /* VDD_2V9 power, two status: 0- VDD_2V9 off, 1-VDD_2V9 on */
  vc_operate_state    vcore_1v2;   /* VDD_CORE_1V2, two status: 0-VDD_CORE_1V2 off, 1-VDD_CORE_1V2 on */
  //vc_operate_state    fmpa;   /* fm pa control, if 1, on; if 0, off */
  vc_operate_state    preview;       /* preview two status: 0-preview off, 1 preview on */
  //vc_operate_state    capture;        /* capture one frame, 1 capture on frame and save file, 0- do nothing */
  vc_operate_state    udisk;       /* UDisk status */ 
  vc_func_parm        parm;          /* parm for get or set func */
  vc_play_mode        play_mode;  /* play state, four state */
  vc_work_mode        pmu_mode;           /* device mode switch among bypass, normal and sleep mode */
  vc_fspos_type       fs_pos;        /* FS access point, two status: 0-QC nor flash fs, 1-external sd */
  vc_dev_info         dev_run;      /* device type: FMPA, BT_MP3, CAMERA and MEDIA */
  //vc_record_type      record_type;   /* record file format */
  vc_sensor_state     sensor;        /* sensor find succeess or not */
  //vc_operate_state    rec_video;    /* record video  */
  vc_sd_state         sd;           /* detect sd exist or not */
  vc_prev_parm_type   prev_parm; /* preview init parm */
  vc_play_info        play_info;  /* set media file info for play func */
  vc_backlight_state  backlight; /* get the backlight if it allows backlight set disable or not */
  vc_fs_info          fs_info;   /* fs_info for sd fs enum_init and enum_next */
  vc_disp_type        disp_info; /* disp info for lcd update */
} vc_union_type;


/*===========================================================================
FUNCTION: VC_DeviceControl

DESCRIPTION
  Control the device except parm GET/SET opertion, this func is designed for external application accessing out side of vc_task.
  but now,  we still put the implement of GET/SET operation in this func.
  vc_items_type<item>:
    VC_DEV_STOP_I,               device VC0848 stop func
    VC_MODE_I,                     device working mode switch among bypass, normal and sleep func
    VC_CLK_I,                          device vc0844 clk control func 
    VC_UDISK_I,                   UDisk Func 
    VC_SENSOR_PWR_I,         sensor power control func 
    VC_SENSOR_STAT_I,       get sensor status if it exist or not func 
    VC_SD_STAT_I,                 get sd card status if it exist or not func
    VC_SPK_SEL_I,               speaker select between QC native speaker or VC0848 speaker func
    VC_AUDIOPA_PWR_I,      audio pa power control func 
    VC_FMPA_PWR_I,            FM PA control func
    VC_VDD_2V9_I,                 VDD2V9 LDO control func 
    VC_VCORE_1V2_I,           VCORE_1V2 control func 
    VC_PREVIEW_I,                camera preview mode control func
    VC_CAPTURE_I,              camera capture operation func 
    VC_PLAY_I,                     camera or media play status control among paly, stop, pause and resume func 
    VC_FS_POS_I,                 fs position control on native efs or sd func
    VC_SET_PARM_I,             set camera parm func 
    VC_GET_PARM_I,         get camera parm func 
    
  vc_func_type<func>:
    VC_CONTROL_F,             device VC0848 control func 
    VC_SET_PARM_F,          set camera parm func 
    VC_GET_PARM_F,          get camera parm func 

  vc_union_type<*data_ptr>:
    input or output parm this this point viarable
    
RETURN VALUE
  The VC return code, which is handled internally.

DEPENDENCIES
  This func will change the item status, so be careful.
===========================================================================*/
extern vc_status_type VC_DeviceControl
(
  vc_items_type   item,                  /* which item */
  vc_func_type    func,                  /* which function */
  vc_union_type   *data_ptr           /* what the action is */
);

extern vc_status_type VC_DeviceControlAsync
(
  vc_items_type   item,                  /* which item */
  vc_func_type    func,                    /* which function */
  vc_union_type   *data_ptr           /* what the action is */
);

/*===========================================================================
FUNCTION: VC_GetDeviceState
   return the operation result for UI APP. Ok, this func should be called after VC_DeviceControlAsync.

   return value:
      VC_DEVICE_CLOSE: device was in close status
      VC_DEVICE_OPEN:  device was opened status
      VC_DEVICE_BUSY:  device was in busy status

DISE EFFECT:
   NONE
===========================================================================*/
extern vc_status_type VC_GetDeviceState(void);
extern vc_dev_async_mod VC_GetDeviceAsyncMode(void);
extern void VC_SetDeviceAsyncMode(vc_dev_async_mod async_mod);
extern vc_dev_type VC_GetCurrentDevice(void);
extern vc_play_mode VC_GetPlayMode(void);
extern void VC_SetTimerSig(unsigned long sig);
extern void OEMMedia_VC_PlayCB(unsigned int command);
extern void VC_SetPlayMode(vc_play_mode mode);
extern void VC_SetTimerState(vc_timer_state timer_state);
extern void 
vc_task 
(
  dword param      /* parameter from REX, unused */
);

extern boolean VC_GetVideoRecorderDoneState(void);
extern void    VC_SetVideoRecorderDoneState(boolean bState);
#endif /* VC0848_H */
