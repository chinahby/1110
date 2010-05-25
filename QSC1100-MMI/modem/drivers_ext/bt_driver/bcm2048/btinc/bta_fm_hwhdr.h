/*****************************************************************************
**
**  Name:           bta_fm_hwhdr.h
**
**  Description:    FM hardware related register and corresponding bit definition.
**
**  Copyright (c) 2006, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/
#ifndef BTA_FM_HWHDR_H
#define BTA_FM_HWHDR_H

#include "hcidefs.h"

#define BTA_FM_2048_OP_CODE         HCI_BRCM_FM_OPCODE

/* I2C register Address */
#define I2C_FM_REG_RDS_SYS          0x00
#define I2C_FM_REG_FM_CTRL          0x01
#define I2C_FM_REG_RDS_CTL0         0x02

#define I2C_FM_REG_AUD_PAUS         0x04
#define I2C_FM_REG_AUD_CTL0         0x05
#define I2C_FM_REG_AUD_CTL1         0x06
#define I2C_FM_REG_SCH_CTL0         0x07

#define I2C_FM_REG_SCH_TUNE         0x09         
#define I2C_FM_REG_FM_FREQ          0x0a
#define I2C_FM_REG_FM_FREQ1         0x0b
#define I2C_FM_REG_AF_FREQ0         0x0c
#define I2C_FM_REG_AF_FREQ1         0x0d
#define I2C_FM_REG_CARRIER          0x0e
#define I2C_FM_REG_RSSI             0x0f
#define I2C_FM_REG_FM_RDS_MSK       0x10
#define I2C_FM_REG_FM_RDS_MSK1      0x11
#define I2C_FM_REG_FM_RDS_FLAG      0x12
#define I2C_FM_REG_FM_RDS_FLAG1     0x13
#define I2C_FM_REG_RDS_WLINE        0x14

#define I2C_FM_REG_BB_MAC0          0x16
#define I2C_FM_REG_BB_MAC1          0x17
#define I2C_FM_REG_BB_MSK0          0x18
#define I2C_FM_REG_BB_MSK1          0x19
#define I2C_FM_REG_PI_MAC0          0x1a
#define I2C_FM_REG_PI_MAC1          0x1b
#define I2C_FM_REG_PI_MSK0          0x1c
#define I2C_FM_REG_PI_MSK1          0x1d

#define I2C_FM_REG_RCV_ID           0x28
#define I2C_FM_REG_I2C_CFG          0x29
#define I2C_FM_REG_RDS_DATA         0x80
#define I2C_FM_REG_AF_FAILURE       0x90    /* AF jump failure reason code */
#define I2C_FM_REG_PCM_ROUTE        0x4d
#define I2C_FM_SEARCH_METHOD        0xfc    /* scan mode for 2048B0 FW */
#define I2C_FM_REG_PRESET_MAX       0xfe
#define I2C_FM_REG_PRESET_STA       0xff


/* bits in register I2C_FM_RDS_SYS  0x00 */     
#define I2C_FM_ON                   0x01
#define I2C_RDS_ON                  0x02

/* bits in register I2C_FM_REG_FM_CTRL  0x01 */ 
#define I2C_BAND_REG_WEST           0x00
#define I2C_BAND_REG_EAST           0x01    
#define I2C_STEREO_AUTO             0x02
#define I2C_STEREO_MANUAL           0x04
#define I2C_STEREO_SWITCH           0x08
#define I2C_HI_LO_INJ               0x10

/* bits in register I2C_FM_REG_RDS_CTL0  0x02 */     
#define I2C_RDS_CTRL_RDS            0x00
#define I2C_RDS_CTRL_RBDS           0x01
#define I2C_RDS_CTRL_FIFO_FLUSH     0x02

/* bits in register I2C_FM_REG_AUD_PAUS 0x04 */
#define I2C_AUD_PAUSE_21_THRESH     0x00 /* bit [0:3]audio pause RSSI magnitude threshold */
#define I2C_AUD_PAUSE_18_THRESH     0x01
#define I2C_AUD_PAUSE_15_THRESH     0x02
#define I2C_AUD_PAUSE_12_THRESH     0x03
#define I2C_AUD_PAUSE_20_MS         0x00 /* bit [4:7]audio pause duration */
#define I2C_AUD_PAUSE_26_MS         0x10
#define I2C_AUD_PAUSE_33_MS         0x20
#define I2C_AUD_PAUSE_40_MS         0x30

