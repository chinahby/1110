/*=============================================================================

FILE: OEMDRM.c

SERVICES: OEM Functions for DRM Agent Interface.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

        Copyright c 2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/qsc60x0/apps/StaticExtensions/OEM/Src/OEMDRM.c#1 $
$DateTime: 2006/08/04 11:26:13 $ $Author: chonda $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
08/05/04     aag     Fixed IPC
07/27/04     jay     Modified Consume.
07/13/04     jay     Added async capability.
07/10/04     jay     Added CONST qualifiers.
06/16/04     jay     Added some additional checks to params.
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

#include "msg.h"
#include "err.h"
#include "secerrno.h"
#include "secdrm.ipc"

#include "AEEDRM.h"



/* IDRM object */
struct IDRM
{
  AEEVTBL(IDRM)*        pvt;      
  IShell*               piShell;
  uint32                uRefs; 
  AEECLSID              ucls;  
  void*                 pac;        /* App context */
  AEECallback*          pcb;
};

static uint32 OEMDRM_AddRef(IDRM *pDRM);
static uint32 OEMDRM_Release(IDRM *pDRM);
static int OEMDRM_QueryInterface(IDRM *pDRM, AEECLSID clsid, void **ppNew);

static void OEMDRM_AddRight( IDRM* pDRM,
                             const char szContentURI[],
                             const AEEDRM_VersionEnumType RELVersion,
                             uint8* pXMLRightsObject,
                             const uint32 XMLRightsObjectSize,
                             AEEDRM_AddRightRspType* const pRsp,
                             AEECallback* pCB );

static int OEMDRM_CreateRight(
          IDRM* pDRM,
          const char  szContentURI[],
          const AEEDRM_PermissionTemplateType PermissionTemplate,
          uint8* pXMLRightsObject,
          uint32* pXMLRightsObjectSize);

static int  OEMDRM_ProtectContent(
           IDRM* pDRM,
           const AEEDRM_RightsHandleType RightsHandle,
           uint8*  pContent,
           const uint32  ContentLen,
           uint8*  pCipherText,
           uint32* pCipherTextLen,
           const boolean IsContentComplete,
           AEEDRM_EncryptionInfoType* const pEncryptionInfo );

static void OEMDRM_LookupRight( IDRM* pDRM,
                                const char szContentURI[],
                                const AEEDRM_RightsHandleType RightsHandle,
                                AEEDRM_LookupRightRspType* const pRsp,
                                AEECallback* pCB );

static int  OEMDRM_RetrieveRight( IDRM* pDRM,
                                  const AEEDRM_XactionIdType XactionID,
                                  AEEDRM_RightsObjectType* pRightsList,
                                  uint32* pRightsListSize );

static int  OEMDRM_RetrieveRawRight( IDRM* pDRM,
                                     const AEEDRM_XactionIdType XactionID,
                                     uint8* pBuffer,
                                     uint32* pBufferSize );

static void OEMDRM_UpdateRightsInfo(
           IDRM* pDRM,
           const AEEDRM_RightsHandleType RightsHandle,
           AEEDRM_RightsObjectType* pUpdatedRight,
           AEEDRM_UpdateRightsInfoRspType* const pRsp,
           AEECallback* pCB );

static void OEMDRM_ConsumeRight( 
            IDRM* pDRM,
            const char szContentURI[],
            const AEEDRM_RightsHandleType RightsHandle,
            const AEEDRM_ActionEnumType Action,
            AEEDRM_ConsumeRightRspType* const pRsp,
            AEECallback* pCB );

static void OEMDRM_UpdateConsumeValue( 
             IDRM* pDRM,
             const AEEDRM_RightsHandleType RightsHandle,
             const AEEDRM_ActionEnumType Action,
             const AEEDRM_NumberType ValueConsumed,
             AEEDRM_UpdateConsumeValueRspType* const pRsp,
             AEECallback* pCB );

static void OEMDRM_DeleteRight( IDRM* pDRM,
                                const AEEDRM_RightsHandleType RightsHandle,
                                AEEDRM_DeleteRightRspType* const pRsp,
                                AEECallback* pCB );

static void OEMDRM_DeleteAssociatedRights( 
              IDRM* pDRM,
              const char szContentURI[],
              AEEDRM_DeleteAssociatedRightsRspType* const pRsp,
              AEECallback* pCB );

/* The function supported by DRM object. */
static const AEEVTBL(IDRM) gOEMDRMFuncs = {
   OEMDRM_AddRef,
   OEMDRM_Release,
   OEMDRM_QueryInterface,
   OEMDRM_AddRight,
   OEMDRM_CreateRight,
   OEMDRM_ProtectContent,
   OEMDRM_LookupRight,
   OEMDRM_RetrieveRight,
   OEMDRM_RetrieveRawRight,
   OEMDRM_UpdateRightsInfo,
   OEMDRM_ConsumeRight,
   OEMDRM_UpdateConsumeValue,
   OEMDRM_DeleteRight,
   OEMDRM_DeleteAssociatedRights
};

/*===========================================================================

               INTERNAL FUNCTION DECLARATIONS   

===========================================================================*/
static int DMSSToAEE_Err( secerrno_enum_type nDRMErr );


/*===========================================================================

                  Internal DRM FUNCTION DEFINITIONS   

===========================================================================*/


