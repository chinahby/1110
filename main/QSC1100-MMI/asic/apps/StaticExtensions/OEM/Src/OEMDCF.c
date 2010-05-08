
/*=============================================================================

FILE: OEMDCF.c

SERVICES: OEM Functions for DCF Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Src/OEMDCF.c#1 $
$DateTime: 2006/08/04 11:26:13 $ $Author: chonda $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
08/05/04     aag     Added uintvar encode/decode, the encryption parser
                     Modified OEMDRM_CreateContentHeader to use uintvar encode
                     Modified OEMDRM_ParseContentHeader to use uintvar decode
                     and the encryption header parser
07/13/04     jay     Added const Qualifiers
06/16/04     jay     Added functionality to create, parse and decrypt
05/20/04     jay     Initial Revision
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "BREWVersion.h"
#include "OEMFeatures.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "OEMHeap.h"
#if MIN_BREW_VERSION(3,0)
#include "AEE_OEMDispatch.h"
#include "OEMCriticalSection.h"
#endif

#ifdef FEATURE_SEC_DRM

#include "AEEDRM.h"
#include "AEEDCF.h"

#include "AESLib.h"
#include "msg.h"

// Backup
// #define USE_SECAPI
#ifdef USE_SECAPI
  #include "secapi.h"
  #include "secerrno.h"
#endif /* USE_SECAPI */



/* IDCF object */
struct IDCF
{
  AEEVTBL(IDCF)*        pvt;      
  IShell*               piShell;
  uint32                uRefs; 
  AEECLSID              ucls; 
  AESInstanceType       AESInstance;
  void*                 pac;        /* App context */
  AEECallback*          pcb;
};

/* Types of nodes encountered in the DCF header specification */
typedef enum OEMDCF_dcf_header_node_e_type
{
  OEMDCF_DCF_HEADER_NODE_MIN                  = 0,
  OEMDCF_DCF_HEADER_NODE_ENCRYPTION_METHOD    = OEMDCF_DCF_HEADER_NODE_MIN,
  OEMDCF_DCF_HEADER_NODE_PADDING              = 1,
  OEMDCF_DCF_HEADER_NODE_PLAINTEXTLEN         = 2,
  OEMDCF_DCF_HEADER_NODE_ENCRYPTION_SEPARATOR = 3,
  OEMDCF_DCF_HEADER_NODE_OPTIONAL_SEPARATOR   = 4,
  OEMDCF_DCF_HEADER_NODE_PADDING_SEPARATOR    = 5,
  /* Should these always be at the end because they are generic? */
  OEMDCF_DCF_HEADER_NODE_GENERIC_ALG_ID,
  OEMDCF_DCF_HEADER_NODE_GENERIC_PADDING_ID,
  OEMDCF_DCF_HEADER_NODE_MAX,
  OEMDCF_DCF_HEADER_NODE_INVAILD              = OEMDCF_DCF_HEADER_NODE_MAX
} OEMDCF_dcf_header_node_e_type;

/* ASCII String text for the names of the nodes in the DCF header spec */
static char *OEMDCF_dcf_header_node_names_ascii[OEMDCF_DCF_HEADER_NODE_MAX] = 
{
  "Encryption-Method",
  "padding",
  "paintextlen",
  ":",
  ";",
  "=",
  "AES128CBC",
  "RFC2630"
};

static uint32 OEMDCF_AddRef(IDCF *pDCF);
static uint32 OEMDCF_Release(IDCF *pDCF);
static int OEMDCF_QueryInterface(IDCF *pDCF, AEECLSID clsid, void **ppNew);

static int OEMDCF_CreateContentHeader( IDCF* pDCF,
                                       const char szContentURI[],
                                       const char szContentMIMEType[],
                                       const uint32 ContentDataLen,
                                       AEEDRM_EncryptionInfoType* const pEncryptionInfo,
                                       uint8* pDCFHeaderBuf,
                                       uint32* pDCFHeaderBufLen );
static int OEMDCF_ParseContentHeader( IDCF* pDCF,
                                      uint8* pContentBuf,
                                      const uint32 BufLen,
                                      AEEDCF_HeaderType* const pDCFHeader ); 
static int OEMDCF_DecryptContent( IDCF* pDCF,
                                  uint8* pContent,
                                  const uint32 ContentLen,
                                  uint8* pPlainText,
                                  uint32* pPlainTextLen,
                                  const boolean IsContentComplete,
                                  const AEEDRM_LicenseKeyType* pLicenseKey,
                                  const AEEDRM_EncryptionInfoType* const pEncryptionInfo );


/* The function supported by DCF object. */
static const AEEVTBL(IDCF) gOEMDCFFuncs = {
   OEMDCF_AddRef,
   OEMDCF_Release,
   OEMDCF_QueryInterface,
   OEMDCF_CreateContentHeader,
   OEMDCF_ParseContentHeader,
   OEMDCF_DecryptContent
};

/*===========================================================================

               INTERNAL FUNCTION DECLARATIONS   

===========================================================================*/
int OEMDCF_EncodeUintvar
( 
  uint8*                      pUintvarBuffer,
  uint32*                     pUintvarBufferLength,
  uint32                      ValueToEncode
);

int OEMDCF_DecodeUintvar
(
  uint8*                      pUintvarBuffer,
  uint32*                     pUintvarLength,
  uint32*                     pDecodedValue
);

int OEMDCF_ParseEncryptionHeader
( 
  uint8*                      pContentBuf,
  uint32                      BufLen,
  uint32*                     pBytesParsed,
  AEEDRM_EncryptionInfoType*  pEncryptionInfo
);
/*===========================================================================

                  Internal DCF FUNCTION DEFINITIONS   

===========================================================================*/

/* Copy a uint32 value to 4 consecutive bytes */
INLINE uint8* OEMDCF_Put32
( 
  uint32  x,   
  uint8*  cp             
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  *cp++ = x >> 24;
  *cp++ = (x >> 16) & 0xFF;
  *cp++ = (x >> 8) & 0xFF;
  *cp++ = x & 0xFF;
  return( cp);
} /* OEMDCF_Put32 */


/*  Copy 4 consecutive bytes to a uint32 */
INLINE uint32 OEMDCF_Get32
(
  uint8* cp               
)
{
  uint32 rval;            
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  rval = *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp++;
  rval <<= 8;
  rval |= *cp;

  return( rval);
} /* OEMDCF_Get32 */


/*===========================================================================

                  IDCF FUNCTION DEFINITIONS   

===========================================================================*/

