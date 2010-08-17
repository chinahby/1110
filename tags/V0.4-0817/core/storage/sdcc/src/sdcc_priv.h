#ifndef __SDCC_PRIV_H
#define __SDCC_PRIV_H

/**********************************************************************
 * sdcc_priv.h
 *
 * This file provides SDCC internal definitions.
 *
 * Copyright (c) 2006-2008 Qualcomm Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 **********************************************************************/
/*======================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/storage/sdcc/rel/02/src/sdcc_priv.h#5 $
  $DateTime: 2008/12/08 19:01:45 $ $Author: vijayj $

when         who     what, where, why
----------   ---     -----------------------------------------------
2008-12-09   vj      Added signal definitions to use FS_OP_COMPLETE_SIG
2008-10-31   vin     Support for SDCC power collapse
2008-06-19   vin     Removed SDCC Mux features
2008-06-11   sc      Ported card detection changes
03/05/08     sc      Removed SDCC_PARAM_NOT_REF macro
01/21/08     sc      Added Mc-EX commands in enum SD_CMD
10/15/07     umr     Move SDCC_CARD_TYPE to sdcc_api.h
08/02/07     vin     Added feature FEATURE_SDCC_WLAN_CONFIG_API
07/25/07     dng     Added the SDCC_RETRY_CNT parameter.
04/11/07     sc      Ported in MMCPlus support
03/02/07     dng     Changed the L4 sdcc_mdelay and sdio_mdelay.
02/15/07     sc      Fixed the PNM field
11/07/06     dng     Added prog_done isr support.
10/20/06     dng     Added the sdcc_sdio_init function prototype.
09/19/06     dng     Added the comments for all the new feature flags and 
                     removed all the define FEATURES.
09/07/06     dng     Added SDCC/SDIO Mux feature.
09/06/06     dng     Changed the sdcc_process_dma to sdcc_process_interrupts
07/17/06     sc      Added SD2.0 feature
07/17/06     dng     First pass of code review clean up.
06/15/06     dng     Added the MCI_CLK_INVERT_IN define for M6550 target.
05/25/06     dng     Changed the SDCC_STATUS_POLL_MAX value for M6280..
05/10/06     dng     Added SDIO WLAN autodetection support.
05/09/06     dng     Added sdcc_write retry support.
05/04/06     dng     Added byte mode support for WLAN SDIO.
04/13/06     dng     Changed the SDCC_DETECT_MAX_RETRIES to 10 from 50.
04/11/06     dng     Changed the clk_busy_wait to rex_sleep for the
                     sdcc_mdelay MACRO.
03/01/06     dng     Added Atheros support.
11/03/05     hwu     Changed to return status code for internal functions.
08/28/05     hwu     General cleanup.
07/04/05     hwu     Changed to use [27:24] for signals.
06/07/05     hwu     Added a timer signal. 
05/26/05     hwu     Merged changes from L4Linux.
05/09/05     hwu     Added interrupt bits and interrupt signal.
04/14/05     hwu     Changed to use sdcc_setup_data_xfer.
                     Moved block_mode and enable_dma to sdcc_data_type.
02/15/05     hwu     Added more SDIO releated internal defines.
10/29/04     hwu     new release for MSM6550 v1.2 chipset.
07/14/04     hwu     Changed so the debugging can be stripped off.
06/27/04     hwu     Added SDCC_DMA_DONE_MAX.
06/10/04     hwu     Initial Version.
==================================================================*/

/**********************************************************/
/*                    INCLUDES                            */
/**********************************************************/
#define DPRINTF(x)
#define TPRINTF(level, x)
#include "sdcc_api.h"
#include "sdcc_debug.h"
#include "rex.h"
#include "task.h"

#if defined(SDCC_UNIT_TEST)
#error code not present
#endif

/*************************************************************/
/*                     DEFINES                               */
/*************************************************************/
#ifndef T_QSC1100
#define SDCC_RETRY_CNT              1
#else
#define SDCC_RETRY_CNT              3
#endif

#define SDCC_CMD8_RETRIES           3

/* Per section 6.4.1 in SD2.0 System Specification, */
/* time out value for initialization process is 1 second. */
#define SDCC_SD_DETECT_MAX_BUSY_MS           1000

/* time interval (in ms) between the ACMD41 retry */
/* Refer to section 3.6 of SD Specifications Part A2 SD Host Controller */
/* Standard Specifrication Version 2.00 */
#define SDCC_SD_DETECT_BUSY_RETRY_INTERVAL   50

