/*===========================================================================

FILE: DRMTestApp.c

SERVICES: DRM test app

GENERAL DESCRIPTION:
   N/A

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
   N/A

(c) COPYRIGHT 2002, 2004 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
===========================================================================*/


/*===========================================================================

$Header: //depot/asic/qsc60x0/apps/FldDbg/DRMTestApp.c#1 $
$DateTime: 2006/08/08 08:56:50 $ $Author: johns $


                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------
08/05/04     aag     Added additional tests, fixed consume tests
06/03/04     aag     Added all existing DRM tests and converted to BREW API
06/02/04     jay     Created file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"         //Handset Feature Definitions
#include "comdef.h"

#include "AEE.h"
#include "AEE_OEM.h"

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AppComFunc.h"
#include "OEMClassIDs.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEFile.h"

#include "msg.h"

#ifdef FEATURE_SEC_DRM
#include "AEEDRM.h"
#include "AEEDCF.h"

/*
 * This should be enabled if you want to turn the asynchronous test suite on
 */
//#define ASYNC_TESTS

/*
 * This should be enabled if you want to turn the internal test suite on
 */
//#define INTERNAL_TESTS

#ifdef FEATURE_SEC_TESTAPPS
#error code not present
#endif /* FEATURE_SEC_TESTAPPS */



/*===========================================================================

FUNCTION DRMUnitTestApp

DESCRIPTION
  Entry point to all BREW based DRM Unit tests

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void DRMUnitTestApp( IShell* pIShell )
{
#ifdef  FEATURE_SEC_TESTAPPS
#error code not present
#endif /* FEATURE_SEC_TESTAPPS */
}/* DRMUnitTestApp */



/*===========================================================================

FUNCTION DRMTestApp

DESCRIPTION
  Entry point to a refernce App using DRM and DCF APIs

DEPENDENCIES
  Does not Return.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/


/* General defines for downloader & player */
#define CIPHER_BUFFER_LENGTH 2048
#define OUTPUT_BUFFER_LENGTH 2048
#define RIGHTS_OBJECT_LIST_LENGTH 2
#define DCF_BASE_DIRECTORY "DCFStore"
#define PATH_SEPARATOR "/"

/*
 * FORWARD LOCK CONTENT
 *
 * Application (.exe)
 */
#define CONTENT_1_LENGTH 512
#define CONTENT_1_URI "content1@foo.com"
#define CONTENT_1_MIME_TYPE "application/octet-stream"
#define CONTENT_1_DCF_FILE_NAME "DCFStore/content1.dcf"

/*
 * COMBINED DELIVERY CONTENT
 *
 * Image (.jpg)
 */
#define CONTENT_2_LENGTH 277
#define CONTENT_2_URI "content2@foo.com"
#define CONTENT_2_MIME_TYPE "image/jpeg"
#define CONTENT_2_DCF_FILE_NAME "DCFStore/content2.dcf"

/*
 * SEPARATE DELIVERY CONTENT
 *
 * Mp3 File (.mp3)
 */
#define CONTENT_3_LENGTH 1029
#define CONTENT_3_URI "content3@foo.com"
#define CONTENT_3_MIME_TYPE "audio/mpeg3"
#define CONTENT_3_DCF_FILE_NAME "DCFStore/content3.dcf"


