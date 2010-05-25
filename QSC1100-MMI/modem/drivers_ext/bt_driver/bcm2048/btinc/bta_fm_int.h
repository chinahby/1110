/*****************************************************************************
**
**  Name:           bta_fm_int.h
**
**  Description:    This is the private file for the FM module.
**
**  Copyright (c) 2006, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_FM_INT_H
#define BTA_FM_INT_H

#include "bt_target.h"
#include "bta_sys.h"
#include "bta_fm_api.h"
#include "bta_fm_af.h"
#include "bta_rds_api.h"

#include "hcidefs.h"
/*****************************************************************************
**  Constants and data types
*****************************************************************************/
#ifndef BTA_FM_RDS_FLAG_TIME_VAL
#define BTA_FM_RDS_FLAG_TIME_VAL        15000        /* FM RDS flag update expiring time */
#endif

enum
{
    BTA_FM_API_ENABLE_EVT       = BTA_SYS_EVT_START(BTA_ID_FM),
    BTA_FM_API_DISABLE_EVT,
    BTA_FM_API_READ_RDS_EVT,
    BTA_FM_API_SFREQ_EVT,
    BTA_FM_API_TFREQ_EVT,
    BTA_FM_API_AUD_MODE_EVT,
    BTA_FM_API_MUTE_EVT,
    BTA_FM_API_SCANSTEP_EVT,
    BTA_FM_API_RDS_MODE_EVT,
    BTA_FM_API_AUD_PATH_EVT,
    BTA_FM_API_AUD_DATA_EVT,
    BTA_FM_API_DEEMPHA_EVT,  
    BTA_FM_INT_ENABLE_EVT,
    BTA_FM_VSC_CMPL_EVT,
    BTA_FM_INT_DISABLE_EVT,
    BTA_FM_INT_CMPL_EVT,
    BTA_FM_RDS_FLAG_TOUT_EVT,       /* FM_RDS_FLAG updating timeout event */
    BTA_FM_UPD_TOUT_EVT,
    BTA_FM_RDS_READ_TOUT_EVT,
    BTA_FM_INTRP_EVT,               /* receive interrupt event */ 
    BTA_FM_RDS_CHK_TOUT_EVT,
    BTA_FM_RDS_SCAN_EVT,
    BTA_FM_SCAN_CMPL,
    BTA_FM_API_ABORT_EVT,
    BTA_FM_INT_AF_JUMP_EVT,
    BTA_FM_AUD_PAUSE_EVT,
    BTA_FM_AF_NEXT_ATMP_EVT,
    BTA_FM_AF_JUMP_CMPL_EVT,
    BTA_FM_ENABLE_TOUT_EVT,
    BTA_FM_API_SET_REG_EVT,
    BTA_FM_API_NFE_EVT
};
typedef UINT16 tBTA_FM_INT_EVT;			/* BTA FM internal state machine events */

#define BTA_FM_KICK_OFF_EVT		0x80	/* BTA FM internal event complete bit */

#define BTA_FM_INVALID_EVT      0xff	/* invalid event */


#define BTA_FM_LIVE_UPD_ON  0x01					/* FM audio quality live updating on bit */
#define BTA_FM_AF_UPD_ON    0x02					/* FM AF algorithm on */
#define BTA_FM_CHNL_ON      0x10					/* FM clear channel on play bit */

/* state machine states */
enum
{
  BTA_FM_IDLE_ST = 0,       /* Disabled state  */
  BTA_FM_ACTIVE_ST,         /* Active State */
  BTA_FM_BUSY_ST,            /* Busy state */
  BTA_FM_MAINT_ST,
  BTA_FM_QUEUE_ST,
  BTA_FM_JUMP_ST,
  BTA_FM_SEARCH_ST
};
typedef UINT8   tBTA_FM_STATE;

#define BTA_FM_SCAN_DUAL_OFFSET         50        /* default minimum scanning step */
#define BTA_FM_SCAN_STEP                100     /* scanning step */

