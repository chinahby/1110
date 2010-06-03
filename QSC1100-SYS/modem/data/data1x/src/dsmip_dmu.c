/*============================================================================

                  M O B I L E   I P   D Y N A M I C 

                  K E Y   U P D A T E   M O D U L E 

GENERAL DESCRIPTION

  This module contains all functions required to implement the 
  Mobile IP Dynamic Key Update (version 1.3, rev 6) protocol.  

  This module makes use of the security/crypto and EFS API's.

  This module is called from the PS task context.

  The DMSS DMU implementation implements the following encryption only:
  - RSA-1024 Key data encryption (ATV=1) for DMU version 1.3 (DMUV=0)
  - Cleartext operation (DMUV=7) for interoperability testing support

  The mobile is provisioned with at least one RSA-1024 public key, stored 
  as an EFS file.  (The file format is described in the appropriate read 
  function's comment block)

  In the future, public keys may be stored in compliance with the 
  Distinguished Encoding Rules (DER) format. 

EXTERNALIZED FUNCTIONS

  dmu_init()                   - initialize the DMU module
  dmu_gen_key_data             - generate temporary random keys 
  dmu_pkoid_supported          - Check if a public key exists for given PKOID 
  dmu_encrypt_key_data         - encrypt the temporary random keys 
  dmu_rand_sig_handler         - perform DMU random generation on signal
  dmu_commit_key_data          - commit provided shared secrets to NV

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dmu_init must be called before any of the other external API's.

Copyright (c) 2001-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/dsmip_dmu.c_v   1.10   14 Feb 2003 17:35:44   jeffd  $                      
$Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_dmu.c#4 $                       $DateTime: 2007/12/12 04:31:44 $ $Author: nsivakum $
 
when        who    what, where, why
--------    ---    -----------------------------------------------------------
11/19/07    sn     Added "AEEstd.h".
11/07/07    sn     Replaced banned string APIs.
08/28/07    sc     Changed the signature of dmu_gen_key_data() so that it
                   returns Success or Fail (boolean)
04/10/07    as     Removed featurized code that is not used any more for DMU.
                   Using secapi_get_random to start preencrypt dmu keys.
11/10/06    an     Fixed compilation errors for 6020
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
01/23/04    jd     Removed support for FEATURE_SEC_DEPRICATED
01/20/04    jd     dmui_pre_encrypt_all_sessions returns operation result.
                   Only set dmu_rand_ready flag if pre-encryption succeeds. 
01/19/04    jd     Free the crypto instance after encryption is complete, so
                   security services will free the DSP for other clients to 
                   use.  Made encryption synchronous as well for simplicity.
07/31/03    mvl    Re-added jd's change from 05/20/03
05/29/03    jay    Changes for new crypto API
05/20/03    jd     Now write CHAP PPP_PASSWORD_I for multimode builds too
02/14/03    jay    Handle encryption failure during DMU
12/29/02    jd     Fixed includes for multimode builds
                   Added params to dmu_sig_handler to be compatible with 
                   ps_set_sig_handler()
12/29/02    jd     Added dmu_rand_sig_handler, moving DMU random signal code
                   out of psmgr and into dmu source
12/23/02    jd     Added dmu_commit_key_data
12/10/02    jd     Cleaned up comments, 
                   removed unused param from dmui_get_pkoid_from_nv()
10/02/02    jay    Added a function to read PKOID from NV and other fixes
                   based on DMU inetrop in SD.
09/20/02    jay    Read MN_Auth from NV and copy to dmu_info structure.
05/22/02    jay    Added functions to generate random data and encrypt them.
05/08/02    jd     Created file

============================================================================*/


/*============================================================================

             Include Files for Module 

============================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU

#include <stdlib.h>
#include <stdio.h>
#include "AEEstd.h"
#include "msg.h"
#include "err.h"
#include "assert.h"
#include "fs.h"
#include "dsmip_dmu.h"
#include "nv.h"
#include "secutil.h"

#include "secapi.h"
#include "pstimer.h"
#ifndef FEATURE_DATA_MM
#include "ppplcp.h"
#include "dsatcop.h"
#include "psi.h"
#else
#include "dsat707mipctab.h"
#include "ps_utils.h"
#include "ps_mip_compat.h"
#endif /* FEATURE_DATA_MM */


/* ======================================================================== */
/*                       External Data                                      */
/* ======================================================================== */

/*----------------------------------------------------------------------------
  Temporary MIP keys.  

  These keys are pre-generated at system startup, then pre-encrypted with 
  the public key corresponding to the PKOID indicated in the active MIP user 
  profile. 
----------------------------------------------------------------------------*/
dmu_info_type  mip_dmu_info[MIP_MAX_SESSIONS];

