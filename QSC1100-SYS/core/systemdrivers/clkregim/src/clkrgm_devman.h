#ifndef __CLKRGM_DEVMAN_H__
#define __CLKRGM_DEVMAN_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          C L O C K  R E G I M E
                        D E V I C E   M A N A G E R
                   E X T E R N A L   D E F I N I T I O N S

                     
GENERAL DESCRIPTION
  This module defines the external interface of the clock regime device
  manager.  This code should be portable across all targets.

EXTERNALIZED FUNCTIONS
  clk_regime_devman_register
  clk_regime_devman_deregister
  clk_regime_devman_request

INITIALIZATION AND SEQUENCING REQUIREMENTS
  clk_regime_rm_init must be called before any other functions


Copyright(c) 2006 by QUALCOMM, Incorporated. All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/clkregim/devman/clkrgm_devman.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/15/07   gfr     Added FEATURE_CLKREGIM_DEVMAN
01/19/07   gfr     Added PMDH and EMDH devices.
12/14/06   gfr     Add VDC device.
12/05/06   gfr     Initial revision.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"



/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/* --------------------------------------------------------------------------
   Support feature
-------------------------------------------------------------------------- */

#define FEATURE_CLKREGIM_DEVMAN


/* --------------------------------------------------------------------------
   Generic clocked device list
-------------------------------------------------------------------------- */

typedef enum
{
  CLKRGM_DEVICE_HBUS,    /* Main system bus        */
  CLKRGM_DEVICE_EBI2,    /* EBI2 bus               */
  CLKRGM_DEVICE_PBUS,    /* Peripherabl bus        */
  CLKRGM_DEVICE_MCPU,    /* Modem ARM              */
  CLKRGM_DEVICE_ACPU,    /* Applications ARM       */
  CLKRGM_DEVICE_MDSP,    /* Modem DSP              */
  CLKRGM_DEVICE_ADSP,    /* Applications DSP       */
  CLKRGM_DEVICE_VDC,     /* Video Core             */
  CLKRGM_DEVICE_PMDH,    /* Primary MDDI host bus  */
  CLKRGM_DEVICE_EMDH,    /* External MDDI host bus */

  CLKRGM_NUM_DEVICES
} clkrgm_device_type;


/* --------------------------------------------------------------------------
   Device parameter types
-------------------------------------------------------------------------- */

/* clkrgm_device_operating_level_type -
 * Enumeration of device operating levels.
 */
typedef enum
{
  /* NORMAL - default operating level
   */
  CLKRGM_DEVICE_OPER_LEVEL_NORMAL,

  /* POWERED - clock to the device may be stopped but it will not be
   * power collapsed
   */
  CLKRGM_DEVICE_OPER_LEVEL_POWERED,

  /* ALWAYS_ON - clock to the device will never be stopped even when
   * halting the ARM clock.
   */
  CLKRGM_DEVICE_OPER_LEVEL_ALWAYS_ON,

  /* FIXED_SPEED - device clock will remain on and at a fixed speed.  This
   * generally means the maximum supported speed.
   */
  CLKRGM_DEVICE_OPER_LEVEL_FIXED_SPEED,

  CLKRGM_NUM_DEVICE_OPER_LEVELS
} clkrgm_device_operating_level_type;


/* clkrgm_device_minimum_speed_type - 
 * Minimum speed in KHz for the given device along with defines for lowest
 * and highest possible.
 */
#define CLKRGM_DEVICE_SPEED_DEFAULT  0
#define CLKRGM_DEVICE_SPEED_MAX      0xFFFFFFFF
typedef uint32 clkrgm_device_minimum_speed_type;


/* clkrgm_device_speed_change_type -
 * Enumeration of which speed change notification this is, either pre or
 * post the speed change.
 */
typedef enum
{
  /* PRE_SPEED_CHANGE - indicates the callback is just before the device
   * speed is being changed.  Note on multi-processor systems this is not
   * guaranteed due to RPC latencies.
   */
  CLKRGM_DEVICE_PRE_SPEED_CHANGE,

  /* POST_SPEED_CHANGE - indicates the callback is after the device
   * speed has been changed.
   */
  CLKRGM_DEVICE_POST_SPEED_CHANGE,

  CLKRGM_NUM_DEVICE_SPEED_CHANGE_POSITIONS
} clkrgm_device_speed_change_type;


/* clkrgm_device_speed_change_handler_type -
 * Function type for the speed change callback.
 */
