#ifndef ADIESERVICES_H
#define ADIESERVICES_H
/*===========================================================================

           ADIE SERVICES

DESCRIPTION
  This header file contains all the definitions necessary to access the
  data contained in the vocoder calibration database.
  
Copyright (c) 1999 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/audio/6k/ver1/avs/main/latest/src/adie_services.h#8 $ $DateTime: 2009/10/15 05:43:22 $ $Author: kmodak $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/09   knm     Integrated new marimba codec driver for SCMM
10/06/09   sj      Added SCMM specific changes
09/16/09   gs      Added support for FM on Lineout, Handset
12/10/08   knm     Added adie services sleep APIs.
11/17/08   knm     Added adie_services_init() API & code to support FM on USB.
10/22/08   dp      FM over BT AG changes
09/15/08   knm     Added support for Aux Line In(FM) Volume control
06/20/08   knm     Created Adie Services APIs and Defintions.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#ifndef MSMAUD_SCMM
#include "adie.h"
#endif /* MSMAUD_SCMM */

#ifndef MSMAUD_SCMM
/* Adie codec config type */
typedef struct {
  uint8                     size;          /* Size of adie codec config    */
  uint8                     num_to_delay;  /* No. of settings to be delayed*/
  const adie_codec_reg_type *val;          /* Adie codec config value      */
} adie_codec_config_type;
#endif /* MSMAUD_SCMM */

typedef enum {
  ADIE_AUX_LINE_IN_HANDSET,    /* Aux Line In Audio on Handset */
  ADIE_AUX_LINE_IN_HEADSET,    /* Aux Line In Audio on Headset */
  ADIE_AUX_LINE_IN_SPEAKER,    /* Aux Line In Audio on Speaker */
  ADIE_AUX_LINE_IN_LINEOUT,    /* Aux Line In Audio on LineOut */
  ADIE_AUX_LINE_IN_USB,        /* Aux Line In Audio on USB */
  ADIE_AUX_LINE_IN_TOADSP     /* Aux Line In Audio to Audio DSP */
} adie_aux_line_in_device_type ;

void adie_services_set_final (
  int4 ms_interval
  /*lint -esym(715,ms_interval) Tell Lint we know ms_interval parameter
  ** is not used */
);

void adie_services_init (void);

#ifndef MSMAUD_SCMM
boolean adie_services_do_wakeup
(
  adie_codec_config_type *adie_codec
);
#endif /* MSMAUD_SCMM  */

boolean adie_services_configure_aux_line_in_on
(
  adie_aux_line_in_device_type device
);

boolean adie_services_configure_aux_line_in_off
(
  adie_aux_line_in_device_type device
);

boolean adie_services_configure_aux_path
(
  adie_aux_line_in_device_type device
);

boolean adie_services_set_aux_line_in_vol
(
  adie_aux_line_in_device_type device, uint8 volume
);

uint8 adie_services_get_aux_line_in_vol(void);

boolean adie_services_get_adie_sleep_status(void);

void adie_services_set_adie_sleep_status(boolean status);

#endif /* ADIESERVICES_H */