/*===========================================================================
  FUNCTION     DMSSToAEE_Err
  
  DESCRIPTION
   Converts a DMSS error code to the corresponding AEE error code.

  DEPENDENCIES
     none
===========================================================================*/
static int DMSSToAEE_Err
(
  secerrno_enum_type nDRMErr
)
{
  int   i;
  int   AEE_Err = -1;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Store mapping between DRM Error Codes and AEE Error codes */
  static const struct
  {
    secerrno_enum_type  from; /* DRM Error Code */
    int                 to;   /* AEE Error Code */
  } map[] =
  {
    { E_SUCCESS            , SUCCESS                      },
    { E_FAILURE            , EFAILED                      },
    { E_DATA_INVALID       , EBADPARM                     },
    { E_NO_MEMORY          , ENOMEMORY                    },
    { E_NOT_SUPPORTED      , EUNSUPPORTED                 },
    { E_DATA_TOO_LARGE     , EFSFULL                      },
    { E_NOT_AVAILABLE      , AEEDRM_ENOTFOUND             },
    { E_AGAIN              , AEEDRM_EMOREDATA             },
    { E_DATA_EXPIRED       , AEEDRM_ERIGHTSEXPIRED        },
    { E_NOT_ALLOWED        , AEEDRM_ENOTALLOWED           }
  };
  
  
  for (i = 0; i < ARRAY_SIZE(map); i++)
  {
    if (nDRMErr == map[i].from)
    {
      AEE_Err = map[i].to;
      break;
    }
  }

  if ( AEE_Err == -1 )
  {
    MSG_ERROR("Error %d not mapped", nDRMErr, 0, 0 );
    AEE_Err = EFAILED;
  }
  return AEE_Err;

} /* DMSSToAEE_Err */



/*===========================================================================

                  IDRM FUNCTION DEFINITIONS   

===========================================================================*/

/*=============================================================================
FUNCTION: OEMDRM_New

DESCRIPTION:  
  Allocates a new IDRM object,stores the App context and initializes object data.
  A new IDRM object is created for every ISHELL_CreateInstance().

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
int OEMDRM_New
(
  IShell*   pIShell,
  AEECLSID  cls,
  void**    ppif
)
{
   IDRM *pNew;
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

   if (cls != AEECLSID_IDRM)
   {
     return EUNSUPPORTED;
   }

   // Allocate the object.
   pNew = (IDRM *) AEE_NewClass( (IBaseVtbl*)&gOEMDRMFuncs, sizeof(IDRM) ); 
   if (pNew == NULL)
   {
     return ENOMEMORY;
   }
      
   /* Initialize the values. */
   pNew->piShell = pIShell;
   pNew->ucls = cls;
   pNew->uRefs = 1;

   pNew->pcb = NULL;
   
   pNew->pac = AEE_GetAppContext();

   *ppif = pNew;

   return AEE_SUCCESS;
} /* OEMDRM_New */


/*=============================================================================
FUNCTION: OEMDRM_AddRef

DESCRIPTION:  
  Indicates that one or more object is now pointing to object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMDRM_AddRef
(
  IDRM*  pMe
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (pMe)
  {
    return (++pMe->uRefs);
  } 
  return 0; 
}/* OEMDRM_AddRef */


/*=============================================================================
FUNCTION: OEMDRM_Release

DESCRIPTION:  
  Decreases the number of references to the IDRM object.  If the
  last reference is removed, it deallocates the object.

DEPENDENCIES:
  None

SIDE EFFECTS:
  None

=============================================================================*/
static uint32 OEMDRM_Release
(
  IDRM*  pMe
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
} /* OEMDRM_Release */


/*===========================================================================

FUNCTION: OEMDRM_QueryInterface

DESCRIPTION
  Allows applications to query if IDRM supports other interfaces and then
  returns a pointer to that new interface supported via IDRM.  Currently,
  we do not support any other interface.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMDRM_QueryInterface
(
  IDRM*     pMe,
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
    case AEECLSID_IDRM:
    case AEECLSID_QUERYINTERFACE:
      if ( OEMDRM_AddRef(pMe) )
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
} /* OEMDRM_QueryInterface */


/*===========================================================================

FUNCTION: OEMDRM_AddRight

DESCRIPTION
  This interface is used by a download agent to install a received
  DRM rights object in the rights database.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/

static void OEMDRM_AddRight
( 
  IDRM*                          pDRM,
  const char                     szContentURI[],
  const AEEDRM_VersionEnumType   RELVersion,
  uint8*                         pXMLRightsObject,
  const uint32                   XMLRightsObjectSize,
  AEEDRM_AddRightRspType* const  pRsp,
  AEECallback*                   pCB
)
{
  secdrm_add_right_msg*    ipc_req_ptr;
  secdrm_right_added_msg*  ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
   
  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET( pRsp, 0, sizeof( AEEDRM_AddRightRspType ) );
  pRsp->RspType = AEEDRM_RSP_ADD_RIGHT;

  if ( ( XMLRightsObjectSize == 0 )
       ||
       ( pXMLRightsObject == NULL )
     )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_sized_new( secdrm_add_right_msg, 
                               XMLRightsObjectSize,
                               IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  
  if ( szContentURI )
  {
    ipc_req_ptr->content_uri_len = MIN( STRLEN( (char*) szContentURI ),
                                        AEEDRM_MAX_URI_LEN );
    MEMCPY( ipc_req_ptr->content_uri, 
            szContentURI, 
            ipc_req_ptr->content_uri_len );
  }
  else
  {
    ipc_req_ptr->content_uri_len = 0;
  }
  
  ipc_req_ptr->rel_version = RELVersion;
  
  ipc_req_ptr->xml_rights_object_size = XMLRightsObjectSize;
  
  MEMCPY( ipc_req_ptr->xml_rights_object,
          pXMLRightsObject,
          XMLRightsObjectSize );

  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;

  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_right_added_msg*) ipc_receive( 
                                          secdrm_right_added_msg_id ); 
    
    /* Check the result sent back and return it */
    if ( ipc_rsp_ptr->secerrno == E_SUCCESS )
    {
      pRsp->RightsHandle = ipc_rsp_ptr->rights_handle;
      
      
      pRsp->ContentURILen = ipc_rsp_ptr->content_uri_len;
      MEMCPY( pRsp->szContentURI,
              ipc_rsp_ptr->content_uri,
              pRsp->ContentURILen );

    }
    
    pRsp->Errno = DMSSToAEE_Err( ipc_rsp_ptr->secerrno );
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }
} /* OEMDRM_AddRight */


