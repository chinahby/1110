#ifndef SECHSHARM_H
#define SECHSHARM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       S E C U R I T Y   S E R V I C E S

                       H A S H I N G   F U N C T I O N S

                   ( A R M   I M P L E M E N T A T I O N S )

GENERAL DESCRIPTION
  This sofware module contains the source code for the Security Services
  Crypto-Interface layer for message digest specific to the ARM.

EXTERNAL FUNCTIONS

  sechsh_create_digest() - Creates the message digest

Copyright (c) 2003-2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/COMMON/vcs/sechsharm.h_v   1.1   30 Aug 2003 20:20:00   omichael  $
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/src/sechsharm.h#1 $ 
  $DateTime: 2007/06/26 13:17:46 $ $Author: jmiera $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/04   df      added proto for sechsharm_platform_init to support dsp-arm
                   failover.
05/04/04   rwh     Removed ARM-platform-specific fields.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_SEC

/*===========================================================================

                     CONSTANTS AND TYPE DECLARATIONS

===========================================================================*/

#ifdef FEATURE_SEC_ARM_CRYPTO

typedef struct 
{
  void*     dummy_ptr;
} sechsharm_ctx_type;

struct sechsharm_instance_s
{
  sechsharm_ctx_type  ctx;
};

/*===========================================================================

                        FUNCTION DECLARATIONS

===========================================================================*/

extern void sechsharm_powerup_init( void );

/*===========================================================================

FUNCTION SECHSHARM_CREATE_DIGEST

DESCRIPTION
  This function will create a message digest using the 
  specified algorithm executing on the ARM.

DEPENDENCIES
  The hash mode should be properly set by calling sechsh_set_param.

RETURN VALUE
  Status of request.
    E_SUCCESS if successful,
    E_FAILURE if failed to process the request.

SIDE EFFECTS
  None.
===========================================================================*/

secerrno_enum_type sechsharm_create_digest
(
  seccrypt_instance_type*  instance_ptr
);

/*===========================================================================

FUNCTION SECHSHARM_PLATFORM_INIT()

DESCRIPTION
  Re-initialization of ARM platform-specific data after switching
  platforms.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void sechsharm_platform_init
(
  seccrypt_instance_type*      instance_ptr
);

extern void sechsharm_op_complete_cb
(
  seccrypt_instance_type*  instance_ptr
);

#else /* FEATURE_SEC_ARM_CRYPTO */

/* If the ARM version is not compiled in make this a dummy */
typedef struct
{
  void*  dummy;
} sechsharm_instance_type;

#endif /* FEATURE_SEC_ARM_CRYPTO */

#endif /* FEATURE_SEC */

#endif /* SECHSHARM_H */
