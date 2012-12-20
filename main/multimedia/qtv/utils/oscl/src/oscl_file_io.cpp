/* =======================================================================
                               oscl_file_io.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation;
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated;
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/src/oscl_file_io.cpp#15 $
$DateTime: 2009/10/05 01:23:08 $
$Change: 1044203 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/*
    This header file contains OS specific functions necessary for
    the File Format library to compile and run on different OS's.
*/

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_file_io.h"

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif

#ifdef FEATURE_QTV_DRM_DCF
 #include "IxStreamFactory.h"
#endif

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

#ifdef FEATURE_QTV_DRM_DCF
/* ======================================================================
FUNCTION
  OSCL_FileOpen

DESCRIPTION
  To wrap IxStream for supporting DCF playback.

DEPENDENCIES
 IxStream/DRM suite

INPUT PARAMETERS:
 ->inputStream:IxStream* for DCF media

RETURN VALUE
 <-OSCL_FILE*

SIDE EFFECTS

========================================================================== */
OSCL_FILE * OSCL_FileOpen(dcf_ixstream_type inputStream)
{
   OSCL_FILE *fp = QTV_New( OSCL_FILE );
   if (fp != NULL)
   {
      memset(fp, 0x0, sizeof(OSCL_FILE));

      //ToDo: add File-IO-Cache support for IxStream
      //Set Stream type to be DRM_IXSTREAM
      fp->streamType  = DRM_IXSTREAM;
      fp->inputStream = inputStream;
   }

   return fp;
}
#endif

/* ======================================================================
FUNCTION
  OSCL_FileOpen

DESCRIPTION
  To open the file and set the read cache buffer size with the given value
  or with the default value.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
OSCL_FILE *OSCL_FileOpen (const OSCL_STRING &filename, const OSCL_TCHAR *mode,
                          int32 bufferSize )
{
  OSCL_FILE     *fp   = NULL;  
  EFS_FILE      *file = NULL;
  /* mapping the mode "c" to "w", as EFs2 doesn't have case to handle "c"
     mode */
  if (strcmp(mode, "c") == 0)
  {
    file = efs_fopen(filename.get_cstr(),_T("w"));
  }
  else
  {
    file = efs_fopen(filename.get_cstr(),mode);
  }

  if (file == NULL)
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR,"efs_open failed .Efs Error No:%d",efs_errno);
  }
  else
  {
    fp = QTV_New( OSCL_FILE );
    if (fp == NULL)
    {
      QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "OSCL_File Open failed" );
      efs_fclose(file);
    }
    else
    {
      memset(fp,0,sizeof(OSCL_FILE));
      fp->efsHandle = file;
	  fp->bBufValid = false;
      fp->bPullBufValid = false;
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_FILE_IO_CACHING
      if(( strcmp(mode, "r") == 0 ) || ( strcmp(mode, "rb") == 0 ))
      {
        fp->cacheValid = 1;
        if (bufferSize > 0)
        {
          /* Allocate required buffer */
          fp->readBuffSize = bufferSize;
        }
        else
        {
          /* if the required size is less than zero allocate with default size */
          fp->readBuffSize = FILE_READ_BUFFER_DEFAULT_SIZE;
        }
        /* Allocate required buffer */
        fp->readBuffer = (uint8 *)QTV_Malloc( fp->readBuffSize );
        if (fp->readBuffer == NULL)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileOpen, no enough memory" );
          fp->cacheValid = 0;
        }
        fp->readBuffPos = -1;
        fp->fileReadPos = -1;
        fp->fileSize = 0;
        //Determine the file-size
        fp->fileSize = OSCL_FileSize( filename );
      }
#endif //FEATURE_QTV_FILE_IO_CACHING
    }    	
  }
  return fp;
}

/* ======================================================================
FUNCTION
  OSCL_FileDelete

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool OSCL_FileDelete(const OSCL_STRING &filename)
{
  if( efs_unlink(filename.get_cstr() ) < 0 )
  {
    QTV_MSG_SPRINTF_PRIO_1( QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR,"Failed to Delete file --> %s",
                            filename.get_cstr() );
    return false;
  }
  else
  {
    QTV_MSG_SPRINTF_PRIO_1( QTVDIAG_GENERAL, QTVDIAG_PRIO_HIGH,"Deleted file --> %s ",
                            filename.get_cstr() );
    return true;
  }  
}


bool OSCL_FileRename (
    const char                  *old_filename,     /* Current name of file */
    const char                  *new_filename     /* New name of file */
)
{  
  if( efs_rename( old_filename, new_filename ) < 0 )
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileRename Failed" );
    return false;
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileRename Success" );    
  return true;
}

