#ifndef AEEDRM_H
#define AEEDRM_H
/*===========================================================================

                    D R M   A G E N T   I N T E R F A C E

                          H E A D E R    F I L E

DESCRIPTION
  This is the header file defining the interface to the Qualcomm
  DRM agent module.
  A typical usage model would be as follows.

  Phase one:
  ----------
    The download agent (e.g. MMS client) retrieves a DRM rights
    object from the network and installs it in the agent's database
    using IDRM_AddRight().
    If the content was delivered unprotected (e.g. as combined delivery)
    the download agent uses IDRM_ProtectContent() to encrypt the content
    data with a unique key before storing it on the device.

  Phase two:
  ----------
    A player accessing DRM-protected content can behave in two ways:
    The simple approach of DRM-enabling an existing player would be
    to just call IDRM_ConsumeRight() with the approriate action on the
    content's URI and then honor the permissions set returned.
    A more involved approach is to call IDRM_LookupRight() for the 
    content's URI, and retrieve the set of rights installed for this
    content to provide a user dialog for rights selection. 

  Rights Maintenance:
  -------------------
    To get a list of all available rights objects an application can
    call IDRM_LookupRight() with a NULL URI.
    Rights can be deleted from the agent's database explicitly via
    the rights handle or per URI (all associated rights get deleted).
    Expired rights are deleted automatically.

  Transaction IDs:
  ----------------
    Transaction IDs are used to correlate between rights lookups and
    rights retrievals. Each unique transaction ID refers to a set of
    rights found for a given URI, so a call to IDRM_LookupRight() 
    returns a transaction ID to be used in subsequent calls to
    IDRM_RetrieveRight() / IDRM_RetrieveRawRight().

  Proprietary / unsupported Rights Formats:
  -----------------------------------------
    The DRM agent supports OMA DRM version 1.0 based rights objects.
    Other formats of rights expression languages (RELs) will be added
    as needed. Furthermore, a player can retrieve the original REL
    for a rights object via IDRM_RetrieveRawRight() for unsupported
    RELs and store parsed information back into the rights object
    via IDRM_UpdateRightsInfo(). This provides a transparent secure
    storage for any type of rights object.
    NOTE: For supported REL versions IDRM_UpdateRightsInfo() is disabled
          as the agent maintains the parsed information itself.


EXTERNALIZED FUNCTIONS

    IDRM_AddRight()               - Add a new rights object
    IDRM_CreateRight              - Used to create Rights Object on the 
                                    fly
    IDRM_ProtectContent()         - Encrypt content from unprotected 
                                    delivery
    IDRM_LookupRight()            - Find rights for given URI
    IDRM_RetrieveRight()          - Retrieve rights data from lookups
    IDRM_RetrieveRawRight()       - Retrieve XML rights data from lookups
    IDRM_UpdateRightsInfo()       - Update rights for non-supported REL's
    IDRM_ConsumeRight()           - Consume a right
    IDRM_UpdateConsumeValue()     - Consume a rights-specific value
    IDRM_DeleteRight()            - Delete a specific rights object
    IDRM_DeleteAssociatedRights() - Delete all rights for a given URI


INITIALIZATION AND SEQUENCING REQUIREMENTS
  Initialization is handled internally and not required by the user.

 Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/



/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEEDRM.h#5 $
$DateTime: 2004/08/05 17:59:07 $ $Author: jayanthm $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/04   jay     Added Rights Handle and modfied consume.
07/13/04   jay     Added AEEDRM_CONSTR_MIN_INTERVAL
07/10/04   jay     Added CONST qualifier
06/16/04   jay     Updated comments
05/20/04   jay     First release
02/26/04   om      Updates from reviews.
02/04/04   om      Created module.
=============================================================================*/



/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "AEE.h"
#include "AEEError.h"
#include "IDRM.bid"  /* AEECLSID_IDRM */


/*===========================================================================

                           PUBLIC DATA TYPES

===========================================================================*/

/* Sizes of rights object fields passed directly */
#define AEEDRM_MAX_VERSION_SIZE        32
#define AEEDRM_MAX_URI_LEN             256
#define AEEDRM_LICENSE_KEY_SIZE        32
#define AEEDRM_ENCRYPTION_BLOCK_SIZE   16
#define AEEDRM_XML_BUFFER_SIZE         1024 /* For creating Rights on the fly
                                               from a template */ 

/*---------------------------------------------------------------------------
  List of supported DRM standards
---------------------------------------------------------------------------*/
typedef enum
{
  AEEDRM_UNKNOWN_VERSION     = 0,
  AEEDRM_OMA_1_0_VERSION     = 1,
  AEEDRM_MAX_VERSION,
  AEEDRM_RESERVED_VERSION    = 0xffffffff
} AEEDRM_VersionEnumType;