/*=============================================================================
FUNCTION: OEMDCF_New

DESCRIPTION:  
  Allocates a new IDCF object,stores the App context and initializes object data.
  A new IDCF object is created for every ISHELL_CreateInstance().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMDCF_New
(
  IShell*   pIShell,
  AEECLSID  cls,
  void**    ppif
)
{
   IDCF *pNew;
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
#ifdef AEE_SIMULATOR
   /* Do not create an object if this is being run on the SDK. */
   *ppif = NULL;
   return EUNSUPPORTED;
#endif

   // Check parameters.
   if( (!pIShell) || (!ppif) )
   {
     return EBADPARM;
   }

   if (cls != AEECLSID_IDCF)
   {
     return EUNSUPPORTED;
   }

   // Allocate the object.
   pNew = (IDCF *) AEE_NewClass( (IBaseVtbl*)&gOEMDCFFuncs, sizeof(IDCF) ); 
   if (pNew == NULL)
   {
     return ENOMEMORY;
   }
      
   /* Initialize the values. */
   pNew->piShell = pIShell;
   pNew->ucls = cls;
   pNew->uRefs = 1;

   MEMSET( &(pNew->AESInstance), 0, sizeof(AESInstanceType) );
   
   pNew->pcb = NULL;
   
   pNew->pac = AEE_GetAppContext();

   *ppif = pNew;

   return AEE_SUCCESS;
} /* OEMDCF_New */


/*=============================================================================
FUNCTION: OEMDCF_AddRef

DESCRIPTION:  
  Indicates that one or more object is now pointing to object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMDCF_AddRef
(
  IDCF*  pMe
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (pMe)
  {
    return (++pMe->uRefs);
  } 
  return 0; 
}/* OEMDCF_AddRef */


/*=============================================================================
FUNCTION: OEMDCF_Release

DESCRIPTION:  
  Decreases the number of references to the IDCF object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMDCF_Release
(
  IDCF*  pMe
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!pMe)
  {
    return (0);
  }

  if (--pMe->uRefs != 0)
  {
    return pMe->uRefs;
  }
  
  /* Cancel scheduled callbacks */
  if ( pMe->pcb )
  {
    CALLBACK_Cancel( pMe->pcb );
  }
  
  /* Ref count is zero.  Release memory */
  FREE(pMe);
  return (0);
} /* OEMDCF_Release */


/*===========================================================================

FUNCTION: OEMDCF_QueryInterface

DESCRIPTION
  Allows applications to query if IDCF supports other interfaces and then
  returns a pointer to that new interface supported via IDCF.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMDCF_QueryInterface
(
  IDCF*     pMe,
  AEECLSID  id,
  void**    ppNew
)
{
  int ret_val = AEE_SUCCESS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( (!pMe) || (!ppNew) )
  {
    return EBADPARM;
  }

  switch (id)
  {
    case AEECLSID_IDCF:
    case AEECLSID_QUERYINTERFACE:
      if ( OEMDCF_AddRef(pMe) )
      {
        *ppNew = pMe;
      }
      else
      {
        *ppNew = NULL;
        ret_val = ECLASSNOTSUPPORT;
      }
      break;

    default:
      *ppNew = NULL;
      ret_val = ECLASSNOTSUPPORT;
      break;
  }
  
  return ret_val;
} /* OEMDCF_QueryInterface */


/*===========================================================================

FUNCTION: OEMDCF_CreateContentHeader

DESCRIPTION
  This interface is used to create a DCF header.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS
  EFAILED
  EBADPARM

SIDE EFFECTS
===========================================================================*/

static int OEMDCF_CreateContentHeader
(
  IDCF*                             pDCF,
  const char                        szContentURI[],
  const char                        szContentMIMEType[],
  const uint32                      ContentDataLen,
  AEEDRM_EncryptionInfoType* const  pEncryptionInfo,
  uint8*                            pDCFHeaderBuf,
  uint32*                           pDCFHeaderBufLen
)
{
  uint8  URILen = 0;
  uint8  MIMETypeLen = 0;
  uint8  PadLen = 0;
  uint8* pTmp = NULL;
  uint32 WorkLen = 0;
  uint32 HdrLen  = 0;
  uint32 CLen    = 0;
  uint32 UintvarLen = 6;
  const char EncryptionMethod[] = "Encryption-Method:AES128CBC;padding=RFC2630";
  int Errno = SUCCESS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Arguments sanity check */
  if ( pDCF == NULL
       ||
       szContentURI == NULL
       ||
       szContentMIMEType == NULL
       ||
       pEncryptionInfo == NULL
       ||
       pDCFHeaderBuf == NULL
       ||
       *pDCFHeaderBufLen == 0
       ||
       *pDCFHeaderBufLen < AEEDCF_DCFHDR_BUFFER_SIZE
     )
  {
    return EBADPARM;
  }

  URILen = STRLEN( szContentURI );
  MIMETypeLen = STRLEN( szContentMIMEType );

  if ( URILen == 0
       ||
       URILen > AEEDRM_MAX_URI_LEN
       ||
       MIMETypeLen == 0
       ||
       MIMETypeLen > AEEDCF_MIMETYPE_MAXLEN
     )
  {
    return EBADPARM;
  }

  /* Point to the incoming buffer */
  pTmp = pDCFHeaderBuf;

  /* Version 1.0 */
  *pTmp = 1;
  pTmp++;
  WorkLen++;

  /* Content MIME Length */
  *pTmp = MIMETypeLen;
  pTmp++;
  WorkLen++;

  /* URI Length */
  *pTmp = URILen;
  pTmp++;
  WorkLen++;

  /* MIME */
  MEMCPY( pTmp,
          szContentMIMEType,
          MIMETypeLen );
  pTmp += MIMETypeLen;
  WorkLen += MIMETypeLen;

  /* URI */
  MEMCPY( pTmp,
          szContentURI,
          URILen );
  pTmp += URILen;
  WorkLen += URILen;
 
  /* Hdr Len */
  HdrLen = STRLEN(EncryptionMethod);

  /* Write the Header length */
  Errno = OEMDCF_EncodeUintvar ( pTmp,
                                 &UintvarLen,
                                 HdrLen );
  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "ENCODE: Couldn't encode %u as a uintvar. (%d)",
        HdrLen, Errno, 0 );
    return EFAILED;
  }

  WorkLen += UintvarLen;
  pTmp += UintvarLen;

  /* ContentDataLen is the length of the plain text data */
  /* Length stored is  =  ContentDataLen + Pad len + IV Len */
  /* pad_len = block_size - (plaintext_len % block_size) */

  /* Calculate the Pad len */
  PadLen = AEEDRM_ENCRYPTION_BLOCK_SIZE -
           ( ContentDataLen % AEEDRM_ENCRYPTION_BLOCK_SIZE );
  
  /* Since const data */
  CLen = ContentDataLen;

  CLen += PadLen;
  
  /* Add IV len: IV Len = AEEDRM_ENCRYPTION_BLOCK_SIZE */
  CLen += AEEDRM_ENCRYPTION_BLOCK_SIZE;

  /* Write DataLen */
  UintvarLen = 6;

  Errno = OEMDCF_EncodeUintvar ( pTmp,
                                 &UintvarLen,
                                 CLen );
  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "ENCODE: Couldn't encode %u as a uintvar. (%d)",
      CLen, Errno, 0 );
    return EFAILED;
  }

  WorkLen += UintvarLen;
  pTmp += UintvarLen;

  /* Write the Headers */
  MEMCPY( pTmp, &EncryptionMethod[0], HdrLen );
  pTmp += HdrLen;
  WorkLen += HdrLen;

  /* Generate IV */
  GETRAND(pTmp, AEEDRM_ENCRYPTION_BLOCK_SIZE );
  WorkLen +=  AEEDRM_ENCRYPTION_BLOCK_SIZE;

  *pDCFHeaderBufLen = WorkLen;

  /* Fill up the encryption info */
  pEncryptionInfo->Method  = AEEDRM_ENC_METHOD_AES_128_CBC;
  pEncryptionInfo->Padding = AEEDRM_ENC_PADDING_RFC2630;
  MEMCPY( &(pEncryptionInfo->IV), pTmp, AEEDRM_ENCRYPTION_BLOCK_SIZE ); 

  return SUCCESS;
   
} /* OEMDCF_CreateContentHeader */