/*----------------------------------------------------------------------------
  Flag to indicate if searcher has started and random data will be available
  for key generation and encryption processes.
----------------------------------------------------------------------------*/
boolean        mip_dmu_rand_ready = FALSE;


/* ======================================================================== */
/*                       Local Data & Definitions                           */
/* ======================================================================== */

/*----------------------------------------------------------------------------
  Macros to convert between dmu_info pointer and dmu_info array index
----------------------------------------------------------------------------*/
#define MIP_DMU_PTR_2_INDEX(dmu_ptr)  abs((dmu_ptr) - mip_dmu_info)
#define INDEX_2_DMU_INFO_PTR(index)   (mip_dmu_info + (index))

/*----------------------------------------------------------------------------
  DMU Public Key directory name (subdirectory in EFS root directory)
----------------------------------------------------------------------------*/
#define DMUEFSI_PK_DIR  "DMU"

/*----------------------------------------------------------------------------
  Maximum length of public key filename, i.e. sizeof("/DMU/255.key\0") 
----------------------------------------------------------------------------*/
#define DMUEFSI_PK_FNAME_LEN  (sizeof(DMUEFSI_PK_DIR)+sizeof("/###.key")+1)

/*----------------------------------------------------------------------------
  buffer for full path public key filename
----------------------------------------------------------------------------*/
LOCAL char  dmui_public_key_filename[DMUEFSI_PK_FNAME_LEN];



/* ======================================================================== */
/*                       Internal Functions                                 */
/* ======================================================================== */

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  */
/*                       NV & EFS function                                  */
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  */


/*============================================================================

FUNCTION DMUI_GET_PKOID

DESCRIPTION

  Get the provisioned pkoid from NV for the active MIP profile 

PARAMETERS

  None 

RETURN VALUE
  -1          - couldn't read key
  0-255       - PKOID read from NV
  256..65534  - should never happen

DEPENDENCIES
  None
============================================================================*/
LOCAL sint15 dmui_get_pkoid_from_nv
(
  void
)
{
  nv_stat_enum_type  nv_status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*--------------------------------------------------------------------------
    Read the pkoid for this session
  --------------------------------------------------------------------------*/
  ps_nv_item.ds_mip_dmu_pkoid.index = ds_atcop_qcmipp_val;

  nv_status = psi_get_nv_item (NV_DS_MIP_DMU_PKOID_I, &ps_nv_item);
  if (nv_status != NV_DONE_S)
  {
    MSG_ERROR ("Couldn't read pkoid for profile %d",
                           ps_nv_item.ds_mip_dmu_pkoid.index,0,0);
    return -1;
  }
  MSG_MED ("Read pkoid %d for profile %d", 
           ps_nv_item.ds_mip_dmu_pkoid.pkoid, 
           ps_nv_item.ds_mip_dmu_pkoid.index, 0);
  return (ps_nv_item.ds_mip_dmu_pkoid.pkoid);

} /* dmui_get_pkoid_from_nv() */



/*============================================================================

FUNCTION DMUI_EFSI_READ_KEY_RSA_1024

DESCRIPTION
  Read the RSA-1024 public key value from the specified EFS file (opened 
  previously) to the specified buffer.

  Note there is no bounds checking on public_key parameter.

  RSA-1024 public key format:

    +---------------+
    | public_key    +  
    | |- exponent   +  (128 bytes)
    | +- modulus    +  (128 bytes)
    +---------------+

PARAMETERS

  fs_handle_type  - EFS handle to read from (input) - must be open already 
  public_exponent - public key exponent value (output)
  modulus         - public key modulus value (output)

RETURN VALUE
  Length of the key, in bytes.
  -1 = couldn't read key

DEPENDENCIES
  None

============================================================================*/
LOCAL boolean dmui_efsi_read_eky_rsa_1024
(
  fs_handle_type *  fs_handle,
  byte *            public_exponent,
  byte *            modulus
)
{
  fs_rsp_msg_type  fs_rsp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Read the exponent into the given buffer.
    Note that the fs_read call is blocking.
  -------------------------------------------------------------------------*/
  fs_read( *fs_handle, 
           public_exponent, 
           MIP_DMU_RSA1024_KEY_LEN, 
           NULL, 
           &fs_rsp );
  if (fs_rsp.read.status != FS_OKAY_S)
  {
    MSG_ERROR( "fs_read failed (%d)", fs_rsp.read.status, 0, 0 );
    return (FALSE);
  }

  /*--------------------------------------------------------------------------
    Read the modulus into the given buffer
    Note that the fs_read call is blocking.
  -------------------------------------------------------------------------*/
  fs_read( *fs_handle, 
           modulus, 
           MIP_DMU_RSA1024_KEY_LEN, 
           NULL, 
           &fs_rsp );
  if (fs_rsp.read.status != FS_OKAY_S)
  {
    MSG_ERROR( "fs_read failed (%d)", fs_rsp.read.status, 0, 0 );
    return (FALSE);
  }

  return (TRUE);

} /* dmui_efsi_read_eky_rsa_1024() */