/*---------------------------------------------------------------------------
  Definition of permissions 
---------------------------------------------------------------------------*/
typedef enum
{
  AEEDRM_PERM_PLAY            = 0x00000001, /* The content may be played     */
  AEEDRM_PERM_DISPLAY         = 0x00000010, /* The content may be displayed  */
  AEEDRM_PERM_EXECUTE         = 0x00000020, /* The content may be executed   */
  AEEDRM_PERM_PRINT           = 0x00000040, /* The content may be printed    */
  AEEDRM_PERM_DIG_AUDIO_OUT   = 0x00010000, /* Digital audio out is allowed  */
  AEEDRM_PERM_DIG_VIDEO_OUT   = 0x00020000, /* Digital video out is allowed  */
  AEEDRM_PERM_STORE2CPRM      = 0x00040000, /* Store downloaded content to   */
                                            /* CPRM (SD cards)               */
  AEEDRM_PERM_UNKNOWN         = 0x80000000  /* Unknown permission type       */
} AEEDRM_PermissionsEnumType;

/*---------------------------------------------------------------------------
  Definition of constraints 
---------------------------------------------------------------------------*/
typedef enum
{
  AEEDRM_CONSTR_UNPROTECTED   = 0x00000001, /* No restrictions               */
  AEEDRM_CONSTR_PERMANENT     = 0x00000010, /* Permanent license             */
  AEEDRM_CONSTR_COUNT         = 0x00000020, /* Count-limited                 */
  AEEDRM_CONSTR_TIME          = 0x00000040, /* Time-limited                  */
  AEEDRM_CONSTR_INTERVAL      = 0x00000080, /* Duration-limited              */
  AEEDRM_CONSTR_PAY_PER_USE   = 0x00000100, /* Pay every time consumed       */
  AEEDRM_CONSTR_MIN_INTERVAL  = 0x00000200, /* Interval-limited              */
  AEEDRM_CONSTR_UNKNOWN       = 0x80000000  /* Unknown constraint type       */
} AEEDRM_ConstraintsEnumType;

/*---------------------------------------------------------------------------
  Definition of DRM actions
---------------------------------------------------------------------------*/
typedef enum
{
  AEEDRM_ACTION_PLAY        = 0x00000001, /* Play content (e.g. song, video) */
  AEEDRM_ACTION_DISPLAY     = 0x00000010, /* Display content (e.g. picture)  */
  AEEDRM_ACTION_EXECUTE     = 0x00000020, /* Execute content (e.g. game)     */
  AEEDRM_ACTION_PRINT       = 0x00000040, /* Print content (e.g. map)        */
  AEEDRM_ACTION_MAX         = 0xffffffff
} AEEDRM_ActionEnumType;

/*---------------------------------------------------------------------------
  Time representation
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  Year;            /* Year [1980..2100]                            */
  uint32  Month;           /* Month of year [1..12]                        */
  uint32  Day;             /* Day of month [1..31] or day of year [1..366] */
  uint32  Hour;            /* Hour of day [0..23]                          */
  uint32  Minute;          /* Minute of hour [0..59]                       */
  uint32  Second;          /* Second of minute [0..59]                     */
  uint32  DayOfWeek;       /* Day of the week [0..6] Monday..Sunday        */
} AEEDRM_TimeType;

/*---------------------------------------------------------------------------
  Opaque character data
---------------------------------------------------------------------------*/
typedef uint8  AEEDRM_PcdataType;

/*---------------------------------------------------------------------------
  Default numbers are 32-bit unsigned integers
---------------------------------------------------------------------------*/
typedef uint32 AEEDRM_NumberType;

/*---------------------------------------------------------------------------
  Rights Handle Type
---------------------------------------------------------------------------*/
typedef uint32 AEEDRM_RightsHandleType;

/*---------------------------------------------------------------------------
  Transaction ID Type
---------------------------------------------------------------------------*/
typedef uint32 AEEDRM_XactionIdType;

/*---------------------------------------------------------------------------
  License information, only filled in when using IDRM_consume_right()
---------------------------------------------------------------------------*/
typedef struct
{
  struct
  {
    uint32         Size;
    uint8          Value[AEEDRM_LICENSE_KEY_SIZE];
  } KeyInfo;
} AEEDRM_LicenseKeyType;

/* Content context identifier */
typedef  struct
{
  AEEDRM_PcdataType  Version[AEEDRM_MAX_VERSION_SIZE];
  char               URI[AEEDRM_MAX_URI_LEN];
} AEEDRM_ContextType;

