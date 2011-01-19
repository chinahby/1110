#ifndef _TLG_DEFS_H_
#define _TLG_DEFS_H_
/****************************************************************************/
/*!@file  tlg_defs.h
 *
 * @brief Support for TLG shared video property definitions.
 *
 * TLG Shared definitions for video properties.
 *
 ****************************************************************************/
/* Revision Information:
 *
 * $Revision: 1.13 $
 *
 * Copyright (c) 2006-2008 Telegent Systems, Inc.  All rights reserved.
 ****************************************************************************/


/*****************************************************************************
*
* INCLUDES
*
*****************************************************************************/


/*****************************************************************************
*
* CONSTANTS
*
*****************************************************************************/

    /*****************/
    /*  Video modes  */
    /*****************/

enum tlg_mode
    {
    TLG_MODE_NONE               = 0x00,  /*  No Mode specified                     */
    TLG_MODE_ANALOG_TV_UNCOMP   = 0x01,  /*  Analog Television mode (uncompressed) */
    TLG_MODE_ANALOG_TV_COMP     = 0x02,  /*  Analog TV mode (compressed)           */
    TLG_MODE_FM_RADIO           = 0x04,  /*  FM Radio mode                         */
    TLG_MODE_DVB_T              = 0x08,  /*  Digital TV (DVB-T)                    */
    TLG_MODE_DVB_H              = 0x10,  /*  Digital TV (DVB-H)                    */
    TLG_MODE_ALL                = 0x1F   /*  All modes                             */
    };


    /********************************************/
    /*  Frequency bandwidth settings supported  */
    /********************************************/

enum    tlg_bw
    {
    TLG_BW_5                    =  5,    /*  5 MHz bandwidth                     */
    TLG_BW_6                    =  6,    /*  6 MHz bandwidth                     */
    TLG_BW_7                    =  7,    /*  7 MHz bandwidth                     */
    TLG_BW_8                    =  8,    /*  8 MHz bandwidth                     */
    TLG_BW_12                   = 12,    /* 12 MHz bandwidth                     */
    TLG_BW_15                   = 15,    /* 15 MHz bandwidth                     */
    TLG_BW_NONE                 =  0
    };


    /***********************************/
    /*  Video color formats supported  */
    /***********************************/

enum  tlgAnalogVideoFormat
    {
    TLG_TUNE_VID_FORMAT_NONE      = 0x00000000,
    TLG_TUNE_VID_FORMAT_YCBCR     = 0x00000001,
    TLG_TUNE_VID_FORMAT_YUV       = 0x00000002,
    TLG_TUNE_VID_FORMAT_RGB_565   = 0x00000004,
    TLG_TUNE_VID_FORMAT_ALL       = 0x00000007
    };

enum tlg_macrovision_status
{
    TLG_MACROVISION_OFF         = 0x00,
    TLG_MACROVISION_AGC_ON      = 0x01,
    TLG_MACROVISION_AGC_CS_T1   = 0x02,
    TLG_MACROVISION_AGC_CS_T2   = 0x03
};

enum  tlg_ir_caps
    {
    TLG_IR_CAP_NONE             = 0x00,  /*!<  No IR capability specified           */
    TLG_IR_CAP_RX               = 0x01,  /*!<  Infrared device - receiver           */
    TLG_IR_CAP_TX               = 0x02   /*!<  Infrared device - transmitter        */
    };

enum  tlg_usb_qos_caps
    {
    TLG_USB_QOS_NONE            = 0x00,  /*!<  No USB speeds specified             */
    TLG_USB_QOS_3K_1K           = 0x01,  /*!<  USB QOS - ISO speed support         */
    TLG_USB_QOS_2K_1K           = 0x02,  /*!<  USB QOS - ISO speed support         */
    TLG_USB_BULK                = 0x04   /*!<  USB QOS - BULK speed support        */
    };

