#ifndef _TLG1100HAL_H_
#define _TLG1100HAL_H_
/****************************************************************************/
/*!@file  tlg1100Hal.h
 *
 * @brief Hardware Definitions for the TLG1100 programmatic interface support.
 *
 * This file contains the api definitions for the TLG1100 Ultra Low Power
 * NTSC/PAL Tuner/Decoder chip support library.
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.29 $
 *
 * (c) 2006 Telegent Systems
 ****************************************************************************/

#include "tlg1100Config.h"

#define BIT0                        ((unsigned short)0x0001) /*!< bit  0 */
#define BIT1                        ((unsigned short)0x0002) /*!< bit  1 */
#define BIT2                        ((unsigned short)0x0004) /*!< bit  2 */
#define BIT3                        ((unsigned short)0x0008) /*!< bit  3 */
#define BIT4                        ((unsigned short)0x0010) /*!< bit  4 */
#define BIT5                        ((unsigned short)0x0020) /*!< bit  5 */
#define BIT6                        ((unsigned short)0x0040) /*!< bit  6 */
#define BIT7                        ((unsigned short)0x0080) /*!< bit  7 */
#define BIT8                        ((unsigned short)0x0100) /*!< bit  8 */
#define BIT9                        ((unsigned short)0x0200) /*!< bit  9 */
#define BIT10                       ((unsigned short)0x0400) /*!< bit 10 */
#define BIT11                       ((unsigned short)0x0800) /*!< bit 11 */
#define BIT12                       ((unsigned short)0x1000) /*!< bit 12 */
#define BIT13                       ((unsigned short)0x2000) /*!< bit 13 */
#define BIT14                       ((unsigned short)0x4000) /*!< bit 14 */
#define BIT15                       ((unsigned short)0x8000) /*!< bit 15 */

#define BIT3_0                      ((unsigned short)0x000f)
#define BIT7_4                      ((unsigned short)0x00f0)
#define BIT11_8                     ((unsigned short)0x0f00)
#define BIT15_12                    ((unsigned short)0xf000)
#define BIT7_0                      (BIT7_4|BIT3_0)
#define BIT15_8                     (BIT15_12|BIT11_8)
#define BIT15_0                     (BIT15_8|BIT7_0)
#define BIT11_0                     (BIT11_8|BIT7_0)
#define BIT14_10                    (BIT14|BIT13|BIT12|BIT11|BIT10)
#define BIT6_0                      (BIT6|BIT5|BIT4|BIT3_0)
#define BIT5_0                      (BIT5|BIT4|BIT3_0)
#define BIT11_9                     (BIT11|BIT10|BIT9)
#define BIT12_9                     (BIT12|BIT11_9)

#define     REG_BIT15_MASK			(BIT15)
#define     REG_BIT15_SHIFT		(15)
#define     REG_BIT14_MASK			(BIT14)
#define     REG_BIT14_SHIFT		(14)
#define     REG_BIT13_MASK			(BIT13)
#define     REG_BIT13_SHIFT		(13)
#define     REG_BIT12_MASK			(BIT12)
#define     REG_BIT12_SHIFT		(12)
#define     REG_BIT11_MASK			(BIT11)
#define     REG_BIT11_SHIFT		(11)
#define     REG_BIT10_MASK			(BIT10)
#define     REG_BIT10_SHIFT		(10)
#define     REG_BIT9_MASK			(BIT9)
#define     REG_BIT9_SHIFT			(9)
#define     REG_BIT8_MASK			(BIT8)
#define     REG_BIT8_SHIFT			(8)
#define     REG_BIT7_MASK			(BIT7)
#define     REG_BIT7_SHIFT			(7)
#define     REG_BIT6_MASK			(BIT6)
#define     REG_BIT6_SHIFT			(6)
#define     REG_BIT5_MASK			(BIT5)
#define     REG_BIT5_SHIFT			(5)
#define     REG_BIT4_MASK			(BIT4)
#define     REG_BIT4_SHIFT			(4)
#define     REG_BIT3_MASK			(BIT3)
#define     REG_BIT3_SHIFT			(3)
#define     REG_BIT2_MASK			(BIT2)
#define     REG_BIT2_SHIFT			(2)
#define     REG_BIT1_MASK			(BIT1)
#define     REG_BIT1_SHIFT			(1)
#define     REG_BIT0_MASK			(BIT0)
#define     REG_BIT0_SHIFT			(0)
/** Clear a masked area in a register value                                   */
#define TLGHAL_CLEAR(reg, mask) reg &= ((uint16) ~(mask))

/** Set a masked area to a value                                              */
#define TLGHAL_SET(reg, var, shift, mask) \
                    reg = ((uint16) (((reg) & ~(mask)) | (((var) << (shift)) & (mask))))

/** Get a field in a register                                                 */
#define TLGHAL_GET(var, reg, shift, mask) var = (((reg) & (mask)) >> (shift))

#endif /* _TLG1100HAL_H_ */