/*===========================================================================

FUNCTION: OEMDRM_CreateRight

DESCRIPTION
  This interface is used to create a rights object on the fly.

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS,
  ENOMEMORY,
  EFAILED,
  EBADPARM
  
SIDE EFFECTS
===========================================================================*/
static int OEMDRM_CreateRight
(
  IDRM*                                pDRM,
  const char                           szContentURI[],
  const AEEDRM_PermissionTemplateType  PermissionTemplate,
  uint8*                               pXMLRightsObject,
  uint32*                              pXMLRightsObjectSize
)
{
  char     count_buf[20];
  int      Errno = SUCCESS;
  uint32   TmpBufSize = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Sanity check on the parameters */
  if ( pDRM == NULL
       ||
       szContentURI == NULL
       ||
       STRLEN( szContentURI ) == 0
       ||
       STRLEN( szContentURI ) > AEEDRM_MAX_URI_LEN
       ||
       pXMLRightsObject == NULL
       ||
       *pXMLRightsObjectSize == 0
       ||
       *pXMLRightsObjectSize < AEEDRM_XML_BUFFER_SIZE
     )
  {
    return EBADPARM;
  }

  /* Copy the buffer size */
  TmpBufSize = *pXMLRightsObjectSize;

  /* Null terminate the string */
  *pXMLRightsObject = 0;

  do
  {
  
    /* Concatenate the strings */
    /* Rights Tag */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     "<o-ex:rights xmlns:o-ex=\"http://odrl.net/1.1/ODRL-EX\" xmlns:o-dd=\"http://odrl.net/1.1/ODRL-DD\" xmlns:ds=\"http://www.w3.org/2000/09/xmldsig#/\">",
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }

    /* Context Tag */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     "<o-ex:context><o-dd:version>1.0</o-dd:version></o-ex:context>",
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }

    /* Start Agreement/Asset Tag */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     "<o-ex:agreement><o-ex:asset><o-ex:context><o-dd:uid>cid:",
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }
    
    /* URI */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     szContentURI,
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }

    /* End Asset Tag */
    Errno =  STRLCAT( (char*)pXMLRightsObject,
                      "</o-dd:uid></o-ex:context></o-ex:asset>",
                      TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }
    
    /* Start Permission Tag */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     "<o-ex:permission>",
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }
    
    /* PLAY */
    if ( PermissionTemplate.Play.IsValid == TRUE )
    {
      if ( PermissionTemplate.Play.TotalCount > 0 )
      {
        /* Start Play Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:play><o-ex:constraint><o-dd:count>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }

        /* Convert integer to string  */
        /* Assumption: The value is not more than XXXX */
        SPRINTF( count_buf, "%ld", PermissionTemplate.Play.TotalCount ); 

        /* Count */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         count_buf,
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
        
        /* End Play Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject, 
                         "</o-dd:count></o-ex:constraint></o-dd:play>",
                          TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
      else
      {
        /* Start - End Play Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:play />",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
    }

    /* DISPLAY */
    if ( PermissionTemplate.Display.IsValid == TRUE )
    {
      if ( PermissionTemplate.Display.TotalCount > 0 )
      {
        /* Start Display Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:display><o-ex:constraint><o-dd:count>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      
        /* Convert integer to string  */
        /* Assumption: The value is not more than XXXX */
        SPRINTF( count_buf, "%ld", PermissionTemplate.Display.TotalCount ); 

        /* Count */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         count_buf,
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
        
        /* End Display Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject, 
                         "</o-dd:count></o-ex:constraint></o-dd:display>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
      else
      {
        /* Start - End Display Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:display />",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
    }

    /* EXECUTE */
    if ( PermissionTemplate.Execute.IsValid == TRUE )
    {
      if ( PermissionTemplate.Execute.TotalCount > 0 )
      {
        /* Start Execute Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:execute><o-ex:constraint><o-dd:count>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }

        /* Convert integer to string  */
        /* Assumption: The value is not more than XXXX */
        SPRINTF( count_buf, "%ld", PermissionTemplate.Execute.TotalCount ); 

        /* Count */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         count_buf,
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
        
        /* End Execute Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject, 
                         "</o-dd:count></o-ex:constraint></o-dd:execute>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
      else
      {
        /* Start - End Execute Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:execute />",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
    }

    /* PRINT */
    if ( PermissionTemplate.Print.IsValid == TRUE )
    {
      if ( PermissionTemplate.Print.TotalCount > 0 )
      {
        /* Start Print Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:print><o-ex:constraint><o-dd:count>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      
        /* Convert integer to string  */
        /* Assumption: The value is not more than XXXX */
        SPRINTF( count_buf, "%ld", PermissionTemplate.Print.TotalCount ); 

        /* Count */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         count_buf,
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
        
        /* End Execute Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject, 
                         "</o-dd:count></o-ex:constraint></o-dd:print>",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
      else
      {
        /* Start - End Print Tag */
        Errno = STRLCAT( (char*)pXMLRightsObject,
                         "<o-dd:print />",
                         TmpBufSize );
        if ( Errno == -1 )
        {
          Errno = EFAILED;
          break;
        }
      }
    }
    
    /* End Permission Tag */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     "</o-ex:permission>",
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }
    
    /* End Agreement/Rights Tag */
    Errno = STRLCAT( (char*)pXMLRightsObject,
                     "</o-ex:agreement></o-ex:rights>",
                     TmpBufSize );
    if ( Errno == -1 )
    {
      Errno = EFAILED;
      break;
    }
    
    /* Determine the Total length */
    *pXMLRightsObjectSize = STRLEN( (char*)pXMLRightsObject );
  
  } while( 0 );

  if ( Errno != EFAILED )
  {
    /* Because STRLEN returns length of concat string if successful */
    Errno = SUCCESS;
  }
  return Errno;

} /* OEMDRM_CreateRight */



/*===========================================================================

FUNCTION: OEMDRM_ProtectContent

DESCRIPTION
  This function takes un-encrypted content and produces the specified
  output format.
  The content is encrypted in place.

DEPENDENCIES
  The content resides in memory accessible by the DRM agent directly.

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMDRM_ProtectContent
( 
  IDRM*                                pDRM,
  const AEEDRM_RightsHandleType        RightsHandle,
  uint8*                               pContent,
  const uint32                         ContentLen,
  uint8*                               pCipherText,
  uint32*                              pCipherTextLen,
  const boolean                        IsContentComplete,
  AEEDRM_EncryptionInfoType* const     pEncryptionInfo
)
{
  secdrm_protect_content_msg*    ipc_req_ptr;
  secdrm_content_protected_msg*  ipc_rsp_ptr;
  secerrno_enum_type             _errno;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Sanity check on the parameters */
  if ( pDRM == NULL
       ||
       (RightsHandle == 0x0)
       ||
       (pContent == NULL)
       ||
       (ContentLen == 0)
       ||
       (pCipherText == NULL)
       ||
       (*pCipherTextLen == 0 )
       ||
       (pEncryptionInfo == NULL)
     )
  {
    return EBADPARM;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_protect_content_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    return ENOMEMORY;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  ipc_req_ptr->rights_handle = RightsHandle;
  ipc_req_ptr->content_shmem_ptr = pContent;
  ipc_req_ptr->content_len = ContentLen;
  ipc_req_ptr->cipher_text_shmem_ptr = pCipherText;
  ipc_req_ptr->cipher_text_len = *pCipherTextLen;
  MEMCPY( &(ipc_req_ptr->encryption_info),
          pEncryptionInfo,
          sizeof( ipc_req_ptr->encryption_info ) );
  
  ipc_req_ptr->is_content_complete = IsContentComplete;
  
  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  /* Wait for the response (this call is synchronous) */
  ipc_rsp_ptr = (secdrm_content_protected_msg*) ipc_receive(
                                       secdrm_content_protected_msg_id );
    
  /* Check the result sent back and return it */
  if ( ipc_rsp_ptr->secerrno == E_SUCCESS )
  {
    MEMCPY( pEncryptionInfo,
            &(ipc_rsp_ptr->encryption_info),
            sizeof( ipc_rsp_ptr->encryption_info ) );
    *pCipherTextLen = ipc_rsp_ptr->cipher_text_len;
  }
  
  _errno =  ipc_rsp_ptr->secerrno;

  /* Delete the response message */
  ipc_delete( &ipc_rsp_ptr );
 
  return DMSSToAEE_Err( _errno );
} /* OEMDRM_ProtectContent */


/*===========================================================================

FUNCTION: OEMDRM_LookupRight

DESCRIPTION
  This interface is used by a DRM-enabled application to lookup rights
  for a specific content.
  If a callback pointer is given the execution is asynchronous and
  the application can retrieve the rights list by calling 
  secdrm_retrieve_right() upon a callback with a success indication.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMDRM_LookupRight
(
  IDRM*                             pDRM,
  const char                        szContentURI[],
  const AEEDRM_RightsHandleType     RightsHandle,
  AEEDRM_LookupRightRspType* const  pRsp,
  AEECallback*                      pCB
)
{
  secdrm_lookup_right_msg*    ipc_req_ptr;
  secdrm_rights_found_msg*    ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET(pRsp, 0, sizeof( AEEDRM_LookupRightRspType ) );
  pRsp->RspType = AEEDRM_RSP_LOOKUP_RIGHT;

  if ( ( szContentURI == NULL )
       &&
       ( RightsHandle == 0 )
     )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_lookup_right_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  ipc_req_ptr->rights_handle = RightsHandle;
  
  if ( szContentURI )
  {
    ipc_req_ptr->content_uri_len = MIN( STRLEN( (char*) szContentURI ),
                                        AEEDRM_MAX_URI_LEN );
    MEMCPY( ipc_req_ptr->content_uri, 
            szContentURI, 
            ipc_req_ptr->content_uri_len );
  }
  else
  {
    ipc_req_ptr->content_uri_len = 0;
  }

  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;

  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_rights_found_msg*) ipc_receive(
                                  secdrm_rights_found_msg_id );
    
    /* Check the result sent back and return it */
    if ( ipc_rsp_ptr->secerrno == E_SUCCESS )
    {
      pRsp->XactionID = ipc_rsp_ptr->xaction_id;
      pRsp->NumRights = ipc_rsp_ptr->num_rights_found;
    }
    pRsp->Errno = DMSSToAEE_Err( ipc_rsp_ptr->secerrno );
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }

} /* OEMDRM_LookupRight */


/*===========================================================================

FUNCTION: OEMDRM_RetrieveRight

DESCRIPTION
  This interface is used by a download agent to install a received
  DRM rights object in the rights database.

DEPENDENCIES
  lookup_right() executed successfully and notified the application, the
  transaction ID is the same as the one generated by the lookup_right() call.

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
static int OEMDRM_RetrieveRight
( 
  IDRM*                           pDRM,
  const AEEDRM_XactionIdType      XactionID,
  AEEDRM_RightsObjectType*        pRightsList,
  uint32*                         pRightsListSize 
)
{
  secerrno_enum_type           _errno;
  secdrm_retrieve_right_msg*   ipc_req_ptr = NULL;
  secdrm_rights_data_msg*      ipc_rsp_ptr = NULL;
  uint32                       list_size;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( !pDRM )
  {
    MSG_ERROR( "OEMDRM: NULL pointer", 0, 0, 0 );
    return EBADPARM;
  }

  /* Sanity check on the parameters */
  if ( (XactionID == 0)
       ||
       (pRightsList == NULL)
       ||
       (pRightsListSize == NULL)
       ||
       (*pRightsListSize == 0)
     )  
  {
    return EBADPARM;
  }

  /* Copy the size locally */
  list_size = *pRightsListSize;

  *pRightsListSize = 0;

  while( list_size > 0 )
  {
    /* Allocate an IPC message to send the request */  
    ipc_req_ptr = ipc_new( secdrm_retrieve_right_msg, 
                           IPC_MSG_POOL_PUBLIC );
    if ( ipc_req_ptr == NULL )
    {
      return ENOMEMORY;
    }

    /* Set up the individual message fields of the request */
    ipc_req_ptr->cookie = 0x00;
    ipc_req_ptr->xaction_id = XactionID;

    /* DRM SVC can receive a MAX of SECDRMIPC_RIGHTS_LIST_SIZE rights at a time */
    ipc_req_ptr->rights_list_size = 
                              MIN( list_size, SECDRMIPC_RIGHTS_LIST_SIZE );

    /* Send the request to the DRM service */
    ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_rights_data_msg*) ipc_receive(
                                         secdrm_rights_data_msg_id );
    _errno = ipc_rsp_ptr->secerrno;
  
    /* Check the result sent back */
    switch ( _errno )
    {
      case E_AGAIN:
      case E_SUCCESS:
        /* Copy the rights */
        MEMCPY( pRightsList,
                &(ipc_rsp_ptr->rights_list),
                sizeof( AEEDRM_RightsObjectType ) *
                ipc_rsp_ptr->rights_list_size );
        
        /* Update the size pointer */
        *pRightsListSize += ipc_rsp_ptr->rights_list_size;

        /* Update the rights list pointer */
        pRightsList += ipc_rsp_ptr->rights_list_size;
        
        /* Update the list size  */
        list_size -= ipc_rsp_ptr->rights_list_size;
        break;

      case E_NOT_AVAILABLE:
        /* Xaction ID not found, need to call lookup again */
        break;

      default:
        /* general error */
        break;
    }
  
    /* break out of the while loop */
    if ( _errno != E_AGAIN ) break;
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  
  } /* while */

  
  if ( ipc_rsp_ptr != NULL )
  {
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }

  return DMSSToAEE_Err( _errno );

} /* OEMDRM_RetrieveRight */