enum   tlg_signal_sources
    {
    TLG_TUNE_SIG_SRC_NONE       = 0x00, /*!< Signal source not specified           */
    TLG_TUNE_SIG_SRC_ANTENNA    = 0x01, /*!< Signal src is: Antenna                */
    TLG_TUNE_SIG_SRC_CABLE      = 0x02, /*!< Signal src is: Coax Cable             */
    TLG_TUNE_SIG_SRC_SVIDEO     = 0x04, /*!< Signal src is: S_VIDEO                */
    TLG_TUNE_SIG_SRC_COMPOSITE  = 0x08, /*!< Signal src is: Composite Video        */
    TLG_TUNE_SIG_SRC_ALL        = 0x0F  /*!< MASK of all Sig sources               */
    };

   /***********************************************************************************/
   /*        VIDEO STANDARDS cross reference                                          */
   /*                                                                                 */
   /*        TLG2300 standards supported         Microsoft standards supported        */
   /*        (from:  "tlg2300Api.h")             (from:  "ksmedia.h")                 */
   /*        ===========================         =============================        */
   /*        TLG_VSTD_NTSC_M                     KS_AnalogVideo_NTSC_M                */
   /*        ===                                 KS_AnalogVideo_NTSC_M_J              */
   /*        ===                                 KS_AnalogVideo_NTSC_433              */
   /*        TLG_VSTD_PAL_B                      KS_AnalogVideo_PAL_B                 */
   /*        TLG_VSTD_PAL_B1                     ===                                  */
   /*        TLG_VSTD_PAL_D                      KS_AnalogVideo_PAL_D                 */
   /*        TLG_VSTD_PAL_D1                     ===                                  */
   /*        TLG_VSTD_PAL_G                      KS_AnalogVideo_PAL_G                 */
   /*        TLG_VSTD_PAL_H                      KS_AnalogVideo_PAL_H                 */
   /*        TLG_VSTD_PAL_I                      KS_AnalogVideo_PAL_I                 */
   /*        TLG_VSTD_PAL_K                      ===                                  */
   /*        TLG_VSTD_PAL_M                      KS_AnalogVideo_PAL_M                 */
   /*        TLG_VSTD_PAL_N                      KS_AnalogVideo_PAL_N                 */
   /*        TLG_VSTD_PAL_NC                     KS_AnalogVideo_PAL_N_COMBO           */
   /*        ===                                 KS_AnalogVideo_PAL_60                */
   /*        TLG_VSTD_SECAM_B                    KS_AnalogVideo_SECAM_B               */
   /*        TLG_VSTD_SECAM_B1                   ===                                  */
   /*        TLG_VSTD_SECAM_D                    KS_AnalogVideo_SECAM_D               */
   /*        TLG_VSTD_SECAM_D1                   ===                                  */
   /*        TLG_VSTD_SECAM_G                    KS_AnalogVideo_SECAM_G               */
   /*        TLG_VSTD_SECAM_H                    KS_AnalogVideo_SECAM_H               */
   /*        TLG_VSTD_SECAM_I                    ===                                  */
   /*        TLG_VSTD_SECAM_K                    KS_AnalogVideo_SECAM_K               */
   /*        ===                                 KS_AnalogVideo_SECAM_K1              */
   /*        TLG_VSTD_SECAM_L                    KS_AnalogVideo_SECAM_L               */
   /*        ===                                 KS_AnalogVideo_SECAM_L1              */
   /*        TLG_VSTD_SECAM_M                    ===                                  */
   /*        TLG_VSTD_SECAM_N                    ===                                  */
   /*        TLG_VSTD_SECAM_NC                   ===                                  */
   /*                                                                                 */
   /***********************************************************************************/

