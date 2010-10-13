#ifndef RFLL_H
#define RFLL_H

/*===========================================================================


      R F  D r i v e r  L o g i c a l  L a y e r  I n t e r f a c e 
                          H e a d e r  F i l e

DESCRIPTION
  This file contains all declarations and definitions necessary to use
  the RF driver logical layer.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 1999 - 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM6200/DRIVERS/vcs/rfll.h_v   1.0   01 Jul 2002 18:16:26   doerfler  $
$Header: //source/qcom/qct/modem/rfa/rf/common/main/qsc1100/src/rf/rfll.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/17/06   ycl     Modifications for initial MSM7600 build.
04/26/05   sar     Added support for rfll_get_band_mask().
03/05/03   eh      Added RFLL_NV_INVALID_STATE.
11/04/02   sd      Added support for intersystem measurements.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rex.h"

#include "rfcom.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Defines different RF states supported by RF driver.
---------------------------------------------------------------------------*/
typedef enum
{
  RFLL_NV_INVALID_STATE=0,/* RF NV invalid state */
  RFLL_PARKED_STATE,    /* Parked state */
  RFLL_RX_STATE,        /* Rx or idle state */
  RFLL_RXTX_STATE,      /* Rx/Tx state */
  RFLL_SLEEP_STATE,     /* Sleep state */
  RFLL_NUM_STATES       /* Number of RF states */
} rfll_state_enum_type;

/*---------------------------------------------------------------------------
  Device descriptor type definition for logical layer of RF driver.  Data is
  maintained in a structure of this type for each instance of the RF driver.
  The device field is const because rfll_device_desc_table has an entry for
  each device enum, and a discrepancy can occur if device were to be modified.
  E.g. rfll_device_desc_table[RFCOM_TRANSCEIVER_0].device == RFCOM_TRANSCEIVER_0 
  and cannot change.
---------------------------------------------------------------------------*/
typedef struct
{
  const rfcom_device_enum_type device;          /* Device identifier */
  rfcom_mode_enum_type         mode;            /* Device operating mode */
  rfcom_mode_enum_type         intersystem_mode;/* Device intersystem operating
                                                   mode */
  rfll_state_enum_type         state;           /* Device RF state */
  rf_path_enum_type            phys_path;       /* Physical path of device */
} rfll_device_desc_type;

/*---------------------------------------------------------------------------
  RF logical layer device descriptor table.  Note: This should be placed
  in an internal logical layer file and not exported in this file.
---------------------------------------------------------------------------*/
extern rfll_device_desc_type rfll_device_desc_table[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  RFLL_INIT

DESCRIPTION
  This function performs the common RF driver initialization for dual
  mode GSM and WCDMA operation.  It reads common and mode specific RF NV
  parameters from NV and initializes common and mode specific NV tables.
  It also initializes common hardware and mode specific RF hardware for
  GSM and WCDMA modes of operation.  RF driver is placed in parked/sleep
  state.

DEPENDENCIES
  This function must be called before any other RF driver function.
  
RETURN VALUE
  Logical layer device descriptor if initialization successful, otherwise
  NULL.

SIDE EFFECTS
  None

===========================================================================*/
rfll_device_desc_type* rfll_init
(
  rfcom_device_enum_type device,        /* RF device */
  rex_tcb_type *caller_tcb_ptr,         /* Calling task TCB pointer */
  rex_sigs_type task_nv_wait_sig,       /* Task signal to wait for when
                                           reading NV */
  void (*task_wait_func_ptr)( rex_sigs_type )
                                        /* Task's wait function to be called
                                           when reading NV */
);
/*===========================================================================

FUNCTION  RFLL_PARK

DESCRIPTION
  This function places the device to parked mode.

DEPENDENCIES
  None  

RETURN VALUE
  Returns  if operation successful otherwise.

SIDE EFFECTS
  None

===========================================================================*/

uint32 rfll_park
(
  rfll_device_desc_type *dev_desc,      /* Device descriptor */         
  rfcom_mode_enum_type rf_mode,         /* RF mode */
  const void *user_data_ptr,            /* Pointer to user data passed into
                                           callback */
  rfcom_cb_handler_type cb_handler      /* Call back handler */  
);
/*===========================================================================

FUNCTION RFLL_GET_BAND_MASK                                   INTERNAL FUNCTION

DESCRIPTION
  Returns enabled/disabled state of bands set in NV item, NV_RF_HW_BC_CONFIG_I.

DEPENDENCIES
  None

RETURN VALUE
 sys_band_mask_type containing enabled/disabled states of bands. 

SIDE EFFECTS
  None

===========================================================================*/
uint64 rfll_get_band_mask(void );


/*===========================================================================

FUNCTION RFLL_TURN_ON_RF_LDO             

DESCRIPTION
  This function turns on RF related LDOs. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rfll_turn_on_rf_ldo( uint16 delay );

/*===========================================================================

FUNCTION RFLLUMTS_IOCTL             

DESCRIPTION
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
rfcom_ioctl_return_type rfllpark_ioctl
(
  rfll_device_desc_type * dev_desc,
  rfcom_ioctl_param_type request,
  void *pbuf,
  int32 length
);

#endif  /* RFLL_H */
