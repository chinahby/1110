#ifndef WMSLL_H
#define WMSLL_H
/*===========================================================================

            W I R E L E S S    M E S S A G I N G   S E R V I C E S
            -- L O W E R   L A Y E R   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary for
  Lower Layers to interface with the WMS module.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED. 
All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsll.h_v   1.2   26 Mar 2002 12:10:04   hqu  $
  $Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/inc_deprecated/wmsll.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/18/08   PMD     Removed FEATURE_WMS_CM_NAS_INTERFACES
                   Reorganized Includes
01/31/08   PMD     Updated Copyright Information to include 2008
08/21/07   PMD     Removed rpt prototypes due to disagreement
02/20/07   HQ      Added support for FEATURE_WMS_CM_NAS_INTERFACES.
01/17/07   HQ      Updated copyright info to include 2007.
11/06/06   PMD     Replaced Feature T_MSM6250 with FEATURE_GWSMS
                   Eliminated FEATURE_SMS_LTK from Code
01/25/06   VK      Updated Copyright Info
10/08/05   VK      Changed byte = uint8, word = uint16, dword = uint32
10/08/05   VK      Updated copyright info
04/04/05   PMD     Fix in GW CB Cell Change Event Structure
03/31/05   PMD     Added Cell Change Type in WMS Report Structure.
01/10/05   Rex     decouple WMS from including NAS header files
01/07/05   PMD     Added feature FEATURE_SMS_LTK
09/22/04   Rex     Featurize SMS link control
08/25/04   Rex     Added support for SMS link control
06/15/04   PMD     Support for Duplicate Detection Indication from CB
12/15/03   PMD     Added 1x Broadcast Support in Multimode Broadcast API
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
03/12/02   HQ      Added support of Multimode WMS - added FEATURE_CDSMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
06/14/01   HQ      Initial version for FEATURE_GWSMS.

===========================================================================*/

#ifdef FEATURE_GWSMS /* SMS API */
#error code not present
#endif /* FEATURE_GWSMS */

#endif /* WMSLL_H */

