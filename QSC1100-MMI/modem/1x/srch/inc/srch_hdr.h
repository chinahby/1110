#ifndef SRCH_HDR_H
#define SRCH_HDR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S R C H  _ H D R ---  H E A D E R   F I L E 

GENERAL DESCRIPTION
  This module contains header information pertaining to srch_hdr.  srch_hdr 
  contains functions which allow the HDR protocol to interface with the SRCH
  task.

EXTERNALIZED FUNCTIONS
  srch_hdr_in_service
  srch_hdr_get_channel_estimate
  
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2005
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_hdr.h#1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
09/08/05   ejv     Implementation, first cut.
===========================================================================*/ 

#include "customer.h"
#include "comdef.h"

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Macros
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Prototypes
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION       SRCH_HDR_IN_SERVICE
DESCRIPTION    This function returns true if srch is on the system.
               It returns false if search is either inactive or trying
               to get on the system. 

DEPENDENCIES   None

RETURN VALUE   TRUE - srch in on the system; otherwise, FALSE 
SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_hdr_in_service (void);

/*===========================================================================

FUNCTION       SRCH_HDR_GET_CHANNEL_ESTIMATE
DESCRIPTION    This function returns an estimate of the channel in 1/8 db units.
               This function is sleep safe and can be called in another task
               context 

DEPENDENCIES   None

RETURN VALUE   Channel estimate in 1/8 db units 
SIDE EFFECTS   None

===========================================================================*/
extern uint32 srch_hdr_get_channel_estimate (void);

#endif /* SRCH_HDR_H */

