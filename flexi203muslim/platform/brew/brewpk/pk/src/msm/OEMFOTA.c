#include "OEMFeatures.h"
#include "AEEFOTA.h"
#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEEFile.h"

// A reference implementation of OEMFOTA
#if (defined(FEATURE_IFOTA) && defined(REFERENCE_IFOTA_IMPL))

#ifndef RELEASEIF
#define RELEASEIF(pi)  do { if (pi) { IBASE_Release((IBase*)(pi)); (pi)=0; }} while(0)
#endif

#define FOTA_DELTA  "fota.delta"
#define FOTA_COMMIT "fota.commit"

struct IFOTA {
  const AEEVTBL(IFOTA) *pvt;

  uint32        m_nOpenType; // Screen or update package
  uint32        m_dwRefs;
  boolean       m_bOpen;
  IFile*        m_pFotaFile;
  IShell*       m_pShell;
};

// limit FOTA read/write to 16K at a time.
#define FOTA_REF_BLOCK_SIZE (16*1024)

static uint32 OEMFOTA_AddRef(IFOTA *me)
{
  me->m_dwRefs++;
  return me->m_dwRefs;
}

static uint32 OEMFOTA_Release(IFOTA *me)
{
  me->m_dwRefs--;
  if (!me->m_dwRefs) {
  
    RELEASEIF(me->m_pFotaFile);
    me->m_bOpen = FALSE;
    
    FREE(me);
    return 0;
  }

  return me->m_dwRefs;
}

static int OEMFOTA_QueryInterface(IFOTA* me, AEECLSID class, void** ppo)
{
  switch (class) {
  case AEECLSID_FOTA:
    *ppo = me;
    OEMFOTA_AddRef((IFOTA*)(*ppo));
    return SUCCESS;

  case AEEIID_FOTAFILE:
    *ppo = me->m_pFotaFile;
    IQI_AddRef(*ppo);
    return SUCCESS;

  default:
    *ppo = 0;
    return ECLASSNOTSUPPORT;
  }
}

static int OEMFOTA_Open(IFOTA *me, int openType)
{
   IFileMgr* fileMgr = 0;
   int nErr = SUCCESS;
   
   switch (openType)
   {
      case FOTA_AGENT_BITMAPS: //fall    
      case FOTA_AGENT_UPDATE_PACKAGE: //fall
      case FOTA_FS_UPDATE_PACKAGE:    
         me->m_nOpenType = openType;
         break;

      default:
         goto bail;
   }

   // create file manager.
   nErr = ISHELL_CreateInstance(me->m_pShell, AEECLSID_FILEMGR, (void**)&fileMgr);
   if ((SUCCESS != nErr) || (0 == fileMgr))
   { 
      if (SUCCESS == nErr)  {nErr = EFAILED;}
      goto bail;
   }
  
   // open file if it exists for read and write.
   // create and open the file if it does not exist for read and write
   me->m_pFotaFile = IFILEMGR_OpenFile(fileMgr, AEEFS_SHARED_DIR FOTA_DELTA, 
                        _OFM_READWRITE );
                        
   if (NULL == me->m_pFotaFile)
   {
      // file does not exist - create it
      me->m_pFotaFile = IFILEMGR_OpenFile(fileMgr, AEEFS_SHARED_DIR FOTA_DELTA, 
         _OFM_CREATE );
   }                     
   
   if (NULL == me->m_pFotaFile)
   {
      nErr = EFAILED;
      goto bail;
   }
   
   me->m_bOpen = TRUE;
   
   RELEASEIF(fileMgr);
   
   return SUCCESS;
   
bail:
   RELEASEIF(fileMgr);
   RELEASEIF(me->m_pFotaFile);
   
   return nErr;
}


static int32 OEMFOTA_Read(IFOTA *me, void * pBuffer, uint32 dwCount)
{
   switch (me->m_nOpenType)
   {
      case FOTA_AGENT_BITMAPS:
      case FOTA_AGENT_UPDATE_PACKAGE:
      case FOTA_FS_UPDATE_PACKAGE:
         break;
   }

   if (me->m_bOpen)
   {
      if (dwCount > FOTA_REF_BLOCK_SIZE)
      {
         dwCount = FOTA_REF_BLOCK_SIZE;
      }
      return IFILE_Read(me->m_pFotaFile, pBuffer, dwCount);
   }
   
   return 0;
  
}