/* ======================================================================
FUNCTION
  OSCL_FileOpen

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
OSCL_FILE *OSCL_FileOpen(unsigned char *pBuf, uint32 bufSize,
                         QtvPlayer::FetchBufferedDataT FetchBufferedData, uint32 trackId, QtvPlayer::InstanceHandleT handle)
{
  OSCL_FILE *fp = NULL;

  if ( (pBuf && bufSize ) || FetchBufferedData)
  {
    fp = QTV_New( OSCL_FILE );
    if(fp == NULL) {
       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_File Open failed" );
       return NULL;
    }
    memset( fp, 0, sizeof(OSCL_FILE) );
    if(FetchBufferedData)
    {
      fp->bPullBufValid = true;
      fp->pullBuf.FetchBufferedData = FetchBufferedData;
      fp->pullBuf.handle = handle;
      fp->pullBuf.curPos = 0;
      fp->pullBuf.trackId = trackId;
      fp->bBufValid = false;
    }
    else
    {
      fp->bBufValid = true;
      fp->bPullBufValid = false;
      fp->memBuf.pBuf = pBuf;
      fp->memBuf.bufSize = bufSize;
      fp->memBuf.curPos = 0;
    }
    //fp->hFile = 0;
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
  }
  return fp;
}

#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

/* ======================================================================
FUNCTION
  OSCL_IsFileOpen

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool OSCL_IsFileOpen(OSCL_FILE *fp)
{
  if( !fp )
    return false;

  if( fp->efsHandle || fp->bBufValid || fp->bPullBufValid
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif
    )
      return true;
#ifdef FEATURE_QTV_DRM_DCF
  else
  {
    switch (fp->streamType)
    {
    case DRM_IXSTREAM:
      //For IxStream input, it is always open!
      return true;
    }
  }
#endif
  return false;
}

/* ======================================================================
FUNCTION
  OSCL_FileRead

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
uint32  OSCL_FileRead (void *buffer, uint32 size,
                       uint32 numelements,  OSCL_FILE *fp)
{
  int32  numRead=0;

  /* sanity check */
  if (!fp || !buffer || !(size*numelements))
    return 0;

  if( fp->efsHandle )
  {
    //fs_rsp_msg_type rsp;
#ifdef FEATURE_QTV_FILE_IO_CACHING
    if (fp->cacheValid)
    {
      uint32 reqSizeToRead = size*numelements;

      /* sanity check on local read buffer */
      if (fp->readBuffer == NULL)
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileOpen, local read buffer is invalid" );
        return 0;
      }

      /* EOF check */
     if (fp->fileReadPos > fp->fileSize)
      {
        QTV_MSG_PRIO2(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileRead, Invalid fileReadPos fp->fileReadPos >= fp->fileSize fp->fileReadPos  %d   fp->fileSize %d", fp->fileReadPos, fp->fileSize);
        return 0;
      }
      if ((fp->fileReadPos + reqSizeToRead) >= fp->fileSize)
      {
        reqSizeToRead = fp->fileSize - fp->fileReadPos - 1;
      }

      /* if the read buffer is valid */
      if (fp->readBuffPos != -1)
      {
        //Read from the local buffer if the sufficient data is available.
        if ((fp->readBuffSize - (uint32)fp->readBuffPos) >= reqSizeToRead)
        {
          //Read from the local buffer.
          memcpy(buffer, fp->readBuffer+fp->readBuffPos, reqSizeToRead);
          numRead = reqSizeToRead;
          fp->fileReadPos += reqSizeToRead;
          fp->readBuffPos += reqSizeToRead;
        }
        else
        {
		  //Seek the original fptr to the correct location within the file.                    
          if( efs_fseek ( fp->efsHandle, fp->fileReadPos+1, SEEK_SET ) < 0 )
          {
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fseek failed. FS Status=%d", efs_errno );
          }
          else
          {              
            numRead = efs_fread ( buffer, 1, reqSizeToRead, fp->efsHandle);
            fp->fileReadPos += numRead;
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "No of Bytes read from EFS=%d", numRead);
          }                                            
          //Declaring that the read-buffer is empty.
          fp->readBuffPos = -1;
        }
      }
      else
      {
        //Read into the buffer OR read directly into the supplied buff.
        if ((reqSizeToRead) > (fp->readBuffSize>>2))
        {                                
          numRead = efs_fread ( buffer, 1, reqSizeToRead, fp->efsHandle);
          fp->fileReadPos += numRead;
          QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "No of Bytes read from EFS=%d", numRead);
          //Declaring that the read-buffer is empty.
          fp->readBuffPos = -1;                    
        }
        else
        {
          //Read directly from the file into Local buffer, filling it.                      
          numRead = efs_fread ( fp->readBuffer, 1, fp->readBuffSize, fp->efsHandle);
          if( numRead > 0 )
          {
            reqSizeToRead = MIN(reqSizeToRead,numRead);
            if(reqSizeToRead)
            {
              memcpy(buffer, fp->readBuffer, reqSizeToRead);
              numRead = reqSizeToRead;
              fp->fileReadPos += numRead;
              fp->readBuffPos += (reqSizeToRead+1);
            }                       
            QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "No of Bytes read from EFS=%d", numRead);                      
          }
        }
      }
    }
    else
