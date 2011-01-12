#ifndef DSATCTAB_H
#define DSATCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               C O M M O N   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  commands common to all modes of operation.  This file only exports
  definitions internal to the AT command processor unit.

   Copyright (c) 1995 - 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatctab.h_v   1.1   08 Nov 2002 12:46:28   wxie  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatctab.h#2 $ $DateTime: 2009/01/22 06:46:48 $ $Author: bhaviks $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/02/09   bs      Added support for 1X AT Phonebook commands.
05/28/03   sb      Fix for WCDMA CS Calls
11/08/02   wx      Move dsat_qcdns_val declaration to dsat.h to export it 
                   for ps.
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   variables.
08/28/01   sjd     Initial version of file to support new ATCOP VU structure.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dsati.h"
#ifdef FEATURE_PM1000
#include "pm1000.h"
#elif defined (FEATURE_PMIC_RTC)
#include "clkrtc.h"
#endif

/*===========================================================================

                        PUBLIC DATA DECLARATIONS
                      
===========================================================================*/

/*---------------------------------------------------------------------------
                        Command defaults and limits
---------------------------------------------------------------------------*/

/* Note: Command defaults and limits should not be exported in the common
   command table header file.  Any command in the common command table
   should behave exactly the same across all operating modes, as such
   it should never be necessary to create another target specific command
   table defining a different behavior for one of these common commands. */


/*---------------------------------------------------------------------------
                        Command Parameter Values
---------------------------------------------------------------------------*/
#define DEFAULT_CPB_STORAGE "SM"
#define DEFAULT_NV_CPB_STORAGE "ME"

/* Parameter shared across common ETSI and IS-707 +CRC commands. */
extern dsat_num_item_type dsat_crc_val;

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
extern dsat_num_item_type dsat_dr_val;
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */
extern dsat_num_item_type dsat_cmee_val;
extern const dflm_type dsat_ds_dflm[];
/* Parameter shared across common ETSI and 1X for Phonebook commands. */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
extern dsat_num_item_type dsat_cpbs_val;
extern const dsat_string_item_type dsat_cpbs_storage_valstr [][8];
extern const def_list_type dsat_cpbs_list;
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
#ifdef FEATURE_DS_SOCKETS_TEST_UDP
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */

extern dsat_string_item_type dsat_qcdnspri_val[];
extern dsat_string_item_type dsat_qcdnssec_val[];

extern dsat_num_item_type dsat_qcter_val;


/*--------------------------------------------------------------------------
                    Common Command Tables and Table Sizes 
--------------------------------------------------------------------------*/

extern const dsati_cmd_type dsat_basic_table[ ];
extern const dsati_cmd_type dsat_basic_action_table[ ];
extern const dsati_cmd_type dsat_sreg_table[ ];
extern const dsati_cmd_type dsat_ext_table[ ];
extern const dsati_cmd_type dsat_vendor_table[ ];

extern const unsigned int dsat_basic_table_size;
extern const unsigned int dsat_basic_action_table_size;
extern const unsigned int dsat_sreg_table_size;
extern const unsigned int dsat_ext_table_size;
extern const unsigned int dsat_vendor_table_size;


/*---------------------------------------------------------------------------
                           NV interface data 
---------------------------------------------------------------------------*/
extern nv_item_type         ds_nv_item;            /* actual data item     */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSATCTAB_DATA_INIT

DESCRIPTION
  This function initializes the AT command defaults, limits, and lists
  that depend on a run-time determination of the hardware.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void dsatctab_data_init( void );


#endif /* FEATURE_DATA */

#endif /* DSATCTAB_H */

