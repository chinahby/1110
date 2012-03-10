/*===========================================================================

FILE: OEMMovieFile.c

SERVICES: IMovieFile interface implementation

DESCRIPTION
   This file implements BREW IMovieFile.  IMovieFile is an interface used to
   read and write audio and video stream data in a movie file.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

    Copyright ?2003-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#ifdef FEATURE_BREW_IMOVIEFILE

#include "videofmt.h"         // Video Format Services (read/write movies)

#ifdef FEATURE_MP4_TRANSCODER
#include "mp4_transcoder.h"
#include "qcamcorder.bid"
#include "AEEMediaUtil.h"
#include "videoeng_amc.h"
#endif
#include "AEEMediaMPEG4.h"
#include "OEMMediaMPEG4.h"
#include "AEEMovieFile.h"     // IMovieFile interface
#include "AEEStdlib.h" 
#include "AEE_OEM.h"          // AEE OEM interface header
#include "AEEMediaFormats.h"  // Pre-defined BREW MultiMedia Formats
#include "OEMFS.h"            // OEM File System Interface

#include "msg.h"
#include "assert.h"

/*===========================================================================

                      MACRO DEFINITIONS

===========================================================================*/

// This is the maximum number of UUID atoms we support reading out of a movie
// file.  If a movie file has more than this many, only the first atoms will
// be available to read back.
#define MOVIEFILE_MAX_USER_ATOMS             10
#define DEFAULT_HEADER_SIZE 30

// This is the maximum number of samples of stream data to write to the video
// format services per call.  This is defined so that we can allocate a cache
// of sample information structures to re-use.
#define MOVIEFILE_BLOCK_SIZE                 100

// This is the desired minimum size of each chunk of media data to write out
// to the output at once.
#define MOVIEFILE_OPTIMAL_CHUNK_SIZE         49152  /* 3 sec at 128Kbps */

// This is the number of bytes to allocate for the movie stream buffers.  It
// should be at least MOVIEFILE_OPTIMAL_CHUNK_SIZE plus the largest expected
// sample size, otherwise it will overflow before it is flushed out by the
// video format services.
#define MOVIEFILE_STREAM_BUF_SIZE            66536  /* 4 sec @ 128Kb + 1000 */

// This is the maximum number of samples that we support recording for any one
// movie stream.
#define MOVIEFILE_MAX_SAMPLES                9000  /* 5 minutes at 30fps */

// This is the maximum number of media data chunks that we support writing for
// any one movie stream.
#define MOVIEFILE_MAX_CHUNKS                 100   /* 5 min at 3 sec/chunk */

// This is the maximum number of bytes to allocate for the decoder specific
// information for any one movie stream.
#define MOVIEFILE_STREAM_HEADER_MAX          64

// This is the rate, in seconds, at which to reset the sample-to-chunk mapping
// table run-length encoding algorithm, which tries to maintain a constant
// number of samples per chunk.  Resetting the algorithm allows more samples
// to be combined into chunks, at the expense of adding at least one 'stsc'
// table entry every time a reset is done.
#define MOVIEFILE_STSC_ALGO_RESET_RATE       5

/* These are the sizes of packets, in bytes, for different vocoder types. */
#define VOCODER_PACKET_SIZE_13K_FULL         35
#define VOCODER_PACKET_SIZE_13K_HALF         17
#define VOCODER_PACKET_SIZE_EVRC             23
#define VOCODER_PACKET_SIZE_AMR              32  /* 12.2 kbps mode */

#define IMOVIEFILE_MP4_MOVIE_EXTENSION ".mp4"
#define IMOVIEFILE_3G_MOVIE_EXTENSION ".3g2"
#define IMOVIEFILE_AMC_MOVIE_EXTENSION ".amc"
#define IMOVIEFILE_3GP_MOVIE_EXTENSION ".3gp"
#define IMOVIEFILE_K3G_MOVIE_EXTENSION ".k3g"
#define IMOVIEFILE_SKM_MOVIE_EXTENSION ".skm"
#define IMOVIEFILE_TMP_VIDEO_FILE   "fs:/~/media/tmpvid.mp4"
#define I_FRAME 0

/* This is the height to make the box for a timed text stream. */
#define MOVIEFILE_TEXT_BOX_HEIGHT            30

/* This is the size to make the font for 3GPP timed text. */
#define MOVIEFILE_TEXT_FONT_SIZE             12

/*===========================================================================

                      PRIVATE TYPES

===========================================================================*/

// This structure is used internally to keep track of where user data atoms
// are in the movie file.
typedef struct AEEMovieFileUserAtom
{
   uint32  pos;          // file absolute offset of user data atom contents
   uint32  size;         // total bytes in user data atom contents
   uint8   uuid [16];    // UUID of user data atom
} AEEMovieFileUserAtom;

// This structure contains a segment header.
typedef struct AEEMovieSegmentInfo
{
   uint32    cchSegment;
   uint8   * rgchSegment;
} AEEMovieSegmentInfo;

// This structure is used internally to keep a copy of the segment headers
// for streams in the movie file.
typedef struct AEEMovieFileInternalStreamInfo
{
   uint32                 nSegments;
   AEEMovieSegmentInfo  * pSegments;
} AEEMovieFileInternalStreamInfo;

/*===========================================================================

                      IMovieFile DECLARATIONS

===========================================================================*/
struct IMovieFile
{
   // This "system object" is used to automatically clean up resources when
   // the application allocating this interface exits.

   AEEVTBL(IMovieFile)   *pvt;
   AEECallback            m_so;

   // This is the interface to the main BREW shell object.
   IShell               * m_pIShell;

   // This is the reference counter for the object.  When it reaches zero, the
   // object is automatically destroyed.
   int32                  m_nRefs;

   // This is a cache of the movie file format ID.
   AEECLSID               m_movieFormat;

   // These flags indicate different modes of operation in which the object is
   // involved.
   flg                    m_bOpen          : 1;  // file is open
   flg                    m_bRecord        : 1;  // movie is being written
   flg                    m_bStatusRcvd    : 1;  // status callback received

   // If the movie file is open for reading, this is the handle to the file in
   // the file system.
   void                 * m_fsHandle;

   // These are callback functions into video format services, used to work
   // with this lower layer.
   video_fmt_end_cb_func_type                   m_pVideoFmtEnd;
   video_fmt_continue_cb_func_type              m_pVideoFmtContinue;
   video_fmt_sample_info_cb_func_type           m_pVideoFmtSampleInfo;
   video_fmt_read_cb_func_type                  m_pVideoFmtRead;
   video_fmt_write_cb_func_type                 m_pVideoFmtWrite;
   video_fmt_write_text_cb_func_type            m_pVideoFmtWriteText;
   video_fmt_write_header_cb_func_type          m_pVideoFmtWriteHeader;
   video_fmt_write_header_text_cb_func_type     m_pVideoFmtWriteHeaderText;
   video_fmt_write_uuid_cb_func_type            m_pVideoFmtWriteUuid;
   video_fmt_alloc_text_cb_func_type            m_pVideoFmtAllocText;
   video_fmt_free_text_cb_func_type             m_pVideoFmtFreeText;
   video_fmt_modify_style_text_cb_func_type     m_pVideoFmtModifyStyle;
   video_fmt_modify_hl_text_cb_func_type        m_pVideoFmtModifyHlText;
   video_fmt_modify_hl_color_text_cb_func_type  m_pVideoFmtModifyHlColor;
   video_fmt_modify_karaoke_text_cb_func_type   m_pVideoFmtModifyKaraoke;
   video_fmt_modify_scroll_text_cb_func_type    m_pVideoFmtModifyScroll;
   video_fmt_modify_link_text_cb_func_type      m_pVideoFmtModifyLink;
   video_fmt_modify_box_text_cb_func_type       m_pVideoFmtModifyBox;
   video_fmt_modify_blink_text_cb_func_type     m_pVideoFmtModifyBlink;
   video_fmt_modify_wrap_text_cb_func_type      m_pVideoFmtModifyWrap;

   // These are server data pointers used with the callback functions above.
   void                 * m_pVideoFmtContext;
   void                 * m_pVideoFmtStreamContext;

   // This flag indicates whether or not we're currently reading from a
   // stream.
   boolean                m_bReadingStream;

   // This records the latest status indicator from the video format services.
   video_fmt_status_type  m_videoFmtStatus;

   // This records the total processed byte count from the last video format
   // services I/O request.
   uint32                 m_nIOCount;

   // This array caches the UUID's, positions, and sizes of all the user data
   // atoms in the movie file, if it is an MP4 file.
   uint32                 m_nUserAtoms;
   AEEMovieFileUserAtom   m_userAtoms [MOVIEFILE_MAX_USER_ATOMS];

   // This is a variable-size array of stream information structures, gathered
   // from parsing a movie file being read.  The "m_nStreams" is also used
   // when writing a movie, to remember how many streams are being written.
   uint16                 m_nStreams;
   AEEMovieStreamInfo   * m_pStreams;

   // This is a variable-size array of internal stream information structures,
   // gathered from parsing a movie file being read.
   uint16                            m_nStreamsInternal;
   AEEMovieFileInternalStreamInfo  * m_pStreamsInternal;

   //File info
   video_fmt_file_info_type  *m_fileInfo;


   // This is a cache of video format services sample information structures,
   // used to speed up calls to write stream data.
   video_fmt_sample_info_type  * m_pSampleInfo;

   // This array keeps track of how many bytes of header information were
   // specified by the client to go into each stream, when writing a movie
   // file.
   uint32               * m_pHeaderSizes;
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif


#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   int32 nVideoTimeScale;
   int32 last_time;
   int32 offset;
   int32 frame_num;


   int32 original_timestamp1;
#endif
};

#ifdef FEATURE_MP4_TRANSCODER
typedef struct CMovieFile{
   boolean bTranscodeHeader;         //whether we need to write the header
                                     //TRUE means header has not been written
   AEEMediaData *md;                 //pointer to media data
   AEEMovieFileInfo *mfi;           //pointer to movie file info
   int32 dwTotalMsec;               //total time of the clip
   xcoder_audio_type        transcode_audio_format; //target audio format
   xcoder_video_type        transcode_video_format; //target video format
   IMedia *pMedia;                  //Pointer to imedia object
   byte *pAudioHeaderContents;      //audio header
   byte *pVideoHeaderContents;      //video header
   int32 audioHeaderSize;           //audio header size
   int32 videoHeaderSize;           //video header
   char * pszOutputFile;
   IShell *pIShell;
   boolean bTerminated;
   boolean bTranscoderStarted;

}CMovieFile;
#endif
// This is the standard method for allocating a new MovieFile object.  As a
// static extension, the module is linked into the global "gICList" table
// through the IMovieFile class ID and this method.
extern int     IMovieFile_New(IShell * ps, AEECLSID cls, void **ppif);

// These are the prototypes for the public methods of the MovieFile
// implementation, used to construct the virtual function pointer table for
// MovieFile objects.
static uint32  IMovieFile_AddRef(IMovieFile * po);
static uint32  IMovieFile_Release(IMovieFile * po);
static int     IMovieFile_QueryInterface(IMovieFile * po, AEECLSID idReq,
                                         void ** ppo);
static int     IMovieFile_Open(IMovieFile * po, const char * pszFileName,
                               boolean bRecord, AEECLSID format,
                               AEEMovieFileInfo * pInfo);
static int     IMovieFile_ReadUserAtom(IMovieFile * po, uint16 nAtom,
                                       void * pBuffer, uint32 * pnSize);
static int     IMovieFile_ReadStream(IMovieFile * po, uint16 nStream,
                                     uint32 nSamples, uint32 nOffset,
                                     void * pBuffer, uint32 * pnSize);
static int     IMovieFile_GetSampleInfo(IMovieFile * po, uint16 nStream,
                                        uint32 nSamples, uint32 nOffset,
                                        AEEMovieSampleInfo * pInfo);
static int     IMovieFile_WriteUserAtom(IMovieFile *po, const void * pBuffer,
                                        uint32 dwSize);
static int     IMovieFile_WriteStream(IMovieFile * po, uint16 nStream,
                                      uint32 nSamples,
                                      const AEEMovieSampleInfo * pSampleInfo,
                                      const void * pBuffer);
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
static byte* IMovieFile_SetVOPTime(IMovieFile *po, int32 delta, byte *buffer, uint32 *sizes);
static int IMovieFile_GetVOPTime(IMovieFile *po, byte *buffer, uint32 size);
static int IMovieFile_GetTimeIncRes(IMovieFile *po, byte *header, int size);
extern boolean mp4_decode_process_start_code(
                                     uint8       *pSRCBitstream,   /* pointer to bit stream buffer */
                                     uint32      NumBytes,         /* number of bytes to decode */
                                     uint32      ByteIndex,        /* starting byte index */
                                     uint32      TimeStamp,        /* frame time stamp */
                                     boolean     fExternalClock    /* external clock source used */
                                     );
#endif
#ifdef FEATURE_MP4_TRANSCODER
static int IMovieFile_TranscoderStart(IMovieFile * po, //output file
                                     IMovieFile * pi,
                                     const char *pszInputFile,
                                     const char *pszOutputFile,
                                     xcoder_audio_type        target_audio_format,
                                     xcoder_video_type        target_video_format,
                                     AEEMovieFileInfo *temp
                                     );

#endif

static int IMovieFile_TextNewSegment(IMovieFile * po, uint16 nStream,
                                     const AEEMovieTextSegment * pSegment);
static int IMovieFile_TextNewSample(IMovieFile * po, const char * rgchText,
                                    uint16 cchText, void ** phSample);
static int IMovieFile_TextDeleteSample(IMovieFile * po, void * hSample);
static int IMovieFile_TextSubmitSample(IMovieFile * po, void * hSample,
                                       uint16 nStream, uint32 dwDelta);
static int IMovieFile_TextModifyStyle(IMovieFile * po, void * hSample,
                                      const AEEMovieTextStyle * rgStyle,
                                      uint16 cStyle);
static int IMovieFile_TextModifyHighlight(IMovieFile * po, void * hSample,
                                          uint16 iStart, uint16 iEnd);
static int IMovieFile_TextModifyHighlightColor(IMovieFile * po,
                                               void * hSample,
                                               RGBVAL color);
static int IMovieFile_TextModifyKaraoke(IMovieFile * po, void * hSample,
                                        uint32 dwStart,
                                        const AEEMovieTextKaraoke * rgKaraoke,
                                        uint16 cKaraoke);
static int IMovieFile_TextModifyScroll(IMovieFile * po, void * hSample,
                                       uint32 dwScrollDelay);
static int IMovieFile_TextModifyLink(IMovieFile * po, void * hSample,
                                     uint16 iStart, uint16 iEnd,
                                     const char * rgchURL, uint8 cchURL,
                                     const char * rgchAlt, uint8 cchAlt);
static int IMovieFile_TextModifyBox(IMovieFile * po, void * hSample,
                                    int16 nLeft, int16 nTop,
                                    int16 nRight, int16 nBottom);
static int IMovieFile_TextModifyBlink(IMovieFile * po, void * hSample,
                                      uint16 iStart, uint16 iEnd);
static int IMovieFile_TextModifyWrap(IMovieFile * po, void * hSample,
                                     flg fAutomatic);
static int IMovieFile_ReadSegmentHeader(IMovieFile * po, uint16 nStream,
                                        uint16 nSegment,
                                        void * pBuffer, uint32 * pnSize);
static int IMovieFile_WriteSegmentHeader(IMovieFile * po, uint16 nStream,
                                         const void * pBuffer,
                                         uint32 dwSize);


// This is the virtual function pointer table for this implementation of
// IMovieFile.
static const AEEVTBL(IMovieFile) gvtIMovieFile = 
{
   IMovieFile_AddRef,
   IMovieFile_Release,
   IMovieFile_QueryInterface,
   IMovieFile_Open,
   IMovieFile_ReadUserAtom,
   IMovieFile_ReadStream,
   IMovieFile_GetSampleInfo,
   IMovieFile_WriteUserAtom,
   IMovieFile_WriteStream,
#ifdef FEATURE_MP4_TRANSCODER
   IMovieFile_TranscoderStart,
#endif
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
   IMovieFile_SetVOPTime,
   IMovieFile_GetVOPTime,
   IMovieFile_GetTimeIncRes,
#endif
   IMovieFile_TextNewSegment,
   IMovieFile_TextNewSample,
   IMovieFile_TextDeleteSample,
   IMovieFile_TextSubmitSample,
   IMovieFile_TextModifyStyle,
   IMovieFile_TextModifyHighlight,
   IMovieFile_TextModifyHighlightColor,
   IMovieFile_TextModifyKaraoke,
   IMovieFile_TextModifyScroll,
   IMovieFile_TextModifyLink,
   IMovieFile_TextModifyBox,
   IMovieFile_TextModifyBlink,
   IMovieFile_TextModifyWrap,
   IMovieFile_ReadSegmentHeader,
   IMovieFile_WriteSegmentHeader,
};
boolean OEMMediaMPEG42PV_GetVideoTrackHeader(byte * pBuf, uint32 *pBufSize);
  boolean OEMMediaMPEG42PV_GetAudioTrackHeader(byte * pBuf, uint32 *pBufSize);