typedef struct
{
  AEEDRM_TimeType  Start;
  AEEDRM_TimeType  End;
} AEEDRM_DateTimeType;

/* Constraint Model (used per permission) */
typedef struct
{
  /* Set of valid constraints (from AEEDRM_ConstraintsEnumType) */
  uint32              ConstraintsMask;

  /* Count based  */
  AEEDRM_NumberType   TotalCount;
  AEEDRM_NumberType   RemainingCount;

  /* Content-specific consumption unit */
  AEEDRM_NumberType   TotalValue;
  AEEDRM_NumberType   RemainingValue;

  /* Absolute time based */
  AEEDRM_DateTimeType DateTime;

  /* Relative time based */
  AEEDRM_TimeType     Duration;
  AEEDRM_TimeType     FirstTimeUsed;

  /* Minimum interval based */
  AEEDRM_TimeType     MinInterval;
  AEEDRM_TimeType     LastTimeUsed;

} AEEDRM_ConstraintType;

/*---------------------------------------------------------------------------
  AEEDRM Rights object
---------------------------------------------------------------------------*/
typedef struct
{
  /* Unique rights object identifier */
  AEEDRM_RightsHandleType  RightsHandle;

  /* Version of SECDRM API rights object */
  AEEDRM_VersionEnumType    DrmVersion;

  /* Content context identifier */
  AEEDRM_ContextType        Context;

  /* Permissions for the renderer (from AEEDRM_PermissionEnumType) */
  uint32                    PermissionsMask;

  /* Constraints for the renderer */
  struct
  {
    /* Right to play content */
    AEEDRM_ConstraintType   Play;
        
    /* Right to display content */
    AEEDRM_ConstraintType   Display;
        
    /* Right to execute content */
    AEEDRM_ConstraintType   Execute;
       
    /* Right to print content */
    AEEDRM_ConstraintType   Print;
  
  } Constraints;
  
  /* License information */
  AEEDRM_LicenseKeyType     LicenseKey;

} AEEDRM_RightsObjectType;

/*---------------------------------------------------------------------------
   DRM Rights Template 
---------------------------------------------------------------------------*/
typedef struct
{
  boolean  IsValid;
  uint32   TotalCount;
} AEEDRM_ActionTemplateType;

typedef struct
{
  AEEDRM_ActionTemplateType  Play;
  AEEDRM_ActionTemplateType  Display;
  AEEDRM_ActionTemplateType  Execute;
  AEEDRM_ActionTemplateType  Print;
} AEEDRM_PermissionTemplateType;

/*---------------------------------------------------------------------------
  DRM content encryption
---------------------------------------------------------------------------*/
typedef enum
{
  AEEDRM_ENC_METHOD_NULL         = 0,
  AEEDRM_ENC_METHOD_AES_128_CBC  = 1,
  AEEDRM_ENC_METHOD_MAX,
  AEEDRM_ENC_METHOD_RESERVED     = 0xffffffff
} AEEDRM_EncryptionMethodEnumType;

typedef enum
{
  AEEDRM_ENC_PADDING_NONE        = 0,
  AEEDRM_ENC_PADDING_RFC2630     = 1,
  AEEDRM_ENC_PADDING_MAX
} AEEDRM_PaddingEnumType;

typedef struct
{
  AEEDRM_EncryptionMethodEnumType  Method;
  AEEDRM_PaddingEnumType           Padding;
  uint8                            IV[AEEDRM_ENCRYPTION_BLOCK_SIZE];
} AEEDRM_EncryptionInfoType;

/*---------------------------------------------------------------------------
  DRM Error codes
---------------------------------------------------------------------------*/
#define DRM_ERROR_BASE                (0xE00)
#define AEEDRM_ENOTFOUND              (DRM_ERROR_BASE)
#define AEEDRM_EMOREDATA              (DRM_ERROR_BASE + 0x01)
#define AEEDRM_ERIGHTSEXPIRED         (DRM_ERROR_BASE + 0x02)
#define AEEDRM_ENOTALLOWED            (DRM_ERROR_BASE + 0x03)

