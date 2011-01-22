/*=============================================================================

FILE: OEMPDP.c

SERVICES: OEM Functions for PDP Registry

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2003 - 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMPDP.c#20 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
08/02/07     sj      Removed feature FEATURE_GSM_SNDCP_IPHC_ROHC
06/15/07     sj      Addition of two flags:im_cn_flag and dhcp_pcscf flag
06/12/07     cvs     Fix compile warnings
05/05/07     mjm     Removed all featurization from AEEPDP.h
12/04/06     sj      Added enum types for PDP header compression and
                     Data compression types.
09/15/06     sj      Featurized pcscf addr changes with FEATURE_IMS
09/11/06     sj      Changed cscf to pcscf in ds structure variables
08/08/06     sj      Added support for PCSCF address
04/26/06     np      Fix packed/unpacked public AEE Struct issue
12/05/05     JAS     Fixed IPV6 featurization
11/10/05     RI      Fixed the error in check in (wrong in/out values for PDP
                     context were copied)
10/28/05     RI      Modified copying of PDP context information to and from DS 
                     to OEM.
08/23/05     sun     fixed Compile Errors
08/02/05     sun     Added support for IPV6
05/26/05     sun     Fixed Macros
05/02/05     RI      QoS changes.
03/23/05     sun     Cancel the callback on Exit
03/17/05     sun     Changes for Saving App Context and using the LinkSysObject to Free 
                     memory on Exit 
02/09/05     rsb     Add FEATURE_PROFILE_OTAP_ENHANCEMENTS for UTF-16 profile_name
03/25/04     ram     BREW3.x changes.
03/04/04     ram     QIS review changes.
03/01/04     ram     Fixed reference count.
01/20/04     ram     Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif //(FEATURE_WCDMA) || (FEATURE_GSM_GPRS)