/* SDCC_SD_DETECT_MAX_RETRIES is derived from the the maximum time out value */
/* of the initialization process.  The time after the end of the first */
/* ACMD41 to card ready is 1 second. */
/* That is 1000 milli-second = 20 x 50 milli-seond where 50 milli-second is */
/* wait time between each ACMD41 retry. */
/* Refer to section 6.4.1 in SD2.0 System Specification */
#define SDCC_SD_DETECT_MAX_RETRIES \
   (SDCC_SD_DETECT_MAX_BUSY_MS / SDCC_SD_DETECT_BUSY_RETRY_INTERVAL)

#define SDCC_SDIO_INIT_RETRIES      3
#define SDCC_DETECT_MAX_RETRIES     10
#define SDCC_DEFAULT_BLK_LENGTH_SIZE  512 /* in Bytes */
#if defined (T_MSM6280)
#error code not present
#else
#define SDCC_STATUS_POLL_MAX        0x2000
#endif
#define SDCC_PROG_DONE_MAX          0x8000
#define SDCC_DMA_DONE_MAX           0x8000
#define SDCC_MAX_NO_BLOCKS          127

#define SDCC_CARD_ERASE_FAIL        0xFDFF8000   /* erase failure mask*/
#define SDCC_NEGOTIATE_OCR          0x00FF8000   /* voltage 2.7-3.6 */

#define SDCC_CARD_STATUS_BMSK       0x0000000F
#define SDCC_CARD_STATUS_SHFT       9

#define SDCC_CARD_STATUS_BLKLEN_ERR 0x20000000

#define SDCC_DATA_READ              0x0UL
#define SDCC_DATA_WRITE             0x1UL

/* transfer flags for adtc command */
#define SDCC_APP_COMMAND_FLAG      0x0001
#define SDCC_READ_TRANSFER_FLAG    0x0002
#define SDCC_WRITE_TRANSFER_FLAG   0x0004   /* do not set the read transfer */
                                            /* bit for writes */

/*************************************************************/
/*           HOST HW SPECIFIC DEFINES                        */
/*************************************************************/
#define MCI_BLOCK_LENGTH            0x0200
#define MCI_ARGU_NULL               0x0000

#define MCI_FIFO_SIZE               16     /* FIFO SIZE = 16 * 32bit */

#define MCI_READ_TIMEOUT            500    /* in msec */
#define MCI_WRITE_TIMEOUT           800    /* in msec */

/* Field enumeration for MCI_POWER */
#define MCI_POWER_CTRL_OFF          0x0
#define MCI_POWER_CTRL_UP           0x2
#define MCI_POWER_CTRL_ON           0x3

/* Field enumeration for MCI_CLK */
#define MCI_CLK_ENABLED             (1UL << HWIO_MCI_CLK_ENABLE_SHFT)
#define MCI_CLK_SET_PWRSAVE         (1UL << HWIO_MCI_CLK_PWRSAVE_SHFT)
#define MCI_CLK_SET_WIDE            (1UL << HWIO_MCI_CLK_WIDEBUS_SHFT)
#define MCI_CLK_SET_FLOW            (1UL << HWIO_MCI_CLK_FLOW_ENA_SHFT)
#define MCI_CLK_INVERT_IN                   HWIO_MCI_CLK_INVERT_IN_BMSK

/* Field enumeration for MCI_DATA_CTL */
#define MCI_DATA_DPSM_ENABLED       (1UL << HWIO_MCI_DATA_CTL_ENABLE_SHFT)
#define MCI_DATA_DPSM_DISABLED      (0UL << HWIO_MCI_DATA_CTL_ENABLE_SHFT)
#define MCI_DATA_READ               (1UL << HWIO_MCI_DATA_CTL_DIRECTION_SHFT)
#define MCI_DATA_WRITE              (0UL << HWIO_MCI_DATA_CTL_DIRECTION_SHFT)
#define MCI_DATA_DMA_ENABLED        (1UL << HWIO_MCI_DATA_CTL_DMA_ENABLE_SHFT)
#define MCI_DATA_DMA_DISABLED       (0UL << HWIO_MCI_DATA_CTL_DMA_ENABLE_SHFT)
#define MCI_DATA_ADTC               (1UL << HWIO_MCI_DATA_CTL_MODE_SHFT)
#define MCI_DATA_BLOCKSIZE          (6UL << HWIO_MCI_DATA_CTL_BLOCKSIZE_SHFT)

