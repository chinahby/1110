/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               M I P   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the Mobile IP AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1995-2002 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mipctab.c_v   1.5   07 Feb 2003 10:24:10   sramacha  $
$Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707mipctab.c#1 $ $DateTime: 2007/11/06 01:00:46 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/18/02   jay     Modified comments and formatted code.
04/03/01   rsl     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707mip.h"
#include "dsat707mipctab.h"
#include "dsatparm.h"
#include "msg.h"


#ifdef FEATURE_DS_MOBILE_IP
/*=========================================================================*/
/* Data declarations for Mobile IP commands */
/*=========================================================================*/
dsat_num_item_type dsat707_qcmip_val;
dsat_num_item_type dsat707_qcmipp_val;

/*---------------------------------------------------------------------------
  Select mn-ha authentication calculation method:

  FALSE = RFC2002 (exclude mn-ha auth SPI)
  TRUE  = RFC2002bis (include mn-ha auth SPI)
---------------------------------------------------------------------------*/
dsat_num_item_type     dsat707_qcmipt_val;
dsat_num_item_type     dsat707_qcmipep_val;
dsat_num_item_type     dsat707_qcmipgetp_val;

dsat_num_item_type     dsat707_qcmiprt_val[2];
dsat_num_item_type     dsat707_qcmipmaspi_val[2];
dsat_num_item_type     dsat707_qcmipmhspi_val[2];
dsat_string_item_type  dsat707_qcmipnai_val[77];
dsat_string_item_type  dsat707_qcmipmass_val[17];
dsat_string_item_type  dsat707_qcmipmhss_val[17];
dsat_num_item_type     dsat707_qcmipmassx_val[2];
dsat_num_item_type     dsat707_qcmipmhssx_val[2];
dsat_string_item_type  dsat707_qcmipha_val[16];
dsat_string_item_type  dsat707_qcmippha_val[16];
dsat_string_item_type  dsat707_qcmipsha_val[16];
uint32                 dsat707_ha_val;
uint32                 dsat707_pri_ha_val;
uint32                 dsat707_sec_ha_val;

/*=========================================================================*/
/* Defaults & Limits for MIP commands */
/*=========================================================================*/
dflm_type dsat707_qcmip_dflm [] =
{
  { 0  , 0  , 2    },
} ;

dflm_type dsat707_qcmipp_dflm [] =
{
  { 0  , 0  , 5    },
} ;

dflm_type dsat707_qcmipt_dflm [] =
{
  { 0  , 0  , 1    },
} ;

dflm_type dsat707_qcmipep_dflm [] =
{
  { 0  , 0  , 1    },
} ;

dflm_type dsat707_qcmiprt_dflm [] =
{
  { 0  , 0  , 1    },
  { 0  , 0  , 1    }
} ;

dflm_type dsat707_qcmipmaspi_dflm [] =
{
  { 0x2  , 0  , 0xFFFFFFFFL},
  { 0    , 0  , 1            }
} ;

dflm_type dsat707_qcmipmhspi_dflm [] =
{
  { 0x3  , 0  , 0xFFFFFFFFL},
  { 0    , 0  , 1            }
} ;

dflm_type dsat707_qcmipgetp_dflm [] =
{
  { 0  , 0  , 5    },
} ;

dflm_type dsat707_qcmipmassx_dflm [] =
{
  { 0  , 0  , 0xFFFFFFFFL  },
  { 0  , 0  , 0x1          }
} ;

dflm_type dsat707_qcmipmhssx_dflm [] =
{
  { 0  , 0  , 0xFFFFFFFFL  },
  { 0  , 0  , 0x1          }
} ;

/*=========================================================================*/
/*  Mobile IP related AT command table */
/*=========================================================================*/

