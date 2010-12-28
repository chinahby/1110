#ifndef SECUTIL_H
#define SECUTIL_H

#include "customer.h"

#ifdef FEATURE_SEC

/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/core/securemsm/crypto/main/latest/inc/secutil.h#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/09   qxu     port INTLOCK/INTFREE to new critical section mechnism on QDSP6 Hexgon. 
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "secerrno.h"
#include "msg.h"

/*===========================================================================

                 INTLOCK definition 

===========================================================================*/
#ifdef IMAGE_QDSP6_PROC
#include "ixlock.h"
extern strLockInfo  g_seclock; /* Rex based critical section structure */
#define SEC_LOCK_INIT()         IxLockInit(&g_seclock, LOCK_TASK_ONLY)
#define SEC_LOCK()	        IxLock(&g_seclock);
#define SEC_UNLOCK()	        IxUnlock(&g_seclock);	
#define SEC_LOCK_SAV()	        IxLock(&g_seclock);
#define SEC_UNLOCK_SAV()	IxUnlock(&g_seclock);	
#define SEC_TASK_LOCK()	        IxLock(&g_seclock);
#define SEC_TASK_UNLOCK()	IxUnlock(&g_seclock);	
#define SEC_INT_TASK_LOCK()	IxLock(&g_seclock);
#define SEC_INT_TASK_UNLOCK()	IxUnlock(&g_seclock);

#else
#define SEC_LOCK_INIT() 
#define SEC_LOCK()	        INTLOCK()
#define SEC_UNLOCK()	        INTFREE()	
#define SEC_LOCK_SAV(isave)     INTLOCK_SAV(isave)
#define SEC_UNLOCK_SAV(isave)   INTFREE_SAV(isave)	
#define SEC_TASK_LOCK()	        TASKLOCK()
#define SEC_TASK_UNLOCK()	TASKFREE()	
#define SEC_INT_TASK_LOCK()	TASKLOCK();INTLOCK()
#define SEC_INT_TASK_UNLOCK()	INTFREE();TASKFREE()	
#endif

/* A substitute for changing all the uses of secutil_log_opaque at once. */
#define secutil_log_opaque(pid, pdata, plen)  \
  secutil_log_opaque_level( (pid), (pdata), (plen), MSG_LVL_MED);

/* SSL Opaques (Byte arrays) Logging ID's */
typedef enum
{
  /* Generic log ID, print extra msg before */
  SECUTIL_OPAQUE_UNKNOWN = 0x0,

  /* Key exchange parameters */
  SECUTIL_OPAQUE_DH_BASE,                   /* DH common base        */
  SECUTIL_OPAQUE_DH_MOD,                    /* DH common modulo      */
  SECUTIL_OPAQUE_DH_EXP,                    /* DH exponent           */
  SECUTIL_OPAQUE_DH_CLNT_PK,                /* DH client public key  */
  SECUTIL_OPAQUE_DH_SVR_PK,                 /* DH server public key  */
  SECUTIL_OPAQUE_DH_CLNT_SK,                /* DH client secret key  */
  SECUTIL_OPAQUE_DH_SVR_SK,                 /* DH server secret key  */
  SECUTIL_OPAQUE_RSA_CLNT_SK,               /* RSA client secret key */
  SECUTIL_OPAQUE_RSA_CLNT_EK,               /* RSA client encr. ket  */

  /* Generic Handshake Protocol ID's */
  SECUTIL_OPAQUE_SVR_MD5_HASH,              /* HSP MD5 hash (server) */
  SECUTIL_OPAQUE_SVR_SHA_HASH,              /* HSP SHA hash (server) */
  SECUTIL_OPAQUE_SVR_VER_DATA,              /* HSP Verify Data (server) */
  SECUTIL_OPAQUE_CLNT_MD5_HASH,             /* HSP MD5 hash (client) */
  SECUTIL_OPAQUE_CLNT_SHA_HASH,             /* HSP SHA hash (client) */
  SECUTIL_OPAQUE_CLNT_VER_DATA,             /* HSP Verify Data (client) */

  /* Key Generation ID's */
  SECUTIL_OPAQUE_KEY_MATERIAL,
  SECUTIL_OPAQUE_MASTER_SECRET,
  SECUTIL_OPAQUE_CLN_MAC_SECRET,
  SECUTIL_OPAQUE_CLN_ENC_KEY,
  SECUTIL_OPAQUE_CLN_ENC_IV,
  SECUTIL_OPAQUE_SVR_MAC_SECRET,
  SECUTIL_OPAQUE_SVR_ENC_KEY,
  SECUTIL_OPAQUE_SVR_ENC_IV
} secutil_opaque_logid_enum_type;