/*===========================================================================

FUNCTION: OEMDCF_ParseContentHeader

DESCRIPTION
  This interface is used to parse DCF header from .DCF file.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS,
  EFAILED,
  EBADPARM
  
SIDE EFFECTS
===========================================================================*/
static int OEMDCF_ParseContentHeader
( 
  IDCF*                     pDCF,
  uint8*                    pContentBuf,
  const uint32              BufLen,
  AEEDCF_HeaderType* const  pDCFHeader
)
{
  uint8   URILen = 0;
  uint8   MIMETypeLen = 0;
  uint8*  pTmp = NULL;
  uint32  WorkLen = 0;
  uint32  HdrLen  = 0;
  uint32  UintvarLen = 0;
  uint32  HeaderParsed = 0;
  int     Errno = SUCCESS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Arguments sanity check */
  if ( pDCF == NULL
       ||
       pContentBuf == NULL
       ||
       BufLen == 0
       ||
       pDCFHeader == NULL
     )
  {
    return EBADPARM;
  }

  /* Get the pointer to the input buffer */
  pTmp =  pContentBuf;

  do
  {
    /* Version */
    if ( *pTmp != 1 )
    {
      Errno = EFAILED;
      break;
    }
    else
    {
      pDCFHeader->Version = AEEDRM_OMA_1_0_VERSION;
      pTmp++;
      WorkLen++;
    }
    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Content Type Len */
    MIMETypeLen =  *pTmp;
    if ( MIMETypeLen > AEEDCF_MIMETYPE_MAXLEN )
    {
      Errno = EFAILED;
      break;
    }
    else
    {
      pTmp++;
      WorkLen++;
    }
    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* URI Len */
    URILen = *pTmp;
    if ( URILen > AEEDRM_MAX_URI_LEN )
    {
      Errno = EFAILED;
      break;
    }
    else
    {
      pTmp++;
      WorkLen++;
    }
    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Copy Content Type */
    MEMCPY( pDCFHeader->szContentMIMEType,
            pTmp,
            MIMETypeLen );
    
    /* Terminate the string with NULL */
    pDCFHeader->szContentMIMEType[MIMETypeLen] = '\0';

    pTmp += MIMETypeLen;
    WorkLen += MIMETypeLen;
    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Copy URI */
    MEMCPY( pDCFHeader->szContentURI,
            pTmp,
            URILen );
    
    /* Terminate the string with NULL */
    pDCFHeader->szContentURI[URILen] = '\0';

    pTmp += URILen;
    WorkLen += URILen;
    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Get the header Len */
    UintvarLen = 0;

    Errno = OEMDCF_DecodeUintvar ( pTmp,
                                   &UintvarLen,
                                   &HdrLen );
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't decode uintvar. (%d)", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    pTmp  += UintvarLen;
    WorkLen += UintvarLen;

    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Get the Content Len */
    UintvarLen = 0;

    Errno = OEMDCF_DecodeUintvar ( pTmp,
                                   &UintvarLen,
                                   &( pDCFHeader->ContentDataLen ) );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't decode uintvar. (%d)", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    pTmp += UintvarLen;
    WorkLen += UintvarLen;

    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Content Len includes the IV length, so remove that */
    pDCFHeader->ContentDataLen -=  AEEDRM_ENCRYPTION_BLOCK_SIZE;

    /*
     * Parse the encryption header
     */
    Errno = OEMDCF_ParseEncryptionHeader ( pTmp,
                                           HdrLen,
                                           &HeaderParsed,
                                           &( pDCFHeader->EncryptionInfo ) );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't parse the encryption header.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    if ( HeaderParsed != HdrLen )
    {
      MSG_MED ( "WARN: Entire header not parsed.", 0, 0, 0 );
    }

    pTmp += HdrLen;
    WorkLen += HdrLen;

    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* Copy the IV */
    MEMCPY( &(pDCFHeader->EncryptionInfo.IV[0]),
            pTmp,
            AEEDRM_ENCRYPTION_BLOCK_SIZE );
    pTmp += AEEDRM_ENCRYPTION_BLOCK_SIZE;
    WorkLen += AEEDRM_ENCRYPTION_BLOCK_SIZE;
    if ( WorkLen > BufLen )
    {
      Errno = EFAILED;
      break;
    }

    /* pTmp must now be pointing to the encrypted data */
    pDCFHeader->pContentData = pTmp;

    /* Sanity Check */
    if ( (WorkLen + pDCFHeader->ContentDataLen) != BufLen )
    {
      Errno = EFAILED;
      break;
    }

  } while( 0 );

  return Errno;

} /* OEMDCF_ParseContentHeader */



