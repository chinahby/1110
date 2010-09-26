#ifndef _TLG1120API_H_
#define _TLG1120API_H_
/****************************************************************************/
/*!@file  tlg1120Api.h
 *
 * @brief Api Definitions for the TLG1120 programmatic interface support.
 *
 * This file contains the api definitions for the TLG1120 Ultra Low Power
 * NTSC/PAL Tuner/Decoder chip support library.
 *
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.93.2.7 $
 *
 * (c) 2008 Telegent Systems
 ****************************************************************************/
//#include "aitc_config.h"
#if 0   //add by yangdecai 09-24
#include "kal_release.h"
#endif
#include "a8_common.h"
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

#if TLG_CHIP_TYPE == TLG_CHIP_TYPE_TLG1120_1

/** This should be a customizable variable for now.
 */
#define TLG_RESTART_THRESH_DEV          (1) /** undefine this to make const */
#define TLG_RESTART_THRESH_DEF_VAL      (500)

#ifdef TLG_RESTART_THRESH_DEV
extern TLGDLL_API int TLG_RESTART_THRESH;
#else /* TLG_RESTART_THRESH_DEV */
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
#else  /* uint32 */
#define   uint32        unsigned long
#endif /* TLG_INT_32BIT */
#endif /* uint32 */

#ifndef int32
#ifdef TLG_INT_32BIT
#define int32           int
#else  /* int32 */
#define int32           long
#endif /* TLG_INT_32BIT */
#endif /* int32 */

#ifndef NULL
#define NULL ((void*)0)
#endif /* NULL */

#ifndef __cplusplus
#ifndef bool
//#define bool            uint32
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

#ifndef abs
#define abs(x) (((x) >= 0) ? (x) : (-(x)))
#endif /* abs */

/*
 * TLG1120 Portable Software Library
 */
#define TLG_VERSION_MAJOR   (2)
#define TLG_VERSION_MINOR   (1)
#define TLG_VERSION_PATCH   (0)
#define TLG_VERSION_BUILD   (0)
#define TLG_VERSION_CUSTM	("TLG")


#define TLG_OFF             (0)
#define TLG_ON              (1)

#define TLG_COLOR_BAR       (2)

#define TLG_MVE_AUTO        (1) /*!<Mobile Video Enhancement Auto Mode      */
#define TLG_MVE_MANUAL      (0) /*!<Mobile Video Enhancement Manual Mode    */

#define TLG_NORMAL          (0) /*!<Normal option                           */
#define TLG_FROZEN          (1) /*!<Frozen option                           */ 

#define TLG_INVERT          (1) /*!<Invert option                           */
#define TLG_SWAP            (1) /*!<Swap option                             */

#define TLG_SLAVE           (0) /*!<Audio Mode I2S Slave option             */
#define TLG_MASTER          (1) /*!<Audio Mode I2S Master option            */
#define TLG_DAC             (2) /*!<Audio Mode Analog (DSM) option          */
#define TLG_UNDEFINED       (3) /*!<Undefined option                        */

#define TLG_FM_MONO         (0) /*!<Audio Stereo Mode Force Mono option     */
#define TLG_FM_STEREO       (1) /*!<Audio Stereo Mode Force Stereo option   */
#define TLG_FM_AUTO_DETECT  (2) /*!<Audio Stereo Mode Auto Detect option    */

#define TLG_MUTE            (0) /*!<Mute Audio                              */
#define TLG_ENABLE          (1) /*!<Enable Audio                            */

#define TLG_1KHZ            (1)
#define TLG_3KHZ            (2)
#define TLG_8KHZ            (3)

#define TLG_LEFT            (0) /*!<Audio Left Channel option               */
#define TLG_LEFT_RIGHT      (1) /*!<Audio Left+Right Channel option         */

#define TLG_CBCR_NORMAL     (0) /*!<CbCr Normal option                      */
#define TLG_CBCR_SWAP       (1) /*!<CbCr Swap option                        */

#define TLG_CHRLUM_NORMAL   (0) /*!<Chroma/Luma Normal option               */
#define TLG_CHRLUM_SWAP     (1) /*!<Chroma/Luma Swap option                 */

#define TLG_VID_LCD         (0) /*!<Video LCD output option                 */
#define TLG_VID_DENC        (1) /*!<video Digial output option              */

#define TLG_ACTIVE_HIGH     (0)
#define TLG_ACTIVE_LOW      (1)

#define TLG_FREEZE_MAX      (0)
#define TLG_FREEZE_MIN      (1)

#define TLG_I2C_MODE_SYNC   (0)     /*!< indicates i2c is Synchronous mode  */
#define TLG_I2C_MODE_ASYNC  (1)     /*!< indicates i2c is Asynchronous mode */

#define TLG_Y_MAX           (0x0f)  /*!< max Y setting for YUV scale        */
#define TLG_U_MAX           (0xff)  /*!< max U setting for YUV scale        */
#define TLG_V_MAX           (0xff)  /*!< max V setting for YUV scale        */

#define TLG_MVE_A_MAX       (0x3f)
#define TLG_MVE_MAX         (1)

#define TLG_MIN_AUD_GAIN        ( 0) /*!< min gain for TLG_MASTER,TLG_SLAVE */
#define TLG_MAX_AUD_GAIN_DIG    ( 9) /*!< max gain for TLG_MASTER,TLG_SLAVE */
#define TLG_MAX_AUD_GAIN_DAC    ( 5) /*!< max gain for TLG_DAC              */

#define TLG_MIN_DSM_VOLUME          (0) /*!<min volume for TLG_DAC                */
#define TLG_MIN_I2S_VOLUME          (0) /*!<min volume for TLG_MASTER, TLG_SLAVE  */
#define TLG_MAX_DSM_VOLUME          (10) /*!<max volume for TLG_DAC               */
#define TLG_MAX_I2S_VOLUME          (14) /*!<max volume for TLG_MASTER, TLG_SLAVE */
#define TLG_MAX_I2S_VOLUME_2_1      (12) /*!<max volume for TLG_MASTER, TLG_SLAVE for 2.1 chip */
#define TLG_MAX_DSM_VOLUME_2_1      (11) /*!<max volume for TLG_DAC for 2.1 chip */

#define TLG_AUD_RATE_NONE           (0) /*!< no i2s sample rate specified   */
#define TLG_AUD_RATE_26_367KHZ      (1) /*!< 26.367KHz i2s sample rate      */
#define TLG_AUD_RATE_32_143KHZ      (2) /*!< 32.143KHz i2s sample rate      */
#define TLG_AUD_RATE_44_118KHZ      (3) /*!< 44.118KHz i2s sample rate      */
#define TLG_AUD_RATE_48_214KHZ      (4) /*!< 48.214KHz i2s sample rate      */
#define TLG_AUD_RATE_52_734KHZ      (5) /*!< 52.734KHz i2s sample rate      */

#define TLG_LOCK_STATE_SYNTH        ((uint16) 1 << 12) /*!< synthesizer lock*/
#define TLG_LOCK_STATE_VSYNC        ((uint16) 1 <<  5) /*!< Video Vsync lock*/
#define TLG_LOCK_STATE_VIDEO        ((uint16) 1 <<  4) /*!< Video lock      */
#define TLG_LOCK_STATE_AUDIO        ((uint16) 1 <<  3) /*!< Audio lock      */
#define TLG_LOCK_STATE_CHROMA       ((uint16) 1 <<  2) /*!< Video Chroma lock*/
#define TLG_LOCK_STATE_LUMA         ((uint16) 1 <<  1) /*!< Video Luma lock */
#define TLG_LOCK_STATE_HSYNC        ((uint16) 1 <<  0) /*!< Video Hsync lock*/
#define TLG_LOCK_STATE_FMPLL        ((uint16) 1 <<  7) /*!< FM Pilot lock   */
#define TLG_LOCK_STATE_FMSNR        ((uint16) 1 << 10) /*!< FM SNR PLL lock */
#define TLG_LOCK_STATE_STEREO       ((uint16) 1 << 11) /*!< FM Stereo lock  */

#define TLG_BAND_CUTOFF_HZ      (1000000000)
#define TLG_UHF_MID_CUTOFF_HZ   (500000000) /*!< midpoint cutoff  for UHF   */
#define TLG_UHF_CUTOFF_HZ       (400000000) /*!< cutoff frequency for UHF   */
#define TLG_VHF2_CUTOFF_HZ      (150000000) /*!< cutoff frequency for VHF2  */
#define TLG_VHF3_CUTOFF_HZ      (150000000) /*!< cutoff frequency for VHF3  */
#define TLG_VHF1_MID_CUTOFF_HZ  ( 80000000) /*!< midpoint cutoff  for VHF1  */
#define TLG_VHF1_CUTOFF_HZ      (000000000) /*!< cutoff frequency for VHF1  */

#define TLG_REGION_WW           (0)         /*!< World Wide except US       */
#define TLG_REGION_US           (1)         /*!< United States Region       */


#define TLG_GPIO1                   (0x0001)
#define TLG_GPIO2                   (0x0002)
#define TLG_GPIO3                   (0x0004)
#define TLG_GPIO4                   (0x0008)
#define TLG_GPIO5                   (0x0010)
#define TLG_GPIO6                   (0x0020)

#define TLG_GPIO1_SET               (TLG_GPIO1)
#define TLG_GPIO2_SET               (TLG_GPIO2)
#define TLG_GPIO3_SET               (TLG_GPIO3)

#define TLG_GPIO1_CLR               (0)
#define TLG_GPIO2_CLR               (0)
#define TLG_GPIO3_CLR               (0)

#define TLG_AVG_FILTER_DEPTH        4    /* must be a power of 2 */
#define TLG_AVG_FILTER_IDX(__i)   ((__i)&(TLG_AVG_FILTER_DEPTH-1))
#define TLG_NUM_SSI_METRIC          8

#define TLG_VIDEO_OUTPUT_MODE_YCbCr (0)
#define TLG_VIDEO_OUTPUT_MODE_YUV   (1)
#define TLG_VIDEO_OUTPUT_MODE_RGB   (2)

#define TLG_AGC_NORMAL_MODE         (0)
#define TLG_AGC_ALWAYS_MODE         (1)
#define TLG_AGC_CUSTOM_MODE         (2)
#define TLG_AGC_SCAN_MODE           (3)
#define TLG_AGC_RESTORE_MODE        (4)

#define TLG_AGC_IN_VBI_TBL          (0)
#define TLG_AGC_ALWAYS_TBL          (1)
#define TLG_DEFAULT_TBL             (2)

#define TLG_NO_VDECIMATION          (0)
#define TLG_VDECIMATE_2ND_LINE      (1)
#define TLG_VDECIMATE_3RD_LINE      (2)
#define TLG_VDEVIMATE_4TH_LINE      (3)

/*
.   The macro TLG_NUM_DETECT_PAL_LOOP has been added to control the number of loops to check the
    chroma lock bit in PAL/SECAM hybrid mode.  If the number of loops is too high, the scanning time for
    PAL will be longer.  If it is too low, it might not be able to detect the SECAM standard correctly
    since the Chroma lock bit is not stable in PAL mode when SECAM channel is present.

*/
#define TLG_NUM_DETECT_PAL_LOOP             12
#define TLG_NUM_PAL_DETECT_THRESHOLD        8
#define TLG_PAL_QUICK_DETECT_THRESHOLD      15
#define TLG_PAL_AVG_DETECT_THRESHOLD        30
#define TLG_SECAM_DETECT_THRESHOLD          500
#define TLG_NUM_SECAM_DETECT_THRESHOLD      8
#define TLG_NUM_RETRY_PAL_LOOP              14


#define TLG_PAL_MAX_DETECT_THRESHOLD        25
#define TLG_SECAM_MIN_DETECT_THRESHOLD      170

/** Closed Caption definition */
/** CC field num                                                   */
typedef enum {
    FIELD_UNKNOW    = 0,
    FIELD_1                 = 1,
    FIELD_2,
    FIELD_MAX
} tlg_cc_field_num;


/** CC Output type                                                   */
typedef enum {
    TLG_CC_OUTPUT_FIFO            = 0,
    TLG_CC_OUTPUT_HSYNC,
    TLG_CC_OUTPUT_VSYNC,
    TLG_CC_OUTPUT_HVSYNC
} tlg_cc_output;

#ifdef TLG_FM_NEW_INTERFACE
#define TLG_FM_MIN_HZ               (76000000)          /* 76 MHZ  */
#define TLG_FM_MAX_HZ               (108000000)         /* 108 MHZ */
#define TLG_FM_MIN_STEP             (10000)             /* 10 KHZ  */
#define TLG_FM_MAX_STEP             (400000)            /* 400KHZ  */
#define TLG_FM_DEFAULT_STEP         (100000)            /* 100KHZ  */

#endif /* TLG_FM_NEW_INTERFACE */

typedef struct tlg_avg_filter_s {
    uint32      taps[TLG_AVG_FILTER_DEPTH];
    uint16      num_sample;
    uint16      cur_tap;

} tlg_avg_filter_t, *p_tlg_avg_filter_t;

/** Supported frequency bandwidth settings for TLG API.                     */

/** LNA Types                                                   */
typedef enum {
    TLG_LNA_NONE            = 0,
    TLG_LNA_UHF             = 3,
    TLG_LNA_VHF2            = 5,
    TLG_LNA_VHF1            = 6,
    TLG_LNA_BAND            = 7,
    TLG_LNA_VHF3            = 8
} tlg_lna_types;

/** Video Standards to configure TLG1120.                                   */
#define TLG_VSTD_HYBRID_BIT     (1 << 15)
typedef enum {
    TLG_VSTD_NTSC_M         = 0                       ,     /*!< NTSC-M                                 */
    TLG_VSTD_PAL_M          = 1                       ,     /*!< PAL-M                                  */
    TLG_VSTD_PAL_B          = 2                       ,     /*!< PAL-B                                  */
    TLG_VSTD_PAL_B1         = 3                       ,     /*!< PAL-B1                                 */
    TLG_VSTD_PAL_D          = 4                       ,     /*!< PAL-D                                  */
    TLG_VSTD_PAL_D1         = 5                       ,     /*!< PAL-D1                                 */
    TLG_VSTD_PAL_G          = 6                       ,     /*!< PAL-G                                  */
    TLG_VSTD_PAL_H          = 7                       ,     /*!< PAL-H                                  */
    TLG_VSTD_PAL_K          = 8                       ,     /*!< PAL-K                                  */
    TLG_VSTD_PAL_N          = 9                       ,     /*!< PAL-N                                  */
    TLG_VSTD_PAL_I          = 10                      ,     /*!< PAL-I                                  */
    TLG_VSTD_PAL_NC         = 11                      ,     /*!< PAL-NC                                 */
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
    TLG_VSTD_NONE               /*!< No Video Standard Specified            */
} tlg_vidstd_t;

#define TLG_VSYNC_LOCK      (BIT6)
#define TLG_VID_LOCK        (BIT4)
#define TLG_CHROMA_LOCK     (BIT2)
#define TLG_LUMA_LOCK       (BIT1)

/** Antenna Configurations supported.                                       */
typedef enum {
    TLG_ANTENNA_DEF = 0,        /*!< Default Antenna Behavior               */
    TLG_ANTENNA_HDPH,           /*!< Settings for the TLG_HDPH Antenna      */
    TLG_ANTENNA_MAX             /*!< Number of Antenna settings             */
} tlg_antenna_t;