/*============================================================================

FUNCTION DMUI_GEN_PK_FILENAME

DESCRIPTION
  Generate the EFS path to the public key file for the specified PKOID.

PARAMETERS

  pkoid  - pkoid of key for which filename is generated

RETURN VALUE
  pointer to the static char array where filename is stored

DEPENDENCIES
  Function is non-reentrant, always call from the same task.
  Filename storage is global - never 


============================================================================*/
LOCAL char * dmui_gen_pk_filename
(
  byte  pkoid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /*--------------------------------------------------------------------------
    Build the filename, based on pkoid
  -------------------------------------------------------------------------*/
  (void) std_strlcpy(
      dmui_public_key_filename, 
      DMUEFSI_PK_DIR, 
      DMUEFSI_PK_FNAME_LEN);

  (void) std_strlprintf(
      &dmui_public_key_filename[sizeof(DMUEFSI_PK_DIR) - 1], 
      DMUEFSI_PK_FNAME_LEN - sizeof(DMUEFSI_PK_DIR) + 1, 
      "/%d.key", 
      pkoid);

  MSG_MED ("Public key filename is", 0,0,0);
  MSG_MED (dmui_public_key_filename,0,0,0);

  return( dmui_public_key_filename );

} /* dmui_gen_pk_filename() */



/*============================================================================

FUNCTION DMUI_EFS_READ_PK

DESCRIPTION

  Read the public key from the EFS file to the specified buffer.

  Public keys are stored in the directory:   /DMU
  Filenames are assumed to be of the format: <pkoid>.key
   where pkoid is the text string of PKOID as a decimal number

  Index file format:

    +---------------+
    | pk_info       +
    | |- pkoid      +  (1 byte)  
    | |- pkoi       +  (1 byte)
    | |- pk_Expansn +  (1 byte)  (set to dflt = 0xff, for dmuv 0 (v1.3))
    | |- atv        +  (4 bits)  (currently only support 1 (RSA-1024))
    | +- dmuv       +  (4 bits)  (this implementation = 0x00 (DMU v1.3))
    +---------------+
    | public_key    +  (variable) public key value, format dependent on atv
    +---------------+

  RSA-1024 public key file format:

    +---------------+
    | public_key    +  
    | |- exponent   +  (128 byte)
    | +- modulus    +  (128 byte)
    +---------------+

  Future:  read public keys at startup (dmu_efs_init) to verify they are ok,
           so provisioners will have some indication at startup if they did 
           their job right.

PARAMETERS

  pkoid           - public key organization identifier (input)
  pk_info         - public key identifier (output)
  public_exponent - public key exponent value (output)
  modulus         - public key modulus value (output)

RETURN VALUE
  Length of the key, in bytes.
  -1 = couldn't read key

DEPENDENCIES
  None

============================================================================*/
LOCAL boolean dmui_efs_read_pk
(
  byte           pkoid,
  dmu_info_type *dmu_info
)
{
  fs_handle_type     fs_handle;
  fs_rsp_msg_type    fs_rsp;
  boolean            retval = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  MSG_MED ("Retrieve DMU public key", 0,0,0);

  /*--------------------------------------------------------------------------
    Generate the public key filename
  --------------------------------------------------------------------------*/
  (void) dmui_gen_pk_filename( pkoid );

  /*--------------------------------------------------------------------------
    Open the public key file
  --------------------------------------------------------------------------*/
  fs_open(dmui_public_key_filename, FS_OA_READONLY, NULL, NULL, &fs_rsp);
  if(fs_rsp.open.status != FS_OKAY_S)
  {
    if(fs_rsp.open.status == FS_NONEXISTENT_FILE_S)
    {
      MSG_HIGH ("No key for PKOID %d", pkoid,0,0);
    }
    else
    {
      MSG_ERROR( "fs_open failed (%d)", fs_rsp.open.status, 0, 0 );
    }
    return FALSE;
  }
  fs_handle = fs_rsp.open.handle;

  /*--------------------------------------------------------------------------
    Read Public Key Identifier info
  --------------------------------------------------------------------------*/
  fs_read(fs_handle, &(dmu_info->pk_info), sizeof (pki_type), NULL, &fs_rsp);
  if (fs_rsp.read.status == FS_OKAY_S)
  {
    /*------------------------------------------------------------------------
      Force filename pkoid over pk info.pkoid in the file itself
      (to allow duplicating a key by copying the file and renaming)
      // JD - add this to DMU design doc & customer guide
    ------------------------------------------------------------------------*/
    if (dmu_info->pk_info.pkoid != pkoid)
    {
      MSG_MED ("Warning: pkoid in file != filename!",0,0,0);
      dmu_info->pk_info.pkoid = pkoid;
    }

    /*------------------------------------------------------------------------
      Read the key based on algorithm type
    ------------------------------------------------------------------------*/
    MSG_MED("DMU AV = %d (%d)", dmu_info->pk_info.av, MIP_DMU_AV_RSA_1024,0);
    switch (dmu_info->pk_info.av)
    {
      /*----------------------------------------------------------------------
        RSA-1024 type supported
      ----------------------------------------------------------------------*/
      case MIP_DMU_AV_RSA_1024:
        retval = dmui_efsi_read_eky_rsa_1024 (&fs_handle,  
                                              dmu_info->pub_exp,  
                                              dmu_info->modulus);
        break;
  
      /*----------------------------------------------------------------------
        Otherwise, not supported, return
      ----------------------------------------------------------------------*/
      default:
        MSG_ERROR( "ATV type (%d) not supported", dmu_info->pk_info.av,0,0);
        break;
    }
  }
  else
  {
    MSG_ERROR( "fs_read failed (%d)", fs_rsp.read.status, 0, 0 );
  } 
 
  /*--------------------------------------------------------------------------
    Close the public key file
  --------------------------------------------------------------------------*/
  fs_close( fs_handle, NULL, &fs_rsp );
  if (fs_rsp.close.status != FS_OKAY_S )
  {
    MSG_ERROR( "fs_close failed (%d)", fs_rsp.close.status, 0, 0 );
  }

  return (retval);

} /* dmui_efs_read_pk() */