static uint32 OEMFOTA_Write(IFOTA *me, const void * pBuffer, uint32 dwCount)
{
   switch (me->m_nOpenType)
   {
      case FOTA_AGENT_BITMAPS:
      case FOTA_AGENT_UPDATE_PACKAGE:
      case FOTA_FS_UPDATE_PACKAGE:
         break;
   }

   if (me->m_bOpen)
   {
      if (dwCount > FOTA_REF_BLOCK_SIZE)
      {
         dwCount = FOTA_REF_BLOCK_SIZE;
      }
      return IFILE_Write(me->m_pFotaFile, (void*) pBuffer, dwCount);
   }
   
   return 0;
}

static int OEMFOTA_Commit(IFOTA *me, void* pvOpenType)
{
   switch (me->m_nOpenType)
   {
      case FOTA_AGENT_UPDATE_PACKAGE:
      case FOTA_AGENT_BITMAPS:
      case FOTA_FS_UPDATE_PACKAGE:
         {
            IFileMgr *piFileMgr = 0;
            IFile *piFile = 0;
            int nErr;

            // create file manager.
            nErr = ISHELL_CreateInstance(me->m_pShell, AEECLSID_FILEMGR, (void**)&piFileMgr);
            if (SUCCESS == nErr) { 
               // open file if it exists for read and write.
               // create and open the file if it does not exist for read and write
               piFile = IFILEMGR_OpenFile(piFileMgr, AEEFS_SHARED_DIR FOTA_COMMIT, 
                                    _OFM_APPEND );
                                    
               if (0 == piFile) {
                  // file does not exist - create it
                  piFile = IFILEMGR_OpenFile(piFileMgr, AEEFS_SHARED_DIR FOTA_COMMIT, 
                     _OFM_CREATE );
               }                     
               
               if (0 != piFile) {
                  IFILE_Write(piFile, (void*) "success", STRLEN("success"));
               }
            }
                        
            RELEASEIF(piFile);
            RELEASEIF(piFileMgr);
            return SUCCESS;
         }
         
      default:
         return EFAILED;
   }
   
   RELEASEIF(me->m_pFotaFile);
   me->m_bOpen = FALSE;
   
   return SUCCESS;
}

static int OEMFOTA_GetStatus(IFOTA *me, char *pnStatus)
{
   return SUCCESS;
}

static int OEMFOTA_SetStatus(IFOTA* me, char nStatus)
{
   return SUCCESS;
}

static const VTBL(IFOTA) vtFOTA = {
   OEMFOTA_AddRef,
   OEMFOTA_Release,
   OEMFOTA_QueryInterface,
   OEMFOTA_Open,
   OEMFOTA_Read,
   OEMFOTA_Write,
   OEMFOTA_Commit,
   OEMFOTA_GetStatus,
   OEMFOTA_SetStatus
};

int OEMFOTA_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   IFOTA *me;

   me = MALLOC(sizeof(IFOTA));

   if ((IFOTA *) 0 == me) {
      return ENOMEMORY;
   }

   me->pvt = &vtFOTA;
   me->m_dwRefs = 1;
   me->m_bOpen = FALSE;
   me->m_pShell = piShell;

   *ppif = (void *)me;
   
   return SUCCESS;
}

// END of Reference implementation

// The code below has not been compiled or tested.
// The implementing OEM needs to make sure their implementation
// compiles and is tested.
#elif (defined(FEATURE_IFOTA) && !defined(REFERENCE_IFOTA_IMPL))

#include "AEEFile.h"
#include "AEEBase.h"

#include "fota_download_api.h"
#include "fs.h" // For File System Download Package


#define FOTA_FS_PKG_DIRNAME "/SWUOTA"
#define FOTA_FS_PKG_FILENAME "/SWUOTA/fotafspkg.up"


// temp defines for compilation

#define FOTA_OEM_INACTIVE 0
#define FOTA_OEM_ACTIVE 1