/*===========================================================================

FUNCTION: OEMDCF_DecryptContent

DESCRIPTION
  This function takes encrypted content and decrypted in place.

DEPENDENCIES
  
RETURN VALUE
  SUCCESS,
  EFAILED,
  EBADPARM,
  EUNSUPPORTED

SIDE EFFECTS
===========================================================================*/
static int OEMDCF_DecryptContent
( 
  IDCF*                                   pDCF,
  uint8*                                  pContent,
  const uint32                            ContentLen,
  uint8*                                  pPlainText,
  uint32*                                 pPlainTextLen,
  const boolean                           IsContentComplete,
  const AEEDRM_LicenseKeyType*            pLicenseKey,
  const AEEDRM_EncryptionInfoType* const  pEncryptionInfo
)
{
#ifndef USE_SECAPI
  
  int  Errno = SUCCESS;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( pDCF == NULL 
       ||
       pContent == NULL
       ||
       ContentLen == 0
       ||
       pPlainText == NULL
       ||
       *pPlainTextLen == 0
       ||
       pLicenseKey == NULL
       ||
       pEncryptionInfo == NULL
     )
  {
    return EBADPARM;
  }

  do
  {
    /* Sanity checks */
    if ( pEncryptionInfo->Method != AEEDRM_ENC_METHOD_AES_128_CBC )
    {
      Errno = EUNSUPPORTED;
      MSG_ERROR("Encryption method %d not supported",
                 pEncryptionInfo->Method, 0, 0 );
      break;
    }

    /* Check the key size */
    if ( pLicenseKey->KeyInfo.Size != AES128_KEY_SIZE )
    {
      Errno = EUNSUPPORTED;
      MSG_ERROR("License Key size %d not supported",
                 pLicenseKey->KeyInfo.Size, 0, 0 );
      break;
    }

    /* Length of the plaintext buffer must be atleast as big as
       ciphertext buffer */
    if ( *pPlainTextLen < ContentLen )
    {
      Errno = EFAILED;
      MSG_ERROR("PlainText buffer not big %d, %d",
                *pPlainTextLen,
                ContentLen, 0 );
    }

    /* Check if it is block multiple */
    if ( ( ContentLen % AEEDRM_ENCRYPTION_BLOCK_SIZE ) != 0 )
    {
      Errno = EFAILED;
      MSG_ERROR("Len %d is not block multiple", ContentLen, 0, 0 );
      break;
    }
    
    if ( IsContentComplete == TRUE )
    {
      if ( pEncryptionInfo->Padding != AEEDRM_ENC_PADDING_RFC2630 )
      {
        Errno = EUNSUPPORTED;
        MSG_ERROR( "Padding not supported, type: %d", 
                    pEncryptionInfo->Padding, 0, 0);
        break;
      }
    }
  } while( 0 );

  if ( Errno != SUCCESS )
  {
    return Errno;
  }

  /* Start the decryption process */
  /* Check if it is the first time DecryptContent is called with this
     instance of IDCF */
  if ( !pDCF->AESInstance.dec_subkeys_generated )
  {
    /* Copy the IV */
    MEMCPY( pDCF->AESInstance.dec.iv,
            pEncryptionInfo->IV,
            AEEDRM_ENCRYPTION_BLOCK_SIZE );

    /* Copy the Key */
    MEMCPY( pDCF->AESInstance.dec.key,
            pLicenseKey->KeyInfo.Value,
            AES128_KEY_SIZE );
    
    /* Set the padding type */
    pDCF->AESInstance.padding.type = RFC2630_PADDING;

  }

  /* Set the mode */
  pDCF->AESInstance.mode = MODE_CBC;

  do
  {
    /* Decrypt */
    Errno = AESLib_Decrypt( &(pDCF->AESInstance),
                            pContent,
                            pPlainText,
                            ContentLen );
    if ( Errno != SUCCESS )
    {
      MSG_ERROR("AES decryption failed", 0, 0, 0 );
      break;
    }

    /* update the plaintext buf length */
    *pPlainTextLen = ContentLen;

    /* If it is the final block, unpad and update the length */
    if ( IsContentComplete )
    {
      /* unpad */
      Errno = AESLib_Unpad( &(pDCF->AESInstance),
                            pPlainText,
                            pPlainTextLen );
      if ( Errno != SUCCESS )
      {
        MSG_ERROR("AES unpadding failed", 0, 0, 0 );
        break;
      }

      /* Be ready if another content is passed in for decryption */
      pDCF->AESInstance.dec_subkeys_generated = FALSE;
    }
  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    /* reset, and be ready for next call */
    pDCF->AESInstance.dec_subkeys_generated = FALSE;
  }
  
  return Errno;

#else  /* USE_SECAPI */

  int                          Errno = SUCCESS;
  secerrno_enum_type           _errno =  E_SUCCESS;
  secerrno_enum_type           _errno2 =  E_SUCCESS;
  secapi_handle_type           cipher_handle;
  secapi_enc_param_data_type   param_data;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if ( pDCF == NULL 
       ||
       pContent == NULL
       ||
       ContentLen == 0
       ||
       pPlainText == NULL
       ||
       *pPlainTextLen == 0
       ||
       pEncryptionInfo == NULL
     )
  {
    return EBADPARM;
  }

  do
  {
    /* Sanity checks */
    if ( pEncryptionInfo->Method != AEEDRM_ENC_METHOD_AES_128_CBC )
    {
      Errno = EUNSUPPORTED;
      MSG_ERROR("Encryption method %d not supported",
                 pEncryptionInfo->Method, 0, 0 );
      break;
    }

    /* Length field is only uint16 internally */
    if ( ContentLen > 0xFFFF
         ||
         *pPlainTextLen > 0xFFFF 
       )
    {
      Errno = EUNSUPPORTED;
      MSG_ERROR( "length %d too big: not supported",
                  ContentLen, 0, 0 );
      break;
    }

    /* Check the key size */
    if ( pLicenseKey->KeyInfo.Size != AES128_KEY_SIZE )
    {
      Errno = EUNSUPPORTED;
      MSG_ERROR("License Key size %d not supported",
                 pLicenseKey->KeyInfo.Size, 0, 0 );
      break;
    }

    /* Check if it is block multiple */
    if ( ( ContentLen % AEEDRM_ENCRYPTION_BLOCK_SIZE ) != 0 )
    {
      Errno = EFAILED;
      MSG_ERROR("Len %d is not block multiple", ContentLen, 0, 0 );
      break;
    }
    
    if ( IsContentComplete == TRUE )
    {
      if ( pEncryptionInfo->Padding != AEEDRM_ENC_PADDING_RFC2630 )
      {
        Errno = EUNSUPPORTED;
        MSG_ERROR( "Padding not supported, type: %d", 
                    pEncryptionInfo->Padding, 0, 0);
        break;
      }
    }
  } while( 0 );

  if ( Errno != SUCCESS )
  {
    return Errno;
  }

  /* Start the Decryption process */
  do
  {
    /* Create the cipher instance */
    _errno = secapi_new( &cipher_handle, SECAPI_AES128 );
    if ( _errno != E_SUCCESS )
    {
      MSG_ERROR("Cipher instance creation failed (%d)", _errno, 0, 0);
      return EFAILED;
    }
    
    /* Set up decryption parameters */

    /* IV and Keys */
    param_data.init_data.dec.key_ptr = pLicenseKey->KeyInfo.Value;
    param_data.init_data.dec.iv_ptr  = pEncryptionInfo->IV;;

    /* Just to overcome crypto bug */
    param_data.init_data.enc.key_ptr = LicenseKey.KeyInfo.Value;
    param_data.init_data.enc.iv_ptr  = NULL;
    
    _errno = secapi_enc_set_param( cipher_handle,
                                   SECAPI_AES128,
                                   SECAPI_ENC_INIT_DATA,
                                   &param_data  );
    if ( _errno != E_SUCCESS )
    {
      MSG_ERROR("Setting IV/Keys failed (%d)", _errno, 0, 0 );
      _errno = E_FAILURE;
      break;
    }

    /* Feedback type */
    param_data.op_mode = SECAPI_ENC_OP_MODE_CBC;
    _errno = secapi_enc_set_param( cipher_handle,
                                   SECAPI_AES128,
                                   SECAPI_ENC_OP_MODE,
                                   &param_data  );
    if ( _errno != E_SUCCESS )
    {
      MSG_ERROR("Setting OP mode failed (%d)", _errno, 0, 0 );
      _errno = E_FAILURE;
      break;
    }

    /* If it is the final stretch need to pad */
    if ( IsContentComplete )
    {
      /* Padding type */
      param_data.padding.type = SECAPI_ENC_RFC2630_PADDING;
      _errno = secapi_enc_set_param( cipher_handle,
                                     SECAPI_AES128,
                                     SECAPI_ENC_PADDING_TYPE,
                                     &param_data  );
      if ( _errno != E_SUCCESS )
      {
        MSG_ERROR("Setting padding type failed (%d)", _errno, 0, 0 );
        _errno = E_FAILURE;
        break;
      }
    }

    /* Execution plaform */
    param_data.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
    _errno = secapi_enc_set_param( cipher_handle,
                                   SECAPI_AES128,
                                   SECAPI_EXEC_PLATFORM,
                                   &param_data );
    if ( _errno != E_SUCCESS ) 
    {
      MSG_ERROR( "Set pref. platform error: (%d)", _errno, 0, 0 );
      _errno = E_FAILURE;
      break;
    }
   
    /* Decrypt */
    _errno = secapi_enc_decrypt( cipher_handle,
                                 SECAPI_AES128,
                                 pContent,
                                 ContentLen,
                                 pPlainText,
                                 (uint16*)pPlainTextLen );
                                 
    if ( _errno != E_SUCCESS )
    {
      MSG_ERROR("Decryption failed (%d)", _errno, 0, 0 );
      _errno = E_FAILURE;
      break;
    }

    /* Read the updated IV */
    param_data.cbc_state.dec_ptr = pEncryptionInfo->IV;
    param_data.cbc_state.enc_ptr = NULL;
    _errno =  secapi_enc_get_param( cipher_handle,
                                    SECAPI_AES128,
                                    SECAPI_ENC_CBC_STATE,
                                    &param_data );
    if ( _errno != E_SUCCESS )
    {
      MSG_ERROR( "Failed to read IV (%d)", _errno, 0, 0 );
      _errno = E_FAILURE;
      break;
    }
  
  } while ( 0 );

  /* Delete the cipher instance */
  _errno2 = secapi_delete( &cipher_handle );
  if ( _errno2 != E_SUCCESS )
  {
    MSG_ERROR("Failed to delete cipher instance (%d)", _errno, 0, 0 );
    _errno = _errno2;
  }

  /* Translate error codes */
  if ( _errno != E_SUCCESS )
  {
    Errno = EFAILED;
  }
  
  return Errno;

#endif /* USE_SECAPI */ 
  
} /* OEMDCF_DecryptContent */