/** States to supply to ASYNC calls, use TLG_STATE0 for first state.        */
typedef enum {
    TLG_STATE0=0,           /*!< Initial state for a TLG ASync State Machine*/
    TLG_STATE1,             /*!< State 1 for a TLG ASync State Machine      */
    TLG_STATE2,             /*!< State 2 for a TLG ASync State Machine      */
    TLG_STATE3,             /*!< State 3 for a TLG ASync State Machine      */
    TLG_STATE4,             /*!< State 4 for a TLG ASync State Machine      */
    TLG_STATE5,             /*!< State 5 for a TLG ASync State Machine      */
    TLG_STATE6,             /*!< State 6 for a TLG ASync State Machine      */
    TLG_STATE7,             /*!< State 6 for a TLG ASync State Machine      */
    TLG_STATE_FINAL         /*!< Final state for a TLG ASync State Machine  */
} TLG_state_t;

enum {
    MIX_MODE_VHF1 = 0x01, // 001 = vhf mixer selected
    MIX_MODE_UHF  = 0x02, // 010 = uhf mixer selected
    MIX_MODE_VHF3 = 0x04  // 100 = vhfbandIII  mixer selected
};

/*
    Create the table of register to reduce the code size
    Using for loop to write, stop when detect reg = 0xFFFF
*/
typedef struct _tlg_reg_pair_s
{

    uint16  reg     ;
    uint16  value   ;
} tlg_reg_pair, *p_tlg_reg_pair;

typedef struct _tlg_tone_notch_reg_t
{
    uint16  tone_notch_reg_1;
    uint16  tone_notch_reg_2;
    uint16  notch_status2_addr;
} tlg_tone_notch_reg_s;
/*
    TLG1100 only has 1 tone notch. 
    TLG1120 has 6 tone notches. 
        They are all in series 
            Each one has: 
                Enable bit: tone_en,  Default to 0 
                Bypass bit: tone_byp, Set it to: 
                    0: video 
                    1: Audio 
                Freq: tone_freq 
                Status: tone_avg_i[11:0] 
        Tone notch 1 is for Video 
        Can use tone notch 2 for audio. But remember to set the tone_byp to 1. 
*/
#define NUM_TONE_NOTCH  6

enum 
{
    MAIN_NOTCH = 0,
    AUDIO_ADJ_3_NOTCH  = 1,
    AUDIO_ADJ_1_NOTCH = 2,
    AUDIO_MAIN_NOTCH  = 3,
    AUDIO_ADJ_2_NOTCH = 4,
    AUDIO_ADJ_4_NOTCH = 5,
    AUDIO_FMSSI_NOTCH = 3,
    MAX_USED_NOTCH
};

/** Maximum number of states for a TLG ASync State Machine.                 */
#define TLG_STATE_MAX                (TLG_STATE_FINAL)

/** Size of the TLG State parm block.                                       */
#define TLG_STATE_PARMS_SIZE        (32)

/** Store internal state information across asynchronous calls.
 *
 * The asynchronous calls in TLG1120 API all have a common calling convention.
 * Each call has a "state" vector and "ms" millisecond delay.
 * Element 0 (e.g. state[0]) should be set to TLG_STATE0 on the initial call.
 * When state[0] == TLG_STATE_FINAL, the ASYNC call has completed. The rest
 * of the elements of the "state" vector are used by the internal state
 * machine of each call. Notice if an asynchronous procedure calls another
 * asynchronous procedure, they can assign a portion of the state vector to
 * the state vector for the subordinate call: e.g.
 *
 * Proc1ASYNC(tlg_statep state, int *ms)
 *
 * {
 *
 *      state[1] = TLG_STATE0;
 *
 *      Proc2ASYNC(&state[1], ms);
 * ...
 *
 * Proc1ASYNC has assigned a portion of its state vector (&state[1]) to be
 * the state vector for Proc2ASYNC. This type of nesting can occur multiple
 * times as long as the size of the state vector is not exceeded.
 */
typedef uint16 tlg_statep[TLG_STATE_PARMS_SIZE];

#define TLG_MIN_REG_ADDR                    (0x0000)
#define TLG_MAX_REG_ADDR                    (0x01b8)

#define TLG_InitCheckAddr                   (0x10053)

#ifdef TLG_USE_SHADOW_REGISTERS
/** CUSTOMER SUPPLIED array of registers to be used as a shadow or cache. */
extern uint16 _TLG_shadow_reg[];
extern uint8  _TLG_shadow_reg_use[];
#endif /* TLG_SHADOW_REGISTER_SIZE */

//#define tlg_win32	

#ifdef TLG_DO_ERROR_CHECKING

/** Perform NULL pointer check and return TLG_ERR_PARAM if it fails.
 */
#define TLG_NULLPTR_CHECK(ptr) if (!(ptr)) {TLG_REG_LOG((b,"Bad Ptr\n"));return(TLG_ERR_PARAM);}

/** Perform bounds check and return TLG_ERR_PARAM if it fails.
 */
#define TLG_BOUNDS_CHECK(var, bound) if ((var) > (bound)) {TLG_REG_LOG((b,"bad bounds check %d > %d\n", (var), (bound)));return(TLG_ERR_PARAM);}
#else  /* TLG_DO_ERROR_CHECKING */
#define TLG_NULLPTR_CHECK(ptr)
#define TLG_BOUNDS_CHECK(var, bound)
#endif /* TLG_DO_ERROR_CHECKING */


#ifdef TLG_DO_REG_LOGGING
#include <stdio.h>
/** CUSTOMER SUPPLIED informative logging facility.
 *
 * @fn void TLG_WriteLog(char *str);
 *
 * If TLG_DO_REG_LOGGING is #defined, informative logging will be turned on.
 * The informative logging depends on a function TLG_WriteLog() which must be
 * created in a separate source file and linked into the final application. If
 * TLG_DO_REG_LOGGING is not #defined, this function is not needed.
 *
 * @param str   Pointer to a character string to be printed to the customer's
 *              logging facility.
 */
TLGDLL_API void TLG_WriteLog(char *str);

#define TLG_REG_LOG(a) do {char b[200];sprintf a ;TLG_WriteLog(b);} while(0)

#else  /* TLG_DO_REG_LOGGING */
#define TLG_REG_LOG(a)	// do {char b[200];sprintf a ;kal_prompt_trace(MOD_WAP,b);} while(0)
#endif /* TLG_DO_REG_LOGGING */

#ifdef DO_TRACE_LOGGIN

#define TLG_TRACE_LOG(a) do {char b[200];sprintf a ;TLG_WriteLog(b);} while(0)

#else /* ! DO_TRACE_LOGGING */

#define TLG_TRACE_LOG(a)

#endif /* DO_TRACE_LOGGING */

#ifdef TLG_EMBEDDED_TIMERS
/** CUSTOMER SUPPLIED millisecond delay timer.
 *
 * @fn TLG_Delay(uint16 milliseconds)
 *
 * This procedure waits the given number of milliseconds before returning to
 * the caller. This function must be created in a separate source file and
 * linked into the final application.
 * Currently TLG_SetTVChnFreq() TLG_SetFMChnFreq(), TLG_ScanTVChn(), and
 * TLG_ScanFMChn() have embedded calles to TLG_Delay(). If TLG_EMBEDDED_TIMERS
 * is not defined then these procedures will return TLG_ERR_NOT_SUPPORTED and
 * the Asynchronous versions of the procedures must be used.
 *
 * @param milliseconds  The number of milliseconds to wait until return.
 */
TLGDLL_API void TLG_Delay(uint16 milliseconds);
#else  /* TLG_EMBEDDED_TIMERS */
#define TLG_Delay(a)
#endif /* TLG_EMBEDDED_TIMERS */

/*
 * These defines support the runtime chip version checking feature.
 */
#ifdef TLG_SUPPORT_RUNTIME_CHECK
extern int g_tlg_chip_ver;
#define TLG_CHIP_VERS_2_1_BEGIN			if (g_tlg_chip_ver >= TLG1120_VERS_2_1) {
#define TLG_CHIP_VERS_1_2_ELSE			} else if (g_tlg_chip_ver >= TLG1120_VERS_1_2) {
#define TLG_CHIP_VERS_1_2_BEGIN         if (g_tlg_chip_ver >= TLG1120_VERS_1_2) {
#define TLG_CHIP_VERS_1_1_ELSE          } else {
#define TLG_CHIP_VERS_ELSE              } else {
#define TLG_CHIP_VERS_END               }
#else /* TLG_SUPPORT_RUNTIME_CHECK */
#define TLG_CHIP_VERS_2_1_BEGIN
#define TLG_CHIP_VERS_1_2_ELSE
#define TLG_CHIP_VERS_1_2_BEGIN
#define TLG_CHIP_VERS_ELSE
#define TLG_CHIP_VERS_1_1_ELSE
#define TLG_CHIP_VERS_END

#endif /* TLG_SUPPORT_RUNTIME_CHECK */