enum   tlg__AnalogVideoStandard
    {
    TLG_TUNE_VSTD_NONE          = 0x00000000,
    TLG_TUNE_VSTD_NTSC_M        = 0x00000001,
    TLG_TUNE_VSTD_NTSC_M_J      = 0x00000002,     /*  TLG2300: unsupported ? (treat M_J as M ?)   */
    TLG_TUNE_VSTD_NTSC_433      = 0x00000004,     /*  TLG2300: unsupported    */
    TLG_TUNE_VSTD_PAL_B         = 0x00000010,
    TLG_TUNE_VSTD_PAL_B1        = 0x00000010,     /*  MS:      unsupported ?  */
    TLG_TUNE_VSTD_PAL_D         = 0x00000020,
    TLG_TUNE_VSTD_PAL_D1        = 0x00000020,     /*  MS:      unsupported ?  */
    TLG_TUNE_VSTD_PAL_G         = 0x00000040,
    TLG_TUNE_VSTD_PAL_H         = 0x00000080,
    TLG_TUNE_VSTD_PAL_I         = 0x00000100,
    TLG_TUNE_VSTD_PAL_K         = 0x01000000,     /* MS:      unsupported    */
    TLG_TUNE_VSTD_PAL_M         = 0x00000200,
    TLG_TUNE_VSTD_PAL_N         = 0x00000400,
    TLG_TUNE_VSTD_PAL_NC        = 0x00100000,
    TLG_TUNE_VSTD_PAL_60        = 0x00000800,     /* TLG2300: unsupported    */
    TLG_TUNE_VSTD_SECAM_B       = 0x00001000,
    TLG_TUNE_VSTD_SECAM_B1      = 0x00001000,     /* MS:      unsupported ?  */
    TLG_TUNE_VSTD_SECAM_D       = 0x00002000,
    TLG_TUNE_VSTD_SECAM_D1      = 0x00002000,     /* MS:      unsupported ?  */
    TLG_TUNE_VSTD_SECAM_G       = 0x00004000,
    TLG_TUNE_VSTD_SECAM_H       = 0x00008000,
    TLG_TUNE_VSTD_SECAM_I       = 0x02000000,     /* MS:      unsupported    */
    TLG_TUNE_VSTD_SECAM_K       = 0x00010000,
    TLG_TUNE_VSTD_SECAM_K1      = 0x00020000,     /* TLG2300: unsupported (treat K1 as K ?)   */
    TLG_TUNE_VSTD_SECAM_L       = 0x00040000,
    TLG_TUNE_VSTD_SECAM_L1      = 0x00080000,     /* TLG2300: unsupported (treat L1 as L ?)   */
    TLG_TUNE_VSTD_SECAM_M       = 0x04000000,     /* MS:      unsupported    */
    TLG_TUNE_VSTD_SECAM_N       = 0x08000000,     /* MS:      unsupported    */
    TLG_TUNE_VSTD_SECAM_NC      = 0x10000000,     /* MS:      unsupported    */
    TLG_TUNE_VSTD_SUPPORTED     = 0x1f1ff7f3,
    TLG_TUNE_VSTD_ALL           = 0x1f1ff7f3
    };

enum	tlg__AnalogAudioStandard
	{
	TLG_TUNE_ASTD_NONE          = 0x00000000,
	TLG_TUNE_ASTD_A2			= 0x00000001,
	TLG_TUNE_ASTD_NICAM			= 0x00000002,
	TLG_TUNE_ASTD_EIAJ			= 0x00000004,
	TLG_TUNE_ASTD_BTSC			= 0x00000008,
	TLG_TUNE_ASTD_FM_US			= 0x00000010,
	TLG_TUNE_ASTD_FM_EUR		= 0x00000020,
	TLG_TUNE_ASTD_ALL           = 0x0000003f
	};

enum  tlg_ExtAudioSupport
    {
    TLG_EXT_AUDIO_NONE              = 0x00000000,
    TLG_EXT_AUDIO_LR                = 0x00000001
    };

enum    tuner_AnalogVideoSpecialModes
{
    TLG_SPECIAL_MODES_NONE          = 0x00000000,
    TLG_SPECIAL_TEST_PATT_LOGO      = 0x00000001,
    TLG_SPECIAL_TEST_PATT_COLOR_BAR = 0x00000002,
    TLG_SPECIAL_MACROVISION         = 0x00000010,
    TLG_SPECIAL_VBI_DATA            = 0x00000100,
    TLG_SPECIAL_VCR_TIMING          = 0x00001000,
    TLG_SPECIAL_ALL                 = 0x00001113
};

enum  tune_atv_audio_mode_caps
    {
    TUNER_TVAUDIO_MODE_NONE         = 0x00000000,
    TUNER_TVAUDIO_MODE_MONO         = 0x00000001,
    TUNER_TVAUDIO_MODE_STEREO       = 0x00000002,
    TUNER_TVAUDIO_MODE_LANG_A       = 0x00000010,   /* Primary language    */
    TUNER_TVAUDIO_MODE_LANG_B       = 0x00000020,   /* 2nd avail language  */
    TUNER_TVAUDIO_MODE_LANG_C       = 0x00000040    /* 3rd avail language  */
};

enum      tune_atv_vid_res_caps
{
    TLG_TUNE_VID_RES_NONE       = 0x00000000,
    TLG_TUNE_VID_RES_720        = 0x00000001,
    TLG_TUNE_VID_RES_704        = 0x00000002,
    TLG_TUNE_VID_RES_360        = 0x00000004,
    TLG_TUNE_VID_RES_ALL        = 0x00000007
};

enum   tuner_atv_tp
{
    TLG_TP_NONE         = 0x00,     /*!< Test Pattern: none               */
    TLG_TP_OFF          = 0x01,     /*!< Test Pattern is: Off             */
    TLG_TP_COLOR_BARS   = 0x02,     /*!< Test Pattern is: color bars      */
    TLG_TP_LOGO         = 0x04      /*!< Test Pattern is: Telegent logo   */
};

