#ifndef _TLG1100API_H_
#define _TLG1100API_H_
/****************************************************************************/
/*!@file  tlg1100Api.h
 *
 * @brief Api Definitions for the TLG1100 programmatic interface support.
 *
 * This file contains the api definitions for the TLG1100 Ultra Low Power
 * NTSC/PAL Tuner/Decoder chip support library.
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.49 $
 *
 * (c) 2006 Telegent Systems
 ****************************************************************************/

#ifdef TLG_USE_DLL
    #ifdef TLG1100DLL_EXPORTS
        #define TLG1100DLL_API __declspec(dllexport)
    #else
        #define TLG1100DLL_API __declspec(dllimport)
    #endif /* TLG1100DLL_EXPORTS */
#else /* !TLG_USE_DLL */
    #define TLG1100DLL_API
#endif /* TLG_USE_DLL */


#ifdef __cplusplus
extern "C"  {
#endif

#include "tlg1100Hal.h"

#include "stack_config.h"

/** This should be a customizable variable for now.
 */
#define TLG_RESTART_THRESH_DEV          (1) /** undefine this to make const */

#define TLG_RESTART_THRESH_DEF_VAL      (500)   

#ifdef TLG_RESTART_THRESH_DEV
extern TLG1100DLL_API int TLG_RESTART_THRESH;
#else
#define TLG_RESTART_THRESH             TLG_RESTART_THRESH_DEF_VAL   
#endif /* TLG_RESTART_THRESH_DEV */

#ifndef   uint8
#define   uint8         unsigned char
#endif /* uint8 */

#ifndef   uint16
#define   uint16        unsigned short
#endif /* uint16 */

#ifndef   uint32
#ifdef TLG_INT_32BIT
#define   uint32        unsigned int
#else
#define   uint32        unsigned long
#endif /* TLG_INT_32BIT */
#endif /* uint32 */

#ifndef int32
#ifdef TLG_INT_32BIT
#define int32           int
#else
#define int32           long
#endif /* TLG_INT_32BIT */
#endif /* int32 */

#ifndef NULL
#define NULL ((void*)0)
#endif /* NULL */

#ifndef __cplusplus
#ifndef bool
#define bool            uint32
#endif /* bool */

#ifndef false
#define false           (0)
#endif /* false */

#ifndef true
#define true            (1)
#endif /* true */
#endif /* __cplusplus */

#ifndef pow2
#define pow2(a)    (((uint32)1) << (a))
#endif /* pow2 */

#ifndef max
#define max(x, y)  (((x) > (y)) ? (x) : (y))
#endif /* max */

#ifndef min
#define min(x, y)  (((x) < (y)) ? (x) : (y))
#endif /* min */

/** Supported frequency bandwidth settings for TLG API.                     */
typedef enum {

    TLG_BW_5 = 5,               /*!< 5 MHz bandwidth                        */
    TLG_BW_6 = 6,               /*!< 6 MHz bandwidth                        */ 
    TLG_BW_7 = 7,               /*!< 7 MHz bandwidth                        */
    TLG_BW_8 = 8,               /*!< 8 MHz bandwidth                        */
    TLG_BW_NONE

} tlg_bw_t;

/** Modes for TLG1100.                                                      */
typedef enum {
    TLG_MODE_NONE,              /*!< No Mode specified                      */
    TLG_MODE_ANALOG_TV,         /*!< Analog Television mode                 */
    TLG_MODE_FM_RADIO           /*!< FM Radio mode                          */
} tlg_mode_t;

/** LNA Types for TLG1100                                                   */
typedef enum {
    TLG_LNA_NONE            = 0,
    TLG_LNA_UHF             = 3,
    TLG_LNA_VHF2            = 5,
    TLG_LNA_VHF1            = 6
} tlg_lna_t;

#define TLG_VSTD_HYBRID_BIT     (1 << 15)
/** Video Standards to configure TLG1100.                                   */
typedef enum {
    TLG_VSTD_NTSC_M = 0,        /*!< NTSC-M                                 */
    TLG_VSTD_PAL_M  = 1,        /*!< PAL-M                                  */
    TLG_VSTD_PAL_B  = 2,        /*!< PAL-B                                  */
    TLG_VSTD_PAL_B1 = 3,        /*!< PAL-B1                                 */
    TLG_VSTD_PAL_D  = 4,        /*!< PAL-D                                  */
    TLG_VSTD_PAL_D1 = 5,        /*!< PAL-D1                                 */
    TLG_VSTD_PAL_G  = 6,        /*!< PAL-G                                  */
    TLG_VSTD_PAL_H  = 7,        /*!< PAL-H                                  */
    TLG_VSTD_PAL_K  = 8,        /*!< PAL-K                                  */
    TLG_VSTD_PAL_N  = 9,        /*!< PAL-N                                  */
    TLG_VSTD_PAL_I  = 10,       /*!< PAL-I                                  */
    TLG_VSTD_PAL_NC = 11,       /*!< PAL-NC                                 */
    TLG_VSTD_SECAM_M        = 17                      ,     /*!< SECAM-M                                */
    TLG_VSTD_SECAM_B        = 18                      ,     /*!< SECAM-B                                */
    TLG_VSTD_SECAM_B1       = 19                      ,     /*!< SECAM-B1                               */
    TLG_VSTD_SECAM_D        = 20                      ,     /*!< SECAM-D                                */
    TLG_VSTD_SECAM_D1       = 21                      ,     /*!< SECAM-D1                               */
    TLG_VSTD_SECAM_G        = 22                      ,     /*!< SECAM-G                                */
    TLG_VSTD_SECAM_H        = 23                      ,     /*!< SECAM-H                                */
    TLG_VSTD_SECAM_K        = 24                      ,     /*!< SECAM-K                                */
    TLG_VSTD_SECAM_N        = 25                      ,     /*!< SECAM-N                                */
    TLG_VSTD_SECAM_I        = 26                      ,     /*!< SECAM-I                                */
    TLG_VSTD_SECAM_NC       = 27                      ,     /*!< SECAM-NC                               */
    TLG_VSTD_SECAM_L        = 31                      ,     /*!< SECAM-L                                */
    TLG_VSTD_PAL_SECAM_M    = 1  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-M                                  */
    TLG_VSTD_PAL_SECAM_B    = 2  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-B                                  */
    TLG_VSTD_PAL_SECAM_B1   = 3  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-B1                                 */
    TLG_VSTD_PAL_SECAM_D    = 4  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-D                                  */
    TLG_VSTD_PAL_SECAM_D1   = 5  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-D1                                 */
    TLG_VSTD_PAL_SECAM_G    = 6  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-G                                  */
    TLG_VSTD_PAL_SECAM_H    = 7  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-H                                  */
    TLG_VSTD_PAL_SECAM_K    = 8  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-K                                  */
    TLG_VSTD_PAL_SECAM_N    = 9  | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-N                                  */
    TLG_VSTD_PAL_SECAM_I    = 10 | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-I                                  */
    TLG_VSTD_PAL_SECAM_NC   = 11 | TLG_VSTD_HYBRID_BIT, /*!< PAL/SECAM-NC                                 */

    TLG_VSTD_NTSC_B_G       = 12,
    TLG_VSTD_NTSC_D_K       = 13,
    TLG_VSTD_NTSC_I         = 14,  
    
    TLG_VSTD_NONE               /*!< No Video Standard Specified            */
} tlg_vidstd_t;

/** Antenna Configurations supported.                                       */
typedef enum {
    TLG_ANTENNA_DEF = 0,        /*!< Default Antenna Behavior               */
    TLG_ANTENNA_HDPH,           /*!< Settings for the TLG_HDPH Antenna      */
    TLG_ANTENNA_MAX             /*!< Number of Antenna settings             */
} tlg_antenna_t;


/** Errors returnd by TLG API.                                              */
typedef enum {
    TLG_ERR_SUCCESS= 0,     /*!< No Errors                                  */
    TLG_ERR_FAIL,           /*!< Generic Failure during processing          */
    TLG_ERR_PARAM,          /*!< Bad parameter passed                       */
    TLG_ERR_NOT_SUPPORTED,  /*!< Requested feature is not supported         */
    TLG_ERR_MAX
} TLGERR;


/** Read a register value, possibly through the shadowed registers.
 *
 * Read a register value. If TLG_USE_SHADOW_REGISTERS is defined then this
 * routine will return the shadowed register value instead of retrieving it
 * from the chip. If TLG_USE_SHADOW_REGISTERS is not defined then this routine
 * will access the register directly from the chip. 
 *
 * @param base_addr     I2c address for TLG1100 chip.
 * @param reg_addr      Register Address to retrieve.
 * @param val           Returned register value.
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLG1100DLL_API int TLG_ReadReg(uint32 base_addr, uint32 reg_addr, uint16 *val);


/** Read a register value from the TLG1100.
 *
 * This procedure will always retrieve the register value direclty from the
 * TLG1100 even if TLG_USE_SHADOW_REGISTERS is defined. If
 * TLG_USE_SHADOW_REGISTERS is defined, the shadowed value will be updated with
 * the new value from the chip.
 *
 * @param base_addr     I2c address for TLG1100 chip.
 * @param reg_addr      Register Address to retrieve.
 * @param val           Returned register value.
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLG1100DLL_API int TLG_ReadRegNS(uint32 base_addr, uint32 reg_addr, uint16 *val);


/** Write a register value to the TLG1100.
 *
 * This procedure writes the specified register value at the specified 
 * register address on the TLG1100. If TLG_USE_SHADOW_REGISTERS is defined it
 * will also update the shadowed registers with the written value.
 *
 * @param base_addr     I2c address for TLG1100 chip.
 * @param reg_addr      Address of register to write.
 * @param val           Register value to write.
 * 
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLG1100DLL_API int TLG_WriteReg(uint32 base_addr, uint32 reg_addr, uint16 val);

/** Perform a synchronous video channel change and verify its locking status.
 *
 * This procedure performs a TLG_SetTVChnFreq() and then interrogates
 * the status registers to ascertain the locking state. Note all initial
 * setup requiremens that apply to TLG_SetTVChnFreq() apply to this function
 * as well.
 * THIS PROCEDURE IS A SYNCHRONOUS CALL AND MAY CONTAIN EMBEDDED CALLS TO 
 * TLG_Delay(). IF TLG_EMBEDDED_TIMERS IS NOT DEFINED THIS PROCEDURE WILL 
 * RETURN TLG_ERR_NOT_SUPPORTED AND NO ACTION WILL BE PERFORMED.
 * 
 * @param base_addr         I2c address for TLG1100 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * 
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLG1100DLL_API int TLG_ScanTVChn(uint32 base_addr, uint32 center_freq_hz);

#ifdef __cplusplus
};
#endif

#endif /* _TLG1100API_H_ */
