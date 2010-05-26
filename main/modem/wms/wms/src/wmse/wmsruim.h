#ifndef WMSRUIM_H
#define WMSRUIM_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           W I R E L E S S   M E S S A G I N G   S E R V I C E S
           -- RUIM Interface Module

DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  defines the RUIM Interface functions.

Copyright (c) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007,
2008 by QUALCOMM, Incorporated.  All Rights Reserved.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmsruim.h_v   1.2   10 Jul 2002 20:17:42   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmsruim.h#3 $ $DateTime: 2010/02/23 11:51:47 $ $Author: rajang $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/08   PMD     Added support for BC-SMS and SMSCAP EFs in RUIM.
01/31/08   PMD     Updated Copyright Information to include 2008
01/17/07   HQ      Updated copyright info to include 2007.
01/25/06   VK      Updated Copyright Info
12/21/05   VK      Lint fixes
10/08/05   VK      Updated Copyright info. 
07/26/03   PMD     Changes to suppress / remove Lint Errors
04/17/03   AP      Added a flag for ruim_write to distinguish a prl update for
                   FEATURE_UIM_TOOLKIT_UTK.
07/10/02   HQ      Moved some #defines out of FEATURE_CDSMS_RUIM.
06/22/02   HQ      Added processing of RUIM SMS init, sms, smsp and smss files.
03/12/02   HQ      Initial version for support of Multimode WMS.

===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_CDSMS_RUIM

#include "wms.h"
#include "auth.h"

/* Service Table Flags */

#define WMS_RUIM_SERVICE_TABLE_SIZE             12

#define WMS_RUIM_SERVICE_TABLE_SMS_ALLOC        0x40
#define WMS_RUIM_SERVICE_TABLE_SMS_ACTIVE       0x80

#define WMS_RUIM_SERVICE_TABLE_SMSP_ALLOC       0x40
#define WMS_RUIM_SERVICE_TABLE_SMSP_ACTIVE      0x80

#define WMS_RUIM_SERVICE_TABLE_BCSMS_ALLOC      0x04
#define WMS_RUIM_SERVICE_TABLE_BCSMS_ACTIVE     0x08

#define WMS_RUIM_SERVICE_TABLE_3GPD_ALLOC       0x10
#define WMS_RUIM_SERVICE_TABLE_3GPD_ACTIVE      0x20


/* Macros
*/
#define WMS_RUIM_SERVICE_TABLE_HAS_SMS( x ) \
  ( ( (x) & WMS_RUIM_SERVICE_TABLE_SMS_ALLOC) && \
    ( (x) & WMS_RUIM_SERVICE_TABLE_SMS_ACTIVE) )

#define WMS_RUIM_SERVICE_TABLE_HAS_SMSP( x ) \
  ( ( (x) & WMS_RUIM_SERVICE_TABLE_SMSP_ALLOC) && \
    ( (x) & WMS_RUIM_SERVICE_TABLE_SMSP_ACTIVE) )

#define WMS_RUIM_SERVICE_TABLE_HAS_BCSMS( x ) \
  ( ( (x) & WMS_RUIM_SERVICE_TABLE_BCSMS_ALLOC) && \
    ( (x) & WMS_RUIM_SERVICE_TABLE_BCSMS_ACTIVE) )

#define WMS_RUIM_SERVICE_TABLE_HAS_3GPD( x ) \
  ( ( (x) & WMS_RUIM_SERVICE_TABLE_3GPD_ALLOC) && \
    ( (x) & WMS_RUIM_SERVICE_TABLE_3GPD_ACTIVE) )


void wms_ruim_init
(
  void
);

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
boolean wms_ruim_init_stepbystep(void);

#ifdef FEATURE_OMH_SMS
void wms_ruim_OMH_init(void);
#endif
#endif

boolean wms_ruim_read_sms
(
  wms_message_index_type   index,
  uint8                    * data
);


boolean wms_ruim_write_sms
(
  wms_message_index_type   index,
  uint8                    * data,
  boolean                  is_prl_update
);

boolean wms_ruim_delete_sms
(
  wms_message_index_type  index
);


boolean wms_ruim_read_smsp
(
  wms_message_index_type   index,
  uint8                    * data
);


/*
*/
boolean wms_ruim_write_smsp
(
  wms_message_index_type   index,
  uint8                    * data
);

/*
*/
boolean wms_ruim_delete_smsp
(
  wms_message_index_type  index
);


boolean wms_ruim_write_smss
(
  uint8   *data
);

#endif /* FEATURE_CDSMS_RUIM */
#endif /* WMSRUIM_H */