/** Get the current product version information for the TLG1120 Portable C Library.
 *
 * This procedure returns the version number for the TLG1120 Portable C
 * Library. The version is in the form major.minor.patch.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param major             The major version number.
 * @param minor             The minor version number.
 * @param patch             The current patch level.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_GetVersion(uint32 base_addr, uint16 *major, uint16 *minor, uint16 *patch);

/** Get the internal build number for the TLG1120 Portable C Library.
 *
 * This procedure returns the build number for the TLG1120 Portable C
 * Library. When combined with the version finformation in TLG_GetVersion,
 * the full version number is in the form major.minor.patch.build.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param build             The internal build number.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_GetBuildNumber(uint32 base_addr, uint16 *build);

/** Get the current version of the TLG1120 chip.
 *
 * This procedure returns the version number for the TLG1120 FE Chip.
 * The version is in the form major.minor.patch.
 *
 * @param base_addr         I2c address for TLG1200 chip.
 * @param version           Returns the chip revision identifier. Valid values:
 *                          - TLG_NO_VERS
 *                          - TLG1120_VERS_1_1
 *                          - TLG1120_VERS_1_2
 *                          - TLG1120_VERS_2_1
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_GetChipVersion(uint32 base_addr, uint32 *version);

/** Perform a hard reset of TLG1120.
 *
 * This procedure performs a hard reset of TLG1120. It should be run if hardware
 * reset (RSTN, Pin-59) fails. This procedure should be called after power-on and
 * before TLG_Init().
 *
 * @param base_addr     I2c address for TLG1120 chip
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    Chip version is not supported.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_HardReset(uint32 base_addr);

/** Perform a synchronous initialization of the TLG1120.
 *
 * This procedure performs initial powerup settings for the TLG1120. See
 * TLG_InitASYNC() for a complete description of powerup procedures. This call
 * embeds the asynchronous nature of TLG_InitASYNC() for those implementations
 * that can handle synchronous calls. NOTE, THIS PROCEDURE RELIES ON
 * TLG_EMBEDDED_TIMERS BEING DEFINED. IF THIS IS NOT DEFINED, THEN THIS
 * PROCEDURE WILL RETURN TLG_ERR_NOT_SUPPORTED AND NO INITIALIZATION WILL BE
 * PERFORMED.
 *
 * @param base_addr     I2c address for TLG1120 chip
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_Init(uint32 base_addr);

/** Initialize TLG1120 Asynchronously.
 *
 * This procedure performs initial power-up settings for the TLG1120. Note that
 * for now there are additional register settings in the tlg1120.ini file that
 * comes with the TLG API source. These register settings should be done prior
 * to the call to TLG_InitASYNC(). Also there are some delays required to the
 * startup procedure in TLG_Init() so it is implemented as a state machine. The
 * simple example of how to perform initialization is:
 *
 * Write custom procedure to set registers defined in tlg1120.ini using
 * TLG_WriteReg().
 *
 * tlg_statep state;
 *
 * int delay;
 *
 * state[0] = TLG_STATE0;
 *
 * while (state[0] != TLG_STATE_FINAL) {
 *
 *      TLG_InitASYNC(base_address, state, &delay);
 *
 *      if (delay != 0) TLG_Delay(delay);
 *
 * }
 *
 * For implementations that can't put delays in their init code, the calls may
 * be distributed over an event messaging or timer callback service.
 *
 * @param base_addr     I2c address for TLG1120 chip
 * @param state         State vector. By convention state[0] holds the state
 *                      variable and Initially this should be set to
 *                      TLG_STATE0. After that it should treated
 *                      as READ ONLY for the remainder of the call.
 *                      When state[0] == TLG_STATE_FINAL initialization is
 *                      complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms            Returns the number of minimum number of milliseconds
 *                      before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_InitASYNC(uint32 base_addr, tlg_statep state, int *ms);

/** Get the current video YUV scaling values.
 *
 * Get the current Y, U, V scaling factors configured into the TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param y             Pointer to parameter to fill with the current y scale.
 *                      Valid range: 0-15.
 * @param u             Pointer to parameter to fill with the current u scale.
 *                      Valid range: 0-255.
 * @param v             Pointer to parameter to fill with the current v scale.
 *                      Valid range: 0-255.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetYUVScale(uint32 base_addr, uint16 *y, uint16 *u, uint16 *v);


/** Set the video YUV scaling values.
 *
 * Set the Y, U, and V scaling values to be used by the TLG1120 video output.
 * The settings will take effect immediately.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param y             The new y scale. Valid range: 0-15.
 * @param u             The new u scale. Valid range: 0-255.
 * @param v             The new v scale. Valid range: 0-255.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetYUVScale(uint32 base_addr, uint16 y,uint16 u, uint16 v);

/** Get the current state of the Force Black/White mode.
 *
 * This procedure returns whether the TLG1120 is in "force black and white"
 * mode. Note: this mode doesn't effect test pattern.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current state.
 *                      Possible values are:
 *                      - TLG_OFF
 *                      - TLG_ON
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetBWVideoMode(uint32 base_addr, uint16 *val);

/** Turn off/on the Force Black/White mode.
 *
 * This procedure enables/disables the "force black and white" mode. This is
 * mainly for testing purposes only.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set state to. Possible values:
 *                      - TLG_OFF
 *                      - TLG_ON
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetBWVideoMode(uint32 base_addr, uint16 val);

/** Get the current state of the Generate Video Test Pattern mode.
 *
 * This procedure returnes whether the TLG1120 is currently in the "display
 * test pattern" mode. This mode is mainly for testing purposes only.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current state.
 *                      Possible values are:
 *                      - TLG_OFF
 *                      - TLG_ON
 *                      - TLG_COLOR_BAR
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetVideoTestPatternState(uint32 base_addr, uint16 *val);

/** Turn off/on the Generate Video Test Pattern mode.
 *
 * Enable/disable the "display test pattern" mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set state to. Possible values:
 *                      - TLG_OFF
 *                      - TLG_ON
 *                      - TLG_COLOR_BAR
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetVideoTestPatternState(uint32 base_addr, uint16 val);

/** Get the current state of the 601 Video clock.
 *
 * This procedure gets the current state of the 601 clock mode. Note that by
 * default the 601 clock is in TLG_INVERT mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current state.
 *                      Possible values are:
 *                      - TLG_NORMAL
 *                      - TLG_INVERT
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Get601VideoOutClock(uint32 base_addr, uint16 *val);

/** Set the state of the 601 Video clock.
 *
 * This procedure sets the behaviour of the 601 clock. Note that by default
 * the 601 clock is in TLS_INVERT mode. This setting will be enabled
 * immediately after the call.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set state to. Possible values:
 *                      - TLG_NORMAL
 *                      - TLG_INVERT - default value!
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set601VideoOutClock(uint32 base_addr, uint16 val);

/** Get the current order of the Chroma/Luma.
 *
 * This procedure returns the current order of the Chroma/Luma output.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current order.
 *                      Possible values are:
 *                      - TLG_CHRLUM_NORMAL
 *                      - TLG_CHRLUM_SWAP
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetChromaLumaOrder(uint32 base_addr, uint16 *val);

/** Set the order of the Chroma/Luma.
 *
 * This procedure sets the order for the Chroma/Luma output. The setting will
 * be enabled immediatley after the call.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set state to. Possible values:
 *                      - TLG_CHRLUM_NORMAL
 *                      - TLG_CHRLUM_SWAP
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetChromaLumaOrder(uint32 base_addr, uint16 val);

/** Get the current order of the Cb/Cr.
 *
 * This procedure returns the currently configured Cb/Cr order.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current order.
 *                      Possible values are:
 *                      - TLG_CBCR_NORMAL
 *                      - TLG_CBCR_SWAP
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetCbCrOrder(uint32 base_addr, uint16 *val);

/** Set the order of the Cb/Cr.
 *
 * This procedure sets the Cb/Cr order. The setting is enabled immediately
 * after the call is made.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set state to. Possible values:
 *                      - TLG_CBCR_NORMAL
 *                      - TLG_CBCR_SWAP
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetCbCrOrder(uint32 base_addr, uint16 val);

/** Get the current audio channel configuration.
 *
 * This procedure returns the currently configured audio channel configuration.
 * Note this information only has meaning with the Audio mode is set to TLG_DAC.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the configuration.
 *                      Possible values are:
 *                      - TLG_LEFT
 *                      - TLG_LEFT_RIGHT
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioChan(uint32 base_addr, uint16 *val);

/** Set the audio channel configuration.
 *
 * This procedure sets the number of audio channels when the Audio Mode is set
 * to TLG_DAC.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set configuration to. Possible values:
 *                      - TLG_LEFT
 *                      - TLG_LEFT_RIGHT
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioChan(uint32 base_addr, uint16 val);

/** Determine whether Audio is currently enabled/muted.
 *
 * This procedure returns whether the TLG1120 audio output is currently enabled
 * or not. This function is valid for all Audio Modes.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the state.
 *                      Possible values are:
 *                      - TLG_ENABLE
 *                      - TLG_MUTE
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioEnable(uint32 base_addr, uint16 *val);

/** Enable/Mute Audio.
 *
 * This procedure enables/disables the audio output. The setting takes place
 * immediately. This function is valid for all Audio Modes.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set state to. Possible values:
 *                      - TLG_ENABLE
 *                      - TLG_MUTE
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioEnable(uint32 base_addr, uint16 val);

/** Get the current Audio Mode.
 *
 * This procedure gets the Audio Mode that the TLG1120 is currently set to.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current mode.
 *                      Possible values are:
 *                      - TLG_SLAVE  (I2S audio out Slave Mode)
 *                      - TLG_MASTER (I2S audio out Master Mode)
 *                      - TLG_DAC    (Analog (dsm) audio out Mode)
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioMode(uint32 base_addr, uint16 *val);

/** Set the Audio Mode.
 *
 * This procedure sets the TLG1120 into the specified mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set mode to. Possible values:
 *                      - TLG_SLAVE
 *                      - TLG_MASTER
 *                      - TLG_DAC
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioMode(uint32 base_addr, uint16 val);

/** Get the currently set I2S Audio Sample Rate.
 *
 * This procedure retrieves the currently configured Audio Sample Rate for
 * I2S audio. Note this value only applies to TLG_MASTER mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param rate          The rate is returned here. Possible values are:
 *                      TLG_AUD_RATE_NONE
 *                      TLG_AUD_RATE_26_367KHZ
 *                      TLG_AUD_RATE_32_143KHZ
 *                      TLG_AUD_RATE_44_118KHZ
 *                      TLG_AUD_RATE_48_214KHZ
 *                      TLG_AUD_RATE_52_734KHZ
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioSampleRate(uint32 base_addr, uint16 *rate);

/** Set the I2S Audio Sample Rate.
 *
 * This procedure sets the specified Audio Sample Rate for
 * I2S audio. Note this value only applies to TLG_MASTER mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param rate          The rate is returned here. Possible values are:
 *                      TLG_AUD_RATE_26_367KHZ
 *                      TLG_AUD_RATE_32_143KHZ
 *                      TLG_AUD_RATE_44_118KHZ
 *                      TLG_AUD_RATE_48_214KHZ
 *                      TLG_AUD_RATE_52_734KHZ
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioSampleRate(uint32 base_addr, uint16 rate);


/** Get the current audio output gain setting.
 *
 * This procedure retrieves the current audio gain settings. 
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param aud_mode      Mode set by TLG_SetAudioMode(). Possible values:
 *                      - TLG_SLAVE
 *                      - TLG_MASTER
 *                      - TLG_DAC
 * @param val           Pointer to parameter to fill with the current gain.
 *                      Valid values are as follows:
 *                                    Silicon Rev
 *                      aud_mode   | 1.1/1.2 | 2.1
 *                      -----------+---------+-----
 *                      TLG_SLAVE  |   0-9   | 0-12
 *                      TLG_MASTER |   0-9   | 0-12
 *                      TLG_DAC    |   0-5   | 0-7 
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioGain(uint32 base_addr, uint16 aud_mode, uint16 *val);

/** Set the audio output gain.
 *
 * This procedure sets the audio gain on the TLG1120. Note that the audio 
 * gain is not intended as a general purpose volume control. It is assumed
 * that a customer's product will have a separate, more fine-tuned volume 
 * control feature. It is recommended that customers find a suitable gain 
 * level to use with their product and keep the gain at that setting.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param aud_mode      Mode set by TLG_SetAudioMode(). Possible values:
 *                      - TLG_SLAVE
 *                      - TLG_MASTER
 *                      - TLG_DAC
 * @param val           Pointer to parameter to fill with the current gain.
 *                      Valid values are as follows:
 *                                    Silicon Rev
 *                      aud_mode   | 1.1/1.2 | 2.1
 *                      -----------+---------+-----
 *                      TLG_SLAVE  |   0-9   | 0-12
 *                      TLG_MASTER |   0-9   | 0-12
 *                      TLG_DAC    |   0-5   | 0-7 
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioGain(uint32 base_addr, uint16 aud_mode, uint16 val);

/** Get the current audio mode and gain settings.
 *
 * This procedure gets the Audio Mode and Gain that the TLG1120 is currently set to.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param aud_mode      Pointer to parameter to fill with the current mode.
 *                      Possible values are:
 *                      - TLG_SLAVE
 *                      - TLG_MASTER
 *                      - TLG_DAC
 * @param gain          Pointer to parameter to fill with the gain value.
 *                      Possible values are: 0-9 for TLG_MASTER, TLG_SLAVE
 *                                           0-5 for TLG_DAC
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioOutput(uint32 base_addr, uint16 *aud_mode, uint16 *gain);

/** Set audio mode and gain settings.
 *
 * This procedure sets the Audio Mode and Gain of the TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param aud_mode      Audio mode selection.
 *                      Possible values are:
 *                      - TLG_SLAVE
 *                      - TLG_MASTER
 *                      - TLG_DAC
 * @param gain          Gain values.
 *                      Possible values are: 0-9 for TLG_MASTER, TLG_SLAVE
 *                                           0-5 for TLG_DAC
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioOutput(uint32 base_addr, uint16 aud_mode, uint16 gain);

/** Get the current I2S data configuration parameter.
 *
 * This procedure gets I2S data configuration value that the TLG1120 is currently set to.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @param R             Pointer to parameter to fill with the data configuration.
 *                      Possible values are: 1-256
 *
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetI2SAudioData(uint32 base_addr, uint16 *R);

/** Set the current I2S data configuration parameter.
 *
 * This procedure sets I2S data configuration parameter of the TLG1120; the R value
 * should be correctly set for both I2S master and slave modes.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @param R             I2S data configuration value.
 *                      Possible values are: 1-256
 *
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetI2SAudioData(uint32 base_addr, uint16 R);

/** Get the current I2S clock configuration parameters.
 *
 * This procedure gets I2S clock configuration parameters that the TLG1120 is
 * currently set to.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @param M             Pointer to parameter to fill with the clock configuration value M.
 *                      Possible values are: 1-32
 * @param S             Pointer to parameter to fill with the clock configuration value S.
 *                      Possible values are: 1-64
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetI2SMasterAudioClock(uint32 base_addr, uint16 *M, uint16 *S);

/** Set the current I2S clock configuration parameters.
 *
 * This procedure sets I2S clock configuration parameters of the TLG1120. The M and S values
 * should be set correctly in I2S master mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @param M             I2S clock configuration, M value.
 *                      Possible values are: 1-32
 * @param S             I2S clock configuration, S value.
 *                      Possible values are: 1-64
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetI2SMasterAudioClock(uint32 base_addr, uint16 M, uint16 S);

/** Get the current state of the Audio Test Tone Mode.
 *
 * This procedure returns whether or not Audio Test Tone mode is enabled. Note
 * Audio Test Tone mode must be enabled before test tones may be generated with
 * TLG_SetAudioTestToneL() or TLG_SetAudioTestToneR().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current mode.
 *                      Possible values are:
 *                      - TLG_OFF
 *                      - TLG_ON
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioTestToneMode(uint32 base_addr, uint16 *val);

/** Enables/Disables Audio Test Tone mode.
 *
 * This procedure is used to turn on/off Audio Test Tone mode. Note Audio Test
 * Tone mode must be enabled before test tones may be generated with
 * TLG_SetAudioTestToneL() or TLG_SetAudioTestToneR().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           The new mode. Possible values are:
 *                      - TLG_OFF
 *                      - TLG_ON
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioTestToneMode(uint32 base_addr, uint16 val);

/** Get the current state of the I2S Debug Waveform.
 *
 * This procedure returns whether or not the I2S Debug Waveform is enabled. Note
 * Audio Test Tone mode must be enabled before the debug waveform may be generated.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current mode.
 *                      Possible values are:
 *                      - TLG_OFF
 *                      - TLG_ON
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetI2SDebugWaveform(uint32 base_addr, uint16 *val);

/** Enables/Disables the I2S Debug Waveform.
 *
 * This procedure is used to turn on/off the I2S Debug Waveform. Note Audio Test
 * Tone mode must be enabled before debug waveform may be generated.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           The new mode. Possible values are:
 *                      - TLG_OFF
 *                      - TLG_ON
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetI2SDebugWaveform(uint32 base_addr, uint16 val);

/** Get the current audio test tone value for left channel.
 *
 * This procedure returns the currently configured Left test tone value. Note
 * that in order to generate test tones you must first call
 * TLG_SetAudioTestToneMode(base_addr, TLG_ON). Test tones are affected by
 * TLG_SetAudioEnable() and TLG_SetAudioGain().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current mode.
 *                      Possible values are:
 *                      - TLG_OFF
 *                      - TLG_1KHZ
 *                      - TLG_3KHZ
 *                      - TLG_8KHZ
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioTestToneL(uint32 base_addr, uint16 *val);

/** Turn on/off audio test tone for left channel.
 *
 * This procedure sets the Left test tone value. Note
 * that in order to generate test tones you must first call
 * TLG_SetAudioTestToneMode(base_addr, TLG_ON). Test tones are affected by
 * TLG_SetAudioEnable() and TLG_SetAudioGain().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set mode to. Possible values:
 *                      - TLG_OFF
 *                      - TLG_1KHZ
 *                      - TLG_3KHZ
 *                      - TLG_8KHZ
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioTestToneL(uint32 base_addr, uint16 val);

/** Get the current audio test tone value for right channel.
 *
 * This procedure returns the currently configured right test tone value. Note
 * that in order to generate test tones you must first call
 * TLG_SetAudioTestToneMode(base_addr, TLG_ON). Test tones are affected by
 * TLG_SetAudioEnable() and TLG_SetAudioGain().
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Pointer to parameter to fill with the current mode.
 *                      Possible values are:
 *                      - TLG_OFF
 *                      - TLG_1KHZ
 *                      - TLG_3KHZ
 *                      - TLG_8KHZ
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioTestToneR(uint32 base_addr, uint16 *val);

/** Turn on/off audio test tone for right channel.
 *
 * This procedure sets the right test tone value. Note
 * that in order to generate test tones you must first call
 * TLG_SetAudioTestToneMode(base_addr, TLG_ON). Test tones are affected by
 * TLG_SetAudioEnable() and TLG_SetAudioGain().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Value to set mode to. Possible values:
 *                      - TLG_OFF
 *                      - TLG_1KHZ
 *                      - TLG_3KHZ
 *                      - TLG_8KHZ
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioTestToneR(uint32 base_addr, uint16 val);


TLGDLL_API int TLG_GetInitFlag(unsigned i2c_addr, unsigned short on);

/** Clear the register shadow used bits.
 *
 * This function clears all the shadow register used flags. This will force
 * all TLG_ReadReg() calls to retrieve regsiters from memory again.
 *
 * NOTE: This function is for diagnostic purposes only and should never need
 * to be called by production code. if TLG_USE_SHADOW_REGISTERS is not defined
 * then this call does nothing.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 */
TLGDLL_API int TLG_ClearRegShadow(uint32 base_addr);