#define BTA_FM_CVT_FREQ(x)      (UINT16)((x + 64000 ) / 10)

/* FM enable API parameter */
typedef struct
{
    BT_HDR              hdr;
    UINT8               app_id;            /* application ID            */
    tBTA_FM_CBACK       *p_cback;        /* BTA FM callback function */
    tBTA_FM_FUNC_MASK   func_mask;        /* BTA FM functionality mask*/

}tBTA_FM_API_ENABLE;

/* FM tuning API parameter */
typedef struct
{
    BT_HDR          hdr;
    UINT16          freq;                /* destination frequency */
}tBTA_FM_API_TUNE;

/* FM search API parameter */
typedef struct 
{
    BT_HDR                  hdr;
    tBTA_FM_SCAN_MODE       sch_mode;
    UINT8                   rssi_thresh;
    tBTA_FM_SCH_RDS_COND    *p_rds_cond;    
} tBTA_FM_API_SEARCH;

/* FM set RDS mode API parameter */
typedef struct
{
    BT_HDR           hdr;
    BOOLEAN          rds_on;                /* turn on/off RDS            */
    BOOLEAN          af_on;                /* turn on/off AF            */
    tBTA_FM_AF_PARAM af_param;            /* AF filter                */
} tBTA_FM_RDS_MODE;

/* FM set ON/OFF config data, parameter is BOOLEAN */
typedef struct
{
    BT_HDR           hdr;
    BOOLEAN          turn_on;                /* turn on/off Audio live updating/mute            */
} tBTA_FM_BOOL_DATA;

typedef union
{
    BT_HDR                   hdr;
    tBTA_FM_API_ENABLE       enable_api;    /* Enable FM API    */
    tBTA_FM_API_TUNE         api_tune;        /* tuning API        */
    tBTA_FM_RDS_MODE         rds_mode;        /* set RDS mode API */
    tBTA_FM_API_SEARCH       search;
    tBTA_FM_BOOL_DATA        bool_data;     /* used by BTA_FM_API_MUTE_EVT */
} tBTA_FM_DATA;


#define BTA_FM_MAX_VCMD_QUEUED      10            /* vendor specific commands limit in queue */

/* bit mask set in vendor specific command detail chk_type field */
#define BTA_FM_REG_WRITE            0x00        /* write I2C registry                    */    
#define BTA_FM_REG_READ             0x01        /* read I2C registry                    */
#define BTA_FM_SYNC_VCMD_BIT        0x80        /* synchronous I2C write command    */
                                                /* asynchronous write cmd if the highest bit is not set*/
#define BTA_FM_ASYNC_READ_VCMD_BIT  0x80        /* asynchronous I2C read command    */
                                                /* synchronous read when this bit is not set */

#define BTA_FM_REG_OP_MASK          0x81        /* VS command operation mask            */

#define BTA_FM_VCMD_NONE_FLAG       0x00        /* no vendor specific command attribute */
#define BTA_FM_VCMD_LEN1_BIT        0x10        /* one byte vendor specific command        */
#define BTA_FM_VCMD_LEN2_BIT        0x20        /* two bytes vendor specific command    */
#define BTA_FM_VCMD_PENDING_BIT     0x40        /* last VS cmd for a pending call back event */
#define BTA_FM_VCMD_LEN_MASK        (BTA_FM_VCMD_LEN1_BIT|BTA_FM_VCMD_LEN2_BIT)
                                                /* vendor specific length mask            */

/* vendor specific command & callback parameter length and byte index */
#define BTA_FM_VCMD_HDR_LENGTH            2 /* one byte I2C address + 1 byte read/wriet opcode 
                                            for every vendor specific command */

#define BTA_FM_VCMD_DATA_LENGTH            2    /* maxinum vcmd data length*/
#define BTA_FM_VCMD_DATA_1ST_BYTE        0    /* vendor specific cmd first byte data index */
#define BTA_FM_VCMD_DATA_2ND_BYTE        1    /* vendor specific cmd second byte data index */

