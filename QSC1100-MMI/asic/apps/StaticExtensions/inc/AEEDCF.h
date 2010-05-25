
#ifndef AEEDCF_H
#define AEEDCF_H
/*===========================================================================

                    D I G I T A L  C O N T E N T  F O R M A T
                            
                            I N T E R F A C E

                          H E A D E R    F I L E

DESCRIPTION
  This is the header file defining the interface to the Qualcomm
  DCF  module.
  
  A typical usage model would be as follows.

  Phase one:
  ----------
  
  Phase two:
  ----------
  
  
EXTERNALIZED FUNCTIONS

    IDCF_CreateContentHeader()  - Create a OMA 1.0 DCF header
    IDCF_ParseContentHeader()   - Parse a OMA 1.0 DCF header
    IDCF_DecryptContent()       - Decrypt content
    


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Initialization is handled internally and not required by the user.

 Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEDCF.h#3 $
$DateTime: 2004/07/14 18:45:38 $ $Author: jayanthm $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/13/04   jay     Added CONST qualifier
06/16/04   jay     Updated DecryptContent
06/01/04   jay     Created module.
=============================================================================*/



/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "AEE.h"
#include "AEEError.h"
#include "AEEDRM.h"
#include "IDCF.bid"  /* AEECLSID_IDCF */


/*===========================================================================

                           PUBLIC DATA TYPES

===========================================================================*/

/* Sizes of rights object fields passed directly */
#define AEEDCF_MIMETYPE_MAXLEN        256
#define AEEDCF_DCFHDR_BUFFER_SIZE     1024

/*---------------------------------------------------------------------------
  DCF Header Type
---------------------------------------------------------------------------*/
typedef struct
{
  AEEDRM_VersionEnumType     Version;
  char                       szContentURI[ AEEDRM_MAX_URI_LEN ];
  char                       szContentMIMEType[ AEEDCF_MIMETYPE_MAXLEN ];
  AEEDRM_EncryptionInfoType  EncryptionInfo;
  uint8*                     pContentData;   /* Pointer to the encrypted data */
  uint32                     ContentDataLen; /* Encrypted data with padding */
} AEEDCF_HeaderType;

/*---------------------------------------------------------------------------
  DCF Error codes
---------------------------------------------------------------------------*/
//#define DCF_ERROR_BASE                (0xE00)

/*===========================================================================

                       IDCF Interface

===========================================================================*/

typedef struct IDCF IDCF;

AEEINTERFACE(IDCF) 
{
  INHERIT_IQueryInterface(IDCF);
  int (*CreateContentHeader) (IDCF* pDCF, const char szContentURI[],
                    const char szContentMIMEType[], const uint32 ContentDataLen,
                    AEEDRM_EncryptionInfoType* const pEncryptionInfo,
                    uint8* pDCFHeaderBuf, uint32* pDCFHeaderBufLen );
  int (*ParseContentHeader) (IDCF* pDCF, uint8*  pContentBuf,
                       const uint32 BufLen, AEEDCF_HeaderType* const pDCFHeader ); 
                       
  int (*DecryptContent) (IDCF* pDCF, uint8* pContent, const uint32 ContentLen,
                         uint8* pPlainText, uint32* pPlainTextLen,
                         const boolean IsContentComplete,
                         const AEEDRM_LicenseKeyType* pLicenseKey,
                         const AEEDRM_EncryptionInfoType* const pEncryptionInfo );
                          
};

#define IDCF_AddRef(p)                   AEEGETPVTBL((p),IDCF)->AddRef((p))
#define IDCF_Release(p)                  AEEGETPVTBL((p),IDCF)->Release((p))
#define IDCF_QueryInterface(p, clsid, pp)                                    \
                          AEEGETPVTBL(p,IDCF)->QueryInterface(p, clsid, pp)
  

#define IDCF_CreateContentHeader(p,uri,mime,cont_len,enc_info,header,       \
                                 header_len)                                \
    AEEGETPVTBL((p),IDCF)->CreateContentHeader((p),(uri),(mime),            \
                         (cont_len),(enc_info),(header),(header_len))
    
#define IDCF_ParseContentHeader(p,buf,len,header)                           \
    AEEGETPVTBL((p),IDCF)->ParseContentHeader((p),(buf),(len),(header))

#define IDCF_DecryptContent(p,content,cont_len,ptxt,ptxt_len,end,key,einfo) \
   AEEGETPVTBL((p),IDCF)->DecryptContent((p),(content),(cont_len),(ptxt),   \
                                           (ptxt_len),(end),(key),(einfo))


 
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION IDCF_CreateContentHeader()