/*===========================================================================

FUNCTION: OEMDRM_RetrieveRawRight

DESCRIPTION
  This function is used by a DRM-enabled application to retrieve rights
  information represented in its original REL format requested with a 
  previous call to secdrm_lookup_right().
  The transaction ID is used to identify the list of rights.
  
DEPENDENCIES
  lookup_right() executed successfully and notified the application, the
  transaction ID is the same as the one generated by the lookup_right() call.

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static int OEMDRM_RetrieveRawRight
(
  IDRM*                       pDRM,
  const AEEDRM_XactionIdType  XactionID,
  uint8*                      pBuffer,
  uint32*                     pBufferSize
)
{
  secerrno_enum_type              _errno;
  secdrm_retrieve_raw_right_msg*  ipc_req_ptr = NULL;
  secdrm_raw_rights_data_msg*     ipc_rsp_ptr = NULL;
  uint32                          num_bytes;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if ( !pDRM )
  {
    MSG_ERROR( "OEMDRM: NULL pointer", 0, 0, 0 );
    return EBADPARM;
  }

  /* Sanity check on the parameters */
  if ( (XactionID == 0)
       ||
       (pBuffer == NULL)
       ||
       (pBufferSize == NULL)
       ||
       (*pBufferSize == 0)
     )  
  {
    return EBADPARM;
  }

  /* Copy the size locally */
  num_bytes = *pBufferSize;

  *pBufferSize = 0;

  while( num_bytes > 0 )
  {
    /* Allocate an IPC message to send the request */  
    ipc_req_ptr = ipc_new( secdrm_retrieve_raw_right_msg, 
                           IPC_MSG_POOL_PUBLIC );
    if ( ipc_req_ptr == NULL )
    {
      return E_NO_MEMORY;
    }

    /* Set up the individual message fields of the request */
    ipc_req_ptr->cookie = 0x00;
    ipc_req_ptr->xaction_id = XactionID;

    /* DRM SVC can receive a MAX of SECDRMIPC_MAX_STREAM_SIZE bytes at a time */
    ipc_req_ptr->num_bytes_requested = 
                              MIN( num_bytes, SECDRMIPC_MAX_STREAM_SIZE );

    /* Send the request to the SEC service */
    ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_raw_rights_data_msg*) ipc_receive(
                                        secdrm_raw_rights_data_msg_id );
    _errno = ipc_rsp_ptr->secerrno;
    
    /* Check the result sent back */
    switch ( _errno )
    {
      case E_AGAIN:
      case E_SUCCESS:
        /* Copy the bytes */
        MEMCPY( pBuffer,
                &(ipc_rsp_ptr->rel_stream),
                ipc_rsp_ptr->rel_stream_size );

        /* Update the size pointer */
        *pBufferSize += ipc_rsp_ptr->rel_stream_size;

        /* Update the rights stream pointer */
        pBuffer += ipc_rsp_ptr->rel_stream_size;
        
        /* update the byte count */
        num_bytes -= ipc_rsp_ptr->rel_stream_size;
        break;

      case E_NOT_AVAILABLE:
        /* Xaction ID not found, need to call lookup again */
        break;

      default:
        /* general error */
        break;
    }

    /* break out of the while loop */
    if ( _errno != E_AGAIN ) break;
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  
  } /* while */

  if ( ipc_rsp_ptr != NULL )
  {
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }
  
  return DMSSToAEE_Err( _errno );

} /* OEMDRM_RetrieveRawRight */