#define BTA_FM_RDS_GROUP_LEN            12  /* length of a RDS group based on RDS tuple */
#define BTA_FM_RDS_TUPLE_LENGTH            3    /* length of RDS tuple retreived from RDS FIFO */
#define BTA_FM_RDS_TUPLE_BYTE1            0    /* 1st byte of a RDS tuple index */
#define BTA_FM_RDS_TUPLE_BYTE2            1    /* 2nd byte of a RDS tuple index */
#define BTA_FM_RDS_TUPLE_BYTE3            2    /* 3rd byte of a RDS tuple index */

/* vendoe specific command complete callback data */
#define BTA_FM_VCMD_CBACK_HDR_LENGTH    3    /* all vcmd cback has 3 bytes data: status, i2c address & op type */
#define BTA_FM_VCMD_CBACK_STS            0    /* vcmd complete status byte */
#define BTA_FM_VCMD_CBACK_I2C            1    /* vcmd I2C address byte */
#define BTA_FM_VCMD_CBACK_OPT            2    /* vcmd operation type byte */
#define BTA_FM_VCMD_CBACK_PARAM0        3    /* read I2C registry callback parameter 0 */
#define BTA_FM_VCMD_CBACK_PARAM1        4    /* read I2C registry callback parameter 1 */

#define BTA_FM_RDS_UPD_ON               0x01
#define BTA_FM_AF_ALGO_ON               0x10
#define BTA_FM_RDS_STAT_MASK            0x0f

#define BTA_FM_RSSI_STER_IDX        0           /* RSSI threshold config index: stereo mode */
#define BTA_FM_RSSI_MONO_IDX        1           /* RSSI threshold config index: mono mode */

#define BTA_FM_SCAN_PTY_MATCH(f)    (UINT16)(f << 5)    /* b5 ~ b9 is PTY in B-Block */
#define BTA_FM_SCAN_PTY_MASK        BTA_FM_SCAN_PTY_MATCH(0x001f)   
#define BTA_FM_SCAN_TP_MATCH        (1 << 10)           /* 11th bit of B-block for TP */
#define BTA_FM_SCAN_TP_MASK         (1 << 10)           /* 11th bit of B-block for TP */

#define BTA_FM_SCAN_DIR_MASK    0xf0            /* high 4 bits of search_direction */

#ifndef BTA_FM_RSSI_THRESH
#define BTA_FM_RSSI_THRESH     0x55       /* RSSI threshhold value 85dBm */
#endif

#define BTA_FM_STEP_MASK        0xf0        /* dual scan step mask */
#define BTA_FM_SCAN_STAT_MASK   0x0f        /* dual scan status mask */
#define BTA_FM_SCAN_DUAL_ACT    0x01        /* dual scan is active */


/* default RDS BER rate, which is 0.3 * 1000 = 300 scaled into Q(13 + 10 = 23)*/
#ifndef BTA_FM_RDS_BER_DEFAULT
#define BTA_FM_RDS_BER_DEFAULT        (300 << 13)
#endif

/* max number of bytes to be read from RDS FIFO */
#define BTA_FM_RDS_FIFO_MAX             240

#define BTYES_TO_UINT16(u16, p) {u16 = (UINT16)(((UINT16)(*(p)) << 8) + (UINT16)(*((p) + 1))); }

/* maximum noise floor sample channel number */
#define     BTA_FM_NFE_SIZE_MAX     200     /* max number of RSSI samples to calculate NFL */
#define     BTA_FM_NFE_SAMPLE_MIN   50      /* min number of RSSI samples */

/* vendor specific command detail structure */
typedef struct
{
    UINT8                   data[BTA_FM_VCMD_DATA_LENGTH];    
                                        /* vendor specific command data, max 2 bytes for a 
                                            2-bytes write vcmd */
    UINT8                   i2c_addr;    /* read/write I2C registrer address */
    UINT8                   chk_type;    /* VS command attributes */
} tBTA_FM_VCMD_DATA;

