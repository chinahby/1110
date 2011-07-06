/* =======================================================================
                              FileBase.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/base/main/latest/src/filebase.cpp#24 $
$DateTime: 2010/09/24 05:36:59 $
$Change: 1452660 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Common definitions                      */

#include "oscl_file_io.h"

#include "utf8conv.h"
#include "isucceedfail.h"

#include "oscl_string.h"

#include "filebase.h"
#include "mpeg4file.h"
uint8 MP4_3GP_SIG_BYTES1[MPG4_FILE_SIGNATURE_BYTES] ={'f','t','y','p'};
uint8 MP4_3GP_SIG_BYTES2[MPG4_FILE_SIGNATURE_BYTES] ={'m','o','o','v'};
uint8 MP4_3GP_SIG_BYTES3[MPG4_FILE_SIGNATURE_BYTES] ={'m','d','a','t'};

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
#include "GenericAudioFile.h"
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_FILE_FRAGMENTATION
#include "mp4FragmentFile.h"
#endif //FEATURE_FILE_FRAGMENTATION

#ifdef FEATURE_QTV_WINDOWS_MEDIA
#include "asffile.h"
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#ifdef FEATURE_QTV_AVI
#include "avifile.h"
#endif

#ifdef FEATURE_QTV_DRM_DCF
   #include "IxStream.h"
#endif

class Mpeg4Player;

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
/*===========================================================================

                      Audio Format Detection Definitions

===========================================================================*/


#define AUDIO_FMT_BYTES       12
#define AUDIO_SMAF_BYTES      18
#define AUDIO_BYTES_MAX   18
#define AUDIO_FMT_MIDI_MTHD      "MThd"
#define AUDIO_FMT_SMAF_MTHD      "MMMD"
#define AUDIO_FMT_PMD_CMID       "cmid"
#define AUDIO_FMT_PMD_MELO       "melo"
#define AUDIO_FMT_PMD_WASF       "wasf"
#define AUDIO_FMT_QCP_RIFF       "RIFF"
#define AUDIO_FMT_QCP_QLCM       "QLCM"
#define AUDIO_FMT_IMELODY_BEGIN  "BEGIN:IMELODY"
#define AUDIO_FMT_AMR_BEGIN      "#!AMR"
#define AUDIO_FMT_MP4_MARK       "ftyp"
#define AUDIO_FMT_ADPCM_MARK     "WAVE"

#define AUDIO_FMT_IS_SYNC(x) \
  (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF0) == 0xF0)

#define AUDIO_FMT_IS_ID3V2_TAG(x) \
  (((byte *)(x))[0] == 0x49 && ((byte *)(x))[1] == 0x44 && \
                                                      ((byte *)(x))[2] == 0x33)
//FF F9 64 40 is AAC mark and FF F3 48 7C is MP3 Mark.
#define AUDIO_FMT_IS_AAC_SYNC(x) \
  (((byte *)(x))[0] == 0xFF && (((byte *)(x))[1] & 0xF6) == 0xF0)

//AAC when first 4 bytes are "ADIF"
#define AUDIO_FMT_IS_AAC_ADIF(x) \
  (\
      (x)[0] == 'A' && \
      (x)[1] == 'D' && \
      (x)[2] == 'I' && \
      (x)[3] == 'F' )

#define AUDIO_FMT_DT_RETURN(pcpsz, mime) \
   { \
      if (pcpsz) \
         *pcpsz = mime; \
      return true; \
   }
#define MT_AUDIO_MIDI      "audio/mid"
#define MT_AUDIO_MP3       "audio/mp3"
#define MT_AUDIO_QCP       "audio/qcp"
#define MT_AUDIO_VND_QCELP "audio/vnd.qcelp"
#define MT_AUDIO_QCF       "audio/qcf"
#define MT_AUDIO_MMF       "audio/mmf"
#define MT_AUDIO_PHR       "audio/spf"
#define MT_AUDIO_IMELODY   "audio/imy"
#define MT_AUDIO_ADPCM     "audio/wav"
#define MT_AUDIO_AAC       "audio/aac"
#define MT_AUDIO_AMR       "audio/amr"
#define MT_AUDIO_WMA       "audio/wma"
#define MT_AUDIO_HVS       "audio/hvs"
#define MT_AUDIO_SAF       "audio/saf"
#define MT_AUDIO_XMF       "audio/xmf"
#define MT_AUDIO_DLS       "audio/dls"
#define MT_VIDEO_PMD       "video/pmd"
#define FMT_QCP_QLCM       "QLCM"
#define MT_VIDEO_MPEG4     "video/mp4"
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
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

