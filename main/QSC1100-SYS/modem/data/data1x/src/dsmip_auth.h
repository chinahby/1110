#ifndef DSMIP_AUTH_H
#define DSMIP_AUTH_H

/*===========================================================================
                                      
                          D S M I P _ A U T H . H

DESCRIPTION

 Header file for the Data Services Mobile IP authentication lookup table 
 implementation.

EXTERNALIZED FUNCTIONS

  mip_auth_lookup_alg()
    Returns a function pointer to the authentication algorithm associated
    with a specified security parameter index (SPI).

  mip_auth_lookup_spi()
    Returns the security parameter index (SPI) associated with the specified 
    authentication algorithm function.

  mip_auth_calc_md5_prefix_plus_suffix
    Generates the MD5 "postfix + suffix" result for the provided data 
    (ref RFC2002 S 3.5.1).

  mip_auth_calc_chap
    Generates the CHAP authenticator for the provided data (ref ?).

Copyright (c) 2000-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_auth.h_v   1.3   12 Jul 2002 15:00:26   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_auth.h#2 $ $DateTime: 2007/12/10 21:46:00 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/07 sq/mga/yz Merged changes to support FEATURE_UIM_SUPPORT_3GPD.
05/10/02   jd      Moved SPI & authenticator length def's to dsmip.h
05/07/02   jd      Added mip_auth_retrieve_ss_data prototype
07/17/01   jd      Authentication functions return 16-bit (was 32) length.
07/04/01   jd      Code cleanup and naming made consistent with coding std
03/05/01   jd      Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "dsmip.h"


/*===========================================================================

                      PUBLIC TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
   Authentication lookup table type definition
---------------------------------------------------------------------------*/
typedef struct
{
  uint32   key;                                              /* lookup key */
  uint16   (* handler)();                                    /* handler fn */
} mip_auth_lookup_table;

/*---------------------------------------------------------------------------
   Handler function
---------------------------------------------------------------------------*/
typedef uint16 (* mip_auth_fn)();


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION MIP_AUTH_LOOKUP_ALG

DESCRIPTION
  This function searches an authorisation lookup table for the specified 
  key (SPI) and returns the associated handler if found.

PARAMETERS
  table - pointer to the appropriate authentication lookup table 
  key   - key (SPI) to look up

RETURN VALUE
  Pointer to the authentication function corresponding to 'key', if found.
  NULL if not found.

DEPENDENCIES
  The last entry in 'table' must have the handler field = NULL.  
  This delineates the bound of the table.

SIDE EFFECTS
  None
===========================================================================*/
extern void * mip_auth_lookup_alg
(
  mip_auth_lookup_table *  table,
  uint32                   key
);


/*===========================================================================
FUNCTION MIP_AUTH_LOOKUP_SPI

DESCRIPTION
  This function searches an authorisation lookup table for the specified 
  handler (function) and returns the associated security parameter index,
  if found.

PARAMETERS
  table - pointer to the appropriate authentication lookup table 
  handler - pointer to the authentication function to look up

RETURN VALUE
  key   - key (SPI) corresponding to 'handler', if found.
          set to SPI of last entry in table if not found.

DEPENDENCIES
  The last entry in 'table' must have the handler field = NULL.  
  This delineates the bound of the table.  the SPI should be configured
  to not fool the software or message recipients.

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 mip_auth_lookup_spi
(
  mip_auth_lookup_table *  table,
  uint16                   (* handler)()
);


/*===========================================================================
FUNCTION MIP_AUTH_CALC_MD5_PREFIX_PLUS_SUFFIX

DESCRIPTION
  This function calculates the "MD5 prefix+suffix" authenticator value
  on the data as follows:

    -  the shared secret provided
    -  the data provided
    -  the shared secret again.

  NOTE:  The caller should anticipate the length of the authenticator it 
         expects returned and make sufficient memory available at the 
         address pointed to by the authenticator parameter (see below).

DEPENDENCIES
  None

PARAMETERS
  data          - ptr to the data for which the authenticator is calculated
  data_len      - the length of 'data'
  secret        - pointer to the shared secret value
  secret_len    - the length of 'secret'
  authenticator - the calculated authenticator is returned via this pointer

RETURN VALUE
  Returns the length of the generated authenticator (in bytes)

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 mip_auth_calc_md5_prefix_plus_suffix
(
  void *   data,                   /* start of data to authenticate        */
  uint32   data_len,               /* length of data                       */
  void *   secret,                 /* shared secret used for calculation   */
  uint32   secret_len,             /* length of shared secret              */
  void *   authenticator,          /* storage for calculated authenticator */
  uint32   profile_index           /* nai entry index on RUIM to use       */
);


/*===========================================================================
FUNCTION MIP_AUTH_CALC_CHAP

DESCRIPTION
  This function calculates the "chap" authenticator value
  on the data as follows:


  NOTE:  The caller should anticipate the length of the authenticator it 
         expects returned and make sufficient memory available at the 
         address pointed to by the authenticator parameter (see below).

DEPENDENCIES
  None

PARAMETERS
  data          - ptr to the data for which the authenticator is calculated
  data_len      - the length of 'data'
  secret        - pointer to the shared secret value
  secret_len    - the length of 'secret'
  authenticator - the calculated authenticator is returned via this pointer

RETURN VALUE
  Returns the length of the generated authenticator (in bytes)

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 mip_auth_calc_chap
(
  void *   data,                   /* start of data to authenticate        */
  uint32   data_len,               /* length of data                       */
  void *   secret,                 /* shared secret used for calculation   */
  uint32   secret_len,             /* length of shared secret              */
  void *   challenge,              /* FA challenge value                   */
  uint32   challenge_len,          /* FA challenge length                  */
  void *   authenticator,           /* storage for calculated authenticator */
  uint32   profile_index           /* nai entry index on RUIM to use       */
);


/*===========================================================================

FUNCTION MIP_AUTH_RETRIEVE_SS_DATA()
 
DESCRIPTION
  Determines (and retrieves if necessary) the shared secret data to be
  used when composing the Mobile IP registration request.

PARAMETERS
  mn_ha_ss      - where to store MN-HA shared secret pointer
  mn_ha_ss_len  - where to store length for above
  mn_aaa_ss     - where to store MN-HA shared secret pointer
  mn_aaa_ss_len - where to store length for above

RETURN VALUE
  mn_ha_ss      - set to point to MN-HA shared secret
  mn_ha_ss_len  - set to length of above
  mn_aaa_ss     - set to point to MN-AAA shared secret
  mn_aaa_ss_len - set to length of above

DEPENDENCIES
  None

SIDE EFFECTS
  Modifies global ps_nv_item 

===========================================================================*/
void mip_auth_retrieve_ss_data
(
  mip_session_info_type * session,
  byte ** mn_ha_ss, 
  byte *  mn_ha_ss_len, 
  byte ** mn_aaa_ss, 
  byte *  mn_aaa_ss_len
);

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* DSMIP_AUTH_H */