/*---------------------------------------------------------------------------
  DRM return value definitions for asynchronous calls
---------------------------------------------------------------------------*/
typedef enum
{
  AEEDRM_RSP_ADD_RIGHT                    = 0,
  AEEDRM_RSP_LOOKUP_RIGHT,
  AEEDRM_RSP_CONSUME_RIGHT,
  AEEDRM_RSP_UPDATE_CONSUME_VALUE,
  AEEDRM_RSP_DELETE_RIGHT,
  AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS,
  AEEDRM_RSP_UPDATE_RIGHTS_INFO,
  AEEDRM_RSP_MAX_TYPES
} AEEDRM_RspEnumType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
  AEEDRM_RightsHandleType  RightsHandle;
  uint32                   ContentURILen;
  char                     szContentURI[AEEDRM_MAX_URI_LEN];
} AEEDRM_AddRightRspType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
  AEEDRM_XactionIdType     XactionID;
  uint32                   NumRights;
} AEEDRM_LookupRightRspType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
} AEEDRM_UpdateRightsInfoRspType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
  AEEDRM_RightsHandleType  RightsHandle;
  AEEDRM_XactionIdType     XactionID;
  AEEDRM_LicenseKeyType    LicenseKey;
} AEEDRM_ConsumeRightRspType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
  AEEDRM_NumberType        RemainingValue;
} AEEDRM_UpdateConsumeValueRspType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
} AEEDRM_DeleteRightRspType;

typedef struct
{
  AEEDRM_RspEnumType       RspType;
  int                      Errno;
} AEEDRM_DeleteAssociatedRightsRspType;

/*===========================================================================

                       IDRM Interface

===========================================================================*/

typedef struct IDRM IDRM;

AEEINTERFACE(IDRM) 
{
  INHERIT_IQueryInterface(IDRM);
  void (*AddRight) (IDRM* pDRM, const char szContentURI[],
                    const AEEDRM_VersionEnumType RELVersion,
                    uint8* pXMLRightsObject, const uint32 XMLRightsObjectSize,
                    AEEDRM_AddRightRspType* const pRsp,
                    AEECallback* pCB );
  int (*CreateRight) ( IDRM* pDRM, const char  szContentURI[],
                       const AEEDRM_PermissionTemplateType PermissionTemplate,
                       uint8* pXMLRightsObject, uint32* pXMLRightsObjectSize);
  int (*ProtectContent) (IDRM* pDRM, const AEEDRM_RightsHandleType RightsHandle,
                          uint8* pContent, const uint32 ContentLen,
                          uint8* pCipherText,uint32* pCipherTextLen,
                          const boolean IsContentComplete,
                          AEEDRM_EncryptionInfoType* const pEncryptionInfo );
  void (*LookupRight) (IDRM* pDRM, const char szContentURI[],
                       const AEEDRM_RightsHandleType RightsHandle,
                       AEEDRM_LookupRightRspType* const pRsp, AEECallback* pCB );
  int  (*RetrieveRight) (IDRM* pDRM, const AEEDRM_XactionIdType XactionID,
                         AEEDRM_RightsObjectType* pRightsList,
                         uint32* pRightsListSize );
  int  (*RetrieveRawRight) (IDRM* pDRM, const AEEDRM_XactionIdType XactionID,
                            uint8* pBuffer, uint32* pBufferSize);
  void (*UpdateRightsInfo) (IDRM* pDRM, const AEEDRM_RightsHandleType RightsHandle,
                            AEEDRM_RightsObjectType* pUpdatedRight,
                            AEEDRM_UpdateRightsInfoRspType* const pRsp,
                            AEECallback* pCB );
  void (*ConsumeRight) (IDRM* pDRM, const char szContentURI[],
                        const AEEDRM_RightsHandleType RightsHandle,
                        const AEEDRM_ActionEnumType Action,
                        AEEDRM_ConsumeRightRspType* const pRsp, AEECallback* pCB );
  void (*UpdateConsumeValue) (IDRM* pDRM, const AEEDRM_RightsHandleType RightsHandle,
                              const AEEDRM_ActionEnumType Action,
                              const AEEDRM_NumberType ValueConsumed,
                              AEEDRM_UpdateConsumeValueRspType* const pRsp,
                              AEECallback* pCB );
  void (*DeleteRight) (IDRM* pDRM, const AEEDRM_RightsHandleType RightsHandle,
                       AEEDRM_DeleteRightRspType* const pRsp, AEECallback* pCB );
  void (*DeleteAssociatedRights) (IDRM* pDRM, const char szContentURI[],
                                  AEEDRM_DeleteAssociatedRightsRspType* const pRsp,
                                  AEECallback* pCB );
};

#define IDRM_AddRef(p)                   AEEGETPVTBL((p),IDRM)->AddRef((p))
#define IDRM_Release(p)                  AEEGETPVTBL((p),IDRM)->Release((p))
#define IDRM_QueryInterface(p, clsid, pp)                                     \
                          AEEGETPVTBL(p,IDRM)->QueryInterface(p, clsid, pp)
  