typedef struct CFile {
  AEEBASE_INHERIT(IFile, IFOTA);
} CFile;


struct IFOTA {

  const AEEVTBL(IFOTA) *pvt;

  CFile               iFile;

  uint32        openType; // Screen or update package
  uint32              dwRefs;
  int                 nFotaErr;
  int                 bOpen;
  fota_download_info  fpi;
  fs_handle_type      fileHandle;
  int                 nSeekOffset;
  int           max_size;
};


static uint32 OEMFOTA_AddRef(IFOTA *me)
{
  me->dwRefs++;
  return me->dwRefs;
}

static uint32 OEMFOTA_Release(IFOTA *me)
{
  me->dwRefs--;
  if (!me->dwRefs) {
    FREE(me);
    return 0;
  }

  return me->dwRefs;
}

static int OEMFOTA_QueryInterface(IFOTA* me, AEECLSID class, void** ppo)
{
  switch (class) {
  case AEECLSID_FOTA:
    *ppo = me;
    OEMFOTA_AddRef((IFOTA*)*ppo);
    return SUCCESS;

  case AEEIID_FOTAFILE:
    *ppo = &me->iFile;
    IQI_AddRef(*ppo);
    return SUCCESS;

  default:
    *ppo = 0;
    return ECLASSNOTSUPPORT;
  }
}


static int OEMFOTA_Open(IFOTA *me, int openType)
{
fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */
fs_open_xparms_type           foptions;   

me->openType = openType;

switch (openType)
{
  case FOTA_AGENT_BITMAPS:
    //Opening for downloading images...Need to set 
    //the languages available flag false
    me->nFotaErr = (int)fota_download_open(&me->fpi);
    fota_download_set_screens_available((int)FOTA_OEM_INACTIVE);
    me->nSeekOffset = me->fpi.ui_screen_tbl_offset;
    me->max_size = me->fpi.ui_screen_tbl_size;
    break;

  case FOTA_AGENT_UPDATE_PACKAGE:
    me->nFotaErr = (int)fota_download_open(&me->fpi);
    me->nSeekOffset = me->fpi.delta_start_offset;
    me->max_size = me->fpi.max_size;    
    break;

  case FOTA_FS_UPDATE_PACKAGE:

    // see if the directory exists. if not, create it.
    fs_nametest(FOTA_FS_PKG_DIRNAME,FS_TEST_DIR,NULL,&fs_rsp);
    if (fs_rsp.nametest.status == FS_OKAY_S && 
        fs_rsp.nametest.name_found == FALSE) {
      fs_mkdir(FOTA_FS_PKG_DIRNAME,NULL,&fs_rsp);
      if (fs_rsp.mkdir.status!=FS_OKAY_S) {
        DBGPRINTF("Failed to create fota dir");
        return FOTA_EFAILED;
      }
    }// if (fs_rsp.nametest.status == FS_OKAY_S && 


    fs_nametest(FOTA_FS_PKG_FILENAME,FS_TEST_FILE,NULL,&fs_rsp);
    if (fs_rsp.nametest.status == FS_OKAY_S && 
        fs_rsp.nametest.name_found == TRUE) {
      /* Here a decision has to be made...
         1. Continue from where we left off
         2. Delete the file and start afresh.
         Currently we will go for option 2, since resume 
         is heavily reliant on the download client's ability 
         to resume the session. With option 2, even if the client
         has the ability it has to buffer the entire file before 
         it opens the iFOTA interface and guarantees correctness
         in operation.
         Note: The update agent will delete the file after a 
         successful update operation.
      */
      fs_remove(FOTA_FS_PKG_FILENAME, NULL, &fs_rsp);
      if (fs_rsp.rmfile.status != FS_OKAY_S)
      {
        return EFAILED;
      }
    }// if (fs_rsp.nametest.status == FS_OKAY_S && 
    // create the file
    // Force the cleanup option to most simple...
    foptions.create.cleanup_option = FS_OC_CLOSE;
    foptions.create.buffering_option = FS_OB_ALLOW;
    foptions.create.attribute_mask = FS_FA_UNRESTRICTED;
    fs_open(FOTA_FS_PKG_FILENAME, FS_OA_CREATE, &foptions, NULL, &fs_rsp);

    if ((me->fileHandle = fs_rsp.open.handle) == NULL || fs_rsp.any.status != FS_OKAY_S)
    {
      me->nFotaErr = (int)FOTA_EFAILED;
    }
    me->nSeekOffset = 0; //Start of the file;
    me->max_size = fs_space_available();       //We have the full EFS at our disposal...
    break;
} // switch ...

if (me->nFotaErr == (int)FOTA_SUCCESS) {
  me->bOpen = TRUE;
  return SUCCESS;
}

me->bOpen = FALSE;
return EFAILED;
}