/* Field enumeration for MCI_CMD  */
#define MCI_CMD_RESPONSE            (1UL << HWIO_MCI_CMD_RESPONSE_SHFT)
#define MCI_CMD_LONG_RESP           (1UL << HWIO_MCI_CMD_LONGRSP_SHFT)
#define MCI_CMD_ENABLED             (1UL << HWIO_MCI_CMD_ENABLE_SHFT)
#define MCI_CMD_PROG_ENABLED        (1UL << HWIO_MCI_CMD_PROG_ENA_SHFT)

/* Field enumeration for MCI_INT_MASK0  */
#define MCI_INT_MASK_PROG_DONE   HWIO_MCI_STATUS_PROG_DONE_BMSK /* 0x800000  */
#define MCI_INT_MASK_SDIO_INTR   HWIO_MCI_STATUS_SDIO_INTR_BMSK /* 0x400000  */
#define MCI_INT_MASK_DMA_DONE    HWIO_MCI_STATUS_DMA_DONE_BMSK  /* 0x1000000 */

#define SDCC_CMD_INTERRUPTS   ( HWIO_FMSK( MCI_STATUS, CMD_RESPONSE_END)  | \
                                HWIO_FMSK( MCI_STATUS, CMD_SENT)          | \
                                HWIO_FMSK( MCI_STATUS, CMD_TIMEOUT)       | \
                                HWIO_FMSK( MCI_STATUS, CMD_CRC_FAIL) ) 

/*************************************************************/
/*             SDHC SPECIFIC DEFINES                         */
/*************************************************************/
/* Related to CMD6_SWITCH_FUNC */
#define SDCC_QUERY_HS               0x00FFFF01 /* Query switchable for High-Speed */
#define SDCC_SWITCH_HS              0x80FFFF01 /* Switch to High-Speed */

/* Related to CMD8_SEND_IF_COND */
#define SDCC_HC_VOLT_SUPPLIED       0x000001AA /* Voltage Host Supplied */
#define SDCC_HC_HCS                 0x40000000 /* Host Capacity Support */
#define SDCC_HC_CCS                 0x40000000 /* Card Capacity Status */

/* CSD Version 2.0 for High Capacity SD memory card */
#define SDCC_CSD_TAAC        0x0E /* data read access-time is 1ms */
#define SDCC_CSD_NSAC        0x00 /* data read access-time in CLK cycles */
#define SDCC_CSD_READ_BL_LEN 0x9 /* max. read data block length is 512 bytes */

/*************************************************************/
/*             SDIO SPECIFIC DEFINES                         */
/*************************************************************/
/* SDIO in general */
#define SDCC_SDIO_FN0               0x0
#define SDCC_SDIO_FN1               0x1

#define SDCC_SDIO_DEFAULT_BLKSZ     0x200

#define SDCC_SDIO_CCCR_MAX_BYTES    0x12
#define SDCC_SDIO_FBR_MAX_BYTES     0x12
#define SDCC_SDIO_MAX_FNS           0x7
#define SDCC_SDIO_DEV_FOUND         0x01
#define SDCC_SDIO_DEV_ENABLED       0x02
#define SDCC_SDIO_IENM              0x1

#define SDCC_SDIO_ENABLE_FN         0x01
#define SDCC_SDIO_DISABLE_FN        0x00

#define SDCC_IO_NUM_FNS             0x70000000   /* number of I/O functions */
#define SDCC_IO_MP                  0x08000000   /* memory present */
#define SDCC_DEVICE_RDY             0x80000000   /* card is ready */
#define SDCC_SDIO_RDY_MAX_RETRIES   0x1000

/* SDIO register offsets in the Common Register Area */
#define SDCC_SDIO_REG_CCCR               0x00
#define SDCC_SDIO_REG_SPEC_REV           0x01
#define SDCC_SDIO_REG_ENABLE             0x02
#define SDCC_SDIO_REG_IO_READY           0x03
#define SDCC_SDIO_REG_INT_ENABLE         0x04
#define SDCC_SDIO_REG_INT_PENDING        0x05
#define SDCC_SDIO_REG_IO_ABORT           0x06
#define SDCC_SDIO_REG_BUS_CONTROL        0x07
#define SDCC_SDIO_REG_CARD_CAPABILITY    0x08
#define SDCC_SDIO_REG_COMMON_CIS_POINTER 0x09 /* extends to 0xA, 0xB */ 
#define SDCC_SDIO_REG_BUS_SUSPEND        0x0C
#define SDCC_SDIO_REG_FUNCTION_SELECT    0x0D
#define SDCC_SDIO_REG_EXEC_FLAGS         0x0E
#define SDCC_SDIO_REG_READY_FLAGS        0x0F
#define SDCC_SDIO_REG_FB0_BLKSZ_L        0x10 
#define SDCC_SDIO_REG_FB0_BLKSZ_H        0x11 

