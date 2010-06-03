#ifndef OEMWMS_H
#define OEMWMS_H
/*===========================================================================

              O E M   W M S

DESCRIPTION
  Definitions to support access to OEM Wireless Messaging Services

INITIALIZATION AND SEQUENCING REQUIREMENTS
   This file should be included directly.  


Copyright (c) 2001,2002,2003,2004,2005,2006,2007,2008 by QUALCOMM, Incorporated.
All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/wms/wmsapp/main/latest/inc_deprecated/OEMWMS.h#2 $
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/31/08   PMD     Updated Copyright Information
01/17/07   HQ      Updated copyright info for 2007.
10/31/03   PMD     Initial Version to define OEMWMS_MsgInfoCacheCb prototype
===========================================================================*/

/* Default Message Info Cache Callback present in OEMWMS */

void OEMWMS_MsgInfoCacheCb
(
  const wms_client_message_s_type     *pMsg,
  uint8                               *pCache
);

#endif /* OEMWMS_H */
