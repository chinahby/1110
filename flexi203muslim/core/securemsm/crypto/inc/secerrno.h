#ifndef SECERRNO_H
#define SECERRNO_H

#ifdef FEATURE_SEC

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

            E R R O R   N U M B E R   D E F I N I T I O N S

DESCRIPTION
  This contains the definition of the Security Services return codes 
  (error numbers). Functions using this definition return an error code
  to indicate the status of execution.
 

Copyright (c) 2000-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/secerrno.h_v   1.9   30 Aug 2003 20:19:56   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secerrno.h#1 $ $DateTime: 2007/11/09 11:58:10 $ $Author: jmiera $


when       who     what, where, why
--------   ---     -----------------------------------------------------------------------
06/01/05   rv      undefined 'errno' & '_errno' defined by VS .NET for sirius LTK builds.
06/10/02   om      Changed secerrno_enum_type to be typedef'd.
05/07/01   yll     Added certificate parsing and verification related errno.
10/23/00   om      Created Module

==========================================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef PLATFORM_LTK
#error code not present
#else
#include "errno.h"
#endif

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/

#ifdef PLATFORM_LTK
#error code not present
#endif

/* Map the SEC errno type to the common errno */
typedef errno_enum_type secerrno_enum_type;

/* Callback fptr type */
typedef void (*secerrno_cb_fptr_type)( void*, secerrno_enum_type );

#endif /* FEATURE_SEC */
#endif /* SECERRNO_H */