static int32 OEMFOTA_Read(IFOTA *me, void * pBuffer, uint32 dwCount)
{
  int nRead;
  fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */

  if (!me->bOpen)
    return 0;




  switch (me->openType)
  {
  case FOTA_AGENT_BITMAPS:
  case FOTA_AGENT_UPDATE_PACKAGE:
    if (me->nSeekOffset + dwCount > me->max_size)
      return 0;
    nRead = fota_download_read(me->nSeekOffset, pBuffer, dwCount);
    if (nRead > 0){
      me->nSeekOffset += nRead;
      return nRead;
    }

    me->nFotaErr = nRead; //nRead <= 0. But a FOTA_SUCCESS is a 0!!!!
    break;
  case FOTA_FS_UPDATE_PACKAGE:
    /* Since we are reading from a file, we need to make sure we don't 
       go beyond the offset
    */
    fs_file_size(FOTA_FS_PKG_FILENAME, NULL, &fs_rsp);
    if ((fs_rsp.file_size.status != FS_OKAY_S) ||
        (me->nSeekOffset + dwCount > fs_rsp.file_size.size ))
      return 0;

    /* For safety we will always move the FILE pointer to nSeekOffset*/

    fs_seek (me->fileHandle, FS_SEEK_SET, me->nSeekOffset, NULL, &fs_rsp);
    if (fs_rsp.seek.status != FS_OKAY_S)
    {
      return 0;
    }

    fs_read(me->fileHandle,pBuffer,dwCount,NULL,&fs_rsp);
    if (fs_rsp.read.count > 0)
    {
      me->nSeekOffset += fs_rsp.read.count;
      return fs_rsp.read.count;
    }
    /*End of FOTA_FS_UPDATE_PACKAGE */

  } //End of case(openType)



  return 0;
}



static uint32 OEMFOTA_Write(IFOTA *me, const void * pBuffer, uint32 dwCount)
{
  int nWritten;
  fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */


  if (!me->bOpen)
    return 0;

  if (me->nSeekOffset + dwCount > me->max_size)
    return 0;


  switch (me->openType)
  {
  case FOTA_AGENT_BITMAPS:
  case FOTA_AGENT_UPDATE_PACKAGE:
    nWritten = fota_download_write(me->nSeekOffset, 
                                   (unsigned char *)pBuffer, dwCount);

    if (nWritten > 0) {
      me->nSeekOffset += nWritten;
      return nWritten;
    }

    me->nFotaErr = nWritten;
    break;
  case FOTA_FS_UPDATE_PACKAGE:

    /* For safety we will always move the FILE pointer to nSeekOffset*/

    fs_seek (me->fileHandle, FS_SEEK_SET, me->nSeekOffset, NULL, &fs_rsp);
    if (fs_rsp.seek.status != FS_OKAY_S)
    {
      return 0;
    }
    fs_write(me->fileHandle,pBuffer,dwCount,NULL,&fs_rsp);
    if (fs_rsp.write.count > 0)
    {
      me->nSeekOffset += fs_rsp.write.count;
      return fs_rsp.write.count;
    }
    /*End of FOTA_FS_UPDATE_PACKAGE */
    me->nFotaErr = fs_rsp.write.count; // To maintain uniformity between FOTA and FS blocks
  }//End of case(openType)

  return 0;
}