#define IDRM_AddRight(p,uri,ver,xml,size,rsp,cb)                              \
    AEEGETPVTBL((p),IDRM)->AddRight((p),(uri),(ver),(xml),(size),(rsp),(cb))
    
#define IDRM_CreateRight(p,uri,templ,xml,size)                               \
    AEEGETPVTBL((p),IDRM)->CreateRight((p),(uri),(templ),(xml),(size))

#define IDRM_ProtectContent(                                                  \
         p,hdl,content,cont_len,cipher_txt,ctext_len,end,einfo)               \
   AEEGETPVTBL((p),IDRM)->ProtectContent(                                     \
      (p),(hdl),(content),(cont_len),(cipher_txt),(ctext_len),(end),(einfo))
   
#define IDRM_LookupRight(p,uri,hdl,rsp,cb)                                    \
   AEEGETPVTBL((p),IDRM)->LookupRight((p),(uri),(hdl),(rsp),(cb))
   
#define IDRM_RetrieveRight(p,xid,list,size)                                   \
   AEEGETPVTBL((p),IDRM)->RetrieveRight((p),(xid),(list),(size))
   
#define IDRM_RetrieveRawRight(p,xid,buff,size)                                \
   AEEGETPVTBL((p),IDRM)->RetrieveRawRight((p),(xid),(buff),(size))
   
#define IDRM_UpdateRightsInfo(p,hdl,ro,rsp,cb)                                \
   AEEGETPVTBL((p),IDRM)->UpdateRightsInfo((p),(hdl),(ro),(rsp),(cb))
   
#define IDRM_ConsumeRight(p,uri,hdl,actn,rsp,cb)                              \
   AEEGETPVTBL((p),IDRM)->ConsumeRight((p),(uri),(hdl),(actn),(rsp),(cb))
   
#define IDRM_UpdateConsumeValue(p,hdl,actn,val,rsp,cb)                        \
   AEEGETPVTBL((p),IDRM)->UpdateConsumeValue((p),(hdl),(actn),(val),(rsp),(cb))
   
#define IDRM_DeleteRight(p,hdl,rsp,cb)                                        \
   AEEGETPVTBL((p),IDRM)->DeleteRight((p),(hdl),(rsp),(cb))
   
#define IDRM_DeleteAssociatedRights(p,uri,rsp,cb)                         \
   AEEGETPVTBL((p),IDRM)->DeleteAssociatedRights((p),(uri),(rsp),(cb))




/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION IDRM_ADDRIGHT()

DESCRIPTION
  This interface is used by a download agent to install a received
  DRM rights object in the rights database.

DEPENDENCIES
  None
  
PROTOTYPE
  void IDRM_AddRight
  ( 
    IDRM*                    pDRM,
    char                     szContentURI[],
    AEEDRM_VersionEnumType   RELVersion,
    uint8*                   pXMLRightsObject,
    uint32                   XMLRightsObjectSize,
    AEEDRM_AddRightRspType*  pRsp,
    AEECallback*             pCB
  )

PARAMETERS
  pDRM                 [In]- Pointer to DRM object
  szContentURI         [In]- Content URI of this rights object (optional)
  RELVersion           [In]- Version of REL used (optional)
  pXMLRightsObject     [In]- Pointer to XML representation of rights object
  XMLRightsObjectSize  [In]- Length of XML text stream
  pRsp                 [Out]- Pointer to memory for return data
  pCB                  [In]- Pointer to AEE Callback structure

RETURN VALUE
None

COMMENTS 
The status will be returned as part of the response (pRsp)

  SUCCESS      Successfully added to the rights database.
  EFSFULL      If no space to store the right
  EBADPARM     Invalid parameters
  ENOMEMORY
  EFAILED

szContentURI and  RELVersion are optional: If the rights object is OMA1.0
these fields can be NULL, but if it is not then the URI is mandatory for
storing the rights object. The purpose of RELVersion is TBD.  
If it is OMA1.0 compliant Rights Object, the URI will be returned in the
response. In adition the Rights Handle is also returned in the response.  

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_CREATERIGHT()

DESCRIPTION
  This interface is used to create a rights object on the fly.
  
DEPENDENCIES
  None
  
PROTOTYPE
  int IDRM_CreateRight
  (
   IDRM*                          pDRM,
   char                           szContentURI[],
   AEEDRM_PermissionTemplateType  PermissionTemplate,
   uint8*                         pXMLRightsObject,
   uint32*                        pXMLRightsObjectSize
  )