typedef void (*clkrgm_device_speed_change_handler_type)
(
  clkrgm_device_type               device,
  clkrgm_device_speed_change_type  position,
  uint32                           old_speed_khz,
  uint32                           new_speed_khz
);


/* clkrgm_device_param_type - 
 * Union of above parameter types.
 */
typedef union
{
  clkrgm_device_operating_level_type       operating_level;
  clkrgm_device_minimum_speed_type         minimum_speed;
  clkrgm_device_speed_change_handler_type  speed_change_handler;
} clkrgm_device_param_type;


/* --------------------------------------------------------------------------
   Device request type
-------------------------------------------------------------------------- */

typedef enum
{
  /* SET_OPERATING_LEVEL - requests that the operating level for the device
   * be set to at least the given level.  The final operating level is the
   * highest of all requested.
   */
  CLKRGM_DEVICE_REQUEST_SET_OPERATING_LEVEL,

  /* SET_MINIMUM_SPEED - requests that the minimum speed for the device
   * be set to the given value (in KHz).  The final speed will be at least
   * the highest of all requested minimums.
   */
  CLKRGM_DEVICE_REQUEST_SET_MINIMUM_SPEED,

  /* SPEED_CHANGE_NOTIFICATION - requests that a callback be made when
   * the device changes clock speeds.
   */
  CLKRGM_DEVICE_REQUEST_SPEED_CHANGE_NOTIFICATION,

  CLKRGM_NUM_DEVICE_REQUESTS
} clkrgm_device_request_type;


/* --------------------------------------------------------------------------
   Client group
-------------------------------------------------------------------------- */

typedef enum
{
  /* DEFAULT - normal group for clients
   */
  CLKRGM_DEVMAN_CLIENT_GROUP_DEFAULT,

  /* QDSP_SERVICES - only group that is allowed to set the speed for the
   * ADSP and MDSP.  Currently this is not strictly enforced however.
   */
  CLKRGM_DEVMAN_CLIENT_GROUP_QDSP_SERVICES,

  CLKRGM_NUM_DEVMAN_CLIENT_GROUPS
} clkrgm_devman_client_group_type;


/* --------------------------------------------------------------------------
   Device client type - note that the format of this stucture is private
   and should not be accessed directly.
-------------------------------------------------------------------------- */

typedef struct
{
  clkrgm_device_operating_level_type       operating_level;
  clkrgm_device_minimum_speed_type         minimum_speed;
  clkrgm_device_speed_change_handler_type  speed_change_handler;
} clkrgm_device_params_type;


typedef struct clkrgm_devman_client_struct
{
  /* Magic flag indicating if this client was registered */
  uint32                              magic;

  /* Client group */
  clkrgm_devman_client_group_type     group;

  /* Client tag used for internal tracking */
  uint32                              tag;

  /* Current client parameters */
  clkrgm_device_params_type           params[CLKRGM_NUM_DEVICES];

  /* Linked list pointers of all registered clients */
  struct clkrgm_devman_client_struct *next;
  struct clkrgm_devman_client_struct *prev;

} clkrgm_devman_client_type;



/*===========================================================================

                             FUNCTION PROTOTYPES

===========================================================================*/



/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_REGISTER

  DESCRIPTION   This function registers a new client with the device
                manager.  Should never be called on a client structure
                previously registered.

  PARAMETERS    client       - the client structure to register
                client_group - the group this client is part of

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_register
(
  clkrgm_devman_client_type       *client,
  clkrgm_devman_client_group_type  client_group
);


/*==========================================================================

  FUNCTION      CLK_REGIME_DEVICE_MANAGER_DEREGISTER

  DESCRIPTION   This function deregisters a client with the device
                manager.  Should never be called on a client structure
                that has not been previously registered.

  PARAMETERS    client - the client structure to deregister

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_deregister
(
  clkrgm_devman_client_type *client
);


/*==========================================================================

  FUNCTION      CLK_REGIME_DEVMAN_REQUEST

  DESCRIPTION   This function processes a device manager request

  PARAMETERS    client - the client structure 
                device - the device in question
                request - the request type
                param  - the parameter structure for the request

  DEPENDENCIES  None.

  RETURN VALUE  None.

  SIDE EFFECTS  None.

==========================================================================*/

void clk_regime_devman_request
(
  clkrgm_devman_client_type *client,
  clkrgm_device_type         device,
  clkrgm_device_request_type request,
  clkrgm_device_param_type  *param
);


#endif /* !__CLKRGM_DEVMAN_H__ */