enum    tuner_atv_audio_rates
{
    ATV_AUDIO_RATE_NONE       = 0x00,     /*!< Audio not supported  */
    ATV_AUDIO_RATE_32K        = 0x01,     /*!< Audio rate = 32 KHz  */
    ATV_AUDIO_RATE_48K        = 0x02,     /*!< Audio rate = 48 KHz  */
    ATV_AUDIO_RATE_31_25K     = 0x04      /*!< Audio rate = 31.25KHz*/
};

enum    tlg_tune_srv_cmd         /* Play Service -- "Set" command   */
{
    TLG_TUNE_PLAY_SVC_START   = 1,
    TLG_TUNE_PLAY_SVC_STOP
 };


enum     tlg_tune_srv_state      /* Play Service -- "Get" command   */
{
    TLG_TUNE_PLAY_SVC_ACTIVE          = 1,
    TLG_TUNE_PLAY_SVC_ACTIVE_PENDING,
    TLG_TUNE_PLAY_SVC_INACTIVE,
    TLG_TUNE_PLAY_SVC_INACTIVE_PENDING
};

enum    tlg_tune_scan_srv_cmd    /* Scan Service -- "Set" command   */
{
    TLG_TUNE_SCAN_SVC_START   = 1,
    TLG_TUNE_SCAN_SVC_STOP
 };

enum tlg_modulation_type                     /*   DVB-T modulation type  */
{
    TLG_MODULATION_NONE    =  0x00000000,
    TLG_MODULATION_16QAM   =  0x00000001,
    TLG_MODULATION_64QAM   =  0x00000002,
    TLG_MODULATION_QPSK    =  0x00000004,
    TLG_MODULATION_ALL     =  0x00000007
};

enum tlg_dvbt_guard_interval
{
    TLG_GUARD_NONE        =  0x00000000,
    TLG_GUARD_1_32        =  0x00000001,
    TLG_GUARD_1_16        =  0x00000002,
    TLG_GUARD_1_8         =  0x00000004,
    TLG_GUARD_1_4         =  0x00000008,
    TLG_GUARD_ALL         =  0x0000000F
};

enum tlg_dvbt_hierarchy_alpha
{
    TLG_HIER_ALPHA_NONE   =  0x00000000,
    TLG_HIER_ALPHA_1      =  0x00000001,
    TLG_HIER_ALPHA_2      =  0x00000002,
    TLG_HIER_ALPHA_4      =  0x00000004,
    TLG_HIER_ALPHA_ALL    =  0x00000007
};

enum tlg_dvbt_code_rate
{
    TLG_CODE_RATE_NONE    =  0x00000000,
    TLG_CODE_RATE_1_2     =  0x00000001,
    TLG_CODE_RATE_2_3     =  0x00000002,
    TLG_CODE_RATE_3_4     =  0x00000004,
    TLG_CODE_RATE_5_6     =  0x00000008,
    TLG_CODE_RATE_7_8     =  0x00000010,
    TLG_CODE_RATE_ALL     =  0x0000001F          /*  Code Rates supported  */
};

#if 0//NOT_USED
enum tlg_dvbt_FEC_method
{
    TLG_FEC_METHOD_NONE   =  0x00000000,
    TLG_FEC_VITERBI       =  0x00000001,
    TLG_FEC_RS_204_188    =  0x00000002,
    TLG_FEC_ALL           =  0x00000003
};
#endif

enum tlg_dvbt_transmission_mode
{
    TLG_XMIT_MODE_NONE    =  0x00000000,
    TLG_XMIT_MODE_2K      =  0x00000001,
    TLG_XMIT_MODE_8K      =  0x00000002,
    TLG_XMIT_MODE_ALL     =  0x00000003
};




    /**
    *************************************************/
    /* VERSION Definitions                              */
    /***************************************************/
#define TLG_VERS_MAJOR_MASK    (0x0000001f) /* 5 bits */
#define TLG_VERS_MINOR_MASK    (0x0000007f) /* 7 bits */
#define TLG_VERS_PATCH_MASK    (0x000000ff) /* 8 bits */
#define TLG_VERS_BUILD_MASK    (0x00000fff) /* 12 bits */
#define TLG_VERS_MAJOR_SHIFT   (27)
#define TLG_VERS_MINOR_SHIFT   (20)
#define TLG_VERS_PATCH_SHIFT   (12)
#define TLG_VERS_BUILD_SHIFT   (0)
#define TLG_VERS_MAJOR(v)      (((v)>> TLG_VERS_MAJOR_SHIFT) \
                                   & TLG_VERS_MAJOR_MASK) /* bits 27..31 */