/* vendor specific commands queue */
typedef struct
{
    tBTA_FM_VCMD_DATA       cmd[BTA_FM_MAX_VCMD_QUEUED];    /* command queue in array */
    UINT8                   rear;        /* queue rear index */
    UINT8                   head;		/* 1st uncomfirmed VS cmd index */
    UINT8                   next;       /* 1st not executed VS cmd index */
} tBTA_FM_VCMD_Q;

/* AF related definition */
typedef struct 
{
    UINT32                  stats[4];
    UINT32                  total;
}tBTA_FM_RDS_BER_STAT;

#if (BTA_FM_AF_INCLUDED && BTA_FM_AF_INCLUDED == TRUE)
#define BTA_FM_AF_CMPL_PENDING      0x80  /* jump complete finalization bit */
#define BTA_FM_AF_JUMP_STARTED      0x40  /* AF Jump has started */
#endif

/* FM/RDS flag bits used with bta_fm_cb.fm_rds_flag */
#define     BTA_FM_RDS_FLAG_CLEAN_BIT       0x01    /* clean FM_RDS_FLAG register */
#define     BTA_FM_RDS_FLAG_SCH_FRZ_BIT     0x02    /* interrupt freeze */
#define     BTA_FM_RDS_FLAG_SCH_BIT         0x04    /* pending search_tune */

/* FM control block */
typedef struct
{
    tBTA_FM_AF_CB           af_cb;          /* AF algorithm related control variables */
    UINT16                  freq;			/* currently tuned station frequency */
    tBTA_FM_FUNC_MASK       func_mask;		/* FM module feature */
    tBTA_FM_CBACK           *p_cback;		/* call back function  */
    tBTA_FM_STATE           state;          /* FM state machine state */
    UINT8                   status;         /* FM operation status */
    UINT8                   app_id;         /* application ID */
    UINT8                   pending_evt;    /* currently pending event */
    UINT8                   rssi;			/* RSSI value of currently tuned station */
    UINT8                   pcm_reg;		/* PCM route register value */
    UINT16                  aud_ctrl;       /* current audio control */
    UINT8                   live_upd_on;    /* audio quality data live update flag */
    BOOLEAN                 enabled;

    UINT8                   fm_rds_flag;    /* FM/RDS action flag */
    tBTA_FM_REGION_CODE     region;         /* region code of current FM setting */
    UINT16                  maint_mask;

    UINT8                   rds_af_mode;

    /* search control variables */
    UINT8                   sch_rds;        /* search mode */
    tBTA_FM_SCH_RDS_COND    sch_cond;       /* search condition */
    tBTA_FM_SCAN_MODE       sch_mode;        /* search mode */
    BOOLEAN                 sch_abort;
#if (defined BTA_FM_SCAN_STEP_ALGO && BTA_FM_SCAN_STEP_ALGO == TRUE)
    UINT8                   sch_dual;
    UINT16                  sch_start_freq;  /* scan start frequency */
    UINT16                  sch_100_freq;   /* current 100KHz stepping frequency */
    UINT16                  sch_50_freq;    /* current 50KHz stepping frequency */
    UINT16                  sch_freq;       /* temp searched frequency for dual scan  */
    tBTA_FM_STATUS          sch_status;     /* dual search temp status */
    UINT8                   sch_rssi;       /* temp searched rssi for dual scan */
#endif
    UINT8                   rssi_thresh;
    UINT8                   nfloor;

    tBTA_FM_VCMD_Q          vcmd_q;            /* vendor specific command queue */
    tBTA_FM_DATA            q_evt_data;     /* event queued in the stack */
    BOOLEAN                 q_waiting;      /* flag indicating if there is an queued event */

    tBTA_FM_AUDIO_MODE      audio_mode;        /* FM audio output control */
    tBTA_FM_AUDIO_QUALITY   fm_signal;      /* FM audio signal detected */
    
    tBTA_FM_AUDIO_PATH      audio_path;        /* audio routing status */

    TIMER_LIST_ENT          fm_reg_timer;     /* FM_RDS register updating timer */
    TIMER_LIST_ENT          aud_timer;          /* audio quality live updating timer */
    TIMER_LIST_ENT          rds_read_timer;     /* read RDS data timer */
} tBTA_FM_CB;



