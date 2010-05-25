// $Header: //source/qcom/qct/multimedia/graphics/svg/services/interface/main/latest/inc/svgver.h#9 $ $DateTime: 2009/02/26 20:10:53 $ $Author: dkthomas $

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        S V G V E R - H E A D E R F I L E

GENERAL DESCRIPTION
  This header file contains SVG version.

REFERENCES

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     	what, where, why
--------   ---     	----------------------------------------------------------
03/09/06   lip     	Created file.
23/02/09   dkthomas 	Svg Engine 6.7 Integration 
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef SVGVER_H
#define SVGVER_H

#ifdef FEATURE_SVG

#if !defined(SVGT_VERSION)

  #define SVGT_1_1       0
  #define SVGT_1_2       1

  #define SVGT_VERSION   SVGT_1_2

  // Increment this for major updates
  #define SVGT_VERSION_MAJOR   3

  // Incrememt this for minor updates
  #define SVGT_VERSION_MINOR   3

  // Increment this every baseline regression
  #define SVGT_VERSION_BUILD   1

  // The combined SVG version ID
  #define SVGT_EXTENSION_VERSION SVGT_VERSION_MAJOR ## "." ## SVGT_VERSION_MINOR ## "." ## SVGT_VERSION_BUILD

#endif //!defined(SVGT_VERSION)
                               
#endif //FEATURE_SVG

#endif //SVGVER_H



