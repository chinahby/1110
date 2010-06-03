#ifndef SRCHCOM_H
#define SRCHCOM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S E A R C H C O M  ---  H E A D E R   F I L E

GENERAL DESCRIPTION
  This module contains header information pertaining to the common state
  machine.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2005 - 2006.
                QUALCOMM Incorporated. All Rights Reserved.
                QUALCOMM Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/src/common/srchcom_sm.h#1 $

when       who      what, where, why
--------   ---      ----------------------------------------------------------
10/13/06   trc     Changes for new STM grammar
06/29/05   kwo      Centralized int cmd queue
05/31/05   rng      Initial Version - Merged from SHDR Sandbox.

===========================================================================*/

/*===========================================================================

FUNCTION SRCHCOM_INIT

DESCRIPTION
  This function initializes the Searcher common state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void srchcom_init( void );

#endif /* SRCHCOM_H */
