#ifndef _HSU_AL_SER_BASE_I_H_
#define _HSU_AL_SER_BASE_I_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
"Base Class" Internal Header File

GENERAL DESCRIPTION
Contains common API for any serial I/F extending the "base class".

EXTERNALIZED FUNCTIONS

Copyright (c) 2006, 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/30/06 esh  First revision of file  
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ser_base.h"

#include "uw_device_cdc.h"
#include "jtypes.h"

/*==============================================================================
Constants and Macros
==============================================================================*/
/*===========================================================================

FUNCTION al_ser_base_ctx_is_valid

DESCRIPTION
Checks if the received context, typically received when a callback is invoked 
by the CDC FD, is a valid hsu_al_ser_base_data_for_ser_if and not a bad pointer 
cast.

DEPENDENCIES
None

RETURN VALUE
TRUE - context is a valid hsu_al_ser_base_data_for_ser_if. FALSE - Otherwise.

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_i_ctx_is_valid
(
  const hsu_al_ser_base_data_for_ser_if* ctx         
                                      /* Pointer to serial I/F data struct,
                                      as the context to be tested          */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_uninit

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized, or as an initializer for the given context when the very first 
hsu_al_ser_base_init is called.

DEPENDENCIES
See cdc_uninit_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
The data structure corresponding to the given context is invalidated.

===========================================================================*/
jresult_t hsu_al_ser_base_i_uninit
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_rx_done

DESCRIPTION
Rx Callback whenever the serial CDC FD wishes to notify a buffer has been received
over the data class I/F.

DEPENDENCIES
See cdc_read_complete_cb for more information

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_i_rx_done
(
  cdc_iostatus_t status,                               /* Operation status */
  void* buffer_ptr,                                  /* Ptr to buffer used */
  juint32_t size,                                /* buffer size (in bytes) */
  jbool_t transfer_complete,                  /* is this transfer complete */
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_tx_done

DESCRIPTION
Tx Callback whenever the serial CDC FD wishes to notify a buffer has been sent 
over the data class I/F.

DEPENDENCIES
See cdc_write_complete_cb for more information

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_i_tx_done
(
  cdc_iostatus_t status,                               /* Operation status */
  void *buffer_ptr,                                  /* Ptr to buffer used */
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_enabled

DESCRIPTION
Enabled callback whenever the serial CDC FD wishes to notify that the interface
is enabled.

DEPENDENCIES
See cdc_enabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_base_i_enabled
(
  cdc_appctx_t ctx,                           /* Context passed in callback */
  boolean issue_new_read                      /* determines whether to issue a 
                                                 new cdc_read command */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_disabled

DESCRIPTION
Disabled callback whenever the serial CDC FD wishes to notify that the interface
is disabled.

DEPENDENCIES
See cdc_disabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_base_i_disabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_resumed

DESCRIPTION
Callback invoked whenever the serial CDC FD wishes to notify that the interface
can resume operations.

DEPENDENCIES
See cdc_resumed_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_base_i_resumed
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);



#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND

/*===========================================================================

FUNCTION hsu_al_ser_enable_byte_pad

DESCRIPTION
This function is called when (and if) a setup packet is received from the host 
that request to support the workaround for HW 1/2 byte remainder bug.
(this setup packet is received as a result of sending an MDLM descriptor by 
the device)

NOTE:
Byte-pad is enabled for all serial instances! 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_enable_byte_pad(void);

#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */



#endif /* _HSU_AL_SER_BASE_I_H_ */