#endif //FEATURE_QTV_FILE_IO_CACHING
    {
      numRead = efs_fread ( buffer, 1, size*numelements, fp->efsHandle);
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_MED, "No of Bytes read from EFS=%d", numRead);    
    }
  }
  else if (fp->bBufValid)
  {
    int32 sizeToRead = MIN((fp->memBuf.bufSize-fp->memBuf.curPos), size*numelements);
    if (sizeToRead > 0)
    {
      memcpy(buffer, (fp->memBuf.pBuf+fp->memBuf.curPos), sizeToRead);
      fp->memBuf.curPos = fp->memBuf.curPos + sizeToRead;
      numRead = sizeToRead;
    }
  }
  else if (fp->bPullBufValid)
  {
    //Read from the OEM buffer using the callback
    //This function will only be used while parsing in the Palyer thread context.

    numRead = (*fp->pullBuf.FetchBufferedData)(buffer, size*numelements, fp->pullBuf.curPos, fp->pullBuf.trackId, fp->pullBuf.handle);
    fp->pullBuf.curPos += numRead;
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF
  else
  {
    switch (fp->streamType)
    {
    case DRM_IXSTREAM:
#ifdef FEATURE_QTV_DRM_DCF_TEST
      if (fp->inputStream >= 0)
      {
        numread = efs_read(*fp->inputStream,buffer, size*numelements);
      }
#elif defined(FEATURE_QTV_DRM_DCF)
      //Read from IxStream
      uint32 reqSizeToRead = size*numelements;
      numRead = 0;
      bool   isEndofStream = false;

      if (fp->inputStream != NULL)
      {
        IxErrnoType errorCode = E_FAILURE;
        errorCode =  ((IxStream*)fp->inputStream)->Read((byte*)buffer,
                                                        (uint32)reqSizeToRead,
                                                        (uint32*)&numRead,
                                                        &isEndofStream);

        if (isEndofStream == true)
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "****$$$$OSCL_FileRead DETECTED isEndofStream TRUE$$$$$****");
        }

        if ((errorCode == E_SUCCESS) && (numRead == 0))
        {
          QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "??????OSCL_FileRead detected DATA_UNDERRUN");
        }
        else if (errorCode != E_SUCCESS)
        {
          //read failure,
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileRead failed..error code %d",errorCode );
        }
      }
#endif
      break;
    }
  }
#endif
  return(numRead);
}

