#ifndef WMS_IWMSC_H
#define WMS_IWMSC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           -- Interfaces with IWMSC APIs

DESCRIPTION
  This module defines the data types and functions for the Short Message
  Services (SMS) which are used by the client software and are part of the
  UAPI (User Application Programming Interface).

  Copyright (c) 2007, 2008 by QUALCOMM, Incorporated.
  All Rights Reserved.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsmsg.h_v   1.8   17 Jun 2002 15:47:58   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wms_iwmsc.h#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information to include 2008
11/14/07   PMD     Added FEATURE_CDSMS guard around wms_iwmsc_send_cdma_msg()
04/19/07   HQ      Removed unused signals and moved mt_tid to rec_ptr.
03/14/07   HQ      Added msg_s_ptr.
03/06/07   HQ      Added piWMSCMsgUtil and mt_tid.
02/06/07   HQ      Increased msgOTA size.
01/31/07   HQ      Initial version.
===========================================================================*/

#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */

#endif /* WMS_IWMSC_H */

