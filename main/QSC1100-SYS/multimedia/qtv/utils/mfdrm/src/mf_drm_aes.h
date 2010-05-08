/* crypto/aes/aes.h -*- mode:C; c-file-style: "eay" -*- */
/* ====================================================================
 * Copyright (c) 1998-2002 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 */

/* =======================================================================
                           Perforce Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/mfdrm/main/latest/src/mf_drm_aes.h#3 $
$DateTime: 2008/05/08 10:55:34 $
$Change: 656201 $

========================================================================== */

#ifndef HEADER_MF_AES_H
#define HEADER_MF_AES_H


/*----------------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------------*/
//#include "mf_drm_cryptotypes.h"
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "mf_drm_aes_locl.h"

/*----------------------------------------------------------------------------
 Defines
----------------------------------------------------------------------------*/

#ifdef OPENSSL_NO_AES
#error AES is disabled.
#endif

#define AES_ENCRYPT     1
#define AES_DECRYPT     0

/* Because array size can't be a const in C, the following two are macros.
   Both sizes are in bytes. */
#define AES_MAXNR 14
#define AES_BLOCK_SIZE 16

#ifdef  __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
  Type Declarations
  ----------------------------------------------------------------------------*/


    typedef enum
    {
        AES_FAIL,
        AES_SUCCESS
    }  MF_AES_Status;

/* This should be a hidden type, but EVP requires that the size be known */
    struct aes_key_st {
        unsigned long rd_key[4 *(AES_MAXNR + 1)];
        int rounds;
    };
    typedef struct aes_key_st AES_KEY;


/*----------------------------------------------------------------------------
  Function Prototypes
  ----------------------------------------------------------------------------*/


/* Internal Functions */
    
    const char *AES_options(void);

    int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                            AES_KEY *key);
    int AES_set_decrypt_key(const unsigned char *userKey, const int bits,
                            AES_KEY *key);
    
    void AES_encrypt(const unsigned char *in, unsigned char *out,
                     const AES_KEY *key);
    void AES_decrypt(const unsigned char *in, unsigned char *out,
                     const AES_KEY *key);
    
    void AES_ctr128_encrypt(const unsigned char *in, unsigned char *out,
                            const unsigned long length, const AES_KEY *key,
                            unsigned char counter[AES_BLOCK_SIZE],
                            unsigned char ecount_buf[AES_BLOCK_SIZE],
                            unsigned int *num);
    


#ifdef  __cplusplus
}
#endif

#endif /* !HEADER_MF_AES_H */