/* ======================================================================
FUNCTION
  OSCL_FileSeekRead

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
uint32 OSCL_FileSeekRead ( void *buffer, uint32 size, uint32 numelements,
                          OSCL_FILE *fp,  int32 offset, int32 origin )
{
  uint32  numRead = 0;

  /* sanity check */
  if( !fp || !buffer || !(size*numelements) )
    return 0;

  if (fp->efsHandle)
  {    
#ifdef FEATURE_QTV_FILE_IO_CACHING
    if(fp->cacheValid)
    {
      int32 fpReadPos = fp->fileReadPos;
      int32 rdBuffPos = fp->readBuffPos;

      switch ( origin )
      {
	    case SEEK_SET:
   
        //SEEK_SET - Set to 'offset'
        /* if the read buffer is valid */
        if(fp->readBuffPos != -1)
        {
          /* if the offset is greater than the current file read position
          and less than the maximum valid read buffer */
          if((offset > fp->fileReadPos) && (offset<= (fp->fileReadPos + fp->readBuffSize - fp->readBuffPos)))
          {
            fp->readBuffPos += (offset - fp->fileReadPos -1);
          }
          /* if the offset less than/equal to file read position and also is greater
          than or equal to the minimum valid read buffer */
          else if((offset<= fp->fileReadPos) && (offset>((fp->fileReadPos - fp->readBuffPos) + 1 )))
          {
            fp->readBuffPos = offset - (( fp->fileReadPos - fp->readBuffPos )+ 1);
          }
          /* if the offset is not within the range of valid read buffer then flush the read buffer */
          else
          {
            fp->readBuffPos = -1;
          }
        }
        fp->fileReadPos = offset-1;
        break;

        case SEEK_CUR:
   
        //SEEK_CUR - Set to 'offset' + current position
        if(fp->readBuffPos != -1)
        {
          if(offset < 0)
          {
            fp->readBuffPos = -1;
          }
          else if((fp->readBuffPos + offset) >= fp->readBuffSize)
          {
            fp->readBuffPos = -1;
          }
          else
          {
            fp->readBuffPos += offset;
          }
        }
        fp->fileReadPos += offset;
        break;

        case SEEK_END:
   
        //SEEK_END - Set to 'offset' + file size
        fp->fileReadPos = fp->fileSize + offset -1;
        fp->readBuffPos = -1;
        break;

        default:
        /* This is an error.
        */
        return 0;
      }      
	  if( efs_fseek ( fp->efsHandle, offset, origin ) < 0 )
      {
        QTV_MSG_PRIO1( QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fseek failed. FS Status=%d", efs_errno );
        fp->fileReadPos = fpReadPos;
        fp->readBuffPos = rdBuffPos;
      }
      else
      {                    
        numRead = OSCL_FileRead(buffer, size, numelements, fp);
      }
    }
    else
#endif /* FEATURE_QTV_FILE_IO_CACHING */
    {            
	  if( efs_fseek ( fp->efsHandle, offset, origin ) < 0 )
      {
        QTV_MSG_PRIO1( QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fseek failed. FS Status=%d", efs_errno );          
      }
      else
      {          
        numRead = efs_fread ( buffer, size, numelements, fp->efsHandle );
      } 
	}
  }
  else if ( fp->bBufValid )
  {
    int32 npos;
    switch ( origin )
    {
    case SEEK_CUR:
      npos = offset + fp->memBuf.curPos;
      break;

    case SEEK_END:
      npos = offset + fp->memBuf.bufSize;
      break;

    case SEEK_SET:
      npos = offset;
      break;

    default:
      npos = (-1); /* error */
      break;
    }
    if ( (npos>=0) && (npos<=(int32)fp->memBuf.bufSize) )
    {
      fp->memBuf.curPos = npos;
      /* now read from this location */
      int32 sizeToRead = MIN((fp->memBuf.bufSize-fp->memBuf.curPos), size*numelements);
      if ( sizeToRead > 0 )
      {
        memcpy(buffer, (fp->memBuf.pBuf+fp->memBuf.curPos), sizeToRead);
        fp->memBuf.curPos = fp->memBuf.curPos + sizeToRead;
        numRead = sizeToRead;
      }
    }
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
  else if(fp->bPullBufValid)
  {
    //Read from the OEM buffer using the callback
    if(origin == SEEK_CUR)
    {
      fp->pullBuf.curPos += offset;
    }
    else if(origin == SEEK_SET)
    {
      fp->pullBuf.curPos = offset;
    }

    if(origin != SEEK_END)
    {
      numRead = (*fp->pullBuf.FetchBufferedData)(buffer, size*numelements, fp->pullBuf.curPos, fp->pullBuf.trackId, fp->pullBuf.handle);
      fp->pullBuf.curPos += numRead;
    }
    else
    {
        //SEEK_END is invalid as the buffer size is unknown.
        QTV_ASSERT(0);
    }
  }
#ifdef FEATURE_QTV_DRM_DCF
  else
  {
    switch (fp->streamType)
    {
      case DRM_IXSTREAM:
        #ifdef FEATURE_QTV_DRM_DCF_TEST
        if (fp->inputStream >= 0)
        {
          int                      whence;

          switch ( origin )
          {
            case SEEK_SET:
            whence = SEEK_SET;
            break;

            case SEEK_CUR:
            whence = SEEK_CUR;
            break;

            case SEEK_END:
            whence = SEEK_END;
            break;

            default:
            /* This is an error.
            */
            return 0;
          }
          if (efs_lseek(*fp->inputStream, offset, whence) >= 0)
          {
            numRead = efs_read (*fp->inputStream, buffer, size*numelements);
          }
          else
          {
            numRead = 0;
          }
        }
        #elif defined(FEATURE_QTV_DRM_DCF)
          // Seek IxStream then Read from IxStream

          IxStreamWhenceType fs_origin;
          switch ( origin )
		      {
		        case SEEK_SET:
		          fs_origin = IX_STRM_SEEK_START;
		          break;

		        case SEEK_CUR:
		          fs_origin = IX_STRM_SEEK_CURRENT;
		          break;

		        case SEEK_END:
		          fs_origin = IX_STRM_SEEK_END;
		          break;

		        default:
		        // This is an error
		          return 0;
          }
          if(fp->inputStream != NULL)
          {
		        IxErrnoType errorCode = E_FAILURE;
			      errorCode = ((IxStream*)fp->inputStream)->Seek(offset,fs_origin);
			      if(errorCode == E_SUCCESS)
			      {
		          uint32 reqSizeToRead = size*numelements;
			        numRead = 0;
			        bool   isEndofStream = false;
			        errorCode = E_FAILURE;

			        errorCode =  ((IxStream*)fp->inputStream)->Read((byte*)buffer,
                                                              (uint32)reqSizeToRead,
                                                              &numRead,
                                                              &isEndofStream);
			       if( (errorCode == E_SUCCESS) && (numRead == 0) )
			       {
				       QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileRead detected DATA_UNDERRUN");
			       }
			       else if(errorCode != E_SUCCESS)
			       {
			         //read failure,
				       QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileRead failed..error code %d",errorCode );
			       }
			      }
			      else
			      {
			        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileSeekRead:IxStream->Seek failed..error code %d",errorCode );
			      }
		      }
        #endif
        break;
    }
  }
#endif

  return(numRead);
}

/* ======================================================================
FUNCTION
  OSCL_FileSeekWrite

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
uint32 OSCL_FileSeekWrite (OSCL_FILE *fp, int32 origin, int32 offset,
                           void *buffer, uint32 size, uint32 numelements)
{
   uint32 numWritten = 0;
   // sanity check
   if( !fp || !buffer || !(size*numelements) )
     return 0;
   
  if( fp->efsHandle )
  {
    if( !( efs_fseek ( fp->efsHandle, offset, origin ) < 0 ) )       
    {      
      numWritten = efs_fwrite (buffer, size, numelements, fp->efsHandle);
#ifdef FEATURE_QTV_FILE_IO_CACHING
      fp->fileSize += numWritten;
#endif    
    }
  }  
  return numWritten;
}

/* ======================================================================
FUNCTION
  OSCL_FileWrite

DESCRIPTION
  Functions seeks to the file offset specified and write 
  the data in the buffer starting from that offset.

INPUT/OUTPUT
  fp          = file pointer
  buffer      = buffer from which to write
  numelements = number of bytes to be written into the file from buffer
  size        = size of the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  bytes written into file
  0 in case of error

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
uint32  OSCL_FileWrite (void *buffer, uint32 size,
                        uint32 numelements,  OSCL_FILE *fp)
{
  uint32  numWritten=0;

  /* sanity check */
  if (!fp || !buffer || !(size*numelements))
    return 0;
  
  if (fp->efsHandle)
  {
    uint32 reqSizeToWrite = size*numelements;
    /* It seems Qtv oscl file wrappers are written under the assumption that efs calls (like efs_fwrite) return absolute bytes. 
    ** But efs calls are returning number of elements (absolute bytes/ element size).
    ** So this code change resolves this inconsistency and size should be 1
    */
    numWritten = efs_fwrite (buffer, 1, reqSizeToWrite, fp->efsHandle);
    if( numWritten == 0 )
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fwrite failed. FS Status=%d", efs_errno );
    }
    else
      fp->fileSize += numWritten;
  }
  else if (fp->bBufValid)
  {
    int32 sizeToWrite = MIN((fp->memBuf.bufSize-fp->memBuf.curPos), size*numelements);
    if (sizeToWrite > 0)
    {
      memcpy((fp->memBuf.pBuf+fp->memBuf.curPos), buffer, sizeToWrite);
      fp->memBuf.curPos = fp->memBuf.curPos + sizeToWrite;
      numWritten = sizeToWrite;
    }
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
  
  return(numWritten);
}

/* ======================================================================
FUNCTION
  OSCL_FileSeek

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 OSCL_FileSeek (OSCL_FILE *fp,  int32 offset, int32 origin)
{
  int32  nReturn = (-1);

  /* sanity check */
  if (!fp)
    return nReturn;
  
  if (fp->efsHandle)
  {      
#ifdef FEATURE_QTV_FILE_IO_CACHING
    if (fp->cacheValid)
    {
      int32 fpReadPos = fp->fileReadPos;
      int32 rdBuffPos = fp->readBuffPos;

	  switch (origin)
      {
        case SEEK_SET:        
        //SEEK_SET - Set to 'offset'
        /* if the read buffer is valid */
        if (fp->readBuffPos != -1)
        {
          /* if the offset is greater than the current file read position
           and less than the maximum valid read buffer */
          if ((offset > fp->fileReadPos) && (offset<= (fp->fileReadPos + fp->readBuffSize - fp->readBuffPos)))
          {
            fp->readBuffPos += (offset - fp->fileReadPos -1);
          }
          /* if the offset less than/equal to file read position and also is greater
          than or equal to the minimum valid read buffer */
          else if ((offset<= fp->fileReadPos) && (offset>((fp->fileReadPos - fp->readBuffPos) + 1 )))
          {
            fp->readBuffPos = offset - (( fp->fileReadPos - fp->readBuffPos )+ 1);
          }
          /* if the offset is not within the range of valid read buffer then flush the read buffer */
          else
          {
            fp->readBuffPos = -1;
          }
        }
        fp->fileReadPos = offset-1;
        break;

        case SEEK_CUR:          
        //SEEK_CUR - Set to 'offset' + current position
        if (fp->readBuffPos != -1)
        {
          if (offset < 0)
          {
            fp->readBuffPos = -1;
          }
          else if ((fp->readBuffPos + offset) >= fp->readBuffSize)
          {
            fp->readBuffPos = -1;
          }
          else
          {
            fp->readBuffPos += offset;
          }
        }
        fp->fileReadPos += offset;
        break;

        case SEEK_END:        
        //SEEK_END - Set to 'offset' + file size
        fp->fileReadPos = fp->fileSize + offset -1;
        fp->readBuffPos = -1;
        break;

        default:
        /* This is an error.
        */
        return -1;
      }

      if( efs_fseek ( fp->efsHandle, offset, origin ) < 0 )
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fseek failed. FS Status=%d", efs_errno );
        fp->fileReadPos = fpReadPos;
        fp->readBuffPos = rdBuffPos;
      }
      else
        nReturn = 0;
    }
    else
#endif  /*FEATURE_QTV_FILE_IO_CACHING */
    {
      switch (origin)
      {
        case SEEK_SET:        
          break;

        case SEEK_CUR:        
          break;

        case SEEK_END:        
          break;

        default:
          /* This is an error.
          */
          return -1;
      }
      if( efs_fseek ( fp->efsHandle, offset, origin ) < 0 )
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fseek failed. FS Status=%d", efs_errno );          
      }
      else
        nReturn = 0;    
    }
  }
  else if (fp->bBufValid)
  {
    int32 npos;
    switch (origin)
    {
      case SEEK_CUR:
      npos = offset + fp->memBuf.curPos;
      break;

      case SEEK_END:
      npos = offset + fp->memBuf.bufSize;
      break;

	  case SEEK_SET:
      npos = offset;
      break;

      default:
      npos = (-1); /* error */
      break;
    }
    if ((npos>=0) && (npos<=(int32)fp->memBuf.bufSize))
    {
      fp->memBuf.curPos = npos;
      nReturn = 0;
    }
  }
  else if (fp->bPullBufValid)
  {
    //seek locally and asuming origin is always SEEK_SET
    if (origin == SEEK_CUR)
    {
      fp->pullBuf.curPos += offset;
    }
    else if (origin == SEEK_SET)
    {
      fp->pullBuf.curPos = offset;
    }

    if (origin != SEEK_END)
    {
      nReturn = 0;
    }
    else
    {
      //SEEK_END is invalid as the buffer size is unknown.
      QTV_ASSERT(0);
    }
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF
  else
  {
    switch (fp->streamType)
    {
      case DRM_IXSTREAM:

#ifdef FEATURE_QTV_DRM_DCF_TEST

      if (fp->inputStream >= 0)
      {
        int whence;

        switch (origin)
        {
          case SEEK_SET:
          whence = SEEK_SET;
          break;

          case SEEK_CUR:
          whence = SEEK_CUR;
          break;

          case SEEK_END:
          whence = SEEK_END;
          break;

          default:
            /* This is an error.
            */
          return -1;
        }

        if(efs_lseek(*fp->inputStream, offset, whence)>=0)
        {
          nReturn = 0;
        }
      }
#elif defined(FEATURE_QTV_DRM_DCF)

      IxStreamWhenceType fs_origin;
      switch (origin)
      {
        case SEEK_SET:
        fs_origin = IX_STRM_SEEK_START;
        break;

        case SEEK_CUR:
        fs_origin = IX_STRM_SEEK_CURRENT;
        break;

        case SEEK_END:
        fs_origin = IX_STRM_SEEK_END;
        break;

        default:
        // This is an error
        return 0;
      }
      if (fp->inputStream != NULL)
      {
        IxErrnoType errorCode = E_FAILURE;
        errorCode = ((IxStream*)fp->inputStream)->Seek(offset,fs_origin);
        if (errorCode != E_SUCCESS)
        {
          //seek failure,set error code to the error code returned from DRM suite.
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileSeek:IxStream->Seek failed..error code %d",errorCode );
        }
        else
          //Seek successful.Debug purpose
          QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileSeek succeed..error code %d",errorCode );
      }
#endif
      break;
    }//switch (fp->streamType)
  }
#endif  
  return(nReturn);
}

