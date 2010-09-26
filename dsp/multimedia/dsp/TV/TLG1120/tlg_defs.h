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
 * $Revision: 1.6 $
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

typedef enum tlg_mode
    {
    TLG_MODE_NONE               = 0x00,  /*  No Mode specified                     */
    TLG_MODE_ANALOG_TV_UNCOMP   = 0x01,  /*  Analog Television mode (uncompressed) */
    TLG_MODE_ANALOG_TV_COMP     = 0x02,  /*  Analog TV mode (compressed)           */
    TLG_MODE_FM_RADIO           = 0x04,  /*  FM Radio mode                         */
    TLG_MODE_DVB_T              = 0x08,  /*  Digital TV (DVB-T)                    */
    TLG_MODE_DVB_H              = 0x10,  /*  Digital TV (DVB-H)                    */
    TLG_MODE_ALL                = 0x1F   /*  All modes                             */
    }tlg_mode_t;


    /********************************************/
    /*  Frequency bandwidth settings supported  */
    /********************************************/

typedef enum    tlg_bw
    {
    TLG_BW_5                    =  5,    /*  5 MHz bandwidth                     */
    TLG_BW_6                    =  6,    /*  6 MHz bandwidth                     */
    TLG_BW_7                    =  7,    /*  7 MHz bandwidth                     */
    TLG_BW_8                    =  8,    /*  8 MHz bandwidth                     */
    TLG_BW_12                   = 12,    /* 12 MHz bandwidth                     */
    TLG_BW_15                   = 15,    /* 15 MHz bandwidth                     */
    TLG_BW_NONE                 =  0
    }tlg_bw_t;


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
    TLG_MACROVISION_OFF   = 0x00,
    TLG_MACROVISION_TYPE1 = 0x01,
    TLG_MACROVISION_TYPE2 = 0x02,
    TLG_MACROVISION_TYPE3 = 0x03,          
};
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