PARAMETERS
  pDRM                 [In]- Pointer to DRM object
  szContentURI         [In]- Content URI of this rights object
  PermissionTemplate   [In]- List of permissions
  pXMLRightsObject     [Out]- Pointer to the buffer that holds Rights Object (XML)
  pXMLRightsObjectSize [In-Out]- Size of the above buffer which will be updated
                                 to the actual size of the Right
  
RETURN VALUE
  SUCCESS      Successfully created a Rights Object.
  EBADPARM     Invalid parameters
  EFAILED


COMMENTS 
  The rights object created will be OMA1.0 compliant. *pXMLRightsObjectSize
  should be atleast AEEDRM_XML_BUFFER_SIZE
   
SIDE EFFECTS
  None
===========================================================================*/


/*===========================================================================

FUNCTION IDRM_PROTECTCONTENT()

DESCRIPTION
  This function takes un-encrypted raw content and produces the specified
  output format.
  The content is encrypted in place, and the necessary header information
  for the DRM file returned to the caller.

DEPENDENCIES
  The content resides in memory accessible by the DRM agent directly.

PROTOTYPE
int IDRM_ProtectContent
( 
  IDRM*                          pDRM,
  AEEDRM_RightsHandleType        RightsHandle,
  uint8*                         pContent,
  uint32                         ContentLen,
  uint8*                         pCipherText,
  uint32*                        pCipherTextLen,
  boolean                        IsContentComplete,
  AEEDRM_EncryptionInfoType*     pEncryptionInfo
)
  

PARAMETERS
  pDRM                [In]- Pointer to DRM object
  RightsHandle        [In]- Associated rights object
  pContent            [In]- Pointer to the content
  ContentLen          [In]- Size of the content
  pCipherText         [Out]- Pointer to the buffer that holds the cipher text
  pCipherTextLen      [In-Out]- Pointer to the size of the abovee buffer, which
                                will be updated with the actual cipher text 
                                length.
  IsContentComplete   [In]- Boolean to indicate final block of data.                   
  pEncryptionInfo     [In-Out]- Encryption information (cipher, padding, IV, etc.)
  
RETURN VALUE
  SUCCESS           Successfully encrypted the content.
  EUNSUPPORTED      Cipher algorithm, padding type not supported.
  AEEDRM_ENOTFOUND  Rights Object not found.
  EBADPARM          Invalid parameters.
  ENOMEMORY
  EFAILED

COMMENTS
 This API can be called multiple times and the IV is updated at the end of
 each call, which should be fed to the next round of encryption.
 When the final set of data is to be encrypted, IsContentComplete should be set
 to TRUE and the DRM agent internally pads (RFC2630) the content irrespective of 
 whether the input length is AES128 block multiple or not.
 For all intermediate encryption, IsContentComplete should be set to FALSE and
 the input length SHOULD be AES128 block multiple (AEEDRM_ENCRYPTION_BLOCK_SIZE). 

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_LOOKUPRIGHT()

DESCRIPTION
  This function is used by a DRM-enabled application to lookup rights
  for a specific content or find all rights a user has installed.
  The response has the number of rights found and the transaction ID 
  required for retrieving the rights.
  If a callback pointer is given the execution is asynchronous and
  the application can retrieve the rights list by calling 
  IDRM_retrieve_right() upon a callback with a success indication.

DEPENDENCIES
  None

PROTOTYPE

void IDRM_LookupRight
(
  IDRM*                       pDRM,
  char                        szContentURI[],
  AEEDRM_RightsHandleType     RightsHandle(Optional),
  AEEDRM_LookupRightRspType*  pRsp,
  AEECallback*                pCB
)


PARAMETERS
  pDRM          [In]- Pointer to DRM object
  szContentURI  [In]- URI of the content, NULL is a wildcard for all rights
  RightsHandle  [In]- Handle for rights object to find (optional)
  pRsp          [Out]- Pointer to memory for return data
  pCB           [In]- Pointer to AEE Callback structure

RETURN VALUE
None

COMMENTS
The status will be returned as part of the response (pRsp)

  SUCCESS           Successfully lookedup rights.
  AEEDRM_ENOTFOUND  Rights Object not found.
  EBADPARM          Invalid parameters.
  ENOMEMORY
  EFAILED

App can Lookup based on URI or Rights Handle.  
The number of rights found and the transaction ID will be returned in the response.  

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_RETRIEVERIGHT()

DESCRIPTION
  This function is used by a DRM-enabled application to retrieve rights
  information requested with a previous call to IDRM_lookup_right().
  The transaction ID is used to identify the list of rights.
  This function executes synchronously.

DEPENDENCIES
  IDRM_lookup_right() executed successfully and notified the
  application, the transaction ID is the same as the one generated 
  by the IDRM_lookup_right() API call.
  This API can be called iteratively till all the rights are retrieved.
  
PROTOTYPE 
 int IDRM_RetrieveRight
 ( 
   IDRM*                     pDRM,
   AEEDRM_XactionIdType      XactionID,
   AEEDRM_RightsObjectType*  pRightsList,
   uint32*                   pRightsListSize 
 ) 

PARAMETERS
  pDRM                [In]- Pointer to DRM object
  XactionId           [In]- Transaction ID from the lookup action
  pRightsList         [Out]- Pointer to a list of rights, filled in by
                        function
  RightsListSize      [In-Out]- Pointer so the size of above list (in list 
                        elements)
  
RETURN VALUE
  SUCCESS              When all Rights are retrieved.
  AEEDRM_EMOREDATA     More Rights available.
  AEEDRM_ENOTFOUND     XactionID not found, need to call Lookup again.
  EBADPARM             Invalid parameters.
  ENOMEMORY
  EFAILED

COMMENTS
  If this function is called after all rights are retrieved,
  *pRightsListSize = 0 and SUCCESS will be returned.


SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_RETRIEVERAWRIGHT()

DESCRIPTION
  This function is used by a DRM-enabled application to retrieve rights
  information in its original rights expression language requested with 
  a previous call to IDRM_lookup_right().
  The transaction ID is used to identify the list of rights, this function
  executes synchronously.
  The rights data is provided as an iSrc object to read from.

DEPENDENCIES
  IDRM_lookup_right() executed successfull and notified the
  application, the transaction ID is the same as the one generated 
  by the IDRM_lookup_right() API call.

PROTOTYPE
  
  int IDRM_RetrieveRawRight
  (
    IDRM*                 pDRM,
    AEEDRM_XactionIdType  XactionID,
    uint8*                pBuffer,
    uint32*               pBufferSize
  )

PARAMETERS
  pDRM          [In]- Pointer to DRM object
  XactionId     [In]- Transaction ID from the lookup action
  pBuffer       [Out]- Buffer that holds the raw rights
  pBufferSize   [In-Out] - Size of the buffer which will be updated

RETURN VALUE
  SUCCESS              When all bytes are retrieved.
  AEEDRM_EMOREDATA     More data available.
  AEEDRM_ENOTFOUND     XactionID not found, need to call Lookup again.
  EBADPARM             Invalid parameters.
  ENOMEMORY
  EFAILED
  
COMMENTS
  If this function is called after all rights are retrieved,
  *pBufferSize = 0 and SUCCESS will be returned.


SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_CONSUMERIGHT()

DESCRIPTION
  This function is used by a DRM-enabled function to retrieve the
  key information required to decrypt the content. Consumbale rights
  are updated in the rights database.

  NOTE: If both URI and rights_handle are specified they are checked
        for consistency. It is valid to specifiy only one of the two
        to identify a specific rights object.
        The rights handle is assigned by the DRM Agent and known to the
        application from the list of rights returned from
        IDRM_LookupRight().

DEPENDENCIES
  None
  
PROTOTYPE
void IDRM_ConsumeRight
( 
  IDRM*                        pDRM,
  char                         szContentURI[],
  AEEDRM_RightsHandleType      RightsHandle,
  AEEDRM_ActionEnumType        Action,
  AEEDRM_ConsumeRightRspType*  pRsp,
  AEECallback*                 pCB
)

PARAMETERS
  pDRM          [In]- Pointer to DRM object
  szContentURI  [In]- URI of the content
  RightsHandle  [In]- Handle for a specific rights object
  Action        [In]- How to consume the right, e.g. play, print, etc.
  pRsp          [Out]- Pointer to memory for return data
  pCB           [In]- Pointer to AEE Callback structure

RETURN VALUE
None

COMMENTS
The status will be returned as part of the response (pRsp)

  SUCCESS                 If the License Key was returned successfully.
  AEEDRM_ERIGHTSEXPIRED   If the Rights have expired.
  AEEDRM_ENOTFOUND        Rights Object not found.
  EBADPARM                Invalid parameters.
  ENOMEMORY
  EFAILED

Consume can be called based on URI or Rights Handle                
The License Key of the content is returned in the response. In addition
the Rights Handle of the consumed RO is returned.

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_UPDATECONSUMEVALUE()

DESCRIPTION
  This function is used by a DRM-enabled function to update a content-
  specific value in the rights object, e.g. for time-based consumption
  the player would call this function with the value (time) actually 
  consumed. The DRM agent make no assumptions on the units of this field.

  NOTE: Since the consumtpion value might differ for individual methods
        of rendering (unlike a simple usage count), this function has
        to be called with one action and value consumed at a time if
        multiple ways of rendering the content were applied.

DEPENDENCIES
  None

PROTOTYPE  

void IDRM_UpdateConsumeValue
( 
  IDRM*                              pDRM,
  AEEDRM_RightsHandleType            RightsHandle,
  AEEDRM_ActionEnumType              Action,
  AEEDRM_NumberType                  ValueConsumed,
  AEEDRM_UpdateConsumeValueRspType*  pRsp,
  AEECallback*                       pCB
)


PARAMETERS
  pDRM          [In]-Pointer to DRM object
  RightsHandle  [In]- Handle for a specific rights object
  Action        [In]- How to consume the right, e.g. play, print, etc.
  pRsp          [Out]- Pointer to memory for return data
  pCB           [In]- Pointer to AEE Callback structure

RETURN VALUE
None

COMMENTS
The status will be returned as part of the response (pRsp)
  
  SUCCESS                 If the value was updated successfully.
  AEEDRM_ENOTFOUND        Rights Object not found.
  EBADPARM                Invalid parameters.
  ENOMEMORY
  EFAILED


SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_DELETERIGHT()

DESCRIPTION
  This function deletes a specific rights object from the DRM agent's
  database.

DEPENDENCIES
  None
  
PROTOTYPE
void IDRM_DeleteRight
( 
  IDRM*                       pDRM,
  AEEDRM_RightsHandleType     RightsHandle,
  AEEDRM_DeleteRightRspType*  pRsp,
  AEECallback*                pCB
)

PARAMETERS
  pDRM           [In]- Pointer to DRM object
  RightsHandle   [In]- Rights handle identifying the rights object to delete
  pRsp           [Out]- Pointer to memory for return data
  pCB            [In]- Pointer to AEE Callback structure

RETURN VALUE
  None

COMMENTS
The status will be returned as part of the response (pRsp)
  
  SUCCESS                 If the RO was deleted successfully.
  EBADPARM                Invalid parameters.
  ENOMEMORY
  EFAILED

SIDE EFFECTS
  None
===========================================================================*/