/* ======================================================================
FUNCTION
  OSCL_FileTell

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 OSCL_FileTell (OSCL_FILE *fp)
{
  int32  fpos = (-1);

  /* sanity check */
  if(!fp)
    return fpos;

  if (fp->efsHandle)
  {
#ifdef FEATURE_QTV_FILE_IO_CACHING
    if(fp->readBuffPos != -1)
    {
      fpos = fp->fileReadPos+1;
    }
    else
    {
      if(( fpos = efs_ftell ( fp->efsHandle ) ) < 0 )
	  {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_ftell failed. FS Status=%d", efs_errno );
      }
    }
#else /* FEATURE_QTV_FILE_IO_CACHING */
    if ( ( fpos = efs_ftell ( fp->efsHandle ) ) < 0 )
    {
      QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_ftell failed. FS Status=%d", efs_errno );
    }
#endif /* FEATURE_QTV_FILE_IO_CACHING */
  }
  else if ( fp->bBufValid )
  {
    fpos = fp->memBuf.curPos;
  }
  else if( fp->bPullBufValid)
  {
    fpos = fp->pullBuf.curPos;
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#ifdef FEATURE_QTV_DRM_DCF
  else
  {
    switch (fp->streamType)
    {
      case DRM_IXSTREAM:
        #ifdef FEATURE_QTV_DRM_DCF_TEST
          if (fp->inputStream >= 0)
          {
            fs_off_t result;
            result = efs_lseek (*fp->inputStream, 0, SEEK_CUR);
            if (result >= 0)
            {
              fpos = (uint32)result;
            }
          }
        #elif defined(FEATURE_QTV_DRM_DCF)
          //FileTell on IxStream

          if(fp->inputStream != NULL)
		      {
		        IxErrnoType errorCode = E_FAILURE;
		  	    errorCode = ((IxStream*)fp->inputStream)->Tell((uint32*)&fpos);
		        if(errorCode != E_SUCCESS)
		        {
		          //tell failure, set error code to the error code returned form DRM suite
		  	      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "OSCL_FileTell:IxStream->Tell failed..error code %d",errorCode );
		   	    }
		      }
        #endif
        break;
    }
  }
