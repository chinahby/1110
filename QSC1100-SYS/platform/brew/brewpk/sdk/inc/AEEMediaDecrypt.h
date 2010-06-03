#ifndef AEEMEDIADECRYPT_H
#define AEEMEDIADECRYPT_H

/*============================================================================
FILE: AEEMediaDecrypt.h

SERVICES:  BREW IMedia Decryption Service

DESCRIPTION:
   This file defines a generic set of APIs, that are IMedia interface enhancements,
   to enable decryption of encrypted media content.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

Copyright © 1999-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/
/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/
/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEEMedia.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
#define MM_PARM_DECRYPT_CAPS     (MM_PARM_BASE + 17)  // [Get] Returns supported decryption capabilities and decryption formats for the media
#define MM_PARM_DECRYPT_INFO     (MM_PARM_BASE + 18)  // [Set/Get] Provides required decryption info to enable media content decryption

//
// IMedia decryption capabilities
//
#define MM_DECRYPT_CAPS_NONE        0x00000000  // Decryption not supported for the media. But app can do decryption, if possible.
#define MM_DECRYPT_CAPS_KEY         0x00000001  // IMedia object accepts key and decrypts media content
#define MM_DECRYPT_CAPS_TXID        0x00000002  // IMedia object accepts DRM transaction ID, queries DRM agent for key and decrypts media content
#define MM_DECRYPT_CAPS_USER_BASE   0x00010000  // Caps base used by derived class

//
// IMedia decryption methods
//
#define MM_DECRYPT_FMT_NONE         1           // Input is not encrypted
#define MM_DECRYPT_FMT_DCF          2           // Supports DCF format and most algorithms used by DCF
#define MM_DECRYPT_FMT_AES128CBC    3           // 128 bit AES, CBC mode with RFC 2630 padding
#define MM_DECRYPT_FMT_AES128CTR    4           // 128 bit AES, CTR mode
#define MM_DECRYPT_FMT_USER_BASE    0x8000000   // Fmt base used by derived class

typedef struct AEEMediaDecryptInfo
{
   int16    wStructSize;   // Size of this struct
   uint32   dwFormat;      // Decryption format: MM_DECRYPT_FMT_XXX
   uint32   dwType;        // Decrypt info type: MM_DECRYPT_CAPS_XXX

   uint8 *  pKey;          // Decryption key buffer and length. Used if dwType
   int16    wKeyLen;       // is MM_DECRYPT_CAPS_KEY. Local (context) use allowed otherwise.

   uint32   dwTxID;        // Transaction ID. Used if dwType is MM_DECRYPT_CAPS_TXID. Local (context) use allowed otherwise.
} AEEMediaDecryptInfo;


//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------

//===============================================================================
// FUNCTION DECLARATIONS and INLINE FUNCTION DEFINITIONS
//===============================================================================

//-------------------------------------------------------------------
// Interface Definitions - C Style
//-------------------------------------------------------------------

#define IMEDIA_GetDecryptCaps(p, pdw, ppdw)     IMEDIA_GetMediaParm((IMedia *)(p), MM_PARM_DECRYPT_CAPS, (int32 *)(pdw), (int32 *)(ppdw))
#define IMEDIA_SetDecryptInfo(p, pdi)           IMEDIA_SetMediaParm((IMedia *)(p), MM_PARM_DECRYPT_INFO, (int32)(pdi),   0)


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEMediaDecryptInfo

Description:
AEEMediaDecryptInfo specifies the decryption infomation required by IMedia object
to decrypt the encrypted media content.

Definition:
   typedef struct AEEMediaDecryptInfo
   {
      int16    wStructSize;
      uint32   dwFormat;
      uint32   dwType;

      uint8 *  pKey;
      int16    wKeyLen;

      uint32   dwTxID;
   } AEEMediaDecryptInfo;

Members:
   wStructSize:   Size of this struct
   dwFormat:      Decryption method: MM_DECRYPT_FMT_XXX
   dwType:        Type of decrypt info: MM_DECRYPT_CAPS_XXX             ~
   pKey, wKeyLen: Decryption key buffer and length. Used if dwType is MM_DECRYPT_CAPS_KEY.
                  Local (context) use allowed otherwise.
   dwTxID:        Transaction ID (See Comments section for DRM Transaction IDs). Used if dwType is MM_DECRYPT_CAPS_TXID.
                  Local (context) use allowed otherwise.

Comments:
   The precise meaning of pKey, wKeyLwn, and dwTxID depend on the decryption
   capability and format. Formats not defined here should define the meaning
   of these fields.

   A given media player can define further parameters to control content
   decryption if they can't be represented by this structure.

   DRM transaction IDs are just a simple uint32 handle that is useful if the media player can interact directly 
   with the DRM agent. Typically the caller app acquires the transaction ID from the DRM agent direclty in lieu 
   of the decrypting key. It passes this on to the media player which interacts with the DRM agent and is given the 
   decryption key.

See Also:
   IMEDIA_SetDecryptInfo()
   IMEDIA_GetDecryptCaps()

=============================================================================

=====================================================================
  INTERFACE DOCUMENTATION
=====================================================================

IMediaDecrypt Interface

Description:
For more details on IMedia interface, refer to IMedia interface description section.

===H2>
Rendering of encrypted media
===/H2>

IMedia specifies generic APIs to enable decryption and playback of encrypted media content.

Content encryption may be based on any standard or custom mechanisms. It may also be part
of DRM system.

[Note: Throughtout the following documentation, media player (codec) refers to the module that performs
actual decoding and rendering of the media content. Typically, IMedia-based implementations interface with
media players to allow BREW apps to render the media content.]

IMedia specifies the following generic ways to enable media decryption:
~
(1) App performs the media decryption. In this case, media player is not involved in decryption.
(2) App obtains the key, passes it to media player, which does the decryption
(3) App fetches a transcation ID from DRM agent, passes it to media player, which fetches the key
    and performs decryption.
*
A media player and calling app can define another convention if need be.

Following steps explain the usage:
~
(1) App gets the MIME type or file extension for the content
(2) App looks up the IMedia-based handler in the Brew registry, creates the IMedia object
    and associates the content to the IMedia object
(3) App queries the IMedia object using IMEDIA_GetDecryptCaps() to find out IMedia object's decryption capabilities.
~
    (1) If it can do none (MM_DECRYPT_CAPS_NONE), then app performs the media decryption and provides the data to the IMedia object
    (2) If it can decrypt using a key (MM_DECRYPT_CAPS_KEY), then app gets the key.
        Uses IMEDIA_SetDecryptInfo() to pass the key and decryption method to the IMedia object, which performs decryption
    (3) If it can accept a transaction ID (MM_DECRYPT_CAPS_TXID), then app gets transaction ID from DRM agent
*
*
    
        Uses IMEDIA_SetDecryptInfo() to pass the transaction ID  and decryption method to the IMedia object, which fetches the key from DRM agent
        and performs decryption

Following table gives the list of IMedia decryption get/set parameters:
===pre>
nParmID                 Op       Desc                       p1                                  p2
----------------------------------------------------------------------------------------------------
MM_PARM_DECRYPT_CAPS    Get      Returns supported          Ptr to ORed MM_DECRYPT_CAPS_XXX     Zero-terminated list
                                 decryption capabilities                                        of decryption formats
                                 & decryption formats for                                       (MM_DECRYPT_FMT_XXX)
                                 the media
MM_PARM_DECRYPT_INFO    Set/Get  Provides decryption info   MM_DECRYPT_CAPS_XXX                 AEEMediaDecryptInfo *
                                 (key) to enable media
                                 content decryption
===/pre>

The following header files are required:
AEEMediaDecrypt.h
AEEMedia.h

=============================================================================

=============================================================================

IMEDIA_GetDecryptCaps()

Description:
   This function retrieves the decryption capabilities, including the
   decryption formats, supported for the media

Prototype:
   int IMEDIA_GetDecryptCaps(IMedia * po, uint32 * pdwCaps, const uint32 ** ppdwFormats);

Parameters:
   po :     Pointer to the IMedia Interface object
   pdwCaps: Pointer to ORed MM_DECRYPT_CAPS_XXX (see comments)
   ppdwFormats: Pointer to a zero-terminated array of decryption formats supported

Return Value:
   SUCCESS:    Successful
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot get parm in the current state
   EUNSUPPORTED: Decryption not supported by this player

Comments:
   On return, *pdwCaps contains the mask that specifies decryption capabilities available
   for the media. Following values are possible for MM_DECRYPT_CAPS_XXX:

   MM_DECRYPT_CAPS_NONE: Decryption not supported for the media. Calling app must do decryption
                         and pass the media in the clear.

   MM_DECRYPT_CAPS_KEY:  IMedia object accepts key and decrypts media content.

   MM_DECRYPT_CAPS_TXID: IMedia object accepts DRM transaction ID, queries DRM agent for key and decrypts media content. 

   Any media player may define its own capability for passing key information in. The bit
   should be MM_DECRYPT_CAPS_USER_BASE or higher. All capabilities need not be defined by Brew.

   Also on return, *ppdwFormats points to a zero-terminated array of uint32s each denoting a
   decryption format (MM_DECRYPT_FMT_XXX) supported by the media player. The lifetime of the list
   is that of the IMedia object. NOTE: Do not release *ppdwFormats. You may copy the list.

   Following values are defined for MM_DECRYPT_FMT_XXX:

   MM_DECRYPT_FMT_NONE:       Input is not encrypted

   MM_DECRYPT_FMT_DCF:        Supports DCF format and most algorithms used by DCF

   MM_DECRYPT_FMT_AES128CBC:  128 bit AES, CBC mode with RFC 2630 padding

   The input to the media player must be one of the supported formats,
   or no encryption.

   If the media is encrypted in a format not supported by the media player
   the only way the media player can be used is to decrypt the media
   before giving it to the media player.

   Any media player can define new values for different encrypted formats it supports. The
   list defined here is minimal. The define values should be above MM_DECRYPT_FMT_USER_BASE.

   For more details, refer to "Rendering of encrypted media" section in the IMediaDecrypt Interface description.

   Note that IMEDIA_GetDecryptCaps() is just a macro wrapping IMEDIA_GetMediaParm so it is not
   actually an interface change to IMedia. IMEDIA_GetDecryptCaps() can be called on any media
   player old or new.

Side Effects:
   None.

Version:
    Introduced BREW SDK 3.1

See Also:
   IMEDIA_SetDecryptInfo()

=============================================================================

IMEDIA_SetDecryptInfo()

Description:
   This function sets required decryption info necessary to decrypt the media
   content.

Prototype:
   int IMEDIA_SetDecryptInfo(IMedia * po, AEEMediaDecryptInfo * pdi);

Parameters:
   po : Pointer to the IMedia Interface object
   pdi: Specifies the decryption info. See AEEMediaDecryptInfo documentation.

Return Value:
   SUCCESS:    Successful
   EBADPARM:   Bad parm
   EBADSTATE:  Cannot set parm in the current state
   EUNSUPPORTED: Parm set not supported by the class (no decryption)
   EINVALIDFORMAT: Media player does not support this encryption format

Comments:
   It is recommended that app call IMEDIA_GetDecryptCaps() to know the
   decryption capabilities before calling this function. This enables app
   to pass relevant info to IMedia object to decrypt media content.

   For more details, refer to "Rendering of encrypted media" section in the interface description above.

Side Effects:
   None.

Version:
    Introduced BREW SDK 3.1

See Also:
   IMEDIA_GetDecryptCaps()

============================================================================= */
#endif // AEEMEDIADECRYPT_H

