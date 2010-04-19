#ifndef _DSMIP_DMU_H
#define _DSMIP_DMU_H

/*===========================================================================
                                      
                          D S M I P _ D M U . H

DESCRIPTION

 Header file for the DMSS implementation of the 
 Mobile IP Dynamic Mobile IP Key Update module.

EXTERNALIZED FUNCTIONS

  dmu_init()                   - initialize the DMU module
  dmu_gen_key_data             - generate temporary random keys 
  dmu_pkoid_supported          - Check if a public key exists for given PKOID
  dmu_encrypt_key_data         - encrypt the temporary random keys 
  dmu_rand_sig_handler         - perform DMU random generation on signal
  dmu_commit_key_data          - commit provided shared secrets to NV

INITIALIZATION AND SEQUENCING REQUIREMENTS

  dmu_init must be called before any of the other external API's.

Copyright (c) 2002-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/dsmip_dmu.h_v   1.9   29 Dec 2002 16:33:22   jeffd  $
$Header: //source/qcom/qct/modem/data/1x/mip/main/lite/inc/dsmip_dmu.h#1 $ $DateTime: 2008/04/11 07:14:56 $ $Author: nsivakum $
  

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/04/07   mga/as  Added dummy vars to dmu info structure to force 32 bit 
                   alignment allow encryption on the ARM instead of DSP.
08/28/07   sc      Changed the signature of dmu_gen_key_data() so that it
                   returns Success or Fail (boolean)
05/05/06   kvd     added uint32 dummy to dmu_info_type to make sure crypto
                   data is word aligned.
01/23/04   jd      Removed support for FEATURE_SEC_DEPRICATED
05/04/04   jd      remove prototype for dmu_pre_encrypt_all_sessions() - 
                   local now.
                   change dmu_crypto_init'd flag to dmu_rand_ready
05/04/04   jd      Updated header to reflect restructuring of DMU API's for
                   freeing crypto instance and making encryption synchronous.
07/09/03   jd      Moved CVSE extension type definition to dsmip_msg.h
05/29/03   jay     Changes for new crypto API
04/03/03   jd      Changed key_data_bit_len in dmu_info_struct to be 16 bits.
                   corresponding to secapi changes.
12/29/02   jd      Fixed includes for multimode builds
                   Added params to dmu_sig_handler to be compatible with 
                   ps_set_sig_handler()
12/29/02   jd      Added dmu_rand_sig_handler, moving DMU random signal code
                   out of psmgr and into dmu source
12/23/02   jd      Added dmu_commit_key_data prototype
12/10/02   jd      Cleaned up comments, removed unused param from dmu_get_pkoid
10/01/02   jay     Re-ordered the MIP_Key_Data type.
08/16/02   jay     Added clear text support
03/07/02   jd/jay  Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#ifdef FEATURE_DS_MOBILE_IP_DMU

/*---------------------------------------------------------------------------
  Verify dependencies
---------------------------------------------------------------------------*/
#if defined(FEATURE_DS_MOBILE_IP_DMU) && !defined(FEATURE_SEC)
  #error FEATURE_DS_MOBILE_IP_DMU requires FEATURE_SEC!
#endif
/*-------------------------------------------------------------------------*/


#include "secapi.h"
#include "dsmip.h"
#ifdef FEATURE_DATA_MM
#include "ps_svc.h"
#endif  /* FEATURE_DATA_MM */



/*===========================================================================

                      PUBLIC CONSTANT DECLARATIONS

===========================================================================*/
#define MIP_DMU_CVSETYPE_KEY_REQ          (1)  /* DMU key request subtype  */
#define MIP_DMU_CVSETYPE_DMU_KEY_DATA     (2)  /* DMU key data subtype     */
#define MIP_DMU_CVSETYPE_AAA_AUTH         (3)  /* DMU AAA auth. subtype    */
#define MIP_DMU_CVSETYPE_PUB_KEY_INVALID  (4)  /* Invalid Public key s.t.  */

#define MIP_CVSE_VERIZON_ORG_ID        (12951) 