#endif

  return(fpos);
}

/* ======================================================================
FUNCTION
  OSCL_FileClose

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 OSCL_FileClose (OSCL_FILE *fp)
{
  int32 nRetVal=0;

  if ( fp )
  {
    if ( fp->efsHandle )
    {
      if ( efs_fclose ( fp->efsHandle ) < 0 )
      {
        QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fclose failed. FS Status=%d", efs_errno );
        nRetVal = EOF;
      }
	  else
		fp->efsHandle = NULL;
    }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
#ifdef FEATURE_QTV_DRM_DCF
    else
    {
      switch (fp->streamType)
      {
        case DRM_IXSTREAM:
        if(fp->inputStream != NULL)
		    {
			    /*
			    * Should not do Close on IxStream here as if we do it here, then
			    * when user tries to play the same clip without exiting QTV,
			    * read fails on IxStream and thus playback erros out.
			    * In this case, we need to do OPEN explicitly on IxStream.
          * To Do: Decide whether we need to call Close here or not.
			    */
		    }
        break;
      }
    }
#endif


#ifdef FEATURE_QTV_FILE_IO_CACHING
/* Freeing allocated read buffer here */
    if(fp->readBuffer)
    {
      QTV_Free (fp->readBuffer);
    }
#endif /* FEATURE_QTV_FILE_IO_CACHING */
    QTV_Delete(fp);/*lint !e449 */
  }
  return(nRetVal);
}

