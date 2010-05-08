/*=====================================================================
FILE: gsm1xactvfile.c

GENERAL DESCRIPTION:
   See gsm1xactvfile.h for the description of interfaces implemented
   here. 
        

IMPLEMENTATION-SPECIFIC COMMENTS:
   None.
       

              (c) COPYRIGHT 2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEFile.h"
#include "AEEStdLib.h"         
#include "AEEGSM1xControl.h"
#include "gsm1xactvfile.h"


/*----------------------------------------------------------------------
 Defines
----------------------------------------------------------------------*/                                     
                   
#define  GSM1XACTVFILE_SYSTEM_PRL_FILE_NAME        "system.prl"
#define  GSM1XACTVFILE_USER_PRL_FILE_NAME          "user.prl"
#define  GSM1XACTVFILE_ICCID_FILE                  "iccid.bin"
#define  GSM1XACTVFILE_PRL_GEN_MODE_FILE           "prlgen.cfg"

             
/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
/* None.*/
/*----------------------------------------------------------------------
 Static Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Definitions
----------------------------------------------------------------------*/


/*=============================================================================
FUNCTION: gsm1xactvfile_GetValue 

DESCRIPTION:
   Copy the value from storage (EFS file) to the provided buffer.    

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
    None
=============================================================================*/
boolean gsm1xactvfile_GetValue
(
    IShell      *shellPtr,
    uint32       valueType, 
    uint32       valueMaxLen,
    char        *valuePtr, 
    uint32      *valueLen
)
{
   IFileMgr    *FileManager = NULL;
   IFile       *FilePtr     = NULL;
   int          bytesRead;

   boolean      retValue = FALSE;


   char         FileName[ GSM1XACTVFILE_EFS_ITEM_CNT ][ GSM1XACTVFILE_FILE_NAME_MAX ] = 
   {
       GSM1XACTVFILE_SYSTEM_PRL_FILE_NAME,
       GSM1XACTVFILE_USER_PRL_FILE_NAME,
       GSM1XACTVFILE_ICCID_FILE,
       GSM1XACTVFILE_PRL_GEN_MODE_FILE
   };
        

   do 
   {

       /* Validate argument(s).*/
       if( (NULL == valuePtr) || (NULL == valueLen) ){
           break;
       }
   
       if(NULL == shellPtr){
           DBGPRINTF("IShell ptr is NULL");
           break;
       }

   
       if((NULL == valuePtr) || (NULL == valueLen)){
           DBGPRINTF("One of the parameters is NULL");
           break;
       }

   
       if( 
            SUCCESS != ISHELL_CreateInstance
                       (
                          shellPtr, 
                          AEECLSID_FILEMGR, 
                          (void **)&FileManager
                      )
     
         ){
           DBGPRINTF("Could not create IFileMgr");
           break;
       }
      
   
       if(NULL == FileManager){
           DBGPRINTF("IFileMgr is NULL");
           break;
       }

   
       FilePtr = IFILEMGR_OpenFile
                 (
                     FileManager,
                     FileName[valueType],
                     _OFM_READ
                 );                                    

       if(NULL == FilePtr){
           DBGPRINTF("FilePtr is NULL");
           break;
       }
  
      
       bytesRead = IFILE_Read( FilePtr, valuePtr, valueMaxLen);
   
       if( bytesRead <= 0 ){
           DBGPRINTF("Read %d bytes",bytesRead);
           break;
       }

       *valueLen = bytesRead;

       retValue = TRUE;

   } while (FALSE);

       
   if(NULL != FilePtr){
       IFILE_Release( FilePtr );
   }

   
   if(NULL != FileManager){
       IFILEMGR_Release( FileManager );
   }
      
   return retValue;
}

/*=============================================================================
FUNCTION: gsm1xactvfile_SetValue 

DESCRIPTION:
   Set the provided value to a field in EFS file, creating a file
   if necessary.    

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
    None
=============================================================================*/
boolean gsm1xactvfile_SetValue
(
    IShell       *shellPtr,
    uint32        valueType, 
    char         *valuePtr, 
    uint32        valueLen
)
{
    IFileMgr    *FileManager = NULL;
    IFile       *FilePtr     = NULL;
    int          bytesWritten;
    
    char         FileName[ GSM1XACTVFILE_EFS_ITEM_CNT ][ GSM1XACTVFILE_FILE_NAME_MAX ] = 
    {
       GSM1XACTVFILE_SYSTEM_PRL_FILE_NAME,
       GSM1XACTVFILE_USER_PRL_FILE_NAME,
       GSM1XACTVFILE_ICCID_FILE,
       GSM1XACTVFILE_PRL_GEN_MODE_FILE
    };

    boolean      retValue = FALSE;


    do
    {
    
        /* Validation of argument(s).*/
        if( NULL == valuePtr ){
            break;
        }

        DBGPRINTF("Writing to file");

        if(NULL == shellPtr){
            DBGPRINTF("IShell ptr is NULL");
            break;
        }

        if(NULL == valuePtr){
            DBGPRINTF("One of parameters is NULL");
            break;
        }

        if(
            
              SUCCESS != ISHELL_CreateInstance
                         (
                            shellPtr, 
                            AEECLSID_FILEMGR, 
                            (void **)&FileManager
                         )
          ){
            DBGPRINTF("Could not create IFileMgr");
            break;
        }
      
        if(NULL == FileManager){
            DBGPRINTF("IFileMgr is NULL");
            break;
        }

        FilePtr = IFILEMGR_OpenFile
                  (
                     FileManager,
                     FileName[valueType],
                     _OFM_READWRITE
                  );                                    

        if(NULL == FilePtr){
            DBGPRINTF("File does not exist - create");

            FilePtr = IFILEMGR_OpenFile
                      (
                         FileManager,
                         FileName[valueType],
                        _OFM_CREATE
                      );

            if(NULL == FilePtr){
               DBGPRINTF("Could not create the file");
               break;
            }
        }
  
      
        bytesWritten = IFILE_Write( FilePtr, valuePtr, valueLen);
   
    
        if( bytesWritten <= 0 ){
            DBGPRINTF("Written %d bytes",bytesWritten);
            break;
        }

        retValue = TRUE;
    
    } while( FALSE );

    if(NULL != FilePtr){
       IFILE_Release( FilePtr );
    }

    if(NULL != FileManager){
       IFILEMGR_Release( FileManager );
    }

    return retValue;
}              
