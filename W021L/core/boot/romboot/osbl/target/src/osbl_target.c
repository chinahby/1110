
/*===========================================================================

                         O S  B o o t  L o a d e r 
                                T a r g e t

DESCRIPTION
  This source file contains declarations and definitions for the OSBL
  that are target-specific

EXTERNALIZED FUNCTIONS
  osbl_target_retrieve_shared_info_from_dbl
  osbl_target_is_appsbl_auth_required
  osbl_target_get_cust_amss_parti_name
  osbl_is_amss_hash_checking_required
  osbl_get_nor_partition_table_location

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2008-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/romboot/osbl/osbl_target.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/09   dk      Removing compiler warnings.
11/07/07   ly      Create
============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <string.h>
#include "osbl.h"
#include "boot_dbl_if.h"
#include "boot_pbl_accessor.h"
#include "osbl_target.h"
#include "osbl_error_handler.h"

#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
extern uint32 get_osbl_ahb_bus_freq(void);
#endif
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_TARGET_PBL_ERROR_HANDLER_WRAPPER

DESCRIPTION
  Function which has interface similar to pbl_error_handler used for wrapper 
  to fsbl_error_handler. This wrapper is passed to pbl via PBL's 
  init_shared_routines.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void osbl_target_pbl_error_handler_wrapper
(
  const char* filename_ptr,         /* File this function was called from -
                                       defined by __FILE__ */
  uint32      line,                 /* Line number this function was called
                                       from - defined by  __LINE__ */
  pbl_err_code_enum_type  err_code, /* Error code to indicate which error
                                       by toggling UART_DP_TX_DATA pin */
  uint32  err_log                   /* Error log to help debug */
)
{
  bl_err_if.error_handler(filename_ptr, line, (uint32)err_code);
}/* osbl_target_pbl_error_handler_wrapper */

/*=============================================================================
FUNCTION      OSBL_CLK_BUSY_WAIT

DESCRIPTION   This function stalls the PAUSE timer.
              Conversion from microseconds to pause time is done in the 
              function.

DEPENDENCIES  None

RETURN VALUE  None
=============================================================================*/
void osbl_clk_busy_wait(uint32 micro_sec)
{
#ifdef FEATURE_QHSUSB_DEAD_BATTERY_CHARGING
uint32 wait_remaining_cycle =0;/* how many cycles we have to wait*/  
	  uint32 pause_cycle=0;/* max is 65535. unit is clock cycle*/
      uint32 ahb_clk_speed=48;
	  /* How many cycles we should stall? */
      ahb_clk_speed=get_osbl_ahb_bus_freq();
	  wait_remaining_cycle = ahb_clk_speed * micro_sec;
	 
	  /* Each penalty of write is 12 cycles */	  
	  while (wait_remaining_cycle >= 13)
	  {
		/* Max of PAUSE TIME stall time is 65535 cycles */
		pause_cycle = MIN(65535, (wait_remaining_cycle-13));
		HWIO_OUT (PAUSE_TIMER, pause_cycle);
		wait_remaining_cycle -= (pause_cycle + 13);
	  }
#endif
} /* osbl_clk_busy_wait() */

/*===========================================================================

FUNCTION  OSBL_TARGET_RETRIEVE_SHARED_INFO_FROM_DBL

DESCRIPTION
  This function retreieves target-specific shared data from the DBL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void osbl_target_retrieve_shared_info_from_dbl
(
  bl_shared_data_type *bl_shared_data,
  boot_dbl_if_shared_info_type *dbl_shared_info_ptr
)
{
  static boot_flash_shared_dev_info_type osbl_pbl_flash_shared_data;
  static boot_pbl_fn_tbl_type osbl_pbl_fn_tbl;

  OSBL_VERIFY( ( bl_shared_data != NULL &&
                 bl_shared_data->dbl_shared_data != NULL &&
                 dbl_shared_info_ptr != NULL &&
                 dbl_shared_info_ptr->pbl_shared_data != NULL &&
                 dbl_shared_info_ptr->pbl_shared_data->flash_shared_data != NULL ),
               BL_ERR_NULL_PTR_PASSED );

  /* Copy the PBL flash shared data structure to OSBL address space. */
  memcpy( &osbl_pbl_flash_shared_data, 
          dbl_shared_info_ptr->pbl_shared_data->flash_shared_data,
          sizeof(osbl_pbl_flash_shared_data) );

  /* Copy the PBL shared function table structure to OSBL address space. */
  memcpy( &osbl_pbl_fn_tbl,
          dbl_shared_info_ptr->pbl_shared_data->fn_tbl,
          sizeof(osbl_pbl_fn_tbl) );

  /* Override the PBL flash shared data structure with our copy */
  bl_shared_data->dbl_shared_data->pbl_shared_data->flash_shared_data =
    &osbl_pbl_flash_shared_data;
  /* Override the PBL flash shared data structure with our copy */
  bl_shared_data->dbl_shared_data->pbl_shared_data->fn_tbl = &osbl_pbl_fn_tbl;

  /* Set to NULL the sub-structures that we won't copy. */
  bl_shared_data->dbl_shared_data->pbl_shared_data->dbl_hdr = NULL;

  boot_pbl_init_shared_routine( osbl_target_pbl_error_handler_wrapper,
                                NULL /* Use FSBL SHA-1 routine */ );

} /* osbl_target_retrieve_shared_info_from_dbl() */

/*===========================================================================

FUNCTION  OSBL_TARGET_GET_CUST_AMSS_PARTI_NAME

DESCRIPTION
  Returns the name of the custom AMSS partition used by the customer.
  If the customer doesn't use a custom name, this function returns NULL.

DEPENDENCIES
  None

RETURN VALUE
  The custom AMSS partition name, or NULL to use the default name

SIDE EFFECTS
  None

===========================================================================*/
char *osbl_target_get_cust_amss_parti_name( void )
{
  /* By default customer doesn't use a custom AMSS partition name, but 
   * customer can change this behavior */
  return NULL;
} /* osbl_target_get_cust_amss_parti_name() */

/*===========================================================================

FUNCTION  OSBL_IS_AMSS_HASH_CHECKING_REQUIRED

DESCRIPTION
  This function is used by the OSBL to determine if AMSS hash checking
  is required.
 
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if AMSS hash checking is required, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean osbl_is_amss_hash_checking_required( void )
{
#ifdef FEATURE_BOOT_VERIFY_AMSS_HASH
  return TRUE;
#else
  return FALSE;
#endif
} /* osbl_is_amss_hash_checking_required() */

/*=============================================================================
FUNCTION  OSBL_GET_NOR_PARTITION_TABLE_LOCATION

DESCRIPTION
  This function will try to find NOR partition table location stored somewhere
  in NOR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECT
  None.
=============================================================================*/
uint8* osbl_get_nor_partition_table_location( void )
{
    return NULL;
} /* osbl_get_nor_partition_table_location() */

