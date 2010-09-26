#ifndef _TLG_BIT_DEFINITION_H_
#define _TLG_BIT_DEFINITION_H_
/****************************************************************************/
/*!@file  tlg_bit_definition.h
 *
 * @brief Macro for all bit definitions.
 *
 * This file contains the bit definitions for the TLG1100 Ultra Low Power
 * NTSC/PAL Tuner/Decoder chip support library.
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.3 $
 *
 * (c) 2008 Telegent Systems
 ****************************************************************************/

#ifndef uint16

#define uint16  unsigned short

#endif /* uint16 */

#ifndef uint32

#define uint32  unsigned long

#endif /* uint32 */


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

#define BIT7_4                      ((unsigned short)0x00f0)
#define BIT11_8                     ((unsigned short)0x0f00)
#define BIT15_12                    ((unsigned short)0xf000)

#define BIT1_0                      ((unsigned short)0x0003)
#define BIT2_0                      ((unsigned short)0x0007)
#define BIT3_0                      ((unsigned short)0x000f)
#define BIT4_0                      ((unsigned short)0x001f)
#define BIT5_0                      ((unsigned short)0x003f)
#define BIT6_0                      ((unsigned short)0x007f)
#define BIT7_0                      ((unsigned short)0x00ff)
#define BIT8_0                      ((unsigned short)0x01ff)
#define BIT9_0                      ((unsigned short)0x03ff)
#define BIT10_0                     ((unsigned short)0x07ff)
#define BIT11_0                     ((unsigned short)0x0fff)
#define BIT12_0                     ((unsigned short)0x1fff)
#define BIT13_0                     ((unsigned short)0x3fff)
#define BIT14_0                     ((unsigned short)0x7fff)
#define BIT15_0                     ((unsigned short)0xffff)

#define BIT2_1                      ((unsigned short)0x0006)
#define BIT3_1                      ((unsigned short)0x000e)
#define BIT4_1                      ((unsigned short)0x001e)
#define BIT5_1                      ((unsigned short)0x003e)
#define BIT6_1                      ((unsigned short)0x007e)
#define BIT7_1                      ((unsigned short)0x00fe)
#define BIT8_1                      ((unsigned short)0x01fe)
#define BIT9_1                      ((unsigned short)0x03fe)
#define BIT10_1                     ((unsigned short)0x07fe)
#define BIT11_1                     ((unsigned short)0x0ffe)
#define BIT12_1                     ((unsigned short)0x1ffe)
#define BIT13_1                     ((unsigned short)0x3ffe)
#define BIT14_1                     ((unsigned short)0x7ffe)
#define BIT15_1                     ((unsigned short)0xfffe)

#define BIT3_2                      ((unsigned short)0x000c)
#define BIT4_2                      ((unsigned short)0x001c)
#define BIT5_2                      ((unsigned short)0x003c)
#define BIT6_2                      ((unsigned short)0x007c)
#define BIT7_2                      ((unsigned short)0x00fc)
#define BIT8_2                      ((unsigned short)0x01fc)
#define BIT9_2                      ((unsigned short)0x03fc)
#define BIT10_2                     ((unsigned short)0x07fc)
#define BIT11_2                     ((unsigned short)0x0ffc)
#define BIT12_2                     ((unsigned short)0x1ffc)
#define BIT13_2                     ((unsigned short)0x3ffc)
#define BIT14_2                     ((unsigned short)0x7ffc)
#define BIT15_2                     ((unsigned short)0xfffc)

#define BIT4_3                      ((unsigned short)0x0018)
#define BIT5_3                      ((unsigned short)0x0038)
#define BIT6_3                      ((unsigned short)0x0078)
#define BIT7_3                      ((unsigned short)0x00f8)
#define BIT8_3                      ((unsigned short)0x01f8)
#define BIT9_3                      ((unsigned short)0x03f8)
#define BIT10_3                     ((unsigned short)0x07f8)
#define BIT11_3                     ((unsigned short)0x0ff8)
#define BIT12_3                     ((unsigned short)0x1ff8)
#define BIT13_3                     ((unsigned short)0x3ff8)
#define BIT14_3                     ((unsigned short)0x7ff8)
#define BIT15_3                     ((unsigned short)0xfff8)

#define BIT5_4                      ((unsigned short)0x0030)
#define BIT6_4                      ((unsigned short)0x0070)
#define BIT7_4                      ((unsigned short)0x00f0)
#define BIT8_4                      ((unsigned short)0x01f0)
#define BIT9_4                      ((unsigned short)0x03f0)
#define BIT10_4                     ((unsigned short)0x07f0)
#define BIT11_4                     ((unsigned short)0x0ff0)
#define BIT12_4                     ((unsigned short)0x1ff0)
#define BIT13_4                     ((unsigned short)0x3ff0)
#define BIT14_4                     ((unsigned short)0x7ff0)
#define BIT15_4                     ((unsigned short)0xfff0)