/*===========================================================================

FUNCTION: OEMDCF_DecodeUintvar

DESCRIPTION
  This function takes a Uintvar value and its length and decodes it.

DEPENDENCIES
  
RETURN VALUE
  SUCCESS,
  EFAILED,
  EBADPARM,
  EUNSUPPORTED

SIDE EFFECTS
===========================================================================*/
int OEMDCF_DecodeUintvar
(
  uint8*                      pUintvarBuffer,
  uint32*                     pUintvarLength,
  uint32*                     pDecodedValue
)
{
  /* Variable declarations */
  int                         Errno = SUCCESS;
  int                         Offset = 0;
  boolean                     Continue = TRUE;
  uint32                      TempInt = 0;
  uint32                      Mask = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#define UINTVAR_MAX_BUFFER_LENGTH 5


  do
  {
    /*
     * Do basic sanity checks
     */
    if ( pUintvarBuffer == NULL 
         || 
         pDecodedValue == NULL
         ||
         pUintvarLength == NULL )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }

    /*
     * In this loop we do the actual decoding
     *
     * TempInt has the intermediate value held in the uintvar
     * Mask holds the value contained in the 7 data bits of the uintvar
     * Continue holds the value of the continue bit
     * Offset counts how many bytes the uintvar has
     *
     * One thing to note is the double negative ( Continue != FALSE )
     *
     * boolean is actually typedef'd as an int.  When I mask a byte to 
     * get the top bit and assign it to a boolean, what I actually get is
     * 128, _not_ 1.  However, TRUE is defined as 1...
     *
     * I go from MSB -> LSB
     */
    do
    {
      TempInt = TempInt << 7;
      Mask = ( *( pUintvarBuffer + Offset ) ) & 0x7F;
      Continue =   *( pUintvarBuffer + Offset ) & 0x80;
      /*
       * Transfer the data bits to the intermediate value
       */
      TempInt = TempInt | Mask;
      Offset = Offset + 1;
    } while ( Continue != FALSE
              &&
              Offset < UINTVAR_MAX_BUFFER_LENGTH );

    /*
     * Check to see if the buffer is too large
     *
     * It is possible for you to break out of the above loop for two reasons:
     *
     * 1) Continue is now FALSE ( this means the uintvar is decoded)
     * 2) Offset is now greater than or equal to the max uintvar buffer 
     *    length (5)
     *
     * What we check for here is the error condition where Continue is still
     * true _and_ we're on the 5th byte.  What this means is that there's an
     * error in the uintvar, because the continue bit of the maximum 
     * specified byte should never be a 1
     */
    if ( Continue != FALSE 
         &&
         Offset >= UINTVAR_MAX_BUFFER_LENGTH
         )
    {
      MSG_ERROR ( "FAIL: Uintvar buffer is too long.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the variables to be returned
     */
    *pUintvarLength = Offset;
    *pDecodedValue = TempInt;
  } while ( 0 );

   
  return Errno;

} /* OEMDCF_DecodeUintvar */



/*===========================================================================

FUNCTION: OEMDCF_EncodeUintvar

DESCRIPTION
  This function takes a Uintvar value and its length and encodes it.

DEPENDENCIES
  
RETURN VALUE
  SUCCESS,
  EFAILED,
  EBADPARM,

SIDE EFFECTS
===========================================================================*/
int OEMDCF_EncodeUintvar
( 
  uint8*                      pUintvarBuffer,
  uint32*                     pUintvarBufferLength,
  uint32                      ValueToEncode
)
{
  int                         Errno = SUCCESS;
  int                         Offset = 0;
  int                         EndOfBuffer = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    /*
     * Basic sanity check
     */
    if ( pUintvarBuffer == NULL 
         || 
         pUintvarBufferLength == NULL 
         ||
         *pUintvarBufferLength == 0 )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }

    /*
     * If the value is 0 optimize
     */
    if ( ValueToEncode == 0  && *pUintvarBufferLength != 0 )
    {
      *pUintvarBufferLength = 1;
      MEMSET ( pUintvarBuffer, 0, 1 );
      Errno = SUCCESS;
      break;
    }

    /*
     * Do some comparisons to figure out how many times we have to shift
     */
    if ( ValueToEncode <= 0x007F )
    {
      Offset = 0;
    }
    else if ( ValueToEncode <= 0x3FFF ) /* 16383 */
    {
      Offset = 1;
    }
    else if ( ValueToEncode <= 0x001FFFFF ) /* 2097151 */
    {
      Offset = 2;
    }
    else if ( ValueToEncode <= 0x0FFFFFFF ) /* 268435455 */
    {
      Offset = 3;
    }
    else
    {
      Offset = 4;
    }

    EndOfBuffer = Offset;
        
    /*
     * Check if the buffer is large enough
     */
    if ( ( Offset + 1 ) > *pUintvarBufferLength )
    {
      MSG_ERROR ( "Buffer too small.  Required: %d.  Actual: %d",
          ( Offset + 1 ), *pUintvarBufferLength, 0 );
      Errno = ENOMEMORY;
      break;
    }

    /*
     * Initialize the buffer to a known state
     */
    MEMSET ( pUintvarBuffer, 0, ( Offset + 1 ) );


    *pUintvarBufferLength = ( Offset + 1 );

    /*
     * Do the actual bit setting
     *
     * Every 8th bit (top of each byte) is a continue bit:
     *
     * C|P|P|P|P|P|P|P <-- LSB
     * represents the structure of each byte in the uintvar structure.  Only
     * the last block will have a continue bit of 0
     *
     *
     * I go from LSB -> MSB ( its simpler )
     */
    do
    {
      /*
       * Get the lowest 7 bits
       */
      *( pUintvarBuffer + Offset ) = ValueToEncode & 0x7F;

      /*
       * Set the mask
       */
      if ( Offset != EndOfBuffer )
      {
        *( pUintvarBuffer + Offset ) |= 0x80;
      }
      /*
       * Move to the more significant byte and then remove the bits in the
       * least significant byte ( since we've used them already )
       */
      Offset--;
      ValueToEncode = ( ValueToEncode >> 7 );
    } while ( Offset >= 0 );

    /*
     * Note...one can do even more optimizations by memsetting to 1 first
     */



  } while ( 0 );



  return Errno;
} /* OEMDCF_DecodeUintvar */