/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  */
/*                        Encryption functions                              */
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =  */


/*============================================================================
FUNCTION DMUI_GET_CRYPTO_INSTANCE

DESCRIPTION
  Get a crypto instance and set it up to do encryption for DMU

PARAMETERS
  dmu_info:  pointer to dmu_info block where allocated crypto handle will be
             stored

RETURN VALUE
  FALSE:  Couldn't allocate or configure crypto instance for dmu
  TRUE:   Crypto instance successfully allocated and configured for dmu
  None
    
DEPENDENCIES
  crypto instance must be freed by caller

SIDE EFFECTS
  None
============================================================================*/
LOCAL boolean dmui_get_crypto_instance
( 
  dmu_info_type * dmu_info
)
{
  secapi_pkx_param_data_type  rsa_param;
  secerrno_enum_type          sec_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*--------------------------------------------------------------------------
    Get a crypto handle for this session. 
  --------------------------------------------------------------------------*/
  sec_errno = secapi_new(&(dmu_info->crypto_handle), SECAPI_RSA);

  if(sec_errno != E_SUCCESS)
  {
    MSG_ERROR("Error creating crypto handler (%d)",sec_errno,0,0);
    return FALSE;
  }
  
  /*--------------------------------------------------------------------------
    Do the encryption in synchronous mode to avoid lots of commanding.

    This is pretty fast on newer targets (< 50ms), but might be slow on <6100
  --------------------------------------------------------------------------*/
  rsa_param.common.exec_mode = SECAPI_SYNC_EXEC;
  sec_errno = secapi_pkx_set_param( dmu_info->crypto_handle,
                                    SECAPI_RSA,
                                    SECAPI_EXEC_MODE,
                                    &rsa_param);
  if(sec_errno != E_SUCCESS )
  {
    MSG_ERROR("Err couldn't set RSA to async mode (%d)",sec_errno,0,0);
    return FALSE;
  }

  return TRUE;

} /* dmui_get_crypto_instance() */



/*============================================================================
FUNCTION DMUI_FREE_CRYPTO_INSTANCE

DESCRIPTION
  Frees a crypto instance associated with specified DMU session info

PARAMETERS
  dmu_info:  pointer to dmu_info block containing crypto handle to free

RETURN VALUE
  None
    
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL void dmui_free_crypto_instance
(
  dmu_info_type *dmu_info
)
{
  secerrno_enum_type  sec_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*--------------------------------------------------------------------------
    Free allocated crypto handle.  crypto_handle is nulled in sec*_delete()
  --------------------------------------------------------------------------*/
  if (dmu_info->crypto_handle != NULL)
  {
    sec_errno = secapi_delete (&(dmu_info->crypto_handle));
    if (sec_errno != E_SUCCESS)
    {
      MSG_ERROR("Error deleting crypto handler (%d)",sec_errno,0,0);
    }
  }
} /* dmui_free_crypto_instance() */



