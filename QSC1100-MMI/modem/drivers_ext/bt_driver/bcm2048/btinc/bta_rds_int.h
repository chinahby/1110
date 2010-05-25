/*****************************************************************************
**
**  Name:           bta_rds_int.h
**
**  Description:    This is the private file for the RDS decoder.
**
**  Copyright (c) 2006, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_RDS_INT_H
#define BTA_RDS_INT_H

#include "bta_rds_api.h"
    
/* block number */
#define BTA_RDS_BLCKA           0x00    /* Block A */
#define BTA_RDS_BLCKB           0x10    /* Block B */
#define BTA_RDS_BLCKC           0x20    /* Block C */
#define BTA_RDS_BLCKD           0x30    /* Block D */
#define BTA_RDS_BLCKC_C         0x40    /* Block C hyphen */
#define BTA_RDS_BLCKE_B         0x50    /* Block E in RBDS */
#define BTA_RDS_BLCKE           0x60    /* Block E */

#define BTA_RDS_AF_FILL         205    /* AF fill in code */
#define BTA_RDS_AF_INVAL_L      205    /* AF invalid code low marker */
#define BTA_RDS_AF_INVAL_M      223    /* AF invalid code middle marker */
#define BTA_RDS_AF_NONE         224    /* 0 AF follow */
#define BTA_RDS_AF_NUM_1        225    /* 1 AF follow */    
#define BTA_RDS_AF_NUM_25       249    /* 25 AFs follow */    
#define BTA_RDS_LF_MF           250    /* LF/MF follow */
#define BTA_RDS_AF_INVAL_H      251    /* AF invalid code high marker */
#define BTA_RDS_AF_INVAL_T      255    /* AF invalid code top marker */
#define BTA_RDS_MF_LOW          0x10   /* lowest MF frequency */

#define BTA_RDS_FM_BASE         875     /* FM base frequency */
#define BTA_RDS_MF_BASE         531     /* MF base frequency */
#define BTA_RDS_LF_BASE         153     /* LF base frequency */

#define BTA_RDS_MIN_DAY         1      /* minimum day */
#define BTA_RDS_MAX_DAY         31     /* maximum day */
#define BTA_RDS_MIN_HUR         0      /* minimum hour */
#define BTA_RDS_MAX_HUR         23     /* maximum hour */
#define BTA_RDS_MIN_MUT         0      /* minimum minute */
#define BTA_RDS_MAX_MUT         59     /* maximum minute */

#define BTA_RDS_LEFT_LEN         24    /* left over rds data length max in control block */
#define BTA_RDS_RT_LEN           64    /* Max radio text length */
#define BTA_RDS_LEN_8            8     /* 8 character RDS feature length, i.e. PS, PTYN */

/* PIN bit mask */
#define BTA_RDS_PIN_DAY_MSK     0xf8    /* high 5 bits of byte 1 of block D */
#define BTA_RDS_PIN_HUR_MSK_H   0x07    /* low 3 bits of byte1 of block D */
#define BTA_RDS_PIN_HUR_MSK_L   0xc0    /* high 2 bits of byte 2 of block D */
#define BTA_RDS_PIN_MIN_MSK     0x3f    /* low 6 bits of byte 2 of block D */

#define BTA_RDS_B0              (0x01 << 0)
#define BTA_RDS_B1              (0x01 << 1)
#define BTA_RDS_B2              (0x01 << 2)
#define BTA_RDS_B3              (0x01 << 3)
#define BTA_RDS_B4              (0x01 << 4)
#define BTA_RDS_B5              (0x01 << 5)
#define BTA_RDS_B6              (0x01 << 6)
#define BTA_RDS_B7              (0x01 << 7)

/* group version */
#define BTA_RDS_GRP_NUM_MASK    0xF0    /* Group type mask */
#define BTA_RDS_GRP_VER_MASK    0x0F    /* Group version mask */
#define BTA_RDS_GTYPE_B_BIT     BTA_RDS_B3  /* Version B group bit */

#define BTA_RDS_GRP_A           0x0A   /* A version group */
#define BTA_RDS_GRP_B           0x0B   /* B version group */
#define BTA_RDS_INVALID_GTYPE   0x00   /* invalid group type */

#define BTA_RDS_GRP_QUA_MASK    (BTA_RDS_B2 | BTA_RDS_B3)    /* group quality mask, b2, b3 */
#define BTA_RDS_BLKNUM_MASK        0xF0    /* Block Number mask */

#define BTA_RDS_TP_MASK         0x0f        /* low 4 bits of a data byte */
#define BTA_RDS_TA_MASK         0xf0        /* high 4 bits of a data byte */

#define BTA_RDS_DI_MASK_FULL    0x0f

#define BTA_RDS_SLC_UNUSE_CODE1  0x04       /* unassigned SLC variant code type 1 */
#define BTA_RDS_SLC_UNUSE_CODE2  0x05       /* unassigned SLC variant code type 2 */

#define BTA_RDS_INVALID_NUM      0xff   /* invalid number */
#define BTA_RDS_FEATURE_AVAL     0xff   /* feature availability mask */

#define BTA_RDS_TUPLE_LENGTH     3    /* length of RDS tuple retreived from RDS FIFO */

#define BTA_RDS_GROUP_LEN     12    /* 1st byte of a RDS tuple index */
/* RDS block data quality */
enum
{
    BTA_RDS_NO_ERR,           /* 0x00 */
    BTA_RDS_2BIT_ERR,         /* 0x01 */
    BTA_RDS_3BIT_ERR,         /* 0x02 */
    BTA_RDS_UNRECOVER         /* 0x03 */
};
typedef UINT8   tBTA_RDS_QUALITY;