/*===========================================================================

FUNCTION IDRM_DELETEASSOCIATEDRIGHTS()

DESCRIPTION
  This function deletes all rights associated with the specified
  content URI from the rights database.

DEPENDENCIES
  None
  
PROTOTYPE 
void IDRM_DeleteAssociatedRights
(
  IDRM*                                  pDRM,
  char                                   szContentURI[],
  AEEDRM_DeleteAssociatedRightsRspType*  pRsp,
  AEECallback*                           pCB 
)
 

PARAMETERS
  pDRM          [In]- Pointer to DRM object
  szContentURI  [In]- URI of the content
  pRsp          [Out]- Pointer to memory for return data
  pCB           [In]- Pointer to AEE Callback structure

RETURN VALUE
  None

COMMENTS
The status will be returned as part of the response (pRsp)
  
  SUCCESS                 If the RO/s was deleted successfully.
  EBADPARM                Invalid parameters.
  ENOMEMORY
  EFAILED
  

SIDE EFFECTS
  None
===========================================================================*/
/*===========================================================================

FUNCTION IDRM_UPDATERIGHTSINFO()

DESCRIPTION
  This function is used by a download agent or player using an REL
  that is not supported by the DRM agent to manipulate constraints
  themselves.

  NOTE: The REL version in the rights object has to be mapped to
        the AEEDRM_UNKNOWN_VERSION, otherwise this function denies
        write access to the constraint fields.

DEPENDENCIES
  None

PROTOTYPE
void IDRM_UpdateRightsInfo
(
  IDRM*                            pDRM,
  AEEDRM_RightsHandleType          RightsHandle,
  AEEDRM_RightsObjectType*         pUpdatedRight,
  AEEDRM_UpdateRightsInfoRspType*  pRsp,
  AEECallback*                     pCB
)
  

PARAMETERS
  pDRM              [In]- Pointer to DRM object
  RightsHandle      [In]- Handle for a specific rights object
  pUpdatedRight     [In]- Pointer to updated rights object
  pRsp              [Out]- Pointer to memory for return data
  pCB               [In]- Pointer to AEE callback info

RETURN VALUE
  None

COMMENTS
The status will be returned as part of the response (pRsp)
  
  SUCCESS                 If the RO was updated successfully.
  AEEDRM_ENOTALLOWED      If the version was not AEEDRM_UNKNOWN_VERSION
  EBADPARM                Invalid parameters.
  ENOMEMORY
  EFAILED
  

SIDE EFFECTS
  None
===========================================================================*/
#endif /* AEEDRM_H */
