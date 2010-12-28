#ifndef _VIDEO_RESOURCES_H_
#define _VIDEO_RESOURCES_H_

/*==============================================================================

            V I D E O / C A M E R A   R E S O U R C E S   H E A D E R   F I L E

                                  DESCRIPTION

This header file defines the types and functions used to manage usage of the
hardware resources required by video and camera.

                             COPYRIGHT INFORMATION

Copyright (c) 2005 QUALCOMM, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/videoresources/6k/main/latest/inc/video_resources.h#1 $ $DateTime: 2008/05/09 16:34:47 $ $Author: karenp $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
02/16/06     dle     Linted.
05/02/05     kwa     Initial version.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include CLKRGM_H

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

#define  VIDEO_RESOURCES_CLIENT_DTV        0x00000001
#define  VIDEO_RESOURCES_CLIENT_JPEGD      0x00000002
#define  VIDEO_RESOURCES_CLIENT_JPEGE      0x00000004
#define  VIDEO_RESOURCES_CLIENT_QCAMCORDER 0x00000008
#define  VIDEO_RESOURCES_CLIENT_QCAMERA    0x00000010
#define  VIDEO_RESOURCES_CLIENT_QTV        0x00000020
#define  VIDEO_RESOURCES_CLIENT_QVP        0x00000040
#define  VIDEO_RESOURCES_CLIENT_MAX        0xffffffff


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  video_resources_clock_on

DESCRIPTION

  Function that turns on a video clock if it is not already on.

RETURN VALUE

  None

==============================================================================*/

extern void video_resources_clock_on
(
  uint32                               client,
  clk_regime_type                      clock
);


/*==============================================================================

FUNCTION NAME

  video_resources_clock_off

DESCRIPTION

  Function that turns off a video clock if it is not in use by another client.

RETURN VALUE

  None

==============================================================================*/

extern void video_resources_clock_off
(
  uint32                               client,
  clk_regime_type                      clock
);

#endif