static int OEMFOTA_Commit(IFOTA *me)
{

  fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */

  if ( ! me->nSeekOffset) {
    return EFAILED;
  }

  switch (me->openType)
  {
  case FOTA_AGENT_UPDATE_PACKAGE:
    fota_download_finished(me->nSeekOffset);
    return SUCCESS;

  case FOTA_AGENT_BITMAPS:
    fota_download_set_screens_available((int)FOTA_OEM_ACTIVE);
    return SUCCESS;
  case FOTA_FS_UPDATE_PACKAGE:
    fs_close(me->fileHandle, NULL, &fs_rsp);
    return SUCCESS;
  default:
    return EFAILED;
   break;
   
  }

}

static int OEMFOTA_GetStatus(IFOTA *me, int *pnStatus)
{
  // fota_image_error_t errFlag;
  int status;
  me->nFotaErr =(int)fota_download_get_update_status(&pnStatus);
  if (me->nFotaErr == (int)FOTA_SUCCESS)
  {
    //Translate the status to syncML Specified status and update *pnStatus
    //OEMFOTA_TranslateStatus(int status, int *pnStatus);
    return SUCCESS;
  }

  else
    return EFAILED;
}
static int OEMFOTA_SetStatus(IFOTA* me, int nStatus)
{
  //fota_image_error_t errFlag;
  me->nFotaErr = (int)fota_download_set_update_status(nStatus); 
  if (me->nFotaErr  == (int)FOTA_SUCCESS)
    return SUCCESS;
  else
    return EFAILED;
}



static void       CFile_Readable(IFile *pf, PFNNOTIFY pfn,void * pUser);
static int32      CFile_Read(IFile *pf, void * pBuffer, uint32 dwCount);
static void       CFile_Cancel(IFile *pf, PFNNOTIFY pfn, void * pUser);
static uint32     CFile_Write(IFile *pf, PACKED const void * pBuffer, uint32 dwCount);
static int        CFile_GetInfo(IFile *pf,FileInfo * pInfo);
static int        CFile_GetInfoEx(IFile *pf,AEEFileInfoEx * pInfo);
static int32      CFile_Seek(IFile *pf, FileSeekType seek, int32 position);
static int        CFile_Truncate(IFile *pf, uint32 truncate_pos);
static int32      CFile_SetCacheSize(IFile *pf, int nSize);
static void *     CFile_Map(IFile *pf,void * pStart, uint32 dwSize, int protections, int flags, uint32 dwOffset);


static void CFile_Readable(IFile *pf, PFNNOTIFY pfn,void * pUser) 
{}

static void CFile_Cancel(IFile *pf, PFNNOTIFY pfn, void * pUser)  
{}

static int  CFile_GetInfo(IFile *pf,FileInfo * pInfo) 
{ return EUNSUPPORTED;}

static int  CFile_GetInfoEx(IFile *pf,AEEFileInfoEx * pInfo) 
{ return EUNSUPPORTED;}

static int32 CFile_SetCacheSize(IFile *pf, int nSize)
{ return SUCCESS;}

static void * CFile_Map(IFile *pf, void *pStart, uint32 dwSize, int prot, int flags, uint32 dwOffset)
{ return NULL;}





static int32 CFile_Read(IFile *pf, void * pBuffer, uint32 dwCount)
{
  return OEMFOTA_Read(((CFile *)pf)->pMe, pBuffer, dwCount);
}


static uint32 CFile_Write(IFile *pf, PACKED const void * pBuffer, uint32 dwCount)
{
  return OEMFOTA_Write(((CFile *)pf)->pMe, (const void *)pBuffer, dwCount);
}