/* Provide QXDM output based on a run-time message level.
*/
#define SEC_MSG_LVL(m_lvl, x_fmt, a, b, c) \
  do{                              \
    switch( m_lvl)                 \
    {                              \
    default:                       \
    case MSG_LVL_FATAL:            \
      MSG_FATAL(x_fmt, a, b, c);   \
      break;                       \
    case MSG_LVL_ERROR:            \
      MSG_ERROR(x_fmt, a, b, c);   \
      break;                       \
    case MSG_LVL_HIGH:             \
      MSG_HIGH(x_fmt, a, b, c);    \
      break;                       \
    case MSG_LVL_MED:              \
      MSG_MED(x_fmt, a, b, c);     \
      break;                       \
    case MSG_LVL_LOW:              \
      MSG_LOW(x_fmt, a, b, c);     \
      break;                       \
    }/* endswitch */               \
  }while(0)


/*===========================================================================

FUNCTION SECUTIL_FILL_RND_DATA

DESCRIPTION
  This function gets random data from the searcher task and merges it into
  the random data buffer storage area.

PARAMETERS
  rnd_data_ptr - Pointer to collected random data
  rnd_data_len - Length of collected random data in bytes

DEPENDENCIES
  The task calling this function is assumed to have higher priority
  than the tasks using the random data. 
  
RETURN VALUE    
  none

SIDE EFFECTS    
  Updates the random data buffer storage area.
  
===========================================================================*/

void secutil_fill_rnd_data
(
  uint8*  rnd_data_ptr,
  uint16  rnd_data_len
);

/*===========================================================================

FUNCTION SECUTIL_GET_RANDOM

DESCRIPTION     
  This function is used to obtain a random number for Security
  layer encryption functions.  The buffer is guaranteed to have no
  string of 8 bits set to zero in a row.
  If sufficient random data has been collected, the random
  buffer is returned.  Otherwise, no data is returned.

PARAMETERS
  rnd_num_ptr - Buffer to be filled in with the random data.
  rnd_num_len - Amount of Random Data requested (max = 128).

DEPENDENCIES    
  The tasks calling this function are assumed to have lower priority
  than the task filling the random data.

RETURN VALUE
  E_SUCCESS  - if enough data was collected
  E_AGAIN    - if more data need to be collected.

SIDE EFFECTS    None
  
===========================================================================*/
secerrno_enum_type secutil_get_random
(
  uint8*  random_ptr,
  uint16  random_len
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION secutil_get_random */
/*~ PARAM OUT random_ptr VARRAY random_len LENGTH random_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/*===========================================================================

FUNCTION SECUTIL_GET_PKCS_RANDOM

DESCRIPTION     
  This function is used to obtain a random number for Security
  layer encryption functions.  The buffer is guaranteed to have no
  string of 8 bits set to zero in a row.
  If sufficient random data has been collected, the random
  buffer is returned.  Otherwise, no data is returned.

  Now using csprng as random source;  csprng is seeded periodically
  by searcher data as before;

PARAMETERS
  rnd_num_ptr - Buffer to be filled in with the random data.
  rnd_num_len - Amount of Random Data

DEPENDENCIES    

RETURN VALUE
  E_SUCCESS     if success
  E_BAD_ADDRESS if NULL pointer was passed

SIDE EFFECTS  
  None
===========================================================================*/
secerrno_enum_type secutil_get_pkcs_random
(
  uint8*  random_ptr,
  uint16  random_len
);

/*===========================================================================
FUNCTION SECUTIL_LOG_OPAQUE_LEVEL

DESCRIPTION
  This function logs arbitrary byte arrays from the handshake phase of SSL.
  The message is output only if the severity level parameter is greater than
  or equal to the compile-time message limit.

DEPENDENCIES
  None
  
PARAMETERS
  session_ptr    - Pointer to session parameter belongs to

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void secutil_log_opaque_level
(
  const secutil_opaque_logid_enum_type  pid,
  const uint8*                          pdata,
  const uint16                          plen,
  const uint32                          msg_lvl
);

/*===========================================================================

FUNCTION SECUTIL_GET_SFS_KEY

DESCRIPTION     
  Returns a unique key stored in SFS on the modem side.

PARAMETERS
  key_ptr - Buffer to be filled in with the key data.
  key_len - Length of buffer passed in

DEPENDENCIES    
  None

RETURN VALUE
  E_SUCCESS if success, another error code on failure

SIDE EFFECTS
  None
  
===========================================================================*/
secerrno_enum_type secutil_get_sfs_key
(
  uint8*  key_ptr,
  uint32  key_len
);

#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION secutil_get_sfs_key */
/*~ PARAM OUT key_ptr VARRAY key_len LENGTH key_len */
#endif /* FEATURE_HTORPC_METACOMMENTS */

#include "secutil_v.h"

#endif /* FEATURE_SEC */

#endif /* SECUTIL_H */
/*lint +e760 */