/*============================================================================
FUNCTION DMU_CONFIG_PUB_KEY

DESCRIPTION
  Configure the crypto instance for the specified session to use the key 
  file named by the pkoid in the NV item corresponding to this session 

PARAMETERS
  pkoid    - pkoid to configure public key for or 
             load pkoid specified in active MIP profile if PKOID_WILD_CARD
  dmu_info - session for which to configure public key

RETURN VALUE
  Success or Failure 

DEPENDENCIES
  None

SIDE EFFECTS
  DMU info is filled with public key and pk info

============================================================================*/
LOCAL boolean dmui_config_pub_key
(
  dmu_info_type * dmu_info
)
{
  sint15          pkoid;
  secerrno_enum_type          sec_errno;
  secapi_pkx_param_data_type  rsa_param;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /*--------------------------------------------------------------------------
    If requested, read pkoid from profile info
  --------------------------------------------------------------------------*/
  pkoid = dmu_info->pk_info.pkoid;
  if (pkoid == PKOID_UNLOADED)
  {
    pkoid = dmui_get_pkoid_from_nv();
    if (pkoid == -1)
    {
      MSG_ERROR("Error while reading pkoid from NV",0,0,0);
      return FALSE;
    }
    dmu_info->pk_info.pkoid = (byte) pkoid;
  }

  /*--------------------------------------------------------------------------
    Read the modulus and exponent value from efs
  --------------------------------------------------------------------------*/
  if (!dmui_efs_read_pk( (byte) pkoid, dmu_info))
  {
    MSG_HIGH("Error while reading PK from EFS",0,0,0);
    return FALSE;
  }
  
  /*--------------------------------------------------------------------------
    Set the modulus value in the crypto instance configuration
  --------------------------------------------------------------------------*/
  rsa_param.rsa.key.modulus_ptr  = dmu_info->modulus;
  rsa_param.rsa.key.modulus_size = MIP_DMU_RSA1024_KEY_LEN;
  sec_errno = secapi_pkx_set_param( dmu_info->crypto_handle,
                                    SECAPI_RSA,
                                    SECAPI_PKX_RSA_MODULUS,
                                    &rsa_param );
  if(sec_errno != E_SUCCESS)
  {
    MSG_ERROR("Error setting modulus (%d)",sec_errno,0,0);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Set the exponent e value in the crypto instance configuration
  --------------------------------------------------------------------------*/
  rsa_param.rsa.key.public_key_ptr = dmu_info->pub_exp;
  rsa_param.rsa.key.key_size       = MIP_DMU_RSA1024_KEY_LEN;
  sec_errno = secapi_pkx_set_param( dmu_info->crypto_handle,
                                    SECAPI_RSA,
                                    SECAPI_PKX_RSA_PUBLIC_KEY,
                                    &rsa_param );
  if(sec_errno != E_SUCCESS)
  {
    MSG_ERROR("Error setting exp_e (%d)",sec_errno,0,0);
    return FALSE;
  }

  return TRUE;
} /* dmui_config_pub_key() */



/*============================================================================
FUNCTION DMUI_START_ENCRYPTION

DESCRIPTION
  Start encrypting the shared secrets (key data)

PARAMETERS
  dmu_info:  pointer to dmu_info block containing key_data to be encrypted

RETURN VALUE
  FALSE:  Error starting encryption
  TRUE:   encryption has started
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL boolean dmui_start_encryption
(
  dmu_info_type *dmu_info
)
{
  secerrno_enum_type  sec_errno;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /*--------------------------------------------------------------------------
    Set the key data length.  
 
    With the old crypto API, this is in bits.
    With the new crypto API, this is in bytes.  
  --------------------------------------------------------------------------*/
  dmu_info->key_data_len = sizeof(dmu_key_data_type);

  /*--------------------------------------------------------------------------
    Start the encryption process
  --------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------
    Initialize encrypted_key_data_len to the buffer length provided to 
    crypto API.  Will be set to actual length of encrypted data on return.
  --------------------------------------------------------------------------*/
  dmu_info->encrypted_key_data_len = MIP_DMU_ENCRYPTED_KEY_DATA_LEN;
  sec_errno = secapi_pkx_encrypt(  dmu_info->crypto_handle,
                                   SECAPI_RSA,
                                   SECAPI_PKX_RSA_PADDING_ENC_PKCS1_V15,
                                   (byte *) &(dmu_info->key_data),
                                   dmu_info->key_data_len,
                                   dmu_info->encrypted_key_data,
                                   &(dmu_info->encrypted_key_data_len) );
  if (sec_errno != E_SUCCESS)
  {
    /*------------------------------------------------------------------------
      Future:  If this fails, perhaps this can be handled at runtime.  
    ------------------------------------------------------------------------*/
    MSG_ERROR("Err during encryption request (%d)",sec_errno,0,0);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Some sanity checks
  --------------------------------------------------------------------------*/
  if( dmu_info->encrypted_key_data_len != MIP_DMU_ENCRYPTED_KEY_DATA_LEN )
  {
    ERR ("Malformed encryption data %d",dmu_info->encrypted_key_data_len,0,0); 
    return FALSE;
  }
  
  return TRUE; 

} /* dmui_start_encryption() */
 


/*============================================================================
FUNCTION DMUI_ENCRYPT_KEY_DATA

DESCRIPTION
  Encrypts the shared secrets using RSA - 1024

PARAMETERS
  dmu_info:  pointer to dmu_info block containing key_data to be encrypted
             and pkoid indicating which public key to load

RETURN VALUE
  FALSE:  Error before encryption was started
  TRUE:   Shared secret encryption has started
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
LOCAL boolean dmui_encrypt_key_data
(
  dmu_info_type *dmu_info
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  /*--------------------------------------------------------------------------
    Clear the encrypted flag
  --------------------------------------------------------------------------*/
  dmu_info->encrypted = FALSE;

  /*--------------------------------------------------------------------------
    Get a crypto instance and control of the QDSP
    - this MUST be freed by the caller immediately after return
  --------------------------------------------------------------------------*/
  if (!dmui_get_crypto_instance(dmu_info))
  {
    ERR("Couldn't allocate crypto handles - can't do dmu!",0,0,0);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Configure the crypto instance with the public key
  --------------------------------------------------------------------------*/
  if (!dmui_config_pub_key(dmu_info))
  {
    ERR("Couldn't configure public key - can't do dmu!",0,0,0);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Encrypt the key data
  --------------------------------------------------------------------------*/
  if (!dmui_start_encryption(dmu_info))
  {
    ERR("Couldn't start key encryption!",0,0,0);
    return FALSE;
  }

  /*--------------------------------------------------------------------------
    Set the flag to indicate that temporary dmu keys are encrypted now
  --------------------------------------------------------------------------*/
  dmu_info->encrypted = TRUE;

  return TRUE;

} /* dmui_encrypt_key_data() */



/* ======================================================================== */
/*                       External Functions                                 */
/* ======================================================================== */

/*============================================================================
FUNCTION DMU_INIT

DESCRIPTION
  Initialize DMU module

PARAMETERS
  None

RETURN VALUE
  None
    
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
void dmu_init
( 
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    Clear the DMU session info block
  --------------------------------------------------------------------------*/
  memset (mip_dmu_info, 0, sizeof (mip_dmu_info));

} /* dmu_init() */



/*============================================================================

FUNCTION DMU_GEN_KEY_DATA

DESCRIPTION
  Generates new temporary keys which will be used for during next DMU.

PARAMETERS
  dmu_info:  pointer to dmu_info block where temporary keys will be stored

RETURN VALUE
  True:  When secapi_get_random() returns success.
  False: Otherwise

DEPENDENCIES
  None

============================================================================*/
boolean dmu_gen_key_data
( 
  dmu_info_type *dmu_info  
)
{
  secerrno_enum_type sec_errno;
  nv_stat_enum_type  nv_status;
  byte               i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    Generate random keys using the RFC 1750 compliant source of random 
    data (provided by security services).
  --------------------------------------------------------------------------*/
  sec_errno = secapi_get_random( SECAPI_SECURE_RANDOM,
                                 (byte *)&(dmu_info->key_data),
                                 sizeof(dmu_key_data_type) );  
  /*--------------------------------------------------------------------------
    Random data generation failed, return FAILURE
  --------------------------------------------------------------------------*/
  if(sec_errno != E_SUCCESS)
  {
    ERR("Error generating random numbers. err number: %d",sec_errno,0,0);
    return FALSE;
  }
  
  /*--------------------------------------------------------------------------
    Retrieve MN Authenticator from NV, overwriting the random value 
    generated above.
  --------------------------------------------------------------------------*/
  ps_nv_item.ds_mip_dmu_mn_auth.index = ds_atcop_qcmipp_val;

  nv_status = psi_get_nv_item(NV_DS_MIP_DMU_MN_AUTH_I,&ps_nv_item);
  if( nv_status != NV_DONE_S )
  {
    ERR("DMU MN Auth unreadable", 0, 0, 0);
    return TRUE;
  }
   
  /*--------------------------------------------------------------------------
    Copy mn_auth into dmu_info, clear mn auth after msg is built.
    Since NV structure is PACKED copy byte by byte.
  --------------------------------------------------------------------------*/
  for( i=0; i<MIP_DMU_MN_AUTH_LEN; i++)
  {
    dmu_info->key_data.mn_auth[i] = ps_nv_item.ds_mip_dmu_mn_auth.mn_auth[i];
  }

  return TRUE;

} /* dmu_gen_key_data() */



/*============================================================================
FUNCTION DMU_PKOID_SUPPORTED

DESCRIPTION
  Check if a public key exists for the given PKOID. 

PARAMETERS
  pkoid - pkoid of the key to look for

RETURN VALUE
  FALSE:  No key for this pkoid
  TRUE:   key present for this pkoid

DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
boolean dmu_pkoid_supported
(
  byte pkoid
)
{
  fs_rsp_msg_type  fs_rsp;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  /*--------------------------------------------------------------------------
    Check if a readable file exists for this pkoid

    - note that we do not verify that the public key contained therein is 
      valid (i.e. length check, successfully configure crypto with key).
  --------------------------------------------------------------------------*/
  fs_nametest( dmui_gen_pk_filename( pkoid ),
               FS_TEST_FILE,
               NULL,          /* syncronous */
               &fs_rsp );

  if (fs_rsp.statfs.status != FS_OKAY_S)
  {
    MSG_HIGH ("Coudn't nametest public key file for pkoid = %d!", pkoid,0,0);
    return FALSE;
  };

  return( fs_rsp.nametest.name_found );

} /* dmu_pkoid_supported() */