/* ======================================================================
FUNCTION
  OSCL_FileFlush

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
int32 OSCL_FileFlush(OSCL_FILE*)
#endif /* FEATURE_QTV_MFDRM */
{
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */

  /* just return 0 for now */
  return 0;
}


/* ======================================================================
FUNCTION
  OSCL_CheckEndOfFile

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 OSCL_CheckEndOfFile (OSCL_FILE *fp)
{
  int32 nRetVal = 0;
  unsigned long  filesize=0;

  /* sanity check */
  if(!fp)
    return 0;

  if (fp->efsHandle)
  {
    struct fs_stat  fileStats;      
    memset( &fileStats, 0, sizeof(fs_stat) );           
    if ( efs_fstat ( efs_fileno( fp->efsHandle ), &fileStats ) < 0 )
    {
      QTV_MSG_PRIO1( QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_fstat failed. FS Status=%d", efs_errno );
    }
    else
    {
      filesize = fileStats.st_size;
      if( efs_ftell( fp->efsHandle ) >= filesize )
      {
        nRetVal = 1;
      }
    }
  }
  else if ( fp->bBufValid )
  {
    nRetVal = (fp->memBuf.curPos==fp->memBuf.bufSize);
  }
#ifdef FEATURE_QTV_MFDRM
#error code not present
#endif /* FEATURE_QTV_MFDRM */
  return(nRetVal);
}

/* ======================================================================
FUNCTION
  OSCL_GetFileError

DESCRIPTION
  Thorough, meaningful description of what this function does.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
int32 OSCL_GetFileError (OSCL_FILE*)
{
  return 0; /* no error */
}
/* ======================================================================
FUNCTION
  OSCL_GetEFSFileSysFreeSpace

DESCRIPTION
  Find if a file with given (full path) name already exists

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if file already exists.

SIDE EFFECTS
  NONE

========================================================================== */
uint64 OSCL_GetEFSFileSysFreeSpace (void)
{
    //return fs_space_available();
	return 0;
}