/* ======================================================================
FUNCTION
  SAMPLE_FUNC

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

/* ======================================================================
FUNCTION
  FileBase::isMp4Filebool

DESCRIPTION
  Static method to Returns True if the file is MP4 otherwise, returns FALSE.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileBase::isMp4Filebool(uint8* pBuf, uint32 mp4size)
{
  if(pBuf && mp4size >=8 )
  {
    if ( (!memcmp(pBuf+4, "ftyp", MPG4_FILE_SIGNATURE_BYTES)) )
      return true;
  }
  return false;
}
bool FileBase::MAKE_AAC_AUDIO_CONFIG(uint8* pBuffer,uint16 nAudObjectType,
                                  uint16 nSamplingFreq,uint16 nChannelsConfig,
                                  uint8* pConfigSize)
{
  uint16 configData = 0;
  bool bRet = false;
  if(pConfigSize)
  {
    bRet = true;
    if(pBuffer)
    {
      memset(pBuffer,0,(size_t)pConfigSize);
      //Syntax of Audio Config is as below
      // MSB 5 bits(15-11) gives Audio Object Type
      // Bits(10-7) gives sampling frequency index
      // Bits(6-3)  gives channel configuration
      // Bits(2-0)  unused as of now
      configData |= ( (nAudObjectType & 0x001F) << 11);
      configData |= ( (nSamplingFreq  & 0x000F) << 7);
      configData |= ( (nChannelsConfig& 0x000F) << 3);      
      //Convert to little endian
      uint8* pData = (uint8*)&configData;
      configData = (((uint16)pData[0])<<8) | (uint16)pData[1];      
      memcpy(pBuffer,&configData,(size_t)pConfigSize);
    }
  }
  return bRet;
}

/* ======================================================================
FUNCTION
  *FileBase::openMediaFile

DESCRIPTION
  Static method to read in a media file from disk and return the FileBase interface

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
FileBase *FileBase::openMediaFile(unsigned char *pBuf,
                                  Mpeg4Player *pMpeg4Player,
                                  uint32 bufSize,
                                  bool bPlayVideo,
                                  bool bPlayAudio,
                                  bool bPlayText
#if defined(FEATURE_QTV_PSEUDO_STREAM) || \
    defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
                                  ,bool bPseudoStream
                                  ,uint32 wBufferOffset
                                  ,uint32 wStartupTime
                                  ,QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize
                                  ,QtvPlayer::FetchBufferedDataT FetchBufferedData
                                  ,QtvPlayer::InstanceHandleT handle
#endif  /* FEATURE_QTV_PSEUDO_STREAM */
        /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
                                    )
{

  uint32 size = 0;
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
//initializing to unknown audio type
AudioDataFormatType audioFormatType = AUDIO_UNKNOWN;
  size = AUDIO_BYTES_MAX;
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
#ifdef FEATURE_QTV_WINDOWS_MEDIA
  size = sizeof(asfFileIdentifier)+1;
 // uint8 Buf[sizeof(asfFileIdentifier)+1];
 // size = readFile(filename, Buf, 0, size);

  if( IsASFFile(pBuf, size)
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
     || IsASFFile(FetchBufferedDataSize, FetchBufferedData,handle)
#endif
   )
  {
    ASFFile *asf = NULL;
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
    asf = QTV_New_Args( ASFFile, (NULL, pMpeg4Player, pBuf, bufSize,NULL, bPlayVideo, bPlayAudio,
                                 bPseudoStream, wBufferOffset, wStartupTime,FetchBufferedDataSize,FetchBufferedData, handle) ); 
#endif /* FEATURE_QTV_MFDRM */

#else   /* #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
   asf = QTV_New_Args( ASFFile, (NULL, pMpeg4Player, pBuf, bufSize,NULL, bPlayVideo, bPlayAudio) );
#endif /* FEATURE_QTV_MFDRM */

#endif /* #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) */
    if (asf)
    {
      if ( asf->FileSuccess())
      {
        return asf;
      }
      else
      {
        QTV_Delete( asf);
      }
    }
    return NULL;
  }
#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#ifdef FEATURE_QTV_AVI
    if(IsAVIFile(NULL,pBuf,false)
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
     || IsAVIFile(FetchBufferedDataSize, FetchBufferedData, handle)
#endif
     )
    {
      AVIFile *avi = NULL;
      #if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
        avi = QTV_New_Args( AVIFile, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio, bPseudoStream, wBufferOffset, wStartupTime,FetchBufferedDataSize,FetchBufferedData, handle));
      #else
        avi = QTV_New_Args( AVIFile, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio) );
      #endif
      if (avi)
      {
        return avi;
      }
      return NULL;
    }
#endif //FEATURE_QTV_AVI

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT 
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
    if((FetchBufferedData) && (FetchBufferedDataSize))
    {
      audioFormatType = isGenericAudioFile(FetchBufferedDataSize, FetchBufferedData);
    }
    else
