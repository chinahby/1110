#ifndef SECBOOT_TYPES_H
#define SECBOOT_TYPES_H

/*===========================================================================

                      QUALCOMM Secondary  B o o t  L o a d e r
                    A P I ' s    F o r    S e c u r i t y   L i b

DESCRIPTION
    This header file contains declarations and definitions for the QCSBL
    interface to the software that authenticates the OEMSBL and AMSS.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/msmshared/secboot2/shared/1_0/sec/secboot_types.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/08   MJS     Added struct names to allow for forward declarations.
05/20/07   anb     API's for Security Library..
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

typedef unsigned long BLONG;

/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/

#define SECBOOT_COMMON_NAME_SIZE     128   /* 128 character name */
#define SECBOOT_SIGNATURE_SIZE       20    /* SHA-1 digest size  */

#define SECBOOT_RSA_EXPONENT         3
#define SECBOOT_VERIFY_ROOT_CERT     0     /* should be verify the root certificate ? */

/* PBL ROOT KEY size in bits, used by RSA */
#define PBL_MAX_KEY_SIZE 4096
#define PBL_MIN_KEY_SIZE 2048


#define SECX509_MAX_VRFY_SIG_LEN  (PBL_MAX_KEY_SIZE/8)

#define BLONGS_PER_MAX_KEY  (PBL_MAX_KEY_SIZE/ 32)

#define PBL_MAX_PUB_EXP_KEY_SIZE 32 /* in bits, but we expect only exp=3 (2 bits) or worst 65537 (17 bits)
                                    ** the generic case would be = PBL_MAX_KEY_SIZE but for memory reasons
                                    ** we stick to a small size to save memory whereever possible */

typedef uint32 buint8;
typedef uint32 buint16;

typedef struct BigInt
{
   BLONG a[(BLONGS_PER_MAX_KEY*2)+1];   /* Storage area { KeySize (in BLONGs) * 2 } +1 */
   int n;                               /* Current length of a */
   int nMax;                            /* Maximum allocated length of a */
} BigInt;


 /* ASN.1 data holder */
typedef struct secasn1_data_type
{
  const uint8 *data;
  buint16 len;
} secasn1_data_type;

/* ASN.1 bit string data holder */
typedef struct secasn1_bit_string_type
{
  const uint8 *data;
  buint16 len;
  buint8 unused;
} secasn1_bit_string_type;

/* Extra structures put here since other files are removed */
typedef struct clk_julian_type
{
  buint16 year;
  buint8 month;
  buint8 day;
  buint8 hour;
  buint8 minute;
  buint8 second;
} clk_julian_type;

typedef struct secboot_dbg_info_type
{
  /* Enable JTag before running DMSS */
  uint32   enable_jtag;
  uint32   oem_id;
  uint64   sw_id;
} secboot_dbg_info_type;

/* Public Key algorithms in the certificate */
typedef enum
{
  SECX509_PUBKEY_RSA = 0,
  SECX509_PUBKEY_DSA = 1,
  SECX509_PUBKEY_DH  = 2,
  SECX509_PUBKEY_MAX,               /* Last one, for error checking */
  SECX509_RESERVED_1 = 0x7FFFFFFF
} pbl_secx509_pubkey_algo_type;

/* Certificate signature algorithm type */
typedef enum
{
  SECX509_md5WithRSAEncryption  = 0,
  SECX509_md2WithRSAEncryption  = 1,
  SECX509_sha1WithRSAEncryption = 2,
  SECX509_SIG_ALGO_MAX,                 /* Last one, for error checking */
  SECX509_RESERVED_2            = 0x7FFFFFFF
} pbl_secx509_sig_algo_type;

/* RSA public key parameters */
typedef struct pbl_secx509_rsa_pubkey_type
{
  uint32  mod_len;
  uint8   mod_data[PBL_MAX_KEY_SIZE/8]; /* from bits -> bytes */
  uint32  exp_e_len;
  uint8   exp_e_data[PBL_MAX_PUB_EXP_KEY_SIZE/8]; /* from bits -> bytes */

} pbl_secx509_rsa_pubkey_type;