const dsati_cmd_type dsat707_mip_table [] =
{
#ifdef FEATURE_DS_AT_TEST_ONLY
  { "$QCMIP",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIP,
      1,
      &dsat707_qcmip_val,
      &dsat707_qcmip_dflm[0],
      dsat707_exec_qcmip_cmd },
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
  { "$QCMIPP",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIPP,
      1,
      &dsat707_qcmipp_val,
      &dsat707_qcmipp_dflm[0],
      dsat707_exec_qcmipp_cmd },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_AT_TEST_ONLY
  { "$QCMIPT",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIPT,
      1,
      &dsat707_qcmipt_val,
      &dsat707_qcmipt_dflm[0],
      dsat707_exec_qcmipt_cmd },
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
  { "$QCMIPEP",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIPEP,
      1,
      &dsat707_qcmipep_val,
      &dsat707_qcmipep_dflm[0],
    dsat707_exec_qcmipep_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMASS",
      EXTENDED | CONFIG | LOCAL_TEST | STRING | NO_QUOTE,
      SPECIAL_QCMIPMASS,
      17,
      &dsat707_qcmipmass_val[0],
      NULL,
      dsat707_exec_qcmipmass_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMHSS",
      EXTENDED | CONFIG | LOCAL_TEST | STRING | NO_QUOTE,
      SPECIAL_QCMIPMHSS,
      17,
      &dsat707_qcmipmhss_val[0],
      NULL,
      dsat707_exec_qcmipmhss_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMASPI",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIPMASPI,
      2,
      &dsat707_qcmipmaspi_val[0],
      &dsat707_qcmipmaspi_dflm[0],
      dsat707_exec_qcmipmaspi_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMHSPI",
      EXTENDED | CONFIG | LOCAL_TEST ,
      SPECIAL_QCMIPMHSPI,
      2,
      &dsat707_qcmipmhspi_val[0],
      &dsat707_qcmipmhspi_dflm[0],
      dsat707_exec_qcmipmhspi_cmd},
/*-------------------------------------------------------------------------*/
  { "$QCMIPRT",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIPRT,
      2,
      &dsat707_qcmiprt_val[0],
      &dsat707_qcmiprt_dflm[0],
      dsat707_exec_qcmiprt_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPNAI",
      EXTENDED | CONFIG | LOCAL_TEST | STRING | NO_QUOTE,
      SPECIAL_QCMIPNAI,
      77,
      &dsat707_qcmipnai_val[0],
      NULL,
      dsat707_exec_qcmipnai_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPHA",
      EXTENDED | CONFIG | LOCAL_TEST | STRING | NO_QUOTE,
      SPECIAL_QCMIPHA,
      16,
      &dsat707_qcmipha_val[0],
      NULL,
      dsat707_exec_qcmipha_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPPHA",
      EXTENDED | CONFIG | LOCAL_TEST | STRING | NO_QUOTE,
      SPECIAL_QCMIPPHA,
      16,
      &dsat707_qcmippha_val[0],
      NULL,
      dsat707_exec_qcmippha_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPSHA",
      EXTENDED | CONFIG | LOCAL_TEST | STRING | NO_QUOTE,
      SPECIAL_QCMIPSHA,
      16,
      &dsat707_qcmipsha_val[0],
      NULL,
      dsat707_exec_qcmipsha_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPGETP",
      EXTENDED | CONFIG | LOCAL_TEST,
      SPECIAL_QCMIPGETP,
      1,
      &dsat707_qcmipgetp_val,
      &dsat707_qcmipgetp_dflm[0],
      dsat707_exec_qcmipgetp_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMASSX",
      EXTENDED | CONFIG | LOCAL_TEST | HEX,
      SPECIAL_QCMIPMASSX,
      2,
      &dsat707_qcmipmassx_val[0],
      &dsat707_qcmipmassx_dflm[0],
      dsat707_exec_qcmipmassx_cmd },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMHSSX",
      EXTENDED | CONFIG | LOCAL_TEST | HEX,
      SPECIAL_QCMIPMHSSX,
      2,
      &dsat707_qcmipmhssx_val[0],
      &dsat707_qcmipmhssx_dflm[0],
      dsat707_exec_qcmipmhssx_cmd }
/*-------------------------------------------------------------------------*/
};
/* Size of Mobile IP command table */
const unsigned int dsat707_mip_table_size = ARR_SIZE( dsat707_mip_table );

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */
#endif /* FEATURE_DATA         */