/* FBR Definitions */
#define SDCC_SDIO_FBR1_OFFSET            0x100
#define SDCC_SDIO_FBR_DEVICE_CODE        0x0    
#define SDCC_SDIO_FBR_DEVICE_CODE_EXT    0x1    
#define SDCC_SDIO_FBR_CISP_BYTE_0        0x9    
#define SDCC_SDIO_FBR_CISP_BYTE_1        0xA    
#define SDCC_SDIO_FBR_CISP_BYTE_2        0xB    
#define SDCC_SDIO_FBR_CSAP_BYTE_0        0xC    
#define SDCC_SDIO_FBR_CSAP_BYTE_1        0xD    
#define SDCC_SDIO_FBR_CSAP_BYTE_2        0xE    
#define SDCC_SDIO_FBR_DATA_ACCESS        0xF    
#define SDCC_SDIO_FBR_IO_BLOCK_SIZE      0x10   
#define SDCC_SDIO_FBR_LENGTH             0x100  

/* Card Capability Bit definitions */
#define SDCC_SDIO_CC_4BLS               (1 << 7)
#define SDCC_SDIO_CC_LSC                (1 << 6)
#define SDCC_SDIO_CC_E4MI               (1 << 5)
#define SDCC_SDIO_CC_S4MI               (1 << 4)
#define SDCC_SDIO_CC_SBS                (1 << 3)
#define SDCC_SDIO_CC_SRW                (1 << 2)
#define SDCC_SDIO_CC_SMB                (1 << 1)
#define SDCC_SDIO_CC_SDC                (1 << 0)

/* Bus Interface Control bit definitions */
#define SDCC_SDIO_CD_DISABLE            (1 << 7)
#define SDCC_SDIO_BUS_WIDTH_1BIT        0x00
#define SDCC_SDIO_BUS_WIDTH_4BIT        0x02

/* SDIO TUPLE support */
#define SDCC_SDIO_CISTPL_MAX_SZ     0x60
#define SDCC_SDIO_CISTPL_MANFID     0x20
#define SDCC_SDIO_CISTPL_MANFID_SZ  0x02


/* SDIO CMD52/CMD53 */
#define SDCC_SDIO_RAW               0x1UL
#define SDCC_SDIO_RAW_N             0x0UL
#define SDCC_SDIO_BLK_MODE          0x1UL
#define SDCC_SDIO_BYTE_MODE         0x0UL
#define SDCC_SDIO_OP_FIXED          0x0UL
#define SDCC_SDIO_OP_INC            0x1UL

#define SDCC_MEM_BLK_MODE           0x1
#define SDCC_MEM_BYTE_MODE          0x0

/*************************************************************/
/*             MMCPLUS SPECIFIC DEFINES                      */
/*************************************************************/
/* Related to SD_CMD6_SWITCH */
#define SDCC_MMCPLUS_CHANGE_BUSWIDTH   0x03B70000
#define SDCC_MMCPLUS_ENABLE_HS_TIMING  0x03B90000
#define SDCC_MMCPLUS_CHANGE_POWER_CL   0x03BB0000

