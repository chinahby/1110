/* linesize(132)
** pagesize(60)
** title("Dual Mode Subscriber Station")
** subtitle("Adie Services")
*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     ADIE   S E R V I C E S

GENERAL DESCRIPTION
  This file contains the functionality to configure Adie

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c) 1992 - 2008 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/adie_services.c#19 $ $DateTime: 2010/03/30 07:46:36 $ $Author: kmodak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/10   knm     Featurized the code for FM support on handset & LineOut
02/16/10   knm     Added FM support on Sc2x.
10/26/09   knm     Added CMI related changes again.
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/06/09   sj      Added SCMM specific changes
09/16/09   gs      Added support for FM over Lineout, Handset.
                   Ported the Headset, BT FM code from ULC
06/03/09   bk      Added Macros __NON_DEMAND_PAGED_FUNCTION__ and
                   __NON_DEMAND_PAGED_FUNCTION_END__ for non demand
                   pageable functions.
12/10/08   knm     Added Adie services sleep APIs.
11/17/08   knm     Added FM recording code and adie_services_init() API.
11/03/08   dp      Correcting featurization issue for FM over BT
10/22/08   dp      FM over BT AG changes
10/22/08   kk      Fixed compiler error for ULC due to changes in version 6.
10/08/08   kk      Fixed compiler warnings.
09/30/08   knm     Fixed compilation errors and warnings.
09/15/08   knm     Added source code for Aux Line In(FM) volume control
08/01/08   knm     Added fix to remove compiler warnings.
07/21/08   knm     Fixed compilation errors for non-ulc targets
06/20/08   knm     Created Adie Services APIs and Defintions

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"             /* Target specific definitions            */
#include "comdef.h"             /* Definitions for byte, word, etc.       */

#include "msmaud.h"
#include "adie_services.h"
#ifndef MSMAUD_SCMM
#include "adie.h"
#include "adie_sbi.h"
#endif /* MSMAUD_SCMM */
#include "msg.h"                /* Message logging macro                   */
#include "err.h"                /* Error logging macro                     */
#ifdef FEATURE_CMI
#error code not present
#else
#include "clk.h"
#endif

#ifdef MSMAUD_PMIC_CONFIG
#include "pm.h"
#include "pmapp.h"
#endif /* MSMAUD_PMIC_CONFIG */

#if defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_CONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DECONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_CONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DECONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_CONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DECONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_CONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DECONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_CONFIG_SEQ) && \
    defined(MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DECONFIG_SEQ)

#define ADIE_SERVICES_AUX_LINE_IN
/* FM over BT AG */
#if defined(MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ)

#define ADIE_SERVICES_FM_OVER_BT_AG

#endif /* MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ */

#endif

#ifdef ADIE_SERVICES_AUX_LINE_IN

#define VOLUME_LEVEL_MAX  8

/* default volume on AuxPGA analog loopback path */
static uint8 aux_volume = 0;

/* Global Variables */

