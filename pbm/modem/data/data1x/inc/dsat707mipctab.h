#ifndef DSAT707MIPCTAB_H
#define DSAT707MIPCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

                M I P   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  Mobile IP commands specific to IS-707 mode of operation.
  

Copyright (c) 1995 - 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mipctab.h_v   1.5   07 Feb 2003 10:24:12   sramacha  $
$Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/inc/dsat707mipctab.h#1 $ $DateTime: 2008/04/11 07:46:55 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/02   jay     Removed AT test feature around QCMIPT
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"

/*=========================================================================*/
/* Mobile IP related AT command table */
/*=========================================================================*/

#ifdef FEATURE_DS_MOBILE_IP
extern const dsati_cmd_type dsat707_mip_table[ ];
extern const unsigned int dsat707_mip_table_size;

/* Data declarations for MIP commands */
extern dsat_num_item_type     dsat707_qcmip_val;
extern dsat_num_item_type     dsat707_qcmipp_val;
extern dsat_num_item_type     dsat707_qcmipt_val;
extern dsat_num_item_type     dsat707_qcmipep_val;
extern dsat_num_item_type     dsat707_qcmipgetp_val;
extern dsat_num_item_type     dsat707_qcmiprt_val[2];
extern dsat_string_item_type  dsat707_qcmipnai_val[];
extern dsat_string_item_type  dsat707_qcmipmass_val[];
extern dsat_string_item_type  dsat707_qcmipmhss_val[];
extern dsat_num_item_type     dsat707_qcmipmassx_val[2];
extern dsat_num_item_type     dsat707_qcmipmhssx_val[2];
extern dsat_num_item_type     dsat707_qcmipmaspi_val[2];
extern dsat_num_item_type     dsat707_qcmipmhspi_val[2];
extern dsat_string_item_type  dsat707_qcmipha_val[];
extern dsat_string_item_type  dsat707_qcmippha_val[];
extern dsat_string_item_type  dsat707_qcmipsha_val[];

extern uint32  dsat707_ha_val;
extern uint32  dsat707_pri_ha_val;
extern uint32  dsat707_sec_ha_val;

extern dflm_type dsat707_qcmip_dflm [];
extern dflm_type dsat707_qcmipp_dflm [];
extern dflm_type dsat707_qcmipt_dflm [];
extern dflm_type dsat707_qcmipep_dflm [];
extern dflm_type dsat707_qcmiprt_dflm [];
extern dflm_type dsat707_qcmipmaspi_dflm [];
extern dflm_type dsat707_qcmipmhspi_dflm [];
extern dflm_type dsat707_qcmipgetp_dflm [];
extern dflm_type dsat707_qcmipmassx_dflm [];
extern dflm_type dsat707_qcmipmhssx_dflm [];

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */  
#endif /* FEATURE_DATA         */

#endif /* DSAT707MIPCTAB_H     */