#endif
    {
      audioFormatType = isGenericAudioFile(pBuf, size);
    }

    if( audioFormatType != AUDIO_UNKNOWN )
    {
      GenericAudioFile *genericAudioFile = NULL;
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD

#ifdef FEATURE_QTV_MFDRM
#error code not present
#else
      genericAudioFile = QTV_New_Args( GenericAudioFile, (NULL, pMpeg4Player, pBuf,
                                     bufSize, bPlayVideo, bPlayAudio, bPlayText,
                                     audioFormatType, bPseudoStream, wBufferOffset,
                                     wStartupTime, FetchBufferedDataSize, FetchBufferedData, handle));
#endif /* FEATURE_QTV_MFDRM */

#endif /* FEATURE_QTV_3GPP_PROGRESSIVE_DNLD */
      if (genericAudioFile)
      {
        if ( genericAudioFile->FileSuccess() )
        {
#if defined (FEATURE_QTV_3GPP_PROGRESSIVE_DNLD) || defined (FEATURE_QTV_PSEUDO_STREAM)
          if( bPlayAudio && !bPseudoStream )
#else
          if( bPlayAudio )
#endif
          {
            genericAudioFile->SetTotalTime();
            genericAudioFile->GetAudioSpec();
          }
#ifdef FEATURE_QTV_3GPP_PROGRESSIVE_DNLD
          if(bPseudoStream)
          {
            genericAudioFile->parseFirstFragment();
          }
#endif  /*FEATURE_QTV_3GPP_PROGRESSIVE_DNLD*/
          return genericAudioFile;
        }
        else
        {
          QTV_Delete( genericAudioFile );
        }
      }
      return NULL;
    }
    else
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
    {
      /* If pBuf is available and size of the buffer is minimum 8 bytes, then use the pBuf.
         If pBuf is NULL, then use FetchBuffer functions to check whether it is valid 3gp
         or mp4 clip*/
      if ( (pBuf != NULL && bufSize >= MPG4_FILE_SIGNATURE_BYTES * 2 &&
            IsMP4_3GPFile(pBuf + MPG4_FILE_SIGNATURE_BYTES, 4))
#if defined(FEATURE_QTV_PSEUDO_STREAM) || defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
            || IsMP4_3GPFile(FetchBufferedDataSize, FetchBufferedData, handle)
#endif
         )
      {
        Mpeg4File *mp4 = NULL;
  
  #ifdef FEATURE_QTV_PSEUDO_STREAM
  
    #ifdef FEATURE_QTV_MFDRM
#error code not present
    #else /* FEATURE_QTV_MFDRM */
      #ifdef FEATURE_FILE_FRAGMENTATION
        mp4 = QTV_New_Args(Mp4FragmentFile, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio, bPlayText,
                                           bPseudoStream, wBufferOffset, wStartupTime, FetchBufferedDataSize, FetchBufferedData, handle));
      #else
        mp4 = QTV_New_Args(Mp4FragmentFile, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio, bPlayText,
                                           bPseudoStream, wBufferOffset, wStartupTime, FetchBufferedDataSize, FetchBufferedData, handle));
    #endif /* FEATURE_FILE_FRAGMENTATION */
    #endif /* FEATURE_QTV_MFDRM */
  
  #elif defined(FEATURE_QTV_3GPP_PROGRESSIVE_DNLD)
    #ifdef FEATURE_QTV_MFDRM
#error code not present
    #else
       #ifdef FEATURE_FILE_FRAGMENTATION
        mp4 = QTV_New_Args( Mp4FragmentFile, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio, bPlayText,
                                       bPseudoStream, wBufferOffset, wStartupTime,
                                       FetchBufferedDataSize, FetchBufferedData,handle) );
       #else
  
        mp4 = QTV_New_Args( Mpeg4File, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio, bPlayText,
                                       bPseudoStream, wBufferOffset, wStartupTime,
                                       FetchBufferedDataSize, FetchBufferedData, handle));
       #endif
    #endif /* FEATURE_QTV_MFDRM */
  #else
    #ifdef FEATURE_FILE_FRAGMENTATION
        mp4 = QTV_New_Args( Mp4FragmentFile, (NULL, pMpeg4Player, pBuf, bufSize,bPlayVideo,bPlayAudio, bPlayText) );
    #else
        mp4 = QTV_New_Args( Mpeg4File, (NULL, pMpeg4Player, pBuf, bufSize, bPlayVideo, bPlayAudio, bPlayText) );
    #endif //FEATURE_FILE_FRAGMENTATION
  #endif //FEATURE_QTV_PSEUDO_STREAM
        if (mp4)
        {
          if ( mp4->FileSuccess())
          {
          mp4->parseFirstFragment();
          return mp4;
          }
          else
          {
            QTV_Delete( mp4 );
          }
        }
      }
      return NULL;
    }
}



/* ======================================================================
FUNCTION
  FileBase::openMediaFile

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
FileBase* FileBase::openMediaFile(  OSCL_STRING filename,
                                    Mpeg4Player *pMpeg4Player,
                                    bool bPlayVideo,
                                    bool bPlayAudio,
                                    bool bPlayText)
{
  uint32 size = 0;
#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
//initialize audioFormatType to unknown audio type
  AudioDataFormatType audioFormatType = AUDIO_UNKNOWN;
  uint8 genericAudioBuf[AUDIO_BYTES_MAX];
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */

#ifdef FEATURE_QTV_AVI 
   uint32 avisize = 12; //DIVX_AVI_FILE_SIGNATURE_BYTES+AVI_FILE_SIGNATURE_BYTES+4
   uint8 aviBuf[12];
   #ifndef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
     avisize = readFile(filename, aviBuf, 0, avisize);
   #endif
#endif

#ifdef FEATURE_QTV_WINDOWS_MEDIA
  uint8 Buf[sizeof(asfFileIdentifier)+1];
  size = sizeof(asfFileIdentifier)+1;

#ifndef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
     size = readFile(filename, Buf, 0, size);
#endif
 
#ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
  if( IsASFFile(filename) )
#else
  if( IsASFFile(Buf, size) )
#endif
  {
    ASFFile *asf = NULL;
    asf = QTV_New_Args( ASFFile, (filename, pMpeg4Player, NULL,0,0,bPlayVideo,bPlayAudio) );
    if (asf)
    {      
      return asf;      
    }
    return NULL;
  }
  else
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
#ifdef FEATURE_QTV_AVI
  #ifndef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
    if(IsAVIFile(filename,aviBuf,false))
  #else
    if(IsAVIFile(filename,aviBuf,true))
  #endif
  {
    AVIFile* aviFile = QTV_New_Args( AVIFile, (filename, pMpeg4Player, NULL, 0, bPlayVideo, bPlayAudio) );
    return aviFile;
  }
  else
