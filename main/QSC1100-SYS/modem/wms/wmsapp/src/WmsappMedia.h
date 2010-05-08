#ifndef WMSAPPMEDIA_H
#define WMSAPPMEDIA_H
/*=============================================================================

FILE: WMSAppMedia.h

    U S E R   I N T E R F A C E   S M S   D I S P L A Y   S E R V I C E

GENERAL DESCRIPTION
  Contains the types and items required for controlling and displaying
  SMS menus and screens.



(c) COPYRIGHT 2003, 2004, 2005, 2006, 2007, 2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/src/WmsappMedia.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/08   PMD     Removed support for Auto / Transparent SMS 
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Updated copyright info for 2007.
01/25/06   VK      Updated Copyright Info
02/20/05   PMD     Added 1x Broadcast Support in Multimode Broadcast
07/07/04   PMD     Changed Function Prototype - WMSAPPMD_ConvertPicToEMS()
03/16/04   AP      Added support for the user app
11/21/03   PMD     Added GSM Cell Broadcast Support
07/29/03   AP      Modified included files to generate the correct dependencies.
07/07/03   PMD     Changes to remove Lint Errors
06/05/03   SUN     Added support for FEATURE_GWSMS_STATUS_REPORTS
05/27/03   PMD     Added Prototypes for ConvertUserMelodytToEMS and PlayMelody
05/12/03   SUN     Fixed Compile Errors
05/12/03   SUN     Added support for Class Summary
05/10/03   HQ      Added #define WMSAPP_OTHER_INFO_TEXT_LENGTH.
03/10/03   SUN     Added support for Memory Usage
02/14/02   SUN     Initial Revision
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "target.h"
#include "comdef.h"

#ifdef FEATURE_WMS_APP
#include "WmsApp.h"

/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define WMSAPP_OTHER_INFO_TEXT_LENGTH    240


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/* <EJECT> */

boolean WMSAPPMD_Display
(
CWMSAPP *pMe
);


void WMSAPPMD_DisplayOtherInfo
(
CWMSAPP *pMe,
AECHAR *psContents
);

boolean  WMSAPPMD_DisplayWhileComposing
(
CWMSAPP *pMe,
AECHAR *data,
boolean newCharSet
);


int  WMSAPPMD_EraseDisplay
(
CWMSAPP *pMe,
int pos,
int len
);

boolean WMSAPPMD_ConvertPicToEMS
(
  CWMSAPP        *pMe,      /* INPUT */
  char           *fileName, /* INPUT */
  wms_udh_s_type *pUdh      /* OUTPUT */
);

int WMSAPPMD_ConvertSmallAnimToEMS
(
CWMSAPP *pMe,
uint8 data[4][8],
char* fileName
);

boolean WMSAPPMD_ConvertLargeAnimToEMS
(
CWMSAPP *pMe,
uint8 data[4][32],
char* fileName
);

int WMSAPPMD_ConvertUserMelodyToEMS
(
CWMSAPP *pMe,
uint8 *data,
char* fileName
);

boolean WMSAPPMD_DisplayMessageInfo
(
  CWMSAPP *pMe
);

void WMSAPPMD_DisplayTemplate
(
  CWMSAPP *pMe,
  AECHAR *psContents,
  wms_client_message_s_type *pMsg
);

void WMSAPPMD_SetScreen
(
CWMSAPP *pMe
);

boolean WMSAPPMD_DisplayMemoryUsage
(
  CWMSAPP *pMe,
  AECHAR *psContents
);

boolean WMSAPPMD_DisplayClassSummary
( CWMSAPP *pMe,
  AECHAR *psContents
);

boolean WMSAPPMD_PlayMelody
(
  CWMSAPP *pMe,
  uint16 wDlgID,
  uint16 selection
);

#ifdef FEATURE_GWSMS_STATUS_REPORTS
#error code not present
#endif/*FEATURE_GWSMS_STATUS_REPORTS*/

#ifdef FEATURE_GWSMS_BROADCAST
#error code not present
#endif /* FEATURE_GWSMS_BROADCAST */

#endif /* FEATURE_WMS_APP*/

#endif /* WMSAPPMedia_H */

/*lint -e752 */