/*************************************************************/
/*             ENUMERATIONS                                  */
/*************************************************************/
/* Standard commands supported by the Multimedia Card */
typedef enum sd_cmd_t
{
    SD_CMD0_GO_IDLE_STATE = 0,
    SD_CMD1_SEND_OP_COND,
    SD_CMD2_ALL_SEND_CID,
    SD_CMD3_SET_RELATIVE_ADDR,
    SD_CMD4_SET_DSR,
    SD_CMD5_IO_SEND_OP_COND,
    SD_CMD6_SWITCH_FUNC,                         /* SD CMD6 adtc cmd type */
    SD_CMD6_SWITCH = SD_CMD6_SWITCH_FUNC,        /* MMC CMD6 ac cmd type */
    SD_CMD7_SELECT_CARD,
    SD_CMD8_SEND_IF_COND,                        /* SD CMD8 bcr cmd type */
    SD_CMD8_SEND_EXT_CSD = SD_CMD8_SEND_IF_COND, /* MMC CMD8 adtc cmd type */
    SD_CMD9_SEND_CSD,
    SD_CMD10_SEND_CID,
    SD_CMD11_READ_DAT_UNTIL_STOP,
    SD_CMD12_STOP_TRANSMISSION,
    SD_CMD13_SEND_STATUS,
    SD_CMD14,
    SD_CMD15_GO_INACTIVE_STATE,
    SD_CMD16_SET_BLOCKLEN,
    SD_CMD17_READ_BLOCK,
    SD_CMD18_READ_MULTIPLE_BLOCK,
    SD_CMD19,
    SD_CMD20_WRITE_DAT_UNTIL_STOP,
    SD_CMD21,
    SD_CMD22,
    SD_CMD23,
    SD_CMD24_WRITE_BLOCK,
    SD_CMD25_WRITE_MULTIPLE_BLOCK,
    SD_CMD26,
    SD_CMD27_PROGRAM_CSD,
    SD_CMD28_SET_WRITE_PROT,
    SD_CMD29_CLR_WRITE_PROT,
    SD_CMD30_SEND_WRITE_PROT,
    SD_CMD31,
    SD_CMD32_TAG_SECTOR_START,
    SD_CMD33_TAG_SECTOR_END,
    SD_CMD34_READ_SEC_CMD = 34,     /* SD Mc-EX specific READ_SEC_CMD */
    SD_CMD34_UNTAG_SECTOR = 34,     /* MMC command index */
    SD_CMD35_WRITE_SEC_CMD = 35,         /* SD Mc-EX specific WRITE_SEC_CMD */
    SD_CMD35_TAG_ERASE_GROUP_START = 35, /* MMC command index with ac cmd type */
    SD_CMD36_SEND_PSI = 36,            /* SD Mc-EX specific SEND_PSI */
    SD_CMD36_TAG_ERASE_GROUP_END = 36, /* MMC command index with ac cmd type */
    SD_CMD37_CONTROL_TRM = 37,       /* SD Mc-EX specific CONTROL_TRM */
    SD_CMD37_UNTAG_ERASE_GROUP = 37, /* MMC command index */
    SD_CMD38_ERASE,
    SD_CMD39_FAST_IO,
    SD_CMD40_GO_IRQ_STATE,
    SD_CMD41,
    SD_CMD42_LOCK_UNLOCK,
    SD_CMD50_DIRECT_SECURE_READ = 50,  /* SD Mc-EX specific DIRECT_SECURE_READ */
    SD_CMD52_IO_RW_DIRECT=52,
    SD_CMD53_IO_RW_EXTENDED,
    SD_CMD55_APP_CMD=55,
    SD_CMD56_GEN_CMD,
    SD_CMD57_DIRECT_SECURE_WRITE = 57 /* SD Mc-EX specific DIRECT_SECURE_WRITE */
} SD_CMD;

/* Application-specific commands supported by all SD cards */
typedef enum sd_acmd_t
{
    SD_ACMD6_SET_BUS_WIDTH=6,  
    SD_ACMD13_SD_STATUS=13,  
    SD_ACMD18_SECURE_READ_MULTI_BLOCK=18,
    SD_ACMD22_SEND_NUM_WR_BLOCKS=22,
    SD_ACMD23_SET_WR_BLK_ERASE_COUNT=23,
    SD_ACMD25_SECURE_WRITE_MULTI_BLOCK=25,
    SD_ACMD26_SECURE_WRITE_MKB=26,
    SD_ACMD38_SECURE_ERASE=38,
    SD_ACMD41_SD_APP_OP_COND=41,
    SD_ACMD42_SET_CLR_CARD_DETECT=42,
    SD_ACMD43_GET_MKB=43,
    SD_ACMD44_GET_MID=44,
    SD_ACMD45_SET_CER_RN1=45,
    SD_ACMD46_GET_CER_RN2=46,
    SD_ACMD47_SET_CER_RES2=47,
    SD_ACMD48_GET_CER_RES1=48,
    SD_ACMD49_CHANGE_SECURE_AREA=49,
    SD_ACMD51_SEND_SCR=51
} SD_ACMD;

/* On-card states */
typedef enum sdcc_host_state_t
{
    SDCC_HOST_IDLE = 0,
    SDCC_HOST_IDENT,
    SDCC_HOST_TRAN
}SDCC_HOST_STATE;

