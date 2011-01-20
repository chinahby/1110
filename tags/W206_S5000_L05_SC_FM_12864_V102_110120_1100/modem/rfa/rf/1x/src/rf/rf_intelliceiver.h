#ifndef RF_INTELLICEIVER_H
#define RF_INTELLICEIVER_H

#ifdef RF_HAS_POWER_MODE_LIB
/*===========================================================================

                         RF IntelliCeiver Interface

DESCRIPTION
  Contains the public interface definitions for the IntelliCeiver software
  control module.

Copyright (c) 2005, 2006, 2007 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rf_intelliceiver.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/07   dyc     Merge QSC60x5 to common.
           bmg     IntelliCeiver port for QSC60x5
03/20/06   bmg     Updated prototype for rf_intelliceiver_set_gpio to use
                   GPIO header file symbols.  This allows the IntelliCeiver
                   GPIO to be forced to an input.
03/31/05   bmg     Moved rf_intelliceiver_init to pubilc API
01/17/05   bmg     Created.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "rflib_share.h"
#include "rflib.h"


/*===========================================================================

                              DATA DEFINITIONS

===========================================================================*/

/*===========================================================================
              
                       FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION    RF_INTELLICEIVER_INIT

DESCRIPTION
  Perform one-time intitialization of the IntelliCeiver module.
   
PARAMETERS
  Receiver path.
  
DEPENDENCIES
  Must be called before any other IntelliCeiver functions, and only
  called once per boot.

RETURN VALUE
  None.

SIDE EFFECTS
  Internal IntelliCeiver state is placed in a known start condition.

===========================================================================*/
void rf_intelliceiver_init( rf_path_enum_type path );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_SET_GPIO

DESCRIPTION
  Configures the IntelliCeiver GPIO feedback pin.
   
PARAMETERS
  GPIO_SignalType enum corresponding to the GPIO the RFR was connected to.
  This must be a GPIO_INPUT_xx symbol.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The current configuration of the specified GPIO is lost, and it is
  reconfigured as an interrupt generating input.

===========================================================================*/
void rf_intelliceiver_set_gpio( rf_path_enum_type path, GPIO_SignalType GPIO );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_RESUME

DESCRIPTION
   Resume IntelliCeiver operation after it was paused for a critical
   section.
   
PARAMETERS
  Receiver path.
  
DEPENDENCIES
  IntelliCeiver must have been paused with the RF_INTELLICEIVER_PAUSE API.

RETURN VALUE
  None.

SIDE EFFECTS
  The IntelliCeiver begins tracking again.

===========================================================================*/
void rf_intelliceiver_resume( rf_path_enum_type );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_PAUSE

DESCRIPTION
   Pause IntelliCeiver operation in preparation of a critical section where
   RF and RXF settings must change in an atomic transaction.
   
PARAMETERS
  Receiver path.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RF power mode stops tracking until RF_INTELLICEIVER_RESUME is called.

===========================================================================*/
void rf_intelliceiver_pause( rf_path_enum_type );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_TUNE

DESCRIPTION
   Inform IntelliCeiver of a tuning update.
   
PARAMETERS
  Receiver path, new band, and new channel.
  
DEPENDENCIES
  IntelliCeiver must be stopped or paused.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_intelliceiver_tune( rf_path_enum_type, rf_card_band_type, uint16 );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_GET_POWER_MODE

DESCRIPTION
   Returns the current IntelliCeiver power mode.
   
PARAMETERS
  Receiver path.
  
DEPENDENCIES
  None.

RETURN VALUE
  RFLIB_NORMAL_POWER_MODE, RFLIB_LOW_POWER_MODE_1, RFLIB_LOW_POWER_MODE_2.

SIDE EFFECTS
  None.

===========================================================================*/
rflib_power_mode_type rf_intelliceiver_get_power_mode( rf_path_enum_type );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_SET_POWER_MODE

DESCRIPTION
   Override IntelliCeiver power mode decision and force the IntelliCeiver
   into a given state.
   
PARAMETERS
  Receiver path and override power mode.
  
DEPENDENCIES
  IntelliCeiver must be actively running.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_intelliceiver_set_power_mode( rf_path_enum_type, rflib_power_mode_type );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_FORCE_HIGH_POWER

DESCRIPTION
  Enables and disabled IntelliCeiver high power override mode.
   
PARAMETERS
  boolean value to indicate override.  True means the IntelliCeiver should
  be forced into high power mode.  False means the IntelliCeiver can return
  to normal operation, tracking the power mode as necessary.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rf_intelliceiver_force_high_power( rf_path_enum_type, boolean );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_IS_ENABLED

DESCRIPTION
  Returns true if the IntelliCeiver has been configured and is being
  used in the current chipset cofiguration.
   
PARAMETERS
  Receive path.
  
DEPENDENCIES
  None.

RETURN VALUE
  True if IntelliCeiver has been configured and should be used, false
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean rf_intelliceiver_is_enabled( rf_path_enum_type );


/*===========================================================================

FUNCTION    RF_INTELLICEIVER_GET_LOWEST_POWER_MODE

DESCRIPTION
  Returns the lowest power mode supported by the current chipset configuration.
   
PARAMETERS
  Receive path.
  
DEPENDENCIES
  rf_intelliceiver_init() must have been called.

RETURN VALUE
  rflib_power_mode_type enumeration of the lowest power mode for the
  current chipset.  Setting the power mode to something lower than
  the value returned by this API is invalid.

SIDE EFFECTS
  None.

===========================================================================*/
rflib_power_mode_type rf_intelliceiver_get_lowest_power_mode( rf_path_enum_type );


#endif /* RF_HAS_POWER_MODE_LIB */

#endif /* RFLIB_INTELLICEIVER_H */