/** Read a register value, possibly through the shadowed registers.
 *
 * Read a register value. If TLG_USE_SHADOW_REGISTERS is defined then this
 * routine will return the shadowed register value instead of retrieving it
 * from the chip. If TLG_USE_SHADOW_REGISTERS is not defined then this routine
 * will access the register directly from the chip.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Register Address to retrieve.
 * @param val           Returned register value.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ReadReg(uint32 base_addr, uint32 reg_addr, uint16 *val);

/** Read a register value from the TLG1120.
 *
 * This procedure will always retrieve the register value direclty from the
 * TLG1120 even if TLG_USE_SHADOW_REGISTERS is defined. If
 * TLG_USE_SHADOW_REGISTERS is defined, the shadowed value will be updated with
 * the new value from the chip.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Register Address to retrieve.
 * @param val           Returned register value.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ReadRegNS(uint32 base_addr, uint32 reg_addr, uint16 *val);

/** Read a register value, possibly through the shadowed registers.
 *
 * Read a register value. If TLG_USE_SHADOW_REGISTERS is defined then this
 * routine will return the shadowed register value instead of retrieving it
 * from the chip. If TLG_USE_SHADOW_REGISTERS is not defined then this routine
 * will access the register directly from the chip.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Register Address to retrieve.
 * @param val           Returned register value.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ReadRegNoLog(uint32 base_addr, uint32 reg_addr, uint16 *val);

/** Read a register value from the TLG1120 without loging.
 *
 * This procedure will always retrieve the register value direclty from the
 * TLG1120 even if TLG_USE_SHADOW_REGISTERS is defined. If
 * TLG_USE_SHADOW_REGISTERS is defined, the shadowed value will be updated with
 * the new value from the chip without logging
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Register Address to retrieve.
 * @param val           Returned register value.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ReadRegNS_NoLog(uint32 base_addr, uint32 reg_addr, uint16 *val);

/** Write a register value to the TLG1120.
 *
 * This procedure writes the specified register value at the specified
 * register address on the TLG1120. If TLG_USE_SHADOW_REGISTERS is defined it
 * will also update the shadowed registers with the written value.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Address of register to write.
 * @param val           Register value to write.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_WriteReg(uint32 base_addr, uint32 reg_addr, uint16 val);


/** Write a register value to the TLG1120 with no log.
 *
 * This procedure writes the specified register value at the specified
 * register address on the TLG1120. If TLG_USE_SHADOW_REGISTERS is defined it
 * will also update the shadowed registers with the written value with no log.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Address of register to write.
 * @param val           Register value to write.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_WriteRegNoLog(uint32 base_addr, uint32 reg_addr, uint16 val);

/** Write a contiguous block of registers on the TLG1120.
 *
 * This procedure writes a block of registers starting at the specified
 * register address on the TLG1120. If TLG_USE_SHADOW_REGISTERS is defined it
 * will also update the shadowed registers with the written values.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param start_addr    Starting address of registers to write.
 * @param len           Number of registers to write.
 * @param val           Address of block of register values to write.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_BurstWrite(uint32 base_addr, uint32 start_addr, int len, uint16 *val);

/** Read a contiguous block of registers from the TLG1120.
 *
 * This procedure reads a block of registers starting at the specified
 * register address on the TLG1120. It will always read the values directly
 * from the chip even if TLG_USE_SHADOW_REGISTERS is defined. If
 * TLG_USE_SHADOW_REGISTERS is defined, it will update the shadowed registers
 * with the updated values. WARNING! NO CHECK IS MADE TO INSURE THAT THE VAL
 * ARRAY IS LARGE ENOUGH TO HOLD THE REQUESTED REGISTERS!
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param start_addr    Starting address of registers to write.
 * @param len           Number of registers to read.
 * @param val           Address of block of register values to write.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_BurstRead(uint32 base_addr, uint32 start_addr, int len, uint16 *val);

/** Get the External HSYNC configuration values.
 *
 * This procedure retrieves the external HSYNC configuration values currently
 * set in the TLG1120. The TLG1120 supports one HSYNC assertion region and two
 * HSYNC de-assertion regions.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param assert_start      The starting location to force assert of HSYNC.
 * @param assert_end        The location where to stop asserting HSYNC.
 * @param deassert1_start   The starting line to force de-assert of HSYNC.
 * @param deassert1_end     The line where to stop forced de-asserting HSYNC.
 * @param deassert2_start   The starting line to force de-assert of HSYNC.
 * @param deassert2_end     The line where to stop forced de-asserting HSYNC.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetExtHSync(uint32 base_addr,
                    uint16 *assert_start,        uint16 *assert_end,
                    uint16 *deassert1_start,     uint16 *deassert1_end,
                    uint16 *deassert2_start,     uint16 *deassert2_end);

/** Set the External HSYNC configuration values.
 *
 * This procedure sets the external HSYNC configuration values in the TLG1120.
 * The TLG1120 supports one HSYNC assertion region. It also supports two
 * vertical scan line regions where HSYNC is prevented from asserting. To stop
 * external HSYNCs from being generated altogether, set all values to 0.
 * The new settings will be used immediately by the TLG1120.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param assert_start      The starting location to force assert of HSYNC.
 * @param assert_end        The location where to stop asserting HSYNC.
 * @param deassert1_start   The starting line to force de-assert of HSYNC.
 * @param deassert1_end     The line where to stop forced de-asserting HSYNC.
 * @param deassert2_start   The starting line to force de-assert of HSYNC.
 * @param deassert2_end     The line where to stop forced de-asserting HSYNC.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetExtHSync(uint32 base_addr,
                    uint16 assert_start,        uint16 assert_end,
                    uint16 deassert1_start,     uint16 deassert1_end,
                    uint16 deassert2_start,     uint16 deassert2_end);

/** Get the External VSYNC configuration values.
 *
 * This procedure retrieves the external VSYNC configuration values currently
 * set in the TLG1120. The TLG1120 supports 3 VSYNC assertion regions.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param assert1_start     The starting line to force assert of VSYNC.
 * @param assert1_end       The line where to stop asserting VSYNC.
 * @param assert2_start     The starting line to force assert of VSYNC.
 * @param assert2_end       The line where to stop asserting VSYNC.
 * @param assert3_start     The starting line to force assert of VSYNC.
 * @param assert3_end       The line where to stop asserting VSYNC.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetExtVSync(uint32 base_addr,
                    uint16 *assert1_start,       uint16 *assert1_end,
                    uint16 *assert2_start,       uint16 *assert2_end,
                    uint16 *assert3_start,       uint16 *assert3_end);

/** Set the External VSYNC configuration values.
 *
 * This procedure sets the external VSYNC configuration values in the TLG1120.
 * The TLG1120 supports 3 VSYNC assertion regions. To stop VSYNCs from being
 * generated at all, set all values to 0. The new settings will be used
 * immediately by the TLG1120.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param assert1_start     The starting line to force assert of VSYNC.
 * @param assert1_end       The line where to stop asserting VSYNC.
 * @param assert2_start     The starting line to force assert of VSYNC.
 * @param assert2_end       The line where to stop asserting VSYNC.
 * @param assert3_start     The starting line to force assert of VSYNC.
 * @param assert3_end       The line where to stop asserting VSYNC.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetExtVSync(uint32 base_addr,
                    uint16 assert1_start,       uint16 assert1_end,
                    uint16 assert2_start,       uint16 assert2_end,
                    uint16 assert3_start,       uint16 assert3_end);

/** Get the HSync Active polarity.
 *
 * This procedure returns the currently configured polarity of the "active
 * region" on the external Horizontal Sync (HSYNC) pin. Valid values are:
 * TLG_ACTIVE_HIGH, TLG_ACTIVE_LOW. The "active" region is the region defined
 * by the assert_start/assert_end fields set by TLG_SetExtHSync().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param polarity      The polarity of the "active" region. Values are:
 *                      - TLG_ACTIVE_HIGH
 *                      - TLG_ACTIVE_LOW
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_SetExtHSync, TLG_GetExtHSync
 */
TLGDLL_API int TLG_GetHSyncPolarity(uint32 base_addr, uint16 *polarity);

/** Set the HSync Active polarity.
 *
 * This procedure sets the polarity of the "active region" on the external
 * Horizontal Sync pin. Valid values are: TLG_ACTIVE_HIGH, TLG_ACTIVE_LOW.
 * The "active region" is the region defined by the assert_start and
 * assert_end fields set by TLG_SetExtHSync().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param polarity      The polarity of the "active" region. Values are:
 *                      - TLG_ACTIVE_HIGH
 *                      - TLG_ACTIVE_LOW
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_SetExtHSync, TLG_GetExtHSync
 */
TLGDLL_API int TLG_SetHSyncPolarity(uint32 base_addr, uint16 polarity);

/** Get the VSync Active polarity.
 *
 * This procedure returns the currently configured polarity of "active region"
 * on the external Vertical Sync (VSYNC) pin. Valid values are:
 * TLG_ACTIVE_HIGH, TLG_ACTIVE_LOW.  The "active region" is the region defined
 * by the assert1_start/assert1_end, assert2_start/assert2_end, and
 * assert3_start/assert3_end fields set by TLG_SetExtVSync().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param polarity      The polarity of the "active" region. Values are:
 *                      - TLG_ACTIVE_HIGH
 *                      - TLG_ACTIVE_LOW
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_SetExtVSync, TLG_GetExtVSync
 */
TLGDLL_API int TLG_GetVSyncPolarity(uint32 base_addr, uint16 *polarity);

/** Set the VSync Active polarity.
 *
 * This procedure sets the polarity of "active region"
 * on the external Vertical Sync (VSYNC) pin. Valid values are:
 * TLG_ACTIVE_HIGH, TLG_ACTIVE_LOW.  The "active region" is the region defined
 * by the assert1_start/assert1_end, assert2_start/assert2_end, and
 * assert3_start/assert3_end fields set by TLG_SetExtVSync().
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param polarity      The polarity of the "active" region. Values are:
 *                      - TLG_ACTIVE_HIGH
 *                      - TLG_ACTIVE_LOW
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_SetExtVSync, TLG_GetExtVSync
 */
TLGDLL_API int TLG_SetVSyncPolarity(uint32 base_addr, uint16 polarity);

/** Set the squelch even frames feature.
 *
 * This procedure enables/disables the squelch even frames feature.
 * Valid values are: TLG_ON, TLG_OFF.
 * When this feature is turned on, external vsync will be set active
 * for all even frames.  This will essentially half the frame rate
 * of the incoming signal.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param squelch       Turn squelch on or off. Values are:
 *                      - TLG_ON
 *                      - TLG_OFF
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SquelchEvenFrames(uint32 base_addr, uint16 squelch);

/** Get the current Horizontal Decimation setting.
 *
 * This procedure retrieves the current horizontal decimation setting from the
 * TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param decimate      Value to set the decimator to. Valid values are:
 *                      TLG_OFF : decimate by 1
 *                      TLG_ON  : decimate by 2
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetHDecimate(uint32 base_addr, uint16 *decimate);

/** Enable/Disable Horizontal Decimation.
 *
 * This procedure controls the horizontal decimation engine in the TLG1120.
 * Currently only decimation by 1 (default) and decimation by 2 are supported.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param decimate      Value to set the decimator to. Valid values are:
 *                      TLG_OFF : decimate by 1
 *                      TLG_ON  : decimate by 2
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetHDecimate(uint32 base_addr, uint16 decimate);

/** Get the configured video output type.
 *
 * This procedure retrieves the configured video output type on the TLG1120.
 * The video output type controls certain timing algorithms of the video output.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param vidOutType    returns the configured video output type.
 *                      Supported values are:
 *                      - TLG_VID_DENC: output destined for digital encoder.
 *                      - TLG_VID_LCD:  output destined for digital device.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetVidOutType(uint32 base_addr, uint16 *vidOutType);

/** Set the configured video output type.
 *
 * This procedure sets the configured video output type on the TLG1120.
 * The video output type controls certain timing algorithms of the video output.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param vidOutType    returns the configured video output type.
 *                      Supported values are:
 *                      - TLG_VID_DENC: output destined for digital encoder.
 *                      - TLG_VID_LCD:  output destined for digital device.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetVidOutType(uint32 base_addr, uint16 vidOutType);

/** Example of how to set the TLG1120 into standard 640x480 601 NTSC mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a standard 601 NTSC device. It sets up the following:
 *
 * visible Horizontal region (assert HSYNC): pixels 137 - 858
 *
 * horizontal blanking regions             : none
 *
 * vertical blanking regions (assert VSYNC): 1-25, 264-287
 *
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetVGA_601_NTSC(uint32 base_addr);

/** Example of how to set the TLG1120 into standard 601 PAL mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a standard 601 PAL device. It sets up the following:
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetVGA_601_PAL(uint32 base_addr);

/** Example of how to set the TLG100 into 240x240 NTSC for LCD display.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display a NTSC signal on a 240x240 LCD display running at 30 fields per
 * second. It sets the following:
 *
 * visible Horizontal region (assert HSYNC): pixels 195 - 674
 *
 * horizontal blanking regions             : pixels 1-15, 262-525
 *
 * vertical blanking regions (assert VSYNC): 1-19
 *
 * use default video configuration except:
 * - turn on horizontal decimation x2
 * - set 601 clock to TLG_NORMAL
 *
 * Notice this configuration does the following
 * for horizontal:
 * - decimates by 2
 * - crops output to 240 visible lines
 *
 * for vertical:
 * - vertical blanking only for first field. This effectively decimates the
 *   verticle output by 2
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set240x240_LCD_NTSC(uint32 base_addr);

/** Example of how to set the TLG100 into 240x240 PAL for LCD display.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display a PAL-G signal on a 240x240 LCD display running at 30 fields per
 * second. It sets the following:
 *
 * visible Horizontal region (assert HSYNC): pixels 197 - 676
 *
 * horizontal blanking regions             : pixels 1-46, 287-625
 *
 * vertical blanking regions (assert VSYNC): 1-19
 *
 * use default video configuration except:
 * - turn on horizontal decimation x2
 * - set 601 clock to TLG_NORMAL
 *
 * Notice this configuration does the following
 * for horizontal:
 * - decimates by 2
 * - crops output to 240 visible lines
 *
 * for vertical:
 * - vertical blanking only for first field. This effectively decimates the
 *   verticle output by 2
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set240x240_LCD_PAL(uint32 base_addr);

/** Example of how to set the TLG1120 into 480x480 NTSC for LCD display.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display a NTSC signal on a 480x480 LCD display running at 60 fields per
 * second. It sets the following:
 *
 * visible Horizontal region (assert HSYNC): pixels 195 - 674
 *
 * horizontal blanking regions             : pixels 1-15, 262-525
 *
 * vertical blanking regions (assert VSYNC): 1-19
 *
 * use default video configuration except:
 * - set 601 clock to TLG_NORMAL
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set480x480_NTSC(uint32 base_addr);

/** Example of how to set the TLG1120 into 480x480 PAL for LCD display.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display a PAL-G signal on a 480x480 LCD display running at 60 fields per
 * second. It sets the following:
 *
 * visible Horizontal region (assert HSYNC): pixels 197 - 676
 *
 * horizontal blanking regions             : pixels 1-46, 287-625
 *
 * vertical blanking regions (assert VSYNC): 1-19
 *
 * use default video configuration except:
 * - set 601 clock to TLG_NORMAL
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set480x480_PAL(uint32 base_addr);

/** Example of how to set the TLG1120 into standard QVGA NTSC mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a QVGA NTSC device. It sets up the following:
 *
 * @todo TLG_SetQVGA_NTSC not implemented.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetQVGA_NTSC(uint32 base_addr);


/** Example of how to set the TLG1120 into standard QVGA PAL mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a QVGA PAL device. It sets up the following:
 *
 * @todo TLG_SetQVGA_PAL not implemented.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetQVGA_PAL(uint32 base_addr);

/** Example of how to set the TLG1120 into standard 720x480 NTSC mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a 720x480 NTSC device. It sets up the following:
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set720x480_NTSC(uint32 base_addr);


/** Example of how to set the TLG1120 into standard 720x576 PAL mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a 720x576 PAL device. It sets up the following:
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set720x576_PAL(uint32 base_addr);

/** Example of how to set the TLG1120 into standard 360x240 NTSC mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a 360x240 NTSC device. It sets up the following:
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set360x240_NTSC(uint32 base_addr);


/** Example of how to set the TLG1120 into standard 360x288 PAL mode.
 *
 * This procedure is an example of how to use the TLG API to set the TLG1120
 * to display on a 360x288 PAL device. It sets up the following:
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set360x288_PAL(uint32 base_addr);

/** Get the current center frequency bandwidth setting on the TLG1120.
 *
 * This procedure returns the currently set bandwidth value on the TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param bw            Returns the current bandwidth setting. Values are:
 *                      - TLG_BW_5
 *                      - TLG_BW_6
 *                      - TLG_BW_7
 *                      - TLG_BW_8
 *                      - TLG_BW_12
 *                      - TLG_BW_15
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetChnBandwidth(uint32 base_addr,  uint16 *bw);

/** Set the video bandwidth on the TLG1120.
 *
 * This procedure sets the center frequency bandwidth value on the TLG1120.
 * This value will remain stable between frequency changes and only needs to
 * be called again when a new bandwidth setting is needed.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param bw            The new bandwidth setting. Values are:
 *                      - TLG_BW_5
 *                      - TLG_BW_6
 *                      - TLG_BW_7
 *                      - TLG_BW_8
 *                      - TLG_BW_12
 *                      - TLG_BW_15
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetChnBandwidth(uint32 base_addr, uint16 bw);

/** Set the play mode for the TLG1120.
 *
 * This procedure configures the TLG1120 to play video or FM Audio. This setting
 * will remain stable between frequency changes and only needs to be called
 * when a new mode setting is needed.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param mode          The desired mode. Valid values are:
 *                      -  TLG_MODE_ANALOG_TV_UNCOMP    Analog Television mode (uncompressed)
 *                      -  TLG_MODE_FM_RADIO            FM Radio mode
 *                      -  TLG_MODE_DVB_T               Digital TV (DVB-T)
 *                         [This is for diagnostic purposes only]
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetChnMode(uint32 base_addr, uint16 mode);

/** Get the currently configured video standard.
 *
 * This procedure gets the currently set video standard on the TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param std           The desired video standard. Valid values are:
 *                      - TLG_VSTD_NTSC_M
 *                      - TLG_VSTD_PAL_M
 *                      - TLG_VSTD_PAL_B
 *                      - TLG_VSTD_PAL_B1
 *                      - TLG_VSTD_PAL_D
 *                      - TLG_VSTD_PAL_D1
 *                      - TLG_VSTD_PAL_G
 *                      - TLG_VSTD_PAL_H
 *                      - TLG_VSTD_PAL_K
 *                      - TLG_VSTD_PAL_N
 *                      - TLG_VSTD_PAL_I
 *                      - TLG_VSTD_PAL_NC
 *                      - TLG_VSTD_SECAM_M
 *                      - TLG_VSTD_SECAM_B
 *                      - TLG_VSTD_SECAM_B1
 *                      - TLG_VSTD_SECAM_D
 *                      - TLG_VSTD_SECAM_D1
 *                      - TLG_VSTD_SECAM_G
 *                      - TLG_VSTD_SECAM_H
 *                      - TLG_VSTD_SECAM_K
 *                      - TLG_VSTD_SECAM_N
 *                      - TLG_VSTD_SECAM_I
 *                      - TLG_VSTD_SECAM_NC
 *                      - TLG_VSTD_SECAM_L
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetChnStandard(uint32 base_addr, uint16 *std);

/** Set the video standard to be received by the TLG1120.
 *
 * This procedure sets the video standard for the TLG1120. This value remains
 * stable between frequency changes and only needs to be called when a new
 * video standard is needed. If mode has been set to TLG_MODE_FM_RADIO, then
 * the video standard needs to be set to TLG_VSTD_NTSC.
 *
 * NOTE: for countries with hybrid channels, it used the hybrid channel map.
 * The TLG_SetChnStandard() should always be called with PAL standard first
 * by removing the TLG_VSTD_HYBRID_BIT (bit 15) out of the standard
 * then call TLG_SetAlternateStandard() to detect and switch to the right
 * standard
 * 
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param std           The desired video standard. Valid values are:
 *                      - TLG_VSTD_NTSC_M
 *                      - TLG_VSTD_PAL_M
 *                      - TLG_VSTD_PAL_B
 *                      - TLG_VSTD_PAL_B1
 *                      - TLG_VSTD_PAL_D
 *                      - TLG_VSTD_PAL_D1
 *                      - TLG_VSTD_PAL_G
 *                      - TLG_VSTD_PAL_H
 *                      - TLG_VSTD_PAL_K
 *                      - TLG_VSTD_PAL_N
 *                      - TLG_VSTD_PAL_I
 *                      - TLG_VSTD_PAL_NC
 *                      - TLG_VSTD_SECAM_M
 *                      - TLG_VSTD_SECAM_B
 *                      - TLG_VSTD_SECAM_B1
 *                      - TLG_VSTD_SECAM_D
 *                      - TLG_VSTD_SECAM_D1
 *                      - TLG_VSTD_SECAM_G
 *                      - TLG_VSTD_SECAM_H
 *                      - TLG_VSTD_SECAM_K
 *                      - TLG_VSTD_SECAM_N
 *                      - TLG_VSTD_SECAM_I
 *                      - TLG_VSTD_SECAM_NC
 *                      - TLG_VSTD_SECAM_L
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetChnStandard(uint32 base_addr, uint16 std);

/** Perform a synchronous Analog TV center frequency change.
 *
 * This procedure performs a channel change to the specified center frequency.
 * Note that before this change may occur, the following must have been done:
 *  Set default register values (see tlg1120.ini)
 *
 *  TLG_Init() (or TLG_InitASYNC() "loop")
 *
 *  Set up video and audio modes
 *
 *  TLG_SetChnMode(base_addr, TLG_ANALOG_TV);
 *
 *  TLG_SetChnStandard(...);
 *
 *  TLG_SetChnBandwidth(...);
 *
 * Once this has been done, TLG_SetTVChnFreq() may be called. It may be called
 * repeatedly without calling the other routines as long as the bandwidth,
 * video standard and mode haven't changed.
 * THIS PROCEDURE IS A SYNCHRONOUS CALL AND MAY CONTAIN EMBEDDED CALLS TO
 * TLG_Delay(). IF TLG_EMBEDDED_TIMERS IS NOT DEFINED THIS PROCEDURE WILL
 * RETURN TLG_ERR_NOT_SUPPORTED AND NO ACTION WILL BE PERFORMED.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_SetTVChnFreq(uint32 base_addr, uint32 center_freq_hz);

/** Perform an asynchronous TV Center frequency change.
 *
 * This is the asynchronous version of TLG_SetTVChnFreq(). It should be called
 * initially with state[0] set to TLG_STATE0. When state[0] == TLG_STATE_FINAL
 * is returned, the full call is completed. The amount of time in milliseconds
 * to wait before calling the procedure again is returned in ms. Note that the
 * values of the parameters should not be changed in-between calls, and the
 * behaviour of the system is unknown if the state machine is not completed.
 *
 * @sa TLG_SetTVChnFreq
 * @sa TLG_InitASYNC
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */

TLGDLL_API int TLG_SetTVChnFreqASYNC(uint32 base_addr, uint32 center_freq_hz,
                          tlg_statep state, int *ms);

/** Perform a synchronous FM Radio center frequency change.
 *
 * This procedure performs a channel change to the specified center frequency.
 * Note that before this change may occur, the following must have been done:
 *  Set default register values (see tlg1120.ini)
 *
 *  TLG_Init() (or TLG_InitASYNC() "loop")
 *
 *  Set up video and audio modes
 *
 *  TLG_SetChnMode(base_addr, TLG_FM_RADIO);
 *
 *  TLG_SetChnStandard(base_addr, TLG_VSTD_NTSC);
 *
 * Once this has been done, TLG_SetFMChnFreq() may be called. It may be called
 * repeatedly without calling the other routines as long as the
 * video standard and mode haven't changed.
 * THIS PROCEDURE IS A SYNCHRONOUS CALL AND MAY CONTAIN EMBEDDED CALLS TO
 * TLG_Delay(). IF TLG_EMBEDDED_TIMERS IS NOT DEFINED THIS PROCEDURE WILL
 * RETURN TLG_ERR_NOT_SUPPORTED AND NO ACTION WILL BE PERFORMED.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param slope             slope value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 * @param offset            offset value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_SetFMChnFreq(uint32 base_addr, uint32 center_freq_hz, int32 slope, int32 offset);

/** Perform an asynchronous FM Center frequency change.
 *
 * This is the asynchronous version of TLG_SetFMChnFreq(). It should be called
 * initially with state[0] set to TLG_STATE0. When state[0] == TLG_STATE_FINAL
 * is returned, the full call is completed. The amount of time in milliseconds
 * to wait before calling the procedure again is returned in ms. Note that the
 * values of the parameters should not be changed in-between calls, and the
 * behaviour of the system is unknown if the state machine is not completed.
 *
 * @sa TLG_SetFMChnFreq
 * @sa TLG_InitASYNC
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param slope             slope value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 * @param offset            offset value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_SetFMChnFreqASYNC(uint32 base_addr, uint32 center_freq_hz,
                     int32 slope, int32 offset, tlg_statep state, int *ms);

/** Perform a synchronous TV channel change and quickly get its lock status.
 *
 * This procedure performs a TLG_SetTVChnFreq() and then quickly detects
 * the locking state. This procedure does not wait for video lock to occur.
 * Instead it checks for other video locking metrics that should also be
 * present in a TV channel. This procedure is ideal for quickly scanning a list
 * of channels for populated video channels, or when performing a channel
 * change where the reliability of the video lock status need not be 100%.
 * Note all initial setup requiremens that apply to TLG_SetTVChnFreq() apply
 * to this function as well.
 * THIS PROCEDURE IS A SYNCHRONOUS CALL AND MAY CONTAIN EMBEDDED CALLS TO
 * TLG_Delay(). IF TLG_EMBEDDED_TIMERS IS NOT DEFINED THIS PROCEDURE WILL
 * RETURN TLG_ERR_NOT_SUPPORTED AND NO ACTION WILL BE PERFORMED.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_FastScanTVChn(uint32 base_addr, uint32 center_freq_hz);

/** Perform an asynchronous TV Channel Change and verify its locking status.
 *
 * This is the asynchronous version of TLG_ScanTVChn().  It should be called
 * initially with state[0] set to TLG_STATE0. When state[0] == TLG_STATE_FINAL
 * is returned, the full call is completed. The amount of time in milliseconds
 * to wait before calling the procedure again is returned in ms. Note that the
 * values of the parameters should not be changed in-between calls, and the
 * behaviour of the system is unknown if the state machine is not completed.
 *
 * @sa TLG_ScanTVChn
 * @sa TLG_InitASYNC
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */

TLGDLL_API int TLG_FastScanTVChnASYNC(uint32 base_addr, uint32 center_freq_hz,
                       tlg_statep state, int *ms);

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
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_ScanTVChn(uint32 base_addr, uint32 center_freq_hz);

/** Perform an asynchronous TV Channel Change and quickly get its lock status.
 *
 * This is the asynchronous version of TLG_FastScanTVChn(). It should be called
 * initially with state[0] set to TLG_STATE0. When state[0] == TLG_STATE_FINAL
 * is returned, the full call is completed. The amount of time in milliseconds
 * to wait before calling the procedure again is returned in ms. Note that the
 * values of the parameters should not be changed in-between calls, and the
 * behaviour of the system is unknown if the state machine is not completed.
 *
 * @sa TLG_ScanTVChn
 * @sa TLG_InitASYNC
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_ScanTVChnASYNC(uint32 base_addr, uint32 center_freq_hz,
                       tlg_statep state, int *ms);

/** Synchronously calculate the starting parameters for the FM scan service.
 *
 * This procedure calculates the slope and offset parameters that are used to
 * perform an optimized FM frequency scan. To enable this mode, this function
 * is called with the minimum and maximum frequencies in the FM frequency map
 * to be used. The parameters slope and offset are returned. These values may
 * then be supplied to subsequent calls to TLG_ScanFMChn() or
 * TLG_ScanFMChnASYNC() to perform FM channel scans optimized for speed.
 * THIS PROCEDURE IS A SYNCHRONOUS CALL AND MAY CONTAIN EMBEDDED CALLS TO
 * TLG_Delay(). IF TLG_EMBEDDED_TIMERS IS NOT DEFINED THIS PROCEDURE WILL
 * RETURN TLG_ERR_NOT_SUPPORTED AND NO ACTION WILL BE PERFORMED.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param min_hz            The center frequency IN HERTZ of the lowest FM
 *                          channel in the frequency map.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param max_hz            The center frequency IN HERTZ of the highest FM
 *                          channel in the frequency map.
 * @param slope             the slope parameter is returned here. This param
 *                          may be supplied in subsequent calls to
 *                          TLG_ScanFMChn()
 * @param offset            the offset parameter is returned here. This param
 *                          may be supplied in subsequent calls to
 *                          TLG_ScanFMChn()
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_InitScanFMChn(uint32 base_addr, uint32 min_hz, uint32 max_hz,
                      int32 *slope, int32 *offset);

/** Perform an asynchronous TLG_InitScanFMChn.
 *
 * This is the asynchronous version of TLG_InitScanFMChn(). It should be called
 * initially with state[0] set to TLG_STATE0. When state[0] == TLG_STATE_FINAL
 * is returned, the full call is completed. The amount of time in milliseconds
 * to wait before calling the procedure again is returned in ms. Note that the
 * values of the parameters should not be changed in-between calls, and the
 * behaviour of the system is unknown if the state machine is not completed.
 *
 * @sa TLG_InitScanFMChn
 * @sa TLG_InitASYNC
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param min_hz            The center frequency IN HERTZ of the lowest FM
 *                          channel in the frequency map.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param max_hz            The center frequency IN HERTZ of the highest FM
 *                          channel in the frequency map.
 * @param slope             the slope parameter is returned here. This param
 *                          may be supplied in subsequent calls to
 *                          TLG_ScanFMChn()
 * @param offset            the offset parameter is returned here. This param
 *                          may be supplied in subsequent calls to
 *                          TLG_ScanFMChn()
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_InitScanFMChnASYNC(uint32 base_addr, uint32 min_hz, uint32 max_hz,
                      int32 *slope, int32 *offset, tlg_statep state, int *ms);

/** Perform a synchronous FM channel change and verify its locking status.
 *
 * This procedure has two modes of operation. The default mode (by setting
 * slope and offset to 0) performs a TLG_SetFMChnFreq() and then interrogates
 * the status registers to ascertain the locking state. Note all initial
 * setup requiremens that apply to TLG_SetTVChnFreq() apply to this function
 * as well. The video standard should be set to TLG_VSTD_NTSC_M, the bandwidth
 * should be set to TLG_BW_6, and the mode should be set to TLG_MODE_FM_RADIO.
 * The second mode of operation is to call this after first calling
 * TLG_InitScanFMChn() and obtaining optimized values for slope and offset. In
 * this mode FM frequency changes will be accelerated.
 * THIS PROCEDURE IS A SYNCHRONOUS CALL AND MAY CONTAIN EMBEDDED CALLS TO
 * TLG_Delay(). IF TLG_EMBEDDED_TIMERS IS NOT DEFINED THIS PROCEDURE WILL
 * RETURN TLG_ERR_NOT_SUPPORTED AND NO ACTION WILL BE PERFORMED.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param slope             slope value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 * @param offset            offset value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_NOT_SUPPORTED    TLG_EMBEDDED_TIMERS is not defined.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_ScanFMChn(uint32 base_addr, uint32 center_freq_hz,
                                        int32 slope, int32 offset);

/** Set the TLGAPI for appropriate region.
 *
 * This procedure any region-specific customization.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param region            Indicates the region. Supported values are:
 *                          TLG_REGION_US - United States
 *                          TLG_REGION-WW - World Wide except United States
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_SetRegion(uint32 base_addr, uint16 region);

/** Get the current region setting.
 *
 * This procedure gets the currently configured region setting.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param region            Indicates the region. Supported values are:
 *                          TLG_REGION_US - United States
 *                          TLG_REGION-WW - World Wide except United States
 *
 * @retval TLG_ERR_SUCCESS          Call succeeded.
 * @retval TLG_ERR_PARAM            Bad parameter value passed.
 * @retval TLG_ERR_FAIL             Error during call.
 */
TLGDLL_API int TLG_GetRegion(uint32 base_addr, uint16 *region);