/* On-card states */
typedef enum sdcc_card_state_t
{
    SDCC_CARD_IDLE = 0,
    SDCC_CARD_READY,
    SDCC_CARD_IDENT,
    SDCC_CARD_STBY,
    SDCC_CARD_TRAN,
    SDCC_CARD_DATA,
    SDCC_CARD_RCV,
    SDCC_CARD_PRG,
    SDCC_CARD_DIS,
    SDCC_CARD_IGNORE         /*dummy entry*/
}SDCC_CARD_STATUS;

/* SDCC response types */
typedef enum sdcc_card_resp_t
{
    SDCC_RESP_NONE   = 0x00,
    SDCC_RESP_SHORT  = 0x01,
    SDCC_RESP_LONG   = 0x04
}SDCC_CARD_RESP;

/* SDCC mode types */
typedef enum sdcc_card_mode_t
{
    SDCC_INIT_MODE            = 0x01,
    SDCC_IDENTIFICATION_MODE  = 0x02,
    SDCC_DATA_TRANSFER_MODE   = 0x04
}SDCC_CARD_MODE;


#define SDIO_PHILLIP_MANFID  0x2C0
#define SDIO_ATHEROS_MANFID  0x271


/******************************************************************/
/*                            TYPES                               */
/******************************************************************/
typedef struct sdcc_cid_t {
    uint8    mid;
    uint16   oid;
    uint8    pnm[7];   
    uint8    prv;
    uint32   psn;
    uint8    mdt;
    uint8    dont_care;
} sdcc_cid_type;

typedef struct sdcc_csd_t {
    uint8  csd_structure;
    uint8  spec_vers;
    uint8  taac;
    uint8  nsac;
    uint8  tran_speed;
    uint16 ccc;
    uint8  read_bl_len;
    uint8  read_bl_partial;
    uint8  write_blk_misalign;
    uint8  read_blk_misalign;
    uint8  dsr_imp;
    uint32 c_size;
    uint8  vdd_r_curr_min;
    uint8  vdd_r_curr_max;
    uint8  vdd_w_curr_min;
    uint8  vdd_w_curr_max;
    uint8  c_size_mult;
    union {
        struct { /* MMC system specification version 3.1 */
            uint8  erase_grp_size;  
            uint8  erase_grp_mult; 
        } v31;
        struct { /* MMC system specification version 2.2 */
            uint8  sector_size;
            uint8  erase_grp_size;
        } v22;
    } erase;
    uint8  wp_grp_size;
    uint8  wp_grp_enable;
    uint8  default_ecc;
    uint8  r2w_factor;
    uint8  write_bl_len;
    uint8  write_bl_partial;
    uint8  file_format_grp;
    uint8  copy;
    uint8  perm_write_protect;
    uint8  tmp_write_protect;
    uint8  file_format;
    uint8  crc;
} sdcc_csd_type;

typedef struct sdcc_cmd_t {
    uint32              cmd;
    uint32              cmd_arg;
    SDCC_CARD_RESP      resp_type;
    uint32              resp[4];
    uint32              prog_scan;
    uint32              status;
} sdcc_cmd_type;

typedef int (* SDCC_SDIO_ISR_FUNC)(void *param);

typedef struct {
    /* CCCR  */
    uint8               cc;           /* card capability */
    uint32              cccr_cis;     /* CCCR cis pointer  */
    uint16              cccr_blksz;   /* CCCR block size in bytes */

    /* FBR   */
    uint8               devfn;        /* device function number */
    uint16              fn_code;      /* function interface code */
    uint32              fn_cis;       /* function cis pointer  */
    uint32              fn_csa;       /* function csa pointer */
    uint16              fn_blksz;     /* function block size in bytes */

    
    uint32              flag;
    uint32              manfid;       /* SDIO manufacture code */
  	uint32              op_code;      /* specify CMD53 op code    */
    uint32              block_mode;   /* specify CMD53 block mode */
    SDCC_SDIO_ISR_FUNC  isr;
    void               *isr_param;

} sdcc_io_type;

typedef struct {
    uint32              psn;
    uint8               pnm[7];        /* product name with termination */
    uint32              block_len;     /* In bytes  */
    uint32              card_size;     /* In blocks */
    uint32              erase_block;
    boolean             multi_block;

    uint8               spec_vers;  /* SD/MMC System Spec version */
    uint8               clk_freq;   /* freq card supported */
                                    /* 0: regular speed, 1: high speed */
} sdcc_mem_type;