/*===========================================================================
FUNCTION
  CreateDCFFile

DESCRIPTION
  This function creates a buffer that holds the contents of a .DCF file.
  Given unencrypted content and all required arguments, the function will
  automatically create a header, encrypt the content and append them
  appropriately into a .DCF  buffer.

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - .DCF buffer created
    ENOMEMORY - Couldn't allocate memory
    EFAILED - General failure case
    EBADPARM - Bad parameters

SIDE EFFECTS
  None
===========================================================================*/
int CreateDCFFile
      ( IShell*                       pIShell,
        IDCF*                         pIDCF,
        IDRM*                         pIDRM,
        const char*                   FileName,
        const char*                   URI,
        const char*                   MimeType,
        uint8*                        pContent,
        uint32                        ContentLength,
        const AEEDRM_RightsHandleType RightsHandle
      )
{
  /* Variable declarations */
  IFileMgr*                           pIFileMgr = NULL;
  IFile*                              pIFile = NULL;
  AEEDRM_EncryptionInfoType           EncryptionInfo;
  uint8*                              pDCFHeader = NULL;
  uint32                              DCFHeaderLength = 0;
  uint8*                              pCipherBuffer = NULL;
  uint32                              CipherBufferLength = 0;
  uint32                              RefCount = 0;
  uint32                              FileError = 0;
  int                                 Errno = SUCCESS;
  boolean                             IsContentComplete = FALSE;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Do sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDCF == NULL
         ||
         pIDRM == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
         ||
         URI == NULL
         ||
         URI[0] == '\0'
         ||
         MimeType == NULL
         ||
         MimeType[0] == '\0'
         ||
         pContent == NULL
         ||
         ContentLength == 0
         ||
         RightsHandle == NULL
        )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * Allocate memory for the DCF header and set its size
     */
    pDCFHeader = (uint8*) MALLOC ( AEEDCF_DCFHDR_BUFFER_SIZE );
    if ( pDCFHeader == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDCF_DCFHDR_BUFFER_SIZE\
          bytes.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    DCFHeaderLength = AEEDCF_DCFHDR_BUFFER_SIZE;



    /*
     * To create a DCF file, the content must first be given a DCF header
     */
    Errno = IDCF_CreateContentHeader
              ( pIDCF,
                URI,
                MimeType,
                ContentLength,
                &EncryptionInfo,
                pDCFHeader,
                &DCFHeaderLength
              );
    /*
     * At the end of this function call, DCFHeaderLength now holds the size of
     * the created DCF header
     */

    /*
     * Check for error conditions to ensure that the header was created
     * successfully
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Could not create content header.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * The next step is to encrypt the content
     *
     * In this case, because the content itself can't be modified, we are
     * creating a separate buffer and dumping the encrypted content in there.
     * This will be done in a SINGLE STEP
     *
     * In other words, encryption is done OUT OF PLACE, SINGLE STEP
     */

    /*
     * Allocate memory for the cipher buffer and set its size
     */
    pCipherBuffer = (uint8*) MALLOC ( CIPHER_BUFFER_LENGTH );
    if ( pCipherBuffer == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate CIPHER_BUFFER_LENGTH bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    CipherBufferLength = CIPHER_BUFFER_LENGTH;
    IsContentComplete = TRUE;

    /*
     * Protect the content OUT OF PLACE, SINGLE STEP
     */
    Errno = IDRM_ProtectContent
        ( pIDRM,
          RightsHandle,
          pContent,
          ContentLength,
          pCipherBuffer,
          &CipherBufferLength,
          IsContentComplete,
          &EncryptionInfo
        );

    /*
     * Check for error conditions to ensure that the content was encrypted
     * successfully
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Could not protect content. Error #: %d.", Errno, 0,
          0 );
      Errno = EFAILED;
      break;
    }



    /*
     * We now have:
     *    a) DCF Header
     *    b) Protected Content
     *
     * Using these two pieces we can create a DCF file (.dcf)
     */

    /*
     * First, get a handle to the IFileMgr interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * First test and see if the file exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                FileName
              );

    if ( Errno == SUCCESS )
    {
      /*
       * The file exists...so delete it!
       */
      Errno = IFILEMGR_Remove
                ( pIFileMgr,
                  FileName
                );
      if ( Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Couldn't delete file.  Error #: %d.", Errno, 0,
            0 );
        Errno = EFAILED;
        break;
      }
    }

    /*
     * Well...it doesn't exist or has been deleted, so reset Errno
     */
    Errno = SUCCESS;

    /*
     * Well, we can create the file now
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  FileName,
                  _OFM_CREATE
                );

    /*
     * Check error conditionss
     */
    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't create file.  Error #: %d.", Errno, 0,
          0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Release the file
     */
    RefCount = IFILE_Release ( pIFile );
    /*
     * The file should be closed now
     */

    /*
     * Now open in R/W mode...do I have to do this in two separate steps?!
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  FileName,
                  _OFM_READWRITE
                );

    /*
     * Check error conditionss
     */
    if ( pIFile == NULL )
    {
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't read/write file.  Error #: %d.",
        Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Finally, we are ready to write to the file
     */

    /*
     * I assume that I'm at the start of the file (since it was just
     * created )
     *
     * Write the DCF header
     */
    FileError = IFILE_Write
                  ( pIFile,
                    pDCFHeader,
                    DCFHeaderLength
                  );

    if ( FileError == 0 )
    {
      MSG_ERROR ( "FAIL: Can't write DCF header to file.", 0, 0, 0 );
      Errno = EFAILED;

      /*
       * Before we break out, close the file - this should only loop once
       */
      do
      {
        RefCount = IFILE_Release ( pIFile );
      } while ( RefCount != 0 );

      break;
    }

    /*
     * Write the encrypted content
     */
    FileError = IFILE_Write
                  ( pIFile,
                    pCipherBuffer,
                    CipherBufferLength
                  );

    if ( FileError == 0 )
    {
      MSG_ERROR ( "FAIL: Can't write encrypted content to file.", 0, 0, 0 );
      Errno = EFAILED;


      /*
       * Before we break out, close the file - this should only loop once
       */
      do
      {
        RefCount = IFILE_Release ( pIFile );
      } while ( RefCount != 0 );

      break;
    }

    /*
     * Close up the file
     */
    RefCount = IFILE_Release ( pIFile );
    /*
     * Now the file is closed and we have a brand new .DCF file!
     */



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** CreateDCFFile FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** CreateDCFFile PASSED *****", 0, 0, 0 );
  }



  FREE ( pDCFHeader );
  FREE ( pCipherBuffer );

  IFILEMGR_Release ( pIFileMgr );

  return Errno;
}