/* config and de-config spec on FM over Handset */
const adie_codec_reg_type adie_aux_line_in_handset_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_CONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_handset_config =
{
  sizeof(adie_aux_line_in_handset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_CONFIG_SIZE,
  adie_aux_line_in_handset_config
};

const adie_codec_reg_type adie_aux_line_in_handset_deconfig[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DECONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_handset_deconfig =
{
  sizeof(adie_aux_line_in_handset_deconfig)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_DECONFIG_SIZE,
  adie_aux_line_in_handset_deconfig
};

/* config and de-config spec on FM over Line-out */
const adie_codec_reg_type adie_aux_line_in_line_out_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_CONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_line_out_config =
{
  sizeof(adie_aux_line_in_line_out_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_CONFIG_SIZE,
  adie_aux_line_in_line_out_config
};

const adie_codec_reg_type adie_aux_line_in_line_out_deconfig[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DECONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_line_out_deconfig =
{
  sizeof(adie_aux_line_in_line_out_deconfig)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_DECONFIG_SIZE,
  adie_aux_line_in_line_out_deconfig
};

const adie_codec_reg_type adie_aux_line_in_stereo_headset_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_CONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_stereo_headset_config =
{
  sizeof(adie_aux_line_in_stereo_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DELAY_CONFIG_SIZE,
  adie_aux_line_in_stereo_headset_config
};

const adie_codec_reg_type adie_aux_line_in_stereo_headset_deconfig[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DECONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_stereo_headset_deconfig =
{

  sizeof(adie_aux_line_in_stereo_headset_deconfig)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_HEADSET_DELAY_DECONFIG_SIZE,
  adie_aux_line_in_stereo_headset_deconfig
};

const adie_codec_reg_type adie_aux_line_in_speaker_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_CONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_speaker_config =
{
  sizeof(adie_aux_line_in_speaker_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DELAY_CONFIG_SIZE,
  adie_aux_line_in_speaker_config
};

const adie_codec_reg_type adie_aux_line_in_speaker_deconfig[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DECONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_speaker_deconfig =
{

  sizeof(adie_aux_line_in_speaker_deconfig)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_SPEAKER_NO_MIC_DELAY_DECONFIG_SIZE,
  adie_aux_line_in_speaker_deconfig
};

const adie_codec_reg_type adie_aux_line_in_stereo_usb_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_CONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_stereo_usb_config =
{
  sizeof(adie_aux_line_in_stereo_usb_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DELAY_CONFIG_SIZE,
  adie_aux_line_in_stereo_usb_config
};

const adie_codec_reg_type adie_aux_line_in_stereo_usb_deconfig[] =
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DECONFIG_SEQ;

const adie_codec_config_type adie_codec_aux_line_in_stereo_usb_deconfig =
{

  sizeof(adie_aux_line_in_stereo_usb_deconfig)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINE_IN_STEREO_USB_DELAY_DECONFIG_SIZE,
  adie_aux_line_in_stereo_usb_deconfig
};

/* FM over BT AG */
#if defined (ADIE_SERVICES_FM_OVER_BT_AG)
const adie_codec_reg_type adie_aux_stereo_bt_headset_config[] =
    MSMAUD_ADIE_CODEC_FM_BT_AG_CONFIG_SEQ;
/*  MSMAUD_ADIE_CODEC_AUX_STEREO_BT_HEADSET_CONFIG_SEQ; */

const adie_codec_config_type adie_codec_aux_stereo_bt_headset_config =
{
  sizeof(adie_aux_stereo_bt_headset_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_FM_BT_AG_DELAY_CONFIG_SIZE,
  adie_aux_stereo_bt_headset_config
};
#endif /* ADIE_SERVICES_FM_OVER_BT_AG */


const adie_codec_reg_type adie_aux_headset_vol0_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL0_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol1_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL1_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol2_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL2_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol3_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL3_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol4_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL4_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol5_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL5_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol6_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL6_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_headset_vol7_config[] =
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL7_CONFIG_SEQ;

const adie_codec_config_type
      adie_codec_aux_headset_vol_config[VOLUME_LEVEL_MAX] =
{
  {
  sizeof(adie_aux_headset_vol0_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol0_config
  },
  {
  sizeof(adie_aux_headset_vol1_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol1_config
  },
  {
  sizeof(adie_aux_headset_vol2_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol2_config
  },
  {
  sizeof(adie_aux_headset_vol3_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol3_config
  },
  {
  sizeof(adie_aux_headset_vol4_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol4_config
  },
  {
  sizeof(adie_aux_headset_vol5_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol5_config
  },
  {
  sizeof(adie_aux_headset_vol6_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol6_config
  },
  {
  sizeof(adie_aux_headset_vol7_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HEADSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_headset_vol7_config
  }
};

const adie_codec_reg_type adie_aux_speaker_vol0_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL0_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol1_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL1_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol2_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL2_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol3_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL3_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol4_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL4_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol5_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL5_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol6_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL6_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_speaker_vol7_config[] =
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL7_CONFIG_SEQ;

const adie_codec_config_type
      adie_codec_aux_speaker_vol_config[VOLUME_LEVEL_MAX] =
{
  {
  sizeof(adie_aux_speaker_vol0_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol0_config
  },
  {
  sizeof(adie_aux_speaker_vol1_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol1_config
  },
  {
  sizeof(adie_aux_speaker_vol2_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol2_config
  },
  {
  sizeof(adie_aux_speaker_vol3_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol3_config
  },
  {
  sizeof(adie_aux_speaker_vol4_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol4_config
  },
  {
  sizeof(adie_aux_speaker_vol5_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol5_config
  },
  {
  sizeof(adie_aux_speaker_vol6_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol6_config
  },
  {
  sizeof(adie_aux_speaker_vol7_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_SPEAKER_VOL_DELAY_CONFIG_SIZE,
  adie_aux_speaker_vol7_config
  }
};

/* vol config for FM on Handset */
const adie_codec_reg_type adie_aux_handset_vol0_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL0_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol1_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL1_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol2_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL2_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol3_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL3_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol4_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL4_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol5_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL5_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol6_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL6_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_handset_vol7_config[] =
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL7_CONFIG_SEQ;

const adie_codec_config_type
      adie_codec_aux_handset_vol_config[VOLUME_LEVEL_MAX] =
{
  {
  sizeof(adie_aux_handset_vol0_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol0_config
  },
  {
  sizeof(adie_aux_handset_vol1_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol1_config
  },
  {
  sizeof(adie_aux_handset_vol2_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol2_config
  },
  {
  sizeof(adie_aux_handset_vol3_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol3_config
  },
  {
  sizeof(adie_aux_handset_vol4_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol4_config
  },
  {
  sizeof(adie_aux_handset_vol5_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol5_config
  },
  {
  sizeof(adie_aux_handset_vol6_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol6_config
  },
  {
  sizeof(adie_aux_handset_vol7_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE,
  adie_aux_handset_vol7_config
  }
};

/* vol config for FM on Line out */
const adie_codec_reg_type adie_aux_lineout_vol0_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL0_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol1_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL1_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol2_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL2_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol3_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL3_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol4_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL4_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol5_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL5_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol6_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL6_CONFIG_SEQ;

const adie_codec_reg_type adie_aux_lineout_vol7_config[] =
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL7_CONFIG_SEQ;

const adie_codec_config_type
      adie_codec_aux_lineout_vol_config[VOLUME_LEVEL_MAX] =
{
  {
  sizeof(adie_aux_lineout_vol0_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol0_config
  },
  {
  sizeof(adie_aux_lineout_vol1_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol1_config
  },
  {
  sizeof(adie_aux_lineout_vol2_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol2_config
  },
  {
  sizeof(adie_aux_lineout_vol3_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol3_config
  },
  {
  sizeof(adie_aux_lineout_vol4_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol4_config
  },
  {
  sizeof(adie_aux_lineout_vol5_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol5_config
  },
  {
  sizeof(adie_aux_lineout_vol6_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol6_config
  },
  {
  sizeof(adie_aux_lineout_vol7_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE,
  adie_aux_lineout_vol7_config
  }
};

const adie_codec_reg_type adie_line_in_stereo_tx_config[] =
  MSMAUD_ADIE_CODEC_LINE_IN_STEREO_TX_CONFIG_SEQ;

const adie_codec_config_type adie_codec_line_in_stereo_tx_config =
{
  sizeof(adie_line_in_stereo_tx_config)/sizeof(adie_codec_reg_type),
  MSMAUD_ADIE_CODEC_LINE_IN_STEREO_TX_DELAY_CONFIG_SIZE,
  adie_line_in_stereo_tx_config
};

#endif /* ADIE_SERVICES_AUX_LINE_IN */

/* set the default adie sleep status as false */
static boolean adie_services_adie_sleep_status = FALSE;

#ifndef MSMAUD_SCMM

/* Clock callback structure that implements final setting in ADIE codec.
*/
	#ifdef FEATURE_CMI
#error code not present
	#else
    		clk_cb_type adie_services_clk_cb;
	#endif


/* Pointer to the last ADIE codec configuration structure.
*/
static adie_codec_config_type *adie_services_last_config = NULL;

/*===========================================================================
FUNCTION adie_services_init

DESCRIPTION
  Initialize ADIE services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void adie_services_init (void)
{
   	#ifdef FEATURE_CMI
#error code not present
	#else
    		clk_def(&adie_services_clk_cb);
	#endif
}

/*===========================================================================
FUNCTION adie_services_set_final

DESCRIPTION
  Set final setting in ADIE codec.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
void adie_services_set_final (
  int4 ms_interval
  /*lint -esym(715,ms_interval) Tell Lint we know ms_interval parameter
  ** is not used */
) {
  adie_codec_config_type *adie_codec = NULL;

  /* Enabling codec output before enabling PA.
     DSP should be producing valid output at this stage.
  */
  adie_codec_enable(ADIE_ENABLE);

  adie_codec = adie_services_last_config;

  if(adie_codec != NULL  &&  adie_codec->num_to_delay) {
    adie_codec_config(adie_codec->num_to_delay,
            (adie_codec_reg_type *)&( adie_codec->val[adie_codec->size -
                                      adie_codec->num_to_delay] ) );
  }
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/*===========================================================================
FUNCTION adie_servies_do_wakeup

DESCRIPTION
  Wake up and configure the Analog Die before using the codec.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
boolean adie_services_do_wakeup(adie_codec_config_type *adie_codec)
{
boolean  ret_val = TRUE;

  /* Wake up the Analog Die if the codec in use is the internal codec. */
  if ( adie_codec != NULL && adie_codec->size != 0 ) {

#ifndef FEATURE_HWTC
    adie_wakeup (ADIE_ENABLE, ADIE_CODEC_BLK);
#endif /* FEATURE_HWTC */

    /* disabling codec output before changing configuration */
    adie_codec_enable(ADIE_DISABLE);

    /* Do all ADIE codec settings except for the last one */
    adie_codec_config(adie_codec->size - adie_codec->num_to_delay,
                     (adie_codec_reg_type *) adie_codec->val);

    /* reconfig. done, enabling codec output */
    adie_codec_enable(ADIE_ENABLE);

    adie_services_last_config = adie_codec;

    if(adie_codec->num_to_delay > 0)
    {
      #ifdef FEATURE_CMI
#error code not present
	  #else
      clk_reg(
        &adie_services_clk_cb,     /* Call-back structure to register  */
        adie_services_set_final,   /* Routine called by Clock Services */
        10,                        /* Call routine after 10ms          */
        0L,                        /* De-register after first call     */
        FALSE );                   /* Do not call on a periodic basis  */
        #endif
    }

  } else {
    ret_val = FALSE;
  }
  return(ret_val);
} /* end adie_services_do_wakeup */

#endif /* MSMAUD_SCMM */

/*===========================================================================

FUNCTION adie_services_configure_aux_line_in_on

DESCRIPTION
  Configure the Analog Die for Line In loopback Audio.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean adie_services_configure_aux_line_in_on
(
  adie_aux_line_in_device_type device
)
{
#ifdef ADIE_SERVICES_AUX_LINE_IN
  adie_codec_config_type *adie_codec = NULL;

  switch(device)
  {
    case ADIE_AUX_LINE_IN_HANDSET:
      #if MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_CONFIG_SIZE != 0xFF
      /* Get the ADIE codec config for handset*/
      MSG_MED("ADIE Line In path routed to handset \n", 0, 0, 0);
      adie_codec = (adie_codec_config_type *)
                     &adie_codec_aux_line_in_handset_config;
      #else
      MSG_ERROR("FM on Handset is not supported",0,0,0);
      #endif /* MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_CONFIG_SIZE */
      break;

    case ADIE_AUX_LINE_IN_HEADSET:

      MSG_MED("ADIE Line In path routed to HEADSET \n", 0, 0, 0);

      #ifdef MSMAUD_PMIC_CONFIG

      pm_hsed_enable(PM_HSED_ENABLE_PWM_TCXO);

      #ifdef MSMAUD_GRND_REFERENCED_CAPLESS_MODE
      /* configure NCP voltage */
      pm_vreg_control(PM_ON_CMD, PM_VREG_NCP_M);
      pm_vreg_set_level(PM_VREG_NCP_ID, 1800);
      #endif /* MSMAUD_GRND_REFERENCED_CAPLESS_MODE */
      #endif /* MSMAUD_PMIC_CONFIG */

      /* Get the ADIE codec config for headset */
      adie_codec = (adie_codec_config_type *)
                           &adie_codec_aux_line_in_stereo_headset_config;
      break;

    case ADIE_AUX_LINE_IN_SPEAKER:
      /* Get the ADIE codec config for speaker*/
      MSG_MED("ADIE Line In path routed to SPEAKER \n", 0, 0, 0);
      adie_codec = (adie_codec_config_type *)
                                  &adie_codec_aux_line_in_speaker_config;
      break;

    case ADIE_AUX_LINE_IN_LINEOUT:
      #if MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_CONFIG_SIZE != 0xFF
      MSG_MED("ADIE Line In path routed to LineOut \n", 0, 0, 0);

      /* Get the ADIE codec config for headset */
      adie_codec = (adie_codec_config_type *)
                           &adie_codec_aux_line_in_line_out_config;
      #else
      MSG_ERROR("FM on LineOut is not supported",0,0,0);
      #endif /* MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_CONFIG_SIZE */
      break;

    case ADIE_AUX_LINE_IN_USB:
      /* Get the ADIE codec config for speaker*/
      MSG_MED("ADIE Line In path routed to USB \n", 0, 0, 0);
      adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_line_in_stereo_usb_config;
      break;

    default:
      /* Print the error massage */
      MSG_ERROR("ADIE Line in routing to device :%d not available \n",
                 device, 0, 0);
      return FALSE;
  }
  return (adie_services_do_wakeup(adie_codec));
#else
  /* Print the error massage */
  MSG_ERROR(" Feature ADIE_SERVICES_AUX_LINE_IN is not available \n", 0, 0, 0);

  return (FALSE);
#endif /* ADIE_SERVICES_AUX_LINE_IN */
} /* end adie_services_configure_aux_line_in_on */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/


/*===========================================================================

FUNCTION adie_services_configure_aux_line_in_off

DESCRIPTION
  Deconfigure the Analog Die for Line In loopback Audio.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean adie_services_configure_aux_line_in_off
(
  adie_aux_line_in_device_type device
)
{
#ifdef ADIE_SERVICES_AUX_LINE_IN
  adie_codec_config_type *adie_codec = NULL;

  switch(device)
  {
    case ADIE_AUX_LINE_IN_HANDSET:
      #if MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_DECONFIG_SIZE != 0xFF
      MSG_MED("ADIE Line In path derouted from HANDSET \n", 0, 0, 0);
      /* Get the ADIE codec deconfig for handset */
      adie_codec = (adie_codec_config_type *)
                          &adie_codec_aux_line_in_handset_deconfig;
      #else
      MSG_ERROR("FM on Handset is not supported",0,0,0);
      #endif /* MSMAUD_ADIE_CODEC_AUX_LINE_IN_HANDSET_DELAY_DECONFIG_SIZE */
      break;

    case ADIE_AUX_LINE_IN_HEADSET:
      MSG_MED("ADIE Line In path derouted from HEADSET \n", 0, 0, 0);
      /* Get the ADIE codec deconfig for headset */
      adie_codec = (adie_codec_config_type *)
                          &adie_codec_aux_line_in_stereo_headset_deconfig;
      break;

    case ADIE_AUX_LINE_IN_SPEAKER:
      MSG_MED("ADIE Line In path derouted from SPEAKER \n", 0, 0, 0);
      /* Get the ADIE codec deconfig for speaker*/
      adie_codec = (adie_codec_config_type *)
                          &adie_codec_aux_line_in_speaker_deconfig;
      break;

    case ADIE_AUX_LINE_IN_LINEOUT:
      #if MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_DECONFIG_SIZE != 0xFF
      MSG_MED("ADIE Line In path derouted from Lineout \n", 0, 0, 0);
      /* Get the ADIE codec deconfig for lineout */
      adie_codec = (adie_codec_config_type *)
                          &adie_codec_aux_line_in_line_out_deconfig;
      #else
      MSG_ERROR("FM on LineOut is not supported",0,0,0);
      #endif /* MSMAUD_ADIE_CODEC_AUX_LINE_IN_LINE_OUT_DELAY_DECONFIG_SIZE */
      break;

    case ADIE_AUX_LINE_IN_USB:
      MSG_MED("ADIE Line In path derouted from USB \n", 0, 0, 0);
      /* Get the ADIE codec deconfig for speaker*/
      adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_line_in_stereo_usb_deconfig;
      break;

    default:
      /* Print the error massage */
      MSG_ERROR(" ADIE Line in derouting from device :%d not available \n",
                      device, 0, 0);
      return FALSE;
  }

  return (adie_services_do_wakeup(adie_codec));
#else
  /* Print the error massage */
  MSG_ERROR("Feature ADIE_SERVICES_AUX_LINE_IN is not available \n", 0, 0, 0);

  return (FALSE);
#endif /* ADIE_SERVICES_AUX_LINE_IN */
} /* end adie_services_configure_aux_line_in_off */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION adie_services_configure_aux_path

DESCRIPTION
  Configure Aux Path

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean adie_services_configure_aux_path
(
  adie_aux_line_in_device_type device
)
{
#if defined(ADIE_SERVICES_AUX_LINE_IN)
  adie_codec_config_type *adie_codec = NULL;

  adie_services_set_aux_line_in_vol(device,aux_volume);

  /* Configure adie for Line In audio recording */
  adie_codec = (adie_codec_config_type *)
               &adie_codec_line_in_stereo_tx_config;

  adie_codec_config(adie_codec->size,
                    (adie_codec_reg_type *) adie_codec->val);

  return (TRUE);
#else
  /* Print the error massage */
  MSG_ERROR("Feature ADIE_SERVICES_AUX_LINE_IN is not available \n", 0, 0, 0);

  return (FALSE);
#endif /* ADIE_SERVICES_AUX_LINE_IN */

} /* end adie_services_configure_aux_path */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION adie_services_set_aux_line_in_vol

DESCRIPTION
  Configure Adie Registers to set Aux Line In Volume

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if ADIE needs to be active for current path.
  FALSE if ADIE does not need to be active for current path.

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
boolean adie_services_set_aux_line_in_vol
(
  adie_aux_line_in_device_type device, uint8 volume
)
{
#if defined(ADIE_SERVICES_AUX_LINE_IN)
  adie_codec_config_type *adie_codec = NULL;

  if(volume >= VOLUME_LEVEL_MAX)
  {
    /* Print the error massage */
    MSG_ERROR("volume level is unacceptable, volume:%d \n", volume, 0, 0);

    return (FALSE);
  }

  /* set the volume for aux pga*/
  aux_volume = volume;

  if(device == ADIE_AUX_LINE_IN_HANDSET)
  {
    #if MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE != 0xFF
    /* Get the ADIE codec config for handset */
    adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_handset_vol_config[aux_volume];
    #else
    MSG_ERROR("FM on handset is not supported",0,0,0);
    #endif /* MSMAUD_ADIE_CODEC_AUX_HANDSET_VOL_DELAY_CONFIG_SIZE */
  }
  else if(device == ADIE_AUX_LINE_IN_HEADSET)
  {
    /* Get the ADIE codec config for headset */
    adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_headset_vol_config[aux_volume];
  }
  else if (device == ADIE_AUX_LINE_IN_SPEAKER) {
    /* Get the ADIE codec config for speaker*/
    adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_speaker_vol_config[aux_volume];
  }
  else if( device == ADIE_AUX_LINE_IN_LINEOUT )
  {
    #if MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE != 0xFF
    /* Get the ADIE codec config for lineout */
    adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_lineout_vol_config[aux_volume];
    #else
    MSG_ERROR("FM on LineOut is not supported",0,0,0);
    #endif /* MSMAUD_ADIE_CODEC_AUX_LINEOUT_VOL_DELAY_CONFIG_SIZE */
  }
  else if (device == ADIE_AUX_LINE_IN_USB) {
    /* Get the ADIE codec config for usb */
    adie_codec = (adie_codec_config_type *)
                 &adie_codec_aux_speaker_vol_config[aux_volume];
  }
#ifdef FEATURE_BT_AG
  /* FM over BT AG */
  else if (device == ADIE_AUX_LINE_IN_TOADSP)
  {
#ifdef ADIE_SERVICES_FM_OVER_BT_AG
    MSG_MED("Configured aux path to ADSP \n", 0, 0, 0);
    /* Get the ADIE codec config for aux stereo bt headset */
    adie_codec = (adie_codec_config_type *)
                    &adie_codec_aux_stereo_bt_headset_config;
#else /* ADIE_SERVICES_FM_OVER_BT_AG */
    MSG_ERROR("Target do not support ADIE_SERVICES_FM_OVER_BT_AG", 0, 0, 0);

#endif /* ADIE_SERVICES_FM_OVER_BT_AG */
  }
#endif /* FEATURE_BT_AG */
  else
  {
    /* Print the error massage */
    MSG_ERROR("This Device=%d is not supported ", device, 0, 0);

    return (FALSE);
  }

  return (adie_services_do_wakeup(adie_codec));
#else
  /* Print the error massage */
  MSG_ERROR("Feature ADIE_SERVICES_AUX_LINE_IN is not available \n", 0, 0, 0);

  return (FALSE);
#endif /* ADIE_SERVICES_AUX_LINE_IN */
} /* end adie_services_set_aux_line_in_vol */
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION adie_services_get_aux_line_in_vol

DESCRIPTION
  Get the Aux Line In Volume

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
/***/ __NON_DEMAND_PAGED_FUNCTION__ /***/
uint8 adie_services_get_aux_line_in_vol(void)
{
#ifdef ADIE_SERVICES_AUX_LINE_IN
  return(aux_volume);
#else
  /* Print the error massage */
  MSG_ERROR("Feature ADIE_SERVICES_AUX_LINE_IN is not available \n", 0, 0, 0);

  return 0;
#endif /* ADIE_SERVICES_AUX_LINE_IN */
}
/***/ __NON_DEMAND_PAGED_FUNCTION_END__ /***/

/*===========================================================================

FUNCTION adie_services_set_adie_sleep_status

DESCRIPTION
  Set adie sleep status.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void adie_services_set_adie_sleep_status(boolean status)
{
  /* set the adie sleep status */
  adie_services_adie_sleep_status = status;
}

/*===========================================================================

FUNCTION adie_services_get_adie_sleep_status

DESCRIPTION
  Return adie sleep status.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - adie is sleeping.
  FALSE - adie is not sleeping.

SIDE EFFECTS
  None.

===========================================================================*/
boolean adie_services_get_adie_sleep_status(void)
{
  return(adie_services_adie_sleep_status);
}

/*===========================================================================

FUNCTION adie_services_get_rx_path_status

DESCRIPTION
  Return Rx path on status. 
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Rx Filter is connected.
  FALSE - Rx Filter is not connected. 

SIDE EFFECTS
  None.

===========================================================================*/
boolean adie_services_get_rx_path_status(void)
{
#ifdef MSMAUD_ADIE_RX_BIQUAD_REG
  boolean status = FALSE;
  byte rx_biquad;

  /* Read the Rx biquad filter connection reg */
  ADIE_IN(MSMAUD_ADIE_RX_BIQUAD_REG,&rx_biquad);
  if(rx_biquad != 0x0)
  {
    status = TRUE;
  }

  return status;
#else
  /* Print the error massage */
  MSG_ERROR("Feature ADIE_SERVICES_AUX_LINE_IN is not available \n", 0, 0, 0);
  return FALSE;
#endif /* MSMAUD_ADIE_RX_BIQUAD_REG */
}


