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

/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wms_iwmsc.c#3 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/08   PMD     Fixed Compile Warnings with FEATURE_IWMSC_GW Enabled
04/22/08   PMD     Added support for WMSE to talk to WMS Components for GW
01/31/08   PMD     Updated Copyright Information to include 2008
10/09/07   PY      Get the TL ack routed into wmsmsg directly.
08/22/07   NP      Updated to use new interfaces
05/01/07   HQ      Handle WMSC layer 2 status and final status properly.
04/26/07   HQ      Updated for code merge with mainline.
04/19/07   HQ      Combined certain functions; TID and rec_ptr/tid changes.
04/12/07   HQ      Check DONE flag for final state; renable signal in callbacks.
03/14/07   HQ      Handle IWMSCMOTrans instances for MO status.
03/06/07   HQ      Added usage of IWMSCMsgUtil, mt_tid and MO status mapping.
03/02/07   HQ      Added MO status and error code mapping.
02/06/07   HQ      Send msg or ack.
02/05/07   HQ      Added error checkings and send/receive updates.
01/31/07   HQ      Initial version.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/
#include "comdef.h"

#ifdef FEATURE_IWMSC
#error code not present
#endif /* FEATURE_IWMSC */

/* END of wms_iwmsc.c */