#ifndef FEATURE_QTV_VIDEO_DISABLE
  boolean OEMMediaMPEG42PV_GetVideoFrameInfo(uint32 timeStamp,  
                                             AEEDataFrameInfo * pFrameInfo
                                            );
#endif /* FEATURE_QTV_VIDEO_DISABLE */
  boolean OEMMediaMPEG42PV_GetAudioFrameInfo(uint32 timeStamp,  AEEDataFrameInfo * pFrameInfo);
  uint32 OEMMediaMPEG42PV_GetMaxVideoFrameSize(void);
  uint32 OEMMediaMPEG42PV_GetMaxAudioFrameSize(void);

  boolean OEMMediaMPEG42PV_GetAudioFrame( uint32 FrameNum, 
                                        AEEDataFrameInfo * pFrameInfo, byte *pBuf, 
                                        uint32 bufSize  );
#ifndef FEATURE_QTV_VIDEO_DISABLE
  boolean OEMMediaMPEG42PV_GetVideoFrame( uint32 FrameNum, 
                                        AEEDataFrameInfo * pFrameInfo, byte *pBuf, 
                                        uint32 bufSize  );
#endif /* FEATURE_QTV_VIDEO_DISABLE */

// These are the brand names to write into new movie files.
static const char *MP4_MAJOR_BRAND = "isom";
static const char *MP4_COMPAT_BRANDS [] = {
    "mp41", "isom"
};
static const char *AMC_MAJOR_BRAND = "isom";
static const char *AMC_COMPAT_BRANDS [] = {
    "mp41"
};
static const char *_3GP_MAJOR_BRAND = "3gp5";
static const char *_3GP_COMPAT_BRANDS [] = {
    "3gp5", "isom"
};
static const char *_3G2_MAJOR_BRAND = "3g2a";
static const char *_3G2_COMPAT_BRANDS [] = {
    "3g2a"
};
static const char *K3G_MAJOR_BRAND = "k3g1";
static const char *K3G_COMPAT_BRANDS [] = {
    "k3g1"
};
static const char *SKM_MAJOR_BRAND = "skm2";
static const char *SKM_COMPAT_BRANDS [] = {
    "skm2", "k3g1", "3gp5"
};

#ifdef FEATURE_MP4_TRANSCODER
static xcoder_status_type xcode_cmd; 
static uint32 dCurrentAudioFrameNum, dCurrentVideoFrameNum;
LOCAL CMovieFile *loc;
#endif
LOCAL int32 mallcs =0;
/*===========================================================================

                     IMovieFile Local Function Prototypes

===========================================================================*/

static void IMovieFile_Close(IMovieFile * po);
static int IMovieFile_AwaitDone(IMovieFile * po);
static void IMovieFile_VideoFmtCallback(video_fmt_status_type status,
                                        void * client_data,
                                        video_fmt_status_cb_info_type * info,
                                        video_fmt_end_cb_func_type end);

/*===========================================================================

                     IMovieFile Function Definitions

===========================================================================*/


#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
/*==================================================================

==================================================================*/
static void IMovieFile_Close(IMovieFile * po)
{
   // Return early if file is not open.
   if (!po->m_bOpen)
   {
      return;
   }

   // Deallocate memory used for stream information structures.
   if (po->m_pStreams)
   {
      FREE (po->m_pStreams);
      po->m_pStreams = NULL;
   }
   if (po->m_nStreamsInternal)
   {
      int i;
      for (i = 0; i < po->m_nStreamsInternal; ++i)
      {
          if (po->m_pStreamsInternal [i].nSegments)
          {
              int j;
              for (j = 0; j < po->m_pStreamsInternal [i].nSegments; ++j)
              {
                  if (po->m_pStreamsInternal [i].pSegments [j].cchSegment)
                  {
                      FREE (po->m_pStreamsInternal [i].pSegments [j]
                            .rgchSegment);
                  }
              }
              FREE (po->m_pStreamsInternal [i].pSegments);
          }
      }
      FREE (po->m_pStreamsInternal);
      po->m_pStreamsInternal = NULL;
      po->m_nStreamsInternal = 0;
   }

   // Deallocate memory used for sample information structures.
   if (po->m_pSampleInfo)
   {
      FREE (po->m_pSampleInfo);
      po->m_pSampleInfo = NULL;
   }

   // Deallocate memory used for header size array, if any.
   if (po->m_pHeaderSizes)
   {
       FREE (po->m_pHeaderSizes);
       po->m_pHeaderSizes = NULL;
   }

   // Close video format services.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtEnd (po->m_pVideoFmtContext);
   IMovieFile_AwaitDone (po);

   // Clear array of user data atoms.
   po->m_nUserAtoms = 0;

   // Close the file itself.
   if (po->m_fsHandle)
   {
      OEMFS_Close (po->m_fsHandle);
      po->m_fsHandle = NULL;
   }
   po->m_bOpen = FALSE;
}

/*==================================================================

==================================================================*/
static int IMovieFile_AwaitDone(IMovieFile * po)
{
   // Repeatedly call video format services until a done status indication
   // is received.
   while (!po->m_bStatusRcvd)
   {
       po->m_pVideoFmtContinue (po->m_bReadingStream
                                ? po->m_pVideoFmtStreamContext
                                : po->m_pVideoFmtContext);
   }

   // Interpret video format services status as success or failure.
   switch (po->m_videoFmtStatus)
   {
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
   case VIDEO_FMT_INFO:
   case VIDEO_FMT_WRITER_READY:
   case VIDEO_FMT_IO_DONE:
   case VIDEO_FMT_DONE:
      return SUCCESS;
   default:
      return EFAILED;
   }
}

/*==================================================================

==================================================================*/
static void IMovieFile_VideoFmtCallback(video_fmt_status_type status,
                                        void * client_data,
                                        video_fmt_status_cb_info_type * info,
                                        video_fmt_end_cb_func_type end)
{
   uint32 nStream;
   uint8 buffer[4];
   AEEDataFrameInfo *pFrameInfo= NULL;
   byte *pFrameContents = NULL;
   int i=0;
   int nErr;

   // Handle status indication.  If indication is a done or error indication,
   // store it in the MovieFile object.
   IMovieFile *po = (IMovieFile *)client_data;
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
   po->m_pVideoFmtEnd = end;
   switch (status)
   {
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
   case VIDEO_FMT_ALLOC:
      info->alloc.ptr = MALLOC (info->alloc.size);
      break;

   case VIDEO_FMT_FREE:
      FREE (info->free.ptr);
      break;

   case VIDEO_FMT_GET_DATA:
      OEMFS_Seek (po->m_fsHandle, _SEEK_START, info->get_data.offset, &nErr);
      if(nErr == SUCCESS)
      {
          info->get_data.num_bytes = OEMFS_Read
              (po->m_fsHandle, info->get_data.buffer,
               info->get_data.num_bytes, &nErr);
      }
      else
      {
          info->get_data.num_bytes = 0;
      }
      po->m_pVideoFmtContinue = info->get_data.callback_ptr;
      if (po->m_bReadingStream)
      {
          po->m_pVideoFmtStreamContext = info->get_data.server_data;
      }
      else
      {
         
#ifdef FEATURE_QTV_RANDOM_ACCESS_REPOS 		  
#error code not present
#else
          po->m_pVideoFmtContext = info->get_data.server_data;
#endif
      }
      break;

   case VIDEO_FMT_PUT_DATA:
      OEMFS_Seek (po->m_fsHandle, _SEEK_START, info->put_data.offset, &nErr);     
      if(SUCCESS == nErr) 
      {
          info->put_data.num_bytes = OEMFS_Write
                 (po->m_fsHandle, info->put_data.buffer,
                  info->put_data.num_bytes, &nErr);
      }
      else
      {
          info->put_data.num_bytes = 0;
      }
      po->m_pVideoFmtContinue = info->put_data.callback_ptr;
      po->m_pVideoFmtContext = info->put_data.server_data;
      break;

   case VIDEO_FMT_CONTINUE:
      po->m_pVideoFmtContinue = info->cont.callback_ptr;
      if (po->m_bReadingStream)
      {
          po->m_pVideoFmtStreamContext = info->get_data.server_data;
      }
      else
      {
          po->m_pVideoFmtContext = info->get_data.server_data;
      }
      break;

   case VIDEO_FMT_SPACE_CHECK:
     // Done indication - set status and flag indicating status received.
         po->m_videoFmtStatus = status;
         po->m_bStatusRcvd = TRUE;
 
       /* If writing to memory buffer, bytes available is total memory buffer
        ** size minus the bytes we've already used, with a margin.
        */
        
        /* If we reach here, we have no way of knowing how much space is left
        ** on the recording device, so we will return a very large number to
        ** keep the movie writer from stopping.  It is up to the client to
        ** make sure we don't actually run out of space!
        */
        info->space_check.bytes_available = (uint32) -1;
        
        break;
   case VIDEO_FMT_INFO:
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
      // Done indication - set status and flag indicating status received.
      po->m_videoFmtStatus = status;
      po->m_bStatusRcvd = TRUE;

      // Save various callback functions given, and video format services
      // context.
      po->m_pVideoFmtRead = info->info.read_cb;
      po->m_pVideoFmtSampleInfo = info->info.sample_info_cb;
      po->m_pVideoFmtContext = info->info.server_data;
      //Allocate memory for file info
      po->m_fileInfo = MALLOC(sizeof(video_fmt_file_info_type));
      if(!po->m_fileInfo)
      {
          return;
      }

      po->m_fileInfo->movie_timescale = info->info.file_info.movie_timescale;      /* time units per sec for whole movie      */
      // Allocate memory for stream data, and fill in information from the
      // stream data structures given.
      po->m_nStreams = info->info.num_streams;
      po->m_pStreams = MALLOC (po->m_nStreams
                               * sizeof (AEEMovieStreamInfo));
      if(!po->m_pStreams)
      {
          FREE (po->m_fileInfo);
          po->m_fileInfo = NULL;
          return;
      }
      if (po->m_nStreamsInternal < po->m_nStreams)
      {
          AEEMovieFileInternalStreamInfo *newStreams;
          if (po->m_nStreamsInternal)
          {
              newStreams = REALLOC
                  (po->m_pStreamsInternal,
                   po->m_nStreams * sizeof (AEEMovieFileInternalStreamInfo));
          }
          else
          {
              newStreams = MALLOC
                  (po->m_nStreams * sizeof (AEEMovieFileInternalStreamInfo));
          }
          if(newStreams)
          {
              po->m_pStreamsInternal = newStreams;
              po->m_nStreamsInternal = po->m_nStreams;
          }
          else
          {
              FREE (po->m_pStreams);
              po->m_pStreams = NULL;
              FREE (po->m_fileInfo);
              po->m_fileInfo = NULL;
              return;
          }
      }
      for (nStream = 0; nStream < po->m_nStreams; ++nStream)
      {
         po->m_pStreams [nStream].cbSize = sizeof (AEEMovieStreamInfo);
         po->m_pStreams [nStream].format = AEECLSID_MEDIA;
         switch (info->info.streams [nStream].type)
         {
         case VIDEO_FMT_STREAM_VIDEO:
            switch (info->info.streams [nStream].subinfo.video.format)
            {
              case VIDEO_FMT_STREAM_VIDEO_MPEG4:
              case VIDEO_FMT_STREAM_VIDEO_H263:
               po->m_pStreams [nStream].format = AEECLSID_MPEG4_VIDEO;
               break;
            }
            po->m_pStreams [nStream].nWidth
                = info->info.streams [nStream].subinfo.video.width;
            po->m_pStreams [nStream].nHeight
                = info->info.streams [nStream].subinfo.video.height;
            po->m_pStreams [nStream].nOriginX
                = info->info.streams [nStream].tkhd_origin_x;
            po->m_pStreams [nStream].nOriginY
                = info->info.streams [nStream].tkhd_origin_y;

            // For MPEG-4 video, make copy of decoder specific info.
            if (info->info.streams [nStream].subinfo.video.format
                == VIDEO_FMT_STREAM_VIDEO_MPEG4)
            {
                // Store a copy of the decoder specific information.
                po->m_pStreamsInternal [nStream].pSegments
                    = MALLOC (sizeof (AEEMovieSegmentInfo));
                if (po->m_pStreamsInternal [nStream].pSegments)
                {
                    po->m_pStreamsInternal [nStream].pSegments [0].cchSegment
                        = info->info.streams [nStream].header;
                    po->m_pStreamsInternal [nStream].pSegments [0].rgchSegment
                        = MALLOC (info->info.streams [nStream].header);
                    if (po->m_pStreamsInternal [nStream]
                        .pSegments [0].rgchSegment)
                    {
                        MEMCPY (po->m_pStreamsInternal [nStream]
                                .pSegments [0].rgchSegment,
                                info->info.streams [nStream].dec_specific_info
                                .info, info->info.streams [nStream].header);
                        po->m_pStreamsInternal [nStream].nSegments = 1;
                    }
                    else
                    {
                        FREE (po->m_pStreamsInternal [nStream].pSegments);
                    }
                }
            }
            break;

         case VIDEO_FMT_STREAM_AUDIO:
            switch (info->info.streams [nStream].subinfo.audio.format)
            {
            case VIDEO_FMT_STREAM_AUDIO_PUREVOICE:
            pFrameInfo = MALLOC(sizeof(AEEDataFrameInfo));
            pFrameContents = MALLOC(50 *sizeof(byte));
            if(!pFrameContents || !pFrameInfo)
            {po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
               po->m_pStreams [nStream].nSubFormat
                   = MM_QCP_FORMAT_FIXED_FULL_13K;
               break;
            }
            if(OEMMediaMPEG42PV_GetAudioFrame(0, pFrameInfo, pFrameContents,
                                    50))
            {if(*pFrameContents==0x03)
             { po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
               po->m_pStreams [nStream].nSubFormat
                 = MM_QCP_FORMAT_FIXED_HALF_13K;
              }
                else if(*pFrameContents == 0x04)
                 { po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
                   po->m_pStreams [nStream].nSubFormat
                   = MM_QCP_FORMAT_FIXED_FULL_13K;
                 }
                }
                else
                { po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
                 po->m_pStreams [nStream].nSubFormat  
                 = MM_QCP_FORMAT_FIXED_FULL_13K;
                }
                FREE(pFrameInfo);
                FREE(pFrameContents);
              break;

            case VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL:
               po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
               po->m_pStreams [nStream].nSubFormat
                   = MM_QCP_FORMAT_FIXED_FULL_13K;
               break;

            case VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF:
               po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
               po->m_pStreams [nStream].nSubFormat
                   = MM_QCP_FORMAT_FIXED_HALF_13K;
               break;

            case VIDEO_FMT_STREAM_AUDIO_EVRC:
               po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
               po->m_pStreams [nStream].nSubFormat
                   = MM_QCP_FORMAT_FIXED_FULL_EVRC;
               break;

            case VIDEO_FMT_STREAM_AUDIO_AMR:
               po->m_pStreams [nStream].format = AEECLSID_MEDIAQCP;
               po->m_pStreams [nStream].nSubFormat
                   = MM_QCP_FORMAT_FIXED_12_2_AMR;
               break;
            }
            break;

         case VIDEO_FMT_STREAM_TEXT:
            po->m_pStreams [nStream].format = AEECLSID_3GPP_TEXT;
            po->m_pStreams [nStream].nWidth
                = info->info.streams [nStream].tkhd_width;
            po->m_pStreams [nStream].nHeight
                = info->info.streams [nStream].tkhd_height;
            po->m_pStreams [nStream].nOriginX
                = info->info.streams [nStream].tkhd_origin_x;
            po->m_pStreams [nStream].nOriginY
                = info->info.streams [nStream].tkhd_origin_y;
            break;

         case VIDEO_FMT_STREAM_DATA:
            switch (info->info.streams [nStream].subinfo.data.format)
            {
            case VIDEO_FMT_STREAM_DATA_BIFS:
               po->m_pStreams [nStream].format = AEECLSID_MPEG4_DATA;
               po->m_pStreams [nStream].nSubFormat
                   = MOVIEFILE_M4S_TYPE_BIFS;
               break;

            case VIDEO_FMT_STREAM_DATA_OD:
               po->m_pStreams [nStream].format = AEECLSID_MPEG4_DATA;
               po->m_pStreams [nStream].nSubFormat
                   = MOVIEFILE_M4S_TYPE_OD;
               break;
            }
            break;
         }
         po->m_pStreams [nStream].nTimescale
             = info->info.streams [nStream].media_timescale;
         po->m_pStreams [nStream].dwLength
             = info->info.streams [nStream].media_duration;
         po->m_pStreams [nStream].dwFrames
             = info->info.streams [nStream].frames;
         po->m_pStreams [nStream].dwBytes
             = info->info.streams [nStream].bytes;
         po->m_pStreams [nStream].dwLargestFrame
             = info->info.streams [nStream].largest;
         po->m_pStreams [nStream].dwHeaderSize
             = info->info.streams [nStream].header;
         if (po->m_pStreams [nStream].dwLength)
         {
             po->m_pStreams [nStream].dwRate
                 = po->m_pStreams [nStream].dwFrames
                 * po->m_pStreams [nStream].nTimescale
                 / po->m_pStreams [nStream].dwLength;
         }
         else
         {
             po->m_pStreams [nStream].dwRate = 0;
         }
         po->m_pStreams [nStream].rgchLanguage [0] = (char)
             (info->info.streams [nStream].media_language >> 10) + '`';
         po->m_pStreams [nStream].rgchLanguage [1] = (char)
             ((info->info.streams [nStream].media_language >> 5) & 0x1F) + '`';
         po->m_pStreams [nStream].rgchLanguage [2] = (char)
             (info->info.streams [nStream].media_language & 0x1F) + '`';
      }
      break;

   case VIDEO_FMT_WRITER_READY:
      // Done indication - set status and flag indicating status received.
      po->m_videoFmtStatus = status;
      po->m_bStatusRcvd = TRUE;

      // Save various callback functions given, and video format services
      // context.
      po->m_pVideoFmtWrite = info->writer_ready.write_cb;
      po->m_pVideoFmtWriteText = info->writer_ready.write_text_cb;
      po->m_pVideoFmtWriteHeader = info->writer_ready.write_header_cb;
      po->m_pVideoFmtWriteHeaderText = info->writer_ready.write_header_text_cb;
      po->m_pVideoFmtWriteUuid = info->writer_ready.write_uuid_cb;
      po->m_pVideoFmtAllocText = info->writer_ready.alloc_text_cb;
      po->m_pVideoFmtFreeText = info->writer_ready.free_text_cb;
      po->m_pVideoFmtModifyStyle = info->writer_ready.modify_style_text_cb;
      po->m_pVideoFmtModifyHlText = info->writer_ready.modify_hl_text_cb;
      po->m_pVideoFmtModifyHlColor
          = info->writer_ready.modify_hl_color_text_cb;
      po->m_pVideoFmtModifyKaraoke = info->writer_ready.modify_karaoke_text_cb;
      po->m_pVideoFmtModifyScroll = info->writer_ready.modify_scroll_text_cb;
      po->m_pVideoFmtModifyLink = info->writer_ready.modify_link_text_cb;
      po->m_pVideoFmtModifyBox = info->writer_ready.modify_box_text_cb;
      po->m_pVideoFmtModifyBlink = info->writer_ready.modify_blink_text_cb;
      po->m_pVideoFmtModifyWrap = info->writer_ready.modify_wrap_text_cb;
      po->m_pVideoFmtContext = info->writer_ready.server_data;
      break;

   case VIDEO_FMT_IO_DONE:
      // I/O Done indication - set status and flag indicating status received.
      po->m_videoFmtStatus = status;
      po->m_bStatusRcvd = TRUE;

      // Store I/O byte count for later use.
      po->m_nIOCount = info->io_done.bytes;
      break;

   case VIDEO_FMT_DONE:
      // All Done indication - set status and flag indicating status received.
      po->m_videoFmtStatus = status;
      po->m_bStatusRcvd = TRUE;
      break;

   case VIDEO_FMT_HINT:
      // Handle according to movie file format.
      switch (po->m_movieFormat)
      {
      case AEECLSID_MEDIAMPEG4:
         // If atom reported is a user data atom, add atom information to
         // user atom list.
         if (info->hint.mp4->user
             && (po->m_nUserAtoms + 1 < MOVIEFILE_MAX_USER_ATOMS))
         {
            po->m_userAtoms [po->m_nUserAtoms].pos
                = info->hint.mp4->offset;
            po->m_userAtoms [po->m_nUserAtoms].size
                = info->hint.mp4->size;
            MEMCPY (po->m_userAtoms [po->m_nUserAtoms].uuid,
                    (const uint8 *) &info->hint.mp4->type,
                    sizeof (po->m_userAtoms [po->m_nUserAtoms].uuid));
                    MEMCPY(buffer, po->m_userAtoms [po->m_nUserAtoms].uuid, 4);
                    for(i=0; i<4; i++)
                    {po->m_userAtoms [po->m_nUserAtoms].uuid[i] = buffer[3-i];
                    }
            
            ++(po->m_nUserAtoms);
         }
         break;
      }
      break;

   case VIDEO_FMT_TX3G:
      // Expand internal stream structures, if necessary.
      if (po->m_nStreamsInternal <= info->tx3g_atom.stream)
      {
          // Attempt to allocate memory.
          AEEMovieFileInternalStreamInfo *newStreams;
          uint32 numNewStreams = info->tx3g_atom.stream + 1;
          if (po->m_nStreamsInternal)
          {
              newStreams = REALLOC
                  (po->m_pStreamsInternal,
                   numNewStreams * sizeof (AEEMovieFileInternalStreamInfo));
          }
          else
          {
              newStreams = MALLOC
                  (numNewStreams * sizeof (AEEMovieFileInternalStreamInfo));
          }

          // Only update number of structures if memory allocation succeeded.
          if (newStreams)
          {
              po->m_nStreamsInternal = numNewStreams;
              po->m_pStreamsInternal = newStreams;
          }
      }

      // Expand number of segment structures, if necessary, and read in a copy
      // of the atom if we can.
      if (po->m_nStreamsInternal <= info->tx3g_atom.stream)
      {
          // Expand atom list if necessary.
          if (po->m_pStreamsInternal [info->tx3g_atom.stream].nSegments
              <= info->tx3g_atom.cur_index)
          {
              // Attempt to allocate memory.
              AEEMovieSegmentInfo *newSegments;
              uint32 numNewSegments = info->tx3g_atom.cur_index + 1;
              if (po->m_pStreamsInternal [info->tx3g_atom.stream].nSegments)
              {
                  newSegments = REALLOC
                      (po->m_pStreamsInternal [info->tx3g_atom.stream]
                       .pSegments,
                       numNewSegments * sizeof (AEEMovieSegmentInfo));
              }
              else
              {
                  newSegments = MALLOC
                      (numNewSegments * sizeof (AEEMovieSegmentInfo));
              }

              // Only update number of structures if memory allocation
              // succeeded.
              if (newSegments)
              {
                  po->m_pStreamsInternal [info->tx3g_atom.stream].nSegments
                      = numNewSegments;
                  po->m_pStreamsInternal [info->tx3g_atom.stream].pSegments
                      = newSegments;
              }
          }

          // Read in a copy of the atom if we can.
          if (po->m_pStreamsInternal [info->tx3g_atom.stream].nSegments
              <= info->tx3g_atom.cur_index)
          {
              // Allocate memory for the atom.
              po->m_pStreamsInternal [info->tx3g_atom.stream].pSegments
                  [info->tx3g_atom.cur_index].rgchSegment
                  = MALLOC (info->tx3g_atom.size);
              if (po->m_pStreamsInternal [info->tx3g_atom.stream].pSegments
                  [info->tx3g_atom.cur_index].rgchSegment)
              {
                  int ret = SUCCESS;
                  
                  // Seek to position of atom contents in file.
                  OEMFS_Seek (po->m_fsHandle, _SEEK_START,
                              info->tx3g_atom.offset, &ret);
                  if (SUCCESS == ret)
                  {
                      int bytes;

                      // Read atom data directly from the file.
                      bytes = OEMFS_Read
                          (po->m_fsHandle, (char *)
                           po->m_pStreamsInternal [info->tx3g_atom.stream]
                           .pSegments [info->tx3g_atom.cur_index]
                           .rgchSegment, info->tx3g_atom.size, &ret);
                      if(bytes == info->tx3g_atom.size)
                      {
                          // Save copy of atom size.
                          po->m_pStreamsInternal [info->tx3g_atom.stream]
                              .pSegments [info->tx3g_atom.cur_index].cchSegment
                              = info->tx3g_atom.size;
                      }
                      else
                      {
                          FREE (po->m_pStreamsInternal [info->tx3g_atom.stream]
                                .pSegments [info->tx3g_atom.cur_index]
                                .rgchSegment);
                      }
                  }
                  else
                  {
                      FREE (po->m_pStreamsInternal [info->tx3g_atom.stream]
                            .pSegments [info->tx3g_atom.cur_index]
                            .rgchSegment);
                  }
              }
          }
      }
      break;

   case VIDEO_FMT_FAILURE:
   case VIDEO_FMT_DATA_CORRUPT:
   case VIDEO_FMT_BUSY:
      // Error indication - set status and flag indicating status received.
      po->m_videoFmtStatus = status;
      po->m_bStatusRcvd = TRUE;
      break;

   default:
      // Ignore any unknown statuses that may be defined in the future.
      break;
   }
}