/*============================================================================
FUNCTION DMU_ENCRYPT_KEY_DATA

DESCRIPTION
  Encrypts the shared secrets using RSA - 1024

PARAMETERS
  None

RETURN VALUE
  FALSE:  Error before encryption was started
  TRUE:   Shared secret encryption has started
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
============================================================================*/
boolean dmu_encrypt_key_data
(
  dmu_info_type *dmu_info
)
{
  boolean  result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  

  MSG_MED ("Key encryption begins",0,0,0);
  /*--------------------------------------------------------------------------
    Encrypt the key_data
  --------------------------------------------------------------------------*/
  result = dmui_encrypt_key_data(dmu_info);

  /*--------------------------------------------------------------------------
    Free the crypto instance regardless of the result
  --------------------------------------------------------------------------*/
  dmui_free_crypto_instance(dmu_info);

  MSG_MED ("Key encryption complete",0,0,0);
  return result;

} /* dmu_encrypt_key_data() */



/*============================================================================

FUNCTION DMU_PRE_ENCRYPT_ALL_SESSIONS

DESCRIPTION
  Generates new temporary keys and encrypt for all the sessions

PARAMETERS
  None

RETURN VALUE
  FALSE:  Pre-encryption failed
  TRUE:   Pre-encryption succeeded

DEPENDENCIES
  This is only called once shortly after startup

============================================================================*/
LOCAL boolean dmui_pre_encrypt_all_sessions
(
  void 
)
{
  dmu_info_type * dmu_info;
  byte            loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  dmu_info = INDEX_2_DMU_INFO_PTR(0);

  for (loop=0; loop < MIP_MAX_SESSIONS; loop++, dmu_info++)
  {
    MSG_HIGH("This is pre-encryption",0,0,0);
    /*------------------------------------------------------------------------
      Generate new key data for this session
    ------------------------------------------------------------------------*/
    if (!dmu_gen_key_data(dmu_info))
    {
      ERR("Couldn't generate key data!",0,0,0); 
      return FALSE;
    }

    /*------------------------------------------------------------------------
      Encrypt key data
    ------------------------------------------------------------------------*/
    if (!dmu_encrypt_key_data(dmu_info))
    {
      /*----------------------------------------------------------------------
        Future: encryption could not complete.  Handle this case.
      ----------------------------------------------------------------------*/
      ERR("Couldn't encrypt key data!",0,0,0); 
      return FALSE;
    }
  } /* for all sessions */

  return TRUE;
} /* dmui_pre_encrypt_all_sessions() */