#endif

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT
  {
    size = AUDIO_BYTES_MAX;
#ifndef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE    
    size = readFile(filename, genericAudioBuf, 0, size);
    audioFormatType = isGenericAudioFile(genericAudioBuf, size);
#endif        

#ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE    
    if(IsGenericAudioFile(filename))
#else
    if( audioFormatType != AUDIO_UNKNOWN )
#endif
    {
      GenericAudioFile *genericAudioFile = NULL;
      genericAudioFile = QTV_New_Args( GenericAudioFile, (filename, pMpeg4Player, NULL,0,bPlayVideo,bPlayAudio,bPlayText,audioFormatType ) );
      if (genericAudioFile)
      {
        if ( genericAudioFile->FileSuccess() )
        {
          if( bPlayAudio )
          {
            genericAudioFile->SetTotalTime();
            genericAudioFile->GetAudioSpec();
          }
          return genericAudioFile;
        }
        else
        {
          QTV_Delete( genericAudioFile );
        }
      }
      return NULL;
    }
    else
#endif /* FEATURE_QTV_GENERIC_AUDIO_FORMAT */
    {
      uint8 mp4_buff[MPG4_FILE_SIGNATURE_BYTES];
      uint32 mp4_bufSize = readFile(filename, mp4_buff, 4, MPG4_FILE_SIGNATURE_BYTES);

      if(IsMP4_3GPFile(mp4_buff, mp4_bufSize))
      {
        Mpeg4File *mp4 = NULL;
  
  #ifdef FEATURE_FILE_FRAGMENTATION
        mp4 = QTV_New_Args( Mp4FragmentFile, (filename, pMpeg4Player, NULL,0,bPlayVideo,bPlayAudio,bPlayText) );
  #else
        mp4 = QTV_New_Args( Mpeg4File, (filename, pMpeg4Player, NULL,0,bPlayVideo,bPlayAudio,bPlayText) );
  #endif //FEATURE_FILE_FRAGMENTATION
        if (mp4)
        {
          if ( mp4->FileSuccess() )
          {
            mp4->parseFirstFragment();
            return mp4;
          }
          else
          {
            QTV_Delete( mp4 );
          }
        }
      }
    }
    return NULL;
  }
}
#endif /* FEATURE_QTV_MFDRM */

#ifdef FEATURE_QTV_DRM_DCF
/* ======================================================================
FUNCTION
  FileBase::openMediaFile

DESCRIPTION
  Creates media instance for supporting DCF playback

INPUT PARAMETERS:
->inputStream:IxStream*
->urnType:It should be URN_INPUTSTREAM
->bPlayVideo:Indicates if this is video instance
->bPlayAudio:Indicates if this is audio instance
->bPlayText:Indicates if this is text instance

DEPENDENCIES
None

RETURN VALUE
FileBase* (media instance such as mpeg4file/asfifle pointer)

SIDE EFFECTS
None
========================================================================== */
FileBase* FileBase::openMediaFile(  dcf_ixstream_type inputStream,
                                    Mpeg4Player *pMpeg4Player,
                                    QtvPlayer::URNTypeT urnType,
                                    bool bPlayVideo,
                                    bool bPlayAudio,
                                    bool bPlayText)
{

  if(urnType != QtvPlayer::URN_INPUTSTREAM)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "FileBase::openMediaFile for dcf_ixstream_type but urnType passed in isn't URN_INPUTSTREAM");
	  return NULL;
  }
#ifdef FEATURE_QTV_WINDOWS_MEDIA

  uint32 size = sizeof(asfFileIdentifier)+1;
  uint8 Buf[sizeof(asfFileIdentifier)+1];

  size = readFile(inputStream, Buf, 0, size);
  if( IsASFFile(Buf, size) )
  {
    ASFFile *asf = NULL;
    asf = QTV_New_Args( ASFFile, (inputStream, pMpeg4Player, bPlayVideo, bPlayAudio) );
    if(asf)
    {      
      return asf;      
    }
    return NULL;
  }
  else
#endif /* FEATURE_QTV_WINDOWS_MEDIA */
  {
    uint8 mp4_buff[MPG4_FILE_SIGNATURE_BYTES];
    uint32 mp4_bufSize = readFile(inputStream, mp4_buff, 4, MPG4_FILE_SIGNATURE_BYTES);

    if(IsMP4_3GPFile(mp4_buff, mp4_bufSize))
    {
      Mpeg4File *mp4 = NULL;
  
  #ifdef FEATURE_FILE_FRAGMENTATION
      mp4 = QTV_New_Args( Mp4FragmentFile, (inputStream, pMpeg4Player, bPlayVideo, bPlayAudio, bPlayText));
  #else
      mp4 = QTV_New_Args( Mpeg4File, (inputStream, pMpeg4Player, bPlayVideo, bPlayAudio, bPlayText));
  #endif //FEATURE_FILE_FRAGMENTATION
      if(mp4)
      {
        if ( mp4->FileSuccess() )
        {
          mp4->parseFirstFragment();
          return mp4;
        }
        else
        {
          QTV_Delete( mp4 );
          return NULL;
        }
      }
    }
    return NULL;
  }
}

