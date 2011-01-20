#ifndef SIO_PRIV_H
#define SIO_PRIV_H

/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                P R I V A TE    H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 2007-2008 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //depot/asic/msmshared/services/sio/02.00.XX/sio_priv.h#1 $ $DateTime: 2008/10/19 17:40:04 $ $Author: vaghasia $

MM/DD/YY   who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/08   bfc     Added some new SMD DATA ports.
01/11/07   rh      Spin off from sio.h

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "dsm.h"        /* Data service buffer definition                  */
#include "sio.h"        /* Defines all public SIO data types               */

/*===========================================================================

                        FEATURE FLAG MAPPING

===========================================================================*/
#if (defined(FEATURE_HS_USB) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR))
#define SIO_USES_HS_USB
#endif

#if defined(FEATURE_USB) || defined(SIO_USES_HS_USB)
#define SIO_USES_FSHS_USB
#endif

#if (defined (FEATURE_USB_DIAG_NMEA) || defined(FEATURE_USB_DIAG)) && \
   !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef SIO_USES_HS_USB
#define SIO_HAS_USB_SER3
#endif
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#define SIO_DEVICE_NUM_MAX             75
#define SIO_DEVICE_HEAD_MAGIC_NUM      0xDEADBEEF


/*===========================================================================

                      PRIVATE FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION sio_smd_port_close_cb 

DESCRIPTION
  This function is the close callback function that is registered with SMD
  for bridge ports.  It de-registers with the SMD port manager, and calls
  the client callback if one is present. 
      
DEPENDENCIES
  port must be bridge port on dual procssor build
      
RETURN VALUE
  None
    
SIDE EFFECTS
  De-registers with SMD port manager
  
===========================================================================*/
void
sio_smd_bridge_port_close_cb
( 
  void  *cb_data
);




#endif /* SIO_PRIV_H */