/*===========================================================================

FUNCTION: OEMDRM_UpdateRightsInfo

DESCRIPTION
  This interface is used by a download agent to install a received
  DRM rights object in the rights database.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMDRM_UpdateRightsInfo
(
  IDRM*                                  pDRM,
  const AEEDRM_RightsHandleType          RightsHandle,
  AEEDRM_RightsObjectType*               pUpdatedRight,
  AEEDRM_UpdateRightsInfoRspType* const  pRsp,
  AEECallback*                           pCB
)
{
  secdrm_update_rights_info_msg*   ipc_req_ptr;
  secdrm_rights_info_updated_msg*  ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET( pRsp,
          0,
          sizeof( AEEDRM_UpdateRightsInfoRspType ) );
  pRsp->RspType = AEEDRM_RSP_UPDATE_RIGHTS_INFO;

  if ( (RightsHandle == 0)
       ||
       (pUpdatedRight == NULL) 
       ||
       (RightsHandle !=  pUpdatedRight->RightsHandle) 
     )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_update_rights_info_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  ipc_req_ptr->rights_handle = RightsHandle;
  MEMCPY( &(ipc_req_ptr->rights_object),
          pUpdatedRight,
          sizeof( AEEDRM_RightsObjectType ) );
  
  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;

  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_rights_info_updated_msg*) ipc_receive(
                             secdrm_rights_info_updated_msg_id );
    
    /* Check the result sent back and return it */
    pRsp->Errno = DMSSToAEE_Err( ipc_rsp_ptr->secerrno );
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }
} /* OEMDRM_UpdateRightsInfo */