/*============================================================================

FUNCTION DMU_COMMIT_KEY_DATA

DESCRIPTION
  Write shared secrets from provided key_data extension into NV

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

============================================================================*/
void dmu_commit_key_data
(
  dmu_key_data_type * key_data
)
{
  nv_ds_mip_ss_user_prof_type * ss_profile;
  byte * d;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*--------------------------------------------------------------------------
    Write MN-HA and MN-AAA shared secrets to active MIP profile

    Future:  decouple atcop dependency here
  --------------------------------------------------------------------------*/
  ss_profile = &ps_nv_item.ds_mip_ss_user_prof;
  ss_profile->index = ds_atcop_qcmipp_val;

  d = (byte *) ss_profile->mn_ha_shared_secret;
  memcpy (d, key_data->mn_ha_key, MIP_DMU_MN_HA_KEY_LEN);
  ss_profile->mn_ha_shared_secret_length = MIP_DMU_MN_HA_KEY_LEN;

  d = (byte *) ss_profile->mn_aaa_shared_secret;
  memcpy (d, key_data->mn_aaa_key, MIP_DMU_MN_AAA_KEY_LEN);
  ss_profile->mn_aaa_shared_secret_length = MIP_DMU_MN_AAA_KEY_LEN;

  psi_put_nv_item (NV_DS_MIP_SS_USER_PROF_I, &ps_nv_item);

  /*--------------------------------------------------------------------------
    Write Simple IP CHAP shared secret to PPP password NV item
  --------------------------------------------------------------------------*/
  d = (byte *) ps_nv_item.ppp_password.password;
  memcpy (d, key_data->chap_key, MIP_DMU_CHAP_KEY_LEN);
  ps_nv_item.ppp_password.password_len = MIP_DMU_CHAP_KEY_LEN;

  psi_put_nv_item (NV_PPP_PASSWORD_I, &ps_nv_item);

#ifndef FEATURE_DATA_MM
  /*--------------------------------------------------------------------------
    Write Simple IP CHAP shared secret to lcp_auth_info structure, since 
    this is not read again after startup
  --------------------------------------------------------------------------*/
  memcpy (lcp_auth_info[IFACE_UM_PPP_STACK].auth_info.passwd_info, 
          key_data->chap_key,
          MIP_DMU_CHAP_KEY_LEN);
  lcp_auth_info[IFACE_UM_PPP_STACK].auth_info.passwd_len =  
          MIP_DMU_CHAP_KEY_LEN;
#endif /* FEATURE_DATA_MM */
} /* dmu_commit_key_data() */