/*==================================================================

==================================================================*/
int IMovieFile_New(IShell * ps, AEECLSID cls, void **ppif)
{
   if(ppif == NULL )
   {
     return EFAILED;
   }

   if (cls == AEECLSID_MOVIEFILE)
   {
      // Allocate memory for MovieFile object.
      IMovieFile *po;
      // = (IMovieFile *)AEE_NewClass
       //   ((AEEVTBL(IBase) *)&gvtIMovieFile, sizeof(IMovieFile));
      //if (NULL == po)
     /// {
       //  return ENOMEMORY;
      //}
      if ((po = (IMovieFile *)AEE_NewClass
           ((AEEVTBL(IBase) *)&gvtIMovieFile, sizeof(IMovieFile))) == NULL) {
           return ENOMEMORY;
      }

      // Initialize MovieFile variables.
      po->m_bOpen = FALSE;
      po->m_bRecord = FALSE;
      po->m_bStatusRcvd = FALSE;
      po->m_fsHandle = NULL;
      po->m_pStreams = NULL;
      po->m_pSampleInfo = NULL;
      po->m_pHeaderSizes = NULL;
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
      po->original_timestamp1 = -1;
#endif

      // Save reference to the shell for later use.
      po->m_pIShell = ps;
      ISHELL_AddRef (po->m_pIShell);

#ifdef FEATURE_MP4_TRANSCODER
      loc = (CMovieFile *)MALLOC(sizeof(CMovieFile));
      if(!loc)
      {
        return ENOMEMORY;
      }
      loc->bTranscoderStarted = FALSE;
      mallcs++;
#endif
      // Return new object.
      *ppif = po;
      return SUCCESS;
   }
   else
   {
      return EUNSUPPORTED;
   }
}

/*==================================================================

==================================================================*/
static uint32 IMovieFile_AddRef(IMovieFile * po)
{
   return (++(po->m_nRefs));
}

/*==================================================================

==================================================================*/
static uint32 IMovieFile_Release(IMovieFile * po)
{
#ifdef FEATURE_MP4_TRANSCODER
   char *output;
   char *input;
#endif
   // Decrement reference count, and automatically destroy the object when the
   // reference count reaches zero.
   if (--(po->m_nRefs))
   {
      // Return new reference count.
      return po->m_nRefs;
   }
   else
   {
     

      
    
#ifdef FEATURE_MP4_TRANSCODER
      xcoder_terminate();


      if(loc)
      { if(loc->bTranscoderStarted){
      
        while(!loc->bTerminated)
        {
          OEMOS_Sleep(100);
        }
       }
      }
#endif
       IMovieFile_Close (po);
       // Release IShell reference
        ISHELL_Release (po->m_pIShell); 
        po->m_pIShell = NULL;
      // Close any previously opened file.
        
#ifdef FEATURE_MP4_TRANSCODER
      if(loc){
        if(loc->pMedia)
         {IMEDIA_Release(loc->pMedia);
         loc->pMedia = NULL;
         }
        if(loc->bTranscoderStarted){
          input = MALLOC(100 * sizeof(char));
          output = MALLOC(100 * sizeof(char));
          if(output && input)
          {
#ifdef FEATURE_MM_COMMON_DIR
          STRCPY(input, "shared/");
          STRCPY(output, "shared/");
          if(STRSTR(loc->pszOutputFile, "card0") || STRSTR(loc->pszOutputFile, "mmc"))
          {
            STRCPY(output, "mmc1/");
          }
#else
          STRCPY(input, "brew/mod/qcamcorder/");
          STRCPY(output, "brew/mod/qcamcorder/");
          if(STRSTR(loc->pszOutputFile, "card0") || STRSTR(loc->pszOutputFile, "mmc"))
          {
            STRCPY(output, "mmc1/");
          }
#endif
          STRCAT(input, STRCHR(STRCHR(IMOVIEFILE_TMP_VIDEO_FILE, '/')+1, '/')+1);
          STRCAT(output, STRCHR(STRCHR(loc->pszOutputFile, '/')+1, '/')+1);
          }
          
         video_eng_amc_fixup(input, output);
         FREE(input);
         FREE(output);
        }
        if(loc->pszOutputFile)
        {FREE(loc->pszOutputFile);
          loc->pszOutputFile = NULL;
          mallcs --;
         }
        FREE(loc);
        loc = NULL;
        mallcs--;
      }
#endif
      // Finally, free the IMovieFile instance itself
      FREE (po);
      po = NULL;
      return 0;
   }
}

/*==================================================================

==================================================================*/
static int IMovieFile_QueryInterface(IMovieFile * po, AEECLSID idReq,
                                     void ** ppo)
{
   // Verify given double pointer is not NULL.
   if (!ppo)
   {
      return EBADPARM;
   }

   // Make new reference to desired interface, if supported.
   switch (idReq)
   {
   case AEECLSID_MOVIEFILE:
   case AEECLSID_QUERYINTERFACE:
      *ppo = po;
      IMovieFile_AddRef (po);
      return SUCCESS;

   default:
      *ppo = NULL;
      return ECLASSNOTSUPPORT;
   }
}

