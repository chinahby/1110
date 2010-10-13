#ifndef PS_PPP_LOGGING_H
#define PS_PPP_LOGGING_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     P S _ P P P _ L O G G I N G . H

GENERAL DESCRIPTION

  Contains the necessary routines to log framed or unframed PPP frames.

EXTERNALIZED FUNCTIONS

  ppp_log_frames_ext()
    This function logs the PPP-in-HDLC frames. The function iterates
    through the PPP packet chain, logging each packet in succession.

Copyright (c) 2001-2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.
   
  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_logging.h_v   1.1   13 Feb 2003 20:14:10   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_logging.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/13/03    jd     removed old ppp_log_frames prototype, ppp_log_frames_ext 
                   prototype updated to use device instead of iface
02/13/03    jd     removed old ppp_log_frames implementation, no longer used
09/12/02    vr     removed redundant #defines
09/06/02   igt     added extended ppp logging capabilities under
                   FEATURE_DEBUG_PPP_LOG_EXT
02/26/02   sjy     Wrapped header file with FEATURE_DS_PPP_LOGGING.
06/20/01   sjy     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS_PPP_LOGGING

#include "dsm.h"
#include "ps_ppp_defs.h"

/*===========================================================================

                      EXPORTED DATA TYPES FOR MODULE

===========================================================================*/
/*-------------------------------------------------------------------------
  These are to help the caller of the extended ppp logging to describe
  the nature of ppp data being passed in. 

  The log code for the diagnostic packet is generated based on these
  and the interface values.
-------------------------------------------------------------------------*/
typedef enum
{
  PPP_LOG_FRAMED_RX = 0,
  PPP_LOG_FRAMED_TX = 1,
  PPP_LOG_UNFRAMED_RX = 2,
  PPP_LOG_UNFRAMED_TX = 3
} pppi_log_flavour_e_type;


/*===========================================================================

                      EXPORTED FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION PPP_LOG_FRAMES_EXT

DESCRIPTION

  This function logs the provided PPP frame (HDLC framed or unframed) on
  the diagnostic interface.


  The function iterates through the input dsm chain, which may contain
  multiple PPP, handling each packets each packet in succession.

  It may be called with framed or unframed ppp data, i.e. before or after
  the HW/SW framing/unframing has occurred.


PARAMETERS
  item_ptr      - pointer to the dsm chain to be logged
  flavour       - The characteristics of this packet,
                  i.e.  framed/unframed, rx/tx
  iface         - the interface over which the packet is passing

  This function constructs the appropriate log code from the ppp_data and
  iface parameters.

  LOG CODE RANGE      DIRECTION      FRAMING
  --------------      ---------      -------
  275-290             Receive        HDLC
  291-306             Transmit       HDLC
  307-322             Receive        (none)
  323-338             Transmit       (none)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void ppp_log_frames_ext
(
  dsm_item_type            *item_ptr,
  pppi_log_flavour_e_type  flavour,
  ppp_dev_enum_type        device
);  

#endif /* FEATURE_DATA_PS_PPP_LOGGING */

#endif /* PS_PPP_LOGGING_H */