/*===========================================================================

FUNCTION: OEMDRM_ConsumeRight

DESCRIPTION
  This interface is used by a DRM-enabled player to retrieve the
  license key required to decrypt the content. Consumable rights
  are updated in the rights database.
  
  NOTE:  URI is used when consume is called without prior calls
         to lookup and retrieve
         Rights handle is used when the player knows the rights
         handle through prior calls to lookup and retrieve                           
         
         If both URI and rights_handle are specified they are checked
         for consistency. It is valid to specifiy only one of the two
         to identify a specific rights object.
         The rights handle is assigned by the DRM Agent and known to the
         application when the rights are added.
          
DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMDRM_ConsumeRight
( 
  IDRM*                              pDRM,
  const char                         szContentURI[],
  const AEEDRM_RightsHandleType      RightsHandle,
  const AEEDRM_ActionEnumType        Action,
  AEEDRM_ConsumeRightRspType* const  pRsp,
  AEECallback*                       pCB
)
{
  secdrm_consume_right_msg*      ipc_req_ptr;
  secdrm_right_consumption_msg*  ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET(pRsp, 0, sizeof( AEEDRM_ConsumeRightRspType ) );
  pRsp->RspType = AEEDRM_RSP_CONSUME_RIGHT;
  
  if ( ( (szContentURI == NULL)
         &&
         (RightsHandle == 0)
       )
       ||
       (Action == 0)
     )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_consume_right_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  if ( szContentURI )
  {
    ipc_req_ptr->content_uri_len = MIN( STRLEN( (char*) szContentURI ),
                                        AEEDRM_MAX_URI_LEN );
    MEMCPY( ipc_req_ptr->content_uri, 
            szContentURI, 
            ipc_req_ptr->content_uri_len );
  }
  else
  {
    ipc_req_ptr->content_uri_len = 0;
  }
  ipc_req_ptr->action  = Action;
  ipc_req_ptr->rights_handle = RightsHandle;

  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;
  
  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
   
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_right_consumption_msg*) ipc_receive( 
                                       secdrm_right_consumption_msg_id );
    
    /* Check the result sent back and return it */
    if ( ipc_rsp_ptr->secerrno == E_SUCCESS )
    {
      MEMCPY( &(pRsp->LicenseKey),
              &(ipc_rsp_ptr->license),
              sizeof( AEEDRM_LicenseKeyType ) );
    }
    pRsp->RightsHandle    = ipc_rsp_ptr->rights_handle;
    pRsp->XactionID       = ipc_rsp_ptr->xaction_id;
    pRsp->Errno           = DMSSToAEE_Err( ipc_rsp_ptr->secerrno );
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }

} /* OEMDRM_ConsumeRight */


/*===========================================================================

FUNCTION: OEMDRM_UpdateConsumeValue

DESCRIPTION
  This interface is used by a download agent to install a received
  DRM rights object in the rights database.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMDRM_UpdateConsumeValue
( 
  IDRM*                                    pDRM,
  const AEEDRM_RightsHandleType            RightsHandle,
  const AEEDRM_ActionEnumType              Action,
  const AEEDRM_NumberType                  ValueConsumed,
  AEEDRM_UpdateConsumeValueRspType* const  pRsp,
  AEECallback*                             pCB
)
{
  secdrm_consumption_value_msg*          ipc_req_ptr;
  secdrm_consumption_value_applied_msg*  ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET( pRsp,
          0,
          sizeof( AEEDRM_UpdateConsumeValueRspType ) );
  pRsp->RspType = AEEDRM_RSP_UPDATE_CONSUME_VALUE;

  if ( (RightsHandle == 0)
       ||
       (Action == 0)
       ||
       (ValueConsumed == 0) 
     )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_consumption_value_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  ipc_req_ptr->rights_handle = RightsHandle;
  ipc_req_ptr->action = Action;
  ipc_req_ptr->value_consumed = ValueConsumed;

  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;

  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_consumption_value_applied_msg*) ipc_receive(
                             secdrm_consumption_value_applied_msg_id );
    
    /* Check the result sent back and return it */
    if ( ipc_rsp_ptr->secerrno == E_SUCCESS )
    {
      pRsp->RemainingValue = ipc_rsp_ptr->remaining_value;
    }

    pRsp->Errno = DMSSToAEE_Err(ipc_rsp_ptr->secerrno);
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }

} /* OEMDRM_UpdateConsumeValue */