extern tBTA_FM_CFG *p_bta_fm_cfg;

/*****************************************************************************
**  Global data
*****************************************************************************/

/* FM control block */
#if BTA_DYNAMIC_MEMORY == FALSE
extern tBTA_FM_CB  bta_fm_cb;
#else
extern tBTA_FM_CB *bta_fm_cb_ptr;
#define bta_fm_cb (*bta_fm_cb_ptr)
#endif

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

extern BOOLEAN  bta_fm_hdl_event(BT_HDR *p_msg);
extern void     bta_fm_sm_execute(tBTA_FM_CB *p_cb, UINT16 event,tBTA_FM_DATA *p_data);

/*****************************************************************************
**  action functions
*****************************************************************************/
extern void bta_fm_api_enable(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_enable(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_init_delay(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_disable (tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_tune_freq(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_scan_freq(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_abort(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_read_rds(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_vsc_cmpl(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_disable_cmpl(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_set_rds_mode(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_set_aud_mode(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_set_audio_path(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_update_aud_data(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_flag_tout(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_live_update(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_int_cmpl(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_set_deemphasis(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_q_evt(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_next_inq(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_init_rds_read(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_cur_freq(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_jump_cmpl(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_find_af(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_check_rds (tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_mute_aud(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_do_nfe(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_set_region(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);

/* AF related function */
extern void bta_fm_af_filter_rssi(tBTA_FM_AF_CB *p_cb, UINT8 rssi);
extern void bta_fm_cal_rds_ber( tBTA_FM_AF_CB *p_cb, UINT8* data, UINT16 length);
extern void bta_fm_af_jump( tBTA_FM_CB *p_cb,  tBTA_FM_DATA *p_data);
extern void bta_fm_set_step(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);
extern void bta_fm_af_abort(tBTA_FM_CB *p_cb, tBTA_FM_DATA *p_data);

/* NFE related functions */
extern UINT8 bta_fm_start_nfe(UINT8* sample_lst, INT32 num_chnl);

/*******************************************************************************/
/** Function         Utility functions for vcmd queue                          */
/*******************************************************************************/
extern void bta_fm_vcmd_enq(UINT8 i2c_addr, UINT8* p_data, UINT8 chk_type);
extern UINT8 bta_fm_vcmd_deq(UINT8 *i2c_addr);
extern BOOLEAN bta_fm_vcmd_qhead(UINT8 *p_i2c_addr, UINT8 *p_data, UINT8 *vcmd_type,UINT8 *length);
extern BOOLEAN bta_fm_vcmd_empty(void);
extern void bta_fm_post_msg(UINT16 event, tBTA_FM_DATA* p_data);

extern void bta_fm_vnd_cmd_write_1b(UINT8 reg_addr, UINT8 data, UINT8 param);
extern void bta_fm_vnd_cmd_write_2b(UINT8 reg_addr, UINT16 pdata, UINT8 param);
extern void bta_fm_vnd_cmd_read(UINT8 reg_addr, UINT8 param, UINT8 length);
extern void bta_fm_snd_vendor_cmd(UINT8 reg_addr, UINT8* data, UINT8 read_write, UINT8 length);
extern void bta_fm_update_maint(UINT16 maint_flag);
extern void bta_fm_set_mask(UINT16 mask, UINT8 cmd_flag,  UINT16 event, INT32 timer_val);
extern UINT8 bta_fm_cal_thresh(UINT8 threshold, UINT8 nfl, tBTA_FM_AUDIO_MODE aud_mode);


#if BTA_FM_DEBUG == TRUE
char *bta_fm_dbg_reg_code(UINT8 i2c);
#endif
#endif /* BTA_FM_INT_H */
