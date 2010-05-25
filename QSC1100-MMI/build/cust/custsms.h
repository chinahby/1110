#ifndef CUSTSMS_H
#define CUSTSMS_H
/*===========================================================================

DESCRIPTION
  Configuration for SMS

  Copyright (c) 2002  by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc60x0/build/cust/custsms.h#1 $ $DateTime: 2006/02/10 15:05:43 $ $Author: milanp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/02   jct     Created
===========================================================================*/

/* -----------------------------------------------------------------------
** Short Message Service
** ----------------------------------------------------------------------- */
/*---------------------------------------------------------------------------
                         Short Messaging Service
---------------------------------------------------------------------------*/

/* Short Message Services (SMS)
*/
#define FEATURE_UASMS

/* Temporary feature for UASMS API changes
*/
#define FEATURE_NEWUASMS

/* NEW_SMS defines the new buffer system for sms messages. This label is a
** temporary until uin is the mainline ui. At that time NEW_SMS ifdef and
** else's, should be deleted. Leaving only the ifdef code.
*/
#define NEW_SMS

#ifdef FEATURE_UASMS
#error code not present
#endif /* FEATURE_UASMS */


#endif /* CUSTSMS_H */