/*==================================================================

==================================================================*/
static int IMovieFile_Open(IMovieFile * po, const char * pszFileName,
                           boolean bRecord, AEECLSID format,
                           AEEMovieFileInfo * pInfo)
{
   video_fmt_type                videoFmtSvcsFormat;
   int                           ret = SUCCESS;
   char                          pszPath [MAX_FILE_NAME];
   int                           nsize;
   int                           i;
   video_fmt_create_params_type  create;
   
   if(!pInfo)
   {
     return EFAILED;
   }
   if(!pszFileName)
   {return EFAILED;
   }
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (pInfo->cbSize < sizeof (*pInfo))
   {
      return EVERSIONNOTSUPPORT;
   }

   // Close any previously opened file.
   IMovieFile_Close (po);

   // Save movie file format.
   po->m_movieFormat = format;

   // Form fully qualified path name to file.
    nsize = MAX_FILE_NAME;
#ifdef FEATURE_MM_COMMON_DIR
    STRCPY(pszPath, "fs:/shared/");
#else
    STRCPY(pszPath, "fs:/mod/qcamcorder/");
#endif
    STRCAT(pszPath, STRCHR(STRCHR(pszFileName, '/')+1, '/')+1);

   // Convert file format argument.
   switch (format)
   {
   case AEECLSID_MEDIAMPEG4:
       videoFmtSvcsFormat = VIDEO_FMT_MP4;
       break;
   default:
       return EBADPARM;
   }

   // If record flag is set, create a new movie file for writing.
   po->m_bRecord = bRecord;
   if (bRecord)
   {
      // Open movie file for writing.
       if(SUCCESS != (ret = OEMFS_Open (pszPath, _OFM_CREATE, &po->m_fsHandle)))//| _OFM_READWRITE);
       {
         return ret;
       }

      // Allocate memory for header size array.
      po->m_nStreams = pInfo->nStreams;
      po->m_pHeaderSizes
          = MALLOC (pInfo->nStreams * sizeof (po->m_pHeaderSizes [0]));
      if(!po->m_pHeaderSizes)
      {return ENOMEMORY;
      }

      // Set up file creation parameters structure.
      MEMSET (&create, 0, sizeof (create));
      create.num_streams = pInfo->nStreams;
      create.streams = MALLOC (pInfo->nStreams * sizeof (create.streams [0]));
      if(!create.streams)
      {return ENOMEMORY;
      }
      create.movie_timescale = pInfo->nMovieTimeScale;// po->m_fileInfo->movie_timescale ;
      switch(pInfo->nBrand)
      {case MOVIEFILE_BRAND_MP4:
      create.major_brand = MP4_MAJOR_BRAND;
      create.num_compat_brands = sizeof (MP4_COMPAT_BRANDS)
          / sizeof (MP4_COMPAT_BRANDS [0]);
      create.compat_brands = &MP4_COMPAT_BRANDS [0];
      break;

      case MOVIEFILE_BRAND_AMC:
        create.major_brand = AMC_MAJOR_BRAND;
        create.num_compat_brands = sizeof (AMC_COMPAT_BRANDS)
          / sizeof (AMC_COMPAT_BRANDS [0]);
        create.compat_brands = &AMC_COMPAT_BRANDS [0];
      break;

      case MOVIEFILE_BRAND_3GP:
          create.major_brand = _3GP_MAJOR_BRAND;
      create.num_compat_brands = sizeof (_3GP_COMPAT_BRANDS)
          / sizeof (_3GP_COMPAT_BRANDS [0]);
      create.compat_brands = &_3GP_COMPAT_BRANDS [0];
      break;

      case MOVIEFILE_BRAND_3G2:
          create.major_brand = _3G2_MAJOR_BRAND;
       create.num_compat_brands = sizeof (_3G2_COMPAT_BRANDS)
          / sizeof (_3G2_COMPAT_BRANDS [0]);
       create.compat_brands = &_3G2_COMPAT_BRANDS [0];
      break;

      case MOVIEFILE_BRAND_K3G:
          create.major_brand = K3G_MAJOR_BRAND;
       create.num_compat_brands = sizeof (K3G_COMPAT_BRANDS)
           / sizeof (K3G_COMPAT_BRANDS [0]);
       create.compat_brands = &K3G_COMPAT_BRANDS [0];
      break;

      case MOVIEFILE_BRAND_SKM:
          create.major_brand = SKM_MAJOR_BRAND;
       create.num_compat_brands = sizeof (SKM_COMPAT_BRANDS)
          / sizeof (SKM_COMPAT_BRANDS [0]);
       create.compat_brands = &SKM_COMPAT_BRANDS [0];
      break;

      default:
      create.major_brand = MP4_MAJOR_BRAND;
      create.num_compat_brands = sizeof (MP4_COMPAT_BRANDS)
          / sizeof (MP4_COMPAT_BRANDS [0]);
      create.compat_brands = &MP4_COMPAT_BRANDS [0];
      break;
      }
      
      for (i = 0; i < pInfo->nStreams; ++i)
      {
          MEMSET (&create.streams [i], 0, sizeof (create.streams [i]));
          switch (pInfo->pStreams [i].format)
          {
          case AEECLSID_MPEG4_VIDEO:
              create.streams [i].type = VIDEO_FMT_STREAM_VIDEO;
              create.streams [i].subinfo.video.format
                  = VIDEO_FMT_STREAM_VIDEO_MPEG4;
              create.streams [i].subinfo.video.width
                  = pInfo->pStreams [i].nWidth;
              create.streams [i].subinfo.video.height
                  = pInfo->pStreams [i].nHeight;
              create.streams [i].inter_frames = TRUE;
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
              po->nVideoTimeScale = pInfo->pStreams[i].nTimescale;
#endif
              break;

          case AEECLSID_MEDIAQCP:
              create.streams [i].type = VIDEO_FMT_STREAM_AUDIO;
              switch (pInfo->pStreams [i].nSubFormat)
              {
              case MM_QCP_FORMAT_FIXED_FULL_13K:
                  create.streams [i].subinfo.audio.format
                      = VIDEO_FMT_STREAM_AUDIO_QCELP13K_FULL;
                  create.streams [i].sample_size
                      = VOCODER_PACKET_SIZE_13K_FULL;
                  break;

              case MM_QCP_FORMAT_FIXED_HALF_13K:
                  create.streams [i].subinfo.audio.format
                      = VIDEO_FMT_STREAM_AUDIO_QCELP13K_HALF;
                  create.streams [i].sample_size
                      = VOCODER_PACKET_SIZE_13K_HALF;
                  break;

              case MM_QCP_FORMAT_FIXED_FULL_EVRC:
                  create.streams [i].subinfo.audio.format
                      = VIDEO_FMT_STREAM_AUDIO_EVRC;
                  create.streams [i].sample_size
                      = VOCODER_PACKET_SIZE_EVRC;
                  break;

              case MM_QCP_FORMAT_FIXED_12_2_AMR:
                  create.streams [i].subinfo.audio.format
                      = VIDEO_FMT_STREAM_AUDIO_AMR;
                  create.streams [i].sample_size
                      = VOCODER_PACKET_SIZE_AMR;
                  break;
              }
              create.streams [i].sample_delta
                  = pInfo->pStreams [i].nTimescale / 50;
              create.streams [i].inter_frames = FALSE;
              break;

          case AEECLSID_MPEG4_DATA:
              create.streams [i].type = VIDEO_FMT_STREAM_DATA;
              switch (pInfo->pStreams [i].nSubFormat)
              {
              case MOVIEFILE_M4S_TYPE_BIFS:
                  create.streams [i].subinfo.data.format
                      = VIDEO_FMT_STREAM_DATA_BIFS;
                  break;

              case MOVIEFILE_M4S_TYPE_OD:
                  create.streams [i].subinfo.data.format
                      = VIDEO_FMT_STREAM_DATA_OD;
                  break;
              }
              break;

          case AEECLSID_MPEG4_AUDIO:
              create.streams [i].type = VIDEO_FMT_STREAM_AUDIO;
              create.streams [i].subinfo.audio.format
                  = VIDEO_FMT_STREAM_AUDIO_MPEG4_AAC;

          case AEECLSID_3GPP_TEXT:
              create.streams [i].type = VIDEO_FMT_STREAM_TEXT;
              create.streams [i].layer = -1;
          }
          create.streams [i].priority = 16; /* compatible with AMC */
          create.streams [i].handler = ""; /* compatible with AMC */
          create.streams [i].media_timescale = pInfo->pStreams [i].nTimescale;
          create.streams [i].interlace = i; /* no interlacing */
          create.streams [i].chunk_size = MOVIEFILE_OPTIMAL_CHUNK_SIZE;
          create.streams [i].buffer_size = MOVIEFILE_STREAM_BUF_SIZE;
          create.streams [i].max_header = pInfo->pStreams [i].dwHeaderSize;
          create.streams [i].max_samples = MOVIEFILE_MAX_SAMPLES;
          create.streams [i].max_chunks = MOVIEFILE_MAX_CHUNKS;
          create.streams [i].stsc_reset_rate = MOVIEFILE_STSC_ALGO_RESET_RATE;
          create.streams [i].frames_per_sample = 1;
          create.streams [i].width
              = (((uint32) pInfo->pStreams [i].nWidth) << 16);
          create.streams [i].height
              = (((uint32) pInfo->pStreams [i].nHeight) << 16);
          create.streams [i].origin_x
              = (((uint32) pInfo->pStreams [i].nOriginX) << 16);
          create.streams [i].origin_y
              = (((uint32) pInfo->pStreams [i].nOriginY) << 16);
          create.streams [i].language
              = ((uint16) (pInfo->pStreams [i].rgchLanguage [0] - '`') << 10)
              | ((uint16) (pInfo->pStreams [i].rgchLanguage [1] - '`') << 5)
              | (uint16) (pInfo->pStreams [i].rgchLanguage [2] - '`');
          po->m_pHeaderSizes [i] = pInfo->pStreams [i].dwHeaderSize;
      }

      // Initialize video format services.
      po->m_bStatusRcvd = FALSE;
      po->m_bReadingStream = FALSE;
      video_fmt_create (&create, IMovieFile_VideoFmtCallback, po,
                        videoFmtSvcsFormat);
      ret = IMovieFile_AwaitDone (po);
      FREE (create.streams);
      if (SUCCESS != ret)
      {
         FREE (po->m_pHeaderSizes);
         po->m_pHeaderSizes = NULL;
         OEMFS_Close (po->m_fsHandle);
         po->m_fsHandle = NULL;
         return ret;
      }
   } // if (bRecord)

   // Otherwise, open an existing movie file for reading.
   else
   {
      // Open the file itself.
      if(SUCCESS != (ret = OEMFS_Open (pszPath, _OFM_READ, &po->m_fsHandle)))//| _OFM_READWRITE);
      {
         return ret;
      }

      // Parse the file with video format services.
      po->m_bStatusRcvd = FALSE;
      po->m_bReadingStream = FALSE;
      video_fmt_open (IMovieFile_VideoFmtCallback, po, videoFmtSvcsFormat);
#ifdef FEATURE_QTV_RANDOM_ACCESS_REPOS
#error code not present
#endif /*FEATURE_QTV_RANDOM_ACCESS_REPOS*/
#ifdef FEATURE_QTV_PSEUDO_STREAM
#error code not present
#endif
      
      ret = IMovieFile_AwaitDone (po);
      if (SUCCESS != ret)
      {
         OEMFS_Close (po->m_fsHandle);
         po->m_fsHandle = NULL;
         return ret;
      }

      // Return information gathered from file parsing.
      pInfo->format = videoFmtSvcsFormat;
      pInfo->nStreams = po->m_nStreams;
      pInfo->pStreams = po->m_pStreams;
      pInfo->nMovieTimeScale = po->m_fileInfo->movie_timescale;
   }

   // Create a cache of sample information structures for use when
   // reading sample information or writing stream data.
   po->m_pSampleInfo = MALLOC (MOVIEFILE_BLOCK_SIZE
                               * sizeof (po->m_pSampleInfo [0]));
   if(!po->m_pSampleInfo)
   { return ENOMEMORY;
   }

   // Mark file as open, and return.
   po->m_bOpen = TRUE;
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int IMovieFile_ReadUserAtom(IMovieFile * po, uint16 nAtom,
                                   void * pBuffer, uint32 * pnSize)
{
   int ret = SUCCESS;
   int bytes = 0;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (nAtom >= po->m_nUserAtoms)
   {  *pnSize = 0;
      return SUCCESS;
   }

   // Save user atom size.
   *pnSize = po->m_userAtoms [nAtom].size
       + sizeof (po->m_userAtoms [nAtom].uuid);

   // If buffer was given, fill buffer with user atom UUID and data.
   if (pBuffer)
   {
      // First copy user data atom UUID.
      MEMCPY (pBuffer, po->m_userAtoms [nAtom].uuid,
              sizeof (po->m_userAtoms [nAtom].uuid));

      // Seek to position of user data atom contents in file.
      OEMFS_Seek (po->m_fsHandle, _SEEK_START,
                                 po->m_userAtoms [nAtom].pos, &ret);
     
      if (SUCCESS != ret)
      {
          return ret;
      }
      
   bytes = OEMFS_Read (po->m_fsHandle, (char *) pBuffer
                      + sizeof (po->m_userAtoms [nAtom].uuid),
                      po->m_userAtoms [nAtom].size, &ret);
   if(bytes != po->m_userAtoms [nAtom].size)
      {
          return ret;
      }

   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int IMovieFile_ReadSegmentHeader(IMovieFile * po, uint16 nStream,
                                        uint16 nSegment,
                                        void * pBuffer, uint32 * pnSize)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (nStream >= po->m_nStreamsInternal)
   {
      *pnSize = 0;
      return SUCCESS;
   }
   if (nSegment >= po->m_pStreamsInternal [nStream].nSegments)
   {
      *pnSize = 0;
      return SUCCESS;
   }

   // Save segment header size.
   *pnSize = po->m_pStreamsInternal [nStream].pSegments [nSegment].cchSegment;

   // If buffer was given, fill buffer with segment header data.
   if (pBuffer)
   {
      MEMCPY (pBuffer, po->m_pStreamsInternal [nStream]
              .pSegments [nSegment].rgchSegment, *pnSize);
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int IMovieFile_ReadStream(IMovieFile * po, uint16 nStream,
                                 uint32 nSamples, uint32 nOffset,
                                 void * pBuffer, uint32 * pnSize)
{
   int ret = SUCCESS;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (nStream >= po->m_nStreams)
   {
      return EBADPARM;
   }

   // Call video format services to read the samples.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = TRUE;
   po->m_pVideoFmtRead (nStream, VIDEO_FMT_DATA_UNIT_FRAME,
                        nOffset, nSamples, (uint8 *)pBuffer,
                        po->m_pVideoFmtContext,
                        IMovieFile_VideoFmtCallback, po);
   ret = IMovieFile_AwaitDone (po);
   if (SUCCESS != ret)
   {
      // Automatically close movie file.
      OEMFS_Close (po->m_fsHandle);
      po->m_fsHandle = NULL;
      po->m_bOpen = FALSE;
      return ret;
   }

   // Return the byte count received from video format services.
   *pnSize = po->m_nIOCount;
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int IMovieFile_GetSampleInfo(IMovieFile * po, uint16 nStream,
                                    uint32 nSamples, uint32 nOffset,
                                    AEEMovieSampleInfo * pInfo)
{
   uint32               nBlockSize;
   uint32               i;
   int                  ret = SUCCESS;
   AEEMovieSampleInfo  * pSample;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (nStream >= po->m_nStreams)
   {
      return EBADPARM;
   }
   for (i = 0, pSample = pInfo; i < nSamples; ++i)
   {
       if (pSample->cbSize < sizeof (*pSample))
       {
           return EVERSIONNOTSUPPORT;
       }
       pSample = (AEEMovieSampleInfo *)
           (((char *) pSample) + pSample->cbSize);
   }

   // Read sample information in groups to reduce the required number of calls
   // to video format services, and to take advantage of the sample
   // information structure cache we allocated when we started up.
   while (nSamples > 0)
   {
       // Determine number of samples to write out this iteration.
       nBlockSize = ((nSamples > MOVIEFILE_BLOCK_SIZE)
                     ? MOVIEFILE_BLOCK_SIZE : nSamples);

       // Call video format services to read the sample information.
       po->m_bStatusRcvd = FALSE;
       po->m_bReadingStream = TRUE;
       po->m_pVideoFmtSampleInfo (nStream, nOffset, nBlockSize,
                                  &po->m_pSampleInfo [0],
                                  po->m_pVideoFmtContext,
                                  IMovieFile_VideoFmtCallback, po);
       ret = IMovieFile_AwaitDone (po);
       if (SUCCESS != ret)
       {
           // Automatically close movie file.
           OEMFS_Close (po->m_fsHandle);
           po->m_fsHandle = NULL;
           po->m_bOpen = FALSE;
           return ret;
       }

       // For each block of sample information structures to be read, convert
       // sample information from the format used by video format services.
       for (i = 0; i < nBlockSize; ++i)
       {
           pInfo->dwBytes   = po->m_pSampleInfo [i].size;
           pInfo->dwTime    = po->m_pSampleInfo [i].time;
           pInfo->dwDelta   = po->m_pSampleInfo [i].delta;
           pInfo->dwSegment = po->m_pSampleInfo [i].sample_desc_index - 1;
           pInfo->bSync     = (po->m_pSampleInfo [i].sync ? TRUE : FALSE);
           pInfo = (AEEMovieSampleInfo *)
               (((char *) pInfo) + pInfo->cbSize);
       }

       // Advance to next block of samples.
       nOffset += nBlockSize;
       nSamples -= nBlockSize;
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int IMovieFile_WriteUserAtom(IMovieFile *po, const void * pBuffer,
                                    uint32 dwSize)
{
   int  ret = SUCCESS;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (dwSize < 16)
   {
      return EBADPARM;
   }

   // Pass user data atom along to the video format services to write to the
   // file.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtWriteUuid (pBuffer, (const char *) pBuffer + 16,
                             dwSize - 16,
                             po->m_pVideoFmtContext,
                             IMovieFile_VideoFmtCallback, po);
   ret = IMovieFile_AwaitDone (po);
   if (SUCCESS != ret)
   {
      // Automatically close movie file.
      OEMFS_Close (po->m_fsHandle);
      po->m_fsHandle = NULL;
      po->m_bOpen = FALSE;
      return ret;
   }
   return SUCCESS;
}

/*==================================================================

==================================================================*/
static int IMovieFile_WriteSegmentHeader(IMovieFile * po, uint16 nStream,
                                         const void * pBuffer,
                                         uint32 dwSize)
{
   int     ret = SUCCESS;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (nStream >= po->m_nStreams)
   {
      return EBADPARM;
   }

   // Write out stream header.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtWriteHeader (nStream, dwSize, (uint8 *)pBuffer,
                               po->m_pVideoFmtContext,
                               IMovieFile_VideoFmtCallback, po);
   ret = IMovieFile_AwaitDone (po);
   if (SUCCESS != ret)
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return ret;
   }
   return SUCCESS;
}

#ifdef FEATURE_MP4_TRANSCODER
/*****************************************************************************************
 * IMovieFile_WriteAudioFrame
 * This function is called to write an audio frame to the output file
 * ARGUMENTS:
 * po: IMovieFile object
 * pData: Structure containing the audio frame data
 *****************************************************************************************/

static boolean IMovieFile_WriteAudioFrame(IMovieFile *po, xcoder_audio_frame_done_type * pData)
{
  AEEMovieSampleInfo audioInfo;
  byte *firstPacket;


  audioInfo.cbSize = sizeof (AEEMovieSampleInfo);
  audioInfo.dwBytes = pData->numBytes; 
  audioInfo.dwTime = pData->msec*8;
  audioInfo.dwDelta = 160;          /* timescale is 8000 for audio */
  audioInfo.bSync =  0;

  if ( pData->msec <0 )
  {
   loc->pAudioHeaderContents = (byte *) MALLOC(audioInfo.dwBytes *sizeof(byte));  
   if(loc->pAudioHeaderContents == NULL)
   {
     return FALSE;
   }
   mallcs++;
    MEMCPY(loc->pAudioHeaderContents, pData->audioPacket, audioInfo.dwBytes);
    loc->audioHeaderSize = audioInfo.dwBytes;
    MSG_HIGH("Recieved Audio header",0,0,0);
    return TRUE;
  }
  else
  {
    if ( audioInfo.dwTime==0 )
    {  //copy the header first
      firstPacket = (byte*) MALLOC(loc->audioHeaderSize + audioInfo.dwBytes);
      if(firstPacket == NULL)
      { return FALSE;

      }
      mallcs++;
      MEMCPY(firstPacket, loc->pAudioHeaderContents, loc->audioHeaderSize);
      MEMCPY(firstPacket + loc->audioHeaderSize, pData->audioPacket, audioInfo.dwBytes);
      audioInfo.dwBytes += loc->audioHeaderSize;
      if ( IMOVIEFILE_WriteStream(po, 1, 1, &audioInfo, firstPacket) != SUCCESS )
      {
        MSG_HIGH("ERROR IN writing audio header to MOVIE File",0,0,0);
        FREE(firstPacket);
        firstPacket = NULL;
        mallcs--;
        FREE(loc->pAudioHeaderContents);
        loc->pAudioHeaderContents = NULL;
        mallcs--;
        return FALSE;
      }
      else
      {
        MSG_HIGH("WRITING audio header to MOVIE File SUCCESS",0,0,0);

      }
      FREE(firstPacket);
      firstPacket = NULL;
      mallcs--;
      FREE(loc->pAudioHeaderContents);
      loc->pAudioHeaderContents = NULL;
      mallcs--;
      return TRUE;
    }

    if ( IMOVIEFILE_WriteStream(po, 1, 1, &audioInfo, pData->audioPacket) != SUCCESS )
    {
      MSG_HIGH("ERROR IN writing audio to MOVIE File",0,0,0);
      return FALSE;


    }
    else
    {
      MSG_HIGH("WRITING audio to MOVIE File SUCCESS",0,0,0);
      return TRUE;

    }
  }
return FALSE;
}


/*****************************************************************************************
 * IMOVIEFILE_VOPHeaderIndex
 * This function is called to get the (VOL?) header size
 * ARGUMENTS:
 * buffer: header 
 * size: size of header
 *****************************************************************************************/

LOCAL int32 IMOVIEFILE_VOPHeaderIndex(uint8 *buffer,uint32 size)
{
  uint8 target[]={0x00,0x00,0x01,0xb6};
  uint8 target_size = 4;
  uint32 i,j;

  for ( i=0; i<size; i++ )
  {
    j=0;
    if ( buffer[i] == target[j] )
    {
      for ( j=1; j<target_size; j++ )
      {
        if ( buffer[i+j] != target[j] )
        {//if we found the string we are looking for break out
          break;
        }
      }
      if ( j == target_size )
      {//we found the header return
        return i;
      }

    }

  }
 return -1;
}


/*****************************************************************************************
 * IMovieFile_WriteVideoFrame
 * This function is called to write a video frame to the output file
 * ARGUMENTS:
 * po: IMovieFile object
 * pData: Structure containing the video frame data
 *****************************************************************************************/
static boolean IMovieFile_WriteVideoFrame(IMovieFile *po, xcoder_video_frame_done_type * pData)
{
  AEEMovieSampleInfo videoInfo;
  int32 headerSize;
  char *tmpFile= MALLOC(MAX_FILE_NAME * 4 );

  if ( loc->bTranscodeHeader )
  {//if the header has not been written write the header
    if(po == NULL){
      return FALSE;
    }
    //Get the header size
    headerSize =  IMOVIEFILE_VOPHeaderIndex(pData->videoPacket, pData->numBytes);
    if ( headerSize >0 )
    {
      loc->mfi->pStreams->dwHeaderSize = headerSize;
    }

    /* all transcoder produced videos are in millisecond unilts
    */
    //set header data from the input file where we read the data
    //for video
   loc->mfi->pStreams[0].dwLength = 1000/loc->mfi->pStreams[0].nTimescale;
    loc->mfi->pStreams[0].dwLength = 0;
    loc->mfi->pStreams[0].dwFrames = 0;
    loc->mfi->pStreams[0].dwBytes  = 0;
    loc->mfi->pStreams[0].nTimescale = 1000;
    if(loc->transcode_video_format == XCODER_QCIF_15FPS)
    {loc->mfi->pStreams[0].nHeight = 144;
     loc->mfi->pStreams[0].nWidth = 176;
    }
    else if(loc->transcode_video_format == XCODER_SUBQCIF_10FPS)
    {loc->mfi->pStreams[0].nHeight = 96;
     loc->mfi->pStreams[0].nWidth = 128;
    }
    else if(loc->transcode_video_format == XCODER_MOVIEMAIL_7_FPS)
    {loc->mfi->pStreams[0].nHeight = 80;
     loc->mfi->pStreams[0].nWidth = 96;
    }
    
    loc->mfi->format = AEECLSID_MEDIAMPEG4;
    loc->mfi->pStreams[0].format = AEECLSID_MPEG4_VIDEO;
    loc->mfi->nBrand = MOVIEFILE_BRAND_AMC;

    if ( pData->mm_audio_subtype ==  MM_QCP_FORMAT_UNKNOWN )
    {
      loc->mfi->nStreams = 1;
    }
    else
    {
      /* also need to set up the audio part
      */
      loc->mfi->pStreams[1].dwLength = 0;
      loc->mfi->pStreams[1].dwFrames = 0;
      loc->mfi->pStreams[1].dwBytes  = 0;
      loc->mfi->pStreams[1].dwLargestFrame = 1000;
      loc->mfi->pStreams[1].dwHeaderSize = pData->audioHeaderSize;
      loc->mfi->pStreams[1].format = pData->mm_audio_type;
      loc->mfi->pStreams[1].nSubFormat = pData->mm_audio_subtype;
    }
    //open the file for writing
#ifdef FEATURE_MM_COMMON_DIR
    STRCPY(tmpFile, "fs:/shared/");

#else
    STRCPY(tmpFile, "fs:/~/");
    
#endif
    STRCAT(tmpFile, STRCHR(STRCHR(IMOVIEFILE_TMP_VIDEO_FILE, '/')+1, '/')+1);   
    if(!OEMFS_Test(tmpFile)){
      OEMFS_Remove(tmpFile);
    }

    if ( IMOVIEFILE_OpenMp4Write(po, tmpFile, (loc->mfi)) != SUCCESS )
      return FALSE;
    //indicate that the header was writen

    loc->bTranscodeHeader = FALSE;
  }
  //set the frame info structure
  videoInfo.cbSize = sizeof (AEEMovieSampleInfo);
  videoInfo.dwBytes = pData->numBytes;
  videoInfo.dwTime = pData->msec;
  videoInfo.dwDelta = pData->delta;
  videoInfo.bSync =  (pData->frame_type == I_FRAME);
  FREEIF(tmpFile);	
  //stream 1 is for video
  if ( IMOVIEFILE_WriteStream(po, 0, 1, &videoInfo, pData->videoPacket) != SUCCESS )
  { MSG_HIGH("ERROR IN WRITING to MOVIE File",0,0,0);
    return FALSE;

  }
  else
  { MSG_HIGH("WRITING to MOVIE File SUCCESS",0,0,0);
    return TRUE;
  }

}

/*****************************************************************************************
 * IMovieFile_transcoder_callback
 * This function is a callback function and interfaces with the main transcoder engine
 * ARGUMENTS:
 * state: State of the transcoder
 * stat: Data specific to current state
 * handle: user defined data
 *****************************************************************************************/

static void IMovieFile_transcoder_callback
(//mp4_transcoder_state_type    state,
 //mp4_transcoder_status_type * stat,
 //mp4_transcoder_handle        handle
  xcoder_state_type    state,
 xcoder_status_type * stat,
 xcoder_handle        handle
)
{ 


  boolean bReturnStatus = TRUE;
  //IShell *shell_ptr = AEE_GetShell();

  uint8 * pFrameContents=NULL ,fRet=0;
  AEEEvent evt = 0;
  uint32 dMaxFrameAudioSize=0, dMaxFrameVideoSize=0;
  AEEDataFrameInfo *pFrameInfo = NULL;
  IMovieFile *pMe = (IMovieFile *) handle;
  //MEMSET(&xcode_cmd, 0x00, sizeof(xcode_cmd));
 MSG_HIGH("GOT the callback %d at beginning of callback",state ,0,0);
  switch(state){
  case XCODER_CB_CMD_ERROR:
  //case MP4_TRANSCODER_STATE_ERROR:
  
  //error from the transcoder, dont do any processing here,
  //just pass it up to the client
  MSG_ERROR("transcoder error return %d %d",
              stat->error.error,
              stat->error.code.qtv_error,
              0);
  loc->bTerminated = TRUE;
   evt = IMOVIEFILE_TRANSCODER_STATE_ERROR;
  break;
  case XCODER_CB_CMD_INIT_DONE:
  //  case MP4_TRANSCODER_STATE_INIT_DONE:
    //no processing required
    //copy the data from the callback ptr into local buffer

    evt = IMOVIEFILE_TRANSCODER_STATE_INIT_DONE;
  break;
  //case XCODER_CB_CMD_AUDIO_START :
    //no processing required
    //copy the data from the callback ptr into local buffer
//    evt = IMOVIEFILE_TRANSCODER_STATE_AUDIO_START;
//  break;
  case XCODER_CB_CMD_AUDIO_DONE:
  //  case MP4_TRANSCODER_STATE_AUDIO_DONE:
    //no processing required
    //copy the data from the callback ptr into local buffer
   evt = IMOVIEFILE_TRANSCODER_STATE_AUDIO_DONE;
  break;
  case XCODER_CB_CMD_VIDEO_CONT:
  //case MP4_TRANSCODER_STATE_VIDEO_CONT:
   //Display the video frame here
   //pass this event upto the client
    MSG_HIGH("GOT VIDEO_CONT CALLBACK",0,0,0);
   evt = IMOVIEFILE_TRANSCODER_STATE_VIDEO_CONT;
   
   //copy the data from the callback ptr into local buffer
   
   xcode_cmd.video.currentFrame = stat->video.currentFrame;
   xcode_cmd.video.height = stat->video.height;
   xcode_cmd.video.msec = stat->video.msec;
   xcode_cmd.video.nframes = stat->video.nframes;
   if(xcode_cmd.video.rgb)
   {FREE(xcode_cmd.video.rgb);
    xcode_cmd.video.rgb = NULL;
    mallcs--;
   }
   xcode_cmd.video.rgb = (uint8*) MALLOC(stat->video.height *stat->video.width *2);
   mallcs++;
   MEMCPY(xcode_cmd.video.rgb, stat->video.rgb, (stat->video.height *stat->video.width *2));
   xcode_cmd.video.totNframes = stat->video.totNframes;
   xcode_cmd.video.width = stat->video.width;

  break;
  //case MP4_TRANSCODER_STATE_DONE:
  case XCODER_CB_CMD_DONE:
    //no processing required
    //copy the data from the callback ptr into local buffer
    

    
    if(loc->mfi->pStreams)
   { FREE(loc->mfi->pStreams);
    loc->mfi->pStreams = NULL;
    mallcs--;
   }
    if(loc->mfi)
   {FREE(loc->mfi);
    loc->mfi = NULL;
    mallcs--;
   }
   if(loc->md->pData)
   {FREE(loc->md->pData);
    loc->md->pData = NULL;
    mallcs --;
   }
   if(loc->md)
   {FREE(loc->md);
    loc->md = NULL;
    mallcs --;
   }



    evt = IMOVIEFILE_TRANSCODER_STATE_DONE;
  break;
  //case XCODER_CB_CMD_AUDIO_FRAME_RESET:
   //no processing required
   //copy the data from the callback ptr into local buffer
   //evt= IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_RESET;
  //break;
    case XCODER_CB_CMD_TERMINATED:
      loc->bTerminated = TRUE;
      MSG_HIGH("GOT THE TERMINATE CALL BACK FROM xcoder ",0,0,0);
      evt = IMOVIEFILE_TRANSCODER_STATE_TERMINATED;
     break;
  case XCODER_CB_CMD_AUDIO_FRAME_REQUEST:
  //case MP4_TRANSCODER_STATE_AUDIO_FRAME_REQUEST:
  {//the transcoder is requesting the next audio frame

    evt = IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_REQUEST;
    dMaxFrameAudioSize =  OEMMediaMPEG42PV_GetMaxAudioFrameSize();
    pFrameInfo = (AEEDataFrameInfo*) MALLOC(sizeof(AEEDataFrameInfo));
    mallcs++;
    //get the next Audio frame
    fRet = OEMMediaMPEG42PV_GetAudioFrame(dCurrentAudioFrameNum++,pFrameInfo,
                                          stat->audio_request.audioPacket,
                                          dMaxFrameAudioSize);

    if ( !fRet )
    {// if an error occured, pass upto the client
      MSG_ERROR("GetAudioFrame Error",0,0,0);
      evt = IMOVIEFILE_TRANSCODER_STATE_ERROR;
    }
    else{
    //if getting the audio frame was a success, 
    stat->audio_request.msec        = pFrameInfo->FrameTime;
    stat->audio_request.numBytes    = pFrameInfo->FrameSize;
    }
    //free the data
    if(pFrameInfo){
     FREE(pFrameInfo);
     pFrameContents = NULL;
     mallcs--;
    }
    

  }
  break;

  //case MP4_TRANSCODER_STATE_AUDIO_FRAME_DONE:
  case XCODER_CB_CMD_AUDIO_FRAME_DONE:
    //Audio frame is done, now write it to the file
    MSG_HIGH("GOT AUDIO FRAME DONE CALLBACK",0,0,0);
    evt = IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_DONE;
    
    //write the audio frame
    if(IMovieFile_WriteAudioFrame(pMe, &(stat->audio_done)))
    { MSG_HIGH("MOVIE FILE WRITE SUCCESS FOR AUDIO FRAME",0,0,0);
    }
    else
    { MSG_HIGH("MOVIE FILE WRITE FAILED FOR AUDIO FRAME",0,0,0);
      evt = IMOVIEFILE_TRANSCODER_STATE_ERROR;
    }
  break;

  case XCODER_CB_CMD_VIDEO_FRAME_REQUEST:
  //  case MP4_TRANSCODER_STATE_VIDEO_FRAME_REQUEST:
  {//transcoder is requesting a video frame
    evt = IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_REQUEST;
    dMaxFrameVideoSize = OEMMediaMPEG42PV_GetMaxVideoFrameSize();
    pFrameInfo = (AEEDataFrameInfo*) MALLOC(sizeof(AEEDataFrameInfo));
    mallcs++;
#ifndef FEATURE_QTV_VIDEO_DISABLE
    if ( OEMMediaMPEG42PV_GetVideoFrame(dCurrentVideoFrameNum++, 
                                        pFrameInfo, 
                                        stat->video_request.videoPacket,
                                        dMaxFrameVideoSize)
       )
    { //if we got the video frame successfully, copy it to the callback ptr
      stat->video_request.numBytes    = pFrameInfo->FrameSize;
      stat->video_request.msec        = pFrameInfo->FrameTime;
      stat->video_request.duration    = pFrameInfo->Framedelta;
    }
    else
#endif /* FEATURE_QTV_VIDEO_DISABLE */
    {
      MSG_ERROR("GetVideoFrame Error",0,0,0);
      evt = IMOVIEFILE_TRANSCODER_STATE_ERROR;
    }
    if(pFrameInfo)
    { FREE(pFrameInfo);
      pFrameInfo = NULL;
      mallcs--;
    }
  }
  
  break;
  case XCODER_CB_CMD_VIDEO_FRAME_DONE:
  //case MP4_TRANSCODER_STATE_VIDEO_FRAME_DONE:
    //the transcoder is ready with another video frame, write it to output file
    evt = IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_DONE;
    MSG_HIGH("GOT VIDEO FRAME DONE CALLBACK",0,0,0);
    
    //copy the data over to the local buffer
    //Write the video Frame
    if(IMovieFile_WriteVideoFrame(pMe, &(stat->video_done)))
    { MSG_HIGH("MOVIE FILE WRITE SUCCESS FOR AUDIO FRAME",0,0,0);
    }
    else
    {MSG_HIGH("MOVIE FILE WRITE FAILED FOR AUDIO FRAME",0,0,0);
     evt = IMOVIEFILE_TRANSCODER_STATE_ERROR;
    }
  break;
  //case MP4_TRANSCODER_STATE_AUDIO_FRAME_RESET:
//    case MP4_TRANSCODER_STATE_AUDIO_FRAME_RESET:
    //evt = IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_RESET;
    //MSG_HIGH("GOT AUDIO FRAME RESET MESSAGE",0,0,0);
    //break;
  default:
     evt = IMOVIEFILE_TRANSCODER_STATE_ERROR;
   MSG_ERROR("bad callback state: %d",state,0,0);
  break;
  }
  if(evt == IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_DONE || 
      evt == IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_DONE ||
     evt == IMOVIEFILE_TRANSCODER_STATE_TERMINATED ||
     evt == IMOVIEFILE_TRANSCODER_STATE_AUDIO_FRAME_REQUEST ||
     evt == IMOVIEFILE_TRANSCODER_STATE_VIDEO_FRAME_REQUEST
     )
  { //if we got a video frame done, or audio frame done, dont pass the 
    //event up to the app
    //the app cannot handle so many events
    bReturnStatus = TRUE;
  }
  else{
   MSG_HIGH("Got the following event %d", (uint16) evt,0,0);
   bReturnStatus = ISHELL_PostEvent(loc->pIShell,  AEECLSID_QCAMCORDER, 
                                      EVT_USER,
                                       (uint16) evt,
                                    (uint32) &xcode_cmd);
  }
  if(bReturnStatus == FALSE)
  {if(evt == IMOVIEFILE_TRANSCODER_STATE_VIDEO_CONT)
   {if(xcode_cmd.video.rgb)
    {FREE(xcode_cmd.video.rgb);
     xcode_cmd.video.rgb = NULL;
     mallcs--;
    }
   }
    MSG_HIGH("MSG Dropped by OEMMovieFile.c",0,0,0);
  }


}
/* ==========================================================================
FUNCTION IMovieFile_GetFileBrand
DESCRIPTION
This function gets the filebrand, by looking at the file extension
============================================================================= */
static uint32 IMovieFile_GetFileBrand(const char* inputfile)
{ 
  char *pExt=NULL;
 
  
  pExt = STRSTR(inputfile, ".") ;
  if ( !STRCMP(pExt, IMOVIEFILE_3G_MOVIE_EXTENSION) )
  {//3g2 brand
    return MOVIEFILE_BRAND_3G2;
  }
  else if ( !STRCMP(pExt, IMOVIEFILE_K3G_MOVIE_EXTENSION) )
  {//K3g brand
    return MOVIEFILE_BRAND_K3G;
  }
  else if ( !STRCMP(pExt, IMOVIEFILE_MP4_MOVIE_EXTENSION) )
  {//MP4 brand
    return MOVIEFILE_BRAND_MP4;
  }
  else if ( !STRCMP(pExt, IMOVIEFILE_AMC_MOVIE_EXTENSION) )
  {//AMC brand
    return MOVIEFILE_BRAND_AMC;
  }
  else if ( !STRCMP(pExt, IMOVIEFILE_3GP_MOVIE_EXTENSION) )
  {//3GP brand
    return MOVIEFILE_BRAND_3GP;
  }
  else if ( !STRCMP(pExt, IMOVIEFILE_SKM_MOVIE_EXTENSION) )
  {//SKM brand
    return MOVIEFILE_BRAND_SKM;
  }
  else
  {
    return MOVIEFILE_BRAND_MP4;
  }

}

/*********************************************************************************
 * IMovieFile_StartTranscode
 * This is the routint that starts up the transcoding process
 * It gets the audio/video header frames, and then starts the transcoder
 * ARGUMENTS:
 * po: IMovieFile object
 *********************************************************************************/

static void IMovieFile_StartTranscode(IMovieFile * po)
{ 
 

  //declare, intialize locals
  AEEDataFrameInfo *pFrameInfo = NULL;
  xcoder_return_type retval = XCODER_NOERROR;
  //xcoder_return_type retval = MP4_TRANSCODER_NOERROR;
  static uint32 audioHeaderSize=0, videoHeaderSize=0, dVideoEndFrameNum=0, dAudioEndFrameNum=0;
  int32 dMaxFrameAudioSize=0, dMaxFrameVideoSize =0;
  boolean status = FALSE;
  static byte *pVideoHeaderContents = NULL, *pAudioHeaderContents = NULL;
  int32 time_of_last_frame;

  loc->bTranscoderStarted = TRUE;
  //set the default header size
  videoHeaderSize = DEFAULT_HEADER_SIZE;
  pVideoHeaderContents = (byte*) MALLOC(videoHeaderSize * sizeof(byte));
  if(!pVideoHeaderContents)
  { return;
  }
  mallcs++;
  //Get the video header
  if( OEMMediaMPEG42PV_GetVideoTrackHeader(pVideoHeaderContents, &videoHeaderSize) == FALSE )
  { //if we could not get the header, try again
    FREE(pVideoHeaderContents);
    pVideoHeaderContents = NULL;
    mallcs--;
    if( videoHeaderSize )
    { pVideoHeaderContents = (byte*) MALLOC(videoHeaderSize);
      mallcs++;
      status = OEMMediaMPEG42PV_GetVideoTrackHeader(pVideoHeaderContents, &videoHeaderSize);
    }
  }
  //set the default audio header size
  audioHeaderSize = DEFAULT_HEADER_SIZE;
  pAudioHeaderContents = (byte*) MALLOC(audioHeaderSize * sizeof(byte));
  mallcs++;
  //Get the audio header
  if( OEMMediaMPEG42PV_GetAudioTrackHeader(pAudioHeaderContents, &audioHeaderSize) == FALSE )
  {
    FREE(pAudioHeaderContents);
    pAudioHeaderContents = NULL;
    mallcs--;
    if( audioHeaderSize )
    { //if we could not get the header, try again
      pAudioHeaderContents = (byte*) MALLOC(audioHeaderSize);
      if(!pAudioHeaderContents)
      {return;
      }
      mallcs++;
      status = OEMMediaMPEG42PV_GetAudioTrackHeader(pAudioHeaderContents, &audioHeaderSize);
      if(!status)
      { MSG_HIGH("NO AUDIO",0,0,0);
      }
    }
  }
  pFrameInfo = MALLOC(sizeof(AEEDataFrameInfo));
  mallcs++;
  
  //Get the last video frame number
  time_of_last_frame = loc->dwTotalMsec;
  while( !OEMMediaMPEG42PV_GetVideoFrameInfo(time_of_last_frame, pFrameInfo) )
  { 
    /* The most of video frame rate is 15 frame per second or 67 msec
    ** per frame. So if we subtract 40 we should be pretty safe
    */
    time_of_last_frame -= 40;
    if (time_of_last_frame <=  0 )
    {
      MSG_HIGH("ERROR IN VIDEO EDITING",0,0,0);
      FREE(pFrameInfo);  
      pFrameInfo = NULL;
      mallcs--;
      return;
    }
  }
  //store the last video frame number
  dVideoEndFrameNum = pFrameInfo->FrameNum;

  //Get the last audio frame number
  time_of_last_frame = loc->dwTotalMsec;
  while( !OEMMediaMPEG42PV_GetAudioFrameInfo(time_of_last_frame, pFrameInfo) )
  {  

    /* Audio frame is 20 msec per frame. So if we subtract 15 msec we
    ** should be pretty safe
    */
    time_of_last_frame -= 15;
    if ( time_of_last_frame <= 0)
    {
      MSG_HIGH("No Audio in file",0,0,0);
      break;
    //  FREE(pFrameInfo);  
     // pFrameInfo = NULL;
      //mallcs--;
      //return;
    }
  }
  dMaxFrameVideoSize = OEMMediaMPEG42PV_GetMaxVideoFrameSize();
  dMaxFrameAudioSize = OEMMediaMPEG42PV_GetMaxAudioFrameSize();
   if(dMaxFrameAudioSize >0)
   {dAudioEndFrameNum = pFrameInfo->FrameNum;
     dAudioEndFrameNum+=1;
   }
   else
   {dAudioEndFrameNum=0;
   }
   //get the last audio frame number
  

  if( pFrameInfo )
  {
    FREE(pFrameInfo);
    mallcs--;
    pFrameInfo = NULL;
  }

  dCurrentVideoFrameNum = 0;
  dCurrentAudioFrameNum = 0;

  //Start the transcoder
  retval = xcoder 
    (
      IMovieFile_transcoder_callback,
      pVideoHeaderContents,
      videoHeaderSize,
      dVideoEndFrameNum+1,
      pAudioHeaderContents,
      audioHeaderSize,
      dAudioEndFrameNum,
      loc->transcode_audio_format,
      loc->transcode_video_format,
      dMaxFrameAudioSize,
      dMaxFrameVideoSize,
      (void*) po
    );

  if(pVideoHeaderContents)
  {FREE(pVideoHeaderContents);
  mallcs--;
   pVideoHeaderContents = NULL;
  }
  if(pAudioHeaderContents)
  { FREE(pAudioHeaderContents);
  mallcs--;
    pAudioHeaderContents = NULL;
  }
  if( retval == XCODER_NOERROR )
  //if( retval == MP4_TRANSCODER_NOERROR)
  {
    MSG_HIGH("TRANSCODER START WITH NO ERRORS",0,0,0);
    return;
  }
  else
  {//send error to client
     //IMovieFile_transcoder_callback(MP4_TRANSCODER_STATE_ERROR, NULL, po) ;
     //IMovieFile_transcoder_callback(XCODER_CB_CMD_ERROR, NULL, po) ;
    MSG_HIGH("ERROR IN TRANSCODER START",0,0,0);
    return;
  }

}

/*********************************************************************************
 * IMovieFile_PlayMediaNotify
 * This the callback registered with QTV
 * We basically care about two events:
 * The callback for total length of the file (in msecs)
 * The callback for the file open completion (MM_MP4_STATUS_OPEN_COMPLETE)
 * ARGUMENTS:
 * pUser: User specific Data
 * pCmdNotify: State specific data
 *********************************************************************************/
static void IMovieFile_PlayMediaNotify(void* pUser, AEEMediaCmdNotify* pCmdNotify)
{
 IMovieFile *pMe = (IMovieFile *) pUser;
 switch( pCmdNotify->nCmd){
 
  case MM_CMD_GETTOTALTIME:
   if ( pCmdNotify->nStatus == MM_STATUS_DONE )
    {//callback for length of file 
     loc->dwTotalMsec = ((uint32)pCmdNotify->pCmdData);
    }
  break;
  case MM_CMD_PLAY:
   if ( pCmdNotify->nStatus == MM_MP4_STATUS_OPEN_COMPLETE )
   { //file open is complete. Start transcoding process
     IMovieFile_StartTranscode(pMe);
   }
  break;
 }


}
/* ==========================================================================
FUNCTION IMovieFile_InitializeMovieFile
DESCRIPTION Opens the movie file so that frames can be extracted.
============================================================================= */
static void IMovieFile_InitializeMovieFile(IMovieFile *po)
{

  //create the media object
  AEEMediaUtil_CreateMedia(loc->pIShell, loc->md, &(loc->pMedia));


  if (loc->pMedia)
  {//Get the total length of the file
   if ( SUCCESS != IMEDIA_GetTotalTime(loc->pMedia))
    {
      MSG_HIGH("COULD NOT GET MEDIA TIME",0,0,0);
      loc->dwTotalMsec = 100;
    }
    //Register the callback
    IMEDIA_RegisterNotify(loc->pMedia, IMovieFile_PlayMediaNotify, po);
  }

}

/*==================================================================
* This function is called by Qcamcorder app to initialize the Transcoder
* ARGUMENTS 
* po: IMovieFile object
* pszInputFile: NULL terminated path of the input file
* pszOutputFile: NULL terminated path of the output fule
* target_audio_format: Audio format of the transcoded file
* target_video_format: Video format of the transcoded file
==================================================================*/
static int IMovieFile_TranscoderStart(IMovieFile * po, //output file
                                     IMovieFile * pi,
                                     const char *pszInputFile,
                                     const char *pszOutputFile,
                                     xcoder_audio_type        target_audio_format,
                                     xcoder_video_type        target_video_format,
                                     AEEMovieFileInfo *temp
                                     )
{   //AEEMovieFileInfo *temp;
    int32 strln=0;

    loc->mfi = (AEEMovieFileInfo*) MALLOC(sizeof(AEEMovieFileInfo));
    mallcs++;
    loc->mfi->cbSize =  sizeof(AEEMovieFileInfo);
    //temp = (AEEMovieFileInfo *) MALLOC(sizeof(AEEMovieFileInfo));
    mallcs++;
    //temp->cbSize = sizeof(AEEMovieFileInfo);
    
    //open the movie file to read, get the po->mfi structure filled up to be used later
    //status = IMovieFile_Open(pi, pszInputFile, FALSE, AEECLSID_MEDIAMPEG4, temp);
     //if ( status != SUCCESS )
    //return EFAILED;
        
    //Get the file brand
     loc->mfi->format = temp->format;
    loc->mfi->nBrand = IMovieFile_GetFileBrand(pszInputFile);
    loc->mfi->nMovieTimeScale = 90000;//temp->nMovieTimeScale;
    loc->mfi->nStreams = temp->nStreams;
    loc->mfi->pStreams = MALLOC(2 * sizeof( AEEMovieStreamInfo));
    mallcs++;
    loc->mfi->pStreams[0].cbSize = temp->pStreams[0].cbSize;
    loc->mfi->pStreams[0].dwBytes = temp->pStreams[0].dwBytes;
    loc->mfi->pStreams[0].dwFrames = temp->pStreams[0].dwFrames;
    loc->mfi->pStreams[0].dwHeaderSize = temp->pStreams[0].dwHeaderSize;
    loc->mfi->pStreams[0].dwLargestFrame = temp->pStreams[0].dwLargestFrame;
    loc->mfi->pStreams[0].dwLength = temp->pStreams[0].dwLength;
    loc->mfi->pStreams[0].dwRate = temp->pStreams[0].dwRate;
    loc->mfi->pStreams[0].format = temp->pStreams[0].format;
    loc->mfi->pStreams[0].nHeight = temp->pStreams[0].nHeight;
    loc->mfi->pStreams[0].nSubFormat = temp->pStreams[0].nSubFormat;
    loc->mfi->pStreams[0].nTimescale= temp->pStreams[0].nTimescale;
    loc->mfi->pStreams[0].nWidth = temp->pStreams[0].nWidth;
    
    //loc->mfi->pStreams[1] = (AEEMovieStreamInfo *) MALLOC(sizeof(AEEMovieStreamInfo));
    loc->mfi->pStreams[1].cbSize = temp->pStreams[1].cbSize;
    loc->mfi->pStreams[1].dwBytes = temp->pStreams[1].dwBytes;
    loc->mfi->pStreams[1].dwFrames = temp->pStreams[1].dwFrames;
    loc->mfi->pStreams[1].dwHeaderSize = temp->pStreams[1].dwHeaderSize;
    loc->mfi->pStreams[1].dwLargestFrame = temp->pStreams[1].dwLargestFrame;
    loc->mfi->pStreams[1].dwLength = temp->pStreams[1].dwLength;
    loc->mfi->pStreams[1].dwRate = temp->pStreams[1].dwRate;
    loc->mfi->pStreams[1].format = temp->pStreams[1].format;
    loc->mfi->pStreams[1].nHeight = temp->pStreams[1].nHeight;
    loc->mfi->pStreams[1].nSubFormat = temp->pStreams[1].nSubFormat;
    loc->mfi->pStreams[1].nTimescale= temp->pStreams[1].nTimescale;
    loc->mfi->pStreams[1].nWidth = temp->pStreams[1].nWidth;

    //This flag determines that the header has to be written

    loc->bTranscodeHeader = TRUE;
    
    //Initialize the Media Data object
    loc->md = MALLOC(sizeof(AEEMediaData));
    mallcs++;
    loc->md->clsData = MMD_FILE_NAME;
    loc->md->dwSize = 0;
    strln = STRLEN(pszInputFile);
    loc->md->pData = (char*) MALLOC(sizeof(char) * (strln+1));
    mallcs++;
    STRCPY(loc->md->pData, pszInputFile);
    //// MALLOC( STRLEN(pszInputFile+1) * sizeof(char));//STRLEN(pszOutputFile));
    //MEMSET(loc->md->pData, 0x0, STRLEN(pszInputFile+1) * sizeof(char));
    //MEMCPY(loc->md->pData, pszInputFile,STRLEN(pszInputFile) * sizeof(char));// STRLEN(pszOutputFile));
    strln = STRLEN(pszOutputFile);

    loc->pszOutputFile = (char*) MALLOC(sizeof(char) * (strln+1));
    mallcs++;
    //MEMSET(loc->pszOutputFile, 0x0, sizeof(char) * STRLEN(pszOutputFile));
    STRCPY(loc->pszOutputFile, pszOutputFile);
    //set the target audio, video format
    loc->transcode_audio_format = target_audio_format;
    loc->transcode_video_format = target_video_format;
    loc->pIShell = po->m_pIShell;
    loc->bTerminated = FALSE;
    loc->bTranscoderStarted = FALSE;
    //Initialize the Move file
    IMovieFile_InitializeMovieFile(po);
    return SUCCESS;

}
#endif
/*==================================================================

==================================================================*/
static int IMovieFile_WriteStream(IMovieFile * po, uint16 nStream,
                                  uint32 nSamples,
                                  const AEEMovieSampleInfo * pSampleInfo,
                                  const void * pBuffer)
{
   int     ret = SUCCESS;
   uint32  nBlockSize;
   uint32  nByteCount;
   uint32  i;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }
   if (nStream >= po->m_nStreams)
   {
      return EBADPARM;
   }

   // Write samples in groups to reduce the required number of calls to video
   // format services, and to take advantage of the sample information
   // structure cache we allocated when we started up.
   while (nSamples > 0)
   {
       // Determine number of samples to write out this iteration.
       nBlockSize = ((nSamples > MOVIEFILE_BLOCK_SIZE)
                     ? MOVIEFILE_BLOCK_SIZE : nSamples);

       // For each block of samples to be written, convert sample information
       // to format used by video format services.
       nByteCount = 0;
       for (i = 0; i < nBlockSize; ++i)
       {
           // Check size of input structure.
           if (pSampleInfo->cbSize < sizeof (*pSampleInfo))
           {
               return EVERSIONNOTSUPPORT;
           }

           // Copy over structure arguments.
           po->m_pSampleInfo [i].size = pSampleInfo->dwBytes;
           po->m_pSampleInfo [i].delta = pSampleInfo->dwDelta;
           po->m_pSampleInfo [i].sync = pSampleInfo->bSync;

           // For the very first sample in the stream, write out the part
           // consisting of the decoder specific information (header)
           // separately, if it is present.
           if (po->m_pHeaderSizes [nStream] > 0)
           {
               // Write out stream header.
               po->m_bStatusRcvd = FALSE;
               po->m_bReadingStream = FALSE;
               po->m_pVideoFmtWriteHeader (nStream,
                                           po->m_pHeaderSizes [nStream],
                                           (uint8 *)pBuffer, 
                                           po->m_pVideoFmtContext,
                                           IMovieFile_VideoFmtCallback, po);
               ret = IMovieFile_AwaitDone (po);
               if (SUCCESS != ret)
               {
                   // Automatically close movie file.
                   OEMFS_Close (po->m_fsHandle);
                   po->m_fsHandle = NULL;
                   po->m_bOpen = FALSE;
                   return ret;
               }

               // Adjust first sample size and buffer pointer to account for
               // header information which was extracted.
               po->m_pSampleInfo [i].size -= po->m_pHeaderSizes [nStream];
               pBuffer = ((char *) pBuffer) + po->m_pHeaderSizes [nStream];

               // Set header to zero for this stream since we're done writing
               // out the header.
               po->m_pHeaderSizes [nStream] = 0;
           }

           // Accumulate byte count.
           nByteCount += po->m_pSampleInfo [i].size;

           // Advance to next input structure.
           pSampleInfo = (AEEMovieSampleInfo *)
               (((char *) pSampleInfo) + pSampleInfo->cbSize);
       }

       // Call video format services to write the samples.
       po->m_bStatusRcvd = FALSE;
       po->m_bReadingStream = FALSE;
       po->m_pVideoFmtWrite (nStream, nBlockSize, &po->m_pSampleInfo [0],
                             (uint8 *)pBuffer, 
                             po->m_pVideoFmtContext,
                             IMovieFile_VideoFmtCallback, po);
       ret = IMovieFile_AwaitDone (po);
       if (SUCCESS != ret)
       {
           // Automatically close movie file.
           OEMFS_Close (po->m_fsHandle);
           po->m_fsHandle = NULL;
           po->m_bOpen = FALSE;
           return ret;
       }

       // Advance to next block of samples.
       pBuffer = ((char *) pBuffer) + nByteCount;
       nSamples -= nBlockSize;
   }
   return SUCCESS;
}
#ifdef FEATURE_QCAMCORDER_CUT_N_MERGE
int IMovieFile_GetVOPTime(IMovieFile *po, byte *buffer, uint32 size)
{
  uint8 target[]={0x00,0x00,0x01,0xb6};

 uint8 target_size = 4;
  uint32 i,j;
  uint32 number=0, vop_time_incr=0, mod_time_base=0;




  int32 offset =8;
  

  for ( i=0; i<size; i++ )
  {
    j=0;
    if ( buffer[i] == target[j] )
    {
      for ( j=1; j<target_size; j++ )
      {
        if ( buffer[i+j] != target[j] )
        {//if we found the string we are looking for break out
          break;
        }
      }
      if ( j == target_size )
      { // return i;
        //now lets try to find the time
        number = ((buffer[i+j]<<24) | (buffer[i+j+1]<<16) |                \
                       (buffer[i+j+2]<<8) | (buffer[i+j+3]));
        mod_time_base = (number >> (32-3)) & ((1<<1) -1);
        if(mod_time_base ==1)
        {offset +=1;
        }
        vop_time_incr =  (number >> (32-offset)) &((1 << 4) - 1);
      }
    }
  }
  po->last_time = vop_time_incr;
  return (vop_time_incr + mod_time_base *po->nVideoTimeScale);
}

static int IMovieFile_GetTimeIncRes(IMovieFile *po, byte *header, int size)
{
  uint8*  bits    = header +20;
 uint32  bitBuf  = (bits[0]<<24) | (bits[1]<<16) | (bits[2]<<8) | bits[3];
 uint32 value = (bitBuf >> (32 - 2 - 16)) & 0xFFFF;
 po->nVideoTimeScale = value;
 
 if(value <=2)
 {po->offset = value;
 }
 else if(value <=4)
 {po->offset = 3;
 }
 else if(value < 16)
 {po->offset = 4;
 }
 else if(value <32)
 {po->offset = 5;
 }
 else if(value <64)
 {po->offset = 6;
 }
 else if(value <128)
 {po->offset = 7;
 }
 else if(value <256)
 {po->offset = 8;
 }
 else if(value <512)
 {po->offset = 9;
 }
 else if(value <1024)
 {po->offset = 10;
 }
 else if(value <2048)
 {po->offset = 11;
 }
 else if(value <4096)
 {po->offset = 12;
 }
 else if(value <8192)
 {po->offset = 13;
 }
 else if(value <16384)
 {po->offset = 14;
 }
 else if(value <32768)
 {po->offset = 15;
 }
 else if(value <65536)
 {po->offset = 16;
 }
 

 
 return value;
}

static boolean IMovieFile_FrameCoded(IMovieFile *po,  byte *buffer, uint32 size)
{ uint8*  bits    = buffer;
  uint32 totalbits =0;
  boolean mod_time_base_present = FALSE;
  uint32 number =0;
  int32 mod_time_base=0;
  int32 coded_bit = 0;
  
  number = ((bits[4]<<24) | (bits[5]<<16) |                \
                       (bits[6]<<8) | (bits[7]));
  mod_time_base = (number >> (32-3)) & ((1<<1) -1);
  totalbits = 32+2;
  if(mod_time_base ==1)
  { mod_time_base_present = TRUE;
    totalbits+=1;
  }
  totalbits+=1; //0

  totalbits+=1; //marker 
  totalbits+= po->offset;
  totalbits+=1;
  totalbits+=1;
  totalbits = totalbits%8;
  if(totalbits ==2)
  {coded_bit = buffer[5] &0x40 ;
  }
  else
  {coded_bit = buffer[5] &0x20 ;
  }
  
 return (coded_bit >0);



  
  

}
static byte* IMovieFile_SetVOPTime(IMovieFile *po, int32 delta, byte *buffer, uint32 *sizes)
{
  uint8 target[]={0x00,0x00,0x01,0xb6};
  byte *newbuffer;
  byte nextbyte, *lastbyte;
  uint32 number1, number2, number3, number4;
  uint32 localpointer=0;
  //uint8[4] 
  uint8 target_size = 4;
  uint32 i,j;
  uint32 number=0, vop_time_incr=0, mod_time_base, cur_vop_time =0;
  uint32 newnumber=0;
  boolean last_is_1;
  boolean shift_everything = FALSE;
  boolean good_header = FALSE, coded= FALSE;
  uint32 size = *sizes;
  int32 diff=0;

  int32 offset =8;
  good_header = mp4_decode_process_start_code(buffer, size, 0, po->frame_num, FALSE);
  po->frame_num++;
  coded = IMovieFile_FrameCoded(po, buffer, size);
  if(!(good_header && coded))
  {MSG_HIGH("Not coded HEADER",0,0,0);
  // FREE(buffer);
   // buffer = NULL;
   //return NULL;
  }
  //else if(size <21)
  //{MSG_HIGH("GOT HERE",0,0,0);
 // }
  
  

  for ( i=0; i<size; i++ )
  {
    j=0;
    if ( buffer[i] == target[j] )
    {
      for ( j=1; j<target_size; j++ )
      {
        if ( buffer[i+j] != target[j] )
        {//if we found the string we are looking for break out
          break;
        }
      }
      if ( j == target_size )
      { // return i;
        //now lets try to find the time
        number = ((buffer[i+j]<<24) | (buffer[i+j+1]<<16) |                \
                       (buffer[i+j+2]<<8) | (buffer[i+j+3]));
        mod_time_base = (number >> (32-3)) & ((1<<1) -1);
        if(mod_time_base ==1)
        {offset +=1;
        }

        cur_vop_time =  (number >> (32-offset)) &

            ((1 << 4) - 1);
        
        vop_time_incr = po->last_time + delta;
        if((po->last_time < ((vop_time_incr %po->nVideoTimeScale) )) && (offset ==9) )
        {//move everything back by 1
            MSG_HIGH("Got here",0,0,0);
            shift_everything = TRUE;
        }
 
        if(po->original_timestamp1 <0)
        {po->original_timestamp1 = cur_vop_time;
         
        }
        else
        { 
          diff = cur_vop_time - po->original_timestamp1;
          po->original_timestamp1 = cur_vop_time;
          if (diff <0)
          {diff = diff + po->nVideoTimeScale;
          }
          //this gives us the difference in time between last and this frame
          //now we have to add this difference to whatever the current value is
          vop_time_incr = po->last_time + diff;
         if(vop_time_incr >= po->nVideoTimeScale)
         {shift_everything = FALSE;
         }
         

        }
        po->last_time = vop_time_incr % po->nVideoTimeScale;//15 ;
            lastbyte = &buffer[size-1];
         if(*lastbyte &0x01)
         {last_is_1 = TRUE;
         }
         else{ last_is_1 = FALSE;
         }
         if(last_is_1){
         newbuffer = MALLOC(size);
         }
         else
         {*sizes = *sizes +1;
             newbuffer = MALLOC(size+1);
         }
        MEMCPY(&(newbuffer[localpointer]), &(buffer[j-4]),4);
        localpointer+=4;
        nextbyte = buffer[j+1];
        if((vop_time_incr >= po->nVideoTimeScale) && mod_time_base !=1)
        {//put modulo time base in bitstream
         vop_time_incr = vop_time_incr %po->nVideoTimeScale;
         mod_time_base =1;
         //nextbyte = buffer[j+1];
         //get the first 2 bits
         number1 = buffer[j] &0xC0;
         //or it with 10, and then marker and then all 0's (1000)
         number1 = number1 |0x28;
         number1 = number1 | (vop_time_incr >>1);
         number1 = number1 &0xFF;
         number2 = buffer[j+1] >>1 &0x7F;
         number2 = number2 | (vop_time_incr << 7);
         number2 = number2 & 0xFF;
         number3 = buffer[j+2] >>1;
         number3 = number3 &0xFF;
         if(buffer[j+1] & 0x01)
         {number3 |=0x80;
         }
         number3 = number3 &0xFF;

         number4 = buffer[j+3] >>1;
         number4 = number4 & 0xFF;
         if(buffer[j+2]&0x01)
         {number4 |=0x80;
         }
         number4 = number4 &0xFF;
         newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4 <<24);
         if(!coded)
         {if(po->offset <11)
         { MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
         }
         else
          { MEMCPY(&(newbuffer[localpointer]), &newnumber, 3);
          }
          //Todo shift if needbe
          FREE(buffer);
          buffer = newbuffer;
          return buffer;
         }
         else{
         MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
         localpointer+=4;
         }
         
         j+=4;
         while(j+4 < size)
         {number1 = buffer[j] >> 1;
          if(buffer[j-1] & 0x01)
          {number1 |= 0x80 ;
           number1 = number1 &0xFF;
          }
          number2 = buffer[j+1] >>1;
          if(buffer[j] &0x01)
          {number2 |=0x80;
           number2 = number2 &0xFF;
          }
          number3 = buffer[j+2] >>1;
          if(buffer[j+1] & 0x01)
          {number3 |=0x80;
           number3 = number3 &0xFF;
          }
          number4 = buffer[j+3] >>1;
          if(buffer[j+2]&0x01)
          {number4 |=0x80;
           number4 = number4 &0xFF;
          }
          newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4<<24);
          MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
          localpointer+=4;
          j+=4;
         }

         if(j+1 <= size)
         {  number1 = buffer[j] >>1;
            number1 = number1 &0xFF;
            if(buffer[j-1] &0x01)
            {number1 |=0x80;
             number1 = number1 &0xFF;
            }
            j+=1;
         }
         if(j+1 <=size)
         {  number2 = buffer[j] >> 1;
            number2 = number2 &0xFF;
            if(buffer[j-1] &0x01)
            {number2 |=0x80;
            number2 = number2 &0xFF;
            }
           j+=1;
         }
         if(j+1 <=size)
         {  number3 = buffer[j] >> 1;
            number3 = number3 &0xFF;
            if(buffer[j-1] &0x01)
            {number3 |=0x80;
            number3 = number3 &0xFF;
            }
           j+=1;
         }
         if(j+1 <=size)
         {  number4 = buffer[j] >> 1;
            number4 = number4 &0xFF;
            if(buffer[j-1] &0x01)
            {number4 |=0x80;
             number4 = number4 &0xFF;
            }
           j+=1;
         }

         if((size %4) ==1)
         {if(!last_is_1)
          {newnumber = (number1 ) | (0x7f <<8) | (0x00 <<16) | (0x00 <<24);
           MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
          }
          else{
           newnumber =  (number1 ) | (0x00 <<8) | (0x00 <<16) | (0x00 <<24);
           MEMCPY(&(newbuffer[localpointer]), &newnumber, 1);
          }
          
         }
         if((size %4) == 2)
         {if(!last_is_1)
         {newnumber =  (number1 ) | (number2 <<8) | (0x7f <<16) | (0x00<<24);
         MEMCPY(&(newbuffer[localpointer]), &newnumber, 3);
         }
         else{
          newnumber =  (number1) | (number2 <<8) | (0x00 <<16) | (0x00 <<24);
          MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
         }
         
         }
         if((size %4) == 3)
         {
             if(!last_is_1)
             {newnumber =  (number1) | (number2 <<8) | (number3 <<16) | (0x7f <<24);
             MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
             }
             else{
              newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (0x00 <<24);
              MEMCPY(&(newbuffer[localpointer]), &newnumber, 3);
             }
         
         }
         if(size %4 == 0)
         { if(!last_is_1)
             {     newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4 <<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
                 localpointer +=4;
                 newnumber = (0x7F ) | (0x00 << 8) | (0x00 <<16) | (0x00 <<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 1);

             }
             else{
                 newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4<<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
             }
          
         }
        

         FREE(buffer);
         buffer = NULL;
         buffer = newbuffer;
         return newbuffer;
         //now we have completed last 4 bytes left

         




        }
        else
        {if (offset ==9)
         {//meaning we got mod_time_base in the beginning of frame
          //need to handle not coded
              //vop_time_incr = 1;
              if(shift_everything)
              {number1 = buffer[j] & 0xD0;
               number1 = number1 |0x10;
               number1 = (number1 | vop_time_incr)& 0xFF;
               number2 = ((buffer[j+1]<<1) | (buffer[j+2]>>7)) & 0xFF;
               number3 = ((buffer[j+2]<<1) | (buffer[j+3]>>7))& 0xFF;
               number4 = ((buffer[j+3]<<1) | (buffer[j+4] >>7))& 0xFF;
               
              }
              else{
              number1 = (buffer[j] | (vop_time_incr %po->nVideoTimeScale) >>1) & 0xFF;
              number2 = (buffer[j+1] |((vop_time_incr %po->nVideoTimeScale) << 7)) & 0xFF;
              number3 = buffer[j+2];
              number4 = buffer[j+3];
              newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4 <<24);
              if(!coded)
              {if(po->offset <12 && !mod_time_base)
               { MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
               }
               else if(po->offset <11 && mod_time_base)
               { MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
               }
               else
                { MEMCPY(&(newbuffer[localpointer]), &newnumber, 3);
                }
                FREE(buffer);
                buffer = newbuffer;
                return buffer;
               }
              }
              newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4 <<24);
              MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
              localpointer+=4;
         }
        else
        {
            number1 = buffer[j] & 0xF0; 
            number1 = number1 | vop_time_incr;
            number2 = buffer[j+1];
            number3 = buffer[j+2];
            number4 = buffer[j+3];
            newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4 <<24);
            if(!coded)
            {if(po->offset <12 && !mod_time_base)
             { MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
             }
             else if(po->offset <11 && mod_time_base)
             { MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
             }
             else
             { MEMCPY(&(newbuffer[localpointer]), &newnumber, 3);
             }
             FREE(buffer);
              buffer = newbuffer;
              return buffer;
              }
             else{
            MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
            }
            localpointer+=4;

        }
        j+=4;
        while (j+4 < size)
        {   
            if(shift_everything)
             {
              number1 = ((buffer[j]<<1  ) | (buffer[j+1]>>7))& 0xFF;
              number2 = ((buffer[j+1]<<1) | (buffer[j+2]>>7))& 0xFF;
              number3 = ((buffer[j+2]<<1) | (buffer[j+3]>>7))& 0xFF;
              number4 = ((buffer[j+3]<<1) | (buffer[j+4] >>7))& 0xFF;
              newnumber =  (number1 ) | (number2 <<8) | (number3 <<16) | (number4 <<24);
              MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
             }
            else{
             MEMCPY(&(newbuffer[localpointer]), &buffer[j], 4);
            }
            j+=4;
            localpointer+=4;
        }

        
        if((size %4) ==1)
         {
             if(shift_everything)
             {   if(buffer[j] == 0x75)
                 {//dont copy, just reduce size by one
                     //for the case where last packing byte is 0x01111111
                     //shifting it by 1 will make it 0
                  *sizes = *sizes -1;
                 }
                 else{//regular shift
                 number1 = (buffer[j] <<1 |0x01) &0xFF;
                 newnumber = number1 | (0x00<<8) | (0x00 <<16) | (0x00 <<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 1);
                }
             }
             else{
           MEMCPY(&(newbuffer[localpointer]), &buffer[j], 1);
             }
                    
         }
         if((size %4) == 2)
         {
           if(shift_everything)
             {  
                 number1 = (buffer[j] <<1 | buffer[j+1] >>7) &0xFF;
                 number2 = ((buffer[j+1] <<1) |(0x01)) &0xFF;
                 if(buffer[j+1] == 0x7F)
                 {number2 = 0x00;
                  *sizes = *sizes -1;
                 }

                 newnumber = number1 | (number2<<8) | (0x00 <<16) | (0x00 <<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 2);
                
             }
             else{//no bit shifting req'd 
          MEMCPY(&(newbuffer[localpointer]), &buffer[j], 2);
             }
          //
                  
         }
         if((size %4) == 3)
         {
             if(shift_everything)
             {  
                 number1 = (buffer[j] <<1 | buffer[j+1] >>7) & 0xFF;
                 number2 = (buffer[j+1]<<1 | buffer[j+2] >> 7) & 0xFF;
                 number3 = ((buffer[j+2] <<1) | 0x01) & 0xFF;
                 if(buffer[j+2] == 0x7F)
                 {number3 = 0x00;
                  *sizes = *sizes -1;
                 }

                 newnumber = number1 | (number2<<8) | (number3 <<16) | (0x00 <<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 3);
                
             }
             else{//no bit shifting req'd 
             MEMCPY(&(newbuffer[localpointer]), &buffer[j], 3);
             }
         }
         if(size %4 == 0)
         { if(shift_everything)
             {  
                 number1 = (buffer[j] <<1 | buffer[j+1] >>7) &0xFF;
                 number2 = (buffer[j+1]<<1 | buffer[j+2] >> 7) &0xFF;
                 number3 = (buffer[j+2] <<1 |buffer[j+3] >>7) &0xFF;
                 number4 = ((buffer[j+3] <<1) | 0x01) &0xFF;
                 if(buffer[j+3] == 0x7F)
                 {number4 = 0x00;
                  *sizes = *sizes -1;
                 }

                 newnumber = number1 | (number2<<8) | (number3 <<16) | (number4 <<24);
                 MEMCPY(&(newbuffer[localpointer]), &newnumber, 4);
                
             }
             else{//no bit shifting req'd 
             MEMCPY(&(newbuffer[localpointer]), &buffer[j], 4);
             }

         }
        FREE(buffer);
        buffer = NULL;
        buffer = newbuffer;
        return newbuffer;
      }
      
        //FREE(newbuffer);

        return NULL;

      }

    }

  }
 return NULL;


}
#endif /* FEATURE_QCAMCORDER_CUT_N_MERGE */


