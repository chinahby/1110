/*****************************************************************************
**
**  Name:           bta_fm_af.h
**
**  Description:    This is the private file for the FM AF algorithm.
**
**  Copyright (c) 2006-2007, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_FM_AF_H
#define BTA_FM_AF_H

#include "gki.h"
#include "bta_api.h"
#include "bta_fm_api.h"

/* RSSI filtering const N which is calculated by bit shifting, 4 is N + 1 = 2^4 = 16 */
#ifndef BTA_FM_AF_FACTOR_RSSI
#define BTA_FM_AF_FACTOR_RSSI   4
#endif

/* BLER filtering const N which is calculated by bit shifting, 6 is N + 1 = 2^6 = 64 */
#ifndef BTA_FM_AF_FACTOR_BER
#define BTA_FM_AF_FACTOR_BER    6
#endif

/* BLER can be calculated based on a sasmple pool size, currently defined as 1 */ 
#define BTA_FM_RDS_SAMPLE_POOL_SIZE     1

/*  Naf_jump is the number of seconds of a time constant that for how long 
    bad RSSI/BER can be tolerant */
#ifndef BTA_FM_AF_JUMP_TOLERANCE    
#define BTA_FM_AF_JUMP_TOLERANCE    10
#endif

/* AF BLER below threshold tolerance time, by default 10 seconds */
#ifndef BTA_FM_AF_JUMP_TOLERANCE_TIMER    
#define BTA_FM_AF_JUMP_TOLERANCE_TIMER    10000
#endif

/* this value stands for the error rate when block error occurs, it is BER amplify 
   for 1000 times, which is approximate 2^10, and scaled by 2^13, so that the BLER 
   calculation can run at Q23 resolution  */
#ifndef BTA_FM_RDS_BER_ERR
#define BTA_FM_RDS_BER_ERR    (1000 << 13)
#endif

/* this value stands for the threshold block error rate, which is 0.6 scaled into Q23 */
#ifndef BTA_FM_RDS_BER_THRESH
#define BTA_FM_RDS_BER_THRESH  (600 << 13)
#endif

#ifndef BTA_FM_RDS_TUPLE_LENGTH
#define BTA_FM_RDS_TUPLE_LENGTH     3
#endif

#define BTA_FM_RDS_GRP_QUA_MASK         0x0C

#define BTA_FM_FILTER_ALGO(x, y, z)  (((x << z) - x + y) >> z)

/* same definition as I2C_FM_REG_AF_FAILURE reason code detail */
#define BTA_AF_FAIL_RSSI_LOW    (1 << 4)    /* RSSI low,  bit 4*/
#define BTA_AF_FAIL_FREQ_OSHI   (1 << 5)    /* Frequency Offset high, bit 5 */
#define BTA_AF_FAIL_PI_ERR      (3 << 4)    /* PI mismatch, bit 4 & 5 */

/* AF jump failure reason */
#define BTA_AF_ERR(x)               (x >> 4)
#define BTA_FM_AF_OK                0
#define BTA_FM_AF_ERR_UNKNOWN       0xff                    
#define BTA_FM_AF_ERR_RSSI          BTA_AF_ERR (BTA_AF_FAIL_RSSI_LOW)       /* 0x01 */
#define BTA_FM_AF_ERR_FREQ_OSHI     BTA_AF_ERR (BTA_AF_FAIL_FREQ_OSHI)      /* 0x02 */
#define BTA_FM_AF_ERR_PI            BTA_AF_ERR (BTA_AF_FAIL_PI_ERR)         /* 0x03 */
#define BTA_FM_AF_ERR_ABORT         0x10            
typedef UINT8   tBTA_FM_AF_STATUS;

/* AF algorithm control block */
typedef struct 
{
    tBTA_FM_AF_STATUS       jump_status;
    UINT16                  pi_code;
    tBTA_FM_AF_LIST         af_list;        /* All available AF list */
    UINT8                   cur_jump_idx;   /* currently jumpping attemp index in af_list */
    UINT8                   first_pi_idx;   /* first PI mismatch channel */
    UINT8                   last_rssi_idx;  /* last RSSI low channel */
    UINT8                   jump_cmpl;      /* jump complete & cmpl pending bit as bit7 */
    UINT32                  rssi_thresh;
    UINT32                  filter_rssi;    /* filtered RSSI */
    UINT8                   rssi_low_timer; /* RSSI below threshold timer */

    UINT32                  filter_ber;     /* filtered BER */
    TIMER_LIST_ENT          ber_low_timer;
}tBTA_FM_AF_CB;

/* channel monitoring algorithm */
extern void bta_fm_rank_af_list(tBTA_FM_AF_CB *p_cb);
extern BOOLEAN bta_fm_af_jump_decision( tBTA_FM_AF_CB *p_cb, UINT8 rssi);
extern BOOLEAN bta_fm_af_ber_jump( tBTA_FM_AF_CB *p_cb, UINT8* p_data, UINT16 length);
extern BOOLEAN bta_fm_filter_rssi (tBTA_FM_AF_CB *p_cb, UINT8 rssi);

#endif

