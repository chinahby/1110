#ifndef SECCRYPTSCHED_H
#define SECCRYPTSCHED_H
/*===========================================================================

                      S E C U R I T Y    S E R V I C E S

               C R Y P T O G R A P H I C    I N T E R F A C E

                 S C H E D U L E R    H E A D E R    F I L E

DESCRIPTION
  This is the header file for the cryptographic services library
  scheduler.
  The scheduler figures out where to run a specific operation of
  an instance.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

 Copyright (c) 2003-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $PVCSPath: L:/src/asw/COMMON/vcs/seccryptsched.h_v   1.1   30 Aug 2003 20:19:50   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/seccryptsched.h#1 $ $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/04   df      changes to support dsp-arm failover
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_SEC

#include "secerrno.h"
#include "seccrypt.h"
#include "seccrypti.h"


/*===========================================================================

                          FUNCTION DECLARATIONS

===========================================================================*/

secerrno_enum_type seccryptsched_get_plat
(
  seccrypt_cmd_enum_type   cmd,
  seccrypt_instance_type*  instance_ptr
);

#endif /* FEATURE_SEC */

#endif /* SECCRYPTSCHED_H */