static int IMovieFile_TextNewSegment(IMovieFile * po, uint16 nStream,
                                     const AEEMovieTextSegment * pSegment)
{
   video_fmt_text_type  hdr;
   video_fmt_font_type  *fontList;
   uint16               i;
   int                  ret = SUCCESS;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Construct text header structure.
   hdr.scroll_in = pSegment->fScrollIn;
   hdr.scroll_out = pSegment->fScrollOut;
   switch (pSegment->dwScrollType)
   {
     case MOVIEFILE_TEXT_SCROLL_VERT_UP:
         hdr.scroll_type = VIDEO_FMT_SCROLL_VERT_UP;
         break;
     case MOVIEFILE_TEXT_SCROLL_HORZ_LEFT:
         hdr.scroll_type = VIDEO_FMT_SCROLL_HORZ_LEFT;
         break;
     case MOVIEFILE_TEXT_SCROLL_VERT_DOWN:
         hdr.scroll_type = VIDEO_FMT_SCROLL_VERT_DOWN;
         break;
     case MOVIEFILE_TEXT_SCROLL_HORZ_RIGHT:
         hdr.scroll_type = VIDEO_FMT_SCROLL_HORZ_RIGHT;
         break;
     default:
         return EBADPARM;
   }
   hdr.continuous_karaoke = pSegment->fContinuousKaraoke;
   hdr.write_vertically = pSegment->fWriteVertically;
   switch (pSegment->dwHorzJustify)
   {
     case MOVIEFILE_TEXT_JUSTIFY_LEFT_OR_TOP:
         hdr.horz_justify = VIDEO_FMT_JUSTIFY_LEFT_OR_TOP;
         break;
     case MOVIEFILE_TEXT_JUSTIFY_CENTER:
         hdr.horz_justify = VIDEO_FMT_JUSTIFY_CENTER;
         break;
     case MOVIEFILE_TEXT_JUSTIFY_RIGHT_OR_BOTTOM:
         hdr.horz_justify = VIDEO_FMT_JUSTIFY_RIGHT_OR_BOTTOM;
         break;
     default:
         return EBADPARM;
   }
   switch (pSegment->dwVertJustify)
   {
     case MOVIEFILE_TEXT_JUSTIFY_LEFT_OR_TOP:
         hdr.vert_justify = VIDEO_FMT_JUSTIFY_LEFT_OR_TOP;
         break;
     case MOVIEFILE_TEXT_JUSTIFY_CENTER:
         hdr.vert_justify = VIDEO_FMT_JUSTIFY_CENTER;
         break;
     case MOVIEFILE_TEXT_JUSTIFY_RIGHT_OR_BOTTOM:
         hdr.vert_justify = VIDEO_FMT_JUSTIFY_RIGHT_OR_BOTTOM;
         break;
     default:
         return EBADPARM;
   }
   hdr.back_color = pSegment->clrBack;
   hdr.default_box_left = pSegment->wDefaultBoxLeft;
   hdr.default_box_top = pSegment->wDefaultBoxTop;
   hdr.default_box_right = pSegment->wDefaultBoxRight;
   hdr.default_box_bottom = pSegment->wDefaultBoxBottom;
   hdr.font_id = pSegment->wFontId;
   hdr.bold = pSegment->fBold;
   hdr.italic = pSegment->fItalic;
   hdr.underline = pSegment->fUnderline;
   hdr.font_size = pSegment->nFontSize;
   hdr.text_color = pSegment->clrText;
   hdr.num_fonts = pSegment->cFonts;

   // Make a copy of the font list.
   fontList = MALLOC (pSegment->cFonts * sizeof (video_fmt_font_type));
   if (!fontList) return ENOMEMORY;
   for (i = 0; i < pSegment->cFonts; ++i)
   {
       fontList [i].font_id = pSegment->rgFonts [i].wFontId;
       fontList [i].font_name = pSegment->rgFonts [i].pszFontName;
   }
   hdr.fonts = fontList;

   // Call video format services to add text header to start new segment.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtWriteHeaderText (nStream, &hdr,
                                   po->m_pVideoFmtContext,
                                   IMovieFile_VideoFmtCallback, po);
   ret = IMovieFile_AwaitDone (po);

   // Release memory used for font list.
   FREE (fontList);

   // Check for errors.
   if (SUCCESS != ret)
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextNewSample(IMovieFile * po, const char * rgchText,
                                    uint16 cchText, void ** phSample)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to release the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   *phSample = po->m_pVideoFmtAllocText (rgchText, cchText,
                                         po->m_pVideoFmtContext,
                                         IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextDeleteSample(IMovieFile * po, void * hSample)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to release the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtFreeText (hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextSubmitSample(IMovieFile * po, void * hSample,
                                       uint16 nStream, uint32 dwDelta)
{
   int     ret = SUCCESS;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }
   if ((nStream >= po->m_nStreams)
       || (po->m_pStreams [nStream].format != AEECLSID_3GPP_TEXT))
   {
      return EBADPARM;
   }

   // Call video format services to write the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtWriteText (nStream, hSample, dwDelta,
                             po->m_pVideoFmtContext,
                             IMovieFile_VideoFmtCallback, po);
   ret = IMovieFile_AwaitDone (po);
   if (SUCCESS != ret)
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return ret;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyStyle(IMovieFile * po, void * hSample,
                                      const AEEMovieTextStyle * rgStyle,
                                      uint16 cStyle)
{
   video_fmt_text_style_type  *entries = NULL;
   uint16                     i;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Construct style entries list.
   entries = MALLOC (cStyle * sizeof (video_fmt_text_style_type));
   if (!entries) return ENOMEMORY;
   for (i = 0; i < cStyle; ++i)
   {
       entries [i].start_char = rgStyle [i].iStart;
       entries [i].end_char = rgStyle [i].iEnd;
       entries [i].font_id = rgStyle [i].wFontId;
       entries [i].bold = rgStyle [i].fBold;
       entries [i].italic = rgStyle [i].fItalic;
       entries [i].underline = rgStyle [i].fUnderline;
       entries [i].font_size = rgStyle [i].nFontSize;
       entries [i].text_color = rgStyle [i].clrText;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyStyle (cStyle, entries,
                               hSample, IMovieFile_VideoFmtCallback, po);

   // Release memory used for entries.
   FREE (entries);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyHighlight(IMovieFile * po, void * hSample,
                                          uint16 iStart, uint16 iEnd)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyHlText (iStart, iEnd,
                                hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyHighlightColor(IMovieFile * po,
                                               void * hSample,
                                               RGBVAL color)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyHlColor (color,
                                 hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyKaraoke(IMovieFile * po, void * hSample,
                                        uint32 dwStart,
                                        const AEEMovieTextKaraoke * rgKaraoke,
                                        uint16 cKaraoke)
{
   video_fmt_karaoke_box_type  *entries = NULL;
   uint16                      i;

   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Construct karaoke box entries list.
   entries = MALLOC (cKaraoke * sizeof (video_fmt_karaoke_box_type));
   if (!entries) return ENOMEMORY;
   for (i = 0; i < cKaraoke; ++i)
   {
       entries [i].end_time = rgKaraoke [i].dwEndTime;
       entries [i].start_char = rgKaraoke [i].iStart;
       entries [i].end_char = rgKaraoke [i].iEnd;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyKaraoke (dwStart, cKaraoke, entries,
                                 hSample, IMovieFile_VideoFmtCallback, po);

   // Release memory used for entries.
   FREE (entries);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyScroll(IMovieFile * po, void * hSample,
                                       uint32 dwScrollDelay)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyScroll (dwScrollDelay,
                                hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyLink(IMovieFile * po, void * hSample,
                                     uint16 iStart, uint16 iEnd,
                                     const char * rgchURL, uint8 cchURL,
                                     const char * rgchAlt, uint8 cchAlt)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyLink (iStart, iEnd, rgchURL, cchURL, rgchAlt, cchAlt,
                              hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyBox(IMovieFile * po, void * hSample,
                                    int16 nLeft, int16 nTop,
                                    int16 nRight, int16 nBottom)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyBox (nLeft, nTop, nRight, nBottom,
                             hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyBlink(IMovieFile * po, void * hSample,
                                      uint16 iStart, uint16 iEnd)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyBlink (iStart, iEnd,
                               hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

static int IMovieFile_TextModifyWrap(IMovieFile * po, void * hSample,
                                     flg fAutomatic)
{
   // Verify arguments.
   if (!po)
   {
      return EBADCLASS;
   }
   if (!po->m_bOpen || !po->m_bRecord)
   {
      return EBADSTATE;
   }

   // Call video format services to modify the text sample.
   po->m_bStatusRcvd = FALSE;
   po->m_bReadingStream = FALSE;
   po->m_pVideoFmtModifyWrap (fAutomatic,
                              hSample, IMovieFile_VideoFmtCallback, po);

   // Check for errors.
   if (po->m_bStatusRcvd && (po->m_videoFmtStatus == VIDEO_FMT_FAILURE))
   {
       // Automatically close movie file.
       OEMFS_Close (po->m_fsHandle);
       po->m_fsHandle = NULL;
       po->m_bOpen = FALSE;
       return EFAILED;
   }
   return SUCCESS;
}

#endif /* FEATURE_BREW_IMOVIEFILE */