#define TLG_VERS_MINOR(v)      (((v)>> TLG_VERS_MINOR_SHIFT) \
                                   & TLG_VERS_MINOR_MASK) /* bits 20..26 */
#define TLG_VERS_PATCH(v)      (((v)>> TLG_VERS_PATCH_SHIFT) \
                                   & TLG_VERS_PATCH_MASK) /* bits 12..19 */
#define TLG_VERS_BUILD(v)      (((v)>> TLG_VERS_BUILD_SHIFT) \
                                   & TLG_VERS_BUILD_MASK) /* bits 0..11 */

#if 1

    /***************************************************/
    /* CHIP Definitions                                */
    /***************************************************/
/*****************************************************************************
* CHIP CONFIG CONSTANTS
*
* The following are chip version configuration constants and should not be
* changed.
*****************************************************************************/
#define TLG_CHIP_TYPE_TLG1100_1     (0x0001)
#define TLG_CHIP_TYPE_TLG1150_1		(0x0002)
#define TLG_CHIP_TYPE_TLG1155_1		(0x0003)
#define TLG_CHIP_TYPE_TLG2300_1		(0x0004)
#define TLG_CHIP_TYPE_TLG1120_1		(0x0005)
#define TLG_CHIP_TYPE_TLG1170_1		(0x0006)

#define TLG_CHIP_REV_1              (0x00000001)
#define TLG_CHIP_REV_2              (0x00000002)
#define TLG_CHIP_REV_3              (0x00000003)
/*****************************************************************************
* CHIP CONFIG CONSTANTS
*
* The following are chip version configuration constants and should not be
* changed.
*****************************************************************************/
#define TLG_CHIP_TYPE_MASK          (0xffff0000)
#define TLG_CHIP_TYPE_SHIFT         (16)

#define TLG_CHIP_REV_MASK           (0x0000ff00)
#define TLG_CHIP_REV_SHIFT          (8)
#define TLG_CHIP_PATCH_MASK         (0x000000ff)
#define TLG_CHIP_PATCH_SHIFT        (0)

#define TLG_CHIP_VER_CFG(t, r)      ( ((t) << TLG_CHIP_TYPE_SHIFT) \
                                    | ((r) << TLG_CHIP_REV_SHIFT))

#define TLG2300_VERS_1_1            \
                    TLG_CHIP_VER_CFG(TLG_CHIP_TYPE_TLG2300_1,TLG_CHIP_REV_1)

#define TLG2300_VERS_1_2            \
                    TLG_CHIP_VER_CFG(TLG_CHIP_TYPE_TLG2300_1,TLG_CHIP_REV_2)

#define TLG2300_VERS_1_3            \
                    TLG_CHIP_VER_CFG(TLG_CHIP_TYPE_TLG2300_1,TLG_CHIP_REV_3)

#define TLG_NO_VERS    (TLG_CHIP_TYPE_MASK|TLG_CHIP_REV_MASK)


/** Chip version configuration.
*
* TLG_CHIP_VERS defines the latest type and version of chip that the current
* code compile will support. It uses the defines TLG_CHIP_TYPE and
* TLG_CHIP_REV which are declared in tlgConfig.h.
*
*/
#define TLG_CHIP_VERS         TLG_CHIP_VER_CFG(TLG_CHIP_TYPE, TLG_CHIP_REV)

/*
 * Succeed only if current chip type is equal to the chip type specified and
 * the currently defined rev is >= the one specified
 */
#define TLG_CHIP_CMP_GE(v) \
     ( (((v) & TLG_CHIP_TYPE_MASK) >> TLG_CHIP_TYPE_SHIFT) == TLG_CHIP_TYPE \
     && TLG_CHIP_REV >= (((v) & TLG_CHIP_REV_MASK)  >> TLG_CHIP_REV_SHIFT))

#define TLG_CHIP_CMP_EQ(v) \
    ( (((v) & TLG_CHIP_TYPE_MASK) >> TLG_CHIP_TYPE_SHIFT) == TLG_CHIP_TYPE \
    && (((v) & TLG_CHIP_REV_MASK)  >> TLG_CHIP_REV_SHIFT)  == TLG_CHIP_REV)

#endif
#endif  /*  _TLG_DEFS_H_  */