/*============================================================================

FUNCTION DMU_RAND_SIG_HANDLER

DESCRIPTION
  Handle random data ready signal from 
  i)  Security services at startup to indicate that random data is avaiable
      for random key generation
  ii) PS task if qcmipp is changed, so random keys can be re-generated (for 
      safety, in case keys were captured from an incomplete DMU update with 
      the previous profile during a key request spoof), public key for new
      profile is read and news keys encrypted.

PARAMETERS
  Ignored

RETURN VALUE
  TRUE - tells PS that signal handling is complete

DEPENDENCIES
  None

============================================================================*/
boolean dmu_rand_sig_handler
(
#ifdef FEATURE_DATA_MM
  ps_sig_enum_type sig,                     /* Signal to be processed       */
  void *user_data_ptr                       /* Parameter carrying user data */
#else
  void
#endif /* FEATURE_DATA_MM */
)
{
  dmu_info_type * dmu_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*--------------------------------------------------------------------------
    Check mip_dmu_rand_ready, 
    - first time called: initialize, generate and pre-encrypt all sessions.
    - after: profile has changed, regenerate and pre-encrypt affected session
  --------------------------------------------------------------------------*/
  if (!mip_dmu_rand_ready)
  {
    if (dmui_pre_encrypt_all_sessions())
    {
      mip_dmu_rand_ready = TRUE;
    }
    return TRUE;
  }

  /*--------------------------------------------------------------------------
    For affected profile, generate new temporary keys and pre-encrypt them
    to prepare for DMU.

    For now, hard code this to session 0.  If multiple sessions need to 
    be supported, need to pass session index/pointer into the function.
  --------------------------------------------------------------------------*/
  MSG_MED ("Pre-encrypt new DMU keys",0,0,0);
  dmu_info = &mip_dmu_info[0];

  /*------------------------------------------------------------------------
    Generate new random key data
  ------------------------------------------------------------------------*/
  (void) dmu_gen_key_data (dmu_info);

  /*------------------------------------------------------------------------
    Reconfigure with the public key for the newly selected MIP profile
  ------------------------------------------------------------------------*/
  dmu_info->pk_info.pkoid = PKOID_UNLOADED;

  /*------------------------------------------------------------------------
    Encrypt key data with the newly loaded public key
  ------------------------------------------------------------------------*/
  if (!dmu_encrypt_key_data(dmu_info))
  {
    ERR("Couldn't encrypt key data!",0,0,0); 
  }

  return TRUE;

} /* dmu_rand_sig_handler() */

#endif /* FEATURE_DS_MOBILE_IP_DMU */
