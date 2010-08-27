/*===========================================================================
                I M P L E M E N T A T I O N   F I L E   F O R 
    R F N V   - - >   R F C O N F I G   M I G R A T I O N   T R A C K E R

DESCRIPTION
  This file implements the interface available for manipulating
  tracker objects that indicate/control what particular RFNV items have been
  read from QCN file and those that must be considered for migration to
  RFConfig database.

  CAUTION:
  Do NOT access the data structure of the tracker object directly; rather
  use only the function interfaces provided to ensure the integrity of the data 
  contained within.

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfnv_rfconfig_migration_tracker.c#2 $ $DateTime: 2008/11/19 11:46:56 $ $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/08   et      klocwork fixes
06/20/07   Vish    Created initial version.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "rfnv_rfconfig_migration_tracker.h"
#include "rf_error.h"


#ifdef RF_HAS_RFCONFIG_RX_LNA

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/
// mark all items as NOT requiring migration from NV to RFConfigDBase mgmt.
void rfnv_rfconfig_migration_tracker_reset(
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr)
{
   RF_ASSERT(NULL != tracker_ptr);
   if (tracker_ptr == NULL) 
   {
	   RF_FATAL_ERROR("tracker_ptr is NULL");
	   rf_error_flag = TRUE;
	   return;
   }

   (*tracker_ptr) = 0;
}

// mark a particular item as requiring migration from NV to RFConfigDBase mgmt.
void rfnv_rfconfig_migration_tracker_enable(
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr,
   rfnv_rfconfig_migration_item_type      tracked_item)
{
   RF_ASSERT(NULL != tracker_ptr);
   RF_ASSERT(tracked_item < RFNV_RFCONFIG_MIGRATION_ITEM__COUNT);

   if (tracked_item < RFNV_RFCONFIG_MIGRATION_ITEM__NONE)
   {
      (*tracker_ptr) |= (1 << tracked_item);
   }
}

// mark a particular item as NOT requiring migration from NV to RFConfigDBase mgmt.
void rfnv_rfconfig_migration_tracker_disable(
   rfnv_rfconfig_migration_tracker_type  *tracker_ptr,
   rfnv_rfconfig_migration_item_type      tracked_item)
{
   RF_ASSERT(NULL != tracker_ptr);
   RF_ASSERT(tracked_item < RFNV_RFCONFIG_MIGRATION_ITEM__COUNT);

   if (tracked_item < RFNV_RFCONFIG_MIGRATION_ITEM__NONE)
   {
      (*tracker_ptr) &= (~(1 << tracked_item));
   }
}

// find if a particular item requires migration from NV to RFConfigDBase mgmt.
boolean rfnv_rfconfig_migration_tracker_is_enabled(
   const rfnv_rfconfig_migration_tracker_type  *tracker_ptr,
   rfnv_rfconfig_migration_item_type            tracked_item)
{
   boolean  enabled_status = FALSE;

   RF_ASSERT(NULL != tracker_ptr);
   RF_ASSERT(tracked_item < RFNV_RFCONFIG_MIGRATION_ITEM__COUNT);

   if (tracked_item < RFNV_RFCONFIG_MIGRATION_ITEM__NONE)
   {
      enabled_status = ( ((*tracker_ptr) & (1 << tracked_item)) != 0 );
   }

   return enabled_status;
}


#endif /* RF_HAS_RFCONFIG_RX_LNA */


