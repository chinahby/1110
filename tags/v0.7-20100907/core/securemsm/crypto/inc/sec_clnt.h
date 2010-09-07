#ifndef SEC_CLNT_H
#define SEC_CLNT_H

/*===========================================================================

                    S E C U R I T Y    S E R V I C E S
                          
			  I P C    C L I E N T

         E X T E R N A L   A P P L I C A T I O N   I N T E R F A C E

DESCRIPTION
  This file defines the SEC IPC client interface used to support
  IPC based security serives functionality

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cryptographic library is initialized by Security Services
  internally and no application specific intialization is required.

 Copyright (c) 2005-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/sec_clnt.h#1 $DateTime: $ $Author: jmiera $					  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/25/05   sb      Created file.

===========================================================================*/


#ifdef	FEATURE_SEC_IPC

/* Include defintions from IPC file. */
#include "seccrypt.ipc"

#endif

#endif /* SEC_CLNT_H */