#ifndef BTA_RDS_ODA_MAX
#define BTA_RDS_ODA_MAX         10       /* max ODA supported */
#endif

typedef struct
{
    tBTA_RDS_AF_LIST    af_info;        /* AF list */
    UINT16              tn_freq;        /* currently tuned service */
    UINT16              num_af;
    UINT16              num_sample;     /* number of AF block samples needed */
    BOOLEAN             af_ready;       /* AF info ready */
    UINT8               m_filter;
    UINT8               first_grps[4];
    UINT8               index;          /* AF index */
    BOOLEAN             lf_mf_on;
    BOOLEAN             pre_mature;

}tBTA_RDS_AF_CB;

typedef struct
{
    UINT8               ps_on[BTA_RDS_LEN_8+1];    /* program service name */
    UINT8               ps_flag_on;                /* PS segment data flag */
    UINT16              pi_code_on;                 /* ON PI code */
    UINT16              pi_on_B;                    /* ON PI transmitted in version B group */
    tBTA_RDS_AF_CB      af_on_cb;

    tBTA_FM_RDS_TP_TYPE ta_tp;
    UINT8               ta_tp_filter;
}tBTA_RDS_EON_INFO;

typedef struct
{
    UINT16              aid;
    UINT8               grp_type;
} tBTA_RDS_ODA_REG;

/* control block for each individual RDS stream */
typedef struct
{
    tBTA_RDS_FEATURE     rds_mask;  /* RDS feature mask interested in */
    tBTA_RDS_CBACK       *p_cback;
    UINT8               app_id;
    UINT8               handle;
    tBTA_FM_RDS_B       rds_mode;   /* RDS mode */
    tBTA_RDS_STATUS     status;     /* carry the decoder status */
    tBTA_RDS_RPTOR_SET  rpoir_type;
    UINT8               filter_cnt[6];   /* RDS data filter counter */

    UINT16              pi_code;    /* PI Code */
    UINT8               pty;        /* program type code */
    
    tBTA_FM_RDS_TP_TYPE ta_tp;      /* Traffic program flag */
    
    UINT8               di;         /* DI */
    UINT8               di_flag;    /* DI segment data flag */
    
    tBTA_RDS_PIN        pin;        /* PIN */
    tBTA_RDS_CT         ct;         /* Clock Time */

    UINT8               rt[BTA_RDS_RT_LEN + 1];    /* program type name buffer, null terminated */
    UINT8               rt_toggle;  /* toggle bit for RT new text indication 8*/
    UINT32              rt_flag;   /* program type name flag, 16 segments */

    UINT8               ptyn_flag;  /* program type name flag */
    UINT8               ptyn[BTA_RDS_LEN_8 + 1];    /* program type name buffer */

    UINT8               ps[BTA_RDS_LEN_8+1];      /* program service name */
    UINT8               ps_flag;    /* PS segment data flag */
    UINT8               ps_gtype;   /* PS group type */

    tBTA_RDS_AF_CB       af_cb;

    UINT8               left_bytes[BTA_RDS_LEFT_LEN]; /* left over RDS data, 8 tuples max */
    UINT8                left_len;       /* left over data length */

    tBTA_RDS_M_S        m_s;            /* music speech content */
        
    tBTA_RDS_ODA_REG     oda_reg[BTA_RDS_ODA_MAX];  /* ODA data */
    UINT8                oda_index;

    tBTA_RDS_EON_INFO   eon;            /* enhanced other network information */

#if BTA_RDS_DEBUG 
    /* [0] no correction; 01: max 2 bits correceted; 02: >2 bits corrected; 3: unrecoverable */
    UINT32          quality_stats[4];
#endif
}tBTA_RDS_STR_CB;

/* control block for RDS decoder, supporting to BTA_RDS_MAX_STREAM different RDs streams */
typedef struct
{
   tBTA_RDS_STR_CB      stream_cb[BTA_RDS_MAX_STREAM]; 
} tBTA_RDS_CB;

#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_RDS_CB  bta_rds_cb;
#else
extern tBTA_RDS_CB *bta_rds_cb_ptr;
#define bta_rds_cb (*bta_rds_cb_ptr)
#endif

/*****************************************************************************
**  action functions
*****************************************************************************/
/* EON decoding action function */
extern void bta_rds_eon_pin(tBTA_RDS_STR_CB *p_cb, UINT8 var, UINT16 pi_eon, UINT8 *p_data);
extern void bta_rds_eon_pty_ta(tBTA_RDS_STR_CB *p_cb, UINT8 var, UINT16 pi_eon,UINT8 *p_data);
extern void bta_rds_eon_link(tBTA_RDS_STR_CB *p_cb, UINT8 var, UINT16 pi_eon, UINT8 *p_data);
extern void bta_rds_eon_mapped_freq(tBTA_RDS_STR_CB *p_cb, UINT8 var, UINT16 pi_eon, UINT8 *p_data);
extern void bta_rds_eon_af_a(tBTA_RDS_STR_CB *p_cb, UINT8 var, UINT16 pi_eon, UINT8 *p_data);
extern void bta_rds_eon_ps(tBTA_RDS_STR_CB *p_cb, UINT8 var, UINT16 pi_eon, UINT8 *p_data);

/* AF decoding action */
extern void bta_rds_decode_af_u (tBTA_RDS_AF_CB *p_cb, UINT8 *p_data);
extern void bta_rds_decode_af_a (tBTA_RDS_AF_CB *p_cb, UINT8 *p_data);
extern void bta_rds_decode_af_b (tBTA_RDS_AF_CB *p_cb, UINT8 *p_data);

#endif /* BTA_RDS_INT_H */