/*===========================================================================
FUNCTION
  HandleForwardLock

DESCRIPTION
  This function demonstrates how the downloader would deal with a forward
  lock media object.

  To recap, a forward lock media object is one which the user has permanent
  rights on.  _However_ they will not be allowed to transfer these objects to
  other users.  Forward lock media objects do not come with associated OMA
  rights.

  To insulate the player application from the downloading scheme, the
  dowloader takes the following steps:
      a) It creates a right with all permissions enabled and permanent usage
      b) It adds this permission to the secure database
      c) It creates a .DCF file with the content

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Forward lock right added to database
    ENOMEMORY - Couldn't allocate memory
    EBADPARM - Bad parameters given
    EFAILED - General failure case

SIDE EFFECTS
  None
===========================================================================*/
int HandleForwardLock
      ( IShell*                         pIShell,
        IDCF*                           pIDCF,
        IDRM*                           pIDRM,
        const char*                     FileName,
        const char*                     URI,
        const char*                     MimeType,
        uint8*                          pContent,
        uint32                          ContentLength
      )
{
  /* Variable declarations */
  AEEDRM_PermissionTemplateType         PermissionsTemplate;
  AEEDRM_AddRightRspType                AddRsp;
  AEEDRM_RightsHandleType               RightsHandle = 0;
  uint8*                                pOMARightsObject = NULL;
  uint32                                OMARightsObjectLength = 0;
  int                                   Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/


  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDCF == NULL
         ||
         pIDRM == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
         ||
         URI == NULL
         ||
         URI[0] == '\0'
         ||
         MimeType == NULL
         ||
         MimeType[0] == '\0'
         ||
         pContent == NULL
         ||
         ContentLength == 0
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The first step is to create a forward lock right for this media object.
     */

    /*
     * The simplest way to create a forward lock right is to memset the entire
     * template to 0.
     */
    MEMSET ( &PermissionsTemplate, 0,
        sizeof ( AEEDRM_PermissionTemplateType ) );

    /*
     * Allocate memory for an OMA 1.0 rights object and set its size
     */
    pOMARightsObject = (uint8*) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARightsObject == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_XML_BUFFER_SIZE bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    OMARightsObjectLength = AEEDRM_XML_BUFFER_SIZE;

    /*
     * Use this template to create a right
     */
    Errno = IDRM_CreateRight
              ( pIDRM,
                URI,
                PermissionsTemplate,
                pOMARightsObject,
                &OMARightsObjectLength
              );
    /*
     * At the end of this call, OMARightsObjectLength is set to the actual
     * length of the created OMA 1.0 rights object
     */

    /*
     * Check for error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Fwd lock rights can't be created.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now we add this newly created right to the secure database, so that we
     * can get a rights handle for it
     */
    IDRM_AddRight
      ( pIDRM,
        URI,
        AEEDRM_OMA_1_0_VERSION,
        pOMARightsObject,
        OMARightsObjectLength,
        &AddRsp,
        NULL
      );

    /*
     * Ensure that the add was successful
     */
    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't add fwd lock right.  Error #: %d.",
          AddRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the rights handle so that it can be used to create a .DCF file
     */
    RightsHandle = AddRsp.RightsHandle;



    /*
     * Now that the right has been added to the database, it is a simple
     * matter to create the .DCF file
     */
    Errno = CreateDCFFile
              ( pIShell,
                pIDCF,
                pIDRM,
                FileName,
                URI,
                MimeType,
                pContent,
                ContentLength,
                RightsHandle
              );

    /*
     * Check the error conditions.  Errors can be one of:
     *    a) EFAILED
     *    b) ENOMEMORY
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create .DCF file.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** HandleForwardLock FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** HandleForwardLock PASSED *****", 0, 0, 0 );
  }

  FREE ( pOMARightsObject );

  return Errno;
}



/*===========================================================================
FUNCTION
  HandleCombinedDelivery

DESCRIPTION
  This function demonstrates the role the downloader application should play
  if it has to deal with a combined delivery media object.

  To recap, in the case of combined delivery, the unencrypted media object is
  shipped together with its rights in a DRM message.  Since the goal is to
  insulate the player application from the downloading method used, the
  downloader has the following responsibilities:
      a) Add the supplied right to the secure database
      b) Create a .DCF file using the content

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Combined delivery right added to database
    EBADPARM - Bad parameters given
    EFAILED - General failure case

SIDE EFFECTS
  None
===========================================================================*/
int HandleCombinedDelivery
      ( IShell*                           pIShell,
        IDCF*                             pIDCF,
        IDRM*                             pIDRM,
        const char*                       FileName,
        const char*                       URI,
        const char*                       MimeType,
        uint8*                            pContent,
        uint32                            ContentLength,
        uint8*                            pRightsObject,
        uint32                            RightsObjectLength,
        const AEEDRM_VersionEnumType      RELVersion
      )
{
  /* Variable declarations */
  AEEDRM_AddRightRspType                    AddRsp;
  AEEDRM_RightsHandleType                   RightsHandle = 0;
  int                                       Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDRM == NULL
         ||
         pIDCF == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
         ||
         URI == NULL
         ||
         URI[0] == '\0'
         ||
         MimeType == NULL
         ||
         MimeType[0] == '\0'
         ||
         pContent == NULL
         ||
         ContentLength == 0
         ||
         pRightsObject == NULL
         ||
         ( (char*)pRightsObject )[0] == '\0' /* Assuming right is in text! */
         ||
         RightsObjectLength == 0
         ||
         RightsObjectLength > AEEDRM_XML_BUFFER_SIZE
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The first step is to add the right to the secure database in order to
     * get a rights handle
     */

    /*
     * Add the right to the database using the user supplied right
     */
    IDRM_AddRight
      ( pIDRM,
        URI,
        RELVersion,
        pRightsObject,
        RightsObjectLength,
        &AddRsp,
        NULL
      );

    /*
     * Check error conditions to ensure that the add was successful
     */
    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot add right.  Error #: %d.", AddRsp.Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    };

    /*
     * Set the rights handle so that it can be used to create a .DCF file
     */
    RightsHandle = AddRsp.RightsHandle;



    /*
     * Now that the right has been added to the database, it is a simple
     * matter to create the .DCF file
     */
    Errno = CreateDCFFile
              ( pIShell,
                pIDCF,
                pIDRM,
                FileName,
                URI,
                MimeType,
                pContent,
                ContentLength,
                RightsHandle
              );

    /*
     * Check the error conditions.  Errors can be one of:
     *    a) EFAILED
     *    b) ENOMEMORY
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create .DCF file.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** HandleCombinedDelivery FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** HandleCombinedDelivery PASSED *****", 0, 0, 0 );
  }


  return Errno;
}



/*===========================================================================
FUNCTION
  HandleSeparateDelivery

DESCRIPTION
  This function demonstrated the role the downloader should take when dealing
  with a separate delivery DRM protected media object.

  To recap, in the case of separate delivery, the rights object and the
  encrypted content are shipped separately.  This is (by far) the simplest
  case for the downloader to deal with.  It simply has to:
      a) Add the supplied right to the secure database

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Added a separate delivery right to the secure database
    EBADPARM - Bad parameters given
    EFAILED - General failure case

SIDE EFFECTS
  None
===========================================================================*/
int HandleSeparateDelivery
      ( IDRM*                                   pIDRM,
        const char*                             URI,
        uint8*                                  pRightsObject,
        uint32                                  RightsObjectLength,
        const AEEDRM_VersionEnumType            RELVersion
      )
{
  /* Variable declarations */
  AEEDRM_AddRightRspType                        AddRsp;
  int                                           Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIDRM == NULL
         ||
         pRightsObject == NULL
         ||
         ( (char*) pRightsObject )[0] == '\0' /* Assuming it is text! */
         ||
         RightsObjectLength == 0
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The only step is to add the right to the secure database in order to
     * get a rights handle
     */

    IDRM_AddRight
      ( pIDRM,
        URI,
        RELVersion,
        pRightsObject,
        RightsObjectLength,
        &AddRsp,
        NULL
      );

    /*
     * Check the error code to ensure that the add was successful
     */
    if ( AddRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot add right.  Error #: %d.", AddRsp.Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    };



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** HandleSeparateDelivery FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** HandleSeparateDelivery PASSED *****", 0, 0, 0 );
  }


  return Errno;
}



/*===========================================================================
FUNCTION
  PrintRightsInformation

DESCRIPTION
  This function does a lookup on a supplied URI and prints out constraints
  information on all the rights associated with that URI

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Lookup successful, rights retrieved successfully and printed
    ENOMEMORY - Couldn't allocate memory
    EBADPARM - Bad parameters given
    EFAILED - Generic failure case

SIDE EFFECTS
  None
===========================================================================*/
int PrintRightsInformation
      ( IDRM*                           pIDRM,
        const char*                     URI
      )
{
  /* Variable declarations */
  AEEDRM_LookupRightRspType             LookupRsp;
  AEEDRM_XactionIdType                  XactionId = 0;
  AEEDRM_RightsObjectType*              pRightsObjectList = NULL;
  uint32                                RightsObjectListLength = 0;
  uint32                                NumRights = 0;
  uint32                                RightsRetrieved = 0;
  uint32                                Counter = 0;
  int                                   Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIDRM == NULL
         ||
         URI == NULL
         ||
         URI[0] == '\0'
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * Lookup the URI
     *
     * We _must_ do a lookup before we do a retrieve because the transaction
     * ID returned by the lookup is associated with the list of rights that
     * the retrieve has to pick up
     */
    IDRM_LookupRight
      ( pIDRM,
        URI,
        0x00,
        &LookupRsp,
        NULL
      );

    /*
     * Do an error check
     */
    if ( LookupRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't look up the URI.  Error #: %d",
          LookupRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Get the number of rights and transaction id
     */
    NumRights = LookupRsp.NumRights;
    XactionId = LookupRsp.XactionID;




    /*
     * Retrieve the rights using the transaction id generated above
     */

    /*
     * Allocate the memory for the rights object list
     */
    pRightsObjectList = (AEEDRM_RightsObjectType*) MALLOC
                          ( sizeof ( AEEDRM_RightsObjectType ) *
                            RIGHTS_OBJECT_LIST_LENGTH
                          );
    if ( pRightsObjectList == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC couldn't allocate memory", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    RightsObjectListLength = RIGHTS_OBJECT_LIST_LENGTH;

    /*
     * Now do the actual retrieve in a loop
     */
    do
    {
      Errno = IDRM_RetrieveRight
                ( pIDRM,
                  XactionId,
                  pRightsObjectList,
                  &RightsObjectListLength
                );

      /*
       * These are the two cases where the retrieve was successful
       */
      if ( Errno == SUCCESS
           ||
           Errno == AEEDRM_EMOREDATA )
      {
        /*
         * Let's keep a running tally of the number of rights retrieved by the
         * API
         */
        RightsRetrieved += RightsObjectListLength;



        /*
         * Print out information about the rights objects retrieved
         */
        for ( Counter = 0; Counter < RightsObjectListLength; Counter++ )
        {
          MSG_HIGH ( "DRM Version: %d.",
              pRightsObjectList[Counter].DrmVersion, 0, 0 );
          MSG_HIGH ( "Permissions Mask: 0x%x.",
              pRightsObjectList[Counter].PermissionsMask, 0, 0 );

          /*
           * Print out information on the Play constraints
           */
          MSG_HIGH ( "Play Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Play.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Play.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Play.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Play.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Play.RemainingValue,
              0, 0 );

          /*
           * Print out information on the Display constraints
           */
          MSG_HIGH ( "Display Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Display.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Display.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Display.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Display.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Display.RemainingValue,
              0, 0 );

          /*
           * Print out information on the Execute constraints
           */
          MSG_HIGH ( "Execute Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Execute.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Execute.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Execute.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Execute.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Execute.RemainingValue,
              0, 0 );

          /*
           * Print out information on the Print constraints
           */
          MSG_HIGH ( "Print Constraints: ", 0, 0, 0 );
          MSG_HIGH ( "", 0, 0, 0 );
          MSG_HIGH ( "Constraint Mask: 0x%x.",
              pRightsObjectList[Counter].Constraints.Print.ConstraintsMask,
              0, 0 );
          MSG_HIGH ( "Total Count: %d.",
              pRightsObjectList[Counter].Constraints.Print.TotalCount,
              0, 0 );
          MSG_HIGH ( "Remaining Count: %d.",
              pRightsObjectList[Counter].Constraints.Print.RemainingCount,
              0, 0 );
          MSG_HIGH ( "Total Value: %d.",
              pRightsObjectList[Counter].Constraints.Print.TotalValue,
              0, 0 );
          MSG_HIGH ( "Remaining Value: %d.",
              pRightsObjectList[Counter].Constraints.Print.RemainingValue,
              0, 0 );
        }

        /*
         * Done printing all the constraints for the rights that were
         * retrieved
         */



      }

      /*
       * In this case, there are more rights to be retrieved, so we reset the
       * size of the rights list so that another RIGHTS_OBJECT_LIST_LENGTH
       * rights can be copied.
       */
      if ( Errno == AEEDRM_EMOREDATA )
      {
        /*
         * Reset the rights object list length again
         */
        RightsObjectListLength = RIGHTS_OBJECT_LIST_LENGTH;
      }



    } while ( Errno == AEEDRM_EMOREDATA );

    if ( Errno != SUCCESS )
    {
      /*
       * It appears we've broken out early because of some sort of error.
       */
      MSG_ERROR ( "FAIL: Unable to retrieve rights.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    MSG_HIGH ( "Number of rights retrieved: %d.", RightsRetrieved, 0, 0 );



  } while ( 0 );

  FREE ( pRightsObjectList );

  return Errno;
}



/*===========================================================================
FUNCTION
  DRMPlayer

DESCRIPTION
  This function demonstrates the role of a DRM enabled media player.

  NOTE: This function does NO actual playing of media.  It simply
        illustrates the steps that have to be taken by the player to
        actually play the content

  To play the content, the player has to go through the following steps:
      a) Consume the right
      b) Check if the actions requested actually match the permissions
         returned
      c) Parse the .DCF file for information about the encrypted content
         attached to it
      d) Decrypt the content using the license key returned by the
         IDRM_ConsumeRight() and the encryption information returned by
         IDCF_ParseContentHeader()
      e) Play the right

  NOTE:  You can supply _ONE_ of URI/RightsHandle.  You don't have to supply
         both.  However, it is an error to supply neither.

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Content played
    ENOMEMORY - Couldn't allocate memory
    EBADPARM - Bad parameters given
    EFAILED - Generic failure case

SIDE EFFECTS
  None
===========================================================================*/
int DRMPlayer
      ( IShell*                           pIShell,
        IDCF*                             pIDCF,
        IDRM*                             pIDRM,
        const char*                       FileName,
        const AEEDRM_ActionEnumType       Action
      )
{
  /* Variable declarations */
  AEEDRM_ConsumeRightRspType              ConsumeRsp;
  AEEDCF_HeaderType                       ParsedDCFHeader;
  IFileMgr*                               pIFileMgr = NULL;
  IFile*                                  pIFile = NULL;
  FileInfo                                DCFFileInfo;
  uint8*                                  pOutput = NULL;
  uint8*                                  pDCFFileBuffer = NULL;
  uint32                                  DCFFileBufferLength = 0;
  uint32                                  RefCount = 0;
  boolean                                 IsContentComplete = FALSE;
  int                                     BytesRead = 0;
  int                                     Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL
         ||
         pIDCF == NULL
         ||
         pIDRM == NULL
         ||
         FileName == NULL
         ||
         FileName[0] == '\0'
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * We need to access the DCF file for a number of reasons:
     *
     * 1) Find the size of the file so that we can allocate memory for a
     *    buffer into which we can place the contents
     * 2) Read the DCF header and content into this buffer
     */

    /*
     * Get a handle to the IFileMgr interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Open up this file for read access
     */
    pIFile = IFILEMGR_OpenFile
                ( pIFileMgr,
                  FileName,
                  _OFM_READ
                );

    if ( pIFile == NULL )
    {
      /*
       * Something broke - so let's check what the error is
       */
      Errno = IFILEMGR_GetLastError ( pIFileMgr );
      MSG_ERROR ( "FAIL: Couldn't read from file.  Error #: %d.",
        Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Get the file size of the file
     */
    Errno = IFILE_GetInfo
              ( pIFile,
                &DCFFileInfo
              );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't get file information.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;

      /*
       * In case we failed with an error, we still want to make sure the file is
       * closed properly
       */
      while ( RefCount != 0 )
      {
        RefCount = IFILE_Release ( pIFile );
      }

      break;
    }

    /*
     * We need to allocate space for the buffer into which the contents of
     * the .DCF file is read.
     */
    pDCFFileBuffer = (uint8*) MALLOC ( DCFFileInfo.dwSize );
    if ( pDCFFileBuffer == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          DCFFileInfo.dwSize, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    DCFFileBufferLength = DCFFileInfo.dwSize;

    /*
     * Read all the bytes from the file
     */
    BytesRead = IFILE_Read
                  ( pIFile,
                    pDCFFileBuffer,
                    DCFFileBufferLength
                  );
    if ( BytesRead == 0 )
    {
      MSG_ERROR ( "FAIL: Can't read from file.", 0, 0, 0 );
      Errno = EFAILED;

      /*
       * In case we failed with an error, we still want to make sure the file is
       * closed properly
       */
      while ( RefCount != 0 )
      {
        RefCount = IFILE_Release ( pIFile );
      }

      break;
    }

    /*
     * Close up the file...we're done - we only need to do this once,
     * but you should check RefCount...
     */
    RefCount = IFILE_Release ( pIFile );



    /*
     * To play the file/decrypt the content we need two pieces of
     * information:
     *    a) License Key
     *    b) Encryption Information
     *
     * The license key is returned as part of a valid consume response.
     * The encryption information has to be retrieved from the .DCF file
     */

    /*
     * Get the encryption information
     */

    /*
     * The first step is to get the information from the .DCF file into the DCF
     * header structure
     */
    Errno = IDCF_ParseContentHeader
              ( pIDCF,
                pDCFFileBuffer,
                DCFFileBufferLength,
                &ParsedDCFHeader
              );

    /*
     * Check the error code to ensure that the DCF file was parsed properly
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "Can't parse the .DCF file.  Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Print out rights information for the rights associated with the
     * content
     */
    Errno = PrintRightsInformation
              ( pIDRM,
                ParsedDCFHeader.szContentURI
              );
    /*
     * Check error condition
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't print out rights information.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now, we use the information in the .DCF file to see if the object can
     * be consumed.  Specifically, we use the URI.
     */

    /*
     * To do this, simply call IDRM_ConsumeRight() with the URI returned in
     * the ParsedDCFHeader
     */
    IDRM_ConsumeRight
      ( pIDRM,
        ParsedDCFHeader.szContentURI,
        0x00,
        Action,
        &ConsumeRsp,
        NULL
      );

    /*
     * Check the error codes...these are extremely important to the player.
     * Right now we don't do anything specific with them.  However, based on
     * the return value, the player can tell exactly what the issue was with
     * the consume
     */
    if ( ConsumeRsp.Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't consume right.  Error #: %d.",
          ConsumeRsp.Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }
   
    /*
     * Finally, we have the pieces required to decrypt the content
     */


    /*
     * Since we are going to decrypt in a single step
     */
    IsContentComplete = TRUE;

    /*
     * For the interests of the example we will demonstrate how to decrypt the
     * content IN PLACE in a SINGLE STEP
     *
     * This API supports IN PLACE decryption and decryption in MULTIPLE STEPS
     */
    Errno = IDCF_DecryptContent
              ( pIDCF,
                ParsedDCFHeader.pContentData,
                ParsedDCFHeader.ContentDataLen,
                ParsedDCFHeader.pContentData,
                &( ParsedDCFHeader.ContentDataLen ),
                IsContentComplete,
                &( ConsumeRsp.LicenseKey ),
                &( ParsedDCFHeader.EncryptionInfo )
              );

    /*
     * Check the error codes to ensure that we decrypted properly
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't decrypt content.  Error #: %d.", Errno,
          0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Now that we have decrypted the content (and it is in pOutput) we (as
     * the player) can do whatever we want with it.  i.e. play it, display it
     * etc.
     */



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** DRMPlayer FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** DRMPlayer PASSED *****", 0, 0, 0 );
  }

  FREE ( pOutput );
  FREE ( pDCFFileBuffer );

  IFILEMGR_Release ( pIFileMgr );

  return Errno;
}



/*===========================================================================
FUNCTION
  CreateDCFDirectory

DESCRIPTION
  This function creates the DCF directory that all the .DCF files are stored
  in

DEPENDENCIES
  None

RETURN VALUE
  Integer error value.
  Can be one of:
    SUCCESS - Directory created successfully
    EBADPARM - Bad parameters given
    EFAILED - Generic failure case

SIDE EFFECTS
  None
===========================================================================*/
int CreateDCFDirectory ( IShell*            pIShell,
                         const char*        DCFDirName
                       )
{
  /* Variable declarations */
  IFileMgr*                                 pIFileMgr = NULL;
  int                                       Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Keep looping as long as there are no errors and the functionality is not
   * complete
   */
  do
  {
    /*
     * Sanity checks
     */
    if ( pIShell == NULL
         ||
         DCFDirName == NULL
         ||
         DCFDirName[0] == '\0'
       )
    {
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * To create the directory we need to create a handle to the IFILEMGR
     * interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
         ( void** )(&pIFileMgr) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * We are going to store all the .DCF files in a directory named
     * DCFStore
     *
     * Since I am assuming BREW 3.0, this name is _CASE SENSITIVE_
     * The exact path for this directory is fs:/DCFStore
     */

    /*
     * First, we do a quick test to see if the directory exists
     */
    Errno = IFILEMGR_Test
              ( pIFileMgr,
                DCFDirName
              );

    /*
     * The directory does not exist, so we must create it
     */
    if ( Errno != SUCCESS )
    {
      /*
       * Create the directory then
       */
      Errno = IFILEMGR_MkDir
                ( pIFileMgr,
                  DCFDirName
                );

      if ( Errno != SUCCESS )
      {
        MSG_ERROR ( "FAIL: Couldn't create DCF_BASE_DIRECTORY.  Error #: %d",
            Errno, 0, 0 );
        Errno = EFAILED;
        break;
      }
    }



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** CreateDCFDirectory FAILED *****", 0, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** CreateDCFDirectory PASSED *****", 0, 0, 0 );
  }

  IFILEMGR_Release ( pIFileMgr );

  return Errno;
}



/*===========================================================================
FUNCTION
  DRMDownloadPlayerApp

DESCRIPTION
  This function is the entry point to demonstrate usage of the
  downloader/player.  It creates content, decides which use case it will
  execute (forward lock, combined delivery, separate delivery), executes
  that use case and finally plays the content

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void DRMTestApp ( IShell* pIShell )
{
  /* Variable declarations */
  IFileMgr*                               pIFileMgr = NULL;
  IDRM*                                   pIDRM = NULL;
  IDCF*                                   pIDCF = NULL;
  AEEDRM_ActionEnumType                   Action = 0x00;
  AEEDRM_VersionEnumType                  RELVersion = AEEDRM_OMA_1_0_VERSION;
  AEEDRM_DeleteAssociatedRightsRspType    DeleteAssocRsp;
  AEEDRM_PermissionTemplateType           PermissionsTemplate;
  uint8*                                  pContent1 = NULL;
  uint32                                  Content1Length = 0;
  uint8*                                  pOMARightsObject = NULL;
  uint32                                  OMARightsObjectLength = 0;
  char*                                   pContent1URI = NULL;
  char*                                   pContent1MimeType = NULL;
  char*                                   pContent1FileName = NULL;
  uint8*                                  pContent2 = NULL;
  uint32                                  Content2Length = 0;
  char*                                   pContent2URI = NULL;
  char*                                   pContent2MimeType = NULL;
  char*                                   pContent2FileName = NULL;
  int                                     StrErr = 0;
  int                                     Errno = SUCCESS;
  /*-----------------------------------------------------------------------*/



  /*
   * Loop as long as there is no error and the functionality is not complete.
   */
  do
  {
    /*
     * Basic sanity checks
     */
    if ( pIShell == NULL )
    {
      /* It should _NEVER_ get here */
      MSG_ERROR ( "FAIL: Bad parameters given.", 0, 0, 0 );
      Errno = EBADPARM;
      break;
    }



    /*
     * The first step in dealing with playing a media object is
     * get a handle to the DRM interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDRM, ( void** )(&pIDRM) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DRM interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * We follow this by creating a handle to the DCF interface
     */
    if ( ISHELL_CreateInstance ( pIShell, AEECLSID_IDCF, ( void** )(&pIDCF) )
         !=
         SUCCESS )
    {
      MSG_ERROR ( "FAIL: Cannot create DCF interface.", 0, 0, 0 );
      Errno = EFAILED;
      break;
    }



    /*
     * Create the directory in which the DCF files are stored
     */
    Errno = CreateDCFDirectory ( pIShell, DCF_BASE_DIRECTORY );

    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create DCF directory.  Error #: %d.", Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * FORWARD LOCK MEDIA OBJECT
     *
     * A forward lock media object is one that is transmitted unprotected to
     * the user.  The user is allowed permanent and unconstrainted rights to
     * do whatever desired with that object _on the device_.  The user cannot
     * transmit this media object to another device.
     *
     * A typical example of a forward lock media object being transmitted
     * using HTTP download is given below
     *
     *
     * HTTP/1.1 200 OK 
     * Content-type: application/vnd.oma.drm.message;
     * boundary=boundary-1 
     * Content-Length: 574 
     * --boundary-1
     * Content-type: image/jpeg
     * Content-Transfer-Encoding: binary
     * ...jpeg image in binary format...
     * --boundary-1--
     *
     * In this case, the downloader would parse the HTTP stream and extract
     * all the headers.  It would then pass the headers we are interested in
     * to the DRM stack.  The information we are interested in is:
     *
     * Content-Length
     * Content-Type (Mime Type)
     * Content (in the above case it would be the jpeg image)
     * Content-URI
     *
     * In the example, I have below we are not receiving our content from a
     * DRM aware downloader.  So, I have to:
     *
     * a) Create the content
     * b) Create a mime-type and URI for this content
     *
     * In a real system, all this information is parsed from the HTTP stream.
     */

    /*
     * The first step is to create a piece of content.  For our purposes, the
     * content will be an opaque memory blob.  We must also set its size
     */
    pContent1 = (uint8*) MALLOC ( CONTENT_1_LENGTH );
    if ( pContent1 == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate CONTENT_1_LENGTH bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    Content1Length = CONTENT_1_LENGTH;

    /*
     * Let's initialize the content to some known value
     */
    MEMSET ( pContent1, 1, Content1Length );


    /*
     * The next step is to allocate space for the URI and mime type
     */
    pContent1URI = (char*) MALLOC ( AEEDRM_MAX_URI_LEN );
    if ( pContent1URI == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_MAX_URI_LEN bytes.", 0,
          0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent1URI[0] = '\0';

    pContent1MimeType = (char*) MALLOC ( AEEDCF_MIMETYPE_MAXLEN );
    if ( pContent1MimeType == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDCF_MIMETYPE_MAXLEN bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent1MimeType[0] = '\0';

    /*
     * Set the URI and mime types of content 1
     */
    STRCPY ( pContent1URI, CONTENT_1_URI );
    STRCPY ( pContent1MimeType, CONTENT_1_MIME_TYPE );

    /*
     * Finally, allocate space for the filename and set it
     */
    pContent1FileName = (char*) MALLOC
                                  ( STRLEN ( CONTENT_1_DCF_FILE_NAME )
                                    +
                                    1 /* Terminating null */
                                  );
    if ( pContent1FileName == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          STRLEN ( CONTENT_1_DCF_FILE_NAME ) + 1, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent1FileName[0] = '\0';

    StrErr = STRLCPY ( pContent1FileName, CONTENT_1_DCF_FILE_NAME,
        STRLEN ( CONTENT_1_DCF_FILE_NAME ) + 1 );

    if ( StrErr == -1 )
    {
      MSG_ERROR ( "FAIL: Can't copy filename.  Buffer too small.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }

    /*
     * Now, simply call the downloader function for forward lock that we
     * created before!
     */
    Errno = HandleForwardLock
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent1FileName,
                pContent1URI,
                pContent1MimeType,
                pContent1,
                Content1Length
              );

    /*
     * Check the returned error code to see if we successfully 'downloaded' a
     * forward locked media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't download fwd lock media.  Error #: %d",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the action mask we want with this media object in preparation for a
     * 'player'
     */
    Action = AEEDRM_ACTION_EXECUTE;

    /*
     * Now, play the media!
     */
    Errno = DRMPlayer
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent1FileName,
                Action
              );

    /*
     * Check the returned error code to see if we successfully 'played' a
     * forward locked media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't play fwd lock media.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * We have now successfully 'downloaded' and 'played' a fwd lock media.
     * Congratulations! :-)
     */




    /*
     * COMBINED DELIVERY MEDIA OBJECT
     *
     * A combined delivery media object is one in which the rights are
     * transmitted along with the unprotected content.  Unlike the forward
     * lock media object, the combined delivery media object does not grant
     * the user with unconstrained usage rights.  In fact, the user is limited
     * according to the rights shipped with the media object.
     *
     * HTTP/1.1 200 OK
     * Content-type: application/vnd.oma.drm.message;
     * boundary=boundary-1
     * Content-Length: 893
     *
     * --boundary-1
     * Content-type: application/vnd.oma.drm.rights+xml
     * Content-Transfer-Encoding: binary
     *
     * <o-ex:rights 
     *      xmlns:o-ex="http://odrl.net/1.1/ODRL-EX" 
     *      xmlns:o-dd="http://odrl.net/1.1/ODRL-DD" >
     *    <o-ex:context>
     *        <o-dd:version>1.0</o-dd:version>
     *    </o-ex:context>
     *    <o-ex:agreement>
     *        <o-ex:asset>
     *            <o-ex:context>
     *                <o-dd:uid>cid:4567829547@foo.bar</o-dd:uid>
     *            </o-ex:context>
     *        </o-ex:asset>
     *        <o-ex:permission>
     *            <o-dd:display/>
     *        </o-ex:permission>
     *    </o-ex:agreement>
     *  </o-ex:rights>
     *
     *  --boundary-1
     *  Content-type: image/jpeg
     *  Content-ID: <45678929547@foo.bar>
     *  Content-Transfer-Encoding: binary
     *
     *  ...jpeg image in binary format...
     * --boundary-1--
     *
     * Notice that the HTTP stream has two sections:
     * a) Rights section
     * b) Content section
     *
     * In a complete system, the downloader would parse the HTTP stream and
     * separate the two different components.  The media rights
     * (described in XML) would be passed, as is, to the IDRM_AddRight()
     * function call.  The following information is required for the content:
     *
     * Content-ID
     * Content-Type (Mime Type)
     * Content-Length
     * Content itself
     *
     * These pieces of information are extracted by the downloader from the
     * second half of the HTTP stream and are passed to the DRM stack.
     *
     * In our case, we are creating a standalone system, so we:
     *
     * a) Create a rights object (instead of downloading it)
     * b) Create a piece of content, associated URI, Mime-Type and Length
     *    (instead of getting the information from the downloader)
     */

    /*
     * The next step is to create a piece of content.  For our purposes, the
     * content will be an opaque memory blob.  We must also set its size
     */
    pContent2 = (uint8*) MALLOC ( CONTENT_2_LENGTH );
    if ( pContent2 == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate CONTENT_2_LENGTH bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    Content2Length = CONTENT_2_LENGTH;

    /*
     * Let's initialize the content to some known value
     */
    MEMSET ( pContent2, 1, Content2Length );

    /*
     * The next step is to allocate space for the URI and mime type
     */
    pContent2URI = (char*) MALLOC ( AEEDRM_MAX_URI_LEN );
    if ( pContent2URI == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_MAX_URI_LEN bytes.", 0,
          0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent2URI[0] = '\0';

    pContent2MimeType = (char*) MALLOC ( AEEDCF_MIMETYPE_MAXLEN );
    if ( pContent2MimeType == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDCF_MIMETYPE_MAXLEN\
          bytes.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent2MimeType[0] = '\0';

    /*
     * Set the URI and mime types of content 2
     */
    STRCPY ( pContent2URI, CONTENT_2_URI );
    STRCPY ( pContent2MimeType, CONTENT_2_MIME_TYPE );

    /*
     * Finally, allocate space for the filename and set it
     */
    pContent2FileName = (char*) MALLOC
                                  ( STRLEN ( CONTENT_2_DCF_FILE_NAME )
                                    +
                                    1 /* Terminating null */
                                  );
    if ( pContent2FileName == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate %d bytes.",
          STRLEN ( CONTENT_2_DCF_FILE_NAME ) + 1, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    pContent2FileName[0] = '\0';

    StrErr = STRLCPY ( pContent2FileName, CONTENT_2_DCF_FILE_NAME,
        STRLEN ( CONTENT_2_DCF_FILE_NAME ) + 1 );

    if ( StrErr == -1 )
    {
      MSG_ERROR ( "FAIL: Can't copy filename.  Buffer too small.", 0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }

    /*
     * Now we have to create a right to use with this content
     *
     * In your application you don't _have_ to do this at all.  Rights are
     * created by the content supplier.  In the case of combined delivery,
     * the rights are delivered at the same time with the unencrypted
     * content
     */

    /*
     * Let's create a right with:
     *    5 display counts
     *    3 print counts
     */

    /*
     * First, lets clear the template out to make sure there aren't any issues
     */
    MEMSET ( &PermissionsTemplate, 0,
        sizeof ( AEEDRM_PermissionTemplateType ) );

    /*
     * Now, set the constraints
     */
    PermissionsTemplate.Display.IsValid = TRUE;
    PermissionsTemplate.Display.TotalCount = 5;
    PermissionsTemplate.Print.IsValid = TRUE;
    PermissionsTemplate.Print.TotalCount = 3;

    /*
     * Allocate memory for an OMA 1.0 rights object and set its size
     */
    pOMARightsObject = (uint8*) MALLOC ( AEEDRM_XML_BUFFER_SIZE );
    if ( pOMARightsObject == NULL )
    {
      MSG_ERROR ( "FAIL: MALLOC can't allocate AEEDRM_XML_BUFFER_SIZE bytes.",
          0, 0, 0 );
      Errno = ENOMEMORY;
      break;
    }
    OMARightsObjectLength = AEEDRM_XML_BUFFER_SIZE;

    /*
     * Use this template to create a right
     */
    Errno = IDRM_CreateRight
              ( pIDRM,
                pContent2URI,
                PermissionsTemplate,
                pOMARightsObject,
                &OMARightsObjectLength
              );
    /*
     * At the end of this call, OMARightsObjectLength is set to the actual
     * length of the created OMA 1.0 rights object
     */

    /*
     * Check for error conditions
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't create combined delivery right.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Now, simply call the downloader function for combined delivery that we
     * created before!
     */
    Errno = HandleCombinedDelivery
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent2FileName,
                pContent2URI,
                pContent2MimeType,
                pContent2,
                Content2Length,
                pOMARightsObject,
                OMARightsObjectLength,
                RELVersion
              );

    /*
     * Check the returned error code to see if we successfully 'downloaded' a
     * forward locked media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Can't download cmbined delivery media.  Error #: %d",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * Set the action mask we want with this media object in preparation for a
     * 'player'
     */
    Action = AEEDRM_ACTION_DISPLAY;

    /*
     * Now, play the media!
     */
    Errno = DRMPlayer
              ( pIShell,
                pIDCF,
                pIDRM,
                pContent2FileName,
                Action
              );

    /*
     * Check the returned error code to see if we successfully 'played' a
     * combined delivery media object
     */
    if ( Errno != SUCCESS )
    {
      MSG_ERROR ( "FAIL: Couldn't play fwd lock media.  Error #: %d.",
          Errno, 0, 0 );
      Errno = EFAILED;
      break;
    }

    /*
     * We have now successfully 'downloaded' and 'played' a combined delivery
     * media
     * Congratulations! :-)
     */



  } while ( 0 );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "***** DRMDownloadPlayerApp FAILED *****", 0, 0, 0 );
    MSG_ERROR ( "***** Error Code Returned: %d     *****", Errno, 0, 0 );
  }
  else
  {
    MSG_FATAL ( "***** DRMDownloadPlayerApp PASSED *****", 0, 0, 0 );
  }



  /*
   * Either way, today, we're going to have to clean up.
   *
   * To clean up we'll be:
   *  1) Freeing memory (as we always should!)
   *  2) Deleting the rights (NOTE)
   *  3) Deleting the content (NOTE)
   *
   * NOTE: In a normal application, we wouldn't want to do this (after all,
   *       the user paid for those rights and content!)
   */

  /*
   * Create the file manager interface
   */
  if ( ISHELL_CreateInstance ( pIShell, AEECLSID_FILEMGR,
        ( void** )(&pIFileMgr) )
      !=
      SUCCESS )
  {
    MSG_ERROR ( "FAIL: Cannot create IFileMgr interface.", 0, 0, 0 );
    Errno = EFAILED;
    /* HMMMMM */
    /* FIXME */
  }

  /*
   * Content #1
   */

  /*
   * Delete the rights
   */
  IDRM_DeleteAssociatedRights
    ( pIDRM,
      CONTENT_1_URI,
      &DeleteAssocRsp,
      NULL
    );

  if ( DeleteAssocRsp.Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't delete CONTENT_1_URI rights", 0, 0, 0 );
  }

  /*
   * Delete the .dcf file
   */
  Errno = IFILEMGR_Remove
            ( pIFileMgr,
              pContent1FileName
            );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't remove Content # 1 .dcf file.  Error #: %d.",
        Errno, 0, 0 );
  }

  /*
   * No need to keep the memory we've allocated around
   */
  FREE ( pContent1 );
  FREE ( pContent1URI );
  FREE ( pContent1MimeType );
  FREE ( pContent1FileName );

  /*
   * Content # 2
   */

  /*
   * Delete the rights
   */
  IDRM_DeleteAssociatedRights
    ( pIDRM,
      CONTENT_2_URI,
      &DeleteAssocRsp,
      NULL
    );

  if ( DeleteAssocRsp.Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't delete CONTENT_2_URI rights", 0, 0, 0 );
  }

  /*
   * Delete the .dcf file
   */
  Errno = IFILEMGR_Remove
            ( pIFileMgr,
              pContent2FileName
            );

  if ( Errno != SUCCESS )
  {
    MSG_ERROR ( "FAIL: Can't remove Content # 2 .dcf file.  Error #: %d.",
        Errno, 0, 0 );
  }

  /*
   * No need to keep the memory we've allocated around
   */
  FREE ( pContent2 );
  FREE ( pContent2URI );
  FREE ( pContent2MimeType );
  FREE ( pContent2FileName );
  FREE ( pOMARightsObject );



  /*
   * Finally, release all the interfaces
   */
  IDRM_Release ( pIDRM );
  IDCF_Release ( pIDCF );
  IFILEMGR_Release ( pIFileMgr );
}
#endif  /* FEATURE_SEC_DRM */