/* ======================================================================
FUNCTION
  OSCL_FileExists

DESCRIPTION
  Find if a file with given (full path) name already exists

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if file already exists.

SIDE EFFECTS
  NONE

========================================================================== */
bool OSCL_FileExists (const OSCL_STRING &filename)
{

  EFS_FILE        *efp = NULL;  
  efp = efs_fopen (filename.get_cstr(), "r");
  if( efp == NULL )
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "File Name does not exist. FS Status=%d", efs_errno );
    return false;
  }
  else
  {
    (void)efs_fclose ( efp );
    return true;
  }
}

/* ======================================================================
FUNCTION
  OSCL_GetFileSysFreeSpace

DESCRIPTION
  Checks the available space in the file system
  path provided and returns that value

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if file already exists.

SIDE EFFECTS
  NONE

========================================================================== */
int64 OSCL_GetFileSysFreeSpace (const char *filePath)
{   
  int64 freeSpace;
  char* dirPath = NULL;

  // Extract directory path if full filename is speicifed.
  int i = 0, dirlen = 0;
  const char *clipNameStart = NULL;
  while ((filePath[i]) !='\0')
  {
    if (filePath[i++] == '/')
	{
	  clipNameStart = &filePath[i];
	}
  }
  if (clipNameStart)
  {
	// + 1 for null terminator
	dirlen = clipNameStart - filePath + 1;
	if (dirlen <= URL::MaxUrlSize)
	{
      dirPath = (char*) QTV_Malloc(dirlen);
	}
  }

  if (dirPath)
  {
	(void)ZUtils::StrcpyN(dirPath, filePath, dirlen);
	struct fs_statvfs buf;         
	memset( &buf, 0, sizeof(fs_statvfs) );         
	if( efs_statvfs ( dirPath, &buf ) == 0 )
	{
 	  freeSpace =  buf.f_bsize * buf.f_bavail;           
	}
	else
	{
	  freeSpace = -1;
	}         
	QTV_Free(dirPath);
	return freeSpace;       
  }
  else
  {
    return -1;
  }    
}

/* ======================================================================
FUNCTION
  OSCL_FileList

DESCRIPTION
  Lists all the files in a given directory on EFS

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if file list found.

SIDE EFFECTS
  NONE

========================================================================== */
bool OSCL_FileList(void *buffer,unsigned char *dirName,unsigned long bufSize)
{
  EFSDIR              *efd        = NULL;
  struct fs_dirent    *fsDentry   = NULL;
  uint32              offSetLngth = 0; 
  uint32              bytesRead   = 0;
   bool status = false;
  efd = efs_opendir ( (char *)dirName);
  if( efd == NULL )
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_opendir failed. FS Status=%d", efs_errno );
    status = false;
  } 
  else
  {
    while( ( fsDentry = efs_readdir ( efd ) ) != NULL  )
    {      
      offSetLngth = strlen( fsDentry->d_name );
      if( ( bytesRead + offSetLngth ) < bufSize )
      {
        memcpy( (char *)buffer + bytesRead, fsDentry->d_name, offSetLngth );
        (( char * )buffer )[offSetLngth]  = '\0';
        offSetLngth++;     
        bytesRead += offSetLngth;
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "Not Enough Buffer To Place All Directory Entries" );
        break; 
      }
    }       
    status = true;      
  }
  efs_closedir(efd); 
  return status;
}

/* ======================================================================
FUNCTION
  OSCL_NumFiles

DESCRIPTION
  Returns number of files in a given directory on the EFS

DEPENDENCIES
  NONE

RETURN VALUE
  TRUE if number of files found

SIDE EFFECTS
  NONE

========================================================================== */
bool OSCL_NumFiles(const char *dirName,int32 *numFiles)
{
  EFSDIR   *efd      = NULL;
  int32    fileCount = 0;
  bool status = false;
  efd = efs_opendir ( dirName );
  if( efd == NULL )
  {
    QTV_MSG_PRIO1( QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_opendir failed. FS Status=%d", efs_errno );
    *numFiles = -1;
    status = false;
  }
  else
  {
    while( efs_readdir ( efd ) != NULL )
    {
      fileCount++;
    }
    *numFiles = fileCount;
    status = true;
  }
  efs_closedir(efd);
  return status;
}

/* ======================================================================
FUNCTION
  OSCL_FileSize

DESCRIPTION
  Returns the size of the file

DEPENDENCIES
  NONE

RETURN VALUE
  The Size of the file

SIDE EFFECTS
  NONE

========================================================================== */
uint32 OSCL_FileSize( const OSCL_STRING &filename )
{
  struct fs_stat buf;
  memset( &buf, 0, sizeof(fs_stat) );              
  if( efs_stat ( filename.get_cstr(), &buf ) == 0 )
  {
    return buf.st_size;
  }
  else
  {
    QTV_MSG_PRIO1(QTVDIAG_FILE_OPS, QTVDIAG_PRIO_ERROR, "efs_stat failed. FS Status=%d", efs_errno );
    return 0;
  }    
}