static int32 CFile_Seek(IFile *pf, FileSeekType seek, int32 position)
{
  IFOTA * me     = ((CFile *)pf)->pMe;
  int     newPos = 0;
  fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */
  fs_seek_origin_type fs_seek_type = FS_SEEK_SET;

  if (!me->bOpen)
    return EFAILED;
  switch (me->openType)
  {
  case FOTA_AGENT_BITMAPS:
  case FOTA_AGENT_UPDATE_PACKAGE:
    if (seek == _SEEK_CURRENT) {
        newPos = me->nSeekOffset + position;
    }
    else if (seek == _SEEK_START) {
      // in this case, move to "position" from start (0)
      if (position > 0) {
      
        newPos = position;
      }
      else
      {
        // make negative positions 0
        newPos = 0;
      }
      
    }
    else {
      // _SEEK_END not supported
      return EFAILED;
    }

    if (newPos > me->max_size)
      return EFAILED;

    me->nSeekOffset = newPos;
    break;
  case FOTA_FS_UPDATE_PACKAGE:


    if (seek == _SEEK_CURRENT) {
      fs_seek_type = FS_SEEK_CURRENT;
    }
    else if (seek == _SEEK_START) {
      fs_seek_type = FS_SEEK_SET;
    }

    fs_seek (me->fileHandle, fs_seek_type, position, NULL, &fs_rsp);
    if (fs_rsp.seek.status != FS_OKAY_S)
    {
      return EFAILED;
    }
    fs_tell (me->fileHandle, NULL, &fs_rsp);

    if (fs_rsp.tell.status == FS_OKAY_S)
    {
      newPos = fs_rsp.tell.position;
    }

    me->nSeekOffset = newPos;
    /*end of case FOTA_FS_UPDATE_PACKAGE */
  }//end of switch(openType)
  return SUCCESS;
}

static int CFile_Truncate(IFile *pf, uint32 truncate_pos)
{
  IFOTA * me = ((CFile *)pf)->pMe;
  fs_rsp_msg_type            fs_rsp;  /* Response from FS task             */
  switch (me->openType)
  {
  case FOTA_AGENT_UPDATE_PACKAGE:
    fota_download_finished(me->nSeekOffset);
    return SUCCESS;
  case FOTA_AGENT_BITMAPS:
    fota_download_set_screens_available((int)FOTA_OEM_ACTIVE);
    return SUCCESS;
  case FOTA_FS_UPDATE_PACKAGE:
    fs_close(me->fileHandle, NULL, &fs_rsp);
    return SUCCESS;
  }

}

static const VTBL(IFOTA) vtFOTA = {
  OEMFOTA_AddRef,
  OEMFOTA_Release,
  OEMFOTA_QueryInterface,
  OEMFOTA_Open,
  OEMFOTA_Read,
  OEMFOTA_Write,
  OEMFOTA_Commit,
  OEMFOTA_GetStatus,
  OEMFOTA_SetStatus,
};

/*
int FOTAFile_New(IShell *piShell, AEECLSID cls, void **ppif)
{
   int nErr;
   IFOTA *piFota = 0;
   
   nErr = IFOTA_New(piShell, AEECLSID_FOTA, (void **)&piFota);
   if (SUCCESS == nErr) {
      nErr = IFOTA_QueryInterface(piFota, cls, ppif);
      IFOTA_Release(piFota);
   }

   return nErr;
}

*/
int IFOTA_New(IShell *piShell, AEECLSID cls, void **ppif)
{
  //default is for the update package. So the screens app should make
  // sure that he calls the IFOTA_OPEN to set the offset correctly

  IFOTA *me;

  me = MALLOC(sizeof(IFOTA));
  if (me) {
    me->pvt = &vtFOTA;
    me->dwRefs = 1;

    {
      static const IFileVtbl vtIFile = {
        AEEBASE_AddRef(IFile),
        AEEBASE_Release(IFile),
        CFile_Readable,
        CFile_Read,
        CFile_Cancel,
        CFile_Write,
        CFile_GetInfo,
        CFile_Seek,
        CFile_Truncate,
        CFile_GetInfoEx,
        CFile_SetCacheSize,
        CFile_Map
      };

      me->iFile.pvt = &vtIFile;
      me->iFile.pMe = me;
    }

    /*
      Must call iFOTA_Open(..) to open the right stream :FOTA or FS

      
    me->nFotaErr = fota_download_open(&me->fpi);
    if(me->nFotaErr == FOTA_SUCCESS)
  {
        me->openType = FOTA_AGENT_UPDATE_PACKAGE;
    me->nSeekOffset = me->fpi.delta_start_offset;
    me->max_size = me->fpi.max_size;
    me->bOpen = TRUE;
  }
    else
  {
      me->bOpen = FALSE;
  }

  */


    me->bOpen = FALSE;

    *ppif = (void *)me;
  }

  return SUCCESS;
}

#endif /* FEATURE_IFOTA && !REFERENCE_IFOTA_IMPL */