/** Perform an Asynchronous FM channel change and verify its locking status.
 *
 * This is the asynchronous version of TLG_ScanFMChn(). It should be called
 * initially with state[0] set to TLG_STATE0. When state[0] == TLG_STATE_FINAL
 * is returned, the full call is completed. The amount of time in milliseconds
 * to wait before calling the procedure again is returned in ms. Note that the
 * values of the parametes should not be changed in-between calls, and the
 * behaviour of the system is unknown if the state machine is not completed.
 *
 * @sa TLG_ScanFMChn
 * @sa TLG_InitASYNC
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param center_freq_hz    The center frequency IN HERTZ to be tuned to.
 *                          i.e. if 55.27MHz is desired, pass 55270000.
 * @param slope             slope value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 * @param offset            offset value calculated in TLG_InitScanFMChn(). If
 *                          this function has not be called, then set this parm
 *                          to 0.
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ScanFMChnASYNC(uint32 base_addr, uint32 center_freq_hz,
                   int32 slope, int32 offset, tlg_statep state, int *ms);

/** Get the status of the currently tuned channel.
 *
 * This procedure will return the locking status for the currently set video
 * channel. Its results are undefined for FM mode.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param isLock            Returns the locking status.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetChnStatus(uint32 base_addr, bool *isLock);

/** Get the currently Enabled GPIOs.
 *
 * This procedure will return the currently enabled GPIOs. The value returned
 * will be a bitmask of the form: TLG_GPIOx | TLG_GPIOy... | TLG_GPIOz. For
 * instance, if GPIO1 and GPIO3 are enabled but GPIO2 is disabled, then the
 * value will be: TLG_GPIO3|TLG_GPIO1.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param value         Returns a bitmask indicating which GPIOs have been enabled.
 *                      Valid components of the bitmask are:
 *                      - TLG_GPIO1
 *                      - TLG_GPIO2
 *                      - TLG_GPIO3
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetGPIOEnable(uint32 base_addr, uint16 *value);

/** Enable/Disable GPIOs.
 *
 * This procedure allows a user to enable or disable GPIOs. Currently GPIOs
 * may only be enabled in the "output" direction. The "mask" parameter
 * determines which GPIO's enable status is going to change, value determines
 * the value of each GPIO. So, for instance, if a user wished to enable
 * GPIO1, disable GPIO2, and leave GPIO3 in its current state, the value for
 * "mask" would be TLG_GPIO1|TLG_GPIO2, and "value" would be set to
 * TLG_GPIO1_SET|TLG_GPIO2_CLR.
 *
 * If a GPIO has been enabled, its output value will reflect whatever has been
 * set by TLG_SetGPIOOutput(). If a GPIO has not been enabled its state will be
 * high impedence.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param mask          A bitmask of which GPIOs will be affected by this call.
 *                      Valid components of the bitmask are:
 *                      - TLG_GPIO1
 *                      - TLG_GPIO2
 *                      - TLG_GPIO3
 * @param value         A bitmask of the values to set each GPIO defined in
 *                      "mask". Valid components of the bitmask are:
 *                      - TLG_GPIO1_SET/TLG_GPIO1_CLR
 *                      - TLG_GPIO2_SET/TLG_GPIO2_CLR
 *                      - TLG_GPIO3_SET/TLG_GPIO3_CLR
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetGPIOEnable(uint32 base_addr, uint16 mask, uint16 value);

/** Get the current output values for GPIOs.
 *
 * This procedure will return the current output values for GPIOs. The value
 * returned will be a bitmask of the values. To determine the value for a
 * specific GPIO, AND the returned value with that GPIO's mask. For instance
 * if ((value & TLG_GPIO1) == TLG_GPIO1_SET) tests if GPIO1 has been set and
 * if ((value & TLG_GPIO2) == TLG_GPIO2_CLR) tests if GPIO2 has been cleared.
 * Note, there is no determination as to whether the GPIO is currently enabled
 * or not. So to get the full output status, the returned value must first
 * be AND'ed with the enable status. For instance,
 *
 * TLG_GetGPIOEnable(i2c_addr, &enableStat);
 *
 * TLG_GetGPIOOutput(i2c_addr, &output);
 *
 * if (((output & enableState) & TLG_GPIO3) == TLG_GPIO3_SET)...
 *
 * will fully determine whether GPIO3 is currently outputting a positive signal
 * on its pin or not.
 *
 * @param base_addr I2c address for TLG1120 chip.
 * @param value     Returns a bitmask indicating which GPIOs have been set.
 *                  Valid components of the bitmask are:
 *                  - TLG_GPIO1_SET/TLG_GPIO2_CLR
 *                  - TLG_GPIO2_SET/TLG_GPIO2_CLR
 *                  - TLG_GPIO3_SET/TLG_GPIO3_CLR
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetGPIOOutput(uint32 base_addr, uint16 *value);

/** Set GPIO output values.
 *
 * This procedure allows a user to set or clear GPIOs. The "mask" parameter
 * determines which GPIO's value is going to change. "value" determines
 * the value of each GPIO. So, for instance, if a user wished to set
 * GPIO1, clear GPIO2, and leave GPIO3 in its current state, the value for
 * "mask" would be TLG_GPIO1|TLG_GPIO2, and "value" would be set to
 * TLG_GPIO1_SET|TLG_GPIO2_CLR. Only GPIOs that are enabled will have their
 * output values set on their respective GPIO pin.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param mask          A bitmask of which GPIOs will be affected by this call.
 *                      Valid components of the bitmask are:
 *                      - TLG_GPIO1
 *                      - TLG_GPIO2
 *                      - TLG_GPIO3
 * @param value         A bitmask of the values to set each GPIO defined in
 *                      "mask". Valid components of the bitmask are:
 *                      - TLG_GPIO1_SET/TLG_GPIO1_CLR
 *                      - TLG_GPIO2_SET/TLG_GPIO2_CLR
 *                      - TLG_GPIO3_SET/TLG_GPIO3_CLR
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetGPIOOutput(uint32 base_addr, uint16 mask, uint16 value);

/** Get the Antenna Type.
 *
 * This procedure retrieves the currently configured antenna type. Note all
 * customers should use TLG_ANTENNA_DEF (the default value) unless told
 * to use a different value by Telegent Systems.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param antenna_type  The type of the antenna. Currently supported values are:
 *                      - TLG_ANTENNA_DEF   : Default antenna behavior.
 *                      - TLG_ANTENNA_HDPH  : Telegent Systems Internal use only.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAntennaType(uint32 base_addr, uint16 *antenna_type);

/** Set the Antenna Type.
 *
 * This procedure sets the antenna type to a specified value. Note all
 * customers should use TLG_ANTENNA_DEF (the default value) unless told
 * to use a different value by Telegent Systems.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param antenna_type  The type of the antenna. Currently supported values are:
 *                      - TLG_ANTENNA_DEF   : Default antenna behavior.
 *                      - TLG_ANTENNA_HDPH  : Telegent Systems Internal use only.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAntennaType(uint32 base_addr, uint16 antenna_type);

/** Reset the internal TLG1120 state machines.
 *
 * This is the asynchronous version of TLG_SoftReset(). It should be used for
 * implementations that run without TLG_EMBEDDED_TIMERS defined. See
 * TLG_SoftReset() for a full description of soft reset.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param state             State vector. By convention state[0] holds the state
 *                          variable and Initially this should be set to
 *                          TLG_STATE0. After that it should treated
 *                          as READ ONLY for the remainder of the call.
 *                          When state[0] == TLG_STATE_FINAL initialization is
 *                          complete. No more calls to TLG_InitASYNC() are needed.
 * @param ms                Returns the number of minimum number of milliseconds
 *                          before the next TLG_InitASYNC() call is made.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_SoftReset
 */
TLGDLL_API int TLG_SoftResetASYNC(uint32 base_addr, tlg_statep state, int *ms);

/** Reset the internal TLG1120 state machines.
 *
 * This procedure resets internal TLG1120 state machines. It does not disturb
 * the current register values or calibration settings. Note, however, that
 * since it preserves the existing calibration settings, it assumes that a
 * channel change (even an "unsuccessful" one) has already occurred before it
 * is called. Therefore, it should not be called until after the initial
 * channel change has been performed.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SoftReset(uint32 base_addr);

/** Get Analog TV Signal Strength Indicator.
 *
 * This procedure provides a signal strength indicator in the range
 * 0 - TLG_NUM_SSI_METRIC for Analog Television signals. For FM use
 * TLG_GetFMSSI().
 *
 * For best results, this procedure should be called at 500 millisecond
 * intervals for the first 5 seconds after a channel change.
 * Subsequent calls may be made at 1 second intervals.
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param metric        Signal Strength metric is returned here.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetTVSSI(uint32 base_addr, uint16 *metric);

/** Get FM Audio Signal Strength Indicator.
 *
 * This procedure provides a signal strength indicator in the range
 * 0 - TLG_NUM_SSI_METRIC for FM Audio signals. For Analog TV use
 * TLG_GetTVSSI().
 *
 * For best results, this procedure should be called at 500 millisecond
 * intervals for the first 5 seconds after a channel change.
 * Subsequent calls may be made at 1 second intervals.
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param metric        Signal Strength metric is returned here.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetFMSSI(uint32 base_addr, uint16 *metric);


/** Determine if the channel locking hardware needs to be restarted.
 *
 * This procedure can be used to determine if the TLG1120 channel locking
 * hardware state machines have moved out of state and need to be restarted.
 * The proper way to restart them is to cal TLG_SoftReset(). This procedure
 * may be called at 500ms - 1 second interval as part of a channel status
 * polling loop. It may be called whether a channel is currently locked (e.g.
 * TLG_ScanTV() returned CHANNEL_EXISTS) or not. This procedure SHOULD NOT be
 * called during channel scan (e.g. TLGAPP_ScanChannels()). So any periodic
 * channel status polling loop should not be started until after the channel
 * change procedures have completed.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param lockStat      The restart status for the hardware is returned here.
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetChnRestartStatus(uint32 base_addr, bool *lockStat);

/** Run the Default Value Register Test.
 *
 * This procedure will test the TLG1120 registers to verify their default
 * values. IT MUST BE RUN BEFORE ANY OTHER PROCEDURE IS RUN. This is to
 * insure that the registers contain their reset values.
 *
 * This test uses TLG_REG_LOG to output its test results so it requires
 * TLG_DO_REG_LOGGING to be defined.
 *
 * The code for this procedure is only compiled into the system if
 * TLG_ENABLE_I2C_QUAL_TESTS is defined.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 */
TLGDLL_API void TLG_TestDefRegs(uint32 base_addr);

/** Run the Register Write/Read Tests.
 *
 * This procedure will test the TLG1120 registers for the ability to write
 * and read different patterns.
 *
 * This test uses TLG_REG_LOG to output its test results so it requires
 * TLG_DO_REG_LOGGING to be defined.
 *
 * The code for this procedure is only compiled into the system if
 * TLG_ENABLE_I2C_QUAL_TESTS is defined.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 */
TLGDLL_API void TLG_TestWriteRegs(uint32 base_addr);

/** Freeze the AGC loops at either their max or min values.
 *
 * This procedure is available for manufacturing tests which need to run
 * performance tests when the AGC loops are frozen. This call must be followed
 * by a call to TLG_RestoreAgcLoops(). THIS PROCEDURE IS FOR
 * MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL OPERATION OF THE
 * TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param minmax        One of the followin values:
 *                      TLG_FREEZE_MAX - freeze loops at maximum gain
 *                      TLG_FREEZE_MIN - freeze loops at minimum gain
 * @param state         state vector to store agc gain values. This param
 *                      must be supplied to TLG_RestoreAgcLoops() to correctly
 *                      restore the agc loops to their previous state.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_FreezeAgcLoops(uint32 base_addr, int minmax, tlg_statep state);

/** Unfreeze the AGC loops and restore them to their previous values.
 *
 * This procedure unfreezes the AGC loops on the TLG1120 that were frozen in
 * a previous TLG_FreezeAgcLoops() call, and restores them to their values
 * before the TLG_FreezeAgcLoops() call was made. THIS PROCEDURE IS FOR
 * MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL OPERATION OF THE
 * TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param state         state vector to store agc gain values. This parameter
 *                      must have been passed to a previous call to
 *                      TLG_FreezeAgcLoops().
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_RestoreAgcLoops(uint32 base_addr, tlg_statep state);

/** Put the TLG1120 into ADC mode.
 *
 * This procedure may be used during manufacturing test to put the TLG110
 * into ADC mode. See TLG1120 documentation for more information about this
 * mode. THIS PROCEDURE IS FOR
 * MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL OPERATION OF THE
 * TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param state         state vector to store the current TLG1120 state. It
 *                      should be passed to a later call to TLG_ExitAdcMode().
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_EnterAdcMode(uint32 base_addr, tlg_statep state);

/** Restore the TLG1120 to normal after previous TLG_EnterAdcMode() call.
 *
 * This procedure should be called after a call to TLG_EnterAdcMode()
 * to restore the TLG1120 back to a normal operating state. THIS PROCEDURE IS
 * FOR MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL OPERATION OF THE
 * TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param state         state vector from a previous call to TLG_EnterAdcMode().
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ExitAdcMode(uint32 base_addr, tlg_statep state);

/** Retrieve the current tracking filter calibration code.
 *
 * This procedure returns the current tracking filter calibrationt code.
 * THIS PROCEDURE IS FOR MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL
 * OPERATION OF THE TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param tfCalCode     points to an array to fill with tf cal code value.
 *                      Depending on the current frequency, different value are
 *                      returned to the array
 *                      VHF1        : tfCalCode[0]  = tf dac bit 9:0
 *                      VHF2/VHF3   : tfCalCode[0]  = cin_code  bit 5:0
 *                                                    cnotch    bit 15:6
 *                      UHF         : tfCalCode[0]  = cgstune   bit 15:11
 *                                                    cal_override  bit 5
 *                                                    citune        bit 4:0
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetTFCalCode(uint32 base_addr, uint16 *tfCalCode);
/** Retrieve the current tracking filter calibration code.
 *
 * This procedure returns the current tracking filter calibrationt code.
 * THIS PROCEDURE IS FOR MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL
 * OPERATION OF THE TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param tfCalCode     points to an array to fill with tf cal code value.
 *                      Depending on the current frequency, different value are
 *                      returned to the array
 *                      VHF1        : tfCalCode[0]  = tf dac bit 9:0
 *                      VHF2/VHF3   : tfCalCode[0]  = cin_code  bit 5:0
 *                                                    cnotch    bit 15:6
 *                      UHF         : tfCalCode[0]  = cgstune   bit 15:11
 *                                                    cal_override  bit 5
 *                                                    citune        bit 4:0
 *
 * @param curFreq      The current freq.  Instead of asking the chip for the current freq
 *                     caller can supply the current frequency so it can return the
 *                     appropriate cal code
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetCalCode(uint32 base_addr, uint16 *tfCalCode, uint32 curFreq);

/** Retrieve the total gain for the TLG1120.
 *
 * This procedure returns the total gain for the currently tuned channel.
 * THIS PROCEDURE IS FOR MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL
 * OPERATION OF THE TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param ttlGain       points to variable to fill with total gain value.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetTotalGain(uint32 base_addr, uint16 *ttlGain);

/** Retrieve the Peak AGC Gain for the TLG1120.
 *
 * This procedure returns the Peak AGC gain for the currently tuned channel.
 * THIS PROCEDURE IS FOR MANUFACTURING TEST ONLY. IT IS NOT NEEDED FOR NOMAL
 * OPERATION OF THE TLG1120.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param peakAgcGain   points to variable to fill with peak agc gain value.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetPeakAgcGain(uint32 base_addr, uint16 *peakAgcGain);

/** Retrieve the locking status of the current channel.
 *
 * This procedure retrieves the current status of the various state machines
 * within the TLG1120. The state machines to check are selected by the
 * "lockTypes" parameter and include: video signal, vsync, hsync, chroma,
 * luma, the frequency synthesizer, and audio signal. lockTypes is a bitmask
 * allowing for a set of locks to be checked in one call. For example to check
 * video lock, vsync, hsync, chroma and luma lockTypes would be set to:
 * (TLG_LOCK_STATE_VIDEO | TLG_LOCK_STATE_VSYNC | TLG_LOCK_STATE_HSYNC |
 *  TLG_LOCK_STATE_CHROMA | TLG_LOCK_STATE_LUMA). "locks" is also a bitmask
 * where a "1" in the corresponding bit indicates the specified state machine
 * lis locked. Only bits set to "1" in lockTypes will have defined values in
 * the returned "locks" value. NOTE THAT CHECKING THE AUDIO
 * SIGNAL COULD COULD DEGRADE THE VIDEO OUTPUT (if present). Therefore setting
 * TLG_LOCK_STATE_AUDIO should only be done for checking FM (where there is no
 * video signal) or during Manufacturing Test Procedures.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param lockTypes     A bitmask determining which machine status's will
 *                      be checked. Values which may be OR'd together are:
 *                      TLG_LOCK_STATE_VIDEO - Check video signal lock state
 *                      TLG_LOCK_STATE_VSYNC - Check vsync lock state
 *                      TLG_LOCK_STATE_HSYNC - Check hsync lock state
 *                      TLG_LOCK_STATE_CHROMA - Check chroma signal lock state
 *                      TLG_LOCK_STATE_LUMA - Check luma signal lock state
 *                      TLG_LOCK_STATE_SYNTH - Check state of the synthesizer
 *                      TLG_LOCK_STATE_AUDIO - Check audio signal lock state
 *                      TLG_LOCK_STATE_STEREO - Check the stereo lock state
 * @param locks         points to variable to be set to the corresponding
 *                      status as defined by lockTypes.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */

TLGDLL_API int TLG_GetLockStatus(uint32 base_addr, uint16 lockTypes, uint16 *locks);



/** Put the TLG1120 into Deep Sleep mode.
 *
 * This function puts the TLG1120 into Deep Sleep mode. Deep Sleep mode
 * means the TLG1120 is in a low power state and is not performing any
 * processing functions. The only thing it can do is respond to I2c commands.
 * Deep Sleep Mode is only supported in chip revision 1.3 and later. In order
 * to use Deep Sleep mode, the TLG1120 I2C must be configured to use the
 * Asynchronous mode option (GPIO5 is tied to Ground). As there are a number
 * of tasks that must be performed to restore the TLG1120 to full operational
 * state after this call, it is recommended that customers use TLGAPP_PowerDown
 * and TLGAPP_ReInitFromPowerDown instead of these calls.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param state         state vector to store state across the TLG1120 sleep.
 *                      state[0] MUST BE SET TO TLG_STATE0 prior to calling
 *                      this function.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_ReturnFromDeepSleep
 * @sa TLG_ReInitFromDeepSleep
 * @sa TLGAPP_PowerDown
 * @sa TLGAPP_ReInitFromPowerDown
 */
TLGDLL_API int TLG_DeepSleep(uint32 base_addr, tlg_statep state);

/** Perform the basic powerup procedure to restore the TLG1120 from Deep Sleep.
 *
 * This procedure restores the TLG1120 to basic operational state after a call
 * TLG_DeepSleep(). The "state" parameter that was passed into TLG_DeepSleep()
 * must be supplied to this call as well to restore the chip to its previous
 * state. Once this procedure has been called, it is expected that the TLG1120
 * registers will be re-initialized and TLG_ReInitFromDeepSleep() will then be
 * call to restore the TLG1120 to is proper powerup state. It is recommended
 * that customers call TLGAPP_PowerDown and TLGAPP_ReInitFromPowerDown to
 * manage the powerdown state rather than making the call dirtectly.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param state         state vector to store state across the TLG1120 sleep.
 *                      state[0] MUST BE SET TO TLG_STATE0 prior to calling
 *                      this function.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_DeepSleep
 * @sa TLG_ReInitFromDeepSleep
 * @sa TLGAPP_PowerDown
 * @sa TLGAPP_ReInitFromPowerDown
 */
TLGDLL_API int TLG_ReturnFromDeepSleep(uint32 base_addr, tlg_statep state);

/** Re-initialize the TLG1120 after it has returned from Deep Sleep mode.
 *
 * This procedure Performs the correct re-initialization sequence on the
 * TLG1120 after a TLG_DeepSleep() and subsequent TLG_ReInitFromDeepSleep()
 * have been called. It is recommended that customers use the TLGAPP_PowerDown
 * and TLGAPP_ReInitFromPowerDown calls to manage the Deep Sleep feature
 * rather than calling these procedures directly.
 *
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_DeepSleep
 * @sa TLG_ReturnFromDeepSleep
 * @sa TLGAPP_PowerDown
 * @sa TLGAPP_ReInitFromPowerDown
 */
TLGDLL_API int TLG_ReInitFromDeepSleep(uint32 base_addr);

/** Get the Hsync Low Power state.
 *
 * This procedure can be used to get the current hsync low power state.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param *val          TLG_ON or TLG_OFF.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetHsyncLowPower(uint32 base_addr, uint16 *val);

/** Set the Hsync signal into Low Power mode.
 *
 * This procedure can be used to set the low power hsync mode.
 * This mode should be used for all configurations that use extnernal
 * syncs.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param *val          TLG_ON or TLG_OFF.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetHsyncLowPower(uint32 base_addr, uint16 val);

/** Get the current state of the vertical decimator.
 *
 * This procedure can be used to get the current VDecimate setting.
 * It reduces the number of lines sending out.  
 * For example if VSync is set to 240:
 *       TLG_NO_VDECIMATION     will result in 240 lines
 *       TLG_VDECIMATE_2ND_LINE will result in 120 lines
 *       TLG_VDECIMATE_3RD_LINE will result in 160 lines
 *       TLG_VDEVIMATE_4TH_LINE will result in 180 lines
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param *val          Vertical decimation selection.
 *                          TLG_NO_VDECIMATION        
 *                          TLG_VDECIMATE_2ND_LINE    
 *                          TLG_VDECIMATE_3RD_LINE    
 *                          TLG_VDEVIMATE_4TH_LINE    
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetVDecimate(uint32 base_addr, uint16 *val);

/** Turn the vertical decimator on or off.
 *
 * This procedure can be used to set the Vertical Decimator.
 * Depending on which bit is on, 
 * the vertical decimator removes 1 line in every 2/3/4 lines from the
 * video output. The vertical decimator only affects output that relies
 * on the external V and H sync signals.
 * For example if VSync is set to 240:
 *       TLG_NO_VDECIMATION     will result in 240 lines
 *       TLG_VDECIMATE_2ND_LINE will result in 120 lines
 *       TLG_VDECIMATE_3RD_LINE will result in 160 lines
 *       TLG_VDEVIMATE_4TH_LINE will result in 180 lines
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param val           Vertical decimation selection.
 *                          TLG_NO_VDECIMATION        
 *                          TLG_VDECIMATE_2ND_LINE    
 *                          TLG_VDECIMATE_3RD_LINE    
 *                          TLG_VDEVIMATE_4TH_LINE    
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetVDecimate(uint32 base_addr, uint16 val);

/** Get Mobile Video Enhancement mode and adjustment value.
 *
 * This procedure can be used to get MVE mode and adjustment value.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param mode          TLG_MVE_AUTO or TLG_MVE_MANUAL.
 * @param val           Amount of MVE adjustment (0~63).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetMobileVidEnhancement(uint32 base_addr, uint16 *mode, uint16 *val);

/** Switch between Mobile Video Enhancement Auto or Manual mode.
 *
 * This procedure can be used to set MVE in auto or manual mode.
 * When in auto mode, the Mobile Video Enhancement feature is automatically
 * adjusted. When in manual mode, the MVE is always on, and the amount of
 * MVE can be changed.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param mode          TLG_MVE_AUTO or TLG_MVE_MANUAL.
 * @param val           Amount of MVE adjustment (0~63).
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetMobileVidEnhancement(uint32 base_addr, uint16 mode, uint16 val);

/** Unlock the screen so it has better tolerance in VSync timing.
 *
 * This procedure should be called during channel set so that it eliminates the fact that
 * the screen stays in incorrect positions until a refresh occurs after channel change.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_UnlockVScreen(uint32 base_addr);

/** Lock the screen so it does not move or jitter vertically.
 *
 * This procedure should be called after a channel set so that it eliminates the fact that
 * the screen moves vertically due to varying VSync timings. This is a recommended setting.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_LockVScreen(uint32 base_addr);


/** Read, modify and write operation.
 *
 * Provide a simple way to modify some bits in the register
 * without too much overhead.
 * Note that all parameters now are 32 bits to be able to support
 * 32 bit address from amber space
 *
 * @param base_addr i2c address for the device
 * @param reg_addr  register address to modify
 * @param new_val   new value to set
 * @param shift     starting bit to modify
 * @param mask      mask of the bits
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 ****************************************************************************/

TLGDLL_API int TLG_SetBits(
    uint32 base_addr    ,
    uint32 reg_addr     ,
    uint32 new_val      ,
    uint32 shift        ,
    uint32 mask
    );

/*****************************************************************************
 * TLG_ModifyBits                                                        *
 ****************************************************************************/
/** Read, modify and write operation only when register value is changed.
 *
 * Provide a simple way to modify some bits in the register 
 * without too much overhead.
 * Note that all parameters now are 32 bits to be able to support
 * 32 bit address from amber space
 * Only write when the register value changed
 *
 * @param base_addr i2c address for the device
 * @param reg_addr  register address to modify
 * @param new_val   new value to set
 * @param shift     starting bit to modify
 * @param mask      mask of the bits
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 * 
 ****************************************************************************/

TLGDLL_API int TLG_ModifyBits(
    uint32 base_addr    , 
    uint32 reg_addr     , 
    uint32 new_val      ,
    uint32 shift        ,
    uint32 mask
    );

/*! TLG_VbiSetup
 *
 * Sets up the Vbi parameters.
 * WARNING: THIS CALL IS DEPRECATED AND WILL BE REMOVED FROM A FUTURE RELEASE.
 *
 * @param    base_addr              - I2c address
 * @param    field_1_start_line     - field 1 start line
 * @param    filed_1_stop_line      - field 1 stop line
 * @param    field_2_start_line     - field 2 start line
 * @param    filed_2_stop_line      - field 2 stop line
 * @param    start_pixel            - starting pixel
 * @param    end_pixel              - end pixel
 * @param    interpolater_disable   - 0: turn on interpolater, 1: turn off interpolater
 *
!*/

TLGDLL_API void TLG_VbiSetup(
    uint32 base_addr           ,
    uint32 field_1_start_line  ,
    uint32 filed_1_stop_line   ,
    uint32 field_2_start_line  ,
    uint32 filed_2_stop_line   ,
    uint32 start_pixel         ,
    uint32 end_pixel           ,
    uint32 interpolater_disable);

/*! Return the VBI size
 * WARNING: THIS CALL IS DEPRECATED AND WILL BE REMOVED FROM A FUTURE RELEASE.
 *
!*/
TLGDLL_API int TLG_VbiGetSize(uint32 base_addr);

typedef enum _TLG_ALTERNATIVE_MODE
{
    TLG_ALT_NORMAL                  = 0x00,
    TLG_ALT_DIGITAL_TEST_MUX_OUT    ,
    TLG_ALT_AFTER_ADC               ,
    TLG_ALT_AFTER_CHANNEL_FILTERING ,
    TLG_ALT_AFTER_DIGITAL_AGC       ,
    TLG_ALT_IQ_MISMATCH_CORRECTION  ,
    TLG_ATOP_BYPASS
} TLG_ALTERNATIVE_MODE;
/** Set the video stream output mode.
 *
 * This procedure sets the chip to the specified mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param videoout_mode   Value to set mode to. Possible values:
 *                   TLG_TUNE_VID_FORMAT_YCBCR     = 0x00000001
 *                   TLG_TUNE_VID_FORMAT_YUV       = 0x00000002
 *                   TLG_TUNE_VID_FORMAT_RGB_565   = 0x00000004
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetVideoStreamOutputMode(uint32 base_addr, uint32 videoout_mode);

/** Get the video stream output mode.
 *
 * This procedure sets the chip to the specified mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_videoout_mode   Returned value to possible values:
 *                   TLG_TUNE_VID_FORMAT_YCBCR     = 0x00000001
 *                   TLG_TUNE_VID_FORMAT_YUV       = 0x00000002
 *                   TLG_TUNE_VID_FORMAT_RGB_565   = 0x00000004
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetVideoStreamOutputMode(uint32 base_addr, uint32 *p_videoout_mode);


/** Set the FM Stereo Mode.
 *
 * This procedure sets the chip to the specified mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param stereo_mode   Value to set mode to. Possible values:
 *                      - TLG_FM_MONO  : audio mono mode
 *                      - TLG_FM_STEREO: audio stereo mode
 *                      - TLG_FM_AUTO  : audio auto detect mode
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Set_FM_AudioStereoMode(uint32 base_addr, uint16 stereo_mode);

/** Get the FM Stereo Mode.
 *
 * This procedure gets FM audio stereo/mono mode.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_stereo_mode   Return the FM audio mono/stereo mode. Possible values:
 *                      - TLG_FM_MONO: audio mono mode
 *                      - TLG_FM_STEREO: audio stereo mode
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_Get_FM_AudioStereoMode(uint32 base_addr, uint16 *p_stereo_mode) ;

typedef struct _tlg_cc_field_s
{
    uint16   cc_line_start  ;
    uint16   cc_line_stop   ;
} tlg_cc_field_t;

typedef struct _tlg_cc_attrib_s
{
    uint16  cc_hsync_sel    ;  /* 0xd0[8]       */
    uint16  cc_flip_vsync   ;  /* 0xd0[5]       */
    uint16  cc_flip_hsync   ;  /* 0xd0[4]       */
    uint16  cc_output_sel   ;  /* 0xd0[3:2]     */
    uint16  cc_data_dly     ;  /* 0xd5[15:8]    */
    uint16  cc_center_dly   ;  /* 0xd5[7:0]     */
    uint16  cc_slice_lpf    ;  /* 0xd6[13:12]   */
    uint16  cc_parity_win   ;  /* 0xd6[11:10]   */
    uint16  cc_pass_parity  ;  /* 0xd6[8]       */
    uint16  cc_correct_errs ;  /* 0xd6[7]       */
    uint16  cc_drop_errs    ;  /* 0xd6[6]       */
    uint16  cc_drop_nulls   ;  /* 0xd6[5]       */
    uint16  cc_insert_nulls ;  /* 0xd6[4]       */
    uint16  cc_avg_sel      ;  /* 0xd6[2]       */
    uint16  cc_lpf_sel      ;  /* 0xd[1:0]      */
    uint16  cc_start_thresh ;  /* 0xd7[15:8]    */
    uint16  cc_freq_adj     ;  /* 0xd7[7:0]     */
} tlg_cc_attrib_t;