/* bits in register I2C_FM_REG_AUD_CTL0  0x05 */     
#define I2C_RF_MUTE                 0x0001    /* bit0 */
#define I2C_MANUAL_MUTE             0x0002    /* bit1 */
#define I2C_Z_MUTE_LEFT_OFF         0x0004    /* bit2 */
#define I2C_Z_MUTE_RITE_OFF         0x0008   /* bit3 */
#define I2C_AUDIO_DAC_ON            0x0010    /* bit4 */
#define I2C_AUDIO_I2S_ON            0x0020    /* bit5 */
#define I2C_DEEMPHA_75_ON           0x0040    /* bit6 */
#define I2C_AUDIO_BAND_WIDTH        0x0080    /* bit7 */

/* FM search control */
#define I2C_SCH_DIRCT_UP            0x80    /* bit7 on  */
#define I2C_SCH_DIRCT_DOWN          0x00    /* bit7 off */

/* FM search tune mode, used to set I2C_FM_REG_SCH_TUNE 0x09 register */
#define I2C_IDLE_MODE            0x00       /* Idle mode */
#define I2C_PRE_SET_MODE         0x01       /* preset mode, used for tuning */
#define I2C_AUTO_SCH_MODE        0x02       /* auto search, used for scanning */
#define I2C_AF_JUMP_MODE         0x03       /* af jump mode */

/* the highest bit on I2C_FM_REG_PCM_ROUTE register */
#define I2C_PCM_ROUTE_ON_BIT        0x80    /* FM on SCO */

/* MSB in I2C_FM_REG_SCH_CTL0 0x07 */
#define I2C_SEARCH_UP_CTRL           0x80        /* scan upward        */
#define I2C_SEARCH_DN_CTRL           0x00         /* scan downward    */


/* bits in I2C_FM_REG_FM_RDS_FLAG 0x12 */
#define I2C_MASK_SRH_TUNE_CMPL_BIT     (0x0001 << 0)    /* FM/RDS register search/tune cmpl bit */
#define I2C_MASK_SRH_TUNE_FAIL_BIT     (0x0001 << 1)    /* FM/RDS register search/tune fail bit */
#define I2C_MASK_RSSI_LOW_BIT          (0x0001 << 2)    /* FM/RDS registerRSSI low bit */
#define I2C_MASK_CARR_HI_ERR_BIT       (0x0001 << 3)    /* FM/RDS register carrier high err bit */
#define I2C_MASK_AUDIO_PAUSE_BIT       (0x0001 << 4)    /* audio has paused for the specific threshold and duration */
#define I2C_MASK_STEREO_DETC_BIT       (0x0001 << 5)    /* FM/RDS register search/tune cmpl bit */
#define I2C_MASK_STEREO_ACTIVE_BIT     (0x0001 << 6)    /* FM/RDS register search/tune fail bit */
/* second bytes of FM_RDS_FLG 0x13 */
#define I2C_MASK_RDS_FIFO_WLINE_BIT    (0x0100 << 1)    /* RDS tuples are currently available at
                                                        a level >= waterline */
#define I2C_MASK_BB_MATCH_BIT          (0x0100 << 3)    /* PI code match found */
#define I2C_MASK_PI_MATCH_BIT          (0x0100 << 5)    /* PI code match found */


/* I2C_FM_REG_RDS_DATA 0x80 reading */
#define I2C_FM_RDS_END_TUPLE_1ST_BYTE    0x7c /* 1st byte of a RDS ending tuple */
#define I2C_FM_RDS_END_TUPLE_2ND_BYTE    0xff /* 2nd byte of a RDS ending tuple */
#define I2C_FM_RDS_END_TUPLE_3RD_BYTE    0xff /* 3rd byte of a RDS ending tuple */


/* I2C_FM_REG_PRESET_STA readfing */
#define I2C_PRESET_STA_MAX              62

/* I2C_FM_SEARCH_METHOD */
#define I2C_FM_SEARCH_NORMAL            0x00
#define I2C_FM_SEARCH_PRESET            0x01
#define I2C_FM_SEARCH_RSSI              0x02

/* I2C_FM_REG_AF_FAILURE reason code detail */
#define I2C_AF_FAIL_RESV        0x00    /* reserved */
#define I2C_AF_FAIL_RSSI_LOW    (1 << 4)    /* RSSI low,  bit 4*/
#define I2C_AF_FAIL_FREQ_OSHI   (1 << 5)    /* Frequency Offset high, bit 5 */
#define I2C_AF_FAIL_PI_ERR      (3 << 4)    /* PI mismatch, bit 4 & 5 */

#endif /* BTA_FM_HWHDR_H */