/* DSA public key parameters */
typedef struct pbl_secx509_dsa_pubkey_type
{
  uint32  dummy;

} pbl_secx509_dsa_pubkey_type;

/* DH public key parameters */
typedef struct pbl_secx509_dh_pubkey_type
{
  uint32 mod_len;
  uint8  mod_data[PBL_MAX_KEY_SIZE];
  uint32 exp_len;
  uint8  exp_data[PBL_MAX_KEY_SIZE];

} pbl_secx509_dh_pubkey_type;


/* Union of all the public key types */
typedef struct pbl_secx509_pubkey_type
{
  pbl_secx509_pubkey_algo_type  algo;
  union
  {
    pbl_secx509_rsa_pubkey_type  rsa;

  }key;

} pbl_secx509_pubkey_type;

/* Signature Structure */
typedef struct pbl_secx509_signature_type
{
  pbl_secx509_sig_algo_type   algo_id;
  secasn1_data_type           val;

} pbl_secx509_signature_type;

/* Distinguished name structure */
typedef struct pbl_secx509_dn_type
{
  uint32             num_attrib;
  secasn1_data_type  data;

} pbl_secx509_dn_type;

/* Version structure */
typedef struct pbl_secx509_version_type
{
  uint32             ver;
  secasn1_data_type  val;

} pbl_secx509_version_type;

/* Time structure */
typedef struct pbl_secx509_time_type
{
  uint32             time;
  secasn1_data_type  data;

} pbl_secx509_time_type;

/* Authority Key Identifier structure */
typedef struct pbl_secx509_auth_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;
  pbl_secx509_dn_type    name;
  secasn1_data_type  serial_number;

} pbl_secx509_auth_key_id_type;

/* Subject Key Identifier structure */
typedef struct pbl_secx509_subject_key_id_type
{
  boolean            set;
  secasn1_data_type  key_id;

} pbl_secx509_subject_key_id_type;

/* Key Usage structure */
typedef struct pbl_secx509_key_usage_type
{
  uint32   val;
  boolean  set;

} pbl_secx509_key_usage_type;

/* CA structure */
typedef struct pbl_secx509_ca_type
{
  boolean  set;
  boolean  val;

} pbl_secx509_ca_type;

/* Extension structure type */
typedef struct pbl_secx509_ext_type
{
  boolean                          set;
  pbl_secx509_auth_key_id_type     auth_key_id;
  pbl_secx509_subject_key_id_type  subject_key_id;
  pbl_secx509_key_usage_type       key_usage;
  pbl_secx509_key_usage_type       ex_key_usage;
  int32                            path_len;
  pbl_secx509_ca_type              ca;

} pbl_secx509_ext_type;

/* Certificate information structure */
typedef struct pbl_secx509_cert_info_type
{
  pbl_secx509_version_type     version;
  secasn1_data_type            serial_number;
  pbl_secx509_signature_type   algorithm;
  pbl_secx509_dn_type          issuer;
  pbl_secx509_time_type        not_before;
  pbl_secx509_time_type        not_after;
  pbl_secx509_dn_type          subject;
  pbl_secx509_pubkey_type      pub_key;
  secasn1_bit_string_type      issuer_unique_id;
  secasn1_bit_string_type      subject_unique_id;
  pbl_secx509_ext_type         extension;

} pbl_secx509_cert_info_type;

/* Certificate structure */
typedef struct pbl_secx509_cert_type
{
  /* The cert_info needs to be the first member */
  pbl_secx509_cert_info_type  cert_info;

  /* Signature info on the cert */
  pbl_secx509_pubkey_type     pkey;
  pbl_secx509_sig_algo_type   sig_algo;
  uint32                      cinf_offset;
  uint32                      cinf_byte_len;
  uint8                       sig[SECX509_MAX_VRFY_SIG_LEN];
  uint32                      sig_len;

  boolean                     __used;
  uint32                      asn1_size_in_bytes;
  /* For bookkeeping */

} pbl_secx509_cert_type;

#endif /* SECBOOT_TYPES_H */