/*===========================================================================

FUNCTION: OEMDCF_SkipSpaces

DESCRIPTION
  This is a convenience function to skip spaces in a buffer

DEPENDENCIES
  
RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void OEMDCF_SkipSpaces
      ( char**                        pContent,
        char**                        pEndOfBuffer
      )
{
  /* Variables declared */
  int                                 SameString = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  /*
   * Search for the space as long as you're not overrunning the buffer
   */
  while ( *pContent < *pEndOfBuffer )
  {
    /*
     * Search for the space
     */
    SameString = STRNCMP ( *pContent, " ", 1 );

    /*
     * If we don't find one...then lets break out and compare
     */
    if ( SameString != 0 )
    {
      break;
    }
    else
    {
      /*
       * If we do find a space then keep going...
       */
      ( *pContent )++;
    }
  }
}
/*===========================================================================

FUNCTION: OEMDCF_ParseEncryptionHeader

DESCRIPTION
  This function takes a pointer to a character buffer and parses it for
  the encryption header.

DEPENDENCIES
  
RETURN VALUE
  SUCCESS,
  EFAILED,
  EBADPARM,
  EUNSUPPORTED

SIDE EFFECTS
===========================================================================*/
int OEMDCF_ParseEncryptionHeader
( 
  uint8*                      pContentBuf,
  uint32                      BufLen,
  uint32*                     pBytesParsed,
  AEEDRM_EncryptionInfoType*  pEncryptionInfo
)
{
  /* Variable declarations */
  boolean                             DoneParsing = FALSE;
  int                                 Errno = SUCCESS;
  int                                 SameString = 0;
  char*                               pContent = NULL;
  char*                               pEndOfBuffer = NULL;
  OEMDCF_dcf_header_node_e_type       ExpectedNode;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */



  do
  {
    /*
     * Basic sanity checks
     */
    if ( ( pContentBuf == NULL )
         ||
         ( pEncryptionInfo == NULL )
         ||
         ( pBytesParsed == NULL )
         ||
         ( BufLen == 0 )
       )
    {
      if ( BufLen == 0 )
      {
        MSG_ERROR ( "FAIL: No encryption information.", 0, 0, 0 );
      }
      else
      {
        MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      }
      Errno = EBADPARM;
      break;
    }

    /*
     * If the buffer length is too short for even "Encryption-Method", then
     * just stop now
     */
    if ( BufLen <= STRLEN ( OEMDCF_dcf_header_node_names_ascii[0] ) )
    {
      MSG_ERROR ( "FAIL: Buffer size too small for encryption headers.",
        0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Initial setup
     */
    pContent = (char*) pContentBuf; /* This pointer moves */
    pEndOfBuffer = pContent + BufLen;
    ExpectedNode = OEMDCF_DCF_HEADER_NODE_ENCRYPTION_METHOD;



    /*
     * Loop as long as the functionality isn't complete and there are no
     * errors
     */
    do
    {


      /*
       * Switch depending on which node you now expect to parse
       */
      switch ( ExpectedNode )
      {



        /*
         * Check for "Encryption-Method"
         */
        case OEMDCF_DCF_HEADER_NODE_ENCRYPTION_METHOD:


          /*
           * Find the "Encryption-Method" substring
           *
           * NOTE: I _cannot_ use STRSTR because it relies on null terminated
           * strings to work.  The header is not null terminated
           */
          do
          {


            /*
             * Check for "Encryption-Method" at the current byte
             */
            SameString = STRNCMP
                          ( pContent,
                            OEMDCF_dcf_header_node_names_ascii[0],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[0] )
                          );

            /*
             * If we aren't finding it, then move to the next byte
             */
            if ( SameString != 0 )
            {
              pContent++;
            }
            else
            {
              /*
               * We found it...so lets break!
               */
              break;
            }



          } while ( pContent 
                    <= 
                    ( pEndOfBuffer - 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[0] )
                    )
                  );

          /*
           * We can't find it...so get out now
           */
          if ( SameString != 0 )
          {
            MSG_ERROR ( "FAIL: Unrecognized encryption header.", 0, 0, 0 );
            Errno = EFAILED;
          }
          else
          {
            pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[0] );
            ExpectedNode = OEMDCF_DCF_HEADER_NODE_ENCRYPTION_SEPARATOR;
          }
          break;



        case OEMDCF_DCF_HEADER_NODE_ENCRYPTION_SEPARATOR:
          /*
           * We're actually looking to match ":"
           *
           * Ok...now it appears we have a different problem.  It appears in
           * practice that people are really 'lax' when it comes to following
           * the RFC's recommendation WRT spaces...which means we have to
           * search for the damn spaces
           */
          
          OEMDCF_SkipSpaces ( &pContent, &pEndOfBuffer );

          /*
           * If we're at the end of the buffer and all we saw were spaces
           */
          if ( pContent >= pEndOfBuffer )
          {
            MSG_ERROR ( "FAIL: Can't find encryption method separator.",
               0, 0, 0 );
            Errno = EFAILED;
          }
          /*
           * Only run this if we aren't going to overrun the buffer
           */
          else if ( ( pContent 
                      + 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[3] )
                    )
                    <=
                    pEndOfBuffer
                  )
          {
            /*
             * Try to find the ":"
             */
            SameString = STRNCMP
                          ( pContent,
                            OEMDCF_dcf_header_node_names_ascii[3],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[3] )
                          );

            /*
             * If it fails...
             */
            if ( SameString != 0 )
            {
              MSG_ERROR ( "FAIL: Invalid encryption method separator.",
                  0, 0, 0 );
              Errno = EFAILED;
            }
            else
            {
              /*
               * Move on to try an find the actual encryption algorithm
               */
              ExpectedNode = OEMDCF_DCF_HEADER_NODE_GENERIC_ALG_ID;
              pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[3] );
            }
          }
          else
          {
            MSG_ERROR ( "FAIL: Buffer is too small.", 0, 0, 0 );
            Errno = EFAILED;
            break;
          }
          break;



        /*
         * Check "padding"
         */
        case OEMDCF_DCF_HEADER_NODE_PADDING:

          OEMDCF_SkipSpaces ( &pContent, &pEndOfBuffer );

          if ( pContent >= pEndOfBuffer )
          {
            MSG_ERROR ( "FAIL: Can't find padding header.", 0, 0, 0 );
            Errno = EFAILED;
          }
          else if ( ( pContent 
                      + 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[1] )
                    )
                    <=
                    pEndOfBuffer
                  )
          {
            SameString = STRNCMP
                          ( pContent, 
                            OEMDCF_dcf_header_node_names_ascii[1],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[1] )
                          );

            if ( SameString != 0 )
            {
              MSG_ERROR ( "FAIL: Unrecognized padding header.", 0, 0, 0 );
              Errno = EFAILED;
            }
            else
            {
              ExpectedNode = OEMDCF_DCF_HEADER_NODE_PADDING_SEPARATOR;
              pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[1] );
            }
          }
          else
          {
            MSG_ERROR ( "FAIL: Buffer is too small.", 0, 0, 0 );
            Errno = EFAILED;
            break;
          }
          break;


          
        /*
         * Check for "="
         */
        case OEMDCF_DCF_HEADER_NODE_PADDING_SEPARATOR:

          OEMDCF_SkipSpaces ( &pContent, &pEndOfBuffer );

          if ( pContent >= pEndOfBuffer )
          {
            MSG_ERROR ( "FAIL: Can't find padding header separator.", 0, 0, 0 );
            Errno = EFAILED;
          }
          else if ( ( pContent 
                      + 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[5] )
                    )
                    <=
                    pEndOfBuffer
                  )
          {
            /*
             * Well...we found a non-space
             */
            SameString = STRNCMP 
                          ( pContent,
                            OEMDCF_dcf_header_node_names_ascii[5],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[5] )
                          );

            if ( SameString != 0 )
            {
              MSG_ERROR ( "FAIL: Invalid padding header separator.",
                  0, 0, 0 );
              Errno = EFAILED;
            }
            else
            {
              /*
               * We've got this far, so we're looking to match the padding
               * type
               */
              ExpectedNode = OEMDCF_DCF_HEADER_NODE_GENERIC_PADDING_ID;
              pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[5] );
            }
          }
          else
          {
            MSG_ERROR ( "FAIL: Buffer is too small.", 0, 0, 0 );
            Errno = EFAILED;
            break;
          }
          break;



        case OEMDCF_DCF_HEADER_NODE_PLAINTEXTLEN:
          break;



        /*
         * We're looking to recognize the actual encryption method
         *
         * Currently we can only recognize "AES128CBC"
         */
        case OEMDCF_DCF_HEADER_NODE_GENERIC_ALG_ID:


          /*
           * We're looking for "AES128CBC"
           *
           * Ok...now it appears we have a different problem.  It appears in
           * practice that people are really 'lax' when it comes to following
           * the RFC's recommendation WRT spaces...which means we have to
           * search for the damn spaces
           */

          OEMDCF_SkipSpaces ( &pContent, &pEndOfBuffer );


          /*
           * If we keep finding spaces...
           */
          if ( pContent >= pEndOfBuffer )
          {
            MSG_ERROR ( "FAIL: Can't find encryption method.", 0, 0, 0 );
            Errno = EFAILED;
          }
          else if ( ( pContent 
                      + 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[6] )
                    )
                    <=
                    pEndOfBuffer
                  )
          {
            /*
             * Check if we can see the "AES128CBC"
             */
            SameString = STRNCMP 
                          ( pContent,
                            OEMDCF_dcf_header_node_names_ascii[6],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[6] )
                          );

            if ( SameString != 0 )
            {
              MSG_ERROR ( "FAIL: Invalid encryption method.", 0, 0, 0 );
              Errno = EFAILED;
            }
            else
            {
              /* Set the structure */
              pEncryptionInfo->Method = AEEDRM_ENC_METHOD_AES_128_CBC;

              pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[6] );

              /*
               * Ok...if there's still space in the buffer, go on
               */
              if ( pContent < pEndOfBuffer )
              {
                ExpectedNode = OEMDCF_DCF_HEADER_NODE_OPTIONAL_SEPARATOR;
              }
              else
              {
                /*
                 * There's no space left, so the optional padding stuff was
                 * ignored
                 *
                 * So...we signal that we're done and set the padding info
                 */
                DoneParsing = TRUE;
                pEncryptionInfo->Padding = AEEDRM_ENC_PADDING_RFC2630;
              }

            }
          }
          else
          {
            MSG_ERROR ( "FAIL: Buffer is too small.", 0, 0, 0 );
            Errno = EFAILED;
            break;
          }
          break;



          /*
           * We're looking for ";" now
           */
        case OEMDCF_DCF_HEADER_NODE_OPTIONAL_SEPARATOR:

          OEMDCF_SkipSpaces ( &pContent, &pEndOfBuffer );

          /*
           * If we keep finding spaces...
           */
          if ( pContent >= pEndOfBuffer )
          {
            /*
             * Well...I guess its valid for the rest of the header to be
             * spaces...after all, the ";" is optional
             */
            DoneParsing = TRUE;
            pEncryptionInfo->Padding = AEEDRM_ENC_PADDING_RFC2630;
          }
          else if ( ( pContent 
                      + 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[4] )
                    )
                    <=
                    pEndOfBuffer
                  )
          {
            SameString = STRNCMP 
                          ( pContent,
                            OEMDCF_dcf_header_node_names_ascii[4],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[4] )
                          );

            if ( SameString != 0 )
            {
              /*
               * The ";" indicates that more information is coming about
               * parameters associated with the encryption method.  If there's
               * no such information, then we're done parsing the
               * Encryption-Method header
               */
              DoneParsing = TRUE;

              /*
               * However, we still want to fill in the padding info to what is
               * 'implied'
               */
              pEncryptionInfo->Padding = AEEDRM_ENC_PADDING_RFC2630;

              MSG_FATAL ( "NOTE: Padding method set to default.", 0, 0, 0 );
            }
            else if ( SameString == 0 )
            {
              /*
               * Ok...the next step is to find out the padding then
               */
              ExpectedNode = OEMDCF_DCF_HEADER_NODE_PADDING;
              pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[4] );
            }
          }
          else
          {
            MSG_ERROR ( "FAIL: Buffer is too small.", 0, 0, 0 );
            Errno = EFAILED;
            break;
          }
          break;



        /*
         * We're trying to reconize the padding type
         *
         * Currently we only know about "RFC2630"
         */
        case OEMDCF_DCF_HEADER_NODE_GENERIC_PADDING_ID:

          OEMDCF_SkipSpaces ( &pContent, &pEndOfBuffer );

          if ( pContent >= pEndOfBuffer )
          {
            MSG_ERROR ( "FAIL: Can't find padding method.", 0, 0, 0 );
            Errno = EFAILED;
          }
          else if ( ( pContent 
                      + 
                      STRLEN ( OEMDCF_dcf_header_node_names_ascii[7] )
                    )
                    <=
                    pEndOfBuffer
                  )
          {
            SameString = STRNCMP 
                          ( pContent,
                            OEMDCF_dcf_header_node_names_ascii[7],
                            STRLEN ( OEMDCF_dcf_header_node_names_ascii[7] )
                          );
            if ( SameString != 0 )
            {
              MSG_ERROR ( "FAIL: Invalid padding method.", 0, 0, 0 );
              Errno = EFAILED;
            }
            else
            {
              /*
               * Set the padding type
               */
              pEncryptionInfo->Padding = AEEDRM_ENC_PADDING_RFC2630;
            
              /*
               * We're not checking for anything beyond this so we move past
               * this to the end
               */
              DoneParsing = TRUE;
              pContent += STRLEN ( OEMDCF_dcf_header_node_names_ascii[7] );
            }
          }
          else
          {
            MSG_ERROR ( "FAIL: Buffer is too small.", 0, 0, 0 );
            Errno = EFAILED;
            break;
          }
          break;

        default:
          Errno = EFAILED;
          break;
      }
    } while ( ( pContent < pEndOfBuffer )
        && 
        ( Errno == SUCCESS )
        &&
        ( DoneParsing == FALSE )
        );


    /*
     * If the encryption header is not parsed and the buffer looks like it'll
     * be overrun, then there's an error
     */
    if ( ( pContent >= pEndOfBuffer )
         &&
         ( DoneParsing == FALSE )
       )
    {
      MSG_ERROR ( "FAIL: Incomplete encryption headers.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

  } while ( 0 );


  *pBytesParsed = ( BufLen - ( pEndOfBuffer - pContent ) );
  return Errno;
} /* OEMDCF_ParseEncryptionHeader */


#endif /* FEATURE_SEC_DRM */

