#ifndef _OBEXAUTH_H
#define _OBEXAUTH_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_obexauth.h#1 $ 

$Log: $

#4       19 Sept 2006           MH
*  Compose challenge prototype is changed.
===========================================================================*/
/**
@file   
OBEX authentication support
*/

/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_stddefs.h"
#include "oi_status.h"
#include "oi_obexspec.h"
#include "oi_obexcommon.h"



/**
 * Resets OBEX authentication state and frees any dynamically allocated memory.
 */

void OI_OBEXAUTH_Reset(OBEX_COMMON *common);


/**
 * Saves the password string.
 */

OI_STATUS OI_OBEXAUTH_SaveAuthInfo(OBEX_COMMON *common,
                                   const OI_BYTE userId[OI_OBEX_MAX_USERID_LEN],
                                   OI_UINT8 userIdLen,
                                   const OI_CHAR *password);

/**
 * Saves an OBEX authentication challenge digest string.
 */

OI_STATUS OI_OBEXAUTH_SaveChallenge(OBEX_COMMON *common,
                                    OI_OBEX_HEADER *challenge);


/**
 * Saves an OBEX authentication response digest string.
 */

OI_STATUS OI_OBEXAUTH_SaveResponse(OBEX_COMMON *common,
                                   OI_OBEX_HEADER *response);


/**
 * Generate the authentication response to an previous authentication challenge.
 */

void OI_OBEXAUTH_ComposeResponse(OBEX_COMMON *common,
                                 OI_OBEX_HEADER *response);


/**
 * Composes an authentication challenge header.
 */

void OI_OBEXAUTH_ComposeChallenge(const OI_CHAR* privateKey,
                                  OBEX_COMMON *common,
                                  OI_OBEX_HEADER *challenge);

/**
 * Checks that correct response was received to an OBEX authentication
 * challenge.
 */

OI_STATUS OI_OBEXAUTH_Authenticate(OBEX_COMMON *common);


#endif