/*---------------------------------------------------------------------------
   Public Key Identifier Constants
---------------------------------------------------------------------------*/
#define MIP_DMU_PK_VERSION             (0xFF)  /* default (no expansion)   */
#define MIP_DMU_AV                     (1)     /* RSA-1024                 */
#define MIP_DMU_DMUV                   (0)     /* DMU v1.3                 */
#define MIP_DMU_DMUV_CLEARTEXT         (7)     /* Support Clear text mode  */

#define MIP_DMU_MN_AAA_KEY_LEN  (16)
#define MIP_DMU_MN_HA_KEY_LEN   (16)
#define MIP_DMU_CHAP_KEY_LEN    (16)
#define MIP_DMU_AAA_AUTH_LEN    (8)
#define MIP_DMU_MN_AUTH_LEN     (3)

/*---------------------------------------------------------------------------
   RSA-1024 constants
---------------------------------------------------------------------------*/
#define MIP_DMU_RSA1024_KEY_LEN (128)   /* 1024 bits */

/*---------------------------------------------------------------------------
   Length of encrypted Mip_Key_Data payload - 
    currently 128 bytes (for RSA-1024)
---------------------------------------------------------------------------*/
#define MIP_DMU_ENCRYPTED_KEY_DATA_LEN  (128)

/*---------------------------------------------------------------------------
  PKOID unknown value to indicate that dmui_config_pub_key should load the
  key for the PKOID indicated in the active MIP profile.
---------------------------------------------------------------------------*/
#define PKOID_UNLOADED                  (0)


/*===========================================================================

                      PUBLIC TYPE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
   Vendor Specific extension RFC 3115
---------------------------------------------------------------------------*/

typedef PACKED struct
{
  byte   type;          /* 38 for CVSE, 134 for NVSE                       */
  byte   reserved;      /* must set to 0 on tx and ignore on receipt       */
  uint16 length;        /* length of extension excl. type and length       */
  uint32 vendor_id;     /* 0 byte, followed by SMI Network Mgmt            */
                        /* Private Enterprise Code of the vendor           */
  uint16 vse_type;      /* Vendor specific type field                      */
  /*     vse_value; */  /* followed by CVSE specific data                  */
} mip_vendor_specific_ext_type;


/*---------------------------------------------------------------------------
   DMU Key_Request CVSE  

   RRP (code=64 "Reason Unspecified") + this extension 
   indicates that the home AAA needs new keys and
   triggers to the mobile to start DMU.
 
   Expected format:
 
   cvse.type      = 38;                    (CVSE)
   cvse.reserved  = 0;
   cvse.length    = 8;
   cvse.vendor_id = 12951 = 0x00003297;    (from DMU specification)
   cvse.vse_type  = 1;                     (DMU_Key_Request)
   pkoid          = public key organization identifier of requesting AAA
---------------------------------------------------------------------------*/
typedef struct
{
  mip_vendor_specific_ext_type  cvse;     /* cvse header */
  byte                          pkoid;    /* PKOID of the requesting AAA */
} dmu_key_request_ext_type;


/*---------------------------------------------------------------------------
   DMU AAA_Authenticator CVSE  

   RRP (code=64 "Reason Unspecified") + this extension 
   indicates that the home AAA has committed the keys sent by the mobile 
   (in the preceding RRQ + DMU_Key_Data ext.) and
   triggers to commit the new keys to the subscriber profile.
 
   Expected format:
 
   cvse.type      = 38;                    (CVSE)
   cvse.reserved  = 0;
   cvse.length    = 15;
   cvse.vendor_id = 12951 = 0x00003297;    (from DMU specification)
   cvse.vse_type  = 3;                     (AAA_Authenticator)
   aaa_auth_val   = if valid, the aaa_auth_val sent in DMU_Key_Data
---------------------------------------------------------------------------*/
typedef struct
{
  mip_vendor_specific_ext_type  cvse;     /* cvse header */
  byte   aaa_auth_val[ MIP_DMU_AAA_AUTH_LEN ];/*valid if = val sent in RRQ */
} dmu_aaa_auth_ext_type;