/* ======================================================================
FUNCTION
  FileBase::readFile

DESCRIPTION
  opens and reads requested bytes from dcf_ixstream_type

INPUT/OUTPUT
  inputStream  = inputStream to be be read from.
  buffer    = buffer in which to read
  pos       = from where to start reading in file
  size      = size of the buffer.

DEPENDENCIES
 None

RETURN VALUE
  bytes read
  0 in case of error

SIDE EFFECTS
  None

========================================================================== */
uint32 FileBase::readFile( dcf_ixstream_type inputStream,
                            uint8 *buffer,
                            uint32 pos,
                            uint32 size )
{
  uint32 nRead = 0;
  IxStream* pStream = NULL;
  bool   isEndofStream = false;
  uint32 reqSizeToRead = size;

  QTV_USE_ARG1(pos);

  if(inputStream == NULL)
    return nRead;

  /*
  * This function is used only to read first few bytes to decide whether it's an ASF/3GP file.
  * if we don't do pStream->Seek(0,IX_STRM_SEEK_START) at the begining before attempting to read,
  * we will hit an error when attempting to play WM clip without exiting QTV.
  *
  * For all other subsequent reads, readFile( OSCL_FILE *fp, ..) is used.
  */

  IxErrnoType errorCode = E_FAILURE;
  pStream = (IxStream*)inputStream;
  if(pStream)
  {
	  errorCode = pStream->Seek(pos,IX_STRM_SEEK_START);
	  if(errorCode == E_SUCCESS)
	  {
	    errorCode = E_FAILURE;
      errorCode = pStream->Read((byte*)buffer,(uint32)reqSizeToRead,&nRead,&isEndofStream);
      if(errorCode == E_SUCCESS)
      {
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "FileBase::readFile successful nRead %d",nRead );
        errorCode = pStream->Seek(0,IX_STRM_SEEK_START);
        QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "Moved back IxStream to BEGINING errorCode= %d",errorCode );
      }
      else
      {
	      QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "FileBase::readFile failed error code %d",errorCode );
	    }
	  }
	  else
	  {
	    QTV_MSG_PRIO1(QTVDIAG_GENERAL, QTVDIAG_PRIO_FATAL, "FileBase::readFile pStream->Seek failed errorCode %d",errorCode );
    }
  }
  return nRead;
}
#endif


/* ======================================================================
FUNCTION
  FileBase::readFile

DESCRIPTION
  opens the file, reads it and closes the file.

INPUT/OUTPUT
  filename  = name of the file to be read.
  buffer    = buffer in which to read
  pos       = from where to start reading in file
  size      = size of the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  bytes read
  0 in case of error

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
uint32 FileBase::readFile( OSCL_STRING filename,
                            uint8 *buffer,
                            uint32 pos,
                            uint32 size )
{
  uint32 nRead = 0;
  OSCL_FILE *fp = OSCL_FileOpen (filename, (OSCL_TCHAR *) _T("rb"));
  if( fp )
  {
    nRead = readFile(fp, buffer, pos, size);
    (void)OSCL_FileClose(fp);
  }
  return nRead;
}

/*===========================================================================

FUNCTION
  FileBase::readFile

DESCRIPTION
  reads from file referenced by file pointer

INPUT/OUTPUT
  fp        = file pointer
  buffer    = buffer in which to read
  pos       = from where to start reading in file
  size      = size of the buffer.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  bytes read
  0 in case of error

SIDE EFFECTS
  Detail any side effects.

===========================================================================*/
uint32 FileBase::readFile( OSCL_FILE *fp,
                            uint8 *buffer,
                            uint32 pos,
                            uint32 size )
{
  if(!fp)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL File Pointer");
    return 0;
  }
  return (uint32)OSCL_FileSeekRead (buffer, 1, size, fp,  pos, SEEK_SET);
}

/*===========================================================================

FUNCTION
  FileBase::seekFile

DESCRIPTION
  sets the file pointer at given offset

INPUT/OUTPUT
  fp        = file pointer
  offset    = what is the offset
  origin    = location from where offset is applied

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
 (-1) for error
 0 for success

SIDE EFFECTS
  Detail any side effects.

===========================================================================*/
int32 FileBase::seekFile( OSCL_FILE *fp, uint32 offset, uint32 origin)
{
  if(!fp)
  {
    QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "NULL File Pointer");
    return (-1);
  }
  return OSCL_FileSeek (fp,  offset, origin);
}