typedef struct {
    SDCC_CARD_TYPE      card_type;
    SDCC_HOST_STATE     host_state;
    uint16              rca[2];
    SDCC_STATUS         errno;
#ifndef T_QSC1100
    boolean             enable_dma;
#else
    boolean             wide_bus;
#endif
    uint32              block_mode;
#ifndef T_QSC1100
    rex_tcb_type       *sdcc_tcb;
#endif
    uint32              status;
#ifndef T_QSC1100
    rex_timer_type      sdcc_dma_timer;
#endif
    sdcc_mem_type       mem;
    sdcc_io_type        io;
    uint16              curr_sd_drv;
#ifdef FEATURE_SDCC_WLAN_CONFIG_API
    boolean             is_wlan;
    boolean             is_wlan_param_set;
    sdcc_wlan_param_t   sdcc_wlan_params;
#endif
    /*Set if card insertion or removal is detected */
    boolean             slot_state_changed;
} sdcc_data_type;

/*********************************************************************/
/*                           MACROS                                  */
/*********************************************************************/
#define IS_ALIGNED(buff)   (!(((uint32)buff) & 0x03))

#define SDCC_RW_COMPLETE_SIG           FS_OP_COMPLETE_SIG

#define SDCC_DMA_DONE_SIG_VAL          0x00000001
#define SDCC_PROG_DONE_SIG_VAL         0x00000002
#define SDCC_TIMER_TIMEOUT_SIG_VAL     0x00000004

/* the static (clearable) bits */
#define MCI_STATUS_STATIC_MASK         0x018007FF

/* Call HWIO_OUTM so that I can shadow this write only register */
#define SDCC_CLR_STATUS(x)       HWIO_OUTM(MCI_CLEAR, x, x)

#define SDCC_DMA_STATUS_BITS ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )  | \
                               HWIO_FMSK( MCI_STATUS, TX_UNDERRUN)    | \
                               HWIO_FMSK( MCI_STATUS, RX_OVERRUN)     | \
                               HWIO_FMSK( MCI_STATUS, START_BIT_ERR)  | \
                               HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) )

#define SDCC_READ_ERROR  ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )   | \
                           HWIO_FMSK( MCI_STATUS, RX_OVERRUN)      | \
                           HWIO_FMSK( MCI_STATUS, START_BIT_ERR)   | \
                           HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) )
                             
#define SDCC_WRITE_ERROR ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )  | \
                           HWIO_FMSK( MCI_STATUS, TX_UNDERRUN)    | \
                           HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) )

#define SDCC_DMA_ERROR   ( HWIO_FMSK( MCI_STATUS, DATA_TIMEOUT )  | \
                           HWIO_FMSK( MCI_STATUS, TX_UNDERRUN)    | \
                           HWIO_FMSK( MCI_STATUS, RX_OVERRUN)     | \
                           HWIO_FMSK( MCI_STATUS, START_BIT_ERR)  | \
                           HWIO_FMSK( MCI_STATUS, DATA_CRC_FAIL) ) 

#define IS_RXDATA_AVLBL(status) (status & (HWIO_FMSK(MCI_STATUS, RXDATA_AVLBL)))
#define IS_TXDATA_AVLBL(status) (status & (HWIO_FMSK(MCI_STATUS, TXDATA_AVLBL)))
#define IS_TXFIFO_EMPTY(status) (status & (HWIO_FMSK(MCI_STATUS, TXFIFO_EMPTY)))
#define IS_TXDATA_FULL(status)  (status & (HWIO_FMSK(MCI_STATUS, TXFIFO_FULL)))
#define IS_RXACTIVE(status)  (status & (HWIO_FMSK(MCI_STATUS, RXACTIVE)))
#define IS_TXACTIVE(status)  (status & (HWIO_FMSK(MCI_STATUS, TXACTIVE)))

#define IS_DATAEND(status)     (status & (HWIO_FMSK(MCI_STATUS, DATAEND)))
#define IS_DATA_BLK_END(status) (status & (HWIO_FMSK(MCI_STATUS, DATA_BLK_END)))
#define IS_DATA_TIMEOUT(status) (status & (HWIO_FMSK(MCI_STATUS, DATA_TIMEOUT)))

#define SDCC_IS_READ_TRANSFER(xfer_flags)    \
   (((xfer_flags) & SDCC_READ_TRANSFER_FLAG) != 0)