DESCRIPTION
  This interface is used to create a DCF header.

DEPENDENCIES
  None
  
PROTOTYPE
  int IDCF_CreateContentHeader
  (
    IDCF*                       pDCF,
    char                        szContentURI[],
    char                        szContentMIMEType[],
    uint32                      ContentDataLen,
    AEEDRM_EncryptionInfoType*  pEncryptionInfo,
    uint8*                      pDCFHeaderBuf,
    uint32*                     pDCFHeaderBufLen
  )

PARAMETERS
  pDCF                 [In]- Pointer to DCF object
  szContentURI         [In]- Content URI of this rights object 
  szContentMIMEType    [In]- MIME type of the Content
  ContentDataLen       [In]- Length of the unencrypted content.
  pEncryptionInfo      [Out]- Pointer to the EncryptionInfo (IV,Padding)
  pDCFHeaderBuf        [Out]- Pointer to a buffer that holds the DCF header
  pDCFHeaderBufLen     [In-Out]- Pointer to size of the buffer above
  

RETURN VALUE
  SUCCESS      Successfully created the DCF header.
  EBADPARM     Invalid parameters
  ENOMEMORY
  EFAILED

COMMENTS 
  IDCF_CreateContentHeader() should be called before IDRM_ProtectContent.
  ContentDataLen should be the length of the un-encrypted content.
 
  
SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDCF_ParseContentHeader()

DESCRIPTION
  This interface is used to parse DCF header from .DCF file.
  
DEPENDENCIES
  None
  
PROTOTYPE
  int IDCF_ParseContentHeader
  ( 
    IDCF*               pDCF,
    uint8*              pContentBuf,
    uint32              BufLen,
    AEEDCF_HeaderType*  pDCFHeader
 )

PARAMETERS
  pDRM                 [In]- Pointer to DRM object
  pContentBuf          [In]- Pointer to the buffer that holds contents of file
                             xxx.dcf
  BufLen               [In]- Length of the above buffer
  pDCFHeader           [Out]- Struct to hold the parsed header fields
    
RETURN VALUE
  SUCCESS      Successfully parsed a .DCF file.
  EBADPARM     Invalid parameters 
  ENOMEMORY
  EFAILED

COMMENTS 
  
SIDE EFFECTS
  None
===========================================================================*/


/*===========================================================================

FUNCTION IDCF_DECRYPTCONTENT()

DESCRIPTION
  This function decrypts a protected content.
   
DEPENDENCIES
  

PROTOTYPE
int IDCF_DecryptContent
( 
  IDCF*                       pDCF,
  uint8*                      pContent,
  uint32                      ContentLen,
  uint8*                      pPlainText,
  uint32*                     pPlainTextLen,
  boolean                     IsContentComplete,
  AEEDRM_LicenseKeyType*      pLicenseKey,
  AEEDRM_EncryptionInfoType*  pEncryptionInfo
)  

PARAMETERS
  pDRM                [In]- Pointer to DRM object
  pContent            [In]- Pointer to the encrypted content
  ContentLen          [In]- Size of the encrypted content
  pPlainText          [Out]- Pointer to the buffer that holds plain text
  pPlainTextLen       [In-Out]- Pointer to the above buffer which gets updated
  IsContentComplete   [In]- Indicates whether the content is complete or more
                           data is to follow
  pLicenseKey         [In]- Decryption Key                         
  pEncryptionInfo     [In]- Encryption information (cipher, padding, IV, etc.)
  
RETURN VALUE
  SUCCESS           Successfully decrypted the content.
  EUNSUPPORTED      Cipher algorithm, padding type not supported.
  EBADPARM          Invalid parameters.
  ENOMEMORY
  EFAILED


COMMENTS
 This API can be called multiple times and the IV is updated at the end of
 each call. IV is stored in the DCF object.
 When the final set of data is to be decrypted, IsContentComplete should be set
 to TRUE and the DRM agent internally unpads (RFC2630) the content irrespective of 
 whether the input length is AES128 block multiple or not.
 For all intermediate decryption, IsContentComplete should be set to FALSE and
 the input length SHOULD be AES128 block multiple (AEEDRM_ENCRYPTION_BLOCK_SIZE). 
  

SIDE EFFECTS
  None
===========================================================================*/

#endif /* AEEDCF_H */
