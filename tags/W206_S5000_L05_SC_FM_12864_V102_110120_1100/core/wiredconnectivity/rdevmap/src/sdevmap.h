
#ifndef SDEVMAP_H
#define SDEVMAP_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S E R I A L   D E V I C E   M A P P E R   S E R V I C E S 

                         H E A D E R    F I L E
                            

GENERAL DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial Device Mapper (SDM).  
  
EXTERNALIZED FUNCTIONS

  sdm_init()
    Initialize Serial Device Mapper Services

  sdm_get_port_id()
    Returns the current SIO Port mapped to the specified Application.
                                                     
  sdm_uart2_opened()
    Informs the Serial Device Mapper that the second UART has been opened.

  sdm_uart2_closed()
    Informs the Serial Device Mapper that the second UART has been closed.

  sdm_mc_dev_map_from_nv()
    Stores the port map retrieved from NV
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sdm_init() must be called prior to any of the other functions of
  this module.
                 
Copyright (c) 1999, 2000 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/sdevmap.h_v   1.2   15 Dec 2000 11:03:44   dpendlet  $
  $Header: //depot/asic/msmshared/services/rdevmap/02.00.XX/sdevmap.h#1 $ $DateTime: 2008/10/19 19:39:23 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/00   dwp     Make SDM port map interfaces mutually exclusive with
                   FEATURE_RUNTIME_DEVMAP.
11/14/00   dwp     Added support for FEATURE_RUNTIME_DEVMAP.
07/27/00   dwp     Added support for FEATURE_USB_DIAG and FEATURE_BT.
07/05/00   jwh     Fixed featurization with regard to FEATURE_DS, etc.
04/09/00   jwh     Added sdm_mc_dev_map_from_nv().
04/08/00   jwh     Added sdm_config_type, sdm_uart2_opened() and 
                   sdm_uart2_closed().
10/18/99   jwh     Initial Release.
10/05/99   jwh     Created module.

===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

#include "customer.h"              /* Customer Specific Definitions        */

#ifdef FEATURE_SERIAL_DEVICE_MAPPER

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION SDM_INIT                                     EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes the Serial Device Mapper Services

DEPENDENCIES
  Should be called just once.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void sdm_init
(
  void
); 



#endif /* FEATURE_SERIAL_DEVICE_MAPPER */

#endif /* SDEVMAP_H */

