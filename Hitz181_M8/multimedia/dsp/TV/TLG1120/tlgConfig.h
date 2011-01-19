#ifndef _TLG_CONFIG_H_
#define _TLG_CONFIG_H_
/****************************************************************************/
/*!@file  tlgConfig.h
 *
 * @brief Compile-time configuration declarations for the TLG API.
 *
 * This file contains the compile time declarations for the TLG Ultra Low 
 * Power NTSC/PAL Tuner/Decoder chip support library.
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.43 $
 *
 * (c) 2008 Telegent Systems
 ****************************************************************************/
#include "tlg_defs.h"

#ifdef TLG_USE_DLL
    #ifdef TLGDLL_EXPORTS
        #define TLGDLL_API __declspec(dllexport)
    #else
        #define TLGDLL_API __declspec(dllimport)
    #endif /* TLGDLL_EXPORTS */
#else /* !TLG_USE_DLL */
    #define TLGDLL_API
#endif /* TLG_USE_DLL */


#ifdef __cplusplus
extern "C"  {
#endif

/** Enable/Disable the use of Shadow Registers.
 * 
 * If this is defined, it creates a shadow register array that is used to read
 * from the TLG to save access time. Note that the array must be populated
 * first by doing TLG_ReadRegNS() from TLG_MIN_REG_ADDR to TLG_MAX_REG_ADDR
 * to get the correct values into the shadow. 
 */
#ifndef TLG_NO_SHADOW
/*
    The TLG_NO_SHADOW is used to disable the shadow register from the compiler
    option or makefile
*/
#define TLG_USE_SHADOW_REGISTERS        (1)
#endif /* TLG_NO_SHADOW */

/** Enable/Disable built in error checking. 
 * 
 * If this is defined the TLG API will perform NULL pointer and bounds 
 * checking on its parameters. 
 */
#define TLG_DO_ERROR_CHECKING           (1)

/** Enable/Disable informative logging facility.
 *
 * If this is defined the TLG API will perform informative logging on exception
 * conditions. Note that for this to be enabled, a user must supply a 
 * custom routine TLG_WriteLog(char *str), to support the logging facility.
 */
//#define TLG_DO_REG_LOGGING              (1)

/** Enable/Disable the use of embedded timers in the channel change procedures.
 *
 * The portable TLG software provides both synchronous and asynchronous 
 * versions of many of its API calls. The synchronous calls require the use
 * of a timer facility TLG_Delay() (this is a customer-supplied procedure) to 
 * operate. This define turns on the use of the embedded timer facility. If
 * only the asynchronous calls are used, this feature should be turned off.
 */
#define TLG_EMBEDDED_TIMERS             (1) 

/** Use new faster lock detection algorithm.
 */
//#define TLG_USE_FAST_LOCK_DETECT        (1)

/** Select which Tracking Filter Dcycle Algorithm to use.
 *
 * this define switches between two tracking filter dcycle calculation
 * methods. This define should only be changed on the advice of a
 * Telegent Field Engineer.
 */
#define TLG_TF_CALC_DCYCLE_BY_HZ        (1)

#ifdef TLG_USE_DLL
/** Enable I2C Register Qualification Tests.
 * 
 * This define compiles in the TLG I2C register access qualification tests.
 *
 * @sa TLG_TestDefRegs
 * @sa TLG_TestWriteRegs
 */
#define TLG_ENABLE_I2C_QUAL_TESTS       (1)
#else /* ! TLG_USE_DLL */
#endif /* TLG_USE_DLL */


#define FIX_FM_DETECT		(1)			//just for v2.2 for remove FM detect issue 



/** Define word size of compiler.
 *
 * These defines define the word size for the compiler. For now only the "int"
 * type is managed. If the default "int" size for the compiler used to build
 * this code is 32, then leave TLG_INT_32BIT defined. If the default "int" 
 * size for the compiler is 16 then remove the define for TLG_INT_32BIT and
 * enable the define for TLG_INT_16BIT.
 */
#define TLG_INT_32BIT                   (1)
#define xTLG_INT_16BIT                   (1)

#if defined(TLG_INT_32BIT) && defined(TLG_INT_16BIT)
ERROR! Both TLG_INT32BIT and TLG_INT_16BIT may not be defined at the same time.
#endif

/** Define status of vertical screen lock.
 *
 * If defined, VSYNC signals are expected to arrive at exact intervals, hence
 * improving screen's vertical stability. However, not all Media Processors 
 * require this to be defined.
 */
#define xxTLG_VSCREEN_LOCK				(1)

/** Define if SSI/Signal Strength status is read by a polling program.
 *
 * SSI readout is strongly recommended to be performed during TLG operation.
 * This Define is recommended to leave as is.
 */
#define xTLG_NO_SSI_READOUT				(1)


/****************************************************************************
* CHIP VERSION CONFIGURATION
*
* These two constants should be set to the latest revision of TLG chip
* supported. Current valid chip versions are:
*
*   TLG_CHIP_TYPE       
*   - TLG_CHIP_TYPE_TLG1100_1
*   - TLG_CHIP_TYPE_TLG1150_1
*   - TLG_CHIP_TYPE_TLG1155_1
*   - TLG_CHIP_TYPE_TLG2300_1
*   - TLG_CHIP_TYPE_TLG1120_1
*
*   TLG_CHIP_REV
*   - TLG_CHIP_REV_1        TLG 1.1 chip
*   - TLG_CHIP_REV_2        TLG 1.2 chip
*   - TLG_CHIP_REV_3        TLG 2.1 chip
*****************************************************************************/

#define TLG_CHIP_TYPE               TLG_CHIP_TYPE_TLG1120_1
#define TLG_CHIP_REV                TLG_CHIP_REV_3

/*
 * Code Configuration Section
 *
 * The TLG API supports two modes of operation for multi-version chip support.
 * 1) Runtime chip support
 *      With this option all code for all versions of the chip is compiled
 *      into the TLG API and runtime checks are made to determine which code
 *      is to be executed based on chip version. This mode is enabled by
 *      the #define TLG_SUPPORT_RUNTIME_CHECK statement.
 *
 * 2) Compile-time chip support
 *      With this option only the code for the currently defined chip version
 *      (see TLG_CHIP_TYPE and TLG_CHIP_REV) is compiled into TLG API. This
 *      mode is enabled by NOT adding the TLG_SUPPORT_RUNTIME_CHECK statement.
 *
 * By default the TLG API is setup to support the Runtime chip support mode.
 * This is good for early development and while transitioning from one version
 * of chip to another. However, there is a small execution and codespace
 * savings for using the Compile-time mode, so this can be used for 
 * the production version of code if desired.
 */
/** Support Runtime determination of which chip to support.
* Remove this define to enable compile-time chip support.
*/
#define TLG_SUPPORT_RUNTIME_CHECK (1)

/*
    When switching the channel several time, the video starts to
    have more noise.  To fix that, force it to mix or amx GC and freeze
    the AGC.
*/

#define FIX_AGC_PROBLEM  

/*
    Temporary macro to test the new optimization
    To be removed later
*/
#define xxOPTIMIZE_AUDIO_NOTCH      (1)

#define xxUSE_5_NOTCHES             (1)

/** FM interface Selection
 *
 * If this is defined the TLG API will use FM new interface (frequency based) which optimise the FM scan 
 * algorithm and reduce the scan time.
 *
 */
#define TLG_FM_NEW_INTERFACE        1
/*
    Enable the 0x6c[10] favg_auto3 for frame averaging
    This option is not tested.
    Leave it off until tested.
*/
#define USE_FAVG_AUTO3

#define RELEASE_AGC_RIGHT_AWAY
/*
    Should use Zeus mode for now.
*/
#define xxUSE_APOLLO_AUDIO_MODE_ONLY (1)

 
/*
    Enable the register setting test
*/
#define xENABLE_REGISTER_SETTING_TEST (1)

/*
    Enable debug file loggging functions

*/
#define xDEBUG_FILE_LOG            (1)

/*
    Enable pal-d/k and pal-i auto detection

*/
//#define HYBRID_DK_I    (1)  // For china mainland and shenzhen

/*
 *   To reduce code, only enable the channel maps suported.
 *   Be default only certain channel maps are enabled
 *   When the channel map is disabled, both the channel map
 *   table and the function is disabled to reduce the code size
*/

#define SUPPORT_CHANNEL_MAP_NTSC
#define SUPPORT_CHANNEL_MAP_CHINA
#define SUPPORT_CHANNEL_MAP_VIETNAM
#define SUPPORT_CHANNEL_MAP_PALWE
#define SUPPORT_CHANNEL_MAP_JAPAN
#define SUPPORT_CHANNEL_MAP_UK
#define SUPPORT_CHANNEL_MAP_BRAZIL
#define SUPPORT_CHANNEL_MAP_ARGENTINA
#define SUPPORT_CHANNEL_MAP_SECAM_BG
#define SUPPORT_CHANNEL_MAP_SECAM_DK




#ifdef __cplusplus
};
#endif

#endif /* _TLG_CONFIG_H_ */