#define SDCC_IS_WRITE_TRANSFER(xfer_flags)   \
   (((xfer_flags) & SDCC_READ_TRANSFER_FLAG) == 0)
#define SDCC_IS_INVALID_TRANSFER_FLAG(xfer_flags) \
   (SDCC_IS_READ_TRANSFER(xfer_flags) &&          \
   (((xfer_flags) & SDCC_WRITE_TRANSFER_FLAG) !=0))
#define SDCC_IS_APP_COMMAND_TRANSFER(xfer_flags)  \
   (((xfer_flags) & SDCC_APP_COMMAND_FLAG) != 0)
#define SDCC_DATA_DIRECTION(xfer_flags)      \
   (SDCC_IS_READ_TRANSFER(xfer_flags) ? SDCC_DATA_READ : SDCC_DATA_WRITE)
#define SDCC_MCI_DIRECTION(xfer_flags)       \
   (SDCC_IS_READ_TRANSFER(xfer_flags) ? MCI_DATA_READ : MCI_DATA_WRITE)

#define sdcc_udelay(us)     clk_busy_wait(us)

#define sdcc_mdelay(ms)    rex_sleep(ms) 
#define sdio_mdelay(ms)    clk_busy_wait(1000*ms)

/* sdcc_pdata is a global structure variable. */
/* It is initialized by sdcc_init() and used by others. */
extern sdcc_data_type   sdcc_pdata;
    
/*********************************************************************/
/*                           PROTOTYPES                              */
/*********************************************************************/
SDCC_STATUS       sdcc_wait_prog_done(void);
SDCC_STATUS       sdcc_command(sdcc_cmd_type *sdcc_cmd);
void              sdcc_config_dma(void);
void              sdcc_config_clk(SDCC_CARD_MODE mode,
                                  SDCC_CARD_TYPE card_type);
SDCC_STATUS       sdcc_wait_card_ready(void);
SDCC_STATUS       sdcc_config_rca(void);
SDCC_STATUS       sdcc_select_card(uint16 rca, uint32 select);
SDCC_CARD_STATUS  sdcc_send_status(void);
uint32            sdcc_setup_data_xfer (uint32 direction, uint16 n_sectors);
SDCC_STATUS       sdcc_process_interrupts(sdcc_cmd_type *sdcc_cmd);
SDCC_STATUS       sdcc_poll_dma(void);
uint32            sdcc_blk_in_bits(uint32 size);
SDCC_CARD_TYPE    sdcc_find_sdio_card( void );
#ifdef T_QSC1100
#define GPIO_SDCC_OUT_ADDR      (HWIO_GPIO_OUT_0_ADDR)
#define GPIO_SDCC_IN_ADDR       (HWIO_GPIO_IN_0_ADDR+3)
#define GPIO_SDCC_OUT_MASK      0x3F000000
#define GPIO_SDCC_OUT_MASK_I    0xC0FFFFFF
#define GPIO_SDCC_CLK_H_MASK    0x3F000000
#define GPIO_SDCC_CLK_L_MASK    0x3E000000
#define GPIO_SDCC_CMD_MASK      0x1F000000
#define GPIO_SDCC_DAT_0_MASK    0x3D000000
#define GPIO_SDCC_DAT_MASK      0x21000000

byte   CRC7    ( unsigned char * chr, int cnt );
uint64 CRC16_4 ( unsigned char * chr, int cnt );
static INLINE void sdcc_clock_out(int cnt);
static INLINE void sdcc_clock_out_slow(int cnt);
static INLINE void sdcc_send_cmd_bytes(byte *pdata, int len);
static INLINE void sdcc_recv_cmd_bytes(byte *pdata, int len);
static INLINE byte sdcc_recv_cmd_byte_wait(void);
static INLINE SDCC_STATUS sdcc_send_data_bytes(byte *pdata, int len);
static INLINE SDCC_STATUS sdcc_recv_data_bytes(byte *pdata, int len);
static INLINE SDCC_STATUS sdcc_send_widedata_bytes(byte *pdata, int len);
static INLINE SDCC_STATUS sdcc_recv_widedata_bytes(byte *pdata, int len);
SDCC_STATUS sdcc_read_data(byte *buff, uint16 length);
SDCC_STATUS sdcc_write_data(byte *buff, uint16 length);
#endif
#endif /* ifdef __SDCC_PRIV_H */