#ifdef FEATURE_QTV_WINDOWS_MEDIA
/* ======================================================================
FUNCTION
  FileBase::IsASFFile

DESCRIPTION
  Checks if the file is WM by checking the ASF header object's GUID.

INPUT/OUTPUT
  pBuf = pointer ot the buffer containing file bytes from start
  size = buffer size, ahould be at least GUID size (16 bytes)

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  TRUE if file is ASF else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileBase::IsASFFile(uint8 * pBuf, uint32 size)
{
  if( pBuf && (size >= sizeof(asfFileIdentifier)) )
  {
    if( !memcmp(pBuf, asfFileIdentifier, sizeof(asfFileIdentifier)) )
    {
      return true;
    }
  }
  return false;
}
/* ======================================================================
FUNCTION
  FileBase::IsASFFile

DESCRIPTION
  Checks if the file is WM by checking the ASF header object's GUID.

INPUT/OUTPUT
  pBuf = pointer ot the buffer containing file bytes from start
  size = buffer size, ahould be at least GUID size (16 bytes)

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  TRUE if file is ASF else FALSE

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileBase::IsASFFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle)
{
  uint8 httpDataBuf[ sizeof(asfFileIdentifier) ];
  uint32 wHttpBufOffset = 0;

  if( FetchBufferedDataSize && FetchBufferedData )
  {
     uint32 userData = 0;
     boolean bEndOfData = false;
     /* Get down loaded data offset from OEM */
     FetchBufferedDataSize( userData,&wHttpBufOffset,&bEndOfData, handle);

     if( wHttpBufOffset >= sizeof(asfFileIdentifier) )
     {
       /* Get the data from OEM, use default track id 0x0 */
       (void)FetchBufferedData( httpDataBuf, sizeof(asfFileIdentifier), 0, 0x0, handle);

       if( !memcmp(httpDataBuf, asfFileIdentifier, sizeof(asfFileIdentifier)) )
       {
         return true;
       }
     }
  }
  return false;
}
#ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
/* ======================================================================
 FUNCTION
   FileBase::IsASFFile
 
 DESCRIPTION
   Checks if the file is WM by checking the file extensions
   This avoids extra FILE OPEN just to determine the type.
   File extensions that will make return TRUE from here
   .asf,.wma,.wmv
 
 INPUT/OUTPUT
   filename identifying the content to be played.
 
 DEPENDENCIES
   List any dependencies for this function, global variables, state,
   resource availability, etc.
 
 RETURN VALUE
   TRUE if file is ASF else FALSE
 
 SIDE EFFECTS
   Detail any side effects.
 
 ========================================================================== */
 bool FileBase::IsASFFile(OSCL_STRING filename)
 {
   if(
       (ZUtils::FindR( (const char*)filename.get_cstr(),".wma") != ZUtils::npos) ||
       (ZUtils::FindR( (const char*)filename.get_cstr(),".asf") != ZUtils::npos)||
       (ZUtils::FindR( (const char*)filename.get_cstr(),".wmv") != ZUtils::npos) 
     )
   {
     return true;
   }
   return false;
}
#endif //FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE

#endif /* FEATURE_QTV_WINDOWS_MEDIA */

#ifdef FEATURE_QTV_GENERIC_AUDIO_FORMAT

 #ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
/* ======================================================================
 FUNCTION
   FileBase::IsGenericAudioFile
 
 DESCRIPTION
   Checks if the file is Generic audio file by checking the file extension.
   This avoids extra FILE OPEN just to determine the type.
   File extensions that will make return TRUE from here
   .aac,.mp3,.midi,.qcp,.amr
 
 INPUT/OUTPUT
   filename identifying the content to be played.
 
 DEPENDENCIES
   List any dependencies for this function, global variables, state,
   resource availability, etc.
 
 RETURN VALUE
   TRUE if file is ASF else FALSE
 
 SIDE EFFECTS
   Detail any side effects.
 
 ========================================================================== */
 bool FileBase::IsGenericAudioFile(OSCL_STRING filename)
 {
   if(
       (ZUtils::FindR( (const char*)filename.get_cstr(),".mp3")  != ZUtils::npos) || //mp3 audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".amr")  != ZUtils::npos) || //amr audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".midi") != ZUtils::npos) || //midi audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".qcp")  != ZUtils::npos) || //qcp audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".wav")  != ZUtils::npos) || //wav audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".imy")  != ZUtils::npos) || //imelody
       (ZUtils::FindR( (const char*)filename.get_cstr(),".melo") != ZUtils::npos) || //web audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".wasf") != ZUtils::npos) || //web audio
       (ZUtils::FindR( (const char*)filename.get_cstr(),".aac")  != ZUtils::npos)    //aac
     )
   {
     return true;
   }
   return false;
 }
#endif//#ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE

/* ======================================================================
FUNCTION
  FileBase::audio_Format_DetectType

DESCRIPTION
  Checks the what type of audio file it is

INPUT/OUTPUT
  cpBuf = pointer ot the buffer containing file bytes from start
  pdwSize = buffer size

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Audio format of the file.

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
AudioDataFormatType  FileBase::audio_Format_DetectType(const void * cpBuf, uint32  pdwSize)
{
  byte *   cbBuf = (byte *)cpBuf;
  uint16   wLen;

  // If all are NULL except pdwSize, return the required number of bytes
  // to do type detection
  if (!cpBuf && pdwSize)
  {
    pdwSize = AUDIO_BYTES_MAX;
    return AUDIO_UNKNOWN;
  }

  if (!cpBuf || !pdwSize)
  {
    return AUDIO_UNKNOWN;
  }

  if (pdwSize < AUDIO_FMT_BYTES)
  {
    pdwSize = AUDIO_FMT_BYTES - pdwSize;
    return AUDIO_UNKNOWN;
  }

   // MIDI:
   // Bytes 0-3 = 4d 54 68 64 ("MThd")
   // Bytes 4-5 = 00 00 / 00 01 (Format 0 or Format 1)
  {
     wLen = 4;
     if (!memcmp(cbBuf, AUDIO_FMT_MIDI_MTHD, wLen))
     {
       if (cbBuf[wLen] == 0x00 && (cbBuf[wLen+1] == 0x00 || cbBuf[wLen+1] == 0x01))
       {
         return(AUDIO_MIDI);
       }
     }
  }
   // SMAF:
   // Bytes 0-3 = 4d 4d 4d 44 ("MMMD")
   // Bytes 17  = if byte 17 == 0xf0 (SPF) otherwise (MMF)
    {
      wLen = 4;
      if (!memcmp(cbBuf, AUDIO_FMT_SMAF_MTHD, wLen))
      {
        if (pdwSize < AUDIO_SMAF_BYTES)
        {
          pdwSize = AUDIO_SMAF_BYTES - pdwSize;
          return AUDIO_UNKNOWN;
        }
        if (cbBuf[17] == 0xf0 )
        {
          return(AUDIO_PHR);
        }
        else
        {
          return(AUDIO_MMF);
        }
      }
    }

   // PMD (WebAudio):
   // Bytes 0-3 = "cmid" / "melo" / "wasf"
    {
      wLen = 4;
      if (!memcmp(cbBuf, AUDIO_FMT_PMD_CMID, wLen) ||
          !memcmp(cbBuf, AUDIO_FMT_PMD_MELO, wLen) ||
          !memcmp(cbBuf, AUDIO_FMT_PMD_WASF, wLen))
      {
        return(VIDEO_PMD);
      }
    }

   // QCP (PureVoice):
   // Bytes 0-3 = "RIFF"
   // Bytes 4-7 = Length of chunck - 8 (not checked)
   // Bytes 8-11 = "QLCM"
    {
      wLen = 4;
      if (!memcmp(cbBuf, AUDIO_FMT_QCP_RIFF, wLen) && !memcmp(cbBuf+8, FMT_QCP_QLCM, wLen))
      {
         return(AUDIO_QCP);
      }
    }

    // AAC
    {
      if (AUDIO_FMT_IS_AAC_SYNC(cbBuf) || AUDIO_FMT_IS_AAC_ADIF(cbBuf))
      {
         return(NONMP4_AUDIO_AAC);
      }
    }

   // MP3
    {
      if (AUDIO_FMT_IS_SYNC(cbBuf) || AUDIO_FMT_IS_ID3V2_TAG(cbBuf))
      {
        return(NONMP4_AUDIO_MP3);
      }
   }

   // IMELODY:
   // Bytes 0-11 = ("BEGIN:IMELODY")
    {
      wLen = 12;
      if (!memcmp(cbBuf, AUDIO_FMT_IMELODY_BEGIN, wLen))
      {
        return(AUDIO_IMELODY);
      }
    }

   // AMR
   // Bytes 0 - 4
    {
      wLen = 5;
      if (!memcmp(cbBuf, AUDIO_FMT_AMR_BEGIN, wLen))
      {
        return(AUDIO_AMR);
      }
    }


   // ADPCM WAV files
   // Bytes 0-3 = "RIFF"
   // Bytes 8-11 = "WAVE"
    {
      wLen = 4;
      if ( !memcmp(cbBuf, AUDIO_FMT_QCP_RIFF, wLen) &&
          !memcmp(cbBuf+8, AUDIO_FMT_ADPCM_MARK, wLen))
      {
        return(AUDIO_ADPCM);
      }
    }

    return AUDIO_UNKNOWN;
}

/* ======================================================================
FUNCTION
  FileBase::isGenericAudioFile

DESCRIPTION
  finds the file is of wihich audio type by checking the  header.

INPUT/OUTPUT
  pBuf = pointer ot the buffer containing file bytes from start
  size = buffer size

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Returns the audio format of the file

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
AudioDataFormatType FileBase::isGenericAudioFile(uint8 * pBuf, uint32 size)
{
  AudioDataFormatType audioFormatType;
  if( pBuf && (size >= AUDIO_FMT_BYTES) )
  {
    audioFormatType = audio_Format_DetectType(pBuf, size);
    if(audioFormatType)
    {
      return audioFormatType;
    }
  }
  return AUDIO_UNKNOWN;
}

/* ======================================================================
FUNCTION
  FileBase::isGenericAudioFile

DESCRIPTION
  finds the file is of wihich audio type by checking the  header.

INPUT/OUTPUT
  pBuf = pointer ot the buffer containing file bytes from start
  size = buffer size

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Returns the audio format of the file

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
AudioDataFormatType FileBase::isGenericAudioFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle)
{
  /*
  * audio_Format_DetectType can 
  * access httpDataBuf at index 17 when detecting SMAF media, 
  * so make sure we read 
  * AUDIO_BYTES_MAX(defined as 18) bytes to prevent out of bounds access. 
  *
  * Declaring httpDataBuf with AUDIO_FMT_BYTES(defined as 12) will cause out of bounds access.
  */
  uint8 httpDataBuf[ AUDIO_BYTES_MAX ];
  uint32 wHttpBufOffset = 0;
  AudioDataFormatType audioFormatType;

  if( FetchBufferedDataSize && FetchBufferedData )
  {
    uint32 userData = 0;
    boolean bEndOfData = false;
    /* Get down loaded data offset from OEM */
    FetchBufferedDataSize( userData, &wHttpBufOffset,&bEndOfData, handle);
    if( wHttpBufOffset >= AUDIO_BYTES_MAX )
    {
      /* Get the data from OEM, use default track id 0x0 */
      (void)FetchBufferedData( httpDataBuf, AUDIO_BYTES_MAX, 0, 0x0, handle);
      audioFormatType = audio_Format_DetectType(httpDataBuf, AUDIO_BYTES_MAX);
      if(audioFormatType)
      {
        return audioFormatType;
      }
    }
  }
  return AUDIO_UNKNOWN;
}
#endif  /*FEATURE_QTV_GENERIC_AUDIO_FORMAT*/