#define BIT6_5                      ((unsigned short)0x0060)
#define BIT7_5                      ((unsigned short)0x00e0)
#define BIT8_5                      ((unsigned short)0x01e0)
#define BIT9_5                      ((unsigned short)0x03e0)
#define BIT10_5                     ((unsigned short)0x07e0)
#define BIT11_5                     ((unsigned short)0x0fe0)
#define BIT12_5                     ((unsigned short)0x1fe0)
#define BIT13_5                     ((unsigned short)0x3fe0)
#define BIT14_5                     ((unsigned short)0x7fe0)
#define BIT15_5                     ((unsigned short)0xffe0)

#define BIT7_6                      ((unsigned short)0x00c0)
#define BIT8_6                      ((unsigned short)0x01c0)
#define BIT9_6                      ((unsigned short)0x03c0)
#define BIT10_6                     ((unsigned short)0x07c0)
#define BIT11_6                     ((unsigned short)0x0fc0)
#define BIT12_6                     ((unsigned short)0x1fc0)
#define BIT13_6                     ((unsigned short)0x3fc0)
#define BIT14_6                     ((unsigned short)0x7fc0)
#define BIT15_6                     ((unsigned short)0xffc0)

#define BIT8_7                      ((unsigned short)0x0180)
#define BIT9_7                      ((unsigned short)0x0380)
#define BIT10_7                     ((unsigned short)0x0780)
#define BIT11_7                     ((unsigned short)0x0f80)
#define BIT12_7                     ((unsigned short)0x1f80)
#define BIT13_7                     ((unsigned short)0x3f80)
#define BIT14_7                     ((unsigned short)0x7f80)
#define BIT15_7                     ((unsigned short)0xff80)

#define BIT9_8                      ((unsigned short)0x0300)
#define BIT10_8                     ((unsigned short)0x0700)
#define BIT11_8                     ((unsigned short)0x0f00)
#define BIT12_8                     ((unsigned short)0x1f00)
#define BIT13_8                     ((unsigned short)0x3f00)
#define BIT14_8                     ((unsigned short)0x7f00)
#define BIT15_8                     ((unsigned short)0xff00)

#define BIT10_9                     ((unsigned short)0x0600)
#define BIT11_9                     ((unsigned short)0x0e00)
#define BIT12_9                     ((unsigned short)0x1e00)
#define BIT13_9                     ((unsigned short)0x3e00)
#define BIT14_9                     ((unsigned short)0x7e00)
#define BIT15_9                     ((unsigned short)0xfe00)

#define BIT11_10                    ((unsigned short)0x0c00)
#define BIT12_10                    ((unsigned short)0x1c00)
#define BIT13_10                    ((unsigned short)0x3c00)
#define BIT14_10                    ((unsigned short)0x7c00)
#define BIT15_10                    ((unsigned short)0xfc00)

#define BIT12_11                    ((unsigned short)0x1800)
#define BIT13_11                    ((unsigned short)0x3800)
#define BIT14_11                    ((unsigned short)0x7800)
#define BIT15_11                    ((unsigned short)0xf800)

#define BIT13_12                    ((unsigned short)0x3000)
#define BIT14_12                    ((unsigned short)0x7000)
#define BIT15_12                    ((unsigned short)0xf000)

#define BIT14_13                    ((unsigned short)0x6000)
#define BIT15_13                    ((unsigned short)0xe000)

#define BIT15_14                    ((unsigned short)0xc000)

/** Clear a masked area in a register value                                   */
#define TLGHAL_CLEAR(reg, mask) reg &= ((uint16) ~(mask))

/** Set a masked area to a value                                              */
#define TLGHAL_SET(reg, var, shift, mask) \
                    reg = ((uint16) (((reg) & ~(mask)) | (((var) << (shift)) & (mask))))

/** Get a field in a register                                                 */
#define TLGHAL_GET(var, reg, shift, mask) var = (((reg) & (mask)) >> (shift))

#define HZCVTUNITS  (100)  /* CVT domain in units of 100s Hz */

/* convert MHZ to CVT domain */
#define MHZCVT(a)  ((uint32)((a)*((uint32)1000000/(uint32)HZCVTUNITS)))

/* convert HZ to CVT domain */
#define HZCVT(a)   ((uint32)(((uint32)(a))/(uint32)HZCVTUNITS))

/* convert CVT domain to MHZ (note since its uint32, will trucate) */
#define CVTMHZ(a)  ((uint32)((((uint32)(a))*HZCVTUNITS)/(uint32)1000000))


#endif /* _TLG_BIT_DEFINITION_H_ */