/*---------------------------------------------------------------------------
   DMU Invalid_public_key CVSE  

   RRP (code=64 "Reason Unspecified") + this extension 
   indicates that the new keys sent by the mobile in MIP_Key_Data were
   encrypted with an unknown public key, so the AAA has NOT committed the
   keys.
 
   Expected format:
 
   cvse.type      = 38;                    (CVSE)
   cvse.reserved  = 0;
   cvse.length    = 6;
   cvse.vendor_id = 12951 = 0x00003297;    (from DMU specification)
   cvse.vse_type  = 4;                     (Invalid_public_key)
---------------------------------------------------------------------------*/
typedef struct
{
  mip_vendor_specific_ext_type  cvse;     /* cvse header */
} dmu_inval_pub_key_ext_type;


/*---------------------------------------------------------------------------
   DMU DMU_Key_Data CVSE 

   this extension is attached, and MN-HA + MN-AAA authenticators are 
   calcuated using keys contained in this extension, for the RRQ sent 
   to update the AAA keys.

   Generated format:
 
   cvse.type      = 38;                    (CVSE)
   cvse.reserved  = 0;
   cvse.length    = 139;
   cvse.vendor_id = 12951 = 0x00003297;    (from DMU specification)
   cvse.vse_type  = 2;                     (DMU_Key_Data)

   encrypted_keys = RSA-1024 (&key_data), where
 
     key_data.mn_aaa_key = MN-AAA shared secret
     key_data.mn_ha_key  = MN-HA shared secret
     key_data.chap_key   = Simple IP CHAP/PAP secret
     key_data.aaa_auth   = AAA_Authenticator token (random, encrypted)
     key_data.mn_auth    = MN_Authenticator token (opt'l, rnd, encr'd)

   pki.pkoid      = vendor organization ID 
   pki.pkoi       = vendor public key index
   pki.pk_version = vendor public key version
   pki.av         = 1 (RSA-1024)
   pki.dmuv       = 0 (v1.3)

---------------------------------------------------------------------------*/

/* Algorithm Type & Version definitions */
#define MIP_DMU_AV_RESERVED1 (0)  /* reserved                        */
#define MIP_DMU_AV_RSA_1024  (1)  /* RSA 1024 bit exponent & modulus */
#define MIP_DMU_AV_RSA_768   (2)  /* not supported                   */
#define MIP_DMU_AV_RSA_2048  (3)  /* not supported                   */
#define MIP_DMU_AV_RESERVED2 (4)  /* reserved                        */
#define MIP_DMU_AV_RESERVED3 (5)  /* reserved                        */
#define MIP_DMU_AV_RESERVED4 (6)  /* reserved                        */
#define MIP_DMU_AV_RESERVED5 (7)  /* reserved                        */

/*---------------------------------------------------------------------------
   Public Key Identifier type

  NOTE:  PKOID valid 0 is reserved in the spec.  We use this value in 
         dmu_info block to indicate that the pkoid should be read from the
         MIP user profile in NV.  If the spec ever changes to use pkoid=0, 
         this code will need to change.
---------------------------------------------------------------------------*/
typedef PACKED struct
{
  byte pkoid;           /* public key organization identifier              */
  byte pkoi;            /* organization specific public key index          */
  byte pk_version;      /* public key version                              */
                        /* NOTE: packed bitfields go LSB -> MSB! strange   */
  byte dmuv:4;          /* DMU version (4 bits)                            */
  byte av:4;            /* Algorithm version (4 bits)                      */
} pki_type;

typedef struct
{
  mip_vendor_specific_ext_type  cvse;                       /* cvse header */
  byte encrypted_keys[MIP_DMU_ENCRYPTED_KEY_DATA_LEN];  /* encr'd key_data */
  pki_type                 pki;                  /* Public Key Identifier  */
} dmu_key_data_ext_type;