/*===========================================================================

FUNCTION: OEMDRM_DeleteRight

DESCRIPTION
  This interface is used to remove  a received DRM rights object in the
  rights database.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMDRM_DeleteRight
( 
  IDRM*                             pDRM,
  const AEEDRM_RightsHandleType     RightsHandle,
  AEEDRM_DeleteRightRspType* const  pRsp,
  AEECallback*                      pCB
)
{
  secdrm_delete_right_msg*   ipc_req_ptr;
  secdrm_right_deleted_msg*  ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET(pRsp, 0, sizeof( AEEDRM_DeleteRightRspType ) );
  pRsp->RspType = AEEDRM_RSP_DELETE_RIGHT;

  if ( RightsHandle == 0 )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_delete_right_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  ipc_req_ptr->rights_handle = RightsHandle;

  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;

  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_right_deleted_msg*) ipc_receive( 
                                    secdrm_right_deleted_msg_id );
    
    /* Check the result sent back and return it */
    pRsp->Errno = DMSSToAEE_Err(ipc_rsp_ptr->secerrno);
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }

}/* OEMDRM_DeleteRight */


/*===========================================================================

FUNCTION: OEMDRM_DeleteAssociatedRights

DESCRIPTION
  This interface deletes all rights associated with the specified
  content URI from the rights database.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
===========================================================================*/
static void OEMDRM_DeleteAssociatedRights
(
  IDRM*                                        pDRM,
  const char                                   szContentURI[],
  AEEDRM_DeleteAssociatedRightsRspType* const  pRsp,
  AEECallback*                                 pCB 
)
{
  secdrm_delete_associated_rights_msg*   ipc_req_ptr;
  secdrm_associated_rights_deleted_msg*  ipc_rsp_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Sanity check on the parameters */
  if ( !pDRM || !pRsp )
  {
    MSG_ERROR( "OEMDRM: NULL pointers", 0, 0, 0 );
    return;
  }
  
  /* Clear the memory */
  MEMSET( pRsp,
          0,
          sizeof( AEEDRM_DeleteAssociatedRightsRspType ) );
  pRsp->RspType = AEEDRM_RSP_DELETE_ASSOCIATED_RIGHTS;
  
  if ( szContentURI == NULL )
  {
    pRsp->Errno = EBADPARM;
    return;
  }

  /* Allocate an IPC message to send the request */  
  ipc_req_ptr = ipc_new( secdrm_delete_associated_rights_msg, 
                         IPC_MSG_POOL_PUBLIC );
  if ( ipc_req_ptr == NULL )
  {
    pRsp->Errno = ENOMEMORY;
    return;
  }

  /* Set up the individual message fields of the request */
  ipc_req_ptr->cookie = 0x00;
  ipc_req_ptr->content_uri_len = MIN( STRLEN( (char*) szContentURI ),
                                      AEEDRM_MAX_URI_LEN );
  MEMCPY( ipc_req_ptr->content_uri, 
          szContentURI, 
          ipc_req_ptr->content_uri_len );

  /* Set up the callback related data */
  ipc_req_ptr->usr_data[0] = (uint32) pDRM;
  ipc_req_ptr->usr_data[1] = (uint32) pRsp;
  ipc_req_ptr->usr_data[2] = (uint32) pCB;

  /* Send the request to the SEC service */
  ipc_send( &ipc_req_ptr, IPCSVC_SECDRM );

  if ( pCB == NULL )
  {
    /* Wait for the response (this call is synchronous) */
    ipc_rsp_ptr = (secdrm_associated_rights_deleted_msg*) ipc_receive( 
                               secdrm_associated_rights_deleted_msg_id );
    
    /* Check the result sent back and return it */
    pRsp->Errno = DMSSToAEE_Err(ipc_rsp_ptr->secerrno);
    
    /* Delete the response message */
    ipc_delete( &ipc_rsp_ptr );
  }

} /* OEMDRM_DeleteAssociatedRights */



/*===========================================================================

FUNCTION OEMDRM_ProcessRightAddedMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous addition of rights.

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessRightAddedMsg
(
  secdrm_right_added_msg**  msg_ptr
)
{
  IDRM*                    pDRM = NULL;
  AEEDRM_AddRightRspType*  pRsp = NULL;
  AEECallback*             pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_AddRightRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Add: pointers corrupted", 0,0,0);
    return;
  }

  /* Check the result sent back and return it */
  if ( (*msg_ptr)->secerrno == E_SUCCESS )
  {
    pRsp->RightsHandle = (*msg_ptr)->rights_handle;
    
    pRsp->ContentURILen = (*msg_ptr)->content_uri_len;
    MEMCPY( pRsp->szContentURI,
            (*msg_ptr)->content_uri,
            pRsp->ContentURILen );
  }
  
  pRsp->Errno = DMSSToAEE_Err( (*msg_ptr)->secerrno );
    
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessRightAddedMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessRightsFoundMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous lookup of rights for a given URI.

EPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessRightsFoundMsg
(
  secdrm_rights_found_msg**  msg_ptr
)
{
  IDRM*                       pDRM = NULL;
  AEEDRM_LookupRightRspType*  pRsp = NULL;
  AEECallback*                pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_LookupRightRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Lookup: pointers corrupted", 0,0,0);
    return;
  }

  /* Check the result sent back and return it */
  if ( (*msg_ptr)->secerrno == E_SUCCESS )
  {
    pRsp->XactionID = (*msg_ptr)->xaction_id;
    pRsp->NumRights = (*msg_ptr)->num_rights_found;
  }

  pRsp->Errno = DMSSToAEE_Err( (*msg_ptr)->secerrno );
    
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessRightsFoundMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessRightsInfoUpdatedMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous updating of rights info.

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessRightsInfoUpdatedMsg
(
  secdrm_rights_info_updated_msg**  msg_ptr
)
{
  IDRM*                            pDRM = NULL;
  AEEDRM_UpdateRightsInfoRspType*  pRsp = NULL;
  AEECallback*                     pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_UpdateRightsInfoRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Update RightsInfo: pointers corrupted", 0,0,0);
    return;
  }

  /* Check the result sent back and return it */
  pRsp->Errno = DMSSToAEE_Err( (*msg_ptr)->secerrno );
   
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessRightsInfoUpdatedMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessRightConsumptionMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous consumption of a right.

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessRightConsumptionMsg
(
  secdrm_right_consumption_msg**  msg_ptr
)
{
  IDRM*                        pDRM = NULL;
  AEEDRM_ConsumeRightRspType*  pRsp = NULL;
  AEECallback*                 pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_ConsumeRightRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Consume: pointers corrupted", 0,0,0);
    return;
  }
  
  /* Check the result sent back and return it */
  if ( (*msg_ptr)->secerrno == E_SUCCESS )
  {
    MEMCPY( &(pRsp->LicenseKey),
            &((*msg_ptr)->license),
            sizeof( AEEDRM_LicenseKeyType ) );
  }
  
  pRsp->RightsHandle    = (*msg_ptr)->rights_handle;
  pRsp->XactionID       = (*msg_ptr)->xaction_id;
  pRsp->Errno           = DMSSToAEE_Err( (*msg_ptr)->secerrno );
    
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessRightConsumptionMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessConsumptionValueAppliedMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous consumption of a rights value

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessConsumptionValueAppliedMsg
(
  secdrm_consumption_value_applied_msg**  msg_ptr
)
{
  IDRM*                              pDRM = NULL;
  AEEDRM_UpdateConsumeValueRspType*  pRsp = NULL;
  AEECallback*                       pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_UpdateConsumeValueRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Update ConsumeVal: pointers corrupted", 0,0,0);
    return;
  }

  /* Check the result sent back and return it */
  if ( (*msg_ptr)->secerrno == E_SUCCESS )
  {
    pRsp->RemainingValue = (*msg_ptr)->remaining_value;
  }

  pRsp->Errno = DMSSToAEE_Err((*msg_ptr)->secerrno);
    
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessConsumptionValueAppliedMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessRightDeletedMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous deletion of a rights object.

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessRightDeletedMsg
(
  secdrm_right_deleted_msg**  msg_ptr
)
{
  IDRM*                       pDRM = NULL;
  AEEDRM_DeleteRightRspType*  pRsp = NULL;
  AEECallback*                pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_DeleteRightRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Delete: pointers corrupted", 0,0,0);
    return;
  }

  /* Check the result sent back and return it */
  pRsp->Errno = DMSSToAEE_Err((*msg_ptr)->secerrno);
    
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessRightDeletedMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessAssociatedRightsDeletedMsg()

DESCRIPTION
  This function handles the IPC responses from the DRM sevice for
  asynchronous .

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessAssociatedRightsDeletedMsg
(
  secdrm_associated_rights_deleted_msg**  msg_ptr
)
{
  IDRM*                                  pDRM = NULL;
  AEEDRM_DeleteAssociatedRightsRspType*  pRsp = NULL;
  AEECallback*                           pCB  = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  pDRM = (IDRM*)(*msg_ptr)->usr_data[0];
  pRsp = (AEEDRM_DeleteAssociatedRightsRspType*)(*msg_ptr)->usr_data[1];
  pCB  = (AEECallback*)(*msg_ptr)->usr_data[2];

  if ( !pDRM ||
       !pRsp ||
       !pCB )
  {
    MSG_ERROR("Delete Assoc: pointers corrupted", 0,0,0);
    return;
  }

  /* Check the result sent back and return it */
  pRsp->Errno = DMSSToAEE_Err((*msg_ptr)->secerrno);
    
  /* Delete the response message */
  ipc_delete( msg_ptr );

  /* Call the Callback */
  #if MIN_BREW_VERSION(3,0)
    AEE_ResumeCallback( pCB, pDRM->pac);
  #else
    AEE_ResumeEx( pCB, 0, pDRM->pac);
  #endif

} /* OEMDRM_ProcessAssociatedRightsDeletedMsg() */


/*===========================================================================

FUNCTION OEMDRM_ProcessIPCRsp()

DESCRIPTION
  This function handles IPC responses from the DRM service for
  asynchronous operations.

DEPENDENCIES
  None

PARAMETERS
  rsp_msg_ptr  - Pointer to the received message

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void OEMDRM_ProcessIPCRsp
(
  union ipc_msg_type**  rsp_msg_ptr
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch ( *((ipc_msg_id_type*)(*rsp_msg_ptr)) )
  {
    case secdrm_right_added_msg_id:
      OEMDRM_ProcessRightAddedMsg( 
        (secdrm_right_added_msg**) (rsp_msg_ptr) );
      break;

    case secdrm_rights_found_msg_id:
      OEMDRM_ProcessRightsFoundMsg(
        (secdrm_rights_found_msg**) (rsp_msg_ptr) );
      break;

    case secdrm_rights_info_updated_msg_id:
      OEMDRM_ProcessRightsInfoUpdatedMsg(
        (secdrm_rights_info_updated_msg**) (rsp_msg_ptr) );
      break;

    case secdrm_right_consumption_msg_id:
      OEMDRM_ProcessRightConsumptionMsg(
        (secdrm_right_consumption_msg**) (rsp_msg_ptr) );
      break;

    case secdrm_consumption_value_applied_msg_id:
      OEMDRM_ProcessConsumptionValueAppliedMsg(
        (secdrm_consumption_value_applied_msg**) (rsp_msg_ptr) );
      break;

    case secdrm_right_deleted_msg_id:
      OEMDRM_ProcessRightDeletedMsg(
        (secdrm_right_deleted_msg**) (rsp_msg_ptr) );
      break;

    case secdrm_associated_rights_deleted_msg_id:
      OEMDRM_ProcessAssociatedRightsDeletedMsg(
        (secdrm_associated_rights_deleted_msg**) (rsp_msg_ptr) );
      break;

    default:
      MSG_ERROR( "OEMDRM: Invalid IPC Response 0x%x", 
                 *((ipc_msg_id_type*)(*rsp_msg_ptr)), 0, 0 );
      break;
  }

} /* OEMDRM_ProcessIPCRsp() */


#endif /* FEATURE_SEC_DRM */