#ifdef FEATURE_QTV_AVI
/* ======================================================================
FUNCTION
  FileBase::IsAVIFile

DESCRIPTION
  Returns True if the file is AVI otherwise, returns FALSE;

INPUT/OUTPUT
  filename: File Name to determine whether it's a divx/avi file.
  pBuf    : DIVX_AVI_FILE_SIGNATURE_BYTES bytes have already been read into pBuf.
            Compare if it's matches with DIVX_AVI_FileIdentifier.
  bUseExtension: When true, QTV will inspect the file extension rather than 
                 opening and reading first few bytes to determine whether it's
                 a avi file.


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
 Returns True is the file is AVI otherwise, returns FALSE;

SIDE EFFECTS
  None
========================================================================== */
bool FileBase::IsAVIFile(OSCL_STRING filename,uint8* pBuf,bool bUseExtension)
{
#ifdef FEATURE_QTV_USE_FILE_EXTENSION_FOR_MEDIA_TYPE
  if(
       (bUseExtension) &&
                         (
                           (ZUtils::FindR( (const char*)filename.get_cstr(),".divx") != ZUtils::npos) ||
                           (ZUtils::FindR( (const char*)filename.get_cstr(),".avi") != ZUtils::npos) 
                         )
    )
   {
     return true;
   }   
  else
#endif
   {
     if(pBuf)
     {
       if ( (!memcmp(pBuf, "RIFF", DIVX_AVI_FILE_SIGNATURE_BYTES)) && (!memcmp(pBuf+8, "AVI", AVI_FILE_SIGNATURE_BYTES)) )
       return true;
     }
   }
   return false;
}

/* ======================================================================
FUNCTION
  FileBase::IsAVIFile

DESCRIPTION
  Returns True if the file is AVI otherwise, returns FALSE;

INPUT/OUTPUT


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
 Returns True is the file is AVI otherwise, returns FALSE;

SIDE EFFECTS
  None
========================================================================== */
bool FileBase::IsAVIFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle)
{
  uint8 httpDataBuf[ DIVX_AVI_FILE_SIGNATURE_BYTES+1 ];
  uint32 wHttpBufOffset = 0;
  boolean bEndOfData = false;

  if( FetchBufferedDataSize && FetchBufferedData )
  {
     /* Get down loaded data offset from OEM */
     /* We will only pass 0 for userdata now since we don't have ixstream implementation for AVI*/
     FetchBufferedDataSize( 0, &wHttpBufOffset, &bEndOfData, handle);

     if( wHttpBufOffset >= DIVX_AVI_FILE_SIGNATURE_BYTES )
     {
       /* Get the data from OEM, use default track id 0x0 */
       (void)FetchBufferedData( httpDataBuf, DIVX_AVI_FILE_SIGNATURE_BYTES, 0, 0x0, handle);

       if( !memcmp(httpDataBuf, "RIFF", DIVX_AVI_FILE_SIGNATURE_BYTES) )
       {
         return true;
       }
     }
  }
  return false;
}
#endif//#ifdef FEATURE_QTV_AVI
/* ======================================================================
FUNCTION
  FileBase::IsMP4_3GPFile

DESCRIPTION
  Returns True if the file is MP4 or 3GP otherwise, returns FALSE;

INPUT/OUTPUT


DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
 Returns True is the file is MP4 or 3GP otherwise, returns FALSE;

SIDE EFFECTS
  None
========================================================================== */
bool FileBase::IsMP4_3GPFile(QtvPlayer::FetchBufferedDataSizeT FetchBufferedDataSize, QtvPlayer::FetchBufferedDataT FetchBufferedData, QtvPlayer::InstanceHandleT handle)
{
  uint8 httpDataBuf[ MPG4_FILE_SIGNATURE_BYTES ];
  uint32 wHttpBufOffset = 0;
  boolean bEndOfData = false;

  if( FetchBufferedDataSize && FetchBufferedData )
  {
     /* Get down loaded data offset from OEM */
     /* We will only pass 0 for userdata now since we don't have ixstream implementation for 3GP*/
     FetchBufferedDataSize( 0, &wHttpBufOffset, &bEndOfData, handle);

     if( wHttpBufOffset >= MPG4_FILE_SIGNATURE_BYTES * 2 )
     {
       (void)FetchBufferedData( httpDataBuf, MPG4_FILE_SIGNATURE_BYTES, 4, 0x0, handle);

       if( !memcmp(httpDataBuf, MP4_3GP_SIG_BYTES1, MPG4_FILE_SIGNATURE_BYTES) ||
           !memcmp(httpDataBuf, MP4_3GP_SIG_BYTES2, MPG4_FILE_SIGNATURE_BYTES) ||
           !memcmp(httpDataBuf, MP4_3GP_SIG_BYTES3, MPG4_FILE_SIGNATURE_BYTES) )
       {
         return true;
       }
     }
  }
  return false;
}

/* ======================================================================
FUNCTION
  FileBase::IsMP4_3GPFile

DESCRIPTION
  Static method to Returns True if the file is MP4 or 3GP otherwise, returns FALSE.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.

========================================================================== */
bool FileBase::IsMP4_3GPFile(uint8* pBuf, uint32 mp4size)
{
  if(pBuf && mp4size >=4 )
  {
    if ( (!memcmp(pBuf, MP4_3GP_SIG_BYTES1, MPG4_FILE_SIGNATURE_BYTES)) ||
         (!memcmp(pBuf, MP4_3GP_SIG_BYTES2, MPG4_FILE_SIGNATURE_BYTES)) ||
         (!memcmp(pBuf, MP4_3GP_SIG_BYTES3, MPG4_FILE_SIGNATURE_BYTES)) )
      return true;
  }
  return false;
}