/*---------------------------------------------------------------------------
   DMU Key Data 

   A token is a random value associated with a peer-to-peer transaction.  

   The peer that initiates a transaction associates a token with the 
   request, and the other peer returns the token with the response for that
   transaction.  

   In our case, the token provides a means of authentication, since its 
   value is encrypted in the request.
---------------------------------------------------------------------------*/
typedef struct
{
  byte mn_aaa_key[MIP_DMU_MN_AAA_KEY_LEN];  /* MN-AAA shared secret        */
  byte mn_ha_key[MIP_DMU_MN_HA_KEY_LEN];    /* MN-HA shared secret         */
  byte chap_key[MIP_DMU_CHAP_KEY_LEN];      /* Simple IP CHAP/PAP secret   */
  byte mn_auth[MIP_DMU_MN_AUTH_LEN];        /* MN_Authenticator token (opt'l) */
  byte aaa_auth[MIP_DMU_AAA_AUTH_LEN];      /* AAA_Authenticator token     */
} dmu_key_data_type;


/*===========================================================================

                      EXPORTED DATA DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  DMU Info type
  
  key_data -> random keys.  If key_data[0] == 0, key_data has not been 
              generated yet (invalid)

  pkoid -> The PKOID of the public key used to generate encrypted_key_data

  encrypted_key_data -> encrypted form of 'key_data', using public key
                        corresponding to PKOID 'pkoid'

  encrypted -> TRUE when encryption is complete, 
               i.e. encrypted_key_data valid

  crypto_handle -> crypto handle used to encrypt this key
---------------------------------------------------------------------------*/

typedef struct
{ 
  /*-------------------------------------------------------------------------
    crypto handle to encrypt key_data with
  -------------------------------------------------------------------------*/
  secapi_handle_type crypto_handle;

  /*-------------------------------------------------------------------------
    unencrypted version of key_data  
  -------------------------------------------------------------------------*/
  dmu_key_data_type  key_data;                    /* random key data    */
  uint16             key_data_len;                /* length of key_data */

  /*-------------------------------------------------------------------------
    public key to encrypt key_data with 
  -------------------------------------------------------------------------*/
  pki_type           pk_info;                          /* public key info  */
 
  /* easy fix is to insert 32 bit dummy var */
  uint32             dummy1;

  byte               pub_exp[MIP_DMU_RSA1024_KEY_LEN]; /* public exponent  */
  byte               modulus[MIP_DMU_RSA1024_KEY_LEN]; /* key-pair modulus */

  /*-------------------------------------------------------------------------
    encrypted version of key_data  

    'encrypted' is true when encryption is complete 
    only true if encrypted_key_data corresponds to key_data 
  -------------------------------------------------------------------------*/
  boolean            encrypted;      
  uint32             dummy;   /* dummy to take care of word-alignment of
                                 crypto data for crypto on ARM          */
  byte               encrypted_key_data[MIP_DMU_ENCRYPTED_KEY_DATA_LEN]; 
  uint16             encrypted_key_data_len;
} dmu_info_type;



/* ======================================================================== */
/*                       External Data                                      */
/* ======================================================================== */

/*----------------------------------------------------------------------------
  Temporary MIP keys.  

  These keys are pre-generated at system startup, then pre-encrypted with 
  the public key corresponding to the PKOID indicated in the active MIP user 
  profile. 
----------------------------------------------------------------------------*/
extern dmu_info_type  mip_dmu_info[MIP_MAX_SESSIONS];

/*----------------------------------------------------------------------------
  Flag to indicate if DMU key data has been pre-encrypted for all sessions
----------------------------------------------------------------------------*/
extern boolean        mip_dmu_rand_ready;



/*===========================================================================

                      EXPORTED FUNCTION DEFINITIONS

===========================================================================*/

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
extern void dmu_init
( 
  void
);



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
extern boolean dmu_gen_key_data
( 
  dmu_info_type *dmu_info  
);



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
extern boolean dmu_pkoid_supported
(
  byte pkoid
);



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
extern boolean dmu_encrypt_key_data
(
  dmu_info_type *dmu_info
);



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
extern void dmu_commit_key_data
(
  dmu_key_data_type * key_data
);



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
extern boolean dmu_rand_sig_handler
(
#ifdef FEATURE_DATA_MM
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
#else
  void
#endif /* FEATURE_DATA_MM */
);

#endif /* FEATURE_DS_MOBILE_IP_DMU */
#endif /* _DSMIP_DMU_H */