/** Get the Close Caption attribute.
 *
 * This procedure gets attribute for the close caption.
 * Use this procedure to get the default value from the chip,
 * modify it if needed, but normally the defaul value should work fine.
 * Only the line_start and line_stop for each field needed to be change
 * For example: field 0 line_start = line_stop = 21 for NTSC and 22 for PAL
 *              field 1 line_start = line_stop = 284 for NTSC and 335 for PAL
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_cc_attrib   pointer to the close caption attribute structure
 *                      normally this should not be changed, the default
 *                      value should work fine
 *                      cc_hsync_sel
 *                      cc_flip_vsync
 *                      cc_flip_hsync
 *                      cc_output_sel
 *                      cc_data_dly
 *                      cc_center_dly
 *                      cc_slice_lpf
 *                      cc_parity_win
 *                      cc_pass_parity
 *                      cc_correct_errs
 *                      cc_drop_errs
 *                      cc_drop_nulls
 *                      cc_insert_nulls
 *                      cc_avg_sel
 *                      cc_lpf_sel
 *                      cc_start_thresh
 *                      cc_freq_adj
 * @param p_cc_field0   pointer to the close caption field 0 attribute:
 *                      cc_line_start  ;
 *                      cc_line_stop   ;
 * @param p_cc_field1   pointer to the close caption field 1 attribute:
 *                      cc_line_start  ;
 *                      cc_line_stop   ;
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_CC_SetAttrib
 * @sa TLG_CC_Control
 * @sa TLG_CC_GetRawData
 */
TLGDLL_API int TLG_CC_GetAttrib(
    uint32                  base_addr,
    tlg_cc_attrib_t        *p_cc_attrib,
    tlg_cc_field_t         *p_cc_field0,
    tlg_cc_field_t         *p_cc_field1);

/** Set the Close Caption attribute.
 *
 * This procedure sets attribute for the close caption.
 * Should call the TLG_CC_GetAttrib() once at the beginning to get the
 * default values, then modify the field 0 & field1 start/stop line number
 * before calling this procedure to write it back
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_cc_attrib   pointer to the close caption attribute structure
 *                      normally this should not be changed, the default
 *                      value should work fine
 *                      cc_hsync_sel
 *                      cc_flip_vsync
 *                      cc_flip_hsync
 *                      cc_output_sel
 *                      cc_data_dly
 *                      cc_center_dly
 *                      cc_slice_lpf
 *                      cc_parity_win
 *                      cc_pass_parity
 *                      cc_correct_errs
 *                      cc_drop_errs
 *                      cc_drop_nulls
 *                      cc_insert_nulls
 *                      cc_avg_sel
 *                      cc_lpf_sel
 *                      cc_start_thresh
 *                      cc_freq_adj
 * @param p_cc_field0   pointer to the close caption field 0 attribute:
 *                      cc_line_start  ;
 *                      cc_line_stop   ;
 * @param p_cc_field1   pointer to the close caption field 1 attribute:
 *                      cc_line_start  ;
 *                      cc_line_stop   ;
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_CC_GetAttrib
 * @sa TLG_CC_Control
 * @sa TLG_CC_GetRawData
 */
TLGDLL_API int TLG_CC_SetAttrib(
    uint32                  base_addr,
    tlg_cc_attrib_t        *p_cc_attrib,
    tlg_cc_field_t         *p_cc_field0,
    tlg_cc_field_t         *p_cc_field1);

/** Start and stop close caption for each field.
 *
 * This procedure starts/stops the close caption data for each field.
 * It should be called before calling the TLG_CC_GetRawData()
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param field0_en     enable capturing close caption data field 0
 * @param field1_en     enable capturing close caption data field 1
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_CC_GetAttrib
 * @sa TLG_CC_SetAttrib
 * @sa TLG_CC_GetRawData
 */
TLGDLL_API int TLG_CC_Control(
    uint32                  base_addr,
    uint16                  field0_en,
    uint16                  field1_en
    );
/** Get close caption raw data for each field.
 *
 * This procedure only gets the raw data for the field specified by
 * the field_number parameter. It does not decode the raw data.
 * User should call this function every 1/2 second or more often to get the cc data
 * Before calling this function, this is the normal sequence:
 *                      TLG_CC_GetAttrib()
 *                          modify the field start/stop lines
 *                      TLG_CC_SetAttrib()
 *                          call to enable cc for field 0 and 1
 *                      TLG_CC_Control()
 *                          every 1/2 second, or more often, polling the data
 *                      TLG_CC_GetRawData()
 *                          call to disable cc for field 0 and 1 when done
 *                      TLG_CC_Control()
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param field_number     0: field 0, 1: field 1
 * @param p_length         return the number of bytes read from the fifo
 * @param p_data           raw data read back from the field
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 * @sa TLG_CC_GetAttrib
 * @sa TLG_CC_SetAttrib
 * @sa TLG_CC_Control
 */
TLGDLL_API int TLG_CC_GetRawData(
    uint32                  base_addr,
    uint16                  field_number,
    uint16                 *p_length ,
    uint8                  *p_data
    );

/** Get the current freq.
 *
 * This procedure gets the current freq
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_current_freq_hz     the current frequency in hz
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API int TLG_GetCurrentFreq(uint32 base_addr, uint32 *p_current_freq_hz);

/** Write a series of register from the table.
 *
 * This procedure writes a series of registers from a table.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_tbl         Table of registers to write.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_WritePair(uint32 base_addr, const tlg_reg_pair *p_tbl);

/** Initialize AGC registers.
 *
 * This procedure writes a set of AGC registers depending on the
 * tv_mode and agc mode. It does not change the internal channel mode or agc mode.
 * To change the agc_mode, use TLG_Set_AGC_Behavior().
 * To change the channel mode, use TLG_SetChnMode
 * It is used mainly internally by other API call
 * @param base_addr     I2c address for TLG1120 chip.
 * @param tv_mode       Current tv_mode
 *                      -  TLG_MODE_NONE                No Mode specified
 *                      -  TLG_MODE_ANALOG_TV_UNCOMP    Analog Television mode (uncompressed)
 *                      -  TLG_MODE_ANALOG_TV_COMP      Analog TV mode (compressed)
 *                      -  TLG_MODE_FM_RADIO            FM Radio mode
 *                      -  TLG_MODE_DVB_T               Digital TV (DVB-T)
 *                      -  TLG_MODE_DVB_H               Digital TV (DVB-H)
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 * 
 * @sa TLG_SetChnMode
 * @sa TLG_Set_AGC_Behavior
 *
 */
TLGDLL_API int TLG_Init_AGC_Reg(
    uint32                  base_addr,
    int                     tv_mode
    );

/** Calibrate with the current frequency.
 *
 * This procedure calibrates with the current freq
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param freq_hz       the current frequency in hz
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API void TLG_TFCalibration( uint32 base_addr, uint32 freq_hz);

/** Set AGC behavior.
 *
 * This procedure sets the current AGC behavior, for tlg1120 rev 1.2 
 * and later only. It permanently changes the AGC behavior to 
 * AGC NORMAL or AGC Always mode.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param new_agc_mode      
 *                          TLG_AGC_NORMAL_MODE: 
 *                              separate register tables for both modes
 *                          TLG_AGC_ALWAYS_MODE: 
 *                              use the same AGC always register tables for both modes
 *                          TLG_AGC_CUSTOM_MODE:
 *                              use the custom tables for both modes
 * @param update_agc_reg    if set, it will call the function TLG_Init_AGC_Reg
 *                          to update the AGC regs.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 */
TLGDLL_API int TLG_Set_AGC_Behavior(
    uint32                  base_addr,
    uint32                  new_agc_mode,
    uint32                  update_agc_reg
    );

/** Get AGC behavior.
 *
 * This procedure returns the current AGC behavior, for tlg1120 rev 1.2 
 * and above only
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param p_agc_mode          return the current AGC mode
 *                          TLG_AGC_NORMAL_MODE: 
 *                              separate register tables for both modes
 *                          TLG_AGC_ALWAYS_MODE: 
 *                              use the same AGC always register tables for both modes
 *                          TLG_AGC_CUSTOM_MODE:
 *                              use the custom tables for both modes
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API int TLG_Get_AGC_Behavior(
    uint32                  base_addr,
    uint32                  *p_agc_mode 
    );


/** Set FM Channel step size.
 *
 * This procedure set the FM channels step size (channel spacing)
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param step_size_hz      fm channel spacing in hz
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API int TLG_Set_FM_StepSize(
    uint32 base_addr        , 
    uint32 step_size_hz
    );

/** Set register table.
 *
 * This procedure set the register table.  Currently only supports 2 AGC tables
 * but it can be used to set other tables as well.
 * This is used to support custom register tables for both AGC in VBI and
 * AGC Always mode.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param table_destination Select which table to set
 *                          TLG_AGC_IN_VBI_TBL: 
 *                              set custom AGC in VBI table
 *                          TLG_AGC_ALWAYS_TBL: 
 *                              set custom AGC always table
 * @param p_tbl             pointer to the new register table
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API int TLG_Set_Table(
    uint32                  base_addr,
    uint32                  table_destination,
    tlg_reg_pair            *p_tbl
    );

/** Get register table.
 *
 * This procedure returns the register table.  Currently only supports 2 AGC tables
 * but it can be used to set other tables as well
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param table_destination Select which table to get
 *                          TLG_AGC_IN_VBI_TBL: 
 *                              set custom AGC in VBI table
 *                          TLG_AGC_ALWAYS_TBL: 
 *                              set custom AGC always table
 * @param p_tbl             pointer to the returning register table
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */
TLGDLL_API int TLG_Get_Table(
    uint32                  base_addr,
    uint32                  table_destination,
    tlg_reg_pair            **p_tbl
    );

/** Set Audio notches to normal running mode.
 *
 * This procedure sets the audio notches to normal running mode
 * After scanning audio channel, this function should be called to
 * set the audio notches back to normal running mode
 * It is embedded in TLGAPP calls. If the application calls TLGAPP 
 * directly to scan or change channel, it does not need to call 
 * this function explicitely
 *
 * @param base_addr         I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 *
 */

TLGDLL_API int TLG_SetAudioNotchesNormal(uint32 base_addr);

/** Write a register value to the TLG1120 if it is changed.
 *
 * This procedure writes the specified register value at the specified
 * register address on the TLG1120 only if it is changed. 
 * If TLG_USE_SHADOW_REGISTERS is defined it
 * will also update the shadowed registers with the written value.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param reg_addr      Address of register to write.
 * @param val           Register value to write.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_ModifyReg(uint32 base_addr, uint32 reg_addr, uint16 val);

typedef struct _tlg_notch_t
{
    uint32  tone_avg_len;
    uint32  tone_bw     ;
    uint32  tone_hold   ;
    uint32  tone_byp    ;
    uint32  tone_en     ;
} tlg_notch_s;
typedef struct _FM_audio_attribute_t
{
    unsigned int    SSI_threshold; 
    unsigned int    step_size_div;
    unsigned int    compare3Notches;
} FM_audio_attribute_s;

/** Set the audio notch parameter.
 *
 * This procedure set the notch parameter.
 * This function is for debugging purpose only.  Do not use.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_notch       notch parameters
 * @param p_fm          fm_audio parameters
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_SetAudioNotchParam(
    uint32 base_addr    , 
    tlg_notch_s          *p_notch,
    FM_audio_attribute_s *p_fm
    );

/** Get the audio notch parameter.
 *
 * This procedure get notch parameter
 * This function is for debugging purpose only.  Do not use.
 *
 * @param base_addr     I2c address for TLG1120 chip.
 * @param p_notch       notch parameters
 * @param p_fm          fm_audio parameters
 * @param read_from_reg  read from registers or from the 
 *                      internal data structure
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call.
 */
TLGDLL_API int TLG_GetAudioNotchParam(
    uint32 base_addr                , 
    tlg_notch_s          *p_notch   ,
    FM_audio_attribute_s *p_fm      ,
    int                  read_from_reg
    );


/** Start channel scan.
 *
 * This procedure sets the current AGC behavior, for tlg1120 rev 1.2 
 * and later only. It temporary changes the AGC behavior to 
 * AGC Always mode during scan.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 */
TLGDLL_API int TLG_StartChannelScan(uint32 base_addr);
/** Stop channel scan.
 *
 * This procedure sets the current AGC behavior, for tlg1120 rev 1.2 
 * and later only. It retores the AGC behavior to 
 * previous AGC mode dafter scan.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 */
TLGDLL_API int TLG_StopChannelScan(uint32 base_addr);

/** Set the TV SSI threshold.
 *
 * This procedure sets the TV SSI threshold to reject the bad channels.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param TV_SSI_Threshold  new TV SSI threshold to reject bad channels.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 */
TLGDLL_API int TLG_SetTV_SSI_ThreshHold(uint32 base_addr, uint16 TV_SSI_Threshold);

/** Check the Alternate standard.
 *
 * This procedure detectst the PAL/SECAM standard in countries with hybrid PAL/SECAM channels.
 * The TLG_SetChnStandard() should always be called with PAL standard first
 * by removing the TLG_VSTD_HYBRID_BIT (bit 15) out of the standard
 * then call TLG_SetAlternateStandard() to detect and switch to the right
 * standard
 *
 * @param base_addr         I2c address for TLG1120 chip.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 * @sa TLG_SetChnStandard
 */
TLGDLL_API int TLG_SetAlternateStandard(uint32 base_addr);

#ifdef TLG_FM_NEW_INTERFACE
/** Check notch power when FM scan
 *
 * This procedure check the notch power of freqOffset based on tlg_fm_scan_set_freq.
 * It will also check the freq: (freqOffset-searchOffset) and (freqOffset+searchOffset).
 *
 * @param base_addr     I2c address for TLG1100 chip.
 * @param freqOffset     the frequency offset related to tlg_fm_scan_set_freq
 * @param searchOffset  search offset from freqOffset. 
 *
 * @retval CHANNEL_EXIST            when (power > 20) && (power > power(freq-searchOffset) ) && (power > power(freq+searchOffset) )
 * @retval CHANNEL_NOT_EXIST    Bad parameter value passed.
 *
 */
TLGDLL_API int TLG_FMScanCheckNotchPower(uint32 base_addr, int32 freqOffset, int32 searchOffset);
#endif /* TLG_FM_NEW_INTERFACE */

#ifdef ENABLE_REGISTER_SETTING_TEST

#define TLG_INIT_TEST   128

/** Set the test call back function.
 *
 * This procedure sets the test call back function to test each checkpoint.
 *
 * @param base_addr         I2c address for TLG1120 chip.
 * @param test_call_back_func Test call back function pointer.
 * @param test_param        Test parameter to pass to the call back function.
 *
 * @retval TLG_ERR_SUCCESS  Call succeeded.
 * @retval TLG_ERR_PARAM    Bad parameter value passed.
 * @retval TLG_ERR_FAIL     Error during call 
 *
 */
typedef int (__stdcall *fp_test_call_back)(int test_num, void *param);

TLGDLL_API int TLG_SetTestCallbackFunc(
    uint32              base_addr, 
    fp_test_call_back   test_call_back,
    void                *test_param);


#define CHECK_POINT(check_point_num)  test_call_back_func(check_point_num, g_test_param)

#else /* ENABLE_REGISTER_SETTING_TEST */

#define CHECK_POINT(check_point_num)

#endif /* ENABLE_REGISTER_SETTING_TEST */

#ifdef __cplusplus
};
#endif

#endif /* TLG_CHIP_TYPE */


#endif /* _TLG1120API_H_ */
